/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_hbbtv.c
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
#include <util.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>

#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_hbbtv.h>

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
STATIC HERROR svc_si_HbbtvConvertTableType2FilterTypeAndExtId (svcSi_HbbtvData_t *pstHbbtv, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
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
		*pulExtId 		= (HUINT32)pstHbbtv->usPmtSvcId;
		*pulPid			= (HUINT32)pstHbbtv->usPmtPid;
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC SvcSi_TableFilterMode_e svc_si_HbbtvGetTableFilterMode (SIxTable_Type_e eTableType, HUINT32 ulExtId)
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
HERROR svc_si_HbbtvGetAitSection(HUINT32 ulActionId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount)
{
	HxLOG_Print("[%s:%d] Get AIT raw section +++ : ulRawArrayMax(%d) \n", __FUNCTION__, __LINE__, ulRawArrayMax);
	if ((pulListCount == NULL) || (ppucRawArray == NULL))			{ return ERR_FAIL; }

	*ppucRawArray = NULL;
	*pulListCount = 0;

	if (PAL_SIRAWPOOL_GetRawArrays(ulActionId, eSIxTABLETYPE_Ait, FALSE, 0, ulRawArrayMax, ppucRawArray, pulListCount) != ERR_OK)
	{
		HxLOG_Print("[%s:%d] Fail to get ait raw section \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Print("[%s:%d] Get AIT raw section --- : count(%d) \n", __FUNCTION__, __LINE__, pulListCount);
	return ERR_OK;
}

HERROR svc_si_HbbtvSetHbbtvData (svcSi_HbbtvData_t *pstHbbtv, Handle_t hAction, HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId)
{
	HUINT32				 ulCnt;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32				 ulDemuxId;
	HERROR				 hErr, hResult = ERR_FAIL;

	if (pstHbbtv == NULL)				{ return ERR_FAIL; }

	hErr = PAL_PIPE_GetResourceId(ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_PIPE_GetResourceId ulActionId(%d) Err\n", ulActionId);
		goto END_FUNC;
	}

	// Set the arguments to the SI Live Data structure.
	pstHbbtv->hAction			= hAction;
	pstHbbtv->usOnId			= usOnId;
	pstHbbtv->usTsId			= usTsId;			// 특별한 Service가 아닌 이상 hService와 hSupService는 같은 것을 가리키고 있어야 한다.
	pstHbbtv->usSvcId			= usSvcId;

	// PMT
	pstHbbtv->usPmtSvcId		= usSvcId;
	pstHbbtv->usPmtPid			= PID_NULL;
	pstHbbtv->usAitPid			= PID_NULL;

	// DMX ID, etc.
	pstHbbtv->ulDemuxId			 = ulDemuxId;

	// Setting Time:
	pstHbbtv->ulPatTimeTick		 = HLIB_STD_GetSystemTick();

	for (ulCnt = 0; ulCnt < eSIxTABLETYPE_MAX; ulCnt++)
	{
		pstHbbtv->aulCrc32[ulCnt] = 0;
		pstHbbtv->aulTimeoutFilterId[ulCnt] = FILTER_ID_NULL;
	}

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}

HERROR svc_si_HbbtvStartSection (svcSi_HbbtvData_t *pstHbbtv, svcSi_FilteringType_b eFiltering)
{
	HBOOL					 bFiltered = FALSE;
	HUINT32					 ulExtId, ulPid;
	SIxTable_Type_e			 eTableType;
	svcSi_FilteringType_b	 eFilterType;
	HERROR					 hErr;

	if (pstHbbtv == NULL)				{ return ERR_FAIL; }

	for (eTableType = 0; eTableType < eSIxTABLETYPE_MAX; eTableType++)
	{
		hErr = svc_si_HbbtvConvertTableType2FilterTypeAndExtId(pstHbbtv, eTableType, &eFilterType, &ulExtId, &ulPid);
		if ((ERR_OK == hErr) && (eFiltering & eFilterType) && (ulPid < PID_NULL))
		{
			SvcSi_TableFilterMode_e	eFilterMode = svc_si_HbbtvGetTableFilterMode(eTableType, ulExtId);

			HxLOG_Print("hAct(0x%x), demux(%d), eTableType(%d), ulExtId(0x%x), ulPid(0x%x)\n", pstHbbtv->hAction, pstHbbtv->ulDemuxId, eTableType, ulExtId, ulPid);
			hErr = svc_si_FilterStartTableSection (pstHbbtv->hAction, pstHbbtv->ulDemuxId, eTableType, ulExtId, ulPid, eFilterMode, NULL);
			if (ERR_OK == hErr)
			{
				pstHbbtv->eTableFiltering |= eFilterType;
				bFiltered = TRUE;
			}
			else
			{
				HxLOG_Error("svc_si_FilterStartTableSection hAction(0x%08x) TableType(%d) err:\n", pstHbbtv->hAction, eTableType);
			}
		}
	}

	if (TRUE != bFiltered)
	{
		HxLOG_Error("No filter has started: All Failed.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_si_HbbtvStopSection (svcSi_HbbtvData_t *pstHbbtv)
{
	HERROR				 hErr;

	if (pstHbbtv == NULL)				{ return ERR_FAIL; }

	hErr = svc_si_FilterStopTableSection(pstHbbtv->hAction, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("svc_si_FilterStopTableSection eSIxTABLETYPE_ALL\n");
	}

	pstHbbtv->eTableFiltering = 0;
	return ERR_OK;
}

/* end of file */

