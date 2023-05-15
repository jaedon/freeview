/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  ipepg_auth.c
	@brief

	Description:  									\n
	Module: IPEPG / IPEPG Privacy & Policy checker				\n

	Copyright (c) 2018 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
 * (c) 2011-2018 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <unistd.h>
#include <signal.h>

#include <vkernel.h>
#include <apk.h>
#include "../include/ipepg.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#if defined(CFG_RELEASE_DEBUG)
#undef HxLOG_Trace
#define HxLOG_Trace()	printf("[IPEPG] %s %d\n", __FUNCTION__, __LINE__);
#undef HxLOG_Debug
#define HxLOG_Debug		printf
#undef HxLOG_Print
#define HxLOG_Print		printf
#endif

#define MAX_PPVER_LEN			36	
#define LEGAL_DOC_VERSION_API		"api/GetLegaldocver?systemid="
#define IPEPG_LIST_MAX_SIZE		(50*1024)
#define USERDB_PP_VER			"UDB/Privacy_Policy_Ver"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static HUINT8		*s_pulIpepgBuf = NULL;
static HINT32		s_uiIpepgDataSize = 0;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

static HINT32 ipepg_DownloadCb(void *ptr, HINT32 size, HINT32 nmemb, void *stream)
{
	HINT32 uiDataSize = 0;
	if(s_pulIpepgBuf == NULL)
	{
		return 0;
	}

	uiDataSize = size * nmemb;
	if(s_uiIpepgDataSize + uiDataSize > IPEPG_LIST_MAX_SIZE)
	{
		return  0;
	}

	HxSTD_MemCopy(s_pulIpepgBuf + s_uiIpepgDataSize, ptr, uiDataSize);
	s_uiIpepgDataSize += uiDataSize;

	return uiDataSize;
}

static HERROR	ipepg_HttpGetWithCallback(const HINT8 *szUrl, HxHttpGet_Callback *callback, HUINT32 ulTimeout)
{
	HERROR					hError = ERR_FAIL;
	HxTRANSPORT_Http_t			*pHttp = NULL;
	HxTRANSPORT_Httpheader_t		*pHttpHeader = NULL;
	HxTRANSPORT_HttpResponse_e		resCode;
	HFLOAT64				ulRealSize = 0;

	HxLOG_Trace();
	IPEPG_GOTO_IF(szUrl == NULL, EXIT_CNXTHUMAX_FUNC);

	pHttp = HLIB_TRANSPORT_HttpOpen();
	IPEPG_GOTO_IF(pHttp == NULL, EXIT_CNXTHUMAX_FUNC);
#if defined(TVPORTAL_ROOT_CA) && defined(TVPORTAL_CLIENT_CA_KEY)
	if(pHttp != NULL)
	{
		if(!strncmp(szUrl, HTTPS_PREFIX, HTTPS_PREFIX_URL_LENGTH))
		{
			hError = HLIB_TRANSPORT_HttpSetCACertificateFile(pHttp, TVPORTAL_ROOT_CA);
        		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_CNXTHUMAX_FUNC);

			hError = HLIB_TRANSPORT_HttpSetCertificateKeyFile(pHttp, TVPORTAL_CLIENT_CA_KEY, eHxTP_HTTP_CERTIFICATE_P12);
        		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_CNXTHUMAX_FUNC);

			hError = HLIB_TRANSPORT_HttpSetPeerVeryfication(pHttp, TRUE);
        		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_CNXTHUMAX_FUNC);

			hError = HLIB_TRANSPORT_HttpSetHostVeryfication(pHttp, FALSE);
        		IPEPG_GOTO_IF(hError != ERR_OK, EXIT_CNXTHUMAX_FUNC);
		}
	}
#endif
	pHttpHeader = HLIB_TRANSPORT_HttpAppendHeader(pHttpHeader, "Content-Type: text/xml");
	IPEPG_GOTO_IF(pHttpHeader == NULL, EXIT_CNXTHUMAX_FUNC);
	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpSetHeaderToTP(pHttp, pHttpHeader) != ERR_OK, EXIT_CNXTHUMAX_FUNC);

	if(ulTimeout != 0)
	{
        	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpSetTimeout(pHttp, ulTimeout) != ERR_OK, EXIT_CNXTHUMAX_FUNC);
	}

	IPEPG_GOTO_IF(HLIB_TRANSPORT_HttpGetWithCallback(pHttp, szUrl, callback, &ulRealSize) != ERR_OK, EXIT_CNXTHUMAX_FUNC);

	hError = ERR_OK;

EXIT_CNXTHUMAX_FUNC :

	if ( hError != ERR_OK )
	{
		HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &resCode);
	}

	if(pHttpHeader != NULL)
	{
		HLIB_TRANSPORT_HttpFreeHeader(pHttpHeader);
	}

	if(pHttp != NULL) 
	{
		HLIB_TRANSPORT_HttpClose(pHttp);
	}

	return hError;
}

/* Get and check Legal Doc (Privacy & Policy) version from Humax TV Portal */
static HERROR ipepg_ParsePPVersion(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HCHAR *ucPPVersion)
{
	HERROR		eErr = ERR_FAIL;
	HxJSON_t	jsonRoot=NULL,jsonItem=NULL,jsonArray=NULL;
	HCHAR		*ucPrivacy=NULL, *ucPPKey=NULL, *ucPPSeq=NULL;
	HUINT32		i=0,ulCnt=0;

	if(ucPPVersion == NULL)
	{
		HxLOG_Error("ucPPVersion is NULL!!\n");
		return eErr;
	}

	/* json parsing */
	jsonRoot = HLIB_JSON_Decode(pRawBuffer);
	if(jsonRoot == NULL)
	{
		return eErr;
	}
	jsonArray = HLIB_JSON_Object_GetArray(jsonRoot,"list");
	if(jsonArray == NULL)
	{
		HLIB_JSON_Delete(jsonRoot);
		return eErr;
	}

	ulCnt = HLIB_JSON_GetSize(jsonArray);
	for(i=0; i < ulCnt; i++)
	{
		jsonItem = HxJSON_Array_Get(jsonArray,i);
		ucPrivacy = (HCHAR *)HLIB_JSON_Object_GetString(jsonItem,"docType");
		if(ucPrivacy == NULL)
		{
			continue;
		}
		if(!strcmp(ucPrivacy,"privacy"))
		{
			
			ucPPKey = (HCHAR *)HLIB_JSON_Object_GetString(jsonItem,"key");
			if(ucPPKey == NULL)
			{
				break;
			}

			ucPPSeq = (HCHAR *)HLIB_JSON_Object_GetString(jsonItem,"seq");
			if(ucPPSeq == NULL)
			{
				break;
			}
			HxSTD_snprintf(ucPPVersion,IPEPG_MAX_PPVER_LEN,"%s_%s",ucPPKey,ucPPSeq);
			HxLOG_Debug("P/P Version =[%s]\n",ucPPVersion);
			eErr = ERR_OK;
		}
			
	}

	HLIB_JSON_Delete(jsonRoot);

	return eErr;	
}

HERROR IPEPG_CheckPrivacyPolicyVer(HBOOL *bUpdated)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		pLegalDocPPUrl[IPEPG_URL_MAX_LEN] = {0,};
	HCHAR		szPPVer[IPEPG_MAX_PPVER_LEN] = {0,};
	HUINT32		ulReceivedSize = 0,ulDbLen=0,ulUrlLen=0, ulSystemId=0;
	HUINT8		*pucServerURL = NULL, *pucPPVer=NULL;

	if(bUpdated == NULL)
	{
		return ERR_FAIL;
	}

	pucServerURL = IPEPG_GetItemPoint(eIPEPG_ITEM_SERVERURL);	
	if(pucServerURL == NULL)
	{
		goto EXIT_FUNC;
	}

	pucPPVer = IPEPG_GetItemPoint(eIPEPG_ITEM_PRIVACY_POLICY_VER);	
	if(pucPPVer == NULL)
	{
		goto EXIT_FUNC;;
	}

	ulSystemId = IPEPG_GetItemValue(eIPEPG_ITEM_SYS_ID);
	
	HxSTD_MemSet(pLegalDocPPUrl, 0x00, IPEPG_URL_MAX_LEN);

	HxSTD_snprintf(pLegalDocPPUrl, IPEPG_URL_MAX_LEN, "%s/%s%02X%02X.%02X%02X", pucServerURL, LEGAL_DOC_VERSION_API, (ulSystemId >> 24) & 0x000000FF, (ulSystemId >> 16) & 0x000000FF, (ulSystemId >> 8) & 0x000000FF, ulSystemId & 0x000000FF);

	HxLOG_Debug("URL : %s\n", pLegalDocPPUrl);

	if(s_pulIpepgBuf == NULL)
	{
		s_pulIpepgBuf = (HUINT8 *)IPEPG_MemAlloc(IPEPG_LIST_MAX_SIZE);
		if(s_pulIpepgBuf == NULL)
		{
			hError = ERR_FAIL;
			goto EXIT_FUNC;
		}
	}

	HxSTD_MemSet(s_pulIpepgBuf, 0x00, IPEPG_LIST_MAX_SIZE);

	s_uiIpepgDataSize = 0;
	hError = ipepg_HttpGetWithCallback(pLegalDocPPUrl, ipepg_DownloadCb, IPEPG_HTTP_TIMEOUT);
	if(hError != ERR_OK)
	{
		goto EXIT_FUNC;
	}

	ulReceivedSize = (HUINT32)s_uiIpepgDataSize;

	HxLOG_Debug("Rec DATA: %s [%d]\n", s_pulIpepgBuf, ulReceivedSize);

	HxSTD_MemSet(szPPVer,0x00,IPEPG_MAX_PPVER_LEN);

	/* Parsing pulIpepgBuf data based on Json format */
	hError=ipepg_ParsePPVersion(s_pulIpepgBuf, ulReceivedSize, szPPVer);
	if(hError == ERR_OK)
	{
		/* Get PP version from UDB and compare szPPVer from Humax TV Server */
		ulDbLen=HxSTD_StrLen(pucPPVer);
		ulUrlLen=HxSTD_StrLen(szPPVer);

		HxLOG_Debug("pucPPVer = %s(%d), szPPVer = %s(%d)\n",pucPPVer,ulDbLen, szPPVer,ulUrlLen);

		if((ulDbLen != ulUrlLen)
			|| (HxSTD_StrNCmp(pucPPVer,szPPVer,ulUrlLen) != 0))
		{
			HxLOG_Debug("P/P ver is differ, UDB ver = %s, Server ver = %s \n",pucPPVer, szPPVer);
			*bUpdated = TRUE;
		}
		else
		{
			HxLOG_Debug("P/P ver is same, UDB ver = %s, Server ver = %s \n",pucPPVer, szPPVer);
			*bUpdated = FALSE;
		}
	}

EXIT_FUNC:
	if(s_pulIpepgBuf != NULL)
	{
		IPEPG_MemFree(s_pulIpepgBuf);
		s_pulIpepgBuf = NULL;
	}

	s_uiIpepgDataSize = 0;

	if(pucServerURL != NULL)
	{
		IPEPG_MemFree(pucServerURL);
	}

	if(pucPPVer != NULL)
	{
		IPEPG_MemFree(pucPPVer);
	}

	return hError;
}
