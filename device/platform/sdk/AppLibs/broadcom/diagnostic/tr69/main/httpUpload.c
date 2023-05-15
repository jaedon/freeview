/*----------------------------------------------------------------------*
<:copyright-broadcom 
 
 Copyright (c) 2005 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
 *----------------------------------------------------------------------*
 * File Name  : httpUpload.c
 *
 * Description: upload functions 
 * $Revision: 1.0 $
 * $Id: httpUpload.c,v 1.16 2012/10/15 21:15:09 dakshr Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>

#ifdef TESTBOX
#include "../bcmLibIF/bcmTestWrapper.h"
#else
#include "syscall.h"
/*#include "ifcdefs.h" */
#include "../bcmLibIF/bcmWrapper.h"
#endif

#include "../inc/appdefs.h"
#include "../SOAPParser/CPEframework.h"
#include "../SOAPParser/RPCState.h"
#include "event.h"
#include "types.h"
#include "utils.h"

#include "../webproto/protocol.h"
#include "../webproto/www.h"
#include "../webproto/wget.h"
#include "../main/informer.h"
#include "../main/httpProto.h"
#include "../SOAPParser/xmlParserSM.h"
#include "../SOAPParser/xmlTables.h"
#include "../SOAPParser/RPCState.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif

/*#define DEBUG */
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

/* !!!JK */
#define SYS_CMD_LEN             256
#define IFC_SMALL_LEN   128

#include "../inc/tr69cdefs.h" /* defines for ACS state */
extern ACSState    acsState;

static void upLoadConnected( void *handle);
static SessionAuth getSessionAuth;
static HttpTask    httpTask;
extern RPCAction   *uloadAction;  /* not very good data hiding here */
                                    /* this is defined in RPCState.c */

//static void bcmTr69KillAllApps(int conn_fd);


void updateUpLoadKey(UploadReq *dlr)
{
        if (acsState.uploadCommandKey)
                free(acsState.uploadCommandKey);
        acsState.uploadCommandKey = dlr->commandKey;
        dlr->commandKey = NULL; 
}

static void getComplete( void *handle)
{
    tWget *wg = (tWget *)handle;
    HttpTask    *ht = (HttpTask*)wg->handle;
    SessionAuth *sa = &getSessionAuth;
    UploadReq *ulreq = &uloadAction->ud.uploadReq;
    int skipResult = 1;
    #ifdef DEBUG
    DBGPRINT((stderr, "httpUpload:getComplete() starting upload\n"));
    #endif
    
    
    if (wg->status==iWgetStatus_Ok){
        if (wg->hdrs->status_code== 401) {
            int closeConn = ht->wio->hdrs->Connection && !strcasecmp(ht->wio->hdrs->Connection,"close");
                        if (wg->hdrs->status_code== 401) {
                                /* need to send authenticate */
                                char *hdrvalue;
                                if (wg->hdrs->content_length>0) {
                                        int     mlth;
                                        char *tmpBuf;
                                        if ((tmpBuf=readResponse(wg,&mlth,0)))
                                                free(tmpBuf);
                                        DBGPRINT((stderr,"proto_Skip ---\n"));
                                        skipResult = proto_Skip(wg->pc);
                                }
                                #ifdef DEBUG
                                DBGPRINT((stderr, "WWW-Authenticate= %s\n", wg->hdrs->wwwAuthenticate));
                                #endif
                                if (!(hdrvalue = generateAuthorizationHdrValue(sa, wg->hdrs->wwwAuthenticate,
                                                                   "GET", ht->wio->uri, ulreq->user, ulreq->pwd))) {
                                        slog(LOG_ERR, "WWWAuthenticate header parsing error: %s", wg->hdrs->wwwAuthenticate);
                                        wget_Disconnect(ht->wio);
                                        ht->wio = NULL;
                                        ht->xfrStatus = eConnectError;
                                        acsState.ulFaultStatus = 9012; /* upload failed */
                                        acsState.ulFaultMsg = "Upload server- unknow authentication header format";
                                        return;
                                }

                                if (closeConn) { /* end of data on 401 skip_Proto() */
                                        /* close connection and reconnect with Authorization header*/
                                        ht->authHdr = hdrvalue;
                                        wget_Disconnect(ht->wio);
                                        ht->wio=wget_Connect(ulreq->url, upLoadConnected, ht);
                                        if (ht->wio==NULL){
                                                slog(LOG_ERR, "upload reconnect failed: %s", wget_LastErrorMsg());
                                                free (ht->authHdr); ht->authHdr = NULL;
                                                ht->wio = NULL;
                                                ht->xfrStatus = eConnectError;
                                                acsState.ulFaultStatus = 9012; /* upload failed */
                                                acsState.ulFaultMsg = "Internal error";
                                        }
                                        return;
                                } else {
                                        wget_AddPostHdr(ht->wio,"Authorization", hdrvalue);
                                        free(hdrvalue);
                                        /* now just resend the last data with the Authorization header */
                                        wget_GetData(ht->wio, getComplete, (void *)ht );
                                        /* now we just return to wait on the response from the server */
                                        return;
                                }
                        } else {
                                /* authentication failed */
                                slog(LOG_ERR, "upload: Server Authenticaion Failed %d", wg->hdrs->status_code );
                                wget_Disconnect(ht->wio);
                                ht->wio = NULL;
                                ht->xfrStatus = eConnectError;
                                acsState.ulFaultStatus = 9012; /* upload failed */
                                acsState.ulFaultMsg = "Upload server authentication failure";
                        }
        } else if (wg->hdrs->status_code==200 && ((wg->hdrs->content_length>0)
                     || (wg->hdrs->TransferEncoding && streq(wg->hdrs->TransferEncoding,"chunked")))) 
        {
                                /* readin upload image */
                                int     mlth;
                                char    *rambuf = readResponse(wg, &mlth, getRAMSize());  

                                #ifdef DEBUG
                                DBGPRINT((stderr, "Upload image size = %d\n", mlth));
                                #endif
                                wget_Disconnect(ht->wio);
                                acsState.endULTime = time(NULL);
                                ht->wio = NULL;
                                ht->xfrStatus = eUploadDone;
                                if ( rambuf && mlth ){
                                        ulreq->fileSize = mlth;  /**/
                                        uploadComplete( ulreq, rambuf );
                                        /* no return from the uploadComplete if successful */
                                } else {
                                        slog(LOG_ERR, "upload from %s failed", ulreq->url);
                                        acsState.ulFaultStatus = 9011; /* upload failed */
                                        acsState.ulFaultMsg = "Upload failed";
                                        free(rambuf);
                                }

        }else if (wg->hdrs->status_code>=100 && wg->hdrs->status_code<200) {
                return; /* ignore these status codes */
        }
        else {
                slog(LOG_ERR, "upload: Error %d", wg->hdrs->status_code );
                wget_Disconnect(ht->wio);
                ht->wio = NULL;
                ht->xfrStatus = eConnectError;
                acsState.ulFaultStatus = 9013; /* upload failed */
                acsState.ulFaultMsg = "Image file not found";
        }
    } else {
        /* if control falls thru to here send a fault status to ACS */
        slog(LOG_ERR, "upload: GET from upload server failed, Status = %d %s\n", wg->status, wg->msg);
        wget_Disconnect(ht->wio);
        ht->wio = NULL;
        ht->xfrStatus = eConnectError;
        acsState.ulFaultStatus = 9011; /* upload failed */
        acsState.ulFaultMsg = "Unable to connect to upload URL";
    }
    acsState.endULTime = time(NULL);
    sendUploadFault();    /* callback contain rpcAction pointer*/
    return;
}


static void upLoadConnected( void *handle)
{
    tWget *wg = (tWget *)handle;
    HttpTask    *ht = (HttpTask *)wg->handle;
    tWgetInternal *req = ht->wio;
   
    if (wg->status != 0) {
        wget_Disconnect(ht->wio);
        ht->wio = NULL;
        slog(LOG_ERR, "Upload Connect Status = %d %s", wg->status, wg->msg);
        ht->xfrStatus = eConnectError;
        acsState.ulFaultStatus = 9011; /* upload failed */
        acsState.ulFaultMsg = "Unable to connect to upload server";
        acsState.endULTime = time(NULL);
        sendUploadFault();
        return;
    }
    wget_ClearPostHdrs(ht->wio);
    if (ht->authHdr){
        wget_AddPostHdr(ht->wio,"Authorization", ht->authHdr);
        free(ht->authHdr); /* only use once*/
        ht->authHdr = NULL;
    }
    char buf[1024];
    sprintf(buf, "http://%s:%s/",
                req->host, req->port);
    wget_PutFile(buf, req->postdata, req->datalen, req->uri , getComplete,(void *) ht);
    acsState.fault = 0;
    return;
}

/* this is called by the callback from the startTimer in doUpload. */
/* we have to use wget_Connect in case of authentication in which case */
/* we need to control the connection */
void startUpload( void *handle) {
    RPCAction   *a = (RPCAction *)handle;
    HttpTask    *ht = &httpTask;
    memset(ht, 0, sizeof(struct HttpTask));
    memset(&getSessionAuth,0, sizeof(struct SessionAuth));
        if ( strstr(a->ud.uploadReq.url, "http:")!=NULL
                 || strstr(a->ud.uploadReq.url, "https:")!=NULL) { /* simple test for http or https */
                ht->callback = (void *)a; /* save action pointer in callback cell */
                ht->wio=wget_Connect(a->ud.uploadReq.url, upLoadConnected, ht); 
                if (ht->wio==NULL){
                        slog(LOG_ERR, "wget_Connect to upload server failed: %s", wget_LastErrorMsg());
                        acsState.ulFaultStatus = 9002; /* internal error */
                        sendUploadFault();
                } else
                        acsState.startULTime = time(NULL);
        } else {
                acsState.ulFaultStatus = 9013;
                acsState.ulFaultMsg = "Protocol not supported";
                sendUploadFault();
        }
    return;
}
