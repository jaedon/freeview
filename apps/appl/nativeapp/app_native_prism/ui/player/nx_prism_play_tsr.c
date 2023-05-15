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
 * @file	  		nx_prism_play_tsr.c
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
static Nx_PlayTsrContext_t *s_pstPlayTsrContext	= NULL;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static Nx_PlayTsrContext_t *nx_play_tsr_GetContext(void);
static ONDK_Result_t	nx_play_tsr_CreateTimer(void);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static Nx_PlayTsrContext_t *nx_play_tsr_GetContext(void)
{
	if (s_pstPlayTsrContext == NULL)
	{
		HxLOG_Print("\n\n[%s.%d] ====>>> Allocated tsr Context \n\n\n",__FUNCTION__,__LINE__);
		s_pstPlayTsrContext = (Nx_PlayTsrContext_t*)NX_APP_Calloc(sizeof(Nx_PlayTsrContext_t));
	}
	return s_pstPlayTsrContext;
}

static ONDK_Result_t	nx_play_tsr_CreateTimer(void)
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
ONDK_Result_t	nx_play_tsr_SetupFunctions(Nx_PlayMgrContext_t *pstMgrContext)
{
	if (pstMgrContext == NULL)
	{
		HxLOG_Error("pstMgrContext is NULL!\n");
		return ONDK_RET_FAIL;
	}

	pstMgrContext->pfnPlay 		= nx_play_tsr_play;
	pstMgrContext->pfnStop		= nx_play_tsr_stop;
	pstMgrContext->pfnSeek		= nx_play_tsr_seek;
	pstMgrContext->pfnCheckProcess = nx_play_tsr_CheckProcess;

	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_tsr_CheckProcess(void)
{
	Nx_PlayTsrContext_t *pstPlayTsrContext = nx_play_tsr_GetContext();
	NxPlayerContentInform_t *pstTsrContentInfo = pstPlayTsrContext->pstTsrContentInform;

	pstTsrContentInfo->ulPlayMaxTime = NX_PLAYER_GetPlayMaxTime(pstTsrContentInfo->ulViewId, &pstTsrContentInfo->stMasterChannel);

	NX_PLAYER_UpdatePlaybackOffsetData(pstTsrContentInfo->ulViewId, &pstTsrContentInfo->stMasterChannel, pstTsrContentInfo);

	HxLOG_Print("\n\n\n[%s.%d] -->> ulPlayMaxTime(%d), playback offset(%u)\n\n\n",__FUNCTION__,__LINE__,
		pstTsrContentInfo->ulPlayMaxTime, pstTsrContentInfo->ulPlaybackOffset);

	// 만약, tsr buffer 가 모두 찬상태라면,, 현재 시간에서 tsr buffer 시간만큼 차감한뒤, start time 을 셋팅.



	return ONDK_RET_OK;
}

ONDK_Result_t	nx_play_tsr_play(void)
{
	HUINT32	ulMainView = 0;
	HUINT32	ulTsrPlayMaxTime = 0;
	HUINT32	ulCurTime = 0;
	NX_Channel_t*	pstMasterChannel = NULL;
	NxPlayerContentInform_t *pstTsrContentInfo = NULL;

	Nx_PlayTsrContext_t *pstPlayTsrContext = nx_play_tsr_GetContext();

	NX_MEDIA_PLAY_GetMainViewId(&ulMainView);

	pstMasterChannel = nx_playmgr_GetMasterChannel();

	ulTsrPlayMaxTime = NX_PLAYER_GetPlayMaxTime(ulMainView, pstMasterChannel);

	if (ulTsrPlayMaxTime < 20)	// buffer 된 tsr 이 20 sec 보다 작으면, 메세지 출력하고 중단.
	{
		HxLOG_Error("No Exist Tsr Buffering..\n");
		//ONDK_GWM_PostMessage(NULL, MSG_PRISM_NO_BUFFER_TSR, 0, 0, 0, 0);
		return ONDK_RET_FAIL;
	}

	if (nx_playmgr_GetCurrentTime(&ulCurTime) != ERR_OK)
	{
		HxLOG_Error("Time is Invalid!\n");
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(pstPlayTsrContext, 0, sizeof(Nx_PlayTsrContext_t));

	// 일단, play position 이 어디든, TSR Player Content 를 만들고 시작..
	pstPlayTsrContext->pstTsrContentInform = NX_PLAYER_MakeContentTsrPlayback(ulMainView);
	pstTsrContentInfo = pstPlayTsrContext->pstTsrContentInform;

	// set contentinfo
	pstTsrContentInfo->ulViewId = ulMainView;
	HxSTD_MemSet(&pstTsrContentInfo->stMasterChannel, 0, sizeof(NX_Channel_t));
	HxSTD_MemCopy(&pstTsrContentInfo->stMasterChannel, pstMasterChannel, sizeof(NX_Channel_t));
	pstTsrContentInfo->ulPlayMaxTime = ulTsrPlayMaxTime;
	pstTsrContentInfo->nPlaySpeed = 1;
	NX_PLAYER_UpdatePlaybackOffsetData(ulMainView, pstMasterChannel, pstTsrContentInfo);

	HxLOG_Print("\n\n[%s.%d] ====>> ulMainView :(%u), contentinfo->ulviewId(%u)\n\n\n",__FUNCTION__,__LINE__,
		ulMainView, pstTsrContentInfo->ulViewId);

	// 같은 service 에 대한 last tsr playback position 이 존재하면 거기에 맞게 이동.
	// 아니면 일단 live 로 시작됨.
	pstTsrContentInfo->ulPlayTime = 0;
	pstTsrContentInfo->ulStartTime = ulCurTime - (ulTsrPlayMaxTime);

	#if defined(CONFIG_DEBUG)
	{
		HxDATETIME_t	datetime;
		HxLOG_Error("\n ulTsrPlayMaxTime (%u)\n", ulTsrPlayMaxTime);
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurTime, &datetime);
		HxLOG_Error("\tsr NowTime (%04d.%02d.%02d)(%02d:%02d:%02d.%03d)\n",
					datetime.stDate.usYear, datetime.stDate.ucMonth, datetime.stDate.ucDay,
					datetime.stTime.ucHour, datetime.stTime.ucMinute, datetime.stTime.ucSecond, datetime.stTime.usMillisecond);

		HLIB_DATETIME_ConvertUnixTimeToDateTime(pstTsrContentInfo->ulStartTime, &datetime);
		HxLOG_Error("\tsr starttime (%04d.%02d.%02d)(%02d:%02d:%02d.%03d)\n",
					datetime.stDate.usYear, datetime.stDate.ucMonth, datetime.stDate.ucDay,
					datetime.stTime.ucHour, datetime.stTime.ucMinute, datetime.stTime.ucSecond, datetime.stTime.usMillisecond);
	}
	#endif

	if (ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_PLAYBAR_Proc) == NULL)
	{
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_PLAYBAR_Proc", APP_DEFAULT_PRIORITY, NX_PRISM_PLAYBAR_Proc, 0, (HINT32)pstTsrContentInfo, 0, 0);
	}

	if (nx_play_tsr_CreateTimer() == ONDK_RET_FAIL)
	{
		return ONDK_RET_FAIL;
	}

//	NX_APP_Free(pstTsrContentInfo);


	#if 0
	HUINT32		startTime = 0;
	HUINT32		maxOffset = 0;

	HUINT32 					 ulSessionId;
	OxMediaPlay_StartInfo_t 	 unStart = {0,};

	NX_Channel_t*	pstChannel = NX_PRISM_LIVE_GetMasterChannel();

	DxService_t		*pstDxSvc = NX_CHANNEL_FindServiceBySvcUid(pstChannel->svcUid);

	unStart.stTsrPb.eType			= eOxMP_MEDIATYPE_TSRPLAYBACK;
	unStart.stTsrPb.ulMasterSvcUid	= pstChannel->svcUid;
	unStart.stTsrPb.ulSuppleSvcUid	= pstChannel->svcUid;
	unStart.stTsrPb.eSuppSvcType	= pstDxSvc->svcType; // ???
	unStart.stTsrPb.ulStartTime 	= 22;

	APK_MEDIA_PLAY_Start (0, &unStart, &ulSessionId);

	#endif

	return ONDK_RET_OK;
}


ONDK_Result_t	nx_play_tsr_stop(void)
{
	ONDK_GWM_KillTimer(NX_PLAY_TIMER_CHECK_PROCESS_ID);

	NX_APP_Free(s_pstPlayTsrContext->pstTsrContentInform);
	NX_APP_Free(s_pstPlayTsrContext);

	s_pstPlayTsrContext = NULL;

	HxLOG_Error("Successed nx_play_tsr_stop!\n");

	return ONDK_RET_OK;
}

ONDK_Result_t 	nx_play_tsr_seek(Nx_PlaySeekType_e eSeekType, HINT32 lSeekTimeMSec)
{
	Nx_PlayTsrContext_t *pstPlayTsrContext = nx_play_tsr_GetContext();
	NxPlayerContentInform_t *pstTsrContentInfo = pstPlayTsrContext->pstTsrContentInform;

	HINT32	nOffset = 0;
	HUINT32	ulPosition = 0;
	HINT32 	nAbsoluteOffset = 0;

	pstTsrContentInfo->ulPlayMaxTime = NX_PLAYER_GetPlayMaxTime(pstTsrContentInfo->ulViewId, &pstTsrContentInfo->stMasterChannel);
	NX_PLAYER_UpdatePlaybackOffsetData(pstTsrContentInfo->ulViewId, &pstTsrContentInfo->stMasterChannel, pstTsrContentInfo);

	if (eSeekType == eNxPlaySeek_LEFT)
	{
		nOffset = -15;
		nAbsoluteOffset = pstTsrContentInfo->ulPlaybackOffset + nOffset;
		HxLOG_Print("[%s.%d] ulReference(1), nAbsoluteOffset(%d) ,ulCurrOffset(%u)\n",__FUNCTION__,__LINE__, nAbsoluteOffset, pstTsrContentInfo->ulPlaybackOffset);

		if (nAbsoluteOffset < 0)								{ nAbsoluteOffset = 0; }
		else if (nAbsoluteOffset > (HINT32)pstTsrContentInfo->ulPlayMaxTime)			{ nAbsoluteOffset = (HINT32)pstTsrContentInfo->ulPlayMaxTime; }

		ulPosition = nAbsoluteOffset + pstTsrContentInfo->ulStartTime;
	}
	else if(eSeekType == eNxPlaySeek_RIGHT)
	{
		nOffset = 15;
		nAbsoluteOffset = pstTsrContentInfo->ulPlaybackOffset + nOffset;
		HxLOG_Print("[%s.%d] ulReference(1), nAbsoluteOffset(%d) ,ulCurrOffset(%u)\n",__FUNCTION__,__LINE__, nAbsoluteOffset, pstTsrContentInfo->ulPlaybackOffset);

		if (nAbsoluteOffset < 0)								{ nAbsoluteOffset = 0; }
		else if (nAbsoluteOffset > (HINT32)pstTsrContentInfo->ulPlayMaxTime)			{ nAbsoluteOffset = (HINT32)pstTsrContentInfo->ulPlayMaxTime; }

		ulPosition = nAbsoluteOffset + pstTsrContentInfo->ulStartTime;
	}

	NX_PLAYER_SetPosition(pstTsrContentInfo, ulPosition);



	HxLOG_Print("\n\n[%s.%d] ulPosition(%u)\n\n",__FUNCTION__,__LINE__, ulPosition);

	return ONDK_RET_OK;

	//return NX_PLAYER_SetPosition(pstTsrContentInfo, ulPosition);
}

