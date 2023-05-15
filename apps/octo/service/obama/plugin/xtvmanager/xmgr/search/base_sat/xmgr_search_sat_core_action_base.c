/**
	@file     ap_sat_search_svc.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/01/15		initiated					JK Baek\n
	 2008/03/14		renamed					wjmoh\n
	 2008/09/05		Satellite part is divided		jinh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
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
#include <svc_epg.h>

#include <isosvc.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_antenna.h>
#include <mgr_search.h>
#include <mgr_spec.h>

#include <svc_resources.h>
#include <mgr_rscmap.h>
#include <xmgr_search.h>
#include "../local_include/_xmgr_search.h"

#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#if defined(CONFIG_DEBUG)
#define	LOG							HxLOG_Print
#define	ERR_LOG						HxLOG_Error
#define	MSG_LOG						HxLOG_Message
#else
#define	LOG							while (0) ((int (*)(char *, ...)) 0)
#define	ERR_LOG						while (0) ((int (*)(char *, ...)) 0)
#define MSG_LOG						while (0) ((int (*)(char *, ...)) 0)
#endif



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
#define SEARCH_TIMER_LOCKING_TIME					(6*1000)	/* 6 sec */
#define SEARCH_TIMER_SI_TIME						(60*1000)	/* 60 sec */


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief search application���� ���࿡ �ʿ��� environment context  */
typedef struct tagXmgrSearch_SatContents_t
{
	HUINT32						 ulActionId;			// Search Application Action ID
	Handle_t					 hAction;				// Search Action handle
	HUINT32						 ulTunerId;				// Tuner ID

	// Search Options :
	DxCasType_e					 eCAFreeType;			//  ALL, FTA, CAS
	DxSvcType_e					 eServiceType;			// ALL, TV, Radio
	HBOOL						 bNetworkSearch;		// TRUE, FALSE
	HUINT32						 ucSearchType;			// Search type ... (for OP spec.)

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

	// === Satellite Information
	HUINT32						 ulAntennaType;			// Antenna Type : LNB only, DiSEqC, etc.
	HUINT32						 ulAntennaIndex;		// Antenna Index
	DbSvc_AntInfo_t					 stAntennaInfo;
} XmgrSearch_SatContents_t;


STATIC XmgrSearch_SatContents_t	s_SatSrchApp_Contents[NUM_SEARCH_ACTION];				/**< Search Appl. runtime context */

/* internal function prototype */

STATIC BUS_Result_t xproc_search_SatProto_Base(XmgrSearch_SatContents_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


#define	_________________________Body___________________________________




STATIC INLINE HERROR xmgr_search_SatCheckActionId (XmgrSearch_SatContents_t *pstContents, Handle_t hAction);
STATIC INLINE void xmgr_search_SatIncreaseSessionVersion (XmgrSearch_SatContents_t *pstContents);
STATIC DbSvc_TuningInfo_t *xmgr_search_SatGetCurrentTuningInfo (XmgrSearch_SatContents_t *pstContents);

STATIC HERROR xmgr_search_SatInitContents (XmgrSearch_SatContents_t *pstContents, Handle_t hAction, MgrSearch_Option_t *pstOption);
STATIC HERROR xmgr_search_SatMakeTpList (XmgrSearch_SatContents_t* pstContents, MgrSearch_Option_t *pstOption);

STATIC HERROR xmgr_search_SatSetTuningParamToSearch (XmgrSearch_SatContents_t *pstContents);

STATIC HERROR xmgr_search_SatDestroyContents (XmgrSearch_SatContents_t *pstContents);

STATIC HERROR xmgr_search_SatTuneSearchTp (XmgrSearch_SatContents_t *pstContents);
STATIC HERROR xmgr_search_SatSetNextTpToSearch (XmgrSearch_SatContents_t *pstContents);

STATIC HERROR xmgr_search_SatSendUpdateDisplayMsg (Handle_t hAction);
STATIC HERROR xmgr_search_SatSendSearchStartedMsg (Handle_t hAction);
STATIC HERROR xmgr_search_SatSendSearchFinished (Handle_t hAction);
//STATIC HERROR xmgr_search_SatSendSearchSaveFinishedMsg (void);
STATIC HERROR xmgr_search_SatSendSearchActionDestroyed (Handle_t hAction);

STATIC HERROR xmgr_search_SatRetrieveResults (XmgrSearch_SatContents_t* pstContents);
STATIC HERROR xmgr_search_SatAddDeliveryInfoInTpList (XmgrSearch_SatContents_t *pstContents);
//STATIC HERROR xmgr_search_SatSaveResult (Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
STATIC HBOOL xmgr_search_SatFilterSameTuningInfo (void *pvItem, void *pvFilter);
STATIC DbSvc_TuningInfo_t *xmgr_search_SatAllocTuneInfo (void);
STATIC void xmgr_search_SatFreeTuneInfo (void *pvTuneInfo);
STATIC void xmgr_search_SatReleaseAction (XmgrSearch_SatContents_t *pstContents);


STATIC HERROR xmgr_search_SatMsgGwmCreate (XmgrSearch_SatContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR xmgr_search_SatMsgGwmDestroyed (XmgrSearch_SatContents_t *pstContents);
STATIC HERROR xmgr_search_SatMsgAppSearchActionStart (XmgrSearch_SatContents_t *pstContents, HINT32 nAppHandle);
//STATIC HERROR xmgr_search_SatMsgAppSearchSaveResult (XmgrSearch_SatContents_t *pstContents, HINT32 nAppHandle);
STATIC HERROR xmgr_search_SatMsgAppSearchCancel (XmgrSearch_SatContents_t *pstContents, HINT32 nAppHandle);
//STATIC HERROR xmgr_search_SatMsgAppBgJobFinished (XmgrSearch_SatContents_t *pstContents, HINT32 nCbPtr, HINT32 nResult);
//STATIC HERROR xmgr_search_SatMsgAppSearchKillProcesses (XmgrSearch_SatContents_t *pstContents);
//STATIC HERROR xmgr_search_SatMsgGwmTimer (XmgrSearch_SatContents_t *pstContents, HINT32 nTimerId);
STATIC HERROR xmgr_search_SatEvtSiSearchDone (XmgrSearch_SatContents_t *pstContents);
STATIC HERROR xmgr_search_SatEvtSiSearchFail (XmgrSearch_SatContents_t *pstContents);
STATIC HERROR xmgr_search_SatEvtChLocked (XmgrSearch_SatContents_t *pstContents);
STATIC HERROR xmgr_search_SatEvtChNoSignal (XmgrSearch_SatContents_t *pstContents);


#define _________________________________________________________________________________

STATIC INLINE HERROR xmgr_search_SatCheckActionId (XmgrSearch_SatContents_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);

	if (pstContents->ulActionId != ulActionId)
	{
		ERR_LOG ("is not matched actionID(0x%x)! line(%d)\n", ulActionId, __LINE__);
		return MESSAGE_PASS;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC INLINE void xmgr_search_SatIncreaseSessionVersion (XmgrSearch_SatContents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

STATIC DbSvc_TuningInfo_t *xmgr_search_SatGetCurrentTuningInfo (XmgrSearch_SatContents_t *pstContents)
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

#define _____STATUS_SETTING_____

STATIC HERROR xmgr_search_SatChangeProgressNumber (XmgrSearch_SatContents_t *pstContents)
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



#define	_____CONTENTS_INITIATION_____

STATIC HERROR xmgr_search_SatInitContents (XmgrSearch_SatContents_t *pstContents, Handle_t hAction, MgrSearch_Option_t *pstOption)
{
	// 1. Clear all the contents.
	HxSTD_memset (pstContents, 0, sizeof(XmgrSearch_SatContents_t));

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
	pstContents->ucSearchType	= pstOption->eSearchType;			// Search Mode : �ӽ����� �ڵ� : �����δ� ���� ���� �ʿ�
#if 0			// ���� �ش� Spec�� �Բ� ���� ����� ã�ƾ� �Ѵ�.
	// 4-2. Antenna Type
	hErr = MGR_SEARCHUTIL_GetSatAntennaTypeFromDb (&ulValue);
	pstContents->ulAntennaType = (hErr == ERR_OK) ? ulValue : eUiAntType_LNB_Only;
#endif

	// 6. Now the tuning information :
	xmgr_search_SatMakeTpList (pstContents, pstOption);

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_search_SatMakeTpList (XmgrSearch_SatContents_t* pstContents, MgrSearch_Option_t *pstOption)
{
	HUINT32				 ulCount;
	DbSvc_TuningInfo_t	*pstTuneInfo;

	if ((0 == pstOption->ulTpNum) || (NULL == pstOption->pstTpArray))
	{
		return ERR_FAIL;
	}

	for (ulCount = 0; ulCount < pstOption->ulTpNum; ulCount++)
	{
		pstTuneInfo = xmgr_search_SatAllocTuneInfo();
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

STATIC HERROR xmgr_search_SatSetTuningParamToSearch (XmgrSearch_SatContents_t *pstContents)
{
	DbSvc_AntInfo_t			 stAntInfo;
	DbSvc_TsInfo_t		 stSvcTsInfo;
	DbSvc_TuningInfo_t	*pstCurrTp;
	HERROR				 hErr;

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

	// Set the antenna info :
	hErr = DB_SVC_GetAntInfo ((HUINT16)pstCurrTp->sat.ucAntId, &stAntInfo);
	if (hErr != ERR_OK)
	{
		// Finish the search
		pstContents->eAppState = eSearchApp_state_searchend;
		return ERR_BUS_NO_OBJECT;
	}

	if(stAntInfo.eSatType != eSatType_ALL)
	{	//pFilterInfo->stAttr.eSatType
		MGR_ANTENNA_GetSatelliteName(stAntInfo.eSatType, stAntInfo.szName, MAX_ANT_NAME);
	}

	pstContents->ulAntennaIndex = (HUINT32)pstCurrTp->sat.ucAntId;
	HxSTD_memcpy (&(pstContents->stAntennaInfo), &stAntInfo, sizeof(DbSvc_AntInfo_t));

	// current Tp�� Ʃ��info ä�� �ش�.
	stSvcTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
	stSvcTsInfo.ucTunerId = (HUINT8)pstContents->ulTunerId;
	HxSTD_memcpy (&(stSvcTsInfo.stTuningParam), pstCurrTp, sizeof(DbSvc_TuningInfo_t));

	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stSvcTsInfo, &(pstContents->stSearchTuningInfo));

	/* Antenna Info ���� */
	MGR_SEARCHRESULT_SetAntennaInfo (pstContents->ulActionId, pstContents->stAntennaInfo);

	MGR_SEARCHRESULT_SetAntennaId(pstContents->ulActionId, (HUINT32)pstCurrTp->sat.ucAntId); // MOONSG_sss

	/* TP Progress Update */
	xmgr_search_SatChangeProgressNumber (pstContents);

//	xmgr_search_SatSendUpdateDisplayMsg(pstContents->hAction); // MOONSG_sss

	return ERR_BUS_NO_ERROR;
}





#define	_____CONTENTS_DESTRUCTION_____

STATIC HERROR xmgr_search_SatDestroyContents (XmgrSearch_SatContents_t *pstContents)
{
	// Clear the search results :
	MGR_SEARCHDATA_ClearProviderArray (&(pstContents->stPrvArray));
	MGR_SEARCHDATA_ClearNetworkArray (&(pstContents->stNetArray));
	MGR_SEARCHDATA_ClearTsArray (&(pstContents->stTsArray));
	MGR_SEARCHDATA_ClearBouquetArray (&(pstContents->stBqArray));

 	pstContents->pstTpToSearch = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTpToSearch, xmgr_search_SatFreeTuneInfo);
 	pstContents->pstFinishedTps = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstFinishedTps, xmgr_search_SatFreeTuneInfo);

	return ERR_BUS_NO_ERROR;
 }


#define	_____DATA_CONVERSION_____



#define	_____STATE_PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_search_SatTuneSearchTp (XmgrSearch_SatContents_t *pstContents)
{
	HBOOL				 bRetry;
	SvcRsc_Info_t			 stRscInfo;
	HERROR				 hErr;

	bRetry = TRUE;
	MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);

	while (bRetry == TRUE)
	{
		hErr = xmgr_search_SatSetTuningParamToSearch (pstContents);
		if (hErr != ERR_OK)
		{
			LOG ("[xmgr_search_SatTuneSearchTp] No Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			return ERR_BUS_NO_OBJECT;
		}

		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			LOG ("[xmgr_search_SatTuneSearchTp] Tuner Stop Failed\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			return ERR_BUS_NO_OBJECT;
		}

		xmgr_search_SatIncreaseSessionVersion (pstContents);			// Tuning ���� �ݵ�� Action Version�� ������Ʈ�ؾ� �Ѵ�.
		hErr = SVC_CH_StartTune(pstContents->hAction, &stRscInfo, (HUINT16)-1, &pstContents->stSearchTuningInfo);
		if (hErr == ERR_OK)
		{
			pstContents->eAppState = eSearchApp_state_tryTuning;

			// DD���� Tuning Signal�� �ö���� �ʾ� �������� ��ٸ��� ���� ���� ���� ���⼭ timer�� ó���ϵ��� �Ѵ�.
			BUS_SetTimer (SEARCH_TIMER_LOCKING_ID, SEARCH_TIMER_LOCKING_TIME);
			return ERR_BUS_NO_ERROR;
		}

		// Tuning failure : Go to the next TP
		hErr = xmgr_search_SatSetNextTpToSearch (pstContents);
		if (hErr != ERR_OK)
		{
			LOG ("[xmgr_search_SatTuneSearchTp] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			bRetry = FALSE;
		}
	}

	// All TP Completed:

	return ERR_BUS_NO_OBJECT;
}

STATIC HERROR xmgr_search_SatSetNextTpToSearch (XmgrSearch_SatContents_t *pstContents)
{
	POINTER_LIST_T		*pstSrchTps, *pstCompTps;
	DbSvc_TuningInfo_t	*pstTuneInfo;

	pstSrchTps = pstContents->pstTpToSearch;
	pstCompTps = pstContents->pstFinishedTps;

	if (pstSrchTps == NULL)					{ return ERR_BUS_NO_OBJECT; }

	// �� �տ� �ִ� ���� ���� TP �̴�.
	pstTuneInfo = (DbSvc_TuningInfo_t *)pstSrchTps->pvContents;
	if (pstTuneInfo == NULL)				{ return ERR_BUS_NO_OBJECT; }

	// ���� TP�� Search TP list���� �����ϰ�, ��� Completed Tp List�� �߰��Ѵ�.
	pstSrchTps = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstSrchTps, pstSrchTps, NULL);
	UTIL_LINKEDLIST_CircularizePointerList (pstSrchTps);

	pstCompTps = UTIL_LINKEDLIST_AppendListItemToPointerList (pstCompTps, (void *)pstTuneInfo);
	UTIL_LINKEDLIST_CircularizePointerList (pstCompTps);

	pstContents->pstTpToSearch = pstSrchTps;
	pstContents->pstFinishedTps = pstCompTps;

	// ���� TP�� ���� ������ �����ؾ� �Ѵ�.
	if (pstSrchTps == NULL)
	{
		return ERR_BUS_NO_OBJECT;
	}

	return ERR_BUS_NO_ERROR;
}


#define	_____MESSAGE_FUNCTIONS_____

// UI ���� ����� ������ Message
STATIC HERROR xmgr_search_SatSendUpdateDisplayMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_STATUS_CHANGED, (Handle_t)hAction, 0, 0, 0);
}

// UI ���� ���¸� �˷��ִ� Message
STATIC HERROR xmgr_search_SatSendSearchStartedMsg (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_STARTED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

STATIC HERROR xmgr_search_SatSendSearchFinished (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_FINISHED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}

#if 0
STATIC HERROR xmgr_search_SatSendSearchSaveFinishedMsg (void)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_SAVE_FINISHED, (Handle_t)NULL, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}
#endif

STATIC HERROR xmgr_search_SatSendSearchActionDestroyed (Handle_t hAction)
{
	return (HERROR)BUS_PostMessage (NULL, eMEVT_SEARCH_ACTION_DESTROYED, (Handle_t)hAction, (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);
}


#define	_____PROCESSING_FUNCTIONS_____
STATIC HERROR xmgr_search_SatRetrieveResults (XmgrSearch_SatContents_t* pstContents)
{
	HUINT32					 ulIndex, ulItemNum;
	HUINT32					 ulBqNum, ulPrvNum, ulSvcNum;
	HUINT32					 ulTempNetIdx, ulTempTsIdx, ulTempPrvIdx;
	HINT32					 nDbTsCnt, nDbTsNum;
	Handle_t				 hSvc;
//	HBOOL					 bAlreadyInDb;
	HBOOL					 bNewTs;
	HUINT16					 usNetIdx = 0, usTsIdx = 0, usPrvIdx;
	DbSvc_TsInfo_t			 stTsInfo;
	DbSvc_TsInfo_t			*pstTsInfoList;
	DbSvc_NetInfo_t			 stNetInfo;
	DbSvc_TsCondition_t		 stTsCond;
//	DbSvc_TripleId_t		 	 stTripleId;
	DbSvc_TsInfo_t			*pstTsInfo = NULL;
	DbSvc_NetInfo_t			*pstNetInfo = NULL;
	DbSvc_Info_t				*astSvcArray = NULL;
	DbSvc_BouqInfo_t			*pstBqArray = NULL;
	DbSvc_PrvInfo_t			*pstPrvArray = NULL;
	SRCHSVC_AppendFlag_t	*peSvcFlagArray = NULL;
	DbSvc_TuningInfo_t		*pstCurrTuneInfo;
	DbSvc_TuningInfo_t		 stSvcTuneInfo;
	DxTuneParam_t			 stChTuneInfo;
	HERROR					 hErr;
	HUINT32					 ulInfoNum = 0;

	//------------------------------------------------------------------------------
	// 1. Network Info
	// ���� Network�� �� �ϳ��� �� �ۿ� ����.
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, &ulItemNum);
	if (hErr != ERR_OK)					{ ulItemNum = 0; }

	if (ulItemNum == 0)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	pstNetInfo = (DbSvc_NetInfo_t *)OxAP_Malloc (sizeof(DbSvc_NetInfo_t));
	if (pstNetInfo == NULL)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Network, 1, &ulInfoNum, (void *)pstNetInfo);
	if (hErr != ERR_OK)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// �����̹Ƿ� ���⼭ Delivery Type�� ������ �� �ִ�.
	pstNetInfo->eOrgDeliType = eDxDELIVERY_TYPE_SAT;

	//------------------------------------------------------------------------------
	// 2. TS Info
	// TS Info�� ������ ���� ���� �߿��ϴ�.
	// ���⼭ TS Info�� ������ ���ϸ� search fail�� �����Ѵ�.
	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, &ulItemNum);
	if (hErr != ERR_OK)					{ ulItemNum = 0; }

	if (ulItemNum == 0)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// ���� TS�� �� �ϳ��� �� �ۿ� ����.
	pstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstTsInfo == NULL)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Transponder, 1, &ulInfoNum, (void *)pstTsInfo);
	if (hErr != ERR_OK)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// SI���� �÷��� TS Info�� ���� ������ Tuning Info ���� �߰��Ͽ� DB�� ������ �� �ִ� data�� ��ȯ�Ѵ�.
	hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
	if (hErr == ERR_OK)
	{
		MGR_SEARCHUTIL_ConvertChTuningParamToSvcTuningInfo (&stChTuneInfo, &stSvcTuneInfo);
		pstCurrTuneInfo = &stSvcTuneInfo;
	}
	else
	{
		pstCurrTuneInfo = xmgr_search_SatGetCurrentTuningInfo (pstContents);
		HxSTD_memcpy(&stSvcTuneInfo, pstCurrTuneInfo, sizeof(DbSvc_TuningInfo_t));
	}

	if (pstCurrTuneInfo == NULL)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	// Tuner ID
	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
	// Current TP Info
	HxSTD_memcpy (&(pstTsInfo->stTuningParam), pstCurrTuneInfo, sizeof(DbSvc_TuningInfo_t));
	// Antenna Idx
	pstTsInfo->stTuningParam.sat.ucAntId = (HUINT8)(pstContents->ulAntennaIndex);
	// Sat Type
	pstTsInfo->stTuningParam.sat.eSatType = pstContents->stAntennaInfo.eSatType;


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
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	astSvcArray = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * ulSvcNum);
	if (astSvcArray == NULL)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_BUS_OUT_OF_MEMORY;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Service, ulSvcNum, &ulInfoNum, (void *)astSvcArray);
	if (astSvcArray == NULL)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	//------------------------------------------------------------------------------
	// 6. Store the searched results to the contents :
	// 6-1. Network Info
	hErr = MGR_SEARCHDATA_AddNetwork (&(pstContents->stNetArray), pstNetInfo, &ulTempNetIdx);
	if (hErr != ERR_OK)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	//------------------------------------------------------------------------------
	// 1. adding/updating a network
	//------------------------------------------------------------------------------
	if (pstNetInfo != NULL)
	{
		pstNetInfo->eOrgDeliType = eDxDELIVERY_TYPE_SAT;
		// Network Info�� �̹� ��ϵǾ� �ִ� ������ üũ�Ѵ�.
		hErr = DB_SVC_FindNetworkIndexById(pstNetInfo->eOrgDeliType, pstNetInfo->usNetId, &usNetIdx);
		if (hErr == ERR_OK)
		{
			if (pstNetInfo->szNetName[0] != '\0')
			{
				DB_SVC_GetNetworkInfo (usNetIdx, &stNetInfo);
				if (MWC_UTIL_DvbStrcmp (pstNetInfo->szNetName, stNetInfo.szNetName) != 0)
				{
					// Network Name�� �޶������� Update �Ѵ�.
					DB_SVC_UpdateNetworkInfo (usNetIdx, pstNetInfo);
					LOG ("\t= [NET] tempIdx = %d, netIdx = %d --> update\n", ulTempNetIdx, usNetIdx);
				}
			}
		}
		else
		{
			// ��ϵǾ� ���� �����Ƿ� ���� �߰��Ѵ�.
			DB_SVC_AddNetworkInfo (pstNetInfo, &usNetIdx);
			LOG ("\t= [NET] tempIdx = %d, netIdx = %d --> add\n", ulTempNetIdx, usNetIdx);
		}
	}

	// 6-2. Ts Info
	// Network Info�� ���� �� ���� index ���� temporary index�ν� TsInfo�� �ִ´�.
	pstTsInfo->usNetIdx = (HUINT16)ulTempNetIdx;
	pstTsInfo->eDeliType = eDxDELIVERY_TYPE_SAT;

	hErr = MGR_SEARCHDATA_AddTs (&(pstContents->stTsArray), pstTsInfo, &ulTempTsIdx);
	if (hErr != ERR_OK)
	{
		ERR_LOG ("Error Log !!\n");
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
		pstTsInfo->eDeliType = eDxDELIVERY_TYPE_SAT;

		// TS Info�� OnId, TsId�� ��ϵǾ� �ִ� �� Ȯ���Ѵ�.
		DB_SVC_InitTsFindCondition (&stTsCond);
		stTsCond.nDeliType	= eDxDELIVERY_TYPE_SAT;
		stTsCond.nOnId		= (HINT32)pstTsInfo->usOnId;
		stTsCond.nTsId		= (HINT32)pstTsInfo->usTsId;

		pstTsInfoList = NULL;

		hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoList);
		if (hErr == ERR_OK && pstTsInfoList != NULL)
		{
			for (nDbTsCnt = 0; nDbTsCnt < nDbTsNum; nDbTsCnt++)
			{
				if (pstTsInfoList[nDbTsCnt].stTuningParam.sat.ucAntId == pstContents->ulAntennaIndex)
				// Found the same TsInfo including the antenna index
				{
					HBOOL bUpdateTs = FALSE;
					HBOOL bAddTs = FALSE;

					stTsInfo = pstTsInfoList[nDbTsCnt];
					usTsIdx = stTsInfo.usUniqueId;

					hErr = xmgr_search_CheckAddTsInfoCondition(eDxDELIVERY_TYPE_SAT, &(pstTsInfo->stTuningParam), &(stTsInfo.stTuningParam), &bAddTs, &bUpdateTs);
					if ((hErr == ERR_OK) && (bUpdateTs == TRUE))
					{
						DB_SVC_UpdateTsInfo (usTsIdx, pstTsInfo);
						LOG ("\t\t= [TS] tempIdx = %d, tsIdx = %d --> update\n", ulTempTsIdx, usTsIdx);
					}

					if ((hErr == ERR_OK) && (bAddTs == FALSE))
					{
						bNewTs = FALSE;
						break;
					}
				}
			}

			DB_SVC_FreeTsInfoList (pstTsInfoList);
		}

		if (bNewTs == TRUE)
		{
			DB_SVC_AddTsInfo (pstTsInfo, &usTsIdx);
			LOG ("\t\t= [TS] tempIdx = %d, tsIdx = %d --> add\n", ulTempTsIdx, usTsIdx);
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
	// Svc Info�� ���� Provider Index�� ������ ���� Provider Info�� ���� ���� ��ġ�� �̸� ����.
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
			// Provider Info�� �̸� ��ϵǾ� �ִ��� Ȯ���Ѵ�.
			hErr = DB_SVC_FindProviderIndexByName (pstPrvArray[ulIndex].szPrvName, &usPrvIdx);
			if (hErr != ERR_OK)
			{
				// ��ϵǾ� ���� ������ �߰��Ѵ�.
				DB_SVC_AddProviderInfo (&pstPrvArray[ulIndex], &usPrvIdx);
				LOG ("\t\t= [PRV] tempIdx = %d, prvIdx = %d --> add\n", ulTempPrvIdx, usPrvIdx);
			}
		}
	}

	// 6-5. Service Info
	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		DbSvc_SetDeliType(astSvcArray[ulIndex], eDxDELIVERY_TYPE_SAT);
		DbSvc_SetSatType(&astSvcArray[ulIndex], pstContents->stAntennaInfo.eSatType);
		DbSvc_SetAntIdx(&astSvcArray[ulIndex], pstTsInfo->stTuningParam.sat.ucAntId);
		DbSvc_SetOnId(&astSvcArray[ulIndex], pstTsInfo->usOnId);
		DbSvc_SetTsId(&astSvcArray[ulIndex], pstTsInfo->usTsId);
		DbSvc_SetTsIdx(&astSvcArray[ulIndex], usTsIdx);
		DbSvc_SetPrvIdx(&astSvcArray[ulIndex], 0); // TODO:
	}

	peSvcFlagArray = (SRCHSVC_AppendFlag_t *)OxAP_Malloc (sizeof(SRCHSVC_AppendFlag_t) * ulSvcNum);
	if (peSvcFlagArray == NULL)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_SetLcnServiceNumber(pstContents->hAction, bNewTs, ulSvcNum, astSvcArray, peSvcFlagArray);
	if (hErr != ERR_OK)
	{
		ERR_LOG ("Error Log !!\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	for (ulIndex = 0; ulIndex < ulSvcNum; ulIndex++)
	{
		switch (peSvcFlagArray[ulIndex])
		{
		case eSRCHSVC_ADD:
			LOG (">>> ADD    : \t%04d, \t%s\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			DB_SVC_AddServiceInfo (&astSvcArray[ulIndex], &hSvc);
			DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));

			// Add the service lists in the GUI :
			MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &(astSvcArray[ulIndex]));
			break;

		case eSRCHSVC_UPDATE:
			LOG (">>> UPDATE : \t%04d, \t%s\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			hErr = DB_SVC_GetServiceHandle (DbSvc_GetSvcSection(&astSvcArray[ulIndex]), DbSvc_GetTsIdx(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), &hSvc);
			if (hErr == ERR_OK)
			{
				DB_SVC_UpdateServiceInfo (hSvc, &astSvcArray[ulIndex]);
				DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));

				// Add the service lists in the GUI :
				MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &(astSvcArray[ulIndex]));
			}

			break;

		default:
			LOG (">>> SKIP   : \t%04d, \t%s\n", DbSvc_GetLcn(&astSvcArray[ulIndex]), DbSvc_GetName(&astSvcArray[ulIndex]));
			hErr = DB_SVC_GetServiceHandle (DbSvc_GetSvcSection(&astSvcArray[ulIndex]), DbSvc_GetTsIdx(&astSvcArray[ulIndex]), DbSvc_GetSvcId(&astSvcArray[ulIndex]), &hSvc);
			if (hErr == ERR_OK)
			{
				DbSvc_SetSvcIdx(&astSvcArray[ulIndex], DB_SVC_GetServiceUID(hSvc));

				// Add the service lists in the GUI :
				MGR_SEARCHRESULT_AddService (pstContents->ulActionId, &(astSvcArray[ulIndex]));
			}

			break;
		}
	}

	(void)MGR_SEARCHRESULT_AddOnTsId(pstContents->ulActionId, pstTsInfo->usOnId, pstTsInfo->usTsId);

	//  list��������� �˸�
	xmgr_search_SatSendUpdateDisplayMsg(pstContents->hAction);

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

STATIC HERROR xmgr_search_SatAddDeliveryInfoInTpList (XmgrSearch_SatContents_t *pstContents)
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
		ERR_LOG ("Error Log !!\n");
		return ERR_BUS_NO_OBJECT;
	}

	if (pstContents->pstTpToSearch->pvContents == NULL)
	{
		ERR_LOG ("Error Log !!\n");
		return ERR_BUS_NO_OBJECT;
	}

	hErr = SVC_SI_CountSearchedResult(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Delivery, &ulNumDeliInfo);
	if (hErr != ERR_OK || ulNumDeliInfo == 0)
	{
		ERR_LOG ("Error Log !!\n");
		return ERR_BUS_NO_OBJECT;
	}

	pstDeliTsArray = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t) * ulNumDeliInfo);
	if (pstDeliTsArray == NULL)
	{
		ERR_LOG ("Error Log !!\n");
		return ERR_BUS_OUT_OF_MEMORY;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->ulActionId, eSI_SRCH_INFOTYPE_Delivery, ulNumDeliInfo, &ulInfoNum, (void *)pstDeliTsArray);
	if (hErr != ERR_OK)
	{
		ERR_LOG ("Error Log !!\n");
		OxAP_Free (pstDeliTsArray);
		return ERR_BUS_NO_OBJECT;
	}

	// ���� TP (ó�� item)�� pstDeliTpList�� �� ó���� �ְ�,  �� �Ŀ� NextTpList������ ����.
	// �̷��� �ϴ� ������ ���߿� CurrTp - DeliveryTpLists - Next Tps  ������ ��ġ�� ���ؼ��̴�.

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

		//NIT�� ���� ã�� TP�� ���� TP�� �ش��ϴ� antenna ID�� �־� �ش�.
		pstDeliTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
		pstDeliTsInfo->stTuningParam.sat.ucAntId = pstSvcTuneInfo->sat.ucAntId;

		// 1. �̹� ó���� TP�� ������ TP�ΰ� ?
		pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstFinishedTps, 0,
																xmgr_search_SatFilterSameTuningInfo,
																(void *)&(pstDeliTsInfo->stTuningParam));
		if (pstFound != NULL)					{ continue; }

		// 2. ���� TP Ȥ�� �̹� Delivery TP List�� �� TP�� ������ TP �ΰ� ?
		pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstDeliTpList, 0,
																xmgr_search_SatFilterSameTuningInfo,
																(void *)&(pstDeliTsInfo->stTuningParam));
		if (pstFound != NULL)					{ continue; }

		// 3. �� TP�� �������� Next TP�� �����ϴ°� ?
		// �� ���� �ó������� ���� �޶��� �� �ִ�.
		// ���⼭�� ���� network �� TP���� �� ���� ��Ƽ� ó���ϱ� ����
		// �ڿ� �ִ� Next TPs�� �ִ� item ���� ����� DeliTpList�� �߰��ϴ� �������� ����.
		pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstNextTpList, 0,
																xmgr_search_SatFilterSameTuningInfo,
																(void *)&(pstDeliTsInfo->stTuningParam));
		while (pstFound != NULL)
		{
			pstNextTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstNextTpList, pstFound, xmgr_search_SatFreeTuneInfo);
			UTIL_LINKEDLIST_CircularizePointerList (pstNextTpList);

			pstFound = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstNextTpList, 0,
																	xmgr_search_SatFilterSameTuningInfo,
																	(void *)&(pstDeliTsInfo->stTuningParam));
		}

		// 4. Delivery TP�� �߰�
		pstDeliSvcTuneInfo = xmgr_search_SatAllocTuneInfo();
		if (pstDeliSvcTuneInfo != NULL)
		{
			HxSTD_memcpy (pstDeliSvcTuneInfo, &(pstDeliTsInfo->stTuningParam), sizeof(DbSvc_TuningInfo_t));
			pstDeliTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstDeliTpList, (void *)pstDeliSvcTuneInfo);
			UTIL_LINKEDLIST_CircularizePointerList (pstDeliTpList);
		}
	}

	// Delivery Tp List�� ���� Next Tp List�� ����
	pstRest = NULL;
	pstContents->pstTpToSearch = UTIL_LINKEDLIST_CombineTwoPointerLists (pstDeliTpList, pstNextTpList, NULL, NULL, &pstRest);
	UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstTpToSearch);
	UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstRest, xmgr_search_SatFreeTuneInfo);

	return ERR_BUS_NO_ERROR;
}


STATIC HBOOL xmgr_search_SatFilterSameTuningInfo (void *pvItem, void *pvFilter)
{
	DbSvc_TuningInfo_t *pstItem, *pstFilter;

	pstItem = (DbSvc_TuningInfo_t *)pvItem;
	pstFilter = (DbSvc_TuningInfo_t *)pvFilter;

	if (DB_SVC_UtilCompareDbTuningParam(eDxDELIVERY_TYPE_SAT, pstItem, pstFilter) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC DbSvc_TuningInfo_t *xmgr_search_SatAllocTuneInfo (void)
{
	return (DbSvc_TuningInfo_t *)OxAP_Malloc (sizeof(DbSvc_TuningInfo_t));
}

STATIC void xmgr_search_SatFreeTuneInfo (void *pvTuneInfo)
{
	if (pvTuneInfo)
	{
		OxAP_Free ((DbSvc_TuningInfo_t *)pvTuneInfo);
	}
}

STATIC void xmgr_search_SatReleaseAction (XmgrSearch_SatContents_t *pstContents)
{
	HERROR			 hErr;

	LOG ("called xmgr_search_SatReleaseAction()\n");
	hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
	if (hErr != ERR_OK)
	{
		ERR_LOG ("Error Log !!\n");
	}

	// Release the action :
	MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
}


#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____

//eMEVT_BUS_CREATE
STATIC HERROR xmgr_search_SatMsgGwmCreate (XmgrSearch_SatContents_t *pstContents, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	MgrSearch_Option_t	*pstOption = (MgrSearch_Option_t *)p1;

	if (NULL == pstOption)
	{
		BUS_MGR_Destroy (NULL);
		return ERR_BUS_NO_ERROR;
	}

	// 1. Initialize the contents
	xmgr_search_SatInitContents (pstContents, hAction, pstOption);

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY:
STATIC HERROR xmgr_search_SatMsgGwmDestroyed (XmgrSearch_SatContents_t *pstContents)
{
	if (pstContents->eAppState == eSearchApp_state_waitingSi)
	{
		// Stop SI & Cancel the search :
		SVC_SI_StopSearch(pstContents->hAction, TRUE);
	}

	if (pstContents->eAppState != eSearchApp_state_searchend)
	{
		// Search�� ������ �ʾҴٸ� Action Release
		xmgr_search_SatReleaseAction (pstContents);
	}

	/* Moonsg : Antenna type�� LNB_ONLY, DISEQC�� �ƴϸ� cable detection�� ���� �ʱ� ������ �ϴ��� signle cable�� ó�� */
	if ( (pstContents->stAntennaInfo.eAntennaType != eDxANT_TYPE_LNB_ONLY)
		&& (pstContents->stAntennaInfo.eAntennaType != eDxANT_TYPE_DISEQC) )
	{
		HERROR		hErr;
//		MWC_PARAM_SetAntennaConnectionType (eDxANTCONN_ONECABLE);
		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32)eDxANTCONN_ONECABLE, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_SetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE) Error!!!\n");
		}
		SVC_PIPE_SetRfInputAbility (0, TRUE);
		SVC_PIPE_SetRfInputAbility (1, FALSE);
	}

	// Destroy the contents
	xmgr_search_SatDestroyContents (pstContents);

	// Send the destoyed message to the UI procedure
	xmgr_search_SatSendSearchActionDestroyed(pstContents->hAction);

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_search_SatMsgAppSearchActionStart (XmgrSearch_SatContents_t *pstContents, HINT32 nAppHandle)
{
	// UI -> Search Action ���� ���� �޽����� �ٸ� Search Action ���ε� �� �� �ִ� �޽����̱� ������ (NULL)
	// ����� MESSAGE_BREAK �� ����ϸ� �ȵȴ�.
	HERROR			 hErr;

	// 1. Set the TP to tuning :
	xmgr_search_SatSetTuningParamToSearch(pstContents);

	// 2. init Search results
	MGR_SEARCH_InitResult(pstContents->ulActionId);

	// 3.Get the Action
	hErr = MGR_ACTION_SetupSearch (pstContents->ulActionId, &pstContents->stSearchTuningInfo, eAmPriority_SEARCH);
	if (hErr != ERR_OK)
	{
		// search action�� ���� �Ҵ��� �̷�� ���� �������Ƿ� search appl.�� ��� �� ����.
		ERR_LOG ("Error Log !!\n");

		BUS_MGR_Destroy(NULL);
		return ERR_OK;
	}

	hErr = MGR_ACTION_Take (pstContents->ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		// search action�� ���� �Ҵ��� �̷�� ���� �������Ƿ� search appl.�� ��� �� ����.
		ERR_LOG ("Error Log !!\n");

		BUS_MGR_Destroy(NULL);
		return ERR_OK;
	}

	if (pstContents->eAppState != eSearchApp_state_searchend)
	{
		pstContents->eAppState = eSearchApp_state_idle;
	}

	// Tune the TP
	hErr = xmgr_search_SatTuneSearchTp (pstContents);
	if (hErr != ERR_OK)
	{
		LOG ("[xmgr_search_SatMsgGwmCreate] No Next Tp left to tune\n");
		pstContents->eAppState = eSearchApp_state_searchend;
	}

	xmgr_search_SatSendSearchStartedMsg (pstContents->hAction);

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_SatSendSearchFinished(pstContents->hAction);
		xmgr_search_SatReleaseAction (pstContents);
	}

	return ERR_OK;

}

// eMEVT_SEARCH_CANCEL
STATIC HERROR xmgr_search_SatMsgAppSearchCancel (XmgrSearch_SatContents_t *pstContents, HINT32 nAppHandle)
{
	// UI -> Search Action ���� ���� �޽����� �ٸ� Search Action ���ε� �� �� �ִ� �޽����̱� ������ (NULL)
	// ����� MESSAGE_BREAK �� ����ϸ� �ȵȴ�.

	if (pstContents->eAppState == eSearchApp_state_waitingSi)
	{
		// Stop SI & Cancel the search :
		SVC_SI_StopSearch(pstContents->hAction, TRUE);
	}

	pstContents->eAppState = eSearchApp_state_searchend;
	xmgr_search_SatSendUpdateDisplayMsg(pstContents->hAction);

	// �� �Լ��� ȣ��Ǹ� oapi builder���� search proc(this proc)�� destroy���ѹ����� search resume�� �ȵȴ�.
	// default�� ���Ƶΰ� �ʿ�� ����.. ���Ѻ��� ����ϴ� ���� ���ٸ� ������ ����
#if 0//!defined(CONFIG_PROD_HMS1000S)
	// UI�� �������� message�� octo 1.0�� UI proc�� �޾Ƽ� break ó���Ͽ�����
	// ����� break ���� �ʰ� �ٽ� builder���� �޾� action proc�� kill �Ͽ� ���� �߻��Ͽ� ����.
	xmgr_search_SatSendSearchFinished(pstContents->hAction);
#endif
	xmgr_search_SatReleaseAction (pstContents);
	return ERR_OK;
}

#if 0
// eMEVT_ACTION_NOTIFY_BGJOB_FINISHED:
STATIC HERROR xmgr_search_SatMsgAppBgJobFinished (XmgrSearch_SatContents_t *pstContents, HINT32 nCbPtr, HINT32 nResult)
{
	(void)pstContents;
	(void)nCbPtr;
	(void)nResult;

	if (nCbPtr == (HINT32)xmgr_search_SatSaveResult)
	{
#if		defined (CONFIG_MW_MM_PVR) && defined (CONFIG_MW_EPG_TVTV)
		HBOOL				bTvTvEnable = FALSE;
		HERROR				hErr = ERR_FAIL;

		/* Load TvTv EPG. */
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_TVTV_ENABLE, (HUINT32 *)&bTvTvEnable, 0);
		if (hErr == ERR_OK && bTvTvEnable == TRUE )
		{
			//MW_EPG_LoadXMLFile(HANDLE_NULL, HANDLE_NULL);
			SVC_EPG_LoadData(eEPG_KIND_TVTV, HANDLE_NULL, HANDLE_NULL);
		}
#endif
		xmgr_search_SatSendSearchSaveFinishedMsg();
		return MESSAGE_BREAK;
	}

	return ERR_OK;
}
#endif

#if 0
// MSG_APP_SEARCH_KILL_PROCESSES
STATIC HERROR xmgr_search_SatMsgAppSearchKillProcesses (XmgrSearch_SatContents_t *pstContents)
{
	// UI -> Search Action ���� ���� �޽����� �ٸ� Search Action ���ε� �� �� �ִ� �޽����̱� ������ (NULL)
	// ����� MESSAGE_BREAK �� ����ϸ� �ȵȴ�.

	if (pstContents->eAppState == eSearchApp_state_waitingSi)
	{
		// Stop SI & Cancel the search :
		MW_SI_StopSearch (pstContents->hAction, eSyncMode);
	}

	if (pstContents->eAppState != eSearchApp_state_searchend)
	{
		pstContents->eAppState = eSearchApp_state_searchend;
		xmgr_search_SatSendSearchFinished(pstContents->hAction);
		xmgr_search_SatReleaseAction (pstContents);
	}

	BUS_MGR_Destroy (NULL);
	return ERR_OK;
}

// eMEVT_BUS_TIMER:
STATIC HERROR xmgr_search_SatMsgGwmTimer (XmgrSearch_SatContents_t *pstContents, HINT32 nTimerId)
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
			// ���� TP �� �Ѿ�� �Ѵ�.
			hErr = xmgr_search_SatSetNextTpToSearch (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_search_SatTuneSearchTp (pstContents);
			}

			if (hErr != ERR_OK)
			{
				LOG ("[xmgr_search_SatMsgGwmTimer] No Next Tp left to tune\n");
				pstContents->eAppState = eSearchApp_state_searchend;
			}
		}

		hRetValue = MESSAGE_BREAK;
		break;

	case SEARCH_TIMER_SI_ID:
		BUS_KillTimer (nTimerId);

		if (pstContents->eAppState == eSearchApp_state_waitingSi)
		{
			// MW SI�� Stop��Ű�� ���� TP�� �Ѿ���� �Ѵ�.
			hErr = SVC_SI_StopSearch(pstContents->hAction, TRUE);
			if (hErr == ERR_OK)
			{
				// ���� TP �� �Ѿ�� �Ѵ�.
				hErr = xmgr_search_SatSetNextTpToSearch (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_search_SatTuneSearchTp (pstContents);
				}
			}

			if (hErr != ERR_OK)
			{
				LOG ("[xmgr_search_SatMsgGwmTimer] Cannot proceed anymore\n");
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
		/* TP Progress Update */
		xmgr_search_SatChangeProgressNumber (pstContents);
		xmgr_search_SatSendUpdateDisplayMsg(pstContents->hAction);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_SatSendSearchFinished(pstContents->hAction);
		xmgr_search_SatReleaseAction (pstContents);
	}

	return hRetValue;
}
#endif

// eSEVT_SI_SEARCH_DONE:
STATIC HERROR xmgr_search_SatEvtSiSearchDone (XmgrSearch_SatContents_t *pstContents)
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

		// �̿� ���� Service Search ����� �߰��ؾ� �Ѵ�.
		{
			hErr = xmgr_search_SatRetrieveResults (pstContents);
		}

		if (hErr == ERR_OK && pstContents->bNetworkSearch == TRUE)
		{
			xmgr_search_SatAddDeliveryInfoInTpList (pstContents);
		}

		hErr = SVC_SI_StopSearch(pstContents->hAction, TRUE);

		// ���� TP �� �Ѿ�� �Ѵ�.
		hErr = xmgr_search_SatSetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_search_SatTuneSearchTp (pstContents);
		}

		// Tuning Failure :
		if (hErr != ERR_OK)
		{
			LOG ("[xmgr_search_SatEvtSiSearchDone] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	default:
		break;
	}

	if (ePrevAppState != pstContents->eAppState)
	{
		/* TP Progress Update */
		xmgr_search_SatChangeProgressNumber (pstContents);
		xmgr_search_SatSendUpdateDisplayMsg(pstContents->hAction);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_SatSendSearchFinished(pstContents->hAction);
		xmgr_search_SatReleaseAction (pstContents);
	}

	return MESSAGE_BREAK;
}

// eSEVT_SI_SEARCH_FAIL:
STATIC HERROR xmgr_search_SatEvtSiSearchFail (XmgrSearch_SatContents_t *pstContents)
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

		// ���� TP �� �Ѿ�� �Ѵ�.
		hErr = xmgr_search_SatSetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_search_SatTuneSearchTp (pstContents);
		}

		if (hErr != ERR_OK)
		{
			LOG ("[xmgr_search_SatEvtSiSearchFail] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
			break;
		}

		break;

	default:
		break;
	}

	if (ePrevAppState != pstContents->eAppState)
	{
		/* TP Progress Update */
		xmgr_search_SatChangeProgressNumber (pstContents);
		xmgr_search_SatSendUpdateDisplayMsg(pstContents->hAction);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_SatSendSearchFinished(pstContents->hAction);
		xmgr_search_SatReleaseAction (pstContents);
	}

	return MESSAGE_BREAK;
}

// eSEVT_CH_LOCKED
STATIC HERROR xmgr_search_SatEvtChLocked (XmgrSearch_SatContents_t *pstContents)
{
	HUINT32				ulNetId;
	MgrSearch_AppState_e	ePrevAppState;
	HERROR				hErr;
	HUINT8				ucSearchType;

	ePrevAppState = pstContents->eAppState;

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_tryTuning :
		// Channel Locked : Kill the timer

		BUS_KillTimer (SEARCH_TIMER_LOCKING_ID);

		ucSearchType = pstContents->ucSearchType;

		ulNetId = (pstContents->bNetworkSearch == TRUE) ? SvcSi_NETID_SEARCH_NITACT : SvcSi_NETID_SEARCH_NOTNIT;
		{
			SvcSi_SearchSpec_t		stSrchSpec;
			SvcSi_SearchOption_t	stSrchOption;

			HxSTD_memset(&stSrchOption, 0x00, sizeof(SvcSi_SearchOption_t));
			stSrchOption.ulSearchType	= (HUINT32)ucSearchType;
			stSrchOption.ulSvcType		= (HUINT32)pstContents->eServiceType;
			stSrchOption.ulCasType		= (HUINT32)pstContents->eCAFreeType;
			stSrchOption.hSession		= (SvcSi_SessionH_t)HANDLE_NULL;	// not used param
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
			    // MW SI�� �������� ��ٸ��� ���� ���� ���� ���⿡�� Timer�� �ɾ�� �Ѵ�.
			    BUS_SetTimer (SEARCH_TIMER_SI_ID, SEARCH_TIMER_SI_TIME);
		    }
		    else
		    {
			    // ���� TP �� �Ѿ�� �Ѵ�.
			    hErr = xmgr_search_SatSetNextTpToSearch (pstContents);
			    if (hErr == ERR_OK)
			    {
				    hErr = xmgr_search_SatTuneSearchTp (pstContents);
			    }

			    if (hErr != ERR_OK)
			    {
					LOG ("[xmgr_search_SatEvtChLocked] No Next Tp left to tune\n");
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
		/* TP Progress Update */
		xmgr_search_SatChangeProgressNumber (pstContents);
		xmgr_search_SatSendUpdateDisplayMsg(pstContents->hAction);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_SatSendSearchFinished(pstContents->hAction);
		xmgr_search_SatReleaseAction (pstContents);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_search_SatEvtChNoSignal (XmgrSearch_SatContents_t *pstContents)
{
	MgrSearch_AppState_e	 ePrevAppState;
	HERROR				 hErr;

	ePrevAppState = pstContents->eAppState;

	switch (pstContents->eAppState)
	{
	case eSearchApp_state_tryTuning :
		// Channel Locked : Kill the timer

		BUS_KillTimer (SEARCH_TIMER_LOCKING_ID);

		// ���� TP �� �Ѿ�� �Ѵ�.
		hErr = xmgr_search_SatSetNextTpToSearch (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_search_SatTuneSearchTp (pstContents);
		}

		if (hErr != ERR_OK)
		{
			LOG ("[xmgr_search_SatEvtChNoSignal] No Next Tp left to tune\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	case eSearchApp_state_waitingSi:
		// Stop SI :
		hErr = SVC_SI_StopSearch(pstContents->hAction, TRUE);
		if (hErr == ERR_OK)
		{
			// ���� TP �� �Ѿ�� �Ѵ�.
			hErr = xmgr_search_SatSetNextTpToSearch (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_search_SatTuneSearchTp (pstContents);
			}

			if (hErr != ERR_OK)
			{
				LOG ("[xmgr_search_SatEvtChNoSignal] No Next Tp left to tune\n");
				pstContents->eAppState = eSearchApp_state_searchend;
			}
		}
		else
		{
			// �� �̻� ���� �Ұ��� : Search End�� �����Ѵ�.
			LOG ("[xmgr_search_SatEvtChNoSignal] SI Re-open failed.\n");
			pstContents->eAppState = eSearchApp_state_searchend;
		}

		break;

	default:
		break;
	}

	if (ePrevAppState != pstContents->eAppState)
	{
		/* TP Progress Update */
		xmgr_search_SatChangeProgressNumber (pstContents);
		xmgr_search_SatSendUpdateDisplayMsg(pstContents->hAction);
	}

	if (pstContents->eAppState == eSearchApp_state_searchend)
	{
		xmgr_search_SatSendSearchFinished(pstContents->hAction);
		xmgr_search_SatReleaseAction (pstContents);
	}

	return MESSAGE_BREAK;
}

/**
  @brief      Search service application main �Լ� .

  Function Name: AP_SatSrchSvc_Proc()
  Function Description: application proc �Լ�
  Parameter: proc parameters
  type: (Sync or Async) async �Լ�
  Return Value:
*/
STATIC BUS_Result_t			xproc_search_SatProto_Base(XmgrSearch_SatContents_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			 hErr = MESSAGE_PASS;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		MSG_LOG ("[%s] MSG:eMEVT_BUS_CREATE hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatMsgGwmCreate (pstContents, hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_TIMER:
		MSG_LOG ("[%s] MSG:eMEVT_BUS_TIMER hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
//		hErr = xmgr_search_SatMsgGwmTimer(pstContents, p1);
		break;

	case eMEVT_BUS_DESTROY:
		MSG_LOG ("[%s] MSG:eMEVT_BUS_DESTROY hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatMsgGwmDestroyed (pstContents);
		break;

	case eMEVT_SEARCH_ACTION_START:
		MSG_LOG ("[%s] MSG:eMEVT_SEARCH_ACTION_START hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatMsgAppSearchActionStart (pstContents, p1);
		break;

#if 0
	case eMEVT_SEARCH_SAVE_RESULT:
		MSG_LOG ("[%s] MSG:eMEVT_SEARCH_SAVE_RESULT hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatMsgAppSearchSaveResult (pstContents, p1);
		break;
#endif

	case eMEVT_SEARCH_CANCEL:
		MSG_LOG ("[%s] MSG:eMEVT_SEARCH_CANCEL hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatMsgAppSearchCancel (pstContents, p1);
		break;

#if 0
	case eMEVT_ACTION_NOTIFY_BGJOB_FINISHED:
		MSG_LOG ("[%s] MSG:eMEVT_ACTION_NOTIFY_BGJOB_FINISHED hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatMsgAppBgJobFinished (pstContents, p1, p2);
		break;
#endif

#if 0
	case eMEVT_SEARCH_ACTION_KILL:
		MSG_LOG ("[%s] MSG:eMEVT_SEARCH_ACTION_KILL hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatMsgAppSearchKillProcesses (pstContents);
		break;
#endif

	case eSEVT_SI_SEARCH_DONE:
		MSG_LOG ("[%s] MSG:eSEVT_SI_SEARCH_DONE hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatCheckActionId (pstContents, hAction);
		if (hErr != MESSAGE_BREAK)
		{
			hErr = xmgr_search_SatEvtSiSearchDone (pstContents);
		}

		break;

	case eSEVT_SI_SEARCH_FAIL:
		MSG_LOG ("[%s] MSG:eSEVT_SI_SEARCH_FAIL hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatCheckActionId (pstContents, hAction);
		if (hErr != MESSAGE_BREAK)
		{
			hErr = xmgr_search_SatEvtSiSearchFail (pstContents);
		}
		break;

	case eSEVT_CH_STARTED:
		MSG_LOG ("[%s] MSG:eSEVT_CH_STARTED hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatCheckActionId (pstContents, hAction);
		if (hErr != MESSAGE_BREAK)
		{
			hErr = xmgr_search_SatSendUpdateDisplayMsg(pstContents->hAction); // MOONSG_sss
		}
		break;

	case eSEVT_CH_LOCKED:
		MSG_LOG ("[%s] MSG:eSEVT_CH_LOCKED hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatCheckActionId (pstContents, hAction);
		if (hErr != MESSAGE_BREAK)
		{
			hErr = xmgr_search_SatEvtChLocked (pstContents);
		}
		break;

	case eSEVT_CH_NO_SIGNAL:
		MSG_LOG ("[%s] MSG:eSEVT_CH_NO_SIGNAL hAction=0x%08x, p1=%d, p2=%d, p3=%d \n", BUS_MGR_GetMgrName (BUS_MGR_GetThis()), hAction, p1, p2, p3);
		hErr = xmgr_search_SatCheckActionId (pstContents, hAction);
		if (hErr != MESSAGE_BREAK)
		{
			hErr = xmgr_search_SatEvtChNoSignal (pstContents);
		}
		break;

	default:
		hErr = MESSAGE_PASS;
		break;
	}

	MGR_SEARCHRESULT_SetActionState (pstContents->ulActionId, pstContents->eAppState);
	return (hErr == MESSAGE_BREAK) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define	_____EXTERNAL_PROCESS_FUNCTION_____

BUS_Result_t		xproc_search_SatAction0_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), "xproc_search_SatAction0_Base");
#if defined(CONFIG_DEBUG)
		//HLIB_LOG_SetLevel (HxLOG_DOMAIN, (HxLOG_MESSAGE | HxLOG_PRINT | HxLOG_FATAL));

#endif
	}

	return xproc_search_SatProto_Base(&(s_SatSrchApp_Contents[0]), message, hAction, p1, p2, p3);
}

BUS_Result_t		xproc_search_SatAction1_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), "xproc_search_SatAction1_Base");
#if defined(CONFIG_DEBUG)
		//HLIB_LOG_SetLevel (HxLOG_DOMAIN, (HxLOG_MESSAGE | HxLOG_PRINT | HxLOG_FATAL));

#endif
	}

	return xproc_search_SatProto_Base(&(s_SatSrchApp_Contents[1]), message, hAction, p1, p2, p3);
}


/* End of File. ---------------------------------------------------------- */


