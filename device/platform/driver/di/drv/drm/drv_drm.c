/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   drv_drm  $
 * $Modtime:   Mar 16 2012 17:20:04  $
 *
 * Author:
 * Description:
 *
 *                                 Copyright (c) 2012 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/



/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include  common utils headers */
#include "htype.h"
#include "linden_trace.h"
#include "vkernel.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif


/* include  drv headers */
#include "di_crypt.h"
#include "drv_crypt.h"
#include "drv_drm.h"

#if defined (CONFIG_PLAYREADY)
#include "nexus_playpump.h"
#include "drmmanager.h"
#include "prdy_http.h"
#include "nexus_memory.h"
#include "nexus_security.h"
#include "util_url.h"
#include <drmutf.h>
#include <drmxmlparser.h>
#include <drmsecureclockstatus.h>
#include <drmsecureclockconstants.h>
#include "nexus_dma.h"
#include "bmp4_util.h"
#include "common_crypto.h"
#include "nexus_base_os.h"
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
#include "oem_brcm.h"
#endif
#if (NEXUS_VERSION >= 1290) /* for playready */
#include "basf_stream.h"
#endif

#endif

#if defined (CONFIG_PLAYREADY_25_BRCM)
#include "drm_prdy_types.h"
#include "drm_prdy_http.h"
#include "drm_prdy.h"
#include "nexus_memory.h"
#include "common_crypto.h"
#include "nexus_base_os.h"
#include "bkni.h"
#endif

#if defined (CONFIG_VIEWRIGHTWEB)
#include "ViewRightWebWrapper.h"
#endif

#if defined (CONFIG_MARLIN_DRM)
#include "nexus_message.h"
#include "nexus_playback.h"
#include "MarlinWrapper.h"
#include "bmpeg2ts_probe.h"
#include "string.h"
#endif

#include "nexus_platform_features.h"
#include "nexus_memory.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE			TRACE_MODULE_DRV_DRM

#define MAX_DRM_SESSION 		64

#ifndef UNUSED_PARAM
#define UNUSED_PARAM(x)  ((void)x)
#endif

#if defined (CONFIG_PLAYREADY_25_BRCM)
#define MAX_WRMHEADER_SIZE	2048
#define MAX_RESPONSE_SIZE	(128*1024) /*124K*/
#endif

#if defined (CONFIG_PLAYREADY)
#define SC_PROTOCOL_STRING "drm_clock_acquire_redirect: "

#define HDR_LICGET_ACTION_VALUE 		"\"http://schemas.microsoft.com/DRM/2007/03/protocols/AcquireLicense\""
#define HDR_LICACK_ACTION_VALUE 		"\"http://schemas.microsoft.com/DRM/2007/03/protocols/AcknowledgeLicense\""

#if defined(CONFIG_PLAYREADY_25)
const DRM_WCHAR rgwchStoreName [] =
{
	ONE_WCHAR('/','\0'),ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('y','\0'),ONE_WCHAR('r','\0'), ONE_WCHAR('e','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('d','\0'),ONE_WCHAR('y','\0'), ONE_WCHAR('2','\0'), ONE_WCHAR('5','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e', '\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')
};
#else
/*	var/lib/humaxtv/drm/sample.hd	*/
const DRM_WCHAR rgwchStoreName [] =
{
	ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e', '\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('s', '\0')
};
#endif
#endif

typedef struct PLAYREADY_DATA {
	/* 필요한 내용 추가 필요함 */
#if defined (CONFIG_PLAYREADY)
	DRM_DECRYPT_CONTEXT  pDecryptContext;
	HBOOL bInclusionListExist;
	NEXUS_DmaHandle dmaHandle;
	CommonCryptoHandle cryptoHandle;
	NEXUS_KeySlotHandle keyWmdrm;
	NEXUS_KeySlotHandle keyPlayready;
	HUINT8* pEkl;
#if defined(CONFIG_PLAYREADY_25)
	HBOOL bSetContext;
	DRM_APP_CONTEXT* pAppContext;
	DRM_VOID *f_pOEMContext;
#endif
#elif defined(CONFIG_PLAYREADY_25_BRCM)
	DRM_Prdy_Init_t     prdyParamSettings;
	DRM_Prdy_Handle_t   drm_context;
	DRM_Prdy_DecryptContext_t decryptor;
	CommonCryptoHandle commonCryptoHandle;
#else
	HUINT8* private;
#endif
} Playready_data_t;

typedef struct VIEWRIGHTWEB_DATA {
	HUINT8		szStorePath[128];
	HUINT8		szCompanyName[128];
	HBOOL		bVCASConnectAndProvision;
	HBOOL		bInit;
	/* 필요한 내용 추가 필요함 */
} ViewRightWeb_data_t;

typedef struct MARLIN_DATA {
#if defined (CONFIG_MARLIN_DRM)
	WSB_DRM pHMarlin;

	HINT8 *MessageURL;
	HINT8 *ContentAccessURL;


	void *pstPlayback;

	HBOOL	bFirstKey;
	HBOOL	bOddKey;
	HINT8	EvenKey[16];
	HINT8	OddKey[16];
	HINT8	CurIv[16];
	HINT8	NextIv[16];
	NEXUS_KeySlotHandle videoKeyHandle;
	NEXUS_KeySlotHandle audioKeyHandle;
	HBOOL	bRewind;

	HUINT32 					ca_section_size;
	HUINT32 					pmt_section_size;
	HUINT8						ca_section_buf[4093];	/* ECM should not be exceed 4093 */
	HUINT8						pmt_section_buf[1021];	/* PMT should not be exceed 1021 */
	HINT32						max_bitrate;
#else
	HUINT8* private;
#endif
} Marlin_data_t;

typedef struct DRM_SESSION {
	HBOOL used;

 	SESSION_HANDLE media_session_handle;
	DRM_Info_t drm_info;

	union {
		Playready_data_t playready;
		ViewRightWeb_data_t viewrightweb;
		Marlin_data_t marlin;
	} private;
} DRM_Session_t;

#if defined (CONFIG_MARLIN_DRM)
#define AES_KEY_SIZE 16
#ifndef PAT_PID
#define PAT_PID 0
#endif
#endif

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
DRM_Session_t s_DRMSession[MAX_DRM_SESSION];
DRM_Type s_defaultDRMType = DI_DRM_TYPE_VIEWRIGHTWEB;

/*******************************************************************/
/****************** Static Function Prototypes *********************/
/*******************************************************************/
#if defined (CONFIG_PLAYREADY_25_BRCM)
static int			P_DRM_initSecureClock(DRM_Prdy_Handle_t drm);
static DRV_Error	P_DRM_PlayReady_BRCM_GetLicense(SESSION_HANDLE media_session_handle, HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize);
static DRV_Error	P_DRM_PlayReady_BRCM_PushPumpBuffer(SESSION_HANDLE media_session_handle, void *pvDst, void *pvSrc, HUINT32 size, HBOOL bflush);
#endif

DRM_Session_t *P_DRM_NewInstance(void)
{
	int i;

	for(i=0; i<MAX_DRM_SESSION; i++)
	{
		if(!s_DRMSession[i].used)
		{
			VK_MEM_Memset(&s_DRMSession[i], 0x0, sizeof(DRM_Session_t));
			return &s_DRMSession[i];
		}
	}

	return NULL;
}

DRM_Session_t *P_DRM_GetInstance(SESSION_HANDLE media_session_handle)
{
	int i;

	for(i=0; i<MAX_DRM_SESSION; i++)
	{
		if(s_DRMSession[i].used)
		{
			if(media_session_handle == s_DRMSession[i].media_session_handle)
			{
				return &s_DRMSession[i];
			}
		}
	}

	return NULL;
}
static void P_DRM_DMA_CompletedCallback(void *context, int param)
{
	unsigned long		vkEvtId = (unsigned long)context;
	unsigned long		vkEvt = (unsigned long)param;
	int					vkRet;

	vkRet = VK_EVENT_Send(vkEvtId, vkEvt);
	if (VK_OK != vkRet)
	{
		PrintError("%s VK_EVENT_Send error\n", __FUNCTION__);
	}
}

#if defined (CONFIG_PLAYREADY)
HINT8* P_DRM_AddMaxdomeCookie(SESSION_HANDLE pHSession, HINT8* pUrl)
{
	DRM_Session_t *drm_handle=NULL;
	HINT32 new_string_size = 0;
	HINT8 *man = NULL;
	int nRet;

	if(pHSession == NULL)
	{
		PrintError("[%s] pHSession is NULL!\n", __FUNCTION__);
		return NULL;
	}

	drm_handle = P_DRM_GetInstance(pHSession);
	if(drm_handle == NULL)
	{
		PrintError("There's no matching drm_handle!\n");
		return NULL;
	}

	if (drm_handle->drm_info.cbSetLicenseData)
	{
		drm_handle->drm_info.cbSetLicenseData(pHSession, pUrl, 0);
		if (drm_handle->drm_info.pucLicenseData == NULL)
		{
			PrintError("pucLicenseData is NULL!!\n\n\n");
			return NULL;
		}

		new_string_size = VK_strlen(pUrl) + VK_strlen(drm_handle->drm_info.pucLicenseData) + 1 + 1;

		man = Oem_MemAlloc(new_string_size);
		if(man == NULL)
		{
			PrintError("Error while malloc new url in %s(%d)\n", __FUNCTION__, __LINE__);
			return NULL;
		}

		nRet = VK_snprintf ((char*)man ,new_string_size, "%s&%s", pUrl, drm_handle->drm_info.pucLicenseData);
		if(nRet<0)
		{
			PrintError("can't make new url in %s(%d)\n", __FUNCTION__, __LINE__);
			return NULL;
		}
	}

	return man;
}

DRM_RESULT PlayReadyBindCallback(void *f_pvPolicyCallbackData, DRM_DWORD  f_dwCallbackType, void *f_pv)
{
	DRM_Session_t *drm_handle;
	OutputControl_t stOPL;

	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)f_pv);
	if(drm_handle == NULL)
	{
		PrintError("There's no matching drm_handle!\n");
		return DRM_S_FALSE;
	}

	if(f_dwCallbackType == DRM_PLAY_OPL_CALLBACK)
	{
		PrintError("PLAY OPL!\n");
		if(drm_handle->drm_info.OutputControl != NULL)
		{
			stOPL.drm_type = DI_DRM_TYPE_PLAYREADY;
			VK_MEM_Memcpy(&stOPL.data.playready, f_pvPolicyCallbackData, sizeof(Playready_OutputControl_t));
			drm_handle->drm_info.OutputControl((SESSION_HANDLE)f_pv, &stOPL);
		}
	}
	else if(f_dwCallbackType == DRM_COPY_OPL_CALLBACK)
	{
		PrintError("COPY OPL!\n");
	}
	else if(f_dwCallbackType == DRM_INCLUSION_LIST_CALLBACK)
	{
		PrintError("INCLUSION OPL!\n");
		if(((DRM_INCLUSION_LIST_CALLBACK_STRUCT*)f_pvPolicyCallbackData)->dwChainDepth != 0)
		{
			drm_handle->private.playready.bInclusionListExist = TRUE;
		}
		else
		{
			drm_handle->private.playready.bInclusionListExist = FALSE;
		}
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK)
	{
		PrintError("CONDITION OPL!\n");
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK)
	{
		PrintError("ACTION OPL!\n");
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK)
	{
		PrintError("QUERY OPL!\n");
	}
	else if(f_dwCallbackType == DRM_SECURE_STATE_TOKEN_RESOLVE_CALLBACK)
	{
		PrintError("RESOLVE OPL!\n");
	}
	else
	{
		PrintError("Invalid Callback Type!\n");
	}
	return DRM_SUCCESS;
}

DRM_RESULT PlayReadyCommitCallback(void *f_pvPolicyCallbackData, DRM_DWORD  f_dwCallbackType,void *f_pv)
{
	f_pvPolicyCallbackData = f_pvPolicyCallbackData;
	f_pv = f_pv;

	if(f_dwCallbackType == DRM_PLAY_OPL_CALLBACK)
	{
		PrintError("PLAY OPL!\n");
	}
	else if(f_dwCallbackType == DRM_COPY_OPL_CALLBACK)
	{
		PrintError("COPY OPL!\n");
	}
	else if(f_dwCallbackType == DRM_INCLUSION_LIST_CALLBACK)
	{
		PrintError("INCLUSION OPL!\n");
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK)
	{
		PrintError("CONDITION OPL!\n");
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK)
	{
		PrintError("ACTION OPL!\n");
	}
	else if(f_dwCallbackType == DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK)
	{
		PrintError("QUERY OPL!\n");
	}
	else if(f_dwCallbackType == DRM_SECURE_STATE_TOKEN_RESOLVE_CALLBACK)
	{
		PrintError("RESOLVE OPL!\n");
	}
	else
	{
		PrintError("Invalid Callback Type!\n");
	}
	return DRM_SUCCESS;
}

DRM_CONTENT_SET_PROPERTY P_DRM_PlayReady_CheckHeaderVersion(HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize)
{
	HUINT32 uiOffset=0;
	DRM_CONTENT_SET_PROPERTY eVersion=DRM_CSP_HEADER_NOT_SET;

/*************************
* WRM Validation check
*************************/
	if(uiWrmHeaderSize < 60)
	{
		PrintError("Invalid WRM HEADER! too small!\n");
		return eVersion;
	}

/*************************
* WRMHEADER tag with version info
*************************/
	while(uiOffset <= (uiWrmHeaderSize-18))
	{
		if((pucWrmHeader[uiOffset] == 'v') && (pucWrmHeader[uiOffset+2] == 'e') && (pucWrmHeader[uiOffset+4] == 'r') &&
			(pucWrmHeader[uiOffset+6] == 's') && (pucWrmHeader[uiOffset+8] == 'i') && (pucWrmHeader[uiOffset+10] == 'o') && (pucWrmHeader[uiOffset+12] == 'n'))
		{
			break;
		}
		uiOffset++;
	}

	if(pucWrmHeader[uiOffset+18] == '2')
	{
		eVersion = DRM_CSP_V2_HEADER;
	}
#if defined(CONFIG_PLAYREADY_25)
	else if(pucWrmHeader[uiOffset+18] == '4' && pucWrmHeader[uiOffset+22] == '1')
	{
		eVersion = DRM_CSP_V4_1_HEADER;
	}
#endif
	else if(pucWrmHeader[uiOffset+18] == '4')
	{
		eVersion = DRM_CSP_V4_HEADER;
	}
	else
	{
		PrintError("Invalid version!\n");
	}

	return eVersion;
}

DRV_Error P_DRM_CreateCrypto(SESSION_HANDLE media_session_handle)
{
	NEXUS_Error nResult;
	DRM_Session_t *drm_handle=NULL;
	NEXUS_DmaSettings dmaSettings;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	NEXUS_SecurityAlgorithmSettings nexusConfig;
	CommonCryptoSettings cryptoSettings;

	if(media_session_handle == NULL)
	{
		PrintError("invalid parameter!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		return DRV_ERR;
	}

/*************************
*	1. CryptoHandle Open
*************************/
	NEXUS_Memory_Allocate(80, NULL, (void **)&(drm_handle->private.playready.pEkl));
	VK_MEM_Memset(&dmaSettings, 0, sizeof(NEXUS_DmaSettings));

	drm_handle->private.playready.dmaHandle = NEXUS_Dma_Open(0, &dmaSettings);
	if(drm_handle->private.playready.dmaHandle == NULL)
	{
		PrintError("NEXUS_Dma_Open failed!\n");
		return DRV_ERR;
	}
	CommonCrypto_GetDefaultSettings(&cryptoSettings);
	drm_handle->private.playready.cryptoHandle = CommonCrypto_Open(&cryptoSettings);

/*************************
*	2. RC4 key Handle Open
*************************/
	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

	drm_handle->private.playready.keyWmdrm = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if(drm_handle->private.playready.keyWmdrm == NULL)
	{
	   PrintError("NEXUS_Security_AllocateKeySlot failed\n");
	   return DRV_ERR;
	}

    NEXUS_Security_GetDefaultAlgorithmSettings(&nexusConfig);

    nexusConfig.algorithm           = NEXUS_SecurityAlgorithm_eWMDrmPd;
    nexusConfig.algorithmVar        = NEXUS_SecurityAlgorithmVariant_eEcb;
    nexusConfig.operation           = NEXUS_SecurityOperation_eDecrypt;
    nexusConfig.terminationMode     = NEXUS_SecurityTerminationMode_eClear;
    nexusConfig.enableExtKey        = TRUE;
    nexusConfig.enableExtIv         = TRUE;

    nResult = NEXUS_Security_ConfigAlgorithm(drm_handle->private.playready.keyWmdrm, &nexusConfig);
    if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ConfigAlgorithm failed!\n");
        return DRV_ERR;
    }

/*************************
*	3. AES Ctr key handle Open
*************************/
	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

	drm_handle->private.playready.keyPlayready = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
	if(drm_handle->private.playready.keyPlayready == NULL)
	{
		PrintError("NEXUS_Security_AllocateKeySlot failed\n");
		return DRV_ERR;
	}

	NEXUS_Security_GetDefaultAlgorithmSettings(&nexusConfig);

	nexusConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes128;
	nexusConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCounter;
	nexusConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
	nexusConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
	nexusConfig.enableExtKey		= TRUE;
	nexusConfig.enableExtIv 		= TRUE;
	nexusConfig.aesCounterSize		= NEXUS_SecurityAesCounterSize_e64Bits;

	nResult = NEXUS_Security_ConfigAlgorithm(drm_handle->private.playready.keyPlayready, &nexusConfig);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Security_ConfigAlgorithm failed!\n");
		return DRV_ERR;
	}

#if defined(CONFIG_PLAYREADY_25)
	drm_handle->private.playready.bSetContext = FALSE;
	drm_handle->private.playready.pAppContext = NULL;
	drm_handle->private.playready.f_pOEMContext = NULL;
#endif

	return DRV_OK;
}

DRV_Error P_DRM_DestroyCrypto(SESSION_HANDLE media_session_handle)
{
	DRM_Session_t *drm_handle=NULL;

	if(media_session_handle == NULL)
	{
		PrintError("invalid parameter!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		return DRV_ERR;
	}
#if defined(CONFIG_PLAYREADY_25)
	if(drm_handle->private.playready.bSetContext)
	{
		Drm_Reader_Close(&(drm_handle->private.playready.pDecryptContext));
		drm_handle->private.playready.bSetContext = FALSE;
	}
	if(drm_handle->private.playready.pAppContext != NULL)
	{
		Drm_Uninitialize(drm_handle->private.playready.pAppContext);
		Oem_MemFree(drm_handle->private.playready.pAppContext);
	}
	if(drm_handle->private.playready.f_pOEMContext != NULL)
	{
		Oem_Destroy(drm_handle->private.playready.f_pOEMContext);
	}
#endif
	if(drm_handle->private.playready.keyWmdrm)
	{
		NEXUS_Security_FreeKeySlot(drm_handle->private.playready.keyWmdrm);
	}

	if(drm_handle->private.playready.keyPlayready)
	{
		NEXUS_Security_FreeKeySlot(drm_handle->private.playready.keyPlayready);
	}

	if(drm_handle->private.playready.dmaHandle)
	{
		NEXUS_Dma_Close(drm_handle->private.playready.dmaHandle);
	}

	if(drm_handle->private.playready.cryptoHandle)
	{
		CommonCrypto_Close(drm_handle->private.playready.cryptoHandle);
	}

	NEXUS_Memory_Free(drm_handle->private.playready.pEkl);
	drm_handle->private.playready.pEkl = NULL;

	return DRV_OK;
}

/** * This is the magic buffer size that causes problems * with the above document */
#define BUFFER_SIZE     16383
#define MAX_URL_SIZE	1024
#define MAX_RCVBUFFER_SIZE (64*1024)

int32_t  bhttpclient_send_request(bhttp_engine* http, const char *method, char *urlstr);
const char* bhttpengine_get_header(bhttp_engine* http, const char* name);

int get_secure_clock_url(char *secure_url, char *buf)
{
	URL_t *pTmpUrl=NULL;
	bhttp_engine http;
	int32_t ret;
	int32_t len = 0;
	int32_t resp_len = MAX_RCVBUFFER_SIZE;

	pTmpUrl = (URL_t*) Oem_MemAlloc (sizeof(URL_t));
	if(pTmpUrl == NULL)
	{
		PrintError ("%s(%d) Error> can't allocate URL_t. \n",__FUNCTION__,__LINE__);
		return DRV_ERR;
	}

	VK_memset(pTmpUrl, 0x0 ,sizeof(URL_t));
	UTIL_UrlParse (pTmpUrl, (char*)secure_url, 0);

#if 1
	PrintDebug ("[URL_t] psz_protocol  = [%s]\n",pTmpUrl->psz_protocol);
	PrintDebug ("[URL_t] psz_username  = [%s]\n",pTmpUrl->psz_username);
	PrintDebug ("[URL_t] psz_password  = [%s]\n",pTmpUrl->psz_password);
	PrintDebug ("[URL_t] psz_host      = [%s]\n",pTmpUrl->psz_host);
	PrintDebug ("[URL_t] i_port        = [%d]\n",pTmpUrl->i_port);
	PrintDebug ("[URL_t] psz_path      = [%s]\n",pTmpUrl->psz_path);
	PrintDebug ("[URL_t] psz_option    = [%s]\n",pTmpUrl->psz_option);
	PrintDebug ("[URL_t] psz_buffer    = [%s]\n",pTmpUrl->psz_buffer);
	PrintDebug ("[URL_t] psz_cookie    = [%s]\n",pTmpUrl->psz_cookie);
#endif

	/* initialize http engine and get */
	VK_MEM_Memset(&http, 0x0, sizeof(bhttp_engine));
	bhttpengine_init(&http);

	ret = bhttpclient_send_request(&http, "GET", secure_url);
	if (ret != 0)
	{
		PrintError("failed on bhttpclient_send_request\n");
		Oem_MemFree(pTmpUrl);
		return DRV_ERR;
	}

	/* set headers, read response */
	bhttpengine_set_headers(&http, "User-Agent", "Client-User-Agent");
	bhttpengine_write(&http, buf, len); bhttpengine_flush(&http);
	if (bhttpclient_read_responsehdr(&http)) {
		PrintError("failed on readResponseHeader\n");
		Oem_MemFree(pTmpUrl);
		return DRV_ERR;
	}

	/* look for a secure clock url */
	bzero(buf, resp_len);
	len = bhttpengine_read(&http, buf, resp_len);
	bhttpengine_headers_cleanup(&http);
	bhttpengine_close(&http);
#if 1
	PrintDebug("HTTP default get:: resp_len <%d>, buf<%s>\n", len, buf);
#endif
	DRM_BYT_MoveBytes( buf, 0, buf, DRMCRT_strlen( SC_PROTOCOL_STRING ), len - DRMCRT_strlen( SC_PROTOCOL_STRING ) );
	PUT_CHAR( buf, len - DRMCRT_strlen( SC_PROTOCOL_STRING ), '\0' );

	PrintDebug("buf : %s\n", buf);

	VK_snprintf(secure_url, MAX_URL_SIZE-1, "%s", buf);

	Oem_MemFree(pTmpUrl);

	return DRV_OK;
}

int get_secure_clock(DRM_APP_CONTEXT *pAppContext, DRM_CHAR *pszChallenge, char *rgchURL)
{ /* get secure clock url*/
	DRM_RESULT dResult;
	DRM_RESULT		 drResponse 	= DRM_SUCCESS;

	bhttp_engine http;
	int32_t ret;
	int32_t len = 0;
	int32_t resp_len = MAX_RCVBUFFER_SIZE;
	char *secure_url = NULL;
	const char* clstr = NULL;
	char* buf = Oem_MemAlloc((DRM_DWORD)resp_len + 1);

	if (buf == NULL) return DRV_ERR;

	/* append drm specific tokens */
	PrintDebug("getting to : %s\n", rgchURL);

	/* initialize http engine and get */
	VK_MEM_Memset(&http, 0x0, sizeof(bhttp_engine));
	bhttpengine_init(&http);
	ret = bhttpclient_send_request(&http, "GET", rgchURL);
	if (ret != 0)
	{
		PrintError("failed on bhttpclient_send_request\n");
		goto error_2;
	}

	/* set headers, read response */
	bhttpengine_set_headers(&http, "User-Agent", "Client-User-Agent");
	bhttpengine_write(&http, buf, len); bhttpengine_flush(&http);
	if (bhttpclient_read_responsehdr(&http)) {
		PrintError("failed on readResponseHeader\n");
		goto error_2;
	}

	/* look for a secure clock url */
	VK_MEM_Memset(buf, 0x0, resp_len + 1);
	len = bhttpengine_read(&http, buf, resp_len);
#if 1
	PrintDebug("HTTP default get:: resp_len <%d>, buf<%s>\n", len, buf);
#endif

	clstr = bhttpengine_get_header((bhttp_engine*)&http, "Location");
	if(clstr == NULL)
	{
		PrintError("Location is not found\n");
		goto error_2;
	}

	secure_url = (char *) Oem_MemAlloc( MAX_URL_SIZE );
	VK_memset(secure_url, 0x0, MAX_URL_SIZE);

	VK_snprintf(secure_url, MAX_URL_SIZE-1, "%s", clstr);
	len = 0;

	bhttpengine_headers_cleanup(&http);
	bhttpengine_close(&http);

	if(get_secure_clock_url(secure_url, buf) != DRV_OK)
	{
		goto error_1;
	}


	PrintDebug("posting to : %s\n", secure_url);

	/* initialize http engine and post */
	bhttpengine_init(&http);
	if ((ret = bhttpclient_post(&http, (char*)secure_url)) != 0) {
		PrintError("bhttp_license_post failed on POST\n");
		goto error_1;
	}

	len = 0;

	len += VK_snprintf(buf + len, resp_len -1 ,"%s", pszChallenge);

	/* set headers, read response */
	bhttpengine_set_headers(&http, "Accept", "Accept: */*");
	bhttpengine_set_headers(&http, "Content-Type", "application/x-www-form-urlencoded");
	bhttpengine_set_header(&http,  "Content-Length", len);
	bhttpengine_set_headers(&http, "User-Agent", "Client-User-Agent");
	bhttpengine_set_headers(&http, "Proxy-Connection", "Keep-Alive");
	bhttpengine_set_headers(&http, "Pragma", "no-cache");
	bhttpengine_set_headers(&http, "Cache-Control", "no-cache");

	bhttpengine_write(&http, buf, len); bhttpengine_flush(&http);
	if (bhttpclient_read_responsehdr(&http)) {
		PrintError("failed on readResponseHeader\n");
		goto error_1;
	}

	/* look for a license */
	VK_MEM_Memset(buf, 0x0, resp_len + 1);
	len = bhttpengine_read(&http, buf, resp_len);
	bhttpengine_headers_cleanup(&http);
	bhttpengine_close(&http);
#if 1
	PrintDebug("HTTP default post:: resp_len <%d>, buf<%s>\n", len, buf);
#endif

	 dResult = Drm_SecureClock_ProcessResponse( pAppContext,
										  (DRM_BYTE *)buf,
										  len,
										 &drResponse );

	if ( drResponse != DRM_SUCCESS )
	{
		PrintError( "Response contains error %d\n", drResponse );
		goto error_1;
	}
	if ( dResult != DRM_SUCCESS )
	{
		PrintError( "Error Processing response %d\n", dResult );
		goto error_1;
	}
	else
	{
		/* if dr is SUCCESS then report drResponse */

	}
	Oem_MemFree((DRM_VOID *)secure_url);
	Oem_MemFree(buf);

	return DRV_OK;

error_1:
	Oem_MemFree((DRM_VOID *)secure_url);
error_2:
	Oem_MemFree(buf);
	return DRV_ERR;
}

static int32_t P_DRM_PlayReady_GetSecureClock(void *pContext)
{
	DRV_Error ret = DRV_OK;
	DRM_RESULT dResult;
/*1*/
	DRM_WCHAR		*pwszSecTime		= NULL;
	DRM_DWORD		 cchSecTime 		= 0;
	DRM_DWORD		 dwFlag 			= 0;
	DRM_BYTE		*pbTimeStatus		= NULL;
	DRM_DWORD		 cbTimeStatus		= 0;
	DRM_DWORD		 cchChallenge		= 0;
	DRM_DWORD		 cchURL 			= 0;
	DRM_CHAR		*pszChallenge		= NULL;
	DRM_WCHAR		*pwszURL			= NULL;

/*2*/
	DRM_CONST_STRING dstrChallenge	 = EMPTY_DRM_STRING;
	DRM_CONST_STRING dstrTemp		 = EMPTY_DRM_STRING;
	DRM_CONST_STRING dstrData		 = EMPTY_DRM_STRING;
	DRM_CONST_STRING dstrURL		 = EMPTY_DRM_STRING;
	DRM_CHAR		 *rgchURL		 = NULL;
	DRM_BYTE		*pbTemp 		 = NULL;
	DRM_DWORD		 cbTemp 		 = 0;
	DRM_SUBSTRING	 dasstrChallenge = EMPTY_DRM_SUBSTRING;
	DRM_APP_CONTEXT* pAppContext=NULL;

	pAppContext = (DRM_APP_CONTEXT*)pContext;
/*1*/
	dResult = Drm_SecureClock_GetValue( pAppContext, pwszSecTime, &cchSecTime, &dwFlag, pbTimeStatus, &cbTimeStatus );

	if ( dResult != DRM_E_BUFFERTOOSMALL )
	{
		PrintError("dResult != DRM_E_BUFFERTOOSMALL, dResult = %x\n", dResult);
		return DRV_ERR;
	}

	Drm_SecureClock_GetValue( pAppContext, pwszSecTime, &cchSecTime, &dwFlag, pbTimeStatus, &cbTimeStatus );

	if ( DRM_CLK_NOT_SET == dwFlag || DRM_CLK_NEEDS_REFRESH == dwFlag )
	{
		dResult = Drm_SecureClock_GenerateChallenge( pAppContext,
												NULL,
											   &cchURL,
												NULL,
											   &cchChallenge );

		if( dResult != DRM_E_BUFFERTOOSMALL )
		{
			PrintError("dResult != DRM_E_BUFFERTOOSMALL\n");
			return DRV_ERR;
		}
		pszChallenge = (DRM_CHAR *)  Oem_MemAlloc(cchChallenge);
		VK_memset(pszChallenge, 0x0, cchChallenge);
		pwszURL 	 = (DRM_WCHAR *) Oem_MemAlloc(cchURL * SIZEOF (DRM_WCHAR));
		VK_memset(pwszURL, 0x0, cchURL * SIZEOF (DRM_WCHAR));
		dResult = Drm_SecureClock_GenerateChallenge( pAppContext,
												  pwszURL,
												 &cchURL,
												  (DRM_BYTE *) pszChallenge,
												 &cchChallenge);
//		VK_printf("cchURL = %d\n", cchURL);
//		VK_printf("pwszURL = %s\n", pwszURL);
		PrintDebug("pszChallenge = %s\n", pszChallenge);
	}
	else
	{
		PrintError("! (DRM_CLK_NOT_SET == dwFlag || DRM_CLK_NEEDS_REFRESH == dwFlag)\n");
		return DRV_OK;
	}

/*2*/

	/* Decode base64 encoded challenge */
   pbTemp = (DRM_BYTE *) Oem_MemAlloc( cchChallenge );
   VK_memset(pbTemp, 0x0, cchChallenge);
   cbTemp  = cchChallenge;
   dasstrChallenge.m_cch = cchChallenge;
   DRM_B64_DecodeA( (DRM_CHAR*)pszChallenge, &dasstrChallenge, &cbTemp, pbTemp, 0 );

   dstrChallenge.pwszString = (DRM_WCHAR*)pbTemp;
   dstrChallenge.cchString = cbTemp / SIZEOF(DRM_WCHAR);

	dResult = DRM_XML_GetNode( &dstrChallenge, &g_dstrTagDRMReqRoot, NULL, NULL, 0, NULL, &dstrTemp );

	dResult = DRM_XML_GetNode( &dstrTemp, &g_dstrTagData, NULL, NULL, 0, &dstrData, NULL );

	/* Read Data section */
	dResult = DRM_XML_GetNode( &dstrTemp, &g_dstrTagData, NULL, NULL, 0, NULL, &dstrData );

	/* Get URL */
	dResult = DRM_XML_GetNode( &dstrData, &g_dstrTagURL, NULL, NULL, 0, NULL, &dstrURL );

	cchURL = 1024;
	rgchURL = (DRM_CHAR *) Oem_MemAlloc( cchURL );
	VK_memset(rgchURL, 0x0, cchURL);
	dResult = DRM_STR_UTF16toUTF8( dstrURL.pwszString, dstrURL.cchString, rgchURL, 0, &cchURL );

	PrintDebug("%s %d - rgchURL = %s\n", __FUNCTION__, __LINE__, rgchURL);


	if(get_secure_clock(pAppContext, pszChallenge, rgchURL) != DRV_OK)
	{
		PrintError("get_secure_clock error\n");
		ret = DRV_ERR;
	}

	Oem_MemFree(pszChallenge);
	Oem_MemFree(pwszURL);
	Oem_MemFree(pbTemp);
	Oem_MemFree(rgchURL);

	return ret;
}

DRV_Error DRV_DRM_PlayReady_GetLicenseURL(SESSION_HANDLE media_session_handle, HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize, HCHAR *licenseURL)
{
	DRM_RESULT dResult;
	const DRM_CONST_STRING dstrStoreName = CREATE_DRM_STRING(rgwchStoreName);
	const DRM_CONST_STRING *rgpdstrRights [1] = { 0 };
	DRM_DWORD f_cRights=0;
	DRM_CHAR* f_pchSilentURL=NULL;
	DRM_DWORD f_pcchSilentURL=0;
	DRM_BYTE* f_pbChallenge=NULL;
	DRM_DWORD f_pcbChallenge=0;
	DRM_LICENSE_RESPONSE LicenseResponse;
	DRM_DWORD cRights = 1;
	const DRM_CONST_STRING *apdcsRights [cRights];
	DRM_APP_CONTEXT* pAppContext = NULL;
	DRM_Session_t *drm_handle=NULL;
	DRM_DECRYPT_CONTEXT *pstDecryptContext=NULL;
	DRM_CONTENT_SET_PROPERTY eHeaderVersion;
	DRM_VOID *f_pOEMContext = NULL;

	DRM_CHAR rgchCustomData[ 1024 ] = "AddMid";
	DRM_DWORD cchCustomData = 0;
#if 0
	DRM_DOMAIN_ID oDomainID = EMPTY_DRM_DOMAIN_ID;
#endif
	DRM_BYTE *__macro_pbOpaqueBuffer = NULL;

#if 0
	DRM_STR_StringCchCatA( rgchCustomData, SIZEOF( rgchCustomData ), " UseDomainCert" );
	DRM_STR_StringCchCatA( rgchCustomData, SIZEOF( rgchCustomData ), " UseAesKey" );
#endif
	DRM_STR_StringCchLengthA( rgchCustomData, SIZEOF( rgchCustomData ), &cchCustomData );

	__macro_pbOpaqueBuffer = (DRM_BYTE*)Oem_MemAlloc(MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE);
	VK_MEM_Memset(&LicenseResponse, 0x0, sizeof(DRM_LICENSE_RESPONSE));

	apdcsRights[0] = (const DRM_CONST_STRING *)&g_dstrWMDRM_RIGHT_PLAYBACK;

	if(pucWrmHeader == NULL)
	{
		PrintError("Invalid Parameter!\n");
		return DRV_ERR;
	}

	pAppContext = (DRM_APP_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_APP_CONTEXT));
	if(pAppContext == NULL)
	{
		PrintError("Out of memory!\n");
		return DRV_ERR;
	}
	VK_memset(pAppContext, 0x0, SIZEOF(DRM_APP_CONTEXT));

	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		Oem_MemFree(pAppContext);
		return DRV_ERR;
	}
	pstDecryptContext = &(drm_handle->private.playready.pDecryptContext);

/********************
*	1. Initialize handle
********************/
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	f_pOEMContext = (DRM_VOID*)Oem_Create();
	if(f_pOEMContext == NULL)
	{
		PrintError("Oem_Create failed!\n");
		Oem_MemFree(pAppContext);
		return DRV_ERR;
	}
#endif

	dResult = Drm_Initialize(pAppContext, f_pOEMContext, __macro_pbOpaqueBuffer, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE, &dstrStoreName);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Initialize failed! 0x%X\n", dResult);
		Oem_MemFree(pAppContext);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
		Oem_Destroy(f_pOEMContext);
#endif
		return DRV_ERR;
	}

/********************
*	2. Set WRM Header
********************/
	eHeaderVersion = P_DRM_PlayReady_CheckHeaderVersion(pucWrmHeader, uiWrmHeaderSize);
	dResult = Drm_Content_SetProperty(pAppContext, eHeaderVersion, pucWrmHeader, uiWrmHeaderSize);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Content_SetProperty failed! 0x%X\n", dResult);
		Drm_Uninitialize(pAppContext);
		Oem_MemFree(pAppContext);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
		Oem_Destroy(f_pOEMContext);
#endif
		return DRV_ERR;
	}


/********************
*	4. Generate Challenge
********************/
	rgpdstrRights[0] = &g_dstrWMDRM_RIGHT_PLAYBACK;
	f_cRights = NO_OF(rgpdstrRights);
#if 0
	dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
		/*rgpdstrRights*/NULL,
		/*f_cRights*/0,
		NULL/*&oDomainID*/,
		/*NULL*/rgchCustomData,
		/*0*/cchCustomData,
		NULL,
		&f_pcchSilentURL,
		NULL,
		NULL,
		NULL,
		&f_pcbChallenge);
#else
	dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
		rgpdstrRights,
		f_cRights,
		NULL,
		NULL,
		0,
		NULL,
		&f_pcchSilentURL,
		NULL,
		NULL,
		NULL,
		&f_pcbChallenge);
#endif
	if(dResult == DRM_E_BUFFERTOOSMALL)
	{
		f_pchSilentURL = (DRM_CHAR*) Oem_MemAlloc(f_pcchSilentURL * SIZEOF (DRM_CHAR));
		VK_memset(f_pchSilentURL, 0x0, f_pcchSilentURL * SIZEOF (DRM_CHAR));
	    f_pbChallenge = (DRM_BYTE*) Oem_MemAlloc(f_pcbChallenge * SIZEOF(DRM_BYTE));
		VK_memset(f_pbChallenge, 0x0, f_pcbChallenge * SIZEOF(DRM_BYTE));
#if 0
		dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
			/*rgpdstrRights*/NULL,
			/*f_cRights*/0,
			NULL/*&oDomainID*/,
			/*NULL*/rgchCustomData,
			/*0*/cchCustomData,
			f_pchSilentURL,
			&f_pcchSilentURL,
			NULL,
			NULL,
			f_pbChallenge,
			&f_pcbChallenge);
#else
		dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
			rgpdstrRights,
			f_cRights,
			NULL,
			NULL,
			0,
			f_pchSilentURL,
			&f_pcchSilentURL,
			NULL,
			NULL,
			f_pbChallenge,
			&f_pcbChallenge);
#endif
		if(dResult != DRM_SUCCESS)
		{
			PrintError("Drm_LicenseAcq_GenerateChallenge failed! 0x%X\n", dResult);
			Drm_Uninitialize(pAppContext);
			Oem_MemFree(pAppContext);
			Oem_MemFree(f_pchSilentURL);
			Oem_MemFree(f_pbChallenge);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
			Oem_Destroy(f_pOEMContext);
#endif
			return DRV_ERR;
		}
		f_pbChallenge[f_pcbChallenge] = '\0';
		PrintDebug("Challenge: [%s]\n", f_pbChallenge);
	}
	else
	{
		PrintError("Drm_LicenseAcq_GenerateChallenge failed! 0x%X\n", dResult);
		Drm_Uninitialize(pAppContext);
		Oem_MemFree(pAppContext);
		Oem_MemFree(f_pchSilentURL);
		Oem_MemFree(f_pbChallenge);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
		Oem_Destroy(f_pOEMContext);
#endif
		return DRV_ERR;
	}

	VK_snprintf(licenseURL, 2048-1, "%s", f_pchSilentURL);

	Drm_Uninitialize(pAppContext);
	Oem_MemFree(pAppContext);

	Oem_MemFree(f_pchSilentURL);
	Oem_MemFree(f_pbChallenge);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	Oem_Destroy(f_pOEMContext);
#endif

	return DRV_OK;
}

DRV_Error P_DRM_PlayReady_GetLicense(SESSION_HANDLE media_session_handle, HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize, HINT8 *pPrivateKeyPath, HINT8 *pCertificatePath)
{
	DRM_RESULT dResult;
	const DRM_CONST_STRING dstrStoreName = CREATE_DRM_STRING(rgwchStoreName);
	const DRM_CONST_STRING *rgpdstrRights [1] = { 0 };
	DRM_DWORD f_cRights=0;
	DRM_CHAR* f_pchSilentURL=NULL;
	DRM_CHAR* pcChallengeUrl=NULL;
	DRM_DWORD f_pcchSilentURL=0;
	DRM_BYTE* f_pbChallenge=NULL;
	DRM_DWORD f_pcbChallenge=0;
	DRM_BYTE* pbData;
	DRM_DWORD cbData;
	DRM_LICENSE_RESPONSE LicenseResponse;
	DRM_BYTE *pbRevocationBuffer=NULL;
	HINT32 iResult;
	HUINT8* pucResponseData=NULL;
	HUINT8* pucResponseStart=NULL;
	HUINT32 uiResponseSize=0;
	DRM_DWORD cRights = 1;
	const DRM_CONST_STRING *apdcsRights [cRights];
	DRM_APP_CONTEXT* pAppContext = NULL;
	DRM_Session_t *drm_handle=NULL;
	DRM_DECRYPT_CONTEXT *pstDecryptContext=NULL;
	DRM_CONTENT_SET_PROPERTY eHeaderVersion;
	DRM_VOID *f_pOEMContext = NULL;
	HBOOL bMaxdome=FALSE;

	DRM_CHAR rgchCustomData[ 1024 ] = "AddMid";
	DRM_DWORD cchCustomData = 0;
#if 0
	DRM_DOMAIN_ID oDomainID = EMPTY_DRM_DOMAIN_ID;
#endif
	DRM_BYTE *__macro_pbOpaqueBuffer = NULL;

#if 0
	DRM_STR_StringCchCatA( rgchCustomData, SIZEOF( rgchCustomData ), " UseDomainCert" );
	DRM_STR_StringCchCatA( rgchCustomData, SIZEOF( rgchCustomData ), " UseAesKey" );
#endif
	DRM_STR_StringCchLengthA( rgchCustomData, SIZEOF( rgchCustomData ), &cchCustomData );

	__macro_pbOpaqueBuffer = (DRM_BYTE*)Oem_MemAlloc(MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE);
	VK_MEM_Memset(&LicenseResponse, 0x0, sizeof(DRM_LICENSE_RESPONSE));

	apdcsRights[0] = (const DRM_CONST_STRING *)&g_dstrWMDRM_RIGHT_PLAYBACK;

	if(pucWrmHeader == NULL)
	{
		PrintError("Invalid Parameter!\n");
		return DRV_ERR;
	}

	pAppContext = (DRM_APP_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_APP_CONTEXT));
	if(pAppContext == NULL)
	{
		PrintError("Out of memory!\n");
		return DRV_ERR;
	}
	VK_memset(pAppContext, 0x0, SIZEOF(DRM_APP_CONTEXT));

	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		Oem_MemFree(pAppContext);
		return DRV_ERR;
	}
	pstDecryptContext = &(drm_handle->private.playready.pDecryptContext);

/********************
*	1. Initialize handle
********************/
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	f_pOEMContext = (DRM_VOID*)Oem_Create();
	if(f_pOEMContext == NULL)
	{
		PrintError("Oem_Create failed!\n");
		Oem_MemFree(pAppContext);
		return DRV_ERR;
	}
#endif

	dResult = Drm_Initialize(pAppContext, f_pOEMContext, __macro_pbOpaqueBuffer, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE, &dstrStoreName);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Initialize failed! 0x%X\n", dResult);
		Oem_MemFree(pAppContext);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
		Oem_Destroy(f_pOEMContext);
#endif
		return DRV_ERR;
	}

/* Get Secure Clock */
#if 1
	if(P_DRM_PlayReady_GetSecureClock(pAppContext) != DRV_OK)
	{
		return DRV_ERR;
	}
#endif

/********************
*	2. Set WRM Header
********************/
	eHeaderVersion = P_DRM_PlayReady_CheckHeaderVersion(pucWrmHeader, uiWrmHeaderSize);
	dResult = Drm_Content_SetProperty(pAppContext, eHeaderVersion, pucWrmHeader, uiWrmHeaderSize);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Content_SetProperty failed! 0x%X\n", dResult);
		Drm_Uninitialize(pAppContext);
		Oem_MemFree(pAppContext);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
		Oem_Destroy(f_pOEMContext);
#endif
		return DRV_ERR;
	}

/********************
*	3. check the local store first
********************/
	dResult = Drm_Reader_Bind (pAppContext, apdcsRights, cRights, (DRMPFNPOLICYCALLBACK)PlayReadyBindCallback, media_session_handle, pstDecryptContext);
	if(dResult == DRM_SUCCESS)
	{
		PrintError("Local Store already has the license!\n");
		Drm_Uninitialize(pAppContext);
		Oem_MemFree(pAppContext);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
		Oem_Destroy(f_pOEMContext);
#endif
		return DRV_OK;
	}

/********************
*	4. Generate Challenge
********************/
	rgpdstrRights[0] = &g_dstrWMDRM_RIGHT_PLAYBACK;
	f_cRights = NO_OF(rgpdstrRights);
#if 0
	dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
		/*rgpdstrRights*/NULL,
		/*f_cRights*/0,
		NULL/*&oDomainID*/,
		/*NULL*/rgchCustomData,
		/*0*/cchCustomData,
		NULL,
		&f_pcchSilentURL,
		NULL,
		NULL,
		NULL,
		&f_pcbChallenge);
#else
	dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
		rgpdstrRights,
		f_cRights,
		NULL,
		NULL,
		0,
		NULL,
		&f_pcchSilentURL,
		NULL,
		NULL,
		NULL,
		&f_pcbChallenge);
#endif
	if(dResult == DRM_E_BUFFERTOOSMALL)
	{
		f_pchSilentURL = (DRM_CHAR*) Oem_MemAlloc(f_pcchSilentURL * SIZEOF (DRM_CHAR));
		VK_memset(f_pchSilentURL, 0x0, f_pcchSilentURL * SIZEOF (DRM_CHAR));
	    f_pbChallenge = (DRM_BYTE*) Oem_MemAlloc(f_pcbChallenge * SIZEOF(DRM_BYTE));
		VK_memset(f_pbChallenge, 0x0, f_pcbChallenge * SIZEOF(DRM_BYTE));
#if 0
		dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
			/*rgpdstrRights*/NULL,
			/*f_cRights*/0,
			NULL/*&oDomainID*/,
			/*NULL*/rgchCustomData,
			/*0*/cchCustomData,
			f_pchSilentURL,
			&f_pcchSilentURL,
			NULL,
			NULL,
			f_pbChallenge,
			&f_pcbChallenge);
#else
		dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
			rgpdstrRights,
			f_cRights,
			NULL,
			NULL,
			0,
			f_pchSilentURL,
			&f_pcchSilentURL,
			NULL,
			NULL,
			f_pbChallenge,
			&f_pcbChallenge);
#endif
		if(dResult != DRM_SUCCESS)
		{
			PrintError("Drm_LicenseAcq_GenerateChallenge failed! 0x%X\n", dResult);
			Drm_Uninitialize(pAppContext);
			Oem_MemFree(pAppContext);
			Oem_MemFree(f_pchSilentURL);
			Oem_MemFree(f_pbChallenge);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
			Oem_Destroy(f_pOEMContext);
#endif
			return DRV_ERR;
		}
		f_pbChallenge[f_pcbChallenge] = '\0';
		PrintDebug("Challenge: [%s]\n", f_pbChallenge);
	}
	else
	{
		PrintError("Drm_LicenseAcq_GenerateChallenge failed! 0x%X\n", dResult);
		Drm_Uninitialize(pAppContext);
		Oem_MemFree(pAppContext);
		Oem_MemFree(f_pchSilentURL);
		Oem_MemFree(f_pbChallenge);
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
		Oem_Destroy(f_pOEMContext);
#endif
		return DRV_ERR;
	}

/********************
*	5. Overwrite URL
********************/
	if(drm_handle->drm_info.drm.playready.licenseURL)
	{
		Oem_MemFree(f_pchSilentURL);
		f_pchSilentURL = (DRM_CHAR*) Oem_MemAlloc(VK_strlen(drm_handle->drm_info.drm.playready.licenseURL) + 1);
		VK_MEM_Memset(f_pchSilentURL, 0x0, VK_strlen(drm_handle->drm_info.drm.playready.licenseURL) + 1);
		VK_MEM_Memcpy(f_pchSilentURL, drm_handle->drm_info.drm.playready.licenseURL, VK_strlen(drm_handle->drm_info.drm.playready.licenseURL));
	}

	pcChallengeUrl = P_DRM_AddMaxdomeCookie(media_session_handle, f_pchSilentURL);
	if(pcChallengeUrl == NULL)
	{
		pcChallengeUrl = f_pchSilentURL;
	}
	else
	{
		bMaxdome = TRUE;
	}

/********************
*	6. POST Challenge (soap)
********************/
	if(eHeaderVersion == DRM_CSP_V4_HEADER)
	{
		bhttpclient_set_certificate(pPrivateKeyPath, pCertificatePath);

		iResult = bhttpclient_license_post_soap(
			(char*)pcChallengeUrl,
			(char*)f_pbChallenge,
			0,
			2000,
			HDR_LICGET_ACTION_VALUE,
			&pucResponseData,
			&pucResponseStart,
			&uiResponseSize);
		if(iResult != 0)
		{
			PrintError("bhttpclient_license_post_soap failed!\n");
			Drm_Uninitialize(pAppContext);
			Oem_MemFree(pAppContext);
			Oem_MemFree(f_pchSilentURL);
			Oem_MemFree(f_pbChallenge);
			if(pucResponseData != NULL)
			{
				BKNI_Free(pucResponseData);
				pucResponseData = NULL;
				pucResponseStart = NULL;
			}
			if(bMaxdome == TRUE)
			{
				if(pcChallengeUrl != NULL)
				{
					Oem_MemFree(pcChallengeUrl);
				}
			}
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
			Oem_Destroy(f_pOEMContext);
#endif
			return DRV_ERR;
		}
	}
	else
	{
		iResult = bhttpclient_license_post_default(
			(char*)pcChallengeUrl,
			(char*)f_pbChallenge,
			0,
			2000,
			NULL,
			&pucResponseData,
			&pucResponseStart,
			&uiResponseSize);
		if(iResult != 0)
		{
			PrintError("bhttpclient_license_post_default failed!\n");
			Drm_Uninitialize(pAppContext);
			Oem_MemFree(pAppContext);
			Oem_MemFree(f_pchSilentURL);
			Oem_MemFree(f_pbChallenge);
			if(pucResponseData != NULL)
			{
				BKNI_Free(pucResponseData);
				pucResponseData = NULL;
				pucResponseStart = NULL;
			}
			if(bMaxdome == TRUE)
			{
				if(pcChallengeUrl != NULL)
				{
					Oem_MemFree(pcChallengeUrl);
				}
			}
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
			Oem_Destroy(f_pOEMContext);
#endif
			return DRV_ERR;
		}
	}
	PrintDebug("pucResponseData[%d] = %s\n", uiResponseSize, pucResponseData);

/********************
*	7. Process Response
********************/

	pbData = pucResponseStart;
	cbData = uiResponseSize;
	pbRevocationBuffer = (DRM_BYTE *)Oem_MemAlloc(REVOCATION_BUFFER_SIZE);

	dResult = Drm_Revocation_SetBuffer(pAppContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Revocation_SetBuffer failed! [%x]\n", dResult);
	}

	dResult = Drm_LicenseAcq_ProcessResponse(pAppContext, DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
		NULL,
		NULL,
		pbData,
		cbData,
		&LicenseResponse);

	PrintDebug("LicenseResponse.m_eType = %d\n", LicenseResponse.m_eType);

	if(dResult == DRM_SUCCESS)
	{
		PrintError("License is Stored well! [%d]\n", LicenseResponse.m_cAcks);
	}
	else if (dResult == DRM_S_MORE_DATA)
	{
		PrintError("License is Stored well, and some of custom data exist! [%d]\n", LicenseResponse.m_cAcks);
	}
	else
	{
		PrintError("Drm_LicenseAcq_ProcessResponse returned 0x%X\n", dResult);
		Drm_Uninitialize(pAppContext);
		Oem_MemFree(pAppContext);
		Oem_MemFree(f_pbChallenge);
		Oem_MemFree(f_pchSilentURL);
		Oem_MemFree(pbRevocationBuffer);
		if(pucResponseData != NULL)
		{
			BKNI_Free(pucResponseData);
			pucResponseData = NULL;
			pucResponseStart = NULL;
		}
		if(bMaxdome == TRUE)
		{
			if(pcChallengeUrl != NULL)
			{
				Oem_MemFree(pcChallengeUrl);
			}
		}
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
		Oem_Destroy(f_pOEMContext);
#endif
		return DRV_ERR;
	}

/*************************
*	8. License Acknowledgment
*	Note: Sending Ack fail doesn't cause the error return!
*************************/
	DRM_DWORD cbAckChallenge;
	DRM_BYTE* pbAckChallenge=NULL;
	DRM_DWORD cbAckResponse;
	DRM_BYTE* pbAckResponse=NULL;

	PrintDebug("LicenseResponse.m_cbTransactionID = %d\n", LicenseResponse.m_cbTransactionID);

	if(LicenseResponse.m_cbTransactionID > 0)
	{
		dResult = Drm_LicenseAcq_GenerateAck(pAppContext, &LicenseResponse, NULL, &cbAckChallenge);
		if(dResult == DRM_E_BUFFERTOOSMALL)
		{
			pbAckChallenge = (DRM_BYTE*) Oem_MemAlloc(cbAckChallenge + 1);
			if(pbAckChallenge == NULL)
			{
				PrintError("Out of Memory!\n");
				Drm_Uninitialize(pAppContext);
				Oem_MemFree(pAppContext);
				Oem_MemFree(f_pbChallenge);
				Oem_MemFree(f_pchSilentURL);
				Oem_MemFree(pbRevocationBuffer);
				if(pucResponseData != NULL)
				{
					BKNI_Free(pucResponseData);
					pucResponseData = NULL;
					pucResponseStart = NULL;
				}
				if(bMaxdome == TRUE)
				{
					if(pcChallengeUrl != NULL)
					{
						Oem_MemFree(pcChallengeUrl);
					}
				}
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
				Oem_Destroy(f_pOEMContext);
#endif
				return DRV_ERR;
			}

			VK_memset(pbAckChallenge, 0x0, cbAckChallenge + 1);
			dResult = Drm_LicenseAcq_GenerateAck(pAppContext, &LicenseResponse, pbAckChallenge, &cbAckChallenge);
			if(dResult != DRM_SUCCESS)
			{
				PrintError("Drm_LicenseAcq_GenerateAck returned 0x%X\n", dResult);
			}
			pbAckChallenge[cbAckChallenge] = '\0';

			PrintDebug("pbAckChallenge[%d] = %s\n", cbAckChallenge, pbAckChallenge);

			iResult = bhttpclient_license_post_soap(
				(char*)f_pchSilentURL,
				(char*)pbAckChallenge,
				0,
				0,
				HDR_LICACK_ACTION_VALUE,
				&pbAckResponse,
				&pbAckResponse,
				&cbAckResponse);

			PrintDebug("pbAckResponse[%d] = %s\n", cbAckResponse, pbAckResponse);
			if(iResult != 0)
			{
				PrintError("bhttpclient_license_post_soap failed!\n");
			}

			dResult = Drm_LicenseAcq_ProcessAckResponse(pAppContext, pbAckResponse, cbAckResponse, NULL);
			if(dResult == DRM_SUCCESS)
			{
				PrintError("Drm_LicenseAcq_ProcessAckResponse Successed!\n");
			}
#if 1 /* ignore "licenses are not veritied" message for dark nkght stream(SSTR). */
			else if (dResult == DRM_S_MORE_DATA)
			{
				PrintError("Drm_LicenseAcq_ProcessAckResponse Successed, and some of data exist!\n");
			}
#endif
			else
			{
				PrintError("Drm_LicenseAcq_ProcessAckResponse returned 0x%X\n", dResult);
			}
		}
		else
		{
			PrintError("Drm_LicenseAcq_GenerateAck returns 0x%x\n", dResult);
		}

		if(pbAckChallenge != NULL)
		{
			Oem_MemFree(pbAckChallenge);
		}
		if(pbAckResponse != NULL)
		{
			Oem_MemFree(pbAckResponse);
		}
	}

/*************************
*	9. Bind the license again
*************************/
	dResult = Drm_Reader_Bind (pAppContext, apdcsRights, cRights, (DRMPFNPOLICYCALLBACK)PlayReadyBindCallback, media_session_handle, pstDecryptContext);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Reader_Bind returns 0x%x\n", dResult);
		Drm_Uninitialize(pAppContext);
		Oem_MemFree(pAppContext);
		Oem_MemFree(f_pbChallenge);
		Oem_MemFree(f_pchSilentURL);
		Oem_MemFree(pbRevocationBuffer);
		if(pucResponseData != NULL)
		{
			BKNI_Free(pucResponseData);
			pucResponseData = NULL;
			pucResponseStart = NULL;
		}
		if(bMaxdome == TRUE)
		{
			if(pcChallengeUrl != NULL)
			{
				Oem_MemFree(pcChallengeUrl);
			}
		}
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
		Oem_Destroy(f_pOEMContext);
#endif
		return DRV_ERR;
	}

/*************************
*	10. Commit, License processing all done!
*************************/
	dResult = Drm_Reader_Commit(pAppContext, (DRMPFNPOLICYCALLBACK)PlayReadyCommitCallback, NULL);
	if(dResult != DRM_SUCCESS)
	{
		PrintError("Drm_Reader_Commit returns 0x%x\n", dResult);
		Drm_Uninitialize(pAppContext);
		Oem_MemFree(pAppContext);
		Oem_MemFree(f_pbChallenge);
		Oem_MemFree(f_pchSilentURL);
		Oem_MemFree(pbRevocationBuffer);
		if(pucResponseData != NULL)
		{
			BKNI_Free(pucResponseData);
			pucResponseData = NULL;
			pucResponseStart = NULL;
		}
		if(bMaxdome == TRUE)
		{
			if(pcChallengeUrl != NULL)
			{
				Oem_MemFree(pcChallengeUrl);
			}
		}
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
		Oem_Destroy(f_pOEMContext);
#endif
		return DRV_ERR;
	}

#if defined(CONFIG_PLAYREADY_25)
	drm_handle->private.playready.pAppContext = pAppContext;
	drm_handle->private.playready.bSetContext = TRUE;
#else
	Drm_Uninitialize(pAppContext);
	Oem_MemFree(pAppContext);
#endif
	Oem_MemFree(f_pbChallenge);
	Oem_MemFree(f_pchSilentURL);
	Oem_MemFree(pbRevocationBuffer);
	if(pucResponseData != NULL)
	{
		BKNI_Free(pucResponseData);
		pucResponseData = NULL;
		pucResponseStart = NULL;
	}
	if(bMaxdome == TRUE)
	{
		if(pcChallengeUrl != NULL)
		{
			Oem_MemFree(pcChallengeUrl);
		}
	}

#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
#if defined(CONFIG_PLAYREADY_25)
	drm_handle->private.playready.f_pOEMContext = f_pOEMContext;
#else
	Oem_Destroy(f_pOEMContext);
#endif
#endif
	return DRV_OK;
}

static size_t P_DRM_atom_cursor_refill(batom_cursor *cursor)
{
	if (cursor->left==0)
	{
		unsigned pos;
		const batom_vec *vec;

		for(pos=cursor->pos,vec=&cursor->vec[pos];pos < cursor->count;vec++)
		{
			pos++;
			cursor->pos = pos;
			if (vec->len>0)
			{
				cursor->left = vec->len;
				cursor->cursor = vec->base;
				return (size_t)cursor->left;
			}
		}

		cursor->left = BATOM_EOF;
	}

	return 0;
}

size_t P_DRM_map_cursor_to_dmablks(batom_cursor *cursor, size_t count, NEXUS_DmaJobBlockSettings *blks, uint32_t nb_blks_avail, uint32_t *nb_blks_used)
{
    size_t left;
    uint32_t ii = 0;

    for(left=count; ii < nb_blks_avail; ) {
        const uint8_t *src=cursor->cursor;
        int src_left = cursor->left;

        if(src_left>=(int)left)
		{
            cursor->cursor = src+left;
            cursor->left = src_left-left;
            NEXUS_DmaJob_GetDefaultBlockSettings(&blks[ii]);
            blks[ii].pSrcAddr  = (uint8_t *)src;
            blks[ii].pDestAddr = (uint8_t *)src;
            blks[ii].blockSize = left;

            left = 0;
            ii++;
            break;
        }
        if(src_left>=0)
		{
            cursor->cursor = src+src_left;
            cursor->left = 0;
            left -= src_left;
            NEXUS_DmaJob_GetDefaultBlockSettings(&blks[ii]);
            blks[ii].pSrcAddr  = (uint8_t *)src;
            blks[ii].pDestAddr = (uint8_t *)src;
            blks[ii].blockSize = src_left;
            ii++;

            if (P_DRM_atom_cursor_refill(cursor)==0)
			{
                break;
            }
        } else {
            break;
        }
    }
    (*nb_blks_used) = ii;
	return count-left;
}

void P_DRM_DoDecryptSample(void* pHSession, void* sample_enc_box, unsigned int sample_no, void* pucData, unsigned int uiSize, unsigned char *pIV)
{
	bmp4_ext_sample_encryption_box* sample_encryption_box = (bmp4_ext_sample_encryption_box*)sample_enc_box;
	batom_cursor* data = (batom_cursor*)pucData;
	batom_checkpoint start;
	DRM_CIPHER_CONTEXT *cipher_ctx;
	DRM_Session_t *drm_handle;
	bmp4_ext_sample_encryption* pSample;
	HUINT8* pEkl=NULL;
	NEXUS_DmaJobBlockSettings stBlkSettings[100];
	unsigned jj=0;
	size_t bytes_ref;
	uint32_t nb_Blks=1;
	uint32_t nb_blks_used=0;
	CommonCryptoJobSettings jobSettings;
	NEXUS_Error nResult;

/*************************
*	1. NULL Check
*************************/
	if((pHSession == NULL) || (sample_encryption_box == NULL) || (data == NULL) || (pIV == NULL))
	{
		PrintError("Invalid Parameter!\n");
		return;
	}

	batom_cursor_save(data, &start);

/*************************
*	2. get key & iv
*************************/
	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)pHSession);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		return;
	}

	cipher_ctx = (DRM_CIPHER_CONTEXT* )drm_handle->private.playready.pDecryptContext.rgbBuffer;
	pEkl = drm_handle->private.playready.pEkl;
	pSample = &sample_encryption_box->samples[sample_no];

	VK_MEM_Memcpy(pEkl,   cipher_ctx->cipher.cipherAES.rgbContentKey+8, 8); /* Copy key.  H and L need to be swapped */
	VK_MEM_Memcpy(pEkl+8, cipher_ctx->cipher.cipherAES.rgbContentKey, 8);
	VK_MEM_Memcpy(&pEkl[16], pSample->InitializationVector_128, 16);
	VK_MEM_Memset(&pEkl[16], 0, 8);

    NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[0]);
    stBlkSettings[0].pSrcAddr  = pEkl;
    stBlkSettings[0].pDestAddr = pEkl;
    stBlkSettings[0].blockSize = 32;
    stBlkSettings[0].resetCrypto              = true;
    stBlkSettings[0].scatterGatherCryptoStart = true;

    NEXUS_FlushCache(stBlkSettings[0].pSrcAddr, 32);

/*************************
*	3. dma setting
*************************/
	if(sample_encryption_box->fullbox.flags &= 0x000002)
	{
		for(jj = 0; jj < pSample->NumberOfEntries; jj++)
		{
			batom_cursor_skip((batom_cursor *)data, pSample->entries[jj].BytesOfClearData);
			bytes_ref = P_DRM_map_cursor_to_dmablks((batom_cursor *)data, pSample->entries[jj].BytesOfEncryptedData, &stBlkSettings[nb_Blks], 99, &nb_blks_used);
			if(bytes_ref != pSample->entries[jj].BytesOfEncryptedData)
			{
				 PrintError("%s - out of DMA blks, return without decrypting\n", __FUNCTION__);
				 return;
			}
			nb_Blks += nb_blks_used;
		}
		stBlkSettings[nb_Blks-1].scatterGatherCryptoEnd = true;
	}
	else
	{
		{
			bytes_ref = P_DRM_map_cursor_to_dmablks((batom_cursor *)data, uiSize, &stBlkSettings[1], 99, &nb_blks_used);
			if(bytes_ref != uiSize){
				 PrintError("%s - out of DMA blks, return without decrypting\n", __FUNCTION__);
				 return;
			}
		}
		nb_Blks += nb_blks_used;
		stBlkSettings[nb_Blks-1].scatterGatherCryptoEnd = true;
	}

/*************************
*	4. decrypt
*************************/
	CommonCrypto_GetDefaultJobSettings(&jobSettings);
	jobSettings.keySlot = drm_handle->private.playready.keyPlayready;

	nResult = CommonCrypto_DmaXfer(drm_handle->private.playready.cryptoHandle, &jobSettings, stBlkSettings, nb_Blks);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("%s - CommonCrypto_DmaXfer failed\n", __FUNCTION__);
	}

	batom_cursor_rollback(data, &start);
}

void P_DRM_DoDecryptAESCtr(void* pHSession, HUINT8* pucData, HUINT32 ulSize, HUINT8* pIV)
{
	DRM_CIPHER_CONTEXT *cipher;
	DRM_Session_t *drm_handle;
	HUINT8* pEkl=NULL;
	HUINT32 ekl_size;
	NEXUS_Error nResult;
	NEXUS_DmaJobBlockSettings stBlkSettings[2];
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobHandle dmaJob = NULL;
	NEXUS_DmaJobStatus jobStatus;
#if defined(CONFIG_PLAYREADY_25)
	DRM_RESULT dr;
	DRM_BYTE *last_15_bytes;
	static DRM_AES_COUNTER_MODE_CONTEXT m_aes_context;
#endif
	unsigned long				vkEvtId = 0;
	unsigned long				vkEvt;
	int 						vkRet;

/*************************
*	1. NULL Check
*************************/
	if(pHSession == NULL)
	{
		PrintError("pHSession is NULL!!\n");
		return;
	}

	if(pucData == NULL)
	{
		PrintError("Buffer is NULL!\n");
		return;
	}

/*************************
*	2. AES Counter decrypt
*************************/
	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)pHSession);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		return;
	}

#if defined(CONFIG_PLAYREADY_25)
/*************************
*	2. Cocktail decrypt
*************************/
	if (ulSize < 15)
	{
		last_15_bytes = (DRM_BYTE*)pucData;
	}
	else
	{
		last_15_bytes = (DRM_BYTE*)pucData + (ulSize - 15);
	}

	/*16 Byte Initialization Vector is not supported (This Callback call with 8 byte array Initialization Vector )*/
	/*refer. BSEAV/lib/asf/basf_stream.c*/
	m_aes_context.qwInitializationVector = (DRM_UINT64)pIV[0] << 56 | (DRM_UINT64)pIV[1] << 48 | (DRM_UINT64)pIV[2] << 40 | (DRM_UINT64)pIV[3] << 32 | (DRM_UINT64)pIV[4] << 24 | (DRM_UINT64)pIV[5] << 16 | (DRM_UINT64)pIV[6] << 8 | (DRM_UINT64)pIV[7];
	m_aes_context.qwBlockOffset = 0; /*(DRM_UINT64)pIV[8] << 56 | (DRM_UINT64)pIV[9] << 48 | (DRM_UINT64)pIV[10] << 40 | (DRM_UINT64)pIV[11] << 32 | (DRM_UINT64)pIV[12] << 24 | (DRM_UINT64)pIV[13] << 16 | (DRM_UINT64)pIV[14] << 8 | (DRM_UINT64)pIV[15];*/
	m_aes_context.bByteOffset = 0;

	dr = Drm_Reader_InitDecrypt(&drm_handle->private.playready.pDecryptContext, last_15_bytes, ulSize);
	if (dr == DRM_SUCCESS)
	{
		dr = Drm_Reader_Decrypt(&drm_handle->private.playready.pDecryptContext, &m_aes_context, pucData, ulSize);
	}
	else
	{
		PrintError(("InitDecrypt failed = 0x%x\n", dr));
	}
#else
	cipher = (DRM_CIPHER_CONTEXT* )drm_handle->private.playready.pDecryptContext.rgbBuffer;

	pEkl = drm_handle->private.playready.pEkl;

	VK_MEM_Memcpy(pEkl,   cipher->cipher.cipherAES.rgbContentKey+8, 8); /* Copy key.  H and L need to be swapped */
	VK_MEM_Memcpy(pEkl+8, cipher->cipher.cipherAES.rgbContentKey, 8);

	VK_MEM_Memset(&pEkl[16], 0, 8);
	VK_MEM_Memcpy(&pEkl[24], pIV, 8);

	ekl_size = 32;

	NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[0]);
	stBlkSettings[0].pSrcAddr                 = pEkl;
	stBlkSettings[0].pDestAddr                = pEkl;
	stBlkSettings[0].blockSize                = ekl_size;
	stBlkSettings[0].cached                   = true;
	stBlkSettings[0].resetCrypto              = true;
	stBlkSettings[0].scatterGatherCryptoStart = true;
	stBlkSettings[0].scatterGatherCryptoEnd   = false;

	NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[1]);
	stBlkSettings[1].pSrcAddr                 = pucData;
	stBlkSettings[1].pDestAddr                = pucData;
	stBlkSettings[1].blockSize                = ulSize;
	stBlkSettings[1].cached                   = true;
	stBlkSettings[1].resetCrypto              = true;
	stBlkSettings[1].scatterGatherCryptoStart = false;
	stBlkSettings[1].scatterGatherCryptoEnd   = true;

/*************************
*	4. DmaJob setting
*************************/
	vkRet = VK_EVENT_Create(&vkEvtId, "DRM_DMA_EVT");
	if (VK_OK != vkRet)
	{
		goto done;
	}

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks                   = 2;
	jobSettings.keySlot                     = drm_handle->private.playready.keyPlayready;
	jobSettings.dataFormat                  = NEXUS_DmaDataFormat_eBlock;
	jobSettings.completionCallback.callback = P_DRM_DMA_CompletedCallback;
	jobSettings.completionCallback.context = (void*)vkEvtId;

	dmaJob = NEXUS_DmaJob_Create(drm_handle->private.playready.dmaHandle, &jobSettings);
	if(dmaJob == NULL){
	    PrintError("NEXUS_DmaJob_Create failed!\n");
		goto done;
	}

/*************************
*	5. Dma transfer
*************************/
	nResult = NEXUS_DmaJob_ProcessBlocks(dmaJob, (NEXUS_DmaJobBlockSettings *)&stBlkSettings, 2);

	vkRet = VK_EVENT_ReceiveTimeout(vkEvtId, &vkEvt, 200);
	if (VK_OK != vkRet)
	{
		PrintError("DMA transfer failed\n");
		goto done;
	}
done:
	if(dmaJob != NULL)
	{
		NEXUS_DmaJob_Destroy(dmaJob);
	}

	if (vkEvtId != 0)
	{
		(void)VK_EVENT_Destroy(vkEvtId);
	}
#endif
	return;
}

void P_DRM_DoDecryptRC4_sw(void* pHSession, unsigned char* pucData, unsigned int uiSize)
{
	DRM_BYTE *last_15_bytes;
    DRM_RESULT dr;
	DRM_CIPHER_CONTEXT *cipher;
	DRM_Session_t *drm_handle;
#if defined(CONFIG_PLAYREADY_25)
	static DRM_AES_COUNTER_MODE_CONTEXT m_aes_context;
#endif
/*************************
*	1. NULL Check
*************************/
	if(pHSession == NULL)
	{
		PrintError("pHSession is NULL!!\n");
		return;
	}

	if(pucData == NULL)
	{
		PrintError("Buffer is NULL!\n");
		return;
	}

/*************************
*	2. Cocktail decrypt
*************************/
	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)pHSession);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		return;
	}

	cipher = (DRM_CIPHER_CONTEXT* )drm_handle->private.playready.pDecryptContext.rgbBuffer;
	if (uiSize < 15)
	{
		last_15_bytes = (DRM_BYTE*)pucData;
	}
	else
	{
		last_15_bytes = (DRM_BYTE*)pucData + (uiSize - 15);
	}
#if defined(CONFIG_PLAYREADY_25)
	m_aes_context.qwInitializationVector = 0;
	m_aes_context.qwBlockOffset = 0;
	m_aes_context.bByteOffset = 0;
#endif
	dr = Drm_Reader_InitDecrypt( &(drm_handle->private.playready.pDecryptContext), last_15_bytes, uiSize);
	if (dr == DRM_SUCCESS)
	{
#if defined(CONFIG_PLAYREADY_25)
		dr = Drm_Reader_Decrypt( &(drm_handle->private.playready.pDecryptContext), &m_aes_context, pucData, uiSize);
#else
		dr = Drm_Reader_Decrypt( &(drm_handle->private.playready.pDecryptContext), NULL, pucData, uiSize);
#endif
	}
	else
	{
		PrintError("InitDecrypt failed = 0x%x\n", dr);
	}
	return;
}

void P_DRM_DoDecryptRC4_hw(void* pHSession, unsigned char* pucData, unsigned int uiSize)
{
	DRM_CIPHER_CONTEXT *cipher;
	DRM_Session_t *drm_handle;
	HUINT8* pEkl=NULL;
	DRM_BYTE *last_15_bytes;
    HUINT32 cbClear=0;
	NEXUS_Error nResult;
    NEXUS_DmaJobBlockSettings stBlkSettings[2];
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobHandle dmaJob = NULL;
	NEXUS_DmaJobStatus jobStatus;

	unsigned long				vkEvtId = 0;
	unsigned long				vkEvt;
	int							vkRet;

/*************************
*	1. NULL Check
*************************/
	if(pHSession == NULL)
	{
		PrintError("pHSession is NULL!!\n");
		return;
	}

	if(pucData == NULL)
	{
		PrintError("Buffer is NULL!\n");
		return;
	}

#if defined(CONFIG_PLAYREADY_25)
	P_DRM_DoDecryptRC4_sw(pHSession, pucData, uiSize);
#else
	if(uiSize < 16)
	{
		P_DRM_DoDecryptRC4_sw(pHSession, pucData, uiSize);
		return;
	}

	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)pHSession);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		return;
	}

	cipher = (DRM_CIPHER_CONTEXT* )drm_handle->private.playready.pDecryptContext.rgbBuffer;

/*************************
*	2. EKL setting
*************************/
	pEkl = drm_handle->private.playready.pEkl;
	if (uiSize < 15)
	{
		last_15_bytes = (DRM_BYTE*)pucData;
	}
	else
	{
		last_15_bytes = (DRM_BYTE*)pucData + (uiSize - 15);
	}

	/*8 byte DES key*/
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.shaOut[12]),8);
	pEkl+=8;

	/*8 byte RC4 content key (the one at the end of of the ASF paylaod */
	cbClear = uiSize % 8;	/*Find Number of clear bytes*/
	VK_MEM_Memcpy(pEkl, &(last_15_bytes[15-8-cbClear]),8);
	pEkl+=8;

    /*8 byte D1 */
    /*D1 = "9.	From the 64 bytes obtained in step 4, use the last 8 bytes to XOR with the result of the DES encryption in step 8."*/
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.desS2[0]),8);
	pEkl+=8;

    /*8 byte D0 */
    /*D0 = "From the 64 bytes obtained in step 4, the 8 bytes following the 48 bytes MAC key are used to XOR the RC4 content key obtained in step 5."*/
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.desS1[0]),8);
	pEkl+=8;

    /*48 byte MAC key */
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.a1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.b1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.c1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.d1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.e1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.f1), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.a2), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.b2), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.c2), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.d2), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.e2), 4);
	pEkl+=4;
	VK_MEM_Memcpy(pEkl, &(cipher->cipher.cipherRC4.mackey.f2), 4);
	pEkl-=76;

/*************************
*	3. Decrypt
*************************/
    NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[0]);
    stBlkSettings[0].pSrcAddr                 = pEkl;
    stBlkSettings[0].pDestAddr                = pEkl;
    stBlkSettings[0].blockSize                = 80;
    stBlkSettings[0].cached                   = true;
    stBlkSettings[0].resetCrypto              = true;
    stBlkSettings[0].scatterGatherCryptoStart = true;
    stBlkSettings[0].scatterGatherCryptoEnd   = false;

    NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[1]);
    stBlkSettings[1].pSrcAddr                 = pucData;
    stBlkSettings[1].pDestAddr                = pucData;
    stBlkSettings[1].blockSize                = uiSize;
    stBlkSettings[1].cached                   = true;
    stBlkSettings[1].resetCrypto              = true;
    stBlkSettings[1].scatterGatherCryptoStart = false;
    stBlkSettings[1].scatterGatherCryptoEnd   = true;

/*************************
*	4. DmaJob setting
*************************/
	vkRet = VK_EVENT_Create(&vkEvtId, "DRM_DMA_EVT");
	if (VK_OK != vkRet)
	{
		goto done;
	}

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks                   = 2;
    jobSettings.keySlot                     = drm_handle->private.playready.keyWmdrm;
    jobSettings.dataFormat                  = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = P_DRM_DMA_CompletedCallback;
	jobSettings.completionCallback.context = (void*)vkEvtId;

    dmaJob = NEXUS_DmaJob_Create(drm_handle->private.playready.dmaHandle, &jobSettings);
    if(dmaJob == NULL){
        PrintError("NEXUS_DmaJob_Create failed!\n");
		goto done;
    }

/*************************
*	5. Dma transfer
*************************/
	nResult = NEXUS_DmaJob_ProcessBlocks(dmaJob, (NEXUS_DmaJobBlockSettings *)&stBlkSettings, 2);

	vkRet = VK_EVENT_ReceiveTimeout(vkEvtId, &vkEvt, 200);
	if (VK_OK != vkRet)
	{
		PrintError("DMA transfer failed\n");
		goto done;
	}

done:

	if(dmaJob != NULL)
	{
		NEXUS_DmaJob_Destroy(dmaJob);
	}

	if (vkEvtId != 0)
	{
		(void)VK_EVENT_Destroy(vkEvtId);
	}
#endif
	return;
}

DRV_Error P_DRM_PlayReady_SetDecryptCb(SESSION_HANDLE pHSession)
{
	DRM_Session_t *drm_handle;

	if(pHSession == NULL)
	{
		PrintError("Invalid parameter, pHSession is NULL!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance(pHSession);
	if(drm_handle == NULL)
	{
		PrintError("There's no matching drm handle!\n");
		return DRV_ERR;
	}

	if(drm_handle->drm_info.drm_type == DI_DRM_TYPE_PLAYREADY)
	{
#if (NEXUS_VERSION < 1290)
		NEXUS_Playpump_SetWmdrmDecryptCb(
				(NEXUS_PlaypumpHandle)(drm_handle->drm_info.drm.playready.pstPlaypump),
				pHSession,
				NULL);

		NEXUS_Playpump_SetPlayReadyAsfDecryptCb(
				(NEXUS_PlaypumpHandle)(drm_handle->drm_info.drm.playready.pstPlaypump),
				pHSession,
				P_DRM_DoDecryptAESCtr); /*16 Byte Initialization Vector is not supported (This Callback call with 8 byte array Initialization Vector )*/

		NEXUS_Playpump_SetPlayReadySstrDecryptCb(
				(NEXUS_PlaypumpHandle)(drm_handle->drm_info.drm.playready.pstPlaypump),
				pHSession,
				P_DRM_DoDecryptSample);
#endif
	}
	else if(drm_handle->drm_info.drm_type == DI_DRM_TYPE_WMDRM)
	{
#if (NEXUS_VERSION < 1290)
		NEXUS_Playpump_SetWmdrmDecryptCb(
				(NEXUS_PlaypumpHandle)(drm_handle->drm_info.drm.playready.pstPlaypump),
				pHSession,
				P_DRM_DoDecryptRC4_hw);

		NEXUS_Playpump_SetPlayReadyAsfDecryptCb(
				(NEXUS_PlaypumpHandle)(drm_handle->drm_info.drm.playready.pstPlaypump),
				pHSession,
				NULL);
#endif
	}
	else
	{
		PrintError("Invalid drm type (%d)\n", drm_handle->drm_info.drm_type);
		return DRV_ERR;
	}
	return DRV_OK;
}

#endif

#if defined (CONFIG_MARLIN_DRM)

static DRV_Error P_DRM_SetDecryptKeys(SESSION_HANDLE media_session_handle);
static DRV_Error P_DRM_AddFilter(SESSION_HANDLE pHSession, HUINT32 nPid);

#define TS_READ_16( BUF ) ((uint16_t)((BUF)[0]<<8|(BUF)[1]))
#define TS_PSI_GET_SECTION_LENGTH( BUF )    (uint16_t)(TS_READ_16( &(BUF)[1] ) & 0x0FFF)

void P_DRM_message_callback(void *context, int param)
{
	SESSION_HANDLE pHSession = (SESSION_HANDLE)context;
	NEXUS_MessageHandle message; //= (NEXUS_MessageHandle *)param;
	HINT32 nMsgIndex = param;

	HUINT8 *buffer= NULL;
	HUINT32 size = 0;
	NEXUS_Error 	tNexusError = NEXUS_SUCCESS;
	HINT32 message_length;
	HUINT32		nProgramNum = 0;

	//PrintEnter;

	DRM_Session_t *drm_handle;

	drm_handle = P_DRM_GetInstance(pHSession);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return;
	}

	message = drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage;


	if(message==NULL)
	{
		PrintDebug("%s(%d) message == NULL\n",__FUNCTION__, __LINE__);
		return;
	}

	tNexusError = NEXUS_Message_GetBuffer(message, (const void **)&buffer, &size);
	if(tNexusError != NEXUS_SUCCESS || size == 0)
	{
		PrintDebug("%s(%d), NEXUS_Message_GetBuffer() return error (%d)[size=%d]\n",__FUNCTION__, __LINE__, tNexusError, size);
		return;
	}

	message_length = 0;
	message_length = buffer[1] & 0x0f;
	message_length <<= 8;
	message_length = message_length | (unsigned char)buffer[2];
	message_length = message_length + 3; /* message_length's Position */


	if( size < (HUINT32)message_length )
	{
		PrintDebug("%08X buffer[1] = %02X, [2] = %02X\n", message_length, (unsigned char)buffer[1], (unsigned char)buffer[2]);
		PrintError("%s(%d), size < message_length = [%d < %d]\n",__FUNCTION__, __LINE__, size, message_length);

		if (message_length % 4)
		{
			message_length += 4 - (message_length % 4);
		}

		NEXUS_Message_ReadComplete(message, message_length); /* delete message buf */

		return;
	}

	if (drm_handle->drm_info.drm.marlin.bPAT_Enable==FALSE && buffer[0] == 0)
	{
		/* Program Association Table */
		for (nProgramNum=0;nProgramNum<(unsigned)(TS_PSI_GET_SECTION_LENGTH(buffer)-7)/4;nProgramNum++)
		{
			unsigned byteOffset = 8 + nProgramNum*4;
			unsigned short pid = (uint16_t)(TS_READ_16( &buffer[byteOffset+2] ) & 0x1FFF);

			if(P_DRM_AddFilter(pHSession, pid) != DRV_OK)
			{
				PrintError("P_MEDIA_AddFilter failed!\n");
				return;
			}
		}
	}
	else if(drm_handle->drm_info.drm.marlin.bPMT_Enable==FALSE && (buffer[0] == 2))
	{
		/* Program Table */
		HINT32 nEcmPid= 0;
		if( MARLIN_UpdatePmt(	drm_handle->private.marlin.pHMarlin,
								(char *)buffer,
								message_length,
								&nEcmPid) != MARLIN_SUCCESS )
		{
			PrintError("%s(%d) WARN Marlin Update PMT Fail\n",__FUNCTION__,__LINE__);
			return;
		}
		if(P_DRM_AddFilter(pHSession, (HUINT32)nEcmPid) != DRV_OK)
		{
			PrintError("P_MEDIA_AddFilter failed!\n");
			return;
		}


	}
	else if(((HUINT8)buffer[0]==(HUINT8)0x80) || ((HUINT8)buffer[0]==(HUINT8)0x81) )
	{
		if( MARLIN_UpdateEcm(	drm_handle->private.marlin.pHMarlin,
								(char *)buffer,
								message_length) != MARLIN_SUCCESS )
		{
			PrintError("%s(%d) WARN Marlin Update ECM Fail\n",__FUNCTION__,__LINE__);
			return;
		}
	}
	else
	{
		//PrintDebug("%s(%d) buffer[0] = 0x%02X\n", __FUNCTION__, __LINE__, buffer[0]);
	}

	/* XPT HW is configured to pad all messages to 4 bytes.
	If we are calling NEXUS_Message_ReadComplete
	based on message length and not the size returned by NEXUS_Message_GetBuffer, then we must add that pad.
	If we are wrong, NEXUS_Message_ReadComplete will fail. */
	if (message_length % 4)
	{
		message_length += 4 - (message_length % 4);
	}

	(void)NEXUS_Message_ReadComplete(message, message_length); /* delete message buf */

	//PrintExit;
	return;
}

static DRV_Error P_DRM_AddFilter(SESSION_HANDLE pHSession, HUINT32 nPid)
{
	NEXUS_MessageSettings settings;
	NEXUS_MessageStartSettings startSettings;
	NEXUS_PidChannelHandle pidChannel;
	NEXUS_Error rc;
	HINT32 nMsgIndex;
	DRV_Error dResult = DRV_OK;

	//PrintEnter;

	DRM_Session_t *drm_handle;

	drm_handle = P_DRM_GetInstance(pHSession);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	for(nMsgIndex=0; nMsgIndex < MAX_MARLIN_MESSAGE_FILTERS; ++nMsgIndex)
	{
		if(drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].bUsed == TRUE && drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].nPid == nPid)
		{
			//PrintDebug("warning : already filter pid : %03X\n", nPid);
			return DRV_OK;
		}
		if(drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage == NULL)
		{
			break;
		}
	}

	pidChannel = NEXUS_Playback_OpenPidChannel(drm_handle->private.marlin.pstPlayback, nPid, NULL);

	drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].bUsed = TRUE;
	drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].nPid = nPid;
	//drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage = (void*)malloc(sizeof(NEXUS_MessageHandle));

	(void)NEXUS_Message_GetDefaultSettings(&settings);
	settings.dataReady.callback = P_DRM_message_callback;
	settings.dataReady.context = (void*)pHSession;
	settings.dataReady.param = (int)nMsgIndex;
	settings.maxContiguousMessageSize = 4096; /*MEDIA_MPEG2TS_188PKT_LEN;*/
	settings.bufferSize = 4096;

	drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage = NEXUS_Message_Open(&settings);

	if((NEXUS_MessageHandle)(drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage) == NULL)
	{
		PrintError("NEXUS_Message_Open Error\n");
		dResult = DRV_ERR;
	}
	else
	{
		PrintDebug("drm_info.drm.marlin.msg_t[%d]->pHMessage = %d\n", nMsgIndex,
			(int)(NEXUS_MessageHandle)drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage);

	}

	(void)NEXUS_Message_GetDefaultStartSettings((NEXUS_MessageHandle)drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage, &startSettings);
	startSettings.pidChannel = pidChannel;
	startSettings.psiCrcDisabled = true;
	startSettings.psfCrcDisabled = true;

	rc = NEXUS_Message_Start((NEXUS_MessageHandle)drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage, &startSettings);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Message_Start Error\n");
		dResult = DRV_ERR;
	}

	//PrintExit;
	return dResult;
}

static DRV_Error P_DRM_FilterSetup(SESSION_HANDLE pHSession, DRM_Key_Info_t *drm_key_info)
{
	NEXUS_PidChannelStatus pidStatus;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;

	//PrintEnter;

	DRM_Session_t *drm_handle;

	drm_handle = P_DRM_GetInstance(pHSession);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	drm_handle->private.marlin.pstPlayback = drm_key_info->offer.marlin.pstPlayback;

	if(*drm_key_info->offer.marlin.videoPidChannel != NULL)
	{
		NEXUS_Playback_ClosePidChannel(drm_handle->private.marlin.pstPlayback, *drm_key_info->offer.marlin.videoPidChannel);
		NEXUS_Playback_ClosePidChannel(drm_handle->private.marlin.pstPlayback, *drm_key_info->offer.marlin.audioPidChannel);
	}

	{
	    /* video */
		NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
		playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
		playbackPidSettings.pidTypeSettings.video.codec = drm_key_info->offer.marlin.video_stream_id;
		playbackPidSettings.pidTypeSettings.video.index = true;
		playbackPidSettings.pidTypeSettings.video.decoder = NULL;
		*drm_key_info->offer.marlin.videoPidChannel= NEXUS_Playback_OpenPidChannel(drm_handle->private.marlin.pstPlayback, drm_key_info->offer.marlin.video_stream_id, &playbackPidSettings);

		/* audio */
		NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
		playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
		playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = NEXUS_AudioCodec_eUnknown; /* Decode will set this later */
		playbackPidSettings.pidTypeSettings.audio.primary = NULL;/* Decode will set this later */
		playbackPidSettings.pidTypeSettings.audio.secondary = NULL; /* Decode will set this later */
		*drm_key_info->offer.marlin.audioPidChannel = NEXUS_Playback_OpenPidChannel(drm_handle->private.marlin.pstPlayback, drm_key_info->offer.marlin.audio_stream_id, &playbackPidSettings);
	}

	NEXUS_PidChannel_GetStatus(*drm_key_info->offer.marlin.videoPidChannel, &pidStatus);
	drm_handle->drm_info.drm.marlin.videoPID_Index = pidStatus.pidChannelIndex;

	NEXUS_PidChannel_GetStatus(*drm_key_info->offer.marlin.audioPidChannel, &pidStatus);
	drm_handle->drm_info.drm.marlin.audioPID_Index = pidStatus.pidChannelIndex;

	if(P_DRM_AddFilter(pHSession, PAT_PID) != DRV_OK)
	{
		PrintError("P_MEDIA_AddFilter failed!\n");
		return DRV_ERR;
	}


	//PrintExit;
	return DRV_OK;
}

static DRV_Error P_DRM_FilterRelease(SESSION_HANDLE pHSession)
{
	HINT32		nMsgIndex;
	DRM_Session_t *drm_handle;

	if(pHSession == NULL)
	{
		PrintError("Invalid parameter, pHSession is NULL!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance(pHSession);
	if(drm_handle == NULL)
	{
		PrintError("There's no matching drm handle!\n");
		return DRV_ERR;
	}


	for(nMsgIndex=0; nMsgIndex < MAX_MARLIN_MESSAGE_FILTERS; ++nMsgIndex)
	{
		if(drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].bUsed == TRUE && drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage != NULL)
		{
			(void)NEXUS_Message_Stop(drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage);
			(void)NEXUS_Message_Close(drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage);
			drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].pHMessage=NULL;
			drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].bUsed=FALSE;
			PrintDebug("Filter PID : %02X Release Complete\n", drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].nPid);
			drm_handle->drm_info.drm.marlin.msg_t[nMsgIndex].nPid=0;
		}
	}

	return DRV_OK;
}



static DRV_Error P_DRM_ReleaseSecuritySettings(SESSION_HANDLE media_session_handle )
{
	DRM_Session_t *drm_handle;

	if(media_session_handle == NULL)
	{
		PrintError("Invalid parameter, pHSession is NULL!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("There's no matching drm handle!\n");
		return DRV_ERR;
	}


	if(drm_handle->private.marlin.videoKeyHandle != NULL)
	{
		NEXUS_Security_FreeKeySlot(drm_handle->private.marlin.videoKeyHandle);
		drm_handle->private.marlin.videoKeyHandle = NULL;
	}

	if(drm_handle->private.marlin.audioKeyHandle != NULL)
	{
		NEXUS_Security_FreeKeySlot(drm_handle->private.marlin.audioKeyHandle);
		drm_handle->private.marlin.audioKeyHandle = NULL;
	}

	return DRV_OK;

}

static DRV_Error P_DRM_SetupSecuritySettings(SESSION_HANDLE media_session_handle )
{
    NEXUS_SecurityAlgorithmSettings AlgConfig;
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    DRM_Session_t *drm_handle;

	if(media_session_handle == NULL)
	{
		PrintError("Invalid parameter, pHSession is NULL!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("There's no matching drm handle!\n");
		return DRV_ERR;
	}

    /***************************************************************************************
     Config CA descrambler
    ***************************************************************************************/
    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eCa;

    /* Allocate AV keyslots */
    /* KeyHandle will modify oneHandle : video and audio Key is same */
    drm_handle->private.marlin.videoKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
    if ( !drm_handle->private.marlin.videoKeyHandle)
    {
      PrintError("\nAllocate CA video keyslot failed \n");
    }
    drm_handle->private.marlin.audioKeyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
    if ( !drm_handle->private.marlin.audioKeyHandle)
    {
      PrintError("\nAllocate CA audio keyslot failed \n");
    }


    /* Config AV algorithms */
    NEXUS_Security_GetDefaultAlgorithmSettings(&AlgConfig);

#if 0
    AlgConfig.algorithm       = NEXUS_SecurityAlgorithm_eAes;
    AlgConfig.algorithmVar    = NEXUS_SecurityAlgorithmVariant_eCbc;
    AlgConfig.terminationMode = NEXUS_SecurityTerminationMode_eBlock;
    AlgConfig.operation       = NEXUS_SecurityOperation_eDecrypt;
    AlgConfig.dest			  = NEXUS_SecurityAlgorithmConfigDestination_eCa;
    AlgConfig.scValue[0]	  = NEXUS_SecurityAlgorithmScPolarity_eClear;
    AlgConfig.keyDestEntryType	= NEXUS_SecurityKeyType_eOddAndEven;
#else
	AlgConfig.key2Select	= NEXUS_SecurityKey2Select_eReserved1;
	AlgConfig.askmModuleID	= NEXUS_SecurityAskmModuleID_eModuleID_3;

    AlgConfig.algorithm 		= NEXUS_SecurityAlgorithm_eAes;
	AlgConfig.algorithmVar 		= NEXUS_SecurityAlgorithmVariant_eCbc;
	AlgConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eCbcResidual;
	AlgConfig.solitarySelect	= NEXUS_SecuritySolitarySelect_eCbcXorIV;

	AlgConfig.scValue[NEXUS_SecurityPacketType_eGlobal] 	= NEXUS_SecurityAlgorithmScPolarity_eClear;
	AlgConfig.scValue[NEXUS_SecurityPacketType_eRestricted] = NEXUS_SecurityAlgorithmScPolarity_eClear;

	AlgConfig.operation       = NEXUS_SecurityOperation_eDecrypt;
	AlgConfig.dest			  = NEXUS_SecurityAlgorithmConfigDestination_eCa;
	//AlgConfig.keyDestEntryType	= NEXUS_SecurityKeyType_eOdd;
	AlgConfig.keyDestEntryType	= NEXUS_SecurityKeyType_eOddAndEven;

	AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = FALSE;
	//AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eRestricted] = TRUE;
#endif

#if DIFF_IV_KEY

#else
    //AlgConfig.modifyScValue[NEXUS_SecurityPacketType_eGlobal] = true;
    if ( NEXUS_Security_ConfigAlgorithm (drm_handle->private.marlin.videoKeyHandle, &AlgConfig) != 0 )
    {
        PrintError("\nConfig video CA Algorithm failed \n");
    }
    if ( NEXUS_Security_ConfigAlgorithm (drm_handle->private.marlin.audioKeyHandle, &AlgConfig) != 0 )
    {
        PrintError("\nConfig audio CA Algorithm failed \n");
    }
#endif



    /* Add video PID channel to EVEN keyslot */
    if ( NEXUS_Security_AddPidChannelToKeySlot(drm_handle->private.marlin.videoKeyHandle, drm_handle->drm_info.drm.marlin.videoPID_Index)!= 0 )
    {
        PrintError("\nConfigPIDPointerTable failed \n");
    }

    /* Add audio PID channel to EVEN keyslot */
    if ( NEXUS_Security_AddPidChannelToKeySlot(drm_handle->private.marlin.audioKeyHandle, drm_handle->drm_info.drm.marlin.audioPID_Index)!= 0 )
    {
    	PrintError("\nConfigPIDPointerTable failed \n");
    }


    return DRV_OK;
}

static DRV_Error P_DRM_SetDecryptKeys(SESSION_HANDLE media_session_handle)
{
    NEXUS_SecurityClearKey key;
    int key_len = 16;
    DRM_Session_t *drm_handle;


	if(media_session_handle == NULL)
	{
		PrintError("Invalid parameter, pHSession is NULL!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("There's no matching drm handle!\n");
		return DRV_ERR;
	}

#if 1
   	if( drm_handle->private.marlin.bFirstKey==TRUE || drm_handle->private.marlin.bOddKey==TRUE )
   	{
	    /* Load AV keys */
	    NEXUS_Security_GetDefaultClearKey(&key);
	    key.keySize = key_len;
	    key.keyEntryType = NEXUS_SecurityKeyType_eEven;

	    BKNI_Memcpy(key.keyData, drm_handle->private.marlin.EvenKey, key.keySize);
	    if ( NEXUS_Security_LoadClearKey (drm_handle->private.marlin.videoKeyHandle, &key) != 0 )
	    {
	       PrintError("\nLoad video EVEN key failed \n");
	    }
	    if ( NEXUS_Security_LoadClearKey (drm_handle->private.marlin.audioKeyHandle, &key) != 0 )
	    {
	       PrintError("\nLoad audio EVEN key failed \n");
	    }

    }
#endif
#if 1
   	if( drm_handle->private.marlin.bFirstKey==TRUE || drm_handle->private.marlin.bOddKey==FALSE)
   	{
		NEXUS_Security_GetDefaultClearKey(&key);
	    key.keySize = key_len;
	    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
	    BKNI_Memcpy(key.keyData, drm_handle->private.marlin.OddKey, key.keySize);

	    if ( NEXUS_Security_LoadClearKey (drm_handle->private.marlin.videoKeyHandle, &key) != 0 )
	    {
	       PrintError("\nLoad video ODD key failed \n");
	    }
	    if ( NEXUS_Security_LoadClearKey (drm_handle->private.marlin.audioKeyHandle, &key) != 0 )
	    {
	       PrintError("\nLoad audio ODD key failed \n");
	    }

	}
#endif
	if( drm_handle->private.marlin.bFirstKey==TRUE)
	{
		/* VIDEO */
	    key.keySize = key_len;
	    key.keyEntryType = NEXUS_SecurityKeyType_eOddAndEven;
	    key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
	    BKNI_Memcpy(key.keyData, drm_handle->private.marlin.CurIv, key.keySize);
	    if ( NEXUS_Security_LoadClearKey (drm_handle->private.marlin.videoKeyHandle, &key) != 0 )
	    {
	       PrintError("\nLoad video EVEN IV failed \n");
	    }

	    /* AUDIO */
		key.keySize = key_len;
		key.keyEntryType = NEXUS_SecurityKeyType_eOddAndEven;
		key.keyIVType = NEXUS_SecurityKeyIVType_eIV;
		BKNI_Memcpy(key.keyData, drm_handle->private.marlin.CurIv, key.keySize);

	    if ( NEXUS_Security_LoadClearKey (drm_handle->private.marlin.audioKeyHandle, &key) != 0 )
	    {
	       PrintError("\nLoad audio EVEN IV failed \n");
	    }

	    drm_handle->private.marlin.bFirstKey=FALSE;

	}
	else
	{
		//PrintDebug("bFirstKey == FALSE\n");
	}

	return DRV_OK;
}




DRM_Session_t *P_DRM_GetInstanceFromMarlinHandle(WSB_DRM pHMarlin)
{
	int i;

	for(i=0; i<MAX_DRM_SESSION; i++)
	{
		if(s_DRMSession[i].private.marlin.pHMarlin == pHMarlin)
		{
			return &s_DRMSession[i];
		}
	}

	return NULL;
}


int P_DRM_Marlin_NewMediaFileCb(WSB_DRM pHMarlin, void *media_file, void *key_manager)
{
	return MARLIN_NewMediaFile(pHMarlin, media_file, key_manager);

}

static void P_DRM_Marlin_KsmInfoChangeCb(WSB_DRM pHMarlin, void *media_info, HUINT32 change_mask)
{
	DRM_Session_t *drm_handle;
	HINT32 flag=0;

	if(pHMarlin == NULL)
	{
		PrintError("Invalid parameter, pHMarlin is NULL!\n");
		return;
	}

	drm_handle = P_DRM_GetInstanceFromMarlinHandle(pHMarlin);
	if(drm_handle == NULL)
	{
		PrintError("There's no matching marlin handle!\n");
		return;
	}

	(void)MARLIN_KsmInfoChange(pHMarlin, media_info, change_mask);

	flag = MARLIN_GetClearKey(pHMarlin, (char*)drm_handle->private.marlin.EvenKey, (char*)drm_handle->private.marlin.OddKey);
	if(flag == MARLIN_FAILURE)
	{
		PrintError("MARLIN_GetClearKey!\n");
		return;
	}
	else if( flag == 2 )
	{
		drm_handle->private.marlin.bOddKey=FALSE;
	}
	else if( flag == 1 )
	{
		drm_handle->private.marlin.bOddKey=TRUE;
	}
	else
	{
		drm_handle->private.marlin.bFirstKey=TRUE;
	}

	if(drm_handle->private.marlin.bRewind == TRUE)
	{
		drm_handle->private.marlin.bFirstKey=TRUE;
	}

	if(MARLIN_GetIv(pHMarlin, (char*)drm_handle->private.marlin.CurIv, (char*)drm_handle->private.marlin.NextIv) == MARLIN_FAILURE)
	{
		PrintError("MARLIN_GetIv!\n");
		return;
	}

	if(P_DRM_SetDecryptKeys(drm_handle->media_session_handle) != DRV_OK)
	{
		return;
	}

	return;
}

#endif

/*******************************************************************/
/****************** Global Function Prototypes *********************/
/*******************************************************************/
#if (NEXUS_VERSION >= 1290) /* for playready */
#if defined (CONFIG_PLAYREADY)
DRV_Error DRV_DRM_RegisterPostCallback(void)
{
	b_asf_register_post_callback(bhttpclient_license_post_soap, bhttpclient_license_post_default);

	return DRV_OK;
}

DRV_Error DRV_DRM_RegisterSecureClockCallback(void)
{
	b_asf_register_get_secure_clock(P_DRM_PlayReady_GetSecureClock);

	return DRV_OK;
}
#endif
#endif

DRV_Error DRV_DRM_CreateSession(SESSION_HANDLE media_session_handle)
{
	DRM_Session_t *drm_handle;

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle != NULL)
	{
		PrintError("media session had drm session already.!!!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_NewInstance();
	if(drm_handle == NULL)
	{
		PrintError("drm session is full.!!!\n");
		return DRV_ERR;
	}

	drm_handle->media_session_handle = media_session_handle;
	drm_handle->used = TRUE;

	/* project 별로 설정에 의한 default DRM Type을 넣어 준다. */
	drm_handle->drm_info.drm_type = s_defaultDRMType;

	return DRV_OK;
}

DRV_Error DRV_DRM_GetInfo(SESSION_HANDLE media_session_handle, DRM_Info_t *drm_info)
{
	DRM_Session_t *drm_handle;

	if(drm_info == NULL)
	{
		PrintError("drm_info is NULL.!!!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	VK_MEM_Memcpy(drm_info, &drm_handle->drm_info, sizeof(DRM_Info_t));

	return DRV_OK;
}

DRV_Error DRV_DRM_ResigterCallback(SESSION_HANDLE media_session_handle, DRM_Info_t *drm_info)
{
	DRM_Session_t *drm_handle;

	if(drm_info == NULL)
	{
		PrintError("drm_info is NULL.!!!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}
#if 0
	if(drm_info->drm_type != drm_handle->drm_info.drm_type)
	{
		PrintError("drm type is not match.!!\n");
		return DRV_ERR;
	}
#endif
	VK_MEM_Memcpy(&drm_handle->drm_info, drm_info, sizeof(DRM_Info_t));

#if defined (CONFIG_PLAYREADY)
#if (NEXUS_VERSION >= 1290)
	FILE* fd = NULL;

	if(drm_handle->drm_info.drm.playready.pPrivateKeyPath != NULL)
	{
		fd = fopen("/var/lib/humaxtv/drm/privatekey.path", "wb");
		if(fd == NULL)
		{
			PrintError("fopen privatekey.path failed!\n");
			return DRV_ERR;
		}
		fwrite(drm_handle->drm_info.drm.playready.pPrivateKeyPath,
				VK_strlen(drm_handle->drm_info.drm.playready.pPrivateKeyPath),
				1,
				fd);
		VK_fflush(fd);
		fclose(fd);
	}

	if(drm_handle->drm_info.drm.playready.pCertificatePath != NULL)
	{
		fd = fopen("/var/lib/humaxtv/drm/certificate.path", "wb");
		if(fd == NULL)
		{
			PrintError("fopen certificate.path failed!\n");
			return DRV_ERR;
		}
		fwrite(drm_handle->drm_info.drm.playready.pCertificatePath,
				VK_strlen(drm_handle->drm_info.drm.playready.pCertificatePath),
				1,
				fd);
		VK_fflush(fd);
		fclose(fd);
	}

	if(drm_handle->drm_info.drm.playready.licenseURL != NULL)
	{
		fd = fopen("/var/lib/humaxtv/drm/license.url", "wb");
		if(fd == NULL)
		{
			PrintError("fopen certificate.path failed!\n");
			return DRV_ERR;
		}
		fwrite(drm_handle->drm_info.drm.playready.licenseURL,
				VK_strlen(drm_handle->drm_info.drm.playready.licenseURL),
				1,
				fd);
		VK_fflush(fd);
		fclose(fd);
	}
#endif

	DRV_Error dResult;
	if((drm_info->drm_type == DI_DRM_TYPE_WMDRM) || (drm_info->drm_type == DI_DRM_TYPE_PLAYREADY))
	{
		dResult = P_DRM_CreateCrypto(media_session_handle);
		if(dResult != DRV_OK)
		{
			PrintError("P_DRM_InitCrypto!\n");
			return DRV_ERR;
		}
	}
#endif

#if defined (CONFIG_PLAYREADY_25_BRCM)
	if(drm_info->drm_type == DI_DRM_TYPE_PLAYREADY_25_BRCM)
	{
		CommonCryptoSettings cmnCryptoSettings;
		HUINT8 *pWrmHeaderWideChar = NULL;
		HINT32 nWrmHeaderWideCharSize = 0;
		HINT32 count = 0;

		PrintDebug("PLAYREADY_25_BRCM\n");
#if 0	/* 만약 multi-session을 사용 한다면, 아래의 CustomHeader 이외에도 wrmHeader등의 String에 대한 Resource정리가 추가로 필요하다. */
		for(count=0; count < drm_info->drm.playready.unHttpCustomHeaders; ++count)
		{
			 drm_handle->drm_info.drm.playready.tHttpCustomHeader[i].pName = VK_StrDup(drm_info->drm.playready.tHttpCustomHeader[i].pName);
			 drm_handle->drm_info.drm.playready.tHttpCustomHeader[i].pValue = VK_StrDup(drm_info->drm.playready.tHttpCustomHeader[i].pValue)
		}
#endif
		DRM_Prdy_GetDefaultParamSettings(&drm_handle->private.playready.prdyParamSettings);

		drm_handle->private.playready.drm_context =  DRM_Prdy_Initialize( &drm_handle->private.playready.prdyParamSettings);
		if( drm_handle->private.playready.drm_context == NULL) {
			PrintError("Failed to create drm_context, quitting....\n");
			return DRV_ERR;
		}

		/* CHAR -> WCHAR */
		nWrmHeaderWideCharSize = (drm_handle->drm_info.drm.playready.nWrmHeaderLen * 2);
		pWrmHeaderWideChar = BKNI_Malloc(nWrmHeaderWideCharSize+1);
		if(pWrmHeaderWideChar == NULL)
		{
			PrintError("%s(%d) BKNI_Malloc(nWrmHeaderWideCharSize) failed, exiting...", __FUNCTION__, __LINE__);
			return DRV_ERR;
		}
		BKNI_Memset(pWrmHeaderWideChar, 0, nWrmHeaderWideCharSize+1);

		for(count=0; count<nWrmHeaderWideCharSize ; count+=2)
		{
			pWrmHeaderWideChar[count] = drm_handle->drm_info.drm.playready.pucWrmHeader[count/2];
			pWrmHeaderWideChar[count+1] = '\0';
		}

		if( DRM_Prdy_Content_SetProperty(drm_handle->private.playready.drm_context,
											DRM_Prdy_contentSetProperty_eAutoDetectHeader,
											pWrmHeaderWideChar, nWrmHeaderWideCharSize) != DRM_Prdy_ok)
		{
			PrintError("%s(%d) Failed to SetProperty for the KID, exiting...", __FUNCTION__, __LINE__);
			if(pWrmHeaderWideChar != NULL)
				BKNI_Free(pWrmHeaderWideChar);
			return DRV_ERR;
		}

		/*commonCryptoHandle*/
		CommonCrypto_GetDefaultSettings(&cmnCryptoSettings);
		drm_handle->private.playready.commonCryptoHandle = CommonCrypto_Open(&cmnCryptoSettings);
		if( drm_handle->private.playready.commonCryptoHandle == NULL) {
			PrintError("Failed to create commonCryptoHandle, quitting....\n");
			if(pWrmHeaderWideChar != NULL)
				BKNI_Free(pWrmHeaderWideChar);
            return DRV_ERR;
        }
		if(pWrmHeaderWideChar != NULL)
			BKNI_Free(pWrmHeaderWideChar);
	}
#endif

#if defined (CONFIG_MARLIN_DRM)
	if(drm_handle->drm_info.drm_type == DI_DRM_TYPE_MARLIN)
	{
		/* Marlin Init & Personalization */
		if( drm_handle->private.marlin.pHMarlin == NULL )
		{
			if( MARLIN_Initialize(&drm_handle->private.marlin.pHMarlin) != MARLIN_SUCCESS )
			{
				PrintError("DRM_Initialize!\n");
				return DRV_ERR;
			}
		}
		/* DrmInfo Update*/
		else
		{
			/* */
		}
	}
#endif

	return DRV_OK;
}

DRV_Error DRV_DRM_GetLicence(SESSION_HANDLE media_session_handle)
{
	DRM_Session_t *drm_handle;

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	switch(drm_handle->drm_info.drm_type)
	{
		case DI_DRM_TYPE_WMDRM:
		case DI_DRM_TYPE_PLAYREADY:
		{
#if defined (CONFIG_PLAYREADY)
#if (NEXUS_VERSION < 1290)
			DRV_Error dResult;

			dResult = P_DRM_PlayReady_SetDecryptCb(media_session_handle);
			if(dResult != DRV_OK)
			{
				PrintError("P_DRM_PlayReady_SetDecryptCb!\n");
				return DRV_ERR;
			}

			dResult = P_DRM_PlayReady_GetLicense(media_session_handle,
				drm_handle->drm_info.drm.playready.pucWrmHeader,
				drm_handle->drm_info.drm.playready.nWrmHeaderLen,
				drm_handle->drm_info.drm.playready.pPrivateKeyPath,
				drm_handle->drm_info.drm.playready.pCertificatePath);
			if(dResult != DRV_OK)
			{
				PrintError("P_DRM_PlayReady_GetLicense failed!\n");
				return DRV_ERR;
			}
#else
			if(drm_handle->drm_info.drm.playready.pucWrmHeader != NULL)
			{
				FILE* fd = NULL;
				fd = fopen("/var/lib/humaxtv/drm/wrm.header", "wb");
				if(fd == NULL)
				{
					PrintError("fopen wrm.header failed!\n");
					return DRV_ERR;
				}

				fwrite(drm_handle->drm_info.drm.playready.pucWrmHeader,
						drm_handle->drm_info.drm.playready.nWrmHeaderLen,
						1,
						fd);
				VK_fflush(fd);
				fclose(fd);

				if (drm_handle->drm_info.cbSetLicenseData)
				{
					drm_handle->drm_info.cbSetLicenseData(media_session_handle, NULL, 0);
				}
			}
#endif
#endif
			break;
		}
#if defined(CONFIG_PLAYREADY_25_BRCM)
		case DI_DRM_TYPE_PLAYREADY_25_BRCM:
		{
			DRV_Error dResult = DRV_OK;

			dResult = P_DRM_PlayReady_BRCM_GetLicense(media_session_handle,
				drm_handle->drm_info.drm.playready.pucWrmHeader,
				drm_handle->drm_info.drm.playready.nWrmHeaderLen);
			if(dResult != DRV_OK)
			{
				PrintError("P_DRM_PlayReady_GetLicense failed!\n");
				return DRV_ERR;
			}

			break;
		}
#endif
		case DI_DRM_TYPE_VIEWRIGHTWEB:
		{
#if defined (CONFIG_VIEWRIGHTWEB)
			/* 필요한 작업 추가 */
			VRWebClientError_t 		errCode;
			if(!drm_handle->private.viewrightweb.bInit)
			{
				WebClient_ViewRight_InitializeSSL();
				WebClient_ViewRight_SetVCASCommunicationHandlerSettings((const char*)drm_handle->drm_info.drm.viewrightweb.szVCASBootAddr,(const char*)drm_handle->private.viewrightweb.szStorePath,(const char*)drm_handle->private.viewrightweb.szCompanyName);
				WebClient_ViewRight_SetLogging(FALSE);
				WebClient_ViewRight_SetKeyRetrievalStatusCallback(NULL, (KeyRetrCallback)drm_handle->drm_info.drm.viewrightweb.SetKeyRetrievalStatusCallback);
				drm_handle->private.viewrightweb.bInit = TRUE;
			}
			errCode = WebClient_ViewRight_IsDeviceProvisioned();
			if(errCode != VR_Success)
			{
				errCode = WebClient_ViewRight_CheckVCASConnection();
				if(errCode == CertificateExpired)
				{
					PrintError("%s(%d) Err> VRW Certification Expired.\n",__FUNCTION__,__LINE__);
					drm_handle->private.viewrightweb.bVCASConnectAndProvision = FALSE;
					return DRV_ERR;
				}
				errCode = WebClient_ViewRight_ConnectAndProvisionDevice();
				if(errCode != VR_Success)
				{
					PrintError("%s(%d) Err> VRW Connect and Provision Fail(errcode : %d)\n",__FUNCTION__,__LINE__,errCode);
					drm_handle->private.viewrightweb.bVCASConnectAndProvision = FALSE;
					return DRV_ERR;
				}
				errCode = WebClient_ViewRight_CheckVCASConnection();
				if(errCode != VR_Success)
				{
					PrintError("%s(%d) Err> VRW Connection Fail(errcode : %d)\n",__FUNCTION__,__LINE__,errCode);
					drm_handle->private.viewrightweb.bVCASConnectAndProvision = FALSE;
					return DRV_ERR;
				}
				drm_handle->private.viewrightweb.bVCASConnectAndProvision = TRUE;
			}
#endif
			break;
		}
		case DI_DRM_TYPE_MARLIN:
		{
#if defined (CONFIG_MARLIN_DRM)
			if(drm_handle->drm_info.drm.marlin.msg.MessageURL != NULL)
			{
				if(drm_handle->private.marlin.ContentAccessURL == NULL)
				{
					//TODO : Get SAS form MS3 Server
					if(MARLIN_Setup(drm_handle->private.marlin.pHMarlin,
								(MARLIN_OnNewMediaFile)P_DRM_Marlin_NewMediaFileCb,
								(MARLIN_OnKsmInfoChange)P_DRM_Marlin_KsmInfoChangeCb,
								(char*)drm_handle->drm_info.drm.marlin.msg.MessageURL) == MARLIN_FAILURE)
					{
						return DRV_ERR;
					}
					/* stored access url */
					drm_handle->private.marlin.ContentAccessURL = VK_StrDup(drm_handle->drm_info.drm.marlin.msg.MessageURL);
				}
				else
				{
					void *Result;
					PrintDebug("drm_handle->private.marlin.ContentAccessURL = %s \n", drm_handle->private.marlin.ContentAccessURL);

					Result = VK_MEM_Memset(drm_handle->drm_info.drm.marlin.msg.MessageURL, 0x00, VK_strlen((char*)drm_handle->drm_info.drm.marlin.msg.MessageURL));
					if(Result != drm_handle->drm_info.drm.marlin.msg.MessageURL)
					{
						return DRV_ERR;
					}
					Result = VK_MEM_Memcpy(drm_handle->drm_info.drm.marlin.msg.MessageURL, drm_handle->private.marlin.ContentAccessURL, VK_strlen(drm_handle->private.marlin.ContentAccessURL));
					if(Result != drm_handle->drm_info.drm.marlin.msg.MessageURL)
					{
						return DRV_ERR;
					}

					PrintDebug("drm_handle->drm_info.drm.marlin.msg.MessageURL = %s \n", drm_handle->drm_info.drm.marlin.msg.MessageURL);
				}
			}
#endif
			break;
		}
		default :
			PrintError("Invalid DRM Type (%d)\n", drm_handle->drm_info.drm_type);
			return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_DRM_GetKey(SESSION_HANDLE media_session_handle, DRM_Key_Info_t *drm_key_info)
{
	DRM_Session_t *drm_handle;

	if(drm_key_info == NULL)
	{
		PrintError("drm_key_info is NULL.!!!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	switch(drm_handle->drm_info.drm_type)
	{
		case DI_DRM_TYPE_PLAYREADY:
		{
			/* 필요한 작업 추가 */
			break;
		}
		case DI_DRM_TYPE_VIEWRIGHTWEB:
		{
			/* 필요한 작업 추가 */
#if defined (CONFIG_VIEWRIGHTWEB)
			DI_ERR_CODE errCode = DI_ERR_ERROR;
			if(!drm_handle->private.viewrightweb.bVCASConnectAndProvision)
			{
				PrintError("%s(%d) Err> VRW has not prepared (Connect And Provision).\n",__FUNCTION__,__LINE__);
				return DRV_ERR;
			}
			else
			{
				if(	drm_handle->drm_info.drm.viewrightweb.KeyRetrvCallback == NULL )
				{
					PrintError("%s(%d) Err> VRW has not prepared (KeyRetrvCallback).\n",__FUNCTION__,__LINE__);
					if(WebClient_ViewRight_RetrieveKeyFile(drm_key_info->offer.viewrightweb.pUrl,drm_key_info->result.viewrightweb.pKey) != VR_Success)
					{
						PrintError("%s(%d) Err> VRW Get Key Fail\n",__FUNCTION__,__LINE__);
						return DRV_ERR;
					}
				}
				else
				{
					(void)drm_handle->drm_info.drm.viewrightweb.KeyRetrvCallback(drm_key_info->offer.viewrightweb.pUrl, drm_key_info->result.viewrightweb.pKey,drm_key_info->result.viewrightweb.puKeySize, &errCode);
					if(errCode != DI_ERR_OK)
					{
						PrintError("%s(%d) Err> VRW Get Key Fail\n",__FUNCTION__,__LINE__);
						return DRV_ERR;
					}
				}
			}
#endif
			break;
		}
		case DI_DRM_TYPE_MARLIN:
		{
#if defined (CONFIG_MARLIN_DRM)
			if(drm_key_info->offer.marlin.cmd == DI_DRM_MARLIN_SETUP)
			{
				if( P_DRM_FilterSetup(media_session_handle, drm_key_info) != DRV_OK )
				{
					PrintError("%s(%d) ERR Marlin Filter Setup Fail\n",__FUNCTION__,__LINE__);
					return DRV_ERR;
				}
				if(drm_handle->private.marlin.videoKeyHandle == NULL && drm_handle->private.marlin.audioKeyHandle == NULL)
				{
					if( P_DRM_SetupSecuritySettings(media_session_handle) != DRV_OK )
					{
						PrintError("%s(%d) ERR Marlin Setup Security Setting Fail\n",__FUNCTION__,__LINE__);
						return DRV_ERR;
					}
				}

				/* FirstKey Update */
				if(drm_handle->private.marlin.pmt_section_size) /* PMT */
				{
					if( MARLIN_UpdatePmt( drm_handle->private.marlin.pHMarlin,
										(char *)drm_handle->private.marlin.pmt_section_buf,
										drm_handle->private.marlin.pmt_section_size,
										(int*)&drm_key_info->result.marlin.ecm_pid) != MARLIN_SUCCESS )
					{
						PrintError("%s(%d) ERR Marlin Update PMT Fail\n",__FUNCTION__,__LINE__);
						return DRV_ERR;
					}
				}
				if (drm_handle->private.marlin.ca_section_size)
				{
					if( MARLIN_UpdateEcm(	drm_handle->private.marlin.pHMarlin,
										(char *)drm_handle->private.marlin.ca_section_buf,
										drm_handle->private.marlin.ca_section_size) != MARLIN_SUCCESS )
					{
						PrintError("%s(%d) ERR Marlin Update PMT Fail\n",__FUNCTION__,__LINE__);
						return DRV_ERR;
					}
				}

				/* pump MaxDataRate control */
				{
					NEXUS_PlaybackSettings playbackSettings;
					(void)NEXUS_Playback_GetSettings(drm_handle->private.marlin.pstPlayback, &playbackSettings);
					playbackSettings.playpumpSettings.maxDataRate = drm_handle->private.marlin.max_bitrate*1.5; /* TODO: user define */
					NEXUS_Playback_SetSettings(drm_handle->private.marlin.pstPlayback, &playbackSettings);
				}


			}
			else if(drm_key_info->offer.marlin.cmd == DI_DRM_MARLIN_RELEASE)
			{
				if(drm_handle->private.marlin.videoKeyHandle != NULL && drm_handle->private.marlin.audioKeyHandle != NULL)
				{
					if( P_DRM_ReleaseSecuritySettings(media_session_handle) != DRV_OK)
					{
						PrintError("%s(%d) ERR Marlin Release Security Setting Fail\n",__FUNCTION__,__LINE__);
						return DRV_ERR;
					}
				}

				if( P_DRM_FilterRelease(media_session_handle) != DRV_OK)
				{
					PrintError("%s(%d) ERR Marlin Filter Release Fail\n",__FUNCTION__,__LINE__);
					return DRV_ERR;
				}
			}
			else if(drm_key_info->offer.marlin.cmd == DI_DRM_MARLIN_SETDATA)
			{
				/* Probe Info Update */
				if(drm_key_info->offer.marlin.stream != NULL)
				{
					drm_key_info->result.marlin.ecm_pid = ((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->ca_pid;

					if(((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->pmt_section_size &&
						((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->pmt_section_buf) /* PMT */
					{
						drm_handle->private.marlin.pmt_section_size = ((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->pmt_section_size;
						VK_MEM_Memcpy(	drm_handle->private.marlin.pmt_section_buf,
										(void*)((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->pmt_section_buf,
										((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->pmt_section_size );

					}

					if(((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->ca_section_size &&
						((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->ca_section_buf) /* ECM */
					{
						drm_handle->private.marlin.ca_section_size = ((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->ca_section_size;
						VK_MEM_Memcpy(	drm_handle->private.marlin.ca_section_buf,
										(void*)((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->ca_section_buf,
										((const bmpeg2ts_probe_stream*)drm_key_info->offer.marlin.stream)->ca_section_size );
					}

					drm_handle->private.marlin.max_bitrate = ((bmedia_probe_stream*)drm_key_info->offer.marlin.stream)->max_bitrate;

				}
			}
			else if(drm_key_info->offer.marlin.cmd == DI_DRM_MARLIN_SETSPEED)
			{
				NEXUS_PlaybackSettings playbackSettings;
				/* Speed Check */
				drm_handle->private.marlin.bRewind = (drm_key_info->offer.marlin.speed <= eDI_MEDIA_Speed_RW_X_1_10) ? TRUE : FALSE;

				/* pump MaxDataRate control */
				(void)NEXUS_Playback_GetSettings(drm_handle->private.marlin.pstPlayback, &playbackSettings);
				if( drm_key_info->offer.marlin.speed == eDI_MEDIA_Speed_Normal )
				{
					playbackSettings.playpumpSettings.maxDataRate = drm_handle->private.marlin.max_bitrate*1.5; /* TODO: user define */
				}
				else
				{
					playbackSettings.playpumpSettings.maxDataRate = 108000000;	/*Pump Default from BRCM NEXUS: 108Mbps*/
				}
				NEXUS_Playback_SetSettings(drm_handle->private.marlin.pstPlayback, &playbackSettings);
			}
#endif

			break;
		}
		default :
			PrintError("Invalid DRM Type (%d)\n", drm_handle->drm_info.drm_type);
			return DRV_ERR;
	}

	return DRV_OK;
}

/* 필요한 argument 정의하여 return 값으로 설정 필요 */
DRV_Error DRV_DRM_GetDRMStatus(SESSION_HANDLE media_session_handle /*, return status 에 필요한 struct define 필요 */)
{
	DRM_Session_t *drm_handle;

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	switch(drm_handle->drm_info.drm_type)
	{
		case DI_DRM_TYPE_PLAYREADY:
		{
			/* 필요한 작업 추가 */
			break;
		}
		case DI_DRM_TYPE_VIEWRIGHTWEB:
		{
			/* 필요한 작업 추가 */
			break;
		}
		case DI_DRM_TYPE_MARLIN:
		{
			/* 필요한 작업 추가 */
			break;
		}
		default :
			PrintError("Invalid DRM Type (%d)\n", drm_handle->drm_info.drm_type);
			return DRV_ERR;
	}

	return DRV_OK;

}

DRV_Error DRV_DRM_DestroySession(SESSION_HANDLE media_session_handle)
{
	DRM_Session_t *drm_handle;

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	switch(drm_handle->drm_info.drm_type)
	{
		case DI_DRM_TYPE_WMDRM:
		case DI_DRM_TYPE_PLAYREADY:
		{
#if defined (CONFIG_PLAYREADY)
			DRV_Error dResult;

			dResult = P_DRM_DestroyCrypto(media_session_handle);
			if(dResult != DRV_OK)
			{
				PrintError("P_DRM_DestroyCrypto failed!\n");
				return DRV_ERR;
			}
			if(drm_handle->drm_info.drm.playready.licenseURL)
			{
				VK_MEM_Free(drm_handle->drm_info.drm.playready.licenseURL);
				drm_handle->drm_info.drm.playready.licenseURL = NULL;
			}
#if (NEXUS_VERSION >= 1290)
			VK_SYSTEM_Command("rm -rf /var/lib/humaxtv/drm/license.url");
#endif
#endif
			break;
		}
#if defined(CONFIG_PLAYREADY_25_BRCM)
		case DI_DRM_TYPE_PLAYREADY_25_BRCM:
		{
			if(drm_handle->private.playready.decryptor.pDecrypt)
			{
				DRM_Prdy_Reader_Close(&(drm_handle->private.playready.decryptor));
				drm_handle->private.playready.decryptor.pDecrypt = NULL;
			}
			if(drm_handle->private.playready.drm_context)
			{
				DRM_Prdy_Uninitialize(drm_handle->private.playready.drm_context);
				drm_handle->private.playready.drm_context = NULL;
			}
			if(drm_handle->private.playready.commonCryptoHandle)
			{
				CommonCrypto_Close(drm_handle->private.playready.commonCryptoHandle);
				drm_handle->private.playready.commonCryptoHandle = NULL;
			}

			break;
		}
#endif
		case DI_DRM_TYPE_VIEWRIGHTWEB:
		{
			/* 필요한 작업 추가 */
			break;
		}
		case DI_DRM_TYPE_MARLIN:
		{
#if defined (CONFIG_MARLIN_DRM)
			DRV_Error dResult;

			dResult = P_DRM_ReleaseSecuritySettings(media_session_handle);
			if(dResult != DRV_OK)
			{
				PrintError("P_DRM_DestroyCrypto failed!\n");
				return DRV_ERR;
			}
			MARLIN_Release(drm_handle->private.marlin.pHMarlin);
			MARLIN_Terminate(&drm_handle->private.marlin.pHMarlin);
			break;
#endif
		}
		default :
			PrintError("Invalid DRM Type (%d)\n", drm_handle->drm_info.drm_type);
			return DRV_ERR;
	}

	drm_handle->media_session_handle = NULL;
	drm_handle->used = FALSE;

	return DRV_OK;
}

DRV_Error DRV_DRM_RegisterOutputControlCallback(SESSION_HANDLE media_session_handle, DI_DRM_OutputControlCallback_t OutputControlCallback)
{
	DRM_Session_t *drm_handle=NULL;

	if(OutputControlCallback == NULL)
	{
		PrintError("OutputControlCallback is NULL.!!!\n");
		return DRV_ERR;
	}

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	drm_handle->drm_info.OutputControl = OutputControlCallback;

	return DRV_OK;
}

DRV_Error DRV_DRM_UnregisterOutputControlCallback(SESSION_HANDLE media_session_handle)
{
	DRM_Session_t *drm_handle=NULL;

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	drm_handle->drm_info.OutputControl = NULL;

	return DRV_OK;
}

DRV_Error DRV_DRM_RegisterLicenseDataCallback(SESSION_HANDLE media_session_handle, DI_DRM_LicenseDataCallback_t callback)
{
	DRM_Session_t *drm_handle=NULL;

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	drm_handle->drm_info.cbSetLicenseData = callback;

	return DRV_OK;
}

DRV_Error DRV_DRM_UnregisterLicenseDataCallback(SESSION_HANDLE media_session_handle)
{
	DRM_Session_t *drm_handle=NULL;

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	drm_handle->drm_info.cbSetLicenseData = NULL;

	return DRV_OK;
}

DRV_Error DRV_DRM_SetLicenseData(SESSION_HANDLE media_session_handle, HUINT8* pucLicenseData)
{
	DRM_Session_t *drm_handle=NULL;

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	if(drm_handle->drm_info.pucLicenseData != NULL)
	{
		VK_MEM_Free(drm_handle->drm_info.pucLicenseData);
		drm_handle->drm_info.pucLicenseData = NULL;
	}
	drm_handle->drm_info.pucLicenseData = VK_StrDup(pucLicenseData);

/*************************
*	Set License Data as a file
*************************/
	FILE* fd = NULL;
	fd = fopen("/var/lib/humaxtv/drm/cookie.txt", "w");
	if(fd == NULL)
	{
		PrintError("[%s] fopen failed!\n", __FUNCTION__);
		return DRV_ERR;
	}

	fwrite(pucLicenseData, 1, VK_strlen(pucLicenseData), fd);
	VK_fflush(fd);
	fclose(fd);

	return DRV_OK;
}
DRV_Error DRV_DRM_PushPumpBuffer(SESSION_HANDLE media_session_handle, void *pvDst, void *pvSrc, HUINT32 unSize, HBOOL bflush)
{
	DRM_Session_t *drm_handle=NULL;

	drm_handle = P_DRM_GetInstance(media_session_handle);
    if(drm_handle == NULL)
    {
            PrintError("media session didn't bind drm session.!!!\n");
            return DRV_ERR;
    }
	switch(drm_handle->drm_info.drm_type)
    {
		case DI_DRM_TYPE_PLAYREADY_25_BRCM:
		{
#if defined(CONFIG_PLAYREADY_25_BRCM)
			P_DRM_PlayReady_BRCM_PushPumpBuffer(media_session_handle, pvDst, pvSrc, unSize, bflush);
			break;
#endif
		}
		case DI_DRM_TYPE_NONE:
		case DI_DRM_TYPE_WMDRM:
		case DI_DRM_TYPE_PLAYREADY:
		case DI_DRM_TYPE_VIEWRIGHTWEB:
		case DI_DRM_TYPE_MARLIN:
		default :
			PrintError("Invalid DRM Type (%d)\n", drm_handle->drm_info.drm_type);
			return DRV_ERR;
	}
	return DRV_OK;
}
void* DRV_DRM_DataMemAlloc(HUINT32 unSize)
{
	NEXUS_MemoryAllocationSettings memSettings;
	void* allocBuffer = NULL;

	NEXUS_Memory_GetDefaultAllocationSettings(&memSettings);
#if defined(NEXUS_MEMC0_MAIN_HEAP)
	memSettings.heap = NEXUS_MEMC0_MAIN_HEAP;
#else
	memSettings.heap = 0;
#endif

	if( NEXUS_Memory_Allocate(unSize, &memSettings, (void *)&allocBuffer) != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Memory_Allocate failed (request size :%d)\n",unSize);
		return NULL;
	}
	return allocBuffer;
}
DRV_Error DRV_DRM_DataMemFree(void *pvBuffer)
{
	if(pvBuffer == NULL)
	{
		PrintError("pvBuffer(input) is NULL\n");
		return DRV_ERR;
	}
	NEXUS_Memory_Free(pvBuffer);
	return DRV_OK;
}
#if defined (CONFIG_PLAYREADY_25_BRCM)
/* Enable CONFIG_PLAYREADY_25_BRCM */
DRV_Error DRV_DRM_SecureClock(SESSION_HANDLE media_session_handle)
{
	HUINT32  secClkStatus=0; /* secure clock status */
	DRM_Prdy_Handle_t   drm_context = NULL;
	DRM_Session_t *drm_handle=NULL;

	drm_handle = P_DRM_GetInstance(media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("media session didn't bind drm session.!!!\n");
		return DRV_ERR;
	}

	drm_context = drm_handle->private.playready.drm_context;
	if(drm_handle == NULL)
	{
		PrintError("don't create drm_context !!!\n");
		return DRV_ERR;
	}

	/* Getting the current state of the secure clock*/
	if( DRM_Prdy_SecureClock_GetStatus(drm_context, &secClkStatus) !=  DRM_Prdy_ok)
	{
		PrintError("%d Failed to get the Playready Secure Clock status, quitting....\n",__LINE__);
		return DRV_ERR;
	}

	if( secClkStatus != DRM_PRDY_CLK_SET)
	{
	       /* setup the Playready secure clock */
	       if(P_DRM_initSecureClock(drm_context) != 0)
	       {
	           PrintError("%d Failed to initiize Secure Clock, quitting....\n",__LINE__);
	           return DRV_ERR;
	       }
	}
	return DRV_OK;
}

#define MAX_TIME_CHALLENGE_RESPONSE_LENGTH (1024*64)
#define MAX_URL_LENGTH (4096)
/* Enable CONFIG_PLAYREADY_25_BRCM */
static int P_DRM_initSecureClock( DRM_Prdy_Handle_t drm)
{
    HINT32               rc = 0;
    HUINT8               *timeChResp = NULL;
    HINT8                *timeChURL = NULL;
    HUINT16              *pTimeURL=NULL;
    HUINT8               *timeCh_data=NULL;
    HINT8                timeURL_cstr[256] = {0};
    HUINT32              timeURL_len=0;
    HUINT32              timeCh_len=0;
    HBOOL                redirect = true;
    HINT32               petRC=0;
    HUINT32              petRespCode = 0;
    HUINT32              startOffset;
    HUINT32              length;
    HUINT32              post_ret;
    NEXUS_MemoryAllocationSettings allocSettings;

    if( DRM_Prdy_SecureClock_GenerateChallenge(drm,NULL,&timeURL_len,NULL,&timeCh_len) != DRM_Prdy_buffer_size)
    {
       PrintError("DRM_Prdy_SecureClock_GenerateChallenge Failed, quitting....\n",__LINE__);
       return 0;
    }

    PrintDebug("%d DRM_Prdy_SecureClock_GenerateChallenge succeeded.\n",__LINE__);
    PrintDebug("\tTime Server URL length %d, challenge len %d\n",timeURL_len,timeCh_len);

    rc = NEXUS_Memory_Allocate(timeURL_len*sizeof(wchar_t), NULL, (void **)(&pTimeURL));
    if(rc != NEXUS_SUCCESS)
    {
        PrintError("%d NEXUS_Memory_Allocate failed for the Time server URL buffer, rc = %d\n",__LINE__, rc);
        goto clean_exit;
    }

    rc = NEXUS_Memory_Allocate(timeCh_len+1, NULL, (void **)(&timeCh_data));
    if(rc != NEXUS_SUCCESS)
    {
        PrintError("NEXUS_Memory_Allocate failed for the time challenge buffer, rc = %d\n",__LINE__, rc);
        goto clean_exit;
    }

    BKNI_Memset(pTimeURL, 0, timeURL_len);
    BKNI_Memset(timeCh_data, 0, timeCh_len+1);

    if( DRM_Prdy_SecureClock_GenerateChallenge(drm,(wchar_t*)pTimeURL,&timeURL_len,timeCh_data,&timeCh_len) != DRM_Prdy_ok)
    {
        PrintError("%d DRM_Prdy_SecureClock_GenerateChallenge failed.\n",__LINE__);
        goto clean_exit;
    }

    timeURL_cstr[timeURL_len] = 0; /* null terminator */
    if( !DRM_Prdy_convertWStringToCString((wchar_t*)pTimeURL, timeURL_cstr, timeURL_len))
    {
        PrintError("%d DRM_Prdy_convertWStringToCString failed to convert URL from wchar to char *, can't procceed time challenge.\n",__LINE__);
        goto clean_exit;
    }

    PrintDebug("\tTime challenge petition server URL: %s\n",timeURL_cstr);

    timeCh_data[timeCh_len] = 0;

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    rc = NEXUS_Memory_Allocate(MAX_URL_LENGTH, &allocSettings, (void **)(&timeChURL));
    if(rc != NEXUS_SUCCESS)
    {
        PrintError("NEXUS_Memory_Allocate failed for time challenge response buffer, rc = %d\n",__LINE__, rc);
        goto clean_exit;
    }

    do
    {
           redirect = false;
           /* send the petition request to Microsoft with HTTP GET */
           petRC = DRM_Prdy_http_client_get_petition( timeURL_cstr,
                                                      &petRespCode,
                                                      (char**)&timeChURL);
           if( petRC != 0)
           {
               PrintError("%d Secure Clock Petition request failed, rc = %d\n",__LINE__, petRC);
               rc = petRC;
               goto clean_exit;
           }
           /* we need to check if the Pettion responded with redirection */
           if( petRespCode == 200)
           {
               redirect = false;
           }
           else if( petRespCode == 302 || petRespCode == 301)
           {
               char * isHttps = NULL;

               PrintDebug("\tPetition responded with redirection. The new redirect petition URL: \n\t%s\n",timeChURL);

               redirect = true;

               memset(timeURL_cstr,0,timeURL_len);

               /* check if the URL is "https" */
               isHttps = strstr(timeChURL,"https");
               if( isHttps )
               {
                   strcpy(timeURL_cstr,"http");
                   strcpy(timeURL_cstr+4,isHttps+5);
               }
               else
               {
                   strcpy(timeURL_cstr,timeChURL);
               }

               memset(timeChURL,0,MAX_URL_LENGTH);
               PrintDebug("\tRe-sending petition to URL: %s\n",timeURL_cstr);
           }
           else
           {
               PrintError("%d Secure Clock Petition responded with unsupported result, rc = %d, can't get the tiem challenge URL\n",__LINE__, petRespCode);
               rc = -1;
               goto clean_exit;
           }
    } while (redirect);

    PrintDebug("[PASSED] - %d Petition requests succeeded. The responded time challage server URL:\n\t%s\n",__LINE__,timeChURL);

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    rc = NEXUS_Memory_Allocate(MAX_TIME_CHALLENGE_RESPONSE_LENGTH, &allocSettings, (void **)(&timeChResp ));
    if(rc != NEXUS_SUCCESS)
    {
        PrintError("%d NEXUS_Memory_Allocate failed for time challenge response buffer, rc = %d\n",__LINE__, rc);
        goto clean_exit;
    }

    BKNI_Memset(timeChResp, 0, MAX_TIME_CHALLENGE_RESPONSE_LENGTH);
    post_ret = DRM_Prdy_http_client_time_challenge_post(timeChURL,
                                                        (char *)timeCh_data,
                                                        1,
                                                        150,
                                                        (unsigned char**)&(timeChResp),
                                                        &startOffset,
                                                        &length);
    if( post_ret != 0)
    {
        PrintError("%d Secure Clock Challenge request failed, rc = %d\n",__LINE__, post_ret);
        rc = post_ret;
        goto clean_exit;
    }

    PrintDebug("[PASSED] - %d DRM_Prdy_http_client_time_challenge_post succeeded for Secure Clock Challenge with response size = %d.\n",__LINE__,length);

    if( DRM_Prdy_SecureClock_ProcessResponse( drm, (uint8_t *) timeChResp, length) != DRM_Prdy_ok)
    {
        PrintError("%d Secure Clock Process Challenge response failed\n",__LINE__);
        rc = -1;
        goto clean_exit;
    }

    PrintDebug("%d Initialized Playready Secure Clock success.\n",__LINE__);

    /* NOW testing the system time */

clean_exit:
    if( timeChResp   != NULL)
        NEXUS_Memory_Free(timeChResp );

    if( timeChURL    != NULL)
        NEXUS_Memory_Free(timeChURL  );

    if( pTimeURL  != NULL)
        NEXUS_Memory_Free(pTimeURL );

    if( timeCh_data  != NULL)
        NEXUS_Memory_Free(timeCh_data );

    return rc;
}

DRV_Error DRV_DRM_GetStoredLicense(SESSION_HANDLE media_session_handle)
{
	DRM_Session_t *drm_handle=NULL;
	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		return DRV_ERR;
	}

	if( DRM_Prdy_Reader_Bind(drm_handle->private.playready.drm_context,
                &drm_handle->private.playready.decryptor)!= DRM_Prdy_ok )
	{
		PrintError("%s(%d) Failed to Bind the license, the license may not exist. Exiting...", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
	PrintDebug("%s(%d) DRM_Prdy_Reader_Bind Success\n", __FUNCTION__, __LINE__);

	if( DRM_Prdy_Reader_Commit(drm_handle->private.playready.drm_context) != DRM_Prdy_ok )
	{
		PrintError("%s(%d) Failed to Commit the license after Reader_Bind, exiting...", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}
	PrintDebug("%s(%d) DRM_Prdy_Reader_Commit Success\n", __FUNCTION__, __LINE__);
	return DRV_OK;
}

/* Enable CONFIG_PLAYREADY_25_BRCM */
DRV_Error P_DRM_PlayReady_BRCM_GetLicense(SESSION_HANDLE media_session_handle, HUINT8* pucWrmHeader, HUINT32 uiWrmHeaderSize)
{
	DRV_Error dResult = DRV_OK;
	DRM_Session_t *drm_handle=NULL;
	HINT32 urlLen = 0;
	HINT32 chLen = 0;
	HINT8 *pCh_url = NULL;
	HINT8 *pCh_data = NULL;
	HUINT8 *pResponse = NULL;
	HINT32  respLen;
	HINT32  respOffset;
	HUINT8	tmpSessionIdBuf[16];

	UNUSED_PARAM(pucWrmHeader);
	UNUSED_PARAM(uiWrmHeaderSize);

	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		return DRV_ERR;
	}

	if( DRM_Prdy_Get_Buffer_Size( drm_handle->private.playready.drm_context,
									DRM_Prdy_getBuffer_licenseAcq_challenge,
									NULL, 0, &urlLen, &chLen) != DRM_Prdy_ok )
	{
		PrintError("%s(%d) DRM_Prdy_Get_Buffer_Size() failed, exiting", __FUNCTION__, __LINE__);
		dResult = DRV_ERR;
		goto clean_exit;
	}

	pCh_url = BKNI_Malloc(urlLen);
	if(pCh_url == NULL)
	{
		PrintError("%s(%d) BKNI_Malloc(urlent) failed, exiting...", __FUNCTION__, __LINE__);
		dResult = DRV_ERR;
    	goto clean_exit;
	}

   	pCh_data = BKNI_Malloc(chLen);
	if(pCh_data == NULL)
	{
		PrintError("%s(%d) BKNI_Malloc(chLen) failed, exiting...", __FUNCTION__, __LINE__);
		dResult = DRV_ERR;
		goto clean_exit;
	}

	if(DRM_Prdy_LicenseAcq_GenerateChallenge(drm_handle->private.playready.drm_context,
				NULL, 0, pCh_url, &urlLen, pCh_data, &chLen) != DRM_Prdy_ok )
	{
		PrintError("%s(%d) DRM_Prdy_License_GenerateChallenge() failed, exiting", __FUNCTION__, __LINE__);
		dResult = DRV_ERR;
		goto clean_exit;
	}

	pCh_data[chLen] = 0;
	PrintDebug("%s(%d) Request(Challenge) Data = %s\n", __FUNCTION__, __LINE__, pCh_data);
	PrintDebug("%s(%d) License URL = %s\n", __FUNCTION__, __LINE__, pCh_url);

	pResponse = BKNI_Malloc(MAX_RESPONSE_SIZE);
	if(pResponse == NULL)
	{
		PrintError("%s(%d) BKNI_Malloc(pResponse) failed, exiting...", __FUNCTION__, __LINE__);
		dResult = DRV_ERR;
    	goto clean_exit;
	}

#if 0
	if(DRM_Prdy_http_client_license_post_soap(pCh_url, pCh_data, 1,
	        2000, (unsigned char **)&pResponse, &respOffset, &respLen) != 0)
	{
		PrintError("%s(%d) DRM_Prdy_http_client_license_post_soap() failed, exiting", __FUNCTION__, __LINE__);
		dResult = DRV_ERR;
		goto clean_exit;
	}
#else
	{
		HUINT32 idxCustomHeader = 0;
		DRM_Prdy_http_hdr_indiv customheaders[DI_DRM_MAX_HTTP_CUSTOMHEADER];
		BKNI_Memset(&customheaders,0x0,sizeof(customheaders));

		for(idxCustomHeader = 0 ; idxCustomHeader < drm_handle->drm_info.drm.playready.unHttpCustomHeaders ; ++idxCustomHeader)
		{
			customheaders[idxCustomHeader].hdr_name = drm_handle->drm_info.drm.playready.tHttpCustomHeader[idxCustomHeader].pName;
			customheaders[idxCustomHeader].hdr_name_len = VK_strlen(drm_handle->drm_info.drm.playready.tHttpCustomHeader[idxCustomHeader].pName);
			customheaders[idxCustomHeader].hdr_value = drm_handle->drm_info.drm.playready.tHttpCustomHeader[idxCustomHeader].pValue;
			customheaders[idxCustomHeader].hdr_value_len = VK_strlen(drm_handle->drm_info.drm.playready.tHttpCustomHeader[idxCustomHeader].pValue);
		}

		if(DRM_Prdy_http_client_license_post_soap_ex(pCh_url, pCh_data, 1,
			2000, (unsigned char **)&pResponse, &respOffset, &respLen,
			customheaders, drm_handle->drm_info.drm.playready.unHttpCustomHeaders) != 0)
		{
			PrintError("%s(%d) DRM_Prdy_http_client_license_post_soap() failed, exiting", __FUNCTION__, __LINE__);
			dResult = DRV_ERR;
			goto clean_exit;
		}
	}

#endif
	PrintDebug("%s(%d) Response(license) Data (from Server) = %s\n", __FUNCTION__, __LINE__, &pResponse[respOffset]);

	if( DRM_Prdy_LicenseAcq_ProcessResponseEx(drm_handle->private.playready.drm_context,
				 (char *)&pResponse[respOffset],
				 respLen, tmpSessionIdBuf, NULL) != DRM_Prdy_ok )
	{
		PrintError("%s(%d) DRM_Prdy_LicenseAcq_ProcessResponseEx() failed with SessionID buffer, exiting", __FUNCTION__, __LINE__);
		dResult = DRV_ERR;
		goto clean_exit;
	}
	PrintDebug("%s(%d) DRM_Prdy_LicenseAcq_ProcessResponse Success\n", __FUNCTION__, __LINE__);

	if( DRM_Prdy_Reader_Bind(drm_handle->private.playready.drm_context,
                &drm_handle->private.playready.decryptor)!= DRM_Prdy_ok )
	{
		PrintError("%s(%d) Failed to Bind the license, the license may not exist. Exiting...", __FUNCTION__, __LINE__);
		dResult = DRV_ERR;
		goto clean_exit;
	}
	PrintDebug("%s(%d) DRM_Prdy_Reader_Bind Success\n", __FUNCTION__, __LINE__);

	if( DRM_Prdy_Reader_Commit(drm_handle->private.playready.drm_context) != DRM_Prdy_ok )
	{
		PrintError("%s(%d) Failed to Commit the license after Reader_Bind, exiting...", __FUNCTION__, __LINE__);
		dResult = DRV_ERR;
		goto clean_exit;
	}
	PrintDebug("%s(%d) DRM_Prdy_Reader_Commit Success\n", __FUNCTION__, __LINE__);

clean_exit:
	if(pCh_url != NULL)
	{
		BKNI_Free(pCh_url);
		pCh_url = NULL;
	}
	if(pCh_data != NULL)
	{
		BKNI_Free(pCh_data);
		pCh_data = NULL;
	}
	if(pResponse != NULL)
	{
		BKNI_Free(pResponse);
		pResponse = NULL;
	}

 	return dResult;
}

static DRV_Error P_DRM_PlayReady_BRCM_PushPumpBuffer(SESSION_HANDLE media_session_handle, void *pvDst, void *pvSrc, HUINT32 size, HBOOL bflush)
{
	DRM_Session_t *drm_handle=NULL;
	NEXUS_DmaJobBlockSettings blkSettings;
	CommonCryptoJobSettings cryptoJobSettings;

#if 0
	PrintDebug("%s: from=%p, to=%p, size=%u\n", __FUNCTION__, pvSrc, pvDst, size);
#endif

	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)media_session_handle);
	if(drm_handle == NULL)
    {
		PrintError("There is no matching drm_session!\n");
		return DRV_ERR;
    }
	if(drm_handle->private.playready.commonCryptoHandle == NULL)
	{
		PrintError("There is no commonCryptoHandle!\n");
		return DRV_ERR;
	}

	NEXUS_DmaJob_GetDefaultBlockSettings(&blkSettings);
	blkSettings.pSrcAddr = pvSrc;
	blkSettings.pDestAddr = pvDst;
	blkSettings.blockSize = size;
	blkSettings.resetCrypto = TRUE;
	blkSettings.scatterGatherCryptoStart = TRUE;
	blkSettings.scatterGatherCryptoEnd = TRUE;

	if (bflush)
	{
		/* Need to flush manually the source buffer (non secure heap). We need to flush manually as soon as we copy data into
		the secure heap. Setting blkSettings[ii].cached = true would also try to flush the destination address in the secure heap
		which is not accessible. This would cause the whole memory to be flushed at once. */
		(void)NEXUS_FlushCache(blkSettings.pSrcAddr, blkSettings.blockSize);
		blkSettings.cached = FALSE; /* Prevent the DMA from flushing the buffers later on */
	}

	CommonCrypto_GetDefaultJobSettings(&cryptoJobSettings);
	CommonCrypto_DmaXfer(drm_handle->private.playready.commonCryptoHandle,  &cryptoJobSettings, &blkSettings, 1);

	if (bflush)
	{
		/* Need to flush manually the source buffer (non secure heap). We need to flush manually as soon as we copy data into
		the secure heap. Setting blkSettings[ii].cached = true would also try to flush the destination address in the secure heap
		which is not accessible. This would cause the whole memory to be flushed at once. */
		(void)NEXUS_FlushCache(blkSettings.pSrcAddr, blkSettings.blockSize);
	}

	return DRV_OK;
}

DRV_Error DRV_DRM_DoDecrypt(SESSION_HANDLE media_session_handle, HUINT8 *pBuff, HINT32 nBuffSize, HUINT8 *pIv, HINT32 nIvSize)
{
	DRM_Session_t *drm_handle=NULL;
	DRM_Prdy_AES_CTR_Info_t AesCtrInfo;

	drm_handle = P_DRM_GetInstance((SESSION_HANDLE)media_session_handle);
	if(drm_handle == NULL)
	{
		PrintError("There is no matching drm_session!\n");
		return DRV_ERR;
	}

	if(drm_handle->private.playready.decryptor.pDecrypt == NULL)
	{
		PrintError("Can't have Decrypt Handle of drm_session!\n");
		return DRV_ERR;
	}

	if(nIvSize != 16)
	{
		PrintError("nIvSize = %d, IV size is 16Bytes(128bit)!\n", nIvSize);
		return DRV_ERR;
	}

#if 0
	PrintDebug("pIv  = %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
		pIv[0], pIv[1], pIv[2], pIv[3], pIv[4], pIv[5], pIv[6], pIv[7],
		pIv[8], pIv[9], pIv[10], pIv[11], pIv[12], pIv[13], pIv[14], pIv[15]);
#endif
	AesCtrInfo.qwInitializationVector = (HUINT64)pIv[0] << 56 | (HUINT64)pIv[1] << 48 | (HUINT64)pIv[2] << 40 | (HUINT64)pIv[3] << 32 | (HUINT64)pIv[4] << 24 | (HUINT64)pIv[5] << 16 | (HUINT64)pIv[6] << 8 | (HUINT64)pIv[7];
	AesCtrInfo.qwBlockOffset = (HUINT64)pIv[8] << 56 | (HUINT64)pIv[9] << 48 | (HUINT64)pIv[10] << 40 | (HUINT64)pIv[11] << 32 | (HUINT64)pIv[12] << 24 | (HUINT64)pIv[13] << 16 | (HUINT64)pIv[14] << 8 | (HUINT64)pIv[15];
	AesCtrInfo.bByteOffset = 0;

	if(DRM_Prdy_Reader_Decrypt(&drm_handle->private.playready.decryptor, &AesCtrInfo, (uint8_t *)pBuff, nBuffSize) != DRM_Prdy_ok)
	{
		PrintError("%s Reader_Decrypt failed - %d", __FUNCTION__, __LINE__);
		return DRV_ERR;
	}

	return DRV_OK;
}

#endif /* #if defined (CONFIG_PLAYREADY_25_BRCM) */
