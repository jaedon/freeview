/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_live_amx_cenam.c
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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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
#include <_svc_si_live.h>
#include <_xsvc_si_lib_live_base.h>

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

STATIC void xsvc_si_LiveMsgCmdStartLive_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_Arg_StartLive_t	*pstStartLive = &(pstMsg->unArg.stStartLive);

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

#if defined(CONFIG_SUPPORT_FCC)
	// FCC svc��� fcc�� �� �ʿ��� table�� ����
	bFiltering = (bSI_FILTERING_PAT | bSI_FILTERING_PMT);

	if (pstStartLive->bFccSvc == FALSE)
	{
		/* main svc */
		bFiltering |= ( bSI_FILTERING_NIT_ACT |
						bSI_FILTERING_SDT_ACT |
						bSI_FILTERING_EIT_ACT_PF |
						bSI_FILTERING_TDT);
	}

	pstLive->bFccSvc = pstStartLive->bFccSvc;
#else

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_NIT_ACT |
					bSI_FILTERING_SDT_ACT |
					bSI_FILTERING_EIT_ACT_PF |
					bSI_FILTERING_TDT );
#endif

	(void)svc_si_LiveSetLiveData (pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopLive_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgCmdStartRec_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_Arg_StartLive_t	*pstStartLive = &(pstMsg->unArg.stStartLive);

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_CAT |
					bSI_FILTERING_EIT_ACT_PF );

	(void)svc_si_LiveSetLiveData (pstLive, pstMsg->hAction, pstStartLive->hMasterSvc, pstStartLive->hSuppleSvc, pstStartLive->ulSupType);
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopRec_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgCmdStartPb_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);
	svcSi_FilteringType_b	 bFiltering = 0;
	SvcSi_PlaybackOption_t *pstPbOption = &(pstMsg->unArg.stStartPb.stOption);

	pstLive->hAction = pstMsg->hAction;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("invalid actionHandle(0x%X)\n", pstLive->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_DVB);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	bFiltering = (	bSI_FILTERING_PAT |
					bSI_FILTERING_PMT |
					bSI_FILTERING_EIT_ACT_PF );

	(void)svc_si_LiveSetPbData (pstLive, pstMsg->hAction, pstPbOption->usTsId, pstPbOption->usSvcId, pstPbOption->usPmtPid );
	(void)svc_si_LiveStartSection (pstLive, bFiltering);
}

STATIC void xsvc_si_LiveMsgCmdStopPb_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_LiveStopSection (pstLive);
}

STATIC void xsvc_si_LiveMsgEvtPatTable_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnPatReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtPmtTable_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnPmtReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtEitTable_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnEitActPfReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtNitTable_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnNitActReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtSdtTable_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnSdtActReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtTdtTable_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnTdtReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_LiveMsgEvtCatTable_Amx_Cenam (svcSi_LiveData_t *pstLive, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_LibLiveOnCatReceived_Base (pstLive, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_LiveProcessMsg_Amx_Cenam (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_LiveData_t	*pstLive = (svcSi_LiveData_t *)pvData;

	if (NULL == pstLive)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_LIVE:
		xsvc_si_LiveMsgCmdStartLive_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_LIVE:
		xsvc_si_LiveMsgCmdStopLive_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_REC:
		xsvc_si_LiveMsgCmdStartRec_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_REC:
		xsvc_si_LiveMsgCmdStopRec_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_START_PB:
		xsvc_si_LiveMsgCmdStartPb_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_CMD_STOP_PB:
		xsvc_si_LiveMsgCmdStopPb_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_LiveMsgEvtPatTable_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_LiveMsgEvtPmtTable_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_EIT_TABLE:
		xsvc_si_LiveMsgEvtEitTable_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_NIT_TABLE:
		xsvc_si_LiveMsgEvtNitTable_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_SDT_TABLE:
		xsvc_si_LiveMsgEvtSdtTable_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_TDT_TABLE:
		xsvc_si_LiveMsgEvtTdtTable_Amx_Cenam (pstLive, pstMsg);
		break;

	case eSIMSG_EVT_CAT_TABLE:
		xsvc_si_LiveMsgEvtCatTable_Amx_Cenam (pstLive, pstMsg);
		break;

	default:
		HxLOG_Debug ("Non-processing MSG (0x%08x) -> skipped:\n", pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_LiveGetLiveActionFunc_Amx_Cenam (void)
{
	return xsvc_si_LiveProcessMsg_Amx_Cenam;
}

void *xsvc_si_LiveAllocLiveActionData_Amx_Cenam (void)
{
	svcSi_LiveData_t	*pstLive;

	pstLive = (svcSi_LiveData_t *)OxSI_Calloc (sizeof(svcSi_LiveData_t));
	if (NULL != pstLive)
	{
		pstLive->eState = eSI_LIVESTATE_Init;
		pstLive->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstLive;
}

void xsvc_si_LiveFreeLiveActionData_Amx_Cenam (void *pvData)
{
	svcSi_LiveData_t	*pstLive = (svcSi_LiveData_t *)pvData;

	if (NULL != pstLive)
	{
		OxSI_Free (pstLive);
	}

	return;
}

HBOOL xsvc_si_LiveSvcEsUpdateTimeByTdt_Amx_Cenam(HUINT32 ulActionId, SIxTable_PsiTable_t *pstTdtTable)
{
	HBOOL					bUpdate = FALSE;
	UNIXTIME				utCurrTime = 0, utSiTime;
	HERROR					hErr;
	static HBOOL			bFirstRequest = TRUE;

	if (NULL == pstTdtTable)					{ return ERR_FAIL; }

	// first req. always update
	if(bFirstRequest == TRUE)
	{
		bFirstRequest = FALSE;
		bUpdate = TRUE;
	}

	utSiTime = pstTdtTable->unInfo.stTdt.utUtcTime;

	hErr = VK_CLOCK_GetTime ((unsigned long *)&utCurrTime);
	if ((VK_OK == hErr) && (utCurrTime > (60 * 60 * 24 * 365)))
	{
		HUINT32 		 ulDiff = (utSiTime > utCurrTime) ? (utSiTime - utCurrTime) : (utCurrTime - utSiTime);

		// greater then 1sec, smaller then 60sec, bigger then 60 is wrong time information
		if ( (ulDiff > 1) && (ulDiff < 60) )
		{
			bUpdate = TRUE;
		}
	}
	else
	{
		bUpdate = TRUE;
	}

	if (TRUE != bUpdate)
	{
		return ERR_FAIL;
	}

	if (TAPI_SetTime(TxSOURCE_SET_BY_SI, utSiTime) != ERR_OK)
	{
		HxLOG_Error("[TAPI_SetTime is Failed - time %u]\n", utSiTime);
	}

	return ERR_OK;
}

/* end of file */
