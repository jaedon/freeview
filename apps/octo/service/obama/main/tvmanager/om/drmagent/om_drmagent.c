/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_drmagent.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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

#if defined(CONFIG_SUPPORT_IMMA)
#define PLAYREADY_SUPPORT
#endif
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <svc_pipe.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_drmagent.h>

#ifdef PLAYREADY_SUPPORT
#include "humax_platform.h"
#include "drmmanager.h"
#include "_om_drmagent_util_http.h"
#endif
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
STATIC HULONG					s_ulOmDrmAgentSemId;
#define	ENTER_OM_DRMAGENT_CACHE	VK_SEM_Get(s_ulOmDrmAgentSemId)
#define	LEAVE_OM_DRMAGENT_CACHE	VK_SEM_Release(s_ulOmDrmAgentSemId)
#define MSG_CACHE 10 // Need Define

#ifdef PLAYREADY_SUPPORT
#define HDR_LICGET_ACTION_VALUE 		"\"http://schemas.microsoft.com/DRM/2007/03/protocols/AcquireLicense\""
#endif


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	e_MgrDrmAgent_SendDRMSystemID_t systemID;
	HUINT16 usMsgId;
	HUINT8 count;
}OmDrmAgent_DrmMessageResponseCount_t;

typedef struct
{
	OmDrmAgent_DrmMessageResponseCount_t stMsg[MSG_CACHE];
	HUINT16 usTotalCount;
}OmDrmAgent_DrmMessage_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC OmDrmAgent_DrmMessage_t stSendmsg ;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#ifdef PLAYREADY_SUPPORT

#ifdef PLAY_READY_25
const DRM_WCHAR rgwchStoreName [] =
{
	ONE_WCHAR('/','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('y','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('e','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('y','\0'), ONE_WCHAR('2','\0'), ONE_WCHAR('5','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e','\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h','\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('s','\0'), ONE_WCHAR('\0','\0')
};

#else /* PLAY_READY_25 */

static DRM_WCHAR rgwchStoreName [] =
{
	ONE_WCHAR('/','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e', '\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')
};
#endif /* PLAY_READY_25 */

#endif /*PLAYREADY_SUPPORT*/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define _____OAPI_COMMAND_FUNCTIONS_____
// bus task 가 아님을 명심한다. 즉, bus에 message 보내기 위해서는 bus task에게 send/post message 해야함. 절대로 바로 mgr code를 부르지 않는다.

#ifdef PLAYREADY_SUPPORT
DRM_VOID * Oem_Create(void);
DRM_VOID Oem_Destroy( DRM_VOID *f_pOEMContext);
#endif

static HUINT8* om_drmagent_LocalUnicodeStrstr(HUINT8 *pSrc, HINT32 srcBytes, HUINT8 *pDst, HINT32 dstBytes)
{
	HUINT32 nPos = 0;
	HINT32 i,j;
	for(i = 0 ; i < srcBytes ; i++)
	{
		for(j = 0 ; j < dstBytes; j++)
		{
			if(pSrc[i + j] == pDst[j])
			{
				if(j == dstBytes - 1)
				{
					nPos = (i + j) - (dstBytes - 1);
					return pSrc + nPos;
				}
			}
			else
			{
				break;
			}
		}
	}
	return NULL;
}

STATIC void om_drmagent_MakeFieldOfDRMResultMsg(HUINT8 **pucMsg, HUINT32 *sizeofbuffer, e_MgrDrmAgent_XmlStructure_t tag, e_MgrDrmAgent_XmlStructure_t attribute, HUINT8 *pDrmResultElement)
{
	HUINT8	tagName[128] = "";
	HUINT8 *pTemp = NULL;

	HINT32 needlength  = 0;

	if( pucMsg == NULL || *pucMsg == NULL)
	{
		HxLOG_Error("Invalid Param\n");
		return ;
	}
	needlength = HxSTD_StrLen(*pucMsg) + HxSTD_StrLen(st_DrmResultMsgField[tag].ucStr) + HxSTD_StrLen(st_DrmResultMsgField[attribute].ucStr) ;
	if(pDrmResultElement != NULL)
		needlength +=  HxSTD_StrLen(pDrmResultElement);

	if(*sizeofbuffer < needlength)
	{
		HUINT8 *newptr = NULL;
		*sizeofbuffer = needlength+1;
		newptr = HLIB_STD_MemAlloc(*sizeofbuffer);
		if(newptr==NULL)
		{
			HxLOG_Error("Out of Memory\n");
			return ;
		}
		HxSTD_MemSet(newptr, 0, *sizeofbuffer);
		HxSTD_StrCpy(newptr, *pucMsg);
		HLIB_STD_MemFree(*pucMsg);
		*pucMsg = newptr;
	}

	if(tag!=eMGR_DRMAGENT_XML_TAG_NONE)// Tag Insert
	{
		HxSTD_StrCat(*pucMsg, st_DrmResultMsgField[tag].ucStr);
	}
	if(attribute!=eMGR_DRMAGENT_XML_ATTRIBUTE_NONE)// Attribute Insert
	{
		if(tag==eMGR_DRMAGENT_XML_TAG_NONE)
		{ // must specify tag value
			HxLOG_Error("must specify tag value\n");
			return ;
		}
#if 0
		HxSTD_StrNCpy(tagName, st_DrmResultMsgField[tag].ucStr+1, HxSTD_StrLen(st_DrmResultMsgField[tag].ucStr)-2);  // 1='<',  2= '<', '>'
		pTemp = HxSTD_StrStr(*pucMsg, tagName);
		HxSTD_StrNCpy(pTemp+HxSTD_StrLen(tagName), " ",HxSTD_StrLen(" ")+1); // 1 = to insert null.
		HxSTD_StrNCpy(pTemp+HxSTD_StrLen(tagName)+HxSTD_StrLen(" "), st_DrmResultMsgField[attribute].ucStr,HxSTD_StrLen(st_DrmResultMsgField[attribute].ucStr)+1);
		HxSTD_StrCat(*pucMsg, ">");
#endif
		HxSTD_StrNCpy(tagName, st_DrmResultMsgField[tag].ucStr+1, HxSTD_StrLen(st_DrmResultMsgField[tag].ucStr)-2);  // 1='<',	2= '<', '>'
		pTemp = HxSTD_StrStr(*pucMsg, tagName);
		if( pTemp == NULL )
		{
			HxLOG_Error("Not Find \n");
			return  ;
		}
		pTemp += HxSTD_StrLen(tagName);
		HxSTD_StrNCpy(pTemp, " ",HxSTD_StrLen(" ")+1);

		pTemp += HxSTD_StrLen(" ");
		HxSTD_StrNCpy(pTemp, st_DrmResultMsgField[attribute].ucStr, HxSTD_StrLen(st_DrmResultMsgField[attribute].ucStr)+1);

		pTemp += HxSTD_StrLen(st_DrmResultMsgField[attribute].ucStr);
		HxSTD_StrNCpy(pTemp, ">", HxSTD_StrLen(" ")+1); // 1 = to insert null.
	}

	if(pDrmResultElement!=NULL)// Element Insert
	{
		if(tag==eMGR_DRMAGENT_XML_TAG_NONE)
		{ // must specify tag value
			HxLOG_Error("must specify tag value\n");
			return ;
		}
		pTemp = HxSTD_StrStr(*pucMsg, st_DrmResultMsgField[tag].ucStr);
		if( pTemp == NULL )
		{
			HxLOG_Error("Not Find \n");
			return  ;
		}
		HxSTD_StrNCpy(pTemp+HxSTD_StrLen(st_DrmResultMsgField[tag].ucStr), pDrmResultElement, HxSTD_StrLen(pDrmResultElement)+1);
	}
	return ;
}

STATIC HERROR om_drmagent_CheckDRMSystemID(HCHAR * pzDrmSystemID, e_MgrDrmAgent_SendDRMSystemID_t *systemID)
{
	HUINT16 i = 0;

	for(i = 0; i < eMGR_DRMAGENT_SYSTEMID_MAX ; i++)
	{
		if(HxSTD_StrCmp((const char *)pzDrmSystemID, (const char *)stDrmSystemId[i].ucStr) == 0)
		{
			*systemID =stDrmSystemId[i].eDrmSystemID;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}

STATIC HERROR om_drmagent_FindCacheMessageIndex(HUINT16 usMsgId, HUINT16 *index)
{
	HUINT16 i = 0;

	for(i = 0; i < MSG_CACHE ; i++)
	{
		if(stSendmsg.stMsg[i].usMsgId == usMsgId)
		{
			*index = i;
			return ERR_OK;
		}
	}
	return ERR_FAIL;
}


STATIC HERROR om_drmagent_ParseDrmMessageType(HCHAR * pucMsgType, e_MgrDrmAgent_SendMsgType_t *etMsgType )
{
	HUINT16 i = 0;
	*etMsgType = eMGR_DRMAGENT_MSGTYPE_MAX;

	if(pucMsgType == NULL)
	{
		HxLOG_Error("pucMsgType is NULL\n");
		return ERR_FAIL;
	}

	for(i = 0; i < eMGR_DRMAGENT_MSGTYPE_MAX; i++) // TODO:
	{
		if(HxSTD_StrCmp((const HUINT8 *)pucMsgType, (const HUINT8 *)stDrmType[i].ucStr) == 0)
		{
			*etMsgType = stDrmType[i].eMsgType;
			HxLOG_Error("Message Type Collect ( %d )\n", *etMsgType);
		}
	}

	if(*etMsgType == eMGR_DRMAGENT_MSGTYPE_MAX)
	{
		HxLOG_Error("eMsgType is error\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}

STATIC HERROR om_drmagent_MakeDRMResultMsg(HUINT8 **pBuffer, HUINT32 sizeofbuffer, HUINT8 *pDrmResultElement)
{
	HERROR			eResult = ERR_OK;
	HINT32 			buffersize = sizeofbuffer;
	//strcat of xmldeclaration.
	HxSTD_StrCat(*pBuffer, s_XmlDeclaration);

	om_drmagent_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_PLAYREADYRESPONSE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_XMLNS, NULL);
	om_drmagent_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_DRM_RESULT_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "0");
	om_drmagent_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_DRM_RESULT_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);

	if(pDrmResultElement != NULL && HxSTD_StrLen(pDrmResultElement))
	{
		HxSTD_StrCat(*pBuffer, pDrmResultElement) ;
		buffersize += HxSTD_StrLen(pDrmResultElement) ;
	}

	om_drmagent_MakeFieldOfDRMResultMsg(pBuffer, &buffersize, eMGR_DRMAGENT_XML_TAG_PLAYREADYRESPONSE_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);

	return eResult;
}

STATIC HERROR om_drmagent_CheckFileExist(const char *pathname)
{
	struct stat st;
	return (0 == stat(pathname, &st) && S_ISREG(st.st_mode)) ? ERR_OK : ERR_FAIL;
}
STATIC HERROR om_drmagent_GetContentsFromTag(HUINT8 *pMsg, HUINT8 *ptagname, HUINT8 **pContent, HINT32 inlength, HINT32 *outlength)
{
	HUINT8 *firstElement=NULL;
	HUINT8 *lastElement=NULL;
	HUINT8 *firstsearched=NULL;
	HUINT8 *lastsearched=NULL;
	HERROR ret = ERR_OK;
	HINT32 sizeoftagname ;
	HINT32 sizeofcontents ;

	if(ptagname == NULL)
	{
		ret = ERR_FAIL;
		goto getcontents_err;
	}
	sizeoftagname = HxSTD_StrLen(ptagname);

	firstElement = HLIB_STD_MemAlloc(sizeoftagname+3); //<content>\0
	lastElement = HLIB_STD_MemAlloc(sizeoftagname+4); //</content>\0
	if(firstElement == NULL || lastElement == NULL)
	{
		ret = ERR_FAIL;
		goto getcontents_err;
	}
	HxSTD_MemSet(*pContent, 0, inlength);

	HxSTD_MemSet(firstElement, 0, sizeoftagname+3);
	HxSTD_MemSet(lastElement, 0, sizeoftagname+4);

	firstElement[0]='<';
	lastElement[0]='<';
	lastElement[1]='/';

	HxSTD_StrCpy(firstElement+1, ptagname);
	HxSTD_StrCpy(lastElement+2, ptagname);

	firstElement[sizeoftagname+1] = '>';
	lastElement[sizeoftagname+2] = '>';

	firstsearched = HxSTD_StrStr(pMsg, firstElement);
	lastsearched = HxSTD_StrStr(pMsg, lastElement);

	if(firstsearched == NULL || lastsearched == NULL)
	{
		ret = ERR_FAIL;
		goto getcontents_err;
	}

	sizeofcontents = HxSTD_StrLen(firstsearched) - HxSTD_StrLen(lastsearched) - HxSTD_StrLen(firstElement);
	*outlength = sizeofcontents;

	if(sizeofcontents != 0)
	{
		if(inlength < sizeofcontents)
		{
			HLIB_STD_MemFree(*pContent);
			*pContent = HLIB_STD_MemAlloc(sizeofcontents+1);
			HxSTD_MemSet(*pContent, 0, sizeofcontents+1);

		}
		HxSTD_StrNCpy(*pContent, firstsearched+HxSTD_StrLen(firstElement),  sizeofcontents);
	}

	ret = ERR_OK;
getcontents_err:

	if(firstElement != NULL)
		HLIB_STD_MemFree(firstElement);
	if(lastElement != NULL)
		HLIB_STD_MemFree(lastElement);

	return ret;
}

#ifdef PLAYREADY_SUPPORT
STATIC HERROR om_drmagent_PRDYLicensePreAcquisition(HUINT8 * pucDrmMsg, HUINT8** pucResultMsg, HUINT32 msgSize, e_MgrDrmAgent_ResultCodeType_t* etResultCode)
{
 // PlayReady SDK Call Sequence.
	HUINT8 *pContents=NULL;
	HINT32 content_length=0;
	HUINT8 *pCustomData=NULL;
	HINT32 lCustomData_length=0;
	DRM_LICENSE_RESPONSE LicenseResponse;
	DRM_BYTE *__macro_pbOpaqueBuffer=NULL;
	DRM_APP_CONTEXT* pstAppContext = NULL;
	DRM_VOID *f_pOEMContext = NULL;
	DRM_DWORD f_cRights=0;
	DRM_DWORD f_pcbChallenge=0;
	DRM_BYTE* f_pbChallenge=NULL;
	uint8_t* pucResponseStart=NULL;
	HUINT32 uiResponseSize=0;
	uint8_t* pucResponseData=NULL;
	HINT32 iResult=0;
	DRM_BYTE* pbData=NULL;
	DRM_DWORD cbData=0;
	DRM_BYTE *pbRevocationBuffer=NULL;
	DRM_DWORD *pf_pcchSilentURL=NULL;
	DRM_CHAR* f_pchSilentURL=NULL;
	DRM_DWORD f_pcchSilentURL=0;
	HUINT8 *unicodedwrmheader = NULL;
	HINT32 i=0 ;
	HERROR eResult = ERR_FAIL;
	const DRM_CONST_STRING *rgpdstrRights [1] = { 0 };
	const DRM_CONST_STRING dstrStoreName = CREATE_DRM_STRING(rgwchStoreName);
	HUINT32 resultMsgSize = msgSize;
	uint8_t ucWrmHeaderUnicodeBegin[]={'<','\0','W','\0','R','\0','M','\0','H','\0','E','\0','A','\0','D','\0','E','\0','R','\0'};
	DRM_RESULT dResult = DRM_E_FAIL;

	pContents = HLIB_STD_MemAlloc(DRMAGENT_DRMMESSAGE_MAX_LEN);
	if(pContents == NULL)
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR;
		return ERR_FAIL;
	}
	HxSTD_MemSet(pContents, 0, DRMAGENT_DRMMESSAGE_MAX_LEN);

	pCustomData = HLIB_STD_MemAlloc(DRMAGENT_DRMMESSAGE_MAX_LEN);
	if(pCustomData == NULL)
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR;
		HLIB_STD_MemFree(pContents);
		return ERR_FAIL;
	}

	if(om_drmagent_GetContentsFromTag(pucDrmMsg, "Header", &pContents, DRMAGENT_DRMMESSAGE_MAX_LEN, &content_length)!=ERR_OK)
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT;
		HLIB_STD_MemFree(pCustomData);
		HLIB_STD_MemFree(pContents);
		return ERR_FAIL;
	}

	//make unicode
	unicodedwrmheader = Oem_MemAlloc(content_length*2);
	for(i=0 ; i<content_length*2; i+=2)
	{
		unicodedwrmheader[i] = pContents[i/2];
		unicodedwrmheader[i+1]='\0';
	}

	if(om_drmagent_GetContentsFromTag(pucDrmMsg, "CustomData", &pCustomData, DRMAGENT_DRMMESSAGE_MAX_LEN, &lCustomData_length)!=ERR_OK)
	{
		/*This is NOT error. */
		HLIB_STD_MemFree(pCustomData);
		pCustomData = NULL;
	}

	if(lCustomData_length == 0 && pCustomData != NULL)
	{
		/*This is NOT error. CustomData tag is existed. but data is not set like this -> <CustomData></CustomData> */
		HLIB_STD_MemFree(pCustomData);
		pCustomData = NULL;
	}


	__macro_pbOpaqueBuffer = (DRM_BYTE*)Oem_MemAlloc(MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE);
	pstAppContext = (DRM_APP_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_APP_CONTEXT));

	f_pOEMContext = (DRM_VOID*)Oem_Create();
	if(f_pOEMContext == NULL)
	{
		HxLOG_Error("#### #### f_pOEMContext == NULL\n");
		goto licenseAcq_error;
	}

	dResult = Drm_Initialize(pstAppContext, f_pOEMContext, __macro_pbOpaqueBuffer, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE, &dstrStoreName);
	if(dResult != DRM_SUCCESS)
	{
		HxLOG_Error("#### #### Drm_Initialize is failed! 0x%X\n", dResult);
		goto licenseAcq_error;
	}

	pbRevocationBuffer = (DRM_BYTE *)Oem_MemAlloc(REVOCATION_BUFFER_SIZE);
	dResult = Drm_Revocation_SetBuffer(pstAppContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE);
	if(dResult != DRM_SUCCESS)
	{
		HxLOG_Error("#### #### Drm_Revocation_SetBuffer is failed! 0x%X\n", dResult);
		goto licenseAcq_error;
	}

	if(om_drmagent_LocalUnicodeStrstr(unicodedwrmheader, content_length*2, (unsigned char*)ucWrmHeaderUnicodeBegin, sizeof(ucWrmHeaderUnicodeBegin))!=NULL)
	{
		dResult = Drm_Content_SetProperty(pstAppContext, DRM_CSP_V4_HEADER, unicodedwrmheader, content_length*2);
		if(dResult != DRM_SUCCESS)
		{
			HxLOG_Error("#### #### Drm_Content_SetProperty is failed\n! 0x%X\n", dResult);
			goto licenseAcq_error;
		}
#if 0
		HxLOG_Info("setting wrmheader = ");
		for(i=0 ; i<length*2 ; ++i)
		{
			HxLOG_Info("%c ",unicodedwrmheader[i]);
		}
		HxLOG_Info("\n\n");
#endif
	}

	pf_pcchSilentURL = &f_pcchSilentURL;
	rgpdstrRights[0] = &g_dstrWMDRM_RIGHT_PLAYBACK;
	f_cRights = NO_OF(rgpdstrRights);
	dResult = Drm_LicenseAcq_GenerateChallenge(pstAppContext,
					rgpdstrRights,
					f_cRights,
					NULL,
					pCustomData,
					lCustomData_length,
					NULL,
					pf_pcchSilentURL,
					NULL,
					NULL,
					NULL,
					&f_pcbChallenge);
	if(dResult == DRM_E_BUFFERTOOSMALL)
	{
		if(pf_pcchSilentURL!=NULL)
		{
			f_pchSilentURL = (DRM_CHAR*) Oem_MemAlloc(*pf_pcchSilentURL * SIZEOF (DRM_CHAR));
		}
		else
		{
			f_pchSilentURL = NULL;
		}

		f_pbChallenge = (DRM_BYTE*) Oem_MemAlloc((f_pcbChallenge * SIZEOF(DRM_BYTE)) + 1);
		dResult = Drm_LicenseAcq_GenerateChallenge(pstAppContext,
														rgpdstrRights,
														f_cRights,
														NULL,
														pCustomData,
														lCustomData_length,
														f_pchSilentURL,
														pf_pcchSilentURL,
														NULL,
														NULL,
														f_pbChallenge,
														&f_pcbChallenge);
		if(dResult != DRM_SUCCESS)
		{
			HxLOG_Error("#### #### Drm_LicenseAcq_GenerateChallenge is failed,	Result Code = 0x%X\n", dResult);
			goto licenseAcq_error;
		}
		f_pbChallenge[f_pcbChallenge] = '\0';
		HxLOG_Debug("[SIZE %d]Challenge: [%s]\n",f_pcbChallenge, f_pbChallenge);

	}
	else
	{
		HxLOG_Error("#### #### Drm_LicenseAcq_GenerateChallenge's get Challenge Size is failed Result Code = 0x%X\n", dResult);
		goto licenseAcq_error;
	}
/*********
* SEND Challenge Data to License Server
*********/
	{
		iResult = om_drmagent_bhttpclient_license_post_soap_ex(
			(char*)f_pchSilentURL,
			(char*)f_pbChallenge,
			0,
			2000,
			HDR_LICGET_ACTION_VALUE,
			&pucResponseData,
			&pucResponseStart,
			&uiResponseSize,
			NULL,
			0);

		if(iResult != 0)
		{
			HxLOG_Error("#### #### bhttpclient_license_post_soap_ex is failed\n");
			dResult = DRM_E_FAIL;
			goto licenseAcq_error;
		}
		pbData = pucResponseStart;
		cbData = uiResponseSize;

		HxSTD_MemSet(&LicenseResponse, 0x0, sizeof(DRM_LICENSE_RESPONSE));
		dResult = Drm_LicenseAcq_ProcessResponse(pstAppContext, DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
				NULL,
				NULL,
				pbData,
				cbData,
				&LicenseResponse);
		if(dResult == DRM_SUCCESS)
		{
			HxLOG_Debug("License is Stored well!\n");
		}
		else if(dResult == DRM_S_MORE_DATA)
		{

			int element_length;
			if( HxSTD_StrStr(pbData, "<CustomData>") != NULL)
			{
				HUINT8 *element = NULL;
				element = HLIB_STD_MemAlloc(DRMAGENT_DRMMESSAGE_MAX_LEN); //<content>\0
				if(element == NULL)
				{

HxLOG_Error("Out of Memory \n");
					goto licenseAcq_error;
				}
				if(om_drmagent_GetContentsFromTag(pbData, "CustomData", &element, 512, &element_length)!=ERR_OK)
				{
					if(element!=NULL)
						HLIB_STD_MemFree(element);
					goto licenseAcq_error;
				}

				om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_CUSTOMDATA_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, element);
				om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_CUSTOMDATA_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
				if(element!=NULL)
					HLIB_STD_MemFree(element);
				dResult = DRM_SUCCESS;
			}
			else
			{
				HxLOG_Error(" \n\n After Drm_LicenseAcq_ProcessResponse Result Code = 0x%0X bot result value does not have a tag (<CustomData>)\n",dResult);
				goto licenseAcq_error;
			}
		}
		else
		{
			HxLOG_Error(" \n\n After Drm_LicenseAcq_ProcessResponse Result Code = 0x%0X\n",dResult);
			goto licenseAcq_error;
		}

	}
licenseAcq_error:
	if(pContents!=NULL)
		HLIB_STD_MemFree(pContents);
	if(pCustomData!=NULL)
		HLIB_STD_MemFree(pCustomData);
	if(unicodedwrmheader!=NULL)
		Oem_MemFree(unicodedwrmheader);
	if(pstAppContext != NULL)
	{
		Drm_Uninitialize(pstAppContext);
		Oem_MemFree(pstAppContext);
	}
	if(__macro_pbOpaqueBuffer != NULL)
		Oem_MemFree(__macro_pbOpaqueBuffer);
	if(f_pchSilentURL != NULL)
		Oem_MemFree(f_pchSilentURL);
	if(f_pbChallenge != NULL)
		Oem_MemFree(f_pbChallenge);
	if(pbRevocationBuffer != NULL)
		Oem_MemFree(pbRevocationBuffer);
	if(pucResponseData != NULL)
	{
		HLIB_STD_MemFree(pucResponseData);
	}
	if(f_pOEMContext)
		Oem_Destroy(f_pOEMContext);
	if(dResult == DRM_SUCCESS)
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_SUCCESSFUL;
		eResult = ERR_OK;
	}
	else
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_CANNOT_PROCESS_REQUEST;
		eResult = ERR_FAIL;
	}
	return eResult;

}
STATIC HERROR om_drmagent_PRDYLicenseStateQuery(HUINT8 * pucDrmMsg, HUINT8** pucResultMsg, HUINT32 msgSize, e_MgrDrmAgent_ResultCodeType_t* etResultCode)
{
// PlayReady SDK Call Sequence.
	HUINT8 *pContents=NULL;
	DRM_APP_CONTEXT* pstAppContext = NULL;
	DRM_BYTE *__macro_pbOpaqueBuffer=NULL;
	HUINT8	dw;
	HUINT8	cRights = 1;
	DRM_RESULT dResult;
	const DRM_CONST_STRING dstrStoreName = CREATE_DRM_STRING(rgwchStoreName);
	const DRM_CONST_STRING		*apdcsRights_test [cRights];
	DRM_VOID *f_pOEMContext = NULL;
	uint8_t ucWrmHeaderUnicodeBegin[]={'<','\0','W','\0','R','\0','M','\0','H','\0','E','\0','A','\0','D','\0','E','\0','R','\0'};
	DRM_LICENSE_STATE_DATA		 statedatavales[NO_OF(apdcsRights_test)];
	HUINT8 *unicodedwrmheader;
	HERROR eResult = ERR_FAIL;
	HUINT32 resultMsgSize = msgSize;

	int content_length;
	int i;

	pContents = HLIB_STD_MemAlloc(DRMAGENT_DRMMESSAGE_MAX_LEN);
	if(pContents == NULL)
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR;
		return ERR_FAIL;
	}

	HxSTD_MemSet(pContents, 0, DRMAGENT_DRMMESSAGE_MAX_LEN);

	if(om_drmagent_GetContentsFromTag(pucDrmMsg, "Header", &pContents, DRMAGENT_DRMMESSAGE_MAX_LEN, &content_length)!=ERR_OK)
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT;
		HLIB_STD_MemFree(pContents);
		return ERR_FAIL;
	}

	pstAppContext = (DRM_APP_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_APP_CONTEXT));
	__macro_pbOpaqueBuffer = (DRM_BYTE*)Oem_MemAlloc(MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE);

	//make unicode
	unicodedwrmheader = Oem_MemAlloc(content_length*2);
	if(unicodedwrmheader == NULL)
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR;
		HLIB_STD_MemFree(pContents);
		return ERR_FAIL;
	}
	for(i=0 ; i<content_length*2; i+=2)
	{
		unicodedwrmheader[i] = pContents[i/2];
		unicodedwrmheader[i+1]='\0';
	}

	f_pOEMContext = (DRM_VOID*)Oem_Create();
	if(f_pOEMContext == NULL)
	{
		HxLOG_Error("#### #### f_pOEMContext == NULL\n");
		dResult = DRM_E_FAIL;
		goto licenseQuery_error;
	}

	dResult = Drm_Initialize(pstAppContext, f_pOEMContext, __macro_pbOpaqueBuffer, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE, &dstrStoreName);
	if(dResult != DRM_SUCCESS)
	{
		HxLOG_Error("#### #### Drm_Initialize is failed! 0x%0X\n",dResult);
		goto licenseQuery_error;
	}

	if(om_drmagent_LocalUnicodeStrstr(unicodedwrmheader, content_length*2, (unsigned char*)ucWrmHeaderUnicodeBegin, sizeof(ucWrmHeaderUnicodeBegin))!=NULL)
	{
		dResult = Drm_Content_SetProperty(pstAppContext, DRM_CSP_V4_HEADER, unicodedwrmheader, content_length*2);
		if(dResult != DRM_SUCCESS)
		{
			HxLOG_Error("#### #### Drm_Content_SetProperty is failed! 0x%0X\n",dResult);
			goto licenseQuery_error;
		}
#if 0
		HxLOG_Info("setting wrmheader = ");
		for(i=0 ; i<length*2 ; ++i)
		{
			HxLOG_Info("%c ",unicodedwrmheader[i]);
		}
		HxLOG_Info("\n\n");
#endif
	}
	apdcsRights_test[0] = (const DRM_CONST_STRING *)&g_dstrWMDRM_RIGHT_PLAYBACK;
	dResult = Drm_LicenseQuery_GetState( pstAppContext, apdcsRights_test, NO_OF( apdcsRights_test ), statedatavales, NULL, NULL );
	if(dResult != DRM_SUCCESS)
	{
			HxLOG_Error("##### ##### Drm_LicenseQuery_GetState is failed! 0x%0X\n",dResult);
			goto licenseQuery_error;
	}
	om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_LICENSEQUERYSTATE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);

	for( dw = 0; dw < NO_OF( apdcsRights_test ); dw++ )
	{
		switch (statedatavales[dw].dwCategory)
		{
			case DRM_LICENSE_STATE_NORIGHT :
			case DRM_LICENSE_STATE_UNLIM :
				{
					if(statedatavales[dw].dwCategory == DRM_LICENSE_STATE_NORIGHT)
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "NORIGHT");
					if (statedatavales[dw].dwCategory == DRM_LICENSE_STATE_UNLIM)
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "UNLIM");
					om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
					if(statedatavales[dw].dwVague==0)
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_VAGUE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "0");
					else if(statedatavales[dw].dwVague==1)
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_VAGUE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "1");
					else
						HxLOG_Error("Not Support\n");
					om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_VAGUE_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
				}
				break;
			case DRM_LICENSE_STATE_FROM :
			case DRM_LICENSE_STATE_UNTIL :
			case DRM_LICENSE_STATE_FROM_UNTIL :
			case DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE :
				{
					if(statedatavales[dw].dwCategory == DRM_LICENSE_STATE_FROM)
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "FROM");
					if (statedatavales[dw].dwCategory == DRM_LICENSE_STATE_UNTIL)
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "UNTIL");
					if (statedatavales[dw].dwCategory == DRM_LICENSE_STATE_FROM_UNTIL)
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "FROM_UNTIL");
					if (statedatavales[dw].dwCategory == DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE)
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "EXPIRATION_AFTER_FIRSTUSE");
					om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_DRM_LICENSE_STATE_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
					for( i = 0; i < statedatavales[dw].dwNumDates; i++ )
					{
						DRMSYSTEMTIME  st;
						HUINT8 datetime[64]={0,};
						Oem_Clock_FileTimeToSystemTime( &(statedatavales[dw].datetime[i]), &st );
						sprintf( datetime, "%04d/%02d/%02d-%02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_DATETIME_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, datetime);
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_DATETIME_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
					}

					if(statedatavales[dw].dwVague==0)
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_VAGUE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "0");
					else if(statedatavales[dw].dwVague==1)
						om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_VAGUE_START, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, "1");
					else
						HxLOG_Error("Not Support\n");
					om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_VAGUE_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);
				}
				break;

			default:
				break;
		}

	}
	om_drmagent_MakeFieldOfDRMResultMsg(pucResultMsg, &resultMsgSize, eMGR_DRMAGENT_XML_TAG_LICENSEQUERYSTATE_END, eMGR_DRMAGENT_XML_ATTRIBUTE_NONE, NULL);

licenseQuery_error:
	if(pContents!=NULL)
		HLIB_STD_MemFree(pContents);
	if(unicodedwrmheader != NULL)
		Oem_MemFree(unicodedwrmheader);
	if(pstAppContext != NULL)
	{
		Drm_Uninitialize(pstAppContext);
		Oem_MemFree(pstAppContext);
	}
	if(__macro_pbOpaqueBuffer != NULL)
		Oem_MemFree(__macro_pbOpaqueBuffer);
	if(f_pOEMContext)
		Oem_Destroy(f_pOEMContext);
	if(dResult == DRM_SUCCESS)
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_SUCCESSFUL;
		eResult = ERR_OK;
	}
	else
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_CANNOT_PROCESS_REQUEST;
		eResult = ERR_FAIL;
	}
	return eResult;
}

STATIC HERROR om_drmagent_ProcessPRDYMessage(HUINT8 * pucDrmMsg, e_MgrDrmAgent_ResultCodeType_t* etResultCode, HUINT8** pucResultMsg, HUINT32 msgSize, e_MgrDrmAgent_PlayReady_MsgType_t* ePRDYMsgType)
{
	HERROR eResult = ERR_OK;
	HINT32 contentsize;

#ifdef PLAY_READY_25
	if(om_drmagent_CheckFileExist("/var/lib/humaxtv/drm/playready25/playready25.bin") != ERR_OK)
	{
		system("rm -rf /var/lib/humaxtv/drm/playready25/*");
		system("mkdir -p /var/lib/humaxtv/drm/playready25");
		system("cp -a /usr/drm/playready25.bin /var/lib/humaxtv/drm/playready25/");
	}
#else
	if(om_drmagent_CheckFileExist("/var/lib/humaxtv/drm/playready.bin") != ERR_OK)
	{
		system("rm -rf /var/lib/humaxtv/drm/*");
		system("mkdir /var/lib/humaxtv/drm");
		system("cp -a /usr/drm/playready.bin /var/lib/humaxtv/drm/");
	}
#endif

	if( HxSTD_StrStr(pucDrmMsg, "PlayReadyInitiator") == NULL)
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT;
		return ERR_FAIL;
	}

	if( HxSTD_StrStr(pucDrmMsg, "LicenseAcquisition") != NULL)
	{
		*ePRDYMsgType = eMGR_DRMAGENT_PLAYREADY_LICENSE_ACQUISITION;
		if(om_drmagent_PRDYLicensePreAcquisition(pucDrmMsg, pucResultMsg, msgSize, etResultCode)!=ERR_OK)
			return ERR_FAIL;
	}
	else if( HxSTD_StrStr(pucDrmMsg, "LicenseQuery") != NULL)
	{
		*ePRDYMsgType = eMGR_DRMAGENT_PLAYREADY_REQUEST_EXISTING_RIGHTS;
		if(om_drmagent_PRDYLicenseStateQuery(pucDrmMsg, pucResultMsg, msgSize, etResultCode)!=ERR_OK)
			return ERR_FAIL;

	}
	else if(HxSTD_StrStr(pucDrmMsg, "LicenseServerUriOverride") != NULL)
	{
		*ePRDYMsgType = eMGR_DRMAGENT_PLAYREADY_LAURL_OVERRIDE;
		if(om_drmagent_GetContentsFromTag(pucDrmMsg, "LA_URL", pucResultMsg, msgSize, &contentsize)!=ERR_OK)
		{
			*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT;
			return ERR_FAIL;
		}
		eResult = ERR_OK;
	}
	else if(HxSTD_StrStr(pucDrmMsg, "SetCustomData") != NULL)
	{
		*ePRDYMsgType = eMGR_DRMAGENT_PLAYREADY_SET_CUSTOMDATA;
 		if(om_drmagent_GetContentsFromTag(pucDrmMsg, "CustomData", pucResultMsg, msgSize, &contentsize)!=ERR_OK)
		{
			*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT;
			return ERR_FAIL;
		}
 		eResult = ERR_OK;
	}
	else
	{
		*etResultCode = eMGR_DRMAGENT_RESULTCODE_WRONG_FORMAT;
		*ePRDYMsgType = eMGR_DRMAGENT_PLAYREADY_NOTSUPPORT;
		HxLOG_Error("Net yet supported \n");
		eResult = ERR_FAIL;
	}
	return eResult;
}
#endif

STATIC void om_drmagent_SendDrmMessage(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HCHAR						*pzMsgType = NULL;
	HCHAR						*pzMsg = NULL;
	HCHAR						*pzDrmSystemId = NULL;
	HUINT16						usMsgId = 0;
	MgrDrmAgentSendDrmMessage_t	stDrmMessage = { { 0, }, };
	e_MgrDrmAgent_SendDRMSystemID_t stSystemID= eMGR_DRMAGENT_SYSTEMID_MAX;
	e_MgrDrmAgent_ResultCodeType_t etResultCode = eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR;
	HUINT8 *szResultElement=NULL;
	HUINT8 *szResultBuffer=NULL;
	DRMAGENT_EVENT_e event;
	e_MgrDrmAgent_SendMsgType_t etMsgType = eMGR_DRMAGENT_MSGTYPE_MAX;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;


	HxLOG_Trace();

	if (argc != 4)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", argc);
		return;
	}

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
	pzMsgType = (HCHAR *)HxOBJECT_STR(argv[0]);
	pzMsg = (HCHAR *)HxOBJECT_STR(argv[1]);
	pzDrmSystemId = (HCHAR *)HxOBJECT_STR(argv[2]);
	usMsgId = (HUINT16)HxOBJECT_INT(argv[3]);

	HxLOG_Debug("pzMsgId: %d \n", usMsgId);

	HxLOG_Debug("pzMsgType: %s \n", pzMsgType);
	HxLOG_Debug("pzMsg: %s \n", pzMsg);
	HxLOG_Debug("pzDrmSystemId: %s \n", pzDrmSystemId);
		//The value of DrmSystemID could be null. so we are not checking DrmSystemID.
		/*
			oipf volume-5-DAE 7.6.1.2 sendDRMMessage Methods.
			In the case that parameter msgType indicates a CSPG-CI+ message as described in section 4.2.3.4.1.1.2 of [OIPF_CSP2] or an embedded CSPG message (see Annex F of [OIPF_CSP2]), the DRMSystemID parameter SHALL be specified.
			Otherwise, the value may be null.
		*/
		om_drmagent_ParseDrmMessageType(pzMsgType, &etMsgType);

		switch(etMsgType) //Current Support : PlayReady, Nagra PRM
		{
			case eMGR_DRMAGENT_MSGTYPE_PRDY :
				{
#ifdef PLAYREADY_SUPPORT

					e_MgrDrmAgent_PlayReady_MsgType_t ePRDYMsgType;
					HERROR	eResult = ERR_FAIL;

					szResultElement = HLIB_STD_MemAlloc(DRMAGENT_DRMMESSAGE_MAX_LEN);
					if(szResultElement == NULL)
					{
						HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", "", usMsgId, etResultCode);
						break;
					}
					szResultBuffer = HLIB_STD_MemAlloc(DRMAGENT_DRMMESSAGE_MAX_LEN);
					if(szResultBuffer == NULL)
					{
						HLIB_STD_MemFree(szResultElement);
						szResultElement = NULL;
						HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", "", usMsgId, etResultCode);
						break;
					}

					HxSTD_MemSet(szResultElement, 0, DRMAGENT_DRMMESSAGE_MAX_LEN);
					HxSTD_MemSet(szResultBuffer, 0, DRMAGENT_DRMMESSAGE_MAX_LEN);

					eResult = om_drmagent_ProcessPRDYMessage(pzMsg, &etResultCode, &szResultElement, DRMAGENT_DRMMESSAGE_MAX_LEN, &ePRDYMsgType);
					if(eResult != ERR_OK)
					{
						HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", "", usMsgId, etResultCode);
					}
					else
					{
						if(ePRDYMsgType == eMGR_DRMAGENT_PLAYREADY_LICENSE_ACQUISITION || ePRDYMsgType == eMGR_DRMAGENT_PLAYREADY_REQUEST_EXISTING_RIGHTS)
						{
						om_drmagent_MakeDRMResultMsg(&szResultBuffer, DRMAGENT_DRMMESSAGE_MAX_LEN, szResultElement);
						if(szResultBuffer != NULL && HxSTD_StrLen(szResultBuffer) != 0)
						HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", szResultBuffer, usMsgId, etResultCode);
						}
						else if(ePRDYMsgType == eMGR_DRMAGENT_PLAYREADY_LAURL_OVERRIDE || ePRDYMsgType == eMGR_DRMAGENT_PLAYREADY_SET_CUSTOMDATA)
						{ // Set LA_URL and Custom Data to om_media_play.
							if(ePRDYMsgType == eMGR_DRMAGENT_PLAYREADY_LAURL_OVERRIDE)
								event = eOEVT_DRMAGENT_SENDDRMMESSAGE_PLAYREADY_LAURL;
							else if(ePRDYMsgType == eMGR_DRMAGENT_PLAYREADY_SET_CUSTOMDATA)
								event = eOEVT_DRMAGENT_SENDDRMMESSAGE_PLAYREADY_CUSTOMDATA;
							else
							{
								HxLOG_Error ("Not Support\n");
								HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", "", usMsgId, etResultCode);
								break;
							}
							HxSTD_StrNCpy((char *)&(stDrmMessage.szDrmMsgType), pzMsgType, (DRMAGENT_DRMMESSAGE_TYPE_MAX_LEN-1));
							HxSTD_StrNCpy((char *)&(stDrmMessage.szDrmMsg), pzMsg, (DRMAGENT_DRMMESSAGE_MAX_LEN-1));
							HxSTD_StrNCpy((char *)&(stDrmMessage.szDrmSystemId), pzDrmSystemId, (DRMAGENT_DRMMESSAGE_SYSTEMID_MAX_LEN-1));

							stSendmsg.stMsg[stSendmsg.usTotalCount].count = NUM_VIEW_ACTION; // In case PRDY
							stSendmsg.stMsg[stSendmsg.usTotalCount].systemID = stSystemID;
							stSendmsg.stMsg[stSendmsg.usTotalCount].usMsgId = usMsgId;
							stSendmsg.usTotalCount++;
							stSendmsg.usTotalCount = (stSendmsg.usTotalCount)%MSG_CACHE;
							(void)BUS_PostData(NULL, event, 0, usMsgId, 0, (void*) &stDrmMessage, sizeof(MgrDrmAgentSendDrmMessage_t));
						}
						else // Not Supported PlayReady Message (Current Support : License Pre-Acquisition, License State Query, LA_URL Override, Set CustomData)
						{
								HxLOG_Error ("Not Support\n");
								HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", "", usMsgId, eMGR_DRMAGENT_RESULTCODE_CANNOT_PROCESS_REQUEST);
						}
					}
#else
					HxLOG_Error ("Not Support\n");
					HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", "", usMsgId, eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR);
#endif
				}
				break;
			case eMGR_DRMAGENT_MSGTYPE_NAPRM_CSPG_HEXBIN:
			case eMGR_DRMAGENT_MSGTYPE_NAPRM_HDPLUSCA_SC:
				{
					HxSTD_StrNCpy((char *)&(stDrmMessage.szDrmMsgType), pzMsgType, (DRMAGENT_DRMMESSAGE_TYPE_MAX_LEN-1));
					HxSTD_StrNCpy((char *)&(stDrmMessage.szDrmMsg), pzMsg, (DRMAGENT_DRMMESSAGE_MAX_LEN-1));
					HxSTD_StrNCpy((char *)&(stDrmMessage.szDrmSystemId), pzDrmSystemId, (DRMAGENT_DRMMESSAGE_SYSTEMID_MAX_LEN-1));

					stSendmsg.stMsg[stSendmsg.usTotalCount].count = 1; // In case NAGRA PRM
					stSendmsg.stMsg[stSendmsg.usTotalCount].systemID = stSystemID;
					stSendmsg.stMsg[stSendmsg.usTotalCount].usMsgId = usMsgId;
					stSendmsg.usTotalCount++;
					stSendmsg.usTotalCount = (stSendmsg.usTotalCount)%MSG_CACHE;
					event = eOEVT_DRMAGENT_SENDDRMMESSAGE_NAGRA_PRM;
					(void)BUS_PostData(NULL, event, 0, usMsgId, 0, (void*) &stDrmMessage, sizeof(MgrDrmAgentSendDrmMessage_t));
				}
				break;
			case eMGR_DRMAGENT_MSGTYPE_VIEWRIGHT_WEB:
				{
					HxLOG_Trace();
					HxSTD_StrNCpy((char *)&(stDrmMessage.szDrmMsgType), pzMsgType, (DRMAGENT_DRMMESSAGE_TYPE_MAX_LEN-1));
					HxSTD_StrNCpy((char *)&(stDrmMessage.szDrmMsg), pzMsg, (DRMAGENT_DRMMESSAGE_MAX_LEN-1));
					HxSTD_StrNCpy((char *)&(stDrmMessage.szDrmSystemId), pzDrmSystemId, (DRMAGENT_DRMMESSAGE_SYSTEMID_MAX_LEN-1));

					stSendmsg.stMsg[stSendmsg.usTotalCount].count = NUM_VIEW_ACTION; // In case NAGRA PRM
					stSendmsg.stMsg[stSendmsg.usTotalCount].systemID = stSystemID;
					stSendmsg.stMsg[stSendmsg.usTotalCount].usMsgId = usMsgId;
					stSendmsg.usTotalCount++;
					stSendmsg.usTotalCount = (stSendmsg.usTotalCount)%MSG_CACHE;
					event = eOEVT_DRMAGENT_SENDDRMMESSAGE_VIEWRIGHT_WEB;
					(void)BUS_PostData(NULL, event, 0, usMsgId, 0, (void*) &stDrmMessage, sizeof(MgrDrmAgentSendDrmMessage_t));
				}
				break;
			default : // Not Supported Message Type (Current Support : PlayReady, Nagra PRM)
				{
					HxLOG_Debug("Not Supported\n");
					etResultCode = eMGR_DRMAGENT_RESULTCODE_UNKNOWN_MIME_TYPE;
					HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", "", usMsgId, etResultCode);
				}
				break;
		}
	}
	if(	szResultElement != NULL)
		HLIB_STD_MemFree(szResultElement);
	if(	szResultBuffer != NULL)
		HLIB_STD_MemFree(szResultBuffer);
	return;
}


// bus task 가 아님을 명심한다. 즉, bus에 message 보내기 위해서는 bus task에게 send/post message 해야함. 절대로 바로 mgr code를 부르지 않는다.
STATIC void om_drmagent_SendDrmSystemStatusRequest(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HCHAR						*pzDrmSystemId = NULL;
	HUINT32						ulSize = 0;

	HxLOG_Trace();

	if (argc != 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", argc);
		return;
	}

	pzDrmSystemId = (HCHAR *)HxOBJECT_STR(argv[0]);
	ulSize = HxSTD_StrLen(pzDrmSystemId);

	if(ulSize == 0)
	{
		HxLOG_Error("ulSize is 0 \n");
		return;
	}

	HxLOG_Debug("pzDrmSystemId: %s \n", pzDrmSystemId);

	(void)BUS_PostData(NULL, eOEVT_DRMAGENT_REQUESTDRMSYSTEMSTAUTS, 0, 0, 0, pzDrmSystemId, ulSize);

	return;

}


#define _____PROC_EVENT_HANDLER_____
STATIC void om_drmagent_DrmMessageResultProcEvent(HUINT16 usMsgId, HUINT32 ulResultCode, const HCHAR *pzResultMsg)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	HUINT16			index;
	HxLOG_Trace();


	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		HxLOG_Debug("usMsgId: %d \n", usMsgId);
		HxLOG_Debug("ulResultCode: %d \n", ulResultCode);
		HxLOG_Debug("pzResultMsg: %s \n", pzResultMsg);

		if(om_drmagent_FindCacheMessageIndex(usMsgId, &index)==ERR_OK)
		{
			stSendmsg.stMsg[index].count--;
			if(stSendmsg.stMsg[index].count == 0)
			{
				HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", pzResultMsg, usMsgId, ulResultCode);
			}
		}
		else
		{
			// resultMsg is not defined.
			HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", "", usMsgId, eMGR_DRMAGENT_RESULTCODE_UKNOWN_ERROR);
		}
	}
	else
	{
		HxLOG_Debug("----OBAMA_RpcProtocol_GetRpcHandle FAIL\n");
	}

}

STATIC void om_drmagent_DrmSystemStatusChangeProcEvent(const HCHAR *pzDrmSystemId)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	HxLOG_Trace();

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		HxLOG_Debug("pzDrmSystemId: %s \n", pzDrmSystemId);
		HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmSystemStatusChanged, "s", pzDrmSystemId);
	}

}



#define _____PROCEDURE_____
STATIC BUS_Result_t proc_drmagent(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("%s created\n", (HINT8*) __FUNCTION__);
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

		case eMEVT_DRMAGENT_COMMON_RESPONSEDRMMESSAGERESULT:
			HxLOG_Debug("eMEVT_DRMAGENT_COMMON_RESPONSEDRMMESSAGERESULT\n");
			om_drmagent_DrmMessageResultProcEvent((HUINT16)p1, (HUINT32)p2, (const HCHAR *)p3);
			break;

		case eMEVT_DRMAGENT_COMMON_RESPONSEDRMSYSTEMSTATUS:
			HxLOG_Debug("eMEVT_DRMAGENT_COMMON_RESPONSEDRMSYSTEMSTATUS\n");
			om_drmagent_DrmSystemStatusChangeProcEvent((const HCHAR *)p3);
			break;

		default:
			break;


	}


	return MESSAGE_PASS;

}


#define ___GLOBAL_FUNCTIONS___

HERROR OM_DRMAGENT_Init(void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;
	HxLOG_Trace();

	if ( VK_SEM_Create(&s_ulOmDrmAgentSemId, "s_ulOmDrmAgentSemId", VK_SUSPENDTYPE_PRIORITY) != VK_OK )
	{
		HxLOG_Error("VK_SEM_Create error! \n");
	}


	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_drmagent, 0, 0, 0, 0);


	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
		//	-------   RPC Methods   -------	//
		hErr = HLIB_RPC_RegisterCall (nRpcHandle, (HCHAR *)RPC_OAPI_DRMAGENT_SendDrmMessage, (HCHAR *)"sssi", (HxRPC_Func_t)om_drmagent_SendDrmMessage, NULL,
								(HCHAR *)"const HCHAR *pzMsgType, const HCHAR *pzMsg, const HCHAR *pzDrmSystemId, HUINT16 usMsgId");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_DRMAGENT_SendDrmMessage, hErr);
		}


		hErr = HLIB_RPC_RegisterCall (nRpcHandle, (HCHAR *)RPC_OAPI_DRMAGENT_SendDrmSystemStautsRequest, (HCHAR *)"s", (HxRPC_Func_t)om_drmagent_SendDrmSystemStatusRequest, NULL,
								(HCHAR *)"const HCHAR *pzDrmSystemId");
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_DRMAGENT_SendDrmSystemStautsRequest, hErr);
		}



		//	-------   RPC Callback proto type   -------	//

		hErr = HLIB_RPC_CreateNotification (nRpcHandle, (HCHAR *)RPC_OBAMA_DRMAGENT_onDrmMessage, "sii",
								(HCHAR *)"const HCHAR *pzResultMsg, HUINT16 usMsgId, HUINT32 ulResultCode\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_DRMAGENT_onDrmMessage, hErr);
		}

		hErr = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmSystemStatusChanged, "s",
								(HCHAR *)"const HCHAR *pzDrmSystemId\n"
								);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_DRMAGENT_onDrmSystemStatusChanged, hErr);
		}

	}

	return ERR_OK;
}




