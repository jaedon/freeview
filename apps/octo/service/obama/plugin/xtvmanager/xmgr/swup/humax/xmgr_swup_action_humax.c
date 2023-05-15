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

#include <linkedlist.h>

#include <bus.h>
#include <db_common.h>
#include <db_param.h>
#include <db_svc.h>
//#include <db_genre.h>

#include <svc_resources.h>
#include <svc_pipe.h>
#include <svc_si.h>
#include <xsvc_si.h>
#include <svc_sys.h>

#include <isosvc.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_tplist.h>
#include <mgr_live.h>
#include <mgr_spec.h>
#include <mgr_swup.h>
#include <mgr_search.h>
#include <mgr_rscmap.h>
#include <xmgr_swup.h>

#include "_xmgr_swup_humax.h"

#include "../local_include/_xmgr_swup.h"

#include <otl.h>

#if defined(CONFIG_OTA_HUMAX)

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
typedef struct tagHumaxOta_Contents_t
{
	Handle_t					 hSwUpdateDetect;
	Handle_t				 	 hSWUpdate;
	HUINT32						 ulActionId;			// Search Application Action ID
	Handle_t					 hAction;				// Search Action handle
	HUINT32						 ulTunerId;				// Tuner ID
	DxDeliveryType_e			 eDeliType;       		// Delivery Type
	MgrSwup_AppState_t			 eAppState;				// Current Search State :
	XsvcSi_SwupMode_e			 eSiSwupMode;			// Current SI Mode

	HBOOL						 bForcedStop;			// OTA Stopped forcely.

	// ==== Ota Check Spec ===
	SvcSi_SwupCheckSpec_t		 stOtaCheckSpec;		// OTA check spec

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

	// === Satellite Information
	HUINT32						 ulAntennaIndex;		// Antenna Index
} xmgrSwup_HumaxActionContents_t;


STATIC xmgrSwup_HumaxActionContents_t	s_stHumax_Contents;

/* internal function prototype */
STATIC xmgrSwup_HumaxActionContents_t* 	xmgr_swup_GetContents_Humax (void);
STATIC INLINE HERROR	xmgr_swup_CheckActionId_Humax (xmgrSwup_HumaxActionContents_t *pstContents, Handle_t hAction);
STATIC INLINE void		xmgr_swup_IncreaseSessionVersion_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC void				xmgr_swup_FreeLinkItem_Humax (void *pvItem);
STATIC HERROR 			xmgr_swup_InitContents_Humax (xmgrSwup_HumaxActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate);
STATIC HERROR			xmgr_swup_MakeHomeTpList_Humax (xmgrSwup_HumaxActionContents_t* pstContents);
STATIC HERROR			xmgr_swup_DestroyContents_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_TuneSearchTp_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_SetTuningParam_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_SetNextTpForOta_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_CountOtaTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents, HUINT32 *pulTpNum);
STATIC HERROR			xmgr_swup_GetOtaTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo);
STATIC void				xmgr_swup_SetSiOtaMode_Humax (xmgrSwup_HumaxActionContents_t *pstContents, XsvcSi_SwupMode_e eOtaMode);
STATIC DbSvc_TsInfo_t*	xmgr_swup_GetCurrentHomeTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC void				xmgr_swup_AddHomeTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo);
STATIC void				xmgr_swup_SetNextHomeTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllHomeTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllRefNitLink_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllSsuScanLink_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC void				xmgr_swup_ClearAllSsuLink_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC void 			xmgr_swup_ClearAllTsLink_Humax (xmgrSwup_HumaxActionContents_t * pstContents);

STATIC HERROR			xmgr_swup_StartAction_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_StopAction_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_StartSi_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC DbSvc_TsInfo_t*	xmgr_swup_AllocTsInfo_Humax (void);
STATIC void				xmgr_swup_FreeTsInfo_Humax (void *pvTsInfo);
STATIC void				xmgr_swup_ReleaseAction_Humax (xmgrSwup_HumaxActionContents_t *pstContents);

STATIC void				xmgr_swup_PrintCurrentTuningInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents);

STATIC HERROR 			xmgr_swup_MsgGwmCreate_Humax (xmgrSwup_HumaxActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate);
STATIC HERROR			xmgr_swup_MsgGwmDestroyed_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_MsgGwmTimer_Humax (xmgrSwup_HumaxActionContents_t *pstContents, HINT32 nTimerId);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccess_Humax (xmgrSwup_HumaxActionContents_t *pstContents, HINT32 bOtaService, HINT32 bOtaDescriptor);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccessInSvcTp_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiOtaSuccessInDescTp_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtSiSearchFail_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtChLocked_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_EvtChNoSignal_Humax (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR			xmgr_swup_MsgGwmQuitStandby_Humax  (xmgrSwup_HumaxActionContents_t *pstContents);
STATIC HERROR 			xmgr_swup_SwDetectFinish_Humax (xmgrSwup_HumaxActionContents_t *pstContents, HUINT8 ucDetectResult, HINT32 *pstOtaTsInfo, HINT32 *pstOtaInfo, MgrSwup_FailReason_e eFailReason);

#if	defined(CONFIG_MW_CH_SATELLITE)
#if	defined(CONFIG_OP_MIDDLE_EAST)
STATIC DbSvc_SatTuningInfo_t	 s_stHumaxOtaHomeSatTuneInfo[] =
{
	{	11766,	27500,	eDxSAT_POLAR_HOR,	eDxSAT_CODERATE_AUTO,	eDxSAT_TRANS_DVBS,	eDxSAT_PSK_QPSK,	eDxSAT_PILOT_OFF,	eDxSAT_ROLL_035,	eSatType_NILESAT, 0},
	{	0,			0,				0,					0,					0,			0,	0,	0,	0, 0},
};


#else
STATIC DbSvc_SatTuningInfo_t	 s_stHumaxOtaHomeSatTuneInfo[] =
{
	{	11766,	27500,	eDxSAT_POLAR_HOR,	eDxSAT_CODERATE_AUTO,	eDxSAT_TRANS_DVBS,	eDxSAT_PSK_QPSK,	eDxSAT_PILOT_OFF,	eDxSAT_ROLL_035,	eSatType_NILESAT, 0},
	{	0,			0,				0,					0,					0,			0,	0,	0,	0, 0},
};

#endif
#define	MAX_SWUP_TP_NUM		(sizeof(s_stHumaxOtaHomeSatTuneInfo) / sizeof(DbSvc_SatTuningInfo_t))

#endif

#if	defined(CONFIG_MW_CH_CABLE)
STATIC DbSvc_CabTuningInfo_t	 s_stHumaxOtaHomeCabTuneInfo[] =
{
	{	450000,	6900,	0xF001,	eDxCAB_CONSTELLATION_64QAM,	eDxCAB_SPECTRUM_AUTO},
	{	0,		0,			0,		0,							0,				  },
};
#define	MAX_SWUP_TP_NUM		(sizeof(s_stHumaxOtaHomeCabTuneInfo) / sizeof(DbSvc_CabTuningInfo_t))
#endif

#define	_________________________Body___________________________________

STATIC xmgrSwup_HumaxActionContents_t *xmgr_swup_GetContents_Humax (void)
{
	return &s_stHumax_Contents;
}

STATIC SvcSi_SwupCheckSpec_t* xmgr_swup_GetOtaSpec_Humax(void)
{
	STATIC SvcSi_SwupCheckSpec_t 	stOtaSpec;

	HxSTD_MemSet(&stOtaSpec, 0x00, sizeof(SvcSi_SwupCheckSpec_t));
	stOtaSpec.eActionKind = eSVCSI_ACT_KIND_SwupCheck;
	stOtaSpec.ulActionSpec = eSVCSI_ACT_SPEC_Humax;

	return &stOtaSpec;
}

STATIC INLINE HERROR xmgr_swup_CheckActionId_Humax (xmgrSwup_HumaxActionContents_t *pstContents, Handle_t hAction)
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

STATIC INLINE void xmgr_swup_IncreaseSessionVersion_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

STATIC void xmgr_swup_FreeLinkItem_Humax (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}


#define	_____CONTENTS_INITIATION_____

STATIC HERROR xmgr_swup_InitContents_Humax (xmgrSwup_HumaxActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate)
{
	DbSvc_TsInfo_t	*pstTsInfo;
	HERROR			 hErr;

	// 1. Clear all the contents.
	HxSTD_memset (pstContents, 0, sizeof(xmgrSwup_HumaxActionContents_t));

	// 2. Action Handle and Action ID
	pstContents->hSWUpdate	= hSWUpdate;
	pstContents->hAction	= hAction;
#if	defined(CONFIG_MW_CH_SATELLITE)
	pstContents->eDeliType	= eDxDELIVERY_TYPE_SAT;
#elif defined(CONFIG_MW_CH_CABLE)
	pstContents->eDeliType	= eDxDELIVERY_TYPE_CAB;
#endif
	pstContents->ulActionId	= SVC_PIPE_GetActionId (hAction);
	pstContents->ulTunerId	= (HUINT32)nTunerId;

	pstContents->bForcedStop = FALSE;

	HxSTD_memcpy(&pstContents->stOtaCheckSpec, xmgr_swup_GetOtaSpec_Humax(), sizeof(SvcSi_SwupCheckSpec_t));

	// 3. tuning information :
	// TP List :
	hErr = xmgr_swup_MakeHomeTpList_Humax (pstContents);
	if (hErr != ERR_OK)					{ return MESSAGE_BREAK; }

	pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Humax (pstContents);
	if (pstTsInfo != NULL)
	{
		pstContents->eDeliType = pstTsInfo->eDeliType;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_MakeHomeTpList_Humax (xmgrSwup_HumaxActionContents_t* pstContents)
{
	HUINT32					 ulTpCnt, ulTpNum;
	DbSvc_TsInfo_t			*pstTsInfo;
	HERROR					 hErr;

	if (pstContents == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	// Get the default TPs and the numbers.
	hErr = xmgr_swup_CountOtaTsInfo_Humax (pstContents, &ulTpNum);
	if (hErr != ERR_OK || ulTpNum == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
	{
		pstTsInfo = xmgr_swup_AllocTsInfo_Humax();
		if (pstTsInfo != NULL)
		{
			hErr = xmgr_swup_GetOtaTsInfo_Humax (pstContents, ulTpCnt, pstTsInfo);
			if (hErr == ERR_OK)
			{
				xmgr_swup_AddHomeTsInfo_Humax (pstContents, pstTsInfo);
			}
			else
			{
				xmgr_swup_FreeTsInfo_Humax (pstTsInfo);
			}
		}
	}

	if (xmgr_swup_GetCurrentHomeTsInfo_Humax (pstContents) == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define	_____CONTENTS_DESTRUCTION_____

STATIC HERROR xmgr_swup_DestroyContents_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	xmgr_swup_ClearAllHomeTsInfo_Humax (pstContents);
	xmgr_swup_ClearAllRefNitLink_Humax (pstContents);
	xmgr_swup_ClearAllSsuScanLink_Humax (pstContents);
	xmgr_swup_ClearAllSsuLink_Humax (pstContents);
	xmgr_swup_ClearAllTsLink_Humax(pstContents);

	return ERR_BUS_NO_ERROR;
}

#define	_____STATE_PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_swup_TuneSearchTp_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	HBOOL			 bRetry;
	SvcRsc_Info_t		 stRscInfo;
	HERROR			 hErr;

	bRetry = TRUE;

	while (bRetry == TRUE)
	{
		hErr = xmgr_swup_SetTuningParam_Humax (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("No Tp left to tune\n");

			hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
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


		xmgr_swup_IncreaseSessionVersion_Humax (pstContents);			// Tuning 전에 반드시 Action Version을 업데이트해야 한다.

		HxLOG_Print("Tune TP  : ");
		xmgr_swup_PrintCurrentTuningInfo_Humax (pstContents);
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
		hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
			bRetry = FALSE;
		}
	}

	return ERR_BUS_NO_OBJECT;
}

STATIC HERROR xmgr_swup_SetTuningParam_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	DbSvc_TsInfo_t		 stSvcTsInfo;
	DbSvc_TsInfo_t		*pstTsInfo;
	HERROR			hErr = ERR_FAIL;

	switch(pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_HUMAX_HOME_TP:
		pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Humax (pstContents);
		if (pstTsInfo == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		stSvcTsInfo = *pstTsInfo;
		break;

	case eSI_SWUP_MODE_HUMAX_SSUSVC_TP:
		xmgr_swup_SetNextHomeTsInfo_Humax (pstContents);
		pstTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Humax (pstContents);
		if (pstTsInfo == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		stSvcTsInfo = *pstTsInfo;

		break;

	default:
		return ERR_BUS_NO_OBJECT;
	}

	hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stSvcTsInfo, &(pstContents->stTuningInfo));

	return hErr;
}

STATIC HERROR xmgr_swup_SetNextTpForOta_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_HUMAX_HOME_TP:
		xmgr_swup_SetNextHomeTsInfo_Humax (pstContents);
		if (xmgr_swup_GetCurrentHomeTsInfo_Humax (pstContents) == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}
		break;

	case eSI_SWUP_MODE_HUMAX_SSUSVC_TP:
		xmgr_swup_SetNextHomeTsInfo_Humax (pstContents);
		if (xmgr_swup_GetCurrentHomeTsInfo_Humax (pstContents) == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}
		break;
	default:
		break;
	}

	return ERR_BUS_NO_ERROR;
}


#define	_____MESSAGE_FUNCTIONS_____


#define	_____CONTENTS_DATA_FUNCTIONS_____

STATIC HERROR xmgr_swup_CountOtaTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents, HUINT32 *pulTpNum)
{
	(void)pstContents;

	if (pulTpNum == NULL)					{ return ERR_BUS_TARGET_NULL; }

	*pulTpNum = MGR_SWUPINFO_CountOtaHomeTp();
	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_GetOtaTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR		 hErr;

	(void)pstContents;

	if (pstTsInfo == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = MGR_SWUPINFO_GetOtaHomeTp (ulTpNum, pstTsInfo);
	if (hErr != ERR_OK)						{ return ERR_BUS_OUT_OF_RANGE; }

	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
	return ERR_BUS_NO_ERROR;
}

STATIC void xmgr_swup_SetSiOtaMode_Humax (xmgrSwup_HumaxActionContents_t *pstContents, XsvcSi_SwupMode_e eOtaMode)
{
	HxLOG_Print("[humax_ota] SI Mode %d -> %d\n", pstContents->eSiSwupMode, eOtaMode);
	pstContents->eSiSwupMode = eOtaMode;
}


// Home Ts Info
STATIC DbSvc_TsInfo_t *xmgr_swup_GetCurrentHomeTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
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

STATIC void xmgr_swup_AddHomeTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo)
{
	if (pstContents != NULL && pstHomeTsInfo != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstHomeTpList, (void *)pstHomeTsInfo);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstHomeTpList);
	}
}

STATIC void xmgr_swup_SetNextHomeTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstHomeTpList != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstHomeTpList, pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Humax);
	}
}

STATIC void xmgr_swup_ClearAllHomeTsInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Humax);
	}
}


STATIC void xmgr_swup_ClearAllRefNitLink_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstRefNitList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstRefNitList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Humax);
	}
}


STATIC void xmgr_swup_ClearAllSsuScanLink_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuScanList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Humax);
	}
}

STATIC void xmgr_swup_ClearAllSsuLink_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuLinkList, (FREE_FUNC)xmgr_swup_FreeLinkItem_Humax);
	}
}

STATIC void xmgr_swup_ClearAllTsLink_Humax (xmgrSwup_HumaxActionContents_t * pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTsInfoListFromSi, (FREE_FUNC)xmgr_swup_FreeLinkItem_Humax);
	}
}

#define	_____PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_swup_StartAction_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.
	HERROR			 hErr;

	xmgr_swup_SetSiOtaMode_Humax (pstContents, eSI_SWUP_MODE_HUMAX_HOME_TP);

	// 1. Set the TP to tuning :
	hErr = xmgr_swup_SetTuningParam_Humax (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	// 2. Get the Action : Humax OTA는 Search와 다를 바가 없으므로 Action은 Search 로써 잡아도 된다.
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
	hErr = xmgr_swup_TuneSearchTp_Humax (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_StopAction_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	if (pstContents->eAppState == eOtaApp_state_waitingSi)
	{
		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
	}

	if (pstContents->eAppState != eOtaApp_state_init)
	{
		xmgr_swup_ReleaseAction_Humax (pstContents);
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_StartSi_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	HUINT16						usExtId1, usExtId2;
	SvcSi_SwupCheckOption_t			stOtaCheckOption;

	usExtId1 = 0;
	usExtId2 = 0;

	HxSTD_memset(&stOtaCheckOption, 0x00, sizeof(SvcSi_SwupCheckOption_t));

	stOtaCheckOption.ulSwupMode = eSI_SWUP_MODE_HUMAX_HOME_TP + abs(pstContents->eSiSwupMode - eSI_SWUP_MODE_HUMAX_HOME_TP);	// new si 적용이 완료되면 new type으로 모두 변경하자 !
	stOtaCheckOption.usExtId1 = usExtId1;
	stOtaCheckOption.usExtId2 = usExtId2;

	HxLOG_Print("[%s, %d] eOtaMode(0x%x) ExtId1(0x%x) ExtId2(0x%x)\n,",
		__FUNCTION__, __LINE__, stOtaCheckOption.ulSwupMode, stOtaCheckOption.usExtId1, stOtaCheckOption.usExtId2 );

	return SVC_SI_StartSwupCheck(pstContents->hAction, &pstContents->stOtaCheckSpec, &stOtaCheckOption, FALSE);
}

STATIC DbSvc_TsInfo_t *xmgr_swup_AllocTsInfo_Humax (void)
{
	return (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
}

STATIC void xmgr_swup_FreeTsInfo_Humax (void *pvTsInfo)
{
	if (pvTsInfo != NULL)
	{
		OxAP_Free (pvTsInfo);
	}
}

STATIC void xmgr_swup_ReleaseAction_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
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

STATIC void xmgr_swup_PrintCurrentTuningInfo_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
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
STATIC HERROR xmgr_swup_MsgGwmCreate_Humax (xmgrSwup_HumaxActionContents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSWUpdate)
{
	HERROR		 hErr;

	// Create시 해 주어야 하는 Initialize 등이 실패할 경우에는 이 Action 자체가 살아 있어서는 안된다.
	// 곧바로 Destroy 해 주도록 한다.

	// 1. Initialize the contents
	hErr = xmgr_swup_InitContents_Humax (pstContents, hAction, nTunerId, hSWUpdate);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		xmgr_swup_SwDetectFinish_Humax(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
		return hErr;
	}

	// 2. Start the OTA Action :
	hErr = xmgr_swup_StartAction_Humax (pstContents);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		xmgr_swup_SwDetectFinish_Humax(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Action);
		return hErr;
	}

	MGR_SWUPINFO_ResetSearchedInfo();

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY:
STATIC HERROR xmgr_swup_MsgGwmDestroyed_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	// Stop all the Action
	xmgr_swup_StopAction_Humax (pstContents);

	// Destroy the contents
	xmgr_swup_DestroyContents_Humax (pstContents);

	// Inform that the OTA action is destroyed
	if (pstContents->bForcedStop != TRUE)
	{
//		BUS_PostMessage (NULL, MSG_APP_OTA_ACTION_DESTROYED, (Handle_t)NULL, (HINT32)BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()), 0, 0);
	}

	return MESSAGE_BREAK;
}

// eMEVT_BUS_TIMER:
STATIC HERROR xmgr_swup_MsgGwmTimer_Humax (xmgrSwup_HumaxActionContents_t *pstContents, HINT32 nTimerId)
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
			hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Humax (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_MsgGwmTimer_Humax] No Next Tp left to tune\n");
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
				hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_swup_TuneSearchTp_Humax (pstContents);
				}
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_MsgGwmTimer_Humax] Cannot proceed anymore\n");
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
		xmgr_swup_SwDetectFinish_Humax(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}

	return hRetValue;
}

// EVT_SI_OTA_SUCCESS:
STATIC HERROR xmgr_swup_EvtSiOtaSuccess_Humax (xmgrSwup_HumaxActionContents_t *pstContents, HINT32 bOtaService, HINT32 bOtaDescriptor)
{
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Print("[%s] appState(0x%x) otaMode (0x%x) bOtaService(%d) , bOtaDescriptor(%d) \n", __FUNCTION__, pstContents->eAppState, pstContents->eSiSwupMode, bOtaService, bOtaDescriptor);

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_waitingSi:
		// SI succeeded : Kill the timer

		BUS_KillTimer (OTA_TIMER_SI_ID);

		pstContents->eAppState = eOtaApp_state_siReceived;

		switch (pstContents->eSiSwupMode)
		{
		case eSI_SWUP_MODE_HUMAX_HOME_TP:
			if(bOtaService == TRUE)
			{
				hErr = xmgr_swup_EvtSiOtaSuccessInSvcTp_Humax(pstContents);
			}
			else if(bOtaDescriptor == TRUE)
			{
				xmgr_swup_SetSiOtaMode_Humax (pstContents, eSI_SWUP_MODE_HUMAX_SSUSVC_TP);

				hErr = xmgr_swup_EvtSiOtaSuccessInDescTp_Humax(pstContents);
			}
			else
			{
				// 모든 TP 다 체크함.
				hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
				if (hErr != ERR_OK)
				{
					pstContents->eAppState = eOtaApp_state_checkend;
				}
			}
			break;

		default:
			break;
		}

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		if (pstContents->eAppState != eOtaApp_state_checkend)
		{
			hErr = xmgr_swup_TuneSearchTp_Humax (pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("[%s] No Next Tp left to tune\n",  __FUNCTION__);
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
		if((pstContents->eSiSwupMode != eSI_SWUP_MODE_HUMAX_SSUSVC_TP)
			|| ((pstContents->eSiSwupMode == eSI_SWUP_MODE_HUMAX_SSUSVC_TP) && (hErr != ERR_OK)))
		{
			// check end 시 fail notify 가 필요하나 ? 현재는 successed 시에도 올라가므로 일단 막자.
			// fail notify 는 다른 signaling 부분에서 처리하지만.. 검토는 필요.
			//xmgr_swup_SwDetectFinish_Humax(pstContents, DETECT_FAIL, 0, 0, eSWUP_FAILREASON_Unknown);
		}
	}

	return MESSAGE_BREAK;
}


STATIC HERROR xmgr_swup_EvtSiOtaSuccessInSvcTp_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	HUINT32				ulDataNum;
	SvcSi_SwupSignal_t	*pstOtaArray;
	SvcSi_SwupSignal_t	stOtaInfo, *pstOtaSignalInfo = NULL;
	DxTuneParam_t		stChTuneInfo;
	DbSvc_TsInfo_t		stOtaTsInfo;
	DbSvc_TsInfo_t		*pstHomeTsInfo;
	HERROR				hErr, hResult = ERR_BUS_NO_ERROR;
	SvcSwUpdate_DetectResult_t	*pstDetectResult = NULL;

	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_HUMAX_SVC, &ulDataNum, (void **)&pstOtaArray);
	if (hErr == ERR_OK && pstOtaArray != NULL)
	{
		pstOtaSignalInfo= (SvcSi_SwupSignal_t *)&(pstOtaArray[0]);

		HxSTD_MemSet(&stOtaTsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

		hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
		if (hErr != ERR_OK)
		{
			stChTuneInfo = pstContents->stTuningInfo;
		}

		OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&stChTuneInfo, &(stOtaTsInfo.stTuningParam));

		pstHomeTsInfo = xmgr_swup_GetCurrentHomeTsInfo_Humax (pstContents);
		if (pstHomeTsInfo != NULL)
		{
#if defined(CONFIG_MW_CH_SATELLITE)
			stOtaTsInfo.stTuningParam.sat.ucAntId 	= pstHomeTsInfo->stTuningParam.sat.ucAntId;
			stOtaTsInfo.eDeliType 				= eDxDELIVERY_TYPE_SAT;
			stOtaTsInfo.ucTunerId 				= pstHomeTsInfo->ucTunerId;
			stOtaTsInfo.usOnId						= pstHomeTsInfo->usOnId;
			stOtaTsInfo.usTsId						= pstHomeTsInfo->usTsId;
#else
			stOtaTsInfo.eDeliType 				= pstHomeTsInfo->eDeliType;
			stOtaTsInfo.ucTunerId 				= pstHomeTsInfo->ucTunerId;
			stOtaTsInfo.usTsId					= pstHomeTsInfo->usTsId;
			stOtaTsInfo.usOnId					= pstHomeTsInfo->usOnId;
#endif
		}

		HxLOG_Print("[Humaxota] DeliType : %04x \n", stOtaTsInfo.eDeliType);
		HxLOG_Print("[Humaxota] tsId (0x%x) onId(0x%x) \n", stOtaTsInfo.usTsId, stOtaTsInfo.usOnId);
#if defined(CONFIG_MW_CH_SATELLITE)
		HxLOG_Print("[Humaxota] FREQ (%d) SR(%d) FEC (%d) \n", stOtaTsInfo.stTuningParam.sat.ulFrequency, stOtaTsInfo.stTuningParam.sat.ulSymbolRate,  stOtaTsInfo.stTuningParam.sat.eFecCodeRate);
#elif defined(CONFIG_MW_CH_CABLE)
		HxLOG_Print("[Humaxota] FREQ (%d) SR(%d) Con (%d) \n", stOtaTsInfo.stTuningParam.cab.ulFreq, stOtaTsInfo.stTuningParam.cab.usSR,  stOtaTsInfo.stTuningParam.cab.ucCon);
#endif
		// 찾은 OTA 정보를 기록한다.
		HxLOG_Print("[Humaxota] Found OTA Info \n");
		HxLOG_Print("[Humaxota] System ID : %08x \n", pstOtaSignalInfo->ulStartSystemId);
		HxLOG_Print("[Humaxota] Found OTA DataVersion : 0x%X\n", pstOtaSignalInfo->ulDataVersion);
		HxLOG_Print("[Humaxota] Found OTA PID : 0x%X\n", pstOtaSignalInfo->usPid);

		HxSTD_MemSet (&stOtaInfo, 0, sizeof(SvcSi_SwupSignal_t));
		pstDetectResult = (SvcSwUpdate_DetectResult_t *)HLIB_STD_MemCalloc(sizeof(SvcSwUpdate_DetectResult_t));
		if (pstDetectResult == NULL)
		{
			HxLOG_Error("Memory alloc fail!!\n");
			hResult = ERR_FAIL;
			goto END_FUNC;
		}

		pstDetectResult->bAvailable = TRUE;
		pstDetectResult->bFound 	= TRUE;

		// emergency set
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stChTuningInfo, &stChTuneInfo, sizeof(DxTuneParam_t));
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stSvcTsInfo, &stOtaTsInfo, sizeof(DbSvc_TsInfo_t));
		pstDetectResult->data.stRfInfo.eDeliType		= eDxDELIVERY_TYPE_SAT;
		pstDetectResult->data.stRfInfo.usPid			= pstOtaSignalInfo->usPid;
		pstDetectResult->data.stRfInfo.eDownloadType = eSWUP_DOWNLOAD_DVBSSU;

		pstDetectResult->ulDataVersion 	= pstOtaSignalInfo->ulDataVersion;
		pstDetectResult->usOtaPid	   	= pstOtaSignalInfo->usPid;
		pstDetectResult->eSource		= eDxSWUPDATE_SOURCE_Rf;
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

		MGR_SWUPINFO_SetSearchedInfo (&stOtaTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);

		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_HUMAX_SVC, (void *)pstOtaArray);

		pstContents->eAppState = eOtaApp_state_checkend;

		// detect 보내기..
		xmgr_swup_SwDetectFinish_Humax(pstContents, DETECT_SUCCESS, (HINT32 *)&stOtaTsInfo,(HINT32 *)&stOtaInfo, eSWUP_FAILREASON_Unknown);
	}
	else
	{
		hResult = ERR_FAIL;
		goto END_FUNC;
	}

END_FUNC:
	if(pstDetectResult != NULL)
	{
		HLIB_STD_MemFree(pstDetectResult);
		pstDetectResult = NULL;
	}

	if(hResult == ERR_FAIL)
	{
		xmgr_swup_SwDetectFinish_Humax(pstContents, DETECT_FAIL, 0, 0, eSWUP_FAILREASON_Unknown);

		hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[%s] all tp checked & set eapp state to eOtaApp_state_checkend\n", __FUNCTION__);
			pstContents->eAppState = eOtaApp_state_checkend;
		}
	}

	return hErr;
}

STATIC HERROR xmgr_swup_EvtSiOtaSuccessInDescTp_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	HERROR				 hErr = ERR_OK;
	HINT32				 nTsCnt;
	HUINT32				 ulDataNum;
	XsvcSi_SwupSignal_Humax_t		 *pstOtaArray, *pstOtaInfo;
	SvcSi_SwupSignal_t			 stOtaInfo;
	DxTuneParam_t		 stChTuneInfo;
	DbSvc_TsCondition_t	 stTsCond;
	DbSvc_TsInfo_t		 *pstTsInfoList = NULL;
	DbSvc_TsInfo_t		 stOtaTsInfo;
	HUINT32				 ulPrevCanceledVer;
	HUINT16				 usPrevCanceledType;

	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_HUMAX_SVC, &ulDataNum, (void **)&pstOtaArray);
	if (hErr == ERR_OK && pstOtaArray != NULL)
	{
		pstOtaInfo = &(pstOtaArray[0]);

		// 찾은 OTA 정보를 기록한다.
		HxLOG_Print("[Humaxota] Found OTA Info \n");
		HxLOG_Print("[Humaxota] System ID : %08x  , System Ver : 0x%0x\n", pstOtaInfo->ulHardwareVersion, pstOtaInfo->ulSoftwareVersion);
		HxLOG_Print("[Humaxota] ForceOta : %d  \n", pstOtaInfo->ucForceFlag);

		HxSTD_memset (&stOtaTsInfo, 0, sizeof(DbSvc_TsInfo_t));
		HxSTD_memset (&stOtaInfo, 0, sizeof(SvcSi_SwupSignal_t));

		DB_SVC_InitTsFindCondition(&stTsCond);


#if	defined(CONFIG_MW_CH_SATELLITE)
		stTsCond.nDeliType = eDxDELIVERY_TYPE_SAT;
#elif	defined(CONFIG_MW_CH_CABLE)
		stTsCond.nDeliType = eDxDELIVERY_TYPE_CAB;
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
		stTsCond.nDeliType = eDxDELIVERY_TYPE_TER;
#endif
		stTsCond.nOnId = pstOtaInfo->usOnId;
		stTsCond.nTsId = pstOtaInfo->usTsId;

		hErr = DB_SVC_FindTsInfoList (&stTsCond, &nTsCnt, &pstTsInfoList);
		if (hErr == ERR_OK && nTsCnt > 0 && pstTsInfoList != NULL)
		{
			stOtaTsInfo.stTuningParam = pstTsInfoList[0].stTuningParam;
		}
		else
		{
			//DB에서 못 찾을 경우 어떻게 할것인가..

#if	defined(CONFIG_MW_CH_SATELLITE)
		HxSTD_memcpy(&stOtaTsInfo.stTuningParam.sat, &s_stHumaxOtaHomeSatTuneInfo, sizeof(DbSvc_SatTuningInfo_t)) ;
#elif	defined(CONFIG_MW_CH_CABLE)
		HxSTD_memcpy(&stOtaTsInfo.stTuningParam.cab, &s_stHumaxOtaHomeCabTuneInfo, sizeof(DbSvc_CabTuningInfo_t)) ;
#endif
		}

		stOtaTsInfo.eDeliType = stTsCond.nDeliType;
		stOtaTsInfo.usOnId = stTsCond.nOnId;
		stOtaTsInfo.usTsId = stTsCond.nTsId;

//		MWC_UTIL_ConvCh2DbTuningParam (&stChTuneInfo, &stOtaTsInfo.stTuningParam);
		OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&stChTuneInfo, &stOtaTsInfo.stTuningParam);

		// 이전의 Cancel OTA Info와 달라졌다면 Cancel Count를 지워야 한다.
		hErr = MGR_SWUPINFO_GetOtaCanceledVersion (&usPrevCanceledType, &ulPrevCanceledVer);
		if (hErr == ERR_OK)
		{
			if(ulPrevCanceledVer != pstOtaInfo->ulSoftwareVersion)
			{
				MGR_SWUPINFO_SetOtaCanceledVersion (0, 0);
			}
		}


		HxLOG_Print("[Humaxota] tsId (0x%x) onId(0x%x) \n", stOtaTsInfo.usTsId, stOtaTsInfo.usOnId);
		HxLOG_Print("[Humaxota] freq (%d) SR(%d) Con (%d) \n", stOtaTsInfo.stTuningParam.cab.ulFreq, stOtaTsInfo.stTuningParam.cab.usSR,  stOtaTsInfo.stTuningParam.cab.ucCon);
		HxLOG_Print("[Humaxota] DeliType : %04x \n", stOtaTsInfo.eDeliType);

		stOtaInfo.ulStartSystemId = pstOtaInfo->ulHardwareVersion;
		stOtaInfo.ulEndSystemId = pstOtaInfo->ulHardwareVersion;
		stOtaInfo.eDataType = eVERSION_TYPE_APPL;
		stOtaInfo.ucSwupType = pstOtaInfo->ucForceFlag;
		stOtaInfo.ulDataVersion = pstOtaInfo->ulSoftwareVersion;
		stOtaInfo.usSvcId = pstOtaInfo->usSvcId;
		stOtaInfo.usPid = pstOtaInfo->usPid;

		MGR_SWUPINFO_SetSearchedInfo (&stOtaTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)&stOtaInfo);

//		MW_SI_FreeOtaDetectData (eSI_SWUP_DATA_HUMAX_SVC, (void *)pstOtaArray);
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_HUMAX_SVC, (void *)pstOtaArray);

		pstContents->eAppState = eOtaApp_state_checkend;
	}
	else
	{
		hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
		if (hErr != ERR_OK)
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			goto END_FUNC;

		}
	}

END_FUNC:
	if(pstTsInfoList)
	{
		OxDB_Free(pstTsInfoList);
		pstTsInfoList = NULL;
	}
	return hErr;
}

// eSEVT_SI_SEARCH_FAIL:
STATIC HERROR xmgr_swup_EvtSiSearchFail_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	HERROR				 hErr;

	HxLOG_Print("[xmgr_swup_EvtSiSearchFail_Humax] AppState (0x%x) \n", pstContents->eAppState);

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_waitingSi:
		// SI failed : Kill the timer

		BUS_KillTimer (OTA_TIMER_SI_ID);

		pstContents->eAppState = eOtaApp_state_siFailed;

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_swup_TuneSearchTp_Humax (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_swup_EvtSiSearchFail_Humax] No Next Tp left to tune\n");
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
		xmgr_swup_SwDetectFinish_Humax(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}

	return MESSAGE_BREAK;
}

// eSEVT_CH_LOCKED
STATIC HERROR xmgr_swup_EvtChLocked_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_tryTuning :
		// Channel Locked : Kill the timer
		BUS_KillTimer (OTA_TIMER_LOCKING_ID);

		// Start the SI for the Humax OTA
		hErr = xmgr_swup_StartSi_Humax (pstContents);
		if (hErr == ERR_OK)
		{
			pstContents->eAppState = eOtaApp_state_waitingSi;
			// MW SI를 무한으로 기다리는 것을 막기 위해 여기에서 Timer를 걸어야 한다.
			BUS_SetTimer (OTA_TIMER_SI_ID, OTA_TIMER_SI_TIME);
		}
		else
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Humax (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_EvtChLocked_Humax] No Next Tp left to tune\n");
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
		xmgr_swup_SwDetectFinish_Humax(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_EvtChNoSignal_Humax (xmgrSwup_HumaxActionContents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_tryTuning :
		// Channel Locked : Kill the timer

		BUS_KillTimer (OTA_TIMER_LOCKING_ID);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_swup_TuneSearchTp_Humax (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_swup_EvtChNoSignal_Humax] No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
		}

		break;

	case eOtaApp_state_waitingSi:
		// Stop SI :
		hErr = SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
		if (hErr == ERR_OK)
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_SetNextTpForOta_Humax (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_TuneSearchTp_Humax (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_EvtChNoSignal_Humax] No Next Tp left to tune\n");
				pstContents->eAppState = eOtaApp_state_checkend;
			}
		}
		else
		{
			// 더 이상 진행 불가능 : Search End로 간주한다.
			HxLOG_Print("[xmgr_swup_EvtChNoSignal_Humax] SI Re-open failed.\n");
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
		xmgr_swup_SwDetectFinish_Humax(pstContents, DETECT_FAIL, NULL, NULL, eSWUP_FAILREASON_Unknown);
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_MsgGwmQuitStandby_Humax  (xmgrSwup_HumaxActionContents_t *pstContents)
{
	pstContents->bForcedStop = TRUE;
	BUS_MGR_Destroy (NULL);

	return ERR_OK;
}

#if defined(CONFIG_MW_CH_SATELLITE)
#if defined(CONFIG_OP_MIDDLE_EAST)
STATIC HERROR xmgr_swup_CheckDefaultAntInfo_MeSat (HINT32 *pnDefAntIdx)
{
	DbSvc_AntInfo_t			stAntInfo;
	HINT32				nAntCnt = 0, nAntNum = 0;
	HINT32				*pnAntIdxArray = NULL;
	HBOOL				bFound = FALSE;
	HERROR			 	hError;

	*pnDefAntIdx = -1;

	hError = DB_SVC_FindAllAntIdxList (&nAntNum, &pnAntIdxArray);
	if (hError == ERR_OK && nAntNum > 0 && pnAntIdxArray != NULL)
	{
		for (nAntCnt = 0; nAntCnt < nAntNum; nAntCnt ++)
		{
			hError = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxArray[nAntCnt], &stAntInfo);
			if (hError == ERR_OK && stAntInfo.eSatType == eSatType_NILESAT_101_102)
			{
				*pnDefAntIdx = pnAntIdxArray[nAntCnt];

				bFound = TRUE;
				break;
			}
		}
	}

	if (pnAntIdxArray != NULL)
	{
		DB_SVC_FreeAntList (pnAntIdxArray);
		pnAntIdxArray= NULL;
	}

	if (bFound)
	{
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("[%s] Antenna(NileSat) not found... Error~ \n", __FUNCTION__);
		return ERR_FAIL;
	}
}
#endif
#endif

STATIC HERROR xmgr_swup_SwDetectFinish_Humax (xmgrSwup_HumaxActionContents_t *pstContents, HUINT8 ucDetectResult, HINT32 *pstOtaTsInfo, HINT32 *pstOtaInfo, MgrSwup_FailReason_e eFailReason)
{
	if(ucDetectResult == DETECT_SUCCESS)
	{
		HxLOG_Print("[%s][%04d] MGR_SWUP_PostMessage &  BUS_PostMessage : eMEVT_SWUP_NOTIFY_DETECT_SUCCESS ( pstContents->hSWUpdate: 0x%X)\n", __FUNCTION__, __LINE__, pstContents->hSWUpdate);

		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_SUCCESS, eSWUP_STATE_Connect, (HINT32)pstOtaTsInfo, (HINT32)pstOtaInfo, 0);
#if defined(CONFIG_OP_MIDDLE_EAST)
		{
			HCHAR				szCurrentVersion[32], szNewSWVersion[32];
			HUINT32 				ulSystemVersion;
			SvcSi_SwupSignal_t			stOtaInfo;
			HERROR				hErr;
			HCHAR				*pszVersion = NULL;

			HxSTD_memset(&stOtaInfo, 0x00, sizeof(SvcSi_SwupSignal_t));

			SVC_SYS_GetSystemVersion(eVERSION_TYPE_APPL, &ulSystemVersion);
			hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_OTA_OTAINFO, (HUINT32 *)&stOtaInfo, sizeof(SvcSi_SwupSignal_t));

			if(hErr == ERR_OK)
			{
				SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, ulSystemVersion, szCurrentVersion);
				SVC_SYS_MakeSystemVerString(eVERSION_TYPE_APPL, stOtaInfo.ulDataVersion, szNewSWVersion);

#if defined(CONFIG_PROD_IR4000HD)
				if (MGR_ACTION_GetState()== eAmState_ACTIVE_STANDBY)
				{
					// standby 에서의 detected 는 처리하지 않음.
					// 이것은 download 완료 시점에 한다.
					//MWC_PARAM_SetSwUpdateVersion(TRUE, TRUE, szCurrentVersion, szNewSWVersion);
				}
				else
				{
					pszVersion = (HCHAR *)xmgr_swup_GetSwUpdateVersionString_Humax(TRUE, FALSE, szCurrentVersion, szNewSWVersion);
					DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, TRUE);
				}
#else
				pszVersion = xmgr_swup_GetSwUpdateVersionString_Humax(TRUE, FALSE, szCurrentVersion, szNewSWVersion);
				DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, TRUE);
#endif
				HxLOG_Debug("\neMEVT_SWUP_NOTIFY_DETECT_SUCCESS!\n");
			}
			else
			{
				pszVersion = xmgr_swup_GetSwUpdateVersionString_Humax(FALSE, FALSE, (HCHAR *)NULL, (HCHAR *)NULL);
				DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, 0);
				HxLOG_Debug("\neMEVT_SWUP_NOTIFY_DETECT_FAIL!\n");
			}
		}
#endif
	}
	else
	{
#if defined(CONFIG_OP_MIDDLE_EAST)
		HCHAR	*pszVersion = NULL;
#endif

		MGR_SWUP_PostMessage(pstContents->hSWUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, (Handle_t)0, 0, 0, eFailReason);
#if defined(CONFIG_OP_MIDDLE_EAST)
		pszVersion = xmgr_swup_GetSwUpdateVersionString_Humax(FALSE, FALSE, (HCHAR *)NULL, (HCHAR *)NULL);
		DB_PARAM_SetItem(eDB_PARAM_ITEM_SWUPDATE_VERSION, (HUINT32)pszVersion, 0);
		HxLOG_Print("[%s][%04d] eMEVT_SWUP_NOTIFY_DETECT_FAIL!\n", __FUNCTION__,__LINE__);
#endif
	}

	return ERR_OK;
}


#define	_____EXTERNAL_PROCESS_FUNCTION_____

BUS_Result_t xproc_swup_Detect_Humax(HINT32 message, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	STATIC HBOOL			s_bFirstStart = TRUE;
	xmgrSwup_HumaxActionContents_t		*pstContents;
	HERROR					hErr;

	pstContents = xmgr_swup_GetContents_Humax();
	if (s_bFirstStart == TRUE)
	{
		s_bFirstStart = FALSE;
		HxSTD_memset (pstContents, 0, sizeof(xmgrSwup_HumaxActionContents_t));
	}

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Print("\t[xproc_swup_Detect_Humax] ----- [eMEVT_BUS_CREATE]\n\n");
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_swup_Humax");
		hErr = xmgr_swup_MsgGwmCreate_Humax (pstContents, hAction, nParam1, nParam2);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Print("\t[xproc_swup_Detect_Humax] ----- [eMEVT_BUS_TIMER]\n\n");
		hErr = xmgr_swup_MsgGwmTimer_Humax (pstContents, nParam1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Print("\t[xproc_swup_Detect_Humax] ----- [eMEVT_BUS_DESTROY]\n\n");
		hErr = xmgr_swup_MsgGwmDestroyed_Humax (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_SUCCESS:
		hErr = xmgr_swup_CheckActionId_Humax (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[xproc_swup_Detect_Humax] ----- [eSEVT_SWUP_DETECT_SUCCESS]\n\n");

		hErr = xmgr_swup_EvtSiOtaSuccess_Humax (pstContents, nParam1, nParam2);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_FAIL:
		hErr = xmgr_swup_CheckActionId_Humax (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[xproc_swup_Detect_Humax] ----- [eSEVT_SWUP_DETECT_FAIL]\n\n");

		hErr = xmgr_swup_EvtSiSearchFail_Humax (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_LOCKED:
		hErr = xmgr_swup_CheckActionId_Humax (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[xproc_swup_Detect_Humax] ----- [eSEVT_CH_LOCKED]\n\n");

		hErr = xmgr_swup_EvtChLocked_Humax (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_NO_SIGNAL:
		hErr = xmgr_swup_CheckActionId_Humax (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[xproc_swup_Detect_Humax] ----- [eSEVT_CH_NO_SIGNAL]\n\n");

		hErr = xmgr_swup_EvtChNoSignal_Humax (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_GO_OPERATION :
		HxLOG_Print("\t[xproc_swup_Detect_Humax] ----- [eMEVT_BUS_GO_OPERATION]\n\n");
		hErr = xmgr_swup_MsgGwmQuitStandby_Humax (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	default:
		break;
	}

	return BUS_ProcessMessageDefault(message, hAction, nParam1, nParam2, nParam3);
}


#define _____SWUPDATE_DEFAULT_HOME_TP_____

HERROR xmgr_swup_SetDefaultHomeTsInfo_Humax (void)
{
	HUINT32				ulCount;
	DbSvc_TsInfo_t			stTsInfo;
#if	defined(CONFIG_MW_CH_CABLE)
	DbSvc_CabTuningInfo_t 	*pstCabTuneInfo;
#endif
#if	defined(CONFIG_MW_CH_SATELLITE)
	DbSvc_SatTuningInfo_t 	*pstSatTuneInfo = NULL;
	HINT32				nDefAntIdx = 0 ;
	HERROR				hErr = ERR_FAIL;
#endif

	MGR_SWUPINFO_ResetHomeTp ();

#if	defined(CONFIG_MW_CH_CABLE)
	for (ulCount = 0; ulCount < MAX_SWUP_TP_NUM; ulCount++)
	{
		pstCabTuneInfo = &(s_stHumaxOtaHomeCabTuneInfo[ulCount]);
		if (pstCabTuneInfo->ulFreq== 0)			{ break; }

		stTsInfo.eDeliType = eDxDELIVERY_TYPE_CAB;
		stTsInfo.stTuningParam.cab	= *pstCabTuneInfo;

		MGR_SWUPINFO_AddOtaHomeTp (&stTsInfo);
	}
#endif

#if defined(CONFIG_MW_CH_SATELLITE)

#if	defined(CONFIG_OP_MIDDLE_EAST)
	hErr = xmgr_swup_CheckDefaultAntInfo_MeSat(&nDefAntIdx);
	if(hErr != ERR_OK || nDefAntIdx == 0)
	{
		HxLOG_Error("hErr(%d) nDefAntIdx (%d) \n\n");
		return ERR_FAIL;
	}
#endif

	for (ulCount = 0; ulCount < MAX_SWUP_TP_NUM; ulCount++)
	{
		pstSatTuneInfo = &(s_stHumaxOtaHomeSatTuneInfo[ulCount]);
		if (pstSatTuneInfo == NULL || pstSatTuneInfo->ulFrequency== 0)		{ break; }

		HxSTD_memset (&stTsInfo, 0, sizeof(DbSvc_TsInfo_t));
		pstSatTuneInfo->ucAntId = (HUINT8)nDefAntIdx;
		stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
		stTsInfo.stTuningParam.sat = *pstSatTuneInfo;

		MGR_SWUPINFO_AddOtaHomeTp (&stTsInfo);
	}
#endif

	return ERR_OK;
}
#endif

/* End of File. ---------------------------------------------------------- */

