/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_service.c
	@brief

	Description:  									\n
	Module: SAMA		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <hlib.h>
#include <vkernel.h>

#include <sama_lib.h>
#include "./local_include/sama_lib_int.h"


/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___PRIVATE_FUNCTIONS_DECLARATION___
/*******************************************************************/
/********************      Private Functions Declaration    ********************/
/*******************************************************************/

#define ___PRIVATE_FUNCTIONS___
/*******************************************************************/
/********************      Private Functions    ********************/
/*******************************************************************/
STATIC void samalib_svc_cbGetSvcInfo (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t		 stIter;
	DxService_t			*pstDstSvc = (DxService_t *)userdata;

	DAPI_InitIterator(&stIter, data, size, 0);
	while (DAPI_NextIterator(&stIter))
	{
		DxService_t			*pstSrcSvc = (DxService_t *)stIter.data;

		if (stIter.uid != (HUID)pstSrcSvc->uid)
		{
			HxLOG_Error("Getter failed: Wrong UID : (%d, %d)\n", stIter.uid, pstSrcSvc->uid);
			continue;
		}

		SAMA_MemCpy(pstDstSvc, pstSrcSvc, sizeof(DxService_t));
	}
}

STATIC void samalib_svc_cbGetTsInfo (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t		 stIter;
	DxTransponder_t		*pstDstTs = (DxTransponder_t *)userdata;

	DAPI_InitIterator(&stIter, data, size, 0);
	while (DAPI_NextIterator(&stIter))
	{
		DxTransponder_t 	*pstSrcTs = (DxTransponder_t *)stIter.data;

		if (stIter.uid != (HUID)pstSrcTs->uid)
		{
			HxLOG_Error("Getter failed: Wrong UID : (%d, %d)\n", stIter.uid, pstSrcTs->uid);
			continue;
		}

		SAMA_MemCpy (pstDstTs, pstSrcTs, sizeof(DxTransponder_t));
	}
}

STATIC void samalib_svc_cbGetAntInfo (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t		 stIter;
	DxAntInfo_t		*pstDstAnt = (DxAntInfo_t *)userdata;

	DAPI_InitIterator(&stIter, data, size, 0);
	while (DAPI_NextIterator(&stIter))
	{
		DxAntInfo_t 	*pstSrcAnt = (DxAntInfo_t *)stIter.data;

		if (stIter.uid != (HUID)pstSrcAnt->uid)
		{
			HxLOG_Error("Getter failed: Wrong UID : (%d, %d)\n", stIter.uid, pstSrcAnt->uid);
			continue;
		}

		SAMA_MemCpy (pstDstAnt, pstSrcAnt, sizeof(DxAntInfo_t));
	}
}
STATIC void samalib_svc_cbGetNetInfo (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAxIterator_t		 stIter;
	DxNetwork_t		*pstDstNet = (DxNetwork_t *)userdata;

	DAPI_InitIterator(&stIter, data, size, 0);
	while (DAPI_NextIterator(&stIter))
	{
		DxNetwork_t 	*pstSrcNet = (DxNetwork_t *)stIter.data;

		if (stIter.uid != (HUID)pstSrcNet->uid)
		{
			HxLOG_Error("Getter failed: Wrong UID : (%d, %d)\n", stIter.uid, pstSrcNet->uid);
			continue;
		}

		SAMA_MemCpy (pstDstNet, pstSrcNet, sizeof(DxNetwork_t));
	}
}


#define ___MEMBER_FUNCTIONS___
/*******************************************************************/
/********************      Member Functions       ******************/
/*******************************************************************/
sama_ServiceCache_t *samalib_svc_GetCache (sama_ConflictCheck_t *pstInst)
{
	if (NULL != pstInst)
	{
		sama_ServiceCache_t		*pstCache = &(pstInst->stSvcCache);

		if (TRUE != pstCache->bInited)
		{
			HUINT32				 ulSemId;
			HERROR				 hErr;

			hErr = HxSEMT_Create(&ulSemId, "sapi_svcsem", HxSEMT_FIFO);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("HxSEMT_Create err:\n");
				return NULL;
			}

			pstCache->bInited		= TRUE;
			pstCache->ulSemId		= ulSemId;
			pstCache->pstSvcList	= NULL;
			pstCache->pstTsList		= NULL;
			pstCache->pstNetList	= NULL;
			pstCache->pstAntInfoList = NULL;
		}

		return pstCache;
	}

	return NULL;
}

HERROR samalib_svc_GetService (sama_ConflictCheck_t *pstInst, HUINT32 ulSvcUid, DxService_t *pstService)
{
	HBOOL					 bFoundCache = FALSE;
	HUINT32					 ulDapiReqId;
	HxList_t				*pstListItem;
	DxService_t				*pstSrcSvc = NULL;
	sama_ServiceCache_t		*pstCache = samalib_svc_GetCache(pstInst);
	HERROR					 hErr;

	if (NULL == pstCache)				{ return ERR_FAIL; }
	if (NULL == pstService)				{ return ERR_FAIL; }

	SAMA_MemSet(pstService, 0, sizeof(DxService_t));

	HxSEMT_Get(pstCache->ulSemId);

	for (pstListItem = pstCache->pstSvcList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstSrcSvc = (DxService_t *)HLIB_LIST_Data(pstListItem);
		if ((NULL != pstSrcSvc) && (ulSvcUid == pstSrcSvc->uid))
		{
			bFoundCache = TRUE;
			break;
		}
	}

	HxSEMT_Release(pstCache->ulSemId);

	if (TRUE == bFoundCache)
	{
		SAMA_MemCpy(pstService, pstSrcSvc, sizeof(DxService_t));
		return ERR_OK;
	}

	ulDapiReqId = DAPI_Get(DxNAMEOF(DxService_t), (HUID)ulSvcUid, samalib_svc_cbGetSvcInfo, (void *)pstService);
	HxLOG_Debug("DAPI_Wait() start, ulDapiReqId: %u \n", ulDapiReqId);
	hErr = DAPI_Wait(6000, ulDapiReqId);
	if ((DERR_TIMEOUT == hErr) || (0 == pstService->uid))
	{
		HxLOG_Error("Service not found:\n");
		return ERR_FAIL;
	}
	HxLOG_Debug("DAPI_Wait() finish\n");

	pstSrcSvc = (DxService_t *)SAMA_Malloc(sizeof(DxService_t));
	if (NULL != pstSrcSvc)
	{
		SAMA_MemCpy(pstSrcSvc, pstService, sizeof(DxService_t));

		HxSEMT_Get(pstCache->ulSemId);
		pstCache->pstSvcList = HLIB_LIST_Append(pstCache->pstSvcList, (void *)pstSrcSvc);
		HxSEMT_Release(pstCache->ulSemId);
	}

	return ERR_OK;
}


STATIC HERROR samalib_svc_CopyAntennaInfo (sama_ConflictCheck_t *pstInst, DxTransponder_t *pstTransponder)
{
	HERROR					hErr = ERR_FAIL;
	DxAntInfo_t				stAntInfo;

	HxLOG_Trace();

	if (NULL != pstTransponder && eDxDELIVERY_TYPE_SAT == pstTransponder->tuningParam.eDeliType)
	{
		hErr = samalib_svc_GetAntenna(pstInst, pstTransponder->tuningParam.sat.antInfo.uid, &stAntInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("samalib_svc_GetAntenna err: antuid(%d)\n", pstTransponder->tuningParam.sat.antInfo.uid);
			return ERR_FAIL;
		}
		HxSTD_MemCopy (&(pstTransponder->tuningParam.sat.antInfo), &stAntInfo, sizeof(DxAntInfo_t));
		pstTransponder->tuningParam.sat.antennaType = stAntInfo.antType;
	}

	return ERR_OK;
}

HERROR samalib_svc_GetTransponder (sama_ConflictCheck_t *pstInst, HUINT32 ulTsUid, DxTransponder_t *pstTransponder)
{
	HBOOL					 bFoundCache = FALSE;
	HUINT32					 ulDapiReqId;
	HxList_t				*pstListItem;
	DxTransponder_t			*pstSrcTs = NULL;
	sama_ServiceCache_t		*pstCache = samalib_svc_GetCache(pstInst);
	HERROR					 hErr;

	if (NULL == pstCache)				{ return ERR_FAIL; }
	if (NULL == pstTransponder)			{ return ERR_FAIL; }

	SAMA_MemSet(pstTransponder, 0, sizeof(DxTransponder_t));

	HxSEMT_Get(pstCache->ulSemId);

	for (pstListItem = pstCache->pstTsList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstSrcTs = (DxTransponder_t *)HLIB_LIST_Data(pstListItem);
		if ((NULL != pstSrcTs) && (ulTsUid == pstSrcTs->uid))
		{
			bFoundCache = TRUE;
			break;
		}
	}

	HxSEMT_Release(pstCache->ulSemId);

	if (TRUE == bFoundCache)
	{
		SAMA_MemCpy(pstTransponder, pstSrcTs, sizeof(DxTransponder_t));
		samalib_svc_CopyAntennaInfo(pstInst, pstTransponder);
		return ERR_OK;
	}

	ulDapiReqId = DAPI_Get(DxNAMEOF(DxTransponder_t), (HUID)ulTsUid, samalib_svc_cbGetTsInfo, (void *)pstTransponder);
	HxLOG_Debug("DAPI_Wait() start, ulDapiReqId: %u \n", ulDapiReqId);
	hErr = DAPI_Wait(6000, ulDapiReqId);
	if ((DERR_TIMEOUT == hErr) || (0 == pstTransponder->uid))
	{
		HxLOG_Error("Transponder not found:\n");
		return ERR_FAIL;
	}
	HxLOG_Debug("DAPI_Wait() finish\n");

	samalib_svc_CopyAntennaInfo(pstInst, pstTransponder);

	pstSrcTs = (DxTransponder_t *)SAMA_Malloc(sizeof(DxTransponder_t));
	if (NULL != pstSrcTs)
	{
		SAMA_MemCpy(pstSrcTs, pstTransponder, sizeof(DxTransponder_t));


		HxSEMT_Get(pstCache->ulSemId);
		pstCache->pstTsList = HLIB_LIST_Append(pstCache->pstTsList, (void *)pstSrcTs);
		HxSEMT_Release(pstCache->ulSemId);
	}

	return ERR_OK;
}

HERROR samalib_svc_GetAntenna (sama_ConflictCheck_t *pstInst, HUINT32 ulAntUid, DxAntInfo_t *pstAntInfo)
{
	HBOOL					 bFoundCache = FALSE;
	HUINT32					 ulDapiReqId;
	HxList_t				*pstListItem;
	DxAntInfo_t				*pstSrcAntInfo = NULL;
	sama_ServiceCache_t		*pstCache = samalib_svc_GetCache(pstInst);
	HERROR					 hErr;

	HxLOG_Trace();

	if (NULL == pstCache)				{ return ERR_FAIL; }
	if (NULL == pstAntInfo)			{ return ERR_FAIL; }

	SAMA_MemSet(pstAntInfo, 0, sizeof(DxAntInfo_t));

	HxSEMT_Get(pstCache->ulSemId);

	for (pstListItem = pstCache->pstAntInfoList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstSrcAntInfo = (DxAntInfo_t *)HLIB_LIST_Data(pstListItem);
		if ((NULL != pstSrcAntInfo) && (ulAntUid == pstSrcAntInfo->uid))
		{
			bFoundCache = TRUE;
			break;
		}
	}

	HxSEMT_Release(pstCache->ulSemId);

	if (TRUE == bFoundCache)
	{
		SAMA_MemCpy(pstAntInfo, pstSrcAntInfo, sizeof(DxAntInfo_t));
		return ERR_OK;
	}

	ulDapiReqId = DAPI_Get(DxNAMEOF(DxAntInfo_t), (HUID)ulAntUid, samalib_svc_cbGetAntInfo, (void *)pstAntInfo);
	HxLOG_Debug("DAPI_Wait() start, ulDapiReqId: %u \n", ulDapiReqId);
	hErr = DAPI_Wait(6000, ulDapiReqId);
	if ((DERR_TIMEOUT == hErr) || (0 == pstAntInfo->uid))
	{
		HxLOG_Error("Transponder not found:\n");
		return ERR_FAIL;
	}
	HxLOG_Debug("DAPI_Wait() finish \n");

	pstSrcAntInfo = (DxAntInfo_t *)SAMA_Malloc(sizeof(DxAntInfo_t));
	if (NULL != pstSrcAntInfo)
	{
		SAMA_MemCpy(pstSrcAntInfo, pstAntInfo, sizeof(DxAntInfo_t));

		HxSEMT_Get(pstCache->ulSemId);
		pstCache->pstAntInfoList = HLIB_LIST_Append(pstCache->pstAntInfoList, (void *)pstSrcAntInfo);
		HxSEMT_Release(pstCache->ulSemId);
	}

	return ERR_OK;
}

HERROR samalib_svc_GetNetwork (sama_ConflictCheck_t *pstInst, HUINT32 ulNetUid, DxNetwork_t *pstNetwork)
{
	HBOOL					 bFoundCache = FALSE;
	HUINT32					 ulDapiReqId;
	HxList_t				*pstListItem;
	DxNetwork_t				*pstSrcNetwork = NULL;
	sama_ServiceCache_t		*pstCache = samalib_svc_GetCache(pstInst);
	HERROR					 hErr;

	if (NULL == pstCache)				{ return ERR_FAIL; }
	if (NULL == pstNetwork)			{ return ERR_FAIL; }

	SAMA_MemSet(pstNetwork, 0, sizeof(DxNetwork_t));

	HxSEMT_Get(pstCache->ulSemId);

	for (pstListItem = pstCache->pstNetList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstSrcNetwork = (DxNetwork_t *)HLIB_LIST_Data(pstListItem);
		if ((NULL != pstSrcNetwork) && (ulNetUid == pstSrcNetwork->uid))
		{
			bFoundCache = TRUE;
			break;
		}
	}

	HxSEMT_Release(pstCache->ulSemId);

	if (TRUE == bFoundCache)
	{
		SAMA_MemCpy(pstNetwork, pstSrcNetwork, sizeof(DxNetwork_t));
		return ERR_OK;
	}

	ulDapiReqId = DAPI_Get(DxNAMEOF(DxNetwork_t), (HUID)ulNetUid, samalib_svc_cbGetNetInfo, (void *)pstNetwork);
	HxLOG_Debug("DAPI_Wait() start, ulDapiReqId: %u \n", ulDapiReqId);
	hErr = DAPI_Wait(6000, ulDapiReqId);
	if ((DERR_TIMEOUT == hErr) || (0 == pstNetwork->uid))
	{
		HxLOG_Error("Network not found:\n");
		return ERR_FAIL;
	}
	HxLOG_Debug("DAPI_Wait() finish\n");

	pstSrcNetwork = (DxNetwork_t *)SAMA_Malloc(sizeof(DxNetwork_t));
	if (NULL != pstSrcNetwork)
	{
		SAMA_MemCpy(pstSrcNetwork, pstNetwork, sizeof(DxNetwork_t));


		HxSEMT_Get(pstCache->ulSemId);
		pstCache->pstNetList = HLIB_LIST_Append(pstCache->pstNetList, (void *)pstSrcNetwork);
		HxSEMT_Release(pstCache->ulSemId);
	}

	return ERR_OK;
}


#define ___PUBLIC_FUNCTIONS___
/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/

HERROR SAMALIB_SVC_GetService (sama_ConflictCheck_t *pstInst, HUINT32 ulSvcUid, DxService_t *pstService)
{
	return samalib_svc_GetService(pstInst, ulSvcUid, pstService);
}

HERROR SAMALIB_SVC_ClearCache (sama_ConflictCheck_t *pstInst)
{
	sama_ServiceCache_t		*pstCache = NULL;

	pstCache = samalib_svc_GetCache(pstInst);
	if (pstCache == NULL)
	{
		HxLOG_Error("cache is null \n");
		goto END_FUNC;
	}

	HxSEMT_Get(pstCache->ulSemId);

	HLIB_LIST_RemoveAllFunc(pstCache->pstSvcList, SAMA_Free_CB);
	pstCache->pstSvcList = NULL;
	HLIB_LIST_RemoveAllFunc(pstCache->pstTsList, SAMA_Free_CB);
	pstCache->pstTsList = NULL;
	HLIB_LIST_RemoveAllFunc(pstCache->pstNetList, SAMA_Free_CB);
	pstCache->pstNetList = NULL;
	HLIB_LIST_RemoveAllFunc(pstCache->pstAntInfoList, SAMA_Free_CB);
	pstCache->pstAntInfoList = NULL;

END_FUNC:
	if (pstCache != NULL)
	{
		HxSEMT_Release(pstCache->ulSemId);
	}
	return ERR_OK;
}


