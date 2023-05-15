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
#include <_svc_si_mediapb.h>

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
STATIC HERROR svc_si_MediapbConvertTableType2FilterTypeAndExtId (svcSi_MediapbData_t *pstMediapb, SIxTable_Type_e eTableType, svcSi_FilteringType_b *peFilterType, HUINT32 *pulExtId, HUINT32 *pulPid)
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
		*pulExtId 		= (HUINT32)pstMediapb->usPmtSvcId;
		*pulPid			= (HUINT32)pstMediapb->usPmtPid;
		break;

	default:
		*peFilterType	= 0;
		return ERR_FAIL;
	}

	return ERR_OK;
}

#define ___MEMBER_FUNCTIONS___

HERROR svc_si_MediapbSetMediapbData(svcSi_MediapbData_t *pstMediapb, Handle_t hAction)
{
	HUINT32				 ulCnt;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);
	HUINT32				 ulDemuxId;
	HERROR				 hErr, hResult = ERR_FAIL;

	if (pstMediapb == NULL)				{ return ERR_FAIL; }

	hErr = PAL_PIPE_GetResourceId (ulActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("PAL_PIPE_GetResourceId ulActionId(%d) Err\n", ulActionId);
		goto END_FUNC;
	}

	// Set the arguments to the SI Live Data structure.
	pstMediapb->hAction			= hAction;
	pstMediapb->ulDemuxId		= ulDemuxId;

	for (ulCnt = 0; ulCnt < eSIxTABLETYPE_MAX; ulCnt++)
	{
		pstMediapb->aulTimeoutFilterId[ulCnt] = FILTER_ID_NULL;
	}

	hResult = ERR_OK;

END_FUNC:

	return hResult;
}

HERROR svc_si_MediapbStartSection (svcSi_MediapbData_t *pstMediapb, svcSi_FilteringType_b eFiltering)
{
	HBOOL					 bFiltered = FALSE;
	HUINT32					 ulExtId, ulPid;
	SIxTable_Type_e			 eTableType;
	svcSi_FilteringType_b	 eFilterType;
	HERROR					 hErr;

	if (pstMediapb == NULL)				{ return ERR_FAIL; }

	for (eTableType = 0; eTableType < eSIxTABLETYPE_MAX; eTableType++)
	{
		hErr = svc_si_MediapbConvertTableType2FilterTypeAndExtId (pstMediapb, eTableType, &eFilterType, &ulExtId, &ulPid);
		if ((ERR_OK == hErr) && (eFiltering & eFilterType) && (ulPid < PID_NULL))
		{
			hErr = svc_si_FilterStartTableSection (pstMediapb->hAction, pstMediapb->ulDemuxId, eTableType, ulExtId, ulPid, eSI_TABLE_FILTER_MODE_Monitor, NULL);
			if (ERR_OK == hErr)
			{
				pstMediapb->eTableFiltering |= eFilterType;
				bFiltered = TRUE;
			}
			else
			{
				HxLOG_Error ("svc_si_FilterStartTableSection hAction(0x%08x) TableType(%d) err:\n", pstMediapb->hAction, eTableType);
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

HERROR svc_si_MediapbStopSection (svcSi_MediapbData_t *pstMediapb)
{
	HERROR				 hErr;

	if (pstMediapb == NULL)				{ return ERR_FAIL; }

	hErr = svc_si_FilterStopTableSection (pstMediapb->hAction, eSIxTABLETYPE_ALL, SILIB_TABLE_EXT_ID_ALL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_FilterStopTableSection eSIxTABLETYPE_ALL\n");
	}

	pstMediapb->eTableFiltering = 0;
	return ERR_OK;
}



/* end of file */

