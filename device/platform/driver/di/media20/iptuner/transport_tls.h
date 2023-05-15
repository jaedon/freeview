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
#include <openssl/ssl.h>
#include "transport_io.h"

typedef void (*Tls_CertificateCallback) (void *context, void *userdata, void *ctx, HINT32 *result);

typedef struct Tls_CertificateCallbackDesc_t {
	Tls_CertificateCallback callback;
	void *context;
	void *userdata;
} Tls_CertificateCallbackDesc;

typedef enum {
	SSL_METHOD_NONE,
	SSL_METHOD_V2,		/* no transformation */
	SSL_METHOD_V3,		/* horizontal flip */
	TLS_METHOD_V1,		/* vertical flip */
	SSL_METHOD_V2_V3,	/* transpose across UL-to-LR axis */
} SSL_METHOD_TYPE;

typedef struct tls_session
{
	void	*p_ctx;
	SSL		*p_ssl;
	char	*psz_prikey_passwd;
	char	*psz_ca_name;
	char	*psz_ca_path;
	char	*psz_certi_path;
	char	*psz_prikey_path;
	char	*psz_host_path;
	Tls_CertificateCallbackDesc	*cbCerficate;
    struct virtual_socket_t sock;
}tls_session_t;

int TLS_Openssl_Init(void);
int TLS_Client_Setup (SSL_METHOD_TYPE eMethod_Type, tls_session_t *pstTls);
int TLS_Client_Connect (tls_session_t *pstTls, int fd, const char *psz_hostname );
int TLS_Client_Disconnect (tls_session_t *pstTls);
int TLS_Client_Release (tls_session_t *pstTls);

#endif /* !__TRANSPORT_TLS_H__ */
