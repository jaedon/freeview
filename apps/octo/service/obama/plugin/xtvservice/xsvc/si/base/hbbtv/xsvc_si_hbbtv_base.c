/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_hbbtv_base.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /HBBTV		\n

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
#include <tapi.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>
#include <pal_pipe.h>
#include <pal_sirawpool.h>

#include <db_svc.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_hbbtv.h>
#include <_xsvc_si_hbbtv.h>
#include <_xsvc_si_lib_hbbtv_base.h>

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

STATIC void xsvc_si_HbbtvMsgCmdStartHbbtv_Base (svcSi_HbbtvData_t *pstHbbtv, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	SvcSi_Arg_StartHbbtv_t	*pstStartHbbtv = &(pstMsg->unArg.stStartHbbtv);

	pstHbbtv->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical("invalid actionHandle(0x%X)\n", pstHbbtv->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(ulActionId, eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(ulActionId, pstHbbtv->eTextEncType);

	(void)svc_si_HbbtvSetHbbtvData(pstHbbtv, pstMsg->hAction, pstStartHbbtv->usOnId, pstStartHbbtv->usTsId, pstStartHbbtv->usSvcId);

	(void)svc_si_HbbtvStartSection(pstHbbtv, bSI_FILTERING_PAT);
}

STATIC void xsvc_si_HbbtvMsgCmdStopHbbtv_Base (svcSi_HbbtvData_t *pstHbbtv, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_HbbtvStopSection(pstHbbtv);
}

STATIC void xsvc_si_HbbtvMsgEvtPatTable_Base (svcSi_HbbtvData_t *pstHbbtv, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibHbbtvOnPatReceived_Base(pstHbbtv, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_HbbtvMsgEvtPmtTable_Base (svcSi_HbbtvData_t *pstHbbtv, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibHbbtvOnPmtReceived_Base(pstHbbtv, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_HbbtvMsgEvtAitTable_Base (svcSi_HbbtvData_t *pstHbbtv, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibHbbtvOnAitReceived_Base(pstHbbtv, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_HbbtvProcessMsg_Base (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_HbbtvData_t	*pstHbbtv = (svcSi_HbbtvData_t *)pvData;

	if (NULL == pstHbbtv)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_HBBTV:
		HxLOG_Info("[eSIMSG_CMD_START_HBBTV]\n");
		xsvc_si_HbbtvMsgCmdStartHbbtv_Base(pstHbbtv, pstMsg);
		break;

	case eSIMSG_CMD_STOP_HBBTV:
		HxLOG_Info("[eSIMSG_CMD_STOP_HBBTV]\n");
		xsvc_si_HbbtvMsgCmdStopHbbtv_Base(pstHbbtv, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		HxLOG_Info("[eSIMSG_EVT_PAT_TABLE]\n");
		xsvc_si_HbbtvMsgEvtPatTable_Base(pstHbbtv, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		HxLOG_Info("[eSIMSG_EVT_PMT_TABLE]\n");
		xsvc_si_HbbtvMsgEvtPmtTable_Base(pstHbbtv, pstMsg);
		break;

	case eSIMSG_EVT_AIT_TABLE:
		HxLOG_Info("[eSIMSG_EVT_AIT_TABLE]\n");
		xsvc_si_HbbtvMsgEvtAitTable_Base(pstHbbtv, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_HbbtvGetActionFunc_Base (void)
{
	return xsvc_si_HbbtvProcessMsg_Base;
}

void *xsvc_si_HbbtvAllocActionData_Base (void)
{
	svcSi_HbbtvData_t	*pstHbbtv;

	pstHbbtv = (svcSi_HbbtvData_t *)OxSI_Calloc(sizeof(svcSi_HbbtvData_t));
	if (NULL != pstHbbtv)
	{
		pstHbbtv->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstHbbtv;
}

void xsvc_si_HbbtvFreeActionData_Base (void *pvData)
{
	svcSi_HbbtvData_t	*pstHbbtv = (svcSi_HbbtvData_t *)pvData;

	if (NULL != pstHbbtv)
	{
		OxSI_Free(pstHbbtv);
	}

	return;
}

/* end of file */

