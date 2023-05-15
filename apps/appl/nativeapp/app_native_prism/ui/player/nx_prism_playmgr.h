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
 * @file	  		nx_prism_playmgr.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/
#ifndef __NX_PRISM_PLAYMGR_H__
#define __NX_PRISM_PLAYMGR_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define		NX_PLAY_TIMER_CHECK_PROCESS_ID	0xb200
#define		NX_PLAY_TIMER_TSR_ID		0xb201
#define		NX_PLAY_TIMER_CHASE_ID		0xb202
#define		NX_PLAY_TIMER_MEDIA			0xb203

#define		NX_PLAY_TIMER_TIMEOUT		1000

/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eNxPlaySeek_LEFT,
	eNxPlaySeek_RIGHT,
} Nx_PlaySeekType_e;

typedef enum
{
	eNxPlaySpeed_REWIND,
	eNxPlaySpeed_FORWARD,
	eNxPlaySpeed_PAUSE,
	eNxPlaySpeed_PLAY
} Nx_PlaySpeedType_e;

typedef struct _NxPlayMedisContext
{
	HCHAR		szContentUrl[NX_MEDIAPLAY_URL];
	HUINT32		ulCurContentIndex;
	HxVector_t 	*pstMediaContentList;
	NxPlayerContentInform_t	*pstMediaContentInform;
} Nx_PlayMediaContext_t;

typedef struct _NxPlayRecContext
{
	HUINT32	ulContentID;
	NxPlayerContentInform_t	*pstRecContentInform;
} Nx_PlayRecContext_t;

typedef struct _NxPlayTsrContext
{
	NxPlayerContentInform_t	*pstTsrContentInform;
} Nx_PlayTsrContext_t;

typedef ONDK_Result_t	(* pfnNxPlayPlay) (void);
typedef	ONDK_Result_t	(* pfnNxPlayPlayNext) (NxPlayerContentInform_t	*pstCurPlayerContent);
typedef ONDK_Result_t	(* pfnNxPlayStop) (void);
typedef ONDK_Result_t	(* pfnNxPlaySeek) (Nx_PlaySeekType_e eSeekType, HINT32 lSeekTimeMSec);
typedef	ONDK_Result_t	(* pfnNxPlaySetSpeed) (HINT32 lSpeed);
typedef ONDK_Result_t	(* pfnNxPlayCheckProcess) (void);

typedef struct NxPlayMgrContext
{
	pfnNxPlayPlay		pfnPlay;
	pfnNxPlayPlayNext	pfnPlayNext;
	pfnNxPlayStop		pfnStop;
	pfnNxPlaySeek		pfnSeek;
	pfnNxPlaySetSpeed	pfnSetSpeed;
	pfnNxPlayCheckProcess	pfnCheckProcess;
	NxPlayerContentInform_t 	*pstCurPlayerContent;
} Nx_PlayMgrContext_t;

/*******************************************************************/
/********************	Private Functions		********************/
/*******************************************************************/
/*
MEDIA PLAYBACK
*/
ONDK_Result_t	nx_play_media_SetupFunctions(Nx_PlayMgrContext_t *pstMgrContext);
ONDK_Result_t	nx_play_media_CreateMediaPlayer(Nx_PlayMgrContext_t *pstContents, HxVector_t *contentList, HINT32 nStartContentIndex);
ONDK_Result_t	nx_play_media_playCurrentItem(NxPlayerContentInform_t *pstCurPlayerContent);
ONDK_Result_t	nx_play_media_playNext(NxPlayerContentInform_t *pstCurPlayerContent);
ONDK_Result_t	nx_play_media_play(void);
ONDK_Result_t	nx_play_media_stop(void);
ONDK_Result_t 	nx_play_media_seek(Nx_PlaySeekType_e eSeekType, HINT32 lSeekTimeMSec);
ONDK_Result_t	nx_play_media_SetSpeed(HINT32 lSpeed);
ONDK_Result_t	nx_play_media_CheckProcess(void);

/*
Recording PLAYBACK
*/
ONDK_Result_t	nx_play_rec_SetupFunctions(Nx_PlayMgrContext_t *pstMgrContext);
ONDK_Result_t	nx_play_rec_CreateRecPlayer(HUINT32 ulContentId);
ONDK_Result_t	nx_play_rec_play(void);
ONDK_Result_t	nx_play_rec_stop(void);
ONDK_Result_t 	nx_play_rec_seek(Nx_PlaySeekType_e eSeekType, HINT32 lSeekTimeMSec);
ONDK_Result_t	nx_play_rec_CheckProcess(void);

/*
TSR PLAYBACK
*/
ONDK_Result_t	nx_play_tsr_SetupFunctions(Nx_PlayMgrContext_t *pstMgrContext);
ONDK_Result_t	nx_play_tsr_play(void);
ONDK_Result_t	nx_play_tsr_stop(void);
ONDK_Result_t	nx_play_tsr_seek(Nx_PlaySeekType_e eSeekType, HINT32 lSeekTimeMSec);
ONDK_Result_t	nx_play_tsr_CheckProcess(void);

/*
PlayMgr
*/
HERROR			nx_playmgr_GetCurrentTime(HUINT32 *pulCurTime);
NX_Channel_t*	nx_playmgr_GetMasterChannel(void);

/*******************************************************************/
/********************	Global Functions		********************/
/*******************************************************************/

#endif	//__NX_PRISM_PLAYMGR_H__