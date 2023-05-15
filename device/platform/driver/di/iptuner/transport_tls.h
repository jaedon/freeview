/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:			$Workfile:   transport_tls.h  $
 * Version:				$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:		$Author: junsy@humaxdigital.com $
 * Date:				$Date: Fri 8 05 10:20:48 KST 2009 $
 * File Description:	TLS(Transport Layer Security) Client Header(SSLv3)
 * Module: 			          
 * Remarks:		
 */

/*******************************************************************/ 	
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/
#ifndef __TRANSPORT_TLS_H__
#define __TRANSPORT_TLS_H__
#if defined (SUPPORT_TLS_V1_PROTOCOL)
#include <openssl/ssl.h>
#include "transport_io.h"

struct tls_session_t
{
	SSL *pssl;
    struct virtual_socket_t sock;
};

struct tls_session_t * tls_ClientCreate (int fd, const char *psz_hostname);
void tls_ClientDelete (struct tls_session_t *cl);
#if defined(CONFIG_IPTUNER_OPENSSL_CERT_MANAGER)
void INT_Clear_ClientInfo(void);
int INT_Load_ClientInfo(void);
#endif
#endif

#endif /* !__TRANSPORT_TLS_H__ */
