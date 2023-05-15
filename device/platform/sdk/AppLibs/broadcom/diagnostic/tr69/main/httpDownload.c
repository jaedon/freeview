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
 * File Name  : httpDownload.c
 *
 * Description: download functions 
 * $Revision: 1.16 $
 * $Id: httpDownload.c,v 1.16 2006/02/17 21:15:09 dmounday Exp $
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

/*#define DEBUG	*/
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

/* !!!JK */
#define SYS_CMD_LEN		256
#define IFC_SMALL_LEN	128

#include "../inc/tr69cdefs.h" /* defines for ACS state */
extern ACSState    acsState;

static void downLoadConnected( void *handle);
static SessionAuth getSessionAuth;
static HttpTask    httpTask;
extern RPCAction   *dloadAction;  /* not very good data hiding here */
                                    /* this is defined in RPCState.c */

static void bcmTr69KillAllApps(int conn_fd);


void updateDownLoadKey(DownloadReq *dlr)
{
	if (acsState.downloadCommandKey)
		free(acsState.downloadCommandKey);
	acsState.downloadCommandKey = dlr->commandKey;
	dlr->commandKey = NULL; 
}

static void getComplete( void *handle)
{
    tWget *wg = (tWget *)handle;
    HttpTask    *ht = (HttpTask*)wg->handle;
    SessionAuth *sa = &getSessionAuth;
    DownloadReq *dlreq = &dloadAction->ud.downloadReq;
    int	skipResult = 1;
    #ifdef DEBUG
    DBGPRINT((stderr, "httpDownload:getComplete() starting download\n"));
    #endif
    
#ifndef TESTBOX
    bcmTr69KillAllApps(wg->pc->fd);
    printf("Done removing processes\n");
#endif
    
    if (wg->status==iWgetStatus_Ok){
        if (wg->hdrs->status_code== 401) {
            int closeConn = ht->wio->hdrs->Connection && !strcasecmp(ht->wio->hdrs->Connection,"close");
			if (wg->hdrs->status_code== 401) {
				/* need to send authenticate */
				char *hdrvalue;
				if (wg->hdrs->content_length>0) {
					int	mlth;
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
								   "GET", ht->wio->uri, dlreq->user, dlreq->pwd))) {
					slog(LOG_ERR, "WWWAuthenticate header parsing error: %s", wg->hdrs->wwwAuthenticate);
					wget_Disconnect(ht->wio);
					ht->wio = NULL;
					ht->xfrStatus = eConnectError;
					acsState.dlFaultStatus = 9012; /* download failed */
					acsState.dlFaultMsg = "Download server- unknow authentication header format";
					return;
				}

				if (closeConn) { /* end of data on 401 skip_Proto() */
					/* close connection and reconnect with Authorization header*/
					ht->authHdr = hdrvalue;
					wget_Disconnect(ht->wio);
					ht->wio=wget_Connect(dlreq->url, downLoadConnected, ht);
					if (ht->wio==NULL){
						slog(LOG_ERR, "download reconnect failed: %s", wget_LastErrorMsg());
						free (ht->authHdr); ht->authHdr = NULL;
						ht->wio = NULL;
						ht->xfrStatus = eConnectError;
						acsState.dlFaultStatus = 9012; /* download failed */
						acsState.dlFaultMsg = "Internal error";
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
				slog(LOG_ERR, "download: Server Authenticaion Failed %d", wg->hdrs->status_code );
				wget_Disconnect(ht->wio);
				ht->wio = NULL;
				ht->xfrStatus = eConnectError;
				acsState.dlFaultStatus = 9012; /* download failed */
				acsState.dlFaultMsg = "Download server authentication failure";
			}
        } else if (wg->hdrs->status_code==200 && ((wg->hdrs->content_length>0)
                     || (wg->hdrs->TransferEncoding && streq(wg->hdrs->TransferEncoding,"chunked")))) 
        {
				/* readin download image */
				int     mlth;
				char    *rambuf = readResponse(wg, &mlth, getRAMSize());  

				#ifdef DEBUG
				DBGPRINT((stderr, "Download image size = %d\n", mlth));
				#endif
				wget_Disconnect(ht->wio);
				acsState.endDLTime = time(NULL);
				ht->wio = NULL;
				ht->xfrStatus = eDownloadDone;
				if ( rambuf && mlth ){
					dlreq->fileSize = mlth;  /**/
					downloadComplete( dlreq, rambuf );
					/* no return from the downloadComplete if successful */
				} else {
					slog(LOG_ERR, "download from %s failed", dlreq->url);
					acsState.dlFaultStatus = 9010; /* download failed */
					acsState.dlFaultMsg = "Download failed";
					free(rambuf);
				}

	}else if (wg->hdrs->status_code>=100 && wg->hdrs->status_code<200) {
		return; /* ignore these status codes */
	}
	else {
		slog(LOG_ERR, "download: Error %d", wg->hdrs->status_code );
		wget_Disconnect(ht->wio);
		ht->wio = NULL;
		ht->xfrStatus = eConnectError;
		acsState.dlFaultStatus = 9013; /* download failed */
		acsState.dlFaultMsg = "Image file not found";
	}
    } else {
	/* if control falls thru to here send a fault status to ACS */
	slog(LOG_ERR, "download: GET from download server failed, Status = %d %s\n", wg->status, wg->msg);
	wget_Disconnect(ht->wio);
	ht->wio = NULL;
	ht->xfrStatus = eConnectError;
	acsState.dlFaultStatus = 9010; /* download failed */
	acsState.dlFaultMsg = "Unable to connect to download URL";
    }
    acsState.endDLTime = time(NULL);
    sendDownloadFault();    /* callback contain rpcAction pointer*/
    return;
}


static void downLoadConnected( void *handle)
{
    tWget *wg = (tWget *)handle;
    HttpTask    *ht = (HttpTask *)wg->handle;
    if (wg->status != 0) {
        wget_Disconnect(ht->wio);
        ht->wio = NULL;
        slog(LOG_ERR, "Download Connect Status = %d %s", wg->status, wg->msg);
        ht->xfrStatus = eConnectError;
        acsState.dlFaultStatus = 9010; /* download failed */
        acsState.dlFaultMsg = "Unable to connect to download server";
        acsState.endDLTime = time(NULL);
        sendDownloadFault();
        return;
    }
    wget_ClearPostHdrs(ht->wio);
    if (ht->authHdr){
        wget_AddPostHdr(ht->wio,"Authorization", ht->authHdr);
        free(ht->authHdr); /* only use once*/
        ht->authHdr = NULL;
    }
    wget_GetData( ht->wio, getComplete,(void *) ht);
    acsState.fault = 0;
    return;
}

/* this is called by the callback from the startTimer in doDownload. */
/* we have to use wget_Connect in case of authentication in which case */
/* we need to control the connection */
void startDownload( void *handle) {
    RPCAction   *a = (RPCAction *)handle;
    HttpTask    *ht = &httpTask;
    memset(ht, 0, sizeof(struct HttpTask));
    memset(&getSessionAuth,0, sizeof(struct SessionAuth));
	if ( strstr(a->ud.downloadReq.url, "http:")!=NULL
		 || strstr(a->ud.downloadReq.url, "https:")!=NULL) { /* simple test for http or https */
		ht->callback = (void *)a; /* save action pointer in callback cell */
		ht->wio=wget_Connect(a->ud.downloadReq.url, downLoadConnected, ht); 
		if (ht->wio==NULL){
			slog(LOG_ERR, "wget_Connect to download server failed: %s", wget_LastErrorMsg());
			acsState.dlFaultStatus = 9002; /* internal error */
			sendDownloadFault();
		} else
			acsState.startDLTime = time(NULL);
	} else {
		acsState.dlFaultStatus = 9013;
		acsState.dlFaultMsg = "Protocol not supported";
		sendDownloadFault();
	}
    return;
}

#ifndef TESTBOX
/***************************************************************************
// Function Name: bcmTr69KillAllApps().
// Description  : Called by TR69 to kill all available applications before
//                download.
// Parameters   : int conn_fd	//TR69 download socket fd
// Returns      : void
****************************************************************************/
static void bcmTr69KillAllApps(int conn_fd) {
	BSTD_UNUSED(conn_fd);
#if 0
   // NOTE: Add the app name before NULL. Order is important
   char *apps[]=
   {
      "bftpd",
      "telnetd",   
      "sshd",
      "snmp",
      "upnp",
      "sntp",
      "ddnsd",
      "reaim",
      "klogd",
      "syslogd",
      "tftpd",
      "ripd",
      "zebra ",
      "pppd",
      "dnsprobe",
      "dhcpc",
      "igmp",      
      NULL,
   };
   char cmd[SYS_CMD_LEN], app[SYS_CMD_LEN], buf[SYS_CMD_LEN];
   char pidStr[SYS_CMD_LEN];
   char ifName[IFC_SMALL_LEN];
   int pid = 0;
   int i = 0;
   FILE *fs;
   int socketPid = 0;
   int curPid = getpid();

   //Validate the socket fd
   if (conn_fd < 0)
   {
      printf("Error: invalid socket fd %d\n", conn_fd);
	  return;
   }

   bcmGetIntfNameSocket(conn_fd, ifName);
   if (ifName[0] == '\0')
   {
      printf("Error: socket interface name not found\n");
      return;
   }

   sprintf(cmd, "/proc/var/fyi/wan/%s/pid", ifName);
   if ((fs = fopen(cmd, "r")) != NULL)
   {
      fgets(cmd, SYS_CMD_LEN, fs);
      socketPid = atoi(cmd);
      fclose(fs);
   }

   bcmSystemMute("ps > /var/pslist");
   fs = fopen("/var/pslist", "r");
   if (fs == NULL)
      return;
   bcmSystem("cat /var/pslist");

   if (socketPid != 0) 
   {
      // add the default route with the ifc the socket is sitting on
      if (strstr(ifName, "nas") == NULL &&       // only do it if it is not MER and not ipoa (eth?)
          strstr(ifName, "eth") == NULL)
      {
         sprintf(cmd, "route add default dev %s", ifName);
         bcmSystem(cmd);
      }
      rewind(fs);
   }
      
   bcmSystem("sysinfo");

#if 0
//   printf("curPid=%d, socketPid=%d\n", curPid, socketPid);
   // get app and read thru pslist according to the order of apps
#endif
   i = 0;
   do 
   {
      strcpy(app, apps[i]);
      while (fgets(buf, SYS_CMD_LEN, fs) > 0)  
      {
         if (strstr(buf, app) != NULL) // found command line with match app name
         {  
            // find pid string
            sscanf(buf, "%s\n", pidStr);
            pid = atoi(pidStr);
            if (pid != curPid && pid != socketPid)
            {
               printf("kill %s (%d) process...\n", app, pid);
               sprintf(cmd, "kill -%d %d", 9, pid);
               bcmSystem(cmd);
            }
         }
      } 
      rewind(fs);    // start pslist over again
   } while (apps[++i] != NULL);

   fclose(fs);
   bcmSystemMute("rm /var/pslist");
   bcmRemoveModules(socketPid);
#endif
}	/*End of bcmTr69KillAllApps() */
#endif
