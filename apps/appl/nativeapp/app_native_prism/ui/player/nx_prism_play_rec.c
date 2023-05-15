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
 * @file	  		nx_prism_play_rec.c
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
static Nx_PlayRecContext_t *s_pstPlayRecContext	= NULL;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static Nx_PlayRecContext_t *nx_play_rec_GetContext(void);
static ONDK_Result_t	nx_play_rec_CreateTimer(void);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static Nx_PlayRecContext_t *nx_play_rec_GetContext(void)
{
	if (s_pstPlayRecContext == NULL)
	{
		s_pstPlayRecContext = (Nx_PlayRecContext_t*)NX_APP_Calloc(sizeof(Nx_PlayRecContext_t));
	}
	return s_pstPlayRecContext;
}

static ONDK_Result_t	nx_play_rec_CreateTimer(void)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;
	retTimer = ONDK_GWM_SetTimer(NX_PLAY_TIMER_CHECK_PROCESS_ID, NX_PLAY_TIMER_TIMEOUT);

	if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_PLAY_TIMER_CHECK_PROCESS_ID);
	}
	return ONDK_RET_OK;
}


#define _________Play_Functions_______________________________________
/*******************************************************************/
/********************	Play Functions		********************/
/*******************************************************************/
ONDK_Result_t	nx_play_rec_SetupFunctions(Nx_PlayMgrContext_t *pstMgrContext)
{
	if (pstMgrContext == NULL)
	{
		HxLOG_Error("pstMgrContext is NULL!\n");
		return ONDK_RET_FAIL;
	}

	pstMgrContext->pfnPlay 		= nx_play_rec_play;
	pstMgrContext->pfnStop		= nx_play_rec_stop;
	pstMgrContext->pfnSeek		= nx_play_rec_seek;
	pstMgrContext->pfnCheckProcess = nx_play_rec_CheckProcess;

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_rec_CreateRecPlayer(HUINT32 ulContentId)
{
	HUINT32	ulMainView = 0;
	HUINT32	ulStartTime = 0;
	Nx_PlayRecContext_t *pstPlayRecContext = nx_play_rec_GetContext();

	if (pstPlayRecContext == NULL)
	{
		HxLOG_Error("pstPlayRecContext is NULL\n");
		return ONDK_RET_FAIL;
	}

	NX_MEDIA_PLAY_GetMainViewId(&ulMainView);
	pstPlayRecContext->pstRecContentInform =
						NX_PLAYER_MakeContentPvrPlayback(ulMainView, ulContentId, ulStartTime);

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_rec_CheckProcess(void)
{
	Nx_PlayRecContext_t *pstPlayRecContext = nx_play_rec_GetContext();
	NxPlayerContentInform_t *pstRecContentInfo = pstPlayRecContext->pstRecContentInform;

	NX_PORT_MEDIA_PLAY_GetDurationTime(pstRecContentInfo->ulViewId, &pstRecContentInfo->ulPlayMaxTime);

	NX_PORT_MEDIA_PLAY_GetPlayPosition(pstRecContentInfo->ulViewId, &pstRecContentInfo->ulPlayTime);

	HxLOG_Error("RecPlayInfo. playMaxTime(%u), ulPlayTime(%u)\n",
			pstRecContentInfo->ulPlayMaxTime, pstRecContentInfo->ulPlayTime);

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_rec_play(void)
{
	Nx_PlayRecContext_t *pstPlayRecContext = nx_play_rec_GetContext();
	NxPlayerContentInform_t *pstRecContentInfo = pstPlayRecContext->pstRecContentInform;

	if (pstRecContentInfo == NULL)
	{
		HxLOG_Error("pstRecContentInfo is NULL!\n");
		return ONDK_RET_FAIL;
	}

	NX_PLAYER_SetContent(NX_PLAYER_INST_01, pstRecContentInfo);

	if (ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_PLAYBAR_Proc) == NULL)
	{
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_PLAYBAR_Proc", APP_DEFAULT_PRIORITY, NX_PRISM_PLAYBAR_Proc, 0, (HINT32)pstRecContentInfo, 0, 0);
	}

	if (nx_play_rec_CreateTimer() == ONDK_RET_FAIL)
	{
		return ONDK_RET_FAIL;
	}

	NX_PLAYER_Play(NX_PLAYER_INST_01);

	return ONDK_RET_OK;
}


ONDK_Result_t	nx_play_rec_stop(void)
{
	ONDK_GWM_KillTimer(NX_PLAY_TIMER_CHECK_PROCESS_ID);

	NX_PLAYER_Stop(NX_PLAYER_INST_01);

	NX_APP_Free(s_pstPlayRecContext->pstRecContentInform);
	NX_APP_Free(s_pstPlayRecContext);

	s_pstPlayRecContext = NULL;

	HxLOG_Error("Successed nx_play_rec_stop!\n");

	HxLOG_Error("Try Tunning to Last Service...\n");
	ONDK_GWM_PostMessage(NULL, MGS_APP_SYSTEM_START_LASTSVC, 0, 0, 0, 0);

	return ONDK_RET_OK;
}

ONDK_Result_t 	nx_play_rec_seek(Nx_PlaySeekType_e eSeekType, HINT32 lSeekTimeMSec)
{
	HUINT32	ulPosition = 0;
	Nx_PlayRecContext_t *pstPlayRecContext = nx_play_rec_GetContext();
	NxPlayerContentInform_t *pstRecContentInfo = pstPlayRecContext->pstRecContentInform;

	if (eSeekType == eNxPlaySeek_LEFT)
	{
		ulPosition = pstRecContentInfo->ulPlayTime = pstRecContentInfo->ulPlayTime - (HINT32)(lSeekTimeMSec / 1000);
	}
	else if(eSeekType == eNxPlaySeek_RIGHT)
	{
		ulPosition = pstRecContentInfo->ulPlayTime = pstRecContentInfo->ulPlayTime + (HINT32)(lSeekTimeMSec / 1000);
	}
	NX_PLAYER_SetPosition(pstRecContentInfo, ulPosition);

	return ONDK_RET_OK;
}

