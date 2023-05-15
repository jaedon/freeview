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

//#define TEMP_CHECK_ONE_RFINPUT_SYSTEM


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

} CntDetect_State_t;

typedef enum
{
	eCntPhase1_None					= 0,
	eCntPhase1_Tp1Tuned,
	eCntPhase1_Tp1Certified,
	eCntPhase1_Tp2Tuned,
	eCntPhase1_Tp2Certified

} CntDetect_P1State_t;

typedef enum
{
	eRfDetect_TuneState_Unknown,
	eRfDetect_TuneState_Succeeded,
	eRfDetect_TuneState_Failed,

	eRfDetect_TuneState_EndOfCase
} _mgrRfDetect_TuneState_e;

typedef struct
{
	HBOOL				 bTuned;
	DxTuneParam_t		 stTuneInfo;
} _mgrRfDetect_TuningInfo_t;

typedef struct
{
	HUINT32				 ulTunerId;

	HxList_t			*pstTunedTpList;				// _mgrRfDetect_TuningInfo_t : _mgrRfDetect_Context_t의 pstCheckTsList와 겹치므로 절대로 여기서 Object Free는 하지 말기.

	HBOOL				 bActionTaken, bTuneTried;
	_mgrRfDetect_TuneState_e	 eTuneState;

	HUINT32				 ulActionId;
} _mgrRfDetect_RfInput_t;

typedef struct
{
	HUINT32					 ulSearchTpNum;
	HUINT32					 ulCurrentRfInputId, ulRfInputNum;
	_mgrRfDetect_RfInput_t		 astRfInput[SVC_CH_MAX_RFINPUT_NUM];

	HxList_t					*pstCheckTsList;		// _mgrRfDetect_TuningInfo_t :
	HxList_t					*pstCurrTuneItem;

	DxAntennaConnection_e		eResultAntConnType;
} _mgrRfDetect_Context_t;


static _mgrRfDetect_Context_t		*s_pstDetectRfInput_Context = NULL;


#define _________________________________________________________________________________

static _mgrRfDetect_Context_t *_rfdetect_getContents (void)
{
	if (s_pstDetectRfInput_Context == NULL)
	{
		s_pstDetectRfInput_Context = (_mgrRfDetect_Context_t *)OxAP_Calloc (sizeof(_mgrRfDetect_Context_t));
	}

	return s_pstDetectRfInput_Context;
}

static void _rfdetect_freeContents (void)
{
	if (s_pstDetectRfInput_Context != NULL)
	{
		OxAP_Free (s_pstDetectRfInput_Context);
		s_pstDetectRfInput_Context = NULL;
	}
}

static HERROR _rfdetect_setAntennaConnectionType (DxAntennaConnection_e eAntConnType)
{
	HERROR	hErr;

	// 여기서 MWC_PARAM_SetAntennaConnectionType을 해 주기 때문에 그에 해당하는 Resource 코드가 필요해서 넣었다.
	{
		switch (eAntConnType)
		{
		case eDxANTCONN_TWOSAME:
//			MWC_PARAM_SetAntennaConnectionType (eDxANTCONN_TWOSAME);
			hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32)eDxANTCONN_TWOSAME, 0);
			if (ERR_OK != hErr)
			{
				HxLOG_Warning("DB_PARAM_SetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE) Error!!!\n");
			}
			SVC_PIPE_SetRfInputAbility (0, TRUE);
			SVC_PIPE_SetRfInputAbility (1, TRUE);
			break;

		case eDxANTCONN_ONECABLE:
		default:
//			MWC_PARAM_SetAntennaConnectionType (eDxANTCONN_ONECABLE);
			hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32)eDxANTCONN_ONECABLE, 0);
			if (ERR_OK != hErr)
			{
				HxLOG_Warning("DB_PARAM_SetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE) Error!!!\n");
			}
			SVC_PIPE_SetRfInputAbility (0, TRUE);
			SVC_PIPE_SetRfInputAbility (1, FALSE);
			break;
		}
	}
	return ERR_OK;
}

#define	_____RESOURCES_____
static HERROR _rfdetect_readRfInputCapacity (_mgrRfDetect_Context_t *pstContext)
{
	HUINT32	ulCount;
	HERROR	hErr;

	pstContext->ulRfInputNum = SVC_CH_CountSatelliteRfInput();
	if (0 == pstContext->ulRfInputNum)
	{
		HxLOG_Error ("Error:\n");
		return ERR_FAIL;
	}
	else if (SVC_CH_MAX_RFINPUT_NUM < pstContext->ulRfInputNum)
	{
		pstContext->ulRfInputNum = SVC_CH_MAX_RFINPUT_NUM;
	}

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32)eDxANTCONN_TWOSAME, 0);
	if(hErr != ERR_OK)
	{
		HxLOG_Error ("Error, SET DB eDxANTCONN_TWOSAME\n");
	}

	for (ulCount = 0; ulCount < pstContext->ulRfInputNum; ulCount++)
	{
		CH_SatRfInput_t 			 stMwRfInput;
		_mgrRfDetect_RfInput_t		*pstRfInputObj = &(pstContext->astRfInput[ulCount]);

		pstRfInputObj->bActionTaken		= FALSE;
		pstRfInputObj->bTuneTried			= FALSE;
		pstRfInputObj->eTuneState			= eRfDetect_TuneState_Unknown;

		SVC_PIPE_SetRfInputAbility (ulCount, TRUE);

		hErr = SVC_CH_GetSatelliteRfInput (ulCount, &stMwRfInput);
		if ((ERR_OK == hErr) && (stMwRfInput.ulConnectableTunerNum > 0))
		{
			//pstRfInputObj->ulTunerId = stMwRfInput.aulConnectableTunerId[0];
			pstRfInputObj->ulTunerId = (ulCount * 2);			// 원래라면 제대로 Connectable Tuner ID와 조합해서 섞이지 않게 해야 한다.
			pstRfInputObj->ulActionId = eActionId_VIEW_0 + ulCount;
		}
		else
		{
			HxLOG_Error ("Error:\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

static _mgrRfDetect_RfInput_t *_rfdetect_getRfInputFromActionId (_mgrRfDetect_Context_t *pstContext, HUINT32 ulActionId)
{
	HUINT32	ulCount;

	for (ulCount = 0; ulCount < pstContext->ulRfInputNum; ulCount++)
	{
		_mgrRfDetect_RfInput_t		*pstRfInputObj = &(pstContext->astRfInput[ulCount]);

		if (ulActionId == pstRfInputObj->ulActionId)
		{
			return pstRfInputObj;
		}
	}

	return NULL;
}

STATIC HERROR _rfdetect_checkTotalConnection (_mgrRfDetect_Context_t *pstContext, HBOOL *pbResultFound)
{
	HUINT32					 ulCount;
	HUINT32					 ulSucceeded = 0, ulFailed = 0;

	*pbResultFound = FALSE;

	for (ulCount = 0; ulCount < pstContext->ulRfInputNum; ulCount++)
	{
		_mgrRfDetect_RfInput_t	 *pstRfInputObj = &(pstContext->astRfInput[ulCount]);

		switch (pstRfInputObj->eTuneState)
		{
		case eRfDetect_TuneState_Succeeded:
			ulSucceeded ++;
			break;

		case eRfDetect_TuneState_Failed:
			if ( (pstContext->ulSearchTpNum == 1) && (ulCount == 0) )
				ulSucceeded ++;
			else
				ulFailed ++;
			break;

		case eRfDetect_TuneState_Unknown:
		default:
			return ERR_OK;
		}
	}

	// 추후 좀 더 정확한 판단을 하려면 여기 Logic이 달라져야 한다.
	if (0 == ulSucceeded)
	{
		// 다 Fail: 그냥 Tuning 안 되는 TP일 수 있다.
		HLIB_CMD_Printf("\n\t ===> Cable Connect is undefined...\n");
		*pbResultFound = FALSE;
	}
	else if (0 == ulFailed)
	{
		// 다 Tuning OK: 이것은 모든 RF-Input으로 다 나갈 수 있다.
		pstContext->eResultAntConnType = eDxANTCONN_TWOSAME;
		HLIB_CMD_Printf("\n\t ===> Cable Connect is DaulSame...\n");
		*pbResultFound = TRUE;
	}
	else
	{
		// 일부는 Tuning되고 일부는 Tuning되지 않았다 :
		pstContext->eResultAntConnType = eDxANTCONN_ONECABLE;
		HLIB_CMD_Printf("\n\t ===> Cable Connect is Single...\n");
		*pbResultFound = TRUE;
	}

	return ERR_OK;
}

#define	_____CONTENTS_INITIATION_____
static HERROR _rfdetect_initContext (_mgrRfDetect_Context_t *pstContext)
{
	HERROR				 hErr;

	// 1. Clear all the contents:
	HxSTD_memset (pstContext, 0, sizeof(_mgrRfDetect_Context_t));

//	pstContext->eResultAntConnType = MWC_PARAM_GetAntennaConnectionType();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32 *)&pstContext->eResultAntConnType, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE) Error!!!\n");
	}

	// 2. Read the RF-Input Capacity
	hErr = _rfdetect_readRfInputCapacity (pstContext);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define	_____PROCESSING_FUNCTIONS_____
static HERROR _rfdetect_setCheckTpList (_mgrRfDetect_Context_t *pstContext, HINT32 nAppHandle)
{
	HINT32				nTsCnt, nTsNum = 0, nChkTsCnt = 0;
	MgrSearch_Option_t 		*pstOption = (MgrSearch_Option_t *)nAppHandle;
	HERROR				 hErr;
	DxSat_Polarization_e		polar = eDxSAT_POLAR_AUTO;

	nTsNum = pstOption->ulTpNum;

	for (nTsCnt = 0; nTsCnt < nTsNum; nTsCnt++)
	{
		DbSvc_TsInfo_t				 stTsInfo;
		DxTuneParam_t 			 stChTuneInfo;
		_mgrRfDetect_TuningInfo_t	*pstTuneInfo;

		HxSTD_memset (&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
		HxSTD_memcpy (&(stTsInfo.stTuningParam), &(pstOption->pstTpArray[nTsCnt].stTuningParam), sizeof(DbSvc_TuningInfo_t));
		stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;

		hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
		if (ERR_OK == hErr)
		{
			if (nChkTsCnt < 2)
			{
				if (polar == eDxSAT_POLAR_AUTO)
				{
					if (stChTuneInfo.sat.tuningInfo.ePolarization != eDxSAT_POLAR_AUTO)
					{
						polar = stChTuneInfo.sat.tuningInfo.ePolarization;
						nChkTsCnt++;
					}
					else continue;
				}
				else
				{
					if (stChTuneInfo.sat.tuningInfo.ePolarization != eDxSAT_POLAR_AUTO && stChTuneInfo.sat.tuningInfo.ePolarization != polar)
					{
						nChkTsCnt++;
					}
					else continue;
				}

				pstTuneInfo = (_mgrRfDetect_TuningInfo_t *)OxAP_Calloc (sizeof(_mgrRfDetect_TuningInfo_t));
				if (NULL != pstTuneInfo)
				{
					HxSTD_memcpy (&(pstTuneInfo->stTuneInfo), &stChTuneInfo, sizeof(DxTuneParam_t));
					pstContext->pstCheckTsList = HLIB_LIST_Append (pstContext->pstCheckTsList, (void *)pstTuneInfo);
				}
				continue;
			}
			break;
		}
	}

	if (nChkTsCnt == 1)
	{
		_mgrRfDetect_TuningInfo_t *pstTuneInfo, *pstNTuneInfo;

		pstTuneInfo = (_mgrRfDetect_TuningInfo_t *)HLIB_LIST_Data(HLIB_LIST_First(pstContext->pstCheckTsList));
		if (pstTuneInfo != NULL)
		{
			if (polar == eDxSAT_POLAR_HOR)
				pstTuneInfo->stTuneInfo.sat.tuningInfo.ePolarization = eDxSAT_POLAR_VER;
			else
				pstTuneInfo->stTuneInfo.sat.tuningInfo.ePolarization = eDxSAT_POLAR_HOR;

			pstNTuneInfo = NULL;
			pstNTuneInfo = (_mgrRfDetect_TuningInfo_t *)OxAP_Calloc (sizeof(_mgrRfDetect_TuningInfo_t));
			if (NULL != pstNTuneInfo)
			{
				HxSTD_memcpy (&(pstNTuneInfo->stTuneInfo), &(pstTuneInfo->stTuneInfo), sizeof(DxTuneParam_t));
				if (polar == eDxSAT_POLAR_HOR)
					pstNTuneInfo->stTuneInfo.sat.tuningInfo.ePolarization = eDxSAT_POLAR_HOR;
				else
					pstNTuneInfo->stTuneInfo.sat.tuningInfo.ePolarization = eDxSAT_POLAR_VER;
				pstContext->pstCheckTsList = HLIB_LIST_Append (pstContext->pstCheckTsList, (void *)pstNTuneInfo);
			}
		}
	}

	pstContext->ulSearchTpNum = nChkTsCnt;

	return (pstContext->pstCheckTsList != NULL) ? ERR_OK : ERR_FAIL;
}

STATIC HERROR _rfdetect_tuneTp (_mgrRfDetect_Context_t *pstContext)
{
	HUINT32						ulCount;
	DxTuneParam_t				stChTuneInfo;
	_mgrRfDetect_RfInput_t			*pstRfInputInfo;
	_mgrRfDetect_TuningInfo_t		*pstTuneInfo;
	HERROR						 hErr;

	if (NULL == pstContext->pstCurrTuneItem)
	{
		pstContext->pstCurrTuneItem = pstContext->pstCheckTsList;
	}
	else
	{
		pstContext->pstCurrTuneItem = pstContext->pstCurrTuneItem->next;
	}

	pstTuneInfo = (_mgrRfDetect_TuningInfo_t *)HLIB_LIST_Data (pstContext->pstCurrTuneItem);
	if (NULL == pstTuneInfo)
	{
		return ERR_FAIL;
	}

	HxSTD_memcpy (&stChTuneInfo, &(pstTuneInfo->stTuneInfo), sizeof(DxTuneParam_t));

	for (ulCount = 0; ulCount < pstContext->ulRfInputNum; ulCount++)
	{
		pstRfInputInfo = &(pstContext->astRfInput[ulCount]);
		if (pstRfInputInfo->eTuneState == eRfDetect_TuneState_Unknown)
		{
			SvcRsc_Info_t stRscInfo;

			pstContext->ulCurrentRfInputId = ulCount;

			stChTuneInfo.sat.ulTunerGroupId	= pstRfInputInfo->ulTunerId;
			stChTuneInfo.sat.ulRfInputId		= ulCount;

			hErr = MGR_ACTION_SetupSearch (pstRfInputInfo->ulActionId, &stChTuneInfo, eAmPriority_SEARCH);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("MGR_ACTION_SetupSearch failed:0x%08x\n", hErr);
				goto ERROR;
			}

			hErr = MGR_ACTION_Take (pstRfInputInfo->ulActionId, eActionType_SEARCH, NULL);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("MGR_ACTION_Take failed:0x%08x\n", hErr);
				goto ERROR;
			}

			pstRfInputInfo->bActionTaken = TRUE;

			MGR_RSC_GetResourceInfo (pstRfInputInfo->ulActionId, &stRscInfo);

			hErr = SVC_CH_StopTune (SVC_PIPE_GetActionHandle (pstRfInputInfo->ulActionId), (HUINT16)-1, eSyncMode);
			/* fix for prevent #150270~#150273 */
			if(ERR_OK != hErr)
			{
				HxLOG_Warning("Error(%d) in SVC_CH_StopTune\n", hErr);
			}

			SVC_PIPE_IncreaseActionVersion (pstRfInputInfo->ulActionId);

			hErr = SVC_CH_StartTune (SVC_PIPE_GetActionHandle (pstRfInputInfo->ulActionId), &stRscInfo, (HUINT16)-1, &stChTuneInfo);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SVC_CH_StartTune failed:0x%08x\n", hErr);
				goto ERROR;
			}

			pstRfInputInfo->bTuneTried = TRUE;
			break;
		}
	}

	return ERR_OK;

ERROR:
{
	_mgrRfDetect_RfInput_t	 *pstRfInputInfo = &(pstContext->astRfInput[ulCount]);

	if (TRUE == pstRfInputInfo->bTuneTried)
	{
		hErr = SVC_CH_StopTune (SVC_PIPE_GetActionHandle (pstRfInputInfo->ulActionId), (HUINT16)-1, eSyncMode);
		/* fix for prevent #150270~#150273 */
		if(ERR_OK != hErr)
		{
			HxLOG_Warning("Error(%d) in SVC_CH_StopTune\n", hErr);
		}
		pstRfInputInfo->bTuneTried = FALSE;
	}

	if (TRUE == pstRfInputInfo->bActionTaken)
	{
		MGR_ACTION_Release (pstRfInputInfo->ulActionId, eActionType_SEARCH);
		pstRfInputInfo->bActionTaken = FALSE;
	}

	return ERR_FAIL;
}
}

static HERROR _rfdetect_clearCheckList (_mgrRfDetect_Context_t *pstContext)
{
	HxList_t	*pstItem = pstContext->pstCheckTsList;

	while (pstItem)
	{
		_mgrRfDetect_TuningInfo_t			*pstTuneInfo = (_mgrRfDetect_TuningInfo_t *)HLIB_LIST_Data (pstItem);

		if (NULL != pstTuneInfo)
		{
			OxAP_Free (pstTuneInfo);
		}

		pstItem = pstItem->next;
	}

	pstContext->pstCheckTsList = HLIB_LIST_RemoveAll (pstContext->pstCheckTsList);
	return ERR_OK;
}

#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____
//eMEVT_BUS_CREATE
static HERROR _rfdetect_msgGwmCreate (_mgrRfDetect_Context_t *pstContext, HINT32 nAppHandle)
{
	HERROR		 hErr;

	BUS_MGR_SetMgrName (BUS_MGR_GetThis(), "xproc_search_SatRfDetect");

	// 1. Initialize the contents
	hErr = _rfdetect_initContext (pstContext);
	if (hErr != ERR_OK)
	{
		HxLOG_Info("error in _rfdetect_initContext....");
	}

	hErr = _rfdetect_setCheckTpList (pstContext, nAppHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Info("error in _rfdetect_setCheckTpList....");
	}

#if defined(TEMP_CHECK_ONE_RFINPUT_SYSTEM)
	if (pstContext->ulRfInputNum < 2)			// RF-Input이 2개 미만 - 1개 이면 Auto Detect를 할 필요도 없는 Single이다.
	{
		pstContext->eResultAntConnType = eDxANTCONN_ONECABLE;
		BUS_MGR_Destroy(NULL);
	}
#endif

	return MESSAGE_BREAK;
}

// eMEVT_SEARCH_ACTION_START
static HERROR _rfdetect_msgGwmStartAction (_mgrRfDetect_Context_t *pstContext)
{
	HERROR	hErr;

	hErr = _rfdetect_tuneTp (pstContext);
	if (hErr != ERR_OK)
	{
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

	return MESSAGE_BREAK;
}

// eMEVT_BUS_DESTROY:
static HERROR _rfdetect_msgGwmDestroyed (_mgrRfDetect_Context_t *pstContext)
{
	HUINT32					 ulCount;
	HERROR					 hErr;

	for (ulCount = 0; ulCount < pstContext->ulRfInputNum; ulCount++)
	{
		_mgrRfDetect_RfInput_t		*pstRfInputObj = &(pstContext->astRfInput[ulCount]);

		if (TRUE == pstRfInputObj->bTuneTried)
		{
			hErr = SVC_CH_StopTune (SVC_PIPE_GetActionHandle (pstRfInputObj->ulActionId), (HUINT16)-1, eSyncMode);
			/* fix for prevent #150270~#150273 */
			if(ERR_OK != hErr)
			{
				HxLOG_Warning("Error(%d) in SVC_CH_StopTune\n", hErr);
			}
			pstRfInputObj->bTuneTried = FALSE;
		}

		if (TRUE == pstRfInputObj->bActionTaken)
		{
			MGR_ACTION_Release (pstRfInputObj->ulActionId, eActionType_SEARCH);
			pstRfInputObj->bActionTaken = FALSE;
		}
	}

	_rfdetect_setAntennaConnectionType (pstContext->eResultAntConnType);

	BUS_PostMessage (NULL, eMEVT_SEARCH_ANT_CONNECT_TYPE_DETECTED, (Handle_t)MGR_ACTION_GetMainActionId(), (HINT32)MGR_SEARCHUTIL_MakeCurrentAppHandle(), 0, 0);

	// Destroy the contents
	_rfdetect_clearCheckList (pstContext);
	_rfdetect_freeContents();

	return MESSAGE_BREAK;
}

// eSEVT_CH_LOCKED:
static HERROR _rfdetect_evtChLocked (_mgrRfDetect_Context_t *pstContext, Handle_t hAction)
{
	HBOOL					bResultFound = FALSE;
	_mgrRfDetect_RfInput_t		*pstRfInputObj = _rfdetect_getRfInputFromActionId (pstContext, SVC_PIPE_GetActionId (hAction));
	HERROR					hErr;

	if ((NULL != pstRfInputObj) && (TRUE == pstRfInputObj->bTuneTried))
	{
		pstRfInputObj->eTuneState = eRfDetect_TuneState_Succeeded;

		hErr = SVC_CH_StopTune (hAction, (HUINT16)-1, eSyncMode);
		/* fix for prevent #150270~#150273 */
		if(ERR_OK != hErr)
		{
			HxLOG_Warning("Error(%d) in SVC_CH_StopTune\n", hErr);
		}
		pstRfInputObj->bTuneTried = FALSE;

		MGR_ACTION_Release (pstRfInputObj->ulActionId, eActionType_SEARCH);
		pstRfInputObj->bActionTaken = FALSE;

		hErr = _rfdetect_checkTotalConnection (pstContext, &bResultFound);
		if (ERR_OK == hErr)
		{
			if (TRUE == bResultFound)
			{
				BUS_MGR_Destroy (NULL);
			}
			else
			{
				hErr = _rfdetect_tuneTp (pstContext);
				if (ERR_OK != hErr)
				{
					BUS_MGR_Destroy (NULL);
				}
			}
		}
	}

	return MESSAGE_PASS;
}

// eSEVT_CH_NO_SIGNAL:
static HERROR _rfdetect_evtChNoSignal (_mgrRfDetect_Context_t *pstContext, Handle_t hAction)
{
	HBOOL					bResultFound = FALSE;
	_mgrRfDetect_RfInput_t		*pstRfInputObj = _rfdetect_getRfInputFromActionId (pstContext, SVC_PIPE_GetActionId (hAction));
	HERROR					hErr;

	if ((NULL != pstRfInputObj) && (TRUE == pstRfInputObj->bTuneTried))
	{
		pstRfInputObj->eTuneState = eRfDetect_TuneState_Failed;

		hErr = SVC_CH_StopTune (hAction, (HUINT16)-1, eSyncMode);
		/* fix for prevent #150270~#150273 */
		if(ERR_OK != hErr)
		{
			HxLOG_Warning("Error(%d) in SVC_CH_StopTune\n", hErr);
		}
		pstRfInputObj->bTuneTried = FALSE;

		MGR_ACTION_Release (pstRfInputObj->ulActionId, eActionType_SEARCH);
		pstRfInputObj->bActionTaken = FALSE;

		hErr = _rfdetect_checkTotalConnection (pstContext, &bResultFound);
		if (ERR_OK == hErr)
		{
			if (TRUE == bResultFound)
			{
				BUS_MGR_Destroy (NULL);
			}
			else
			{
				hErr = _rfdetect_tuneTp (pstContext);
				if (ERR_OK != hErr)
				{
					BUS_MGR_Destroy (NULL);
				}
			}
		}
	}

	return MESSAGE_PASS;
}


#define	_____EXTERNAL_PROCESS_FUNCTION_____

// =============================================================================================
//
//   Connection Type 을 자동적으로 알아 내는 Action
// 		여기서는 Single (Loopthrough) 과 Dual Same을 알아내도록 한다.
//
// =============================================================================================

BUS_Result_t xproc_search_SatRfDetect (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	_mgrRfDetect_Context_t	*pstContext;
	HERROR					 hErr;

	pstContext = _rfdetect_getContents();
	if (pstContext == NULL)			{ return MESSAGE_BREAK; }

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		hErr = _rfdetect_msgGwmCreate (pstContext, p1);
		break;

	case eMEVT_BUS_DESTROY:
		hErr = _rfdetect_msgGwmDestroyed (pstContext);
		break;

	case eMEVT_SEARCH_ACTION_START:
		hErr = _rfdetect_msgGwmStartAction (pstContext);
		break;

	case eSEVT_CH_LOCKED:
		hErr = _rfdetect_evtChLocked (pstContext, hAction);
		break;

	case eSEVT_CH_NO_SIGNAL:
		hErr = _rfdetect_evtChNoSignal (pstContext, hAction);
		break;

	default:
		hErr = ERR_OK;
		break;
	}

	HxLOG_Info("xproc_search_SatRfDetect() out\n");

	if ((MESSAGE_BREAK != hErr) || (MESSAGE_PASS != hErr))
	{
		return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
	}

	return hErr;
}


/* End of File. ---------------------------------------------------------- */


