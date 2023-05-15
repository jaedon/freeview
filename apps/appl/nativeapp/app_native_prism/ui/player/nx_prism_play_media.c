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

/******************************************************************************/
/**
 * @file	  		nx_prism_play_media.c
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include "nx_prism_playmgr.h"


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_PlayMediaContext_t *s_pstPlayMediaContext	= NULL;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static Nx_PlayMediaContext_t *nx_play_media_GetContext(void);
static ONDK_Result_t	nx_play_media_CreateTimer(void);
static NxPlayerContentInform_t *nx_play_media_MakeContentInform(DxFileInfo_t *pstFsInfo);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static Nx_PlayMediaContext_t *nx_play_media_GetContext(void)
{
	if (s_pstPlayMediaContext == NULL)
	{
		s_pstPlayMediaContext = (Nx_PlayMediaContext_t*)NX_APP_Calloc(sizeof(Nx_PlayMediaContext_t));
	}
	return s_pstPlayMediaContext;
}

static ONDK_Result_t	nx_play_media_CreateTimer(void)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;
	retTimer = ONDK_GWM_SetTimer(NX_PLAY_TIMER_CHECK_PROCESS_ID, NX_PLAY_TIMER_TIMEOUT);

	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_PLAY_TIMER_CHECK_PROCESS_ID);
	}
	return ONDK_RET_OK;
}

static NxPlayerContentInform_t *nx_play_media_MakeContentInform(DxFileInfo_t *pstFsInfo)
{
	HUINT32	ulMainView = 0;
	HUINT32	ulStartTime = 0;
	HUINT8	szFilePath[DLIB_FILEINFO_NAME_LEN];

	HxSTD_snprintf(szFilePath, DLIB_FILEINFO_NAME_LEN, "file:/%s", (char*)pstFsInfo->szFileName);

	NX_MEDIA_PLAY_GetMainViewId(&ulMainView);

	return NX_PLAYER_MakeContentMediaPlayback(ulMainView, ulStartTime, (HCHAR*)szFilePath);
}

#define _________Play_Functions_______________________________________
/*******************************************************************/
/********************	Play Functions		********************/
/*******************************************************************/
ONDK_Result_t	nx_play_media_SetupFunctions(Nx_PlayMgrContext_t *pstMgrContext)
{
	if (pstMgrContext == NULL)
	{
		HxLOG_Error("pstMgrContext is NULL!\n");
		return ONDK_RET_FAIL;
	}

	pstMgrContext->pfnPlay 		= nx_play_media_play;
	pstMgrContext->pfnPlayNext	= nx_play_media_playNext;
	pstMgrContext->pfnStop		= nx_play_media_stop;
	pstMgrContext->pfnSeek		= nx_play_media_seek;
	pstMgrContext->pfnSetSpeed  = nx_play_media_SetSpeed;
	pstMgrContext->pfnCheckProcess	= nx_play_media_CheckProcess;

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_media_CreateMediaPlayer(Nx_PlayMgrContext_t *pstContents, HxVector_t *contentList, HINT32 nStartContentIndex)
{

	DxFileInfo_t		*pstFsInfo = NULL;
	Nx_PlayMediaContext_t *pstPlayMediaContext = nx_play_media_GetContext();

	if (pstPlayMediaContext == NULL)
	{
		HxLOG_Error("pstPlayMediaContext is NULL\n");
		return ONDK_RET_FAIL;
	}
	if (contentList == NULL || nStartContentIndex < 0)
	{
		HxLOG_Error("pszContentUrl is NULL or Invalid Start Content Index!\n");
		return ONDK_RET_FAIL;
	}

	pstPlayMediaContext->pstMediaContentList = contentList;
	pstPlayMediaContext->ulCurContentIndex = nStartContentIndex;

	pstFsInfo = HLIB_VECTOR_ItemAt(contentList, nStartContentIndex);
	if (pstFsInfo == NULL)
	{
		HxLOG_Error("Not Found (%d) index DxFileInfo_t data.\n");
		return ONDK_RET_FAIL;
	}

	pstPlayMediaContext->pstMediaContentInform = nx_play_media_MakeContentInform(pstFsInfo);
	if (pstPlayMediaContext->pstMediaContentInform == NULL)
	{
		HxLOG_Error("Failed Make Media PlayContentInform!\n");
		return ONDK_RET_FAIL;
	}

	pstContents->pstCurPlayerContent = pstPlayMediaContext->pstMediaContentInform;

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_media_playCurrentItem(NxPlayerContentInform_t *pstCurPlayerContent)
{
	DxFileInfo_t			*pstFsInfo = NULL;
	Nx_PlayMediaContext_t 	*pstPlayMediaContext = nx_play_media_GetContext();

	pstFsInfo = HLIB_VECTOR_ItemAt(pstPlayMediaContext->pstMediaContentList, pstPlayMediaContext->ulCurContentIndex);
	if (pstFsInfo == NULL)
	{
		HxLOG_Error("Not Found (%d) index DxFileInfo_t data.\n");
		return ONDK_RET_FAIL;
	}

	if (pstPlayMediaContext->pstMediaContentInform)
	{
		NX_APP_Free(pstPlayMediaContext->pstMediaContentInform);
		pstPlayMediaContext->pstMediaContentInform = NULL;
	}

	pstPlayMediaContext->pstMediaContentInform = nx_play_media_MakeContentInform(pstFsInfo);
	if (pstPlayMediaContext->pstMediaContentInform == NULL)
	{
		HxLOG_Error("Failed Make Media PlayContentInform!\n");
		return ONDK_RET_FAIL;
	}

	pstCurPlayerContent = pstPlayMediaContext->pstMediaContentInform;

	nx_play_media_play();

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_media_playNext(NxPlayerContentInform_t *pstCurPlayerContent)
{
	HINT32					lContentListLength = 0;
	Nx_PlayMediaContext_t 	*pstPlayMediaContext = nx_play_media_GetContext();
	NxMediaPlay_Repeat_e	eRepeatOption = eNxMediaPlay_Video_RepeatOnce;

	if (pstPlayMediaContext->pstMediaContentInform == NULL || pstPlayMediaContext->pstMediaContentList == NULL)
	{
		HxLOG_Error("pstPlayMediaContext->pstMediaContentInform || pstPlayMediaContext->pstMediaContentList is NULL!\n");
		return ONDK_RET_FAIL;
	}

	NX_MEDIA_PLAY_GetVideoRepeatOption(&eRepeatOption);
	lContentListLength = HLIB_VECTOR_Length(pstPlayMediaContext->pstMediaContentList);

	if (eRepeatOption == eNxMediaPlay_Video_RepeatAll)
	{
		if (pstPlayMediaContext->ulCurContentIndex >= lContentListLength - 1)
		{
			pstPlayMediaContext->ulCurContentIndex = 0;
		}
		else
		{
			pstPlayMediaContext->ulCurContentIndex++;
		}
		nx_play_media_playCurrentItem(pstCurPlayerContent);
	}
	else if (eRepeatOption == eNxMediaPlay_Video_RepeatOnce)
	{
		if (pstPlayMediaContext->ulCurContentIndex >= lContentListLength - 1)
		{
			HxLOG_Debug("Nothing to play!\n");
			ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYMGR_PLAY_STOP, 0, 0, 0, 0);
		}
		else
		{
			pstPlayMediaContext->ulCurContentIndex++;
			nx_play_media_playCurrentItem(pstCurPlayerContent);
		}
	}
	else if (eRepeatOption == eNxMediaPlay_Video_Repeat1)
	{
		nx_play_media_playCurrentItem(pstCurPlayerContent);
	}

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_media_play(void)
{
	Nx_PlayMediaContext_t *pstPlayMediaContext = nx_play_media_GetContext();
	NxPlayerContentInform_t *pstMediaContentInfo = pstPlayMediaContext->pstMediaContentInform;

	if (pstMediaContentInfo == NULL)
	{
		HxLOG_Error("pstMediaContentInfo is NULL!\n");
		return ONDK_RET_FAIL;
	}

	NX_PLAYER_SetContent(NX_PLAYER_INST_01, pstMediaContentInfo);

	if (ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_PLAYBAR_Proc) == NULL)
	{
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_PLAYBAR_Proc", APP_DEFAULT_PRIORITY, NX_PRISM_PLAYBAR_Proc, 0, (HINT32)pstMediaContentInfo, 0, 0);
	}
	else
	{
		ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYMGR_PLAY_START, 0, (HINT32)pstMediaContentInfo, 0, 0);
	}

	if (nx_play_media_CreateTimer() == ONDK_RET_FAIL)
	{
		return ONDK_RET_FAIL;
	}

	NX_PLAYER_Play(NX_PLAYER_INST_01);
	pstMediaContentInfo->nPlaySpeed = 1;

	return ONDK_RET_OK;
}


ONDK_Result_t	nx_play_media_stop(void)
{
	ONDK_GWM_KillTimer(NX_PLAY_TIMER_CHECK_PROCESS_ID);

	NX_PLAYER_Stop(NX_PLAYER_INST_01);

	NX_APP_Free(s_pstPlayMediaContext->pstMediaContentInform);
	NX_APP_Free(s_pstPlayMediaContext);

	s_pstPlayMediaContext = NULL;

	HxLOG_Error("Successed nx_play_media_stop!\n");

	HxLOG_Error("Try Tunning to Last Service...\n");
	ONDK_GWM_PostMessage(NULL, MGS_APP_SYSTEM_START_LASTSVC, 0, 0, 0, 0);

	return ONDK_RET_OK;
}

ONDK_Result_t 	nx_play_media_seek(Nx_PlaySeekType_e eSeekType, HINT32 lSeekTimeMSec)
{
	HUINT32	ulPosition = 0;
	Nx_PlayMediaContext_t *pstPlayMediaContext = nx_play_media_GetContext();
	NxPlayerContentInform_t *pstMediaContentInfo = pstPlayMediaContext->pstMediaContentInform;

	if (eSeekType == eNxPlaySeek_LEFT)
	{
		ulPosition = pstMediaContentInfo->ulPlayTime = pstMediaContentInfo->ulPlayTime - lSeekTimeMSec;
	}
	else if(eSeekType == eNxPlaySeek_RIGHT)
	{
		ulPosition = pstMediaContentInfo->ulPlayTime = pstMediaContentInfo->ulPlayTime + lSeekTimeMSec;
	}
	NX_PLAYER_SetPosition(pstMediaContentInfo, ulPosition);

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_media_SetSpeed(HINT32 lSpeed)
{
	Nx_PlayMediaContext_t *pstPlayMediaContext = nx_play_media_GetContext();
	NxPlayerContentInform_t *pstMediaContentInfo = pstPlayMediaContext->pstMediaContentInform;

	if (pstMediaContentInfo == NULL)
	{
		HxLOG_Error("pstMediaContentInfo is NULL\n");
		return ONDK_RET_INITFAIL;
	}

	pstMediaContentInfo->nPlaySpeed = lSpeed;

	NX_PLAYER_SetSpeed(pstMediaContentInfo, (HINT32)(pstMediaContentInfo->nPlaySpeed * 100));

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_media_CheckProcess(void)
{
	HINT32	lPlaySpeed = 0;
	Nx_PlayMediaContext_t *pstPlayMediaContext = nx_play_media_GetContext();
	NxPlayerContentInform_t *pstMediaContentInfo = pstPlayMediaContext->pstMediaContentInform;

	NX_PORT_MEDIA_PLAY_GetDurationTime(pstMediaContentInfo->ulViewId, &pstMediaContentInfo->ulPlayMaxTime);

	NX_PORT_MEDIA_PLAY_GetPlayPosition(pstMediaContentInfo->ulViewId, &pstMediaContentInfo->ulPlayTime);

	NX_PORT_MEDIA_PLAY_GetPlaySpeed(pstMediaContentInfo->ulViewId, &lPlaySpeed);
	pstMediaContentInfo->nPlaySpeed = (HINT32)(lPlaySpeed / 100);

	HxLOG_Error("MediaPlayInfo. playMaxTime(%u), ulPlayTime(%u)\n",
			pstMediaContentInfo->ulPlayMaxTime, pstMediaContentInfo->ulPlayTime);

	return ONDK_RET_OK;
}

