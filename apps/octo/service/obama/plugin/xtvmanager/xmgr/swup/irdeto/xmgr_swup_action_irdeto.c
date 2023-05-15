/**
	@file     ap_sat_search_svc.c
		---> xmgr_sw_update_action_irdeto.c
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
#include <svc_si.h>
#include <svc_sys.h>
#include <svc_resources.h>

#include <isosvc.h>

#include <mgr_tplist.h>
#include <mgr_live.h>
#include <mgr_pvr_action.h>
#include <mgr_swup.h>
#include <mgr_search.h>
#include <mgr_rscmap.h>
#include <xmgr_swup.h>

#include <mgr_common.h>

#include <mgr_spec.h>
#include <xsvc_si.h>
#include <otl.h>

#if defined(CONFIG_MW_CAS_IRDETO)
#include <ir_api.h>
#endif

#include "../local_include/_xmgr_swup.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define OTA_TIMER_LOCKING_ID			(0x2101)
#define OTA_TIMER_SI_ID					(0x2102)
#define OTA_TIMER_LOCKING_TIME			(4*1000)			// 4sec
#define OTA_TIMER_SI_TIME				(60*1000)		// 60sec


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief search application에서 실행에 필요한 environment context  */
typedef struct tagxmgrSwup_IrdetoActionContents_t
{
	Handle_t					 hSWUpdate;
	HUINT32						 ulActionId;			// Search Application Action ID
	Handle_t					 hAction;				// Search Action handle
	HUINT32						 ulTunerId;				// Tuner ID
	DxDeliveryType_e			 eDeliType;       		// Delivery Type
	MgrSwup_AppState_t			 eAppState;				// Current Search State :
	XsvcSi_SwupMode_e			 eSiSwupMode;			// Current SI Mode

	HBOOL						 bForcedStop;			// OTA Stopped forcely.

	SvcSi_SwupCheckSpec_t		 stOtaCheckSpec;		// OTA Check Sepc

	// === Tuning Informations ===
	DxTuneParam_t				 stTuningInfo;

	// === Home TP Lists ===
	POINTER_LIST_T				*pstHomeTpList;			// Home TsInfo : DbSvc_TsInfo_t

	// === Reference NIT TripleIds ===
	POINTER_LIST_T				*pstRefNitList;			// Reference NIT Links : DbSvc_TripleId_t

	// === SSU Scan Link TripleIds ===
	POINTER_LIST_T				*pstSsuScanList;		// SSU Scan Links : DbSvc_TripleId_t

	// === SSU Link TripleIds ===
	POINTER_LIST_T				*pstSsuLinkList;		// SSU Links : DbSvc_TripleId_t

	// === NIT TS Infos ===
	POINTER_LIST_T				*pstTsInfoListFromSi;	// TsInfo From SI : DbSvc_TsInfo_t

	// === Unt Meesage Descriptor Infos ===
	POINTER_LIST_T				*pstMessageInfo;		// MessageInfo From Unt : SvcSi_SsuMessageInfo_t

	// === Unt Update Descriptor Infos ===
	POINTER_LIST_T				*pstUpdateInfo;			// UpdateInfo From Unt : SvcSi_SsuUpdateInfo_t

	// === Unt Schedule Descriptor Infos ===
	POINTER_LIST_T				*pstScheduleInfo;		// ScheduleInfo From Unt : SvcSi_SsuScheduleInfo_t

	// === Satellite Information
	HUINT32						 ulAntennaIndex;		// Antenna Index
} xmgrSwup_IrdetoActionContents_t;


STATIC xmgrSwup_IrdetoActionContents_t	s_stIrdetoOta_Contents;

/* internal function prototype */

STATIC xmgrSwup_IrdetoActionContents_t* xmgr_swup_GetContents_Irdeto (void);

STATIC INLINE HERROR	xmgr_swup_CheckActionId_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, Handle_t hAction);
STATIC INLINE void		xmgr_swup_IncreaseSessionVersion_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC void				xmgr_swup_FreeLinkItem_Irdeto (void *pvItem);
STATIC void 			xmgr_swup_FreeMessageInfoLinkItem_Irdeto (void *pvItem);

STATIC HERROR 			xmgr_swup_InitContents_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate);
STATIC HERROR			xmgr_swup_MakeHomeTpList_Irdeto (xmgrSwup_IrdetoActionContents_t* pstContents);

STATIC HERROR			xmgr_swup_DestroyContents_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_FindTsInfoByTripleId_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, DbSvc_TripleId_t *pstTripleId, DbSvc_TsInfo_t *pstSvcTsInfo);

STATIC HERROR			xmgr_swup_TuneSearchTp_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_SetTuningParam_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_SetNextTpForOta_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_CountOtaTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, HUINT32 *pulTpNum);
STATIC HERROR			xmgr_swup_GetOtaTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo);
STATIC void				xmgr_swup_SetSiOtaMode_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, XsvcSi_SwupMode_e eOtaMode);
STATIC DbSvc_TsInfo_t*	xmgr_swup_GetCurrentHomeTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC void				xmgr_swup_AddHomeTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo);
STATIC void				xmgr_swup_SetNextHomeTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllHomeTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllRefNitLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC DbSvc_TripleId_t*	xmgr_swup_GetCurrentSsuScanLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC void				xmgr_swup_SetNextSsuScanLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllSsuScanLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC DbSvc_TripleId_t*	xmgr_swup_GetCurrentSsuLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC void				xmgr_swup_AddSsuLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuLink);
STATIC void				xmgr_swup_SetNextSsuLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllSsuLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC void 			xmgr_swup_ClearAllTsLink_Irdeto (xmgrSwup_IrdetoActionContents_t * pstContents);
STATIC void 			xmgr_swup_ClearSsuDescriptorLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_StartAction_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_StopAction_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_StartSi_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC DbSvc_TsInfo_t*	xmgr_swup_AllocTsInfo_Irdeto (void);
STATIC void				xmgr_swup_FreeTsInfo_Irdeto (void *pvTsInfo);
STATIC void				xmgr_swup_ReleaseAction_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);

STATIC void				xmgr_swup_PrintTripleId_Irdeto (DbSvc_TripleId_t *pstTripleId);
STATIC void				xmgr_swup_PrintCurrentTuningInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);

STATIC HERROR 			xmgr_swup_MsgGwmCreate_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate);
STATIC HERROR			xmgr_swup_MsgGwmDestroyed_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_MsgGwmTimer_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, HINT32 nTimerId);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccess_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccessInHomeTp_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiSearchFail_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtChLocked_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtChNoSignal_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_MsgGwmQuitStandby_Irdeto  (xmgrSwup_IrdetoActionContents_t *pstContents);


#define	_________________________Body___________________________________

STATIC xmgrSwup_IrdetoActionContents_t *xmgr_swup_GetContents_Irdeto (void)
{
	return &s_stIrdetoOta_Contents;
}

#define	____________________for__UI_____________________


#define _________________________________________________________________________________

STATIC INLINE HERROR xmgr_swup_CheckActionId_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, Handle_t hAction)
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

STATIC INLINE void xmgr_swup_IncreaseSessionVersion_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

STATIC void xmgr_swup_FreeLinkItem_Irdeto (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

STATIC void xmgr_swup_FreeMessageInfoLinkItem_Irdeto (void *pvItem)
{
	SvcSi_SsuMessageInfo_t	*pstMessageInfo;

	if (pvItem != NULL)
	{
		pstMessageInfo = (SvcSi_SsuMessageInfo_t*)pvItem;

		if(pstMessageInfo->pucText != NULL)
		{
			OxAP_Free(pstMessageInfo->pucText);
		}

		OxAP_Free (pstMessageInfo);
	}
}


STATIC SvcSi_SwupCheckSpec_t* xmgr_swup_GetOtaSpec_Irdeto(void)
{
	STATIC SvcSi_SwupCheckSpec_t 	stOtaSpec;

	HxSTD_MemSet(&stOtaSpec, 0x00, sizeof(SvcSi_SwupCheckSpec_t));
	stOtaSpec.eActionKind = eSVCSI_ACT_KIND_SwupCheck;
	stOtaSpec.ulActionSpec = eSVCSI_ACT_SPEC_Irdeto;

	return &stOtaSpec;
}



#define	_____CONTENTS_INITIATION_____

STATIC HERROR xmgr_swup_InitContents_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate)
{
	DbSvc_TsInfo_t	*pstTsInfo;
	HERROR			 hErr;

	// 1. Clear all the contents.
	HxSTD_memset (pstContents, 0, sizeof(xmgrSwup_IrdetoActionContents_t));

	// 2. Action Handle and Action ID
	pstContents->hSWUpdate	= hSWUpdate;
	pstContents->hAction	= hAction;
#if defined(CONFIG_MW_CH_SATELLITE)
	pstContents->eDeliType	= eDxDELIVERY_TYPE_SAT;
#elif defined(CONFIG_MW_CH_CABLE)
	pstContents->eDeliType	= eDxDELIVERY_TYPE_CAB;
#endif
	pstContents->ulActionId	= SVC_PIPE_GetActionId (hAction);
	pstContents->ulTunerId	= (HUINT32)nTunerId;

	pstContents->bForcedStop = FALSE;

	HxSTD_memcpy(&pstContents->stOtaCheckSpec, xmgr_swup_GetOtaSpec_Irdeto(), sizeof(SvcSi_SwupCheckSpec_t));

	// 3. tuning information :
	// TP List :
	hErr = xmgr_swup_MakeHomeTpList_Irdeto (pstContents);
	if (hErr != ERR_OK)					{ return MESSAGE_BREAK; }

	pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Irdeto (pstContents);
	if (pstTsInfo != NULL)
	{
		pstContents->eDeliType = pstTsInfo->eDeliType;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_MakeHomeTpList_Irdeto (xmgrSwup_IrdetoActionContents_t* pstContents)
{
	HUINT32					 ulTpCnt, ulTpNum;
	DbSvc_TsInfo_t			*pstTsInfo;
	HERROR					 hErr;

	if (pstContents == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	// Get the default TPs and the numbers.
	hErr = xmgr_swup_CountOtaTsInfo_Irdeto (pstContents, &ulTpNum);
	if (hErr != ERR_OK || ulTpNum == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
	{
		pstTsInfo = xmgr_swup_AllocTsInfo_Irdeto();
		if (pstTsInfo != NULL)
		{
			hErr = xmgr_swup_GetOtaTsInfo_Irdeto (pstContents, ulTpCnt, pstTsInfo);
			if (hErr == ERR_OK)
			{
				xmgr_swup_AddHomeTsInfo_Irdeto (pstContents, pstTsInfo);
			}
			else
			{
				xmgr_swup_FreeTsInfo_Irdeto (pstTsInfo);
			}
		}
	}

	if (xmgr_swup_GetCurrentHomeTsInfo_Irdeto (pstContents) == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define	_____CONTENTS_DESTRUCTION_____

STATIC HERROR xmgr_swup_DestroyContents_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	xmgr_swup_ClearAllHomeTsInfo_Irdeto (pstContents);
	xmgr_swup_ClearAllRefNitLink_Irdeto (pstContents);
	xmgr_swup_ClearAllSsuScanLink_Irdeto (pstContents);
	xmgr_swup_ClearAllSsuLink_Irdeto (pstContents);
	xmgr_swup_ClearAllTsLink_Irdeto(pstContents);
	xmgr_swup_ClearSsuDescriptorLink_Irdeto(pstContents);

	return ERR_BUS_NO_ERROR;
}


#define	_____DATA_CONVERSION_____

STATIC HERROR xmgr_swup_FindTsInfoByTripleId_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, DbSvc_TripleId_t *pstTripleId, DbSvc_TsInfo_t *pstSvcTsInfo)
{
	DbSvc_TsInfo_t		*pstTsInfo;
	POINTER_LIST_T		*pstTsList;

	if (pstContents == NULL || pstTripleId == NULL)		{ return ERR_BUS_SOURCE_NULL; }
	if (pstSvcTsInfo == NULL)							{ return ERR_BUS_TARGET_NULL; }

	if (pstContents->pstTsInfoListFromSi == NULL)		{ return ERR_BUS_SOURCE_NULL; }
	pstTsList = pstContents->pstTsInfoListFromSi;

	while (pstTsList != NULL)
	{
		pstTsInfo = (DbSvc_TsInfo_t *)pstTsList->pvContents;
		if (pstTsInfo != NULL)
		{
			if (pstTsInfo->usOnId == pstTripleId->usOnId &&
				pstTsInfo->usTsId == pstTripleId->usTsId)
			{
				*pstSvcTsInfo = *pstTsInfo;
				return ERR_BUS_NO_ERROR;
			}
		}

		pstTsList = pstTsList->pNext;
		if (pstTsList == pstContents->pstTsInfoListFromSi)			{ break; }
	}

	return ERR_BUS_NO_OBJECT;
}

#define	_____STATE_PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_swup_TuneSearchTp_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	HBOOL				 bRetry;
	SvcRsc_Info_t			 stRscInfo;
	HERROR				 hErr;

	bRetry = TRUE;

	while (bRetry == TRUE)
	{
		hErr = xmgr_swup_SetTuningParam_Irdeto (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("No Tp left to tune\n");

			hErr = xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("No Next Tp left to tune\n");
				pstContents->eAppState = eOtaApp_state_checkend;
				bRetry = FALSE;
			}

			continue;
		}

		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("Tuner Stop Failed\n");
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_BUS_NO_OBJECT;
		}

		xmgr_swup_IncreaseSessionVersion_Irdeto (pstContents);			// Tuning 전에 반드시 Action Version을 업데이트해야 한다.

		HxLOG_Print("Tune TP  : ");
		xmgr_swup_PrintCurrentTuningInfo_Irdeto (pstContents);
		HxLOG_Print("\n");

		MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);
		hErr = SVC_CH_StartTune(pstContents->hAction, &stRscInfo, (HUINT16)-1, &pstContents->stTuningInfo);
		if (hErr == ERR_OK)
		{
			pstContents->eAppState = eOtaApp_state_tryTuning;

			// DD에서 Tuning Signal이 올라오지 않아 무한으로 기다리는 것을 막기 위해 여기서 timer를 처리하도록 한다.
			BUS_SetTimer (OTA_TIMER_LOCKING_ID, OTA_TIMER_LOCKING_TIME);
			return ERR_BUS_NO_ERROR;
		}

		// Tuning failure : Go to the next TP
		hErr = xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
			bRetry = FALSE;
		}
	}

	return ERR_BUS_NO_OBJECT;
}

STATIC HERROR xmgr_swup_SetTuningParam_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	DbSvc_TsInfo_t		 stSvcTsInfo;
	DbSvc_TsInfo_t		*pstTsInfo;
	DbSvc_TripleId_t		*pstTripleId;
	HERROR				 hErr;

	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_IRDETO_HOME_TP:
		pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Irdeto (pstContents);
		if (pstTsInfo == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		stSvcTsInfo = *pstTsInfo;

		break;

	case eSI_SWUP_MODE_IRDETO_SSUSCAN_TP:
		// SSU SCAN List
		pstTripleId = xmgr_swup_GetCurrentSsuScanLink_Irdeto (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		hErr = xmgr_swup_FindTsInfoByTripleId_Irdeto (pstContents, pstTripleId, &stSvcTsInfo);
		if (hErr != ERR_BUS_NO_ERROR)
		{
			return ERR_BUS_NO_OBJECT;
		}
		break;

	case eSI_SWUP_MODE_IRDETO_SSUSVC_TP:
		// SSU LINK List
		pstTripleId = xmgr_swup_GetCurrentSsuLink_Irdeto (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		hErr = xmgr_swup_FindTsInfoByTripleId_Irdeto (pstContents, pstTripleId, &stSvcTsInfo);
		if (hErr != ERR_BUS_NO_ERROR)
		{
			return ERR_BUS_NO_OBJECT;
		}
		break;

	default:
		return ERR_BUS_NO_OBJECT;
	}

	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stSvcTsInfo, &(pstContents->stTuningInfo));

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_SetNextTpForOta_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_IRDETO_HOME_TP:
		xmgr_swup_SetNextHomeTsInfo_Irdeto (pstContents);
		if (xmgr_swup_GetCurrentHomeTsInfo_Irdeto (pstContents) == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_IRDETO_SSUSCAN_TP:
		xmgr_swup_SetNextSsuScanLink_Irdeto (pstContents);
		if (xmgr_swup_GetCurrentSsuScanLink_Irdeto (pstContents) == NULL)
		{
			// SSU-BAT TP 가 더 이상 남아 있지 않다 : 다음 Home TP로 가서 작업을 한다.
			xmgr_swup_SetSiOtaMode_Irdeto (pstContents, eSI_SWUP_MODE_IRDETO_HOME_TP);
			return xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
		}

		break;

	case eSI_SWUP_MODE_IRDETO_SSUSVC_TP:
		xmgr_swup_SetNextSsuLink_Irdeto (pstContents);
		if (xmgr_swup_GetCurrentSsuLink_Irdeto (pstContents) == NULL)
		{
			// SSU-SVC TP 가 더 이상 남아 있지 않다 : 다음 SSU-BAT TP로 가서 작업을 한다.
			xmgr_swup_SetSiOtaMode_Irdeto (pstContents, eSI_SWUP_MODE_IRDETO_SSUSCAN_TP);
			return xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
		}

		break;

	default:
		break;
	}

	return ERR_BUS_NO_ERROR;
}


#define	_____CONTENTS_DATA_FUNCTIONS_____

STATIC HERROR xmgr_swup_CountOtaTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, HUINT32 *pulTpNum)
{
	(void)pstContents;

	if (pulTpNum == NULL)					{ return ERR_BUS_TARGET_NULL; }

	*pulTpNum = MGR_SWUPINFO_CountOtaHomeTp();
	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_GetOtaTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR		 hErr;

	(void)pstContents;

	if (pstTsInfo == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = MGR_SWUPINFO_GetOtaHomeTp (ulTpNum, pstTsInfo);
	if (hErr != ERR_OK)						{ return ERR_BUS_OUT_OF_RANGE; }

	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
	return ERR_BUS_NO_ERROR;
}

STATIC void xmgr_swup_SetSiOtaMode_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, XsvcSi_SwupMode_e eOtaMode)
{
	HxLOG_Print("[xmgr_swup_SetSiOtaMode_Irdeto] SI Mode %d -> %d\n", pstContents->eSiSwupMode, eOtaMode);
	pstContents->eSiSwupMode = eOtaMode;
}


// Home Ts Info
STATIC DbSvc_TsInfo_t *xmgr_swup_GetCurrentHomeTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		if (pstContents->pstHomeTpList != NULL)
		{
			return (DbSvc_TsInfo_t *)pstContents->pstHomeTpList->pvContents;
		}
	}

	return NULL;
}

STATIC void xmgr_swup_AddHomeTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo)
{
	if (pstContents != NULL && pstHomeTsInfo != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstHomeTpList, (void *)pstHomeTsInfo);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstHomeTpList);
	}
}

STATIC void xmgr_swup_SetNextHomeTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstHomeTpList != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstHomeTpList, pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Irdeto);
	}
}

STATIC void xmgr_swup_ClearAllHomeTsInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Irdeto);
	}
}
STATIC void xmgr_swup_ClearAllRefNitLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstRefNitList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstRefNitList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Irdeto);
	}
}


// SSU SCAN LINK : SSU-BAT
STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentSsuScanLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		if (pstContents->pstSsuScanList != NULL)
		{
			return (DbSvc_TripleId_t *)pstContents->pstSsuScanList->pvContents;
		}
	}

	return NULL;
}

STATIC void xmgr_swup_SetNextSsuScanLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstSsuScanList != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuScanList, pstContents->pstSsuScanList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Irdeto);
	}
}

STATIC void xmgr_swup_ClearAllSsuScanLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuScanList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Irdeto);
	}
}

// SSU LINK : SSU-SVC
STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentSsuLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		if (pstContents->pstSsuLinkList != NULL)
		{
			return (DbSvc_TripleId_t *)pstContents->pstSsuLinkList->pvContents;
		}
	}

	return NULL;
}

STATIC void xmgr_swup_AddSsuLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuLink)
{
	if (pstContents != NULL && pstSsuLink != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSsuLinkList, (void *)pstSsuLink);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstSsuLinkList);
	}
}

STATIC void xmgr_swup_SetNextSsuLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstSsuLinkList != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuLinkList, pstContents->pstSsuLinkList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Irdeto);
	}
}

STATIC void xmgr_swup_ClearAllSsuLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuLinkList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Irdeto);
	}
}

STATIC void xmgr_swup_ClearAllTsLink_Irdeto (xmgrSwup_IrdetoActionContents_t * pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTsInfoListFromSi, (FREE_FUNC)xmgr_swup_FreeLinkItem_Irdeto);
	}
}

STATIC void xmgr_swup_ClearSsuDescriptorLink_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		if(pstContents->pstMessageInfo)
		{
			pstContents->pstMessageInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstMessageInfo, (FREE_FUNC)xmgr_swup_FreeMessageInfoLinkItem_Irdeto);
		}

		if(pstContents->pstUpdateInfo)
		{
			pstContents->pstUpdateInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstUpdateInfo, (FREE_FUNC)xmgr_swup_FreeLinkItem_Irdeto);
		}

		if(pstContents->pstScheduleInfo)
		{
			pstContents->pstScheduleInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstScheduleInfo, (FREE_FUNC)xmgr_swup_FreeLinkItem_Irdeto);
		}
	}
}

#define	_____PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_swup_StartAction_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.
	HERROR			 hErr;

	//	Stop Record....
#if	defined(CONFIG_MW_MM_PVR)
	{
		HUINT32 	ulActionId;

		for (ulActionId = eActionId_REC_FIRST; ulActionId < eActionId_REC_LAST; ulActionId++)
		{
			BUS_SendMessage (NULL, eMEVT_PVR_RECORD_STOP_REC, SVC_PIPE_GetActionHandle (ulActionId), 0, 0, 0);
		}
	}
#endif

	xmgr_swup_SetSiOtaMode_Irdeto (pstContents, eSI_SWUP_MODE_IRDETO_HOME_TP);

	// 1. Set the TP to tuning :
	hErr = xmgr_swup_SetTuningParam_Irdeto (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	// 2. Get the Action : IRDETO OTA는 Search와 다를 바가 없으므로 Action은 Search 로써 잡아도 된다.
	hErr = MGR_ACTION_SetupSearch (pstContents->ulActionId, &pstContents->stTuningInfo, eAmPriority_SEARCH);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_Take (pstContents->ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	pstContents->eAppState = eOtaApp_state_idle;

	// Tune the TP
	hErr = xmgr_swup_TuneSearchTp_Irdeto (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_StopAction_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	if (pstContents->eAppState == eOtaApp_state_waitingSi)
	{
		// Stop SI

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
	}

	if (pstContents->eAppState != eOtaApp_state_init)
	{
		xmgr_swup_ReleaseAction_Irdeto (pstContents);
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_StartSi_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	HUINT16			 	 usExtId1, usExtId2;
	DbSvc_TripleId_t		*pstTripleId = NULL;
	SvcSi_SwupCheckOption_t 	stOtaCheckOption;
#if defined(CONFIG_MW_CH_CABLE)
	HUINT32 				 ulNetworkId = 0;
#endif

	usExtId1 = 0;
	usExtId2 = 0;

	// SSU Link에 있는 SVCID가 필요하다.
	if (pstContents->eSiSwupMode == eSI_SWUP_MODE_IRDETO_SSUSVC_TP)
	{
		pstTripleId = xmgr_swup_GetCurrentSsuLink_Irdeto (pstContents);
		if (pstTripleId != NULL)
		{
			usExtId1 = pstTripleId->usSvcId;
		}
	}

#if defined(CONFIG_MW_CH_CABLE)
	MGR_SEARCHUTIL_GetCabNetworkIdFromDb(&ulNetworkId);
	if (ulNetworkId > 0)
	{
		usExtId2 = (HUINT16)ulNetworkId;
	}
#endif

	HxSTD_MemSet(&stOtaCheckOption, 	0x00, sizeof(SvcSi_SwupCheckOption_t));
	stOtaCheckOption.ulSwupMode = eSI_SWUP_MODE_IRDETO_HOME_TP + abs(pstContents->eSiSwupMode - eSI_SWUP_MODE_IRDETO_HOME_TP);	// new si 적용이 완료되면 new type으로 모두 변경하자 !
	stOtaCheckOption.usExtId1 = usExtId1;
	stOtaCheckOption.usExtId2 = usExtId2;

	HxLOG_Print("[%s, %d] eOtaMode(0x%x) ExtId1(0x%x) ExtId2(0x%x)\n,",
		__FUNCTION__, __LINE__, stOtaCheckOption.ulSwupMode, stOtaCheckOption.usExtId1, stOtaCheckOption.usExtId2 );

	return SVC_SI_StartSwupCheck(pstContents->hAction, &pstContents->stOtaCheckSpec, &stOtaCheckOption, FALSE);
}

STATIC DbSvc_TsInfo_t *xmgr_swup_AllocTsInfo_Irdeto (void)
{
	return (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
}

STATIC void xmgr_swup_FreeTsInfo_Irdeto (void *pvTsInfo)
{
	if (pvTsInfo != NULL)
	{
		OxAP_Free (pvTsInfo);
	}
}

STATIC void xmgr_swup_ReleaseAction_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	HERROR			 hErr;

	hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	// Release the action :
	MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
}


#define _____DEBUG_FUNCTIONS_____

STATIC void	xmgr_swup_PrintTripleId_Irdeto (DbSvc_TripleId_t *pstTripleId)
{
#if defined(CONFIG_DEBUG)
	HxLOG_Print("(%04x:%04x:%04x)", pstTripleId->usOnId, pstTripleId->usTsId, pstTripleId->usSvcId);
#else
	(void)pstTripleId;
#endif
}

STATIC void xmgr_swup_PrintCurrentTuningInfo_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
#if defined(CONFIG_DEBUG)
	DbSvc_TuningInfo_t	 stSvcTuneInfo;
	HERROR				 hErr;

	hErr = OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&(pstContents->stTuningInfo), &stSvcTuneInfo);
	if (hErr == ERR_OK)
	{
		XMGR_SWUP_PrintTuningInfo(pstContents->stTuningInfo.eDeliType, &stSvcTuneInfo);
	}
#else
	(void)pstContents;
#endif
}

#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____

//eMEVT_BUS_CREATE
STATIC HERROR xmgr_swup_MsgGwmCreate_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate)
{
	HERROR		 hErr;

	// Create시 해 주어야 하는 Initialize 등이 실패할 경우에는 이 Action 자체가 살아 있어서는 안된다.
	// 곧바로 Destroy 해 주도록 한다.

	// 1. Initialize the contents
	hErr = xmgr_swup_InitContents_Irdeto (pstContents, hAction, nTunerId, hSWUpdate);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SWUP_NOTIFY_DETECT_FAIL, (Handle_t)pstContents->hSWUpdate, 0, 0, 0);
		return hErr;
	}

	// 2. Start the OTA Action :
	hErr = xmgr_swup_StartAction_Irdeto (pstContents);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0, eSWUP_FAILREASON_Action);
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SWUP_NOTIFY_DETECT_FAIL, (Handle_t)pstContents->hSWUpdate, 0, 0, eSWUP_FAILREASON_Action);
		return hErr;
	}

	MGR_SWUPINFO_ResetSearchedInfo();

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY:
STATIC HERROR xmgr_swup_MsgGwmDestroyed_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	// Stop all the Action
	xmgr_swup_StopAction_Irdeto (pstContents);

	// Destroy the contents
	xmgr_swup_DestroyContents_Irdeto (pstContents);

	// Inform that the OTA action is destroyed
	if (pstContents->bForcedStop != TRUE)
	{
//		BUS_PostMessage (NULL, MSG_APP_OTA_ACTION_DESTROYED, (Handle_t)NULL, (HINT32)BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()), 0, 0);
	}

	return MESSAGE_BREAK;
}

// eMEVT_BUS_TIMER:
STATIC HERROR xmgr_swup_MsgGwmTimer_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents, HINT32 nTimerId)
{
	HERROR				 hErr, hRetValue;

	hRetValue = ERR_BUS_NO_ERROR;

	switch (nTimerId)
	{
	case OTA_TIMER_LOCKING_ID:
		BUS_KillTimer (nTimerId);

		if (pstContents->eAppState == eOtaApp_state_tryTuning)
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Irdeto (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[%s] No Next Tp left to tune\n", __FUNCTION__);
				pstContents->eAppState = eOtaApp_state_checkend;
			}
		}

		hRetValue = MESSAGE_BREAK;
		break;

	case OTA_TIMER_SI_ID:
		BUS_KillTimer (nTimerId);

		if (pstContents->eAppState == eOtaApp_state_waitingSi)
		{
			// MW SI를 Stop시키고 다음 TP로 넘어가도록 한다.

			hErr = SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
			if (hErr == ERR_OK)
			{
				// 다음 TP 로 넘어가야 한다.
				hErr = xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_swup_TuneSearchTp_Irdeto (pstContents);
				}
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[%s] Cannot proceed anymore\n",__FUNCTION__);
				pstContents->eAppState = eOtaApp_state_checkend;
			}
		}

		hRetValue = MESSAGE_BREAK;
		break;

	default:
		break;
	}

	if (pstContents->eAppState == eOtaApp_state_checkend)
	{
		// Tuning 단에서 실패 후 곧바로 State End :
		// 더 이상 진행할 수 없으니 Action App을 종료시킨다.
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SWUP_NOTIFY_DETECT_FAIL, (Handle_t)pstContents->hSWUpdate, 0, 0, 0);
	}

	return hRetValue;
}

// EVT_SI_OTA_SUCCESS:
STATIC HERROR xmgr_swup_EvtSiOtaSuccess_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Print("[%s] pstContents->eAppState: %d, pstContents->eSiSwupMode: %d \n", __FUNCTION__, pstContents->eAppState, pstContents->eSiSwupMode);

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_waitingSi:
		// SI succeeded : Kill the timer

		BUS_KillTimer (OTA_TIMER_SI_ID);

		pstContents->eAppState = eOtaApp_state_siReceived;

		switch (pstContents->eSiSwupMode)
		{
		case eSI_SWUP_MODE_IRDETO_HOME_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInHomeTp_Irdeto (pstContents);
			break;

		case eSI_SWUP_MODE_IRDETO_SSUSVC_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Irdeto (pstContents);
			break;

		default:
			break;
		}

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		if (pstContents->eAppState != eOtaApp_state_checkend)
		{
			hErr = xmgr_swup_TuneSearchTp_Irdeto (pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("[%s] No Next Tp left to tune\n", __FUNCTION__);
				pstContents->eAppState = eOtaApp_state_checkend;
			}
		}
		break;

	default:
		break;
	}

	if (pstContents->eAppState == eOtaApp_state_checkend)
	{
		// Tuning 단에서 실패 후 곧바로 State End :
		// 더 이상 진행할 수 없으니 Action App을 종료시킨다.
		if((pstContents->eSiSwupMode != eSI_SWUP_MODE_IRDETO_SSUSVC_TP)
			|| ((pstContents->eSiSwupMode == eSI_SWUP_MODE_IRDETO_SSUSVC_TP) && (hErr != ERR_OK)))
		{
			MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
			BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SWUP_NOTIFY_DETECT_FAIL, (Handle_t)pstContents->hSWUpdate, 0, 0, 0);
		}
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_EvtSiOtaSuccessInHomeTp_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	HBOOL					bSiDataFound;
	HUINT32					ulIndex, ulDataNum;
	XsvcSi_SwupMode_e	 	eNextOtaMode = eSI_SWUP_MODE_MAX;
	DbSvc_TsInfo_t			*pstHomeTsInfo;
	DbSvc_TsInfo_t			*pstTsInfo, *pstTsInfoArray;
	DbSvc_TripleId_t		*pstTripleId, *pstTripleIdArray;
	HERROR					hErr;

	bSiDataFound = FALSE;

	pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Irdeto (pstContents);
	if (pstHomeTsInfo != NULL)
	{
		pstContents->ulAntennaIndex = (HUINT32)pstHomeTsInfo->stTuningParam.sat.ucAntId;

		// Get SSU Scan Triple ID from BAT
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
		if (hErr == ERR_OK && pstTripleIdArray != NULL)
		{
			HxLOG_Print("[irdetoota] BAT-DataNum : %d (%s %d lines)\n", ulDataNum, __FUNCTION__, __LINE__);

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
				if (pstTripleId != NULL)
				{
					*pstTripleId = pstTripleIdArray[ulIndex];
					xmgr_swup_AddSsuLink_Irdeto (pstContents, pstTripleId);

					HxLOG_Print("[irdetoota] BAT-Index (%d) \n", ulIndex);
					xmgr_swup_PrintTripleId_Irdeto (pstTripleId);
					HxLOG_Print("\n");
				}
			}

			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, (void *)pstTripleIdArray);
		}
		else
		{
			HxLOG_Print("[irdetoota] BAT Info Not Found\n");
		}

		// Search TS Info from NIT
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, &ulDataNum, (void **)&pstTsInfoArray);
		if (hErr == ERR_OK && pstTsInfoArray != NULL)
		{
			HxLOG_Print("[irdetoota] NIT TS Info : %d\n", ulDataNum);

			bSiDataFound = TRUE;
			eNextOtaMode = eSI_SWUP_MODE_IRDETO_SSUSVC_TP;

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				pstTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
				if (pstTsInfo != NULL)
				{
					*pstTsInfo = pstTsInfoArray[ulIndex];

					pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
					pstTsInfo->eDeliType = pstContents->eDeliType;
					if (pstContents->eDeliType == eDxDELIVERY_TYPE_SAT)
					{
						pstTsInfo->stTuningParam.sat.ucAntId = pstHomeTsInfo->stTuningParam.sat.ucAntId;
					}

					HxLOG_Print("[irdetoota] TP (%d) ", ulIndex);
					XMGR_SWUP_PrintTuningInfo(pstTsInfo->eDeliType, &pstTsInfo->stTuningParam);
					HxLOG_Print("\n");

					pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTsInfoListFromSi, (void *)pstTsInfo);
				}
			}

			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, (void *)pstTsInfoArray);
		}
		else
		{
			HxLOG_Print("[%s] NIT Info Not Found\n",__FUNCTION__);
		}
	}
	else
	{
		HxLOG_Print("[%s] pstHomeTsInfo == NULL \n", __FUNCTION__);
	}

	HxLOG_Print("[%s] bSiDataFound: %d, eNextOtaMode: %d \n", __FUNCTION__, bSiDataFound, eNextOtaMode);
	if (bSiDataFound == TRUE)
	{
		xmgr_swup_SetSiOtaMode_Irdeto(pstContents, eNextOtaMode);
	}
	else
	{
		hErr = xmgr_swup_SetNextTpForOta_Irdeto(pstContents);
		if (hErr != ERR_OK)			// 모든 TP를 다 체크함
		{
			HxLOG_Print("[%s] eOtaApp_state_checkend !! \n", __FUNCTION__);
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;
}

#if !defined(CONFIG_OTA_IRDETO_LOADER_V5)
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	HUINT32 					 ulDataNum;
	XsvcSi_SwupSignal_Irdeto_t	 *pstIrOtaArray, *pstIrOtaInfo;
	DxTuneParam_t 				 stChTuneInfo;
	DbSvc_TsInfo_t				 *pstHomeTsInfo;
	HERROR						 hErr, hResult = ERR_BUS_NO_ERROR;
	SvcSi_SwupSignal_t			 stOtaInfo;
	DbSvc_TsInfo_t				 stOtaTsInfo;
	SvcSwUpdate_DetectResult_t	 *pstDetectResult = NULL;

	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SIGNAL, &ulDataNum, (void **)&pstIrOtaArray);
	if (hErr == ERR_OK && pstIrOtaArray != NULL)
	{
		pstIrOtaInfo = &(pstIrOtaArray[0]);

		HxSTD_MemSet (&stOtaTsInfo, 0, sizeof(DbSvc_TsInfo_t));
		HxSTD_MemSet (&stOtaInfo, 0, sizeof(SvcSi_SwupSignal_t));

		pstDetectResult = (SvcSwUpdate_DetectResult_t *)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
		if (pstDetectResult == NULL)
		{
			HxLOG_Error("Memory alloc fail!!\n");
			hResult = ERR_FAIL;
			goto END_FUNC;
		}

		// OTA Transponder Information
		hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
		if (hErr != ERR_OK)
		{
			stChTuneInfo = pstContents->stTuningInfo;
		}

		OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&stChTuneInfo, &stOtaTsInfo.stTuningParam);

		pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Irdeto (pstContents);
		if (pstHomeTsInfo != NULL)
		{
			stOtaTsInfo.eDeliType 				= pstHomeTsInfo->eDeliType;
			stOtaTsInfo.ucTunerId 				= pstHomeTsInfo->ucTunerId;
#if defined(CONFIG_MW_CH_SATELLITE)
			stOtaTsInfo.stTuningParam.sat.ucAntId 	= pstHomeTsInfo->stTuningParam.sat.ucAntId;
			stOtaTsInfo.ucTunerId				= pstHomeTsInfo->ucTunerId;
			stOtaTsInfo.usOnId						= pstHomeTsInfo->usOnId;
			stOtaTsInfo.usTsId						= pstHomeTsInfo->usTsId;
#endif
		}
		else
		{
			HxLOG_Critical("\n\n");
		}

		// OTA Software information
		stOtaInfo.eDataType = eVERSION_TYPE_APPL;
		XMGR_SWUP_GetForcedOtaFlag_Irdeto(&stOtaInfo.ucSwupType);
		stOtaInfo.usPid = pstIrOtaInfo->usPid;
		stOtaInfo.ucTableId = pstIrOtaInfo->ucTableId;
		HxSTD_memcpy(&stOtaInfo.stDownloadTime,  &(pstIrOtaInfo->stDownloadTime), sizeof(HxDATETIME_Time_t));
		stOtaInfo.ucLoadSeqNumber = pstIrOtaInfo->ucLoadSeqNumber;

		HxLOG_Print("[%s][%04] ============= Found OTA Info \n", __FUNCTION__, __LINE__);
		HxLOG_Print("\t tsId (0x%x) onId(0x%x) \n", stOtaTsInfo.usTsId, stOtaTsInfo.usOnId);
#if defined(CONFIG_MW_CH_SATELLITE)
		HxLOG_Print("\t FREQ (%d) SR(%d) FEC (%d) \n", stOtaTsInfo.stTuningParam.sat.ulFrequency, stOtaTsInfo.stTuningParam.sat.ulSymbolRate,	stOtaTsInfo.stTuningParam.sat.eFecCodeRate);
#endif
#if defined(CONFIG_MW_CH_CABLE)
		HxLOG_Print("\t Freq (%d) SR(%d) Con (%d) \n", stOtaTsInfo.stTuningParam.cab.ulFreq, stOtaTsInfo.stTuningParam.cab.usSR,  stOtaTsInfo.stTuningParam.cab.ucCon);
#endif
		HxLOG_Print("\t DeliType : %04x \n", stOtaTsInfo.eDeliType);
		HxLOG_Print("\t ucOtaType : %04x \n", stOtaInfo.ucSwupType);
		HxLOG_Print("\t usOtaPid : %04x \n", stOtaInfo.usPid);
		HxLOG_Print("\t ucTableId : %04x \n", stOtaInfo.ucTableId);
		HxLOG_Print("\t ucLoadSeqNumber : %04x \n", stOtaInfo.ucLoadSeqNumber);
		HxLOG_Print("=====================================\n\n");

		// save SvcSwUpdate_DetectResult_t inform
		pstDetectResult->bAvailable	= TRUE;
		pstDetectResult->bFound		= TRUE;
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stChTuningInfo, &stChTuneInfo, sizeof(DxTuneParam_t));
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stSvcTsInfo, &stOtaTsInfo, sizeof(DbSvc_TsInfo_t));

		// irdoto 에서는 version 을 ucLoadSeqNumber 에 처리하므로.. stDetectResult.ulDataVersion 는 기록 안함.
		pstDetectResult->usOtaPid	   		= pstIrOtaInfo->usPid;
		pstDetectResult->eSource			= eDxSWUPDATE_SOURCE_Rf;
		MGR_SWUP_SetDetectInfo(pstContents->hSWUpdate, pstDetectResult);
		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32)pstDetectResult, sizeof(SvcSwUpdate_DetectResult_t));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x]\n", hErr);
			hResult = ERR_FAIL;
			goto END_FUNC;
		}

		MGR_SWUPINFO_SetSearchedInfo(&stOtaTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);

		if (stOtaInfo.ucSwupType == 1)	// if forced ota
		{
			DbSvc_TsInfo_t				 stTsInfo;
			SvcSi_SwupSignal_t 	 		 stForcedOtaInfo;
			DxTuneParam_t				 stChTuneInfo;
			HUINT32 					 ulDiTuneDataSize=0;
			void						*pvDiTuneData = NULL;

			// serched info 가 존재하는지 check
			 hErr = MGR_SWUPINFO_GetSearchedInfo(&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stForcedOtaInfo);
			if (hErr != ERR_OK)
			{
				hResult = ERR_FAIL;
				goto END_FUNC;
			}

			// 1. Tuning Info 를 DD용으로 converting 한다.
			// 1-1. DB Tuning Info -> Ch Tuning Info
			hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
			if (hErr != ERR_OK)
			{
				hResult = ERR_FAIL;
				goto END_FUNC;
			}

			// 1-2. Ch Tuning Info -> DD Tuning Data
			hErr = SVC_CH_MakeDiTuningDataByChTuningParam(&stChTuneInfo, &ulDiTuneDataSize, &pvDiTuneData);
		 	if (hErr != ERR_OK)
		 	{
				HxLOG_Critical("Error!!!!\n");
				hResult = ERR_FAIL;
				goto END_FUNC;
		 	}

		#if defined(CONFIG_MW_CAS_IRDETO)
			// 2. Set OTA Info to NVRAM
			xsvc_cas_IrOtaSetChannelParam(stTsInfo.ucTunerId, pvDiTuneData);
		#endif

			// 3. Cancel 한 정보 등을 다 삭제하도록 한다.
			MGR_SWUPINFO_ResetSearchedInfo();
			MGR_SWUPINFO_ResetOtaRetryCount();
			MGR_SWUPINFO_SetOtaCanceledVersion (0, 0);

			// 4. Now, store the informations
			//	당장 Reboot 하지 않아도 곧 Reboot할 수 있으니 적어놓도록 한다.
			SVC_SYS_SetOtaType(eSWUP_INFO_SLOT_0, eSWUP_DOWNLOAD_DVBSSU); // RF는 eOTA_INFO_SLOT_0를 사용한다.
			SVC_SYS_SetOtaPid(eSWUP_INFO_SLOT_0, stForcedOtaInfo.usPid);
			SVC_SYS_SetOtaTuningInfo(eSWUP_INFO_SLOT_0, &stChTuneInfo);
			SVC_CH_FreeDiTuningData(pvDiTuneData);

		#if defined(CONFIG_MW_CAS_IRDETO)
			xsvc_cas_IrOtaSetFlagUp(TRUE, stForcedOtaInfo.usPid, stForcedOtaInfo.ucTableId);
		#endif

			XMGR_SWUP_SetForcedOtaFlag_Irdeto(0);
			pstContents->eAppState = eOtaApp_state_checkend;

			//	OTA나왔으니 스스로 죽자...
			BUS_MGR_Destroy(NULL);

			MGR_ACTION_RebootSystem(); // destroy 하고 reboot 하는 것으로 수정. 이전에 ota fail 나는 이슈 존재 ...
			hErr = ERR_BUS_NO_ERROR;
			goto END_FUNC;
		}

		// detect 보내기..
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_SUCCESS, eSWUP_STATE_Unknown, (HINT32)&stOtaTsInfo,(HINT32)&stOtaInfo,0);

		pstContents->eAppState = eOtaApp_state_checkend;
	}
	else
	{
		hResult = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(pstIrOtaArray)
	{
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SIGNAL, (void *)pstIrOtaArray);
	}

	if(pstDetectResult != NULL)
	{
		HLIB_STD_MemFree(pstDetectResult);
		pstDetectResult = NULL;
	}

	if(hResult == ERR_FAIL)
	{
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, (Handle_t)0, 0, 0, 0);

		HxLOG_Print("[irdetoota] hErr = %d, pstOtaArray = 0x%x\n", hErr, pstIrOtaArray);
		hErr = xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[%s] all tp checked & set eapp state to eOtaApp_state_checkend\n", __FUNCTION__);
			pstContents->eAppState = eOtaApp_state_checkend;
		}
	}

	return hErr;
}
#else
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	HUINT32 					 ulDataNum;
	XsvcSi_SwupSignal_Irdeto_t	 *pstIrOtaArray, *pstIrOtaInfo;
	DxTuneParam_t 				 stChTuneInfo;
	DbSvc_TsInfo_t				 *pstHomeTsInfo;
	HERROR						 hErr, hResult = ERR_BUS_NO_ERROR;
	SvcSi_SwupSignal_t			 stOtaInfo;
	DbSvc_TsInfo_t				 stOtaTsInfo;
	SvcSwUpdate_DetectResult_t	 *pstDetectResult = NULL;

	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SIGNAL, &ulDataNum, (void **)&pstIrOtaArray);
	if (hErr == ERR_OK && pstIrOtaArray != NULL)
	{
		pstIrOtaInfo = &(pstIrOtaArray[0]);

		HxSTD_MemSet (&stOtaTsInfo, 0, sizeof(DbSvc_TsInfo_t));
		HxSTD_MemSet (&stOtaInfo, 0, sizeof(SvcSi_SwupSignal_t));

		pstDetectResult = (SvcSwUpdate_DetectResult_t *)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
		if (pstDetectResult == NULL)
		{
			HxLOG_Error("Memory alloc fail!!\n");
			hResult = ERR_FAIL;
			goto END_FUNC;
		}

		// OTA Transponder Information
		hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
		if (hErr != ERR_OK)
		{
			stChTuneInfo = pstContents->stTuningInfo;
		}

		OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&stChTuneInfo, &stOtaTsInfo.stTuningParam);

		pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Irdeto (pstContents);
		if (pstHomeTsInfo != NULL)
		{
			stOtaTsInfo.eDeliType 				= pstHomeTsInfo->eDeliType;
			stOtaTsInfo.ucTunerId 				= pstHomeTsInfo->ucTunerId;
#if defined(CONFIG_MW_CH_SATELLITE)
			stOtaTsInfo.stTuningParam.sat.ucAntId 	= pstHomeTsInfo->stTuningParam.sat.ucAntId;
			stOtaTsInfo.ucTunerId				= pstHomeTsInfo->ucTunerId;
			stOtaTsInfo.usOnId						= pstHomeTsInfo->usOnId;
			stOtaTsInfo.usTsId						= pstHomeTsInfo->usTsId;
#endif
		}
		else
		{
			HxLOG_Critical("\n\n");
		}

		// OTA Software information
		stOtaInfo.eDataType = eVERSION_TYPE_APPL;
		XMGR_SWUP_GetForcedOtaFlag_Irdeto(&stOtaInfo.ucSwupType);
		stOtaInfo.usPid = pstIrOtaInfo->usPid;
		stOtaInfo.usDlownloadSeqNumber = pstIrOtaInfo->usDownloadSequenceNumber;

		HxLOG_Print("[%s][%04] ============= Found OTA Info \n", __FUNCTION__, __LINE__);
		HxLOG_Print("\t tsId (0x%x) onId(0x%x) \n", stOtaTsInfo.usTsId, stOtaTsInfo.usOnId);
#if defined(CONFIG_MW_CH_SATELLITE)
		HxLOG_Print("\t FREQ (%d) SR(%d) FEC (%d) \n", stOtaTsInfo.stTuningParam.sat.ulFrequency, stOtaTsInfo.stTuningParam.sat.ulSymbolRate,	stOtaTsInfo.stTuningParam.sat.eFecCodeRate);
#endif
#if defined(CONFIG_MW_CH_CABLE)
		HxLOG_Print("\t Freq (%d) SR(%d) Con (%d) \n", stOtaTsInfo.stTuningParam.cab.ulFreq, stOtaTsInfo.stTuningParam.cab.usSR, stOtaTsInfo.stTuningParam.cab.ucCon);
#endif
		HxLOG_Print("\t DeliType : %04x \n", stOtaTsInfo.eDeliType);
		HxLOG_Print("\t ucOtaType : %04x \n", stOtaInfo.ucSwupType);
		HxLOG_Print("\t usOtaPid : %04x \n", stOtaInfo.usPid);
		HxLOG_Print("\t usDlownloadSeqNumber : %04x \n", stOtaInfo.usDlownloadSeqNumber);
		HxLOG_Print("=====================================\n\n");

		// save SvcSwUpdate_DetectResult_t inform
		pstDetectResult->bAvailable	= TRUE;
		pstDetectResult->bFound		= TRUE;
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stChTuningInfo, &stChTuneInfo, sizeof(DxTuneParam_t));
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stSvcTsInfo, &stOtaTsInfo, sizeof(DbSvc_TsInfo_t));

		// irdoto 에서는 version 을 ucLoadSeqNumber 에 처리하므로.. stDetectResult.ulDataVersion 는 기록 안함.
		pstDetectResult->usOtaPid	   		= pstIrOtaInfo->usPid;
		pstDetectResult->eSource			= eDxSWUPDATE_SOURCE_Rf;
		MGR_SWUP_SetDetectInfo(pstContents->hSWUpdate, pstDetectResult);
		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32)pstDetectResult, sizeof(SvcSwUpdate_DetectResult_t));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x]\n", hErr);
			hResult = ERR_FAIL;
			goto END_FUNC;
		}

		MGR_SWUPINFO_SetSearchedInfo(&stOtaTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);

		if (stOtaInfo.ucSwupType == 1)	// if forced ota
		{
			DbSvc_TsInfo_t				 stTsInfo;
			SvcSi_SwupSignal_t 	 		 stForcedOtaInfo;
			DxTuneParam_t				 stChTuneInfo;
			HUINT32 					 ulDiTuneDataSize=0;
			void						*pvDiTuneData = NULL;

			// serched info 가 존재하는지 check
			 hErr = MGR_SWUPINFO_GetSearchedInfo(&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stForcedOtaInfo);
			if (hErr != ERR_OK)
			{
				hResult = ERR_FAIL;
				goto END_FUNC;
			}

			// 1. Tuning Info 를 DD용으로 converting 한다.
			// 1-1. DB Tuning Info -> Ch Tuning Info
			hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
			if (hErr != ERR_OK)
			{
				hResult = ERR_FAIL;
				goto END_FUNC;
			}

			// 1-2. Ch Tuning Info -> DD Tuning Data
			hErr = SVC_CH_MakeDiTuningDataByChTuningParam(&stChTuneInfo, &ulDiTuneDataSize, &pvDiTuneData);
		 	if (hErr != ERR_OK)
		 	{
				HxLOG_Critical("Error!!!!\n");
				hResult = ERR_FAIL;
				goto END_FUNC;
		 	}

		#if defined(CONFIG_MW_CAS_IRDETO)
			// 2. Set OTA Info to NVRAM
			xsvc_cas_IrOtaSetChannelParam(stTsInfo.ucTunerId, pvDiTuneData);
		#endif

			// 3. Cancel 한 정보 등을 다 삭제하도록 한다.
			MGR_SWUPINFO_ResetSearchedInfo();
			MGR_SWUPINFO_ResetOtaRetryCount();
			MGR_SWUPINFO_SetOtaCanceledVersion (0, 0);

			// 4. Now, store the informations
			//	당장 Reboot 하지 않아도 곧 Reboot할 수 있으니 적어놓도록 한다.
			SVC_SYS_SetOtaType(eSWUP_INFO_SLOT_0, eSWUP_DOWNLOAD_DVBSSU); // RF는 eOTA_INFO_SLOT_0를 사용한다.
			SVC_SYS_SetOtaPid(eSWUP_INFO_SLOT_0, stForcedOtaInfo.usPid);
			SVC_SYS_SetOtaTuningInfo(eSWUP_INFO_SLOT_0, &stChTuneInfo);
			SVC_CH_FreeDiTuningData(pvDiTuneData);

		#if defined(CONFIG_MW_CAS_IRDETO)
			xsvc_cas_IrOtaSetFlagUp(TRUE, stForcedOtaInfo.usPid, stForcedOtaInfo.ucTableId);
		#endif

			XMGR_SWUP_SetForcedOtaFlag_Irdeto(0);
			pstContents->eAppState = eOtaApp_state_checkend;

			//	OTA나왔으니 스스로 죽자...
			BUS_MGR_Destroy(NULL);

			MGR_ACTION_RebootSystem(); // destroy 하고 reboot 하는 것으로 수정. 이전에 ota fail 나는 이슈 존재 ...
			hErr = ERR_BUS_NO_ERROR;
			goto END_FUNC;
		}

		// detect 보내기..
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_SUCCESS, eSWUP_STATE_Unknown, (HINT32)&stOtaTsInfo,(HINT32)&stOtaInfo,0);

		pstContents->eAppState = eOtaApp_state_checkend;
	}
	else
	{
		hResult = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(pstIrOtaArray)
	{
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SIGNAL, (void *)pstIrOtaArray);
	}

	if(pstDetectResult != NULL)
	{
		HLIB_STD_MemFree(pstDetectResult);
		pstDetectResult = NULL;
	}

	if(hResult == ERR_FAIL)
	{
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, (Handle_t)0, 0, 0, 0);

		HxLOG_Print("[irdetoota] hErr = %d, pstOtaArray = 0x%x\n", hErr, pstIrOtaArray);
		hErr = xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[%s] all tp checked & set eapp state to eOtaApp_state_checkend\n", __FUNCTION__);
			pstContents->eAppState = eOtaApp_state_checkend;
		}
	}

	return hErr;
}
#endif

// eSEVT_SI_SEARCH_FAIL:
STATIC HERROR xmgr_swup_EvtSiSearchFail_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_waitingSi:
		// SI failed : Kill the timer

		BUS_KillTimer (OTA_TIMER_SI_ID);

		pstContents->eAppState = eOtaApp_state_siFailed;

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_swup_TuneSearchTp_Irdeto (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_swup_EvtSiSearchFail_Irdeto] No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
			break;
		}
		break;

	default:
		break;
	}

	if (pstContents->eAppState == eOtaApp_state_checkend)
	{
		// Tuning 단에서 실패 후 곧바로 State End :
		// 더 이상 진행할 수 없으니 Action App을 종료시킨다.
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
	}

	return MESSAGE_BREAK;
}

// eSEVT_CH_LOCKED
STATIC HERROR xmgr_swup_EvtChLocked_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_tryTuning :
		// Channel Locked : Kill the timer
		BUS_KillTimer (OTA_TIMER_LOCKING_ID);

		// Start the SI for the IRDETO OTA
		hErr = xmgr_swup_StartSi_Irdeto (pstContents);
		if (hErr == ERR_OK)
		{
			pstContents->eAppState = eOtaApp_state_waitingSi;
			// MW SI를 무한으로 기다리는 것을 막기 위해 여기에서 Timer를 걸어야 한다.
			BUS_SetTimer (OTA_TIMER_SI_ID, OTA_TIMER_SI_TIME);
		}
		else
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Irdeto (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_EvtChLocked_Irdeto] No Next Tp left to tune\n");
				pstContents->eAppState = eOtaApp_state_checkend;
			}
		}
		break;

	default:
		break;
	}

	if (pstContents->eAppState == eOtaApp_state_checkend)
	{
		// Tuning 단에서 실패 후 곧바로 State End :
		// 더 이상 진행할 수 없으니 Action App을 종료시킨다.
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_EvtChNoSignal_Irdeto (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_tryTuning :
		// Channel Locked : Kill the timer

		BUS_KillTimer (OTA_TIMER_LOCKING_ID);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_swup_TuneSearchTp_Irdeto (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[%s] No Next Tp left to tune\n" ,__FUNCTION__);
			pstContents->eAppState = eOtaApp_state_checkend;
		}

		break;

	case eOtaApp_state_waitingSi:
		// Stop SI :

		hErr = SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
		if (hErr == ERR_OK)
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_SetNextTpForOta_Irdeto (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Irdeto (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[%s] No Next Tp left to tune\n",__FUNCTION__);
				pstContents->eAppState = eOtaApp_state_checkend;
			}
		}
		else
		{
			// 더 이상 진행 불가능 : Search End로 간주한다.
			HxLOG_Print("[%s] SI Re-open failed.\n",__FUNCTION__);
			pstContents->eAppState = eOtaApp_state_checkend;
		}

		break;

	default:
		break;
	}

	if (pstContents->eAppState == eOtaApp_state_checkend)
	{
		// Tuning 단에서 실패 후 곧바로 State End :
		// 더 이상 진행할 수 없으니 Action App을 종료시킨다.
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_MsgGwmQuitStandby_Irdeto  (xmgrSwup_IrdetoActionContents_t *pstContents)
{
	pstContents->bForcedStop = TRUE;
	BUS_MGR_Destroy (NULL);

	return ERR_OK;
}

#define	_____EXTERNAL_PROCESS_FUNCTION_____

BUS_Result_t xproc_swup_Detect_Irdeto(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	static HBOOL			 s_bFirstStart = TRUE;
	xmgrSwup_IrdetoActionContents_t		*pstContents;
	HERROR					 hErr;

	pstContents = xmgr_swup_GetContents_Irdeto();
	if (s_bFirstStart == TRUE)
	{
		s_bFirstStart = FALSE;
		HxSTD_memset (pstContents, 0, sizeof(xmgrSwup_IrdetoActionContents_t));
	}

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Print("\t[%s] ----- [eMEVT_BUS_CREATE]\n\n", __FUNCTION__);
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "XMGR_OTA_IRDETO_Proc");
		hErr = xmgr_swup_MsgGwmCreate_Irdeto (pstContents, hAction, p1, p2);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Print("\t[%s] ----- [eMEVT_BUS_TIMER]\n\n", __FUNCTION__);
		hErr = xmgr_swup_MsgGwmTimer_Irdeto (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Print("\t[%s] ----- [eMEVT_BUS_DESTROY]\n\n", __FUNCTION__);
		hErr = xmgr_swup_MsgGwmDestroyed_Irdeto (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_SUCCESS:
		hErr = xmgr_swup_CheckActionId_Irdeto (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[%s] ----- [eSEVT_SWUP_DETECT_SUCCESS]\n\n", __FUNCTION__);

		hErr = xmgr_swup_EvtSiOtaSuccess_Irdeto (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_FAIL:
		hErr = xmgr_swup_CheckActionId_Irdeto (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[%s] ----- [eSEVT_SWUP_DETECT_FAIL]\n\n", __FUNCTION__);

		hErr = xmgr_swup_EvtSiSearchFail_Irdeto (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_LOCKED:
		hErr = xmgr_swup_CheckActionId_Irdeto (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[%s] ----- [eSEVT_CH_LOCKED]\n\n", __FUNCTION__);

		hErr = xmgr_swup_EvtChLocked_Irdeto (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_NO_SIGNAL:
		hErr = xmgr_swup_CheckActionId_Irdeto (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[%s] ----- [eSEVT_CH_NO_SIGNAL]\n\n", __FUNCTION__);

		hErr = xmgr_swup_EvtChNoSignal_Irdeto (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_GO_OPERATION :
		HxLOG_Print("\t[%s] ----- [eMEVT_BUS_GO_OPERATION]\n\n", __FUNCTION__);
		hErr = xmgr_swup_MsgGwmQuitStandby_Irdeto (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	default:
		break;
	}

	HxLOG_Print("[%s] ------ \n", __FUNCTION__);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


STATIC HUINT8				s_ucControlByte = 0;

void XMGR_SWUP_SetControlByte_Irdeto(HUINT8 ucControlByte)
{
	s_ucControlByte = ucControlByte;
}

void XMGR_SWUP_GetControlByte_Irdeto(HUINT8 *pucControlByte)
{
	if (pucControlByte)
	{
		*pucControlByte = s_ucControlByte;
	}
}

void XMGR_SWUP_CheckDisableIrdetoOta_Irdeto(HUINT8 *pucMenuDisable, HUINT8 *pucEmmDisable)
{
	HxLOG_Print("[%s] ControlByte (0x%x)\n", __FUNCTION__, s_ucControlByte);
	switch(s_ucControlByte)
	{
		case 0x00 :
			*pucMenuDisable = TRUE;
			*pucEmmDisable = TRUE;
			break;
		case 0x01 :
			*pucMenuDisable = TRUE;
			*pucEmmDisable = FALSE;
			break;
		case 0x02 :
			*pucMenuDisable = FALSE;
			*pucEmmDisable = FALSE;
			break;
		case 0x03 :
			*pucMenuDisable = FALSE;
			*pucEmmDisable = FALSE;
			break;
		default :
			*pucMenuDisable = TRUE;
			*pucEmmDisable = TRUE;
			break;
	}
}


STATIC HUINT8	s_ucForcedOta = 0;


void	XMGR_SWUP_SetForcedOtaFlag_Irdeto(HUINT8 ucForcedOta)
{
	s_ucForcedOta = ucForcedOta;
}


void	XMGR_SWUP_GetForcedOtaFlag_Irdeto(HUINT8 *pucForcedOta)
{
	*pucForcedOta = s_ucForcedOta;
}


#define _____OTA_TP_____
#if 0
#if	defined(CONFIG_MW_CH_SATELLITE)
static DbSvc_SatTuningInfo_t	 s_stIrdetoOtaHomeTuneInfo[] =
{
	{	12603,	22000,	eDxSAT_POLAR_HOR,	eDxSAT_CODERATE_5_6,	eDxSAT_TRANS_DVBS,	eDxSAT_PSK_QPSK,	0,	0,	0},
	{	0,	0,		0,		0,				0,					0,				0,				0,	0},
};
#endif

#if	defined(CONFIG_MW_CH_CABLE)
static DbSvc_CabTuningInfo_t	 s_stIrdetoOtaHomeCabTuneInfo[] =
{
	{	450000,	6900,		0x2717,	eDxCAB_CONSTELLATION_64QAM,	eDxCAB_SPECTRUM_AUTO},
	{	0,		0,			0,		0,							0,				  },
};
#endif
#endif

HERROR xmgr_swup_SetDefaultHomeTsInfo_Irdeto (void)
{
#if	defined(CONFIG_MW_CH_SATELLITE)
	Handle_t				hSvcHandle = HANDLE_NULL;
#endif
	DbSvc_TsInfo_t			stTsInfo;

#if	defined(CONFIG_MW_CH_CABLE)
	DbSvc_CabTuningInfo_t 	*pstCabTuneInfo;
#endif
	DbSvc_TsCondition_t		stCond;
	DbSvc_TsInfo_t			*pstTsInfo = NULL;
	HINT32					i, lAllTpCount = 0;
	HERROR					hErr = ERR_OK;


	MGR_SWUPINFO_ResetHomeTp ();
	HxSTD_memset (&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));

#if	defined(CONFIG_MW_CH_SATELLITE)

	//Set the Current TP : mhoh
	hErr = MGR_LIVE_GetServiceHandle (MGR_ACTION_GetMainActionId(), eLiveViewPlaySvc_ACTUAL, &hSvcHandle);
	if(hErr != ERR_OK || hSvcHandle == HANDLE_NULL)
	{
		HxLOG_Error("[%s] OxMGR_WTV_GetSvcHandle() fail!!\n",__FUNCTION__);

		hErr = MGR_LIVE_GetLastSvcHandle(eLastSvcItem_SAT, &hSvcHandle);
	}

	if (hErr == ERR_OK && hSvcHandle != HANDLE_NULL)
	{
		hErr = DB_SVC_GetTsInfoByServiceHandle(hSvcHandle, &stTsInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("[%s] DB_SVC_GetTsInfoByServiceHandle fail!!\n",__FUNCTION__);
		}
		MGR_SWUPINFO_AddOtaHomeTp (&stTsInfo);
	}
	else
	{
		HxLOG_Error("[%s] Check All Searched Tp!!\n", __FUNCTION__);

		DB_SVC_InitTsFindCondition(&stCond);
		stCond.nDeliType = eDxDELIVERY_TYPE_SAT;

		hErr = DB_SVC_FindTsInfoList(&stCond, &lAllTpCount, &pstTsInfo);
		if (( hErr != ERR_OK) || (lAllTpCount == 0))
		{
			HxLOG_Error("[%s] Ts Condition error!! \n" ,__FUNCTION__);
			goto END_FUNC;
		}

		for ( i = 0; i < lAllTpCount; i++ )
		{
			MGR_SWUPINFO_AddOtaHomeTp (pstTsInfo + i);
		}
		DB_SVC_FreeTsInfoList(pstTsInfo);
		pstTsInfo = NULL;
	}
#endif

#if	defined(CONFIG_MW_CH_CABLE)

	// Hard cording 된 TP가 아니라 Search 된 TP를 Home TP에 Add 하도록 한다. - jhlee
	DB_SVC_InitTsFindCondition(&stCond);
	stCond.nDeliType = eDxDELIVERY_TYPE_CAB;

	hErr = DB_SVC_FindTsInfoList(&stCond, &lAllTpCount, &pstTsInfo);
	if (( hErr != ERR_OK) ||( lAllTpCount == 0))
	{
		HxLOG_Error("[%s] Ts Condition error!! \n",__FUNCTION__);
		goto END_FUNC;
	}

	for ( i = 0; i < lAllTpCount; i++ )
	{
		stTsInfo.eDeliType = eDxDELIVERY_TYPE_CAB;
		stTsInfo.stTuningParam.cab = pstTsInfo[i].stTuningParam.cab;
		MGR_SWUPINFO_AddOtaHomeTp (&stTsInfo);
		HxLOG_Print("add (%d th TP) %dMhz, SR:%d, Netwok ID : %d, Con: %d\n",
			i, pstTsInfo[i].stTuningParam.cab.ulFreq, pstTsInfo[i].stTuningParam.cab.usSR, pstTsInfo[i].stTuningParam.cab.usNetworkId, pstTsInfo[i].stTuningParam.cab.ucCon);
	}

	DB_SVC_FreeTsInfoList(pstTsInfo);
#endif

END_FUNC:
	if(pstTsInfo)
	{
		DB_SVC_FreeTsInfoList(pstTsInfo);
		pstTsInfo = NULL;
	}

	return hErr;
}


