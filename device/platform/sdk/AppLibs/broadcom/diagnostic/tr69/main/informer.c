
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
 * File Name  : informer.c
 *
 * Description: Inform logic for tr69
 * $Revision: 1.51 $
 * $Id: informer.c,v 1.51 2006/02/17 21:22:41 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/if.h>
#include <syslog.h>
#include <fcntl.h>

#include "../inc/tr69cdefs.h"
#include "../inc/appdefs.h"
#include "../SOAPParser/RPCState.h"
#include "../SOAPParser/xmlParserSM.h"
#include "../SOAPParser/xmlTables.h"
#include "event.h"
#include "types.h"
#include "utils.h"

#include "../webproto/protocol.h"
#include "../webproto/www.h"
#include "../webproto/wget.h"
#include "../main/informer.h"
#include "../main/httpProto.h"
#include "../main/httpDownload.h"
#include "../main/httpUpload.h"
#include "acsListener.h"
#ifdef TESTBOX
#ifdef DMALLOC
#include "dmalloc.h"
#endif
#include "../bcmLibIF/bcmTestWrapper.h"
#else
#include "../bcmLibIF/bcmWrapper.h"
#endif

#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

#include "../inc/tr69cdefs.h" /* defines for ACS state */
extern ACSState    acsState;

extern TRxObjNode rootDevice[];
static void postComplete(void *handle);
static void sendNullHttp(void *handle);
static void sendInform(void* handle);
static void Connected(void *handle);
static void nullHttpTimeout(void *handle);

extern int saveNotificationsFlag;
extern int sendGETRPC;                  /* send a GetRPCMetods */
//removing extern for trellis, will look into it.
int tr69cTerm;                  /* TR69C termination flag */

static int      sendDiagComplete;       /* send Diagnostic inform */
eInformState     informState;

InformEvList    informEvList;


static int      sentACSNull;            /* set if last msg to ACS was NULL, cleared if non-null */
                                                                /* response received from ACS */
static HttpTask    httpTask;       /* http io desc */
static SessionAuth sessionAuth;
static int      notificationPending;
static int      connectionReqPending;
static int      periodicInformPending;
extern int      transferCompletePending;

static void refreshMyIPInfo(void)
{
        initACSConnectionURL();
    return;
}

static void clearInformEventList(void)
{
    informEvList.informEvCnt = 0;
        informEvList.mMethod = rpcUnknown;
}
static int getInformEvCnt(void)
{
        return informEvList.informEvCnt;
}
static int putInformEvent(eInformEvent event)
{
    if (informEvList.informEvCnt<MAXINFORMEVENTS) {
        informEvList.informEvList[informEvList.informEvCnt++] = event;
        return informEvList.informEvCnt;
    }
    return 0;
}
static void putInformEventMmethod( eRPCMethods mMethod )
{
        informEvList.mMethod = mMethod;
}


static int isConnectionReqPending(void)
{
        int ret = connectionReqPending;
        connectionReqPending = 0;
        return ret;
}

static int isAcsConnected(void)
{
        return httpTask.wio!=NULL;
}

static void closeACSConnection( HttpTask *ht)
{
        if (ht->wio){
                wget_Disconnect(ht->wio);
                ht->wio = NULL;
        }
}
static void acsDisconnect(HttpTask *ht, AcsStatus acsStatus)
{
        static  int             acsConnectFails;
        int     active = 0;
    stopTimer(nullHttpTimeout, (void *)ht);
    saveNotificationAttributes();
        /* Unlock local configuration data here */
        CfmDbUnlock();
        free(ht->authHdr);
        ht->authHdr =NULL;
        
        closeACSConnection(ht);
        ht->xfrStatus = acsStatus;
        ht->eHttpState = eClosed;
        ht->eAuthState = sIdle;
        
        switch (acsStatus) {
        case eAuthError:
                ++acsConnectFails;
                slog(LOG_INFO, "Failed authentication with ACS");
                break;
        case eConnectError:
        case eGetError:
        case ePostError:
                ++acsConnectFails;
                slog(LOG_ERR, "ACS Disconnect with error %d", acsStatus);
                #ifdef ALLOW_DISCONNECT_ERROR
        saveACSContactedState();
                saveConfigurations();
                rebootCompletion();
                factoryResetCompletion();
                #endif
                break;
        case eAcsDone:
        default:
                acsConnectFails = 0;
                #ifdef DEBUG
                slog(LOG_DEBUG, "ACS Disconnect: ok");
                #endif
                /* if no error then run thru pending disconnect actions */
                saveACSContactedState();
                saveConfigurations();
        rebootCompletion();
                factoryResetCompletion();
                break;
        }
        
        if (notificationPending){
                active = checkActiveNotifications();
                notificationPending = 0;
        }
        if (tr69cTerm) {
                #ifdef DEBUG
                slog(LOG_DEBUG, "TR69C terminated");
                #endif
                tr69cTerm = 0;                  /* Clear tr69c termination flag */
                unlink(TR69_PID_FILE);  /* Remove tr69c pid file */
                exit(0);
        }
        if (    active >0
                 || connectionReqPending                /* check for pending events */
                 || periodicInformPending
                 || sendDiagComplete
                 || sendGETRPC
                 || (acsStatus==eAuthError)
            ) {
                if (acsConnectFails==0)
                        sendInform(NULL);
                else {
                        /* retry ACS connect errors with decreasing retry time interval */
                        int backOffTime = acsConnectFails>CONN_DECAYMAX?
                                 CONN_DECAYTIME*CONN_DECAYMAX*1000: CONN_DECAYTIME*acsConnectFails*1000;
                        setTimer(sendInform, NULL, backOffTime);
                }
        }
        return;

}

static void updateAuthorizationHdr( HttpTask *ht )
{
        if (ht->authHdr){
                if ( sessionAuth.qopType==eAuth ) {
                        free(ht->authHdr);
                        ht->authHdr = generateNextAuthorizationHdrValue( &sessionAuth, acsState.acsUser, acsState.acsPwd );
                        wget_AddPostHdr(ht->wio,"Authorization", ht->authHdr);   /* replace header */
                } else
                        /* must be basic or not "Auth" */
                        wget_AddPostHdr(ht->wio,"Authorization", ht->authHdr);   /* replace header */
        }
}

static void nullHttpTimeout(void *handle)
{
    HttpTask    *ht = (HttpTask*)handle;
        DBGPRINT((stderr,"nullHttpTimeout - disconnect\n"));
        acsDisconnect(ht,ePostError);
    return;

}

static void sendNullHttp(void *handle)
{
    HttpTask    *ht = (HttpTask*)handle;
    if (ht->wio)
                DBGPRINT((stderr,"sendNullHttp(%s) to ACS\n",ht->eHttpState==eClose? "close": "keepOpen"));
        if (ht->postMsg) {
                free(ht->postMsg);
                ht->postMsg = NULL;
        }
        sendToAcs(NULL);
    sentACSNull =1;
        setTimer(nullHttpTimeout,(void *)ht, ACSRESPONSETIME/2); /* half of max */
    return;
}

#ifdef SUPPORT_ACS_CISCO
void informRspTimeout( void *handle )
{
        char    *rspBuf;

        informState = eACSInformed;
        setACSContactedState(eACS_CONTACTED);
        if (transferCompletePending) {
        if ( (rspBuf = sendTransferComplete()) )
                        sendToAcs(rspBuf);
                transferCompletePending = 0;
        } else
                sendNullHttp(handle);

    /* need this to get CISCO notifications to work.Since we never get a */
        /* InformResponse for the Cisco tool */
        initNotification();  /* update all copies of param data */
        return;
}
#endif
/*
* tWget *wg is an connected web descriptor,
 *      *mlth is pointer to location of result read length,
 *      maxBufferSize is maximum size to read if non-zero. No limit if maxSize is 0.
 * Returns:
 *     pointer to response buffer or NULL.
 *      *mlth contain size of buffer. Undefined if return is NULL.
 */     
char *readResponse( tWget *wg, int *mlth, int maxBufferSize)
{
        int             maxSize;
    char    *soapBuf = NULL;

    *mlth = 0;
    maxSize = maxBufferSize>0? maxBufferSize: MAXWEBBUFSZ;
    if (wg->hdrs->content_length>0) {
        int bufCnt = 0;
        int bufLth = wg->hdrs->content_length;
        int readSz;
        if ((soapBuf = (char *) malloc(bufLth+1))) {   /* add one for null */
            while ( bufCnt < wg->hdrs->content_length ) {
                if ( (readSz = proto_ReadWait(wg->pc, soapBuf+bufCnt, bufLth)) > 0 ){
                    bufCnt += readSz;
                    bufLth -= readSz;
                } else {
                    /* read error */
                    *mlth = 0;
                    free(soapBuf);
                    return NULL;
                }
            }
                        DBGPRINT((stderr, "soapBuf readCnt=%d ContentLth=%d\n" , bufCnt, wg->hdrs->content_length ));
            *mlth = bufCnt;
                        soapBuf[bufCnt] = '\0';
            return soapBuf;
        }
    }else if (wg->hdrs->TransferEncoding && !strcasecmp(wg->hdrs->TransferEncoding,"chunked")) {
        char cbuf[80];   
        /* read length of first chunk*/
        if (proto_Readline(wg->pc, cbuf, sizeof(cbuf))) {
            int chksz=0;
            sscanf(cbuf, "%x", (unsigned int *)&chksz);
            while (chksz) {
                /* walk thru chunks and read all chunks into the realloc buffer */
                char *newBuf;
                                if ( (*mlth+chksz) > maxSize ) {
                                        free (soapBuf);
                                        *mlth = 0;
                                        return NULL;
                                }
                if( (newBuf=realloc(soapBuf, chksz))==NULL){
                    free(soapBuf);
                    *mlth = 0;
                    return NULL;
                }
                soapBuf = newBuf;
                if ( proto_ReadWait(wg->pc, soapBuf+*mlth, chksz)!=chksz){
                    free(soapBuf);
                    *mlth = 0;
                    return NULL;
                }
                *mlth += chksz;
                /* read next chunk size */
                if (proto_Readline(wg->pc, cbuf, sizeof(cbuf))) {
                    chksz=0;
                    sscanf(cbuf, "%x", (unsigned int *)&chksz);
                } else {
                    free(soapBuf);
                    *mlth = 0;
                    return NULL;
                }
            }
            proto_Readline(wg->pc, cbuf, sizeof(cbuf));   /* flush off trailing crlf */
            proto_Skip(wg->pc);         /* skip(flush) anything else */
            return soapBuf;
        }
    }
    return NULL;
}
/*
* Data has been posted to the server or an
 * error has ocurred.
 */
static void postComplete(void *handle)
{
    tWget *wg = (tWget *)handle;
    HttpTask    *ht = (HttpTask*)wg->handle;
    SessionAuth *sa = &sessionAuth;
        int                     skipResult=1;

    DBGPRINT((stderr, "============\n"));
    DBGPRINT((stderr, "postComplete\n"));
    DBGPRINT((stderr, "============\n"));
    stopTimer(nullHttpTimeout, (void *)ht);

    if (wg->status==iWgetStatus_Ok)
    {
        if ( ht->wio->hdrs->Connection && !strcasecmp(ht->wio->hdrs->Connection,"close"))
                        ht->eHttpState = eClose;  
        DBGPRINT((stderr, "Connection = %s\n", ht->eHttpState==eClose?"close": "keep-alive"));
                if (wg->hdrs->status_code== 401) 
                {
            /* need to send authenticate */
            char *hdrvalue;
                        printf("postComplete: status code = 401\n"); 
                        if (wg->hdrs->content_length>0
                                || (wg->hdrs->TransferEncoding 
                                        && streq(wg->hdrs->TransferEncoding,"chunked"))) 
                        {
                                int     mlth;
                                char *tmpBuf;
                                if ((tmpBuf=readResponse(wg,&mlth,0)))
                                        free(tmpBuf);
                                DBGPRINT((stderr,"proto_Skip ---\n"));
                                skipResult = proto_Skip(wg->pc);
            }
                        sentACSNull = 0;
                        free(ht->authHdr); /* free in case of reauth requested during connection */
                        if ( ht->eAuthState==sAuthenticating) 
                        {
                                ht->eAuthState = sAuthFailed;
                                /* disconnect and delay */
                                acsDisconnect(ht, eAuthError);
                                return;
                        } 
                        else
                        {
                                ht->eAuthState = sAuthenticating;
            }
            DBGPRINT((stderr, "WWW-Authenticate= %s\n", wg->hdrs->wwwAuthenticate));
                        if (!(hdrvalue = generateAuthorizationHdrValue(sa, wg->hdrs->wwwAuthenticate,
                                                           "POST", ht->wio->uri, acsState.acsUser, acsState.acsPwd))) 
                        {
                slog(LOG_ERR, "WWWAuthenticate header parsing error: %s", wg->hdrs->wwwAuthenticate);
                                free(ht->postMsg); ht->postMsg = NULL;
                                acsDisconnect(ht, ePostError);
                return;
            }
                        printf("postComplete: 1\n"); 
                        ht->authHdr = hdrvalue;
                        if (skipResult==0 || ht->eHttpState==eClose ) { /* end of data on 401 skip_Proto() */
                                                                /* close connection and reconnect with Authorization header*/
                                printf("postComplete: close connection and reconnect\n"); 
                                closeACSConnection(ht);
                                ht->wio=wget_Connect(acsState.acsURL, Connected, ht);
                                if (ht->wio==NULL){
                                        slog(LOG_ERR, "ACS Connect failed: %s", wget_LastErrorMsg());
                                        free (ht->postMsg); ht->postMsg = NULL;
                                        free (ht->authHdr); ht->authHdr = NULL;
                                }
                                return;
                        } 
                        else 
                        {
                                printf("postComplete: resend last data with the Authorization header\n"); 
                                wget_AddPostHdr(ht->wio,"Authorization", ht->authHdr);
                                /* now just resend the last data with the Authorization header */
                                wget_PostData(ht->wio, ht->postMsg, ht->postLth,"text/xml", postComplete, (void*)ht);
                        }
                        printf("postComplete: 2\n"); 
                        /* now we just return to wait on the response from the server */
            #ifdef FORCE_NULL_AFTER_INFORM
                        DBGPRINT((stderr, "set Timer to Force Null send to ACS (Cisco tool)\n"));
            setTimer(informRspTimeout, ht,1*1000); /******** ?????????????CISCO TOOL ???????? send null if server doesn't respond */
            #endif
        } 
        else 
        {
            /* If status_code==200 instead of 401 after inform, and eAuthState == sIdle,
             * we're in the wrong authentication state.  The correct state should be from 
             * sAuthenticating to sAutenticated.
                         */
                        printf("postComplete: status code = 200\n"); 
            if (ht->eAuthState == sIdle) 
            {
                ht->eAuthState = sAuthFailed;
                /* disconnect and delay */
                acsDisconnect(ht, eAuthError);                
                DBGPRINT((stderr, "Error: From sIdle -> sAuthenticated w/t sAuthenticating\n"));
                return;
            }
            
                        ht->eAuthState = sAuthenticated;
            if (wg->hdrs->status_code==200
                 && ((wg->hdrs->content_length>0)
                     || (wg->hdrs->TransferEncoding && streq(wg->hdrs->TransferEncoding,"chunked")))) 
            {
                    /* allocate buffer and parse the response */
                    int     mlth;
                    char    *soapmsg;
                                         
                                        /* msg posted - free buffer */
                                        free(ht->postMsg); ht->postMsg = NULL;
                                        sentACSNull = 0;
                                        resetNotificationBuf();
                                        setACSContactedState(eACS_CONTACTED);
                    soapmsg = readResponse(wg, &mlth,0);
    
                                        printf("====================================\n"); 
                                        printf("postComplete: check for soap message\n"); 
                                        printf("====================================\n"); 
                    if (soapmsg) 
                    {
                        eParseStatus    status;
                        ParseHow    parseReq;
                        #ifdef DUMPSOAPIN
                        fprintf(stderr, "SoapInMsg=>>>>>\n%s\n<\n", soapmsg);
                        #endif
                                                printf("===================================\n"); 
                                                printf("postComplete: received soap message\n"); 
                                                printf("===================================\n\n\n"); 

                        parseReq.topLevel = envelopeDesc;
                        parseReq.nameSpace = nameSpaces;
                        status = parseGeneric(NULL,soapmsg, mlth,&parseReq);
                        free(soapmsg);
                                                if (ht->eHttpState==eClose)
                                                        closeACSConnection(ht);
                        if (status==NO_ERROR)
                        {
                                                        if ( runRPC()==eRPCRunFail )
                                                                /* couldn't run the RPC: so just disconnect */
                                                                acsDisconnect(ht, eAcsDone);
                                                } 
                                                else 
                                                {
                            slog(LOG_DEBUG, "informer: ACS Msg. Parse Error %80s", soapmsg);
                                                        acsDisconnect(ht, eAcsDone);  /* force disconnect on parse error*/
                                                }
                    } 
                    else 
                    {                
                         /* no response */
                        DBGPRINT((stderr, "informer: status = 200, no Soapmsg. sentACSNull=%d\n", sentACSNull));
                                                if (!sentACSNull)
                                                        sendNullHttp(ht);
                                                acsDisconnect(ht, eAcsDone);
                    }
            } 
            else if ( (wg->hdrs->status_code == 200 && !wg->hdrs->TransferEncoding )
                                            || wg->hdrs->status_code == 204 )
                        { 
                 /* empty ACS message -- ACS is done */
                DBGPRINT((stderr, "informer: empty ACS msg - sentACSNull=%d\n", sentACSNull ));
                                if (ht->eHttpState==eClose)
                                        closeACSConnection(ht);
                                /* msg posted - free buffer */
                                free(ht->postMsg); ht->postMsg = NULL;
                                resetNotificationBuf();
                                if (!sentACSNull)
                                {
printf("###postComplete: 1###\n");
                                        if (ht->eHttpState==eClose) 
                                        {
                                                closeACSConnection(ht);
                                                sendNullHttp(ht);
                                        } 
                                        else 
                                        {
                                                sendNullHttp(ht);
                                                acsDisconnect(ht, eAcsDone);
                                        }
                                }
                                else
                                {
                                        acsDisconnect(ht,eAcsDone);
                                }
                        } else if (wg->hdrs->status_code== 100) 
                        {
                                /* 100 Continue: Just ignore this status */
                                /* msg posted - free buffer */
                                free(ht->postMsg); ht->postMsg = NULL;
                                resetNotificationBuf();
            } 
            else if ( (wg->hdrs->status_code>=300 && wg->hdrs->status_code<=307 
                                                 && wg->hdrs->locationHdr )) 
                        {
                                /* Redirect status with new location */
                                /* repost msg to new URL */
                                char redirectURL[256];
                                strcpy(redirectURL, wg->hdrs->locationHdr);
                                closeACSConnection(ht);
                                ht->eHttpState = eStart;
                                slog(LOG_DEBUG, "Redirect to %s", redirectURL);
                ht->wio=wget_Connect(redirectURL, Connected, ht);
                if (ht->wio==NULL)
                {
                    slog(LOG_ERR, "Redirect failed: %s", wget_LastErrorMsg());
                                        acsDisconnect(ht, ePostError);
                }
            } 
            else 
            {
                                /* msg posted - free buffer */
                                free(ht->postMsg); ht->postMsg = NULL;
                                resetNotificationBuf();
                slog(LOG_ERR, "informer: Unknown status_code=%d received from ACS or encoding", wg->hdrs->status_code);
                                acsDisconnect(ht, ePostError);
            }
        }
        } 
        else 
        {
        slog(LOG_ERR, "informer: Post to ACS failed, Status = %d %s\n", wg->status, wg->msg);
        free(ht->postMsg); ht->postMsg = NULL;
                acsDisconnect(ht, ePostError);
    }
}
/*
* The connection to the ACS has been completed or
 * an error has ocurred.
 */
static void Connected(void *handle)
{
    tWget *wg = (tWget *)handle;
    HttpTask    *ht = (HttpTask*)wg->handle;
    if (wg->status != 0) {
        free(ht->postMsg); ht->postMsg = NULL;
                acsDisconnect(ht, eConnectError);
        slog(LOG_ERR, "ACS Connect Status = %d %s", wg->status, wg->msg);
        return;
    }
    if (ht->wio==NULL) {
        ht->eHttpState = eClosed;
        slog(LOG_CRIT,"Error -- pointer to IO desc is NULL");
        return;
    }
        ht->eHttpState = eConnected;
        if (ht->authHdr){
        if ( ht->eAuthState==sAuthenticated)
                        updateAuthorizationHdr(ht);
                else
                        wget_AddPostHdr(ht->wio,"Authorization", ht->authHdr);
        }
    #ifdef GENERATE_SOAPACTION_HDR
    wget_AddPostHdr(ht->wio,"SOAPAction", "");
    #endif
        #ifdef DUMPSOAPOUT
        fprintf(stderr, "sendToAcs(Connected): \n%s\n", ht->postMsg);
        #endif
        #ifdef SUPPRESS_XML_NEWLINES
        if (ht->postMsg) {/* replace \n with space to avoid cisco tool parser problems */
                {/* replace \n with space to avoid cisco tool parser problems */
                        char *sp = ht->postMsg;
                        char *dp = sp;
                        while (*sp) {
                                if (*sp != '\n')
                                        *dp++ = *sp++;
                                else
                                        ++sp; 
                        }
                        *dp = '\0';
                        ht->postLth = strlen(ht->postMsg);
                }
        }
        #endif
    wget_PostData(ht->wio, ht->postMsg, ht->postLth,
                  "text/xml", postComplete, (void *)ht);
    return;
}
/*
 * Send the current buffer to the ACS. This is an async call. The 
 * return status only indicates that the connection has started.
 * The httpTask structure represents the only connection to an
 * ACS. If the connection is up then it is reused; otherwise,
 * a new connection is attempted.
 * 
* Returs 0: oK
*       -1: Error
*/
int sendToAcs(char *buf )
{
        printf("///////////////////////////// Entered send to acs %s\n", acsState);
    HttpTask    *ht = &httpTask;
    if ( acsState.acsURL!=NULL ) 
    {
                if (ht->postMsg) 
                {
                        printf("///////////////////////////// post msg not null\n");
                        slog(LOG_ERR,"sendToAcs internal error: postMsg buffer not null");
                        free(ht->postMsg);
                }
                ht->postMsg = buf;
                ht->postLth = buf? strlen(buf): 0;
                if (ht->wio == NULL) 
                {
                        printf("///////////////////////////// ht->wio is null\n");

                        DBGPRINT((stderr, "Connect to ACS at %s\n", acsState.acsURL));
                        ht->eHttpState = eStart;
#ifdef DEBUG
                fprintf(stderr, "sendToAcs: 1\n");
#endif
                        ht->wio=wget_Connect(acsState.acsURL, Connected, ht);
#ifdef DEBUG
                fprintf(stderr, "sendToAcs: 2\n");
#endif
                        if (ht->wio==NULL)
                        {
                                DBGPRINT((stderr, "sendToAcs: ACS Connect Failed\n"));
                                slog(LOG_ERR, "ACS Connect Failed: %s", wget_LastErrorMsg());
                                free (buf); ht->postMsg = NULL;
                                return -1;
                        }
                }
                else
                {  /* already connected: use old connection */
                        printf("/////////////////////////////ht->wio is not null\n");

                        wget_ClearPostHdrs(ht->wio);
                        #ifdef GENERATE_SOAPACTION_HDR
                        wget_AddPostHdr(ht->wio,"SOAPAction", "");
                        #endif
                        #ifdef DUMPSOAPOUT
                        fprintf(stderr, "sendToAcs(existing connection): \n%s\n", 
                                        ht->postMsg? ht->postMsg:"(empty)");
                        #endif
                        #ifdef SUPPRESS_XML_NEWLINES
            if ( ht->postMsg && ht->postLth>0 )
            {
                                {/* replace \n with space to avoid cisco tool parser problems */
                                        char *sp = ht->postMsg;
                                        char *dp = sp;
                                        while (*sp) {
                                                if (*sp != '\n')
                                                        *dp++ = *sp++;
                                                else
                                                        ++sp; 
                                        }
                                        *dp = '\0';
                                        ht->postLth = strlen(ht->postMsg);
                                }
                        }
                        #endif

#ifdef DEBUG
                fprintf(stderr, "sendToAcs: 3\n");
#endif
                        updateAuthorizationHdr(ht);
                        if (ht->eHttpState == eClose)
                        {
#ifdef DEBUG
                        fprintf(stderr, "sendToAcs: 4(HttpStat==eClose)\n");
#endif
                                wget_PostDataClose(ht->wio, ht->postMsg, ht->postLth, "text/xml", postComplete,( void*) ht);
                        }
                        else
                        {
#ifdef DEBUG
                        fprintf(stderr, "sendToAcs: 5(HttpStat!=eClose)\n");
#endif
                                wget_PostData(ht->wio, ht->postMsg, ht->postLth,
                                                  "text/xml", postComplete, (void *)ht);
                        }
                }
                return 0;
    }
    return -1;
}

/*
* Send initial powerup Inform.
 * If this is first time time we have sent an Inform to
 * the current ACS URL then use EventCode is "0 BOOTSTRAP"
 * otherwise; "1 BOOT"
 */
static void sendBootXInform(eInformEvent iEvent)
{
        char    *msg;
        eACSContactedState cState = eACS_NEVERCONTACTED;
        RPCAction   *a;

        BSTD_UNUSED(iEvent);
        a = newRPCAction();
        if (informState == eJustBooted) 
        {       /* ACS has not been informed yet */
                if ( (cState=getACSContactedState()) == eACS_NEVERCONTACTED)
                {
                        printf("sendBootXInform: a\n");
                        putInformEvent(eIEBootStrap);
                }
                else 
                {
                        printf("sendBootXInform: b\n");
                        putInformEvent(eIEBoot);
                        switch(cState){
                        case eACS_DOWNLOADREBOOT:
                                printf("sendBootXInform: eACS_DOWNLOADREBOOT\n");
                                putInformEvent(eIETransferComplete);
                                putInformEvent(eIEMethodResult);
                                putInformEventMmethod (rpcDownload);
                                break;
                        case eACS_SETVALUEREBOOT:
                                printf("sendBootXInform: eACS_SETVALUEREBOOT\n");
                                putInformEvent(eIEValueChanged);
                                putInformEvent(eIEMethodResult);
                                putInformEventMmethod (rpcSetParameterValues);
                                break;
                        case eACS_ADDOBJECTREBOOT:
                                printf("sendBootXInform: eACS_ADDOBJECTREBOOT\n");
                                putInformEvent(eIEValueChanged);
                                putInformEvent(eIEMethodResult);
                                putInformEventMmethod (rpcAddObject);
                                break;
                        case eACS_DELOBJECTREBOOT:
                                printf("sendBootXInform: eACS_DELOBJECTREBOOT\n");
                                putInformEvent(eIEValueChanged);
                                putInformEvent(eIEMethodResult);
                                putInformEventMmethod (rpcDeleteObject);
                                break;
                        case eACS_RPCREBOOT:
                                printf("sendBootXInform: eACS_RPCREBOOT\n");
                                putInformEvent(eIEMethodResult);
                                putInformEventMmethod (rpcReboot);
                                break;
                        default:
                                break;
                        }
                }
        }
        if ((msg=buildInform(a, &informEvList,wanState.ip))){
                #ifdef DEBUG
                if (cState == eACS_DOWNLOADREBOOT) {
                        DBGPRINT((stderr, "Set PENDING call to sendTransferComplete\n"));
                }
                #endif
                transferCompletePending = cState == eACS_DOWNLOADREBOOT;
                sendToAcs(msg);
                /* this starts a multi-event set of actions-- so don't free msg yet*/
        }
        freeRPCAction(a);
}

void reInitInstances(void);
/* 
 * Send an Inform message. The possible handle values are
 * eIEConnectionRequest
 * eIETransferComplete
 * eIEJustBooted.
 * The other possible events are set by xxxPending flags.
 * eIEPeriodix
 * eIEDiagnostics
 * 
 */
static void sendInform(void* handle)
{
    eInformEvent iEvent = (eInformEvent)handle;
    char *msg;

        if ( isAcsConnected() ){
                connectionReqPending = connectionReqPending ||(iEvent==eIEConnectionRequest);
                return;
        }

        /* Attempt to lock the CFM database */
        if ( CfmDbLock() < 0) {
                setTimer( sendInform, handle, LOCK_CHECK_INTERVAL);
                return;
        }

        DBGPRINT((stderr, "sendInform( %d )\n", iEvent));
        /* Reinitialize instances that have been added by the
         * Web user interface or by some other non-TR69 method.
         */
        reInitInstances();

        sentACSNull = 0;
        refreshMyIPInfo();                      /* updates ip and url info */
    clearInformEventList();
        if ( getAllNotifications()>0 )
                putInformEvent(eIEValueChanged);
    if (informState==eJustBooted)
        {
                printf("sendInform: just booted\n");
        sendBootXInform(iEvent);
        }
    else 
    {
                RPCAction *a;
                a = newRPCAction();
                if ( periodicInformPending || iEvent== eIEPeriodix )
                {
                        putInformEvent(eIEPeriodix);
                        periodicInformPending =0;
                }
                /* Connection Request signal or Periodic Inform*/
                if ( isConnectionReqPending() || iEvent==eIEConnectionRequest)
                {
                        printf("sendInform: connection request\n");
                        putInformEvent( eIEConnectionRequest );
                }
                if ( sendDiagComplete )
                {
                        printf("sendInform: diag complete\n");
                        putInformEvent( eIEDiagnostics );
                        sendDiagComplete = 0;
                } 
                else if ( iEvent && (iEvent != eIEConnectionRequest) )
                {
                        printf("sendInform: etc\n");
                        putInformEvent( iEvent );
                }
                if ( getInformEvCnt()==0 )              /* force a Periodic event if none others */
                {
                        printf("sendInform: periodic\n");
                        putInformEvent( eIEPeriodix );
                } 
                if ( (msg=buildInform(a, &informEvList, wanState.ip)))
                {
                        printf("sendInform: send inform msg to acs\n");
                        sendToAcs(msg);
                        /* this starts a multi-event set of actions-- so don't free msg yet*/
                }
                freeRPCAction(a);
    }
}
/*
* Periodic Timer callback.
* if inform is enable and informInterval > 0 then always reset.
*/
static void periodicInformTimeOut(void *handle)
{
        BSTD_UNUSED(handle);
#ifdef DEBUG
        printf("Sending periodic inform\n");
#endif
        periodicInformPending = 1;
        sendInform(NULL);
        if (acsState.informEnable && acsState.informInterval)
                setTimer(periodicInformTimeOut,NULL, acsState.informInterval*1000);
}
/*
* Called from setter function to update next inform time
*/
void resetPeriodicInform(unsigned long interval)
{
        stopTimer(periodicInformTimeOut, NULL);
        setTimer(periodicInformTimeOut,NULL, interval*1000);
}
/*
* 
* This can only be called after doDownload() has already sent the Download response.
* It is called by the httpDownload functions if an error occurs.
* Thus we need to send a TransferComplete message here.
 * If the ACS is not connected then an Inform needs to be started.
*/
void sendDownloadFault()
{
        if (isAcsConnected()){
                transferCompletePending = 1;
                DBGPRINT((stderr, " acs is connected -- set transferCompletePending\n"));
        }
        else {
                DBGPRINT((stderr, " acs is not connected -- sendInform(TransferComplete)\n"));
                transferCompletePending = 1;
                sendInform( (void*)eIETransferComplete);
        }
}

/*
*
* This can only be called after doUpload() has already sent the Download response.
* It is called by the httpUpload functions if an error occurs.
* Thus we need to send a TransferComplete message here.
 * If the ACS is not connected then an Inform needs to be started.
*/
void sendUploadFault()
{
        if (isAcsConnected()){
                transferCompletePending = 1;
                DBGPRINT((stderr, " acs is connected -- set transferCompletePending\n"));
        }
        else {
                DBGPRINT((stderr, " acs is not connected -- sendInform(TransferComplete)\n"));
                transferCompletePending = 1;
                sendInform( (void*)eIETransferComplete);
        }
}


static void startACSComm(void *handle)
{
        BSTD_UNUSED(handle);
    if ( getWanState()==eWAN_ACTIVE ) {
        startACSListener();
        if (acsState.informEnable)
            sendInform( (void*) eJustBooted);
        /* WAN check timer is stopped -- */
        if (acsState.informEnable && acsState.informInterval) {
            setTimer(periodicInformTimeOut, NULL, acsState.informInterval*1000);
        }
    } else
        setTimer(startACSComm,NULL,CHECKWANINTERVAL);
        /* keep checking until wanup */
}

/* called once when tr69c starts - must be called AFTER all instances are initialized */
void initInformer(void)
{
    informState = eJustBooted;
    /* start trying ACSComm in a moment */
    setTimer(startACSComm,NULL, ACSINFORMDELAY);
        initNotification();   /* this creates all the InstanceDope descriptors for current */
                                                  /* instancesf or all parameters */
        restoreNotificationAttr();
        initNotification();   /* this applies all the notification attributes. Retrieve a */
                                                  /* current copy of the parameter values for any notify params*/
    setCallback(&informState,sendInform, (void *)eIEConnectionRequest );
                                            /* flag to indcate signal from conn Req*/ 

}

/*
* When we get kicked set a timer to allow the cfm to possibbly complete
 * its update of the configuration data. When timer expires
 * then check if we are connected to ACS. 
* If yes then wait until the current set of transactions completes
* otherwise; sendInform
*/
static void cfmDelayTimeOut( void *handle )
{
        int             notifies;
        DBGPRINT((stderr, "cfmDelayTimeOut\n"));

        /* Attempt to lock CFM configuration data before checking for notification */
        stopTimer(cfmDelayTimeOut,handle);
        if ( CfmDbLock()< 0 ) {
                setTimer(cfmDelayTimeOut, handle, LOCK_CHECK_INTERVAL);
                return;
        }
        reInitInstances();
        saveNotificationsFlag = 1;
        if ( ((notifies=checkActiveNotifications())>0)
                 || sendGETRPC>0 || sendDiagComplete>0 ) {
                CfmDbUnlock();
                DBGPRINT((stderr, "Active notifications =%d\n",notifies)); 
                if ( isAcsConnected())
                        notificationPending++;
                else
                        sendInform(NULL);
        } else
                CfmDbUnlock();
        /* no active Notifications- do nothing */
        return;
}

extern TRX_STATUS setMSrvrInformEnable(const char *value);
static void cfmKickListener(void *handle)
{
        int fd = (int)handle;
        eCFMMsg buf;

        if ( read(fd, (void *)&buf, sizeof(buf))>0 ) {
                DBGPRINT((stderr, "cfmKickListener called msg=%0x\n", buf));
                if ( buf == SEND_GETRPC)
                        sendGETRPC = 1;
                else if ( buf == SEND_ENABLE_INFORM ) {
                        setMSrvrInformEnable("1");
                        resetPeriodicInform(acsState.informInterval);                        
                } else if ( buf == SEND_DISABLE_INFORM ) {
                        setMSrvrInformEnable("0");
                        stopTimer(periodicInformTimeOut, NULL);                        
                } else if ( buf == SEND_DIAG_COMPLETE ) {
                        sendDiagComplete = 1;
                }
                /* other option is PARAM_CHANGE which we always check */
                if ( buf != SEND_ENABLE_INFORM && buf != SEND_DISABLE_INFORM ) {
                        stopTimer(cfmDelayTimeOut, NULL);       /* restart timer on each cfm msg */
                        setTimer(cfmDelayTimeOut,NULL, CFMKICKDELAY );
                }                      
        }
        return;
}

/* */
static int udp_listen(unsigned int ip, int port, char *inf, int broadcast)
{
  struct ifreq interface;
  int fd;
  struct sockaddr_in addr;
  int n = 1;


#ifdef DEBUG
  slog(LOG_DEBUG,"Opening listen socket on 0x%08x:%d %s\n", ip, port, 
      inf == NULL ? "all ifcs" : inf);
#endif
  if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    return -1;
  }
  
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(ip);

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &n, sizeof(n)) == -1) {
    close(fd);
    return -1;
  }
  if (broadcast &&
      setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char *) &n, sizeof(n)) == -1) {
    close(fd);
    return -1;
  }
  
  if (inf != NULL) {
    strncpy(interface.ifr_ifrn.ifrn_name, inf, IFNAMSIZ);
    if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE,
                   (char *)&interface, sizeof(interface)) < 0) {
      close(fd);
      return -1;
    }
  }
  
  if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
    close(fd);
    return -1;
  }
  return fd;
}

void initCFMListener(void)
{
    int fd;
    if ((fd = udp_listen(INADDR_ANY, CFMLISTENPORT, NULL, FALSE)) < 0) {
         slog(LOG_ERR, "could not initiate UDP server socket (port=%d)", CFMLISTENPORT);
         return;
    }
        setListener( fd, cfmKickListener, (void *)fd );
}

