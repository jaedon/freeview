/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_live.c
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>

#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_live.h>

/*******************************************************************/
/********************      Defines         *************************/
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

#define ___LOCAL_FUNCTIONS___
STATIC HERROR svc_si_LiveConvertTableType2FilterTypeAndExtId (svcSi_LiveData_t *pstLiveData, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
{
	switch (eTableType)
	{
	case eSIxTABLETYPE_Pat:
		*peFilterType	= bSI_FILTERING_PAT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= PAT_PID;
		break;

	case eSIxTABLETYPE_Pmt:
		*peFilterType	= bSI_FILTERING_PMT;
		*pulExtId 		= (HUINT32)pstLiveData->usPmtSvcId;
		*pulPid			= (HUINT32)pstLiveData->usPmtPid;
		break;

	case eSIxTABLETYPE_EitActPf:
		*peFilterType	= bSI_FILTERING_EIT_ACT_PF;
		*pulExtId 		= (HUINT32)pstLiveData->usEitSvcId;
		*pulPid			= EIT_PID;
		break;

	case eSIxTABLETYPE_SdtAct:
		*peFilterType	= bSI_FILTERING_SDT_ACT;
		*pulExtId 		= (HUINT32)pstLiveData->usSdtTsId;
		*pulPid			= SDT_PID;
		break;

	case eSIxTABLETYPE_SdtOth:
		*peFilterType	= bSI_FILTERING_SDT_OTH;
		*pulExtId		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid 		= SDT_PID;
		break;

	case eSIxTABLETYPE_NitAct:
		*peFilterType	= bSI_FILTERING_NIT_ACT;
		*pulExtId 		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= NIT_PID;
		break;

	case eSIxTABLETYPE_Bat:
		*peFilterType	= bSI_FILTERING_BAT;
		*pulExtId		= (HUINT32)pstLiveData->usBatBouquetId;
		*pulPid 		= BAT_PID;
		break;

	case eSIxTABLETYPE_Tdt:
		*peFilterType	= bSI_FILTERING_TDT;
		*pulExtId		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= TDT_PID;
		break;

	case eSIxTABLETYPE_Tot:
		*peFilterType	= bSI_FILTERING_TOT;
		*pulExtId		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= TOT_PID;
		break;

	case eSIxTABLETYPE_Cat:
		*peFilterType	= bSI_FILTERING_CAT;
		*pulExtId		= SILIB_TABLE_EXT_ID_NOT;
		*pulPid			= CAT_PID;
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC SvcSi_TableFilterMode_e svc_si_LiveGetTableFilterMode(SIxTable_Type_e eTableType, HUINT32 ulExtId)
{
	SvcSi_TableFilterMode_e		 eFilterMode = eSI_TABLE_FILTER_MODE_Monitor;

	if (ulExtId >= 0xffff)
	{
		/* ext id를 사용하지 않는 경우 : SILIB_TABLE_EXT_ID_NOT, SILIB_TABLE_EXT_ID_ALL*/
		switch (eTableType)
		{
			case eSIxTABLETYPE_Bat:			//< BAT ALL
			/* 필요하면 여시오
			case eSIxTABLETYPE_Sgt:			//< SGT ALL
			case eSIxTABLETYPE_SdtOth:		//< SDT Other
			case eSIxTABLETYPE_NitOth:		//< NIT Other
			*/
				eFilterMode = eSI_TABLE_FILTER_MODE_AllMonitor;
				break;

			default:
				/* nothing to do */
				break;
		}
	}

	return eFilterMode;
}

#define ___MEMBER_FUNCTIONS___
HERROR svc_si_LiveSetPbData (svcSi_LiveData_t *pstLiveData, Handle_t hAction, HUINT16 usTsId, HUINT16 usSvcId, HUINT16 usPmtPid )
{
	HUINT32				 ulCnt;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32				 ulDemuxId;
	DbSvc_Info_t			*pstMasterSvcInfo = NULL, *pstSuppleSvcInfo = NULL;
	HERROR				 hErr, hResult = ERR_FAIL;

	if (pstLiveData == NULL)				{ return ERR_FAIL; }

	pstMasterSvcInfo	= (DbSvc_Info_t *)OxSI_Malloc (sizeof(DbSvc_Info_t));
	pstSuppleSvcInfo	= (DbSvc_Info_t *)OxSI_Malloc (sizeof(DbSvc_Info_t));

	if ((NULL == pstMasterSvcInfo) || (NULL == pstSuppleSvcInfo))
	{
		goto END_FUNC;
	}

	hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_PIPE_GetResourceId ulActionId(%d) Err\n", ulActionId);
		goto END_FUNC;
	}

	pstLiveData->bAbortPatPmt		= FALSE;

	// Set the arguments to the SI Live Data structure.
	pstLiveData->hAction			= hAction;
	pstLiveData->hService			= 0;
	pstLiveData->hSupService		= 0;			// 특별한 Service가 아닌 이상 hService와 hSupService는 같은 것을 가리키고 있어야 한다.

	// PMT
	pstLiveData->usPmtSvcId			= usSvcId;
	pstLiveData->usPmtPid			= usPmtPid;

	// EIT ACT P/F
	pstLiveData->usEitSvcId			= usSvcId;

	// SDT ACT
	pstLiveData->usSdtTsId			= usTsId;

	// DMX ID, etc.
	pstLiveData->ulDemuxId			 = ulDemuxId;
	pstLiveData->eState				 = eSI_LIVESTATE_Running;

	for (ulCnt = 0; ulCnt < eSIxTABLETYPE_MAX; ulCnt++)
	{
		pstLiveData->aulCrc32[ulCnt] = 0;
		pstLiveData->aulTimeoutFilterId[ulCnt] = FILTER_ID_NULL;
	}

	hResult = ERR_OK;


END_FUNC:
	if (NULL != pstMasterSvcInfo)				{ OxSI_Free (pstMasterSvcInfo); }
	if (NULL != pstSuppleSvcInfo)				{ OxSI_Free (pstSuppleSvcInfo); }

	return hResult;
}

HERROR svc_si_LiveSetLiveData (svcSi_LiveData_t *pstLiveData, Handle_t hAction, Handle_t hMasterSvc, Handle_t hSuppleSvc, HUINT32 ulSuppType)
{
	HUINT32				 ulCnt;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32				 ulDemuxId;
	DbSvc_Info_t			*pstMasterSvcInfo = NULL, *pstSuppleSvcInfo = NULL;
	HERROR				 hErr, hResult = ERR_FAIL;

	if (pstLiveData == NULL)				{ return ERR_FAIL; }

	pstMasterSvcInfo	= (DbSvc_Info_t *)OxSI_Malloc (sizeof(DbSvc_Info_t));
	pstSuppleSvcInfo	= (DbSvc_Info_t *)OxSI_Malloc (sizeof(DbSvc_Info_t));

	if ((NULL == pstMasterSvcInfo) || (NULL == pstSuppleSvcInfo))
	{
		goto END_FUNC;
	}

	hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_PIPE_GetResourceId ulActionId(%d) Err\n", ulActionId);
		goto END_FUNC;
	}

	// Get the service info from the DB
	hErr = DB_SVC_GetServiceInfo (hMasterSvc, pstMasterSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("DB_SVC_GetServiceInfo hMasterSvc(0x%08x) Err\n", hMasterSvc);
		goto END_FUNC;
	}

	hErr = DB_SVC_GetServiceInfo (hSuppleSvc, pstSuppleSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("DB_SVC_GetServiceInfo hSuppleSvc(0x%08x) Err\n", hSuppleSvc);
		goto END_FUNC;
	}

	pstLiveData->bAbortPatPmt		= FALSE;

	// Set the arguments to the SI Live Data structure.
	pstLiveData->hAction			= hAction;
	pstLiveData->hService			= hMasterSvc;
	pstLiveData->hSupService		= hSuppleSvc;			// 특별한 Service가 아닌 이상 hService와 hSupService는 같은 것을 가리키고 있어야 한다.

	// PMT
	pstLiveData->usPmtSvcId			= DbSvc_GetSvcId(pstMasterSvcInfo);
	pstLiveData->usPmtPid			= DbSvc_GetPmtPid(pstMasterSvcInfo);

	// EIT ACT P/F
	pstLiveData->usEitSvcId			= DbSvc_GetSvcId(pstMasterSvcInfo);

	// SDT ACT
	pstLiveData->usSdtTsId			= DbSvc_GetTsId(pstMasterSvcInfo);

	// DMX ID, etc.
	pstLiveData->ulDemuxId			 = ulDemuxId;
	pstLiveData->eState				 = eSI_LIVESTATE_Running;

	// Setting Time:
	pstLiveData->ulPatTimeTick		 = HLIB_STD_GetSystemTick();

	for (ulCnt = 0; ulCnt < eSIxTABLETYPE_MAX; ulCnt++)
	{
		pstLiveData->aulCrc32[ulCnt] = 0;
		pstLiveData->aulTimeoutFilterId[ulCnt] = FILTER_ID_NULL;
	}

	hResult = ERR_OK;

END_FUNC:
	if (NULL != pstMasterSvcInfo)				{ OxSI_Free (pstMasterSvcInfo); }
	if (NULL != pstSuppleSvcInfo)				{ OxSI_Free (pstSuppleSvcInfo); }

	return hResult;
}

HERROR svc_si_LiveStartSection (svcSi_LiveData_t *pstLiveData, svcSi_FilteringType_b eFiltering)
{
	HBOOL					 bFiltered = FALSE;
	HUINT32					 ulExtId, ulPid;
	SIxTable_Type_e			 eTableType;
	svcSi_FilteringType_b	 eFilterType;
	HERROR					 hErr;

	if (pstLiveData == NULL)				{ return ERR_FAIL; }

	for (eTableType = 0; eTableType < eSIxTABLETYPE_MAX; eTableType++)
	{
		hErr = svc_si_LiveConvertTableType2FilterTypeAndExtId (pstLiveData, eTableType, &eFilterType, &ulExtId, &ulPid);
		if ((ERR_OK == hErr) && (eFiltering & eFilterType) && (ulPid < PID_NULL))
		{
			SvcSi_TableFilterMode_e	eFilterMode = svc_si_LiveGetTableFilterMode(eTableType, ulExtId);

			HxLOG_Print("hAct(0x%x), demux(%d), eTableType(%d), ulExtId(0x%x), ulPid(0x%x)\n", pstLiveData->hAction, pstLiveData->ulDemuxId, eTableType, ulExtId, ulPid);
			hErr = svc_si_FilterStartTableSection (pstLiveData->hAction, pstLiveData->ulDemuxId, eTableType, ulExtId, ulPid, eFilterMode, NULL);
			if (ERR_OK == hErr)
			{
				pstLiveData->eTableFiltering |= eFilterType;
				bFiltered = TRUE;
			}
			else
			{
				HxLOG_Error ("svc_si_FilterStartTableSection hAction(0x%08x) TableType(%d) err:\n", pstLiveData->hAction, eTableType);
			}
		}
	}

	if (TRUE != bFiltered)
	{
		HxLOG_Error ("No filter has started: All Failed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_LiveStopSection (svcSi_LiveData_t *pstLiveData)
{
	HERROR				 hErr;

	if (pstLiveData == NULL)				{ return ERR_FAIL; }

	hErr = svc_si_FilterStopTableSection (pstLiveData->hAction, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_FilterStopTableSection eSIxTABLETYPE_ALL\n");
	}

	pstLiveData->eTableFiltering = 0;
	return ERR_OK;
}



/* end of file */

