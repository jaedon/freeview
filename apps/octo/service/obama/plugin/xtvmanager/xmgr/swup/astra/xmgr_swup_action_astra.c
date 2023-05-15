/**
	@file     ap_sat_search_svc.c
		---> xmgr_sw_update_action_astra.c
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

#include <mgr_common.h>
#include <mgr_swup.h>
#include <mgr_search.h>
#include <mgr_rscmap.h>
#include <mgr_spec.h>

#include <xmgr_swup.h>
#include "../local_include/_xmgr_swup.h"

#include <otl.h>

#if defined(CONFIG_OTA_ASTRA)

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
typedef struct tagAstraOta_Contents_t
{
	Handle_t					 hSwUpdate;
	HUINT32						 ulActionId;			// Search Application Action ID
	Handle_t					 hAction;				// Search Action handle
	HUINT32						 ulTunerId;				// Tuner ID
	DxDeliveryType_e			 eDeliType;       		// Delivery Type
	MgrSwup_AppState_t			 eAppState;				// Current Search State :
	XsvcSi_SwupMode_e			 eSiSwupMode;			// Current SI Mode

	HBOOL						 bForcedStop;			// OTA Stopped forcely.

	SvcSi_SwupCheckSpec_t		 stOtaCheckSpec;		// OTA Check Spec

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
} AstraOta_Contents_t;


STATIC AstraOta_Contents_t	s_stAstraOta_Contents;

/* internal function prototype */

STATIC AstraOta_Contents_t* xmgr_swup_astra_GetContents (void);

STATIC INLINE HERROR	xmgr_swup_astra_CheckActionId (AstraOta_Contents_t *pstContents, Handle_t hAction);
STATIC INLINE void		xmgr_swup_astra_IncreaseSessionVersion (AstraOta_Contents_t *pstContents);
STATIC void				xmgr_swup_astra_FreeLinkItem (void *pvItem);

STATIC HERROR			xmgr_swup_astra_InitContents (AstraOta_Contents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSwUpdate);
STATIC HERROR			xmgr_swup_astra_MakeHomeTpList (AstraOta_Contents_t* pstContents);

STATIC HERROR			xmgr_swup_astra_DestroyContents (AstraOta_Contents_t *pstContents);

STATIC HERROR			xmgr_swup_astra_FindTsInfoByTripleId (AstraOta_Contents_t *pstContents, DbSvc_TripleId_t *pstTripleId, DbSvc_TsInfo_t *pstSvcTsInfo);

STATIC HERROR			xmgr_swup_astra_TuneSearchTp (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_SetTuningParam (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_SetNextTpForOta (AstraOta_Contents_t *pstContents);

STATIC HERROR			xmgr_swup_astra_CountOtaTsInfo (AstraOta_Contents_t *pstContents, HUINT32 *pulTpNum);
STATIC HERROR			xmgr_swup_astra_GetOtaTsInfo (AstraOta_Contents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo);
STATIC void				xmgr_swup_astra_SetSiOtaMode (AstraOta_Contents_t *pstContents, XsvcSi_SwupMode_e eOtaMode);
STATIC DbSvc_TsInfo_t*	xmgr_swup_astra_GetCurrentHomeTsInfo (AstraOta_Contents_t *pstContents);
STATIC void				xmgr_swup_astra_AddHomeTsInfo (AstraOta_Contents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo);
STATIC void				xmgr_swup_astra_SetNextHomeTsInfo (AstraOta_Contents_t *pstContents);
STATIC void				xmgr_swup_astra_ClearAllHomeTsInfo (AstraOta_Contents_t *pstContents);
STATIC DbSvc_TripleId_t*	xmgr_swup_astra_GetCurrentRefNitLink (AstraOta_Contents_t *pstContents);
STATIC void				xmgr_swup_astra_AddRefNitLink (AstraOta_Contents_t *pstContents, DbSvc_TripleId_t *pstSsuScan);
STATIC void				xmgr_swup_astra_SetNextRefNitLink (AstraOta_Contents_t *pstContents);
STATIC void				xmgr_swup_astra_ClearAllRefNitLink (AstraOta_Contents_t *pstContents);
STATIC DbSvc_TripleId_t*	xmgr_swup_astra_GetCurrentSsuScanLink (AstraOta_Contents_t *pstContents);
STATIC void				xmgr_swup_astra_AddSsuScanLink (AstraOta_Contents_t *pstContents, DbSvc_TripleId_t *pstSsuScan);
STATIC void				xmgr_swup_astra_SetNextSsuScanLink (AstraOta_Contents_t *pstContents);
STATIC void				xmgr_swup_astra_ClearAllSsuScanLink (AstraOta_Contents_t *pstContents);
STATIC DbSvc_TripleId_t*	xmgr_swup_astra_GetCurrentSsuLink (AstraOta_Contents_t *pstContents);
STATIC void				xmgr_swup_astra_AddSsuLink (AstraOta_Contents_t *pstContents, DbSvc_TripleId_t *pstSsuLink);
STATIC void				xmgr_swup_astra_SetNextSsuLink (AstraOta_Contents_t *pstContents);
STATIC void				xmgr_swup_astra_ClearAllSsuLink (AstraOta_Contents_t *pstContents);

STATIC HERROR			xmgr_swup_astra_StartAction (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_StopAction (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_StartSi (AstraOta_Contents_t *pstContents);
STATIC DbSvc_TsInfo_t*	xmgr_swup_astra_AllocTsInfo (void);
STATIC void				xmgr_swup_astra_FreeTsInfo (void *pvTsInfo);
STATIC void				xmgr_swup_astra_ReleaseAction (AstraOta_Contents_t *pstContents);

STATIC void				xmgr_swup_astra_PrintTripleId (DbSvc_TripleId_t *pstTripleId);
STATIC void				xmgr_swup_astra_PrintCurrentTuningInfo (AstraOta_Contents_t *pstContents);

STATIC HERROR			xmgr_swup_astra_MsgGwmCreate (AstraOta_Contents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSwUpdate);
STATIC HERROR			xmgr_swup_astra_MsgGwmDestroyed (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_MsgGwmTimer (AstraOta_Contents_t *pstContents, HINT32 nTimerId);
STATIC HERROR			xmgr_swup_astra_EvtSiOtaSuccess (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_EvtSiOtaSuccessInAnchorTp (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_EvtSiOtaSuccessInRefNitTp (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_EvtSiOtaSuccessInSsuBatTp (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_EvtSiOtaSuccessInSsuSvcTp (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_EvtSiSearchFail (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_EvtChLocked (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_EvtChNoSignal (AstraOta_Contents_t *pstContents);
STATIC HERROR			xmgr_swup_astra_MsgGwmQuitStandby  (AstraOta_Contents_t *pstContents);



STATIC HERROR		xmgr_swup_astra_GetAstraAntIdx (HUINT32 *pulAntIdx);


#define	_________________________Body___________________________________

STATIC AstraOta_Contents_t *xmgr_swup_astra_GetContents (void)
{
	return &s_stAstraOta_Contents;
}

STATIC SvcSi_SwupCheckSpec_t* xmgr_swup_astra_GetOtaSpec(void)
{
	STATIC SvcSi_SwupCheckSpec_t 	stOtaSpec;

	HxSTD_MemSet(&stOtaSpec, 0x00, sizeof(SvcSi_SwupCheckSpec_t));
	stOtaSpec.eActionKind = eSVCSI_ACT_KIND_SwupCheck;
	stOtaSpec.ulActionSpec = eSVCSI_ACT_SPEC_Astra;

	return &stOtaSpec;
}

#define	____________________for__UI_____________________


#define _________________________________________________________________________________

STATIC INLINE HERROR xmgr_swup_astra_CheckActionId (AstraOta_Contents_t *pstContents, Handle_t hAction)
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

STATIC INLINE void xmgr_swup_astra_IncreaseSessionVersion (AstraOta_Contents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

STATIC void xmgr_swup_astra_FreeLinkItem (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}


#define	_____CONTENTS_INITIATION_____

STATIC HERROR xmgr_swup_astra_InitContents (AstraOta_Contents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSwUpdate)
{
	HERROR			 hErr;

	// 1. Clear all the contents.
	HxSTD_memset (pstContents, 0, sizeof(AstraOta_Contents_t));

	// 2. Action Handle and Action ID
	pstContents->hSwUpdate	= hSwUpdate;
	pstContents->hAction	= hAction;
	pstContents->eDeliType	= eDxDELIVERY_TYPE_SAT;
	pstContents->ulActionId	= SVC_PIPE_GetActionId (hAction);
	pstContents->ulTunerId	= (HUINT32)nTunerId;

	pstContents->bForcedStop = FALSE;

	HxSTD_memcpy(&pstContents->stOtaCheckSpec, xmgr_swup_astra_GetOtaSpec(), sizeof(SvcSi_SwupCheckSpec_t));

	// 3. tuning information :
	// TP List :
	hErr = xmgr_swup_astra_MakeHomeTpList (pstContents);
	if (hErr != ERR_OK)					{ return MESSAGE_BREAK; }

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_astra_MakeHomeTpList (AstraOta_Contents_t* pstContents)
{
	HUINT32					 ulTpCnt, ulTpNum;
	DbSvc_TsInfo_t			*pstTsInfo;
	HERROR					 hErr;

	if (pstContents == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	// Get the default TPs and the numbers.
	hErr = xmgr_swup_astra_CountOtaTsInfo (pstContents, &ulTpNum);
	if (hErr != ERR_OK || ulTpNum == 0)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
	{
		pstTsInfo = xmgr_swup_astra_AllocTsInfo();
		if (pstTsInfo != NULL)
		{
			hErr = xmgr_swup_astra_GetOtaTsInfo (pstContents, ulTpCnt, pstTsInfo);
			if (hErr == ERR_OK)
			{
				xmgr_swup_astra_AddHomeTsInfo (pstContents, pstTsInfo);
			}
			else
			{
				xmgr_swup_astra_FreeTsInfo (pstTsInfo);
			}
		}
	}

	if (xmgr_swup_astra_GetCurrentHomeTsInfo (pstContents) == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define	_____CONTENTS_DESTRUCTION_____

STATIC HERROR xmgr_swup_astra_DestroyContents (AstraOta_Contents_t *pstContents)
{
	xmgr_swup_astra_ClearAllHomeTsInfo (pstContents);
	xmgr_swup_astra_ClearAllRefNitLink (pstContents);
	xmgr_swup_astra_ClearAllSsuScanLink (pstContents);
	xmgr_swup_astra_ClearAllSsuLink (pstContents);

 	pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTsInfoListFromSi, (FREE_FUNC)xmgr_swup_astra_FreeLinkItem);

	return ERR_BUS_NO_ERROR;
}


#define	_____DATA_CONVERSION_____

STATIC HERROR xmgr_swup_astra_FindTsInfoByTripleId (AstraOta_Contents_t *pstContents, DbSvc_TripleId_t *pstTripleId, DbSvc_TsInfo_t *pstSvcTsInfo)
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

STATIC HERROR xmgr_swup_astra_TuneSearchTp (AstraOta_Contents_t *pstContents)
{
	HBOOL			 bRetry;
	SvcRsc_Info_t		 stRscInfo;
	HERROR			 hErr;

	bRetry = TRUE;

	while (bRetry == TRUE)
	{
		hErr = xmgr_swup_astra_SetTuningParam (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[%s,%d] No Tp left to tune\n",__FUNCTION__, __LINE__);

			hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("[%s,%d] No Next Tp left to tune\n",__FUNCTION__, __LINE__);
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

		xmgr_swup_astra_IncreaseSessionVersion (pstContents);			// Tuning 전에 반드시 Action Version을 업데이트해야 한다.

		HxLOG_Print("[astraota] Tune TP  : ");
		xmgr_swup_astra_PrintCurrentTuningInfo (pstContents);
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
		hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[%s,%d] No Tp left to tune\n",__FUNCTION__, __LINE__);
			pstContents->eAppState = eOtaApp_state_checkend;
			bRetry = FALSE;
		}
	}

	return ERR_BUS_NO_OBJECT;
}

STATIC HERROR xmgr_swup_astra_SetTuningParam (AstraOta_Contents_t *pstContents)
{
	DbSvc_TsInfo_t		 stSvcTsInfo;
	DbSvc_TsInfo_t		*pstTsInfo;
	DbSvc_TripleId_t		*pstTripleId;
	HERROR				 hErr;

	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_ASTRA_HOME_TP:
		pstTsInfo = xmgr_swup_astra_GetCurrentHomeTsInfo (pstContents);
		if (pstTsInfo == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		stSvcTsInfo = *pstTsInfo;

		break;

	case eSI_SWUP_MODE_ASTRA_REFNIT_TP:
		// Reference NIT List
		pstTripleId = xmgr_swup_astra_GetCurrentRefNitLink (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		hErr = xmgr_swup_astra_FindTsInfoByTripleId (pstContents, pstTripleId, &stSvcTsInfo);
		if (hErr != ERR_BUS_NO_ERROR)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_ASTRA_SSUBAT_TP:
		// SSU SCAN List
		pstTripleId = xmgr_swup_astra_GetCurrentSsuScanLink (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		hErr = xmgr_swup_astra_FindTsInfoByTripleId (pstContents, pstTripleId, &stSvcTsInfo);
		if (hErr != ERR_BUS_NO_ERROR)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_ASTRA_SSUSVC_TP:
		// SSU LINK List
		pstTripleId = xmgr_swup_astra_GetCurrentSsuLink (pstContents);
		if (pstTripleId == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		hErr = xmgr_swup_astra_FindTsInfoByTripleId (pstContents, pstTripleId, &stSvcTsInfo);
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

STATIC HERROR xmgr_swup_astra_SetNextTpForOta (AstraOta_Contents_t *pstContents)
{
	switch (pstContents->eSiSwupMode)
	{
	case eSI_SWUP_MODE_ASTRA_HOME_TP:
		xmgr_swup_astra_SetNextHomeTsInfo (pstContents);
		if (xmgr_swup_astra_GetCurrentHomeTsInfo (pstContents) == NULL)
		{
			return ERR_BUS_NO_OBJECT;
		}

		break;

	case eSI_SWUP_MODE_ASTRA_REFNIT_TP:
		xmgr_swup_astra_SetNextRefNitLink (pstContents);
		if (xmgr_swup_astra_GetCurrentRefNitLink(pstContents) == NULL)
		{
			// REF-NIT TP 가 더 이상 남아 있지 않다 : 다음 HOME TP로 가서 작업을 한다.
			xmgr_swup_astra_SetSiOtaMode (pstContents, eSI_SWUP_MODE_ASTRA_HOME_TP);
			return xmgr_swup_astra_SetNextTpForOta (pstContents);
		}

		break;

	case eSI_SWUP_MODE_ASTRA_SSUBAT_TP:
		xmgr_swup_astra_SetNextSsuScanLink (pstContents);
		if (xmgr_swup_astra_GetCurrentSsuScanLink (pstContents) == NULL)
		{
			// ASTRA SSU-BAT TP 가 더 이상 남아 있지 않다 : 다음 REF-NIT TP로 가서 작업을 한다.
			xmgr_swup_astra_SetSiOtaMode (pstContents, eSI_SWUP_MODE_ASTRA_REFNIT_TP);
			return xmgr_swup_astra_SetNextTpForOta (pstContents);
		}

		break;

	case eSI_SWUP_MODE_ASTRA_SSUSVC_TP:
		xmgr_swup_astra_SetNextSsuLink (pstContents);
		if (xmgr_swup_astra_GetCurrentSsuLink (pstContents) == NULL)
		{
			// ASTRA SSU-SVC TP 가 더 이상 남아 있지 않다 : 다음 SSU-BAT TP로 가서 작업을 한다.
			xmgr_swup_astra_SetSiOtaMode (pstContents, eSI_SWUP_MODE_ASTRA_SSUBAT_TP);
			return xmgr_swup_astra_SetNextTpForOta (pstContents);
		}

		break;

	default:
		break;
	}

	return ERR_BUS_NO_ERROR;
}


#define	_____MESSAGE_FUNCTIONS_____


#define	_____CONTENTS_DATA_FUNCTIONS_____

STATIC HERROR xmgr_swup_astra_CountOtaTsInfo (AstraOta_Contents_t *pstContents, HUINT32 *pulTpNum)
{
	(void)pstContents;

	if (pulTpNum == NULL)					{ return ERR_BUS_TARGET_NULL; }

	*pulTpNum = MGR_SWUPINFO_CountOtaHomeTp();
	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_astra_GetOtaTsInfo (AstraOta_Contents_t *pstContents, HUINT32 ulTpNum, DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR		 hErr;

	(void)pstContents;

	if (pstTsInfo == NULL)					{ return ERR_BUS_TARGET_NULL; }

	hErr = MGR_SWUPINFO_GetOtaHomeTp (ulTpNum, pstTsInfo);
	if (hErr != ERR_OK)						{ return ERR_BUS_OUT_OF_RANGE; }

	pstTsInfo->ucTunerId = (HUINT8)pstContents->ulTunerId;
	return ERR_BUS_NO_ERROR;
}

STATIC void xmgr_swup_astra_SetSiOtaMode (AstraOta_Contents_t *pstContents, XsvcSi_SwupMode_e eOtaMode)
{
	HxLOG_Print("[astra_ota] SI Mode %d -> %d\n", pstContents->eSiSwupMode, eOtaMode);
	pstContents->eSiSwupMode = eOtaMode;
}


// Home Ts Info
STATIC DbSvc_TsInfo_t *xmgr_swup_astra_GetCurrentHomeTsInfo (AstraOta_Contents_t *pstContents)
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

STATIC void xmgr_swup_astra_AddHomeTsInfo (AstraOta_Contents_t *pstContents, DbSvc_TsInfo_t *pstHomeTsInfo)
{
	if (pstContents != NULL && pstHomeTsInfo != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstHomeTpList, (void *)pstHomeTsInfo);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstHomeTpList);
	}
}

STATIC void xmgr_swup_astra_SetNextHomeTsInfo (AstraOta_Contents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstHomeTpList != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstHomeTpList, pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_astra_FreeLinkItem);
	}
}

STATIC void xmgr_swup_astra_ClearAllHomeTsInfo (AstraOta_Contents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstHomeTpList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstHomeTpList, (FREE_FUNC)xmgr_swup_astra_FreeLinkItem);
	}
}

// Reference NIT
STATIC DbSvc_TripleId_t *xmgr_swup_astra_GetCurrentRefNitLink (AstraOta_Contents_t *pstContents)
{
	if (pstContents != NULL)
	{
		if (pstContents->pstRefNitList != NULL)
		{
			return (DbSvc_TripleId_t *)pstContents->pstRefNitList->pvContents;
		}
	}

	return NULL;
}

STATIC void xmgr_swup_astra_AddRefNitLink (AstraOta_Contents_t *pstContents, DbSvc_TripleId_t *pstSsuScan)
{
	if (pstContents != NULL && pstSsuScan != NULL)
	{
		pstContents->pstRefNitList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstRefNitList, (void *)pstSsuScan);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstRefNitList);
	}
}

STATIC void xmgr_swup_astra_SetNextRefNitLink (AstraOta_Contents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstRefNitList != NULL)
	{
		pstContents->pstRefNitList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstRefNitList, pstContents->pstRefNitList, (FREE_FUNC)xmgr_swup_astra_FreeLinkItem);
	}
}

STATIC void xmgr_swup_astra_ClearAllRefNitLink (AstraOta_Contents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstRefNitList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstRefNitList, (FREE_FUNC)xmgr_swup_astra_FreeLinkItem);
	}
}


// SSU SCAN LINK : SSU-BAT
STATIC DbSvc_TripleId_t *xmgr_swup_astra_GetCurrentSsuScanLink (AstraOta_Contents_t *pstContents)
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

STATIC void xmgr_swup_astra_AddSsuScanLink (AstraOta_Contents_t *pstContents, DbSvc_TripleId_t *pstSsuScan)
{
	if (pstContents != NULL && pstSsuScan != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSsuScanList, (void *)pstSsuScan);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstSsuScanList);
	}
}

STATIC void xmgr_swup_astra_SetNextSsuScanLink (AstraOta_Contents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstSsuScanList != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuScanList, pstContents->pstSsuScanList, (FREE_FUNC)xmgr_swup_astra_FreeLinkItem);
	}
}

STATIC void xmgr_swup_astra_ClearAllSsuScanLink (AstraOta_Contents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuScanList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuScanList, (FREE_FUNC)xmgr_swup_astra_FreeLinkItem);
	}
}

// SSU LINK : SSU-SVC
STATIC DbSvc_TripleId_t *xmgr_swup_astra_GetCurrentSsuLink (AstraOta_Contents_t *pstContents)
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

STATIC void xmgr_swup_astra_AddSsuLink (AstraOta_Contents_t *pstContents, DbSvc_TripleId_t *pstSsuLink)
{
	if (pstContents != NULL && pstSsuLink != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSsuLinkList, (void *)pstSsuLink);
		UTIL_LINKEDLIST_CircularizePointerList (pstContents->pstSsuLinkList);
	}
}

STATIC void xmgr_swup_astra_SetNextSsuLink (AstraOta_Contents_t *pstContents)
{
	if (pstContents != NULL && pstContents->pstSsuLinkList != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSsuLinkList, pstContents->pstSsuLinkList, (FREE_FUNC)xmgr_swup_astra_FreeLinkItem);
	}
}

STATIC void xmgr_swup_astra_ClearAllSsuLink (AstraOta_Contents_t *pstContents)
{
	if (pstContents != NULL)
	{
		pstContents->pstSsuLinkList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSsuLinkList, (FREE_FUNC)xmgr_swup_astra_FreeLinkItem);
	}
}




#define	_____PROCESSING_FUNCTIONS_____

STATIC HERROR xmgr_swup_astra_StartAction (AstraOta_Contents_t *pstContents)
{
	// UI -> Search Action 으로 오는 메시지는 다른 Search Action 으로도 갈 수 있는 메시지이기 때문에 (NULL)
	// 절대로 MESSAGE_BREAK 를 사용하면 안된다.
	HERROR			 hErr;

	xmgr_swup_astra_SetSiOtaMode (pstContents, eSI_SWUP_MODE_ASTRA_HOME_TP);

	// 1. Set the TP to tuning :
	hErr = xmgr_swup_astra_SetTuningParam (pstContents);
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
	hErr = xmgr_swup_astra_TuneSearchTp (pstContents);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_astra_StopAction (AstraOta_Contents_t *pstContents)
{
	if (pstContents->eAppState == eOtaApp_state_waitingSi)
	{
		// Stop SI
		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
	}

	if (pstContents->eAppState != eOtaApp_state_init)
	{
		xmgr_swup_astra_ReleaseAction (pstContents);
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_astra_StartSi (AstraOta_Contents_t *pstContents)
{
	HUINT16			 			usExtId1, usExtId2;
	DbSvc_TripleId_t				*pstTripleId;
	SvcSi_SwupCheckOption_t			stOtaCheckOption;

	usExtId1 = 0;
	usExtId2 = 0;

	// SSU Link에 있는 SVCID가 필요하다.
	if (pstContents->eSiSwupMode == eSI_SWUP_MODE_ASTRA_SSUSVC_TP)
	{
		pstTripleId = xmgr_swup_astra_GetCurrentSsuLink (pstContents);
		if (pstTripleId != NULL)
		{
			usExtId1 = pstTripleId->usSvcId;
		}
	}

	stOtaCheckOption.ulSwupMode = eSI_SWUP_MODE_ASTRA_HOME_TP + abs(pstContents->eSiSwupMode - eSI_SWUP_MODE_ASTRA_HOME_TP);	// new si 적용이 완료되면 new type으로 모두 변경하자 !
	stOtaCheckOption.usExtId1 = usExtId1;
	stOtaCheckOption.usExtId2 = usExtId2;

	HxLOG_Print("[%s, %d] eOtaMode(0x%x) ExtId1(0x%x) ExtId2(0x%x)\n,",
		__FUNCTION__, __LINE__, stOtaCheckOption.ulSwupMode, stOtaCheckOption.usExtId1, stOtaCheckOption.usExtId2 );

	return SVC_SI_StartSwupCheck(pstContents->hAction, &pstContents->stOtaCheckSpec, &stOtaCheckOption, FALSE);
}

STATIC DbSvc_TsInfo_t *xmgr_swup_astra_AllocTsInfo (void)
{
	return (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
}

STATIC void xmgr_swup_astra_FreeTsInfo (void *pvTsInfo)
{
	if (pvTsInfo != NULL)
	{
		OxAP_Free (pvTsInfo);
	}
}

STATIC void xmgr_swup_astra_ReleaseAction (AstraOta_Contents_t *pstContents)
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

STATIC void	xmgr_swup_astra_PrintTripleId (DbSvc_TripleId_t *pstTripleId)
{
#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s, %d] (%04x:%04x:%04x)", __FUNCTION__, __LINE__, pstTripleId->usOnId, pstTripleId->usTsId, pstTripleId->usSvcId);
#else
	(void)pstTripleId;
#endif
}

STATIC void xmgr_swup_astra_PrintCurrentTuningInfo (AstraOta_Contents_t *pstContents)
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
STATIC HERROR xmgr_swup_astra_MsgGwmCreate (AstraOta_Contents_t *pstContents, Handle_t hAction, HINT32 nTunerId, Handle_t hSwUpdate)
{
	HERROR		 hErr;

	// Create시 해 주어야 하는 Initialize 등이 실패할 경우에는 이 Action 자체가 살아 있어서는 안된다.
	// 곧바로 Destroy 해 주도록 한다.

	// 1. Initialize the contents
	hErr = xmgr_swup_astra_InitContents (pstContents, hAction, nTunerId, hSwUpdate);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
		return hErr;
	}

	// 2. Start the OTA Action :
	hErr = xmgr_swup_astra_StartAction (pstContents);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0, eSWUP_FAILREASON_Action);
		return hErr;
	}

	MGR_SWUPINFO_ResetSearchedInfo();

	return ERR_BUS_NO_ERROR;
}

// eMEVT_BUS_DESTROY:
STATIC HERROR xmgr_swup_astra_MsgGwmDestroyed (AstraOta_Contents_t *pstContents)
{
	// Stop all the Action
	xmgr_swup_astra_StopAction (pstContents);

	// Destroy the contents
	xmgr_swup_astra_DestroyContents (pstContents);

	// Inform that the OTA action is destroyed
	if (pstContents->bForcedStop != TRUE)
	{
//		BUS_PostMessage (NULL, MSG_APP_OTA_ACTION_DESTROYED, (Handle_t)NULL, (HINT32)BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()), 0, 0);
	}

	return MESSAGE_BREAK;
}

// eMEVT_BUS_TIMER:
STATIC HERROR xmgr_swup_astra_MsgGwmTimer (AstraOta_Contents_t *pstContents, HINT32 nTimerId)
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
			hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_astra_TuneSearchTp (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_astra_MsgGwmTimer] No Next Tp left to tune\n");
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
				hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
				if (hErr == ERR_OK)
				{
					hErr = xmgr_swup_astra_TuneSearchTp (pstContents);
				}
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_astra_MsgGwmTimer] Cannot proceed anymore\n");
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
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
	}

	return hRetValue;
}

// EVT_SI_OTA_SUCCESS:
STATIC HERROR xmgr_swup_astra_EvtSiOtaSuccess (AstraOta_Contents_t *pstContents)
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
		case eSI_SWUP_MODE_ASTRA_HOME_TP:
			hErr = xmgr_swup_astra_EvtSiOtaSuccessInAnchorTp (pstContents);
			break;

		case eSI_SWUP_MODE_ASTRA_REFNIT_TP:
			hErr = xmgr_swup_astra_EvtSiOtaSuccessInRefNitTp (pstContents);
			break;

		case eSI_SWUP_MODE_ASTRA_SSUBAT_TP:
			hErr = xmgr_swup_astra_EvtSiOtaSuccessInSsuBatTp (pstContents);
			break;

		case eSI_SWUP_MODE_ASTRA_SSUSVC_TP:
			hErr = xmgr_swup_astra_EvtSiOtaSuccessInSsuSvcTp (pstContents);
			break;

		default:
			break;
		}

		SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);

		// 다음 TP 로 넘어가야 한다.
		if (pstContents->eAppState != eOtaApp_state_checkend)
		{
			hErr = xmgr_swup_astra_TuneSearchTp (pstContents);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_astra_EvtSiSearchDone] No Next Tp left to tune\n");
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
		if((pstContents->eSiSwupMode != eSI_SWUP_MODE_ASTRA_SSUSVC_TP)
			|| ((pstContents->eSiSwupMode == eSI_SWUP_MODE_ASTRA_SSUSVC_TP) && (hErr != ERR_OK)))
		{
			MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);
		}
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_astra_EvtSiOtaSuccessInAnchorTp (AstraOta_Contents_t *pstContents)
{
	HBOOL				 bSiDataFound;
	HUINT32				 ulIndex, ulDataNum;
	XsvcSi_SwupMode_e		 eNextOtaMode;
	DbSvc_TsInfo_t		*pstHomeTsInfo;
	DbSvc_TsInfo_t		*pstTsInfo, *pstTsInfoArray;
	DbSvc_TripleId_t		*pstTripleId, *pstTripleIdArray;
	HERROR				 hErr;

	bSiDataFound = TRUE;
	eNextOtaMode = eSI_SWUP_MODE_ASTRA_SSUBAT_TP;

	pstHomeTsInfo = xmgr_swup_astra_GetCurrentHomeTsInfo (pstContents);
	if (pstHomeTsInfo != NULL)
	{
		pstContents->ulAntennaIndex = (HUINT32)pstHomeTsInfo->stTuningParam.sat.ucAntId;

		hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, &ulDataNum, (void **)&pstTsInfoArray);
		if (hErr == ERR_OK && pstTsInfoArray != NULL)
		{
			HxLOG_Print("[%s,%d] NIT TS Info : %d\n", __FUNCTION__, __LINE__, ulDataNum);

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

					HxLOG_Print("[%s, %d] TP (%d) ", __FUNCTION__, __LINE__, ulIndex);
					XMGR_SWUP_PrintTuningInfo(pstTsInfo->eDeliType, &pstTsInfo->stTuningParam);
					HxLOG_Print("\n");

					pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTsInfoListFromSi, (void *)pstTsInfo);
				}
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, (void *)pstTsInfoArray);
		}
		else
		{
			bSiDataFound = FALSE;
		}

		// Reference-NIT
		hErr = SVC_SI_GetSwupCheckData (pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_REFNIT_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
		if (hErr == ERR_OK && pstTripleIdArray != NULL)
		{
			HxLOG_Print("[%s,%d] REF-NIT : %d\n", __FUNCTION__, __LINE__, ulDataNum);
			eNextOtaMode = eSI_SWUP_MODE_ASTRA_REFNIT_TP;

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
				if (pstTripleId != NULL)
				{
					*pstTripleId = pstTripleIdArray[ulIndex];
					xmgr_swup_astra_AddRefNitLink (pstContents, pstTripleId);

					HxLOG_Print("[%s,%d] REF-NIT (%d) ", __FUNCTION__, __LINE__, ulIndex);
					xmgr_swup_astra_PrintTripleId (pstTripleId);
					HxLOG_Print("\n");
				}
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_REFNIT_TRIPLEID, (void *)pstTripleIdArray);
		}
		else
		{
			bSiDataFound = FALSE;
		}
	}
	else
	{
		bSiDataFound = FALSE;
	}

	if (bSiDataFound == TRUE)
	{
		xmgr_swup_astra_SetSiOtaMode (pstContents, eNextOtaMode);
	}
	else
	{
		hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
		if (hErr != ERR_OK)			// 모든 TP를 다 체크함
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_astra_EvtSiOtaSuccessInRefNitTp (AstraOta_Contents_t *pstContents)
{
	HBOOL					 bSiDataFound;
	HUINT32					 ulIndex, ulDataNum;
	XsvcSi_SwupMode_e			 eNextOtaMode;
	DbSvc_TsInfo_t			*pstTsInfo, *pstTsInfoArray;
	DbSvc_TripleId_t		*pstTripleId, *pstTripleIdArray;
	HERROR					 hErr;

	bSiDataFound = TRUE;
	eNextOtaMode = eSI_SWUP_MODE_ASTRA_SSUBAT_TP;

	pstTripleId = xmgr_swup_astra_GetCurrentRefNitLink (pstContents);
	if (pstTripleId != NULL)
	{
		hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, &ulDataNum, (void **)&pstTsInfoArray);
		if (hErr == ERR_OK && pstTsInfoArray != NULL)
		{
			HxLOG_Print("[%s, %d] NIT TS Info : %d\n", __FUNCTION__, __LINE__, ulDataNum);

			pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstTsInfoListFromSi, (FREE_FUNC)xmgr_swup_astra_FreeLinkItem);

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
						pstTsInfo->stTuningParam.sat.ucAntId = (HUINT8)pstContents->ulAntennaIndex;
					}

					HxLOG_Print("[%s, %d]  TP (%d) ", __FUNCTION__,__LINE__, ulIndex);
					XMGR_SWUP_PrintTuningInfo(pstTsInfo->eDeliType, &pstTsInfo->stTuningParam);
					HxLOG_Print("\n");

					pstContents->pstTsInfoListFromSi = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstTsInfoListFromSi, (void *)pstTsInfo);
				}
			}

			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_DELIVERY_TSINFO, (void *)pstTsInfoArray);
		}
		else
		{
			bSiDataFound = FALSE;
		}

		// SSU-BAT
		hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCAN_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
		if (hErr == ERR_OK && pstTripleIdArray != NULL)
		{
			HxLOG_Print("[%s, %d] SSU-BAT : %d\n", __FUNCTION__, __LINE__, ulDataNum);
			eNextOtaMode = eSI_SWUP_MODE_ASTRA_SSUBAT_TP;

			for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
			{
				pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
				if (pstTripleId != NULL)
				{
					*pstTripleId = pstTripleIdArray[ulIndex];
					xmgr_swup_astra_AddSsuScanLink (pstContents, pstTripleId);

					HxLOG_Print("[%s, %d]  SSU-BAT (%d) " , __FUNCTION__, __LINE__,ulIndex);
					xmgr_swup_astra_PrintTripleId (pstTripleId);
					HxLOG_Print("\n");
				}
			}
			SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_SCAN_TRIPLEID, (void *)pstTripleIdArray);
		}
		else
		{
			hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
			if (hErr == ERR_OK && pstTripleIdArray != NULL)
			{
				HxLOG_Print("[%s, %d]  SSU-SVC : %d\n", __FUNCTION__, __LINE__, ulDataNum);
				eNextOtaMode = eSI_SWUP_MODE_ASTRA_SSUSVC_TP;

				for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
				{
					pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
					if (pstTripleId != NULL)
					{
						*pstTripleId = pstTripleIdArray[ulIndex];
						xmgr_swup_astra_AddSsuLink (pstContents, pstTripleId);

						HxLOG_Print("[%s, %d]  SSU-SVC (%d) ", __FUNCTION__,__LINE__, ulIndex);
						xmgr_swup_astra_PrintTripleId (pstTripleId);
						HxLOG_Print("\n");
					}
				}
				SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, (void *)pstTripleIdArray);
			}
			else
			{
				bSiDataFound = FALSE;
			}
		}
	}
	else
	{
		bSiDataFound = FALSE;
	}

	if (bSiDataFound == TRUE)
	{
		xmgr_swup_astra_SetSiOtaMode (pstContents, eNextOtaMode);
	}
	else
	{
		hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
		if (hErr != ERR_OK)			// 모든 TP를 다 체크함
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_astra_EvtSiOtaSuccessInSsuBatTp (AstraOta_Contents_t *pstContents)
{
	HBOOL				 bSiDataFound;
	HUINT32				 ulIndex, ulDataNum;
	DbSvc_TripleId_t		*pstTripleId, *pstTripleIdArray;
	HERROR				 hErr;

	bSiDataFound = TRUE;

	// SSU-SVC
	hErr = SVC_SI_GetSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, &ulDataNum, (void **)&pstTripleIdArray);
	if (hErr == ERR_OK && pstTripleIdArray != NULL)
	{
		HxLOG_Print("[%s, %d]  SSU-SVC : %d\n", __FUNCTION__,__LINE__, ulDataNum);

		for (ulIndex = 0; ulIndex < ulDataNum; ulIndex++)
		{
			pstTripleId = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t));
			if (pstTripleId != NULL)
			{
				*pstTripleId = pstTripleIdArray[ulIndex];
				xmgr_swup_astra_AddSsuLink (pstContents, pstTripleId);

				HxLOG_Print("[%s, %d]  SSU-SVC (%d) ", __FUNCTION__, __LINE__,ulIndex);
				xmgr_swup_astra_PrintTripleId (pstTripleId);
				HxLOG_Print("\n");
			}
		}
		SVC_SI_FreeSwupCheckData(pstContents->hAction, &pstContents->stOtaCheckSpec, eSI_SWUP_DATA_SSU_LINK_TRIPLEID, (void *)pstTripleIdArray);
	}
	else
	{
		bSiDataFound = FALSE;
	}

	if (bSiDataFound == TRUE)
	{
		xmgr_swup_astra_SetSiOtaMode (pstContents, eSI_SWUP_MODE_ASTRA_SSUSVC_TP);
	}
	else
	{
		hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
		if (hErr != ERR_OK)			// 모든 TP를 다 체크함
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC HERROR xmgr_swup_astra_EvtSiOtaSuccessInSsuSvcTp (AstraOta_Contents_t *pstContents)
{
	HBOOL				 bSiDataFound;
	HUINT32				 ulDataNum;
	SvcSi_SwupSignal_t	 *pstOtaArray	= NULL, *pstOtaInfo= NULL;
	DxTuneParam_t		 stChTuneInfo;
	DbSvc_TsInfo_t		 *pstHomeTsInfo = NULL;
	DbSvc_TripleId_t	 *pstTripleId	= NULL;
	HERROR				 hErr, hResult	= ERR_BUS_NO_ERROR;
	DbSvc_TsInfo_t		 *pstOtaTsInfo	= NULL;
	SvcSwUpdate_DetectResult_t *pstDetectResult = NULL;

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
			goto ERROR;
		}

		HxSTD_MemSet(pstOtaInfo, 0, sizeof(SvcSi_SwupSignal_t));
		HxSTD_MemCopy(pstOtaInfo, &(pstOtaArray[0]), sizeof(SvcSi_SwupSignal_t));

		// 찾은 OTA 정보를 기록한다.
		HxLOG_Print("[%s, %d] Found OTA Info \n",__FUNCTION__, __LINE__);
		HxLOG_Print("[%s, %d]  System ID : 0x%08x ~ 0x%08x \n", __FUNCTION__, __LINE__,pstOtaInfo->ulStartSystemId, pstOtaInfo->ulEndSystemId);
		HxLOG_Print("[%s, %d]  System Ver : %08d \n", __FUNCTION__, __LINE__,pstOtaInfo->ulDataVersion);
		HxLOG_Print("[%s, %d]  PID : %04x \n", __FUNCTION__, __LINE__,pstOtaInfo->usPid);

		pstOtaTsInfo = (DbSvc_TsInfo_t*)HLIB_STD_MemAlloc(sizeof(DbSvc_TsInfo_t));
		if(pstOtaTsInfo == NULL)
		{
			hResult = ERR_FAIL;
			goto ERROR;
		}

		// Write OTA TS Info
		HxSTD_MemSet(pstOtaTsInfo, 0, sizeof(DbSvc_TsInfo_t));

		hErr = SVC_CH_GetLockedInfo (pstContents->ulActionId, &stChTuneInfo);
		if (hErr != ERR_OK)
		{
			stChTuneInfo = pstContents->stTuningInfo;
		}

		OTL_CONV_DxTuneParam2DbTuningInfo((const DxTuneParam_t *)&stChTuneInfo, &pstOtaTsInfo->stTuningParam);

		pstHomeTsInfo = xmgr_swup_astra_GetCurrentHomeTsInfo (pstContents);
		if (pstHomeTsInfo != NULL)
		{
			pstOtaTsInfo->eDeliType					= pstHomeTsInfo->eDeliType;
			pstOtaTsInfo->ucTunerId					= pstHomeTsInfo->ucTunerId;
			pstOtaTsInfo->stTuningParam.sat.ucAntId	= pstHomeTsInfo->stTuningParam.sat.ucAntId;
		}

		pstTripleId = xmgr_swup_astra_GetCurrentSsuLink (pstContents);
		if (pstTripleId != NULL)
		{
			pstOtaTsInfo->usOnId = pstTripleId->usOnId;
			pstOtaTsInfo->usTsId = pstTripleId->usTsId;
		}

		pstDetectResult = (SvcSwUpdate_DetectResult_t*)HLIB_STD_MemAlloc(sizeof(SvcSwUpdate_DetectResult_t));
		if(pstDetectResult == NULL)
		{
			hResult = ERR_FAIL;
			goto ERROR;
		}

		HxSTD_MemSet(pstDetectResult, 0x00, sizeof(SvcSwUpdate_DetectResult_t));

		pstDetectResult->bAvailable = TRUE;
		pstDetectResult->bFound 	= TRUE;
		pstDetectResult->ulDataVersion 		= pstOtaInfo->ulDataVersion;
		pstDetectResult->usOtaPid	   		= pstOtaInfo->usPid;
		pstDetectResult->eSource			= eDxSWUPDATE_SOURCE_Rf;

		// emergency set
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stChTuningInfo, &stChTuneInfo, sizeof(DxTuneParam_t));
		HxSTD_MemCopy(&pstDetectResult->data.stRfInfo.stSvcTsInfo, pstOtaTsInfo, sizeof(DbSvc_TsInfo_t));
		pstDetectResult->data.stRfInfo.eDeliType	=	eDxDELIVERY_TYPE_SAT;
		pstDetectResult->data.stRfInfo.usPid		=	pstOtaInfo->usPid;
		pstDetectResult->data.stRfInfo.eDownloadType = eSWUP_DOWNLOAD_DVBSSU;

		MGR_SWUP_SetDetectInfo(pstContents->hSwUpdate, pstDetectResult);

		hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_OTA_DETECT_INFO, (HUINT32)pstDetectResult, sizeof(SvcSwUpdate_DetectResult_t));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("DB_PARAM_SetItem() return Error : result [0x%x]\n", hErr);
			hResult = ERR_FAIL;
			goto ERROR;
		}

		MGR_SWUPINFO_SetSearchedInfo (pstOtaTsInfo, sizeof(SvcSi_SwupSignal_t), (void *)pstOtaInfo);

		// detect 보내기..
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_SUCCESS, eSWUP_STATE_Unknown, (HINT32)pstOtaTsInfo,(HINT32)pstOtaInfo,0);

		pstContents->eAppState = eOtaApp_state_checkend;
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

	if(pstOtaTsInfo != NULL)
	{
		HLIB_STD_MemFree(pstOtaTsInfo);
		pstOtaTsInfo = NULL;
	}

	if(pstOtaInfo!= NULL)
	{
		HLIB_STD_MemFree(pstOtaInfo);
		pstOtaInfo = NULL;
	}

	if(pstDetectResult != NULL)
	{
		HLIB_STD_MemFree(pstDetectResult);
		pstDetectResult = NULL;
	}

	if( hResult != ERR_OK)
	{
		MGR_SWUP_PostMessage(pstContents->hSwUpdate, eMEVT_SWUP_NOTIFY_DETECT_FAIL, eSWUP_STATE_Unknown, 0,0,0);

		hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
		if (hErr != ERR_OK) 		// 모든 TP를 다 체크함
		{
			pstContents->eAppState = eOtaApp_state_checkend;
			return ERR_FAIL;
		}
	}

	return ERR_BUS_NO_ERROR;
}

// eSEVT_SI_SEARCH_FAIL:
STATIC HERROR xmgr_swup_astra_EvtSiSearchFail (AstraOta_Contents_t *pstContents)
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
		hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
		if (hErr == ERR_OK)
		{
			// Tune the TP
			hErr = xmgr_swup_astra_TuneSearchTp (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_swup_astra_EvtSiSearchFail] No Next Tp left to tune\n");
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
	}

	return MESSAGE_BREAK;
}

// eSEVT_CH_LOCKED
STATIC HERROR xmgr_swup_astra_EvtChLocked (AstraOta_Contents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_tryTuning :
		// Channel Locked : Kill the timer
		BUS_KillTimer (OTA_TIMER_LOCKING_ID);

		// Start the SI for the ASTRA OTA
		hErr = xmgr_swup_astra_StartSi (pstContents);
		if (hErr == ERR_OK)
		{
			pstContents->eAppState = eOtaApp_state_waitingSi;
			// MW SI를 무한으로 기다리는 것을 막기 위해 여기에서 Timer를 걸어야 한다.
			BUS_SetTimer (OTA_TIMER_SI_ID, OTA_TIMER_SI_TIME);
		}
		else
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_astra_TuneSearchTp (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_astra_EvtChLocked] No Next Tp left to tune\n");
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
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_astra_EvtChNoSignal (AstraOta_Contents_t *pstContents)
{
	HERROR				 hErr;

	switch (pstContents->eAppState)
	{
	case eOtaApp_state_tryTuning :
		// Channel Locked : Kill the timer

		BUS_KillTimer (OTA_TIMER_LOCKING_ID);

		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
		if (hErr == ERR_OK)
		{
			hErr = xmgr_swup_astra_TuneSearchTp (pstContents);
		}

		if (hErr != ERR_OK)
		{
			HxLOG_Print("[xmgr_swup_astra_EvtChNoSignal] No Next Tp left to tune\n");
			pstContents->eAppState = eOtaApp_state_checkend;
		}

		break;

	case eOtaApp_state_waitingSi:
		// Stop SI :
		hErr = SVC_SI_StopSwupCheck(pstContents->hAction, TRUE);
		if (hErr == ERR_OK)
		{
			// 다음 TP 로 넘어가야 한다.
			hErr = xmgr_swup_astra_SetNextTpForOta (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_swup_astra_TuneSearchTp (pstContents);
			}

			if (hErr != ERR_OK)
			{
				HxLOG_Print("[xmgr_swup_astra_EvtChNoSignal] No Next Tp left to tune\n");
				pstContents->eAppState = eOtaApp_state_checkend;
			}
		}
		else
		{
			// 더 이상 진행 불가능 : Search End로 간주한다.
			HxLOG_Print("[xmgr_swup_astra_EvtChNoSignal] SI Re-open failed.\n");
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
	}

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_swup_astra_MsgGwmQuitStandby  (AstraOta_Contents_t *pstContents)
{
	pstContents->bForcedStop = TRUE;
	BUS_MGR_Destroy (NULL);

	return ERR_OK;
}



#define	_____EXTERNAL_PROCESS_FUNCTION_____

BUS_Result_t xproc_swup_Detect_Astra(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	STATIC HBOOL			 s_bFirstStart = TRUE;
	AstraOta_Contents_t		*pstContents;
	HERROR					 hErr;

	pstContents = xmgr_swup_astra_GetContents();
	if (s_bFirstStart == TRUE)
	{
		s_bFirstStart = FALSE;
		HxSTD_memset (pstContents, 0, sizeof(AstraOta_Contents_t));
	}

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Print("\t[eMEVT_BUS_CREATE]\n\n");
		BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "XMGR_OTA_ASTRA_Proc");
		// p2 is software update handle
		hErr = xmgr_swup_astra_MsgGwmCreate (pstContents, hAction, p1, p2);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_TIMER:
		HxLOG_Print("\t[eMEVT_BUS_TIMER]\n\n");
		hErr = xmgr_swup_astra_MsgGwmTimer (pstContents, p1);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Print("\t[eMEVT_BUS_DESTROY]\n\n");
		hErr = xmgr_swup_astra_MsgGwmDestroyed (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_SUCCESS:
		hErr = xmgr_swup_astra_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[eSEVT_SWUP_DETECT_SUCCESS]\n\n");

		hErr = xmgr_swup_astra_EvtSiOtaSuccess (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_SWUP_DETECT_FAIL:
		hErr = xmgr_swup_astra_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[eSEVT_SWUP_DETECT_FAIL]\n\n");

		hErr = xmgr_swup_astra_EvtSiSearchFail (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_LOCKED:
		hErr = xmgr_swup_astra_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[eSEVT_CH_LOCKED]\n\n");

		hErr = xmgr_swup_astra_EvtChLocked (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eSEVT_CH_NO_SIGNAL:
		hErr = xmgr_swup_astra_CheckActionId (pstContents, hAction);
		if (hErr == MESSAGE_PASS)			{ return MESSAGE_PASS; }

		HxLOG_Print("\t[eSEVT_CH_NO_SIGNAL]\n\n");

		hErr = xmgr_swup_astra_EvtChNoSignal (pstContents);
		if (hErr == MESSAGE_BREAK)			{ return MESSAGE_BREAK; }
		break;

	case eMEVT_BUS_GO_OPERATION :
		HxLOG_Print("\t[eMEVT_BUS_GO_OPERATION]\n\n");
		hErr = xmgr_swup_astra_MsgGwmQuitStandby (pstContents);
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;


	default:
		break;
	}

	//HxLOG_Print("AP_SatSrchSvc_Proc() out\n");

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


#define _____OTA_TP_____
#if 0
STATIC DbSvc_SatTuningInfo_t	 s_stAstraOtaHomeTuneInfo[] =
{
	{	12603,	22000,	eDxSAT_POLAR_HOR,	eDxSAT_CODERATE_5_6,	eDxSAT_TRANS_DVBS,	eDxSAT_PSK_QPSK,	0,	0,	0},
	{	0,	0,		0,		0,				0,					0,				0,				0,	0},
};
#endif

STATIC HERROR xmgr_swup_astra_GetAstraAntIdx (HUINT32 *pulAntIdx)
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

HERROR xmgr_swup_SetDefaultHomeTsInfo_Astra (void)
{
	DbSvc_TsInfo_t		*astTsInfo;
	DxOperator_e			  eOperator = eDxOPERATOR_None;
	HUINT32				  ulNumTs, ulCount;
	HERROR				 hErr;

	MGR_SWUPINFO_ResetHomeTp ();

	MGR_SPEC_GetOperatorCode(&eOperator);

	HxLOG_Debug("eOperator (%d) \n", eOperator);

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
#if 0
	HUINT32					 ulAntIdx;
	HUINT32					 ulCount;
	DbSvc_TsInfo_t			 stTsInfo;
	HERROR					 hErr;

	DbSvc_SatTuningInfo_t		*pstSatTuneInfo;
	hErr = xmgr_swup_astra_GetAstraAntIdx (&ulAntIdx);
	if (hErr == ERR_OK)
	{
		ulCount = 0;
		pstSatTuneInfo	= &(s_stAstraOtaHomeTuneInfo[ulCount]);

		while(pstSatTuneInfo && pstSatTuneInfo->ulFrequency != 0)
		{
			stTsInfo.eDeliType 					= eDxDELIVERY_TYPE_SAT;
			stTsInfo.stTuningParam.sat			= *pstSatTuneInfo;
			stTsInfo.stTuningParam.sat.ucAntId 	= (HUINT8)ulAntIdx;

			HxLOG_Debug("Default SAT Inform.  frequency : 0x%X, symbolRate : 0x%X , codeRate : 0x%X, pilot : 0x%X, ePolarization : 0x%X \n",
							stTsInfo.stTuningParam.sat.ulFrequency, stTsInfo.stTuningParam.sat.ulSymbolRate,
							stTsInfo.stTuningParam.sat.eFecCodeRate, stTsInfo.stTuningParam.sat.ePilot, stTsInfo.stTuningParam.sat.ePolarization);

			MGR_SWUPINFO_AddOtaHomeTp (&stTsInfo);

			pstSatTuneInfo	= &(s_stAstraOtaHomeTuneInfo[++ulCount]);
		}
	}

	return ERR_BUS_NO_ERROR;
#endif
}


#endif
/* End of File. ---------------------------------------------------------- */


