/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   transport_io.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  Apr 17 03:08:21 KST 2009 $
 * File Description:	     Transport Layer (IO) Header
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __TRANSPORT_IO_H__
#define __TRANSPORT_IO_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include <unistd.h>
#include <stdarg.h>

#include <sys/socket.h>
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
/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
/* Functions to read from or write to the networking layer */
struct virtual_socket_t
{
    void *p_sys;
    int (*pf_recv) ( void *, void *, int );
	int (*pf_checkbuf) (void*);
    int (*pf_send) ( void *, const void *, int );
};

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
int net_SetupSocket (int fd);
int net_Socket (int family, int socktype, int protocol);
ssize_t net_Read (int fd, const struct virtual_socket_t *p_vs, HUINT8 *restrict p_buf, size_t i_buflen, HBOOL waitall);
ssize_t net_Write( int fd, const struct virtual_socket_t *p_vs, const HUINT8 *p_data, size_t i_data );
char *net_Gets (int fd, const struct virtual_socket_t *p_vs );
ssize_t net_Printf (int fd, const struct virtual_socket_t *p_vs, const char *psz_fmt, ... );
ssize_t net_vaPrintf ( int fd, const struct virtual_socket_t *p_vs, const char *psz_fmt, va_list args );
ssize_t net_Sendmsg (int s, struct msghdr *hdr, int flags);
ssize_t net_Recvmsg (int s, struct msghdr *hdr, int flags);
int net_Close (int fd);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __TRANSPORT_IO_H__ */
