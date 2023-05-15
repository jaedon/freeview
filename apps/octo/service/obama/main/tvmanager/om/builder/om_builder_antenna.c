/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_builder_antenna.c
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

#include <db_param.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
//#include <xmgr_search.h>

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
HERROR om_builder_antconn_StartSearch (HUINT32 ulBuilderId)
{
	omBldContext_t		*pstContext = om_builder_GetContextByBuilderId (ulBuilderId);
	HUINT32 			ulActionId;
	Handle_t			hAction;

	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context found: ulBuildId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	ulActionId = pstContext->ulActionId;
	hAction = SVC_PIPE_GetActionHandle(ulActionId);

	pstContext->pfnSearchProc = MGR_SEARCH_GetSatConnectionAction();

	if (BUS_MGR_Create(NULL, APP_ACTION_PRIORITY, pstContext->pfnSearchProc, hAction, (HINT32)&pstContext->stSearchOption, 0, 0) == ERR_OK)
	{
		pstContext->eState = eDxBUILDER_STATE_RUNNING;
		BUS_SendMessage(pstContext->pfnSearchProc, eMEVT_SEARCH_ACTION_START, hAction, 0, 0, 0);
	}

	return ERR_OK;
}


HERROR om_builder_antconn_StopSearch (HUINT32 ulBuilderId)
{
	omBldContext_t		*pstContext = om_builder_GetContextByBuilderId (ulBuilderId);

	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context found: ulBuildId(%d)\n", ulBuilderId);
		return ERR_FAIL;
	}

	BUS_MGR_Destroy(pstContext->pfnSearchProc);

	pstContext->eState = eDxBUILDER_STATE_STOPPED;
	return ERR_OK;
}

BUS_Result_t om_builder_antconn_MsgSearchAntConnectTypeDetected (Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				 ulActionId = SVC_PIPE_GetActionId(hAction);
	omBldContext_t			*pstContext;
	BUS_Result_t			 eResult = MESSAGE_PASS;
	HERROR					 hErr;

	pstContext = om_builder_GetContextByActionId (ulActionId);
	if (NULL == pstContext)
	{
		HxLOG_Error ("No Context Exist: ulActionId(%d)\n", ulActionId);
		return eResult;
	}

	if (pstContext->stOption.eOptionType != eOxBUILDER_OPTIONTYPE_ANTCONNECT)
	{
		HxLOG_Error ("not allowed search type : eOptionType(%d)\n", pstContext->stOption.eOptionType);
		return eResult;
	}

	pstContext->stBuilderState.ulCompleteTuningNum 	= 2;
	pstContext->stBuilderState.ulTotalTuningNum 	= 2;
//	pstContext->stBuilderState.ulSearchedSvcNum 	= MWC_PARAM_GetAntennaConnectionType();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE, (HUINT32 *)&pstContext->stBuilderState.ulSearchedSvcNum, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_ANT_CONN_TYPE) Error!!!\n");
	}

	pstContext->eState = eDxBUILDER_STATE_FINISHED;
	om_builder_CheckAndNotifyStateChanged(pstContext->ulBuilderId);

	return eResult;
}

/* end of file */
