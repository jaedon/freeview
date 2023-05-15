
/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ip.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/19/07 12:35p $
 *
 * Module Description: Helpful functions for IP 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/ip.h $
 * 
 * 2   9/19/07 12:35p jrubio
 * PR34613: add new recv function
 * 
 * 1   9/17/07 12:41p jrubio
 * PR34613: Helper funcs for ip
 * 
 *
 ***************************************************************************/
#ifndef __IP_H__
#define __IP_H__

#ifdef __cplusplus
extern "C" {
#endif

int tcpConnect(const char * ipv4, int port);
void tcpClose(int sd);

int tcprecv(int fd, void *buf, size_t n, int flags);

#ifdef __cplusplus
}
#endif

#endif // IP_H__
