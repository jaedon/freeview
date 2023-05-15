/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_time_base.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIBRARY		\n

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
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_time.h>

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

#define ___MEMBER_FUNCTIONS___


HERROR xsvc_si_LibTimeOnTdtReceived_Base (svcSi_TimeData_t *pstTime, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	SIxTable_PsiTable_t	*pstTdtTable;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstTime->aulTimeoutFilterId[eSIxTABLETYPE_Tdt]) ||
			(pstSecCb->ulFilterId != pstTime->aulTimeoutFilterId[eSIxTABLETYPE_Tdt]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_TIME_RECEIVING_FINISHED, hAction, (HINT32)FALSE, 0, 0);
		}

		pstTime->aulTimeoutFilterId[eSIxTABLETYPE_Tdt] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		pstTdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Tdt, SILIB_TABLE_EXT_ID_NOT);
		if (NULL != pstTdtTable)
		{
			if (svc_si_LibUpdateTimeByTdt (ulActionId, 0x00 /* svcHandle */, pstTdtTable) == ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SI_TIME_RECEIVING_FINISHED, hAction, (HINT32)TRUE, 0, 0);
			}

			PAL_SIRAWPOOL_FreeTable (pstTdtTable);
		}

		break;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibTimeOnTotReceived_Base (svcSi_TimeData_t *pstTime, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	SIxTable_PsiTable_t	*pstTotTable;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstTime->aulTimeoutFilterId[eSIxTABLETYPE_Tot]) ||
			(pstSecCb->ulFilterId != pstTime->aulTimeoutFilterId[eSIxTABLETYPE_Tot]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_TOT_TIMEOUT, hAction, 0, 0, 0);
			svc_si_PostMsgToAp (eSEVT_SI_TIME_RECEIVING_FINISHED, hAction, (HINT32)FALSE, 0, 0);
		}

		pstTime->aulTimeoutFilterId[eSIxTABLETYPE_Tot] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		pstTotTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Tot, SILIB_TABLE_EXT_ID_NOT);
		if (NULL != pstTotTable)
		{
			if (svc_si_LibUpdateTimeByTot (ulActionId) == ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SI_TOT, hAction, 0, 0, 0);
				svc_si_PostMsgToAp (eSEVT_SI_TIME_RECEIVING_FINISHED, hAction, (HINT32)TRUE, 0, 0);
			}

			PAL_SIRAWPOOL_FreeTable (pstTotTable);
		}

		break;
	}

	return ERR_OK;
}


/* end of file */

