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
 * @file	  		nx_prism_shahid_stream.c
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
#include <nx_prism_playmgr.h>
#include "nx_prism_shahid_player.h"

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_PlayShahidContext_t *s_pstPlayShahidContext	= NULL;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
ONDK_Result_t	nx_shahid_stream_Play(void);
ONDK_Result_t	nx_shahid_stream_Stop(void);
ONDK_Result_t	nx_shahid_stream_Seek(Nx_PlaySeekType_e eSeekType, HINT32 lSeekTimeMSec);
ONDK_Result_t	nx_shahid_stream_SetSpeed(HINT32 lSpeed);
ONDK_Result_t	nx_shahid_stream_CheckProcess(void);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static Nx_PlayShahidContext_t *nx_shahid_stream_GetContext(void)
{
	if(s_pstPlayShahidContext == NULL)
	{
		s_pstPlayShahidContext = (Nx_PlayShahidContext_t*)NX_APP_Calloc(sizeof(Nx_PlayShahidContext_t));
	}
	return s_pstPlayShahidContext;
}

static ONDK_Result_t	nx_shahid_stream_CreateTimer(void)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;
	retTimer = ONDK_GWM_SetTimer(NX_PLAY_TIMER_CHECK_PROCESS_ID, NX_PLAY_TIMER_TIMEOUT);

	if(retTimer == GWM_RET_LINK_CONFLICT)
	{
		ONDK_GWM_ResetTimer(NX_PLAY_TIMER_CHECK_PROCESS_ID);
	}
	return ONDK_RET_OK;
}

#define _________Play_Functions_______________________________________
/*******************************************************************/
/********************	Play Functions		********************/
/*******************************************************************/

ONDK_Result_t	nx_shahid_stream_SetupFunctions(Nx_PlayMgrContext_t *pstMgrContext)
{
	if(pstMgrContext == NULL)
	{
		HxLOG_Error("pstMgrContext is NULL!\n");
		return ONDK_RET_FAIL;
	}

	pstMgrContext->pfnPlay 		= nx_shahid_stream_Play;
	pstMgrContext->pfnPlayNext	= NULL;
	pstMgrContext->pfnStop		= nx_shahid_stream_Stop;
	pstMgrContext->pfnSeek		= nx_shahid_stream_Seek;
	pstMgrContext->pfnSetSpeed  = nx_shahid_stream_SetSpeed;
	pstMgrContext->pfnCheckProcess	= nx_shahid_stream_CheckProcess;

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_shahid_stream_MakeContentInform(Nx_PlayMgrContext_t *pstContents, HCHAR *szStreamURL)
{
	HUINT32 ulMainView = 0;
	HUINT32 ulStartTime = 0;
	Nx_PlayShahidContext_t *pstPlayShahidContext = nx_shahid_stream_GetContext();

	if(pstPlayShahidContext == NULL)
	{
		HxLOG_Error("pstPlayShahidContext is NULL\n");
		return ONDK_RET_FAIL;
	}

	NX_MEDIA_PLAY_GetMainViewId(&ulMainView);

	pstPlayShahidContext->pstShahidContentInform = NX_PLAYER_MakeContentMediaPlayback(ulMainView,ulStartTime,szStreamURL);
	pstContents->pstCurPlayerContent = pstPlayShahidContext->pstShahidContentInform;

	return ONDK_RET_OK;
}


ONDK_Result_t	nx_shahid_stream_Play(void)
{
	Nx_PlayShahidContext_t	*pstPlayShahidContext = nx_shahid_stream_GetContext();
	NxPlayerContentInform_t	*pstShahidContentInfo = pstPlayShahidContext->pstShahidContentInform;

	if(pstShahidContentInfo == NULL)
	{
		HxLOG_Error("pstShahidContentInfo is NULL! \n");
		return ONDK_RET_FAIL;
	}
	NX_PLAYER_SetContent(NX_PLAYER_INST_01, pstShahidContentInfo);

	if(ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_SHAHID_Player_Proc)==NULL)
	{
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_SHAHID_Player_Proc", APP_DEFAULT_PRIORITY, NX_PRISM_SHAHID_Player_Proc, 0, (HINT32)pstShahidContentInfo, 0, 0);
	}
	else
	{
		ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYMGR_PLAY_START, 0, (HINT32)pstShahidContentInfo, 0, 0);
	}

	if(nx_shahid_stream_CreateTimer() == ONDK_RET_FAIL)
	{
		return ONDK_RET_FAIL;
	}

	NX_PLAYER_Play(NX_PLAYER_INST_01);
	pstShahidContentInfo->nPlaySpeed = 1;

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_shahid_stream_Stop(void)
{
	ONDK_GWM_KillTimer(NX_PLAY_TIMER_CHECK_PROCESS_ID);
	NX_PLAYER_Stop(NX_PLAYER_INST_01);

	NX_APP_Free(s_pstPlayShahidContext->pstShahidContentInform);
	NX_APP_Free(s_pstPlayShahidContext);

	s_pstPlayShahidContext = NULL;

	HxLOG_Error("success nx_play_stream_Stop \n");

	HxLOG_Error("Turn to last Service..\n");

	ONDK_GWM_PostMessage(NULL, MGS_APP_SYSTEM_START_LASTSVC,0,0,0,0);

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_shahid_stream_Seek(Nx_PlaySeekType_e eSeekType, HINT32 lSeekTimeMSec)
{
	HUINT32 ulPosition = 0;
	Nx_PlayShahidContext_t	*pstPlayShahidContext = nx_shahid_stream_GetContext();
	NxPlayerContentInform_t	*pstShahidContentInfo = pstPlayShahidContext->pstShahidContentInform;

	if(eSeekType == eNxPlaySeek_LEFT)
	{
		pstShahidContentInfo->ulPlayTime-= lSeekTimeMSec;
	}
	else if (eSeekType == eNxPlaySeek_RIGHT)
	{
		pstShahidContentInfo->ulPlayTime += lSeekTimeMSec;
	}

	ulPosition = pstShahidContentInfo->ulPlayTime;

	NX_PLAYER_SetPosition(pstShahidContentInfo, ulPosition);

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_shahid_stream_SetSpeed(HINT32 lSpeed)
{
	Nx_PlayShahidContext_t *pstPlayShahidContext = nx_shahid_stream_GetContext();
	NxPlayerContentInform_t *pstShahidContentInfo = pstPlayShahidContext->pstShahidContentInform;

	if(pstShahidContentInfo == NULL)
	{
		HxLOG_Error("pstShahidContentInfo is NULL\n");
		return ONDK_RET_INITFAIL;
	}

	pstShahidContentInfo->nPlaySpeed = lSpeed;

	NX_PLAYER_SetSpeed(pstShahidContentInfo, (HINT32)(pstShahidContentInfo->nPlaySpeed *100));

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_shahid_stream_CheckProcess(void)
{

	HINT32 lPlaySpeed = 0;
	Nx_PlayShahidContext_t *pstPlayShahidContext = nx_shahid_stream_GetContext();
	NxPlayerContentInform_t *pstShahidContentInfo = pstPlayShahidContext->pstShahidContentInform;

	NX_PORT_MEDIA_PLAY_GetDurationTime(pstShahidContentInfo->ulViewId, &pstShahidContentInfo->ulPlayMaxTime);

	NX_PORT_MEDIA_PLAY_GetPlayPosition(pstShahidContentInfo->ulViewId, &pstShahidContentInfo->ulPlayTime);

	NX_PORT_MEDIA_PLAY_GetPlaySpeed(pstShahidContentInfo->ulViewId, &lPlaySpeed);
	pstShahidContentInfo->nPlaySpeed = (HINT32)(lPlaySpeed / 100);

	return ONDK_RET_OK;

}

