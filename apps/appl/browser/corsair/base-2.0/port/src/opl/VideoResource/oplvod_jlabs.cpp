/*************************************************************************************************************
	File 		: oplvod_jlabs.cpp
	author 		: B2BGroup tb2bgsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/16
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Â¡Â°HumaxÂ¡Â±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED Â¡Â°AS ISÂ¡Â± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#include "oplvod_jlabs.h"

#include "hlib.h"
#include "oapi.h"
#include "apk.h"

#include "hapi.h"

#include <cstdio>
#include <sys/time.h>
#include "oplprogramme_japan.h"


static eOplAVComponentSubType __convert_audio_to_subtype(OxMediaPlay_AudioMode_e type)
{
	switch ( type )
	{
	case eOxMP_AUDMODE_SINGLEMONO:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_SINGLEMONO;

	case eOxMP_AUDMODE_DUALMONO:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_DUALMONO;

	case eOxMP_AUDMODE_STEREO:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;

	case eOxMP_AUDMODE_MULTILINGUAL:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_MULTILINGUAL;

	case eOxMP_AUDMODE_SURROUND:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_SURROUND;

	case eOxMP_AUDMODE_DOLBY:
		return OPLAVCOMPONENT_SUBTYPE_AUDIO_DOLBY;

	default:
		break;
	}

	return OPLAVCOMPONENT_SUBTYPE_AUDIO_STEREO;
}

static eOplAVComponentStreamType __convert_audio_to_stream_type(DxAudioCodec_e codec)
{
	switch ( codec )
	{
		case eDxAUDIO_CODEC_MPEG:
			return OPLAVCOMPONENT_STREAM_TYPE_AUDIO_CODEC_MPEG2;

		case eDxAUDIO_CODEC_AAC:
			return OPLAVCOMPONENT_STREAM_TYPE_AUDIO_CODEC_AAC;

		case eDxAUDIO_CODEC_AAC_PLUS:
			return OPLAVCOMPONENT_STREAM_TYPE_AUDIO_CODEC_AAC_PLUS;

		default:
			break;
	}

	return OPLAVCOMPONENT_STREAM_TYPE_NONE;
}

/****************************************************************************************
 * OplVodJlabs
 ***************************************************************************************/
OplVodJlabs::OplVodJlabs(int aVideoResouceId)
	: OplVodAVControl(aVideoResouceId)
{
}

OplVodJlabs::~OplVodJlabs()
{
}

bool OplVodJlabs::getProgramme(OplVodProgramme_t *out_prog)
{
	HERROR 					eRet = ERR_FAIL;
	DxEvent_t 				event_info;
	HUINT32					ulLongDescriptionLen = 0;


#if	1
	HxLOG_Critical("[%s;%d] ---> not tested and implemented!!!\n", __FUNCTION__, __LINE__);
#else
	eRet = APK_MEDIA_PLAY_GetEventInfo(m_dest, &event_info);

	if ( eRet != ERR_OK )
	{
		return false;
	}

	out_prog->svcuid = event_info.svcuid;
	out_prog->eventId = event_info.eventId;
	out_prog->parentalRating = event_info.parentalRating;

	if ( event_info.name != NULL )
	{
		out_prog->name = HLIB_STD_StrDup(event_info.name);
		HLIB_STD_MemFree(event_info.name);
	}
	else
	{
		out_prog->name = NULL;
	}

	if ( event_info.text != NULL )
	{
		out_prog->description = HLIB_STD_StrDup(event_info.text);
		HLIB_STD_MemFree(event_info.text);
	}
	else
	{
		out_prog->description = NULL;
	}

	if (HxSTD_StrEmpty(event_info.longText) == FALSE)
	{
		ulLongDescriptionLen = HxSTD_StrLen(event_info.longText);
#if 0
		if (ulLongDescriptionLen != event_info.ulLongDescriptionLen)
		{
			HxLOG_Warning("Different long description size=[%d:%d] \n", ulLongDescriptionLen, event_info.ulLongDescriptionLen);
			HxLOG_Print("Received long description=%s\n", event_info.longDescription);
			out_prog->longDescription = (char *)HLIB_STD_MemAlloc(event_info.ulLongDescriptionLen + 1);
			if (out_prog->longDescription != NULL)
			{
				HxSTD_MemSet(out_prog->longDescription, 0, event_info.ulLongDescriptionLen + 1);
				HxSTD_StrNCpy(out_prog->longDescription, event_info.longDescription, event_info.ulLongDescriptionLen);
				out_prog->longDescription[event_info.ulLongDescriptionLen] = '\0';
			}
		}
		else
#endif
		{
			out_prog->longDescription = HLIB_STD_StrDup(event_info.longText);
		}
		HLIB_STD_MemFree(event_info.longText);
	}
	else
	{
		out_prog->longDescription = NULL;
	}

	out_prog->startTime = event_info.startTime;

	if (event_info.ulDuration != 0)
	{
		out_prog->duration = event_info.duration;
	}
	else
	{
		HxLOG_Warning("Is this undefined event informaiton? \n");
		out_prog->duration = 0x00ffffff;
	}
	out_prog->hasRecording = FALSE;
	out_prog->subtitles = event_info.bSubtitles;
	out_prog->isHD = event_info.bIsHD;
	out_prog->content = ((event_info.ucContentNibble << 8) | event_info.ucUserNibble);
	out_prog->digitalCopyControl = event_info.ucDigitalCopyControl;
	out_prog->analogCopyControl = event_info.ucAnalogCopyControl;
	out_prog->is3D = FALSE;
	out_prog->isCharged = FALSE;
	out_prog->contractVertificationInfo = NULL;
	out_prog->recordingStatus = 0;
#endif

	return true;
}

bool OplVodJlabs::getBmlStartFlag()
{
	// not implemented yet.

	return true;
}

bool OplVodJlabs::getBmlEndParam(int *out_cause, char *out_param)
{
	// Not implemented yet.

	return true;
	if ( out_cause ) *out_cause = 0;
	if ( out_param ) out_param = NULL;

	return true;
}

bool OplVodJlabs::showVideoMessage(int type)
{
	HERROR eRet = ERR_OK;

	eRet = APK_MEDIA_PLAY_SetVideoHide(m_dest, TRUE);
	if (ERR_OK != eRet)
	{
		HxLOG_Error("APK_MEDIA_PLAY_SetVideoHide ret Error!!!!\n");
	}

	return eRet == ERR_OK;
}

bool OplVodJlabs::hideVideoMessage(int type)
{
	HERROR eRet = ERR_OK;

	eRet = APK_MEDIA_PLAY_SetVideoHide(m_dest, FALSE);
	if (ERR_OK != eRet)
	{
		HxLOG_Error("APK_MEDIA_PLAY_SetVideoHide ret Error!!!!\n");
	}

	return eRet == ERR_OK;
}

bool OplVodJlabs::startBML()
{
	// Not implemented yet.

	return true;
}

bool OplVodJlabs::endBML()
{
	// Not implemented yet.

	return true;
}

bool OplVodJlabs::setBMLKeyControlMode(bool permit, int *ret_val)
{
//	APK_EXE_BML_Set((HBOOL)permit);
	*ret_val = 0; // ½ÇÆÐÇÒ ÀÌÀ¯°¡ ¾ø´Ù. octo °¡ Á×¾î¼­?
	return true;
}

#define __AV_VIDEO_COMPONENT__
unsigned int OplVodJlabs::getVideoComponentCount()
{
	HERROR 			eRet = ERR_FAIL;
	unsigned int 	count = 0;

	eRet = APK_MEDIA_PLAY_GetComponentNum(m_dest, eOxMP_COMPONENT_VIDEO, &count);

	return ( eRet == ERR_OK ) ? count : 0;
}

bool OplVodJlabs::getVideoComponent(int index, OplAVComponent_t *out_comp)
{
	HERROR 						eRet = ERR_FAIL;
	OxMediaplay_Component_t 	stComponet;

	if (!out_comp)
	{
		return false;
	}

	eRet = APK_MEDIA_PLAY_GetComponent(m_dest, eOxMP_COMPONENT_VIDEO, index, &stComponet);
	if ( eRet != ERR_OK )
	{
		return false;
	}

	out_comp->eType				= OPLAVCOMPONENT_TYPE_VIDEO;
	out_comp->ulPid				= stComponet.stVideo.nPid;
	out_comp->ulComponentTag	= stComponet.stVideo.nComponentTag;
	out_comp->ulStreamType 		= stComponet.stVideo.nStreamType;
	out_comp->ulComponentType 	= stComponet.stVideo.nComponentType;

#if defined(CONFIG_OP_JAPAN)
//	out_comp->fAspectRatio = OPL_PROGRAMME_JAPAN_GetVideoAspectRatio(video_track.nComponentType);
#endif
	HLIB_STD_StrNCpySafe(out_comp->szText, reinterpret_cast<char *>(stComponet.stVideo.aucStreamName),OPLAVCOMPONENT_TEXT_SIZE);

#if	defined(CONFIG_DEBUG)
	HxLOG_Warning("===== VideoComponent Info index=[%d] =====\n", index);
	HxLOG_Warning("usPid=%#x\n", 			stComponet.stVideo.nPid);
	HxLOG_Warning("ucComponentTag=%#x\n", 	stComponet.stVideo.nComponentTag);
	HxLOG_Warning("nComponentType=%#x\n", 	stComponet.stVideo.nComponentType);
	HxLOG_Warning("nStreamType=%#x\n\n", 	stComponet.stVideo.nStreamType);
#endif

	return true;
}

bool OplVodJlabs::getActiveVideoComponent(OplAVComponent_t *out_comp)
{
	HERROR 		eRet = ERR_FAIL;
	int 		index = 0;

	eRet = APK_MEDIA_PLAY_GetComponentIndex(m_dest, eOxMP_COMPONENT_VIDEO, &index, NULL);

	if ( eRet != ERR_OK )
	{
		return false;
	}

	return getVideoComponent(index, out_comp);
}

bool OplVodJlabs::setActiveVideoComponent(int pid)
{
	HERROR 			eRet = ERR_FAIL;
	unsigned int 	count = 0;
	OxMediaplay_Component_t 	stComponet;

	eRet = APK_MEDIA_PLAY_GetComponentNum(m_dest, eOxMP_COMPONENT_VIDEO, &count);

	if ((eRet != ERR_OK) || (count == 0))
	{
		return false;
	}

	for (unsigned int i=0; i<count; ++i)
	{
		eRet = APK_MEDIA_PLAY_GetComponent(m_dest, eOxMP_COMPONENT_VIDEO, i, &stComponet);
		if ( eRet != ERR_OK ) continue;
		if ( stComponet.stVideo.nPid == pid )
		{
			eRet = APK_MEDIA_PLAY_SetComponentIndex(m_dest, eOxMP_COMPONENT_VIDEO, i);
			if ( eRet != ERR_OK ) continue;
			return true;
		}
	}

	return false;
}

#define __AV_AUDIO_COMPONENT__
unsigned int OplVodJlabs::getAudioComponentCount()
{
	HERROR 			eRet = ERR_FAIL;
	unsigned int 	count = 0;

	eRet = APK_MEDIA_PLAY_GetComponentNum(m_dest, eOxMP_COMPONENT_AUDIO, &count);

	return ( eRet == ERR_OK ) ? count : 0;
}

bool OplVodJlabs::getAudioComponent(int index, OplAVComponent_t *out_comp)
{
	HERROR 						eRet = ERR_FAIL;
	OxMediaplay_Component_t 	stComponet;

	if (!out_comp)
	{
		return false;
	}

	eRet = APK_MEDIA_PLAY_GetComponent(m_dest, eOxMP_COMPONENT_AUDIO, index, &stComponet);
	if ( eRet != ERR_OK )
	{
		return false;
	}

	out_comp->eType 			= OPLAVCOMPONENT_TYPE_AUDIO;
	out_comp->ulComponentTag 	= (HUINT32)stComponet.stAudio.nComponentTag;
	out_comp->ulPid 			= (HUINT32)stComponet.stAudio.nPid;
	out_comp->eSubType 			= __convert_audio_to_subtype(stComponet.stAudio.eMode);

	HLIB_STD_StrNCpySafe(out_comp->szLanguage,reinterpret_cast<char *>(stComponet.stAudio.astLangInfo[0].szLangCode), 4);

	out_comp->ulComponentType 	= stComponet.stAudio.ucComponentType;
	out_comp->ulStreamType 		= __convert_audio_to_stream_type(stComponet.stAudio.eCodec);

	if ( (stComponet.stAudio.ucStreamContent == 0x02) && (out_comp->ulComponentType == 0x40) )
	{
		/* arib std-b10 6.2 Descriptor coding ( 101 page )*/
		out_comp->bAudioDescription = TRUE;
	}
	else
	{
		out_comp->bAudioDescription = FALSE;
	}

	switch ( out_comp->ulComponentType )
	{
		case 0x01:
		case 0x02:
			out_comp->ulAudioChannels = 1;
			break;

		case 0x03:
			out_comp->ulAudioChannels = 2;
			break;

		case 0x04:
		case 0x05:
			out_comp->ulAudioChannels = 3;
			break;

		case 0x06:
		case 0x07:
			out_comp->ulAudioChannels = 4;
			break;

		case 0x08:
		case 0x09:
			out_comp->ulAudioChannels = 5;
			break;

		default:
			out_comp->ulAudioChannels = 0;
			break;
	}

	HLIB_STD_StrNCpySafe(out_comp->szLanguage,reinterpret_cast<char *>(stComponet.stAudio.astLangInfo[0].szLangCode), 4);
	HLIB_STD_StrNCpySafe(out_comp->szLanguage2,reinterpret_cast<char *>(stComponet.stAudio.astLangInfo[1].szLangCode), 4);
	HLIB_STD_StrNCpySafe(out_comp->szText,reinterpret_cast<char *>(stComponet.stAudio.aucStreamName), OPLAVCOMPONENT_TEXT_SIZE);


#if	defined(CONFIG_DEBUG)
	HxLOG_Warning("===== AudioComponent Info index=[%d] =====\n", index);
	HxLOG_Warning("usPid=%#x\n", 			stComponet.stAudio.nPid);
	HxLOG_Warning("ucComponentTag=%#x\n",	stComponet.stAudio.nComponentTag);
	HxLOG_Warning("ulComponentType=%#x\n",	stComponet.stAudio.ucComponentType);
	HxLOG_Warning("ulStreamType=%#x\n\n",	out_comp->ulStreamType);
#endif

	return true;
}

bool OplVodJlabs::getActiveAudioComponent(OplAVComponent_t *out_comp)
{
	HERROR 			eRet = ERR_FAIL;
	int 			index = 0;

	if (!out_comp) return false;

	eRet = APK_MEDIA_PLAY_GetComponentIndex(m_dest, eOxMP_COMPONENT_AUDIO, &index, NULL);
	if ( eRet != ERR_OK )
	{
		return false;
	}

	return getAudioComponent(index, out_comp);
}

bool OplVodJlabs::setActiveAudioComponent(int pid)
{
	HERROR 			eRet = ERR_FAIL;
	unsigned int 	count = 0;

	OxMediaplay_Component_t stComponet;

	eRet = APK_MEDIA_PLAY_GetComponentNum(m_dest, eOxMP_COMPONENT_AUDIO, &count);
	if ( eRet != ERR_OK || count == 0 )
	{
		return false;
	}

	for (unsigned int i=0; i<count; ++i)
	{
		eRet = APK_MEDIA_PLAY_GetComponent(m_dest, eOxMP_COMPONENT_AUDIO, i, &stComponet);
		if ( eRet != ERR_OK ) continue;
		if ( stComponet.stAudio.nPid == pid )
		{
			eRet = APK_MEDIA_PLAY_SetComponentIndex(m_dest, eOxMP_COMPONENT_AUDIO, i);
			if ( eRet != ERR_OK ) continue;
			return true;
		}
	}

	return false;
}

#define __AV_SUBTITLE_COMPONENT__
unsigned int OplVodJlabs::getSubtitleComponentCount()
{
#if	1
	HxLOG_Critical("[%s;%d] <--- not tested & implemented...\n", __FUNCTION__, __LINE__);
	return 0;
#else
	HERROR			eRet = ERR_FAIL;
	DxEvent_t		event_info;

	eRet = APK_MEDIA_PLAY_GetEventInfo(m_dest, &event_info);
	if (event_info.name != NULL)
	{
		HLIB_STD_MemFree(event_info.name);
	}

	if (event_info.text != NULL)
	{
		HLIB_STD_MemFree(event_info.text);
	}

	if (HxSTD_StrEmpty(event_info.longText) == FALSE)
	{
		HLIB_STD_MemFree(event_info.longDescription);
	}

	return ( eRet == ERR_OK ) ? event_info.stCaptionList.ulItemNum : 0;
#endif
}


bool OplVodJlabs::getSubtitleComponent(int index, OplAVComponent_t *out_comp)
{
	HERROR			eRet = ERR_FAIL;
	DxEvent_t		event_info;

#if	1
	HxLOG_Critical("[%s;%d] <--- not tested & implemented...\n", __FUNCTION__, __LINE__);
	return false;
#else
	if (!out_comp) return false;

	eRet = APK_MEDIA_PLAY_GetEventInfo(m_dest, &event_info);

	if (event_info.name != NULL)
	{
		HLIB_STD_MemFree(event_info.name);
	}

	if (event_info.text != NULL)
	{
		HLIB_STD_MemFree(event_info.text);
	}

	if (HxSTD_StrEmpty(event_info.longText) == FALSE)
	{
		HLIB_STD_MemFree(event_info.longDescription);
	}

	if ((eRet != ERR_OK) || (index >= DxPVR_MAX_DATA_ES_NUM))
	{
		return false;
	}

	out_comp->eType = OPLAVCOMPONENT_TYPE_SUPERIMPOSE;
	out_comp->ulPid = event_info.stCaptionList.astDataInfo[index].usPid;
	out_comp->ulComponentTag = event_info.stCaptionList.astDataInfo[index].ucComponentTag;
	HLIB_STD_StrNCpySafe(out_comp->szLanguage,
		reinterpret_cast<char *>(event_info.stCaptionList.astDataInfo[index].szLangCode), 4);
	return true;
#endif
}


bool OplVodJlabs::getActiveSubtitleComponent(OplAVComponent_t *out_comp)
{
	return false;
/*
	if ( !m_is_pvr ) return false;
	if ( !out_comp ) return false;

	unsigned int index = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentIndex(m_dest, eOxMP_COMPONENT_SUBTITLE, &index, NULL);
	if ( eRet != ERR_OK ) return false;

	return getSubtitleComponent(index, out_comp);
*/
}

bool OplVodJlabs::setActiveSubtitleComponent(int pid)
{
	return false;
/*
	if ( !m_is_pvr ) return false;

	unsigned int count = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentNum(m_dest, eOxMP_COMPONENT_SUBTITLE, &count);
	if ( eRet != ERR_OK || count == 0 ) return false;

	for (unsigned int i=0; i<count; ++i)
	{
		eRet = APK_MEDIA_PLAY_GetComponent(m_dest, eOxMP_COMPONENT_SUBTITLE, i, &stComponet);
		if ( eRet != ERR_OK ) continue;
		if ( stComponet.stSubitle.nPid == pid )
		{
			eRet = APK_MEDIA_PLAY_SetComponentIndex(m_dest, eOxMP_COMPONENT_SUBTITLE, i);
			if ( eRet != ERR_OK ) continue;
			return true;
		}
	}

	return false;
*/
}

#define __AV_SUPERIMPOSE_COMPONENT__
unsigned int OplVodJlabs::getSuperimposeComponentCount()
{
#if	1
	HxLOG_Critical("[%s;%d] <--- not tested & implemented...\n", __FUNCTION__, __LINE__);
	return false;
#else
	HERROR			eRet = ERR_FAIL;
	DxEvent_t		event_info;

	eRet = APK_MEDIA_PLAY_GetEventInfo(m_dest, &event_info);

	if (event_info.name != NULL)
	{
		HLIB_STD_MemFree(event_info.name);
	}

	if (event_info.description != NULL)
	{
		HLIB_STD_MemFree(event_info.description);
	}

	if ((event_info.longDescription != NULL) && (event_info.ulLongDescriptionLen != 0))
	{
		HLIB_STD_MemFree(event_info.longDescription);
	}

	return ( eRet == ERR_OK ) ? event_info.stSuperimposeList.ulItemNum : 0;
#endif
}

bool OplVodJlabs::getSuperimposeComponent(int index, OplAVComponent_t *out_comp)
{
#if	1
	HxLOG_Critical("[%s;%d] <--- not tested & implemented...\n", __FUNCTION__, __LINE__);
	return false;
#else
	HERROR			eRet = ERR_FAIL;
	DxEvent_t		event_info;

	if (!out_comp) return false;

	eRet = APK_MEDIA_PLAY_GetEventInfo(m_dest, &event_info);

	if (event_info.name != NULL)
	{
		HLIB_STD_MemFree(event_info.name);
	}

	if (event_info.description != NULL)
	{
		HLIB_STD_MemFree(event_info.description);
	}

	if ((event_info.longDescription != NULL) && (event_info.ulLongDescriptionLen != 0))
	{
		HLIB_STD_MemFree(event_info.longDescription);
	}

	if ((eRet != ERR_OK) || (index >= DxPVR_MAX_DATA_ES_NUM))
	{
		return false;
	}

	out_comp->eType = OPLAVCOMPONENT_TYPE_SUPERIMPOSE;
	out_comp->ulPid = event_info.stSuperimposeList.astDataInfo[index].usPid;
	out_comp->ulComponentTag = event_info.stSuperimposeList.astDataInfo[index].ucComponentTag;
	HLIB_STD_StrNCpySafe(out_comp->szLanguage,
		reinterpret_cast<char *>(event_info.stSuperimposeList.astDataInfo[index].szLangCode), 4);
	return true;
#endif
}

#define __AV_DATA_COMPONENT__
unsigned int OplVodJlabs::getDataComponentCount()
{
#if	1
	HxLOG_Critical("[%s;%d] <--- not tested & implemented...\n", __FUNCTION__, __LINE__);
	return false;
#else
	HERROR			eRet = ERR_FAIL;
	DxEvent_t		event_info;

	eRet = APK_MEDIA_PLAY_GetEventInfo(m_dest, &event_info);

	if (event_info.name != NULL)
	{
		HLIB_STD_MemFree(event_info.name);
	}

	if (event_info.description != NULL)
	{
		HLIB_STD_MemFree(event_info.description);
	}

	if ((event_info.longDescription != NULL) && (event_info.ulLongDescriptionLen != 0))
	{
		HLIB_STD_MemFree(event_info.longDescription);
	}

#if	defined(CONFIG_DEBUG)
{
	int	i;

	if (eRet == ERR_OK)
	{
		HxLOG_Warning("===== DataComponent Info total=[%d] =====\n", event_info.stDataList.ulItemNum);
		for (i=0; i<event_info.stDataList.ulItemNum; i++)
		{
			if (event_info.stDataList.astDataInfo[i].ucComponentTag == 0x40)
			{
				HxLOG_Warning("usPid=%#x\n", event_info.stDataList.astDataInfo[i].usPid);
				HxLOG_Warning("ucComponentTag=%#x\n", event_info.stDataList.astDataInfo[i].ucComponentTag);
				HxLOG_Warning("uDataEntryComponentExistFlag=%d\n", event_info.stDataList.astDataInfo[i].uDataEntryComponentExistFlag);
				HxLOG_Warning("uAssociatedContentsFlag=%d\n", event_info.stDataList.astDataInfo[i].uAssociatedContentsFlag);
				HxLOG_Warning("aucTextChar=%s\n\n", event_info.stDataList.astDataInfo[i].aucTextChar);
			}
		}
	}
}
#endif

	return ( eRet == ERR_OK ) ? event_info.stDataList.ulItemNum : 0;
#endif
}

bool OplVodJlabs::getDataComponent(int index, OplAVComponent_t *out_comp)
{
#if	1
	HxLOG_Critical("[%s;%d] <--- not tested & implemented...\n", __FUNCTION__, __LINE__);
	return false;
#else
	HERROR			eRet = ERR_FAIL;
	DxEvent_t		event_info;

	if (!out_comp) return false;

	eRet = APK_MEDIA_PLAY_GetEventInfo(m_dest, &event_info);

	if (event_info.name != NULL)
	{
		HLIB_STD_MemFree(event_info.name);
	}

	if (event_info.description != NULL)
	{
		HLIB_STD_MemFree(event_info.description);
	}

	if ((event_info.longDescription != NULL) && (event_info.ulLongDescriptionLen != 0))
	{
		HLIB_STD_MemFree(event_info.longDescription);
	}

	if ((eRet != ERR_OK) || (index >= DxPVR_MAX_DATA_ES_NUM))
	{
		return false;
	}

	out_comp->eType = OPLAVCOMPONENT_TYPE_DATA;
	out_comp->ulPid = (HUINT32)event_info.stDataList.astDataInfo[index].usPid;
	out_comp->ulComponentTag = event_info.stDataList.astDataInfo[index].ucComponentTag;
	out_comp->bAssociatedContentFlag = event_info.stDataList.astDataInfo[index].uAssociatedContentsFlag;
	out_comp->bDataEntryComponentFlag = event_info.stDataList.astDataInfo[index].uDataEntryComponentExistFlag;
	out_comp->nDigitalCopyControl = event_info.ucDigitalCopyControl;
	HLIB_STD_StrNCpySafe(out_comp->szText,
		reinterpret_cast<char *>(event_info.stDataList.astDataInfo[index].aucTextChar),
		OPLAVCOMPONENT_TEXT_SIZE);
	return true;
#endif
}

#define __AV_CONTROL_OBJECT__
unsigned int OplVodJlabs::getComponentsCount(eOplAVComponentType comp_type)
{
	switch ( comp_type )
	{
		case OPLAVCOMPONENT_TYPE_VIDEO:
			return getVideoComponentCount();
		case OPLAVCOMPONENT_TYPE_AUDIO:
			return getAudioComponentCount();
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			return getSubtitleComponentCount();
		case OPLAVCOMPONENT_TYPE_SUPERIMPOSE:
			return getSuperimposeComponentCount();
		case OPLAVCOMPONENT_TYPE_DATA:
			return getDataComponentCount();
		default:
			break;
	}

	return 0;
}

bool OplVodJlabs::getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp)
{
	switch ( comp_type )
	{
		case OPLAVCOMPONENT_TYPE_VIDEO:
			return getVideoComponent(index, out_comp);
		case OPLAVCOMPONENT_TYPE_AUDIO:
			return getAudioComponent(index, out_comp);
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			return getSubtitleComponent(index, out_comp);
		case OPLAVCOMPONENT_TYPE_SUPERIMPOSE:
			return getSuperimposeComponent(index, out_comp);
		case OPLAVCOMPONENT_TYPE_DATA:
			return getDataComponent(index, out_comp);
		default:
			break;
	}

	return false;
}

bool OplVodJlabs::getActiveComponent(eOplAVComponentType comp_type, OplAVComponent_t *out_comp)
{
	switch ( comp_type )
	{
		case OPLAVCOMPONENT_TYPE_VIDEO:
			return getActiveVideoComponent(out_comp);
		case OPLAVCOMPONENT_TYPE_AUDIO:
			return getActiveAudioComponent(out_comp);
		default:
			break;
	}

	return false;
}

bool OplVodJlabs::setActiveComponent(eOplAVComponentType comp_type, int pid)
{
	switch ( comp_type )
	{
		case OPLAVCOMPONENT_TYPE_VIDEO:
			return setActiveVideoComponent(pid);
		case OPLAVCOMPONENT_TYPE_AUDIO:
			return setActiveAudioComponent(pid);
		default:
			break;
	}

	return false;
}

