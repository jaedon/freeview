/*************************************************************************************************************
	File 		: oplvod.cpp
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/16
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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

#include "OplVideoOnDemand.h"
#include "oplvideoutil.h"

#include "hlib.h"
#include "oapi.h"

#include <apk.h>

#include "hapi.h"

#include <iostream>
#include <sys/time.h>


#define INVALID_SPEED_VALUE 		0xFFFF
#define OPLVOD_PVR_URL_PREFEX		"hpvr://"
#define MAX_SPEED_NUM	24
#define OPLVOD_DOWNLOAD_URI_FORM    "hbuffer://cso_session/%d"

#define	IsHardOfHearingSubtitle(type)			(((type) == 0x05) || (0x20 <= (type) && (type) <= 0x24))


#if 1
#define ENTRY	HxLOG_Debug("[OplVideoOnDemand][%s:%d]++\n", __FUNCTION__, __LINE__);
#define EXIT	HxLOG_Debug("[OplVideoOnDemand][%s:%d]--\n", __FUNCTION__, __LINE__);
#else
#define ENTRY	HxLOG_Debug("\033[1;37;41m OplVideoOnDemand][%s:%d]++ \033[0m n", __FUNCTION__, __LINE__);
#define EXIT	HxLOG_Debug("\033[1;37;41m OplVideoOnDemand][%s:%d]-- \033[0m ", __FUNCTION__, __LINE__);
#define HxLOG_Debug(fmt, ...)  \
	do{\
		printf("\033[1;37;41m");\
		printf("[%s:%s:%d] ", __FILE__,__FUNCTION__,__LINE__);\
		printf( fmt, ##__VA_ARGS__);\
		printf("\n \033[0m");\
	}while(0)
#endif


using namespace std;

/* static functions */
static HBOOL oplvod_eventFilter (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated);
static void oplvod_proc (unsigned int ulViewId, unsigned int ulSessionId, OxMediaPlay_MediaType_e eType,
			OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData );
static void pvr_proc(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent,
			OxMediaPlay_NotifierData_t *pstNotifyData);
static void vod_proc(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent,
			OxMediaPlay_NotifierData_t *pstNotifyData);
static void oplvod_download_proc(HUINT32 ulDownloadId, DxDownload_Type_e eType, DxDownload_Event_e eEvent,
			DxDownload_NotifierData_t *punNotifyData, void *pvUserData);


// WIDGET_LOCAL_MEDIA
string OplVideoOnDemand::m_application_url;
string OplVideoOnDemand::m_useragent;


static eOplVideoOnDemandPlayState __convert_play_state(OxMediaPlay_PlayState_e state)
{
	eOplVideoOnDemandPlayState oipfVodState;
	switch ( state )
	{
	case eOxMP_PLAYSTATE_NONE:
	case eOxMP_PLAYSTATE_STOPPED:
		oipfVodState = OPLVOD_PLAY_STATE_STOPPED;
		break;
	case eOxMP_PLAYSTATE_PLAYING:
		oipfVodState = OPLVOD_PLAY_STATE_PLAYING;
		break;
	case eOxMP_PLAYSTATE_PAUSE:
		oipfVodState = OPLVOD_PLAY_STATE_PAUSED;
		break;
	case eOxMP_PLAYSTATE_CONNECTING:
	case eOxMP_PLAYSTATE_REQUESTED:
		oipfVodState = OPLVOD_PLAY_STATE_CONNECTING;
		break;
	case eOxMP_PLAYSTATE_BUFFERING:
		oipfVodState = OPLVOD_PLAY_STATE_BUFFERING;
		break;
	case eOxMP_PLAYSTATE_BOS:
		oipfVodState = OPLVOD_PLAY_STATE_PLAYING;
		break;
	case eOxMP_PLAYSTATE_EOS:
		oipfVodState = OPLVOD_PLAY_STATE_FINISHED;
		break;
	default:
		oipfVodState = OPLVOD_PLAY_STATE_ERROR;
		break;
	}

	return oipfVodState;
}

static eOplVideoOnDemandPlayError __convert_play_error(OxMediaPlay_PlayError_e error)
{
	eOplVideoOnDemandPlayError		 eOipfError;
	switch ( error )
	{
	case eOxMP_PLAYERROR_NO_ERROR:
		eOipfError = OPLVOD_PLAY_ERROR_NO_ERROR;
		break;
	case eOxMP_PLAYERROR_NOT_SUPPORTED:
	case eOxMP_PLAYERROR_PLAY_ERROR:
		eOipfError = OPLVOD_PLAY_ERROR_NOT_SUPPORTED_FORMAT;
		break;

	case eOxMP_PLAYERROR_SERVER_FAIL:
	case eOxMP_PLAYERROR_PROBE_FAILED:
		eOipfError = OPLVOD_PLAY_ERROR_SERVER_FAIL;
		break;

	case eOxMP_PLAYERROR_BUFFER_ERROR:
		eOipfError = OPLVOD_PLAY_ERROR_SERVER_FAIL;
		break;

	case eOxMP_PLAYERROR_PARENT_CONTROL:
		eOipfError = OPLVOD_PLAY_ERROR_PARENTAL_CONTROL;
		break;

	case eOxMP_PLAYERROR_SEEK_ERROR:
		eOipfError = OPLVOD_PLAY_ERROR_NOT_AVAILABLE_POSITION;
		break;

	case eOxMP_PLAYERROR_CONFLICT_RESOURCE:
		eOipfError = OPLVOD_PLAY_ERROR_INSUFFICIENT_RESOURCE;
		break;
	default:
		eOipfError = OPLVOD_PLAY_ERROR_UNDEFINED;
		break;
	}

	return eOipfError;
}

static eOplVideoOnDemandBufferState __convert_buffer_state(OxMediaPlay_BufferState_e state)
{
	eOplVideoOnDemandBufferState bufferState;

	switch ( state )
	{
	case eOxMP_BUFFERSTATE_CanPlay:
		bufferState = OPLVOD_BUFFER_STATE_CANPLAY;
		break;
	case eOxMP_BUFFERSTATE_NotEnough:
		bufferState = OPLVOD_BUFFER_STATE_NOT_ENOUGH;
		break;
	case eOxMP_BUFFERSTATE_Enough:
		bufferState = OPLVOD_BUFFER_STATE_ENOUGH;
		break;
	case eOxMP_BUFFERSTATE_Buffering_Finished:
		bufferState = OPLVOD_BUFFER_STATE_FINISHED;
		break;
	default:
		bufferState = OPLVOD_BUFFER_STATE_NONE;
		break;
	}

	return bufferState;
}

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

static const char *__convertAudioCodecToOplEncoding (DxAudioCodec_e eCodec)
{
	switch (eCodec)
	{
	case eDxAUDIO_CODEC_MPEG:
		return "MPEG1_L2";

	case eDxAUDIO_CODEC_MP3:
		return "MPEG1_L3";

	case eDxAUDIO_CODEC_DOLBY_AC3:
		return "AC3";

	case eDxAUDIO_CODEC_DOLBY_AC3P:
		return "E-AC3";

	case eDxAUDIO_CODEC_PCM:
		return "WAV";

	case eDxAUDIO_CODEC_AAC:
	case eDxAUDIO_CODEC_AAC_LOAS:
	case eDxAUDIO_CODEC_AAC_PLUS:
	case eDxAUDIO_CODEC_AAC_PLUS_ADTS:
		return "HEAAC";

	case eDxAUDIO_CODEC_DTS:
	case eDxAUDIO_CODEC_DTS_HD:
		return "DTS";

	default: break;
	}

	return "UNKNOWN";
}

static int __audio_codec_to_appkit(eOplVideoOnDemandAudioCodec codec)
{
	DxAudioCodec_e appkit_codec = eDxAUDIO_CODEC_UNKNOWN;
	switch ( codec )
	{
	case OPLVOD_AUDIO_CODEC_MPEG:
		appkit_codec = eDxAUDIO_CODEC_MPEG;
		break;
	case OPLVOD_AUDIO_CODEC_MP3:
		appkit_codec = eDxAUDIO_CODEC_MP3;
		break;
	case OPLVOD_AUDIO_CODEC_DOLBY_AC3:
		appkit_codec = eDxAUDIO_CODEC_DOLBY_AC3;
		break;
	case OPLVOD_AUDIO_CODEC_DOLBY_AC3P:
		appkit_codec = eDxAUDIO_CODEC_DOLBY_AC3P;
		break;
	case OPLVOD_AUDIO_CODEC_PCM:
		appkit_codec = eDxAUDIO_CODEC_PCM;
		break;
	case OPLVOD_AUDIO_CODEC_AAC:
		appkit_codec = eDxAUDIO_CODEC_AAC;
		break;
	case OPLVOD_AUDIO_CODEC_AAC_LOAS:
		appkit_codec = eDxAUDIO_CODEC_AAC_LOAS;
		break;
	case OPLVOD_AUDIO_CODEC_AAC_PLUS:
		appkit_codec = eDxAUDIO_CODEC_AAC_PLUS;
		break;
    case OPLVOD_AUDIO_CODEC_PCMWAVE:
        appkit_codec = eDxAUDIO_CODEC_PCMWAVE;
        break;
    case OPLVOD_AUDIO_CODEC_OPUS:
        appkit_codec = eDxAUDIO_CODEC_OPUS;
        break;
	default:
		break;
	}

	return static_cast<int>(appkit_codec);
}

static int __video_codec_to_appkit(eOplVideoOnDemandVideoCodec codec)
{
	DxVideoCodec_e appkit_codec = eDxVIDEO_CODEC_UNKNOWN;
	switch ( codec )
	{
	case OPLVOD_VIDEO_CODEC_MPEG1:
		appkit_codec = eDxVIDEO_CODEC_MPEG1;
		break;
	case OPLVOD_VIDEO_CODEC_MPEG2:
		appkit_codec = eDxVIDEO_CODEC_MPEG2;
		break;
	case OPLVOD_VIDEO_CODEC_MPEG4_PART2:
		appkit_codec = eDxVIDEO_CODEC_MPEG4_PART2;
		break;
	case OPLVOD_VIDEO_CODEC_H261:
		appkit_codec = eDxVIDEO_CODEC_H261;
		break;
	case OPLVOD_VIDEO_CODEC_H263:
		appkit_codec = eDxVIDEO_CODEC_H263;
		break;
	case OPLVOD_VIDEO_CODEC_H264:
		appkit_codec = eDxVIDEO_CODEC_H264;
		break;
	case OPLVOD_VIDEO_CODEC_H264_SVC:
		appkit_codec = eDxVIDEO_CODEC_H264_SVC;
		break;
	case OPLVOD_VIDEO_CODEC_H264_MVC:
		appkit_codec = eDxVIDEO_CODEC_H264_MVC;
		break;
	case OPLVOD_VIDEO_CODEC_VC1:
		appkit_codec = eDxVIDEO_CODEC_VC1;
		break;
	case OPLVOD_VIDEO_CODEC_VC1_SIMPLEMAIN:
		appkit_codec = eDxVIDEO_CODEC_VC1_SIMPLEMAIN;
		break;
	case OPLVOD_VIDEO_CODEC_AVS:
		appkit_codec = eDxVIDEO_CODEC_AVS;
		break;
	case OPLVOD_VIDEO_CODEC_RV40:
		appkit_codec = eDxVIDEO_CODEC_RV40;
		break;
	case OPLVOD_VIDEO_CODEC_VP6:
		appkit_codec = eDxVIDEO_CODEC_VP6;
		break;
	case OPLVOD_VIDEO_CODEC_DIVX311:
		appkit_codec = eDxVIDEO_CODEC_DIVX311;
		break;
	case OPLVOD_VIDEO_CODEC_XVID:
		appkit_codec = eDxVIDEO_CODEC_XVID;
		break;
	case OPLVOD_VIDEO_CODEC_VP8:
		appkit_codec = eDxVIDEO_CODEC_VP8;
		break;
	case OPLVOD_VIDEO_CODEC_VP9:
		appkit_codec = eDxVIDEO_CODEC_VP9;
		break;
	default:
		break;
	}

	return static_cast<int>(appkit_codec);
}



/****************************************************************************************
 * OplVideoOnDemand class
 ***************************************************************************************/
std::list<OplVideoOnDemand *> OplVideoOnDemand::s_vods;

OplVideoOnDemand::OplVideoOnDemand(int aVideoResourceId)
	:
	m_is_fullscreen(false),
	m_is_pvr(false),
	m_dest(0),
	m_user_data(NULL),
	m_latest_tick(0),
	m_parental_blocked(false),
	m_parental_rating_value(0),
	m_session_id(0),
	m_seleted_index(0),
	m_start_pos(0),
	m_last_pos(0),
	m_duration(0),
	m_download_id(0),
	m_listener(NULL),
	m_play_state(OPLVOD_PLAY_STATE_STOPPED),
	m_buffer_state(OPLVOD_BUFFER_STATE_NONE),
	m_play_error(OPLVOD_PLAY_ERROR_NO_ERROR),
	m_play_action(VOD_ACTION_STOP),
        m_cache(0),
	m_is_active(false),
	m_is_download_vod(false),
	m_duration_Infinity(0)

{
	ENTRY
	HERROR	hErr;
	HUINT32 	ulViewId;

	m_cas_system_id[0] = '\0';

	ulViewId = aVideoResourceId;

	m_dest = static_cast<int>(ulViewId);

	if ( m_useragent.size() )
		APK_MEDIA_PLAY_SetBrowserUserAgentData(ulViewId, (HCHAR *)m_useragent.c_str());

	if ( s_vods.empty() )
	{
		hErr = APK_MEDIA_PLAY_RegisterNotifier (ulViewId, oplvod_proc, oplvod_eventFilter, NULL);
		if( ERR_OK != hErr )
			HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_RegisterNotifier Error: \n", __FUNCTION__, __LINE__);

		hErr = APK_DOWNLOAD_RegisterNotifier (oplvod_download_proc, NULL);
		if( ERR_OK != hErr )
			HxLOG_Error("[%s:%d] APK_DOWNLOAD_RegisterNotifier Error: \n", __FUNCTION__, __LINE__);
	}

	memset(&m_probe_info, 0x0, sizeof(m_probe_info));
	setDownloadState(OPLVOD_DOWNLOAD_STATE_CREATE);
	checkVodsStatus();

	s_vods.push_back(this);

	HxLOG_Debug("OplVideoOnDemand object is created. (handle=%p, total=%d\n", this, s_vods.size());
}

OplVideoOnDemand::~OplVideoOnDemand()
{
	ENTRY
	if( m_is_active == true) 	stop();

	s_vods.remove(this);

	if ( s_vods.empty() )
	{
		stop();
		//setFullScreen(true);

		{
			HERROR hErr;
			HUINT32 	ulViewId;

			ulViewId = m_dest;

			hErr = APK_DOWNLOAD_UnregisterNotifier(oplvod_download_proc, NULL);
			if( ERR_OK != hErr )
				HxLOG_Error("[%s:%d] APK_DOWNLOAD_UnregisterNotifier Error: \n", __FUNCTION__, __LINE__);

			hErr = APK_MEDIA_PLAY_UnregisterNotifier(ulViewId, oplvod_proc, NULL);
			if( ERR_OK != hErr )
				HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_UnregisterNotifier Error: \n", __FUNCTION__, __LINE__);
		}
	}

	if(!m_queue_url.empty())
		m_queue_url.clear();

	HxLOG_Debug("OplVideoOnDemand object is destroyed. (total=%d\n", this, s_vods.size());
}

void OplVideoOnDemand::setListener(void *hPlayer, OplVideoOnDemandListener_t listener)
{
	ENTRY
	m_player = hPlayer;
	m_listener = listener;
}

void OplVideoOnDemand::setUrl(const char *src_url)
{
    ENTRY;
    if ( NULL == src_url ) return;
	
    if ( 0 == m_url.compare(src_url) )
    {
        HxLOG_Debug("OplVideoOnDemand::setUrl duplicated url.\n");
        return;
    }

    string dest_url = src_url;
#if 1
	/* BEGIN OF DTV Labs FVP v0.10 */
    const char *is_url = strstr(src_url, "://");
    if(is_url == NULL)
    {
        string new_url = OplVideoOnDemand::m_application_url;
        std::size_t found = new_url.find_last_of("/");
        if(found)
        {
            new_url.erase(found+1);
            new_url.append(src_url);
        }
        dest_url = new_url;
        HxLOG_Debug( "url :: %s  \n", new_url.c_str() );
    }
    /* END OF DTV Labs FVP v0.10 */
#endif
	stop(true);

	m_url = dest_url;
	m_is_pvr = (HLIB_STD_StrNCaseCmp(m_url.c_str(), OPLVOD_PVR_URL_PREFEX,
				HxSTD_StrLen(OPLVOD_PVR_URL_PREFEX)) == 0) ? true : false;

	// for sdk 3.5, recording data's prefix is capital letter. 
	if( HxSTD_StrNCmp(m_url.c_str(), OPLVOD_PVR_URL_PREFEX, HxSTD_StrLen(OPLVOD_PVR_URL_PREFEX)) == 0 )
	{
		std::string findString = OPLVOD_PVR_URL_PREFEX;
		std::string::size_type offset = 0;
		while( true )
		{
			offset = m_url.find(findString, offset);
			if(std::string::npos == offset)
				break;
			else
				m_url.replace(offset, findString.length(),"HPVR://");
		}
	}
	
	HxLOG_Debug("OplVideoOnDemand::setUrl: url=%s\n", m_url.c_str());
	touch();


	EXIT;
}

bool OplVideoOnDemand::setPlayerType(eOplVideoOnDemandPlayerType type)
{
	ENTRY
	OxMediaPlay_PlayerType_e apk_type = eOxMP_MEDIAPLAY_PLAYERTYPE_NONE;

	switch ( type )
	{
	case OPLVOD_PLAYER_TYPE_MSE:
		apk_type = eOxMP_MEDIAPLAY_PLAYERTYPE_MSE;
		break;
	case OPLVOD_PLAYER_TYPE_HMX:
	default:
		apk_type = eOxMP_MEDIAPLAY_PLAYERTYPE_HMX;
		break;
	}

	return APK_MEDIA_PLAY_SetPlayerType(m_dest, apk_type) == ERR_OK;
}


bool OplVideoOnDemand::probe()
{
	ENTRY;

	HERROR eRet = ERR_OK;

	HxLOG_Debug("OplVideoOnDemand::probe: play_state=%d, play_action=%d, url=%s\n", \
			m_play_state, m_play_action, m_url.c_str());

	if ( m_is_pvr
		|| m_url.c_str() == NULL
		|| m_play_action == VOD_ACTION_PROBE
		|| m_play_action == VOD_ACTION_PLAY )
	{
		return true;
	}

	setBufferingMode(false, 10000);

	this->m_is_download_vod = true;
	m_download_id = startDownload();

	HxLOG_Debug("OplVideoOnDemand::probe:download id(%d), url=%s\n", m_download_id, m_url.c_str());

	setDownloadState(OPLVOD_DOWNLOAD_STATE_STARTED);

	m_play_action = VOD_ACTION_PROBE;

	EXIT;

	return eRet == ERR_OK;
}

bool OplVideoOnDemand::play()
{
	HERROR eRet = ERR_OK;

	ENTRY;

	HxLOG_Debug("OplVideoOnDemand::play: play_state=%d, play_action=%d\n", m_play_state, m_play_action);
	if ( m_play_action == VOD_ACTION_PLAY ) return true;

	HUINT32 ulViewId = m_dest;
	HUINT32 ulSessionId = 0;
	OxMediaPlay_StartInfo_t unStart;
	HxSTD_memset( &unStart, 0x00, sizeof(OxMediaPlay_StartInfo_t) );

	if ( m_is_pvr )
	{
		unStart.stPvrPb.eType = eOxMP_MEDIATYPE_PVRPLAYBACK;
		unStart.stPvrPb.ulContentId = APK_META_RECORD_GetCID( const_cast<char *>(m_url.c_str()) );
		unStart.stPvrPb.ulStartTime = 0;

		eRet = APK_MEDIA_PLAY_Start (ulViewId, &unStart, &ulSessionId);
		if ( eRet != ERR_OK )
		{
			HxLOG_Error ("[%s:%d] Error> APK_MEDIA_PLAY_Start err: [ViewId=%u, SessionId=%u]\n", __FUNCTION__, __LINE__, ulViewId, ulSessionId);
		}
	}
	else
	{
		unStart.stMediaPlay.eType = eOxMP_MEDIATYPE_MEDIAPLAY;
#if defined(CONFIG_TIMESTAMP_64)
		unStart.stMediaPlay.ullStartTime = 0;
#else
		unStart.stMediaPlay.ulStartTime = 0;
#endif
		HxSTD_StrNCpy( unStart.stMediaPlay.szUrl, m_url.c_str(), sizeof(unStart.stMediaPlay.szUrl)-1 );
		eRet = APK_MEDIA_PLAY_Start (ulViewId, &unStart, &ulSessionId);
		if ( eRet != ERR_OK )
		{
			HxLOG_Error ("[%s:%d] Error> APK_MEDIA_PLAY_Start err: [ViewId=%u, SessionId=%u]\n", __FUNCTION__, __LINE__, ulViewId, ulSessionId);
		}
	}

	m_play_action = VOD_ACTION_PLAY;
	m_session_id = ulSessionId;
	m_is_active = true;

	touch();

	EXIT;

	return eRet == ERR_OK;
}

bool OplVideoOnDemand::play(OplVideoOnDemandProbeInfo *probe_info)
{
	HERROR eRet = ERR_OK;
	ENTRY;

	OplVideoOnDemand *vod = OplVideoOnDemand::getActiveVod();

	if ( vod != this && vod != NULL )
	{
		eRet = APK_MEDIA_PLAY_Stop(vod->m_dest /*ulViewId*/);
		if( eRet != ERR_OK )
			HxLOG_Error("OplVideoOnDemand::play - APK_MEDIA_PLAY_Stop Error \n");

		vod->m_is_active = false;
		vod->m_play_action = VOD_ACTION_PROBE;
	}

	HxLOG_Debug("OplVideoOnDemand::play: play_state=%d, play_action=%d, download_state=%s, last_pos(%d), duration(%d), url(%s)\n",
			m_play_state, m_play_action, getDownloadStateString(getDownloadState()), m_last_pos, m_duration, getDownloadUrl());

	if ( m_play_action == VOD_ACTION_PLAY ) return true;

	m_is_active = true;

	if ( getDownloadState() != OPLVOD_DOWNLOAD_STATE_PROBE_COMPLETED
		&& getDownloadState() != OPLVOD_DOWNLOAD_STATE_CREATE )
	{
		return true;
	}

	HUINT32 					 ulViewId, ulSessionId;
	OxMediaPlay_StartInfo_t 	 unStart;
	HERROR						 hErr;

	memset(&unStart, 0x00, sizeof(OxMediaPlay_StartInfo_t));

	ulViewId = m_dest;
	unStart.stMediaPlay.eType			= eOxMP_MEDIATYPE_MEDIAPLAY;
#if defined(CONFIG_TIMESTAMP_64)
	unStart.stMediaPlay.ullStartTime 	= m_last_pos;
#else
	unStart.stMediaPlay.ulStartTime 	= m_last_pos;
#endif
	unStart.stMediaPlay.ulAudioPid 		= probe_info->audio_pid;
	unStart.stMediaPlay.ulVideoPid 		= probe_info->video_pid;
	unStart.stMediaPlay.eAudioCodec 	= static_cast<DxAudioCodec_e>(__audio_codec_to_appkit(probe_info->audio_codec));
	unStart.stMediaPlay.eVideoCodec 	= static_cast<DxVideoCodec_e>(__video_codec_to_appkit(probe_info->video_codec));

	if( getDownloadState() != OPLVOD_DOWNLOAD_STATE_CREATE )
		APK_MEDIA_PLAY_Probe( m_dest /*ulViewId*/, (HCHAR *)getDownloadUrl());

	hErr = APK_MEDIA_PLAY_Start (ulViewId, &unStart, &ulSessionId);
	debugVodList();

	if (ERR_OK != hErr)
	{
		HxLOG_Error ("[%s:%d] APK_MEDIA_PLAY_Start err: ViewId=%dn", __FUNCTION__, __LINE__, ulViewId);
		eRet = hErr;
	}
		
	m_session_id = ulSessionId;

	m_play_action = VOD_ACTION_PLAY;

	return eRet == ERR_OK;
}

bool OplVideoOnDemand::stop(bool internal)
{
	HERROR eRet = ERR_OK;
	OxMediaPlay_MediaType_e eOapiType;

	ENTRY;

	if ( m_play_action == VOD_ACTION_STOP
		|| m_play_action == VOD_ACTION_INTERNAL_STOP
		|| ( m_play_state == OPLVOD_PLAY_STATE_STOPPED && internal == TRUE ))
	{
		HxLOG_Debug("OplVideoOnDemand::stop: the playback is already stopped.\n");
		m_play_action = (internal) ? VOD_ACTION_INTERNAL_STOP : VOD_ACTION_STOP;
		return true;
	}

	if( this->isPVRPlay() )
	{
		APK_MEDIA_PLAY_GetPlayType( m_dest , &eOapiType );
		if ( eOxMP_MEDIATYPE_LIVE != eOapiType )
		{
			eRet = APK_MEDIA_PLAY_Stop( m_dest /*ulViewId*/);
		}
	}
	else
	{
		APK_MEDIA_PLAY_GetPlayType( m_dest , &eOapiType );
		if ( eOxMP_MEDIATYPE_LIVE != eOapiType )
		{
			list<OplVideoOnDemand*>::iterator begin_it = s_vods.begin();
			for ( list<OplVideoOnDemand*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
			{
				if(((*it)->m_is_active == true) && (*it) == this)
				{
					eRet = APK_MEDIA_PLAY_Stop( (*it)->m_dest /*ulViewId*/);
					(*it)->m_is_active = false;
				}
			}
		}

		APK_DOWNLOAD_Stop(m_download_id);
	}

	if ( !internal )
	{
		m_play_action = VOD_ACTION_STOP;
	}
	else
	{
		m_play_action = VOD_ACTION_INTERNAL_STOP;
		HxLOG_Debug("OplVideoOnDemand::stop: internal stop is called.\n");
	}

	touch();

	EXIT;

	return eRet == ERR_OK;
}

#if defined(CONFIG_TIMESTAMP_64)
bool OplVideoOnDemand::seek(unsigned long long position)
{
	ENTRY
	HERROR eRet = ERR_OK;

	HxLOG_Debug("OplVideoOnDemand::seek: playback position will be changed to %llu.\n", position);
	if ( m_is_pvr )
		position = position / 1000;

	eRet = APK_MEDIA_PLAY_SetPlayPosition( m_dest /*ulViewId*/, position);

	touch();

	return eRet == ERR_OK;
}
#else
bool OplVideoOnDemand::seek(unsigned int position)
{
	ENTRY
	HERROR eRet = ERR_OK;

	HxLOG_Debug("OplVideoOnDemand::seek: playback position will be changed to %d.\n", position);
	if ( m_is_pvr )
		position = position / 1000;

	eRet = APK_MEDIA_PLAY_SetPlayPosition( m_dest /*ulViewId*/, position);

	touch();

	return eRet == ERR_OK;
}
#endif

bool OplVideoOnDemand::seekSegment(unsigned int pos_segment)
{
	ENTRY
	HERROR eRet = ERR_OK;

	HxLOG_Debug("OplVideoOnDemand::seekSegment: playback segment will be changed to %d.\n", pos_segment);

	eRet = APK_MEDIA_PLAY_SetPlaySegment( m_dest /*ulViewId*/, pos_segment);

	touch();

	return eRet == ERR_OK;
}

#if defined(CONFIG_TIMESTAMP_64)
unsigned long long OplVideoOnDemand::getPlayPosition()
{
	unsigned long long position = 0;

	if ( !isPlaying() )
	{
		if( m_buffer_state == OPLVOD_BUFFER_STATE_CANPLAY )
		{
			return m_start_pos;
		}
		return 0;
	}

	APK_MEDIA_PLAY_GetPlayPosition (m_dest, &position);
	return ( m_is_pvr ) ? position*1000 : position;
}

unsigned long long OplVideoOnDemand::getDuration()
{
	ENTRY
	
	if( (m_play_state == OPLVOD_PLAY_STATE_PLAYING)
		|| (m_play_state == OPLVOD_PLAY_STATE_ERROR))
	{
		unsigned long long duration = 0;

			HERROR eRet = APK_MEDIA_PLAY_GetDurationTime( m_dest, &duration);
			if ( eRet != ERR_OK ) return m_duration;

		return ( m_is_pvr ) ? duration*1000 : duration;
	}

	return m_duration;
}
#else
unsigned int OplVideoOnDemand::getPlayPosition()
{
	unsigned int position = 0;

	if ( !isPlaying() )
	{
		if( m_buffer_state == OPLVOD_BUFFER_STATE_CANPLAY )
		{
			return m_start_pos;
		}
		return 0;
	}

	APK_MEDIA_PLAY_GetPlayPosition (m_dest, &position);
	return ( m_is_pvr ) ? position*1000 : position;
}

unsigned int OplVideoOnDemand::getDuration()
{
	ENTRY
	
	if( (m_play_state == OPLVOD_PLAY_STATE_PLAYING)
		|| (m_play_state == OPLVOD_PLAY_STATE_ERROR))
	{
		unsigned int duration = 0;

			HERROR eRet = APK_MEDIA_PLAY_GetDurationTime( m_dest, &duration);
			if ( eRet != ERR_OK ) return m_duration;

		return ( m_is_pvr ) ? duration*1000 : duration;
	}

	return m_duration;
}

#endif

bool OplVideoOnDemand::getDurationInfinity()
{
	HxLOG_Debug("OplVod::getDurationInfinity: %d\n", m_duration_Infinity);
	return m_duration_Infinity;
}

void OplVideoOnDemand::setDuration(unsigned int duration)
{
	ENTRY;
	APK_MEDIA_PLAY_SetDuration(m_dest, duration);
}

int OplVideoOnDemand::getPlaySpeed()
{
	ENTRY
	int speed = INVALID_SPEED_VALUE;

	HERROR eRet = APK_MEDIA_PLAY_GetPlaySpeed( m_dest /*ulViewId*/, &speed);

	return ( eRet == ERR_OK ) ? speed : 0;
}

bool OplVideoOnDemand::setPlaySpeed(int speed_sec)
{
	HERROR eRet = ERR_OK;

	ENTRY;

	if ( !isPlaying() )
	{
		HxLOG_Debug("OplVideoOnDemand::setPlaySpeed: the playback is not started yet.\n");
		return true;
	}

	eRet = APK_MEDIA_PLAY_SetPlaySpeed( m_dest /*ulViweId*/, speed_sec);

	if ( speed_sec == 0 ) /*  pause */
		APK_MEDIA_PLAY_GetPlayPosition (m_dest, &m_last_pos );

	EXIT;

	return eRet == ERR_OK;
}

int OplVideoOnDemand::getPlaySpeedCount()
{
	ENTRY
	unsigned int count = 0;
	int speeds[MAX_SPEED_NUM] = {0, };
	HERROR eRet = ERR_OK;

	eRet = APK_MEDIA_PLAY_GetSupportedSpeeds( m_dest /*ulViewId*/, &count, MAX_SPEED_NUM, speeds);
	if ( eRet != ERR_OK ) return -1;

	return count;
}

bool OplVideoOnDemand::getPlaySpeeds(int **speeds_out)
{
	ENTRY
	unsigned int count = 0;
	int speeds[MAX_SPEED_NUM] = {0, };
	HERROR eRet = ERR_OK;

	eRet = APK_MEDIA_PLAY_GetSupportedSpeeds( m_dest /*ulViewId*/, &count, MAX_SPEED_NUM, speeds);

	if ( eRet != ERR_OK ) return false;

	*speeds_out = new int[count];
	if ( !*speeds_out ) return false;

	if ( count >= MAX_SPEED_NUM ) count = MAX_SPEED_NUM;

	for ( unsigned int i=0; i<count; ++i )
		(*speeds_out)[i] = speeds[i]/100;

	return true;
}

void OplVideoOnDemand::setPlayState(eOplVideoOnDemandPlayState state)
{
	ENTRY;
	m_play_state = state;

	if( getActiveVod() == NULL ) return;

	switch ( m_play_state )
	{
	case OPLVOD_PLAY_STATE_ERROR:
		if ( getActiveVod()->getPlayError() != OPLVOD_PLAY_ERROR_NOT_AVAILABLE_POSITION )
		{
			m_url = "";
		}
		break;
	case OPLVOD_PLAY_STATE_STOPPED:
		{
			m_is_active = false;
		}
		break;
	default:
		break;
	}
}

unsigned int OplVideoOnDemand::getBufferingInfo(unsigned  int **start_times, unsigned int **end_times)
{
	ENTRY
	if ( m_is_pvr ) return 0;

	unsigned int chunk_count = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetBufferingChunkInfo( m_dest /*ulViewId*/, &chunk_count, start_times, end_times);
	if ( eRet != ERR_OK )
	{
		return 0;
	}

	return chunk_count;
}

unsigned int OplVideoOnDemand::getBufferingRemainedTime()
{
	ENTRY
	unsigned int remained_time = 0;

	HERROR eRet = APK_MEDIA_PLAY_GetBufferingRemainedTime( m_dest /*ulViewId*/, &remained_time);
	if ( eRet != ERR_OK ) return 0;

	HxLOG_Debug("OplVideoOnDemand::getBufferingRemainedTime: %d.\n", remained_time);

	return remained_time;
}

unsigned int OplVideoOnDemand::getBufferingPercent()
{
	ENTRY
	unsigned int buffer_percent = 0;

	HERROR eRet = APK_MEDIA_PLAY_GetBufferingPercent( m_dest /*ulViewId*/, &buffer_percent);
	if ( eRet != ERR_OK ) return 0;

	HxLOG_Debug("OplVideoOnDemand::getBufferingPercent: %d.\n", buffer_percent);

	return buffer_percent;
}

bool OplVideoOnDemand::setFullScreen(bool full_screen)
{
	ENTRY
	if ( full_screen )
	{
		HERROR eRet = APK_MEDIA_PLAY_SetVideoSize(m_dest /*ulViewId*/, 0, 0, 0, 0);
		if ( eRet != ERR_OK ) return false;
	}

	m_is_fullscreen = full_screen;

	return true;
}

bool OplVideoOnDemand::setVisibility(bool bVisible)
{
    HERROR  hErr = ERR_FAIL;

    HxLOG_Critical("setVisibility resourceId=%d bVisible=%d\n", m_dest, bVisible);
    hErr = APK_MEDIA_PLAY_SetVideoHide(m_dest, !bVisible);
    if (ERR_OK != hErr)
    {
        HxLOG_Error ("[%s:%d] OPL_Broadcast_SetVisibility Err. ViewId=%d\n", __FUNCTION__, __LINE__, m_dest);
        return false;
    }
    return true;
}

bool OplVideoOnDemand::setWindow(int x, int y, int width, int height)
{
	ENTRY
	int x_tmp, y_tmp, width_tmp, height_tmp;

	if ( x < 0 || y < 0 || width < 0 || height < 0
            || x+width > OSD_WIDTH || y+height > OSD_HEIGHT )
    {
        return true;
    }

	if(((x>0) || (y>0)) && (width == OSD_WIDTH) && (height == OSD_HEIGHT))
	{
		x_tmp = x;
		y_tmp = y;
		width_tmp = width - x;
		height_tmp = height - y;
	}
	else
	{
		x_tmp = x;
		y_tmp = y;
		width_tmp = width;
		height_tmp = height;
	}

	return APK_MEDIA_PLAY_SetVideoSize( m_dest /*ulViewId*/, x_tmp, y_tmp, width_tmp, height_tmp) == ERR_OK;
}

bool OplVideoOnDemand::getVideoResolution(int *width, int *height)
{
	ENTRY
	HERROR hErr = ERR_OK;
	HINT32	lwidth = 0, lheight = 0;
	HxLOG_Trace();

	hErr = APK_MEDIA_PLAY_GetStreamVideoSize( m_dest, (HUINT32 *)&lwidth, (HUINT32 *)&lheight);
	if( hErr == ERR_OK)
	{
		if ( lwidth == 0 )
		{
			HxLOG_Error( "[%s,%d] invalid size width : %d\n", __FUNCTION__, __LINE__, lwidth );
			*width = 1280;
		}
		else
		{
			*width = lwidth;
		}

		if ( lheight == 0 )
		{
			HxLOG_Error( "[%s,%d] invalid size height : %d\n", __FUNCTION__, __LINE__, lheight );
			*height = 1280;
		}
		else
		{
			*height = lheight;
		}
	}

	return true;
}

double OplVideoOnDemand::getStartDate(void)
{
#if 0
	double start_date = 0;
	HUINT8 starttime[256] = {0,};
	struct tm _base, _time;
	time_t base, time;
	HERROR err = ERR_FAIL;

 	_base.tm_sec = 0;
    _base.tm_min = 0;
    _base.tm_hour = 0;
    _base.tm_mday = 1;
    _base.tm_mon = 0;
    _base.tm_year = 70;
    _base.tm_isdst = -1;
	_base.tm_gmtoff = 0;
	base = mktime(&_base);

	err = APK_DOWNLOAD_GetAvailabilityStartTime(m_download_id, 256, starttime);
	HxLOG_Debug("StartDate == %s", starttime);

	if( (strstr((char *)starttime, "None") != NULL ) || \
			(err == ERR_FAIL) || (starttime[0] == 0) )
	{
		HxLOG_Debug("StartDate == None");
		return -1;
	}

	strptime((char *)starttime, "%Y-%m-%dT%H:%M:%SZ", &_time);
	_time.tm_isdst = -1;
	time = mktime(&_time);
	start_date = difftime(time + _time.tm_gmtoff, base);

	return start_date * 1000.;
#else
	return 0.0;
#endif
}

// Download
int OplVideoOnDemand::startDownload(void)
{
	ENTRY
	DxDownload_Start_t arg;
	memset(&arg, 0x00, sizeof(DxDownload_Start_t));

	arg.eType = eDxDNLD_TYPE_IpMediaStream;
	arg.stIpMedia.eType = eDxDNLD_TYPE_IpMediaStream;
	arg.stIpMedia.bContentVerification = true;
	arg.stIpMedia.bMhegIcsStream = false;
	HxSTD_snprintf(arg.stIpMedia.szUrl, DxDNLD_MEDIAPLAY_URL, "%s", getUrl());

	return APK_DOWNLOAD_Start (0, &arg);
}

int OplVideoOnDemand::probeDownload(void)
{
	ENTRY
	DxDownload_Start_t arg;
	memset(&arg, 0x00, sizeof(DxDownload_Start_t));

	arg.eType = eDxDNLD_TYPE_IpMediaStream;
	arg.stIpMedia.eType = eDxDNLD_TYPE_IpMediaStream;
	arg.stIpMedia.bContentVerification = false;
	arg.stIpMedia.bMhegIcsStream = false;
	HxSTD_snprintf(arg.stIpMedia.szUrl, DxDNLD_MEDIAPLAY_URL, "%s", getUrl());

	return  APK_DOWNLOAD_Probe (&arg);
}

bool OplVideoOnDemand::stopDownload(void)
{
	ENTRY
	HERROR eRet = APK_DOWNLOAD_Stop(m_download_id);
	return eRet == ERR_OK;
}

bool OplVideoOnDemand::resumeDownload(void)
{
	ENTRY
	HERROR eRet = APK_DOWNLOAD_Resume (m_download_id);
	return eRet == ERR_OK;
}

bool OplVideoOnDemand::pauseDownload(void)
{
	ENTRY
	HERROR eRet = APK_DOWNLOAD_Pause (m_download_id);
	return eRet == ERR_OK;
}

bool OplVideoOnDemand::getDownloadStatus(OplDownloadStatus_t *status)
{
	ENTRY
	HERROR eRet = ERR_OK;
	DxDownload_Status_t _status;

	memset(&_status, 0x00, sizeof(DxDownload_Status_t));
	eRet = APK_DOWNLOAD_GetStatus (m_download_id, &_status);

	return eRet == ERR_OK;
}

#if defined(CONFIG_TIMESTAMP_64)
void OplVideoOnDemand::setDownloadInfo(unsigned int id, unsigned long long duration)
{
	ENTRY

	char tmp[OxMP_MEDIAPLAY_URL];
	memset(tmp, 0x00, OxMP_MEDIAPLAY_URL);
	HxSTD_snprintf(tmp, OxMP_MEDIAPLAY_URL, OPLVOD_DOWNLOAD_URI_FORM, id);

	m_dnld_url = tmp;
	m_duration = duration;
}
#else
void OplVideoOnDemand::setDownloadInfo(unsigned int id, unsigned duration)
{
	ENTRY

	char tmp[OxMP_MEDIAPLAY_URL];
	memset(tmp, 0x00, OxMP_MEDIAPLAY_URL);
	HxSTD_snprintf(tmp, OxMP_MEDIAPLAY_URL, OPLVOD_DOWNLOAD_URI_FORM, id);

	m_dnld_url = tmp;
	m_duration = duration;
}
#endif

const char *OplVideoOnDemand::getDownloadStateString(eOplVideoOnDemandDownloadState state)
{
	switch ( state )
	{
		case OPLVOD_DOWNLOAD_STATE_NONE:
			return "none";
		case OPLVOD_DOWNLOAD_STATE_STARTED:
			return "start";
		case OPLVOD_DOWNLOAD_STATE_PROBE_COMPLETED:
			return "probe complete";
		case OPLVOD_DOWNLOAD_STATE_PAUSE:
			return "pause";
		case OPLVOD_DOWNLOAD_STATE_COMPLETED:
			return "complete";
		case OPLVOD_DOWNLOAD_STATE_CREATE:
			return "create";
		default:
			return "unknown";
	}

	return NULL;
}

/* Subtitle */
bool OplVideoOnDemand::getSubtitleDisplay()
{
	ENTRY
	if ( m_is_pvr ) return false;

	int show = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetSubtitleEnable( m_dest/*ulViewId*/, &show);
	if ( eRet != ERR_OK ) return false;

	return ( show == 0 ) ? false : true;
}

bool OplVideoOnDemand::setSubtitleDisplay(bool display)
{
	ENTRY
	if ( m_is_pvr ) return false;

	return APK_MEDIA_PLAY_SetSubtitleEnable( m_dest/*ulViewId*/, display) == ERR_OK;
}

int OplVideoOnDemand::getSubtitleLanguageCount()
{
	ENTRY
	if ( m_is_pvr ) return false;

	unsigned int count = 0;

	HERROR eRet = APK_MEDIA_PLAY_GetComponentNum( m_dest/*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, &count);
	return ( eRet == ERR_OK ) ? count : -1;
}

int OplVideoOnDemand::getSubtitleActiveLanguageIndex()
{
	ENTRY
	if ( m_is_pvr ) return false;

	int index = 0;

	HERROR eRet =APK_MEDIA_PLAY_GetComponentIndex( m_dest/*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, &index, NULL);
	return ( eRet == ERR_OK ) ? index : -1;
}

bool OplVideoOnDemand::getSubtitleLanguage(int index, char *out_lang)
{
	ENTRY
	if ( m_is_pvr ) return false;

	OxMediaPlay_Component_t stComponet;
	HERROR hError = ERR_FAIL;

	hError = APK_MEDIA_PLAY_GetComponent( m_dest/*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, index, &stComponet);
	if (hError != ERR_OK) {
		return false;
	}

	switch( stComponet.stSubtitle.eSubtitleType )
	{
		case eOxMP_SUBTITLETYPE_Dvb:
			if (out_lang) {
				HLIB_STD_StrUtf8NCpy(out_lang, stComponet.stSubtitle.utArg.stDvbInfo.szLangCode, sizeof(stComponet.stSubtitle.utArg.stDvbInfo.szLangCode));
			}
			break;
		case eOxMP_SUBTITLETYPE_Ebu:
			if (out_lang) {
				HLIB_STD_StrUtf8NCpy(out_lang, stComponet.stSubtitle.utArg.stEbuInfo.szLangCode, sizeof(stComponet.stSubtitle.utArg.stEbuInfo.szLangCode));
			}
			break;
		case eOxMP_SUBTITLETYPE_Media:
			if (out_lang) {
				HLIB_STD_StrUtf8NCpy(out_lang, stComponet.stSubtitle.utArg.stMediaInfo.szLangCode, sizeof(stComponet.stSubtitle.utArg.stMediaInfo.szLangCode));
			}
			break;
		default:
			return false;
	}

	return true;
}

bool OplVideoOnDemand::setSubtitleLanguage(int index)
{
	ENTRY
	if ( m_is_pvr ) return false;

	return APK_MEDIA_PLAY_SetComponentIndex( m_dest/*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, index) == ERR_OK;
}

int OplVideoOnDemand::getSubtitleTextPosition()
{
	ENTRY
	if ( m_is_pvr ) return false;

	unsigned int position = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetSubtitleTextPosition( m_dest/*ulViewId*/, &position);

	return ( eRet == ERR_OK ) ? position : -1;
}

bool OplVideoOnDemand::setSubtitleTextPosition(int pos)
{
	ENTRY
	if ( m_is_pvr ) return false;

	return APK_MEDIA_PLAY_SetSubtitleTextPosition( m_dest/*ulViewId*/, pos) == ERR_OK;
}

int OplVideoOnDemand::getSubtitleFontSize()
{
	ENTRY
	if ( m_is_pvr ) return false;

	unsigned int size = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetSubtitleFontSize( m_dest/*ulViewId*/, &size);
	return ( eRet == ERR_OK ) ? size : -1;
}

bool OplVideoOnDemand::setSubtitleFontSize(int size)
{
	ENTRY
	if ( m_is_pvr ) return false;

	return APK_MEDIA_PLAY_SetSubtitleFontSize( m_dest/*ulViewId*/, size) == ERR_OK;
}

int OplVideoOnDemand::getSubtitleSyncTime()
{
	ENTRY
	if ( m_is_pvr ) return false;

	int time = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetSubtitleSyncTime( m_dest/*ulViewId*/, (unsigned int *) &time);
	return ( eRet == ERR_OK ) ? time : -1;
}

bool OplVideoOnDemand::setSubtitleSyncTime(int sync_time)
{
	ENTRY
	if ( m_is_pvr ) return false;

	return (APK_MEDIA_PLAY_SetSubtitleSyncTime( m_dest/*ulViewId*/, sync_time) == ERR_OK);
}

/* AVComponent */
eOplAVComponentType OplVideoOnDemand::getSelectedComponentType()
{
	ENTRY
	return m_seleted_comptype;
}

bool OplVideoOnDemand::setSelectedComponentType(eOplAVComponentType comp_type)
{
	ENTRY
	m_seleted_comptype = comp_type;
	return true;;
}

int OplVideoOnDemand::getSelectedComponentIndex(void)
{
	ENTRY
	return m_seleted_index;
}

bool OplVideoOnDemand::setSelectedComponentIndex(int index)
{
	ENTRY
	m_seleted_index = index;
	return true;;
}

unsigned int OplVideoOnDemand::getComponentsCount(eOplAVComponentType comp_type)
{
	ENTRY

	HERROR eRet = ERR_OK;
	unsigned int count = 0;
	
	if( m_is_pvr )
	{
		OxMediaPlay_Component_e type = eOxMP_COMPONENT_NONE;
		switch(comp_type)
		{
			case OPLAVCOMPONENT_TYPE_VIDEO: 
				type = eOxMP_COMPONENT_VIDEO; break;
			case OPLAVCOMPONENT_TYPE_AUDIO:
				type = eOxMP_COMPONENT_AUDIO; break;
			case OPLAVCOMPONENT_TYPE_SUBTITLE:
				type = eOxMP_COMPONENT_SUBTITLE; break;
			case OPLAVCOMPONENT_TYPE_SUPERIMPOSE:
			case OPLAVCOMPONENT_TYPE_DATA:
			case OPLAVCOMPONENT_TYPE_MAX:
				break;
		}
		eRet = APK_MEDIA_PLAY_GetComponentNum( m_dest, type, &count);
	}
	else
	{
		OxDownload_Component_e download_comp_type = eOxDNLD_COMPONENT_NONE;
		switch(comp_type)
		{
			case OPLAVCOMPONENT_TYPE_VIDEO:
				download_comp_type = eOxDNLD_COMPONENT_VIDEO; break;
			case OPLAVCOMPONENT_TYPE_AUDIO:
				download_comp_type = eOxDNLD_COMPONENT_AUDIO; break;
			case OPLAVCOMPONENT_TYPE_SUBTITLE:
				download_comp_type = eOxDNLD_COMPONENT_SUBTITLE; break;
			case OPLAVCOMPONENT_TYPE_SUPERIMPOSE:
				download_comp_type = eOxDNLD_COMPONENT_NONE; break;
			case OPLAVCOMPONENT_TYPE_DATA:
				download_comp_type = eOxDNLD_COMPONENT_DATA; break;
			case OPLAVCOMPONENT_TYPE_MAX:
				download_comp_type = eOxDNLD_COMPONENT_NONE; break;
			default: break;
		}
		
		eRet = APK_DOWNLOAD_GetComponentNum(m_download_id,download_comp_type,&count);
	}
	
	return ( eRet == ERR_OK ) ? count : 0;
}

bool OplVideoOnDemand::getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp)
{
	ENTRY

	HERROR hErr = ERR_FAIL;
	
	if( m_is_pvr )
	{
		switch ( comp_type )
		{
		case OPLAVCOMPONENT_TYPE_VIDEO:
			return getVideoComponent(index, out_comp);
		case OPLAVCOMPONENT_TYPE_AUDIO:
			return getAudioComponent(index, out_comp);
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			return getSubtitleComponent(index, out_comp);
		default:
			break;
		}
	}
	else
	{
		OxDownload_Component_e download_comp_type = eOxDNLD_COMPONENT_NONE;
		OxDownload_Component_t stComponet;
		switch(comp_type)
		{
			case OPLAVCOMPONENT_TYPE_VIDEO:
				download_comp_type = eOxDNLD_COMPONENT_VIDEO; break;
			case OPLAVCOMPONENT_TYPE_AUDIO:
				download_comp_type = eOxDNLD_COMPONENT_AUDIO; break;
			case OPLAVCOMPONENT_TYPE_SUBTITLE:
				download_comp_type = eOxDNLD_COMPONENT_SUBTITLE; break;
			case OPLAVCOMPONENT_TYPE_SUPERIMPOSE:
				download_comp_type = eOxDNLD_COMPONENT_NONE; break;
			case OPLAVCOMPONENT_TYPE_DATA:
				download_comp_type = eOxDNLD_COMPONENT_DATA; break;
			case OPLAVCOMPONENT_TYPE_MAX:
				download_comp_type = eOxDNLD_COMPONENT_NONE; break;
			default: break;
		}
				

		hErr = APK_DOWNLOAD_GetComponent (m_download_id, download_comp_type, index, &stComponet);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] APK_DOWNLOAD_GetComponent Error : \n", __FUNCTION__, __LINE__);
		}

		HxSTD_MemSet (out_comp, 0, sizeof(OplAVComponent_t));
		out_comp->eType = comp_type;
		
		switch ( comp_type )
		{
		case OPLAVCOMPONENT_TYPE_VIDEO:
			{
				out_comp->ulPid				=	stComponet.stVideo.nPid;
				out_comp->ulComponentTag 	=	stComponet.stVideo.nComponentTag;
				out_comp->ulStreamType		= 	stComponet.stVideo.nStreamType;
				out_comp->ulComponentType	=   (unsigned int)stComponet.stVideo.nComponentType;
				HLIB_STD_StrNCpySafe(out_comp->szText, reinterpret_cast<char *>(stComponet.stVideo.aucStreamName), OPLAVCOMPONENT_TEXT_SIZE);
			}
			break;
		case OPLAVCOMPONENT_TYPE_AUDIO:
			{
				out_comp->ulPid				=	stComponet.stAudio.nPid;
				out_comp->ulComponentTag 	=	stComponet.stAudio.nComponentTag;
				const char *encoding		= __convertAudioCodecToOplEncoding(stComponet.stAudio.eCodec);
				HLIB_STD_StrNCpySafe (out_comp->encoding, (HCHAR*)encoding, HxSTD_StrLen(encoding) + 1);

				out_comp->eSubType	= __convert_audio_to_subtype((OxMediaPlay_AudioMode_e)stComponet.stAudio.eMode);
				HLIB_STD_StrNCpySafe(out_comp->szLanguage, reinterpret_cast<char *>(stComponet.stAudio.astLangInfo[0].szLangCode), 4);

				HLIB_STD_StrNCpySafe(out_comp->compName, reinterpret_cast<char *>(stComponet.stAudio.aucCompName), OPLAVCOMPONENT_COMP_NAME);
				HLIB_STD_StrNCpySafe(out_comp->compRole, reinterpret_cast<char *>(stComponet.stAudio.aucCompRole), OPLAVCOMPONENT_COMP_ROLE);

				out_comp->bAudioDescription =  OAPI_AUDIO_IS_AD_BM_TYPE_DOWNLOAD(stComponet.stAudio.astLangInfo[0].eAudType,stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
				out_comp->bAudioDescription |= OAPI_AUDIO_IS_AD_BM_TYPE_DOWNLOAD(stComponet.stAudio.astLangInfo[1].eAudType,stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
				out_comp->bAudioDescription |= OAPI_AUDIO_IS_AD_BM_TYPE_DOWNLOAD(stComponet.stAudio.astLangInfo[0].eAudType, stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
				out_comp->bAudioDescription |= OAPI_AUDIO_IS_AD_BM_TYPE_DOWNLOAD(stComponet.stAudio.astLangInfo[1].eAudType, stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);

				HxLOG_Debug("getComponent: nPid(%d), nComponentTag(%d), encoding(%s), eSubType(%d), aucCompName(%s), aucCompRole(%s), bAudioDescription(%d)\n", 
					stComponet.stAudio.nPid, stComponet.stAudio.nComponentTag, encoding, out_comp->eSubType, 
					stComponet.stAudio.aucCompName, stComponet.stAudio.aucCompRole, out_comp->bAudioDescription);
				
			}
			break;
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			{
				out_comp->ulPid	= (HUINT32)stComponet.stSubtitle.nPid;
				if( stComponet.stSubtitle.eSubtitleType == eOxDNLD_SUBTITLETYPE_Dvb )
				{
					out_comp->eSubType = OPLAVCOMPONENT_SUBTYPE_SUBTITLE_DVB;
					out_comp->ulComponentTag = stComponet.stSubtitle.utArg.stDvbInfo.nComponentTag;
					HLIB_STD_StrNCpySafe(out_comp->szLanguage,
						reinterpret_cast<char *>(stComponet.stSubtitle.utArg.stDvbInfo.szLangCode), 4);

					out_comp->bHardOfHearing_Impaired	= IsHardOfHearingSubtitle((HUINT32)stComponet.stSubtitle.utArg.stDvbInfo.nTypeValue);
				}
				else if( stComponet.stSubtitle.eSubtitleType == eOxDNLD_SUBTITLETYPE_Ebu )
				{

					out_comp->eSubType = OPLAVCOMPONENT_SUBTYPE_SUBTITLE_EBU;
					out_comp->ulComponentTag = stComponet.stSubtitle.utArg.stEbuInfo.nComponentTag;
					HLIB_STD_StrNCpySafe(out_comp->szLanguage,
						reinterpret_cast<char *>(stComponet.stSubtitle.utArg.stEbuInfo.szLangCode), 4);
				}
			}
			break;
		default: break;
		}
	}


	return hErr;
}

bool OplVideoOnDemand::getActiveComponent(eOplAVComponentType comp_type, OplAVComponent_t *out_comp)
{
	ENTRY
	switch ( comp_type )
	{
	case OPLAVCOMPONENT_TYPE_VIDEO:
		return getActiveVideoComponent(out_comp);
	case OPLAVCOMPONENT_TYPE_AUDIO:
		return getActiveAudioComponent(out_comp);
	case OPLAVCOMPONENT_TYPE_SUBTITLE:
		return getActiveSubtitleComponent(out_comp);
	default:
		break;
	}

	return false;
}

bool OplVideoOnDemand::setActiveComponent(eOplAVComponentType comp_type, int pid, char *pszLang)
{
	ENTRY
	switch ( comp_type )
	{
	case OPLAVCOMPONENT_TYPE_VIDEO:
		return setActiveVideoComponent(pid);
	case OPLAVCOMPONENT_TYPE_AUDIO:
		return setActiveAudioComponent(pid, pszLang);
	case OPLAVCOMPONENT_TYPE_SUBTITLE:
		return setActiveSubtitleComponent(pid, pszLang);
	default:
		break;
	}

	return false;
}

bool OplVideoOnDemand::setActiveComponent(eOplAVComponentType comp_type, int index)
{
	ENTRY
	HERROR hErr = ERR_FAIL;

	switch ( comp_type )
	{
		case OPLAVCOMPONENT_TYPE_VIDEO:
			hErr = APK_MEDIA_PLAY_SetComponentIndex(m_dest, eOxMP_COMPONENT_VIDEO, index); break;
		case OPLAVCOMPONENT_TYPE_AUDIO:
			hErr = APK_MEDIA_PLAY_SetComponentIndex(m_dest, eOxMP_COMPONENT_AUDIO, index); break;
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			hErr = APK_MEDIA_PLAY_SetComponentIndex(m_dest, eOxMP_COMPONENT_SUBTITLE, index); break;
		default: break;
	}

	return hErr;
}

bool OplVideoOnDemand::unsetActiveComponent(eOplAVComponentType comp_type, int index)
{
	HERROR hErr = ERR_FAIL;

	switch ( comp_type )
	{
		case OPLAVCOMPONENT_TYPE_VIDEO:
			hErr = APK_MEDIA_PLAY_UnsetComponentIndex(m_dest, eOxMP_COMPONENT_VIDEO, index); break;
		case OPLAVCOMPONENT_TYPE_AUDIO:
			hErr = APK_MEDIA_PLAY_UnsetComponentIndex(m_dest, eOxMP_COMPONENT_AUDIO, index); break;
		case OPLAVCOMPONENT_TYPE_SUBTITLE:
			hErr = APK_MEDIA_PLAY_UnsetComponentIndex(m_dest, eOxMP_COMPONENT_SUBTITLE, index); break;
		default: break;
	}

	return hErr;
}

bool OplVideoOnDemand::offComponent(eOplAVComponentType comp_type, int pid)
{
	ENTRY
	switch ( comp_type )
	{
	case OPLAVCOMPONENT_TYPE_VIDEO:
		if ( ERR_OK == APK_MEDIA_PLAY_SetComponentIndex(m_dest, eOxMP_COMPONENT_VIDEO, -1) )
		{
		}
		break;
	case OPLAVCOMPONENT_TYPE_AUDIO:
		if ( ERR_OK == APK_MEDIA_PLAY_SetComponentIndex(m_dest, eOxMP_COMPONENT_AUDIO, -1) )
		{
		}
		break;
	case OPLAVCOMPONENT_TYPE_SUBTITLE:
		return offSubtitleComponent(pid);
	default:
		break;
	}

	return false;
}

bool OplVideoOnDemand::setSelectedComponentTypeIndex(eOplAVComponentType *comp_type, int *index)
{
	ENTRY
	bool ret = false;
	if (!s_vods.empty() )
	{
		*comp_type = getSelectedComponentType();
		*index = getSelectedComponentIndex();
	}
	return ret;
}

bool OplVideoOnDemand::getParentalRating(bool *blocked, unsigned int *rating, char **cas_id)
{
	ENTRY
	if ( !blocked || !rating || *cas_id ) return false;

	*blocked = m_parental_blocked;
	*rating = m_parental_rating_value;
	*cas_id = m_cas_system_id;

	return true;
}

void OplVideoOnDemand::setParentalRating(bool blocked, unsigned int rating, const char *cas_id)
{
	ENTRY
	m_parental_blocked = blocked;
	m_parental_rating_value = rating;
	if ( cas_id ) HxSTD_StrNCpy(m_cas_system_id, cas_id, 127);
}

bool OplVideoOnDemand::getDrmRightsError(eOplVideoOnDemandDrmRightsError *error, char **cas_id, char **content_id, char **rights_issuer_id)
{
	ENTRY
	if ( !error || *cas_id || *content_id || *rights_issuer_id) return false;
	*error = m_drm_rights_error;
	*cas_id = m_cas_system_id;
	*content_id = m_drm_content_id;
	*rights_issuer_id = m_drm_rights_issuer_url;

	return true;
}

void OplVideoOnDemand::setDrmRightsError(eOplVideoOnDemandDrmRightsError error, const char *cas_id, const char *content_id, const char *rights_issuer_id)
{
	ENTRY
	m_drm_rights_error = error;
	if ( cas_id ) HxSTD_StrNCpy(m_cas_system_id, cas_id, 127);
	if ( content_id ) HxSTD_StrNCpy(m_drm_content_id, content_id, 1023);
	if ( rights_issuer_id ) HxSTD_StrNCpy(m_drm_rights_issuer_url, rights_issuer_id, 1023);
}

/* Media Source Extension */
bool OplVideoOnDemand::setProbeInfo(OplVideoOnDemandProbeInfo *probe_info)
{
	ENTRY
	bool ret = false;

	m_probe_info.audio_pid = probe_info->audio_pid;
	m_probe_info.video_pid = probe_info->video_pid;
	m_probe_info.audio_codec = probe_info->audio_codec;
	m_probe_info.video_codec = probe_info->video_codec;

	return ret;
}

int OplVideoOnDemand::getPumpIndex()
{
	ENTRY;
	unsigned int index = 0;
	HERROR hErr = APK_MEDIA_PLAY_GetPumpIndex(m_dest, &index);
	if ( hErr != ERR_OK ) return -1;

	return index;
}

/***************************************************************************************
 * private functions
 **************************************************************************************/
void OplVideoOnDemand::touch()
{
	ENTRY
	struct timeval tv;
	gettimeofday( &tv, 0 );

	m_latest_tick = tv.tv_sec*1000 + (tv.tv_usec+500)/1000;
}

bool OplVideoOnDemand::isPlaying()
{
	HxLOG_Debug("-----> action : %d, state : %d, error : %d\n", m_play_action, m_play_state, m_play_error );
	if ( m_play_action == VOD_ACTION_NONE
		|| m_play_state == OPLVOD_PLAY_STATE_STOPPED
		|| (m_play_state == OPLVOD_PLAY_STATE_ERROR
			&& m_play_error != OPLVOD_PLAY_ERROR_NOT_AVAILABLE_POSITION)
		|| m_play_state == OPLVOD_PLAY_STATE_PROBE_FAILED
		|| m_play_state == OPLVOD_PLAY_STATE_BOF ) return false;

	return true;
}

void OplVideoOnDemand::checkVodsStatus()
{
	ENTRY
		
	OplVideoOnDemand *vod = OplVideoOnDemand::getActiveVod();
	if(vod != this && vod != NULL)
	{
		eOplVideoOnDemandPlayState state = vod->getPlayState();
		if( (state == OPLVOD_PLAY_STATE_PAUSED)
			|| (state == OPLVOD_PLAY_STATE_FINISHED))
		{
			APK_MEDIA_PLAY_Stop(vod->m_dest);
			vod->m_is_active = false;
			vod->m_play_action = VOD_ACTION_PROBE;
		}
	}
}

bool OplVideoOnDemand::getVideoComponent(int index, OplAVComponent_t *out_comp)
{
	ENTRY
	//if ( !m_is_pvr ) return false;
	if ( !out_comp ) return false;

	{
		HERROR hErr;
		HUINT32 ulViewId;
		OxMediaPlay_Component_t stComponet;

		ulViewId = m_dest;

		hErr = APK_MEDIA_PLAY_GetComponent (ulViewId, eOxMP_COMPONENT_VIDEO, index, &stComponet);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponent Error : \n", __FUNCTION__, __LINE__);
		}

		HxSTD_MemSet (out_comp, 0, sizeof(OplAVComponent_t));

		out_comp->eType = OPLAVCOMPONENT_TYPE_VIDEO;

		out_comp->ulPid				=	stComponet.stVideo.nPid;
		out_comp->ulComponentTag 	=	stComponet.stVideo.nComponentTag;
		out_comp->ulStreamType		= 	stComponet.stVideo.nStreamType;
		out_comp->ulComponentType	=   (unsigned int)stComponet.stVideo.nComponentType;
		HLIB_STD_StrNCpySafe(out_comp->szText, reinterpret_cast<char *>(stComponet.stVideo.aucStreamName),
			OPLAVCOMPONENT_TEXT_SIZE);

	}
	return true;
}

bool OplVideoOnDemand::getAudioComponent(int index, OplAVComponent_t *out_comp)
{
	ENTRY
	//if ( !m_is_pvr ) return false;
	if ( !out_comp ) return false;

	{
		HERROR hErr;
		HUINT32 ulViewId;
		OxMediaPlay_Component_t stComponet;

		ulViewId = m_dest;

		hErr = APK_MEDIA_PLAY_GetComponent (ulViewId, eOxMP_COMPONENT_AUDIO, index, &stComponet);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponent Error : \n", __FUNCTION__, __LINE__);
		}

		HxSTD_MemSet (out_comp, 0, sizeof(OplAVComponent_t));

		out_comp->eType = OPLAVCOMPONENT_TYPE_AUDIO;

		out_comp->ulPid				=	stComponet.stAudio.nPid;
		out_comp->ulComponentTag 	=	stComponet.stAudio.nComponentTag;


		const char *encoding	= __convertAudioCodecToOplEncoding(stComponet.stAudio.eCodec);
		HLIB_STD_StrNCpySafe (out_comp->encoding, (HCHAR*)encoding, HxSTD_StrLen(encoding) + 1);

		out_comp->eSubType		= __convert_audio_to_subtype(stComponet.stAudio.eMode);
		HLIB_STD_StrNCpySafe(out_comp->szLanguage, reinterpret_cast<char *>(stComponet.stAudio.astLangInfo[0].szLangCode), 4);

		out_comp->bAudioDescription =  OAPI_AUDIO_IS_AD_BM_TYPE(stComponet.stAudio.astLangInfo[0].eAudType,stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
		out_comp->bAudioDescription |= OAPI_AUDIO_IS_AD_BM_TYPE(stComponet.stAudio.astLangInfo[1].eAudType,stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
		out_comp->bAudioDescription |= OAPI_AUDIO_IS_AD_RM_TYPE(stComponet.stAudio.astLangInfo[0].eAudType, stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);
		out_comp->bAudioDescription |= OAPI_AUDIO_IS_AD_RM_TYPE(stComponet.stAudio.astLangInfo[1].eAudType, stComponet.stAudio.ucMixType, stComponet.stAudio.eSupAudType);

	}

	return true;
}

bool OplVideoOnDemand::getSubtitleComponent(int index, OplAVComponent_t *out_comp)
{
	ENTRY
	//if ( !m_is_pvr ) return false;
	if ( !out_comp ) return false;

	{
		HERROR hErr;
		HUINT32 ulViewId;
		OxMediaPlay_Component_t stComponet;

		ulViewId = m_dest;

		hErr = APK_MEDIA_PLAY_GetComponent (ulViewId, eOxMP_COMPONENT_SUBTITLE, index, &stComponet);
		if( ERR_OK != hErr )
		{
			HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_GetComponent Error : \n", __FUNCTION__, __LINE__);
		}

		HxSTD_MemSet (out_comp, 0, sizeof(OplAVComponent_t));

		out_comp->eType	= OPLAVCOMPONENT_TYPE_SUBTITLE;
		out_comp->ulPid	= (HUINT32)stComponet.stSubtitle.nPid;

		switch ( stComponet.stSubtitle.eSubtitleType)
		{
		case eOxMP_SUBTITLETYPE_Dvb:
			out_comp->eSubType = OPLAVCOMPONENT_SUBTYPE_SUBTITLE_DVB;
			out_comp->ulComponentTag = stComponet.stSubtitle.utArg.stDvbInfo.nComponentTag;
			HLIB_STD_StrNCpySafe(out_comp->szLanguage,
				reinterpret_cast<char *>(stComponet.stSubtitle.utArg.stDvbInfo.szLangCode), 4);

			out_comp->bHardOfHearing_Impaired	= IsHardOfHearingSubtitle((HUINT32)stComponet.stSubtitle.utArg.stDvbInfo.nTypeValue);
			break;

		case eOxMP_SUBTITLETYPE_Ebu:
			out_comp->eSubType = OPLAVCOMPONENT_SUBTYPE_SUBTITLE_EBU;
			out_comp->ulComponentTag = stComponet.stSubtitle.utArg.stEbuInfo.nComponentTag;
			HLIB_STD_StrNCpySafe(out_comp->szLanguage,
				reinterpret_cast<char *>(stComponet.stSubtitle.utArg.stEbuInfo.szLangCode), 4);
			break;

		default:
			return false;
		}
	}

	return true;
}

bool OplVideoOnDemand::getActiveVideoComponent(OplAVComponent_t *out_comp)
{
	ENTRY
	//if ( !m_is_pvr ) return false;
	if ( !out_comp ) return false;

	int index = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_VIDEO, &index, NULL);
	if ( eRet != ERR_OK ) return false;

	return getVideoComponent(index, out_comp);
}

bool OplVideoOnDemand::getActiveAudioComponent(OplAVComponent_t *out_comp)
{
	ENTRY
	//if ( !m_is_pvr ) return false;
	if ( !out_comp ) return false;

	int index = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_AUDIO, &index, NULL);
	if ( eRet != ERR_OK ) return false;

	return getAudioComponent(index, out_comp);
}

bool OplVideoOnDemand::getActiveSubtitleComponent(OplAVComponent_t *out_comp)
{
	ENTRY
	//if ( !m_is_pvr ) return false;
	if ( !out_comp ) return false;

	int index = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, &index, NULL);
	if ( eRet != ERR_OK || index == -1 ) return false;

	return getSubtitleComponent(index, out_comp);
}

int OplVideoOnDemand::getActiveComponentIndex(eOplAVComponentType comp_type)
{
	ENTRY
	int index = -1;
	HERROR ret = ERR_FAIL;

	if( m_is_pvr )
	{
		switch(comp_type)
		{
			case OPLAVCOMPONENT_TYPE_VIDEO:
				{
					ret = APK_MEDIA_PLAY_GetComponentIndex(m_dest, eOxMP_COMPONENT_VIDEO, &index, NULL);
				}
				break;
			case OPLAVCOMPONENT_TYPE_AUDIO:
				{
					ret = APK_MEDIA_PLAY_GetComponentIndex(m_dest, eOxMP_COMPONENT_AUDIO, &index, NULL);
				}
				break;
			case OPLAVCOMPONENT_TYPE_SUBTITLE:
				{
					ret = APK_MEDIA_PLAY_GetComponentIndex(m_dest, eOxMP_COMPONENT_SUBTITLE, &index, NULL);
				}
				break;
			default: break;
		}
	}
	else
	{
		switch(comp_type)
		{
			case OPLAVCOMPONENT_TYPE_VIDEO:
				{
					ret = APK_DOWNLOAD_GetComponentIndex(m_download_id, eOxDNLD_COMPONENT_VIDEO, &index, NULL);
				}
				break;
			case OPLAVCOMPONENT_TYPE_AUDIO:
				{
					ret = APK_DOWNLOAD_GetComponentIndex(m_download_id, eOxDNLD_COMPONENT_AUDIO, &index, NULL);
				}
				break;
			case OPLAVCOMPONENT_TYPE_SUBTITLE:
				{
					ret = APK_DOWNLOAD_GetComponentIndex(m_download_id, eOxDNLD_COMPONENT_SUBTITLE, &index, NULL);
				}
				break;
			default: break;	
		}
	}

	if( ret != ERR_OK ) index = -1;

	return index;
}

bool OplVideoOnDemand::setActiveVideoComponent(int pid)
{
	ENTRY
	//if ( !m_is_pvr ) return false;

	unsigned int count = 0;
	HERROR hErr;
	hErr = APK_MEDIA_PLAY_GetComponentNum (m_dest /*ulViewId*/, eOxMP_COMPONENT_VIDEO, &count);
	if ( hErr != ERR_OK || count == 0 ) return false;

	for ( unsigned int i=0; i<count; ++i )
	{
		OxMediaPlay_Component_t stComponet;

		hErr = APK_MEDIA_PLAY_GetComponent (m_dest /*ulViewId*/, eOxMP_COMPONENT_VIDEO, i, &stComponet);
		if ( hErr != ERR_OK ) continue;
		if ( stComponet.stVideo.nPid == pid )
		{
			hErr = APK_MEDIA_PLAY_SetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_VIDEO, i);
			if ( hErr != ERR_OK ) continue;
			return true;
		}
	}
	return false;
}

bool OplVideoOnDemand::setActiveAudioComponent(int pid, char* pszLang)
{
	ENTRY
	//if ( !m_is_pvr ) return false;

	unsigned int count = 0;

	HERROR hErr;
	hErr = APK_MEDIA_PLAY_GetComponentNum (m_dest /*ulViewId*/, eOxMP_COMPONENT_AUDIO, &count);
	if ( hErr != ERR_OK || count == 0 ) return false;

	for ( unsigned int i=0; i<count; ++i )
	{
		OxMediaPlay_Component_t stComponet;

		hErr = APK_MEDIA_PLAY_GetComponent (m_dest /*ulViewId*/, eOxMP_COMPONENT_AUDIO, i, &stComponet);
		if ( hErr != ERR_OK ) continue;
		if ( stComponet.stAudio.nPid == pid ) // !!! �� �߰� �ʿ� pszLang
		{
			hErr = APK_MEDIA_PLAY_SetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_AUDIO, i);
			if ( hErr != ERR_OK ) continue;
			return true;
		}
	}
	return false;
}

bool OplVideoOnDemand::setActiveSubtitleComponent(int pid, char* pszLang)
{
	ENTRY
	//if ( !m_is_pvr ) return false;

	unsigned int count = 0;
	HERROR hErr;
	hErr = APK_MEDIA_PLAY_GetComponentNum (m_dest /*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, &count);
	if ( hErr != ERR_OK || count == 0 ) return false;

	for ( unsigned int i=0; i<count; ++i )
	{
		OxMediaPlay_Component_t stComponet;

		hErr = APK_MEDIA_PLAY_GetComponent (m_dest /*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, i, &stComponet);
		if ( hErr != ERR_OK ) continue;
		if ( stComponet.stSubtitle.nPid == pid ) // !!! �� �߰� �ʿ� pszLang
		{
			if( NULL != pszLang )
			{
				if (eOxMP_SUBTITLETYPE_Dvb==stComponet.stSubtitle.eSubtitleType)
				{
					if(HxSTD_StrNCmp((HCHAR*)stComponet.stSubtitle.utArg.stDvbInfo.szLangCode, (HCHAR*)pszLang, 3) != 0)
						continue;
				}
				else if (eOxMP_SUBTITLETYPE_Ebu==stComponet.stSubtitle.eSubtitleType)
				{
					if(HxSTD_StrNCmp((HCHAR*)stComponet.stSubtitle.utArg.stEbuInfo.szLangCode, (HCHAR*)pszLang, 3) != 0)
						continue;
				}
			}

			hErr = APK_MEDIA_PLAY_SetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, i);
			if ( hErr != ERR_OK ) continue;

			return true;
		}
	}
	return false;
}

bool OplVideoOnDemand::offSubtitleComponent(int pid)
{
	ENTRY
	HERROR					 hErr;
	//if ( !m_is_pvr ) return false;

	hErr = APK_MEDIA_PLAY_SetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, -1);
	if(ERR_OK == hErr)
	{
		HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", 0);

		return true;
	}
	return true;
}

bool OplVideoOnDemand::setDrmLicenseData(const char *data)
{
	ENTRY
	return APK_MEDIA_PLAY_SetDrmLicenseData( m_dest/*ulViewId*/, data) == ERR_OK;
}

void OplVideoOnDemand::callback(eOplVideoOnDemandEventType event_type)
{
#if 0	/* this callback is not needed anymore due to VRM */
	if ( m_callback ) m_callback(this, event_type);
#endif

	if ( m_listener )
	{
		m_listener(m_player, this, event_type);
	}
}

bool OplVideoOnDemand::setBufferingMode(bool bauto, int threshold_seconds)
{
	ENTRY
	return APK_MEDIA_PLAY_SeBufferingScenario( m_dest/*ulViewId*/, bauto, threshold_seconds) == ERR_OK;
}

bool OplVideoOnDemand::isValidEvent(eOplVideoOnDemandPlayState state)
{
	if ( !m_is_pvr )
	{
		HxLOG_Debug("OplVideoOnDemand::isValidEvent: previous=%s, current=%s\n",
			OplVideoOnDemand::getPlayStateString(m_play_state), OplVideoOnDemand::getPlayStateString(state));
		if ( m_play_state == state )
		{
			HxLOG_Debug("OplVideoOnDemand:: duplicated event is skipped.\n");
			return false;
		}
	}

	switch ( state )
	{
	case OPLVOD_PLAY_STATE_STOPPED:
		if ( m_play_action != VOD_ACTION_STOP )
		{
			HxLOG_Debug("OplVideoOnDemand:: internal stop event is skipped.\n");
			return false;
		}
		break;
	default:
		break;
	}

	return true;
}

/***************************************************************************************
 * static interfaces
 **************************************************************************************/
bool OplVideoOnDemand::getResourceMap(OplVideoOnDemandResourceMap *resource_map)
{
	ENTRY
	if ( resource_map )
	{
		OxMediaPlay_ResourceMap_t map;
		HERROR hErr = APK_MEDIA_PLAY_GetResourceMap(&map);
		if ( hErr != ERR_OK ) return false;

		resource_map->num_of_playpump = map.ulNumOfPlaypump;
		resource_map->playpump_list = map.pulPlaypumpList;
		resource_map->num_of_dma = map.ulNumOfDma;
		resource_map->dma_list = map.pulDmaList;
        resource_map->ulAudioMixerHandle = map.ulAudioMixerHandle;
	}
	else
	{
		return APK_MEDIA_PLAY_GetResourceMap(NULL) == ERR_OK;
	}

	return true;
}

OplVideoOnDemand *OplVideoOnDemand::getActiveVod()
{
	ENTRY
	if ( s_vods.empty() ) return NULL;

	list<OplVideoOnDemand*>::iterator begin_it = s_vods.begin();
	OplVideoOnDemand *active_vod = (*begin_it);
	
	for ( list<OplVideoOnDemand*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
	{
		//if ( (*it)->m_latest_tick >= active_vod->m_latest_tick )
		if( (*it)->m_is_active == true)
		{
			active_vod = (*it);
			return active_vod;
		}
	}

	return NULL;
}

OplVideoOnDemand *OplVideoOnDemand::findVodByDownloadid(const unsigned int id)
{
	if ( s_vods.empty() ) return NULL;

	list<OplVideoOnDemand*>::iterator begin_it = s_vods.begin();
	for ( list<OplVideoOnDemand*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
	{
		OplVideoOnDemand *vod = static_cast <OplVideoOnDemand *> (*it);
		if ( vod->m_download_id == id )	return vod;
	}

	return NULL;
}


OplVideoOnDemand *OplVideoOnDemand::findVodByOapiSession(const unsigned int id)
{
	if ( s_vods.empty() ) return NULL;

	list<OplVideoOnDemand*>::iterator begin_it = s_vods.begin();
	OplVideoOnDemand *active_vod = NULL;//(*begin_it);
	for ( list<OplVideoOnDemand*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
	{
		if ( (*it)->m_session_id == id )
			active_vod = (*it);
	}

	return active_vod;
}

const char *OplVideoOnDemand::getPlayStateString(eOplVideoOnDemandPlayState state)
{
	switch ( state )
	{
	case OPLVOD_PLAY_STATE_STOPPED:
		return "stopped";
	case OPLVOD_PLAY_STATE_PLAYING:
		return "playing";
	case OPLVOD_PLAY_STATE_PAUSED:
		return "paused";
	case OPLVOD_PLAY_STATE_CONNECTING:
		return "connecting";
	case OPLVOD_PLAY_STATE_BUFFERING:
		return "buffering";
	case OPLVOD_PLAY_STATE_FINISHED:
		return "finished(endofstream)";
	case OPLVOD_PLAY_STATE_ERROR:
		return "error";
	default:
		return "unknown";
	}

	return NULL;
}

void OplVideoOnDemand::printVodInfo()
{
	if ( s_vods.empty() ) return ;

	list<OplVideoOnDemand*>::iterator begin_it = s_vods.begin();
	OplVideoOnDemand *vod = (*begin_it);
	int cnt = 0;
	for ( list<OplVideoOnDemand*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
	{
		vod = (*it);
		printf("%d > [%s] > %s \n",  ++cnt, vod->getUrl(), vod->m_is_active?"v":" ");
	}
}

void  OplVideoOnDemand::debugVodList(void){

	list<OplVideoOnDemand*>::iterator begin_it = s_vods.begin();
	HxLOG_Debug( "***************************************** VOD LIST ************************************\n");
	for ( list<OplVideoOnDemand*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
	{
		OplVideoOnDemand *vod = static_cast <OplVideoOnDemand *> (*it);
		if( NULL != vod )
		{
			HxLOG_Debug( "ACTIVE[%d] URL(%s)\n", vod->m_is_active, vod->getUrl());
		}
	}
	HxLOG_Debug( "***************************************************************************************\n");
}

static void vod_proc(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData)
{
	if ( OplVideoOnDemand::s_vods.empty() ) return;

	OplVideoOnDemand *vod = OplVideoOnDemand::findVodByOapiSession(session);
	if ( (vod == NULL) || (vod != OplVideoOnDemand::getActiveVod()) ) return;

	eOplVideoOnDemandEventType event_type = OPLVOD_EVENT_TYPE_UNKWON;

	switch ( eEvent )
	{
	case eOxMP_EVENT_STARTED:
		return;

	case eOxMP_EVENT_PLAYTIME:
		event_type = OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED;
		break;
	case eOxMP_EVENT_PLAYDURATION:
		{
#if defined(CONFIG_TIMESTAMP_64)
			unsigned long long duration_changed = pstNotifyData->stEventNotify.ullParam1;
			bool bInfinity = (bool)pstNotifyData->stEventNotify.ullParam2;
			vod->m_duration_Infinity = bInfinity;
			HxLOG_Error("vod_proc >> Duration Changed (%llu[0x%X]), bInfinity : %d \n", duration_changed, duration_changed, bInfinity);            
#else
			unsigned long long duration_changed = pstNotifyData->stEventNotify.ulParam1;
			bool bInfinity = (bool)pstNotifyData->stEventNotify.ulParam2;
			vod->m_duration_Infinity = bInfinity;
			HxLOG_Error("vod_proc >> Duration Changed (%lu[0x%X]), bInfinity : %d \n", duration_changed, duration_changed, bInfinity);            
#endif
			event_type = OPLVOD_EVENT_TYPE_PLAY_DURATION_CHANGED;
		}
		break;
	case eOxMP_EVENT_MEDIA_PlayInfo:
		break;

	case eOxMP_EVENT_PLAYSTATE:
	{
		eOplVideoOnDemandPlayState state = __convert_play_state(pstNotifyData->eStateChanged);

		if ( !vod->isValidEvent(state) ) return;

		HxLOG_Debug("OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED: %s ==> %s\n",
				OplVideoOnDemand::getPlayStateString(vod->getPlayState()), OplVideoOnDemand::getPlayStateString(state));

		vod->setPlayState(state);

		event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
	}
		break;

	case eOxMP_EVENT_PLAYERROR:
	{
		OxMediaPlay_PlayError_e appkit_error;
		OxMediaPlay_ErrorReason_e peReason;
		eOplVideoOnDemandPlayError error_type = OPLVOD_PLAY_ERROR_NO_ERROR;
		APK_MEDIA_PLAY_GetPlayError( ulViewId, &appkit_error, &peReason);
		error_type = __convert_play_error(appkit_error);
		if( (error_type == OPLVOD_PLAY_ERROR_NOT_SUPPORTED_FORMAT)
			|| (error_type == OPLVOD_PLAY_ERROR_INSUFFICIENT_RESOURCE))
		{
			switch(peReason){
#if 0
				case eOxMP_ERROR_REASON_DATA_MALFORMED_GENERIC:
					error_type = OPLVOD_PLAY_ERROR_CONTENT_CORRUPT; break;

				case eOxMP_ERROR_REASON_SERVER_HTTP_STATUS_NOT_FOUND:
					error_type = OPLVOD_PLAY_ERROR_SERVER_FAIL; break;

				case eOxMP_ERROR_REASON_NETWORK_UNREACHABLE:
					error_type = OPLVOD_PLAY_ERROR_SERVER_FAIL; break;

				case eOxMP_ERROR_REASON_SERVER_HTTP_STATUS_SERVER_ERROR:
					error_type = OPLVOD_PLAY_ERROR_SERVER_FAIL; break;
#endif
				default:
					error_type = OPLVOD_PLAY_ERROR_NOT_SUPPORTED_FORMAT; break;
			}
		}

		vod->setPlayError(error_type);
		vod->setPlayState(OPLVOD_PLAY_STATE_ERROR);
		HxLOG_Error("OPLVOD_PLAY_STATE_ERROR: reason=%d\n", error_type);

		event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
	}
		break;

	case eOxMP_EVENT_MEDIA_BufferStateChanged:
	{
		event_type = OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED;

		OxMediaPlay_BufferState_e appkit_state;
		APK_MEDIA_PLAY_GetBufferState( ulViewId, &appkit_state);

		eOplVideoOnDemandBufferState state = __convert_buffer_state(appkit_state);
		if ( state == OPLVOD_BUFFER_STATE_NONE || ( vod->m_is_download_vod && (state == OPLVOD_BUFFER_STATE_CANPLAY) ) ) return;

		vod->setBufferState(state);
	}
		break;

	case eOxMP_EVENT_MEDIA_BufferInfoChanged:
		event_type = OPLVOD_EVENT_TYPE_BUFFER_INFO_CHANGED;
		break;

	case eOxMP_EVENT_VIDEOSTREAMINFO:
		event_type = OPLVOD_EVENT_TYPE_VIDEOWINDOW_CHANGED;
		break;

	case eOxMP_EVENT_STOPPED:
        /* the stop event should be notifed only when the application calls stop(). */
        if ( !vod->isValidEvent(OPLVOD_PLAY_STATE_STOPPED) ) return;

		event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
		vod->setPlayState(OPLVOD_PLAY_STATE_STOPPED);
		break;

	case eOxMP_EVENT_SPEED_CHANGED:
		event_type = OPLVOD_EVENT_TYPE_PLAY_SPEED_CHANGED;
		break;

	case eOxMP_EVENT_SupportedSpeedChanged:
		event_type = OPLVOD_EVENT_TYPE_SUPPORTED_PLAY_SPEED_CHANGED;
		break;

	case eOxMP_EVENT_SI_SIT_CHANGED:
		event_type = OPLVOD_EVENT_TYPE_PROGRAMME_CHANGED;
		break;

	case eOxMP_EVENT_MEDIA_PumpIndexChanged:
	printf("############################################################\n");
	printf("OPLVOD_EVENT_TYPE_PUMP_INDEX_CHANGED\n");
	printf("############################################################\n");
		event_type = OPLVOD_EVENT_TYPE_PUMP_INDEX_CHANGED;
		break;

	case eOxMP_EVENT_MEDIA_SeekReady:
		event_type = OPLVOD_EVENT_TYPE_SEEK_READY;
		break;
	case eOxMP_EVENT_DRM_BLOCK:
	{
		if( NULL == pstNotifyData )
			break;

		unsigned int	error_status	= pstNotifyData->stDrmRightsError.eErrorState;

		eOplVideoOnDemandDrmRightsError error = OPLVOD_DRM_RIGHTS_NO_LICENSE;
		switch ( error_status )
		{
			case eOxMP_DRMERROR_ValidLicense:
				error = OPLVOD_DRM_RIGHTS_VALID_LICENSE;
				break;
			case eOxMP_DRMERROR_InvalidLicense:
				error = OPLVOD_DRM_RIGHTS_INVALID_LICENSE;
				break;
			case eOxMP_DRMERROR_NoLicense:
			default:
				error = OPLVOD_DRM_RIGHTS_NO_LICENSE;
				break;
		}
		vod->setDrmRightsError(error, pstNotifyData->stDrmRightsError.szDRMSystemID, pstNotifyData->stDrmRightsError.szContentID, pstNotifyData->stDrmRightsError.szRightsIssuerURL );

		HxLOG_Debug("Got DRM Changed event: error_status=%d\n", error_status);

		event_type = OPLVOD_EVENT_TYPE_DRM_RIGHT_ERROR;
	}
		break;

	case eOxMP_EVENT_EOF:
		{
			if( !vod->m_queue_url.empty() )
			{
				string queue_url = vod->m_queue_url.front();
				vod->setUrl(queue_url.c_str());
				vod->m_queue_url.remove(queue_url);
				vod->probe();
				vod->play();
			}
		}
		break;
	case eOxMP_EVENT_COMPONENT_SELECTED:
	case eOxMP_EVENT_COMPONENT_UNSELECTED:
		{
#if defined(CONFIG_TIMESTAMP_64)
			OxMediaPlay_Component_e oapi_type = (OxMediaPlay_Component_e)pstNotifyData->stEventNotify.ullParam1;
#else
			OxMediaPlay_Component_e oapi_type = (OxMediaPlay_Component_e)pstNotifyData->stEventNotify.ulParam1;
#endif
			eOplAVComponentType opl_type = OPLAVCOMPONENT_TYPE_MAX;

			if( oapi_type == eOxMP_COMPONENT_VIDEO )
			{
				opl_type = OPLAVCOMPONENT_TYPE_VIDEO;
			}
			else if( oapi_type == eOxMP_COMPONENT_AUDIO )
			{
				opl_type = OPLAVCOMPONENT_TYPE_AUDIO;
			}
			else if( oapi_type == eOxMP_COMPONENT_SUBTITLE )
			{
				opl_type = OPLAVCOMPONENT_TYPE_SUBTITLE;
			}

			vod->setSelectedComponentType(opl_type);
#if defined(CONFIG_TIMESTAMP_64)
			vod->setSelectedComponentIndex((int)pstNotifyData->stEventNotify.ullParam2);
#else
			vod->setSelectedComponentIndex((int)pstNotifyData->stEventNotify.ulParam2);
#endif

#if 1
			if( eEvent == eOxMP_EVENT_COMPONENT_UNSELECTED )
			{
#if defined(CONFIG_TIMESTAMP_64)
				HxLOG_Debug("eOxMP_EVENT_COMPONENT_UNSELECTED : component type -> %d, index -> %d \n", opl_type, (int)pstNotifyData->stEventNotify.ullParam2);
#else
				HxLOG_Debug("eOxMP_EVENT_COMPONENT_UNSELECTED : component type -> %d, index -> %d \n", opl_type, (int)pstNotifyData->stEventNotify.ulParam2);
#endif
				event_type = OPLVOD_EVENT_TYPE_UNSELECTED_COMPONENT_CHANGED;
			}
			else
			{
#if defined(CONFIG_TIMESTAMP_64)
				HxLOG_Debug("eOxMP_EVENT_COMPONENT_SELECTED : component type -> %d, index -> %d \n", opl_type, (int)pstNotifyData->stEventNotify.ullParam2);
#else
				HxLOG_Debug("eOxMP_EVENT_COMPONENT_SELECTED : component type -> %d, index -> %d \n", opl_type, (int)pstNotifyData->stEventNotify.ulParam2);
#endif
				event_type = OPLVOD_EVENT_TYPE_SELECTED_COMPONENT_CHANGED;
			}
#else
			HxLOG_Debug("eOxMP_EVENT_COMPONENT_SELECTED : component type -> %d, index -> %d \n", opl_type, (int)pstNotifyData->stEventNotify.ullParam2);
			event_type = OPLVOD_EVENT_TYPE_SELECTED_COMPONENT_CHANGED;
#endif
		}
		break;
	default:
		return;
	}

	if(event_type != OPLVOD_EVENT_TYPE_UNKWON)
		vod->callback(event_type);
}

static void pvr_proc(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData)
{
	eOplVideoOnDemandEventType event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
	unsigned int prev_session = 0x0;

	if ( OplVideoOnDemand::s_vods.empty() ) return;

	OplVideoOnDemand *vod = OplVideoOnDemand::findVodByOapiSession(session);
	//OplVideoOnDemand *vod = OplVideoOnDemand::getActiveVod();
	if ( !vod ) return;

	prev_session = vod->getSessionID();

	switch ( eEvent )
	{
	case eOxMP_EVENT_STARTED:
		return;

	case eOxMP_EVENT_AV_STARTED:
		vod->setPlayState(OPLVOD_PLAY_STATE_PLAYING);
		event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
		break;

	case eOxMP_EVENT_STOPPED:
		/* Fixed #83275, #83274, #83273 : Compare sessionID to prevent current mediapb stop()
		   when receiving event/error caused by previous PVR or Live playback stop() */
		if (prev_session && (prev_session != session))
		{
			HxLOG_Error("OplVideoOnDemand::pvr_proc: Error> unmatched sessionID(%d, prev:%d] for [ViewId:%d, eEvent:%d]\n", session, prev_session, ulViewId, eEvent);
			return;
		}

		/* the stop event should be notifed only when the application calls stop(). */
		if ( vod->isValidEvent(OPLVOD_PLAY_STATE_STOPPED) )
		{
			event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
			vod->setPlayState(OPLVOD_PLAY_STATE_STOPPED);
		}
		else
		{
	    	event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
			vod->setPlayState(OPLVOD_PLAY_STATE_ERROR);
		}

		break;

	case eOxMP_EVENT_PLAYERROR:
	{
		/* Fixed #83275, #83274, #83273 : Compare sessionID to prevent current mediapb stop()
		   when receiving event/error caused by previous PVR or Live playback stop() */
		if (prev_session && (prev_session != session))
		{
			HxLOG_Error("OplVideoOnDemand::pvr_proc: Error> unmatched sessionID(%d, prev:%d] for [ViewId:%d, eEvent:%d]\n", session, prev_session, ulViewId, eEvent);
			return;
		}

		vod->setPlayState(OPLVOD_PLAY_STATE_ERROR);

		OxMediaPlay_PlayError_e appkit_error;
		APK_MEDIA_PLAY_GetPlayError( ulViewId, &appkit_error, NULL);

		eOplVideoOnDemandPlayError error = __convert_play_error(appkit_error);
		vod->setPlayError(error);

		event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
	}
		break;

	case eOxMP_EVENT_BOF:
		/* HMS1000S, IR4000HD : SPEED 100 , After EOF*/
#if 1
		vod->setPlaySpeed(100);
#else
		vod->setPlaySpeed(0);
#endif
		return;

	case eOxMP_EVENT_EOF:
		event_type = OPLVOD_EVENT_TYPE_PLAY_SPEED_CHANGED;
		vod->callback(event_type);
		event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
		vod->setPlayState(OPLVOD_PLAY_STATE_FINISHED);

		break;

	case eOxMP_EVENT_PLAYTIME:
		event_type = OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED;
		break;

	case eOxMP_EVENT_SupportedSpeedChanged:
		event_type = OPLVOD_EVENT_TYPE_SUPPORTED_PLAY_SPEED_CHANGED;
		break;

	case eOxMP_EVENT_COMPONENT_SELECTED:

#if defined(CONFIG_TIMESTAMP_64)
		if (eOxMP_COMPONENT_SUBTITLE == (OxMediaPlay_Component_e)pstNotifyData->stEventNotify.ullParam1)
#else
		if (eOxMP_COMPONENT_SUBTITLE == (OxMediaPlay_Component_e)pstNotifyData->stEventNotify.ulParam1)
#endif
		{
			{
#if defined(CONFIG_TIMESTAMP_64)
				HINT32	nTrackIndex = (HINT32)pstNotifyData->stEventNotify.ullParam2;
#else
				HINT32	nTrackIndex = (HINT32)pstNotifyData->stEventNotify.ulParam2;
#endif
				if (nTrackIndex >= 0)
				{
					OxMediaPlay_Component_t stComponent;

					if (APK_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_SUBTITLE, nTrackIndex, &stComponent) == ERR_OK)
					{
						HAPI_BroadcastSignal("signal:onSubtitleTypeChanged", 1, "i", stComponent.stSubtitle.eSubtitleType);
						HAPI_BroadcastSignal("signal:onSubtitleState", TRUE, "i", 1);
					}
				}
			}

		}

		event_type = OPLVOD_EVENT_TYPE_SELECTED_COMPONENT_CHANGED;
		break;

	case eOxMP_EVENT_SPEED_CHANGED:
		if ( vod->getPlayState() == OPLVOD_PLAY_STATE_PLAYING
				&& vod->getPlaySpeed() == 0 )
		{
			event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
			vod->setPlayState(OPLVOD_PLAY_STATE_PAUSED);
			vod->callback(event_type);
		}
		else if ( vod->getPlayState() == OPLVOD_PLAY_STATE_PAUSED )
		{
			event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
			vod->setPlayState(OPLVOD_PLAY_STATE_PLAYING);
			vod->callback(event_type);
		}

		event_type = OPLVOD_EVENT_TYPE_PLAY_SPEED_CHANGED;
		break;

	/*
	case eMctrlEvent_SI_PmtChanged:
		event_type = OPLVOD_EVENT_TYPE_PROGRAMME_CHANGED;
		break;
	*/
	case eOxMP_EVENT_SI_SIT_CHANGED:
		event_type = OPLVOD_EVENT_TYPE_PROGRAMME_CHANGED;
		break;

	case eOxMP_EVENT_PinCtrlChanged:
	{
		if( NULL == pstNotifyData )
			break;

#if defined(CONFIG_TIMESTAMP_64)
		bool 			blocked	= (bool)pstNotifyData->stEventNotify.ullParam1;
		unsigned int 	rating	= (unsigned int)pstNotifyData->stEventNotify.ullParam2;
		unsigned int 	cas_id	= (unsigned int)pstNotifyData->stEventNotify.ullParam3;
#else
		bool 			blocked	= pstNotifyData->stEventNotify.ulParam1;
		unsigned int 	rating	= pstNotifyData->stEventNotify.ulParam2;
		unsigned int 	cas_id	= pstNotifyData->stEventNotify.ulParam3;
#endif

		char sys_id[128] = {0,};
		HxSTD_snprintf(sys_id, sizeof(sys_id), "urn:dvb:casystemid:%d", cas_id);
		vod->setParentalRating(blocked, rating, sys_id);

		event_type = OPLVOD_EVENT_TYPE_PARENTAL_RATING_CHANGE;
	}
		break;

	case eOxMP_EVENT_DRM_BLOCK:
	{
		if( NULL == pstNotifyData )
			break;

		unsigned int	error_status	= pstNotifyData->stDrmRightsError.eErrorState;

		eOplVideoOnDemandDrmRightsError error = OPLVOD_DRM_RIGHTS_NO_LICENSE;
		switch ( error_status )
		{
			case eOxMP_DRMERROR_ValidLicense:
				error = OPLVOD_DRM_RIGHTS_VALID_LICENSE;
				break;
			case eOxMP_DRMERROR_InvalidLicense:
				error = OPLVOD_DRM_RIGHTS_INVALID_LICENSE;
				break;
			case eOxMP_DRMERROR_NoLicense:
			default:
				error = OPLVOD_DRM_RIGHTS_NO_LICENSE;
				break;
		}
		vod->setDrmRightsError(error, pstNotifyData->stDrmRightsError.szDRMSystemID, pstNotifyData->stDrmRightsError.szContentID, pstNotifyData->stDrmRightsError.szRightsIssuerURL );

		HxLOG_Debug("Got DRM Changed event: error_status=%d\n", error_status);

		event_type = OPLVOD_EVENT_TYPE_DRM_RIGHT_ERROR;
	}
		break;

	case eOxMP_EVENT_TrickRestrictModeChanged:
	{
		event_type = 	OPLVOD_EVENT_TYPE_TRICK_RESTRICT_MODE_CHANGED;
	}
		break;

	default:
		return;
	}

	vod->callback(event_type);
}

static HBOOL oplvod_eventFilter (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated)
{
	*updated = FALSE;

	if ((eOxMP_MEDIATYPE_MEDIAPLAY == eType) || (eOxMP_MEDIATYPE_PVRPLAYBACK == eType))
	{
		*updated = TRUE;
	}

	return TRUE;
}


static void oplvod_proc ( unsigned int ulViewId, unsigned int ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData )
{
	switch( eType )
	{
		case eOxMP_MEDIATYPE_PVRPLAYBACK:
		{
			pvr_proc(ulViewId, ulSessionId,	eEvent, pstNotifyData);
			break;
		}
		case eOxMP_MEDIATYPE_MEDIAPLAY:
		{
			vod_proc(ulViewId, ulSessionId,	eEvent, pstNotifyData);
			break;
		}
		case eOxMP_MEDIATYPE_LIVE:
		default :
			break;
	}
}

static void oplvod_download_proc(HUINT32 ulDownloadId, DxDownload_Type_e eType, DxDownload_Event_e eEvent, DxDownload_NotifierData_t *punNotifyData, void *pvUserData)
{
	if ( OplVideoOnDemand::s_vods.empty() ) return;

	OplVideoOnDemand *vod = OplVideoOnDemand::findVodByDownloadid(ulDownloadId);

	if ( !vod ) return;

	switch( eEvent )
	{
		case eDxDNLD_EVENT_Started:
			break;

		case eDxDNLD_EVENT_Completed:
			HxLOG_Debug("OplVideoOnDemand::oplvod_download_proc - download completed (url=%s, id=%d)  \n", vod->getUrl(), ulDownloadId);
			break;

		case eDxDNLD_EVENT_Failed:
			break;

		case eDxDNLD_EVENT_DataReceiving:
			break;

		case eDxDNLD_EVENT_ProbeSuccess:
			{
				HxLOG_Debug("OplVideoOnDemand::oplvod_download_proc - download probe success (url=%s, id=%d)  \n", vod->getUrl(), ulDownloadId);
#if defined(CONFIG_TIMESTAMP_64)
				HxLOG_Debug("-- info : cso_session_id=%d, duration=%d, bitrate=%d \n", \
						punNotifyData->stDnldSessionInfo.ulSessionId, punNotifyData->stDnldSessionInfo.ullDuration, punNotifyData->stDnldSessionInfo.ulMaxBitrate );
#else
				HxLOG_Debug("-- info : cso_session_id=%d, duration=%d, bitrate=%d \n", \
						punNotifyData->stDnldSessionInfo.ulSessionId, punNotifyData->stDnldSessionInfo.ulDuration, punNotifyData->stDnldSessionInfo.ulMaxBitrate );
#endif

#if defined(CONFIG_TIMESTAMP_64)
				vod->setDownloadInfo(punNotifyData->stDnldSessionInfo.ulSessionId, punNotifyData->stDnldSessionInfo.ullDuration);
#else
				vod->setDownloadInfo(punNotifyData->stDnldSessionInfo.ulSessionId, punNotifyData->stDnldSessionInfo.ulDuration);
#endif
				HxLOG_Debug("vod->getDownloadState() : %d \n", vod->getDownloadState());
				switch(vod->getDownloadState()){
					case OPLVOD_DOWNLOAD_STATE_PAUSE:
						{
							vod->setDownloadState(OPLVOD_DOWNLOAD_STATE_PROBE_COMPLETED);
							if( vod->m_is_active ) {
								OplVideoOnDemandProbeInfo info;
								memset(&info, 0x00, sizeof(OplVideoOnDemandProbeInfo));
								HxLOG_Debug("OplVideoOnDemand::DownloadProbeSuccess -  %s play retry.\n", vod->getUrl() );
								vod->play(&info);
							}
						}
						break;
					case OPLVOD_DOWNLOAD_STATE_STARTED:
						{
							vod->m_duration_Infinity = punNotifyData->stDnldSessionInfo.bInfinity;
							vod->setDownloadState(OPLVOD_DOWNLOAD_STATE_PROBE_COMPLETED);
							vod->setBufferState(OPLVOD_BUFFER_STATE_CANPLAY);
							vod->callback(OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED);
							//vod->setStartPos(punNotifyData->stDnldSessionInfo.ulStartPos);
							vod->callback(OPLVOD_EVENT_TYPE_PLAY_POSITION_CHANGED);
						}
						break;

					default: break;
				}

//				debugVodList();
			}
			break;
		case eDxDNLD_EVENT_ProbeFailed:
			{
				vod->setPlayError(OPLVOD_PLAY_ERROR_SERVER_FAIL);
				vod->setPlayState(OPLVOD_PLAY_STATE_ERROR);
				vod->callback(OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED);
			}
			break;
		case eDxDNLD_EVENT_DownloadInfoChanged:
			{
			// ���� Download ���� bufferring ������ ���Ǵ� app �� ���?�÷����� �ʵ���, 
			// ���Ŀ� �ʿ��� ��쿡��?�÷��ִ� �������� ( �� ������ ��Ƽ ������Ʈ�� ����ϴ�?��쿡��?�ش� �� ) 
			/*
				eOplVideoOnDemandPlayState play_state = vod->getPlayState();
				if( (play_state != OPLVOD_PLAY_STATE_PLAYING)
					|| (play_state != OPLVOD_PLAY_STATE_FINISHED)
					|| (play_state != OPLVOD_PLAY_STATE_PAUSED)
					|| (play_state != OPLVOD_PLAY_STATE_STOPPED))
				{
					vod->callback(OPLVOD_EVENT_TYPE_BUFFER_INFO_CHANGED);
				}
				*/
			}
			
			break;
		default:
			break;
	}
}
