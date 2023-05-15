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
#include "di_iptuner.h"

#if defined (SUPPORT_TLS_V1_PROTOCOL)
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


/** SSL Environment : TODO (temperary)*/
// 이 비밀번호도 Binary를 hexa editor로 open하면 쉽게 들키므로 다른 방법을 강구해야 한다.
// 우선 rootfs의 private key를 protect하기 위해서 아래와 같이 비밀번호를 셋팅하도록 한다.
// 2010-12-06 ultracat
#if !defined(CONFIG_IPTUNER_OPENSSL_CERT_MANAGER)
#if defined(USE_IPTUNER_OPENSSL)
#if defined(USE_IPTUNER_OPENSSL_PRIVATEKEYPW)
#define PRIVATE_KEY_PASSWORD USE_IPTUNER_OPENSSL_PRIVATEKEYPW
#else
#define PRIVATE_KEY_PASSWORD NULL
#endif

#if defined(USE_IPTUNER_OPENSSL_CA_PATH)
#define CA_PATH					USE_IPTUNER_OPENSSL_CA_PATH
#endif
#if defined(USE_IPTUNER_OPENSSL_CLIENT_CERTI)
#if defined(USE_IPTUNER_OPENSSL_CLIENT_CERTI_PATH)
#define CLIENT_CERTI_PATH			USE_IPTUNER_OPENSSL_CLIENT_CERTI_PATH
#endif
#if defined(USE_IPTUNER_OPENSSL_CLIENT_PRIVATEKEY_PATH)
#define CLIENT_PRIVKEY_PATH			USE_IPTUNER_OPENSSL_CLIENT_PRIVATEKEY_PATH
#endif

#else
#define CLIENT_CERTI_PATH NULL
#define CLIENT_PRIVKEY_PATH NULL
#endif
#else
#define PRIVATE_KEY_PASSWORD NULL
#define CA_PATH NULL

#endif
#else
#define CERTIFICATE_LIST_MAX_NUM	50
#endif

#define CERTIFICATE_AUTHORITY_DIR	NULL
//"/usr/browser/res/for_iptuner_root/"  // This is test root certificate verification.
#define CIPHER_LIST					"ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
#if defined(CONFIG_IPTUNER_OPENSSL_CERT_MANAGER)
typedef enum
{
	eEncrypt_TYPE_UNKNOWN			= 0,
	eEncrypt_TYPE_P12	,
	eEncrypt_TYPE_PEM,
	eEncrypt_TYPE_ASN1,
	eEncrypt_TYPE_MAX,
} EncryptType_e;

typedef struct ClientPathInfo
{
	char			*domain;
	char			*cert_path;
	EncryptType_e	certEncryptType;
	char			*private_key_path;
	char			*private_key_password;
	EncryptType_e	privateEncryptType;
} ClientPathInfo_t;


typedef struct ClientCertInfo
{
	HUINT32				PathNum;
	ClientPathInfo_t	PathList[CERTIFICATE_LIST_MAX_NUM];
	char				*CaPath;
} ClientCertInfo_t;
#endif

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
#if defined(CONFIG_IPTUNER_OPENSSL_CERT_MANAGER)
static ClientCertInfo_t	s_stClientCertInfo={0, {{NULL, NULL, eEncrypt_TYPE_UNKNOWN, NULL, NULL, eEncrypt_TYPE_UNKNOWN}, }, 0};
#endif
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
#if defined(TRANSPORT_TLS_DEBUG)
void apps_ssl_info_callback(const SSL *s, int where, int ret)
{
	const char *str;
	int w;

	w=where& ~SSL_ST_MASK;

	if (w & SSL_ST_CONNECT) str="SSL_connect";
	else if (w & SSL_ST_ACCEPT) str="SSL_accept";
	else str="undefined";

	if (where & SSL_CB_LOOP)
		{
		PrintDebug ("%s:%s\n",str,SSL_state_string_long(s));
		}
	else if (where & SSL_CB_ALERT)
		{
		str=(where & SSL_CB_READ)?"read":"write";
		PrintDebug ("SSL3 alert %s:%s:%s\n",
			str,
			SSL_alert_type_string_long(ret),
			SSL_alert_desc_string_long(ret));
		}
	else if (where & SSL_CB_EXIT)
		{
		if (ret == 0)
			PrintDebug ("%s:failed in %s\n",
				str,SSL_state_string_long(s));
		else if (ret < 0)
			{
			PrintDebug ("%s:error in %s\n",
				str,SSL_state_string_long(s));
			}
		}
}
#endif

#if defined(USE_IPTUNER_OPENSSL_PRIVATEKEYPW) || defined(CONFIG_IPTUNER_OPENSSL_CERT_MANAGER)
int pem_passwd_cb(char *buf, int size, int rwflag, void *password)
{
	UNUSED_PARAM(rwflag);

	VK_strncpy(buf, (char *)(password), size);
	buf[size - 1] = '\0';
	return(VK_strlen(buf));
}
#endif


#if defined(USE_IPTUNER_OPENSSL_CLIENT_CERTI) || defined(CONFIG_IPTUNER_OPENSSL_CERT_MANAGER)
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
#endif /* (USE_IPTUNER_OPENSSL_CLIENT_CERTI) */

#if defined(CONFIG_IPTUNER_OPENSSL_CERT_MANAGER)
static SSL_CTX* INT_TLS_Client_CtxSetup (const char *psz_hostname)
{
	SSL_CTX *ctx=NULL;
	HUINT32		i;

	PrintEnter;

	ctx = SSL_CTX_new (SSLv23_method());
	if(ctx == NULL)
	{
		PrintError ("%s(%d) Error> SSL_CTX_new Failed. Aborting\n",__FUNCTION__,__LINE__);
		return NULL;
	}
	/* else { continue; } */

	if ( s_stClientCertInfo.CaPath== NULL )
	{
		PrintError ("%s(%d) Error> CA PATH is not defined !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	if(VK_strstr(s_stClientCertInfo.CaPath,".pem") != NULL)
	{
		if ( SSL_CTX_load_verify_locations (ctx, s_stClientCertInfo.CaPath, CERTIFICATE_AUTHORITY_DIR) !=1 )
		{
			PrintError ("%s(%d) Error> loading CA_PATH and/or DIRECTORY !!\n",__FUNCTION__,__LINE__);
			return NULL;
		}
	}
	else
	{
		if ( SSL_CTX_load_verify_locations (ctx, NULL, s_stClientCertInfo.CaPath) !=1 )
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

	/*
	 * SSL_CTX_use_certificate_chain_file() loads a certificate chain from file into ctx.
	 * The certificates must be in PEM format and must be sorted starting with the subject's certificate
	 * (actual client or server certificate), followed by intermediate CA certificates if applicable, and ending at the highest level (root) CA.
	 * There is no corresponding function working on a single SSL object.
	 */

	for(i = 0 ; i < s_stClientCertInfo.PathNum ; i++)
	{
		if(VK_strstr(psz_hostname, s_stClientCertInfo.PathList[i].domain) != NULL)
		{
			if ( SSL_CTX_use_certificate_chain_file (ctx, s_stClientCertInfo.PathList[i].cert_path) != 1)
			{
				PrintError ("%s(%d) Error> loading CERTIFICATE From File !!\n",__FUNCTION__,__LINE__);
				return NULL;
			}

			if(s_stClientCertInfo.PathList[i].private_key_password != NULL)
			{
				SSL_CTX_set_default_passwd_cb(ctx, pem_passwd_cb);
				SSL_CTX_set_default_passwd_cb_userdata(ctx,  (void *)s_stClientCertInfo.PathList[i].private_key_password);
			}

			/* adds the first private key found in file to ctx */
			if(s_stClientCertInfo.PathList[i].privateEncryptType == eEncrypt_TYPE_PEM)
			{
				if ( SSL_CTX_use_PrivateKey_file (ctx, s_stClientCertInfo.PathList[i].private_key_path, SSL_FILETYPE_PEM) != 1)
				{
					PrintError ("%s(%d) Error> loading Private Key From File !!\n",__FUNCTION__,__LINE__);
					return NULL;
				}
			}
			else if(s_stClientCertInfo.PathList[i].privateEncryptType == eEncrypt_TYPE_ASN1)
			{
				if ( SSL_CTX_use_PrivateKey_file (ctx, s_stClientCertInfo.PathList[i].private_key_path, SSL_FILETYPE_ASN1) != 1)
				{
					PrintError ("%s(%d) Error> loading Private Key From File !!\n",__FUNCTION__,__LINE__);
					return NULL;
				}
			}

			/* register verify callback about peer(server) */
			SSL_CTX_set_verify (ctx, SSL_VERIFY_PEER, INT_TLSClient_PeerVerify_Callback);
			SSL_CTX_set_verify_depth (ctx, 4);
			/* not support to SSL Version 2 */
			SSL_CTX_set_options (ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2);
#if defined(TRANSPORT_TLS_DEBUG)
			SSL_CTX_set_info_callback(ctx,apps_ssl_info_callback);
#endif
			/* Settings CIPHER LIST : choose list of available SSL_CIPHERs*/
			if ( SSL_CTX_set_cipher_list (ctx, CIPHER_LIST) != 1)
			{
				PrintError ("%s(%d) Error> loading Private Key From File !!\n",__FUNCTION__,__LINE__);
				return NULL;
			}
			break;
		}
	}

	PrintExit;
	return ctx;
}
#else
static SSL_CTX* INT_TLS_Client_CtxSetup (const char *psz_hostname)
{
	SSL_CTX *ctx=NULL;

	UNUSED_PARAM(psz_hostname);
	PrintEnter;
#if !defined(USE_IPTUNER_OPENSSL_CLIENT_CERTI)
	ctx = SSL_CTX_new (SSLv2_client_method());
#else
	ctx = SSL_CTX_new (SSLv23_method());
#endif
	if(ctx == NULL)
	{
		PrintError ("%s(%d) Error> SSL_CTX_new Failed. Aborting\n",__FUNCTION__,__LINE__);
		return NULL;
	}
	/* else { continue; } */

	if ( CA_PATH == NULL )
	{
		PrintError ("%s(%d) Error> CA PATH is not defined !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}
	if ( SSL_CTX_load_verify_locations (ctx, CA_PATH, CERTIFICATE_AUTHORITY_DIR) !=1 )
	{
		PrintError ("%s(%d) Error> loading CA_PATH and/or DIRECTORY !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	if ( SSL_CTX_set_default_verify_paths (ctx) != 1)
	{
		PrintError ("%s(%d) Error> loading default CA_PATH and/or DIRECTORY !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	/*
	 * SSL_CTX_use_certificate_chain_file() loads a certificate chain from file into ctx.
	 * The certificates must be in PEM format and must be sorted starting with the subject's certificate
	 * (actual client or server certificate), followed by intermediate CA certificates if applicable, and ending at the highest level (root) CA.
	 * There is no corresponding function working on a single SSL object.
	 */
#if defined(USE_IPTUNER_OPENSSL_CLIENT_CERTI)
	if ( SSL_CTX_use_certificate_chain_file (ctx, CLIENT_CERTI_PATH) != 1)
	{
		PrintError ("%s(%d) Error> loading CERTIFICATE From File !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}

#if defined(USE_IPTUNER_OPENSSL_PRIVATEKEYPW)
	SSL_CTX_set_default_passwd_cb(ctx, pem_passwd_cb);
	SSL_CTX_set_default_passwd_cb_userdata(ctx,  (void *)PRIVATE_KEY_PASSWORD);
#endif

	/* adds the first private key found in file to ctx */
	if ( SSL_CTX_use_PrivateKey_file (ctx, CLIENT_PRIVKEY_PATH, SSL_FILETYPE_PEM) != 1)
	{
		PrintError ("%s(%d) Error> loading Private Key From File !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}

	/* register verify callback about peer(server) */
	SSL_CTX_set_verify (ctx, SSL_VERIFY_PEER, INT_TLSClient_PeerVerify_Callback);
	SSL_CTX_set_verify_depth (ctx, 4);
	/* not support to SSL Version 2 */
	SSL_CTX_set_options (ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2);
#if defined(TRANSPORT_TLS_DEBUG)
	SSL_CTX_set_info_callback(ctx,apps_ssl_info_callback);
#endif
	/* Settings CIPHER LIST : choose list of available SSL_CIPHERs*/
	if ( SSL_CTX_set_cipher_list (ctx, CIPHER_LIST) != 1)
	{
		PrintError ("%s(%d) Error> loading Private Key From File !!\n",__FUNCTION__,__LINE__);
		return NULL;
	}
#endif /* (USE_IPTUNER_OPENSSL_CLIENT_CERTI) */
	PrintExit;
	return ctx;
}
#endif

/**
 * Sends data through a TLS session.
 */
static int INT_TLS_Send ( void *p_session, const void *buf, int i_length )
{
	int res,ssl_errcode,count=0;
	struct tls_session_t  *pSys = (struct tls_session_t *)p_session;

RETRY:

#if defined(TRANSPORT_TLS_DEBUG)
	PrintError ("%s(%d) Error>   SSL_get_state(pSys->pssl):0x%X\n",__FUNCTION__,__LINE__, SSL_get_state(pSys->pssl));
	PrintError ("\tclient waiting in SSL_connect - %s\n", SSL_state_string_long(pSys->pssl));
//	Util_Print_Dump(buf, i_length, 0,1);
#endif
	res = SSL_write ( pSys->pssl, buf, i_length );
#if defined(TRANSPORT_TLS_DEBUG)
	ssl_errcode = SSL_get_error(pSys->pssl, res);
	PrintDebug ("%s(%d) Error>   err %i:(sslcode:%d)\n",__FUNCTION__,__LINE__, res, ssl_errcode);
#else
	UNUSED_PARAM(ssl_errcode);
#endif
	if ( res < 0 ) // ( ( SSL_ERROR_WANT_READ == ssl_errcode) ||  ( SSL_ERROR_WANT_READ == ssl_errcode) )
	{
		count++;

		if (count > 15)
			goto END;

		VK_TASK_Sleep(500);
		goto RETRY;
	}

END:
	return res;
}

static int INT_TLS_Pending ( void *p_session)
{
	struct tls_session_t  *pSys = (struct tls_session_t *)p_session;

	if(pSys==NULL)
	{
		PrintError ("%s(%d) Error>   invalid tls_session_t handle\n",__FUNCTION__,__LINE__);
		return -1;
	}

	return SSL_pending(pSys->pssl);
}

/**
 * Receives data through a TLS session.
 */
static int INT_TLS_Recv ( void *p_session, void *buf, int i_length )
{
	int res,ssl_errcode,count=0;
	struct tls_session_t  *pSys = (struct tls_session_t *)p_session;

RETRY:
#if defined(TRANSPORT_TLS_DEBUG)
//	PrintError("%s(%d) Error>   SSL_get_state(pSys->pssl):0x%X\n",__FUNCTION__,__LINE__, SSL_get_state(pSys->pssl));
//	PrintError("\tclient waiting in SSL_connect - %s\n", SSL_state_string_long(pSys->pssl));
//	Util_Print_Dump(buf, i_length, 0,1);
#endif
	res = SSL_read ( pSys->pssl, buf, i_length);
#if defined(TRANSPORT_TLS_DEBUG)
	ssl_errcode = SSL_get_error(pSys->pssl, res);
	PrintDebug ("%s(%d) Error>   err %i:(sslcode:%d)\n",__FUNCTION__,__LINE__, res, ssl_errcode);
#else
	UNUSED_PARAM(ssl_errcode);
#endif
	if ( res < 0 ) // ( ( SSL_ERROR_WANT_READ == ssl_errcode) ||  ( SSL_ERROR_WANT_READ == ssl_errcode) )
	{
		count++;

		if (count > 15)
			goto END;

		VK_TASK_Sleep(100);
		goto RETRY;
	}

END:
	return res;
}

#if defined(CONFIG_IPTUNER_OPENSSL_CERT_MANAGER)
void INT_Clear_ClientInfo()
{
	HUINT32		i;

	for(i = 0 ; i < s_stClientCertInfo.PathNum ; i++)
	{
		if(s_stClientCertInfo.PathList[i].domain != NULL)
		{
			VK_free(s_stClientCertInfo.PathList[i].domain);
			s_stClientCertInfo.PathList[i].domain = NULL;
		}

		if(s_stClientCertInfo.PathList[i].cert_path != NULL)
		{
			VK_free(s_stClientCertInfo.PathList[i].cert_path);
			s_stClientCertInfo.PathList[i].cert_path = NULL;
		}

		s_stClientCertInfo.PathList[i].certEncryptType = eEncrypt_TYPE_UNKNOWN;

		if(s_stClientCertInfo.PathList[i].private_key_path != NULL)
		{
			VK_free(s_stClientCertInfo.PathList[i].private_key_path);
			s_stClientCertInfo.PathList[i].private_key_path = NULL;
		}

		s_stClientCertInfo.PathList[i].privateEncryptType = eEncrypt_TYPE_UNKNOWN;

		if(s_stClientCertInfo.PathList[i].private_key_password != NULL)
		{
			VK_free(s_stClientCertInfo.PathList[i].private_key_password);
			s_stClientCertInfo.PathList[i].private_key_password = NULL;
		}
	}

	s_stClientCertInfo.PathNum = 0;

	if(s_stClientCertInfo.CaPath != NULL)
	{
		VK_free(s_stClientCertInfo.CaPath);
		s_stClientCertInfo.CaPath = NULL;
	}
}

int INT_Load_ClientInfo()
{
	FILE *fp;
	char	ch;
	char	tmpDomain[255] = {0, }, tmpCertPath[255] = {0, }, tmpCertType[20] = {0, }, tmpCertEncrypt[20] = {0, }, tmpPrivatePath[255] = {0, }, tmpPrivatePassword[255] = {0, }, tmpPrivateEncrypt[20] = {0, };

	if(s_stClientCertInfo.PathNum > 0)
	{
		INT_Clear_ClientInfo();
	}

	fp = fopen(CONFIG_IPTUNER_OPENSSL_CERT_MANAGER_DATA_PATH, "r");
	if ( fp )
	{
		// 주석 줄은 넘어간다.
		while(1)
		{
			ch = fgetc(fp);
			if(ch == '#' || ch == 10)
			{
				fscanf(fp, "%*[^\n]");
				fscanf(fp, "%*c]");
			}
			else
			{
				fseek(fp, -1, SEEK_CUR);
				break;
			}
		}

		// data 는 tab 으로 구분된다.
		// domain, cert_path, cert_type, private key path, private key password 순.
		while(s_stClientCertInfo.PathNum < CERTIFICATE_LIST_MAX_NUM)
		{
			if(fscanf(fp, "%s	%s	%s	%s	%s	%s	%s", \
						tmpDomain, tmpCertPath, tmpCertType, tmpCertEncrypt, \
						tmpPrivatePath, tmpPrivateEncrypt, tmpPrivatePassword) == EOF)
						break;
			if(VK_strcmp(tmpCertType, "client") == 0)
			{
				s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].domain = VK_StrDup(tmpDomain);
				s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].cert_path = VK_StrDup(tmpCertPath);

				if(VK_strcmp(tmpCertEncrypt, "P12") == 0)
				{
					s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].certEncryptType = eEncrypt_TYPE_P12;
				}
				else if(VK_strcmp(tmpCertEncrypt, "PEM") == 0)
				{
					s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].certEncryptType = eEncrypt_TYPE_PEM;
				}
				else
				{
					PrintError ("%s(%d) Error> Unknown Cert Type!!\n",__FUNCTION__,__LINE__);
					s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].certEncryptType = eEncrypt_TYPE_UNKNOWN;
				}

				if(VK_strcmp(tmpPrivatePath, "NULL") == 0)
				{
					s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].private_key_path = NULL;
					s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].privateEncryptType = eEncrypt_TYPE_UNKNOWN;
					s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].private_key_password= NULL;
				}
				else
				{
					s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].private_key_path = VK_StrDup(tmpPrivatePath);

					if(VK_strcmp(tmpPrivateEncrypt, "ASN1") == 0)
					{
						s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].privateEncryptType = eEncrypt_TYPE_ASN1;
					}
					else if(VK_strcmp(tmpPrivateEncrypt, "PEM") == 0)
					{
						s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].privateEncryptType = eEncrypt_TYPE_PEM;
					}
					else
					{
						PrintError ("%s(%d) Error> Unknown Private Type!!\n",__FUNCTION__,__LINE__);
						s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].privateEncryptType = eEncrypt_TYPE_UNKNOWN;
					}

					if(VK_strcmp(tmpPrivatePassword, "NULL") == 0)
					{
						s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].private_key_password= NULL;
					}
					else
					{
						s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].private_key_password = VK_StrDup(tmpPrivatePassword);
					}
				}

#if defined(CONFIG_DEBUG)
				PrintDebug("%s(%d) == Path List #%d info ==\n",__FUNCTION__,__LINE__, s_stClientCertInfo.PathNum);
				PrintDebug("%s(%d) Domain = %s\n",__FUNCTION__,__LINE__, s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].domain);
				PrintDebug("%s(%d) Cert Path = %s\n",__FUNCTION__,__LINE__, s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].cert_path);
				PrintDebug("%s(%d) Cert Type = %d\n",__FUNCTION__,__LINE__, s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].certEncryptType);
				PrintDebug("%s(%d) Private Path = %s\n",__FUNCTION__,__LINE__, s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].private_key_path);
				PrintDebug("%s(%d) Private Type = %d\n",__FUNCTION__,__LINE__, s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].privateEncryptType);
				PrintDebug("%s(%d) Private Password = %s\n\n",__FUNCTION__,__LINE__, s_stClientCertInfo.PathList[s_stClientCertInfo.PathNum].private_key_password);
#endif
				s_stClientCertInfo.PathNum++;
			}
			else if(VK_strcmp(tmpCertType, "root") == 0)
			{
				if(s_stClientCertInfo.CaPath != NULL)
					DD_MEM_Free(s_stClientCertInfo.CaPath);
				s_stClientCertInfo.CaPath = VK_StrDup(tmpCertPath);
#if defined(CONFIG_DEBUG)
				PrintDebug("%s(%d) == CA Path = %s ==\n\n",__FUNCTION__,__LINE__, s_stClientCertInfo.CaPath);
#endif
			}
			else
			{
				PrintError("%s(%d) Invalid Cert Type!! (%s)\n\n",__FUNCTION__,__LINE__, tmpCertType);
			}
			fscanf(fp, "%*[^\n]");
			fscanf(fp, "%*c]");
		}

		if(s_stClientCertInfo.PathNum == CERTIFICATE_LIST_MAX_NUM)
			PrintError("%s(%d) Cert list is full !! (%d/%d)\n\n",__FUNCTION__,__LINE__, s_stClientCertInfo.PathNum, CERTIFICATE_LIST_MAX_NUM);

	}
	else
	{
		PrintError ("%s(%d) Error> Cert.conf open Failed !!\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	fclose(fp);
	return ERR_OK;
}
#endif

/**
 * Allocates a client's TLS credentials and shakes hands through the network.
 * This is a blocking network operation.
 *
 * @param fd stream socket through which to establish the secure communication
 * layer.
 * @param psz_hostname Server Name Indication to pass to the server, or NULL.
 *
 * @return NULL on error.
 **/
struct tls_session_t * tls_ClientCreate (int fd, const char *psz_hostname)
{
	BIO *bio=NULL;
	SSL *ssl=NULL;
	SSL_CTX *ctx=NULL;
	long verify_res=0;
	struct tls_session_t *pstTls=NULL;

	PrintEnter;
	if (fd < 0)
	{
		PrintError ("%s(%d) Error> Invalid Socket Descriptor \n",__FUNCTION__,__LINE__);
		return NULL;
	}

	/* create tls_session_t */
	pstTls = (struct tls_session_t *) DD_MEM_Alloc (sizeof(struct tls_session_t));
	if(pstTls == NULL)
	{
		PrintError ("%s(%d) Error> VK_MEM_Alloc Failed !!\n",__FUNCTION__,__LINE__);
		goto error_free_return;
	}

	SSL_library_init ();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings ();

	/* Generate Random number : RNc*/
	(void) RAND_load_file ("/dev/urandom", 1024);

	ctx = INT_TLS_Client_CtxSetup (psz_hostname);
	if (ctx == NULL)
	{
		PrintError ("%s(%d) Error> INT_TLS_Client_CtxSetup() Failed !! \n",__FUNCTION__,__LINE__);
		if(pstTls) DD_MEM_Free (pstTls);
		return NULL;
	}

#if 1
	/* create a new SSL structure for a connection */
	PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	ssl = SSL_new (ctx);
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
#else
	/* Connect To Server */
	bio = BIO_new_connect ("111.111.111.230" ":" "443");
	if (!bio)
	{
		PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	}

	if (BIO_do_connect(bio) <= 0)
	{
		PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	}

	ssl = SSL_new(ctx);
	SSL_set_bio(ssl, bio, bio);
	if (SSL_connect(ssl) <= 0)
	{
		PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	}
#endif


#if 0
	/* Verify Server Certificate ?? */
	PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	/* Verify Peer Certificate (optional) */
	if ( (err = INT_TLS_Client_PostConnection_Check (ssl,psz_hostname)) != X509_V_OK)
	{
		PrintError ("%s(%d) Error> INT_TLS_Client_PostConnection_Check Failed (%ld)!! \n",__FUNCTION__,__LINE__,err);
		goto error_free_return;
	}
#endif

//	SSL_CTX_set_mode(ctx, SSL_MODE_ENABLE_PARTIAL_WRITE);
//	SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);
	/* Attempt to connect */

	BIO_set_conn_hostname(bio, psz_hostname);

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
		PrintError("[%s:%d]  SSL_get_verify_result X509_V_OK\n",__FUNCTION__,__LINE__);


	if (SSL_connect(ssl) <= 0)
	{
		PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	}

	PrintDebug("%s(%d)\n",__FUNCTION__,__LINE__);
	/* Settings pf_recv/ pf_send Fields */
	pstTls->pssl = ssl;
	pstTls->sock.p_sys = pstTls;
	pstTls->sock.pf_send = INT_TLS_Send;
	pstTls->sock.pf_checkbuf= INT_TLS_Pending;
	pstTls->sock.pf_recv = INT_TLS_Recv;

	if(ctx) SSL_CTX_free(ctx);

	PrintDebug ("%s(%d) pstTls(%p)\n",__FUNCTION__,__LINE__,pstTls);
	PrintExit;
	return pstTls;

error_free_return:
	if(ssl)
	{
		SSL_shutdown(ssl);
		SSL_clear(ssl);
	}

	if(ssl) SSL_free(ssl);
	if(ctx) SSL_CTX_free(ctx);
	if(pstTls) DD_MEM_Free (pstTls);

	PrintExit;
    return NULL;
}


/**
 * Releases data allocated with tls_ClientCreate.
 * It is your job to close the underlying socket.
 */
void tls_ClientDelete (struct tls_session_t *pstTls)
{
	PrintEnter;
    if (pstTls == NULL) return;

	if (pstTls->pssl)
	{
		SSL_shutdown(pstTls->pssl);
		SSL_clear(pstTls->pssl);
		SSL_free(pstTls->pssl);
	}

	DD_MEM_Free (pstTls);

	PrintExit;
	return;
}
#endif
