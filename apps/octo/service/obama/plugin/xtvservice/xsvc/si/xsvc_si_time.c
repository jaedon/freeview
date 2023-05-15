/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_time.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIVE			\n

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
#include <pal_pipe.h>
#include <pal_sirawpool.h>
#include <db_svc.h>
#include <svc_pipe.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_filtering.h>
#include <_svc_si_time.h>
#include <_xsvc_si_lib_time_base.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	SVC_SatType_t			 eSatType;
	HUINT32					 ulFreq;
	HUINT32					 ulSymbolRate;
} XsvcLive_MeTimeTpInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


#define ___LOCAL_FUNCTIONS___

STATIC void xsvc_si_TimeMsgCmdStartTime(svcSi_TimeData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 	 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 	 bFiltering = 0;
	SvcSi_Arg_StartTimeUpdate_t	*pstStartTime = &(pstMsg->unArg.stStartTimeUpdate);

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType(ulActionId, eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType(ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = bSI_FILTERING_TDT;
	if (pstStartTime->bUseTot == TRUE)
	{
		bFiltering |= bSI_FILTERING_TOT;
	}

	(void)svc_si_TimeSetLiveData(pstLive, pstMsg->hAction);
	(void)svc_si_TimeStartSection(pstLive, bFiltering);
}

STATIC void xsvc_si_TimeMsgCmdStopTime(svcSi_TimeData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_TimeStopSection(pstLive);
}

STATIC void xsvc_si_TimeMsgEvtTdtTable(svcSi_TimeData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibTimeOnTdtReceived_Base(pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_TimeMsgEvtTotTable(svcSi_TimeData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibTimeOnTotReceived_Base(pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}


#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_TimeProcessMsg (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_TimeData_t	*pstLive = (svcSi_TimeData_t *)pvData;

	if (NULL == pstLive)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_TIMEUPDATE:
		xsvc_si_TimeMsgCmdStartTime(pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_TIMEUPDATE:
		xsvc_si_TimeMsgCmdStopTime(pstLive, pstMsg);
		break;

	case eSIMSG_EVT_TDT_TABLE:
		xsvc_si_TimeMsgEvtTdtTable(pstLive, pstMsg);
		break;

	case eSIMSG_EVT_TOT_TABLE:
		xsvc_si_TimeMsgEvtTotTable(pstLive, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_TimeGetTimeActionFunc(void)
{
	return xsvc_si_TimeProcessMsg;
}

void *xsvc_si_TimeAllocTimeActionData(void)
{
	svcSi_TimeData_t	*pstLive;

	pstLive = (svcSi_TimeData_t *)OxSI_Calloc (sizeof(svcSi_TimeData_t));
	if (NULL != pstLive)
	{
		pstLive->eState 		= eSI_TIMESTATE_Init;
		pstLive->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstLive;
}

void xsvc_si_TimeFreeTimeActionData(void *pvData)
{
	svcSi_TimeData_t	*pstLive = (svcSi_TimeData_t *)pvData;

	if (NULL != pstLive)
	{
		OxSI_Free (pstLive);
	}

	return;
}

/* end of file */
