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
 * @file	  		nx_core_media.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_MEDIA_H__
#define __NX_CORE_MEDIA_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_core.h>
#include <nx_port.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
#define	NX_CORE_AUDIO_MAX_NUM								16		/* MAX_AUDIO_ES_NUM 의 ES 개수와 맞춘다. */
#define	NX_CORE_SUBTTL_MAX_NUM								12		/* MAX_SUBTTL_ES_NUM 의 ES 개수와 맞춘다. */


/*******************************************************************/
/********************		Public Struct		********************/
/*******************************************************************/

// DVB-SI 300468 Table 26 : stream_content and component_type
typedef enum
{
	eNxMediaPlay_Subttl_None 				= 0x00,
	eNxMediaPlay_Subttl_EBU_TTX_SUBT 		= 0x01,
	eNxMediaPlay_Subttl_EBU_TTX				= 0x02,
	eNxMediaPlay_Subttl_EBU_TTX_VBI			= 0x03,
	eNxMediaPlay_Subttl_EBU_HH_TTX_SUBT		= 0x05,	//	TTX_SBTL_PAGE_HERARING_IMPAIRED
	eNxMediaPlay_Subttl_DVB_NO_ASPECT		= 0X10,
	eNxMediaPlay_Subttl_DVB_4_3_ASPECT		= 0X11,
	eNxMediaPlay_Subttl_DVB_16_9_ASPECT		= 0X12,
	eNxMediaPlay_Subttl_DVB_2_1_ASPECT		= 0X13,
	eNxMediaPlay_Subttl_DVB_HD				= 0X14,
	eNxMediaPlay_Subttl_DVB_STEREOSCOPIC_HD	= 0X15,
	eNxMediaPlay_Subttl_DVB_HH_NO_ASPECT	= 0X20,
	eNxMediaPlay_Subttl_DVB_HH_4_3_ASPECT	= 0X21,
	eNxMediaPlay_Subttl_DVB_HH_16_9_ASPECT	= 0X22,
	eNxMediaPlay_Subttl_DVB_HH_2_1_ASPECT	= 0X23,
	eNxMediaPlay_Subttl_DVB_HH_HD			= 0X24,
	eNxMediaPlay_Subttl_DVB_HH_STEREOSCOPIC_HD = 0X25,
} NxMediaPlay_Subtitle_e;

typedef	enum
{
	eNxMediaPlay_Video_RepeatAll,
	eNxMediaPlay_Video_RepeatOnce,
	eNxMediaPlay_Video_Repeat1
} NxMediaPlay_Repeat_e;

typedef struct
{
	DxStereoSelect_e	eAudioType;
	HUINT32				ulOrgIndex;
} NX_MediaPlay_AudioOtherComp_t;

typedef struct tagNX_MEDIA_SBTLLIST
{
    HUINT32     					ulSubtitleNum;
    OxMediaPlay_SubtitleComp_t		*pstSubtitlelInfo;//[NX_CORE_SUBTTL_MAX_NUM];
} NX_Media_SBTList_t;

typedef struct tagAUDIO_INFO_LIST
{
	HUINT32							ulAudioNum;
	OxMediaPlay_AudioComp_t			*pstAudioInfo;//[NX_CORE_AUDIO_MAX_NUM];
} NX_Media_AUDIOList_t;


/*******************************************************************/
/********************	Public Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
HERROR	NX_MEDIA_PLAY_Init(void);
HERROR 	NX_MEDIA_PLAY_GetMainViewId(HUINT32 *pulViewId);
HERROR 	NX_MEDIA_PLAY_Stop(HUINT32 ulViewId);
HERROR 	NX_MEDIA_PLAY_Start(HUINT32 ulViewId, OxMediaPlay_StartInfo_t *pstStart, HUINT32 *pulSessionId);
HERROR	NX_MEDIA_PLAY_GetPlayState(HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState);
HERROR	NX_MEDIA_PLAY_CheckPlayChangable(HUINT32 ulViewId, HUINT32 ulSvcuid, HBOOL *pbChangable);
HERROR 	NX_MEDIA_PLAY_GetPlayType(HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType);
HERROR	NX_MEDIA_PLAY_GetNeedRadioBg (HUINT32 ulViewId, HBOOL *pbNeed);
HERROR	NX_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId);
HERROR	NX_MEDIA_PLAY_GetComponentNum (HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HUINT32 *pulCompNum);

HERROR	NX_MEDIA_PLAY_GetComponentIndex(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 *plCompIndex, HINT32 *plCompSubIndex);
HERROR	NX_MEDIA_PLAY_SetComponentIndex(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 lCompIndex);

HERROR 	NX_MEDIA_PLAY_GetComponent(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 lCompIndex, OxMediaPlay_Component_t *pstComponet);
HERROR	NX_MEDIA_PLAY_GetComponentListForSubtitle(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, NX_Media_SBTList_t *pstComponetSubtitle);
HERROR	NX_MEDIA_PLAY_GetComponentListForAudio(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, NX_Media_AUDIOList_t *pstComponetAudio);

HERROR	NX_MEDIA_PLAY_GetVideoRepeatOption(NxMediaPlay_Repeat_e *eRepeatOption);
HERROR	NX_MEDIA_PLAY_GetSubtitleEnable(HUINT32 ulViewId, HBOOL *pbEnable);
HERROR	NX_MEDIA_PLAY_GetSubtitleSyncTime(HUINT32 ulViewId, HUINT32 *pulSyncTime);
HERROR	NX_MEDIA_PLAY_GetSubtitleFontSize(HUINT32 ulViewId, HUINT32 *pulFontSize);
HERROR	NX_MEDIA_PLAY_GetSubtitleTextPosition(HUINT32 ulViewId, HUINT32	*pulTextPosition);
HERROR	NX_MEDIA_PLAY_SetVideoRepeatOption(NxMediaPlay_Repeat_e eRepeatOption);
HERROR	NX_MEDIA_PLAY_SetSubtitleEnable(HUINT32 ulViewId, HBOOL bEnable);
HERROR	NX_MEDIA_PLAY_SetSubtitleSyncTime(HUINT32 ulViewId, HUINT32	ulSyncTime);
HERROR	NX_MEDIA_PLAY_SetSubtitleFontSize(HUINT32 ulViewId,	HUINT32	ulFontSize);
HERROR	NX_MEDIA_PLAY_SetSubtitleTextPosition(HUINT32 ulViewId,	HUINT32	ulTextPosition);

#endif // __NX_CORE_SYSTEM_H__

