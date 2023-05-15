/**
	@file     port_itk_misc.c
	@brief    ITK Host Misc 모듈.

	Description: ITK Host Misc Interface 이다.							\n
	Module: mw/mheg/itk												\n

	Copyright (c) 2008 HUMAX Co., Ltd.									\n
	All rights reserved.													\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <hapi.h>
#include <apk.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

#include "port_itk_main.h"
#include "port_itk_display.h"
#include "port_itk_misc.h"
#include "port_itk_service.h"
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined (WIN32)
#define __func__	__FUNCTION__
#endif




#ifdef CONFIG_DEBUG
#define ItkTrace(message)		ItkLogTrace(ITK_LOG_MISC, message)
#define ItkPrint(message)		ItkLogPrint(ITK_LOG_MISC, message)
#define ItkData(message)		ItkLogData(ITK_LOG_MISC, message)
#define ItkAssert(message) 		ItkLogAssert(ITK_LOG_MISC, message)
#define ItkInfo(message)		ItkLogInfo(ITK_LOG_MISC, message)
#define ItkWarn(message)		ItkLogWarn(ITK_LOG_MISC, message)
#define ItkError(message) 		ItkLogError(ITK_LOG_MISC, message)
#define ItkFatal(c) 			ItkLogFatal(ITK_LOG_MISC, c)
#else
#define ItkTrace(message)
#define ItkPrint(message)
#define ItkData(message)
#define ItkAssert(message)
#define ItkInfo(message)
#define ItkWarn(message)
#define ItkError(message)
#define ItkFatal(c)
#endif

#define ITK_PREFERRED_LAN				"eng"

#define ITK_HOST_VERSION				"HMXcccvvv"			/* PORT_ITK_MISC_GetIdString함수 참고 */

#define MAX_LAUNCHAPP_URL_LENGTH		512

#define	MAX_SYSTEMID_LEN				12

#define	ITK_MISC_CERTIFICATE_AUTHORITY_DIR		"/etc/pki/tls"
#define ITK_MIS_HTTPS_PORT 				"443"

//#define ITK_MISC_CERTIFICATE_CHECK			1

/** @brief WatchTV application state enumerates */
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
// It should be the same one on port_itk_service.c
typedef struct tagUrl_Data
{
	HCHAR*		scheme;
	HCHAR*		host;
	HCHAR*		name;
	HCHAR*		value;
}ITK_UrlData_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
itk_user_setting_callback_t g_misc_user_setting_callback = NULL;
void *g_misc_user_setting_callbackContext = NULL;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static itk_misc_t			s_stMiscObj;
static HCHAR				s_szSystemId[MAX_SYSTEMID_LEN];
static HUINT32				s_ulPromptForGuide = 0;
static HBOOL				s_bPFG = 0;
static HCHAR				s_szURL[MAX_LAUNCHAPP_URL_LENGTH] = {0,};

/*******************************************************************/
/******************* external Function Prototypes  *******************/
/*******************************************************************/
extern HINT32 PORT_ITK_SVC_ParseUrlUntilHost(HCHAR* url, ITK_UrlData_t *uf);
extern void PORT_ITK_SVC_FreeUrlData(ITK_UrlData_t *uf);

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define _____MHEG_HOST_MANAGER_APIs________________________________________________________________

/*
 * Host copies the preferred language code as ISO 3 characters code in the
 * provided buffers with '\0' terminated. If no preferred language available
 * for any of the parameters then "und" shall be selected.
 */
void mheg_itk_port_miscGetPreferredLanguage(struct itk_misc_t* thiz,
						itk_lang_code_t applicationLang,
						itk_lang_code_t audioLang,
						itk_lang_code_t subtitleLang)
{

	HCHAR	buf[4];
	HERROR	hError = ERR_OK;

	if (thiz == NULL)
	{
		HxLOG_Assert(thiz);
		return;
	}

	HxLOG_Debug("[%s, %d]  (+) \n", __FUNCTION__, __LINE__);

	if (applicationLang)
	{
		hError = APK_SCENARIO_GetMainLanguage(buf);
		if(hError == ERR_OK)
		{
			strncpy(applicationLang, buf, 4);
		}
		else
		{
			strncpy(applicationLang, ITK_PREFERRED_LAN, 4);
		}
		applicationLang[3] = '\0';
	}

	HxLOG_Debug("[%s, %d]  applicationLang (%s)  \n", __FUNCTION__, __LINE__, applicationLang);

	if (audioLang)
	{
		hError = APK_SCENARIO_GetMainLanguage(buf);
		if(hError == ERR_OK)
		{
			strncpy(audioLang, buf, 4);
		}
		else
		{
			strncpy(audioLang, ITK_PREFERRED_LAN, 4);
		}
		audioLang[3] = '\0';
	}

	HxLOG_Debug("[%s, %d]  audioLang (%s)  \n", __FUNCTION__, __LINE__, audioLang);

	if (subtitleLang)
	{
		hError = APK_SCENARIO_GetMainLanguage(buf);
		if(hError == ERR_OK)
		{
			strncpy(subtitleLang, buf, 4);
		}
		else
		{
			strncpy(subtitleLang, ITK_PREFERRED_LAN, 4); //
		}
		subtitleLang[3] = '\0';
	}

	HxLOG_Debug("[%s, %d]  subtitleLang (%s)  \n", __FUNCTION__, __LINE__, subtitleLang);
	HxLOG_Debug("[%s, %d]  (-) \n", __FUNCTION__, __LINE__);
}

/**
 * Adds a callback to invoke when a viewer setting changes
 *
 * @param[in] thiz The host
 * @param[in] callback The callback function to link to
 * @param[in] callbackContext A context pointer to send when the callback is
 *            invoked
 *
 * @return ITKE_OK if the callback is registered
 * @return ITKE_NO_RESOURCE if there is already a callback registered
 */
itk_errcode_t mheg_itk_port_miscAddSettingCallback(struct itk_misc_t *thiz,
                                         itk_user_setting_callback_t callback,
                                         void *callbackContext)
{
	if (thiz == NULL)
	{
		HxLOG_Assert(0)
		return ITKE_FAIL;
	}

	if (callback == NULL)
	{
		HxLOG_Assert(0);
		return ITKE_FAIL;
	}

	g_misc_user_setting_callback = callback;
	g_misc_user_setting_callbackContext = callbackContext;

	HxLOG_Debug("Registered !!! (%p/%p) \n", callback, callbackContext);

	return ITKE_OK;
}


/**
 * Removes a callback to invoke when a user setting changes
 *
 * @param[in] thiz The host
 */
void mheg_itk_port_miscRemoveSettingCallback(struct itk_misc_t *thiz)
{
	if (thiz == NULL)
	{
		HxLOG_Assert(0);
		return;
	}

	g_misc_user_setting_callback = NULL;
	g_misc_user_setting_callbackContext = NULL;
}

itk_control_t mheg_itk_port_miscGetSubtitleSetting(struct itk_misc_t *thiz)
{
	HBOOL	bEnable = FALSE;

	if (thiz == NULL)
	{
		HxLOG_Assert(0);
		return ITK_DISABLE;
	}

	(void)APK_SCENARIO_GetSubtitleSetting(&bEnable);

	HxLOG_Debug("bEnable : %s \n", bEnable ? "ITK_ENABLE" : "ITK_DISABLE" );

	return (bEnable ? ITK_ENABLE : ITK_DISABLE);
}

itk_control_t mheg_itk_port_miscGetADSetting(struct itk_misc_t *thiz)
{
	HBOOL	bEnable = FALSE;

	if (thiz == NULL)
	{
		HxLOG_Assert(0);
		return ITK_DISABLE;
	}

	(void)APK_OUTPUT_AUDIO_GetAudioDescription(&bEnable);

	HxLOG_Debug("bEnable : %s \n", bEnable ? "ITK_ENABLE" : "ITK_DISABLE" );

	return (bEnable ? ITK_ENABLE : ITK_DISABLE);
}

itk_pin_setting_t mheg_itk_port_miscGetPINSetting(struct itk_misc_t *thiz)
{
	DxGuidancePolicy_e		eGP = eDxGUIDANCEPOLICY_OFF;

	itk_int8_t				fPIN;

	if (thiz == NULL)
	{
		HxLOG_Assert(0);
		return 0;
	}

	if (APK_SCENARIO_GetGuidancePolicy(&eGP) != ERR_OK)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_GUIDANCE_POLICY) Error!!!\n");
	}

	printf("\033[30;43m[%s : %d] Get Pin Setting : %d \033[0m\n", __FUNCTION__, __LINE__, eGP);

	switch(eGP)
	{
		case eDxGUIDANCEPOLICY_ALLCONTENTS:
			fPIN = ITK_PIN_ALWAYS;
			break;

		case eDxGUIDANCEPOLICY_INTERNETCONTENTS:
		case eDxGUIDANCEPOLICY_MARKEDCONTENTS:
			fPIN = ITK_PIN_RESTRICTED;
			break;

		case eDxGUIDANCEPOLICY_OFF:
			fPIN = ITK_PIN_DISABLED;
			break;

		default:
			fPIN = ITK_PIN_NOT_SUPPORTED;
			break;
	}

	printf("\033[30;43m[%s : %d] fPIN : %d \033[0m\n", __FUNCTION__, __LINE__, fPIN);

	return (itk_int8_t)fPIN;
}

itk_validation_t mheg_itk_port_miscIsValidPIN(struct itk_misc_t *thiz, const char *pin)
{
#if 1
	//	Appl에서 pincode를 가져 갈수 없으니 따로 물어봐야 하는데...
	//MHEG_NOT_IMPLEMENTED;
	printf("\033[30;43m[%s : %d] Check Valid pin : %s  \033[0m\n", __FUNCTION__, __LINE__, pin);
	return ITK_NOT_VALID;
#else
	HERROR			hErr = ERR_FAIL;
	HBOOL			fValid = FALSE;
	HUINT8			szPinCode[5] = {0,};

	if (thiz == NULL)
	{
		ItkTrace(("ERROR : thiz is NULL !\n"));
		return ITK_NOT_VALID;
	}

	if (pin == NULL)
	{
		ItkTrace(("ERROR : pin is NULL !\n"));
		return ITK_NOT_VALID;
	}

#if defined(CONFIG_OP_FREESAT)
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_USER_PINCODE, (HUINT32 *)szPinCode, 0);
#else
//	hErr = MWC_PARAM_GetPinCode(szPinCode, 5);
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_PINCODE, (HUINT32 *)szPinCode, 5);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_PINCODE) Error\n");
	}
#endif

	if(hErr != ERR_OK)
	{
		strncpy(szPinCode, "0000", 4);
		szPinCode[4] = '\0';
	}

	if(strncmp(szPinCode, pin, 4) == 0)
	{
		fValid = TRUE;
	}

	ItkTrace(("szpinCode : %s, fValid : %s !!! \n", szPinCode, fValid ? "VALID" : "NOT VALID"));

	return (fValid ? ITK_VALID : ITK_NOT_VALID);
#endif
}

itk_validation_t mheg_itk_port_miscPromptForGuidance(struct itk_misc_t *thiz, const char *restriction)
{
	if (thiz == NULL)
	{
		HxLOG_Assert(thiz);
		return ITK_NOT_VALID;
	}

	PORT_ITK_PostMsg(eMHEG_MISC_PROMPT_FOR_GUIDANCE, 0, (HINT32)restriction, strlen(restriction), 0, 0);

	HxSEMT_Get(s_ulPromptForGuide);

	return (s_bPFG == TRUE) ? ITK_VALID : ITK_NOT_VALID;
}

void mheg_itk_port_miscClosePincodeForPFG(HBOOL bPFG)
{
	HxLOG_Warning("Close pincode for PFG (%d)!\n", bPFG);

	s_bPFG = bPFG;

	HxSEMT_Release(s_ulPromptForGuide);
}

static int mheg_itk_port_misc_verify_callback(int preverify, X509_STORE_CTX* x509_ctx)
{
	if (!preverify)
	{
		HCHAR data[256] = {0,};
		X509 *cert = X509_STORE_CTX_get_current_cert (x509_ctx);
		HINT32  depth = X509_STORE_CTX_get_error_depth (x509_ctx);
		HINT32  err = X509_STORE_CTX_get_error (x509_ctx);

		HxLOG_Debug("Error with certificate at depth: %i\n",depth);
		X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
		HxLOG_Debug("issuer   = %s\n",data);
		X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
		HxLOG_Debug("subject  = %s\n",data);
		HxLOG_Debug("err %i:%s\n",err, X509_verify_cert_error_string(err));
	}
	else if (preverify == 1)
	{
		HxLOG_Debug("server cert is OK.\n");
	}

	return preverify;
}

// refer  https://wiki.openssl.org/index.php/SSL/TLS_Client
static int local_check_cert(const char *psz_url)
{
	int res = 1;
	int retVal = 0 ; // 0: OK

	SSL_CTX* ctx = NULL;
	BIO *web = NULL;
	SSL *ssl = NULL;
	const SSL_METHOD* method;
	const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
	ITK_UrlData_t	stUrlField;
	X509* cert;

	memset(&stUrlField, 0xFF, sizeof(ITK_UrlData_t));
	if (NULL == psz_url)
	{
		HxLOG_Error("Null URL \r\n");
		retVal = -1;
		goto exit_veri_cert;
	}

	if(PORT_ITK_SVC_ParseUrlUntilHost((HCHAR*)psz_url, &stUrlField))
	{
		if (psz_url)
			HxLOG_Error(" invalid url (%s) \r\n", psz_url);
		memset(&stUrlField, 0xFF, sizeof(ITK_UrlData_t));
		retVal = -1;
		goto exit_veri_cert;
	}

	method = SSLv23_method();
	if((NULL == method))
	{
		HxLOG_Warning("Fail SSLv23_method !\n");
		retVal = -1;
		goto exit_veri_cert;
	}

	ctx = SSL_CTX_new(method);
	if( ctx == NULL)
	{
		HxLOG_Warning("Fail SSL_CTX_new !\n");
		retVal = -1;
		goto exit_veri_cert;
	}

	/* Cannot fail ??? */
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, mheg_itk_port_misc_verify_callback);

	/* Cannot fail ??? */
	SSL_CTX_set_verify_depth(ctx, 4);

	/* Cannot fail ??? */
	SSL_CTX_set_options(ctx, flags);

	res = SSL_CTX_load_verify_locations(ctx, NULL, ITK_MISC_CERTIFICATE_AUTHORITY_DIR);
	if(1 != res)
	{
		HxLOG_Warning("Fail SSL_CTX_load_verify_locations !\n");
		retVal = -1;
		goto exit_veri_cert;
	}

	web = BIO_new_ssl_connect(ctx);
	if( web == NULL)
	{
		HxLOG_Warning("Fail BIO_new_ssl_connect !\n");
		retVal = -1;
		goto exit_veri_cert;
	}

	if (HxSTD_StrChar(stUrlField.host, ':') == NULL) {
		/*If host doens't have port number, add default https port number(443). */
		HINT32 size_https_connt_host = HxSTD_StrLen(stUrlField.host) + HxSTD_StrLen(":") + HxSTD_StrLen(ITK_MIS_HTTPS_PORT) + 1 ; //
		HCHAR *psz_https_conn_host = HLIB_STD_MemAlloc( size_https_connt_host );
		HxSTD_sprintf(psz_https_conn_host, "%s:%s", stUrlField.host, ITK_MIS_HTTPS_PORT);
		psz_https_conn_host[size_https_connt_host-1] = '\0';
		res = BIO_set_conn_hostname(web, psz_https_conn_host);
		HxLOG_Debug("Connect hostname %s\n", psz_https_conn_host);
		HLIB_STD_MemFree(psz_https_conn_host);
	} else {
		res = BIO_set_conn_hostname(web, stUrlField.host);
		HxLOG_Debug("Connect hostname %s\n", stUrlField.host );
	}
	if(1 != res)
	{
		HxLOG_Warning("Fail BIO_set_conn_hostname !\n");
		retVal = -1;
		goto exit_veri_cert;
	}

	BIO_get_ssl(web, &ssl);
	if( ssl == NULL)
	{
		HxLOG_Warning("Fail BIO_get_ssl !\n");
		retVal = -1;
		goto exit_veri_cert;
	}

#if 0 // Todo: the TLS SNI extension is needed?
	res = SSL_set_tlsext_host_name(ssl, stUrlField.host);
	if( ssl == NULL)
	{
		HxLOG_Warning("Fail SSL_set_tlsext_host_name !\n");
		retVal = -1;
		goto exit_veri_cert;
	}
#endif

	res = BIO_do_connect(web);
	if(1 != res)
	{
		HxLOG_Warning("Fail BIO_do_connect !\n");
		retVal = -1;
		goto exit_veri_cert;
	}

	res = BIO_do_handshake(web);
	if(1 != res)
	{
		HxLOG_Warning("Fail BIO_do_handshake !\n");
		retVal = -1;
		goto exit_veri_cert;
	}

	/* Step 1: verify a server certificate was presented during the negotiation */
	cert = SSL_get_peer_certificate(ssl);
	if(cert) { X509_free(cert); } /* Free immediately */
	if(NULL == cert)
	{
		HxLOG_Warning("Fail to get a server certificate !\n");
		retVal = -1;
		goto exit_veri_cert;
	}

	/* Step 2: verify the result of chain verification */
	res = SSL_get_verify_result(ssl);
	if(X509_V_OK != res)
	{
		HxLOG_Warning("Fail SSL_get_verify_result !\n");
		retVal = -1;
		goto exit_veri_cert;
	}

exit_veri_cert:
	if(web != NULL)
		BIO_free_all(web);

	if(NULL != ctx)
		SSL_CTX_free(ctx);

	PORT_ITK_SVC_FreeUrlData(&stUrlField);

	return retVal;
}

STATIC void mheg_itk_miscReplaceStr(HUINT8* strSource, HUINT8* strBefore, HUINT8* strAfter)
{
	HUINT8 	tmp[DxRCT_MAX_IMAGE_ICON_URL_LEN+1]="", *p;
	HUINT32	lenBefore  = HxSTD_StrLen(strBefore);
	HUINT32 	lenAfter = HxSTD_StrLen(strAfter);

	p = HxSTD_StrStr(strSource, strBefore);
	if(p != NULL)
	{
		HxSTD_StrNCpy(tmp, p + lenBefore, (DxRCT_MAX_IMAGE_ICON_URL_LEN+1));
		tmp[DxRCT_MAX_IMAGE_ICON_URL_LEN] = '\0';
		HxSTD_snprintf(p, (DxRCT_MAX_IMAGE_ICON_URL_LEN+1+lenAfter), "%s%s",strAfter, tmp);
	}
}

itk_errcode_t mheg_itk_port_miscLaunchApplication(struct itk_misc_t *thiz,
                                        itk_int32_t appType,
                                        const itk_uint8_t *appReference,
                                        itk_int32_t argCount,
                                        itk_uint8_t *args[])
{
	int i;

	if (thiz == NULL)
	{
		HxLOG_Assert(0);
		return ITKE_FAIL;
	}

	HxLOG_Debug("--- appType (%d), appReference (%s), argCount (%d)\n", appType, appReference, argCount);

	HxSTD_MemSet(s_szURL, 0x00, MAX_LAUNCHAPP_URL_LENGTH);

	snprintf(s_szURL, MAX_LAUNCHAPP_URL_LENGTH, "%s", appReference);

	for (i = 0; i < argCount; i++)
	{
		snprintf(s_szURL, MAX_LAUNCHAPP_URL_LENGTH, "%s?%s", s_szURL, args[i]);
	}

#if defined(ITK_MISC_CERTIFICATE_CHECK)
	/* in the case of https, check if a server certificate is trusted or not */
	if ( ( HxSTD_StrLen(s_szURL) >= 5) && (HxSTD_StrNCmp(s_szURL, "https", 5) == 0) )
	{
		/* if the server is untrusted, then return fail. */
		if ( local_check_cert(s_szURL) < 0 )
		{
			HxLOG_Debug("Untrust webserver... \n");
			return ITKE_FAIL;
		}
	}
#endif

	PORT_ITK_PostMsg(eMHEG_MISC_LAUNCH_APPLICATION, 0, 0, 0, (HINT32)s_szURL, strlen(s_szURL));

	return ITKE_OK;
}

itk_validation_t mheg_itk_port_miscIsValidAppMimeType(struct itk_misc_t *thiz, const char *mimeType )
{
	char	*pMimeType;

	if (thiz == NULL)
	{
		HxLOG_Assert(0);
 		return ITK_NOT_VALID;
	}

	pMimeType = HLIB_STD_StrLower(mimeType);
#if 1
	// fvp-4000t는 아래의 mimetype이 valid인데, 타 모델은 추후에 확인 필요함.
	if( (HxSTD_StrCmp(pMimeType, "-1") !=0)
		&& (HxSTD_StrCmp(pMimeType, "application/vnd.hbbtv.xhtml+xml") !=0)
		&& (HxSTD_StrCmp(pMimeType, "application/vnd.dvb.ait+xml") != 0)
		&& (HxSTD_StrCmp(pMimeType, "text/html") != 0))
	{
		HxLOG_Debug("--- Not Valid : pMimeType(%s) mimeType (%s)\n", pMimeType, mimeType);
 		return ITK_NOT_VALID;
	}
#else
	// fvp-4000t는 아래의 mimetype이 valid인데, 타 모델은 추후에 확인 필요함.
	if( (HxSTD_StrCmp(mimeType, "-1") !=0)
		&& (HxSTD_StrCmp(mimeType, "application/vnd.hbbtv.xhtml+xml") !=0)
		&& (HxSTD_StrCmp(mimeType, "application/vnd.dvb.ait+xml") != 0)
		&& (HxSTD_StrCmp(mimeType, "text/html") != 0)
		&& (HxSTD_StrCmp(mimeType, "tExT/hTmL") != 0))
	{
		HxLOG_Debug("--- Not Valid : mimeType (%s)\n", mimeType);
 		return ITK_NOT_VALID;
	}
#endif
	HxLOG_Debug("---  Valid : pMimeType(%s) mimeType (%s)\n", pMimeType, mimeType);
 	return ITK_VALID; // TODO:
}

itk_errcode_t mheg_itk_port_miscQueryHostSupportString(struct itk_misc_t *thiz,
                                            const char *feature,
                                            itk_validation_t *known,
                                            itk_validation_t *supported)
{
	if (thiz == NULL)
	{
		HxLOG_Debug("ERROR : thiz is NULL !\n");
		*known = ITK_NOT_VALID;
		*supported = ITK_NOT_VALID;
		return ITK_NOT_VALID;
	}

	HxLOG_Debug("QueryHostSupportString feature [%s]\n", feature);

	*known = ITK_VALID;
	*supported = ITK_VALID;

	return ITKE_OK;
}

/* mmmcccvvv
    mmm is a manufacturer code - registered with DTG Testing Ltd,
    ccc is a manufacturer specific product code
    vvv is a version of software for the product
*/
const char* mheg_itk_port_miscGetIdString(struct itk_misc_t* thiz)
{
	static HCHAR	_szIdString[10];

	if (thiz == NULL)
	{
		HxLOG_Assert(thiz);
		return NULL;
	}

/*
	(before 2014) Humax제품간의 구분을 위해 SYSTEM ID를 Version ID로 활용한다
	1. mmm은 Humax code(manufacturer) HMX를 사용
	2. cccvvv 6자리는 SYSTEM_ID 4byte중 하위 1byte는 사용하지 않으므로 버리고, 상위 3byte를 취한다
*/
/*
	(2015) Humax 제품간의 구분을 위해 SYSTEM ID의 룰에서 MHEG whoami를 접목하여 사용.
	//ex) FVP-4000T		HMX - 8 (STB) B (지상파 with IP) 0 (model number) 000 (whoami 변경 필요시 업데이트)
	//ex) HB-2000S		HMX - 8 (STB) 9 (위성 with IP) 0 (model number) 000 (whoami 변경 필요시 업데이트)
*/
#if defined(CONFIG_PROD_HDFOXT)
	/*
		2010. 01. 31. rjlee
		Integration workshop을 위해 임시 WhoAmI 스트링 등록
	*/
	strncpy(_szIdString, "HMX260000", 10);
	_szIdString[9] = '\0';
#elif defined(CONFIG_PROD_HDRFOXT2)  // value from marketing's wonju kim (wjkim@humaxdigital.com)
	strncpy(_szIdString, "HMX7E0000", 10);
	_szIdString[9] = '\0';
#elif defined(CONFIG_PROD_HDR1000S)
	strncpy(_szIdString, "HMX809C7D", 10);
	_szIdString[9] = '\0';
#elif defined(CONFIG_PROD_HB1000S)
	strncpy(_szIdString, "HMX8D0C7D", 10);
	_szIdString[9] = '\0';
#elif defined(CONFIG_PROD_FVP4000T)
	strncpy(_szIdString, "HMX8B0000", 10);
	_szIdString[9] = '\0';
#else // TODO : for test (swkim) string 확인 필요
	snprintf(_szIdString, 10, "HMX%s", "80BC7E");
	//strncpy(_szIdString, "HMX7D1000", 10);		// test id string (HDR-Freesat)
	_szIdString[9] = '\0';
#endif

	HxLOG_Debug("Id string : %s\n", _szIdString);

	return (const HINT8*)_szIdString;
}

const char* mheg_itk_port_miscGetUniqueIdString(struct itk_misc_t* thiz)
{
#if 1
	MHEG_NOT_IMPLEMENTED;
	return NULL;
#else
	HERROR			hErr = ERR_FAIL;
	HCHAR			szMacAddr[8] = {0,};
	static char		szUniqueIdString[18] = {0,};

	if(thiz == NULL)
	{
		ItkTrace(("Input paramter is NULL \r\n"));
		return NULL;
	}

	hErr = MWC_NET_GetMacAddressOfIF(eNet_Inet_1st, szMacAddr);
	snprintf(szUniqueIdString, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
		szMacAddr[0], szMacAddr[1], szMacAddr[2], szMacAddr[3], szMacAddr[4], szMacAddr[5]);

	ItkTrace(("szUniqueIdString : %s\n", szUniqueIdString));

	return (const HINT8*)szUniqueIdString;
#endif
}

itk_date_t mheg_itk_port_miscGetDate(struct itk_misc_t* thiz)
{
	HUINT32			ulCurTime;
	HxDATETIME_t	stDateTime;
	itk_date_t		stDate;

	memset(&stDate, 0x00, sizeof(itk_date_t));

	ulCurTime = (HUINT32)time(NULL);
	if (HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurTime, &stDateTime) == ERR_OK)
	{
		if(stDateTime.stDate.usYear > 1900)
			stDate.year	= (HUINT8)(stDateTime.stDate.usYear - 1900);
		else
			stDate.year	= (HUINT8)stDateTime.stDate.usYear;			/* 1900 = 0 */

		stDate.month	= stDateTime.stDate.ucMonth - 1;	/* jan = 0 */
		stDate.day		= stDateTime.stDate.ucDay;
		stDate.hour		= stDateTime.stTime.ucHour;
		stDate.minute	= stDateTime.stTime.ucMinute;
		stDate.second	= stDateTime.stTime.ucSecond;
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_CYAN" Date(%d/%d/%d) Time(%d:%d:%d)"C_CODE_RESET"\n",
				stDate.year,
				stDate.month,
				stDate.day,
				stDate.hour,
				stDate.minute,
				stDate.second);

	return stDate;
}


itk_uint32_t mheg_itk_port_miscGetUpTime(struct itk_misc_t* thiz)
{
	if (thiz == NULL)
	{
		HxLOG_Assert(0);
		return 0;
	}

	return HLIB_STD_GetSystemTick();
}

itk_misc_t*	mheg_itk_port_miscGetObject(void)
{
	return &s_stMiscObj;
}

itk_errcode_t mheg_itk_port_miscGetBannerSettings(struct itk_misc_t *thiz, itk_int32_t *transparency, itk_int32_t *timeOut, char *language)
{
	if (thiz == NULL)
	{
		HxLOG_Assert(0);

 		return ITK_NOT_VALID;
	}

	// TODO: ZA Profile (out param : transparency, timeOut, language)

	return ITKE_OK;
}

static void	mheg_itk_port_miscEventCallback (void *context, itk_user_setting_t setting)
{
	if (context != NULL)
	{
		HxLOG_Debug(" setting : %d\n", setting);
	}

	if (g_misc_user_setting_callback)
	{
		g_misc_user_setting_callback(g_misc_user_setting_callbackContext, setting);
	}
}

static void	mheg_itk_port_miscExcuteEventCallback (void *context, itk_user_setting_t setting)
{
	HxLOG_Debug(" setting : %d\n", setting);

	mheg_itk_port_miscEventCallback(context, setting);
}

#define _____PUBLIC_APIs________________________________________________________________

void PORT_ITK_MISC_CreateObject(redkey_profile_t eItkProfile)
{
	HxLOG_Debug(C_CODE_F_WHITE_B_BLUE"HTTP Object Profile (%d) !!!"C_CODE_RESET"\n", eItkProfile);

	s_stMiscObj.getDate				= mheg_itk_port_miscGetDate;
	s_stMiscObj.getUptime				= mheg_itk_port_miscGetUpTime;
	s_stMiscObj.getIdString				= mheg_itk_port_miscGetIdString;
	s_stMiscObj.getUniqueIdString		= mheg_itk_port_miscGetUniqueIdString;
	s_stMiscObj.getPreferredLanguage	= mheg_itk_port_miscGetPreferredLanguage;
	s_stMiscObj.setPreferredLanguage	= NULL;
	s_stMiscObj.addSettingsCallback		= mheg_itk_port_miscAddSettingCallback;
	s_stMiscObj.removeSettingsCallback	= mheg_itk_port_miscRemoveSettingCallback;
	s_stMiscObj.getSubtitleSetting 		= mheg_itk_port_miscGetSubtitleSetting;
	s_stMiscObj.getADSetting 			= mheg_itk_port_miscGetADSetting;
	s_stMiscObj.getPINSetting 			= mheg_itk_port_miscGetPINSetting;
	s_stMiscObj.isValidPIN				= mheg_itk_port_miscIsValidPIN;
	s_stMiscObj.promptForGuidance		= mheg_itk_port_miscPromptForGuidance;
	s_stMiscObj.launchApplication		= mheg_itk_port_miscLaunchApplication;
	s_stMiscObj.isValidAppMimeType		= mheg_itk_port_miscIsValidAppMimeType;
	s_stMiscObj.getBannerSettings		= mheg_itk_port_miscGetBannerSettings;
	s_stMiscObj.queryHostSupportString	= mheg_itk_port_miscQueryHostSupportString;

	// according to each profile, set NULL to usused function
	if (eItkProfile == REDKEY_PROFILE_UK_HD)
	{
		s_stMiscObj.getUniqueIdString = NULL;
		s_stMiscObj.isValidPIN = NULL;
		s_stMiscObj.getBannerSettings = NULL;
	}
	else if (eItkProfile == REDKEY_PROFILE_INTERNAL5)
	{
		s_stMiscObj.getUniqueIdString = NULL;
		s_stMiscObj.isValidPIN = NULL;
		s_stMiscObj.getBannerSettings = NULL;
	}
	else if ( (eItkProfile == REDKEY_PROFILE_UK_FSAT)
		|| (eItkProfile == REDKEY_PROFILE_FREESATG2) )
	{
		s_stMiscObj.promptForGuidance = NULL;
		s_stMiscObj.getBannerSettings = NULL;
	}
	else if  (eItkProfile == REDKEY_PROFILE_CIPLUS)
	{
		s_stMiscObj.getUniqueIdString = NULL;
		s_stMiscObj.addSettingsCallback = NULL;
		s_stMiscObj.removeSettingsCallback	= NULL;
		s_stMiscObj.getSubtitleSetting 		= NULL;
		s_stMiscObj.getADSetting 			= NULL;
		s_stMiscObj.getPINSetting 			= NULL;
		s_stMiscObj.isValidPIN				= NULL;
		s_stMiscObj.promptForGuidance		= NULL;
		s_stMiscObj.launchApplication		= NULL;
		s_stMiscObj.isValidAppMimeType	= NULL;
		s_stMiscObj.getBannerSettings		= NULL;
	}

	ITK_registerManager(ITK_MISC, (void*)&s_stMiscObj);

	HxSEMT_Create(&s_ulPromptForGuide, "itkPFG", HxSEMT_FIFO);
	HxSEMT_Get(s_ulPromptForGuide);

	APK_SYSINFO_STB_GetSystemId(s_szSystemId, MAX_SYSTEMID_LEN);
}


void PORT_ITK_MISC_NotifySubtitleChangedEvent(void)
{
	mheg_itk_port_miscExcuteEventCallback(NULL, ITK_USER_SETTING_SUBTITLES);
}

void PORT_ITK_MISC_NotifyAudioDescriptionChangedEvent(void)
{
	mheg_itk_port_miscExcuteEventCallback(NULL, ITK_USER_SETTING_AUDIO_DESC);
}

