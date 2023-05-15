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


#include <mgr_action.h>
#include <bus.h>
#include <svc_pipe.h>
#include <db_common.h>
#include <db_param.h>
#include <db_svc.h>
#include <db_param.h>
#include <linkedlist.h>
#include <svc_resources.h>
#include <svc_ch.h>
// #include <svc_sys.h>

#include <isosvc.h>

#include <mgr_common.h>
#include <mgr_search.h>

#include <svc_resources.h>
#include <mgr_rscmap.h>
#include "../local_include/_xmgr_search.h"

#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define CONDETECT_MASTER						0
#define CONDETECT_SLAVE							1

#define CONDETECT_TIMER_TUNING_ID				0x2001
#define CONDETECT_TIMER_TUNING_TIME				(5 * 1000)			// 5 Sec

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eCntState_Start		 			= 0,
	eCntState_Phase1,					// Master Tuner 로 Tuning 되는 V/H TP를 찾는다
	eCntState_Phase2_Master,			// Slave Tuner에 걸을 TP와 동일한 Polar의 TP를 Master 에 건다.
	eCntState_Phase2_Slave,				// Master 에 걸린 TP와 동일한 Polar의 TP를 Slave에 건다.
	eCntState_Phase3_Master,			// Master 에 Horizontal TP를 건다.
	eCntState_Phase3_Slave,				// Slave 에 Vertical TP 를 건다.

	eCntState_Finish

} XmgrSearch_SatDetectState_e;

typedef enum
{
	eCntPhase1_None					= 0,
	eCntPhase1_Tp1Tuned,
	eCntPhase1_Tp1Certified,
	eCntPhase1_Tp2Tuned,
	eCntPhase1_Tp2Certified

} XmgrSearch_SatDetectP1State_e;

/** @brief search application에서 실행에 필요한 environment context  */
// Connection Detect Action은 Tuner 2개를 동시에 사용해야 하므로 Action을 2개 잡아야 한다.
typedef struct tagXmgrSearch_SatConnectContents_t
{
	HBOOL							 abActionTaken[2];
	HUINT32							 aulActionId[2];				// Search Application Action ID
	Handle_t						 ahAction[2];					// Search Action handle
	HUINT32							 aulTunerId[2];					// Tuner ID

	XmgrSearch_SatDetectState_e		 eState;
	XmgrSearch_SatDetectP1State_e	 ePhase1State;

	POINTER_LIST_T					*pstCheckTsList;

	// Tuning Type :
	// Antenna ID나 Polar의 차이로 Loop through 일 경우에 동시에 걸 수 없는 튜닝 타입
	// 주로 Hor/Ver 로 나뉠 가능성이 크다

	// Tuning Type 1:
	HBOOL					 bType1Tuned;
	HUINT32					 ulType1AntId;
	DxSat_Polarization_e		 eType1Polar;
	DxTuneParam_t			 stType1TuneInfo;

	// Tuning Type 2:
	HBOOL					 bType2Tuned;
	DxTuneParam_t			 stType2TuneInfo;

	// Master/Slave Tuner Received
	HBOOL					 bMasterTuned, bSlaveTuned;

	DxAntennaConnection_e			 eConnType;
} XmgrSearch_SatConnectContents_t;


STATIC XmgrSearch_SatConnectContents_t		*s_pstSatCntAct_Contents = NULL;


STATIC XmgrSearch_SatConnectContents_t*	xmgr_search_SatConnectGetContents (void);
STATIC void						xmgr_search_SatConnectFreeContents (void);
STATIC INLINE HERROR			xmgr_search_SatConnectCheckActionId (XmgrSearch_SatConnectContents_t *pstContents, Handle_t hAction);
STATIC INLINE HERROR			xmgr_search_SatConnectGetTuneIndex (XmgrSearch_SatConnectContents_t *pstContents, Handle_t hAction, HUINT32 *pulTuneIndex);
STATIC INLINE void				xmgr_search_SatConnectIncreaseSessionVersion (XmgrSearch_SatConnectContents_t *pstContents, HUINT32 ulTuneIndex);
STATIC HERROR	xmgr_search_SatConnectsetAntennaConnectionType (DxAntennaConnection_e eAntConnType);

STATIC HERROR	xmgr_search_SatConnectInitContents (XmgrSearch_SatConnectContents_t *pstContents);

STATIC HERROR	xmgr_search_SatConnectDestroyContents (XmgrSearch_SatConnectContents_t *pstContents);

STATIC HERROR	xmgr_search_SatConnectGetTunerId (HUINT32 ulCount, HUINT32 *pulTunerId);
STATIC HERROR	xmgr_search_SatConnectTakeAction (HUINT32 ulActionId, HUINT32 ulTunerId);
STATIC HERROR	xmgr_search_SatConnectSetCheckTpList (XmgrSearch_SatConnectContents_t *pstContents, HINT32 nAppHandle);
STATIC HERROR	xmgr_search_SatConnectSetNextTp (XmgrSearch_SatConnectContents_t *pstContents);
STATIC HERROR	xmgr_search_SatConnectStopAllTuner (XmgrSearch_SatConnectContents_t *pstContents);
STATIC HERROR	xmgr_search_SatConnectTuneCheckTsList (XmgrSearch_SatConnectContents_t *pstContents);
STATIC HERROR	xmgr_search_SatConnectClearCheckList (XmgrSearch_SatConnectContents_t *pstContents);
STATIC void		xmgr_search_SatConnectFreeTsInfo (void *pvItem);
STATIC HERROR	xmgr_search_SatConnectProcessPhase1 (XmgrSearch_SatConnectContents_t *pstContents);
STATIC HERROR	xmgr_search_SatConnectProcessPhase2Master (XmgrSearch_SatConnectContents_t *pstContents, HUINT32 ulTuneIndex);
STATIC HERROR	xmgr_search_SatConnectProcessPhase2Slave (XmgrSearch_SatConnectContents_t *pstContents);
STATIC HERROR	xmgr_search_SatConnectProcessPhase3Master (XmgrSearch_SatConnectContents_t *pstContents, HUINT32 ulTuneIndex);
STATIC HERROR	xmgr_search_SatConnectProcessPhase3Slave  (XmgrSearch_SatConnectContents_t *pstContents);
STATIC HERROR	xmgr_search_SatConnectMsgGwmCreate (XmgrSearch_SatConnectContents_t *pstContents, HINT32 nAppHandle);
STATIC HERROR	xmgr_search_SatConnectMsgGwmDestroyed (XmgrSearch_SatConnectContents_t *pstContents);
STATIC HERROR	xmgr_search_SatConnectMsgGwmTimer (XmgrSearch_SatConnectContents_t *pstContents, HINT32 nTimerId);
STATIC HERROR	xmgr_search_SatConnectEvtChLocked (XmgrSearch_SatConnectContents_t *pstContents, Handle_t hAction);
STATIC HERROR	xmgr_search_SatConnectEvtChNoSignal (XmgrSearch_SatConnectContents_t *pstContents, Handle_t hAction);


#define _________________________________________________________________________________

STATIC XmgrSearch_SatConnectContents_t *xmgr_search_SatConnectGetContents (void)
{
	if (s_pstSatCntAct_Contents == NULL)
	{
		s_pstSatCntAct_Contents = (XmgrSearch_SatConnectContents_t *)OxAP_Malloc (sizeof(XmgrSearch_SatConnectContents_t));
		if (s_pstSatCntAct_Contents != NULL)
		{
			HxSTD_memset (s_pstSatCntAct_Contents, 0, sizeof(XmgrSearch_SatConnectContents_t));
		}
	}

	return s_pstSatCntAct_Contents;
}

STATIC void xmgr_search_SatConnectFreeContents (void)
{
	if (s_pstSatCntAct_Contents != NULL)
	{
		OxAP_Free (s_pstSatCntAct_Contents);
		s_pstSatCntAct_Contents = NULL;
	}
}

STATIC INLINE HERROR xmgr_search_SatConnectCheckActionId (XmgrSearch_SatConnectContents_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulTuneIndex;
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);

	for (ulTuneIndex = 0; ulTuneIndex < 2; ulTuneIndex ++)
	{
		if (pstContents->aulActionId[ulTuneIndex] == ulActionId)
		{
			return ERR_BUS_NO_ERROR;
		}
	}

	HxLOG_Info("is not matched actionID(0x%x)! line(%d)\n", ulActionId, __LINE__);
	return MESSAGE_PASS;
}

STATIC INLINE HERROR xmgr_search_SatConnectGetTuneIndex (XmgrSearch_SatConnectContents_t *pstContents, Handle_t hAction, HUINT32 *pulTuneIndex)
{
	HUINT32			 ulTuneIndex;
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);

	for (ulTuneIndex = 0; ulTuneIndex < 2; ulTuneIndex ++)
	{
		if (pstContents->aulActionId[ulTuneIndex] == ulActionId)
		{
			*pulTuneIndex = ulTuneIndex;
			return ERR_BUS_NO_ERROR;
		}
	}

	HxLOG_Info("is not matched actionID(0x%x)! line(%d)\n", ulActionId, __LINE__);
	return MESSAGE_PASS;
}

STATIC INLINE void xmgr_search_SatConnectIncreaseSessionVersion (XmgrSearch_SatConnectContents_t *pstContents, HUINT32 ulTuneIndex)
{
	pstContents->ahAction[ulTuneIndex] = SVC_PIPE_IncreaseActionVersion (pstContents->aulActionId[ulTuneIndex]);
}

STATIC HERROR xmgr_search_SatConnectsetAntennaConnectionType (DxAntennaConnection_e eAntConnType)
{
	HERROR		hErr;
//	MWC_PARAM_SetAntennaConnectionType (eAntConnType);
	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32)eAntConnType, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_SetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE) Error!!!\n");
	}

	// 여기서 MWC_PARAM_SetAntennaConnectionType을 해 주기 때문에 그에 해당하는 Resource 코드가 필요해서 넣었다.
	{
		switch (eAntConnType)
		{
		case eDxANTCONN_TWOSAME:
			SVC_PIPE_SetRfInputAbility (0, TRUE);
			SVC_PIPE_SetRfInputAbility (1, TRUE);
			break;

		case eDxANTCONN_ONECABLE:
		default:
			SVC_PIPE_SetRfInputAbility (0, TRUE);
			SVC_PIPE_SetRfInputAbility (1, FALSE);
			break;
		}
	}
	return ERR_OK;
}


#define	_____CONTENTS_INITIATION_____

STATIC HERROR xmgr_search_SatConnectInitContents (XmgrSearch_SatConnectContents_t *pstContents)
{
	HUINT32				 ulActionCnt;
	HUINT32		 ulTunerId;
	HERROR				 hErr;

	xmgr_search_SatConnectsetAntennaConnectionType (eDxANTCONN_TWOSAME);

#if 0 // Motor SCD  는 추후 고려 한다.
	// test code.
	DB_SVC_GetAntCount(&count);
	HxLOG_Debug("xmgr_search_SatConnectInitContents count = %d\n", count);

	if(DB_SVC_GetAntInfo(1, &stAntInfo) == ERR_FAIL)
	{
		return ERR_FAIL;
	}
	HxLOG_Debug("stAntInfo name = %s\n", stAntInfo.szName);

	ulAntType = stAntInfo.eAntennaType;

	if (ulAntType == eDxANT_TYPE_SCD)
	{
		xmgr_search_SatConnectsetAntennaConnectionType (eDxANTCONN_TWOSAME);

		return MESSAGE_BREAK;
	}
	else if (ulAntType == eUiAntType_Motor)
	{
		xmgr_search_SatConnectsetAntennaConnectionType (eDxANTCONN_ONECABLE);
		return MESSAGE_BREAK;
	}
#endif

	// 1. Clear all the contents:
	HxSTD_memset (pstContents, 0, sizeof(XmgrSearch_SatConnectContents_t));

	pstContents->eState = eCntState_Start;
	pstContents->ePhase1State = eCntPhase1_None;

	// 2. Action Handle and Action ID
	pstContents->aulActionId[CONDETECT_MASTER]	= MGR_ACTION_GetMainActionId();
	pstContents->ahAction[CONDETECT_MASTER]		= SVC_PIPE_GetActionHandle (pstContents->aulActionId[CONDETECT_MASTER]);

	pstContents->aulActionId[CONDETECT_SLAVE]	= MGR_ACTION_GetSubActionId();
	pstContents->ahAction[CONDETECT_SLAVE]		= SVC_PIPE_GetActionHandle (pstContents->aulActionId[CONDETECT_SLAVE]);

	// 3. Take the Actions
	for (ulActionCnt = 0; ulActionCnt < 2; ulActionCnt++)
	{
		hErr = xmgr_search_SatConnectGetTunerId (ulActionCnt, &ulTunerId);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("xmgr_search_SatConnectGetTunerId fail in action id %d\n", pstContents->aulActionId[ulActionCnt]);
			return ERR_FAIL;
		}

		HxLOG_Error(" -> Action %d Set the Tuner %d\n", pstContents->aulActionId[ulActionCnt], ulTunerId);

		hErr = xmgr_search_SatConnectTakeAction (pstContents->aulActionId[ulActionCnt], (HUINT32)ulTunerId);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("xmgr_search_SatConnectTakeAction fail in action id %d\n", pstContents->aulActionId[ulActionCnt]);
			return ERR_FAIL;
		}

		pstContents->abActionTaken[ulActionCnt] = TRUE;

		hErr = SVC_PIPE_GetResourceId (pstContents->aulActionId[ulActionCnt], eRxRSCTYPE_TUNER_GROUP, &(pstContents->aulTunerId[ulActionCnt]));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("SVC_PIPE_GetResourceId fail in action id %d\n", pstContents->aulActionId[ulActionCnt]);
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}


#define	_____CONTENTS_MODIFICATION_____



#define	_____CONTENTS_DESTRUCTION_____

STATIC HERROR xmgr_search_SatConnectDestroyContents (XmgrSearch_SatConnectContents_t *pstContents)
{
	xmgr_search_SatConnectClearCheckList (pstContents);

	return ERR_BUS_NO_ERROR;
}


#define	_____DATA_CONVERSION_____



#define	_____ACTION_FUNCTIONS_____

STATIC HERROR xmgr_search_SatConnectGetTunerId (HUINT32 ulCount, HUINT32 *pulTunerId)
{
	if (pulTunerId == NULL)					{ return ERR_BUS_TARGET_NULL; }

	switch (ulCount)
	{
	case CONDETECT_MASTER:
		*pulTunerId = 0;
		break;

	case CONDETECT_SLAVE:
		*pulTunerId = 1;
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_search_SatConnectTakeAction (HUINT32 ulActionId, HUINT32 ulTunerId)
{
	DxTuneParam_t		 stTuneInfo;
	HERROR				 hErr;

	HxSTD_memset (&stTuneInfo, 0, sizeof(DxTuneParam_t));

	stTuneInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
	stTuneInfo.sat.ulTunerGroupId	= ulTunerId;
	stTuneInfo.sat.ulRfInputId		= ulTunerId;

	hErr = MGR_ACTION_SetupSearch (ulActionId, &stTuneInfo, eAmPriority_SEARCH);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_Take (ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}




#define	_____MESSAGE_FUNCTIONS_____



#define	_____PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_search_SatConnectSetCheckTpList (XmgrSearch_SatConnectContents_t *pstContents, HINT32 nAppHandle)
{
	HINT32				 nTsCnt, nTsNum = 0;
	DbSvc_TsInfo_t			*pstTsInfo;
	MgrSearch_Option_t 		*pstOption = (MgrSearch_Option_t *)nAppHandle;

	nTsNum = pstOption->ulTpNum;

	for (nTsCnt = 0; nTsCnt < nTsNum; nTsCnt++)
	{
		pstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
		if (pstTsInfo != NULL)
		{
			HxLOG_Info(">>> TP (Freq:%d, SR:%d) AntId = %d ======================\n"
			, pstOption->pstTpArray[nTsCnt].stTuningParam.sat.ulFrequency
			, pstOption->pstTpArray[nTsCnt].stTuningParam.sat.ulSymbolRate
			, pstOption->pstTpArray[nTsCnt].stTuningParam.sat.ucAntId);

			HxSTD_memcpy(&(pstTsInfo->stTuningParam), &(pstOption->pstTpArray[nTsCnt].stTuningParam), sizeof(DbSvc_TuningInfo_t));
			pstTsInfo->ucTunerId = (HUINT8)pstContents->aulTunerId[CONDETECT_MASTER];
			pstTsInfo->eDeliType = eDxDELIVERY_TYPE_SAT;

			pstContents->pstCheckTsList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstCheckTsList, (void *)pstTsInfo);
		}
	}

	return (pstContents->pstCheckTsList != NULL) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR xmgr_search_SatConnectSetNextTp (XmgrSearch_SatConnectContents_t *pstContents)
{
	DbSvc_TsInfo_t		*pstTsInfo;

	if (pstContents->pstCheckTsList == NULL)
	{
		return ERR_FAIL;
	}

	// 일단 지금 것은 없애고 본다.
	pstContents->pstCheckTsList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstCheckTsList, pstContents->pstCheckTsList, xmgr_search_SatConnectFreeTsInfo);

	// Type 1이 이미 Tuning 되었다면 Type2는 Type1과 Antenna, Polar가 다른 걸로 선택되어야 한다.
	if (pstContents->bType1Tuned == TRUE)
	{
		while (pstContents->pstCheckTsList != NULL)
		{
			pstTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstContents->pstCheckTsList);
			if (pstTsInfo != NULL)
			{
				if ((pstTsInfo->stTuningParam.sat.ucAntId != (HUINT8)pstContents->ulType1AntId) ||
					(pstTsInfo->stTuningParam.sat.ePolarization != pstContents->eType1Polar))
				{
					break;
				}
			}

			pstContents->pstCheckTsList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstCheckTsList, pstContents->pstCheckTsList, xmgr_search_SatConnectFreeTsInfo);
		}
	}

	if (pstContents->pstCheckTsList == NULL)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_search_SatConnectStopAllTuner (XmgrSearch_SatConnectContents_t *pstContents)
{
	HERROR		 hErr;

	HxLOG_Info(">>> xmgr_search_SatConnectStopAllTuner = %d ======================\n", pstContents->eState);

	hErr  = SVC_CH_StopTune (pstContents->ahAction[CONDETECT_MASTER], (HUINT16)-1, eSyncMode);
	hErr |= SVC_CH_StopTune (pstContents->ahAction[CONDETECT_SLAVE], (HUINT16)-1, eSyncMode);

	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_search_SatConnectTuneCheckTsList (XmgrSearch_SatConnectContents_t *pstContents)
{
	DbSvc_TsInfo_t	*pstTsInfo;
	SvcRsc_Info_t		 stRscInfo;
	DxTuneParam_t	 stTuneInfo;
	HERROR			 hErr;

	hErr = ERR_FAIL;

	HxLOG_Info(">>> pstContents->eState = %d ======================\n", pstContents->eState);

	switch (pstContents->eState)
	{
	case eCntState_Phase1:
		while (pstContents->pstCheckTsList != NULL)
		{
			MGR_RSC_GetResourceInfo (pstContents->aulActionId[CONDETECT_MASTER], &stRscInfo);

			HxLOG_Info("\n\n");
			pstTsInfo = (DbSvc_TsInfo_t *)pstContents->pstCheckTsList->pvContents;
			if (pstTsInfo == NULL)
			{
				pstContents->pstCheckTsList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstCheckTsList, pstContents->pstCheckTsList, xmgr_search_SatConnectFreeTsInfo);
				continue;
			}

			HxLOG_Info("\n\n");
			hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)pstTsInfo, &stTuneInfo);
			if (hErr != ERR_OK)
			{
				pstContents->pstCheckTsList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstCheckTsList, pstContents->pstCheckTsList, xmgr_search_SatConnectFreeTsInfo);
				continue;
			}

			HxLOG_Info("\n\n");
			hErr = SVC_CH_StopTune (pstContents->ahAction[CONDETECT_MASTER], (HUINT16)-1, eSyncMode);
			if (hErr != ERR_OK)
			{
				break;
			}

			HxLOG_Info("\n\n");
			xmgr_search_SatConnectIncreaseSessionVersion (pstContents, CONDETECT_MASTER);
			HxLOG_Info("\n\n");
			hErr = SVC_CH_StartTune (pstContents->ahAction[CONDETECT_MASTER], &stRscInfo, (HUINT16)-1, &stTuneInfo);
			if (hErr != ERR_OK)
			{
				pstContents->pstCheckTsList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstCheckTsList, pstContents->pstCheckTsList, xmgr_search_SatConnectFreeTsInfo);
				continue;
			}
			HxLOG_Info("\n\n");

			hErr = ERR_OK;
			break;
		}

		break;

	case eCntState_Phase2_Master:
		if (pstContents->bType1Tuned != TRUE)
		{
			stTuneInfo = pstContents->stType1TuneInfo;
		}
		else if (pstContents->bType2Tuned != TRUE)
		{
			stTuneInfo = pstContents->stType2TuneInfo;
		}
		else
		{
			return ERR_FAIL;
		}

		// Master Tuner
		MGR_RSC_GetResourceInfo (pstContents->aulActionId[CONDETECT_MASTER], &stRscInfo);
		stTuneInfo.sat.ulTunerGroupId = (HUINT32)pstContents->aulTunerId[CONDETECT_MASTER];

		hErr = SVC_CH_StopTune (pstContents->ahAction[CONDETECT_MASTER], (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		xmgr_search_SatConnectIncreaseSessionVersion (pstContents, CONDETECT_MASTER);
		hErr = SVC_CH_StartTune (pstContents->ahAction[CONDETECT_MASTER], &stRscInfo, (HUINT16)-1, &stTuneInfo);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		// Slave Tuner
		MGR_RSC_GetResourceInfo (pstContents->aulActionId[CONDETECT_SLAVE], &stRscInfo);
		stTuneInfo.sat.ulTunerGroupId = (HUINT32)pstContents->aulTunerId[CONDETECT_SLAVE];

		hErr = SVC_CH_StopTune (pstContents->ahAction[CONDETECT_SLAVE], (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		xmgr_search_SatConnectIncreaseSessionVersion (pstContents, CONDETECT_SLAVE);
		hErr = SVC_CH_StartTune (pstContents->ahAction[CONDETECT_SLAVE], &stRscInfo, (HUINT16)-1, &stTuneInfo);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		hErr = ERR_OK;
		break;

	case eCntState_Phase2_Slave:
		if (pstContents->bType1Tuned != TRUE)
		{
			stTuneInfo = pstContents->stType1TuneInfo;
		}
		else if (pstContents->bType2Tuned != TRUE)
		{
			stTuneInfo = pstContents->stType2TuneInfo;
		}
		else
		{
			return ERR_FAIL;
		}

		MGR_RSC_GetResourceInfo (pstContents->aulActionId[CONDETECT_SLAVE], &stRscInfo);
		stTuneInfo.sat.ulTunerGroupId = (HUINT32)pstContents->aulTunerId[CONDETECT_SLAVE];

		hErr = SVC_CH_StopTune (pstContents->ahAction[CONDETECT_SLAVE], (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		xmgr_search_SatConnectIncreaseSessionVersion (pstContents, CONDETECT_SLAVE);
		hErr = SVC_CH_StartTune (pstContents->ahAction[CONDETECT_SLAVE], &stRscInfo, (HUINT16)-1, &stTuneInfo);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		hErr = ERR_OK;
		break;

	case eCntState_Phase3_Master:
		// Master Tune
		MGR_RSC_GetResourceInfo (pstContents->aulActionId[CONDETECT_MASTER], &stRscInfo);

		hErr = SVC_CH_StopTune (pstContents->ahAction[CONDETECT_MASTER], (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		stTuneInfo = pstContents->stType1TuneInfo;
		stTuneInfo.sat.ulTunerGroupId = (HUINT32)pstContents->aulTunerId[CONDETECT_MASTER];

		xmgr_search_SatConnectIncreaseSessionVersion (pstContents, CONDETECT_MASTER);
		hErr = SVC_CH_StartTune (pstContents->ahAction[CONDETECT_MASTER], &stRscInfo, (HUINT16)-1, &stTuneInfo);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		// DD 버그로 인해 Tuner 0/1을 동시에 걸 경우 제대로 Tuning 정보가 오지 않는다.
		// 이에 Sleep을 두어서 제대로 된 결과를 기대한다.
		// DD 수정 후 이 항목을 반드시 지워야 한다.
		// VK_TASK_Sleep (500);

		// Slave Tune
		MGR_RSC_GetResourceInfo (pstContents->aulActionId[CONDETECT_SLAVE], &stRscInfo);
		hErr = SVC_CH_StopTune (pstContents->ahAction[CONDETECT_SLAVE], (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		stTuneInfo = pstContents->stType2TuneInfo;
		stTuneInfo.sat.ulTunerGroupId = (HUINT32)pstContents->aulTunerId[CONDETECT_SLAVE];

		xmgr_search_SatConnectIncreaseSessionVersion (pstContents, CONDETECT_SLAVE);
		hErr = SVC_CH_StartTune (pstContents->ahAction[CONDETECT_SLAVE], &stRscInfo, (HUINT16)-1, &stTuneInfo);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		hErr = ERR_OK;
		break;

	case eCntState_Phase3_Slave:
		// Slave 에 Type 2 을 건다.
		MGR_RSC_GetResourceInfo (pstContents->aulActionId[CONDETECT_SLAVE], &stRscInfo);

		hErr = SVC_CH_StopTune (pstContents->ahAction[CONDETECT_SLAVE], (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		stTuneInfo = pstContents->stType2TuneInfo;
		stTuneInfo.sat.ulTunerGroupId = (HUINT32)pstContents->aulTunerId[CONDETECT_SLAVE];

		xmgr_search_SatConnectIncreaseSessionVersion (pstContents, CONDETECT_SLAVE);
		hErr = SVC_CH_StartTune (pstContents->ahAction[CONDETECT_SLAVE], &stRscInfo, (HUINT16)-1, &stTuneInfo);
		if (hErr != ERR_OK)
		{
			return ERR_FAIL;
		}

		hErr = ERR_OK;
		break;

	default:
		return ERR_FAIL;
	}

	if (hErr == ERR_OK)				// Tuning Success :
	{
		BUS_SetTimer (CONDETECT_TIMER_TUNING_ID, CONDETECT_TIMER_TUNING_TIME);
	}

	return hErr;
}


STATIC HERROR xmgr_search_SatConnectClearCheckList (XmgrSearch_SatConnectContents_t *pstContents)
{
	if (pstContents->pstCheckTsList != NULL)
	{
		pstContents->pstCheckTsList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstCheckTsList, xmgr_search_SatConnectFreeTsInfo);
	}

	return ERR_OK;
}

STATIC void xmgr_search_SatConnectFreeTsInfo (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

STATIC HERROR xmgr_search_SatConnectProcessPhase1 (XmgrSearch_SatConnectContents_t *pstContents)
{
	HUINT32			 ulAntId;
	DxTuneParam_t	 stTuneInfo;
	SvcRsc_Info_t		 stRscInfo;
	DbSvc_TsInfo_t	*pstCurrTsInfo;
	HERROR			 hErr;

	// Get the current TP
	hErr = SVC_CH_GetLockedInfo (pstContents->aulActionId[CONDETECT_MASTER], &stTuneInfo);
	if (hErr == ERR_OK)
	{
		switch (pstContents->ePhase1State)
		{
		case eCntPhase1_None:
			// 반대 Polar로도 Tuning 되는지 검증이 필요하다 : V/H로 다 튜닝되는 TP는 Connect Detection 용으로 쓸 수 없다.
			ulAntId = 0;
			if (pstContents->pstCheckTsList != NULL)
			{
				pstCurrTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstContents->pstCheckTsList);
				if (pstCurrTsInfo != NULL)
				{
					ulAntId = (HUINT32)pstCurrTsInfo->stTuningParam.sat.ucAntId;
				}
			}

			pstContents->ulType1AntId	 = ulAntId;
			pstContents->eType1Polar	 = stTuneInfo.sat.tuningInfo.ePolarization;
			pstContents->stType1TuneInfo = stTuneInfo;

			MGR_RSC_GetResourceInfo (pstContents->aulActionId[CONDETECT_MASTER], &stRscInfo);
			stTuneInfo.sat.tuningInfo.ePolarization = (stTuneInfo.sat.tuningInfo.ePolarization == eDxSAT_POLAR_HOR) ? eDxSAT_POLAR_VER : eDxSAT_POLAR_HOR;

			hErr = SVC_CH_StopTune (pstContents->ahAction[CONDETECT_MASTER], (HUINT16)-1, eSyncMode);
			if (hErr != ERR_OK)					{ break; }

			xmgr_search_SatConnectIncreaseSessionVersion (pstContents, CONDETECT_MASTER);
			hErr = SVC_CH_StartTune (pstContents->ahAction[CONDETECT_MASTER], &stRscInfo, (HUINT16)-1, &stTuneInfo);
			if (hErr != ERR_OK)					{ break; }

			pstContents->ePhase1State = eCntPhase1_Tp1Tuned;
			return ERR_OK;

		case eCntPhase1_Tp1Tuned:
			// 반대 Polar로 Tune 해서 LOCK되면 잘못된 TP이다.
			pstContents->ePhase1State = eCntPhase1_None;
			break;

		case eCntPhase1_Tp1Certified:
			// TP 2도 반대 Polar를 Test 해야 한다.
			MGR_RSC_GetResourceInfo (pstContents->aulActionId[CONDETECT_MASTER], &stRscInfo);

			pstContents->stType2TuneInfo = stTuneInfo;

			stTuneInfo.sat.tuningInfo.ePolarization = (stTuneInfo.sat.tuningInfo.ePolarization == eDxSAT_POLAR_HOR) ? eDxSAT_POLAR_VER : eDxSAT_POLAR_HOR;

			hErr = SVC_CH_StopTune (pstContents->ahAction[CONDETECT_MASTER], (HUINT16)-1, eSyncMode);
			if (hErr != ERR_OK)					{ break; }

			xmgr_search_SatConnectIncreaseSessionVersion (pstContents, CONDETECT_MASTER);
			hErr = SVC_CH_StartTune (pstContents->ahAction[CONDETECT_MASTER], &stRscInfo, (HUINT16)-1, &stTuneInfo);
			if (hErr != ERR_OK)					{ break; }

			pstContents->ePhase1State = eCntPhase1_Tp2Tuned;
			return ERR_OK;

		case eCntPhase1_Tp2Tuned:
			// 반대 Polar로 Tune 해서 LOCK되면 잘못된 TP이다.
			pstContents->ePhase1State = eCntPhase1_Tp1Certified;
			break;

		default:
			break;
		}
	}

	// Horizontal / Vertical 이 아직일 경우 :
	hErr = xmgr_search_SatConnectSetNextTp (pstContents);
	if (hErr != ERR_OK)
	{
		return MESSAGE_BREAK;
	}

	hErr = xmgr_search_SatConnectTuneCheckTsList (pstContents);
	if (hErr != ERR_OK)
	{
		return MESSAGE_BREAK;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_search_SatConnectProcessPhase2Master (XmgrSearch_SatConnectContents_t *pstContents, HUINT32 ulTuneIndex)
{
	HERROR			 hErr;

	// Type 1 TP Tuning 인가?
	if (pstContents->bType1Tuned != TRUE)
	{
		if (ulTuneIndex == CONDETECT_MASTER)
		{
			pstContents->bMasterTuned = TRUE;
		}
		else if (ulTuneIndex == CONDETECT_SLAVE)
		{
			pstContents->bSlaveTuned = TRUE;
		}

		if (pstContents->bMasterTuned == TRUE && pstContents->bSlaveTuned == TRUE)
		{
			// Slave에 VER TP를 걸도록 한다.
			HxLOG_Info("[ConnectDetect] 2nd Phase : Slave Tuned VER TP. Next phase.\n");

			// 여기서 Tuner Reset 할 필요가 있다
			xmgr_search_SatConnectStopAllTuner (pstContents);

			pstContents->eState			= eCntState_Phase3_Master;
			pstContents->bType1Tuned	= FALSE;
			pstContents->bType2Tuned	= FALSE;
			pstContents->bMasterTuned	= FALSE;
			pstContents->bSlaveTuned	= FALSE;

			hErr = xmgr_search_SatConnectTuneCheckTsList (pstContents);
			if (hErr != ERR_OK)
			{
				return MESSAGE_BREAK;
			}
		}

		return ERR_OK;
	}
	//  Type 2 TP Tuning 인가?
	else if (pstContents->bType2Tuned != TRUE)
	{
		// Slave에 VER TP를 걸도록 한다.
		HxLOG_Info("[ConnectDetect] 2nd Phase : Master Tuned VER TP. Tune the slave.\n");
		pstContents->eState = eCntState_Phase2_Slave;
		hErr = xmgr_search_SatConnectTuneCheckTsList (pstContents);
		if (hErr != ERR_OK)
		{
			return MESSAGE_BREAK;
		}

		return ERR_OK;
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_search_SatConnectProcessPhase2Slave (XmgrSearch_SatConnectContents_t *pstContents)
{
	HERROR			 hErr;

	// Horizontal TP Tuning 인가?
	if (pstContents->bType1Tuned != TRUE)
	{
		// Slave에 HOR TP를 걸도록 한다.
		HxLOG_Info("[ConnectDetect] 2nd Phase : Slave Tuned HOR TP. Test VER TP.\n");

		// 여기서 Tuner Reset 할 필요가 있다
		xmgr_search_SatConnectStopAllTuner (pstContents);

		pstContents->bType1Tuned = TRUE;
		pstContents->eState = eCntState_Phase2_Master;
		hErr = xmgr_search_SatConnectTuneCheckTsList (pstContents);
		if (hErr != ERR_OK)
		{
			return MESSAGE_BREAK;
		}

		return ERR_OK;
	}
	// Vertical TP Tuning 인가?
	else if (pstContents->bType2Tuned != TRUE)
	{
		// Slave에 VER TP를 걸도록 한다.
		HxLOG_Info("[ConnectDetect] 2nd Phase : Slave Tuned VER TP. Next phase.\n");

		// 여기서 Tuner Reset 할 필요가 있다
		xmgr_search_SatConnectStopAllTuner (pstContents);

		pstContents->bType1Tuned = FALSE;
		pstContents->bType2Tuned = FALSE;
		pstContents->eState = eCntState_Phase3_Master;
		hErr = xmgr_search_SatConnectTuneCheckTsList (pstContents);
		if (hErr != ERR_OK)
		{
			return MESSAGE_BREAK;
		}

		return ERR_OK;
	}

	return MESSAGE_BREAK;
}


STATIC HERROR xmgr_search_SatConnectProcessPhase3Master  (XmgrSearch_SatConnectContents_t *pstContents, HUINT32 ulTuneIndex)
{
#if defined(CONFIG_DEBUG)
	HCHAR			*szPolStr;
#endif

	// Type 1 TP Tuning 인가?
	if (pstContents->bType1Tuned != TRUE)
	{
		if (ulTuneIndex == CONDETECT_MASTER)
		{
#if defined(CONFIG_DEBUG)
			szPolStr = (pstContents->stType1TuneInfo.sat.tuningInfo.ePolarization == eDxSAT_POLAR_HOR) ? "HOR" : "VER";
			HxLOG_Info("[ConnectDetect] 3rd Phase : Master Tuned %s TP\n", szPolStr);
#endif
			pstContents->bMasterTuned = TRUE;
		}
		else if (ulTuneIndex == CONDETECT_SLAVE)
		{
#if defined(CONFIG_DEBUG)
			szPolStr = (pstContents->stType2TuneInfo.sat.tuningInfo.ePolarization == eDxSAT_POLAR_HOR) ? "HOR" : "VER";
			HxLOG_Info("[ConnectDetect] 3rd Phase : Slave Tuned %s TP\n", szPolStr);
#endif
			pstContents->bSlaveTuned = TRUE;
		}

		if (pstContents->bMasterTuned == TRUE && pstContents->bSlaveTuned == TRUE)
		{
			pstContents->eState = eCntState_Finish;
			pstContents->eConnType = eDxANTCONN_TWOSAME;
			xmgr_search_SatConnectsetAntennaConnectionType (eDxANTCONN_TWOSAME);

			return MESSAGE_BREAK;
		}

		return ERR_OK;
	}

	return ERR_OK;
}

STATIC HERROR xmgr_search_SatConnectProcessPhase3Slave  (XmgrSearch_SatConnectContents_t *pstContents)
{
	(void)pstContents;

	HxLOG_Info("[ConnectDetect] 3rd Phase : Slave Tuned VER TP\n");

	pstContents->eState = eCntState_Finish;
	pstContents->eConnType = eDxANTCONN_TWOSAME;
	xmgr_search_SatConnectsetAntennaConnectionType (eDxANTCONN_TWOSAME);

	return MESSAGE_BREAK;
}


#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____


//eMEVT_BUS_CREATE
STATIC HERROR xmgr_search_SatConnectMsgGwmCreate (XmgrSearch_SatConnectContents_t *pstContents, HINT32 nAppHandle)
{
	HERROR		 hErr;


	BUS_MGR_SetMgrName (BUS_MGR_GetThis(), "xproc_search_SatConnectionTypeDetectAction");

	// 1. Initialize the contents
	hErr = xmgr_search_SatConnectInitContents (pstContents);
	if (hErr != ERR_OK)
	{
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

	// 2. Tuning test : 1st Tuner
	pstContents->eState = eCntState_Phase1;
	pstContents->ePhase1State = eCntPhase1_None;

	hErr = xmgr_search_SatConnectSetCheckTpList (pstContents, nAppHandle);

	HxLOG_Info("\n\n");

	if (hErr != ERR_OK)
	{
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

	HxLOG_Info("\n\n");
	xmgr_search_SatConnectStopAllTuner (pstContents);

	HxLOG_Info("\n\n");
	hErr = xmgr_search_SatConnectTuneCheckTsList (pstContents);
	HxLOG_Info("\n\n");
	if (hErr != ERR_OK)
	{
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}
	HxLOG_Info("\n\n");

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY:
STATIC HERROR xmgr_search_SatConnectMsgGwmDestroyed (XmgrSearch_SatConnectContents_t *pstContents)
{
	HUINT32		 ulActionCnt;

	xmgr_search_SatConnectStopAllTuner (pstContents);
	HxLOG_Info("\n\n");

	// Action을 잡은 것을 놓아야 한다.
	for (ulActionCnt = 0; ulActionCnt < 2; ulActionCnt++)
	{
		if (pstContents->abActionTaken[ulActionCnt] == TRUE)
		{
			MGR_ACTION_Release (pstContents->aulActionId[ulActionCnt], eActionType_SEARCH);
		}
	}

	BUS_PostMessage (NULL, eMEVT_SEARCH_ANT_CONNECT_TYPE_DETECTED, (Handle_t)MGR_ACTION_GetMainActionId(), (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);

	// Destroy the contents
	xmgr_search_SatConnectDestroyContents (pstContents);
	xmgr_search_SatConnectFreeContents();

	return MESSAGE_BREAK;
}

// eMEVT_BUS_TIMER:
STATIC HERROR xmgr_search_SatConnectMsgGwmTimer (XmgrSearch_SatConnectContents_t *pstContents, HINT32 nTimerId)
{
	switch (nTimerId)
	{
	case CONDETECT_TIMER_TUNING_ID:
		// Timeout :
		break;

	default:
		break;
	}

	return ERR_BUS_NO_ERROR;
}

// eSEVT_CH_LOCKED:
STATIC HERROR xmgr_search_SatConnectEvtChLocked (XmgrSearch_SatConnectContents_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulTuneIndex;
	HERROR			 hErr;

	hErr = xmgr_search_SatConnectGetTuneIndex (pstContents, hAction, &ulTuneIndex);
	if (hErr == ERR_OK)
	{
		switch (pstContents->eState)
		{
		case eCntState_Phase1:
			// 이 단계에서 Master 이외의 곳에서 Tuning Message가 올 수 없다 : Action Stop
			if (ulTuneIndex != CONDETECT_MASTER)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			hErr = xmgr_search_SatConnectProcessPhase1 (pstContents);
			if (hErr != ERR_OK)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			break;

		case eCntState_Phase2_Master:
			#if 0
			// 이 단계에서 Master 이외의 곳에서 Tuning Message가 올 수 없다 : Action Stop
			if (ulTuneIndex != CONDETECT_MASTER)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}
			#endif

			hErr = xmgr_search_SatConnectProcessPhase2Master (pstContents, ulTuneIndex);
			if (hErr != ERR_OK)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			break;

		case eCntState_Phase2_Slave:
			// 이 단계에서 Slave 이외의 곳에서 Tuning Message가 올 수 없다 : Action Stop
			if (ulTuneIndex != CONDETECT_SLAVE)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			hErr = xmgr_search_SatConnectProcessPhase2Slave (pstContents);
			if (hErr != ERR_OK)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			break;

		case eCntState_Phase3_Master:
			#if 0
			// 이 단계에서 Master 이외의 곳에서 Tuning Message가 올 수 없다 : Action Stop
			if (ulTuneIndex != CONDETECT_MASTER)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}
			#endif

			hErr = xmgr_search_SatConnectProcessPhase3Master (pstContents, ulTuneIndex);
			if (hErr != ERR_OK)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			break;

		case eCntState_Phase3_Slave:
			// 이 단계에서 Slave 이외의 곳에서 Tuning Message가 올 수 없다 : Action Stop
			if (ulTuneIndex != CONDETECT_SLAVE)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			hErr = xmgr_search_SatConnectProcessPhase3Slave (pstContents);
			if (hErr != ERR_OK)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			break;

		default:
			break;
		}
	}


	return MESSAGE_BREAK;
}

// eSEVT_CH_NO_SIGNAL:
STATIC HERROR xmgr_search_SatConnectEvtChNoSignal (XmgrSearch_SatConnectContents_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulTuneIndex;
	HERROR			 hErr;

	hErr = xmgr_search_SatConnectGetTuneIndex (pstContents, hAction, &ulTuneIndex);
	if (hErr == ERR_OK)
	{
		switch (pstContents->eState)
		{
		case eCntState_Phase1:
			// 이 단계에서 Tuner 0 이외의 곳에서 Tuning Message가 올 수 없다 : Action Stop
			if (ulTuneIndex != CONDETECT_MASTER)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			switch (pstContents->ePhase1State)
			{
			case eCntPhase1_Tp1Tuned:
				// 반대 Polar로 Tune 해서 Fail되어야 테스트로 쓸 수 있는 TP이다.
				pstContents->bType1Tuned = TRUE;
				pstContents->ePhase1State = eCntPhase1_Tp1Certified;
				break;

			case eCntPhase1_Tp2Tuned:
				// 반대 Polar로 Tune 해서 Fail되어야 테스트로 쓸 수 있는 TP이다.
				pstContents->bType2Tuned = TRUE;
				pstContents->ePhase1State = eCntPhase1_Tp2Certified;
				break;

			default:
				break;
			}

			// Horizontal / Vertical 둘 다 Tuning 성공 : 다음 단계인 2nd Tuner 시도
			if (pstContents->bType1Tuned == TRUE && pstContents->bType2Tuned == TRUE)
			{
				HxLOG_Info("[ConnectDetect] 1st Phase : HOR/Ver TP Found, Go to 2nd Phase\n");

				pstContents->eState 		= eCntState_Phase2_Master;
				pstContents->bType1Tuned	= FALSE;
				pstContents->bType2Tuned	= FALSE;
				pstContents->bMasterTuned	= FALSE;
				pstContents->bSlaveTuned	= FALSE;

				// Tuner 1으로 Hor/Ver을 각각 Tuning 시도
				hErr = xmgr_search_SatConnectTuneCheckTsList (pstContents);
				if (hErr != ERR_OK)
				{
					return MESSAGE_BREAK;
				}

				return ERR_OK;
			}

			// 여기서 Locking Fail이 일어나면 다음 TP로 테스트해야 한다.
			hErr = xmgr_search_SatConnectSetNextTp (pstContents);
			if (hErr != ERR_OK)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			hErr = xmgr_search_SatConnectTuneCheckTsList (pstContents);
			if (hErr != ERR_OK)
			{
				BUS_MGR_Destroy (NULL);
				return MESSAGE_BREAK;
			}

			break;

		case eCntState_Phase2_Master:
			// Tuning Fail되면 Error 처리
			HxLOG_Info("[ConnectDetect] 2rd Phase : Master Failed\n");

			pstContents->eState = eCntState_Finish;
			pstContents->eConnType = eDxANTCONN_ONECABLE;
			xmgr_search_SatConnectsetAntennaConnectionType (eDxANTCONN_ONECABLE);

			BUS_MGR_Destroy (NULL);
			return MESSAGE_BREAK;

		case eCntState_Phase2_Slave:
			// Tuning Fail되면 Error 처리
			HxLOG_Info("[ConnectDetect] 2rd Phase : Slave Failed\n");

			pstContents->eState = eCntState_Finish;
			pstContents->eConnType = eDxANTCONN_ONECABLE;
			xmgr_search_SatConnectsetAntennaConnectionType (eDxANTCONN_ONECABLE);

			BUS_MGR_Destroy (NULL);
			return MESSAGE_BREAK;

		case eCntState_Phase3_Master:
			// Tuning Fail되면 Error 처리
#if 0
			HxLOG_Info("[ConnectDetect] 3rd Phase : Master Failed\n");
			BUS_MGR_Destroy (NULL);
#endif
			if (ulTuneIndex == CONDETECT_MASTER)
			{
				HxLOG_Info("[ConnectDetect] 3rd Phase : Master Failed (One Cable)\n");

				pstContents->eState = eCntState_Finish;
				pstContents->eConnType = eDxANTCONN_ONECABLE;
				xmgr_search_SatConnectsetAntennaConnectionType (eDxANTCONN_ONECABLE);

				BUS_MGR_Destroy (NULL);
			}
			else if (ulTuneIndex == CONDETECT_SLAVE)
			{
				HxLOG_Info("[ConnectDetect] 3rd Phase : Slave Failed (One Cable)\n");

				pstContents->eState = eCntState_Finish;
				pstContents->eConnType = eDxANTCONN_ONECABLE;
				xmgr_search_SatConnectsetAntennaConnectionType (eDxANTCONN_ONECABLE);
				BUS_MGR_Destroy (NULL);
			}

			return MESSAGE_BREAK;

		case eCntState_Phase3_Slave:
			// 1nd, 2nd phase를 성공했는데 동시에 거는 것이 실패했다면 이는 Single (Loop Through)라고 판단
			HxLOG_Info("[ConnectDetect] 3rd Phase : Slave Failed (One Cable)\n");

			pstContents->eState = eCntState_Finish;
			pstContents->eConnType = eDxANTCONN_ONECABLE;
			xmgr_search_SatConnectsetAntennaConnectionType (eDxANTCONN_ONECABLE);
			BUS_MGR_Destroy (NULL);
			return MESSAGE_BREAK;

		default:
			BUS_MGR_Destroy (NULL);
			return MESSAGE_BREAK;
		}
	}


	return MESSAGE_BREAK;
}


#define	_____EXTERNAL_PROCESS_FUNCTION_____

// =============================================================================================
//
//   Connection Type 을 자동적으로 알아 내는 Action
// 		여기서는 Single (Loopthrough) 과 Dual Same을 알아내도록 한다.
//
// =============================================================================================

BUS_Result_t xproc_search_SatConnectionTypeDetectAction (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrSearch_SatConnectContents_t	*pstContents;
	HERROR					 hErr;

	pstContents = xmgr_search_SatConnectGetContents();
	if (pstContents == NULL)			{ return MESSAGE_BREAK; }

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		// Argument :
		// p1 : Antenna ID
		HxLOG_Info("\t[eMEVT_BUS_CREATE]\n\n");
		hErr = xmgr_search_SatConnectMsgGwmCreate (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Info("\t[eMEVT_BUS_DESTROY]\n\n");
		hErr = xmgr_search_SatConnectMsgGwmDestroyed (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Info("\t[eMEVT_BUS_TIMER]\n\n");
		hErr = xmgr_search_SatConnectMsgGwmTimer (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_LOCKED:
		HxLOG_Print("\t[eSEVT_CH_LOCKED]\n\n");

		hErr = xmgr_search_SatConnectCheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		hErr = xmgr_search_SatConnectEvtChLocked (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_NO_SIGNAL:
		HxLOG_Print("\t[eSEVT_CH_NO_SIGNAL]\n\n");

		hErr = xmgr_search_SatConnectCheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }

		hErr = xmgr_search_SatConnectEvtChNoSignal (pstContents, hAction);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

#if 0	// this code needs to remove in octo2
	case MSG_GWM_KEYUP:
	case MSG_GWM_KEYDOWN:
	case MSG_GWM_CLICKED:
		// UI 관련 메시지들은 무조건 pass해야 한다.
		return MESSAGE_PASS;
#endif

	default:
		break;
	}

	HxLOG_Info("xproc_search_SatConnectionTypeDetectAction() out\n");

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/* End of File. ---------------------------------------------------------- */


