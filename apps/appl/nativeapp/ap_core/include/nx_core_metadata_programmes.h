/*
 * (c) 2011-2015 Humax Co., Ltd.
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
 * @file	  		nx_core_metadata_programmes.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_METADATA_PROGRAMMES_H__
#define __NX_CORE_METADATA_PROGRAMMES_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>
#include <clib.h>

#include <nx_common.h>
#include <nx_port.h>
#include <nx_core_channel.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	NX_CORE_METADATA_EPISODE_DATA				"EpisodeNumber"


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	eNX_META_AUDIO_TYPE_UNKNOW = 0x00,
	eNX_META_AUDIO_TYPE_MONO = 0x01,
	eNX_META_AUDIO_TYPE_STEREO = 0x02,
	eNX_META_AUDIO_TYPE_MULTI_CHANNEL = 0x04
} NX_MetaAudioType_e;

typedef struct
{
	HUINT32 	svcuid;
	DxEvent_t	*event;
}NX_Programme_t;

typedef struct
{
	HCHAR	pszEventName[EPG_HUMAX_NAME_LEN];
	HUINT32	startTime;
	HUINT32 duration;
	HINT32	eventId;
	HINT32	svcUid;
	HCHAR	*pszDuration[NX_TEXT_SIZE_32];
	HBOOL	bFocus;
	HBOOL	bNoProgramme;
}NX_ProgrammeItem_t;

/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


#define _____________Programmes_Functions________________
/**************************************************************************************************/
void 	NX_METADATA_PROGRAMMES_DeleteProgrammes(NX_Programme_t *pstProgrammes);
DxEPG_Type_e	NX_METADATA_PROGRAMMES_GetEpgType(NX_Programme_t *pstProgramme);
HERROR 	NX_METADATA_PROGRAMMES_GetName(NX_Programme_t *pstProgramme, char *desBuf);
HERROR 	NX_METADATA_PROGRAMMES_GetDescription(NX_Programme_t *pstProgramme, char *desBuf);
HERROR  NX_METADATA_PROGRAMMES_GetLongDescription(NX_Programme_t *pstProgramme, char *desBuf);
HINT32 	NX_METADATA_PROGRAMMES_GetEpisode(NX_Programme_t *pstProgramme);
HxVector_t *NX_METADATA_PROGRAMMES_GetGenre(NX_Programme_t *pstProgramme);
HUINT8	NX_METADATA_PROGRAMMES_GetGenreNibbleLevel1(NX_Programme_t *pstProgramme);
HUINT8	NX_METADATA_PROGRAMMES_GetGenreNibbleLevel2(NX_Programme_t *pstProgramme);
HBOOL 	NX_METADATA_PROGRAMMES_IsHD(NX_Programme_t *pstProgramme);
HUINT32 NX_METADATA_PROGRAMMES_GetStartTime(NX_Programme_t *pstProgramme);
HUINT32	NX_METADATA_PROGRAMMES_GetDuration(NX_Programme_t *pstProgramme);
HINT32 	NX_METADATA_PROGRAMMES_GetEventID(NX_Programme_t *pstProgramme);
HINT32 	NX_METADATA_PROGRAMMES_GetSvcUID(NX_Programme_t *pstProgramme);
HINT32	NX_METADATA_PROGRAMMES_GetParentalRating(NX_Programme_t *pstProgramme);
HINT32	NX_METADATA_PROGRAMMES_GetSubtitleType(NX_Programme_t *pstProgramme);
HINT32	NX_METADATA_PROGRAMMES_GetSubtitleTypes(NX_Programme_t *pstProgramme);
HINT32 	NX_METADATA_PROGRAMMES_GetAudioType(NX_Programme_t *pstProgramme);

HERROR	NX_METADATA_PROGRAMMES_GetDrm(NX_Programme_t *pstProgramme, HCHAR *desBuf);
HERROR	NX_METADATA_PROGRAMMES_GetActor(NX_Programme_t *pstProgramme, HCHAR *desBuf);
HUINT32 NX_METADATA_PROGRAMMES_GetSeason(NX_Programme_t *pstProgramme);
HUINT32	NX_METADATA_PROGRAMMES_GetSeriesID(NX_Programme_t *pstProgramme);
HERROR	NX_METADATA_PROGRAMMES_GetThumbnailURL(NX_Programme_t *pstProgramme, HCHAR *desBuf);

#endif // __NX_CORE_METADATA_PROGRAMMES_H__



