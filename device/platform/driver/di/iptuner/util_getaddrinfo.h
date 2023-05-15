/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   util_getaddrinfo.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  May 20 03:08:21 KST 2009 $
 * File Description:	     UTIL Header (getaddrinfo)
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __UTIL_GETADDRINFO_H__
#define __UTIL_GETADDRINFO_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "htype.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
# define NI_MAXNUMERICHOST 64
/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/* Portable network names/addresses resolution layer */
int util_inet_pton (int af, const char *src, void *dst);
const char *util_inet_ntop (int af, const void *src, char *dst, socklen_t cnt);
void util_freeaddrinfo ( struct addrinfo *infos );
int util_getaddrinfo ( const char *node, int i_port, const struct addrinfo *p_hints, struct addrinfo **res );
int util_getnameinfo ( const struct sockaddr *sa, int salen, char *host, int hostlen, int *portnum, int flags );
HBOOL net_SockAddrIsMulticast (const struct sockaddr *addr, socklen_t len); 
int net_GetSockAddress ( int fd, char *address, int *port );
int net_GetPeerAddress ( int fd, char *address, int *port );
HUINT16 net_GetPort (const struct sockaddr *addr);
void net_SetPort (struct sockaddr *addr, HUINT16 port);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* !__UTIL_GETADDRINFO_H__ */
