/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_media_play_mheg.c
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
#include <svc_av.h>
#include <svc_output.h>
#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_3rdparty.h>
#include <mgr_live.h>
#include <mgr_svclist.h>
#include <mgr_copyrightcontrol.h>
#include <mgr_output.h>

#include "_om_media_play.h"

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
STATIC HINT32					s_nMpMheg_RpcHandle;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___

#define _____OAPI_COMMAND_FUNCTIONS_____
STATIC HERROR om_media_play_mheg_CbStart (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR					hErr;
	HUINT32					 ulViewId = (HUINT32)HxOBJECT_INT (apArgv[1]);
	OxMediaPlay_StartInfo_t 	*punStart = (OxMediaPlay_StartInfo_t *)HxOBJECT_BIN_DATA (apArgv[2]);
	Handle_t					hSvc;

	if(punStart == NULL)
	{
		HxLOG_Error("punStart is Null\n");
		return ERR_FAIL;
	}

	hErr = DB_SVC_GetServiceHandleBySvcIdx(punStart->stLive.ulSuppleSvcUid, &hSvc);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx Error (ulSppleSvcUid : 0x%x) \n", punStart->stLive.ulSuppleSvcUid);
		return ERR_FAIL;
	}

	
	HxLOG_Debug ("ulViewId : %d, ulSuppleSvcUid : 0x%x, hSvc : 0x%x ) \n", ulViewId, punStart->stLive.ulSuppleSvcUid, hSvc);
	
	BUS_SendMessage(NULL, eMEVT_MHEG_BG_START, 0 , hSvc, ulViewId , 0);

	return ERR_OK;
}

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void om_media_play_mheg_CbMhegMonitorStarted (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify(s_nMpMheg_RpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegMonStarted, "iii", ulViewId, ulSessionId, eMediaType);
}

STATIC void om_media_play_mheg_CbMhegMonitorStopped (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify(s_nMpMheg_RpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegMonStopped, "iii", ulViewId, ulSessionId, eMediaType);
}

STATIC void om_media_play_mheg_CbMhegSvcStarted (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, Handle_t hSvc)
{
	HUINT32	ulSvcUid = DB_SVC_GetServiceUID(hSvc);

	HLIB_RPC_Notify(s_nMpMheg_RpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegSvcStarted, "iiii", ulViewId, ulSessionId, eMediaType, ulSvcUid);
}

STATIC void om_media_play_mheg_CbMhegSvcStopped (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify(s_nMpMheg_RpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegSvcStopped, "iii", ulViewId, ulSessionId, eMediaType);
}

STATIC void om_media_play_mheg_CbMhegDemuxChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulDemuxId)
{
	HLIB_RPC_Notify(s_nMpMheg_RpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegDmxChanged, "iiii", ulViewId, ulSessionId, eMediaType, ulDemuxId);
}

STATIC void om_media_play_mheg_CbOnMhegPmtChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, Handle_t hSvc, OxMediaPlay_MhegSectionData_t *pstMhegSecData, HUINT8 *pucPatRawSection, HUINT8 *pucPmtRawSection, OxMediaPlay_MhegPmtStatus_e	eMhegPmtState)
{
	HUINT32	ulSvcUid = DB_SVC_GetServiceUID(hSvc);

	HLIB_RPC_Notify(s_nMpMheg_RpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegPmtChanged, "iiiibbbi", ulViewId, ulSessionId, eMediaType, ulSvcUid,
						pstMhegSecData, sizeof(OxMediaPlay_MhegSectionData_t),
						pucPatRawSection, pstMhegSecData->ulPatTotalSecLen,
						pucPmtRawSection, pstMhegSecData->ulPmtTotalSecLen,
						eMhegPmtState);
}

#define _____PROCESS_FUNCTIONS_____
STATIC HERROR om_media_play_mheg_CombineSections (HUINT8 **ppaucRawSec, HUINT32 ulSecCount, HUINT32 *paulSecLen, HUINT32 *pulTotalSecLen, HUINT8 **ppucMergedRawSec)
{
	HUINT32		i;
	HUINT8		*pucRaw = NULL;
	HUINT8		*pucNewRawSec = NULL;
	HUINT32		ulTotalSecLen = 0, ulCurPos = 0, ulSize = 0;

	if ((ppaucRawSec == NULL)
		|| (ulSecCount < 1)
		|| (paulSecLen == NULL)
		|| (pulTotalSecLen == NULL)
		|| (ppucMergedRawSec == NULL))
	{
		HxLOG_Error("Invalid param..! \n");
		return ERR_FAIL;
	}

	// get section length
	for (i = 0; i < ulSecCount; i++)
	{
		pucRaw = ppaucRawSec[i];
		if(pucRaw == NULL)
		{
			HxLOG_Error("Invalid param..! \n");
			return ERR_FAIL;
		}

		ulSize = (HUINT32)HxMACRO_Get12Bit(pucRaw + 1) + 3;
		paulSecLen[i] = ulSize;
		ulTotalSecLen += ulSize;
	}

	if (ulTotalSecLen == 0)
	{
		HxLOG_Error("Check this..! Zero section length..!! \n");
		return ERR_FAIL;
	}

	// pack raw section for OAPI
	pucNewRawSec = (HUINT8 *)OxMGR_Malloc(sizeof(HUINT8) * ulTotalSecLen);
	if (pucNewRawSec == NULL)
	{
		HxLOG_Error("OxMGR_Calloc Error: size(%d) \n", ulTotalSecLen);
		return ERR_FAIL;
	}

	for (i = 0; i < ulSecCount; i++)
	{
		pucRaw = ppaucRawSec[i];
		ulSize = paulSecLen[i];

		HxSTD_MemCopy(&pucNewRawSec[ulCurPos], pucRaw, ulSize);
		ulCurPos += ulSize;
	}

	*pulTotalSecLen = ulTotalSecLen;
	*ppucMergedRawSec = pucNewRawSec;
	return ERR_OK;
}

#define _____MSG_PROCESS_FUNCTIONS_____
STATIC BUS_Result_t om_media_play_mheg_mevt3rdMhegMonitoringStarted (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulLiveActionId = (HUINT32)p2;
	HUINT32 	ulViewId;
	HUINT32 	ulSessionId = pstContext->ulSessionId;
	HERROR		hErr;

	if (pstContext->ulActionId == ulLiveActionId)
	{
		hErr = MGR_ACTION_GetViewIdByActionId(ulLiveActionId, &ulViewId);
		if (ERR_OK != hErr)
		{
			return MESSAGE_PASS;
		}

		om_media_play_mheg_CbMhegMonitorStarted(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mheg_mevt3rdMhegMonitoringStopped (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulLiveActionId = (HUINT32)p2;
	HUINT32 	ulViewId;
	HUINT32 	ulSessionId = pstContext->ulSessionId;
	HERROR		hErr;

	if (pstContext->ulActionId == ulLiveActionId)
	{
		hErr = MGR_ACTION_GetViewIdByActionId(ulLiveActionId, &ulViewId);
		if (ERR_OK != hErr)
		{
			return MESSAGE_PASS;
		}

		om_media_play_mheg_CbMhegMonitorStopped(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mheg_mevt3rdMhegServiceStarted (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulLiveActionId = (HUINT32)p2;
	HUINT32 	ulViewId;
	HUINT32 	ulSessionId = pstContext->ulSessionId;
	Handle_t	hSvc = (Handle_t)p1;
	HERROR		hErr;

	if (pstContext->ulActionId == ulLiveActionId)
	{
		hErr = MGR_ACTION_GetViewIdByActionId(ulLiveActionId, &ulViewId);
		if (ERR_OK != hErr)
		{
			return MESSAGE_PASS;
		}

		om_media_play_mheg_CbMhegSvcStarted(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, hSvc);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mheg_mevt3rdMhegServiceStopped (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulLiveActionId = (HUINT32)p2;
	HUINT32 	ulViewId;
	HUINT32 	ulSessionId = pstContext->ulSessionId;
	HERROR		hErr;

	if (pstContext->ulActionId == ulLiveActionId)
	{
		hErr = MGR_ACTION_GetViewIdByActionId(ulLiveActionId, &ulViewId);
		if (ERR_OK != hErr)
		{
			return MESSAGE_PASS;
		}

		om_media_play_mheg_CbMhegSvcStopped(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mheg_mevt3rdMhegDemuxChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulDemuxId = (HUINT32)p1;
	HUINT32		ulLiveActionId = (HUINT32)p2;
	HUINT32 	ulViewId;
	HUINT32 	ulSessionId = pstContext->ulSessionId;
	HERROR		hErr;

	if (pstContext->ulActionId == ulLiveActionId)
	{
		hErr = MGR_ACTION_GetViewIdByActionId(ulLiveActionId, &ulViewId);
		if (ERR_OK != hErr)
		{
			return MESSAGE_PASS;
		}

		om_media_play_mheg_CbMhegDemuxChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, ulDemuxId);
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mheg_mevt3rdMhegPmtChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		 ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32		 ulLiveActionId = (HUINT32)p2;
	HUINT32 	 ulSessionId = pstContext->ulSessionId;
	HUINT32 	 ulPatSecNum = 0, ulPmtSecNum = 0;
	HUINT8		*pucPatCombined = NULL, *pucPmtCombined = NULL;
	Handle_t	 hSvc = (Handle_t)p1;
	HERROR		 hErr;
	OxMediaPlay_MhegPmtStatus_e	eMhegPmtState = eMhegPmtStatus_None;


	switch(p3)
	{
		case eSEVT_SI_PMT_PIDREMOVED:	eMhegPmtState = eMhegPmtStatus_PidRemoved;	break;
		case eSEVT_SI_PMT_PIDCHANGED:	eMhegPmtState = eMhegPmtStatus_PidChanged;	break;
		case eSEVT_SI_PMT_TIMEOUT:	eMhegPmtState = eMhegPmtStatus_Timeout;	break;
		case eSEVT_SI_PMT:			eMhegPmtState = eMhegPmtStatus_Received;	break;
		default:
			HxLOG_Error("invalid evt si Mheg Pmt msg(%d) \n",  eMhegPmtState);
			return ERR_FAIL;
	}

	if (pstContext->ulActionId == ulLiveActionId)
	{
		HUINT32 	 ulViewId;
		HUINT32		 ulPatCombinedLen = 0, ulPmtCombinedLen = 0;
		HUINT8		*apucPatRawSec[OxMP_PAT_SECTION_NUM_MAX];
		HUINT8		*apucPmtRawSec[OxMP_PMT_SECTION_NUM_MAX];
		OxMediaPlay_MhegSectionData_t	 stMhegSecData;

		hErr = MGR_ACTION_GetViewIdByActionId(ulLiveActionId, &ulViewId);
		if (ERR_OK != hErr)
		{
			goto END_FUNC;
		}

		HxSTD_MemSet(&stMhegSecData, 0x00, sizeof(OxMediaPlay_MhegSectionData_t));
		HxSTD_MemSet(apucPatRawSec, 0x00, sizeof(apucPatRawSec));
		HxSTD_MemSet(apucPmtRawSec, 0x00, sizeof(apucPmtRawSec));

		// prepare PAT raw section for OAPI
		hErr = SVC_SI_GetMhegPatRawSection(ulActionId, OxMP_PAT_SECTION_NUM_MAX, apucPatRawSec, &ulPatSecNum);
		if ((ERR_OK == hErr) && (ulPatSecNum > 0))
		{
			(void)om_media_play_mheg_CombineSections(apucPatRawSec, ulPatSecNum, stMhegSecData.aulPatSecLen, &ulPatCombinedLen, &pucPatCombined);
		}

		if ((NULL == pucPatCombined) || (ulPatCombinedLen < 3))
		{
			if (NULL != pucPatCombined)				{ HLIB_STD_MemFree(pucPatCombined); }

			pucPatCombined = (HUINT8 *)HLIB_STD_MemCalloc(sizeof(HUINT8) * 4);
			if (NULL == pucPatCombined)
			{
				HxLOG_Error("Memory allocation failed:\n");
				goto END_FUNC;
			}

			ulPatCombinedLen = 4;
		}

		if(eMhegPmtState == eMhegPmtStatus_Received)
		{
			// prepare PMT raw section for OAPI
			hErr = SVC_SI_GetMhegPmtRawSection(ulActionId, OxMP_PMT_SECTION_NUM_MAX, apucPmtRawSec, &ulPmtSecNum);
			if ((ERR_OK == hErr) && (ulPmtSecNum > 0))
			{
				(void)om_media_play_mheg_CombineSections(apucPmtRawSec, ulPmtSecNum, stMhegSecData.aulPmtSecLen, &ulPmtCombinedLen, &pucPmtCombined);
			}
		}

		if ((NULL == pucPmtCombined) || (ulPmtCombinedLen < 3))
		{
			if (NULL != pucPmtCombined)				{ HLIB_STD_MemFree(pucPmtCombined); }

			pucPmtCombined = (HUINT8 *)HLIB_STD_MemCalloc(sizeof(HUINT8) * 4);
			if (NULL == pucPmtCombined)
			{
				HxLOG_Error("Memory allocation failed:\n");
				goto END_FUNC;
			}

			ulPmtCombinedLen = 4;
		}

		stMhegSecData.ulPatSecCount = ulPatSecNum;
		stMhegSecData.ulPatTotalSecLen = ulPatCombinedLen;
		stMhegSecData.ulPmtSecCount = ulPmtSecNum;
		stMhegSecData.ulPmtTotalSecLen = ulPmtCombinedLen;

		HxLOG_Debug("ulPatSecNum(%d),ulPatCombinedLen(%d), ulPmtSecNum(%d), ulPmtCombinedLen(%d), eMhegPmtState(%d)\n",ulPatSecNum,ulPatCombinedLen, ulPmtSecNum, ulPmtCombinedLen,  eMhegPmtState);

		om_media_play_mheg_CbOnMhegPmtChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_LIVE, hSvc, &stMhegSecData, pucPatCombined, pucPmtCombined, eMhegPmtState);
	}

END_FUNC:
	if (NULL != pucPatCombined)					{ OxMGR_Free(pucPatCombined); }
	if (NULL != pucPmtCombined)					{ OxMGR_Free(pucPmtCombined); }

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}


#define ___MEMBER_FUNCTIONS___
BUS_Result_t proc_om_media_play_mheg_Proto (omMediaPlay_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult;

	switch (message)
	{
	case eMEVT_3RD_MHEG_MONITORING_STARTED:
		eResult = om_media_play_mheg_mevt3rdMhegMonitoringStarted(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_3RD_MHEG_MONITORING_STOPPED:
		eResult = om_media_play_mheg_mevt3rdMhegMonitoringStopped(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_3RD_MHEG_SERVICE_STARTED:
		eResult = om_media_play_mheg_mevt3rdMhegServiceStarted(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_3RD_MHEG_SERVICE_STOPPED:
		eResult = om_media_play_mheg_mevt3rdMhegServiceStopped(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_3RD_MHEG_DEMUX_CHANGED:
		eResult = om_media_play_mheg_mevt3rdMhegDemuxChanged(pstContext, hHandle, p1, p2, p3);
		break;

	case eMEVT_3RD_MHEG_PMT_CHANGED:
		eResult = om_media_play_mheg_mevt3rdMhegPmtChanged(pstContext, hHandle, p1, p2, p3);
		break;

	default:
		eResult = MESSAGE_PASS;
		break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define _____RPC_INITIALIZATION_____
HERROR om_media_play_mheg_InitRpcFunctions (HINT32 nRpcHandle)
{
	HLIB_RPC_CreateNotification(nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegMonStarted,	"iii",		"MHEG MonStarted. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId)\n");
	HLIB_RPC_CreateNotification(nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegMonStopped,	"iii",		"MHEG MonStopped. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId)\n");
	HLIB_RPC_CreateNotification(nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegDmxChanged,	"iiii",		"MHEG DmxChanged. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, HUINT32 ulDemuxId)\n");
	HLIB_RPC_CreateNotification(nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegSvcStarted,	"iiii",		"MHEG SvcChanged. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, HUINT32 ulSvcUid)\n");
	HLIB_RPC_CreateNotification(nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegSvcStopped,	"iii",		"MHEG SvcStopped. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId)\n");
	HLIB_RPC_CreateNotification(nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnMhegPmtChanged,	"iiiibbbi",	"LIVE Pat/Pmt raw section.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, HUINT32 ulSvcUid,  OxMediaPlay_MhegSectionData_t *stMhegSecData, HUINT8 *pucPatRawSection, HUINT8 *pucPmtRawSection, OxMediaPlay_MhegPmtStatus_e	eMhegPmtState)\n");

	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_MhegBgStart,				"iib",	om_media_play_mheg_CbStart			, NULL,"Start the mheg: (HUINT32 ulSessionId, HUINT32 ulViewId, OxMediaPlay_StartInfo_t *punStart)");

	s_nMpMheg_RpcHandle = nRpcHandle;

	return ERR_OK;
}

#define _____DEBUG_FUNCTION_____


/* end of file */
