/**
	@file     ap_ota_action_satdvbssu.c
		---> xmgr_sw_update_action_satdvbssu.c
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

#include <mgr_spec.h>
#include <mgr_tplist.h>
#include <mgr_swup.h>
#include <mgr_search.h>
#include <mgr_rscmap.h>

#include <xmgr_spec.h>
#include <xmgr_swup.h>
//#include <xmgr_sw_update_dvbssu.h>
//#include "../local_include/_xmgr_swup_loc_param.h"
#include "_xmgr_swup_dvbssu.h"

#include "../local_include/_xmgr_swup.h"

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

enum
{
	DETECT_FAIL,
	DETECT_SUCCESS,
};


/** @brief search application에서 실행에 필요한 environment context  */
typedef struct tagDvbSsuOta_Contents_t
{
	Handle_t					 hSWUpdate;
	HUINT32						 ulActionId;			// Search Application Action ID
	Handle_t					 hAction;				// Search Action handle
	HUINT32						 ulTunerId;				// Tuner ID
	HUINT16						 usTsUniqId;			// TS Uniq IDX
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
} xmgrSwup_DvbssuActionContents_t;


STATIC xmgrSwup_DvbssuActionContents_t	s_stDvbSsu_Contents;

/* internal function prototype */

STATIC xmgrSwup_DvbssuActionContents_t* xmgr_swup_GetContents_Dvbssu (void);

STATIC INLINE HERROR	xmgr_swup_CheckActionId_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, Handle_t hAction);
STATIC INLINE void		xmgr_swup_IncreaseSessionVersion_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC void				xmgr_swup_FreeLinkItem_Dvbssu (void *pvItem);
STATIC void 			xmgr_swup_FreeMessageInfoLinkItem_Dvbssu (void *pvItem);

STATIC HERROR			xmgr_swup_InitContents_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate);
STATIC HERROR			xmgr_swup_MakeHomeTpList_Dvbssu (xmgrSwup_DvbssuActionContents_t* pstContents);

STATIC HERROR			xmgr_swup_DestroyContents_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_FindTsInfoByTripleId_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, DbSvc_TripleId_t *pstTripleId, DbSvc_TsInfo_t *pstSvcTsInfo);

STATIC HERROR			xmgr_swup_TuneSearchTp_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_SetTuningParam_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_SetNextTpForOta_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_CountOtaTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, HUINT32 *pulTpNum);
STATIC HERROR			xmgr_swup_GetOtaTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo);
STATIC void				xmgr_swup_SetSiOtaMode_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, XsvcSi_SwupMode_e eOtaMode);
STATIC DbSvc_TsInfo_t*	xmgr_swup_GetCurrentHomeTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC void				xmgr_swup_AddHomeTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo);
STATIC void				xmgr_swup_SetNextHomeTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllHomeTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllRefNitLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC DbSvc_TripleId_t*	xmgr_swup_GetCurrentSsuScanLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC void				xmgr_swup_AddSsuScanLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuScan);
STATIC void				xmgr_swup_SetNextSsuScanLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllSsuScanLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC DbSvc_TripleId_t*	xmgr_swup_GetCurrentSsuLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC void				xmgr_swup_AddSsuLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuLink);
STATIC void				xmgr_swup_SetNextSsuLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllSsuLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC void 			xmgr_swup_ClearAllTsLink_Dvbssu (xmgrSwup_DvbssuActionContents_t * pstContents);
STATIC void 			xmgr_swup_ClearSsuDescriptorLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_StartAction_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_StopAction_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_StartSi_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC DbSvc_TsInfo_t*	xmgr_swup_AllocTsInfo_Dvbssu (void);
STATIC void				xmgr_swup_FreeTsInfo_Dvbssu (void *pvTsInfo);
STATIC void				xmgr_swup_ReleaseAction_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);

STATIC void				xmgr_swup_PrintTripleId_Dvbssu (DbSvc_TripleId_t *pstTripleId);
STATIC void				xmgr_swup_PrintCurrentTuningInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);

STATIC HERROR			xmgr_swup_MsgGwmCreate_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate);
STATIC HERROR			xmgr_swup_MsgGwmDestroyed_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_MsgGwmTimer_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, HINT32 nTimerId);
STATIC HERROR 			xmgr_swup_EvtSiDescriptorSuccess_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccess_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccessInAnchorTp_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccessInSsuScanTp_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiSearchFail_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, HINT32 nFailReason);
STATIC HERROR			xmgr_swup_EvtChLocked_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtChNoSignal_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_MsgGwmQuitStandby_Dvbssu  (xmgrSwup_DvbssuActionContents_t *pstContents);
STATIC HERROR 			xmgr_swup_SwDetectFinish_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, HUINT8 ucDetectResult, HINT32 *pstOtaTsInfo, HINT32 *pstOtaInfo, MgrSwup_FailReason_e eFailReason);

#define	_________________________Body___________________________________

STATIC xmgrSwup_DvbssuActionContents_t *xmgr_swup_GetContents_Dvbssu (void)
{
	return &s_stDvbSsu_Contents;
}

#define	____________________for__UI_____________________


#define _________________________________________________________________________________

STATIC INLINE HERROR xmgr_swup_CheckActionId_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, Handle_t hAction)
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

STATIC INLINE void xmgr_swup_IncreaseSessionVersion_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

STATIC void xmgr_swup_FreeLinkItem_Dvbssu (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

STATIC void xmgr_swup_FreeMessageInfoLinkItem_Dvbssu (void *pvItem)
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

STATIC SvcSi_SwupCheckSpec_t* xmgr_swup_GetOtaSpec_Dvbssu(void)
{
	STATIC SvcSi_SwupCheckSpec_t 	stOtaSpec;

	HxSTD_MemSet(&stOtaSpec, 0x00, sizeof(SvcSi_SwupCheckSpec_t));
	stOtaSpec.eActionKind = eSVCSI_ACT_KIND_SwupCheck;
	stOtaSpec.ulActionSpec = eSVCSI_ACT_SPEC_DvbSsu;

	return &stOtaSpec;
}


#define	_____CONTENTS_INITIATION_____

STATIC HERROR xmgr_swup_InitContents_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate)
{
	DbSvc_TsInfo_t	*pstTsInfo;
	HERROR			 hErr;

	// 1. Clear all the contents.
	HxSTD_memset (pstContents, 0, sizeof(xmgrSwup_DvbssuActionContents_t));

	// 2. Action Handle and Action ID
	pstContents->hAction	= hAction;
	pstContents->hSWUpdate	= hSWUpdate;

#if defined(CONFIG_MW_CH_SATELLITE)
	pstContents->eDeliType	= eDxDELIVERY_TYPE_SAT;
#elif defined(CONFIG_MW_CH_SATELLITE)
	pstContents->eDeliType	= eDxDELIVERY_TYPE_TER;
#elif defined(CONFIG_MW_CH_CABLE)
	pstContents->eDeliType	= eDxDELIVERY_TYPE_CAB;
#else
	pstContents->eDeliType	= eDxDELIVERY_TYPE_SAT;
#endif
	pstContents->ulActionId	= SVC_PIPE_GetActionId (hAction);
	pstContents->ulTunerId	= (HUINT32)nTunerId;

	pstContents->bForcedStop = FALSE;

	HxSTD_memcpy(&pstContents->stOtaCheckSpec, xmgr_swup_GetOtaSpec_Dvbssu(), sizeof(SvcSi_SwupCheckSpec_t));

	// 3. tuning information :
	// TP List :
	hErr = xmgr_swup_MakeHomeTpList_Dvbssu (pstContents);
	if (hErr != ERR_OK)					{ return MESSAGE_BREAK; }

	pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Dvbssu (pstContents);
	if (pstTsInfo != NULL)
	{
		pstContents->eDeliType = pstTsInfo->eDeliType;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_MakeHomeTpList_Dvbssu (xmgrSwup_DvbssuActionContents_t* pstContents)
{
	HUINT32					 ulTpCnt = 0 , ulTpNum = 0 ;
	DbSvc_TsInfo_t			*pstTsInfo = NULL;
	HERROR					 hErr;

	if (pstContents == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	// Get the default TPs and the numbers.
	hErr = xmgr_swup_CountOtaTsInfo_Dvbssu (pstContents, &ulTpNum);
	if (hErr != ERR_OK || ulTpNum == 0)
	{
		HxLOG_Critical("xmgr_swup_CountOtaTsInfo_Dvbssu Error (hErr : %d , ulTpNum : %d)\n\n", hErr, ulTpNum);
		return ERR_FAIL;
	}

	for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
	{
		pstTsInfo = xmgr_swup_AllocTsInfo_Dvbssu();
		if (pstTsInfo != NULL)
		{
			hErr = xmgr_swup_GetOtaTsInfo_Dvbssu (pstContents, ulTpCnt, pstTsInfo);
			if (hErr == ERR_OK)
			{
				xmgr_swup_AddHomeTsInfo_Dvbssu (pstContents, pstTsInfo);
			}
			else
			{
				xmgr_swup_FreeTsInfo_Dvbssu (pstTsInfo);
			}
		}
	}

	if (xmgr_swup_GetCurrentHomeTsInfo_Dvbssu (pstContents) == NULL)
	{
		HxLOG_Critical("xmgr_swup_GetCurrentHomeTsInfo_Dvbssu Error \n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define	_____CONTENTS_DESTRUCTION_____

STATIC HERROR xmgr_swup_DestroyContents_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	xmgr_swup_ClearAllHomeTsInfo_Dvbssu (pstContents);
	xmgr_swup_ClearAllRefNitLink_Dvbssu (pstContents);
	xmgr_swup_ClearAllSsuScanLink_Dvbssu (pstContents);
	xmgr_swup_ClearAllSsuLink_Dvbssu (pstContents);
	xmgr_swup_ClearAllTsLink_Dvbssu(pstContents);
	xmgr_swup_ClearSsuDescriptorLink_Dvbssu(pstContents);

	return ERR_BUS_NO_ERROR;
}

#define	_____DATA_CONVERSION_____

STATIC HERROR xmgr_swup_FindTsInfoByTripleId_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, DbSvc_TripleId_t *pstTripleId, DbSvc_TsInfo_t *pstSvcTsInfo)
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

STATIC HERROR xmgr_swup_TuneSearchTp_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	HBOOL				 bRetry;
	SvcRsc_Info_t			 stRscInfo;
	HERROR		 		 hErr;

	bRetry = TRUE;
	MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);

	while (bRetry == TRUE)
	{
		hErr = xmgr_swup_SetTuningParam_Dvbssu (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("No Tp left to tune\n");

			hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
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
			HxLOG_Error("SVC_CH_StopTune Failed\n");
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_BUS_NO_OBJECT;
		}


		xmgr_swup_IncreaseSessionVersion_Dvbssu (pstContents);			// Tuning 전에 반드시 Action Version을 업데이트해야 한다.

		HxLOG_Print("Tune TP  : ");
		xmgr_swup_PrintCurrentTuningInfo_Dvbssu (pstContents);
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
		hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
			bRetry = FALSE;
		}
	}

	return ERR_BUS_NO_OBJECT;
}

STATIC HERROR xmgr_swup_SetTuningParam_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	DbSvc_TsInfo_t		 stSvcTsInfo;
	DbSvc_TsInfo_t		*pstTsInfo;
	DbSvc_TripleId_t		*pstTripleId;
	HERROR				 hErr;

	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_DVBSSU_HOME_TP:
		pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Dvbssu (pstContents);
		if (pstTsInfo == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		stSvcTsInfo = *pstTsInfo;

		break;

	case eSI_SWUP_MODE_DVBSSU_SSUSCAN_TP:
		// SSU SCAN List
		pstTripleId = xmgr_swup_GetCurrentSsuScanLink_Dvbssu (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		hErr = xmgr_swup_FindTsInfoByTripleId_Dvbssu (pstContents, pstTripleId, &stSvcTsInfo);
		if (hErr != ERR_BUS_NO_ERROR)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:
		// SSU LINK List
		pstTripleId = xmgr_swup_GetCurrentSsuLink_Dvbssu (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		hErr = xmgr_swup_FindTsInfoByTripleId_Dvbssu (pstContents, pstTripleId, &stSvcTsInfo);
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

STATIC HERROR xmgr_swup_SetNextTpForOta_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_DVBSSU_HOME_TP:
		xmgr_swup_SetNextHomeTsInfo_Dvbssu (pstContents);
		if (xmgr_swup_GetCurrentHomeTsInfo_Dvbssu (pstContents) == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_DVBSSU_SSUSCAN_TP:
		xmgr_swup_SetNextSsuScanLink_Dvbssu (pstContents);
		if (xmgr_swup_GetCurrentSsuScanLink_Dvbssu (pstContents) == NULL)
		{
			// SSU-BAT TP 가 더 이상 남아 있지 않다 : 다음 Home TP로 가서 작업을 한다.
			xmgr_swup_SetSiOtaMode_Dvbssu (pstContents, eSI_SWUP_MODE_DVBSSU_HOME_TP);
			return xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
		}

		break;

	case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:
		xmgr_swup_SetNextSsuLink_Dvbssu (pstContents);
		if (xmgr_swup_GetCurrentSsuLink_Dvbssu (pstContents) == NULL)
		{
			// SSU-SVC TP 가 더 이상 남아 있지 않다 : 다음 SSU-BAT TP로 가서 작업을 한다.
			xmgr_swup_SetSiOtaMode_Dvbssu (pstContents, eSI_SWUP_MODE_DVBSSU_SSUSCAN_TP);
			return xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
		}

		break;

	default:
		break;
	}

	return ERR_BUS_NO_ERROR;
}


#define	_____MESSAGE_FUNCTIONS_____


#define	_____CONTENTS_DATA_FUNCTIONS_____

STATIC HERROR xmgr_swup_CountOtaTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, HUINT32 *pulTpNum)
{
	(void)pstContents;

	if (pulTpNum == NULL)					{ return ERR_BUS_TARGET_NULL; }

	*pulTpNum = MGR_SWUPINFO_CountOtaHomeTp();
	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_GetOtaTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR		 hErr;

	(void)pstContents;

	if (pstTsInfo == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = MGR_SWUPINFO_GetOtaHomeTp (ulTpNum, pstTsInfo);
	if (hErr != ERR_OK)						{ return ERR_BUS_OUT_OF_RANGE; }

	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
	return ERR_BUS_NO_ERROR;
}

STATIC void xmgr_swup_SetSiOtaMode_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, XsvcSi_SwupMode_e eOtaMode)
{
	HxLOG_Print("SI Mode %d -> %d\n", pstContents->eSiSwupMode, eOtaMode);
	pstContents->eSiSwupMode = eOtaMode;
}

STATIC DbSvc_TsInfo_t *xmgr_swup_GetCurrentHomeTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
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

STATIC void xmgr_swup_AddHomeTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo)
{
	if (pstContents != NULL && pstHomeTsInfo != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstHomeTpList, (void *)pstHomeTsInfo);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstHomeTpList);
	}
}

STATIC void xmgr_swup_SetNextHomeTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstHomeTpList != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstHomeTpList, pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dvbssu);
	}
}

STATIC void xmgr_swup_ClearAllHomeTsInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dvbssu);
	}
}

STATIC void xmgr_swup_ClearAllRefNitLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstRefNitList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstRefNitList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dvbssu);
	}
}

STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentSsuScanLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
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

STATIC void xmgr_swup_AddSsuScanLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuScan)
{
	if (pstContents != NULL && pstSsuScan != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSsuScanList, (void *)pstSsuScan);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstSsuScanList);
	}
}

STATIC void xmgr_swup_SetNextSsuScanLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstSsuScanList != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuScanList, pstContents->pstSsuScanList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dvbssu);
	}
}

STATIC void xmgr_swup_ClearAllSsuScanLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuScanList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dvbssu);
	}
}

STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentSsuLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
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

STATIC void xmgr_swup_AddSsuLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuLink)
{
	if (pstContents != NULL && pstSsuLink != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSsuLinkList, (void *)pstSsuLink);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstSsuLinkList);
	}
}

STATIC void xmgr_swup_SetNextSsuLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstSsuLinkList != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuLinkList, pstContents->pstSsuLinkList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dvbssu);
	}
}

STATIC void xmgr_swup_ClearAllSsuLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuLinkList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dvbssu);
	}
}

STATIC void xmgr_swup_ClearAllTsLink_Dvbssu (xmgrSwup_DvbssuActionContents_t * pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTsInfoListFromSi, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dvbssu);
	}
}

STATIC void xmgr_swup_ClearSsuDescriptorLink_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		if(pstContents->pstMessageInfo)
		{
			pstContents->pstMessageInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstMessageInfo, (FREE_FUNC)xmgr_swup_FreeMessageInfoLinkItem_Dvbssu);
		}

		if(pstContents->pstUpdateInfo)
		{
			pstContents->pstUpdateInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstUpdateInfo, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dvbssu);
		}

		if(pstContents->pstScheduleInfo)
		{
			pstContents->pstScheduleInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstScheduleInfo, (FREE_FUNC)xmgr_swup_FreeLinkItem_Dvbssu);
		}
	}
}

#define	_____PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_swup_StartAction_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.
	HERROR			 hErr;

	xmgr_swup_SetSiOtaMode_Dvbssu (pstContents, eSI_SWUP_MODE_DVBSSU_HOME_TP);

	// 1. Set the TP to tuning :
	hErr = xmgr_swup_SetTuningParam_Dvbssu (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("xmgr_swup_SetTuningParam_Dvbssu Error \n\n");
		return ERR_FAIL;
	}

	// 2. Get the Action : DVB-SSU OTA는 Search와 다를 바가 없으므로 Action은 Search 로써 잡아도 된다.
	hErr = MGR_ACTION_SetupSearch (pstContents->ulActionId, &pstContents->stTuningInfo, eAmPriority_SEARCH);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("MGR_ACTION_SetupSearch Error \n\n");
		return ERR_FAIL;
	}

	hErr = MGR_ACTION_Take (pstContents->ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("MGR_ACTION_Take Error\n\n");
		return ERR_FAIL;
	}

	pstContents->eAppState = eOtaApp_state_idle;

	// Tune the TP
	hErr = xmgr_swup_TuneSearchTp_Dvbssu (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("xmgr_swup_TuneSearchTp_Dvbssu Error\n\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_StopAction_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	if (pstContents->eAppState == eOtaApp_state_waitingSi)
	{
		// stop sw update detect
		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
	}

	if (pstContents->eAppState != eOtaApp_state_init)
	{
HxLOG_Print("Keep action until Connecting module is ready!!!\n");
HxLOG_Print("Keep action until Connecting module is ready!!!\n");
HxLOG_Print("Keep action until Connecting module is ready!!!\n");
		xmgr_swup_ReleaseAction_Dvbssu (pstContents);
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_StartSi_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	HUINT16			 			usExtId1, usExtId2;
	DbSvc_TripleId_t			*pstTripleId;
	SvcSi_SwupCheckOption_t		stOtaCheckOption;
	DbSvc_TsInfo_t				*pstTsInfo;

	usExtId1 = 0;
	usExtId2 = 0;

	// SSU Link에 있는 SVCID가 필요하다.
	if (pstContents->eSiSwupMode == eSI_SWUP_MODE_DVBSSU_SSUSVC_TP)
	{
		pstTripleId = xmgr_swup_GetCurrentSsuLink_Dvbssu (pstContents);
		if (pstTripleId != NULL)
		{
			usExtId1 = pstTripleId->usSvcId;
		}
	}
	else if (pstContents->eSiSwupMode == eSI_SWUP_MODE_DVBSSU_HOME_TP)
	{
		pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Dvbssu(pstContents);
		if(NULL != pstTsInfo)
		{
			if(eDxDELIVERY_TYPE_CAB == pstTsInfo->eDeliType)
			{
				usExtId1 = pstTsInfo->stTuningParam.cab.usNetworkId;
			}
		}
	}

	stOtaCheckOption.ulSwupMode = eSI_SWUP_MODE_DVBSSU_HOME_TP + abs(pstContents->eSiSwupMode - eSI_SWUP_MODE_DVBSSU_HOME_TP);
	stOtaCheckOption.usExtId1 = usExtId1;
	stOtaCheckOption.usExtId2 = usExtId2;

	HxLOG_Print("eOtaMode(0x%x) ExtId1(0x%x) ExtId2(0x%x)\n,",
		stOtaCheckOption.ulSwupMode, stOtaCheckOption.usExtId1, stOtaCheckOption.usExtId2 );


	return SVC_SI_StartSwupCheck(pstContents->hAction, &pstContents->stOtaCheckSpec, &stOtaCheckOption, FALSE);
}

STATIC DbSvc_TsInfo_t *xmgr_swup_AllocTsInfo_Dvbssu (void)
{
	return (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
}

STATIC void xmgr_swup_FreeTsInfo_Dvbssu (void *pvTsInfo)
{
	if (pvTsInfo != NULL)
	{
		OxAP_Free (pvTsInfo);
	}
}

STATIC void xmgr_swup_ReleaseAction_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	HERROR			 hErr;

	hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("SVC_CH_StopTune Error \n\n\n");
	}

	// Release the action :
	MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
}


#define _____DEBUG_FUNCTIONS_____

STATIC void	xmgr_swup_PrintTripleId_Dvbssu (DbSvc_TripleId_t *pstTripleId)
{
#if defined(CONFIG_DEBUG)
	HLIB_CMD_Printf("(%04x:%04x:%04x)\n", pstTripleId->usOnId, pstTripleId->usTsId, pstTripleId->usSvcId);
#else
	(void)pstTripleId;
#endif
}

STATIC void xmgr_swup_PrintCurrentTuningInfo_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
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
STATIC HERROR xmgr_swup_MsgGwmCreate_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate)
{
	HERROR		 hErr;

	// Create시 해 주어야 하는 Initialize 등이 실패할 경우에는 이 Action 자체가 살아 있어서는 안된다.
	// 곧바로 Destroy 해 주도록 한다.

	// 1. Initialize the contents
	hErr = xmgr_swup_InitContents_Dvbssu (pstContents, hAction, nTunerId, hSWUpdate);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		xmgr_swup_SwDetectFinish_Dvbssu(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
		return hErr;
	}

	// 2. Start the OTA Action :
	hErr = xmgr_swup_StartAction_Dvbssu (pstContents);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		xmgr_swup_SwDetectFinish_Dvbssu(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Action);
		return hErr;
	}

	MGR_SWUPINFO_ResetSearchedInfo();

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY:
STATIC HERROR xmgr_swup_MsgGwmDestroyed_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	// Stop all the Action
	xmgr_swup_StopAction_Dvbssu (pstContents);

	// Destroy the contents
	xmgr_swup_DestroyContents_Dvbssu (pstContents);

	// Inform that the OTA action is destroyed
	if (pstContents->bForcedStop != TRUE)
	{
//		BUS_PostMessage (NULL, MSG_APP_OTA_ACTION_DESTROYED, (Handle_t)NULL, (HINT32)BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()), 0, 0);
	}

	return MESSAGE_BREAK;
}

// eMEVT_BUS_TIMER:
STATIC HERROR xmgr_swup_MsgGwmTimer_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, HINT32 nTimerId)
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
			hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Dvbssu (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("OTA_TIMER_LOCKING_ID :: No Next Tp left to tune\n");
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
				hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_swup_TuneSearchTp_Dvbssu (pstContents);
				}
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("OTA_TIMER_SI_ID :: No Next Tp left to tune\n");
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
		xmgr_swup_SwDetectFinish_Dvbssu(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}

	return hRetValue;
}


// EVT_SI_OTA_DESCRIPTOR_SUCCESS:
STATIC HERROR xmgr_swup_EvtSiDescriptorSuccess_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	HERROR			hErr;
	HUINT32 			ulDataNum, ulIndex;
	SvcSi_SsuMessageInfo_t	*pstMessageInfo, *pvMessageInfoArray;
	SvcSi_SsuUpdateInfo_t	*pstUpdateInfo, *pvUpdateInfoArray;
	SvcSi_SsuScheduleInfo_t	*pstScheduleInfo, *pvScheduleInfoArray;

	// message descriptor
	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_MESSAGE_INFO, &ulDataNum, (void **)&pvMessageInfoArray);
	if (hErr == ERR_OK && pvMessageInfoArray != NULL)
	{
		HxLOG_Print("eSI_SWUP_DATA_SSU_MESSAGE_INFO: %d\n", ulDataNum);

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

	// update descriptor
	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_UPDATE_INFO, &ulDataNum, (void **)&pvUpdateInfoArray);
	if (hErr == ERR_OK && pvUpdateInfoArray != NULL)
	{
		HxLOG_Print("eSI_SWUP_DATA_SSU_UPDATE_INFO: %d\n", ulDataNum);

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

	// schedule descriptor
	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCHEDULE_INFO, &ulDataNum, (void **)&pvScheduleInfoArray);
	if (hErr == ERR_OK && pvScheduleInfoArray != NULL)
	{
		HxLOG_Print("eSI_SWUP_DATA_SSU_SCHEDULE_INFO: %d\n", ulDataNum);

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

// EVT_SI_OTA_SUCCESS:
STATIC HERROR xmgr_swup_EvtSiOtaSuccess_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	HERROR				 hErr = ERR_FAIL;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_waitingSi:
		// SI succeeded : Kill the timer

		BUS_KillTimer (OTA_TIMER_SI_ID);

		pstContents->eAppState = eOtaApp_state_siReceived;

		switch (pstContents->eSiSwupMode)
		{
		case eSI_SWUP_MODE_DVBSSU_HOME_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInAnchorTp_Dvbssu (pstContents);
			break;

		case eSI_SWUP_MODE_DVBSSU_SSUSCAN_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInSsuScanTp_Dvbssu (pstContents);
			break;

		case eSI_SWUP_MODE_DVBSSU_SSUSVC_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Dvbssu (pstContents);
			break;

		default:
			break;
		}

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		if (pstContents->eAppState != eOtaApp_state_checkend)
		{
			hErr = xmgr_swup_TuneSearchTp_Dvbssu (pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("No Next Tp left to tune\n");
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
		if((pstContents->eSiSwupMode != eSI_SWUP_MODE_DVBSSU_SSUSVC_TP)
			|| ((pstContents->eSiSwupMode == eSI_SWUP_MODE_DVBSSU_SSUSVC_TP) && (hErr != ERR_OK)))
		{
			// TODO: 확인 필요..
			//xmgr_swup_SwDetectFinish_Dvbssu(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
			//MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
			//BUS_PostMessageToOM(NULL, eMEVT_DN_SWUP_DETECT_FAIL, HANDLE_NULL, pstContents->hSWUpdate, 0, 0);
		}
	}

	return MESSAGE_BREAK;
}



STATIC HERROR xmgr_swup_EvtSiOtaSuccessInAnchorTp_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	HBOOL					 bSiDataFound;
	HUINT32					 ulIndex, ulDataNum;
	XsvcSi_SwupMode_e	 eNextOtaMode;
	DbSvc_TsInfo_t			*pstHomeTsInfo;
	DbSvc_TsInfo_t			*pstTsInfo, *pstTsInfoArray;
	DbSvc_TripleId_t		*pstTripleId, *pstTripleIdArray;
	HERROR					 hErr;

	bSiDataFound = FALSE;
	HxSTD_MemSet(&eNextOtaMode, 0x00, sizeof(eNextOtaMode));

	pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Dvbssu (pstContents);

	if (pstHomeTsInfo != NULL)
	{
		pstContents->ulAntennaIndex = (HUINT32)pstHomeTsInfo->stTuningParam.sat.ucAntId;

		hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, &ulDataNum, (void **)&pstTsInfoArray);
		if (hErr == ERR_OK && pstTsInfoArray != NULL)
		{
			HxLOG_Print("NIT TS Info : %d\n", ulDataNum);

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

#if defined(CONFIG_DEBUG)
					HxLOG_Print("TP (%d) ", ulIndex);
					XMGR_SWUP_PrintTuningInfo(pstTsInfo->eDeliType, &pstTsInfo->stTuningParam);
					HxLOG_Print("\n");
#endif
					pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTsInfoListFromSi, (void *)pstTsInfo);
				}
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, (void *)pstTsInfoArray);
		}

		// SSU Scan Triple ID
		hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCAN_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
		if (hErr == ERR_OK && pstTripleIdArray != NULL)
		{
			HxLOG_Print("SSU SCAN TRIPLE : %d\n", ulDataNum);

			bSiDataFound = TRUE;
			eNextOtaMode = eSI_SWUP_MODE_DVBSSU_SSUSCAN_TP;

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
				if (pstTripleId != NULL)
				{
					*pstTripleId = pstTripleIdArray[ulIndex];
					xmgr_swup_AddSsuScanLink_Dvbssu (pstContents, pstTripleId);
#if defined(CONFIG_DEBUG)
					HxLOG_Print("SSU SCAN TRIPLE (%d) ", ulIndex);
					xmgr_swup_PrintTripleId_Dvbssu (pstTripleId);
#endif
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
				HxLOG_Print("SSU LINK TRIPLE : %d \n", ulDataNum );

				bSiDataFound = TRUE;
				eNextOtaMode = eSI_SWUP_MODE_DVBSSU_SSUSVC_TP;

				for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
				{
					pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
					if (pstTripleId != NULL)
					{
						*pstTripleId = pstTripleIdArray[ulIndex];
						xmgr_swup_AddSsuLink_Dvbssu (pstContents, pstTripleId);

#if defined(CONFIG_DEBUG)
						HxLOG_Print("SSU LINK TRIPLE  (%d) ", ulIndex);
						xmgr_swup_PrintTripleId_Dvbssu (pstTripleId);
#endif
					}
				}
				SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, (void *)pstTripleIdArray);
			}
		}
	}

	if (bSiDataFound == TRUE)
	{
		xmgr_swup_SetSiOtaMode_Dvbssu (pstContents, eNextOtaMode);
	}
	else
	{
		hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
		if (hErr != ERR_OK)			// 모든 TP를 다 체크함
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSsuScanTp_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	HBOOL				 bSiDataFound;
	HUINT32				 ulIndex, ulDataNum;
	DbSvc_TripleId_t		*pstTripleId, *pstTripleIdArray;
	HERROR				 hErr;

	bSiDataFound = FALSE;

	// SSU-SVC
	hErr =SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
	if (hErr == ERR_OK && pstTripleIdArray != NULL)
	{
		HxLOG_Print("SSU LINK TRIPLE : %d\n", ulDataNum);

		bSiDataFound = TRUE;

		for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
		{
			pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
			if (pstTripleId != NULL)
			{
				*pstTripleId = pstTripleIdArray[ulIndex];
				xmgr_swup_AddSsuLink_Dvbssu (pstContents, pstTripleId);

				HxLOG_Print("[dvbssuota] SSU-SVC (%d) ", ulIndex);
				xmgr_swup_PrintTripleId_Dvbssu (pstTripleId);
			}
		}
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, (void *)pstTripleIdArray);
	}

	if (bSiDataFound == TRUE)
	{
		xmgr_swup_SetSiOtaMode_Dvbssu (pstContents, eSI_SWUP_MODE_DVBSSU_SSUSVC_TP);
	}
	else
	{
		hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
		if (hErr != ERR_OK)			// 모든 TP를 다 체크함
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	HUINT32				 ulDataNum;
	SvcSi_SwupSignal_t	 *pstOtaArray, *pstOtaSignalInfo=NULL,stOtaInfo;
	DxTuneParam_t		 stChTuneInfo;
	DbSvc_TsInfo_t		 *pstHomeTsInfo;
	DbSvc_TripleId_t	 *pstTripleId;
	DbSvc_TsInfo_t		 stOtaTsInfo;
	HERROR				 hErr, hResult = ERR_BUS_NO_ERROR;
	SvcSwUpdate_DetectResult_t		   *pstDetectResult = NULL;

	// SSU-SVC
	hErr =SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SIGNAL, &ulDataNum, (void **)&pstOtaArray);
	if (hErr == ERR_OK && pstOtaArray != NULL)
	{
		// Write OTA Signal Info
		pstOtaSignalInfo= (SvcSi_SwupSignal_t *)&(pstOtaArray[0]);

		// 찾은 OTA 정보를 기록한다.
		HxLOG_Error("[dvbssuota] Found OTA Info \n");
		HxLOG_Error("[dvbssuota] System ID : 0x%08x ~ 0x%08x \n", pstOtaSignalInfo->ulStartSystemId, pstOtaSignalInfo->ulEndSystemId);
		HxLOG_Error("[dvbssuota] System Ver : 0x%08x \n", pstOtaSignalInfo->ulDataVersion);
		HxLOG_Error("[dvbssuota] PID : %04x \n", pstOtaSignalInfo->usPid);

		HxSTD_MemSet(&stOtaTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

		hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
		if (hErr != ERR_OK)
		{
			stChTuneInfo = pstContents->stTuningInfo;
		}

		OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&stChTuneInfo, &(stOtaTsInfo.stTuningParam));

		pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Dvbssu (pstContents);
		if (pstHomeTsInfo != NULL)
		{
			stOtaTsInfo.eDeliType					= pstHomeTsInfo->eDeliType;
			stOtaTsInfo.ucTunerId					= pstHomeTsInfo->ucTunerId;
			stOtaTsInfo.stTuningParam.sat.ucAntId	= pstHomeTsInfo->stTuningParam.sat.ucAntId;
		}

		pstTripleId = xmgr_swup_GetCurrentSsuLink_Dvbssu (pstContents);
		if (pstTripleId != NULL)
		{
			stOtaTsInfo.usOnId = pstTripleId->usOnId;
			stOtaTsInfo.usTsId = pstTripleId->usTsId;
		}

		pstDetectResult = (SvcSwUpdate_DetectResult_t *)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
		if (pstDetectResult == NULL)
		{
			HxLOG_Error("Memory alloc fail!!\n");
			hResult = ERR_FAIL;
			goto END_FUNC;
		}

		HxSTD_MemSet (&stOtaInfo, 0, sizeof(SvcSi_SwupSignal_t));

		pstDetectResult->bAvailable = TRUE;
		pstDetectResult->bFound     = TRUE;

		// emergency set
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stChTuningInfo, &stChTuneInfo, sizeof(DxTuneParam_t));
		// 이것은 임시코드
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stSvcTsInfo, &stOtaTsInfo, sizeof(DbSvc_TsInfo_t));

#if defined(CONFIG_MW_CH_SATELLITE)
		pstDetectResult->data.stRfInfo.eDeliType	=	eDxDELIVERY_TYPE_SAT;
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
		pstDetectResult->data.stRfInfo.eDeliType	=	eDxDELIVERY_TYPE_TER;
#elif defined(CONFIG_MW_CH_CABLE)
		pstDetectResult->data.stRfInfo.eDeliType	=	eDxDELIVERY_TYPE_CAB;
#else
		pstDetectResult->data.stRfInfo.eDeliType	=	eDxDELIVERY_TYPE_SAT;
#endif
		pstDetectResult->data.stRfInfo.usPid		=	pstOtaSignalInfo->usPid;
		pstDetectResult->data.stRfInfo.eDownloadType = eSWUP_DOWNLOAD_DVBSSU;

		pstDetectResult->ulDataVersion = pstOtaSignalInfo->ulDataVersion;

		pstDetectResult->usOtaPid      = pstOtaSignalInfo->usPid;
		pstDetectResult->eSource	   = eDxSWUPDATE_SOURCE_Rf;
		MGR_SWUP_SetDetectInfo(pstContents->hSWUpdate, pstDetectResult);

		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32)pstDetectResult, sizeof(SvcSwUpdate_DetectResult_t));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x]\n", hErr);
			hResult = ERR_FAIL;
			goto END_FUNC;
		}

		stOtaInfo.ulStartSystemId = pstOtaSignalInfo->ulStartSystemId;
		stOtaInfo.ulEndSystemId = pstOtaSignalInfo->ulEndSystemId;
		stOtaInfo.eDataType = eVERSION_TYPE_APPL;
		stOtaInfo.ulDataVersion = pstOtaSignalInfo->ulDataVersion;
		stOtaInfo.usSvcId = pstOtaSignalInfo->usSvcId;
		stOtaInfo.usPid = pstOtaSignalInfo->usPid;
#if defined(CONFIG_OP_ERTELECOM)
		stOtaInfo.ucSwupType = pstOtaSignalInfo->ucSwupType;
#endif

		MGR_SWUPINFO_SetSearchedInfo (&stOtaTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);

		// detect 보내기..
		xmgr_swup_SwDetectFinish_Dvbssu(pstContents, DETECT_SUCCESS, (HINT32 *)&stOtaTsInfo,(HINT32 *)&stOtaInfo, eSWUP_FAILREASON_Unknown);

		HxLOG_Error("SEND event eMEVT_DN_SWUP_DETECT_SUCCESS\n");

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

	if(hResult == ERR_FAIL)
	{
		xmgr_swup_SwDetectFinish_Dvbssu(pstContents, DETECT_FAIL, 0, 0, eSWUP_FAILREASON_Unknown);

		hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
		if (hErr != ERR_OK) 		// 모든 TP를 다 체크함
		{
			HxLOG_Print("[%s] all tp checked & set eapp state to eOtaApp_state_checkend\n", __FUNCTION__);
			pstContents->eAppState = eOtaApp_state_checkend;
		}
	}

	return hErr;
}

// eSEVT_SI_SEARCH_FAIL:
STATIC HERROR xmgr_swup_EvtSiSearchFail_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, HINT32 nFailReason)
{
	HERROR				 hErr;
	MgrSwup_FailReason_e	eFailReason;

	HxLOG_Debug("nFailReason : %d, eAppState : %d\n", nFailReason, pstContents->eAppState);

#if defined(CONFIG_OP_TDC)
	if (nFailReason == eSvcSi_OTA_FAILREASON_Latest_Version)
	{
		// if eSvcSi_OTA_FAILREASON_Latest_Version, stop to detecting OTA.
		pstContents->eAppState = eOtaApp_state_checkend;
	}
#endif

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_waitingSi:
		// SI failed : Kill the timer

		BUS_KillTimer (OTA_TIMER_SI_ID);

		pstContents->eAppState = eOtaApp_state_siFailed;

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_swup_TuneSearchTp_Dvbssu (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
			break;
		}

		break;

	default:
		break;
	}

	if (pstContents->eAppState == eOtaApp_state_checkend)
	{
		eFailReason = MGR_SWUP_ConvertFailReasonSvctoMgr((SvcSi_OtaFailReason_e)nFailReason);

		// Tuning 단에서 실패 후 곧바로 State End :
		// 더 이상 진행할 수 없으니 Action App을 종료시킨다.
		xmgr_swup_SwDetectFinish_Dvbssu(pstContents, DETECT_FAIL, NULL, NULL, eFailReason);
	}

	return MESSAGE_BREAK;
}

// eSEVT_CH_LOCKED
STATIC HERROR xmgr_swup_EvtChLocked_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
		case eOtaApp_state_tryTuning :
			// Channel Locked : Kill the timer
			BUS_KillTimer (OTA_TIMER_LOCKING_ID);

			// Start the SI for the DVB-SSU OTA
			hErr = xmgr_swup_StartSi_Dvbssu (pstContents);
			if (hErr == ERR_OK)
			{
				pstContents->eAppState = eOtaApp_state_waitingSi;
				// MW SI를 무한으로 기다리는 것을 막기 위해 여기에서 Timer를 걸어야 한다.
				BUS_SetTimer (OTA_TIMER_SI_ID, OTA_TIMER_SI_TIME);
			}
			else
			{
				// 다음 TP 로 넘어가야 한다.
				hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_swup_TuneSearchTp_Dvbssu (pstContents);
				}

				if (hErr != ERR_OK)
				{
					HxLOG_Print("No Next Tp left to tune\n");
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
		xmgr_swup_SwDetectFinish_Dvbssu(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_EvtChNoSignal_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_tryTuning :
		// Channel Locked : Kill the timer

		BUS_KillTimer (OTA_TIMER_LOCKING_ID);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_swup_TuneSearchTp_Dvbssu (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("eOtaApp_state_tryTuning :: No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
		}

		break;

	case eOtaApp_state_waitingSi:
		// Stop SI :
		hErr = SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
		if (hErr == ERR_OK)
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_SetNextTpForOta_Dvbssu (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Dvbssu (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("eOtaApp_state_waitingSi :: No Next Tp left to tune\n");
				pstContents->eAppState = eOtaApp_state_checkend;
			}
		}
		else
		{
			// 더 이상 진행 불가능 : Search End로 간주한다.
			HxLOG_Print("SI Re-open failed.\n");
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
		xmgr_swup_SwDetectFinish_Dvbssu(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_MsgGwmQuitStandby_Dvbssu  (xmgrSwup_DvbssuActionContents_t *pstContents)
{
	pstContents->bForcedStop = TRUE;
	BUS_MGR_Destroy (NULL);

	return ERR_OK;
}


STATIC HERROR xmgr_swup_SwDetectFinish_Dvbssu (xmgrSwup_DvbssuActionContents_t *pstContents, HUINT8 ucDetectResult, HINT32 *pstOtaTsInfo, HINT32 *pstOtaInfo, MgrSwup_FailReason_e eFailReason)
{
	if(ucDetectResult == DETECT_SUCCESS)
	{
		HxLOG_Print("eMEVT_SWUP_NOTIFY_DETECT_SUCCESS (pstContents->hSWUpdate: 0x%X)\n", pstContents->hSWUpdate);
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_SUCCESS, eSWUP_STATE_Connect, (HINT32)pstOtaTsInfo, (HINT32)pstOtaInfo, 0);
	}
	else
	{
		HxLOG_Print("eMEVT_SWUP_NOTIFY_DETECT_FAIL! (eFailReason = %d)\n", eFailReason);
		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, (Handle_t)0, 0, 0, eFailReason);
	}

	return ERR_OK;
}




#define	_____EXTERNAL_PROCESS_FUNCTION_____

BUS_Result_t xproc_swup_Detect_Dvbssu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	STATIC HBOOL			 s_bFirstStart = TRUE;
	xmgrSwup_DvbssuActionContents_t		*pstContents = NULL;
	HERROR			 hErr;

	pstContents = xmgr_swup_GetContents_Dvbssu();
	if (s_bFirstStart == TRUE)
	{
		s_bFirstStart = FALSE;
		HxSTD_memset (pstContents, 0, sizeof(xmgrSwup_DvbssuActionContents_t));
	}

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Print("\teMEVT_BUS_CREATE \n\n");
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "XMGR_SWUP_DETECT_DVBSSU_Proc");
		hErr = xmgr_swup_MsgGwmCreate_Dvbssu (pstContents, hAction, p1, p2);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Print("\teMEVT_BUS_TIMER \n\n");
		hErr = xmgr_swup_MsgGwmTimer_Dvbssu (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Print("\teMEVT_BUS_DESTROY \n\n");
		hErr = xmgr_swup_MsgGwmDestroyed_Dvbssu (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_PRIVATE_DATA_FOUND:
		hErr = xmgr_swup_CheckActionId_Dvbssu (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\teSEVT_SWUP_DETECT_PRIVATE_DATA_FOUND \n\n");

		hErr = xmgr_swup_EvtSiDescriptorSuccess_Dvbssu (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_SUCCESS:
		hErr = xmgr_swup_CheckActionId_Dvbssu (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\teSEVT_SWUP_DETECT_SUCCESS \n\n");

		hErr = xmgr_swup_EvtSiOtaSuccess_Dvbssu (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_FAIL:
		hErr = xmgr_swup_CheckActionId_Dvbssu (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\teSEVT_SWUP_DETECT_FAIL \n\n");

		hErr = xmgr_swup_EvtSiSearchFail_Dvbssu (pstContents, p3);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_LOCKED:
		hErr = xmgr_swup_CheckActionId_Dvbssu (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\teSEVT_CH_LOCKED \n\n");

		hErr = xmgr_swup_EvtChLocked_Dvbssu (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_NO_SIGNAL:
		hErr = xmgr_swup_CheckActionId_Dvbssu (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\teSEVT_CH_NO_SIGNAL \n\n");

		hErr = xmgr_swup_EvtChNoSignal_Dvbssu (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_GO_OPERATION :
		HxLOG_Print("\teMEVT_BUS_GO_OPERATION \n\n");
		hErr = xmgr_swup_MsgGwmQuitStandby_Dvbssu (pstContents);
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
HERROR xmgr_swup_SetDefaultHomeTsInfo_Dvbssu (void)
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


