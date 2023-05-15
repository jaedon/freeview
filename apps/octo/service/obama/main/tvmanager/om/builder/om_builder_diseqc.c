/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_builder_diseqc.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <otl.h>

#include "_om_builder.h"


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

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
HERROR om_builder_diseqc_StartDetection (HUINT32 ulBuilderId)
{
	omBldContext_t		*pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	HUINT32				 ulActionId;
	Handle_t			 hAction;
	SVC_SatType_t		 eSatType;

	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context found: ulBuildId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	ulActionId = pstContext->ulActionId;
	hAction = SVC_PIPE_GetActionHandle(ulActionId);

	pstContext->pfnSearchProc = MGR_SEARCH_GetFinadSatAction(ulActionId);
	eSatType = OTL_CONV_SatString2SVC_SatType(pstContext->stOption.unArgs.stSatInfo.szSatelliteName);

	if (BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, pstContext->pfnSearchProc, hAction, (HINT32)&pstContext->stSearchOption, (HINT32)eSatType, 0) == ERR_OK)
	{
		pstContext->eState = eDxBUILDER_STATE_RUNNING;
		BUS_SendMessage(pstContext->pfnSearchProc, eMEVT_SEARCH_ACTION_START, hAction, 0, 0, 0);
	}

	return ERR_OK;
}

HERROR om_builder_diseqc_StopDetection (HUINT32 ulBuilderId)
{
	omBldContext_t		*pstContext = om_builder_GetContextByBuilderId (ulBuilderId);

	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context found: ulBuildId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	pstContext->eState = eDxBUILDER_STATE_STOPPED;
	return ERR_OK;
}

BUS_Result_t om_builder_diseqc_MsgSearchFinished (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					ulActionId = SVC_PIPE_GetActionId(hAction);
	omBldContext_t			*pstContext = NULL;
	BUS_Result_t			eResult = MESSAGE_PASS;

	pstContext = om_builder_GetContextByActionId (ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulActionId(%d)\n", ulActionId);
		return eResult;
	}

	pstContext->stBuilderState.ulTotalTuningNum		= 2;
	pstContext->stBuilderState.ulCompleteTuningNum	= 2;
	pstContext->stBuilderState.ulSearchedSvcNum		= (HUINT32)p2;

	BUS_MGR_Destroy(pstContext->pfnSearchProc);

	return eResult;
}

/* end of file */
