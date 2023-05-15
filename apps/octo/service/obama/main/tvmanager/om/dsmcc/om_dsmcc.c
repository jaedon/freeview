/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_dsmcc.c
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <db_svc.h>
#include <svc_pipe.h>
#include <svc_output.h>
#include <svc_av.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_hbbtv.h>
#include <svc_si.h>
#include <svc_download.h>
#include <dlib_dsmcc.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#undef	HxLOG_DOMAIN
//#define	HxLOG_DOMAIN	"om_dsmcc"
//#define WORKAROUND_DSMCC_PMTCAROUSEL_WITH_SVCHANDLE

#define	MAX_PATH_BUFF		10

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eOmDsmcc_Event_Update_None				= 0x00000000,
	eOmDsmcc_Event_Update_Name				= 0x00000001,
	eOmDsmcc_Event_Update_Desc				= 0x00000002,
	eOmDsmcc_Event_Update_InfoByte			= 0x00000004,
	eOmDsmcc_Event_Update_Url				= 0x00000008,
	eOmDsmcc_Event_Update_Tag				= 0x00000010,
	eOmDsmcc_Event_Update_All				= 0xFFFFFFFF,
} omDsmcc_EventUpdateFlag_e;

typedef enum
{
	eOmDsmcc_Status_Init				= 0,
	eOmDsmcc_Status_Download_Start,
	eOmDsmcc_Status_Download_Fail,
	eOmDsmcc_Status_Download_Success,
	eOmDsmcc_Status_Download_Stop,
} omDsmcc_Status_e;

typedef struct tagDsmccStreamEvtList
{
	HUINT32				ulRequestNum;
	HxList_t				*pstStreamEventInfoList;	// DxDSMCC_StreamInfo_t
} omDsmcc_StreamEvtList_t;

typedef struct
{
	Handle_t				hSession;			// Session Unique Id from Media Control
	HUINT32				ulActionId;			// Action Id
	HUINT16 				usTsUniqId;
} omDsmcc_Session_t;

typedef struct tagDsmccAccessInfo
{
	Handle_t				 	hDsmcc;			// hAction or id
	DxDSMCC_Type_e			eDsmccType;
	omDsmcc_Session_t		 stSession;
	DxDSMCC_CarouselList_t	stCarouselList;
	omDsmcc_StreamEvtList_t	 stStreamEventList;
} omDsmcc_AccessInfo_t;

typedef struct
{
	omDsmcc_Status_e		eStatus;
	HUINT32				ulCarouselId;
	HUINT8				ucComponentTag;
	HUINT8				ucStreamType;
	HUINT16				usPid;
	HUINT16				usDataBroadcastTag;
} omDsmcc_PidInfo_t;

typedef struct tagDsmccPidList
{
	HUINT32						 ulNum;
	omDsmcc_PidInfo_t				*astPidInfo;
} omDsmcc_PidList_t;

typedef struct
{
	HUINT32						 ulCarouselId;
	HxList_t						*pstPidList;
	SvcDownload_ActionType_e		eActType;
	HCHAR						 szCachePath[eDxDSMCC_MAX_PATH_LENGTH];
	void							*pvUserData;
} omDsmcc_CarouselInfo_t;

typedef struct tagDsmccObject
{
	HBOOL							 bDownloadStarted;
	omDsmcc_AccessInfo_t			 stAccessInfo;
	HxList_t						*pstCarouselList;		// omDsmcc_CarouselInfo_t
} omDsmcc_Object_t;

typedef struct
{
	Handle_t					hAction;
	HUINT16					usTsUniqId;
	omDsmcc_PidList_t			stPidList;
} omDsmcc_PmtPidInfo_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HxList_t					*_pstOmDsmccMap = NULL;		// List of omDsmcc_Object_t
STATIC omDsmcc_PmtPidInfo_t		s_stPmtPidInfo;


STATIC HUINT32	s_ulPathIdx = 0;
STATIC HCHAR		s_szPath[MAX_PATH_BUFF][1024] = { { 0,  }, { 0, } };

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define ___STATIC_FUNCTIONS___

#if defined(CONFIG_DEBUG)
STATIC void om_dsmcc_pmtpidlist_Print(omDsmcc_PmtPidInfo_t *pstInfo)
{
	HUINT32 ulCount;

	if (NULL == pstInfo)
		return;

	HxLOG_Debug("\n\n\t =======================================================================\n");
	HxLOG_Debug("\t = hAction  : [0x%08x], TsUniqId : [%d]\n", pstInfo->hAction, pstInfo->usTsUniqId);
	HxLOG_Debug("\t = List Num : [%d]\n", pstInfo->stPidList.ulNum);
	HxLOG_Debug("\t =======================================================================\n");
	if (pstInfo->stPidList.astPidInfo)
	{
		for (ulCount=0; ulCount<pstInfo->stPidList.ulNum; ulCount++)
		{
			HxLOG_Debug("\t = [%02d] : CarouselId [0x%08x], CompTag [0x%02x], Pid [0x%04x], Type [0x%02x], DataTag [0x%04x]\n",
									ulCount,
									pstInfo->stPidList.astPidInfo[ulCount].ulCarouselId,
									pstInfo->stPidList.astPidInfo[ulCount].ucComponentTag,
									pstInfo->stPidList.astPidInfo[ulCount].usPid,
									pstInfo->stPidList.astPidInfo[ulCount].ucStreamType,
									pstInfo->stPidList.astPidInfo[ulCount].usDataBroadcastTag);
		}
	}
	else
	{
		HxLOG_Debug("\t = List Empty!!!\n");
	}
	HxLOG_Debug("\t =======================================================================\n\n\n");
}

STATIC void om_dsmcc_stream_event_Print(DxDSMCC_StreamEvent_t *pstEvent, HCHAR *pszPrefix)
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

STATIC void om_dsmcc_carousel_info_Print(DxDSMCC_CarouselInfo_t *pstCarouselInfo, HCHAR *pszPrefix)
{
	if (pstCarouselInfo)
	{
		HCHAR szIndent[32];

		HxLOG_Debug("======================================================== \n");

		HxSTD_snprintf(szIndent, 32-1, "%s\t", pszPrefix ? pszPrefix : "");
		HxLOG_Debug("%s ulCarouselId : [0x%08x] num(%d) \n", szIndent, pstCarouselInfo->ulCarouselId, pstCarouselInfo->ulNum);
		if (0 < pstCarouselInfo->ulNum && NULL != pstCarouselInfo->astEsInfo)
		{
			HUINT32 ulCount;
			for (ulCount=0; ulCount<pstCarouselInfo->ulNum; ulCount++)
			{
				HxLOG_Debug("%s\t [%02d] : CompTag[0x%02x], StreamType[0x%02x] \n", szIndent, ulCount, pstCarouselInfo->astEsInfo[ulCount].ucComponentTag,
											pstCarouselInfo->astEsInfo[ulCount].ucStreamType);
			}
		}
		else
		{
			HxLOG_Debug("%s 0 == pstInfo->ulNum || NULL == pstInfo->astList \n", szIndent, __FUNCTION__, __LINE__);
		}


		HxLOG_Debug("======================================================== \n");
	}
}

STATIC void om_dsmcc_carousel_list_Print(HUINT32 ulNum, DxDSMCC_CarouselList_t *astCarouselList)
{
	if (astCarouselList)
	{
		HxLOG_Debug("\t ======================================================================== \n");
		HxLOG_Debug("\t =\t Carousel Num [%d], Addr [%p] \n", ulNum, astCarouselList);
		if (0 < ulNum && NULL != astCarouselList)
		{
			HUINT32 ulCount;
			for (ulCount=0; ulCount<ulNum; ulCount++)
			{
				HxLOG_Debug("\t ------------------------------------------------------------------------ \n");
				om_dsmcc_carousel_info_Print(astCarouselList[ulCount].pstCarouselInfo, "\t");
				if (ulCount != ulNum-1)
					HxLOG_Debug("\t ------------------------------------------------------------------------ \n");
			}
		}
		else
		{
			HxLOG_Debug("0 == ulNum || NULL == astList \n");
		}
		HxLOG_Debug("\t ======================================================================== \n");
	}
}

STATIC void om_dsmcc_carousel_pidlist_Print(HUINT32 ulCarouselId, HxList_t *pstPidList)
{
	HxList_t			*pstList;
	omDsmcc_PidInfo_t	*pstPidInfo;

	HxLOG_Debug("\n\n\t =======================================================================\n");
	HxLOG_Debug("\t = CarouselId : [0x%08x]\n", ulCarouselId);
	HxLOG_Debug("\t = List Num   : [%d]\n", HLIB_LIST_Length(pstPidList));
	HxLOG_Debug("\t =======================================================================\n");
	pstList = HLIB_LIST_First(pstPidList);

	if (NULL == pstList)
	{
		HxLOG_Debug("\t = List Empty!!!\n");
	}
	else
	{
		HUINT32		ulCount = 0;
		while(pstList)
		{
			pstPidInfo = (omDsmcc_PidInfo_t *)HLIB_LIST_Data(pstList);
			if (pstPidInfo)
			{
				HxLOG_Debug("\t = [%02d] : Status [%d], CompTag [0x%x], Type [0x%x], Pid [0x%x], DataTag [0x%x]\n", ulCount,
															pstPidInfo->eStatus,		pstPidInfo->ucComponentTag,
															pstPidInfo->ucStreamType,	pstPidInfo->usPid,
															pstPidInfo->usDataBroadcastTag);
			}
			ulCount++;
			pstList = pstList->next;
		}
	}

	HxLOG_Debug("\t =======================================================================\n\n\n");
}


#endif

STATIC HBOOL	om_dsmcc_map_CB_findByType(void *pvUserData, void *pvListData)
{
	omDsmcc_Object_t		*pstListInfo;
	DxDSMCC_Type_e		 eType;

	if (NULL == pvListData)
	{
		return FALSE;
	}

	pstListInfo = (omDsmcc_Object_t *)pvListData;
	eType = (DxDSMCC_Type_e)pvUserData;

	return (pstListInfo->stAccessInfo.eDsmccType == eType) ? TRUE : FALSE;
}

STATIC HERROR om_dsmcc_map_FindByType(DxDSMCC_Type_e eType, omDsmcc_Object_t **ppstObject)
{
	HxList_t			*pstFound, *pstList;

	pstList = HLIB_LIST_First(_pstOmDsmccMap);
	pstFound = HLIB_LIST_FindEx(pstList, (const void *)eType, om_dsmcc_map_CB_findByType);
	if (NULL == pstFound)
	{
		return ERR_FAIL;
	}

	if (ppstObject)
	{
		*ppstObject = (omDsmcc_Object_t *)HLIB_LIST_Data(pstFound);
	}

	return ERR_OK;
}

STATIC HBOOL	om_dsmcc_map_CB_findByHandle(void *pvUserData, void *pvListData)
{
	omDsmcc_Object_t	*pstListInfo;
	Handle_t		 hDsmcc;

	if (NULL == pvListData)
	{
		return FALSE;
	}

	pstListInfo = (omDsmcc_Object_t *)pvListData;
	hDsmcc = (Handle_t)pvUserData;

	return (pstListInfo->stAccessInfo.hDsmcc == hDsmcc) ? TRUE : FALSE;
}

STATIC HERROR	om_dsmcc_map_FindByHandle(Handle_t hDsmcc, omDsmcc_Object_t **ppstObject)
{
	HxList_t			*pstFound, *pstList;

	//HxLOG_Debug("List Num [%d]\n", HLIB_LIST_Length(_pstDsmccMap));

	pstList = HLIB_LIST_First(_pstOmDsmccMap);
	pstFound = HLIB_LIST_FindEx(pstList, (const void *)hDsmcc, om_dsmcc_map_CB_findByHandle);
	if (NULL == pstFound)
	{
		return ERR_FAIL;
	}

	if (ppstObject)
	{
		*ppstObject = (omDsmcc_Object_t *)HLIB_LIST_Data(pstFound);
	}

	return ERR_OK;
}

STATIC HBOOL	om_dsmcc_carousel_map_CB_findByCarouselId(void *pvUserData, void *pvListData)
{
	omDsmcc_CarouselInfo_t	*pstListInfo;
	HUINT32				 	 ulCarouselId;

	if (NULL == pvListData)
	{
		return FALSE;
	}

	pstListInfo = (omDsmcc_CarouselInfo_t *)pvListData;
	ulCarouselId = (HUINT32)pvUserData;

	return (pstListInfo->ulCarouselId == ulCarouselId) ? TRUE : FALSE;
}

STATIC HERROR om_dsmcc_carousel_map_FindByCarouselId (HxList_t *pstCarouselList, HUINT32 ulCarouselId, omDsmcc_CarouselInfo_t **ppstCarousel)
{
	HxList_t			*pstFound, *pstList;

	pstList = HLIB_LIST_First(pstCarouselList);
	pstFound = HLIB_LIST_FindEx(pstList, (const void *)ulCarouselId, om_dsmcc_carousel_map_CB_findByCarouselId);
	if (NULL == pstFound)
	{
		return ERR_FAIL;
	}

	if (ppstCarousel)
	{
		*ppstCarousel = (omDsmcc_CarouselInfo_t *)HLIB_LIST_Data(pstFound);
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_carousel_map_FindByPid(omDsmcc_Object_t *pstObj, HUINT16 usPid, omDsmcc_CarouselInfo_t **ppstCarousel, omDsmcc_PidInfo_t **ppstPidInfo)
{
	HxList_t				*pstList, *pstPidList;
	omDsmcc_CarouselInfo_t	*pstCarousel;
	omDsmcc_PidInfo_t		*pstPidInfo = NULL;
	HBOOL					 bFound;

	bFound = FALSE;
	pstCarousel = NULL;
	pstList = HLIB_LIST_First(pstObj->pstCarouselList);
	while(pstList)
	{
		pstCarousel = (omDsmcc_CarouselInfo_t *)HLIB_LIST_Data(pstList);
		if (pstCarousel)
		{
			pstPidList = HLIB_LIST_First(pstCarousel->pstPidList);
			while(pstPidList)
			{
				pstPidInfo = (omDsmcc_PidInfo_t *)HLIB_LIST_Data(pstPidList);
				if (pstPidInfo)
				{
					if (pstPidInfo->usPid == usPid)
					{
						bFound = TRUE;
						break;
					}
				}
				pstPidList = pstPidList->next;
			}
			if (bFound)
				break;
		}
		pstList = pstList->next;
	}

	if (ppstCarousel)
	{
		*ppstCarousel = (bFound) ? pstCarousel : NULL;
	}

	if (ppstPidInfo)
	{
		*ppstPidInfo = (bFound) ? pstPidInfo : NULL;
	}

	return (bFound) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR om_dsmcc_map_Create(DxDSMCC_Type_e eType, omDsmcc_Object_t **ppstObject)
{
	omDsmcc_Object_t *pstObj;

	pstObj = HLIB_STD_MemAlloc(sizeof(omDsmcc_Object_t));
	if (NULL == pstObj)
	{
		HxLOG_Error("NULL == pstObj\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstObj, 0, sizeof(omDsmcc_Object_t));
	HxSTD_MemSet(&pstObj->stAccessInfo.stSession, 0xFF, sizeof(omDsmcc_Session_t));
	pstObj->bDownloadStarted = FALSE;

	_pstOmDsmccMap = HLIB_LIST_Append(_pstOmDsmccMap, (void *)pstObj);

	if (ppstObject)
	{
		*ppstObject = pstObj;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_carousel_map_Create(HxList_t **ppstList, HUINT32 ulCarouselId, omDsmcc_CarouselInfo_t **ppstCarousel)
{
	omDsmcc_CarouselInfo_t	*pstCarousel = NULL;

	if (NULL == ppstList)
	{
		HxLOG_Error("NULL == ppstList\n");
		return ERR_FAIL;
	}

	pstCarousel = (omDsmcc_CarouselInfo_t *)HLIB_STD_MemAlloc(sizeof(omDsmcc_CarouselInfo_t));
	if (NULL == pstCarousel)
	{
		HxLOG_Critical("NULL == pstCarousel\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstCarousel, 0, sizeof(omDsmcc_CarouselInfo_t));
	pstCarousel->ulCarouselId = ulCarouselId;
	*ppstList = HLIB_LIST_Append(*ppstList, (void *)pstCarousel);

	if (ppstCarousel)
	{
		*ppstCarousel = pstCarousel;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_carousel_map_Remove(HxList_t **ppstList, HUINT32 ulCarouselId, omDsmcc_CarouselInfo_t **ppstCarousel)
{
	omDsmcc_CarouselInfo_t	*pstCarousel = NULL;

	if (NULL == ppstList)
		return ERR_FAIL;

	om_dsmcc_carousel_map_FindByCarouselId(*ppstList, ulCarouselId, &pstCarousel);
	if (NULL == pstCarousel)
	{
		return ERR_FAIL;
	}

	*ppstList = HLIB_LIST_Remove(*ppstList, (const void *)pstCarousel);
	if (ppstCarousel)
	{
		*ppstCarousel = pstCarousel;
	}

	return ERR_OK;
}


STATIC void om_dsmcc_accessInfoInit(void)
{
	_pstOmDsmccMap = NULL;
}

STATIC HERROR om_dsmcc_carousel_Create(Handle_t hDsmcc, HUINT32 ulCarouselId, omDsmcc_CarouselInfo_t **ppstCarousel)
{
	omDsmcc_Object_t			*pstObj = NULL;
	omDsmcc_CarouselInfo_t	*pstCarousel = NULL;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	hErr = om_dsmcc_map_FindByHandle(hDsmcc, &pstObj);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("NULL == pstObj\n");
		*ppstCarousel = NULL;
		return ERR_FAIL;
	}

	pstCarousel = NULL;
	om_dsmcc_carousel_map_FindByCarouselId(pstObj->pstCarouselList, ulCarouselId, &pstCarousel);
	if (NULL == pstCarousel)
	{
		om_dsmcc_carousel_map_Create(&pstObj->pstCarouselList, ulCarouselId, &pstCarousel);
	}

	if (ppstCarousel)
	{
		*ppstCarousel = pstCarousel;
	}

	return (pstCarousel) ? ERR_OK : ERR_FAIL;
}


STATIC HCHAR *om_dsmcc_get_cachePath(HCHAR *pszBasePath)
{
	HCHAR *pszPath;

	if (NULL == pszBasePath)
		return NULL;

	pszPath = s_szPath[s_ulPathIdx];
	s_ulPathIdx = ((s_ulPathIdx + 1) >= MAX_PATH_BUFF) ? 0 : s_ulPathIdx + 1;

	if (HLIB_STD_StrEndWith((const HCHAR *)pszBasePath, "/"))
		HxSTD_snprintf(pszPath, 1023, "%s", pszBasePath);
	else
		HxSTD_snprintf(pszPath, 1023, "%s/", pszBasePath);

	return pszPath;
}

STATIC HERROR om_dsmcc_start_download (omDsmcc_Object_t *pstDsmccObject, omDsmcc_CarouselInfo_t *pstCarousel)
{
	Handle_t				 hAction;
	HUINT16					 usTsUniqId;
	HxList_t				*pstList;
	omDsmcc_PidInfo_t		*pstPidInfo;
	SvcDownload_SetData_t	 stSetData;
	HERROR					 hErr, hResult = ERR_OK;


	HxLOG_Debug("PidList Count [%d] \n", HLIB_LIST_Length(pstCarousel->pstPidList));

	hAction = SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId);
	usTsUniqId = pstDsmccObject->stAccessInfo.stSession.usTsUniqId;

	if (TRUE != pstDsmccObject->bDownloadStarted)
	{
		hErr = SVC_DOWNLOAD_Start(hAction, usTsUniqId, eDOWN_DOWNLOAD_HBBTV, PID_NULL, 0, 0, 0);
		if (ERR_OK == hErr)
		{
			pstDsmccObject->bDownloadStarted = TRUE;
		}
		else
		{
			HxLOG_Error("SVC_DOWNLOAD_Start err: hAction(0x%08x), usTsUniqId(%d)\n", hAction, usTsUniqId);
			return ERR_FAIL;
		}
	}

	// Stop Carousel if it starts:
	for (pstList = HLIB_LIST_First(pstCarousel->pstPidList); NULL != pstList; pstList = HLIB_LIST_Next(pstList))
	{
		pstPidInfo = (omDsmcc_PidInfo_t *)HLIB_LIST_Data(pstList);
		if (pstPidInfo)
		{
			HxSTD_MemSet(&stSetData, 0, sizeof(SvcDownload_SetData_t));
			stSetData.eSetType = eDOWN_SET_HBBTV_REMOVEPID;
			stSetData.stHbbtvRemovePid.ulCarouselId = pstPidInfo->ulCarouselId;
			stSetData.stHbbtvRemovePid.usPid = pstPidInfo->usPid;

			hErr = SVC_DOWNLOAD_Set(hAction, eDOWN_SET_HBBTV_REMOVEPID, &stSetData);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_DOWNLOAD_Set err:\n");
			}
		}
	}

	// Start all the PID in the carousel:
	for (pstList = HLIB_LIST_First(pstCarousel->pstPidList); NULL != pstList; pstList = HLIB_LIST_Next(pstList))
	{
		pstPidInfo = (omDsmcc_PidInfo_t *)HLIB_LIST_Data(pstList);
		if (pstPidInfo)
		{
			// Skip the event message:
			if (0x0C == pstPidInfo->ucStreamType)
			{
				continue;
			}

			HxLOG_Debug("hAction(0x%x) CachePath(%s) call \n", hAction, pstCarousel->szCachePath);

			HxSTD_MemSet(&stSetData, 0, sizeof(SvcDownload_SetData_t));
			stSetData.eSetType = eDOWN_SET_HBBTV_ADDPID;
			stSetData.stHbbtvAddPid.ulCarouselId	= pstPidInfo->ulCarouselId;
			stSetData.stHbbtvAddPid.usPid			= pstPidInfo->usPid;
			stSetData.stHbbtvAddPid.ucStreamType	= pstPidInfo->ucStreamType;
			stSetData.stHbbtvAddPid.pszCachePath	= pstCarousel->szCachePath;

			hErr = SVC_DOWNLOAD_Set(hAction, eDOWN_SET_HBBTV_ADDPID, &stSetData);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SVC_DOWNLOAD_Set err: CarouselId(%d), PID(%d)\n", stSetData.stHbbtvAddPid.ulCarouselId, stSetData.stHbbtvAddPid.usPid);
				hResult = ERR_FAIL;
			}
		}
	}

	return hResult;
}

STATIC HERROR om_dsmcc_stop_download (omDsmcc_Object_t *pstDsmccObject)
{
	Handle_t			 hAction;
	HERROR				 hErr;

	if (NULL == pstDsmccObject)
	{
		return ERR_FAIL;
	}

	if (TRUE == pstDsmccObject->bDownloadStarted)
	{
		hAction = SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId);
		hErr = SVC_DOWNLOAD_Stop(hAction, eAsyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_DOWNLOAD_Stop err: hAction(0x%08x)\n", hAction);
			return ERR_FAIL;
		}

		pstDsmccObject->bDownloadStarted = FALSE;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_RequestStreamEventDownload (omDsmcc_Object_t *pstDsmccObject, Handle_t hAction, HUINT16 usTsUniqId, HUINT32 ulRequestNum, omDsmcc_PmtPidInfo_t *pstPmtPid)
{
	HUINT32					 ulCnt;
	HCHAR					*pszCachePath = "/tmp/cache/";
	SvcDownload_SetData_t	 stSetData;
	HERROR					 hErr, hResult = ERR_OK;

	if (NULL == pstPmtPid)
	{
		HxLOG_Error("pstPmtPid NULL\n");
		return ERR_FAIL;
	}

	pstDsmccObject->stAccessInfo.stStreamEventList.ulRequestNum = ulRequestNum;

	HxLOG_Debug("hAction [0x%x], usTsUniqId [0x%04x], ulRequestNum [%d] \n", hAction, usTsUniqId, ulRequestNum);

	for (ulCnt = 0; ulCnt < pstPmtPid->stPidList.ulNum; ulCnt++)
	{
		omDsmcc_PidInfo_t	*pstPidInfo = &(pstPmtPid->stPidList.astPidInfo[ulCnt]);

		if (0x0C == pstPidInfo->ucStreamType)
		{
			HxSTD_MemSet(&stSetData, 0, sizeof(SvcDownload_SetData_t));

			stSetData.eSetType = eDOWN_SET_HBBTV_ADDPID;
			stSetData.stHbbtvAddPid.ulCarouselId = pstPidInfo->ulCarouselId;
			stSetData.stHbbtvAddPid.usPid = pstPidInfo->usPid;
			stSetData.stHbbtvAddPid.ucStreamType = pstPidInfo->ucStreamType;
			stSetData.stHbbtvAddPid.pszCachePath = pszCachePath;

			HxLOG_Debug("[%02d] : CarouselId [0x%08x], StreamType [0x%02x] \n", ulCnt, pstPidInfo->ulCarouselId, pstPidInfo->ucStreamType);
			hErr = SVC_DOWNLOAD_Set(hAction, eDOWN_SET_HBBTV_ADDPID, &stSetData);
			if (ERR_OK == hErr)
			{
				pstPidInfo->eStatus = eOmDsmcc_Status_Download_Start;
			}
			else
			{
				HxLOG_Error("SVC_DOWNLOAD_Set err: hAction(0x%08x), usPid(0x%04x), ulCarouselId(%d)\n", hAction, pstPidInfo->usPid, pstPidInfo->ulCarouselId);
				hResult = ERR_FAIL;
			}
		}
	}

	return hResult;
}

STATIC HBOOL om_dsmcc_pmtpidlist_Check(omDsmcc_PmtPidInfo_t *pstPmtPidInfo)
{
	HxLOG_Assert(pstPmtPidInfo);

	HxLOG_Debug("pstPmtPidInfo->hAction : [%x] \n", pstPmtPidInfo->hAction);
	if (HANDLE_NULL == pstPmtPidInfo->hAction)
		return FALSE;

	HxLOG_Debug("pstPmtPidInfo->stPidList.ulNum : [%d] \n", pstPmtPidInfo->stPidList.ulNum);
	if (NULL == pstPmtPidInfo->stPidList.astPidInfo)
		return FALSE;

	if (0 == pstPmtPidInfo->stPidList.ulNum)
		return FALSE;

	return TRUE;
}

STATIC HxList_t *om_dsmcc_pidlist_AddList(HUINT32 ulCarouselId, omDsmcc_PidList_t *pstSrc)
{
	HINT32		 ulNum;
	HxList_t	*pstList;

	pstList = NULL;
	ulNum = pstSrc->ulNum;
	if (0 < ulNum && NULL != pstSrc->astPidInfo)
	{
		HUINT32 			 ulCount;
		omDsmcc_PidInfo_t	*pstInfo;

		HxLOG_Debug("\t Adding Carousel Id [0x%08x]\n", ulCarouselId);
		for (ulCount=0; ulCount<ulNum; ulCount++)
		{
			pstInfo = &pstSrc->astPidInfo[ulCount];
			if (pstInfo->ulCarouselId == ulCarouselId)
			{
				HxLOG_Debug("\t Add : Status[%d], CTAG [0x%02x], Stype [0x%02x], PID [0x%04x], DTAG [0x%04x]\n",
																pstInfo->eStatus,
																pstInfo->ucComponentTag, pstInfo->ucStreamType,
																pstInfo->usPid, pstInfo->usDataBroadcastTag);
				pstList = HLIB_LIST_Append(pstList, pstInfo);
			}
		}
	}
	else
	{
		HxLOG_Debug("[%s:%04] 0 == nCount or NULL == pstSrc->stPidList.astPidInfo \n", __FUNCTION__, __LINE__);
	}

	return pstList;
}

STATIC void om_dsmcc_pidlist_ClearStatus(void *pvData)
{
	omDsmcc_PidInfo_t	*pstPidList = (omDsmcc_PidInfo_t *)pvData;

	if (pstPidList)
	{
		pstPidList->eStatus = eOmDsmcc_Status_Init;
	}
}

STATIC HxList_t *om_dsmcc_pidlist_Clear(HxList_t *pstPidList)
{
	HxList_t	*pstList;

	pstList = HLIB_LIST_First(pstPidList);
	HLIB_LIST_Foreach(pstList, om_dsmcc_pidlist_ClearStatus);

	return HLIB_LIST_RemoveAll(pstList);
}

STATIC HERROR	om_dsmcc_carousel_list_Free(DxDSMCC_CarouselList_t *pstDsmccCarouselList)
{
	HUINT32 	ulCount;

	HxLOG_Assert(pstDsmccCarouselList);

	if (NULL == pstDsmccCarouselList->pstCarouselInfo)
	{
		HxLOG_Debug("NULL == pstList->astCarouselInfoList \n");
		return ERR_FAIL;
	}

	for (ulCount=0; ulCount<pstDsmccCarouselList->ulNum; ulCount++)
	{
		if (pstDsmccCarouselList->pstCarouselInfo[ulCount].astEsInfo)
		{
			HxSTD_MemSet(pstDsmccCarouselList->pstCarouselInfo[ulCount].astEsInfo, 0x00, sizeof(DxDSMCC_StreamInfo_t) *eDxDSMCC_MAX_ES_STREAM_NUM );
		}
	}
	HLIB_STD_MemFree(pstDsmccCarouselList->pstCarouselInfo);
	pstDsmccCarouselList->pstCarouselInfo = NULL;
	pstDsmccCarouselList->ulNum = 0;

	return ERR_OK;
}

STATIC HERROR om_dsmcc_carousel_list_Build(DxDSMCC_CarouselList_t *pstDest, omDsmcc_PmtPidInfo_t *pstSrc)
{
	HUINT32					 ulCount = 0, ulIdx = 0, ulListCount = 0, ulCarouselId = 0;
	DxDSMCC_CarouselInfo_t	*pstCarouselInfo = NULL, *pstTempCarouselInfo = NULL;
	omDsmcc_PidInfo_t			*pstPidInfo;

	if (0 == pstSrc->stPidList.ulNum || NULL == pstSrc->stPidList.astPidInfo)
	{
		HxLOG_Error("[%s:][%04d] NULL == pstSrc->stPidList.astPidInfo\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pstCarouselInfo = NULL;
	if (pstSrc->stPidList.ulNum)
	{
		pstCarouselInfo = (DxDSMCC_CarouselInfo_t *)HLIB_STD_MemAlloc(sizeof(DxDSMCC_CarouselInfo_t)*pstSrc->stPidList.ulNum);
	}

	if (NULL == pstCarouselInfo)
	{
		HxLOG_Error("[%s:][%04d] NULL == astList \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstCarouselInfo, 0, sizeof(DxDSMCC_CarouselInfo_t)*pstSrc->stPidList.ulNum);

	ulListCount = 0;
	for (ulCount=0; ulCount<pstSrc->stPidList.ulNum; ulCount++)
	{
		pstPidInfo = &pstSrc->stPidList.astPidInfo[ulCount];
		ulCarouselId = pstPidInfo->ulCarouselId;

		// Find Carousel Id
		pstTempCarouselInfo = NULL;
		for (ulIdx=0; ulIdx < ulListCount; ulIdx++)
		{
			if (pstCarouselInfo[ulIdx].ulCarouselId == ulCarouselId)
			{
				pstTempCarouselInfo = &pstCarouselInfo[ulIdx];
				break;
			}
		}

		// New Carousel Id
		if (NULL == pstTempCarouselInfo)
		{
			// Add
			pstTempCarouselInfo = &pstCarouselInfo[ulListCount];

			HxLOG_Info("[%s:][%04d] Add ulCarouselId [0x%08x] \n", __FUNCTION__, __LINE__, ulCarouselId);

			pstTempCarouselInfo->ulCarouselId = ulCarouselId;
			pstTempCarouselInfo->ulNum = 0;
			ulListCount++;
		}

		if (pstTempCarouselInfo->astEsInfo)
		{
			// Add ES Info
			pstTempCarouselInfo->astEsInfo[pstTempCarouselInfo->ulNum].ucComponentTag = pstPidInfo->ucComponentTag;
			pstTempCarouselInfo->astEsInfo[pstTempCarouselInfo->ulNum].ucStreamType = pstPidInfo->ucStreamType;
			pstTempCarouselInfo->ulNum++;

			HxLOG_Info("[%s:][%04d] ulNum(%d) ulCarouselId(0x%08x)  CompTag(0x%x)  StreamType (0x%x) \n", __FUNCTION__, __LINE__,
														pstTempCarouselInfo->ulNum, ulCarouselId,
														pstPidInfo->ucComponentTag,
														pstPidInfo->ucStreamType);
		}
	}

	pstDest->pstCarouselInfo = pstCarouselInfo;
	pstDest->ulNum = ulListCount;

	return ERR_OK;
}

STATIC HERROR om_dsmcc_carousel_pidlist_Clear(HxList_t *pstCarouselList)
{
	HERROR					 hErr;
	HxList_t					*pstList;
	omDsmcc_CarouselInfo_t 	*pstCarousel;

	hErr = ERR_OK;
	pstList = HLIB_LIST_First(pstCarouselList);
	while(pstList)
	{
		pstCarousel = (omDsmcc_CarouselInfo_t *)HLIB_LIST_Data(pstList);
		if (pstCarousel)
		{
			pstCarousel->pstPidList = om_dsmcc_pidlist_Clear(pstCarousel->pstPidList);
		}
		pstList = pstList->next;
	}

	return hErr;
}

STATIC HxList_t *om_dsmcc_carousel_pidlist_BuildOne(omDsmcc_CarouselInfo_t *pstCarousel, omDsmcc_PidList_t *pstPidList)
{
	HxLOG_Assert(pstCarousel);
	HxLOG_Assert(pstPidList);

	pstCarousel->pstPidList =om_dsmcc_pidlist_Clear(pstCarousel->pstPidList);
	pstCarousel->pstPidList = om_dsmcc_pidlist_AddList(pstCarousel->ulCarouselId, pstPidList);

	return pstCarousel->pstPidList;
}

STATIC HERROR om_dsmcc_carousel_pidlist_BuildAll(HxList_t *pstCarouselList, omDsmcc_PidList_t *pstPidList)
{
	HxList_t				*pstList;
	omDsmcc_CarouselInfo_t	*pstCarousel;

	HxLOG_Assert(pstCarouselList);
	HxLOG_Assert(pstPidList);

	pstList = HLIB_LIST_First(pstCarouselList);
	while(pstList)
	{
		pstCarousel = (omDsmcc_CarouselInfo_t *)HLIB_LIST_Data(pstList);
		if (pstCarousel)
		{
			om_dsmcc_carousel_pidlist_BuildOne(pstCarousel, pstPidList);
		}
		pstList = pstList->next;
	}

	return ERR_OK;
}

STATIC void om_dsmcc_pmtpidlist_Free(omDsmcc_PidList_t *pstPidList)
{
	if (pstPidList->astPidInfo)
	{
		HLIB_STD_MemFree(pstPidList->astPidInfo);
	}
	pstPidList->astPidInfo = NULL;
	pstPidList->ulNum = 0;
}

STATIC HERROR om_dsmcc_pmtpidlist_Get(Handle_t hAction, omDsmcc_PidList_t *pstPidList, HUINT16 *pusTsUniqId)
{
#if defined(WORKAROUND_DSMCC_PMTCAROUSEL_WITH_SVCHANDLE)
	HUINT16						 usSvcId;
	Handle_t					 hSvc;
	HERROR						 hErr;
#endif
	HINT32						 nListNum;
	HUINT16						 usTsUniqId;
	SvcSi_WebPmtInfo_t			*astPmtList;
	omDsmcc_PidInfo_t			*pstPidInfo;
	HUINT32			 			 ulCount;

	HxLOG_Assert(pstPidList);

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("hAction == HANDLE_NULL >>>\n");
		return ERR_FAIL;
	}

	// Get PID list from PMT
	usTsUniqId = 0;
	nListNum = 0;
	astPmtList = NULL;

	if ( SVC_SI_GetWebPmtList(hAction, &astPmtList, &nListNum, &usTsUniqId) != ERR_OK )
	{
		HxLOG_Error("SVC_SI_GetWebPmtList(() Error >>>\n");
		return ERR_FAIL;
	}

	if (NULL == astPmtList || 0 >= nListNum)
	{
		HxLOG_Error("astPmtList(%p) nListNum(%d) \n", astPmtList, nListNum);
		return ERR_FAIL;
	}

	if (pusTsUniqId)
	{
		*pusTsUniqId = usTsUniqId;
	}

#if defined(WORKAROUND_DSMCC_PMTCAROUSEL_WITH_SVCHANDLE)
	hErr = MGR_LIVE_GetServiceHandle(SVC_PIPE_GetActionId(hAction), eLiveViewPlaySvc_ACTUAL, &hSvc);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_LIVE_GetServiceHandle err: ActionId(%d)\n", SVC_PIPE_GetActionId(hAction));
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceIdFromSvcHandle(hSvc, &usSvcId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle err: hSvc(0x%08x)\n", hSvc);
		return ERR_FAIL;
	}
#endif

	HxLOG_Debug("[%s:][%04d] hAction [0x%08x], TsUniqId [%d] PmtList Count  [%d]>>>\n", __FUNCTION__, __LINE__, hAction, usTsUniqId, nListNum);

	pstPidList->astPidInfo = NULL;
	pstPidList->ulNum = 0;
	pstPidList->astPidInfo = HLIB_STD_MemAlloc(sizeof(omDsmcc_PidInfo_t)*nListNum);
	if (pstPidList->astPidInfo)
	{
		HUINT32			 ulDstNum = 0;

		for (ulCount=0; ulCount<nListNum; ulCount++)
		{
			pstPidInfo = &pstPidList->astPidInfo[ulDstNum];

#if defined(WORKAROUND_DSMCC_PMTCAROUSEL_WITH_SVCHANDLE)
			if (astPmtList[ulCount].usPmtSvcId != usSvcId)
			{
				continue;
			}
#endif
			pstPidInfo->eStatus				= eOmDsmcc_Status_Init;
			pstPidInfo->ulCarouselId			= astPmtList[ulCount].ulCarouselIDTag;
			pstPidInfo->ucComponentTag			= astPmtList[ulCount].ucComponentTag;
			pstPidInfo->ucStreamType			= astPmtList[ulCount].ucStreamType;
			pstPidInfo->usPid					= astPmtList[ulCount].usPid;
			pstPidInfo->usDataBroadcastTag		= astPmtList[ulCount].usDataBroadcastTag;
			HxLOG_Debug("List[%02d] CarouselId:0x%08x, ComTag:0x%02x, Pid:0x%04x, StreamType:0x%02x >>>\n",
														ulCount, pstPidInfo->ulCarouselId, pstPidInfo->ucComponentTag,
														pstPidInfo->usPid, pstPidInfo->ucStreamType);
			ulDstNum++;
		}

		pstPidList->ulNum = ulDstNum;
	}

	if (pstPidList->ulNum == 0)
	{
		if (NULL != pstPidList->astPidInfo)
		{
			HLIB_STD_MemFree(pstPidList->astPidInfo);
			pstPidList->astPidInfo = NULL;
		}

		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_pmtpidlist_Process(omDsmcc_Object_t *pstDsmccObject, Handle_t hAction, HBOOL bUpdateCarousel, HBOOL bNotifyAccessCode, HUINT16 *pusTsUniqId)
{
	HERROR	hError;
	HUINT16	usTsUniqId;

	HxLOG_Info("bUpdateCarousel [%d], bNotifyAccessCode [%d]\n", bUpdateCarousel, bNotifyAccessCode);

	// Clear Carousel's Pid List
	if (bUpdateCarousel && pstDsmccObject->pstCarouselList)
	{
		om_dsmcc_carousel_pidlist_Clear(pstDsmccObject->pstCarouselList);
		pstDsmccObject->pstCarouselList = NULL;
	}

	// TODO: Compare Old and New

	// Free PMT PidInfo
	om_dsmcc_pmtpidlist_Free(&s_stPmtPidInfo.stPidList);

	// Build PMT PidInfo
	usTsUniqId = 0;
	hError = om_dsmcc_pmtpidlist_Get(hAction, &s_stPmtPidInfo.stPidList, &usTsUniqId);

	HxLOG_Info("om_dsmcc_pmtpidlist_Get [hError  : %d] \n", hError);

	if (hError == ERR_OK)
	{
		// Set Action Handle
		s_stPmtPidInfo.hAction		= hAction;
		s_stPmtPidInfo.usTsUniqId	= usTsUniqId;

#if defined(CONFIG_DEBUG)
		om_dsmcc_pmtpidlist_Print(&s_stPmtPidInfo);
#endif
		if (pusTsUniqId)
		{
			*pusTsUniqId = usTsUniqId;
		}

		// Free Carousel List
		om_dsmcc_carousel_list_Free(&pstDsmccObject->stAccessInfo.stCarouselList);

		// Build Carousel List
		hError = om_dsmcc_carousel_list_Build(&pstDsmccObject->stAccessInfo.stCarouselList, &s_stPmtPidInfo);

		HxLOG_Debug("om_dsmcc_carousel_list_Build [hError	: %d] \n", hError);

		if (hError == ERR_OK)
		{
			// Build PMT Pid List
			if (bUpdateCarousel)
			{
				if (TRUE == om_dsmcc_pmtpidlist_Check(&s_stPmtPidInfo))
				{
					if (pstDsmccObject->pstCarouselList)
					{
						HxLOG_Debug("om_dsmcc_carousel_pidlist_BuildAll() \n");
						om_dsmcc_carousel_pidlist_BuildAll(pstDsmccObject->pstCarouselList, &s_stPmtPidInfo.stPidList);
					}
				}
				else
				{
					HxLOG_Debug("FALSE == om_dsmcc_pmtpidlist_Check() \n");
				}
			}
		}
	}

	return hError;
}

STATIC HERROR om_dsmcc_AllocUserData(DxDSMCC_Type_e eType, void **pvUserData)
{
	HERROR		hErr;

	if (NULL == pvUserData)
		return ERR_FAIL;

	if (*pvUserData)
	{
		HLIB_STD_MemFree(*pvUserData);
		*pvUserData = NULL;
	}

	hErr = ERR_OK;
	switch (eType)
	{
	case eDxDSMCC_TYPE_HBBTV:
		*pvUserData = NULL;
		break;
	default:
		*pvUserData = NULL;
		break;
	}

	return hErr;
}


STATIC HERROR om_dsmcc_PrepareDownload(Handle_t hDsmcc, HUINT32 ulCarouselId, omDsmcc_CarouselInfo_t **ppstCarousel)
{
	DxDSMCC_Type_e			 eDsmccType = eDxDSMCC_TYPE_Unknown;
	omDsmcc_Object_t			*pstDsmccObject = NULL;
	omDsmcc_CarouselInfo_t	*pstCarouselInfo = NULL;
	HERROR					hError;

	hError = om_dsmcc_map_FindByHandle(hDsmcc, &pstDsmccObject);
	if (hError != ERR_OK ||  NULL == pstDsmccObject)
	{
		HxLOG_Error(" pstDsmccObject(%p) hError(%d)\n", pstDsmccObject,hError );
		return ERR_FAIL;
	}

	eDsmccType = pstDsmccObject->stAccessInfo.eDsmccType;

	HxLOG_Debug("hDsmcc [%d] ulCarouselId [0x%08x] eDsmccType[%d] \n", hDsmcc, ulCarouselId, eDsmccType);

	hError = om_dsmcc_carousel_Create(hDsmcc, ulCarouselId, &pstCarouselInfo);
	if (hError != ERR_OK || NULL == pstCarouselInfo)
	{
		HxLOG_Error(" pstCarouselInfo(%p) hError(%d) \n", pstCarouselInfo, hError);
		return ERR_FAIL;
	}

	om_dsmcc_AllocUserData(eDsmccType, &pstCarouselInfo->pvUserData);

	if (ppstCarousel)
	{
		*ppstCarousel = pstCarouselInfo;
	}

	return ERR_OK;
}

STATIC HUINT32 om_dsmcc_GetActionIdBySessionHandle(Handle_t hSession)
{
	HUINT32			ulActionId;

	// TODO: Get Action ID From Media Control by Session Handle
	ulActionId = MGR_ACTION_GetMainActionId();
	// Maybe we can find action id from Media Control Module Veiw0 or View1

	return ulActionId;
}

STATIC void om_dsmcc_stream_event_info_Free(DxDSMCC_StreamEvent_t *pstEvent)
{
	if (NULL == pstEvent)
		return;

	if (pstEvent->szDescription)		HLIB_STD_MemFree(pstEvent->szDescription);
	if (pstEvent->szEventName)		HLIB_STD_MemFree(pstEvent->szEventName);
	if (pstEvent->pucObjInfoByte)		HLIB_STD_MemFree(pstEvent->pucObjInfoByte);

	HxSTD_MemSet(pstEvent, 0, sizeof(DxDSMCC_StreamEvent_t));
}

STATIC void om_dsmcc_stream_event_CB_Free(void *pvData)
{
	DxDSMCC_StreamEvent_t *pstEvent = (DxDSMCC_StreamEvent_t *)pvData;

	if (pstEvent)
	{
		om_dsmcc_stream_event_info_Free(pstEvent);
		if (pstEvent)
			HLIB_STD_MemFree(pstEvent);
	}
}

STATIC HERROR om_dsmcc_stream_event_list_Free(omDsmcc_StreamEvtList_t *pstList)
{
	HxLOG_Assert(pstList);

	// Free Stream Event List
	if (pstList->pstStreamEventInfoList)
	{
		HLIB_LIST_Foreach(pstList->pstStreamEventInfoList, om_dsmcc_stream_event_CB_Free);
		pstList->pstStreamEventInfoList = HLIB_LIST_RemoveAll(pstList->pstStreamEventInfoList);
	}

	return ERR_OK;
}

STATIC void om_dsmcc_carousel_download_CheckComplete(omDsmcc_CarouselInfo_t *pstCarousel, HBOOL *pbAll, HBOOL *pbPrimary)
{
	HxList_t			*pstList;
	omDsmcc_PidInfo_t	*pstPidInfo;
	HBOOL				 bAllComplete, bPrimaryComplete;

	bAllComplete = bPrimaryComplete = FALSE;

	pstList = HLIB_LIST_First(pstCarousel->pstPidList);
	if (NULL == pstList)
	{
		HxLOG_Error("[%s]][%04d] NULL == pstList\n", __FUNCTION__, __LINE__);
		return;
	}

	bAllComplete = TRUE;
	bPrimaryComplete = FALSE;
	while(pstList)
	{
		pstPidInfo = (omDsmcc_PidInfo_t *)HLIB_LIST_Data(pstList);
		if (pstPidInfo)
		{
			HxLOG_Debug("CarouselId [0x%08x] : Pid [0x%x], Tag [0x%04x], eStatus [%d]\n",
								pstPidInfo->ulCarouselId, pstPidInfo->usPid, pstPidInfo->usDataBroadcastTag, pstPidInfo->eStatus);
			if ((pstPidInfo->eStatus == eOmDsmcc_Status_Download_Success))
			{
				if ((FALSE == bPrimaryComplete) && (pstPidInfo->usDataBroadcastTag))
				{
					bPrimaryComplete = TRUE;
				}
			}
			else
			{
				bAllComplete = FALSE;
			}

			if (TRUE == bPrimaryComplete && FALSE == bAllComplete)
				break;
		}
		pstList = pstList->next;
	}

	*pbAll = bAllComplete;
	*pbPrimary = bPrimaryComplete;

}

STATIC HBOOL om_dsmcc_stream_event_CB_FindByEventName(void *pvUserData, void *pvListData)
{
	HUINT8					*pszEventName = (HUINT8 *)pvUserData;
	DxDSMCC_StreamEvent_t	*pstEvent = (DxDSMCC_StreamEvent_t *)pvListData;
	HUINT32					 ulLength, ulDestLength;
	HBOOL					 bRet;
	HCHAR					*pszDestName = NULL;

	if (NULL == pszEventName || NULL == pstEvent || NULL == pstEvent->szEventName)
	{
		//HxLOG_Error("NULL == pstList\n", pstEvent, );
		return FALSE;
	}

	pszDestName = NULL;
	ulLength = HxSTD_StrLen(pstEvent->szEventName);
	if (ulLength)
	{
		ulDestLength = HLIB_MATH_DecodeBase64(NULL, (const HCHAR *)pstEvent->szEventName, ulLength);
		if (ulDestLength)
		{
			pszDestName = HLIB_STD_MemAlloc(ulDestLength+1);
			if (pszDestName)
			{
				HLIB_MATH_DecodeBase64(pszDestName, (const HCHAR *)pstEvent->szEventName, ulLength);
				pszDestName[ulDestLength] = '\0';
			}
		}
	}

	bRet = FALSE;
	if (pszDestName)
	{
		bRet = (HxSTD_StrCmp(pszEventName, pszDestName) == 0) ? TRUE : FALSE;
	}

	HxLOG_Debug("[%s] : [%s] %s [%s] >>> \n", __FUNCTION__, pszEventName, (bRet) ? "==" : "!=", pszDestName);

	if (pszDestName)
	{
		HLIB_STD_MemFree(pszDestName);
	}

	return bRet;
}

STATIC HBOOL om_dsmcc_stream_event_CB_FindByEventId(void *pvUserData, void *pvListData)
{
	HUINT16					*pusEventId = (HUINT16 *)pvUserData;
	DxDSMCC_StreamEvent_t	*pstEvent = (DxDSMCC_StreamEvent_t *)pvListData;

	if ((NULL == pstEvent) || (NULL == pusEventId))
	{
		//HxLOG_Error("NULL == pstList\n", pstEvent, );
		return FALSE;
	}

	if ( pstEvent->nEventId == *pusEventId )
		return TRUE;

	return FALSE;
}

STATIC HERROR om_dsmcc_stream_event_FindByEventName(omDsmcc_StreamEvtList_t *pstEventList, HUINT8 *pszEventName, DxDSMCC_StreamEvent_t **ppstStreamEvent)
{
	HxList_t		*pstFound;

	HxLOG_Assert(pstEventList);

	if (ppstStreamEvent)
	{
		*ppstStreamEvent = NULL;
	}

	if (NULL == pszEventName)
	{
		return ERR_FAIL;
	}

	pstFound =  HLIB_LIST_FindEx(pstEventList->pstStreamEventInfoList, (const void *)pszEventName, om_dsmcc_stream_event_CB_FindByEventName);

	if (ppstStreamEvent)
	{
		*ppstStreamEvent = (DxDSMCC_StreamEvent_t *)HLIB_LIST_Data(pstFound);
	}

	return (pstFound) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR om_dsmcc_stream_event_FindByEventId(omDsmcc_StreamEvtList_t *pstEventList, HUINT16 usEventId, DxDSMCC_StreamEvent_t **ppstStreamEvent)
{
	HxList_t		*pstFound;

	HxLOG_Assert(pstEventList);

	if (ppstStreamEvent)
	{
		*ppstStreamEvent = NULL;
	}

	pstFound =  HLIB_LIST_FindEx(pstEventList->pstStreamEventInfoList, (const void *)&usEventId, om_dsmcc_stream_event_CB_FindByEventId);

	if (ppstStreamEvent)
	{
		*ppstStreamEvent = (DxDSMCC_StreamEvent_t *)HLIB_LIST_Data(pstFound);
	}

	return (pstFound) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR	om_dsmcc_util_encodeBase64(const HCHAR *pszSrc, HUINT32 ulSrcLenght, HCHAR **ppszDest,HUINT32 *ulEncodeLenght )
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

	ulDestLength = HLIB_MATH_EncodeBase64(NULL, (const HCHAR *)pszSrc, ulSrcLenght);
	if (ulDestLength)
	{
		pszDest = HLIB_STD_MemAlloc(ulDestLength+1);
		if (pszDest)
		{
			HLIB_MATH_EncodeBase64(pszDest, (const HCHAR *)pszSrc, ulSrcLenght);
			pszDest[ulDestLength] = '\0';
		}
	}

	*ppszDest = pszDest;

	if(ulDestLength)
		*ulEncodeLenght = ulDestLength;
	else
		*ulEncodeLenght = 0;

	return ERR_OK;
}

STATIC HERROR om_dsmcc_stream_event_Convert(DxDSMCC_StreamEvent_t *pstSrc, DxDSMCC_StreamEvent_t *pstDest, omDsmcc_EventUpdateFlag_e eUpdateFlag)
{
	HUINT32 ulEncodeLenght=0;

	HxLOG_Assert(pstSrc);
	HxLOG_Assert(pstDest);

	HxLOG_Debug("\t eUpdateFlag   : 0x%08x\n", eUpdateFlag);

	pstDest->nEventId								= pstSrc->nEventId;
	pstDest->usPid								= pstSrc->usPid;

	if (eOmDsmcc_Event_Update_Tag & eUpdateFlag)
	{
		pstDest->usTag							= pstSrc->usTag;
	}

	if (eOmDsmcc_Event_Update_Url & eUpdateFlag)
	{
		if (pstDest->pszUrl)
		{
			HLIB_STD_MemFree(pstDest->pszUrl);
			pstDest->pszUrl = NULL;
		}
		if (pstSrc->pszUrl)
		{
			om_dsmcc_util_encodeBase64((const HCHAR *)pstSrc->pszUrl, HxSTD_StrLen(pstSrc->pszUrl), (HCHAR **)&pstDest->pszUrl,&ulEncodeLenght);
		}
	}

	if (eOmDsmcc_Event_Update_Desc& eUpdateFlag)
	{
			if (pstDest->szDescription)
			{
				HLIB_STD_MemFree(pstDest->szDescription);
				pstDest->szDescription = NULL;
			}
		if (pstSrc->szDescription && pstSrc->ulDescLen)
		{
			om_dsmcc_util_encodeBase64((const HCHAR *)pstSrc->szDescription, pstSrc->ulDescLen, (HCHAR **)&pstDest->szDescription,&ulEncodeLenght);
			pstDest->ulDescLen = ulEncodeLenght;
		}
	}

	if (eOmDsmcc_Event_Update_Name& eUpdateFlag)
	{
		if (pstDest->szEventName)
		{
			HLIB_STD_MemFree(pstDest->szEventName);
			pstDest->szEventName = NULL;
		}
		if (pstSrc->szEventName)
		{
			om_dsmcc_util_encodeBase64((const HCHAR *)pstSrc->szEventName, HxSTD_StrLen(pstSrc->szEventName), (HCHAR **)&pstDest->szEventName,&ulEncodeLenght);
		}
	}

	if (eOmDsmcc_Event_Update_InfoByte & eUpdateFlag)
	{
		if (pstDest->pucObjInfoByte)
		{
			HLIB_STD_MemFree(pstDest->pucObjInfoByte);
			pstDest->pucObjInfoByte = NULL;
		}
		if (pstSrc->pucObjInfoByte && pstSrc->ulObjInfoByteLen)
		{
			om_dsmcc_util_encodeBase64((const HCHAR *)pstSrc->pucObjInfoByte, pstSrc->ulObjInfoByteLen, (HCHAR **)&pstDest->pucObjInfoByte,&ulEncodeLenght);
			pstDest->ulObjInfoByteLen = ulEncodeLenght - 1;

		}
	}

#if 0//defined(CONFIG_DEBUG)
	HxLOG_Debug("\t EId   : 0x%04x -> 0x%04x\n", pstSrc->nEventId, pstDest->nEventId);
	HxLOG_Debug("\t PID   : 0x%04x -> 0x%04x\n", pstSrc->usPid, pstDest->usPid);

	if (eOmDsmcc_Event_Update_Tag& eUpdateFlag)
	{
		HxLOG_Debug("\t A Tag : 0x%04x -> 0x%04x\n", pstSrc->usTag, pstDest->usTag);
	}

	if (eOmDsmcc_Event_Update_Url & eUpdateFlag)
	{
		HxLOG_Debug("\t URL   : [%s] -> [%s]\n", pstSrc->pszUrl, pstDest->pszUrl);
	}

	if (eOmDsmcc_Event_Update_Desc& eUpdateFlag)
	{
		if (pstSrc->szDescription && pstSrc->ulDescLen)
		{
			//HLIB_LOG_Dump(pstSrc->szDescription, pstSrc->ulDescLen, 0, FALSE);
			HxLOG_Debug("\t DESC  : -> [%s]\n", pstDest->szDescription ? (HCHAR *)pstDest->szDescription : "Null");
		}
	}

	if (eOmDsmcc_Event_Update_Name & eUpdateFlag)
	{
		if (pstSrc->szEventName)
		{
			HxLOG_Debug("\t EName : [%s] -> [%s]\n", (HCHAR *)pstSrc->szEventName, pstDest->szEventName ? (HCHAR *)pstDest->szEventName : "Null");
		}
	}

	if (eOmDsmcc_Event_Update_InfoByte& eUpdateFlag)
	{
		if (pstSrc->pucObjInfoByte && pstSrc->ulObjInfoByteLen)
		{
			HxLOG_Debug("\t OINFO : [%s] -> [%s]\n", (HCHAR *)pstSrc->pucObjInfoByte, pstDest->pucObjInfoByte ? (HCHAR *)pstDest->pucObjInfoByte : "Null");
		}
	}
#endif

	return ERR_OK;
}
STATIC HERROR om_dsmcc_stream_event_DataCheck(DxDSMCC_StreamEvent_t *pstStreamEvent, HUINT32 *ulDataFmat)
{
	HUINT32	ulformat = eDxDSMCC_EventData_None;

	if( pstStreamEvent->pszUrl != NULL && HxSTD_StrLen(pstStreamEvent->pszUrl)>0 )
	{
		HxLOG_Debug("pszUrl: %s \n", pstStreamEvent->pszUrl);
		ulformat |= eDxDSMCC_EventData_Url;
	}
	if( pstStreamEvent->szEventName != NULL && HxSTD_StrLen(pstStreamEvent->szEventName)>0 )
	{
		HxLOG_Debug("szEventName: %s \n", pstStreamEvent->szEventName);
		ulformat |= eDxDSMCC_EventData_EventName;
	}
	if( pstStreamEvent->szDescription != NULL && HxSTD_StrLen(pstStreamEvent->szDescription)>0 && pstStreamEvent->ulDescLen > 0)
	{
		HxLOG_Debug("szDescription: %s \n", pstStreamEvent->szDescription);
		ulformat |= eDxDSMCC_EventData_Description;
	}
	if( pstStreamEvent->pucObjInfoByte != NULL && pstStreamEvent->ulObjInfoByteLen > 0)
	{
		HxLOG_Debug("pucObjInfoByte: %s \n", (HCHAR *)pstStreamEvent->pucObjInfoByte);
		ulformat |= eDxDSMCC_EventData_ObjectInfo;
	}

	HxLOG_Debug("\t ulDataformat   : 0x%08x\n", ulformat);

	*ulDataFmat = ulformat;

	return ERR_OK;
}

STATIC HERROR om_dsmcc_stream_event_SerializeData(DxDSMCC_StreamEvent_t *pstStreamEvent,HINT32 nSrzr, HUINT32 ulDataFmat, void	*pvPrevData, HUINT32 ulPrevSize)
{
	if( ulPrevSize == 0 || pvPrevData == NULL )
	{
		switch(ulDataFmat)
		{
			case 15: // url + eventname + description + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibbibii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;
			case 14: // eventname + description + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibbibii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;
			case 13:// url + description + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibibii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;
			case 12:// description + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibibii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;
			case 11:// url + eventname + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibibii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 10:// eventname + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibibii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 9:// url + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibiibii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 8:// objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiiibii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 7:// url + eventname + description
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibbiii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 6:// eventname + description
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibbiii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 5:// url + description
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibiii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 4://  description
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibiii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 3:// url + eventname
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibiii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 2:// eventname
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibiii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 1:// url
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibiiii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

			case 0:
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiiiii",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag);
				break;

		}
	}
	else
	{
		switch(ulDataFmat)
		{
			case 15: // url + eventname + description + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibbibiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);

				break;
			case 14: // eventname + description + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibbibiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;
			case 13:// url + description + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibibiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;
			case 12:// description + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibibiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;
			case 11:// url + eventname + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibibiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

			case 10:// eventname + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibibiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

			case 9:// url + objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibiibiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

			case 8:// objectinfo
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiiibiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												pstStreamEvent->pucObjInfoByte,
												(HINT32)pstStreamEvent->ulObjInfoByteLen+1,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);

				break;

			case 7:// url + eventname + description
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibbiiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

			case 6:// eventname + description
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibbiiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

			case 5:// url + description
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibiiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

			case 4://  description
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibiiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szDescription,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

			case 3:// url + eventname
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibibiiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

			case 2:// eventname
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiibiiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												pstStreamEvent->szEventName,
												(HINT32)HxSTD_StrLen(pstStreamEvent->szEventName)+1,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

			case 1:// url
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iibiiiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												pstStreamEvent->pszUrl,
												(HINT32)HxSTD_StrLen(pstStreamEvent->pszUrl)+1,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

			case 0:
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "iiiiiib",
												(HINT32)ulDataFmat,
												(HINT32)pstStreamEvent->usPid,
												(HINT32)pstStreamEvent->ulDescLen,
												(HINT32)pstStreamEvent->ulObjInfoByteLen,
												(HINT32)pstStreamEvent->nEventId,
												(HINT32)pstStreamEvent->usTag,
												pvPrevData,
												(HINT32)ulPrevSize);
				break;

		}
	}
	return ERR_OK;
}


STATIC HERROR om_dsmcc_stream_event_list_Append(DxDSMCC_StreamEvent_t *pstSrc, omDsmcc_StreamEvtList_t *pstDest, omDsmcc_EventUpdateFlag_e eUpdateFlag, DxDSMCC_StreamEvent_t **ppstStreamEvent)
{
	DxDSMCC_StreamEvent_t	*pstStreamEvent = NULL;

	HxLOG_Assert(pstSrc);
	HxLOG_Assert(pstDest);

	pstStreamEvent = (DxDSMCC_StreamEvent_t *)HLIB_STD_MemAlloc(sizeof(DxDSMCC_StreamEvent_t));
	if (NULL == pstStreamEvent)
	{
		HxLOG_Error("NULL == pstStreamEvent \n");
		return ERR_FAIL;
	}

	// Add Stream Event List to Session Map
	HxSTD_MemSet(pstStreamEvent, 0, sizeof(DxDSMCC_StreamEvent_t));
	om_dsmcc_stream_event_Convert(pstSrc, pstStreamEvent, eUpdateFlag);
	pstDest->pstStreamEventInfoList = HLIB_LIST_Append(pstDest->pstStreamEventInfoList, (void *)pstStreamEvent);

	if (ppstStreamEvent)
	{
		*ppstStreamEvent = pstStreamEvent;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_Init(void)
{
	_pstOmDsmccMap= NULL;
	HxSTD_MemSet(&s_stPmtPidInfo, 0, sizeof(omDsmcc_PmtPidInfo_t));

	return ERR_OK;
}

STATIC HERROR om_dsmcc_UnInit(void)
{
	return ERR_OK;
}

STATIC HERROR om_dsmcc_CreateHandle (DxDSMCC_Type_e eDsmccType, Handle_t *phDsmcc)
{
	omDsmcc_Object_t	*pstDsmccObject = NULL;
	HERROR				 hErr;

	hErr = om_dsmcc_map_Create(eDsmccType, &pstDsmccObject);
	if ((ERR_OK != hErr) || (NULL == pstDsmccObject))
	{
		HxLOG_Error("om_dsmcc_map_Create failed:\n");
		return ERR_FAIL;
	}

	// Assign Dsmcc Type
	pstDsmccObject->stAccessInfo.eDsmccType		= eDsmccType;

	// Assign Dsmcc Handle
	pstDsmccObject->stAccessInfo.hDsmcc			= (Handle_t)pstDsmccObject;

	if (NULL != phDsmcc)						{ *phDsmcc = pstDsmccObject->stAccessInfo.hDsmcc; }
	return ERR_OK;
}


STATIC HERROR om_dsmcc_Open (omDsmcc_Object_t *pstDsmccObject, Handle_t hSession)
{
	HUINT16		 		 usTsUniqId;
	HUINT32				 ulHbbtvActId;
	HINT32				 nRpcHandle = 0;
	Handle_t			 hHbbtvAct;
	HERROR				 hErr;

	hErr = MGR_HBBTV_GetActionId(&ulHbbtvActId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("MGR_HBBTV_GetActionId err:\n");
		return ERR_FAIL;
	}

	HxLOG_Info("[0x%08x] => [0x%08x]\n", pstDsmccObject->stAccessInfo.stSession.hSession, hSession);
	HxLOG_Info("ulActionId [0x%08x] \n", ulHbbtvActId);

	pstDsmccObject->stAccessInfo.stSession.hSession = hSession;
	pstDsmccObject->stAccessInfo.stSession.ulActionId = ulHbbtvActId;

	hHbbtvAct = SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId);

	hErr = om_dsmcc_pmtpidlist_Process(pstDsmccObject, hHbbtvAct, TRUE, TRUE, &usTsUniqId);
	if (ERR_OK == hErr)
	{
		pstDsmccObject->stAccessInfo.stSession.usTsUniqId = usTsUniqId;

		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
		if ((ERR_OK == hErr) && (NULL != pstDsmccObject->pstCarouselList))
		{
			HUINT32					 ulCarouselNum = pstDsmccObject->stAccessInfo.stCarouselList.ulNum;
			DxDSMCC_CarouselInfo_t	*pstCarouselList = pstDsmccObject->stAccessInfo.stCarouselList.pstCarouselInfo;
			HINT32					 nSrzr;
			void					*pvData;
			HUINT32					 ulSize;

			nSrzr = HLIB_SERIALIZER_Open();
			if (0 != nSrzr)
			{
				HLIB_SERIALIZER_MakeSerializedData(nSrzr, "b",
												(void*)pstCarouselList,
												(HINT32)(ulCarouselNum * sizeof(DxDSMCC_CarouselInfo_t))),

				pvData = HLIB_SERIALIZER_Pack(nSrzr, &ulSize);

				HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle,
													RPC_OAPI_DSMCC_EventNotifier,
													"siiib",
													RPC_OAPI_DSMCC_EventNotifier,
													pstDsmccObject->stAccessInfo.hDsmcc,
													eDxDSMCC_EVENT_CarouselList,
													ulCarouselNum,
													pvData, ulSize);

				HLIB_SERIALIZER_Close(nSrzr);
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_Close (omDsmcc_Object_t *pstDsmccObject, Handle_t hSession)
{
	HxList_t			*pstList;
	HERROR				 hErr;

	if (pstDsmccObject->stAccessInfo.stSession.hSession != hSession)
	{
		HxLOG_Error("hSessin is not found (hSession : 0x%08x, 0x%08x) \n", hSession, pstDsmccObject->stAccessInfo.stSession.hSession);
		return ERR_FAIL;
	}

	// Stop Download
	hErr = om_dsmcc_stop_download(pstDsmccObject);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_dsmcc_stop_download err:\n");
	}

	// Free PID List
	pstList = HLIB_LIST_First(pstDsmccObject->pstCarouselList);
	for (pstList = HLIB_LIST_First(pstDsmccObject->pstCarouselList); NULL != pstList; pstList = HLIB_LIST_Next(pstList))
	{
		omDsmcc_CarouselInfo_t	*pstCarousel = (omDsmcc_CarouselInfo_t *)HLIB_LIST_Data(pstList);

		if (pstCarousel)
		{
			pstCarousel->pstPidList = om_dsmcc_pidlist_Clear(pstCarousel->pstPidList);
		}
	}
	pstDsmccObject->pstCarouselList = NULL;

	// Free Carousel List
	om_dsmcc_carousel_list_Free(&pstDsmccObject->stAccessInfo.stCarouselList);

	// Free Stream Event List
	om_dsmcc_stream_event_list_Free(&pstDsmccObject->stAccessInfo.stStreamEventList);
	pstDsmccObject->stAccessInfo.stStreamEventList.pstStreamEventInfoList = NULL;

	// Reset Session Info
	HxSTD_MemSet(&pstDsmccObject->stAccessInfo.stSession, 0xFF, sizeof(omDsmcc_Session_t));

	return ERR_OK;
}

STATIC HERROR om_dsmcc_Start (omDsmcc_Object_t *pstDsmccObject, HUINT32 ulCarouselId, Handle_t hSession, HCHAR *pszCachePath)
{
	omDsmcc_CarouselInfo_t	*pstCarouselInfo = NULL;
	HERROR					 hErr;

	hErr = om_dsmcc_PrepareDownload(pstDsmccObject->stAccessInfo.hDsmcc, ulCarouselId, &pstCarouselInfo);
	if ((ERR_OK != hErr) || (NULL == pstCarouselInfo))
	{
		HxLOG_Error("om_dsmcc_PrepareDownload err:\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("hDsmcc[0x%08x]  ulCarouselId [0x%08x]  hSession[0x%08x] pszCachePath[%s] \n", pstDsmccObject->stAccessInfo.hDsmcc, ulCarouselId, hSession, pszCachePath);

	// Add Update Cache Path
	HLIB_STD_StrNCpySafe(pstCarouselInfo->szCachePath, (const HCHAR *)pszCachePath, eDxDSMCC_MAX_PATH_LENGTH);

	om_dsmcc_carousel_pidlist_BuildOne(pstCarouselInfo, &s_stPmtPidInfo.stPidList);

#if defined(CONFIG_DEBUG)
	om_dsmcc_pmtpidlist_Print(&s_stPmtPidInfo);
	om_dsmcc_carousel_pidlist_Print(pstCarouselInfo->ulCarouselId, pstCarouselInfo->pstPidList);
#endif

	hErr = om_dsmcc_start_download(pstDsmccObject, pstCarouselInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_dsmcc_start_download err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_Restart (omDsmcc_Object_t *pstDsmccObject, HUINT32 ulCarouselId, Handle_t hSession)
{
	omDsmcc_CarouselInfo_t	*pstCarouselInfo = NULL;
	HERROR					 hErr, hResult = ERR_OK;

	hErr = om_dsmcc_carousel_map_FindByCarouselId(pstDsmccObject->pstCarouselList, ulCarouselId, &pstCarouselInfo);
	if ((ERR_OK != hErr) || (NULL == pstCarouselInfo))
	{
		HxLOG_Error("om_dsmcc_carousel_map_FindByCarouselId err: CarouselId(%d)\n", ulCarouselId);
		return ERR_FAIL;
	}

	hErr = om_dsmcc_stop_download(pstDsmccObject);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_dsmcc_stop_download err:\n");
		hResult = ERR_FAIL;
	}

	hErr = om_dsmcc_start_download(pstDsmccObject, pstCarouselInfo);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_dsmcc_start_download err:\n");
		hResult = ERR_FAIL;
	}

	NOT_USED_PARAM(hSession);
	return hResult;
}

STATIC HERROR om_dsmcc_Stop (omDsmcc_Object_t *pstDsmccObject, HUINT32 ulCarouselId, Handle_t hSession)
{
	HINT32					 nRpcHandle = 0;
	omDsmcc_CarouselInfo_t	*pstCarouselInfo = NULL;
	HERROR					 hErr;

	// Remove Session Node
	hErr = om_dsmcc_carousel_map_Remove(&pstDsmccObject->pstCarouselList, ulCarouselId, &pstCarouselInfo);
	if ((ERR_OK != hErr) || (NULL == pstCarouselInfo))
	{
		HxLOG_Error("pstCarouselInfo is NULL >>> \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("hDsmcc : [0x%08x] ulCarouselId : [0x%08x] hSession :[0x%08x]>>> \n", pstDsmccObject->stAccessInfo.hDsmcc, ulCarouselId, hSession);

	hErr = om_dsmcc_stop_download(pstDsmccObject);
	if (ERR_OK == hErr)
	{
		switch (pstDsmccObject->stAccessInfo.eDsmccType)
		{
		case eDxDSMCC_TYPE_HBBTV:
			pstCarouselInfo->pstPidList = om_dsmcc_pidlist_Clear(pstCarouselInfo->pstPidList);
			hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
			if (ERR_OK == hErr)
			{
				HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle,
													RPC_OAPI_DSMCC_EventNotifier,
													"siii",
													RPC_OAPI_DSMCC_Stop,
													pstDsmccObject->stAccessInfo.hDsmcc,
													eDxDSMCC_EVENT_Stopped,
													ulCarouselId);
			}

			break;
		default:
			break;
		}
	}

	if (NULL != pstCarouselInfo)
	{
		HLIB_STD_MemFree(pstCarouselInfo);
	}

	return ERR_OK;
}


#define _____RPC_FUNCTION_____
STATIC HERROR om_dsmcc_CmdCreateHandle (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t		 hDsmcc = HANDLE_NULL;
	HINT32			 nRpcHandle = 0;
	DxDSMCC_Type_e	 eDsmccType = eDxDSMCC_TYPE_Unknown;
	HERROR			 hErr;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
		return ERR_FAIL;
	}

	eDsmccType = (DxDSMCC_Type_e)HxOBJECT_INT(argv[0]);

	hErr = om_dsmcc_CreateHandle(eDsmccType, &hDsmcc);
	if ((ERR_OK != hErr) || (HANDLE_NULL == hDsmcc))
	{
		hErr = ERR_FAIL;
		hDsmcc = HANDLE_NULL;
	}

	HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, hErr, "i", hDsmcc);
	return hErr;
}


STATIC HERROR om_dsmcc_CmdOpen (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t			 hDsmccHandle = (Handle_t)HxOBJECT_INT(argv[0]);
	Handle_t			 hSessionHandle = (Handle_t)HxOBJECT_INT(argv[1]);
	omDsmcc_Object_t	*pstDsmccObject = NULL;
	HERROR				 hErr;

	hErr = om_dsmcc_map_FindByHandle(hDsmccHandle, &pstDsmccObject);
	if ((ERR_OK != hErr) || (NULL == pstDsmccObject))
	{
		HxLOG_Error("om_dsmcc_map_FindByHandle err:\n");
		return ERR_FAIL;
	}

	hErr = om_dsmcc_Open(pstDsmccObject, hSessionHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_dsmcc_Open err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_CmdClose (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t			 hDsmccHandle = (Handle_t)HxOBJECT_INT(argv[0]);
	Handle_t			 hSessionHandle = (Handle_t)HxOBJECT_INT(argv[1]);
	omDsmcc_Object_t	*pstDsmccObject = NULL;
	HERROR				 hErr;

	hErr = om_dsmcc_map_FindByHandle(hDsmccHandle, &pstDsmccObject);
	if ((ERR_OK != hErr) || (NULL == pstDsmccObject))
	{
		HxLOG_Error("om_dsmcc_map_FindByHandle err:\n");
		return ERR_FAIL;
	}

	hErr = om_dsmcc_Close(pstDsmccObject, hSessionHandle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_dsmcc_Close err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_CmdStart(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32				 nRpcHandle = 0;
	Handle_t			 hDsmcc = (Handle_t)HxOBJECT_INT(argv[0]);
	HUINT32				 ulCarouselId = (HUINT32)HxOBJECT_INT(argv[1]);
	Handle_t			 hSession = (Handle_t)HxOBJECT_INT(argv[2]);
	HCHAR				*pszCachePath = HxOBJECT_STR(argv[3]);
	omDsmcc_Object_t	*pstDsmccObject = NULL;
	HERROR				 hErr;

	hErr = om_dsmcc_map_FindByHandle(hDsmcc, &pstDsmccObject);
	if ((ERR_OK != hErr) || (NULL == pstDsmccObject))
	{
		HxLOG_Error("om_dsmcc_map_FindByHandle err:\n");
		return ERR_FAIL;
	}

	if ((NULL == pszCachePath) || ('\0' == *pszCachePath))
	{
		pszCachePath = "/tmp/cache";
	}

	hErr = om_dsmcc_Start(pstDsmccObject, ulCarouselId, hSession, pszCachePath);
	if (ERR_OK == hErr)
	{
		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
		if (ERR_OK == hErr)
		{
			HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle,
												RPC_OAPI_DSMCC_EventNotifier,
												"siii",
												RPC_OAPI_DSMCC_Start,
												hDsmcc,
												eDxDSMCC_EVENT_Started,
												ulCarouselId);
		}
	}
	else
	{
		HxLOG_Error("om_dsmcc_Start err: hDsmcc(0x%08x), ulCarouselId(%d), hSession(0x%08x), pszCachePath(%s)\n", hDsmcc, ulCarouselId, hSession, pszCachePath);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_CmdRestart (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t			 hDsmcc = (Handle_t)HxOBJECT_INT(argv[0]);
	HUINT32				 ulCarouselId = (HUINT32)HxOBJECT_INT(argv[1]);
	Handle_t			 hSession = (Handle_t)HxOBJECT_INT(argv[2]);
	omDsmcc_Object_t	*pstDsmccObject = NULL;
	HERROR				 hErr;

	hErr = om_dsmcc_map_FindByHandle(hDsmcc, &pstDsmccObject);
	if ((ERR_OK != hErr) || (NULL == pstDsmccObject))
	{
		HxLOG_Error("om_dsmcc_map_FindByHandle err:\n");
		return ERR_FAIL;
	}

	hErr = om_dsmcc_Restart(pstDsmccObject, ulCarouselId, hSession);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_dsmcc_Restart err: hDsmcc(0x%08x), ulCarouselId(%d), hSession(0x%08x)\n", hDsmcc, ulCarouselId, hSession);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_CmdStop (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	Handle_t			 hDsmcc = (Handle_t)HxOBJECT_INT(argv[0]);
	HUINT32				 ulCarouselId = (HUINT32)HxOBJECT_INT(argv[1]);
	Handle_t			 hSession = (Handle_t)HxOBJECT_INT(argv[2]);
	omDsmcc_Object_t	*pstDsmccObject = NULL;
	HERROR				 hErr;

	hErr = om_dsmcc_map_FindByHandle(hDsmcc, &pstDsmccObject);
	if ((ERR_OK != hErr) || (NULL == pstDsmccObject))
	{
		HxLOG_Error("om_dsmcc_map_FindByHandle err: hDsmcc(0x%08x)\n", hDsmcc);
		return ERR_FAIL;
	}

	hErr = om_dsmcc_Stop(pstDsmccObject, ulCarouselId, hSession);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("om_dsmcc_Stop err: ulCarouselId(%d), hSession(0x%08x)\n", ulCarouselId, hSession);
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR om_dsmcc_CmdRequesetStreamEvent (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR						hError;
	Handle_t						hDsmcc, hSession, hAction;
	omDsmcc_Object_t				*pstDsmccObject = NULL;
	HUINT32						ulRequestNum;
	HBOOL						bAll;

	hDsmcc 		= (Handle_t)HxOBJECT_INT(argv[0]);
	hSession		= (Handle_t)HxOBJECT_INT(argv[1]);
	bAll 			= (HBOOL)HxOBJECT_INT(argv[2]);

	if(bAll)
		ulRequestNum	= (HUINT32) 0;
	else
		ulRequestNum	= (HUINT32) 1;

	hError = om_dsmcc_map_FindByHandle(hDsmcc, &pstDsmccObject);
	if(hError !=ERR_OK || pstDsmccObject == NULL)
	{
		HxLOG_Error("om_dsmcc_map_FindByHandle() Fail : hErr [0x%x] \n", hError);
		return ERR_FAIL;
	}

	HxLOG_Debug("hDsmcc : [0x%08x]  hSession :[0x%08x]>>> \n", hDsmcc, hSession);

	NOT_USED_PARAM(ulRequestNum);
	NOT_USED_PARAM(hAction);

	hError = ERR_FAIL;
	switch (pstDsmccObject->stAccessInfo.eDsmccType)
	{
	case eDxDSMCC_TYPE_HBBTV:
		hAction = SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId);
		hError = om_dsmcc_RequestStreamEventDownload(pstDsmccObject, hAction, pstDsmccObject->stAccessInfo.stSession.usTsUniqId, ulRequestNum, &s_stPmtPidInfo);
		break;
	default:
		break;
	}

	return hError;
}

#define _____RPC_INITIALIZATION_____
STATIC HERROR om_dsmcc_InitRpcFunctions (void)
{
	HERROR			hError;
	HINT32			nRpcHandle = 0;

	hError = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hError == ERR_OK)
	{
		// APPKIT -> OBAMA
		hError = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_DSMCC_CreateHandle, "i", (HxRPC_Func_t)om_dsmcc_CmdCreateHandle, NULL,
								"Dsmcc Create Handle : (DxDSMCC_Type_e eType) ");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_DSMCC_CreateHandle, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_DSMCC_Start, "iiis", (HxRPC_Func_t)om_dsmcc_CmdStart, NULL,
								"Dsmcc Start: (Handle_t hDsmcc, Handle_t hCarousel,  Handle_t hSession, HCHAR pszCachePath)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_DSMCC_Start, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_DSMCC_ReStart, "iii", (HxRPC_Func_t)om_dsmcc_CmdRestart, NULL,
								"Dsmcc Stop: (Handle_t hDsmcc , Handle_t CarouselID, Handle_t hSession)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_DSMCC_ReStart, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_DSMCC_Stop, "iii", (HxRPC_Func_t)om_dsmcc_CmdStop, NULL,
								"Dsmcc Stop: (Handle_t hDsmcc , Handle_t CarouselID, Handle_t hSession)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_DSMCC_Stop, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_DSMCC_RequestEvent, "iii", (HxRPC_Func_t)om_dsmcc_CmdRequesetStreamEvent, NULL,
								"Dsmcc Request Event: (Handle_t hDsmcc , Handle_t hSession, HUINT32 Request Num)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_DSMCC_RequestEvent, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_DSMCC_Open, "ii", (HxRPC_Func_t)om_dsmcc_CmdOpen, NULL,
								"Dsmcc Open (Handle_t hDsmcc , Handle_t hSession)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_DSMCC_Open, hError);
		}

		hError = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_DSMCC_Close, "ii", (HxRPC_Func_t)om_dsmcc_CmdClose, NULL,
								"Dsmcc Close (Handle_t hDsmcc , Handle_t hSession)");
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_DSMCC_Close, hError);
		}

		// OBAMA -> APPKIT
		hError = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OAPI_DSMCC_EventNotifier, NULL,
								"Audio output parameters and status changed.\n"
								"\t   - (OxOutputAudio_Cache_t *pstOutputAudioStatus)\n"
								);
		if(hError != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, (%s) returned error(%d)\n", RPC_OAPI_DSMCC_EventNotifier, hError);
		}

	}



	return ERR_OK;
}

#define _____PROCEDURE_____

STATIC HERROR om_dsmcc_stream_event_UnSerializeData(DxDSMCC_StreamEvent_t *pstStreamEvent, HxObject_t *pstObject)
{
	HUINT32 ulformat = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;

	switch(ulformat)
	{
		case 15: // url + eventname + description + objectinfo

			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szDescription =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->szEventName =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->pucObjInfoByte =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 7)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 7)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 8)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 9)->u.num;

			break;

		case 14: // eventname + description + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szDescription =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->szEventName =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->pucObjInfoByte =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 8)->u.num;

			break;

		case 13:// url + description + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szDescription =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->pucObjInfoByte =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 8)->u.num;

			break;

		case 12:// description + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szDescription =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->pucObjInfoByte =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			break;

		case 11:// url + eventname + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szEventName =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->pucObjInfoByte =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 6)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 8)->u.num;

			break;

		case 10:// eventname + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szEventName =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->pucObjInfoByte =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			break;

		case 9:// url + objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->pucObjInfoByte =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			break;

		case 8:// objectinfo
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->pucObjInfoByte =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;

			break;

		case 7:// url + eventname + description
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szDescription =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->szEventName =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 5)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 8)->u.num;

			break;

		case 6:// eventname + description
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szDescription =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->szEventName =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			break;

		case 5:// url + description
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szDescription =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			break;

		case 4://  description
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szDescription =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;

			break;

		case 3:// url + eventname
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->szEventName =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 4)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 7)->u.num;

			break;

		case 2:// eventname
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->szEventName =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size);
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;

			break;

		case 1:// url
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->pszUrl =HLIB_STD_MemDup(HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data,HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size);
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 6)->u.num;

			break;

		case 0:
			pstStreamEvent->usPid = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			pstStreamEvent->ulDescLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
			pstStreamEvent->ulObjInfoByteLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.num;
			pstStreamEvent->nEventId = HxOBJECT_ARR_VAL(pstObject, 4)->u.num;
			pstStreamEvent->usTag = HxOBJECT_ARR_VAL(pstObject, 5)->u.num;

			break;

	}
	return ERR_OK;
}

STATIC BUS_Result_t proc_om_dsmcc(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	omDsmcc_Object_t 			*pstDsmccObject = NULL;
	omDsmcc_CarouselInfo_t	*pstCarouselInfo = NULL;
	HERROR					 hError;
	Handle_t					hAction;
	SvcDownload_ActionType_e	eActionType;
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	hAction = (Handle_t)hHandle;

	switch ( message )
	{
	case eMEVT_HBBTV_NOTIFY_SESSION_STARTED:
		hError = om_dsmcc_map_FindByType(eDxDSMCC_TYPE_HBBTV, &pstDsmccObject);
		if ((ERR_OK == hError) && (NULL != pstDsmccObject))
		{
			pstDsmccObject->stAccessInfo.stSession.ulActionId = SVC_PIPE_GetActionId(hHandle);
		}

		break;

	case eMEVT_HBBTV_NOTIFY_SESSION_STOPPED:
		hError = om_dsmcc_map_FindByType(eDxDSMCC_TYPE_HBBTV, &pstDsmccObject);
		if ((ERR_OK == hError) && (NULL != pstDsmccObject))
		{
			HxList_t				*pstList;
			omDsmcc_CarouselInfo_t	*pstCarousel;

			// Stop Download
			hError = om_dsmcc_stop_download(pstDsmccObject);

			// Free PID List
			pstList = HLIB_LIST_First(pstDsmccObject->pstCarouselList);
			while(pstList)
			{
				pstCarousel = (omDsmcc_CarouselInfo_t *)HLIB_LIST_Data(pstList);
				if (pstCarousel)
				{
					pstCarousel->pstPidList = om_dsmcc_pidlist_Clear(pstCarousel->pstPidList);
				}
				pstList = pstList->next;
			}
			pstDsmccObject->pstCarouselList = NULL;


			// Free Carousel List
			om_dsmcc_carousel_list_Free(&pstDsmccObject->stAccessInfo.stCarouselList);

			// Free Stream Event List
			om_dsmcc_stream_event_list_Free(&pstDsmccObject->stAccessInfo.stStreamEventList);
			pstDsmccObject->stAccessInfo.stStreamEventList.pstStreamEventInfoList = NULL;

			// Reset Session Info
			HxSTD_MemSet(&pstDsmccObject->stAccessInfo.stSession, 0xFF, sizeof(omDsmcc_Session_t));
		}

		break;

	case eSEVT_SI_PMT:
	case eSEVT_SI_PMT_PIDREMOVED:
		{
			HUINT16 	usTsUniqId;
			HBOOL	bUpdateCarouselList;

//			HxLOG_Debug("eMEVT_DSMCC_PMT_UPDATED (hAction : 0x%x) \n\n", hAction);

			usTsUniqId = 0;
			pstDsmccObject = NULL;
			hError = om_dsmcc_map_FindByType(eDxDSMCC_TYPE_HBBTV, &pstDsmccObject);
			if ((hError == ERR_OK) && (pstDsmccObject !=NULL))
			{
				bUpdateCarouselList = FALSE;

				if (hAction == SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId))
				{
					HxLOG_Debug("eMEVT_DSMCC_PMT_UPDATED (hAction : 0x%x) \n\n", hAction);
					bUpdateCarouselList = (pstDsmccObject->pstCarouselList) ? TRUE : FALSE;
					hError = om_dsmcc_pmtpidlist_Process(pstDsmccObject, hAction, bUpdateCarouselList, TRUE, &usTsUniqId);
					if (hError == ERR_OK)
					{
						pstDsmccObject->stAccessInfo.stSession.usTsUniqId = usTsUniqId;
						if (nRpcHandle)
						{
#if defined(CONFIG_DEBUG)
							HUINT8					i;
#endif
							HUINT32					ulCarouselNum;
							DxDSMCC_CarouselInfo_t	*pstCarouselList = NULL;
							HINT32 nSrzr;
							void	*pvData;
							HUINT32 ulSize;

							ulCarouselNum = pstDsmccObject->stAccessInfo.stCarouselList.ulNum;
							pstCarouselList = pstDsmccObject->stAccessInfo.stCarouselList.pstCarouselInfo;
#if defined(CONFIG_DEBUG)
							for(i=0; i < ulCarouselNum; i++)
							{
								om_dsmcc_carousel_info_Print(&pstCarouselList[i], "proc_om_dsmcc");
							}
#endif
							nSrzr = HLIB_SERIALIZER_Open();

							HLIB_SERIALIZER_MakeSerializedData(nSrzr, "b",
															(void*)pstCarouselList,
															(HINT32)(ulCarouselNum * sizeof(DxDSMCC_CarouselInfo_t)));

							pvData = HLIB_SERIALIZER_Pack(nSrzr, &ulSize);

							HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle,
																RPC_OAPI_DSMCC_EventNotifier,
																"siiib",
																RPC_OAPI_DSMCC_EventNotifier,
																pstDsmccObject->stAccessInfo.hDsmcc,
																eDxDSMCC_EVENT_CarouselList,
																ulCarouselNum,
																pvData, ulSize);

							HLIB_SERIALIZER_Close(nSrzr);
						}
					}
				}
				else
				{
					HxLOG_Error("[hAction:0x%08x] != [0x%08x-SVC_PIPE_GetActionHandle(pstObj->stAccessInfo.stSession.ulActionId)] \n", hAction, SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId));
				}
			}
			else
			{
				HxLOG_Error("DSMCC Module is Not Initialized!!!! \n");
			}
		}
		break;

	/* DSM-CC */
	case eSEVT_DOWN_DOWNLOAD_SUCCESS:		// eSEVT_DOWN_DOWNLOAD_SUCCESS
		{
			omDsmcc_PidInfo_t			*pstPidInfo;
			HUINT16 					usPid;
			HUINT32 					ulCarouselId;

			eActionType = (SvcDownload_ActionType_e)p1;
			usPid = (HUINT16)p2;
			ulCarouselId = (HUINT32)p3;

			HxLOG_Debug("EVT_PAL_DSMCC_DOWNLOAD_SUCCESS (+) :: (ActionType [%d], Pid [0x%x], CarouselId [0x%08x] )\n" , eActionType, usPid, ulCarouselId);

			if (eDOWN_DOWNLOAD_HBBTV != eActionType)
			{
				HxLOG_Error("EVT_PAL_DSMCC_DOWNLOAD_SUCCESS (-)  :: Invalid Download Type [%d]\n", eActionType);
				return MESSAGE_BREAK;
			}

			pstDsmccObject = NULL;
			hError = om_dsmcc_map_FindByType(eDxDSMCC_TYPE_HBBTV, &pstDsmccObject);
			if ((hError == ERR_OK) && (pstDsmccObject != NULL))
			{
				pstPidInfo = NULL;
				pstCarouselInfo = NULL;
				om_dsmcc_carousel_map_FindByPid(pstDsmccObject, usPid, &pstCarouselInfo, &pstPidInfo);
				if (pstCarouselInfo)
				{
					if (hAction == SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId))
					{
						HBOOL	bAllCompleted, bPrimaryCompleted;

						pstPidInfo->eStatus = eOmDsmcc_Status_Download_Success;
						bAllCompleted = bPrimaryCompleted = FALSE;

						NOT_USED_PARAM(bAllCompleted);
						NOT_USED_PARAM(bPrimaryCompleted);

						//om_dsmcc_carousel_download_CheckComplete(pstCarouselInfo, &bAllCompleted, &bPrimaryCompleted);
						//if (bAllCompleted || bPrimaryCompleted)
						{
							HxLOG_Debug("eDSMCC_STATUS_DOWNLOAD_Success\n");
							if (nRpcHandle)
							{
								HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle,
																	RPC_OAPI_DSMCC_EventNotifier,
																	"siii",
																	RPC_OAPI_DSMCC_EventNotifier,
																	pstDsmccObject->stAccessInfo.hDsmcc,
																	eDxDSMCC_EVENT_Success,
																	pstCarouselInfo->ulCarouselId);
							}
						}
					}
					else
					{
						HxLOG_Error("[0x%08x] != [0x%08x]) \n", hAction, SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId));
					}
				}
			}
			HxLOG_Debug("EVT_ePAL_DSMCC_DOWNLOAD_SUCCESS (-)\n");
		}
		return MESSAGE_BREAK;

	case eSEVT_DOWN_DOWNLOAD_CHANGED:		// eSEVT_DOWN_DOWNLOAD_CHANGED
		{
			omDsmcc_PidInfo_t				*pstPidInfo;
			HUINT16 			 			usPid;
			HUINT32 					 	ulCarouselId;

			eActionType = (SvcDownload_ActionType_e)p1;
			usPid = (HUINT16)p2;
			ulCarouselId = (HUINT32)p3;

			HxLOG_Debug("EVT_PAL_DSMCC_DOWNLOAD_CHANGED (+) ( ActionType [%d], Pid [0x%04x], CarouselId [0x%08x] )\n", eActionType, usPid, ulCarouselId);

			if (eDOWN_DOWNLOAD_HBBTV != eActionType)
			{
				HxLOG_Error("EVT_PAL_DSMCC_DOWNLOAD_CHANGED (-) :: Invalid Download Type [%d]\n", eActionType);
				return MESSAGE_BREAK;
			}

			pstDsmccObject = NULL;
			hError = om_dsmcc_map_FindByType(eDxDSMCC_TYPE_HBBTV, &pstDsmccObject);
			if ((hError == ERR_OK) && (pstDsmccObject != NULL))
			{
				pstPidInfo = NULL;
				pstCarouselInfo = NULL;

				om_dsmcc_carousel_map_FindByCarouselId(pstDsmccObject->pstCarouselList, ulCarouselId, &pstCarouselInfo);
				if (pstCarouselInfo)
				{
					if (hAction == SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId))
					{
						HxList_t				*pstPidList;
						SvcDownload_SetData_t	 stSetData;

						for (pstPidList = pstCarouselInfo->pstPidList; NULL != pstPidList; pstPidList = HLIB_LIST_Next(pstPidList))
						{
							pstPidInfo = (omDsmcc_PidInfo_t *)HLIB_LIST_Data(pstPidList);
							if (NULL != pstPidInfo)
							{
								HxSTD_MemSet(&stSetData, 0, sizeof(SvcDownload_SetData_t));

								stSetData.eSetType = eDOWN_SET_HBBTV_REMOVEPID;
								stSetData.stHbbtvRemovePid.usPid = pstPidInfo->usPid;
								stSetData.stHbbtvRemovePid.ulCarouselId = pstCarouselInfo->ulCarouselId;

								hErr = SVC_DOWNLOAD_Set(hAction, eDOWN_SET_HBBTV_REMOVEPID, &stSetData);
								if (ERR_OK == hErr)
								{
									pstPidInfo->eStatus = eOmDsmcc_Status_Download_Stop;
								}
								else
								{
									HxLOG_Error("SVC_DOWNLOAD_Set err: eDOWN_SET_HBBTV_REMOVEPID, usPid(0x%04x), ulCarouselId(%d)\n", pstPidInfo->usPid, pstCarouselInfo->ulCarouselId);
								}
							}
						}

 						HxLOG_Info("%s (%d) \n", (pstPidInfo->eStatus == eOmDsmcc_Status_Download_Stop) ? "eOmDsmcc_Status_Download_Stop" : "eOmDsmcc_Status_Init", pstPidInfo->eStatus);
						if (nRpcHandle)
						{
							HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle,
																RPC_OAPI_DSMCC_EventNotifier,
																"siii",
																RPC_OAPI_DSMCC_EventNotifier,
																pstDsmccObject->stAccessInfo.hDsmcc,
																eDxDSMCC_EVENT_Changed,
																pstCarouselInfo->ulCarouselId);
						}
					}
					else
					{
						HxLOG_Error("[0x%08x] != [0x%08x]) \n", hAction, SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId));
					}
				}
			}
			HxLOG_Debug("EVT_ePAL_DSMCC_DOWNLOAD_CHANGED (-)\n");
		}
		return MESSAGE_PASS;

	case eSEVT_DOWN_DOWNLOAD_FAIL:		// eSEVT_DOWN_DOWNLOAD_FAIL
		{
			omDsmcc_PidInfo_t				*pstPidInfo;
			HUINT16 			 			usPid;

			eActionType 	= (SvcDownload_ActionType_e)p1;
			usPid 		= (HUINT16)p2;

			HxLOG_Debug("eMEVT_DSMCC_DOWNLOAD_FAIL (hAction : 0x%x, ActionType : %d, usPid : 0x%04x) \n", hAction, eActionType,  usPid);

			if (eDOWN_DOWNLOAD_HBBTV != eActionType)
			{
				HxLOG_Error("eMEVT_DSMCC_DOWNLOAD_FAIL :: Invalid Download Type [%d] (-) \n", eActionType);
				return MESSAGE_PASS;
			}

			pstDsmccObject = NULL;
			hError = om_dsmcc_map_FindByType(eDxDSMCC_TYPE_HBBTV, &pstDsmccObject);
			if ((hError == ERR_OK) && (pstDsmccObject != NULL))
			{
				pstPidInfo = NULL;
				om_dsmcc_carousel_map_FindByPid(pstDsmccObject, usPid, &pstCarouselInfo, &pstPidInfo);
				if (pstCarouselInfo)
				{
					if (hAction == SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId))
					{
						pstPidInfo->eStatus = eOmDsmcc_Status_Download_Fail;
						if (nRpcHandle)
						{
							HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle,
																RPC_OAPI_DSMCC_EventNotifier,
																"siii",
																RPC_OAPI_DSMCC_EventNotifier,
																pstDsmccObject->stAccessInfo.hDsmcc,
																eDxDSMCC_EVENT_Fail,
																pstCarouselInfo->ulCarouselId);
						}
					}
					else
					{
						HxLOG_Error("[0x%08x] != [0x%08x]) \n", hAction, SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId));
					}
				}
			}

			HxLOG_Debug("eMEVT_DSMCC_DOWNLOAD_FAIL (-)\n");
		}
		return MESSAGE_PASS;

	case eSEVT_DOWN_DSMCC_EVENT_FOUND:			// eSEVT_DOWN_DSMCC_EVENT_FOUND
	case eSEVT_DOWN_DSMCC_STREAM_DESCRIPTOR_EVENT: 		// eSEVT_DOWN_DSMCC_STREAM_DESCRIPTOR_EVENT
		{
			// svc download hbbtv에서 serialize 된 data를 oapi로 바로 보내면 될 것 같은데, 기존 코드에 om의 list를 전체로 보내는 내용이 존재하여 (왜 ??)
			// 여기서 한번 풀고, list에 추가후 다시 serialize 해서 보낸다. 추후 svc에서 보낸것을 by pass 하게 고치려면, svc단에서 encode 64도 추가.
			HUINT16 					 usPid;
			DxDSMCC_StreamEvent_t	*pstEvent = NULL;
			HUINT32 				ulSize;
			void				*pvData = (void*)p3;
			HxObject_t		*pstObject;

			eActionType 	= (SvcDownload_ActionType_e)p1;
			ulSize		= p2;

			if(pvData == NULL || ulSize == 0)
			{
				HxLOG_Error("invalid data \n");
				return MESSAGE_BREAK;
			}

			if (eDOWN_DOWNLOAD_HBBTV != eActionType)
			{
				HxLOG_Error("eDOWN_DOWNLOAD_HBBTV (eActionType : %s)(-)\n", (message == eSEVT_DOWN_DSMCC_EVENT_FOUND) ? "eSEVT_DOWN_DSMCC_EVENT_FOUND" : "eSEVT_DOWN_DSMCC_STREAM_DESCRIPTOR_EVENT");
				return MESSAGE_BREAK;
			}

			pstObject = HLIB_SERIALIZER_Unpack(pvData, ulSize);

			if (pstObject == NULL)
			{
				HxLOG_Error("cannot unpack the data \n");
				return MESSAGE_BREAK;
			}

			pstEvent = HLIB_STD_MemCalloc(sizeof(DxDSMCC_StreamEvent_t));
			if (NULL == pstEvent)
			{
				HxLOG_Error("alloc fail. \n");
				return MESSAGE_BREAK;
			}

			om_dsmcc_stream_event_UnSerializeData(pstEvent, pstObject);


			usPid = pstEvent->usPid;

			HxLOG_Debug("%s (hAction : 0x%x, ActionType : %d, usPid : 0x%04x) \n", message ==eSEVT_DOWN_DSMCC_EVENT_FOUND ? "eSEVT_DOWN_DSMCC_EVENT_FOUND" : "eSEVT_DOWN_DSMCC_STREAM_DESCRIPTOR_EVENT", hAction, eActionType,  usPid);


			HxLOG_Debug("EventId : [0x%04x] , szEventName : [%s] \n", pstEvent->nEventId, pstEvent->szEventName);

			pstDsmccObject = NULL;
			hError = om_dsmcc_map_FindByType(eDxDSMCC_TYPE_HBBTV, &pstDsmccObject);
			if ((hError == ERR_OK) && (pstDsmccObject != NULL))
			{
				if (hAction == SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId))
				{
					DxDSMCC_Event_e				eDxDsmccEvent = eDxDSMCC_EVENT_Unknown;

#if defined(CONFIG_DEBUG)
					om_dsmcc_stream_event_Print(pstEvent, "\t[StreamEvent:EventProc1]");
#endif

					if (nRpcHandle)
					{
						omDsmcc_StreamEvtList_t  	*pstDsmccStreamEvtLst = NULL;
						DxDSMCC_StreamEvent_t	*pstStreamEventList = NULL;
						HUINT32					ulEsNum;

						if(message == eSEVT_DOWN_DSMCC_EVENT_FOUND)
							eDxDsmccEvent = eDxDSMCC_EVENT_StreamEvtList;
						else if(message == eSEVT_DOWN_DSMCC_STREAM_DESCRIPTOR_EVENT)
							eDxDsmccEvent = eDxDSMCC_EVENT_StreamDescriptorEvt;

						NOT_USED_PARAM(pstDsmccStreamEvtLst);
						NOT_USED_PARAM(pstStreamEventList);
						NOT_USED_PARAM(ulEsNum);

						{
							HINT32 nSrzr = 0;
							void	*pvData = NULL;
							HUINT32 ulSize;
							HUINT32 ulDataFmat;
							DxDSMCC_StreamEvent_t		*pstStreamEvent = NULL;

							pstStreamEvent = (DxDSMCC_StreamEvent_t *)HLIB_STD_MemAlloc(sizeof(DxDSMCC_StreamEvent_t));
							if (NULL == pstStreamEvent)
							{
								HxLOG_Error("NULL == pstStreamEvent \n");
								return ERR_FAIL;
							}

							// Add Stream Event List to Session Map
 							HxSTD_MemSet(pstStreamEvent, 0, sizeof(DxDSMCC_StreamEvent_t));
							om_dsmcc_stream_event_Convert(pstEvent, pstStreamEvent, eOmDsmcc_Event_Update_All);

#if defined(CONFIG_DEBUG)
							om_dsmcc_stream_event_Print(pstStreamEvent, "\t[StreamEvent:EventProc2]");
#endif

							nSrzr = HLIB_SERIALIZER_Open();
							om_dsmcc_stream_event_DataCheck(pstStreamEvent,&ulDataFmat);
							om_dsmcc_stream_event_SerializeData(pstStreamEvent, nSrzr, ulDataFmat, NULL, 0);
							pvData = HLIB_SERIALIZER_Pack(nSrzr, &ulSize);

							HLIB_RPC_NotifyWithoutSignatureCheck(nRpcHandle,
																RPC_OAPI_DSMCC_EventNotifier,
																"siiib",
																RPC_OAPI_DSMCC_EventNotifier,
																pstDsmccObject->stAccessInfo.hDsmcc,
																eDxDsmccEvent,
																1,
																pvData, ulSize);
							HLIB_SERIALIZER_Close(nSrzr);
							HLIB_STD_MemFree(pstStreamEvent);
						}
					}
				}
				else
				{
					HxLOG_Error("0x%08x] != [0x%08x]) \n", hAction, SVC_PIPE_GetActionHandle(pstDsmccObject->stAccessInfo.stSession.ulActionId));
				}
			}

			HxLOG_Debug("%s (-)\n\n\n\n", (message == eSEVT_DOWN_DSMCC_EVENT_FOUND) ? "eSEVT_DOWN_DSMCC_EVENT_FOUND" : "eSEVT_DOWN_DSMCC_STREAM_DESCRIPTOR_EVENT");

			if (NULL != pstEvent)
			{
				HLIB_STD_MemFree(pstEvent);
				pstEvent = NULL;
			}
		}

		return MESSAGE_BREAK;

	default:
		break;
	}

	return MESSAGE_PASS;
}


#define ___GLOBAL_FUNCTIONS___

HERROR OM_DSMCC_Init(void)
{
	HERROR	hError;

	HxLOG_Debug("[%s] (+) \n", __FUNCTION__);

	om_dsmcc_Init();

	// Create event handler proc
	BUS_MGR_Create ("proc_om_dsmcc", APP_ACTION_PRIORITY, proc_om_dsmcc, 0, 0, 0, 0);

	hError = om_dsmcc_InitRpcFunctions();
	if(hError != ERR_OK)
	{
		HxLOG_Error("\n[%s] om_dsmcc_InitRpcFunctions returned error(%d)\n",  __FUNCTION__, hError);
	}

	HxLOG_Debug("[%s] (-) \n", __FUNCTION__);

	return ERR_OK;
}

