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
 * @file	  		nx_prism_playmgr.c
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
#if defined(CONFIG_OP_SHAHID_APPS)
#include "nx_prism_shahid_player.h"
#endif

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_PlayMgrContext_t s_stNxPlayMgr_Context;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static Nx_PlayMgrContext_t* nx_playmgr_GetContext(void);
static ONDK_Result_t	nx_playmgr_MsgGwmCreate(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_playmgr_MsgGwmDestroy(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_playmgr_MsgGwmKeyDown(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_playmgr_MsgGwmTimer(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_playmgr_RecvMsgDoStart(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_playmgr_RecvMsgDoStop(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_playmgr_RecvMsgDoLeftSeek(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_playmgr_RecvMsgDoRightSeek(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_playmgr_RecvMsgSpeedChange(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_playmgr_RecvMsgStoppedTsrPlayBack(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_playmgr_RecvMsgPlayStateChanged(Nx_PlayMgrContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_playmgr_RecvMsgPlayError(Nx_PlayMgrContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_playmgr_RecvMsgPlayBufferStateChanged(Nx_PlayMgrContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_playmgr_RecvMsgPlaySupportedSpeedChanged(Nx_PlayMgrContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_playmgr_RecvMsgSubtitleChanged(Nx_PlayMgrContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static Nx_PlayMgrContext_t* nx_playmgr_GetContext(void)
{
	return &s_stNxPlayMgr_Context;
}

static ONDK_Result_t	nx_playmgr_MsgGwmCreate(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_playmgr_MsgGwmDestroy(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_playmgr_MsgGwmKeyDown(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
		case KEY_EXIT:
			//nx_play_tsr_stop();
			//ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_OK:
/*
			if(FALSE) // when something needs
			{
				return ONDK_MESSAGE_CONSUMED;
			}
*/
			return ONDK_MESSAGE_PASS;

		case KEY_ARROWRIGHT:
		case KEY_ARROWLEFT:
/*
			if(FALSE) // when something needs
			{
				return ONDK_MESSAGE_CONSUMED;
			}
*/
			return ONDK_MESSAGE_PASS;

		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
/*
			if(FALSE) // when something needs
			{
				return ONDK_MESSAGE_CONSUMED;
			}
*/
			return ONDK_MESSAGE_PASS;


		/*******************
			PVR Keys
		********************/
		case KEY_PLAY:
		case KEY_PAUSE:
			//nx_play_tsr_play();
			return ONDK_MESSAGE_BREAK;

		case KEY_RECORD:
		case KEY_STOP:
/*
			if(FALSE) // when something needs
			{
				return ONDK_MESSAGE_CONSUMED;
			}
*/
			return ONDK_MESSAGE_PASS;


		case KEY_SPECIAL_FASTFORWARD:
			return ONDK_MESSAGE_BREAK;

		case KEY_SPECIAL_REWIND:
			return ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			//ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_playmgr_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}

static ONDK_Result_t	nx_playmgr_MsgGwmTimer(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_PLAY_TIMER_CHECK_PROCESS_ID)
	{
		HxLOG_Debug("NX_PLAY_TIMER_CHECK_PROCESS_ID\n");
		pstContents->pfnCheckProcess();

			pstContents->pstCurPlayerContent->ulPlayTime, pstContents->pstCurPlayerContent->ulPlayMaxTime);
		ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYMGR_PLAYINFO_UPDATED, 0, (HINT32)pstContents->pstCurPlayerContent, 0, 0);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return GWM_RET_OK;
}


static ONDK_Result_t	nx_playmgr_RecvMsgDoStart(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult	= ONDK_RET_FAIL;
	NxPlayerType_e	ePlayerType = (NxPlayerType_e)p1;

	switch (ePlayerType)
	{
#if defined(CONFIG_MW_MM_MEDIA)
		case ePlayerType_MEDIAPLAY:
		if (nx_play_media_SetupFunctions(pstContents) == ONDK_RET_FAIL)
		{
			HxLOG_Error("nx_play_tsr_SetupFunctions is FAILED.!\n");
		}
		else
		{
			HxVector_t	*playList 	= (HxVector_t*)p2;
			HINT32		nStartContentIndex = p3;
			//HCHAR *pszUrl = (HCHAR*)p2;
			eResult = nx_play_media_CreateMediaPlayer(pstContents, playList, nStartContentIndex);
		}
		break;
#endif

#if defined(CONFIG_MW_MM_PVR)
	case ePlayerType_PVRPLAYBACK:
		if (nx_play_rec_SetupFunctions(pstContents) == ONDK_RET_FAIL)
		{
			HxLOG_Error("nx_play_tsr_SetupFunctions is FAILED.!\n");
		}
		else
		{
			HUINT32 ulContentID = (HUINT32)p2;
			eResult = nx_play_rec_CreateRecPlayer(ulContentID);
		}
		break;

	case ePlayerType_TSRPLAYBACK:
		eResult = nx_play_tsr_SetupFunctions(pstContents);
		break;
#endif

#if defined(CONFIG_OP_SHAHID_APPS)
	case ePlayerType_SHAHID:
		if (nx_shahid_stream_SetupFunctions(pstContents) == ONDK_RET_FAIL)
		{
			HxLOG_Error("nx_shahid_stream_SetupFunction failed.!\n");
		}
		else
		{
			HCHAR *pStreamURL = HLIB_STD_StrDup((HCHAR*)p2);
			if(pStreamURL == NULL)
			{
				HxLOG_Error("stream url is empty\n");
				break;
			}
			eResult = nx_shahid_stream_MakeContentInform(pstContents, pStreamURL);
		}
		break;
#endif
	default:
		break;
	}

	if (eResult == ONDK_RET_OK)
		eResult = pstContents->pfnPlay();

	return eResult;
}

static ONDK_Result_t	nx_playmgr_RecvMsgDoStop(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->pfnStop();
	//ONDK_GWM_APP_Destroy(0);
	return GWM_RET_OK;
}

static ONDK_Result_t	nx_playmgr_RecvMsgDoLeftSeek(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->pfnSeek(eNxPlaySeek_LEFT, p1);
	return GWM_RET_OK;
}

static ONDK_Result_t	nx_playmgr_RecvMsgDoRightSeek(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	pstContents->pfnSeek(eNxPlaySeek_RIGHT, p1);
	return GWM_RET_OK;
}

static ONDK_Result_t	nx_playmgr_RecvMsgSpeedChange(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents->pfnSetSpeed)
	{
		pstContents->pfnSetSpeed(p1);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_playmgr_RecvMsgStoppedTsrPlayBack(Nx_PlayMgrContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_PostMessage(NULL, MGS_APP_SYSTEM_START_LASTSVC, 0, 0, 0, 0);
	return GWM_RET_OK;
}

static ONDK_Result_t nx_playmgr_RecvMsgPlayStateChanged(Nx_PlayMgrContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMediaPlay_PlayState_e		eStateChanged = p1;

	switch (eStateChanged)
	{
		case eOxMP_PLAYSTATE_PAUSE:
			break;

		case eOxMP_PLAYSTATE_STOPPED:
			break;

		case eOxMP_PLAYSTATE_EOS:
			if (pstContext->pfnPlayNext)
			{
				pstContext->pfnPlayNext(pstContext->pstCurPlayerContent);
			}
			break;

		case eOxMP_PLAYSTATE_BOS:
			if (pstContext->pfnSetSpeed)
			{
				pstContext->pfnSetSpeed(1);
			}
			ONDK_GWM_PostMessage(NULL, MSG_APP_PLAYMGR_PLAY_START, 0, (HINT32)pstContext->pstCurPlayerContent, 0, 0);
			break;

		case eOxMP_PLAYSTATE_NONE:
		case eOxMP_PLAYSTATE_STOPPING:
		case eOxMP_PLAYSTATE_REQUESTED:
		case eOxMP_PLAYSTATE_CONNECTING:
		case eOxMP_PLAYSTATE_BUFFERING:
		case eOxMP_PLAYSTATE_PLAYING:
		case eOxMP_PLAYSTATE_ENDOFCASE:
		default:
			break;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_playmgr_RecvMsgPlayError(Nx_PlayMgrContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMediaPlay_PlayError_e 	ePlayError = p1;
	OxMediaPlay_ErrorReason_e	eErrorReason = p2;
	NX_APP_Info("ePlayError(%d), eErrorReason(%d)\n", ePlayError, eErrorReason);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_playmgr_RecvMsgPlayBufferStateChanged(Nx_PlayMgrContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMediaPlay_BufferState_e	eBufferState = p1;
	NX_APP_Info("eBufferState(%d)\n", eBufferState);

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_playmgr_RecvMsgPlaySupportedSpeedChanged(Nx_PlayMgrContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_APP_Info("RecvMsgPlaySupportedSpeedChanged \n");

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_playmgr_RecvMsgSubtitleChanged(Nx_PlayMgrContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulViewId = 0;
	HINT32		lSubtitleTrackIndex = -1;
	HBOOL		bShow = FALSE;
	NxPlayerType_e		ePlayerType = (NxPlayerType_e)p1;

	NX_MEDIA_PLAY_GetMainViewId(&ulViewId);

	hError = NX_MEDIA_PLAY_GetComponentIndex(ulViewId, eOxMP_COMPONENT_SUBTITLE, &lSubtitleTrackIndex, NULL);
	if (hError != ERR_OK)
	{
		bShow = FALSE;
	}
	else
	{
		bShow = (lSubtitleTrackIndex == -1) ? FALSE : TRUE;
	}

	if (ePlayerType == ePlayerType_MEDIAPLAY)
	{
		NX_MEDIA_PLAY_SetSubtitleEnable(ulViewId, bShow);
	}
	NX_SYSTEM_ShowSubtitle(bShow);

	return ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
HERROR	nx_playmgr_GetCurrentTime(HUINT32 *pulCurTime)
{
	if (NX_SYSTEM_GetTime(pulCurTime) != ERR_OK)
	{
		HxLOG_Error("time is Invalid!!!\n");
		return ONDK_RET_FAIL;
	}
	return ERR_OK;
}

NX_Channel_t*	nx_playmgr_GetMasterChannel(void)
{
	return NX_PRISM_LIVE_GetMasterChannel();
}

ONDK_Result_t	NX_PRISM_PLAYMGR_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;
	Nx_PlayMgrContext_t	*pstContext = nx_playmgr_GetContext();

	if(pstContext == NULL)
	{
		eResult = ONDK_RET_FAIL;
	}

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_playmgr_MsgGwmCreate(pstContext, p1, p2, p3);
			if (eResult != ONDK_RET_OK)
			{
				HxLOG_Error("\nFailed Create NX_PRISM_PLAYMGR_Proc!!!\n\n");
			}
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_playmgr_MsgGwmDestroy(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_playmgr_MsgGwmKeyDown(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			eResult = nx_playmgr_MsgGwmTimer(pstContext, p1, p2, p3);
			break;

		case MSG_APP_PLAYER_DO_START:
			eResult = nx_playmgr_RecvMsgDoStart(pstContext, p1, p2, p3);
			break;

		case MSG_APP_PLAYER_DO_STOP:
			eResult = nx_playmgr_RecvMsgDoStop(pstContext, p1, p2, p3);
			break;

		case MSG_APP_PLAYER_DO_LEFTSEEK:
			eResult = nx_playmgr_RecvMsgDoLeftSeek(pstContext, p1, p2 ,p3);
			break;

		case MSG_APP_PLAYER_DO_RIGHTSEEK:
			eResult = nx_playmgr_RecvMsgDoRightSeek(pstContext, p1, p2 ,p3);
			break;

		case MSG_APP_PLAYER_DO_SPEED_CHANGE:
			eResult = nx_playmgr_RecvMsgSpeedChange(pstContext, p1, p2 ,p3);
			break;

		// from nx_core_player
		case MSG_CORE_PLAYER_STOPPED_TSR_PLAYBACK:
			eResult = nx_playmgr_RecvMsgStoppedTsrPlayBack(pstContext, p1, p2, p3);
			break;

		case MSG_CORE_PLAYER_PLAY_STATE_CHANGED:
			eResult = nx_playmgr_RecvMsgPlayStateChanged(pstContext, p1, p2, p3);
			break;
		case MSG_CORE_PLAYER_PLAY_ERROR:
			eResult = nx_playmgr_RecvMsgPlayError(pstContext, p1, p2, p3);
			break;

		case MSG_CORE_PLAYER_PLAY_BUFFER_STATE_CHANGED:
			eResult = nx_playmgr_RecvMsgPlayBufferStateChanged(pstContext, p1, p2, p3);
			break;

		case MSG_CORE_PLAYER_PLAY_SUPPORTED_PLAY_SPEED_CHANGED:
			eResult = nx_playmgr_RecvMsgPlaySupportedSpeedChanged(pstContext, p1, p2, p3);
			break;

		case MSG_CORE_PLAYER_COMPONENT_SUBTITLE_CHANGED:
			eResult = nx_playmgr_RecvMsgSubtitleChanged(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return	eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

HERROR		NX_PRISM_PLAYMGR_PlayStart(NxPlayerType_e ePlayerType, void *playData, HUINT32 ulExtraFilter, HBOOL bUsedSendMsg)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (bUsedSendMsg == TRUE)
	{
		eResult = ONDK_GWM_SendMessage(NX_PRISM_PLAYMGR_Proc, MSG_APP_PLAYER_DO_START, 0, (HINT32)ePlayerType, (HUINT32)playData, ulExtraFilter);
	}
	else
	{
		eResult = ONDK_GWM_PostMessage(NX_PRISM_PLAYMGR_Proc, MSG_APP_PLAYER_DO_START, 0, (HINT32)ePlayerType, (HUINT32)playData, ulExtraFilter);
	}

	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error(" Error!!! NX_PRISM_PLAYMGR_PlayStart() - bUsedSendMsg(%d) Fail!!\n", bUsedSendMsg);
	}

	return ERR_OK;
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
