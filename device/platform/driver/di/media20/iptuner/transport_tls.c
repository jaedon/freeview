/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:			$Workfile:   util_tls.c  $
 * Version:				$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:		$Author: junsy@humaxdigital.com $
 * Date:				$Date: Fri June 05 10:20:48 KST 2009 $
 * File Description:	TLS(Transport Layer Security) Client Functions
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */

#include "di_iptuner_config.h"
#include "di_iptuner20.h"

#include "di_iptuner_common.h"

/** SSL Include file */
#include "vkernel.h"
#include "di_uart.h"
#include "transport_tls.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
#define DEFAUTL_ROOT_CERTI_PATH			"/etc/pki/tls"

//#define TRANSPORT_TLS_DEBUG
//#define TRANSPORT_TLS_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG	/* DEBUG Define */

#ifdef TRANSPORT_TLS_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* The END of DEBUG DEFINE */

#ifdef TRANSPORT_TLS_CRITICAL_DEBUG
#define PrintCriticalDebug      DI_UART_Print
#else
#define PrintCriticalDebug      while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)

//"/usr/browser/res/for_iptuner_root/"  // This is test root certificate verification.
#define CIPHER_LIST					"ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
void INT_TLS_info_callback(const SSL *s, int where, int ret)
{
	const char *pStr;
	int nMaskWord=0;

	nMaskWord=where& ~SSL_ST_MASK;

	if (nMaskWord & SSL_ST_CONNECT)
	{
		pStr="SSL_connect";
	}
	else if (nMaskWord & SSL_ST_ACCEPT)
	{
		pStr="SSL_accept";
	}
	else
	{
		pStr="undefined";
	}

	if (where & SSL_CB_LOOP)
	{
		PrintDebug ("[INT_TLS_info_callback] %s:%s\n",pStr,SSL_state_string_long(s));
	}
	else if (where & SSL_CB_ALERT)
	{
		pStr=(where & SSL_CB_READ)?"read":"write";
		PrintDebug ("[INT_TLS_info_callback] SSL3 alert %s:%s:%s\n",
			pStr,
			SSL_alert_type_string_long(ret),
			SSL_alert_desc_string_long(ret));
	}
	else if (where & SSL_CB_EXIT)
	{
		if (ret == 0)
		{
			PrintDebug ("[INT_TLS_info_callback] %s:failed in %s\n",
				pStr,SSL_state_string_long(s));
		}
		else if (ret < 0)
		{
			PrintDebug ("[INT_TLS_info_callback] %s:error in %s\n",
				pStr,SSL_state_string_long(s));
		}
	}
}

int pem_passwd_cb(char *buf, int size, int rwflag, void *password)
{
	UNUSED_PARAM(rwflag);

	VK_strncpy(buf, (char *)(password), size);
	buf[size - 1] = '\0';
	return(VK_strlen(buf));
}

static int P_verify_callback(X509_STORE_CTX *ctx, void *arg)
{
	HINT32 nRet=0;
	tls_session_t *ptTls;

	ptTls = (tls_session_t *)arg;

	if(ptTls->cbCerficate->callback != NULL)
	{
		ptTls->cbCerficate->callback(ptTls->cbCerficate->context, ptTls->cbCerficate->userdata, (void *)ctx, &nRet);
	}

	PrintDebug("verify_callback nRet=%d\n", nRet);

	return nRet;
}

static int INT_TLSClient_PeerVerify_Callback (int ok, X509_STORE_CTX *store)
{
	char data[256];
	PrintEnter;

	VK_MEM_Memset (data,0,sizeof(data));
	if (!ok)
	{
		X509 *cert = X509_STORE_CTX_get_current_cert (store);
		int  depth = X509_STORE_CTX_get_error_depth (store);
		int  err = X509_STORE_CTX_get_error (store);

		PrintError ("%s(%d) Error> -Error with certificate at depth: %i\n",__FUNCTION__,__LINE__, depth);
		X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
		PrintError ("%s(%d) Error>   issuer   = %s\n",__FUNCTION__,__LINE__, data);
		X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
		PrintError ("%s(%d) Error>   subject  = %s\n",__FUNCTION__,__LINE__, data);
		PrintError ("%s(%d) Error>   err %i:%s\n",__FUNCTION__,__LINE__, err, X509_verify_cert_error_string(err));
	}

	PrintExit;
	return ok;
}

static SSL_CTX* INT_TLS_Client_CtxSetup (SSL_METHOD_TYPE eMethod_Type, tls_session_t *pstTls)
{
	SSL_CTX *ctx=NULL;
	char		*pCA_Url=NULL;
	HUINT32	nUrlLength=0;

	PrintEnter;

	switch(eMethod_Type)
	{
		case SSL_METHOD_V2:
			{
				ctx = SSL_CTX_new (SSLv2_method());
				break;
			}
		case SSL_METHOD_V2_V3:
			{
				ctx = SSL_CTX_new (SSLv23_method());
				break;
			}
		case TLS_METHOD_V1:
			{
				ctx = SSL_CTX_new (TLSv1_method());
				break;
			}
		case SSL_METHOD_V3:
			{
				ctx = SSL_CTX_new (SSLv3_method());
				break;
			}
		default:
			{
				ctx = SSL_CTX_new (SSLv23_method());
				break;
			}
	}

	PrintDebug("### %s(%d) : (TLS) psz_ca_name(%s) ###\n",__FUNCTION__,__LINE__, pstTls->psz_ca_name);
	PrintDebug("### %s(%d) : (TLS) psz_ca_path(%s) ###\n",__FUNCTION__,__LINE__, pstTls->psz_ca_path);
	PrintDebug("### %s(%d) : (TLS) psz_certi_path(%s) ###\n",__FUNCTION__,__LINE__, pstTls->psz_certi_path);
	PrintDebug("### %s(%d) : (TLS) psz_prikey_path(%s) ###\n",__FUNCTION__,__LINE__, pstTls->psz_prikey_path);
	PrintDebug("### %s(%d) : (TLS) psz_prikey_passwd(%s) ###\n",__FUNCTION__,__LINE__, pstTls->psz_prikey_passwd);

	if(ctx == NULL)
	{
		PrintError ("%s(%d) Error> SSL_CTX_new Failed. Aborting\n",__FUNCTION__,__LINE__);
		return NULL;
	}
	/* else { continue; } */

	if(pstTls->cbCerficate != NULL)
	{
		/* register verify callback about peer(server) */
		SSL_CTX_set_verify (ctx, SSL_VERIFY_PEER, NULL);
		SSL_CTX_set_verify_depth (ctx, 4);

		SSL_CTX_set_cert_verify_callback( ctx, P_verify_callback, (void *)pstTls);
	}
	else
	{
		if (( pstTls->psz_ca_name == NULL ) && ( pstTls->psz_ca_path == NULL ))
		{
			if ( SSL_CTX_load_verify_locations (ctx, NULL, DEFAUTL_ROOT_CERTI_PATH) !=1 )
			{
				PrintError ("%s(%d) Error> loading CA_PATH and/or DIRECTORY !!\n",__FUNCTION__,__LINE__);
				return NULL;
			}
		}
		else if (( pstTls->psz_ca_name != NULL ) && ( pstTls->psz_ca_path != NULL ))
		{
			nUrlLength = VK_strlen(pstTls->psz_ca_name)+VK_strlen(pstTls->psz_ca_path)+1+1;
			pCA_Url = DD_MEM_Alloc(nUrlLength);
			if(pCA_Url == NULL)
			{
				PrintError ("%s(%d) Error> Alloc Fail !!\n",__FUNCTION__,__LINE__);
				return NULL;
			}
			else
			{
				VK_MEM_Memset(pCA_Url, 0x0, nUrlLength);
				VK_snprintf(pCA_Url, nUrlLength, "%s%s",pstTls->psz_ca_path,pstTls->psz_ca_name);
				PrintDebug ("%s(%d) pCA_Url=%s\n",__FUNCTION__,__LINE__, pCA_Url);
			}

			if ( SSL_CTX_load_verify_locations (ctx, pCA_Url, NULL) !=1 )
			{
				PrintError ("%s(%d) Error> loading CA_PATH and/or DIRECTORY !!\n",__FUNCTION__,__LINE__);
				return NULL;
			}
			else
			{
				DD_MEM_Free(pCA_Url);
				pCA_Url = NULL;
			}
		}
		else
		{
			if ( SSL_CTX_load_verify_locations (ctx, pstTls->psz_ca_name, pstTls->psz_ca_path) !=1 )
			{
				PrintError ("%s(%d) Error> loading CA_PATH and/or DIRECTORY !!\n",__FUNCTION__,__LINE__);
				return NULL;
			}
		}

		if ( SSL_CTX_set_default_verify_paths (ctx) != 1)
		{
			PrintError ("%s(%d) Error> loading default CA_PATH and/or DIRECTORY !!\n",__FUNCTION__,__LINE__);
			return NULL;
		}

		if(pstTls->psz_certi_path != NULL)
		{
			/*
			 * SSL_CTX_use_certificate_chain_file() loads a certificate chain from file into ctx.
			 * The certificates must be in PEM format and must be sorted starting with the subject's certificate
			 * (actual client or server certificate), followed by intermediate CA certificates if applicable, and ending at the highest level (root) CA.
			 * There is no corresponding function working on a single SSL object.
			 */
			if ( SSL_CTX_use_certificate_chain_file (ctx, pstTls->psz_certi_path) != 1)
			{
				PrintError ("%s(%d) Error> loading CERTIFICATE From File !!\n",__FUNCTION__,__LINE__);
				return NULL;
			}
		}

		if(pstTls->psz_prikey_path != NULL)
		{
			SSL_CTX_set_default_passwd_cb(ctx, pem_passwd_cb);
			SSL_CTX_set_default_passwd_cb_userdata(ctx,  (void *)pstTls->psz_prikey_path);

			/* adds the first private key found in file to ctx */
			if ( SSL_CTX_use_PrivateKey_file (ctx, pstTls->psz_prikey_path, SSL_FILETYPE_PEM) != 1)
			{
				PrintError ("%s(%d) Error> loading Private Key From File !!\n",__FUNCTION__,__LINE__);
				return NULL;
			}
		}

		/* register verify callback about peer(server) */
		SSL_CTX_set_verify (ctx, SSL_VERIFY_PEER, INT_TLSClient_PeerVerify_Callback);
		SSL_CTX_set_verify_depth (ctx, 4);
	}

	/* not support to SSL Version 2 */
	SSL_CTX_set_options (ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2);
#if defined(TRANSPORT_TLS_DEBUG)
	SSL_CTX_set_info_callback(ctx,INT_TLS_info_callback);
#endif
	/* Settings CIPHER LIST : choose list of available SSL_CIPHERs*/
	if ( SSL_CTX_set_cipher_list (ctx, CIPHER_LIST) != 1)
	{
		PrintError ("%s(%d) Error> loading Private Key From File !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	PrintExit;
	return ctx;
}

static int INT_TLS_Send ( void *p_session, const void *buf, int i_length )
{
	int	nRet = 0;
	int	nSsl_Errcode = 0;
	int	count=0;
	tls_session_t  *pSys = (tls_session_t *)p_session;

RETRY:

#if defined(TRANSPORT_TLS_DEBUG)
	PrintError ("%s(%d) SSL_get_state(pSys->p_ssl):0x%X\n",__FUNCTION__,__LINE__, SSL_get_state(pSys->p_ssl));
	PrintError ("\tclient waiting in SSL_connect - %s\n", SSL_state_string_long(pSys->p_ssl));
#endif
	nRet = SSL_write ( pSys->p_ssl, buf, i_length );
#if defined(TRANSPORT_TLS_DEBUG)
	nSsl_Errcode = SSL_get_error(pSys->p_ssl, nRet);
	if((nSsl_Errcode == 0) && (nRet < 4096))
	{
//		Util_Print_Dump(buf, i_length, 0,1);
	}

	PrintDebug ("%s(%d) Send (%d, %d):(sslcode:%d)\n",__FUNCTION__,__LINE__, nRet, i_length, nSsl_Errcode);
#else
	UNUSED_PARAM(nSsl_Errcode);
#endif
	if ( nRet < 0 ) // ( ( SSL_ERROR_WANT_READ == ssl_errcode) ||  ( SSL_ERROR_WANT_READ == ssl_errcode) )
	{
		nSsl_Errcode = SSL_get_error(pSys->p_ssl, nRet);
		PrintDebug("%s(%d) Send (%d, %d):(sslcode:%d)\n",__FUNCTION__,__LINE__, nRet, i_length, nSsl_Errcode);
		count++;

		if (count > 15)
			goto END;

		VK_TASK_Sleep(200);
		goto RETRY;
	}

END:
	return nRet;
}
static int INT_TLS_Pending ( void *p_session)
{
	tls_session_t  *pSys = (tls_session_t *)p_session;

	if(pSys==NULL)
	{
		PrintError ("%s(%d) Error> invalid tls_session_t handle \n",__FUNCTION__,__LINE__);
		return -1;
	}

	/* check internal ssl buffer */
	return SSL_pending (pSys->p_ssl);
}

static int INT_TLS_Recv ( void *p_session, void *buf, int i_length )
{
	int	nRet = 0;
	int	nSsl_Errcode = 0;
	int	count=0;
	tls_session_t  *pSys = (tls_session_t *)p_session;

RETRY:
#if defined(TRANSPORT_TLS_DEBUG)
	PrintDebug("%s(%d) SSL_get_state(pSys->p_ssl):0x%X\n",__FUNCTION__,__LINE__, SSL_get_state(pSys->p_ssl));
	PrintDebug("\tclient waiting in SSL_connect - %s\n", SSL_state_string_long(pSys->p_ssl));
#endif
	nRet = SSL_read ( pSys->p_ssl, buf, i_length);
#if defined(TRANSPORT_TLS_DEBUG)
	nSsl_Errcode = SSL_get_error(pSys->p_ssl, nRet);
	if((nSsl_Errcode == 0) && (nRet < 4096))
	{
//		Util_Print_Dump(buf, i_length, 0,1);
	}

	PrintDebug ("%s(%d) Recv (%d, %d):(sslcode:%d)\n",__FUNCTION__,__LINE__, nRet, i_length, nSsl_Errcode);
#else
	UNUSED_PARAM(nSsl_Errcode);
#endif
	if ( nRet < 0 ) // ( ( SSL_ERROR_WANT_READ == ssl_errcode) ||  ( SSL_ERROR_WANT_READ == ssl_errcode) )
	{
		nSsl_Errcode = SSL_get_error(pSys->p_ssl, nRet);
		PrintDebug("%s(%d) Recv (%d, %d):(sslcode:%d)\n",__FUNCTION__,__LINE__, nRet, i_length, nSsl_Errcode);
		count++;

		if (count > 15)
			goto END;

		VK_TASK_Sleep(100);
		goto RETRY;
	}

END:
	return nRet;
}

int TLS_Openssl_Init(void)
{
	int	nRet = DI_IPT_SUCCESS;

	PrintEnter;

	SSL_library_init ();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings ();

	PrintExit;

	return nRet;
}

int TLS_Client_Setup (SSL_METHOD_TYPE eMethod_Type, tls_session_t *pstTls)
{
	int	nRet=DI_IPT_SUCCESS;
	SSL_CTX *ctx=NULL;

	PrintEnter;

	/* Generate Random number : RNc*/
	(void) RAND_load_file ("/dev/urandom", 1024);

	if (pstTls == NULL)
	{
		PrintError ("%s(%d) Error> pstTls is null !! \n",__FUNCTION__,__LINE__);
		return DI_IPT_EBADVAR;
	}

	ctx = INT_TLS_Client_CtxSetup (eMethod_Type, pstTls);
	if (ctx == NULL)
	{
		PrintError ("%s(%d) Error> INT_TLS_Client_CtxSetup() Failed !! \n",__FUNCTION__,__LINE__);
		return DI_IPT_EGENERIC;
	}
	else
	{
		pstTls->p_ctx = ctx;
	}

	PrintDebug ("%s(%d) nRet(%d)\n",__FUNCTION__,__LINE__,nRet);

	PrintExit;

	return nRet;
}

int TLS_Client_Connect (tls_session_t *pstTls, int fd, const char *psz_hostname )
{
	int	nRet=DI_IPT_SUCCESS;
	BIO *bio=NULL;
	SSL *ssl=NULL;
	long verify_res=0;

	PrintEnter;

	if (fd < 0)
	{
		PrintError ("%s(%d) Error> Invalid Socket Descriptor \n",__FUNCTION__,__LINE__);
		return DI_IPT_EBADVAR;
	}

	if (pstTls == NULL)
	{
		PrintError ("%s(%d) Error> pstTls is null !! \n",__FUNCTION__,__LINE__);
		return DI_IPT_EBADVAR;
	}

	if (pstTls->p_ctx == NULL)
	{
		PrintError ("%s(%d) Error> pstTls->p_ctx is null !! \n",__FUNCTION__,__LINE__);
		return DI_IPT_EBADVAR;
	}

	/* create a new SSL structure for a connection */
	PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	ssl = SSL_new (pstTls->p_ctx);
	if (ssl == NULL)
	{
		PrintError ("%s(%d) Error> SSL_new() Failed !! \n",__FUNCTION__,__LINE__);
		goto error_free_return;
	}

	/* a socket BIO using fd and close_flag. */
	PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	bio = BIO_new_socket (fd, BIO_NOCLOSE);
	if(bio == NULL)
	{
		PrintError ("%s(%d) Error> BIO_new_socket Failed!! \n",__FUNCTION__,__LINE__);
		goto error_free_return;
	}

	PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	(void) SSL_set_bio (ssl, bio, bio );

	/* Bound fd is already connected Socket before tls_ClientCreate().
	 * Hence, change SSL state only.
	 */
	PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
//	(void) SSL_set_connect_state(ssl);

//	SSL_CTX_set_mode(ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
//	SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);
	/* Attempt to connect */

#if 0 /* Caution to warning: cast discards qualifiers from pointer target type */
	BIO_set_conn_hostname(bio, psz_hostname);
#else
	/* long BIO_set_conn_hostname(BIO *b, char *name); */
	pstTls->psz_host_path = NULL;
	pstTls->psz_host_path = VK_MEM_Alloc(VK_strlen(psz_hostname)+1);
	if(pstTls->psz_host_path != NULL)
	{
		VK_memset(pstTls->psz_host_path, 0x0, VK_strlen(psz_hostname)+1);
		VK_snprintf(pstTls->psz_host_path, VK_strlen(psz_hostname), "%s", psz_hostname);
		BIO_set_conn_hostname(bio, pstTls->psz_host_path);
	}
	else
	{
		PrintError ("%s(%d) Error> Mem Alloc Failed!!(Host Url) \n",__FUNCTION__,__LINE__);
		goto error_free_return;
	}
#endif

	/* Verify the connection opened and perform the handshake */
	if(BIO_do_connect(bio) <= 0)
	{
	    /* Handle failed connection */
	}

 	verify_res = SSL_get_verify_result(ssl);
	if( X509_V_OK != verify_res )
	{
	    /* Handle the failed verification */
		PrintError("[%s:%d]  SSL_get_verify_result return : %ld\n",__FUNCTION__,__LINE__, verify_res);
	}
	else
	{
		PrintError("[%s:%d]  SSL_get_verify_result X509_V_OK\n",__FUNCTION__,__LINE__);
	}

	if (SSL_connect(ssl) <= 0)
	{
		PrintError("%s(%d) SSL_connect Error\n",__FUNCTION__,__LINE__);
		goto error_free_return;
	}

	PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	/* Settings pf_recv/ pf_send Fields */
	pstTls->p_ssl = ssl;
	pstTls->sock.p_sys = pstTls;
	pstTls->sock.pf_send = INT_TLS_Send;
	pstTls->sock.pf_checkbuf = INT_TLS_Pending;
	pstTls->sock.pf_recv = INT_TLS_Recv;

	PrintDebug ("%s(%d) nRet(%d)\n",__FUNCTION__,__LINE__,nRet);
	PrintExit;

	return nRet;

error_free_return:
	if(ssl)
	{
		SSL_shutdown(ssl);
		SSL_clear(ssl);
		SSL_free(ssl);
	}

	if(pstTls->psz_host_path != NULL)
	{
		VK_MEM_Free(pstTls->psz_host_path);
		pstTls->psz_host_path = NULL;
	}

	PrintExit;
    return DI_IPT_EGENERIC;
}

int TLS_Client_Disconnect (tls_session_t *pstTls)
{
	int	nRet=DI_IPT_SUCCESS;

	PrintEnter;

    if (pstTls == NULL)
    {
   		PrintError("%s(%d) Invalid tls_Session\n",__FUNCTION__,__LINE__);
	    return DI_IPT_EBADVAR;
    }

	if (pstTls->p_ssl != NULL)
	{
		SSL_shutdown(pstTls->p_ssl);
		SSL_clear(pstTls->p_ssl);
		SSL_free(pstTls->p_ssl);
		pstTls->p_ssl = NULL;
	}

	if(pstTls->psz_host_path != NULL)
	{
		VK_MEM_Free(pstTls->psz_host_path);
		pstTls->psz_host_path = NULL;
	}

	PrintExit;

	return nRet;
}

int TLS_Client_Release (tls_session_t *pstTls)
{
	int	nRet=DI_IPT_SUCCESS;

	PrintEnter;

	if (pstTls == NULL)
	{
		PrintError ("%s(%d) Error> pstTls is null !! \n",__FUNCTION__,__LINE__);
		return DI_IPT_EBADVAR;
	}

    if (pstTls->p_ctx != NULL)
    {
    	SSL_CTX_free(pstTls->p_ctx);
		PrintDebug ("%s(%d) Tls_Ctx(%p)\n",__FUNCTION__,__LINE__,pstTls->p_ctx);
    	nRet = 0;
	}
    else
    {
		PrintError("%s(%d) Invalid Tls_Ctx\n",__FUNCTION__,__LINE__);
    	nRet = DI_IPT_EBADVAR;
    }

	PrintExit;

	return nRet;
}
