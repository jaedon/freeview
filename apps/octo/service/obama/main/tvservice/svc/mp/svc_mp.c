/** **********************************************************************************************************
	@file 		mw_mp.c

	@date		2012/01/02
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
*********************************************************************************************************** */

#include <octo_common.h>

#include "svc_mplayer.h"
#include "_svc_mp_manager.h"
#include "_svc_mp_utils.h"


#include "pal_media.h"
#include "pal_media_sub.h"
#include "pal_scaler.h"
#include "pal_video.h"
#include "pal_audio.h"
#include "pal_output.h"
#include "pal_thumbnail.h"
#include "pal_pipe.h"
#include "stb_params.h"


#include <string.h>
#include <pal_datastream.h>

#if 0
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)
#include "pal_sef.h"
#include "svc_mplayersi.h"
#endif
#endif

#define DATA_STREAM_MEDIA_SUBTITLE    "mediasubtitle"

/*---------------------------------------------------------------------------------
 * defines
 *---------------------------------------------------------------------------------*/



#define SVC_MP_ASSERT(x)		\
	if ( x == NULL )\
	{\
		HxLOG_Error("MW_MP: the param is NULL.\n");\
		return ERR_FAIL;\
	}\

#define ENTRY   HxLOG_Print("%s:%s: ++\n", __FILE__, __FUNCTION__)
#define EXIT   HxLOG_Print("%s:%s: --\n", __FILE__, __FUNCTION__)

#define SVC_MP_EVENT_TASK_NAME			"MediaPlayerEventTask"
#define SVC_MP_EVENT_TASK_PRIORITY		55
#define SVC_MP_EVENT_TASK_STACK_SIZE		(128*1024)

#define SVC_MP_QUEUE_NAME				"MediaPlayerEvent"
#define SVC_MP_QUEUE_SIZE				32
#define SVC_MP_COMMAND_LIMIT				5
#define MP_MP_SEMAPHORE_NAME			"MediaPlayerSem"

#define SVC_MP_DEFAULT_HDD_BUFFER_PATH	"dvr:///mnt/hd3/download"

#define SVC_MP_CAPTURE_WIDTH				140
#define SVC_MP_CAPTURE_HEIGHT			78

#define SVC_MP_SUB_DEFAULT_FONT_SIZE 33
#define SVC_MP_SUB_DEFAULT_TEXT_POSITON 600

/* supported URIs */
#define SUPPORTED_URI_HTTP 				"http:"
#define SUPPORTED_URI_HTTPS 			"https:"
#define SUPPORTED_URI_MMS 				"mms:"
#define SUPPORTED_URI_RTP 				"rtp:"
#define SUPPORTED_URI_VTUNER 			"vtuner:"
#define SUPPORTED_URI_FILE 				"file:"
#define SUPPORTED_URI_DLNA 				"hdlna://"



/*---------------------------------------------------------------------------------
 * typedef
 *---------------------------------------------------------------------------------*/

typedef struct _MW_MediaPlayerInfo_t {
	SvcMp_Config_t		config;
	HULONG						hmsg;
	HULONG						event_task;
	HULONG						lock;
	HBOOL						task_flag;
	HCHAR						*szBrowserUserAgentString;
} MW_MediaPlayerInfo_t;

typedef struct _MW_MediaPlayerEventInfo_t {
	SvcMp_MediaPlayer_t			*player;
	SvcMp_Command_e		command;
	PAL_MEDIA_Event_e			event;
	HUINT32						param;
	SvcMp_EventType_e local_event;
} MW_MediaPlayerEventInfo_t;

typedef struct _MW_MediaPlayerData_t {
	HINT8					url[SVC_MP_MAX_URL_LEN];
	SvcMp_PlayType_e	playType;
} MW_MediaPlayerData_t;


/*---------------------------------------------------------------------------------
  static variables
 *---------------------------------------------------------------------------------*/
static MW_MediaPlayerInfo_t		s_media_info;

/*---------------------------------------------------------------------------------
 * static functions
 *---------------------------------------------------------------------------------*/
static inline PAL_MEDIA_t *__pal(SvcMp_MediaPlayer_t *player)
{
	return (PAL_MEDIA_t *)player->phyMediaHandle;
}

static inline void __state(SvcMp_MediaPlayer_t *player, SvcMp_MediaState_e state)
{
	player->eState = state;
}

static void __lock(void)
{
	VK_SEM_Get(s_media_info.lock);
}

static void __unlock(void)
{
	VK_SEM_Release(s_media_info.lock);
}

static HBOOL __prefix_match(const HINT8* url, const HINT8 *prefix)
{
	HBOOL match = VK_strncmp(url, prefix, strlen(prefix) ) == 0;
	return match;
}

static void __mediaplayer_send_event(SvcMp_MediaPlayer_t *player,
					SvcMp_Event_e event)
{
	media_player_utils_PostMsgToMgr (event, PAL_PIPE_GetActionHandle(player->hActionId), 0, 0, 0);
}

static void __mediaplayer_send_command(SvcMp_MediaPlayer_t *player,
						SvcMp_Command_e command, HUINT32 p1)
{
	MW_MediaPlayerEventInfo_t info;
	HINT32 nMsgCount = 0, nMsgMax = 0;

	VK_MSG_GetMessageCount(s_media_info.hmsg, &nMsgCount, &nMsgMax);
	if ( nMsgCount > SVC_MP_COMMAND_LIMIT ) VK_MSG_Clear(s_media_info.hmsg);

	info.player = player;
	info.command = command;
	info.event = 0;
	info.param = p1;
	info.local_event = eSVC_MP_LOCAL_EVENT_COMMAND;

	VK_MSG_Send(s_media_info.hmsg, (void *)&info, sizeof(info));
}

static SvcMp_UriType_e __mediaplayer_get_uri_type(const HINT8 *url)
{
	if ( __prefix_match(url, SUPPORTED_URI_HTTP) ) return eSVC_MP_URI_HTTP;
	if ( __prefix_match(url, SUPPORTED_URI_HTTPS) ) return eSVC_MP_URI_HTTPS;
	if ( __prefix_match(url, SUPPORTED_URI_MMS) ) return eSVC_MP_URI_MMS;
	if ( __prefix_match(url, SUPPORTED_URI_RTP) ) return eSVC_MP_URI_RTP;
	if ( __prefix_match(url, SUPPORTED_URI_VTUNER) ) return eSVC_MP_URI_VTUNER;
	if ( __prefix_match(url, SUPPORTED_URI_FILE) ) return eSVC_MP_URI_FILE;
	if ( __prefix_match(url, SUPPORTED_URI_DLNA) ) return eSVC_MP_URI_DLNA;

	return eSVC_MP_URI_NOT_SUPPORTED;
}

static HBOOL __mediaplayer_is_supported_uri(SvcMp_MediaPlayer_t *player, const HINT8 *url)
{
	player->eUriType = __mediaplayer_get_uri_type(url);
	return ( player->eUriType == eSVC_MP_URI_NOT_SUPPORTED ) ? FALSE : TRUE;
}

static HBOOL __mediaplayer_is_file_content(const HINT8 *url)
{
	if ( url == NULL ) return FALSE;

	return __prefix_match(url, SUPPORTED_URI_FILE);
}

static HBOOL __mediaplayer_is_dlna_content(const HINT8 *url)
{
	if ( url == NULL ) return FALSE;

	return __prefix_match(url, SUPPORTED_URI_DLNA);
}

static HBOOL __mediaplayer_is_vtuner_content(const HINT8 *url)
{
	if ( url == NULL ) return FALSE;

	return __prefix_match(url, SUPPORTED_URI_VTUNER);
}

static HBOOL __mediaplayer_is_smooth_streaming_content(const HINT8 *url)
{
	if ( url == NULL ) return FALSE;

	if ( strstr(url, "Manifest") ||  strstr(url, "manifest") || strstr(url, ".ism") )
	{
		return TRUE;
	}

	return FALSE;
}

static HBOOL __mediaplayer_is_audio_content(SvcMp_MediaPlayer_t *player)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_INFO_t tMediaInfo;

	SVC_MP_ASSERT(player);

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: __mediaplayer_is_audio_content: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	return ( tMediaInfo.video_pes_id == 0 && tMediaInfo.audio_pes_id != 0 ) ? TRUE : FALSE;
}

static HBOOL __mediaplayer_is_pdl(SvcMp_MediaPlayer_t *player)
{
	HBOOL bPDL = FALSE;

	if ( player->ePlayType == eSVC_MP_PLAYTYPE_AUTO )
	{
		/*
		HxLOG_Print("MW_MP: __mediaplayer_is_pdl: if your box have an internal HDD,\
			 the player will be operated by progressive download automatically.\n");*/

		bPDL = ( s_media_info.config.bProgressiveDownload == TRUE ) ? TRUE : FALSE;
	}
	else
	{
		bPDL = ( player->ePlayType == eSVC_MP_PLAYTYPE_PROGORESSIVE ) ? TRUE : FALSE;
	}

#if 0
	HxLOG_Print("MW_MP: __mediaplayer_is_pdl: the player will be operated by %s.\n",
			(bPDL) ? "progressive download" : "streaming");
#endif
	return bPDL;
}

static void __mediaplayer_sync_output(SvcMp_MediaPlayer_t *player)
{
	HERROR eRet = ERR_OK;

	if ( player == NULL ) return;

	HxLOG_Print("__mediaplayer_sync_output: w(%d), h(%d)\n",
						__pal(player)->ulVideoStreamWidth, __pal(player)->ulVideoStreamHeight);

	eRet = PAL_SCALER_SetVideoAttribute(0, player->hActionId, TRUE,
				__pal(player)->ulVideoStreamWidth,
				__pal(player)->ulVideoStreamHeight,
				(eDxASPECTRATIO_UNKNOWN == __pal(player)->eVideoAspectRatio) ?\
										eDxASPECTRATIO_1X1 : __pal(player)->eVideoAspectRatio,
				(eDxASPECTRATIO_UNKNOWN == __pal(player)->eVideoAspectRatio) ? eAfd_None : eAfd_Full,
				FALSE,
				eFrameRate_Unknown,
				eDxVIDEO_3DFORMAT_NONE
				);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("PAL_SCALER_SetVideoAttribute() returned ERR_FAIL.\n");
	}
}

static void __mediaplayer_send_subtitle(SvcMp_MediaPlayer_t *player, HUINT8 *buffer, HINT32 len)
{
	PAL_MEDIA_SUB_Display_Packet_t t_subpkt;
	memset(&t_subpkt, 0x00, sizeof(PAL_MEDIA_SUB_Display_Packet_t));

	if(buffer != NULL)
		memcpy(&t_subpkt, buffer, sizeof(PAL_MEDIA_SUB_Display_Packet_t));
	else
		t_subpkt.ulStrNum = -1;

	t_subpkt.ulFontSize = player->ulSubtitleFontSize;
	t_subpkt.ulTextPosition = player->ulSubtitleDisplayPosition;

	PAL_DSTREAM_Broadcast(DATA_STREAM_MEDIA_SUBTITLE, len, (void*)&t_subpkt);
}

static void __mediaplayer_subtitle_open(SvcMp_MediaPlayer_t *player)
{
	if ( __mediaplayer_is_file_content(player->szUrl) )
	{
		HUINT32 ulSubRet = ERR_FAIL;
		HUINT8 *pSubFile = (HUINT8 *)HLIB_STD_MemAlloc(SVC_MP_MAX_URL_LEN);
		HUINT8 *szSubtitleFile = (HUINT8 *)HLIB_STD_MemAlloc(SVC_MP_MAX_URL_LEN);

		memset(pSubFile, 0x00, SVC_MP_MAX_URL_LEN);
		memset(szSubtitleFile, 0x00, SVC_MP_MAX_URL_LEN);

		VK_strncpy(szSubtitleFile, (player->szUrl)+5, strlen(player->szUrl));
		media_player_utils_GetFileNameWithoutExt(szSubtitleFile, pSubFile);

		ulSubRet = PAL_MEDIA_SUB_Open(__pal(player), pSubFile);
		if(ulSubRet == ERR_OK)
		{
			media_player_utils_PostMsgToMgr (eSVC_MP_EVENT_SBTL_START, PAL_PIPE_GetActionHandle (player->hActionId), 0, 0, 0);
			__mediaplayer_send_subtitle(player, NULL,  sizeof(PAL_MEDIA_SUB_Display_Packet_t));
			player->bhasSubtitle = TRUE;

			/* Default Subtitle Show ? */
			PAL_MEDIA_SUB_SetDisplay(__pal(player), TRUE);
		}
		else
		{
			media_player_utils_PostMsgToMgr (eSVC_MP_EVENT_SBTL_UPDATE, PAL_PIPE_GetActionHandle (player->hActionId), 0, 0, 0);
			PAL_MEDIA_SUB_Close( __pal(player));
		}

		HLIB_STD_MemFree(pSubFile);
		HLIB_STD_MemFree(szSubtitleFile);
	}
}

static void __mediaplayer_subtitle_close(SvcMp_MediaPlayer_t *player)
{
	if ( __mediaplayer_is_file_content(player->szUrl) )
	{
		(void)PAL_MEDIA_SUB_Close( __pal(player));
	}
}

static void __mediaplayer_prepare(SvcMp_MediaPlayer_t *player, SvcMp_PlayType_e ePlayType,
                            const HINT8 *url)
{
	VK_strncpy(player->szUrl, url, VK_strlen(url)+1);
	player->ePlayType = ePlayType;

	if ( __mediaplayer_is_file_content(url)
			|| __mediaplayer_is_dlna_content(url)
			|| __mediaplayer_is_vtuner_content(url)
			|| __mediaplayer_is_smooth_streaming_content(url) )
	{
		player->ePlayType = eSVC_MP_PLAYTYPE_STREAMING;
	}

	if ( __mediaplayer_is_dlna_content(url) )
	{
		VK_strncpy(player->szUrl, url + VK_strlen(SUPPORTED_URI_DLNA), sizeof(player->szUrl)-1);
	}

	if ( __mediaplayer_is_vtuner_content(url) )
	{
		VK_strncpy(player->szUrl, SUPPORTED_URI_HTTP, VK_strlen(SUPPORTED_URI_HTTP)+1);
		VK_strncpy(player->szUrl+VK_strlen(SUPPORTED_URI_HTTP),
			url + VK_strlen(SUPPORTED_URI_VTUNER), sizeof(player->szUrl)-1);
	}

	HxLOG_Print("__mediaplayer_prepare: PlayUrl = %s\n", player->szUrl);
	HxLOG_Print("__mediaplayer_prepare: PlayType = %d\n", player->ePlayType);
}

static void __mediaplayer_set_uastring(SvcMp_MediaPlayer_t *player)
{
	HINT32 nUASize =0;
	if ( s_media_info.szBrowserUserAgentString != NULL )
	{
		nUASize = strlen(s_media_info.szBrowserUserAgentString);
		PAL_MEDIA_SetUserAgentString(__pal(player), s_media_info.szBrowserUserAgentString, nUASize);
	}
}

HERROR __mediaplayer_stop(SvcMp_MediaPlayer_t *player)
{
	ENTRY;

#if 0
	switch ( player->eState )
	{
	case eSVC_MP_STATE_SESSION_STARTED:
	case eSVC_MP_STATE_PLAYING:
	case eSVC_MP_STATE_SEEKING:
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)
		(void)SVC_MP_SI_Release();
#endif
		PAL_MEDIA_StopSession(__pal(player));

	case eSVC_MP_STATE_SESSION_SETUP:
		PAL_MEDIA_ReleaseSession(__pal(player));

	case eSVC_MP_STATE_SESSION_OPENED:
		PAL_MEDIA_CloseSession(__pal(player));

	case eSVC_MP_STATE_BUFFER_STARTED:
		if ( __mediaplayer_is_pdl(player) )
			PAL_MEDIA_StopBuffering(__pal(player));

	case eSVC_MP_STATE_PROBE_FINISHED:
	case eSVC_MP_STATE_PROBE_CANCELED:
	case eSVC_MP_STATE_PROBE_FAILED:
	case eSVC_MP_STATE_SESSION_CREATED:
		__lock();
		PAL_MEDIA_DestroySession(__pal(player));
		__unlock();

	case eSVC_MP_STATE_IDLE:
	case eSVC_MP_STATE_STOPPED:
	default:
		break;
	}
#endif
	__lock();
	switch ( player->eState )
	{
	case eSVC_MP_STATE_SESSION_STARTED:
	case eSVC_MP_STATE_PLAYING:
	case eSVC_MP_STATE_SEEKING:
	case eSVC_MP_STATE_SESSION_SETUP:
#if 0
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)
		(void)SVC_MP_SI_Release();
#endif
#endif
		PAL_MEDIA_StopSession(__pal(player));

	case eSVC_MP_STATE_SESSION_OPENED:
		PAL_MEDIA_ReleaseSession(__pal(player));
		PAL_MEDIA_CloseSession(__pal(player));

	case eSVC_MP_STATE_BUFFER_STARTED:
		if ( __mediaplayer_is_pdl(player) )
			PAL_MEDIA_StopBuffering(__pal(player));

	case eSVC_MP_STATE_PROBE_FINISHED:
	case eSVC_MP_STATE_PROBE_CANCELED:
	case eSVC_MP_STATE_PROBE_FAILED:
	case eSVC_MP_STATE_SESSION_CREATED:
		PAL_MEDIA_DestroySession(__pal(player));

	case eSVC_MP_STATE_IDLE:
	case eSVC_MP_STATE_STOPPED:
	default:
		break;
	}

	player->nSpeed = 100;
	player->ullStartTime = 0;
	player->ePlayType = eSVC_MP_PLAYTYPE_NONE;

	__state(player, eSVC_MP_STATE_STOPPED);

	__unlock();

	EXIT;

	return ERR_OK;
}

static HERROR __mediaplayer_start_buffering(SvcMp_MediaPlayer_t *player)
{
	HERROR eRet = ERR_OK;

	ENTRY;

	if ( player->eState == eSVC_MP_STATE_BUFFER_STARTED )
	{
		HxLOG_Error("MW_MP: __mediaplayer_buffering: Buffering is already started.\n");
		return ERR_OK;
	}

	eRet = PAL_MEDIA_StartBuffering(__pal(player), s_media_info.config.szPDLPath);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: __mediaplayer_buffering: Start or Resume Buffering failed.\n");
		return ERR_FAIL;
	}
	__state(player, eSVC_MP_STATE_BUFFER_STARTED);

	EXIT;

	return ERR_OK;
}

static HERROR __mediaplayer_probe(SvcMp_MediaPlayer_t *player)
{
	HERROR 				eRet = ERR_OK;
	PAL_MEDIA_INFO_t	tMediaInfo;

	ENTRY;

#if 0
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)
	(void)SVC_MP_SI_Setup(player);
#endif
#endif

	eRet = PAL_MEDIA_CreateSession(__pal(player), player->szUrl);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: __mediaplayer_probe: CreateSession failed.(url=%s)\n", player->szUrl);
#if 0
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)
		(void)SVC_MP_SI_Release();
#endif
#endif
		return ERR_FAIL;
	}
	__state(player, eSVC_MP_STATE_SESSION_CREATED);

	__mediaplayer_set_uastring(player);

	eRet = PAL_MEDIA_ProbeSession(__pal(player));
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: __mediaplayer_probe: ProbeSession failed.\n");
#if 0
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)
		(void)SVC_MP_SI_Release();
#endif
#endif
		__state(player, eSVC_MP_STATE_PROBE_FAILED);
		return ERR_FAIL;
	}
	__state(player, eSVC_MP_STATE_PROBE_FINISHED);

	(void)PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);

	/* check if this content uses icy protocol */
	if ( tMediaInfo.isIcyProtocol == TRUE )
		player->ePlayType = eSVC_MP_PLAYTYPE_STREAMING;

	if ( __mediaplayer_is_pdl(player) )
	{
		eRet = __mediaplayer_start_buffering(player);
		if ( eRet != ERR_OK )
		{
			__mediaplayer_send_event(player, eSVC_MP_EVENT_ERROR);
			HxLOG_Error("MW_MP: __mediaplayer_probe: the buffering is failed.\n");
			return ERR_FAIL;
		}
		HxLOG_Print("MW_MP: __mediaplayer_probe: the buffering starts.\n");
	}
	else
	{
		__mediaplayer_send_event(player, eSVC_MP_EVENT_BUFFERING_CANPLAY);
		HxLOG_Print("MW_MP: __mediaplayer_probe: no HDD: realtime streaming.\n");
	}

	EXIT;

	return ERR_OK;
}

static HERROR __mediaplayer_play(SvcMp_MediaPlayer_t *player)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_INFO_t tMediaInfo;

	ENTRY;

	eRet = PAL_MEDIA_OpenSession(__pal(player));
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: __mediaplayer_play: OpenSession failed.\n");
		return ERR_FAIL;
	}

	__state(player, eSVC_MP_STATE_SESSION_OPENED);

	eRet = PAL_MEDIA_SetupSession(__pal(player));
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: __mediaplayer_play: SetupSession failed.\n");
		return ERR_FAIL;
	}

	__state(player, eSVC_MP_STATE_SESSION_SETUP);

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: __mediaplayer_play: PAL_MEDIA_GetProbeInfo failed.\n");
		return ERR_FAIL;
	}

	eRet = PAL_OUT_SetDigitalAudioOutCodec(tMediaInfo.audio_codec, eDxAUDIO_CODEC_UNKNOWN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: __mediaplayer_play: PAL_OUT_SetDigitalAudioOutCodec() failed.\n");
		return ERR_FAIL;
	}

	eRet = PAL_MEDIA_StartSession(__pal(player), player->ullStartTime);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: __mediaplayer_play: StartSession failed.\n");
		return ERR_FAIL;
	}
	player->ullStartTime = 0;
	__state(player, eSVC_MP_STATE_SESSION_STARTED);

	EXIT;

	return ERR_OK;
}

static HERROR __mediaplayer_cmd_probe(SvcMp_MediaPlayer_t *player)
{
	HERROR eRet = ERR_OK;

	if ( player->eState != eSVC_MP_STATE_IDLE &&
		player->eState != eSVC_MP_STATE_STOPPED )	__mediaplayer_stop(player);

	__lock();
	eRet = __mediaplayer_probe(player);
	if ( eRet != ERR_OK )
	{
		__unlock();
		__mediaplayer_send_event(player, eSVC_MP_EVENT_PROBE_FAILED);
		__mediaplayer_stop(player);
		HxLOG_Error("MW_MP: __mediaplayer_cmd_probe: the content probing is failed.\n");
		return ERR_FAIL;
	}

	__unlock();
	__mediaplayer_send_event(player, eSVC_MP_EVENT_PROBE_FINISHED);

	HxLOG_Print("__mediaplayer_cmd_probe: the probe is finished. (url=%s)\n", player->szUrl);

	return ERR_OK;
}

static HERROR __mediaplayer_cmd_play(SvcMp_MediaPlayer_t *player)
{
	HERROR eRet = ERR_OK;

	if ( player->eState != eSVC_MP_STATE_BUFFER_STARTED
			&& player->eState != eSVC_MP_STATE_PROBE_FINISHED )
	{
		HxLOG_Error("MW_MP: Play: you should call probe function before play().\n");
		return ERR_FAIL;
	}

	__lock();
	eRet = __mediaplayer_play(player);
	if ( eRet != ERR_OK )
	{
		__unlock();
		__mediaplayer_send_event(player, eSVC_MP_EVENT_ERROR);
		__mediaplayer_stop(player);
		HxLOG_Error("MW_MP: Play: the playback is failed.\n");
		return ERR_FAIL;
	}
	__unlock();

	__mediaplayer_subtitle_open(player);

	HxLOG_Print("MW_MP: Play: the content playing is started. (url=%s)\n", player->szUrl);

	return ERR_OK;
}

static HERROR __mediaplayer_cmd_stop(SvcMp_MediaPlayer_t *player)
{
	    return __mediaplayer_stop(player);
}

static HERROR __mediaplayer_cmd_peek(SvcMp_MediaPlayer_t *player)
{
	HERROR eRet = ERR_OK;

	eRet = __mediaplayer_probe(player);
	if ( eRet != ERR_OK )
	{
		__mediaplayer_send_event(player, eSVC_MP_EVENT_PROBE_FAILED);
		__mediaplayer_stop(player);
		HxLOG_Error("MW_MP: __mediaplayer_cmd_peek: the content probing is failed.\n");
		return ERR_FAIL;
	}

	__mediaplayer_send_event(player, eSVC_MP_EVENT_PROBE_FINISHED);

	HxLOG_Print("__mediaplayer_cmd_peek: the probe is finished. (url=%s)\n", player->szUrl);

	return ERR_OK;
}

static void __mediaplayer_event_handler(void *identifier,
							PAL_MEDIA_Event_e event, HUINT32 param)
{
	MW_MediaPlayerEventInfo_t info;

	info.player = (SvcMp_MediaPlayer_t *)media_player_manager_find(identifier);
	info.command = eSVC_MP_LOCAL_COMMAND_NONE;
	info.event = event;
	info.param = param;
	info.local_event = eSVC_MP_LOCAL_EVENT_CALLBACK;

	VK_MSG_Send(s_media_info.hmsg, (void *)&info, sizeof(info));
}

static void __mediaplayer_event_command_process(MW_MediaPlayerEventInfo_t *info)
{
	SvcMp_MediaPlayer_t *player = info->player;
#if 0
	HERROR			nResult = ERR_OK;
#endif
	if ( player == NULL ) return;

	switch ( info->command )
	{
	case eSVC_MP_LOCAL_COMMAND_PROBE:
	{
		MW_MediaPlayerData_t *data = (MW_MediaPlayerData_t *)info->param;
		__mediaplayer_prepare(player, data->playType, data->url);
		HLIB_STD_MemFree(data);
		(void)__mediaplayer_cmd_probe(player);
		break;
	}
	case eSVC_MP_LOCAL_COMMAND_PLAY:
		(void)__mediaplayer_cmd_play(player);
		break;
	case eSVC_MP_LOCAL_COMMAND_STOP:
		(void)__mediaplayer_cmd_stop(player);
		break;
	case eSVC_MP_LOCAL_COMMAND_SEEK:
		break;
	case eSVC_MP_LOCAL_COMMAND_PEEK:
	{
		MW_MediaPlayerData_t *data = (MW_MediaPlayerData_t *)info->param;
		__mediaplayer_prepare(player, data->playType, data->url);
		HLIB_STD_MemFree(data);
		(void)__mediaplayer_cmd_peek(player);
		break;
	}

#if 0
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)
	case eSVC_MP_LOCAL_COMMAND_SI:
		nResult = SVC_MP_SI_CmdSi(PAL_PIPE_GetActionHandle(player->hActionId), (eMEDIA_RETRIEVE_PAT | eMEDIA_RETRIEVE_EIT_ACT_PF));
		if(nResult != ERR_OK)
		{
			HxLOG_Error("Media_CmdSi failure \r\n");
		}
		break;

	case eSVC_MP_LOCAL_COMMAND_PAT_TABLE:
		nResult = SVC_MP_SI_ReceivedPAT(PAL_PIPE_GetActionHandle(player->hActionId), (HUINT32)(info->param >> 16), (HUINT32)(info->param & 0xff));
		if(nResult != ERR_OK)
		{
			HxLOG_Error("Media_ReceivedPAT failure \r\n");
		}
		break;

	case eSVC_MP_LOCAL_COMMAND_PMT_TABLE:
		nResult = SVC_MP_SI_ReceivedPMT(PAL_PIPE_GetActionHandle(player->hActionId), (HUINT32)(info->param >> 16), (HUINT32)(info->param & 0xff));
		if(nResult != ERR_OK)
		{
			HxLOG_Error("Media_ReceivedPMT failure \r\n");
		}
		break;

	case eSVC_MP_LOCAL_COMMAND_SIT_TABLE:
		nResult = SVC_MP_SI_ReceivedSIT(PAL_PIPE_GetActionHandle(player->hActionId), (HUINT32)(info->param >> 16), (HUINT32)(info->param & 0xff));
		if(nResult != ERR_OK)
		{
			HxLOG_Error("Media_ReceivedSIT failure \r\n");
		}
		break;
	case eSVC_MP_LOCAL_COMMAND_EIT_TABLE:
		nResult = SVC_MP_SI_ReceivedPfEIT(PAL_PIPE_GetActionHandle(player->hActionId), (HUINT32)(info->param >> 16), (HUINT32)(info->param & 0xff));
		if(nResult != ERR_OK)
		{
			HxLOG_Error("SVC_MP_SI_ReceivedPfEIT failure \r\n");
		}
		break;
#endif
#endif

	case eSVC_MP_LOCAL_COMMAND_NONE:
	default:
		break;
	}
}

static void __mediaplayer_event_callback_process(MW_MediaPlayerEventInfo_t *info)
{
	SvcMp_MediaPlayer_t *player = info->player;
	if ( player ==  NULL ) return;

	switch ( info->event )
	{
	case PAL_MEDIA_EVT_VIDEO_FRAMESTART:
	{
		HxLOG_Print("MW_MP: EventListener: PAL_MEDIA_EVT_VIDEO_FRAMESTART\n");

		PAL_VIDEO_SetHide(0, eAvHideClient_Decoder, FALSE);

		__mediaplayer_sync_output(player);

		if ( player->eState != eSVC_MP_STATE_PLAYING )
		{
			__mediaplayer_send_event(player, eSVC_MP_EVENT_PLAYING);
			__state(player, eSVC_MP_STATE_PLAYING);
		}

		HxLOG_Print("MW_MP: EventListener: PAL_MEDIA_eEVT_AUDIO_NEWFRAME: Settings Done.\n");
	}
		break;

	case PAL_MEDIA_EVT_VIDEO_UNDERRUN:
		HxLOG_Print("MW_MP: EventListener: PAL_MEDIA_EVT_VIDEO_UNDERRUN.\n");
		//PAL_VIDEO_SetHide(0, eAvHideClient_Decoder, TRUE);
		__mediaplayer_send_event(player, eSVC_MP_EVENT_VIDEO_UNDERRUN);
		break;
	case PAL_MEDIA_EVT_VIDEO_FORMAT_CHANGED:
	case PAL_MEDIA_EVT_VIDEO_SEQUENCE_HEADER_INFO_CHANGED:
	case PAL_MEDIA_EVT_VIDEO_AFD_CHANGED:
	case PAL_MEDIA_EVT_VIDEO_ERROR_NONE:
		break;

	case PAL_MEDIA_eEVT_AUDIO_NEWFRAME:
		HxLOG_Print("MW_MP: EventListener: PAL_MEDIA_eEVT_AUDIO_NEWFRAME\n");
		PAL_AUDIO_SetHide(eAvHideClient_Decoder, FALSE);
		PAL_OUT_SetDecoderAudioMute(FALSE);

		if ( player->nSpeed == 0 ) SVC_MP_SetPlaySpeed(player, player->nSpeed);

		if ( player->ullStartTime != 0 )
		{
			PAL_MEDIA_SetPosition(__pal(player), player->ullStartTime);
			player->ullStartTime = 0;
		}

		if ( player->eState != eSVC_MP_STATE_PLAYING )
		{
			__mediaplayer_send_event(player, eSVC_MP_EVENT_PLAYING);
			__state(player, eSVC_MP_STATE_PLAYING);
		}

		HxLOG_Print("MW_MP: EventListener: PAL_MEDIA_eEVT_AUDIO_NEWFRAME: Settings Done.\n");

		break;

	case PAL_MEDIA_eEVT_AUDIO_NODATA:
		HxLOG_Print("MW_MP: EventListener: PAL_MEDIA_eEVT_AUDIO_NODATA.\n");
		PAL_AUDIO_SetHide(eAvHideClient_Decoder, TRUE);
		PAL_OUT_SetDecoderAudioMute(TRUE);
		break;

	case PAL_MEDIA_eEVT_AUDIO_FORMAT_CHANGED:
		break;

	case PAL_MEDIA_eEVT_AUDIO_STREAM_COMPLETE:
		break;

	case PAL_MEDIA_EVT_MEDIA_BEGINOFSTREAM:
		__mediaplayer_send_event(player, eSVC_MP_EVENT_BEGIN_OF_STREAM);
		break;
	case PAL_MEDIA_EVT_MEDIA_ERROROFSTREAM:
		HxLOG_Error("MW_MP: EventListener: PAL_MEDIA_EVT_MEDIA_ERROROFSTREAM\n");
		__mediaplayer_send_event(player, eSVC_MP_EVENT_ERROR);
		break;
	case PAL_MEDIA_EVT_MEDIA_ENDOFSTREAM:
		__mediaplayer_send_event(player, eSVC_MP_EVENT_END_OF_STREAM);
		break;
	case PAL_MEDIA_EVT_MEDIA_CHANGEPLAYTIME:
		__mediaplayer_send_event(player, eSVC_MP_EVENT_PLAY_POSITION_CHANGED);
		break;

		/* the events related the playback are notified by MW MediaPlayer */
	case PAL_MEDIA_EVT_MEDIA_START:
		__mediaplayer_send_event(player, eSVC_MP_EVENT_PLAY_START);
		break;

	case PAL_MEDIA_EVT_MEDIA_PAUSE:
	case PAL_MEDIA_EVT_MEDIA_TRICK:
	case PAL_MEDIA_EVT_MEDIA_STOP:
	case PAL_MEDIA_EVT_MEDIA_PLAYING:
	case PAL_MEDIA_EVT_MEDIA_PROBE_START:
	case PAL_MEDIA_EVT_MEDIA_PROBE_COMPLETED:
		break;

	case PAL_MEDIA_EVT_MEDIA_PROBE_FAILURE:
		HxLOG_Error("MW_MP: EventListener: PAL_MEDIA_EVT_MEDIA_PROBE_FAILURE\n");
		__mediaplayer_send_event(player, eSVC_MP_EVENT_ERROR);
		break;

	case PAL_MEDIA_EVT_MEDIA_BUFFER_CANPLAY:
		HxLOG_Print("MW_MP: EVENTLISTENER: PAL_MEDIA_EVT_MEDIA_BUFFER_CANPLAY\n");
		__mediaplayer_send_event(player, eSVC_MP_EVENT_BUFFERING_CANPLAY);
		break;

	case PAL_MEDIA_EVT_MEDIA_BUFFER_UNDERFLOW:
		HxLOG_Print("MW_MP: EVENTLISTENER: PAL_MEDIA_EVT_MEDIA_BUFFER_UNDERFLOW\n");
		__mediaplayer_send_event(player, eSVC_MP_EVENT_BUFFERING_NOT_ENOUGH);
		if ( !player->bBufferingAutoMode ) PAL_MEDIA_SetSpeed(__pal(player), 0);
		break;

	case PAL_MEDIA_EVT_MEDIA_BUFFER_ENOUGH:
		HxLOG_Print("MW_MP: EVENTLISTENER: PAL_MEDIA_EVT_MEDIA_BUFFER_ENOUGH\n");
		__mediaplayer_send_event(player, eSVC_MP_EVENT_BUFFERING_ENOUGH);
		if ( !player->bBufferingAutoMode )
		{
			if ( player->nSpeed == 0 ) break;
			PAL_MEDIA_SetSpeed(__pal(player), 100);
		}
		break;

	case PAL_MEDIA_EVT_MEDIA_BUFFER_COMPLETE:
		HxLOG_Print("MW_MP: EVENTLISTENER: PAL_MEDIA_EVT_MEDIA_BUFFER_FINISHED\n");
		__mediaplayer_send_event(player, eSVC_MP_EVENT_BUFFERING_FINISHED);
		break;


	case PAL_MEDIA_EVT_MEDIA_BUFFER_FAILURE:
		HxLOG_Error("MW_MP: EventListener: PAL_MEDIA_EVT_MEDIA_BUFFER_FAILURE\n");
		__mediaplayer_send_event(player, eSVC_MP_EVENT_ERROR);
		break;

	case PAL_MEDIA_EVT_MEDIA_BUFFER_ADAPTIVS_STREAM_CHANGE:
		break;

	case PAL_MEDIA_EVT_DRM_OUTPUT_CONTROL:
		__mediaplayer_send_event(player, eSVC_MP_EVENT_OUTPUT_CONTROL);
		HxLOG_Print("MW_MP: EVENTLISTENER: PAL_MEDIA_EVT_DRM_OUTPUT_CONTROL\n");
		break;

	case PAL_MEDIA_EVT_MEDIA_SUB_UPDATE:
		{
			__mediaplayer_send_subtitle(player, (HUINT8 *)info->param,  sizeof(PAL_MEDIA_SUB_Display_Packet_t));
			media_player_utils_PostMsgToMgr (eSVC_MP_EVENT_SBTL_UPDATE, PAL_PIPE_GetActionHandle (player->hActionId), 0, 0, 0);
			break;
		}

	default:
		break;
	}
}

static void __mediaplayer_event_task(void *args)
{
	while ( 1 )
	{
		MW_MediaPlayerEventInfo_t info;
		HxSTD_memset(&info, 0, sizeof(info));

		if ( s_media_info.task_flag == FALSE ) break;

		if ( VK_MSG_ReceiveTimeout(s_media_info.hmsg, &info, sizeof(info), 500) != VK_OK )
		{
			/* Bufering Information Changed Event */
			SvcMp_MediaPlayer_t * player = SVC_MP_GetActivePlayer(0); // how to get main view
			if ( !player ) continue;
			if ( __mediaplayer_is_pdl(player) == FALSE ) continue;

			if ( player->eState == eSVC_MP_STATE_BUFFER_STARTED
				|| player->eState == eSVC_MP_STATE_SESSION_OPENED
				|| player->eState == eSVC_MP_STATE_SESSION_SETUP
				|| player->eState == eSVC_MP_STATE_SESSION_STARTED
				|| player->eState == eSVC_MP_STATE_PLAYING )
			{
				//HxLOG_Print("MW_MP: IDLE: Buffering Info Updated...\n");
				__mediaplayer_send_event(player, eSVC_MP_EVENT_BUFFERING_UPDATED);
			}

			continue;
		}

		if ( info.local_event == eSVC_MP_LOCAL_EVENT_CALLBACK )
		{
			__mediaplayer_event_callback_process(&info);
		}
		else
		{
			__mediaplayer_event_command_process(&info);
		}

	}
}

/*---------------------------------------------------------------------------------
 * Interfaces
 *---------------------------------------------------------------------------------*/
HERROR SVC_MP_Init(const SvcMp_Config_t *ptConfig, PostMsgToMgrCb_t fnPostMsgToMgr)
{
	if ( ptConfig == NULL )
	{
		HxLOG_Error("MW_MP: Init: you should input the configuration\n");
		return ERR_FAIL;
	}

	media_player_utils_RegisterPostMsgCallback(fnPostMsgToMgr);

	HxSTD_memcpy(&s_media_info.config, ptConfig, sizeof(s_media_info.config));

	media_player_manager_init();

	if ( VK_SEM_Create(&s_media_info.lock, MP_MP_SEMAPHORE_NAME,
			VK_SUSPENDTYPE_PRIORITY ) != VK_OK )
	{
		return ERR_FAIL;
	}

	if ( VK_MSG_Create(SVC_MP_QUEUE_SIZE, sizeof(MW_MediaPlayerEventInfo_t), SVC_MP_QUEUE_NAME,
			&s_media_info.hmsg, VK_SUSPENDTYPE_PRIORITY) != VK_OK )
	{
		HxLOG_Error("MW_MP: Init: Cannot create the event for media player.\n");
		return ERR_FAIL;
	}

	s_media_info.task_flag = TRUE;

	if ( VK_TASK_Create(__mediaplayer_event_task, SVC_MP_EVENT_TASK_PRIORITY,
			SVC_MP_EVENT_TASK_STACK_SIZE, SVC_MP_EVENT_TASK_NAME,
			NULL, &s_media_info.event_task, 0) != VK_OK )
	{
		HxLOG_Error("MW_MP: Init: Cannot create the event task for media player.\n");
		return ERR_FAIL;
	}

	if ( VK_TASK_Start(s_media_info.event_task) != VK_OK )
	{
		HxLOG_Error("MW_MP: Init: Cannot start the event task for media player.\n");
		return ERR_FAIL;
	}

#if 0
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)
	(void)SVC_MP_SI_Init();
#endif
#endif
	{
		// Temporary Code
		PAL_MEDIA_CONFIG_t config;
		config.bInit = TRUE;
		config.bUseStorage = TRUE;
		config.nChunkBaseExponent = 0;
		config.nStreamBufferingSecond = 0;
		PAL_MEDIA_Init(&config);
	}

	HxLOG_Print("MW_MP: Init: Media player is initialized succefully.\n");

	return ERR_OK;
}

HERROR SVC_MP_UnInit(void)
{
	media_player_manager_uninit();

	VK_MSG_Destroy(s_media_info.hmsg);

	s_media_info.task_flag = FALSE;

	VK_SEM_Destroy(s_media_info.lock);

	HxSTD_memset(&s_media_info.config, 0, sizeof(s_media_info.config));

	HxLOG_Print("MW_MP: Init: Media player is uninitialized succefully.\n");

	return ERR_OK;
}

void SVC_MP_GetDefaultConfig(SvcMp_Config_t *ptConfig)
{
	if ( ptConfig == NULL ) return;

#ifdef	CONFIG_MW_MM_PVR_INTERNAL
	// if there is internal harddisk in settop box.
	ptConfig->bProgressiveDownload = TRUE;
	HxSTD_memset(ptConfig->szPDLPath, 0, SVC_MP_MAX_URL_LEN);
	VK_strncpy(ptConfig->szPDLPath, SVC_MP_DEFAULT_HDD_BUFFER_PATH,
			VK_strlen(SVC_MP_DEFAULT_HDD_BUFFER_PATH));
#else
	ptConfig->bProgressiveDownload = FALSE;
	ptConfig->szPDLPath[0] = '\0';
#endif
}

HERROR SVC_MP_SetBrowserUserAgentData(HCHAR *szString)
{
	HUINT32 nUASize = strlen(szString) + 1;
	SVC_MP_ASSERT(szString);

	if(s_media_info.szBrowserUserAgentString  != NULL)
	{
		HLIB_STD_MemFree(s_media_info.szBrowserUserAgentString);
	}

	s_media_info.szBrowserUserAgentString = HLIB_STD_MemAlloc(nUASize);

	HxSTD_memset(s_media_info.szBrowserUserAgentString, 0x00, nUASize);
	VK_strncpy(s_media_info.szBrowserUserAgentString, szString, nUASize);

	return ERR_OK;
}

SvcMp_MediaPlayer_t *SVC_MP_CreatePlayer(HUINT32 hActionId)
{
	SvcMp_MediaPlayer_t *player = (SvcMp_MediaPlayer_t *)HLIB_STD_MemAlloc(sizeof(*player));
	if ( player == NULL )
	{
		HxLOG_Error("MW_MP: CreatePlayer: Insufficient memory.\n");
		return NULL;
	}

	HxSTD_memset(player, 0, sizeof(*player));

	player->phyMediaHandle = PAL_MEDIA_Create();
	if ( player->phyMediaHandle == NULL )
	{
		HxLOG_Error("MW_MP: CreatePlayer: Cannot create the PAL MEDIA instance.\n");
		HLIB_STD_MemFree(player);
		return NULL;
	}

	player->hActionId = hActionId;
	player->nSpeed = 100;
	player->ullStartTime = 0;
	player->userAgent = NULL;
	player->eState = eSVC_MP_STATE_IDLE;
	player->ePlayType = eSVC_MP_PLAYTYPE_NONE;
	player->bBufferingAutoMode = 0;
	player->nBufferingThreshold = 10000;
	player->next = NULL;

	media_player_manager_add(player);

	HxLOG_Print("MW_MP: CreatePlayer: A media player is created.(handle=%p)\n", player);
	HxLOG_Print("MW_MP: CreatePlayer: Total player count is %d\n",
		media_player_manager_get_count());

	PAL_MEDIA_RegisterMediaEventCallback(__mediaplayer_event_handler);

	return player;
}

HERROR SVC_MP_DestroyPlayer(SvcMp_MediaPlayer_t *player)
{
	SVC_MP_ASSERT(player);

	if ( player->eState != eSVC_MP_STATE_STOPPED )
	{
		HxLOG_Print("MW_MP: DestroyPlayer: the playback is not stopped and so the playback\
			will be stopped forcely.\n");
		SVC_MP_Stop(player);
	}

	if(s_media_info.szBrowserUserAgentString != NULL)
		HLIB_STD_MemFree(s_media_info.szBrowserUserAgentString);

	PAL_MEDIA_UnregisterMediaEventCallback();

	PAL_MEDIA_Destroy(__pal(player));

	if ( player->userAgent ) HLIB_STD_MemFree(player->userAgent);

	media_player_manager_del(player);

	HLIB_STD_MemFree(player);

	HxLOG_Print("MW_MP: DestroyPlayer: A media player is destroyed.\n");
	HxLOG_Print("MW_MP: DestroyPlayer: Total player count is %d\n",
		media_player_manager_get_count());

	return ERR_OK;
}

HUINT32 SVC_MP_GetActionId(SvcMp_MediaPlayer_t *player)
{
	return player->hActionId;
}

SvcMp_MediaPlayer_t *SVC_MP_GetActivePlayer(HUINT32 hActionId)
{
	return (SvcMp_MediaPlayer_t *)(media_player_manager_find_by_action(hActionId));
}

HERROR SVC_MP_Probe(SvcMp_MediaPlayer_t *player, SvcMp_PlayType_e ePlayType,
							const HINT8 *url)
{
	MW_MediaPlayerData_t *data = NULL;

	SVC_MP_ASSERT(player);

	if ( !__mediaplayer_is_file_content(url) )
	{
		__mediaplayer_send_event(player, eSVC_MP_EVENT_CONNECTING);
	}

	if ( !__mediaplayer_is_supported_uri(player, url) )
	{
		HxLOG_Error("MW_MP: Probe: NOT Supported URL=%s.\n", url);
		__mediaplayer_send_event(player, eSVC_MP_EVENT_ERROR);
		return ERR_FAIL;
	}

	data = HLIB_STD_MemAlloc(sizeof(MW_MediaPlayerData_t));
	if ( !data ) return ERR_FAIL;
	HxSTD_memset(data, 0, sizeof(*data));
	VK_strncpy(data->url, url, VK_strlen(url)+1);
	data->playType = ePlayType;

	__mediaplayer_send_command(player, eSVC_MP_LOCAL_COMMAND_PROBE, data);

	HxLOG_Print("MW_MP: SVC_MP_Probe: Started.\n");

	return ERR_OK;
}

HERROR SVC_MP_Peek(SvcMp_MediaPlayer_t *player, SvcMp_PlayType_e ePlayType,
							const HINT8 *url)
{
	MW_MediaPlayerData_t *data = NULL;

	SVC_MP_ASSERT(player);

	data = HLIB_STD_MemAlloc(sizeof(MW_MediaPlayerData_t));
	if ( !data ) return ERR_FAIL;

	HxSTD_memset(data, 0, sizeof(*data));
	VK_strncpy(data->url, url, VK_strlen(url)+1);
	data->playType = ePlayType;

	__mediaplayer_send_command(player, eSVC_MP_LOCAL_COMMAND_PEEK, data);

	HxLOG_Print("MW_MP: SVC_MP_Peek: Started.\n");

	return ERR_OK;
}

HERROR SVC_MP_Play(SvcMp_MediaPlayer_t *player, HUINT32 ulStartTime)
{
	SVC_MP_ASSERT(player);

	HxLOG_Print("MW_MP: SVC_MP_Play: Started.\n");

	player->ullStartTime = ulStartTime;

	__mediaplayer_send_command(player, eSVC_MP_LOCAL_COMMAND_PLAY, 0);

	return ERR_OK;
}

HERROR SVC_MP_Stop(SvcMp_MediaPlayer_t *player)
{
	HERROR			nResult = ERR_OK;

	SVC_MP_ASSERT(player);

	if(player->bhasSubtitle == TRUE)
	{
		__mediaplayer_send_subtitle(player, NULL,  sizeof(PAL_MEDIA_SUB_Display_Packet_t));
		player->bhasSubtitle = FALSE;
	}

	if(__pal(player)->tMediaInfo.isSuccesiveSeekMode == TRUE)
	{
		PAL_OUT_SetDecoderAudioMute(FALSE);
	}

	media_player_utils_PostMsgToMgr (eSVC_MP_EVENT_SBTL_STOP, PAL_PIPE_GetActionHandle (player->hActionId), 0, 0, 0);

	switch ( player->eState )
	{
		case eSVC_MP_STATE_SESSION_CREATED:
			__state(player, eSVC_MP_STATE_PROBE_CANCELED);
			PAL_MEDIA_ProbeCancel(__pal(player));
			nResult = ERR_OK;
			break;
		case eSVC_MP_STATE_PROBE_FAILED:
			nResult = ERR_OK;
			break;
		default:
			__mediaplayer_subtitle_close(player);
			__mediaplayer_send_event(player, eSVC_MP_EVENT_STOPPED);
			nResult = __mediaplayer_stop(player);
			break;
	}

	HxLOG_Print("MW_MP: Stop: Stopping the playback is completed.\n");

	return nResult;
}

HERROR SVC_MP_Seek(SvcMp_MediaPlayer_t *player, HUINT64 ullPosition_ms)
{
	HERROR eRet = ERR_OK;

	SVC_MP_ASSERT(player);

	if ( player->eState != eSVC_MP_STATE_PLAYING
		&& player->eState != eSVC_MP_STATE_SESSION_STARTED )
	{
		player->ullStartTime = ullPosition_ms;
		HxLOG_Print("MW_MP: Seek: Cannot change the play position on \"NOT playing\" state.\n");
		return ERR_OK;
	}

	//__state(player, eSVC_MP_STATE_SEEKING);
	eRet = PAL_MEDIA_SetPosition(__pal(player), ullPosition_ms);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: Seek: Cannot change the play position. (pos=%lld msec)\n",
			 ullPosition_ms);
		__mediaplayer_send_event(player, eSVC_MP_EVENT_SEEK_ERROR);
		__state(player, eSVC_MP_STATE_PLAYING);
		return ERR_FAIL;
	}
	//__state(player, eSVC_MP_STATE_PLAYING);

 	return ERR_OK;
}

HERROR SVC_MP_GetPlayUrl(SvcMp_MediaPlayer_t *player, HINT8 *pszUrlBuf)
{
	HxLOG_Print("MW_MP: GetPlayUrl\n");

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pszUrlBuf);

	VK_strncpy(pszUrlBuf, player->szUrl, VK_strlen(player->szUrl)+1);

	HxLOG_Print("MW_MP: GetPlayUrl: url = %s\n", pszUrlBuf);

	return ERR_OK;
}

HERROR SVC_MP_SetPlayUrl(SvcMp_MediaPlayer_t *player, const HINT8 *szUrl)
{
	HxLOG_Print("MW_MP: SetPlayUrl\n");

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(szUrl);

	HxSTD_memset(player->szUrl, 0, sizeof(player->szUrl));
	VK_strncpy(player->szUrl, szUrl, VK_strlen(szUrl)+1);

	HxLOG_Print("MW_MP: SetPlayUrl: url = %s\n", player->szUrl);

	return ERR_OK;
}

HERROR SVC_MP_GetPlayType(SvcMp_MediaPlayer_t *player,
							SvcMp_PlayType_e *pePlayType)
{
	HxLOG_Print("MW_MP: GetPlayType\n");

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pePlayType);

	*pePlayType = player->ePlayType;

	HxLOG_Print("MW_MP: GetPlayType: type = %d\n", *pePlayType);

	return ERR_OK;
}

HERROR SVC_MP_GetPlayPosition(SvcMp_MediaPlayer_t *player, HUINT64 *pullPosition_ms)
{
	HERROR eRet = ERR_OK;

	SVC_MP_ASSERT(player);

	if ( pullPosition_ms == NULL )
	{
		HxLOG_Error("MW_MP: GetPlayPosition: Invaild param.)\n");
		return ERR_FAIL;
	}

	if ( player->eState != eSVC_MP_STATE_PLAYING
		&& player->eState != eSVC_MP_STATE_SESSION_STARTED )
	{
		HxLOG_Error("MW_MP: GetPlayPosition: the playback is not started yet.)\n");
		return ERR_FAIL;
	}

	eRet = PAL_MEDIA_GetPosition(__pal(player), pullPosition_ms);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetPlayPosition: Cannot get the playback position.)\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_MP_SetPlayPosition(SvcMp_MediaPlayer_t *player, HUINT64 ullPosition_ms)
{
	return SVC_MP_Seek(player, ullPosition_ms);
}

HERROR SVC_MP_GetPlaySpeed(SvcMp_MediaPlayer_t *player, HINT32 *pnSpeed)
{
	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pnSpeed);

	*pnSpeed = player->nSpeed;

	HxLOG_Print("MW_MP: GetPlaySpeed: speed = %d\n", *pnSpeed);

	return ERR_OK;
}

HERROR SVC_MP_GetSupportedSpeeds(SvcMp_MediaPlayer_t *player, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray)
{
	HERROR eRet = ERR_OK;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pulSpeedNum);

	eRet = PAL_MEDIA_GetSupportedSpeeds(__pal(player), pulSpeedNum, ppnSpeedArray);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetSupportedSpeeds: Cannot get the speed array.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_MP_SetPlaySpeed(SvcMp_MediaPlayer_t *player, HINT32 nSpeed)
{
	HERROR eRet = ERR_OK;

	HxLOG_Print("MW_MP: SetPlaySpeed (speed=%d)\n", nSpeed);

	SVC_MP_ASSERT(player);

	if ( player->eState != eSVC_MP_STATE_PLAYING
		&& player->eState != eSVC_MP_STATE_SESSION_STARTED )
	{
		__mediaplayer_send_event(player, eSVC_MP_EVENT_PLAY_SPEED_CHANGED);
		player->nSpeed = nSpeed;
		return ERR_OK;
	}

	if ( nSpeed != 100 )
	{
		PAL_AUDIO_SetHide(eAvHideClient_Decoder, TRUE);
		if(__pal(player)->tMediaInfo.isSuccesiveSeekMode == TRUE)
		{
		PAL_OUT_SetDecoderAudioMute(TRUE);
		}
	}
	else
	{
		PAL_AUDIO_SetHide(eAvHideClient_Decoder, FALSE);
		if(__pal(player)->tMediaInfo.isSuccesiveSeekMode == TRUE)
		{
		PAL_OUT_SetDecoderAudioMute(FALSE);
		}
	}

	eRet = PAL_MEDIA_SetSpeed(__pal(player), nSpeed);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: SetPlaySpeed: Cannot change the play speed.\n");
		if(100 == player->nSpeed)
		{
		PAL_OUT_SetDecoderAudioMute(FALSE);
		}
		__mediaplayer_send_event(player, eSVC_MP_EVENT_PLAY_SPEED_CHANGED);
		return ERR_FAIL;
	}

	if ( nSpeed == 0 ) /* pause */
	{
		HxLOG_Print("MW_MP: SetPlaySpeed: the playback will be paused.\n");
		__mediaplayer_send_event(player, eSVC_MP_EVENT_PAUSE);
	}
	else
	{
		__mediaplayer_send_event(player, eSVC_MP_EVENT_PLAYING);
		HxLOG_Print("MW_MP: SetPlaySpeed: the playback speed will be changed to %d\n",
			nSpeed);
	}
	__mediaplayer_send_event(player, eSVC_MP_EVENT_PLAY_SPEED_CHANGED);

	player->nSpeed = nSpeed;

	return ERR_OK;
}

HERROR SVC_MP_GetDuration(SvcMp_MediaPlayer_t *player, HUINT64 *pullDuration_ms)
{
	HERROR eRet = ERR_OK;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pullDuration_ms);

	if ( player->eState == eSVC_MP_STATE_IDLE
			|| player->eState == eSVC_MP_STATE_SESSION_CREATED
			|| player->eState == eSVC_MP_STATE_PROBE_CANCELED )
	{
		*pullDuration_ms = 0;
		return ERR_OK;
	}

	eRet = PAL_MEDIA_GetDuration(__pal(player), pullDuration_ms);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetDuration: Cannot get the duration of content\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_MP_GetPlayState(SvcMp_MediaPlayer_t *player, SvcMp_Event_e *peEvent)
{
	HxLOG_Print("MW_MP: GetPlayState\n");

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(peEvent);

	*peEvent = player->eState;

	return ERR_OK;
}

HERROR SVC_MP_GetDrmStatus(SvcMp_MediaPlayer_t *player, HBOOL *pbDrm)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_INFO_t tMediaInfo;

	HxLOG_Print("MW_MP: GetDrmStatus\n");

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pbDrm);

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetDrmStatus: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	*pbDrm = ( tMediaInfo.is_drm ) ? TRUE : FALSE;

	HxLOG_Print("MW_MP: GetDrmStatus: status = %s\n", (*pbDrm) ? "true" : "false");

	return ERR_OK;
}

HINT32 SVC_MP_GetVideoTrackCount(SvcMp_MediaPlayer_t *player)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_INFO_t tMediaInfo;

	SVC_MP_ASSERT(player);

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetVideoTrackCount: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	return tMediaInfo.ucTotalVideoTrack;
}

HERROR SVC_MP_GetVideoTrack(SvcMp_MediaPlayer_t *player, HINT32 nIndex,
					SvcMp_VideoTrack_t *pVideoTrack)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_INFO_t tMediaInfo;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pVideoTrack);

	if ( nIndex < 0 || nIndex >= PAL_MEDIA_MAX_VIDEO_TRACK )
	{
		HxLOG_Error("MW_MP: GetVideoTrack: Invaild index (%d)\n", nIndex);
		return ERR_FAIL;
	}

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetVideoTrack: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pVideoTrack, 0, sizeof(*pVideoTrack));

	pVideoTrack->ulVideoStreamId = tMediaInfo.stVideoTrack[nIndex].video_stream_id;
	pVideoTrack->eVideoCodec = tMediaInfo.stVideoTrack[nIndex].video_codec;
	pVideoTrack->ulVideoPesId = tMediaInfo.stVideoTrack[nIndex].video_pes_id;

	return ERR_OK;
}

HERROR SVC_MP_GetActiveVideoTrack(SvcMp_MediaPlayer_t *player,
					SvcMp_VideoTrack_t *pVideoTrack)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_INFO_t tMediaInfo;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pVideoTrack);

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetActiveVideoTrack: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pVideoTrack, 0, sizeof(*pVideoTrack));

	pVideoTrack->ulVideoStreamId = tMediaInfo.video_stream_id;
	pVideoTrack->eVideoCodec = tMediaInfo.video_codec;
	pVideoTrack->ulVideoPesId = tMediaInfo.video_pes_id;

	return ERR_OK;
}

HERROR SVC_MP_SetActiveVideoTrack(SvcMp_MediaPlayer_t *player,
					const SvcMp_VideoTrack_t *pVideoTrack)
{
	SVC_MP_ASSERT(player);

	return ERR_OK;
}

HINT32 SVC_MP_GetAudioTrackCount(SvcMp_MediaPlayer_t *player)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_INFO_t tMediaInfo;

	HxLOG_Print("MW_MP: GetAudioTrackCount\n");

	SVC_MP_ASSERT(player);

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetDrmStatus: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	return tMediaInfo.ucTotalAudioTrack;
}

HERROR SVC_MP_GetAudioTrack(SvcMp_MediaPlayer_t *player, HINT32 nIndex,
					SvcMp_AudioTrack_t *pAudioTrack)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_INFO_t tMediaInfo;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pAudioTrack);

	if ( nIndex < 0 || nIndex >= PAL_MEDIA_MAX_AUDIO_TRACK )
	{
		HxLOG_Error("MW_MP: GetAudioTrack: Invaild index (%d)\n", nIndex);
		return ERR_FAIL;
	}

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetAudioTrack: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pAudioTrack, 0, sizeof(*pAudioTrack));

	pAudioTrack->ulAudioStreamId = tMediaInfo.stAudioTrack[nIndex].audio_stream_id;
	pAudioTrack->eAudioCodec = tMediaInfo.stAudioTrack[nIndex].audio_codec;
	pAudioTrack->ulAudioPesId = tMediaInfo.stAudioTrack[nIndex].audio_pes_id;
	HLIB_STD_StrNCpySafe(pAudioTrack->szAudioLanguage, tMediaInfo.stAudioTrack[nIndex].uacAudioLang,
		sizeof(pAudioTrack->szAudioLanguage));

	return ERR_OK;
}

HERROR SVC_MP_GetActiveAudioTrack(SvcMp_MediaPlayer_t *player,
					SvcMp_AudioTrack_t *pAudioTrack)
{
	HERROR eRet = ERR_OK;
	HINT32 i = 0;
	PAL_MEDIA_INFO_t tMediaInfo;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pAudioTrack);

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetActiveAudioTrack: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pAudioTrack, 0, sizeof(*pAudioTrack));

	pAudioTrack->ulAudioStreamId = tMediaInfo.audio_stream_id;
	pAudioTrack->eAudioCodec = tMediaInfo.audio_codec;
	pAudioTrack->ulAudioPesId = tMediaInfo.audio_pes_id;

	for ( i=0; i<tMediaInfo.ucTotalAudioTrack; ++i )
	{
		if ( pAudioTrack->ulAudioStreamId == tMediaInfo.stAudioTrack[i].audio_stream_id )
		{
			HxSTD_memcpy(pAudioTrack->szAudioLanguage, tMediaInfo.stAudioTrack[i].uacAudioLang,
					VK_strlen(tMediaInfo.stAudioTrack[i].uacAudioLang)+1);
			break;
		}
	}

	return ERR_OK;
}

HERROR SVC_MP_SetActiveAudioTrack(SvcMp_MediaPlayer_t *player, const SvcMp_AudioTrack_t *pAudioTrack)
{
	HERROR eRet = ERR_OK;
	HINT32 i = 0;
	PAL_MEDIA_INFO_t tMediaInfo;
	HUINT8 ucAudioTrack = 0;

	SVC_MP_ASSERT(player);

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetActiveAudioTrack: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	for ( i=0; i<tMediaInfo.ucTotalAudioTrack; ++i )
	{
		if ( pAudioTrack->ulAudioStreamId == tMediaInfo.stAudioTrack[i].audio_stream_id )
		{
			ucAudioTrack = i;
			break;
		}
	}

	eRet = PAL_MEDIA_SetAudioTrack(__pal(player), ucAudioTrack);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: __mediaplayer_probe: SetUserAgentString failed.\n");
	}

	return ERR_OK;
}

HERROR SVC_MP_GetAudioStereoConfig(SvcMp_MediaPlayer_t *player, DxStereoSelect_e *eStereoConfig)
{
	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(eStereoConfig);

	return ERR_OK;
}

HERROR SVC_MP_GetStereoType(SvcMp_MediaPlayer_t *player, DxStereoFormat_e *peStereoType)
{
	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(peStereoType);

	return ERR_OK;
}

HERROR SVC_MP_GetStreamType(SvcMp_MediaPlayer_t *player, eMEDIA_StreamType_t *peStreamType)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_INFO_t tMediaInfo;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(peStreamType);

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetStreamType: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	*peStreamType = tMediaInfo.stream_type;

	return ERR_OK;
}

HERROR SVC_MP_GetVideoWindow(SvcMp_MediaPlayer_t *player, SvcMp_VideoWindow_t *pWindow)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_INFO_t tMediaInfo;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pWindow);

	eRet = PAL_MEDIA_GetProbeInfo(__pal(player), &tMediaInfo);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("MW_MP: GetVideoWindow: Cannot get the stream info\n");
		return ERR_FAIL;
	}

	pWindow->width = tMediaInfo.usVideoWidth;
	pWindow->height = tMediaInfo.usVideoHeight;

	return ERR_OK;
}

HERROR SVC_MP_GetBufferingProgress(SvcMp_MediaPlayer_t *player, HUINT32 *pulProgress)
{
	SVC_MP_ASSERT(player);

	return ERR_FAIL;
}

HERROR __mediaplayer_get_bufferingChunkInfo(SvcMp_MediaPlayer_t *player,
							SvcMp_BufferInfo_t *ptChunkInfo)
{
	HERROR eRet = ERR_OK;
	SvcMp_BufferInfo_t tChunkInfo;
	PAL_MEDIA_BufferStatus_t tBufferStatus;

	SVC_MP_ASSERT(player);

	if ( !__mediaplayer_is_pdl(player) ) return ERR_OK;

	if ( player->eState == eSVC_MP_STATE_STOPPED
			|| player->eState == eSVC_MP_STATE_IDLE
			|| player->eState == eSVC_MP_STATE_PROBE_CANCELED
			|| player->eState == eSVC_MP_STATE_SESSION_CREATED )
	{
		return ERR_FAIL;
	}

	HxSTD_memset(&tChunkInfo, 0x00, sizeof(SvcMp_BufferInfo_t));
	eRet = PAL_MEDIA_GetBufferingChunkInfo(__pal(player), &(tChunkInfo.ulChunks), &tChunkInfo.ullStartTime, &tChunkInfo.ullEndTime);
	if ( eRet != ERR_OK  ) /* || (tChunkInfo.ulChunks == 0)) */
	{
		HxLOG_Error("MW_MP: SVC_MP_GetBufferingChunkInfo: Cannot get the BufferingChunks info\n");
		return ERR_FAIL;
	}

	ptChunkInfo->ulChunks = tChunkInfo.ulChunks;
	ptChunkInfo->ullStartTime = tChunkInfo.ullStartTime;
	ptChunkInfo->ullEndTime = tChunkInfo.ullEndTime;

	HxSTD_memset(&tBufferStatus, 0x00, sizeof(tBufferStatus));
	eRet = PAL_MEDIA_GetBufferingStatus(__pal(player), &tBufferStatus);
	if ( eRet != ERR_OK  )
	{
		HxLOG_Error("MW_MP: SVC_MP_GetBufferingChunkInfo: Cannot get the BufferingStatus info\n");
		return ERR_FAIL;
	}

	ptChunkInfo->ulBufferRemainedTime = tBufferStatus.uiRemainBufferSec;
	ptChunkInfo->ulBufferPercent = tBufferStatus.uiBufferPercentage;

	return ERR_OK;
}

HERROR SVC_MP_SetBufferingScenario(SvcMp_MediaPlayer_t *player, HBOOL bAuto, HINT32 nThreshold)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_BufferingModeParams_t param;

	HxLOG_Print("MW_MP: SVC_MP_SetBufferingScenario\n");

	SVC_MP_ASSERT(player);

	HxSTD_memset(&param, 0, sizeof(param));
	param.bAuto = bAuto;
	param.nBufferThreshold = nThreshold;

	eRet = PAL_MEDIA_SetBufferingMode(__pal(player), &param);
	if( eRet != ERR_OK )
	{
		return ERR_FAIL;
	}

	player->bBufferingAutoMode = bAuto;
	player->nBufferingThreshold = nThreshold;

	return ERR_OK;
}

HERROR SVC_MP_GetWebUserAgent(SvcMp_MediaPlayer_t *player, HINT8 *pszUserAgent)
{
	HxLOG_Print("MW_MP: GetWebUserAgent\n");

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pszUserAgent);

	VK_strncpy(pszUserAgent, player->userAgent, VK_strlen(player->userAgent)+1);

	HxLOG_Print("MW_MP: GetWebUserAgent: UA string = %s\n", pszUserAgent);

	return ERR_OK;
}

HERROR SVC_MP_SetWebUserAgent(SvcMp_MediaPlayer_t *player, const HINT8 *szUserAgent)
{
	HxLOG_Print("MW_MP: SetWebUserAgent\n");

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(szUserAgent);

	if ( player->userAgent ) HLIB_STD_MemFree(player->userAgent);

	player->userAgent = (HINT8 *)HLIB_STD_MemAlloc(VK_strlen(szUserAgent)+1);
	if ( player->userAgent == NULL )
	{
		HxLOG_Error("MW_MP: SetWebUserAgent: Insufficient memory.\n");
		return ERR_FAIL;
	}
	VK_strncpy(player->userAgent, szUserAgent, VK_strlen(szUserAgent)+1);

	HxLOG_Print("MW_MP: SetWebUserAgent: UA string.\n", player->userAgent);

	return ERR_OK;
}

HERROR SVC_MP_CaptureScreen(SvcMp_MediaPlayer_t *player, const HINT8 *szSavePath)
{
	HERROR eRet = ERR_OK;
	HUINT32 ulVideoId = 0;
	PalVideo_FrameBuffer_t	stFrame;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(szSavePath);

	eRet = PAL_PIPE_GetResourceId(player->hActionId, eRxRSCTYPE_AV_DEC, &ulVideoId);
	if( eRet != ERR_OK )
	{
		return ERR_FAIL;
	}

	stFrame.ulWidth = SVC_MP_CAPTURE_WIDTH;
	stFrame.ulHeight = SVC_MP_CAPTURE_HEIGHT;
	stFrame.pFrameBuffer =\
		 (HUINT32 *)HLIB_STD_MemAlloc(SVC_MP_CAPTURE_WIDTH*SVC_MP_CAPTURE_HEIGHT*sizeof(HUINT32));
	if( stFrame.pFrameBuffer == NULL )
	{
		return ERR_FAIL;
	}

	eRet = PAL_VIDEO_CreateVideoFrameCapture(ulVideoId,
				SVC_MP_CAPTURE_WIDTH, SVC_MP_CAPTURE_HEIGHT, &stFrame);
	if( eRet != ERR_OK )
	{
		HLIB_STD_MemFree(stFrame.pFrameBuffer);
		return ERR_FAIL;
	}

	eRet = media_player_utils_make_png((HUINT8 *)&stFrame, szSavePath);
	if( eRet != ERR_OK )
	{
		HLIB_STD_MemFree(stFrame.pFrameBuffer);
		return ERR_FAIL;
	}

	HLIB_STD_MemFree(stFrame.pFrameBuffer);

	return ERR_OK;
}

HERROR SVC_MP_SetSubtitleDisplay(SvcMp_MediaPlayer_t *player, HBOOL bEnable)
{
	HERROR eRet = ERR_OK;

	SVC_MP_ASSERT(player);

	eRet = PAL_MEDIA_SUB_SetDisplay(__pal(player), bEnable);
	if(eRet != ERR_OK)
	{
		HxLOG_Error("MW_MP: SVC_MP_SetSubtitleDisplay: \n");
		return ERR_FAIL;
	}
	if(bEnable == FALSE)
		 __mediaplayer_send_subtitle(player, NULL,  sizeof(PAL_MEDIA_SUB_Display_Packet_t));

	return ERR_OK;
}

HERROR SVC_MP_SetSubtitleLanguage(SvcMp_MediaPlayer_t *player, HUINT32 ulLangIdx)
{
	HERROR eRet = ERR_OK;

	SVC_MP_ASSERT(player);

	eRet = PAL_MEDIA_SUB_SetLanguage(__pal(player), ulLangIdx);
	if(eRet != ERR_OK)
	{
		HxLOG_Error("MW_MP: SVC_MP_SetSubtitleLanguage: \n");
		return ERR_FAIL;
	}


	return ERR_OK;
}

HERROR SVC_MP_ChangeSubtitleSyncTime(SvcMp_MediaPlayer_t *player, HINT32 ulMsTime)
{
	HERROR eRet = ERR_OK;

	SVC_MP_ASSERT(player);

	/* TODO - ulMsTime Check */

	eRet = PAL_MEDIA_SUB_ChangeSyncOffsetTime(__pal(player), ulMsTime);
	if(eRet != ERR_OK)
	{
		HxLOG_Error("MW_MP: SVC_MP_ChangeSubtitleSyncTime: \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_MP_GetSubtitleDisplay(SvcMp_MediaPlayer_t *player, HBOOL *pbShow)
{
	HERROR eRet = ERR_OK;
	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pbShow);

	eRet = PAL_MEDIA_SUB_GetDisplay(__pal(player), pbShow);
	if( eRet != ERR_OK)
	{
		HxLOG_Error("MW_MP: SVC_MP_GetSubtitleDisplay: Cannot get the Subtitle info\n");
		*pbShow = 0;
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR SVC_MP_GetSubtitleTotalLangCnt(SvcMp_MediaPlayer_t *player, HUINT32 *pnCount)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_SUB_LangInfo_t stLangInfo;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pnCount);

	HxSTD_memset(&stLangInfo, 0, sizeof(stLangInfo));
	eRet = PAL_MEDIA_SUB_GetLanguageInfo(__pal(player), &stLangInfo);
	if( eRet != ERR_OK)
	{
		HxLOG_Error("MW_MP: SVC_MP_GetSubtitleLanguageCount: Cannot get the Subtitle info\n");
		*pnCount = 0;
		return ERR_FAIL;
	}

	*pnCount = stLangInfo.ulTotalLang;

	return ERR_OK;
}

HERROR SVC_MP_GetSubtitleCurrLangIndex(SvcMp_MediaPlayer_t *player, HUINT32 *pnIndex)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_SUB_LangInfo_t stLangInfo;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(pnIndex);

	HxSTD_memset(&stLangInfo, 0, sizeof(stLangInfo));
	eRet = PAL_MEDIA_SUB_GetLanguageInfo(__pal(player), &stLangInfo);
	if( eRet != ERR_OK)
	{
		HxLOG_Error("MW_MP: SVC_MP_GetSubtitleLanguageCount: Cannot get the Subtitle info\n");
		return ERR_FAIL;
	}

	*pnIndex = stLangInfo.ulCurIdx;

	return ERR_OK;
}

HERROR SVC_MP_GetSubtitleLanguageByIndex(SvcMp_MediaPlayer_t *player, HINT32 nIndex, HINT8 *szLang)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_SUB_LangInfo_t stLangInfo;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(szLang);

	HxSTD_memset(&stLangInfo, 0, sizeof(stLangInfo));
	eRet = PAL_MEDIA_SUB_GetLanguageInfo(__pal(player), &stLangInfo);
	if( eRet != ERR_OK)
	{
		HxLOG_Error("MW_MP: SVC_MP_GetSubtitleLanguageInfo: Cannot get the Subtitle info\n");
		return ERR_FAIL;
	}

	if(nIndex > stLangInfo.ulTotalLang)
	{
		HxLOG_Error("MW_MP: SVC_MP_GetSubtitleLanguageInfo: Invalid Subtitle Index\n");
		return ERR_FAIL;
	}

	//memcpy(szLang, &stLangInfo.szName[nIndex], 16);
	memcpy(szLang, &stLangInfo.szDispLang[nIndex], 16);

	return ERR_OK;
}

HERROR SVC_MP_GetSubtitleCurSyncTime(SvcMp_MediaPlayer_t *player, HINT32 *ulMsTime)
{
	HERROR eRet = ERR_OK;

	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(ulMsTime);

	eRet = PAL_MEDIA_SUB_GetCurSyncOffsetTime(__pal(player), ulMsTime);
	if(eRet != ERR_OK)
	{
		HxLOG_Error("MW_MP: SVC_MP_GetSubtitleCurSyncTime: \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_MP_SetSubtitleFontSize(SvcMp_MediaPlayer_t *player, HUINT32 ulSize)
{
	SVC_MP_ASSERT(player);
	if(ulSize == 0)
		player->ulSubtitleFontSize = SVC_MP_SUB_DEFAULT_FONT_SIZE;
	else
		player->ulSubtitleFontSize = ulSize;

	return ERR_OK;
}

HERROR SVC_MP_GetSubtitleFontSize(SvcMp_MediaPlayer_t *player, HUINT32 *ulSize)
{
	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(ulSize);

	*ulSize = player->ulSubtitleFontSize;

	return ERR_OK;
}

HERROR SVC_MP_SetSubtitleDisplayPosition(SvcMp_MediaPlayer_t *player, HUINT32 ulPos)
{
	SVC_MP_ASSERT(player);
	if(ulPos == 0)
		player->ulSubtitleDisplayPosition = SVC_MP_SUB_DEFAULT_TEXT_POSITON;
	else
		player->ulSubtitleDisplayPosition = ulPos;

	return ERR_OK;
}

HERROR SVC_MP_GetSubtitleDisplayPosition(SvcMp_MediaPlayer_t *player, HUINT32 *ulPos)
{
	SVC_MP_ASSERT(player);
	SVC_MP_ASSERT(ulPos);

	*ulPos = player->ulSubtitleDisplayPosition;

	return ERR_OK;
}

HERROR SVC_MP_SetDrmLicenseData(SvcMp_MediaPlayer_t *player, HCHAR *szString)
{
	return PAL_MEDIA_SetDrmLicenseData(__pal(player), szString, VK_strlen(szString));
}


SvcMp_Error_e SVC_MP_GetLastErrorReason(SvcMp_MediaPlayer_t *player)
{
	SVC_MP_ASSERT(player);
	return PAL_MEDIA_GetLastErrorReason(__pal(player));
}

HERROR SVC_MP_SetTLS(SvcMp_MediaPlayer_t *player, SvcMp_VerifyExternalCallback_t *VerifyExternalCb)
{
	SVC_MP_ASSERT(player);

	return PAL_MEDIA_SetTLS(__pal(player), (PAL_MEDIA_VerifyExternalCallback_t *)VerifyExternalCb);
}

void SVC_MP_SendCommand(SvcMp_MediaPlayer_t *player, SvcMp_Command_e command, HUINT32 p1)
{
	__mediaplayer_send_command(player, command, p1);
}

HERROR SVC_MP_UpdateUserSelectAudioTrackInfo(SvcMp_MediaPlayer_t *mplayer, HINT32 nIndex)
{
	return ERR_OK;
}

HERROR SVC_MP_GetAudioDualMonoConfig(SvcMp_MediaPlayer_t *mplayer, DxDualMonoSelect_e *eDualMonoConfig)
{
	HERROR 					hErr = ERR_FAIL;
	DxStereoSelect_e 	eStereoConfig = eDxSTEREO_SELECT_UNKNOWN;

	hErr = SVC_MP_GetAudioStereoConfig(mplayer, &eStereoConfig);
	if (hErr != ERR_OK)
	{
		HxLOG_Print("SVC_MP_GetAudioDualMonoConfig() fail!!\n");
		return ERR_FAIL;
	}

	switch (eStereoConfig)
	{
		case eDxSTEREO_SELECT_MONOLEFT:
			*eDualMonoConfig = eDxDUALMONO_SELECT_LEFT;
			break;

		case eDxSTEREO_SELECT_MONORIGHT:
			*eDualMonoConfig = eDxDUALMONO_SELECT_RIGHT;
			break;

		default:
			*eDualMonoConfig = eDxDUALMONO_SELECT_BOTH;
			break;
	}

	return ERR_OK;
}

HERROR SVC_MP_SetAudioDualMonoConfig(SvcMp_MediaPlayer_t *mplayer, DxDualMonoSelect_e eDualMonoConfig)
{
	DxStereoSelect_e eStereoConfig = eDxSTEREO_SELECT_UNKNOWN;
	HERROR eRet = ERR_OK;

	switch (eDualMonoConfig)
	{
		case eDxDUALMONO_SELECT_LEFT:
			eStereoConfig = eDxSTEREO_SELECT_MONOLEFT;
			break;

		case eDxDUALMONO_SELECT_RIGHT:
			eStereoConfig = eDxSTEREO_SELECT_MONORIGHT;
			break;

		default:
			eStereoConfig = eDxSTEREO_SELECT_STEREO;
			break;
	}

	SVC_MP_ASSERT(mplayer);

	/* Stereo설정은 Playing중일때만 처리 */
	if ((mplayer->eState == eSVC_MP_STATE_PROBE_FINISHED) ||
		(mplayer->eState == eSVC_MP_STATE_PLAYING) ||
		(mplayer->eState == eSVC_MP_STATE_SESSION_STARTED))
	{
		eRet = PAL_MEDIA_SetAudioStereoConfig(__pal(mplayer), eStereoConfig);
		if ( eRet != ERR_OK )
		{
			HxLOG_Error("MW_MP:SetAudioStereoConfig: Cannot get the stream info\n");
			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Error("MW_MP:SetAudioStereoConfig: Playback is not started. state=%d eStereoConfig=%d\n", mplayer->eState, eStereoConfig);
	}

	return ERR_OK;
}

HERROR SVC_MP_GetBufferingChunkInfo(SvcMp_MediaPlayer_t *mplayer, SvcMp_BufferInfo_t *pInfo)
{

	HERROR hErr = ERR_FAIL;
	SvcMp_BufferInfo_t tChunkInfo;

	VK_MEM_Memset(&tChunkInfo, 0, sizeof(SvcMp_BufferInfo_t));
	hErr = __mediaplayer_get_bufferingChunkInfo(mplayer, &tChunkInfo);

	//HLIB_CMD_Printf("[%s:%d] Chunks(%d) StartOffsets(%p) EndOffsets(%p)\n", __FUNCTION__, __LINE__, tChunkInfo.ulChunks, tChunkInfo.ullStartTime, tChunkInfo.ullEndTime);
	if(hErr != ERR_OK)
{
		HxLOG_Print("AP_MPlayer_GetBufferingInfo() fail!!\n");
		return ERR_FAIL;
	}

	pInfo->ulChunks = tChunkInfo.ulChunks;
	pInfo->ullStartTime = tChunkInfo.ullStartTime;
	pInfo->ullEndTime  = tChunkInfo.ullEndTime;
	pInfo->ulBufferRemainedTime= tChunkInfo.ulBufferRemainedTime;
	pInfo->ulBufferPercent= tChunkInfo.ulBufferPercent;

	return ERR_OK;
}

HERROR SVC_MP_GetOutputControlInfo(SvcMp_MediaPlayer_t *mplayer, SvcMp_OutputControl_t *control)
{
	HERROR eRet = ERR_OK;
	PAL_MEDIA_OutputControl_t outputControl;

	SVC_MP_ASSERT(mplayer);
	SVC_MP_ASSERT(control);

	eRet = PAL_MEDIA_GetOutputControlInfo(__pal(mplayer), &outputControl);
	if ( eRet != ERR_OK )
	{
		return ERR_FAIL;
	}

	control->bHdcp = outputControl.bHdcp;
	control->bScms= outputControl.bScms;
	control->bCgms= outputControl.bCgms;
	control->bMacrovision= outputControl.bMacrovision;

	return ERR_OK;
}



