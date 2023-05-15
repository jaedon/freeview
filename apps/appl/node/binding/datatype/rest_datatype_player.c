/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_player.c
	@brief

	Description:  									\n
	Module: node/binding/datatype_convert		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <apk.h>
#include <rest_datatype.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
// Converting Types:
STATIC RestEnumStringTable_t	 s_stRestDataType_SrcType =
{
	(HCHAR*)DxNAMEOF(OxMediaPlay_MediaType_e),
		
	{
		{	"live",			eOxMP_MEDIATYPE_LIVE		},
		{	"pvrpb",		eOxMP_MEDIATYPE_PVRPLAYBACK	},
		{	"tsrpb",		eOxMP_MEDIATYPE_TSRPLAYBACK	},
		{	"media", 		eOxMP_MEDIATYPE_MEDIAPLAY	},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_PlayState =
{
	(HCHAR*)DxNAMEOF(OxMediaPlay_PlayState_e),
		
	{
		{	"none",			eOxMP_PLAYSTATE_NONE		},
		{	"paused",		eOxMP_PLAYSTATE_PAUSE		},
		{	"stopping",		eOxMP_PLAYSTATE_STOPPING	},
		{	"stopped", 		eOxMP_PLAYSTATE_STOPPED		},
		{	"requested",	eOxMP_PLAYSTATE_REQUESTED	},
		{	"starting",		eOxMP_PLAYSTATE_CONNECTING	},
		{	"starting",		eOxMP_PLAYSTATE_BUFFERING	},
		{	"started",		eOxMP_PLAYSTATE_PLAYING		},
		{	"bos",			eOxMP_PLAYSTATE_BOS			},
		{	"eos",			eOxMP_PLAYSTATE_EOS			},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_ComponentType = 
{
	(HCHAR *)DxNAMEOF(OxMediaPlay_Component_e),

	{
		{	"video",		eOxMP_COMPONENT_VIDEO			},
		{	"audio",		eOxMP_COMPONENT_AUDIO			},
		{	"subtitle",		eOxMP_COMPONENT_SUBTITLE		},
		{	"teletext", 	eOxMP_COMPONENT_TELETEXT		},
		{	"ccaption",		eOxMP_COMPONENT_CLOSEDCAPTION	},
		{	"data",			eOxMP_COMPONENT_DATA			},
		{	NULL,			0								}
		
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_AudioMode = 
{
	(HCHAR *)DxNAMEOF(OxMediaPlay_AudioMode_e),

	{
		{	"singlemono",	eOxMP_AUDMODE_SINGLEMONO		},
		{	"dualmono",		eOxMP_AUDMODE_DUALMONO			},
		{	"stereo",		eOxMP_AUDMODE_STEREO			},
		{	"multilingual", eOxMP_AUDMODE_MULTILINGUAL		},
		{	"surround",		eOxMP_AUDMODE_SURROUND			},
		{	"dolby",		eOxMP_AUDMODE_DOLBY				},
		{	NULL,			0								}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_AudioType = 
{
	(HCHAR *)DxNAMEOF(OxMediaPlay_AUDIOType_e),

	{
		{	"normal",			eOxMP_AUDIOTYPE_UNDEFINED			},
		{	"clean_effect",		eOxMP_AUDIOTYPE_CLEAN_EFFECTS		},
		{	"hearing_impared",	eOxMP_AUDIOTYPE_HEARING_IMPAIRED	},
		{	"visual_impared",	eOxMP_AUDIOTYPE_VISUAL_IMPAIRED		},
		{	"reserved",			eOxMP_AUDIOTYPE_RESERVED			},
		{	NULL,				0									}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SuppleAudioType = 
{
	(HCHAR *)DxNAMEOF(OxMediaPlay_AUDIOType_SUPP_e),
		
	{	
		{	"main",									eOxMP_AUDIOTYPE_SUPP_MAIN							},
		{	"ad_visual_impared", 					eOxMP_AUDIOTYPE_SUPP_AD_VISUAL_IMPARED				},
		{	"supp_clean_audio_hearing_impared",		eOxMP_AUDIOTYPE_SUPP_CLEAN_AUDIO_HEARING_IMPAIRED	},
		{	"supp_spoken_subtitle_visual_impared",	eOxMP_AUDIOTYPE_SUPP_SPOKEN_SUBITITE_VISUAL_IMPARED },
		{	"supp_main", 							eOxMP_AUDIOTYPE_SUPPLEMENTARY_MAIN					},
		{	NULL,				0													}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SubtitleType = 
{
	(HCHAR *)DxNAMEOF(OxMediaPlay_SubtitleType_e),
		
	{
		{	"dvb",			eOxMP_SUBTITLETYPE_Dvb		},
		{	"ebu",			eOxMP_SUBTITLETYPE_Ebu		},
		{	"media",		eOxMP_SUBTITLETYPE_Media	},
		{	NULL,			0							}
	}
};

STATIC HCHAR *s_aszRestDataType_ComponentNames[] =
{
	NULL,					// eOxMP_COMPONENT_NONE
	"videoComponent",		// eOxMP_COMPONENT_VIDEO
	"audioComponent",		// eOxMP_COMPONENT_AUDIO
	"subtitleComponent",	// eOxMP_COMPONENT_SUBTITLE
	NULL,					// eOxMP_COMPONENT_TELETEXT,
	NULL,					// eOxMP_COMPONENT_CLOSEDCAPTION,
	"dataComponent",		// eOxMP_COMPONENT_DATA
	
	NULL					// eOxMP_COMPONENT_ENDOFCASE
};

STATIC HCHAR *s_aszRestDataType_CurrCompIndex[] =
{
	NULL,					// eOxMP_COMPONENT_NONE
	"currentVideoIndex",	// eOxMP_COMPONENT_VIDEO
	"currentAudioIndex",	// eOxMP_COMPONENT_AUDIO
	"currentSubtitleIndex",	// eOxMP_COMPONENT_SUBTITLE
	NULL,					// eOxMP_COMPONENT_TELETEXT,
	NULL,					// eOxMP_COMPONENT_CLOSEDCAPTION,
	"currentDataIndex",		// eOxMP_COMPONENT_DATA
	
	NULL					// eOxMP_COMPONENT_ENDOFCASE
};


#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
HINT32 rest_datatype_JsonizePlayerStatus (HINT32 nJsonWriter, RestPlayerStatus_t *pstStatus, HCHAR *szSelect)
{
	if (NULL == pstStatus)
	{
		HxLOG_Error("pstStatus NULL\n");
		return nJsonWriter;
	}

	if ((eOxMP_PLAYSTATE_NONE == pstStatus->ePlayState) ||
		(eOxMP_PLAYSTATE_STOPPED == pstStatus->ePlayState))
	{
		return rest_datatype_JsonizePlayerStatus_None(nJsonWriter, pstStatus, szSelect);
	}

	switch (pstStatus->unSource.eMediaType)
	{
	case eOxMP_MEDIATYPE_LIVE:
		return rest_datatype_JsonizePlayerStatus_Live(nJsonWriter, pstStatus, szSelect);

	case eOxMP_MEDIATYPE_PVRPLAYBACK:
		return rest_datatype_JsonizePlayerStatus_PvrPb(nJsonWriter, pstStatus, szSelect);

	case eOxMP_MEDIATYPE_TSRPLAYBACK:
		return rest_datatype_JsonizePlayerStatus_TsrPb(nJsonWriter, pstStatus, szSelect);
		
	default:
		return rest_datatype_JsonizePlayerStatus_None(nJsonWriter, pstStatus, szSelect);
	}

	return nJsonWriter;
}

HINT32 rest_datatype_JsonizePlayerStatus_None (HINT32 nJsonWriter, RestPlayerStatus_t *pstStatus, HCHAR *szSelect)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "viewId", (HINT32)pstStatus->ulViewId, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "sourceType", "none", szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "state", DxNAMEOF(OxMediaPlay_PlayState_e), (HINT32)pstStatus->ePlayState, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HINT32 rest_datatype_JsonizePlayerStatus_Live (HINT32 nJsonWriter, RestPlayerStatus_t *pstStatus, HCHAR *szSelect)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		HINT32			 nCompType, nCount;
		
		(void)rest_datatype_SetJsonInteger(nWriter, "playerId", (HINT32)pstStatus->ulViewId, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "sourceType", "live", szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "state", DxNAMEOF(OxMediaPlay_PlayState_e), (HINT32)pstStatus->ePlayState, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "svcUid", (HINT32)pstStatus->unSource.stLive.ulSvcUid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "speed", 1, szSelect);

		// Components
		for (nCompType = 0; nCompType < eOxMP_COMPONENT_ENDOFCASE; nCompType++)
		{
			HCHAR		*szCompName = s_aszRestDataType_ComponentNames[nCompType];
			HCHAR		*szCurrIdxName = s_aszRestDataType_CurrCompIndex[nCompType];

			if ((NULL != szCompName) && (NULL != szCurrIdxName))
			{
				RestPlayerComponent_t		 *pstComponent = &(pstStatus->astComponents[nCompType]);

				if ((pstComponent->nCurrIndex >= 0) && (pstComponent->nNumComponent > 0) &&
					(NULL != pstComponent->pstCompArray))
				{
					(void)rest_datatype_SetJsonInteger(nWriter, szCurrIdxName, pstComponent->nCurrIndex, szSelect);

					if (rest_datatype_CheckWriteSelected(szCompName, szSelect))
					{
						(void)HxJSONWRITER_Array(nWriter, szCompName);
						for (nCount = 0; nCount < pstComponent->nNumComponent; nCount++)
						{
							(void)rest_datatype_JsonizeMediaComponent(nWriter, &(pstComponent->pstCompArray[nCount]), NULL);
						}
						(void)HxJSONWRITER_ArrayEnd(nWriter);
					}
				}
			}
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}

HINT32 rest_datatype_JsonizePlayerStatus_PvrPb (HINT32 nJsonWriter, RestPlayerStatus_t *pstStatus, HCHAR *szSelect)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		HINT32			 nCompType, nCount;
		
		(void)rest_datatype_SetJsonInteger(nWriter, "viewId", (HINT32)pstStatus->ulViewId, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "sourceType", "pvrpb", szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "state", DxNAMEOF(OxMediaPlay_PlayState_e), (HINT32)pstStatus->ePlayState, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "recordUid", (HINT32)pstStatus->unSource.stPvrPb.ulContentsId, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "speed", (HINT32)pstStatus->nSpeed, szSelect);

		// Components
		for (nCompType = 0; nCompType < eOxMP_COMPONENT_ENDOFCASE; nCompType++)
		{
			HCHAR		*szCompName = s_aszRestDataType_ComponentNames[nCompType];
			HCHAR		*szCurrIdxName = s_aszRestDataType_CurrCompIndex[nCompType];

			if ((NULL != szCompName) && (NULL != szCurrIdxName))
			{
				RestPlayerComponent_t		 *pstComponent = &(pstStatus->astComponents[nCompType]);

				if ((pstComponent->nCurrIndex >= 0) && (pstComponent->nNumComponent > 0) &&
					(NULL != pstComponent->pstCompArray))
				{
					(void)rest_datatype_SetJsonInteger(nWriter, szCurrIdxName, pstComponent->nCurrIndex, szSelect);

					if (rest_datatype_CheckWriteSelected(szCompName, szSelect))
					{
						(void)HxJSONWRITER_Array(nWriter, szCompName);
						for (nCount = 0; nCount < pstComponent->nNumComponent; nCount++)
						{
							(void)rest_datatype_JsonizeMediaComponent(nWriter, &(pstComponent->pstCompArray[nCount]), NULL);
						}
						(void)HxJSONWRITER_ArrayEnd(nWriter);
					}
				}
			}
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}

HINT32 rest_datatype_JsonizePlayerStatus_TsrPb (HINT32 nJsonWriter, RestPlayerStatus_t *pstStatus, HCHAR *szSelect)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		HINT32			 nCompType, nCount;
		
		(void)rest_datatype_SetJsonInteger(nWriter, "viewId", (HINT32)pstStatus->ulViewId, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "sourceType", "tsrpb", szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "state", DxNAMEOF(OxMediaPlay_PlayState_e), (HINT32)pstStatus->ePlayState, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "svcUid", (HINT32)pstStatus->unSource.stLive.ulSvcUid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "speed", (HINT32)pstStatus->nSpeed, szSelect);

		// Components
		for (nCompType = 0; nCompType < eOxMP_COMPONENT_ENDOFCASE; nCompType++)
		{
			HCHAR		*szCompName = s_aszRestDataType_ComponentNames[nCompType];
			HCHAR		*szCurrIdxName = s_aszRestDataType_CurrCompIndex[nCompType];

			if ((NULL != szCompName) && (NULL != szCurrIdxName))
			{
				RestPlayerComponent_t		 *pstComponent = &(pstStatus->astComponents[nCompType]);

				if ((pstComponent->nCurrIndex >= 0) && (pstComponent->nNumComponent > 0) &&
					(NULL != pstComponent->pstCompArray))
				{
					(void)rest_datatype_SetJsonInteger(nWriter, szCurrIdxName, pstComponent->nCurrIndex, szSelect);

					if (rest_datatype_CheckWriteSelected(szCompName, szSelect))
					{
						(void)HxJSONWRITER_Array(nWriter, szCompName);
						for (nCount = 0; nCount < pstComponent->nNumComponent; nCount++)
						{
							(void)rest_datatype_JsonizeMediaComponent(nWriter, &(pstComponent->pstCompArray[nCount]), NULL);
						}
						(void)HxJSONWRITER_ArrayEnd(nWriter);
					}
				}
			}
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}

HINT32 rest_datatype_JsonizeMediaComponent (HINT32 nJsonWriter, OxMediaPlay_Component_t *pstComponent, HCHAR *szSelect)
{
	switch (pstComponent->eCompType)
	{
	case eOxMP_COMPONENT_VIDEO:
		return rest_datatype_JsonizeMediaComponent_Video(nJsonWriter, &(pstComponent->stVideo), szSelect);

	case eOxMP_COMPONENT_AUDIO:
		return rest_datatype_JsonizeMediaComponent_Audio(nJsonWriter, &(pstComponent->stAudio), szSelect);
		
	case eOxMP_COMPONENT_SUBTITLE:
		return rest_datatype_JsonizeMediaComponent_Subtitle(nJsonWriter, &(pstComponent->stSubtitle), szSelect);

	case eOxMP_COMPONENT_DATA:
		return rest_datatype_JsonizeMediaComponent_Data(nJsonWriter, &(pstComponent->stData), szSelect);

	default:
		break;
	}

	return nJsonWriter;
}

HINT32 rest_datatype_JsonizeMediaComponent_Video (HINT32 nJsonWriter, OxMediaPlay_VideoComp_t *pstVideo, HCHAR *szSelect)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		DxVideoType_e		 eVideoType = (pstVideo->nHdVideo) ? eDxVIDEO_TYPE_HD : eDxVIDEO_TYPE_SD;
		
		(void)rest_datatype_SetJsonEnum(nWriter, "componentType", DxNAMEOF(OxMediaPlay_Component_e), (HINT32)pstVideo->eCompType, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "pid", (HINT32)pstVideo->nPid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "componentTag", (HINT32)pstVideo->nComponentTag, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "videoCodec", DxNAMEOF(DxVideoCodec_e), (HINT32)pstVideo->eCodec, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "videoType", DxNAMEOF(DxVideoType_e), (HINT32)eVideoType, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}

HINT32 rest_datatype_JsonizeMediaComponent_Audio (HINT32 nJsonWriter, OxMediaPlay_AudioComp_t *pstAudio, HCHAR *szSelect)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		HINT32			 nIndex;
		HINT32			 nLangNum = (pstAudio->nLangNum < OxMP_AUDIO_LANG_NUM) ? pstAudio->nLangNum : OxMP_AUDIO_LANG_NUM;

		(void)rest_datatype_SetJsonEnum(nWriter, "componentType", DxNAMEOF(OxMediaPlay_Component_e), (HINT32)pstAudio->eCompType, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "pid", (HINT32)pstAudio->nPid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "componentTag", (HINT32)pstAudio->nComponentTag, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "audioCodec", DxNAMEOF(DxAudioCodec_e), (HINT32)pstAudio->eCodec, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "audioMode", DxNAMEOF(OxMediaPlay_AudioMode_e), (HINT32)pstAudio->eMode, szSelect);

		if ((rest_datatype_CheckWriteSelected("language", szSelect)) && (nLangNum > 0))
		{
			(void)HxJSONWRITER_Array(nWriter, "language");
			{
				(void)HxJSONWRITER_ObjectBegin(nWriter);
				for (nIndex = 0; nIndex < nLangNum; nIndex++)
				{
					OxMediaPlay_AudioALangInfo_t	*pstAudioLangInfo = &(pstAudio->astLangInfo[nIndex]);

					(void)rest_datatype_SetJsonEnum(nWriter, "audioType", DxNAMEOF(OxMediaPlay_AUDIOType_e), (HINT32)pstAudioLangInfo->eAudType, NULL);
					(void)rest_datatype_SetJsonString(nWriter,"languageCode", (HCHAR *)pstAudioLangInfo->szLangCode, NULL);
				}
				(void)HxJSONWRITER_ObjectEnd(nWriter);
			}
			(void)HxJSONWRITER_ArrayEnd(nWriter);
		}

		(void)rest_datatype_SetJsonEnum(nWriter, "audioSuppleType", DxNAMEOF(OxMediaPlay_AUDIOType_SUPP_e), (HINT32)pstAudio->eSupAudType, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}

HINT32 rest_datatype_JsonizeMediaComponent_Subtitle (HINT32 nJsonWriter, OxMediaPlay_SubtitleComp_t *pstSubtitle, HCHAR *szSelect)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonEnum(nWriter, "componentType", DxNAMEOF(OxMediaPlay_Component_e), (HINT32)pstSubtitle->eCompType, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "pid", (HINT32)pstSubtitle->nPid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "componentTag", (HINT32)pstSubtitle->nComponentTag, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "subtitleType", DxNAMEOF(OxMediaPlay_SubtitleType_e), (HINT32)pstSubtitle->eSubtitleType, szSelect);
		switch(pstSubtitle->eSubtitleType)
		{
		case eOxMP_SUBTITLETYPE_Dvb:
			{
				OxMediaPlay_Subtitle_DVB_t	 *pstDvbInfo = &(pstSubtitle->utArg.stDvbInfo);

				if (pstDvbInfo->nTypeValue >= 0)
				{
					(void)rest_datatype_SetJsonInteger(nWriter, "typeValue", (HINT32)pstDvbInfo->nTypeValue, szSelect);
				}
				
				if (pstDvbInfo->nComponentTag >= 0)
				{
					(void)rest_datatype_SetJsonInteger(nWriter, "componentTag", (HINT32)pstDvbInfo->nComponentTag, szSelect);
				}
				
				if (pstDvbInfo->nCompositionPageId >= 0)
				{
					(void)rest_datatype_SetJsonInteger(nWriter, "compositionPageId", (HINT32)pstDvbInfo->nCompositionPageId, szSelect);
				}

				if (pstDvbInfo->nAncillaryPageId >= 0)
				{
					(void)rest_datatype_SetJsonInteger(nWriter, "ancillaryPageId", (HINT32)pstDvbInfo->nAncillaryPageId, szSelect);
				}

				if ('\0' != pstDvbInfo->szLangCode[0])
				{
					(void)rest_datatype_SetJsonString(nWriter, "languageCode", pstDvbInfo->szLangCode, szSelect);
				}
			}

			break;

		case eOxMP_SUBTITLETYPE_Ebu:
			{
				OxMediaPlay_Subtitle_EBU_t	 *pstEbuInfo = &(pstSubtitle->utArg.stEbuInfo);

				if (pstEbuInfo->nTypeValue >= 0)
				{
					(void)rest_datatype_SetJsonInteger(nWriter, "typeValue", (HINT32)pstEbuInfo->nTypeValue, szSelect);
				}
				
				if (pstEbuInfo->nComponentTag >= 0)
				{
					(void)rest_datatype_SetJsonInteger(nWriter, "componentTag", (HINT32)pstEbuInfo->nComponentTag, szSelect);
				}
				
				if (pstEbuInfo->nMagazineNum >= 0)
				{
					(void)rest_datatype_SetJsonInteger(nWriter, "magazineNum", (HINT32)pstEbuInfo->nMagazineNum, szSelect);
				}

				if (pstEbuInfo->nPageNum >= 0)
				{
					(void)rest_datatype_SetJsonInteger(nWriter, "pageNum", (HINT32)pstEbuInfo->nPageNum, szSelect);
				}

				if ('\0' != pstEbuInfo->szLangCode[0])
				{
					(void)rest_datatype_SetJsonString(nWriter, "languageCode", pstEbuInfo->szLangCode, szSelect);
				}
			}

			break;

		case eOxMP_SUBTITLETYPE_Media:
			{
				OxMediaPlay_Subtitle_MEDIA_t	 *pstMediaInfo = &(pstSubtitle->utArg.stMediaInfo);

				if ('\0' != pstMediaInfo->szLangCode[0])
				{
					(void)rest_datatype_SetJsonString(nWriter, "languageCode", pstMediaInfo->szLangCode, szSelect);
				}
			}

			break;

		default:
			break;
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}

HINT32 rest_datatype_JsonizeMediaComponent_Data (HINT32 nJsonWriter, OxMediaPlay_DataComp_t *pstData, HCHAR *szSelect)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonEnum(nWriter, "componentType", DxNAMEOF(OxMediaPlay_Component_e), (HINT32)pstData->eCompType, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "pid", (HINT32)pstData->nPid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "componentTag", (HINT32)pstData->nComponentTag, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "streamName", pstData->szStreamName, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;
}

HERROR rest_datatype_InitPlayer (void)
{
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SrcType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_PlayState);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_ComponentType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_AudioMode);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_AudioType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SuppleAudioType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SubtitleType);

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___

