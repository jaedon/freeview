/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_search_core_action_nordig.c
	@brief

	Description:  									\n
	Module: plugin\xtvmanager\xmgr\search\nordig	\n

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
#include <octo_common.h>


#include <mgr_action.h>
#include <bus.h>
#include <svc_pipe.h>
#include <db_common.h>
#include <db_svc.h>
#include <db_param.h>
#include <linkedlist.h>
#include <svc_si.h>
#include <svc_sys.h>
#include <svc_resources.h>

#include <isosvc.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_search.h>
#include <mgr_spec.h>
#include <mgr_tplist.h>

#include <mgr_appmgr.h>
#include <mgr_rscmap.h>

#include "../local_include/_xmgr_search.h"

#include <otl.h>
#include <hapi.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




//#define USE_SAVE_TIME_TRACE
#if defined(CONFIG_DEBUG) && defined(USE_SAVE_TIME_TRACE)
STATIC HUINT32 s_ulSaveTraceStartTime, s_ulSaveTraceEndTime;
STATIC HUINT32 s_ulSearchStart;
#define __save_time_trace_reset__	{ s_ulSaveTraceEndTime = VK_TIMER_GetSystemTick(); }
#define __save_time_trace_check__                                                           \
	{                                                                                       \
		HUINT32 ulDur;                                                                      \
		s_ulSaveTraceStartTime = VK_TIMER_GetSystemTick();                                  \
		ulDur = s_ulSaveTraceStartTime - s_ulSaveTraceEndTime;                              \
		s_ulSaveTraceEndTime = s_ulSaveTraceStartTime;                                      \
		HxLOG_Print("\t$$ [%s:%d] TIME TRACE - %d msec\n", __FUNCTION__, __LINE__, ulDur);     \
	}
#define __search_time_trace_start__                                                         \
	{                                                                                       \
		s_ulSearchStart = VK_TIMER_GetSystemTick();                                         \
		HxLOG_Print("\n####################################################\n");               \
		HxLOG_Print("    == AP SEARCH TIME TRACE START\n");                                    \
		HxLOG_Print("####################################################\n\n");               \
	}
#define __search_time_trace_check__                                                         \
	{                                                                                       \
		HUINT32		ulDur;                                                                  \
		ulDur = VK_TIMER_GetSystemTick();                                                   \
		ulDur -= s_ulSearchStart;                                                           \
		HxLOG_Print("\n##########################################################\n");         \
		HxLOG_Print("  [%s:%d] SEARCH TIME CHECK\n", __FUNCTION__, __LINE__);                  \
		HxLOG_Print("      --> (state = %d, duration = %d sec)\n", pstContents->eAppState, ulDur / 1000); \
		HxLOG_Print("##########################################################\n\n");         \
	}
#else
#define __save_time_trace_reset__
#define __save_time_trace_check__
#define __search_time_trace_start__
#define __search_time_trace_check__
#endif

#define SEARCH_TIMER_LOCKING_ID						0x2001
#define SEARCH_TIMER_SI_ID							0x2002
#define SEARCH_TIMER_LOCKING_TIME					(4*1000)	/* 4 sec */
#define SEARCH_TIMER_SI_TIME						(60*1000)	/* 60 sec */

#define TDC_TEMP_NID								100

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief search application에서 실행에 필요한 environment context  */
typedef struct tagNordigSearch_Contents_t
{
	HUINT32						 ulActionId;			// Search Application Action ID
	Handle_t					 hAction;				// Search Action handle
	HUINT32						 ulTunerId;				// Tuner ID

	// Search Options :
	DxCasType_e					 eCAFreeType;			//  ALL, FTA, CAS
	DxSvcType_e					 eServiceType;			// ALL, TV, Radio
	HBOOL						 bNetworkSearch;		// TRUE, FALSE
	HUINT16						 usNetworkId;
	HBOOL						 bIsOpenCable;			// open cable - linear search 를 별도의 방법으로 함
	HINT32						 lSearchType;			// SvcSi_SearchType_e
	HBOOL						 bReplaceChannel;
	HBOOL						 bAutoMode;				// if not found in automatic search, add linear tp list

	MgrSearch_AppState_e		 eAppState;				// Current Search State :
	HBOOL						 bSvcSaved;				// Searched Services are saved:
	HBOOL						 bNitTpList;			// Use TP list from NIT after 1 TP is locked from Home TP

	// == TP List to be searched:
	POINTER_LIST_T				*pstTpToSearch;
	POINTER_LIST_T				*pstFinishedTps;

	// == Searched Compontns ==
	MgrSearch_PrvInfoArray_t	 stPrvArray;
	MgrSearch_NetInfoArray_t	 stNetArray;
	MgrSearch_TsInfoArray_t		 stTsArray;
	MgrSearch_BqInfoArray_t		 stBqArray;

	// === Tuning Informations ===
	DxTuneParam_t				 stSearchTuningInfo;

	// === Searched Service Numbers
	HUINT32						 ulNumOfTvSearched;				// Number of TV searched
	HUINT32						 ulNumOfRadioSearched;			// Number of Radio searched
	HUINT32						 ulNumOfDataSearched;			// Number of Data searched
} NordigSearch_Contents_t;

/*******************************************************************/
/********************      Local variable         *************************/
/*******************************************************************/

STATIC NordigSearch_Contents_t	 s_NordigSrchApp_Contents[NUM_SEARCH_ACTION];				/**< Search Appl. runtime context */


#define	____________________for__UI_____________________

STATIC HERROR xmgr_nordigsrch_ChangeTpText (NordigSearch_Contents_t *pstContents, DbSvc_NetInfo_t *pstNetworkInfo)
{
	DbSvc_TuningInfo_t	 stTuneInfo;

//	MWC_UTIL_ConvCh2DbTuningParam (&(pstContents->stSearchTuningInfo), &(stTuneInfo));
	OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&(pstContents->stSearchTuningInfo), &(stTuneInfo));

	MGR_SEARCHRESULT_SetTransponder (pstContents->ulActionId, pstContents->stSearchTuningInfo.cab.deliveryType, stTuneInfo);
	if (NULL != pstNetworkInfo)
	{
		MGR_SEARCHRESULT_SetNetwork (pstContents->ulActionId, pstNetworkInfo);
	}

	return ERR_OK;
}

STATIC HERROR xmgr_nordigsrch_ChangeProgressNumber (NordigSearch_Contents_t *pstContents)
{
	HUINT32		 ulNumFinishedTp, ulNumLeftTp;
	HUINT32		 ulTotalNumTp;

__search_time_trace_check__;

	ulNumFinishedTp = UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstContents->pstFinishedTps, NULL, NULL);
	ulNumLeftTp 	= UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstContents->pstTpToSearch, NULL, NULL);
	ulTotalNumTp	= ulNumFinishedTp + ulNumLeftTp;

	MGR_SEARCHRESULT_SetNumTpCompleted (pstContents->ulActionId, ulNumFinishedTp);
	MGR_SEARCHRESULT_SetNumTpTotal (pstContents->ulActionId, ulTotalNumTp);

	return ERR_OK;
}

#define ____DERIVATION___________________

STATIC SvcSi_SearchType_e xmgr_nordigsrch_GetSearchType(HBOOL bNetworkSearch, HUINT16 usNetworkId)
{
	if(TRUE == bNetworkSearch)
	{
		// (1) Network id 가 있는 경우
		if((0 < usNetworkId) && (0xFFFF > usNetworkId))
		{
			return eSVCSI_SEARCHTYPE_NORDIG_NitOthBased;
		}
		// (2) Network id 가 0인 경우
		else
		{
			return eSVCSI_SEARCHTYPE_NORDIG_NitActBased;
		}
	}
	// (3) Humax UI의 Network Off 경우
	else
	{
#if defined(CONFIG_OP_TELENOR)
		// Telenor에서는 LCD 처리를 위해서 NIT를 무조건 처리 해야 한다.
		return eSVCSI_SEARCHTYPE_NORDIG_NitActBased;
#elif defined(CONFIG_OP_TDC)
		/*
			TDC Rules of Operation

			7.4 Manual frequency scan
			In addition to the NIT-based channel search described in the previous sections, the CPE menu can
			offer a way to perform a single TS channel scan installing all channels listed in the service loop of the
			SDT, disregarding any information in the NIT.
		*/
		return eSVCSI_SEARCHTYPE_NORDIG_WithoutNit;
#else
		return eSVCSI_SEARCHTYPE_NORDIG_NitActBased;
#endif
	}
}


#define _________________________________________________________________________________

STATIC INLINE HERROR xmgr_nordigsrch_CheckActionId (NordigSearch_Contents_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);

	if (pstContents->ulActionId != ulActionId)
	{
		HxLOG_Info("is not matched actionID(0x%x)! line(%d)\n", ulActionId, __LINE__);
		return MESSAGE_PASS;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC INLINE void xmgr_nordigsrch_IncreaseSessionVersion (NordigSearch_Contents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

STATIC DbSvc_TuningInfo_t *xmgr_nordigsrch_GetCurrentTuningInfo (NordigSearch_Contents_t *pstContents)
{
	if (pstContents != NULL)
	{
		if (pstContents->pstTpToSearch != NULL)
		{
			return (DbSvc_TuningInfo_t *)pstContents->pstTpToSearch->pvContents;
		}
	}

	return NULL;
}

STATIC HUINT16 xmgr_nordigsrch_GetUniqueTsIndex (HUINT16 usOnId, HUINT16 usTsId)
{
	HINT32					nTsNumInDb;
	DbSvc_TsCondition_t		stTsCond;
	DbSvc_TsInfo_t			*pstTsInfoInDb;
	HUINT16					usTsIdx = 0xFF;
	HERROR					hErr;

	nTsNumInDb = 0;
	pstTsInfoInDb = NULL;

	DB_SVC_InitTsFindCondition (&stTsCond);
	stTsCond.nDeliType	= eDxDELIVERY_TYPE_CAB;
	stTsCond.nOnId		= (HINT32)usOnId;
	stTsCond.nTsId		= (HINT32)usTsId;

	hErr = DB_SVC_FindTsInfoList (&stTsCond, &nTsNumInDb, &pstTsInfoInDb);
	if (hErr == ERR_OK && nTsNumInDb > 0 && pstTsInfoInDb != NULL)
	{
		usTsIdx = pstTsInfoInDb[0].usUniqueId;
	}

	if(pstTsInfoInDb != NULL)
	{
		DB_SVC_FreeTsInfoList(pstTsInfoInDb);
	}

	return usTsIdx;
}

STATIC DbSvc_TuningInfo_t *xmgr_nordigsrch_AllocTuneInfo (void)
{
	return (DbSvc_TuningInfo_t *)OxAP_Malloc (sizeof(DbSvc_TuningInfo_t));
}

STATIC void xmgr_nordigsrch_FreeTuneInfo (void *pvTuneInfo)
{
	if (pvTuneInfo)
	{
		OxAP_Free ((DbSvc_TuningInfo_t *)pvTuneInfo);
	}
}

STATIC HERROR xmgr_nordigsrch_GetServiceHandle (HUINT16 usTsIdx, HUINT16 usTsId, HUINT16 usSvcId, HUINT16 usOnId, Handle_t *phSvc)
{
	HERROR			 hErr = ERR_FAIL;

	hErr = DB_SVC_GetServiceHandle (eDxSvcSection_All, usTsIdx, usSvcId, phSvc);

	return hErr;
}

STATIC HBOOL xmgr_nordigsrch_FilterSameTuningInfo (void *pvItem, void *pvFilter)
{
	DbSvc_TuningInfo_t *pstItem, *pstFilter;

	pstItem = (DbSvc_TuningInfo_t *)pvItem;
	pstFilter = (DbSvc_TuningInfo_t *)pvFilter;

	if (DB_SVC_UtilCompareDbTuningParam(eDxDELIVERY_TYPE_CAB, pstItem, pstFilter) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

#define	_____MESSAGE_FUNCTIONS_____

// UI 에게 명령을 내리는 Message
STATIC HERROR xmgr_nordigsrch_SendUpdateDisplayMsg (NordigSearch_Contents_t *pstContents)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_STATUS_CHANGED, (Handle_t)pstContents->hAction, pstContents->stSearchTuningInfo.cab.ulFrequency, 0, 0);
}

// UI 에게 상태를 알려주는 Message
STATIC HERROR xmgr_nordigsrch_SendSearchStartedMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_STARTED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

STATIC HERROR xmgr_nordigsrch_SendSearchFinished (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_FINISHED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

STATIC HERROR xmgr_nordigsrch_SendSearchSaveFinishedMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_SAVE_FINISHED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

STATIC HERROR xmgr_nordigsrch_SendChannelListUpdatedMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_CHANNEL_UPDATED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

STATIC HERROR xmgr_nordigsrch_SendSearchActionDestroyed (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_ACTION_DESTROYED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

#define	_____CONTENTS_INITIATION_____

STATIC HERROR xmgr_nordigsrch_UseAutoMode(DxOperator_e eOpType)
{
	switch(eOpType)
	{
	case eDxOPERATOR_Tdc:
	case eDxOPERATOR_Telenor:
		return TRUE;

	default:
		return FALSE;
	}
}

STATIC HERROR xmgr_nordigsrch_UseNitTpList(DxOperator_e eOpType)
{
	switch(eOpType)
	{
	case eDxOPERATOR_Tdc:
	case eDxOPERATOR_Telenor:
		return TRUE;

	default:
		return FALSE;
	}
}

STATIC HERROR xmgr_nordigsrch_MakeTpList (NordigSearch_Contents_t* pstContents, MgrSearch_Option_t *pstOption)
{
	HUINT32					 ulCount;
	HUINT32					 ulNumTs = 0;
	DbSvc_TsInfo_t			*astTsInfo = NULL;
	DbSvc_TuningInfo_t		*pstTuneInfo;
	HERROR					 hErr;
	DxOperator_e		 	 eOpType = eDxOPERATOR_None;

	if (pstContents == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	// Initialize the TP List
	pstContents->pstTpToSearch = NULL;
	pstContents->pstFinishedTps = NULL;

	HxLOG_Print("pstOption->ulTpNum = %d\n", pstOption->ulTpNum);
	if ((0 == pstOption->ulTpNum) || (NULL == pstOption->pstTpArray))
	{
		MGR_SPEC_GetOperatorCode(&eOpType);
		HxLOG_Print("eOpType = %d\n", eOpType);

		pstContents->bAutoMode = xmgr_nordigsrch_UseAutoMode(eOpType);
		pstContents->bNitTpList = xmgr_nordigsrch_UseNitTpList(eOpType);

		hErr = MGR_TPLIST_GetCabSearchTsList(eOpType, eTPLIST_TYPE_AUTO, &astTsInfo, &ulNumTs);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("MGR_TPLIST_GetCabSearchTsList failed. opType(%d) \n",eOpType);
		}

		if (NULL == astTsInfo || 0 == ulNumTs)
		{
			return ERR_FAIL;
		}

		HxLOG_Print("ulNumTs = %d\n", ulNumTs);

		for (ulCount = 0; ulCount < ulNumTs; ulCount++)
		{
			pstTuneInfo = (DbSvc_TuningInfo_t *)xmgr_nordigsrch_AllocTuneInfo();
			if (NULL != pstTuneInfo)
			{
				*pstTuneInfo = astTsInfo[ulCount].stTuningParam;
				pstContents->usNetworkId = pstTuneInfo->cab.usNetworkId;
				if(pstTuneInfo->cab.ulFreq == 0)	/* skip */
				{
					xmgr_nordigsrch_FreeTuneInfo(pstTuneInfo);
					continue;
				}
				else
				{
					pstContents->pstTpToSearch = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTpToSearch, (void *)pstTuneInfo);
					UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstTpToSearch);
				}
				HxLOG_Print("(%d) NetID [%d], %d MHz, %d\n", ulCount, pstTuneInfo->cab.usNetworkId, pstTuneInfo->cab.ulFreq / 1000, pstTuneInfo->cab.usSR);
			}
		}

		if(NULL != astTsInfo)
		{
			HLIB_STD_MemFree(astTsInfo);
		}
	}
	else
	{
#if defined(CONFIG_OP_TDC)
		if(pstOption->bNetworkSearch == TRUE && pstContents->lSearchType == eSVCSI_SEARCHTYPE_NORDIG_NitOthBased)
		{
			// If TDC NIT automatic search, use AutoMode and NitTpList
			MGR_SPEC_GetOperatorCode(&eOpType);
			pstContents->bAutoMode = xmgr_nordigsrch_UseAutoMode(eOpType);
			pstContents->bNitTpList = xmgr_nordigsrch_UseNitTpList(eOpType);
		}
#endif
		/* Hidden/Menu에 의한  검색 TP List 작성  */
		for (ulCount = 0; ulCount < pstOption->ulTpNum; ulCount++)
		{
			pstTuneInfo = (DbSvc_TuningInfo_t *)xmgr_nordigsrch_AllocTuneInfo();
			if (NULL != pstTuneInfo)
			{
				*pstTuneInfo = pstOption->pstTpArray[ulCount].stTuningParam;
				pstContents->usNetworkId = pstTuneInfo->cab.usNetworkId;
				if(pstTuneInfo->cab.ulFreq == 0)	/* skip */
				{
					xmgr_nordigsrch_FreeTuneInfo(pstTuneInfo);
					continue;
				}
				else
				{
					pstContents->pstTpToSearch = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTpToSearch, (void *)pstTuneInfo);
					UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstTpToSearch);
				}
				HxLOG_Print("(%d) NetID [%d], %d MHz, %d\n", ulCount, pstTuneInfo->cab.usNetworkId, pstTuneInfo->cab.ulFreq / 1000, pstTuneInfo->cab.usSR);
			}
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_nordigsrch_MakeLinearTpList (NordigSearch_Contents_t* pstContents)
{
	HUINT32					 ulCount;
	HUINT32					 ulNumTs = 0;
	DbSvc_TsInfo_t			*astTsInfo = NULL;
	DbSvc_TuningInfo_t		*pstTuneInfo;
	DxOperator_e		 	 eOpType = eDxOPERATOR_None;

	if (pstContents == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	MGR_SPEC_GetOperatorCode(&eOpType);
	// TODO: default 값 제대로 들어가면, 아래 코드 지울 것.
#ifdef CONFIG_OP_TELENOR
	eOpType = eDxOPERATOR_Telenor;
#endif

	(void)MGR_TPLIST_GetCabSearchTsList(eOpType, eTPLIST_TYPE_LINEAR, &astTsInfo, &ulNumTs);
	if (NULL == astTsInfo || 0 == ulNumTs)
	{
		return ERR_FAIL;
	}

	HxLOG_Print("ulNumTs = %d\n", ulNumTs);

	for (ulCount = 0; ulCount < ulNumTs; ulCount++)
	{
		pstTuneInfo = (DbSvc_TuningInfo_t *)xmgr_nordigsrch_AllocTuneInfo();
		if (NULL != pstTuneInfo)
		{
			*pstTuneInfo = astTsInfo[ulCount].stTuningParam;
			pstContents->usNetworkId = pstTuneInfo->cab.usNetworkId;
			if(pstTuneInfo->cab.ulFreq == 0)	/* skip */
			{
				xmgr_nordigsrch_FreeTuneInfo(pstTuneInfo);
				continue;
			}
			else
			{
				pstContents->pstTpToSearch = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTpToSearch, (void *)pstTuneInfo);
				UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstTpToSearch);
			}
			HxLOG_Print("(%d) NetID [%d], %d MHz, %d\n", ulCount, pstTuneInfo->cab.usNetworkId, pstTuneInfo->cab.ulFreq / 1000, pstTuneInfo->cab.usSR);
		}
	}

	if(NULL != astTsInfo)
	{
		HLIB_STD_MemFree(astTsInfo);
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_nordigsrch_InitContents (NordigSearch_Contents_t *pstContents, Handle_t hAction, MgrSearch_Option_t *pstOption, HINT32 lSearchType)
{
	// 1. Clear all the contents.
	HxSTD_memset (pstContents, 0, sizeof(NordigSearch_Contents_t));

	// 2. Action Handle and Action ID
	pstContents->ulTunerId = (HUINT32)-1;
	pstContents->ulActionId = MGR_ACTION_GetMainActionId();
	pstContents->hAction	= SVC_PIPE_GetActionHandle (pstContents->ulActionId);

	pstContents->bSvcSaved = FALSE;

	// 3. Init the search result components:
	MGR_SEARCHDATA_InitProviderArray (&(pstContents->stPrvArray));
	MGR_SEARCHDATA_InitNetworkArray (&(pstContents->stNetArray));
	MGR_SEARCHDATA_InitTsArray (&(pstContents->stTsArray));
	MGR_SEARCHDATA_InitBouquetArray (&(pstContents->stBqArray));

	// 4. Search Options
	pstContents->eCAFreeType	= pstOption->eCasType;				// Scramble On/Off
	pstContents->eServiceType	= pstOption->eSvcType;				// Channel Type
	pstContents->bNetworkSearch	= pstOption->bNetworkSearch;		// Network Search
	pstContents->bReplaceChannel = pstOption->bReplaceChannel;		// Existing Channels remove


	// NIT Search - Act/Oth
	pstContents->lSearchType = xmgr_nordigsrch_GetSearchType(pstOption->bNetworkSearch, pstOption->usNetwork_id);

	HxLOG_Print(HxANSI_COLOR_YELLOW("bNetworkSearch = %d (NID:%d), pstContents->lSearchType = %d\n"), pstContents->bNetworkSearch, pstOption->usNetwork_id, pstContents->lSearchType);
	HxLOG_Print(HxANSI_COLOR_YELLOW("bReplaceChannel = %d \n"), pstContents->bReplaceChannel);

	// 5-1. TP List :
	xmgr_nordigsrch_MakeTpList(pstContents, pstOption);

	if(pstContents->bReplaceChannel == TRUE)
	{
		// TODO: 향별로 remove에 대한 시나리오 적용
		// Remove All Service
		DB_SVC_DeleteAllServiceInfo();
		DB_SVC_DeleteAllTsInfo();
		DB_SVC_DeleteAllProviderInfo();
		DB_SVC_DeleteAllNetworkInfo();
		DB_SVC_DeleteAllBouquetInfo();
	}

	/* 검색 목적이 아닌 기존 채널을 지우기 위해 search action 이용 - 에러 상황 아님. */
	if (pstContents->pstTpToSearch == NULL)
	{
		MGR_SEARCH_InitResult(pstContents->ulActionId);				/* 기존 검색 결과를 clear 해줌. */
		MGR_SEARCHRESULT_SetNumTpCompleted (pstContents->ulActionId, 1);	/* search succcess 처리하기 위해(meta svc save 하기 위함.) complete 설정 */
		MGR_SEARCHRESULT_SetNumTpTotal (pstContents->ulActionId, 1);		/* search succcess 처리하기 위해(meta svc save 하기 위함.) complete 설정 */
		xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
		BUS_MGR_Destroy (NULL);
		return ERR_BUS_NO_ERROR;
	}

	return ERR_BUS_NO_ERROR;
}

#define	_____CONTENTS_MODIFICATION_____

STATIC HERROR xmgr_nordigsrch_SetTuningParamToSearch (NordigSearch_Contents_t *pstContents)
{
	DbSvc_TsInfo_t		 stSvcTsInfo = {0};
	DbSvc_TuningInfo_t	*pstCurrTp;

	// No more TP to be searched
	if (pstContents->pstTpToSearch == NULL)
	{
		// Finish the search
		pstContents->eAppState = eSearchApp_state_searchend;
		HxLOG_Info("pstTpToSearch is NULL! line(%d)\n", __LINE__);
		return ERR_BUS_NO_OBJECT;
	}

	pstContents->eAppState = eSearchApp_state_idle;

	// Get the first TP for tuning :
	pstCurrTp = (DbSvc_TuningInfo_t *)(pstContents->pstTpToSearch->pvContents);
	if (pstCurrTp == NULL)
	{
		// Finish the search
		pstContents->eAppState = eSearchApp_state_searchend;
		HxLOG_Info("pstCurrTp is NULL! line(%d)\n", __LINE__);
		return ERR_BUS_NO_OBJECT;
	}

	pstContents->usNetworkId = pstCurrTp->cab.usNetworkId;

	// current Tp로 튜닝info 채워 준다.
	stSvcTsInfo.eDeliType = eDxDELIVERY_TYPE_CAB;
	stSvcTsInfo.ucTunerId = (HUINT8)pstContents->ulTunerId;
	HxSTD_memcpy (&(stSvcTsInfo.stTuningParam), pstCurrTp, sizeof(DbSvc_TuningInfo_t));

	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stSvcTsInfo, &(pstContents->stSearchTuningInfo));

	HxLOG_Print("eDeliveryType:%d\n", pstContents->stSearchTuningInfo.cab.deliveryType);

	/* Tp info 변경 */
	xmgr_nordigsrch_ChangeTpText (pstContents, NULL);

	/* status bar update */
	xmgr_nordigsrch_ChangeProgressNumber (pstContents);

	xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);

	return ERR_BUS_NO_ERROR;
}





#define	_____CONTENTS_DESTRUCTION_____

STATIC HERROR xmgr_nordigsrch_DestroyContents (NordigSearch_Contents_t *pstContents)
{
	// Clear the search results :
	MGR_SEARCHDATA_ClearProviderArray (&(pstContents->stPrvArray));
	MGR_SEARCHDATA_ClearNetworkArray (&(pstContents->stNetArray));
	MGR_SEARCHDATA_ClearTsArray (&(pstContents->stTsArray));
	MGR_SEARCHDATA_ClearBouquetArray (&(pstContents->stBqArray));

 	pstContents->pstTpToSearch = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTpToSearch, xmgr_nordigsrch_FreeTuneInfo);
 	pstContents->pstFinishedTps = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstFinishedTps, xmgr_nordigsrch_FreeTuneInfo);

	return ERR_BUS_NO_ERROR;
 }


#define	_____DATA_CONVERSION_____



#define	_____STATE_PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_nordigsrch_SetNextTpToSearch (NordigSearch_Contents_t *pstContents)
{
	POINTER_LIST_T		*pstSrchTps, *pstCompTps;
	DbSvc_TuningInfo_t	*pstTuneInfo;

	pstSrchTps = pstContents->pstTpToSearch;
	pstCompTps = pstContents->pstFinishedTps;

	if (pstSrchTps == NULL)					{ return ERR_BUS_NO_OBJECT; }

	// 맨 앞에 있는 것이 현재 TP 이다.
	pstTuneInfo = (DbSvc_TuningInfo_t *)pstSrchTps->pvContents;
	if (pstTuneInfo == NULL)				{ return ERR_BUS_NO_OBJECT; }

	// 현재 TP를 Search TP list에서 삭제하고, 대신 Completed Tp List에 추가한다.
	pstSrchTps = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstSrchTps, pstSrchTps, NULL);
	UTIL_LINKEDLIST_CircularizePointerList (pstSrchTps);

	pstCompTps = UTIL_LINKEDLIST_AppendListItemToPointerList (pstCompTps, (void *)pstTuneInfo);
	UTIL_LINKEDLIST_CircularizePointerList (pstCompTps);

	pstContents->pstTpToSearch = pstSrchTps;
	pstContents->pstFinishedTps = pstCompTps;

	// 남은 TP가 전혀 없으면 종료해야 한다.
	if (pstSrchTps == NULL)
	{
		/* automatic search를 위한 tp list로 서치 결과가 없는 경우, linear search를 수행한다. Telenor 사양은 그렇게 되어 있음 */
		if(TRUE == pstContents->bAutoMode)
		{
			HUINT32		ulSvcNum = 0;
			HERROR		hErr = ERR_FAIL;

			hErr = MGR_SEARCHRESULT_CountService(pstContents->ulActionId, eDxSVC_TYPE_All, &ulSvcNum);
			if ((ERR_OK != hErr) || (0 == ulSvcNum))
			{
				HxLOG_Print(HxANSI_COLOR_YELLOW("Trying linear tp list\n"));
				xmgr_nordigsrch_MakeLinearTpList(pstContents);
				pstContents->bAutoMode = FALSE;

#if defined(CONFIG_OP_TDC)
				// NitOthBased -> NitActBased
				// Set Network ID to 0
				HAPI_SetRegistryInt(DxDB_NETWORK_ID_FOR_NETWORK_SEARCH, 0);
				pstContents->lSearchType = eSVCSI_SEARCHTYPE_NORDIG_NitActBased;
#endif

				return xmgr_nordigsrch_SetNextTpToSearch(pstContents);
			}
		}

		return ERR_BUS_NO_OBJECT;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_nordigsrch_TuneSearchTp (NordigSearch_Contents_t *pstContents)
{
	HBOOL				 bRetry;
	SvcRsc_Info_t			 stRscInfo;
	HERROR				 hErr;

	bRetry = TRUE;

	while (bRetry == TRUE)
	{
		hErr = xmgr_nordigsrch_SetTuningParamToSearch (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_nordigsrch_TuneSearchTp] No Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			return ERR_BUS_NO_OBJECT;
		}

		HxLOG_Print("\n\n\t ---------------------------------------------------------- \n");
		HxLOG_Print("\t -----Tune Search TP : Freq(%d), SR(%d) \n",
				pstContents->stSearchTuningInfo.cab.ulFrequency, pstContents->stSearchTuningInfo.cab.ulSymbolRate);

		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_nordigsrch_TuneSearchTp] Tuner Stop Failed\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			return ERR_BUS_NO_OBJECT;
		}

		MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);

		xmgr_nordigsrch_IncreaseSessionVersion (pstContents);			// Tuning 전에 반드시 Action Version을 업데이트해야 한다.

		hErr = SVC_CH_StartTune(pstContents->hAction, &stRscInfo, (HUINT16)-1, &pstContents->stSearchTuningInfo);
		if (hErr == ERR_OK)
		{
			pstContents->eAppState = eSearchApp_state_tryTuning;

			HxLOG_Info("\t -----SVC_CH_StartTune() OK... \n");

			// DD에서 Tuning Signal이 올라오지 않아 무한으로 기다리는 것을 막기 위해 여기서 timer를 처리하도록 한다.
			// BUS_SetTimer (SEARCH_TIMER_LOCKING_ID, SEARCH_TIMER_LOCKING_TIME);
			return ERR_BUS_NO_ERROR;
		}

		// Tuning failure : Go to the next TP
		hErr = xmgr_nordigsrch_SetNextTpToSearch (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_nordigsrch_TuneSearchTp] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			bRetry = FALSE;

			HxLOG_Info("\t -----bRetry = FALSE \n");
		}

		HxLOG_Info("\t -----next \n");
	}

	// All TP Completed:

	return ERR_BUS_NO_OBJECT;
}


#define	_____PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_nordigsrch_RetrieveResults_Normal (NordigSearch_Contents_t* pstContents)
{
	HINT32					 nTsNumInDb;
	HUINT32					 ulIndex, ulItemNum;
	HUINT32					 ulBqNum, ulPrvNum, ulSvcNum;
	HUINT32					 ulTempNetIdx, ulTempTsIdx, ulTempPrvIdx;
	Handle_t				 hSvc;
	HBOOL					 bNewTs;
	HUINT16					 usNetIdx = 0xff, usTsIdx = 0xff, usPrvIdx;
	DbSvc_NetInfo_t			 stNetInfoInDb;
	DbSvc_TsCondition_t		 stTsCond;
	DbSvc_TsInfo_t			*pstTsInfoInDb;
	DbSvc_TsInfo_t			*pstTsInfo = NULL;
	DbSvc_NetInfo_t			*pstNetInfo = NULL;
	DbSvc_Info_t			*astSvcArray = NULL;
	DbSvc_BouqInfo_t		*pstBqArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SRCHSVC_AppendFlag_t	*peSvcFlagArray = NULL;
	DbSvc_TuningInfo_t		*pstCurrTuneInfo;
	DbSvc_TuningInfo_t		 stSvcTuneInfo;
	DxTuneParam_t			 stChTuneInfo;
	HERROR					 hErr;
	HUINT32					 ulInfoNum = 0;

	//------------------------------------------------------------------------------
	// 1. Network Info
	// 현재 Network는 단 하나일 수 밖에 없다.
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, &ulItemNum);
	if (hErr != ERR_OK)					{ ulItemNum = 0; }

	if (ulItemNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	pstNetInfo = (DbSvc_NetInfo_t *)OxAP_Malloc (sizeof(DbSvc_NetInfo_t));
	if (pstNetInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, 1, &ulInfoNum, (void *)pstNetInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	HxLOG_Print("[%s (%04d)] SVC_SI_GetSearchedResults() Network Name [%s]\n", __FILE__, __LINE__, pstNetInfo->szNetName);

	pstNetInfo->eOrgDeliType = eDxDELIVERY_TYPE_CAB;

	//------------------------------------------------------------------------------
	// 2. TS Info
	// TS Info를 얻어오는 것이 제일 중요하다.
	// 여기서 TS Info를 얻어오지 못하면 search fail로 간주한다.
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, &ulItemNum);
	if (hErr != ERR_OK)					{ ulItemNum = 0; }

	if (ulItemNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// 현재 TS는 단 하나일 수 밖에 없다.
	pstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstTsInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, 1, &ulInfoNum, (void *)pstTsInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// SI에서 올려준 TS Info에 실제 물리적 Tuning Info 등을 추가하여 DB에 저장할 수 있는 data로 변환한다.
	hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
	if (hErr == ERR_OK)
	{
		MGR_SEARCHUTIL_ConvertChTuningParamToSvcTuningInfo (&stChTuneInfo, &stSvcTuneInfo);
		pstCurrTuneInfo = &stSvcTuneInfo;
	}
	else
	{
		pstCurrTuneInfo = xmgr_nordigsrch_GetCurrentTuningInfo (pstContents);
	}

	if (pstCurrTuneInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// Tuner ID
	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
	// Current TP Info
	HxSTD_memcpy (&(pstTsInfo->stTuningParam), pstCurrTuneInfo, sizeof(DbSvc_TuningInfo_t));

	pstTsInfo->eDeliType = eDxDELIVERY_TYPE_CAB;


	//------------------------------------------------------------------------------
	// 3. Bouquet Info
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Bouquet, &ulBqNum);
	if (hErr != ERR_OK)					{ ulBqNum = 0; }
	if (ulBqNum > 0)
	{
		pstBqArray = (DbSvc_BouqInfo_t *)OxAP_Malloc (sizeof(DbSvc_BouqInfo_t) * ulBqNum);
		if (pstBqArray != NULL)
		{
			hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Bouquet, ulBqNum, &ulInfoNum, (void *)pstBqArray);
			if (hErr != ERR_OK)
			{
				ulBqNum = 0;
				OxAP_Free (pstBqArray);
				pstBqArray = NULL;
			}
		}
	}

	//------------------------------------------------------------------------------
	// 4. Provider Info
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Provider, &ulPrvNum);
	if (hErr != ERR_OK)					{ ulPrvNum = 0; }
	if (ulPrvNum > 0)
	{
		pstPrvArray = (DbSvc_PrvInfo_t *)OxAP_Malloc (sizeof(DbSvc_PrvInfo_t) * ulPrvNum);
		if (pstPrvArray != NULL)
		{
			hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Provider, ulPrvNum, &ulInfoNum, (void *)pstPrvArray);
			if (hErr != ERR_OK)
			{
				ulPrvNum = 0;
				OxAP_Free (pstPrvArray);
				pstPrvArray = NULL;
			}
		}
	}

	//------------------------------------------------------------------------------
	// 5. Service Info
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, &ulSvcNum);
	if (hErr != ERR_OK)					{ ulSvcNum = 0; }

	if (ulSvcNum == 0)
	{
		HxLOG_Critical("ulSvcNum == 0 !!!!\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	astSvcArray = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * ulSvcNum);
	if (astSvcArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, ulSvcNum, &ulInfoNum, (void *)astSvcArray);
	if (astSvcArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		DbSvc_SetDeliType(astSvcArray[ulIndex], eDxDELIVERY_TYPE_CAB);
	}

	//------------------------------------------------------------------------------
	// 6. Store the searched results to the contents :
	// 6-1. Network Info
	hErr = MGR_SEARCHDATA_AddNetwork (&(pstContents->stNetArray), pstNetInfo, &ulTempNetIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	//------------------------------------------------------------------------------
	// 1. adding/updating a network
	//------------------------------------------------------------------------------
	if (pstNetInfo != NULL)
	{
		HxLOG_Print("\n\n\n[xmgr_nordigsrch_RetrieveResults_Normal] Network Name [%s]\n", pstNetInfo->szNetName);
		// Network Info가 이미 등록되어 있는 것인지 체크한다.
		hErr =	DB_SVC_FindNetworkIndexById(pstNetInfo->eOrgDeliType, pstNetInfo->usNetId, &usNetIdx);
		if (hErr == ERR_OK)
		{
			if (pstNetInfo->szNetName[0] != '\0')
			{
				DB_SVC_GetNetworkInfo (usNetIdx, &stNetInfoInDb);
				if (MWC_UTIL_DvbStrcmp (pstNetInfo->szNetName, stNetInfoInDb.szNetName) != 0)
				{
					HxLOG_Print("[xmgr_nordigsrch_RetrieveResults_Normal(%04d)] Network Name [%s], Network ID [%d]\n\n\n\n", __LINE__, pstNetInfo->szNetName, pstNetInfo->usNetId);
					// Network Name이 달라졌으면 Update 한다.
					DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfo);
					HxLOG_Print("\t= [NET] tempIdx = %d, netIdx = %d --> update\n", ulTempNetIdx, usNetIdx);
				}
			}
		}
		else
		{
			HxLOG_Print("[xmgr_nordigsrch_RetrieveResults_Normal(%04d)] Network Name [%s], Network ID [%d]\n\n\n\n", __LINE__, pstNetInfo->szNetName, pstNetInfo->usNetId);
			// 등록되어 있지 않으므로 새로 추가한다.
			DB_SVC_AddNetworkInfo (pstNetInfo, &usNetIdx);
			HxLOG_Print("\t= [NET] tempIdx = %d, netIdx = %d --> add\n", ulTempNetIdx, usNetIdx);
		}
	}

	// 6-2. Ts Info
	// Network Info를 넣을 때 나온 index 값을 temporary index로써 TsInfo에 넣는다.
	pstTsInfo->usNetIdx = (HUINT16)ulTempNetIdx;
	hErr = MGR_SEARCHDATA_AddTs (&(pstContents->stTsArray), pstTsInfo, &ulTempTsIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	bNewTs = TRUE;
	//------------------------------------------------------------------------------
	// 2. adding/updating a ts
	//------------------------------------------------------------------------------
	if (pstTsInfo != NULL)
	{
		pstTsInfo->usNetIdx = usNetIdx;
		pstTsInfo->eDeliType = eDxDELIVERY_TYPE_CAB;

		// TS Info가 OnId, TsId로 등록되어 있는 지 확인한다.
		DB_SVC_InitTsFindCondition (&stTsCond);
		stTsCond.nDeliType	= eDxDELIVERY_TYPE_CAB;
		stTsCond.nOnId		= (HINT32)pstTsInfo->usOnId;
		stTsCond.nTsId		= (HINT32)pstTsInfo->usTsId;

		bNewTs = TRUE;
		nTsNumInDb = 0;
		pstTsInfoInDb = NULL;

		hErr = DB_SVC_FindTsInfoList (&stTsCond, &nTsNumInDb, &pstTsInfoInDb);
		if (hErr == ERR_OK && nTsNumInDb > 0 && pstTsInfoInDb != NULL)
		{
			// 하나 이상이 발견되면 맨 처음 것을 사용하도록 한다.
			bNewTs = FALSE;
			usTsIdx = pstTsInfoInDb[0].usUniqueId;

			if (DB_SVC_UtilCompareDbTuningParam(eDxDELIVERY_TYPE_CAB, &(pstTsInfo->stTuningParam), &(pstTsInfoInDb[0].stTuningParam)) != 0)
			{
				DB_SVC_UpdateTsInfo (usTsIdx, pstTsInfo);
				HxLOG_Print("\t\t= [TS] tempIdx = %d, tsIdx = %d --> update\n", ulTempTsIdx, usTsIdx);
			}
		}

		if (bNewTs == TRUE)
		{
			DB_SVC_AddTsInfo (pstTsInfo, &usTsIdx);
			HxLOG_Print("\t\t= [TS] tempIdx = %d, tsIdx = %d --> update\n", ulTempTsIdx, usTsIdx);
		}

		if (pstTsInfoInDb != NULL)
		{
			DB_SVC_FreeTsInfoList (pstTsInfoInDb);
			pstTsInfoInDb = NULL;
		}

	}

	// 6-3. Bouquet Info
	if (pstBqArray != NULL)
	{
		for (ulIndex = 0; ulIndex < ulBqNum; ulIndex++)
		{
			MGR_SEARCHDATA_AddBouquet (&(pstContents->stBqArray), &(pstBqArray[ulIndex]));
		}
	}

	// 6-4. Provider Info
	// Svc Info에 들어가는 Provider Index의 재계산을 위해 Provider Info가 새로 들어가는 위치를 미리 얻어낸다.
	hErr = MGR_SEARCHDATA_CountProvider (&(pstContents->stPrvArray), &ulTempPrvIdx);
	if (hErr != ERR_OK)						{ ulTempPrvIdx = 0; }

	if (pstPrvArray != NULL)
	{
		for (ulIndex = 0; ulIndex < ulPrvNum; ulIndex++)
		{
			MGR_SEARCHDATA_AddProvider (&(pstContents->stPrvArray), &(pstPrvArray[ulIndex]));

			//------------------------------------------------------------------------------
			// 3. adding/updating providers
			//------------------------------------------------------------------------------
			// Provider Info가 미리 등록되어 있는지 확인한다.
			hErr = DB_SVC_FindProviderIndexByName (pstPrvArray[ulIndex].szPrvName, &usPrvIdx);
			if (hErr != ERR_OK)
			{
				// 등록되어 있지 않으면 추가한다.
				DB_SVC_AddProviderInfo (&pstPrvArray[ulIndex], &usPrvIdx);
				HxLOG_Print("\t\t= [PRV] tempIdx = %d, prvIdx = %d --> add\n", ulTempPrvIdx, usPrvIdx);
			}
		}
	}

	// 6-5. Service Info
	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		if (MGR_SEARCHUTIL_CompareSearchedSvcType (pstContents->eServiceType, DbSvc_GetSvcType(&astSvcArray[ulIndex])) == TRUE)
		{
			DbSvc_SetDeliType(astSvcArray[ulIndex], eDxDELIVERY_TYPE_CAB);
			//astSvcArray[ulIndex].usOnId = pstTsInfo->usOnId;
			//astSvcArray[ulIndex].usTsId = pstTsInfo->usTsId;
			DbSvc_SetTsIdx(&astSvcArray[ulIndex], ulTempTsIdx);
			DbSvc_SetPrvIdx(&astSvcArray[ulIndex], DbSvc_GetPrvIdx(&astSvcArray[ulIndex]) + (HUINT16)ulTempPrvIdx);
			DbSvc_SetTsIdx(&astSvcArray[ulIndex], usTsIdx);
			DbSvc_SetPrvIdx(&astSvcArray[ulIndex], 0); // TODO:
		}
	}

	peSvcFlagArray = (SRCHSVC_AppendFlag_t *)OxAP_Malloc (sizeof(SRCHSVC_AppendFlag_t) * ulSvcNum);
	if (peSvcFlagArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_SetLcnServiceNumber(pstContents->hAction, bNewTs, ulSvcNum, astSvcArray, peSvcFlagArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		switch (peSvcFlagArray[ulIndex])
		{
		case eSRCHSVC_ADD:
			HxLOG_Print(">>> ADD    : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			DB_SVC_AddServiceInfo (&astSvcArray[ulIndex], &hSvc);
			DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));

			// Add the service lists in the GUI :
			MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &astSvcArray[ulIndex]);
			break;

		case eSRCHSVC_UPDATE:
			HxLOG_Print(">>> UPDATE : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			hErr = xmgr_nordigsrch_GetServiceHandle (DbSvc_GetTsIdx(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), DbSvc_GetOnId(&astSvcArray[ulIndex]), &hSvc);
			if (hErr == ERR_OK)
			{
				DB_SVC_UpdateServiceInfo (hSvc, &astSvcArray[ulIndex]);
				DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));

				// Add the service lists in the GUI :
				MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &astSvcArray[ulIndex]);
			}
			break;

		default:
			HxLOG_Print(">>> SKIP   : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));

			// Add the service lists in the GUI :
			hErr = xmgr_nordigsrch_GetServiceHandle (DbSvc_GetTsIdx(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), DbSvc_GetOnId(&astSvcArray[ulIndex]), &hSvc);
			if (hErr == ERR_OK)
			{
				DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));
				// 어찌 되었건 리스트에 보여야 한다.
				MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &astSvcArray[ulIndex]);
			}

			break;
		}
	}

	(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, pstTsInfo->usOnId, pstTsInfo->usTsId);

	//  list변경됐음을 알림
	xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);

	hErr = ERR_BUS_NO_ERROR;

ERROR_FAULT_OCCURRED:
	if (pstTsInfo != NULL)
	{
		OxAP_Free (pstTsInfo);
	}

	if (pstNetInfo != NULL)
	{
		OxAP_Free (pstNetInfo);
	}

	if (astSvcArray != NULL)
	{
		OxAP_Free (astSvcArray);
	}

	if (pstBqArray != NULL)
	{
		OxAP_Free (pstBqArray);
	}

	if (pstPrvArray != NULL)
	{
		OxAP_Free (pstPrvArray);
	}

	if (peSvcFlagArray != NULL)
	{
		OxAP_Free (peSvcFlagArray);
	}

	return hErr;
}

STATIC HERROR xmgr_nordigsrch_RetrieveResults_NitActBased (NordigSearch_Contents_t* pstContents)
{
	HINT32					 nTsNumInDb;
	HUINT32					 ulIndex, ulItemNum;
	HUINT32					 ulPrvNum, ulSvcNum;
	HUINT32					 ulTempNetIdx, ulTempTsIdx, ulTempPrvIdx;
	Handle_t				 hSvc;
	HBOOL					 bNewTs;
	HUINT16					 usNetIdx = 0xff, usTsIdx = 0xff, usPrvIdx;
	DbSvc_NetInfo_t			 stNetInfoInDb;
	DbSvc_TsCondition_t		 stTsCond;
	DbSvc_TsInfo_t			*pstTsInfoInDb;
	DbSvc_TsInfo_t			*pstTsInfo = NULL;
	DxTuneParam_t			 stChTuneInfo;
	DbSvc_TuningInfo_t		 stSvcTuneInfo;
	DbSvc_TuningInfo_t		*pstCurrTuneInfo;
	DbSvc_NetInfo_t			*pstNetInfo = NULL;
	DbSvc_Info_t			*astSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SRCHSVC_AppendFlag_t	*peSvcFlagArray = NULL;
	HERROR					 hErr;
	HUINT32					 ulInfoNum = 0;

	//------------------------------------------------------------------------------
	// 1. Network Info
	// 현재 Network는 단 하나일 수 밖에 없다.
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, &ulItemNum);
	if (hErr != ERR_OK)					{ ulItemNum = 0; }

	if (ulItemNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	pstNetInfo = (DbSvc_NetInfo_t *)OxAP_Malloc (sizeof(DbSvc_NetInfo_t));
	if (pstNetInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, 1, &ulInfoNum, (void *)pstNetInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	pstNetInfo->eOrgDeliType = eDxDELIVERY_TYPE_CAB;

	//------------------------------------------------------------------------------
	// 2. TS Info
	// TS Info를 얻어오는 것이 제일 중요하다.
	// 여기서 TS Info를 얻어오지 못하면 search fail로 간주한다.
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, &ulItemNum);
	if (hErr != ERR_OK)					{ ulItemNum = 0; }

	HxLOG_Print("eSI_SRCH_INFOTYPE_Transponder: ulItemNum [%d]\n", ulItemNum);

	if (ulItemNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// 현재 TS는 단 하나일 수 밖에 없다.
	pstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstTsInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, 1, &ulInfoNum, (void *)pstTsInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// SI에서 올려준 TS Info에 실제 물리적 Tuning Info 등을 추가하여 DB에 저장할 수 있는 data로 변환한다.
	hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
	if (hErr == ERR_OK)
	{
		MGR_SEARCHUTIL_ConvertChTuningParamToSvcTuningInfo (&stChTuneInfo, &stSvcTuneInfo);
		pstCurrTuneInfo = &stSvcTuneInfo;
	}
	else
	{
		pstCurrTuneInfo = xmgr_nordigsrch_GetCurrentTuningInfo (pstContents);
	}

	if (pstCurrTuneInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// Tuner ID
	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
	// Current TP Info
	HxSTD_memcpy (&(pstTsInfo->stTuningParam), pstCurrTuneInfo, sizeof(DbSvc_TuningInfo_t));

	pstTsInfo->eDeliType = eDxDELIVERY_TYPE_CAB;

	//------------------------------------------------------------------------------
	// 4. Provider Info
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Provider, &ulPrvNum);
	if (hErr != ERR_OK)					{ ulPrvNum = 0; }
	if (ulPrvNum > 0)
	{
		pstPrvArray = (DbSvc_PrvInfo_t *)OxAP_Malloc (sizeof(DbSvc_PrvInfo_t) * ulPrvNum);
		if (pstPrvArray != NULL)
		{
			hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Provider, ulPrvNum, &ulInfoNum, (void *)pstPrvArray);
			if (hErr != ERR_OK)
			{
				ulPrvNum = 0;
				OxAP_Free (pstPrvArray);
				pstPrvArray = NULL;
			}
		}
	}

	//------------------------------------------------------------------------------
	// 5. Service Info
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, &ulSvcNum);
	if (hErr != ERR_OK)					{ ulSvcNum = 0; }

	if (ulSvcNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	astSvcArray = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * ulSvcNum);
	if (astSvcArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, ulSvcNum, &ulInfoNum, (void *)astSvcArray);
	if (astSvcArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		DbSvc_SetDeliType(astSvcArray[ulIndex], eDxDELIVERY_TYPE_CAB);
	}

	//------------------------------------------------------------------------------
	// 6. Store the searched results to the contents :
	// 6-1. Network Info
	hErr = MGR_SEARCHDATA_AddNetwork (&(pstContents->stNetArray), pstNetInfo, &ulTempNetIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	//------------------------------------------------------------------------------
	// 1. adding/updating a network
	//------------------------------------------------------------------------------
	if (pstNetInfo != NULL)
	{
		HxLOG_Print("Network Name [%s]\n", pstNetInfo->szNetName);
		// Network Info가 이미 등록되어 있는 것인지 체크한다.
		hErr =	DB_SVC_FindNetworkIndexById(pstNetInfo->eOrgDeliType, pstNetInfo->usNetId, &usNetIdx);
		if (hErr == ERR_OK)
		{
			if (pstNetInfo->szNetName[0] != '\0')
			{
				DB_SVC_GetNetworkInfo (usNetIdx, &stNetInfoInDb);
				if (MWC_UTIL_DvbStrcmp (pstNetInfo->szNetName, stNetInfoInDb.szNetName) != 0)
				{
					HxLOG_Print("Network Name [%s], Network ID [%d]\n\n\n\n", pstNetInfo->szNetName, pstNetInfo->usNetId);
					// Network Name이 달라졌으면 Update 한다.
					DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfo);
					HxLOG_Print("\t= [NET] tempIdx = %d, netIdx = %d --> update\n", ulTempNetIdx, usNetIdx);
				}
			}
		}
		else
		{
			HxLOG_Print("Network Name [%s], Network ID [%d]\n\n\n\n", pstNetInfo->szNetName, pstNetInfo->usNetId);
			// 등록되어 있지 않으므로 새로 추가한다.
			DB_SVC_AddNetworkInfo (pstNetInfo, &usNetIdx);
			HxLOG_Print("\t\t= [NET] tempIdx = %d, netIdx = %d --> add\n", ulTempNetIdx, usNetIdx);
		}
	}

	// 6-2. Ts Info
	// Network Info를 넣을 때 나온 index 값을 temporary index로써 TsInfo에 넣는다.
	pstTsInfo->usNetIdx = (HUINT16)ulTempNetIdx;
	hErr = MGR_SEARCHDATA_AddTs (&(pstContents->stTsArray), pstTsInfo, &ulTempTsIdx);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	bNewTs = TRUE;
	//------------------------------------------------------------------------------
	// 2. adding/updating a ts
	//------------------------------------------------------------------------------
	if (pstTsInfo != NULL)
	{
		pstTsInfo->usNetIdx = usNetIdx;
		pstTsInfo->eDeliType = eDxDELIVERY_TYPE_CAB;

		// TS Info가 OnId, TsId로 등록되어 있는 지 확인한다.
		DB_SVC_InitTsFindCondition (&stTsCond);
		stTsCond.nDeliType	= eDxDELIVERY_TYPE_CAB;
		stTsCond.nOnId		= (HINT32)pstTsInfo->usOnId;
		stTsCond.nTsId		= (HINT32)pstTsInfo->usTsId;

		bNewTs = TRUE;
		nTsNumInDb = 0;
		pstTsInfoInDb = NULL;

		hErr = DB_SVC_FindTsInfoList (&stTsCond, &nTsNumInDb, &pstTsInfoInDb);
		if (hErr == ERR_OK && nTsNumInDb > 0 && pstTsInfoInDb != NULL)
		{
			// 하나 이상이 발견되면 맨 처음 것을 사용하도록 한다.
			bNewTs = FALSE;
			usTsIdx = pstTsInfoInDb[0].usUniqueId;

			if (DB_SVC_UtilCompareDbTuningParam(eDxDELIVERY_TYPE_CAB, &(pstTsInfo->stTuningParam), &(pstTsInfoInDb[0].stTuningParam)) != 0)
			{
				DB_SVC_UpdateTsInfo (usTsIdx, pstTsInfo);
				HxLOG_Print("\t\t= [TS] tempIdx = %d, tsIdx = %d --> update\n", ulTempTsIdx, usTsIdx);
			}
		}

		if (bNewTs == TRUE)
		{
			DB_SVC_AddTsInfo (pstTsInfo, &usTsIdx);
			HxLOG_Print("\t\t= [TS] tempIdx = %d, tsIdx = %d --> update\n", ulTempTsIdx, usTsIdx);
		}

		if (pstTsInfoInDb != NULL)
		{
			DB_SVC_FreeTsInfoList (pstTsInfoInDb);
			pstTsInfoInDb = NULL;
		}
	}

	// 6-4. Provider Info
	// Svc Info에 들어가는 Provider Index의 재계산을 위해 Provider Info가 새로 들어가는 위치를 미리 얻어낸다.
	hErr = MGR_SEARCHDATA_CountProvider (&(pstContents->stPrvArray), &ulTempPrvIdx);
	if (hErr != ERR_OK)						{ ulTempPrvIdx = 0; }

	if (pstPrvArray != NULL)
	{
		for (ulIndex = 0; ulIndex < ulPrvNum; ulIndex++)
		{
			MGR_SEARCHDATA_AddProvider (&(pstContents->stPrvArray), &(pstPrvArray[ulIndex]));

			//------------------------------------------------------------------------------
			// 3. adding/updating providers
			//------------------------------------------------------------------------------
			// Provider Info가 미리 등록되어 있는지 확인한다.
			hErr = DB_SVC_FindProviderIndexByName (pstPrvArray[ulIndex].szPrvName, &usPrvIdx);
			if (hErr != ERR_OK)
			{
				// 등록되어 있지 않으면 추가한다.
				DB_SVC_AddProviderInfo (&pstPrvArray[ulIndex], &usPrvIdx);
				HxLOG_Print("\t\t= [PRV] tempIdx = %d, prvIdx = %d --> add\n", ulTempPrvIdx, usPrvIdx);
			}
		}
	}

	// 6-5. Service Info
	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		if (MGR_SEARCHUTIL_CompareSearchedSvcType (pstContents->eServiceType, DbSvc_GetSvcType(&astSvcArray[ulIndex])) == TRUE)
		{
			DbSvc_SetDeliType(astSvcArray[ulIndex], eDxDELIVERY_TYPE_CAB);
			// onid와 tsid를 이용해서 TsIndex를 가져온다.
			ulTempTsIdx = xmgr_nordigsrch_GetUniqueTsIndex(DbSvc_GetOnId(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]));
			DbSvc_SetTsIdx(&astSvcArray[ulIndex], ulTempTsIdx);

			DbSvc_SetPrvIdx(&astSvcArray[ulIndex], DbSvc_GetPrvIdx(&astSvcArray[ulIndex]) + (HUINT16)ulTempPrvIdx);
			DbSvc_SetPrvIdx(&astSvcArray[ulIndex], 0); // TODO:
		}
	}

	peSvcFlagArray = (SRCHSVC_AppendFlag_t *)OxAP_Malloc (sizeof(SRCHSVC_AppendFlag_t) * ulSvcNum);
	if (peSvcFlagArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_SetLcnServiceNumber(pstContents->hAction, bNewTs, ulSvcNum, astSvcArray, peSvcFlagArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		switch (peSvcFlagArray[ulIndex])
		{
		case eSRCHSVC_ADD:
			//HxLOG_Print(">>> ADD    : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			DB_SVC_AddServiceInfo (&astSvcArray[ulIndex], &hSvc);
			DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));
			HxLOG_Print("ADD: %d, %s, idx=(%d,%d,%d), id=(%d,%d,%d), type=0x%x, hSvc=0x%x\n"
				, DbSvc_GetLcn(&astSvcArray[ulIndex])
				, DbSvc_GetName(&astSvcArray[ulIndex])
				, DbSvc_GetAntIdx(&astSvcArray[ulIndex])
				, DbSvc_GetTsIdx(&astSvcArray[ulIndex])
				, DbSvc_GetSvcIdx(&astSvcArray[ulIndex])
				, DbSvc_GetOnId(&astSvcArray[ulIndex])
				, DbSvc_GetTsId(&astSvcArray[ulIndex])
				, DbSvc_GetSvcId(&astSvcArray[ulIndex])
				, DbSvc_GetSvcType(&astSvcArray[ulIndex])
				, hSvc
				);

			// Add the service lists in the GUI :
			MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &astSvcArray[ulIndex]);
			(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, DbSvc_GetOnId(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]));
			break;

		case eSRCHSVC_UPDATE:
			//HxLOG_Print(">>> UPDATE : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			HxLOG_Print("UPDATE: %d, %s, idx=(%d,%d,%d), id=(%d,%d,%d), type=0x%x, hSvc=0x%x\n"
				, DbSvc_GetLcn(&astSvcArray[ulIndex])
				, DbSvc_GetName(&astSvcArray[ulIndex])
				, DbSvc_GetAntIdx(&astSvcArray[ulIndex])
				, DbSvc_GetTsIdx(&astSvcArray[ulIndex])
				, DbSvc_GetSvcIdx(&astSvcArray[ulIndex])
				, DbSvc_GetOnId(&astSvcArray[ulIndex])
				, DbSvc_GetTsId(&astSvcArray[ulIndex])
				, DbSvc_GetSvcId(&astSvcArray[ulIndex])
				, DbSvc_GetSvcType(&astSvcArray[ulIndex])
				, hSvc
				);

			hErr = xmgr_nordigsrch_GetServiceHandle (DbSvc_GetTsIdx(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), DbSvc_GetOnId(&astSvcArray[ulIndex]), &hSvc);
			if (hErr == ERR_OK)
			{
				DB_SVC_UpdateServiceInfo (hSvc, &astSvcArray[ulIndex]);
				DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));

				// Add the service lists in the GUI :
				MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &astSvcArray[ulIndex]);
				(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, DbSvc_GetOnId(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]));
			}

			break;

		default:
			//HxLOG_Print(">>> SKIP   : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			HxLOG_Print("SKIP: %d, %s, idx=(%d,%d,%d), id=(%d,%d,%d), type=0x%x, hSvc=0x%x\n"
				, DbSvc_GetLcn(&astSvcArray[ulIndex])
				, DbSvc_GetName(&astSvcArray[ulIndex])
				, DbSvc_GetAntIdx(&astSvcArray[ulIndex])
				, DbSvc_GetTsIdx(&astSvcArray[ulIndex])
				, DbSvc_GetSvcIdx(&astSvcArray[ulIndex])
				, DbSvc_GetOnId(&astSvcArray[ulIndex])
				, DbSvc_GetTsId(&astSvcArray[ulIndex])
				, DbSvc_GetSvcId(&astSvcArray[ulIndex])
				, DbSvc_GetSvcType(&astSvcArray[ulIndex])
				, hSvc
				);

			// 어찌 되었건 리스트에 보여야 한다.
			hErr = xmgr_nordigsrch_GetServiceHandle (DbSvc_GetTsIdx(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), DbSvc_GetOnId(&astSvcArray[ulIndex]), &hSvc);
			if (hErr == ERR_OK)
			{
				DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));

				// Add the service lists in the GUI :
				MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &astSvcArray[ulIndex]);
				(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, DbSvc_GetOnId(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]));
			}

			break;
		}
	}

	(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, pstTsInfo->usOnId, pstTsInfo->usTsId);

	//  list변경됐음을 알림
	xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);

	hErr = ERR_BUS_NO_ERROR;

ERROR_FAULT_OCCURRED:
	if (pstTsInfo != NULL)
	{
		OxAP_Free (pstTsInfo);
	}

	if (pstNetInfo != NULL)
	{
		OxAP_Free (pstNetInfo);
	}

	if (astSvcArray != NULL)
	{
		OxAP_Free (astSvcArray);
	}

	if (pstPrvArray != NULL)
	{
		OxAP_Free (pstPrvArray);
	}

	if (peSvcFlagArray != NULL)
	{
		OxAP_Free (peSvcFlagArray);
	}

	return hErr;
}

STATIC HERROR xmgr_nordigsrch_RetrieveResults_NitOthBased (NordigSearch_Contents_t* pstContents)
{
	HINT32					 nTsNumInDb;
	HUINT32					 ulIndex, i;
	HUINT32					 ulNetNum = 0, ulTsNum = 0, ulPrvNum = 0, ulSvcNum = 0;
	HUINT32					 ulTempNetIdx, ulTempTsIdx, ulTempPrvIdx;
	Handle_t				 hSvc;
	HBOOL					 bNewTs;
	HUINT16					 usNetIdx = 0xff, usTsIdx = 0xff, usPrvIdx;
	DbSvc_NetInfo_t			 stNetInfoInDb;
	DbSvc_TsCondition_t		 stTsCond;
	DbSvc_TsInfo_t			*pstTsInfoInDb;
	DbSvc_TsInfo_t			*pstTsArray = NULL;
	DbSvc_NetInfo_t			*pstNetArray = NULL, *pstNetInfo = NULL;
	DbSvc_Info_t			*astSvcArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SRCHSVC_AppendFlag_t	*peSvcFlagArray = NULL;
	HERROR					 hErr;
	HUINT32					 ulInfoNum = 0;

	//------------------------------------------------------------------------------
	// 1. Network Info
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, &ulNetNum);
	if (hErr != ERR_OK)					{ ulNetNum = 0; }

	if (ulNetNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	pstNetArray = (DbSvc_NetInfo_t *)OxAP_Malloc (sizeof(DbSvc_NetInfo_t) * ulNetNum);
	if (pstNetArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, ulNetNum, &ulInfoNum, (void *)pstNetArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("-------------------------------------------------------\n");
	for (i = 0; i < ulNetNum; i++)
	{
		HxLOG_Print("NET(%d) deli:%d, netidx:%d, nid:%d, %s\n"
			, i
			, pstNetArray[i].eOrgDeliType
			, pstNetArray[i].usNetIdx
			, pstNetArray[i].usNetId
			, pstNetArray[i].szNetName
			);
	}
	HxLOG_Print("-------------------------------------------------------\n");
#endif

	//------------------------------------------------------------------------------
	// 2. TS Info
	// TS Info를 얻어오는 것이 제일 중요하다.
	// 여기서 TS Info를 얻어오지 못하면 search fail로 간주한다.
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, &ulTsNum);
	if (hErr != ERR_OK)					{ ulTsNum = 0; }

	HxLOG_Print("eSI_SRCH_INFOTYPE_Transponder: ulItemNum [%d]\n", ulTsNum);

	if (ulTsNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// NIT의 System Delivery descriptor 와 SDT ACT/SDT OTH의 조합으로 TS Info를 구성한다.
	pstTsArray = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t) * ulTsNum);
	if (pstTsArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, ulTsNum, &ulInfoNum, (void *)pstTsArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print("-------------------------------------------------------\n");
	for (i = 0; i < ulTsNum; i++)
	{
		HxLOG_Print("TS(%d) deli:%d, netidx:%d, tsidx:%d, on/ts:0x%x/0x%x, %dKHz %d %d\n"
			, i
			, pstTsArray[i].eDeliType
			, pstTsArray[i].usNetIdx
			, pstTsArray[i].usTsIdx
			, pstTsArray[i].usOnId
			, pstTsArray[i].usTsId
			, pstTsArray[i].stTuningParam.cab.ulFreq
			, pstTsArray[i].stTuningParam.cab.usSR
			, pstTsArray[i].stTuningParam.cab.ucCon
			);
	}
	HxLOG_Print("-------------------------------------------------------\n");
#endif

	//------------------------------------------------------------------------------
	// 4. Provider Info
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Provider, &ulPrvNum);
	if (hErr != ERR_OK)					{ ulPrvNum = 0; }
	if (ulPrvNum > 0)
	{
		pstPrvArray = (DbSvc_PrvInfo_t *)OxAP_Malloc (sizeof(DbSvc_PrvInfo_t) * ulPrvNum);
		if (pstPrvArray != NULL)
		{
			hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Provider, ulPrvNum, &ulInfoNum, (void *)pstPrvArray);
			if (hErr != ERR_OK)
			{
				ulPrvNum = 0;
				OxAP_Free (pstPrvArray);
				pstPrvArray = NULL;
			}
		}
	}

	//------------------------------------------------------------------------------
	// 5. Service Info
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, &ulSvcNum);
	if (hErr != ERR_OK)					{ ulSvcNum = 0; }

	if (ulSvcNum == 0)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	astSvcArray = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * ulSvcNum);
	if (astSvcArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, ulSvcNum, &ulInfoNum, (void *)astSvcArray);
	if (astSvcArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		DbSvc_SetDeliType(astSvcArray[ulIndex], eDxDELIVERY_TYPE_CAB);
	}

	//------------------------------------------------------------------------------
	// 6. Store the searched results to the contents :
	// 6-1. Network Info
	for (i = 0; i < ulNetNum; i++)
	{
		pstNetInfo = &pstNetArray[i];
		if (pstNetInfo != NULL)
		{
			pstNetInfo->eOrgDeliType = eDxDELIVERY_TYPE_CAB;
			hErr = MGR_SEARCHDATA_AddNetwork (&(pstContents->stNetArray), pstNetInfo, &ulTempNetIdx);
			if (hErr != ERR_OK)
			{
				HxLOG_Critical("\n\n");
				hErr = ERR_FAIL;
				goto ERROR_FAULT_OCCURRED;
			}

			//------------------------------------------------------------------------------
			// 2. adding/updating a network
			//------------------------------------------------------------------------------
			HxLOG_Print("Network Name [%s], Network ID [%d]\n", pstNetInfo->szNetName, pstNetInfo->usNetId);
			// Network Info가 이미 등록되어 있는 것인지 체크한다.
			hErr =	DB_SVC_FindNetworkIndexById(pstNetInfo->eOrgDeliType, pstNetInfo->usNetId, &usNetIdx);
			if (hErr == ERR_OK)
			{
				if (pstNetInfo->szNetName[0] != '\0')
				{
					DB_SVC_GetNetworkInfo (usNetIdx, &stNetInfoInDb);
					if (MWC_UTIL_DvbStrcmp (pstNetInfo->szNetName, stNetInfoInDb.szNetName) != 0)
					{
						// Network Name이 달라졌으면 Update 한다.
						DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfo);
						HxLOG_Print("\t= [NET] tempIdx = %d, netIdx = %d --> update\n", pstNetInfo->usNetIdx, usNetIdx);
					}
				}
			}
			else
			{
				// 등록되어 있지 않으므로 새로 추가한다.
				DB_SVC_AddNetworkInfo (pstNetInfo, &usNetIdx);
				HxLOG_Print("\t= [NET] tempIdx = %d, netIdx = %d --> add\n", pstNetInfo->usNetIdx, usNetIdx);
			}

			for (ulIndex = 0; ulIndex < ulTsNum; ulIndex++)
			{
				if (NULL != &pstTsArray[ulIndex])
				{
					if(pstNetInfo->usNetIdx == pstTsArray[ulIndex].usNetIdx)
					{
						pstTsArray[ulIndex].usNetIdx = usNetIdx;
					}
				}
			}
		}
		else
		{
			HxLOG_Critical("\n\n");
			hErr = ERR_FAIL;
			goto ERROR_FAULT_OCCURRED;
		}
	}


	// SI에서 받은 TS Info 개수만큼 for loop을 돌면서 ADD해 준다.
	// 6-2. Ts Info
	// Network Info를 넣을 때 나온 index 값을 temporary index로써 TsInfo에 넣는다.
	for (i = 0; i < ulTsNum; i++)
	{
		//pstTsArray[i].usNetIdx = (HUINT16)ulTempNetIdx;
		hErr = MGR_SEARCHDATA_AddTs (&(pstContents->stTsArray), &pstTsArray[i], &ulTempTsIdx);
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			hErr = ERR_FAIL;
			goto ERROR_FAULT_OCCURRED;
		}

		bNewTs = TRUE;
		//------------------------------------------------------------------------------
		// 2. adding/updating a ts
		//------------------------------------------------------------------------------
		if (&pstTsArray[i] != NULL)
		{
			//pstTsArray[i].usNetIdx = usNetIdx;
			//pstTsArray[i].eDeliType = eDxDELIVERY_TYPE_CAB;

			// TS Info가 OnId, TsId로 등록되어 있는 지 확인한다.
			DB_SVC_InitTsFindCondition (&stTsCond);
			stTsCond.nDeliType	= eDxDELIVERY_TYPE_CAB;
			stTsCond.nOnId		= (HINT32)pstTsArray[i].usOnId;
			stTsCond.nTsId		= (HINT32)pstTsArray[i].usTsId;

			bNewTs = TRUE;
			nTsNumInDb = 0;
			pstTsInfoInDb = NULL;

			hErr = DB_SVC_FindTsInfoList (&stTsCond, &nTsNumInDb, &pstTsInfoInDb);
			if (hErr == ERR_OK && nTsNumInDb > 0 && pstTsInfoInDb != NULL)
			{
				// 하나 이상이 발견되면 맨 처음 것을 사용하도록 한다.
				bNewTs = FALSE;
				usTsIdx = pstTsInfoInDb[0].usUniqueId;

				if (DB_SVC_UtilCompareDbTuningParam (eDxDELIVERY_TYPE_CAB, &(pstTsArray[i].stTuningParam), &(pstTsInfoInDb[0].stTuningParam)) != 0)
				{
					DB_SVC_UpdateTsInfo (usTsIdx, &pstTsArray[i]);
					HxLOG_Print("\t\t= [TS] tempIdx = %d, tsIdx = %d, nedIdx = %d --> update\n", ulTempTsIdx, usTsIdx, pstTsArray[i].usNetIdx);
				}
			}

			if (bNewTs == TRUE)
			{
				DB_SVC_AddTsInfo (&pstTsArray[i], &usTsIdx);
				HxLOG_Print("\t\t= [TS] tempIdx = %d, tsIdx = %d, nedIdx = %d --> add\n", ulTempTsIdx, usTsIdx, pstTsArray[i].usNetIdx);
			}

			if (pstTsInfoInDb != NULL)
			{
				DB_SVC_FreeTsInfoList (pstTsInfoInDb);
				pstTsInfoInDb = NULL;
			}
		}
	}

	// 6-4. Provider Info
	// Svc Info에 들어가는 Provider Index의 재계산을 위해 Provider Info가 새로 들어가는 위치를 미리 얻어낸다.
	hErr = MGR_SEARCHDATA_CountProvider (&(pstContents->stPrvArray), &ulTempPrvIdx);
	if (hErr != ERR_OK)						{ ulTempPrvIdx = 0; }

	if (pstPrvArray != NULL)
	{
		for (ulIndex = 0; ulIndex < ulPrvNum; ulIndex++)
		{
			MGR_SEARCHDATA_AddProvider (&(pstContents->stPrvArray), &(pstPrvArray[ulIndex]));

			//------------------------------------------------------------------------------
			// 3. adding/updating providers
			//------------------------------------------------------------------------------
			// Provider Info가 미리 등록되어 있는지 확인한다.
			hErr = DB_SVC_FindProviderIndexByName (pstPrvArray[ulIndex].szPrvName, &usPrvIdx);
			if (hErr != ERR_OK)
			{
				// 등록되어 있지 않으면 추가한다.
				DB_SVC_AddProviderInfo (&pstPrvArray[ulIndex], &usPrvIdx);
				HxLOG_Print("\t\t= [PRV] tempIdx = %d, prvIdx = %d --> add\n", ulTempPrvIdx, usPrvIdx);
			}
		}
	}

	// 6-5. Service Info
	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		if (MGR_SEARCHUTIL_CompareSearchedSvcType (pstContents->eServiceType, DbSvc_GetSvcType(&astSvcArray[ulIndex])) == TRUE)
		{
			DbSvc_SetDeliType(astSvcArray[ulIndex], eDxDELIVERY_TYPE_CAB);
			// onid와 tsid를 이용해서 TsIndex를 가져온다.
			ulTempTsIdx = xmgr_nordigsrch_GetUniqueTsIndex(DbSvc_GetOnId(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]));
			DbSvc_SetTsIdx(&astSvcArray[ulIndex], ulTempTsIdx);

			DbSvc_SetPrvIdx(&astSvcArray[ulIndex], DbSvc_GetPrvIdx(&astSvcArray[ulIndex]) + (HUINT16)ulTempPrvIdx);
			DbSvc_SetPrvIdx(&astSvcArray[ulIndex], 0); // TODO:
		}
	}

	peSvcFlagArray = (SRCHSVC_AppendFlag_t *)OxAP_Malloc (sizeof(SRCHSVC_AppendFlag_t) * ulSvcNum);
	if (peSvcFlagArray == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_SetLcnServiceNumber(pstContents->hAction, bNewTs, ulSvcNum, astSvcArray, peSvcFlagArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		switch (peSvcFlagArray[ulIndex])
		{
		case eSRCHSVC_ADD:
			HxLOG_Print(">>> ADD    : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			DB_SVC_AddServiceInfo (&astSvcArray[ulIndex], &hSvc);
			DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));

			// Add the service lists in the GUI :
			MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &astSvcArray[ulIndex]);
			(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, DbSvc_GetOnId(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]));
			break;

		case eSRCHSVC_UPDATE:
			HxLOG_Print(">>> UPDATE : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			hErr = xmgr_nordigsrch_GetServiceHandle (DbSvc_GetTsIdx(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), DbSvc_GetOnId(&astSvcArray[ulIndex]), &hSvc);
			if (hErr == ERR_OK)
			{
				DB_SVC_UpdateServiceInfo (hSvc, &astSvcArray[ulIndex]);
				DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));

				// Add the service lists in the GUI :
				MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &astSvcArray[ulIndex]);
				(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, DbSvc_GetOnId(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]));
			}

			break;

		default:
			HxLOG_Print(">>> SKIP   : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));

			// 어찌 되었건 리스트에 보여야 한다.
			hErr = xmgr_nordigsrch_GetServiceHandle (DbSvc_GetTsIdx(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), DbSvc_GetOnId(&astSvcArray[ulIndex]), &hSvc);
			if (hErr == ERR_OK)
			{
				DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));

				// Add the service lists in the GUI :
				MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &astSvcArray[ulIndex]);
				(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, DbSvc_GetOnId(&astSvcArray[ulIndex]), DbSvc_GetTsId(&astSvcArray[ulIndex]));
			}

			break;
		}
	}

	//  list변경됐음을 알림
	xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);

	hErr = ERR_BUS_NO_ERROR;

ERROR_FAULT_OCCURRED:
	if (pstTsArray != NULL)
	{
		OxAP_Free (pstTsArray);
	}

	if (pstNetArray != NULL)
	{
		OxAP_Free (pstNetArray);
	}

	if (astSvcArray != NULL)
	{
		OxAP_Free (astSvcArray);
	}

	if (pstPrvArray != NULL)
	{
		OxAP_Free (pstPrvArray);
	}

	if (peSvcFlagArray != NULL)
	{
		OxAP_Free (peSvcFlagArray);
	}

	return hErr;
}

STATIC HERROR xmgr_nordigsrch_AddDeliveryInfoInTpList (NordigSearch_Contents_t *pstContents)
{
	HUINT32				 ulIndex, ulNumDeliInfo;
	DbSvc_TsInfo_t		*pstDeliTsInfo, *pstDeliTsArray;
	DbSvc_TuningInfo_t	*pstSvcTuneInfo, *pstDeliSvcTuneInfo;
	POINTER_LIST_T		*pstNextTpList, *pstDeliTpList;
	POINTER_LIST_T		*pstFound, *pstRest;
	HERROR				 hErr;
	HUINT32				 ulInfoNum = 0;

	if (pstContents->pstTpToSearch == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_BUS_NO_OBJECT;
	}

	if (pstContents->pstTpToSearch->pvContents == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_BUS_NO_OBJECT;
	}

	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Delivery, &ulNumDeliInfo);
	if (hErr != ERR_OK || ulNumDeliInfo == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_BUS_NO_OBJECT;
	}

	pstDeliTsArray = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t) * ulNumDeliInfo);
	if (pstDeliTsArray == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_BUS_OUT_OF_MEMORY;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Delivery, ulNumDeliInfo, &ulInfoNum, (void *)pstDeliTsArray);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		OxAP_Free (pstDeliTsArray);
		return ERR_BUS_NO_OBJECT;
	}

	// 현재 TP (처음 item)을 pstDeliTpList의 맨 처음에 넣고,  그 후에 NextTpList에서는 뺀다.
	// 이렇게 하는 이유는 나중에 CurrTp - DeliveryTpLists - Next Tps  순으로 합치기 위해서이다.

	pstSvcTuneInfo = (DbSvc_TuningInfo_t *)pstContents->pstTpToSearch->pvContents;
	pstNextTpList = pstContents->pstTpToSearch;
	pstDeliTpList = NULL;

	pstNextTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstNextTpList, pstNextTpList, NULL);
	UTIL_LINKEDLIST_CircularizePointerList (pstNextTpList);

	pstDeliTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstDeliTpList, (void *)pstSvcTuneInfo);
	UTIL_LINKEDLIST_CircularizePointerList (pstDeliTpList);

	for (ulIndex = 0; ulIndex < ulNumDeliInfo; ulIndex++)
	{
		pstDeliTsInfo = &(pstDeliTsArray[ulIndex]);

		// 1. 이미 처리한 TP와 동일한 TP인가 ?
		pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstFinishedTps, 0,
																xmgr_nordigsrch_FilterSameTuningInfo,
																(void *)&(pstDeliTsInfo->stTuningParam));
		if (pstFound != NULL)					{ continue; }

		// 2. 현재 TP 혹은 이미 Delivery TP List에 들어간 TP와 동일한 TP 인가 ?
		pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstDeliTpList, 0,
																xmgr_nordigsrch_FilterSameTuningInfo,
																(void *)&(pstDeliTsInfo->stTuningParam));
		if (pstFound != NULL)					{ continue; }

		// 3. 이 TP가 앞으로의 Next TP에 존재하는가 ?
		// 이 경우는 시나리오에 따라 달라질 수 있다.
		// 여기서는 같은 network 인 TP들을 한 곳에 모아서 처리하기 위해
		// 뒤에 있는 Next TPs에 있는 item 쪽을 지우고 DeliTpList에 추가하는 방향으로 간다.
		pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstNextTpList, 0,
																xmgr_nordigsrch_FilterSameTuningInfo,
																(void *)&(pstDeliTsInfo->stTuningParam));
		while (pstFound != NULL)
		{
			pstNextTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstNextTpList, pstFound, xmgr_nordigsrch_FreeTuneInfo);
			UTIL_LINKEDLIST_CircularizePointerList (pstNextTpList);

			pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstNextTpList, 0,
																	xmgr_nordigsrch_FilterSameTuningInfo,
																	(void *)&(pstDeliTsInfo->stTuningParam));
		}

		// 4. Delivery TP를 추가
		pstDeliSvcTuneInfo = xmgr_nordigsrch_AllocTuneInfo();
		if (pstDeliSvcTuneInfo != NULL)
		{
			HxSTD_memcpy (pstDeliSvcTuneInfo, &(pstDeliTsInfo->stTuningParam), sizeof(DbSvc_TuningInfo_t));
			pstDeliTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstDeliTpList, (void *)pstDeliSvcTuneInfo);
			UTIL_LINKEDLIST_CircularizePointerList (pstDeliTpList);
		}
	}

	// Delivery Tp List와 남은 Next Tp List를 결합
	pstRest = NULL;
	pstContents->pstTpToSearch = UTIL_LINKEDLIST_CombineTwoPointerLists (pstDeliTpList, pstNextTpList, NULL, NULL, &pstRest);
	UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstTpToSearch);
	UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstRest, xmgr_nordigsrch_FreeTuneInfo);

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_nordigsrch_SaveResult (Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	DefaultLcnPolicy_t			eLcnPolicy;
	PreProgrammedChNumPolicy_t	eChNumPolicy;
	NordigSearch_Contents_t		*pstContents;
	HERROR						hErr;

__save_time_trace_reset__;
__search_time_trace_start__;

	(void)hAction;
	(void)ulParam1;
	(void)ulParam2;
	(void)ulParam3;

	pstContents = (NordigSearch_Contents_t *)ulParam1;
	if (pstContents == NULL)
	{
		return ERR_FAIL;
	}

	hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);

	SVC_SI_PostProcessForSimulcastService(pstContents->hAction);

	/* LCN 스펙 중, non-LCN에 대한 처리가 없으면, non-LCN 서비스는 LCN 마지막에 덧붙여 할당한다 (Humax Rule) */
	MGR_SEARCH_GetLcnPolicy (&eLcnPolicy, &eChNumPolicy);
	SVC_SI_PostProcessForSvcNumbering (pstContents->hAction, eLcnPolicy, eDxDELIVERY_TYPE_CAB);

	HxLOG_Print("(+++++ DB_SVC_RemoveSearchFlag +++++)\n");
	DB_SVC_RemoveSearchFlag();
	HxLOG_Print("(----- DB_SVC_RemoveSearchFlag -----)\n");
__save_time_trace_check__;

	hErr = ERR_BUS_NO_ERROR;
	pstContents->bSvcSaved = TRUE;
__search_time_trace_check__;

	return hErr;
}

STATIC void xmgr_nordigsrch_ReleaseAction (NordigSearch_Contents_t *pstContents)
{
	HERROR			 hErr;

	hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n\n");
	}

	// Release the action :
	MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
}

/**
  *		lock된 TP(pstTpToSearch의 맨 앞)을 제외하고 8M 내에 존재하는 tp list를 pstFinishedTps 로 이동시킨다
**/
#define OPENCABLE_JUMP_LOCK_FREQ		8000		// 8M
#define OPENCABLE_UHF_START_FREQ		474000	// 474M

STATIC HERROR xmgr_nordigsrch_RemoveNearTP(NordigSearch_Contents_t *pstContents)
{
	POINTER_LIST_T		*pstSrchTps = NULL;		// search할 TP list root
	POINTER_LIST_T		*pstCompTps = NULL;		// search 완료된 tp list root
	POINTER_LIST_T		*pstTraverse = NULL;
	DbSvc_TuningInfo_t		*pstTuneInfo = NULL;
	HUINT32				ulLockedFreq = 0;

	HxLOG_Info("\t[xmgr_nordigsrch_RemoveNearTP] ++ \n");

	pstSrchTps = pstContents->pstTpToSearch;
	pstCompTps = pstContents->pstFinishedTps;

	// check list
	if ( (pstSrchTps == NULL)
		|| (UTIL_LINKEDLIST_GetNextPointer(pstSrchTps) == pstSrchTps)
		|| (UTIL_LINKEDLIST_GetNextPointer(pstSrchTps) == NULL) )
	{
		return ERR_OK;
	}

	// get tuning tp info
	pstTuneInfo = (DbSvc_TuningInfo_t*)UTIL_LINKEDLIST_GetContents(pstSrchTps);
	if (pstTuneInfo == NULL)
	{
		return ERR_FAIL;
	}
	ulLockedFreq = pstTuneInfo->cab.ulFreq;
	HxLOG_Info("\t[xmgr_nordigsrch_RemoveNearTP] Current Locked TP Freq(%d)  \n", ulLockedFreq);

	// UHF대역에서는 remove TP 작업이 필요없다
	if (ulLockedFreq >= OPENCABLE_UHF_START_FREQ)
	{
		return ERR_OK;
	}

	pstTraverse = UTIL_LINKEDLIST_GetNextPointer(pstSrchTps);
	while ( (pstTraverse != NULL)
			&& (pstTraverse != pstSrchTps) )
	{
		// get contents
		pstTuneInfo = (DbSvc_TuningInfo_t*)UTIL_LINKEDLIST_GetContents(pstTraverse);
		if (pstTuneInfo == NULL)
		{
			break;
		}

		// check diff
		if (ulLockedFreq + OPENCABLE_JUMP_LOCK_FREQ > pstTuneInfo->cab.ulFreq)
		{
			HxLOG_Info("\t[xmgr_nordigsrch_RemoveNearTP] Remove TP Freq(%d)  \n", pstTuneInfo->cab.ulFreq);

			pstSrchTps = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstSrchTps, pstTraverse, NULL);
			//UTIL_LINKEDLIST_CircularizePointerList (pstSrchTps);

			pstCompTps = UTIL_LINKEDLIST_AppendListItemToPointerList (pstCompTps, (void *)pstTuneInfo);
			UTIL_LINKEDLIST_CircularizePointerList (pstCompTps);

		}
		else
		{
			// finish
			HxLOG_Info("\t[xmgr_nordigsrch_RemoveNearTP] Freq check finish \n");
			break;
		}

		// get 2nd tp
		pstTraverse = UTIL_LINKEDLIST_GetNextPointer(pstSrchTps);
	}

	// 필요는 없으나..
	pstContents->pstTpToSearch = pstSrchTps;
	pstContents->pstFinishedTps = pstCompTps;

	return ERR_OK;
}

#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____

//eMEVT_BUS_CREATE
STATIC HERROR xmgr_nordigsrch_MsgGwmCreate (NordigSearch_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrSearch_Option_t	*pstOption = (MgrSearch_Option_t *)p1;

	HxLOG_Info("\thAction (%08x)\n", hAction);

	if (NULL == pstOption)
	{
		BUS_MGR_Destroy (NULL);
		return ERR_BUS_NO_ERROR;
	}

	// 1. Initialize the contents
	xmgr_nordigsrch_InitContents (pstContents, hAction, pstOption, p2);

	// 2. Initialize Search Result
	MGR_SEARCH_InitResult(pstContents->ulActionId);

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY:
STATIC HERROR xmgr_nordigsrch_MsgGwmDestroyed (NordigSearch_Contents_t *pstContents)
{
	if (pstContents->eAppState == eSearchApp_state_waitingSi)
	{
		// Stop SI & Cancel the search :
		SVC_SI_StopSearch(pstContents->hAction, TRUE);
	}

	if (pstContents->eAppState != eSearchApp_state_searchend)
	{
		// Search가 끝나지 않았다면 Action Release
		xmgr_nordigsrch_ReleaseAction (pstContents);
	}

	if (pstContents->bSvcSaved == FALSE)
	{
		//something
	}
	else
	{

	}

	// Destroy the contents
	xmgr_nordigsrch_DestroyContents (pstContents);

	// Send the destoyed message to the UI procedure
	xmgr_nordigsrch_SendSearchActionDestroyed(pstContents->hAction);

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_nordigsrch_MsgAppSearchActionStart (NordigSearch_Contents_t *pstContents, HINT32 nAppHandle)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.
	HERROR			 hErr;

	// 1. Set the TP to tuning :
	xmgr_nordigsrch_SetTuningParamToSearch(pstContents);

	// 3.Get the Action
	hErr = MGR_ACTION_SetupSearch (pstContents->ulActionId, &pstContents->stSearchTuningInfo, eAmPriority_SEARCH);
	if (hErr != ERR_OK)
	{
		// search action에 대한 할당이 이루어 지지 못했으므로 search appl.을 띄울 수 없다.
		HxLOG_Critical("\n\n");

		BUS_MGR_Destroy(NULL);
		return ERR_OK;
	}

	hErr = MGR_ACTION_Take (pstContents->ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		// search action에 대한 할당이 이루어 지지 못했으므로 search appl.을 띄울 수 없다.
		HxLOG_Critical("\n\n");

		BUS_MGR_Destroy(NULL);
		return ERR_OK;
	}

	if (pstContents->eAppState != eSearchApp_state_searchend)
	{
		pstContents->eAppState = eSearchApp_state_idle;
	}

	// Tune the TP
	hErr = xmgr_nordigsrch_TuneSearchTp (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("[xmgr_nordigsrch_MsgAppSearchActionStart] No Next Tp left to tune\n");
		pstContents->eAppState = eSearchApp_state_searchend;
	}

	xmgr_nordigsrch_SendSearchStartedMsg (pstContents->hAction);

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_nordigsrch_SendSearchFinished(pstContents->hAction);
		xmgr_nordigsrch_ReleaseAction (pstContents);
	}

	return ERR_OK;

}

// eMEVT_SEARCH_SAVE_RESULT
STATIC HERROR xmgr_nordigsrch_MsgAppSearchSaveResult (NordigSearch_Contents_t *pstContents, HINT32 nAppHandle)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_nordigsrch_SaveResult(pstContents->hAction, (HUINT32)pstContents, 0, 0);
		xmgr_nordigsrch_SendChannelListUpdatedMsg(pstContents->hAction);
	}

	return ERR_OK;
}

// eMEVT_SEARCH_CANCEL
STATIC HERROR xmgr_nordigsrch_MsgAppSearchCancel (NordigSearch_Contents_t *pstContents, HINT32 nAppHandle)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.

	if (pstContents->eAppState == eSearchApp_state_waitingSi)
	{
		// Stop SI & Cancel the search :
		SVC_SI_StopSearch(pstContents->hAction, TRUE);
	}

	pstContents->eAppState = eSearchApp_state_searchend;
	xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
	xmgr_nordigsrch_ReleaseAction (pstContents);
	return ERR_OK;
}


//eMEVT_SEARCH_FINISHED
STATIC HERROR xmgr_nordigsrch_MsgAppSearchFinished(NordigSearch_Contents_t *pstContents, HINT32 nAppHandle)
{
	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_nordigsrch_SaveResult(pstContents->hAction, (HUINT32)pstContents, 0, 0);
		xmgr_nordigsrch_SendSearchSaveFinishedMsg(pstContents->hAction);
	}
	return ERR_OK;
}

//eMEVT_SEARCH_SAVE_FINISHED
STATIC HERROR xmgr_nordigsrch_MsgAppSearchSaveFinished(NordigSearch_Contents_t *pstContents, HINT32 nAppHandle)
{
	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		BUS_MGR_Destroy (NULL);
	}
	return ERR_OK;
}


// eMEVT_ACTION_NOTIFY_BGJOB_FINISHED:
STATIC HERROR xmgr_nordigsrch_MsgAppBgJobFinished (NordigSearch_Contents_t *pstContents, HINT32 nCbPtr, HINT32 nResult)
{
	(void)pstContents;
	(void)nCbPtr;
	(void)nResult;

	if (nCbPtr == (HINT32)xmgr_nordigsrch_SaveResult)
	{
		xmgr_nordigsrch_SendSearchSaveFinishedMsg(pstContents->hAction);
		return MESSAGE_BREAK;
	}

	return ERR_OK;
}

// MSG_APP_SEARCH_KILL_PROCESSES
STATIC HERROR xmgr_nordigsrch_MsgAppSearchKillProcesses (NordigSearch_Contents_t *pstContents)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.

	if (pstContents->eAppState == eSearchApp_state_waitingSi)
	{
		// Stop SI & Cancel the search :
		SVC_SI_StopSearch(pstContents->hAction, TRUE);
	}

	if (pstContents->eAppState != eSearchApp_state_searchend)
	{
		pstContents->eAppState = eSearchApp_state_searchend;
		xmgr_nordigsrch_SendSearchFinished(pstContents->hAction);
		xmgr_nordigsrch_ReleaseAction (pstContents);
	}

	BUS_MGR_Destroy (NULL);
	return ERR_OK;
}

// eMEVT_BUS_TIMER:
STATIC HERROR xmgr_nordigsrch_MsgGwmTimer (NordigSearch_Contents_t *pstContents, HINT32 nTimerId)
{
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 hErr, hRetValue;

	hRetValue = ERR_BUS_NO_ERROR;
	ePrevAppState = pstContents->eAppState;

	switch (nTimerId)
	{
	case SEARCH_TIMER_LOCKING_ID:
		BUS_KillTimer (nTimerId);

		if (pstContents->eAppState == eSearchApp_state_tryTuning)
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_nordigsrch_SetNextTpToSearch (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_nordigsrch_TuneSearchTp (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_nordigsrch_MsgGwmTimer] No Next Tp left to tune\n");
				pstContents->eAppState = eSearchApp_state_searchend;
			}
		}

		hRetValue = MESSAGE_BREAK;
		break;

	case SEARCH_TIMER_SI_ID:
		BUS_KillTimer (nTimerId);

		if (pstContents->eAppState == eSearchApp_state_waitingSi)
		{
			// MW SI를 Stop시키고 다음 TP로 넘어가도록 한다.
			hErr = SVC_SI_StopSearch(pstContents->hAction, TRUE);
			if (hErr == ERR_OK)
			{
				// 다음 TP 로 넘어가야 한다.
				hErr = xmgr_nordigsrch_SetNextTpToSearch (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_nordigsrch_TuneSearchTp (pstContents);
				}
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_nordigsrch_MsgGwmTimer] Cannot proceed anymore\n");
				pstContents->eAppState = eSearchApp_state_searchend;
			}
		}

		hRetValue = MESSAGE_BREAK;
		break;

	default:
		break;
	}

	if (ePrevAppState != pstContents->eAppState)
	{
		// set UI button text
		xmgr_nordigsrch_ChangeProgressNumber (pstContents);
		xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_nordigsrch_SendSearchFinished(pstContents->hAction);
		xmgr_nordigsrch_ReleaseAction (pstContents);
	}

	return hRetValue;
}


/**
 * Telenor
 * - Home TP를 순차적으로 찾아서 실제 TP List를 얻게 되면, 나머지 Home TP들은 Search 하지 않습니다.
 * - Example) 306000/6875/64-QAM에서 Locking 되고 TP List가 얻어지면, 나머지 17개의 TP는 무시 됩니다.
 */

STATIC HERROR xmgr_nordigsrch_MakeNitTpList (NordigSearch_Contents_t* pstContents)
{
	HUINT32							ulCount;
	HUINT32					 		ulNumTs = 0;
	DbSvc_TsInfo_t					*astTsInfo = NULL;
	DbSvc_TuningInfo_t				*pstTuneInfo;
	POINTER_LIST_T					*pstSrchTPs;
	DbSvc_TsInfo_t					*pstTsArray = NULL;
	HUINT32					 		ulItemNum;
	HERROR				 	 		hErr = ERR_FAIL;
	HUINT32				 	 		ulInfoNum = 0;

	HxLOG_Print("Enter > xmgr_nordigsrch_MakeNitTpList \n");

	if (pstContents == NULL)					{ return ERR_FAIL; }
	if (pstContents->pstTpToSearch == NULL)		{ return ERR_FAIL; }


	/* (1) NIT TS Loop에 있는 TP List 정보를 가져온다. */
	//------------------------------------------------------------------------------
	// TS Info를 얻어오는 것이 제일 중요하다.
	// 여기서 TS Info를 얻어오지 못하면 이 TP를 PASS 하고 계속해서 Home TP를 찾는다.
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Delivery, &ulItemNum);
	if (hErr != ERR_OK)					{ ulItemNum = 0; }

	HxLOG_Print("eSI_SRCH_INFOTYPE_Transponder: ulItemNum [%d]\n", ulItemNum);

	if (ulItemNum == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_BUS_NO_OBJECT;
	}

	// NIT의 System Delivery descriptor 와 SDT ACT/SDT OTH의 조합으로 TS Info를 구성한다.
	pstTsArray = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t) * ulItemNum);
	if (pstTsArray == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_BUS_OUT_OF_MEMORY;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Delivery, ulItemNum, &ulInfoNum, (void *)pstTsArray);
	if (hErr != ERR_OK)
	{
		OxAP_Free(pstTsArray);
		HxLOG_Critical("\n\n");
		return ERR_BUS_NO_OBJECT;
	}

	astTsInfo = pstTsArray;
	ulNumTs = ulItemNum;

	/* (2) 나머지 Home TP 삭제 */
	pstSrchTPs = pstContents->pstTpToSearch;
	pstTuneInfo = (DbSvc_TuningInfo_t*)UTIL_LINKEDLIST_GetContents(pstSrchTPs);

	/* delete current node without content */
	pstSrchTPs = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstSrchTPs, pstSrchTPs, NULL);

	/* free whole tp lists */
	pstSrchTPs = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pstSrchTPs, xmgr_nordigsrch_FreeTuneInfo);

	/* Append Searched tp information */
	pstSrchTPs = UTIL_LINKEDLIST_AppendListItemToPointerList (pstSrchTPs, (void *)pstTuneInfo);
	UTIL_LINKEDLIST_CircularizePointerList (pstSrchTPs);

	for (ulCount = 0; ulCount < ulNumTs; ulCount++)
	{
		pstTuneInfo = (DbSvc_TuningInfo_t *)xmgr_nordigsrch_AllocTuneInfo();
		if (NULL != pstTuneInfo)
		{
			*pstTuneInfo = astTsInfo[ulCount].stTuningParam;
			if (pstTuneInfo->cab.ulFreq == 0)	/* skip */
			{
				xmgr_nordigsrch_FreeTuneInfo(pstTuneInfo);
				continue;
			}
			else
			{
				pstSrchTPs = UTIL_LINKEDLIST_AppendListItemToPointerList (pstSrchTPs, (void *)pstTuneInfo);
				UTIL_LINKEDLIST_CircularizePointerList (pstSrchTPs);
			}
			HxLOG_Print("(%d) NetID [%d], %d MHz, %d\n", ulCount, pstTuneInfo->cab.usNetworkId, pstTuneInfo->cab.ulFreq / 1000, pstTuneInfo->cab.usSR);
		}
	}

	pstContents->pstTpToSearch = pstSrchTPs;
	pstContents->bNitTpList = FALSE;

	return ERR_OK;
}

// eSEVT_SI_SEARCH_DONE:

STATIC HERROR _xmgr_nordigsrch_EvtSiSearchDone_Normal(NordigSearch_Contents_t *pstContents)
{
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 hErr = ERR_FAIL;

	ePrevAppState = pstContents->eAppState;

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_waitingSi:
		// SI succeeded : Kill the timer

		BUS_KillTimer (SEARCH_TIMER_SI_ID);
		pstContents->eAppState = eSearchApp_state_siReceived;
		hErr = xmgr_nordigsrch_RetrieveResults_Normal (pstContents);
		{
			HERROR		hError;
			DbSvc_NetInfo_t stNetInfo, *pstNetInfo;

			HxSTD_MemSet(&stNetInfo, 0, sizeof(DbSvc_NetInfo_t));
			pstNetInfo = NULL;
			hError = MGR_SEARCHUTIL_MakeDbNetInfoFromSi(pstContents->hAction, pstContents->usNetworkId, &stNetInfo) ;
			if (hError == ERR_OK)
			{
				pstNetInfo = &stNetInfo;
			}

			xmgr_nordigsrch_ChangeTpText (pstContents, pstNetInfo);
			xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
		}

		if (hErr == ERR_OK && pstContents->bNetworkSearch == TRUE)
		{
			xmgr_nordigsrch_AddDeliveryInfoInTpList (pstContents);
		}

		SVC_SI_StopSearch(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_nordigsrch_SetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_nordigsrch_TuneSearchTp (pstContents);
		}

		// Tuning Failure :
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_nordigsrch_EvtSiSearchDone] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	default:
		break;
	}

	if (ePrevAppState != pstContents->eAppState)
	{
		// set UI button text
		xmgr_nordigsrch_ChangeProgressNumber (pstContents);
		xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_nordigsrch_SendSearchFinished(pstContents->hAction);
		xmgr_nordigsrch_ReleaseAction (pstContents);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR _xmgr_nordigsrch_EvtSiSearchDone_NitActBased(NordigSearch_Contents_t *pstContents)
{
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 	 hErr = ERR_FAIL;

	ePrevAppState = pstContents->eAppState;

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_waitingSi:
		// SI succeeded : Kill the timer

		BUS_KillTimer (SEARCH_TIMER_SI_ID);
		pstContents->eAppState = eSearchApp_state_siReceived;

		if(pstContents->bNitTpList == TRUE) // Search for Home TP
		{
			// Home TP를 찾아서 TP List를 구성한다. 채널은 버린다.
			if (ERR_OK == xmgr_nordigsrch_MakeNitTpList(pstContents))
			{
				HxLOG_Print("Yeah! Make NIT TP List! \n");
			}
			else
			{
				HxLOG_Print("Can't Make NIT TP List.. \n");
			}
		}
		else
		{
			DbSvc_NetInfo_t stNetInfo, *pstNetInfo;


			hErr = xmgr_nordigsrch_RetrieveResults_NitActBased (pstContents);

			if (hErr == ERR_OK && pstContents->bNetworkSearch == TRUE)
			{
				xmgr_nordigsrch_AddDeliveryInfoInTpList (pstContents);
			}

			HxSTD_MemSet(&stNetInfo, 0, sizeof(DbSvc_NetInfo_t));
			pstNetInfo = NULL;
			hErr = MGR_SEARCHUTIL_MakeDbNetInfoFromSi(pstContents->hAction, pstContents->usNetworkId, &stNetInfo) ;
			if (hErr == ERR_OK)
			{
				pstNetInfo = &stNetInfo;
			}

			xmgr_nordigsrch_ChangeTpText (pstContents, pstNetInfo);
			xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
		}

		SVC_SI_StopSearch(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_nordigsrch_SetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_nordigsrch_TuneSearchTp (pstContents);
		}

		// Tuning Failure :
		if (hErr != ERR_OK)
		{
			HxLOG_Print("No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	default:
		break;
	}

	HxLOG_Print("ePrevAppState = %d, pstContents->eAppState = %d\n", ePrevAppState, pstContents->eAppState);
	if (ePrevAppState != pstContents->eAppState)
	{
		// set UI button text
		xmgr_nordigsrch_ChangeProgressNumber (pstContents);
		xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_nordigsrch_SendSearchFinished(pstContents->hAction);
		xmgr_nordigsrch_ReleaseAction (pstContents);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR _xmgr_nordigsrch_EvtSiSearchDone_NitOthBased(NordigSearch_Contents_t *pstContents)
{
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 hErr = ERR_FAIL;

	ePrevAppState = pstContents->eAppState;

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_waitingSi:
		// SI succeeded : Kill the timer

		BUS_KillTimer (SEARCH_TIMER_SI_ID);
		pstContents->eAppState = eSearchApp_state_siReceived;
		hErr = xmgr_nordigsrch_RetrieveResults_NitOthBased (pstContents);

		{
			HERROR		hError;
			DbSvc_NetInfo_t stNetInfo, *pstNetInfo;

			HxSTD_MemSet(&stNetInfo, 0, sizeof(DbSvc_NetInfo_t));
			pstNetInfo = NULL;
			hError = MGR_SEARCHUTIL_MakeDbNetInfoFromSi(pstContents->hAction, pstContents->usNetworkId, &stNetInfo) ;
			if (hError == ERR_OK)
			{
				pstNetInfo = &stNetInfo;
			}

			xmgr_nordigsrch_ChangeTpText (pstContents, pstNetInfo);
			xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
		}

		if (hErr == ERR_OK && pstContents->bNetworkSearch == TRUE)
		{
			xmgr_nordigsrch_AddDeliveryInfoInTpList (pstContents);
		}

		SVC_SI_StopSearch(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_nordigsrch_SetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_nordigsrch_TuneSearchTp (pstContents);
		}

		// Tuning Failure :
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_nordigsrch_EvtSiSearchDone] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	default:
		break;
	}

	HxLOG_Print("ePrevAppState = %d, pstContents->eAppState = %d\n", ePrevAppState, pstContents->eAppState);
	if (ePrevAppState != pstContents->eAppState)
	{
		// set UI button text
		xmgr_nordigsrch_ChangeProgressNumber (pstContents);
		xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_nordigsrch_SendSearchFinished(pstContents->hAction);
		xmgr_nordigsrch_ReleaseAction (pstContents);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_nordigsrch_EvtSiSearchDone (NordigSearch_Contents_t *pstContents)
{
	if(NULL == pstContents)
		return ERR_FAIL;

	switch(pstContents->lSearchType)
	{
	case eSVCSI_SEARCHTYPE_NORDIG_NitActBased:
		return _xmgr_nordigsrch_EvtSiSearchDone_NitActBased(pstContents);

	case eSVCSI_SEARCHTYPE_NORDIG_NitOthBased:
		return _xmgr_nordigsrch_EvtSiSearchDone_NitOthBased(pstContents);

	case eSVCSI_SEARCHTYPE_NORDIG_WithoutNit:
		// TODO:
	case eSVCSI_SEARCHTYPE_Normal:
	default:
		return _xmgr_nordigsrch_EvtSiSearchDone_Normal(pstContents);
	}
}

// eSEVT_SI_SEARCH_FAIL:
STATIC HERROR xmgr_nordigsrch_EvtSiSearchFail (NordigSearch_Contents_t *pstContents)
{
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 hErr;

	ePrevAppState = pstContents->eAppState;

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_waitingSi:
		// SI failed : Kill the timer

		BUS_KillTimer (SEARCH_TIMER_SI_ID);

		pstContents->eAppState = eSearchApp_state_siFailed;

		SVC_SI_StopSearch(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_nordigsrch_SetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_nordigsrch_TuneSearchTp (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_nordigsrch_EvtSiSearchFail] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			break;
		}

		break;

	default:
		break;
	}

	if (ePrevAppState != pstContents->eAppState)
	{
		// set UI button text
		xmgr_nordigsrch_ChangeProgressNumber (pstContents);
		xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_nordigsrch_SendSearchFinished(pstContents->hAction);
		xmgr_nordigsrch_ReleaseAction (pstContents);
	}

	return MESSAGE_BREAK;
}

// eSEVT_CH_LOCKED
STATIC HERROR xmgr_nordigsrch_EvtChLocked (NordigSearch_Contents_t *pstContents)
{
	HUINT32				 ulNetId = SvcSi_NETID_SEARCH_NITACT;
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 hErr;

	ePrevAppState = pstContents->eAppState;

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_tryTuning :
		// Channel Locked : Kill the timer

		// BUS_KillTimer (SEARCH_TIMER_LOCKING_ID);

		HxLOG_Print("pstContents->bNetworkSearch [%d]\n", pstContents->bNetworkSearch);
		HxLOG_Print("pstContents->usNetworkId    [%d](0x%x)\n", pstContents->usNetworkId, pstContents->usNetworkId);

		if (pstContents->usNetworkId > 0 && pstContents->usNetworkId < 0xFFFF)
		{
			ulNetId = pstContents->usNetworkId;
		}

		// check cable mode - open cable search (network search가 off인 경우-full search-에만 8M 이내 tp를 지운다)
		if ( (pstContents->bIsOpenCable == TRUE)
			&& (pstContents->bNetworkSearch == FALSE) )
		{
			// open cable에서 lock되면 다음 tp를 freq 8M 이후로 설정해 주어야 한다
			hErr = xmgr_nordigsrch_RemoveNearTP(pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("xmgr_nordigsrch_RemoveNearTP() FAIL \n");
			}
		}

		HxLOG_Print("ulNetId = %d\n", ulNetId);
		{
			SvcSi_SearchSpec_t 		stSrchSpec;
			SvcSi_SearchOption_t	stSrchOption;

			HxSTD_memset(&stSrchOption, 0x00, sizeof(SvcSi_SearchOption_t));
			stSrchOption.ulSearchType	= (HUINT32)pstContents->lSearchType;
			stSrchOption.ulSvcType		= (HUINT32)pstContents->eServiceType;
			stSrchOption.ulCasType		= (HUINT32)pstContents->eCAFreeType;
			stSrchOption.hSession		= (SvcSi_SessionH_t)0;
			stSrchOption.ulNetId		= ulNetId;

			hErr = MGR_SEARCH_GetSiSpec(&stSrchOption, &stSrchSpec);
			if (hErr != ERR_FAIL)
			{
				hErr = SVC_SI_StartSearch(pstContents->hAction,
												&stSrchSpec,
												&stSrchOption,
												TRUE);
			}

		    if (hErr == ERR_OK)
		    {
			    pstContents->eAppState = eSearchApp_state_waitingSi;
			    // MW SI를 무한으로 기다리는 것을 막기 위해 여기에서 Timer를 걸어야 한다.
			    BUS_SetTimer (SEARCH_TIMER_SI_ID, SEARCH_TIMER_SI_TIME);
		    }
		    else
		    {
			    // 다음 TP 로 넘어가야 한다.
			    hErr = xmgr_nordigsrch_SetNextTpToSearch (pstContents);
			    if (hErr == ERR_OK)
			    {
				    // Tune the TP
				    hErr = xmgr_nordigsrch_TuneSearchTp (pstContents);
			    }

			    if (hErr != ERR_OK)
			    {
				    HxLOG_Print("[xmgr_nordigsrch_EvtChLocked] No Next Tp left to tune\n");
				    pstContents->eAppState = eSearchApp_state_searchend;
			    }
		    }
		}
		break;

	default:
		break;
	}

	if (ePrevAppState != pstContents->eAppState)
	{
		// set UI button text
		xmgr_nordigsrch_ChangeProgressNumber (pstContents);
		xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_nordigsrch_SendSearchFinished(pstContents->hAction);
		xmgr_nordigsrch_ReleaseAction (pstContents);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_nordigsrch_EvtChNoSignal (NordigSearch_Contents_t *pstContents)
{
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 hErr;

	ePrevAppState = pstContents->eAppState;

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_tryTuning :
		// Channel Locked : Kill the timer

		// BUS_KillTimer (SEARCH_TIMER_LOCKING_ID);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_nordigsrch_SetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_nordigsrch_TuneSearchTp (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_nordigsrch_EvtChNoSignal] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	case eSearchApp_state_waitingSi:
		// Stop SI :
		hErr = SVC_SI_StopSearch(pstContents->hAction, TRUE);
		if (hErr == ERR_OK)
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_nordigsrch_SetNextTpToSearch (pstContents);
			if (hErr == ERR_OK)
			{
				// Tune the TP
				hErr = xmgr_nordigsrch_TuneSearchTp (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_nordigsrch_EvtChNoSignal] No Next Tp left to tune\n");
				pstContents->eAppState = eSearchApp_state_searchend;
			}
		}
		else
		{
			// 더 이상 진행 불가능 : Search End로 간주한다.
			HxLOG_Print("[xmgr_nordigsrch_EvtChNoSignal] SI Re-open failed.\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	default:
		break;
	}

	if (ePrevAppState != pstContents->eAppState)
	{
		// set UI button text
		xmgr_nordigsrch_ChangeProgressNumber (pstContents);
		xmgr_nordigsrch_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_nordigsrch_SendSearchFinished(pstContents->hAction);
		xmgr_nordigsrch_ReleaseAction (pstContents);
	}

	return MESSAGE_BREAK;
}


/**
  @brief      Search service application main 함수 .

  Function Name: AP_CabSrchSvc_Proc()
  Function Description: manager proc 함수
  Parameter: proc parameters
  type: (Sync or Async) async 함수
  Return Value:
*/
STATIC BUS_Result_t			xproc_search_nordig_Proto(NordigSearch_Contents_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			 hErr;

	HxLOG_Info("in message(0x%x)\n", message);

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Print("\t[eMEVT_BUS_CREATE]\n\n");
__search_time_trace_start__;
		hErr = xmgr_nordigsrch_MsgGwmCreate (pstContents, hAction, p1, p2, p3);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Print("\t[eMEVT_BUS_TIMER]\n\n");
		hErr = xmgr_nordigsrch_MsgGwmTimer (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Print("\t[eMEVT_BUS_DESTROY]\n\n");
		hErr = xmgr_nordigsrch_MsgGwmDestroyed (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_ACTION_START:
		HxLOG_Print("\t[eMEVT_SEARCH_ACTION_START]\n\n");
		hErr = xmgr_nordigsrch_MsgAppSearchActionStart (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_SAVE_RESULT:
		HxLOG_Print("\t[eMEVT_SEARCH_SAVE_RESULT]\n\n");
		hErr = xmgr_nordigsrch_MsgAppSearchSaveResult (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_CANCEL:
		HxLOG_Print("\t[eMEVT_SEARCH_CANCEL]\n\n");
		hErr = xmgr_nordigsrch_MsgAppSearchCancel (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_FINISHED:
		HxLOG_Print("\t[eMEVT_SEARCH_FINISHED]\n\n");
		hErr =xmgr_nordigsrch_MsgAppSearchFinished (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_ACTION_NOTIFY_BGJOB_FINISHED:
		HxLOG_Print("\t[eMEVT_ACTION_NOTIFY_BGJOB_FINISHED]\n\n");
		hErr = xmgr_nordigsrch_MsgAppBgJobFinished (pstContents, p1, p2);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_ACTION_KILL:
		HxLOG_Print("\t[eMEVT_SEARCH_ACTION_KILL]\n\n");
		hErr = xmgr_nordigsrch_MsgAppSearchKillProcesses (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SI_SEARCH_DONE:
		HxLOG_Print("\t[eSEVT_SI_SEARCH_DONE]\n\n");
		hErr = xmgr_nordigsrch_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		hErr = xmgr_nordigsrch_EvtSiSearchDone (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SI_SEARCH_FAIL:
		HxLOG_Print("\t[eSEVT_SI_SEARCH_FAIL]\n\n");
		hErr = xmgr_nordigsrch_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		hErr = xmgr_nordigsrch_EvtSiSearchFail (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_LOCKED:
		HxLOG_Print("\t[eSEVT_CH_LOCKED]\n\n");
		hErr = xmgr_nordigsrch_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		hErr = xmgr_nordigsrch_EvtChLocked (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_NO_SIGNAL:
		HxLOG_Print("\t[eSEVT_CH_NO_SIGNAL]\n\n");
		hErr = xmgr_nordigsrch_CheckActionId (pstContents, hAction);
		if(MESSAGE_PASS != hErr)
		{
			hErr = xmgr_nordigsrch_EvtChNoSignal (pstContents);
		}
		return hErr;

	case eMEVT_SEARCH_SAVE_FINISHED:
		HxLOG_Print("\t[eMEVT_SEARCH_SAVE_FINISHED]\n\n");
		hErr = xmgr_nordigsrch_MsgAppSearchSaveFinished (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	default:
		break;
	}

	HxLOG_Info("out\n");

	MGR_SEARCHRESULT_SetActionState (pstContents->ulActionId, pstContents->eAppState);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#define	_____EXTERNAL_PROCESS_FUNCTION_____

BUS_Result_t		xproc_search_nordig_Action_0 (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HUINT8*)__FUNCTION__);
	}

	return xproc_search_nordig_Proto(&(s_NordigSrchApp_Contents[0]), message, hAction, p1, p2, p3);
}

BUS_Result_t		xproc_search_nordig_Action_1 (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HUINT8*)__FUNCTION__);
	}

	return xproc_search_nordig_Proto(&(s_NordigSrchApp_Contents[1]), message, hAction, p1, p2, p3);
}



/* End of File. ---------------------------------------------------------- */
