/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_live_eusat.c
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
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
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
STATIC HERROR xsvc_si_MediaPbParseDtcpDesc_Ertelecom(HUINT32 ulActionId, HUINT8 *pucRawDes)
{
	SIxErtelecomDtcpDes_t	*pstDesc = NULL;

	if (pucRawDes == NULL)
	{
		HxLOG_Error("null raw desc !!  \n");
		return ERR_FAIL;
	}

	pstDesc = (SIxErtelecomDtcpDes_t *)PAL_SIRAWPOOL_ParseDes (ulActionId, (HUINT8 *)pucRawDes);
	if (pstDesc == NULL)
	{
		HxLOG_Error("SIxErtelecomDtcpDes_t parse failed..!!  \n");
		return ERR_FAIL;
	}

	//found !
	HxLOG_Error("%s(%d) caSystemId(0x%X) \n",__FUNCTION__, __LINE__, pstDesc->usCaSystemId);

#ifdef CONFIG_SUPPORT_ROVI
	{
		Handle_t hAction = NULL;
		HUINT32 ulTempAPS = 0;

		hAction = PAL_PIPE_GetActionHandle(ulActionId);
		if(hAction == NULL)
		{
			HxLOG_Error("PAL_PIPE_GetActionHandle hAction is NULL \n");
		}
		ulTempAPS = (HUINT32)pstDesc->pstInfo->ucAPS;
		HxLOG_Debug("eSEVT_SI_DRM_SIGNALLED Emit \n");
		BUS_PostMessage(NULL, eSEVT_SI_DRM_SIGNALLED, hAction, 0, 0, ulTempAPS);
		HxLOG_Debug("APS Send to xproc_mplayer_Base() \n");
	}
#endif /* CONFIG_SUPPORT_ROVI */

	PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDesc);

	return ERR_OK;
}

STATIC HERROR xsvc_si_MediaPbCheckDtcpFromPmt_Ertelecom(HUINT32 ulActionId)
{
	SIxTable_PsiTable_t	*pstPmtTable = NULL;
	HxList_t			*pstPmtLoop = NULL;
	HUINT8				*pucRawDes = NULL;
	HERROR				 hResult = ERR_FAIL;
	HERROR				 hErr = ERR_FAIL;

	pstPmtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);
	if (NULL == pstPmtTable)
	{
		HxLOG_Error ("[%s:%d] PAL_SIRAWPOOL_ParseTable ulActionId=%d, PMT\n", __FUNCTION__, __LINE__, ulActionId);
		return ERR_FAIL;
	}

	// 1st loop desc. parsing
	for (pstPmtLoop = pstPmtTable->pstDesLoop; NULL != pstPmtLoop; pstPmtLoop = pstPmtLoop->next)
	{
		pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstPmtLoop);
		if (eSIxDESCTAG_ERTELECOM_DTCP == *pucRawDes)
		{
			hErr = xsvc_si_MediaPbParseDtcpDesc_Ertelecom(ulActionId, pucRawDes);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("xsvc_si_MediaPbParseDtcpDesc_Ertelecom() failed..!! \n");
			}
		}
	}

	// 2nd loop desc. parsing
	for (pstPmtLoop = pstPmtTable->pst2ndLoop; NULL != pstPmtLoop; pstPmtLoop = pstPmtLoop->next)
	{
		SIxTable_2ndLoop_t		*pst2ndItem = HLIB_LIST_Data (pstPmtLoop);
		HxList_t				*pstPmtDesLoop;

		if (NULL == pst2ndItem) 				{ continue; }

		for (pstPmtDesLoop = pst2ndItem->pstDesLoop; NULL != pstPmtDesLoop; pstPmtDesLoop = pstPmtDesLoop->next)
		{
			pucRawDes = (HUINT8 *)HLIB_LIST_Data (pstPmtDesLoop);
			if (eSIxDESCTAG_ERTELECOM_DTCP == *pucRawDes)
			{
				hErr = xsvc_si_MediaPbParseDtcpDesc_Ertelecom(ulActionId, pucRawDes);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("xsvc_si_MediaPbParseDtcpDesc_Ertelecom() failed..!! \n");
				}
			}
		}
	}

	hResult = ERR_OK;

	PAL_SIRAWPOOL_FreeTable (pstPmtTable);

	return hResult;
}

STATIC HERROR xsvc_si_MediapbOnPatReceived_Ertelecom (svcSi_MediapbData_t *pstMediapb, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	HERROR					 hErr = ERR_FAIL;
	HUINT32					 ulCnt, ulNumPmt = 0;
	SIxTable_PatProgram_t	*pstProgramArray = NULL;

	if (eSI_TABLE_TIMEOUT == pstSecCb->ulTableStatus)
	{
		if ((FILTER_ID_NULL == pstMediapb->aulTimeoutFilterId[eSIxTABLETYPE_Pat]) ||
			(pstSecCb->ulFilterId != pstMediapb->aulTimeoutFilterId[eSIxTABLETYPE_Pat]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_PAT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstMediapb->aulTimeoutFilterId[eSIxTABLETYPE_Pat] = pstSecCb->ulFilterId;
		return ERR_OK;
	}

	hErr = svc_si_LibGetAllPmtProgramsFromPat (ulActionId, &ulNumPmt, &pstProgramArray);
	if ((ERR_OK != hErr) || (0 == ulNumPmt) || (NULL == pstProgramArray))
	{
		HxLOG_Error ("[%s:%d] svc_si_LibGetAllPmtProgramsFromPat err: ActionId(%d)\n", __FUNCTION__, __LINE__, ulActionId);
		if (NULL != pstProgramArray)					{ OxSI_Free (pstProgramArray); }
		return ERR_FAIL;
	}

	for (ulCnt = 0; ulCnt < ulNumPmt; ulCnt++)
	{
		SIxTable_PatProgram_t	*pstPatProg = &(pstProgramArray[ulCnt]);

		pstMediapb->usPmtSvcId 	= pstPatProg->usProgNum;
		pstMediapb->usPmtPid 	= pstPatProg->usProgMapPid;

		hErr = svc_si_MediapbStartSection (pstMediapb, bSI_FILTERING_PMT);
		{
			HxLOG_Error ("[%s:%d] svc_si_MediapbStartSection err: ProgNum(%d), PID(%d)\n", __FUNCTION__, __LINE__, (HUINT32)pstPatProg->usProgNum, (HUINT32)pstPatProg->usProgMapPid);
		}
	}

	if (NULL != pstProgramArray)					{ OxSI_Free (pstProgramArray); }

	return ERR_OK;
}

STATIC HERROR xsvc_si_MediapbOnPmtReceived_Ertelecom (svcSi_MediapbData_t *pstMediapb, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstMediapb->aulTimeoutFilterId[eSIxTABLETYPE_Pmt]) ||
			(pstSecCb->ulFilterId != pstMediapb->aulTimeoutFilterId[eSIxTABLETYPE_Pmt]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_PMT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstMediapb->aulTimeoutFilterId[eSIxTABLETYPE_Pmt] = pstSecCb->ulFilterId;
		return ERR_OK;

	case eSI_TABLE_RECEIVED:
	default:
		svc_si_PostMsgToAp (eSEVT_SI_PMT, hAction, 0, 0, 0);
		xsvc_si_MediaPbCheckDtcpFromPmt_Ertelecom(PAL_PIPE_GetActionId(hAction));
		break;
	}

	return ERR_OK;
}

#define _____SI_MSG_FUNCTION_____

STATIC void xsvc_si_MediapbMsgCmdStartMediapb_Ertelecom (svcSi_MediapbData_t *pstMediapb, SvcSi_Msg_t *pstMsg)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(pstMsg->hAction);

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Critical ("[%s:%d] invalid actionHandle(0x%X)\n", __FUNCTION__, __LINE__, pstMediapb->hAction);
		return;
	}

	(void)PAL_SIRAWPOOL_ChangeSiSpecType (ulActionId, eSIxSPEC_ERTELECOM);
	(void)PAL_SIRAWPOOL_ChangeSiTextEncType (ulActionId, eSIxTEXT_ENC_300468_8859_9_BASE);

	(void)svc_si_MediapbSetMediapbData (pstMediapb, pstMsg->hAction);
	(void)svc_si_MediapbStartSection (pstMediapb, bSI_FILTERING_PAT);
}

STATIC void xsvc_si_MediapbMsgCmdStopMediapb_Ertelecom (svcSi_MediapbData_t *pstMediapb, SvcSi_Msg_t *pstMsg)
{
	(void)svc_si_MediapbStopSection (pstMediapb);
}

STATIC void xsvc_si_MediapbMsgEvtPatTable_Ertelecom (svcSi_MediapbData_t *pstMediapb, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_MediapbOnPatReceived_Ertelecom (pstMediapb, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

STATIC void xsvc_si_MediapbMsgEvtPmtTable_Ertelecom (svcSi_MediapbData_t *pstMediapb, SvcSi_Msg_t *pstMsg)
{
	(void)xsvc_si_MediapbOnPmtReceived_Ertelecom (pstMediapb, pstMsg->hAction, &(pstMsg->unArg.stSection));
}

#define _____SI_MSG_PROCESS_FUNCTION_____
STATIC void xsvc_si_MediapbProcessMsg_Ertelecom (SvcSi_Msg_t *pstMsg, void *pvData)
{
	svcSi_MediapbData_t	*pstMediapb = (svcSi_MediapbData_t *)pvData;

	if (NULL == pstMediapb)				{ return; }

	switch (pstMsg->eMsgClass)
	{
	case eSIMSG_CMD_START_MEDIAPB:
		xsvc_si_MediapbMsgCmdStartMediapb_Ertelecom (pstMediapb, pstMsg);
		break;

	case eSIMSG_CMD_STOP_MEDIAPB:
		xsvc_si_MediapbMsgCmdStopMediapb_Ertelecom (pstMediapb, pstMsg);
		break;

	case eSIMSG_EVT_PAT_TABLE:
		xsvc_si_MediapbMsgEvtPatTable_Ertelecom (pstMediapb, pstMsg);
		break;

	case eSIMSG_EVT_PMT_TABLE:
		xsvc_si_MediapbMsgEvtPmtTable_Ertelecom (pstMediapb, pstMsg);
		break;

	default:
		HxLOG_Debug ("[%s:%d] Non-processing MSG (0x%08x) -> skipped:\n", __FUNCTION__, __LINE__, pstMsg->eMsgClass);
		break;
	}
}

#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
SvcSi_ActFunc xsvc_si_MediapbGetMediapbActionFunc_Ertelecom (void)
{
	return xsvc_si_MediapbProcessMsg_Ertelecom;
}

void *xsvc_si_MediapbAllocMediapbActionData_Ertelecom (void)
{
	svcSi_MediapbData_t	*pstMediapb;

	pstMediapb = (svcSi_MediapbData_t *)OxSI_Calloc (sizeof(svcSi_MediapbData_t));
	if (NULL != pstMediapb)
	{
		pstMediapb->eTextEncType	= eSITEXT_ENCODED_300468_EUROPE;
	}

	return pstMediapb;
}

void xsvc_si_MediapbFreeMediapbActionData_Ertelecom (void *pvData)
{
	svcSi_MediapbData_t	*pstMediapb = (svcSi_MediapbData_t *)pvData;

	if (NULL != pstMediapb)
	{
		OxSI_Free (pstMediapb);
	}

	return;
}

/* end of file */

