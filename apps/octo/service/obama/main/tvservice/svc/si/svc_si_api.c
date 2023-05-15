/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_si_api.c
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
#include <octo_common.h>
#include <isosvc.h>

#include <svc_si.h>
#include <pal_pipe.h>
#include <pal_sirawpool.h>

#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_lcn.h>
#include <_svc_si_filtering.h>
#include <_svc_si_search.h>
#include <_svc_si_extsearch.h>
#include <_svc_si_satrecord.h>
#include <_svc_si_swup.h>
#include <_svc_si_dynamicupdate.h>
#include <psvc_si.h>

#if defined(CONFIG_MW_SI_AIT)
#include <_svc_si_hbbtv.h>
#endif

#if defined(CONFIG_3RD_MHEG)
#include <_svc_si_mheg.h>
#endif


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

#define ___API_FUNCTIONS___
HERROR SVC_SI_Init (PostMsgToMgrCb_t fnPostFunc, PostDataToMgrCb_t fnPostData)
{
	return svc_si_InitTask(fnPostFunc, fnPostData);
}

HERROR SVC_SI_SetCallback (SvcSi_GetSysConfigCb_t fnSetSysConfig)
{
	if (NULL == fnSetSysConfig)
		return ERR_FAIL;

	svc_si_SetCallback(fnSetSysConfig);

	return ERR_OK;
}

HERROR SVC_SI_SetAudioPriorityTable (SvcSi_EsTypePriority_t *astTable, HUINT32 ulNum)
{
	if (NULL == astTable || 0 >= ulNum)
		return ERR_FAIL;

	return svc_si_SetAudioPriorityTable(astTable, ulNum);
}

HERROR SVC_SI_SetSubtitlePriorityTable (SvcSi_EsTypePriority_t *astTable, HUINT32 ulNum)
{
	if (NULL == astTable || 0 >= ulNum)
		return ERR_FAIL;

	return svc_si_SetSubtitlePriorityTable(astTable, ulNum);
}

#define _____RECORD_____
HERROR SVC_SI_StartRec (Handle_t hAction, SvcSi_RecSpec_t *pstSiSpec, Handle_t hSvc, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (NULL == pstSiSpec)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_REC;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartRec.bSyncMode		= bSync;
	stMsg.unArg.stStartRec.phResult			= (bSync == TRUE) ? &hResult : NULL;
	stMsg.unArg.stStartRec.hMasterSvc		= hSvc;
	stMsg.unArg.stStartRec.hSuppleSvc		= hSvc;

	HxSTD_MemCopy (&(stMsg.unArg.stStartRec.unSpec), pstSiSpec, sizeof(SvcSi_RecSpec_t));
	stMsg.unArg.stStartRec.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_Record;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopRec (Handle_t hAction, Handle_t hNextSvc, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass						= eSIMSG_CMD_STOP_REC;
	stMsg.hAction						= hAction;
	stMsg.unArg.stStopRec.bSyncMode		= bSync;
	stMsg.unArg.stStopRec.phResult		= (bSync == TRUE) ? &hResult : NULL;
	stMsg.unArg.stStopRec.hNextSvc		= hNextSvc;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);

	return hResult;
}

#define _____SATIP_____
HERROR SVC_SI_StartSatip (Handle_t hAction, SvcSi_SatipSpec_t *pstSiSpec, Handle_t hSvc, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (NULL == pstSiSpec)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_SATIP;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartSatip.bSyncMode		= bSync;
	stMsg.unArg.stStartSatip.phResult		= (bSync == TRUE) ? &hResult : NULL;
	stMsg.unArg.stStartSatip.hMasterSvc		= hSvc;
	stMsg.unArg.stStartSatip.hSuppleSvc		= hSvc;

	HxSTD_MemCopy (&(stMsg.unArg.stStartSatip.unSpec), pstSiSpec, sizeof(SvcSi_SatipSpec_t));
	stMsg.unArg.stStartSatip.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_Satip;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopSatip (Handle_t hAction, Handle_t hNextSvc, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass						= eSIMSG_CMD_STOP_SATIP;
	stMsg.hAction						= hAction;
	stMsg.unArg.stStopSatip.bSyncMode	= bSync;
	stMsg.unArg.stStopSatip.phResult	= (bSync == TRUE) ? &hResult : NULL;
	stMsg.unArg.stStopSatip.hNextSvc	= hNextSvc;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);

	return hResult;
}


#define _____HBBTV_____
HERROR SVC_SI_StartHbbtv (Handle_t hAction, HUINT16 usOnId, HUINT16 usTsId, HUINT16 usSvcId, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_HBBTV;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartHbbtv.bSyncMode		= bSync;
	stMsg.unArg.stStartHbbtv.phResult		= (bSync == TRUE) ? &hResult : NULL;
	stMsg.unArg.stStartHbbtv.usOnId			= usOnId;
	stMsg.unArg.stStartHbbtv.usTsId			= usTsId;
	stMsg.unArg.stStartHbbtv.usSvcId		= usSvcId;

	stMsg.unArg.stStartHbbtv.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_HbbTv;
//	stMsg.unArg.stStartHbbtv.unSpec.stTemplate.ulActionSpec = eSVCSI_ACT_SPEC_EuSat;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopHbbtv (Handle_t hAction, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass						= eSIMSG_CMD_STOP_HBBTV;
	stMsg.hAction						= hAction;
	stMsg.unArg.stStopHbbtv.bSyncMode	= bSync;
	stMsg.unArg.stStopHbbtv.phResult	= (bSync == TRUE) ? &hResult : NULL;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);

	return hResult;
}


#define _____PLAYBACK_____
HERROR SVC_SI_StartPb (Handle_t hAction, SvcSi_PbSpec_t *pstSiSpec, SvcSi_PlaybackOption_t *pstPbOption, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if ((NULL == pstSiSpec) || (NULL == pstPbOption))
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_PB;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartPb.bSyncMode			= bSync;
	stMsg.unArg.stStartPb.phResult			= (bSync == TRUE) ? &hResult : NULL;

	HxSTD_MemCopy (&(stMsg.unArg.stStartPb.unSpec.stPbSpec), pstSiSpec, sizeof(SvcSi_PbSpec_t));
	HxSTD_MemCopy (&(stMsg.unArg.stStartPb.stOption), pstPbOption, sizeof(SvcSi_PlaybackOption_t));
	stMsg.unArg.stStartPb.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_Playback;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;

}

HERROR SVC_SI_StopPb (Handle_t hAction, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR 			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_STOP_PB;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStopPb.bSyncMode			= bSync;
	stMsg.unArg.stStopPb.phResult			= (bSync == TRUE) ? &hResult : NULL;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

#define _____MEDIA_PLAYBACK_____
HERROR SVC_SI_StartMediaPb (Handle_t hAction, SvcSi_MediaPbSpec_t *pstSiSpec, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (NULL == pstSiSpec)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_MEDIAPB;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartMediaPb.bSyncMode	= bSync;
	stMsg.unArg.stStartMediaPb.phResult		= (bSync == TRUE) ? &hResult : NULL;

	HxSTD_MemCopy (&(stMsg.unArg.stStartMediaPb.unSpec.stMediaPbSpec), pstSiSpec, sizeof(SvcSi_MediaPbSpec_t));
	stMsg.unArg.stStartPb.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_MediaPlayback;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;

}

HERROR SVC_SI_StopMediaPb (Handle_t hAction, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR 			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_STOP_MEDIAPB;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStopMediaPb.bSyncMode			= bSync;
	stMsg.unArg.stStopMediaPb.phResult			= (bSync == TRUE) ? &hResult : NULL;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}


#define _____LIVE_____

HERROR SVC_SI_StartLive (Handle_t hAction, SvcSi_LiveSpec_t *pstSiSpec, Handle_t hSvc, HBOOL bSync, HBOOL bFccSvc)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (NULL == pstSiSpec)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_LIVE;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartLive.bSyncMode		= bSync;
	stMsg.unArg.stStartLive.phResult		= (bSync == TRUE) ? &hResult : NULL;
	stMsg.unArg.stStartLive.hMasterSvc		= hSvc;
	stMsg.unArg.stStartLive.hSuppleSvc		= hSvc;
	stMsg.unArg.stStartLive.bFccSvc			= bFccSvc;

	HxSTD_MemCopy (&(stMsg.unArg.stStartLive.unSpec), pstSiSpec, sizeof(SvcSi_LiveSpec_t));
	stMsg.unArg.stStartLive.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_Live;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopLive (Handle_t hAction, Handle_t hNextSvc, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_STOP_LIVE;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStopLive.bSyncMode		= bSync;
	stMsg.unArg.stStopLive.phResult			= (bSync == TRUE) ? &hResult : NULL;
	stMsg.unArg.stStopLive.hNextSvc			= hNextSvc;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

#define _____SEARCH_____

HERROR SVC_SI_StartSearch (Handle_t hAction, SvcSi_SearchSpec_t *pstSrchSpec, SvcSi_SearchOption_t *pstSrchOption, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if ((NULL == pstSrchSpec) || (NULL == pstSrchOption))
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_SEARCH;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartSrch.bSyncMode		= bSync;

	stMsg.unArg.stStartSrch.phResult		= (bSync == TRUE) ? &hResult : NULL;
	HxSTD_MemCopy (&(stMsg.unArg.stStartSrch.unSpec.stSrchSpec), pstSrchSpec, sizeof(SvcSi_SearchSpec_t));
	HxSTD_MemCopy (&(stMsg.unArg.stStartSrch.stOption), pstSrchOption, sizeof(SvcSi_SearchOption_t));
	stMsg.unArg.stStartSrch.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_Search;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopSearch (Handle_t hAction, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_STOP_SEARCH;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStopSrch.bSyncMode		= bSync;
	stMsg.unArg.stStopSrch.phResult			= (bSync == TRUE) ? &hResult : NULL;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

#define _____EXT_SEARCH_____

// ses 에서는 ext search를 lcn search로 사용한다.
// 다른 op나 사양에서는 ext search를 다른 목적으로도 사용할 수 있다
HERROR SVC_SI_StartExtSearch (Handle_t hAction, SvcSi_ExtSearchSpec_t *pstSrchSpec, SvcSi_ExtSearchOption_t *pstSrchOption, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if ((NULL == pstSrchSpec) || (NULL == pstSrchOption))
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_EXTSEARCH;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartExtSrch.bSyncMode	= bSync;

	stMsg.unArg.stStartExtSrch.phResult		= (bSync == TRUE) ? &hResult : NULL;
	HxSTD_MemCopy (&(stMsg.unArg.stStartExtSrch.unSpec.stExtSrchSpec), pstSrchSpec, sizeof(SvcSi_ExtSearchSpec_t));
	HxSTD_MemCopy (&(stMsg.unArg.stStartExtSrch.stOption), pstSrchOption, sizeof(SvcSi_ExtSearchOption_t));
	stMsg.unArg.stStartExtSrch.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_ExtSearch;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopExtSearch (Handle_t hAction, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_STOP_EXTSEARCH;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStopExtSrch.bSyncMode		= bSync;
	stMsg.unArg.stStopExtSrch.phResult		= (bSync == TRUE) ? &hResult : NULL;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_SetExtSearchOption(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchOptionType_t eOptionType, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	return svc_si_ExtSrchSetSrchOption(hAction, pstExtSrchSpec, eOptionType, ulParam1, ulParam2, ulParam3);
}

HERROR SVC_SI_GetExtSearchData(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	return svc_si_ExtSrchGetSrchData(hAction, pstExtSrchSpec, eDataType, pulDataNum, ppvDataArray);
}

HERROR SVC_SI_FreeExtSearchData(Handle_t hAction, SvcSi_ExtSearchSpec_t *pstExtSrchSpec, SvcSi_ExtSrchDataType_t eDataType, void *pvDataArray)
{
	return svc_si_ExtSrchFreeSrchData(hAction, pstExtSrchSpec, eDataType, pvDataArray);
}

#define _____SAT_RECORD_____

HERROR SVC_SI_StartSatRecord (Handle_t hAction, SvcSi_SatRecordSpec_t *pstSrchSpec, SvcSi_SatRecordOption_t *pstSrchOption, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if ((NULL == pstSrchSpec) || (NULL == pstSrchOption))
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_SATRECORD;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartSatRecord.bSyncMode	= bSync;

	stMsg.unArg.stStartSatRecord.phResult	= (bSync == TRUE) ? &hResult : NULL;
	HxSTD_MemCopy (&(stMsg.unArg.stStartSatRecord.unSpec.stSatRecordSpec), pstSrchSpec, sizeof(SvcSi_SatRecordSpec_t));
	HxSTD_MemCopy (&(stMsg.unArg.stStartSatRecord.stOption), pstSrchOption, sizeof(SvcSi_SatRecordOption_t));
	stMsg.unArg.stStartSatRecord.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_SatRecord;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopSatRecord (Handle_t hAction, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_STOP_SATRECORD;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStopSatRecord.bSyncMode	= bSync;
	stMsg.unArg.stStopSatRecord.phResult	= (bSync == TRUE) ? &hResult : NULL;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_GetSatRecordData(Handle_t hAction, SvcSi_SatRecordSpec_t *pstSatRecordSpec, SvcSi_SatRecordDataType_t eDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	return svc_si_SatRecordGetSrchData(hAction, pstSatRecordSpec, eDataType, pulDataNum, ppvDataArray);
}

HERROR SVC_SI_FreeSatRecordData(Handle_t hAction, SvcSi_SatRecordSpec_t *pstSatRecordSpec, SvcSi_SatRecordDataType_t eDataType, void *pvDataArray)
{
	return svc_si_SatRecordFreeSrchData(hAction, pstSatRecordSpec, eDataType, pvDataArray);
}

#define _____OTA_____

HERROR SVC_SI_StartSwupCheck (Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupSpec, SvcSi_SwupCheckOption_t *pstOption, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if ((NULL == pstSwupSpec) || (NULL == pstOption))
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_OTACHECK;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartSwupCheck.bSyncMode	= bSync;
	stMsg.unArg.stStartSwupCheck.phResult	= (bSync == TRUE) ? &hResult : NULL;
	HxSTD_MemCopy (&(stMsg.unArg.stStartSwupCheck.unSpec.stSwupCheckSpec), pstSwupSpec, sizeof(SvcSi_SwupCheckSpec_t));
	HxSTD_MemCopy (&(stMsg.unArg.stStartSwupCheck.stOption), pstOption, sizeof(SvcSi_SwupCheckOption_t));
	stMsg.unArg.stStartSwupCheck.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_SwupCheck;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopSwupCheck (Handle_t hAction, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_STOP_OTACHECK;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStopSwupCheck.bSyncMode	= bSync;
	stMsg.unArg.stStopSwupCheck.phResult		= (bSync == TRUE) ? &hResult : NULL;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR	SVC_SI_SetSwupCheckParam (SvcSi_SwupCheckSpec_t *pstSwupSpec, SvcSi_SwupFlag_t *pstSwupCheckParam)
{
	if (NULL == pstSwupSpec)
		return ERR_FAIL;

	if (NULL == pstSwupCheckParam)
		return ERR_FAIL;

	return svc_si_SwupCheckSetCheckParam(pstSwupSpec, pstSwupCheckParam);
}

// 필요할 경우 SVC_SI_SetSwupCheckParam 를 먼저 call 해줘야 한다. SVC<->SVC 간 dependency 를 없애기 위해 mgr 단에서 필요한 정보 set 하도록 함.
HERROR SVC_SI_GetSwupCheckData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupSpec, HUINT32 ulDataType, HUINT32 *pulDataNum, void **ppvDataArray)
{
	return svc_si_SwupCheckGetData(hAction, pstSwupSpec, ulDataType, pulDataNum, ppvDataArray);
}

HERROR SVC_SI_FreeSwupCheckData(Handle_t hAction, SvcSi_SwupCheckSpec_t *pstSwupCheckSpec, HUINT32 ulDataType, void *pvDataArray)
{
	return svc_si_SwupCheckFreeData(hAction, pstSwupCheckSpec, ulDataType, pvDataArray);
}

#define _____LIVE_OTA_____

HERROR SVC_SI_StartLiveSwupCheck (Handle_t hAction, SvcSi_LiveSwupSpec_t *pstSpec, SvcSi_LiveSwupOption_t *pstOption, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	HxLOG_Debug("hAction: 0x%x,  ulActionId: 0x%x \n", hAction, ulActionId);

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if ((NULL == pstSpec) || (NULL == pstOption))
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_START_OTALIVE;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStartLiveSwupCheck.bSyncMode	= bSync;

	stMsg.unArg.stStartLiveSwupCheck.phResult		= (bSync == TRUE) ? &hResult : NULL;
	HxSTD_MemCopy (&(stMsg.unArg.stStartLiveSwupCheck.unSpec.stLiveSwupSpec), pstSpec, sizeof(SvcSi_LiveSwupSpec_t));
	HxSTD_MemCopy (&(stMsg.unArg.stStartLiveSwupCheck.stOption), pstOption, sizeof(SvcSi_LiveSwupOption_t));
	stMsg.unArg.stStartLiveSwupCheck.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_LiveSwupCheck;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopLiveSwupCheck (Handle_t hAction, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass							= eSIMSG_CMD_STOP_OTALIVE;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStopSwupCheck.bSyncMode	= bSync;
	stMsg.unArg.stStopSwupCheck.phResult		= (bSync == TRUE) ? &hResult : NULL;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}


#define _____TIME_UPDATE_____

HERROR SVC_SI_StartTimeupdate (Handle_t hAction, SvcSi_TimeUpdateSpec_t *pstSiSpec, HBOOL bUseTot, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (NULL == pstSiSpec)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass								= eSIMSG_CMD_START_TIMEUPDATE;
	stMsg.hAction								= hAction;
	stMsg.unArg.stStartTimeUpdate.bSyncMode		= bSync;
	stMsg.unArg.stStartTimeUpdate.phResult		= (bSync == TRUE) ? &hResult : NULL;
	stMsg.unArg.stStartTimeUpdate.bUseTot		= bUseTot;

	HxSTD_MemCopy (&(stMsg.unArg.stStartTimeUpdate.unSpec.stTimeUpdateSpec), pstSiSpec, sizeof(SvcSi_TimeUpdateSpec_t));
	stMsg.unArg.stStartTimeUpdate.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_TimeUpdate;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopTimeupdate (Handle_t hAction, HBOOL bSync)
{
	HUINT32 				 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t 			 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass 						= eSIMSG_CMD_STOP_TIMEUPDATE;
	stMsg.hAction							= hAction;
	stMsg.unArg.stStopTimeUpdate.bSyncMode	= bSync;
	stMsg.unArg.stStopTimeUpdate.phResult 	= (bSync == TRUE) ? &hResult : NULL;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

#define _____DYNAMIC_UPDATE_____

HERROR SVC_SI_StartDynamicUpdate (Handle_t hAction, SvcSi_DynamicUpdateSpec_t *pstSpec, SvcSi_DynamicUpdateOption_t *pstOption, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	HxLOG_Debug("hAction: 0x%x,  ulActionId: 0x%x \n", hAction, ulActionId);

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if ((NULL == pstSpec) || (NULL == pstOption))
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass									= eSIMSG_CMD_START_DYNAMICUPDATE;
	stMsg.hAction									= hAction;
	stMsg.unArg.stStartDynamicUpdate.bSyncMode		= bSync;

	stMsg.unArg.stStartDynamicUpdate.phResult		= (bSync == TRUE) ? &hResult : NULL;
	HxSTD_MemCopy (&(stMsg.unArg.stStartDynamicUpdate.unSpec.stDynamicUpdateSpec), pstSpec, sizeof(SvcSi_DynamicUpdateSpec_t));
	HxSTD_MemCopy (&(stMsg.unArg.stStartDynamicUpdate.stOption), pstOption, sizeof(SvcSi_DynamicUpdateOption_t));
	stMsg.unArg.stStartDynamicUpdate.unSpec.stTemplate.eActionKind = eSVCSI_ACT_KIND_DynamicUpdate;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

HERROR SVC_SI_StopDynamicUpdate (Handle_t hAction, HBOOL bSync)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	SvcSi_Msg_t				 stMsg;
	HERROR					 hErr = ERR_FAIL;
	STATIC HERROR			 hResult = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	HxSTD_MemSet (&stMsg, 0, sizeof(SvcSi_Msg_t));

	stMsg.eMsgClass									= eSIMSG_CMD_STOP_DYNAMICUPDATE;
	stMsg.hAction									= hAction;
	stMsg.unArg.stStopDynamicUpdate.bSyncMode		= bSync;
	stMsg.unArg.stStopDynamicUpdate.phResult		= (bSync == TRUE) ? &hResult : NULL;

	hErr = svc_si_SendMsgToSiTask (&stMsg);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_SendMsgToSiTask err:\n");
		return ERR_FAIL;
	}

	if (TRUE != bSync)
	{
		return ERR_OK;
	}

	svc_si_GetSyncSem (ulActionId);
	return hResult;
}

#define _____ES_RELATED_____

HERROR SVC_SI_GetPmtPidBySvcId (Handle_t hAction, HUINT32 ulSvcId, HUINT16 *pusPmtPid)
{
	HUINT16					 usPmtPid;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	svcSi_ContextState_e	 eState = eSvcSi_CtxtState_None;
	HERROR					 hErr;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (NULL == pusPmtPid)
	{
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiState(ulActionId, &eState);
	if ((ERR_OK != hErr) || (eSvcSi_CtxtState_Running != eState))
	{
		HxLOG_Error ("svc_si_GetSiState err(%d): hAction(0x08x), eState(%d)\n", hErr, hAction, eState);
		return ERR_FAIL;
	}

	hErr = svc_si_LibGetPmtPidFromPat(ulActionId, (HUINT16)ulSvcId, &usPmtPid);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_LibGetServiceInfoPid err:\n");
		return ERR_FAIL;
	}

	*pusPmtPid = usPmtPid;

	return ERR_OK;
}

HERROR SVC_SI_GetLiveServiceInfoPid (Handle_t hAction, HUINT32 ulSvcId, DbSvc_Info_t *pstSrcSvcInfo, DbSvc_Info_t *pstDstSvcInfo, HBOOL *pbChanged)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	svcSi_ContextState_e	 eState = eSvcSi_CtxtState_None;
	HERROR					 hErr;
	SvcSi_UserSettings_t	 stUserSettings = { { 0, }, };

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (NULL == pstDstSvcInfo)
	{
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiState(ulActionId, &eState);
	if ((ERR_OK != hErr) || (eSvcSi_CtxtState_Running != eState))
	{
		HxLOG_Error ("svc_si_GetSiState err(%d): hAction(0x08x), eState(%d)\n", hErr, hAction, eState);
		return ERR_FAIL;
	}

	(void)svc_si_SvcInfo2UserSettings((const DbSvc_Info_t *)pstSrcSvcInfo, &stUserSettings);

	hErr = svc_si_LibGetServiceInfoPid(ulActionId, ulSvcId, pstSrcSvcInfo, pstDstSvcInfo, svc_si_GetSysConfig(&stUserSettings), pbChanged);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_LibGetServiceInfoPid err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#if 0
HERROR SVC_SI_GetServiceInfoPid(Handle_t hAction, HUINT32 ulSvcId, void *pvSrcInfo, void *pvDstInfo, HBOOL *pbChanged)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	svcSi_ContextState_e	 eState = eSvcSi_CtxtState_None;
	HERROR					 hErr;
	SvcSi_UserSettings_t	 stUserSettings = { { 0, }, };

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (NULL == pvDstInfo)
	{
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiState(ulActionId, &eState);
	if ((ERR_OK != hErr) || (eSvcSi_CtxtState_Running != eState))
	{
		HxLOG_Error ("svc_si_GetSiState err(%d): hAction(0x08x), eState(%d)\n", hErr, hAction, eState);
		return ERR_FAIL;
	}

	(void)svc_si_SvcInfo2UserSettings((const DbSvc_Info_t *)pvSrcInfo, &stUserSettings);

	hErr = svc_si_LibGetServiceInfoPid(ulActionId, ulSvcId, pvSrcInfo, pvDstInfo, svc_si_GetSysConfig(&stUserSettings), pbChanged);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_LibGetServiceInfoPid err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

#if defined(SvcSi_SUPPORT_LIB_ES_NEW)
STATIC HBOOL svc_si_CheckUpdateEsAvailable(SvcSi_UpdateEs_t *pstUpdate)
{
	if (NULL == pstUpdate)
	{
		HxLOG_Error ("Error: pstUpdate\n");
		return FALSE;
	}

	if ((NULL == pstUpdate->pvOrgInfo) || (NULL == pstUpdate->pvDstInfo))
	{
		HxLOG_Error ("Error: pstUpdate->pvOrgInfo or pstUpdate->pvDstInfo\n");
		return FALSE;
	}

	if ((0 == pstUpdate->ulSize))
	{
		HxLOG_Error ("Error: pstUpdate->ulSize\n");
		return FALSE;
	}

	if ((NULL == pstUpdate->fnUpdateEs) || (NULL == pstUpdate->fnUpdateSvc) || (NULL == pstUpdate->fnCompareSvc))
	{
		HxLOG_Error ("Error: pstUpdate->callbackFn\n");
		return FALSE;
	}

	return TRUE;
}

HERROR SVC_SI_UpdateServiceInfo (Handle_t hAction, HUINT32 ulSvcId, SvcSi_UpdateEs_t *pstUpdate, HBOOL *pbChanged)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	svcSi_ContextState_e	 eState = eSvcSi_CtxtState_None;
	HERROR					 hErr;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (TRUE != svc_si_CheckUpdateEsAvailable(pstUpdate))
	{
		HxLOG_Error ("svc_si_CheckUpdateEsAvailable Error:\n");
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiState(ulActionId, &eState);
	if ((ERR_OK != hErr) || (eSvcSi_CtxtState_Running != eState))
	{
		HxLOG_Error ("svc_si_GetSiState err(%d): hAction(0x08x), eState(%d)\n", hErr, hAction, eState);
		return ERR_FAIL;
	}

	// Copy
	HxSTD_MemCopy (pstUpdate->pvDstInfo, pstUpdate->pvOrgInfo, pstUpdate->ulSize);

	hErr = svc_si_LibGetServiceInfoPid2(ulActionId, ulSvcId, (const SvcSi_UpdateEs_t *)pstUpdate, pbChanged);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_LibGetServiceInfoPid2 err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_SI_GetServiceInfoPid2(Handle_t hAction, HUINT32 ulSvcId, SvcSi_UpdateEs_t *pstUpdate, HBOOL *pbChanged)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	svcSi_ContextState_e	 eState = eSvcSi_CtxtState_None;
	HERROR					 hErr;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (TRUE != svc_si_CheckUpdateEsAvailable(pstUpdate))
	{
		HxLOG_Error ("Error:\n");
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiState(ulActionId, &eState);
	if ((ERR_OK != hErr) || (eSvcSi_CtxtState_Running != eState))
	{
		HxLOG_Error ("svc_si_GetSiState err(%d): hAction(0x08x), eState(%d)\n", hErr, hAction, eState);
		return ERR_FAIL;
	}

	// Copy
	HxSTD_MemCopy (pstUpdate->pvDstInfo, pstUpdate->pvOrgInfo, pstUpdate->ulSize);

	hErr = svc_si_LibGetServiceInfoPid2(ulActionId, ulSvcId, (const SvcSi_UpdateEs_t *)pstUpdate, pbChanged);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_LibGetServiceInfoPid2 err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_SI_GetEsInfo(SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, HINT32 nIndex, HUINT16 *pusPid, HUINT32 *pulCodec, HUINT8 *pucComponentTag, HUINT32 *pulParam)
{
	if (HANDLE_NULL == hEsInfoList)
		return ERR_FAIL;

	if (0 > nIndex)
		return ERR_FAIL;

	if (NULL == pusPid)
		return ERR_FAIL;

	return svc_si_LibGetEsInfo(eType, hEsInfoList, nIndex, pusPid, pulCodec, pucComponentTag, pulParam);
}
#endif

// SDT에서 변경된 정보가 있다면 svcInfo에 update한다
HERROR SVC_SI_GetLiveServiceInfo(Handle_t hAction, HUINT32 ulSvcId, DbSvc_Info_t *pstSrcSvcInfo, DbSvc_Info_t *pstDstSvcInfo, HBOOL *pbChanged)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	svcSi_ContextState_e	 eState = eSvcSi_CtxtState_None;
	HERROR					 hErr;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(0x%08x)\n", hAction);
		return ERR_FAIL;
	}

	if (NULL == pstDstSvcInfo)
	{
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiState(ulActionId, &eState);
	if ((ERR_OK != hErr) || (eSvcSi_CtxtState_Running != eState))
	{
		HxLOG_Error ("svc_si_GetSiState err(%d): hAction(0x08x), eState(%d)\n", hErr, hAction, eState);
		return ERR_FAIL;
	}

	hErr = svc_si_LibGetServiceInfo(ulActionId, ulSvcId, pstSrcSvcInfo, pstDstSvcInfo, pbChanged);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_LibGetServiceInfoPid err:\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_SI_UpdateAllService_inSDT(HUINT32 ulActionId, const DbSvc_Info_t *pstSvcInfo, HBOOL *pbChanged)
{
	return svc_si_LibUpdateAllService_inSDT(ulActionId, pstSvcInfo, pbChanged);
}

// 이 함수를 호출하면 이후에 항상 SVC_SI_FreeEventInfo() 를 불러줘야 함~
HERROR SVC_SI_GetEvtInfoFromPfEvent (HUINT32 ulActionId, DbSvc_TripleId_t stTripleId, HBOOL bNow, HBOOL bOnlyEit, SvcSi_Event_t *pEvent, SvcSi_ExtEvent_t *pExtEvent)
{
	HERROR			hErr = ERR_FAIL;
	HBOOL			bUsePmt = FALSE;

	if (pEvent == NULL)
	{
		HxLOG_Error ("Invalid parameter \n");
		return ERR_FAIL;
	}

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(%d)\n", ulActionId);
		return ERR_FAIL;
	}
	bUsePmt = (bOnlyEit == TRUE) ? FALSE : TRUE;

	hErr = svc_si_LibGetEventInfoFromEitPf (ulActionId, stTripleId, bNow, bUsePmt, pEvent, pExtEvent);

	return hErr;
}

HERROR SVC_SI_FreeEventInfo (SvcSi_Event_t *pstEvent)
{
	return svc_si_LibFreeEventInfo(pstEvent);
}

HERROR SVC_SI_FreeExtEventInfo(SvcSi_ExtEvent_t * pstExtEvent)
{
	return svc_si_LibFreeExtEventInfo(pstExtEvent);
}

HERROR SVC_SI_GetVideoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_VideoList_t *pstVideoList)
{
	HERROR					 hErr;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	if (NULL == pstVideoList)
	{
		return ERR_FAIL;
	}

	pstVideoList->nVideo = 0;
	pstVideoList->pstVideoInfo = NULL;

	hErr = svc_si_LibGetVideoInfoList(ulActionId, usSvcId, pstVideoList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_LibGetAudioInfoList err(%d)\n", hErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_SI_GetAudioList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_AudioList_t *pstAudioList)
{
	HERROR					 hErr;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	if (NULL == pstAudioList)
	{
		return ERR_FAIL;
	}

	pstAudioList->nAudio = 0;
	pstAudioList->pstAudioInfo = NULL;

	hErr = svc_si_LibGetAudioInfoList(ulActionId, usSvcId, pstAudioList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_LibGetAudioInfoList err(%d)\n", hErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_SI_GetEcmList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_EcmList_t *pstEcmList)
{
	HERROR					 hErr;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	if (NULL == pstEcmList)
	{
		return ERR_FAIL;
	}

	pstEcmList->nEcm = 0;
	pstEcmList->pstEcmInfo = NULL;

	hErr = svc_si_LibGetEcmInfoList(ulActionId, usSvcId, pstEcmList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_LibGetEcmInfoList err(%d)\n", hErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_SI_GetSubtitleList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_SubtitleList_t *pstSubtitleList)
{
	HERROR					 hErr = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	if (NULL == pstSubtitleList)
	{
		return ERR_FAIL;
	}

	pstSubtitleList->ulSubtitleNum = 0;
	pstSubtitleList->pstSubtitleInfo = NULL;

	hErr = svc_si_LibGetSubtitleInfoList(ulActionId, usSvcId, pstSubtitleList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_LibGetAudioInfoList err(%d)\n", hErr);
	}

	return hErr;
}

/* Superimpose + ClosedCaption :  */
HERROR SVC_SI_GetSuperCcList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_SubtitleList_t *pstSuperCcList)
{
	HERROR					 hErr = ERR_FAIL;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	if (NULL == pstSuperCcList)
	{
		return ERR_FAIL;
	}

	pstSuperCcList->ulSubtitleNum = 0;
	pstSuperCcList->pstSubtitleInfo = NULL;

	hErr = svc_si_LibGetSuperCcInfoList(ulActionId, usSvcId, pstSuperCcList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_LibGetAudioInfoList err(%d)\n", hErr);
	}

	return hErr;
}

HERROR SVC_SI_FreeVideoList (SvcSi_VideoList_t *pstVideoList)
{
	return svc_si_LibFreeVideoList(pstVideoList);
}

HERROR SVC_SI_FreeAudioList (SvcSi_AudioList_t *pstAudioList)
{
	return svc_si_LibFreeAudioList(pstAudioList);
}

HERROR SVC_SI_FreeSubtitleList (SvcSi_SubtitleList_t *pstSubtitleList)
{
	return svc_si_LibFreeSubtitleList(pstSubtitleList);
}

HERROR SVC_SI_FreeSuperCcList (SvcSi_SubtitleList_t *pstSubtitleList)
{
	return svc_si_LibFreeSubtitleList(pstSubtitleList);
}

HERROR SVC_SI_FreeEcmList (SvcSi_EcmList_t *pstEcmList)
{
	return svc_si_LibFreeEcmList(pstEcmList);
}

#define _____CAT_RELATED_____
HERROR SVC_SI_GetEmmList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_EmmList_t *pstEmmList)
{
	HERROR					 hErr;

	if (ulActionId >= eActionId_MAX)
	{
		HxLOG_Error ("Wrong ActionHandle(%d)\n", ulActionId);
		return ERR_FAIL;
	}

	if (NULL == pstEmmList)
	{
		return ERR_FAIL;
	}

	pstEmmList->nEmm = 0;
	pstEmmList->pstEmmInfo = NULL;

	hErr = svc_si_LibGetEmmInfoList(ulActionId, usSvcId, pstEmmList);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("svc_si_LibGetEmmInfoList err(%d)\n", hErr);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_SI_FreeEmmList (SvcSi_EmmList_t *pstEmmList)
{
	return svc_si_LibFreeEmmList(pstEmmList);
}

#define _____SEARCH_RESULT_____

HERROR SVC_SI_CountSearchedResult (HUINT32 ulActionId, SvcSi_SrchInfoType_e eInfoType, HUINT32 *pulInfoNum)
{
	svcSi_ContextState_e	 eState = eSvcSi_CtxtState_None;
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	if ((ulActionId >= eActionId_MAX) || (eInfoType >= eSI_SRCH_INFOTYPE_MAX) || (NULL == pulInfoNum))
	{
		HxLOG_Error("[%s:%d] invalid param. : actionId(%d) infoType(%d) ", ulActionId, eInfoType);
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiState (ulActionId, &eState);
	if ((ERR_OK != hErr) || (eSvcSi_CtxtState_Running != eState))
	{
		HxLOG_Error ("svc_si_GetSiState err(%d): ulActionId(%d), eState(%d)\n", hErr, ulActionId, eState);
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err(%d): ulActionId(%d)\n", hErr, ulActionId, eState);
		return ERR_FAIL;
	}

	HxLOG_Info("unSiSpec.stTemplate.eActionKind: %d \n", unSiSpec.stTemplate.eActionKind);

	switch (unSiSpec.stTemplate.eActionKind)
	{
	case eSVCSI_ACT_KIND_Search:
	case eSVCSI_ACT_KIND_ExtSearch:
		hErr = svc_si_SrchCountSearchedResult (ulActionId, eInfoType, pulInfoNum);
		break;

	case eSVCSI_ACT_KIND_DynamicUpdate:
		hErr = svc_si_DuCountSearchedResult (ulActionId, eInfoType, pulInfoNum);
		break;

	default:
		HxLOG_Error ("Unsupported Action Kind:(%d)\n", unSiSpec.stTemplate.eActionKind);
		return ERR_FAIL;
	}

	return hErr;
}

HERROR SVC_SI_GetSearchedResults (HUINT32 ulActionId, SvcSi_SrchInfoType_e eInfoType, HUINT32 ulMaxArrayNum, HUINT32 *pulInfoNum, void *pvInfoArray)
{
	svcSi_ContextState_e	 eState = eSvcSi_CtxtState_None;
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	if ((ulActionId >= eActionId_MAX) || (eInfoType >= eSI_SRCH_INFOTYPE_MAX) || (0 == ulMaxArrayNum) || (NULL == pvInfoArray))
	{
		return ERR_FAIL;
	}

	if (NULL != pulInfoNum)					{ *pulInfoNum = 0; }

	hErr = svc_si_GetSiState (ulActionId, &eState);
	if ((ERR_OK != hErr) || (eSvcSi_CtxtState_Running != eState))
	{
		HxLOG_Error ("svc_si_GetSiState err(%d): ulActionId(%d), eState(%d)\n", hErr, ulActionId, eState);
		return ERR_FAIL;
	}

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err(%d): ulActionId(%d)\n", hErr, ulActionId);
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.eActionKind)
	{
	case eSVCSI_ACT_KIND_Search:
	case eSVCSI_ACT_KIND_ExtSearch:
		hErr = svc_si_SrchGetSearchedResults (ulActionId, eInfoType, ulMaxArrayNum, pulInfoNum, pvInfoArray);
		break;

	case eSVCSI_ACT_KIND_DynamicUpdate:
		hErr = svc_si_DuGetSearchedResults (ulActionId, eInfoType, ulMaxArrayNum, pulInfoNum, pvInfoArray);
		break;

	default:
		HxLOG_Error ("Unsupported Action Kind:(%d)\n", unSiSpec.stTemplate.eActionKind);
		return ERR_FAIL;
	}

	return hErr;
}

#define _____SI_TABLE_____

SIxTable_PsiTable_t *SVC_SI_ParseTable (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulExtId)
{
	return PAL_SIRAWPOOL_ParseTable(ulActionId, eSiType, ulExtId);
}

void SVC_SI_FreeTable (SIxTable_PsiTable_t *pstTable)
{
	PAL_SIRAWPOOL_FreeTable(pstTable);
}

#define _____ETC_____

HERROR SVC_SI_MakeSiSection (HUINT32 ulActionId, SvcSi_MakeSectionSpec_t *pstSpec, SvcSi_MakeSecArg_t *punArg, HUINT8 *pucRawSec, HUINT32 *pulSecLen)
{
	return svc_si_LibMakeSiSection(ulActionId, pstSpec, punArg, pucRawSec, pulSecLen);
}

HERROR SVC_SI_GetAitRawSection(HUINT32 ulActionId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount)
{
	HERROR	hErr = ERR_FAIL;

#if defined(CONFIG_MW_SI_AIT)
	hErr = svc_si_HbbtvGetAitSection(ulActionId, ulRawArrayMax, ppucRawArray, pulListCount);
#endif

	return hErr;
}

HERROR SVC_SI_GetAitPidFromPMT(HUINT32 ulActionId, HxList_t **pstAitList)
{
	HERROR					 hErr = ERR_FAIL;

	hErr = svc_si_LibGetAllAitPidFromPmt(ulActionId, pstAitList);

	return hErr;
}



HERROR	SVC_SI_SetLcnServiceNumber(Handle_t hAction, HBOOL bIsNewTp, HUINT32 ulNum, DbSvc_Info_t *pstSvcArray, SRCHSVC_AppendFlag_t *peFlagArray)
{
	HUINT32					 ulActionId = PAL_PIPE_GetActionId(hAction);
	SvcSi_SiSpec_t			 unSiSpec;
	HERROR					 hErr;

	if (0 == ulNum)								{ HxLOG_Error("ulNum is 0 \n"); return ERR_FAIL; }
	if (NULL == pstSvcArray)					{ HxLOG_Error("pstSvcArray is NULL \n"); return ERR_FAIL; }
	if (NULL == peFlagArray)					{ HxLOG_Error("peFlagArray is NULL \n"); return ERR_FAIL; }

	hErr = svc_si_GetSiSpec (ulActionId, &unSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("svc_si_GetSiSpec err(%d): ulActionId(%d)\n", hErr, ulActionId);
		return ERR_FAIL;
	}

	switch (unSiSpec.stTemplate.eActionKind)
	{
	case eSVCSI_ACT_KIND_Search:
	case eSVCSI_ACT_KIND_ExtSearch:
		hErr = svc_si_LcnSetServiceNumbers(hAction, unSiSpec.stTemplate.ulActionSpec, FALSE, bIsNewTp, ulNum, pstSvcArray, peFlagArray);
		break;

	case eSVCSI_ACT_KIND_DynamicUpdate:
		hErr = svc_si_LcnSetServiceNumbers(hAction, unSiSpec.stTemplate.ulActionSpec, TRUE, bIsNewTp, ulNum, pstSvcArray, peFlagArray);
		break;

	default:
		HxLOG_Error ("Unsupported Action Kind:(%d)\n", unSiSpec.stTemplate.eActionKind);
		return ERR_FAIL;
	}

	return hErr;
}

HERROR	SVC_SI_SetSelectedConflictLcn(Handle_t hAction, HUINT32 ulNum, DxBuilder_SearchedInfo_t* pstInfoArray)
{
	HERROR					 hErr;

	if (0 == ulNum)								{ HxLOG_Error("ulNum is 0 \n"); return ERR_FAIL; }
	if (NULL == pstInfoArray)					{ HxLOG_Error("pstInfoArray is NULL \n"); return ERR_FAIL; }

	hErr = svc_si_LcnSetSelectedConflictLcn(hAction, ulNum, pstInfoArray);

	return hErr;
}

HERROR SVC_SI_PostProcessForSvcNumbering (Handle_t hAction, DefaultLcnPolicy_t eLcnPolicy, DxDeliveryType_e eDeliType)
{
	return psvc_si_PostProcessForSvcNumbering (hAction, eLcnPolicy, eDeliType);
}

HERROR SVC_SI_PostProcessForSimulcastService(Handle_t hAction)
{
	return psvc_si_PostProcessForSimulcastService(hAction);
}

HERROR SVC_SI_UpdateLiveTimeByTot (Handle_t hAction)
{
	return svc_si_LibUpdateTimeByTot(PAL_PIPE_GetActionId(hAction));
}

#if defined(CONFIG_MW_SI_LOCALTIME)
HERROR SVC_SI_UpdateLocaltime_linkTzDB(HxCountry_e eCountryId, HINT32 nTimeRegion)
{
	return psvc_si_UpdateLocaltime_linkTzDB(eCountryId, nTimeRegion);
}
#endif

HERROR SVC_SI_GetSiProperty (HUINT32 ulActionId, SIxSpec_e *peSpec, SIxTextEncoding_e *peTextEnc)
{
	return PAL_SIRAWPOOL_GetSiProperty(ulActionId, peSpec, peTextEnc);
}

HERROR	SVC_SI_SetSiMode (DxSIMode_e ulSiMode)
{
	return psvc_si_SetSiMode ((HUINT32)ulSiMode);
}

HERROR	SVC_SI_GetSiMode (DxSIMode_e *pulSiMode)
{
	return psvc_si_GetSiMode ((HUINT32*)pulSiMode);
}

HERROR	SVC_SI_GetSwupFlag (Handle_t hAction, HUINT32 *pulSwupFlag)
{
	return psvc_si_GetSwupCheckFlag (hAction, pulSwupFlag);
}


#if defined(CONFIG_MW_SI_AIT)
#define SvcSi_WEB_PMT_LIST_COUNT_MAX				(30)

STATIC SvcSi_WebPmtInfo_t		s_astWebPmtList[SvcSi_WEB_PMT_LIST_COUNT_MAX];
#endif

HERROR SVC_SI_GetWebPmtList(Handle_t hAction, SvcSi_WebPmtInfo_t **apList, HUINT32 *punListCount, HUINT16 *pusTsUniqId)
{
#if defined(CONFIG_MW_SI_AIT)
	HUINT16					 usTsUniqId	= 0;
	HUINT32 				 ulActionId;
	SIxTable_PsiTable_t		*pstPmtTable = NULL, *pstCurrPmt = NULL;
	HxList_t				*pst2ndLoop = NULL;
	HUINT32					 ulIdx = 0, ulListCount=0;

	if (NULL == apList)
		return ERR_FAIL;

	HxSTD_memset(&s_astWebPmtList, 0x0, sizeof(SvcSi_WebPmtInfo_t)*SvcSi_WEB_PMT_LIST_COUNT_MAX);

	usTsUniqId = DB_SVC_GetAllUniqueId();

	if (pusTsUniqId)
	{
		*pusTsUniqId = usTsUniqId;
	}

	ulActionId = PAL_PIPE_GetActionId (hAction);

	pstCurrPmt = pstPmtTable = SVC_SI_ParseTable(ulActionId, eSIxTABLETYPE_Pmt, SILIB_TABLE_EXT_ID_ALL);

	while(pstCurrPmt!=NULL)
	{
		HBOOL		 bDataBroadcastIdFound = FALSE, bCarouselIdFound = FALSE;
		HUINT16		 usDataBroadcastId = 0;
		HUINT32		 ulCarouselId = 0;

		for (pst2ndLoop = pstCurrPmt->pst2ndLoop; NULL != pst2ndLoop; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t*)HLIB_LIST_Data (pst2ndLoop);
			SIxTable_PmtElement_t	*pstPmtElem = NULL;

			pstPmtElem = (SIxTable_PmtElement_t*)&(pst2ndItem->unInfo.stPmt);
			if(pstPmtElem  == NULL)	{continue;}

			if((pstPmtElem->ucStreamType == 0x0A || pstPmtElem->ucStreamType == 0x0B ||pstPmtElem->ucStreamType == 0x0C))
			{
				HxList_t				*pstDescList = NULL;

				for (pstDescList = pst2ndItem->pstDesLoop; NULL != pstDescList; pstDescList = pstDescList->next)
				{
					HUINT8					*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstDescList);
					SIxDescriptor_t 		*pstDes = NULL;

					switch (*pucDesRaw)
					{
					case	eSIxDESCTAG_DATA_BROADCAST_ID:
						{
							SIxDataBroadcastIdDes_t 		*pstDataBroadcastIdDes = NULL;

							if (!bDataBroadcastIdFound)
							{
								pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
								if(pstDes !=NULL)
								{
									pstDataBroadcastIdDes = (SIxDataBroadcastIdDes_t *)pstDes;

									usDataBroadcastId = pstDataBroadcastIdDes->usDataBroadcastId;
									bDataBroadcastIdFound = TRUE;

									PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDes);
								}
							}
						}
						break;

					case eSIxDESCTAG_CAROUSEL_ID:
						{
							SIxCarouselIdDes_t	*pstCarouselIdDesDes = NULL;

							if (!bCarouselIdFound)
							{
								pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
								if(pstDes !=NULL)
								{
									pstCarouselIdDesDes = (SIxCarouselIdDes_t *)pstDes;

									ulCarouselId = pstCarouselIdDesDes->ulCarouselId;
									bCarouselIdFound = TRUE;

									PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDes);
								}
							}
						}

						break;

					default:
						break;
					}
				}
			}

			if (bDataBroadcastIdFound && bCarouselIdFound)
			{
				break;
			}
		}

		for (pst2ndLoop = pstCurrPmt->pst2ndLoop; NULL != pst2ndLoop; pst2ndLoop = pst2ndLoop->next)
		{
			SIxTable_2ndLoop_t		*pst2ndItem = (SIxTable_2ndLoop_t*)HLIB_LIST_Data (pst2ndLoop);
			SIxTable_PmtElement_t	*pstPmtElem = NULL;

			pstPmtElem = (SIxTable_PmtElement_t*)&(pst2ndItem->unInfo.stPmt);
			if(pstPmtElem  == NULL)	{continue;}

			if((pstPmtElem->ucStreamType == 0x0A || pstPmtElem->ucStreamType == 0x0B ||pstPmtElem->ucStreamType == 0x0C))
			{
				HxList_t				*pstDescList = NULL;

				s_astWebPmtList[ulIdx].ucStreamType 	=  pstPmtElem->ucStreamType;
				s_astWebPmtList[ulIdx].usPid 		=  pstPmtElem->usElemPid;
				if (bDataBroadcastIdFound)
				{
					s_astWebPmtList[ulIdx].usDataBroadcastTag = usDataBroadcastId;
				}

				if (bCarouselIdFound)
				{
					s_astWebPmtList[ulIdx].ulCarouselIDTag = ulCarouselId;
				}

				for (pstDescList = pst2ndItem->pstDesLoop; NULL != pstDescList; pstDescList = pstDescList->next)
				{
					HUINT8					*pucDesRaw = (HUINT8 *)HLIB_LIST_Data (pstDescList);
					SIxDescriptor_t 		*pstDes = NULL;

					switch (*pucDesRaw)
					{
						case eSIxDESCTAG_STREAM_IDENTIFIER:
							{
								SIxStmIdentDes_t 			*pstStrmIdentDes = NULL;

								pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
								if(pstDes !=NULL)
								{
									pstStrmIdentDes = (SIxStmIdentDes_t*)pstDes;

									s_astWebPmtList[ulIdx].ucComponentTag= pstStrmIdentDes->ucCompTag;

									PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDes);
								}
							}
							break;

						case	eSIxDESCTAG_DATA_BROADCAST_ID:
							{
								SIxDataBroadcastIdDes_t 		*pstDataBroadcastIdDes = NULL;

								pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
								if(pstDes !=NULL)
								{
									pstDataBroadcastIdDes = (SIxDataBroadcastIdDes_t *)pstDes;

									s_astWebPmtList[ulIdx].usDataBroadcastTag = pstDataBroadcastIdDes->usDataBroadcastId;
									usDataBroadcastId = pstDataBroadcastIdDes->usDataBroadcastId;
									bDataBroadcastIdFound = TRUE;

									PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDes);
								}
							}
							break;

						case eSIxDESCTAG_CAROUSEL_ID:
							{
								SIxCarouselIdDes_t 	*pstCarouselIdDesDes = NULL;

								pstDes = PAL_SIRAWPOOL_ParseDes (ulActionId, pucDesRaw);
								if(pstDes !=NULL)
								{
									pstCarouselIdDesDes = (SIxCarouselIdDes_t *)pstDes;

									s_astWebPmtList[ulIdx].ulCarouselIDTag = pstCarouselIdDesDes->ulCarouselId;
									ulCarouselId = pstCarouselIdDesDes->ulCarouselId;
									bCarouselIdFound = TRUE;

									PAL_SIRAWPOOL_FreeDes ((SIxDescriptor_t *)pstDes);
								}
							}
							break;

						default:
							break;
					}
				}

				ulIdx += 1;
				ulListCount += 1;
			}
		}

		pstCurrPmt = pstCurrPmt->pstNextTable;
	}

	if(pstPmtTable != NULL)
	{
		SVC_SI_FreeTable(pstPmtTable);
	}

	if (punListCount)
	{
		*punListCount = ulListCount;
	}

	*apList = s_astWebPmtList;

	if(ulListCount)
		return ERR_OK;
	else
		return ERR_FAIL;
#else
	return ERR_OK;
#endif
}

HERROR SVC_SI_GetRctInfo(Handle_t hAction, Handle_t hSvc, DxService_RctInfo_t *pstRctInfo, HBOOL bMhegInterruptibleFlag)
{
	return svc_si_LibGetRctInfo(hAction, hSvc, pstRctInfo, bMhegInterruptibleFlag);
}

#define	_____MHEG_APPL_____
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
HERROR SVC_SI_GetMhegPatRawSection(HUINT32 ulActionId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount)
{
	return svc_si_MhegGetPatRawSection(ulActionId, ulRawArrayMax, ppucRawArray, pulListCount);
}

HERROR SVC_SI_GetMhegPmtRawSection(HUINT32 ulActionId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulListCount)
{
	return svc_si_MhegGetPmtRawSection(ulActionId, ulRawArrayMax, ppucRawArray, pulListCount);
}
#endif

#define _____TARGET_REGION_INFO_____
HERROR SVC_SI_GetTrdConflictedList(Handle_t hAction, HUINT32 ulDepth, HUINT32 *pulconflictcount,HxList_t **ppConflictList)
{
	return psvc_si_GetTrdConflictedList(hAction,ulDepth,pulconflictcount, ppConflictList);
}

HERROR SVC_SI_SetTrdConflictedList(HUINT32 ulDepthIdx, HUINT32 index)
{
	return psvc_si_SetTrdConflictedList(ulDepthIdx, index);
}
HERROR SVC_SI_InitTrdConflicteSvcHandleTable(void)
{
	return psvc_si_InitTrdConflicteSvcHandleTable();
}

HERROR SVC_SI_SetNcdNotifyData(HUINT32 ulASONotifyFlag)
{
	return psvc_si_SetNcdNotifyData(ulASONotifyFlag);
}
HERROR SVC_SI_ClearNcdNotifyData(void)
{
	return psvc_si_ClearNcdNotifyData();
}

#define	_____HOMECHANNEL_SERVICE_____
HERROR SVC_SI_HomeChannelProcessNit(Handle_t hAction, HINT32 nDeliType, HUINT16 usTsUniqId, HUINT32 ulHead)
{
#if	defined(CONFIG_MW_SI_HOME_CHANNEL)
	return svc_si_HomeChannelProcessNit(hAction, nDeliType, usTsUniqId, ulHead);
#else
	return ERR_OK;
#endif
}

HERROR SVC_SI_HomeChannelGetChannelInfo(DbSvc_TripleId_t *pstChannelInfo, HINT32 *pnOrgDeliType, HBOOL *pbValid)
{
#if	defined(CONFIG_MW_SI_HOME_CHANNEL)
	return svc_si_HomeChannelGetChannelInfo(pstChannelInfo, pnOrgDeliType, pbValid);
#else
	return ERR_FAIL;
#endif
}

/* end of file */

