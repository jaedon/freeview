/**
	@file     ap_ota_action_dtt.c
		---> xmgr_sw_update_action_dtt.c
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
#include <mgr_swup.h>
#include <mgr_search.h>
#include <mgr_rscmap.h>
#include <mgr_spec.h>

#include <xmgr_spec.h>
#include <xmgr_swup.h>
//#include "../local_include/_xmgr_swup_loc_param.h"

#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define OTA_TIMER_LOCKING_ID						0x2101
#define OTA_TIMER_SI_ID								0x2102
#define OTA_TIMER_LOCKING_TIME						(4*1000)			// 4sec
#define OTA_TIMER_SI_TIME							(60*1000)			// 60sec

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief search application에서 실행에 필요한 environment context  */
typedef struct tagDttOta_Contents_t
{
	Handle_t					 hSwUpdate;
	HUINT32						 ulActionId;			// Search Application Action ID
	Handle_t					 hAction;				// Search Action handle
	HUINT32						 ulTunerId;				// Tuner ID
	DxDeliveryType_e			 eDeliType;       		// Delivery Type
	MgrSwup_AppState_t			 eAppState;				// Current Search State :
	XsvcSi_SwupMode_e			 eSiSwupMode;			// Current SI Mode

	HBOOL						 bForcedStop;			// OTA Stopped forcely.

	SvcSi_SwupCheckSpec_t		 stOtaCheckSpec;		// OTA Check Spec.

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
} xmgrSwup_DttActionContents_t;


STATIC xmgrSwup_DttActionContents_t	s_stDtt_Contents;

/* internal function prototype */

STATIC xmgrSwup_DttActionContents_t* xmgr_swup_GetContents_Dtt (void);

STATIC INLINE HERROR	xmgr_swup_CheckActionId_Dtt (xmgrSwup_DttActionContents_t *pstContents, Handle_t hAction);
STATIC INLINE void		xmgr_swup_IncreaseSessionVersion_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC void				xmgr_swup_FreeLinkItem_Dtt (void *pvItem);
STATIC void 			xmgr_swup_FreeMessageInfoLinkItem_Dtt (void *pvItem);

STATIC HERROR			xmgr_swup_InitContents_Dtt (xmgrSwup_DttActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSwUpdate);
STATIC HERROR			xmgr_swup_MakeHomeTpList_Dtt (xmgrSwup_DttActionContents_t* pstContents);

STATIC HERROR			xmgr_swup_DestroyContents_Dtt (xmgrSwup_DttActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_FindTsInfoByTripleId_Dtt (xmgrSwup_DttActionContents_t *pstContents, DbSvc_TripleId_t *pstTripleId, DbSvc_TsInfo_t *pstSvcTsInfo);

STATIC HERROR			xmgr_swup_TuneSearchTp_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_SetTuningParam_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_SetNextTpForOta_Dtt (xmgrSwup_DttActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_CountOtaTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents, HUINT32 *pulTpNum);
STATIC HERROR			xmgr_swup_GetOtaTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo);
STATIC void				xmgr_swup_SetSiOtaMode_Dtt (xmgrSwup_DttActionContents_t *pstContents, XsvcSi_SwupMode_e eOtaMode);
STATIC DbSvc_TsInfo_t*	xmgr_swup_GetCurrentHomeTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC void				xmgr_swup_AddHomeTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo);
STATIC void				xmgr_swup_SetNextHomeTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllHomeTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllRefNitLink_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC DbSvc_TripleId_t*	xmgr_swup_GetCurrentSsuScanLink_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC void				xmgr_swup_AddSsuScanLink_Dtt (xmgrSwup_DttActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuScan);
STATIC void				xmgr_swup_SetNextSsuScanLink_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllSsuScanLink_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC DbSvc_TripleId_t*	xmgr_swup_GetCurrentSsuLink_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC void				xmgr_swup_AddSsuLink_Dtt (xmgrSwup_DttActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuLink);
STATIC void				xmgr_swup_SetNextSsuLink_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllSsuLink_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC void 			xmgr_swup_ClearAllTsLink_Dtt (xmgrSwup_DttActionContents_t * pstContents);
STATIC void 			xmgr_swup_ClearSsuDescriptorLink_Dtt (xmgrSwup_DttActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_StartAction_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_StopAction_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_StartSi_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC DbSvc_TsInfo_t*	xmgr_swup_AllocTsInfo_Dtt (void);
STATIC void				xmgr_swup_FreeTsInfo_Dtt (void *pvTsInfo);
STATIC void				xmgr_swup_ReleaseAction_Dtt (xmgrSwup_DttActionContents_t *pstContents);

STATIC void				xmgr_swup_PrintTripleId_Dtt (DbSvc_TripleId_t *pstTripleId);
STATIC void				xmgr_swup_PrintCurrentTuningInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_MsgGwmCreate_Dtt (xmgrSwup_DttActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSwUpdate);
STATIC HERROR			xmgr_swup_MsgGwmDestroyed_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_MsgGwmTimer_Dtt (xmgrSwup_DttActionContents_t *pstContents, HINT32 nTimerId);
STATIC HERROR 			xmgr_swup_EvtSiDescriptorSuccess_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccess_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccessInAnchorTp_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccessInSsuScanTp_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiSearchFail_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtChLocked_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtChNoSignal_Dtt (xmgrSwup_DttActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_MsgGwmQuitStandby_Dtt  (xmgrSwup_DttActionContents_t *pstContents);

#define	_________________________Body___________________________________

STATIC xmgrSwup_DttActionContents_t *xmgr_swup_GetContents_Dtt (void)
{
	return &s_stDtt_Contents;
}

STATIC SvcSi_SwupCheckSpec_t* xmgr_swup_GetOtaSpec_Dtt(void)
{
	STATIC SvcSi_SwupCheckSpec_t 	stOtaSpec;

	HxSTD_MemSet(&stOtaSpec, 0x00, sizeof(SvcSi_SwupCheckSpec_t));
	stOtaSpec.eActionKind = eSVCSI_ACT_KIND_SwupCheck;
	stOtaSpec.ulActionSpec = eSVCSI_ACT_SPEC_DTT;

	return &stOtaSpec;
}

#define	____________________for__UI_____________________


#define _________________________________________________________________________________

STATIC INLINE HERROR xmgr_swup_CheckActionId_Dtt (xmgrSwup_DttActionContents_t *pstContents, Handle_t hAction)
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

STATIC INLINE void xmgr_swup_IncreaseSessionVersion_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

STATIC void xmgr_swup_FreeLinkItem_Dtt (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

STATIC void xmgr_swup_FreeMessageInfoLinkItem_Dtt (void *pvItem)
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


#define	_____CONTENTS_INITIATION_____

STATIC HERROR xmgr_swup_InitContents_Dtt (xmgrSwup_DttActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSwUpdate)
{
	DbSvc_TsInfo_t	*pstTsInfo;
	HERROR			 hErr;

	// 1. Clear all the contents.
	HxSTD_memset (pstContents, 0, sizeof(xmgrSwup_DttActionContents_t));

	// 2. Action Handle and Action ID
	pstContents->hAction	= hAction;
	pstContents->hSwUpdate	= hSwUpdate;

	pstContents->eDeliType	= eDxDELIVERY_TYPE_TER;
	pstContents->ulActionId	= SVC_PIPE_GetActionId (hAction);
	pstContents->ulTunerId	= (HUINT32)nTunerId;

	pstContents->bForcedStop = FALSE;

	HxSTD_memcpy(&pstContents->stOtaCheckSpec, xmgr_swup_GetOtaSpec_Dtt(), sizeof(SvcSi_SwupCheckSpec_t));

	// 3. tuning information :
	// TP List :
	hErr = xmgr_swup_MakeHomeTpList_Dtt (pstContents);
	if (hErr != ERR_OK)					{ return MESSAGE_BREAK; }

	pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Dtt (pstContents);
	if (pstTsInfo != NULL)
	{
		pstContents->eDeliType = pstTsInfo->eDeliType;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_MakeHomeTpList_Dtt (xmgrSwup_DttActionContents_t* pstContents)
{
	HUINT32					 ulTpCnt, ulTpNum;
	DbSvc_TsInfo_t			*pstTsInfo;
	HERROR					 hErr;

	if (pstContents == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	// Get the default TPs and the numbers.
	hErr = xmgr_swup_CountOtaTsInfo_Dtt (pstContents, &ulTpNum);
	if (hErr != ERR_OK || ulTpNum == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
	{
		pstTsInfo = xmgr_swup_AllocTsInfo_Dtt();
		if (pstTsInfo != NULL)
		{
			hErr = xmgr_swup_GetOtaTsInfo_Dtt (pstContents, ulTpCnt, pstTsInfo);
			if (hErr == ERR_OK)
			{
				xmgr_swup_AddHomeTsInfo_Dtt (pstContents, pstTsInfo);
			}
			else
			{
				xmgr_swup_FreeTsInfo_Dtt (pstTsInfo);
			}
		}
	}

	if (xmgr_swup_GetCurrentHomeTsInfo_Dtt (pstContents) == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define	_____CONTENTS_DESTRUCTION_____

STATIC HERROR xmgr_swup_DestroyContents_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	xmgr_swup_ClearAllHomeTsInfo_Dtt (pstContents);
	xmgr_swup_ClearAllRefNitLink_Dtt (pstContents);
	xmgr_swup_ClearAllSsuScanLink_Dtt (pstContents);
	xmgr_swup_ClearAllSsuLink_Dtt (pstContents);
	xmgr_swup_ClearAllTsLink_Dtt(pstContents);
	xmgr_swup_ClearSsuDescriptorLink_Dtt(pstContents);

	return ERR_BUS_NO_ERROR;
}

#define	_____DATA_CONVERSION_____

STATIC HERROR xmgr_swup_FindTsInfoByTripleId_Dtt (xmgrSwup_DttActionContents_t *pstContents, DbSvc_TripleId_t *pstTripleId, DbSvc_TsInfo_t *pstSvcTsInfo)
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

STATIC HERROR xmgr_swup_TuneSearchTp_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	HBOOL				 bRetry;
	SvcRsc_Info_t			 stRscInfo;
	HERROR		 		 hErr;

	bRetry = TRUE;
	MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);

	while (bRetry == TRUE)
	{
		hErr = xmgr_swup_SetTuningParam_Dtt (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("No Tp left to tune\n");

			hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
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


		xmgr_swup_IncreaseSessionVersion_Dtt (pstContents);			// Tuning 전에 반드시 Action Version을 업데이트해야 한다.

		HxLOG_Print("Tune TP  : ");
		xmgr_swup_PrintCurrentTuningInfo_Dtt (pstContents);
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
		hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
			bRetry = FALSE;
		}
	}

	return ERR_BUS_NO_OBJECT;
}

STATIC HERROR xmgr_swup_SetTuningParam_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	DbSvc_TsInfo_t		 stSvcTsInfo;
	DbSvc_TsInfo_t		*pstTsInfo;
	DbSvc_TripleId_t		*pstTripleId;
	HERROR				 hErr;

	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_EUDTT_HOME_TP:
		pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Dtt (pstContents);
		if (pstTsInfo == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		stSvcTsInfo = *pstTsInfo;

		break;

	case eSI_SWUP_MODE_EUDTT_SSUSCAN_TP:
		// SSU SCAN List
		pstTripleId = xmgr_swup_GetCurrentSsuScanLink_Dtt (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		hErr = xmgr_swup_FindTsInfoByTripleId_Dtt (pstContents, pstTripleId, &stSvcTsInfo);
		if (hErr != ERR_BUS_NO_ERROR)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_EUDTT_SSUSVC_TP:
		// SSU LINK List
		pstTripleId = xmgr_swup_GetCurrentSsuLink_Dtt (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		hErr = xmgr_swup_FindTsInfoByTripleId_Dtt (pstContents, pstTripleId, &stSvcTsInfo);
		if (hErr != ERR_BUS_NO_ERROR)
		{
#if 0	//test : 그냥 OTA TS 파일 play해서 테스트하면 필요한 ts info가 없으므로 그냥 home ts를 다시 세팅
			pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Dtt (pstContents);
			if (pstTsInfo == NULL)
			{
				return ERR_BUS_NO_OBJECT;
			}

			stSvcTsInfo = *pstTsInfo;
#else
			return ERR_BUS_NO_OBJECT;
#endif
		}

		break;

	default:
		return ERR_BUS_NO_OBJECT;
	}

	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stSvcTsInfo, &(pstContents->stTuningInfo));

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_SetNextTpForOta_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_EUDTT_HOME_TP:
		xmgr_swup_SetNextHomeTsInfo_Dtt (pstContents);
		if (xmgr_swup_GetCurrentHomeTsInfo_Dtt (pstContents) == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_EUDTT_SSUSCAN_TP:
		xmgr_swup_SetNextSsuScanLink_Dtt (pstContents);
		if (xmgr_swup_GetCurrentSsuScanLink_Dtt (pstContents) == NULL)
		{
			// SSU-BAT TP 가 더 이상 남아 있지 않다 : 다음 Home TP로 가서 작업을 한다.
			xmgr_swup_SetSiOtaMode_Dtt (pstContents, eSI_SWUP_MODE_EUDTT_HOME_TP);
			return xmgr_swup_SetNextTpForOta_Dtt (pstContents);
		}

		break;

	case eSI_SWUP_MODE_EUDTT_SSUSVC_TP:
		xmgr_swup_SetNextSsuLink_Dtt (pstContents);
		if (xmgr_swup_GetCurrentSsuLink_Dtt (pstContents) == NULL)
		{
			// SSU-SVC TP 가 더 이상 남아 있지 않다 : 다음 SSU-BAT TP로 가서 작업을 한다.
			xmgr_swup_SetSiOtaMode_Dtt (pstContents, eSI_SWUP_MODE_EUDTT_SSUSCAN_TP);
			return xmgr_swup_SetNextTpForOta_Dtt (pstContents);
		}

		break;

	default:
		break;
	}

	return ERR_BUS_NO_ERROR;
}


#define	_____MESSAGE_FUNCTIONS_____


#define	_____CONTENTS_DATA_FUNCTIONS_____

STATIC HERROR xmgr_swup_CountOtaTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents, HUINT32 *pulTpNum)
{
	(void)pstContents;

	if (pulTpNum == NULL)					{ return ERR_BUS_TARGET_NULL; }

	*pulTpNum = MGR_SWUPINFO_CountOtaHomeTp();
	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_GetOtaTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR		 hErr;

	(void)pstContents;

	if (pstTsInfo == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = MGR_SWUPINFO_GetOtaHomeTp (ulTpNum, pstTsInfo);
	if (hErr != ERR_OK)						{ return ERR_BUS_OUT_OF_RANGE; }

	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
	return ERR_BUS_NO_ERROR;
}

STATIC void xmgr_swup_SetSiOtaMode_Dtt (xmgrSwup_DttActionContents_t *pstContents, XsvcSi_SwupMode_e eOtaMode)
{
	HxLOG_Print("[xmgr_swup_SetSiOtaMode_Dtt] SI Mode %d -> %d\n", pstContents->eSiSwupMode, eOtaMode);
	pstContents->eSiSwupMode = eOtaMode;
}


// Home Ts Info
STATIC DbSvc_TsInfo_t *xmgr_swup_GetCurrentHomeTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents)
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

STATIC void xmgr_swup_AddHomeTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo)
{
	if (pstContents != NULL && pstHomeTsInfo != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstHomeTpList, (void *)pstHomeTsInfo);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstHomeTpList);
	}
}

STATIC void xmgr_swup_SetNextHomeTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstHomeTpList != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstHomeTpList, pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dtt);
	}
}

STATIC void xmgr_swup_ClearAllHomeTsInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dtt);
	}
}

STATIC void xmgr_swup_ClearAllRefNitLink_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstRefNitList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstRefNitList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dtt);
	}
}


// SSU SCAN LINK : SSU-BAT
STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentSsuScanLink_Dtt (xmgrSwup_DttActionContents_t *pstContents)
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

STATIC void xmgr_swup_AddSsuScanLink_Dtt (xmgrSwup_DttActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuScan)
{
	if (pstContents != NULL && pstSsuScan != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSsuScanList, (void *)pstSsuScan);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstSsuScanList);
	}
}

STATIC void xmgr_swup_SetNextSsuScanLink_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstSsuScanList != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuScanList, pstContents->pstSsuScanList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dtt);
	}
}

STATIC void xmgr_swup_ClearAllSsuScanLink_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuScanList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dtt);
	}
}

// SSU LINK : SSU-SVC
STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentSsuLink_Dtt (xmgrSwup_DttActionContents_t *pstContents)
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

STATIC void xmgr_swup_AddSsuLink_Dtt (xmgrSwup_DttActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuLink)
{
	if (pstContents != NULL && pstSsuLink != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSsuLinkList, (void *)pstSsuLink);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstSsuLinkList);
	}
}

STATIC void xmgr_swup_SetNextSsuLink_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstSsuLinkList != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuLinkList, pstContents->pstSsuLinkList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dtt);
	}
}

STATIC void xmgr_swup_ClearAllSsuLink_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuLinkList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dtt);
	}
}

STATIC void xmgr_swup_ClearAllTsLink_Dtt (xmgrSwup_DttActionContents_t * pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTsInfoListFromSi, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dtt);
	}
}

STATIC void xmgr_swup_ClearSsuDescriptorLink_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
//	SvcSi_SsuMessageInfo_t	*pstMessageInfo;

	if (pstContents != NULL)
	{
		if(pstContents->pstMessageInfo)
		{
			pstContents->pstMessageInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstMessageInfo, (FREE_FUNC)xmgr_swup_FreeMessageInfoLinkItem_Dtt);
		}

		if(pstContents->pstUpdateInfo)
		{
			pstContents->pstUpdateInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstUpdateInfo, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dtt);
		}

		if(pstContents->pstScheduleInfo)
		{
			pstContents->pstScheduleInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstScheduleInfo, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dtt);
		}
	}
}

#define	_____PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_swup_StartAction_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.
	HERROR			 hErr;

	xmgr_swup_SetSiOtaMode_Dtt (pstContents, eSI_SWUP_MODE_EUDTT_HOME_TP);

	// 1. Set the TP to tuning :
	hErr = xmgr_swup_SetTuningParam_Dtt (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	// 2. Get the Action : DVB-SSU OTA는 Search와 다를 바가 없으므로 Action은 Search 로써 잡아도 된다.
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
	hErr = xmgr_swup_TuneSearchTp_Dtt (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_StopAction_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	if (pstContents->eAppState == eOtaApp_state_waitingSi)
	{
		// stop sw update detect
		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
	}

	if (pstContents->eAppState != eOtaApp_state_init)
	{
HxLOG_Print("[%s](%d) Keep action until Connecting module is ready!!!\n", __FUNCTION__, __LINE__);
HxLOG_Print("[%s](%d) Keep action until Connecting module is ready!!!\n", __FUNCTION__, __LINE__);
HxLOG_Print("[%s](%d) Keep action until Connecting module is ready!!!\n", __FUNCTION__, __LINE__);
		xmgr_swup_ReleaseAction_Dtt (pstContents);
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_StartSi_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	HUINT16			 usExtId1, usExtId2;
	DbSvc_TripleId_t	*pstTripleId;
	SvcSi_SwupCheckOption_t	stOtaCheckOption;

	usExtId1 = 0;
	usExtId2 = 0;

	// SSU Link에 있는 SVCID가 필요하다.
	if (pstContents->eSiSwupMode == eSI_SWUP_MODE_EUDTT_SSUSVC_TP)
	{
		pstTripleId = xmgr_swup_GetCurrentSsuLink_Dtt (pstContents);
		if (pstTripleId != NULL)
		{
			usExtId1 = pstTripleId->usSvcId;
		}
	}

	stOtaCheckOption.ulSwupMode = eSI_SWUP_MODE_EUDTT_HOME_TP + abs(pstContents->eSiSwupMode - eSI_SWUP_MODE_EUDTT_HOME_TP);
	stOtaCheckOption.usExtId1 = usExtId1;
	stOtaCheckOption.usExtId2 = usExtId2;

	HxLOG_Print("[%s, %d] eOtaMode(0x%x) ExtId1(0x%x) ExtId2(0x%x)\n,",
		__FUNCTION__, __LINE__, stOtaCheckOption.ulSwupMode, stOtaCheckOption.usExtId1, stOtaCheckOption.usExtId2 );

	return SVC_SI_StartSwupCheck(pstContents->hAction, &pstContents->stOtaCheckSpec, &stOtaCheckOption, FALSE);

}

STATIC DbSvc_TsInfo_t *xmgr_swup_AllocTsInfo_Dtt (void)
{
	return (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
}

STATIC void xmgr_swup_FreeTsInfo_Dtt (void *pvTsInfo)
{
	if (pvTsInfo != NULL)
	{
		OxAP_Free (pvTsInfo);
	}
}

STATIC void xmgr_swup_ReleaseAction_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	HERROR			 hErr;

	/* active standby mode에서 shutdown process의 stop ch이 다소 지연될때 searc action이 rerlease되지 않은체 corsair proc이가 service start하게 되면
	    av play안되는 문제가 있다. 일단 근본적인 해결이 되기전까지는 Async mode로 CH stop한다. by MOON */
#if (0)
	hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
#else
	hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eAsyncMode);
#endif
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n\n");
	}

	// Release the action :
	MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
}


#define _____DEBUG_FUNCTIONS_____

STATIC void	xmgr_swup_PrintTripleId_Dtt (DbSvc_TripleId_t *pstTripleId)
{
#if defined(CONFIG_DEBUG)
	HxLOG_Print("(%04x:%04x:%04x)", pstTripleId->usOnId, pstTripleId->usTsId, pstTripleId->usSvcId);
#else
	(void)pstTripleId;
#endif
}

STATIC void xmgr_swup_PrintCurrentTuningInfo_Dtt (xmgrSwup_DttActionContents_t *pstContents)
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
STATIC HERROR xmgr_swup_MsgGwmCreate_Dtt (xmgrSwup_DttActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSwUpdate)
{
	HERROR		 hErr;

	// Create시 해 주어야 하는 Initialize 등이 실패할 경우에는 이 Action 자체가 살아 있어서는 안된다.
	// 곧바로 Destroy 해 주도록 한다.

#if defined(CONFIG_OP_AUS_DTT)
	// RF OTA는 지원하지 않으므로 바로 detect fail로 처리해줌
	MGR_SWUP_PostMessage(hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
	BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, hSwUpdate, 0, 0);

	BUS_MGR_Destroy (NULL);
	return ERR_BUS_NO_ERROR;

#else

	// 1. Initialize the contents
	hErr = xmgr_swup_InitContents_Dtt (pstContents, hAction, nTunerId, hSwUpdate);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
		return hErr;
	}

	// 2. Start the OTA Action :
	hErr = xmgr_swup_StartAction_Dtt (pstContents);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0, eSWUP_FAILREASON_Action);
		return hErr;
	}

	MGR_SWUPINFO_ResetSearchedInfo();

	return ERR_BUS_NO_ERROR;

#endif
}

// eMEVT_BUS_DESTROY:
STATIC HERROR xmgr_swup_MsgGwmDestroyed_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	// Stop all the Action
	xmgr_swup_StopAction_Dtt (pstContents);

	// Destroy the contents
	xmgr_swup_DestroyContents_Dtt (pstContents);

	// Inform that the OTA action is destroyed
	if (pstContents->bForcedStop != TRUE)
	{
//		BUS_PostMessage (NULL, MSG_APP_OTA_ACTION_DESTROYED, (Handle_t)NULL, (HINT32)BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()), 0, 0);
	}

	return MESSAGE_BREAK;
}

// eMEVT_BUS_TIMER:
STATIC HERROR xmgr_swup_MsgGwmTimer_Dtt (xmgrSwup_DttActionContents_t *pstContents, HINT32 nTimerId)
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
			hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Dtt (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_MsgGwmTimer_Dtt] No Next Tp left to tune\n");
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
				hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_swup_TuneSearchTp_Dtt (pstContents);
				}
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_MsgGwmTimer_Dtt] Cannot proceed anymore\n");
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
		// detect 보내기..
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, pstContents->hSwUpdate, 0, 0);
	}

	return hRetValue;
}


// EVT_SI_OTA_DESCRIPTOR_SUCCESS:
STATIC HERROR xmgr_swup_EvtSiDescriptorSuccess_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	HERROR			hErr;
	HUINT32 			ulDataNum, ulIndex;
	SvcSi_SsuMessageInfo_t	*pstMessageInfo, *pvMessageInfoArray;
	SvcSi_SsuUpdateInfo_t	*pstUpdateInfo, *pvUpdateInfoArray;
	SvcSi_SsuScheduleInfo_t	*pstScheduleInfo, *pvScheduleInfoArray;

	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_MESSAGE_INFO, &ulDataNum, (void **)&pvMessageInfoArray);
	if (hErr == ERR_OK && pvMessageInfoArray != NULL)
	{
		HxLOG_Print("[xmgr_swup_EvtSiDescriptorSuccess_Dtt] eSI_SWUP_DATA_SSU_MESSAGE_INFO: %d\n", ulDataNum);

		for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
		{
			pstMessageInfo = (SvcSi_SsuMessageInfo_t *)OxAP_Malloc (sizeof(SvcSi_SsuMessageInfo_t));
			if (pstMessageInfo != NULL)
			{
				*pstMessageInfo = pvMessageInfoArray[ulIndex];
				if(pvMessageInfoArray[ulIndex].pucText != NULL)
				{
					pstMessageInfo->pucText =(HUINT8 *) OxSI_Malloc(sizeof(HUINT8) *pvMessageInfoArray[ulIndex].usTextLen);
					if(pstMessageInfo->pucText !=NULL)
					{
						HxSTD_memcpy(pstMessageInfo->pucText, pvMessageInfoArray[ulIndex].pucText, sizeof(HUINT8) * (pvMessageInfoArray[ulIndex].usTextLen) );
					}
				}

				pstContents->pstMessageInfo= UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstMessageInfo, (void *)pstMessageInfo);
			}
		}

		for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
		{
			if(pvMessageInfoArray[ulIndex].pucText)
			{
				OxAP_Free(pvMessageInfoArray[ulIndex].pucText);
			}
		}
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_MESSAGE_INFO, (void *)pvMessageInfoArray);
	}

	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_UPDATE_INFO, &ulDataNum, (void **)&pvUpdateInfoArray);
	if (hErr == ERR_OK && pvUpdateInfoArray != NULL)
	{
		HxLOG_Print("[xmgr_swup_EvtSiDescriptorSuccess_Dtt] eSI_SWUP_DATA_SSU_UPDATE_INFO: %d\n", ulDataNum);

		for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
		{
			pstUpdateInfo = (SvcSi_SsuUpdateInfo_t *)OxAP_Malloc (sizeof(SvcSi_SsuUpdateInfo_t));
			if (pstUpdateInfo != NULL)
			{
				*pstUpdateInfo = pvUpdateInfoArray[ulIndex];
				pstContents->pstUpdateInfo= UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstUpdateInfo, (void *)pstUpdateInfo);
			}
		}
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_UPDATE_INFO, (void *)pvUpdateInfoArray);
	}

	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCHEDULE_INFO, &ulDataNum, (void **)&pvScheduleInfoArray);
	if (hErr == ERR_OK && pvScheduleInfoArray != NULL)
	{
		HxLOG_Print("[xmgr_swup_EvtSiDescriptorSuccess_Dtt] eSI_SWUP_DATA_SSU_SCHEDULE_INFO: %d\n", ulDataNum);

		for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
		{
			pstScheduleInfo = (SvcSi_SsuScheduleInfo_t *)OxAP_Malloc (sizeof(SvcSi_SsuScheduleInfo_t));
			if (pstScheduleInfo != NULL)
			{
				*pstScheduleInfo = pvScheduleInfoArray[ulIndex];
				pstContents->pstScheduleInfo= UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstScheduleInfo, (void *)pstScheduleInfo);
			}
		}
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCHEDULE_INFO, (void *)pvScheduleInfoArray);
	}

	return MESSAGE_BREAK;
}

// eSEVT_SWUP_DETECT_SUCCESS:
STATIC HERROR xmgr_swup_EvtSiOtaSuccess_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	HERROR				 hErr = ERR_OK;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_waitingSi:
		// SI succeeded : Kill the timer

		BUS_KillTimer (OTA_TIMER_SI_ID);

		pstContents->eAppState = eOtaApp_state_siReceived;

		switch (pstContents->eSiSwupMode)
		{
		case eSI_SWUP_MODE_EUDTT_HOME_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInAnchorTp_Dtt (pstContents);
			break;

		case eSI_SWUP_MODE_EUDTT_SSUSCAN_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInSsuScanTp_Dtt (pstContents);
			break;

		case eSI_SWUP_MODE_EUDTT_SSUSVC_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Dtt (pstContents);
			break;

		default:
			break;
		}

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		if (pstContents->eAppState != eOtaApp_state_checkend)
		{
			hErr = xmgr_swup_TuneSearchTp_Dtt (pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_EvtSiSearchDone] No Next Tp left to tune\n");
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

		// SSUSVC Type 인 경우에 OTA info 가 존재하면 eSWUP_STATE_Error 를 보내면 안됨...
		if((pstContents->eSiSwupMode != eSI_SWUP_MODE_EUDTT_SSUSVC_TP)
			|| ((pstContents->eSiSwupMode == eSI_SWUP_MODE_EUDTT_SSUSVC_TP) && (hErr != ERR_OK)))
		{
			MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
			BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, pstContents->hSwUpdate, 0, 0);
		}
	}

	return MESSAGE_BREAK;
}



STATIC HERROR xmgr_swup_EvtSiOtaSuccessInAnchorTp_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	HBOOL					 bSiDataFound;
	HUINT32					 ulIndex, ulDataNum;
	XsvcSi_SwupMode_e	 eNextOtaMode;
	DbSvc_TsInfo_t			*pstHomeTsInfo;
	DbSvc_TsInfo_t			*pstTsInfo, *pstTsInfoArray;
	DbSvc_TripleId_t		*pstTripleId, *pstTripleIdArray;
	HERROR					 hErr;

	bSiDataFound = FALSE;

	pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Dtt (pstContents);

	if (pstHomeTsInfo != NULL)
	{
		pstContents->ulAntennaIndex = (HUINT32)pstHomeTsInfo->stTuningParam.sat.ucAntId;

		hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, &ulDataNum, (void **)&pstTsInfoArray);
		if (hErr == ERR_OK && pstTsInfoArray != NULL)
		{
			HxLOG_Print("[dttota] NIT TS Info : %d\n", ulDataNum);

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

					HxLOG_Print("[dttota] TP (%d) ", ulIndex);
					XMGR_SWUP_PrintTuningInfo(pstTsInfo->eDeliType, &pstTsInfo->stTuningParam);
					HxLOG_Print("\n");

					pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTsInfoListFromSi, (void *)pstTsInfo);
				}
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, (void *)pstTsInfoArray);
		}

		// SSU Scan Triple ID
		hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCAN_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
		if (hErr == ERR_OK && pstTripleIdArray != NULL)
		{
			HxLOG_Print("[dttota] REF-NIT : %d (%s %d lines)\n", ulDataNum, __FUNCTION__, __LINE__);

			bSiDataFound = TRUE;
			eNextOtaMode = eSI_SWUP_MODE_EUDTT_SSUSCAN_TP;

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
				if (pstTripleId != NULL)
				{
					*pstTripleId = pstTripleIdArray[ulIndex];
					xmgr_swup_AddSsuScanLink_Dtt (pstContents, pstTripleId);

					HxLOG_Print("[dttota] REF-NIT (%d) ", ulIndex);
					xmgr_swup_PrintTripleId_Dtt (pstTripleId);
					HxLOG_Print("\n");
				}
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCAN_TRIPLEID, (void *)pstTripleIdArray);
		}
		else
		{
			// SSU Link Triple ID
			hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
			if (hErr == ERR_OK && pstTripleIdArray != NULL)
			{
				HxLOG_Print("[dttota] REF-NIT : %d (%s %d lines)\n", ulDataNum, __FUNCTION__, __LINE__);

				bSiDataFound = TRUE;
				eNextOtaMode = eSI_SWUP_MODE_EUDTT_SSUSVC_TP;

				for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
				{
					pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
					if (pstTripleId != NULL)
					{
						*pstTripleId = pstTripleIdArray[ulIndex];
						xmgr_swup_AddSsuLink_Dtt (pstContents, pstTripleId);

						HxLOG_Print("[dttota] REF-NIT (%d) ", ulIndex);
						xmgr_swup_PrintTripleId_Dtt (pstTripleId);
						HxLOG_Print("\n");
					}
				}
				SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, (void *)pstTripleIdArray);
			}
		}
	}

	if (bSiDataFound == TRUE)
	{
		xmgr_swup_SetSiOtaMode_Dtt (pstContents, eNextOtaMode);
	}
	else
	{
		hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
		if (hErr != ERR_OK)			// 모든 TP를 다 체크함
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSsuScanTp_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	HBOOL				 bSiDataFound;
	HUINT32				 ulIndex, ulDataNum;
	DbSvc_TripleId_t		*pstTripleId, *pstTripleIdArray;
	HERROR				 hErr;

	bSiDataFound = FALSE;

	// SSU-SVC
	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
	if (hErr == ERR_OK && pstTripleIdArray != NULL)
	{
		HxLOG_Print("[dttota] SSU-SVC : %d\n", ulDataNum);

		bSiDataFound = TRUE;

		for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
		{
			pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
			if (pstTripleId != NULL)
			{
				*pstTripleId = pstTripleIdArray[ulIndex];
				xmgr_swup_AddSsuLink_Dtt (pstContents, pstTripleId);

				HxLOG_Print("[dttota] SSU-SVC (%d) ", ulIndex);
				xmgr_swup_PrintTripleId_Dtt (pstTripleId);
				HxLOG_Print("\n");
			}
		}
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, (void *)pstTripleIdArray);
	}

	if (bSiDataFound == TRUE)
	{
		xmgr_swup_SetSiOtaMode_Dtt (pstContents, eSI_SWUP_MODE_EUDTT_SSUSVC_TP);
	}
	else
	{
		hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
		if (hErr != ERR_OK)			// 모든 TP를 다 체크함
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	HBOOL				 bSiDataFound;
	HUINT32				 ulDataNum;
	SvcSi_SwupSignal_t	 *pstOtaArray, *pstOtaInfo;
	DxTuneParam_t		 stChTuneInfo;
	DbSvc_TsInfo_t		 *pstHomeTsInfo;
	DbSvc_TripleId_t	 *pstTripleId;
	DbSvc_TsInfo_t		 *pstOtaTsInfo;
	HERROR				 hErr, hResult = ERR_BUS_NO_ERROR;
	SvcSwUpdate_DetectResult_t	*pstDetectResult = NULL;

	bSiDataFound = TRUE;

	// SSU-SVC
	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SIGNAL, &ulDataNum, (void **)&pstOtaArray);
	if (hErr == ERR_OK && pstOtaArray != NULL)
	{
		// Write OTA Signal Info
		pstOtaInfo = (SvcSi_SwupSignal_t*)HLIB_STD_MemAlloc(sizeof(SvcSi_SwupSignal_t));
		if(pstOtaInfo == NULL)
		{
			hResult = ERR_FAIL;
			goto END_FUNC;
		}

		HxSTD_memset(pstOtaInfo, 0, sizeof(SvcSi_SwupSignal_t));
		HxSTD_memcpy(pstOtaInfo, &(pstOtaArray[0]), sizeof(SvcSi_SwupSignal_t));

		// 찾은 OTA 정보를 기록한다.
		HxLOG_Print("[dttota] Found OTA Info \n");
		HxLOG_Print("[dttota] System ID : 0x%08x ~ 0x%08x \n", pstOtaInfo->ulStartSystemId, pstOtaInfo->ulEndSystemId);
		HxLOG_Print("[dttota] System Ver : 0x%08x \n", pstOtaInfo->ulDataVersion);
		HxLOG_Print("[dttota] PID : %04x \n", pstOtaInfo->usPid);

		pstOtaTsInfo = (DbSvc_TsInfo_t*)HLIB_STD_MemAlloc(sizeof(DbSvc_TsInfo_t));
		if(pstOtaTsInfo == NULL)
		{
			hResult = ERR_FAIL;
			goto END_FUNC;
		}

		// Write OTA TS Info
		HxSTD_memset(pstOtaTsInfo, 0, sizeof(DbSvc_TsInfo_t));

		hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
		if (hErr != ERR_OK)
		{
			stChTuneInfo = pstContents->stTuningInfo;
		}

		OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&stChTuneInfo, &pstOtaTsInfo->stTuningParam);

		pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Dtt (pstContents);
		if (pstHomeTsInfo != NULL)
		{
			pstOtaTsInfo->eDeliType					= pstHomeTsInfo->eDeliType;
			pstOtaTsInfo->ucTunerId					= pstHomeTsInfo->ucTunerId;
			pstOtaTsInfo->stTuningParam.sat.ucAntId	= pstHomeTsInfo->stTuningParam.sat.ucAntId;
		}

		pstTripleId = xmgr_swup_GetCurrentSsuLink_Dtt (pstContents);
		if (pstTripleId != NULL)
		{
			pstOtaTsInfo->usOnId = pstTripleId->usOnId;
			pstOtaTsInfo->usTsId = pstTripleId->usTsId;
		}

		MGR_SWUPINFO_SetSearchedInfo(pstOtaTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)pstOtaInfo);

		pstDetectResult = (SvcSwUpdate_DetectResult_t *)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
		if (pstDetectResult == NULL)
		{
			HxLOG_Error("Memory alloc fail!!\n");
			hResult = ERR_FAIL;
			goto END_FUNC;
		}

		pstDetectResult->bAvailable = TRUE;
		pstDetectResult->bFound	= TRUE;

		// emergency set
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stChTuningInfo, &stChTuneInfo, sizeof(DxTuneParam_t));
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stSvcTsInfo, pstOtaTsInfo, sizeof(DbSvc_TsInfo_t));
		pstDetectResult->data.stRfInfo.eDeliType	 =	eDxDELIVERY_TYPE_SAT;
		pstDetectResult->data.stRfInfo.usPid		 =	pstOtaInfo->usPid;
		pstDetectResult->data.stRfInfo.eDownloadType = eSWUP_DOWNLOAD_DVBSSU;

		pstDetectResult->ulDataVersion	= pstOtaInfo->ulDataVersion;
		pstDetectResult->usOtaPid 		= pstOtaInfo->usPid;
		pstDetectResult->eSource		= eDxSWUPDATE_SOURCE_Rf;
		MGR_SWUP_SetDetectInfo(pstContents->hSwUpdate, pstDetectResult);

		// detect 보내기..
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_SUCCESS, eSWUP_STATE_Unknown, (HINT32)pstOtaTsInfo,(HINT32)pstOtaInfo,0);
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_SUCCESS, HANDLE_NULL, pstContents->hSwUpdate, (HINT32)pstOtaTsInfo,(HINT32)pstOtaInfo);
		pstContents->eAppState = eOtaApp_state_checkend;
	}
	else
	{
		hResult = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(pstOtaArray)
	{
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SIGNAL, (void *)pstOtaArray);
	}

	if(pstDetectResult != NULL)
	{
		HLIB_STD_MemFree(pstDetectResult);
		pstDetectResult = NULL;
	}

	if(pstOtaInfo)
	{
		HLIB_STD_MemFree(pstOtaInfo);
		pstOtaInfo = NULL;
	}

	if(pstOtaTsInfo)
	{
		HLIB_STD_MemFree(pstOtaTsInfo);
		pstOtaInfo = NULL;
	}

	if(hResult == ERR_FAIL)
	{
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);

		hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
		if (hErr != ERR_OK) 		// 모든 TP를 다 체크함
		{
			HxLOG_Print("[%s] all tp checked & set eapp state to eOtaApp_state_checkend\n", __FUNCTION__);
			pstContents->eAppState = eOtaApp_state_checkend;
		}
	}

	return hErr;
}

// eSEVT_SI_SEARCH_FAIL:
STATIC HERROR xmgr_swup_EvtSiSearchFail_Dtt (xmgrSwup_DttActionContents_t *pstContents)
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
		hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_swup_TuneSearchTp_Dtt (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_swup_EvtSiSearchFail_Dtt] No Next Tp left to tune\n");
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

		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, pstContents->hSwUpdate, 0, 0);
	}

	return MESSAGE_BREAK;
}

// eSEVT_CH_LOCKED
STATIC HERROR xmgr_swup_EvtChLocked_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
		case eOtaApp_state_tryTuning :
			// Channel Locked : Kill the timer
			BUS_KillTimer (OTA_TIMER_LOCKING_ID);

			// Start the SI for the DVB-SSU OTA
			hErr = xmgr_swup_StartSi_Dtt (pstContents);
			if (hErr == ERR_OK)
			{
				pstContents->eAppState = eOtaApp_state_waitingSi;
				// MW SI를 무한으로 기다리는 것을 막기 위해 여기에서 Timer를 걸어야 한다.
				BUS_SetTimer (OTA_TIMER_SI_ID, OTA_TIMER_SI_TIME);
			}
			else
			{
				// 다음 TP 로 넘어가야 한다.
				hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_swup_TuneSearchTp_Dtt (pstContents);
				}

				if (hErr != ERR_OK)
				{
					HxLOG_Print("[xmgr_swup_EvtChLocked_Dtt] No Next Tp left to tune\n");
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
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, pstContents->hSwUpdate, 0, 0);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_EvtChNoSignal_Dtt (xmgrSwup_DttActionContents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_tryTuning :
		// Channel Locked : Kill the timer

		BUS_KillTimer (OTA_TIMER_LOCKING_ID);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_swup_TuneSearchTp_Dtt (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_swup_EvtChNoSignal_Dtt] No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
		}

		break;

	case eOtaApp_state_waitingSi:
		hErr = SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
		if (hErr == ERR_OK)
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_SetNextTpForOta_Dtt (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Dtt (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_EvtChNoSignal_Dtt] No Next Tp left to tune\n");
				pstContents->eAppState = eOtaApp_state_checkend;
			}
		}
		else
		{
			// 더 이상 진행 불가능 : Search End로 간주한다.
			HxLOG_Print("[xmgr_swup_EvtChNoSignal_Dtt] SI Re-open failed.\n");
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
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
		BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, pstContents->hSwUpdate, 0, 0);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_MsgGwmQuitStandby_Dtt  (xmgrSwup_DttActionContents_t *pstContents)
{
	pstContents->bForcedStop = TRUE;
	BUS_MGR_Destroy (NULL);

	return ERR_OK;
}

#define	_____EXTERNAL_PROCESS_FUNCTION_____

BUS_Result_t xproc_swup_Detect_Dtt(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	STATIC HBOOL			 s_bFirstStart = TRUE;
	xmgrSwup_DttActionContents_t		*pstContents;
	HERROR			 hErr;

#if defined(CONFIG_DEBUG)

#endif

	pstContents = xmgr_swup_GetContents_Dtt();
	if (s_bFirstStart == TRUE)
	{
		s_bFirstStart = FALSE;
		HxSTD_memset (pstContents, 0, sizeof(xmgrSwup_DttActionContents_t));
	}


	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Print("\t[eMEVT_BUS_CREATE]\n\n");
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_swup_Detect_Dtt");
		hErr = xmgr_swup_MsgGwmCreate_Dtt (pstContents, hAction, p1, p2);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Print("\t[eMEVT_BUS_TIMER]\n\n");
		hErr = xmgr_swup_MsgGwmTimer_Dtt (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Print("\t[eMEVT_BUS_DESTROY]\n\n");
		hErr = xmgr_swup_MsgGwmDestroyed_Dtt (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_PRIVATE_DATA_FOUND:
		hErr = xmgr_swup_CheckActionId_Dtt (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[eSEVT_SI_DESCRIPTOR_SUCCESS]\n\n");

		hErr = xmgr_swup_EvtSiDescriptorSuccess_Dtt (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_SUCCESS:
		hErr = xmgr_swup_CheckActionId_Dtt (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[eSEVT_SWUP_DETECT_SUCCESS]\n\n");

		hErr = xmgr_swup_EvtSiOtaSuccess_Dtt (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_FAIL:
		hErr = xmgr_swup_CheckActionId_Dtt (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[eSEVT_SWUP_DETECT_FAIL]\n\n");

		hErr = xmgr_swup_EvtSiSearchFail_Dtt (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_LOCKED:
		hErr = xmgr_swup_CheckActionId_Dtt (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[eSEVT_CH_LOCKED]\n\n");

		hErr = xmgr_swup_EvtChLocked_Dtt (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_NO_SIGNAL:
		hErr = xmgr_swup_CheckActionId_Dtt (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[eSEVT_CH_NO_SIGNAL]\n\n");

		hErr = xmgr_swup_EvtChNoSignal_Dtt (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_GO_OPERATION :
		HxLOG_Print("\t[eMEVT_BUS_GO_OPERATION]\n\n");
		hErr = xmgr_swup_MsgGwmQuitStandby_Dtt (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;


	default:
		break;
	}
#if defined(CONFIG_DEBUG)

#endif
	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


#define _____OTA_TP_____
HERROR xmgr_swup_SetDefaultHomeTsInfo_Dtt (void)
{
	HERROR					  hErr;
	HUINT32					  ulCount, ulNumTs;
	DbSvc_TsInfo_t			 *astTsInfo;
	DxOperator_e			  eOperator = eDxOPERATOR_None;

	MGR_SWUPINFO_ResetHomeTp();

	MGR_SPEC_GetOperatorCode(&eOperator);

	ulNumTs = 0;
	astTsInfo = NULL;
	hErr = MGR_TPLIST_GetHomeTsList(eOperator, eHomeTs_OTA, &astTsInfo, &ulNumTs);
	if (NULL == astTsInfo || 0 == ulNumTs)
	{
		return ERR_FAIL;
	}

	hErr = ERR_OK;
	for (ulCount=0; ulCount<ulNumTs; ulCount++)
	{
		hErr |= MGR_SWUPINFO_AddOtaHomeTp (&astTsInfo[ulCount]);
	}

	HLIB_STD_MemFree(astTsInfo);

	return hErr;
}

/* End of File. ---------------------------------------------------------- */


