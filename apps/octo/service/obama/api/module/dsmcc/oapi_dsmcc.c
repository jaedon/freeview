/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_dsmcc.c
	@brief

	Description:  									\n
	Module: OAPI / API					\n

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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	DSMCC_DEFAULT_CACHE_PATH			"/tmp/cache/"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct _oapiDSMCC_AccessInfo
{
	Handle_t				 hDsmcc;
	HCHAR					 szDefaultCachePath[eDxDSMCC_MAX_PATH_LENGTH];
	DxDSMCC_Type_e			 eType;
	Handle_t				 hSession;				// Session
	DxDSMCC_CarouselList_t	 stCarouselList;		// Carousel List
	DxDSMCC_Event_Info_t	 stEventInfo;			// Event
	HxList_t				*pstStreamEvent;		// DxDSMCC_StreamEvent_t
	HxList_t				*pstHandler;			// Event Listener

	// AIT Info
	OxDsmcc_AitSectionData_t	 stAitSectionData;
	HUINT8						*pucTotalAitBinary;
} oapiDsmcc_Info_t;

typedef struct _DSMCC_ListMap
{
	HxList_t				*pstDsmccList;		// oapiDsmcc_Info_t
} oapiDsmcc_Map_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC oapiDsmcc_Map_t	s_stOapiDsmccMap = { NULL, };
STATIC Handle_t 		s_hTempDsmcc ;
STATIC HCHAR			s_stDsmccDefaultCachePath[eDxDSMCC_MAX_PATH_LENGTH];

// async noti 를 위해서.
STATIC OAPI_DSMCC_NotifilerForAppkit_t	s_fnDsmccApkNotifier;

/*******************************************************************/
/********************      local Functions	********************************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___

#if defined(CONFIG_DEBUG)
STATIC const HINT8 * oapi_dsmcc_MakeStrEventType(DxDSMCC_Event_e eEvtType)
{
	switch (eEvtType)
	{
		ENUM_TO_STR(eDxDSMCC_EVENT_Created);
		ENUM_TO_STR(eDxDSMCC_EVENT_Started);
		ENUM_TO_STR(eDxDSMCC_EVENT_Stopped);
		ENUM_TO_STR(eDxDSMCC_EVENT_Progress);
		ENUM_TO_STR(eDxDSMCC_EVENT_Changed);
		ENUM_TO_STR(eDxDSMCC_EVENT_Fail);
		ENUM_TO_STR(eDxDSMCC_EVENT_Success);
		ENUM_TO_STR(eDxDSMCC_EVENT_StreamEvtFound);
		ENUM_TO_STR(eDxDSMCC_EVENT_StreamDescriptorEvt);
		ENUM_TO_STR(eDxDSMCC_EVENT_StreamEvtList);
		ENUM_TO_STR(eDxDSMCC_EVENT_CarouselList);
		ENUM_TO_STR(eDxDSMCC_EVENT_GeneralError);
		ENUM_TO_STR(eDxDSMCC_EVENT_Unknown);
		default:
			break;
	}
	return ("Unknown");
}

STATIC void 	oapi_dsmcc_CarouselListPrint(DxDSMCC_CarouselList_t *pstCarouselList, HCHAR *pszPreFix)
{
	if (pstCarouselList)
	{
		HUINT32 ulCount, ulIdx, ulNumEs;
		DxDSMCC_StreamInfo_t	*pstEsInfo;

		if (NULL == pszPreFix)
			pszPreFix = "";

		HxLOG_Debug("%s ======================================================================\n", pszPreFix);
		HxLOG_Debug("%s Carousel Num : [%d], Addr [%p] \n", pszPreFix, pstCarouselList->ulNum, pstCarouselList);
		HxLOG_Debug("%s ======================================================================\n", pszPreFix);
		if(pstCarouselList->pstCarouselInfo != NULL)
		{
			for (ulCount=0; ulCount<pstCarouselList->ulNum; ulCount++)
			{
				HxLOG_Debug("%s [%02d] : CarouselId [0x%08x], ES Num [%d]\n", pszPreFix, ulCount, pstCarouselList->pstCarouselInfo[ulCount].ulCarouselId, pstCarouselList->pstCarouselInfo[ulCount].ulNum);
				HxLOG_Debug("%s ----------------------------------------------------------------------\n", pszPreFix);
				ulNumEs = pstCarouselList->pstCarouselInfo[ulCount].ulNum;
				for(ulIdx=0; ulIdx<ulNumEs; ulIdx++)
				{
					pstEsInfo = &pstCarouselList->pstCarouselInfo[ulCount].astEsInfo[ulIdx];
					if (ulIdx)
						HxLOG_Debug("%s ----------------------------------------------------------------------\n", pszPreFix);

					HxLOG_Debug("%s\t ES[%02d] : CompTag [0x%02x], StreamType [0x%02x]\n", pszPreFix, ulIdx,
																		pstEsInfo->ucComponentTag, pstEsInfo->ucStreamType);
				}
				HxLOG_Debug("%s ----------------------------------------------------------------------\n", pszPreFix);
			}
		}
		HxLOG_Debug("%s ======================================================================\n", pszPreFix);
	}
}

STATIC void oapi_dsmcc_stream_event_Print(DxDSMCC_StreamEvent_t *pstEvent, HCHAR *pszPrefix)
{
	if (pstEvent)
	{
		if (NULL == pszPrefix)
			pszPrefix = "\t[StreamEvent]";
		HxLOG_Debug("==========================================\n");
		HxLOG_Debug("%s EId   : [0x%04x]\n", pszPrefix, pstEvent->nEventId);
		HxLOG_Debug("%s PID   : [0x%04x]\n", pszPrefix, pstEvent->usPid);
		HxLOG_Debug("%s A Tag : [0x%04x]\n", pszPrefix, pstEvent->usTag);

		HxLOG_Debug("%s URL   : [%s]\n", pszPrefix, pstEvent->pszUrl);
		HxLOG_Debug("%s EName : [%s]\n", pszPrefix, pstEvent->szEventName ? (HCHAR *)pstEvent->szEventName : "Null");
		HxLOG_Debug("%s DESC  : [%s]\n", pszPrefix, pstEvent->szDescription ? (HCHAR *)pstEvent->szDescription : "Null");
		HxLOG_Debug("%s OINFO : [%s]\n", pszPrefix, pstEvent->pucObjInfoByte ? (HCHAR *)pstEvent->pucObjInfoByte : "Null");
		HxLOG_Debug("==========================================\n");
	}
}

#endif

STATIC void oapi_dsmcc_HandleCreated(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HxLOG_Debug("\n");

	HxSTD_memset(&s_stDsmccDefaultCachePath[0], 0x00, sizeof(HCHAR)* eDxDSMCC_MAX_PATH_LENGTH);

	s_hTempDsmcc = (Handle_t)HxOBJECT_INT(argv[0]) ;

	HLIB_STD_StrNCpySafe(&s_stDsmccDefaultCachePath[0], DSMCC_DEFAULT_CACHE_PATH, eDxDSMCC_MAX_PATH_LENGTH);

	HxLOG_Debug("s_hTempDsmcc (0x%x) \n", s_hTempDsmcc);

	return;
}



STATIC HBOOL	oapi_dsmcc_CbFindDsmccInfoByHandle(void *pvUserData, void *pvListData)
{
	oapiDsmcc_Info_t	*pstListInfo;
	Handle_t			 hDsmcc;

	if (NULL == pvListData)
		return FALSE;

	hDsmcc = (Handle_t)pvUserData;
	pstListInfo = (oapiDsmcc_Info_t *)pvListData;

	return (hDsmcc == pstListInfo->hDsmcc) ? TRUE : FALSE;
}

STATIC HBOOL		oapi_dsmcc_FindDsmccInfoByHandle(Handle_t hDsmcc, oapiDsmcc_Info_t **ppstInfo)
{
	HxList_t				*pstFound;

	pstFound = HLIB_LIST_FindEx(s_stOapiDsmccMap.pstDsmccList, (const void *)hDsmcc, oapi_dsmcc_CbFindDsmccInfoByHandle);

	if (ppstInfo)
	{
		*ppstInfo = (oapiDsmcc_Info_t *)HLIB_LIST_Data(pstFound);
	}

	return (pstFound) ? TRUE : FALSE;
}

STATIC HERROR oapi_dsmcc_RemoveDsmccInfo(Handle_t hDsmcc, oapiDsmcc_Info_t **ppstInfo)
{
	HERROR hErr = ERR_OK;
	oapiDsmcc_Info_t	*pstDsmccInfo = NULL;

	hErr = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if (NULL == pstDsmccInfo)
	{
		HxLOG_Error("NULL == pstDsmccInfo \n");
		return ERR_FAIL;
	}

	s_stOapiDsmccMap.pstDsmccList = HLIB_LIST_Remove(s_stOapiDsmccMap.pstDsmccList, (const void *)pstDsmccInfo);
	if (ppstInfo)
	{
		*ppstInfo = pstDsmccInfo;
	}

	return ERR_OK;
}

STATIC void	oapi_dsmcc_StreamEventClear(DxDSMCC_StreamEvent_t *pstStreamEvent)
{
	if (pstStreamEvent)
	{
		if (pstStreamEvent->pszUrl)			HLIB_STD_MemFree(pstStreamEvent->pszUrl);
		if (pstStreamEvent->szDescription)	HLIB_STD_MemFree(pstStreamEvent->szDescription);
		if (pstStreamEvent->szEventName)	HLIB_STD_MemFree(pstStreamEvent->szEventName);
		if (pstStreamEvent->pucObjInfoByte)	HLIB_STD_MemFree(pstStreamEvent->pucObjInfoByte);
	}
}

STATIC void	oapi_dsmcc_StreamEventFree(void *pvData)
{
	DxDSMCC_StreamEvent_t *pstStreamEvent = (DxDSMCC_StreamEvent_t *)pvData;

	if (pstStreamEvent)
	{
		oapi_dsmcc_StreamEventClear(pstStreamEvent);
		HLIB_STD_MemFree(pstStreamEvent);
	}
}

STATIC void	oapi_dsmcc_StreamEventListFree(HxList_t **ppstStreamEventList)
{
	HxList_t					*pstList;

	HxLOG_Assert(ppstStreamEventList);

	pstList = HLIB_LIST_First(*ppstStreamEventList);
	HLIB_LIST_Foreach(pstList, oapi_dsmcc_StreamEventFree);

	*ppstStreamEventList = HLIB_LIST_RemoveAll(pstList);
}

STATIC void oapi_dsmcc_CarouselListFree(DxDSMCC_CarouselList_t *pstList)
{
	HUINT32 	ulCount;
	if (pstList)
	{
		if (pstList->pstCarouselInfo)
		{
			for (ulCount=0; ulCount<pstList->ulNum; ulCount++)
			{
				if (pstList->pstCarouselInfo[ulCount].astEsInfo)
				{
					HxSTD_MemSet(pstList->pstCarouselInfo[ulCount].astEsInfo, 0x00, sizeof(DxDSMCC_StreamInfo_t) *eDxDSMCC_MAX_ES_STREAM_NUM );
				}
			}
			HLIB_STD_MemFree(pstList->pstCarouselInfo);
			pstList->pstCarouselInfo = NULL;
		}
		HxSTD_MemSet(pstList, 0, sizeof(DxDSMCC_CarouselList_t));
	}
}

STATIC void	oapi_dsmcc_UnregisterNotifier(oapiDsmcc_Info_t *pstDsmccInfo)
{
	if (NULL == pstDsmccInfo)
	{
		HxLOG_Error("NULL == pstDsmccInfo \n");
		return;
	}

	// Remove Event Handler
	pstDsmccInfo->pstHandler = HLIB_LIST_RemoveAll(pstDsmccInfo->pstHandler);
	pstDsmccInfo->pstHandler = NULL;
}

STATIC void	oapi_dsmcc_UnregisterNotifierAll(HxList_t *pstDsmccList)
{
	HxList_t			*pstList;
	oapiDsmcc_Info_t	*pstInfo;

	pstList = HLIB_LIST_First(pstDsmccList);
	while(pstList)
	{
		pstInfo = (oapiDsmcc_Info_t *)HLIB_LIST_Data(pstList);
		if (pstInfo)
		{
			oapi_dsmcc_UnregisterNotifier(pstInfo);
			HLIB_STD_MemFree(pstInfo);
		}
		pstList = pstList->next;
	}
}

STATIC HERROR	oapi_dsmcc_RegisterNotifierToList(HxList_t **ppstHandler, OAPI_DSMCC_Notifiler_t  pfnObjectHandler)
{
	HxList_t	*list;

	list = HLIB_LIST_Find(*ppstHandler, pfnObjectHandler);
	if (list)
	{
		HxLOG_Error("pfnObjectHandler(%X) is already registered!!!\n", pfnObjectHandler);
		return ERR_FAIL;
	}

	*ppstHandler = HLIB_LIST_Append(*ppstHandler, (void*)pfnObjectHandler);

	return ERR_OK;
}

STATIC HERROR 	oapi_dsmcc_UnregisterNotifierFromList(HxList_t **ppstHandler, OAPI_DSMCC_Notifiler_t pfnObjectHandler)
{
	HxList_t	*list;

	list = HLIB_LIST_Find(*ppstHandler, pfnObjectHandler);
	if (list == NULL)
	{
		HxLOG_Error("pfnObjectHandler(%X) does not registered yet..\n", pfnObjectHandler);
		return ERR_FAIL;
	}

	*ppstHandler = HLIB_LIST_Remove(*ppstHandler, pfnObjectHandler);

	return ERR_OK;
}

STATIC void	oapi_dsmcc_freeEventCb(HINT32 nArgc, void *apArgV[])
{
	void	*pvData;


	HxLOG_Assert(nArgc == 4);

	pvData = apArgV[3];

	if (pvData)
	{
		HLIB_STD_MemFree(pvData);
		pvData = NULL;
	}

}

STATIC void oapi_dsmcc_ExecuteHandler(HxList_t *pstHandler, Handle_t hDsmcc, DxDSMCC_Event_e eEvent, DxDSMCC_EventParam_t *pstParam)
{
	HxList_t			*pstList;
	OAPI_DSMCC_Notifiler_t 	pfnObjectHandler;
	DxDSMCC_EventParam_t *pstDsmccEventParam;


	pstList = HLIB_LIST_First(pstHandler);
	while (pstList)
	{
		pfnObjectHandler = (OAPI_DSMCC_Notifiler_t)HLIB_LIST_Data(pstList);
		if (pfnObjectHandler)
		{
			if(s_fnDsmccApkNotifier != NULL)
			{
				pstDsmccEventParam = (DxDSMCC_EventParam_t *)HLIB_STD_MemCalloc(sizeof(DxDSMCC_EventParam_t));
				if(pstDsmccEventParam == NULL)
				{
					HxLOG_Error("alloc fail ! \n");
					continue;
				}
				if(pstParam != NULL)
				{
					HxSTD_MemCopy(pstDsmccEventParam, pstParam, sizeof(DxDSMCC_EventParam_t));
				}
				s_fnDsmccApkNotifier(pfnObjectHandler, oapi_dsmcc_freeEventCb, hDsmcc, eEvent, (HUINT32)pstDsmccEventParam);
			}
		}
		pstList = pstList->next;
	}
}

STATIC HERROR	oapi_dsmcc_AddInfo(Handle_t hDsmccHandle, HCHAR *pstPath)
{
	oapiDsmcc_Info_t *pstDsmcctInfo;

	HxLOG_Debug("[%s][%04d] (+) (hDsmccHandle : 0x%x) \n",  __FUNCTION__, __LINE__, hDsmccHandle);

	pstDsmcctInfo = HLIB_STD_MemCalloc(sizeof(oapiDsmcc_Info_t));
	if (NULL == pstDsmcctInfo)
	{
		HxLOG_Error("NULL == pstDsmcctInfo \n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstDsmcctInfo, 0, sizeof(oapiDsmcc_Info_t));

	pstDsmcctInfo->hDsmcc = hDsmccHandle;
	HLIB_STD_StrNCpySafe(&pstDsmcctInfo->szDefaultCachePath[0], pstPath, eDxDSMCC_MAX_PATH_LENGTH);

	s_stOapiDsmccMap.pstDsmccList = HLIB_LIST_Append(s_stOapiDsmccMap.pstDsmccList, (void *)pstDsmcctInfo);

	HxLOG_Debug("[%s][%04d] (-) (hDsmccHandle : 0x%x) \n",  __FUNCTION__, __LINE__, hDsmccHandle);

	return ERR_OK;
}

STATIC HERROR oapi_dsmcc_stream_event_list_Append(HxList_t **pstStreamEventList, DxDSMCC_StreamEvent_t *pstStreamEvent)
{
	DxDSMCC_StreamEvent_t	*pstStreamEventDst = NULL;

	if(pstStreamEvent == NULL)
	{
		HxLOG_Error("pstStreamEvent is NULL \n");
		return ERR_FAIL;
	}

	pstStreamEventDst = (DxDSMCC_StreamEvent_t *)HLIB_STD_MemCalloc(sizeof(DxDSMCC_StreamEvent_t));
	if (NULL == pstStreamEventDst)
	{
		HxLOG_Error("NULL == pstStreamEvent \n");
		return ERR_FAIL;
	}

	// Add Stream Event List to Session Map
	HxSTD_memcpy(pstStreamEventDst, pstStreamEvent, sizeof(DxDSMCC_StreamEvent_t));

	*pstStreamEventList = HLIB_LIST_Append(*pstStreamEventList, (void *)pstStreamEventDst);
	(void)pstStreamEventList; /* fix for prevent #174760 */
	// pstStreamEventList 는 oapi_dsmcc_StreamEventListFree 에서 free하게 됨. 여기서 free 하지 마세요.

	return ERR_OK;
}
STATIC HERROR	oapi_dsmcc_util_decodeBase64(const HCHAR *pszSrc, HUINT32 ulSrcLenght, HCHAR **ppszDest,HUINT32  *ulDecodeLength)
{
	HCHAR	*pszDest = NULL;
	HUINT32  ulDestLength;

	if (NULL == pszSrc || NULL == ppszDest)
	{
		return ERR_FAIL;
	}

	if (0 == ulSrcLenght)
	{
		return ERR_FAIL;
	}
	ulDestLength = 0;
	pszDest = NULL;

	ulDestLength = HLIB_MATH_DecodeBase64(NULL, (const HCHAR *)pszSrc, ulSrcLenght);

	if (ulDestLength)
	{
		pszDest = HLIB_STD_MemCalloc(ulDestLength+1);
		if (pszDest)
		{
			HLIB_MATH_DecodeBase64(pszDest, (const HCHAR *)pszSrc, ulSrcLenght);
			pszDest[ulDestLength] = '\0';
		}
	}

	*ppszDest = pszDest;

	if(ulDestLength)
		*ulDecodeLength = ulDestLength;
	else
		*ulDecodeLength = 0;

	return ERR_OK;
}
STATIC HERROR oapi_dsmcc_stream_event_Convert(DxDSMCC_StreamEvent_t *pstDest)
{
	HCHAR	*pstSrc = NULL;
	HUINT32	 ulSrcLen;
	HUINT32  ulDecodeLength = 0;

	HxLOG_Assert(pstDest);

	if (pstDest->pszUrl)
	{
		pstSrc = OxMEM_StrDup(pstDest->pszUrl);
		HLIB_STD_MemFree(pstDest->pszUrl);
		pstDest->pszUrl = NULL;
		oapi_dsmcc_util_decodeBase64((const HCHAR *)pstSrc, HxSTD_StrLen(pstSrc), (HCHAR **)&pstDest->pszUrl,&ulDecodeLength);
		HLIB_STD_MemFree(pstSrc);
		pstSrc = NULL;
	}
	if (pstDest->szDescription && pstDest->ulDescLen)
	{
		pstSrc = OxMEM_MemDup(pstDest->szDescription,pstDest->ulDescLen);
		ulSrcLen = pstDest->ulDescLen;
		HLIB_STD_MemFree(pstDest->szDescription);
		pstDest->szDescription = NULL;
		oapi_dsmcc_util_decodeBase64((const HCHAR *)pstSrc, ulSrcLen, (HCHAR **)&pstDest->szDescription,&ulDecodeLength);
		pstDest->ulDescLen = ulDecodeLength;
		HLIB_STD_MemFree(pstSrc);
		pstSrc = NULL;
	}
	if (pstDest->szEventName)
	{
		pstSrc = OxMEM_StrDup(pstDest->szEventName);
		HLIB_STD_MemFree(pstDest->szEventName);
		pstDest->szEventName = NULL;
		oapi_dsmcc_util_decodeBase64((const HCHAR *)pstSrc, HxSTD_StrLen(pstSrc), (HCHAR **)&pstDest->szEventName,&ulDecodeLength);
		HLIB_STD_MemFree(pstSrc);
		pstSrc = NULL;
	}
	if (pstDest->pucObjInfoByte && pstDest->ulObjInfoByteLen)
	{
		pstSrc = OxMEM_MemDup(pstDest->pucObjInfoByte,pstDest->ulObjInfoByteLen);
		ulSrcLen = pstDest->ulObjInfoByteLen;
		HLIB_STD_MemFree(pstDest->pucObjInfoByte);
		pstDest->pucObjInfoByte = NULL;
		oapi_dsmcc_util_decodeBase64((const HCHAR *)pstSrc, ulSrcLen, (HCHAR **)&pstDest->pucObjInfoByte,&ulDecodeLength);
		pstDest->ulObjInfoByteLen = ulDecodeLength;
		HLIB_STD_MemFree(pstSrc);
		pstSrc = NULL;
	}
	return ERR_OK;

}

STATIC HERROR oapi_dsmcc_stream_event_UnSerializeData(DxDSMCC_StreamEvent_t *pstStreamEvent, HxObject_t *pstObject, HUINT32 *ulArrLen)
{
	HUINT32 ulformat = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;

	HxLOG_Debug("ulformat: 0x%x \n");

	switch(ulformat)
	{
		case 15: // url + eventname + description + objectinfo

			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szDescription =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->szEventName =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->pucObjInfoByte =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 7)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 7)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 8)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 9)->u.num;

			*ulArrLen = 9;
			break;
		case 14: // eventname + description + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szDescription =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->szEventName =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->pucObjInfoByte =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 8)->u.num;

			*ulArrLen = 8;
			break;
		case 13:// url + description + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szDescription =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->pucObjInfoByte =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 8)->u.num;

			*ulArrLen = 8;
			break;
		case 12:// description + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szDescription =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->pucObjInfoByte =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			*ulArrLen = 7;
			break;
		case 11:// url + eventname + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szEventName =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->pucObjInfoByte =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 8)->u.num;

			*ulArrLen = 8;
			break;

		case 10:// eventname + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szEventName =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->pucObjInfoByte =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			*ulArrLen = 7;
			break;

		case 9:// url + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->pucObjInfoByte =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			*ulArrLen = 7;
			break;

		case 8:// objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->pucObjInfoByte =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;

			*ulArrLen = 6;
			break;

		case 7:// url + eventname + description
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szDescription =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->szEventName =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 8)->u.num;

			*ulArrLen = 8;
			break;

		case 6:// eventname + description
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szDescription =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->szEventName =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			*ulArrLen = 7;
			break;

		case 5:// url + description
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szDescription =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			*ulArrLen = 7;
			break;

		case 4://  description
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szDescription =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;

			*ulArrLen = 6;
			break;

		case 3:// url + eventname
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szEventName =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			*ulArrLen = 7;
			break;

		case 2:// eventname
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szEventName =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;

			*ulArrLen = 6;
			break;

		case 1:// url
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =OxMEM_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;

			*ulArrLen = 6;
			break;

		case 0:
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;

			*ulArrLen = 5;
			break;

	}
	return ERR_OK;
}

#define _____NOTIFICATION_CALLBACKS_____
STATIC void oapi_dsmcc_EventNotifier(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	// notify 받은 param
	DxDSMCC_Event_e			eDsmccEvent;
	DxDSMCC_Event_Info_t	stDsmccEventInfo;
	HCHAR 					*pActionName ;
	Handle_t 				hDsmccHandle = HANDLE_NULL;
	HxObject_t				*pstObject = NULL, *pstObjDesc = NULL;

	// 내부에서 사용하는 param
	oapiDsmcc_Info_t		*pstDsmccInfo = NULL;
	HBOOL					bResult = FALSE;

	if(argv == NULL)
	{
		HxLOG_Error("argv is NULL \n");
		goto END_FUNC;
	}

	pActionName 	= HxOBJECT_STR(argv[0]);
	hDsmccHandle 	= (Handle_t) HxOBJECT_INT(argv[1]);
	eDsmccEvent 	= (DxDSMCC_Event_e)HxOBJECT_INT(argv[2]);

	HxLOG_Debug("hDsmccHandle(0x%08x) pActionName(%s) eDsmccEvent(%d) \n", hDsmccHandle, pActionName, eDsmccEvent);

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmccHandle, &pstDsmccInfo);
	if(bResult == FALSE || pstDsmccInfo == NULL)
	{
		HxLOG_Error("hDsmccHandle(0x%08x) bResult(%d) pDsmccInfo(%p)\n", hDsmccHandle, bResult, pstDsmccInfo);
		goto END_FUNC;
	}

	HxSTD_memset(&stDsmccEventInfo, 0x00, sizeof(DxDSMCC_Event_Info_t));

	switch(eDsmccEvent)
	{
	case eDxDSMCC_EVENT_Created:
		{
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s \n", oapi_dsmcc_MakeStrEventType(eDsmccEvent));
#endif
		}
		break;
	case eDxDSMCC_EVENT_Started:
		{
			HUINT32 	ulCarouselId;

			ulCarouselId	   = HxOBJECT_INT(argv[3]);
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s \n", oapi_dsmcc_MakeStrEventType(eDsmccEvent));
#endif
		}
		break;
	case eDxDSMCC_EVENT_Stopped:
		{
			HUINT32 	ulCarouselId;

			ulCarouselId	   = HxOBJECT_INT(argv[3]);
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s \n", oapi_dsmcc_MakeStrEventType(eDsmccEvent));
#endif
		}
		break;
	case eDxDSMCC_EVENT_Progress:
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s \n", oapi_dsmcc_MakeStrEventType(eDsmccEvent));
#endif
		break;
	case eDxDSMCC_EVENT_Changed:
		{
			HUINT32 				ulCarouselId;

			ulCarouselId	   = HxOBJECT_INT(argv[3]);
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s :: ulCarouselId(0x%x)\n", oapi_dsmcc_MakeStrEventType(eDsmccEvent), ulCarouselId);
#endif
			stDsmccEventInfo.hDsmcc = hDsmccHandle;
			stDsmccEventInfo.eEvent = eDsmccEvent;
			stDsmccEventInfo.unParam.stDownload.ulParam1 = ulCarouselId;
			oapi_dsmcc_ExecuteHandler(pstDsmccInfo->pstHandler, (HUINT32)stDsmccEventInfo.hDsmcc , (HUINT32)stDsmccEventInfo.eEvent, &stDsmccEventInfo.unParam);
		}
		break;
	case eDxDSMCC_EVENT_Fail:
		{
			HUINT32				ulCarouselId;

			ulCarouselId 	   = HxOBJECT_INT(argv[3]);
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s :: ulCarouselId(0x%x) \n", oapi_dsmcc_MakeStrEventType(eDsmccEvent), ulCarouselId);
#endif
			stDsmccEventInfo.hDsmcc = hDsmccHandle;
			stDsmccEventInfo.eEvent = eDsmccEvent;
			stDsmccEventInfo.unParam.stDownload.ulParam1 = ulCarouselId;
			oapi_dsmcc_ExecuteHandler(pstDsmccInfo->pstHandler, (HUINT32)stDsmccEventInfo.hDsmcc , (HUINT32)stDsmccEventInfo.eEvent, &stDsmccEventInfo.unParam);
		}
		break;
	case eDxDSMCC_EVENT_Success:
		{
			HUINT32				ulCarouselId;
			HBOOL				hRst = TRUE;

			ulCarouselId 	   = HxOBJECT_INT(argv[3]);

			hRst = oapi_dsmcc_FindDsmccInfoByHandle(hDsmccHandle,&pstDsmccInfo);

#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s :: ulCarouselId(0x%x) \n", oapi_dsmcc_MakeStrEventType(eDsmccEvent), ulCarouselId);
#endif
			stDsmccEventInfo.hDsmcc = hDsmccHandle;
			stDsmccEventInfo.eEvent = eDsmccEvent;
			stDsmccEventInfo.unParam.stDownload.ulParam1 = ulCarouselId;

			oapi_dsmcc_ExecuteHandler(pstDsmccInfo->pstHandler, (HUINT32)stDsmccEventInfo.hDsmcc , (HUINT32)stDsmccEventInfo.eEvent, &stDsmccEventInfo.unParam);
		}
		break;
	case eDxDSMCC_EVENT_StreamEvtFound:
		{
			HUINT32 		ulStremEventNum;
			HxList_t		*pstStreamEventList;

			 ulStremEventNum		= HxOBJECT_INT(argv[3]);
			 pstStreamEventList 	= (HxList_t *)HxOBJECT_INT(argv[4]);

#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s :: ulStremEventNum(%d) pstStreamEventList(%p)\n", oapi_dsmcc_MakeStrEventType(eDsmccEvent), ulStremEventNum, pstStreamEventList);
#endif
			stDsmccEventInfo.hDsmcc = hDsmccHandle;
			stDsmccEventInfo.eEvent = eDsmccEvent;
			stDsmccEventInfo.unParam.stStreamEvent.pstEventList = pstStreamEventList;

			oapi_dsmcc_ExecuteHandler(pstDsmccInfo->pstHandler, (HUINT32)stDsmccEventInfo.hDsmcc , (HUINT32)stDsmccEventInfo.eEvent, &stDsmccEventInfo.unParam);
		}
		break;
	case eDxDSMCC_EVENT_StreamDescriptorEvt:
		{
			HUINT32 		ulStremEventNum;
			void			*pvStreamDescEvt = NULL;
			HUINT32		ulSize;
			DxDSMCC_StreamEvent_t		*pstStreamEventData = NULL;
			HxList_t		*pstStreamEventList = NULL;
			HUINT32		ulCnt = 0;
			HUINT32 	ulArrLen = 0;


			ulStremEventNum		= HxOBJECT_INT(argv[3]);

			pvStreamDescEvt	= (void *)HxOBJECT_BIN_DATA (argv[4]);
			ulSize			= (HUINT32 )HxOBJECT_BIN_SIZE (argv[4]);

			pstObject = HLIB_SERIALIZER_Unpack(pvStreamDescEvt, ulSize);
			if (pstObject == NULL)
			{
				HxLOG_Error("cannot unpack the data \n");
				goto END_FUNC;
			}

			if (HxOBJECT_ARR_LEN(pstObject) <= 4)
			{
				HxLOG_Error("invalid object arr len. \n");
				goto END_FUNC;
			}

			pstStreamEventData = HLIB_STD_MemCalloc(ulStremEventNum* sizeof(DxDSMCC_StreamEvent_t));
			if(pstStreamEventData)
			{
				HxSTD_memset(pstStreamEventData, 0x00, ulStremEventNum* sizeof(DxDSMCC_StreamEvent_t));
				for(ulCnt = 0; ulCnt < ulStremEventNum; ulCnt++)
				{
					oapi_dsmcc_stream_event_UnSerializeData(&pstStreamEventData[ulCnt],pstObject,&ulArrLen);

					if( (ulCnt < ulStremEventNum - 1) )//&& (HxOBJECT_ARR_LEN(pstObject) == ulArrLen+1))
					{
						pvStreamDescEvt = (void *)HxOBJECT_ARR_VAL(pstObject, ulArrLen+1)->u.bin.data;
						ulSize = (HUINT32)HxOBJECT_ARR_VAL(pstObject, ulArrLen+1)->u.bin.size;

						pstObjDesc = HLIB_SERIALIZER_Unpack(pvStreamDescEvt, ulSize);
						if (pstObjDesc == NULL)
						{
							if(pstStreamEventData)
							{
								HLIB_STD_MemFree(pstStreamEventData);
								pstStreamEventData = NULL;
							}
							HxLOG_Error("cannot unpack the data \n");
							goto END_FUNC;
						}
					}

				}

			}

			for(ulCnt = 0; ulCnt < ulStremEventNum; ulCnt++)
			{
				HxLOG_Debug("convert %d th item \n", ulCnt);
				oapi_dsmcc_stream_event_Convert(&(pstStreamEventData[ulCnt]));
#if defined(CONFIG_DEBUG)
				oapi_dsmcc_stream_event_Print(&(pstStreamEventData[ulCnt]), "\t[StreamEvent:EventProc]");
#endif
				oapi_dsmcc_stream_event_list_Append(&pstStreamEventList, &(pstStreamEventData[ulCnt]));
			}


			if(pstStreamEventData)
			{
				HLIB_STD_MemFree(pstStreamEventData);
				pstStreamEventData = NULL;
			}

			stDsmccEventInfo.hDsmcc = hDsmccHandle;
			stDsmccEventInfo.eEvent = eDsmccEvent;
			stDsmccEventInfo.unParam.stStreamEvent.pstEventList = pstStreamEventList;

			pstDsmccInfo->pstStreamEvent = pstStreamEventList;

			oapi_dsmcc_ExecuteHandler(pstDsmccInfo->pstHandler, (HUINT32)stDsmccEventInfo.hDsmcc , (HUINT32)stDsmccEventInfo.eEvent, &stDsmccEventInfo.unParam);
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s :: ulStremEventNum(%d) pstStreamEventList(%p)\n", oapi_dsmcc_MakeStrEventType(eDsmccEvent), ulStremEventNum, pstStreamEventList);
#endif
		}
		break;
	case eDxDSMCC_EVENT_StreamEvtList:
		{
			HUINT32		ulStremEventNum;
			void			*pvStreamDescEvt = NULL;
			HUINT32		ulSize;
			DxDSMCC_StreamEvent_t		*pstStreamEventData = NULL;
			HxList_t		*pstStreamEventList = NULL;
			HUINT32		ulCnt = 0;
			HUINT32 	ulArrLen = 0;

			 ulStremEventNum 		= HxOBJECT_INT(argv[3]);

			pvStreamDescEvt	= (void *)HxOBJECT_BIN_DATA (argv[4]);
			ulSize			= (HUINT32 )HxOBJECT_BIN_SIZE (argv[4]);

			pstObject = HLIB_SERIALIZER_Unpack(pvStreamDescEvt, ulSize);
			if (pstObject == NULL)
			{
				HxLOG_Error("cannot unpack the data \n");
				goto END_FUNC;
			}

			if(HxOBJECT_ARR_LEN(pstObject)  <= 4)
			{
				HxLOG_Error("invalid object arr len. \n");
				goto END_FUNC;
			}

			pstStreamEventData = HLIB_STD_MemCalloc(ulStremEventNum* sizeof(DxDSMCC_StreamEvent_t));
			if(pstStreamEventData)
			{
				HxSTD_memset(pstStreamEventData, 0x00, ulStremEventNum* sizeof(DxDSMCC_StreamEvent_t));
				for(ulCnt = 0; ulCnt < ulStremEventNum; ulCnt++)
				{
					oapi_dsmcc_stream_event_UnSerializeData(&pstStreamEventData[ulCnt],pstObject,&ulArrLen);

					if( (ulCnt < ulStremEventNum - 1) )//&& (HxOBJECT_ARR_LEN(pstObject) == ulArrLen+1))
					{
						pvStreamDescEvt = (void *)HxOBJECT_ARR_VAL(pstObject, ulArrLen+1)->u.bin.data;
						ulSize = (HUINT32)HxOBJECT_ARR_VAL(pstObject, ulArrLen+1)->u.bin.size;

						pstObjDesc = HLIB_SERIALIZER_Unpack(pvStreamDescEvt, ulSize);
						if (pstObjDesc == NULL)
						{
							if(pstStreamEventData)
							{
								HLIB_STD_MemFree(pstStreamEventData);
								pstStreamEventData = NULL;
							}
							HxLOG_Error("cannot unpack the data \n");
							goto END_FUNC;
						}
					}

				}

			}

			for(ulCnt = 0; ulCnt < ulStremEventNum; ulCnt++)
			{
				HxLOG_Debug("convert %d th item \n", ulCnt);
				oapi_dsmcc_stream_event_Convert(&(pstStreamEventData[ulCnt]));
#if defined(CONFIG_DEBUG)
				oapi_dsmcc_stream_event_Print(&(pstStreamEventData[ulCnt]), "\t[StreamEvent:EventProc]");
#endif
				oapi_dsmcc_stream_event_list_Append(&pstStreamEventList, &(pstStreamEventData[ulCnt]));
			}


			if(pstStreamEventData)
			{
				HLIB_STD_MemFree(pstStreamEventData);
				pstStreamEventData = NULL;
			}

			stDsmccEventInfo.hDsmcc = hDsmccHandle;
			stDsmccEventInfo.eEvent = eDsmccEvent;
			stDsmccEventInfo.unParam.stStreamEvent.pstEventList = pstStreamEventList;

			pstDsmccInfo->pstStreamEvent = pstStreamEventList;

			oapi_dsmcc_ExecuteHandler(pstDsmccInfo->pstHandler, (HUINT32)stDsmccEventInfo.hDsmcc , (HUINT32)stDsmccEventInfo.eEvent, &stDsmccEventInfo.unParam);
		}
		break;
	case eDxDSMCC_EVENT_CarouselList:
		{
			HUINT32 		ulCarouselNum = 0;
			void			*pvCarouselList = NULL;
			HUINT32		ulSize;
			DxDSMCC_CarouselInfo_t	*pstCarouselList = NULL;

			ulCarouselNum 		= HxOBJECT_INT(argv[3]);

			pvCarouselList 	= (void *)HxOBJECT_BIN_DATA (argv[4]);
			ulSize			= (HUINT32 )HxOBJECT_BIN_SIZE (argv[4]);

			pstObject = HLIB_SERIALIZER_Unpack(pvCarouselList, ulSize);
			if (pstObject == NULL)
			{
				HxLOG_Error("cannot unpack the data \n");
				goto END_FUNC;
			}

			if (HxOBJECT_ARR_LEN(pstObject) != 1)
			{
				HxLOG_Error("invalid object arr len. \n");
				goto END_FUNC;
			}

			pstCarouselList = HLIB_STD_MemCalloc(ulCarouselNum* sizeof(DxDSMCC_CarouselInfo_t));
			if(pstCarouselList)
			{
				HxSTD_memset(pstCarouselList, 0x00,ulCarouselNum*sizeof(DxDSMCC_CarouselInfo_t));
				HxSTD_MemCopy(pstCarouselList ,
								HxOBJECT_ARR_VAL(pstObject, 0)->u.bin.data,
								HxOBJECT_ARR_VAL(pstObject, 0)->u.bin.size);

			}


			if((ulCarouselNum* sizeof(DxDSMCC_CarouselInfo_t)) != HxOBJECT_ARR_VAL(pstObject, 0)->u.bin.size)
			{
				HxLOG_Error("(ulCarouselNum* sizeof(DxDSMCC_CarouselInfo_t))(%d) != HxOBJECT_ARR_VAL(pstObject, 0)->u.bin.size (%d) !!! should be checked!!! \n", (ulCarouselNum* sizeof(DxDSMCC_StreamInfo_t)), HxOBJECT_ARR_VAL(pstObject, 0)->u.bin.size);
			}


			HxSTD_memset(&pstDsmccInfo->stCarouselList, 0x00, sizeof(DxDSMCC_CarouselList_t));
			pstDsmccInfo->stCarouselList.ulNum = ulCarouselNum;
			pstDsmccInfo->stCarouselList.pstCarouselInfo = pstCarouselList;
			if(ulCarouselNum != HxOBJECT_BIN_SIZE(pstObject))
			{
				HxLOG_Error("ulCarouselNum(%d) != HxOBJECT_BIN_SIZE(pstObject)(%d) !!! should be checked!!! \n", ulCarouselNum, HxOBJECT_BIN_SIZE(pstObject));
			}



#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s :: ulCarouselNum (%d) pstCarouselInfoList(%p) \n", oapi_dsmcc_MakeStrEventType(eDsmccEvent),ulCarouselNum, pstCarouselList);
			oapi_dsmcc_CarouselListPrint(&pstDsmccInfo->stCarouselList, "EventNotifier");
#endif

			stDsmccEventInfo.hDsmcc = hDsmccHandle;
			stDsmccEventInfo.eEvent = eDsmccEvent;
			HxSTD_memcpy(&stDsmccEventInfo.unParam.stCarousel, &pstDsmccInfo->stCarouselList, sizeof(DxDSMCC_CarouselList_t));
			oapi_dsmcc_ExecuteHandler(pstDsmccInfo->pstHandler, (HUINT32)stDsmccEventInfo.hDsmcc , (HUINT32)stDsmccEventInfo.eEvent, &stDsmccEventInfo.unParam);
		}
		break;

	case eDxDSMCC_EVENT_Ait:
		{
			HUINT32						 ulCnt;
			HUINT32						 ulSecDataSize, ulRawDataSize;
			HUINT8						*pucSrcRaw, *pucDstRaw = NULL;
			HUINT8						*pucCurrRaw = NULL;
			OxDsmcc_AitSectionData_t	*pstSrcAitSecData;

			pstSrcAitSecData	= (OxDsmcc_AitSectionData_t *)HxOBJECT_BIN_DATA(argv[3]);
			ulSecDataSize		= (HUINT32)HxOBJECT_BIN_SIZE(argv[3]);

			pucSrcRaw			= (HUINT8 *)HxOBJECT_BIN_DATA (argv[4]);
			ulRawDataSize		= (HUINT32 )HxOBJECT_BIN_SIZE (argv[4]);

			if (ulSecDataSize != sizeof(OxDsmcc_AitSectionData_t))
			{
				HxLOG_Error("ulSecDataSize != sizeof(OxDsmcc_AitSectionData_t)");
				goto END_FUNC;
			}

			HxSTD_MemSet(pstSrcAitSecData->paucAitRawSec, 0, sizeof(HUINT8 *) * OxDSMCC_AIT_SECTION_NUM_MAX);

			if ((pstSrcAitSecData->ulAitSecCount > 0) && (pstSrcAitSecData->ulAitTotaSeclLen > 0))
			{
				pucDstRaw = HLIB_STD_MemDup(pucSrcRaw, ulRawDataSize);
				if (NULL == pucDstRaw)
				{
					HxLOG_Error("Memory allocation failed:\n");
					goto END_FUNC;
				}

				for (ulCnt = 0, pucCurrRaw = pucDstRaw; ulCnt < pstSrcAitSecData->ulAitSecCount; ulCnt++)
				{
					pstSrcAitSecData->paucAitRawSec[ulCnt] = pucCurrRaw;
					pucCurrRaw += pstSrcAitSecData->aulAitSecLen[ulCnt];
				}
			}

			HxSTD_MemCopy(&(pstDsmccInfo->stAitSectionData), pstSrcAitSecData, sizeof(OxDsmcc_AitSectionData_t));
			if (NULL != pstDsmccInfo->pucTotalAitBinary)
			{
				HLIB_STD_MemFree(pstDsmccInfo->pucTotalAitBinary);
				pstDsmccInfo->pucTotalAitBinary = NULL;
			}
			pstDsmccInfo->pucTotalAitBinary = pucDstRaw;

			stDsmccEventInfo.hDsmcc = hDsmccHandle;
			stDsmccEventInfo.eEvent = eDsmccEvent;

			oapi_dsmcc_ExecuteHandler(pstDsmccInfo->pstHandler, (HUINT32)stDsmccEventInfo.hDsmcc , (HUINT32)stDsmccEventInfo.eEvent, &stDsmccEventInfo.unParam);
		}
		break;

	case eDxDSMCC_EVENT_GeneralError:
		{
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s \n", oapi_dsmcc_MakeStrEventType(eDsmccEvent));
#endif
		}
		break;

	case eDxDSMCC_EVENT_Unknown:
		{
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("==> %s \n", oapi_dsmcc_MakeStrEventType(eDsmccEvent));
#endif
		}
		break;
	}

END_FUNC:
	if (NULL != pstObject)					{ HLIB_SERIALIZER_FreeObject(pstObject); }
	if (NULL != pstObjDesc)					{ HLIB_SERIALIZER_FreeObject(pstObjDesc); }
	return;
}

#define ______GLOBAL_FUNCTIONS______
void	OAPI_DSMCC_Init(OAPI_DSMCC_NotifilerForAppkit_t pfnAppkitNotifier)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxSTD_MemSet(&s_stOapiDsmccMap, 0, sizeof(oapiDsmcc_Map_t));

	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OAPI_DSMCC_EventNotifier, NULL, NULL, (HxRPC_Noti_t)oapi_dsmcc_EventNotifier);

	s_fnDsmccApkNotifier = pfnAppkitNotifier;

}

void	OAPI_DSMCC_DeInit(void)
{
	oapi_dsmcc_UnregisterNotifierAll(s_stOapiDsmccMap.pstDsmccList);

	// TODO:
	//OAPI_UnregisterCallback(RPC_OAPI_DSMCC_EventNotifier, oapi_dsmcc_EventNotifier);

	HxSTD_MemSet(&s_stOapiDsmccMap, 0, sizeof(oapiDsmcc_Map_t));
}

HERROR	OAPI_DSMCC_Create(DxDSMCC_Type_e eType, Handle_t *phDsmcc, HCHAR **ppszCachePath)
{
	HERROR		hError;
	HINT32		nReqId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_dsmcc_HandleCreated, NULL, RPC_OAPI_DSMCC_CreateHandle, "i", eType);

	HxLOG_Debug("eType [%d] nReqId [0x%x] \n", eType, nReqId);

	if (HLIB_RPC_Join (nRpcHandle, nReqId, 10000) == ERR_FAIL)
	{
		HxLOG_Error("==========> ERROR !!!! (%s) has HLIB_RPC_Request (nRpcHandle, ) time out !!!!\n\n", __FUNCTION__);
		*phDsmcc = HANDLE_NULL;

		return ERR_FAIL;
	}

	if(s_hTempDsmcc== HANDLE_NULL)
	{
		HxLOG_Error("s_hTempDsmcc is HANDLE_NULL \n");
		*phDsmcc = HANDLE_NULL;
		return ERR_FAIL;
	}

	*phDsmcc = s_hTempDsmcc;
	*ppszCachePath = &s_stDsmccDefaultCachePath[0];

	hError = oapi_dsmcc_AddInfo(s_hTempDsmcc, &s_stDsmccDefaultCachePath[0]);

	HxLOG_Debug("hError[%d] s_hTempDsmcc [0x%08x]  s_stDsmccDefaultCachePath[%s] \n", hError, s_hTempDsmcc, s_stDsmccDefaultCachePath);

	return hError;
}


HERROR OAPI_DSMCC_Destroy(Handle_t hDsmcc)
{
	oapiDsmcc_Info_t	*pstDsmccInfo = NULL;

	HxLOG_Debug("hDsmcc [%d]\n", hDsmcc);

	// Detache Info From Map
	oapi_dsmcc_RemoveDsmccInfo(hDsmcc, &pstDsmccInfo);

	// Unresigter Notifier
	oapi_dsmcc_UnregisterNotifier(pstDsmccInfo);

	// Remove Stream Event List
	oapi_dsmcc_StreamEventListFree(&pstDsmccInfo->pstStreamEvent);

	// Remove Carousel List
	oapi_dsmcc_CarouselListFree(&pstDsmccInfo->stCarouselList);

	// Remove AIT Info
	HxSTD_MemSet(&(pstDsmccInfo->stAitSectionData), 0, sizeof(OxDsmcc_AitSectionData_t));
	if (NULL != pstDsmccInfo->pucTotalAitBinary)
	{
		HLIB_STD_MemFree(pstDsmccInfo->pucTotalAitBinary);
		pstDsmccInfo->pucTotalAitBinary = NULL;
	}

	// Free Info
	HLIB_STD_MemFree(pstDsmccInfo);

	return ERR_OK;

}

void	OAPI_DSMCC_FreePath(HCHAR *ppszCachePath)
{
	HxSTD_memset(&s_stDsmccDefaultCachePath[0], 0x00, sizeof(HCHAR)* eDxDSMCC_MAX_PATH_LENGTH);
}

void	OAPI_DSMCC_RegisterNotifier(Handle_t hDsmcc, OAPI_DSMCC_Notifiler_t pfnObjectHandler)
{
	HBOOL			bResult;
	oapiDsmcc_Info_t	*pstDsmccInfo = NULL;

	HxLOG_Debug("hDsmcc [%d]\n", hDsmcc);

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == TRUE) && (pstDsmccInfo != NULL))
	{
		oapi_dsmcc_RegisterNotifierToList(&pstDsmccInfo->pstHandler, pfnObjectHandler);

		HxLOG_Debug("oapi_dsmcc_RegisterNotifierToList :: pstInfo->pstHandler : [0x%x]  \n", pstDsmccInfo->pstHandler);
	}
}

void	OAPI_DSMCC_UnRegisterNotifier(Handle_t hDsmcc, OAPI_DSMCC_Notifiler_t pfnObjectHandler)
{
	HBOOL			bResult;
	oapiDsmcc_Info_t	*pstDsmccInfo = NULL;

	HxLOG_Debug("hDsmcc [%d]\n", hDsmcc);

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == TRUE) && (pstDsmccInfo!=NULL) && (pstDsmccInfo->pstHandler != NULL))
	{
		oapi_dsmcc_UnregisterNotifierFromList(&pstDsmccInfo->pstHandler, pfnObjectHandler);

		HxLOG_Debug("oapi_dsmcc_UnregisterNotifierFromList :: pstInfo->pstHandler : [0x%x]  \n", pstDsmccInfo->pstHandler);
	}
}

HERROR OAPI_DSMCC_GetCarouselList(Handle_t hDsmcc, DxDSMCC_CarouselList_t *pstDsmccCarouselList)
{
	HERROR						hErr;
	oapiDsmcc_Info_t				*pstDsmccInfo = NULL;
	DxDSMCC_CarouselList_t		*pstSrc = NULL;
	HBOOL			bResult;

	HxLOG_Debug("hDsmcc [0x%x]\n", hDsmcc);

	if (NULL == pstDsmccCarouselList)
	{
		HxLOG_Error("NULL == pstList \n");
		return ERR_FAIL;
	}

	pstDsmccInfo = NULL;
	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == FALSE) || (NULL == pstDsmccInfo))
	{
		HxLOG_Error("Error : pstDsmccInfo (0x%08x) bResult(%d) !!! \n",pstDsmccInfo, bResult);
		return ERR_FAIL;
	}

	pstSrc = &pstDsmccInfo->stCarouselList;

#if defined(CONFIG_DEBUG)
	oapi_dsmcc_CarouselListPrint(pstSrc, "[GetList:Source]\t");
#endif

	hErr = ERR_FAIL;
	pstDsmccCarouselList->ulNum = 0;
	pstDsmccCarouselList->pstCarouselInfo = NULL;

	if (pstSrc->ulNum)
	{
		pstDsmccCarouselList->pstCarouselInfo = (DxDSMCC_CarouselInfo_t *)HLIB_STD_MemCalloc(sizeof(DxDSMCC_CarouselInfo_t)*pstSrc->ulNum);
		pstDsmccCarouselList->ulNum = pstSrc->ulNum;
	}

	if (pstDsmccCarouselList->pstCarouselInfo)
	{
		HxSTD_MemSet(pstDsmccCarouselList->pstCarouselInfo, 0, sizeof(DxDSMCC_CarouselInfo_t)*pstSrc->ulNum);
		HxSTD_MemCopy(pstDsmccCarouselList->pstCarouselInfo, pstSrc->pstCarouselInfo, sizeof(DxDSMCC_CarouselInfo_t)*pstSrc->ulNum);
		hErr = ERR_OK;
	}
	else
	{
		pstDsmccCarouselList->ulNum = 0;
	}

#if defined(CONFIG_DEBUG)
	oapi_dsmcc_CarouselListPrint(pstDsmccCarouselList, "[GetList:Dest]\t");
#endif

	return hErr;

}

void	OAPI_DSMCC_FreeCarouselInfo(DxDSMCC_CarouselInfo_t *pstCarouselInfo)
{
	HxLOG_Trace();

	HLIB_STD_MemFree(pstCarouselInfo);
	pstCarouselInfo = NULL;
}

HERROR OAPI_DSMCC_Start(Handle_t hDsmcc, Handle_t hCarousel, const HCHAR *pszCachePath)
{
	HBOOL			bResult;
	oapiDsmcc_Info_t 	*pstDsmccInfo = NULL;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Debug("hDsmcc [%d], hCarousel [%d], pszCachePathe [%s]\n", hDsmcc, hCarousel, pszCachePath ? pszCachePath : "Null");

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == FALSE) || (pstDsmccInfo == NULL) )
	{
		HxLOG_Error("Error : pstDsmccInfo (0x%08x) bResult(%d) !!! \n",pstDsmccInfo, bResult);
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_DSMCC_Start, "iiis", hDsmcc, hCarousel,  pstDsmccInfo->hSession, pszCachePath);

	return ERR_OK;
}

HERROR	OAPI_DSMCC_ReStart(Handle_t hDsmcc, Handle_t hCarousel)
{
	HBOOL			bResult;
	oapiDsmcc_Info_t	*pstDsmccInfo = NULL;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Debug("hDsmcc [%d], hCarousel [%d]\n", hDsmcc, hCarousel);

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == FALSE) || (pstDsmccInfo == NULL) )
	{
		HxLOG_Error("pstDsmccInfo (0x%08x) bResult(%d) !!! \n",pstDsmccInfo, bResult);
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_DSMCC_ReStart, "iii", hDsmcc, hCarousel , pstDsmccInfo->hSession);

	return ERR_OK;
}

HERROR	OAPI_DSMCC_Stop(Handle_t hDsmcc, Handle_t hCarousel)
{
	HBOOL			bResult;
	oapiDsmcc_Info_t	*pstDsmccInfo = NULL;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Debug("hDsmcc [%d], hCarousel [%d]\n", hDsmcc, hCarousel);

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == FALSE) || (pstDsmccInfo == NULL) )
	{
		HxLOG_Error("Error : pstDsmccInfo (0x%08x) bResult(%d) !!! \n",pstDsmccInfo, bResult);

		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_DSMCC_Stop, "iii", hDsmcc, hCarousel, pstDsmccInfo->hSession);

	return ERR_OK;
}

HERROR OAPI_DSMCC_RequestEvent(Handle_t hDsmcc, HBOOL bAll)
{
	oapiDsmcc_Info_t 	*pstDsmccInfo = NULL;
	HBOOL			bResult ;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Debug("hDsmcc [%d], bAll [%d]\n", hDsmcc, bAll);

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == FALSE) || (pstDsmccInfo == NULL) )
	{
		HxLOG_Error("pstDsmccInfo == NULL  !!! \n");
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_DSMCC_RequestEvent, "iii", hDsmcc, pstDsmccInfo->hSession, bAll);

	return ERR_OK;
}

HERROR OAPI_DSMCC_Open(Handle_t hDsmcc, Handle_t hSession)
{
	oapiDsmcc_Info_t 	*pstDsmccInfo = NULL;
	HBOOL			bResult ;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Debug("hDsmcc [0x%x] hSession [0x%x] \n", hDsmcc, hSession);

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc,&pstDsmccInfo);
	HxLOG_Debug("bResult: %d \n", bResult);
	if ((bResult == FALSE) || (pstDsmccInfo == NULL) )
	{
		HxLOG_Error("Error : pstDsmccInfo (0x%08x) bResult(%d) !!! \n",pstDsmccInfo, bResult);
		return ERR_FAIL;
	}

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_DSMCC_Open, "ii", hDsmcc, hSession);

	return ERR_OK;
}

HERROR OAPI_DSMCC_Close(Handle_t hDsmcc, Handle_t hSession)
{
	oapiDsmcc_Info_t	*pstDsmccInfo = NULL;
	Handle_t			 hSessionToClose;
	HBOOL			bResult ;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	// Session Close
	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == FALSE) || (pstDsmccInfo == NULL) )
	{
		HxLOG_Error("Error : pstDsmccInfo (0x%08x) bResult(%d) !!! \n",pstDsmccInfo, bResult);
		return ERR_FAIL;
	}

	HxLOG_Debug("Session to Close : hDsmcc [%d], hSession [0x%08x]\n", hDsmcc, hSession);

	// Reset Session ID
	hSessionToClose = hSession;
	pstDsmccInfo->hSession = HANDLE_NULL;

	// Remove Stream Event List
	if (pstDsmccInfo->pstStreamEvent)
	{
		oapi_dsmcc_StreamEventListFree(&pstDsmccInfo->pstStreamEvent);
		pstDsmccInfo->pstStreamEvent = NULL;
	}

	// Remove Carousel List
	oapi_dsmcc_CarouselListFree(&pstDsmccInfo->stCarouselList);

	// Remove AIT Info
	HxSTD_MemSet(&(pstDsmccInfo->stAitSectionData), 0, sizeof(OxDsmcc_AitSectionData_t));
	if (NULL != pstDsmccInfo->pucTotalAitBinary)
	{
		HLIB_STD_MemFree(pstDsmccInfo->pucTotalAitBinary);
		pstDsmccInfo->pucTotalAitBinary = NULL;
	}

	HxLOG_Debug("Session To Close [hDsmcc:%d, hSession:0x%08x, hSessionToClose : 0x%08x] >>>\n", hDsmcc, hSession, hSessionToClose );

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_DSMCC_Close, "ii", hDsmcc, hSessionToClose);

	return ERR_OK;
}

HERROR OAPI_DSMCC_CountAitSection (Handle_t hDsmcc, HUINT32 *pulAitNum)
{
	oapiDsmcc_Info_t	*pstDsmccInfo = NULL;
	HBOOL				 bResult ;

	if (NULL == pulAitNum)
	{
		HxLOG_Error("pulAitNum NULL\n");
		return ERR_FAIL;
	}

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == FALSE) || (pstDsmccInfo == NULL) )
	{
		HxLOG_Error("Error : pstDsmccInfo (0x%08x) bResult(%d) !!! \n",pstDsmccInfo, bResult);
		return ERR_FAIL;
	}

	*pulAitNum = pstDsmccInfo->stAitSectionData.ulAitSecCount;
	return ERR_OK;
}

HERROR OAPI_DSMCC_GetAitPid (Handle_t hDsmcc, HUINT16 *pusPrevAitPid, HUINT16 *pusCurrAitPid)
{
	oapiDsmcc_Info_t	*pstDsmccInfo = NULL;
	HBOOL				 bResult ;

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == FALSE) || (pstDsmccInfo == NULL) )
	{
		HxLOG_Error("Error : pstDsmccInfo (0x%08x) bResult(%d) !!! \n",pstDsmccInfo, bResult);
		return ERR_FAIL;
	}

	if (NULL != pusPrevAitPid)
	{
		*pusPrevAitPid = pstDsmccInfo->stAitSectionData.usPrevAitPid;
	}

	if (NULL != pusCurrAitPid)
	{
		*pusCurrAitPid = pstDsmccInfo->stAitSectionData.usCurrAitPid;
	}

	return ERR_OK;
}

HERROR OAPI_DSMCC_GetAitSection (Handle_t hDsmcc, HUINT32 ulIndex, HUINT8 **ppucAitSec, HUINT32 *ulSecLen)
{
	oapiDsmcc_Info_t	*pstDsmccInfo = NULL;
	HBOOL				 bResult ;
	HUINT8				*pucAitSec = NULL;
	HUINT32				 ulSectionLen = 0;

	if (NULL == ppucAitSec)						{ return ERR_FAIL; }
	if (NULL == ulSecLen)						{ return ERR_FAIL; }

	bResult = oapi_dsmcc_FindDsmccInfoByHandle(hDsmcc, &pstDsmccInfo);
	if ((bResult == FALSE) || (pstDsmccInfo == NULL) )
	{
		HxLOG_Error("Error : pstDsmccInfo (0x%08x) bResult(%d) !!! \n",pstDsmccInfo, bResult);
		return ERR_FAIL;
	}

	*ppucAitSec 	= NULL;
	*ulSecLen 		= 0;

	if (ulIndex >= OxDSMCC_AIT_SECTION_NUM_MAX)
	{
		HxLOG_Error ("invalid index(%d) : max(%d) \n", ulIndex, OxDSMCC_AIT_SECTION_NUM_MAX);
		return ERR_FAIL;
	}

	ulSectionLen = pstDsmccInfo->stAitSectionData.aulAitSecLen[ulIndex];
	if (ulSectionLen > 0)
	{
		pucAitSec = (HUINT8 *)HLIB_STD_MemCalloc(ulSectionLen);
		if (pucAitSec == NULL)
		{
			HxLOG_Error ("mem alloc failed.....\n");
			return ERR_FAIL;
		}

		HxSTD_MemCopy(pucAitSec, pstDsmccInfo->stAitSectionData.paucAitRawSec[ulIndex], ulSectionLen);

		*ppucAitSec = pucAitSec;
		*ulSecLen	= ulSectionLen;
	}

	return ERR_OK;
}

HERROR OAPI_DSMCC_FreeAitSection (HUINT8 *pucAitSec)
{
	if (pucAitSec != NULL)
	{
		HLIB_STD_MemFree(pucAitSec);
	}

	return ERR_OK;
}


