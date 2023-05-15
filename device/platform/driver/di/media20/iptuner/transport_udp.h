/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   transport_udp.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  May 20 03:08:21 KST 2009 $
 * File Description:	     Transport Layer (UDP) Header
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __TRANSPORT_UDP_H__
#define __TRANSPORT_UDP_H__

#ifdef  __cplusplus
extern "C" {
#endif


/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <sys/socket.h>
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
int net_Subscribe (int fd, const struct sockaddr *addr, socklen_t addrlen);
int net_OpenDgram ( const char *psz_bind, int i_bind, const char *psz_server, int i_server, int family, int protocol );
int net_SetCSCov (int fd, int sendcov, int recvcov);
int net_ConnectDgram( const char *psz_host, int i_port, int i_hlim, int proto );
int net_ConnectUDP (const char *host, int port, int hlim);
int net_ListenUDP1 (const char *host, int port);

int net_SetMulticastRMEM(void);
void net_SetMulticastBuffer(int fd,  int bufsize);
int net_JoinIGMP(const char *hostIP, int port); /* hostIP: dot string IPv4 address */
int net_LeaveIGMP(int nfd, const char *hostIP);
ssize_t net_Recvfrom(int nfd, /*const struct virtual_socket_t *vs,*/ HUINT8 *restrict p_buf, size_t i_buflen, HBOOL waitall, unsigned int to_usec);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __TRANSPORT_UDP_H__ */
