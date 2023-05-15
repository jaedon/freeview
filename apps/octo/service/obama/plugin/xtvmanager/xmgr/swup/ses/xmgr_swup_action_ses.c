/**
	@file     ap_sat_search_svc.c
		---> xmgr_sw_update_action_humax.c
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
#if defined(CONFIG_MW_CAS)
#include <svc_cas.h>
#endif
#include <db_common.h>
#include <db_svc.h>
#include <linkedlist.h>
#include <svc_si.h>
#include <svc_sys.h>
#include <svc_resources.h>

#include <isosvc.h>

#include <mgr_common.h>
#include <mgr_spec.h>
#include <mgr_swup.h>
#include <mgr_search.h>

#include <mgr_rscmap.h>
#include <xmgr_swup.h>
#include <xmgr_module_search.h>
#include <xsvc_si.h>

#include "_xmgr_swup_ses.h"

#include <otl.h>

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

typedef struct
{
	HBOOL			 	 bTuned;
	DbSvc_TripleId_t	 stTripleId;
} xmgrSwup_SesOtaTripleId_t;

enum
{
	DETECT_FAIL,
	DETECT_SUCCESS,
};

/** @brief search application에서 실행에 필요한 environment context  */
typedef struct tagSesOta_Contents_t
{
	Handle_t					 hSWUpdate;
	HUINT32						 ulActionId;				// Search Application Action ID
	Handle_t					 hAction;					// Search Action handle
	HUINT32						 ulTunerId;					// Tuner ID
	DxDeliveryType_e			 eDeliType;       			// Delivery Type
	MgrSwup_AppState_t			 eAppState;					// Current Search State :
	XsvcSi_SwupMode_e			 eSiSwupMode;				// Current SI Mode

	HBOOL						 bForcedStop;				// Is the action killed by the other procedures?
	SesHdFuncMode_t				 eFuncMode;					// Function Mode (Normal / Boot / Standby)

	SvcSi_SwupCheckSpec_t		 stOtaCheckSpec;			// OTA Check Spec

	// === Tuning Informations ===
	DbSvc_TripleId_t			 stCurrTripleId;
	DxTuneParam_t				 stTuningInfo;

	// === Home TP Lists ===
	POINTER_LIST_T				*pstHomeTpList;				// Home TsInfo : SVC_TsInfo_t

	// === Reference NIT TripleIds ===
	POINTER_LIST_T				*pstRefNitList;				// Reference NIT Links : ApsOtaTripleId_t
	POINTER_LIST_T				*pstCheckedRefNitList;		// Checked Reference NIT Links : ApsOtaTripleId_t

	// === SSU Scan Link TripleIds ===
	POINTER_LIST_T				*pstSsuScanList;			// SSU Scan Links : ApsOtaTripleId_t
	POINTER_LIST_T				*pstCheckedSsuScanList;		// Checked SSU Scan Links : ApsOtaTripleId_t

	// === SSU Link TripleIds ===
	POINTER_LIST_T				*pstSsuLinkList;			// SSU Links : ApsOtaTripleId_t
	POINTER_LIST_T				*pstCheckedSsuLinkList;		// Checked SSU Links : ApsOtaTripleId_t

	// === NIT TS Infos ===
	POINTER_LIST_T				*pstTsInfoListFromSi;		// TsInfo From SI : SVC_TsInfo_t

	// === Satellite Information
	HUINT32						 ulAntennaIndex;			// Antenna Index
} xmgrSwup_SesActionContents_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC xmgrSwup_SesActionContents_t	s_stSes_Contents;

/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */
#define _____Static_Function_ProtoType_____
STATIC xmgrSwup_SesActionContents_t *xmgr_swup_GetContents_Ses (void);
STATIC INLINE HERROR xmgr_swup_CheckActionId_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction);
STATIC INLINE void xmgr_swup_IncreaseSessionVersion_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_FreeLinkItem_Ses (void *pvItem);
STATIC HERROR xmgr_swup_InitContents_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction, Handle_t hSWUpdate);
STATIC HERROR xmgr_swup_MakeHomeTpList_Ses (xmgrSwup_SesActionContents_t* pstContents);
STATIC HERROR xmgr_swup_DestroyContents_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_GetAstraAntIdx_Ses (HUINT32 *pulAntIdx);
STATIC HERROR xmgr_swup_TuneSearchTp_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_SetTuningParam_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_SetNextTpForOta_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_CountOtaTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents, HUINT32 *pulTpNum);
STATIC HBOOL xmgr_swup_FilterSameOnTsId_Ses (void *pvItem, void *pvFilter);
STATIC HBOOL xmgr_swup_FilterSameTripleId_Ses (void *pvItem, void *pvFilter);
STATIC HERROR xmgr_swup_GetOtaTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo);
STATIC DbSvc_TsInfo_t *xmgr_swup_GetCurrentHomeTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_AddHomeTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo);
STATIC void xmgr_swup_SetNextHomeTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_ClearAllHomeTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentRefNitLink_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_AddRefNitLink_Ses (xmgrSwup_SesActionContents_t *pstContents, DbSvc_TripleId_t *pstRefNitLink);
STATIC void xmgr_swup_SetNextRefNitLink_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_SetCurrRefNitLinkTuned_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_ClearAllRefNitLink_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentSsuScanLink_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_AddSsuScanLink_Ses (xmgrSwup_SesActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuScan);
STATIC void xmgr_swup_SetNextSsuScanLink_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_SetCurrSsuScanLinkTuned_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_ClearAllSsuScanLink_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentSsuLink_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_AddSsuLink_Ses (xmgrSwup_SesActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuLink);
STATIC void xmgr_swup_SetNextSsuLink_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_SetCurrSsuLinkTuned_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_ClearAllSsuLink_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_ClearAllTsLink_Ses (xmgrSwup_SesActionContents_t * pstContents);
STATIC HERROR xmgr_swup_StartAction_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_StopAction_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void xmgr_swup_SetSiOtaMode_Ses (xmgrSwup_SesActionContents_t *pstContents, XsvcSi_SwupMode_e eOtaMode);
STATIC HERROR xmgr_swup_StartSi_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC DbSvc_TsInfo_t *xmgr_swup_AllocTsInfo_Ses (void);
STATIC void xmgr_swup_FreeTsInfo_Ses (void *pvTsInfo);
STATIC void xmgr_swup_ReleaseAction_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC void	xmgr_swup_PrintTripleId_Ses (DbSvc_TripleId_t *pstTripleId);
STATIC HERROR xmgr_swup_FindTsInfoByTripleId_Ses (xmgrSwup_SesActionContents_t *pstContents, DbSvc_TripleId_t *pstTripleId, DbSvc_TsInfo_t *pstSvcTsInfo);
STATIC HERROR xmgr_swup_MsgGwmCreate_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate);
STATIC HERROR xmgr_swup_MsgGwmDestroyed_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_MsgGwmTimer_Ses (xmgrSwup_SesActionContents_t *pstContents, HINT32 nTimerId);
STATIC HERROR xmgr_swup_EvtSiSearchFail_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction);
STATIC HERROR xmgr_swup_EvtSiOtaSuccess_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction);
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInAnchorTp_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInRefNitTp_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSsuBatTp_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInBurstUpdateTp_Ses (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_EvtChLocked_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction);
STATIC HERROR xmgr_swup_EvtChNoSignal_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction);
STATIC HERROR xmgr_swup_MsgGwmQuitStandby_Ses  (xmgrSwup_SesActionContents_t *pstContents);
STATIC HERROR xmgr_swup_SwDetectFinish_Ses (xmgrSwup_SesActionContents_t *pstContents, HUINT8 ucDetectResult, HINT32 *pstOtaTsInfo, HINT32 *pstOtaInfo, MgrSwup_FailReason_e eFailReason);

/*******************************************************************/
/********************      Functions         *************************/
/*******************************************************************/
#define _____Static_Function_____

/**
 * @description     get SES swup contents
 * @param     	void
 * @return     	SES swup content variable.
 */
STATIC xmgrSwup_SesActionContents_t *xmgr_swup_GetContents_Ses (void)
{
	return &s_stSes_Contents;
}

#define _________________________________________________________________________________

/**
 * @description     compare param's action id with stored action id
 * @param     	SES swup content variable.
 * @param     	action handle
 * @return     	if action handle is match, return ERR_BUS_NO_ERROR, else MESSAGE_PASS
 */
STATIC INLINE HERROR xmgr_swup_CheckActionId_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction)
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

/**
 * @description     increase session version
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC INLINE void xmgr_swup_IncreaseSessionVersion_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

/**
 * @description     Free item of linked list.
 * @param     	item in linked list
 * @return     	void
 */
STATIC void xmgr_swup_FreeLinkItem_Ses (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

STATIC SvcSi_SwupCheckSpec_t* xmgr_swup_GetOtaSpec_Ses(void)
{
	STATIC SvcSi_SwupCheckSpec_t 	stOtaSpec;

	HxSTD_MemSet(&stOtaSpec, 0x00, sizeof(SvcSi_SwupCheckSpec_t));
	stOtaSpec.eActionKind = eSVCSI_ACT_KIND_SwupCheck;
	stOtaSpec.ulActionSpec = eSVCSI_ACT_SPEC_Ses;

	return &stOtaSpec;
}

#define	_____CONTENTS_INITIATION_____
/**
 * @description     initialize SES swup content variable.
 * @param     	SES swup content variable.
 * @param     	own proc's handle
 * @param     	swup handle (in xmgr_swup_ses.c)
 * @return     	if success return ERR_BUS_NO_ERROR, else MESSAGE_BREAK
 */
STATIC HERROR xmgr_swup_InitContents_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction, Handle_t hSWUpdate)
{
	HERROR			 hErr;

	// 1. Clear all the contents.
	HxSTD_memset (pstContents, 0, sizeof(xmgrSwup_SesActionContents_t));

	// 2. Get the function mode
	hErr = XMGR_SEARCH_INFO_GetSesHdFuncMode (&(pstContents->eFuncMode));
	if (hErr != ERR_OK)
	{
		pstContents->eFuncMode = eSesHdFuncMode_Normal;
	}

	// 3. Action Handle and Action ID
	pstContents->hSWUpdate	= hSWUpdate;
	pstContents->hAction	= hAction;
	pstContents->eDeliType	= eDxDELIVERY_TYPE_SAT;
	pstContents->ulActionId	= SVC_PIPE_GetActionId (hAction);
	pstContents->ulTunerId	= (HUINT32)-1;

	pstContents->bForcedStop = TRUE;

	HxSTD_memcpy(&pstContents->stOtaCheckSpec, xmgr_swup_GetOtaSpec_Ses(), sizeof(SvcSi_SwupCheckSpec_t));

	// 3. tuning information :
	// TP List :
	hErr = xmgr_swup_MakeHomeTpList_Ses (pstContents);
	if (hErr != ERR_OK)
	{
		//Check Burst Update
		if(XMGR_SWUP_CountBurstUpdateTp() <= 0)		return MESSAGE_BREAK;
	}

	return ERR_BUS_NO_ERROR;
}

/**
 * @description     make Home TP list for OTA
 * @param     	SES swup content variable.
 * @return     	after making home tp, get current Hompe. if Success return ERR_OK, else return ERR_FAIL.
 */
STATIC HERROR xmgr_swup_MakeHomeTpList_Ses (xmgrSwup_SesActionContents_t* pstContents)
{
	HUINT32					 ulTpCnt, ulTpNum;
	DbSvc_TsInfo_t			*pstTsInfo;
	HERROR					 hErr;
	if (pstContents == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	// Get the default TPs and the numbers.
	hErr = xmgr_swup_CountOtaTsInfo_Ses (pstContents, &ulTpNum);
	HxLOG_Print("[%s] result of getting tp number (%s), tp count (%d)\n", __FUNCTION__, (hErr == ERR_OK)?"ERR_OK":"ERR_FAIL", ulTpNum);

	if (hErr != ERR_OK || ulTpNum == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
	{
		pstTsInfo = xmgr_swup_AllocTsInfo_Ses();
		if (pstTsInfo != NULL)
		{
			hErr = xmgr_swup_GetOtaTsInfo_Ses (pstContents, ulTpCnt, pstTsInfo);
			if (hErr == ERR_OK)
			{
				xmgr_swup_AddHomeTsInfo_Ses (pstContents, pstTsInfo);
			}
			else
			{
				xmgr_swup_FreeTsInfo_Ses (pstTsInfo);
			}
		}
	}

	if (xmgr_swup_GetCurrentHomeTsInfo_Ses (pstContents) == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define	_____CONTENTS_DESTRUCTION_____

/**
 * @description	destroy contents (home tp, Reference nit, ssu scan, ssu link list and pstTsInfoListFromSi)
 * @param     	SES swup content variable.
 * @return		ERR_BUS_NO_ERROR
 */
STATIC HERROR xmgr_swup_DestroyContents_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	xmgr_swup_ClearAllHomeTsInfo_Ses (pstContents);
	xmgr_swup_ClearAllRefNitLink_Ses (pstContents);
	xmgr_swup_ClearAllSsuScanLink_Ses (pstContents);
	xmgr_swup_ClearAllSsuLink_Ses (pstContents);
	xmgr_swup_ClearAllTsLink_Ses (pstContents);

	return ERR_BUS_NO_ERROR;
}


#define	_____STATE_PROCESSING_FUNCTIONS_____

/**
 * @description	get astra antenna index.
 * @param     	pointer variable for andex index
 * @return		if success return ERR_OK, else ERR_FAIL
 */
STATIC HERROR xmgr_swup_GetAstraAntIdx_Ses (HUINT32 *pulAntIdx)
{
	HBOOL			 bFound;
	HUINT32			 ulAntIdx;
	HINT32			 nAntCnt, nTotalAntNum;
	HINT32			*pnAntIdxArray;
	DbSvc_AntInfo_t		 stAntInfo;
	HERROR			 hErr;

	if (pulAntIdx == NULL)				{ return ERR_BUS_TARGET_NULL; }

	// ASTRA로 설정된 Antenna를 찾아야 한다.
	bFound = FALSE;
	pnAntIdxArray = NULL;
	ulAntIdx = 0;

	hErr = DB_SVC_FindAllAntIdxList (&nTotalAntNum, &pnAntIdxArray);
	if (hErr == ERR_OK)
	{
		for (nAntCnt = 0; nAntCnt < nTotalAntNum; nAntCnt++)
		{
			hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxArray[nAntCnt], &stAntInfo);
			if (hErr == ERR_OK)
			{
				if (stAntInfo.eSatType == eSatType_ASTRA_1)
				{
					bFound = TRUE;
					ulAntIdx = (HUINT32)pnAntIdxArray[nAntCnt];
					break;
				}
			}
		}

		DB_SVC_FreeAntList (pnAntIdxArray);
	}

	if (bFound == FALSE)
	{
		HxLOG_Critical("\n\n\n");
		return ERR_FAIL;
	}

	*pulAntIdx = ulAntIdx;
	return ERR_OK;
}


/**
 * @description	set tuning param and start tune.
 * @param     	SES swup content variable.
 * @return		if success return ERR_BUS_NO_ERROR, else ERR_BUS_NO_OBJECT
 */
STATIC HERROR xmgr_swup_TuneSearchTp_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	HBOOL			 bRetry;
	SvcRsc_Info_t	 stRscInfo;
	HERROR			 hErr;

	bRetry = TRUE;

	while (bRetry == TRUE)
	{
		hErr = xmgr_swup_SetTuningParam_Ses (pstContents);

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[local_astraota_TuneSearchTp] No Tp left to tune\n");

			hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("[local_astraota_TuneSearchTp] No Next Tp left to tune\n");
				pstContents->eAppState = eOtaApp_state_checkend;
				bRetry = FALSE;
			}

			continue;
		}

		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[local_astraota_TuneSearchTp] Tuner Stop Failed\n");
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_BUS_NO_OBJECT;
		}


		xmgr_swup_IncreaseSessionVersion_Ses (pstContents);			// Tuning 전에 반드시 Action Version을 업데이트해야 한다.

//		HxLOG_Print("[astraota] Tune TP  : ");
//		local_sesota_PrintCurrentTuningInfo (pstContents);
//		HxLOG_Print("\n");

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
		hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[local_astraota_TuneSearchTp] No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
			bRetry = FALSE;
		}
	}

	return ERR_BUS_NO_OBJECT;
}

/**
 * @description	set tuning param by si OTA mode
 * @param     	SES swup content variable.
 * @return		if success return ERR_BUS_NO_ERROR, else ERR_BUS_NO_OBJECT
 */
STATIC HERROR xmgr_swup_SetTuningParam_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	DbSvc_TsInfo_t		 stSvcTsInfo;
	DbSvc_TsInfo_t		*pstTsInfo;
	DbSvc_TripleId_t		*pstTripleId;
	HERROR				 hErr;
	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_SES_HOME_TP:
		pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Ses (pstContents);
		if (pstTsInfo == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		stSvcTsInfo = *pstTsInfo;

		break;

	case eSI_SWUP_MODE_SES_REFNIT_TP:
		// Reference NIT List
		pstTripleId = xmgr_swup_GetCurrentRefNitLink_Ses (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		xmgr_swup_SetCurrRefNitLinkTuned_Ses (pstContents);

		hErr = xmgr_swup_FindTsInfoByTripleId_Ses (pstContents, pstTripleId, &stSvcTsInfo);
		if (hErr != ERR_BUS_NO_ERROR)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_SES_SSUBAT_TP:
		// SSU SCAN List
		pstTripleId = xmgr_swup_GetCurrentSsuScanLink_Ses (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		xmgr_swup_SetCurrSsuScanLinkTuned_Ses (pstContents);

		hErr = xmgr_swup_FindTsInfoByTripleId_Ses (pstContents, pstTripleId, &stSvcTsInfo);
		if (hErr != ERR_BUS_NO_ERROR)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_SES_SSUSVC_TP:
		// SSU LINK List
		pstTripleId = xmgr_swup_GetCurrentSsuLink_Ses (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		xmgr_swup_SetCurrSsuLinkTuned_Ses (pstContents);

		hErr = xmgr_swup_FindTsInfoByTripleId_Ses (pstContents, pstTripleId, &stSvcTsInfo);
		if (hErr != ERR_BUS_NO_ERROR)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_SES_BURST_TP:
		hErr = XMGR_SWUP_GetBurstUpdateTp (0, &stSvcTsInfo);
		if (hErr != ERR_OK)
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

/**
 * @description	set TP for OTA by si OTA mode
 * @param     	SES swup content variable.
 * @return		if success return ERR_BUS_NO_ERROR, else ERR_BUS_NO_OBJECT
 */
STATIC HERROR xmgr_swup_SetNextTpForOta_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_SES_HOME_TP:
		xmgr_swup_SetNextHomeTsInfo_Ses (pstContents);
		if (xmgr_swup_GetCurrentHomeTsInfo_Ses (pstContents) == NULL)
		{
			HxLOG_Print("[%s] return ERR_BUS_NO_OBJECT\n", __FUNCTION__);
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_SES_REFNIT_TP:
		xmgr_swup_SetNextRefNitLink_Ses (pstContents);
		if (xmgr_swup_GetCurrentRefNitLink_Ses(pstContents) == NULL)
		{
			// REF-NIT TP 가 더 이상 남아 있지 않다 : 다음 HOME TP로 가서 작업을 한다.
			xmgr_swup_SetSiOtaMode_Ses (pstContents, eSI_SWUP_MODE_SES_HOME_TP);
			return xmgr_swup_SetNextTpForOta_Ses (pstContents);
		}

		break;

	case eSI_SWUP_MODE_SES_SSUBAT_TP:
		xmgr_swup_SetNextSsuScanLink_Ses (pstContents);
		if (xmgr_swup_GetCurrentSsuScanLink_Ses (pstContents) == NULL)
		{
			// ASTRA SSU-BAT TP 가 더 이상 남아 있지 않다 : 다음 REF-NIT TP로 가서 작업을 한다.
			xmgr_swup_SetSiOtaMode_Ses (pstContents, eSI_SWUP_MODE_SES_REFNIT_TP);
			return xmgr_swup_SetNextTpForOta_Ses (pstContents);
		}

		break;

	case eSI_SWUP_MODE_SES_SSUSVC_TP:
		xmgr_swup_SetNextSsuLink_Ses (pstContents);
		if (xmgr_swup_GetCurrentSsuLink_Ses (pstContents) == NULL)
		{
			// ASTRA SSU-SVC TP 가 더 이상 남아 있지 않다 : 다음 SSU-BAT TP로 가서 작업을 한다.
			xmgr_swup_SetSiOtaMode_Ses (pstContents, eSI_SWUP_MODE_SES_SSUBAT_TP);
			return xmgr_swup_SetNextTpForOta_Ses (pstContents);
		}

		break;

	case eSI_SWUP_MODE_SES_BURST_TP:
		XMGR_SWUP_ResetBurstUpdateTp();
		return ERR_BUS_NO_OBJECT;

	default:
		break;
	}

	return ERR_BUS_NO_ERROR;
}

#define	_____CONTENTS_DATA_FUNCTIONS_____

/**
 * @description     compare pvItem's TsID, OnID with pvFilter's
 * @param     	pvItem
 * @param     	pvFilter
 * @return     	if it is equal, return TRUE, else return FALSE
 */
STATIC HBOOL xmgr_swup_FilterSameOnTsId_Ses (void *pvItem, void *pvFilter)
{
	xmgrSwup_SesOtaTripleId_t	*pstItem;
	DbSvc_TripleId_t	*pstFilter;

	pstItem = (xmgrSwup_SesOtaTripleId_t *)pvItem;
	pstFilter = (DbSvc_TripleId_t *)pvFilter;

	if (pstItem != NULL && pstFilter != NULL)
	{
		if (pstItem->stTripleId.usOnId == pstFilter->usOnId &&
			pstItem->stTripleId.usTsId == pstFilter->usTsId)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/**
 * @description     compare pvItem's TsID, OnID and serviceID with pvFilter's
 * @param     	pvItem
 * @param     	pvFilter
 * @return     	if it is equal, return TRUE, else return FALSE
 */
STATIC HBOOL xmgr_swup_FilterSameTripleId_Ses (void *pvItem, void *pvFilter)
{
	xmgrSwup_SesOtaTripleId_t	*pstItem;
	DbSvc_TripleId_t		*pstFilter;

	pstItem = (xmgrSwup_SesOtaTripleId_t *)pvItem;
	pstFilter = (DbSvc_TripleId_t *)pvFilter;

	if (pstItem != NULL && pstFilter != NULL)
	{
		if (pstItem->stTripleId.usOnId  == pstFilter->usOnId &&
			pstItem->stTripleId.usTsId  == pstFilter->usTsId &&
			pstItem->stTripleId.usSvcId == pstFilter->usSvcId)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/**
 * @description     count Home tp number
 * @param     	SES swup content variable.
 * @param     	pointer variable for tp count
 * @return     	ERR_BUS_NO_ERROR
 */
STATIC HERROR xmgr_swup_CountOtaTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents, HUINT32 *pulTpNum)
{
	(void)pstContents;

	if (pulTpNum == NULL)					{ return ERR_BUS_TARGET_NULL; }

	*pulTpNum = MGR_SWUPINFO_CountOtaHomeTp();
	return ERR_BUS_NO_ERROR;
}

/**
 * @description     get ota home ts info.
 * @param     	SES swup content variable.
 * @param     	index of tp info in home tp list
 * @param     	pointer variable for tp info
 * @return     	if pointer is null, return ERR_BUS_TARGET_NULL. if success, return ERR_BUS_NO_ERROR else ERR_BUS_OUT_OF_RANGE
 */
STATIC HERROR xmgr_swup_GetOtaTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR		 hErr;

	(void)pstContents;

	if (pstTsInfo == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = MGR_SWUPINFO_GetOtaHomeTp (ulTpNum, pstTsInfo);
	if (hErr != ERR_OK)						{ return ERR_BUS_OUT_OF_RANGE; }

	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
	return ERR_BUS_NO_ERROR;
}

// Home tp
/**
 * @description     get current (first) ota home ts info in Home tp list.
 * @param     	SES swup content variable.
 * @return     	if pstHomeTpList is not null, return pvContents of pstHomeTpList's head. else return NULL.
 */
STATIC DbSvc_TsInfo_t *xmgr_swup_GetCurrentHomeTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents)
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

/**
 * @description     append ts info in Home tp list.
 * @param     	SES swup content variable.
 * @param     	ts info.
 * @return     	void
 */
STATIC void xmgr_swup_AddHomeTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo)
{
	if (pstContents != NULL && pstHomeTsInfo != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstHomeTpList, (void *)pstHomeTsInfo);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstHomeTpList);
	}
}

/**
 * @description	Delete head in home tp list,  secont tp will be head.
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_SetNextHomeTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstHomeTpList != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstHomeTpList, pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Ses);
	}
}


/**
 * @description	clear Home TP list
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_ClearAllHomeTsInfo_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Ses);
	}
}

// Reference NIT
/**
 * @description     get triple id of current (first) tp in Reference NIT tp list.
 * @param     	SES swup content variable.
 * @return     	if pstRefNitList is not null, return triple id of pstRefNitList's head. else return NULL.
 */
STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentRefNitLink_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	xmgrSwup_SesOtaTripleId_t	*pstItem;

	if (pstContents != NULL)
	{
		if (pstContents->pstRefNitList != NULL)
		{
			pstItem = (xmgrSwup_SesOtaTripleId_t *)UTIL_LINKEDLIST_GetContents(pstContents->pstRefNitList);
			if (pstItem != NULL)
			{
				return &(pstItem->stTripleId);
			}
		}
	}

	return NULL;
}

/**
 * @description     append ref nit triple id in reference nit list.
 * @param     	SES swup content variable.
 * @param     	triple id
 * @return     	void
 */
STATIC void xmgr_swup_AddRefNitLink_Ses (xmgrSwup_SesActionContents_t *pstContents, DbSvc_TripleId_t *pstRefNitLink)
{
	xmgrSwup_SesOtaTripleId_t	*pstAddItem;
	POINTER_LIST_T		*pstItem;

	if (pstContents != NULL && pstRefNitLink != NULL)
	{
		// 이미 리스트에 있거나 이전에 처리한 적이 있는 Link라면 skip 한다.
		pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstRefNitList, 0, xmgr_swup_FilterSameOnTsId_Ses, (void *)pstRefNitLink);
		if (pstItem != NULL)					{ return; }

		pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstCheckedRefNitList, 0, xmgr_swup_FilterSameOnTsId_Ses, (void *)pstRefNitLink);
		if (pstItem != NULL)					{ return; }

		pstAddItem = (xmgrSwup_SesOtaTripleId_t *)OxAP_Malloc (sizeof(xmgrSwup_SesOtaTripleId_t));
		if (pstAddItem != NULL)
		{
			pstAddItem->bTuned = FALSE;
			pstAddItem->stTripleId = *pstRefNitLink;

			pstContents->pstRefNitList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstRefNitList, (void *)pstAddItem);
			UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstRefNitList);
		}
	}
}

/**
 * @description	Delete head in reference NIT tp list,  secont tp will be head and append removed item into checked reference NIT list
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_SetNextRefNitLink_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	xmgrSwup_SesOtaTripleId_t *pstItem;

	if (pstContents != NULL && pstContents->pstRefNitList != NULL)
	{
		pstItem = UTIL_LINKEDLIST_GetContents (pstContents->pstRefNitList);
		if (pstItem != NULL)
		{
			if (pstItem->bTuned == TRUE)
			{
				pstContents->pstRefNitList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstRefNitList, pstContents->pstRefNitList, NULL);
				if (pstItem != NULL)
				{
					pstContents->pstCheckedRefNitList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstCheckedRefNitList, (void *)pstItem);
				}

			}
		}
		else
		{
			pstContents->pstRefNitList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstRefNitList, pstContents->pstRefNitList, NULL);
		}
	}
}

/**
 * @description	Set content's tune flag in pstRefNitList to TRUE.
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_SetCurrRefNitLinkTuned_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	xmgrSwup_SesOtaTripleId_t *pstItem;

	if (pstContents != NULL && pstContents->pstRefNitList != NULL)
	{
		pstItem = UTIL_LINKEDLIST_GetContents (pstContents->pstRefNitList);
		if (pstItem != NULL)
		{
			pstItem->bTuned = TRUE;
		}
	}
}

/**
 * @description	clear reference NIT list
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_ClearAllRefNitLink_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstRefNitList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstRefNitList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Ses);
		pstContents->pstCheckedRefNitList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstCheckedRefNitList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Ses);
	}
}

// SSU SCAN LINK : SSU-BAT
/**
 * @description     get triple id of current (first) tp in ssu scna list.
 * @param     	SES swup content variable.
 * @return     	if pstSsuScanList is not null, return triple id of pstSsuScanList's head. else return NULL.
 */
STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentSsuScanLink_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	xmgrSwup_SesOtaTripleId_t	*pstItem;

	if (pstContents != NULL)
	{
		if (pstContents->pstSsuScanList != NULL)
		{
			pstItem = (xmgrSwup_SesOtaTripleId_t *)UTIL_LINKEDLIST_GetContents(pstContents->pstSsuScanList);
			if (pstItem != NULL)
			{
				return &(pstItem->stTripleId);
			}
		}
	}

	return NULL;
}

/**
 * @description     append ssu scan triple id in ssu scan list.
 * @param     	SES swup content variable.
 * @param     	triple id
 * @return     	void
 */
STATIC void xmgr_swup_AddSsuScanLink_Ses (xmgrSwup_SesActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuScan)
{
	xmgrSwup_SesOtaTripleId_t	*pstAddItem;
	POINTER_LIST_T		*pstItem;

	if (pstContents != NULL && pstSsuScan != NULL)
	{
		// 이미 리스트에 있거나 이전에 처리한 적이 있는 Link라면 skip 한다.
		pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstSsuScanList, 0, xmgr_swup_FilterSameOnTsId_Ses, (void *)pstSsuScan);
		if (pstItem != NULL)					{ return; }

		pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstCheckedSsuScanList, 0, xmgr_swup_FilterSameOnTsId_Ses, (void *)pstSsuScan);
		if (pstItem != NULL)					{ return; }

		pstAddItem = (xmgrSwup_SesOtaTripleId_t *)OxAP_Malloc (sizeof(xmgrSwup_SesOtaTripleId_t));
		if (pstAddItem != NULL)
		{
			pstAddItem->bTuned = FALSE;
			pstAddItem->stTripleId = *pstSsuScan;

			pstContents->pstSsuScanList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSsuScanList, (void *)pstAddItem);
			UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstSsuScanList);
		}
	}
}

/**
 * @description	Delete head in ssu scan list,  secont tp will be head and append removed item into checked ssu scan list
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_SetNextSsuScanLink_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	xmgrSwup_SesOtaTripleId_t *pstItem;

	if (pstContents != NULL && pstContents->pstSsuScanList != NULL)
	{
		pstItem = UTIL_LINKEDLIST_GetContents (pstContents->pstSsuScanList);
		if (pstItem != NULL)
		{
			if (pstItem->bTuned == TRUE)
			{
				pstContents->pstSsuScanList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuScanList, pstContents->pstSsuScanList, NULL);
				if (pstItem != NULL)
				{
					pstContents->pstCheckedSsuScanList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstCheckedSsuScanList, (void *)pstItem);
				}
			}
		}
		else
		{
			pstContents->pstSsuScanList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuScanList, pstContents->pstSsuScanList, NULL);
		}
	}
}

/**
 * @description	Set item's tune flag in pstSsuScanList to TRUE.
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_SetCurrSsuScanLinkTuned_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	xmgrSwup_SesOtaTripleId_t *pstItem;

	if (pstContents != NULL && pstContents->pstSsuScanList != NULL)
	{
		pstItem = UTIL_LINKEDLIST_GetContents (pstContents->pstSsuScanList);
		if (pstItem != NULL)
		{
			pstItem->bTuned = TRUE;
		}
	}
}

/**
 * @description	clear ssu scan link list
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_ClearAllSsuScanLink_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuScanList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Ses);
		pstContents->pstCheckedSsuScanList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstCheckedSsuScanList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Ses);
	}
}

// SSU LINK : SSU-SVC
/**
 * @description     get triple id of current (first) tp in ssu link list.
 * @param     	SES swup content variable.
 * @return     	if pstSsuLinkList is not null, return triple id of pstSsuLinkList's head. else return NULL.
 */
STATIC DbSvc_TripleId_t *xmgr_swup_GetCurrentSsuLink_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	xmgrSwup_SesOtaTripleId_t	*pstItem;

	if (pstContents != NULL)
	{
		if (pstContents->pstSsuLinkList != NULL)
		{
			pstItem = (xmgrSwup_SesOtaTripleId_t *)UTIL_LINKEDLIST_GetContents(pstContents->pstSsuLinkList);
			if (pstItem != NULL)
			{
				return &(pstItem->stTripleId);
			}
		}
	}

	return NULL;
}

/**
 * @description     append ssu link triple id in ssu link list.
 * @param     	SES swup content variable.
 * @param     	triple id
 * @return     	void
 */
STATIC void xmgr_swup_AddSsuLink_Ses (xmgrSwup_SesActionContents_t *pstContents, DbSvc_TripleId_t *pstSsuLink)
{
	xmgrSwup_SesOtaTripleId_t	*pstAddItem;
	POINTER_LIST_T		*pstItem;

	if (pstContents != NULL && pstSsuLink != NULL)
	{
		// 이미 리스트에 있거나 이전에 처리한 적이 있는 Link라면 skip 한다.
		pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstSsuLinkList, 0, xmgr_swup_FilterSameTripleId_Ses, (void *)pstSsuLink);
		if (pstItem != NULL)					{ return; }

		pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstCheckedSsuLinkList, 0, xmgr_swup_FilterSameTripleId_Ses, (void *)pstSsuLink);
		if (pstItem != NULL)					{ return; }

		pstAddItem = (xmgrSwup_SesOtaTripleId_t *)OxAP_Malloc (sizeof(xmgrSwup_SesOtaTripleId_t));
		if (pstAddItem != NULL)
		{
			pstAddItem->bTuned = FALSE;
			pstAddItem->stTripleId = *pstSsuLink;

			pstContents->pstSsuLinkList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSsuLinkList, (void *)pstAddItem);
			UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstSsuLinkList);
		}
	}
}

/**
 * @description	Delete head in ssu link list,  secont tp will be head and append removed item into checked ssu link list
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_SetNextSsuLink_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	xmgrSwup_SesOtaTripleId_t	*pstItem;

	if (pstContents != NULL && pstContents->pstSsuLinkList != NULL)
	{
		pstItem = UTIL_LINKEDLIST_GetContents (pstContents->pstSsuLinkList);
		if (pstItem != NULL)
		{
			if (pstItem->bTuned == TRUE)
			{
				pstContents->pstSsuLinkList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuLinkList, pstContents->pstSsuLinkList, NULL);
				if (pstItem != NULL)
				{
					pstContents->pstCheckedSsuLinkList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstCheckedSsuLinkList, (void *)pstItem);
				}
			}
		}
		else
		{
			pstContents->pstSsuLinkList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuLinkList, pstContents->pstSsuLinkList, NULL);
		}
	}
}

/**
 * @description	Set item's tune flag in pstSsuLinkList to TRUE.
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_SetCurrSsuLinkTuned_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	xmgrSwup_SesOtaTripleId_t *pstItem;

	if (pstContents != NULL && pstContents->pstSsuLinkList != NULL)
	{
		pstItem = UTIL_LINKEDLIST_GetContents (pstContents->pstSsuLinkList);
		if (pstItem != NULL)
		{
			pstItem->bTuned = TRUE;
		}
	}
}

/**
 * @description	clear ssu link list
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_ClearAllSsuLink_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuLinkList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Ses);
		pstContents->pstCheckedSsuLinkList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstCheckedSsuLinkList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Ses);
	}
}

/**
 * @description	clear pstTsInfoListFromSi
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_ClearAllTsLink_Ses (xmgrSwup_SesActionContents_t * pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTsInfoListFromSi, (FREE_FUNC)xmgr_swup_FreeLinkItem_Ses);
	}
}

#define	_____PROCESSING_FUNCTIONS_____

/**
 * @description	start OTA
 * @param     	SES swup content variable.
 * @return     	if success return ERR_BUS_NO_ERROR, else ERR_FAIL
 */
STATIC HERROR xmgr_swup_StartAction_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.
	HERROR			 hErr;

	if(XMGR_SWUP_CountBurstUpdateTp() > 0)
		xmgr_swup_SetSiOtaMode_Ses (pstContents, eSI_SWUP_MODE_SES_BURST_TP);
	else
		xmgr_swup_SetSiOtaMode_Ses (pstContents, eSI_SWUP_MODE_SES_HOME_TP);

	// 1. Set the TP to tuning :
	hErr = xmgr_swup_SetTuningParam_Ses (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	// 2. Get the Action : ASTRA OTA는 Search와 다를 바가 없으므로 Action은 Search 로써 잡아도 된다.
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
	hErr = xmgr_swup_TuneSearchTp_Ses (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

/**
 * @description	stop OTA
 * @param     	SES swup content variable.
 * @return     	ERR_BUS_NO_ERROR
 */
STATIC HERROR xmgr_swup_StopAction_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	if (pstContents->eAppState == eOtaApp_state_waitingSi)
	{
		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
	}

	if (pstContents->eAppState != eOtaApp_state_init)
	{
		xmgr_swup_ReleaseAction_Ses (pstContents);
	}

	return ERR_BUS_NO_ERROR;
}

/**
 * @description     set si ota mode
 * @param     	SES swup content variable.
 * @param     	si ota mode
 * @return     	void
 */
STATIC void xmgr_swup_SetSiOtaMode_Ses (xmgrSwup_SesActionContents_t *pstContents, XsvcSi_SwupMode_e eOtaMode)
{
	HxLOG_Print("[ses_ota] SI Mode %d -> %d\n", pstContents->eSiSwupMode, eOtaMode);
	pstContents->eSiSwupMode = eOtaMode;
}

/**
 * @description     start si search for OTA
 * @param     	SES swup content variable.
 * @return     	if success return ERR_BUS_NO_ERROR, else ERR_FAIL
 */
STATIC HERROR xmgr_swup_StartSi_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	HUINT16			 		usExtId1, usExtId2;
	DbSvc_TripleId_t			*pstTripleId;
	SvcSi_SwupCheckOption_t		stOtaCheckOption;

	usExtId1 = 0;
	usExtId2 = 0;

	switch (pstContents->eSiSwupMode)
	{
		case eSI_SWUP_MODE_SES_REFNIT_TP:
			pstTripleId = xmgr_swup_GetCurrentRefNitLink_Ses (pstContents);
			if (pstTripleId != NULL)
			{
				usExtId1 = pstTripleId->usOnId;
				usExtId2 = pstTripleId->usTsId;
			}
			break;

		case eSI_SWUP_MODE_SES_SSUSVC_TP:
			pstTripleId = xmgr_swup_GetCurrentSsuLink_Ses (pstContents);
			if (pstTripleId != NULL)
			{
				usExtId1 = pstTripleId->usSvcId;
			}
			break;

		case eSI_SWUP_MODE_SES_BURST_TP:
			{
				HERROR					hErr;
				SvcSi_SwupSchedule_t	stSchedule;
				hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_SCHEDULEINFO, (HUINT32 *)&stSchedule, (HUINT32)sizeof(SvcSi_SwupSchedule_t));
				if(hErr == ERR_OK)
				{
					usExtId1 = stSchedule.stTripleId.usSvcId;
				}
			}
			break;

		default:
			break;
	}

	HxSTD_MemSet(&stOtaCheckOption,	0x00, sizeof(SvcSi_SwupCheckOption_t));

	stOtaCheckOption.ulSwupMode = eSI_SWUP_MODE_SES_HOME_TP + abs(pstContents->eSiSwupMode - eSI_SWUP_MODE_SES_HOME_TP);	// new si 적용이 완료되면 new type으로 모두 변경하자 !
	stOtaCheckOption.usExtId1 = usExtId1;
	stOtaCheckOption.usExtId2 = usExtId2;

	HxLOG_Print("[%s, %d] eOtaMode(0x%x) ExtId1(0x%x) ExtId2(0x%x)\n,",
		__FUNCTION__, __LINE__, stOtaCheckOption.ulSwupMode, stOtaCheckOption.usExtId1, stOtaCheckOption.usExtId2 );

	return SVC_SI_StartSwupCheck(pstContents->hAction, &pstContents->stOtaCheckSpec, &stOtaCheckOption, FALSE);

}

/**
 * @description     alloc memory for DbSvc_TsInfo_t.
 * @param     	void
 * @return     	if success DbSvc_TsInfo_t else NULL
 */
STATIC DbSvc_TsInfo_t *xmgr_swup_AllocTsInfo_Ses (void)
{
	return (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
}

/**
 * @description     free momory for DbSvc_TsInfo_t
 * @param     	DbSvc_TsInfo_t pointer
 * @return     	void
 */
STATIC void xmgr_swup_FreeTsInfo_Ses (void *pvTsInfo)
{
	if (pvTsInfo != NULL)
	{
		OxAP_Free (pvTsInfo);
	}
}

/**
 * @description     stop tune and release search action
 * @param     	SES swup content variable.
 * @return     	void
 */
STATIC void xmgr_swup_ReleaseAction_Ses (xmgrSwup_SesActionContents_t *pstContents)
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

#define _____DEBUG_FUNCTIONS_____

/**
 * @description     print triple id
 * @param     	triple id
 * @return     	void
 */
STATIC void	xmgr_swup_PrintTripleId_Ses (DbSvc_TripleId_t *pstTripleId)
{
#if defined(CONFIG_DEBUG)
	HxLOG_Print("(%04x:%04x:%04x)", pstTripleId->usOnId, pstTripleId->usTsId, pstTripleId->usSvcId);
#else
	(void)pstTripleId;
#endif
}

#define	_____DATA_CONVERSION_____

/**
 * @description     find ts info by triple id
 * @param     	SES swup content variable.
 * @param     	triple id
 * @param     	pointer variable for ts info
 * @return     	void
 */
STATIC HERROR xmgr_swup_FindTsInfoByTripleId_Ses (xmgrSwup_SesActionContents_t *pstContents, DbSvc_TripleId_t *pstTripleId, DbSvc_TsInfo_t *pstSvcTsInfo)
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


#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____

//eMEVT_BUS_CREATE
/**
 * @description     init contents and start ota action.
 * @param     	SES boot contents
 * @param     	Action handle
 * @param     	tuner id
 * @param     	swup handle (in xmgr_swup_ses.c)
 * @return     	ERR_BUS_NO_ERROR
 */
STATIC HERROR xmgr_swup_MsgGwmCreate_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate)
{
	HERROR		 hErr;

	// Create시 해 주어야 하는 Initialize 등이 실패할 경우에는 이 Action 자체가 살아 있어서는 안된다.
	// 곧바로 Destroy 해 주도록 한다.

	// 1. Initialize the contents
	hErr = xmgr_swup_InitContents_Ses (pstContents, hAction, hSWUpdate);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		pstContents->bForcedStop = FALSE;
		xmgr_swup_SwDetectFinish_Ses(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
		return hErr;
	}

	// 2. Start the OTA Action :
	hErr = xmgr_swup_StartAction_Ses (pstContents);

	if (hErr != ERR_BUS_NO_ERROR)
	{
		pstContents->bForcedStop = FALSE;
		xmgr_swup_SwDetectFinish_Ses(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Action);
		return hErr;
	}

	if(pstContents->eSiSwupMode != eSI_SWUP_MODE_SES_BURST_TP)
	{
		MGR_SWUPINFO_ResetSearchedInfo();
	}

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY:
/**
 * @description     handle destroy event. stop action and destroy contents
 * @param     	SES boot contents
 * @return     	ERR_BUS_NO_ERROR
 */
STATIC HERROR xmgr_swup_MsgGwmDestroyed_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	// Stop all the Action
	xmgr_swup_StopAction_Ses (pstContents);

	// Destroy the contents
	xmgr_swup_DestroyContents_Ses (pstContents);

	#if 0	//sending message code is removed in octo2.0
	// Inform that the OTA action is destroyed
	if (pstContents->bForcedStop != TRUE)
	{
//		BUS_PostMessage (NULL, MSG_APP_OTA_ACTION_DESTROYED, (Handle_t)NULL, (HINT32)BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()), 0, 0);
	}
	#endif

	return MESSAGE_BREAK;
}

// eMEVT_BUS_TIMER:
/**
 * @description     handle locking, si timer
 * @param     	SES boot contents
 * @param     	timer id
 * @return     	ERR_BUS_NO_ERROR
 */
STATIC HERROR xmgr_swup_MsgGwmTimer_Ses (xmgrSwup_SesActionContents_t *pstContents, HINT32 nTimerId)
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
			hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Ses (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[local_astraota_MsgGwmTimer] No Next Tp left to tune\n");
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
				hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_swup_TuneSearchTp_Ses (pstContents);
				}
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[local_astraota_MsgGwmTimer] Cannot proceed anymore\n");
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
		pstContents->bForcedStop = FALSE;
		xmgr_swup_SwDetectFinish_Ses(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}

	return hRetValue;
}


// eSEVT_SI_SEARCH_FAIL:
/**
 * @description     handle ota fail event. if current tp is last tp, finish ota action, else tune next tp
 * @param     	SES boot contents
 * @param     	Action handle
 * @return     	MESSAGE_BREAK
 */
STATIC HERROR xmgr_swup_EvtSiSearchFail_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction)
{
	HERROR				 hErr;

	hErr = xmgr_swup_CheckActionId_Ses (pstContents, hAction);
	if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_waitingSi:
		// SI failed : Kill the timer

		BUS_KillTimer (OTA_TIMER_SI_ID);

		pstContents->eAppState = eOtaApp_state_siFailed;

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_swup_TuneSearchTp_Ses (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[local_astraota_EvtSiSearchFail] No Next Tp left to tune\n");
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
		xmgr_swup_SwDetectFinish_Ses(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}

	return MESSAGE_BREAK;
}

// EVT_SI_OTA_SUCCESS:
/**
 * @description     handle ota succes event
 * @param     	SES boot contents
 * @param     	Action handle
 * @return     	MESSAGE_BREAK
 */
STATIC HERROR xmgr_swup_EvtSiOtaSuccess_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction)
{
	HERROR				 hErr = ERR_FAIL;
	DbSvc_TsInfo_t		*pstTsInfo = NULL, *pstHomeTsInfo = NULL;

	hErr = xmgr_swup_CheckActionId_Ses (pstContents, hAction);
	if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }


	switch (pstContents->eAppState)
	{
	case eOtaApp_state_waitingSi:
		// SI succeeded : Kill the timer

		BUS_KillTimer (OTA_TIMER_SI_ID);

		pstContents->eAppState = eOtaApp_state_siReceived;

		switch (pstContents->eSiSwupMode)
		{
		case eSI_SWUP_MODE_SES_HOME_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInAnchorTp_Ses (pstContents);
			break;

		case eSI_SWUP_MODE_SES_REFNIT_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInRefNitTp_Ses (pstContents);
			break;

		case eSI_SWUP_MODE_SES_SSUBAT_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInSsuBatTp_Ses (pstContents);
			break;

		case eSI_SWUP_MODE_SES_SSUSVC_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Ses (pstContents);
			break;

		case eSI_SWUP_MODE_SES_BURST_TP:
			hErr = xmgr_swup_EvtSiOtaSuccessInBurstUpdateTp_Ses (pstContents);
			break;

		default:
			break;
		}

		if (pstContents->eFuncMode == eSesHdFuncMode_ColdBoot)
		{
			// Boot OTA 중 HUMAX OUI가 발견되었다면 더 이상의 검색을 멈추고 오직 SSU SVC만을 체크하도록 한다.
			if (xmgr_swup_GetCurrentSsuLink_Ses (pstContents) != NULL)
			{
				// 현재 Home TP는 중요한 정보이므로 이거만은 남기도록 한다.
				pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Ses (pstContents);
				if (pstTsInfo != NULL)
				{
					pstHomeTsInfo = xmgr_swup_AllocTsInfo_Ses();
					if (pstHomeTsInfo != NULL)
					{
						*pstHomeTsInfo = *pstTsInfo;
					}
				}

				xmgr_swup_ClearAllHomeTsInfo_Ses (pstContents);
				xmgr_swup_ClearAllRefNitLink_Ses (pstContents);
				xmgr_swup_ClearAllSsuScanLink_Ses (pstContents);

				if (pstHomeTsInfo != NULL)
				{
					xmgr_swup_AddHomeTsInfo_Ses (pstContents, pstHomeTsInfo);
				}
			}

		}

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		if (pstContents->eAppState != eOtaApp_state_checkend)
		{
			hErr = xmgr_swup_TuneSearchTp_Ses (pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("[local_astraota_EvtSiSearchDone] No Next Tp left to tune\n");
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
		if((pstContents->eSiSwupMode != eSI_SWUP_MODE_SES_SSUSVC_TP)
			|| ((pstContents->eSiSwupMode == eSI_SWUP_MODE_SES_SSUSVC_TP) && (hErr != ERR_OK)))
		{
			pstContents->bForcedStop = FALSE;
			xmgr_swup_SwDetectFinish_Ses(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
		}
	}

	return MESSAGE_BREAK;
}

/**
 * @description     when si ota mode is eSI_SWUP_MODE_SES_HOME_TP, handle ota success event.
 * @param     	SES boot contents
 * @return     	ERR_BUS_NO_ERROR or ERR_FAIL
 */
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInAnchorTp_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	HUINT32				 ulIndex, ulDataNum;
	XsvcSi_SwupMode_e		 eNextOtaMode;
	DbSvc_TsInfo_t		 stTsInfo;
	DbSvc_TsInfo_t		*pstHomeTsInfo;
	DbSvc_TsInfo_t		*pstTsInfo, *pstTsInfoArray;
	DbSvc_NetInfo_t		*pstNetInfoArray;
	DbSvc_TripleId_t	*pstTripleIdArray;
	HERROR				 hErr;

	eNextOtaMode = eSI_SWUP_MODE_SES_HOME_TP;

	pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Ses (pstContents);
	if (pstHomeTsInfo != NULL)
	{
		pstContents->ulAntennaIndex = (HUINT32)pstHomeTsInfo->stTuningParam.sat.ucAntId;

		// Network Info
		ulDataNum = 0;
		pstNetInfoArray = NULL;
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_NETWORK_NETINFO, &ulDataNum, (void **)&pstNetInfoArray);
		if (hErr == ERR_OK && pstNetInfoArray != NULL)
		{
			if (pstContents->eFuncMode == eSesHdFuncMode_ColdBoot)
			{
				XMGR_SEARCH_INFO_AddNetworkInfo (&(pstNetInfoArray[0]));
			}

			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_NETWORK_NETINFO, (void *)pstNetInfoArray);
		}

		// Delivery TS Info
		ulDataNum = 0;
		pstTsInfoArray = NULL;
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, &ulDataNum, (void **)&pstTsInfoArray);
		if (hErr == ERR_OK && pstTsInfoArray != NULL)
		{
			HxLOG_Print("[astraota] NIT TS Info : %d\n", ulDataNum);

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
						pstTsInfo->stTuningParam.sat.eSatType = pstHomeTsInfo->stTuningParam.sat.eSatType;
					}

					if (pstContents->eFuncMode == eSesHdFuncMode_ColdBoot)
					{
						XMGR_SEARCH_INFO_AddDeliveryTsInfo(pstTsInfo);
					}

//					HxLOG_Print("[astraota] TP (%d) ", ulIndex);
//					local_sesota_PrintTsInfo (pstTsInfo);
//					HxLOG_Print("\n");

					pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTsInfoListFromSi, (void *)pstTsInfo);
				}
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, (void *)pstTsInfoArray);
		}
		else
		{
			goto END_PROCESS;
		}

		// 다음 State는 SSU-SVC -> SSU-BAT -> Reference NIT가 발견된 순서에 따른다.

		// SSU-SVC
		pstTripleIdArray = NULL;
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec,eSI_SWUP_DATA_SSU_LINK_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
		if (hErr == ERR_OK && pstTripleIdArray != NULL)
		{
			HxLOG_Print("[sesota_act] SSU-SVC : %d\n", ulDataNum);

			if (eNextOtaMode == eSI_SWUP_MODE_SES_HOME_TP)
			{
				eNextOtaMode = eSI_SWUP_MODE_SES_SSUSVC_TP;
			}

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				xmgr_swup_AddSsuLink_Ses (pstContents, &(pstTripleIdArray[ulIndex]));

				HxLOG_Print("[sesota_act] SSU-SVC (%d) ", ulIndex);
				xmgr_swup_PrintTripleId_Ses (&(pstTripleIdArray[ulIndex]));
				HxLOG_Print("\n");
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, (void *)pstTripleIdArray);
		}

		// SSU-BAT
		pstTripleIdArray = NULL;
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCAN_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
		if (hErr == ERR_OK && pstTripleIdArray != NULL)
		{
			HxLOG_Print("[sesota_act] SSU-BAT : %d\n", ulDataNum);
			if (eNextOtaMode == eSI_SWUP_MODE_SES_HOME_TP)
			{
				eNextOtaMode = eSI_SWUP_MODE_SES_SSUBAT_TP;
			}

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				xmgr_swup_AddSsuScanLink_Ses (pstContents, &(pstTripleIdArray[ulIndex]));

				HxLOG_Print("[sesota_act] SSU-BAT (%d) ", ulIndex);
				xmgr_swup_PrintTripleId_Ses (&(pstTripleIdArray[ulIndex]));
				HxLOG_Print("\n");
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCAN_TRIPLEID, (void *)pstTripleIdArray);
		}

		// Reference-NIT
		pstTripleIdArray = NULL;
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec,eSI_SWUP_DATA_REFNIT_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
		if (hErr == ERR_OK && pstTripleIdArray != NULL)
		{
			HxLOG_Print("[sesota] REF-NIT : %d\n", ulDataNum);
			if (eNextOtaMode == eSI_SWUP_MODE_SES_HOME_TP)
			{
				eNextOtaMode = eSI_SWUP_MODE_SES_REFNIT_TP;
			}

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				xmgr_swup_AddRefNitLink_Ses (pstContents, &(pstTripleIdArray[ulIndex]));

				HxLOG_Print("[sesota_act] REF-NIT (%d) ", ulIndex);
				xmgr_swup_PrintTripleId_Ses (&(pstTripleIdArray[ulIndex]));
				HxLOG_Print("\n");


				// Booting Sequence 중이면 Reference NIT TS를 저장해두도록 한다.
				if (pstContents->eFuncMode == eSesHdFuncMode_ColdBoot)
				{
					hErr = xmgr_swup_FindTsInfoByTripleId_Ses (pstContents, &(pstTripleIdArray[ulIndex]), &stTsInfo);
					if (hErr == ERR_OK)
					{
						XMGR_SEARCH_INFO_AddAstraReferenceTsInfo (&stTsInfo);
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
						XMGR_SEARCH_INFO_AddAstraPortalReferenceTsInfo(&stTsInfo);
#endif
					}
				}
			}

			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_REFNIT_TRIPLEID, (void *)pstTripleIdArray);
		}


		// Booting Sequence 중이면 SGT Ts Info도 넣도록 한다.
		if (pstContents->eFuncMode == eSesHdFuncMode_ColdBoot)
		{
			pstTripleIdArray = NULL;
			hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec,eSI_SWUP_DATA_SESLCN_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
			if (hErr == ERR_OK && pstTripleIdArray != NULL)
			{
				for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
				{
					hErr = xmgr_swup_FindTsInfoByTripleId_Ses (pstContents, &(pstTripleIdArray[ulIndex]), &stTsInfo);
					if (hErr == ERR_OK)
					{
						HxLOG_Print("[sesota_act] Add SGT Link (%02d) : (0x%04x:0x%04x:0x%04x)\n", ulDataNum, pstTripleIdArray[ulIndex].usOnId, pstTripleIdArray[ulIndex].usTsId, pstTripleIdArray[ulIndex].usSvcId);
						XMGR_SEARCH_INFO_AddAstraSgtTsInfo (&stTsInfo, &(pstTripleIdArray[ulIndex]));
					}
				}
				SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SESLCN_TRIPLEID, (void *)pstTripleIdArray);
			}
		}
	}

END_PROCESS:

	if (eNextOtaMode != eSI_SWUP_MODE_SES_HOME_TP)
	{
		xmgr_swup_SetSiOtaMode_Ses (pstContents, eNextOtaMode);
	}
	else
	{
		hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
		if (hErr != ERR_OK) 		// 모든 TP를 다 체크함
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;

}

/**
 * @description     when si ota mode is eSI_SWUP_MODE_SES_REFNIT_TP, handle ota success event.
 * @param     	SES boot contents
 * @return     	ERR_BUS_NO_ERROR or ERR_FAIL
 */
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInRefNitTp_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	HUINT32				 ulIndex, ulDataNum;
	XsvcSi_SwupMode_e		 eNextOtaMode;
	DbSvc_TsInfo_t		 stTsInfo;
//	DbSvc_TsInfo_t		*pstNitTsInfo;
	DbSvc_TsInfo_t		*pstHomeTsInfo;
	DbSvc_TsInfo_t		*pstTsInfo, *pstTsInfoArray;
	DbSvc_NetInfo_t		*pstNetInfoArray;
	DbSvc_TripleId_t	*pstTripleIdArray;
	HERROR				 hErr;

	eNextOtaMode = eSI_SWUP_MODE_SES_REFNIT_TP;

	pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Ses (pstContents);
	if(pstHomeTsInfo != NULL)
	{
		pstContents->ulAntennaIndex = (HUINT32)pstHomeTsInfo->stTuningParam.sat.ucAntId;

		// Network Info
		ulDataNum = 0;
		pstNetInfoArray = NULL;
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_NETWORK_NETINFO, &ulDataNum, (void **)&pstNetInfoArray);
		if (hErr == ERR_OK && pstNetInfoArray != NULL)
		{
			if (pstContents->eFuncMode == eSesHdFuncMode_ColdBoot)
			{
				XMGR_SEARCH_INFO_AddNetworkInfo(&(pstNetInfoArray[0]));
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_NETWORK_NETINFO, (void *)pstNetInfoArray);
		}

		ulDataNum = 0;
		pstTsInfoArray = NULL;
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, &ulDataNum, (void **)&pstTsInfoArray);
		if (hErr == ERR_OK && pstTsInfoArray != NULL)
		{
			HxLOG_Print("[sesota] NIT TS Info : %d\n", ulDataNum);

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

					if (pstContents->eFuncMode == eSesHdFuncMode_ColdBoot)
					{
						XMGR_SEARCH_INFO_AddDeliveryTsInfo(pstTsInfo);
					}
//					HxLOG_Print("[sesota_act] TP (%d) ", ulIndex);
//					local_sesota_PrintTsInfo (&(pstTripleIdArray[ulIndex]));
//					HxLOG_Print("\n");

					pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTsInfoListFromSi, (void *)pstTsInfo);
				}
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, (void *)pstTsInfoArray);
		}
		else
		{
			goto END_PROCESS;
		}

		// 다음 State는 SSU-SVC -> SSU-BAT 가 발견된 순서에 따른다.

		// SSU-SVC
		pstTripleIdArray = NULL;
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
		if (hErr == ERR_OK && pstTripleIdArray != NULL)
		{
			HxLOG_Print("[sesota_act] SSU-SVC : %d\n", ulDataNum);

			if (eNextOtaMode == eSI_SWUP_MODE_SES_REFNIT_TP)
			{
				eNextOtaMode = eSI_SWUP_MODE_SES_SSUSVC_TP;
			}

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				xmgr_swup_AddSsuLink_Ses (pstContents, &(pstTripleIdArray[ulIndex]));

				HxLOG_Print("[sesota_act] SSU-SVC (%d) ", ulIndex);
				xmgr_swup_PrintTripleId_Ses (&(pstTripleIdArray[ulIndex]));
				HxLOG_Print("\n");
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, (void *)pstTripleIdArray);
		}

		// SSU-BAT
		pstTripleIdArray = NULL;
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCAN_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
		if (hErr == ERR_OK && pstTripleIdArray != NULL)
		{
			HxLOG_Print("[sesota_act] SSU-BAT : %d\n", ulDataNum);
			if (eNextOtaMode == eSI_SWUP_MODE_SES_REFNIT_TP)
			{
				eNextOtaMode = eSI_SWUP_MODE_SES_SSUBAT_TP;
			}

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				xmgr_swup_AddSsuLink_Ses (pstContents, &(pstTripleIdArray[ulIndex]));

				HxLOG_Print("[sesota_act] SSU-BAT (%d) ", ulIndex);
				xmgr_swup_PrintTripleId_Ses (&(pstTripleIdArray[ulIndex]));
				HxLOG_Print("\n");
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCAN_TRIPLEID, (void *)pstTripleIdArray);
		}

		// Booting Sequence 중이면 SGT Ts Info도 넣도록 한다.
		if (pstContents->eFuncMode == eSesHdFuncMode_ColdBoot)
		{
			pstTripleIdArray = NULL;
			hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SESLCN_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
			if (hErr == ERR_OK && pstTripleIdArray != NULL)
			{
				for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
				{
					hErr = xmgr_swup_FindTsInfoByTripleId_Ses (pstContents, &(pstTripleIdArray[ulIndex]), &stTsInfo);
					if (hErr == ERR_OK)
					{
						HxLOG_Print("[sesota_act] Add SGT Link (%02d) : (0x%04x:0x%04x:0x%04x)\n", ulDataNum, pstTripleIdArray[ulIndex].usOnId, pstTripleIdArray[ulIndex].usTsId, pstTripleIdArray[ulIndex].usSvcId);
						XMGR_SEARCH_INFO_AddAstraSgtTsInfo(&stTsInfo, &(pstTripleIdArray[ulIndex]));
					}
				}
				SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SESLCN_TRIPLEID, (void *)pstTripleIdArray);
			}
		}


#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
		// Booting Sequence 중이면 Astra Portal Ts Info도 넣도록 한다.
		if (pstContents->eFuncMode == eSesHdFuncMode_ColdBoot)
		{
			pstTripleIdArray = NULL;
			hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_ASTAR_PORTAL_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
			if (hErr == ERR_OK && pstTripleIdArray != NULL)
			{
				HxLOG_Print("[sesota_act] AstraPortal LINK : %d\n", ulDataNum);

				for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
				{
					hErr = xmgr_swup_FindTsInfoByTripleId_Ses (pstContents, &(pstTripleIdArray[ulIndex]), &stTsInfo);
					if (hErr == ERR_OK)
					{
						XMGR_SEARCH_INFO_AddAstraPortalTsInfo (&stTsInfo, &(pstTripleIdArray[ulIndex]));
					}
				}
				SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_ASTAR_PORTAL_TRIPLEID, (void *)pstTripleIdArray);
			}
		}

		// Booting Sequence 중이면 Astra Regional Protal Ts Info도 넣도록 한다.
		if (pstContents->eFuncMode == eSesHdFuncMode_ColdBoot)
		{
			pstTripleIdArray = NULL;
			hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_ASTAR_REGIONAL_PORTAL_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
			if (hErr == ERR_OK && pstTripleIdArray != NULL)
			{
				HxLOG_Print("[sesota_act] AstraRegionalPortal LINK : %d\n", ulDataNum);

				for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
				{
					hErr = xmgr_swup_FindTsInfoByTripleId_Ses (pstContents, &(pstTripleIdArray[ulIndex]), &stTsInfo);
					if (hErr == ERR_OK)
					{
						XMGR_SEARCH_INFO_AddAstraRegionalPortalTsInfo (&stTsInfo, &(pstTripleIdArray[ulIndex]));
					}
				}
				SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_ASTAR_REGIONAL_PORTAL_TRIPLEID, (void *)pstTripleIdArray);
			}
		}
#endif
	}


	END_PROCESS:

		if (eNextOtaMode != eSI_SWUP_MODE_SES_REFNIT_TP)
		{
			xmgr_swup_SetSiOtaMode_Ses (pstContents, eNextOtaMode);
		}
		else
		{
			hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
			if (hErr != ERR_OK) 		// 모든 TP를 다 체크함
			{
				pstContents->eAppState = eOtaApp_state_checkend;
				return ERR_FAIL;
			}
		}

		return ERR_BUS_NO_ERROR;
}

/**
 * @description     when si ota mode is eSI_SWUP_MODE_SES_SSUBAT_TP, handle ota success event.
 * @param     	SES boot contents
 * @return     	ERR_BUS_NO_ERROR or ERR_FAIL
 */
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSsuBatTp_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	HBOOL				 bSiDataFound;
	HUINT32				 ulIndex, ulDataNum;
	DbSvc_TripleId_t	*pstTripleIdArray;
	HERROR				 hErr;

	bSiDataFound = TRUE;

	// SSU-SVC
	hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec,eSI_SWUP_DATA_SSU_LINK_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
	if (hErr == ERR_OK && pstTripleIdArray != NULL)
	{
		HxLOG_Print("[astraota] SSU-SVC : %d\n", ulDataNum);

		for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
		{
			xmgr_swup_AddSsuLink_Ses (pstContents, &(pstTripleIdArray[ulIndex]));

			HxLOG_Print("[sesota] SSU-SVC (%d) ", ulIndex);
			xmgr_swup_PrintTripleId_Ses (&(pstTripleIdArray[ulIndex]));
			HxLOG_Print("\n");
		}
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, (void *)pstTripleIdArray);
	}
	else
	{
		bSiDataFound = FALSE;
	}

	if (bSiDataFound == TRUE)
	{
		xmgr_swup_SetSiOtaMode_Ses (pstContents, eSI_SWUP_MODE_SES_SSUSVC_TP);
	}
	else
	{
		hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
		if (hErr != ERR_OK)			// 모든 TP를 다 체크함
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;
}

/**
 * @description     when si ota mode is eSI_SWUP_MODE_SES_SSUSVC_TP, handle ota success event.
 * @param     	SES boot contents
 * @return     	ERR_BUS_NO_ERROR or ERR_FAIL
 */
STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSsuSvcTp_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	HERROR				 hErr, hResult = ERR_BUS_NO_ERROR;
	HUINT32				 ulDataNum = 0, ulScheduleNum = 0;
	SvcSi_SwupSignal_t	*pstOtaArray = NULL, *pstOtaInfo = NULL;
	DxTuneParam_t		 stChTuneInfo;
	DbSvc_TsInfo_t		*pstHomeTsInfo = NULL, *pstOtaTsInfo	= NULL;
	DbSvc_TripleId_t	*pstTripleId = NULL;
	SvcSi_SwupSchedule_t		*pstSchedule = NULL;
	SvcSwUpdate_DetectResult_t	*pstDetectResult = NULL;

	// SSU-SVC
	hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec,eSI_SWUP_DATA_SSU_SIGNAL, &ulDataNum, (void **)&pstOtaArray);
	if (hErr == ERR_OK && pstOtaArray != NULL && ulDataNum > 0)
	{
		pstOtaInfo = (SvcSi_SwupSignal_t*)HLIB_STD_MemAlloc(sizeof(SvcSi_SwupSignal_t));
		if(pstOtaInfo == NULL)
		{
			hResult = ERR_FAIL;
			goto ERROR;
		}

		HxSTD_MemSet(pstOtaInfo, 0, sizeof(SvcSi_SwupSignal_t));
		HxSTD_MemCopy(pstOtaInfo, pstOtaArray, sizeof(SvcSi_SwupSignal_t));

		// 찾은 OTA 정보를 기록한다.
		HxLOG_Critical("[sesota_act] Found OTA Info \n");
		HxLOG_Critical("[sesota_act] Software update type : %d\n", pstOtaInfo->ucSwupType);
		HxLOG_Critical("[sesota_act] System ID : 0x%08x ~ 0x%08x \n", pstOtaInfo->ulStartSystemId, pstOtaInfo->ulEndSystemId);
		HxLOG_Critical("[sesota_act] System Ver : 0x%08x \n", pstOtaInfo->ulDataVersion);
		HxLOG_Critical("[sesota_act] PID : %04x \n", pstOtaInfo->usPid);

		pstOtaTsInfo = (DbSvc_TsInfo_t*)HLIB_STD_MemAlloc(sizeof(DbSvc_TsInfo_t));
		if(pstOtaTsInfo == NULL)
		{
			hResult = ERR_FAIL;
			goto ERROR;
		}

		// Write OTA TS Info
		HxSTD_MemSet(pstOtaTsInfo, 0, sizeof(DbSvc_TsInfo_t));

		if(pstOtaInfo->ucSwupType == SvcSi_SWUP_TYPE_BURST_USER_SELECT || pstOtaInfo->ucSwupType == SvcSi_SWUP_TYPE_BURST_FORCED)
		{
			hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec,eSI_SWUP_DATA_SSU_SCHEDULE_INFO, &ulScheduleNum, (void **)&pstSchedule);
			if (hErr == ERR_OK && ulScheduleNum > 0)
			{
				// TO DO : Compare VESRION & Info, update Schedule
				(void) DB_PARAM_SetItem (eDB_PARAM_ITEM_OTA_SCHEDULEINFO, (HUINT32)pstSchedule, (HUINT32)sizeof(SvcSi_SwupSchedule_t));
				//temp
				pstOtaInfo->ulDataVersion = pstSchedule->ulDataVersion;
				hErr = xmgr_swup_FindTsInfoByTripleId_Ses (pstContents, &(pstSchedule->stTripleId), pstOtaTsInfo);
				if (ERR_OK != hErr)
				{
					HxLOG_Critical("xmgr_swup_FindTsInfoByTripleId_Ses err:\n");
				}
			}
			else
			{
				hResult = ERR_FAIL;
				goto ERROR;
			}
		}
		else
		{
			hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
			if (hErr != ERR_OK)
			{
				stChTuneInfo = pstContents->stTuningInfo;
			}

			OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&stChTuneInfo, &pstOtaTsInfo->stTuningParam);

			pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Ses (pstContents);
			if (pstHomeTsInfo != NULL)
			{
				pstOtaTsInfo->eDeliType 				= pstHomeTsInfo->eDeliType;
				pstOtaTsInfo->ucTunerId 				= pstHomeTsInfo->ucTunerId;
				pstOtaTsInfo->stTuningParam.sat.ucAntId = pstHomeTsInfo->stTuningParam.sat.ucAntId;
			}

			pstTripleId = xmgr_swup_GetCurrentSsuLink_Ses (pstContents);
			if (pstTripleId != NULL)
			{
				pstOtaTsInfo->usOnId = pstTripleId->usOnId;
				pstOtaTsInfo->usTsId = pstTripleId->usTsId;
			}
			else
			{
				HxLOG_Critical("\n\n");
			}
		}

		MGR_SWUPINFO_SetSearchedInfo(pstOtaTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)pstOtaInfo);

		pstDetectResult = (SvcSwUpdate_DetectResult_t *)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
		if (pstDetectResult == NULL)
		{
			HxLOG_Error("Memory alloc fail!!\n");
			hResult = ERR_FAIL;
			goto ERROR;
		}

		pstDetectResult->bAvailable = TRUE;
		pstDetectResult->bFound	= TRUE;

		// emergency set
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stChTuningInfo, &stChTuneInfo, sizeof(DxTuneParam_t));
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stSvcTsInfo, pstOtaTsInfo, sizeof(DbSvc_TsInfo_t));
		pstDetectResult->data.stRfInfo.eDeliType	 = eDxDELIVERY_TYPE_SAT;
		pstDetectResult->data.stRfInfo.usPid		 = pstOtaInfo->usPid;
		pstDetectResult->data.stRfInfo.eDownloadType = eSWUP_DOWNLOAD_DVBSSU;

		pstDetectResult->ulDataVersion	= pstOtaInfo->ulDataVersion;
		pstDetectResult->usOtaPid		= pstOtaInfo->usPid;
		pstDetectResult->eSource		= eDxSWUPDATE_SOURCE_Rf;
		MGR_SWUP_SetDetectInfo(pstContents->hSWUpdate, pstDetectResult);

		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32)pstDetectResult, sizeof(SvcSwUpdate_DetectResult_t));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x]\n", hErr);
			hResult = ERR_FAIL;
			goto ERROR;
		}

		xmgr_swup_SwDetectFinish_Ses(pstContents, DETECT_SUCCESS, (HINT32 *)pstOtaTsInfo,(HINT32 *)pstOtaInfo, eSWUP_FAILREASON_Unknown);

	}
	else
	{
		hResult = ERR_FAIL;
		goto ERROR;
	}

ERROR:
	if(pstOtaArray)
	{
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SIGNAL, (void *)pstOtaArray);
	}

	if(pstSchedule)
	{
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCHEDULE_INFO, (void *)pstSchedule);
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
		xmgr_swup_SwDetectFinish_Ses(pstContents, DETECT_FAIL, 0, 0, eSWUP_FAILREASON_Unknown);

		hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[%s] all tp checked & set eapp state to eOtaApp_state_checkend\n", __FUNCTION__);
			pstContents->eAppState = eOtaApp_state_checkend;
		}
	}

	return hErr;
}

STATIC HERROR xmgr_swup_EvtSiOtaSuccessInBurstUpdateTp_Ses (xmgrSwup_SesActionContents_t *pstContents)
{
	HERROR	hErr;
	HUINT32 ulDataNum = 0;
	HUINT16 *pstPidArray = NULL;
	DbSvc_TsInfo_t		stTsInfo;
	DxTuneParam_t		stChTuneInfo;
	SvcSi_SwupSignal_t	stOtaInfo;
	SvcSwUpdate_DetectResult_t	*pstDetectResult = NULL;

	hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_BURST_INFO, &ulDataNum, (void **)&pstPidArray);

	if(hErr == ERR_OK && ulDataNum && pstPidArray != NULL)
	{
		hErr = MGR_SWUPINFO_GetSearchedInfo (&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);

		if(hErr == ERR_OK)
		{
			stOtaInfo.usPid = pstPidArray[0];
			MGR_SWUPINFO_SetSearchedInfo(&stTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);

			pstDetectResult = (SvcSwUpdate_DetectResult_t *)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
			if (pstDetectResult == NULL)
			{
				HxLOG_Error("Memory alloc fail!!\n");
				return ERR_FAIL;
			}

			pstDetectResult->bAvailable = TRUE;
			pstDetectResult->bFound	= TRUE;

			// emergency set
			hErr = OTL_CONV_DbTuningInfo2DxTuneParam ((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
			if(hErr == ERR_OK)
			{
				HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stChTuningInfo, &stChTuneInfo, sizeof(DxTuneParam_t));
			}

			HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stSvcTsInfo, &stTsInfo, sizeof(DbSvc_TsInfo_t));
			pstDetectResult->data.stRfInfo.eDeliType	 = eDxDELIVERY_TYPE_SAT;
			pstDetectResult->data.stRfInfo.usPid		 = stOtaInfo.usPid;
			pstDetectResult->data.stRfInfo.eDownloadType = eSWUP_DOWNLOAD_DVBSSU;

			pstDetectResult->ulDataVersion	= stOtaInfo.ulDataVersion;
			pstDetectResult->usOtaPid	 	= stOtaInfo.usPid;
			pstDetectResult->eSource		= eDxSWUPDATE_SOURCE_Rf;
			MGR_SWUP_SetDetectInfo(pstContents->hSWUpdate, pstDetectResult);

			xmgr_swup_SwDetectFinish_Ses(pstContents, DETECT_SUCCESS, NULL,NULL, eSWUP_FAILREASON_Unknown);

			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_BURST_INFO, (void *)pstPidArray);

			if(pstDetectResult != NULL)
			{
				HLIB_STD_MemFree(pstDetectResult);
			}

			return ERR_BUS_NO_ERROR;
		}
	}

	if(pstPidArray != NULL)
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_BURST_INFO, (void *)pstPidArray);

	hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
	if (hErr != ERR_OK) 		// 모든 TP를 다 체크함
	{
		HxLOG_Print("[%s] all tp checked & set eapp state to eOtaApp_state_checkend\n", __FUNCTION__);
		pstContents->eAppState = eOtaApp_state_checkend;
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

// eSEVT_CH_LOCKED
/**
 * @description     handle channel lock event. set si timer and start si ota
 * @param     	SES boot contents
 * @param     	Action handle
 * @return     	ERR_BUS_NO_ERROR or ERR_FAIL
 */

STATIC HERROR xmgr_swup_EvtChLocked_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction)
{
	HERROR				 hErr;

	hErr = xmgr_swup_CheckActionId_Ses (pstContents, hAction);
	if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }


	switch (pstContents->eAppState)
	{
	case eOtaApp_state_tryTuning :
		// Channel Locked : Kill the timer
		BUS_KillTimer (OTA_TIMER_LOCKING_ID);

		// Start the SI for the ASTRA OTA
		hErr = xmgr_swup_StartSi_Ses (pstContents);
		if (hErr == ERR_OK)
		{
			pstContents->eAppState = eOtaApp_state_waitingSi;
			// MW SI를 무한으로 기다리는 것을 막기 위해 여기에서 Timer를 걸어야 한다.
			BUS_SetTimer (OTA_TIMER_SI_ID, OTA_TIMER_SI_TIME);
		}
		else
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Ses (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[local_astraota_EvtChLocked] No Next Tp left to tune\n");
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
		pstContents->bForcedStop = FALSE;
		xmgr_swup_SwDetectFinish_Ses(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}
	return MESSAGE_BREAK;
}

//eSEVT_CH_NO_SIGNAL
/**
 * @description     handle channel lock event. tune next tp
 * @param     	SES boot contents
 * @param     	Action handle
 * @return     	ERR_BUS_NO_ERROR or ERR_FAIL
 */
STATIC HERROR xmgr_swup_EvtChNoSignal_Ses (xmgrSwup_SesActionContents_t *pstContents, Handle_t hAction)
{
	HERROR				 hErr;

	hErr = xmgr_swup_CheckActionId_Ses (pstContents, hAction);
	if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }


	switch (pstContents->eAppState)
	{
	case eOtaApp_state_tryTuning :
		// Channel Locked : Kill the timer

		BUS_KillTimer (OTA_TIMER_LOCKING_ID);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_swup_TuneSearchTp_Ses (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[%s] No Next Tp left to tune\n", __FUNCTION__);
			pstContents->eAppState = eOtaApp_state_checkend;
		}

		break;

	case eOtaApp_state_waitingSi:
		// Stop SI :
		hErr = SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
		if (hErr == ERR_OK)
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_SetNextTpForOta_Ses (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Ses (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[%s] No Next Tp left to tune\n", __FUNCTION__);
				pstContents->eAppState = eOtaApp_state_checkend;
			}
		}
		else
		{
			// 더 이상 진행 불가능 : Search End로 간주한다.
			HxLOG_Print("[%s] SI Re-open failed.\n", __FUNCTION__);
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
		pstContents->bForcedStop = FALSE;
		xmgr_swup_SwDetectFinish_Ses(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}
	return MESSAGE_BREAK;
}

//eMEVT_BUS_GO_OPERATION
/**
 * @description     handle channel lock event. tune next tp
 * @param     	SES boot contents
 * @param     	Action handle
 * @return     	ERR_BUS_NO_ERROR or ERR_FAIL
 */
STATIC HERROR xmgr_swup_MsgGwmQuitStandby_Ses  (xmgrSwup_SesActionContents_t *pstContents)
{
	pstContents->bForcedStop = TRUE;
	BUS_MGR_Destroy (NULL);

	return ERR_OK;
}

/**
 * @description     finish ota acction, send eMEVT_SWUP_NOTIFY_DETECT_SUCCESS to swUpdate proc
 * @param     	SES boot contents
 * @param     	detect result (DETECT_SUCCESS/DETECT_FAIL)
 * @param     	ota ts info (it is exist only when ota success)
 * @param     	ota info (it is exist only when ota success)
 * @return     	ERR_BUS_NO_ERROR or ERR_FAIL
 */
STATIC HERROR xmgr_swup_SwDetectFinish_Ses (xmgrSwup_SesActionContents_t *pstContents, HUINT8 ucDetectResult, HINT32 *pstOtaTsInfo, HINT32 *pstOtaInfo, MgrSwup_FailReason_e eFailReason)
{
	if(ucDetectResult == DETECT_SUCCESS)
	{
		HCHAR				szCurrentVersion[32], szNewSWVersion[32];
		HUINT32 				ulSystemVersion;
		SvcSi_SwupSignal_t			stOtaInfo;
		HERROR				hErr;
		HCHAR				*pszVersion = NULL;

		HxLOG_Debug("[%s.%d] MGR_SWUP_PostMessage &  BUS_PostMessage : eMEVT_SWUP_NOTIFY_DETECT_SUCCESS ( pstContents->hSWUpdate: 0x%X)\n", __FUNCTION__, __LINE__, pstContents->hSWUpdate);

		HxSTD_memset(&stOtaInfo, 0x00, sizeof(SvcSi_SwupSignal_t));

		SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSystemVersion);
		hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_OTAINFO, (HUINT32 *)&stOtaInfo, sizeof(SvcSi_SwupSignal_t));

		if(hErr == ERR_OK)
		{
			SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, ulSystemVersion, szCurrentVersion);
			SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, stOtaInfo.ulDataVersion, szNewSWVersion);

			if (MGR_ACTION_GetState()== eAmState_ACTIVE_STANDBY)
			{
				// standby 에서의 detected 는 처리하지 않음.
				// 이것은 download 완료 시점에 한다.
				//MWC_PARAM_SetSwUpdateVersion(TRUE, TRUE, szCurrentVersion, szNewSWVersion);
			}
			else
			{
				HCHAR	szScheduleInfo[60];
				HxSTD_memset(szScheduleInfo, 0, 60);

				if(stOtaInfo.ucSwupType == SvcSi_SWUP_TYPE_BURST_USER_SELECT || stOtaInfo.ucSwupType == SvcSi_SWUP_TYPE_BURST_FORCED)
				{
					SvcSi_SwupSchedule_t	stSchedule;

					hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_SCHEDULEINFO, (HUINT32 *)&stSchedule, (HUINT32)sizeof(SvcSi_SwupSchedule_t));
					if(hErr == ERR_OK)
					{
						HxSTD_PrintToStrN (szScheduleInfo, 60, "%s %d~%d", SYSTEM_APP_VER_HEAD, stSchedule.startDateTime[0], stSchedule.endDateTime[0]);
					}
				}

				pszVersion = (HCHAR *)xmgr_swup_GetSwUpdateVersionString_Ses(TRUE, FALSE, stOtaInfo.ucSwupType, szCurrentVersion, szNewSWVersion, szScheduleInfo);
				DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, TRUE);
			}
			HxLOG_Debug("\neMEVT_SWUP_NOTIFY_DETECT_SUCCESS!\n");
		}
		else
		{
			pszVersion = xmgr_swup_GetSwUpdateVersionString_Ses(FALSE, FALSE, 0,(HCHAR *)NULL, (HCHAR *)NULL, (HCHAR *)NULL);
			DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, 0);
			HxLOG_Debug("\neMEVT_SWUP_NOTIFY_DETECT_FAIL!\n");
		}

		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_SUCCESS, eSWUP_STATE_Connect, (HINT32)pstOtaTsInfo, (HINT32)pstOtaInfo, 0);
	}
	else
	{
		HCHAR	*pszVersion = NULL;

		HxLOG_Debug("[%s.%d] MGR_SWUP_PostMessage &  BUS_PostMessage : eMEVT_SWUP_NOTIFY_DETECT_FAIL ( pstContents->hSWUpdate: 0x%X)\n", __FUNCTION__, __LINE__, pstContents->hSWUpdate);

		pszVersion = xmgr_swup_GetSwUpdateVersionString_Ses(FALSE, FALSE, 0, (HCHAR *)NULL, (HCHAR *)NULL, (HCHAR *)NULL);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, 0);
		HxLOG_Print("[%s][%04d] eMEVT_SWUP_NOTIFY_DETECT_FAIL!\n", __FUNCTION__,__LINE__);

		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, (Handle_t)0, 0, 0, eFailReason);
	}

	return ERR_OK;
}


#define	_____EXTERNAL_PROCESS_FUNCTION_____

/**
 * @description     set default Home TP List
 * @param     	void
 * @return     	ERR_BUS_NO_ERROR
 */
HERROR xmgr_swup_SetDefaultHomeTsInfo_Ses (void)
{
	HUINT32					 ulTsCnt, ulTsNum;
	HUINT32					 ulAntIdx;
	SVC_SatType_t			 eSatType;
	DbSvc_TsInfo_t			 stTsInfo;
	HERROR					 hErr;
	DbSvc_AntInfo_t		 stAntInfo;

	MGR_SWUPINFO_ResetHomeTp ();
	HxSTD_memset (&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));

	hErr = xmgr_swup_GetAstraAntIdx_Ses (&ulAntIdx);

	if (hErr == ERR_OK)
	{
		hErr = DB_SVC_GetAntInfo ((HUINT16)ulAntIdx, &stAntInfo);
		eSatType = (hErr == ERR_OK) ? stAntInfo.eSatType : eSatType_ASTRA_1;
		ulTsNum = XMGR_SEARCH_INFO_CountAnchorTsInfo();


		for (ulTsCnt = 0; ulTsCnt < ulTsNum; ulTsCnt++)
		{
			hErr = XMGR_SEARCH_INFO_GetAnchorTsInfo (ulTsCnt, &stTsInfo);

			if (hErr == ERR_OK)
			{
				stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
				stTsInfo.ucTunerId = (HUINT8)-1;
				stTsInfo.stTuningParam.sat.ucAntId = (HUINT8)ulAntIdx;
				stTsInfo.stTuningParam.sat.eSatType = eSatType;

				MGR_SWUPINFO_AddOtaHomeTp (&stTsInfo);
			}
		}
	}

	return ERR_BUS_NO_ERROR;
}

/**
 * @description     SES SWUP action proc
 * @param     	message, action handle, param 1,2,3
 * @return     	MESSAGE_BREAK or MESSAGE_PASS, ERR_OK
 */
BUS_Result_t xproc_swup_Detect_Ses(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	STATIC HBOOL			 s_bFirstStart = TRUE;
	xmgrSwup_SesActionContents_t 	*pstContents;
	HERROR					 hErr;

	pstContents = xmgr_swup_GetContents_Ses();
	if (s_bFirstStart == TRUE)
	{
		s_bFirstStart = FALSE;
		HxSTD_memset (pstContents, 0, sizeof(xmgrSwup_SesActionContents_t));
	}

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Print("\t[MSG_GWM_CREATE] 0x%08x, (%08x:%08x:%08x)\n\n", hAction, p1, p2, p3);
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "XMGR_OTA_SES_Proc");
		// p2 is software update handle
		hErr = xmgr_swup_MsgGwmCreate_Ses (pstContents, hAction, p1, p2);
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Print("\t[MSG_GWM_TIMER] 0x%08x, (%08x:%08x:%08x)\n\n", hAction, p1, p2, p3);
		hErr = xmgr_swup_MsgGwmTimer_Ses (pstContents, p1);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Print("\t[MSG_GWM_DESTROY] 0x%08x, (%08x:%08x:%08x)\n\n", hAction, p1, p2, p3);
		hErr = xmgr_swup_MsgGwmDestroyed_Ses (pstContents);
		break;

	case eSEVT_SWUP_DETECT_SUCCESS:
		HxLOG_Print("\t[eSEVT_SWUP_DETECT_SUCCESS] 0x%08x, (%08x:%08x:%08x)\n\n", hAction, p1, p2, p3);
		hErr = xmgr_swup_EvtSiOtaSuccess_Ses (pstContents, hAction);
		break;


	case eSEVT_SWUP_DETECT_FAIL:
		HxLOG_Print("\t[eSEVT_SWUP_DETECT_FAIL] 0x%08x, (%08x:%08x:%08x)\n\n", hAction, p1, p2, p3);
		hErr = xmgr_swup_EvtSiSearchFail_Ses (pstContents, hAction);
		break;

	case eSEVT_CH_LOCKED:
		HxLOG_Print("\t[eSEVT_CH_LOCKED] 0x%08x, (%08x:%08x:%08x)\n\n", hAction, p1, p2, p3);
		hErr = xmgr_swup_EvtChLocked_Ses (pstContents, hAction);
		break;

	case eSEVT_CH_NO_SIGNAL:
		HxLOG_Print("\t[eSEVT_CH_NO_SIGNAL] 0x%08x, (%08x:%08x:%08x)\n\n", hAction, p1, p2, p3);
		hErr = xmgr_swup_EvtChNoSignal_Ses (pstContents, hAction);
		break;

	case eMEVT_BUS_GO_OPERATION :
		HxLOG_Print("\t[eMEVT_BUS_GO_OPERATION]\n\n");
		hErr = xmgr_swup_MsgGwmQuitStandby_Ses (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	default:
		hErr = ERR_OK;
		break;
	}

	if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

/* End of File. ---------------------------------------------------------- */

