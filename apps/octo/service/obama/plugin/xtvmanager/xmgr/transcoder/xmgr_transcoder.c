/**
	@file     	xmgr_transcoder.c
	@brief    	transcode recording control application.	\n
			this could handle n recording instances.

	Description:  						\n
	Module: 						 	\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <octo_common.h>

#include <svc_pipe.h>
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>

#include <svc_transcoder.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_transcoder.h>
//#include <xmgr_transcoder.h>
#include "./local_include/_xmgr_transcoder.h"

#define ___LOCAL_DEFINITIONS___

#define ___LOCAL_DATATYPES___

#define ___LOCAL_VARIABLES___
STATIC xMgrTrcdContext_t			 s_astXmgrTrcd_Context[MAX_TRANSCODE_ACTION];

#define ___LOCAL_FUNCTION_PROTOTYPES___

#define ___LOCAL_FUNCTIONS___
STATIC BUS_Callback_t mgr_trcd_getActionProc (HUINT32 ulActionId, MgrTrcd_Start_t *pstStartArg)
{
	if (NULL != pstStartArg)
	{
		switch (pstStartArg->eSrcType)
		{
		case eMgrTrcd_SrcType_Playback:
			return XMGR_TRCD_PB_BASE_GetActionProc (ulActionId);

		default:
			break;
		}
	}

	return NULL;
}

#define ___MEMBER_FUNCTIONS___
xMgrTrcdContext_t *XMGR_TRCD_GetContext (HUINT32 ulActionId)
{
	STATIC HBOOL			 s_bInitialized = FALSE;

	if (TRUE != s_bInitialized)
	{
		HxSTD_memset (s_astXmgrTrcd_Context, 0, sizeof(xMgrTrcdContext_t) * MAX_TRANSCODE_ACTION);
		s_bInitialized = TRUE;
	}

	if ((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST))
	{
		return &(s_astXmgrTrcd_Context[ulActionId - eActionId_VIEW_FIRST]);
	}
	else if ((ulActionId >= eActionId_BG_FIRST) && (ulActionId <= eActionId_BG_LAST))
	{
		return &(s_astXmgrTrcd_Context[ulActionId - eActionId_BG_FIRST + NUM_VIEW_ACTION]);
	}

	return NULL;
}

HERROR XMGR_TRCD_SetStartArguments (xMgrTrcdContext_t *pstContext, Handle_t hAction, MgrTrcd_Start_t *pstStart)
{
	if ((NULL == pstContext) || (NULL == pstStart))
	{
		HxLOG_Error ("arguments NULL!\n");
		return ERR_FAIL;
	}

	pstContext->hAction = hAction;

	// Source:
	switch (pstStart->eSrcType)
	{
	case eMgrTrcd_SrcType_Live:
		pstContext->eSourceType = eSVC_TRANSCODER_SRCTYPE_Live;
		if (DB_SVC_CheckValidServiceHandle (pstStart->unSrc.stLive.hSvcHandle) != ERR_OK)
		{
			HxLOG_Error ("SvcHandle not valid: hSvcHandle(0x%08x)\n", pstStart->unSrc.stLive.hSvcHandle);
			return ERR_FAIL;
		}

		pstContext->unSrc.stLive.hSvcHandle = pstStart->unSrc.stLive.hSvcHandle;
		break;

	case eMgrTrcd_SrcType_Playback:
		pstContext->eSourceType = eSVC_TRANSCODER_SRCTYPE_Pvr;
		if (SVC_META_GetServiceInfo (pstStart->unSrc.stPb.ulContentId, &(pstContext->unSrc.stPb.stPvrSvcInfo)) != ERR_OK)
		{
			HxLOG_Error ("ContentId not valid: ulContentId(0x%08x)\n", pstStart->unSrc.stPb.ulContentId);
			return ERR_FAIL;
		}

		pstContext->unSrc.stPb.ulContentId = pstStart->unSrc.stPb.ulContentId;
		break;

	default:
		HxLOG_Error ("Source Type Invalid:(SrcType=%d)\n", pstStart->eSrcType);
		return ERR_FAIL;
	}

	// Target:
	switch (pstStart->eDstType)
	{
	case eMgrTrcd_DstType_RingBuf:
		pstContext->eOutputType = eSVC_TRANSCODER_OUTPUTTYPE_BUFFER;
		pstContext->unOutput.stRingBuf.hRingBuf = pstStart->unDst.stRingBuf.hRingBuf;
		break;

	case eMgrTrcd_DstType_File:
		pstContext->eOutputType = eSVC_TRANSCODER_OUTPUTTYPE_FILE;
		HxSTD_StrNCpy (pstContext->unOutput.stFile.szFileUrl, pstStart->unDst.stFile.szFileUrl, TRANSCODING_FILE_LEN-1);
		break;

	default:
		HxLOG_Error ("Destination Type Invalid:(DstType=%d)\n", pstStart->eDstType);
		return ERR_FAIL;
	}

	//etc
	pstContext->bPcrTsPacketInsert = pstStart->bPcrTsPacketInsert;

	return ERR_OK;
}

#define ___API_FUNCTIONS___
HERROR xmgr_transcoder_Start (HUINT32 ulActionId, MgrTrcd_Start_t *pstStartArg)
{
	Handle_t				 hAction = SVC_PIPE_GetActionHandle (ulActionId);
	xMgrTrcdContext_t		*pstContext = XMGR_TRCD_GetContext (ulActionId);
	BUS_Callback_t			 pfActProc;
	HERROR					 hResult = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pstContext)
	{
		HxLOG_Error ("XMGR_TRCD_GetContext err: ulActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	if (eTrcdState_Idle != pstContext->eState)
	{
		HxLOG_Error ("Context State is not idle: ulActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	pfActProc = mgr_trcd_getActionProc (ulActionId, pstStartArg);
	if (NULL == pfActProc)
	{
		HxLOG_Error ("mgr_trcd_getActionProc err: ulActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, pfActProc, hAction, (HINT32)pstStartArg, 0, 0);
	pstContext->pfActionProc = pfActProc;

	hResult = ERR_OK;

END_FUNC:
	HxLOG_Trace();
	return hResult;
}

HERROR xmgr_transcoder_Stop (HUINT32 ulActionId)
{
	xMgrTrcdContext_t		*pstContext = XMGR_TRCD_GetContext (ulActionId);
	HERROR					 hResult = ERR_FAIL;

	HxLOG_Trace();

	if (NULL == pstContext)
	{
		HxLOG_Error ("XMGR_TRCD_GetContext err: ulActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	if (eTrcdState_Idle == pstContext->eState)
	{
		HxLOG_Error ("Context State is idle: ulActionId(%d)\n", ulActionId);
		goto END_FUNC;
	}

	if (NULL != pstContext->pfActionProc)
	{
		BUS_MGR_Destroy (pstContext->pfActionProc);
	}

	hResult = ERR_OK;

END_FUNC:
	HxLOG_Trace();
	return hResult;
}

// end of file
