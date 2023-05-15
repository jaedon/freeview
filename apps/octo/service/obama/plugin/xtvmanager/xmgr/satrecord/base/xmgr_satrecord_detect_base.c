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
#include <octo_common.h>
#include <linkedlist.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_rscmap.h>
#include <mgr_satrecord.h>

#include <xmgr_module_search.h>
#include <_xmgr_satrecord_base.h>

#include <otl.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define ASTRA_RECORDING_LINKAGE			0xA0
#define HDPLUS_RECORDING_LINKAGE			0xA1

#define SATREC_TIMER_LOCKING_ID						0x2101
#define SATREC_TIMER_SI_ID								0x2102
#define SATREC_TIMER_LOCKING_TIME						(4*1000)			// 4sec
#define SATREC_TIMER_SI_TIME							(10*1000)			// 10sec

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSatRecState_detect_Start = 0,
	eSatRecState_detect_HomeTp_Tuning,
	eSatRecState_detect_HomeTp_WatingSi,
	eSatRecState_detect_RefNitTp_Tuning,
	eSatRecState_detect_RefNitTp_WatingSi,
	eSatRecState_detect_SatRecTp_Tuning,
	eSatRecState_detect_SatRecTp_WatingSi,

	eSatRecState_detect_SearchDone,
	eSatRecState_detect_SearchFail,

	eSatRecState_Finish,
} SatRec_DetectState_e;

typedef struct
{
	HUINT8			ucLinkType;
	HUINT16			usSvcId;
	DbSvc_TsInfo_t	stTsInfo;
} SatRec_SatRecTsInfo_t;

typedef struct
{
	HUINT32					ulActionId;
	Handle_t				hAction;

	HINT32					nAntId;
	SVC_SatType_t			eSatType;

	HBOOL					bActionTaken;

	POINTER_LIST_T			*pstHomeTsInfo; 		// Contents : DbSvc_TsInfo_t
	POINTER_LIST_T			*pstRefNitTsInfo;			// Contents : DbSvc_TsInfo_t
	POINTER_LIST_T			*pstDeliTsInfo;				// Contents : DbSvc_TsInfo_t
	POINTER_LIST_T			*pstSatRecTsInfo;			// Contents : SatRec_SatRecTsInfo_t
	POINTER_LIST_T			*pstSatRecInfo;				// Contents : DbSvc_TsInfo_t*/

	SatRec_DetectState_e	eState;

} xmgrSatrec_BaseDetectContents_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC xmgrSatrec_BaseDetectContents_t	s_stBase_Contents;


/*******************************************************************/
/********************      Functions         *************************/
/*******************************************************************/
static HERROR xmgr_satrecord_detect_LoadVariables (xmgrSatrec_BaseDetectContents_t *pstContents);
static HERROR xmgr_satrecord_detect_ReleaseAction (xmgrSatrec_BaseDetectContents_t *pstContents);

#define _____Static_Function_____

STATIC xmgrSatrec_BaseDetectContents_t *xmgr_satrecord_detect_GetContents_Base (void)
{
	return &s_stBase_Contents;
}

static INLINE HERROR xmgr_satrecord_detect_CheckActionId (xmgrSatrec_BaseDetectContents_t *pstContents, Handle_t hAction)
{
	HUINT32			 ulActionId;

	ulActionId = SVC_PIPE_GetActionId (hAction);
	if (pstContents->ulActionId != ulActionId)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static INLINE void xmgr_satrecord_detect_IncreaseSessionVersion (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	pstContents->hAction = SVC_PIPE_IncreaseActionVersion (pstContents->ulActionId);
}

#define	_____CONTENTS_DATA_FUNCTIONS_____

static void xmgr_satrecord_detect_FreeTsInfo (void *pvItem)
{
	if (pvItem != NULL)
	{
		OxAP_Free (pvItem);
	}
}

static HBOOL xmgr_satrecord_detect_FilterTsInfoSameOnTsId (void *pvItem, void *pvFilter)
{
	DbSvc_TsInfo_t		*pstItem, *pstFilter;

	pstItem = (DbSvc_TsInfo_t *)pvItem;
	pstFilter = (DbSvc_TsInfo_t *)pvFilter;

	if ((pstItem->eDeliType == pstFilter->eDeliType) &&
		(pstItem->usOnId == pstFilter->usOnId) &&
		(pstItem->usTsId == pstFilter->usTsId))
	{
		return TRUE;
	}

	return FALSE;
}

static HBOOL xmgr_satrecord_detect_FilterSatRecTsInfoSameTripleId (void *pvItem, void *pvFilter)
{
	SatRec_SatRecTsInfo_t		*pstItem, *pstFilter;

	pstItem = (SatRec_SatRecTsInfo_t *)pvItem;
	pstFilter = (SatRec_SatRecTsInfo_t *)pvFilter;

	if ((pstItem->usSvcId == pstFilter->usSvcId) &&
		(pstItem->stTsInfo.usOnId == pstFilter->stTsInfo.usOnId) &&
		(pstItem->stTsInfo.usTsId == pstFilter->stTsInfo.usTsId))
	{
		return TRUE;
	}

	return FALSE;
}

static HBOOL xmgr_satrecord_detect_FilterSatRecInfoSameDsmccPid (void *pvItem, void *pvFilter)
{
	SatRecordInfo_t		*pstItem, *pstFilter;

	pstItem = (SatRecordInfo_t *)pvItem;
	pstFilter = (SatRecordInfo_t *)pvFilter;

	if (pstItem->dsmccPid == pstFilter->dsmccPid)
	{
		return TRUE;
	}

	return FALSE;
}

static HERROR xmgr_satrecord_detect_GetHomeTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t	*pstSrcTsInfo;

	if (pstTsInfo == NULL)				{ return ERR_BUS_TARGET_NULL; }

	if (pstContents->pstHomeTsInfo != NULL)
	{
		pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstContents->pstHomeTsInfo);
		if (pstSrcTsInfo != NULL)
		{
			*pstTsInfo = *pstSrcTsInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_satrecord_detect_AppendHomeTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t		*pstHomeTsInfo;

	if (pstTsInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	// Home TS는 겹치지 않고 들어 왔다고 가정한다. (Hardcoding 된 정보)

	pstHomeTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
	if (pstHomeTsInfo == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

	*pstHomeTsInfo = *pstTsInfo;
	pstContents->pstHomeTsInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstHomeTsInfo,
																			(void *)pstHomeTsInfo);

	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_ClearHomeTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	pstContents->pstHomeTsInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstHomeTsInfo, xmgr_satrecord_detect_FreeTsInfo);
	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_GetRefNitTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t	*pstSrcTsInfo;

	if (pstTsInfo == NULL)				{ return ERR_BUS_TARGET_NULL; }

	if (pstContents->pstRefNitTsInfo != NULL)
	{
		pstSrcTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstContents->pstRefNitTsInfo);
		if (pstSrcTsInfo != NULL)
		{
			*pstTsInfo = *pstSrcTsInfo;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_satrecord_detect_AppendRefNitTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t		*pstRefNitTsInfo;
	POINTER_LIST_T		*pstItem;

	if (pstTsInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstRefNitTsInfo, 0,
															xmgr_satrecord_detect_FilterTsInfoSameOnTsId,
															(void *)pstTsInfo);

	if (pstItem != NULL)
	{
		pstRefNitTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstRefNitTsInfo != NULL)
		{
			*pstRefNitTsInfo = *pstTsInfo;
		}
	}
	else
	{
		pstRefNitTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
		if (pstRefNitTsInfo == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

		*pstRefNitTsInfo = *pstTsInfo;
		pstContents->pstRefNitTsInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstRefNitTsInfo,
																					(void *)pstRefNitTsInfo);
	}

	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_ClearRefNitTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	pstContents->pstRefNitTsInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstRefNitTsInfo, xmgr_satrecord_detect_FreeTsInfo);
	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_CountDeliveryTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents, HUINT32 *pulNumTsInfo)
{
	if (pulNumTsInfo == NULL)				{ return ERR_BUS_TARGET_NULL; }

	*pulNumTsInfo = (HUINT32)UTIL_LINKEDLIST_GetNumListItemsInPointerList (pstContents->pstDeliTsInfo, NULL, NULL);
	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_GetDeliTsInfoByOnTsId (xmgrSatrec_BaseDetectContents_t *pstContents, HUINT16 usOnId, HUINT16 usTsId, DbSvc_TsInfo_t **ppstTsInfo)
{
	DbSvc_TsInfo_t		 stFilterTsInfo;
	DbSvc_TsInfo_t		*pstDeliTsInfo;
	POINTER_LIST_T		*pstItem;

	if (ppstTsInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	stFilterTsInfo.eDeliType	= eDxDELIVERY_TYPE_SAT;
	stFilterTsInfo.usOnId		= usOnId;
	stFilterTsInfo.usTsId		= usTsId;

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstDeliTsInfo, 0,
															xmgr_satrecord_detect_FilterTsInfoSameOnTsId,
															(void *)&stFilterTsInfo);
	if (pstItem == NULL)
	{
		return ERR_FAIL;
	}

	pstDeliTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
	if (pstDeliTsInfo == NULL)
	{
		return ERR_FAIL;
	}

	*ppstTsInfo = pstDeliTsInfo;
	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_AppendDeliTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	DbSvc_TsInfo_t		stFilterTsInfo;
	DbSvc_TsInfo_t		*pstDeliTsInfo;
	POINTER_LIST_T		*pstItem;

	if (pstTsInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	HxSTD_memset (&stFilterTsInfo, 0, sizeof(DbSvc_TsInfo_t));

	stFilterTsInfo = *pstTsInfo;
	stFilterTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
	stFilterTsInfo.ucTunerId = (HUINT8)-1;
	stFilterTsInfo.stTuningParam.sat.ucAntId = (HUINT8)pstContents->nAntId;
	stFilterTsInfo.stTuningParam.sat.eSatType = pstContents->eSatType;

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstDeliTsInfo, 0,
															xmgr_satrecord_detect_FilterTsInfoSameOnTsId,
															(void *)&stFilterTsInfo);
	if (pstItem != NULL)
	{
		pstDeliTsInfo = (DbSvc_TsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstDeliTsInfo != NULL)
		{
			*pstDeliTsInfo = stFilterTsInfo;
		}
	}
	else
	{
		pstDeliTsInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t));
		if (pstDeliTsInfo == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

		*pstDeliTsInfo = stFilterTsInfo;
		pstContents->pstDeliTsInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstDeliTsInfo,
																				(void *)pstDeliTsInfo);
	}

	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_ClearDeliTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	pstContents->pstDeliTsInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstDeliTsInfo, xmgr_satrecord_detect_FreeTsInfo);
	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_GetSatRecTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo, HUINT16 *pusSvcId)
{
	SatRec_SatRecTsInfo_t	*pstSrcTsInfo;

	if (pstContents->pstSatRecTsInfo != NULL)
	{
		pstSrcTsInfo = (SatRec_SatRecTsInfo_t *)UTIL_LINKEDLIST_GetContents (pstContents->pstSatRecTsInfo);

		if (pstSrcTsInfo != NULL)
		{
			if(pstTsInfo != NULL)
				*pstTsInfo = pstSrcTsInfo->stTsInfo;

			if(pusSvcId != NULL)
				*pusSvcId  = pstSrcTsInfo->usSvcId;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static HERROR xmgr_satrecord_detect_AppendSatRecTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo, HUINT16 usSvcId, HUINT8 ucLinkType)
{
	SatRec_SatRecTsInfo_t	stSatRecTsInfo;
	SatRec_SatRecTsInfo_t	*pstSatRecTsInfo;
	POINTER_LIST_T			*pstItem;

	if (pstTsInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	stSatRecTsInfo.stTsInfo = *pstTsInfo;
	stSatRecTsInfo.usSvcId  = usSvcId;
	stSatRecTsInfo.ucLinkType = ucLinkType;

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstSatRecTsInfo, 0,
															xmgr_satrecord_detect_FilterSatRecTsInfoSameTripleId,
															(void *)&stSatRecTsInfo);

	HxLOG_Print("[%s:%d] : APPEND TS INFO - TSID (0x%x), ONID (0x%x), SVCID (0x%x)\n", __FUNCTION__, __LINE__, pstTsInfo->usTsId, pstTsInfo->usOnId, usSvcId);

	if (pstItem != NULL)
	{
		pstSatRecTsInfo = (SatRec_SatRecTsInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstSatRecTsInfo != NULL)
		{
			pstSatRecTsInfo->stTsInfo = *pstTsInfo;
		}
	}
	else
	{
		pstSatRecTsInfo = (SatRec_SatRecTsInfo_t *)OxAP_Malloc (sizeof(SatRec_SatRecTsInfo_t));
		if (pstSatRecTsInfo == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

		pstSatRecTsInfo->stTsInfo = *pstTsInfo;
		pstSatRecTsInfo->usSvcId  = usSvcId;
		pstSatRecTsInfo->ucLinkType = ucLinkType;

		pstContents->pstSatRecTsInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSatRecTsInfo,
																			(void *)pstSatRecTsInfo);
	}

	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_ClearSatRecTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	pstContents->pstSatRecTsInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSatRecTsInfo, xmgr_satrecord_detect_FreeTsInfo);
	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_AppendSatRecInfo (xmgrSatrec_BaseDetectContents_t *pstContents, SatRecordInfo_t *pstSatRecInfo)
{
	POINTER_LIST_T		*pstItem;
	SatRecordInfo_t		*pstTempSatRecInfo;

	if (pstSatRecInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }

	pstItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList (pstContents->pstSatRecInfo, 0,
															xmgr_satrecord_detect_FilterSatRecInfoSameDsmccPid,
															(void *)pstSatRecInfo);
	if (pstItem != NULL)
	{
		pstTempSatRecInfo = (SatRecordInfo_t *)UTIL_LINKEDLIST_GetContents (pstItem);
		if (pstTempSatRecInfo != NULL)
		{
			*pstTempSatRecInfo = *pstSatRecInfo;
		}
	}
	else
	{
		pstTempSatRecInfo = (SatRecordInfo_t *)OxAP_Malloc (sizeof(SatRecordInfo_t));
		if (pstTempSatRecInfo == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

		*pstTempSatRecInfo = *pstSatRecInfo;
		pstContents->pstSatRecInfo = UTIL_LINKEDLIST_AppendListItemToPointerList (pstContents->pstSatRecInfo,
																					(void *)pstTempSatRecInfo);
	}

	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_ClearSatRecInfo (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	pstContents->pstSatRecInfo = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList (pstContents->pstSatRecInfo, xmgr_satrecord_detect_FreeTsInfo);
	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_GetCurrentTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents, DbSvc_TsInfo_t *pstTsInfo)
{
	if (pstTsInfo == NULL)				{ return ERR_BUS_TARGET_NULL; }

	switch (pstContents->eState)
	{
		case eSatRecState_detect_HomeTp_Tuning:
			return xmgr_satrecord_detect_GetHomeTsInfo (pstContents, pstTsInfo);

		case eSatRecState_detect_RefNitTp_Tuning:
			return xmgr_satrecord_detect_GetRefNitTsInfo (pstContents, pstTsInfo);

		case eSatRecState_detect_SatRecTp_Tuning:
			return xmgr_satrecord_detect_GetSatRecTsInfo (pstContents, pstTsInfo, NULL);

		default:
			break;
	}

	return ERR_FAIL;
}

static HERROR xmgr_satrecord_detect_SetToNextTsInfo (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	switch (pstContents->eState)
	{
		case eSatRecState_detect_HomeTp_Tuning:
			pstContents->pstHomeTsInfo = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstHomeTsInfo, pstContents->pstHomeTsInfo, xmgr_satrecord_detect_FreeTsInfo);
			if (pstContents->pstHomeTsInfo == NULL)
			{
				return ERR_FAIL;
			}
			return ERR_OK;

		case eSatRecState_detect_RefNitTp_Tuning:
			pstContents->pstRefNitTsInfo = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstRefNitTsInfo, pstContents->pstRefNitTsInfo, xmgr_satrecord_detect_FreeTsInfo);
			if (pstContents->pstRefNitTsInfo == NULL)
			{
				pstContents->eState = eSatRecState_detect_HomeTp_Tuning;
				return xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
			}
			return ERR_OK;

		case eSatRecState_detect_SatRecTp_Tuning:
			pstContents->pstSatRecTsInfo = UTIL_LINKEDLIST_DeleteListItemFromPointerList (pstContents->pstSatRecTsInfo, pstContents->pstSatRecTsInfo, xmgr_satrecord_detect_FreeTsInfo);
			if (pstContents->pstSatRecTsInfo == NULL)
			{
				pstContents->eState = eSatRecState_detect_RefNitTp_Tuning;
				return xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
			}
			return ERR_OK;

		default:
			break;
	}

	return ERR_FAIL;
}

static HERROR xmgr_satrecord_detect_GetDeliveryTsInfoFromSi (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR			 			hErr;
	SvcSi_SatRecordSpec_t		stSiSpec;

	HUINT32 					ulTsCnt, ulTsNum;
	DbSvc_TsInfo_t				*pstTsArray;

	hErr = MGR_SATRECORD_GetSatRecSiSpec (&stSiSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	// Get the Delivery TS Info
	hErr = SVC_SI_GetSatRecordData (pstContents->hAction, &stSiSpec, eSVCSI_SATRECORD_DATA_SES_DELIVERY_TSINFO, &ulTsNum, (void **)&pstTsArray);
	if (hErr != ERR_OK || ulTsNum == 0)
	{
		return ERR_FAIL;
	}

	for (ulTsCnt = 0; ulTsCnt < ulTsNum; ulTsCnt++)
	{
		if (pstTsArray[ulTsCnt].eDeliType == eDxDELIVERY_TYPE_SAT)
		{
			xmgr_satrecord_detect_AppendDeliTsInfo (pstContents, &(pstTsArray[ulTsCnt]));
		}
	}

	SVC_SI_FreeSatRecordData (pstContents->hAction, &stSiSpec, eSVCSI_EXTSEARCH_DATA_SES_DELIVERY_TSINFO, (void *)pstTsArray);

	return hErr;
}

static HERROR xmgr_satrecord_detect_GetRefNitTsInfoFromSi (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR			 			hErr;
	SvcSi_SatRecordSpec_t		stSiSpec;

	HBOOL			 			bNitTsInfoFound = FALSE;
	HUINT32			 			ulLinkCnt, ulLinkNum;
	DbSvc_TripleId_t			*pstTripleIdArray;
	DbSvc_TsInfo_t				*pstTsInfo;

	hErr = MGR_SATRECORD_GetSatRecSiSpec (&stSiSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = SVC_SI_GetSatRecordData (pstContents->hAction, &stSiSpec, eSVCSI_SATRECORD_DATA_SES_REFNITLINK_TRIPLEID, &ulLinkNum, (void **)&pstTripleIdArray);
	if (hErr != ERR_OK || ulLinkNum == 0)
	{
		return ERR_FAIL;
	}

	for (ulLinkCnt = 0; ulLinkCnt < ulLinkNum; ulLinkCnt++)
	{
		hErr = xmgr_satrecord_detect_GetDeliTsInfoByOnTsId (pstContents,
														pstTripleIdArray[ulLinkCnt].usOnId,
														pstTripleIdArray[ulLinkCnt].usTsId,
														&pstTsInfo);

		if (hErr == ERR_OK)
		{
			bNitTsInfoFound = TRUE;
			xmgr_satrecord_detect_AppendRefNitTsInfo (pstContents, pstTsInfo);
		}
	}

	SVC_SI_FreeSatRecordData (pstContents->hAction, &stSiSpec, eSVCSI_SATRECORD_DATA_SES_REFNITLINK_TRIPLEID, (void *)pstTripleIdArray);

	return (bNitTsInfoFound == TRUE) ? ERR_OK : ERR_FAIL;
}

static HERROR xmgr_satrecord_detect_GetSatRecordTsInfoFromSi (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR			 			hErr;
	SvcSi_SatRecordSpec_t		stSiSpec;

	HBOOL			 			bAstraRRTsInfoFound = FALSE, bHDPlusRRTsInfoFound = FALSE;
	HUINT32			 			ulLinkCnt, ulLinkNum;
	DbSvc_TripleId_t			*pstTripleIdArray;
	DbSvc_TsInfo_t				*pstTsInfo;


	hErr = MGR_SATRECORD_GetSatRecSiSpec (&stSiSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = SVC_SI_GetSatRecordData (pstContents->hAction, &stSiSpec, eSVCSI_SATRECORD_DATA_SES_ASTRA_RR_TRIPLEID, &ulLinkNum, (void **)&pstTripleIdArray);

	if (hErr == ERR_OK && ulLinkNum != 0)
	{
		for (ulLinkCnt = 0; ulLinkCnt < ulLinkNum; ulLinkCnt++)
		{
			hErr = xmgr_satrecord_detect_GetDeliTsInfoByOnTsId (pstContents,
															pstTripleIdArray[ulLinkCnt].usOnId,
															pstTripleIdArray[ulLinkCnt].usTsId,
															&pstTsInfo);

			if (hErr == ERR_OK)
			{
				bAstraRRTsInfoFound = TRUE;
				HxLOG_Print("[%s:%d] (%d) append (linkage type : ASTRA_RECORDING_LINKAGE) ts - onID (0x%x) tsID (0x%x) svcID (0x%x)\n"
					, __FUNCTION__, __LINE__, ulLinkCnt, pstTsInfo->usOnId, pstTsInfo->usTsId, pstTripleIdArray[ulLinkCnt].usSvcId);
				xmgr_satrecord_detect_AppendSatRecTsInfo (pstContents, pstTsInfo, pstTripleIdArray[ulLinkCnt].usSvcId, ASTRA_RECORDING_LINKAGE);
			}
		}

		SVC_SI_FreeSatRecordData (pstContents->hAction, &stSiSpec, eSVCSI_SATRECORD_DATA_SES_ASTRA_RR_TRIPLEID, (void *)pstTripleIdArray);
	}

	ulLinkNum = 0;

	hErr = SVC_SI_GetSatRecordData (pstContents->hAction, &stSiSpec, eSVCSI_SATRECORD_DATA_SES_HDPLUS_RR_TRIPLEID, &ulLinkNum, (void **)&pstTripleIdArray);
	if (hErr == ERR_OK && ulLinkNum != 0)
	{
		for (ulLinkCnt = 0; ulLinkCnt < ulLinkNum; ulLinkCnt++)
		{
			hErr = xmgr_satrecord_detect_GetDeliTsInfoByOnTsId (pstContents,
															pstTripleIdArray[ulLinkCnt].usOnId,
															pstTripleIdArray[ulLinkCnt].usTsId,
															&pstTsInfo);

			if (hErr == ERR_OK)
			{
				bHDPlusRRTsInfoFound = TRUE;
				HxLOG_Print("[%s:%d] (%d) append (linkage type : HDPLUS_RECORDING_LINKAGE) ts - onID (0x%x) tsID (0x%x) svcID (0x%x)\n"
					, __FUNCTION__, __LINE__, ulLinkCnt, pstTsInfo->usOnId, pstTsInfo->usTsId, pstTripleIdArray[ulLinkCnt].usSvcId);
				xmgr_satrecord_detect_AppendSatRecTsInfo (pstContents, pstTsInfo, pstTripleIdArray[ulLinkCnt].usSvcId, HDPLUS_RECORDING_LINKAGE);
			}
		}

		SVC_SI_FreeSatRecordData (pstContents->hAction, &stSiSpec, eSVCSI_SATRECORD_DATA_SES_HDPLUS_RR_TRIPLEID, (void *)pstTripleIdArray);
	}

	if((bAstraRRTsInfoFound == FALSE) && (bHDPlusRRTsInfoFound == FALSE))
		return ERR_FAIL;
	else
		return ERR_OK;
}

static HERROR xmgr_satrecord_detect_GetSatRecordInfoFromSi (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR			 			hErr;
	SvcSi_SatRecordSpec_t		stSiSpec;
	xmgrSatrec_DetectInfo_t		stDetectInfo;
	SatRecordInfo_t				*pstInfoArray;

	HUINT32				 		ulDataNum = 0;

	hErr = MGR_SATRECORD_GetSatRecSiSpec (&stSiSpec);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = SVC_SI_GetSatRecordData (pstContents->hAction, &stSiSpec, eSVCSI_SATRECORD_DATA_SES_SATREC_DSMCCINFO, &ulDataNum, (void **)&pstInfoArray);

	if (hErr == ERR_OK && ulDataNum != 0)
	{
		stDetectInfo.nDsmccPid = pstInfoArray[0].dsmccPid;
		hErr = xmgr_satrecord_detect_GetSatRecTsInfo (pstContents, &stDetectInfo.stDetectTsInfo, NULL);

		if(hErr == ERR_OK)
		{
			hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_SATREC_DETECTINFO, (HUINT32)&stDetectInfo, sizeof(xmgrSatrec_DetectInfo_t));
			DB_PARAM_Sync ();
		}
	}

	SVC_SI_FreeSatRecordData (pstContents->hAction, &stSiSpec, eSVCSI_SATRECORD_DATA_SES_SATREC_DSMCCINFO, (void *)pstInfoArray);
	return ERR_OK;
}


#define	_____CONTENTS_INITIATION_____

static HERROR xmgr_satrecord_detect_InitContents (xmgrSatrec_BaseDetectContents_t *pstContents, Handle_t hAction)
{
	HERROR						hErr = ERR_OK;
	xmgrSatrec_DetectInfo_t		stDetectInfo;

	// 1. Clear all the contents:
	HxSTD_memset (pstContents, 0, sizeof(xmgrSatrec_BaseDetectContents_t));

	HxSTD_memset (&stDetectInfo, 0, sizeof(xmgrSatrec_DetectInfo_t));
	stDetectInfo.nDsmccPid = -1;

	DB_PARAM_SetItem(eDB_PARAM_ITEM_SATREC_DETECTINFO, (HUINT32)&stDetectInfo, sizeof(xmgrSatrec_DetectInfo_t));
	DB_PARAM_Sync ();

	// 2. Load the arguments
	pstContents->hAction = hAction;
	pstContents->ulActionId = SVC_PIPE_GetActionId (hAction);

	hErr = xmgr_satrecord_detect_LoadVariables (pstContents);

	if (hErr != ERR_OK)
	{
		pstContents->eState = eSatRecState_detect_SearchFail;
		return ERR_OK;
	}

	pstContents->eState = eSatRecState_detect_Start;

	return ERR_OK;
}

static HERROR xmgr_satrecord_detect_LoadVariables (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR				hErr = ERR_OK;
	HUINT32				ulAntIdx;
	HUINT32				ulNetNum;
	HUINT32				ulTpCnt, ulTpNum;
	DbSvc_TsInfo_t		stTsInfo;
	DbSvc_AntInfo_t		stAntInfo;
	DbSvc_NetInfo_t		stNetInfo;

	// 1. Antenna ID : ASTRA1 을 얻어 와야 한다.
	hErr = XMGR_SEARCH_INFO_GetAstraAntIdx (&ulAntIdx);
	if (hErr != ERR_OK)					{ return ERR_FAIL; }

	pstContents->nAntId = (HINT32)ulAntIdx;
	hErr = DB_SVC_GetAntInfo ((HUINT16)ulAntIdx, &stAntInfo);
	pstContents->eSatType = (hErr == ERR_OK) ? stAntInfo.eSatType : eSatType_ASTRA_1;

	// 2. Home TP (Anchor TP)
	ulTpNum = XMGR_SEARCH_INFO_CountAnchorTsInfo();
	HxLOG_Print("[%s:%d] anchorTP num (%d)\n", __FUNCTION__, __LINE__, ulTpNum);
	if (ulTpNum > 0)
	{
		for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
		{
			hErr = XMGR_SEARCH_INFO_GetAnchorTsInfo (ulTpCnt, &stTsInfo);
			if (hErr == ERR_OK)
			{
				stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
				stTsInfo.ucTunerId = (HUINT8)-1;
				stTsInfo.stTuningParam.sat.ucAntId = (HUINT8)ulAntIdx;

				xmgr_satrecord_detect_AppendHomeTsInfo (pstContents, &stTsInfo);
			}
		}
	}

	// 3. Reference TP
	ulTpNum = XMGR_SEARCH_INFO_CountAstraReferenceTsInfo();
	HxLOG_Print("[%s:%d] refNit TP num (%d)\n", __FUNCTION__, __LINE__, ulTpNum);
	if (ulTpNum > 0)
	{
		for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
		{
			hErr = XMGR_SEARCH_INFO_GetAstraReferenceTsInfo (ulTpCnt, &stTsInfo);
			if (hErr == ERR_OK)
			{
				stTsInfo.eDeliType = eDxDELIVERY_TYPE_SAT;
				stTsInfo.ucTunerId = (HUINT8)-1;
				stTsInfo.stTuningParam.sat.ucAntId = (HUINT8)ulAntIdx;

				xmgr_satrecord_detect_AppendRefNitTsInfo (pstContents, &stTsInfo);
			}
		}
	}

	// 4. Delivery Data && Network Info
	ulNetNum = XMGR_SEARCH_INFO_CountNetworkInfo();
	ulTpNum = XMGR_SEARCH_INFO_CountDeliveryTsInfo();
	HxLOG_Print("[%s:%d] net num (%d) delivery tp num (%d)\n", __FUNCTION__, __LINE__, ulNetNum, ulTpNum);

	if (ulNetNum > 0 && ulTpNum > 0)
	{
		hErr = XMGR_SEARCH_INFO_GetNetworkInfo (0, &stNetInfo);
		if (hErr == ERR_OK)
		{
			// Get and copy the Delivery TS Info
			for (ulTpCnt = 0; ulTpCnt < ulTpNum; ulTpCnt++)
			{
				hErr = XMGR_SEARCH_INFO_GetDeliveryTsInfo (ulTpCnt, &stTsInfo);
				if (hErr == ERR_OK)
				{
					if (stTsInfo.eDeliType == eDxDELIVERY_TYPE_SAT)
					{
						xmgr_satrecord_detect_AppendDeliTsInfo (pstContents, &stTsInfo);
					}
				}
			}
		}
	}

	return ERR_OK;
}

#define	_____CONTENTS_DESTRUCTION_____

static HERROR xmgr_satrecord_detect_DestroyContents (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	xmgr_satrecord_detect_ClearHomeTsInfo(pstContents);
	xmgr_satrecord_detect_ClearDeliTsInfo(pstContents);
	xmgr_satrecord_detect_ClearRefNitTsInfo(pstContents);
	xmgr_satrecord_detect_ClearSatRecTsInfo(pstContents);
	xmgr_satrecord_detect_ClearSatRecInfo(pstContents);

	HxSTD_memset (pstContents, 0, sizeof(xmgrSatrec_BaseDetectContents_t));

	return ERR_BUS_NO_ERROR;
}

#define	_____PROCESSING_FUNCTIONS_____
static HERROR xmgr_satrecord_detect_TuneSearchTp (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR				 hErr;
	HBOOL				 bRetry;

	SvcRsc_Info_t		 stRscInfo;
	DbSvc_TsInfo_t		 stTsInfo;
	DxTuneParam_t		 stChTuneInfo;

	bRetry = TRUE;
	MGR_RSC_GetResourceInfo (pstContents->ulActionId, &stRscInfo);

	while (bRetry == TRUE)
	{
		hErr = xmgr_satrecord_detect_GetCurrentTsInfo (pstContents, &stTsInfo);

		if (hErr != ERR_OK)
		{
			hErr = xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
			bRetry = (hErr == ERR_OK) ? TRUE : FALSE;
			continue;
		}

		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			break;
		}

		xmgr_satrecord_detect_IncreaseSessionVersion (pstContents);

		OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
		hErr = SVC_CH_StartTune (pstContents->hAction, &stRscInfo, (HUINT16)-1, &stChTuneInfo);
		if (hErr == ERR_OK)
		{
			// DD에서 Tuning Signal이 올라오지 않아 무한으로 기다리는 것을 막기 위해 여기서 timer를 처리하도록 한다.
			BUS_SetTimer (SATREC_TIMER_LOCKING_ID, SATREC_TIMER_LOCKING_TIME);

			return ERR_OK;
		}

		hErr = xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
		bRetry = (hErr == ERR_OK) ? TRUE : FALSE;
	}

	// Failed :
	pstContents->eState = eSatRecState_detect_SearchFail;
	return ERR_FAIL;
}

static HERROR xmgr_satrecord_detect_StartDetectPID (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR					hErr;
	DbSvc_TsInfo_t			stTsInfo;
	DxTuneParam_t			stChTuneInfo;
	SatRec_DetectState_e	eNextState;

	HxLOG_Print("[%s:%d] Context state (%d)\n", __FUNCTION__, __LINE__, pstContents->eState);

	if (pstContents->eState != eSatRecState_detect_Start)
	{
		return ERR_FAIL;
	}

	if (xmgr_satrecord_detect_GetRefNitTsInfo (pstContents, &stTsInfo) == ERR_OK)
	{
		eNextState = eSatRecState_detect_RefNitTp_Tuning;
	}
	else if (xmgr_satrecord_detect_GetHomeTsInfo (pstContents, &stTsInfo) == ERR_OK)
	{
		eNextState = eSatRecState_detect_HomeTp_Tuning;
	}
	else
	{
		eNextState = eSatRecState_detect_SearchFail;
	}



	// Take the Action
	OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&stTsInfo, &stChTuneInfo);
	hErr = MGR_ACTION_SetupSearch (pstContents->ulActionId, &stChTuneInfo, eAmPriority_SEARCH);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("setup Search fail!\n");
		pstContents->eState = eSatRecState_detect_SearchFail;
		return ERR_FAIL;
	}


	hErr = MGR_ACTION_Take (pstContents->ulActionId, eActionType_SEARCH, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
	if (hErr != ERR_OK)
	{
		HxLOG_Error("take action fail!\n");
		pstContents->eState = eSatRecState_detect_SearchFail;
		return ERR_FAIL;
	}

	pstContents->bActionTaken = TRUE;
	pstContents->eState = eNextState;

	hErr = xmgr_satrecord_detect_TuneSearchTp (pstContents);
	if (hErr != ERR_OK)
	{
		pstContents->eState = eSatRecState_detect_SearchFail;
	}

	if (pstContents->eState == eSatRecState_detect_SearchDone ||
		pstContents->eState == eSatRecState_detect_SearchFail)
	{
		xmgr_satrecord_detect_ReleaseAction (pstContents);
		return ERR_FAIL;
	}

	return ERR_OK;
}

static BUS_Result_t xmgr_satrecord_detect_ProcessHomeTpSi (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32			 	ulNumTsInfo;

	hErr = xmgr_satrecord_detect_CountDeliveryTsInfo (pstContents, &ulNumTsInfo);

	if (hErr != ERR_OK || ulNumTsInfo == 0)
	{
		hErr = xmgr_satrecord_detect_GetDeliveryTsInfoFromSi (pstContents);
	}

	if (hErr == ERR_OK)
	{
		hErr = xmgr_satrecord_detect_GetRefNitTsInfoFromSi (pstContents);
		if (hErr != ERR_OK)
		{
			pstContents->eState = eSatRecState_detect_HomeTp_Tuning;
			hErr = xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
			if (hErr != ERR_OK)
			{
				pstContents->eState = eSatRecState_detect_SearchDone;
			}
		}
		else
		{
			pstContents->eState = eSatRecState_detect_RefNitTp_Tuning;
		}
	}
	else
	{
		pstContents->eState = eSatRecState_detect_SearchFail;
	}

	return hErr;
}

static BUS_Result_t xmgr_satrecord_detect_ProcessRefNitTpSi (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR	hErr;

	hErr = xmgr_satrecord_detect_GetSatRecordTsInfoFromSi (pstContents);
	if (hErr != ERR_OK)
	{
		pstContents->eState = eSatRecState_detect_RefNitTp_Tuning;
		hErr = xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
		if (hErr != ERR_OK)
		{
			pstContents->eState = eSatRecState_detect_SearchDone;
		}
	}
	else
	{
		pstContents->eState = eSatRecState_detect_SatRecTp_Tuning;
	}

	SVC_SI_StopSatRecord (pstContents->hAction, TRUE);
	return hErr;
}

static BUS_Result_t xmgr_satrecord_detect_ProcessSatRecTpSi (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR	hErr;

	hErr = xmgr_satrecord_detect_GetSatRecordInfoFromSi (pstContents);
	if (hErr != ERR_OK)
	{
		pstContents->eState = eSatRecState_detect_SatRecTp_Tuning;
		hErr = xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
		if (hErr != ERR_OK)
		{
			pstContents->eState = eSatRecState_detect_SearchDone;
		}
	}
	else
	{
		pstContents->eState = eSatRecState_detect_SearchDone;
	}

	HxLOG_Print("[%s:%d] pstContents->eState : %d\n", __FUNCTION__, __LINE__, pstContents->eState);
	SVC_SI_StopSatRecord (pstContents->hAction, TRUE);
	return hErr;
}

static HERROR xmgr_satrecord_detect_ReleaseAction (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	HERROR		 hErr;

	if (pstContents->bActionTaken == TRUE)
	{
		hErr = SVC_CH_StopTune (pstContents->hAction, (HUINT16)-1, eSyncMode);
		if (hErr != ERR_OK)
		{
			// SesLcnActAssert (0);
		}

		// Release the action :
		MGR_ACTION_Release (pstContents->ulActionId, eActionType_SEARCH);
		pstContents->bActionTaken = FALSE;
	}

	return ERR_OK;
}


#define	_____INTERNAL_MSG_PROCESS_FUNCTION_____

static BUS_Result_t xmgr_satrecord_detect_MsgBusCreate_Base (xmgrSatrec_BaseDetectContents_t *pstContents, Handle_t hAction)
{
	HERROR hErr = ERR_OK;

	if (pstContents == NULL)
	{
		HxLOG_Error ("Context NULL!!!\n");
		return MESSAGE_PASS;
	}

	// 1. Initialize the contents
	hErr = xmgr_satrecord_detect_InitContents (pstContents, hAction);

	//2. Start Detect PID
	if(pstContents->eState == eSatRecState_detect_Start)
	{
		hErr = xmgr_satrecord_detect_StartDetectPID(pstContents);
		if(hErr != ERR_OK)
		{
			BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SATRECORD_NOTIFY_DETECT_FINISH, (Handle_t)NULL, 0, 0, 0);
		}
	}
	else
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SATRECORD_NOTIFY_DETECT_FINISH, (Handle_t)NULL, 0, 0, 0);
	}

	return MESSAGE_BREAK;
}

static BUS_Result_t xmgr_satrecord_detect_MsgBusDestroy_Base (xmgrSatrec_BaseDetectContents_t *pstContents)
{
	if (pstContents->eState == eSatRecState_detect_HomeTp_WatingSi ||
		pstContents->eState == eSatRecState_detect_RefNitTp_WatingSi ||
		pstContents->eState == eSatRecState_detect_SatRecTp_WatingSi)
	{
		SVC_SI_StopSatRecord (pstContents->hAction, TRUE);
	}

	xmgr_satrecord_detect_ReleaseAction(pstContents);

	// Destroy the contents
	xmgr_satrecord_detect_DestroyContents (pstContents);

	return MESSAGE_BREAK;
}

STATIC HERROR xmgr_satrecord_detect_MsgGwmTimer_Ses (xmgrSatrec_BaseDetectContents_t *pstContents, HINT32 nTimerId)
{
	HERROR				 hErr, hRetValue;

	hRetValue = ERR_BUS_NO_ERROR;
	switch (nTimerId)
	{
		case SATREC_TIMER_LOCKING_ID:
			BUS_KillTimer (nTimerId);
			hRetValue = MESSAGE_BREAK;
			break;

		case SATREC_TIMER_SI_ID:
			BUS_KillTimer (nTimerId);

			if (pstContents->eState == eSatRecState_detect_HomeTp_WatingSi ||
				pstContents->eState == eSatRecState_detect_RefNitTp_WatingSi ||
				pstContents->eState == eSatRecState_detect_SatRecTp_WatingSi)
			{
				// MW SI를 Stop시키고 다음 TP로 넘어가도록 한다.
				hErr = SVC_SI_StopSatRecord (pstContents->hAction, TRUE);
				if (hErr == ERR_OK)
				{
					switch (pstContents->eState)
					{
						case eSatRecState_detect_HomeTp_WatingSi:
							pstContents->eState = eSatRecState_detect_HomeTp_Tuning;
							break;

						case eSatRecState_detect_RefNitTp_WatingSi:
							pstContents->eState = eSatRecState_detect_RefNitTp_Tuning;
							break;

						case eSatRecState_detect_SatRecTp_WatingSi:
							pstContents->eState = eSatRecState_detect_SatRecTp_Tuning;
							break;

						default:
							pstContents->eState = eSatRecState_detect_SearchFail;
							break;
					}

				}
			}

			hRetValue = MESSAGE_BREAK;
			break;

		default:
			break;
	}

	if (pstContents->eState == eSatRecState_detect_HomeTp_Tuning ||
		pstContents->eState == eSatRecState_detect_RefNitTp_Tuning ||
		pstContents->eState == eSatRecState_detect_SatRecTp_Tuning)
	{
		// 다음 TP 로 넘어가야 한다.
		hErr = xmgr_satrecord_detect_SetToNextTsInfo (pstContents);

		if (hErr == ERR_OK)
		{
			hErr = xmgr_satrecord_detect_TuneSearchTp (pstContents);
			if (hErr != ERR_OK)
			{
				// SesLcnActAssert (0);
				pstContents->eState = eSatRecState_detect_SearchFail;
			}
		}
		else
		{
			pstContents->eState = eSatRecState_detect_SearchDone;
		}
	}

	if (pstContents->eState == eSatRecState_detect_SearchDone||
		pstContents->eState == eSatRecState_detect_SearchFail)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SATRECORD_NOTIFY_DETECT_FINISH, (Handle_t)NULL, 0, 0, 0);
	}

	return hRetValue;
}

static BUS_Result_t xmgr_satrecord_detect_EvtChLocked_Base (xmgrSatrec_BaseDetectContents_t *pstContents, Handle_t hAction)
{
	HERROR					 	hErr;

	HUINT16 					usSvcId;
	HINT32					 	nArg;

	SatRec_DetectState_e		eNextState;
	SvcSi_SatRecordType_e		eSatRecMode;
	SvcSi_SatRecordOption_t		stSatRecOption;
	SvcSi_SatRecordSpec_t		stSiSpec;

	hErr = xmgr_satrecord_detect_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK)
	{
		return MESSAGE_PASS;
	}

	HxLOG_Print("[%s:%d] eState : %d\n", __FUNCTION__, __LINE__, pstContents->eState);

	switch (pstContents->eState)
	{
		case eSatRecState_detect_HomeTp_Tuning:
			eNextState = eSatRecState_detect_HomeTp_WatingSi;
			eSatRecMode = eSVCSI_SATRECORDTYPE_SES_HOME_TP;
			nArg = 0;
			break;

		case eSatRecState_detect_RefNitTp_Tuning:
			eNextState = eSatRecState_detect_RefNitTp_WatingSi;
			eSatRecMode = eSVCSI_SATRECORDTYPE_SES_REFNIT_TP;
			nArg = 0;
			break;

		case eSatRecState_detect_SatRecTp_Tuning:
			xmgr_satrecord_detect_GetSatRecTsInfo (pstContents, NULL, &usSvcId);
			eNextState = eSatRecState_detect_SatRecTp_WatingSi;
			eSatRecMode = eSVCSI_SATRECORDTYPE_SES_SATREC_TP;
			nArg = (HINT32)usSvcId;
			break;

		default:
			return ERR_FAIL;
	}

	// Channel Locked : Kill the timer
	BUS_KillTimer (SATREC_TIMER_LOCKING_ID);

	HxSTD_MemSet(&stSatRecOption, 0x0, sizeof(SvcSi_SatRecordOption_t));
	stSatRecOption.ulSatRecordMode	= (HUINT32)eSatRecMode;
	stSatRecOption.usExtId1		= 0;		//change to bouquet id
	stSatRecOption.usExtId2		= 0;
	stSatRecOption.usExtId3		= nArg;

	hErr = MGR_SATRECORD_GetSatRecSiSpec (&stSiSpec);
	if(hErr != ERR_FAIL)
	{
		hErr = SVC_SI_StartSatRecord(pstContents->hAction,
											&stSiSpec,
											&stSatRecOption,
											TRUE);
		if (hErr == ERR_OK)
		{
			pstContents->eState = eNextState;
			BUS_SetTimer (SATREC_TIMER_SI_ID, SATREC_TIMER_SI_TIME);
			return MESSAGE_BREAK;
		}
	}

	hErr = xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
	if (hErr != ERR_OK)
	{
		pstContents->eState = eSatRecState_detect_SearchDone;
	}

	if (pstContents->eState == eSatRecState_detect_SearchDone||
		pstContents->eState == eSatRecState_detect_SearchFail)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SATRECORD_NOTIFY_DETECT_FINISH, (Handle_t)NULL, 0, 0, 0);
	}

	return ERR_BUS_NO_ERROR;
}

// eSEVT_CH_NO_SIGNAL:
static HERROR xmgr_satrecord_detect_EvtChNoSignal (xmgrSatrec_BaseDetectContents_t *pstContents, Handle_t hAction)
{
	HERROR					 	hErr;

	hErr = xmgr_satrecord_detect_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK)
	{
		return MESSAGE_PASS;
	}

	switch (pstContents->eState)
	{
		case eSatRecState_detect_HomeTp_Tuning:
		case eSatRecState_detect_RefNitTp_Tuning:
		case eSatRecState_detect_SatRecTp_Tuning:
			// Channel Locked : Kill the timer
			BUS_KillTimer (SATREC_TIMER_LOCKING_ID);

			hErr = xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_satrecord_detect_TuneSearchTp (pstContents);
				if (hErr != ERR_OK)
				{
					// SesLcnActAssert (0);
					pstContents->eState = eSatRecState_detect_SearchFail;
				}
			}
			else
			{
				pstContents->eState = eSatRecState_detect_SearchDone;
			}

			break;
		case eSatRecState_detect_HomeTp_WatingSi:
		case eSatRecState_detect_RefNitTp_WatingSi:
		case eSatRecState_detect_SatRecTp_WatingSi:
			hErr = SVC_SI_StopSatRecord (hAction, TRUE);

			hErr = xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
			if (hErr == ERR_OK)
			{
				hErr = xmgr_satrecord_detect_TuneSearchTp (pstContents);
				if (hErr != ERR_OK)
				{
					// SesLcnActAssert (0);
					pstContents->eState = eSatRecState_detect_SearchFail;
				}
			}
			else
			{
				pstContents->eState = eSatRecState_detect_SearchDone;
			}
			break;
		default :
			pstContents->eState = eSatRecState_detect_SearchFail;
			break;
	}

	if (pstContents->eState == eSatRecState_detect_SearchDone||
		pstContents->eState == eSatRecState_detect_SearchFail)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SATRECORD_NOTIFY_DETECT_FINISH, (Handle_t)NULL, 0, 0, 0);
	}

	return MESSAGE_BREAK;
}

// eSEVT_SI_SEARCH_DONE:
static BUS_Result_t xmgr_satrecord_detect_EvtSiSearchDone (xmgrSatrec_BaseDetectContents_t *pstContents, Handle_t hAction)
{
	HERROR					 	hErr;

	hErr = xmgr_satrecord_detect_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK)				{ return MESSAGE_BREAK; }

	BUS_KillTimer (SATREC_TIMER_SI_ID);

	switch (pstContents->eState)
	{
		case eSatRecState_detect_HomeTp_WatingSi:
			hErr = xmgr_satrecord_detect_ProcessHomeTpSi(pstContents);
			break;

		case eSatRecState_detect_RefNitTp_WatingSi:
			hErr = xmgr_satrecord_detect_ProcessRefNitTpSi(pstContents);
			break;

		case eSatRecState_detect_SatRecTp_WatingSi:
			hErr = xmgr_satrecord_detect_ProcessSatRecTpSi(pstContents);
			break;

		default:
			pstContents->eState = eSatRecState_detect_SearchFail;
			break;
	}

	switch (pstContents->eState)
	{
		case eSatRecState_detect_HomeTp_Tuning:
		case eSatRecState_detect_RefNitTp_Tuning:
		case eSatRecState_detect_SatRecTp_Tuning:
			hErr = xmgr_satrecord_detect_TuneSearchTp (pstContents);
			if (hErr != ERR_OK)
			{
				pstContents->eState = eSatRecState_detect_SearchFail;
			}
			break;

		default :
			break;
	}

	if (pstContents->eState == eSatRecState_detect_SearchDone||
		pstContents->eState == eSatRecState_detect_SearchFail)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SATRECORD_NOTIFY_DETECT_FINISH, (Handle_t)NULL, 0, 0, 0);
	}

	return MESSAGE_BREAK;
}

// eSEVT_SI_SEARCH_FAIL:
static HERROR xmgr_satrecord_detect_EvtSiSearchFail (xmgrSatrec_BaseDetectContents_t *pstContents, Handle_t hAction)
{
	HERROR			 hErr;

	hErr = xmgr_satrecord_detect_CheckActionId (pstContents, hAction);
	if (hErr != ERR_OK) 			{ return MESSAGE_BREAK; }

	HxLOG_Print("[%s:%d] eState : %d\n", __FUNCTION__, __LINE__, pstContents->eState);

	BUS_KillTimer (SATREC_TIMER_SI_ID);

	switch (pstContents->eState)
	{
		case eSatRecState_detect_HomeTp_WatingSi:
			hErr = SVC_SI_StopSatRecord (hAction, TRUE);
			pstContents->eState = eSatRecState_detect_HomeTp_Tuning;
			break;

		case eSatRecState_detect_RefNitTp_WatingSi:
			hErr = SVC_SI_StopSatRecord (hAction, TRUE);
			pstContents->eState = eSatRecState_detect_RefNitTp_Tuning;
			break;

		case eSatRecState_detect_SatRecTp_WatingSi:
			hErr = SVC_SI_StopSatRecord (hAction, TRUE);
			pstContents->eState = eSatRecState_detect_SatRecTp_Tuning;
			break;

		default:
			pstContents->eState = eSatRecState_detect_SearchFail;
			break;
	}

	hErr = xmgr_satrecord_detect_SetToNextTsInfo (pstContents);
	HxLOG_Print("[%s:%d] eState : %d\n", __FUNCTION__, __LINE__, pstContents->eState);

	if (hErr == ERR_OK)
	{
		hErr = xmgr_satrecord_detect_TuneSearchTp (pstContents);
		if (hErr != ERR_OK)
		{
			// SesLcnActAssert (0);
			pstContents->eState = eSatRecState_detect_SearchFail;
		}
	}
	else
	{
		pstContents->eState = eSatRecState_detect_SearchDone;
	}

	if (pstContents->eState == eSatRecState_detect_SearchDone||
		pstContents->eState == eSatRecState_detect_SearchFail)
	{
		BUS_PostMessage((BUS_Callback_t)NULL, eMEVT_SATRECORD_NOTIFY_DETECT_FINISH, (Handle_t)NULL, 0, 0, 0);
	}

	return MESSAGE_BREAK;
}

BUS_Result_t xproc_satrecord_DetectPID_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	eResult = MESSAGE_PASS;

	xmgrSatrec_BaseDetectContents_t 	*pstContents = xmgr_satrecord_detect_GetContents_Base();

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print ("[%s:%d] eMEVT_BUS_CREATE (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, hAction, p1, p2, p3);
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "XMGR_SATRECORD_BASE_Proc");
			eResult = xmgr_satrecord_detect_MsgBusCreate_Base (pstContents, hAction);
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Print ("[%s:%d] (%d) eMEVT_BUS_DESTROY (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_detect_MsgBusDestroy_Base(pstContents);
			break;

		case eMEVT_BUS_TIMER:
			HxLOG_Print("\t[MSG_GWM_TIMER] 0x%08x, (%08x:%08x:%08x)\n\n", hAction, p1, p2, p3);
			eResult = xmgr_satrecord_detect_MsgGwmTimer_Ses (pstContents, p1);
			break;

		case eSEVT_CH_LOCKED :
			HxLOG_Print ("[%s:%d] (%d) eSEVT_CH_LOCKED (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_detect_EvtChLocked_Base(pstContents, hAction);
			break;

		case eSEVT_CH_NO_SIGNAL:
			HxLOG_Print ("[%s:%d] (%d) eSEVT_CH_NO_SIGNAL (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_detect_EvtChNoSignal(pstContents, hAction);
			break;

		case eSEVT_SI_SEARCH_DONE:
			HxLOG_Print ("[%s:%d] (%d) eSEVT_SI_SEARCH_DONE (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_detect_EvtSiSearchDone(pstContents, hAction);
			break;

		case eSEVT_SI_SEARCH_FAIL:
			HxLOG_Print ("[%s:%d] (%d) eSEVT_SI_SEARCH_FAIL (0x%08x:%d,%d,%d)\n", __FUNCTION__, __LINE__, pstContents->ulActionId, hAction, p1, p2, p3);
			eResult = xmgr_satrecord_detect_EvtSiSearchFail(pstContents, hAction);
			break;
		default:
			break;
	}

	if (eResult == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

/* End Of File */
