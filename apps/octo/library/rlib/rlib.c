/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  rlib.c
	@brief

	Description:  									\n
	Module: Library/RLIB			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#include <hlib.h>
#include <rlib.h>

#define RxMAX_RESOURCE_NUM					32
#define	hCmdHandle							"RLIB"

#define REQUEST_CONTAINS_DUBBED_INST

#define ___INTERNAL_STRUCTURES___
/*
	Resource Object Instance 관리 Structures:
*/
/* Resource Object Instance를 관리하기 위한 Table. */
typedef struct
{
	HBOOL					 bInited;
	RxRscType_e				 eRscType;					/**< 이 Resource의 Type */
	HUINT32					 ulRscNum;					/**< 이 Resource 개수... */
	HUINT32					 ulPriorResourceId;			/**< Tuner Group에서 사용 빈 Resource 를 찾을 때 우선되는 ResourceId 를 적는다. Default value 는 0이다. */
	RxRscObject_t			 astRscObj[RxMAX_RESOURCE_NUM];
} _rxRscTable_t;

/*
	Resource Map 관리 Structures:
*/
typedef struct
{
	HUINT32					 ulActionId;							/**< Action Handle */
	HUINT32 				 aulResourceId[RxMAX_RESOURCE_NUM];		/**< Resource Id */
} _rxRscMap_t;

/*
	Resource Trial 용 Structure
*/
typedef struct
{
	HBOOL				 bEnabled;
	HUINT32				 ulActionId;
	HUINT32 			 ulPriority;
} _rxActionInfo_t;

typedef struct
{
	HBOOL				 bRequested;
	HUINT32				 ulResourceId;
	HBOOL				 bSharable;
	RxUsageType_e		 eUsage;
} _rxRscReqItem_t;

typedef struct
{
	RxRscReqHandle_t	 hReqHandle;

	HUINT32				 ulActionId;
	HUINT32				 ulActionPriority;

	HUINT32				 ulNumRemovableAction;
	_rxActionInfo_t		*pstRemovableActions;

	HUINT32				 ulNumIgnorableAction;
	_rxActionInfo_t		*pstIgnorableActions;

	_rxRscReqItem_t		 astRscReqItem[eRxRSCTYPE_MAX];

	HxList_t			*pstArgList;			// RxTrialArg_t

	RxRscTrialResult_t	*pstTrialResult;

	// Source & Duplicated Inst:
	void				*pvDuplicatedInst;			// _rxInst_t
	void				*pvOriginalInst;			// _rxInst_t
} _rxRscReq_t;

typedef struct
{
	HUINT32			 ulNumAction;
	HUINT32			*pulActIdArray;
} _rxKillActItem_t;

typedef struct
{
	HCHAR				 szInstName[RxINST_NAME_LEN];

	HUINT32				 ulActionNum;
	_rxRscTable_t		*pstRscTable;
	_rxRscMap_t			*pstRscMaps;

	// Custom Session Table/Map
	_rxRscTable_t		*pstCustomRscTable;
	_rxRscMap_t			*pstCustomRscMaps;

	// Request Handles:
	HxList_t			*pstRscReqList;				// _rxRscReq_t
} _rxInst_t;

#define ___INTERNAL_VARIABLES___
//		For Multi-Instance			//
STATIC HxList_t				*s_pstRxRscInstList = NULL;				// _rxInst_t

#define ___PRIVATE_FUNCTION___
STATIC HERROR rlib_InitDebugCmd (void);

STATIC Handle_t rxlib_CreateInstance (HCHAR *szInstName)
{
	_rxInst_t		*pstInst;

#if defined(CONFIG_DEBUG)
	rlib_InitDebugCmd();
#endif

	pstInst = (_rxInst_t *)HLIB_STD_MemCalloc(sizeof(_rxInst_t));
	if (NULL == pstInst)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto ERROR;
	}

	pstInst->pstRscTable = (_rxRscTable_t *)HLIB_STD_MemCalloc(sizeof(_rxRscTable_t) * eRxRSCTYPE_MAX);
	pstInst->pstCustomRscTable = (_rxRscTable_t *)HLIB_STD_MemCalloc(sizeof(_rxRscTable_t) * eRxRSCTYPE_MAX);
	if ((NULL == pstInst->pstRscTable) || (NULL == pstInst->pstCustomRscTable))
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto ERROR;
	}

	if ((NULL == szInstName) || ('\0' == szInstName[0]))
	{
		HUINT32			 ulCnt = HLIB_LIST_Length(s_pstRxRscInstList);

		HxSTD_PrintToStrN(pstInst->szInstName, RxINST_NAME_LEN, "UNKNOWN_%d", ulCnt);
	}
	else
	{
		HLIB_STD_StrNCpySafe(pstInst->szInstName, szInstName, RxINST_NAME_LEN);
	}

	s_pstRxRscInstList = HLIB_LIST_Append(s_pstRxRscInstList, (void *)pstInst);

	return (Handle_t)pstInst;

ERROR:
	if (NULL != pstInst)
	{
		if (NULL != pstInst->pstRscTable)				{ HLIB_STD_MemFree(pstInst->pstRscTable); }
		if (NULL != pstInst->pstCustomRscTable)			{ HLIB_STD_MemFree(pstInst->pstCustomRscTable); }

		HLIB_STD_MemFree(pstInst);
	}

	return RxHANDLE_NULL;
}

STATIC void rxlib_FreeInstance (_rxInst_t *pstInst)
{
	if (NULL != pstInst)
	{
		if (NULL != pstInst->pstRscTable)				{ HLIB_STD_MemFree(pstInst->pstRscTable); }
		if (NULL != pstInst->pstRscMaps)				{ HLIB_STD_MemFree(pstInst->pstRscMaps); }
		if (NULL != pstInst->pstCustomRscTable)			{ HLIB_STD_MemFree(pstInst->pstCustomRscTable); }
		if (NULL != pstInst->pstCustomRscMaps)			{ HLIB_STD_MemFree(pstInst->pstCustomRscMaps); }

		HLIB_STD_MemFree(pstInst);
	}
}

STATIC _rxInst_t *rxlib_DuplicateInstance (_rxInst_t *pstSrcInst)
{
	_rxInst_t		*pstDstInst = NULL;

	pstDstInst = (_rxInst_t *)HLIB_STD_MemCalloc(sizeof(_rxInst_t));
	if (NULL == pstDstInst)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto ERROR;
	}

	pstDstInst->ulActionNum = pstSrcInst->ulActionNum;
	pstDstInst->pstRscTable = (_rxRscTable_t *)HLIB_STD_MemDup(pstSrcInst->pstRscTable, sizeof(_rxRscTable_t) * eRxRSCTYPE_MAX);
	pstDstInst->pstRscMaps = (_rxRscMap_t *)HLIB_STD_MemDup(pstSrcInst->pstRscMaps, sizeof(_rxRscMap_t) * pstSrcInst->ulActionNum);
	pstDstInst->pstCustomRscTable = (_rxRscTable_t *)HLIB_STD_MemDup(pstSrcInst->pstCustomRscTable, sizeof(_rxRscTable_t) * eRxRSCTYPE_MAX);
	pstDstInst->pstCustomRscMaps = (_rxRscMap_t *)HLIB_STD_MemDup(pstSrcInst->pstCustomRscMaps, sizeof(_rxRscMap_t) * pstSrcInst->ulActionNum);
	if ((NULL == pstDstInst->pstRscTable) || (NULL == pstDstInst->pstRscMaps) ||
		(NULL == pstDstInst->pstCustomRscTable) || (NULL == pstDstInst->pstCustomRscMaps))
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto ERROR;
	}

	return pstDstInst;

ERROR:
	rxlib_FreeInstance(pstDstInst);
	return NULL;
}


INLINE STATIC _rxRscTable_t *rxlib_GetResourceObjectTable (_rxInst_t *pstInst, RxRscType_e eRscType)
{
	return (eRscType < eRxRSCTYPE_MAX) ? &(pstInst->pstRscTable[eRscType]) : NULL;
}

INLINE STATIC HUINT32 rxlib_GetResourceNumber (_rxInst_t *pstInst, RxRscType_e eRscType)
{
	return (eRscType < eRxRSCTYPE_MAX) ? pstInst->pstRscTable[eRscType].ulRscNum : 0;
}

STATIC RxRscObject_t *rxlib_GetResourceObject (_rxInst_t *pstInst, RxRscType_e eRscType, HUINT32 ulRscId)
{
	_rxRscTable_t	*pstRscTable = rxlib_GetResourceObjectTable(pstInst, eRscType);

	if (NULL != pstRscTable)
	{
		if (ulRscId < pstRscTable->ulRscNum)
		{
			return &(pstRscTable->astRscObj[ulRscId]);
		}
	}

	return NULL;
}

#define _____TRIAL_RESULT_INSTANCE_____
STATIC RxRscTrialResult_t *rxlib_AllocTrialResult (_rxInst_t *pstInst)
{
	HUINT32				*pulActionsToBeKilled = NULL;
	RxRscTrialResult_t	*pstRscTrial = NULL;

	pulActionsToBeKilled = (HUINT32 *)HLIB_STD_MemCalloc(sizeof(HUINT32) * pstInst->ulActionNum);
	pstRscTrial = (RxRscTrialResult_t *)HLIB_STD_MemCalloc(sizeof(RxRscTrialResult_t));

	if ((NULL == pulActionsToBeKilled) || (NULL == pstRscTrial))
	{
		goto ERROR;
	}

	pstRscTrial->pulActionsToBeKilled	= pulActionsToBeKilled;
	return pstRscTrial;

ERROR:
	if (NULL != pulActionsToBeKilled)				{ HLIB_STD_MemFree(pulActionsToBeKilled); }
	if (NULL != pstRscTrial)						{ HLIB_STD_MemFree(pstRscTrial); }

	return NULL;
}

STATIC void rxlib_FreeTrialResult (RxRscTrialResult_t *pstResult)
{
	if (NULL != pstResult)
	{
		if (NULL != pstResult->pulActionsToBeKilled)
		{
			HLIB_STD_MemFree(pstResult->pulActionsToBeKilled);
		}

		HLIB_STD_MemFree(pstResult);
	}
}

#define _____RESOURCE_REQUEST_INSTANCE_____
STATIC void rxlib_FreeRscReq (_rxRscReq_t *pstRscReq)
{
	if (NULL != pstRscReq)
	{
		if (NULL != pstRscReq->pstRemovableActions)
		{
			HLIB_STD_MemFree(pstRscReq->pstRemovableActions);
		}

		if (NULL != pstRscReq->pstIgnorableActions)
		{
			HLIB_STD_MemFree(pstRscReq->pstIgnorableActions);
		}

		if (NULL != pstRscReq->pstTrialResult)
		{
			rxlib_FreeTrialResult(pstRscReq->pstTrialResult);
		}

		if (NULL != pstRscReq->pstArgList)
		{
			pstRscReq->pstArgList = HLIB_LIST_RemoveAllFunc(pstRscReq->pstArgList, HLIB_MEM_Free_CB);
		}

		if (NULL != pstRscReq->pvDuplicatedInst)
		{
			rxlib_FreeInstance((_rxInst_t *)pstRscReq->pvDuplicatedInst);
		}

		HLIB_STD_MemFree(pstRscReq);
	}
}

STATIC void rxlib_cbFreeRscReq (void *pvData)
{
	_rxRscReq_t		*pstRscReq = (_rxRscReq_t *)pvData;
	rxlib_FreeRscReq(pstRscReq);
}


STATIC _rxRscReq_t *rxlib_AllocRscReq (_rxInst_t *pstInst)
{
	_rxRscReq_t			*pstRscReq = NULL;

	pstRscReq = (_rxRscReq_t *)HLIB_STD_MemCalloc(sizeof(_rxRscReq_t));
	if (NULL == pstRscReq)					{ goto ERROR; }

	pstRscReq->pstRemovableActions = (_rxActionInfo_t *)HLIB_STD_MemCalloc(sizeof(_rxActionInfo_t) * pstInst->ulActionNum);
	if (NULL == pstRscReq->pstRemovableActions)
	{
		goto ERROR;
	}

	pstRscReq->pstIgnorableActions = (_rxActionInfo_t *)HLIB_STD_MemCalloc(sizeof(_rxActionInfo_t) * pstInst->ulActionNum);
	if (NULL == pstRscReq->pstIgnorableActions)
	{
		goto ERROR;
	}

	pstRscReq->pvDuplicatedInst = (void *)rxlib_DuplicateInstance(pstInst);
	if (NULL == pstRscReq->pvDuplicatedInst)
	{
		goto ERROR;
	}

	pstRscReq->pvOriginalInst = (void *)pstInst;

	return pstRscReq;

ERROR:
	if (NULL != pstRscReq)
	{
		rxlib_FreeRscReq(pstRscReq);
	}

	return NULL;
}

STATIC HERROR rxlib_GetInstAndRscReqFromReqHandle (RxRscReqHandle_t hReqHandle, _rxInst_t **ppstInst, _rxRscReq_t **ppstReq)
{
	HxList_t		*pstInstItem, *pstReqItem;

	for (pstInstItem = s_pstRxRscInstList; NULL != pstInstItem; pstInstItem = pstInstItem->next)
	{
		_rxInst_t		*pstInst = (_rxInst_t *)HLIB_LIST_Data(pstInstItem);

		if (NULL != pstInst)
		{
			for (pstReqItem = pstInst->pstRscReqList; NULL != pstReqItem; pstReqItem = pstReqItem->next)
			{
				_rxRscReq_t 		*pstRscReq = (_rxRscReq_t *)HLIB_LIST_Data(pstReqItem);

				if ((NULL != pstRscReq) && (hReqHandle == pstRscReq->hReqHandle))
				{
					*ppstInst = pstInst;
					*ppstReq = pstRscReq;
					return ERR_OK;
				}
			}
		}
	}

	*ppstInst = NULL;
	*ppstReq = NULL;
	return ERR_FAIL;
}

#define _____TRIAL_ARGUMENT_____
STATIC RxTrialArg_t *rxlib_GetArgumentByArgType (_rxRscReq_t *pstRscReq, RxTrialArgType_e eArgType)
{
	HxList_t			*pstListItem;

	for (pstListItem = pstRscReq->pstArgList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		RxTrialArg_t			*pstArg = (RxTrialArg_t *)HLIB_LIST_Data(pstListItem);

		if ((NULL != pstArg) && (eArgType == pstArg->eArgType))
		{
			return pstArg;
		}
	}

	return NULL;
}

#define _____RESOURCE_TABLE_MAP_FUNCTION_____
INLINE STATIC void rxlib_SetResourceIdToResourceMap (_rxInst_t *pstInst, HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 ulResourceId)
{
	pstInst->pstRscMaps[ulActionId].aulResourceId[eRscType] = ulResourceId;
}

INLINE STATIC HUINT32 rxlib_GetResourceIdFromResourceMap (_rxInst_t *pstInst, HUINT32 ulActionId, RxRscType_e eRscType)
{
	return pstInst->pstRscMaps[ulActionId].aulResourceId[eRscType];
}

STATIC HERROR rxlib_SetShareObjectToResourceMap (_rxInst_t *pstInst, RxRscType_e eRscType, HUINT32 ulRscId, HBOOL bSharable, RxTrialArg_t *punArg)
{
	_rxRscTable_t		*pstRscTable = NULL;

	pstRscTable = rxlib_GetResourceObjectTable(pstInst, eRscType);
	if (NULL == pstRscTable)
	{
		return ERR_FAIL;
	}

	if (ulRscId >= pstRscTable->ulRscNum)
	{
		return ERR_FAIL;
	}

	pstRscTable->astRscObj[ulRscId].bSharable = bSharable;

	switch (eRscType)
	{
	case eRxRSCTYPE_TUNER_GROUP:
		if (NULL != punArg)
		{
			RxTunerObject_t		*pstTunerObj = &(pstRscTable->astRscObj[ulRscId].utObj.stTuner);
			HxSTD_MemCopy((&pstTunerObj->stTuneParam), &(punArg->stTuneParam.stTuneParam), sizeof(DxTuneParam_t));
		}
		else
		{
			return ERR_FAIL;
		}

		break;

	case eRxRSCTYPE_RFINPUT:
		if (NULL != punArg)
		{
			RxRfInputObject_t		*pstRfInputObj = &(pstRscTable->astRscObj[ulRscId].utObj.stRfInput);

			if (eDxDELIVERY_TYPE_SAT == pstRfInputObj->eSupportedDeliType)
			{
				RxSatRfInputObj_t		*pstSatRf = &(pstRfInputObj->utObj.stSatRf);
				HxSTD_MemCopy(&(pstSatRf->stMasterTuneParam), &(punArg->stTuneParam.stTuneParam), sizeof(DxTuneParam_t));
			}
		}
		else
		{
			return ERR_FAIL;
		}

		break;

	default:
		break;
	}

	return ERR_OK;
}

STATIC void rxlib_UpdateRefCountInResourceMap (_rxInst_t *pstInst, RxRscType_e eRscType)
{
	HUINT32				 ulRscCnt, ulActCnt;
	_rxRscTable_t		*pstRscTable = rxlib_GetResourceObjectTable(pstInst, eRscType);

	if ((NULL != pstRscTable) && (eRscType < eRxRSCTYPE_MAX))
	{
		for (ulRscCnt = 0; ulRscCnt < pstRscTable->ulRscNum; ulRscCnt++)
		{
			RxRscObject_t 		*pstRscObj = rxlib_GetResourceObject(pstInst, eRscType, ulRscCnt);

			if (NULL != pstRscObj)
			{
				pstRscObj->nRefCount = 0;

				for (ulActCnt = 0; ulActCnt < pstInst->ulActionNum; ulActCnt++)
				{
					if (rxlib_GetResourceIdFromResourceMap(pstInst, ulActCnt, eRscType) == ulRscCnt)
					{
						pstRscObj->nRefCount++;
					}
				}
			}
		}
	}
}

#define _____CUSTOM_TABLE_MAP_FUNCTION_____
INLINE STATIC void rxlib_SetResourceIdToCustomResourceMap (_rxInst_t *pstInst, HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 ulResourceId)
{
	pstInst->pstCustomRscMaps[ulActionId].aulResourceId[eRscType] = ulResourceId;
}

INLINE STATIC _rxRscTable_t *rxlib_GetCustomResourceObjectTable (_rxInst_t *pstInst, RxRscType_e eRscType)
{
	return (eRscType < eRxRSCTYPE_MAX) ? &(pstInst->pstCustomRscTable[eRscType]) : NULL;
}

INLINE STATIC HUINT32 rxlib_GetResourceIdFromCustomResourceMap (_rxInst_t *pstInst, HUINT32 ulActionId, RxRscType_e eRscType)
{
	return pstInst->pstCustomRscMaps[ulActionId].aulResourceId[eRscType];
}

STATIC RxRscObject_t *rxlib_GetCustomResourceObject (_rxInst_t *pstInst, RxRscType_e eRscType, HUINT32 ulResourceId)
{
	_rxRscTable_t		*pstCustomRscTable = NULL;

	pstCustomRscTable = rxlib_GetCustomResourceObjectTable(pstInst, eRscType);
	if (NULL == pstCustomRscTable)
	{
		return NULL;
	}

	if (ulResourceId >= pstCustomRscTable->ulRscNum)
	{
		return NULL;
	}

	return &pstCustomRscTable->astRscObj[ulResourceId];
}

STATIC void rxlib_UpdateRefCountInCustomResourceMap (_rxInst_t *pstInst, RxRscType_e eRscType)
{
	HUINT32				 ulRscCnt, ulActCnt;
	_rxRscTable_t		*pstCustomRscTable = rxlib_GetCustomResourceObjectTable(pstInst, eRscType);

	if ((NULL != pstCustomRscTable) && (eRscType < eRxRSCTYPE_MAX))
	{
		for (ulRscCnt = 0; ulRscCnt < pstCustomRscTable->ulRscNum; ulRscCnt++)
		{
			RxRscObject_t 		*pstCustomRscObj = rxlib_GetCustomResourceObject(pstInst, eRscType, ulRscCnt);

			if (NULL != pstCustomRscObj)
			{
				pstCustomRscObj->nRefCount = 0;

				for (ulActCnt = 0; ulActCnt < pstInst->ulActionNum; ulActCnt++)
				{
					if (rxlib_GetResourceIdFromCustomResourceMap(pstInst, ulActCnt, eRscType) == ulRscCnt)
					{
						pstCustomRscObj->nRefCount++;
					}
				}
			}
		}
	}
}

STATIC HERROR rxlib_InitCustomResourceMap (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq)
{
	HUINT32					 ulActCnt, ulRscCnt;

	// 1. Copy the current resource object table and map
	HxSTD_MemCopy(pstInst->pstCustomRscTable, pstInst->pstRscTable, sizeof(_rxRscTable_t) * eRxRSCTYPE_MAX);
	HxSTD_MemCopy(pstInst->pstCustomRscMaps, pstInst->pstRscMaps, sizeof(_rxRscMap_t) * pstInst->ulActionNum);

	// 2. if the action ID is available, then remove the current action's resources and re-calculate the resource maps:
	if (pstRscReq->ulActionId < pstInst->ulActionNum)
	{
		for (ulRscCnt = 0; ulRscCnt < eRxRSCTYPE_MAX; ulRscCnt++)
		{
			rxlib_SetResourceIdToCustomResourceMap(pstInst, pstRscReq->ulActionId, ulRscCnt, RxRSCID_NULL);
		}
	}

	// 3. Ignorable Action:
	for (ulActCnt = 0; ulActCnt < pstInst->ulActionNum; ulActCnt++)
	{
		_rxActionInfo_t *pstActInfo;

		pstActInfo = &(pstRscReq->pstIgnorableActions[ulActCnt]);
		if (TRUE == pstActInfo->bEnabled)
		{
			for (ulRscCnt = 0; ulRscCnt < eRxRSCTYPE_MAX; ulRscCnt++)
			{
				rxlib_SetResourceIdToCustomResourceMap(pstInst, ulActCnt, ulRscCnt, RxRSCID_NULL);
			}
		}
	}

	for (ulRscCnt = 0; ulRscCnt < eRxRSCTYPE_MAX; ulRscCnt++)
	{
		rxlib_UpdateRefCountInCustomResourceMap(pstInst, ulRscCnt);
	}

	return ERR_OK;
}

#define _____TRIAL_RFINPUT_TUNER_FUNCTION_____
STATIC HBOOL rxlib_CheckAntennaTuningParamSame (DxAntInfo_t *pstAnt1, DxAntInfo_t *pstAnt2)
{
	if (pstAnt1->antType != pstAnt2->antType)
	{
		return FALSE;
	}

	switch (pstAnt1->antType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		{
			DxLnbAntenna_t			*pstLnb1 = &(pstAnt1->info.lnb);
			DxLnbAntenna_t			*pstLnb2 = &(pstAnt2->info.lnb);

			if ((pstLnb1->ulLnbFreq != pstLnb2->ulLnbFreq) ||
				(pstLnb1->lnbVoltage != pstLnb2->lnbVoltage))
			{
				return FALSE;
			}
		}

		break;

	case eDxANT_TYPE_DISEQC:
		{
			DxDiseqcAntenna_t		*pstDiseqc1 = &(pstAnt1->info.diseqc);
			DxDiseqcAntenna_t		*pstDiseqc2 = &(pstAnt2->info.diseqc);

			if ((pstDiseqc1->ulLnbFreq != pstDiseqc2->ulLnbFreq) ||
				(pstDiseqc1->lnbVoltage != pstDiseqc2->lnbVoltage))
			{
				return FALSE;
			}

			if ((pstDiseqc1->ulLnbFreq != DxSAT_UNIVERSAL_LNB_FREQ) &&
				(pstDiseqc1->b22kTone != pstDiseqc2->b22kTone))
			{
				return FALSE;
			}

			if ((pstDiseqc1->diseqcVersion != pstDiseqc2->diseqcVersion) ||
				(pstDiseqc1->toneBurst != pstDiseqc2->toneBurst))
			{
				return FALSE;
			}

			switch (pstDiseqc1->diseqcVersion)
			{
			case eDxDISEQC_VER_1_2:
				if (pstDiseqc1->ucMotorPosition != pstDiseqc2->ucMotorPosition)
				{
					return FALSE;
				}

				break;

			case eDxDISEQC_VER_1_3:
				if (pstDiseqc1->sSatLonggitude != pstDiseqc2->sSatLonggitude)
				{
					return FALSE;
				}

				break;

			case eDxDISEQC_VER_1_0:
			case eDxDISEQC_VER_2_0:
			default:
				if (pstDiseqc1->diseqcInput != pstDiseqc2->diseqcInput)
				{
					return FALSE;
				}

				break;
			}
		}

		break;

	case eDxANT_TYPE_SCD:
		{
			DxScdAntenna_t			*pstScd1 = &(pstAnt1->info.scd);
			DxScdAntenna_t			*pstScd2 = &(pstAnt2->info.scd);

			if ((pstScd1->ulLnbFreq != pstScd2->ulLnbFreq) ||
				(pstScd1->lnbVoltage != pstScd2->lnbVoltage) ||
				(pstScd1->b22kTone != pstScd2->b22kTone))
			{
				return FALSE;
			}

			// except. scdInput
			if ((pstScd1->ulScdUserBandNum != pstScd2->ulScdUserBandNum) ||
				(pstScd1->ulScdUserBandFreq != pstScd2->ulScdUserBandFreq) ||
				(pstScd1->ulScdUserBandNum1 != pstScd2->ulScdUserBandNum1) ||
				(pstScd1->ulScdUserBandFreq1 != pstScd2->ulScdUserBandFreq1) ||
				(pstScd1->ulScdUserBandNum2 != pstScd2->ulScdUserBandNum2) ||
				(pstScd1->ulScdUserBandFreq2 != pstScd2->ulScdUserBandFreq2) ||
				(pstScd1->ulScdUserBandNum3 != pstScd2->ulScdUserBandNum3) ||
				(pstScd1->ulScdUserBandFreq3 != pstScd2->ulScdUserBandFreq3))
			{
				return FALSE;
			}
		}

		break;

	default:
		return FALSE;
	}

	return TRUE;
}

STATIC HUINT32 rxlib_GetActualLnbFreq (HUINT32 ulTpFreq)
{
	if (abs((HINT32)(ulTpFreq - 9750)) < 1950) /* MHz */
	{
		return 9750;
	}
	else
	{
		return 10600;
	}
}

STATIC HBOOL rxlib_GetActual22kTone (HUINT32 ulTpFreq)
{
	if (abs((HINT32)(ulTpFreq - 9750)) < 1950) /* MHz */
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

STATIC HBOOL rxlib_CheckRfInputSatAcceptable (DxTuneParam_t *pstRfTune, DxTuneParam_t *pstTpTune)
{
	DxAntInfo_t					*pstRfAntInfo = &(pstRfTune->sat.antInfo);
	DxAntInfo_t					*pstTpAntInfo = &(pstTpTune->sat.antInfo);
	DxSat_TuningTPInfo_t		*pstRfTuneInfo = &(pstRfTune->sat.tuningInfo);
	DxSat_TuningTPInfo_t		*pstTpTuneInfo = &(pstTpTune->sat.tuningInfo);

	if (rxlib_CheckAntennaTuningParamSame(pstRfAntInfo, pstTpAntInfo) != TRUE)
	{
		return FALSE;
	}

	// SCD는 Antenna 정보만 겹치지 않으면 RF-Input 단에서 충돌할 일이 없으므로 막는다.
	if (eDxANT_TYPE_SCD != pstRfAntInfo->antType)
	{
		if (pstRfTuneInfo->ePolarization != pstTpTuneInfo->ePolarization)
		{
			return FALSE;
		}

		if ((eDxANT_TYPE_DISEQC == pstRfAntInfo->antType) &&
			(DxSAT_UNIVERSAL_LNB_FREQ == pstRfAntInfo->info.diseqc.ulLnbFreq))
		{
			HUINT32 		 ulRfLnbFreq	= rxlib_GetActualLnbFreq(pstRfTuneInfo->ulFrequency);
			HUINT32 		 ulTpLnbFreq	= rxlib_GetActualLnbFreq(pstTpTuneInfo->ulFrequency);
			HBOOL			 bRf22kTone 	= rxlib_GetActual22kTone(pstRfTuneInfo->ulFrequency);
			HBOOL			 bTp22kTone 	= rxlib_GetActual22kTone(pstTpTuneInfo->ulFrequency);

			if ((ulRfLnbFreq != ulTpLnbFreq) ||
				(bRf22kTone != bTp22kTone))
			{
				return FALSE;
			}
		}
		else if ((eDxANT_TYPE_LNB_ONLY == pstRfAntInfo->antType) &&
			(DxSAT_UNIVERSAL_LNB_FREQ == pstRfAntInfo->info.lnb.ulLnbFreq))
		{
			HUINT32 		 ulRfLnbFreq	= rxlib_GetActualLnbFreq(pstRfTuneInfo->ulFrequency);
			HUINT32 		 ulTpLnbFreq	= rxlib_GetActualLnbFreq(pstTpTuneInfo->ulFrequency);
			HBOOL			 bRf22kTone 	= rxlib_GetActual22kTone(pstRfTuneInfo->ulFrequency);
			HBOOL			 bTp22kTone 	= rxlib_GetActual22kTone(pstTpTuneInfo->ulFrequency);

			if ((ulRfLnbFreq != ulTpLnbFreq) ||
				(bRf22kTone != bTp22kTone))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

STATIC HERROR rxlib_CheckRfInputIdAvailable (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq, HUINT32 ulRfInputId)
{
	RxRscObject_t			*pstRfInputObj;

	pstRfInputObj	= rxlib_GetCustomResourceObject(pstInst, eRxRSCTYPE_RFINPUT, ulRfInputId);

	if (NULL == pstRfInputObj)
	{
		return ERR_FAIL;
	}

	// 1. Satellite 향이고 현재 막혀 있으면 안된다.
	if ((eDxDELIVERY_TYPE_SAT & pstRfInputObj->utObj.stRfInput.eSupportedDeliType) &&
		(TRUE != pstRfInputObj->utObj.stRfInput.utObj.stSatRf.bAvailable))
	{
		return ERR_FAIL;
	}

	// 1. 아무도 사용하고 있지 않은 RF Input: 문제 없음.
	if (pstRfInputObj->nRefCount <= 0)					// 아무도 사용하고 있지 않다
	{
		return ERR_OK;
	}

	// 2. 위성향이 아닌 RF-Input: 역시 충돌 문제 없다.
	if (!(eDxDELIVERY_TYPE_SAT & pstRfInputObj->utObj.stRfInput.eSupportedDeliType))
	{
		return ERR_OK;
	}

	// 3. TP Conflict가 나는가 확인 :
	{
		RxSatRfInputObj_t	*pstSatRf = &(pstRfInputObj->utObj.stRfInput.utObj.stSatRf);
		RxTrialArg_t		*punArg = rxlib_GetArgumentByArgType(pstRscReq, eRxARGTYPE_TUNEPARAM);

		if ((NULL != punArg) &&
			(rxlib_CheckRfInputSatAcceptable(&(pstSatRf->stMasterTuneParam), &(punArg->stTuneParam.stTuneParam)) == TRUE))
		{
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HBOOL rxlib_CheckSatTunerAcceptable (DxSatTuningInfo_t *pstTune1, DxSatTuningInfo_t *pstTune2)
{
	HINT32						 nFreqDiff;
	DxAntInfo_t					*pstAnt1 = &(pstTune1->antInfo);
	DxAntInfo_t					*pstAnt2 = &(pstTune2->antInfo);
	DxSat_TuningTPInfo_t		*pstTpInfo1 = &(pstTune1->tuningInfo);
	DxSat_TuningTPInfo_t		*pstTpInfo2 = &(pstTune2->tuningInfo);

	if (rxlib_CheckAntennaTuningParamSame(pstAnt1, pstAnt2) != TRUE)
	{
		return FALSE;
	}

	if ((pstTpInfo1->ulSymbolRate != pstTpInfo2->ulSymbolRate) ||
		(pstTpInfo1->eTransSpec != pstTpInfo2->eTransSpec) )

/*
 * author : hsseo
 * note : rollOff 값이 DB, 값 convert 중에 어긋나는 부분이 존재. 항상 FALSE
 	      추후 삭제 필요
 */
#if 0
		(pstTpInfo1->rollOff != pstTpInfo2->rollOff))
#endif
	{
		return FALSE;
	}

	if ((eDxSAT_CODERATE_AUTO != pstTpInfo1->eFecCodeRate) && (eDxSAT_CODERATE_AUTO != pstTpInfo2->eFecCodeRate))
	{
		if (pstTpInfo1->eFecCodeRate != pstTpInfo2->eFecCodeRate)
		{
			return FALSE;
		}
	}

	if ((eDxSAT_POLAR_AUTO != pstTpInfo1->ePolarization) && (eDxSAT_POLAR_AUTO != pstTpInfo2->ePolarization))
	{
		if (pstTpInfo1->ePolarization != pstTpInfo2->ePolarization)
		{
			return FALSE;
		}
	}

	if ((eDxSAT_PSK_AUTO != pstTpInfo1->ePskMod) && (eDxSAT_PSK_AUTO != pstTpInfo2->ePskMod))
	{
		if (pstTpInfo1->ePskMod != pstTpInfo2->ePskMod)
		{
			return FALSE;
		}
	}

	if ((eDxSAT_PILOT_AUTO != pstTpInfo1->ePilot) && (eDxSAT_PILOT_AUTO != pstTpInfo2->ePilot))
	{
		if (pstTpInfo1->ePilot != pstTpInfo2->ePilot)
		{
			return FALSE;
		}
	}

	//////////////// Frequency 비교 ////////////////
	//		45~18Ms/s	=	+/- (8MHz) search
	//		18 ~ 11Ms/s =	+/- (6MHz) search
	//		11 ~ 8Ms/s	=	+/- (5MHz) search
	//		8 ~ 4Ms/s	=	+/- (4MHz) search
	//		4 ~ 2Ms/s	=	+/- (3.8MHz) search
	//	=> 2003.09.09, 권기호 과장으로부터 입수
	nFreqDiff = (HINT32)pstTpInfo1->ulFrequency - (HINT32)pstTpInfo2->ulFrequency;

	if (pstTpInfo1->ulSymbolRate >= 18000)
	{
		if ((nFreqDiff >= 8) || (nFreqDiff <= -8))
		{
			return FALSE;
		}
	}
	else if (pstTpInfo1->ulSymbolRate >= 11000)
	{
		if ((nFreqDiff >= 6) || (nFreqDiff <= -6))
		{
			return FALSE;
		}
	}
	else if (pstTpInfo1->ulSymbolRate >= 8000)
	{
		if ((nFreqDiff >= 5) || (nFreqDiff <= -5))
		{
			return FALSE;
		}
	}
	else if (pstTpInfo1->ulSymbolRate >= 2000)
	{
		if ((nFreqDiff >= 4) || (nFreqDiff <= -4))
		{
			return FALSE;
		}
	}
	else
	{
		if (nFreqDiff != 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

STATIC HBOOL rxlib_CheckCabTunerAcceptable (DxCabTuningInfo_t *pstTune1, DxCabTuningInfo_t *pstTune2)
{
	HINT32						 nFreqDiff;

	if (pstTune1->ulSymbolRate != pstTune2->ulSymbolRate)
	{
		return FALSE;
	}

	if ((eDxCAB_CONSTELLATION_AUTO != pstTune1->constellation) && (eDxCAB_CONSTELLATION_AUTO != pstTune2->constellation))
	{
		if (pstTune1->constellation != pstTune2->constellation)
		{
			return FALSE;
		}
	}

	if ((eDxCAB_SPECTRUM_AUTO != pstTune1->spectrum) && (eDxCAB_SPECTRUM_AUTO != pstTune2->spectrum))
	{
		if (pstTune1->spectrum != pstTune2->spectrum)
		{
			return FALSE;
		}
	}

	nFreqDiff = (HINT32)pstTune1->ulFrequency - (HINT32)pstTune2->ulFrequency;
	if ((nFreqDiff > 32) || (nFreqDiff < -32))
	{
		return FALSE;
	}

	return TRUE;
}

STATIC HBOOL rxlib_CheckTerTunerAcceptable (DxTerTuningInfo_t *pstTune1, DxTerTuningInfo_t *pstTune2)
{
	HINT32						 nFreqDiff;

	if ((eDxTER_TRANSMODE_AUTO != pstTune1->eTransMode) && (eDxTER_TRANSMODE_AUTO != pstTune2->eTransMode))
	{
		if (pstTune1->eTransMode != pstTune2->eTransMode)
		{
			return FALSE;
		}
	}

	if ((eDxTER_GUARDINTERVAL_AUTO != pstTune1->eGuardInterval) && (eDxTER_GUARDINTERVAL_AUTO != pstTune2->eGuardInterval))
	{
		if (pstTune1->eGuardInterval != pstTune2->eGuardInterval)
		{
			return FALSE;
		}
	}

	if ((eDxTER_DVB_T2 == pstTune1->eSystem) && (0x0000FFFF != pstTune1->stT2Param.ulPlpId) &&
		(eDxTER_DVB_T2 == pstTune2->eSystem) && (0x0000FFFF != pstTune2->stT2Param.ulPlpId))
	{
		if (pstTune1->stT2Param.ulPlpId != pstTune2->stT2Param.ulPlpId)
		{
			return FALSE;
		}
	}

	nFreqDiff = (HINT32)pstTune1->ulFrequency - (HINT32)pstTune2->ulFrequency;
	if ((nFreqDiff > 3000) || (nFreqDiff < -3000))
	{
		return FALSE;
	}

	return TRUE;
}

STATIC HBOOL rxlib_CheckTunerAcceptable (DxTuneParam_t *pstRfTune, DxTuneParam_t *pstTpTune)
{
	switch (pstTpTune->eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		return rxlib_CheckSatTunerAcceptable(&(pstRfTune->sat), &(pstTpTune->sat));

	case eDxDELIVERY_TYPE_CAB:
		return rxlib_CheckCabTunerAcceptable(&(pstRfTune->cab), &(pstTpTune->cab));

	case eDxDELIVERY_TYPE_TER:
		return rxlib_CheckTerTunerAcceptable(&(pstRfTune->ter), &(pstTpTune->ter));

	default:
		break;
	}

	return FALSE;
}

STATIC HBOOL rxlib_IsTunerAccessableToRfInput (_rxInst_t *pstInst, HUINT32 ulRfInputId, HUINT32 ulTunerGroupId)
{
	RxRscObject_t		*pstRfInputRsc, *pstTunerRsc;

	pstRfInputRsc	= rxlib_GetResourceObject(pstInst, eRxRSCTYPE_RFINPUT, ulRfInputId);
	pstTunerRsc		= rxlib_GetResourceObject(pstInst, eRxRSCTYPE_TUNER_GROUP, ulTunerGroupId);
	if ((NULL == pstRfInputRsc) || (NULL == pstTunerRsc))
	{
		return FALSE;
	}

	if (pstTunerRsc->utObj.stTuner.eSupportedDeliType != pstRfInputRsc->utObj.stRfInput.eSupportedDeliType)
	{
		return FALSE;
	}

	if (eDxDELIVERY_TYPE_SAT == pstTunerRsc->utObj.stTuner.eSupportedDeliType)
	{
		HUINT32						 ulCount;
		RxSatRfInputObj_t			*pstSatRfInput = &(pstRfInputRsc->utObj.stRfInput.utObj.stSatRf);

		for (ulCount = 0; ulCount < pstSatRfInput->ulConnectableTunerNum; ulCount++)
		{
			if (pstSatRfInput->aulConnectableTunerId[ulCount] == ulTunerGroupId)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	return TRUE;
}

STATIC HERROR rxlib_CheckTunerIdAvailable (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq, HUINT32 ulTunerGroupId, HUINT32 ulRfInputId)
{
	_rxRscReqItem_t			*pstReqTuner = &(pstRscReq->astRscReqItem[eRxRSCTYPE_TUNER_GROUP]);
	RxRscObject_t			*pstTunerObj = rxlib_GetCustomResourceObject(pstInst, eRxRSCTYPE_TUNER_GROUP, ulTunerGroupId);
	RxTrialArg_t			*punArg = rxlib_GetArgumentByArgType(pstRscReq, eRxARGTYPE_TUNEPARAM);

	if ((NULL == pstTunerObj) || (NULL == punArg))
	{
		return ERR_FAIL;
	}

	// 1. 해당 튜너가 해당 RF-Input과 연결 가능한지 확인
	if (rxlib_IsTunerAccessableToRfInput(pstInst, ulRfInputId, ulTunerGroupId) != TRUE)
	{
		HxLOG_Error("RF-Input(%d) and Tuner(%d) cannot be connectable\n", ulRfInputId, ulTunerGroupId);
		return ERR_FAIL;
	}

	// 2. 아무도 사용하고 있지 않은 Tuner: 문제 없음.
	if (pstTunerObj->nRefCount <= 0)					// 아무도 사용하고 있지 않다
	{
		return ERR_OK;
	}

	// 3. Sharing 환경이 맞는가
	if ((TRUE == pstReqTuner->bSharable) &&
		(TRUE == pstTunerObj->bSharable) &&
		(rxlib_CheckTunerAcceptable(&(pstTunerObj->utObj.stTuner.stTuneParam), &(punArg->stTuneParam.stTuneParam)) == TRUE))
	{
		return ERR_OK;
	}

	HxLOG_Info("Tuner(%d) not sharable or tuning conflict\n", ulTunerGroupId);
	return ERR_FAIL;
}


STATIC HERROR rxlib_FindRfInputAndTunerAvailable (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq,
												HUINT32 ulNumRfInputId, HUINT32 *pulRfInputIdArray,
												HUINT32 ulNumTunerGroupId, HUINT32 *pulTunerGroupIdArray,
												HUINT32 *pulFoundRfInputId, HUINT32 *pulFoundTunerGroupId)
{
	HUINT32					 ulRfInputCnt, ulTunerCnt;
	HUINT32					 ulRfInputId, ulTunerGroupId;
	HUINT32					 ulFoundNum = 0;
	HUINT32					 aulFoundRfInputId[RxMAX_RESOURCE_NUM], aulFoundTunerGroupId[RxMAX_RESOURCE_NUM];
	HERROR					 hErr;

	for (ulRfInputCnt = 0; ulRfInputCnt < ulNumRfInputId; ulRfInputCnt++)
	{
		ulRfInputId = pulRfInputIdArray[ulRfInputCnt];

		hErr = rxlib_CheckRfInputIdAvailable(pstInst, pstRscReq, ulRfInputId);
		if (ERR_OK == hErr)
		{
			for (ulTunerCnt = 0; ulTunerCnt < ulNumTunerGroupId; ulTunerCnt++)
			{
				ulTunerGroupId = pulTunerGroupIdArray[ulTunerCnt];

				hErr = rxlib_CheckTunerIdAvailable(pstInst, pstRscReq, ulTunerGroupId, ulRfInputId);
				if (ERR_OK == hErr)
				{
					if (ulFoundNum < RxMAX_RESOURCE_NUM)
					{
						aulFoundRfInputId[ulFoundNum] = ulRfInputId;
						aulFoundTunerGroupId[ulFoundNum] = ulTunerGroupId;
						ulFoundNum ++;
					}
				}
			}
		}
	}

	if (0 == ulFoundNum)
	{
		return ERR_FAIL;
	}
	else if (ulFoundNum >= 2)
	{
		HBOOL				 bSameTunerId = TRUE;
		HUINT32				 ulCnt;

		for (ulCnt = 1; ulCnt < ulFoundNum; ulCnt++)
		{
			if (aulFoundTunerGroupId[0] != aulFoundTunerGroupId[ulCnt])
			{
				bSameTunerId = FALSE;
				break;
			}
		}

		if (TRUE == bSameTunerId)
		{
			HUINT32				 ulCurrIndex = 0xFFFF;
			HUINT32				 ulCurrTunerNum = 0xFFFF;

			// 쓸 수 있는 Tuner가 다 동일한데 RF-Input을 다수 선택할 수 있는 상황
			// Connectable Tuner 개수가 많은 RF-Input은, 역으로 그 RF-Input 로 연결할 수 밖에 없는 Tuner가 많다는 뜻과 동일하다.
			// 그러한 RF-Input의 사용은 가급적 자제하는 방향으로 가야 한다.
			// 즉, 여기서는 역으로 Connectable Tuner가 적은 RF-Input 순서대로 쓰는 것이 바람직하다.
			for (ulCnt = 0; ulCnt < ulFoundNum; ulCnt++)
			{
				RxRscObject_t 	*pstRfInputObj = rxlib_GetCustomResourceObject(pstInst, eRxRSCTYPE_RFINPUT, aulFoundRfInputId[ulCnt]);

				if ((NULL != pstRfInputObj) && (eDxDELIVERY_TYPE_SAT == pstRfInputObj->utObj.stRfInput.eSupportedDeliType))
				{
					RxSatRfInputObj_t		*pstSatRf = &(pstRfInputObj->utObj.stRfInput.utObj.stSatRf);

					if (pstSatRf->ulConnectableTunerNum < ulCurrTunerNum)
					{
						ulCurrIndex = ulCnt;
						ulCurrTunerNum = pstSatRf->ulConnectableTunerNum;
					}
				}
			}

			if (( ulCurrIndex < ulFoundNum ) && ( ulCurrIndex < RxMAX_RESOURCE_NUM ))
			{
				*pulFoundRfInputId = aulFoundRfInputId[ulCurrIndex];
				*pulFoundTunerGroupId = aulFoundTunerGroupId[ulCurrIndex];
				return ERR_OK;
			}
		}
	}

	*pulFoundRfInputId = aulFoundRfInputId[0];
	*pulFoundTunerGroupId = aulFoundTunerGroupId[0];
	return ERR_OK;
}

#define _____TRIAL_DEMUX_FUNCTION_____
STATIC HERROR rxlib_CheckSharableDemuxIdAvailable (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq, RxRscTrialResult_t *pstTrialResult, HUINT32 ulDemuxId)
{
	HBOOL					 bDemuxFound = FALSE;
	HUINT32					 ulActCnt;
	HUINT32					 ulAvDecId = RxRSCID_NULL;
	HUINT32					 ulAllocatedAvDecId = RxRSCID_NULL;
	HUINT32					 ulTunerGroupId = RxRSCID_NULL;
	RxRscType_e				 eRscType;
	RxRscObject_t			*pstDemuxObj = rxlib_GetCustomResourceObject(pstInst, eRxRSCTYPE_DEMUX, ulDemuxId);
	RxRscObject_t			*pstTunerObj = NULL;
	RxRscAlloc_t			*pstTrialRsc = NULL;

	if (NULL == pstDemuxObj)
	{
		return ERR_FAIL;
	}

	// Do not share the demux in this function
	if (pstDemuxObj->nRefCount <= 0)
	{
		return ERR_FAIL;
	}

	// Same Demux usage type
	if (eRxUSAGETYPE_DEMUX_LIVEREC != pstDemuxObj->utObj.stTemplate.eUsageType)
	{
		return ERR_FAIL;
	}

	// Check the Tuner Requested
	pstTrialRsc = &(pstTrialResult->astRscAllocTable[eRxRSCTYPE_TUNER_GROUP]);
	if (TRUE == pstTrialRsc->bRequest)
	{
		ulTunerGroupId = pstTrialRsc->ulResourceId;
	}
	else
	{
		return ERR_FAIL;
	}

	pstTunerObj = rxlib_GetCustomResourceObject(pstInst, eRxRSCTYPE_TUNER_GROUP, ulTunerGroupId);
	if ((NULL == pstTunerObj) || (TRUE != pstTunerObj->bSharable) || (pstTunerObj->nRefCount <= 0))
	{
		return ERR_FAIL;
	}

	// Check the A/V Decoder Requested
	pstTrialRsc = &(pstTrialResult->astRscAllocTable[eRxRSCTYPE_AV_DEC]);
	if (TRUE == pstTrialRsc->bRequest)
	{
		ulAvDecId = pstTrialRsc->ulResourceId;
	}

	// Check the other requested resources : If you request the other resources, then the demux cannot be shared
	for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
	{
		switch (eRscType)
		{
		case eRxRSCTYPE_TUNER_GROUP:
		case eRxRSCTYPE_DEMUX:
		case eRxRSCTYPE_RFINPUT:
		case eRxRSCTYPE_AV_DEC:
			break;

		// The DEMUX connecting the other resources (except the upper four resources) cannot be shared at all:
		default:
			pstTrialRsc = &(pstTrialResult->astRscAllocTable[eRscType]);
			if (TRUE == pstTrialRsc->bRequest)
			{
				return ERR_FAIL;
			}

			break;
		}
	}


	for (ulActCnt = 0; ulActCnt < pstInst->ulActionNum; ulActCnt++)
	{
		HUINT32					 ulCheckTunerId = rxlib_GetResourceIdFromCustomResourceMap(pstInst, ulActCnt, eRxRSCTYPE_TUNER_GROUP);
		HUINT32					 ulCheckDemuxId = rxlib_GetResourceIdFromCustomResourceMap(pstInst, ulActCnt, eRxRSCTYPE_DEMUX);
		HUINT32					 ulCheckAvDecId = rxlib_GetResourceIdFromCustomResourceMap(pstInst, ulActCnt, eRxRSCTYPE_AV_DEC);

		if ((RxRSCID_NULL == ulCheckTunerId) || (ulCheckTunerId != ulTunerGroupId) ||
			(RxRSCID_NULL == ulCheckDemuxId) || (ulCheckDemuxId != ulDemuxId))
		{
			continue;
		}

		for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
		{
			switch (eRscType)
			{
			case eRxRSCTYPE_RFINPUT:
			case eRxRSCTYPE_TUNER_GROUP:
			case eRxRSCTYPE_DEMUX:
			case eRxRSCTYPE_AV_DEC:
				break;

			default:
				// The DEMUX connecting the other resources (except the upper four resources) cannot be shared at all:
				if (rxlib_GetResourceIdFromCustomResourceMap(pstInst, ulActCnt, eRscType) != RxRSCID_NULL)
				{
					return ERR_FAIL;
				}

				break;
			}
		}

		bDemuxFound = TRUE;
		if (RxRSCID_NULL != ulCheckAvDecId)
		{
			if (RxRSCID_NULL == ulAllocatedAvDecId)
			{
				ulAllocatedAvDecId = ulCheckAvDecId;
			}
			else
			{
				return ERR_FAIL;
			}
		}
	}

	if (TRUE != bDemuxFound)
	{
		return ERR_FAIL;
	}

	if ((RxRSCID_NULL != ulAllocatedAvDecId) && (RxRSCID_NULL != ulAvDecId))
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR rxlib_CheckNonSharableDemuxIdAvailable (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq, HUINT32 ulDemuxId)
{
	RxUsageType_e			 eDemuxUsage;
	_rxRscReqItem_t			*pstReqDemux = &(pstRscReq->astRscReqItem[eRxRSCTYPE_DEMUX]);
	RxRscObject_t			*pstDemuxObj = rxlib_GetCustomResourceObject(pstInst, eRxRSCTYPE_DEMUX, ulDemuxId);

	if (NULL == pstDemuxObj)
	{
		return ERR_FAIL;
	}

	eDemuxUsage = pstReqDemux->eUsage;

	// Do not share the demux in this function
	if (pstDemuxObj->nRefCount > 0)
	{
		return ERR_FAIL;
	}

	// Demux 사용 목적이 같도록 한다.
	if (pstDemuxObj->utObj.stTemplate.eUsageType != eDemuxUsage)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____TRIAL_AVDEC_FUNCTION_____
STATIC HERROR rxlib_CheckAvDecIdAvailable (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq, HUINT32 ulAvDecId)
{
	_rxRscReqItem_t 		*pstReqAvDec = &(pstRscReq->astRscReqItem[eRxRSCTYPE_AV_DEC]);
	RxRscObject_t 			*pstAvDecObj = rxlib_GetCustomResourceObject(pstInst, eRxRSCTYPE_AV_DEC, ulAvDecId);

	if (NULL == pstAvDecObj)
	{
		return ERR_FAIL;
	}

	// A/V DEC은 Action간 공유할 수 있는 것이 아니다.
	if (pstAvDecObj->nRefCount > 0)
	{
		return ERR_FAIL;
	}

	// A/V DEC 사용 목적이 같도록 한다.
	if (pstAvDecObj->utObj.stTemplate.eUsageType != pstReqAvDec->eUsage)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____TRIAL_HDDREC_FUNCTION_____
STATIC HERROR rxlib_CheckHddRecIdAvailable (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq, HUINT32 ulHddRecId)
{
	_rxRscReqItem_t 		*pstReqHddRec = &(pstRscReq->astRscReqItem[eRxRSCTYPE_HDD_REC]);
	RxRscObject_t 			*pstHddRecObj = rxlib_GetCustomResourceObject(pstInst, eRxRSCTYPE_HDD_REC, ulHddRecId);

	if (NULL == pstHddRecObj)
	{
		return ERR_FAIL;
	}

	// HDDREC 은 Action간 공유할 수 있는 것이 아니다.
	if (pstHddRecObj->nRefCount > 0)
	{
		return ERR_FAIL;
	}

	// HDDREC 사용 목적이 같도록 한다.
	if (pstHddRecObj->utObj.stTemplate.eUsageType != pstReqHddRec->eUsage)
	{
		return ERR_FAIL;
	}


	return ERR_OK;
}

#define _____TRIAL_GENERAL_RESOURCE_FUNCTION_____
STATIC HERROR rxlib_CheckExclusionRscIdAvailable (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq, RxRscType_e eRscType, HUINT32 ulRscId)
{
	RxRscObject_t 			*pstRscObj;

	if (eRscType >= eRxRSCTYPE_MAX)
	{
		return ERR_FAIL;
	}

	pstRscObj = rxlib_GetCustomResourceObject(pstInst, eRscType, ulRscId);

	if (NULL == pstRscObj)
	{
		return ERR_FAIL;
	}

	if (pstRscObj->nRefCount > 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define _____TRIAL_FUNCTION_____
STATIC _rxKillActItem_t *rxlib_getKillAction (_rxInst_t *pstInst, HUINT32 ulNumKillAction, HUINT32 *aulKillActionId, HUINT32 ulCaseCnt)
{
	HUINT32					 ulNumAct = 0;
	HUINT32					 ulCount, ulBitPattern = ulCaseCnt;
	_rxKillActItem_t		*pstKillAction;

	pstKillAction = (_rxKillActItem_t *)HLIB_STD_MemCalloc(sizeof(_rxKillActItem_t));
	if (NULL == pstKillAction)					{ return NULL; }

	pstKillAction->pulActIdArray = (HUINT32 *)HLIB_STD_MemCalloc(sizeof(HUINT32) * pstInst->ulActionNum);
	if (NULL == pstKillAction->pulActIdArray)
	{
		HLIB_STD_MemFree(pstKillAction);
		return NULL;
	}

	for (ulCount = 0; ulCount < ulNumKillAction; ulCount++, ulBitPattern = ulBitPattern >> 1)
	{
		if (0 != (ulBitPattern & 0x0001))
		{
			pstKillAction->pulActIdArray[ulNumAct++] = aulKillActionId[ulCount];
		}
	}

	pstKillAction->ulNumAction = ulNumAct;
	return pstKillAction;
}

STATIC void rxlib_FreeKillAction (void *pvKillItem)
{
	_rxKillActItem_t		*pstKillItem = (_rxKillActItem_t *)pvKillItem;

	if (NULL != pstKillItem)
	{
		if (NULL != pstKillItem->pulActIdArray)
		{
			HLIB_STD_MemFree(pstKillItem->pulActIdArray);
		}

		HLIB_STD_MemFree(pstKillItem);
	}
}

STATIC HINT32 rxlib_CompareNumActionInKillActItem (void *pvData, void *pvListItem)
{
	_rxKillActItem_t			*pstData = (_rxKillActItem_t *)pvData;
	_rxKillActItem_t			*pstListItem = (_rxKillActItem_t *)pvListItem;

	if ((NULL != pstData) && (NULL != pstListItem))
	{
		return (HINT32)pstListItem->ulNumAction - (HINT32)pstData->ulNumAction;
	}

	return 0;
}

STATIC HxList_t *rxlib_GetKillActionList (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq)
{
	HUINT32					 ulCnt;
	HUINT32					 ulNumKillAction = 0, ulNumCaseKills = 1;
	HUINT32					*pulKillActIdArray = NULL;
	HxList_t				*pstKillActList = NULL;

	pulKillActIdArray = (HUINT32 *)HLIB_STD_MemAlloc (sizeof(HUINT32) * pstInst->ulActionNum);
	if (NULL == pulKillActIdArray)			{ goto END_FUNC; }

	for (ulCnt = 0; ulCnt < pstInst->ulActionNum; ulCnt++)
	{
		_rxActionInfo_t		*pstAction = &(pstRscReq->pstRemovableActions[ulCnt]);

		HxLOG_Debug("pstAction->bEnabled: %d, pstAction->ulPriority: %d, pstRscReq->ulActionPriority: %d \n", pstAction->bEnabled, pstAction->ulPriority,	pstRscReq->ulActionPriority);
		if ((TRUE == pstAction->bEnabled) && ((pstAction->ulPriority) <= (pstRscReq->ulActionPriority)))
		{
			pulKillActIdArray[ulNumKillAction++] = ulCnt;
		}
	}

	// Kill Action이 두개 이상 일때, Priority가 낮은 것 부터 처리 되도록 한다.
	if(2 <= ulNumKillAction)
	{
		// Sort the killable action as the priority:
		for (ulCnt = 0; ulCnt < ulNumKillAction-1; ulCnt++)
		{
			HUINT32					 ulSubCnt;
			_rxActionInfo_t			*pstAction = &(pstRscReq->pstRemovableActions[pulKillActIdArray[ulCnt]]);

			for (ulSubCnt = ulCnt+1; ulSubCnt<ulNumKillAction; ulSubCnt++)
			{
				_rxActionInfo_t 	*pstSubAction = &(pstRscReq->pstRemovableActions[pulKillActIdArray[ulSubCnt]]);

				if (pstAction->ulPriority > pstSubAction->ulPriority)
				{
					HUINT32			 ulTempActId;

					ulTempActId = pulKillActIdArray[ulCnt];
					pulKillActIdArray[ulCnt] = pulKillActIdArray[ulSubCnt];
					pulKillActIdArray[ulSubCnt] = ulTempActId;
					
					pstAction = &(pstRscReq->pstRemovableActions[pulKillActIdArray[ulCnt]]);
				}
			}
		}
	}

	// 죽여도 되는 Action의 조합 갯수 : 죽어도 되는 Action의 2^n = << 계산
	ulNumCaseKills = (0x00000001) << ulNumKillAction;

	HxLOG_Debug("ulNumKillAction: %d, ulNumCaseKills: %d \n", ulNumKillAction, ulNumCaseKills);

	for (ulCnt = 0; ulCnt < ulNumCaseKills; ulCnt++)
	{
		_rxKillActItem_t			*pstKillAction = rxlib_getKillAction(pstInst, ulNumKillAction, pulKillActIdArray, ulCnt);

		if (NULL != pstKillAction)
		{
			pstKillActList = HLIB_LIST_AppendSortEx (pstKillActList, (void*)pstKillAction, rxlib_CompareNumActionInKillActItem);
		}
	}


END_FUNC:
	if (NULL != pulKillActIdArray)				{ HLIB_STD_MemFree(pulKillActIdArray); }
	return pstKillActList;
}

STATIC HUINT32 rxlib_AppendNewRscIdToArray (HUINT32 *pulRscIdArray, HUINT32 ulRscNum, HUINT32 ulRscId)
{
	HUINT32				 ulCount;

	for (ulCount = 0; ulCount < ulRscNum; ulCount++)
	{
		if (pulRscIdArray[ulCount] == ulRscId)
		{
			return ulRscNum;
		}
	}

	pulRscIdArray[ulRscNum] = ulRscId;
	return (ulRscNum + 1);
}

STATIC HERROR rxlib_GetResourceIdArrayForCheckingAllocation (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq, RxRscType_e eRscType, HUINT32 *pulNumRscId, HUINT32 *pulRscIdArray)
{
	HUINT32						 ulRscId, ulNumRsc = 0;
	_rxRscReqItem_t 			*pstReqItem;
	RxRscObject_t				*pstRscObj;
	_rxRscTable_t				*pstRscTable;

	if (eRscType >= eRxRSCTYPE_MAX)
	{
		return ERR_FAIL;
	}

	pstReqItem = &(pstRscReq->astRscReqItem[eRscType]);
	pstRscTable = rxlib_GetCustomResourceObjectTable(pstInst, eRscType);

	// Resource ID가 정해진 상태: 다른 Resource는 보지 말고 오직 하나만 봐야 한다.
	if (pstReqItem->ulResourceId < pstRscTable->ulRscNum)
	{
		*pulNumRscId = 1;
		pulRscIdArray[0] = pstReqItem->ulResourceId;
		return ERR_OK;
	}

	// 1. 1st Priority: Sharable이라면, 일단 이미 사용되고 있는 resource들의 우선순위가 제일 높다.
	if (TRUE == pstReqItem->bSharable)
	{
		for (ulRscId = 0; ulRscId < pstRscTable->ulRscNum; ulRscId++)
		{
			pstRscObj = rxlib_GetCustomResourceObject(pstInst, eRscType, ulRscId);
			if ((NULL != pstRscObj) && (pstRscObj->nRefCount <= 0))
			{
				continue;
			}

			ulNumRsc = rxlib_AppendNewRscIdToArray(pulRscIdArray, ulNumRsc, ulRscId);
		}
	}

	// 2. 2nd Priority: 이전에 이 Action이 사용하고 있던 Resource가 있으면 그 Resource의 우선순위가 높다.
	if (pstRscReq->ulActionId < pstInst->ulActionNum)
	{
		// 실제 현재 Action ID (Customize되지 않은 상태)에 할당된 Resource가 있으면 제1 체크 요망
		ulRscId = rxlib_GetResourceIdFromResourceMap(pstInst, pstRscReq->ulActionId, eRscType);
		if (RxRSCID_NULL != ulRscId)
		{
			ulNumRsc = rxlib_AppendNewRscIdToArray(pulRscIdArray, ulNumRsc, ulRscId);
		}
	}

	// 3. 3rd priority: Resource Table에 기재된 Prior Resource ID
	if (RxRSCID_NULL != pstRscTable->ulPriorResourceId)
	{
		ulNumRsc = rxlib_AppendNewRscIdToArray(pulRscIdArray, ulNumRsc, pstRscTable->ulPriorResourceId);
	}

	// 4. 4th priority: 위의 경우에 해당하지 않는 비어 있는 Resource
	for (ulRscId = 0; ulRscId < pstRscTable->ulRscNum; ulRscId++)
	{
		ulNumRsc = rxlib_AppendNewRscIdToArray(pulRscIdArray, ulNumRsc, ulRscId);
	}

	*pulNumRscId = ulNumRsc;
	return ERR_OK;
}


STATIC HERROR rxlib_CheckResourceAllocableInCustomResourceMap (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq, RxRscTrialResult_t *pstTrialResult)
{
	HUINT32						 ulRfInputId = RxRSCID_NULL, ulTunerGroupId = RxRSCID_NULL;
	RxRscType_e					 eRscType;
	_rxRscReqItem_t 			*pstReqRfInput = &(pstRscReq->astRscReqItem[eRxRSCTYPE_RFINPUT]);
	_rxRscReqItem_t 			*pstReqTuner = &(pstRscReq->astRscReqItem[eRxRSCTYPE_TUNER_GROUP]);
	_rxRscReqItem_t 			*pstReqDemux = &(pstRscReq->astRscReqItem[eRxRSCTYPE_DEMUX]);
	HERROR						 hErr;

	// Init the result values
	for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
	{
		RxRscAlloc_t			*pstAllocItem = &(pstTrialResult->astRscAllocTable[eRscType]);

		pstAllocItem->bRequest		= FALSE;
		pstAllocItem->ulResourceId	= RxRSCID_NULL;
	}

	// 1. RF-Input & Tuner 을 적용
	if ((TRUE == pstReqRfInput->bRequested) && (TRUE == pstReqTuner->bRequested))
	{
		HUINT32				 ulRfInputIdNum = 0, ulTunerIdNum = 0;
		HUINT32				 aulRfInputIdArray[RxMAX_RESOURCE_NUM], aulTunerIdArray[RxMAX_RESOURCE_NUM];

		if (NULL == rxlib_GetArgumentByArgType(pstRscReq, eRxARGTYPE_TUNEPARAM))
		{
			HxLOG_Error("No Tuning Info!!!\n");
			return ERR_FAIL;
		}

		rxlib_GetResourceIdArrayForCheckingAllocation(pstInst, pstRscReq, eRxRSCTYPE_RFINPUT, &ulRfInputIdNum, aulRfInputIdArray);
		rxlib_GetResourceIdArrayForCheckingAllocation(pstInst, pstRscReq, eRxRSCTYPE_TUNER_GROUP, &ulTunerIdNum, aulTunerIdArray);

		hErr = rxlib_FindRfInputAndTunerAvailable(pstInst, pstRscReq, ulRfInputIdNum, aulRfInputIdArray, ulTunerIdNum, aulTunerIdArray, &ulRfInputId, &ulTunerGroupId);
		if ((ERR_OK != hErr) || (RxRSCID_NULL == ulRfInputId) || (RxRSCID_NULL == ulTunerGroupId))
		{
			HxLOG_Info("rxlib_FindRfInputAndTunerAvailable err\n");
			return ERR_FAIL;
		}

		pstTrialResult->astRscAllocTable[eRxRSCTYPE_RFINPUT].bRequest			= TRUE;
		pstTrialResult->astRscAllocTable[eRxRSCTYPE_RFINPUT].ulResourceId		= ulRfInputId;

		pstTrialResult->astRscAllocTable[eRxRSCTYPE_TUNER_GROUP].bRequest		= TRUE;
		pstTrialResult->astRscAllocTable[eRxRSCTYPE_TUNER_GROUP].ulResourceId	= ulTunerGroupId;
	}
	else if (TRUE == pstReqRfInput->bRequested)
	{
		HUINT32				 ulRfCnt, ulRfInputIdNum = 0;
		HUINT32				 aulRfInputIdArray[RxMAX_RESOURCE_NUM];

		if (NULL == rxlib_GetArgumentByArgType(pstRscReq, eRxARGTYPE_TUNEPARAM))
		{
			HxLOG_Info("No Tuning Info!!!\n");
			return ERR_FAIL;
		}

		rxlib_GetResourceIdArrayForCheckingAllocation(pstInst, pstRscReq, eRxRSCTYPE_RFINPUT, &ulRfInputIdNum, aulRfInputIdArray);

		for (ulRfCnt = 0; ulRfCnt < ulRfInputIdNum; ulRfCnt++)
		{
			hErr = rxlib_CheckRfInputIdAvailable(pstInst, pstRscReq, aulRfInputIdArray[ulRfCnt]);
			if (ERR_OK == hErr)
			{
				ulRfInputId = aulRfInputIdArray[ulRfCnt];
				break;
			}
		}

		if (RxRSCID_NULL == ulRfInputId)
		{
			HxLOG_Info("rxlib_CheckRfInputIdAvailable err\n");
			return ERR_FAIL;
		}

		pstTrialResult->astRscAllocTable[eRxRSCTYPE_RFINPUT].bRequest			= TRUE;
		pstTrialResult->astRscAllocTable[eRxRSCTYPE_RFINPUT].ulResourceId		= ulRfInputId;
	}
	else if (TRUE == pstReqTuner->bRequested)
	{
		HUINT32				 ulTunerCnt, ulTunerIdNum = 0;
		HUINT32				 aulTunerIdArray[RxMAX_RESOURCE_NUM];

		if (NULL == rxlib_GetArgumentByArgType(pstRscReq, eRxARGTYPE_TUNEPARAM))
		{
			HxLOG_Info("No Tuning Info!!!\n");
			return ERR_FAIL;
		}

		rxlib_GetResourceIdArrayForCheckingAllocation(pstInst, pstRscReq, eRxRSCTYPE_TUNER_GROUP, &ulTunerIdNum, aulTunerIdArray);

		for (ulTunerCnt = 0; ulTunerCnt < ulTunerIdNum; ulTunerCnt++)
		{
			hErr = rxlib_CheckTunerIdAvailable(pstInst, pstRscReq, aulTunerIdArray[ulTunerCnt], 0);
			if (ERR_OK == hErr)
			{
				ulTunerGroupId = aulTunerIdArray[ulTunerCnt];
				break;
			}
		}

		if (RxRSCID_NULL == ulTunerGroupId)
		{
			HxLOG_Info("rxlib_CheckTunerIdAvailable err\n");
			return ERR_FAIL;
		}

		pstTrialResult->astRscAllocTable[eRxRSCTYPE_TUNER_GROUP].bRequest		= TRUE;
		pstTrialResult->astRscAllocTable[eRxRSCTYPE_TUNER_GROUP].ulResourceId	= ulTunerGroupId;
	}

	// 2. Select the output-related resources (output related resources cannot be shared at all)
	for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
	{
		HUINT32					 ulRscCnt, ulRscIdNum = 0;
		HUINT32					 aulRscIdArray[RxMAX_RESOURCE_NUM];
		_rxRscReqItem_t			*pstReqItem = &(pstRscReq->astRscReqItem[eRscType]);
		RxRscAlloc_t			*pstAllocItem = &(pstTrialResult->astRscAllocTable[eRscType]);

		if (TRUE != pstReqItem->bRequested)					{ continue; }

		switch (eRscType)
		{
		case eRxRSCTYPE_TUNER_GROUP:
		case eRxRSCTYPE_RFINPUT:
			// Input Resource is allocated already:
		case eRxRSCTYPE_DEMUX:
			// Demux will be allocated later
			break;

		case eRxRSCTYPE_AV_DEC:
			rxlib_GetResourceIdArrayForCheckingAllocation(pstInst, pstRscReq, eRscType, &ulRscIdNum, aulRscIdArray);

			for (ulRscCnt = 0; ulRscCnt < ulRscIdNum; ulRscCnt++)
			{
				hErr = rxlib_CheckAvDecIdAvailable(pstInst, pstRscReq, aulRscIdArray[ulRscCnt]);
				if (ERR_OK == hErr)
				{
					pstAllocItem->bRequest		= TRUE;
					pstAllocItem->ulResourceId	= aulRscIdArray[ulRscCnt];
					break;
				}
			}

			break;

		case eRxRSCTYPE_HDD_REC:
			rxlib_GetResourceIdArrayForCheckingAllocation(pstInst, pstRscReq, eRscType, &ulRscIdNum, aulRscIdArray);

			for (ulRscCnt = 0; ulRscCnt < ulRscIdNum; ulRscCnt++)
			{
				hErr = rxlib_CheckHddRecIdAvailable(pstInst, pstRscReq, aulRscIdArray[ulRscCnt]);
				if (ERR_OK == hErr)
				{
					pstAllocItem->bRequest		= TRUE;
					pstAllocItem->ulResourceId	= aulRscIdArray[ulRscCnt];
					break;
				}
			}

			break;

		case eRxRSCTYPE_PVR_PLAYER:
		case eRxRSCTYPE_TRANSCODER:
			rxlib_GetResourceIdArrayForCheckingAllocation(pstInst, pstRscReq, eRscType, &ulRscIdNum, aulRscIdArray);

			for (ulRscCnt = 0; ulRscCnt < ulRscIdNum; ulRscCnt++)
			{
				hErr = rxlib_CheckExclusionRscIdAvailable(pstInst, pstRscReq, eRscType, aulRscIdArray[ulRscCnt]);
				if (ERR_OK == hErr)
				{
					pstAllocItem->bRequest		= TRUE;
					pstAllocItem->ulResourceId	= aulRscIdArray[ulRscCnt];
					break;
				}
			}

			break;

		default:
			break;
		}
	}

	// Demux allocation:
	if (TRUE == pstReqDemux->bRequested)
	{
		HBOOL					 bDemuxShared = FALSE;

		// 3. Sharabe DEMUX if it is requested
		if (TRUE == pstReqDemux->bSharable)
		{
			HUINT32					 ulDemuxId, ulDemuxNum = 0;

			ulDemuxNum = rxlib_GetResourceNumber(pstInst, eRxRSCTYPE_DEMUX);
			for (ulDemuxId = 0; ulDemuxId < ulDemuxNum; ulDemuxId++)
			{
				hErr = rxlib_CheckSharableDemuxIdAvailable(pstInst, pstRscReq, pstTrialResult, ulDemuxId);
				if (ERR_OK == hErr)
				{
					RxRscAlloc_t		*pstAllocItem = &(pstTrialResult->astRscAllocTable[eRxRSCTYPE_DEMUX]);

					pstAllocItem->bRequest		= TRUE;
					pstAllocItem->ulResourceId	= ulDemuxId;
					bDemuxShared				= TRUE;
					break;
				}
			}
		}

		// 4. Non-sharable DEMUX
		if (TRUE != bDemuxShared)
		{
			HUINT32 				 ulRscCnt, ulRscIdNum = 0;
			HUINT32 				 aulRscIdArray[RxMAX_RESOURCE_NUM];

			rxlib_GetResourceIdArrayForCheckingAllocation(pstInst, pstRscReq, eRxRSCTYPE_DEMUX, &ulRscIdNum, aulRscIdArray);

			for (ulRscCnt = 0; ulRscCnt < ulRscIdNum; ulRscCnt++)
			{
				hErr = rxlib_CheckNonSharableDemuxIdAvailable(pstInst, pstRscReq, aulRscIdArray[ulRscCnt]);
				if (ERR_OK == hErr)
				{
					RxRscAlloc_t			*pstAllocItem = &(pstTrialResult->astRscAllocTable[eRxRSCTYPE_DEMUX]);

					pstAllocItem->bRequest		= TRUE;
					pstAllocItem->ulResourceId	= aulRscIdArray[ulRscCnt];
					break;
				}
			}
		}
	}

	// 5. Check whether there are resources that wasn't allocated:
	// If all the resources are not allocated, then the trial fails.
	for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
	{
		_rxRscReqItem_t			*pstReqItem = &(pstRscReq->astRscReqItem[eRscType]);
		RxRscAlloc_t			*pstAllocItem = &(pstTrialResult->astRscAllocTable[eRscType]);

		if ((TRUE == pstReqItem->bRequested) && (TRUE != pstAllocItem->bRequest))
		{
			HxLOG_Info("Request fail in rsc(%d)err\n", eRscType);
			return ERR_FAIL;
		}
	}

	HxLOG_Debug("Success\n");

	return ERR_OK;
}

STATIC HERROR rxlib_AppendActionToBeKilledToTrialResult (_rxInst_t *pstInst, RxRscTrialResult_t *pstTrialResult, HUINT32 ulActionId)
{
	HUINT32				 ulCount;

	if (pstTrialResult->ulNumActionToBeKilled >= pstInst->ulActionNum)
	{
		return ERR_FAIL;
	}

	for (ulCount = 0; ulCount < pstTrialResult->ulNumActionToBeKilled; ulCount++)
	{
		if (pstTrialResult->pulActionsToBeKilled[ulCount] == ulActionId)
		{
			return ERR_OK;
		}
	}

	pstTrialResult->pulActionsToBeKilled[pstTrialResult->ulNumActionToBeKilled] = ulActionId;
	pstTrialResult->ulNumActionToBeKilled ++;

	return ERR_OK;
}

STATIC HBOOL rxlib_CheckTrialResultRscsUsedByAction (_rxInst_t *pstInst, _rxRscReq_t *pstRscReq, RxRscTrialResult_t *pstTrialResult, HUINT32 ulActionId)
{
	HUINT32			 ulRscId;
	RxRscType_e		 eRscType;

	for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
	{
		RxRscAlloc_t		 *pstRscAlloc = &(pstTrialResult->astRscAllocTable[eRscType]);

		if (TRUE == pstRscAlloc->bRequest)				{ continue; }

		ulRscId = rxlib_GetResourceIdFromResourceMap(pstInst, ulActionId, eRscType);
		if (ulRscId > RxMAX_RESOURCE_NUM)				{ continue; }

		if (pstRscAlloc->ulResourceId != ulRscId)		{ continue; }

		switch (eRscType)
		{
		case eRxRSCTYPE_RFINPUT:
			{
				RxRscObject_t 			*pstRfInputObj = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_RFINPUT, ulRscId);
				RxTrialArg_t			*punArg = rxlib_GetArgumentByArgType(pstRscReq, eRxARGTYPE_TUNEPARAM);

				if ((NULL != punArg) && (NULL != pstRfInputObj) &&
					(eDxDELIVERY_TYPE_SAT & pstRfInputObj->utObj.stRfInput.eSupportedDeliType))
				{
					RxSatRfInputObj_t	*pstSatRf = &(pstRfInputObj->utObj.stRfInput.utObj.stSatRf);

					if (rxlib_CheckRfInputSatAcceptable(&(pstSatRf->stMasterTuneParam), &(punArg->stTuneParam.stTuneParam)) != TRUE)
					{
						return TRUE;
					}
				}
			}

			break;

		case eRxRSCTYPE_TUNER_GROUP:
			{
				RxRscObject_t 			*pstTunerObj = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_TUNER_GROUP, ulRscId);
				RxTrialArg_t			*punArg = rxlib_GetArgumentByArgType(pstRscReq, eRxARGTYPE_TUNEPARAM);

				if ((NULL != punArg) && (NULL != pstTunerObj))
				{
					if ((TRUE != pstTunerObj->bSharable) ||
						(rxlib_CheckTunerAcceptable(&(pstTunerObj->utObj.stTuner.stTuneParam), &(punArg->stTuneParam.stTuneParam)) != TRUE))
					{
						return TRUE;
					}
				}
			}

			break;

		default:
			return TRUE;
		}
	}

	return FALSE;
}

STATIC HERROR rxlib_GetActualLnbFrequency (HUINT32 ulLnbFreq, HUINT32 ulFreq, DxSat_Polarization_e ePolar, HBOOL b22kTone, HUINT32 *pulRealLnbFreq, HBOOL *pbPhysical22kTone)
{
	HBOOL			 bPhysical22kTone = FALSE;
	HUINT32			 ulRealLnbFreq = DxSAT_UNIVERSAL_LNB_FREQ;

	switch (ulLnbFreq)
	{
	case DxSAT_UNIVERSAL_LNB_FREQ:
		if (abs((HINT32)(ulFreq - 9750)) < 1950) /* MHz */
		{
			ulRealLnbFreq = 9750;
			bPhysical22kTone = FALSE;
		}
		else
		{
			ulRealLnbFreq = 10600;
			bPhysical22kTone = TRUE;
		}

		break;

	case DxSAT_MDU1_LNB_FREQ:
		ulRealLnbFreq = (eDxSAT_POLAR_HOR == ePolar) ? 10450 : 10000;
		bPhysical22kTone = FALSE;
		break;

	case DxSAT_MDU2_LNB_FREQ:
		if ((ulFreq >= 10780) && (ulFreq <= 10950))
		{
			ulRealLnbFreq = (eDxSAT_POLAR_HOR == ePolar) ? 9830 : 9550;
		}
		else if ((ulFreq >= 11450) && (ulFreq <= 11700))
		{
			ulRealLnbFreq = (eDxSAT_POLAR_HOR == ePolar) ? 9925 : 9550;
		}

		bPhysical22kTone = FALSE;
		break;

	case DxSAT_MDU3_LNB_FREQ:
		if ((ulFreq >= 10780) && (ulFreq <= 10950))
		{
			ulRealLnbFreq = (eDxSAT_POLAR_HOR == ePolar) ? 12930 : 9830;
		}
		else if ((ulFreq >= 11450) && (ulFreq <= 11700))
		{
			ulRealLnbFreq = (eDxSAT_POLAR_HOR == ePolar) ? 9830 : 12930;
		}

		bPhysical22kTone = FALSE;
		break;

	case DxSAT_MDU4_LNB_FREQ:
		if ((ulFreq >= 10780) && (ulFreq <= 10950))
		{
			ulRealLnbFreq = (eDxSAT_POLAR_HOR == ePolar) ? 12930 : 12650;
		}
		else if ((ulFreq >= 11450) && (ulFreq <= 11700))
		{
			ulRealLnbFreq = (eDxSAT_POLAR_HOR == ePolar) ? 13035 : 12650;
		}

		bPhysical22kTone = FALSE;
		break;

	default:
		ulRealLnbFreq = ulLnbFreq;
		bPhysical22kTone = b22kTone;
	}

	if ((DxSAT_UNIVERSAL_LNB_FREQ == ulRealLnbFreq) || (0 == ulRealLnbFreq))
	{
		HxLOG_Error("Something wrnog in actual LNB frequency... ulLnbFreq(%d, 0x%08x), ulFreq(%d), Polar(%d)\n", ulLnbFreq, ulLnbFreq, ulFreq, ePolar);
		return ERR_FAIL;
	}

	if (NULL != pulRealLnbFreq)
	{
		*pulRealLnbFreq = ulRealLnbFreq;
	}

	if (NULL != pbPhysical22kTone)
	{
		*pbPhysical22kTone = bPhysical22kTone;
	}

	return ERR_OK;
}


#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
Handle_t RLIB_CreateSessionHandle (HCHAR *szInstName)
{
	return rxlib_CreateInstance(szInstName);
}


HERROR RLIB_ClearSessionHandleInfo (Handle_t hSession)
{
	_rxInst_t				*pstInst = (_rxInst_t *)hSession;

	HxLOG_Trace();

	pstInst->ulActionNum = 0;

	if(pstInst->pstRscTable != NULL)
	{
		HLIB_STD_MemFree(pstInst->pstRscTable);
		pstInst->pstRscTable = NULL;
	}

	if(pstInst->pstRscMaps != NULL)
	{
		HLIB_STD_MemFree(pstInst->pstRscMaps);
		pstInst->pstRscMaps = NULL;
	}

	if(pstInst->pstCustomRscTable != NULL)
	{
		HLIB_STD_MemFree(pstInst->pstCustomRscTable);
		pstInst->pstCustomRscTable = NULL;
	}

	if(pstInst->pstCustomRscMaps != NULL)
	{
		HLIB_STD_MemFree(pstInst->pstCustomRscMaps);
		pstInst->pstCustomRscMaps = NULL;
	}


	HLIB_LIST_RemoveAllFunc(pstInst->pstRscReqList, rxlib_cbFreeRscReq);
	pstInst->pstRscReqList = NULL;

	// 이름은 남겨놓는다. RLIB_CreateSessionHandle로 채워져 있을 값.
//	HxSTD_MemSet(pstInst, 0x00, sizeof(_rxInst_t));

	return ERR_OK;

}

HERROR RLIB_CopySessionHandleInfo (Handle_t hSessionSrc, Handle_t hSessionDst)
{
	_rxInst_t				*pstInstSrc = (_rxInst_t *)hSessionSrc;
	_rxInst_t				*pstInstDst = (_rxInst_t *)hSessionDst;
#if 0
	HxList_t				*pstReqItem;
#endif

	if(pstInstSrc == NULL || pstInstDst == NULL)
	{
		HxLOG_Error("invalid parameter \n");
		return ERR_FAIL;
	}

	if(pstInstSrc->ulActionNum == 0)
	{
		HxLOG_Error("pstInstSrc->ulActionNum == 0 \n");
		return ERR_FAIL;
	}

	pstInstDst->ulActionNum = pstInstSrc->ulActionNum;

	if(pstInstDst->pstRscTable == NULL)
	{
		pstInstDst->pstRscTable = (_rxRscTable_t*)HLIB_STD_MemCalloc(sizeof(_rxRscTable_t) * eRxRSCTYPE_MAX);
		if(pstInstDst->pstRscTable == NULL)
		{
			HxLOG_Error("alloc fail \n");
			return ERR_FAIL;
		}
	}

	if(pstInstDst->pstRscMaps == NULL)
	{
		pstInstDst->pstRscMaps = (_rxRscMap_t *)HLIB_STD_MemCalloc(sizeof(_rxRscMap_t) * pstInstDst->ulActionNum);
		if(pstInstDst->pstRscMaps == NULL)
		{
			HxLOG_Error("alloc fail \n");
			return ERR_FAIL;
		}
	}
	else // 어차피 RLIB_ClearSessionHandleInfo로 clear 하면서 NULL 을 넣긴 했을 테지만 혹시 모르니.
	{
		HxLOG_Error("pstInstDst->pstRscMaps shoud be NULL! \n");
		return ERR_FAIL;
	}

	if(pstInstDst->pstCustomRscTable == NULL)
	{
		pstInstDst->pstCustomRscTable = (_rxRscTable_t*)HLIB_STD_MemCalloc(sizeof(_rxRscTable_t) * eRxRSCTYPE_MAX);
		if(pstInstDst->pstCustomRscTable == NULL)
		{
			HxLOG_Error("alloc fail \n");
			return ERR_FAIL;
		}
	}

	if(pstInstDst->pstCustomRscMaps == NULL)
	{
		pstInstDst->pstCustomRscMaps = (_rxRscMap_t *)HLIB_STD_MemCalloc(sizeof(_rxRscMap_t) * pstInstDst->ulActionNum);
		if(pstInstDst->pstCustomRscMaps == NULL)
		{
			HxLOG_Error("alloc fail \n");
			return ERR_FAIL;
		}
	}
	else // 어차피 RLIB_ClearSessionHandleInfo로 clear 하면서 NULL 을 넣긴 했을 테지만 혹시 모르니.
	{
		HxLOG_Error("pstInstDst->pstCustomRscMaps shoud be NULL! \n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstInstDst->pstRscTable, pstInstSrc->pstRscTable, sizeof(_rxRscTable_t) * eRxRSCTYPE_MAX);
	HxSTD_MemCopy(pstInstDst->pstRscMaps, pstInstSrc->pstRscMaps, sizeof(_rxRscMap_t) * pstInstDst->ulActionNum);
	HxSTD_MemCopy(pstInstDst->pstCustomRscTable, pstInstSrc->pstCustomRscTable, sizeof(_rxRscTable_t) * eRxRSCTYPE_MAX);
	HxSTD_MemCopy(pstInstDst->pstCustomRscMaps, pstInstSrc->pstCustomRscMaps, sizeof(_rxRscMap_t) * pstInstDst->ulActionNum);

#if 0 // 이 값은 RLIB_MakeResourceRequestHandle 로 채워져야함.
	for (pstReqItem = pstInstSrc->pstRscReqList; NULL != pstReqItem; pstReqItem = pstReqItem->next)
	{
		_rxRscReq_t 		*pstRscReq = (_rxRscReq_t *)HLIB_LIST_Data(pstReqItem);

		RLIB_MakeResourceRequestHandle(hSessionDst, pstRscReq->ulActionId, pstRscReq->ulActionPriority);

	}
#endif

	return ERR_OK;

}


HERROR RLIB_SetActionNumber (Handle_t hSession, HUINT32 ulMaxActNum)
{
	HUINT32				 ulCount, ulSubCnt;
	_rxInst_t			*pstInst = (_rxInst_t *)hSession;
	_rxRscMap_t			*pstRscMaps = NULL, *pstCustomRscMaps = NULL;

	if (NULL == pstInst)
	{
		HxLOG_Error("[%s:%d] hSession NULL\n");
		return ERR_FAIL;
	}

	if ((0 != pstInst->ulActionNum) || (0 == ulMaxActNum))
	{
		return ERR_FAIL;
	}

	pstRscMaps = (_rxRscMap_t *)HLIB_STD_MemCalloc(sizeof(_rxRscMap_t) * ulMaxActNum);
	pstCustomRscMaps = (_rxRscMap_t *)HLIB_STD_MemCalloc(sizeof(_rxRscMap_t) * ulMaxActNum);
	if ((NULL == pstRscMaps) || (NULL == pstCustomRscMaps))
	{
		HxLOG_Error("Memory allocation failed\n");
		goto ERROR;
	}

	for (ulCount = 0; ulCount < ulMaxActNum; ulCount++)
	{
		pstRscMaps[ulCount].ulActionId	= ulCount;
		pstCustomRscMaps[ulCount].ulActionId	= ulCount;

		for (ulSubCnt = 0; ulSubCnt < RxMAX_RESOURCE_NUM; ulSubCnt++)
		{
			pstRscMaps[ulCount].aulResourceId[ulSubCnt] = RxRSCID_NULL;
			pstCustomRscMaps[ulCount].aulResourceId[ulSubCnt] = RxRSCID_NULL;
		}
	}

	pstInst->ulActionNum		= ulMaxActNum;
	pstInst->pstRscMaps			= pstRscMaps;
	pstInst->pstCustomRscMaps	= pstCustomRscMaps;

	return ERR_OK;

ERROR:
	if (NULL != pstRscMaps)					{ HLIB_STD_MemFree(pstRscMaps); }
	if (NULL != pstCustomRscMaps)			{ HLIB_STD_MemFree(pstCustomRscMaps); }

	return ERR_FAIL;
}

HERROR RLIB_SetResourceNumber (Handle_t hSession, RxRscType_e eRscType, HUINT32 ulRscNum)
{
	HUINT32				 ulCount;
	_rxInst_t			*pstInst = (_rxInst_t *)hSession;
	_rxRscTable_t		*pstRscTable;

	if (NULL == pstInst)
	{
		HxLOG_Error("[%s:%d] hSession NULL\n");
		return ERR_FAIL;
	}

	pstRscTable = rxlib_GetResourceObjectTable(pstInst, eRscType);
	if ((NULL == pstRscTable) || (0 == ulRscNum))
	{
		return ERR_FAIL;
	}

	if (ulRscNum > RxMAX_RESOURCE_NUM)
	{
		HxLOG_Critical("\n\n!!! RLIB has the limit of Resource Num(%d:%d) !!!\n\n", ulRscNum, RxMAX_RESOURCE_NUM);
		abort();
	}

	if (TRUE == pstRscTable->bInited)
	{
		HxLOG_Error("Already RSCNUM setted: eRscType(%d)\n", eRscType);
		return ERR_FAIL;
	}

	pstRscTable->bInited		= TRUE;
	pstRscTable->eRscType		= eRscType;
	pstRscTable->ulRscNum		= ulRscNum;

	for (ulCount = 0; ulCount < ulRscNum; ulCount++)
	{
		RxRscObject_t			*pstRscObj = &(pstRscTable->astRscObj[ulCount]);

		pstRscObj->ulRscId		= ulCount;
		pstRscObj->eRscType		= eRscType;
	}

	return ERR_OK;
}

HERROR RLIB_SetRfInfoCapacity (Handle_t hSession, HUINT32 ulRscId, RxRfInfoCapacity_t *pstCapacity)
{
	_rxInst_t			*pstInst = (_rxInst_t *)hSession;
	RxRscObject_t		*pstRscObj;
	RxRfInputObject_t	*pstRfInput;

	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstCapacity)
	{
		return ERR_FAIL;
	}

	pstRscObj = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_RFINPUT, ulRscId);
	if (NULL == pstRscObj)
	{
		HxLOG_Error("rxlib_GetResourceObject NULL: eRscType:eRxRSCTYPE_RFINPUT, ulRscId(%d)\n", ulRscId);
		return ERR_FAIL;
	}

	pstRfInput = &(pstRscObj->utObj.stRfInput);
	pstRfInput->eSupportedDeliType = pstCapacity->eSupportedDeliType;

	if (pstCapacity->eSupportedDeliType & eDxDELIVERY_TYPE_SAT)
	{
		RxSatRfInputObj_t		*pstSatRfInput = &(pstRfInput->utObj.stSatRf);

		pstSatRfInput->ulConnectableTunerNum	= (pstCapacity->ulConnectableTunerNum > RxTUNER_NUM_MAX) ? RxTUNER_NUM_MAX : pstCapacity->ulConnectableTunerNum;
		HxSTD_MemCopy(pstSatRfInput->aulConnectableTunerId, pstCapacity->aulConnectableTunerId, sizeof(HUINT32) * pstSatRfInput->ulConnectableTunerNum);
	}

	return ERR_OK;
}

HERROR RLIB_SetTunerCapacity (Handle_t hSession, HUINT32 ulRscId, DxDeliveryType_e eSupportedDeliType)
{
	_rxInst_t			*pstInst = (_rxInst_t *)hSession;
	RxRscObject_t		*pstRscObj;
	RxTunerObject_t		*pstTuner;

	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	pstRscObj = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_TUNER_GROUP, ulRscId);
	if (NULL == pstRscObj)
	{
		HxLOG_Error("rxlib_GetResourceObject NULL: eRscType:eRxRSCTYPE_TUNER_GROUP, ulRscId(%d)\n", ulRscId);
		return ERR_FAIL;
	}

	pstTuner = &(pstRscObj->utObj.stTuner);
	pstTuner->eSupportedDeliType	= eSupportedDeliType;

	return ERR_OK;
}

HERROR RLIB_SetResourceUsageType (Handle_t hSession, RxRscType_e eRscType, HUINT32 ulRscId, RxUsageType_e eUsageType)
{
	_rxInst_t			*pstInst = (_rxInst_t *)hSession;
	RxRscObject_t		*pstRscObj;

	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	pstRscObj = rxlib_GetResourceObject(pstInst, eRscType, ulRscId);
	if (NULL == pstRscObj)
	{
		HxLOG_Error("rxlib_GetResourceObject NULL: eRscType(%d), ulRscId(%d)\n", eRscType, ulRscId);
		return ERR_FAIL;
	}

	pstRscObj->utObj.stTemplate.eUsageType = eUsageType;
	return ERR_OK;
}

RxRscReqHandle_t RLIB_MakeResourceRequestHandle (Handle_t hSession, HUINT32 ulActionId, HUINT32 ulActionPriority)
{
	_rxInst_t		*pstInst = (_rxInst_t *)hSession;
	_rxRscReq_t		*pstRscReq;

	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return (RxRscReqHandle_t)NULL;
	}

	pstRscReq = rxlib_AllocRscReq(pstInst);
	if (NULL == pstRscReq)
	{
		HxLOG_Error("RscReq Allocation failed:\n");
		return (RxRscReqHandle_t)NULL;
	}

	pstRscReq->hReqHandle		= (RxRscReqHandle_t)pstRscReq;
	pstRscReq->ulActionId		= ulActionId;
	pstRscReq->ulActionPriority	= ulActionPriority;

	pstInst->pstRscReqList = HLIB_LIST_Append (pstInst->pstRscReqList, (void *)pstRscReq);
	return (RxRscReqHandle_t)pstRscReq;
}

RxRscReqHandle_t RLIB_DuplicateRequestHandle (RxRscReqHandle_t hRscReq)
{
	_rxInst_t		*pstInst = NULL, *pstDupInst = NULL;
	_rxRscReq_t		*pstSrcRscReq = NULL, *pstDstRscReq = NULL;

	(void)rxlib_GetInstAndRscReqFromReqHandle(hRscReq, &pstInst, &pstSrcRscReq);

	if ((NULL == pstInst) || (NULL == pstSrcRscReq))
	{
		HxLOG_Error("Argument wrong: hRscReq(0x%08x)\n", hRscReq);
		goto ERROR;
	}

	pstDupInst = (void *)pstSrcRscReq->pvDuplicatedInst;
	if (NULL == pstDupInst)
	{
		HxLOG_Error("No Duplicated Instance: something wrong!\n");
		goto ERROR;
	}

	pstDstRscReq = (_rxRscReq_t *)HLIB_STD_MemCalloc(sizeof(_rxRscReq_t));
	if (NULL == pstDstRscReq)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto ERROR;
	}

	pstDstRscReq->hReqHandle			= (RxRscReqHandle_t)pstDstRscReq;
	pstDstRscReq->ulActionId			= pstSrcRscReq->ulActionId;
	pstDstRscReq->ulActionPriority		= pstSrcRscReq->ulActionPriority;
	pstDstRscReq->ulNumRemovableAction	= pstSrcRscReq->ulNumRemovableAction;
	pstDstRscReq->pstRemovableActions	= (_rxActionInfo_t *)HLIB_STD_MemDup(pstSrcRscReq->pstRemovableActions, sizeof(_rxActionInfo_t) * pstInst->ulActionNum);
	pstDstRscReq->ulNumIgnorableAction	= pstSrcRscReq->ulNumIgnorableAction;
	pstDstRscReq->pstIgnorableActions	= (_rxActionInfo_t *)HLIB_STD_MemDup(pstSrcRscReq->pstIgnorableActions, sizeof(_rxActionInfo_t) * pstInst->ulActionNum);
	HxSTD_MemCopy(pstDstRscReq->astRscReqItem, pstSrcRscReq->astRscReqItem, sizeof(_rxRscReqItem_t) * eRxRSCTYPE_MAX);

	pstDstRscReq->pvDuplicatedInst		= (void *)rxlib_DuplicateInstance(pstDupInst);
	pstDstRscReq->pvOriginalInst		= pstSrcRscReq->pvOriginalInst;

	pstInst->pstRscReqList = HLIB_LIST_Append (pstInst->pstRscReqList, (void *)pstDstRscReq);
	return (RxRscReqHandle_t)pstDstRscReq;

ERROR:
	if (NULL != pstDstRscReq)				{ rxlib_FreeRscReq(pstDstRscReq); }
	return (RxRscReqHandle_t)NULL;
}

HERROR RLIB_AddRemovableActionIdToRequestHandle (RxRscReqHandle_t hRscReq, HUINT32 ulActionId, HUINT32 ulActionPriority)
{
	_rxInst_t		*pstInst = NULL;
	_rxRscReq_t		*pstRscReq = NULL;
	_rxActionInfo_t	*pstActInfo;

	(void)rxlib_GetInstAndRscReqFromReqHandle(hRscReq, &pstInst, &pstRscReq);

	if ((NULL == pstInst) || (NULL == pstRscReq) || (ulActionId >= pstInst->ulActionNum))
	{
		HxLOG_Error("Argument wrong: hRscReq(0x%08x), ulActionId(%d)\n", hRscReq, ulActionId);
		return ERR_FAIL;
	}

	if (ulActionPriority > pstRscReq->ulActionPriority)
	{
		HxLOG_Info ("The action (%d) has the higher priority (%d): cannot destroyed:\n", ulActionId, ulActionPriority);
		return ERR_OK;
	}

	pstActInfo = &(pstRscReq->pstRemovableActions[ulActionId]);

	pstActInfo->bEnabled = TRUE;
	pstActInfo->ulActionId = ulActionId;
	pstActInfo->ulPriority = ulActionPriority;

	return ERR_OK;
}

HERROR RLIB_AddIgnorableActionIdToRequestHandle (RxRscReqHandle_t hRscReq, HUINT32 ulActionId)
{
	_rxInst_t		*pstInst = NULL;
	_rxRscReq_t		*pstRscReq = NULL;
	_rxActionInfo_t	*pstActInfo;

	(void)rxlib_GetInstAndRscReqFromReqHandle(hRscReq, &pstInst, &pstRscReq);

	if ((NULL == pstInst) || (NULL == pstRscReq) || (ulActionId >= pstInst->ulActionNum))
	{
		HxLOG_Error("Argument wrong: hRscReq(0x%08x), ulActionId(%d)\n", hRscReq, ulActionId);
		return ERR_FAIL;
	}

	pstActInfo = &(pstRscReq->pstIgnorableActions[ulActionId]);

	pstActInfo->bEnabled = TRUE;
	pstActInfo->ulActionId = ulActionId;

	return ERR_OK;
}

HERROR RLIB_AddRequiredResourceToRequestHandle (RxRscReqHandle_t hRscReq, RxRscType_e eRscType, HUINT32 ulResourceId, HBOOL bSharable, RxUsageType_e eUsageType)
{
	_rxInst_t			*pstInst = NULL;
	_rxRscReq_t			*pstRscReq = NULL;
	_rxRscTable_t		*pstRscTable;
	_rxRscReqItem_t		*pstRscReqItem;

	(void)rxlib_GetInstAndRscReqFromReqHandle(hRscReq, &pstInst, &pstRscReq);

	if ((NULL == pstInst) || (NULL == pstRscReq))
	{
		HxLOG_Error("Argument wrong: hRscReq(0x%08x), eRscType(%d)\n", hRscReq, eRscType);
		return ERR_FAIL;
	}

#if defined(REQUEST_CONTAINS_DUBBED_INST)
	pstInst = (_rxInst_t *)pstRscReq->pvDuplicatedInst;
	if (NULL == pstInst)
	{
		HxLOG_Error("No duplicated instance... something wrong.\n");
		return ERR_FAIL;
	}
#endif

	pstRscTable = rxlib_GetResourceObjectTable(pstInst, eRscType);
	if (NULL == pstRscTable)
	{
		HxLOG_Error("Argument wrong: hRscReq(0x%08x), eRscType(%d)\n", hRscReq, eRscType);
		return ERR_FAIL;
	}

	pstRscReqItem = &(pstRscReq->astRscReqItem[eRscType]);

	pstRscReqItem->bRequested		= TRUE;
	pstRscReqItem->ulResourceId		= ulResourceId;
	pstRscReqItem->bSharable		= bSharable;
	pstRscReqItem->eUsage			= eUsageType;

	return ERR_OK;
}

HERROR RLIB_AddArgumentToRequestHandle (RxRscReqHandle_t hRscReq, RxTrialArg_t *punArg)
{
	_rxInst_t			*pstInst = NULL;
	_rxRscReq_t			*pstRscReq = NULL;
	RxTrialArg_t		*punInArg;

	(void)rxlib_GetInstAndRscReqFromReqHandle(hRscReq, &pstInst, &pstRscReq);

	if ((NULL == pstInst) || (NULL == pstRscReq) || (NULL == punArg))
	{
		HxLOG_Error("Argument wrong: hRscReq(0x%08x), punArg(0x%08x)\n", hRscReq, punArg);
		return ERR_FAIL;
	}

	punInArg = rxlib_GetArgumentByArgType(pstRscReq, punArg->eArgType);
	if (NULL != punInArg)
	{
		HxSTD_MemCopy(punInArg, punArg, sizeof(RxTrialArg_t));
	}
	else
	{
		punInArg = (RxTrialArg_t *)HLIB_STD_MemAlloc (sizeof(RxTrialArg_t));
		if (NULL == punInArg)
		{
			HxLOG_Error("Memory Allocation failed:\n");
			return ERR_FAIL;
		}

		HxSTD_MemCopy(punInArg, punArg, sizeof(RxTrialArg_t));
		pstRscReq->pstArgList = HLIB_LIST_Append (pstRscReq->pstArgList, (void *)punInArg);
	}

	return ERR_OK;
}

HERROR RLIB_TryResourceRequest (RxRscReqHandle_t hRscReq)
{
	HUINT32				 ulActCnt, ulRscCnt;
	HxList_t			*pstItem, *pstKillList = NULL;
	_rxInst_t			*pstInst = NULL;
	_rxRscReq_t			*pstRscReq = NULL;
	RxRscTrialResult_t	*pstTrialResult = NULL;
	HERROR				 hErr, hResult = ERR_FAIL;

	(void)rxlib_GetInstAndRscReqFromReqHandle(hRscReq, &pstInst, &pstRscReq);

	if ((NULL == pstInst) || (NULL == pstRscReq))
	{
		HxLOG_Error("Argument wrong: hRscReq(0x%08x)\n", hRscReq);
		goto END_FUNC;
	}

#if defined(REQUEST_CONTAINS_DUBBED_INST)
	pstInst = (_rxInst_t *)pstRscReq->pvDuplicatedInst;
	if (NULL == pstInst)
	{
		HxLOG_Error("No duplicated instance... something wrong.\n");
		return ERR_FAIL;
	}
#endif

	if (NULL != pstRscReq->pstTrialResult)
	{
		rxlib_FreeTrialResult(pstRscReq->pstTrialResult);
		pstRscReq->pstTrialResult = NULL;
	}

	pstTrialResult = rxlib_AllocTrialResult(pstInst);
	if (NULL == pstTrialResult)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto END_FUNC;
	}

	// 1. RequestList에서 ActionKill Patern을 만든다.
	pstKillList = rxlib_GetKillActionList(pstInst, pstRscReq);

	// 2. Action Kill Item에 맞추어서 Resource를 빼보는 체크를 한다.
	for (pstItem = pstKillList; NULL != pstItem; pstItem = pstItem->next)
	{
		_rxKillActItem_t			*pstKillAction = (_rxKillActItem_t *)HLIB_LIST_Data(pstItem);

		if (NULL != pstKillAction)
		{
			rxlib_InitCustomResourceMap(pstInst, pstRscReq);

			for (ulActCnt = 0; ulActCnt < pstKillAction->ulNumAction; ulActCnt++)
			{
				HUINT32			 ulActionId = pstKillAction->pulActIdArray[ulActCnt];

				for (ulRscCnt = 0; ulRscCnt < eRxRSCTYPE_MAX; ulRscCnt++)
				{
					rxlib_SetResourceIdToCustomResourceMap(pstInst, ulActionId, ulRscCnt, RxRSCID_NULL);
				}
			}

			for (ulRscCnt = 0; ulRscCnt < eRxRSCTYPE_MAX; ulRscCnt++)
			{
				rxlib_UpdateRefCountInCustomResourceMap(pstInst, ulRscCnt);
			}

			hErr = rxlib_CheckResourceAllocableInCustomResourceMap(pstInst, pstRscReq, pstTrialResult);
			if (ERR_OK == hErr)
			{
				HUINT32			 ulActionId;
				_rxActionInfo_t *pstActInfo;

				for (ulActCnt = 0; ulActCnt < pstKillAction->ulNumAction; ulActCnt++)
				{
					HUINT32 		 ulActionId = pstKillAction->pulActIdArray[ulActCnt];

					rxlib_AppendActionToBeKilledToTrialResult(pstInst, pstTrialResult, ulActionId);
				}

				for (ulActionId = 0; ulActionId < pstInst->ulActionNum; ulActionId++)
				{
					pstActInfo = &(pstRscReq->pstIgnorableActions[ulActionId]);
					if (TRUE == pstActInfo->bEnabled)
					{
						if (rxlib_CheckTrialResultRscsUsedByAction(pstInst, pstRscReq, pstTrialResult, ulActionId) == TRUE)
						{
							rxlib_AppendActionToBeKilledToTrialResult(pstInst, pstTrialResult, ulActionId);
						}
					}
				}

				hResult = ERR_OK;

				pstRscReq->pstTrialResult = pstTrialResult;
				pstTrialResult = NULL;

				goto END_FUNC;
			}
		}
	}

	// HxLOG_Error("No resource found\n");

END_FUNC:
	if (NULL != pstTrialResult)					{ rxlib_FreeTrialResult(pstTrialResult); }
	if (NULL != pstKillList)
	{
		HLIB_LIST_RemoveAllFunc (pstKillList, rxlib_FreeKillAction);
	}

	return hResult;

}

HERROR RLIB_SetResourcesByTrialResults (RxRscReqHandle_t hRscReq)
{
	RxRscType_e			 eRscType;
	RxTrialArg_t		*punArg;
	_rxInst_t			*pstInst = NULL;
	_rxRscReq_t			*pstRscReq = NULL;

	(void)rxlib_GetInstAndRscReqFromReqHandle(hRscReq, &pstInst, &pstRscReq);

	if ((NULL == pstInst) || (NULL == pstRscReq))
	{
		HxLOG_Error("Argument wrong: hRscReq(0x%08x)\n", hRscReq);
		return ERR_FAIL;
	}

	if (NULL == pstRscReq->pstTrialResult)
	{
		HxLOG_Error("Not Tried, or Try failed.\n");
		return ERR_FAIL;
	}

	for (eRscType = 0; eRscType < eRxRSCTYPE_MAX; eRscType++)
	{
		_rxRscReqItem_t		*pstReqItem = &(pstRscReq->astRscReqItem[eRscType]);
		RxRscAlloc_t		*pstRscAlloc = &(pstRscReq->pstTrialResult->astRscAllocTable[eRscType]);

		if ((TRUE == pstRscAlloc->bRequest) && (pstRscAlloc->ulResourceId < RxMAX_RESOURCE_NUM))
		{
			rxlib_SetResourceIdToResourceMap(pstInst, pstRscReq->ulActionId, eRscType, pstRscAlloc->ulResourceId);
			rxlib_UpdateRefCountInResourceMap(pstInst, eRscType);

			switch (eRscType)
			{
			case eRxRSCTYPE_TUNER_GROUP:
			case eRxRSCTYPE_RFINPUT:
				punArg = rxlib_GetArgumentByArgType(pstRscReq, eRxARGTYPE_TUNEPARAM);
				break;

			default:
				punArg = NULL;
				break;
			}

			rxlib_SetShareObjectToResourceMap(pstInst, eRscType, pstRscAlloc->ulResourceId, pstReqItem->bSharable, punArg);
		}
	}

	return ERR_OK;
}

RxRscTrialResult_t *RLIB_GetRequestTrialResult (RxRscReqHandle_t hRscReq)
{
	_rxInst_t			*pstInst = NULL;
	_rxRscReq_t			*pstRscReq = NULL;

	(void)rxlib_GetInstAndRscReqFromReqHandle(hRscReq, &pstInst, &pstRscReq);

	if ((NULL == pstInst) || (NULL == pstRscReq))
	{
		HxLOG_Error("Argument wrong: hRscReq(0x%08x)\n", hRscReq);
		return NULL;
	}

	return pstRscReq->pstTrialResult;
}

HERROR RLIB_DestroyResourceRequestHandle (RxRscReqHandle_t hRscReq)
{
	_rxInst_t			*pstInst = NULL;
	_rxRscReq_t			*pstRscReq = NULL;

	(void)rxlib_GetInstAndRscReqFromReqHandle(hRscReq, &pstInst, &pstRscReq);

	if ((NULL == pstInst) || (NULL == pstRscReq))
	{
		HxLOG_Error("Argument wrong: hRscReq(0x%08x)\n", hRscReq);
		return ERR_FAIL;
	}

	pstInst->pstRscReqList = HLIB_LIST_Remove(pstInst->pstRscReqList, (const void *)pstRscReq);
	rxlib_FreeRscReq(pstRscReq);

	return ERR_OK;
}

HERROR RLIB_ClearActionResource (Handle_t hSession, HUINT32 ulActionId)
{
	HUINT32				 ulRscCnt;
	_rxInst_t			*pstInst = (_rxInst_t *)hSession;

	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	for (ulRscCnt = 0; ulRscCnt < RxMAX_RESOURCE_NUM; ulRscCnt++)
	{
		rxlib_SetResourceIdToResourceMap(pstInst, ulActionId, ulRscCnt, RxRSCID_NULL);
		rxlib_UpdateRefCountInResourceMap(pstInst, ulRscCnt);
	}

	return ERR_OK;
}

HERROR RLIB_GetResourceId (Handle_t hSession, HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 *pulRscId)
{
	HUINT32					 ulRscId;
	_rxInst_t				*pstInst = (_rxInst_t *)hSession;

	if (NULL == pulRscId)					{ return ERR_FAIL; }

	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	if (ulActionId >= pstInst->ulActionNum)
	{
		HxLOG_Error("ActionId (%d)\n", ulActionId);
		return ERR_FAIL;
	}

	if (eRscType >= eRxRSCTYPE_MAX)
	{
		HxLOG_Error("eRscType (%d)\n", eRscType);
		return ERR_FAIL;
	}

	ulRscId = rxlib_GetResourceIdFromResourceMap(pstInst, ulActionId, eRscType);
	if (RxRSCID_NULL == ulRscId)
	{
		HxLOG_Print("ActionId(%d), RscType(%d) not allocated:\n", ulActionId, eRscType);
		return ERR_FAIL;
	}

	*pulRscId = ulRscId;
	return ERR_OK;
}

HERROR RLIB_ChangeResourceId (Handle_t hSession, HUINT32 ulActionId, RxRscType_e eRscType, HUINT32 ulRscId)
{
	HUINT32					 ulRscNum = 0;
	_rxInst_t				*pstInst = (_rxInst_t *)hSession;
	HERROR					 hErr;

	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	if (ulActionId >= pstInst->ulActionNum)
	{
		HxLOG_Error("ActionId (%d)\n", ulActionId);
		return ERR_FAIL;
	}

	hErr = RLIB_GetResourceNumber (hSession, eRscType, &ulRscNum);
	if ((ERR_OK != hErr) || (0 == ulRscNum))
	{
		HxLOG_Error("RLIB_GetResourceNumber err: eRscType(%d)\n", eRscType);
		return ERR_FAIL;
	}

	if (ulRscId < ulRscNum)
	{
		rxlib_SetResourceIdToResourceMap(pstInst, ulActionId, eRscType, ulRscId);
	}
	else
	{
		rxlib_SetResourceIdToResourceMap(pstInst, ulActionId, eRscType, RxRSCID_NULL);
	}

	rxlib_UpdateRefCountInResourceMap(pstInst, eRscType);
	return ERR_OK;
}

HERROR RLIB_GetActionIdByResourceId (Handle_t hSession, HUINT32 ulRscId, RxRscType_e eRscType, HUINT32 *pulActionId)
{
	HUINT32			 ulActionId;
	_rxInst_t		*pstInst = (_rxInst_t *)hSession;
	HUINT32			 ulFoundRscId;

	if (NULL == pulActionId)					{ return ERR_FAIL; }
	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	if ((eRscType >= eRxRSCTYPE_MAX) || (ulRscId >= RxMAX_RESOURCE_NUM))
	{
		return ERR_FAIL;
	}

	for (ulActionId = 0; ulActionId < pstInst->ulActionNum; ulActionId++)
	{
		ulFoundRscId = rxlib_GetResourceIdFromResourceMap(pstInst, ulActionId, eRscType);
		if ((RxRSCID_NULL != ulFoundRscId) && (ulFoundRscId == ulRscId))
		{
			*pulActionId = ulActionId;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR RLIB_GetRfInputAbility (Handle_t hSession, HUINT32 ulRfInputId, HBOOL *pbAbility)
{
	_rxInst_t		*pstInst = (_rxInst_t *)hSession;
	RxRscObject_t 	*pstRfInputRsc;

	if (NULL == pbAbility)						{ return ERR_FAIL; }
	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	pstRfInputRsc = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_RFINPUT, ulRfInputId);
	if (NULL == pstRfInputRsc)
	{
		return ERR_FAIL;
	}

	switch (pstRfInputRsc->utObj.stRfInput.eSupportedDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		{
			RxSatRfInputObj_t		*pstRfInputObj = &(pstRfInputRsc->utObj.stRfInput.utObj.stSatRf);

			*pbAbility = pstRfInputObj->bAvailable;
			return ERR_OK;
		}

		break;

	default:
		break;
	}

	return ERR_FAIL;
}

HERROR RLIB_SetRfInputAbility (Handle_t hSession, HUINT32 ulRfInputId, HBOOL bAbility)
{
	_rxInst_t		*pstInst = (_rxInst_t *)hSession;
	RxRscObject_t 	*pstRfInputRsc;

	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	pstRfInputRsc = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_RFINPUT, ulRfInputId);
	if (NULL == pstRfInputRsc)
	{
		return ERR_FAIL;
	}

	switch (pstRfInputRsc->utObj.stRfInput.eSupportedDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		{
			RxSatRfInputObj_t		*pstRfInputObj = &(pstRfInputRsc->utObj.stRfInput.utObj.stSatRf);

			pstRfInputObj->bAvailable = bAbility;
		}

		break;

	default:
		break;
	}

	return ERR_OK;
}

HERROR RLIB_GetResourceObject (Handle_t hSession, HUINT32 ulActionId, RxRscType_e eRscType, RxRscObject_t *pstRscObj)
{
	HUINT32			 ulRscId;
	_rxInst_t		*pstInst = (_rxInst_t *)hSession;
	RxRscObject_t	*pstObj;

	if (NULL == pstRscObj)					{ return ERR_FAIL; }
	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	ulRscId = rxlib_GetResourceIdFromResourceMap(pstInst, ulActionId, eRscType);
	pstObj = rxlib_GetResourceObject(pstInst, eRscType, ulRscId);
	if (NULL == pstObj)
	{
		HxLOG_Info ("No Resource(%d) in Action(%d)\n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstRscObj, pstObj, sizeof(RxRscObject_t));
	return ERR_OK;
}

HERROR RLIB_SetResourceObject (Handle_t hSession, HUINT32 ulActionId, RxRscType_e eRscType, RxRscObject_t *pstRscObj)
{
	RxRscObject_t	*pstObj;
	_rxInst_t		*pstInst = (_rxInst_t *)hSession;

	if (NULL == pstRscObj)					{ return ERR_FAIL; }
	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	if (pstRscObj->eRscType != eRscType)
	{
		HxLOG_Error("Resource Type and Object is different:\n");
		return ERR_FAIL;
	}

	pstObj = rxlib_GetResourceObject(pstInst, eRscType, pstRscObj->ulRscId);
	if (NULL == pstObj)
	{
		HxLOG_Error("Resource ID different\n");
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstObj, pstRscObj, sizeof(RxRscObject_t));

	rxlib_SetResourceIdToResourceMap(pstInst, ulActionId, eRscType, pstRscObj->ulRscId);
	rxlib_UpdateRefCountInResourceMap(pstInst, eRscType);

	return ERR_OK;
}

HERROR RLIB_GetResourceNumber (Handle_t hSession, RxRscType_e eRscType, HUINT32 *pulRscNum)
{
	_rxRscTable_t		*pstRscTable;
	_rxInst_t			*pstInst = (_rxInst_t *)hSession;

	if (NULL == pulRscNum)					{ return ERR_FAIL; }
	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	pstRscTable = rxlib_GetResourceObjectTable(pstInst, eRscType);
	if (NULL == pstRscTable)
	{
		HxLOG_Error("rxlib_GetResourceObjectTable err: eRscType(%d)\n", eRscType);
		return ERR_FAIL;
	}

	if (TRUE != pstRscTable->bInited)
	{
		HxLOG_Error("pstRscTable Not Inited: eRscType(%d)\n", eRscType);
		return ERR_FAIL;
	}

	*pulRscNum = pstRscTable->ulRscNum;
	return ERR_OK;
}

HERROR RLIB_GetRfInfoCapacity (Handle_t hSession, HUINT32 ulRscId, RxRfInfoCapacity_t *pstCapacity)
{
	RxRscObject_t		*pstRscObj;
	RxRfInputObject_t	*pstRfInput;
	_rxInst_t			*pstInst = (_rxInst_t *)hSession;

	if (NULL == pstCapacity)				{ return ERR_FAIL; }
	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	pstRscObj = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_RFINPUT, ulRscId);
	if (NULL == pstRscObj)
	{
		HxLOG_Error("rxlib_GetResourceObject NULL: eRscType:eRxRSCTYPE_RFINPUT, ulRscId(%d)\n", ulRscId);
		return ERR_FAIL;
	}

	pstRfInput = &(pstRscObj->utObj.stRfInput);

	pstCapacity->eSupportedDeliType		= pstRfInput->eSupportedDeliType;
	pstCapacity->ulConnectableTunerNum	= 0;

	if ( pstRfInput->eSupportedDeliType & eDxDELIVERY_TYPE_SAT)
	{
		RxSatRfInputObj_t		*pstSatRfInput = &(pstRfInput->utObj.stSatRf);

		pstCapacity->ulConnectableTunerNum		= pstSatRfInput->ulConnectableTunerNum;
		HxSTD_MemCopy(pstCapacity->aulConnectableTunerId, pstSatRfInput->aulConnectableTunerId, sizeof(HUINT32) * pstSatRfInput->ulConnectableTunerNum);
	}

	return ERR_OK;
}

HERROR RLIB_GetTunerCapacity (Handle_t hSession, HUINT32 ulRscId, DxDeliveryType_e *peSupportedDeliType)
{
	RxRscObject_t			*pstRscObj;
	RxTunerObject_t			*pstTuner;
	_rxInst_t				*pstInst = (_rxInst_t *)hSession;

	if (NULL == peSupportedDeliType)		{ return ERR_FAIL; }
	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	pstRscObj = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_TUNER_GROUP, ulRscId);
	if (NULL == pstRscObj)
	{
		HxLOG_Error("rxlib_GetResourceObject NULL: eRscType:eRxRSCTYPE_TUNER_GROUP, ulRscId(%d)\n", ulRscId);
		return ERR_FAIL;
	}

	pstTuner = &(pstRscObj->utObj.stTuner);
	*peSupportedDeliType = pstTuner->eSupportedDeliType;

	return ERR_OK;
}

HERROR RLIB_GetResourceUsageType (Handle_t hSession, RxRscType_e eRscType, HUINT32 ulRscId, RxUsageType_e *peUsageType)
{
	_rxInst_t				*pstInst = (_rxInst_t *)hSession;
	RxRscObject_t			*pstRscObj;

	if (NULL == peUsageType)				{ return ERR_FAIL; }
	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	pstRscObj = rxlib_GetResourceObject(pstInst, eRscType, ulRscId);
	if (NULL == pstRscObj)
	{
		HxLOG_Error("rxlib_GetResourceObject NULL: eRscType(%d), ulRscId(%d)\n", eRscType, ulRscId);
		return ERR_FAIL;
	}

	*peUsageType = pstRscObj->utObj.stTemplate.eUsageType;
	return ERR_OK;
}

HERROR RLIB_GetResourceReferenceCount (Handle_t hSession, RxRscType_e eRscType, HUINT32 ulRscId, HINT32 *pnRefCount)
{
	_rxInst_t				*pstInst = (_rxInst_t *)hSession;
	RxRscObject_t			*pstRscObj;

	if (NULL == pnRefCount)				{ return ERR_FAIL; }
	if (NULL == pstInst)
	{
		HxLOG_Error("hSession NULL\n");
		return ERR_FAIL;
	}

	pstRscObj = rxlib_GetResourceObject(pstInst, eRscType, ulRscId);
	if (NULL == pstRscObj)
	{
		HxLOG_Error("rxlib_GetResourceObject NULL: eRscType(%d), ulRscId(%d)\n", eRscType, ulRscId);
		return ERR_FAIL;
	}

	*pnRefCount = pstRscObj->nRefCount;
	return ERR_OK;
}

HERROR RLIB_GetPhysicalTuningParamInSatellite (HUINT32 ulLnbFreq, HUINT32 ulFreq, DxSat_Polarization_e ePolar, HBOOL b22kTone, HUINT32 *pulPhysicalFreq, HBOOL *pbPhysical22kTone)
{
	HBOOL			 bPhysical22kTone = FALSE;
	HUINT32			 ulActualLnbFreq = DxSAT_UNIVERSAL_LNB_FREQ;

	if (rxlib_GetActualLnbFrequency(ulLnbFreq, ulFreq, ePolar, b22kTone, &ulActualLnbFreq, &bPhysical22kTone) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if (NULL != pulPhysicalFreq)
	{
		*pulPhysicalFreq = (HUINT32)abs((HINT32)ulFreq - (HINT32)ulActualLnbFreq);
	}

	if (NULL != pbPhysical22kTone)
	{
		*pbPhysical22kTone = bPhysical22kTone;
	}

	return ERR_OK;
}

HERROR RLIB_GetActualLnbFrequency (HUINT32 ulLnbFreq, HUINT32 ulFreq, DxSat_Polarization_e ePolar, HUINT32 *pulActualLnbFreq)
{
	HUINT32			 ulActualLnbFreq = DxSAT_UNIVERSAL_LNB_FREQ;

	if (rxlib_GetActualLnbFrequency(ulLnbFreq, ulFreq, ePolar, FALSE, &ulActualLnbFreq, NULL) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if (NULL != pulActualLnbFreq)
	{
		*pulActualLnbFreq = ulActualLnbFreq;
	}

	return ERR_OK;
}

#define ___FOR_DEBUGGER___
#if defined(CONFIG_DEBUG)
STATIC HINT32 rlib_dbg_PrintRscInfo (void *pvArg)
{
	HCHAR		*szArg = NULL;
	HINT32		nPrintIndex = 0;

	if(pvArg != NULL)
	{
		szArg = HLIB_CMD_GetNextArg((HCHAR**)&pvArg);
		if (szArg != NULL)
		{
			(void)sscanf(szArg, "%d", &nPrintIndex);
		}
	}

	RLIB_PrintRscInfo((RxDbgPrintCallback_t)HLIB_CMD_Printf, nPrintIndex);
	return HxCMD_OK;
}

STATIC HERROR rlib_InitDebugCmd (void)
{
	STATIC HBOOL		 s_bFirstCmd = TRUE;

	if (TRUE == s_bFirstCmd)
	{
		HLIB_CMD_RegisterWord(hCmdHandle, rlib_dbg_PrintRscInfo,
							(HCHAR *)"print_rsc",
							(HCHAR *)"print Resource Info",
							(HCHAR *)"print_rsc");

		s_bFirstCmd = FALSE;
	}

	return ERR_OK;
}

STATIC void rlib_PrintRscInstance (_rxInst_t *pstInst, RxDbgPrintCallback_t pfPrintFunc)
{
	HUINT32			 ulActCnt, ulTunerCnt, ulRscIdx, ulRscTypeIdx;
	_rxRscTable_t	*pstRscTable;
	RxRscObject_t	*pstRscObj;

	pfPrintFunc("Resource Print: Instance [%s]\n", pstInst->szInstName);
	pfPrintFunc("Resource MAX NUM:\n");

	for (ulRscTypeIdx = 0; ulRscTypeIdx < eRxRSCTYPE_MAX; ulRscTypeIdx++)
	{
		pstRscTable = rxlib_GetResourceObjectTable(pstInst, ulRscTypeIdx);
		if (NULL != pstRscTable)
		{
			pfPrintFunc(" - [%s] : Max(%d)\n", RLIB_GetResourceStr (ulRscTypeIdx), pstRscTable->ulRscNum);
		}
	}

	pstRscTable = rxlib_GetResourceObjectTable(pstInst, eRxRSCTYPE_RFINPUT);
	if ((NULL != pstRscTable) && (pstRscTable->ulRscNum > 0))
	{
		pfPrintFunc("\n");
		pfPrintFunc("RF-Input Status:\n");

		for (ulRscIdx = 0; ulRscIdx < pstRscTable->ulRscNum; ulRscIdx++)
		{
			pstRscObj = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_RFINPUT, ulRscIdx);
			if (NULL == pstRscObj)					{ continue; }

			if (pstRscObj->utObj.stRfInput.eSupportedDeliType & eDxDELIVERY_TYPE_SAT)
			{
				RxSatRfInputObj_t	*pstSatRfInput = &(pstRscObj->utObj.stRfInput.utObj.stSatRf);

				pfPrintFunc(" - RF-Input(%d): Satellite:\n", ulRscIdx);
				pfPrintFunc("   Connectable Tuner = ");

				for (ulTunerCnt = 0; ulTunerCnt < pstSatRfInput->ulConnectableTunerNum; ulTunerCnt++)
				{
					pfPrintFunc("%d ", pstSatRfInput->aulConnectableTunerId[ulTunerCnt]);
				}

				if (TRUE == pstSatRfInput->bAvailable)
				{
					pfPrintFunc(" -> Available\n");
				}
				else
				{
					pfPrintFunc(" -> Not Available\n");
				}

				pfPrintFunc("	 Connected Tuner = ");

				for (ulActCnt = 0; ulActCnt < pstInst->ulActionNum; ulActCnt++)
				{
					HUINT32			 ulCheckRscId = rxlib_GetResourceIdFromResourceMap(pstInst, ulActCnt, eRxRSCTYPE_RFINPUT);
					RxRscObject_t	*pstChkRscObj = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_RFINPUT, ulCheckRscId);

					if ((NULL != pstChkRscObj) && (ulRscIdx == ulCheckRscId))
					{
						HUINT32 		 ulTunerId = rxlib_GetResourceIdFromResourceMap(pstInst, ulActCnt, eRxRSCTYPE_TUNER_GROUP);
						pstChkRscObj = rxlib_GetResourceObject(pstInst, eRxRSCTYPE_TUNER_GROUP, ulTunerId);

						if (NULL != pstChkRscObj)
						{
							pfPrintFunc("%d ", ulTunerId);
						}
					}
				}

				pfPrintFunc("%\n");
			}
			else
			{
				pfPrintFunc(" - RF-Input(%d): Non-Satellite:\n", ulRscIdx);
			}

		}
	}

	pfPrintFunc("\n");
	pfPrintFunc("Per Action Status:\n");
	for (ulActCnt = 0; ulActCnt < pstInst->ulActionNum; ulActCnt++)
	{
		pfPrintFunc(" - ACT[%d] Tuner(%d), Demux(%d), AV(%d), Player(%d), REC(%d), Transcoder(%d)\n", ulActCnt,
							(HINT32)rxlib_GetResourceIdFromResourceMap(pstInst, ulActCnt, eRxRSCTYPE_TUNER_GROUP),
							(HINT32)rxlib_GetResourceIdFromResourceMap(pstInst, ulActCnt, eRxRSCTYPE_DEMUX),
							(HINT32)rxlib_GetResourceIdFromResourceMap(pstInst, ulActCnt, eRxRSCTYPE_AV_DEC),
							(HINT32)rxlib_GetResourceIdFromResourceMap(pstInst, ulActCnt, eRxRSCTYPE_PVR_PLAYER),
							(HINT32)rxlib_GetResourceIdFromResourceMap(pstInst, ulActCnt, eRxRSCTYPE_HDD_REC),
							(HINT32)rxlib_GetResourceIdFromResourceMap(pstInst, ulActCnt, eRxRSCTYPE_TRANSCODER)
					);

		for (ulRscTypeIdx = 0; ulRscTypeIdx < eRxRSCTYPE_MAX; ulRscTypeIdx++)
		{
			HUINT32 		 ulCheckRscId = rxlib_GetResourceIdFromResourceMap(pstInst, ulActCnt, ulRscTypeIdx);
			RxRscObject_t	*pstChkRscObj = rxlib_GetResourceObject(pstInst, ulRscTypeIdx, ulCheckRscId);

			if (NULL != pstChkRscObj)
			{
				pfPrintFunc("    [%s] nRefCount(%d), bSharable(%d)",
						RLIB_GetResourceStr (ulRscTypeIdx), pstChkRscObj->nRefCount, pstChkRscObj->bSharable);
				switch (ulRscTypeIdx)
				{
				case eRxRSCTYPE_TUNER_GROUP:
					pfPrintFunc(", SupportedDeliType:0x%X, DeliType:0x%X\n", pstChkRscObj->utObj.stTuner.eSupportedDeliType,
																pstChkRscObj->utObj.stTuner.stTuneParam.eDeliType);
					break;

				case eRxRSCTYPE_AV_DEC:
				case eRxRSCTYPE_DEMUX:
					pfPrintFunc(", UsageType:0x%X\n", pstChkRscObj->utObj.stTemplate.eUsageType);
					break;

				default:
					pfPrintFunc("\n");
					break;
				}
			}
		}
	}

	pfPrintFunc("\n");
	pfPrintFunc("Per Resource Status:\n");
	for (ulRscTypeIdx = 0; ulRscTypeIdx < eRxRSCTYPE_MAX; ulRscTypeIdx++)
	{
		pstRscTable = rxlib_GetResourceObjectTable(pstInst, ulRscTypeIdx);
		if (NULL != pstRscTable)
		{
			for (ulRscIdx = 0; ulRscIdx < pstRscTable->ulRscNum; ulRscIdx++)
			{
				pstRscObj = rxlib_GetResourceObject(pstInst, ulRscTypeIdx, ulRscIdx);
				if (NULL != pstRscObj)
				{
					pfPrintFunc(" - [%s(%02d)] : RefCount(%d)\n", RLIB_GetResourceStr (ulRscTypeIdx), ulRscIdx, pstRscObj->nRefCount);
				}
			}
		}
	}

	pfPrintFunc("\n\n");
}


HCHAR *RLIB_GetResourceStr (RxRscType_e eRscType)
{
	switch(eRscType)
	{
	ENUM_TO_STR(eRxRSCTYPE_TUNER_GROUP);
	ENUM_TO_STR(eRxRSCTYPE_DEMUX);
	ENUM_TO_STR(eRxRSCTYPE_AV_DEC);
	ENUM_TO_STR(eRxRSCTYPE_PVR_PLAYER);
	ENUM_TO_STR(eRxRSCTYPE_HDD_REC);
	ENUM_TO_STR(eRxRSCTYPE_RFINPUT);
	ENUM_TO_STR(eRxRSCTYPE_TRANSCODER);
	default:
		break;
	}

	return "Unknown";
}

void RLIB_PrintRscInfo (RxDbgPrintCallback_t pfPrintFunc, HINT32 nPrintIndex)
{
	HxList_t		*pstInstItem;
	HINT32			nCount = 0;

	if (NULL == pfPrintFunc)			{ return; }

	for (pstInstItem = s_pstRxRscInstList; NULL != pstInstItem; pstInstItem = pstInstItem->next, nCount++)
	{
		if(-1 != nPrintIndex)
		{
			if(nPrintIndex != nCount)
				continue;
		}

		{
			_rxInst_t		*pstInst = (_rxInst_t *)HLIB_LIST_Data(pstInstItem);

			if (NULL != pstInst)
			{
				pfPrintFunc("-----------------------------------------------------\n");
				rlib_PrintRscInstance(pstInst, pfPrintFunc);
			}
		}
	}
}
#endif

/* end of file */
