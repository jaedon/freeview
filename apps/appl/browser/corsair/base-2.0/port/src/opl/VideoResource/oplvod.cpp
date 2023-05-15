/*************************************************************************************************************
	File 		: oplvod.cpp
	author 		: IPM tipmsw1@humaxdigital.com
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
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/
#include "bpldb.h"

#include "oplvod.h"
#include "oplvod_extapp.h"
#include "oplvideoutil.h"
#include "bplglobal.h"

#include "hlib.h"
#include "oapi.h"
#include <apk.h>
#include "hapi.h"

#include <iostream>
#include <sys/time.h>


#define INVALID_SPEED_VALUE 		0xFFFF
#define OPLVOD_PVR_URL_PREFEX		"HPVR://"
#define MAX_SPEED_NUM	24

#define	IsHardOfHearingSubtitle(type)			(((type) == 0x05) || (0x20 <= (type) && (type) <= 0x24))

#define ENTRY	HxLOG_Debug("[OplVideoOnDemand][%s:%d]++\n", __FUNCTION__, __LINE__);
#define EXIT	HxLOG_Debug("[OplVideoOnDemand][%s:%d]--\n", __FUNCTION__, __LINE__);


using namespace std;

	static HBOOL oplvod_eventFilter (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated);
	static void oplvod_proc ( unsigned int ulViewId, unsigned int ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData );
	static void pvr_proc(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData);
	static void vod_proc(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData);

// WIDGET_LOCAL_MEDIA
string OplVod::m_application_url;
string OplVod::m_useragent;


static eOplVodPlayState __convert_play_state(OxMediaPlay_PlayState_e state)
{
	eOplVodPlayState oipfVodState;

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

static eOplVodPlayError __convert_play_error(OxMediaPlay_PlayError_e error)
{
	eOplVodPlayError		 eOipfError;

	//printf("-----------------__convert_play_error [%d]\n", error);
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

static eOplVodBufferState __convert_buffer_state(OxMediaPlay_BufferState_e state)
{
	eOplVodBufferState bufferState;

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


/****************************************************************************************
 * OplVod class
 ***************************************************************************************/
std::list<OplVod *> OplVod::s_vods;
bool OplVod::s_auto_mode = true;

OplVod::OplVod(int aVideoResourceId)
	:
	m_callback(NULL),
	m_listener(NULL),
	m_is_fullscreen(false),
	m_is_pvr(false),
	m_play_state(OPLVOD_PLAY_STATE_STOPPED),
	m_buffer_state(OPLVOD_BUFFER_STATE_NONE),
	m_play_error(OPLVOD_PLAY_ERROR_NO_ERROR),
	m_play_errorDetails(OPLVOD_PLAY_ERRORDETAILS_UNKNOWN_ERROR),
	m_dest(0),
	m_user_data(NULL),
	m_latest_tick(0),
	m_parental_blocked(false),
	m_parental_rating_value(0),
	m_play_action(VOD_ACTION_STOP),
	m_session_id(0),
	m_is_active(false),
	m_is_download_vod(false),
	m_loop(0),
	m_seleted_index(0),
	m_start_pos(0),
	m_duration_Infinity(0),
	m_is_buffered(0)
{
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
	}

	s_vods.push_back(this);

	HxLOG_Debug("OplVod object is created. (handle=%p, total=%d\n", this, s_vods.size());
}

OplVod::~OplVod()
{
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

			hErr = APK_MEDIA_PLAY_UnregisterNotifier(ulViewId, oplvod_proc, NULL);
			if( ERR_OK != hErr )
				HxLOG_Error("[%s:%d] APK_MEDIA_PLAY_UnregisterNotifier Error: \n", __FUNCTION__, __LINE__);
		}
	}

	if(!m_queue_url.empty())
		m_queue_url.clear();

	HxLOG_Debug("OplVod object is destroyed. (total=%d\n", this, s_vods.size());
}

bool OplVod::isEmptyVod(void)
{
	if ( !s_auto_mode )
	{
		return false;
	}
	else
	{
		if ( s_vods.size() == 1 )
		{
			return true;
		}
	}
	return false;
}

int OplVod::getResourceId()
{
	return m_dest;
}

const char *OplVod::getUrl()
{
	return m_url.c_str();
}

void OplVod::setUrl(const char *url, bool is_new_url)
{
	ENTRY;

	if ( url )
	{
		string __url = url;

		if( HxSTD_StrNCmp(url, "widget://", 9) == 0 )
		{
			char temp[512];
			char * position = (char *)url + 9;
			memset( temp, 0, 512);
			HxSTD_StrNCat((char*)temp, (char*)OplVod::m_application_url.c_str(), 511);
            //modify prevent issue
            if(HxSTD_StrStr(position, "/") != NULL){
				HxSTD_StrNCat((char*)temp, HxSTD_StrStr(position, "/"), (511 - strlen(temp)));
            }
			position = HxSTD_StrStr(temp, "://") + 3;
			if ( position )
			{
				HLIB_STD_StrReplace(position, position, "//", "/");
			}
			__url = temp;
		}

#if 1
		// DTV Labs FVP v0.10
		{
			const char *is_url = strstr(url, "://");
			if(is_url == NULL){
				string new_url = OplVod::m_application_url;
				std::size_t found = new_url.find_last_of("/");
				if(found){
					new_url.erase(found+1);
					new_url.append(url);
				}

				__url = new_url;
				printf ( "[%s:%s:%d] url :: %s  \n", __FILE__, __FUNCTION__, __LINE__, new_url.c_str() );
			}
		}
#endif

		if ( m_url == __url )
		{
			HxLOG_Debug("OplVod::setUrl: duplicated url.\n");
			return;
		}

        if( is_new_url ) stop(true);

		m_url = __url;

		m_is_pvr = (HxSTD_StrNCmp(m_url.c_str(), OPLVOD_PVR_URL_PREFEX,
					HxSTD_StrLen(OPLVOD_PVR_URL_PREFEX)) == 0) ? true : false;

		HxLOG_Debug("OplVod::setUrl: url=%s\n", m_url.c_str());

		m_loop = 0;

		touch();
	}

	EXIT;
}

const char *OplVod::getMimeType()
{
	return m_mime_type.c_str();
}

void OplVod::setMimeType(const char *mime_type)
{
	if ( mime_type )
	{
		m_mime_type = mime_type;
	}
}

void OplVod::setListener(OplVodListener_t listener)
{
	if ( listener ) m_callback = listener;
}

void OplVod::setListener(IMediaPlayerListener* aListener)
{
	m_listener = aListener;
}

bool OplVod::isPVRPlay()
{
	return m_is_pvr;
}

bool OplVod::isFullScreen()
{
	return m_is_fullscreen;
}

bool OplVod::setFullScreen(bool full_screen)
{
	if ( full_screen )
	{
		HERROR eRet = APK_MEDIA_PLAY_SetVideoSize(m_dest /*ulViewId*/, 0, 0, 0, 0);
		if ( eRet != ERR_OK ) return false;
	}

	m_is_fullscreen = full_screen;

	return true;
}

bool OplVod::setVideoPosition(int x, int y, int width, int height)
{
    int adjusted_x = 0, adjusted_y = 0, adjusted_w = 0, adjusted_h = 0;
	int input_x = 0, input_y = 0, input_w = 0, input_h = 0;
    int output_x = 0, output_y = 0, output_w = 0, output_h = 0;
    int video_x = 0, video_y = 0, video_w = 0, video_h = 0;
//    int aspect_ratio = 1;
    float _float_video_ratio = 0; /* this is for replace video_ratio value as float */
    DxAspectRatio_e video_ratio = eDxASPECTRATIO_UNKNOWN;

    HxLOG_Print("[%s,%d] Requested position x %d y %d w %d h %d \n", __FUNCTION__, __LINE__, x, y, width, height );

    if ( x > OSD_WIDTH || y > OSD_HEIGHT || width == 0 || height == 0 )
    {
        HxLOG_Error("Over range request, this shall be ignored. !!!!\n");
        return true;
    }

    if ( x == 0 && y == 0 && width == OSD_WIDTH && height == OSD_HEIGHT )
    {
        HxLOG_Print("Maybe User want to show it as fullscreen !!!!\n");
        HERROR eRet = APK_MEDIA_PLAY_SetVideoSize(m_dest /*ulViewId*/, 0, 0, 0, 0);
		if ( eRet != ERR_OK )
		{
		    HxLOG_Error("APK_MEDIA_PLAY_SetVideoSize called as fullscreen but ret Error!!!!\n");
		    return false;
		}
        return true;
    }

    if (ERR_OK != APK_MEDIA_PLAY_GetStreamVideoSize(m_dest, (unsigned int*)&video_w, (unsigned int*)&video_h))
    {
        HxLOG_Error("APK_MEDIA_PLAY_GetVideoSize ret Error!!!!\n");
    }

    if ( video_w == 0 )
        video_w = OSD_WIDTH;
    if ( video_h == 0 )
        video_h = OSD_HEIGHT;

    if (ERR_OK != APK_MEDIA_PLAY_GetStreamAspectRatio(m_dest, &video_ratio))
    {
        HxLOG_Error("APK_MEDIA_PLAY_GetStreamAspectRatio ret Error!!!!\n");
        return false;
    }

    switch( video_ratio )
    {
         case eDxASPECTRATIO_4X3:
            _float_video_ratio = (float) 4/ (float) 3;
            break;

         case eDxASPECTRATIO_16X9:
            _float_video_ratio = (float) 16/ (float) 9;
            break;

         case eDxASPECTRATIO_14X9:
            _float_video_ratio = (float) 14/ (float) 9;
            break;

         case eDxASPECTRATIO_1X1:
            _float_video_ratio = (float) 1;
            break;

         default:
            break;
    }

    OplVideoUtil_getFittedPosition( x, y, width, height, \
                                    _float_video_ratio,
                                    video_x, video_y, video_w, video_h, \
                                    &adjusted_x, &adjusted_y, &adjusted_w, &adjusted_h );
    OplVideoUtil_getVideoOutputWindow( adjusted_x, adjusted_y, adjusted_w, adjusted_h,
                                       &output_x, &output_y, &output_w, &output_h );
    OplVideoUtil_getVideoInputWindow( adjusted_x, adjusted_y, adjusted_w, adjusted_h, \
                                      video_x, video_y, video_w, video_h,
                                      &input_x, &input_y, &input_w, &input_h );

    HxLOG_Print("[%s,%d] Video size x %d y %d w %d h %d \n", __FUNCTION__, __LINE__, video_x, video_y, video_w, video_h );
    HxLOG_Print("[%s,%d] Video ratio %s\n", __FUNCTION__, __LINE__, \
                                       (eDxASPECTRATIO_16X9 == video_ratio) ? "16:9" : \
                                       (( eDxASPECTRATIO_4X3 == video_ratio ) ? "4:3" : "unknow") );
    HxLOG_Print("[%s,%d] VideoOutputWindow x %d y %d w %d h %d \n", __FUNCTION__, __LINE__, output_x, output_y, output_w, output_h );
    HxLOG_Print("[%s,%d] VideoInputWindow x %d y %d w %d h %d \n", __FUNCTION__, __LINE__, input_x, input_y, input_w, input_h );
    HxLOG_Print("[%s,%d] Request to Platform\n", __FUNCTION__, __LINE__);

    return APK_MEDIA_PLAY_SetPigRect( m_dest, 0,
                                      input_x, input_y, input_w, input_h, video_w, video_h,
                                      output_x, output_y, output_w, output_h, OSD_WIDTH, OSD_HEIGHT,
                                      eDxASPECTRATIO_16X9, /* osd ratio, shall be 16:9 */
                                      eDxDFC_SELECT_UNKNOWN, /* video output status like full/letterbox/pillerbox */
                                      video_ratio );/* shall keep the information of video resource */
}

bool OplVod::getMediaWindow(int *width, int *height)
{
	HERROR hErr = ERR_OK;
	HINT32	x =0, y =0;
	HxLOG_Trace();

	if( ( width == NULL ) || ( height == NULL ))
	{
		HxLOG_Error( "[%s,%d] argument is NULL !! \n" , __FUNCTION__, __LINE__ );
		return false;
	}

	hErr = APK_MEDIA_PLAY_GetVideoSize( m_dest, (HINT32 *)&x, (HINT32 *)&y, (HINT32 *)width, (HINT32 *)height );
	if( hErr == ERR_OK)
	{
		if ( x < 0 || y < 0 || *width < 0 || *height < 0
				|| x+*width > OSD_WIDTH || y+*height > OSD_HEIGHT )
		{
			HxLOG_Error( "[%s,%d] invalid scope[x : %d, y : %d, w : %d , h : %d] \n" , __FUNCTION__, __LINE__, x, y, *width, *height );
			return false;
		}

		HxLOG_Print("[%s,%d] x : %d, y : %d, w : %d , h : %d \n", __FUNCTION__, __LINE__, x, y, *width, *height );
	}

	return true;
}

bool OplVod::getVideoResolution(int *width, int *height)
{
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

bool OplVod::probe()
{
	HERROR eRet = ERR_OK;

	ENTRY;

	HxLOG_Debug("OplVod::probe: play_state=%d, play_action=%d\n", m_play_state, m_play_action);
	HxLOG_Debug("OplVod::probe: url=%s\n", m_url.c_str());

	if ( m_is_pvr
		|| m_url.c_str() == NULL
		|| m_play_action == VOD_ACTION_PROBE
		|| m_play_action == VOD_ACTION_PLAY )
	{
		return true;
	}

	__apply_scenario();

	eRet = APK_MEDIA_PLAY_Probe( m_dest /*ulViewId*/, const_cast<char *>(m_url.c_str()));

	m_play_action = VOD_ACTION_PROBE;

	m_is_active = true;

	touch();

	EXIT;

	return eRet == ERR_OK;
}

bool OplVod::play()
{
	HERROR eRet = ERR_OK;

	ENTRY;

	HxLOG_Debug("OplVod::play: play_state=%d, play_action=%d\n", m_play_state, m_play_action);
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

bool OplVod::stop(bool internal)
{
	HERROR eRet = ERR_OK;
	OxMediaPlay_MediaType_e eOapiType;

	ENTRY;

	if ( m_play_action == VOD_ACTION_STOP
		|| m_play_action == VOD_ACTION_INTERNAL_STOP
		|| ( m_play_state == OPLVOD_PLAY_STATE_STOPPED && internal == TRUE ))
	{
		HxLOG_Debug("OplVod::stop: the playback is already stopped.\n");
		m_play_action = (internal) ? VOD_ACTION_INTERNAL_STOP : VOD_ACTION_STOP;
		return true;
	}

	APK_MEDIA_PLAY_GetPlayType( m_dest , &eOapiType );
	if ( eOxMP_MEDIATYPE_LIVE != eOapiType )
	{
		eRet = APK_MEDIA_PLAY_Stop( m_dest /*ulViewId*/);
	}

	if ( !internal )
	{
		/* workaround for Obigo Applications: invalid handling: stopped event */
		if ( OplVodExtApp::stayMusicBaebleApp() || OplVodExtApp::stayItalianFoodApp()
				|| OplVodExtApp::stayAbsoluteRadioApp() )
		{
			HxLOG_Debug("OplVod::stop: Music Bable bug fix (workaround).\n");
			setPlayState( OPLVOD_PLAY_STATE_STOPPED );
		}

		m_play_action = VOD_ACTION_STOP;
	}
	else
	{
		m_play_action = VOD_ACTION_INTERNAL_STOP;
		HxLOG_Debug("OplVod::stop: internal stop is called.\n");
	}

	touch();

	EXIT;

	return eRet == ERR_OK;
}

#if defined(CONFIG_TIMESTAMP_64)
bool OplVod::seek(unsigned long long position)
{
	HERROR eRet = ERR_OK;

	HxLOG_Debug("OplVod::seek: playback position will be changed to %llu.\n", position);

	if ( m_is_pvr )
		position = position / 1000;

	eRet = APK_MEDIA_PLAY_SetPlayPosition( m_dest /*ulViewId*/, position);

	touch();

	return eRet == ERR_OK;
}
#else
bool OplVod::seek(unsigned int position)
{
	HERROR eRet = ERR_OK;

	HxLOG_Debug("OplVod::seek: playback position will be changed to %d.\n", position);

	if ( m_is_pvr )
		position = position / 1000;

	eRet = APK_MEDIA_PLAY_SetPlayPosition( m_dest /*ulViewId*/, position);

	touch();

	return eRet == ERR_OK;
}
#endif

bool OplVod::seekSegment(unsigned int pos_segment)
{
	HERROR eRet = ERR_OK;

	HxLOG_Debug("OplVod::seekSegment: playback segment will be changed to %d.\n", pos_segment);

	eRet = APK_MEDIA_PLAY_SetPlaySegment( m_dest /*ulViewId*/, pos_segment);

	touch();

	return eRet == ERR_OK;
}

#if defined(CONFIG_TIMESTAMP_64)
static bool s_error = false;
unsigned long long OplVod::getPlayPosition()
{
	unsigned long long position = 0;
//	HERROR eRet;

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
unsigned long long OplVod::getDuration()
{
	unsigned long long duration = 0;

	HERROR eRet = APK_MEDIA_PLAY_GetDurationTime( m_dest /*ulViewId*/, &duration);
	if ( eRet != ERR_OK ) return 0;

	return ( m_is_pvr ) ? duration*1000 : duration;
}

#else
unsigned int OplVod::getPlayPosition()
{
	unsigned int position = 0;
//	HERROR eRet;

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
unsigned int OplVod::getDuration()
{
	unsigned int duration = 0;

	HERROR eRet = APK_MEDIA_PLAY_GetDurationTime( m_dest /*ulViewId*/, &duration);
	if ( eRet != ERR_OK ) return 0;

	return ( m_is_pvr ) ? duration*1000 : duration;
}
#endif

bool OplVod::getDurationInfinity()
{
	HxLOG_Debug("OplVod::getDurationInfinity: %d\n", m_duration_Infinity);
	return m_duration_Infinity;
}

int OplVod::getPlaySpeed()
{
	int speed = INVALID_SPEED_VALUE;

	HERROR eRet = APK_MEDIA_PLAY_GetPlaySpeed( m_dest /*ulViewId*/, &speed);

	return ( eRet == ERR_OK ) ? speed : 0;
}

bool OplVod::setPlaySpeed(int speed_sec)
{
	HERROR eRet = ERR_OK;

	ENTRY;
// om_media_play ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ç¾ï¿½ play ï¿½ï¿½ï¿½ï¿½ ï¿½Æ´ï¿½ ï¿½ï¿½ì¿¡ï¿½ï¿?
/*
	if ( !isPlaying() )
	{
		touch();
		HxLOG_Debug("OplVod::setPlaySpeed: the playback is not started yet.\n");
		return true;
	}
*/
	eRet = APK_MEDIA_PLAY_SetPlaySpeed( m_dest /*ulViweId*/, speed_sec);

	touch();

	EXIT;

	return eRet == ERR_OK;
}

unsigned int OplVod::getSessionID()
{
	return m_session_id;
}

eOplVodPlayState OplVod::getPlayState()
{
	return m_play_state;
}

void OplVod::setPlayState(eOplVodPlayState state)
{
	m_play_state = state;

	if( getActiveVod() == NULL ) return;

	switch ( m_play_state )
	{
		case OPLVOD_PLAY_STATE_ERROR:
		case OPLVOD_PLAY_STATE_FINISHED:
			if(OPLVOD_PLAY_ERROR_NOT_AVAILABLE_POSITION == getActiveVod()->getPlayError())
			{
			}
			else
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

	touch();
}

eOplVodPlayError OplVod::getPlayError()
{
	return m_play_error;
}

void OplVod::setPlayError(eOplVodPlayError error)
{
	touch();
	m_play_error = error;
}

eOplVodBufferState OplVod::getBufferState()
{
	return m_buffer_state;
}

void OplVod::setBufferState(eOplVodBufferState state)
{
	touch();
	m_buffer_state = state;
}

unsigned int OplVod::getBufferingInfo(unsigned  int **start_times, unsigned int **end_times)
{
	if ( m_is_pvr ) return 0;

	unsigned int chunk_count = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetBufferingChunkInfo( m_dest /*ulViewId*/, &chunk_count, start_times, end_times);
	if ( eRet != ERR_OK )
	{
		return 0;
	}

	return chunk_count;
}

unsigned int OplVod::getSeekableInfo(unsigned long long **seekable_start, unsigned long long **seekable_end)
{
	if ( m_is_pvr ) return 0;

	unsigned int chunk_count = 0;
// (KSKIM2-basemerge) appkit api °¡ ¹Ý¿µÀÌ µÇ¾î¾ß ÇÔ

	HERROR eRet = APK_MEDIA_PLAY_GetSeekableRangeInfo( m_dest, &chunk_count, seekable_start, seekable_end);
	HxLOG_Debug("OplVod::getSeekableInfo:chunk_count %d\n", chunk_count);

	if ( eRet != ERR_OK )
	{
		return 0;
	}

	return chunk_count;
}

unsigned int OplVod::getBufferingRemainedTime()
{
	unsigned int remained_time = 0;

	HERROR eRet = APK_MEDIA_PLAY_GetBufferingRemainedTime( m_dest /*ulViewId*/, &remained_time);
	if ( eRet != ERR_OK ) return 0;

	HxLOG_Debug("OplVod::getBufferingRemainedTime: %d.\n", remained_time);

	return remained_time;
}

unsigned int OplVod::getBufferingPercent()
{
	unsigned int buffer_percent = 0;

	HERROR eRet = APK_MEDIA_PLAY_GetBufferingPercent( m_dest /*ulViewId*/, &buffer_percent);
	if ( eRet != ERR_OK ) return 0;

	HxLOG_Debug("OplVod::getBufferingPercent: %d.\n", buffer_percent);

	return buffer_percent;
}

int OplVod::getPlaySpeedCount()
{
	unsigned int count = 0;
	int speeds[MAX_SPEED_NUM] = {0, };
	HERROR eRet = ERR_OK;

	eRet = APK_MEDIA_PLAY_GetSupportedSpeeds( m_dest /*ulViewId*/, &count, MAX_SPEED_NUM, speeds);
	if ( eRet != ERR_OK ) return -1;

	return count;
}

bool OplVod::getPlaySpeeds(int **speeds_out)
{
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

const char * OplVod::getPlayingThumbnail(unsigned int pos)
{
	char *thumbnail = NULL;

	//unsigned int time;
	//thumbnail = APK_MEDIA_PLAY_GetThumbnail( m_dest /*ulViewId*/, pos, &time);

	return thumbnail;
}

bool OplVod::getTrickRestrictMode(OplVodTrickRestrictMode_e *peMode)
{
	HERROR						hErr;

	unsigned int eMode;
	hErr = APK_MEDIA_PLAY_GetTrickRestrictMode( m_dest /*ulViewId*/, &eMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_MEDIA_PLAY_GetTrickRestrictMode err: ulViewId(0x%04x)\n", m_dest);
		return false;
	}
	*peMode = static_cast<OplVodTrickRestrictMode_e>(eMode);

	return true;
}

void OplVod::callback(eOplVodEventType event_type)
{
#if 0	/* this callback is not needed anymore due to VRM */
	if ( m_callback ) m_callback(this, event_type);
#endif
	if ( m_listener )
	{
		m_listener->playStateChanged(event_type);
	}
}

bool OplVod::isPlaying()
{
	if ( m_play_action == VOD_ACTION_NONE
		|| m_play_state == OPLVOD_PLAY_STATE_STOPPED
		|| (m_play_state == OPLVOD_PLAY_STATE_ERROR
			&& m_play_error != OPLVOD_PLAY_ERROR_NOT_AVAILABLE_POSITION)
		|| m_play_state == OPLVOD_PLAY_STATE_PROBE_FAILED
		|| m_play_state == OPLVOD_PLAY_STATE_BOF ) return false;

	return true;
}

bool OplVod::isValidEvent(eOplVodPlayState state)
{
	if ( !m_is_pvr )
	{
		HxLOG_Debug("OplVod::isValidEvent: previous=%s, current=%s\n",
			OplVod::getPlayStateString(m_play_state), OplVod::getPlayStateString(state));
		if ( m_play_state == state )
		{
			// (kskim2-150722) BBC Sport ÀÜ»ó ÀÌ½´·Î ÀÎÇÑ workaround
			if( m_play_state != OPLVOD_PLAY_STATE_PAUSED )
			{
			HxLOG_Debug("OplVod:: duplicated event is skipped.\n");
			return false;
		}
	}
	}

	switch ( state )
	{
	case OPLVOD_PLAY_STATE_STOPPED:
		if ( m_play_action == VOD_ACTION_INTERNAL_STOP || m_play_action == VOD_ACTION_NONE)
		{
			HxLOG_Debug("OplVod:: internal stop event is skipped.\n");
			return false;
		}
		break;
    case OPLVOD_PLAY_STATE_ERROR:
        if( isPlaying() && (m_play_action == VOD_ACTION_PLAY) )  
        {
            return true;
        }
        else
        {
            return false;
        }
	default:
		break;
	}

	return true;
}


/* Subtitle */
bool OplVod::getSubtitleDisplay()
{
	if ( m_is_pvr ) return false;

	int show = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetSubtitleEnable( m_dest/*ulViewId*/, &show);
	if ( eRet != ERR_OK ) return false;

	return ( show == 0 ) ? false : true;
}

bool OplVod::setSubtitleDisplay(bool display)
{
	if ( m_is_pvr ) return false;

	return APK_MEDIA_PLAY_SetSubtitleEnable( m_dest/*ulViewId*/, display) == ERR_OK;
}

int OplVod::getSubtitleLanguageCount()
{
	if ( m_is_pvr ) return false;

	unsigned int count = 0;

	HERROR eRet = APK_MEDIA_PLAY_GetComponentNum( m_dest/*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, &count);
	return ( eRet == ERR_OK ) ? count : -1;
}

int OplVod::getSubtitleActiveLanguageIndex()
{
	if ( m_is_pvr ) return false;

	int index = 0;

	HERROR eRet =APK_MEDIA_PLAY_GetComponentIndex( m_dest/*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, &index, NULL);
	return ( eRet == ERR_OK ) ? index : -1;
}

bool OplVod::getSubtitleLanguage(int index, char *out_lang)
{
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

bool OplVod::setSubtitleLanguage(int index)
{
	if ( m_is_pvr ) return false;

	return APK_MEDIA_PLAY_SetComponentIndex( m_dest/*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, index) == ERR_OK;
}

int OplVod::getSubtitleTextPosition()
{
	if ( m_is_pvr ) return false;

	unsigned int position = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetSubtitleTextPosition( m_dest/*ulViewId*/, &position);

	return ( eRet == ERR_OK ) ? position : -1;
}

bool OplVod::setSubtitleTextPosition(int pos)
{
	if ( m_is_pvr ) return false;

	return APK_MEDIA_PLAY_SetSubtitleTextPosition( m_dest/*ulViewId*/, pos) == ERR_OK;
}

int OplVod::getSubtitleFontSize()
{
	if ( m_is_pvr ) return false;

	unsigned int size = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetSubtitleFontSize( m_dest/*ulViewId*/, &size);
	return ( eRet == ERR_OK ) ? size : -1;
}

bool OplVod::setSubtitleFontSize(int size)
{
	if ( m_is_pvr ) return false;

	return APK_MEDIA_PLAY_SetSubtitleFontSize( m_dest/*ulViewId*/, size) == ERR_OK;
}

int OplVod::getSubtitleSyncTime()
{
	if ( m_is_pvr ) return false;

	int time = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetSubtitleSyncTime( m_dest/*ulViewId*/, (unsigned int *) &time);
	return ( eRet == ERR_OK ) ? time : -1;
}

bool OplVod::setSubtitleSyncTime(int sync_time)
{
	if ( m_is_pvr ) return false;

	return (APK_MEDIA_PLAY_SetSubtitleSyncTime( m_dest/*ulViewId*/, sync_time) == ERR_OK);
}

/* AVComponent */
eOplAVComponentType OplVod::getSelectedComponentType()
{
	return m_seleted_comptype;
}

bool OplVod::setSelectedComponentType(eOplAVComponentType comp_type)
{
	m_seleted_comptype = comp_type;
	return true;;
}

int OplVod::getSelectedComponentIndex(void)
{
	return m_seleted_index;
}

bool OplVod::setSelectedComponentIndex(int index)
{
	m_seleted_index = index;
	return true;;
}

unsigned int OplVod::getComponentsCount(eOplAVComponentType comp_type)
{
	switch ( comp_type )
	{
	case OPLAVCOMPONENT_TYPE_VIDEO:
		return getVideoComponentCount();
	case OPLAVCOMPONENT_TYPE_AUDIO:
		return getAudioComponentCount();
	case OPLAVCOMPONENT_TYPE_SUBTITLE:
		return getSubtitleComponentCount();
	default:
		break;
	}

	return 0;
}

bool OplVod::getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp)
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

	return false;
}

bool OplVod::getActiveComponent(eOplAVComponentType comp_type, OplAVComponent_t *out_comp)
{
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

bool OplVod::setActiveComponent(eOplAVComponentType comp_type, int pid, char *pszLang)
{
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

bool OplVod::setActiveComponent(eOplAVComponentType comp_type, int index)
{
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

bool OplVod::unsetActiveComponent(eOplAVComponentType comp_type, int index)
{
	HERROR hErr = ERR_FAIL;
// (KSKIM2-basemerge)  appkit api °¡ ¹Ý¿µÀÌ µÇ¾î¾ß ÇÔ

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

bool OplVod::offComponent(eOplAVComponentType comp_type, int pid)
{
	bool ret = false;
	switch ( comp_type )
	{
	case OPLAVCOMPONENT_TYPE_VIDEO:
		(void)APK_MEDIA_PLAY_SetComponentIndex(m_dest, eOxMP_COMPONENT_VIDEO, -1);
		ret = true; break;
	case OPLAVCOMPONENT_TYPE_AUDIO:
		(void)APK_MEDIA_PLAY_SetComponentIndex(m_dest, eOxMP_COMPONENT_AUDIO, -1);
		ret = true; break;
	case OPLAVCOMPONENT_TYPE_SUBTITLE:
		return offSubtitleComponent(pid);
	default:
		break;
	}

	return false;
}

bool OplVod::setSelectedComponentTypeIndex(eOplAVComponentType *comp_type, int *index)
{
	bool ret = false;
	if (!s_vods.empty() )
	{
		*comp_type = getSelectedComponentType();
		*index = getSelectedComponentIndex();
	}
	return ret;
}

void OplVod::setParentalRating(bool blocked, unsigned int rating, const char *cas_id)
{
	m_parental_blocked = blocked;
	m_parental_rating_value = rating;
	if ( cas_id ) HxSTD_StrNCpy(m_cas_system_id, cas_id, 127);
}

bool OplVod::getParentalRating(bool *blocked, unsigned int *rating, char **cas_id)
{
	if ( !blocked || !rating || *cas_id ) return false;

	*blocked = m_parental_blocked;
	*rating = m_parental_rating_value;
	*cas_id = m_cas_system_id;

	return true;
}

void OplVod::setDrmRightsError(eOplVodDrmRightsError error, const char *cas_id, const char *content_id, const char *rights_issuer_id)
{
	m_drm_rights_error = error;
	if ( cas_id ) HxSTD_StrNCpy(m_cas_system_id, cas_id, 127);
	if ( content_id ) HxSTD_StrNCpy(m_drm_content_id, content_id, 1023);
	if ( rights_issuer_id ) HxSTD_StrNCpy(m_drm_rights_issuer_url, rights_issuer_id, 1023);
}

bool OplVod::getDrmRightsError(eOplVodDrmRightsError *error, char **cas_id, char **content_id, char **rights_issuer_id)
{
	if ( !error || *cas_id || *content_id || *rights_issuer_id) return false;
	*error = m_drm_rights_error;
	*cas_id = m_cas_system_id;
	*content_id = m_drm_content_id;
	*rights_issuer_id = m_drm_rights_issuer_url;

	return true;
}

void OplVod::setStorageRight(OplVodStorageRight_e right)
{
	m_storage_right = right;
}

bool OplVod::getStorageRight(OplVodStorageRight_e *right)
{
	if ( !right ) return false;
	*right = m_storage_right;
	return true;
}

eOplVodPlayErrorDetails OplVod::getPlayErrorDetails()
{
	return m_play_errorDetails;
}

void OplVod::setPlayErrorDetails(eOplVodPlayErrorDetails error)
{
	touch();
	m_play_errorDetails = error;
}

double OplVod::getStartDate(void)
{
	double start_date = 0;
	HUINT8 starttime[256] = {0,};
	struct tm time;

	APK_MEDIA_PLAY_GetAvailabilityStartTime(m_dest, 256, starttime);

	if( strstr((char *)starttime, "None") != NULL )
	{
		HxLOG_Debug("StartDate == None");
		return 0;
	}

	HxLOG_Debug("StartDate == %s", starttime);
	strptime((char *)starttime, "%Y-%m-%dT%H:%M:%SZ", &time);

	return start_date;
}

#if defined(CONFIG_TIMESTAMP_64)
bool OplVod::setStartPos(long long int pos)
{
	m_start_pos = pos;
	return true;
}
#else
bool OplVod::setStartPos(int pos)
{
	m_start_pos = pos;
	return true;
}
#endif

/***************************************************************************************
 * private interfaces
 **************************************************************************************/
void OplVod::__apply_scenario()
{
	std::string cookie;
	if ( OplVodExtApp::stayMaxdomeApp() )
	{
		HxLOG_Debug("OplVod::__apply_scenario: this content is for Maxdome.\n");
		if ( OplVodExtApp::getMaxdomeCookie(cookie) )
		    setDrmLicenseData(cookie.c_str());
        else
		    setDrmLicenseData("");

		HxLOG_Debug("OplVod::__apply_scenario: license=%s.\n", cookie.c_str());
		setBufferingMode(true, 0);
	}
	else
	{
		setDrmLicenseData("");
		setBufferingMode(false, 10000);
	}
}

bool OplVod::setDrmLicenseData(const char *data)
{
	return APK_MEDIA_PLAY_SetDrmLicenseData( m_dest/*ulViewId*/, data) == ERR_OK;
}

bool OplVod::setBufferingMode(bool bauto, int threshold_seconds)
{
	return APK_MEDIA_PLAY_SeBufferingScenario( m_dest/*ulViewId*/, bauto, threshold_seconds) == ERR_OK;
}

bool OplVod::setPlayerType(eOplVodPlayerType type)
{
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


void OplVod::touch()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );

	m_latest_tick = tv.tv_sec*1000 + (tv.tv_usec+500)/1000;
}

unsigned int OplVod::getVideoComponentCount()
{
	//if ( !m_is_pvr ) return 0;

	unsigned int count = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentNum( m_dest /*ulViewId*/, eOxMP_COMPONENT_VIDEO, &count);
	return ( eRet == ERR_OK ) ? count : 0;
}

unsigned int OplVod::getAudioComponentCount()
{
	//if ( !m_is_pvr ) return 0;

	unsigned int count = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentNum( m_dest /*ulViewId*/, eOxMP_COMPONENT_AUDIO, &count);
	return ( eRet == ERR_OK ) ? count : 0;
}

unsigned int OplVod::getSubtitleComponentCount()
{
	//if ( !m_is_pvr ) return 0;

	unsigned int count = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentNum( m_dest /*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, &count);
	return ( eRet == ERR_OK ) ? count : 0;
}

bool OplVod::getVideoComponent(int index, OplAVComponent_t *out_comp)
{
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

bool OplVod::getAudioComponent(int index, OplAVComponent_t *out_comp)
{
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

bool OplVod::getSubtitleComponent(int index, OplAVComponent_t *out_comp)
{
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

bool OplVod::getActiveVideoComponent(OplAVComponent_t *out_comp)
{
	//if ( !m_is_pvr ) return false;
	if ( !out_comp ) return false;

	int index = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_VIDEO, &index, NULL);
	if ( eRet != ERR_OK ) return false;

	return getVideoComponent(index, out_comp);
}

bool OplVod::getActiveAudioComponent(OplAVComponent_t *out_comp)
{
	//if ( !m_is_pvr ) return false;
	if ( !out_comp ) return false;

	int index = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_AUDIO, &index, NULL);
	if ( eRet != ERR_OK ) return false;

	return getAudioComponent(index, out_comp);
}

bool OplVod::getActiveSubtitleComponent(OplAVComponent_t *out_comp)
{
	//if ( !m_is_pvr ) return false;
	if ( !out_comp ) return false;

	int index = 0;
	HERROR eRet = APK_MEDIA_PLAY_GetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_SUBTITLE, &index, NULL);
	if ( eRet != ERR_OK || index == -1 ) return false;

	return getSubtitleComponent(index, out_comp);
}

int OplVod::getActiveComponentIndex(eOplAVComponentType comp_type)
{
	int index = -1;
	HERROR ret = ERR_FAIL;

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

	if( ret != ERR_OK ) index = -1;

	return index;
}

bool OplVod::setActiveVideoComponent(int pid)
{
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

bool OplVod::setActiveAudioComponent(int pid, char* pszLang)
{
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
        if( m_is_pvr )
		{
			if ( stComponet.stAudio.nPid == pid ) // !!! ºñ±³ Ãß°¡ ÇÊ¿ä pszLang
			{
				hErr = APK_MEDIA_PLAY_SetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_AUDIO, i);
				if ( hErr != ERR_OK ) continue;
				return true;
			}
		}
		else
		{
			if( (pszLang == NULL) && (stComponet.stAudio.nPid == pid) )
			{
				hErr = APK_MEDIA_PLAY_SetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_AUDIO, i);
				if ( hErr != ERR_OK ) continue;
				return true;
			}
			else if (( stComponet.stAudio.nPid == pid ) && (HxSTD_StrNCmp((HCHAR*)stComponet.stAudio.astLangInfo[0].szLangCode, (HCHAR*)pszLang, 3) == 0))
			{
				hErr = APK_MEDIA_PLAY_SetComponentIndex( m_dest /*ulViewId*/, eOxMP_COMPONENT_AUDIO, i);
				if ( hErr != ERR_OK ) continue;
				return true;
			}

		}
	}
	return false;
}

bool OplVod::setActiveSubtitleComponent(int pid, char* pszLang)
{
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
		if ( stComponet.stSubtitle.nPid == pid ) // !!! ï¿½ï¿½ ï¿½ß°ï¿½ ï¿½Ê¿ï¿½ pszLang
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

bool OplVod::offSubtitleComponent(int pid)
{
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


/***************************************************************************************
 * static interfaces
 **************************************************************************************/
OplVod *OplVod::getActiveVod()
{
	if ( s_vods.empty() ) return NULL;

	list<OplVod*>::iterator begin_it = s_vods.begin();
	OplVod *active_vod = (*begin_it);
	for ( list<OplVod*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
	{
		//if ( (*it)->m_latest_tick >= active_vod->m_latest_tick )
		if( (*it)->m_is_active == true){
			active_vod = (*it);
			return active_vod;
		}
	}

	return NULL;
}

OplVod *OplVod::findVodByOapiSession(const unsigned int id)
{
	if ( s_vods.empty() ) return NULL;

	list<OplVod*>::iterator begin_it = s_vods.begin();
	OplVod *active_vod = NULL;//(*begin_it);
	for ( list<OplVod*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
	{
		if ( (*it)->m_session_id == id )
			active_vod = (*it);
	}

	return active_vod;
}

void OplVod::printVodInfo()
{
	if ( s_vods.empty() ) return ;

	list<OplVod*>::iterator begin_it = s_vods.begin();
	OplVod *vod = (*begin_it);
	int cnt = 0;
	for ( list<OplVod*>::iterator it=s_vods.begin(); it!=s_vods.end(); it++ )
	{
		vod = (*it);
		printf("%d > [%s] > %s \n",  ++cnt, vod->getUrl(), vod->m_is_active?"v":" ");
	}
}


void OplVod::setAutoMode(bool auto_mode)
{
	s_auto_mode = auto_mode;
}

const char *OplVod::getPlayStateString(eOplVodPlayState state)
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

void OplVod::setUseragent(const char* useragent)
{
	m_useragent = useragent;
}

void OplVod::setApplicationUrl(const char* url)
{
	m_application_url = url;
}

static void vod_proc(unsigned int ulViewId, unsigned int session,  OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData)
{
	if ( OplVod::s_vods.empty() ) return;

	OplVod *vod = OplVod::findVodByOapiSession(session);
	if ( (vod == NULL) || (vod != OplVod::getActiveVod()) ) return;

	eOplVodEventType event_type = OPLVOD_EVENT_TYPE_UNKWON;

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
		unsigned long long duration_changed = (int)pstNotifyData->stEventNotify.ullParam1;
		bool bInfinity = (bool)pstNotifyData->stEventNotify.ullParam2;
   		vod->m_duration_Infinity = bInfinity;
		HxLOG_Error("vod_proc >> Duration Changed (%llu[0x%X]), bInfinity : %d \n", duration_changed, duration_changed, bInfinity);
#else
		unsigned int duration_changed = (int)pstNotifyData->stEventNotify.ulParam1;
		bool bInfinity = (bool)pstNotifyData->stEventNotify.ulParam2;
   		vod->m_duration_Infinity = bInfinity;
		HxLOG_Error("vod_proc >> Duration Changed (%lu[0x%X]), bInfinity : %d \n", duration_changed, duration_changed, bInfinity);
#endif
		event_type = OPLVOD_EVENT_TYPE_PLAY_DURATION_CHANGED;
	}
		break;

	case eOxMP_EVENT_PLAYSTATE:
	{
		eOplVodPlayState state = __convert_play_state(pstNotifyData->eStateChanged);

		if ( !vod->isValidEvent(state) ) return;

		HxLOG_Debug("OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED: %s ==> %s\n",
				OplVod::getPlayStateString(vod->getPlayState()), OplVod::getPlayStateString(state));

		if( state == OPLVOD_PLAY_STATE_FINISHED )
		{
			if( vod->m_loop > 1)
			{
				vod->m_loop = vod->m_loop - 1;
				vod->seek(0);
				return;
			}
		}

		vod->setPlayState(state);

		event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
	}
		break;

	case eOxMP_EVENT_PLAYERROR:
	{
		OxMediaPlay_PlayError_e appkit_error;
		OxMediaPlay_ErrorReason_e peReason;
		eOplVodPlayError error_type = OPLVOD_PLAY_ERROR_NO_ERROR;
		APK_MEDIA_PLAY_GetPlayError( ulViewId, &appkit_error, &peReason);
		HxLOG_Debug("eOxMP_EVENT_PLAYERROR: error=%d, reason=%d\n", appkit_error, peReason);
		error_type = __convert_play_error(appkit_error);

		if( (error_type == OPLVOD_PLAY_ERROR_NOT_SUPPORTED_FORMAT)
			|| (error_type == OPLVOD_PLAY_ERROR_INSUFFICIENT_RESOURCE))
		{
			switch(peReason){
				case eOxMP_ERROR_REASON_DATA_MALFORMED_GENERIC:
					error_type = OPLVOD_PLAY_ERROR_CONTENT_CORRUPT; break;

				case eOxMP_ERROR_REASON_SERVER_HTTP_STATUS_NOT_FOUND:
					error_type = OPLVOD_PLAY_ERROR_SERVER_FAIL; break;

				case eOxMP_ERROR_REASON_NETWORK_UNREACHABLE:
					error_type = OPLVOD_PLAY_ERROR_SERVER_FAIL; break;

				case eOxMP_ERROR_REASON_SERVER_HTTP_STATUS_SERVER_ERROR:
					error_type = OPLVOD_PLAY_ERROR_SERVER_FAIL; break;
				default:
					error_type = OPLVOD_PLAY_ERROR_NOT_SUPPORTED_FORMAT; break;
			}
		}


		vod->setPlayError(error_type);
		vod->setPlayState(OPLVOD_PLAY_STATE_ERROR);

		event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
	}
		break;

	case eOxMP_EVENT_MEDIA_BufferStateChanged:
	{
		event_type = OPLVOD_EVENT_TYPE_BUFFER_STATE_CHANGED;

		OxMediaPlay_BufferState_e appkit_state;
		APK_MEDIA_PLAY_GetBufferState( ulViewId, &appkit_state);

		eOplVodBufferState state = __convert_buffer_state(appkit_state);
		vod->m_is_buffered = true;
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

		eOplVodDrmRightsError error = OPLVOD_DRM_RIGHTS_NO_LICENSE;
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
			if( eEvent == eOxMP_EVENT_COMPONENT_UNSELECTED )
			{
#if defined(CONFIG_TIMESTAMP_64)
				HxLOG_Debug("eOxMP_EVENT_COMPONENT_UNSELECTED : component type -> %lld, index -> %d \n", opl_type, (int)pstNotifyData->stEventNotify.ullParam2);
#else
				HxLOG_Debug("eOxMP_EVENT_COMPONENT_UNSELECTED : component type -> %d, index -> %d \n", opl_type, (int)pstNotifyData->stEventNotify.ulParam2);
#endif
				event_type = OPLVOD_EVENT_TYPE_UNSELECTED_COMPONENT_CHANGED;
			}
			else
			{
#if defined(CONFIG_TIMESTAMP_64)
				HxLOG_Debug("eOxMP_EVENT_COMPONENT_SELECTED : component type -> %lld, index -> %d \n", opl_type, (int)pstNotifyData->stEventNotify.ullParam2);
#else
				HxLOG_Debug("eOxMP_EVENT_COMPONENT_SELECTED : component type -> %d, index -> %d \n", opl_type, (int)pstNotifyData->stEventNotify.ulParam2);
#endif
				event_type = OPLVOD_EVENT_TYPE_SELECTED_COMPONENT_CHANGED;
			}
		}
		break;
	case eOxMP_EVENT_AudioOutputRestrict:
		HxLOG_Debug("eOxMP_EVENT_AudioOutputRestrict\n");
		event_type = OPLVOD_EVENT_TYPE_AUDIO_OUTPUT_RESTRICT;
		break;
    /*
      ** @note : this is adding for redmine reference 112020, 112377 and removed for alm 338385, 
      ** is appear aing at alm 490660 
      ** If VRM stop the vod playing (chaning to videobroadcast), send error event 
      ** spec :  6.2.2.7 Suspension of access to broadcast resources
      */
	case eOxMP_EVENT_RESOURCE_LOST:
        if ( vod->isValidEvent(OPLVOD_PLAY_STATE_ERROR) )
        {
            HxLOG_Debug("Sending error event\n");
    		vod->setPlayError(OPLVOD_PLAY_ERROR_CONTENT_NOT_AVAILABLE);
    		vod->setPlayState(OPLVOD_PLAY_STATE_ERROR);
    		event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
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
	eOplVodEventType event_type = OPLVOD_EVENT_TYPE_PLAY_STATE_CHANGED;
	unsigned int prev_session = 0x0;

	if ( OplVod::s_vods.empty() ) return;

	OplVod *vod = OplVod::findVodByOapiSession(session);
	//OplVod *vod = OplVod::getActiveVod();
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
			HxLOG_Error("OplVod::pvr_proc: Error> unmatched sessionID(%d, prev:%d] for [ViewId:%d, eEvent:%d]\n", session, prev_session, ulViewId, eEvent);
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
			HxLOG_Error("OplVod::pvr_proc: Error> unmatched sessionID(%d, prev:%d] for [ViewId:%d, eEvent:%d]\n", session, prev_session, ulViewId, eEvent);
			return;
		}

		vod->setPlayState(OPLVOD_PLAY_STATE_ERROR);

		OxMediaPlay_PlayError_e appkit_error;
		APK_MEDIA_PLAY_GetPlayError( ulViewId, &appkit_error, NULL);

		eOplVodPlayError error = __convert_play_error(appkit_error);
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
					HINT32 					nlValue;

					if (APK_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_SUBTITLE, nTrackIndex, &stComponent) == ERR_OK)
					{
						HAPI_BroadcastSignal("signal:onSubtitleTypeChanged", 1, "i", stComponent.stSubtitle.eSubtitleType);
						APK_DB_GetInt(USERDB_SUBTITLE_DISPLAY, &nlValue);
						HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", nlValue ? 1 : 0);
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
		bool 			blocked	= (bool)pstNotifyData->stEventNotify.ulParam1;
		unsigned int 	rating	= (unsigned int)pstNotifyData->stEventNotify.ulParam2;
		unsigned int 	cas_id	= (unsigned int)pstNotifyData->stEventNotify.ulParam3;
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

		eOplVodDrmRightsError error = OPLVOD_DRM_RIGHTS_NO_LICENSE;
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
		if( NULL == pstNotifyData )
			break;

#if defined(CONFIG_TIMESTAMP_64)
		OplVodStorageRight_e eStorageRight = (OplVodStorageRight_e)pstNotifyData->stEventNotify.ullParam2;
#else
		OplVodStorageRight_e eStorageRight = (OplVodStorageRight_e)pstNotifyData->stEventNotify.ulParam2;
#endif

		vod->setStorageRight(eStorageRight);
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

