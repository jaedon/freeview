/**
@file     file_name.c File Name을 적습니다.
@brief    file_name & simple comment.

Description: File에 대한 설명을 적습니다.		\n
Module: Layer와 모듈 명을 적습니다.			 	\n
Remarks : 										\n

Copyright (c) 2008 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <hlib.h>
#include <vkernel.h>
#include "../include/ipepg.h"
#include "ipepg_manager.h"
#include "ipepg_downloader.h"
#include "ipepg_svclistupload.h"
#include "ipepg_auth.h"

#if defined(CFG_OPEN_DEBUG)
#undef HxLOG_Trace
#define HxLOG_Trace()	printf("[IPEPG] %s %d\n", __FUNCTION__, __LINE__);
#undef HxLOG_Debug
#define HxLOG_Debug		printf
#undef HxLOG_Print
#define HxLOG_Print		printf
#endif

#ifdef IPEPG_FUNCTION_COLOR_TRACE
	#define IPEPG_FUNC_START()	HxLOG_Debug( HxANSI_COLOR_CYAN("[%s][%d] +++ \n") , __FUNCTION__, __LINE__);
	#define IPEPG_FUNC_END()	HxLOG_Debug(  HxANSI_COLOR_CYAN("[%s][%d] --- \n") , __FUNCTION__, __LINE__);
#else
	#define IPEPG_FUNC_START()	HxLOG_Debug("[%s][%d] +++ \n" , __FUNCTION__, __LINE__);
	#define IPEPG_FUNC_END()	HxLOG_Debug("[%s][%d] --- \n" , __FUNCTION__, __LINE__);
#endif



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************     Macro         *************************/
/******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define IPEPG_TRANSPORT_MAX_BUF_SIZE		(1024*350)
#define IPEPG_HUMAX_XML_PID_KIND 			(2)

#define XML_WRITE_START_WITH_ATTR(des, len ,tag, attrtag, data) 	HxSTD_PrintToStrN(des, len, "<%s %s=\"%d\">", tag,attrtag, data)
#define XML_WRITE_START(des, len ,tag) 								HxSTD_PrintToStrN(des, len, "<%s>", tag)
#define XML_WRITE_END(des,len,tag) 									HxSTD_PrintToStrN(des, len, "</%s>", tag)
#define XML_WRITE_DECIMAL(des,len,tag,data) 						HxSTD_PrintToStrN(des, len, "<%s>%d</%s>", tag, data, tag)
#define XML_WRITE_STRING(des,len,tag,data) 							HxSTD_PrintToStrN(des, len, "<%s>%s</%s>", tag, data, tag)
typedef struct
{
	HUINT8		ucOrgStrLen;
	HUINT8		ucFlagReplace;
	HUINT8		szOrgStr[8];
	HUINT8		ucReplaceCode;
} EPG_REPLACESTR_t;

static EPG_REPLACESTR_t		s_astReplaceXmlString[] =
{
	{	5,		FALSE,	{'&', 'a', 'm', 'p', ';', '\0', 0, },		0x26	},
	{	6,		TRUE,	{'&', 'q', 'u', 'o', 't', ';', '\0', 0, },	0x22	},
	{	4,		TRUE,	{'&', 'g', 't', ';', '\0', 0, },			0x3C	},
	{	4,		TRUE,	{'&', 'l', 't', ';', '\0', 0, },			0x3E	},


	// system specification 2.4.2
	// 5.3 programme data
	{	2, 		TRUE, 	{'\\', 'n', '\0', 0, }, 					0x0A	},
	{	2, 		TRUE, 	{'\\', 't', '\0', 0, }, 					0x09	},
	{	1, 		TRUE, 	{'\\', '\0', 0, },		 					0x5C	},
	{	0, 		FALSE, 	{0, }, 										0x00	}
};


static HERROR		ipepg_SetReplaceString(HUINT8 *pucSrcPtr, HUINT8 *pucDesPtr, HUINT32 *pucSaveSize, HUINT32 ulDeslen)
{
	HUINT32					ulSrcLen = 0;
	HUINT32					ulDesPos = 0;
	HUINT32					ulSrcPos = 0;
	HUINT32					ulReplaceCharPos = 0;
	HUINT8					pucTempPtr[1024] = {0,};

	IPEPG_RETURN_IF(pucSrcPtr == NULL || pucDesPtr == NULL || ulDeslen == 0, ERR_FAIL);
	ulSrcLen = HxSTD_StrLen(pucSrcPtr);

	IPEPG_RETURN_IF(ulSrcLen >= ulDeslen, ERR_FAIL);

	for( ulSrcPos = 0, ulDesPos = 0 ; ulSrcPos <  ulSrcLen ; ulSrcPos++)
	{
		HUINT32 ulReplaceCharIdx = 0;
		for(ulReplaceCharIdx = 0 ; ulReplaceCharIdx < 7 ; ulReplaceCharIdx++)
		{
			if(pucSrcPtr[ulSrcPos] == s_astReplaceXmlString[ulReplaceCharIdx].ucReplaceCode)
			{
				break;
			}
		}

		if(ulReplaceCharIdx == 7)
		{
			pucTempPtr[ulDesPos++] = pucSrcPtr[ulSrcPos];
			continue;
		}


		for(ulReplaceCharPos = 0; ulReplaceCharPos < s_astReplaceXmlString[ulReplaceCharIdx].ucOrgStrLen ; ulReplaceCharPos++)
		{
			pucTempPtr[ulDesPos++] = s_astReplaceXmlString[ulReplaceCharIdx].szOrgStr[ulReplaceCharPos];
		}
	}

	ulSrcLen = HxSTD_StrLen(pucTempPtr);

	for( ulSrcPos = 0, ulDesPos = 0 ; ulSrcPos <  ulSrcLen ; ulSrcPos++)
	{

		if(pucTempPtr[ulSrcPos] == 0xee || pucTempPtr[ulSrcPos] == 0x82 || pucTempPtr[ulSrcPos] == 0x86 || pucTempPtr[ulSrcPos] == 0x87)
		{
			continue;
		}

		// 근본원인은 record info의 aucDisplayEvtName 저장시 이미 UTF8 변환중 문제가 생김
		// 여기서는 일단 문제 character는 제거하는 것으로 처리
		if(pucTempPtr[ulSrcPos] < 0x20 || pucTempPtr[ulSrcPos] >= 0xE0)
		{
			continue;
		}

		pucDesPtr[ulDesPos++] = pucTempPtr[ulSrcPos];
	}

	return ERR_OK;
}


static HERROR ipepg_SendServiceListToServer(IPEPG_DATAMGR_t * dataMgr)
{
	HERROR						hErr = ERR_FAIL;
	HINT8						ucRpUploadUrl[IPEPG_URL_MAX_LEN] = {0,};
	HUINT8 						* pucMacAddress = NULL;
	HUINT8 						* pServerURL = NULL;
	HFLOAT64 					uiUploadSize = 0;
	HxTRANSPORT_Http_t			*pHttp = NULL;
	HxTRANSPORT_Httpheader_t	*pHttpHeader = NULL;
	IPEPG_FUNC_START();

	pServerURL = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_SERVERURL);
	pucMacAddress = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);

	if(pServerURL == NULL || pucMacAddress == NULL)
	{
		IPEPG_MemFree(pServerURL);
		IPEPG_MemFree(pucMacAddress);
		IPEPG_FUNC_END();
		return ERR_FAIL;
	}

	/* get rpserver_address */
	HxSTD_snprintf(ucRpUploadUrl, IPEPG_URL_MAX_LEN, "%s/%s%s", pServerURL, EPG_SET_TRIPLEID_URL, pucMacAddress);
	IPEPG_MemFree(pServerURL);
	IPEPG_MemFree(pucMacAddress);

	HxLOG_Debug("SVCLIST URL : %s\n", ucRpUploadUrl);

	pHttp = HLIB_TRANSPORT_HttpOpen();

	if(pHttp != NULL)
	{
#if defined(TVPORTAL_ROOT_CA) && defined(TVPORTAL_CLIENT_CA_KEY)
		if(!strncmp(ucRpUploadUrl, HTTPS_PREFIX, HTTPS_PREFIX_URL_LENGTH))
		{
			hErr = HLIB_TRANSPORT_HttpSetCACertificateFile(pHttp, TVPORTAL_ROOT_CA);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("[RP_Network_Upload_Process.1] ##ERROR: Error code = %x\n", HxTransport_GetLastError());
				HLIB_TRANSPORT_HttpClose(pHttp);
				return hErr;
			}
			hErr = HLIB_TRANSPORT_HttpSetCertificateKeyFile(pHttp, TVPORTAL_CLIENT_CA_KEY, eHxTP_HTTP_CERTIFICATE_P12);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("[RP_Network_Upload_Process.2] ##ERROR: Error code = %x\n", HxTransport_GetLastError());
				HLIB_TRANSPORT_HttpClose(pHttp);
				return hErr;
			}
			hErr = HLIB_TRANSPORT_HttpSetPeerVeryfication(pHttp, TRUE);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("[RP_Network_Upload_Process.3] ##ERROR: Error code = %x\n", HxTransport_GetLastError());
				HLIB_TRANSPORT_HttpClose(pHttp);
				return hErr;
			}
			hErr = HLIB_TRANSPORT_HttpSetHostVeryfication(pHttp, FALSE);
			if(hErr != ERR_OK)
			{
				HxLOG_Debug("[RP_Network_Upload_Process.4] ##ERROR: Error code = %x\n", HxTransport_GetLastError());
				HLIB_TRANSPORT_HttpClose(pHttp);
				return hErr;
			}
		}
#endif
		pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Content-Type: text/xml");
		if(pHttpHeader != NULL)
		{
			if(HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader) == ERR_OK)
			{
				hErr = HLIB_TRANSPORT_HttpPut(pHttp, ucRpUploadUrl, EPG_TRIPLEID_FILE, &uiUploadSize);
				if ( hErr != ERR_OK )
				{
#if defined(CONFIG_DEBUG)
					HxTRANSPORT_HttpResponse_e	resCode = 0;
					HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &resCode);
#endif
				}
				else
				{
					HxLOG_Debug("SEND SUCCESS[%d]\n", (HUINT32)uiUploadSize);
				}
				HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
			}
		}

		HLIB_TRANSPORT_HttpClose(pHttp);
	}

	IPEPG_FUNC_END();
	return ERR_OK;
}

static	HERROR		ipepg_MakeServicelist(IPEPG_DATAMGR_t * pDataMgr)
{
	HxList_t	*phSvcList = NULL, *phTempSvcList = NULL;
	HxFILE_t	pFile = NULL;
	HINT8		pStrBuf[XML_TAG_MAX_LEN]="";
	HxHASH_t		*phSvcHash = NULL;
	HUINT32		ulCountOfService = 0;
	IPEPG_FUNC_START();

	phSvcHash = IPEPG_GetItemPoint(eIPEPG_ITEM_SVCLIST);
	phSvcList = HLIB_HASH_GetValues (phSvcHash);

	if(phSvcList == NULL)
	{
		HLIB_HASH_Destroy(phSvcHash);
		HxLOG_Debug("ipepg_MakeServicelist is Fail. Svc list is NULL\n");
		IPEPG_FUNC_END();
		return ERR_FAIL;
	}
	phTempSvcList = phSvcList;

	HLIB_FILE_Delete(EPG_TRIPLEID_FILE);
	pFile = HLIB_FILE_Open(EPG_TRIPLEID_FILE,"wb");

	HxSTD_MemSet(pStrBuf, 0, XML_TAG_MAX_LEN);
	XML_WRITE_START_WITH_ATTR(pStrBuf, XML_TAG_MAX_LEN, "tripleid", "kind", IPEPG_HUMAX_XML_PID_KIND);
	if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));



	while(phSvcList != NULL)
	{
		IPEPG_SvcInfo_t *pstSvcInfo = NULL;
		HUINT8 pucCHName[DxNAME_LEN * 2]="";

		pstSvcInfo = (IPEPG_SvcInfo_t *)HLIB_LIST_Data(phSvcList);

		if(pstSvcInfo == NULL)
		{
			phSvcList = phSvcList->next;
			continue;
		}

		//HxLOG_Debug("onid [%04x] tsid[%x] svcid[%x] lcn[%x] name[%s]\n", pstSvcInfo->stTripleId.usOnId, pstSvcInfo->stTripleId.usTsId, pstSvcInfo->stTripleId.usSvcId, pstSvcInfo->ulLcn, pstSvcInfo->ucName);
		XML_WRITE_START(pStrBuf, XML_TAG_MAX_LEN, "row");
		if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));

		XML_WRITE_DECIMAL(pStrBuf, XML_TAG_MAX_LEN, "onid",pstSvcInfo->stTripleId.usOnId);
		if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));

		XML_WRITE_DECIMAL(pStrBuf, XML_TAG_MAX_LEN, "tsid",pstSvcInfo->stTripleId.usTsId);
		if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));

		XML_WRITE_DECIMAL(pStrBuf, XML_TAG_MAX_LEN, "svcid",pstSvcInfo->stTripleId.usSvcId);
		if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));

		XML_WRITE_DECIMAL(pStrBuf, XML_TAG_MAX_LEN, "hsvc",pstSvcInfo->ulUid);
		if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));

		ipepg_SetReplaceString(pstSvcInfo->ucName, pucCHName, NULL, DxNAME_LEN * 2);
		XML_WRITE_STRING(pStrBuf, XML_TAG_MAX_LEN, "CHName",pucCHName);
		if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));

		XML_WRITE_DECIMAL(pStrBuf, XML_TAG_MAX_LEN, "CHNum",pstSvcInfo->ulLcn);
		if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));

		XML_WRITE_END(pStrBuf, XML_TAG_MAX_LEN, "row");
		if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));

		ulCountOfService++;
		phSvcList = phSvcList->next;
	}

	XML_WRITE_END(pStrBuf, XML_TAG_MAX_LEN, "tripleid");
	if(pFile != NULL)	HLIB_FILE_Write(pFile,pStrBuf, 1,HxSTD_StrLen(pStrBuf));
	if(pFile != NULL)	HLIB_FILE_Close(pFile);

	if(phTempSvcList != NULL)	HLIB_LIST_RemoveAll(phTempSvcList);
	if(phSvcHash != NULL)	HLIB_HASH_Destroy(phSvcHash);

#if defined(CONFIG_DEBUG)
	{
		HUINT8	* pucServicelistXml = NULL;
		HUINT32 ulLen = 0;
		HLIB_FILE_ReadFile(EPG_TRIPLEID_FILE, (void **)&pucServicelistXml, &ulLen);

		HxLOG_Print("Service list Counts [%d] \n", ulCountOfService);
		if(pucServicelistXml != NULL) HxLOG_Print("Service list xml [%s] [%d]\n", pucServicelistXml, ulLen);
		IPEPG_MemFree(pucServicelistXml);
	}
#endif

	IPEPG_FUNC_END();
	return ERR_OK;
}

static HERROR	ipepg_HttpGetWithCallback(const HINT8 * szUrl, HxHttpGet_Callback *callback, HUINT32 ulTimeout)
{
	HERROR							hError = ERR_FAIL;
	HxTRANSPORT_Http_t			*pHttp = NULL;
	HxTRANSPORT_Httpheader_t		*pHttpHeader = NULL;
	HxTRANSPORT_HttpResponse_e	resCode;
	HFLOAT64						ulRealSize = 0;

    HxLOG_Trace();
	IPEPG_GOTO_IF(szUrl == NULL, EXIT_FUNC);
	pHttp = HLIB_TRANSPORT_HttpOpen();
	IPEPG_GOTO_IF(pHttp == NULL, EXIT_FUNC);
#if defined(TVPORTAL_ROOT_CA) && defined(TVPORTAL_CLIENT_CA_KEY)
	if(pHttp != NULL)
	{
		if(!strncmp(szUrl, HTTPS_PREFIX, HTTPS_PREFIX_URL_LENGTH))
		{
			hError = HLIB_TRANSPORT_HttpSetCACertificateFile(pHttp, TVPORTAL_ROOT_CA);
        		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

			hError = HLIB_TRANSPORT_HttpSetCertificateKeyFile(pHttp, TVPORTAL_CLIENT_CA_KEY, eHxTP_HTTP_CERTIFICATE_P12);
        		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

			hError = HLIB_TRANSPORT_HttpSetPeerVeryfication(pHttp, TRUE);
        		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);

			hError = HLIB_TRANSPORT_HttpSetHostVeryfication(pHttp, FALSE);
        		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_FUNC);
		}
	}
#endif
	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Content-Type: text/xml");
	IPEPG_GOTO_IF(pHttpHeader == NULL, EXIT_FUNC);

#if defined(IPEPG_USE_COMPRESS)
	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Accept-Encoding : GZIP");
#endif
	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader) != ERR_OK, EXIT_FUNC);

    if(ulTimeout != 0)
    {
        IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpSetTimeout(pHttp, ulTimeout) != ERR_OK, EXIT_FUNC);
    }

	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpGetWithCallback(pHttp, szUrl, callback, &ulRealSize) != ERR_OK, EXIT_FUNC);

	hError = ERR_OK;
EXIT_FUNC :

	if ( hError != ERR_OK )
	{
		HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &resCode);
	}

	if(pHttpHeader != NULL) HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
	if(pHttp != NULL) HLIB_TRANSPORT_HttpClose(pHttp);

	return hError;
}

static HUINT8		*s_login_buffer = NULL;
static HINT32		s_login_size = 0;

#define IPEPG_LOGIN_DATA_MAX_SIZE		(1024*1024)

static HINT32   ipepg_LoginCallback(void *ptr, HINT32 size, HINT32 nmemb, void *stream)
{
    HINT32 uiDataSize = 0;
	if(s_login_buffer == NULL)
		return 0;

	uiDataSize = size * nmemb;
	if(s_login_size + uiDataSize > IPEPG_LOGIN_DATA_MAX_SIZE)
		return  0;


	HxSTD_MemCopy(s_login_buffer+s_login_size, ptr, uiDataSize);
	s_login_size += uiDataSize;

    return uiDataSize;
}

#define LOGIN_OK_STRING		"AuthTicket"

static HERROR ipepg_SetIpepgLogin(void)
{
	HUINT8		*pucMaccAddr = NULL;
	HUINT8		*pServerURL = NULL;
	HUINT8		*pucReceivedData = NULL;
	HERROR		ret_err = ERR_FAIL;
	static HBOOL bSuccess = FALSE;
	IPEPG_FUNC_START();

	if(bSuccess == TRUE)
	{
		HxLOG_Print("== > Already Logon Succeed\n");
		ret_err = ERR_OK;
	}
	else
	{
		HINT32		SysOffsetHour = 0, SysOffsetMin = 0;

		if(HLIB_DATETIME_GetOffsetTime (&SysOffsetHour, &SysOffsetMin) != ERR_OK)
		{
			HxLOG_Print("[%s] HLIB_DATETIME_GetOffsetTime() failed\n", __FUNCTION__);
			goto ENDPROC;
		}
		else
		{
			HUINT32		ulSystemId = 0, ulSystemVersion = 0;
			HERROR		hError = ERR_FAIL;
			HUINT8		pEpgLoginUrl[IPEPG_URL_MAX_LEN]={0,};
			HBOOL		bUpdated = FALSE;

			pucMaccAddr = IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);
			pServerURL = IPEPG_GetItemPoint(eIPEPG_ITEM_SERVERURL);
			ulSystemId = IPEPG_GetItemValue(eIPEPG_ITEM_SYS_ID);
			ulSystemVersion = IPEPG_GetItemValue(eIPEPG_ITEM_SYS_VER);

			hError = IPEPG_CheckPrivacyPolicyVer(&bUpdated);
			/* bUpdated is TRUE means that can't access Portal Server, because of GDPR */
			if((hError != ERR_OK) || (bUpdated == TRUE))
			{
				HxLOG_Print("Privacy Policy is not agreed or matched with Humax TV server\n");
				goto ENDPROC;
			}

			if(pucMaccAddr == NULL || pServerURL == NULL)
			{
				HxLOG_Print("pucMaccAddr[%p],pucMaccAddr[%p]\n", pucMaccAddr,pServerURL);
				goto ENDPROC;
			}

			HxSTD_snprintf(pEpgLoginUrl, IPEPG_URL_MAX_LEN,
				"%s/%s%s&MACAddress=%s&SystemID=%02X%02X.%02X%02X&SWVer=%X.%X.%X&serialNumber=&timeZone=%d",
				pServerURL, IPEPG_GETLOGIN_URL,pucMaccAddr, pucMaccAddr,
				(ulSystemId>>24)&0x000000FF, (ulSystemId>>16)&0x000000FF, (ulSystemId>>8)&0x000000FF, (ulSystemId)&0x000000FF,
				(ulSystemVersion>>16)&0x000000FF, (ulSystemVersion>>8)&0x000000FF, (ulSystemVersion)&0x000000FF,
				SysOffsetHour*60+SysOffsetMin
				);

			HxLOG_Print("URL = [%s]\n", pEpgLoginUrl);


			pucReceivedData = (HUINT8 *)IPEPG_MemCalloc(IPEPG_LOGIN_DATA_MAX_SIZE);
			s_login_buffer = pucReceivedData;
			s_login_size = 0;


			hError = ipepg_HttpGetWithCallback(pEpgLoginUrl, ipepg_LoginCallback, IPEPG_HTTP_TIMEOUT);
			if(pucReceivedData)
			{
				HxLOG_Print("LOGIN DATA = [%s]\n", pucReceivedData);
			}
			else
			{
				HxLOG_Error(" IPEPG_MemCalloc error !\n");
				goto ENDPROC;
			}

			if(hError != ERR_OK)
			{
				HxLOG_Error( "LOGIN PUT FAIL==================>>>>>> CRITICAL\n");
				goto ENDPROC;
			}

			if (HxSTD_StrStr(pucReceivedData, LOGIN_OK_STRING) == NULL)
			{
				HxLOG_Error( "LOGIN FAIL==================>>>>>> WRONG RESPONSE [%s]\n", pucReceivedData);
				goto ENDPROC;
			}

			// -----------------
			ret_err = ERR_OK;
			// -----------------
			bSuccess = TRUE;
		}
	}


ENDPROC:
	IPEPG_MemFree(pucMaccAddr);
	IPEPG_MemFree(pServerURL);
	IPEPG_MemFree(pucReceivedData);

	IPEPG_FUNC_END();
	return ret_err;
}

static	HERROR		ipepg_UploadServiceList(IPEPG_DATAMGR_t	*pstMgr)
{
	HBOOL	bServiceUpdated = FALSE;
	UNIXTIME	ulSystemTime = 0;

	IPEPG_FUNC_START();

	if(VK_CLOCK_GetTime((unsigned long*)&ulSystemTime) != ERR_OK)
	{

		HxLOG_Warning("ipepg_UploadServiceList is Fail Time is Not OK\n");
		IPEPG_FUNC_END();
		return ERR_FAIL;
	}

	IPEPG_RETURN_IF(pstMgr == NULL , ERR_FAIL);

	bServiceUpdated = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_SVC_UPDATED);
	if(bServiceUpdated == TRUE)
	{
		IPEPG_FUNC_END();
		return ERR_OK;
	}

	if(ipepg_MakeServicelist(pstMgr) == ERR_OK)
	{
		if(ipepg_SendServiceListToServer(pstMgr) == ERR_OK)
		{
			HxLOG_Debug("SendSvcListTo Server Success\n");
			IPEPG_FUNC_END();
			return ERR_OK;
		}
		else
		{
			HxLOG_Debug("SendSvcListTo Server Fail\n");
		}
	}

	HxLOG_Warning("ipepg_MakeServicelist is Fail\n");
	IPEPG_FUNC_END();
	return ERR_FAIL;

}

static	HERROR		ipepg_Regist_SendSvclistToServer(IPEPG_TASK_MGR_t	*pstMgr)
{
	IPEPG_DATAMGR_t * dataMgr = NULL;
	HBOOL	bIpEpgEnabled = FALSE;
	HBOOL	bServiceUpdated = FALSE;
	HUINT8	*pMacAddress = NULL;
	HERROR	err = ERR_FAIL;

	IPEPG_FUNC_START();

	bIpEpgEnabled = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_IPEPG_ONOFF);
	bServiceUpdated = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_SVC_UPDATED);
	IPEPG_RETURN_IF(pstMgr == NULL || pstMgr->pstData == NULL, ERR_FAIL);
	dataMgr = pstMgr->pstData;
	IPEPG_RETURN_IF(bIpEpgEnabled == FALSE, ERR_FAIL);

	pMacAddress = (HUINT8 *)IPEPG_GetItemPoint(eIPEPG_ITEM_MAC);

	HxLOG_Debug("bIpEpgEnable[%d] [servicelistUpdated[%d] Mac[%s]\n", bIpEpgEnabled, bServiceUpdated, pMacAddress);
	if(bIpEpgEnabled == TRUE && bServiceUpdated == TRUE
		&& pMacAddress != NULL )
	{
		IPEPG_MemFree(pMacAddress);
		HxLOG_Warning("ipepg_Regist_SendSvclistToServer : Updating Service Already Succeed\n");
		IPEPG_FUNC_END()
		return ERR_OK;
	}
	err = ipepg_UploadServiceList(dataMgr);

	if(err == ERR_OK)
	{
		if((pstMgr->eStatus & eIPEPG_STOP) != eIPEPG_STOP)
		{
			HBOOL bNewServiceUpdated = FALSE;
			bNewServiceUpdated = (HBOOL)IPEPG_GetItemValue(eIPEPG_ITEM_FLAG_SVC_UPDATED);
			IPEPG_SetItem(eIPEPG_ITEM_FLAG_SVC_UPDATED,(void *)TRUE);

			if(bServiceUpdated != bNewServiceUpdated)
			{
				IPEPG_SetItem(eIPEPG_ITEM_LAST_UPDATE_TIME, (void *)0);	// Service list 가 새롭게 update 되어지면 ipepg 를 다시 받도록 시간을 초기화 시킨다.
				IPEPG_SetItem(eIPEPG_ITEM_STAMP_TIME, (void *)0);
			}
		}
		else
		{
			err = ERR_FAIL;
			HxLOG_Warning("[%s] pstMgr status is wrong (0x%x) \n", __FUNCTION__, pstMgr->eStatus);
		}
	}
	else
	{
		HxLOG_Warning("ipepg_UploadServiceList is Fail\n");
	}
	IPEPG_MemFree(pMacAddress);
	IPEPG_FUNC_END();

	return err;
}


void IPEPG_UploadSvclistData(void *arg)
{
	HUINT32 			MsgQId = 0;
	HERROR				ret = ERR_OK;
	HUINT32				utCurrTime = 0;

	IPEPG_TASK_MGR_t * pstTaskMgr = (IPEPG_TASK_MGR_t *)arg;
	IPEPG_FUNC_START();

	if(pstTaskMgr == NULL || pstTaskMgr->pstData == NULL)
	{
		IPEPG_FUNC_END();
		return;
	}

	while(1)
	{
		utCurrTime = HLIB_STD_GetSystemTime();
		if(HLIB_DATETIME_IsValidUnixTime(utCurrTime) == ERR_OK)
		{
			break;
		}

		HxLOG_Error("[%s] Invalid Current Time( %d )\n", __FUNCTION__, utCurrTime);
		HLIB_STD_TaskSleep(1000);
	}

	if(ipepg_SetIpepgLogin() == ERR_OK)
	{
		ret = ipepg_Regist_SendSvclistToServer(pstTaskMgr);
		if(ret != ERR_OK)
		{
			HxLOG_Warning("ipepg_Regist_SendSvclistToServer Failed\n");
		}
	}
	else
	{
		HxLOG_Warning("CANT LOGIN. FAILED\n");
	}

	// DO NOT CALL LOG FUNCTION AFTER VK_MSG_SEND
	IPEPG_FUNC_END();

	if(IPEPG_GetMsgQId(&MsgQId) != ERR_FAIL)
	{
		IpEpgMsg_t	stMsg;

		stMsg.ulType   = 0;
#if defined(CONFIG_OP_UK_DTT)
		stMsg.ulMsg    = eIPEPG_MSG_DONE_UPDATE_SERVICELIST;
#else
		stMsg.ulMsg    = eIPEPG_MSG_DONE_DOWNLOAD;
		stMsg.ulParam1 = eIPEPG_UPLOAD_START;
#endif
		stMsg.ulParam2 = ERR_OK;
		stMsg.ulParam3 = 0;

		VK_MSG_Send(MsgQId, (void*)&stMsg, sizeof(IpEpgMsg_t));
	}
}


void IPEPG_UploadSvcTask (void * pstDownMgr)
{
	HUINT32 			ulUploadSvcTaskQId;
	IpEpgMsg_t			stMsg;
	IPEPG_TASK_MGR_t * pstTaskMgr = NULL;

	HxLOG_Trace();

	pstTaskMgr = (IPEPG_TASK_MGR_t *)pstDownMgr;
	if(pstTaskMgr == NULL || pstTaskMgr->ulUploadSvcMsgQId == 0)
	{
		return;
	}
	ulUploadSvcTaskQId = pstTaskMgr->ulUploadSvcMsgQId;

	while(1)
	{
		if (VK_MSG_ReceiveTimeout(ulUploadSvcTaskQId, (void*)&stMsg, sizeof(IpEpgMsg_t), 300) == VK_OK)
		{
			switch (stMsg.ulMsg)
			{
				case eIPEPG_MSG_START_ONLY_SVCLIST:
				case eIPEPG_MSG_START_UPDATE_SERVICELIST:
					IPEPG_UploadSvclistData(pstTaskMgr);
					break;
				default :
					HxLOG_Error("IPEPG_UploadSvcTask reveived OtherMessage[%d]\n", stMsg.ulMsg);
					break;
			}
		}
	}
	return;
}



