/** **********************************************************************************************************
  @file 		svc_cso.c

  @date		2013/02/22
  @author		IPM-SW2T (huyang@humaxdigital.com)
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


#include "svc_mplayercso.h"
//#include "mwc_util.h"
//#include "pal_media.h"
#include "pal_audio.h"
#include "pal_video.h"
#include "pal_output.h"
#include "pal_scaler.h"
#include "pal_pipe.h"
#include <di_video.h>
#include <di_audio.h>
#include <di_err.h>
#include "di_calypso.h"
#include "di_media_types.h"
#include "di_pvr.h"

#include "octo_config.h"
#include "get_enum_str.h"
#include "_svc_mp_utils.h"
#include "_svc_cso_sub.h"

#include "vkernel.h"
#include "hlib.h"

#include <string.h>
#include <pal_datastream.h>
#include <di_demux.h>
#ifdef CONFIG_SUPPORT_ROVI /* for ROVI Cert */
#include <svc_si.h>
#include <svc_output.h>
#endif


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
#define SVC_CSO_ASSERT(x)		\
	if ( x == NULL )\
{\
	HxLOG_Error("SVC_CSO: the param is NULL.\n");\
	return ERR_FAIL;\
}\

#define ENTRY   HxLOG_Print("%s:%s: ++\n", __FILE__, __FUNCTION__)
#define EXIT   HxLOG_Print("%s:%s: --\n", __FILE__, __FUNCTION__)

#define SVC_CSO_FOREVER					1

#define SVC_CSO_EVENT_TASK_NAME			"SVC_CSOEventTask"
#define SVC_CSO_EVENT_TASK_PRIORITY		55
#define SVC_CSO_EVENT_TASK_STACK_SIZE	(128*1024)

#define SVC_CSO_QUEUE_NAME				"SVC_CSOEvent"
#define SVC_CSO_QUEUE_SIZE				32
#define SVC_CSO_COMMAND_LIMIT			2
#define SVC_CSO_SEMAPHORE_NAME			"SVC_CSOSem"

#define SVC_CSO_CAPTURE_WIDTH			140
#define SVC_CSO_CAPTURE_HEIGHT			78

#define SVC_CSO_SUB_DEFAULT_FONT_SIZE	33
#define SVC_CSO_SUB_DEFAULT_TEXT_POSITON 600

#define SVC_CSO_NUMBER_OF_RESOURCE		NUM_VIEW_ACTION /* THIS SHOULD BE SAME AS VIEW NUMBERS*/
#define SVC_CSO_DEFAULT_HDD_BUFFER_PATH	"dvr:///mnt/hd3/download"

#define SVC_CSO_NETFLIX_START_AUDIO_PID	(0x101)	/* For Netflix */

/* supported URIs */
#define SUPPORTED_URI_HTTP 				"http:"
#define SUPPORTED_URI_HTTPS 			"https:"
#define SUPPORTED_URI_MMS 				"mms:"
#define SUPPORTED_URI_RTP 				"rtp:"
#define SUPPORTED_URI_VTUNER 			"vtuner:"
#define SUPPORTED_URI_FILE 				"file:"
#define SUPPORTED_URI_DLNA 				"hdlna://"

/* For Audio Ease function */
#define SVC_CSO_AUDIOFADE_SEMAPHORE_NAME	"SVC_CSOAudioFadeSem"

#define SVC_CSO_AUDIOFADE_QUEUE_NAME		"SVC_CSOAudioFadeQueue"
#define SVC_CSO_AUDIOFADE_QUEUE_SIZE		(32)

#define SVC_CSO_AUDIOFADE_TASK_NAME			"SVC_CSOAudioFadeTask"
#define SVC_CSO_AUDIOFADE_TASK_PRIORITY		VK_TASK_PRIORITY_MW_REF
#define SVC_CSO_AUDIOFADE_TASK_STACK_SIZE	SIZE_16K
#define SVC_CSO_AUDIOFADE_TASK_SLEEP		(100)

#define AUDIOFADE_SEMA_LOCK		VK_SEM_Get(stAudioFade.ulLockId)
#define AUDIOFADE_SEMA_UNLOCK	VK_SEM_Release(stAudioFade.ulLockId)

/*---------------------------------------------------------------------------------
 * typedef
 *---------------------------------------------------------------------------------*/

typedef struct _SvcCso_Info_t {
	SvcCso_Config_t		config;
	HULONG						hmsg;
	HULONG						event_task;
	HULONG						lock;
	HBOOL						task_flag;
	HULONG						state_task;
} SvcCso_Info_t;

typedef struct _SvcCso_EventInfo_t {
	SvcCso_t			*player;
	SvcCso_Command_e		command;
	SvcCso_Local_Event_e			event;
	HUINT32						param;
	SvcCso_EventType_e local_event;
} SvcCso_EventInfo_t;

typedef struct _SvcCso_Data_t {
	HINT8					url[SVC_CSO_MAX_URL_LEN];
} SvcCso_Data_t;

typedef struct _SvcCso_EvtCaller_t {
	HBOOL	bHasCaller;
	HINT32	nVideoCaller;
	HINT32	nAudioCaller;
} SvcCso_EvtCaller_t;

typedef struct _SvcCso_AudioFade_t {
	HULONG	ulMsgId;
	HULONG	ulTaskId;
	HULONG	ulLockId;
	HBOOL	bRunning;
	SvcCso_Status_AudioFade_e eStatus;
	HUINT32 unVolume;	/* Current audio decoder volume */
} SvcCso_AudioFade_t;

typedef struct _SvcCso_Info_AudioFade_t {
	HUINT32 unStartVolume;
	HUINT32 unTargetVolume;
	HINT32 nEase;
	HINT32 nEaseTimeElapsed;
	HINT32 nEaseTransitionFrames;
} SvcCso_Info_AudioFade_t;

typedef struct _SvcCso_Msg_AudioFade_t {
	SvcCso_t *player;
	SvcCso_Event_AudioFade_e eEvent;
	SvcCso_Info_AudioFade_t stInfo;
} SvcCso_Msg_AudioFade_t;

/* Define Audio Decoder Type */
typedef enum {
	eSVC_CSO_LOCAL_AUDIO_DECODER_MAIN,       // Main decoder (MPEG, AC3 downmix .... )
	eSVC_CSO_LOCAL_AUDIO_DECODER_SUB,        // Audio Description
	eSVC_CSO_LOCAL_AUDIO_DECODER_SPDIF,      // AC3 bypass
	eSVC_CSO_LOCAL_AUDIO_DECODER_MAX,
} SvcCso_Local_AudioDecoder_Type_e;

/* Define video Decoder Type */
typedef enum {
	eSVC_CSO_LOCAL_VIDEO_DECODER_MAIN,       // Main decoder
	eSVC_CSO_LOCAL_VIDEO_DECODER_SUB,        // Sub decoder
	eSVC_CSO_LOCAL_VIDEO_DECODER_MAX,
} SvcCso_Local_VideoDecoder_Type_e;


/*---------------------------------------------------------------------------------
  static variables
 *---------------------------------------------------------------------------------*/
static SvcCso_Info_t		s_media_info;
static SvcCso_AudioFade_t	stAudioFade;

static HBOOL b_cso_mdeia_initialized = FALSE;

static SvcCso_EvtCaller_t s_tPlayerEventCaller[SVC_CSO_NUMBER_OF_RESOURCE] = {0,};

static SvcCso_t * s_tPlayerEntry[SVC_CSO_NUMBER_OF_RESOURCE] = {NULL,};

STATIC PostMsgToMgrCb_t 	s_fnSvcCso_PostMsgToAp = NULL;

STATIC HUINT32	s_ulMaxVideoDecoder = 0;

STATIC DI_VIDEO_CAP_t	*s_pstDiVideoCapability = NULL;

//static SVC_CSO_LOCAL_Nexus_Resource_t s_ResourceMap;

/*---------------------------------------------------------------------------------
 * static functions
 *---------------------------------------------------------------------------------*/
STATIC HINT32 svc_cso_PostMsgToAp(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	if (s_fnSvcCso_PostMsgToAp)
	{
		return (* s_fnSvcCso_PostMsgToAp)(nMsg, hAction, nParam1, nParam2, nParam3);
	}

	return 1;	// error !
}

static inline void __state(SvcCso_t *player, SvcCso_State_e state)
{
	player->eState = state;
}

#if 0
static void __lock(void)
{
	VK_SEM_Get(s_media_info.lock);
}

static void __unlock(void)
{
	VK_SEM_Release(s_media_info.lock);
}
#endif

static HBOOL __prefix_match(const HINT8* url, const HINT8 *prefix)
{
	HBOOL match = VK_strncmp(url, prefix, strlen(prefix) ) == 0;
	return match;
}

static HERROR __mediaplayer_get_file_name_without_ext(HUINT8 *pucPathName, HUINT8 *pucPathFileName)
{
	HUINT32             i, ulLen = 0;

	if(pucPathName == NULL || pucPathFileName == NULL)
	{
		return ERR_FAIL;
	}

	ulLen = strlen(pucPathName);
	if(ulLen > 3)
	{
		for(i = ulLen - 1; i > 0; i--)
		{
			if(pucPathName[i] == '.')
			{
				strncpy(pucPathFileName, pucPathName, i);
				pucPathFileName[i] = '\0';
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

static HBOOL __svc_cso_is_file_content(const HINT8 *url)
{
	if ( url == NULL ) return FALSE;

	return __prefix_match(url, SUPPORTED_URI_FILE);
}


static void __svc_cso_event_handler(SvcCso_t *player, SvcCso_Event_e event)
{
	svc_cso_PostMsgToAp(event, PAL_PIPE_GetActionHandle(player->hActionId), 0, 0, 0);
}

static void __svc_cso_send_event(SvcCso_t * player, SvcCso_Local_Event_e event, HUINT32 param)
{
	SvcCso_EventInfo_t info;

	info.player = player;
	info.command = eSVC_CSO_LOCAL_COMMAND_NONE;
	info.event = event;
	info.param = param;
	info.local_event = eSVC_CSO_LOCAL_EVENT_CALLBACK;

	VK_MSG_Send(s_media_info.hmsg, (void *)&info, sizeof(info));
}

static void __svc_cso_send_command(SvcCso_t *player,
		SvcCso_Command_e command, HUINT32 p1)
{
	SvcCso_EventInfo_t info;
	HUINT32 nMsgCount = 0, nMsgMax = 0;

	VK_MSG_GetMessageCount(s_media_info.hmsg, (unsigned long *)&nMsgCount, (unsigned long *)&nMsgMax);
	if ( nMsgCount > SVC_CSO_COMMAND_LIMIT ) VK_MSG_Clear(s_media_info.hmsg);

	info.player = player;
	info.command = command;
	info.event = 0;
	info.param = p1;
	info.local_event = eSVC_CSO_LOCAL_EVENT_COMMAND;

	VK_MSG_Send(s_media_info.hmsg, (void *)&info, sizeof(info));
}

static void __svc_cso_sync_output(SvcCso_t *player)
{
	HERROR eRet = ERR_OK;
	VIDEO_FrameRate_t	eVideoFrameRate = eFrameRate_Unknown;
	HBOOL	bProgressive = FALSE;

	if ( player == NULL ) return;

#if defined(CONFIG_PROD_HMX4KBBC)
	eVideoFrameRate = eFrameRate_50;
	bProgressive	= TRUE;
#endif

	HxLOG_Debug("__svc_cso_sync_output: w(%d), h(%d), Ratio(%d)\n",
						player->ulVideoStreamWidth, player->ulVideoStreamHeight, player->eVideoAspectRatio);
	eRet = PAL_SCALER_SetVideoAttribute(0, player->hActionId, TRUE,
				player->ulVideoStreamWidth,
				player->ulVideoStreamHeight,
				(eDxASPECTRATIO_UNKNOWN == player->eVideoAspectRatio) ?\
										eDxASPECTRATIO_1X1 : player->eVideoAspectRatio,
				(eDxASPECTRATIO_UNKNOWN == player->eVideoAspectRatio) ? eAfd_None : eAfd_Full,
				bProgressive,
				eVideoFrameRate,
				eDxVIDEO_3DFORMAT_NONE
				);

	if ( eRet != ERR_OK )
	{
		HxLOG_Error("PAL_SCALER_SetVideoAttribute() returned ERR_FAIL.\n");
	}
}

static void __svc_cso_send_subtitle(SvcCso_t *player, HUINT8 *buffer, HINT32 len)
{
	SvcCsoSub_Display_Packet_t t_subpkt;
	memset(&t_subpkt, 0x00, sizeof(SvcCsoSub_Display_Packet_t));

	if(buffer != NULL)
	{
		memcpy(&t_subpkt, buffer, sizeof(SvcCsoSub_Display_Packet_t));
	}
	else
	{
		t_subpkt.ulStrNum = -1;
	}

	t_subpkt.ulFontSize = player->ulSubtitleFontSize;
	t_subpkt.ulTextPosition = player->ulSubtitleDisplayPosition;

	PAL_DSTREAM_Broadcast(DATA_STREAM_MEDIA_SUBTITLE, len, (void*)&t_subpkt);
}

static void __svc_cso_subtitle_open(SvcCso_t *player)
{
	if ( __svc_cso_is_file_content(player->pszUrl) && player->bUseSubtitle)
	{
		HINT32	nUrlLen = 0;
		HUINT32 ulSubRet = ERR_FAIL;
		HUINT8 *pSubFile = NULL;
		HUINT8 *szSubtitleFile = NULL;

		nUrlLen = strlen(player->pszUrl);
		if(nUrlLen <= 0)
		{
			HxLOG_Error("can't Access String\n");
			return;
		}
		pSubFile = (HUINT8 *)HLIB_STD_MemAlloc(nUrlLen);
		if(pSubFile == NULL)
		{
			HxLOG_Error("Out Of Memory\n");
			return;
		}
		szSubtitleFile = (HUINT8 *)HLIB_STD_MemAlloc(nUrlLen+5);
		if(szSubtitleFile == NULL)
		{
			HxLOG_Error("Out Of Memory\n");
			HLIB_STD_MemFree(pSubFile);
			pSubFile = NULL;
			return;
		}

		memset(pSubFile, 0x00, nUrlLen);
		memset(szSubtitleFile, 0x00, nUrlLen+5);

		/*TODO: why? +5? -5?*/
		/* jhchoi2 : Maybe "file:///mnt/media/a/a.ts" or "dvr:///mnt/media/a/a.ts", pointer move "///mnt/media/a/a.ts" or "//mnt/media/a/a.ts" */
		VK_strncpy(szSubtitleFile, (player->pszUrl)+5, nUrlLen-5);
		__mediaplayer_get_file_name_without_ext(szSubtitleFile, pSubFile);

		ulSubRet = SVC_CSO_SUB_Open(player, pSubFile);
		if(ulSubRet == ERR_OK)
		{
			svc_cso_PostMsgToAp (eSVC_CSO_EVENT_SBTL_START, PAL_PIPE_GetActionHandle (player->hActionId), 0, 0, 0);
			__svc_cso_send_subtitle(player, NULL,  sizeof(SvcCsoSub_Display_Packet_t));
			player->bhasSubtitle = TRUE;

			/* Default Subtitle Show ? */
			SVC_CSO_SUB_SetDisplay(player, TRUE);
		}
		else
		{
			svc_cso_PostMsgToAp (eSVC_CSO_EVENT_SBTL_UPDATE, PAL_PIPE_GetActionHandle (player->hActionId), 0, 0, 0);
			SVC_CSO_SUB_Close(player);
		}
		if(pSubFile != NULL)
		{
			HLIB_STD_MemFree(pSubFile);
			pSubFile = NULL;
		}
		if(szSubtitleFile != NULL)
		{
			HLIB_STD_MemFree(szSubtitleFile);
			szSubtitleFile = NULL;
		}
	}
}

static void __svc_cso_subtitle_close(SvcCso_t *player)
{
	if ( __svc_cso_is_file_content(player->pszUrl) && player->bUseSubtitle)
	{
		(void)SVC_CSO_SUB_Close(player);
	}
}

static HERROR __svc_cso_cmd_play(SvcCso_t *player)
{
	HERROR eRet = ERR_OK;
	DI_ERR_CODE dResult = DI_ERR_OK;

	/********************
	 *	0. State Check
	 ********************/
	if(player->eState != eSVC_CSO_STATE_STOPPED)
	{
		HxLOG_Error("SVC_CSO: Play: you should call probe function before play().\n");
		return ERR_FAIL;
	}

	/********************
	 *	1. Speed Normal (Start Init)
	 ********************/
	dResult = DI_CSO_StartSession(player->hMediaHandle, 0, 0/*UNUSED*/);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("SVC_CSO: Play: DI_CSO_StartSession(Normal) failed!\n");
		return ERR_FAIL;
	}


	/********************
	 *	2. Decoder Start
	 ********************/
	if((player->ulVideoCodecId != DI_VIDEO_FORMAT_NONE) && (player->ulVideoStreamId > 0))
	{
		eRet = DI_VIDEO_Start(eSVC_CSO_LOCAL_VIDEO_DECODER_MAIN, player->ulVideoCodecId);
		if ( eRet != DI_ERR_OK )
		{
			__svc_cso_send_event(player, eSVC_CSO_EVT_ERROR, 0);
			__svc_cso_send_command(player, eSVC_CSO_LOCAL_COMMAND_STOP, 0);
			return ERR_FAIL;
		}
	}

	/* TODO : Why Decoder Setting together Main and Sub */
	(void)DI_AUDIO_SetMediaType(eSVC_CSO_LOCAL_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
	(void)DI_AUDIO_SetMediaType(eSVC_CSO_LOCAL_AUDIO_DECODER_SUB, DI_AUDIOTYPE_MEDIA);

	if((player->ulAudioCodecId != DI_AUDFORMAT_NONE) && (player->ulAudioStreamId > 0))
	{
		PAL_AUDIO_SetHide(eAvHideClient_Decoder, FALSE);
		eRet = DI_AUDIO_Start(eSVC_CSO_LOCAL_AUDIO_DECODER_MAIN, player->ulAudioCodecId);
		if ( eRet != DI_ERR_OK )
		{
			__svc_cso_send_event(player, eSVC_CSO_EVT_ERROR, 0);
			__svc_cso_send_command(player, eSVC_CSO_LOCAL_COMMAND_STOP, 0);
			return ERR_FAIL;
		}
	}

	__svc_cso_subtitle_open(player);

	/********************
	 *	3. State Change
	 ********************/
	__state(player, eSVC_CSO_STATE_PLAYING);
	return ERR_OK;
}

static HERROR __svc_cso_cmd_stop(SvcCso_t *player)
{
	/********************
	 *	1. State Check
	 ********************/
#if 0
#if defined(CONFIG_MW_MM_DLNA_DMP_SI)
	if(player->eState == eSVC_CSO_STATE_PLAYING)
	{
		(void)SVC_CSO_SI_Release();
	}
#endif
#endif

	/********************
	 *	2. Clear the info.
	 ********************/
	player->nSpeed = 100;
	player->ullStartTime = 0;

	/********************
	 *	3. Stop Decoders
	 ********************/
	if((player->ulVideoCodecId != DI_VIDEO_FORMAT_NONE) && (player->ulVideoStreamId > 0))
	{
		(void)DI_VIDEO_Stop(eSVC_CSO_LOCAL_VIDEO_DECODER_MAIN);
		player->ulVideoCodecId = DI_VIDEO_FORMAT_NONE;
	}

	if((player->ulAudioCodecId != DI_AUDFORMAT_NONE) && (player->ulAudioStreamId > 0))
	{
		(void)DI_AUDIO_Stop(eSVC_CSO_LOCAL_AUDIO_DECODER_MAIN);
		player->ulAudioCodecId = DI_AUDFORMAT_NONE;
	}

	(void)DI_AUDIO_SetMediaType(eSVC_CSO_LOCAL_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_STREAM);
	(void)DI_AUDIO_SetMediaType(eSVC_CSO_LOCAL_AUDIO_DECODER_SUB, DI_AUDIOTYPE_STREAM);

	if(DI_CSO_StopSession(player->hMediaHandle, 0/*UNUSED*/) != DI_ERR_OK)
	{
		HxLOG_Error("DI_CSO_StopSession() returned ERR_FAIL.\n");
	}

	__state(player, eSVC_CSO_STATE_STOPPED);

	return ERR_OK;
}

static void __svc_cso_event_command_process(SvcCso_EventInfo_t *info)
{
	SvcCso_t *player = info->player;

	if ( player == NULL ) return;

	switch ( info->command )
	{
		case eSVC_CSO_LOCAL_COMMAND_PLAY:
			(void)__svc_cso_cmd_play(player);
			break;
		case eSVC_CSO_LOCAL_COMMAND_STOP:
			(void)__svc_cso_cmd_stop(player);
			break;
		case eSVC_CSO_LOCAL_COMMAND_SEEK:
			break;
		case eSVC_CSO_LOCAL_COMMAND_NONE:
		default:
			break;
	}
}

static void __svc_cso_event_callback_process(SvcCso_EventInfo_t *info)
{
	SvcCso_t *player=NULL;

	/********************
	 *	1. check parameters
	 ********************/
	if(info == NULL)
	{
		HxLOG_Error("Invalid parameter!\n");
		return;
	}

	player = info->player;
	if(player == NULL)
	{
		HxLOG_Error("Invalid parameter!\n");
		return;
	}

	if(player->eState != eSVC_CSO_STATE_PLAYING)
	{
		HxLOG_Debug("Can't Media Player: Ignore Decoder Event (%d)!\n", info->event);
		return;
	}

	/********************
	 *	2. event handling (Common)
	 ********************/
	if(info->event == eSVC_CSO_EVT_VIDEO_NEW_FRAME)
	{
		/* action */
		PAL_VIDEO_SetHide(player->nIdxVideoDecoder, eAvHideClient_Decoder, FALSE);
		__svc_cso_sync_output(player);
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_VIDEO_NEW_FRAME);
		//__svc_cso_event_handler(player, SVC_CSO_EVENT_PLAYING);
	}
	else if(info->event == eSVC_CSO_EVT_VIDEO_UNDERRUN)
	{
		/* action */
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_VIDEO_UNDERRUN);
	}
	else if(info->event == eSVC_CSO_EVT_AUDIO_NEW_FRAME)
	{
		/* Audio Mute Patch without 1FF */
		DI_MEDIA_PlaySpeed_e eSpeed;
		(void)DI_CSO_GetSpeed(player->hMediaHandle, 0, &eSpeed);
		if(eSpeed == eDI_MEDIA_Speed_Normal)
		{
			/* action : UnMute */
			PAL_AUDIO_SetHide(eAvHideClient_Decoder, FALSE);
			HxLOG_Print("SVC_CSO: eSpeed = %d, Disable Mute\n", eSpeed);
		}
		else
		{
			/* action : Mute */
			PAL_AUDIO_SetHide(eAvHideClient_Decoder, TRUE);
			HxLOG_Print("SVC_CSO: eSpeed = %d, Enable Mute\n", eSpeed);
		}
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_AUDIO_NEW_FRAME);
		//__svc_cso_event_handler(player, SVC_CSO_EVENT_PLAYING);
	}
	else if(info->event == eSVC_CSO_EVT_AUDIO_UNDERRUN)
	{
		/* action */
		PAL_AUDIO_SetHide(eAvHideClient_Decoder, TRUE);
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_AUDIO_UNDERRUN);
	}
	else if(info->event == eSVC_CSO_EVT_AUDIO_DECODER_VOLUME_CHANGED)
	{
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_AUDIO_DECODER_VOLUME_CHANGED);
	}
	else if(info->event == eSVC_CSO_EVT_MEDIA_CHANGEPLAYTIME)
	{
		/* action */
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_PLAY_POSITION_CHANGED);
		//svc_cso_PostMsgToAp(SVC_CSO_EVENT_PLAY_POSITION_CHANGED, PAL_PIPE_GetActionHandle(player->hActionId), , player->ullDuration, 0);
	}
	else if(info->event == eSVC_CSO_EVT_MEDIA_BUFFERING_UPDATED)
	{
		/* action */
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_BUFFERING_UPDATED);
	}
	else if(info->event == eSVC_CSO_EVT_BEGIN_OF_STREAM)
	{
		/* action */
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_BEGIN_OF_STREAM);
	}
	else if(info->event == eSVC_CSO_EVT_END_OF_STREAM)
	{
		/* action */
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_END_OF_STREAM);
	}
	else if(info->event == eSVC_CSO_EVT_MEDIA_PAUSE)
	{
		/* action */
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_PAUSE);
	}
	else if(info->event == eSVC_CSO_EVT_MEDIA_STOPPED)
	{
		/* action */
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_STOPPED);
	}
	else if(info->event == eSVC_CSO_EVT_MEDIA_SUB_START)
	{
		/* action */
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_SBTL_START);
	}
	else if(info->event == eSVC_CSO_EVT_MEDIA_SUB_UPDATE)
	{
		/* action */
		__svc_cso_send_subtitle(player, (HUINT8 *)info->param, sizeof(SvcCsoSub_Display_Packet_t));
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_SBTL_UPDATE);
	}
	else if(info->event == eSVC_CSO_EVT_MEDIA_SUB_STOP)
	{
		/* action */
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_SBTL_STOP);
	}
	else if(info->event == eSVC_CSO_EVT_ERROR)
	{
		/* action */
		/* event */
		__svc_cso_event_handler(player, eSVC_CSO_EVENT_ERROR);
	}
}

static void __svc_cso_state_task(void *args)
{
	HBOOL bBOF = FALSE, bEOF = FALSE;
	DI_ERR_CODE dResult = DI_ERR_OK;
	SvcCso_t* player = NULL;
	HINT32 nIdx = 0;

	while(SVC_CSO_FOREVER)
	{
		/*SVC_CSO_EVT_MEDIA_CHANGEPLAYTIME, SVC_CSO_EVT_MEDIA_BUFFERING_UPDATED, BOF, EOF*/
		for(nIdx=0; nIdx<=eActionId_VIEW_LAST; ++nIdx)
		{
			player = SVC_CSO_GetActivePlayer(nIdx);
			if (player == NULL)
			{
				//HxLOG_Error("%s: %d Player Handle.... Cannot process event... \n", __FUNCTION__, nIdx);
				continue;
			}

			if(player->eState == eSVC_CSO_STATE_PLAYING)
			{
				//HxLOG_Print("SVC_CSO: player: %d: IDLE: Buffering Info Updated...\n", nIdx);
				__svc_cso_send_event(player, eSVC_CSO_EVT_MEDIA_CHANGEPLAYTIME, 0);

				//-- Apply Calypso 0.0.14 Patch!
				// Will Delete Code
				// if(s_media_info.config.bProgressiveDownload == TRUE)
				{
					__svc_cso_send_event(player, eSVC_CSO_EVT_MEDIA_BUFFERING_UPDATED, 0);
				}

				bBOF = FALSE;
				bEOF = FALSE;

				dResult = DI_CSO_GetCurrentStatus(player->hMediaHandle, &bBOF, &bEOF);
				if(dResult != DI_ERR_OK)
				{
					HxLOG_Error("DI_CSO_GetCurrentStatus failed!\n");
				}

				if(bBOF == TRUE)
				{
					__svc_cso_send_event(player, eSVC_CSO_EVT_BEGIN_OF_STREAM, 0);
				}

				if(bEOF == TRUE)
				{
					__svc_cso_send_event(player, eSVC_CSO_EVT_END_OF_STREAM, 0);
				}

			}
		}

		VK_TASK_Sleep(500);
	}
}

static void __svc_cso_event_task(void *args)
{
	SvcCso_EventInfo_t info;

	while ( SVC_CSO_FOREVER )
	{
		HxSTD_memset(&info, 0, sizeof(info));

		/********************
		 *	1. exit check
		 ********************/
		if (s_media_info.task_flag == FALSE)
		{
			break;
		}

		/********************
		 *	2. get the message
		 ********************/
		if(VK_MSG_ReceiveTimeout(s_media_info.hmsg, &info, sizeof(info), 500) != VK_OK)
		{
			/* can't action! */
#if 0
			HBOOL bBOF = FALSE, bEOF = FALSE;
			DI_ERR_CODE dResult = DI_ERR_OK;
			SvcCso_t* player = NULL;
			HINT32 nIdx = 0;

			for(nIdx=0; nIdx<=eActionId_VIEW_LAST; ++nIdx)
			{
				player = SVC_CSO_GetActivePlayer(nIdx);
				if (player == NULL)
				{
					//HxLOG_Error("%s: %d Player Handle.... Cannot process event... \n", __FUNCTION__, nIdx);
					continue;
				}

				if(player->eState == SVC_CSO_STATE_PLAYING)
				{
					//HxLOG_Print("SVC_CSO: player: %d: IDLE: Buffering Info Updated...\n", nIdx);
					__svc_cso_send_event(player, SVC_CSO_EVT_MEDIA_CHANGEPLAYTIME, 0);
					if(s_media_info.config.bProgressiveDownload == TRUE)
					{
						__svc_cso_send_event(player, SVC_CSO_EVT_MEDIA_BUFFERING_UPDATED, 0);
					}

					bBOF = FALSE;
					bEOF = FALSE;

					dResult = DI_CSO_GetCurrentStatus(player->hMediaHandle, &bBOF, &bEOF);
					if(dResult != DI_ERR_OK)
					{
						HxLOG_Error("DI_CSO_GetCurrentStatus failed!\n");
					}

					if(bBOF == TRUE)
					{
						__svc_cso_send_event(player, SVC_CSO_EVT_BEGIN_OF_STREAM, 0);
					}

					if(bEOF == TRUE)
					{
						__svc_cso_send_event(player, SVC_CSO_EVT_END_OF_STREAM, 0);
					}
				}
			}
#endif
		}
		else
		{
			if ( info.local_event == eSVC_CSO_LOCAL_EVENT_CALLBACK )
			{
				__svc_cso_event_callback_process(&info);
			}
			else
			{
				__svc_cso_event_command_process(&info);
			}
		}
	}
}

static DI_VIDEO_FORMAT __svc_cso_video_codec_to_di(DxVideoCodec_e codec)
{
	DI_VIDEO_FORMAT eDiVideoCodec;

	/* Codec Conversion */
	switch(codec)
	{
		case eDxVIDEO_CODEC_MPEG1:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_MPEG1;
			break;
		case eDxVIDEO_CODEC_MPEG2:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_MPEG2;
			break;
		case eDxVIDEO_CODEC_MPEG4_PART2:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_MPEG4Part2;
			break;
		case eDxVIDEO_CODEC_H261:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H261;
			break;
		case eDxVIDEO_CODEC_H263:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H263;
			break;
		case eDxVIDEO_CODEC_H264:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H264;
			break;
		case eDxVIDEO_CODEC_H265:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H265;
			break;
		case eDxVIDEO_CODEC_H264_SVC:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H264;
			break;
		case eDxVIDEO_CODEC_H264_MVC:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_H264;
			break;
		case eDxVIDEO_CODEC_VC1:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_VC1;
			break;
		case eDxVIDEO_CODEC_VC1_SIMPLEMAIN:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_VC1SimpleMain;
			break;
		case eDxVIDEO_CODEC_AVS:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_AVS;
			break;
		case eDxVIDEO_CODEC_RV40:
			eDiVideoCodec = DI_VIDEO_FORMAT_NONE;
			break;
		case eDxVIDEO_CODEC_VP6:
			eDiVideoCodec = DI_VIDEO_FORMAT_NONE;
			break;
		case eDxVIDEO_CODEC_DIVX311:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_DIVX_311;
			break;
		case eDxVIDEO_CODEC_XVID:
			eDiVideoCodec = DI_VIDEO_FORMAT_NONE;
			break;
		case eDxVIDEO_CODEC_VP8:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_VP8;
			break;
		case eDxVIDEO_CODEC_VP9:
			eDiVideoCodec = DI_VIDEO_FORMAT_MEDIA_VP9;
			break;
		default:
			eDiVideoCodec = DI_VIDEO_FORMAT_NONE;
			HxLOG_Print("not defined or supported pal codec (%s) \n", OTL_ENUM2STR_VideoCodec(codec));
			break;
	}

	return eDiVideoCodec;
}

static DI_AUDIO_FORMAT __svc_cso_audio_codec_to_di(DxAudioCodec_e codec)
{
	DI_AUDIO_FORMAT eDiCodec;

	switch(codec)
	{
		case eDxAUDIO_CODEC_MPEG:
			eDiCodec = DI_AUDFORMAT_MPEG;
			break;
		case eDxAUDIO_CODEC_MP3:
			eDiCodec = DI_AUDFORMAT_MP3;
			break;
		case eDxAUDIO_CODEC_DOLBY_AC3:
			eDiCodec = DI_AUDFORMAT_AC3;
			break;
		case eDxAUDIO_CODEC_DOLBY_AC3P:
			eDiCodec = DI_AUDFORMAT_AC3_PLUS;
			break;
		case eDxAUDIO_CODEC_PCM:
			eDiCodec = DI_AUDFORMAT_PCM;
			break;
		case eDxAUDIO_CODEC_AAC:
			eDiCodec = DI_AUDFORMAT_AAC;
			break;
		case eDxAUDIO_CODEC_AAC_LOAS:
			eDiCodec = DI_AUDFORMAT_AAC_LOAS;
			break;
		case eDxAUDIO_CODEC_AAC_PLUS:
			eDiCodec = DI_AUDFORMAT_AAC_PLUS;
			break;
		case eDxAUDIO_CODEC_AAC_PLUS_ADTS:
			eDiCodec = DI_AUDFORMAT_AAC_PLUS_ADTS;
			break;
		case eDxAUDIO_CODEC_DTS:
			eDiCodec = DI_AUDFORMAT_DTS;
			break;
		case eDxAUDIO_CODEC_DTS_HD:
			eDiCodec = DI_AUDFORMAT_DTS_HD;
			break;
		case eDxAUDIO_CODEC_LPCM_DVD:
			eDiCodec = DI_AUDFORMAT_LPCM_DVD;
			break;
		case eDxAUDIO_CODEC_LPCM_HD_DVD:
			eDiCodec = DI_AUDFORMAT_LPCM_HD_DVD;
			break;
		case eDxAUDIO_CODEC_LPCM_BLUERAY:
			eDiCodec = DI_AUDFORMAT_LPCM_BLUERAY;
			break;
		case eDxAUDIO_CODEC_WMA_STD:
			eDiCodec = DI_AUDFORMAT_WMA_STD;
			break;
		case eDxAUDIO_CODEC_WMA_PRO:
			eDiCodec = DI_AUDFORMAT_WMA_PRO;
			break;
		case eDxAUDIO_CODEC_FLAC:
			eDiCodec = DI_AUDFORMAT_FLAC;
			break;
		case eDxAUDIO_CODEC_ADPCM:
			eDiCodec = DI_AUDFORMAT_ADPCM;
			break;
		case eDxAUDIO_CODEC_PCMWAVE:
			eDiCodec = DI_AUDFORMAT_PCMWAVE;
			break;
		case eDxAUDIO_CODEC_DRA:
			eDiCodec = DI_AUDFORMAT_DRA;
			break;
		case eDxAUDIO_CODEC_AIFF:
			eDiCodec = DI_AUDFORMAT_PCM;
			break;
		case eDxAUDIO_CODEC_VORBIS:
			eDiCodec = DI_AUDFORMAT_VORBIS;
			break;
		case eDxAUDIO_CODEC_OPUS:
			eDiCodec = DI_AUDFORMAT_OPUS;
			break;
		case eDxAUDIO_CODEC_RIFF:
		default:
			HxLOG_Print("not supported codec (%s) \n",  OTL_ENUM2STR_AudioCodec(codec));
			eDiCodec = DI_AUDFORMAT_MPEG;
			break;
	}

	return eDiCodec;
}

static HBOOL __svc_cso_audiofade_get_running(void)
{
	HBOOL bRunning = FALSE;

	AUDIOFADE_SEMA_LOCK;

	bRunning = stAudioFade.bRunning;
	
	AUDIOFADE_SEMA_UNLOCK;
	
	return bRunning;
}

static void __svc_cso_audiofade_set_running(HBOOL bRunning)
{
	AUDIOFADE_SEMA_LOCK;

	stAudioFade.bRunning = bRunning;

	AUDIOFADE_SEMA_UNLOCK;

	return;
}

static SvcCso_Status_AudioFade_e __svc_cso_audiofade_get_status(void)
{
	SvcCso_Status_AudioFade_e eStatus = eSVC_CSO_STATUS_AUDIOFADE_STOPPED;
	
	AUDIOFADE_SEMA_LOCK;

	eStatus = stAudioFade.eStatus;

	AUDIOFADE_SEMA_UNLOCK;

	return eStatus;
}

static void __svc_cso_audiofade_set_status(SvcCso_Status_AudioFade_e eStatus)
{
	AUDIOFADE_SEMA_LOCK;

	stAudioFade.eStatus = eStatus;

	AUDIOFADE_SEMA_UNLOCK;

	return;
}

static HUINT32 __svc_cso_audiofade_get_audio_decoder_volume(void)
{
	HUINT32 unVolume = 0;

	AUDIOFADE_SEMA_LOCK;

	unVolume = stAudioFade.unVolume;

	AUDIOFADE_SEMA_UNLOCK;

	return unVolume;
}

static void __svc_cso_audiofade_set_audio_decoder_volume(HUINT32 unVolume)
{
	AUDIOFADE_SEMA_LOCK;

	stAudioFade.unVolume = unVolume;

	AUDIOFADE_SEMA_UNLOCK;
}

static void __svc_cso_audiofade_send_event(SvcCso_t *player, SvcCso_Event_AudioFade_e eEvent, HUINT32 unParam)
{
	SvcCso_Msg_AudioFade_t stMsg;
	SvcCso_Info_AudioFade_t *pstInfo = NULL;
	
	if(player == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] player is NULL\033[0m\n", __func__, __LINE__);
		goto func_done;
	}

	if(stAudioFade.ulMsgId == 0)
	{
		HxLOG_Debug("\033[1;31m @@@@@ [%s:%d] Cannot send message to AudioFade Task.\033[0m\n", __func__, __LINE__);
		goto func_done;
	}
	
	VK_MEM_Memset(&stMsg, 0x00, sizeof(SvcCso_Msg_AudioFade_t));

	stMsg.player = player;
	stMsg.eEvent = eEvent;
	
	pstInfo = (SvcCso_Info_AudioFade_t *)unParam;
	if(pstInfo != NULL)
	{
		stMsg.stInfo.unStartVolume = pstInfo->unStartVolume;
		stMsg.stInfo.unTargetVolume = pstInfo->unTargetVolume;
		stMsg.stInfo.nEase = pstInfo->nEase;
		stMsg.stInfo.nEaseTimeElapsed = pstInfo->nEaseTimeElapsed;
		stMsg.stInfo.nEaseTransitionFrames = pstInfo->nEaseTransitionFrames;
	}

	AUDIOFADE_SEMA_LOCK;
	if(stAudioFade.ulMsgId != 0)
	{
		VK_MSG_Send(stAudioFade.ulMsgId, (void *)&stMsg, sizeof(SvcCso_Msg_AudioFade_t));
	}
	AUDIOFADE_SEMA_UNLOCK;
	
func_done:
	return;
}

static HINT32 __svc_cso_audiofade_ease_next(SvcCso_Ease_AudioFade_e eEase, HINT32 t, HUINT32 b, HINT32 c, HINT32 d)
{
	double dT = 0.0, dB = 0.0, dC = 0.0, dD = 0.0;
	
	if(d == 0)
	{
		/* Prevent divide by zero */
		d = 100;
	}

	dT = (double)t;
	dB = ((double)b) / 100.0;
	dC = ((double)c) / 100.0;
	dD = (double)d;
	
	switch(eEase)
	{
		default:
		case eSVC_CSO_EASE_AUDIOFADE_EASELINEAR:
			return (HINT32)((dC*dT/dD + dB) * 100.0);
		case eSVC_CSO_EASE_AUDIOFADE_EASEINCUBIC:
			dT/=dD;
			return (HINT32)((dC*dT*dT*dT + dB) * 100.0);
		case eSVC_CSO_EASE_AUDIOFADE_EASEOUTCUBIC:
			dT=dT/dD-1;
			return (HINT32)((dC*(dT*dT*dT + 1) + dB) * 100.0);
	}
}

static HERROR __svc_cso_audiofade_ease(SvcCso_Info_AudioFade_t *pstInfo, HUINT32 *punSetVolume)
{
	HERROR hErr = ERR_OK;
	HINT32 nDelta = 0;
	
	if(pstInfo == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] pstInfo is NULL\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_done;
	}

	if(punSetVolume == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] punSetVolume is NULL\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_done;
	}

	if(pstInfo->nEaseTransitionFrames == 0)
	{
		*punSetVolume = pstInfo->unTargetVolume;
		goto func_done;
	}

	nDelta = pstInfo->unTargetVolume - pstInfo->unStartVolume;
	if(pstInfo->nEaseTransitionFrames && pstInfo->nEaseTimeElapsed < pstInfo->nEaseTransitionFrames)
	{
		*punSetVolume = __svc_cso_audiofade_ease_next(	(SvcCso_Ease_AudioFade_e)pstInfo->nEase, 
														pstInfo->nEaseTimeElapsed, 
														pstInfo->unStartVolume, 
														nDelta, 
														pstInfo->nEaseTransitionFrames - SVC_CSO_AUDIOFADE_TASK_SLEEP);
	}

	if(pstInfo->nEaseTimeElapsed < pstInfo->nEaseTransitionFrames)
	{
		pstInfo->nEaseTimeElapsed += SVC_CSO_AUDIOFADE_TASK_SLEEP;
	}

func_done:
	return hErr;
}

static HERROR __svc_cso_audiofade_check_mute(HUINT32 unSetVolume)
{
	HERROR hErr = ERR_OK;

	if(unSetVolume == 0)
	{
		hErr = PAL_OUT_SetDecoderAudioMute(TRUE);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to set decoder audio mute, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
			goto func_done;
		}
	}
	else
	{
		hErr = PAL_OUT_SetDecoderAudioMute(FALSE);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to set decoder audio mute, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
			goto func_done;
		}
	}

func_done:
	return hErr;
}

static void __svc_cso_audiofade_task(void *args)
{
	SvcCso_Msg_AudioFade_t stMsg;
	HINT32 nVkErr = VK_OK;
	HBOOL bRunning = FALSE;
	SvcCso_Status_AudioFade_e eStatus = eSVC_CSO_STATUS_AUDIOFADE_STOPPED;
	SvcCso_t *player = NULL;
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	HUINT32 unSetVolume = 0;
	SvcCso_Info_AudioFade_t stInfo;
	HERROR hErr = ERR_OK;
	HBOOL bUnderrun = FALSE;

	if(args == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] args is NULL\033[0m\n", __func__, __LINE__);
		goto func_done;
	}

	player = args;

	VK_MEM_Memset(&stMsg, 0x00, sizeof(SvcCso_Msg_AudioFade_t));
	VK_MEM_Memset(&stInfo, 0x00, sizeof(SvcCso_Info_AudioFade_t));
	
	bRunning = TRUE;
	__svc_cso_audiofade_set_running(bRunning);

	eStatus = eSVC_CSO_STATUS_AUDIOFADE_RUNNING;
	__svc_cso_audiofade_set_status(eStatus);
	
	while(1)
	{
		bRunning = __svc_cso_audiofade_get_running();
		if(!bRunning)
		{
			HxLOG_Debug("\033[1;31m @@@@@ [%s:%d] AudioFade Task is destroying..\033[0m\n", __func__, __LINE__);
			break;
		}
		
		nVkErr = VK_MSG_ReceiveTimeout(stAudioFade.ulMsgId, &stMsg, sizeof(SvcCso_Msg_AudioFade_t), SVC_CSO_AUDIOFADE_TASK_SLEEP);
		if(nVkErr == VK_OK)
		{
			switch(stMsg.eEvent)
			{
				case eSVC_CSO_EVT_AUDIOFADE_SET_FADE:
					HxLOG_Debug("\033[1;31m @@@@@ [%s:%d] eSVC_CSO_EVT_AUDIOFADE_SET_FADE\033[0m\n", __func__, __LINE__);
					if(player != stMsg.player)
					{
						HxLOG_Error("\033[1;31m @@@@@ [%s:%d] player(%p) != stMsg.player(%p)\033[0m\n", __func__, __LINE__, player, stMsg.player);
					}
					else
					{
						stInfo.unStartVolume = stMsg.stInfo.unStartVolume;
						stInfo.unTargetVolume = stMsg.stInfo.unTargetVolume;
						stInfo.nEase = stMsg.stInfo.nEase;
						stInfo.nEaseTimeElapsed = stMsg.stInfo.nEaseTimeElapsed;
						stInfo.nEaseTransitionFrames = stMsg.stInfo.nEaseTransitionFrames;
						if(stInfo.nEaseTransitionFrames == SVC_CSO_AUDIOFADE_TASK_SLEEP)
						{
							/* Prevent calculation error */
							stInfo.nEaseTransitionFrames <<= 1;
						}

						/* If we receive underrun event, then skip to set decoder volume */
						if(bUnderrun)
						{
							HxLOG_Debug("\033[1;31m @@@@@ [%s:%d] UNDERRUN! skip to set decoder volume..\033[0m\n", __func__, __LINE__);
							continue;
						}
						
						unSetVolume = 0;

						hErr = __svc_cso_audiofade_ease(&stInfo, &unSetVolume);
						if(hErr != ERR_OK)
						{
							HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to ease volume, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
							continue;
						}

						eDiErr = DI_AUDIO_SetDecoderVolume(player->nIdxAudioDecoder, unSetVolume);
						if(eDiErr != DI_ERR_OK)
						{
							HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to set decoder volume, eDiErr = %d, unSetVolume = %u\033[0m\n", __func__, __LINE__, eDiErr, unSetVolume);
						}
						else
						{
							/* 	Fix for Netflix NTS AUDIO-007-TC3(Audio Mixer - Playing sounds during video playback with 5.1 passthrough audio.)
								When target decoder volume is set to 0, mute should be set to TRUE. */
							hErr = __svc_cso_audiofade_check_mute(unSetVolume);
							if(hErr != ERR_OK)
							{
								HxLOG_Debug("\033[1;31m @@@@@ [%s:%d] Failed to check mute, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
							}
							
							__svc_cso_audiofade_set_audio_decoder_volume(unSetVolume);
							__svc_cso_send_event(player, eSVC_CSO_EVT_AUDIO_DECODER_VOLUME_CHANGED, unSetVolume);
						}

						if(stInfo.nEaseTimeElapsed >= stInfo.nEaseTransitionFrames)
						{
							/* Volume settings have been completed. */
							VK_MEM_Memset(&stInfo, 0x00, sizeof(SvcCso_Info_AudioFade_t));
						}
					}
					break;
				case eSVC_CSO_EVT_AUDIOFADE_VIDEO_NEW_FRAME:
				case eSVC_CSO_EVT_AUDIOFADE_AUDIO_NEW_FRAME:
					bUnderrun = FALSE;
					break;
				case eSVC_CSO_EVT_AUDIOFADE_VIDEO_UNDERRUN:
				case eSVC_CSO_EVT_AUDIOFADE_AUDIO_UNDERRUN:
					bUnderrun = TRUE;
					break;
				default:
					break;
			}
			
			VK_MEM_Memset(&stMsg, 0x00, sizeof(SvcCso_Msg_AudioFade_t));
		}
		else if(nVkErr == VK_TIMEOUT)
		{
			if(stInfo.nEaseTimeElapsed < stInfo.nEaseTransitionFrames)
			{
				/* If we receive underrun event, then skip to set decoder volume */
				if(bUnderrun)
				{
					HxLOG_Debug("\033[1;31m @@@@@ [%s:%d] UNDERRUN! skip to set decoder volume..\033[0m\n", __func__, __LINE__);
					continue;
				}
				
				unSetVolume = 0;
				hErr = __svc_cso_audiofade_ease(&stInfo, &unSetVolume);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to ease volume, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
					continue;
				}

				eDiErr = DI_AUDIO_SetDecoderVolume(player->nIdxAudioDecoder, unSetVolume);
				if(eDiErr != DI_ERR_OK)
				{
					HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to set decoder volume, eDiErr = %d, unSetVolume = %u\033[0m\n", __func__, __LINE__, eDiErr, unSetVolume);
				}
				else
				{
					/* 	Fix for Netflix NTS AUDIO-007-TC3(Audio Mixer - Playing sounds during video playback with 5.1 passthrough audio.)
						When target decoder volume is set to 0, mute should be set to TRUE. */
					hErr = __svc_cso_audiofade_check_mute(unSetVolume);
					if(hErr != ERR_OK)
					{
						HxLOG_Debug("\033[1;31m @@@@@ [%s:%d] Failed to check mute, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
					}
					
					__svc_cso_audiofade_set_audio_decoder_volume(unSetVolume);
					__svc_cso_send_event(player, eSVC_CSO_EVT_AUDIO_DECODER_VOLUME_CHANGED, unSetVolume);
				}

				if(stInfo.nEaseTimeElapsed >= stInfo.nEaseTransitionFrames)
				{
					/* Volume settings have been completed. */
					VK_MEM_Memset(&stInfo, 0x00, sizeof(SvcCso_Info_AudioFade_t));
				}
			}
		}
		else
		{
			HxLOG_Debug("\033[1;31m @@@@@ [%s:%d] Unexpected error, nVkErr = %d\033[0m\n", __func__, __LINE__, nVkErr);
		}
	}

	/* Restore Audio Decoder Volume */
	unSetVolume = 100;
	DI_AUDIO_SetDecoderVolume(player->nIdxAudioDecoder, unSetVolume);
	hErr = __svc_cso_audiofade_check_mute(unSetVolume);
	if(hErr != ERR_OK)
	{
		HxLOG_Debug("\033[1;31m @@@@@ [%s:%d] Failed to check mute, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
	}

	eStatus = eSVC_CSO_STATUS_AUDIOFADE_TERMINATE;
	__svc_cso_audiofade_set_status(eStatus);

func_done:
	return;
}

static HERROR svc_cso_ConvertDi2Pal_AspectRatio(DI_VIDEO_AspectRatio_t eDiAspectRatio,
		DxAspectRatio_e *pePalAspectRatio)
{
	if ( pePalAspectRatio == NULL )
	{
		return ERR_FAIL;
	}

	switch ( eDiAspectRatio )
	{
		case DI_VIDEO_ASPECTRATIO_16_9:
			*pePalAspectRatio = eDxASPECTRATIO_16X9;
			break;
		case DI_VIDEO_ASPECTRATIO_4_3:
			*pePalAspectRatio = eDxASPECTRATIO_4X3;
			break;
		case DI_VIDEO_ASPECTRATIO_14_9:
			*pePalAspectRatio = eDxASPECTRATIO_14X9;
			break;
		case DI_VIDEO_ASPECTRATIO_1_1:
			*pePalAspectRatio = eDxASPECTRATIO_1X1;
			break;
		default:
			*pePalAspectRatio = eDxASPECTRATIO_UNKNOWN;
			break;
	}

	return ERR_OK;
}


static void svc_cso_video_callback(void *pvParam)
{
	DI_ERR_CODE dResult;
	SvcCso_t* player=NULL;
	DI_VIDEO_CALLBACK_DATA_t* pstCallback=NULL;
	DI_VIDEO_RUNNING_STATE_EVT_t* pstEvent=NULL;
	DI_VIDEO_StreamInfo_t stVideoStreamInfo;
	HUINT32 unTempActionId = 0;
	HINT32 nidx = 0;


	/*************************
	 *	1. check parameters
	 *************************/
	pstCallback = (DI_VIDEO_CALLBACK_DATA_t *)pvParam;
	if(pstCallback == NULL)
	{
		return;
	}

	/* find player and view */
	for(nidx=eActionId_VIEW_FIRST; nidx <= eActionId_VIEW_LAST; ++nidx)
	{
		if(s_tPlayerEntry[nidx] == NULL)
		{
			continue;
		}
		if(s_tPlayerEntry[nidx]->nIdxVideoDecoder == pstCallback->ulDecId)
		{
			unTempActionId = nidx;
			break;
		}
	}
	if(nidx > eActionId_VIEW_LAST)
	{
		HxLOG_Error("Can't found player!\n");
		return;
	}

	player = s_tPlayerEntry[unTempActionId];

#if 0 /*single mode*/
	player = SVC_CSO_GetActivePlayer(0);
	if(player == NULL)
	{
		HxLOG_Error("svc_cso_video_callback: NO Player Handle.... \n");
		return;
	}
#endif
	pstEvent = (DI_VIDEO_RUNNING_STATE_EVT_t *)pstCallback->pvEventData;

	/*************************
	 *	2. getting events
	 *************************/
	if(pstCallback->eEvent == DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED)
	{
		if(pstEvent->state == DI_VIDEO_RUNNING_STATE_DECODING)
		{
			dResult = DI_VIDEO_GetSequenceHeaderInfo(eSVC_CSO_LOCAL_VIDEO_DECODER_MAIN, &stVideoStreamInfo);
			if(dResult != DI_ERR_OK)
			{
				HxLOG_Error("DI_VIDEO_GetSequenceHeaderInfo failed!\n");
				return;
			}
			player->ulVideoStreamWidth = stVideoStreamInfo.ulWidth;
			player->ulVideoStreamHeight = stVideoStreamInfo.ulHeight;
			svc_cso_ConvertDi2Pal_AspectRatio(stVideoStreamInfo.eAspectRatio, &(player->eVideoAspectRatio));

			__svc_cso_send_event(player, eSVC_CSO_EVT_VIDEO_NEW_FRAME, 0);
			__svc_cso_audiofade_send_event(player, eSVC_CSO_EVT_AUDIOFADE_VIDEO_NEW_FRAME, 0);
		}
		else if(pstEvent->state == DI_VIDEO_RUNNING_STATE_NODATA)
		{
			__svc_cso_send_event(player, eSVC_CSO_EVT_VIDEO_UNDERRUN, 0);
			__svc_cso_audiofade_send_event(player, eSVC_CSO_EVT_AUDIOFADE_VIDEO_UNDERRUN, 0);
		}
	}
#if !defined(CONFIG_PROD_YSR2000) && !defined(CONFIG_PROD_YS1000)
	else if(pstCallback->eEvent == DI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED)
	{

		DxAspectRatio_e			eVideoAspectRatio = eDxASPECTRATIO_UNKNOWN;
		
		if( DI_VIDEO_GetSequenceHeaderInfo(pstCallback->ulDecId, &stVideoStreamInfo) != DI_ERR_OK )
		{
			HxLOG_Error("Error from DI_VIDEO_GetSequenceHeaderInfo()\n");
			return;
		}
		svc_cso_ConvertDi2Pal_AspectRatio(stVideoStreamInfo.eAspectRatio, &eVideoAspectRatio);
		if(player->eVideoAspectRatio != eVideoAspectRatio)
		{
			HxLOG_Debug("Aspect Ratio is changed from DI VIDEO : %d  ---> %d\n", stVideoStreamInfo.eAspectRatio, eVideoAspectRatio);
			player->eVideoAspectRatio = eVideoAspectRatio;
			/* Process here directly */
			__svc_cso_sync_output(player);				
		}
	}
#endif
	return;
}

static void svc_cso_audio_callback(void *pvParam)
{
	SvcCso_t* player=NULL;
	DI_AUDIO_CALLBACK_DATA_t* pstCallback=NULL;
	DI_AUDIO_RUNNING_STATE_EVT_t* pstEvent=NULL;
	HUINT32 unTempActionId = 0;
	HINT32 nidx = 0;

	/*************************
	 *	1. check parameters
	 *************************/
	pstCallback = (DI_AUDIO_CALLBACK_DATA_t *)pvParam;
	if(pstCallback == NULL)
	{
		return;
	}

	/* find player and view */
	for(nidx=eActionId_VIEW_FIRST; nidx <= eActionId_VIEW_LAST; ++nidx)
	{
		if(s_tPlayerEntry[nidx] == NULL)
		{
			continue;
		}
		if(s_tPlayerEntry[nidx]->nIdxAudioDecoder == pstCallback->ulDecId)
		{
			unTempActionId = nidx;
			break;
		}
	}
	if(nidx > eActionId_VIEW_LAST)
	{
		HxLOG_Error("Can't found player!\n");
		return;
	}

	player = s_tPlayerEntry[unTempActionId];

#if 0 /*single mode*/
	player = SVC_CSO_GetActivePlayer(0);
	if(player == NULL)
	{
		HxLOG_Error("svc_cso_video_callback: NO Player Handle.... \n");
		return;
	}
#endif
	pstEvent = (DI_AUDIO_RUNNING_STATE_EVT_t*)pstCallback->pvEventData;

	/*************************
	 *	2. getting events
	 *************************/
	if(pstCallback->nEvent == DI_AUDIO_RUNNING_STATE_CHANGED)
	{
		if(pstEvent->state == DI_AUDIO_STATE_DECODING)
		{
			__svc_cso_send_event(player, eSVC_CSO_EVT_AUDIO_NEW_FRAME, 0);
			__svc_cso_audiofade_send_event(player, eSVC_CSO_EVT_AUDIOFADE_AUDIO_NEW_FRAME, 0);
		}
		else if(pstEvent->state == DI_AUDIO_STATE_NODATA)
		{
			__svc_cso_send_event(player, eSVC_CSO_EVT_AUDIO_UNDERRUN, 0);
			__svc_cso_audiofade_send_event(player, eSVC_CSO_EVT_AUDIOFADE_AUDIO_UNDERRUN, 0);
		}
	}

	return;
}


#if __________media__________
#endif

HERROR SVC_CSO_Init(PostMsgToMgrCb_t pfncPostMsgCallback)
{
	DI_ERR_CODE diErr = DI_ERR_OK;

	if ( b_cso_mdeia_initialized == TRUE )
	{
		return ERR_OK;
	}

	if (pfncPostMsgCallback == NULL)
	{
		HxLOG_Error("[%s:%s] invalid msg callback function !! \n", __FUNCTION__, __LINE__);
	}
	s_fnSvcCso_PostMsgToAp = pfncPostMsgCallback;

	memset(&s_media_info, 0x00, sizeof(SvcCso_Info_t));

	/* 0. Get Video Capabillity */
        diErr = DI_VIDEO_GetCapability(&s_ulMaxVideoDecoder, &s_pstDiVideoCapability);
        if(diErr != DI_ERR_OK)
        {
                HxLOG_Error("\tCritical Eror!!!\n");
        }


	/********************
	 *	1. Create SEM, MSG, TASK
	 ********************/
	if ( VK_SEM_Create(&s_media_info.lock, SVC_CSO_SEMAPHORE_NAME,
				VK_SUSPENDTYPE_PRIORITY ) != VK_OK )
	{
		return ERR_FAIL;
	}

	if ( VK_MSG_Create(SVC_CSO_QUEUE_SIZE, sizeof(SvcCso_EventInfo_t), SVC_CSO_QUEUE_NAME,
				&s_media_info.hmsg, VK_SUSPENDTYPE_PRIORITY) != VK_OK )
	{
		HxLOG_Error("SVC_CSO: Init: Cannot create the event for media player.\n");
		return ERR_FAIL;
	}

	s_media_info.task_flag = TRUE;

	if ( VK_TASK_Create(__svc_cso_event_task, SVC_CSO_EVENT_TASK_PRIORITY,
				SVC_CSO_EVENT_TASK_STACK_SIZE, SVC_CSO_EVENT_TASK_NAME,
				NULL, &s_media_info.event_task, 0) != VK_OK )
	{
		HxLOG_Error("SVC_CSO: Init: Cannot create the event task for media player.\n");
		return ERR_FAIL;
	}

	if ( VK_TASK_Start(s_media_info.event_task) != VK_OK )
	{
		HxLOG_Error("SVC_CSO: Init: Cannot start the event task for media player.\n");
		return ERR_FAIL;
	}

	if ( VK_TASK_Create(__svc_cso_state_task, SVC_CSO_EVENT_TASK_PRIORITY,
				SVC_CSO_EVENT_TASK_STACK_SIZE, SVC_CSO_EVENT_TASK_NAME,
				NULL, &s_media_info.state_task, 0) != VK_OK )
	{
		HxLOG_Error("SVC_CSO: Init: Cannot create the state task for media player.\n");
		return ERR_FAIL;
	}


	//(void)DI_CSO_SetCalypsoMode(NULL);

	/* Audio Ease function for Netflix */
	VK_MEM_Memset(&stAudioFade, 0x00, sizeof(SvcCso_AudioFade_t));

	/********************
	 *	2. Initialize DnP Config
	 ********************/
#ifdef	CONFIG_MW_MM_PVR_INTERNAL
	// if there is internal harddisk in settop box.
	s_media_info.config.bProgressiveDownload = TRUE;
	HxSTD_memset(s_media_info.config.szPDLPath, 0, SVC_CSO_MAX_URL_LEN);
	VK_strncpy(s_media_info.config.szPDLPath, SVC_CSO_DEFAULT_HDD_BUFFER_PATH,
			VK_strlen(SVC_CSO_DEFAULT_HDD_BUFFER_PATH));
#else
	s_media_info.config.bProgressiveDownload = FALSE;
	s_media_info.config.szPDLPath[0] = '\0';
#endif

	b_cso_mdeia_initialized = TRUE;

	SVC_CSO_SUB_Init((SvcCsoSub_EventCallback_t)__svc_cso_send_event);

	HxLOG_Print("SVC_CSO: Init: Media player is initialized succefully.\n");

	return ERR_OK;
}

HERROR SVC_CSO_UnInit(void)
{
	VK_MSG_Destroy(s_media_info.hmsg);

	s_media_info.task_flag = FALSE;

	VK_SEM_Destroy(s_media_info.lock);

	HxSTD_memset(&s_media_info.config, 0, sizeof(s_media_info.config));

	b_cso_mdeia_initialized = FALSE;

	HxLOG_Print("SVC_CSO: Init: Media player is uninitialized succefully.\n");

	return ERR_OK;
}

SvcCso_t *SVC_CSO_Create(HUINT32 hActionId, PostMsgToMgrCb_t pfncPostMsgCallback)
{
	HUINT32 i = 0;
	HERROR	nResult = ERR_OK;
	HUINT32 ulVideoCaller = 0;
	HUINT32 ulAudioCaller = 0;
	HUINT32 nTempVideoIndex = 0;
	HUINT32 nTempAudioIndex = 0;
	DI_ERR_CODE dResult;
	SvcCso_t *player = NULL;

	const HUINT8 videoEvents[4] = {
		DI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED,
		DI_VIDEO_NOTIFY_EVT_FORMAT_CHANGED,
		DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED,
		DI_VIDEO_NOTIFY_EVT_AFD_CHANGED
	};

	const HUINT8 audioEvents[3] = {
		DI_AUDIO_RUNNING_STATE_CHANGED,
		DI_AUDIO_FORMAT_CHANGED,
		DI_AUDIO_MEMORY_STREAM_COMPLETED
	};

	/********************
	 *	1. State Task Start
	 ********************/
	if ( VK_TASK_Start(s_media_info.state_task) != VK_OK )
	{
		HxLOG_Error("SVC_CSO: CreatePlayer: Cannot start the state task for media player.\n");
		return NULL;
	}

	/********************
	 *	2. Create Player
	 ********************/
	player = (SvcCso_t *)HLIB_STD_MemAlloc(sizeof(SvcCso_t));
	if ( player == NULL )
	{
		HxLOG_Error("SVC_CSO: CreatePlayer: Insufficient memory.\n");
		return NULL;
	}

	VK_MEM_Memset(player, 0, sizeof(SvcCso_t));

	player->hActionId = hActionId;
	player->nSpeed = 100;
	player->ullStartTime = 0;
	player->eState = eSVC_CSO_STATE_STOPPED;
	player->next = NULL;
	player->bCalypsoPlayerMode = TRUE;
	player->bEnableDemuxPath = FALSE;
	dResult = DI_CSO_CreateSession(NULL, &player->hMediaHandle);
	if(dResult != DI_ERR_OK)
	{
		HLIB_STD_MemFree(player);
		HxLOG_Error("SVC_CSO: CreatePlayer: DI_CSO_CreateSession failed!\n");
		return NULL;
	}
#if 0
	(void)DI_CSO_SetCurrentMediaInstance(player->hMediaHandle);
#endif

	/********************
	 *	3. Register Callbacks
	 ********************/
	/* TODO : If view count over '2' then add implementation below */
#if 0
	if(hActionId == eActionId_VIEW_0)
	{
		player->nIdxVideoDecoder = eSVC_CSO_LOCAL_VIDEO_DECODER_MAIN;
		player->nIdxAudioDecoder = eSVC_CSO_LOCAL_AUDIO_DECODER_MAIN;
		nTempVideoIndex = DI_CSO_VIDEO_DECODER_MAIN;
		nTempAudioIndex = DI_CSO_AUDIO_DECODER_MAIN;
	}
	else if(hActionId == eActionId_VIEW_1)
	{
		player->nIdxVideoDecoder = eSVC_CSO_LOCAL_VIDEO_DECODER_SUB;
		player->nIdxAudioDecoder = eSVC_CSO_LOCAL_AUDIO_DECODER_SUB;
		nTempVideoIndex = DI_CSO_VIDEO_DECODER_SUB;
		nTempAudioIndex = DI_CSO_AUDIO_DECODER_SUB;
	}
	else
	{
		HxLOG_Error("SVC_CSO: %s Failed, Not Support Add View. Support View Number : %d\n", __FUNCTION__, (HINT32)eActionId_VIEW_1);
	}
#else
	/* TODO : Temporary Patch!! Will Chacnge ActionID and DecoderID */
	/* DEFAULT */
	player->nIdxVideoDecoder = eSVC_CSO_LOCAL_VIDEO_DECODER_MAIN;
	player->nIdxAudioDecoder = eSVC_CSO_LOCAL_AUDIO_DECODER_MAIN;
	nTempVideoIndex = DI_MEDIA_VIDEO_DECODER_MAIN;
	nTempAudioIndex = DI_MEDIA_AUDIO_DECODER_MAIN;
#endif
	if(!s_tPlayerEventCaller[hActionId].bHasCaller)
	{
		for ( i=0; i<sizeof(videoEvents); ++i )
		{
			nResult = DI_VIDEO_RegisterEventCallback(player->nIdxVideoDecoder, videoEvents[i], svc_cso_video_callback, &ulVideoCaller);
			if ( nResult != ERR_OK )
			{
				HxLOG_Error("SVC_CSO: %s : DI_VIDEO_RegisterEventCallback() failed.\n", __FUNCTION__);
				dResult = DI_CSO_DestroySession(player->hMediaHandle);
				if(dResult != DI_ERR_OK)
				{
					HLIB_STD_MemFree(player);
					HxLOG_Error("SVC_CSO: CreatePlayer: DI_CSO_DestroySession failed!\n");
					return NULL;
				}
				HLIB_STD_MemFree(player);
				return NULL;
			}
		}

		for ( i=0; i<sizeof(audioEvents); ++i )
		{
			nResult = DI_AUDIO_RegisterEventCallback(player->nIdxAudioDecoder, audioEvents[i], svc_cso_audio_callback, &ulAudioCaller);
			if ( nResult != ERR_OK )
			{
				HxLOG_Error("SVC_CSO: %s : DI_AUDIO_RegisterEventCallback() failed.\n", __FUNCTION__);
				dResult = DI_CSO_DestroySession(player->hMediaHandle);
				if(dResult != DI_ERR_OK)
				{
					HLIB_STD_MemFree(player);
					HxLOG_Error("SVC_CSO: CreatePlayer: DI_CSO_DestroySession failed!\n");
					return NULL;
				}
				HLIB_STD_MemFree(player);
				return NULL;
			}
		}

		s_tPlayerEventCaller[hActionId].bHasCaller = TRUE;
		s_tPlayerEventCaller[hActionId].nVideoCaller = ulVideoCaller;
		s_tPlayerEventCaller[hActionId].nAudioCaller = ulAudioCaller;
	}
	dResult = DI_CSO_SetDecoderIndex(player->hMediaHandle, nTempVideoIndex, nTempAudioIndex);
	if(dResult != DI_ERR_OK)
	{
		dResult = DI_CSO_DestroySession(player->hMediaHandle);
		if(dResult != DI_ERR_OK)
		{
			HLIB_STD_MemFree(player);
			HxLOG_Error("SVC_CSO: CreatePlayer: DI_CSO_DestroySession failed!\n");
			return NULL;
		}
	}

	nResult = SVC_CSO_SUB_Create(player);
	if ( nResult != ERR_OK )
	{
		HxLOG_Error("Subtitle Context Create Fail... \n");
	}

	s_tPlayerEntry[hActionId] = player;

	return player;
}

HERROR SVC_CSO_Destroy(SvcCso_t *player)
{
	DI_ERR_CODE dResult;
	SVC_CSO_ASSERT(player);

	if ( player->eState != eSVC_CSO_STATE_STOPPED )
	{
		HxLOG_Print("SVC_CSO: DestroyPlayer: the playback is not stopped and so the playback\
				will be stopped forcely.\n");
		SVC_CSO_Stop(player);
	}

	dResult = DI_CSO_DestroySession(player->hMediaHandle);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("SVC_CSO: DestroyPlayer: DI_CSO_DestroySession failed!\n");
		return ERR_FAIL;
	}
	//(void) DI_CSO_SetCurrentMediaInstance(NULL);

	SVC_CSO_SUB_Destroy(player);

	s_tPlayerEntry[player->hActionId] = NULL;
	if(player != NULL)
	{
		HLIB_STD_MemFree(player);
		player=NULL;
	}

	HxLOG_Print("SVC_CSO: DestroyPlayer: A media player is destroyed.\n");

	return ERR_OK;
}

HUINT32 SVC_CSO_GetActionId(SvcCso_t *player)
{
	return player->hActionId;
}

void SVC_CSO_SetDemuxId(SvcCso_t *player, HUINT32 ulDemuxId )
{
	player->ulDemuxId = ulDemuxId;

	(void)DI_CSO_SetDemuxId(player->hMediaHandle, ulDemuxId);
}

void SVC_CSO_EnableDemuxPath(SvcCso_t *player, HBOOL enable)
{
	player->bEnableDemuxPath = enable;
}


SvcCso_t *SVC_CSO_GetActivePlayer(HUINT32 hActionId)
{
	SvcCso_t * player = NULL ;

	player = s_tPlayerEntry[hActionId];

	if ( player )
		return player;

	//HxLOG_Debug("SVC_CSO : There is no created plyaer with acticonID = %d \n", hActionId);
	return player;
}

HERROR SVC_CSO_Play(SvcCso_t *player, HUINT32 ulStartTime)
{
	SVC_CSO_ASSERT(player);

	HxLOG_Error("SVC_CSO: SVC_CSO_Play: Started.\n");

	player->ullStartTime = ulStartTime;

#if 0	/* Async */
	__svc_cso_send_command(player, SVC_CSO_LOCAL_COMMAND_PLAY, 0);
#else	/* Sync */
	__svc_cso_cmd_play(player);
#endif
	return ERR_OK;
}

HERROR SVC_CSO_Stop(SvcCso_t *player)
{
	SVC_CSO_ASSERT(player);

	if(player->bhasSubtitle == TRUE)
	{
		__svc_cso_send_subtitle(player, NULL,  sizeof(SvcCsoSub_Display_Packet_t));
		player->bhasSubtitle = FALSE;
	}

	/********************
	 *	1. check the state
	 ********************/
	if(player->eState != eSVC_CSO_STATE_PLAYING)
	{
		HxLOG_Error("State is not SVC_CSO_STATE_PLAYING now!\n");
		return ERR_FAIL;
	}

	/********************
	 *	2. send SBTL_STOP event
	 ********************/
	svc_cso_PostMsgToAp (eSVC_CSO_EVENT_SBTL_STOP, PAL_PIPE_GetActionHandle (player->hActionId), 0, 0, 0);
	__svc_cso_subtitle_close(player);

	//__svc_cso_send_event(player, SVC_CSO_EVT_MEDIA_SUB_STOP, 0);

	/********************
	 *	3. send command & event
	 ********************/
#if 0	/* Async */
	__svc_cso_send_command(player, eSVC_CSO_LOCAL_COMMAND_STOP, 0);
#else	/* Sync */
	(void)__svc_cso_cmd_stop(player);
#endif

#ifdef CONFIG_SUPPORT_ROVI /* for ROVI Cert */
#else
	if(player->bEnableDemuxPath)
#endif
	{
		HERROR nResult;
		HUINT32 ulDemuxId;
		DI_ERR_CODE dResult;

		nResult = PAL_PIPE_GetResourceId(player->hActionId, eRxRSCTYPE_DEMUX, &ulDemuxId);
		if(nResult != ERR_OK)
		{
				HxLOG_Error("SVC_CSO: PAL_PIPE_GetResourceId failed!\n");
				return ERR_FAIL;
		}

		dResult = DI_DEMUX_SetPath (ulDemuxId, DI_DEMUX_INPUT_NONE, 0, DI_DEMUX_OUTPUT_CURRENT, 0, -1);
		if(dResult != ERR_OK)
		{
				HxLOG_Error("SVC_CSO: DI_DEMUX_ReleasePath failed!\n");
				return ERR_FAIL;
		}
#ifdef CONFIG_SUPPORT_ROVI /* for ROVI Cert */
		if(SVC_OUT_SetMacrovision(player->hActionId, eMACROVISION_OFF) != ERR_OK)
		{
			HxLOG_Error("[%s, %d] SVC_OUT_SetMacrovision\n", __FUNCTION__, __LINE__);
			return ERR_FAIL;
		}
#endif		
		//VK_TASK_Sleep(500); /* Sync bwtween ReleasePlaypump and SVC_CSO_Stop */
		HxLOG_Error("CONFIG_SUPPORT_ROVI Free PlaypumpByIndex!\n");
	}

	__svc_cso_send_event(player, eSVC_CSO_EVT_MEDIA_STOPPED, 0);

	return ERR_OK;
}

HERROR SVC_CSO_Flush(SvcCso_t *player)
{
	DI_ERR_CODE dResult;
	HxLOG_Print("Seek Process Pause Step 2 : Recv. PAUSE EVENT Complete!\n");

#if 0
	dResult = DI_AUDIO_Stop(DI_AUDIO_DECODER_MAIN);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("[#ERROR] DI_AUDIO_Stop failed!\n");
		return ERR_FAIL;
	}
	dResult = DI_AUDIO_SetMediaType(DI_AUDIO_DECODER_MAIN, DI_AUDIOTYPE_MEDIA);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
		return ERR_FAIL;
	}
	dResult = DI_AUDIO_Start(DI_AUDIO_DECODER_MAIN, player->ulAudioCodecId);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("[#ERROR] DI_AUDIO_Start failed!\n");
		return ERR_FAIL;
	}
	dResult = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("[#ERROR] DI_VIDEO_Stop failed!\n");
		return ERR_FAIL;
	}
	dResult = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, player->ulVideoCodecId);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("[#ERROR] DI_VIDEO_Start failed!\n");
		return ERR_FAIL;
	}
#else

	if(DI_VIDEO_Flush(player->nIdxVideoDecoder) != DI_ERR_OK)
	{
		HxLOG_Error("SVC_CSO: DI_VIDEO_Flush failed!\n");
		return ERR_FAIL;
	}

	if(DI_AUDIO_Flush(player->nIdxAudioDecoder) != DI_ERR_OK)
	{
		HxLOG_Error("SVC_CSO: DI_AUDIO_Flush failed!\n");
		return ERR_FAIL;
	}

	dResult = DI_CSO_Flush(player->hMediaHandle);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("SVC_CSO: DI_CSO_Flush failed!\n");
		return ERR_FAIL;
	}

#endif
	HxLOG_Print("-------------------------------------------------------\n");
	HxLOG_Print("Media(Decoder) Flush Complete\n");

	return ERR_OK;
}

HERROR SVC_CSO_TrickMode(SvcCso_t *player, HBOOL bEnable)
{
	DI_ERR_CODE dResult;
	HxLOG_Print("SVC_CSO: Trick Mode : %s\n", bEnable == TRUE ? "TRUE" : "FALSE");

	dResult = DI_CSO_TrickMode(player->hMediaHandle, bEnable);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("SVC_CSO: DI_CSO_TrickMode failed!\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}


HERROR SVC_CSO_AcquireResource(SvcCso_t *player, void * resources, HUINT32 * ulsize)
{
	return ERR_OK;
}

HERROR SVC_CSO_GetDecodedPicture(SvcCso_t *player, HUINT32 *pulDecodedPic, HUINT32 *pulDisplayedPic)
{
	DI_ERR_CODE dResult;

	SVC_CSO_ASSERT(player);

	if ( pulDecodedPic == NULL || pulDisplayedPic == NULL )
	{
		HxLOG_Error("SVC_CSO: GetPlayPosition: Invaild param.)\n");
		return ERR_FAIL;
	}

	if ( player->eState != eSVC_CSO_STATE_PLAYING)
	{
		HxLOG_Error("SVC_CSO: GetPlayPosition: the player is not started yet.)\n");
		return ERR_OK;
	}

	dResult = DI_CSO_GetDecodedPicture(player->hMediaHandle, 0, pulDecodedPic, pulDisplayedPic);
	
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("DI_CSO_GetPosition failed!\n");
		return ERR_FAIL;
	}
	//HxLOG_Error("%llu ", *pullPosition_ms);

	return ERR_OK;
}

HERROR SVC_CSO_GetPlayPosition(SvcCso_t *player, HUINT64 *pullPosition_ms)
{
	DI_ERR_CODE dResult;

	SVC_CSO_ASSERT(player);

	if ( pullPosition_ms == NULL )
	{
		HxLOG_Error("SVC_CSO: GetPlayPosition: Invaild param.)\n");
		return ERR_FAIL;
	}

	if ( player->eState != eSVC_CSO_STATE_PLAYING)
	{
		HxLOG_Error("SVC_CSO: GetPlayPosition: the player is not started yet.)\n");
		return ERR_OK;
	}

	dResult = DI_CSO_GetPosition(player->hMediaHandle, 0, pullPosition_ms);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("DI_CSO_GetPosition failed!\n");
		return ERR_FAIL;
	}
	//HxLOG_Error("%llu ", *pullPosition_ms);

	return ERR_OK;
}

HERROR SVC_CSO_SetPlaySpeed(SvcCso_t *player, HINT32 nSpeed)
{
	DI_ERR_CODE dResult;
	DI_MEDIA_PlaySpeed_e eDiSpeed;

	HxLOG_Print("SVC_CSO: SetPlaySpeed (speed=%d)\n", nSpeed);

	SVC_CSO_ASSERT(player);

	switch ( nSpeed )
	{
		case 0:
			eDiSpeed = eDI_MEDIA_Speed_Pause;
			break;
		case -100:
			eDiSpeed = eDI_MEDIA_Speed_RW_X1;
			break;
		case -200:
			eDiSpeed = eDI_MEDIA_Speed_RW_X2;
			break;
		case -400:
			eDiSpeed = eDI_MEDIA_Speed_RW_X4;
			break;
		case -800:
			eDiSpeed = eDI_MEDIA_Speed_RW_X8;
			break;
		case -1600:
			eDiSpeed = eDI_MEDIA_Speed_RW_X16;
			break;
		case -3200:
			eDiSpeed = eDI_MEDIA_Speed_RW_X32;
			break;
		case -6400:
			eDiSpeed = eDI_MEDIA_Speed_RW_X64;
			break;
		case -12800:
			eDiSpeed = eDI_MEDIA_Speed_RW_X128;
			break;
		case -25600:
			eDiSpeed = eDI_MEDIA_Speed_RW_X256;
			break;
		case 100:
			eDiSpeed = eDI_MEDIA_Speed_Normal;
			break;
		case 200:
			eDiSpeed = eDI_MEDIA_Speed_FF_X2;
			break;
		case 400:
			eDiSpeed = eDI_MEDIA_Speed_FF_X4;
			break;
		case 800:
			eDiSpeed = eDI_MEDIA_Speed_FF_X8;
			break;
		case 1600:
			eDiSpeed = eDI_MEDIA_Speed_FF_X16;
			break;
		case 3200:
			eDiSpeed = eDI_MEDIA_Speed_FF_X32;
			break;
		case 6400:
			eDiSpeed = eDI_MEDIA_Speed_FF_X64;
			break;
		case 12800:
			eDiSpeed = eDI_MEDIA_Speed_FF_X128;
			break;
		case 25600:
			eDiSpeed = eDI_MEDIA_Speed_FF_X256;
			break;
		default:
			HxLOG_Error("not supported speed (%d) \n", nSpeed);
			eDiSpeed = eDI_MEDIA_Speed_None;
			break;
	}

	if(eDiSpeed < eDI_MEDIA_Speed_Pause || eDiSpeed == eDI_MEDIA_Speed_None)
	{
		HxLOG_Error("SVC_CSO: Decoder not supported speed (%d) \n", nSpeed);
		return ERR_FAIL;
	}

	if(player->nSpeed == nSpeed)
	{
		HxLOG_Error("SVC_CSO: Same Speed and no change speed, Old Speed: %d, New Speed: %d\n", player->nSpeed, nSpeed);
		return ERR_OK;
	}

	dResult = DI_CSO_SetSpeed(player->hMediaHandle, 0, eDiSpeed);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("DI_CSO_SetSpeed failed!\n");
		return ERR_FAIL;
	}

	/* Update Current Speed */
	player->nSpeed = nSpeed;

	/* Audio Mute Patch without 1FF */
	if(eDiSpeed == eDI_MEDIA_Speed_Normal)
	{
		/* action : UnMute */
		HxLOG_Print("SVC_CSO: eDiSpeed = %d, Disable Mute\n", eDiSpeed);
		PAL_AUDIO_SetHide(eAvHideClient_Decoder, FALSE);
	}
	else
	{
		/* action : Mute */
		HxLOG_Print("SVC_CSO: eDiSpeed = %d, Enable Mute\n", eDiSpeed);
		PAL_AUDIO_SetHide(eAvHideClient_Decoder, TRUE);
	}
	return ERR_OK;
}

typedef	struct	SVC_CSO_PLAYPUMP_INSTANCE
{
	/* CHECK: P_PLAYPUMP_INSTANCE in "di_pb.c"*/
	void	*playpump;
	void	*allpassPidChannel;
	HBOOL	bIsUsed;
	HUINT32	ulBuffClass; /*0:small buffer, 1:large buffer*/
	HINT32	iDemuxId; /*only available when allpass pidchannel used.*/
} SVC_CSO_PLAYPUMP_INSTANCE_t;

HERROR SVC_CSO_GetResourceMap(SvcCso_Resource_t **pMap)
{
	HUINT32 i = 0;
	DI_ERR_CODE dResult;
	SVC_CSO_PLAYPUMP_INSTANCE_t* pPlaypumpInstance = NULL;
	SvcCso_Resource_t* pTempMap = NULL;
	void* pPlaypumpList = NULL;

	pTempMap = HLIB_STD_MemAlloc(sizeof(SvcCso_Resource_t));
	if(pTempMap == NULL)
	{
		HxLOG_Error("Out of Memory!\n");
		return ERR_FAIL;
	}
	memset(pTempMap, 0x00, sizeof(SvcCso_Resource_t));

	/*************************
	 *	1. Getting Playpump Map
	 *************************/
	/* pPlaypumpList is allocated in di_pvr_play module. */
	dResult = DI_PVR_PLAY_GetPlaypumpResourceMap(&pPlaypumpList, &pTempMap->ulNumOfPlaypump);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("DI_PVR_GetPlaypumpMap failed!\n");
		if(pTempMap!=NULL)
		{
			HLIB_STD_MemFree(pTempMap);
			pTempMap=NULL;
		}
		return ERR_FAIL;
	}
	pPlaypumpInstance = (SVC_CSO_PLAYPUMP_INSTANCE_t*)pPlaypumpList;
	if(pPlaypumpInstance != NULL && pTempMap->ulNumOfPlaypump > 0)
	{
		pTempMap->pulPlaypumpList = HLIB_STD_MemAlloc(pTempMap->ulNumOfPlaypump * sizeof(unsigned int));
		if( pTempMap->pulPlaypumpList != NULL)
		{
			for(i = 0 ; i < pTempMap->ulNumOfPlaypump; i++)
			{
				pTempMap->pulPlaypumpList[i] = (HUINT32)(pPlaypumpInstance+i)->playpump;
				HxLOG_Debug("%s(%d) pTempMap->pulPlaypumpList[%d] = [%08X]\n", __FUNCTION__, __LINE__, i, pTempMap->pulPlaypumpList[i]);
			}
		}
	}
	if(pPlaypumpList != NULL)
	{
		/* pPlaypumpList is allocated in di_pvr_play module. */
		VK_MEM_Free(pPlaypumpList);
		pPlaypumpList = NULL;
	}

	dResult = DI_AUDIO_GetMixerHandle((void *)&pTempMap->ulAudioMixerHandle);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("@@@@@ [%s:%d] Failed to get AudioMixer handle\n", __func__, __LINE__);

		if(pTempMap != NULL)
		{
			if(pTempMap->pulPlaypumpList != NULL)
			{
				HLIB_STD_MemFree(pTempMap->pulPlaypumpList);
				pTempMap->pulPlaypumpList = NULL;
			}

			HLIB_STD_MemFree(pTempMap);
			pTempMap = NULL;
		}

		return ERR_FAIL;
	}
		
	*pMap = pTempMap;
	return ERR_OK;
}

void SVC_CSO_ReleaseResourceMap(void* pMap)
{
	SvcCso_Resource_t *tmpResourceMap=NULL;

	if(pMap == NULL)
	{
		HxLOG_Error("All Ready Free Resource MAP or Bad Param!\n");
		return;
	}

	tmpResourceMap = pMap;
	if(tmpResourceMap->pulPlaypumpList != NULL)
	{
		HLIB_STD_MemFree(tmpResourceMap->pulPlaypumpList);
		tmpResourceMap->pulPlaypumpList = NULL;
	}

	if(pMap != NULL)
	{
		HLIB_STD_MemFree(pMap);
		pMap = NULL;
	}

	return;
}

HERROR SVC_CSO_AcquireDtcpIpKey(HINT8 **pDtcpKey, HINT32 *nSize)
{
	if(DI_CSO_DTCP_AcquireDtcpIpKey(pDtcpKey, nSize) != DI_ERR_OK)
	{
		return ERR_FAIL;
	}
	return ERR_OK;
}

void SVC_CSO_ReleaseDtcpIpKey(HINT8 *pDtcpKey)
{
	return DI_CSO_DTCP_ReleaseDtcpIpKey(pDtcpKey);
}

HERROR SVC_CSO_GetPlaypumpIndex(SvcCso_t *player, HUINT32* pulIndex)
{
	DI_ERR_CODE dResult;

	dResult = DI_PVR_PLAY_GetPlaypumpIndex(pulIndex);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("DI_PVR_GetPlaypumpIndex failed!\n");
		return ERR_FAIL;
	}
#if 0 /* TODO : WILL DELETE - jhchoi2*/
	dResult = DI_CSO_SetCurrentPlaypump(player->hMediaHandle, *pulIndex);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("DI_CSO_SetCurrentPlaypump failed!\n");
		return ERR_FAIL;
	}

#ifdef CONFIG_SUPPORT_ROVI /* for ROVI Cert */
	dResult = DI_DEMUX_SetPath(player->ulDemuxId , DI_DEMUX_INPUT_MEDIA_PLAYER, 0, DI_DEMUX_OUTPUT_EXTERNAL, 0, -1);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("SVC_CSO: DI_DEMUX_SetPath failed!\n");
		return ERR_FAIL;
	}
#endif	/* for ROVI Cert */
#endif
	return ERR_OK;
}

HERROR SVC_CSO_SetCurrentPlaypump(SvcCso_t *player, HUINT32 ulIndex)
{
	DI_ERR_CODE dResult;

	dResult = DI_CSO_SetCurrentPlaypump(player->hMediaHandle, ulIndex);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("DI_CSO_SetCurrentPlaypump failed!\n");
		return ERR_FAIL;
	}

#ifdef CONFIG_SUPPORT_ROVI /* for ROVI Cert */
#else
	if(player->bEnableDemuxPath)
#endif
	{
		dResult = DI_DEMUX_SetPath(player->ulDemuxId , DI_DEMUX_INPUT_MEDIA_PLAYER, 0, DI_DEMUX_OUTPUT_EXTERNAL, 0, -1);
		if(dResult != DI_ERR_OK)
		{
			HxLOG_Error("SVC_CSO: DI_DEMUX_SetPath failed!\n");
			return ERR_FAIL;
		}
	}
	return ERR_OK;
}

HERROR SVC_CSO_FreePlaypumpIndex(SvcCso_t *player, HUINT32 ulIndex)
{
	DI_ERR_CODE dResult;

	dResult = DI_PVR_PLAY_ReleasePlaypumpByIndex(ulIndex);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("DI_PVR_FreePlaypumpIndex failed!\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}
static DI_CSO_PumpType_e svc_cso_convert_di_pumptype(SvcCso_PumpType_e ePumpType)
{
	DI_CSO_PumpType_e eDiPumpType = DI_CSO_PUMPTYPE_AV;
	switch(ePumpType)
	{
		case eSVC_CSO_PUMPTYPE_AV:
			eDiPumpType = DI_CSO_PUMPTYPE_AV;
			break;
		case eSVC_CSO_PUMPTYPE_A:
			eDiPumpType = DI_CSO_PUMPTYPE_A;
			break;
		case eSVC_CSO_PUMPTYPE_V:
			eDiPumpType = DI_CSO_PUMPTYPE_V;
			break;
		default :
			HxLOG_Error("Can't Find PumpType : %d\n", ePumpType);
			break;
	}
	return eDiPumpType;
}

static DI_CSO_InputStreamType_e svc_cso_convert_di_input_streamtype(SvcCso_InputStreamType_e eInputStreamType)
{
	DI_CSO_InputStreamType_e eDiStreamType = DI_CSO_PUMPTYPE_AV;
	switch(eInputStreamType)
	{
		case eSVC_CSO_INPUT_STREAMTYPE_TS_MUX:
			eDiStreamType = DI_CSO_INPUT_STREAMTYPE_TS_MUX;
			break;
		case eSVC_CSO_INPUT_STREAMTYPE_ES_DIRECT:
			eDiStreamType = DI_CSO_INPUT_STREAMTYPE_ES_DIRECT;
			break;
		case eSVC_CSO_INPUT_STREAMTYPE_TS_LIVE:
			eDiStreamType = DI_CSO_INPUT_STREAMTYPE_TS_LIVE;
			break;
		default :
			HxLOG_Error("Can't Find StreamType : %d\n", eInputStreamType);
			break;
	}
	return eDiStreamType;
}


void SVC_CSO_SetPlaypumpType(SvcCso_t *player, HUINT32 ulIndex, SvcCso_PumpType_e ePumpType)
{
	DI_ERR_CODE dResult;

	dResult = DI_CSO_SetPlaypumpType(player->hMediaHandle, ulIndex, svc_cso_convert_di_pumptype(ePumpType));
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("DI_CSO_SetCurrentPlaypump failed!\n");
		return;
	}

#ifdef CONFIG_SUPPORT_ROVI /* for ROVI Cert */
#else
	if(player->bEnableDemuxPath)
#endif
	{
		dResult = DI_DEMUX_SetPath(player->ulDemuxId , DI_DEMUX_INPUT_MEDIA_PLAYER, 0, DI_DEMUX_OUTPUT_EXTERNAL, 0, -1);
		if(dResult != DI_ERR_OK)
		{
			HxLOG_Error("SVC_CSO: DI_DEMUX_SetPath failed!\n");
			return;
		}
	}
}

HERROR SVC_CSO_GetDnPConfig(SvcCso_t *player, SvcCso_Config_t* config)
{
	if(config == NULL)
	{
		HxLOG_Error("Invalid Parameter!\n");
		return ERR_FAIL;
	}
	if ( s_media_info.config.bProgressiveDownload ) 
	{
		if ( NULL != HxSTD_StrStr(s_media_info.config.szPDLPath, "dvr://") )
		{
			HINT32 nLen = HxSTD_StrLen("dvr://");
			if ( nLen >= 0 && TRUE != HLIB_DIR_IsMountedPath((const HCHAR *)&s_media_info.config.szPDLPath[nLen]))
			{
				HxLOG_Info("DnP : Invalid Mount Path(change to realtime) : %s\n", s_media_info.config.szPDLPath);
				config->bProgressiveDownload = FALSE;
				memset(config->szPDLPath, 0x00, SVC_CSO_MAX_URL_LEN);
				return ERR_OK;
			}
			HxLOG_Info("DnP : Download Path : %s\n", s_media_info.config.szPDLPath);
		}
	}
	config->bProgressiveDownload = s_media_info.config.bProgressiveDownload;
	memcpy(config->szPDLPath, s_media_info.config.szPDLPath, SVC_CSO_MAX_URL_LEN);
	return ERR_OK;
}

void SVC_CSO_SetDuration(SvcCso_t *player, HUINT64 ullDuration)
{
	/* Update Duration Info to App */
	/* parkjh4 : Do not send position info currently */
	//__svc_cso_event_handler(player, eSVC_CSO_EVENT_PLAY_POSITION_CHANGED);
	/* Set Duration to DI */
	(void)DI_CSO_SetDuration(player->hMediaHandle, ullDuration);
	HxLOG_Debug("ullDuration %llu !\n", ullDuration);
}

void SVC_CSO_SetFirstPosition(SvcCso_t *player, HUINT64 ullFirstPosition)
{
	(void)DI_CSO_SetFirstPosition(player->hMediaHandle, ullFirstPosition);
}

void SVC_CSO_SetInputStreamType(SvcCso_t *player, SvcCso_InputStreamType_e eInputStreamType)
{
	(void)DI_CSO_SetInputStreamType(player->hMediaHandle, svc_cso_convert_di_input_streamtype(eInputStreamType));
}

HERROR SVC_CSO_GetActiveVideoTrack(SvcCso_t *player, SvcCso_VideoTrack_t *pVideoTrack)
{
	(void)DI_CSO_GetActiveVideoTrack(player->hMediaHandle, &pVideoTrack->eVideoCodec, &pVideoTrack->ulVideoStreamId);
	return ERR_OK;
}

HBOOL  SVC_CSO_IsVideoCodecSupport(DxVideoCodec_e codec)
{
	HBOOL bSupported = FALSE;
	DI_VIDEO_FORMAT eDiVideoCodec = DI_VIDEO_FORMAT_NONE;
	
	if ( s_ulMaxVideoDecoder == 0 )
	{
		HxLOG_Error("\t Not Initialized!!! \n");
		bSupported = FALSE;
	} 
	else
	{
		eDiVideoCodec = __svc_cso_video_codec_to_di(codec);
		if (  eDiVideoCodec == DI_VIDEO_FORMAT_NONE )
		{
			HxLOG_Error("\tVideo codec is not supported. ulDecId : 0\n");
			bSupported = FALSE;
		}
		else
		{
			/* TODO Do more things for Multi Video Decorder */
			if( (s_pstDiVideoCapability[0].eSupportedFormat & eDiVideoCodec) == 0)
			{
				HxLOG_Error("\tVideo codec is not supported. ulDecId : 0\n");
				bSupported = FALSE;
			}
			else
			{
				bSupported = TRUE;
			}
		}	
	}
	
	return bSupported;
}

HERROR SVC_CSO_SetActiveVideoTrack(SvcCso_t *player, const SvcCso_VideoTrack_t *pVideoTrack)
{
	DI_ERR_CODE dResult;
	SVC_CSO_ASSERT(player);
	if(pVideoTrack == NULL)
	{
		if(player->eState == eSVC_CSO_STATE_PLAYING && player->ulVideoCodecId != DI_VIDEO_FORMAT_NONE)
		{			
			dResult = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
			if(dResult != DI_ERR_OK)
			{
				HxLOG_Error("[#ERROR] DI_VIDEO_Stop failed!\n");
				return ERR_FAIL;
			}
			HxLOG_Debug("[#Debug] Runtime DI_VIDEO_Stop \n");			
			player->ulVideoCodecId = DI_VIDEO_FORMAT_NONE;
			return ERR_OK;
		}
		else
		{
			HxLOG_Error("[#Debug] Already DI_VIDEO_Stop!\n");
			return ERR_OK;
		}		
	}
	
	if(pVideoTrack->ulVideoStreamId == 0)
	{
		player->ulVideoCodecId = DI_VIDEO_FORMAT_NONE;
		HxLOG_Debug("Video Track Unknow\n");
	}
	else
	{
		player->ulVideoCodecId = __svc_cso_video_codec_to_di(pVideoTrack->eVideoCodec);
	}
	player->ulVideoStreamId = pVideoTrack->ulVideoStreamId;

	(void)DI_CSO_SetActiveVideoTrack(player->hMediaHandle, player->ulVideoCodecId, pVideoTrack->ulVideoStreamId);

	if(player->eState == eSVC_CSO_STATE_PLAYING)
	{
		dResult = DI_VIDEO_Stop(DI_VIDEO_DECODER_MAIN);
		if(dResult != DI_ERR_OK)
		{
			HxLOG_Error("[#ERROR] DI_VIDEO_Stop failed!\n");
			return ERR_FAIL;
		}
		dResult = DI_VIDEO_Start(DI_VIDEO_DECODER_MAIN, player->ulVideoCodecId);
		if(dResult != DI_ERR_OK)
		{
			HxLOG_Error("[#ERROR] DI_VIDEO_Start failed!\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

HERROR SVC_CSO_GetActiveAudioTrack(SvcCso_t *player, SvcCso_AudioTrack_t *pAudioTrack)
{
	SVC_CSO_ASSERT(player);

	(void)DI_CSO_GetActiveAudioTrack(player->hMediaHandle, &pAudioTrack->eAudioCodec, &pAudioTrack->ulAudioStreamId);
	return ERR_OK;
}

HERROR SVC_CSO_SetActiveAudioTrack(SvcCso_t *player, const SvcCso_AudioTrack_t *pAudioTrack)
{
	DI_ERR_CODE dResult;
	HERROR eRet = ERR_OK;

	if(pAudioTrack == NULL)
	{
		if(player->eState == eSVC_CSO_STATE_PLAYING && player->ulAudioCodecId != DI_AUDFORMAT_NONE)
		{		
			dResult = DI_AUDIO_Stop(player->nIdxAudioDecoder);
			if(dResult != DI_ERR_OK)
			{
				HxLOG_Error("[#ERROR] DI_AUDIO_Stop failed!\n");
				return ERR_FAIL;
			}
			HxLOG_Debug("[#Debug] Runtime DI_AUDIO_Stop \n");
			player->ulAudioCodecId = DI_AUDFORMAT_NONE;
			return ERR_OK;
		}
		else
		{
			HxLOG_Error("[#Debug] Already DI_AUDIO_Stop!\n");
			return ERR_OK;
		}
	}
	
	if(pAudioTrack->ulAudioStreamId == 0)
	{
		player->ulAudioCodecId = DI_AUDFORMAT_NONE;
		HxLOG_Debug("Audio Track Unknow\n");
	}
	else
	{
		player->ulAudioCodecId = __svc_cso_audio_codec_to_di(pAudioTrack->eAudioCodec);
	}
	player->ulAudioStreamId = pAudioTrack->ulAudioStreamId;

	eRet = PAL_OUT_SetDigitalAudioOutCodec(pAudioTrack->eAudioCodec, eDxAUDIO_CODEC_UNKNOWN);
	if ( eRet != ERR_OK )
	{
		HxLOG_Error("[#ERROR] PAL_OUT_SetDigitalAudioOutCodec failed.\n");
		return ERR_FAIL;
	}

	(void)DI_CSO_SetActiveAudioTrack(player->hMediaHandle, player->ulAudioCodecId, pAudioTrack->ulAudioStreamId);

	if(player->eState == eSVC_CSO_STATE_PLAYING)
	{
		dResult = DI_AUDIO_Stop(player->nIdxAudioDecoder);
		if(dResult != DI_ERR_OK)
		{
			HxLOG_Error("[#ERROR] DI_AUDIO_Stop failed!\n");
			return ERR_FAIL;
		}
		dResult = DI_AUDIO_SetMediaType(player->nIdxAudioDecoder, DI_AUDIOTYPE_MEDIA);
		if(dResult != DI_ERR_OK)
		{
			HxLOG_Error("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
			return ERR_FAIL;
		}
		dResult = DI_AUDIO_Start(player->nIdxAudioDecoder, player->ulAudioCodecId);
		if(dResult != DI_ERR_OK)
		{
			HxLOG_Error("[#ERROR] DI_AUDIO_Start failed!\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

HERROR SVC_CSO_AudioFlush(SvcCso_t *player, HUINT32 unAudioTrack)
{
	HERROR hErr = ERR_OK;
	DI_ERR_CODE dResult = DI_ERR_OK;
	HUINT32 unAudioCodecId = 0, unAudioStreamId = 0;
	
	HxLOG_Print("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);

	if(player == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] player is NULL\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_done;
	}

	dResult = DI_CSO_GetActiveAudioTrack(player->hMediaHandle, &unAudioCodecId, &unAudioStreamId);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to get an active audio track, dResult = %d\033[0m\n", __func__, __LINE__, dResult);
		hErr = ERR_FAIL;
		goto func_done;	
	}
	
	if(unAudioStreamId == 0)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] unAudioStreamId is 0\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_done;		
	}
	else if(unAudioStreamId > SVC_CSO_NETFLIX_START_AUDIO_PID + 3)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] unAudioStreamId(0x%X) is bigger than 0x%X\033[0m\n", __func__, __LINE__, unAudioStreamId, SVC_CSO_NETFLIX_START_AUDIO_PID + 3);
		hErr = ERR_FAIL;
		goto func_done;		
	}
	else
	{	
		unAudioStreamId = SVC_CSO_NETFLIX_START_AUDIO_PID + unAudioTrack;
	}

	player->ulAudioStreamId = unAudioStreamId;
	player->ulAudioCodecId = unAudioCodecId;

	dResult = DI_CSO_SetActiveAudioTrack(player->hMediaHandle, unAudioCodecId, unAudioStreamId);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to set an active audio track, dResult = %d\033[0m\n", __func__, __LINE__, dResult);
		hErr = ERR_FAIL;
		goto func_done;	
	}

	dResult = DI_AUDIO_Stop(player->nIdxAudioDecoder);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("[#ERROR] DI_AUDIO_Stop failed!\n");
		hErr = ERR_FAIL;
		goto func_done;
	}

	dResult = DI_AUDIO_SetMediaType(player->nIdxAudioDecoder, DI_AUDIOTYPE_MEDIA);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("[#ERROR] DI_AUDIO_SetMediaType failed!\n");
		hErr = ERR_FAIL;
		goto func_done;
	}

func_done:	
	HxLOG_Print("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return hErr;
}

HERROR SVC_CSO_AudioRestart(SvcCso_t *player)
{
	HERROR hErr = ERR_OK;
	DI_ERR_CODE dResult = DI_ERR_OK;
	HxLOG_Print("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	if(player == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] player is NULL\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_done;
	}

	dResult = DI_AUDIO_Start(player->nIdxAudioDecoder, player->ulAudioCodecId);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("[#ERROR] DI_AUDIO_Start failed!\n");
		hErr = ERR_FAIL;
		goto func_done;
	}

	dResult = DI_CSO_Flush(player->hMediaHandle);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to invalidate STC Channel, dResult = %d\033[0m\n", __func__, __LINE__, dResult);
		hErr = ERR_FAIL;
		goto func_done; 	
	}
	
func_done:
	HxLOG_Print("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return hErr;
}

HERROR SVC_CSO_AudioCodecSwitch(SvcCso_t *player, HUINT32 unAudioTrack, HUINT32 unAudioCodec)
{
	HERROR hErr = ERR_OK;
	HUINT32 unAudioCodecId = 0, unAudioStreamId = 0;
	DI_ERR_CODE dResult = DI_ERR_OK;

	HxLOG_Print("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
	
	if(player == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] player is NULL\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_done;
	}
	
	dResult = DI_CSO_GetActiveAudioTrack(player->hMediaHandle, &unAudioCodecId, &unAudioStreamId);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to get an active audio track, dResult = %d\033[0m\n", __func__, __LINE__, dResult);
		hErr = ERR_FAIL;
		goto func_done;	
	}
	
	unAudioStreamId = SVC_CSO_NETFLIX_START_AUDIO_PID + unAudioTrack;
	
	/* Set changed audio settings */
	player->ulAudioStreamId = unAudioStreamId;
#if 0
	player->ulAudioCodecId = __svc_cso_audio_codec_to_di((AUDIO_Codec_t)unAudioCodec);
#else
	player->ulAudioCodecId = __svc_cso_audio_codec_to_di((DxAudioCodec_e)unAudioCodec);
#endif
	
#if 0 /* TODO : This will be applied in accordance with the Netflix's feedback for next model certi. */
	/* 
	 * - Fixed Netflix NTS's audio mute issue when changing 5.1 -> stereo (HUMXAAAAAA-201/HUMXAAAAAA-216) 
	 * - SYNC-001-TC1 issue under (HDMI to 5.1 receiver to TV) 
	 *       5.1 audio can be heard from 5.1 speakers. When changing from 5.1 to stereo during playback, 
	 *       stereo audio is not heard from 5.1 speakers
	 *       Stereo is not heard on TV
	 * - NTS HUMXAAAAAA-201 : https://issues.nrd.netflix.com/browse/HUMXAAAAAA-201 
	 * - NTS HUMXAAAAAA-216 : https://issues.nrd.netflix.com/browse/HUMXAAAAAA-216 
	 */
	(void)PAL_OUT_SetDigitalAudioOutCodec(unAudioCodec, eDxAUDIO_CODEC_UNKNOWN);
#endif
	
	dResult = DI_CSO_SetActiveAudioTrack(player->hMediaHandle, player->ulAudioCodecId, player->ulAudioStreamId);
	if(dResult != DI_ERR_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to set an active audio track, dResult = %d\033[0m\n", __func__, __LINE__, dResult);
		hErr = ERR_FAIL;
		goto func_done;	
	}

func_done:
	HxLOG_Print("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return hErr;
}

HERROR SVC_CSO_CreateAudioFadeTask(SvcCso_t *player)
{
	HERROR hErr = ERR_OK;
	HINT32 nVkErr = VK_OK;

	if(player == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] player is NULL\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_error;
	}

	nVkErr = VK_SEM_Create(&stAudioFade.ulLockId, SVC_CSO_AUDIOFADE_SEMAPHORE_NAME, VK_SUSPENDTYPE_PRIORITY);
	if(nVkErr != VK_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to create AudioFade Semaphore, nVkErr = %d\033[0m\n", __func__, __LINE__, nVkErr);
		hErr = ERR_FAIL;
		goto func_error;
	}

	nVkErr = VK_MSG_Create(SVC_CSO_AUDIOFADE_QUEUE_SIZE, sizeof(SvcCso_Msg_AudioFade_t), SVC_CSO_AUDIOFADE_QUEUE_NAME, &stAudioFade.ulMsgId, VK_SUSPENDTYPE_PRIORITY);
	if(nVkErr != VK_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to create AudioFade Message Queue, nVkErr = %d\033[0m\n", __func__, __LINE__, nVkErr);
		hErr = ERR_FAIL;
		goto func_error;
	}

	nVkErr = VK_TASK_Create(__svc_cso_audiofade_task, 
							SVC_CSO_AUDIOFADE_TASK_PRIORITY, 
							SVC_CSO_AUDIOFADE_TASK_STACK_SIZE, 
							SVC_CSO_AUDIOFADE_TASK_NAME, 
							(void *)player, 
							&stAudioFade.ulTaskId, 
							1 /* Joinable */);
	if(nVkErr != VK_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to create AudioFade Task, nVkErr = %d\033[0m\n", __func__, __LINE__, nVkErr);
		hErr = ERR_FAIL;
		goto func_error;
	}

	nVkErr = VK_TASK_Start(stAudioFade.ulTaskId);
	if(nVkErr != VK_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to start AudioFade Task, nVkErr = %d\033[0m\n", __func__, __LINE__, nVkErr);
		hErr = ERR_FAIL;
		goto func_error;
	}

	return hErr;
	
func_error:
	if(stAudioFade.ulLockId != 0)
	{
		VK_SEM_Destroy(stAudioFade.ulLockId);
		stAudioFade.ulLockId = 0;
	}
	if(stAudioFade.ulMsgId != 0)
	{
		VK_MSG_Destroy(stAudioFade.ulMsgId);
		stAudioFade.ulMsgId = 0;
	}
	if(stAudioFade.ulTaskId != 0)
	{
		VK_TASK_Destroy(stAudioFade.ulTaskId);
		stAudioFade.ulTaskId = 0;
	}
	return hErr;
}

HERROR SVC_CSO_DestroyAudioFadeTask(SvcCso_t *player)
{
	HERROR hErr = ERR_OK;
	HBOOL bRunning = FALSE;
	HUINT32 unWaitCnt = 0, unVolume = 0;
	SvcCso_Status_AudioFade_e eStatus = eSVC_CSO_STATUS_AUDIOFADE_STOPPED;
	
	if(player == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] player is NULL\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_done;
	}

	bRunning = FALSE;
	__svc_cso_audiofade_set_running(bRunning);

	while(1)
	{
		eStatus = __svc_cso_audiofade_get_status();
		if(eStatus == eSVC_CSO_STATUS_AUDIOFADE_TERMINATE)
		{
			HxLOG_Error("\033[1;31m @@@@@ [%s:%d] AudioFade Task has been destroyed.\033[0m\n", __func__, __LINE__);
			break;
		}
				
		if(unWaitCnt++ > 30)
		{
			HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Timeout! Can't wait to destroy AudioFade Task..\033[0m\n", __func__, __LINE__);
			break;
		}
		
		VK_TASK_Sleep(SVC_CSO_AUDIOFADE_TASK_SLEEP);
	}

	eStatus = eSVC_CSO_STATUS_AUDIOFADE_STOPPED;
	__svc_cso_audiofade_set_status(eStatus);

	unVolume = 0;
	__svc_cso_audiofade_set_audio_decoder_volume(unVolume);

	AUDIOFADE_SEMA_LOCK;

	VK_TASK_Destroy(stAudioFade.ulTaskId);
	stAudioFade.ulTaskId = 0;

	VK_MSG_Destroy(stAudioFade.ulMsgId);
	stAudioFade.ulMsgId = 0;
	
	AUDIOFADE_SEMA_UNLOCK;

	VK_SEM_Destroy(stAudioFade.ulLockId);
	stAudioFade.ulLockId = 0;
	
func_done:
	return hErr;
}

HERROR SVC_CSO_SetAudioFade(SvcCso_t *player, HUINT32 unStartVolume, HUINT32 unTargetVolume, HINT32 nEase, HINT32 nEaseTimeElapsed, HINT32 nEaseTransitionFrames)
{
	HERROR hErr = ERR_OK;
	SvcCso_Info_AudioFade_t stInfo;
	
	HxLOG_Print("\033[1;36m @@@@@ [%s:%d] %s() (+)\033[0m\n", __func__, __LINE__, __func__);
		
	if(player == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] player is NULL\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_done;
	}
	
	HxLOG_Info("\033[1;31m @@@@@ [%s:%d] unStartVolume = %d\033[0m\n", __func__, __LINE__, unStartVolume);
	HxLOG_Info("\033[1;31m @@@@@ [%s:%d] unTargetVolume = %d\033[0m\n", __func__, __LINE__, unTargetVolume);
	HxLOG_Info("\033[1;31m @@@@@ [%s:%d] nEase = %d\033[0m\n", __func__, __LINE__, nEase);
	HxLOG_Info("\033[1;31m @@@@@ [%s:%d] nEaseTimeElapsed = %d\033[0m\n", __func__, __LINE__, nEaseTimeElapsed);
	HxLOG_Info("\033[1;31m @@@@@ [%s:%d] nEaseTransitionFrames = %d\033[0m\n", __func__, __LINE__, nEaseTransitionFrames);
 
	stInfo.unStartVolume = unStartVolume;
	stInfo.unTargetVolume = unTargetVolume;
	stInfo.nEase = nEase;
	stInfo.nEaseTimeElapsed = nEaseTimeElapsed;
	stInfo.nEaseTransitionFrames = nEaseTransitionFrames;

	__svc_cso_audiofade_send_event(player, eSVC_CSO_EVT_AUDIOFADE_SET_FADE, (HUINT32)&stInfo);

func_done:
	HxLOG_Print("\033[1;36m @@@@@ [%s:%d] %s() (-)\033[0m\n", __func__, __LINE__, __func__);
	return hErr;
}

HERROR SVC_CSO_GetAudioDecoderVolume(SvcCso_t *player, HUINT32 *punVolume)
{
	HERROR hErr = ERR_OK;

	if(player == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] player is NULL\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_done;
	}

	if(punVolume == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] punVolume is NULL\033[0m\n", __func__, __LINE__);
		hErr = ERR_FAIL;
		goto func_done;
	}

	*punVolume = __svc_cso_audiofade_get_audio_decoder_volume();

func_done:
	return hErr;
}

HERROR SVC_CSO_SetVideoHide(SvcCso_t *player, HBOOL bHide)
{
	
	HxLOG_Error("SVC_CSO: SVC_CSO_SetVideoHide (bHide=%d)\n", bHide);

	SVC_CSO_ASSERT(player);
	
	return PAL_VIDEO_SetHide(player->nIdxVideoDecoder, eAvHideClient_Decoder, bHide);
}
HERROR SVC_CSO_SetAudioHide(SvcCso_t *player, HBOOL bHide)
{
	
	HxLOG_Error("SVC_CSO: SVC_CSO_SetAudioHide (bHide=%d)\n", bHide);

	SVC_CSO_ASSERT(player);
	
	return PAL_AUDIO_SetHide(eAvHideClient_Decoder, bHide);
}

#if __________subtitle__________
#endif

HERROR SVC_CSO_CaptureScreen(SvcCso_t *player, const HINT8 *szSavePath)
{
	HERROR eRet = ERR_OK;
	HUINT32 ulVideoId = 0;
	PalVideo_FrameBuffer_t	stFrame;

	SVC_CSO_ASSERT(player);
	SVC_CSO_ASSERT(szSavePath);

	eRet = PAL_PIPE_GetResourceId(player->hActionId, eRxRSCTYPE_AV_DEC, &ulVideoId);
	if( eRet != ERR_OK )
	{
		return ERR_FAIL;
	}

	stFrame.ulWidth = SVC_CSO_CAPTURE_WIDTH;
	stFrame.ulHeight = SVC_CSO_CAPTURE_HEIGHT;
	stFrame.pFrameBuffer =\
			      (HUINT32 *)HLIB_STD_MemAlloc(SVC_CSO_CAPTURE_WIDTH*SVC_CSO_CAPTURE_HEIGHT*sizeof(HUINT32));
	if( stFrame.pFrameBuffer == NULL )
	{
		return ERR_FAIL;
	}

	eRet = PAL_VIDEO_CreateVideoFrameCapture(ulVideoId,
			SVC_CSO_CAPTURE_WIDTH, SVC_CSO_CAPTURE_HEIGHT, &stFrame);
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

HERROR SVC_CSO_SetSubtitleUsage(SvcCso_t *player, HBOOL bUse)
{
	SVC_CSO_ASSERT(player);

#if 0
	HERROR eRet = ERR_OK;
	eRet = SVC_CSO_SUB_SetEnable(player, bUse);
	if(eRet != ERR_OK)
	{
		HxLOG_Error("SVC_CSO: SVC_CSO_SetSubtitleUsage: \n");
		return ERR_FAIL;
	}
	if(bEnable == FALSE)
		 __svc_cso_send_subtitle(player, NULL,  sizeof(SvcCsoSub_Display_Packet_t));
#else
	player->bUseSubtitle = bUse;
#endif
	return ERR_OK;

}

HERROR SVC_CSO_SetSubtitleDisplay(SvcCso_t *player, HBOOL bEnable)
{
	HERROR eRet = ERR_OK;

	SVC_CSO_ASSERT(player);

	eRet = SVC_CSO_SUB_SetDisplay(player, bEnable);
	if(eRet != ERR_OK)
	{
		HxLOG_Error("SVC_CSO: SVC_CSO_SetSubtitleDisplay: \n");
		return ERR_FAIL;
	}
	if(bEnable == FALSE)
		 __svc_cso_send_subtitle(player, NULL,  sizeof(SvcCsoSub_Display_Packet_t));

	return ERR_OK;
}

HERROR SVC_CSO_SetSubtitleLanguage(SvcCso_t *player, HUINT32 ulLangIdx)
{
	HERROR eRet = ERR_OK;

	SVC_CSO_ASSERT(player);

	eRet = SVC_CSO_SUB_SetLanguage(player, ulLangIdx);
	if(eRet != ERR_OK)
	{
		HxLOG_Error("SVC_CSO: SVC_CSO_SetSubtitleLanguage: \n");
		return ERR_FAIL;
	}


	return ERR_OK;
}

HERROR SVC_CSO_ChangeSubtitleSyncTime(SvcCso_t *player, HINT32 ulMsTime)
{
	HERROR eRet = ERR_OK;

	SVC_CSO_ASSERT(player);

	/* TODO - ulMsTime Check */

	eRet = SVC_CSO_SUB_ChangeSyncOffsetTime(player, ulMsTime);
	if(eRet != ERR_OK)
	{
		HxLOG_Error("SVC_CSO: SVC_CSO_ChangeSubtitleSyncTime: \n");
		return ERR_FAIL;
	}

	return ERR_OK;

}

HERROR SVC_CSO_GetSubtitleDisplay(SvcCso_t *player, HBOOL *pbShow)
{
	HERROR eRet = ERR_OK;
	SVC_CSO_ASSERT(player);
	SVC_CSO_ASSERT(pbShow);

	eRet = SVC_CSO_SUB_GetDisplay(player, pbShow);
	if( eRet != ERR_OK)
	{
		HxLOG_Error("SVC_CSO: SVC_CSO_GetSubtitleDisplay: Cannot get the Subtitle info\n");
		*pbShow = 0;
		return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR SVC_CSO_GetSubtitleTotalLangCnt(SvcCso_t *player, HUINT32 *pnCount)
{
	HERROR eRet = ERR_OK;
	SvcCsoSub_LangInfo_t stLangInfo;

	SVC_CSO_ASSERT(player);
	SVC_CSO_ASSERT(pnCount);

	HxSTD_memset(&stLangInfo, 0, sizeof(stLangInfo));
	eRet = SVC_CSO_SUB_GetLanguageInfo(player, &stLangInfo);
	if( eRet != ERR_OK)
	{
		HxLOG_Error("SVC_CSO: SVC_CSO_GetSubtitleLanguageCount: Cannot get the Subtitle info\n");
		*pnCount = 0;
		return ERR_FAIL;
	}

	*pnCount = stLangInfo.ulTotalLang;

	return ERR_OK;
}

HERROR SVC_CSO_GetSubtitleCurrLangIndex(SvcCso_t *player, HUINT32 *pnIndex)
{
	HERROR eRet = ERR_OK;
	SvcCsoSub_LangInfo_t stLangInfo;

	SVC_CSO_ASSERT(player);
	SVC_CSO_ASSERT(pnIndex);

	HxSTD_memset(&stLangInfo, 0, sizeof(stLangInfo));
	eRet = SVC_CSO_SUB_GetLanguageInfo(player, &stLangInfo);
	if( eRet != ERR_OK)
	{
		HxLOG_Error("SVC_CSO: SVC_CSO_GetSubtitleLanguageCount: Cannot get the Subtitle info\n");
		return ERR_FAIL;
	}

	*pnIndex = stLangInfo.ulCurIdx;

	return ERR_OK;

}

HERROR SVC_CSO_GetSubtitleLanguageByIndex(SvcCso_t *player, HINT32 nIndex, HINT8 *szLang)
{
	HERROR eRet = ERR_OK;
	SvcCsoSub_LangInfo_t stLangInfo;

	SVC_CSO_ASSERT(player);
	SVC_CSO_ASSERT(szLang);

	HxSTD_memset(&stLangInfo, 0, sizeof(stLangInfo));
	eRet = SVC_CSO_SUB_GetLanguageInfo(player, &stLangInfo);
	if( eRet != ERR_OK)
	{
		HxLOG_Error("SVC_CSO: SVC_CSO_GetSubtitleLanguageInfo: Cannot get the Subtitle info\n");
		return ERR_FAIL;
	}

	if(nIndex > stLangInfo.ulTotalLang)
	{
		HxLOG_Error("SVC_CSO: SVC_CSO_GetSubtitleLanguageInfo: Invalid Subtitle Index\n");
		return ERR_FAIL;
	}

	//memcpy(szLang, &stLangInfo.szName[nIndex], 16);
	memcpy(szLang, &stLangInfo.szDispLang[nIndex], 16);

	return ERR_OK;
}

HERROR SVC_CSO_GetSubtitleCurSyncTime(SvcCso_t *player, HINT32 *ulMsTime)
{
	HERROR eRet = ERR_OK;

	SVC_CSO_ASSERT(player);
	SVC_CSO_ASSERT(ulMsTime);

	eRet = SVC_CSO_SUB_GetCurSyncOffsetTime(player, ulMsTime);
	if(eRet != ERR_OK)
	{
		HxLOG_Error("SVC_CSO: SVC_CSO_GetSubtitleCurSyncTime: \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR SVC_CSO_SetSubtitleFontSize(SvcCso_t *player, HUINT32 ulSize)
{
	SVC_CSO_ASSERT(player);
	if(ulSize == 0)
		player->ulSubtitleFontSize = SVC_CSO_SUB_DEFAULT_FONT_SIZE;
	else
		player->ulSubtitleFontSize = ulSize;

	return ERR_OK;

}

HERROR SVC_CSO_GetSubtitleFontSize(SvcCso_t *player, HUINT32 *ulSize)
{
	SVC_CSO_ASSERT(player);
	SVC_CSO_ASSERT(ulSize);

	*ulSize = player->ulSubtitleFontSize;

	return ERR_OK;
}

HERROR SVC_CSO_SetSubtitleDisplayPosition(SvcCso_t *player, HUINT32 ulPos)
{
	SVC_CSO_ASSERT(player);
	if(ulPos == 0)
		player->ulSubtitleDisplayPosition = SVC_CSO_SUB_DEFAULT_TEXT_POSITON;
	else
		player->ulSubtitleDisplayPosition = ulPos;

	return ERR_OK;
}

HERROR SVC_CSO_GetSubtitleDisplayPosition(SvcCso_t *player, HUINT32 *ulPos)
{
	SVC_CSO_ASSERT(player);
	SVC_CSO_ASSERT(ulPos);

	*ulPos = player->ulSubtitleDisplayPosition;

	return ERR_OK;
}
