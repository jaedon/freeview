/**
	@file     ap_ter_search_action.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/11/06		initiated					jinh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <linkedlist.h>
#include <bus.h>

#include <db_common.h>
#include <db_param.h>
#include <db_svc.h>

#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_sys.h>

#include <isosvc.h>
#include <svc_resources.h>


#include <mgr_rscmap.h>
#include <mgr_action.h>
#include <mgr_search.h>
#include <mgr_spec.h>
#include "../local_include/_xmgr_search.h"

#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




#define TER_SIGNAL_QUALITY_THRESHOLD		5
#define TER_SIGNAL_LEVEL_THRESHOLD			5

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


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief search application에서 실행에 필요한 environment context  */
typedef struct tagTerSrchApp_Contents_t
{
	HUINT32						 ulActionId;			// Search Application Action ID
	Handle_t					 hAction;				// Search Action handle
	HUINT32						 ulTunerId;				// Tuner ID

	// Search Options :
	DxCasType_e					 eCAFreeType;			//  ALL, FTA, CAS
	DxSvcType_e					 eServiceType;			// ALL, TV, Radio
	HBOOL						 bNetworkSearch;		// TRUE, FALSE
	HUINT32						 ulNetworkId;			// Network Id
	HUINT32						 ucSearchType;			// Search Type ... (for OP spec.)

	MgrSearch_AppState_e			 eAppState;				// Current Search State :
	HBOOL						 bSvcSaved;				// Searched Services are saved:

	// == TP List to be searched:
	POINTER_LIST_T				*pstTpToSearch;
	POINTER_LIST_T				*pstFinishedTps;

	// == Searched Compontns ==
	MgrSearch_PrvInfoArray_t			 stPrvArray;
	MgrSearch_NetInfoArray_t			 stNetArray;
	MgrSearch_TsInfoArray_t			 stTsArray;
	MgrSearch_BqInfoArray_t			 stBqArray;

	// === Tuning Informations ===
	DxTuneParam_t				 stSearchTuningInfo;

	// === Searched Service Numbers
	HUINT32						 ulNumOfTvSearched;				// Number of TV searched
	HUINT32						 ulNumOfRadioSearched;			// Number of Radio searched
	HUINT32						 ulNumOfDataSearched;			// Number of Data searched
} TerSrchApp_Contents_t;



static TerSrchApp_Contents_t	s_TerSrchApp_Contents[NUM_SEARCH_ACTION];				/**< Search Appl. runtime context */


/* internal function prototype */

STATIC BUS_Result_t xproc_search_ter_Proto(TerSrchApp_Contents_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


#define	_________________________Body___________________________________




static HERROR xmgr_search_ter_ChangeTpText (TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_ChangeProgressNumber (TerSrchApp_Contents_t *pstContents);

static INLINE HERROR xmgr_search_ter_CheckActionId (TerSrchApp_Contents_t *pstContents, Handle_t hAction);
static DbSvc_TuningInfo_t *xmgr_search_ter_GetCurrentTuningInfo (TerSrchApp_Contents_t *pstContents);
static INLINE void xmgr_search_ter_IncreaseSessionVersion (TerSrchApp_Contents_t *pstContents);

static HERROR xmgr_search_ter_InitContents (TerSrchApp_Contents_t *pstContents, Handle_t hAction, MgrSearch_Option_t *pstOption);
static HERROR xmgr_search_ter_MakeTpList (TerSrchApp_Contents_t* pstContents, MgrSearch_Option_t *pstOption);

static HERROR xmgr_search_ter_SetTuningParamToSearch (TerSrchApp_Contents_t *pstContents);

static HERROR xmgr_search_ter_DestroyContents (TerSrchApp_Contents_t *pstContents);

static HERROR xmgr_search_ter_TuneSearchTp (TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_SetNextTpToSearch (TerSrchApp_Contents_t *pstContents);

static HERROR xmgr_search_ter_SendUpdateDisplayMsg (TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_SendSearchStartedMsg (Handle_t hAction);
static HERROR xmgr_search_ter_SendSearchFinished (Handle_t hAction);
static HERROR xmgr_search_ter_SendSearchSaveFinishedMsg (Handle_t hAction);
static HERROR xmgr_search_ter_SendSearchActionDestroyed (Handle_t hAction);

static HERROR xmgr_search_ter_ProcessSearchEnd(TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_IsSignalStrongerThanDbTsInfo (TerSrchApp_Contents_t* pstContents, DbSvc_TsInfo_t *pstNewTsInfo, DbSvc_TsInfo_t *pstDbTsInfo, HBOOL *pbStronger);
static HERROR xmgr_search_ter_RetrieveResults (TerSrchApp_Contents_t* pstContents);
static HERROR xmgr_search_ter_AddDeliveryInfoInTpList (TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_AddLpTuningInfoInTpList (TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_SaveResult (Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static HBOOL xmgr_search_ter_FilterSameTuningInfo (void *pvItem, void *pvFilter);
static DbSvc_TuningInfo_t *xmgr_search_ter_AllocTuneInfo (void);
static void xmgr_search_ter_FreeTuneInfo (void *pvTuneInfo);
static void xmgr_search_ter_ReleaseAction (TerSrchApp_Contents_t *pstContents);


static HERROR xmgr_search_ter_MsgGwmCreate (TerSrchApp_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static HERROR xmgr_search_ter_MsgGwmDestroyed (TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_MsgAppSearchActionStart (TerSrchApp_Contents_t *pstContents, HINT32 nAppHandle);
static HERROR xmgr_search_ter_MsgAppSearchSaveResult (TerSrchApp_Contents_t *pstContents, HINT32 nAppHandle);
static HERROR xmgr_search_ter_MsgAppSearchCancel (TerSrchApp_Contents_t *pstContents, HINT32 nAppHandle);
static HERROR xmgr_search_ter_MsgAppBgJobFinished (TerSrchApp_Contents_t *pstContents, HINT32 nCbPtr, HINT32 nResult);
static HERROR xmgr_search_ter_MsgAppSearchKillProcesses (TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_MsgGwmTimer (TerSrchApp_Contents_t *pstContents, HINT32 nTimerId);
static HERROR xmgr_search_ter_EvtSiSearchDone (TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_EvtSiSearchFail (TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_EvtChLocked (TerSrchApp_Contents_t *pstContents);
static HERROR xmgr_search_ter_EvtChNoSignal (TerSrchApp_Contents_t *pstContents);



#define	____________________for__UI_____________________

static HERROR xmgr_search_ter_ChangeTpText (TerSrchApp_Contents_t *pstContents)
{
	DbSvc_TuningInfo_t	 stTuneInfo;

//	MWC_UTIL_ConvCh2DbTuningParam (&(pstContents->stSearchTuningInfo), &(stTuneInfo));
	OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&(pstContents->stSearchTuningInfo), &(stTuneInfo));

	MGR_SEARCHRESULT_SetTransponder (pstContents->ulActionId, pstContents->stSearchTuningInfo.eDeliType, stTuneInfo);

	return ERR_OK;
}
static HERROR xmgr_search_ter_ChangeProgressNumber (TerSrchApp_Contents_t *pstContents)
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


#define _________________________________________________________________________________

static INLINE HERROR xmgr_search_ter_CheckActionId (TerSrchApp_Contents_t *pstContents, Handle_t hAction)
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

static INLINE void xmgr_search_ter_IncreaseSessionVersion (TerSrchApp_Contents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}



static DbSvc_TuningInfo_t *xmgr_search_ter_GetCurrentTuningInfo (TerSrchApp_Contents_t *pstContents)
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


#define	_____CONTENTS_INITIATION_____

static HERROR xmgr_search_ter_InitContents (TerSrchApp_Contents_t *pstContents, Handle_t hAction, MgrSearch_Option_t *pstOption)
{
	// 1. Clear all the contents.
	HxSTD_memset (pstContents, 0, sizeof(TerSrchApp_Contents_t));

	// 2. Action Handle and Action ID, and Tuner ID
	pstContents->ulTunerId = (HUINT32)-1;
	pstContents->ulActionId = MGR_ACTION_GetMainActionId();
	pstContents->hAction	= SVC_PIPE_GetActionHandle (pstContents->ulActionId);

	pstContents->bSvcSaved	= FALSE;

	// 3. Init the search result components:
	MGR_SEARCHDATA_InitProviderArray (&(pstContents->stPrvArray));
	MGR_SEARCHDATA_InitNetworkArray (&(pstContents->stNetArray));
	MGR_SEARCHDATA_InitTsArray (&(pstContents->stTsArray));
	MGR_SEARCHDATA_InitBouquetArray (&(pstContents->stBqArray));

	// 4. Search Options
	pstContents->eCAFreeType	= pstOption->eCasType;				// Scramble On/Off
	pstContents->eServiceType	= pstOption->eSvcType;				// Channel Type
	pstContents->bNetworkSearch	= pstOption->bNetworkSearch;		// Network Search
	pstContents->ucSearchType	= pstOption->eSearchType;			// Search Type : 임시적인 코드 : 실제로는 추후 구현 필요
	pstContents->ulNetworkId		= pstOption->usNetwork_id;		// Network ID

	// 6. Now the tuning information :
	xmgr_search_ter_MakeTpList (pstContents, pstOption);

	/* 검색 목적이 아닌 기존 채널을 지우기 위해 search action 이용 - 에러 상황 아님. */
	if (pstContents->pstTpToSearch == NULL)
	{
		MGR_SEARCH_InitResult(pstContents->ulActionId);				/* 기존 검색 결과를 clear 해줌. */
		MGR_SEARCHRESULT_SetNumTpCompleted (pstContents->ulActionId, 1);	/* search succcess 처리하기 위해(meta svc save 하기 위함.) complete 설정 */
		MGR_SEARCHRESULT_SetNumTpTotal (pstContents->ulActionId, 1);		/* search succcess 처리하기 위해(meta svc save 하기 위함.) complete 설정 */
		xmgr_search_ter_SendUpdateDisplayMsg(pstContents);
		BUS_MGR_Destroy (NULL);
		return ERR_BUS_NO_ERROR;
	}

	return ERR_BUS_NO_ERROR;
}

static HERROR xmgr_search_ter_MakeTpList (TerSrchApp_Contents_t* pstContents, MgrSearch_Option_t *pstOption)
{
	HUINT32				 ulCount;
	DbSvc_TuningInfo_t	*pstTuneInfo;

	if ((0 == pstOption->ulTpNum) || (NULL == pstOption->pstTpArray))
	{
		return ERR_FAIL;
	}

	for (ulCount = 0; ulCount < pstOption->ulTpNum; ulCount++)
	{
		pstTuneInfo = xmgr_search_ter_AllocTuneInfo();
		if (NULL != pstTuneInfo)
		{
			*pstTuneInfo = pstOption->pstTpArray[ulCount].stTuningParam;
			pstContents->pstTpToSearch = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTpToSearch, (void *)pstTuneInfo);
			UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstTpToSearch);
		}
	}

	return ERR_OK;
}




#define	_____CONTENTS_MODIFICATION_____

static HERROR xmgr_search_ter_SetTuningParamToSearch (TerSrchApp_Contents_t *pstContents)
{
	DbSvc_TsInfo_t		 stSvcTsInfo;
	DbSvc_TuningInfo_t	*pstCurrTp;

	// No more TP to be searched
	if (pstContents->pstTpToSearch == NULL)
	{
		// Finish the search
		pstContents->eAppState = eSearchApp_state_searchend;
		return ERR_BUS_NO_OBJECT;
	}

	pstContents->eAppState = eSearchApp_state_idle;

	// Get the first TP for tuning :
	pstCurrTp = (DbSvc_TuningInfo_t *)(pstContents->pstTpToSearch->pvContents);
	if (pstCurrTp == NULL)
	{
		// Finish the search
		pstContents->eAppState = eSearchApp_state_searchend;
		return ERR_BUS_NO_OBJECT;
	}

	// current Tp로 튜닝info 채워 준다.
	stSvcTsInfo.eDeliType = eDxDELIVERY_TYPE_TER;
	stSvcTsInfo.ucTunerId = (HUINT8)pstContents->ulTunerId;
	HxSTD_memcpy (&(stSvcTsInfo.stTuningParam), pstCurrTp, sizeof(DbSvc_TuningInfo_t));

	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stSvcTsInfo, &(pstContents->stSearchTuningInfo));

	/* Tp info 변경 */
	xmgr_search_ter_ChangeTpText (pstContents);

	/* status bar update */
	xmgr_search_ter_ChangeProgressNumber (pstContents);

	xmgr_search_ter_SendUpdateDisplayMsg(pstContents);

	return ERR_BUS_NO_ERROR;
}


#define	_____CONTENTS_DESTRUCTION_____

static HERROR xmgr_search_ter_DestroyContents (TerSrchApp_Contents_t *pstContents)
{
	// Clear the search results :
	MGR_SEARCHDATA_ClearProviderArray (&(pstContents->stPrvArray));
	MGR_SEARCHDATA_ClearNetworkArray (&(pstContents->stNetArray));
	MGR_SEARCHDATA_ClearTsArray (&(pstContents->stTsArray));
	MGR_SEARCHDATA_ClearBouquetArray (&(pstContents->stBqArray));

 	pstContents->pstTpToSearch = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTpToSearch, xmgr_search_ter_FreeTuneInfo);
 	pstContents->pstFinishedTps = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstFinishedTps, xmgr_search_ter_FreeTuneInfo);

	return ERR_BUS_NO_ERROR;
 }


#define	_____DATA_CONVERSION_____



#define	_____STATE_PROCESSING_FUNCTIONS_____

static HERROR xmgr_search_ter_TuneSearchTp (TerSrchApp_Contents_t *pstContents)
{
	HBOOL			 bRetry;
	SvcRsc_Info_t		 stRscInfo;
	HERROR			 hErr;

	bRetry = TRUE;
	MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);

	while (bRetry == TRUE)
	{
		hErr = xmgr_search_ter_SetTuningParamToSearch (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_search_ter_TuneSearchTp] No Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			return ERR_BUS_NO_OBJECT;
		}

		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_search_ter_TuneSearchTp] Tuner Stop Failed\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			return ERR_BUS_NO_OBJECT;
		}

		xmgr_search_ter_IncreaseSessionVersion (pstContents);
		hErr = SVC_CH_StartTune(pstContents->hAction, &stRscInfo, (HUINT16)-1, &pstContents->stSearchTuningInfo);

#if defined(CONFIG_MW_CH_DVB_T2)
		HxLOG_Print(" \n");
		HxLOG_Print("[TUNING] => ulFrequency : %d \n",pstContents->stSearchTuningInfo.ter.ulFrequency);
		HxLOG_Print("[TUNING] => bandWidth : %d \n",pstContents->stSearchTuningInfo.ter.eBandWidth);
		HxLOG_Print("[TUNING] => ulPlpId : %d \n",pstContents->stSearchTuningInfo.ter.stT2Param.ulPlpId );
#endif
		if (hErr == ERR_OK)
		{
			pstContents->eAppState = eSearchApp_state_tryTuning;

			// DD에서 Tuning Signal이 올라오지 않아 무한으로 기다리는 것을 막기 위해 여기서 timer를 처리하도록 한다.
			// BUS_SetTimer (SEARCH_TIMER_LOCKING_ID, SEARCH_TIMER_LOCKING_TIME);
			return ERR_BUS_NO_ERROR;
		}

		// Tuning failure : Go to the next TP
		hErr = xmgr_search_ter_SetNextTpToSearch (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_search_ter_TuneSearchTp] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			bRetry = FALSE;
		}
	}

	// All TP Completed:

	return ERR_BUS_NO_OBJECT;
}

static HERROR xmgr_search_ter_SetNextTpToSearch (TerSrchApp_Contents_t *pstContents)
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
		return ERR_BUS_NO_OBJECT;
	}

	return ERR_BUS_NO_ERROR;
}


#define	_____MESSAGE_FUNCTIONS_____

// UI 에게 명령을 내리는 Message
static HERROR xmgr_search_ter_SendUpdateDisplayMsg (TerSrchApp_Contents_t *pstContents)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_STATUS_CHANGED, pstContents->hAction, pstContents->stSearchTuningInfo.ter.ulFrequency, 0, 0);
}

// UI 에게 상태를 알려주는 Message
static HERROR xmgr_search_ter_SendSearchStartedMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_STARTED, hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

static HERROR xmgr_search_ter_SendSearchFinished (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_FINISHED, hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

static HERROR xmgr_search_ter_SendSearchSaveFinishedMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_SAVE_FINISHED, hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

static HERROR xmgr_search_ter_SendSearchActionDestroyed (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_ACTION_DESTROYED, hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}


#define	_____PROCESSING_FUNCTIONS_____
static HERROR xmgr_search_ter_ProcessSearchEnd(TerSrchApp_Contents_t *pstContents)
{
	HERROR hErr;

	hErr = xmgr_search_ter_SendSearchFinished(pstContents->hAction);
	xmgr_search_ter_ReleaseAction (pstContents);

	return hErr;
}

static HERROR xmgr_search_ter_IsSignalStrongerThanDbTsInfo (TerSrchApp_Contents_t* pstContents, DbSvc_TsInfo_t *pstNewTsInfo, DbSvc_TsInfo_t *pstDbTsInfo, HBOOL *pbStronger)
{
	DbSvc_TerTuningInfo_t		*pstNewTuneInfo, *pstDbTuneInfo;

	pstNewTuneInfo	= &(pstNewTsInfo->stTuningParam.ter);
	pstDbTuneInfo	= &(pstDbTsInfo->stTuningParam.ter);

	// Quality 가 높으면 TRUE
	if ((HUINT32)pstNewTuneInfo->ucQuality > ((HUINT32)pstDbTuneInfo->ucQuality + TER_SIGNAL_QUALITY_THRESHOLD))
	{
		*pbStronger = TRUE;
		return ERR_OK;
	}

	// Quality 가 낮으면 FALSE
	if (((HUINT32)pstNewTuneInfo->ucQuality + TER_SIGNAL_QUALITY_THRESHOLD) < (HUINT32)pstDbTuneInfo->ucQuality)
	{
		*pbStronger = FALSE;
		return ERR_OK;
	}

	// Quality가 거의 동일
	// Level 이 높으면 TRUE
	if ((HUINT32)pstNewTuneInfo->ucLevel > ((HUINT32)pstDbTuneInfo->ucLevel + TER_SIGNAL_LEVEL_THRESHOLD))
	{
		*pbStronger = TRUE;
		return ERR_OK;
	}

	// 그 외에는 FALSE
	*pbStronger = FALSE;

	return ERR_OK;
}

static HERROR xmgr_search_ter_RetrieveResults (TerSrchApp_Contents_t* pstContents)
{
	HUINT32					 ulIndex, ulItemNum;
	HUINT32					 ulBqNum, ulPrvNum, ulSvcNum;
	HUINT32					 ulTempNetIdx, ulTempTsIdx;
//	HUINT32					 ulPpvInfoNum;
	HINT32					 nDbTsCnt, nDbTsNum;
	Handle_t				 hSvc;
	HBOOL					 bNewTs;
	HBOOL					 bSignalStrong;
	HUINT16					 usNetIdx, usTsIdx, usPrvIdx, usTvChNum, usRadioChNum;
	DbSvc_NetInfo_t			 stNetInfoInDb;
	DbSvc_TsCondition_t		 stTsCond;
	DbSvc_TsInfo_t			*pstTsInfo = NULL;
	DbSvc_TsInfo_t			*pstTsInfoArray = NULL;
	DbSvc_NetInfo_t			*pstNetInfo = NULL;
	DbSvc_Info_t			*astSvcArray = NULL;
	DbSvc_BouqInfo_t		*pstBqArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SRCHSVC_AppendFlag_t	*peSvcFlagArray = NULL;
//	PpvServiceInfo_t		*pstPpvInfoArray = NULL;
	DbSvc_TuningInfo_t		*pstCurrTuneInfo;
	DbSvc_TuningInfo_t		 stSvcTuneInfo;
	DxTuneParam_t			 stChTuneInfo;
	DxDeliveryType_e		 eCurrDeliType;
	CH_SignalInfo_t			 stChSignalInfo;
	HERROR					 hErr;
	SvcSi_ModuleInfo_t		 stModuleInfo;
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

	pstNetInfo->eOrgDeliType = eDxDELIVERY_TYPE_TER;

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
	HxLOG_Print("\teDeliType:0x%X TsIdx:0x%X UniqueId:0x%X NetIdx:0x%X TdId:0x%X OnId:0x%X TunerId:%d\n",
		pstTsInfo->eDeliType,pstTsInfo->usTsIdx,pstTsInfo->usUniqueId,pstTsInfo->usNetIdx,pstTsInfo->usTsId,pstTsInfo->usOnId,pstTsInfo->ucTunerId);

	// SI에서 올려준 TS Info에 실제 물리적 Tuning Info 등을 추가하여 DB에 저장할 수 있는 data로 변환한다.
	hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);

#if defined(CONFIG_MW_CH_DVB_T2)

	HxLOG_Print(" \n");
	HxLOG_Print(" ulFrequency 		: %d\n",stChTuneInfo.ter.ulFrequency);
	HxLOG_Print(" constellation 		: %d\n",stChTuneInfo.ter.eConstellation);
	HxLOG_Print(" hierachy 			: %d\n",stChTuneInfo.ter.eHierachy);
	HxLOG_Print(" codeRate 		: %d\n",stChTuneInfo.ter.eCodeRate);
	HxLOG_Print(" transMode 		: %d\n",stChTuneInfo.ter.eTransMode);
	HxLOG_Print(" guardInterval 		: %d\n",stChTuneInfo.ter.eGuardInterval);
	HxLOG_Print(" offset 			: %d\n",stChTuneInfo.ter.eOffset);
	HxLOG_Print(" bandWidth 		: %d\n",stChTuneInfo.ter.eBandWidth);
	HxLOG_Print(" system 			: %d\n",stChTuneInfo.ter.eSystem);
	HxLOG_Print(" preambleFormat 	: %d\n",stChTuneInfo.ter.stT2Param.preambleFormat);
	HxLOG_Print(" mixed 			: %d\n",stChTuneInfo.ter.stT2Param.mixed);
	HxLOG_Print(" pilotPattern 		: %d\n",stChTuneInfo.ter.stT2Param.pilotPattern);
	HxLOG_Print(" extenedCarrier 	: %d\n",stChTuneInfo.ter.stT2Param.extenedCarrier);
	HxLOG_Print(" PAPRreduction 	: %d\n",stChTuneInfo.ter.stT2Param.PAPRreduction);
	HxLOG_Print(" numPlpBlocks 		: %d\n",stChTuneInfo.ter.stT2Param.numPlpBlocks);
	HxLOG_Print(" ulPlpId 			: %d\n",stChTuneInfo.ter.stT2Param.ulPlpId);
#endif

	if (hErr == ERR_OK)
	{
		MGR_SEARCHUTIL_ConvertChTuningParamToSvcTuningInfo (&stChTuneInfo, &stSvcTuneInfo);
		pstCurrTuneInfo = &stSvcTuneInfo;
	}
	else
	{
		pstCurrTuneInfo = xmgr_search_ter_GetCurrentTuningInfo (pstContents);
	}

	if (pstCurrTuneInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// Tuner ID
	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;

	/*	이태리 Mediaset test항목 중 시청중에 장비 설정을 MFN에서 SFN으로 변경하여도
		AV가 정상적으로 나와야 하므로 Guard Interval과 Code rate를 항상 auto로 하도록 함.
		Code rate의 경우 driver단에서 처리하여 여기서는 Guard interval만 처리하도록 함.
	*/

#if 0// 현재 사용하진 않지만 history 차원에서 관련 코드 남겨둠
	hErr = MW_SI_GetModuleInfo (pstContents->hSiModule, &stModuleInfo);
	if ((ERR_OK == hErr) && (eDxDELIVERY_TYPE_TER == stModuleInfo.stSpecifier.eDeliType && eCountryID_ITA == stModuleInfo.stSpecifier.eCountryId))
	{
		pstCurrTuneInfo->ter.ucGuardInterval = eDxTER_GUARDINTERVAL_AUTO;
	}
#endif

	// Current TP Info
	HxSTD_memcpy (&(pstTsInfo->stTuningParam), pstCurrTuneInfo, sizeof(DbSvc_TuningInfo_t));

	pstTsInfo->eDeliType = eDxDELIVERY_TYPE_TER;


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
		DbSvc_SetDeliType(astSvcArray[ulIndex], eDxDELIVERY_TYPE_TER);
	}

#if 0
	//------------------------------------------------------------------------------
	// Etc. PPV Info
	hErr = MW_SI_GetSearchData (pstContents->hAction, eSRCH_DATA_TYPE_PPVINFO, &ulPpvInfoNum, (void *)&pstPpvInfoArray);
	if (hErr == ERR_OK && pstPpvInfoArray != NULL)
	{
		MW_SI_FreeSearchData (pstContents->hAction, eSRCH_DATA_TYPE_PPVINFO, (void *)pstPpvInfoArray);
	}
#endif

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
		// Network Info가 이미 등록되어 있는 것인지 체크한다.
		hErr = DB_SVC_FindNetworkIndexById(pstNetInfo->eOrgDeliType, pstNetInfo->usNetId, &usNetIdx);
		if (hErr == ERR_OK)
		{
			if (pstNetInfo->szNetName[0] != '\0')
			{
				HBOOL	bCompDefAuth, bIsSameDefAuth;
				HUINT8	*aucDefAuthNew = NULL, *aucDefAuth = NULL;
				HUINT32	ulFTAContentMgr = 0,ulFTAContentMgrNew = 0;

				DB_SVC_GetNetworkInfo (usNetIdx, &stNetInfoInDb);

				(void)DB_SVC_GetNetworkDefaultAuthority((const DbSvc_NetInfo_t *)pstNetInfo, &aucDefAuthNew);
				(void)DB_SVC_GetNetworkDefaultAuthority((const DbSvc_NetInfo_t *)&stNetInfoInDb, &aucDefAuth);

				bIsSameDefAuth = TRUE;
				bCompDefAuth = ((NULL != aucDefAuthNew) && (NULL != aucDefAuth)) ? TRUE : FALSE;
				if (bCompDefAuth)
				{
					bCompDefAuth = ((aucDefAuthNew[0] =='\0' ) || (aucDefAuth[0] =='\0')) ? FALSE : TRUE;
					if(bCompDefAuth)
					{
						bIsSameDefAuth = (0 != MWC_UTIL_DvbStrcmp (aucDefAuthNew, aucDefAuth)) ? FALSE : TRUE;
					}
				}

				(void)DB_SVC_GetNetworkFTAContentMgr((const DbSvc_NetInfo_t *)pstNetInfo, &ulFTAContentMgrNew);
				(void)DB_SVC_GetNetworkFTAContentMgr((const DbSvc_NetInfo_t *)&stNetInfoInDb, &ulFTAContentMgr);

				if ((MWC_UTIL_DvbStrcmp (pstNetInfo->szNetName, stNetInfoInDb.szNetName) != 0) ||
					(TRUE == bCompDefAuth && FALSE == bIsSameDefAuth) ||
					(ulFTAContentMgrNew != ulFTAContentMgr))
				{
					// Network Name이 달라졌으면 Update 한다.
					DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfo);
					HxLOG_Print("\t= [NET] tempIdx = %d, netIdx = %d --> update\n", ulTempNetIdx, usNetIdx);
				}
			}
		}
		else
		{
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

	usTvChNum = usRadioChNum = 0;
	bNewTs = TRUE;
	//------------------------------------------------------------------------------
	// 2. adding/updating a ts
	//------------------------------------------------------------------------------
	if (pstTsInfo != NULL)
	{
		pstTsInfo->usNetIdx = usNetIdx;

		// Signal의 Quality를 기록해야 한다.
		eCurrDeliType = eDxDELIVERY_TYPE_TER;
		hErr = SVC_CH_GetSignalInfo (pstContents->ulActionId, &eCurrDeliType, &stChSignalInfo);
		if (hErr != ERR_OK)
		{
			stChSignalInfo.signalLevel = 0;
			stChSignalInfo.signalQuality = 0;
		}

		HxLOG_Print("[SVC_SEARCH] SIGNAL Strength (%d:%d)\n", stChSignalInfo.signalLevel, stChSignalInfo.signalQuality);
		pstTsInfo->stTuningParam.ter.ucLevel = (HUINT8)stChSignalInfo.signalLevel;
		pstTsInfo->stTuningParam.ter.ucQuality = (HUINT8)stChSignalInfo.signalQuality;

		// TS Info가 OnId, TsId로 등록되어 있는 지 확인한다.
		DB_SVC_InitTsFindCondition (&stTsCond);
		stTsCond.nDeliType	= pstTsInfo->eDeliType;
		stTsCond.nOnId		= (HINT32)pstTsInfo->usOnId;
		stTsCond.nTsId		= (HINT32)pstTsInfo->usTsId;

		if (MGR_SEARCH_IsTsInfoAppendableForSameOnTsIdAndDiffTuneInfo (pstContents->ulActionId, eDxDELIVERY_TYPE_TER) == TRUE)
		{
			// 특정 향에서 주파수가 다른 실제 동일 서비스라도 계속 추가해야 할 경우
			bNewTs = TRUE;
			nDbTsNum = 0;
			pstTsInfoArray = NULL;
			usTsIdx = 0;

			hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoArray);
			if (hErr == ERR_OK && nDbTsNum > 0 && pstTsInfoArray != NULL)
			{
				for (nDbTsCnt = 0; nDbTsCnt < nDbTsNum; nDbTsCnt++)
				{
					if (DB_SVC_UtilCompareDbTuningParam(pstTsInfoArray[nDbTsCnt].eDeliType, &(pstTsInfo->stTuningParam), &(pstTsInfoArray[nDbTsCnt].stTuningParam)) == 0)
					{
						HBOOL	 bIsSameDefAuth = FALSE;
						HUINT8	*aucDefAuth, *aucDefAuthNew;

						bNewTs = FALSE;
						usTsIdx = pstTsInfoArray[nDbTsCnt].usUniqueId;

						aucDefAuth = aucDefAuthNew = NULL;
						(void)DB_SVC_GetTsDefaultAuthority((const DbSvc_TsInfo_t *)pstTsInfo, &aucDefAuthNew);
						(void)DB_SVC_GetTsDefaultAuthority((const DbSvc_TsInfo_t *)&pstTsInfoArray[nDbTsCnt], &aucDefAuth);

						bIsSameDefAuth = FALSE;
						if (NULL != aucDefAuth && NULL != aucDefAuthNew)
						{
							bIsSameDefAuth = (0 != MWC_UTIL_DvbStrcmp (aucDefAuthNew, aucDefAuth)) ? FALSE : TRUE;
						}

						if (TRUE == bIsSameDefAuth)
						{
							DB_SVC_UpdateTsInfo (usTsIdx, pstTsInfo);
						}
					}
				}
			}

			if (bNewTs == TRUE)
			{
				DB_SVC_AddTsInfo (pstTsInfo, &usTsIdx);
			}

		}
		else
		{
			nDbTsNum = 0;
			pstTsInfoArray = NULL;

			hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoArray);
			if (hErr == ERR_OK && nDbTsNum > 0 && pstTsInfoArray != NULL)
			{
				HxLOG_Print ("\n\n\t ==============> Find Same Ts (OnId:0x%X, TsId:0x%X)\n", stTsCond.nOnId, stTsCond.nTsId);
				usTsIdx = pstTsInfoArray[0].usUniqueId;
				bNewTs = FALSE;

				if (DB_SVC_UtilCompareDbTuningParam(pstTsInfoArray[0].eDeliType, &(pstTsInfo->stTuningParam), &(pstTsInfoArray[0].stTuningParam)) != 0)
				{
					HxLOG_Print("\t ==============>But different Tunning Param\n");
#if defined(CONFIG_MW_CH_DVB_T2)
					if(pstTsInfo->stTuningParam.ter.ucSystem && pstTsInfo->stTuningParam.ter.stT2param.ulPlpId != 0x0000FFFF && pstTsInfo->stTuningParam.ter.stT2param.ulPlpId != pstTsInfoArray[0].stTuningParam.ter.stT2param.ulPlpId)
					{
						DB_SVC_AddTsInfo (pstTsInfo, &usTsIdx);
						HxLOG_Print("\tAdd New TP (TsIdx:0x%X)\n",usTsIdx);
						bNewTs = TRUE;
					}
					else
#endif
					{
						// Same On/Ts Id, but the different Tuning Info :
						// FLD 개념에 의거, Signal이 훨씬 세면 Update, 그렇지 않으면 이 TP 자체를 무시한다.
						// 이사 갔는데, 다시 서치하는 경우, 기존 채널이 있으면 서치가 안 될 수도 있음
#if !defined (CONFIG_PROD_DIGIMAXT2)
						bSignalStrong = TRUE;
						xmgr_search_ter_IsSignalStrongerThanDbTsInfo (pstContents, pstTsInfo, &(pstTsInfoArray[0]), &bSignalStrong);
						if (bSignalStrong == TRUE)
						{
							HxLOG_Print("\t ==============> DB_SVC_UpdateTsInfo(TsIdx:0x%X)\n",usTsIdx);
							DB_SVC_UpdateTsInfo (usTsIdx, pstTsInfo);
							HxLOG_Print("\t\t= [TS] tempIdx = %d, tsIdx = %d --> update\n", ulTempTsIdx, usTsIdx);
						}
						else
						{
							HxLOG_Critical("\n\n");
							DB_SVC_FreeTsInfoList (pstTsInfoArray);
							hErr = ERR_FAIL;
							goto ERROR_FAULT_OCCURRED;
						}
#else
						DbSvc_Condition_t	stSvcCond;
						HINT32				nDbSvcNum;
						Handle_t			*phSvcList;

						DB_SVC_InitSvcFindCondition(&stSvcCond);

						stSvcCond.nTsIdx		= (HINT32)usTsIdx;
						stSvcCond.nOnId 		= (HINT32)pstTsInfoArray[0].usOnId;
						stSvcCond.nTsId 		= (HINT32)pstTsInfoArray[0].usTsId;

						hErr = DB_SVC_FindSvcHandleList(&stSvcCond, &nDbSvcNum, &phSvcList);
						if (ulSvcNum == nDbSvcNum)
						{
							bSignalStrong = TRUE;
							xmgr_search_ter_IsSignalStrongerThanDbTsInfo (pstContents, pstTsInfo, &(pstTsInfoArray[0]), &bSignalStrong);
							if (bSignalStrong == TRUE)
							{
								HxLOG_Print("\t ==============> DB_SVC_UpdateTsInfo(TsIdx:0x%X)\n",usTsIdx);
								DB_SVC_UpdateTsInfo (usTsIdx, pstTsInfo);
								HxLOG_Print("\t\t= [TS] tempIdx = %d, tsIdx = %d --> update\n", ulTempTsIdx, usTsIdx);
							}
							else
							{
								HxLOG_Critical("\n\n");
								DB_SVC_FreeTsInfoList (pstTsInfoArray);
								hErr = ERR_FAIL;
								goto ERROR_FAULT_OCCURRED;
							}
						}
						else
						{
							if (ulSvcNum > nDbSvcNum)
							{
								HxLOG_Print("\t ==============> DB_SVC_UpdateTsInfo(TsIdx:0x%X)\n",usTsIdx);
								DB_SVC_UpdateTsInfo (usTsIdx, pstTsInfo);
								HxLOG_Print("\t\t= [TS] tempIdx = %d, tsIdx = %d --> update\n", ulTempTsIdx, usTsIdx);
							}
							else
							{
								HxLOG_Critical("\n\n");
								DB_SVC_FreeTsInfoList (pstTsInfoArray);
								hErr = ERR_FAIL;
								goto ERROR_FAULT_OCCURRED;
							}
						}
#endif
					}
				}
			}
			else
			{
				DB_SVC_AddTsInfo (pstTsInfo, &usTsIdx);
				HxLOG_Print("\tDB_SVC_AddTsInfo(TsIdx:0x%X)\n",usTsIdx);
				HxLOG_Print("\t\t= [TS] tempIdx = %d, tsIdx = %d --> add\n", ulTempTsIdx, usTsIdx);
			}
			if (pstTsInfoArray != NULL)
			{
				DB_SVC_FreeTsInfoList (pstTsInfoArray);
				pstTsInfoArray = NULL;
			}

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
				HxLOG_Print("\t\t= [PRV] prvIdx = %d --> add\n", usPrvIdx);
			}
			pstPrvArray[ulIndex].usPrvIdx = usPrvIdx;
		}
	}

	// 6-5. Service Info
	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		if (MGR_SEARCHUTIL_CompareSearchedSvcType (pstContents->eServiceType, DbSvc_GetSvcType(&astSvcArray[ulIndex])) == TRUE)
		{
			HUINT16		usPrvIdx;

			DbSvc_SetOnId(&astSvcArray[ulIndex], pstTsInfo->usOnId);
			DbSvc_SetTsId(&astSvcArray[ulIndex], pstTsInfo->usTsId);
			DbSvc_SetTsIdx(&astSvcArray[ulIndex], ulTempTsIdx);
			usPrvIdx = DbSvc_GetPrvIdx(&astSvcArray[ulIndex]);

			if ((pstPrvArray != NULL) && (usPrvIdx < ulPrvNum))
			{
				DbSvc_SetPrvIdx(&astSvcArray[ulIndex], pstPrvArray[usPrvIdx].usPrvIdx);
			}
			else
			{
				DbSvc_SetPrvIdx(&astSvcArray[ulIndex], 0);
			}

			DbSvc_SetTsIdx(&astSvcArray[ulIndex], usTsIdx);
		}

		if (eDxDELIVERY_TYPE_TER == stModuleInfo.stSpecifier.eDeliType && eCountryID_GBR == stModuleInfo.stSpecifier.eCountryId)
		{
			MWC_UTIL_EliminateShyAndControlCharacter(DbSvc_GetName(&astSvcArray[ulIndex]), MWC_UTIL_DvbStrlen(DbSvc_GetName(&astSvcArray[ulIndex])));
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
			// Add the service lists in the GUI :
			if (DbSvc_GetSvcRemoved(&astSvcArray[ulIndex]) == FALSE)
			{
				DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));
				MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &(astSvcArray[ulIndex]));
			}
			break;

		case eSRCHSVC_UPDATE:
			HxLOG_Print(">>> UPDATE : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			hErr = DB_SVC_GetServiceHandle (eDxSvcSection_All, DbSvc_GetTsIdx(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), &hSvc);
			if (hErr == ERR_OK)
			{
				DB_SVC_UpdateServiceInfo (hSvc, &astSvcArray[ulIndex]);

				// Add the service lists in the GUI :
				if (DbSvc_GetSvcRemoved(&astSvcArray[ulIndex]) == FALSE)
				{
					DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));
					MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &(astSvcArray[ulIndex]));
				}
			}
			break;

		case eSRCHSVC_SKIP:
			HxLOG_Print(">>> SKIP : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			break;

		default:
			HxLOG_Print(">>> etc : %04d, [%s]\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			// 어찌 되었건 UI 리스트에 보여야 한다.
			hErr = DB_SVC_GetServiceHandle (eDxSvcSection_All, DbSvc_GetTsIdx(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), &hSvc);
			if (hErr == ERR_OK)
			{
				// Add the service lists in the GUI :
				if (DbSvc_GetSvcRemoved(&astSvcArray[ulIndex]) == FALSE)
				{
					DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));
					MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &(astSvcArray[ulIndex]));
				}
			}

			break;
		}
	}

	(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, pstTsInfo->usOnId, pstTsInfo->usTsId);

	//  list변경됐음을 알림
	xmgr_search_ter_SendUpdateDisplayMsg(pstContents);

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

static HERROR xmgr_search_ter_AddDeliveryInfoInTpList (TerSrchApp_Contents_t *pstContents)
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
																xmgr_search_ter_FilterSameTuningInfo,
																(void *)&(pstDeliTsInfo->stTuningParam));
		if (pstFound != NULL)					{ continue; }

		// 2. 현재 TP 혹은 이미 Delivery TP List에 들어간 TP와 동일한 TP 인가 ?
		pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstDeliTpList, 0,
																xmgr_search_ter_FilterSameTuningInfo,
																(void *)&(pstDeliTsInfo->stTuningParam));
		if (pstFound != NULL)					{ continue; }

		// 3. 이 TP가 앞으로의 Next TP에 존재하는가 ?
		// 이 경우는 시나리오에 따라 달라질 수 있다.
		// 여기서는 같은 network 인 TP들을 한 곳에 모아서 처리하기 위해
		// 뒤에 있는 Next TPs에 있는 item 쪽을 지우고 DeliTpList에 추가하는 방향으로 간다.
		pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstNextTpList, 0,
																xmgr_search_ter_FilterSameTuningInfo,
																(void *)&(pstDeliTsInfo->stTuningParam));
		while (pstFound != NULL)
		{
			pstNextTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstNextTpList, pstFound, xmgr_search_ter_FreeTuneInfo);
			UTIL_LINKEDLIST_CircularizePointerList (pstNextTpList);

			pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstNextTpList, 0,
																	xmgr_search_ter_FilterSameTuningInfo,
																	(void *)&(pstDeliTsInfo->stTuningParam));
		}

		// 4. Delivery TP를 추가
		pstDeliSvcTuneInfo = xmgr_search_ter_AllocTuneInfo();
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
	UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstRest, xmgr_search_ter_FreeTuneInfo);

	OxAP_Free (pstDeliTsArray);

	return ERR_BUS_NO_ERROR;
}

static HERROR xmgr_search_ter_AddLpTuningInfoInTpList (TerSrchApp_Contents_t *pstContents)
{
	DxTuneParam_t		 stChTuneInfo;
	DbSvc_TuningInfo_t	*pstSvcTuneInfo, *pstLpSvcTuneInfo;
	POINTER_LIST_T		*pstNextTpList, *pstCurrTpList;
	POINTER_LIST_T		*pstRest;
	HERROR				 hErr;

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

	// 현재 TP (처음 item)을 pstCurrTpList 의 맨 처음에 넣고,  그 후에 NextTpList에서는 뺀다.
	// 이렇게 하는 이유는 나중에 CurrTp - LP CurrTp - Next Tps  순으로 합치기 위해서이다.

	pstSvcTuneInfo = (DbSvc_TuningInfo_t *)pstContents->pstTpToSearch->pvContents;
	pstNextTpList = pstContents->pstTpToSearch;
	pstCurrTpList = NULL;

	pstNextTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstNextTpList, pstNextTpList, NULL);
	UTIL_LINKEDLIST_CircularizePointerList (pstNextTpList);

	pstCurrTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstCurrTpList, (void *)pstSvcTuneInfo);
	UTIL_LINKEDLIST_CircularizePointerList (pstCurrTpList);

	if (pstSvcTuneInfo->ter.ucStream == eDxTER_STREAM_HP)
	{
		hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
		if (hErr == ERR_OK)
		{
			// Hierarchy가 None 이 아니면 HP 외에 LP로도 TP가 있을 수 있다.
			// TER_HIERACH_AUTO 인 경우도 제외 --> 임시, 향후 확인 필요.
			if (stChTuneInfo.ter.eHierachy != eDxTER_HIERACHY_NONE && stChTuneInfo.ter.eHierachy != eDxTER_HIERACHY_AUTO)
			{
				pstLpSvcTuneInfo = xmgr_search_ter_AllocTuneInfo();
				if (pstLpSvcTuneInfo != NULL)
				{
					// 현재 Tuning Info와 동일하지만 Stream만 LP인 새로운 Tuning Info를 추가한다.
					HxSTD_memcpy (pstLpSvcTuneInfo, pstSvcTuneInfo, sizeof(DbSvc_TuningInfo_t));

					pstLpSvcTuneInfo->ter.ucStream = eDxTER_STREAM_LP;
					pstCurrTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstCurrTpList, (void *)pstLpSvcTuneInfo);
					UTIL_LINKEDLIST_CircularizePointerList (pstCurrTpList);
				}
			}
		}
	}

	// Delivery Tp List와 남은 Next Tp List를 결합
	pstRest = NULL;
	pstContents->pstTpToSearch = UTIL_LINKEDLIST_CombineTwoPointerLists (pstCurrTpList, pstNextTpList, NULL, NULL, &pstRest);
	UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstTpToSearch);
	UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstRest, xmgr_search_ter_FreeTuneInfo);

	return ERR_BUS_NO_ERROR;
}

#if defined(CONFIG_MW_CH_DVB_T2)
static HERROR xmgr_search_ter_AddMPLPTuningInfoInTpList (TerSrchApp_Contents_t *pstContents, HUINT8 *ucPlpIds, HUINT8 ucNumPlps )
{
	DbSvc_TuningInfo_t	*pstSvcTuneInfo, *pstMPLPSvcTuneInfo;
	POINTER_LIST_T		*pstNextTpList, *pstCurrTpList;
	POINTER_LIST_T		*pstRest;
	HUINT8				cntPLP=0;

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

	pstSvcTuneInfo = (DbSvc_TuningInfo_t *)pstContents->pstTpToSearch->pvContents;
	pstNextTpList = pstContents->pstTpToSearch;
	pstCurrTpList = NULL;

	pstNextTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstNextTpList, pstNextTpList, NULL);
	UTIL_LINKEDLIST_CircularizePointerList (pstNextTpList);

	HxLOG_Print(" \n");
	while(ucNumPlps > cntPLP)
	{
		pstMPLPSvcTuneInfo = xmgr_search_ter_AllocTuneInfo();
		if (pstMPLPSvcTuneInfo != NULL)
		{
			// 현재 Tuning Info와 동일하지만 Stream만 LP인 새로운 Tuning Info를 추가한다.
			HxSTD_memcpy (pstMPLPSvcTuneInfo, pstSvcTuneInfo, sizeof(DbSvc_TuningInfo_t));

			pstMPLPSvcTuneInfo->ter.stT2param.ulPlpId = ucPlpIds[cntPLP];
			pstMPLPSvcTuneInfo->ter.stT2param.ulNumPlpBlocks= ucNumPlps;
			HxLOG_Print("[ADD PLP] => (%d) PLPId [%d] \n",cntPLP,pstMPLPSvcTuneInfo->ter.stT2param.ulPlpId );

			pstCurrTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstCurrTpList, (void *)pstMPLPSvcTuneInfo);
			UTIL_LINKEDLIST_CircularizePointerList (pstCurrTpList);
		}
		cntPLP++;
	}

	// Delivery Tp List와 남은 Next Tp List를 결합
	pstRest = NULL;
	pstContents->pstTpToSearch = UTIL_LINKEDLIST_CombineTwoPointerLists (pstCurrTpList, pstNextTpList, NULL, NULL, &pstRest);
	UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstTpToSearch);
	UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstRest, xmgr_search_ter_FreeTuneInfo);

	return ERR_BUS_NO_ERROR;
}

#endif

static HERROR xmgr_search_ter_SaveResult (Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	DefaultLcnPolicy_t		 eLcnPolicy;
	HUINT16					*pusNetIdxArray = NULL;
	HUINT16					*pusPrvIdxArray = NULL;
	HUINT16					*pusTsIdxArray  = NULL;
	HUINT16					*pusIsOldTs		= NULL;
	TerSrchApp_Contents_t	*pstContents;
	HERROR					 hErr;

__save_time_trace_reset__;
__search_time_trace_start__;

	(void)hAction;
	(void)ulParam1;
	(void)ulParam2;
	(void)ulParam3;

	pstContents = (TerSrchApp_Contents_t *)ulParam1;
	if (pstContents == NULL)
	{
		return ERR_FAIL;
	}

	hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);

	HxLOG_Print("(+++++ DB_SVC_RemoveSearchFlag +++++)\n");
	DB_SVC_RemoveSearchFlag();
	HxLOG_Print("(----- DB_SVC_RemoveSearchFlag -----)\n");
__save_time_trace_check__;


	/* LCN 스펙 중, non-LCN에 대한 처리가 없으면, non-LCN 서비스는 LCN 마지막에 덧붙여 할당한다 (Humax Rule) */
	MGR_SEARCH_GetLcnPolicy (&eLcnPolicy, NULL);
	SVC_SI_PostProcessForSvcNumbering (pstContents->hAction, eLcnPolicy, eDxDELIVERY_TYPE_TER);

__save_time_trace_check__;
// sqlitemem;

	hErr = ERR_BUS_NO_ERROR;
	pstContents->bSvcSaved = TRUE;

	if (pusNetIdxArray != NULL)
	{
		OxAP_Free (pusNetIdxArray);
	}

	if (pusPrvIdxArray != NULL)
	{
		OxAP_Free (pusPrvIdxArray);
	}

	if (pusTsIdxArray != NULL)
	{
		OxAP_Free (pusTsIdxArray);
	}

	if (pusIsOldTs != NULL)
	{
		OxAP_Free (pusIsOldTs);
	}

__search_time_trace_check__;
	return hErr;
}

static HBOOL xmgr_search_ter_FilterSameTuningInfo (void *pvItem, void *pvFilter)
{
	DbSvc_TuningInfo_t *pstItem, *pstFilter;

	pstItem = (DbSvc_TuningInfo_t *)pvItem;
	pstFilter = (DbSvc_TuningInfo_t *)pvFilter;

	if (DB_SVC_UtilCompareDbTuningParam(eDxDELIVERY_TYPE_TER, pstItem, pstFilter) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

static DbSvc_TuningInfo_t *xmgr_search_ter_AllocTuneInfo (void)
{
	return (DbSvc_TuningInfo_t *)OxAP_Malloc (sizeof(DbSvc_TuningInfo_t));
}

static void xmgr_search_ter_FreeTuneInfo (void *pvTuneInfo)
{
	if (pvTuneInfo)
	{
		OxAP_Free ((DbSvc_TuningInfo_t *)pvTuneInfo);
	}
}

static void xmgr_search_ter_ReleaseAction (TerSrchApp_Contents_t *pstContents)
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


#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____

//eMEVT_BUS_CREATE
static HERROR xmgr_search_ter_MsgGwmCreate (TerSrchApp_Contents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrSearch_Option_t	*pstOption = (MgrSearch_Option_t *)p1;

	HxLOG_Info("\t[xmgr_search_ter_MsgGwmCreate] hAction (%08x)\n", hAction);

	if (NULL == pstOption)
	{
		BUS_MGR_Destroy (NULL);
		return ERR_BUS_NO_ERROR;
	}

	// 1. Initialize the contents
	xmgr_search_ter_InitContents (pstContents, hAction, pstOption);

	/* automatic search dama db 삭제후 obama db가 삭제되는 callback이 등록되어져 있어야 하나
	     search중에 해당 callback을 제거하서 삭제안되어서 막음. ResetTable 동작과 Search동작 async해서 생기는 문제임. by MOON
	DB_SVC_SetNotifier(FALSE); */

	// 2. Initialize Search Result
	MGR_SEARCH_InitResult(pstContents->ulActionId);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY:
static HERROR xmgr_search_ter_MsgGwmDestroyed (TerSrchApp_Contents_t *pstContents)
{
	if (pstContents->eAppState == eSearchApp_state_waitingSi)
	{
		// Stop SI & Cancel the search :
		SVC_SI_StopSearch(pstContents->hAction, TRUE);
	}

	if (pstContents->eAppState != eSearchApp_state_searchend)
	{
		// Search가 끝나지 않았다면 Action Release
		xmgr_search_ter_ReleaseAction (pstContents);
	}

#if 0 //skk 확인후 제거
	if (pstContents->bSvcSaved == FALSE)
	{
		DB_SVC_DeleteAllSearchingService();
	}
#endif

	// Destroy the contents
	xmgr_search_ter_DestroyContents (pstContents);

	// Send the destoyed message to the UI procedure
	xmgr_search_ter_SendSearchActionDestroyed(pstContents->hAction);

	return MESSAGE_BREAK;
}

static HERROR xmgr_search_ter_MsgAppSearchActionStart (TerSrchApp_Contents_t *pstContents, HINT32 nAppHandle)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.
	HERROR			 hErr;

	// 1. Set the TP to tuning :
	xmgr_search_ter_SetTuningParamToSearch(pstContents);

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
	hErr = xmgr_search_ter_TuneSearchTp (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("[xmgr_search_ter_MsgGwmCreate] No Next Tp left to tune\n");
		pstContents->eAppState = eSearchApp_state_searchend;
	}

	xmgr_search_ter_SendSearchStartedMsg (pstContents->hAction);

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_ter_ProcessSearchEnd(pstContents);
	}

	return ERR_OK;

}

// eMEVT_SEARCH_SAVE_RESULT
static HERROR xmgr_search_ter_MsgAppSearchSaveResult (TerSrchApp_Contents_t *pstContents, HINT32 nAppHandle)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		MGR_ACTION_BgJobExecuteFunction (xmgr_search_ter_SaveResult, pstContents->hAction, (HUINT32)pstContents, 0, 0);
	}
	else
	{
		xmgr_search_ter_SendSearchSaveFinishedMsg(pstContents->hAction);
	}

	return ERR_OK;
}

// eMEVT_SEARCH_CANCEL
static HERROR xmgr_search_ter_MsgAppSearchCancel (TerSrchApp_Contents_t *pstContents, HINT32 nAppHandle)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.

	if (pstContents->eAppState == eSearchApp_state_waitingSi)
	{
		// Stop SI & Cancel the search :
		SVC_SI_StopSearch(pstContents->hAction, TRUE);
	}

	pstContents->eAppState = eSearchApp_state_searchend;
	xmgr_search_ter_SendUpdateDisplayMsg(pstContents);
	xmgr_search_ter_ReleaseAction (pstContents);

	return ERR_OK;
}

// eMEVT_ACTION_NOTIFY_BGJOB_FINISHED:
static HERROR xmgr_search_ter_MsgAppBgJobFinished (TerSrchApp_Contents_t *pstContents, HINT32 nCbPtr, HINT32 nResult)
{
	(void)pstContents;
	(void)nCbPtr;
	(void)nResult;

	if (nCbPtr == (HINT32)xmgr_search_ter_SaveResult)
	{
		xmgr_search_ter_SendSearchSaveFinishedMsg(pstContents->hAction);
		return MESSAGE_BREAK;
	}

	return ERR_OK;
}

// MSG_APP_SEARCH_KILL_PROCESSES
static HERROR xmgr_search_ter_MsgAppSearchKillProcesses (TerSrchApp_Contents_t *pstContents)
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
		xmgr_search_ter_ProcessSearchEnd(pstContents);
	}

	BUS_MGR_Destroy (NULL);
	return ERR_OK;
}

// eMEVT_BUS_TIMER:
static HERROR xmgr_search_ter_MsgGwmTimer (TerSrchApp_Contents_t *pstContents, HINT32 nTimerId)
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
			hErr = xmgr_search_ter_SetNextTpToSearch (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_search_ter_TuneSearchTp (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_search_ter_MsgGwmTimer] No Next Tp left to tune\n");
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
				hErr = xmgr_search_ter_SetNextTpToSearch (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_search_ter_TuneSearchTp (pstContents);
				}
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_search_ter_MsgGwmTimer] Cannot proceed anymore\n");
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
		xmgr_search_ter_ChangeProgressNumber (pstContents);
		xmgr_search_ter_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_ter_ProcessSearchEnd(pstContents);
	}

	return hRetValue;
}

// eSEVT_SI_SEARCH_DONE:
static HERROR xmgr_search_ter_EvtSiSearchDone (TerSrchApp_Contents_t *pstContents)
{
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 hErr;

	ePrevAppState = pstContents->eAppState;

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_waitingSi:
		// SI succeeded : Kill the timer

		BUS_KillTimer (SEARCH_TIMER_SI_ID);

		pstContents->eAppState = eSearchApp_state_siReceived;

		hErr = xmgr_search_ter_RetrieveResults (pstContents);
		if (hErr == ERR_OK && pstContents->bNetworkSearch == TRUE)
		{
			xmgr_search_ter_AddDeliveryInfoInTpList (pstContents);
		}

		SVC_SI_StopSearch(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_search_ter_SetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_search_ter_TuneSearchTp (pstContents);
		}

		// Tuning Failure :
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_search_ter_EvtSiSearchDone] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	default:
		break;
	}

	if (ePrevAppState != pstContents->eAppState)
	{
		// set UI button text
		xmgr_search_ter_ChangeProgressNumber (pstContents);
		xmgr_search_ter_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_ter_ProcessSearchEnd(pstContents);
	}

	return MESSAGE_BREAK;
}

// eSEVT_SI_SEARCH_FAIL:
static HERROR xmgr_search_ter_EvtSiSearchFail (TerSrchApp_Contents_t *pstContents)
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
		hErr = xmgr_search_ter_SetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_search_ter_TuneSearchTp (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_search_ter_EvtSiSearchFail] No Next Tp left to tune\n");
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
		xmgr_search_ter_ChangeProgressNumber (pstContents);
		xmgr_search_ter_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_ter_ProcessSearchEnd(pstContents);
	}

	return MESSAGE_BREAK;
}

// eSEVT_CH_LOCKED
static HERROR xmgr_search_ter_EvtChLocked (TerSrchApp_Contents_t *pstContents)
{
	HUINT32				 ulNetId;
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 hErr;
#if defined(CONFIG_MW_CH_DVB_T2)
	HUINT8				*pucMPLPIds;
	HUINT8				ucNumPlps ;
	DbSvc_TuningInfo_t		*pstSvcTuneInfo;
#endif
	ePrevAppState = pstContents->eAppState;

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_tryTuning :
		// Channel Locked : Kill the timer

		// BUS_KillTimer (SEARCH_TIMER_LOCKING_ID);

		// 지상파의 경우, 현재 Tuning된 정보를 다시 읽어 와서 Frequency가 동일하고 stream이 LP인 TP가 더 있는 지 확인해야 한다.
#if !defined(WIN32)			// Emulator 에서는 HP/LP 구분이 불가능하다.
		xmgr_search_ter_AddLpTuningInfoInTpList (pstContents);
#endif
#if defined(CONFIG_MW_CH_DVB_T2)
		pstSvcTuneInfo = (DbSvc_TuningInfo_t *)pstContents->pstTpToSearch->pvContents;
		if(pstSvcTuneInfo->ter.ucSystem && !pstSvcTuneInfo->ter.stT2param.ulNumPlpBlocks)
		{
			hErr = SVC_CH_GetPLPCount(pstContents->ulActionId, &ucNumPlps );
			pucMPLPIds = (HUINT8 *)OxAP_Malloc(sizeof(HUINT8)*ucNumPlps);
			if (pucMPLPIds == NULL)
			{
				HxLOG_Critical("\n\n");
				return ERR_BUS_OUT_OF_MEMORY;
			}

			hErr = SVC_CH_GetDataPLPInfo (pstContents->ulActionId, pucMPLPIds, &ucNumPlps );
			if(hErr == ERR_OK)
			{
				xmgr_search_ter_AddMPLPTuningInfoInTpList (pstContents,pucMPLPIds, ucNumPlps );
			}

			hErr = xmgr_search_ter_TuneSearchTp (pstContents);

			break;
		}
#endif
		if (pstContents->bNetworkSearch == TRUE)
		{
			if (pstContents->ulNetworkId > 0 && pstContents->ulNetworkId < 0xFFFF)
			{
				ulNetId = (pstContents->ulNetworkId & 0xFFFF);
			}
			else
			{
				ulNetId = SvcSi_NETID_SEARCH_NITACT;
			}
		}
		else
		{
			ulNetId = SvcSi_NETID_SEARCH_NOTNIT;
		}

		{
			SvcSi_SearchSpec_t		stSrchSpec;
			SvcSi_SearchOption_t	stSrchOption;

			HxSTD_memset(&stSrchOption, 0x00, sizeof(SvcSi_SearchOption_t));
			stSrchOption.ulSearchType	= eSVCSI_SEARCHTYPE_Normal;	// 아래에선 0으로 SET했는데 설마 NONE을 의도한 건 아닐거다.. normal로 설정
			stSrchOption.ulSvcType		= (HUINT32)pstContents->eServiceType;
			stSrchOption.ulCasType		= (HUINT32)pstContents->eCAFreeType;
			stSrchOption.hSession		= (SvcSi_SessionH_t)HANDLE_NULL;	// not use
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
			    hErr = xmgr_search_ter_SetNextTpToSearch (pstContents);
			    if (hErr == ERR_OK)
			    {
				    hErr = xmgr_search_ter_TuneSearchTp (pstContents);
			    }

			    if (hErr != ERR_OK)
			    {
				    HxLOG_Print("[xmgr_search_ter_EvtChLocked] No Next Tp left to tune\n");
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
		xmgr_search_ter_ChangeProgressNumber (pstContents);
		xmgr_search_ter_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_ter_ProcessSearchEnd(pstContents);
	}

	return MESSAGE_BREAK;
}

static HERROR xmgr_search_ter_EvtChNoSignal (TerSrchApp_Contents_t *pstContents)
{
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 hErr;

	ePrevAppState = pstContents->eAppState;
	/* lock fail시에는 정보가 너무 빨리 갱신되어 TP정보를 출력하지 못하는 문제가 있어
	    아래와 같이 무조건 현재 Lock시도한 TP정보를 출력하도록 함.*/
	xmgr_search_ter_SendUpdateDisplayMsg(pstContents);

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_tryTuning :
		// Channel Locked : Kill the timer

		// BUS_KillTimer (SEARCH_TIMER_LOCKING_ID);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_search_ter_SetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_search_ter_TuneSearchTp (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_search_ter_EvtChNoSignal] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	case eSearchApp_state_waitingSi:
		// Stop SI :
		hErr = SVC_SI_StopSearch(pstContents->hAction, TRUE);
		if (hErr == ERR_OK)
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_search_ter_SetNextTpToSearch (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_search_ter_TuneSearchTp (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_search_ter_EvtChNoSignal] No Next Tp left to tune\n");
				pstContents->eAppState = eSearchApp_state_searchend;
			}
		}
		else
		{
			// 더 이상 진행 불가능 : Search End로 간주한다.
			HxLOG_Print("[xmgr_search_ter_EvtChNoSignal] SI Re-open failed.\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	default:
		break;
	}

	if (ePrevAppState != pstContents->eAppState)
	{
		// set UI button text
		xmgr_search_ter_ChangeProgressNumber (pstContents);
		xmgr_search_ter_SendUpdateDisplayMsg(pstContents);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_ter_ProcessSearchEnd(pstContents);
	}

	return MESSAGE_BREAK;
}


#define	_____EXTERNAL_PROCESS_FUNCTION_____



BUS_Result_t		xproc_search_TerAction0_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return xproc_search_ter_Proto(&(s_TerSrchApp_Contents[0]), message, hAction, p1, p2, p3);
}

BUS_Result_t		xproc_search_TerAction1_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return xproc_search_ter_Proto(&(s_TerSrchApp_Contents[1]), message, hAction, p1, p2, p3);
}

/**
  @brief      Search service application main 함수 .

  Function Name: AP_TerSrchSvc_Proc()
  Function Description: application proc 함수
  Parameter: proc parameters
  type: (Sync or Async) async 함수
  Return Value:
*/
STATIC BUS_Result_t xproc_search_ter_Proto(TerSrchApp_Contents_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			 hErr;

//	HxLOG_Info("AP_TerSrchSvc_Proc() in message(0x%x)\n", message);

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Info("\t[eMEVT_BUS_CREATE]\n\n");
__search_time_trace_start__;
		hErr = xmgr_search_ter_MsgGwmCreate (pstContents, hAction, p1, p2, p3);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Info("\t[eMEVT_BUS_TIMER]\n\n");
		hErr = xmgr_search_ter_MsgGwmTimer (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Info("\t[eMEVT_BUS_DESTROY]\n\n");
		hErr = xmgr_search_ter_MsgGwmDestroyed (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_ACTION_START:
		HxLOG_Info("\t[eMEVT_SEARCH_ACTION_START]\n\n");
		hErr = xmgr_search_ter_MsgAppSearchActionStart (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_SAVE_RESULT:
		HxLOG_Info("\t[eMEVT_SEARCH_SAVE_RESULT]\n\n");
		hErr = xmgr_search_ter_MsgAppSearchSaveResult (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_CANCEL:
		HxLOG_Info("\t[eMEVT_SEARCH_CANCEL]\n\n");
		hErr = xmgr_search_ter_MsgAppSearchCancel (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_ACTION_NOTIFY_BGJOB_FINISHED:
		HxLOG_Info("\t[eMEVT_ACTION_NOTIFY_BGJOB_FINISHED]\n\n");
		hErr = xmgr_search_ter_MsgAppBgJobFinished (pstContents, p1, p2);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_SEARCH_ACTION_KILL:
		HxLOG_Info("\t[eMEVT_SEARCH_ACTION_KILL]\n\n");
		hErr = xmgr_search_ter_MsgAppSearchKillProcesses (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SI_SEARCH_DONE:
		hErr = xmgr_search_ter_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		HxLOG_Info("\t[eSEVT_SI_SEARCH_DONE]\n\n");

		hErr = xmgr_search_ter_EvtSiSearchDone (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SI_SEARCH_FAIL:
		hErr = xmgr_search_ter_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		HxLOG_Info("\t[eSEVT_SI_SEARCH_DONE]\n\n");

		hErr = xmgr_search_ter_EvtSiSearchFail (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_LOCKED:

		HxLOG_Print("\t[eSEVT_CH_LOCKED]\n\n");

		hErr = xmgr_search_ter_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		hErr = xmgr_search_ter_EvtChLocked (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_NO_SIGNAL:
		hErr = xmgr_search_ter_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		HxLOG_Print("\t[eSEVT_CH_NO_SIGNAL]\n\n");

		hErr = xmgr_search_ter_EvtChNoSignal (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		return MESSAGE_BREAK;
		break;

	default:
		break;
	}

//	HxLOG_Info("AP_TerSrchSvc_Proc() out\n");

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/* End of File. ---------------------------------------------------------- */


