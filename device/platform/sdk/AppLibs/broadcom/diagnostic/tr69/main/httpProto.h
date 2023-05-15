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

#ifndef HTTPPROTO_H
#define HTTPPROTO_H

typedef enum {
    sIdle,
    sAuthenticating,
	sAuthenticated,
    sAuthFailed,
    sShutdown
} AuthState;
 
typedef enum {
	eClosed,	/* connection is closed */
	eClose,		/* set Connection: close on next send */
	eStart,		/* connection is connecting */
	eConnected	/* connection has completed */
} HttpState;

typedef struct HttpTask {
    /* current posted msg dope */
	HttpState		eHttpState;
	AuthState		eAuthState;
    tWgetInternal   *wio;
    char    *postMsg;
    int     postLth;
    AcsStatus   xfrStatus;
    CallBack    callback;
	char	*authHdr;		
} HttpTask;


char *readResponse( tWget *wg, int *mlth, int maxSize);

#endif /*HTTPPROTO_H*/
