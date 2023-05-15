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
 * @file	  		nx_core_player.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_PLAYER_H__
#define __NX_CORE_PLAYER_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>
#include <nx_core_message.h>
#include <nx_core_channel.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define		NX_PLAYER_SUPPORT_INST_NUM		1

#define		NX_MEDIAPLAY_URL				2048

// PIG 같은것이 생기면, 여러개의 미디어를 지원하기 위해..
#define		NX_PLAYER_INST_01		0
#define		NX_PLAYER_INST_02		1
#define		NX_PLAYER_INST_03		2
/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	ePlayerType_NONE,
	ePlayerType_LIVE,
	ePlayerType_PVRPLAYBACK,
	ePlayerType_TSRPLAYBACK,
	ePlayerType_MEDIAPLAY,
	ePlayerType_SHAHID,
}NxPlayerType_e;

typedef struct
{
	HUINT32					ulViewId;
	HUINT32					ulSessionId;
	NxPlayerType_e			ePlayerType;
	OxMediaPlay_StartInfo_t	stStartInfo;

	NX_Channel_t			stMasterChannel;
	HUINT32					ulPlayTime;				// play 되는 위치의 time
	HUINT32					ulStartTime;			// Tsr 등의 경우에 변경이 된다..
	HUINT32					ulPlaybackOffset;		// starttime 등과 같은 기준에서 현재 play 되는 시간 시간 사이의값. play duration 같은...
	HUINT32					ulPlayMaxTime;			// content 총 길이에 대한 시간.
	HINT32					nPlaySpeed;
}NxPlayerContentInform_t;

typedef struct
{
	NxPlayerContentInform_t	*pstPlayerInstance[NX_PLAYER_SUPPORT_INST_NUM];
}NxPlayerContext_t;


/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	Global Functions		********************/
/*******************************************************************/
HERROR	NX_PLAYER_Init(void);
NxPlayerContentInform_t *NX_PLAYER_MakeContentPvrPlayback(HUINT32 ulViewId, HUINT32 ulContentId, HUINT32 ulStartTime);
NxPlayerContentInform_t *NX_PLAYER_MakeContentMediaPlayback(HUINT32 ulViewId, HUINT32 ulStartTime, HCHAR *szUrl);
NxPlayerContentInform_t *NX_PLAYER_MakeContentTsrPlayback(HUINT32 ulViewId);
HERROR	NX_PLAYER_SetContent(HINT32 nPlayerContentIdx, NxPlayerContentInform_t *pstContentInform);
HERROR	NX_PLAYER_Play(HINT32 nPlayerContentIdx);
HERROR	NX_PLAYER_Stop(HINT32 nPlayerContentIdx);
HERROR	NX_PLAYER_SetPosition(NxPlayerContentInform_t *pstPlayerContent, HUINT32 ulPosition);
HERROR	NX_PLAYER_SetSpeed(NxPlayerContentInform_t *pstPlayerContent, HINT32 lSpeed);

HERROR	NX_PLAYER_UpdatePlaybackOffsetData(HUINT32 ulViewId, NX_Channel_t *pstMasterChannel, NxPlayerContentInform_t *pstPlayerContent);
HUINT32	NX_PLAYER_GetPlayMaxTime(HUINT32 ulViewId, NX_Channel_t *pstMasterChannel);



#endif // __NX_CORE_PLAYER_H__



