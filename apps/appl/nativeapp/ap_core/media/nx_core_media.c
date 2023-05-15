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
 * @file	  		nx_core_media.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_core.h>
#include <nx_port.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static HERROR	nx_media_play_GetComponentListForSubtitle(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, NX_Media_SBTList_t *pstComponetSubtitle);
static HERROR	nx_media_play_GetComponentListForAudio(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, NX_Media_AUDIOList_t *pstComponetAudio);

#if defined(CONFIG_DEBUG)
static HERROR	nx_media_play_PrintComponentList(OxMediaPlay_Component_e eCompType, OxMediaPlay_Component_t *pstComponet);
#endif

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static HERROR	nx_media_play_GetComponentListForSubtitle(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, NX_Media_SBTList_t *pstComponetSubtitle)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulMaxNum = 0, i = 0;
	OxMediaPlay_Component_t 	*pstMediaComponet = NULL;
	OxMediaPlay_SubtitleComp_t	*pstCompSubttl = NULL;

	pstMediaComponet = (OxMediaPlay_Component_t *)NX_APP_Calloc(sizeof(OxMediaPlay_Component_t));

	hError = NX_MEDIA_PLAY_GetComponentNum(ulViewId, eCompType, &ulMaxNum);
	if (hError != ERR_OK)
	{
		if (pstMediaComponet)
		{
			NX_APP_Free(pstMediaComponet);
			pstMediaComponet = NULL;
		}
		NX_APP_Error("ERROR!!! Please Check! \n");
		return	ERR_FAIL;
	}

	NX_APP_Info("View ID : [0x%x], eOxMP_COMPONENT_SUBTITLE Count : [%d]\n", ulViewId, ulMaxNum);
	if (ulMaxNum < 1)
	{
		if (pstMediaComponet)
		{
			NX_APP_Free(pstMediaComponet);
			pstMediaComponet = NULL;
		}
		return	ERR_OK;
	}

	if (ulMaxNum > NX_CORE_SUBTTL_MAX_NUM)
	{
		ulMaxNum = NX_CORE_SUBTTL_MAX_NUM;
	}

	pstComponetSubtitle->ulSubtitleNum = ulMaxNum;
	for (i = 0; i < ulMaxNum; i++)
	{
		HxSTD_memset(pstMediaComponet, 0x00, sizeof(OxMediaPlay_Component_t));
		hError = NX_MEDIA_PLAY_GetComponent(ulViewId, eCompType, (HINT32)i, pstMediaComponet);
		if (hError == ERR_OK)
		{
			pstCompSubttl = &pstComponetSubtitle->pstSubtitlelInfo[i];
			HxSTD_MemCopy(pstCompSubttl, &pstMediaComponet->stSubtitle, sizeof(OxMediaPlay_SubtitleComp_t));

#if defined(CONFIG_DEBUG)
			NX_APP_Info("=== OxMP_COMPONENT_SUBTITLE [%d] =\n", ulMaxNum);
			NX_APP_Info("==== Index : [%d] ================\n", i);
			nx_media_play_PrintComponentList(eCompType, pstMediaComponet);
			NX_APP_Info("==================================\n");
#endif
		}
	}

	if (pstMediaComponet)
	{
		NX_APP_Free(pstMediaComponet);
		pstMediaComponet = NULL;
	}

	return	ERR_OK;
}


static HERROR	nx_media_play_GetComponentListForAudio(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, NX_Media_AUDIOList_t *pstComponetAudio)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulMaxNum = 0, i = 0;
	OxMediaPlay_Component_t 	*pstMediaComponet = NULL;
	OxMediaPlay_AudioComp_t		*pstCompAudio = NULL;

	pstMediaComponet = (OxMediaPlay_Component_t*)NX_APP_Calloc(sizeof(OxMediaPlay_Component_t));

	hError = NX_MEDIA_PLAY_GetComponentNum(ulViewId, eCompType, &ulMaxNum);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! Please Check! \n");
		if (pstMediaComponet)
		{
			NX_APP_Free(pstMediaComponet);
			pstMediaComponet = NULL;
		}
		return	ERR_FAIL;
	}

	NX_APP_Info("View ID : [0x%x], eOxMP_COMPONENT_AUDIO Count : [%d]\n", ulViewId, ulMaxNum);
	if (ulMaxNum < 1)
	{
		if (pstMediaComponet)
		{
			NX_APP_Free(pstMediaComponet);
			pstMediaComponet = NULL;
		}
		return	ERR_OK;
	}
	if (ulMaxNum > NX_CORE_AUDIO_MAX_NUM)
	{
		ulMaxNum = NX_CORE_AUDIO_MAX_NUM;
	}

	pstComponetAudio->ulAudioNum = ulMaxNum;
	for (i = 0; i < ulMaxNum; i++)
	{
		HxSTD_memset(pstMediaComponet, 0x00, sizeof(OxMediaPlay_Component_t));
		hError = NX_MEDIA_PLAY_GetComponent(ulViewId, eCompType, (HINT32)i, pstMediaComponet);
		if (hError == ERR_OK)
		{
			pstCompAudio = &pstComponetAudio->pstAudioInfo[i];
			HxSTD_MemCopy(pstCompAudio, &pstMediaComponet->stAudio, sizeof(OxMediaPlay_AudioComp_t));

#if defined(CONFIG_DEBUG)
			NX_APP_Info("=== eOxMP_COMPONENT_AUDIO [%d] ===\n", ulMaxNum);
			NX_APP_Info("==== Index : [%d] ================\n", i);
			nx_media_play_PrintComponentList(eCompType, pstMediaComponet);
			NX_APP_Info("==================================\n");
#endif
		}
	}

	if (pstMediaComponet)
	{
		NX_APP_Free(pstMediaComponet);
		pstMediaComponet = NULL;
	}

	return	ERR_OK;
}


#if defined(CONFIG_DEBUG)
static HERROR	nx_media_play_PrintComponentList(OxMediaPlay_Component_e eCompType, OxMediaPlay_Component_t *pstComponet)
{
	HINT32		i = 0;

	switch (eCompType)
	{
		case eOxMP_COMPONENT_SUBTITLE:
			NX_APP_Info("==== eCompType	: [%d]\n", pstComponet->stSubtitle.eCompType);
			NX_APP_Info("==== PID : [0x%x]\n", pstComponet->stSubtitle.nPid);
			NX_APP_Info("==== Component Tag : [0x%x]\n", pstComponet->stSubtitle.nComponentTag);
			if (pstComponet->stSubtitle.eSubtitleType == eOxMP_SUBTITLETYPE_Dvb)
			{
				NX_APP_Info("==== eSubtitleType : [eOxMP_SUBTITLETYPE_Dvb]\n");
				NX_APP_Info("==== nTypeValue : [0x%x]\n", pstComponet->stSubtitle.utArg.stDvbInfo.nTypeValue);
				NX_APP_Info("==== nComponentTag : [0x%x]\n", pstComponet->stSubtitle.utArg.stDvbInfo.nComponentTag);
				NX_APP_Info("==== nCompositionPageId : [0x%x]\n", pstComponet->stSubtitle.utArg.stDvbInfo.nCompositionPageId);
				NX_APP_Info("==== nAncillaryPageId	: [0x%x]\n", pstComponet->stSubtitle.utArg.stDvbInfo.nAncillaryPageId);
				NX_APP_Info("==== szLangCode : [%s]\n", pstComponet->stSubtitle.utArg.stDvbInfo.szLangCode);
			}
			else if (pstComponet->stSubtitle.eSubtitleType == eOxMP_SUBTITLETYPE_Ebu)
			{
				NX_APP_Info("==== eSubtitleType : [eOxMP_SUBTITLETYPE_Ebu]\n");
				NX_APP_Info("==== nTypeValue : [0x%x]\n", pstComponet->stSubtitle.utArg.stEbuInfo.nTypeValue);
				NX_APP_Info("==== nComponentTag : [0x%x]\n", pstComponet->stSubtitle.utArg.stEbuInfo.nComponentTag);
				NX_APP_Info("==== nMagazineNum	: [0x%x]\n", pstComponet->stSubtitle.utArg.stEbuInfo.nMagazineNum);
				NX_APP_Info("==== nPageNum	: [0x%x]\n", pstComponet->stSubtitle.utArg.stEbuInfo.nPageNum);
				NX_APP_Info("==== szLangCode : [%s]\n", pstComponet->stSubtitle.utArg.stEbuInfo.szLangCode);
			}
			else if (pstComponet->stSubtitle.eSubtitleType == eOxMP_SUBTITLETYPE_Media)
			{
				NX_APP_Info("==== eSubtitleType : [eOxMP_SUBTITLETYPE_Media]\n");
				NX_APP_Info("==== szLangCode : [%s]\n", pstComponet->stSubtitle.utArg.stMediaInfo.szLangCode);
			}
			else
			{
				NX_APP_Error("==== ERROR!!! Unknow subtitle type!\n");
			}
			break;
		case eOxMP_COMPONENT_AUDIO:
			NX_APP_Info("==== eCompType	: [%d]\n", pstComponet->stAudio.eCompType);
			NX_APP_Info("==== PID : [0x%x]\n", pstComponet->stAudio.nPid);
			NX_APP_Info("==== Component Tag : [0x%x]\n", pstComponet->stAudio.nComponentTag);
			NX_APP_Info("==== eCodec : [%d]\n", pstComponet->stAudio.eCodec);
			NX_APP_Info("==== bIsDolby : [%d]\n", pstComponet->stAudio.bIsDolby);
			NX_APP_Info("==== eMode : [%d]\n", pstComponet->stAudio.eMode);
			NX_APP_Info("==== eSupAudType : [%d]\n", pstComponet->stAudio.eSupAudType);
			NX_APP_Info("==== ucMixType : [%d]\n", pstComponet->stAudio.ucMixType);
			NX_APP_Info("==== nLangNum : [%d]\n", pstComponet->stAudio.nLangNum);
			for (i = 0; i < pstComponet->stAudio.nLangNum; i++)
			{
				NX_APP_Info("==== eAudType : [%d]\n", pstComponet->stAudio.astLangInfo[i].eAudType);
				NX_APP_Info("==== szLangCode : [%s]\n", pstComponet->stAudio.astLangInfo[i].szLangCode);
			}
			break;
		default:
			break;
	}

	return	ERR_OK;
}
#endif


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
HERROR	NX_MEDIA_PLAY_Init(void)
{
#if !defined(CONFIG_PROD_HMS1000S)
	HINT32	lValue = 0;
#endif

	// set TSR Enable
#if defined(CONFIG_PROD_HMS1000S)
	NX_PORT_MEDIA_PLAY_SetTSREnable(TRUE);
#else
	if (NX_PORT_DB_GetInt(USERDB_RECORDING_TSR_ONOFF, &lValue) == ERR_OK)
	{
		if (lValue)
		{
			NX_PORT_MEDIA_PLAY_SetTSREnable(TRUE);
		}
	}
#endif
	return ERR_OK;
}

HERROR	NX_MEDIA_PLAY_GetMainViewId (HUINT32 *pulViewId)
{
	return NX_PORT_MEDIA_PLAY_GetMainViewId(pulViewId);
}

HERROR	NX_MEDIA_PLAY_GetPlayType(HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType)
{
	return NX_PORT_MEDIA_PLAY_GetPlayType(ulViewId, peType);
}

HERROR	NX_MEDIA_PLAY_GetNeedRadioBg (HUINT32 ulViewId, HBOOL *pbNeed)
{
	return NX_PORT_MEDIA_PLAY_GetNeedRadioBg(ulViewId, pbNeed);
}

HERROR	NX_MEDIA_PLAY_Stop (HUINT32 ulViewId)
{
	return NX_PORT_MEDIA_PLAY_Stop(ulViewId);
}

HERROR	NX_MEDIA_PLAY_Start (HUINT32 ulViewId, OxMediaPlay_StartInfo_t *pstStart, HUINT32 *pulSessionId)
{
	return NX_PORT_MEDIA_PLAY_Start (ulViewId, pstStart, pulSessionId);
}

HERROR	NX_MEDIA_PLAY_GetPlayState(HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState)
{
	return NX_PORT_MEDIA_PLAY_GetPlayState (ulViewId, peState);
}

HERROR	NX_MEDIA_PLAY_CheckPlayChangable(HUINT32 ulViewId, HUINT32 ulSvcuid, HBOOL *pbChangable)
{
	return NX_PORT_MEDIA_PLAY_CheckPlayChangable(ulViewId, ulSvcuid, pbChangable);
}

HERROR	NX_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId)
{
	return NX_PORT_MEDIA_PLAY_GetRequestId(ulViewId, eType, pulRequestId);
}

HERROR	NX_MEDIA_PLAY_GetComponentNum(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HUINT32 *pulCompNum)
{
	return	NX_PORT_MEDIA_PLAY_GetComponentNum (ulViewId, eCompType, pulCompNum);
}

HERROR	NX_MEDIA_PLAY_GetComponentIndex(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 *plCompIndex, HINT32 *plCompSubIndex)
{
	return	NX_PORT_MEDIA_PLAY_GetComponentIndex(ulViewId, eCompType, plCompIndex, plCompSubIndex);
}

HERROR	NX_MEDIA_PLAY_SetComponentIndex(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 lCompIndex)
{
	return	NX_PORT_MEDIA_PLAY_SetComponentIndex(ulViewId, eCompType, lCompIndex);
}

HERROR	NX_MEDIA_PLAY_GetComponent(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, HINT32 lCompIndex, OxMediaPlay_Component_t *pstComponet)
{
	return NX_PORT_MEDIA_PLAY_GetComponent(ulViewId, eCompType, lCompIndex, pstComponet);
}

HERROR	NX_MEDIA_PLAY_GetComponentListForSubtitle(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, NX_Media_SBTList_t *pstComponetSubtitle)
{
	if (eCompType == eOxMP_COMPONENT_SUBTITLE)
	{
		return	nx_media_play_GetComponentListForSubtitle(ulViewId, eCompType, pstComponetSubtitle);
	}
	else
	{
		return	ERR_FAIL;
	}
}

HERROR	NX_MEDIA_PLAY_GetComponentListForAudio(HUINT32 ulViewId, OxMediaPlay_Component_e eCompType, NX_Media_AUDIOList_t *pstComponetAudio)
{
	if (eCompType == eOxMP_COMPONENT_AUDIO)
	{
		return	nx_media_play_GetComponentListForAudio(ulViewId, eCompType, pstComponetAudio);
	}
	else
	{
		return	ERR_FAIL;
	}
}

HERROR	NX_MEDIA_PLAY_GetVideoRepeatOption(NxMediaPlay_Repeat_e *eRepeatOption)
{
	return NX_PORT_DB_GetInt(USERDB_VIDEO_REPEAT_OPTION, (HINT32*)eRepeatOption);
}

HERROR	NX_MEDIA_PLAY_GetSubtitleEnable(HUINT32 ulViewId, HBOOL *pbEnable)
{
	return NX_PORT_MEDIA_PLAY_GetSubtitleEnable(ulViewId, pbEnable);
}

HERROR	NX_MEDIA_PLAY_GetSubtitleSyncTime(HUINT32 ulViewId, HUINT32 *pulSyncTime)
{
	return NX_PORT_MEDIA_PLAY_GetSubtitleSyncTime(ulViewId, pulSyncTime);
}

HERROR	NX_MEDIA_PLAY_GetSubtitleFontSize(HUINT32 ulViewId, HUINT32 *pulFontSize)
{
	return NX_PORT_MEDIA_PLAY_GetSubtitleFontSize(ulViewId, pulFontSize);
}

HERROR	NX_MEDIA_PLAY_GetSubtitleTextPosition(HUINT32 ulViewId, HUINT32	*pulTextPosition)
{
	return NX_PORT_MEDIA_PLAY_GetSubtitleTextPosition(ulViewId, pulTextPosition);
}

HERROR	NX_MEDIA_PLAY_SetVideoRepeatOption(NxMediaPlay_Repeat_e eRepeatOption)
{
	return NX_PORT_DB_SetInt(USERDB_VIDEO_REPEAT_OPTION, (HINT32)eRepeatOption);
}

HERROR	NX_MEDIA_PLAY_SetSubtitleEnable(HUINT32 ulViewId, HBOOL bEnable)
{
	return NX_PORT_MEDIA_PLAY_SetSubtitleEnable(ulViewId, bEnable);
}

HERROR	NX_MEDIA_PLAY_SetSubtitleSyncTime(HUINT32 ulViewId, HUINT32	ulSyncTime)
{
	return NX_PORT_MEDIA_PLAY_SetSubtitleSyncTime(ulViewId, ulSyncTime);
}

HERROR	NX_MEDIA_PLAY_SetSubtitleFontSize(HUINT32 ulViewId,	HUINT32	ulFontSize)
{
	return NX_PORT_MEDIA_PLAY_SetSubtitleFontSize(ulViewId, ulFontSize);
}

HERROR	NX_MEDIA_PLAY_SetSubtitleTextPosition(HUINT32 ulViewId,	HUINT32	ulTextPosition)
{
	return NX_PORT_MEDIA_PLAY_SetSubtitleTextPosition(ulViewId, ulTextPosition);
}

#define ______________RELATED_PLAYBACK__________________


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

