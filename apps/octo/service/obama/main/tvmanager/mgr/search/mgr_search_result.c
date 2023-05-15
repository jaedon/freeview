/**
	@file     mgr_search_svc.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/01/15		initiated					JK Baek\n
	 2008/03/14		renamed					wjmoh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <db_common.h>
#include <db_param.h>
#include <db_svc.h>

#include <svc_si.h>
#include <svc_sys.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_search.h>
#include <pmgr_search.h>

#include "./local_include/_mgr_search.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */


#define	_________________________Body___________________________________

STATIC HERROR mgr_srchrst_ClearServiceNameList (HUINT32 ulActionId)
{
	MgrSearch_ResultContext_t	*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)						{ return ERR_FAIL; }

	if (NULL != pstContents->pstSvcArray)
	{
		OxAP_Free (pstContents->pstSvcArray);
		pstContents->pstSvcArray = NULL;
	}

	if (NULL != pstContents->pstOnTsIdArray)
	{
		OxAP_Free(pstContents->pstOnTsIdArray);
		pstContents->pstOnTsIdArray = NULL;
	}

	pstContents->ulNumSvc = 0;
	pstContents->ulSvcArraySize = 0;

	pstContents->ulNumOnTs = 0;
	pstContents->ulOnTsArraySize = 0;

	return ERR_OK;
}


#define	_____GLOBAL_FUNCTIONS_____

#define _____SEARCHED_SERVICES_INFO_____

HERROR MGR_SEARCHDATA_InitProviderArray (MgrSearch_PrvInfoArray_t *pstArray)
{
	DbSvc_PrvInfo_t	*pstInfo;

	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	HxSTD_memset (pstArray, 0, sizeof(MgrSearch_PrvInfoArray_t));

	pstInfo = (DbSvc_PrvInfo_t *)OxAP_Malloc (sizeof(DbSvc_PrvInfo_t) * DEFAULT_NUM_PROVIDER);
	if (pstInfo == NULL)					{ return ERR_BUS_OUT_OF_MEMORY; }

	HxSTD_memset (pstInfo, 0, sizeof(DbSvc_PrvInfo_t) * DEFAULT_NUM_PROVIDER);

	pstArray->ulCurrArraySize = DEFAULT_NUM_PROVIDER;
	pstArray->astProviderInfo = pstInfo;

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_ClearProviderArray (MgrSearch_PrvInfoArray_t *pstArray)
{
	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (pstArray->astProviderInfo != NULL)
	{
		OxAP_Free (pstArray->astProviderInfo);
	}

	HxSTD_memset (pstArray, 0, sizeof(MgrSearch_PrvInfoArray_t));

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_AddProvider (MgrSearch_PrvInfoArray_t *pstArray, DbSvc_PrvInfo_t *pstInfo)
{
	HUINT32			 ulNewSize;
	DbSvc_PrvInfo_t	*pstNewInfo;

	if (pstInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }
	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (pstArray->ulProviderNum >= pstArray->ulCurrArraySize)
	{
		ulNewSize = pstArray->ulCurrArraySize + APPEND_NUM_PROVIDER;

		pstNewInfo = (DbSvc_PrvInfo_t *)OxAP_Malloc (sizeof(DbSvc_PrvInfo_t) * ulNewSize);
		if (pstNewInfo == NULL) 				   { return ERR_BUS_OUT_OF_MEMORY; }

		HxSTD_memset (pstNewInfo, 0, sizeof(DbSvc_PrvInfo_t) * ulNewSize);

		if (pstArray->astProviderInfo != NULL)
		{
			HxSTD_memcpy (pstNewInfo, pstArray->astProviderInfo, sizeof(DbSvc_PrvInfo_t) * pstArray->ulCurrArraySize);
			OxAP_Free (pstArray->astProviderInfo);
		}

		pstArray->ulCurrArraySize = ulNewSize;
		pstArray->astProviderInfo = pstNewInfo;
	}

	HxSTD_memcpy (&(pstArray->astProviderInfo[pstArray->ulProviderNum]), pstInfo, sizeof(DbSvc_PrvInfo_t));
	pstArray->ulProviderNum++;

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_CountProvider (MgrSearch_PrvInfoArray_t *pstArray, HUINT32 *pulNum)
{
	if (pstArray == NULL)					{ return ERR_BUS_SOURCE_NULL; }
	if (pulNum == NULL)						{ return ERR_BUS_TARGET_NULL; }

	*pulNum = pstArray->ulProviderNum;
	return ERR_BUS_NO_ERROR;
}

DbSvc_PrvInfo_t *MGR_SEARCHDATA_GetProvider (MgrSearch_PrvInfoArray_t *pstArray, HUINT32 ulIndex)
{
	if (pstArray != NULL)
	{
		if (ulIndex < pstArray->ulProviderNum)
		{
			return &(pstArray->astProviderInfo[ulIndex]);
		}
	}

	return NULL;
}


HERROR MGR_SEARCHDATA_InitNetworkArray (MgrSearch_NetInfoArray_t *pstArray)
{
	HUINT32			*pulNetIdx;
	DbSvc_NetInfo_t	*pstInfo;

	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	HxSTD_memset (pstArray, 0, sizeof(MgrSearch_NetInfoArray_t));

	pstInfo = (DbSvc_NetInfo_t *)OxAP_Malloc (sizeof(DbSvc_NetInfo_t) * DEFAULT_NUM_NETWORKS);
	pulNetIdx = (HUINT32 *)OxAP_Malloc (sizeof(HUINT32) * DEFAULT_NUM_NETWORKS);
	if (pstInfo == NULL || pulNetIdx == NULL)
	{
		if (pstInfo != NULL)				{ OxAP_Free (pstInfo); }
		if (pulNetIdx != NULL)				{ OxAP_Free (pulNetIdx); }

		return ERR_BUS_OUT_OF_MEMORY;
	}

	HxSTD_memset (pstInfo, 0, sizeof(DbSvc_NetInfo_t) * DEFAULT_NUM_NETWORKS);
	HxSTD_memset (pulNetIdx, 0, sizeof(HUINT32) * DEFAULT_NUM_NETWORKS);

	pstArray->ulCurrArraySize = DEFAULT_NUM_NETWORKS;
	pstArray->aulNetIdx = pulNetIdx;
	pstArray->astNetworkInfo = pstInfo;

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_ClearNetworkArray (MgrSearch_NetInfoArray_t *pstArray)
{
	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (pstArray->astNetworkInfo != NULL)
	{
		OxAP_Free (pstArray->astNetworkInfo);
	}

	if (pstArray->aulNetIdx != NULL)
	{
		OxAP_Free (pstArray->aulNetIdx);
	}

	HxSTD_memset (pstArray, 0, sizeof(MgrSearch_NetInfoArray_t));

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_AddNetwork (MgrSearch_NetInfoArray_t *pstArray, DbSvc_NetInfo_t *pstInfo, HUINT32 *pulIndex)
{
	HUINT32			 ulNewSize;
	HUINT32			*pulNewNetIdx;
	DbSvc_NetInfo_t	*pstNewInfo;

	if (pstInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }
	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (pstArray->ulNetworkNum >= pstArray->ulCurrArraySize)
	{
		ulNewSize = pstArray->ulCurrArraySize + APPEND_NUM_NETWORKS;

		pstNewInfo = (DbSvc_NetInfo_t *)OxAP_Malloc (sizeof(DbSvc_NetInfo_t) * ulNewSize);
		pulNewNetIdx = (HUINT32 *)OxAP_Malloc (sizeof(HUINT32) * ulNewSize);
		if (pstNewInfo == NULL || pulNewNetIdx == NULL)
		{
			if (pstNewInfo != NULL)				{ OxAP_Free (pstNewInfo); }
			if (pulNewNetIdx != NULL)			{ OxAP_Free (pulNewNetIdx); }

			return ERR_BUS_OUT_OF_MEMORY;
		}

		HxSTD_memset (pstNewInfo, 0, sizeof(DbSvc_NetInfo_t) * ulNewSize);
		if (pstArray->astNetworkInfo != NULL)
		{
			HxSTD_memcpy (pstNewInfo, pstArray->astNetworkInfo, sizeof(DbSvc_NetInfo_t) * pstArray->ulCurrArraySize);
			OxAP_Free (pstArray->astNetworkInfo);
		}
		pstArray->astNetworkInfo = pstNewInfo;

		HxSTD_memset (pulNewNetIdx, 0, sizeof(HUINT32) * ulNewSize);
		if (pstArray->aulNetIdx != NULL)
		{
			HxSTD_memcpy (pulNewNetIdx, pstArray->aulNetIdx, sizeof(HUINT32) * pstArray->ulCurrArraySize);
			OxAP_Free (pstArray->aulNetIdx);
		}
		pstArray->aulNetIdx = pulNewNetIdx;

		pstArray->ulCurrArraySize = ulNewSize;
	}

	if (pulIndex != NULL)					{ *pulIndex = pstArray->ulNetworkNum; }
	HxSTD_memcpy (&(pstArray->astNetworkInfo[pstArray->ulNetworkNum]), pstInfo, sizeof(DbSvc_NetInfo_t));
	pstArray->ulNetworkNum++;

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_CountNetwork (MgrSearch_NetInfoArray_t *pstArray, HUINT32 *pulNum)
{
	if (pstArray == NULL)					{ return ERR_BUS_SOURCE_NULL; }
	if (pulNum == NULL)						{ return ERR_BUS_TARGET_NULL; }

	*pulNum = pstArray->ulNetworkNum;
	return ERR_BUS_NO_ERROR;
}

DbSvc_NetInfo_t *MGR_SEARCHDATA_GetNetwork (MgrSearch_NetInfoArray_t *pstArray, HUINT32 ulIndex)
{
	if (pstArray != NULL)
	{
		if (ulIndex < pstArray->ulNetworkNum)
		{
			return &(pstArray->astNetworkInfo[ulIndex]);
		}
	}

	return NULL;
}


HERROR MGR_SEARCHDATA_InitTsArray (MgrSearch_TsInfoArray_t *pstArray)
{
	DbSvc_TsInfo_t	*pstInfo;

	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	HxSTD_memset (pstArray, 0, sizeof(MgrSearch_TsInfoArray_t));

	pstInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t) * DEFAULT_NUM_TS);
	if (pstInfo == NULL)					{ return ERR_BUS_OUT_OF_MEMORY; }

	HxSTD_memset (pstInfo, 0, sizeof(DbSvc_TsInfo_t) * DEFAULT_NUM_TS);

	pstArray->ulCurrArraySize = DEFAULT_NUM_TS;
	pstArray->astTsInfo = pstInfo;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_ClearTsArray (MgrSearch_TsInfoArray_t *pstArray)
{
	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (pstArray->astTsInfo != NULL)
	{
		OxAP_Free (pstArray->astTsInfo);
	}

	HxSTD_memset (pstArray, 0, sizeof(MgrSearch_TsInfoArray_t));

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_AddTs (MgrSearch_TsInfoArray_t *pstArray, DbSvc_TsInfo_t *pstInfo, HUINT32 *pulIndex)
{
	HUINT32			 ulNewSize;
	DbSvc_TsInfo_t	*pstNewInfo;

	if (pstInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }
	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (pstArray->ulTsNum >= pstArray->ulCurrArraySize)
	{
		ulNewSize = pstArray->ulCurrArraySize + APPEND_NUM_TS;

		pstNewInfo = (DbSvc_TsInfo_t *)OxAP_Malloc (sizeof(DbSvc_TsInfo_t) * ulNewSize);
		if (pstNewInfo == NULL) 				   { return ERR_BUS_OUT_OF_MEMORY; }

		HxSTD_memset (pstNewInfo, 0, sizeof(DbSvc_TsInfo_t) * ulNewSize);

		if (pstArray->astTsInfo != NULL)
		{
			HxSTD_memcpy (pstNewInfo, pstArray->astTsInfo, sizeof(DbSvc_TsInfo_t) * pstArray->ulCurrArraySize);
			OxAP_Free (pstArray->astTsInfo);
		}

		pstArray->ulCurrArraySize = ulNewSize;
		pstArray->astTsInfo = pstNewInfo;
	}

	if (pulIndex != NULL)					{ *pulIndex = pstArray->ulTsNum; }
	HxSTD_memcpy (&(pstArray->astTsInfo[pstArray->ulTsNum]), pstInfo, sizeof(DbSvc_TsInfo_t));
	pstArray->ulTsNum++;

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_CountTs (MgrSearch_TsInfoArray_t *pstArray, HUINT32 *pulNum)
{
	if (pstArray == NULL)					{ return ERR_BUS_SOURCE_NULL; }
	if (pulNum == NULL)						{ return ERR_BUS_TARGET_NULL; }

	*pulNum = pstArray->ulTsNum;
	return ERR_BUS_NO_ERROR;
}

DbSvc_TsInfo_t *MGR_SEARCHDATA_GetTsInfo (MgrSearch_TsInfoArray_t *pstArray, HUINT32 ulIndex)
{
	if (pstArray != NULL)
	{
		if (ulIndex < pstArray->ulTsNum)
		{
			return &(pstArray->astTsInfo[ulIndex]);
		}
	}

	return NULL;
}

HERROR MGR_SEARCHDATA_InitBouquetArray (MgrSearch_BqInfoArray_t *pstArray)
{
	DbSvc_BouqInfo_t	*pstInfo;

	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	HxSTD_memset (pstArray, 0, sizeof(MgrSearch_BqInfoArray_t));

	pstInfo = (DbSvc_BouqInfo_t *)OxAP_Malloc (sizeof(DbSvc_BouqInfo_t) * DEFAULT_NUM_BOUQUET);
	if (pstInfo == NULL)					{ return ERR_BUS_OUT_OF_MEMORY; }

	HxSTD_memset (pstInfo, 0, sizeof(DbSvc_BouqInfo_t) * DEFAULT_NUM_BOUQUET);

	pstArray->ulCurrArraySize = DEFAULT_NUM_BOUQUET;
	pstArray->astBqInfo = pstInfo;
	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_ClearBouquetArray (MgrSearch_BqInfoArray_t *pstArray)
{
	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (pstArray->astBqInfo != NULL)
	{
		OxAP_Free (pstArray->astBqInfo);
	}

	HxSTD_memset (pstArray, 0, sizeof(MgrSearch_BqInfoArray_t));

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_AddBouquet (MgrSearch_BqInfoArray_t *pstArray, DbSvc_BouqInfo_t *pstInfo)
{
	HUINT32			 ulNewSize;
	DbSvc_BouqInfo_t	*pstNewInfo;

	if (pstInfo == NULL)					{ return ERR_BUS_SOURCE_NULL; }
	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (pstArray->ulBouquetNum >= pstArray->ulCurrArraySize)
	{
		ulNewSize = pstArray->ulCurrArraySize + APPEND_NUM_BOUQUET;

		pstNewInfo = (DbSvc_BouqInfo_t *)OxAP_Malloc (sizeof(DbSvc_BouqInfo_t) * ulNewSize);
		if (pstNewInfo == NULL) 				   { return ERR_BUS_OUT_OF_MEMORY; }

		HxSTD_memset (pstNewInfo, 0, sizeof(DbSvc_BouqInfo_t) * ulNewSize);

		if (pstArray->astBqInfo != NULL)
		{
			HxSTD_memcpy (pstNewInfo, pstArray->astBqInfo, sizeof(DbSvc_BouqInfo_t) * pstArray->ulCurrArraySize);
			OxAP_Free (pstArray->astBqInfo);
		}

		pstArray->ulCurrArraySize = ulNewSize;
		pstArray->astBqInfo = pstNewInfo;
	}

	HxSTD_memcpy (&(pstArray->astBqInfo[pstArray->ulBouquetNum]), pstInfo, sizeof(DbSvc_BouqInfo_t));
	pstArray->ulBouquetNum++;

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_CountBouquet (MgrSearch_BqInfoArray_t *pstArray, HUINT32 *pulNum)
{
	if (pstArray == NULL)					{ return ERR_BUS_SOURCE_NULL; }
	if (pulNum == NULL)						{ return ERR_BUS_TARGET_NULL; }

	*pulNum = pstArray->ulBouquetNum;
	return ERR_BUS_NO_ERROR;
}

DbSvc_BouqInfo_t *MGR_SEARCHDATA_GetBouquet (MgrSearch_BqInfoArray_t *pstArray, HUINT32 ulIndex)
{
	if (pstArray != NULL)
	{
		if (ulIndex < pstArray->ulBouquetNum)
		{
			return &(pstArray->astBqInfo[ulIndex]);
		}
	}

	return NULL;
}

#if 0	// 사용하는 곳 없어 막음
HERROR MGR_SEARCHDATA_InitServiceArray (MgrSearch_SvcInfoArray_t *pstArray)
{
	DbSvc_Info_t		*pstSvcInfo;

	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	HxSTD_memset (pstArray, 0, sizeof(MgrSearch_SvcInfoArray_t));

	pstSvcInfo = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * DEFAULT_NUM_SVC);
	if (pstSvcInfo == NULL)
	{
		return ERR_BUS_OUT_OF_MEMORY;
	}

	HxSTD_memset (pstSvcInfo, 0, sizeof(DbSvc_Info_t) * DEFAULT_NUM_SVC);

	pstArray->ulCurrArraySize = DEFAULT_NUM_SVC;
	pstArray->astSvcInfo = pstSvcInfo;

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_ClearServiceArray (MgrSearch_SvcInfoArray_t *pstArray)
{
	if (pstArray == NULL)					{ return ERR_BUS_TARGET_NULL; }

	if (pstArray->astSvcInfo != NULL)
	{
		OxAP_Free (pstArray->astSvcInfo);
	}

	HxSTD_memset (pstArray, 0, sizeof(MgrSearch_SvcInfoArray_t));

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_AddService (MgrSearch_SvcInfoArray_t *pstArray, DbSvc_Info_t *pstSvcInfo)
{
	HUINT32			 ulNewSize;
	DbSvc_Info_t		*pstNewSvcInfo;

	if (pstSvcInfo == NULL)								{ return ERR_BUS_SOURCE_NULL; }
	if (pstArray == NULL)								{ return ERR_BUS_TARGET_NULL; }

	if (pstArray->ulServiceNum >= pstArray->ulCurrArraySize)
	{
		ulNewSize = pstArray->ulCurrArraySize + APPEND_NUM_SVC;

		pstNewSvcInfo = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * ulNewSize);
		if (pstNewSvcInfo == NULL)
		{
			return ERR_BUS_OUT_OF_MEMORY;
		}

		HxSTD_memset (pstNewSvcInfo, 0, sizeof(DbSvc_Info_t) * ulNewSize);
		if (pstArray->astSvcInfo != NULL)
		{
			HxSTD_memcpy (pstNewSvcInfo, pstArray->astSvcInfo, sizeof(DbSvc_Info_t) * pstArray->ulCurrArraySize);
			OxAP_Free (pstArray->astSvcInfo);
		}
		pstArray->astSvcInfo = pstNewSvcInfo;

		pstArray->ulCurrArraySize = ulNewSize;
	}

	HxSTD_memcpy (&(pstArray->astSvcInfo[pstArray->ulServiceNum]), pstSvcInfo, sizeof(DbSvc_Info_t));
	pstArray->ulServiceNum++;

	return ERR_BUS_NO_ERROR;
}

HERROR MGR_SEARCHDATA_CountService (MgrSearch_SvcInfoArray_t *pstArray, HUINT32 *pulNum)
{
	if (pstArray == NULL)					{ return ERR_BUS_SOURCE_NULL; }
	if (pulNum == NULL)						{ return ERR_BUS_TARGET_NULL; }

	*pulNum = pstArray->ulServiceNum;
	return ERR_BUS_NO_ERROR;
}

DbSvc_Info_t *MGR_SEARCHDATA_GetService (MgrSearch_SvcInfoArray_t *pstArray, HUINT32 ulIndex)
{
	if (pstArray != NULL)
	{
		if (ulIndex < pstArray->ulServiceNum)
		{
			return &(pstArray->astSvcInfo[ulIndex]);
		}
	}

	return NULL;
}
#endif


#define _____SEARCH_DISPLAY_INFO_____

HERROR MGR_SEARCH_InitResult (HUINT32 ulActionId)
{
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	MGR_SEARCH_ClearResult (ulActionId);
	return ERR_OK;
}

HERROR MGR_SEARCH_ClearResult (HUINT32 ulActionId)
{
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	mgr_srchrst_ClearServiceNameList (ulActionId);

	HxSTD_memset (pstContents, 0, sizeof(MgrSearch_ResultContext_t));

	return ERR_OK;
}

HERROR MGR_SEARCH_ClearSearchSvcList (HUINT32 ulActionId)
{
	mgr_srchrst_ClearServiceNameList (ulActionId);

	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_GetActionState (HUINT32 ulActionId, MgrSearch_AppState_e *peState)
{
	MgrSearch_ResultContext_t		*pstContents;

	if (NULL == peState)									{ return ERR_FAIL; }

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	*peState = pstContents->eAppState;
	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_SetActionState (HUINT32 ulActionId, MgrSearch_AppState_e eState)
{
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	pstContents->eAppState = eState;
	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_GetNetwork (HUINT32 ulActionId, DbSvc_NetInfo_t *pstNetInfo)
{
	MgrSearch_ResultContext_t		*pstContents;

	if (pstNetInfo == NULL)									{ return ERR_FAIL; }

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	*pstNetInfo		= pstContents->stNetwork;

	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_SetNetwork (HUINT32 ulActionId, DbSvc_NetInfo_t *pstNetInfo)
{
	MgrSearch_ResultContext_t		*pstContents;

	if (NULL == pstNetInfo)									{ return ERR_FAIL; }

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	HxSTD_MemCopy(&pstContents->stNetwork, pstNetInfo, sizeof(DbSvc_NetInfo_t));

	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_GetTransponder (HUINT32 ulActionId, DxDeliveryType_e *peChType, DbSvc_TuningInfo_t *pstTuneInfo)
{
	MgrSearch_ResultContext_t		*pstContents;

	if (peChType == NULL || pstTuneInfo == NULL)			{ return ERR_FAIL; }

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	*peChType		= pstContents->eChType;
	*pstTuneInfo	= pstContents->stTransponder;

	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_SetTransponder (HUINT32 ulActionId, DxDeliveryType_e eChType, DbSvc_TuningInfo_t stTuneInfo)
{
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	pstContents->eChType		= eChType;
	pstContents->stTransponder	= stTuneInfo;

	return ERR_OK;
}


HERROR MGR_SEARCHRESULT_GetNumTpCompleted (HUINT32 ulActionId, HUINT32 *pulNumTp)
{
	MgrSearch_ResultContext_t		*pstContents;

	if (NULL == pulNumTp)									{ return ERR_FAIL; }

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	*pulNumTp = pstContents->ulCurrTpCnt;
	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_SetNumTpCompleted (HUINT32 ulActionId, HUINT32 ulTpNum)
{
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	pstContents->ulCurrTpCnt = ulTpNum;

	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_GetNumTpTotal (HUINT32 ulActionId, HUINT32 *pulNumTp)
{
	MgrSearch_ResultContext_t		*pstContents;

	if (NULL == pulNumTp)									{ return ERR_FAIL; }

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	*pulNumTp = pstContents->ulTotalTpNum;
	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_SetNumTpTotal (HUINT32 ulActionId, HUINT32 ulTpNum)
{
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	pstContents->ulTotalTpNum = ulTpNum;

	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_GetAntennaInfo (HUINT32 ulActionId, DbSvc_AntInfo_t *pstAntInfo)
{
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	*pstAntInfo = pstContents->stAntennaInfo;
	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_SetAntennaInfo (HUINT32 ulActionId, DbSvc_AntInfo_t stAntInfo)
{
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	pstContents->stAntennaInfo = stAntInfo;

	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_GetAntennaId (HUINT32 ulActionId, HUINT32 *pulAntId) // MOONSG_sss
{
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	*pulAntId = pstContents->ulAntId;
	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_SetAntennaId (HUINT32 ulActionId, HUINT32 ulAntId)
{
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	pstContents->ulAntId = ulAntId;

	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_AddOnTsId (HUINT32 ulActionId, HUINT16 usOnId, HUINT16 usTsId)
{
	HUINT32							 ulCnt;
	DbSvc_TripleId_t				*pstOnTsId;
	MgrSearch_ResultContext_t		*pstContents;

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	if (pstContents->ulNumOnTs >= pstContents->ulOnTsArraySize)
	{
		HUINT32				 ulNewSize = pstContents->ulOnTsArraySize + 50;
		DbSvc_TripleId_t	*pstOnTsArray;

		pstOnTsArray = (DbSvc_TripleId_t *)OxAP_Malloc (sizeof(DbSvc_TripleId_t) * ulNewSize);
		if ((NULL != pstContents->pstOnTsIdArray) && (pstContents->ulNumOnTs > 0))
		{
			HxSTD_memcpy (pstOnTsArray, pstContents->pstOnTsIdArray, sizeof(DbSvc_TripleId_t) *(pstContents->ulNumOnTs));
			OxAP_Free (pstContents->pstOnTsIdArray);
		}

		pstContents->ulOnTsArraySize = ulNewSize;
		pstContents->pstOnTsIdArray = pstOnTsArray;
	}

	for (ulCnt = 0; ulCnt < pstContents->ulNumOnTs; ulCnt++)
	{
		pstOnTsId = &(pstContents->pstOnTsIdArray[ulCnt]);

		if ((usOnId == pstOnTsId->usOnId) &&
			(usTsId == pstOnTsId->usTsId))
		{
			return ERR_OK;
		}
	}

	pstOnTsId = &(pstContents->pstOnTsIdArray[pstContents->ulNumOnTs]);
	pstOnTsId->usOnId = usOnId;
	pstOnTsId->usTsId = usTsId;
	pstOnTsId->usSvcId = 0;
	pstContents->ulNumOnTs ++;

	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_AddService (HUINT32 ulActionId, DbSvc_Info_t *pstSvcInfo)
{
	HBOOL							 bCheckSameSvc = FALSE;
	HUINT32							 ulCnt;
	MgrSearch_ResultContext_t		*pstContents;

	if (NULL == pstSvcInfo)									{ return ERR_FAIL; }

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	if (pstContents->ulNumSvc >= pstContents->ulSvcArraySize)
	{
		HUINT32			 ulNewSize = pstContents->ulSvcArraySize + 250;
		DbSvc_Info_t		*pstSvcArray;

		pstSvcArray = (DbSvc_Info_t *)OxAP_Malloc (sizeof(DbSvc_Info_t) * ulNewSize);
		if ((NULL != pstContents->pstSvcArray) && (pstContents->ulNumSvc > 0))
		{
			HxSTD_memcpy (pstSvcArray, pstContents->pstSvcArray, sizeof(DbSvc_Info_t) *(pstContents->ulNumSvc));
			OxAP_Free (pstContents->pstSvcArray);
		}

		pstContents->ulSvcArraySize = ulNewSize;
		pstContents->pstSvcArray = pstSvcArray;
	}

	for (ulCnt = 0; ulCnt < pstContents->ulNumOnTs; ulCnt++)
	{
		DbSvc_TripleId_t		*pstOnTsId = &(pstContents->pstOnTsIdArray[ulCnt]);

		if ((DbSvc_GetOnId(pstSvcInfo) == pstOnTsId->usOnId) &&
			(DbSvc_GetTsId(pstSvcInfo) == pstOnTsId->usTsId))
		{
			bCheckSameSvc = TRUE;
			break;
		}
	}

	if (TRUE == bCheckSameSvc)
	{
		for (ulCnt = 0; ulCnt < pstContents->ulNumSvc; ulCnt++)
		{
			if (DbSvc_GetSvcIdx(&pstContents->pstSvcArray[ulCnt]) == DbSvc_GetSvcIdx(pstSvcInfo))
			{
				return ERR_OK;
			}
		}
	}

	pstContents->pstSvcArray[pstContents->ulNumSvc] = *pstSvcInfo;
	pstContents->ulNumSvc ++;

	return ERR_OK;
}

HERROR MGR_SEARCHRESULT_CountService (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 *pulSvcNum)
{
	if (NULL == pulSvcNum)
		return ERR_FAIL;

	return pmgr_search_ResultCountService (ulActionId, eSvcType, pulSvcNum);
}

HERROR MGR_SEARCHRESULT_GetService (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 ulIndex, DbSvc_Info_t *pstSvcInfo)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pmgr_search_ResultGetService (ulActionId, eSvcType, ulIndex, pstSvcInfo);
}

HERROR MGR_SEARCHRESULT_GetServiceList (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 ulArrayNum, DbSvc_Info_t *pstSvcArray, HUINT32 *pulSvcNum)
{
	HUINT32					 ulCount, ulSvcCnt = 0;
	MgrSearch_ResultContext_t	*pstContents;

	if ((NULL == pstSvcArray) || (0 == ulArrayNum))			{ return ERR_FAIL; }

	pstContents = mgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	for (ulCount = 0, ulSvcCnt = 0; ulCount < pstContents->ulNumSvc; ulCount++)
	{
		if ((eDxSVC_TYPE_All == eSvcType) ||
			((DbSvc_GetSvcType(&pstContents->pstSvcArray[ulCount]) & eSvcType) != 0))
		{
			if (ulSvcCnt < ulArrayNum)
			{
				pstSvcArray[ulSvcCnt] = pstContents->pstSvcArray[ulCount];
			}

			ulSvcCnt ++;
		}
	}

	if (0 == ulSvcCnt)
	{
		return ERR_FAIL;
	}

	if (NULL != pulSvcNum)
	{
		*pulSvcNum = ulSvcCnt;
	}

	return ERR_OK;
}

/* End of File. ---------------------------------------------------------- */


