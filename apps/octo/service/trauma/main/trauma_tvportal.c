
/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <trauma_int.h>
#include <hapi.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define TRAUMA_TVPORTAL_HTTP_TIMEOUT					(100)
#define TRAUMA_TVPORTAL_API_LOGIN						"api/Login?deviceID="
#define TRAUMA_TVPORTAL_API_GET_LOGFILTER				"api/GetLogfilter?authticket="
#define TRAUMA_TVPORTAL_API_UPLOAD_STATISTICS			"api/UploadStatistics?authticket="
#define TRAUMA_TVPORTAL_API_ACCEPT_PRIVACY				"api/AcceptPrivacy?authticket="
#define TRAUMA_TVPORTAL_UPLOAD_DATA_PATH				"/tmp/trauma.dat"

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct{
	HUINT32 	ulSemId;
	HCHAR 		*pszAuthTicket;
	HBOOL 		bAcceptPrivacy;
	HBOOL		bLogfilterActive;
	HUINT32		ulInverval,ulTTL;
	HxList_t 	*pDenyList,*pAllowList;
}trauma_tvportal_t;
/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */
static 	HLONG s_ulMsgId  = 0;

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
static trauma_tvportal_t* trauma_tvportal_getContext(void)
{
	static trauma_tvportal_t *s_pcontext = NULL;

	if(s_pcontext == NULL)
	{
		s_pcontext = (trauma_tvportal_t*)HLIB_MEM_Calloc(sizeof(trauma_tvportal_t));
		HxSEMT_Create(&s_pcontext->ulSemId, "trauma_tv_sem", HxSEMT_FIFO);
	}
	HxSEMT_Get(s_pcontext->ulSemId);

	return s_pcontext;
}

static void trauma_tvportal_releaseContext(trauma_tvportal_t *pContext)
{
	if(pContext)
		HxSEMT_Release(pContext->ulSemId);
}

static HINT32 trauma_tvportal_httpCallback(void *ptr, HINT32 size, HINT32 nmemb, void *stream)
{
	HxJSON_t		root;
	HBOOL			bIsOK;
	root = HLIB_JSON_Decode((HCHAR*)ptr);
	if(root==NULL)
	{
		HxLOG_Assert("HLIB_JSON_Decode Failed...");
		return 0;
	}
	bIsOK = HLIB_JSON_Object_GetBoolean(root, "status");
	HxLOG_Debug("status :%d \n",bIsOK);
	HLIB_JSON_Delete(root);
	return (size*nmemb);
}

static HINT32 trauma_tvportal_httpCallback_login(void *ptr, HINT32 size, HINT32 nmemb, void *stream)
{
	TRAUMA_MSG_t 	stMsgData;
	HxJSON_t		root;

	root = HLIB_JSON_Decode((HCHAR*)ptr);
	if(root==NULL)
	{
		HxLOG_Assert("HLIB_JSON_Decode Failed...");
		return 0;
	}

	HxSTD_MemSet(&stMsgData,0x0,sizeof(TRAUMA_MSG_t));
	stMsgData.msg = eTRAUMA_MSG_TVPORTAL_HTTPRESPONSE_LOGIN;
	stMsgData.param1 = (HINT32)root;
	(void)VK_MSG_SendTimeout(s_ulMsgId,&stMsgData,sizeof(TRAUMA_MSG_t),0);

	return (size*nmemb);
}

static HINT32 trauma_tvportal_httpCallback_logfilter(void *ptr, HINT32 size, HINT32 nmemb, void *stream)
{
	TRAUMA_MSG_t 	stMsgData;
	HxJSON_t		root;

	root = HLIB_JSON_Decode((HCHAR*)ptr);
	if(root==NULL)
	{
		HxLOG_Assert("HLIB_JSON_Decode Failed...");
		return 0;
	}
	HxLOG_Debug("LogFilter : %s \n",(HCHAR*)ptr);

	HxSTD_MemSet(&stMsgData,0x0,sizeof(TRAUMA_MSG_t));
	stMsgData.msg = eTRAUMA_MSG_TVPORTAL_HTTPRESPONSE_LOGFILTER;
	stMsgData.param1 = (HINT32)root;
	(void)VK_MSG_SendTimeout(s_ulMsgId,&stMsgData,sizeof(TRAUMA_MSG_t),0);

	return (size*nmemb);
}

static void trauma_tvportal_httpResponse_login(trauma_tvportal_t *pContext,TRAUMA_MSG_t *pstMsgData)
{
	HCHAR 			*authticket;
	HBOOL			pSavedPrivacy =FALSE;
	HxJSON_t		root = (HxJSON_t*)pstMsgData->param1;

	authticket = (HCHAR*)HLIB_JSON_Object_GetString(root, "AuthTicket");
	pContext->bAcceptPrivacy = HLIB_JSON_Object_GetBoolean(root, "AcceptPrivacy");
	if(authticket)
	{
		if(HxSTD_StrEmpty(pContext->pszAuthTicket) == FALSE)
			HLIB_MEM_Free(pContext->pszAuthTicket);
		pContext->pszAuthTicket = HLIB_STD_StrDup(authticket);

		HxLOG_Warning("***************************************\n");
		HxLOG_Warning("Login[OK]:Auth[%s],AcceptPrivacy[%d]\n",HxSTD_StrEmpty(pContext->pszAuthTicket)==TRUE ? "FAIL":"OK",pContext->bAcceptPrivacy);
		HxLOG_Warning("***************************************\n");
		HxLOG_Debug("AuthTicket:%s\n",pContext->pszAuthTicket);
		if(HAPI_GetRegistryInt(TRAUMA_DB_ACCEPT_PRIVACY,&pSavedPrivacy) == ERR_OK)
		{
			if(pSavedPrivacy != pContext->bAcceptPrivacy) // compare saved data and http data
			{
				pContext->bAcceptPrivacy = pSavedPrivacy;
				TRAUMA_TVPORTAL_SetAcceptPrivacy(pSavedPrivacy);
				HxLOG_Warning("Change AcceptPrivacy : %d \n",pSavedPrivacy);
			}
		}
#if (1) //temp......
		else
		{
			HxLOG_Warning("***************************************\n");
			HxLOG_Warning("Force.!!!! Change AcceptPrivacy : true \n");
			HxLOG_Warning("Force.!!!! Change AcceptPrivacy : true \n");
			HxLOG_Warning("***************************************\n");
			TRAUMA_TVPORTAL_SetAcceptPrivacy(TRUE);
		}
#endif

		if(pContext->bAcceptPrivacy == TRUE)
		{
			TRAUMA_TVPORTAL_GetLogFilter();
#ifdef CONFIG_TRAUMA_HCR
			TRAUMA_HCR_Check();
#endif
			TRAUMA_STATISTICS_Start();
		}
	}
	else
	{
		HxLOG_Error("TV portal Login[FAIL]\n");
	}
	HLIB_JSON_Delete(root);
}

static void trauma_tvportal_httpResponse_getlogfilter(trauma_tvportal_t *pContext,TRAUMA_MSG_t *pstMsgData)
{
	HBOOL		bActive;
	HxJSON_t	root = (HxJSON_t*)pstMsgData->param1;
	HxJSON_t	array,item;
	HUINT32 	i,ulCnt,ulInterval,ulTTL;
	HCHAR		*pszEncode;
	TRAUMA_MSG_t stMsg;

	if(pContext->pDenyList)
		HLIB_LIST_RemoveAllFunc(pContext->pDenyList,HLIB_MEM_Free_CB);
	if(pContext->pAllowList)
		HLIB_LIST_RemoveAllFunc(pContext->pAllowList,HLIB_MEM_Free_CB);
	pContext->pAllowList = pContext->pDenyList = NULL;

	bActive = HLIB_JSON_Object_GetBoolean(root, "ACTIVE");
	pContext->bLogfilterActive = bActive;

	ulInterval = HLIB_JSON_Object_GetInteger(root, "INTERVAL");
	pContext->ulInverval = ulInterval;

	ulTTL = HLIB_JSON_Object_GetInteger(root, "TTL");
	pContext->ulTTL= ulTTL;

	if(bActive == TRUE)
	{
		array = HLIB_JSON_Object_GetArray(root,"DENY");
		if (array)
		{
			ulCnt = HLIB_JSON_GetSize(array);
			for (i = 0; i < ulCnt; i++)
			{
				item = HxJSON_Array_Get(array, i);
				pszEncode = HLIB_JSON_Encode(item,NULL,0);
				pContext->pDenyList = HLIB_LIST_Append(pContext->pDenyList,(void*)pszEncode);
			}
		}
	}
	else
	{
		array = HLIB_JSON_Object_GetArray(root,"ALLOW");
		if (array)
		{
			ulCnt = HLIB_JSON_GetSize(array);
			for (i = 0; i < ulCnt; i++)
			{
				item = HxJSON_Array_Get(array, i);
				pszEncode = HLIB_JSON_Encode(item,NULL,0);
				pContext->pAllowList = HLIB_LIST_Append(pContext->pAllowList,(void*)pszEncode);
			}
		}
	}

	HLIB_JSON_Delete(root);


	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	stMsg.msg =eTRAUMA_MSG_STATISTICS_CHECK_LOGFILTER;
	TRAUMA_MSG_Send(&stMsg);
}


static HERROR trauma_tvportal_httpGet(const HINT8 * szUrl, HxHttpGet_Callback *callback, HUINT32 ulTimeout)
{
	HERROR							hError = ERR_FAIL;
	HxTRANSPORT_Http_t				*pHttp = NULL;
	HxTRANSPORT_Httpheader_t		*pHttpHeader = NULL;
	HxTRANSPORT_HttpResponse_e		resCode;
	HFLOAT64						ulRealSize = 0;

	pHttp = HLIB_TRANSPORT_HttpOpen();
	if(pHttp == NULL)
		goto ret_err;

	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Content-Type: text/xml");
	if(pHttpHeader == NULL)
		goto ret_err;

	if(HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader) != ERR_OK)
		goto ret_err;

    if(ulTimeout != 0)
    {
        if(HLIB_TRANSPORT_HttpSetTimeout(pHttp, ulTimeout) != ERR_OK)
			goto ret_err;
    }
	if(HLIB_TRANSPORT_HttpGetWithCallback(pHttp, szUrl, callback, &ulRealSize) != ERR_OK)
		goto ret_err;

	hError = ERR_OK;

ret_err :
	if ( hError != ERR_OK )
	{
		HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &resCode);
		HxLOG_Warning("Http-Get Error[%d]... %s \n",resCode,szUrl);
	}

	if(pHttpHeader != NULL)
		HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
	if(pHttp != NULL)
		HLIB_TRANSPORT_HttpClose(pHttp);

	return hError;
}

static void trauma_tvportal_httpGet_login(trauma_tvportal_t *pContext)
{
	HINT32		SysOffsetHour = 0, SysOffsetMin = 0, SysTimeZone = 0;
	HCHAR		pUrl[TRAUMA_TVPORTAL_URL_MAX_LEN]={0,};
	HCHAR		*pucMaccAddr = NULL,*pszSystemId = NULL,*pszSystemVersion = NULL;

	if(HLIB_DATETIME_GetOffsetTime (&SysOffsetHour, &SysOffsetMin) != ERR_OK)
	{
		HxLOG_Error("HLIB_DATETIME_GetOffsetTime() failed\n");
		goto ret_err;
	}

	pucMaccAddr = TRAUMA_GetLanMac();
	SysTimeZone = (SysOffsetHour*60) + SysOffsetMin;

	if(TRAUMA_GetSystemInfo(&pszSystemId,&pszSystemVersion) == ERR_OK)
	{
		HxSTD_snprintf(pUrl, TRAUMA_TVPORTAL_URL_MAX_LEN,
			"%s/%s%s&MACAddress=%s&SystemID=%s&SWVer=%s&serialNumber=&timeZone=%d",
			CONFIG_APPPORTAL_SERVER_URL, TRAUMA_TVPORTAL_API_LOGIN,
			pucMaccAddr, pucMaccAddr,
			pszSystemId,pszSystemVersion,
			SysTimeZone
			);
		HxLOG_Debug(HxANSI_COLOR_GREEN("URL = [%s]\n"), pUrl);
		trauma_tvportal_httpGet(pUrl,trauma_tvportal_httpCallback_login,TRAUMA_TVPORTAL_HTTP_TIMEOUT);
	}

ret_err:

	return;
}

static void trauma_tvportal_httpGet_getlogfilter(trauma_tvportal_t *pContext)
{
	HCHAR	pUrl[TRAUMA_TVPORTAL_URL_MAX_LEN]={0,};

	HxSTD_snprintf(pUrl, TRAUMA_TVPORTAL_URL_MAX_LEN,
		"%s/%s%s",CONFIG_APPPORTAL_SERVER_URL,TRAUMA_TVPORTAL_API_GET_LOGFILTER,pContext->pszAuthTicket);
	HxLOG_Debug(HxANSI_COLOR_GREEN("URL = [%s]\n"),pUrl);

	trauma_tvportal_httpGet(pUrl,trauma_tvportal_httpCallback_logfilter,TRAUMA_TVPORTAL_HTTP_TIMEOUT);
}

static void trauma_tvportal_httpGet_acceptprivacy(trauma_tvportal_t *pContext,TRAUMA_MSG_t *pstMsgData)
{
	HCHAR						pUrl[TRAUMA_TVPORTAL_URL_MAX_LEN]={0,};
	HBOOL						bAccept;

	bAccept = (HBOOL)pstMsgData->param1;

	if(HxSTD_StrEmpty(pContext->pszAuthTicket) == FALSE)
	{
		HxSTD_snprintf(pUrl, TRAUMA_TVPORTAL_URL_MAX_LEN,
			"%s/%s%s&agree=%s",CONFIG_APPPORTAL_SERVER_URL,TRAUMA_TVPORTAL_API_ACCEPT_PRIVACY,pContext->pszAuthTicket,bAccept ? "Y":"N");
		HxLOG_Debug(HxANSI_COLOR_GREEN("URL = [%s]\n"),pUrl);

		trauma_tvportal_httpGet(pUrl,trauma_tvportal_httpCallback,TRAUMA_TVPORTAL_HTTP_TIMEOUT);
	}
}

static HERROR trauma_tvportal_httpPut(HCHAR *pUpLoadUrl,HCHAR *pszData)
{
	HERROR							hError = ERR_FAIL;
	HxTRANSPORT_Http_t				*pHttp = NULL;
	HxTRANSPORT_Httpheader_t		*pHttpHeader = NULL;
	HxTRANSPORT_HttpResponse_e		resCode;
	HFLOAT64 						uiUploadSize = 0;
	HxFILE_t 						*pFile;

	if(HLIB_FILE_Exist(TRAUMA_TVPORTAL_UPLOAD_DATA_PATH)==TRUE)
		(void)HLIB_FILE_Delete(TRAUMA_TVPORTAL_UPLOAD_DATA_PATH);
	pFile = HLIB_FILE_Open(TRAUMA_TVPORTAL_UPLOAD_DATA_PATH,"wb");
	if(pFile == NULL)
	{
		goto ret_err;
	}
	HLIB_FILE_Write(pFile,pszData, 1,HxSTD_StrLen(pszData));// make post data...
	HLIB_FILE_Close(pFile);

	pHttp = HLIB_TRANSPORT_HttpOpen();
	if(pHttp == NULL)
		goto ret_err;

	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Content-Type: text/xml");
	if(pHttpHeader == NULL)
		goto ret_err;

	if(HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader) != ERR_OK)
		goto ret_err;

	if(HLIB_TRANSPORT_HttpPut(pHttp, pUpLoadUrl, TRAUMA_TVPORTAL_UPLOAD_DATA_PATH, &uiUploadSize) == ERR_OK)
	{
		HxLOG_Debug("HTTP-SEND SUCCESS (size:%d)\n", (HUINT32)uiUploadSize);
	}
	else
	{
		goto ret_err;
	}

	hError = ERR_OK;

ret_err :
	if ( hError != ERR_OK )
	{
		HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &resCode);
		HxLOG_Warning("Http-Get Error[%d]... %s \n",resCode,pUpLoadUrl);
	}

	if(pHttpHeader != NULL)
		HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
	if(pHttp != NULL)
		HLIB_TRANSPORT_HttpClose(pHttp);

	return hError;

}

static void trauma_tvportal_httpPut_hcr(trauma_tvportal_t *pContext,TRAUMA_MSG_t *pstMsgData)
{
	HCHAR 		*pszpUrl;
	HCHAR 		*pszData;

	pszpUrl = (HCHAR*)pstMsgData->param1;
	pszData = (HCHAR*)pstMsgData->param2;

	HxLOG_Debug(HxANSI_COLOR_GREEN("URL = [%s]\n"),pszpUrl);
	trauma_tvportal_httpPut(pszpUrl,pszData);

	HLIB_MEM_Free(pszpUrl);
	HLIB_MEM_Free(pszData);
}

static void trauma_tvportal_httpPut_statisticsupload(trauma_tvportal_t *pContext,TRAUMA_MSG_t *pstMsgData)
{
	HCHAR	pUrl[TRAUMA_TVPORTAL_URL_MAX_LEN]={0,};
	HCHAR	*pszData = (HCHAR*)pstMsgData->param1;


	HxSTD_snprintf(pUrl, TRAUMA_TVPORTAL_URL_MAX_LEN,
		"%s/%s%s",CONFIG_APPPORTAL_SERVER_URL,TRAUMA_TVPORTAL_API_UPLOAD_STATISTICS,pContext->pszAuthTicket);
	HxLOG_Debug(HxANSI_COLOR_GREEN("URL = [%s]\n") HxANSI_COLOR_PURPLE("POST = [%s] \n"), pUrl,pszData);

	trauma_tvportal_httpPut(pUrl,pszData);
	HLIB_MEM_Free(pszData);
}

static void trauma_tvportal_logout(trauma_tvportal_t *pContext)
{
	if(pContext->pszAuthTicket)
		HLIB_MEM_Free(pContext->pszAuthTicket);
	pContext->bAcceptPrivacy = FALSE;
}

static void trauma_tvportal_process(TRAUMA_MSG_t *pstMsgData)
{
	trauma_tvportal_t *pContext;

	pContext = trauma_tvportal_getContext();

	switch(pstMsgData->msg)
	{
	case eTRAUMA_MSG_TVPORTAL_HTTPGET_LOGIN:
		trauma_tvportal_httpGet_login(pContext);
		break;
	case eTRAUMA_MSG_TVPORTAL_HTTPRESPONSE_LOGIN:
		trauma_tvportal_httpResponse_login(pContext,pstMsgData);
		break;
	case eTRAUMA_MSG_TVPORTAL_LOGOUT:
		trauma_tvportal_logout(pContext);
		break;
	case eTRAUMA_MSG_TVPORTAL_HTTPPUT_STATISTICS_UPLOAD:
		trauma_tvportal_httpPut_statisticsupload(pContext,pstMsgData);
		break;
	case eTRAUMA_MSG_TVPORTAL_HTTPGET_ACCEPT_PRIVACY:
		trauma_tvportal_httpGet_acceptprivacy(pContext,pstMsgData);
		break;
	case eTRAUMA_MSG_TVPORTAL_HTTPPUT_HCR:
		trauma_tvportal_httpPut_hcr(pContext,pstMsgData);
		break;
	case eTRAUMA_MSG_TVPORTAL_HTTPGET_GETLOGFILTER:
		trauma_tvportal_httpGet_getlogfilter(pContext);
		break;
	case eTRAUMA_MSG_TVPORTAL_HTTPRESPONSE_LOGFILTER:
		trauma_tvportal_httpResponse_getlogfilter(pContext,pstMsgData);
		break;
	default:
		HxLOG_Error("Wrong message type...[0x%x]\n",pstMsgData->msg);
		break;
	}
	trauma_tvportal_releaseContext(pContext);
}

static void	trauma_tvportal_task (void *arg)
{
	TRAUMA_MSG_t stMsgData;

	while(1)
	{
		if(APK_SYSINFO_STB_Initialized() == ERR_OK)
		{
			HxSTD_MemSet(&stMsgData,0x0,sizeof(TRAUMA_MSG_t));
			if(VK_MSG_ReceiveTimeout( s_ulMsgId, &stMsgData, sizeof(TRAUMA_MSG_t),TRAUMA_TASK_SLEEP) == VK_OK)
			{
				trauma_tvportal_process(&stMsgData);
			}
		}
		else
		{
			HxLOG_Debug("APK_SYSINFO_STB_Initialized == FAIL \n");
			VK_TASK_Sleep(TRAUMA_TASK_SLEEP);
		}
	}
}

/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
void TRAUMA_TVPORTAL_Init(void)
{
	HULONG	ulTaskId=0;

	HLIB_TRANSPORT_HttpInit();

	(void)VK_TASK_Create(trauma_tvportal_task, VK_TASK_PRIORITY_MW_REF, SIZE_256K, "trauma_tvportal", NULL, &ulTaskId, 0);
	(void)VK_MSG_Create(512, sizeof(TRAUMA_MSG_t), "trauma_tvportal_msg", &s_ulMsgId, VK_SUSPENDTYPE_FIFO );
	(void)VK_TASK_Start(ulTaskId);
}

void TRAUMA_TVPORTAL_Login(void)
{
	TRAUMA_MSG_t stMsgData;

	HxSTD_MemSet(&stMsgData,0x0,sizeof(TRAUMA_MSG_t));
	stMsgData.msg = eTRAUMA_MSG_TVPORTAL_HTTPGET_LOGIN;

	(void)VK_MSG_SendTimeout(s_ulMsgId,&stMsgData,sizeof(TRAUMA_MSG_t),0);
}

void TRAUMA_TVPORTAL_LogOut(void)
{
	TRAUMA_MSG_t stMsgData;

	HxSTD_MemSet(&stMsgData,0x0,sizeof(TRAUMA_MSG_t));
	stMsgData.msg = eTRAUMA_MSG_TVPORTAL_LOGOUT;

	(void)VK_MSG_SendTimeout(s_ulMsgId,&stMsgData,sizeof(TRAUMA_MSG_t),0);
}

void TRAUMA_TVPORTAL_GetLogFilter(void)
{
	TRAUMA_MSG_t stMsgData;

	HxSTD_MemSet(&stMsgData,0x0,sizeof(TRAUMA_MSG_t));
	stMsgData.msg = eTRAUMA_MSG_TVPORTAL_HTTPGET_GETLOGFILTER;

	(void)VK_MSG_SendTimeout(s_ulMsgId,&stMsgData,sizeof(TRAUMA_MSG_t),0);
}

HCHAR* TRAUMA_TVPORTAL_GetAuthTicket(void)
{
	trauma_tvportal_t 	*pContext;
	HCHAR				*pAuthTicket;

	pContext = trauma_tvportal_getContext();
	pAuthTicket = HLIB_STD_StrDup(pContext->pszAuthTicket);
	trauma_tvportal_releaseContext(pContext);

	return pAuthTicket;
}

HBOOL TRAUMA_TVPORTAL_GetAcceptPrivacy(void)
{
	trauma_tvportal_t 	*pContext;
	HBOOL				bPrivacy;

	pContext = trauma_tvportal_getContext();
	bPrivacy = pContext->bAcceptPrivacy;
	trauma_tvportal_releaseContext(pContext);

	return bPrivacy;
}

HBOOL TRAUMA_TVPORTAL_GetLogFilterActiveStatus(void)
{
	trauma_tvportal_t 	*pContext;
	HBOOL				bActive;

	pContext = trauma_tvportal_getContext();
	bActive = pContext->bLogfilterActive;
	trauma_tvportal_releaseContext(pContext);

	return bActive;
}

HUINT32 TRAUMA_TVPORTAL_GetLogFilterInterval(void)
{
	trauma_tvportal_t 	*pContext;
	HUINT32				ulInverval;

	pContext = trauma_tvportal_getContext();
	ulInverval = pContext->ulInverval;
	trauma_tvportal_releaseContext(pContext);

	return ulInverval;
}

HUINT32 TRAUMA_TVPORTAL_GetLogFilterTTL(void)
{
	trauma_tvportal_t 	*pContext;
	HUINT32				ulTTL;

	pContext = trauma_tvportal_getContext();
	ulTTL = pContext->ulTTL;
	trauma_tvportal_releaseContext(pContext);

	return ulTTL;
}

HxList_t* TRAUMA_TVPORTAL_GetLogFilterAllowList(void)
{
	trauma_tvportal_t 	*pContext;
	HxList_t			*pList, *pRetList= NULL;

	pContext = trauma_tvportal_getContext();
	pList = pContext->pAllowList;
	while (pList)
	{
		HCHAR *pData = (HCHAR*)HLIB_LIST_Data(pList);

		if(pData)
		{
			pRetList = HLIB_LIST_Append(pRetList,HLIB_STD_StrDup(pData));
		}
		pList = HLIB_LIST_Next(pList);
	}

	trauma_tvportal_releaseContext(pContext);

	return pRetList;
}

HxList_t* TRAUMA_TVPORTAL_GetLogFilterDenyList(void)
{
	trauma_tvportal_t 	*pContext;
	HxList_t			*pList, *pRetList= NULL;

	pContext = trauma_tvportal_getContext();
	pList = pContext->pDenyList;
	while (pList)
	{
		HCHAR *pData = (HCHAR*)HLIB_LIST_Data(pList);

		if(pData)
		{
			pRetList = HLIB_LIST_Append(pRetList,HLIB_STD_StrDup(pData));
		}
		pList = HLIB_LIST_Next(pList);
	}

	trauma_tvportal_releaseContext(pContext);

	return pRetList;
}


void TRAUMA_TVPORTAL_SetAcceptPrivacy(HBOOL bAccept)
{
	TRAUMA_MSG_t stMsgData;

	HxSTD_MemSet(&stMsgData,0x0,sizeof(TRAUMA_MSG_t));
	stMsgData.msg = eTRAUMA_MSG_TVPORTAL_HTTPGET_ACCEPT_PRIVACY;
	stMsgData.param1 = (HINT32)bAccept;

	HAPI_SetRegistryInt(TRAUMA_DB_ACCEPT_PRIVACY,bAccept);
	(void)VK_MSG_SendTimeout(s_ulMsgId,&stMsgData,sizeof(TRAUMA_MSG_t),0);

	TRAUMA_TVPORTAL_Login(); // and try login
}

void TRAUMA_TVPORTAL_StatisticsUpload(HCHAR *pszData)
{
	TRAUMA_MSG_t stMsgData;

	if(HxSTD_StrEmpty(pszData) == TRUE)
	{
		HxLOG_Debug("Empty data...!\n");
		return;
	}
	HxSTD_MemSet(&stMsgData,0x0,sizeof(TRAUMA_MSG_t));
	stMsgData.msg = eTRAUMA_MSG_TVPORTAL_HTTPPUT_STATISTICS_UPLOAD;
	stMsgData.param1 = (HINT32)pszData;

	(void)VK_MSG_SendTimeout(s_ulMsgId,&stMsgData,sizeof(TRAUMA_MSG_t),0);
}

void TRAUMA_TVPORTAL_MSG_Send(TRAUMA_MSG_t *pstMsgData)
{
	(void)VK_MSG_SendTimeout(s_ulMsgId,pstMsgData,sizeof(TRAUMA_MSG_t),0);
}



