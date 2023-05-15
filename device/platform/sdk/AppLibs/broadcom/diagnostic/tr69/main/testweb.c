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
 *----------------------------------------------------------------------*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#include "event.h"
#include "types.h"
#include "utils.h"
#include "digcalc.h"

#include "../webproto/protocol.h"
#include "../webproto/www.h"
#include "../webproto/wget.h"
#include "../SOAPParser/xmlParserSM.h"
#include "../SOAPParser/xmlTables.h"

int verbose;
char    *inf;
size_t len;
char buf[10000];
tWgetInternal   *wio;

typedef struct SessionAuth  {
    char    *nonce;
    char    *realm;
    char    *domain;
    char    *cnonce;
    char    *user;
    char    *pwd;
} SessionAuth;

SessionAuth sessionAuth;
char   sessionKey[HASHHEXLEN+1];

void  resetSessionAuth( SessionAuth *s)
{
    free(s->nonce);
    free(s->realm);
    free(s->domain);
    free(s->cnonce);
    free(s->user);
    free(s->pwd);
}
/* general system status and configruation retrieval */
/* Returns state of WAN interface to be used by tr69 client */
typedef enum {
    eWAN_INACTIVE,
    eWAN_ACTIVE
} eWanState;

 /* Returns state of WAN interface to be used by tr69 client */
eWanState getWanState()
{
    /* add BCM shared library call here to determine status*/
    return eWAN_ACTIVE;
}

char *getArg(char *p, char *pattern, char **argval)
{
    char    *s;
    if(s=strstr(p, pattern)){
        if (s=strchr(s,'=')) {
            if(s=strchr(s,'\"')){
                char *e;
                if (e=strchr(s+1,'\"')) {
                    int lth = e-s-1;
                    if (lth)
                        *argval = strndup(s+1,lth);
                    return s+1;
                }
            }

        }
    }
    *argval = strdup("");     /* dup a NULL string to keep arg checking simple*/
    return NULL;
}

void md5ToAscii( unsigned char *s /*16bytes */, unsigned char *d /*33bytes*/)
{
    int i;
    unsigned char j;

    for (i = 0; i < 16; i++)
        snprintf((char *)&d[i*2],3,"%02x", s[i]);
}

void parsewwwAuthenticate(char *ahdr, SessionAuth *sa)
{
    char    *p;
    char    md5inbuf[256];
    char    hdrvalue[128];
    unsigned char md5buf[16];
    unsigned char HA1[33];
    unsigned char HA2[33];
    unsigned char reqDigest[33];

    resetSessionAuth(&sessionAuth);
     if ( p=strstr(ahdr, "digest")) {
         getArg(p,"realm", &sa->realm);
         getArg(p,"nonce", &sa->nonce);
         getArg(p,"domain", &sa->domain);
         getArg(p,"cnonce", &sa->cnonce);
     }
     snprintf(md5inbuf, sizeof(md5inbuf), "%s:%s:%s", "don", sa->realm, "gatespace");
     tr69_md5it(md5buf, md5inbuf);
     md5ToAscii(md5buf,HA1);
     snprintf(md5inbuf, sizeof(md5inbuf), "%s:%s", "POST", "/");
     tr69_md5it(md5buf, md5inbuf);
     md5ToAscii(md5buf,HA2);
     snprintf(md5inbuf, sizeof(md5inbuf), "%s:%s:%s", HA1, sa->nonce, HA2);
     tr69_md5it(md5buf, md5inbuf);
     md5ToAscii(md5buf, reqDigest);
     snprintf(hdrvalue, sizeof(hdrvalue),
       "Digest username=\"%s\", realm=\"%s\", uri=\"%s\", nonce=\"%s\", response=\"%s\"",
              "don", sa->realm, "/", sa->nonce, reqDigest); 
     wget_AddPostHdr(wio,"Authorization", hdrvalue);
}

void postComplete(void *handle)
{
    tWget *wg = (tWget *)handle;
    int     n;
    int     numhdrs;

    fprintf(stderr, "postComplete\n");
    if (wg->status != 0) {
        fprintf(stderr, "Status = %d http status = %d %s\n", 
                wg->status, wg->hdrs->status_code, wg->msg);
    }
    if (wg->hdrs->status_code== 401) {
        /* need to send authenticate */
        fprintf(stderr, "WWW-Authenticate= %s\n", wg->hdrs->wwwAuthenticate);
        parsewwwAuthenticate(wg->hdrs->wwwAuthenticate, &sessionAuth);
        /* now just resend the last data with the Authorization header */
        wget_PostData(wio,buf,len,"text", postComplete, (void *)wio);
    }

    if ( wg->status==0 && wg->hdrs->status_code<=200 && wg->hdrs->content_length>0) {
        for(;;){
            fprintf(stderr, "proto_Readline\n");
            n = proto_Readline(wg->pc, buf, sizeof(buf));
            if (n < 0)
            {
                fprintf(stderr, "error: sfd=%d Bad response headers: %s(%d)\n", wio->pc->fd, 
                        strerror(errno), errno);
                wget_Disconnect(wio);
                return;
            }
            if (n == 0)
            {
                fprintf(stderr, "error: EOF when reading data \n");
                wget_Disconnect(wio);
                return;
            }
            fprintf(stderr, "%s", buf);
    
        } /* for(;;) */
    }
}

void Connected(void *handle)
{
    tWget *wg = (tWget *)handle;

    if (wio==NULL) {
        fprintf(stderr,"pointer to IO desc is NULL\n");
        exit(0);
    }
    if (wg->status != 0) {
        fprintf(stderr, "Status = %d %s\n", wg->status, wg->msg);
        return;
    }
#ifdef GENERATE_SOAPACTION_HDR
    wget_AddPostHdr(wio,"SOAPAction", "");
#endif
    wget_PostData(wio,buf,len,"text/xml", postComplete, (void *)wio);
    return;
}

static int postFile()
{
    int file;

    if (inf != NULL) {
        if ( (file = open(inf, O_RDONLY, 0 ))== -1){
            slog(LOG_ERR, "Parser:Could not open file %s", inf);
            return -1;
        }
        
        len = read(file, buf, sizeof(buf));
        if (len>0) {
            wio=wget_Connect("http://192.168.10.106:9966/",Connected,NULL);
            if (wio==NULL){
                slog(LOG_ERR, "wget_Connect failed");
                return -1;
            }
            return 0;
        }
    }
    return -1;
}
 
/*
* Initialize all the various tasks 
 */
static void initTasks()
{
    /* Just booted so send initial Inform */
    postFile();

    /* init Kick Listener */

    /* initialize Inform timer */

    /* initialize Scheduled Inform */

}
int main(int argc, char** argv)
{
    int verbose = 0;
    char    c;

    while ((c=getopt(argc, argv, "vf:")) != -1) {
        switch(c){
        case 'v':
            verbose = 1;
            break;
        case 'f':
            inf = optarg;
            break;
        default:
            break;
        }
    }
    
 
 
    #ifdef WRITEPIDFILE
    mkdirs(TR69_DIR);

    pidfile = fopen(TR69_PID_FILE, "w");
    if (pidfile != NULL) {
        fprintf(pidfile, "%d\n", getpid());
        fclose(pidfile);
    }
    #endif

    initLog(verbose);
    /*
    if (!no_daemonize)
        daemonize();
        */
    initTasks();
    eventLoop();
    return 0;

}
