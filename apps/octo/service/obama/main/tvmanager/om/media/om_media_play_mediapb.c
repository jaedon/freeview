/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_media_play_media.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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
#include <octo_common.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>

#include <pthread.h>

#include <svc_output.h>

#ifdef CONFIG_SUPPORT_IMMA
#include <svc_mplayercso.h>
#include <dlib_base_types.h>
//#include <mwc_util.h>
#include "cso.h" // TODO: 위치 확인.
#include "mediapb_ipc.h"
#endif

#include <svc_fs.h>
#include <svc_mplayer.h>
#include <svc_mplayersi.h>
#ifdef CONFIG_SUPPORT_IMMA
#include <svc_mplayercso.h>
#endif

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_storage.h>
#include <mgr_media.h>
#include <mgr_copyrightcontrol.h>
#include <mgr_output.h>

#include <om_download.h>
#include <om_media_play.h>

#include "_om_media_play.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#define MAX_PDL_URL				2048
#define MAX_SUBTITLE_LANG 		4
#define MAX_SUBTITLE_STR  		16
#define PTS_EOF_CORRECTION 2000 /* 2secs */
#define UNDERRUN_CONTROL_EXEMPTION_RANGE 1500
#define IFRAME_YOUTUBE_WORK_ARROUND 1
#define HLS_DISCONTINUITY
#define UPDATE_BUFFERINFO_ON_TASK
#define SUBTITLE_OFF					(-1) /* subtitle index를 -1로 주면 subtitle task가 멈춘다. */

//#define MAXDOME_OLD_PDLMODE
//#define __TEMPORARY_BLOCKED_FOR_CSO__

#ifdef CONFIG_SUPPORT_IMMA
#define AV_TRACK_CHANGE
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#ifdef CONFIG_SUPPORT_IMMA
typedef enum
{
	eCSO_NONE = 0,
	eCSO_STOPPED,
	eCSO_PROBING,
	eCSO_PROBE_FAILED,
	eCSO_PROBED,
	eCSO_PROBE_CANCELED,
	eCSO_PLAYING,						/* PLAYING */
	eCSO_PLAYING_FAILED,				/* PLAYING FAILED */
	eCSO_SEEK_REQUESTED, 				/* SEEKING */
	eCSO_DECODER_TRICKING,				/* Decoder TRICKING */
	eCSO_JUMP_TRICK_REQUESTED,			/* Seek TRICKING */
	eCSO_JUMP_TRICK_CANCEL_REQUESTED,	/* Seek TRICKING */
	eCSO_JUMP_TRICKING,					/* Seek TRICKING */
}Status_e;

typedef enum
{
	eOM_CSO_UNDERFLOW_NONSTOP=0,
	eOM_CSO_UNDERFLOW_THRESHOLD,
	eOM_CSO_UNDERFLOW_NONE,
}OM_CSO_UnderflowMode_e;

typedef struct
{
	HUINT32 		uiMediaCtrlSemaId;
	Status_e		eStatus;
	HUINT32			uiDecoderSpeed;
	time_t			ulFlushedTime;
	HUINT32			uiPrevPlayingSpeed;
	HUINT32			uiPrevDecoderSpeed;
	HBOOL			bIFrameDecodingMode;
	HINT32			uiPlayingSpeed;
#if defined(CONFIG_TIMESTAMP_64)
	HUINT64			ullPlayingTime;
	HUINT64			ullRequestPlayTime;
#else
	HUINT32			uiPlayingTime;
	HUINT32			uiRequestPlayTime;
#endif
	HUINT32			uiRequestPlaySpeed;
	HBOOL			bUnderrun;
}CalypsoStatus_t;

typedef struct
{
	HBOOL			bProgressiveDownload;
	HINT8			szPDLPath[MAX_PDL_URL];	/* progressive download buffer path */
} CalypsoConfig_t;

typedef struct
{
	HUINT32		unDestIpAddress;
	HCHAR		ucAesKey[16];
	HCHAR		ucAesIv[16];
	HCHAR		ucFmtStr[64];
	HCHAR		ucAudioCodec[8];
	HUINT32		unDataPort;
	HUINT32		unControlPort;
	HUINT32		unTimingPort;
}
CalypsoRaop_t;
#endif

typedef struct
{
	HUINT32				bDisplay;
	HUINT32             ulTotalLangCnt;
	HUINT32             ulCurrLangIdx;
	HUINT32             ulSyncTime;
	HUINT32				ulTextPosition;
	HUINT32				ulFontSize;
	HCHAR               szLang[MAX_SUBTITLE_LANG][MAX_SUBTITLE_STR];
} omMP_SUBTTL_Info_t;

typedef struct
{
	OxMediaPlay_PlayerType_e	eMediaPlayerType;
	HUINT8 						*pucLicenseData;
	HUINT8						*pucLAURL;
	HUINT8						*pucCustomData;
	HBOOL						bControlFlag;
	HBOOL						bUnderflowAutoMode;
	HINT32						nUnderflowThreshold;
	HBOOL						bUseVmx;
	HINT8						*pucVmxCasUrl;
	HINT8						*pucVmxVendorName;
	HBOOL						bVideoTrackSelected;
	HINT32						nVideoTrack;
	HBOOL						bAudioTrackSelected;
	HINT32						nAudioTrack;
} omMPMediaPlayerCache_t;

typedef struct
{
#ifdef CONFIG_SUPPORT_IMMA
	SvcCso_t					*pMediaPlayer;
	HUINT32 					ulUsedPumpCount;
	HUINT32 					ulPumpId;
	HUINT32 					*pulPumpId;
	OxMediaPlay_PumpType_e		*ePumpType;
	HUINT32 					ulCsoSessionId;
	CalypsoStatus_t 			stCsoStatus;
	HBOOL						bStartRequested;
	HBOOL						bCheckPositionValid;
	HUINT32 					ulActivatedVideoStreamId;
	HUINT32 					ulActivatedVideoCodecId;
	HUINT32 					ulActivatedAudioCodecId;
	HUINT32 					ulActivatedAudioStreamId;
	HUINT32 					ulBitrate;
	HUINT64 					ullDuration;
	HUINT64						ullStartTime;
	HUINT64						ullSeekedTimeByCso;
	HUINT64						ullFirstPts;
	HUINT64						ullBasePosition;
	HUINT32 					ulMaxDecoderTrick;
	OxMediaPlay_BufferInfo_t 	stBufferInfo;
	HUINT32 					ulDownloadRate;
	HUINT64						ullLastDecodedTime;
	HBOOL						bUnderflowAutoMode;
	OM_CSO_UnderflowMode_e		eUnderflowMode;
	HINT32						nUnderflowThreshold;
	HUINT32 					ulBufferReleaseThreshold;
	HUINT32 					ulBufferPauseThreshold;
	HBOOL						bNonstopState;
	HBOOL						bBufferingEvtNotify;
	HBOOL						bBufferingCompleted;
#ifdef MAXDOME_OLD_PDLMODE
	HUINT64						ullNonstopBaseTime;
#endif
	HINT8						*userAgent;
	CalypsoConfig_t 			stCsoConfig;
	HBOOL						bSupport_trick;
	HBOOL						bSupport_seek;
	HBOOL						bBofReached;
	HBOOL						bEofReached;
	HBOOL						bCsoEofReached;
	time_t						ulCsoEofReachedTime;
	HBOOL						bExtOpened;
	HBOOL						bDlna;
#if defined(CONFIG_MW_OUTPUT_HDAUDIO_SR_RESTRICT)
	HBOOL						bHdAudio;
	HBOOL						bHdmiMute;
#endif
	HBOOL						bAirplaying;
	HFLOAT32					fAirPlayingStartPercentage;
	CalypsoRaop_t				*stRaopSetting;
	HBOOL						bPausedPlay;
	HINT32						nStartPlaySpeed;
#ifdef HLS_DISCONTINUITY
	HBOOL						bNeedRestart;
	HBOOL						bImmediatelyRestart;
#endif
#ifdef AV_TRACK_CHANGE
	HUINT64						ullCurPosition;
	HINT32						nActivatedVideoTrackNum;
	HINT32						nActivatedAudioTrackNum;
#endif
#ifdef IFRAME_YOUTUBE_WORK_ARROUND
	HBOOL						bBufferEnoughReported;
	HBOOL						bBufferNotEnoughReported;
#endif
	HBOOL						bAVUnderrun;  /* cso에서 AV underrun event가 올라 underrun 상태인지 확인용. */
	CSO_Settings_t				*pCsoSettings; //SEAMLESS_SWITCHING
#else
	SvcMp_MediaPlayer_t			*pMediaPlayer;
#endif

	HBOOL						bCheckDisplayPic; /* CSO 로부터 Seek 후에 Pause 가 내려오는 경우만 대응. Decoder 에 frame 이 들어오는지 확인 */
	HBOOL						bOnlyAudioContents; /* Only Audio 인지 확인 */
	HBOOL						bAdaptiveStreaming; /* is adaptivestreaming */
	omMP_SUBTTL_Info_t				stSubttlInfo;
	OxMediaPlay_SubtitleStatus_t		stSubttlStatus;
	OxMediaPlay_ErrorReason_e		eErrorReason;
}omMPMediaPlayContext_t;


#ifdef CONFIG_SUPPORT_IMMA

#define CalcActionId(x) (x - eActionId_VIEW_FIRST)

static omMPMediaPlayerCache_t s_stMediaPlayerCache[NUM_VIEW_ACTION];
static char						s_BrowserUserAgent[MAX_PDL_URL]= {0,};
static omMPMediaPlayContext_t 	s_stMediaPlayer[NUM_VIEW_ACTION];
static HBOOL 					s_bIsAliveCsoBufferInfoThread[NUM_VIEW_ACTION] = {0, };
static pthread_t 				s_pCsoBufferInfoThread[NUM_VIEW_ACTION] = {0, };

#if defined(CONFIG_PROD_YSR2000)
static const HINT32 s_SupportedSpeed[] =
{
	-25600, -6400, -3200, -400, 0, 100, 400, 3200, 6400, 25600
};
#else
static const HINT32 s_SupportedSpeed[] =
{
	-6400, -3200, -1600, -800, -400, -200, 0, 100, 200, 400, 800, 1600, 3200, 6400
};
#endif

static const char *s_CsoEventAPString[eSVC_CSO_EVENT_EIT_CHANGED-eSEVT_MEDIA_START + 1] =
{
	"NONE\0", 									//eSVC_CSO_EVENT_NONE                   = eSEVT_MEDIA_START(0x00400000),
	"[Player] Conneting...\0", 					//eSVC_CSO_EVENT_CONNECTING,
	"[Player] Probe Finished!\0", 				//eSVC_CSO_EVENT_PROBE_FINISHED,
	"[Player] Probe failed!\0", 				//eSVC_CSO_EVENT_PROBE_FAILED,
	"[Player] Playing...\0", 					//eSVC_CSO_EVENT_PLAYING,
	"[Player] Stopped!\0", 						//eSVC_CSO_EVENT_STOPPED,
	"[Player] Paused!\0", 						//eSVC_CSO_EVENT_PAUSE,
	"[Player] Reached to Begin of Stream!\0", 	//eSVC_CSO_EVENT_BEGIN_OF_STREAM,
	"[Player] Reached to End of Stream!\0", 	//eSVC_CSO_EVENT_END_OF_STREAM,
	"[Player] Position Changed!\0", 			//eSVC_CSO_EVENT_PLAY_POSITION_CHANGED,
	"[Player] Play speed changed!\0", 			//eSVC_CSO_EVENT_PLAY_SPEED_CHANGED,
	"[Player] Got video new frame!\0", 			//eSVC_CSO_EVENT_VIDEO_NEW_FRAME,
	"[Player] Got video underrun!\0", 			//eSVC_CSO_EVENT_VIDEO_UNDERRUN,
	"[Player] Got audio new frame!\0", 			//eSVC_CSO_EVENT_AUDIO_NEW_FRAME,
	"[Player] Got audio underrun!\0",			//eSVC_CSO_EVENT_AUDIO_UNDERRUN
	"[Player] Audio decoder volume changed!\0",	//eSVC_CSO_EVENT_AUDIO_DECODER_VOLUME_CHANGED
	"[Subtitle] Started!\0", 					//eSVC_CSO_EVENT_SBTL_START,
	"[Subtitle] Stoped!\0", 					//eSVC_CSO_EVENT_SBTL_STOP,
	"[Subtitle] Updated!\0", 					//eSVC_CSO_EVENT_SBTL_UPDATE,
	"[Player] Error!\0", 						//eSVC_CSO_EVENT_ERROR,
	"[Player] Seek Error!\0",					//eSVC_CSO_EVENT_SEEK_ERROR
	"[Buffering] Can play now!\0", 				//eSVC_CSO_EVENT_BUFFERING_CANPLAY,
	"[Buffering] Not Enough!\0", 				//eSVC_CSO_EVENT_BUFFERING_NOT_ENOUGH,
	"[Buffering] Enough!\0", 					//eSVC_CSO_EVENT_BUFFERING_ENOUGH,
	"[Buffering] Finished!\0",					//eSVC_CSO_EVENT_BUFFERING_FINISHED,
	"[Buffering] Updated!\0", 					//eSVC_CSO_EVENT_BUFFERING_UPDATED,
	"[OutputControl] Need Output Control!\0", 	//eSVC_CSO_EVENT_OUTPUT_CONTROL,

	"Other Event\0", //	eSVC_CSO_EVENT_PLAY_START,				// DD player starting
	"Other Event\0", //	eSVC_CSO_EVENT_PAT_TIMEOUT,
	"Other Event\0", // eSVC_CSO_EVENT_PMT_PIDREMOVED,
	"Other Event\0", //	eSVC_CSO_EVENT_PMT_PIDCHANGED,
	"Other Event\0", //	eSVC_CSO_EVENT_PMT_CHANGED,
	"Other Event\0", //	eSVC_CSO_EVENT_PMT_TIMEOUT,
	"Other Event\0", //	eSVC_CSO_EVENT_SIT_CHANGED,
	"Other Event\0", //	eSVC_CSO_EVENT_SIT_TIMEOUT,
	"Other Event\0", //	eSVC_CSO_EVENT_EIT_TIMEOUT,
	"Other Event\0" //	eSVC_CSO_EVENT_EIT_CHANGED,
};

static const char *s_CsoStatusString[eCSO_JUMP_TRICKING+1] =
{
		"[Status] None\0", 							//eCSO_NONE,
		"[Status] Stopped!\0", 						//eCSO_STOPPED,
		"[Status] Probing...\0", 					//eCSO_PROBING,
		"[Status] Probe Failed!\0", 				//eCSO_PROBE_FAILED,
		"[Status] Probed!\0",						//eCSO_PROBED,
		"[Status] Probe Canceled!!\0", 				//eCSO_PROBE_CANCELED,
		"[Status] Playing...\0", 					//eCSO_PLAYING,			/* PLAYING */
		"[Status] Seek Requested!\0", 				//eCSO_SEEK_REQUESTED, 	/* SEEKING */
		"[Status] Decoder Tricking...\0", 			//eCSO_DECODER_TRICKING,			/* Decoder TRICKING */
		"[Status] Jump Trick Requested!\0", 		//eCSO_JUMP_TRICK_REQUESTED,		/* Seek TRICKING */
		"[Status] Jump Trick Cancel Requested!\0", 	//eCSO_JUMP_TRICK_CANCEL_REQUESTED,	/* Seek TRICKING */
		"[Status] Jump Tricking...\0" 				//eCSO_JUMP_TRICKING,				/* Seek TRICKING */
};

static const char *s_CsoMediaEventString[CSO_EVENT_MAX+1] =
{
		"[CSO Event] UNKNOWN\0", 					//CSO_EVENT_UNKNOWN=0,	/**< Unknown Event */

		"[CSO Event] Probe Cancel!\0",				//CSO_EVENT_PROBE_CANCEL,	/**< Probe is canceled */
		"[CSO Event] Probe Complete!\0", 			//CSO_EVENT_PROBE_COMPLETE,	/**< Probe is complete */
		"[CSO Event] Probe Failure!\0", 			//CSO_EVENT_PROBE_FAILURE,	/**< Probe is failure */
		"[CSO Event] Probe Info Updated!\0",		//CSO_EVENT_PROBE_UPDATE,	/**< Probe is updated */
		"[CSO Event] Buffering Start!\0", 			//CSO_EVENT_BUFFERING_START,	/**< Buffering has been started */
		"[CSO Event] Buffering Enough!\0", 			//CSO_EVENT_BUFFERING_ENOUGH,	/**< Buffered data is enough */
		"[CSO Event] Buffering Not Enough!\0", 		//CSO_EVENT_BUFFERING_NOTENOUGH,	/**< Buffered data is NOT enough */
													//CSO_EVENT_BUFFERING_CANPLAY,
		"[CSO Event] Buffering Complete!\0", 		//CSO_EVENT_BUFFERING_COMPLETE,	/**< Buffering is complete */
		"[CSO Event] Buffering Failure!\0", 		//CSO_EVENT_BUFFERING_FAILURE,	/**< Buffering is failure */

		"[CSO Event] Resumed!\0", 					//CSO_EVENT_MEDIA_RESUME,	/**< Media playback is resume */
		"[CSO Event] Pause!\0", 					//CSO_EVENT_MEDIA_PAUSE,	/**< Media playback is paused */

		"[CSO Event] Begin Of Stream!\0", 			//CSO_EVENT_MEDIA_BEGINOFSTREAM,	/**< Media playback has been reached begin of stream */
		"[CSO Event] End Of Stream!\0", 			//CSO_EVENT_MEDIA_ENDOFSTREAM,	/**< Media playback has been reached end of stream */
		"[CSO Event] Error Of Stream!\0", 			//CSO_EVENT_MEDIA_ERROROFSTREAM,	/**< Error has occured during playback */

		"[CSO Event] Requested Seek Completed !\0",	//CSO_EVENT_MEDIA_SEEK_COMPLETE,
		"[CSO Event] Requested Trick Completed!\0",	//CSO_EVENT_MEDIA_TRICK_COMPLETE,
		"[CSO Event] Requested Trick Rate Changed!!\0",//CSO_EVENT_MEDIA_TRICK_CHANGE_RATE,

		"[CSO Event] Need More Data !\0",	//CSO_EVENT_MEDIA_NEEDMOREDATA, /**< */
		"[CSO Event] Need ReStart !\0",	//CSO_EVENT_MEDIA_NEEDRESTART, /**< */
		"[CSO Event] Selected Video track complete !\0" ,
		"[CSO Event] Selected Audio track complete !\0" ,
		"[CSO Event] Selected Subtitle track complete !\0" ,
		"[CSO Event] Need Seek !\0" ,
		"[CSO Event] Duration changed !\0" ,
		"[CSO Event] Audio flush !\0" ,
		"[CSO Event] Audio restart !\0" ,
		"[CSO Event] Base position updated! \0",
		
		"[CSO Event] Extradata updated! \0",

		"[CSO Event] Requested Seek Failed !\0",	//CSO_EVENT_ERROR_SEEK_FAIL,
		"[CSO Event] Requested Trick Failed !\0",	//CSO_EVENT_ERROR_TRICK_FAIL,
		"[CSO Event] Selected Failed !\0",	//CSO_EVENT_ERROR_TRICK_FAIL,

		"[CSO Event] ViewRightWeb !\0",				//CSO_EVENT_DRM_VIEWRIGHTWEB,
		"[CSO Event] Playready Output !\0",			//CSO_EVENT_DRM_PLAYREADY_UPDATE_OUTPUTCONTROL,
		"[CSO Event] Nagra !\0",					//CSO_EVENT_DRM_NAGRAPRM_REQUESTKEY,
/*
		"[CSO Event] Multicast tuned !\0",			//CSO_EVENT_MULTICAST_TUNED,
		"[CSO Event] Multicast Locked TS !\0",		//CSO_EVENT_MULTICAST_LOCKED_TS,
		"[CSO Event] Multicast Locked TTS !\0",		//CSO_EVENT_MULTICAST_LOCKED_TTS,
		"[CSO Event] Multicast Unlocked !\0",		//CSO_EVENT_MULTICAST_UNLOCKED,
*/
		"[CSO Event] MAX!\0" 					//CSO_EVENT_MAX,
};

#define URL_PREFIX_HBUFFER "hbuffer://cso_session/"
#define URL_PREFIX_HDLNA "hdlna://"
#define URL_PREFIX_HAIRPLAY "hairplay://"

#endif

STATIC HINT32				 	s_nRpcHandle = 0;
STATIC HxList_t *s_pRemoveSemaList = NULL;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

STATIC void om_media_play_mediapb_cso_EnterSection(omMediaPlay_Context_t *pstContext);
STATIC void om_media_play_mediapb_cso_ExitSection(omMediaPlay_Context_t *pstContext);
STATIC HUINT32 om_media_play_mediapb_cso_GetSemaphoreId(omMediaPlay_Context_t *pstContext);
STATIC HERROR om_media_play_mediapb_cso_StoreProbeInfo(omMediaPlay_Context_t *pstContext, CSO_ProbeInfo_t *cso_probeinfo);


STATIC BUS_Result_t om_media_play_mediapb_EvtPause (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void om_media_play_mediapb_CbOnBufferStateChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_BufferState_e eBufferState)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBufferStateChanged, "iiii", ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eBufferState);
}
STATIC void om_media_play_mediapb_CbOnBufferInfoChanged (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_BufferInfo_t *pstBufferInfo)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBufferInfoChanged, "iiib", ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, pstBufferInfo, sizeof(OxMediaPlay_BufferInfo_t));
}
STATIC void om_media_play_mediapb_CbOnPlayInfo (HUINT32 ulViewId, HUINT32 ulSessionId, HUINT64 ullPosition, HUINT64 ullDration, HUINT32 ulBitrate)
{
#if defined(CONFIG_TIMESTAMP_64)
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayInfo, "iiilli", ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, ullPosition, ullDration, ulBitrate);
#else
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayInfo, "iiiiii", ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, (HUINT32)ullPosition, (HUINT32)ullDration, ulBitrate);
#endif
}
STATIC void om_media_play_mediapb_CbOnPumpIndexChanged (HUINT32 ulViewId, HUINT32 ulSessionId, HUINT32 ulPumpId, OxMediaPlay_PumpType_e ePumpType)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPumpIndexChanged, "iiiii", ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, ulPumpId, ePumpType);
}

STATIC void om_media_play_mediapb_CbOnSeekReady (HUINT32 ulViewId, HUINT32 ulSessionId)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSeekReady, "iii", ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY);
}

STATIC void om_media_play_mediapb_CbOnAudioCodecSwitched(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify(s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioCodecSwitched, "iii", ulViewId, ulSessionId, eMediaType);
}

STATIC void om_media_play_mediapb_CbOnAudioFlushCompleted(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify(s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioFlushCompleted, "iii", ulViewId, ulSessionId, eMediaType);
}

STATIC void om_media_play_mediapb_CbOnAudioRestartCompleted(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify(s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioRestartCompleted, "iii", ulViewId, ulSessionId, eMediaType);
}

STATIC void om_media_play_mediapb_CbOnAudioFadeCompleted(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify(s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioFadeCompleted, "iii", ulViewId, ulSessionId, eMediaType);
}

STATIC void om_media_play_mediapb_CbOnAudioFadeFailed(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)
{
	HLIB_RPC_Notify(s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioFadeFailed, "iii", ulViewId, ulSessionId, eMediaType);
}

STATIC void om_media_play_mediapb_CbOnAudioDecoderVolumeChanged(HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 unVolume)
{
	HLIB_RPC_Notify(s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioDecoderVolumeChanged, "iiii", ulViewId, ulSessionId, eMediaType, unVolume);
}

STATIC void om_media_play_mediapb_CbOnSBTLUpdate (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_SubtitleStatus_t *pstStatus)
{
	if( NULL == pstStatus )
		return ;

	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSBTLUpdate, "iiib", ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, pstStatus,  sizeof(OxMediaPlay_SubtitleStatus_t) );
}
STATIC void om_media_play_mediapb_CbOnResourceMapChanged (OxMediaPlay_RPCResourceMap_t	*pstRPCResourceMap, HUINT32 ulSize)
{
	if( NULL == pstRPCResourceMap )
		return ;

	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResourceMapChanged, "iiiib",
		eOxMP_MEDIATYPE_MEDIAPLAY,
		pstRPCResourceMap->ulNumOfPlaypump,
		pstRPCResourceMap->ulNumOfDma,
		pstRPCResourceMap->ulAudioMixerHandle,
		pstRPCResourceMap->aResourceArray,
		ulSize );
}

STATIC void om_media_play_mediapb_CbOnSubtitleSyncTime (HUINT32 ulViewId, HINT32 nSubtitle_SyncTime)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleSyncTime, "ii", ulViewId, nSubtitle_SyncTime);
}
STATIC void om_media_play_mediapb_CbOnSubtitleFontSize (HUINT32 ulViewId, HINT32 nSubtitle_FontSize)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleFontSize, "ii", ulViewId, nSubtitle_FontSize);
}
STATIC void om_media_play_mediapb_CbOnSubtitleTextPosition (HUINT32 ulViewId, HINT32 nSubtitle_TextPosition)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleTextPosition, "ii", ulViewId, nSubtitle_TextPosition);
}
STATIC void om_media_play_mediapb_CbOnBufferingScenario (HUINT32 ulViewId)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBufferingScenario, "i", ulViewId);
}
STATIC void om_media_play_mediapb_CbOnDrmLicenseData (HUINT32 ulViewId)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnDrmLicenseData, "i", ulViewId);
}
STATIC void om_media_play_mediapb_CbOnVmxRightsErrorChanged(HUINT32 ulViewId, HUINT32 ulSessionId,HINT32 nErrorCode)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVmxDrmErrorChanged, "iii",ulViewId,ulSessionId,nErrorCode);
}

STATIC void om_media_play_mediapb_CbOnBrowserUserAgentData (HUINT32 ulViewId)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBrowserUserAgentData, "i", ulViewId);
}
STATIC void om_media_play_mediapb_CbOnDuration (HUINT32 ulViewId)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnDuration, "i", ulViewId);
}
STATIC void om_media_play_mediapb_CbOnPlayerType (HUINT32 ulViewId)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayerType, "i", ulViewId);
}

#if defined(CONFIG_MW_OUTPUT_HDAUDIO_SR_RESTRICT)
STATIC void om_media_play_mediapb_CbOnAudioOutputRestrict (HUINT32 ulViewId, HUINT32 ulSessionId)
{
	HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioOutputRestrict, "iii", ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY );
}
#endif

STATIC HERROR om_media_play_mediapb_CbProbe (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 				 ulSessionId = (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32 				 ulViewId 	 = (HUINT32)HxOBJECT_INT (apArgv[1]);
	HUINT32 				 ulActionId  = eActionId_VIEW_FIRST + ulViewId;
	HCHAR					*pszUri		 = (HCHAR *)HxOBJECT_STR (apArgv[2]);
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	//HxLOG_Error("%s(%d) [CRB] Start Probe. \n", __FUNCTION__, __LINE__);

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		hErr = om_media_play_mediapb_InitAction(pstContext);
		if (ERR_OK == hErr)
		{
			hErr = om_media_play_mediapb_Probe (ulActionId, pszUri);
			if (ERR_OK == hErr)
			{
				pstContext->stCache.eState	= eOxMP_PLAYSTATE_BUFFERING;
				pstContext->stCache.eError	= eOxMP_PLAYERROR_NO_ERROR;
			}
		}
	}

	if (ERR_OK == hErr)
	{
		om_media_play_CbOnPlayStateChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYSTATE_BUFFERING);
	}
	else
	{
		omMPMediaPlayContext_t     	*pstMediaPlay = NULL;
		HUINT32 eErrorReason  = 0;
		if(pstContext)
		{
			pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);
			if ( pstMediaPlay )
				eErrorReason = pstMediaPlay->eErrorReason;
		}
		om_media_play_CbOnPlayErrorChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYERROR_PROBE_FAILED, (OxMediaPlay_ErrorReason_e)eErrorReason);
	}

	return ERR_OK;

}

STATIC HERROR om_media_play_mediapb_CbSetSubtitleSyncTime (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	HINT32 					 nValue			= (HINT32)HxOBJECT_INT (apArgv[2]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext		= NULL;
	HERROR					 hErr 			= ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			break;//N/A
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_MEDIAPLAY:		hErr = om_media_play_mediapb_SetSubtitleSyncTime( ulActionId, nValue );
												break;

			default:
				hErr = ERR_FAIL;
				break;
		}
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
		om_media_play_mediapb_CbOnSubtitleSyncTime (ulViewId, nValue);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbSetSubtitleFontSize (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	HINT32 					 nValue			= (HINT32)HxOBJECT_INT (apArgv[2]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext		= NULL;
	HERROR					 hErr 			= ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			break;//N/A
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_MEDIAPLAY:		hErr = om_media_play_mediapb_SetSubtitleFontSize( ulActionId, nValue );
												break;

			default:
				hErr = ERR_FAIL;
				break;
		}
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
		om_media_play_mediapb_CbOnSubtitleFontSize (ulViewId, nValue);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbSetSubtitleTextPosition (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	HINT32 					 nValue			= (HINT32)HxOBJECT_INT (apArgv[2]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext		= NULL;
	HERROR					 hErr 			= ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			break;//N/A
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_MEDIAPLAY:		hErr = om_media_play_mediapb_SetSubtitleTextPosition( ulActionId, nValue );
												break;

			default:
				hErr = ERR_FAIL;
				break;
		}
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
		om_media_play_mediapb_CbOnSubtitleTextPosition (ulViewId, nValue);
	}

	(void)(ulSessionId);

	return ERR_OK;
}


STATIC HERROR om_media_play_mediapb_CbSetBufferingScenario (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	HBOOL					 bAuto 			= (HBOOL)  HxOBJECT_INT (apArgv[2]);
	HINT32 					 nThreshold 	= (HINT32) HxOBJECT_INT (apArgv[3]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		hErr = om_media_play_mediapb_SetBufferingScenario( ulActionId, bAuto, nThreshold );
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
		om_media_play_mediapb_CbOnBufferingScenario (ulViewId);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbSetDrmLicenseData (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	HCHAR 					*pszData		= (HCHAR*) HxOBJECT_STR (apArgv[2]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		hErr = om_media_play_mediapb_SetDrmLicenseData( ulActionId, pszData );
		if(ERR_FAIL== hErr)
		{
			HxLOG_Error("%s(%d) om_media_play_mediapb_SetDrmLicenseData Fail\n", __FUNCTION__, __LINE__);
		}
		else
		{
			/*EMIT CHANGED EVENT*/
			(void)om_media_play_mediapb_CbOnDrmLicenseData(ulViewId);
		}
	}

	(void)(ulSessionId);

	return hErr;
}

STATIC HERROR om_media_play_mediapb_CbOnSetPlayReadyInfo (HUINT32 ulActionId, HUINT8 *pszLaUrl, HUINT8 *pszCustomData, HBOOL bControlFlag)
{
    omMediaPlay_Context_t           *pstContext     =   om_media_play_GetContext (ulActionId);
    unsigned int uiLaUrlLen=0;
    unsigned int uiCustomDataLen=0;

    if (NULL == pstContext)                 { return ERR_FAIL; }

#ifdef CONFIG_SUPPORT_IMMA
	if(pszLaUrl != NULL)
	{
		uiLaUrlLen = VK_strlen(pszLaUrl);
	}
	if(pszCustomData != NULL)
	{
		uiCustomDataLen = VK_strlen(pszCustomData);
	}

	if((pszLaUrl == NULL || uiLaUrlLen == 0)
	&& (pszCustomData == NULL || uiCustomDataLen == 0)
	&& bControlFlag == FALSE)
	{
		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL)
		{
			OxMGR_Free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL);
			s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL = NULL;
		}
		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData)
		{
			OxMGR_Free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData);
			s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData = NULL;
		}
		s_stMediaPlayerCache[CalcActionId(ulActionId)].bControlFlag = FALSE;
		HxLOG_Debug("Initialize PlayReadyInfo\n");
		return ERR_OK;
	}

	if(pszLaUrl)
	{
		if(uiLaUrlLen > 0)
		{
			if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL != NULL)
			{
			   OxMGR_Free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL);
			   s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL = NULL;
			}

			s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL = HLIB_STD_MemAlloc(uiLaUrlLen+1);
			if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL == NULL)
			{
			   HxLOG_Error("Out of memory!\n");
			   return ERR_FAIL;
			}
			HxSTD_memset(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL, 0x0, uiLaUrlLen+1);
			HxSTD_memcpy(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL, pszLaUrl , uiLaUrlLen);
			HxLOG_Debug("LicenseURL = [%s]\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL);
		}
		else /* string = "" */
		{
			HxLOG_Debug("pszLaUrl = [%s], uiLaUrlLen = [%u]\n", pszLaUrl, uiLaUrlLen);
		}
	}

	if((pszLaUrl == NULL) || (uiLaUrlLen == 0))
	{
		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL)
		{
			OxMGR_Free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL);
			s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL = NULL;
		}
		HxLOG_Debug("LicenseURL = [%s] - LicenseURL is Clear!\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL);
	}

	if(pszCustomData)
	{
		if(uiCustomDataLen > 0)
		{
			if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData != NULL)
			{
			   OxMGR_Free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData);
			   s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData = NULL;
			}

			s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData = HLIB_STD_MemAlloc(uiCustomDataLen+1);
			if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData == NULL)
			{
			   HxLOG_Error("Out of memory!\n");
			   return ERR_FAIL;
			}
			HxSTD_memset(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData, 0x0, uiCustomDataLen+1);
			HxSTD_memcpy(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData, pszCustomData , uiCustomDataLen);
			HxLOG_Debug("CustomData = [%s]\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData);
		}
		else /* string = "" */
		{
			HxLOG_Debug("pszCustomData = [%s], uiCustomDataLen = [%u]\n", pszCustomData, uiCustomDataLen);
		}
	}

	if((pszCustomData == NULL) || (uiCustomDataLen == 0))
	{
		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData != NULL)
		{
		   OxMGR_Free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData);
		   s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData = NULL;
		}
		HxLOG_Debug("CustomData = [%s] - CustomData is Clear!\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData);
	}

	s_stMediaPlayerCache[CalcActionId(ulActionId)].bControlFlag = bControlFlag;
	HxLOG_Debug("Control Flag = [%d]\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].bControlFlag);
#endif
    return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbOnSetVmxCas (HUINT32 ulActionId, HUINT8 *pCasUrl, HUINT8 *pVendorName)
{
    omMediaPlay_Context_t           *pstContext     =   om_media_play_GetContext (ulActionId);
    unsigned int uiCasUrlLen=0;
    unsigned int uiVendorNameLen=0;

    if (NULL == pstContext)                 { return ERR_FAIL; }
    if (NULL == pCasUrl)                    { return ERR_FAIL; }
    if (NULL == pVendorName)                { return ERR_FAIL; }

#ifdef CONFIG_SUPPORT_IMMA
    uiCasUrlLen = VK_strlen(pCasUrl);
    if ( uiCasUrlLen == 0 )
    {
        HxLOG_Error ("There is Cas Url)\n");
        return ERR_FAIL;
    }
    if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl != NULL)
    {
        OxMGR_Free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl);
		s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl = NULL;
    }

    s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl = HLIB_STD_MemAlloc(uiCasUrlLen+1);
    if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl == NULL)
    {
        HxLOG_Error("Out of memory!\n");
        return ERR_FAIL;
    }
    HxSTD_memset(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl, 0x0, uiCasUrlLen+1);
    HxSTD_memcpy(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl, pCasUrl, uiCasUrlLen);

	uiVendorNameLen = VK_strlen(pVendorName);
    if ( uiVendorNameLen != 0 )
    {
		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName != NULL)
		{
			OxMGR_Free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName);
			s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName = NULL;
		}

		s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName = HLIB_STD_MemAlloc(uiVendorNameLen+1);
		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName == NULL)
		{
			HxLOG_Error("Out of memory!\n");
			return ERR_FAIL;
		}
		HxSTD_memset(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName, 0x0, uiVendorNameLen+1);
		HxSTD_memcpy(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName, pVendorName, uiVendorNameLen);
    }
	else
	{
	    HxLOG_Debug ("There is No VmxVendorName)\n");
	}
	s_stMediaPlayerCache[CalcActionId(ulActionId)].bUseVmx = TRUE;
#endif
    return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbOnUnsetVmxCas (HUINT32 ulActionId)
{
    omMediaPlay_Context_t           *pstContext     =   om_media_play_GetContext (ulActionId);

    if (NULL == pstContext)                 { return ERR_FAIL; }
#ifdef CONFIG_SUPPORT_IMMA
	s_stMediaPlayerCache[CalcActionId(ulActionId)].bUseVmx = FALSE;
#endif
    return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbSetBrowserUserAgentData (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	HCHAR 					*pszUAData		= (HCHAR*) HxOBJECT_STR (apArgv[2]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			//break;//N/A
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	//break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	//break;//N/A
			case eOxMP_MEDIATYPE_MEDIAPLAY:		hErr = om_media_play_mediapb_SetBrowserUserAgentData( ulActionId, pszUAData ); break;

			default:
				hErr = ERR_FAIL;
				break;
		}
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
		om_media_play_mediapb_CbOnBrowserUserAgentData(ulViewId);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbSetPlayReadyInfo (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
    HUINT32                  ulViewId       = (HUINT32)HxOBJECT_INT (apArgv[0]);

    HCHAR                   *pszLaUrl     = (HCHAR*) HxOBJECT_STR (apArgv[1]);
    HCHAR                   *pszCustomData      = (HCHAR*) HxOBJECT_STR (apArgv[2]);
    HBOOL                   bControlFlag      = (HBOOL)  HxOBJECT_INT (apArgv[3]);

    HUINT32                  ulActionId     = eActionId_VIEW_FIRST + ulViewId;
    omMediaPlay_Context_t    *pstContext;
    HERROR                   hErr = ERR_FAIL;

    pstContext = om_media_play_GetContext (ulActionId);
    if (NULL != pstContext)
    {
        hErr = om_media_play_mediapb_CbOnSetPlayReadyInfo ( ulActionId, pszLaUrl, pszCustomData, bControlFlag);
    }

    if (ERR_OK == hErr)
    {
        /*EMIT CHANGED EVENT*/
    }

    return ERR_OK;
}


STATIC HERROR om_media_play_mediapb_CbSetVmxCas (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
    HUINT32                  ulViewId       = (HUINT32)HxOBJECT_INT (apArgv[0]);

    HCHAR                   *pszVCASUrl     = (HCHAR*) HxOBJECT_STR (apArgv[1]);
    HCHAR                   *pszVendorName      = (HCHAR*) HxOBJECT_STR (apArgv[2]);

    HUINT32                  ulActionId     = eActionId_VIEW_FIRST + ulViewId;
    omMediaPlay_Context_t           *pstContext;
    HERROR                   hErr = ERR_FAIL;

    pstContext = om_media_play_GetContext (ulActionId);
    if (NULL != pstContext)
    {
        hErr = om_media_play_mediapb_CbOnSetVmxCas( ulActionId, pszVCASUrl, pszVendorName);
    }

    if (ERR_OK == hErr)
    {
        /*EMIT CHANGED EVENT*/
    }

    return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbUnsetVmxCas (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
    HUINT32                  ulViewId       = (HUINT32)HxOBJECT_INT (apArgv[0]);

    HUINT32                  ulActionId     = eActionId_VIEW_FIRST + ulViewId;
    omMediaPlay_Context_t           *pstContext;
    HERROR                   hErr = ERR_FAIL;

    pstContext = om_media_play_GetContext (ulActionId);
    if (NULL != pstContext)
    {
       hErr = om_media_play_mediapb_CbOnUnsetVmxCas( ulActionId );
    }

    if (ERR_OK == hErr)
    {
        /*EMIT CHANGED EVENT*/
    }

    return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbResourceMap (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR					 hErr = ERR_FAIL;


	hErr = om_media_play_mediapb_ResourceMap();

	/*
		Event Responese !!
		om_media_play_mediapb_CbOnResourceMapChanged() 는 om_media_play_mediapb_ResourceMap() 함수 내에서 호출 함.
	 */
	return hErr;
}

STATIC HERROR om_media_play_mediapb_CbSetDuration (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	HUINT64					 ullDuration	= (HUINT64)HxOBJECT_INT64 (apArgv[2]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			break;//N/A
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_MEDIAPLAY:		hErr = om_media_play_mediapb_SetDuration( ulActionId, ullDuration); break;

			default:
				hErr = ERR_FAIL;
				break;
		}
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
		om_media_play_mediapb_CbOnDuration(ulViewId);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbSetPlayerType (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);

	OxMediaPlay_PlayerType_e eMediaPlayPlayerType	= (OxMediaPlay_PlayerType_e)HxOBJECT_INT (apArgv[2]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		HxLOG_Debug("ulSessionId:%u ulViewId:%u\n", ulSessionId, ulViewId);
		hErr = om_media_play_mediapb_SetPlayerType(ulActionId, eMediaPlayPlayerType);
#if 0 /* Type unknow */
		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			break;//N/A
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_MEDIAPLAY:		hErr = om_media_play_mediapb_SetPlayerType( ulActionId, eMediaPlayPlayerType); break;

			default:
				hErr = ERR_FAIL;
				break;
		}
#endif
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
		om_media_play_mediapb_CbOnPlayerType(ulViewId);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbAudioFlush(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32 ulViewId = (HUINT32)HxOBJECT_INT (apArgv[1]);
	HUINT32 unTryToChangeAudioTrack = (HUINT32)HxOBJECT_INT (apArgv[2]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		HxLOG_Debug("ulSessionId:%u ulViewId:%u\n", ulSessionId, ulViewId);
		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			break;//N/A
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_MEDIAPLAY:
				hErr = om_media_play_mediapb_AudioFlush( ulActionId, unTryToChangeAudioTrack);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("@@@@@ [%s:%d] Failed to flush audio, hErr = %d\n", __func__, __LINE__, hErr);
				}
				break;

			default:
				hErr = ERR_FAIL;
				break;
		}
	}

	if(ERR_OK == hErr)
	{
		/*EMIT COMPLETE EVENT*/
		om_media_play_mediapb_CbOnAudioFlushCompleted(ulViewId, ulSessionId, pstContext->eMediaType);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbAudioRestart(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32					 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32					 ulViewId 		= (HUINT32)HxOBJECT_INT (apArgv[1]);
	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		HxLOG_Debug("ulSessionId:%u ulViewId:%u\n", ulSessionId, ulViewId);
		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			break;//N/A
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_MEDIAPLAY:
				hErr = om_media_play_mediapb_AudioRestart(ulActionId);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("@@@@@ [%s:%d] Failed to restart audio, hErr = %d\n", __func__, __LINE__, hErr);
				}
				break;

			default:
				hErr = ERR_FAIL;
				break;
		}
	}

	if(ERR_OK == hErr)
	{
		/*EMIT COMPLETE EVENT*/
		om_media_play_mediapb_CbOnAudioRestartCompleted(ulViewId, ulSessionId, pstContext->eMediaType);
	}

	(void)(ulSessionId);

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbAudioCodecSwitch(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32 ulViewId = (HUINT32)HxOBJECT_INT (apArgv[1]);
	HUINT32 unTryToChangeAudioTrack = (HUINT32)HxOBJECT_INT (apArgv[2]);
	HINT32 nAudioCodec = (HINT32) HxOBJECT_INT (apArgv[3]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		HxLOG_Debug("ulSessionId:%u ulViewId:%u\n", ulSessionId, ulViewId);

		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			break;//N/A
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_MEDIAPLAY:
				hErr = om_media_play_mediapb_AudioCodecSwitch(ulActionId, unTryToChangeAudioTrack, nAudioCodec);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("@@@@@ [%s:%d] Failed to switch audio codec, hErr = %d, unTryToChangeAudioTrack = %d, nAudioCodec = %d\n",
						__func__, __LINE__, hErr, unTryToChangeAudioTrack, nAudioCodec);
				}
				break;

			default:
				hErr = ERR_FAIL;
				break;
		}
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
		om_media_play_mediapb_CbOnAudioCodecSwitched(ulViewId, ulSessionId, pstContext->eMediaType);
	}

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbSetFade(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 ulSessionId	= (HUINT32)HxOBJECT_INT (apArgv[0]);
	HUINT32 ulViewId = (HUINT32)HxOBJECT_INT (apArgv[1]);
	HUINT32 unStartVolume = (HUINT32)HxOBJECT_INT (apArgv[2]);
	HUINT32 unTargetVolume = (HUINT32) HxOBJECT_INT (apArgv[3]);
	HINT32 nEase = (HINT32)HxOBJECT_INT(apArgv[4]);
	HINT32 nEaseTimeElapsed = (HINT32)HxOBJECT_INT(apArgv[5]);
	HINT32 nEaseTransitionFrames = (HINT32)HxOBJECT_INT(apArgv[6]);

	HUINT32					 ulActionId 	= eActionId_VIEW_FIRST + ulViewId;
	omMediaPlay_Context_t			*pstContext;
	HERROR					 hErr = ERR_FAIL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL != pstContext)
	{
		HxLOG_Debug("ulSessionId:%u ulViewId:%u\n", ulSessionId, ulViewId);

		switch (pstContext->eMediaType)
		{
			case eOxMP_MEDIATYPE_LIVE:			break;//N/A
			case eOxMP_MEDIATYPE_TSRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_PVRPLAYBACK:	break;//N/A
			case eOxMP_MEDIATYPE_MEDIAPLAY:
				hErr = om_media_play_mediapb_SetFade(ulActionId, unStartVolume, unTargetVolume, nEase, nEaseTimeElapsed, nEaseTransitionFrames);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("@@@@@ [%s:%d] Failed to set fade, hErr = %d\n", __func__, __LINE__, hErr);
				}
				break;

			default:
				hErr = ERR_FAIL;
				break;
		}
	}

	if (ERR_OK == hErr)
	{
		/*EMIT CHANGED EVENT*/
		om_media_play_mediapb_CbOnAudioFadeCompleted(ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY);
	}
	else
	{
		om_media_play_mediapb_CbOnAudioFadeFailed(ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY);
	}

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_CbGetVmxVersion()
{
	HERROR err = ERR_FAIL;
	omMediaPlay_Context_t *pstContext = om_media_play_GetContext (0);
	err = HLIB_RPC_Notify (s_nRpcHandle, RPC_OBAMA_CAS_VERIMATRIX_onWebVersionChanged, "s", pstContext->stCache.szVmxVersion);
	return err;
}


STATIC void om_media_play_mediapb_destroySemaphore(void *semaid) 
{
	HxSEMT_Destroy((HUINT32)semaid);
}

#define ___LOCAL_FUNCTIONS___

#ifdef CONFIG_SUPPORT_IMMA
STATIC BUS_Result_t om_media_play_mediapb_PlaySpeedChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_PumpIndexChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtConnecting (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtPlaying (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtProbeFailed (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtError (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtBeginOfStream (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtEndOfStream (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtBufferingCanplay (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtPlayInfo(omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtBufferingNotEnough (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtBufferingEnough (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtBufferingFinished (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtPlayPositionChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtPlayDurationChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HUINT64 ulDuration);
STATIC BUS_Result_t om_media_play_mediapb_EvtOutputControl (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t om_media_play_mediapb_EvtMediaPBDrmBlock (omMediaPlay_Context_t *pstContext, Handle_t hAction, OxMediaPlay_DrmRightsError_t* pstDrmRightsError);
STATIC HERROR om_media_play_mediapb_CheckUnderrunThreshold(omMediaPlay_Context_t *pstContext,HUINT64 ullPlayingTime, HUINT64 ullDecodeTime, OxMediaPlay_BufferInfo_t *pstBufferInfo, HBOOL *bUnderrun);
#ifdef UPDATE_BUFFERINFO_ON_TASK
STATIC HERROR om_media_play_mediapb_CheckUnderrunNonStop(omMediaPlay_Context_t *pstContext,HUINT64 ullPlayingTime, HUINT64 ullDecodeTime, OxMediaPlay_BufferInfo_t *pstBufferInfo, HBOOL *bUnderrun);
#else
STATIC HERROR om_media_play_mediapb_CheckUnderrunNonStop(omMediaPlay_Context_t *pstContext,HUINT64 ullPlayingTime, HUINT64 ullDecodeTime, CSO_BufferInfo_t *pstCsoBufferInfo, HBOOL *bUnderrun);
#endif
STATIC HERROR om_media_play_mediapb_ControlUnderrun(omMediaPlay_Context_t *pstContext, HBOOL bUnderrun);
STATIC HERROR om_media_play_mediapb_CreateBufferInfoTask(omMediaPlay_Context_t *pstContext);
STATIC HERROR om_media_play_mediapb_ApplyPlaySpeed(HUINT32 ulActionId, HINT32 nSpeed);
STATIC HERROR om_media_play_mediapb_cso_SeekTrickRequest(omMediaPlay_Context_t *pstContext);
STATIC HERROR om_media_play_mediapb_cso_SeekTrickCancel(omMediaPlay_Context_t *pstContext);

STATIC void om_media_play_mediapb_cso_MemFree_BufferInfo(CSO_BufferInfo_t *pInfo)
{
	if (pInfo == NULL)
		return;

	if(pInfo->tBufferChunkStatus.pullStartOffsets != NULL ) 
	{
		free(pInfo->tBufferChunkStatus.pullStartOffsets);
		pInfo->tBufferChunkStatus.pullStartOffsets = NULL;
	}
	if(pInfo->tBufferChunkStatus.pullEndOffsets != NULL ) 
	{
		free(pInfo->tBufferChunkStatus.pullEndOffsets);
		pInfo->tBufferChunkStatus.pullEndOffsets = NULL;
	}

	if(pInfo->tSeekableRange.pullSeekableStart != NULL ) 
	{
		free(pInfo->tSeekableRange.pullSeekableStart);
		pInfo->tSeekableRange.pullSeekableStart = NULL;
	}
	if(pInfo->tSeekableRange.pullSeekableEnd != NULL ) 
	{
		free(pInfo->tSeekableRange.pullSeekableEnd);
		pInfo->tSeekableRange.pullSeekableEnd = NULL;
	}
}

STATIC HERROR om_media_play_mediapb_ApplyPlaySpeed(HUINT32 ulActionId, HINT32 nSpeed)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR				 hErr;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

//	if (TRUE == pstContext->bActionTaken)
	{
		HxLOG_Debug("\033[1;92m ----- , Previous Playing Speed! = %d, %d \033[0m \n", nSpeed, pstMediaPlay->stCsoStatus.uiPlayingSpeed);

		if ( pstMediaPlay->stCsoStatus.eStatus < eCSO_PLAYING )
		{
			HxLOG_Error ("not playing status... speed = %d\n", nSpeed);
			if(nSpeed == 0)
			{
				pstMediaPlay->bPausedPlay = TRUE;
			}
			return ERR_OK;
		}

		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE ||
				s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT)
		{
			if(pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_REQUESTED)
			{
				HxLOG_Error ("Not Support in PlayerType: %d\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
				return ERR_FAIL;
			}
		}

		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_SEEK_REQUESTED )
		{
			HxLOG_Error ("now seeking status... Can't change the speed = %d\n", nSpeed);
#if !defined(CONFIG_PROD_YSR2000) || !defined(MEDIAPB_RECOVER_TRICKMODE) // recover trick speed on seek complete.
			if( /*pstMediaPlay->bUnderflowAutoMode && */ (nSpeed == 0 || nSpeed == 100))
#endif
			{
				pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = nSpeed;
				HxLOG_Error ("In AutoMode, Will keep requested pause or resume operation = %d\n", nSpeed);
			}
			return ERR_FAIL;
		}

		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_REQUESTED )
		{
			if ( nSpeed == pstMediaPlay->stCsoStatus.uiRequestPlaySpeed )
			{
				HxLOG_Error ("Speed %d is already requested! wait for TRICK_COMPLETE from CSO \n", nSpeed);
				return ERR_OK;
			}
		}

		pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = nSpeed;
		if(pstMediaPlay->bUnderflowAutoMode == TRUE)
		{
			if(pstMediaPlay->stCsoStatus.uiRequestPlaySpeed == 100)
			{
				if(pstMediaPlay->bBufferEnoughReported == TRUE)
				{
					pstMediaPlay->bBufferEnoughReported = FALSE;
				}
#ifdef MAXDOME_OLD_PDLMODE
				pstMediaPlay->bNonstopState = FALSE;
#endif
			}
			else if(pstMediaPlay->stCsoStatus.uiRequestPlaySpeed == 0)
			{
				if(pstMediaPlay->bBufferNotEnoughReported == TRUE)
				{
					pstMediaPlay->bBufferNotEnoughReported = FALSE;
				}
#ifdef MAXDOME_OLD_PDLMODE
	#if defined(CONFIG_TIMESTAMP_64)
				pstMediaPlay->ullNonstopBaseTime = pstMediaPlay->stCsoStatus.ullPlayingTime;
	#else
				pstMediaPlay->ullNonstopBaseTime = pstMediaPlay->stCsoStatus.uiPlayingTime;
	#endif
#endif
			}
		}
		/* DECODER Tricking or PLAYING... */
		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_DECODER_TRICKING || pstMediaPlay->stCsoStatus.eStatus == eCSO_PLAYING )
		{
			/* this means decoder trick...*/
			if ( (0 <= nSpeed ) && (nSpeed <= (HINT32)pstMediaPlay->ulMaxDecoderTrick) )
			{
				HINT32 nCnt = 0;
				HUINT32 pulDecodedPic = 0;
				HUINT32 pulDisplayedPic = 0;
				if(nSpeed == 0 && pstMediaPlay->bCheckDisplayPic == TRUE)
				{
					while(nCnt < 300)
					{
						SVC_CSO_GetDecodedPicture(pstMediaPlay->pMediaPlayer, &pulDecodedPic, &pulDisplayedPic);
						//HxLOG_Error ("pulDecodedPic : %lu, pulDisplayedPic : %lu\n",pulDecodedPic, pulDisplayedPic);
						if(pulDisplayedPic > 0)
						{
							pstMediaPlay->bCheckDisplayPic = FALSE;
							break;
						}
						nCnt++;
						VK_TASK_Sleep(10);
					}
				}
				else
				{
					HxLOG_Error ("It's only audio contents. So, does not check decoded picture.\n");
				}
				HxLOG_Error ("DECODER TRICKING or PLAYING ----> DECODER TRICKING \n");
//				if( pstMediaPlay->stCsoStatus.uiDecoderSpeed != nSpeed )
				{
					hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, nSpeed);
					if ( hErr != ERR_OK )
					{
						HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
						return ERR_FAIL;
					}
				}

				/* this means PAUSE */
				if ( nSpeed == 0 )
				{
					om_media_play_mediapb_EvtPause (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
					pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;

					/* Decoder Tricking or Playing ---> Pause : [parkjh4] PAUSE REPORTING TO CSO */
					CSO_CTRL_PARAM_t tParam;
					HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
					if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_PAUSE, &tParam))
					{
						HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
						return ERR_FAIL;
					}

				}
				else if ( nSpeed == 100 )
				{
					if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
					{
						om_media_play_mediapb_EvtPlaying (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
					}

					/* Decoder Tricking or Pause ---> Resume : [parkjh4] RESUME REPORTING TO CSO */
					CSO_CTRL_PARAM_t tParam;
					HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
					if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_RESUME, &tParam))
					{
						HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
						pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
						return ERR_FAIL;
					}

					pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
				}
				else
				{
					/* Pause or Decoder Tricking ---> Decoder Tricking : [parkjh4] RESUME REPORTING TO CSO */
					if(pstMediaPlay->stCsoStatus.uiDecoderSpeed == 0)
					{
						CSO_CTRL_PARAM_t tParam;
						HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
						if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_RESUME, &tParam))
						{
							HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
							pstMediaPlay->stCsoStatus.eStatus = eCSO_DECODER_TRICKING;
							return ERR_FAIL;
						}
					}
					pstMediaPlay->stCsoStatus.eStatus = eCSO_DECODER_TRICKING;
				}

				pstMediaPlay->stCsoStatus.uiPlayingSpeed = nSpeed;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = nSpeed;

				om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->stCsoStatus.uiPlayingSpeed, 0, 0);
			}
			/* this means seek trick... need trick request!!! and decoder mode setting for seek trrrrrick.*/
			else
			{
				HxLOG_Error ("DECODER TRICKING ----> JUMP TRICK Requested x%d\n",nSpeed);

				hErr = om_media_play_mediapb_cso_SeekTrickRequest(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickCancel() Failed.\n");
					return ERR_FAIL;
				}

				/* Pause or Decoder Tricking ---> Seek Tricking : [parkjh4] RESUME REPORTING TO CSO */
				if(pstMediaPlay->stCsoStatus.uiDecoderSpeed == 0)
				{
					CSO_CTRL_PARAM_t tParam;
					HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
					if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_RESUME, &tParam))
					{
						HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
						pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_REQUESTED;
						return ERR_FAIL;
					}
				}
				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_REQUESTED;
			}
		}
		else if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICKING )
		{
			/* this means decoder trick...*/
			if ( (0 <= nSpeed ) &&  (nSpeed <= pstMediaPlay->ulMaxDecoderTrick) )
			{

				HxLOG_Error ("JUMP TRICKING ----> JUMP TRICK Cancel Requested \n");

				/* cancel CSO Seek Trick! */
				hErr = om_media_play_mediapb_cso_SeekTrickCancel(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("cso_SeekTrickCancel() Failed.\n");
					return ERR_FAIL;
				}

				/* Then, change the speed! */
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, nSpeed);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
					return ERR_FAIL;
				}

				/* JumpTricking ---> Decoder Tricking or Pause : [parkjh4] PAUSE REPORTING TO CSO : Will be notify speed with 100, we need to notify also paused state */
				if(nSpeed == 0)
				{
					CSO_CTRL_PARAM_t tParam;
					HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
					if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_PAUSE, &tParam))
					{
						HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
						return ERR_FAIL;
					}
				}
				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_CANCEL_REQUESTED;
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = nSpeed;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = nSpeed;
			}
			/* this means seek trick... need trick request!!! and decoder mode setting for seek trrrrrick.*/
			else
			{
				HxLOG_Error ("JUMP TRICKING ----> JUMP TRICK Requested \n");

				hErr = om_media_play_mediapb_cso_SeekTrickRequest(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickRequest() Failed.\n");
					return ERR_FAIL;
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_REQUESTED;
			}
		}
		else if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_REQUESTED ) /* abnormal case... fast function call...*/
		{
			/* this means decoder trick...*/
			if ( (0 <= nSpeed ) &&  (nSpeed <= pstMediaPlay->ulMaxDecoderTrick) )
			{
				HxLOG_Error ("JUMP TRICK Requested ----> JUMP TRICK Cancel Requested \n");

				/* cancel CSO Seek Trick! */
				hErr = om_media_play_mediapb_cso_SeekTrickCancel(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickCancel() Failed.\n");
					return ERR_FAIL;
				}

				/* Then, change the speed! */
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, nSpeed);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
					return ERR_FAIL;
				}

				/* JumpTricking ---> JumpTrick Cancel with Decoder Tricking or Pause : [parkjh4] PAUSE REPORTING TO CSO : Will be notify speed with 100, we need to notify also paused state */
				if(nSpeed == 0)
				{
					CSO_CTRL_PARAM_t tParam;
					HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
					if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_PAUSE, &tParam))
					{
						HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
						return ERR_FAIL;
					}
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_CANCEL_REQUESTED;
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = nSpeed;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = nSpeed;
			}
			/* this means seek trick... need trick request!!! and decoder mode setting for seek trrrrrick.*/
			else
			{
				HxLOG_Error ("JUMP TRICK Requested ----> JUMP TRICK Requested \n");

				hErr = om_media_play_mediapb_cso_SeekTrickRequest(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickRequest() Failed.\n");
					return ERR_FAIL;
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_REQUESTED;
			}
		}
		else /* eCSO_JUMP_TRICK_CANCEL_REQUESTED */ /* abnormal case... fast function call...*/
		{
			/* this means decoder trick...*/
			if ( (0 <= nSpeed ) &&  (nSpeed <= pstMediaPlay->ulMaxDecoderTrick) )
			{
				HxLOG_Error ("Abnormal CASE JUMP TRICK Cancel Requested ----> JUMP TRICK Cancel Requested \n");
#if defined(CONFIG_PROD_YSR2000)
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, nSpeed);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
					return ERR_FAIL;
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_CANCEL_REQUESTED;
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = nSpeed;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = nSpeed;
#else
				return ERR_FAIL;
#endif

#if 0
				/* cancel CSO Seek Trick! */
				hErr = om_media_play_mediapb_cso_SeekTrickCancel(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickCancel() Failed.\n");
					return ERR_FAIL;
				}

				/* Then, change the speed! */
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, nSpeed);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
					return ERR_FAIL;
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_CANCEL_REQUESTED;
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = nSpeed;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = nSpeed;
#endif
			}
			/* this means seek trick... need trick request!!! and decoder mode setting for seek trrrrrick.*/
			else
			{
				HxLOG_Error ("JUMP TRICK Cancel Requested ----> JUMP TRICK Requested \n");

				hErr = om_media_play_mediapb_cso_SeekTrickRequest(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickRequest() Failed.\n");
					return ERR_FAIL;
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_REQUESTED;
			}

		}
	}
	return ERR_OK;
}
STATIC HERROR om_media_play_mediapb_ConvertBufferInfo(OxMediaPlay_BufferInfo_t *pBufferInfo, CSO_BufferInfo_t *pstCSOBufferInfo)
{
	CSO_ERR	 err = CSO_ERR_OK;
	HUINT32  ulIdx			 =	 0;

	if(pstCSOBufferInfo == NULL)
	{
		HxLOG_Error("pstCSOBufferInfo is NULL !!!\n");
		return ERR_FAIL;
	}
	if(pBufferInfo == NULL)
	{
		HxLOG_Error("pBufferInfo is NULL !!!\n");
		return ERR_FAIL;
	}

	pBufferInfo->ulBufferRemainedTime	= pstCSOBufferInfo->tBufferStatus.uiRemainBufferSec;
	pBufferInfo->ulBufferPercent		= pstCSOBufferInfo->tBufferStatus.uiBufferPercentage;

	if( pstCSOBufferInfo->tSeekableRange.uiChunks > 0)
	{
		if(pstCSOBufferInfo->tSeekableRange.uiChunks > 20)
			pstCSOBufferInfo->tSeekableRange.uiChunks = 20;

		pBufferInfo->ulSeekableChunks = pstCSOBufferInfo->tSeekableRange.uiChunks;
//		HxLOG_Debug("[%s](L:%d) Seekable chunks : %d\n", __FUNCTION__, __LINE__, pstCSOBufferInfo->tSeekableRange.uiChunks);
		for(ulIdx=0; ulIdx<pstCSOBufferInfo->tSeekableRange.uiChunks; ulIdx++)
		{
			pBufferInfo->ullSeekableStart[ulIdx] = pstCSOBufferInfo->tSeekableRange.pullSeekableStart[ulIdx];
			pBufferInfo->ullSeekableEnd[ulIdx] = pstCSOBufferInfo->tSeekableRange.pullSeekableEnd[ulIdx];
//			HxLOG_Debug("[%s](L:%d) Idx:%u Seekabe Start:%llu, Seekable End:%llu\n", __FUNCTION__, __LINE__, ulIdx, pBufferInfo->ullSeekableStart[ulIdx], pBufferInfo->ullSeekableEnd[ulIdx]);
		}
	}

	if( pstCSOBufferInfo->tBufferChunkStatus.uiChunks > 0 )
	{
		/* TODO: will need change Dynamic Chunks */
		if(pstCSOBufferInfo->tBufferChunkStatus.uiChunks > 20)
			pstCSOBufferInfo->tBufferChunkStatus.uiChunks = 20;

		pBufferInfo->ulChunks = pstCSOBufferInfo->tBufferChunkStatus.uiChunks;
//		HxLOG_Debug("[%s](L:%d) Decoded TimeStamp :%llu  \n", __FUNCTION__, __LINE__,pstCSOBufferInfo->ullLastDecodeTime);
		for(ulIdx=0; ulIdx<pstCSOBufferInfo->tBufferChunkStatus.uiChunks; ulIdx++)
		{
			pBufferInfo->ullStartOffsets[ulIdx] = pstCSOBufferInfo->tBufferChunkStatus.iStartOffsets[ulIdx];
			pBufferInfo->ullEndOffsets[ulIdx] = pstCSOBufferInfo->tBufferChunkStatus.iEndOffsets[ulIdx];
//			HxLOG_Debug("[%s](L:%d) Idx:%u Start:%u End:%u  \n", __FUNCTION__, __LINE__, ulIdx, pBufferInfo->ullStartOffsets[ulIdx], pBufferInfo->ullEndOffsets[ulIdx]);
		}
	}

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_UpdateBufferInfo(omMPMediaPlayContext_t *pstMediaPlay, CSO_BufferInfo_t *pstCSOBufferInfo)
{
	CSO_ERR	 err = CSO_ERR_OK;
	HUINT32  ulIdx			 =	 0;

	if(pstMediaPlay == NULL)
	{
		HxLOG_Error("pstMediaPlay is NULL !!!\n");
		return ERR_FAIL;
	}

	if(pstCSOBufferInfo == NULL)
	{
		HxLOG_Error("pstCSOBufferInfo is NULL !!!\n");
		return ERR_FAIL;
	}

	om_media_play_mediapb_ConvertBufferInfo(&pstMediaPlay->stBufferInfo, pstCSOBufferInfo);

	pstMediaPlay->ulDownloadRate = pstCSOBufferInfo->tBufferStatus.nArrivalBytedPerSecond;

	if( pstCSOBufferInfo->tBufferChunkStatus.uiChunks > 0 )
	{
		pstMediaPlay->ullLastDecodedTime = pstCSOBufferInfo->ullLastDecodeTime;
	}

	return ERR_OK;
}

STATIC void	*om_media_play_mediapb_BufferInfoUpdateTask(void *pParam)
{
	HUINT32	ulViewId		= 0;
	HUINT32  ulSessionId	 =	 0;
	omMPMediaPlayContext_t *pstMediaPlay	= NULL;
	OxMediaPlay_BufferInfo_t stBufferInfo;
	CSO_BufferInfo_t stCSOBufferInfo;
	CSO_ERR err = CSO_ERR_FAIL;
	omMediaPlay_Context_t *pstContext = pParam;
	HUINT32	waitForTask = 0;
	pthread_t selfTid =  0;

	if(pstContext == NULL)
	{
		return NULL;
	}
	if (NULL == pstContext->pExtension) 	{ return NULL; }

	selfTid = pthread_self();
	ulSessionId	 =	 pstContext->ulSessionId;
	ulViewId = pstContext->ulActionId - eActionId_VIEW_FIRST;

	while(TRUE)
	{
		pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
		if (NULL == pstMediaPlay)
		{
			HxLOG_Error("[%s](L:%d) pstMediaPlay is NULL\n", __FUNCTION__, __LINE__ );
			break;
		}

		HxSTD_MemSet(&stCSOBufferInfo, 0, sizeof(CSO_BufferInfo_t));
		HxSTD_MemSet(&stBufferInfo, 0, sizeof(OxMediaPlay_BufferInfo_t));

		if (selfTid != s_pCsoBufferInfoThread[CalcActionId(pstContext->ulActionId)]
				|| pstMediaPlay->stCsoStatus.eStatus <= eCSO_STOPPED)
		{
			HxLOG_Debug("[%s](L:%d) Warning - Not Update Buffering Info, Mediapb Status:%d\n",
						__FUNCTION__, __LINE__, pstMediaPlay->stCsoStatus.eStatus);
			break;
		}
#if 0
		if ( pstMediaPlay->bBufferingCompleted )
		{
			HxLOG_Debug("[%s](L:%d) Buffering completed!!!\n", __FUNCTION__, __LINE__);
			break;
		}
#endif

		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_SEEK_REQUESTED )
		{
			HxLOG_Debug("[%s](L:%d) Waiting until the seek operation has completed. \n", __FUNCTION__, __LINE__);
			VK_TASK_Sleep(100);
			continue;
		}

		err = MEDIAPB_IPC_CSO_BufferInfo(pstMediaPlay->ulCsoSessionId, &stCSOBufferInfo);
		if ( err == CSO_ERR_INVALID_SESSION)
		{
			HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_BufferInfo (Invalid Session)\n", __FUNCTION__, __LINE__);
			break;
		}
		else if ( err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_BufferInfo\n", __FUNCTION__, __LINE__);
			break;
		}

		err = om_media_play_mediapb_ConvertBufferInfo(&stBufferInfo, &stCSOBufferInfo);
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 18))
		MEDIAPB_IPC_CSO_IPCMSG_MemFree_BufferInfo(&stCSOBufferInfo);
#else
		om_media_play_mediapb_cso_MemFree_BufferInfo(&stCSOBufferInfo);
#endif
		if ( err != ERR_OK )
		{
			break;
		}

		if(pstContext->pExtension && s_stMediaPlayerCache[CalcActionId(pstContext->ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX)
		{
			(void)om_media_play_mediapb_CbOnBufferInfoChanged(ulViewId, ulSessionId, &stBufferInfo);
		}
		waitForTask = 0;
		while(waitForTask < 20)
		{
			if (selfTid != s_pCsoBufferInfoThread[CalcActionId(pstContext->ulActionId)]
					|| pstMediaPlay->stCsoStatus.eStatus <= eCSO_STOPPED)
			{

				HxLOG_Debug("[%s](L:%d) Warning - Not Update Buffering Info, Mediapb Status:%d\n",
							__FUNCTION__, __LINE__, pstMediaPlay->stCsoStatus.eStatus);
				break;
			}
			/* 500ms */
			VK_TASK_Sleep(10);
			waitForTask++;
		}

	}
	HxLOG_Debug("[%s](L:%d) Exit Task\n", __FUNCTION__, __LINE__);
	VK_TASK_Sleep(10);
	return NULL;
}

STATIC HERROR om_media_play_mediapb_CreateBufferInfoTask(omMediaPlay_Context_t *pstContext)
{
	HINT32 nRet = 0;
	pthread_attr_t stack_attr;
	size_t stack_size = (32*1024);

	if (NULL == pstContext) { return ERR_FAIL; }
//	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	nRet = pthread_attr_init(&stack_attr);
	if (nRet < 0)
	{
		return ERR_FAIL;
	}
#if	0
	if (stack_size < PTHREAD_STACK_MIN)
	{
		cso_log("Error> Invalid stack size(%d)\n", stack_size);
		CSO_PRINT_ERROR(CSO_ERR_FAIL);
	}
#endif
	if( s_pCsoBufferInfoThread[CalcActionId(pstContext->ulActionId)] > 0 )
	{
		HxLOG_Debug("[%s](L:%d) Buffering Info Task is already started\n", __FUNCTION__, __LINE__);
		return ERR_OK;
	}
	/* detached */
	(void)pthread_attr_setdetachstate(&stack_attr, PTHREAD_CREATE_DETACHED);
	(void)pthread_attr_setstacksize(&stack_attr, stack_size);
	if (pthread_create(&s_pCsoBufferInfoThread[CalcActionId(pstContext->ulActionId)], &stack_attr, om_media_play_mediapb_BufferInfoUpdateTask, (void*)pstContext) < 0)
	{
		return ERR_FAIL;
	}
	pthread_attr_destroy(&stack_attr);

	HxLOG_Debug("[%s](L:%d) Buffering Info Task Start\n", __FUNCTION__, __LINE__);
	return ERR_OK;
}

#ifdef UPDATE_BUFFERINFO_ON_TASK
STATIC HERROR om_media_play_mediapb_CheckUnderrunNonStop(omMediaPlay_Context_t *pstContext,HUINT64 ullPlayingTime, HUINT64 ullDecodeTime, OxMediaPlay_BufferInfo_t *pstBufferInfo, HBOOL *bUnderrun)
{
	HUINT32 ulIdx = 0;
	HUINT32 ulContentsBitrate = 0;
	HUINT64 ullThreshold = 0;
	HUINT64 ullDownloadedTime = 0;
	HUINT32 ulDownloadRate = 0;
	HUINT64 ullLeftTime = 0;
#ifdef MAXDOME_OLD_PDLMODE
	HUINT64 ullTempPlayingTime = 0;
#endif
	omMPMediaPlayContext_t *pstMediaPlay = NULL;

	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if(pstContext->stCache.eBufferState == eOxMP_BUFFERSTATE_Buffering_Finished)
	{
		HxLOG_Debug("[%s](L:%d) BUFFERING COMPLETE\n", __FUNCTION__, __LINE__);
		*bUnderrun = FALSE;
		pstMediaPlay->bNonstopState = TRUE;
		return ERR_OK;
	}
	ulContentsBitrate = pstMediaPlay->ulBitrate;
	ulDownloadRate = pstMediaPlay->ulDownloadRate * 8;

#ifdef MAXDOME_OLD_PDLMODE
	ullTempPlayingTime = ullPlayingTime;
	ullPlayingTime = pstMediaPlay->ullNonstopBaseTime;
#endif
	ullLeftTime = pstMediaPlay->ullDuration - ullPlayingTime;
	if ( ulDownloadRate > ulContentsBitrate )
	{
		*bUnderrun = FALSE;
//		pstMediaPlay->bNonstopState = TRUE;
		HxLOG_Debug("[%s](L:%d) ContentBitrate(%u) < DownloadRate(%u) \n", __FUNCTION__, __LINE__, ulContentsBitrate, ulDownloadRate);
	}
	else
	{
		HxLOG_Debug("[%s](L:%d) LeftTime(%llu) ContentBitrate(%u), DownloadRate(%u) \n", __FUNCTION__, __LINE__,ullLeftTime, ulContentsBitrate, ulDownloadRate);
		if( ulDownloadRate != 0 )
		{
//			ullThreshold = ullLeftTime - ( ullLeftTime * ((double)ulDownloadRate / ulContentsBitrate)); /* original rule */
//			ullThreshold = (ullLeftTime - ( ullLeftTime * ((double)ulDownloadRate / ulContentsBitrate))) * (double)1.2;
			ullThreshold = ullLeftTime * ((double)(ulContentsBitrate - ulDownloadRate) / ulDownloadRate);
		}
		else
		{
			if(pstMediaPlay->bNonstopState == FALSE)
			{
				*bUnderrun = TRUE;
			}
		}
	}

	for(ulIdx=0; ulIdx<pstBufferInfo->ulChunks; ulIdx++)
	{
		if((pstBufferInfo->ullStartOffsets[ulIdx] <= ullPlayingTime)
				&& ullPlayingTime < (pstBufferInfo->ullEndOffsets[ulIdx]))
		{
			ullDownloadedTime += (pstBufferInfo->ullEndOffsets[ulIdx] - ullPlayingTime);
		}
	}
	if( ullThreshold <= ullDownloadedTime)
	{
		pstMediaPlay->bNonstopState = TRUE;
		*bUnderrun = FALSE;
	}
	else
	{
		if(pstMediaPlay->bNonstopState == FALSE)
		{
			*bUnderrun = TRUE;
		}
	}

	HxLOG_Debug("[%s](L:%d) Underrun : %d(State %d), Threshold : %llu, DownloadedTime : %llu (Time %u) \n", __FUNCTION__, __LINE__, *bUnderrun,pstMediaPlay->bNonstopState,ullThreshold, ullDownloadedTime,VK_TIMER_GetSystemTick());

#ifdef MAXDOME_OLD_PDLMODE
	ullDownloadedTime = 0;
	ullPlayingTime = ullTempPlayingTime;
	for(ulIdx=0; ulIdx<pstBufferInfo->ulChunks; ulIdx++)
	{
		if((pstBufferInfo->ullStartOffsets[ulIdx] <= ullPlayingTime)
				&& ullPlayingTime < (pstBufferInfo->ullEndOffsets[ulIdx]))
		{
			ullDownloadedTime += (pstBufferInfo->ullEndOffsets[ulIdx] - ullPlayingTime);
		}
	}
#endif

	if(ullPlayingTime < ullDecodeTime)
	{
		if(pstMediaPlay->ullDuration <= PTS_EOF_CORRECTION || ullDecodeTime >= ((HUINT64)pstMediaPlay->ullDuration - PTS_EOF_CORRECTION /*pstMediaPlay->ulBufferPauseThreshold*//*Default:1000*/))
		{
			HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH (near EOF)\n", __FUNCTION__, __LINE__);
			pstMediaPlay->bNonstopState = TRUE;
			*bUnderrun = FALSE;
			HxLOG_Debug("[%s](L:%d) [Fin.] Underrun : %d(State %d), Threshold : %llu, DownloadedTime : %llu \n", __FUNCTION__, __LINE__, *bUnderrun,pstMediaPlay->bNonstopState,ullThreshold, ullDownloadedTime);

		}
		else if((ullDecodeTime - ullPlayingTime) <= pstMediaPlay->ulBufferPauseThreshold /*Default:1000*/
			&& ullDownloadedTime < pstMediaPlay->ulBufferReleaseThreshold
			&& pstMediaPlay->stCsoStatus.uiDecoderSpeed <= 100)
		{
			HxLOG_Debug("[%s](L:%d) BUFFER NOTENOUGH (NOT ENOUGH Decoded Range)\n", __FUNCTION__, __LINE__);
#ifdef MAXDOME_OLD_PDLMODE
			pstMediaPlay->ullNonstopBaseTime = ullPlayingTime;
#endif
			*bUnderrun = TRUE;
			pstMediaPlay->bNonstopState = FALSE;
			HxLOG_Debug("[%s](L:%d) [Low Fill.] Underrun : %d(State %d), Threshold : %llu, DownloadedTime : %llu \n", __FUNCTION__, __LINE__, *bUnderrun,pstMediaPlay->bNonstopState,ullThreshold, ullDownloadedTime);
		}
	}
	return ERR_OK;
}
#else
STATIC HERROR om_media_play_mediapb_CheckUnderrunNonStop(omMediaPlay_Context_t *pstContext,HUINT64 ullPlayingTime, HUINT64 ullDecodeTime, CSO_BufferInfo_t *pstCsoBufferInfo, HBOOL *bUnderrun)
{
	HUINT32 ulIdx = 0;
	HUINT32 ulContentsBitrate = 0;
	HUINT32 ulDownloadRate = 0;
	HUINT64 ullThreshold = 0;
	HUINT64 ullDownloadedTime = 0;
	HUINT64 ullLeftTime = 0;
#ifdef MAXDOME_OLD_PDLMODE
	HUINT64 ullTempPlayingTime = 0;
#endif
	omMPMediaPlayContext_t *pstMediaPlay = NULL;

	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if(pstContext->stCache.eBufferState == eOxMP_BUFFERSTATE_Buffering_Finished)
	{
		HxLOG_Debug("[%s](L:%d) BUFFERING COMPLETE\n", __FUNCTION__, __LINE__);
		*bUnderrun = FALSE;
		pstMediaPlay->bNonstopState = TRUE;
		return ERR_OK;
	}
	ulContentsBitrate = pstMediaPlay->ulBitrate;
	ulDownloadRate = pstCsoBufferInfo->tBufferStatus.nArrivalBytedPerSecond * 8;
#ifdef MAXDOME_OLD_PDLMODE
	ullTempPlayingTime = ullPlayingTime;
	ullPlayingTime = pstMediaPlay->ullNonstopBaseTime;
#endif
	ullLeftTime = pstMediaPlay->ullDuration - ullPlayingTime;
	if ( ulDownloadRate > ulContentsBitrate )
	{
		*bUnderrun = FALSE;
//		pstMediaPlay->bNonstopState = TRUE;
		HxLOG_Debug("[%s](L:%d) ContentBitrate(%u) < DownloadRate(%u) \n", __FUNCTION__, __LINE__, ulContentsBitrate, ulDownloadRate);
	}
	else
	{
		HxLOG_Debug("[%s](L:%d) LeftTime(%llu) ContentBitrate(%u), DownloadRate(%u) \n", __FUNCTION__, __LINE__,ullLeftTime, ulContentsBitrate, ulDownloadRate);
		if( ulDownloadRate != 0 )
		{
//			ullThreshold = ullLeftTime - ( ullLeftTime * ((double)ulDownloadRate / ulContentsBitrate)); /* original rule */
//			ullThreshold = (ullLeftTime - ( ullLeftTime * ((double)ulDownloadRate / ulContentsBitrate))) * (double)1.2;
			ullThreshold = ullLeftTime * ((double)(ulContentsBitrate - ulDownloadRate) / ulDownloadRate);
		}
		else
		{
			if(pstMediaPlay->bNonstopState == FALSE)
			{
				*bUnderrun = TRUE;
			}
		}
	}

	for(ulIdx=0; ulIdx<pstCsoBufferInfo->tBufferChunkStatus.uiChunks; ulIdx++)
	{
		if((pstCsoBufferInfo->tBufferChunkStatus.iStartOffsets[ulIdx] <= ullPlayingTime)
				&& ullPlayingTime < (pstCsoBufferInfo->tBufferChunkStatus.iEndOffsets[ulIdx]))
		{
			ullDownloadedTime += (pstCsoBufferInfo->tBufferChunkStatus.iEndOffsets[ulIdx] - ullPlayingTime);
		}
	}
	if( ullThreshold <= ullDownloadedTime)
	{
		pstMediaPlay->bNonstopState = TRUE;
		*bUnderrun = FALSE;
	}
	else
	{
		if(pstMediaPlay->bNonstopState == FALSE)
		{
			*bUnderrun = TRUE;
		}
	}

	HxLOG_Debug("[%s](L:%d) Underrun : %d(State %d), Threshold : %llu, DownloadedTime : %llu (Time %u) \n", __FUNCTION__, __LINE__, *bUnderrun,pstMediaPlay->bNonstopState,ullThreshold, ullDownloadedTime,VK_TIMER_GetSystemTick());

#ifdef MAXDOME_OLD_PDLMODE
	ullDownloadedTime = 0;
	ullPlayingTime = ullTempPlayingTime;
	for(ulIdx=0; ulIdx<pstCsoBufferInfo->tBufferChunkStatus.uiChunks; ulIdx++)
	{
		if((pstCsoBufferInfo->tBufferChunkStatus.iStartOffsets[ulIdx] <= ullPlayingTime)
				&& ullPlayingTime < (pstCsoBufferInfo->tBufferChunkStatus.iEndOffsets[ulIdx]))
		{
			ullDownloadedTime += (pstCsoBufferInfo->tBufferChunkStatus.iEndOffsets[ulIdx] - ullPlayingTime);
		}
	}
#endif

	if(ullPlayingTime < ullDecodeTime)
	{
		if(pstMediaPlay->ullDuration <= PTS_EOF_CORRECTION || ullDecodeTime >= ((HUINT64)pstMediaPlay->ullDuration - PTS_EOF_CORRECTION /*pstMediaPlay->ulBufferPauseThreshold*//*Default:1000*/))
		{
			HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH (near EOF)\n", __FUNCTION__, __LINE__);
			pstMediaPlay->bNonstopState = TRUE;
			*bUnderrun = FALSE;
			HxLOG_Debug("[%s](L:%d) [Fin.] Underrun : %d(State %d), Threshold : %llu, DownloadedTime : %llu \n", __FUNCTION__, __LINE__, *bUnderrun,pstMediaPlay->bNonstopState,ullThreshold, ullDownloadedTime);

		}
		else if((ullDecodeTime - ullPlayingTime) <= pstMediaPlay->ulBufferPauseThreshold /*Default:1000*/
			&& ullDownloadedTime < pstMediaPlay->ulBufferReleaseThreshold
			&& pstMediaPlay->stCsoStatus.uiDecoderSpeed <= 100)
		{
			HxLOG_Debug("[%s](L:%d) BUFFER NOTENOUGH (NOT ENOUGH Decoded Range)\n", __FUNCTION__, __LINE__);
#ifdef MAXDOME_OLD_PDLMODE
			pstMediaPlay->ullNonstopBaseTime = ullPlayingTime;
#endif
			*bUnderrun = TRUE;
			pstMediaPlay->bNonstopState = FALSE;
			HxLOG_Debug("[%s](L:%d) [Low Fill.] Underrun : %d(State %d), Threshold : %llu, DownloadedTime : %llu \n", __FUNCTION__, __LINE__, *bUnderrun,pstMediaPlay->bNonstopState,ullThreshold, ullDownloadedTime);
		}
	}
	return ERR_OK;
}
#endif

STATIC HERROR om_media_play_mediapb_CheckUnderrunThreshold(omMediaPlay_Context_t *pstContext,HUINT64 ullPlayingTime, HUINT64 ullDecodeTime, OxMediaPlay_BufferInfo_t *pstBufferInfo, HBOOL *bUnderrun)
{
	HUINT32 ulIdx = 0;

	omMPMediaPlayContext_t *pstMediaPlay = NULL;

	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if(ullPlayingTime < ullDecodeTime)
	{
		if(pstMediaPlay->ullDuration <= PTS_EOF_CORRECTION || ullDecodeTime >= ((HUINT64)pstMediaPlay->ullDuration - PTS_EOF_CORRECTION /*pstMediaPlay->ulBufferPauseThreshold*/ /*Default:1000*/))
		{
			HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH (near EOF)\n", __FUNCTION__, __LINE__);
			*bUnderrun = FALSE;
		}
		else if((ullDecodeTime - ullPlayingTime) <= pstMediaPlay->ulBufferPauseThreshold /*Default:1000*/
		&& pstMediaPlay->stCsoStatus.uiDecoderSpeed <= 100)
		{
			HxLOG_Debug("[%s](L:%d) BUFFER NOTENOUGH (NOT ENOUGH Decoded Range)\n", __FUNCTION__, __LINE__);
			*bUnderrun = TRUE;
			return ERR_OK;
		}
	}
	if(pstBufferInfo->ulBufferPercent == 10000)
	{
		HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH (Buffer Full)\n", __FUNCTION__, __LINE__);
		*bUnderrun = FALSE;
		return ERR_OK;
	}
	for(ulIdx=0; ulIdx<pstBufferInfo->ulChunks; ulIdx++)
	{
		if((pstBufferInfo->ullStartOffsets[ulIdx] <= ullPlayingTime)
				&& ullPlayingTime < (pstBufferInfo->ullEndOffsets[ulIdx]))
		{
			if(pstMediaPlay->ullDuration <= PTS_EOF_CORRECTION || pstBufferInfo->ullEndOffsets[ulIdx] >= ((HUINT64)pstMediaPlay->ullDuration - PTS_EOF_CORRECTION /*pstMediaPlay->ulBufferPauseThreshold*/ /*Default:1000*/))
			{
				HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH (near EOF)\n", __FUNCTION__, __LINE__);
				*bUnderrun = FALSE;
			}
			else if((pstBufferInfo->ullEndOffsets[ulIdx] - ullPlayingTime)
					<= pstMediaPlay->ulBufferPauseThreshold /*Default:1000*/)
			{
				HxLOG_Debug("[%s](L:%d) BUFFER NOTENOUGH\n", __FUNCTION__, __LINE__);
				*bUnderrun = TRUE;
			}
			else if((pstBufferInfo->ullEndOffsets[ulIdx] - ullPlayingTime)
					>= pstMediaPlay->ulBufferReleaseThreshold /*Default:5000*/)
			{
				HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH\n", __FUNCTION__, __LINE__);
				*bUnderrun = FALSE;
			}
			else
			{
				if(pstMediaPlay->stCsoStatus.bUnderrun != TRUE)
				{
					HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH\n", __FUNCTION__, __LINE__);
					*bUnderrun = FALSE;
				}
				else
				{
					HxLOG_Debug("[%s](L:%d) BUFFER NOTENOUGH\n", __FUNCTION__, __LINE__);
					*bUnderrun = TRUE;
				}
			}
		}
		else if((pstBufferInfo->ullStartOffsets[ulIdx] <= ullPlayingTime)
				&& ullPlayingTime >= (pstBufferInfo->ullEndOffsets[ulIdx]))
		{
			HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH\n", __FUNCTION__, __LINE__);
			*bUnderrun = FALSE;
		}
		else
		{
			HxLOG_Debug("[%s](L:%d) BUFFER NOTENOUGH\n", __FUNCTION__, __LINE__);
			*bUnderrun = TRUE;
		}
	}
	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_ControlUnderrun(omMediaPlay_Context_t *pstContext, HBOOL bUnderrun)
{
	omMPMediaPlayContext_t *pstMediaPlay	= NULL;
	BUS_Result_t eResult = MESSAGE_PASS;
	HERROR hErr = ERR_FAIL;
	HBOOL	bNotify = TRUE;

	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if (!pstMediaPlay->bUnderflowAutoMode)
	{
		if(bUnderrun == TRUE)
		{
			if( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICKING
			|| pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_REQUESTED
			|| pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_CANCEL_REQUESTED
			|| pstMediaPlay->stCsoStatus.eStatus == eCSO_DECODER_TRICKING )
			{
				HxLOG_Debug("[%s](L:%d) BUFFER UNDERRUN, But do not enter underrun state.(TRICKING) \n", __FUNCTION__, __LINE__);
				bUnderrun = FALSE;
				bNotify = FALSE;
			}
			else if(pstMediaPlay->stCsoStatus.ulFlushedTime != 0
			&& time(NULL) - pstMediaPlay->stCsoStatus.ulFlushedTime < 2 )
			{
				HxLOG_Debug("[%s](L:%d) BUFFER UNDERRUN, But do not enter underrun state. \n", __FUNCTION__, __LINE__);
				bUnderrun = FALSE;
				bNotify = FALSE;
			}
			else if(bUnderrun == pstMediaPlay->stCsoStatus.bUnderrun && pstMediaPlay->stCsoStatus.uiPlayingSpeed != 0 /* USER RESUMED */)
			{
				HxLOG_Debug("[%s](L:%d) BUFFER UNDERRUN ( Enough Notification Duplicated!?) \n", __FUNCTION__, __LINE__);
				bUnderrun = FALSE;
				bNotify = FALSE;
			}
			else if( bUnderrun != pstMediaPlay->stCsoStatus.bUnderrun)
			{
				/* Do not emit underrun event if decoder is paused :
					in OIPF state transition machine, there are no changes from PAUSE to CONNECTING by automatically */
				if(pstMediaPlay->stCsoStatus.uiDecoderSpeed != 0)
				{
				om_media_play_mediapb_EvtConnecting (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 0); /* 0 means Pause */
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() \n");
				}
#if defined(CONFIG_PROD_YSR2000) && defined(MEDIAPB_RECOVER_TRICKMODE) // underrun state on tricking
				pstMediaPlay->stCsoStatus.uiPrevDecoderSpeed = pstMediaPlay->stCsoStatus.uiDecoderSpeed;
					pstMediaPlay->stCsoStatus.uiPrevPlayingSpeed = pstMediaPlay->stCsoStatus.uiPlayingSpeed;
#endif
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = 0;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = 0;
				HxLOG_Error("[%s](L:%d) BUFFER UNDERRUN - CurrentPlayStatus %d\n", __FUNCTION__, __LINE__,pstMediaPlay->stCsoStatus.eStatus);
				HxLOG_Error("[%s](L:%d) BUFFER UNDERRUN - PAUSED\n", __FUNCTION__, __LINE__);
				if(!pstMediaPlay->bBufferingEvtNotify)
				{
					HxLOG_Debug("[%s](L:%d) BUFFER UNDERRUN - Will Notify\n", __FUNCTION__, __LINE__);
					eResult = om_media_play_mediapb_EvtBufferingNotEnough (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				}
//				om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			}
				else
				{
					HxLOG_Debug("[%s](L:%d) Buffer undrrrun is detected, but current decoder is paused. Do not handle anything\n");
				}
			}
		}
		else
		{
			if( bUnderrun != pstMediaPlay->stCsoStatus.bUnderrun && pstMediaPlay->stCsoStatus.uiRequestPlaySpeed != 0 /* Do not resume after user pause */)
			{
				eResult = om_media_play_mediapb_EvtPlaying (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
#if defined(CONFIG_PROD_YSR2000) && defined(MEDIAPB_RECOVER_TRICKMODE) // underrun state on tricking
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, pstMediaPlay->stCsoStatus.uiPrevDecoderSpeed); /* 100 means Play 1X */
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() \n");
				}
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = pstMediaPlay->stCsoStatus.uiPrevPlayingSpeed;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = pstMediaPlay->stCsoStatus.uiPrevDecoderSpeed;
#else
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 100); /* 100 means Play 1X */
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() \n");
				}
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = 100;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = 100;
#endif
				HxLOG_Error("[%s](L:%d) BUFFER ENOUGH - RESUME\n", __FUNCTION__, __LINE__);
				if(!pstMediaPlay->bBufferingEvtNotify)
				{
					HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH - Will Notify\n", __FUNCTION__, __LINE__);
					eResult = om_media_play_mediapb_EvtBufferingEnough (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				}
//				om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 100, 0, 0);
			}
		}
	}
	else
	{
		if( bUnderrun != pstMediaPlay->stCsoStatus.bUnderrun)
		{
			if(bUnderrun == TRUE)
			{
				HxLOG_Debug("[%s](L:%d) BUFFER UNDERRUN - Will Notify\n", __FUNCTION__, __LINE__);
				eResult = om_media_play_mediapb_EvtBufferingNotEnough (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				pstMediaPlay->bBufferNotEnoughReported = TRUE; /* TODO: Will Remove, After Buffering Patch */
			}
			else
			{
				HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH - Will Notify\n", __FUNCTION__, __LINE__);
				eResult = om_media_play_mediapb_EvtBufferingEnough (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				pstMediaPlay->bBufferEnoughReported = TRUE; /* TODO: Will Remove, After Buffering Patch */
			}
		}
		else
		{
			if(bUnderrun == TRUE && (pstMediaPlay->stCsoStatus.uiPlayingSpeed != 0 || pstMediaPlay->bBufferNotEnoughReported == TRUE))
			{
				HxLOG_Debug("[%s](L:%d) BUFFER UNDERRUN - Will Notify (Again)\n", __FUNCTION__, __LINE__);
				eResult = om_media_play_mediapb_EvtBufferingNotEnough (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			}
			else if(bUnderrun == FALSE && pstMediaPlay->stCsoStatus.uiRequestPlaySpeed == 0 && pstMediaPlay->bBufferEnoughReported == TRUE)
			{
				HxLOG_Debug("[%s](L:%d) BUFFER ENOUGH - Will Notify (Again)\n", __FUNCTION__, __LINE__);
				eResult = om_media_play_mediapb_EvtBufferingEnough (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			}
		}
	}
	pstMediaPlay->stCsoStatus.bUnderrun = bUnderrun;

#if 0
	if(pstMediaPlay->stCsoStatus.bUnderrun != bUnderrun)
	{
		pstMediaPlay->stCsoStatus.bUnderrun = bUnderrun;
		if(bNotify && pstMediaPlay->bUnderflowAutoMode)
		{
			if(bUnderrun == TRUE)
			{
				eResult = om_media_play_mediapb_EvtBufferingNotEnough (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				pstMediaPlay->bBufferEnoughReported = FALSE; /* TODO: Will Remove, After Buffering Patch */
			}
			else
			{
				eResult = om_media_play_mediapb_EvtBufferingEnough (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				pstMediaPlay->bBufferEnoughReported = TRUE; /* TODO: Will Remove, After Buffering Patch */
			}
		}
	}
#endif

	return (MESSAGE_BREAK == eResult) ? ERR_FAIL: ERR_OK;
}

STATIC HBOOL om_media_play_mediapb_cso_is_opened(const HINT8 *pUrl, HUINT32 *pSessionId)
{
	if(pUrl && pSessionId)
	{
		if(strlen(pUrl) > strlen(URL_PREFIX_HBUFFER) && strstr(pUrl,URL_PREFIX_HBUFFER))
		{
			*pSessionId = atoi((pUrl+strlen(URL_PREFIX_HBUFFER)));
			return TRUE;
		}
	}
	return FALSE;
}

STATIC HUINT8 *om_media_play_mediapb_cso_is_dlna(const HINT8 *pUrl, CSO_Settings_t *pSet)
{
	HCHAR *pszBuf = NULL, *pszPar = NULL;
	HCHAR *pIter = NULL, *pToken = NULL, *pRet;
	HINT32 i, pos = 0, strLength = 0;
	HCHAR *class[4]={"_Rate=", "_Dura=", "_Size=", "_Prot="};

	strLength = strlen(pUrl);

	pszBuf = OxMGR_Malloc(strLength + 1);
	if(pszBuf == NULL)
	{
		HxLOG_Error("Out of memory.\n");
		return NULL;
	}

	pszPar = OxMGR_Malloc(strLength + 1);
	if(pszPar == NULL)
	{
		OxMGR_Free(pszBuf);
		pszBuf = NULL;
		HxLOG_Error("Out of memory.\n");
		return NULL;
	}


	strncpy(pszPar, pUrl, strlen(pUrl));
	pIter = (HCHAR *)pUrl;

	for(i=0; i<4; i++)
	{
		HxSTD_memset(pszBuf, 0x00, strLength+1);
		HxSTD_memset(pszPar, 0x00, strLength+1);
		strncpy(pszPar, pUrl, strlen(pIter)-pos);

		pToken = strstr(pszPar, class[i]);
		if(pToken == NULL)
		{
			continue;
		}

		strncpy(pszBuf, pToken+6, strlen(pToken)-6);
		pos += strlen(pToken);

		switch(i)
		{
			case 0:
				pSet->tDlnaSetting.nByteRate = atoi(pszBuf);
				break;
			case 1:
				pSet->tDlnaSetting.unDuration = atoi(pszBuf);
				break;
			case 2:
				pSet->tDlnaSetting.nFileSize = atoll(pszBuf);
				break;
			case 3:
				pSet->pProtocolInfo = HLIB_STD_StrDup(pszBuf);
					//pSet->bSuccesiveSeekForTrick = TRUE;
				break;
		}
	}

	HxSTD_memset(pszBuf, 0x00, strLength+1);
	strncpy(pszBuf, pUrl, strlen(pUrl)-pos);

	pRet = strstr(pszBuf,URL_PREFIX_HDLNA);
	if ( pRet )
	{
		pRet = HLIB_STD_StrDup(pszBuf+8);
		HxLOG_Print("[%s:%d] This is DLNA Media. %s \n", __FUNCTION__, __LINE__, pRet);
	}
	else
	{
		pRet = NULL;
	}

	if(pszBuf != NULL)
    {
		OxMGR_Free( pszBuf );
		pszBuf = NULL;
	}
	if(pszPar != NULL)
    {
		OxMGR_Free( pszPar );
		pszPar = NULL;
	};

	return pRet;
}

STATIC HUINT8 om_media_play_mediapb_cso_hextobyte(HUINT8 * hex)
{
	HUINT8 result = 0;
	HINT32 c ;
	HUINT8 *s = hex;

	if (c=(*s-'0'),(c>=0 && c <=9)) result|=c;
	else if (c=(*s-'A'),(c>=0 && c <=5)) result|=(c+10);
	else if (c=(*s-'a'),(c>=0 && c <=5)) result|=(c+10);
	else
	{
		HxLOG_Error("Not hex!!!! \n");
		return 0;
	}
	++s;
	result = result << 4;
	if (c=(*s-'0'),(c>=0 && c <=9)) result|=c;
	else if (c=(*s-'A'),(c>=0 && c <=5)) result|=(c+10);
	else if (c=(*s-'a'),(c>=0 && c <=5)) result|=(c+10);
	else
	{
		HxLOG_Error("Not hex!!!! \n");
		return 0;
	}

	return result;
}

STATIC HUINT8 *om_media_play_mediapb_cso_is_airplay(const HINT8 *pUrl, omMPMediaPlayContext_t * mpCtx)
{
    HCHAR *pszBuf = NULL;
    HCHAR *pszPar = NULL;
    HCHAR *pToken = NULL, *pRet = NULL;
    HINT32 pos = 0, i = 0, strLength = 0;
    HCHAR *pszRetUrl = NULL;

	strLength = strlen(pUrl);

	pszBuf = OxMGR_Malloc(strLength + 1);
	if(pszBuf == NULL)
	{
		HxLOG_Error("Out of memory.\n");
		return NULL;
	}

	pszPar = OxMGR_Malloc(strLength + 1);
	if(pszPar == NULL)
	{
		OxMGR_Free(pszBuf);
		pszBuf = NULL;
		HxLOG_Error("Out of memory.\n");
		return NULL;
	}

    pRet = strstr(pUrl,URL_PREFIX_HAIRPLAY);
    if ( pRet )
    {
    	if ( strstr(pUrl,"http://") || strstr(pUrl,"https://") )
    	{
    		HxLOG_Print("[%s:%d] This is an Airplay Media. %s \n", __FUNCTION__, __LINE__, pRet);

    		HxSTD_memset(pszBuf, 0x00, strLength+1);
    		HxSTD_memset(pszPar, 0x00, strLength+1);
    		strncpy(pszPar, pUrl, strlen(pUrl));

    		pToken = strstr(pszPar, "START=");
    		if(pToken == NULL)
    		{
    			pszRetUrl = (HINT8 *)HLIB_STD_StrDup(pszBuf+11);	/* Returns original URL : Free to Caller */
				OxMGR_Free( pszBuf );
				OxMGR_Free( pszPar );
				return pszRetUrl;
    		}

    		strncpy(pszBuf, pToken+6, strlen(pToken)-6);
    		pos += strlen(pToken);

    		mpCtx->bAirplaying = TRUE;
    		mpCtx->fAirPlayingStartPercentage = atof(pszBuf);

    		HxLOG_Print("[Airplay] Start Percentage = %f\n", mpCtx->fAirPlayingStartPercentage);

    		HxSTD_memset(pszBuf, 0x00, strLength + 1);
    		strncpy(pszBuf, pUrl, strlen(pUrl)-pos-1);

    		pRet = HLIB_STD_StrDup(pszBuf+11);
    	}
    	else if ( strstr(pUrl,"raop://") )
    	{
    		mpCtx->stRaopSetting = OxMGR_Malloc(sizeof(CalypsoRaop_t));
    		if (!mpCtx->stRaopSetting)
    		{
    			HxLOG_Error("error - Raop Setting Struct alloc faield!!! !!!\n");
    			pRet = NULL;
    		}
    		else
    		{
    			HxSTD_memset(mpCtx->stRaopSetting, 0x00, sizeof(CalypsoRaop_t));
    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			/* aes key */
    			pToken = strstr(pUrl, "aeskey=");
    			if ( pToken )
    			{
    				HxLOG_Debug("AES Key =");
            		strncpy(pszPar, pToken+7, 32);
        			for ( i=0 ; i<16 ; i++)
        			{
        				mpCtx->stRaopSetting->ucAesKey[i] = om_media_play_mediapb_cso_hextobyte(&pszPar[i*2]);
        				HxLOG_Debug("%02X", mpCtx->stRaopSetting->ucAesKey[i] );
        			}
    				HxLOG_Debug("\n");
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			/* aes iv */
    			pToken = strstr(pUrl, "aesiv=");
    			if ( pToken )
    			{
    				HxLOG_Debug("AES IV =");
            		strncpy(pszPar, pToken+6, 32);
        			for ( i=0 ; i<16 ; i++)
        			{
        				mpCtx->stRaopSetting->ucAesIv[i] = om_media_play_mediapb_cso_hextobyte(&pszPar[i*2]);
        				HxLOG_Debug("%02X", (HUINT32)mpCtx->stRaopSetting->ucAesIv[i] );
        			}
    				HxLOG_Debug("\n");
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			pToken = strstr(pUrl, "dport=");
    			if ( pToken )
    			{
            		strncpy(pszPar, pToken+6, 32);
    				mpCtx->stRaopSetting->unDataPort = atoi(pszPar);
    				HxLOG_Debug("Data port = %d\n", mpCtx->stRaopSetting->unDataPort );
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			pToken = strstr(pUrl, "cport=");
    			if ( pToken )
    			{
            		strncpy(pszPar, pToken+6, 32);
    				mpCtx->stRaopSetting->unControlPort = atoi(pszPar);
    				HxLOG_Debug("Control port = %d\n", mpCtx->stRaopSetting->unControlPort );
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			pToken = strstr(pUrl, "tport=");
    			if ( pToken )
    			{
            		strncpy(pszPar, pToken+6, 32);
    				mpCtx->stRaopSetting->unTimingPort = atoi(pszPar);
    				HxLOG_Debug("Timing port = %d\n", mpCtx->stRaopSetting->unTimingPort );
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			pToken = strstr(pUrl, "acodec=");
    			if ( pToken )
    			{
            		strncpy(pszPar, pToken+7, 5);
            		if ( strncmp("alac", pszPar, 4) == 0 )
            		{
            			strncpy(mpCtx->stRaopSetting->ucAudioCodec , pszPar, 4);
            		}
            		else if ( strncmp("pcm", pszPar, 3) == 0 )
            		{
            			strncpy(mpCtx->stRaopSetting->ucAudioCodec , pszPar, 3);
            		}
            		else if ( strncmp("aac", pszPar, 3) == 0 )
            		{
            			strncpy(mpCtx->stRaopSetting->ucAudioCodec , pszPar, 3);
            		}
            		else
            		{
						snprintf(mpCtx->stRaopSetting->ucAudioCodec, 8, "Unknown"); /* fix for prevent #168884 */
            		}
    				HxLOG_Debug("Audio Codec = %s\n", mpCtx->stRaopSetting->ucAudioCodec );
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			pToken = strstr(pUrl, "fmtpstr=");
    			if ( pToken )
    			{
					snprintf(pszPar, strLength, "%s", pToken+8); /* fix for prevent #168886 */
        			strncpy(mpCtx->stRaopSetting->ucFmtStr, pszPar, sizeof(mpCtx->stRaopSetting->ucFmtStr)-1);

    				HxLOG_Debug("FMTP String = %s\n", mpCtx->stRaopSetting->ucFmtStr );
    			}
    		}

    		HxSTD_memset(pszBuf, 0x00, strLength + 1);
    		strncpy(pszBuf, pUrl, strlen(pUrl));
    		pRet = HLIB_STD_StrDup(pszBuf+11);
    	}
    	else
    	{
            HxLOG_Error("[%s:%d] This is airplay url but, Unrecognized URL !!!!! : %s\n", __FUNCTION__, __LINE__, pUrl);
            pRet = NULL;
    	}
    }
    else
    {
        pRet = NULL;
    }

    if(pszBuf != NULL)
    {
		OxMGR_Free( pszBuf );
		pszBuf = NULL;
	}
	if(pszPar != NULL)
    {
		OxMGR_Free( pszPar );
		pszPar = NULL;
	}

    return pRet;
}

#if defined(CONFIG_MW_OUTPUT_HDAUDIO_SR_RESTRICT)
STATIC HBOOL om_media_play_mediapb_is_hdAudio(const HINT8 *pUrl)
{
	HCHAR *pszExt = NULL;
	HCHAR szCaseExt[16] = { 0 };
	if ( NULL == HxSTD_StrStr( pUrl, "file://"))
		return FALSE;

	pszExt = HLIB_PATH_Extension(pUrl);
	if( pszExt == NULL )
		return FALSE;

	HxSTD_StrNCpy( szCaseExt, pszExt, sizeof(szCaseExt)-1 );
	if ( 0 == HxSTD_StrCmp( "flac", szCaseExt ) || 0 == HxSTD_StrCmp( "wav", szCaseExt ))
	{
		return TRUE;
	}
	return FALSE;
}

STATIC HERROR om_media_play_mediapb_cso_getAudioSampleRate(OxMediaPlay_AudioCompTable_t *pstAudioCompTable, DxHdmiAudioSR_e *peContentSR)
{
	HINT32 tracks = 0;
	HINT32 nSampleRate = 0;
	HERROR hError = ERR_FAIL;

	if ( NULL == pstAudioCompTable || NULL == peContentSR )
		return hError;

	for ( tracks = 0; tracks < pstAudioCompTable->nArraySize; ++tracks )
	{
		nSampleRate = pstAudioCompTable->astCompArray[tracks].ulSampleRate;
		HxLOG_Print("[OM_MP] Audio tracks[%d], sampleRate[%d]\n", tracks, nSampleRate );
	}
	nSampleRate = nSampleRate / 1000;
	HxLOG_Debug("[OM_MP] sampleRate[%d]\n", nSampleRate );
	hError = ERR_OK;

	switch ( nSampleRate )
	{
		case 32 :
			*peContentSR = eDxHDMI_AUDIO_SR_32KHz; break;
		case 44 :
			*peContentSR = eDxHDMI_AUDIO_SR_44KHz; break;
		case 48 :
			*peContentSR = eDxHDMI_AUDIO_SR_48KHz; break;
		case 88 :
			*peContentSR = eDxHDMI_AUDIO_SR_88KHz; break;
		case 96 :
			*peContentSR = eDxHDMI_AUDIO_SR_96KHz; break;
		case 176 :
			*peContentSR = eDxHDMI_AUDIO_SR_176KHz; break;
		case 192 :
			*peContentSR = eDxHDMI_AUDIO_SR_192KHz; break;
		default :
			*peContentSR = eDxHDMI_AUDIO_SR_Max;
			hError = ERR_FAIL;
			break;
	}
	return hError;
}
#endif

STATIC HUINT8 *om_media_play_mediapb_cso_is_airplay_ex(const HINT8 *pUrl, OmMedia_MediapbInfo_t *pMediaInfo, CalypsoRaop_t *pstRaopSetting)
{
    HCHAR *pszBuf = NULL;
    HCHAR *pszPar = NULL;
    HCHAR *pToken = NULL, *pRet = NULL;
    HINT32 pos = 0, i = 0, strLength = 0;
    HCHAR *pszRetUrl = NULL;

	if(pUrl == NULL || pMediaInfo == NULL || pstRaopSetting == NULL)
	{
		HxLOG_Error("Instance is NULL\n");
		return NULL;
	}
	strLength = strlen(pUrl);

	pszBuf = OxMGR_Malloc(strLength + 1);
	if(pszBuf == NULL)
	{
		HxLOG_Error("Out of memory.\n");
		return NULL;
	}

	pszPar = OxMGR_Malloc(strLength + 1);
	if(pszPar == NULL)
	{
		OxMGR_Free(pszBuf);
		pszBuf = NULL;
		HxLOG_Error("Out of memory.\n");
		return NULL;
	}

    pRet = strstr(pUrl,URL_PREFIX_HAIRPLAY);
    if ( pRet )
    {
    	if ( strstr(pUrl,"http://") || strstr(pUrl,"https://") )
    	{
    		HxLOG_Print("[%s:%d] This is an Airplay Media. %s \n", __FUNCTION__, __LINE__, pRet);

    		HxSTD_memset(pszBuf, 0x00, strLength+1);
    		HxSTD_memset(pszPar, 0x00, strLength+1);
    		strncpy(pszPar, pUrl, strlen(pUrl));

    		pToken = strstr(pszPar, "START=");
    		if(pToken == NULL)
    		{
    			pszRetUrl = (HINT8 *)HLIB_STD_StrDup(pszBuf+11);	/* Returns original URL : Free to Caller */
				OxMGR_Free( pszBuf );
				OxMGR_Free( pszPar );
				return pszRetUrl;
    		}

    		strncpy(pszBuf, pToken+6, strlen(pToken)-6);
    		pos += strlen(pToken);

    		pMediaInfo->bAirplaying = TRUE;
    		pMediaInfo->fAirPlayingStartPercentage = atof(pszBuf);

    		HxLOG_Print("[Airplay] Start Percentage = %f\n", pMediaInfo->fAirPlayingStartPercentage);

    		HxSTD_memset(pszBuf, 0x00, strLength + 1);
    		strncpy(pszBuf, pUrl, strlen(pUrl)-pos-1);

    		pRet = HLIB_STD_StrDup(pszBuf+11);
    	}
    	else if ( strstr(pUrl,"raop://") )
    	{
    		{
    			HxSTD_memset(pstRaopSetting, 0x00, sizeof(CalypsoRaop_t));
    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			/* aes key */
    			pToken = strstr(pUrl, "aeskey=");
    			if ( pToken )
    			{
    				HxLOG_Debug("AES Key =");
            		strncpy(pszPar, pToken+7, 32);
        			for ( i=0 ; i<16 ; i++)
        			{
        				pstRaopSetting->ucAesKey[i] = om_media_play_mediapb_cso_hextobyte(&pszPar[i*2]);
        				HxLOG_Debug("%02X", pstRaopSetting->ucAesKey[i] );
        			}
    				HxLOG_Debug("\n");
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			/* aes iv */
    			pToken = strstr(pUrl, "aesiv=");
    			if ( pToken )
    			{
    				HxLOG_Debug("AES IV =");
            		strncpy(pszPar, pToken+6, 32);
        			for ( i=0 ; i<16 ; i++)
        			{
        				pstRaopSetting->ucAesIv[i] = om_media_play_mediapb_cso_hextobyte(&pszPar[i*2]);
        				HxLOG_Debug("%02X", (HUINT32)pstRaopSetting->ucAesIv[i] );
        			}
    				HxLOG_Debug("\n");
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			pToken = strstr(pUrl, "dport=");
    			if ( pToken )
    			{
            		strncpy(pszPar, pToken+6, 32);
    				pstRaopSetting->unDataPort = atoi(pszPar);
    				HxLOG_Debug("Data port = %d\n", pstRaopSetting->unDataPort );
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			pToken = strstr(pUrl, "cport=");
    			if ( pToken )
    			{
            		strncpy(pszPar, pToken+6, 32);
    				pstRaopSetting->unControlPort = atoi(pszPar);
    				HxLOG_Debug("Control port = %d\n", pstRaopSetting->unControlPort );
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			pToken = strstr(pUrl, "tport=");
    			if ( pToken )
    			{
            		strncpy(pszPar, pToken+6, 32);
    				pstRaopSetting->unTimingPort = atoi(pszPar);
    				HxLOG_Debug("Timing port = %d\n", pstRaopSetting->unTimingPort );
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			pToken = strstr(pUrl, "acodec=");
    			if ( pToken )
    			{
            		strncpy(pszPar, pToken+7, 5);
            		if ( strncmp("alac", pszPar, 4) == 0 )
            		{
            			strncpy(pstRaopSetting->ucAudioCodec , pszPar, 4);
            		}
            		else if ( strncmp("pcm", pszPar, 3) == 0 )
            		{
            			strncpy(pstRaopSetting->ucAudioCodec , pszPar, 3);
            		}
            		else if ( strncmp("aac", pszPar, 3) == 0 )
            		{
            			strncpy(pstRaopSetting->ucAudioCodec , pszPar, 3);
            		}
            		else
            		{
						snprintf(pstRaopSetting->ucAudioCodec, 8, "Unknown"); /* fix for prevent #168884 */
            		}
    				HxLOG_Debug("Audio Codec = %s\n", pstRaopSetting->ucAudioCodec );
    			}

    			HxSTD_memset(pszPar, 0x00, strLength + 1);
    			pToken = strstr(pUrl, "fmtpstr=");
    			if ( pToken )
    			{
					snprintf(pszPar, strLength, "%s", pToken+8); /* fix for prevent #168886 */
        			strncpy(pstRaopSetting->ucFmtStr, pszPar, 64);

    				HxLOG_Debug("FMTP String = %s\n", pstRaopSetting->ucFmtStr );
    			}
    		}

    		HxSTD_memset(pszBuf, 0x00, strLength + 1);
    		strncpy(pszBuf, pUrl, strlen(pUrl));
    		pRet = HLIB_STD_StrDup(pszBuf+11);
    	}
    	else
    	{
            HxLOG_Error("[%s:%d] This is airplay url but, Unrecognized URL !!!!! : %s\n", __FUNCTION__, __LINE__, pUrl);
            pRet = NULL;
    	}
    }
    else
    {
        pRet = NULL;
    }

    if(pszBuf != NULL)
    {
		OxMGR_Free( pszBuf );
		pszBuf = NULL;
	}
	if(pszPar != NULL)
    {
		OxMGR_Free( pszPar );
		pszPar = NULL;
	}

    return pRet;
}




STATIC int om_media_play_mediapb_cso_doConvertVideoCodec( HUINT32 CodecId )
{
    int codec = 0;

    switch (CodecId) {
	    case CSO_VCODEC_H264:
		    codec = eDxVIDEO_CODEC_H264;
		    break;
	    case CSO_VCODEC_MPEG1:
		    codec = eDxVIDEO_CODEC_MPEG1;
		    break;
	    case CSO_VCODEC_MPEG2:
		    codec = eDxVIDEO_CODEC_MPEG2;
		    break;
	    case CSO_VCODEC_MPEG4Part2:
		    codec = eDxVIDEO_CODEC_MPEG4_PART2;
		    break;
	    case CSO_VCODEC_VC1:
		    codec = eDxVIDEO_CODEC_VC1;
		    break;
	    case CSO_VCODEC_WMV2:
		    HxLOG_Error("we do not support WMV2 codec !!!\n");
		    break;
	    case CSO_VCODEC_VC1SimpleMain:
		    codec = eDxVIDEO_CODEC_VC1_SIMPLEMAIN;
		    break;
	    case CSO_VCODEC_MSMPEGV3:
		    codec = eDxVIDEO_CODEC_DIVX311;
		    break;
	    case CSO_VCODEC_H265:
		    codec = eDxVIDEO_CODEC_H265;
		    break;

#if defined(CALYPSO_REQUIRED_VERSION)
#if CALYPSO_REQUIRED_VERSION(1,1,11,0,0)
		case CSO_VCODEC_VP8:
			codec = eDxVIDEO_CODEC_VP8;
			break;
		case CSO_VCODEC_VP9:
			codec = eDxVIDEO_CODEC_VP9;
			break;
#endif
#endif
	    default :
		    HxLOG_Error("error - invalid  codec (%d) !!!  \n", CodecId);
		    codec = eDxVIDEO_CODEC_UNKNOWN;
		    break;
    }

	if ( codec != eDxVIDEO_CODEC_UNKNOWN )
	{
    		if ( !SVC_CSO_IsVideoCodecSupport(codec) )
		{
			HxLOG_Error("error - codec (%d) is not supported by Video Decoder!!!  \n", CodecId);
			codec = eDxVIDEO_CODEC_UNKNOWN;
		}
	}

    return codec;
}

STATIC int om_media_play_mediapb_cso_doConvertAudioCodec( HUINT32 CodecId )
{
    int codec = 0;

    switch (CodecId) {
               case CSO_ACODEC_MP3:
                         codec = eDxAUDIO_CODEC_MP3;
                         break;
              case CSO_ACODEC_AAC:
                         codec = eDxAUDIO_CODEC_AAC;
                         break;
              case CSO_ACODEC_AAC_PLUS_LOAS:
                         codec = eDxAUDIO_CODEC_AAC_LOAS;
                         break;
              case CSO_ACODEC_AAC_PLUS_ADTS:
                         codec = eDxAUDIO_CODEC_AAC_PLUS_ADTS;
                         break;
              case CSO_ACODEC_AC3:
                         codec = eDxAUDIO_CODEC_DOLBY_AC3;
                         break;
              case CSO_ACODEC_AC3_PLUS:
                         codec = eDxAUDIO_CODEC_DOLBY_AC3P;
                         break;
              case CSO_ACODEC_MPEG:
                         codec = eDxAUDIO_CODEC_MPEG;
                         break;
              case CSO_ACODEC_WMA_STD:
                         codec = eDxAUDIO_CODEC_WMA_STD;
                         break;
              case CSO_ACODEC_WMA_PRO:
                         codec = eDxAUDIO_CODEC_WMA_PRO;
                         break;
              case CSO_ACODEC_FLAC:
                         codec = eDxAUDIO_CODEC_FLAC;
                         break;
              case CSO_ACODEC_PCM_S16BE:
              case CSO_ACODEC_PCM_S16LE:
              case CSO_ACODEC_PCM_S24BE:
              case CSO_ACODEC_PCM_S24LE:
              case CSO_ACODEC_PCM_S32BE:
              case CSO_ACODEC_PCM_S32LE:
              case CSO_ACODEC_PCM_DVD:
                         codec = eDxAUDIO_CODEC_PCMWAVE;
                         break;
              case CSO_ACODEC_ADPCM_MS:
			  			 codec = eDxAUDIO_CODEC_ADPCM;
						 break;
#if defined(CALYPSO_REQUIRED_VERSION)
#if CALYPSO_REQUIRED_VERSION(1,1,11,0,0)
			  case CSO_ACODEC_VORBIS:
						 codec = eDxAUDIO_CODEC_VORBIS;
						 break;
			  case CSO_ACODEC_OPUS:
						 codec = eDxAUDIO_CODEC_OPUS;
						 break;
#endif
#endif
              case CSO_ACODEC_DTS:
            	  HxLOG_Error("error - Can't Play DTS. !!!\n");
                         codec = eDxAUDIO_CODEC_UNKNOWN;
                         break;
              default :
            	  HxLOG_Error("error - invalid  codec (%d) !!!\n", CodecId);
                         codec = eDxAUDIO_CODEC_UNKNOWN;
                         break;
    }
    return codec;
}

#if defined(CONFIG_PROD_FVP4000T)
STATIC int om_media_play_mediapb_cso_doConvertSubtitleType( HUINT32 type )
{
    int subtype = eOxMP_SUBTITLETYPE_Unknown;

    switch (type) {
	case eCSO_SUBTITLE_DVB :
		subtype = eOxMP_SUBTITLETYPE_Dvb;
		break;
	case eCSO_SUBTITLE_EBU :
		subtype = eOxMP_SUBTITLETYPE_Ebu;
		break;
	case eCSO_SUBTITLE_XSUB_DXSA :
		break;
	case eCSO_SUBTITLE_XSUB_DXSB :
		break;
	case eCSO_SUBTITLE_TTML :
		break;
	case eCSO_SUBTITLE_TEXT :
		break;
	case eCSO_SUBTITLE_NONE :
	default :
		HxLOG_Error("error - invalid subtype !!!\n");
		subtype = eOxMP_SUBTITLETYPE_Unknown;
		break;
    }
    return subtype;
}
#endif

STATIC omMediaPlay_Context_t *om_media_play_mediapb_cso_getContextWithSessionId (HUINT32 ulCsoSessionId)
{
	HUINT32 			ulActionId;
	HINT32 				nLoop;
	omMediaPlay_Context_t 		*stRtnContext = NULL;

	for (nLoop = 0 ; nLoop < NUM_VIEW_ACTION; nLoop ++)
	{
		omMediaPlay_Context_t			*pstContext 	=	om_media_play_GetContext (nLoop + eActionId_VIEW_FIRST);
		omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

		if ( (pstContext != NULL) && ( NULL != pstContext->pExtension ) )
		{
			pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

			if( /*(pstContext->bActionTaken == TRUE) && */ (pstMediaPlay->ulCsoSessionId == ulCsoSessionId) )
			{
				ulActionId = nLoop + eActionId_VIEW_FIRST;
				HxLOG_Debug("[%s](L:%d) Found (%d):(%u)\n", __FUNCTION__, __LINE__, nLoop, ulActionId);
				stRtnContext = pstContext;
			}
		}
	}

	if ( stRtnContext == NULL )
	{
		HxLOG_Error("[%s](L:%d) Error there is no matched pbContext with this CSO Session %d \n", __FUNCTION__, __LINE__, ulCsoSessionId);
	}

	return stRtnContext;
}

STATIC void om_media_play_mediapb_cso_PrintCSOEvent(HUINT32 ulCsoSessionId, CSO_EVENT_e event, HINT64 param)
{
	if ( CSO_EVENT_MAX < event)
	{
		HxLOG_Error("\033[1;92m ----- CSO      : %s(%d) with SESSION ID = %d, PARAM = %lld \033[0m \n", s_CsoMediaEventString[CSO_EVENT_MAX], event, ulCsoSessionId, param);
	}
	else
	{
		HxLOG_Error("\033[1;92m ----- CSO      : %s(%d) with SESSION ID = %d, PARAM = %lld \033[0m \n", s_CsoMediaEventString[event],event, ulCsoSessionId, param);
	}
}

STATIC void om_media_play_mediapb_cso_PrintProcEvent(omMediaPlay_Context_t *pstContext, SvcCso_Event_e event)
{
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext)	{ return; }
	if (NULL == pstContext->pExtension) 	{ return; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if ((eSVC_CSO_EVENT_NONE > event) || (event > eSVC_CSO_EVENT_EIT_CHANGED) )
	{
		return;
	}

	if ( (event != eSVC_CSO_EVENT_PLAY_POSITION_CHANGED ) && (event != eSVC_CSO_EVENT_BUFFERING_UPDATED ) )
	{
		HxLOG_Error("\033[1;92m ----- MEDIAPB Status : %s \033[0m \n", s_CsoStatusString[pstMediaPlay->stCsoStatus.eStatus]);
		HxLOG_Error("\033[1;92m ----- Event SVC_CSO : %s \033[0m \n", s_CsoEventAPString[event-eSEVT_MEDIA_START]);
	}
}

STATIC void om_media_play_mediapb_cso_EnterSection(omMediaPlay_Context_t *pstContext)
{
	if (NULL == pstContext)					{ HxLOG_Error ("NULL \n"); return ; }
	if (NULL == pstContext->pExtension)
	{
		HxLOG_Error("pstContext->pExtension is NULL \n");
		return ;
	}

	{
		omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
		pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);

		HxSEMT_Get(pstMediaPlay->stCsoStatus.uiMediaCtrlSemaId);
	}

	return;
}

STATIC void om_media_play_mediapb_cso_ExitSection(omMediaPlay_Context_t *pstContext)
{
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext)	{ return; }
	if (NULL == pstContext->pExtension)
	{
		HxLOG_Error("pstContext->pExtension is NULL \n");
		return ;
	}

	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);

	HxSEMT_Release(pstMediaPlay->stCsoStatus.uiMediaCtrlSemaId);
	return;
}

STATIC HUINT32 om_media_play_mediapb_cso_GetSemaphoreId(omMediaPlay_Context_t *pstContext)
{
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext)
	{
		return 0 ;
	}
	if (NULL == pstContext->pExtension)
	{
		HxLOG_Error("pstContext->pExtension is NULL \n");
		return 0 ;
	}
	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);
	return pstMediaPlay->stCsoStatus.uiMediaCtrlSemaId;
}

STATIC SvcCso_PumpType_e om_media_play_convert_svc_pumptype(OxMediaPlay_PumpType_e ePumpType)
{
	SvcCso_PumpType_e eSvcPumpType=eSVC_CSO_PUMPTYPE_AV;

	switch(ePumpType)
	{
		case eOxMP_MEDIAPLAY_PUMPTYPE_AV:
			eSvcPumpType = eSVC_CSO_PUMPTYPE_AV;
			break;
		case eOxMP_MEDIAPLAY_PUMPTYPE_A:
			eSvcPumpType = eSVC_CSO_PUMPTYPE_A;
			break;
		case eOxMP_MEDIAPLAY_PUMPTYPE_V:
			eSvcPumpType = eSVC_CSO_PUMPTYPE_V;
			break;
		default :
			HxLOG_Error("Can't Find PumpType : %d\n", ePumpType);
	}
	return eSvcPumpType;
}

STATIC SvcCso_InputStreamType_e om_media_play_convert_svc_input_streamptype(OxMediaPlay_PlayerType_e ePlayerType)
{
	SvcCso_InputStreamType_e eSvcInputStreamType=eSVC_CSO_INPUT_STREAMTYPE_TS_MUX;

	switch(ePlayerType)
	{
		case eOxMP_MEDIAPLAY_PLAYERTYPE_HMX:
		case eOxMP_MEDIAPLAY_PLAYERTYPE_MSE:
			eSvcInputStreamType = eSVC_CSO_INPUT_STREAMTYPE_TS_MUX;
			break;
		case eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT:
			eSvcInputStreamType = eSVC_CSO_INPUT_STREAMTYPE_ES_DIRECT;
			break;
		default :
			HxLOG_Error("Can't Find PlayerType : %d\n", ePlayerType);
	}
	return eSvcInputStreamType;
}

STATIC HERROR om_media_play_mediapb_processDvbEbuSubtitle(HUINT32 ulActionId, HINT32 CompIndex)
{
	omMediaPlay_Context_t	*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable = NULL;
// 	HERROR					 hErr;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	pstSubtitleCompTable = &pstContext->stCache.stSubtitleCompTable;

	/* 인텍스(-1)를 통한 Subtitle 중지는 DVB-Subtitle 만 구현 되어 있다. */
	if (CompIndex == SUBTITLE_OFF)
	{
		MGR_MPlayer_DVB_Sbtl_Stop(pstMediaPlay->pMediaPlayer);
		//1 ToDo Ebu Subtitle Stop 만일 Ebu도 인덱스를 통해 Subtitle을 stop 한다면.
		/* smi등 외부 subtitle은 index로 subtitle 를 off 하지 않으므로 여기서 추가 진행을 하지 않도록 막음. */
		return ERR_OK;
	}

	if ( pstSubtitleCompTable->nArraySize <= 0)
	{
		return ERR_FAIL;
	}

	if ( CompIndex >= 0 && CompIndex < pstSubtitleCompTable->nArraySize)
	{
		if (pstSubtitleCompTable->astCompArray[CompIndex].eSubtitleType== eOxMP_SUBTITLETYPE_Dvb)
		{
			HxLOG_Debug("[%s:%d] DVB Subtitle Pid: %d, CPageId: %d, APageId: %d\033[0m\n", __FUNCTION__, __LINE__, (HUINT16)pstSubtitleCompTable->astCompArray[CompIndex].nPid,
			pstSubtitleCompTable->astCompArray[CompIndex].utArg.stDvbInfo.nCompositionPageId,  pstSubtitleCompTable->astCompArray[CompIndex].utArg.stDvbInfo.nAncillaryPageId);
			MGR_MPlayer_DVB_Sbtl_Start(pstMediaPlay->pMediaPlayer, (HUINT16)pstSubtitleCompTable->astCompArray[CompIndex].nPid,
			pstSubtitleCompTable->astCompArray[CompIndex].utArg.stDvbInfo.nCompositionPageId, pstSubtitleCompTable->astCompArray[CompIndex].utArg.stDvbInfo.nAncillaryPageId);
			return ERR_OK;
		}
		else if (pstSubtitleCompTable->astCompArray[CompIndex].eSubtitleType== eOxMP_SUBTITLETYPE_Ebu)
		{
			//1 ToDo Ebu 처리는 나중에 필요시 구현 한다.
			HxLOG_Error("EBU Subtitle for Media Playback is not yet implemented!\n");
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


/**
* @b Function @b Description <br>
* this function makes Probed --> Started
*
* @param[in] omMediaPlay_Context_t   : media pb context
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/

STATIC HERROR om_media_play_mediapb_cso_Start(omMediaPlay_Context_t *pstContext)
{
	CSO_ERR 		err;
	HERROR			hErr;
	CSO_Settings_t *pSettings = NULL;
	SvcCso_VideoTrack_t VideoTrack;
	SvcCso_AudioTrack_t AudioTrack;
	HUINT32						ulActionId;
	SvcCso_Resource_t	*pstTempResourceMap = NULL;
	HERROR 			hResult;
	HUINT32			ulNumOfPlaypump = 0;
	HBOOL bDisableVideoTrack = FALSE;
	HBOOL bDisableAudioTrack = FALSE;
	OxMediaPlay_AudioCompTable_t *pstAudioCompTable	= NULL;
	OxMediaPlay_VideoCompTable_t *pstVideoCompTable	= NULL;
	OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable = NULL;
	OmDownload_IpMediaInfo_t *pIpMediaInfo = NULL;
	HBOOL bNecessarySettings = FALSE;
	CSO_UnderflowMode_e eUnderflowMode = eCSO_UNDERFLOW_NONSTOP;

	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	//HxLOG_Error("%s(%d) [CRB] Start decoder... \n", __FUNCTION__, __LINE__);

	ulActionId = pstContext->ulActionId;
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
	pstSubtitleCompTable = &pstContext->stCache.stSubtitleCompTable;

	/********************
	 *	Get the available playpump index
	 ********************/
#if 1
	hResult = SVC_CSO_GetResourceMap(&pstTempResourceMap);
	if((hResult != ERR_OK) || (pstTempResourceMap == NULL))
	{
		HxLOG_Error("SVC_CSO_GetResourceMap failed!\n");
		return ERR_FAIL;
	}
	ulNumOfPlaypump = pstTempResourceMap->ulNumOfPlaypump;
	(void)SVC_CSO_ReleaseResourceMap(pstTempResourceMap);

	pstMediaPlay->pulPumpId = OxMGR_Malloc(sizeof(HUINT32)*ulNumOfPlaypump);
	pstMediaPlay->ePumpType = OxMGR_Malloc(sizeof(OxMediaPlay_PumpType_e)*ulNumOfPlaypump);

	if(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT)
	{
		pstMediaPlay->ulUsedPumpCount = 0;

		hErr = SVC_CSO_GetPlaypumpIndex(pstMediaPlay->pMediaPlayer, &pstMediaPlay->pulPumpId[pstMediaPlay->ulUsedPumpCount]);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("SVC_CSO_GetPlaypumpIndex failed!\n");
			return hErr;
		}

		pstMediaPlay->ePumpType[pstMediaPlay->ulUsedPumpCount] = eOxMP_MEDIAPLAY_PUMPTYPE_A;
		(void)SVC_CSO_SetPlaypumpType(pstMediaPlay->pMediaPlayer,
				pstMediaPlay->pulPumpId[pstMediaPlay->ulUsedPumpCount],
				om_media_play_convert_svc_pumptype(pstMediaPlay->ePumpType[pstMediaPlay->ulUsedPumpCount]));

		++pstMediaPlay->ulUsedPumpCount;

		hErr = SVC_CSO_GetPlaypumpIndex(pstMediaPlay->pMediaPlayer, &pstMediaPlay->pulPumpId[pstMediaPlay->ulUsedPumpCount]);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("SVC_CSO_GetPlaypumpIndex failed!\n");
			return hErr;
		}

		pstMediaPlay->ePumpType[pstMediaPlay->ulUsedPumpCount] = eOxMP_MEDIAPLAY_PUMPTYPE_V;
		(void)SVC_CSO_SetPlaypumpType(pstMediaPlay->pMediaPlayer,
				pstMediaPlay->pulPumpId[pstMediaPlay->ulUsedPumpCount],
				om_media_play_convert_svc_pumptype(pstMediaPlay->ePumpType[pstMediaPlay->ulUsedPumpCount]));

		++pstMediaPlay->ulUsedPumpCount;
	}
	else
	{
		hErr = SVC_CSO_GetPlaypumpIndex(pstMediaPlay->pMediaPlayer, &pstMediaPlay->ulPumpId);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("SVC_CSO_GetPlaypumpIndex failed!\n");
			return hErr;
		}

		if(!pstMediaPlay->bExtOpened)
		{
			if(NULL != pstMediaPlay->pCsoSettings)
			{
				pSettings = pstMediaPlay->pCsoSettings;
			}
		}
		else
		{
			pIpMediaInfo = OM_DOWNLOAD_GetMediaInfoByCsoId(pstMediaPlay->ulCsoSessionId);
			if(pIpMediaInfo != NULL)
			{
				if(NULL != pIpMediaInfo->pCsoSettings)
				{
					pSettings = (CSO_Settings_t *)pIpMediaInfo->pCsoSettings;
				}
			}
		}

		if(pSettings == NULL)
		{
			pSettings = HLIB_STD_MemAlloc(sizeof(CSO_Settings_t));
			HxSTD_MemSet(pSettings, 0x00, sizeof(CSO_Settings_t));
			HxLOG_Error("[%s](L:%d) It's first time to getting cso's setting value!\n", __FUNCTION__, __LINE__);
			err = MEDIAPB_IPC_CSO_GetSettings(pstMediaPlay->ulCsoSessionId, pSettings);
			if(err != CSO_ERR_OK)
			{
				HxLOG_Error("MEDIAPB_IPC_CSO_GetSettings failed!\n");
			}
			else
			{
				// store cache
				if(!pstMediaPlay->bExtOpened)
				{
					if(pstMediaPlay->pCsoSettings != NULL)
					{
						(void)MEDIAPB_IPC_CSO_IPCMSG_MemFree_Settings(*pstMediaPlay->pCsoSettings);
						OxMGR_Free(pstMediaPlay->pCsoSettings);
						pstMediaPlay->pCsoSettings = NULL;
					}
					HxLOG_Error("[%s](L:%d) so, store cso's setting value to cache in MediaPB\n", __FUNCTION__, __LINE__);
					pstMediaPlay->pCsoSettings = pSettings;
				}
				else
				{
					if(pIpMediaInfo != NULL)
					{
						if(pIpMediaInfo->pCsoSettings != NULL)
						{
							HxLOG_Error("[%s:%d] pIpMediaInfo->pCsoSettings != NULL\n", __FUNCTION__, __LINE__);
							(void)MEDIAPB_IPC_CSO_IPCMSG_MemFree_Settings(*((CSO_Settings_t *)pIpMediaInfo->pCsoSettings));
							OxMGR_Free(pIpMediaInfo->pCsoSettings);
							pIpMediaInfo->pCsoSettings = NULL;
						}
						HxLOG_Error("[%s](L:%d) so, store cso's setting value to cache in Download\n", __FUNCTION__, __LINE__);
						pIpMediaInfo->pCsoSettings = (void *)pSettings;
					}
					else
					{
						HxLOG_Error("[%s](L:%d) Can't Store cache\n", __FUNCTION__, __LINE__);
					}
				}
			}
		}
		else
		{
			HxLOG_Error("[%s](L:%d) Using cso'setting value from cache\n", __FUNCTION__, __LINE__);
		}
		if(pSettings != NULL && pSettings->bBypassing)
		{
			SVC_CSO_EnableDemuxPath(pstMediaPlay->pMediaPlayer, TRUE);
		}
		else
		{
			SVC_CSO_EnableDemuxPath(pstMediaPlay->pMediaPlayer, FALSE);
		}
		hErr = SVC_CSO_SetCurrentPlaypump(pstMediaPlay->pMediaPlayer, pstMediaPlay->ulPumpId);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("SVC_CSO_SetCurrentPlaypump failed!\n");
			return hErr;
		}
	}
#else
	hErr = SVC_CSO_GetPlaypumpIndex(pstMediaPlay->pMediaPlayer, &pstMediaPlay->ulPumpId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("SVC_CSO_GetPlaypumpIndex failed!\n");
		return hErr;
	}
#endif
	if(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX)
	{
		if (pSettings != NULL && pstMediaPlay->nUnderflowThreshold > 0)
		{
			pSettings->tPlaybackStartParams.ulStartThreshold = pstMediaPlay->nUnderflowThreshold;
			bNecessarySettings = TRUE;
		}

		if ( pstMediaPlay->bAirplaying )
		{
			pstMediaPlay->ullStartTime = ( pstMediaPlay->fAirPlayingStartPercentage * pstMediaPlay->ullDuration ) / 100;
			HxLOG_Error("Airplay position %lld, duration %lld\n", pstMediaPlay->ullStartTime, pstMediaPlay->ullDuration);
		}

		if(pSettings != NULL && pstMediaPlay->ullStartTime > 0)
		{
			pSettings->ullStartTime = pstMediaPlay->ullStartTime;
			bNecessarySettings = TRUE;
		}

		if(pstMediaPlay->bUnderflowAutoMode == TRUE && pstMediaPlay->stCsoConfig.bProgressiveDownload)
		{
			if(pstMediaPlay->bDlna == TRUE || pstMediaPlay->bAdaptiveStreaming == TRUE)
			{
				pstMediaPlay->eUnderflowMode = eOM_CSO_UNDERFLOW_NONE;
				HxLOG_Debug("No Underrun Control\n");
			}
			else
			{
				pstMediaPlay->eUnderflowMode = eOM_CSO_UNDERFLOW_NONSTOP;
				pstMediaPlay->ulBufferPauseThreshold = 1000; /*Default*/
				pstMediaPlay->ulBufferReleaseThreshold = 5000; /*Default*/
#ifdef MAXDOME_OLD_PDLMODE
				pstMediaPlay->bNonstopState = FALSE;
#endif
				HxLOG_Debug("NONSTOP MODE with Auto Control by WebAppl(AutoMode) \n");
			}

		}
		else
		{
			if(pstMediaPlay->bDlna == TRUE || pstMediaPlay->bAdaptiveStreaming == TRUE)
			{
				pstMediaPlay->eUnderflowMode = eOM_CSO_UNDERFLOW_NONE;
				HxLOG_Debug("No Underrun Control\n");
			}
			else
			{
				pstMediaPlay->eUnderflowMode = eOM_CSO_UNDERFLOW_THRESHOLD;
			}
#if 0
			if(!pstMediaPlay->bUnderflowAutoMode )
			{
				pstMediaPlay->ulBufferPauseThreshold = 1000; /*Default*/
				pstMediaPlay->ulBufferReleaseThreshold = 5000; /*Default*/
			}
			else
			{
				pstMediaPlay->ulBufferPauseThreshold = 1000; /*Default*/
				pstMediaPlay->ulBufferReleaseThreshold = 1500; /*Default*/
			}
#else
			pstMediaPlay->ulBufferPauseThreshold = 1000; /*Default*/
			pstMediaPlay->ulBufferReleaseThreshold = 5000; /*Default*/
#endif
			/* Maxdome & Zapper 인 경우, Underrun 상태 시, Evt 를 올리지 않도록 함. TRUE - Not notity */
			if(pstMediaPlay->bUnderflowAutoMode == TRUE)
			{
				pstMediaPlay->bBufferingEvtNotify = TRUE;
			}
			pstMediaPlay->bUnderflowAutoMode = FALSE;
			HxLOG_Debug("THRESHOLD MODE with Auto Control by Humax (Non-AutoMode) PauseThreshold(%u),ReleaseThreshold(%u) \n",pstMediaPlay->ulBufferPauseThreshold,pstMediaPlay->ulBufferReleaseThreshold);
		}

		if (pstMediaPlay->bUnderflowAutoMode )
		{
			eUnderflowMode = eCSO_UNDERFLOW_NONSTOP;
		}
		else
		{
			eUnderflowMode = eCSO_UNDERFLOW_THRESHOLD;
		}	
	
		if(pSettings != NULL && pSettings->eUnderflowMode != eUnderflowMode)
		{
			pSettings->eUnderflowMode = eUnderflowMode;
			bNecessarySettings = TRUE;
		}

		if(pSettings != NULL && bNecessarySettings == TRUE)
		{
			err = MEDIAPB_IPC_CSO_SetSettings(pstMediaPlay->ulCsoSessionId, pSettings);
			if(err != CSO_ERR_OK)
			{
				HxLOG_Error("MEDIAPB_IPC_CSO_SetSettings failed!\n");
			}
		}
		else
			HxLOG_Error("[%s](L:%d) Don't Set Settings \n", __FUNCTION__, __LINE__);
	} /* eOxMP_MEDIAPLAY_PLAYERTYPE_HMX */
	if(pstMediaPlay->ullDuration == 0x4189374BC6A7)
	{
		(void)SVC_CSO_SetDuration(pstMediaPlay->pMediaPlayer, 0);
	}
	else
	{
		(void)SVC_CSO_SetDuration(pstMediaPlay->pMediaPlayer, pstMediaPlay->ullDuration);
	}
	/* Airplay : it always play MP4, and it can be played with none zero firstPTS. so don't need to set FirstPTS to SVC_CSO */
	if ( !pstMediaPlay->bAirplaying )
	{
		(void)SVC_CSO_SetFirstPosition(pstMediaPlay->pMediaPlayer, pstMediaPlay->ullFirstPts);
	}

	if(s_stMediaPlayerCache[CalcActionId(ulActionId)].bVideoTrackSelected)
	{
		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].nVideoTrack >= 0)
		{
			pstVideoCompTable =	&pstContext->stCache.stVideoCompTable;
			if( pstVideoCompTable->nArraySize > s_stMediaPlayerCache[CalcActionId(ulActionId)].nVideoTrack)
			{
				pstMediaPlay->ulActivatedVideoCodecId = pstVideoCompTable->astCompArray[s_stMediaPlayerCache[CalcActionId(ulActionId)].nVideoTrack].eCodec;
				pstMediaPlay->ulActivatedVideoStreamId = pstVideoCompTable->astCompArray[s_stMediaPlayerCache[CalcActionId(ulActionId)].nVideoTrack].nPid;
			}
		}
		else
			bDisableVideoTrack = TRUE;
	}

	if(!bDisableVideoTrack)
	{
		VideoTrack.eVideoCodec = pstMediaPlay->ulActivatedVideoCodecId;
		VideoTrack.ulVideoStreamId = pstMediaPlay->ulActivatedVideoStreamId;

		hErr = SVC_CSO_SetActiveVideoTrack(pstMediaPlay->pMediaPlayer, &VideoTrack);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("SVC_CSO_SetActiveVideoTrack failed!\n");
		}
	}
	else
		HxLOG_Debug("Video Track Disabled.\n");

	if(s_stMediaPlayerCache[CalcActionId(ulActionId)].bAudioTrackSelected)
	{
		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].nAudioTrack >= 0)
		{
			pstAudioCompTable = &pstContext->stCache.stAudioCompTable;
			if( pstAudioCompTable->nArraySize > s_stMediaPlayerCache[CalcActionId(ulActionId)].nAudioTrack)
			{
				pstMediaPlay->ulActivatedAudioCodecId = pstAudioCompTable->astCompArray[s_stMediaPlayerCache[CalcActionId(ulActionId)].nAudioTrack].eCodec;
				pstMediaPlay->ulActivatedAudioStreamId = pstAudioCompTable->astCompArray[s_stMediaPlayerCache[CalcActionId(ulActionId)].nAudioTrack].nPid;
			}
		}
		else
			bDisableAudioTrack = TRUE;
	}

	if(!bDisableAudioTrack)
	{
		AudioTrack.eAudioCodec = pstMediaPlay->ulActivatedAudioCodecId;
		AudioTrack.ulAudioStreamId = pstMediaPlay->ulActivatedAudioStreamId;

		HxLOG_Error("\n\n\tJWJI AudioTrack.ulAudioStreamId : %u\n\n\n", AudioTrack.ulAudioStreamId);

		hErr = SVC_CSO_SetActiveAudioTrack(pstMediaPlay->pMediaPlayer, &AudioTrack);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("SVC_CSO_SetActiveAudioTrack failed!\n");
		}
	}
	else
		HxLOG_Debug("Audio Track Disabled.\n");

	/* Fixed subtitle issue for MSE playback in switching between netflix and mediaCenter file playback */
	/* Reference http://svn:3000/issues/110709 */
	if(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE)
	{
		/* MSE type streaming doesn't use file base subtitle */
		hErr = MGR_MPlayer_SetSubtitleUsage(pstMediaPlay->pMediaPlayer, FALSE);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("Error> MGR_MPlayer_SetSubtitleUsage failed!\n");
		}
	}
	else
	{
		hErr = MGR_MPlayer_SetSubtitleUsage(pstMediaPlay->pMediaPlayer, TRUE);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("Error> MGR_MPlayer_SetSubtitleUsage failed!\n");
		}
	}

#ifdef CONFIG_SUPPORT_ROVI
	hErr = MGR_MPlayer_SI_Start(pstMediaPlay->pMediaPlayer);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error ("MGR_MPlayer_SI_Start() Failed.\n");
	}
#endif


#if defined(CONFIG_TIMESTAMP_64)
	hErr = MGR_MPlayer_Play (pstMediaPlay->pMediaPlayer, (HUINT32)pstContext->stCache.stMediaPlay.ullStartTime);
#else
	hErr = MGR_MPlayer_Play(pstMediaPlay->pMediaPlayer, pstContext->stCache.stMediaPlay.ulStartTime);
#endif
	if ( hErr != ERR_OK )
	{
		HxLOG_Error ("MGR_MPlayer_Play() with (%s) url Failed.\n", pstContext->stCache.stMediaPlay.szUrl);
		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT)
		{
			HUINT32 ulIdx=0;
			for(ulIdx = 0; ulIdx<pstMediaPlay->ulUsedPumpCount; ++ulIdx)
			{
				hErr = SVC_CSO_FreePlaypumpIndex(pstMediaPlay->pMediaPlayer, pstMediaPlay->pulPumpId[ulIdx]);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("SVC_CSO_FreePlaypumpIndex Index[%u] failed!\n", ulIdx);
				}
			}
		}
		else
		{
			hErr = SVC_CSO_FreePlaypumpIndex(pstMediaPlay->pMediaPlayer, pstMediaPlay->ulPumpId);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("SVC_CSO_FreePlaypumpIndex failed!\n");
			}
		}
		return ERR_FAIL;
	}
#ifdef CONFIG_DEBUG
	if(!pstMediaPlay->bStartRequested)
#endif
	{
		MGR_MPlayer_DVB_Sbtl_Stop(pstMediaPlay->pMediaPlayer);
		if (pstSubtitleCompTable->nArraySize > 0 )
		{
			/* Subtitle index가 있을 때만 start */
			if(pstSubtitleCompTable->nCurrIndex >=0)
			{
				HINT32 nIndex = pstSubtitleCompTable->nCurrIndex;
				MGR_MPlayer_DVB_Sbtl_Start(pstMediaPlay->pMediaPlayer, (HUINT16)pstSubtitleCompTable->astCompArray[nIndex].nPid,
						pstSubtitleCompTable->astCompArray[nIndex].utArg.stDvbInfo.nCompositionPageId, pstSubtitleCompTable->astCompArray[nIndex].utArg.stDvbInfo.nAncillaryPageId);
			}
		}
	}
	if(s_stMediaPlayerCache[CalcActionId(ulActionId)].bControlFlag == TRUE)
	{
		SVC_OUT_SetMacrovision(pstContext->ulActionId, eMACROVISION_4LINE);
	}
	if( pstMediaPlay->bPausedPlay == TRUE )
	{
		hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 0);
		if ( hErr != ERR_OK )
		{
			HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
			om_media_play_mediapb_cso_ExitSection(pstContext);
			return ERR_FAIL;
		}

		/* this means PAUSE */
		{
			om_media_play_mediapb_EvtPause (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		}
	}
	if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX)
	{
		//HxLOG_Error("%s(%d) [CRB] Start CSO_START \n", __FUNCTION__, __LINE__);
		err = MEDIAPB_IPC_CSO_Start(pstMediaPlay->ulCsoSessionId, pstMediaPlay->ulPumpId);
		if ( err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Start\n", __FUNCTION__, __LINE__);
			return ERR_FAIL;
		}
		pstMediaPlay->stCsoStatus.bUnderrun = FALSE;
		pstMediaPlay->stCsoStatus.uiPlayingSpeed = 100;
		pstMediaPlay->stCsoStatus.uiDecoderSpeed = 100;
		pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = 100;
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
		om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->stCsoStatus.uiPlayingSpeed, 0, 0);
	}
	else if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE)
	{
		hErr = SVC_CSO_CreateAudioFadeTask(pstMediaPlay->pMediaPlayer);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to create AudioFade Task, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
		}

		pstMediaPlay->stCsoStatus.bUnderrun = FALSE;
		pstMediaPlay->stCsoStatus.uiPlayingSpeed = 100;
		pstMediaPlay->stCsoStatus.uiDecoderSpeed = 100;
		pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = 100;
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
		om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->stCsoStatus.uiPlayingSpeed, 0, 0);
		/* Emit PumpIndex to MSE Player */
		om_media_play_mediapb_PumpIndexChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->ulPumpId, 0, 0);
	}
	else if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT)
	{
		HINT32 nIdx = 0;
		pstMediaPlay->stCsoStatus.bUnderrun = FALSE;
		pstMediaPlay->stCsoStatus.uiPlayingSpeed = 100;
		pstMediaPlay->stCsoStatus.uiDecoderSpeed = 100;
		pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = 100;
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
		om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->stCsoStatus.uiPlayingSpeed, 0, 0);
		/* Emit PumpIndex to ES Player */
		for(nIdx=0; nIdx<pstMediaPlay->ulUsedPumpCount; ++nIdx)
		{
			om_media_play_mediapb_PumpIndexChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->pulPumpId[nIdx], pstMediaPlay->ePumpType[nIdx], 0);
		}
	}
	if( pstMediaPlay->bPausedPlay == TRUE )
	{
		pstMediaPlay->stCsoStatus.uiPlayingSpeed = 0;
		pstMediaPlay->stCsoStatus.uiDecoderSpeed = 0;
		pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = 0;
		pstMediaPlay->bPausedPlay = FALSE;
	}
	if( pstMediaPlay->nStartPlaySpeed != 0)
	{
		om_media_play_mediapb_ApplyPlaySpeed(ulActionId, pstMediaPlay->nStartPlaySpeed);
		pstMediaPlay->nStartPlaySpeed = 0;
	}
	pstMediaPlay->bBufferEnoughReported = TRUE;
#ifdef UPDATE_BUFFERINFO_ON_TASK
	if ( pstMediaPlay->bExtOpened)
	{
		OmDownload_Instance_t *pDownInst = OM_DOWNLOAD_GetInstanceWithCsoId(pstMediaPlay->ulCsoSessionId);
		if ( pDownInst && pDownInst->unExtInfo.stIpMedia.pSession)
		{
			CsoSessionDNLD_t *pDownSession  = (CsoSessionDNLD_t *)pDownInst->unExtInfo.stIpMedia.pSession;
#if 0
			pstMediaPlay->bBufferingCompleted = pDownSession->bDownloadCompleted;
			if (pDownSession->bDownloadCompleted)
			{
				om_media_play_mediapb_EvtBufferingFinished (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			}
#endif
		}
	}
	(void)om_media_play_mediapb_CreateBufferInfoTask( pstContext );
#endif
	return ERR_OK;
}

/**
* @b Function @b Description <br>
* this function makes Started --> Stopped
*
* @param[in] omMediaPlay_Context_t   : media pb context
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/
STATIC HERROR om_media_play_mediapb_cso_Stop(omMediaPlay_Context_t *pstContext)
{
	CSO_ERR 		err;
	HERROR			hErr;
	HUINT32			ulActionId;
#ifdef UPDATE_BUFFERINFO_ON_TASK
	HUINT32			waitForTask = 0;
#endif
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	ulActionId = pstContext->ulActionId;
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_STOPPED )
	{
		HxLOG_Error ("CSO Already Stopped!!!!!!!!!!!!\n");
		return ERR_OK;
	}

	if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX)
	{
		if(!pstMediaPlay->bExtOpened)
		{
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 8))
		err = MEDIAPB_IPC_CSO_Stop(pstMediaPlay->ulCsoSessionId, -1);
#else
		err = MEDIAPB_IPC_CSO_Stop(pstMediaPlay->ulCsoSessionId, -1);
#endif
		}
		else
		{
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 8))
	#if defined(CONFIG_TIMESTAMP_64)
			err = MEDIAPB_IPC_CSO_Stop(pstMediaPlay->ulCsoSessionId, pstMediaPlay->stCsoStatus.ullPlayingTime);
	#else
			err = MEDIAPB_IPC_CSO_Stop(pstMediaPlay->ulCsoSessionId, pstMediaPlay->stCsoStatus.uiPlayingTime);
	#endif
#else
			err = MEDIAPB_IPC_CSO_Stop(pstMediaPlay->ulCsoSessionId, -1);
#endif
		}
		if ( err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Stop %u\n", __FUNCTION__, __LINE__,pstMediaPlay->ulCsoSessionId);
		}
	}

	/* make them default */
	if ( pstMediaPlay->nUnderflowThreshold > 0 )
	{
		pstMediaPlay->nUnderflowThreshold = 0;
	}
	if ( pstMediaPlay->bUnderflowAutoMode )
	{
		pstMediaPlay->bUnderflowAutoMode = FALSE;
	}

/************************
*	Free Slot
************************/
#ifdef CONFIG_SUPPORT_ROVI
	hErr = MGR_MPlayer_SI_Stop(pstMediaPlay->pMediaPlayer);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error ("MGR_MPlayer_SI_Stop() Failed.\n");
	}
#endif

	if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX)
	{
		if(!pstMediaPlay->bExtOpened)
		{
			CSO_Settings_t *pSettings = NULL;
			if(pstMediaPlay->pCsoSettings != NULL)
			{
				pSettings = pstMediaPlay->pCsoSettings;
				(void)MEDIAPB_IPC_CSO_IPCMSG_MemFree_Settings(*pSettings);
				OxMGR_Free( pstMediaPlay->pCsoSettings );
				pstMediaPlay->pCsoSettings = NULL;
 			}
			err = MEDIAPB_IPC_CSO_Close(pstMediaPlay->ulCsoSessionId);
			if ( err != CSO_ERR_OK)
			{
				HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Close %u\n", __FUNCTION__, __LINE__,pstMediaPlay->ulCsoSessionId);
			}
		}
	}

	/* Reset CopyProtection */
	{
		MgrCopyrightControl_OutputControlInfo_t stCopyControl;
		HxSTD_MemSet(&stCopyControl, 0x00, sizeof(MgrCopyrightControl_OutputControlInfo_t));
		hErr = MGR_OUTPUT_SetCopyProtect(ulActionId, stCopyControl);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("MGR_OUTPUT_SetCopyProtect Fail \n");
		}
	}

	/* Stop DVB/EBU Subtitle */
	om_media_play_mediapb_processDvbEbuSubtitle(ulActionId, SUBTITLE_OFF);

	hErr = MGR_MPlayer_Stop (pstMediaPlay->pMediaPlayer);
	if ( ERR_OK != hErr )
	{
		HxLOG_Error ("MGR_MPlayer_Stop() Failed.\n");
	}

	if(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT)
	{
		HUINT32 ulIdx=0;

		for(ulIdx = 0; ulIdx<pstMediaPlay->ulUsedPumpCount; ++ulIdx)
		{
			hErr = SVC_CSO_FreePlaypumpIndex(pstMediaPlay->pMediaPlayer, pstMediaPlay->pulPumpId[ulIdx]);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("SVC_CSO_FreePlaypumpIndex Index[%u] failed!\n", ulIdx);
			}
		}
	}
	else
	{
		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE)
		{
			/* Destroy AudioFade Task */
			hErr = SVC_CSO_DestroyAudioFadeTask(pstMediaPlay->pMediaPlayer);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to destroy AudioFade Task, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
			}
		}

		hErr = SVC_CSO_FreePlaypumpIndex(pstMediaPlay->pMediaPlayer, pstMediaPlay->ulPumpId);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("SVC_CSO_FreePlaypumpIndex failed!\n");
		}
	}

	if( pstMediaPlay->pulPumpId )
	{
		HLIB_STD_MemFree(pstMediaPlay->pulPumpId);
		pstMediaPlay->pulPumpId = NULL;
	}
	if( pstMediaPlay->ePumpType )
	{
		HLIB_STD_MemFree(pstMediaPlay->ePumpType);
		pstMediaPlay->ePumpType = NULL;
	}


#if defined(CONFIG_MW_OUTPUT_HDAUDIO_SR_RESTRICT)
	if ( pstMediaPlay->bHdmiMute )
	{
		pstMediaPlay->bHdmiMute = FALSE;
		SVC_OUT_SetHdmiAudioMute( pstMediaPlay->bHdmiMute );
	}
#endif

	pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = 0;
#if defined(CONFIG_TIMESTAMP_64)
	pstMediaPlay->stCsoStatus.ullRequestPlayTime = 0;
	pstMediaPlay->stCsoStatus.ullPlayingTime =0;
#else
	pstMediaPlay->stCsoStatus.uiRequestPlayTime = 0;
	pstMediaPlay->stCsoStatus.uiPlayingTime = 0;
#endif
	pstMediaPlay->stCsoStatus.uiPlayingSpeed = 0;
	pstMediaPlay->stCsoStatus.uiDecoderSpeed = 0;
	pstMediaPlay->ullDuration = 0;
	pstMediaPlay->ullStartTime = 0;
	pstMediaPlay->ullFirstPts = 0;
	pstMediaPlay->bAirplaying = 0;
	pstMediaPlay->fAirPlayingStartPercentage = 0.0;
	pstMediaPlay->bStartRequested = FALSE;
	pstMediaPlay->stCsoStatus.eStatus = eCSO_STOPPED;
	pstMediaPlay->bSupport_trick = TRUE;
	pstMediaPlay->bSupport_seek = TRUE;
	pstMediaPlay->bBofReached = FALSE;
	pstMediaPlay->bBufferingCompleted = FALSE;
#ifdef UPDATE_BUFFERINFO_ON_TASK
	if(s_pCsoBufferInfoThread[CalcActionId(pstContext->ulActionId)] != 0)
	{
		s_pCsoBufferInfoThread[CalcActionId(pstContext->ulActionId)] = 0;
	}
#endif
	return ERR_OK;
}

/**
* @b Function @b Description <br>
* this function makes Stopped --> Probing
*
* @param[in] omMediaPlay_Context_t   : media pb context
* @param[in] ulActionId   : ActionID
* @param[in] pstProbe   : Probeinfo
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/
STATIC HERROR om_media_play_mediapb_cso_ProbeRequest(omMediaPlay_Context_t *pstContext, HUINT32 ulActionId, HCHAR* pszUri)
{
	CSO_ERR 		err;
	HERROR			hErr;
	CSO_Settings_t *pSettings = NULL;
	CSO_Settings_t dlnaSettings;
	HCHAR * parsedUrl = NULL;
	HCHAR * dlnaUrl = NULL;
	HCHAR * airplayUrl = NULL;
	HUINT32			ulUALen = 0;
	OmDownload_IpMediaInfo_t *pIpMediaInfo = NULL;
	HBOOL bNecessarySettings = FALSE;

	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	HxSTD_MemSet(&pstMediaPlay->stCsoConfig, 0, sizeof(CalypsoConfig_t));
	HxSTD_MemSet(&dlnaSettings, 0, sizeof(CSO_Settings_t));

	pstMediaPlay->bExtOpened = om_media_play_mediapb_cso_is_opened (pszUri, &pstMediaPlay->ulCsoSessionId);
	if(!pstMediaPlay->bExtOpened)
	{
		hErr = SVC_CSO_GetDnPConfig(pstMediaPlay->pMediaPlayer, (SvcCso_Config_t*)&pstMediaPlay->stCsoConfig);
		if ( hErr != ERR_OK )
		{
			HxLOG_Error ("SVC_CSO_GetDnPConfig() Failed.\n");
			return ERR_FAIL;
		}
		pstMediaPlay->bDlna = FALSE;
		dlnaUrl = om_media_play_mediapb_cso_is_dlna(pszUri, &dlnaSettings);
		if ( dlnaUrl )
		{
			pstMediaPlay->bDlna = TRUE;
			parsedUrl = dlnaUrl;
			HxLOG_Print("[%s][%d] Found DLNA Url Parsed = %s\n", __FUNCTION__,__LINE__, parsedUrl);
		}
		airplayUrl = om_media_play_mediapb_cso_is_airplay(pszUri, pstMediaPlay);
		if ( airplayUrl )
		{
			if( parsedUrl )
			{
				OxMGR_Free( parsedUrl );
			}
			parsedUrl = airplayUrl;
			HxLOG_Print("[%s][%d] Found Airplay Url Parsed = %s\n", __FUNCTION__,__LINE__, parsedUrl);
		}

#if defined(CONFIG_MW_OUTPUT_HDAUDIO_SR_RESTRICT)
		pstMediaPlay->bHdAudio = om_media_play_mediapb_is_hdAudio(pszUri);
#endif

		if ( parsedUrl == NULL )
		{
			parsedUrl = HLIB_STD_StrDup(pszUri);
	//		parsedUrl = HLIB_STD_StrDup("http://10.0.14.41/streams/wmv/theeast_5419270_1901_2.wmv");

			/* fix for prevent #168878 */
			if(NULL == parsedUrl)
			{
				HxLOG_Error("parsedUrl is NULL\n");
				return ERR_FAIL;
			}

			HxLOG_Print("[%s][%d] Original Url = %s\n", __FUNCTION__,__LINE__, parsedUrl);
		}

		if ((pstMediaPlay->stCsoConfig.bProgressiveDownload) && (dlnaSettings.pProtocolInfo == NULL))
		{
			err = MEDIAPB_IPC_CSO_Open(parsedUrl, eCSO_MODE_DNP, &pstMediaPlay->ulCsoSessionId);
			if(err != CSO_ERR_OK)
			{
				HxLOG_Error("MEDIAPB_IPC_CSO_Open failed!\n");
				pstMediaPlay->stCsoStatus.eStatus = eCSO_STOPPED;

				if( parsedUrl )
				{
					HLIB_STD_MemFree (parsedUrl);
				}

				return ERR_FAIL;
			}
		}
		else
		{
			err = MEDIAPB_IPC_CSO_Open(parsedUrl, eCSO_MODE_PLAY, &pstMediaPlay->ulCsoSessionId);
			if(err != CSO_ERR_OK)
			{
				HxLOG_Error("MEDIAPB_IPC_CSO_Open failed!\n");
				pstMediaPlay->stCsoStatus.eStatus = eCSO_STOPPED;

				if( parsedUrl )
				{
					HLIB_STD_MemFree (parsedUrl);
				}

				return ERR_FAIL;
			}
		}
	}
	else
	{
		pIpMediaInfo = OM_DOWNLOAD_GetMediaInfoByCsoId(pstMediaPlay->ulCsoSessionId);
		if(pIpMediaInfo == NULL)
		{
			return ERR_FAIL;
		}
		pstMediaPlay->bDlna = pIpMediaInfo->bDlna;
		pstMediaPlay->bAirplaying = pIpMediaInfo->bAirplaying;
		pstMediaPlay->fAirPlayingStartPercentage = pIpMediaInfo->fAirPlayingStartPercentage;
		pstMediaPlay->ullBasePosition = pIpMediaInfo->ullBasePosition;

		if(NULL != pIpMediaInfo->pCsoSettings)
		{
			pSettings = (CSO_Settings_t *)pIpMediaInfo->pCsoSettings;
		}
	}

	HxLOG_Error("[%s](L:%d) pstMediaPlay->ulCsoSessionId : %lu\n", __FUNCTION__, __LINE__, pstMediaPlay->ulCsoSessionId);
	if(pSettings == NULL)
	{
		pSettings = HLIB_STD_MemAlloc(sizeof(CSO_Settings_t));
		HxSTD_MemSet(pSettings, 0x00, sizeof(CSO_Settings_t));

		HxLOG_Error("[%s](L:%d) It's first time to getting cso's setting value!\n", __FUNCTION__, __LINE__);
		err = MEDIAPB_IPC_CSO_GetSettings(pstMediaPlay->ulCsoSessionId, pSettings);

	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_GetSettings failed!\n");
	}
		else
		{
			// store cache
			// download 모듈을 사용하지 않음.
			if(!pstMediaPlay->bExtOpened)
			{
				if(pstMediaPlay->pCsoSettings != NULL)
				{
					OxMGR_Free(pstMediaPlay->pCsoSettings);
					pstMediaPlay->pCsoSettings = NULL;
				}
				HxLOG_Error("[%s](L:%d) so, store cso's setting value to cache in MediaPB\n", __FUNCTION__, __LINE__);
				pstMediaPlay->pCsoSettings = pSettings;
			}
			// download 모듈을 사용.
			else
			{
				if(pIpMediaInfo != NULL)
				{
					if(pIpMediaInfo->pCsoSettings != NULL)
					{
						HxLOG_Error("[%s:%d] pIpMediaInfo->pCsoSettings != NULL\n", __FUNCTION__, __LINE__);
						OxMGR_Free(pIpMediaInfo->pCsoSettings);
						pIpMediaInfo->pCsoSettings = NULL;
					}
					HxLOG_Error("[%s](L:%d) so, store cso's setting value to cache in Download\n", __FUNCTION__, __LINE__);
					pIpMediaInfo->pCsoSettings = (void *)pSettings;
				}
				else
				{
					HxLOG_Error("[%s](L:%d) Can't Store cache\n", __FUNCTION__, __LINE__);
				}
			}
		}
	}
	else
	{
		HxLOG_Error("[%s](L:%d) Using cso'setting value from cache\n", __FUNCTION__, __LINE__);
	}

	/* File Path of DNP */
	if(pSettings != NULL && (pstMediaPlay->stCsoConfig.bProgressiveDownload) && (dlnaSettings.pProtocolInfo == NULL))
	{
		if(pSettings->pSaveFilePath != NULL && strcmp(pSettings->pSaveFilePath, pstMediaPlay->stCsoConfig.szPDLPath) == 0)
		{
			HxLOG_Error("[%s](L:%d) Same File Path, so doesn't set settings \n", __FUNCTION__, __LINE__);
		}
		else
		{
			if(pSettings->pSaveFilePath != NULL)
			{
				OxMGR_Free(pSettings->pSaveFilePath);
				pSettings->pSaveFilePath = NULL;
			}
			pSettings->pSaveFilePath = HLIB_STD_StrDup(pstMediaPlay->stCsoConfig.szPDLPath);
			bNecessarySettings = TRUE;
		}
		HxLOG_Error("[%s](L:%d) Save File Path = %s\n", __FUNCTION__, __LINE__, pSettings->pSaveFilePath);
	}

	/* */
	if (pSettings != NULL && parsedUrl && strstr(parsedUrl, "raop://") )
	{
		HxSTD_memcpy ( pSettings->tRaopSetting.ucAesKey, pstMediaPlay->stRaopSetting->ucAesKey, 16 );
		HxSTD_memcpy ( pSettings->tRaopSetting.ucAesIv, pstMediaPlay->stRaopSetting->ucAesIv, 16 );
		pSettings->tRaopSetting.unDataPort = pstMediaPlay->stRaopSetting->unDataPort;
		pSettings->tRaopSetting.unControlPort = pstMediaPlay->stRaopSetting->unControlPort;
		pSettings->tRaopSetting.unTimingPort = pstMediaPlay->stRaopSetting->unTimingPort;
		pSettings->tRaopSetting.unDestIpAddress = 100;
		strncpy(pSettings->tRaopSetting.ucAudioCodec , pstMediaPlay->stRaopSetting->ucAudioCodec, sizeof(pSettings->tRaopSetting.ucAudioCodec) - 1);
		strncpy(pSettings->tRaopSetting.ucFmtStr , pstMediaPlay->stRaopSetting->ucFmtStr, sizeof( pSettings->tRaopSetting.ucFmtStr) - 1);
		bNecessarySettings = TRUE;
	}

	/* Playready DRM Setting */
#if defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	if(pSettings != NULL)
	{
		pSettings->ullDecThresholdSW = 50*1024*1024; /* SW Decrypt Enable, BlockSize < 50 Mbytes */
		bNecessarySettings = TRUE;
	}
#endif
	if(pSettings != NULL && s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData != NULL)
	{
		if(pSettings->tPRDYSetting.pucLicenseData != NULL)
		{
			OxMGR_Free(pSettings->tPRDYSetting.pucLicenseData);
			pSettings->tPRDYSetting.pucLicenseData = NULL;
		}
		pSettings->tPRDYSetting.pucLicenseData = HLIB_STD_StrDup(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData);
		HxLOG_Error("LicenseData = %s\n", pSettings->tPRDYSetting.pucLicenseData);
		bNecessarySettings = TRUE;
	}

	if(pSettings != NULL && s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL != NULL )
	{
		if(pSettings->tPRDYSetting.pucLAURL != NULL)
		{
			OxMGR_Free(pSettings->tPRDYSetting.pucLAURL);
			pSettings->tPRDYSetting.pucLAURL = NULL;
		}
		pSettings->tPRDYSetting.pucLAURL = HLIB_STD_StrDup(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL);
		bNecessarySettings = TRUE;
	}

	if(pSettings != NULL && s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData != NULL )
	{
		if(pSettings->tPRDYSetting.pucCustomData != NULL)
		{
			OxMGR_Free(pSettings->tPRDYSetting.pucCustomData);
			pSettings->tPRDYSetting.pucCustomData = NULL;
		}
		pSettings->tPRDYSetting.pucCustomData = HLIB_STD_StrDup(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData);
		bNecessarySettings = TRUE;
	}
	//Setting LA URL. (Case of SendDRMMessage)
	if(pSettings != NULL && pstContext->stCache.pucLAURL!=NULL)
	{// Using DrmAgent sendDRMMessage.    LAURL Override data is saved in pstContext->stCache.pucLAURL
		HxLOG_Error("PRDY : Setting LAURL %s\n", pstContext->stCache.pucLAURL);
		if(pSettings->tPRDYSetting.pucLAURL != NULL)
		{
			HxLOG_Error("[%s:%d] pSettings->tPRDYSetting.pucLAURL is NOT NULL!!\n", __FUNCTION__, __LINE__);
			OxMGR_Free(pSettings->tPRDYSetting.pucLAURL);
			pSettings->tPRDYSetting.pucLAURL = NULL;
		}
		pSettings->tPRDYSetting.pucLAURL = HLIB_STD_StrDup(pstContext->stCache.pucLAURL);
		// After LAURL Override is used, data is reset.
		HLIB_STD_MemFree(pstContext->stCache.pucLAURL);
		pstContext->stCache.pucLAURL = NULL;
		bNecessarySettings = TRUE;
	}

	//Setting CustomData (Case of SendDRMMessage)
	if(pSettings != NULL && pstContext->stCache.pucPRDYCustomData!=NULL)
	{// Using DrmAgent sendDRMMessage.    LAURL Override data is saved in pstContext->stCache.pucLAURL
		HxLOG_Error("PRDY : Setting CustomData %s\n", pstContext->stCache.pucPRDYCustomData);
		if(pSettings->tPRDYSetting.pucCustomData!= NULL)
	{
			HxLOG_Error("[%s:%d] pSettings->tPRDYSetting.pucLAURL is NOT NULL!!\n", __FUNCTION__, __LINE__);
			OxMGR_Free(pSettings->tPRDYSetting.pucCustomData);
			pSettings->tPRDYSetting.pucCustomData = NULL;
		}
		pSettings->tPRDYSetting.pucCustomData= HLIB_STD_StrDup(pstContext->stCache.pucPRDYCustomData);
		// After Customdata is used, data is reset.
		HLIB_STD_MemFree(pstContext->stCache.pucPRDYCustomData);
		pstContext->stCache.pucPRDYCustomData = NULL;
		bNecessarySettings = TRUE;
	}

	/* DLNA Setting */
	if (pSettings != NULL && dlnaSettings.pProtocolInfo )
	{
		if( pSettings->pProtocolInfo != NULL)
		{
			OxMGR_Free(pSettings->pProtocolInfo);
			pSettings->pProtocolInfo = NULL;
		}
		pSettings->pProtocolInfo = HLIB_STD_StrDup(dlnaSettings.pProtocolInfo);
		pSettings->tDlnaSetting.nByteRate = dlnaSettings.tDlnaSetting.nByteRate;
		pSettings->tDlnaSetting.unDuration = dlnaSettings.tDlnaSetting.unDuration;
		pSettings->tDlnaSetting.nFileSize = dlnaSettings.tDlnaSetting.unDuration;

		HxLOG_Error("DLNA : ProtocolInfo %s\n", pSettings->pProtocolInfo);
		bNecessarySettings = TRUE;
	}

	/* VMX DRM Setting */
	if(pSettings != NULL && s_stMediaPlayerCache[CalcActionId(ulActionId)].bUseVmx == TRUE && s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl != NULL)
	{
		HxLOG_Debug("VMX is Enabled\n");
		pSettings->eDrmType = eCSO_DRM_VIEWRIGHTWEB;
		if(pSettings->tVRWSetting.pCompanyName != NULL)
		{
			OxMGR_Free(pSettings->tVRWSetting.pCompanyName);
			pSettings->tVRWSetting.pCompanyName = NULL;
		}
		if(pSettings->tVRWSetting.pVCASBootAddr != NULL)
		{
			OxMGR_Free(pSettings->tVRWSetting.pVCASBootAddr);
			pSettings->tVRWSetting.pVCASBootAddr = NULL;
		}
		if( s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName != NULL)
		{
			pSettings->tVRWSetting.pCompanyName = HLIB_STD_StrDup(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName);
		}
		pSettings->tVRWSetting.pVCASBootAddr = HLIB_STD_StrDup(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl);
		bNecessarySettings = TRUE;
		/* Do not release memory of strings(s_pucVmxVendorName[ulActionId], s_pucVmxCasUrl[ulActionId]) after setting.*/
	}
#if 1
	/* NAGRA PRM */
	if(pSettings != NULL && pstContext->stOmCache.stDrmInformation.eDRMType == MGR_MEDIA_DRMTYPE_NAGRAPRM)
	{
		HxLOG_Debug("NagraPRM is Enabled\n");
		pSettings->eDrmType = eCSO_DRM_NAGRAPRM;
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
		pSettings->tNAGRAPRMSetting.hHdl = pstContext->stOmCache.stDrmInformation.ulKeyHandle;
		pSettings->tNAGRAPRMSetting.usEmi = pstContext->stOmCache.stDrmInformation.usEmi;
		pSettings->tNAGRAPRMSetting.ulKeySize = pstContext->stOmCache.stDrmInformation.ulKeyIdSize;
		if(pSettings->tNAGRAPRMSetting.ulKeySize > 0
		&& pSettings->tNAGRAPRMSetting.ulKeySize < sizeof(pSettings->tNAGRAPRMSetting.ucEncryptKey)
		&& pSettings->tNAGRAPRMSetting.ulKeySize <= sizeof(pstContext->stOmCache.stDrmInformation.aucKeyId))
		{
			HxSTD_memcpy(pSettings->tNAGRAPRMSetting.ucEncryptKey, pstContext->stOmCache.stDrmInformation.aucKeyId, pSettings->tNAGRAPRMSetting.ulKeySize);
		}
#else
		pSettings->tNAGRAPRMSetting.ulKeySize = pstContext->stOmCache.stDrmInformation.ulKeySize;
		if(pSettings->tNAGRAPRMSetting.ulKeySize > 0
		&& pSettings->tNAGRAPRMSetting.ulKeySize < sizeof(pSettings->tNAGRAPRMSetting.ucEncryptKey)
		&& pSettings->tNAGRAPRMSetting.ulKeySize <= sizeof(pstContext->stOmCache.stDrmInformation.ucEncryptKey))
		{
			HxSTD_memcpy(pSettings->tNAGRAPRMSetting.ucEncryptKey, pstContext->stOmCache.stDrmInformation.ucEncryptKey, pSettings->tNAGRAPRMSetting.ulKeySize);
		}
		bNecessarySettings = TRUE;
#endif
	}
#endif

#if defined(CONFIG_PROD_FVP4000T)
	if(pSettings != NULL && pSettings->bStartLowestStream == FALSE)
	{
		pSettings->bStartLowestStream = TRUE;
		bNecessarySettings = TRUE;
	}
#endif

	if(pSettings != NULL)
	{
		HBOOL bAudioDescription = FALSE;
		HxLANG_Id_e eAudioLangId = eLangID_MAX;

		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION, (HUINT32 *)&bAudioDescription, 0);
		if (ERR_OK != hErr)
		{
			bAudioDescription = FALSE;
		}
		if(bAudioDescription)
		{
			pSettings->bSelectAudioDescription = TRUE;
		}
		else
		{
			pSettings->bSelectAudioDescription = FALSE;
		}

		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0, (HUINT32 *)&eAudioLangId, 0);
		if (ERR_OK != hErr)
		{
			pSettings->ePreperredAudioLanguage = CSO_LANG_NONE;
			bNecessarySettings = TRUE;
		}
		else if (ERR_OK == hErr  && (eAudioLangId != eLangID_MAX))
		{
			if(pSettings->ePreperredAudioLanguage != (CSO_LangCode_t)eAudioLangId)
			{
				pSettings->ePreperredAudioLanguage = (CSO_LangCode_t)eAudioLangId;	/* CSO_LANG is compatible with HxLANG. So directly assign here */
				bNecessarySettings = TRUE;
			}
		}
	}

//	memcpy(s_BrowserUserAgent,"Linux; U; HbbTV/1.1.1 (Humax;Calypso;) ce-html/1.0 (NETRANGEMMH; );",strlen("Linux; U; HbbTV/1.1.1 (Humax;Calypso;) ce-html/1.0 (NETRANGEMMH; );") + 1);
	ulUALen =  VK_strlen(s_BrowserUserAgent);
	if(pSettings != NULL && ulUALen > 0)
	{
		HxLOG_Debug("Browser User Agent Data %s\n",s_BrowserUserAgent);
		if(pSettings->pUserAgentString != NULL && strcmp(pSettings->pUserAgentString, s_BrowserUserAgent) == 0)
		{
			HxLOG_Error("[%s](L:%d) Same User Agent, so doesn't set settings \n", __FUNCTION__, __LINE__);
		}
		else
		{
			if(pSettings->pUserAgentString != NULL)
		{
				OxMGR_Free(pSettings->pUserAgentString);
				pSettings->pUserAgentString = NULL;
			}
			pSettings->pUserAgentString = HLIB_STD_StrDup(s_BrowserUserAgent);
			bNecessarySettings = TRUE;
		}
	}

	if(pSettings != NULL && bNecessarySettings == TRUE)
	{
#ifdef CONFIG_SUPPORT_ROVI
		pSettings->bBypassing = TRUE;
#endif
		err = MEDIAPB_IPC_CSO_SetSettings(pstMediaPlay->ulCsoSessionId, pSettings);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_SetSettings failed!\n");
	}
	}
	else
		HxLOG_Error("[%s](L:%d) Don't Set Settings \n", __FUNCTION__, __LINE__);

	HxLOG_Error("[%s](L:%d) MEDIAPB_IPC_CSO_Open - SessionId = %d\n", __FUNCTION__, __LINE__, pstMediaPlay->ulCsoSessionId);

	if ( s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData )
	{
		OxMGR_Free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData);
		s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData = NULL;
	}

#if 1 /*TODO */
	if ( parsedUrl != NULL )
	{
		OxMGR_Free(parsedUrl);
	}
#endif

	//HxLOG_Error("%s(%d) [CRB] Start CSO_PROBE in MediaPB. \n", __FUNCTION__, __LINE__);
	if(pstMediaPlay->bExtOpened)
	{
		OmDownload_Instance_t *pDownInst = OM_DOWNLOAD_GetInstanceWithCsoId(pstMediaPlay->ulCsoSessionId);
		if (pDownInst && pDownInst->unExtInfo.stIpMedia.pSession)
		{
			CsoSessionDNLD_t *pDownSession  = (CsoSessionDNLD_t *)pDownInst->unExtInfo.stIpMedia.pSession;
			CSO_ProbeInfo_t *pCsoProbeInfo = (CSO_ProbeInfo_t *)pDownSession->pProbeInfo;
			if (pCsoProbeInfo)
			{
				om_media_play_mediapb_cso_StoreProbeInfo(pstContext, pCsoProbeInfo);
				   om_media_play_mediapb_EvtPlayInfo(pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), (HINT32)pstMediaPlay->ullSeekedTimeByCso, (HINT32)pstMediaPlay->ullDuration, pstMediaPlay->ulBitrate);
				   om_media_play_mediapb_EvtBufferingCanplay (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				return ERR_OK;
			}
		}
	}
	err = MEDIAPB_IPC_CSO_Probe(pstMediaPlay->ulCsoSessionId);
	if (err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Probe\n", __FUNCTION__, __LINE__);
		if(!pstMediaPlay->bExtOpened)
		{
			CSO_Settings_t *pSettings = NULL;
			if(pstMediaPlay->pCsoSettings != NULL)
			{
				pSettings = pstMediaPlay->pCsoSettings;
				(void)MEDIAPB_IPC_CSO_IPCMSG_MemFree_Settings(*pSettings);
				OxMGR_Free( pstMediaPlay->pCsoSettings );
				pstMediaPlay->pCsoSettings = NULL;
 			}
			err = MEDIAPB_IPC_CSO_Close( pstMediaPlay->ulCsoSessionId );
			if ( err != CSO_ERR_OK)
			{
				HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Close\n", __FUNCTION__, __LINE__);
			}
		}
		pstMediaPlay->stCsoStatus.eStatus = eCSO_STOPPED;
	}

	pstMediaPlay->stCsoStatus.eStatus = eCSO_PROBING;

	return ERR_OK;
}

/**
* @b Function @b Description <br>
* this function makes Probing --> Stopped
*
* @param[in] omMediaPlay_Context_t   : media pb context
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/
STATIC HERROR om_media_play_mediapb_cso_CancelProbeRequest(omMediaPlay_Context_t *pstContext)
{
	CSO_ERR 		err;

	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	HxLOG_Error("\n\n\n[%s](L:%d) cancel it \n", __FUNCTION__, __LINE__);
	if(!pstMediaPlay->bExtOpened)
	{
		CSO_Settings_t *pSettings = NULL;
		if(pstMediaPlay->pCsoSettings != NULL)
		{
			pSettings = pstMediaPlay->pCsoSettings;
			(void)MEDIAPB_IPC_CSO_IPCMSG_MemFree_Settings(*pSettings);
			OxMGR_Free( pstMediaPlay->pCsoSettings );
			pstMediaPlay->pCsoSettings = NULL;
			}
		err = MEDIAPB_IPC_CSO_Close(pstMediaPlay->ulCsoSessionId);
		if ( err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Close %u\n", __FUNCTION__, __LINE__,pstMediaPlay->ulCsoSessionId);
		}
	}

	pstMediaPlay->stCsoStatus.eStatus = eCSO_STOPPED;
#if defined(CONFIG_TIMESTAMP_64)
	pstMediaPlay->stCsoStatus.ullRequestPlayTime = 0;
	pstMediaPlay->stCsoStatus.ullPlayingTime =0;
#else
	pstMediaPlay->stCsoStatus.uiRequestPlayTime = 0;
	pstMediaPlay->stCsoStatus.uiPlayingTime =0;
#endif
	pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = 0;
	pstMediaPlay->stCsoStatus.uiPlayingSpeed = 0;
	pstMediaPlay->ullDuration = 0;
	if ( pstMediaPlay->bStartRequested )
	{
		pstMediaPlay->bStartRequested = FALSE;
	}

	return ERR_OK;
}

STATIC HERROR om_media_play_mediapb_cso_StoreProbeInfo(omMediaPlay_Context_t *pstContext, CSO_ProbeInfo_t *cso_probeinfo)
{
	HUINT32		ulActionId		=	0;
	HUINT32 	ulViewId 		= 	0;
	HUINT32		ulSessionId 	=	0;

	HUINT32 		tracks = 0;
	HUINT32 		nTotalVideoTrack = 0;
	HUINT32 		nTotalAudioTrack = 0;
	HUINT32 		nTotalSubttlTrack = 0;
	HBOOL			bVideoPlayable = FALSE;
	HBOOL			bAudioPlayable = FALSE;
	HBOOL			bSubtitlePlayable = FALSE;
	HUINT16 		uValidVTrackCnt = 0; /* debugging */
	HUINT16 		uValidATrackCnt = 0; /* debugging */
	HUINT16 		uValidSTrackCnt = 0; /* debugging */
	OxMediaPlay_AudioCompTable_t *pstAudioCompTable	= NULL;
	OxMediaPlay_VideoCompTable_t *pstVideoCompTable	= NULL;
	OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable = NULL;
	OxMediaPlay_Cache_t *pCache = NULL;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pCache = &pstContext->stCache;

	ulActionId		=	pstContext->ulActionId;
	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	ulSessionId		=	pstContext->ulSessionId;
	pstAudioCompTable = &pCache->stAudioCompTable;
	pstVideoCompTable =	&pCache->stVideoCompTable;
	pstSubtitleCompTable = &pCache->stSubtitleCompTable;

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	pstMediaPlay->ulActivatedAudioCodecId = eDxAUDIO_CODEC_UNKNOWN;
	pstMediaPlay->ulActivatedVideoCodecId = eDxVIDEO_CODEC_UNKNOWN;

	/* AvailabilityStartTime 획득*/
	memcpy(pCache->szAvailabilityStartTime, cso_probeinfo->uacAvailabilityStartTime, sizeof(pCache->szAvailabilityStartTime));
	HxLOG_Debug("AvailabilityStartTime : %s\n", pCache->szAvailabilityStartTime);
	if(HxSTD_StrLen(pstContext->stCache.szAvailabilityStartTime) > 0)
	{
		om_media_play_CbOnAvailabilityStartTimeChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, pstContext->stCache.szAvailabilityStartTime);
	}
	if(cso_probeinfo->ullInternalSeekedTime != 0)
	{
		pstMediaPlay->ullSeekedTimeByCso = cso_probeinfo->ullInternalSeekedTime;
		HxLOG_Debug("Calypso internal seek has been occured (Pos : %lld)\n", cso_probeinfo->ullInternalSeekedTime);
	}

	/* VideoTrack 정보 획득...*/
	uValidVTrackCnt = 0;
	HxSTD_MemSet (pstVideoCompTable, 0, sizeof(OxMediaPlay_VideoCompTable_t));
	if(cso_probeinfo->ucTotalVideoTrack > OxMP_VIDEO_ES_NUM_MAX)
	{
		HxLOG_Error ("MAX OxMP_VIDEO_ES_NUM_MAX is %d, But video track is %d from CSO_Probe. So the number of emitting OxMP_VIDEO_ES_NUM_MAX is %d\n", OxMP_VIDEO_ES_NUM_MAX, cso_probeinfo->ucTotalVideoTrack, OxMP_VIDEO_ES_NUM_MAX);
		nTotalVideoTrack = OxMP_VIDEO_ES_NUM_MAX;
	}
	else
	{
		nTotalVideoTrack = cso_probeinfo->ucTotalVideoTrack;
	}
	for ( tracks = 0; tracks < nTotalVideoTrack; tracks++ )
	{
		pstVideoCompTable->nCurrIndex		=	tracks;
		pstVideoCompTable->nArraySize		=	tracks+1;
		pstVideoCompTable->astCompArray[tracks].nPid		=	cso_probeinfo->stVideoTrack[tracks].video_stream_id;
		pstVideoCompTable->astCompArray[tracks].eCodec		=	om_media_play_mediapb_cso_doConvertVideoCodec(cso_probeinfo->stVideoTrack[tracks].video_codec);
		pstVideoCompTable->astCompArray[tracks].eCompType 	=	eOxMP_COMPONENT_VIDEO;
		pstVideoCompTable->astCompArray[tracks].nHdVideo	=	0; // N/A
		pstVideoCompTable->astCompArray[tracks].nComponentTag =	0; // N/A
		pstVideoCompTable->astCompArray[tracks].nCasFound 	=	0; // N/A
		memcpy(pstVideoCompTable->astCompArray[tracks].aucCompName, cso_probeinfo->stVideoTrack[tracks].uacVideoName, OxMP_COMP_NAME_NUM);
		memcpy(pstVideoCompTable->astCompArray[tracks].aucCompRole, cso_probeinfo->stVideoTrack[tracks].uacVideoRole, OxMP_COMP_ROLE_NUM);
		if ( pstVideoCompTable->astCompArray[tracks].eCodec != eDxVIDEO_CODEC_UNKNOWN )
		{
			bVideoPlayable = TRUE;
			pstMediaPlay->ulActivatedVideoCodecId = om_media_play_mediapb_cso_doConvertVideoCodec(cso_probeinfo->video_codec);
			pstMediaPlay->ulActivatedVideoStreamId = cso_probeinfo->video_stream_id;
			uValidVTrackCnt++;
		}
	}
	pstVideoCompTable->nMaxSize			=	sizeof(OxMediaPlay_VideoCompTable_t);

	for( tracks = 0; tracks < pstVideoCompTable->nArraySize; tracks++ )
	{
		HxLOG_Debug ("[%s](L:%d) Found track %d video codec %d \n", __FUNCTION__, __LINE__, tracks, pstVideoCompTable->astCompArray[tracks].eCodec );
		HxLOG_Debug ("[%s](L:%d) Component Name : %s, Component Role : %s\n", __FUNCTION__, __LINE__,
			pstVideoCompTable->astCompArray[tracks].aucCompName, pstVideoCompTable->astCompArray[tracks].aucCompRole);
	}

	/* AudioTrack 정보 획득...*/
	uValidATrackCnt = 0;
	HxSTD_MemSet (pstAudioCompTable, 0, sizeof(OxMediaPlay_AudioCompTable_t));
	if(cso_probeinfo->ucTotalAudioTrack > OxMP_AUDIO_ES_NUM_MAX)
	{
		HxLOG_Error ("MAX OxMP_AUDIO_ES_NUM_MAX is %d, But audio track is %d from CSO_Probe. So the number of emitting OxMP_AUDIO_ES_NUM_MAX is %d\n", OxMP_AUDIO_ES_NUM_MAX, cso_probeinfo->ucTotalAudioTrack, OxMP_AUDIO_ES_NUM_MAX);
		nTotalAudioTrack = OxMP_AUDIO_ES_NUM_MAX;
	}
	else
	{
		nTotalAudioTrack = cso_probeinfo->ucTotalAudioTrack;
	}
	for ( tracks = 0; tracks < nTotalAudioTrack; tracks++ )
	{
		pstAudioCompTable->nCurrIndex		=	tracks;
		pstAudioCompTable->nArraySize		=	tracks+1;
		pstAudioCompTable->astCompArray[tracks].nPid			=	cso_probeinfo->stAudioTrack[tracks].audio_stream_id;
		pstAudioCompTable->astCompArray[tracks].eCompType 	=	eOxMP_COMPONENT_AUDIO;
		pstAudioCompTable->astCompArray[tracks].nComponentTag =	0; //N/A
		pstAudioCompTable->astCompArray[tracks].nLangNum		=	0; //stCsoAudioTrack.szAudioLanguage

		memcpy(pstAudioCompTable->astCompArray[tracks].aucCompRole, cso_probeinfo->stAudioTrack[tracks].uacAudioRole, OxMP_COMP_ROLE_NUM);
		memcpy(pstAudioCompTable->astCompArray[tracks].aucCompName, cso_probeinfo->stAudioTrack[tracks].uacAudioName, OxMP_COMP_NAME_NUM);
		if( strstr (cso_probeinfo->stAudioTrack[tracks].uacAudioRole, "description") )
		{
			/* Audio Role 내 description string이 발견될 경우 VISUAL IMPAIRED 로 세팅 */
			pstAudioCompTable->astCompArray[tracks].astLangInfo[0].eAudType = eOxMP_AUDIOTYPE_VISUAL_IMPAIRED;
		}
		/* 우선 CSO로 얻은 Audio Lang은 0번째 LangInfor에 저장한다. */
		memcpy(pstAudioCompTable->astCompArray[tracks].astLangInfo[0].szLangCode, cso_probeinfo->stAudioTrack[tracks].uacAudioLang, 3);
		pstAudioCompTable->astCompArray[tracks].eCodec		=	om_media_play_mediapb_cso_doConvertAudioCodec(cso_probeinfo->stAudioTrack[tracks].audio_codec);
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 0)) /* supported sample_rate at calypso_ver1.1.0 */
		pstAudioCompTable->astCompArray[tracks].ulSampleRate	=	cso_probeinfo->stAudioTrack[tracks].sample_rate;
#else
		pstAudioCompTable->astCompArray[tracks].ulSampleRate	=	0;
#endif
		if ( pstAudioCompTable->astCompArray[tracks].eCodec == eDxAUDIO_CODEC_DOLBY_AC3 || pstAudioCompTable->astCompArray[tracks].eCodec == eDxAUDIO_CODEC_DOLBY_AC3P)
		{
			pstAudioCompTable->astCompArray[tracks].bIsDolby		=	TRUE;
		}
		else
		{
			pstAudioCompTable->astCompArray[tracks].bIsDolby		=	FALSE;
		}

		if ( pstAudioCompTable->astCompArray[tracks].eCodec !=  eDxAUDIO_CODEC_UNKNOWN)
		{
			bAudioPlayable = TRUE;
			pstMediaPlay->ulActivatedAudioCodecId = om_media_play_mediapb_cso_doConvertAudioCodec(cso_probeinfo->audio_codec);
			pstMediaPlay->ulActivatedAudioStreamId = cso_probeinfo->audio_stream_id;
			uValidATrackCnt++;
		}
	}
	pstAudioCompTable->nMaxSize			=	sizeof(OxMediaPlay_AudioCompTable_t);

	for( tracks = 0; tracks < pstAudioCompTable->nArraySize; tracks++ )
	{
		HxLOG_Debug ("[%s](L:%d) Found track %d audio codec %d \n", __FUNCTION__, __LINE__, tracks, pstAudioCompTable->astCompArray[tracks].eCodec );
		HxLOG_Debug ("[%s](L:%d) Component Name : %s, Component Role : %s, Language : %s\n", __FUNCTION__, __LINE__,
			pstAudioCompTable->astCompArray[tracks].aucCompName, pstAudioCompTable->astCompArray[tracks].aucCompRole, pstAudioCompTable->astCompArray[tracks].astLangInfo[0].szLangCode);
	}
#if defined(CONFIG_PROD_FVP4000T)
	if(cso_probeinfo->ucTotalSubTrack > OxMP_SUBTTL_ES_NUM_MAX)
	{
		HxLOG_Error ("MAX OxMP_AUDIO_ES_NUM_MAX is %d, But audio track is %d from CSO_Probe. So the number of emitting OxMP_AUDIO_ES_NUM_MAX is %d\n", OxMP_AUDIO_ES_NUM_MAX, cso_probeinfo->ucTotalAudioTrack, OxMP_AUDIO_ES_NUM_MAX);
		nTotalSubttlTrack = OxMP_SUBTTL_ES_NUM_MAX;
	}
	else
	{
		nTotalSubttlTrack = cso_probeinfo->ucTotalSubTrack;
	}
	/* SubtitleTrack 정보 획득...*/
	uValidSTrackCnt = 0;
	HxSTD_MemSet (pstSubtitleCompTable, 0, sizeof(OxMediaPlay_SubtitleCompTable_t));
	for ( tracks = 0; tracks < nTotalSubttlTrack; tracks++ )
	{
		pstSubtitleCompTable->nCurrIndex		=	tracks;
		pstSubtitleCompTable->nArraySize		=	tracks+1;
		pstSubtitleCompTable->astCompArray[tracks].nPid		=	cso_probeinfo->stSubTrack[tracks].stream_id;
		pstSubtitleCompTable->astCompArray[tracks].eCompType 	=	eOxMP_COMPONENT_SUBTITLE;
		pstSubtitleCompTable->astCompArray[tracks].eSubtitleType = om_media_play_mediapb_cso_doConvertSubtitleType(cso_probeinfo->stSubTrack[tracks].eSubType);
		if ( pstSubtitleCompTable->astCompArray[tracks].eSubtitleType == eOxMP_SUBTITLETYPE_Dvb)
		{
			bSubtitlePlayable = TRUE;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.nTypeValue = pstSubtitleCompTable->astCompArray[tracks].eSubtitleType;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.nCompositionPageId = cso_probeinfo->stSubTrack[tracks].stDvb.usCompositionPageId;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.nAncillaryPageId = cso_probeinfo->stSubTrack[tracks].stDvb.usAncillaryPageId;
			memcpy(pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.szLangCode,cso_probeinfo->stSubTrack[tracks].uacSubLang,3);
			uValidSTrackCnt++;
		}
		else if ( pstSubtitleCompTable->astCompArray[tracks].eSubtitleType == eOxMP_SUBTITLETYPE_Ebu)
		{
			bSubtitlePlayable = TRUE;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stEbuInfo.nTypeValue = pstSubtitleCompTable->astCompArray[tracks].eSubtitleType;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stEbuInfo.nMagazineNum = cso_probeinfo->stSubTrack[tracks].stEbu.ucMagazineNum;
			pstSubtitleCompTable->astCompArray[tracks].utArg.stEbuInfo.nPageNum = cso_probeinfo->stSubTrack[tracks].stEbu.ucPageNum;
			memcpy(pstSubtitleCompTable->astCompArray[tracks].utArg.stEbuInfo.szLangCode,cso_probeinfo->stSubTrack[tracks].uacSubLang,3);
			uValidSTrackCnt++;
		}


	}
	pstSubtitleCompTable->nMaxSize = sizeof(OxMediaPlay_SubtitleCompTable_t);

	for( tracks = 0; tracks < pstSubtitleCompTable->nArraySize; tracks++ )
	{
		HxLOG_Debug ("[%s](L:%d) Found track %d Subtitle Type : %d(%s) \n", __FUNCTION__, __LINE__, tracks, pstSubtitleCompTable->astCompArray[tracks].eSubtitleType,pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.szLangCode);
	}
#endif
#ifdef AV_TRACK_CHANGE
	/* print AV track info */
	for( tracks = 0; tracks < pstVideoCompTable->nArraySize; tracks++ )
	{
		HxLOG_Error ("pstVideoCompTable->astCompArray[%d].nPid								   [ %d ] \n", tracks, pstVideoCompTable->astCompArray[tracks].nPid);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%d].eCodec							   [ %d ] \n\n", tracks, pstVideoCompTable->astCompArray[tracks].eCodec);
	}
	for( tracks = 0; tracks < pstAudioCompTable->nArraySize; tracks++ )
	{
		HxLOG_Error ("pstAudioCompTable->astCompArray[%d].nPid								   [ %d ] \n", tracks, pstAudioCompTable->astCompArray[tracks].nPid);
		HxLOG_Error ("pstAudioCompTable->astCompArray[%d].eCodec							   [ %d ] \n\n", tracks, pstAudioCompTable->astCompArray[tracks].eCodec);
	}
#endif
#ifdef AV_TRACK_CHANGE
	pstMediaPlay->nActivatedAudioTrackNum = pstAudioCompTable->nCurrIndex = cso_probeinfo->nCurrentAudioTrack;
	pstMediaPlay->ulActivatedAudioCodecId  = pstAudioCompTable->astCompArray[cso_probeinfo->nCurrentAudioTrack].eCodec;
	pstMediaPlay->ulActivatedAudioStreamId = pstAudioCompTable->astCompArray[cso_probeinfo->nCurrentAudioTrack].nPid;

	pstMediaPlay->nActivatedVideoTrackNum = pstVideoCompTable->nCurrIndex = cso_probeinfo->nCurrentVideoTrack;
	pstMediaPlay->ulActivatedVideoCodecId  = pstVideoCompTable->astCompArray[cso_probeinfo->nCurrentVideoTrack].eCodec;
	pstMediaPlay->ulActivatedVideoStreamId = pstVideoCompTable->astCompArray[cso_probeinfo->nCurrentVideoTrack].nPid;

	HxLOG_Error ("CSO's CurrentAudioTrack [ %d ], pstAudioCompTable->astCompArray[cso_probeinfo->nCurrentAudioTrack].nPid [ %d ] \n", cso_probeinfo->nCurrentAudioTrack, pstAudioCompTable->astCompArray[cso_probeinfo->nCurrentAudioTrack].nPid);
	HxLOG_Error ("CSO's CurrentVideoTrack [ %d ] \n", cso_probeinfo->nCurrentVideoTrack);
#endif

	/*this is Audio Only Contents...*/
	if ( (cso_probeinfo->ucTotalAudioTrack > 0) && (cso_probeinfo->ucTotalVideoTrack == 0 ) )
	{
		/* 오디오 only 일 때 Audio Probe 결과로만 판단하여 재생하기 위함. */
		bVideoPlayable = TRUE;
		pstMediaPlay->bOnlyAudioContents = TRUE;
	}

	/*this is Video Only Contents...*/
	if ( (cso_probeinfo->ucTotalAudioTrack == 0 ) && (cso_probeinfo->ucTotalVideoTrack > 0 ) )
	{
		/* Video only 일 때 Video Probe 결과로만 판단하여 재생하기 위함. */
		bAudioPlayable = TRUE;
	}

	if( bSubtitlePlayable )
	{
		HERROR			 hErr;
		HxLANG_Id_e		eSubTitleLangId			= eLangID_MAX;
		HxLANG_Id_e		eTrackLangId1			= eLangID_MAX;

		pstSubtitleCompTable->nCurrIndex = SUBTITLE_OFF; // 우선 Subtitle OFF로 해놓고
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0, (HUINT32 *)&eSubTitleLangId, 0);
		if ((hErr == ERR_OK) && (eSubTitleLangId != eLangID_MAX))
		{
			for( tracks = 0; tracks < pstSubtitleCompTable->nArraySize; tracks++ )
			{
				// Check the language number:
				hErr = MWC_UTIL_GetLangIDBy639Code (pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.szLangCode, &eTrackLangId1);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("[%s](L:%d) Fail To get track subtitle language.... \n", __FUNCTION__, __LINE__ );
					eTrackLangId1 = eLangID_MAX;
				}

				if((eSubTitleLangId==eTrackLangId1))
				{
					HxLOG_Debug ("[%s](L:%d) Current Subtitle Index: %d \n", __FUNCTION__, __LINE__, tracks);
					pstSubtitleCompTable->nCurrIndex=tracks;
					break;
				}
			}
		}
		else
		{
			HxLOG_Error ("[%s](L:%d) Fail To get system subtitle language.... \n", __FUNCTION__, __LINE__ );
		}

		/* APP으로 Event를 보내 DVB-Subtitle이 있음을 알림. . */
		om_media_play_CbOnComponentSubtitleChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, pstSubtitleCompTable );
	}

	if( bAudioPlayable )
	{
		HERROR			hErr;
		HxLANG_Id_e 	eAudioLangId 		= eLangID_MAX;
		HxLANG_Id_e 	eTrackLangId1		= eLangID_MAX;

		pstMediaPlay->nActivatedAudioTrackNum=pstAudioCompTable->nCurrIndex = cso_probeinfo->nCurrentAudioTrack; // cso 에서 선택된 Audio track 재생
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0, (HUINT32 *)&eAudioLangId, 0);
		if ((hErr == ERR_OK) && (eAudioLangId != eLangID_MAX))
		{
			for( tracks = 0; tracks < pstAudioCompTable->nArraySize; tracks++ )
			{
				// Check the language number:
				hErr = MWC_UTIL_GetLangIDBy639Code (pstAudioCompTable->astCompArray[tracks].astLangInfo[0].szLangCode, &eTrackLangId1);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("[%s](L:%d) Fail To get track audio language.... \n", __FUNCTION__, __LINE__ );
					eTrackLangId1 = eLangID_MAX;
				}

				if((eAudioLangId==eTrackLangId1))
				{
					HxLOG_Debug ("[%s](L:%d) Current Audio Index: %d \n", __FUNCTION__, __LINE__, tracks);
					pstMediaPlay->nActivatedAudioTrackNum = pstAudioCompTable->nCurrIndex = tracks;
					/* 여기서는 코덱 아이디, 스트림아이디 도 변경해 준다. cso_start() 에서 바로 이용 가능하도록 */
					pstMediaPlay->ulActivatedAudioCodecId  = pstAudioCompTable->astCompArray[tracks].eCodec;
					pstMediaPlay->ulActivatedAudioStreamId = pstAudioCompTable->astCompArray[tracks].nPid;
					break;
				}
			}
		}
		else
		{
			HxLOG_Error ("[%s](L:%d) Fail To get audio subtitle language.... %d %x \n", __FUNCTION__, __LINE__, hErr, eAudioLangId );
		}
	}

	if ( bVideoPlayable && bAudioPlayable)
	{
		/*
		 * Media는 ActionTaken Message 대신 ProbeFinished 사용 : 왜 여기에 필요한지 문의 필요...
		 */
		pstContext->bActionTaken	=	TRUE;

		/*
		 * ProbeFinished 시 Audio,Video Component Update 해야함.
		 */
		HxLOG_Debug ("[%s](L:%d) Play Video Codec = %08X, Stream ID = %d, Audio Codec = %08X, Stream ID = %d \n", __FUNCTION__, __LINE__, cso_probeinfo->video_codec, cso_probeinfo->video_stream_id, cso_probeinfo->audio_codec, cso_probeinfo->audio_stream_id );
		om_media_play_CbOnComponentVideoChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, pstVideoCompTable);
		om_media_play_CbOnComponentAudioChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, pstAudioCompTable);
	}
	else
	{
		HxLOG_Error ("[%s](L:%d) there is no Playable Audio, Video Codecs.... \n", __FUNCTION__, __LINE__ );
		return ERR_FAIL;
	}
	pstMediaPlay->ulBitrate = cso_probeinfo->max_bitrate;
	pstMediaPlay->ullDuration = cso_probeinfo->duration;

	if ( cso_probeinfo->ullStartTime != 0 )
	{
		pstMediaPlay->ullFirstPts = cso_probeinfo->ullStartTime;
	}
	else
	{
		pstMediaPlay->ullFirstPts = 0;
	}

	pstMediaPlay->bSupport_seek = cso_probeinfo->support_seek;
	if ( !pstMediaPlay->bSupport_seek )
	{
		HxLOG_Error("[%s](L:%d) This Content cann't be seeked.\n", __FUNCTION__, __LINE__);
	}
	pstMediaPlay->bSupport_trick = cso_probeinfo->support_trick;
	if ( !pstMediaPlay->bSupport_trick )
	{
		HxLOG_Error("[%s](L:%d) This Content cann't be tricked.\n", __FUNCTION__, __LINE__);
	}
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 15))
	pstMediaPlay->bAdaptiveStreaming = cso_probeinfo->is_adaptivestreaming;
#endif
	pstMediaPlay->stCsoStatus.eStatus = eCSO_PROBED;

	return ERR_OK;
}


/**
* @b Function @b Description <br>
* this function makes Probing --> probed or probe failed
*
* @param[in] omMediaPlay_Context_t   : media pb context
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/
HERROR om_media_play_mediapb_cso_GetProbeInfo(omMediaPlay_Context_t *pstContext)
{
	CSO_ProbeInfo_t 	*cso_probeinfo = NULL;
	CSO_ERR 		err = CSO_ERR_FAIL;
	HERROR hErr = ERR_FAIL;

	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	cso_probeinfo = OxMGR_Malloc(sizeof(CSO_ProbeInfo_t));
	if ( !cso_probeinfo )
	{
		HxLOG_Error("[%s](L:%d) Error - Alloc failed!\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	HxSTD_MemSet(cso_probeinfo, 0, sizeof(CSO_ProbeInfo_t));

	err = MEDIAPB_IPC_CSO_ProbeInfo(pstMediaPlay->ulCsoSessionId, cso_probeinfo);
	if ( err != CSO_ERR_OK)
	{
		pstMediaPlay->ulActivatedAudioCodecId = eDxAUDIO_CODEC_UNKNOWN;
		pstMediaPlay->ulActivatedVideoCodecId = eDxVIDEO_CODEC_UNKNOWN;
		OxMGR_Free(cso_probeinfo);
		return ERR_FAIL;
	}

	hErr = om_media_play_mediapb_cso_StoreProbeInfo( pstContext, cso_probeinfo );
	OxMGR_Free(cso_probeinfo);
	return hErr;
}

/**
* @b Function @b Description <br>
* this function makes Playing --> eCSO_JUMP_TRICK_REQUESTED
*
* @param[in] omMediaPlay_Context_t   : media pb context
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/
STATIC HERROR om_media_play_mediapb_cso_SeekTrickRequest(omMediaPlay_Context_t *pstContext)
{
	CSO_ERR 		err;
	HERROR			hErr;
	CSO_CTRL_PARAM_t 	tParam;
	HUINT64 			ullPlayPosition;

	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext) { return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));

	/* Position from MEDIA */
	hErr = SVC_CSO_GetPlayPosition(pstMediaPlay->pMediaPlayer, &ullPlayPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error :\n");
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
		return ERR_FAIL;
	}

	if ( ullPlayPosition == 0)
	{
#if defined(CONFIG_TIMESTAMP_64)
		ullPlayPosition = pstMediaPlay->stCsoStatus.ullPlayingTime;
#else
		ullPlayPosition = pstMediaPlay->stCsoStatus.uiPlayingTime;
#endif

	}

#if 0	/* SVC_CSO does this */
	if (pstContext->ullStartTime != 0 )
	{
		HxLOG_Debug("[%s](L:%d) StartTime = %lld \n", __FUNCTION__, __LINE__, pstContext->ullStartTime);
		pstContext->stCsoStatus.ullRequestPlayTime = ullPlayPosition - pstContext->ullStartTime;
		if ( pstContext->stCsoStatus.ullRequestPlayTime < 0 )
		{
			pstContext->stCsoStatus.ullRequestPlayTime = 0;
		}
	}
	else
#endif
	{
#if defined(CONFIG_TIMESTAMP_64)		
		pstMediaPlay->stCsoStatus.ullRequestPlayTime = ullPlayPosition;
#else
		pstMediaPlay->stCsoStatus.uiRequestPlayTime = ullPlayPosition;	
#endif
	}

#if defined(CONFIG_TIMESTAMP_64)
	tParam.ullPosition = pstMediaPlay->stCsoStatus.ullRequestPlayTime;
#else
	tParam.ullPosition = pstMediaPlay->stCsoStatus.uiRequestPlayTime;
#endif
	tParam.nTrick = pstMediaPlay->stCsoStatus.uiRequestPlaySpeed;

	err = MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_TRICK, &tParam);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
		return ERR_FAIL;
	}

	return ERR_OK;
}

/**
* @b Function @b Description <br>
* this function makes no state change.
*
* @param[in] omMediaPlay_Context_t   : media pb context
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/
STATIC HERROR om_media_play_mediapb_cso_SeekTrickCancel(omMediaPlay_Context_t *pstContext)
{
	CSO_ERR 		err;
	HERROR			hErr;
	CSO_CTRL_PARAM_t	tParam;
	HUINT64 			ullPlayPosition;

	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));

	/* Position from MEDIA */
	hErr = SVC_CSO_GetPlayPosition(pstMediaPlay->pMediaPlayer, &ullPlayPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error :\n");
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
		return ERR_FAIL;
	}

	if ( ullPlayPosition == 0)
	{
#if defined(CONFIG_TIMESTAMP_64)
		ullPlayPosition = pstMediaPlay->stCsoStatus.ullPlayingTime;
#else
		ullPlayPosition = pstMediaPlay->stCsoStatus.uiPlayingTime;
#endif
	}

#if 0 /* SVC_CSO does this */
	if (pstContext->ullStartTime != 0 )
	{
		HxLOG_Debug("[%s](L:%d) StartTime = %lld \n", __FUNCTION__, __LINE__, pstContext->ullStartTime);
		pstContext->stCsoStatus.ullRequestPlayTime = ullPlayPosition - pstContext->ullStartTime;
		if ( pstContext->stCsoStatus.ullRequestPlayTime < 0 )
		{
			pstContext->stCsoStatus.ullRequestPlayTime = 0;
		}
	}
	else
#endif
	{
		if ( pstMediaPlay->bBofReached )
		{
#if defined(CONFIG_TIMESTAMP_64)
			pstMediaPlay->stCsoStatus.ullRequestPlayTime = 0;
#else
			pstMediaPlay->stCsoStatus.uiRequestPlayTime = 0;
#endif
			pstMediaPlay->bBofReached = FALSE;
		}
		else
		{
#if defined(CONFIG_TIMESTAMP_64)
			pstMediaPlay->stCsoStatus.ullRequestPlayTime = ullPlayPosition;
#else
			pstMediaPlay->stCsoStatus.uiRequestPlayTime = ullPlayPosition;
#endif
		}
	}

#if defined(CONFIG_TIMESTAMP_64)
	tParam.ullPosition = pstMediaPlay->stCsoStatus.ullRequestPlayTime;
#else
	tParam.ullPosition = pstMediaPlay->stCsoStatus.uiRequestPlayTime;
#endif
	tParam.nTrick = 100;

	err = MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_TRICK, &tParam);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
		return ERR_FAIL;
	}

	return ERR_OK;
}

/**
 * * @b Function @b Description <br>
 * * this function makes no state change.
 * *
 * * @param[in] omMediaPlay_Context_t   : media pb context
 * *
 * * @return  #  returns ERR_OK or ERR_FAIL
 * *
 * * @attention Just Test!
 * */
#ifdef HLS_DISCONTINUITY
STATIC HERROR om_media_play_mediapb_cso_Reset(omMediaPlay_Context_t *pstContext)
{
	CSO_ERR 		err;
	HERROR			hErr;
	CSO_CTRL_PARAM_t	tParam;
	//HUINT64 			ullPlayPosition;
	HUINT32 				ulActionId;

	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	ulActionId = pstContext->ulActionId;

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));

	#if 1
	if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_STOPPED )
	{
			HxLOG_Error ("CSO Already Stopped!!!!!!!!!!!!\n");
			return ERR_OK;
	}
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 8))
	err = MEDIAPB_IPC_CSO_Stop(pstMediaPlay->ulCsoSessionId, -1);
#else
	err = MEDIAPB_IPC_CSO_Stop(pstMediaPlay->ulCsoSessionId, -1);
#endif
	if ( err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Stop %u, %d\n", __FUNCTION__, __LINE__,pstMediaPlay->ulCsoSessionId, pstMediaPlay->stCsoStatus.eStatus);
	}


	/* make them default */
	if ( pstMediaPlay->nUnderflowThreshold > 0 )
	{
			pstMediaPlay->nUnderflowThreshold = 0;
	}
	if ( pstMediaPlay->bUnderflowAutoMode )
	{
			pstMediaPlay->bUnderflowAutoMode = FALSE;
	}

/************************
 * *		Free Slot
 * ************************/
	hErr = MGR_MPlayer_Reset(pstMediaPlay->pMediaPlayer);
	if ( ERR_OK != hErr )
	{
			HxLOG_Error ("MGR_MPlayer_Stop() Failed.\n");
	}

	hErr = SVC_CSO_FreePlaypumpIndex(pstMediaPlay->pMediaPlayer, pstMediaPlay->ulPumpId);
	if(hErr != ERR_OK)
	{
			HxLOG_Error("SVC_CSO_FreePlaypumpIndex failed!\n");
	}

	MEDIAPB_IPC_CSO_Reset(pstMediaPlay->ulCsoSessionId);

	err = MEDIAPB_IPC_CSO_Probe(pstMediaPlay->ulCsoSessionId);
	if ( err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Probe\n", __FUNCTION__, __LINE__);
		if(!pstMediaPlay->bExtOpened)
		{
			CSO_Settings_t *pSettings = NULL;
			if(pstMediaPlay->pCsoSettings != NULL)
			{
				pSettings = pstMediaPlay->pCsoSettings;
				(void)MEDIAPB_IPC_CSO_IPCMSG_MemFree_Settings(*pSettings);
				OxMGR_Free( pstMediaPlay->pCsoSettings );
				pstMediaPlay->pCsoSettings = NULL;
 			}
			err = MEDIAPB_IPC_CSO_Close( pstMediaPlay->ulCsoSessionId );
			if ( err != CSO_ERR_OK)
			{
				HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Close\n", __FUNCTION__, __LINE__);
			}
		}
		pstMediaPlay->stCsoStatus.eStatus = eCSO_STOPPED;
	}
	pstMediaPlay->stCsoStatus.eStatus = eCSO_PROBING;


	pstMediaPlay->stCsoStatus.bUnderrun = FALSE;
	pstMediaPlay->stCsoStatus.uiPlayingSpeed = 100;
	pstMediaPlay->stCsoStatus.uiDecoderSpeed = 100;
	pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = 100;
	pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
	om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->stCsoStatus.uiPlayingSpeed, 0, 0);


	#endif

	HxLOG_Error("[%s][%d] Call MEDIAPB_IPC_CSO_Restart! DONE \n", __FUNCTION__,__LINE__);

	return ERR_OK;
}
#endif
#ifdef AV_TRACK_CHANGE
/*	코드 리뷰 후 개선사항
*	- 주요 내용
*		1. 공통 구조체를 사용. 중복되는 구조체는 사용하지 않는다.
*		2. 이미 사용 중인 변수의 사용법을 이용해 쓸모없는 변수 사용 줄임.
*
*	- 역할 : 넘겨받은 video track 의 번호를 cso 로 전달.
*/
STATIC HERROR om_media_play_mediapb_cso_SetVideoTrack (HUINT32 ulActionId, HINT32 nVideoTrackNum)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t			*pstMediaPlay	=	NULL;
	OxMediaPlay_VideoCompTable_t 	*pstVideoCompTable	= NULL;
	CSO_CTRL_PARAM_t				tParam;
	CSO_ERR 						err = CSO_ERR_OK;
	HERROR							hErr;
//	SvcCso_VideoTrack_t 			VideoTrack;
//	HUINT32							i = 0;
	HUINT64 						pullPosition_ms = 0;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
	pstVideoCompTable =	&pstContext->stCache.stVideoCompTable;

	s_stMediaPlayerCache[CalcActionId(ulActionId)].nVideoTrack = nVideoTrackNum;
	s_stMediaPlayerCache[CalcActionId(ulActionId)].bVideoTrackSelected = TRUE;

	if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType != eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
	{
		HxLOG_Error("Can't Support MediaPlayerType : %d\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		return ERR_FAIL;
	}

	/* valid check for selected VIDEO track */
	if(pstVideoCompTable->nArraySize <= nVideoTrackNum)
	{
		HxLOG_Error ("This video track doesn't exist\n");
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Error ("Selected video track eVideoCodec        [ %d ] \n", pstVideoCompTable->astCompArray[nVideoTrackNum].eCodec);
		HxLOG_Error ("Selected video track VideoStreamId      [ %d ] \n", pstVideoCompTable->astCompArray[nVideoTrackNum].nPid);
	}
	if(pstMediaPlay->nActivatedVideoTrackNum == nVideoTrackNum)
	{
		HxLOG_Error ("It's same video name, so send COMPONENT_SELECTED event\n");
		om_media_play_mediapb_SetVideoHide(ulActionId, FALSE);
		return ERR_OK;
	}
	if ( !pstMediaPlay->bSupport_seek )
	{
		HxLOG_Error ("Failed to Seek!!!! \n");
		return ERR_FAIL;
	}
	if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_SEEK_REQUESTED )
	{
		HxLOG_Error ("now is seeking. so fail.\n");
		return ERR_FAIL;
	}

	om_media_play_mediapb_cso_EnterSection(pstContext);

	/* 	NEED_SEEK event 시 CurPosition 값을 RequestPlayTime 에 저장.
	* 	CurPosition 이 0 이고 이전에 저장한 값(ullRequestPlayTime)이 0 이 아니면.
	*	svc_cso 가 준비가 안된 상태이므로 return
	*/
	hErr = SVC_CSO_GetPlayPosition(pstMediaPlay->pMediaPlayer, &pullPosition_ms);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error : GetPlayPosition\n");
	}
#if defined(CONFIG_TIMESTAMP_64)
	if ( ( pullPosition_ms == 0 ) && ( pstMediaPlay->stCsoStatus.ullRequestPlayTime != 0  ) )
#else
	if ( ( pullPosition_ms == 0 ) && ( pstMediaPlay->stCsoStatus.uiRequestPlayTime != 0  ) )
#endif
	{
		HxLOG_Error ("Cur Position is 0(zero).\n");
		om_media_play_mediapb_cso_ExitSection(pstContext);
		return ERR_FAIL;
	}
	/* Status Initalization */
	pstMediaPlay->bBofReached = FALSE;
	pstMediaPlay->bEofReached = FALSE;
	pstMediaPlay->bCsoEofReached = FALSE;
	pstMediaPlay->ulCsoEofReachedTime = 0;
	pstMediaPlay->ullCurPosition = pullPosition_ms;
	HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));

	tParam.ulParam1 = nVideoTrackNum;

	err = MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_SELECT_VIDEO_TRACK, &tParam);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
		om_media_play_mediapb_cso_ExitSection(pstContext);
		return ERR_FAIL;
	}
	pstMediaPlay->nActivatedVideoTrackNum = nVideoTrackNum;

#if 0 // Debug; view comptalbe
	HxLOG_Error ("pstMediaPlay->nActivatedVideoTrackNum    [ %d ]\n", pstMediaPlay->nActivatedVideoTrackNum);
	for(i = 0; i < pstVideoCompTable->nArraySize; i++)
	{
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].aucVideoName   [ %s ] \n", i, pstVideoCompTable->astCompArray[i].aucVideoName);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].eCodec         [ %d ] \n", i, pstVideoCompTable->astCompArray[i].eCodec);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].nPid           [ %d ] \n", i, pstVideoCompTable->astCompArray[i].nPid);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].aucStreamName  [ %s ] \n", i, pstVideoCompTable->astCompArray[i].aucStreamName);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].nComponentType [ %d ] \n", i, pstVideoCompTable->astCompArray[i].nComponentType);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].nStreamType    [ %d ] \n", i, pstVideoCompTable->astCompArray[i].nStreamType);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].nHdVideo       [ %d ] \n", i, pstVideoCompTable->astCompArray[i].nHdVideo);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].nCasFound      [ %d ] \n", i, pstVideoCompTable->astCompArray[i].nCasFound);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].eCodec         [ %d ] \n", i, pstVideoCompTable->astCompArray[i].eCodec);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].eCompType      [ %d ] \n", i, pstVideoCompTable->astCompArray[i].eCompType);
		HxLOG_Error ("pstVideoCompTable->astCompArray[%lu].nComponentTag  [ %d ] \n", i, pstVideoCompTable->astCompArray[i].nComponentTag);
	}
#endif
	om_media_play_mediapb_cso_ExitSection(pstContext);

	return ERR_OK;
}

/*
*	- 역할 : 넘겨받은 audio track 의 번호를 cso 로 전달.
*/
STATIC HERROR om_media_play_mediapb_cso_SetAudioTrack (HUINT32 ulActionId, HINT32 nAudioTrackNum)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t			*pstMediaPlay	=	NULL;
	OxMediaPlay_AudioCompTable_t 	*pstAudioCompTable	= NULL;
	OxMediaPlay_VideoCompTable_t 	*pstVideoCompTable	= NULL;
	HERROR							hErr;
//	SvcCso_AudioTrack_t				AudioTrack;
//	HUINT32							i = 0;
	CSO_CTRL_PARAM_t				tParam;
	CSO_ERR 						err = CSO_ERR_OK;
	HUINT64 						pullPosition_ms = 0;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
	pstVideoCompTable =	&pstContext->stCache.stVideoCompTable;
	pstAudioCompTable = &pstContext->stCache.stAudioCompTable;

	s_stMediaPlayerCache[CalcActionId(ulActionId)].nAudioTrack = nAudioTrackNum;
	s_stMediaPlayerCache[CalcActionId(ulActionId)].bAudioTrackSelected = TRUE;

	if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType != eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
	{
		HxLOG_Error("Can't Support MediaPlayerType : %d\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		return ERR_FAIL;
	}

	/* valid check for selected AUDIO track */
	if(pstAudioCompTable->nArraySize <= nAudioTrackNum)
	{
		HxLOG_Error ("This audio track doesn't exist\n");
		return ERR_FAIL;
	}
	else
	{
		HxLOG_Error ("Selected audio track eAudioCodec        [ %d ] \n", pstAudioCompTable->astCompArray[nAudioTrackNum].eCodec);
		HxLOG_Error ("Selected audio track AudioStreamId      [ %d ] \n", pstAudioCompTable->astCompArray[nAudioTrackNum].nPid);
	}
	if(pstMediaPlay->nActivatedAudioTrackNum == nAudioTrackNum)
	{
		HxLOG_Error ("It's same audio name, so send COMPONENT_SELECTED event\n");
		om_media_play_mediapb_SetAudioHide(ulActionId, FALSE);
		return ERR_OK;
	}
	if ( !pstMediaPlay->bSupport_seek )
	{
		HxLOG_Error ("Failed to Seek!!!! \n");
		return ERR_FAIL;
	}
	if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_SEEK_REQUESTED )
	{
		HxLOG_Error ("now is seeking. so fail.\n");
		return ERR_FAIL;
	}

	om_media_play_mediapb_cso_EnterSection(pstContext);

	/* 	NEED_SEEK event 시 CurPosition 값을 RequestPlayTime 에 저장.
	* 	CurPosition 이 0 이고 이전에 저장한 값(ullRequestPlayTime)이 0 이 아니면.
	*	svc_cso 가 준비가 안된 상태이므로 return
	*/
	hErr = SVC_CSO_GetPlayPosition(pstMediaPlay->pMediaPlayer, &pullPosition_ms);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error : GetPlayPosition\n");
	}
#if defined(CONFIG_TIMESTAMP_64)	
	if ( ( pullPosition_ms == 0 ) && ( pstMediaPlay->stCsoStatus.ullRequestPlayTime != 0  ) )
#else
	if ( ( pullPosition_ms == 0 ) && ( pstMediaPlay->stCsoStatus.uiRequestPlayTime != 0  ) )
#endif
	{
		HxLOG_Error ("Cur Position is 0(zero).\n");
		om_media_play_mediapb_cso_ExitSection(pstContext);
		return ERR_FAIL;
	}
	/* Status Initalization */
	pstMediaPlay->bBofReached = FALSE;
	pstMediaPlay->bEofReached = FALSE;
	pstMediaPlay->bCsoEofReached = FALSE;
	pstMediaPlay->ulCsoEofReachedTime = 0;
	pstMediaPlay->ullCurPosition = pullPosition_ms;
	HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));

	tParam.ulParam1 = nAudioTrackNum;

	err = MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_SELECT_AUDIO_TRACK, &tParam);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
		om_media_play_mediapb_cso_ExitSection(pstContext);
		return ERR_FAIL;
	}
	pstMediaPlay->nActivatedAudioTrackNum = nAudioTrackNum;

#if 0 // Debug; View Comptable
	HxLOG_Error ("pstMediaPlay->nActivatedAudioTrackNum  [ %d ]\n", pstMediaPlay->nActivatedAudioTrackNum);
	for(i = 0; i < pstAudioCompTable->nArraySize; i++)
	{
		HxLOG_Error ("pstAudioCompTable->astCompArray[%lu].aucAudioName   [ %s ] \n", i, pstAudioCompTable->astCompArray[i].aucAudioName);
		HxLOG_Error ("pstAudioCompTable->astCompArray[%lu].eCodec         [ %d ] \n", i, pstAudioCompTable->astCompArray[i].eCodec);
		HxLOG_Error ("pstAudioCompTable->astCompArray[%lu].nPid           [ %d ] \n", i, pstAudioCompTable->astCompArray[i].nPid);
		HxLOG_Error ("pstAudioCompTable->astCompArray[%lu].aucStreamName  [ %s ] \n", i, pstAudioCompTable->astCompArray[i].aucStreamName);
		HxLOG_Error ("pstAudioCompTable->astCompArray[%lu].eCodec         [ %d ] \n", i, pstAudioCompTable->astCompArray[i].eCodec);
		HxLOG_Error ("pstAudioCompTable->astCompArray[%lu].eCompType      [ %d ] \n", i, pstAudioCompTable->astCompArray[i].eCompType);
		HxLOG_Error ("pstAudioCompTable->astCompArray[%lu].nComponentTag  [ %d ] \n", i, pstAudioCompTable->astCompArray[i].nComponentTag);
	}
#endif
	om_media_play_mediapb_cso_ExitSection(pstContext);


	return ERR_OK;
}
#endif

static void CSO_Error_Callback(HUINT32 ulSessionId, CSO_ERROR_CATEGORY_e eCategory, HINT64 /*CSO_ERROR_REASON_e*/ llReason)
{
	omMediaPlay_Context_t		*pstContext = NULL;
	omMPMediaPlayContext_t     	*pstMediaPlay = NULL;

	HxLOG_Error("[%s](L:%d) Callback CATEGORY(%d), REASON(%lld)\n", __FUNCTION__, __LINE__, eCategory, llReason);

	pstContext = om_media_play_mediapb_cso_getContextWithSessionId (ulSessionId);
	if ( !pstContext )
	{
		HxLOG_Error("[%s](L:%d) pstContext is NULL\n", __FUNCTION__, __LINE__);
		return ;
	}
	if (NULL == pstContext->pExtension)		{ return ; }

	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);
#if 0 // for onDRMRightsError.
/*
onDRMRightsError need data about ContentID, DRMSystemID, RightsIssuerURL and errorState
but CSO_Error_Callback can't not noti this value.
so it can change.
*/
	if( eCategory == CSO_EVENT_ADVANCED_ERROR_DRM_PLAYREADY)
	{
		HxLOG_Error("[%s](L:%d) Callback CATEGORY(%d), REASON(%lld)\n", __FUNCTION__, __LINE__, eCategory, llReason);
		if ( !pstContext )
		{
			HxLOG_Error("\n\n\n[%s](L:%d) Ignore this Session ID ( %d ) Callback \n\n\n", __FUNCTION__, __LINE__, ulSessionId );
			return;
		}
		//OxMediaPlay_DrmRightsError_t stDrmRightsError;
		//memcpy(stDrmRightsError.szContentID, "TEST! CONTENT ID!!",strlen("TEST! CONTENT ID!!")+1);
		//memcpy(stDrmRightsError.szDRMSystemID, "TEST! DRMSYSTEM ID!!",strlen("TEST! DRMSYSTEM ID!!")+1);
		//memcpy(stDrmRightsError.szRightsIssuerURL, "TEST! RIGHTSISSUERURL!!",strlen("TEST! RIGHTSISSUERURL!!")+1);
		//stDrmRightsError.eErrorState = eDrmError_InvalidLicense;
		//om_media_play_mediapb_EvtMediaPBDrmBlock(pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), &stDrmRightsError);
	}
#endif
#if !defined (CONFIG_OP_SES)
	if ( eCategory == CSO_EVENT_ADVANCED_ERROR_DATA )
	{
		if ( llReason == CSO_ERROR_R_DATA_MALFORMED_MHEG_X_KEY )
		{
			pstMediaPlay->eErrorReason = eOxMP_ERROR_REASON_X_KEY;
			HxLOG_Error("[%s](L:%d) DATA ERROR - X Key Error\n", __FUNCTION__, __LINE__);
		}
		else if ( llReason == CSO_ERROR_R_DATA_MALFORMED_GENERIC )
		{
			pstMediaPlay->eErrorReason = eOxMP_ERROR_REASON_DATA_MALFORMED_GENERIC;
			HxLOG_Error("[%s](L:%d) DATA ERROR - Data malformed\n", __FUNCTION__, __LINE__);
		}
		else
		{
			HxLOG_Error("[%s](L:%d) Unhandled DATA ERROR - REASON(%lld)\n", __FUNCTION__, __LINE__, llReason);
		}
	}
	else if ( eCategory == CSO_EVENT_ADVANCED_ERROR_SERVER )
	{
		if ( llReason == CSO_ERROR_R_SERVER_HTTP_STATUS_NOT_FOUND || llReason == CSO_ERROR_R_SERVER_OUT_OF_WINDOW )
		{
			pstMediaPlay->eErrorReason = eOxMP_ERROR_REASON_SERVER_HTTP_STATUS_NOT_FOUND;
			HxLOG_Error("[%s](L:%d) SERVER ERROR - HTTP STATUS NOT FOUND (404)\n", __FUNCTION__, __LINE__);
		}
		else if ( llReason == CSO_ERROR_R_SERVER_HTTP_STATUS_INTERNAL_SERVER_ERROR )
		{
			pstMediaPlay->eErrorReason = eOxMP_ERROR_REASON_SERVER_HTTP_STATUS_SERVER_ERROR;
			HxLOG_Error("[%s](L:%d) SERVER ERROR - HTTP STATUS SERVER ERROR (500)\n", __FUNCTION__, __LINE__);
		}
		else
		{
			HxLOG_Error("[%s](L:%d) Unhandled SERVER ERROR - REASON(%lld)\n", __FUNCTION__, __LINE__, llReason);
		}
	}
	else if ( eCategory == CSO_EVENT_ADVANCED_ERROR_NETWORK )
	{
		if ( llReason == CSO_ERROR_R_NETWORK_UNREACHABLE )
		{
			pstMediaPlay->eErrorReason = eOxMP_ERROR_REASON_NETWORK_UNREACHABLE;
			HxLOG_Error("[%s](L:%d) NETWORK ERROR - Unreachable destination\n", __FUNCTION__, __LINE__);
		}
		else
		{
			HxLOG_Error("[%s](L:%d) Unhandled NETWORK ERROR - REASON(%lld)\n", __FUNCTION__, __LINE__, llReason);
		}
	}
	else
	{
		HxLOG_Error("[%s](L:%d) Unhandled ERROR(%d) - REASON(%lld)\n", __FUNCTION__, __LINE__, eCategory, llReason);
	}
#endif
	return;
}

static void CSO_Media_Callback(HUINT32 ulSessionId, HINT32 ev, HINT64 param)
{
	CSO_EVENT_e cso_event = (CSO_EVENT_e)ev;
	omMediaPlay_Context_t			*pstContext;
	CSO_ERR 				 err;
	HERROR					 hErr;
	CSO_CTRL_PARAM_t 		 tParam;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HUINT32					ulActionId;
	SvcCso_VideoTrack_t 	VideoTrack;
	SvcCso_AudioTrack_t 	AudioTrack;
	HINT32					nVideoTrackNum = 0;
	HINT32 					nAudioTrackNum = 0;
	HUINT32 				semaid = 0;
	OxMediaPlay_AudioCompTable_t *pstAudioCompTable	= NULL;
	OxMediaPlay_VideoCompTable_t *pstVideoCompTable	= NULL;

#if defined(CONFIG_PROD_YSR2000) && defined(MEDIAPB_RECOVER_TRICKMODE)// recover trick speed on seek complete.
	HBOOL bKeepStatus = FALSE;
#endif
	HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));

	om_media_play_mediapb_cso_PrintCSOEvent(ulSessionId, cso_event, param);

	pstContext = om_media_play_mediapb_cso_getContextWithSessionId (ulSessionId);

	if ( !pstContext )
	{
		HxLOG_Error("\n\n\n[%s](L:%d) Ignore this Session ID ( %d ) Callback \n\n\n", __FUNCTION__, __LINE__, ulSessionId );
		return;
	}

	if (NULL == pstContext->pExtension)
	{
		return;
	}

	ulActionId = pstContext->ulActionId;
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
	om_media_play_mediapb_cso_PrintProcEvent(pstContext, ev);

	switch ( cso_event )
	{
	case CSO_EVENT_PROBE_COMPLETE:
		/* 다른 Task에서 pstContext->pExtension = NULL로 만들면, lock이 잡힌 sema가 풀리지 않고,
		 * 빠져나가는 경우가 존재하여, 저장된sema id를 해재하도록 함. 4개의 Task에서 사용됨*/
		semaid = om_media_play_mediapb_cso_GetSemaphoreId(pstContext);
		if (semaid == 0 )
		{
			HxLOG_Error("[%s](L:%d) Error - semaphoreId is 0.\n", __FUNCTION__, __LINE__);
			return;
		}
		HxSEMT_Get(semaid);
		if(pstMediaPlay->stCsoStatus.eStatus == eCSO_PROBE_CANCELED)
		{
			HxLOG_Error("Probe complete event received. However, we send cancel request. Skip it.\n");
			HxSEMT_Release(semaid);
			return;
		}
		hErr = om_media_play_mediapb_cso_GetProbeInfo(pstContext);
		if (ERR_OK != hErr)
		{
			pstMediaPlay->eErrorReason = eOxMP_ERROR_REASON_NOT_SUPPORTED_FORMAT;
			HxLOG_Error ("om_media_play_mediapb_cso_GetProbeInfo failed \n");
			om_media_play_mediapb_EvtProbeFailed (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);

			if (pstMediaPlay->bStartRequested)
			{
				om_media_play_mediapb_EvtError (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			}
			pstMediaPlay->eErrorReason = eOxMP_ERROR_REASON_NONE;
			HxSEMT_Release(semaid);
			return;
		}
		/* Emit Duration to App */
		HxLOG_Print("\033[30;43m[%s : %d] duration : %d, bitrate : %d  \033[0m\n", __FUNCTION__, __LINE__, (HUINT32)pstMediaPlay->ullDuration, pstMediaPlay->ulBitrate);
		HxLOG_Error("%s(%d) [CRB] Receive Probe Complete from CSO. so send ProbeComplete event in MediaPB\n", __FUNCTION__, __LINE__);
		om_media_play_mediapb_EvtPlayInfo(pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), (HINT32)pstMediaPlay->ullSeekedTimeByCso, (HINT32)pstMediaPlay->ullDuration, pstMediaPlay->ulBitrate);
		om_media_play_mediapb_EvtBufferingCanplay (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
#if defined(CONFIG_MW_OUTPUT_HDAUDIO_SR_RESTRICT)
		if ( pstMediaPlay->bHdAudio )
		{
			DxHdmiAudioSR_e eContentSR = eDxHDMI_AUDIO_SR_Max;
			HBOOL bSupportSR = FALSE;
			/* Get Sample Rate of HD Audio Content*/
			om_media_play_mediapb_cso_getAudioSampleRate( &pstContext->stCache.stAudioCompTable, &eContentSR);

			HxLOG_Warning("-- SampleRate enum : (0x%0x)!!!!\n", eContentSR);
			/* checking for output Sampling Rate of hdmi */
			hErr = SVC_OUT_CheckHdmiAudioOutput(eDxAUDIO_CODEC_PCM, eContentSR,TRUE, &bSupportSR);
			if (ERR_OK == hErr && TRUE != bSupportSR)
			{
				/* Not Supported High Sample Rate. -> HDMI Mute */
				HxLOG_Warning("Try... HDMI Mute(High SampleRate:%d)!!!!\n", eContentSR);
				pstMediaPlay->bHdmiMute = TRUE;
				SVC_OUT_SetHdmiAudioMute( pstMediaPlay->bHdmiMute );
				om_media_play_mediapb_CbOnAudioOutputRestrict( (ulActionId - eActionId_VIEW_FIRST), pstContext->ulSessionId);
			}
			else
			{
				HxLOG_Warning("Try... HDMI Unmute(SampleRate:%d)\n", eContentSR);
				pstMediaPlay->bHdmiMute = FALSE;
				SVC_OUT_SetHdmiAudioMute( pstMediaPlay->bHdmiMute );
			}
		}
		else
		{
			if(pstMediaPlay->bHdmiMute)
			{
				pstMediaPlay->bHdmiMute = FALSE;
				SVC_OUT_SetHdmiAudioMute( pstMediaPlay->bHdmiMute );
			}
		}
#endif
		if ( pstMediaPlay->bStartRequested )
		{
			hErr = om_media_play_mediapb_cso_Start(pstContext);
			if (ERR_OK != hErr)
			{
				om_media_play_mediapb_EvtError (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING_FAILED;
				HxSEMT_Release(semaid);
				return;
			}
			pstContext->stCache.eState = eOxMP_PLAYSTATE_PLAYING;//pstContext->bPlayed = TRUE;
#if 0
			if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
			{
				om_media_play_mediapb_EvtPlaying (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			}
#endif
		}
		/* If internal seek has been occured, do not send position 0 */
		if(pstMediaPlay->ullSeekedTimeByCso != 0)
		{
			pstMediaPlay->bCheckPositionValid = TRUE;
		}
		HxSEMT_Release(semaid);

		break;
	case CSO_EVENT_PROBE_FAILURE:
		semaid = om_media_play_mediapb_cso_GetSemaphoreId(pstContext);
		if (semaid == 0 )
		{
			HxLOG_Error("[%s](L:%d) Error - semaphoreId is 0.\n", __FUNCTION__, __LINE__);
			return;
		}
		HxSEMT_Get(semaid);
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PROBE_FAILED;
		pstMediaPlay->ulActivatedAudioCodecId = eDxAUDIO_CODEC_UNKNOWN;
		pstMediaPlay->ulActivatedVideoCodecId = eDxVIDEO_CODEC_UNKNOWN;

		om_media_play_mediapb_EvtProbeFailed (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);

		if (pstMediaPlay->bStartRequested)
		{
			om_media_play_mediapb_EvtError (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		}
		pstMediaPlay->eErrorReason = eOxMP_ERROR_REASON_NONE;

		HxSEMT_Release(semaid);
		break;
	case CSO_EVENT_PROBE_CANCEL:
		HxLOG_Error ("Probe canceled!\n");
		break;
	case CSO_EVENT_BUFFERING_START:
		break;
	case CSO_EVENT_BUFFERING_ENOUGH:
#if 0
#if 1
		/* Check Auto Mode Function */
		if(pstMediaPlay->eUnderflowMode == eOM_CSO_UNDERFLOW_NONSTOP)
		{
			hErr = om_media_play_mediapb_ControlUnderrun(pstContext, FALSE);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s](L:%d) ControlUnderrun Fail\n", __FUNCTION__, __LINE__);
			}
		}
#else
		om_media_play_mediapb_EvtBufferingEnough (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		if ( (!pstMediaPlay->bUnderflowAutoMode) && (pstMediaPlay->stCsoStatus.uiRequestPlaySpeed != 0/*PAUSE*/))
		{
			om_media_play_mediapb_EvtPlaying (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);

			hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 100); /* 100 means Play 1X */
			if ( hErr != ERR_OK )
			{
				HxLOG_Error ("SVC_CSO_SetPlaySpeed() \n");
			}
			pstMediaPlay->stCsoStatus.uiPlayingSpeed = 100;
			pstMediaPlay->stCsoStatus.uiDecoderSpeed = 100;
		}
#endif
#endif
		break;
	case CSO_EVENT_BUFFERING_NOTENOUGH:
#if 0
#if 1
		if(pstMediaPlay->eUnderflowMode == eOM_CSO_UNDERFLOW_NONSTOP)
		{
			hErr = om_media_play_mediapb_ControlUnderrun(pstContext, TRUE);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s](L:%d) ControlUnderrun Fail\n", __FUNCTION__, __LINE__);
			}
		}
#else
		om_media_play_mediapb_EvtBufferingNotEnough (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		if ( !pstMediaPlay->bUnderflowAutoMode)
		{
			om_media_play_mediapb_EvtConnecting (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);

			hErr =SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 0); /* 0 means Pause */
			if ( hErr != ERR_OK )
			{
				HxLOG_Error ("SVC_CSO_SetPlaySpeed() \n");
			}
			pstMediaPlay->stCsoStatus.uiPlayingSpeed = 0;
			pstMediaPlay->stCsoStatus.uiDecoderSpeed = 0;
		}
#endif
#endif
		break;
	case CSO_EVENT_BUFFERING_COMPLETE:
		om_media_play_mediapb_EvtBufferingFinished (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		break;
	case CSO_EVENT_BUFFERING_FAILURE:
		HxLOG_Error ("Got CSO_EVENT_BUFFERING_FAILURE \n");
		om_media_play_mediapb_EvtError (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		break;
	case CSO_EVENT_MEDIA_RESUME:
		break;
	case CSO_EVENT_MEDIA_PAUSE:
		break;
	case CSO_EVENT_MEDIA_BEGINOFSTREAM:
		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICKING)
		{
			om_media_play_mediapb_EvtBeginOfStream (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			pstMediaPlay->bBofReached = TRUE;
		}
		break;
	case CSO_EVENT_MEDIA_DURATION_CHANGED:
		semaid = om_media_play_mediapb_cso_GetSemaphoreId(pstContext);
		if (semaid == 0 )
		{
			HxLOG_Error("[%s](L:%d) Error - semaphoreId is 0.\n", __FUNCTION__, __LINE__);
			return ;
		}
		HxSEMT_Get(semaid);
		if ( param != pstMediaPlay->ullDuration)
		{
			HxLOG_Error("Duration updated from CSO (%llu[0x%X] ---> %llu[0x%X])\n", pstMediaPlay->ullDuration, pstMediaPlay->ullDuration, param, param);
			pstMediaPlay->ullDuration = param;
			(void)SVC_CSO_SetDuration(pstMediaPlay->pMediaPlayer, pstMediaPlay->ullDuration);
			om_media_play_mediapb_EvtPlayDurationChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->ullDuration);
		}
		HxSEMT_Release(semaid);
		break;
	case CSO_EVENT_MEDIA_UPDATE_BASE_POSITION:
		pstMediaPlay->ullBasePosition = param;
		//HxLOG_Error("parkjh4 > Update base position : %llu\n", pstMediaPlay->ullBasePosition);
		break;
	case CSO_EVENT_MEDIA_ENDOFSTREAM:
#ifdef HLS_DISCONTINUITY
		if ( param != pstMediaPlay->ullDuration)
		{
			HxLOG_Error("Got EOF with New Duration from CSO\n");
			if( pstMediaPlay->bNeedRestart != TRUE )
			{
				pstMediaPlay->ullDuration = param;
				(void)SVC_CSO_SetDuration(pstMediaPlay->pMediaPlayer, pstMediaPlay->ullDuration);
				om_media_play_mediapb_EvtPlayDurationChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->ullDuration);
			}
		}

		pstMediaPlay->bCsoEofReached = TRUE;
		pstMediaPlay->ulCsoEofReachedTime = time(NULL);
		HxLOG_Error("Got EOF from CSO\n");

		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICKING)
		{
			if( pstMediaPlay->bNeedRestart != TRUE )
			{
				om_media_play_mediapb_EvtEndOfStream (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			}
		}
#if 1
		if(pstMediaPlay->bImmediatelyRestart == TRUE)
		{
			pstMediaPlay->bNeedRestart = FALSE;
			pstMediaPlay->bCsoEofReached = FALSE;
			pstMediaPlay->bImmediatelyRestart = FALSE;
			om_media_play_mediapb_cso_Reset( pstContext );
		}
#endif
		break;
#else
		if ( param != pstMediaPlay->ullDuration)
		{
			HxLOG_Error("Got EOF with New Duration from CSO\n");
			pstMediaPlay->ullDuration = param;
			(void)SVC_CSO_SetDuration(pstMediaPlay->pMediaPlayer, pstMediaPlay->ullDuration);
			om_media_play_mediapb_EvtPlayDurationChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->ullDuration);
		}

		pstMediaPlay->bCsoEofReached = TRUE;
		pstMediaPlay->ulCsoEofReachedTime = time(NULL);
		HxLOG_Error("Got EOF from CSO\n");

		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICKING)
		{
			om_media_play_mediapb_EvtEndOfStream (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		}
		break;
#endif
#ifdef AV_TRACK_CHANGE
	case CSO_EVENT_MEDIA_SELECTED_VIDEO_TRACK_COMPLETE:
	        HxLOG_Error("[%s:%d]CSO_EVENT_MEDIA_SELECTED_VIDEO_TRACK_COMPLETE \n",__FUNCTION__, __LINE__);

			pstVideoCompTable = &pstContext->stCache.stVideoCompTable;

			nVideoTrackNum = pstMediaPlay->nActivatedVideoTrackNum;
			if(nVideoTrackNum >= 0)
			{
				pstMediaPlay->ulActivatedVideoCodecId = pstVideoCompTable->astCompArray[nVideoTrackNum].eCodec;
				pstMediaPlay->ulActivatedVideoStreamId = pstVideoCompTable->astCompArray[nVideoTrackNum].nPid;
				VideoTrack.eVideoCodec = pstMediaPlay->ulActivatedVideoCodecId;
				VideoTrack.ulVideoStreamId = pstMediaPlay->ulActivatedVideoStreamId;
				HxLOG_Error ("VideoTrack.eVideoCodec        [ %d ] \n", VideoTrack.eVideoCodec);
				HxLOG_Error ("VideoTrack.ulVideoStreamId    [ %d ] \n", VideoTrack.ulVideoStreamId);
				hErr = SVC_CSO_SetActiveVideoTrack(pstMediaPlay->pMediaPlayer, &VideoTrack);
			}
			else
			{
				HxLOG_Error ("Video Track Stop		        [ %d ] \n", nVideoTrackNum);
				hErr = SVC_CSO_SetActiveVideoTrack(pstMediaPlay->pMediaPlayer, NULL);
			}
			if(hErr != ERR_OK)
			{
				HxLOG_Error("SVC_CSO_SetActiveVideoTrack failed!\n");
			}
		break;
	case CSO_EVENT_MEDIA_SELECTED_AUDIO_TRACK_COMPLETE:
	        HxLOG_Error("[%s:%d]CSO_EVENT_MEDIA_SELECTED_AUDIO_TRACK_COMPLETE \n",__FUNCTION__, __LINE__);

			pstAudioCompTable = &pstContext->stCache.stAudioCompTable;

			nAudioTrackNum = pstMediaPlay->nActivatedAudioTrackNum;
			if(nAudioTrackNum >= 0)
			{
				pstMediaPlay->ulActivatedAudioCodecId = pstAudioCompTable->astCompArray[nAudioTrackNum].eCodec;
				pstMediaPlay->ulActivatedAudioStreamId = pstAudioCompTable->astCompArray[nAudioTrackNum].nPid;
				AudioTrack.eAudioCodec = pstMediaPlay->ulActivatedAudioCodecId;
				AudioTrack.ulAudioStreamId = pstMediaPlay->ulActivatedAudioStreamId;
				HxLOG_Error ("nAudioTrackNum		        [ %d ] \n", nAudioTrackNum);
				HxLOG_Error ("AudioTrack.eAudioCodec		[ %d ] \n", AudioTrack.eAudioCodec);
				HxLOG_Error ("AudioTrack.ulAudioStreamId	[ %d ] \n", AudioTrack.ulAudioStreamId);
				hErr = SVC_CSO_SetActiveAudioTrack(pstMediaPlay->pMediaPlayer, &AudioTrack);
			}
			else
			{
				HxLOG_Error ("Audio Track Stop		        [ %d ] \n", nAudioTrackNum);
				hErr = SVC_CSO_SetActiveAudioTrack(pstMediaPlay->pMediaPlayer, NULL);
			}
			if(hErr != ERR_OK)
			{
				HxLOG_Error("SVC_CSO_SetActiveAudioTrack failed!\n");
			}
		break;
	case CSO_EVENT_ERROR_SELECTED_FAIL:
		HxLOG_Error("CSO_EVENT_ERROR_SELECTED_FAIL\n");
		break;
	case CSO_EVENT_MEDIA_NEED_SEEK:
		HxLOG_Error("[%s:%d]CSO_EVENT_MEDIA_NEED_SEEK\n",__FUNCTION__, __LINE__);
		semaid = om_media_play_mediapb_cso_GetSemaphoreId(pstContext);
		if (semaid == 0 )
		{
			HxLOG_Error("[%s](L:%d) Error - semaphoreId is 0.\n", __FUNCTION__, __LINE__);
			return;
		}
		HxSEMT_Get(semaid);
#if defined(CONFIG_TIMESTAMP_64)
		pstMediaPlay->stCsoStatus.ullRequestPlayTime = pstMediaPlay->ullCurPosition;
#else
		pstMediaPlay->stCsoStatus.uiRequestPlayTime = pstMediaPlay->ullCurPosition;
#endif
		pstMediaPlay->stCsoStatus.eStatus = eCSO_SEEK_REQUESTED;
		tParam.ullPosition = pstMediaPlay->ullCurPosition;

		err = MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_SEEK, &tParam);
		if(err != CSO_ERR_OK)
		{
			HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
			pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
			HxSEMT_Release(semaid);
			return ;
		}
		HxSEMT_Release(semaid);
		break;
#endif
#ifdef HLS_DISCONTINUITY
	case CSO_EVENT_MEDIA_NEEDRESTART:
	    HxLOG_Error("[%s:%d]Receive NEEDRESTART. \n",__FUNCTION__, __LINE__);
		pstMediaPlay->bNeedRestart = TRUE;
		if ( param != 0 )
		{
			pstMediaPlay->bImmediatelyRestart = TRUE;
		}
		break;
#endif
	case CSO_EVENT_MEDIA_ERROROFSTREAM:
		om_media_play_mediapb_EvtError (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		break;
	case CSO_EVENT_MEDIA_SEEK_COMPLETE:
		semaid = om_media_play_mediapb_cso_GetSemaphoreId(pstContext);
		if (semaid == 0 )
		{
			HxLOG_Error("[%s](L:%d) Error - semaphoreId is 0.\n", __FUNCTION__, __LINE__);
			return;
		}
		HxSEMT_Get(semaid);

		if ( pstMediaPlay->stCsoStatus.eStatus < eCSO_PLAYING )
		{
			HxLOG_Error ("not playing status...\n");
			HxSEMT_Release(semaid);
			return;
		}
		/* After the Flush, this position value will be checked by GetPlayInfo. */

		pstMediaPlay->bCheckPositionValid = TRUE;

		if(pstMediaPlay->bUnderflowAutoMode)
		{
#ifdef MAXDOME_OLD_PDLMODE
	#if defined(CONFIG_TIMESTAMP_64)
			pstMediaPlay->ullNonstopBaseTime = pstMediaPlay->stCsoStatus.ullRequestPlayTime;
	#else
			pstMediaPlay->ullNonstopBaseTime = pstMediaPlay->stCsoStatus.uiRequestPlayTime;
	#endif
			pstMediaPlay->bNonstopState = FALSE;
#endif
		}
		if (pstMediaPlay->bUnderflowAutoMode == FALSE && pstMediaPlay->stCsoStatus.uiPlayingSpeed != 100 && pstMediaPlay->stCsoStatus.uiRequestPlaySpeed != 0)
			{
#if defined(CONFIG_PROD_YSR2000) && defined(MEDIAPB_RECOVER_TRICKMODE) // recover trick speed on seek complete.
			pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
			hErr = om_media_play_mediapb_ApplyPlaySpeed(ulActionId,pstMediaPlay->stCsoStatus.uiRequestPlaySpeed);
			bKeepStatus = TRUE;
#else
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 100);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
				}
			if( pstMediaPlay->stCsoStatus.uiRequestPlaySpeed != 0 && pstMediaPlay->stCsoStatus.uiPlayingSpeed == 0)
			{
				if(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX)
				{
					om_media_play_mediapb_EvtPlaying(pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				}
			}
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = 100;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = 100;
			pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = 100;
#endif
			}
		else if ( pstMediaPlay->stCsoStatus.uiRequestPlaySpeed != 0 )
			{
#if defined(CONFIG_PROD_YSR2000) && defined(MEDIAPB_RECOVER_TRICKMODE) // recover trick speed on seek complete.
			pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
			hErr = om_media_play_mediapb_ApplyPlaySpeed(ulActionId,pstMediaPlay->stCsoStatus.uiRequestPlaySpeed);
			bKeepStatus = TRUE;
#else
			hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 100);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
				}
			pstMediaPlay->stCsoStatus.uiPlayingSpeed = 100;
			pstMediaPlay->stCsoStatus.uiDecoderSpeed = 100;
			//om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 100, 0, 0);
#endif
			}
		else if( pstMediaPlay->stCsoStatus.uiRequestPlaySpeed == 0)
		{
			hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 0);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
				}
			pstMediaPlay->stCsoStatus.uiPlayingSpeed = 0;
			pstMediaPlay->stCsoStatus.uiDecoderSpeed = 0;
			om_media_play_mediapb_EvtPause (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			HxLOG_Error ("Send Paused STATE\n");
			om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		}
		hErr = SVC_CSO_TrickMode(pstMediaPlay->pMediaPlayer, FALSE);
		if ( hErr != ERR_OK )
		{
			HxLOG_Error ("SVC_CSO_TrickMode() Failed.\n");
		}

		hErr = SVC_CSO_Flush (pstMediaPlay->pMediaPlayer);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_CSO_Flush Failed \n");
		}
		pstMediaPlay->stCsoStatus.ulFlushedTime = time(NULL);

		err = MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_RESUME, &tParam);
		if(err != CSO_ERR_OK)
		{
			HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
			pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
			om_media_play_mediapb_EvtError (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			HxSEMT_Release(semaid);
			return;
		}
#if defined(CONFIG_PROD_YSR2000) && defined(MEDIAPB_RECOVER_TRICKMODE) // recover trick speed on seek complete.
		if( bKeepStatus != TRUE)
#endif
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;

		HxSEMT_Release(semaid);
		break;
		/* This callback comes right after Entering SEEKTRICK, Exiting SEEKTRICK  */
	case CSO_EVENT_MEDIA_TRICK_COMPLETE:
		semaid = om_media_play_mediapb_cso_GetSemaphoreId(pstContext);
		if (semaid == 0 )
		{
			HxLOG_Error("[%s](L:%d) Error - semaphoreId is 0.\n", __FUNCTION__, __LINE__);
			return;
		}
		HxSEMT_Get(semaid);

		if ( pstMediaPlay->stCsoStatus.eStatus < eCSO_PLAYING )
		{
			HxLOG_Error ("not playing status...\n");
			HxSEMT_Release(semaid);
			return;
		}

		if ( pstMediaPlay->stCsoStatus.eStatus  == eCSO_JUMP_TRICK_REQUESTED )
		{
			/* Previous Decoder Tricking...*/
			if ( pstMediaPlay->stCsoStatus.uiDecoderSpeed != 100 )
			{
				/* After the Flush, this position value will be checked by GetPlayInfo. */
				pstMediaPlay->bCheckPositionValid = TRUE;

				hErr = SVC_CSO_Flush (pstMediaPlay->pMediaPlayer);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_CSO_Flush Failed \n");
				}
				pstMediaPlay->stCsoStatus.ulFlushedTime = time(NULL);
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 100);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
				}

				pstMediaPlay->stCsoStatus.uiDecoderSpeed = 100;

				hErr = SVC_CSO_TrickMode(pstMediaPlay->pMediaPlayer, TRUE);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_TrickMode() Failed.\n");
				}

				pstMediaPlay->stCsoStatus.uiPlayingSpeed = (HINT32)param;
			}
			else
			{
				/* Previous Jump Tricking... No Need Flush...*/
				if  ( pstMediaPlay->stCsoStatus.uiPlayingSpeed != 100 )
				{
					hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 100);
					if ( hErr != ERR_OK )
					{
						HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
					}
					hErr = SVC_CSO_TrickMode(pstMediaPlay->pMediaPlayer, TRUE);
					if ( hErr != ERR_OK )
					{
						HxLOG_Error ("SVC_CSO_TrickMode() Failed.\n");
					}

					pstMediaPlay->stCsoStatus.uiPlayingSpeed = (HINT32)param;
				}
				else /* Previous Normal Play...*/
				{
					/* After the Flush, this position value will be checked by GetPlayInfo. */
					pstMediaPlay->bCheckPositionValid = TRUE;

					hErr = SVC_CSO_Flush (pstMediaPlay->pMediaPlayer);
					if (ERR_OK != hErr)
					{
						HxLOG_Error ("SVC_CSO_Flush Failed \n");
					}
					pstMediaPlay->stCsoStatus.ulFlushedTime = time(NULL);
					hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, 100);
					if ( hErr != ERR_OK )
					{
						HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
					}

					pstMediaPlay->stCsoStatus.uiDecoderSpeed = 100;

					hErr = SVC_CSO_TrickMode(pstMediaPlay->pMediaPlayer, TRUE);
					if ( hErr != ERR_OK )
					{
						HxLOG_Error ("SVC_CSO_TrickMode() Failed.\n");
					}

					pstMediaPlay->stCsoStatus.uiPlayingSpeed = (HINT32)param;
				}
			}

			err = MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_RESUME, &tParam);
			if(err != CSO_ERR_OK)
			{
				HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
				pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
				om_media_play_mediapb_EvtError (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				HxSEMT_Release(semaid);
				return;
			}
			pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICKING;
		}
#if defined(CONFIG_PROD_YSR2000)
		else if ( pstMediaPlay->stCsoStatus.eStatus  == eCSO_JUMP_TRICK_CANCEL_REQUESTED
		|| pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICKING)
#else
		else if ( pstMediaPlay->stCsoStatus.eStatus  == eCSO_JUMP_TRICK_CANCEL_REQUESTED )
#endif
		{
			if ( param != 100 )
			{
				HxLOG_Error ("Abnormal case Currnt Status is Cancel Requeseted but, %lldX completed!!!. this should be 100\n", param);
#if defined(CONFIG_PROD_YSR2000)
				HxLOG_Error ("%lldX completed!!!. but last request is %u\n", param,pstMediaPlay->stCsoStatus.uiRequestPlaySpeed);
				if( pstMediaPlay->stCsoStatus.uiRequestPlaySpeed == 100)
				{
					param = 100;
				}
				hErr = om_media_play_mediapb_ApplyPlaySpeed(ulActionId,param);
#endif
			}

#if defined(CONFIG_PROD_YSR2000)
			else
			{
#endif
			HxLOG_Error ("eCSO_JUMP_TRICK_CANCEL_REQUESTED Decoder Speed = %d \n",  pstMediaPlay->stCsoStatus.uiDecoderSpeed);

			/* After the Flush, this position value will be checked by GetPlayInfo. */
			pstMediaPlay->bCheckPositionValid = TRUE;

			hErr = SVC_CSO_Flush (pstMediaPlay->pMediaPlayer);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SVC_CSO_Flush Failed \n");
			}
			pstMediaPlay->stCsoStatus.ulFlushedTime = time(NULL);

			hErr = SVC_CSO_TrickMode(pstMediaPlay->pMediaPlayer, FALSE);
			if ( hErr != ERR_OK )
			{
				HxLOG_Error ("SVC_CSO_TrickMode() Failed.\n");
			}

			err = MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_RESUME, &tParam);
			if(err != CSO_ERR_OK)
			{
				HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
				pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
				om_media_play_mediapb_EvtError (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
				HxSEMT_Release(semaid);
				return;
			}

			if ( pstMediaPlay->stCsoStatus.uiDecoderSpeed != 100 && pstMediaPlay->stCsoStatus.uiDecoderSpeed != 0 )
			{
				pstMediaPlay->stCsoStatus.eStatus = eCSO_DECODER_TRICKING;
			}
#if defined(CONFIG_PROD_YSR2000)
				else if( pstMediaPlay->stCsoStatus.uiPlayingSpeed != 100 && pstMediaPlay->stCsoStatus.uiPlayingSpeed != 0
				&&  pstMediaPlay->stCsoStatus.uiDecoderSpeed == 100)
				{
					pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICKING;
				}
#endif
			else
			{
				pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
			}
#if defined(CONFIG_PROD_YSR2000)
			}
#endif
		}
		else /* Abnormal Case !!!!!!! */
		{
			HxLOG_Error ("Abnormal case !!!!!!!!!!!!!!!!!!! need to Check this out!!! rate = %lld,State %d \n", param,pstMediaPlay->stCsoStatus.eStatus);
		}

		om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->stCsoStatus.uiPlayingSpeed, 0, 0);

		HxSEMT_Release(semaid);

		break;
	case CSO_EVENT_MEDIA_TRICK_CHANGE_RATE:
		HxLOG_Error ("This event shouldn't be got!!!!!!!! %d\n", cso_event);

		if ( pstMediaPlay->stCsoStatus.eStatus < eCSO_PLAYING )
		{
			HxLOG_Error ("not playing status...\n");
			return;
		}
		break;
	case CSO_EVENT_ERROR_SEEK_FAIL:
		break;
	case CSO_EVENT_ERROR_TRICK_FAIL:
		break;
	case CSO_EVENT_DRM_PLAYREADY_UPDATE_OUTPUTCONTROL:
		/* Output Level 조정이 필요한 경우, 즉 DRM Contents 일 경우*/
		om_media_play_mediapb_EvtOutputControl(pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		break;
	case CSO_EVENT_DRM_VIEWRIGHTWEB:
	{
		HINT32 nErrorCode = param;
		pstContext->stCache.nVmxErrorCode = nErrorCode;
		om_media_play_mediapb_CbOnVmxRightsErrorChanged(pstContext->ulActionId - eActionId_VIEW_FIRST, pstContext->ulSessionId, nErrorCode);
		break;
	}
	default:
		break;
	}

}
#endif /* #ifdef CONFIG_CALYPSO */
STATIC HERROR om_media_play_mediapb_getSubtitleEnableInfo(HUINT32 ulActionId, HBOOL *bEnable)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMP_SUBTTL_Info_t tCurrSetting;
	HERROR				 hErr;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

#ifdef CONFIG_SUPPORT_IMMA
	hErr = SVC_CSO_GetSubtitleDisplay(pstMediaPlay->pMediaPlayer, &tCurrSetting.bDisplay);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}

	if (TRUE == pstContext->bActionTaken)
	{
        pstMediaPlay->stSubttlInfo.bDisplay = (HUINT32)tCurrSetting.bDisplay;
        *bEnable = pstMediaPlay->stSubttlInfo.bDisplay;
        HxLOG_Print("[%s:%d]bDisplay : %d\n",__FUNCTION__, __LINE__,tCurrSetting.bDisplay);
	}
#else
	hErr = SVC_MP_GetSubtitleDisplay(pstMediaPlay->pMediaPlayer, &tCurrSetting.bDisplay);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}

	if (TRUE == pstContext->bActionTaken)
	{
        pstMediaPlay->stSubttlInfo.bDisplay = (HUINT32)tCurrSetting.bDisplay;
        *bEnable = pstMediaPlay->stSubttlInfo.bDisplay;
        HxLOG_Print("[%s:%d]bDisplay : %d\n",__FUNCTION__, __LINE__,tCurrSetting.bDisplay);
	}
#endif
	return ERR_OK;
}
STATIC HERROR om_media_play_mediapb_setSubtitleInfo(HUINT32 ulActionId, omMP_SUBTTL_Info_t *pstSubtitleInfo)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMP_SUBTTL_Info_t tCurrSetting;
	HERROR				 hErr;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

#ifdef CONFIG_SUPPORT_IMMA
	hErr = SVC_CSO_GetSubtitleDisplay(pstMediaPlay->pMediaPlayer, &tCurrSetting.bDisplay);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}

	hErr = SVC_CSO_GetSubtitleCurrLangIndex(pstMediaPlay->pMediaPlayer, &tCurrSetting.ulCurrLangIdx);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}

	hErr = SVC_CSO_GetSubtitleCurSyncTime(pstMediaPlay->pMediaPlayer, &tCurrSetting.ulSyncTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}

	hErr = SVC_CSO_GetSubtitleFontSize(pstMediaPlay->pMediaPlayer, &tCurrSetting.ulFontSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}

	hErr = SVC_CSO_GetSubtitleDisplayPosition(pstMediaPlay->pMediaPlayer, &tCurrSetting.ulTextPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}


	if (TRUE == pstContext->bActionTaken)
	{
		if(tCurrSetting.ulCurrLangIdx != pstSubtitleInfo->ulCurrLangIdx)
		{
			hErr = SVC_CSO_SetSubtitleLanguage(pstMediaPlay->pMediaPlayer, pstSubtitleInfo->ulCurrLangIdx);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}

		if(tCurrSetting.ulSyncTime != pstSubtitleInfo->ulSyncTime)
		{
			hErr = SVC_CSO_ChangeSubtitleSyncTime(pstMediaPlay->pMediaPlayer, pstSubtitleInfo->ulSyncTime);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}

		if(tCurrSetting.bDisplay != pstSubtitleInfo->bDisplay)
		{
			hErr = SVC_CSO_SetSubtitleDisplay(pstMediaPlay->pMediaPlayer, pstSubtitleInfo->bDisplay);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}

		if(tCurrSetting.ulFontSize != pstSubtitleInfo->ulFontSize)
		{
			hErr = SVC_CSO_SetSubtitleFontSize(pstMediaPlay->pMediaPlayer, pstSubtitleInfo->ulFontSize);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}

		if(tCurrSetting.ulTextPosition != pstSubtitleInfo->ulTextPosition)
		{
			hErr = SVC_CSO_SetSubtitleDisplayPosition(pstMediaPlay->pMediaPlayer, pstSubtitleInfo->ulTextPosition);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}
	}
#else
	hErr = SVC_MP_GetSubtitleDisplay(pstMediaPlay->pMediaPlayer, &tCurrSetting.bDisplay);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}

	hErr = SVC_MP_GetSubtitleCurrLangIndex(pstMediaPlay->pMediaPlayer, &tCurrSetting.ulCurrLangIdx);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}

	hErr = SVC_MP_GetSubtitleCurSyncTime(pstMediaPlay->pMediaPlayer, &tCurrSetting.ulSyncTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}

	hErr = SVC_MP_GetSubtitleFontSize(pstMediaPlay->pMediaPlayer, &tCurrSetting.ulFontSize);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}

	hErr = SVC_MP_GetSubtitleDisplayPosition(pstMediaPlay->pMediaPlayer, &tCurrSetting.ulTextPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
		return ERR_FAIL;
	}


	if (TRUE == pstContext->bActionTaken)
	{
		if(tCurrSetting.ulCurrLangIdx != pstSubtitleInfo->ulCurrLangIdx)
		{
			hErr = SVC_MP_SetSubtitleLanguage(pstMediaPlay->pMediaPlayer, pstSubtitleInfo->ulCurrLangIdx);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}

		if(tCurrSetting.ulSyncTime != pstSubtitleInfo->ulSyncTime)
		{
			hErr = SVC_MP_ChangeSubtitleSyncTime(pstMediaPlay->pMediaPlayer, pstSubtitleInfo->ulSyncTime);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}

		if(tCurrSetting.bDisplay != pstSubtitleInfo->bDisplay)
		{
			hErr = SVC_MP_SetSubtitleDisplay(pstMediaPlay->pMediaPlayer, pstSubtitleInfo->bDisplay);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}

		if(tCurrSetting.ulFontSize != pstSubtitleInfo->ulFontSize)
		{
			hErr = SVC_MP_SetSubtitleFontSize(pstMediaPlay->pMediaPlayer, pstSubtitleInfo->ulFontSize);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}

		if(tCurrSetting.ulTextPosition != pstSubtitleInfo->ulTextPosition)
		{
			hErr = SVC_MP_SetSubtitleDisplayPosition(pstMediaPlay->pMediaPlayer, pstSubtitleInfo->ulTextPosition);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("Error (pMediaPlayer:0x%08x):\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}
	}
#endif
	return ERR_OK;
}




/**
* @b Function @b Description <br>
* this function makes no state change.
*
* @param[in] omMediaPlay_Context_t   : media pb context
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/


#define _____MSG_PROCESS_FUNCTIONS_____
#if 0
STATIC BUS_Result_t mgr_appmgr_mctrl_mediapb_msgBusOapiAppKilled (_mctrlMediaPbContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			 ulAppId = (HUINT32)p1;
	HUINT32			 ulSessionVersion = 0;
	OCTOAPPInfo_t	 stActTakenApp;
	HERROR			 hErr;

	// 현재 Media Playback 중일 경우 :
	if (eViewType_MOVIE_PB == MGR_ACTION_GetType (pstContext->ulActionId))
	{
		hErr = APP_MGR_GetAppInfoByActionId (pstContext->ulActionId, &stActTakenApp, &ulSessionVersion);
		if ((ERR_OK == hErr) && (stActTakenApp.ulApplicationId == ulAppId))
		{
			// App이 죽었으면 당연히 그 App이 실행시킨 MediaPlay는 Stop 시킨다.
			if (NULL != pstContext->pMediaPlayer)
			{
				hErr = MGR_MPlayer_Stop (pstContext->pMediaPlayer);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("MGR_MPlayer_Stop() Failed.\n");
				}

				pstContext->bPlayed = FALSE;
			}

			APP_MGR_ResetActionToApp (pstContext->ulActionId);
		}
	}

	NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);
	return MESSAGE_PASS;
}
#endif

STATIC BUS_Result_t om_media_play_mediapb_EvtConnecting (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId		=	pstContext->ulSessionId;

	pstContext->stCache.eState	= eOxMP_PLAYSTATE_CONNECTING;
	pstContext->stCache.eError	= eOxMP_PLAYERROR_NO_ERROR;

	om_media_play_CbOnPlayStateChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYSTATE_CONNECTING);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtPlaying (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId		=	pstContext->ulSessionId;

	pstContext->stCache.eState	= eOxMP_PLAYSTATE_PLAYING;
	pstContext->stCache.eError	= eOxMP_PLAYERROR_NO_ERROR;

	/* take over start informaion */
	{
		HUINT32 ulNumSpeed;
		HINT32 anSpeeds[OxMP_SUPPORTING_SPEED_MAX];

		//printf("[parkjh4] Status is changed to playing\n");
		om_media_play_mediapb_GetSupportedSpeeds	(ulActionId, &ulNumSpeed, anSpeeds);

		om_media_play_CbOnSupportedSpeedChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, ulNumSpeed, (HINT32 *)anSpeeds);
		om_media_play_CbOnPlayStateChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYSTATE_PLAYING);
	}

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtNewFrame (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32 	ulSessionId	 =	 0;
	OxMediaPlay_VideoStreamInfo_t	stOapiVideoStreamInfo = {0,};
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	ulSessionId	 =	 pstContext->ulSessionId;
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if (pstMediaPlay == NULL)
	{
		return ERR_FAIL;
	}

#ifdef CONFIG_SUPPORT_IMMA
	stOapiVideoStreamInfo.ulHSize = pstMediaPlay->pMediaPlayer->ulVideoStreamWidth;
	stOapiVideoStreamInfo.ulVSize = pstMediaPlay->pMediaPlayer->ulVideoStreamHeight;
	stOapiVideoStreamInfo.eAspectRatio = pstMediaPlay->pMediaPlayer->eVideoAspectRatio;
#endif
	//HxLOG_Error("%s(%d) [CRB] Video New Frame. \n", __FUNCTION__, __LINE__);

	om_media_play_CbOnVideoStreamInfo(ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, &stOapiVideoStreamInfo);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtAVResume (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32 	ulSessionId	 =	 0;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	ulSessionId	 =	 pstContext->ulSessionId;
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

#ifdef CONFIG_SUPPORT_IMMA
	pstMediaPlay->bAVUnderrun = FALSE;
#endif

	om_media_play_CbOnAVResume(ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtAVUnderrun (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32 	ulSessionId	 =	 0;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	ulSessionId	 =	 pstContext->ulSessionId;
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

#ifdef CONFIG_SUPPORT_IMMA
	pstMediaPlay->bAVUnderrun = TRUE;
#endif

	om_media_play_CbOnAVUnderrun(ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtPause (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId		=	pstContext->ulSessionId;

	pstContext->stCache.eState	= eOxMP_PLAYSTATE_PAUSE;
	pstContext->stCache.eError	= eOxMP_PLAYERROR_NO_ERROR;

	om_media_play_CbOnPlayStateChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYSTATE_PAUSE);

	return MESSAGE_PASS;
}

#ifndef CONFIG_SUPPORT_IMMA
STATIC BUS_Result_t om_media_play_mediapb_EvtStopped (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId		=	pstContext->ulSessionId;

	pstContext->stCache.eState	= eOxMP_PLAYSTATE_STOPPED;
	pstContext->stCache.eError	= eOxMP_PLAYERROR_NO_ERROR;

	/* clean cache */
	om_media_play_CleanCache (pstContext, eOxMP_MEDIATYPE_MEDIAPLAY);

	om_media_play_CbOnPlayStateChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYSTATE_STOPPED);

	return MESSAGE_PASS;
}
#endif

STATIC BUS_Result_t om_media_play_mediapb_EvtProbeFailed (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId		=	pstContext->ulSessionId;
	omMPMediaPlayContext_t     *pstMediaPlay = NULL;

	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);
	pstContext->stCache.eError	= eOxMP_PLAYERROR_PROBE_FAILED;

	om_media_play_CbOnPlayErrorChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYERROR_PROBE_FAILED, pstMediaPlay->eErrorReason);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtError (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId		=	pstContext->ulSessionId;
	omMPMediaPlayContext_t     *pstMediaPlay = NULL;

	//printf("[parkjh4] Status is changed to error\n");
	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);
	pstContext->stCache.eError	= eOxMP_PLAYERROR_PLAY_ERROR;

	om_media_play_CbOnPlayErrorChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYERROR_PLAY_ERROR, pstMediaPlay->eErrorReason);

	return MESSAGE_PASS;
}

#ifndef CONFIG_SUPPORT_IMMA
STATIC BUS_Result_t om_media_play_mediapb_EvtSeekError (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId	=	pstContext->ulSessionId;

	pstContext->stCache.eError	= eOxMP_PLAYERROR_SEEK_ERROR;

	om_media_play_CbOnPlayErrorChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYERROR_SEEK_ERROR, 0);

	return MESSAGE_PASS;
}
#endif

STATIC BUS_Result_t om_media_play_mediapb_EvtBeginOfStream (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);

	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32 	ulSessionId	=	pstContext->ulSessionId;

	om_media_play_CbOnPlayStateChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYSTATE_BOS);
	om_media_play_CbOnBof (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtEndOfStream (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32 	ulSessionId	=	0;

	omMPMediaPlayContext_t *pstMediaPlay    = NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
    if (NULL == pstContext->pExtension)        { return ERR_FAIL; }

	ulSessionId		=	pstContext->ulSessionId;
    pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);

#ifdef CONFIG_SUPPORT_IMMA
    pstMediaPlay->bEofReached = TRUE;
#if defined(CONFIG_TIMESTAMP_64)
    pstMediaPlay->stCsoStatus.ullPlayingTime = pstMediaPlay->ullDuration;
#else
	pstMediaPlay->stCsoStatus.uiPlayingTime = pstMediaPlay->ullDuration;
#endif

	/* MHEG ICS003 underflow 와 resume event는 pair를 이루어야 한다. 종료전 underflow 상태이면, resume을 보낸다. */
	if (pstMediaPlay->bAVUnderrun)
	{
		om_media_play_mediapb_EvtAVResume(pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), p1, p2, p3);
	}

    om_media_play_CbOnPlayTimeChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY,  pstMediaPlay->ullDuration, pstMediaPlay->ullDuration);
#endif
	om_media_play_CbOnPlayStateChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, eOxMP_PLAYSTATE_EOS);
	om_media_play_CbOnEof (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY);

	return MESSAGE_PASS;
}

#ifdef CONFIG_SUPPORT_IMMA
STATIC BUS_Result_t om_media_play_mediapb_EvtPlayDurationChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HUINT64 ullDuration)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId 	=	0;

	omMPMediaPlayContext_t *pstMediaPlay    = NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
    if (NULL == pstContext->pExtension)        { return ERR_FAIL; }

	ulSessionId 	=	pstContext->ulSessionId;
    pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);

#if defined(CONFIG_TIMESTAMP_64)
	om_media_play_CbOnPlayDurationChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY,  ullDuration, (ullDuration == 0x4189374BC6A7)? TRUE: FALSE);
#else
	om_media_play_CbOnPlayDurationChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY,  (HUINT32)ullDuration, (ullDuration == 0x4189374BC6A7)? TRUE: FALSE);
#endif
	return MESSAGE_PASS;
}
#endif
STATIC BUS_Result_t om_media_play_mediapb_EvtPlayPositionChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32	ulSessionId 		=	0;
#ifdef CONFIG_SUPPORT_IMMA
	HERROR      hErr;
	HUINT64		pullPosition_ms = 0;
#endif

	omMPMediaPlayContext_t *pstMediaPlay    = NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
    if (NULL == pstContext->pExtension)        { return ERR_FAIL; }

	ulSessionId 		=	pstContext->ulSessionId;
    pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);

#ifdef CONFIG_SUPPORT_IMMA
    if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_SEEK_REQUESTED )
    {
            HxLOG_Error("[%s](L:%d) Seek Requested or Just Seeked. \n", __FUNCTION__, __LINE__);
#if defined(CONFIG_TIMESTAMP_64)
            pullPosition_ms = pstMediaPlay->stCsoStatus.ullRequestPlayTime;
#else
			pullPosition_ms = pstMediaPlay->stCsoStatus.uiRequestPlayTime;
#endif
            goto do_notify;
    }
    else if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_REQUESTED )
    {
            HxLOG_Error("[%s](L:%d) Status is Just Trick Requested... \n", __FUNCTION__, __LINE__);
#if defined(CONFIG_TIMESTAMP_64)
            pullPosition_ms = pstMediaPlay->stCsoStatus.ullPlayingTime;
#else
			pullPosition_ms = pstMediaPlay->stCsoStatus.uiPlayingTime;
#endif
            goto do_notify;
    }
    else if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_CANCEL_REQUESTED )
    {
            HxLOG_Error("[%s](L:%d) Status is Just Trick Cancel Requested... \n", __FUNCTION__, __LINE__);
#if defined(CONFIG_TIMESTAMP_64)
            pullPosition_ms = pstMediaPlay->stCsoStatus.ullPlayingTime;
#else
			pullPosition_ms = pstMediaPlay->stCsoStatus.uiPlayingTime;
#endif
            goto do_notify;
    }

    hErr = SVC_CSO_GetPlayPosition(pstMediaPlay->pMediaPlayer, &pullPosition_ms);
    if (ERR_OK != hErr)
    {
    	HxLOG_Error ("Error : GetPlayPosition\n");
    }

    if ( pstMediaPlay->bCheckPositionValid )
    {
#if defined(CONFIG_TIMESTAMP_64)
            if ( ( pullPosition_ms == 0 ) && ( pstMediaPlay->stCsoStatus.ullRequestPlayTime != 0  ) )
#else
			if ( ( pullPosition_ms == 0 ) && ( pstMediaPlay->stCsoStatus.uiRequestPlayTime != 0  ) )
#endif
            {
                    HxLOG_Error("[%s](L:%d) !!!! Position is now at invalid position... !!! \n", __FUNCTION__, __LINE__);
#if defined(CONFIG_TIMESTAMP_64)					
                    pstMediaPlay->stCsoStatus.ullPlayingTime = pstMediaPlay->stCsoStatus.ullRequestPlayTime;
                    pullPosition_ms = pstMediaPlay->stCsoStatus.ullRequestPlayTime;
#else
					pstMediaPlay->stCsoStatus.uiPlayingTime = pstMediaPlay->stCsoStatus.uiRequestPlayTime;
					pullPosition_ms = pstMediaPlay->stCsoStatus.uiRequestPlayTime;
#endif
            }
			else if ( ( pullPosition_ms == 0 ) && ( pstMediaPlay->ullSeekedTimeByCso != 0) )
			{
					HxLOG_Error("[%s](L:%d) !!! Calypso returns internal seekd position (%lld) !!! \n", __FUNCTION__, __LINE__, pstMediaPlay->ullSeekedTimeByCso);
#if defined(CONFIG_TIMESTAMP_64)
					pstMediaPlay->stCsoStatus.ullPlayingTime = pstMediaPlay->ullSeekedTimeByCso;
					pullPosition_ms = pstMediaPlay->ullSeekedTimeByCso;
#else
					pstMediaPlay->stCsoStatus.uiPlayingTime = pstMediaPlay->ullSeekedTimeByCso;
					pullPosition_ms = pstMediaPlay->ullSeekedTimeByCso;
#endif
					pstMediaPlay->ullSeekedTimeByCso = 0;
			}
            else
            {
            	pstMediaPlay->bCheckPositionValid = FALSE;
				pullPosition_ms += pstMediaPlay->ullBasePosition;		/* Add baseposition */
            }
    }
    else
    {
        if ( pstMediaPlay->bEofReached )
        {
#if defined(CONFIG_TIMESTAMP_64)			
        	if (pstMediaPlay->stCsoStatus.ullPlayingTime !=  pstMediaPlay->ullDuration)
#else
			if (pstMediaPlay->stCsoStatus.uiPlayingTime !=  pstMediaPlay->ullDuration)
#endif
        	{
                HxLOG_Error("[%s](L:%d) End Of Stream returns position and duratin %lld !!! \n", __FUNCTION__, __LINE__, pstMediaPlay->ullDuration);
#if defined(CONFIG_TIMESTAMP_64)			
        		pstMediaPlay->stCsoStatus.ullPlayingTime = pstMediaPlay->ullDuration;
#else
				pstMediaPlay->stCsoStatus.uiPlayingTime = pstMediaPlay->ullDuration;
#endif
        		pullPosition_ms = pstMediaPlay->ullDuration;
        		goto do_notify;
        	}
        	else
        	{
        		//HxLOG_Debug("bEofReached, pstMediaPlay->stCsoStatus.ullPlayingTime:%llu Playtime:%llu Duration: %llu \n", pstMediaPlay->stCsoStatus.uiPlayingTime, pullPosition_ms, pstMediaPlay->ullDuration);
        		return MESSAGE_PASS;
        	}
        }

    	/* Position Not Changed...*/
#if defined(CONFIG_TIMESTAMP_64)		
    	if ( pstMediaPlay->stCsoStatus.ullPlayingTime == pullPosition_ms)
#else
		if ( pstMediaPlay->stCsoStatus.uiPlayingTime == pullPosition_ms)
#endif
    	{
#if defined(CONFIG_TIMESTAMP_64)			
    		HxLOG_Debug("pstMediaPlay->stCsoStatus.ullPlayingTime:%llu Playtime:%llu Duration: %llu \n", pstMediaPlay->stCsoStatus.ullPlayingTime, pullPosition_ms, pstMediaPlay->ullDuration);
#else
			HxLOG_Debug("pstMediaPlay->stCsoStatus.ullPlayingTime:%u Playtime:%llu Duration: %llu \n", pstMediaPlay->stCsoStatus.uiPlayingTime, pullPosition_ms, pstMediaPlay->ullDuration);
#endif
			if((pstMediaPlay->ullDuration != 0 ) && (pullPosition_ms >= pstMediaPlay->ullDuration))
			{
				/* Issue : playtime > duration */
				pstMediaPlay->bCsoEofReached = TRUE;
			}

    		if(pstMediaPlay->bCsoEofReached == TRUE)
    		{
    			if(pstMediaPlay->stCsoStatus.uiRequestPlaySpeed == 0) /*pause*/
    			{
    				pstMediaPlay->ulCsoEofReachedTime = time(NULL);
    			}
    			else
    			{
    				if(pstMediaPlay->ulCsoEofReachedTime == 0)
    					pstMediaPlay->ulCsoEofReachedTime = time(NULL);
					HxLOG_Debug ("bCsoEofReached : pstMediaPlay->ulCsoEofReachedTime %u:%u time \n", pstMediaPlay->ulCsoEofReachedTime, time(NULL));
	    			if((pstMediaPlay->ulCsoEofReachedTime != 0) && ((time(NULL) - pstMediaPlay->ulCsoEofReachedTime) > 2/*sec*/))
	    			{
	    				HxLOG_Error("[%s](L:%d) End Of Stream returns position and duratin %lld !!! \n", __FUNCTION__, __LINE__, pstMediaPlay->ullDuration);
#if defined(CONFIG_TIMESTAMP_64)									
		        		pstMediaPlay->stCsoStatus.ullPlayingTime = pstMediaPlay->ullDuration;
#else
						pstMediaPlay->stCsoStatus.uiPlayingTime = pstMediaPlay->ullDuration;
#endif
		        		pullPosition_ms = pstMediaPlay->ullDuration;
		        		pstMediaPlay->ulCsoEofReachedTime = 0;
#ifdef HLS_DISCONTINUITY
						if( pstMediaPlay->bNeedRestart == TRUE )
						{
							pstMediaPlay->bNeedRestart = FALSE;
							pstMediaPlay->bCsoEofReached = FALSE;
							om_media_play_mediapb_cso_Reset( pstContext );
						}
						else
						{
							om_media_play_mediapb_EvtEndOfStream(pstContext, hAction, p1, p2, p3);
						}
#else
		        		om_media_play_mediapb_EvtEndOfStream(pstContext, hAction, p1, p2, p3);
#endif
					}
	    		}
    		}
    		return MESSAGE_PASS;
    	}
    	else
    	{
			pullPosition_ms += pstMediaPlay->ullBasePosition;
#if defined(CONFIG_TIMESTAMP_64)			
        	pstMediaPlay->stCsoStatus.ullPlayingTime = pullPosition_ms;
#else
			pstMediaPlay->stCsoStatus.uiPlayingTime = pullPosition_ms;
#endif
        	if(pstMediaPlay->bCsoEofReached == TRUE)
        	{
        		pstMediaPlay->ulCsoEofReachedTime = time(NULL);
        	}
    	}
    }

    if ( pstMediaPlay->bBofReached )
    {
#if defined(CONFIG_TIMESTAMP_64)
    	pstMediaPlay->stCsoStatus.ullPlayingTime = pstMediaPlay->ullBasePosition;
#else
		pstMediaPlay->stCsoStatus.uiPlayingTime = pstMediaPlay->ullBasePosition;
#endif
    	pullPosition_ms = pstMediaPlay->ullBasePosition;
    }


do_notify:
	HxLOG_Error("Playtime:%llu BasePosition:%llu Duration: %llu \n", pullPosition_ms, pstMediaPlay->ullBasePosition, pstMediaPlay->ullDuration);
	om_media_play_CbOnPlayTimeChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY,  pullPosition_ms, pstMediaPlay->ullDuration);
#endif

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_PlaySpeedChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);

	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId 	=	pstContext->ulSessionId;

#if 0
	if( 0 == p1 )	om_media_play_CbOnPlayStateChanged (ulViewId, pstContext->ulSessionId, eOxMP_PLAYSTATE_PAUSE);
	else			om_media_play_CbOnPlayStateChanged (ulViewId, pstContext->ulSessionId, eOxMP_PLAYSTATE_PLAYING);
#endif

	om_media_play_CbOnSpeedChanged( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, p1 );

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_PumpIndexChanged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulActionId		=	SVC_PIPE_GetActionId (hAction);

	HUINT32 	ulViewId		=	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId 	=	pstContext->ulSessionId;

	om_media_play_mediapb_CbOnPumpIndexChanged (ulViewId, ulSessionId, p1, p2);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_SeekReady (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulActionId		=	SVC_PIPE_GetActionId (hAction);

	HUINT32 	ulViewId		=	ulActionId - eActionId_VIEW_FIRST;
	HUINT32		ulSessionId 	=	pstContext->ulSessionId;

	om_media_play_mediapb_CbOnSeekReady (ulViewId, ulSessionId);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtSBTLStart (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);

	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32 	ulSessionId 	= 	0;

	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	ulSessionId 	= 	pstContext->ulSessionId;
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	pstMediaPlay->stSubttlStatus.eState		= eSUBTTLState_Start;
	pstMediaPlay->stSubttlStatus.ulSharedKey 	= p1;
	pstMediaPlay->stSubttlStatus.ulSharedSize	= p2;

	om_media_play_mediapb_CbOnSBTLUpdate (ulViewId, ulSessionId, &(pstMediaPlay->stSubttlStatus));

#ifdef CONFIG_SUPPORT_IMMA
	/* subtitle */
	{
		HERROR hErr = ERR_FAIL;
		HINT32	i = 0;
		HUINT32 ulSubtitleCnt = 0;
		HUINT32 ulCurrIndex = 0;
		HINT8	szDispLang[32] = {0,};
        HBOOL   bEnable;

		OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable = &pstContext->stCache.stSubtitleCompTable;
		HxSTD_MemSet (pstSubtitleCompTable, 0, sizeof(OxMediaPlay_SubtitleCompTable_t));

		hErr = SVC_CSO_GetSubtitleTotalLangCnt( pstMediaPlay->pMediaPlayer, &ulSubtitleCnt );
		if( (0 < ulSubtitleCnt) && (ERR_OK == hErr) )
		{
			SVC_CSO_GetSubtitleCurrLangIndex (pstMediaPlay->pMediaPlayer, &ulCurrIndex);

			pstSubtitleCompTable->nCurrIndex		=	(HINT32)ulCurrIndex;
			pstSubtitleCompTable->nArraySize		=	ulSubtitleCnt;
			pstSubtitleCompTable->nMaxSize			=	sizeof(OxMediaPlay_SubtitleCompTable_t);

			for( i=0 ; i<ulSubtitleCnt; i++ )
			{
				HxSTD_MemSet(szDispLang, 0, sizeof(szDispLang));
				hErr = SVC_CSO_GetSubtitleLanguageByIndex (pstMediaPlay->pMediaPlayer,  i, szDispLang);
				if( ERR_OK == hErr )
				{
					pstSubtitleCompTable->astCompArray[i].eSubtitleType =	eOxMP_SUBTITLETYPE_Media;
					HLIB_STD_MemCpySafe(	(pstSubtitleCompTable->astCompArray[i].utArg.stMediaInfo.szLangCode),
											sizeof(pstSubtitleCompTable->astCompArray[i].utArg.stMediaInfo.szLangCode),
											(szDispLang),
											sizeof(szDispLang) );

				}
				else
				{
					HxLOG_Error("SVC_CSO_GetSubtitleLanguageByIndex Error : \n");
				}

			}
            om_media_play_mediapb_getSubtitleEnableInfo(ulActionId,&bEnable);
            om_media_play_CbOnSubtitleEnable(ulViewId,bEnable);
			om_media_play_CbOnComponentSubtitleChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, pstSubtitleCompTable);
		}
	}/* end of subtitle */
#endif

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtSBTLStop (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32 	ulSessionId	 =	 0;

	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	ulSessionId		=	pstContext->ulSessionId;
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	pstMediaPlay->stSubttlStatus.eState		= eSUBTTLState_Stopped;

	om_media_play_mediapb_CbOnSBTLUpdate (ulViewId, ulSessionId, &(pstMediaPlay->stSubttlStatus));

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtSBTLUpdate (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId		=	SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	HUINT32 	ulSessionId	 =	 0;

	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	ulSessionId	 =	 pstContext->ulSessionId;
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	pstMediaPlay->stSubttlStatus.eState = eSUBTTLState_Updated;

	pstMediaPlay->stSubttlStatus.ulLastRetrievingOffset = p1;
	pstMediaPlay->stSubttlStatus.ulLastPacketSize = p2;

	om_media_play_mediapb_CbOnSBTLUpdate (ulViewId, ulSessionId, &(pstMediaPlay->stSubttlStatus));

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtBufferingCanplay (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	 ulActionId 	 =	 SVC_PIPE_GetActionId (hAction);
	HUINT32	 ulViewId		 =	 ulActionId - eActionId_VIEW_FIRST;
	HUINT32 ulSessionId	 	 =	 pstContext->ulSessionId;

	pstContext->stCache.eBufferState   =  eOxMP_BUFFERSTATE_CanPlay;
	om_media_play_mediapb_CbOnBufferStateChanged (ulViewId, ulSessionId, eOxMP_BUFFERSTATE_CanPlay);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtPlayInfo(omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	 ulActionId 	 =	 SVC_PIPE_GetActionId (hAction);
	HUINT32	 ulViewId		 =	 ulActionId - eActionId_VIEW_FIRST;
	HUINT32 ulSessionId	 	 =	 pstContext->ulSessionId;

	om_media_play_mediapb_CbOnPlayInfo (ulViewId, ulSessionId, p1, p2, p3);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtBufferingNotEnough (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	 ulActionId 	 =	 SVC_PIPE_GetActionId (hAction);
	HUINT32	 ulViewId		 =	 ulActionId - eActionId_VIEW_FIRST;
	HUINT32	 ulSessionId	 =	 pstContext->ulSessionId;

	pstContext->stCache.eBufferState   =  eOxMP_BUFFERSTATE_NotEnough;
	om_media_play_mediapb_CbOnBufferStateChanged (ulViewId, ulSessionId, eOxMP_BUFFERSTATE_NotEnough);
	HxLOG_Debug("[%s](L:%d) Notify Buffering + UNDERRUN +\n",__FUNCTION__, __LINE__);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtBufferingEnough (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	 ulActionId 	 =	 SVC_PIPE_GetActionId (hAction);
	HUINT32	 ulViewId		 =	 ulActionId - eActionId_VIEW_FIRST;
	HUINT32	 ulSessionId	 =	 pstContext->ulSessionId;

	pstContext->stCache.eBufferState   =  eOxMP_BUFFERSTATE_Enough;
	om_media_play_mediapb_CbOnBufferStateChanged (ulViewId, ulSessionId, eOxMP_BUFFERSTATE_Enough);
	HxLOG_Debug("[%s](L:%d) Notify Buffering + ENOUGH +\n",__FUNCTION__, __LINE__);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtBufferingFinished (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	 ulActionId 	 =	 SVC_PIPE_GetActionId (hAction);
	HUINT32	 ulViewId		 =	 ulActionId - eActionId_VIEW_FIRST;
	HUINT32	 ulSessionId	 =	 pstContext->ulSessionId;

	pstContext->stCache.eBufferState   =  eOxMP_BUFFERSTATE_Buffering_Finished;
	om_media_play_mediapb_CbOnBufferStateChanged (ulViewId, ulSessionId, eOxMP_BUFFERSTATE_Buffering_Finished);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtMediaPBDrmBlock (omMediaPlay_Context_t *pstContext, Handle_t hAction, OxMediaPlay_DrmRightsError_t* pstDrmRightsError)
{
	HUINT32 	ulActionId  = SVC_PIPE_GetActionId (hAction);
	HUINT32 	ulViewId	= ulActionId - eActionId_VIEW_FIRST; //om_media_play_GetViewId(ulActionId);????
	HUINT32		ulSessionId 	= pstContext->ulSessionId;

	om_media_play_CbOnDrmBlocked( ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, pstDrmRightsError);

	//NOT_USED_PROC_ARGUMENT (hAction, p1, p2, p3);??
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtBufferingUpdate (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#ifdef CONFIG_SUPPORT_IMMA
	HUINT64		pullPosition_ms = 0;

	if( s_stMediaPlayerCache[CalcActionId(SVC_PIPE_GetActionId (hAction))].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
	{
		//HUINT32  ulActionId 	 =	 SVC_PIPE_GetActionId (hAction);
		//HUINT32  ulViewId		 =	 ulActionId - eActionId_VIEW_FIRST;
		HUINT32  ulSessionId	 =	 0;
#ifndef UPDATE_BUFFERINFO_ON_TASK
		HUINT32  ulIdx			 =	 0;
#else
		CSO_ERR err;
#endif
		omMPMediaPlayContext_t *pstMediaPlay	= NULL;
		OxMediaPlay_BufferInfo_t stBufferInfo;
		CSO_BufferInfo_t stCSOBufferInfo;
		HBOOL bUnderrun = FALSE;
		HERROR hErr = ERR_OK;
		HUINT32 semaid = 0;

		if (NULL == pstContext)	{ return ERR_FAIL; }
		if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

		ulSessionId	 =	 pstContext->ulSessionId;
		pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

		HxSTD_MemSet(&stCSOBufferInfo, 0, sizeof(CSO_BufferInfo_t));
		HxSTD_MemSet(&stBufferInfo, 0, sizeof(OxMediaPlay_BufferInfo_t));


		if(pstMediaPlay->stCsoStatus.eStatus <= eCSO_STOPPED)
		{
			HxLOG_Debug("[%s](L:%d) Warning - Not Update Buffering Info, Mediapb Status:%d\n",
						__FUNCTION__, __LINE__, pstMediaPlay->stCsoStatus.eStatus);
			return MESSAGE_PASS;
		}
		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_SEEK_REQUESTED )
		{
			HxLOG_Debug("[%s](L:%d) Waiting until the seek operation has completed. \n", __FUNCTION__, __LINE__);
			return MESSAGE_PASS;
		}
#ifdef UPDATE_BUFFERINFO_ON_TASK
		err = MEDIAPB_IPC_CSO_BufferInfo(pstMediaPlay->ulCsoSessionId, &stCSOBufferInfo);
		if ( err == CSO_ERR_INVALID_SESSION)
		{
			HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_BufferInfo (Invalid Session)\n", __FUNCTION__, __LINE__);
			return MESSAGE_PASS;
		}
		else if ( err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_BufferInfo\n", __FUNCTION__, __LINE__);
			return MESSAGE_PASS;
		}

		/* 다른 Task에서 pstContext->pExtension = NULL로 만들면, lock이 잡힌 sema가 풀리지 않고,
		 * 빠져나가는 경우가 존재하여, 저장된sema id를 해재하도록 함. 4개의 Task에서 사용됨*/
		semaid = om_media_play_mediapb_cso_GetSemaphoreId(pstContext);
		if (semaid == 0 )
		{
			HxLOG_Error("[%s](L:%d) Error - semaphoreId is 0.\n", __FUNCTION__, __LINE__);
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 18))
			MEDIAPB_IPC_CSO_IPCMSG_MemFree_BufferInfo(&stCSOBufferInfo);
#else
			om_media_play_mediapb_cso_MemFree_BufferInfo(&stCSOBufferInfo);
#endif
			return MESSAGE_PASS;
		}
		HxSEMT_Get(semaid);
		err = om_media_play_mediapb_UpdateBufferInfo(pstMediaPlay, &stCSOBufferInfo);
#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 18))
		MEDIAPB_IPC_CSO_IPCMSG_MemFree_BufferInfo(&stCSOBufferInfo);
#else
		om_media_play_mediapb_cso_MemFree_BufferInfo(&stCSOBufferInfo);
#endif
		if ( ERR_OK != err)
		{
			HxSEMT_Release(semaid);
			HxLOG_Error("[%s](L:%d) Error - om_media_play_mediapb_UpdateBufferInfo()\n", __FUNCTION__, __LINE__);
			return MESSAGE_PASS;
		}

		HxSTD_memcpy(&stBufferInfo, &pstMediaPlay->stBufferInfo, sizeof(OxMediaPlay_BufferInfo_t));
		if( stBufferInfo.ulChunks > 0 )
		{

//			HxLOG_Debug("[%s](L:%d) Decoded TimeStamp :%llu  \n", __FUNCTION__, __LINE__,pstMediaPlay->ullLastDecodedTime);
#if	0
			hErr = SVC_CSO_GetPlayPosition(pstMediaPlay->pMediaPlayer, &pullPosition_ms);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("[%s:%d] Error : GetPlayPosition\n", __FUNCTI
						ON__, __LINE__);
			}
#else

#if defined(CONFIG_TIMESTAMP_64)
			pullPosition_ms = pstMediaPlay->stCsoStatus.ullPlayingTime;
#else
			pullPosition_ms = pstMediaPlay->stCsoStatus.uiPlayingTime;
#endif

#endif

//			HxLOG_Debug("[%s](L:%d) Playing TimeStamp :%llu (Gap : %llu) \n", __FUNCTION__, __LINE__,pullPosition_ms,stBufferInfo.ullEndOffsets[0] - pullPosition_ms);
			if ( pstMediaPlay->eUnderflowMode == eOM_CSO_UNDERFLOW_THRESHOLD )
			{
				hErr = om_media_play_mediapb_CheckUnderrunThreshold(pstContext,pullPosition_ms,pstMediaPlay->ullLastDecodedTime,&stBufferInfo, &bUnderrun);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("[%s](L:%d) CheckUnderrunThreshold Fail\n", __FUNCTION__, __LINE__);
				}
			}
			else if ( pstMediaPlay->eUnderflowMode == eOM_CSO_UNDERFLOW_NONSTOP && pstMediaPlay->stCsoConfig.bProgressiveDownload)
			{

				hErr = om_media_play_mediapb_CheckUnderrunNonStop(pstContext,pullPosition_ms,pstMediaPlay->ullLastDecodedTime,&stBufferInfo, &bUnderrun);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("[%s](L:%d) CheckUnderrunNonStop Fail\n", __FUNCTION__, __LINE__);
				}

			}
			else if( pstMediaPlay->eUnderflowMode == eOM_CSO_UNDERFLOW_NONE )
			{
				/* nothing */
			}
			else
			{
				hErr = om_media_play_mediapb_CheckUnderrunThreshold(pstContext,pullPosition_ms,pstMediaPlay->ullLastDecodedTime,&stBufferInfo, &bUnderrun);

				if(hErr != ERR_OK)
				{
					HxLOG_Error("[%s](L:%d) CheckUnderrunThreshold Fail\n", __FUNCTION__, __LINE__);
				}
			}

			/* Undercontrol must be start after transcoder decoding is stabilized
			 * Here, ullFirstPts is directly accesses. To be honest, mediapb do not know the unit of first pts. However we need know this value here. */
			if(pstMediaPlay->eUnderflowMode != eOM_CSO_UNDERFLOW_NONE 
			&& pstMediaPlay->ullLastDecodedTime + (pstMediaPlay->ullFirstPts / 1000.) >= UNDERRUN_CONTROL_EXEMPTION_RANGE)
			{
				hErr = om_media_play_mediapb_ControlUnderrun(pstContext, bUnderrun);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("[%s](L:%d) ControlUnderrun Fail\n", __FUNCTION__, __LINE__);
				}
			}
			else
			{
				if(bUnderrun == TRUE)
				{
					HxLOG_Error("[%s](L:%d) Underrun occurs, however calypso decoded time is not enough to decide. keep going\n");
				}
			}

			/* move to om_media_play_mediapb_BufferInfoUpdateTask() */
			//(void)om_media_play_mediapb_CbOnBufferInfoChanged(ulViewId, ulSessionId, &stBufferInfo);
		}

		HxSEMT_Release(semaid);
	}
	else
	{
		/* eOxMP_MEDIAPLAY_PLAYERTYPE_MSE not emit event */
		pstContext = pstContext;
		hAction = hAction;
		p1 = p1;
		p2 = p2;
		p3 = p3;
	}
#else
		err = MEDIAPB_IPC_CSO_BufferInfo(pstMediaPlay->ulCsoSessionId, &stCSOBufferInfo);
		if ( err != CSO_ERR_OK)
		{
			HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_BufferInfo\n", __FUNCTION__, __LINE__);
		}
		else
		{
			stBufferInfo.ulBufferRemainedTime	= stCSOBufferInfo.tBufferStatus.uiRemainBufferSec;
			stBufferInfo.ulBufferPercent		= stCSOBufferInfo.tBufferStatus.uiBufferPercentage;
		}

		if( stCSOBufferInfo.tBufferChunkStatus.uiChunks > 0 )
		{
			/* TODO: will need change Dynamic Chunks */
			if(stCSOBufferInfo.tBufferChunkStatus.uiChunks > 20)
				stCSOBufferInfo.tBufferChunkStatus.uiChunks = 20;

			stBufferInfo.ulChunks = stCSOBufferInfo.tBufferChunkStatus.uiChunks;
			HxLOG_Debug("[%s](L:%d) Decoded TimeStamp :%llu  \n", __FUNCTION__, __LINE__,stCSOBufferInfo.ullLastDecodeTime);
			for(ulIdx=0; ulIdx<stCSOBufferInfo.tBufferChunkStatus.uiChunks; ulIdx++)
			{
				stBufferInfo.ullStartOffsets[ulIdx] = stCSOBufferInfo.tBufferChunkStatus.iStartOffsets[ulIdx];
				stBufferInfo.ullEndOffsets[ulIdx] = stCSOBufferInfo.tBufferChunkStatus.iEndOffsets[ulIdx];
				HxLOG_Debug("[%s](L:%d) Idx:%u Start:%u End:%u  \n", __FUNCTION__, __LINE__, ulIdx, stBufferInfo.ullStartOffsets[ulIdx], stBufferInfo.ullEndOffsets[ulIdx]);
			}
#if	0
			hErr = SVC_CSO_GetPlayPosition(pstMediaPlay->pMediaPlayer, &pullPosition_ms);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("[%s:%d] Error : GetPlayPosition\n", __FUNCTI
						ON__, __LINE__);
			}
#else

#if defined(CONFIG_TIMESTAMP_64)
			pullPosition_ms = pstMediaPlay->stCsoStatus.ullPlayingTime;
#else
			pullPosition_ms = pstMediaPlay->stCsoStatus.uiPlayingTime;
#endif

#endif

			HxLOG_Debug("[%s](L:%d) Playing TimeStamp :%llu (Gap : %llu) \n", __FUNCTION__, __LINE__,pullPosition_ms,stBufferInfo.ullEndOffsets[0] - pullPosition_ms);
			if ( pstMediaPlay->eUnderflowMode == eOM_CSO_UNDERFLOW_THRESHOLD )
			{
				hErr = om_media_play_mediapb_CheckUnderrunThreshold(pstContext,pullPosition_ms,stCSOBufferInfo.ullLastDecodeTime, &stBufferInfo, &bUnderrun);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("[%s](L:%d) CheckUnderrunThreshold Fail\n", __FUNCTION__, __LINE__);
				}
			}
			else if ( pstMediaPlay->eUnderflowMode == eOM_CSO_UNDERFLOW_NONSTOP && pstMediaPlay->stCsoConfig.bProgressiveDownload)
			{
				hErr = om_media_play_mediapb_CheckUnderrunNonStop(pstContext,pullPosition_ms,stCSOBufferInfo.ullLastDecodeTime,&stCSOBufferInfo, &bUnderrun);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("[%s](L:%d) CheckUnderrunNonStop Fail\n", __FUNCTION__, __LINE__);
				}

			}
			else if( pstMediaPlay->eUnderflowMode == eOM_CSO_UNDERFLOW_NONE )
			{
				/* nothing */
			}
			else
			{
				hErr = om_media_play_mediapb_CheckUnderrunThreshold(pstContext,pullPosition_ms,stCSOBufferInfo.ullLastDecodeTime,&stBufferInfo, &bUnderrun);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("[%s](L:%d) CheckUnderrunThreshold Fail\n", __FUNCTION__, __LINE__);
				}
			}

			/* Check Auto Mode Function */
			hErr = om_media_play_mediapb_ControlUnderrun(pstContext, bUnderrun);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("[%s](L:%d) ControlUnderrun Fail\n", __FUNCTION__, __LINE__);
			}

			(void)om_media_play_mediapb_CbOnBufferInfoChanged(ulViewId, ulSessionId, &stBufferInfo);

		}

#if ((CALYPSO_VERSION_MAJOR >= 1) && (CALYPSO_VERSION_MINOR >= 1) && (CALYPSO_VERSION_MICRO >= 18))
		MEDIAPB_IPC_CSO_IPCMSG_MemFree_BufferInfo(&stCSOBufferInfo);
#else
		om_media_play_mediapb_cso_MemFree_BufferInfo(&stCSOBufferInfo);
#endif
		om_media_play_mediapb_cso_ExitSection(pstContext);
	}
	else
	{
		/* eOxMP_MEDIAPLAY_PLAYERTYPE_MSE not emit event */
		pstContext = pstContext;
		hAction = hAction;
		p1 = p1;
		p2 = p2;
		p3 = p3;
	}
#endif
#endif
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtAudioDecoderVolumeChanged(omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eResult = MESSAGE_PASS;
#ifdef CONFIG_SUPPORT_IMMA
	HUINT32  ulActionId = 0, ulViewId = 0, ulSessionId = 0, unVolume = 0;
	HERROR hErr = ERR_OK;
	omMPMediaPlayContext_t *pstMediaPlay = NULL;

	if(pstContext == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		eResult = ERR_BUS_SOURCE_NULL;
		goto func_done;
	}

	if(pstContext->pExtension == NULL)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] pstContext is NULL\033[0m\n", __func__, __LINE__);
		eResult = ERR_BUS_SOURCE_NULL;
		goto func_done;
	}

	ulActionId 	= SVC_PIPE_GetActionId (hAction);
	ulViewId 	= ulActionId - eActionId_VIEW_FIRST;
	ulSessionId = pstContext->ulSessionId;

	pstMediaPlay = (omMPMediaPlayContext_t *)(pstContext->pExtension);

	hErr = SVC_CSO_GetAudioDecoderVolume(pstMediaPlay->pMediaPlayer, &unVolume);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to get audio decoder volume, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
		eResult = ERR_BUS_SOURCE_NULL;
		goto func_done;
	}

	pstContext->stCache.unAudioDecoderVolume = unVolume;

	om_media_play_mediapb_CbOnAudioDecoderVolumeChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, unVolume);

func_done:
#endif	/* CONFIG_SUPPORT_IMMA */
	return eResult;
}

STATIC BUS_Result_t om_media_play_mediapb_EvtOutputControl (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#ifdef CONFIG_SUPPORT_IMMA
	CSO_ERR 		err;
	CSO_OutputControl_t	tOutputControl;
	omMPMediaPlayContext_t			*pstMediaPlay	=	NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);


	HxSTD_MemSet(&tOutputControl, 0, sizeof(CSO_OutputControl_t));

	err = MEDIAPB_IPC_CSO_GetOutputControl(pstMediaPlay->ulCsoSessionId, &tOutputControl );
	if ( err != CSO_ERR_OK)
	{
		HxLOG_Error("GetOutputLevelInfo Failed!!!\n");
		return MESSAGE_BREAK;
	}

	(void)BUS_PostData(NULL, eMEVT_MEDIA_OUTPUT_CONTROL, 0, 0, 0, (void*) &tOutputControl, sizeof(omMPMediaPlayContext_t));
#else
	HERROR							 hErr = ERR_OK;
	omMPMediaPlayContext_t			*pstMediaPlay	=	NULL;
	SvcMp_OutputControl_t	 outputControl;

	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);


	HxSTD_MemSet(&outputControl, 0, sizeof(outputControl));


	hErr = SVC_MP_GetOutputControlInfo(pstMediaPlay->pMediaPlayer, &outputControl);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("om_media_play_mediapb_mwMpEventOutputControl: SVC_MP_GetOutputControlInfo failed.\n");
		return MESSAGE_BREAK;
	}

	SVC_OUT_SetHdcp(pstContext->ulActionId, outputControl.bHdcp);

	if ( outputControl.bScms == TRUE )
	{
		SVC_OUT_SetScmsCopyRight(pstContext->ulActionId, eCOPYRIGHT_COPY_NEVER);
	}

	if ( outputControl.bCgms == TRUE )
	{
		SVC_OUT_SetCgmsMacrovision(pstContext->ulActionId, eMACROVISION_4LINE);
	}

	if ( outputControl.bMacrovision == TRUE )
	{
		SVC_OUT_SetMacrovision(pstContext->ulActionId, eMACROVISION_4LINE);
	}
#endif
	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_MsgNotifyUnplugged (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
#ifdef CONFIG_SUPPORT_IMMA
	HERROR	 hErr = ERR_OK;
#endif
	HUINT32	 ulActionId 	 =	 SVC_PIPE_GetActionId (hAction);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

#ifdef CONFIG_SUPPORT_IMMA
	if ((ulActionId == pstContext->ulActionId) && (TRUE == pstContext->bActionTaken))
	{
		//SvcFs_DevInfo_t	stMwDevInfo = { 0, };
		SvcFs_DevInfo_t * pstDevInfo = (SvcFs_DevInfo_t *)p3;

		HxLOG_Critical("MGR_MEDIACTRL_MEDIAPB: Device Unplug: URL=%s\n", pstContext->stCache.stMediaPlay.szUrl);
		if ( HxSTD_StrNCmp(pstContext->stCache.stMediaPlay.szUrl, "file", 4) != 0 )
		{
			return MESSAGE_PASS;
		}

		if ( NULL == pstMediaPlay->pMediaPlayer || eOxMP_PLAYSTATE_PLAYING != pstContext->stCache.eState)
		{
			return MESSAGE_PASS;
		}

#if 0
		if ( SVC_FS_GetDeviceInfo((SvcFs_DevIdx_e)p1, &stMwDevInfo) != ERR_OK )
		{
			return MESSAGE_PASS;
		}
#endif
		HxLOG_Critical("MGR_MEDIACTRL_MEDIAPB: Mount Path=%s\n", pstDevInfo->szMountPath);
		if ( HxSTD_StrStr(pstContext->stCache.stMediaPlay.szUrl, pstDevInfo->szMountPath) )
		{
			HxLOG_Critical("MGR_MEDIACTRL_MEDIAPB: auto stop by external device unplug.\n");
			om_media_play_mediapb_cso_EnterSection(pstContext);

			if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_PROBING || pstMediaPlay->stCsoStatus.eStatus == eCSO_PROBED )
			{
				pstMediaPlay->bStartRequested = FALSE;
				HxLOG_Error ("CSO is now probing... cancel StartRequest!!! \n" );
				hErr = om_media_play_mediapb_cso_CancelProbeRequest(pstContext);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("om_media_play_mediapb_cso_CancelProbeRequest failed \n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return hErr;
				}
				pstMediaPlay->stCsoStatus.eStatus = eCSO_PROBE_CANCELED;
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return ERR_OK;
			}

			hErr = om_media_play_mediapb_cso_Stop(pstContext);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("om_media_play_mediapb_cso_Stop failed \n");
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return hErr;
			}
			pstContext->stCache.eState = eOxMP_PLAYSTATE_STOPPED;

			om_media_play_mediapb_cso_ExitSection(pstContext);
		}
	}
#else

	if ((ulActionId == pstContext->ulActionId) && (TRUE == pstContext->bActionTaken))
	{
		//SvcFs_DevInfo_t	stMwDevInfo = { 0, };
		SvcFs_DevInfo_t * pstDevInfo = (SvcFs_DevInfo_t *)p3;
		HINT8 *url = OxMGR_Malloc(2048);
		if ( !url ) return MESSAGE_PASS;

		HxSTD_MemSet(url, 0, 2048);
		SVC_MP_GetPlayUrl(pstMediaPlay->pMediaPlayer, url);
		HxLOG_Critical("MGR_MEDIACTRL_MEDIAPB: Device Unplug: URL=%s\n", url);
		if ( HxSTD_StrNCmp(url, "file", 4) != 0 )
		{
			OxMGR_Free(url);
			return MESSAGE_PASS;
		}

		if ( NULL == pstMediaPlay->pMediaPlayer || eOxMP_PLAYSTATE_PLAYING != pstContext->stCache.eState )
		{
			OxMGR_Free(url);
			return MESSAGE_PASS;
		}
#if 0
		if ( SVC_FS_GetDeviceInfo((SvcFs_DevIdx_e)p1, stMwDevInfo) != ERR_OK )
		{
			OxMGR_Free(url);
			return MESSAGE_PASS;
		}
#endif
		HxLOG_Critical("MGR_MEDIACTRL_MEDIAPB: Mount Path=%s\n", pstDevInfo->szMountPath);
		if ( HxSTD_StrStr(url, pstDevInfo->szMountPath) )
		{
			HxLOG_Critical("MGR_MEDIACTRL_MEDIAPB: auto stop by external device unplug.\n");
			MGR_MPlayer_Stop(pstMediaPlay->pMediaPlayer);
			pstContext->stCache.eState = eOxMP_PLAYSTATE_STOPPED;//pstContext->bPlayed = FALSE;
		}

		OxMGR_Free(url);
	}
#endif

	return MESSAGE_PASS;
}

STATIC BUS_Result_t om_media_play_mediapb_MsgDbParamUpdated (omMediaPlay_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR	 					hErr = ERR_OK;
	BUS_Result_t			 eResult = MESSAGE_PASS;
	HUINT32	 			ulActionId 	 = SVC_PIPE_GetActionId (hAction);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HUINT32 				ulViewId = 	0;
	HUINT32				ulSessionId  =	0;

	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	ulSessionId		=	pstContext->ulSessionId;

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if ((ulActionId == pstContext->ulActionId) && (TRUE == pstContext->bActionTaken))
	{
		if (NULL == pstMediaPlay->pMediaPlayer)
		{
			HxLOG_Error("No pMediaPlayer instance!!!\n");
			return MESSAGE_PASS;
		}


		/* process a subtitle language change*/
		if( p1 == eDB_PARAM_ITEM_LANG_SUBTITLE0 )
		{
			HERROR			hErr;
			HxLANG_Id_e 	eSubTitleLangId 		= eLangID_MAX;
			HxLANG_Id_e 	eTrackLangId1			= eLangID_MAX;
			OxMediaPlay_SubtitleCompTable_t *pstSubtitleCompTable = &pstContext->stCache.stSubtitleCompTable;
			HUINT32 		tracks = 0;

			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0, (HUINT32 *)&eSubTitleLangId, 0);
			if ((hErr == ERR_OK) && (eSubTitleLangId != eLangID_MAX))
			{
			#ifdef CONFIG_SUPPORT_IMMA
				om_media_play_mediapb_cso_EnterSection(pstContext);
			#endif
				pstSubtitleCompTable->nCurrIndex = SUBTITLE_OFF; // OFF로 설정해 놓고
				for( tracks = 0; tracks < pstSubtitleCompTable->nArraySize; tracks++ )
				{
					// Check the language number:
					hErr = MWC_UTIL_GetLangIDBy639Code (pstSubtitleCompTable->astCompArray[tracks].utArg.stDvbInfo.szLangCode, &eTrackLangId1);
					if ( hErr != ERR_OK )
					{
						HxLOG_Error ("[%s](L:%d) Fail To get track subtitle language.... \n", __FUNCTION__, __LINE__ );
						eTrackLangId1 = eLangID_MAX;
					}

					if((eSubTitleLangId==eTrackLangId1))
					{
						HxLOG_Debug ("[%s](L:%d) Subtitle Current Index: %d \n", __FUNCTION__, __LINE__, tracks);
						pstSubtitleCompTable->nCurrIndex=tracks;
						break;
					}
				}
			#ifdef CONFIG_SUPPORT_IMMA
				om_media_play_mediapb_cso_ExitSection(pstContext);
			#endif
			}
			else
			{
				HxLOG_Error ("[%s](L:%d) Fail To get system subtitle language.... \n", __FUNCTION__, __LINE__ );
			}

			/*DVB-Subtitle Change (Stop/Start)*/
			MGR_MPlayer_DVB_Sbtl_Stop(pstMediaPlay->pMediaPlayer);
			if (pstSubtitleCompTable->nArraySize > 0 && pstSubtitleCompTable->nCurrIndex >=0 )
			{
				HINT32 nIndex = pstSubtitleCompTable->nCurrIndex;
				MGR_MPlayer_DVB_Sbtl_Start(pstMediaPlay->pMediaPlayer, (HUINT16)pstSubtitleCompTable->astCompArray[nIndex].nPid,
				pstSubtitleCompTable->astCompArray[nIndex].utArg.stDvbInfo.nCompositionPageId, pstSubtitleCompTable->astCompArray[nIndex].utArg.stDvbInfo.nAncillaryPageId);
			}

			/* Subtitle Track이 변경 되었다고, MHEG에 알림. */
			om_media_play_CbOnComponentSubtitleChanged (ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, pstSubtitleCompTable );

		}
		/* process a audio language change*/
		else if (p1 == eDB_PARAM_ITEM_LANG_AUDIO0)
		{
			HxLANG_Id_e 	eAudioLangId 		    = eLangID_MAX;
			HxLANG_Id_e 	eTrackLangId1			= eLangID_MAX;
			OxMediaPlay_AudioCompTable_t *pstAudioCompTable	= &pstContext->stCache.stAudioCompTable;
			HUINT32 		tracks = 0;

			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0, (HUINT32 *)&eAudioLangId, 0);
			if ((hErr == ERR_OK) && (eAudioLangId != eLangID_MAX))
			{
				/* 만일 Preferred Lang에 맞는 audio가 없으면 변경 되지 않는다. */
				for( tracks = 0; tracks < pstAudioCompTable->nArraySize; tracks++ )
				{
					hErr = MWC_UTIL_GetLangIDBy639Code (pstAudioCompTable->astCompArray[tracks].astLangInfo[0].szLangCode, &eTrackLangId1);
					if ( hErr != ERR_OK )
					{
						HxLOG_Error ("[%s](L:%d) Fail To get track subtitle language.... \n", __FUNCTION__, __LINE__ );
						eTrackLangId1 = eLangID_MAX;
					}
					if(eAudioLangId == eTrackLangId1)
					{
					#ifdef CONFIG_SUPPORT_IMMA
						HxLOG_Debug ("[%s](L:%d) Audio Current Index : %d \n", __FUNCTION__, __LINE__, tracks);
						om_media_play_mediapb_cso_EnterSection(pstContext);
						pstAudioCompTable->nCurrIndex = tracks;
						om_media_play_mediapb_cso_ExitSection(pstContext);
						/* change audio track*/
						om_media_play_mediapb_cso_SetAudioTrack(ulActionId, pstAudioCompTable->nCurrIndex);
						/* Audio Track이 변경 되었다고, MHEG에 알림. */
						om_media_play_CbOnComponentAudioChanged(ulViewId, ulSessionId, eOxMP_MEDIATYPE_MEDIAPLAY, pstAudioCompTable );
					#endif
						break;
					}
				}
			}
			else
			{
				HxLOG_Error ("[%s](L:%d) Fail To get system audio language.... \n", __FUNCTION__, __LINE__ );
			}
		}
	}

	return eResult;
}


#define ___MEMBER_FUNCTIONS___
BUS_Result_t proc_om_media_play_mediapb_Proto (omMediaPlay_Context_t *pstContext, HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t			 eResult = MESSAGE_PASS;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	if (NULL == pstContext)	{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

#ifdef CONFIG_SUPPORT_IMMA
	om_media_play_mediapb_cso_PrintProcEvent(pstContext, (SvcCso_Event_e)message);
#endif

	switch (message)
	{
#ifdef CONFIG_SUPPORT_IMMA
		case eSVC_CSO_EVENT_CONNECTING:
		case eSVC_CSO_EVENT_PLAYING:
		case eSVC_CSO_EVENT_PAUSE:
		case eSVC_CSO_EVENT_STOPPED:
		case eSVC_CSO_EVENT_PROBE_FINISHED:
		case eSVC_CSO_EVENT_PROBE_FAILED:
		case eSVC_CSO_EVENT_SEEK_ERROR:
		case eSVC_CSO_EVENT_BEGIN_OF_STREAM:
		case eSVC_CSO_EVENT_PLAY_SPEED_CHANGED:
		case eSVC_CSO_EVENT_BUFFERING_CANPLAY:
		case eSVC_CSO_EVENT_BUFFERING_NOT_ENOUGH:
		case eSVC_CSO_EVENT_BUFFERING_ENOUGH:
		case eSVC_CSO_EVENT_BUFFERING_FINISHED:
		case eSVC_CSO_EVENT_OUTPUT_CONTROL:
			HxLOG_Error ("these events should not comes from SVC_CSO\n");
			break;
		case eSVC_CSO_EVENT_BUFFERING_UPDATED:
			// move to eSVC_CSO_EVENT_PLAY_POSITION_CHANGED
			//eResult = om_media_play_mediapb_EvtBufferingUpdate (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_CSO_EVENT_ERROR:
			eResult = om_media_play_mediapb_EvtError (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_CSO_EVENT_END_OF_STREAM:
			#ifdef HLS_DISCONTINUITY
			if( pstMediaPlay->bNeedRestart == TRUE )
			{
				pstMediaPlay->bNeedRestart = FALSE;
				om_media_play_mediapb_cso_Reset( pstContext );
			}
			else
			{
				eResult = om_media_play_mediapb_EvtEndOfStream (pstContext, hHandle, p1, p2, p3);
			}
			#else
			eResult = om_media_play_mediapb_EvtEndOfStream (pstContext, hHandle, p1, p2, p3);
			#endif
			break;
		case eSVC_CSO_EVENT_PLAY_POSITION_CHANGED:
			eResult = om_media_play_mediapb_EvtPlayPositionChanged (pstContext, hHandle, p1, p2, p3);
			eResult = om_media_play_mediapb_EvtBufferingUpdate (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_CSO_EVENT_SBTL_START:
			eResult = om_media_play_mediapb_EvtSBTLStart (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_CSO_EVENT_SBTL_STOP:
			eResult = om_media_play_mediapb_EvtSBTLStop (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_CSO_EVENT_SBTL_UPDATE:
			eResult = om_media_play_mediapb_EvtSBTLUpdate (pstContext, hHandle, p1, p2, p3);
			break;
#ifdef IFRAME_YOUTUBE_WORK_ARROUND
		case eSVC_CSO_EVENT_VIDEO_NEW_FRAME:
#if 0		// In case of HMX Player (generic media play), playing event will be triggered by new frame event.
			if( s_stMediaPlayerCache[CalcActionId(pstContext->ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE ||
					s_stMediaPlayerCache[CalcActionId(pstContext->ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT)
#endif
			{
				om_media_play_mediapb_EvtPlaying (pstContext, hHandle, 0, 0, 0);
			}

			eResult = om_media_play_mediapb_EvtNewFrame(pstContext, hHandle, p1, p2, p3);
			eResult = om_media_play_mediapb_EvtAVResume(pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_CSO_EVENT_VIDEO_UNDERRUN:
			if( s_stMediaPlayerCache[CalcActionId(pstContext->ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE )
			{
				om_media_play_mediapb_EvtBufferingNotEnough (pstContext, hHandle, 0, 0, 0);
			}
			eResult = om_media_play_mediapb_EvtAVUnderrun(pstContext, hHandle, p1, p2, p3);
			break;

		case eSVC_CSO_EVENT_AUDIO_NEW_FRAME:
#if 0		// In case of HMX Player (generic media play), playing event will be triggered by new frame event.
			if( s_stMediaPlayerCache[CalcActionId(pstContext->ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE ||
					s_stMediaPlayerCache[CalcActionId(pstContext->ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT)
#endif
			{
				om_media_play_mediapb_EvtPlaying (pstContext, hHandle, 0, 0, 0);
			}
			eResult = om_media_play_mediapb_EvtAVResume(pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_CSO_EVENT_AUDIO_UNDERRUN:
			if( s_stMediaPlayerCache[CalcActionId(pstContext->ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE )
			{
				om_media_play_mediapb_EvtBufferingNotEnough (pstContext, hHandle, 0, 0, 0);
			}
			eResult = om_media_play_mediapb_EvtAVUnderrun(pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_CSO_EVENT_AUDIO_DECODER_VOLUME_CHANGED:
			if( s_stMediaPlayerCache[CalcActionId(pstContext->ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE )
			{
				om_media_play_mediapb_EvtAudioDecoderVolumeChanged (pstContext, hHandle, 0, 0, 0);
			}
			break;
#endif
#else
		case eSVC_MP_EVENT_CONNECTING:
			eResult = om_media_play_mediapb_EvtConnecting (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_PLAYING:
			eResult = om_media_play_mediapb_EvtPlaying (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_PAUSE:
			eResult = om_media_play_mediapb_EvtPause (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_STOPPED:
			eResult = om_media_play_mediapb_EvtStopped (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_PROBE_FAILED:		// failed to probe contents
			eResult = om_media_play_mediapb_EvtProbeFailed (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_ERROR: 			// failed to start playback
			eResult = om_media_play_mediapb_EvtError (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_SEEK_ERROR:				// failed to start playback
			eResult = om_media_play_mediapb_EvtSeekError (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_BEGIN_OF_STREAM:
			eResult = om_media_play_mediapb_EvtBeginOfStream (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_END_OF_STREAM:
			eResult = om_media_play_mediapb_EvtEndOfStream (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_PLAY_POSITION_CHANGED:
			eResult = om_media_play_mediapb_EvtPlayPositionChanged (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_PLAY_SPEED_CHANGED:
			eResult = om_media_play_mediapb_PlaySpeedChanged (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_SBTL_START:
			eResult = om_media_play_mediapb_EvtSBTLStart (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_SBTL_STOP:
			eResult = om_media_play_mediapb_EvtSBTLStop (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_SBTL_UPDATE:
			eResult = om_media_play_mediapb_EvtSBTLUpdate (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_BUFFERING_CANPLAY:
			eResult = om_media_play_mediapb_EvtBufferingCanplay (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_BUFFERING_NOT_ENOUGH:
			eResult = om_media_play_mediapb_EvtBufferingNotEnough (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_BUFFERING_ENOUGH:
			eResult = om_media_play_mediapb_EvtBufferingEnough (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_BUFFERING_FINISHED:
			eResult = om_media_play_mediapb_EvtBufferingFinished (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_BUFFERING_UPDATED:
			eResult = om_media_play_mediapb_EvtBufferingUpdate (pstContext, hHandle, p1, p2, p3);
			break;
		case eSVC_MP_EVENT_OUTPUT_CONTROL:
			//eResult = om_media_play_mediapb_EvtOutputControl (pstContext, hHandle, p1, p2, p3);
			break;
#endif
		case eMEVT_UMMA_NOTIFY_STORAGE_DISCONNECTED :
			eResult = om_media_play_mediapb_MsgNotifyUnplugged (pstContext, hHandle, p1, p2, p3);
			break;

		case eMEVT_MEDIA_SEND_DRM_INFORMATION:
			//proc_om_media_play_Proto() 에서 pstContext->stOmCache.stDrmInformation에 setting 해 놓았음.
			// 즉 여기서 pstContext->stOmCache.stDrmInformation 를 IMMA 로 set.
			break;
		case eSEVT_DB_PARAM_UPDATED:
			eResult= om_media_play_mediapb_MsgDbParamUpdated(pstContext, hHandle, p1, p2, p3);
			break;

		default:
			eResult = MESSAGE_PASS;
			break;
	}

	return (MESSAGE_BREAK == eResult) ? MESSAGE_BREAK : MESSAGE_PASS;

}


#ifdef CONFIG_SUPPORT_IMMA
static unsigned char testDtcpKey[348] = {
	0x01, 0x10, 0x02, 0xBC, 0x00, 0x05, 0x80, 0x84, 0x3A, 0x1F, 0xE0, 0x52,
	0xC4, 0x58, 0x42, 0x9E, 0x8A, 0xA7, 0xB5, 0xBF, 0x44, 0x1B, 0xF0, 0xDF,
	0xF0, 0x2F, 0x3E, 0x71, 0x27, 0x1C, 0x91, 0xD4, 0x2E, 0x52, 0x0F, 0xF1,
	0xDD, 0xB7, 0xA4, 0x1C, 0xF5, 0xE7, 0x6A, 0x72, 0x45, 0x3B, 0xA8, 0x49,
	0x01, 0xEE, 0xE7, 0xA7, 0x31, 0x7A, 0xF1, 0x7B, 0x46, 0x95, 0xC0, 0x78,
	0x65, 0x2A, 0x05, 0xE1, 0x3D, 0xF3, 0x7A, 0x08, 0x01, 0x8B, 0x5C, 0x01,
	0x25, 0xA4, 0xA7, 0xDF, 0x9E, 0xEC, 0xC0, 0xC6, 0x5D, 0xB6, 0xB1, 0xBC,
	0x1E, 0x22, 0xCA, 0x74, 0x00, 0x49, 0x38, 0xD3, 0x9B, 0x7B, 0x2D, 0x07,
	0x3E, 0xBF, 0x60, 0x89, 0x94, 0xB3, 0x82, 0x18, 0xCB, 0x2B, 0xCF, 0x56,
	0x6E, 0x6E, 0x94, 0xC4, 0x5D, 0xBE, 0xDD, 0x46, 0x7E, 0x42, 0xB7, 0x89,
	0xCC, 0xE9, 0x5C, 0xD1, 0x9A, 0xBC, 0xC2, 0xCC, 0x35, 0xD2, 0x6B, 0xC0,
	0xDF, 0xF1, 0xDF, 0xEB, 0x28, 0x57, 0x08, 0x8E, 0x05, 0x2C, 0x5D, 0x9B,
	0x2B, 0x33, 0x9A, 0xD5, 0x94, 0xA8, 0xE1, 0x6A, 0xAD, 0x4F, 0x50, 0xEF,
	0xE2, 0x9B, 0x9D, 0x5C, 0x0C, 0x0B, 0x6D, 0x48, 0x1B, 0x27, 0xDE, 0x53,
	0x94, 0xA8, 0xE1, 0x6A, 0xAD, 0x4F, 0x50, 0xEF, 0xE2, 0x9B, 0x9D, 0x5C,
	0x0C, 0x0B, 0x6D, 0x48, 0x1B, 0x27, 0xDE, 0x50, 0x4F, 0xD9, 0x18, 0xE5,
	0xB0, 0xC4, 0x51, 0x3F, 0xA3, 0xEF, 0x30, 0x33, 0xFE, 0x08, 0xF0, 0x7C,
	0x3B, 0x08, 0xC7, 0x6D, 0x02, 0x27, 0x8C, 0x8A, 0xD2, 0x47, 0xA4, 0x6B,
	0x5C, 0x53, 0xDE, 0x4C, 0xC9, 0xA0, 0x07, 0x24, 0x88, 0xF0, 0x28, 0xA3,
	0x78, 0x66, 0x89, 0x13, 0xDE, 0xAB, 0x22, 0x75, 0x9E, 0x58, 0x7E, 0xB3,
	0x93, 0xD7, 0xDA, 0x46, 0xCE, 0x24, 0xB1, 0x96, 0x4D, 0x2B, 0xA9, 0x96,
	0xA7, 0xE3, 0xD9, 0xAF, 0x4D, 0x26, 0x6A, 0xFE, 0x00, 0x44, 0x3B, 0x95,
	0xD6, 0x87, 0x06, 0x06, 0xA3, 0x80, 0x6F, 0x97, 0x16, 0x35, 0xC1, 0x79,
	0x6D, 0xEA, 0xED, 0x24, 0x28, 0xB5, 0x22, 0xC7, 0xCD, 0xB2, 0x2D, 0x49,
	0x0E, 0xF8, 0xE3, 0xD4, 0x7C, 0x0D, 0x87, 0xC7, 0x6C, 0x19, 0x56, 0x62,
	0xF9, 0x8C, 0x14, 0xE6, 0xE6, 0xED, 0x3E, 0x30, 0x78, 0xFF, 0x0E, 0x5B,
	0x3A, 0x27, 0x3C, 0xCB, 0x45, 0xB1, 0x46, 0xA0, 0x61, 0x1A, 0x50, 0x1A,
	0xC3, 0x2B, 0xFA, 0xD4, 0xDE, 0x66, 0x5A, 0xDE, 0xC0, 0x15, 0x61, 0x12,
	0xCF, 0x26, 0x28, 0xEB, 0x95, 0xDC, 0x3A, 0x44, 0x90, 0x28, 0xEB, 0x3C
};
#endif

HERROR om_media_play_mediapb_Init(void)
{
#ifdef CONFIG_SUPPORT_IMMA
	CSO_ERR 		err;
	CSO_Settings_t 	cso_settings;
	HERROR 			hResult;
	SvcCso_Resource_t* pMap = NULL;
	CSO_DrmInfo_t drminfo;

	HUINT8 * pDtcpKey = NULL;
	HUINT32 SizeOfDtcpKey;
	HUINT32	ulActionId = 0;
	omMediaPlay_Context_t	*pstContext	= om_media_play_GetContext (ulActionId);

	/* these for IPC */
	hResult = MEDIAPB_IPC_Create(48629);
	if ( hResult < 0 )
	{
		HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_Create err = %d\n", __FUNCTION__, __LINE__, hResult);
	}

	HxSTD_MemSet (&cso_settings, 0, sizeof(CSO_Settings_t));

	err = MEDIAPB_IPC_CSO_GetDefaultSettings(&cso_settings);
	if ( err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_GetDefaultSettings\n", __FUNCTION__, __LINE__);
	}

	cso_settings.cbErrorEvent 			= CSO_Error_Callback;
	cso_settings.cbMediaEvent 			= CSO_Media_Callback;
#ifdef  CONFIG_IMMA_SESSION_MAX
        cso_settings.nMaxSessions 			= CONFIG_IMMA_SESSION_MAX;
        HxLOG_Info("[%s](L:%d) cso session max : %d\n", __FUNCTION__, __LINE__, cso_settings.nMaxSessions);
#endif

	err = SVC_CSO_AcquireDtcpIpKey((HINT8**)&pDtcpKey, (HINT32 *)&SizeOfDtcpKey);
	if ( err != ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Error - SVC_CSO_AcquireDtcpIpKey\n", __FUNCTION__, __LINE__);
	}

	if( NULL == pDtcpKey )
	{
		HxLOG_Error("[%s](L:%d) Error \n", __FUNCTION__, __LINE__);
		goto EXIT_PROC;
	}


	if ( pDtcpKey[0] == 0 && pDtcpKey[1] == 0 && pDtcpKey[346] == 0 && pDtcpKey[347] == 0)
	{
		cso_settings.pDtcpKey = testDtcpKey;
		HxLOG_Error("[%s](L:%d) DTCPKEY 0x%02X, 0x%02X ------ 0x%02X, 0x%02X \n", __FUNCTION__, __LINE__, testDtcpKey[0], testDtcpKey[1], testDtcpKey[346], testDtcpKey[347]);
	}
	else
	{
		cso_settings.pDtcpKey = pDtcpKey;
		HxLOG_Debug("[%s](L:%d) DTCPKEY 0x%02X, 0x%02X ------ 0x%02X, 0x%02X \n", __FUNCTION__, __LINE__, pDtcpKey[0], pDtcpKey[1], pDtcpKey[346], pDtcpKey[347]);
	}

	HxLOG_Debug("[%s](L:%d) CSO_Init\n", __FUNCTION__, __LINE__);

	err = MEDIAPB_IPC_CSO_Init(&cso_settings);

	if ( err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Error - CSO_Init err = %d\n", __FUNCTION__, __LINE__, err);
	}

	/*************************
	 *	Resource Mapping
	 *************************/
	HxLOG_Debug("SVC_CSO_GetResourceMap\n");

	hResult = SVC_CSO_GetResourceMap(&pMap);
	if(hResult != ERR_OK)
	{
		HxLOG_Error("SVC_CSO_GetResourceMap failed!\n");
		return ERR_FAIL;
	}
	HxLOG_Debug("MEDIAPB_IPC_CSO_SetResourceMap\n");

	err = MEDIAPB_IPC_CSO_SetResourceMap((void*)pMap);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_SetResourceMap failed!\n");
		return ERR_FAIL;
	}

#if 1
	HxSTD_memset(&drminfo,0x0,sizeof(drminfo));
	err = MEDIAPB_IPC_CSO_DrmInfo(0,&drminfo);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_DrmInfo failed!\n");
	}
	if(drminfo.vmxInfo.unVmxUniqueIdLen > 0 && drminfo.vmxInfo.pVmxUniqueId != NULL)
	{
		if(pstContext != NULL)
		{
			HxSTD_memset(pstContext->stCache.szVmxIdentifier,0x0,sizeof(pstContext->stCache.szVmxIdentifier));
			HxSTD_memcpy(pstContext->stCache.szVmxIdentifier,drminfo.vmxInfo.pVmxUniqueId,drminfo.vmxInfo.unVmxUniqueIdLen);
			HLIB_STD_StrNCpySafe(pstContext->stCache.szVmxVersion,drminfo.vmxInfo.pVmxVersion,128);
		}
	}
	if(drminfo.vmxInfo.pVmxUniqueId != NULL)
	{
		free(drminfo.vmxInfo.pVmxUniqueId);
	}
	if(drminfo.vmxInfo.pVmxVersion != NULL)
	{
		free(drminfo.vmxInfo.pVmxVersion);
	}
	if(drminfo.nagraPRMInfo.prmsyntax != NULL)
	{
		free(drminfo.nagraPRMInfo.prmsyntax);
	}
#else
	HxSTD_memset(&drminfo,0x0,sizeof(drminfo));
	err = MEDIAPB_IPC_CSO_DrmInfo(&drminfo);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_DrmInfo failed!\n");
	}
	if(drminfo.unVmxUniqueIdLen > 0 && drminfo.pVmxUniqueId != NULL)
	{
		if(pstContext != NULL)
		{
			HxSTD_memset(pstContext->stCache.szVmxIdentifier,0x0,sizeof(pstContext->stCache.szVmxIdentifier));
			HxSTD_memcpy(pstContext->stCache.szVmxIdentifier,drminfo.pVmxUniqueId,drminfo.unVmxUniqueIdLen);
		}
	}
	if(drminfo.pVmxUniqueId != NULL)
	{
		free(drminfo.pVmxUniqueId);
	}
#endif
EXIT_PROC:

	if ( NULL != pDtcpKey )
	{
		free(pDtcpKey);
	}


	if ( NULL != pMap )
	{
		OxMGR_Free(pMap);
	}


#endif

	//(void)mgr_appmgr_mctrl_mediapb_mwMpSubttlStopped;
	return ERR_OK;
}

STATIC void om_media_play_mediapb_cso_DestroyMediaPlay(omMediaPlay_Context_t *pstContext)
{
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	if (NULL == pstContext)					{ return ; }
	if (NULL == pstContext->pExtension)		{ return ; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

#ifdef CONFIG_SUPPORT_IMMA
	if (pstMediaPlay->stCsoStatus.uiMediaCtrlSemaId)
	{
		HUINT32 semaid = 0;
		semaid = om_media_play_mediapb_cso_GetSemaphoreId(pstContext);
		if ( !HLIB_LIST_Find(s_pRemoveSemaList, (void*)semaid) )
		{
			s_pRemoveSemaList = HLIB_LIST_Append(s_pRemoveSemaList, (void *)semaid);
		}
	}
#endif

	return;
}

HERROR om_media_play_mediapb_InitAction(omMediaPlay_Context_t *pstContext)
{
	omMPMediaPlayContext_t 	*pstMediaPlay;
	HUINT32					ulActionId;

	ulActionId = pstContext->ulActionId;

	pstContext->eMediaType = eOxMP_MEDIATYPE_MEDIAPLAY ;

//	pstMediaPlay = (omMPMediaPlayContext_t*)OxMGR_Malloc( sizeof(omMPMediaPlayContext_t) );
#ifdef CONFIG_SUPPORT_IMMA
	pstMediaPlay = &s_stMediaPlayer[CalcActionId(ulActionId)];

	/* Stop 없이 InitAction 이 내려오는 경우 STOP 이 필요하다... extention 도 check! */
	if ( (pstMediaPlay->stCsoStatus.eStatus >= eCSO_PLAYING) && pstContext->pExtension )
	{
		if (ERR_OK != om_media_play_mediapb_cso_Stop(pstContext))
		{
			HxLOG_Error ("om_media_play_mediapb_cso_Stop failed \n");
			return ERR_FAIL;
		}
		pstContext->stCache.eState = eOxMP_PLAYSTATE_STOPPED;	//pstContext->bPlayed = FALSE;
	}

	/* Extention 정리 */
	(void)om_media_play_mediapb_cso_DestroyMediaPlay(pstContext);

	HxSTD_MemSet(pstMediaPlay, 0x00, sizeof(omMPMediaPlayContext_t));

	if(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_NONE)
	{
		s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType = eOxMP_MEDIAPLAY_PLAYERTYPE_HMX;
	}
#endif
	pstContext->pExtension	= (void *)pstMediaPlay;

	pstMediaPlay->pMediaPlayer	= MGR_MPlayer_Create (pstContext->ulActionId);
	HxLOG_Error ("pstMediaPlay->pMediaPlayer %p \n", pstMediaPlay->pMediaPlayer);

	if (NULL == pstMediaPlay->pMediaPlayer)
	{
		HxLOG_Error ("MGR_MPlayer_Create Failed\n");
		return ERR_FAIL;
	}

#ifdef CONFIG_SUPPORT_IMMA
	pstMediaPlay->stCsoStatus.eStatus = eCSO_NONE;
	pstMediaPlay->ulMaxDecoderTrick = 400; /* 400 means 4X */
	pstMediaPlay->bSupport_seek = TRUE;
	pstMediaPlay->bSupport_trick = TRUE;

	pstMediaPlay->bUnderflowAutoMode = s_stMediaPlayerCache[CalcActionId(ulActionId)].bUnderflowAutoMode;
	pstMediaPlay->nUnderflowThreshold = s_stMediaPlayerCache[CalcActionId(ulActionId)].nUnderflowThreshold;

	HxLOG_Debug ("BufferingScenario auto = %d, threshold = %d)\n", pstMediaPlay->bUnderflowAutoMode, pstMediaPlay->nUnderflowThreshold);

	s_pRemoveSemaList = HLIB_LIST_RemoveAllFunc(s_pRemoveSemaList, om_media_play_mediapb_destroySemaphore );
	/* TODO name of string should be changed for multi context, ex. "mctrlCtxSema0", "mctrlCtxSema1" */
	HxSEMT_Create(&pstMediaPlay->stCsoStatus.uiMediaCtrlSemaId, "mctrlCtxSema", HxSEMT_FIFO);

	/* Config of DNP */
	if(pstMediaPlay->bUnderflowAutoMode == TRUE)
	{
		pstMediaPlay->eUnderflowMode = eOM_CSO_UNDERFLOW_NONSTOP;
		HxLOG_Debug("Set NONSTOP MODE \n");
	}
	else
	{
		pstMediaPlay->eUnderflowMode = eOM_CSO_UNDERFLOW_THRESHOLD;
		HxLOG_Debug("Set THRESHOLD MODE \n");
	}

	if (pstMediaPlay->pMediaPlayer)
	{
		(void)SVC_CSO_SetInputStreamType(pstMediaPlay->pMediaPlayer, om_media_play_convert_svc_input_streamptype(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType));
	}
#endif
	// Set the BUS-message receiver
//		MGR_APPMGR_AppendProcItem ((void *)pstMediaPlay, mgr_appmgr_mctrl_mediapb_proc);

	return ERR_OK;
}

/**
* @b Function @b Description <br>
* this function makes Stopped --> Probing
*
* @param[in] omMediaPlay_Context_t   : media pb context
* @param[in] ulActionId   : ActionID
* @param[in] pstProbe   : Probeinfo
*
* @return  #  returns ERR_OK or ERR_FAIL
*
* @attention don't call any functions with om_media_play_mediapb_cso_* named, or State machine may corrupted!
*/
HERROR om_media_play_mediapb_cso_prepareSession(HCHAR* pszUri, HCHAR* pSaveFilePath, OmMedia_MediapbInfo_t *pMediaInfo)
{
#ifdef CONFIG_SUPPORT_IMMA
	CSO_ERR 		err;
	HERROR			hErr;
	CSO_Settings_t *pSettings = NULL;
	CSO_Settings_t dlnaSettings;
	HCHAR * parsedUrl = NULL;
	HCHAR * dlnaUrl = NULL;
	HCHAR * airplayUrl = NULL;
	HUINT32			ulUALen = 0;
	SvcCso_Config_t stCsoConfig;
	CalypsoRaop_t stRaopSettings;
	HUINT32 ulActionId = 0; // TODO: Action ID 고정
	HBOOL bNecessarySettings = FALSE;

	omMediaPlay_Context_t			*pstContext 	=	om_media_play_GetContext (ulActionId);

#if 0
	if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE /* ||
			s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT */ )
	{
		HxLOG_Error ("Not Support in PlayerType: %d\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		return ERR_FAIL;
	}
#endif

	HxSTD_MemSet(&dlnaSettings, 0, sizeof(CSO_Settings_t));
	HxSTD_MemSet(&stCsoConfig, 0, sizeof(SvcCso_Config_t));
	HxSTD_MemSet(&stRaopSettings, 0, sizeof(CalypsoRaop_t));

	if(pszUri == NULL)
	{
		HxLOG_Error ("Failed> URL is NULL\n");
		return ERR_FAIL;
	}
	if(pMediaInfo == NULL)
	{
		HxLOG_Error ("Failed> MediaInfo Handle is NULL\n");
		return ERR_FAIL;
	}

	{
		hErr = SVC_CSO_GetDnPConfig(NULL, (SvcCso_Config_t*)&stCsoConfig);
		if ( hErr != ERR_OK )
		{
			HxLOG_Error ("SVC_CSO_GetDnPConfig() Failed.\n");
			return ERR_FAIL;
		}

		pMediaInfo->bDlna = FALSE;
		dlnaUrl = om_media_play_mediapb_cso_is_dlna(pszUri, &dlnaSettings);
		if ( dlnaUrl )
		{
			pMediaInfo->bDlna = TRUE;
			parsedUrl = dlnaUrl;
			HxLOG_Print("[%s][%d] Found DLNA Url Parsed = %s\n", __FUNCTION__,__LINE__, parsedUrl);
		}

		airplayUrl = om_media_play_mediapb_cso_is_airplay_ex(pszUri, pMediaInfo, &stRaopSettings);
		if ( airplayUrl )
		{
			if( parsedUrl )
			{
				OxMGR_Free( parsedUrl );
			}
			parsedUrl = airplayUrl;
			HxLOG_Print("[%s][%d] Found Airplay Url Parsed = %s\n", __FUNCTION__,__LINE__, parsedUrl);
		}

		if ( parsedUrl == NULL )
		{
			parsedUrl = HLIB_STD_StrDup(pszUri);
	//		parsedUrl = HLIB_STD_StrDup("http://10.0.14.41/streams/wmv/theeast_5419270_1901_2.wmv");

			/* fix for prevent #168878 */
			if(NULL == parsedUrl)
			{
				HxLOG_Error("parsedUrl is NULL\n");
				return ERR_FAIL;
			}

			HxLOG_Print("[%s][%d] Original Url = %s\n", __FUNCTION__,__LINE__, parsedUrl);
		}

		if ((stCsoConfig.bProgressiveDownload) && (dlnaSettings.pProtocolInfo == NULL))
		{
			err = MEDIAPB_IPC_CSO_Open(parsedUrl, eCSO_MODE_DNP, &pMediaInfo->ulCsoSessionId);
			if(err != CSO_ERR_OK)
			{
				HxLOG_Error("MEDIAPB_IPC_CSO_Open failed!\n");
//				pstMediaPlay->stCsoStatus.eStatus = eCSO_STOPPED;

				if( parsedUrl )
				{
					HLIB_STD_MemFree (parsedUrl);
				}

				return ERR_FAIL;
			}
		}
		else
		{
			err = MEDIAPB_IPC_CSO_Open(parsedUrl, eCSO_MODE_PLAY, &pMediaInfo->ulCsoSessionId);
			if(err != CSO_ERR_OK)
			{
				HxLOG_Error("MEDIAPB_IPC_CSO_Open failed!\n");
//				pstMediaPlay->stCsoStatus.eStatus = eCSO_STOPPED;

				if( parsedUrl )
				{
					HLIB_STD_MemFree (parsedUrl);
				}

				return ERR_FAIL;
			}
		}
	}
	HxLOG_Error("[%s](L:%d) pMediaInfo->ulCsoSessionId : %lu\n", __FUNCTION__, __LINE__, pMediaInfo->ulCsoSessionId);
	if(NULL == pMediaInfo->pCsoSettings)
	{
		pSettings = HLIB_STD_MemAlloc(sizeof(CSO_Settings_t));
		HxSTD_MemSet(pSettings, 0x00, sizeof(CSO_Settings_t));
		HxLOG_Error("[%s](L:%d) It's first time to getting cso's setting value!\n", __FUNCTION__, __LINE__);
		err = MEDIAPB_IPC_CSO_GetSettings(pMediaInfo->ulCsoSessionId, pSettings);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_GetSettings failed!\n");
	}
		else
		{
			// store cache
			HxLOG_Error("[%s](L:%d) so, store cso's setting value to cache\n", __FUNCTION__, __LINE__);
			pMediaInfo->pCsoSettings = (void *)pSettings;
		}
	}
	else
	{
		HxLOG_Error("[%s](L:%d) Using cso'setting value from cache\n", __FUNCTION__, __LINE__);
		pSettings = (CSO_Settings_t *)pMediaInfo->pCsoSettings;
	}

	/* File Path of DNP */
	if(pSettings != NULL && (stCsoConfig.bProgressiveDownload && pSaveFilePath) && (dlnaSettings.pProtocolInfo == NULL))
	{
		if(pSettings->pSaveFilePath != NULL && strcmp(pSettings->pSaveFilePath, pSaveFilePath) == 0)
		{
			HxLOG_Error("[%s](L:%d) Same File Path, so doesn't set settings \n", __FUNCTION__, __LINE__);
		}
		else
	{
			if(pSettings->pSaveFilePath != NULL)
		{
				OxMGR_Free(pSettings->pSaveFilePath);
				pSettings->pSaveFilePath = NULL;
		}
			pSettings->pSaveFilePath = HLIB_STD_StrDup(pSaveFilePath);
			bNecessarySettings = TRUE;
		}
		HxLOG_Error("[%s](L:%d) Save File Path = %s\n", __FUNCTION__, __LINE__, pSettings->pSaveFilePath);
	}

	/* */
	if (pSettings != NULL && parsedUrl && strstr(parsedUrl, "raop://") )
	{
		HxSTD_memcpy ( pSettings->tRaopSetting.ucAesKey, stRaopSettings.ucAesKey, 16 );
		HxSTD_memcpy ( pSettings->tRaopSetting.ucAesIv, stRaopSettings.ucAesIv, 16 );
		pSettings->tRaopSetting.unDataPort = stRaopSettings.unDataPort;
		pSettings->tRaopSetting.unControlPort = stRaopSettings.unControlPort;
		pSettings->tRaopSetting.unTimingPort = stRaopSettings.unTimingPort;
		pSettings->tRaopSetting.unDestIpAddress = 100;
		strncpy(pSettings->tRaopSetting.ucAudioCodec , stRaopSettings.ucAudioCodec, sizeof(stRaopSettings.ucAudioCodec));
		strncpy(pSettings->tRaopSetting.ucFmtStr , stRaopSettings.ucFmtStr, sizeof( stRaopSettings.ucFmtStr));
		bNecessarySettings = TRUE;
	}

	/* Playready DRM Setting */
#if 1
#if defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	if(pSettings)
	{
		pSttings->ullDecThresholdSW = 50*1024*1024; /* SW Decrypt Enable, BlockSize < 50 Mbytes */
		bNecessarySettings = TRUE;
	}
#endif
	if(pSettings != NULL && s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData != NULL)
	{
		if(pSettings->tPRDYSetting.pucLicenseData != NULL)
		{
			OxMGR_Free(pSettings->tPRDYSetting.pucLicenseData);
			pSettings->tPRDYSetting.pucLicenseData = NULL;
		}
		pSettings->tPRDYSetting.pucLicenseData = HLIB_STD_StrDup(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData);
		HxLOG_Error("LicenseData = %s\n", pSettings->tPRDYSetting.pucLicenseData);
		bNecessarySettings = TRUE;
	}

	if(pSettings != NULL && s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL != NULL )
	{
		if(pSettings->tPRDYSetting.pucLAURL != NULL)
		{
			OxMGR_Free(pSettings->tPRDYSetting.pucLAURL);
			pSettings->tPRDYSetting.pucLAURL = NULL;
		}
		pSettings->tPRDYSetting.pucLAURL = HLIB_STD_StrDup(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLAURL);
		bNecessarySettings = TRUE;
	}
	//Setting LA URL. (Case of SendDRMMessage)
	if(pSettings != NULL && pstContext->stCache.pucLAURL!=NULL)
	{// Using DrmAgent sendDRMMessage.    LAURL Override data is saved in pstContext->stCache.pucLAURL
		HxLOG_Error("PRDY : Setting LAURL %s\n", pstContext->stCache.pucLAURL);
		if(pSettings->tPRDYSetting.pucLAURL != NULL)
		{
			HxLOG_Error("[%s:%d] pSettings->tPRDYSetting.pucLAURL is NOT NULL!!\n", __FUNCTION__, __LINE__);
			OxMGR_Free(pSettings->tPRDYSetting.pucLAURL);
			pSettings->tPRDYSetting.pucLAURL = NULL;
		}
		pSettings->tPRDYSetting.pucLAURL = HLIB_STD_StrDup(pstContext->stCache.pucLAURL);
		// After LAURL Override is used, data is reset.
		HLIB_STD_MemFree(pstContext->stCache.pucLAURL);
		pstContext->stCache.pucLAURL = NULL;
		bNecessarySettings = TRUE;
	}

	//Setting CustomData (Case of SendDRMMessage)
	if(pSettings != NULL && pstContext->stCache.pucPRDYCustomData!=NULL)
	{// Using DrmAgent sendDRMMessage.    LAURL Override data is saved in pstContext->stCache.pucLAURL
		HxLOG_Error("PRDY : Setting CustomData %s\n", pstContext->stCache.pucPRDYCustomData);
		if(pSettings->tPRDYSetting.pucCustomData!= NULL)
		{
			HxLOG_Error("[%s:%d] pSettings->tPRDYSetting.pucLAURL is NOT NULL!!\n", __FUNCTION__, __LINE__);
			OxMGR_Free(pSettings->tPRDYSetting.pucCustomData);
			pSettings->tPRDYSetting.pucCustomData = NULL;
		}
		pSettings->tPRDYSetting.pucCustomData= HLIB_STD_StrDup(pstContext->stCache.pucPRDYCustomData);
		// After Customdata is used, data is reset.
		HLIB_STD_MemFree(pstContext->stCache.pucPRDYCustomData);
		pstContext->stCache.pucPRDYCustomData = NULL;
		bNecessarySettings = TRUE;
	}


	if(pSettings != NULL && s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData != NULL )
	{
		if(pSettings->tPRDYSetting.pucCustomData != NULL)
		{
			OxMGR_Free(pSettings->tPRDYSetting.pucCustomData);
			pSettings->tPRDYSetting.pucCustomData = NULL;
		}
		pSettings->tPRDYSetting.pucCustomData = HLIB_STD_StrDup(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucCustomData);
		bNecessarySettings = TRUE;
	}
#endif

	/* DLNA Setting */
	if (pSettings != NULL && dlnaSettings.pProtocolInfo )
	{
		if( pSettings->pProtocolInfo != NULL)
		{
			OxMGR_Free(pSettings->pProtocolInfo);
			pSettings->pProtocolInfo = NULL;
		}
		pSettings->pProtocolInfo = HLIB_STD_StrDup(dlnaSettings.pProtocolInfo);
		pSettings->tDlnaSetting.nByteRate = dlnaSettings.tDlnaSetting.nByteRate;
		pSettings->tDlnaSetting.unDuration = dlnaSettings.tDlnaSetting.unDuration;
		pSettings->tDlnaSetting.nFileSize = dlnaSettings.tDlnaSetting.unDuration;

		HxLOG_Error("DLNA : ProtocolInfo %s\n", pSettings->pProtocolInfo);
		bNecessarySettings = TRUE;
	}
#if 1
	/* VMX DRM Setting */
	if(pSettings != NULL && s_stMediaPlayerCache[CalcActionId(ulActionId)].bUseVmx == TRUE && s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl != NULL)
	{
		HxLOG_Debug("VMX is Enabled\n");
		pSettings->eDrmType = eCSO_DRM_VIEWRIGHTWEB;
		if(pSettings->tVRWSetting.pCompanyName != NULL)
		{
			OxMGR_Free(pSettings->tVRWSetting.pCompanyName);
			pSettings->tVRWSetting.pCompanyName = NULL;
		}
		if(pSettings->tVRWSetting.pVCASBootAddr != NULL)
		{
			OxMGR_Free(pSettings->tVRWSetting.pVCASBootAddr);
			pSettings->tVRWSetting.pVCASBootAddr = NULL;
		}
		if( s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName != NULL)
		{
			pSettings->tVRWSetting.pCompanyName = HLIB_STD_StrDup(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxVendorName);
		}
		pSettings->tVRWSetting.pVCASBootAddr = HLIB_STD_StrDup(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucVmxCasUrl);
		bNecessarySettings = TRUE;
		/* Do not release memory of strings(s_pucVmxVendorName[ulActionId], s_pucVmxCasUrl[ulActionId]) after setting.*/
	}
#if 1
	/* NAGRA PRM */
	if(pSettings != NULL && pstContext->stOmCache.stDrmInformation.eDRMType == MGR_MEDIA_DRMTYPE_NAGRAPRM)
	{
		HxLOG_Debug("NagraPRM is Enabled\n");
		pSettings->eDrmType = eCSO_DRM_NAGRAPRM;
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
		pSettings->tNAGRAPRMSetting.hHdl = pstContext->stOmCache.stDrmInformation.ulKeyHandle;
		pSettings->tNAGRAPRMSetting.usEmi = pstContext->stOmCache.stDrmInformation.usEmi;
		pSettings->tNAGRAPRMSetting.ulKeySize = pstContext->stOmCache.stDrmInformation.ulKeyIdSize;
		if(pSettings->tNAGRAPRMSetting.ulKeySize > 0
		&& pSettings->tNAGRAPRMSetting.ulKeySize < sizeof(pSettings->tNAGRAPRMSetting.ucEncryptKey)
		&& pSettings->tNAGRAPRMSetting.ulKeySize <= sizeof(pstContext->stOmCache.stDrmInformation.aucKeyId))
		{
			HxSTD_memcpy(pSettings->tNAGRAPRMSetting.ucEncryptKey, pstContext->stOmCache.stDrmInformation.aucKeyId, pSettings->tNAGRAPRMSetting.ulKeySize);
		}
#else
		pSettings->tNAGRAPRMSetting.ulKeySize = pstContext->stOmCache.stDrmInformation.ulKeySize;
		if(pSettings->tNAGRAPRMSetting.ulKeySize > 0
		&& pSettings->tNAGRAPRMSetting.ulKeySize < sizeof(pSettings->tNAGRAPRMSetting.ucEncryptKey)
		&& pSettings->tNAGRAPRMSetting.ulKeySize <= sizeof(pstContext->stOmCache.stDrmInformation.ucEncryptKey))
		{
			HxSTD_memcpy(pSettings->tNAGRAPRMSetting.ucEncryptKey, pstContext->stOmCache.stDrmInformation.ucEncryptKey, pSettings->tNAGRAPRMSetting.ulKeySize);
		}
		bNecessarySettings = TRUE;
#endif
	}
#endif
#endif

#if defined(CONFIG_PROD_FVP4000T)
	if(pSettings != NULL && pSettings->bStartLowestStream == FALSE)
	{
		pSettings->bStartLowestStream = TRUE;
		bNecessarySettings = TRUE;
	}
#endif

//	memcpy(s_BrowserUserAgent,"Linux; U; HbbTV/1.1.1 (Humax;Calypso;) ce-html/1.0 (NETRANGEMMH; );",strlen("Linux; U; HbbTV/1.1.1 (Humax;Calypso;) ce-html/1.0 (NETRANGEMMH; );") + 1);
	ulUALen =  VK_strlen(s_BrowserUserAgent);
	if(pSettings != NULL && ulUALen > 0)
	{
		if(pSettings->pUserAgentString != NULL && strcmp(pSettings->pUserAgentString, s_BrowserUserAgent) == 0)
		{
			HxLOG_Error("[%s](L:%d) Same User Agent, so doesn't set settings \n", __FUNCTION__, __LINE__);
		}
		else
	{
			if(pSettings->pUserAgentString != NULL)
			{
				OxMGR_Free(pSettings->pUserAgentString);
				pSettings->pUserAgentString = NULL;
			}
			pSettings->pUserAgentString = HLIB_STD_StrDup(s_BrowserUserAgent);
			bNecessarySettings = TRUE;
		}
	}

	if(pSettings != NULL && pSettings->ePreperredAudioLanguage == CSO_LANG_NONE)
	{
		HxLANG_Id_e eAudioLangId = eLangID_MAX;
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0, (HUINT32 *)&eAudioLangId, 0);
		if (ERR_OK == hErr  && (eAudioLangId != eLangID_MAX))
		{
			pSettings->ePreperredAudioLanguage = (CSO_LangCode_t)eAudioLangId;	/* CSO_LANG is compatible with HxLANG. So directly assign here */
		}
		else
		{
			pSettings->ePreperredAudioLanguage = CSO_LANG_NONE;
		}
		bNecessarySettings = TRUE;
	}

	if(pSettings != NULL && bNecessarySettings == TRUE)
	{
#ifdef CONFIG_SUPPORT_ROVI
		pSettings->bBypassing = TRUE ;
#endif
		err = MEDIAPB_IPC_CSO_SetSettings(pMediaInfo->ulCsoSessionId, pSettings);
	if(err != CSO_ERR_OK)
	{
		HxLOG_Error("MEDIAPB_IPC_CSO_SetSettings failed!\n");
	}
	}
	else
		HxLOG_Error("[%s](L:%d) Don't Set Settings \n", __FUNCTION__, __LINE__);


	HxLOG_Error("[%s](L:%d) MEDIAPB_IPC_CSO_Open - SessionId = %d\n", __FUNCTION__, __LINE__, pMediaInfo->ulCsoSessionId);

#if 0
	if ( s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData )
	{
		OxMGR_Free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData);
		s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData = NULL;
	}
#endif

#if 1 /*TODO */
	if ( parsedUrl != NULL )
	{
		OxMGR_Free(parsedUrl);
	}
#endif

#if 0
	err = MEDIAPB_IPC_CSO_Probe(pstMediaPlay->ulCsoSessionId);
	if ( err != CSO_ERR_OK)
	{
		HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Probe\n", __FUNCTION__, __LINE__);
		if(!pstMediaPlay->bExtOpened)
		{
			err = MEDIAPB_IPC_CSO_Close( pstMediaPlay->ulCsoSessionId );
			if ( err != CSO_ERR_OK)
			{
				HxLOG_Error("[%s](L:%d) Error - MEDIAPB_IPC_CSO_Close\n", __FUNCTION__, __LINE__);
			}
		}
		pstMediaPlay->stCsoStatus.eStatus = eCSO_STOPPED;
	}

	pstMediaPlay->stCsoStatus.eStatus = eCSO_PROBING;
#endif
#endif

	return ERR_OK;
}



HERROR om_media_play_mediapb_Probe (HUINT32 ulActionId, HCHAR *pszUri)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR				 hErr;

#ifdef CONFIG_SUPPORT_IMMA
	if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE /* ||
			s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT */ )
	{
		HxLOG_Error ("Not Support in PlayerType: %d\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		return ERR_FAIL;
	}
#endif

	if (NULL == pstContext)					{ HxLOG_Error ("NULL \n"); return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ HxLOG_Error ("NULL \n"); return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if (NULL == pstMediaPlay->pMediaPlayer)
	{
		HxLOG_Error ("Error\n");
		return ERR_FAIL;
	}

	hErr = MGR_MPlayer_Probe(pstMediaPlay->pMediaPlayer, pszUri, eMPLAYER_PLAYTYPE_AUTO);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error ("MGR_MPlayer_Play() with (%s) url Failed.\n", pszUri);
		return ERR_FAIL;
	}
#ifdef CONFIG_SUPPORT_IMMA
	om_media_play_mediapb_cso_EnterSection(pstContext);

	if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_PROBING || pstMediaPlay->stCsoStatus.eStatus == eCSO_PROBED )
	{
		pstMediaPlay->bStartRequested = FALSE;
		HxLOG_Error ("CSO is now probing... cancel StartRequest!!! \n" );
		hErr = om_media_play_mediapb_cso_CancelProbeRequest(pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("om_media_play_mediapb_cso_CancelProbeRequest failed \n");
			om_media_play_mediapb_cso_ExitSection(pstContext);
			return hErr;
		}
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PROBE_CANCELED;
		om_media_play_mediapb_cso_ExitSection(pstContext);
		return ERR_OK;
	}

	if ( pstMediaPlay->stCsoStatus.eStatus >= eCSO_PLAYING )
	{
		hErr = om_media_play_mediapb_cso_Stop(pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("om_media_play_mediapb_cso_Stop failed \n");
			om_media_play_mediapb_cso_ExitSection(pstContext);
			return hErr;
		}
		pstContext->stCache.eState = eOxMP_PLAYSTATE_STOPPED;	//pstContext->bPlayed = FALSE;
	}

	om_media_play_mediapb_EvtConnecting (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);

	hErr = om_media_play_mediapb_cso_ProbeRequest(pstContext, ulActionId, pszUri);
	if (hErr !=  ERR_OK)
	{
		HxLOG_Error ("CSO Probe Request failed with (%s) url Failed.\n", pszUri);
		om_media_play_mediapb_cso_ExitSection(pstContext);
		return ERR_FAIL;
	}

	om_media_play_mediapb_cso_ExitSection(pstContext);
#endif

	pstContext->stCache.eState = eOxMP_PLAYSTATE_STOPPED;//pstContext->bPlayed = FALSE;

	return ERR_OK;
}

HERROR om_media_play_mediapb_Start (HUINT32 ulActionId, void *pstStart)
{
//	HUINT32 				 ulSessionVersion = 0;
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR					 hErr;
	OxMediaPlay_StartMediaPlay_t *pstTempStart;

	//HxLOG_Error("%s(%d) [CRB] Start MediaPB \n", __FUNCTION__, __LINE__);

	if (NULL == pstContext)					{ return ERR_FAIL; }
#ifdef CONFIG_SUPPORT_IMMA

	if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
	{
		if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }
	}
	else if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE ||
				s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT)
	{
		pstTempStart = (OxMediaPlay_StartMediaPlay_t *)pstStart;
		hErr = om_media_play_mediapb_InitAction(pstContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Error ("%d om_media_play_mediapb_InitAction Error\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
			return ERR_FAIL;
		}
		HxLOG_Debug ("%d InitAction Complete!!!\n",s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);
		if (NULL == pstMediaPlay->pMediaPlayer)
		{
			HxLOG_Error ("Error\n");
			return ERR_FAIL;
		}
		/* Set MSE Player form Browser */
		pstMediaPlay->ulActivatedVideoCodecId = pstTempStart->eVideoCodec;
		pstMediaPlay->ulActivatedVideoStreamId = pstTempStart->ulVideoPid;
		pstMediaPlay->ulActivatedAudioCodecId = pstTempStart->eAudioCodec;
		pstMediaPlay->ulActivatedAudioStreamId = pstTempStart->ulAudioPid;
		/* Set PROBED for MSE Player */
		pstMediaPlay->stCsoStatus.eStatus = eCSO_PROBED;
	}
#endif

	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);
	if(pstMediaPlay == NULL || pstMediaPlay->pMediaPlayer == NULL)
	{
		HxLOG_Error ("Error\n");
		return ERR_FAIL;
	}

#ifdef CONFIG_SUPPORT_IMMA
	HxLOG_Debug ("om_media_play_mediapb_cmdStart called!!!\n");

	om_media_play_mediapb_cso_EnterSection(pstContext);

/*
	if (!pstStart)
	{
		HxLOG_Error ("Error no information for Play\n");
		om_media_play_mediapb_cso_ExitSection(pstMediaPlay);
		return ERR_FAIL;
	}
	pstContext->pstStart = HxSTD_MemDup(pstStart, sizeof(MEDIADEC_PB_StartInfo_t));
*/
	/* 상위로 부터 내려오는 StartTime 의 반영 */
	pstTempStart = (OxMediaPlay_StartMediaPlay_t *)pstStart;
#if defined(CONFIG_TIMESTAMP_64)
	if ( pstTempStart->ullStartTime > 0 )
#else
	if ( pstTempStart->ulStartTime > 0 )
#endif
	{
#if defined(CONFIG_TIMESTAMP_64)
		pstMediaPlay->ullStartTime = (HUINT64)pstTempStart->ullStartTime;
#else
		pstMediaPlay->ullStartTime = (HUINT64)pstTempStart->ulStartTime;
#endif
	}

	if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_PROBING )
	{
		pstMediaPlay->bStartRequested = TRUE;
	}
	else if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_PROBED )
	{
		hErr = om_media_play_mediapb_cso_Start(pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("om_media_play_mediapb_cso_Start failed \n");
			om_media_play_mediapb_EvtError (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
			pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING_FAILED;
			om_media_play_mediapb_cso_ExitSection(pstContext);
			return hErr;
		}
		pstContext->stCache.eState = eOxMP_PLAYSTATE_PLAYING;//pstContext->bPlayed = TRUE;
#if 0
		if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
		{
			om_media_play_mediapb_EvtPlaying (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		}
#endif
	}
	else
	{
		HxLOG_Error ("CSO is not opened and probed.... \n");
		om_media_play_mediapb_cso_ExitSection(pstContext);
		return ERR_FAIL;
	}

	om_media_play_mediapb_cso_ExitSection(pstContext);
#else

#if defined(CONFIG_TIMESTAMP_64)
	hErr = MGR_MPlayer_Play (pstMediaPlay->pMediaPlayer, (HUINT32)pstContext->stCache.stMediaPlay.ullStartTime);
#else
	hErr = MGR_MPlayer_Play (pstMediaPlay->pMediaPlayer, pstContext->stCache.stMediaPlay.ulStartTime);
#endif
	if ( hErr != ERR_OK )
	{
		HxLOG_Error ("MGR_MPlayer_Play() with (%s) url Failed.\n", pstContext->stCache.stMediaPlay.szUrl);
		return ERR_FAIL;
	}

	pstContext->stCache.eState = eOxMP_PLAYSTATE_PLAYING;//pstContext->bPlayed = TRUE;
#endif

	//APP_MGR_SetActionToApp (ulActionId, pstAppInfo, ulSessionVersion);

	return ERR_OK;
}

HERROR om_media_play_mediapb_Stop (HUINT32 ulActionId)
{
//	HUINT32 				 ulSessionVersion = 0;
//	OCTOAPPInfo_t			 stActTakenApp;

	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR				 hErr = ERR_FAIL;
	HUINT32 			 semaid = 0;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if (/*(FALSE == pstContext->bActionTaken) &&*/ (eOxMP_PLAYSTATE_PLAYING == pstContext->stCache.eState) ) //pstContext->bPlayed == TRUE )
	{
		HxLOG_Error ("MediaPB not yet started with ActionID (%d)\n", ulActionId);
		goto EXIT_PROC;
	}

	if (NULL != pstMediaPlay->pMediaPlayer)
	{
#ifdef CONFIG_SUPPORT_IMMA
		HxLOG_Debug ("om_media_play_mediapb_cmdStop called!!!\n");


		om_media_play_mediapb_cso_EnterSection(pstContext);

		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_PROBING || pstMediaPlay->stCsoStatus.eStatus == eCSO_PROBED )
		{
			pstMediaPlay->bStartRequested = FALSE;
			HxLOG_Error ("CSO is now probing... cancel StartRequest!!! \n" );
			hErr = om_media_play_mediapb_cso_CancelProbeRequest(pstContext);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("om_media_play_mediapb_cso_CancelProbeRequest failed \n");
				om_media_play_mediapb_cso_ExitSection(pstContext);
				goto EXIT_PROC;
			}
			pstMediaPlay->stCsoStatus.eStatus = eCSO_PROBE_CANCELED;
			om_media_play_mediapb_cso_ExitSection(pstContext);
			goto EXIT_PROC;
		}
		hErr = om_media_play_mediapb_cso_Stop(pstContext);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("om_media_play_mediapb_cso_Stop failed \n");
			om_media_play_mediapb_cso_ExitSection(pstContext);
			goto EXIT_PROC;
		}
		pstContext->stCache.eState = eOxMP_PLAYSTATE_STOPPED;

		/* clean cache */
		om_media_play_CleanCache (pstContext, eOxMP_MEDIATYPE_MEDIAPLAY);

		om_media_play_mediapb_cso_ExitSection(pstContext);
#else
		hErr = MGR_MPlayer_Stop (pstMediaPlay->pMediaPlayer);
		if ( ERR_OK != hErr )
		{
			HxLOG_Error ("MGR_MPlayer_Stop() Failed.\n");
			goto EXIT_PROC;
		}

		pstContext->stCache.eState = eOxMP_PLAYSTATE_STOPPED;
#if 0
		eRet = om_media_play_mediapb_mwMpEventStopped(pstContext, 0, 0, 0, 0);
#endif
#endif
	}


EXIT_PROC:

#ifdef CONFIG_SUPPORT_IMMA
	s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType = eOxMP_MEDIAPLAY_PLAYERTYPE_NONE;
	s_stMediaPlayerCache[CalcActionId(ulActionId)].bAudioTrackSelected = FALSE;
	s_stMediaPlayerCache[CalcActionId(ulActionId)].bVideoTrackSelected = FALSE;
	s_stMediaPlayerCache[CalcActionId(ulActionId)].nAudioTrack = 0;
	s_stMediaPlayerCache[CalcActionId(ulActionId)].nVideoTrack = 0;

	semaid = om_media_play_mediapb_cso_GetSemaphoreId(pstContext);
	HxSEMT_Get(semaid);
	pstContext->pExtension = NULL;
	HxSEMT_Release(semaid);

	if ( !HLIB_LIST_Find(s_pRemoveSemaList, (void*)semaid) )
	{
		s_pRemoveSemaList = HLIB_LIST_Append(s_pRemoveSemaList, (void *)semaid);
	}
#endif
/*
	if( NULL != pstContext->pExtension )
	{
		OxMGR_Free( pstContext->pExtension );
		pstContext->pExtension = NULL;
	}
*/
/*
	if (NULL != pstAppInfo)
	{
		hErr = APP_MGR_GetAppInfoByActionId (ulActionId, &stActTakenApp, &ulSessionVersion);
		if ((ERR_OK == hErr) && (stActTakenApp.ulApplicationId == pstAppInfo->ulApplicationId))
		{
			APP_MGR_ResetActionToApp (ulActionId);
		}
	}
*/
	return hErr;
}


HERROR om_media_play_mediapb_SetComponentIndex (HUINT32 ulActionId, OxMediaPlay_Component_e eCompType, HINT32 CompIndex)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR					 hErr;
	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	switch( eCompType )
	{
#ifdef AV_TRACK_CHANGE
		case eOxMP_COMPONENT_VIDEO:
			hErr = om_media_play_mediapb_cso_SetVideoTrack(ulActionId, CompIndex);
			if( ERR_OK != hErr )
			{
				HxLOG_Error ("om_media_play_mediapb_cso_SetVideoTrack( ) Failed.\n");
				return ERR_FAIL;
			}
			break;
		case eOxMP_COMPONENT_AUDIO:
			hErr = om_media_play_mediapb_cso_SetAudioTrack(ulActionId, CompIndex);
			if( ERR_OK != hErr )
			{
				HxLOG_Error ("om_media_play_mediapb_cso_SetAudioTrack( ) Failed.\n");
				return ERR_FAIL;
			}
			break;
#else
		case eOxMP_COMPONENT_VIDEO:	break;
		case eOxMP_COMPONENT_AUDIO:	break;
#endif
		case eOxMP_COMPONENT_SUBTITLE:
		#ifdef CONFIG_SUPPORT_IMMA
			/* process DVB/EBU subtitle first */
			if(om_media_play_mediapb_processDvbEbuSubtitle(ulActionId, CompIndex) == ERR_OK)
			{
				break;
			}
			/* 외부 subtitle(smi 등) 은 SetSubtitleEnable()로 off/on 하므로. 0 미만의 index는 처리하지 않는다. */
			if (CompIndex < 0)
			{
				HxLOG_Error ("%d index is not supported for external subtile(ex, smi ...) \n", CompIndex);
				break;
			}
			{
				pstMediaPlay->stSubttlInfo.ulCurrLangIdx	=	CompIndex;
				hErr = om_media_play_mediapb_setSubtitleInfo(ulActionId, &(pstMediaPlay->stSubttlInfo));
				if( ERR_OK != hErr )
				{
					HxLOG_Error ("om_media_play_mediapb_setSubtitleInfo (pMediaPlayer:0x%08x)\n", (HUINT32)pstMediaPlay->pMediaPlayer);
					return ERR_FAIL;
				}
				break;
			}
		#else
			break;
		#endif
		case eOxMP_COMPONENT_TELETEXT:	break;
		case eOxMP_COMPONENT_CLOSEDCAPTION:	break;
		case eOxMP_COMPONENT_DATA:	break;
		default:
			break;
	}
	return ERR_OK;
}

HERROR om_media_play_mediapb_SetVideoHide(HUINT32 ulActionId, HBOOL bHide)
{
	omMediaPlay_Context_t	*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR					 hErr;
	HUINT32 				 ulViewId 		=	0;
	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	hErr = SVC_CSO_SetVideoHide(pstMediaPlay->pMediaPlayer, bHide);

	return hErr;
}

HERROR om_media_play_mediapb_SetAudioHide(HUINT32 ulActionId, HBOOL bHide)
{
	omMediaPlay_Context_t	*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR				 	 hErr;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	hErr = SVC_CSO_SetAudioHide(pstMediaPlay->pMediaPlayer, bHide);

	return hErr;
}

HERROR om_media_play_mediapb_UnsetComponentIndex (HUINT32 ulActionId, OxMediaPlay_Component_e eCompType, HINT32 nCompIndex)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR					 hErr;
	OxMediaPlay_VideoCompTable_t *pstVideoCompTable	= NULL;
	OxMediaPlay_AudioCompTable_t *pstAudioCompTable	= NULL;
	HUINT32 				 ulViewId 		=	0;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	ulViewId 		= 	ulActionId - eActionId_VIEW_FIRST;
	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
	pstVideoCompTable =	&pstContext->stCache.stVideoCompTable;
	pstAudioCompTable = &pstContext->stCache.stAudioCompTable;

	switch( eCompType )
	{
#ifdef AV_TRACK_CHANGE
		case eOxMP_COMPONENT_VIDEO:
			if(pstMediaPlay->nActivatedVideoTrackNum != nCompIndex)
			{
				HxLOG_Error ("Activated Video Track is [ %d ]. But delivered index is [ %d ]. So, doesn't do UnSetComponentIndex( )\n", pstMediaPlay->nActivatedVideoTrackNum, nCompIndex);
				return ERR_FAIL;
			}
			hErr = om_media_play_mediapb_SetVideoHide(ulActionId, TRUE);
			if(ERR_OK != hErr)
			{
				HxLOG_Error ("om_media_play_mediapb_SetVideoHide( ) Failed.\n");
				return ERR_FAIL;
			}
			break;
		case eOxMP_COMPONENT_AUDIO:
			if(pstMediaPlay->nActivatedAudioTrackNum != nCompIndex)
			{
				HxLOG_Error ("Activated Audio Track is [ %d ]. But delivered index is [ %d ]. So, doesn't do UnSetComponentIndex( )\n", pstMediaPlay->nActivatedVideoTrackNum, nCompIndex);
				return ERR_FAIL;
			}
			hErr = om_media_play_mediapb_SetAudioHide(ulActionId, TRUE);
			if(ERR_OK != hErr)
			{
				HxLOG_Error ("om_media_play_mediapb_SetAudioHide( ) Failed.\n");
				return ERR_FAIL;
			}
			break;
#else
		case eOxMP_COMPONENT_VIDEO:	break;
		case eOxMP_COMPONENT_AUDIO:	break;
#endif
 		case eOxMP_COMPONENT_SUBTITLE: break;
		case eOxMP_COMPONENT_TELETEXT:	 break;
		case eOxMP_COMPONENT_CLOSEDCAPTION:	break;
		case eOxMP_COMPONENT_DATA:	break;
		default:
			break;
	}
	return ERR_OK;
}

#if 1
HERROR om_media_play_mediapb_SetPlaySpeed (HUINT32 ulActionId, HINT32 nSpeed)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR				 hErr;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

//	if (TRUE == pstContext->bActionTaken)
	{
#ifdef CONFIG_SUPPORT_IMMA
		om_media_play_mediapb_cso_EnterSection(pstContext);
		hErr = om_media_play_mediapb_ApplyPlaySpeed(ulActionId,nSpeed);
		om_media_play_mediapb_cso_ExitSection(pstContext);
#else
		hErr = SVC_MP_SetPlaySpeed (pstMediaPlay->pMediaPlayer, nSpeed);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_MP_SetPlaySpeed (pMediaPlayer:0x%08x)\n", (HUINT32)pstMediaPlay->pMediaPlayer);
			return ERR_FAIL;
		}
#endif
	}
	return hErr;
}
#else
HERROR om_media_play_mediapb_SetPlaySpeed (HUINT32 ulActionId, HINT32 nSpeed)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR				 hErr;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

//	if (TRUE == pstContext->bActionTaken)
	{
#ifdef CONFIG_SUPPORT_IMMA
		om_media_play_mediapb_cso_EnterSection(pstContext);
		HxLOG_Debug("\033[1;92m ----- , Previous Playing Speed! = %d, %d \033[0m \n", nSpeed, pstMediaPlay->stCsoStatus.uiPlayingSpeed);

		if ( pstMediaPlay->stCsoStatus.eStatus < eCSO_PLAYING )
		{
			HxLOG_Error ("not playing status... speed = %d\n", nSpeed);
			if(nSpeed == 0)
			{
				pstMediaPlay->bPausedPlay = TRUE;
			}
			else
			{
				pstMediaPlay->nStartPlaySpeed = nSpeed;
			}
			om_media_play_mediapb_cso_ExitSection(pstContext);
			return ERR_OK;
		}

		if(s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE ||
				s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT)
		{
			if(pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_REQUESTED)
			{
				HxLOG_Error ("Not Support in PlayerType: %d\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return ERR_FAIL;
			}
		}

		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_SEEK_REQUESTED )
		{
			HxLOG_Error ("now seeking status... Can't change the speed = %d\n", nSpeed);
			if( /*pstMediaPlay->bUnderflowAutoMode && */ (nSpeed == 0 || nSpeed == 100))
			{
				pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = nSpeed;
				HxLOG_Error ("In AutoMode, Will keep requested pause or resume operation = %d\n", nSpeed);
			}
			om_media_play_mediapb_cso_ExitSection(pstContext);
			return ERR_FAIL;
		}

		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_REQUESTED )
		{
			if ( nSpeed == pstMediaPlay->stCsoStatus.uiRequestPlaySpeed )
			{
				HxLOG_Error ("Speed %d is already requested! wait for TRICK_COMPLETE from CSO \n", nSpeed);
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return ERR_OK;
			}
		}

		pstMediaPlay->stCsoStatus.uiRequestPlaySpeed = nSpeed;
		if(pstMediaPlay->bUnderflowAutoMode == TRUE)
		{
			if(pstMediaPlay->stCsoStatus.uiRequestPlaySpeed == 100)
			{
				if(pstMediaPlay->bBufferEnoughReported == TRUE)
				{
					pstMediaPlay->bBufferEnoughReported = FALSE;
				}
#ifdef MAXDOME_OLD_PDLMODE
				pstMediaPlay->bNonstopState = FALSE;
#endif
			}
			else if(pstMediaPlay->stCsoStatus.uiRequestPlaySpeed == 0)
			{
				if(pstMediaPlay->bBufferNotEnoughReported == TRUE)
				{
					pstMediaPlay->bBufferNotEnoughReported = FALSE;
				}
#ifdef MAXDOME_OLD_PDLMODE
				pstMediaPlay->ullNonstopBaseTime = (HUINT64)pstMediaPlay->stCsoStatus.uiPlayingTime;
#endif
			}
		}
		/* DECODER Tricking or PLAYING... */
		if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_DECODER_TRICKING || pstMediaPlay->stCsoStatus.eStatus == eCSO_PLAYING )
		{
			/* this means decoder trick...*/
			if ( (0 <= nSpeed ) && (nSpeed <= (HINT32)pstMediaPlay->ulMaxDecoderTrick) )
			{
				HINT32 nCnt = 0;
				HUINT32 pulDecodedPic = 0;
				HUINT32 pulDisplayedPic = 0;
				if(nSpeed == 0 && pstMediaPlay->bCheckDisplayPic == TRUE)
				{
					while(nCnt < 300)
					{
						SVC_CSO_GetDecodedPicture(pstMediaPlay->pMediaPlayer, &pulDecodedPic, &pulDisplayedPic);
						//HxLOG_Error ("pulDecodedPic : %lu, pulDisplayedPic : %lu\n",pulDecodedPic, pulDisplayedPic);
						if(pulDisplayedPic > 0)
						{
							pstMediaPlay->bCheckDisplayPic = FALSE;
							break;
						}
						nCnt++;
						VK_TASK_Sleep(10);
					}
				}
				else
				{
					HxLOG_Error ("It's only audio contents. So, does not check decoded picture.\n");
				}
				HxLOG_Error ("DECODER TRICKING or PLAYING ----> DECODER TRICKING \n");
//				if( pstMediaPlay->stCsoStatus.uiDecoderSpeed != nSpeed )
				{
					hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, nSpeed);
					if ( hErr != ERR_OK )
					{
						HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
						om_media_play_mediapb_cso_ExitSection(pstContext);
						return ERR_FAIL;
					}
				}

				/* this means PAUSE */
				if ( nSpeed == 0 )
				{
					om_media_play_mediapb_EvtPause (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
					pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;

					/* Decoder Tricking or Playing ---> Pause : [parkjh4] PAUSE REPORTING TO CSO */
					CSO_CTRL_PARAM_t tParam;
					HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
					if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_PAUSE, &tParam))
					{
						HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
						om_media_play_mediapb_cso_ExitSection(pstContext);
						return ERR_FAIL;
					}

				}
				else if ( nSpeed == 100 )
				{
					if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
					{
						om_media_play_mediapb_EvtPlaying (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
					}

					/* Decoder Tricking or Pause ---> Resume : [parkjh4] RESUME REPORTING TO CSO */
					CSO_CTRL_PARAM_t tParam;
					HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
					if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_RESUME, &tParam))
					{
						HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
						pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
						om_media_play_mediapb_cso_ExitSection(pstContext);
						return ERR_FAIL;
					}

					pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
				}
				else
				{
					/* Pause or Decoder Tricking ---> Decoder Tricking : [parkjh4] RESUME REPORTING TO CSO */
					if(pstMediaPlay->stCsoStatus.uiDecoderSpeed == 0)
					{
						CSO_CTRL_PARAM_t tParam;
						HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
						if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_RESUME, &tParam))
						{
							HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
							pstMediaPlay->stCsoStatus.eStatus = eCSO_DECODER_TRICKING;
							om_media_play_mediapb_cso_ExitSection(pstContext);
							return ERR_FAIL;
						}
					}
					pstMediaPlay->stCsoStatus.eStatus = eCSO_DECODER_TRICKING;
				}

				pstMediaPlay->stCsoStatus.uiPlayingSpeed = nSpeed;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = nSpeed;

				om_media_play_mediapb_PlaySpeedChanged (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), pstMediaPlay->stCsoStatus.uiPlayingSpeed, 0, 0);
			}
			/* this means seek trick... need trick request!!! and decoder mode setting for seek trrrrrick.*/
			else
			{
				HxLOG_Error ("DECODER TRICKING ----> JUMP TRICK Requested \n");

				hErr = om_media_play_mediapb_cso_SeekTrickRequest(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickCancel() Failed.\n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return ERR_FAIL;
				}

				/* Pause or Decoder Tricking ---> Seek Tricking : [parkjh4] RESUME REPORTING TO CSO */
				if(pstMediaPlay->stCsoStatus.uiDecoderSpeed == 0)
				{
					CSO_CTRL_PARAM_t tParam;
					HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
					if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_RESUME, &tParam))
					{
						HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
						pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_REQUESTED;
						om_media_play_mediapb_cso_ExitSection(pstContext);
						return ERR_FAIL;
					}
				}
				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_REQUESTED;
			}
		}
		else if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICKING )
		{
			/* this means decoder trick...*/
			if ( (0 <= nSpeed ) &&  (nSpeed <= pstMediaPlay->ulMaxDecoderTrick) )
			{

				HxLOG_Error ("JUMP TRICKING ----> JUMP TRICK Cancel Requested \n");

				/* cancel CSO Seek Trick! */
				hErr = om_media_play_mediapb_cso_SeekTrickCancel(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("cso_SeekTrickCancel() Failed.\n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return ERR_FAIL;
				}

				/* Then, change the speed! */
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, nSpeed);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return ERR_FAIL;
				}

				/* JumpTricking ---> Decoder Tricking or Pause : [parkjh4] PAUSE REPORTING TO CSO : Will be notify speed with 100, we need to notify also paused state */
				if(nSpeed == 0)
				{
					CSO_CTRL_PARAM_t tParam;
					HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
					if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_PAUSE, &tParam))
					{
						HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
						om_media_play_mediapb_cso_ExitSection(pstContext);
						return ERR_FAIL;
					}
				}
				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_CANCEL_REQUESTED;
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = nSpeed;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = nSpeed;
			}
			/* this means seek trick... need trick request!!! and decoder mode setting for seek trrrrrick.*/
			else
			{
				HxLOG_Error ("JUMP TRICKING ----> JUMP TRICK Requested \n");

				hErr = om_media_play_mediapb_cso_SeekTrickRequest(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickRequest() Failed.\n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return ERR_FAIL;
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_REQUESTED;
			}
		}
		else if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_REQUESTED ) /* abnormal case... fast function call...*/
		{
			/* this means decoder trick...*/
			if ( (0 <= nSpeed ) &&  (nSpeed <= pstMediaPlay->ulMaxDecoderTrick) )
			{
				HxLOG_Error ("JUMP TRICK Requested ----> JUMP TRICK Cancel Requested \n");

				/* cancel CSO Seek Trick! */
				hErr = om_media_play_mediapb_cso_SeekTrickCancel(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickCancel() Failed.\n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return ERR_FAIL;
				}

				/* Then, change the speed! */
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, nSpeed);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return ERR_FAIL;
				}

				/* JumpTricking ---> JumpTrick Cancel with Decoder Tricking or Pause : [parkjh4] PAUSE REPORTING TO CSO : Will be notify speed with 100, we need to notify also paused state */
				if(nSpeed == 0)
				{
					CSO_CTRL_PARAM_t tParam;
					HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));
					if(CSO_ERR_OK != MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_PAUSE, &tParam))
					{
						HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
						om_media_play_mediapb_cso_ExitSection(pstContext);
						return ERR_FAIL;
					}
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_CANCEL_REQUESTED;
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = nSpeed;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = nSpeed;
			}
			/* this means seek trick... need trick request!!! and decoder mode setting for seek trrrrrick.*/
			else
			{
				HxLOG_Error ("JUMP TRICK Requested ----> JUMP TRICK Requested \n");

				hErr = om_media_play_mediapb_cso_SeekTrickRequest(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickRequest() Failed.\n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return ERR_FAIL;
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_REQUESTED;
			}
		}
		else /* eCSO_JUMP_TRICK_CANCEL_REQUESTED */ /* abnormal case... fast function call...*/
		{
			/* this means decoder trick...*/
			if ( (0 <= nSpeed ) &&  (nSpeed <= pstMediaPlay->ulMaxDecoderTrick) )
			{
				HxLOG_Error ("Abnormal CASE JUMP TRICK Cancel Requested ----> JUMP TRICK Cancel Requested \n");
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return ERR_FAIL;

#if 0
				/* cancel CSO Seek Trick! */
				hErr = om_media_play_mediapb_cso_SeekTrickCancel(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickCancel() Failed.\n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return ERR_FAIL;
				}

				/* Then, change the speed! */
				hErr = SVC_CSO_SetPlaySpeed(pstMediaPlay->pMediaPlayer, nSpeed);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("SVC_CSO_SetPlaySpeed() Failed.\n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return ERR_FAIL;
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_CANCEL_REQUESTED;
				pstMediaPlay->stCsoStatus.uiPlayingSpeed = nSpeed;
				pstMediaPlay->stCsoStatus.uiDecoderSpeed = nSpeed;
#endif
			}
			/* this means seek trick... need trick request!!! and decoder mode setting for seek trrrrrick.*/
			else
			{
				HxLOG_Error ("JUMP TRICK Cancel Requested ----> JUMP TRICK Requested \n");

				hErr = om_media_play_mediapb_cso_SeekTrickRequest(pstContext);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error ("om_media_play_mediapb_cso_SeekTrickRequest() Failed.\n");
					om_media_play_mediapb_cso_ExitSection(pstContext);
					return ERR_FAIL;
				}

				pstMediaPlay->stCsoStatus.eStatus = eCSO_JUMP_TRICK_REQUESTED;
			}

		}

		om_media_play_mediapb_cso_ExitSection(pstContext);
#else
		hErr = SVC_MP_SetPlaySpeed (pstMediaPlay->pMediaPlayer, nSpeed);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_MP_SetPlaySpeed (pMediaPlayer:0x%08x)\n", (HUINT32)pstMediaPlay->pMediaPlayer);
			return ERR_FAIL;
		}
#endif
	}

	return ERR_OK;
}
#endif

#if defined(CONFIG_TIMESTAMP_64)
HERROR om_media_play_mediapb_SetPlaySegment (HUINT32 ulActionId, HINT32 ulSegment, HUINT64 *pullPosition)
#else
HERROR om_media_play_mediapb_SetPlaySegment (HUINT32 ulActionId, HINT32 ulSegment, HUINT32 *pulPosition)
#endif
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	{
#ifdef CONFIG_SUPPORT_IMMA
		CSO_CTRL_PARAM_t	tParam;

		if ( !pstMediaPlay->bSupport_seek )
		{
			HxLOG_Error ("Failed to Seek!!!! \n");
			return ERR_FAIL;
		}

		if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
		{

			HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));

			om_media_play_mediapb_cso_EnterSection(pstContext);

			if ( pstMediaPlay->stCsoStatus.eStatus < eCSO_PLAYING )
			{
				HxLOG_Error ("not playing status... \n");
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return ERR_OK;
			}

			if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_REQUESTED )
			{
				HxLOG_Error ("now tricking status... \n");
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return ERR_FAIL;
			}
			HxLOG_Error ("om_media_play_mediapb_setPlayJump segment = %d \n",ulSegment);

#if 0
			/* this for the circle drawing test...*/
			om_media_play_mediapb_EvtConnecting (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
#endif

			pstMediaPlay->stCsoStatus.eStatus = eCSO_SEEK_REQUESTED;
#if defined(CONFIG_TIMESTAMP_64)
			tParam.ullPosition = pstMediaPlay->stCsoStatus.ullRequestPlayTime;		
#else
			tParam.ullPosition = (HUINT64) pstMediaPlay->stCsoStatus.uiRequestPlayTime;		
#endif
			tParam.ullParam2 = (HUINT64) ulSegment;
			if(MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_SEEK_SEGMENT, &tParam) != CSO_ERR_OK)
			{
				HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
				pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return ERR_FAIL;
			}
			//printf("SetSegment %u(Time %u)\n",ulSegment,(HUINT32)tParam.ullPosition);
#if defined(CONFIG_TIMESTAMP_64)			
			pstMediaPlay->stCsoStatus.ullRequestPlayTime = tParam.ullPosition;
			*pullPosition = pstMediaPlay->stCsoStatus.ullRequestPlayTime;
#else
			pstMediaPlay->stCsoStatus.uiRequestPlayTime = (HUINT32)tParam.ullPosition;
			*pulPosition = pstMediaPlay->stCsoStatus.uiRequestPlayTime;
#endif
			om_media_play_mediapb_cso_ExitSection(pstContext);
		}
		else
		{
			HxLOG_Error ("Not Support in PlayerType: %s\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		}
#else
#if 0
		hErr = SVC_MP_Seek (pstMediaPlay->pMediaPlayer, ulPosition);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_MP_Seek (pMediaPlayer:0x%08x)\n", (HUINT32)pstMediaPlay->pMediaPlayer);
			return ERR_FAIL;
		}
#endif
#endif
	}

	return ERR_OK;
}


#if defined(CONFIG_TIMESTAMP_64)
HERROR om_media_play_mediapb_SetPlayPosition (HUINT32 ulActionId, HINT64 ullPosition)
#else
HERROR om_media_play_mediapb_SetPlayPosition (HUINT32 ulActionId, HINT32 ulPosition)
#endif
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR					 hErr;
	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	{
#ifdef CONFIG_SUPPORT_IMMA
		CSO_CTRL_PARAM_t	tParam;
		CSO_ERR 			err = CSO_ERR_OK;

		if ( !pstMediaPlay->bSupport_seek )
		{
			HxLOG_Error ("Failed to Seek!!!! \n");
			return ERR_FAIL;
		}

		/* Status Initalization */
		pstMediaPlay->bBofReached = FALSE;
		pstMediaPlay->bEofReached = FALSE;
		pstMediaPlay->bCsoEofReached = FALSE;
		pstMediaPlay->ulCsoEofReachedTime = 0;

		if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
		{

			HxSTD_MemSet(&tParam, 0x00, sizeof(CSO_CTRL_PARAM_t));

			om_media_play_mediapb_cso_EnterSection(pstContext);

			if ( pstMediaPlay->stCsoStatus.eStatus < eCSO_PLAYING )
			{
#if defined(CONFIG_TIMESTAMP_64)				
				HxLOG_Error ("not playing status... time = %llu\n", ullPosition);
				pstMediaPlay->ullStartTime = ullPosition;
#else
				HxLOG_Error ("not playing status... time = %llu\n", ulPosition);
				pstMediaPlay->ullStartTime = ulPosition;
#endif
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return ERR_OK;
			}

			if ( pstMediaPlay->stCsoStatus.eStatus == eCSO_JUMP_TRICK_REQUESTED )
			{
#if defined(CONFIG_TIMESTAMP_64)
				HxLOG_Error ("now tricking status... time = %llu\n", ullPosition);
#else
				HxLOG_Error ("now tricking status... time = %d\n", ulPosition);
#endif
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return ERR_FAIL;
			}

#if defined(CONFIG_TIMESTAMP_64)
			HxLOG_Error ("om_media_play_mediapb_setPlayJump time = %llu \n", ullPosition);

			pstMediaPlay->stCsoStatus.ullRequestPlayTime = ullPosition;
#else
			HxLOG_Error ("om_media_play_mediapb_setPlayJump time = %u \n", ulPosition);

			pstMediaPlay->stCsoStatus.uiRequestPlayTime = ulPosition;
#endif
#if 0
			/* this for the circle drawing test...*/
			om_media_play_mediapb_EvtConnecting (pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
#endif

			pstMediaPlay->stCsoStatus.eStatus = eCSO_SEEK_REQUESTED;
#if defined(CONFIG_TIMESTAMP_64)
			tParam.ullPosition = pstMediaPlay->stCsoStatus.ullRequestPlayTime;
#else
			tParam.ullPosition = pstMediaPlay->stCsoStatus.uiRequestPlayTime;
#endif

			err = MEDIAPB_IPC_CSO_Control(pstMediaPlay->ulCsoSessionId, eCSO_CTRL_SEEK, &tParam);
			if(err != CSO_ERR_OK)
			{
				HxLOG_Error("MEDIAPB_IPC_CSO_Control failed!\n");
				pstMediaPlay->stCsoStatus.eStatus = eCSO_PLAYING;
				om_media_play_mediapb_cso_ExitSection(pstContext);
				return ERR_FAIL;
			}

			if(pstMediaPlay->bOnlyAudioContents != TRUE)
				pstMediaPlay->bCheckDisplayPic = TRUE;

			om_media_play_mediapb_cso_ExitSection(pstContext);
		}
		else if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE )
		{
			hErr = SVC_CSO_Flush (pstMediaPlay->pMediaPlayer);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("SVC_CSO_Flush Failed \n");
			}
			pstMediaPlay->stCsoStatus.ulFlushedTime = time(NULL);

			/* Emit SeekReady to MSE Player */
			om_media_play_mediapb_SeekReady(pstContext, SVC_PIPE_GetActionHandle(pstContext->ulActionId), 0, 0, 0);
		}
		else
		{
			HxLOG_Error ("Not Support in PlayerType: %s\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		}
#else
#if defined(CONFIG_TIMESTAMP_64)
		hErr = SVC_MP_Seek (pstMediaPlay->pMediaPlayer, ullPosition);
#else
		hErr = SVC_MP_Seek (pstMediaPlay->pMediaPlayer, ulPosition);
#endif
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_MP_Seek (pMediaPlayer:0x%08x)\n", (HUINT32)pstMediaPlay->pMediaPlayer);
			return ERR_FAIL;
		}
#endif

	}

	return ERR_OK;
}

HERROR om_media_play_mediapb_SetSubtitleEnable (HUINT32 ulActionId, HBOOL bEnable)
{
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR					 hErr	=	ERR_FAIL;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
#ifdef CONFIG_SUPPORT_IMMA
	if( NULL != pstMediaPlay )
	{
		pstMediaPlay->stSubttlInfo.bDisplay =	bEnable;
		hErr = om_media_play_mediapb_setSubtitleInfo(ulActionId, &(pstMediaPlay->stSubttlInfo));
		if( ERR_OK != hErr )
		{
			HxLOG_Error ("om_media_play_mediapb_setSubtitleInfo (pMediaPlayer:0x%08x)\n", (HUINT32)pstMediaPlay->pMediaPlayer);
			return ERR_FAIL;
		}
	}
#endif

	return hErr;
}

HERROR om_media_play_mediapb_SetVideoHideEnable (HUINT32 ulActionId, HBOOL bEnable)
{
	return ERR_OK;
}

HERROR om_media_play_mediapb_GetSupportedSpeeds (HUINT32 ulActionId, HUINT32 *pulNumSpeed, HINT32 *pSpeeds)
{
#if !defined(CONFIG_SUPPORT_IMMA)
	HERROR					 hErr = ERR_OK;
	HINT32					*nSpeedArray = NULL;
#endif
	HUINT32					 ulCount = 0;
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HINT32					 i = 0;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	HxSTD_MemSet (pSpeeds, 0, sizeof(HINT32) * OxMP_SUPPORTING_SPEED_MAX);

#ifdef CONFIG_SUPPORT_IMMA
	if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_HMX )
	{
		if ( pstMediaPlay->bSupport_trick )
		{
#if 1
			for ( i = 0 ; i < sizeof(s_SupportedSpeed) / sizeof(HINT32) ;i++)
			{
				pSpeeds[ulCount++]	= s_SupportedSpeed[i];
			}
#else
			pSpeeds[ulCount++]	= -6400;
			pSpeeds[ulCount++]	= -3200;
			pSpeeds[ulCount++]	= -1600;
			pSpeeds[ulCount++]	= -800;
			pSpeeds[ulCount++]	= -400;
			pSpeeds[ulCount++]	= -200;
			pSpeeds[ulCount++]	= 0;
			pSpeeds[ulCount++]	= 100;
			pSpeeds[ulCount++]	= 200;
			pSpeeds[ulCount++]	= 400;
			pSpeeds[ulCount++]	= 800;
			pSpeeds[ulCount++]	= 1600;
			pSpeeds[ulCount++]	= 3200;
			pSpeeds[ulCount++]	= 6400;
#endif
		}
		else
		{
			pSpeeds[ulCount++]	= 0;
			pSpeeds[ulCount++]	= 100;
		}
	}
	else if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_MSE )
	{
		pSpeeds[ulCount++]	= 0;
		pSpeeds[ulCount++]	= 100;
	}
	else if( s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType == eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT )
	{
		//pSpeeds[ulCount++]	= 0;
		//pSpeeds[ulCount++]	= 100;
		HxLOG_Error ("Not Support Speed in PlayerType: eOxMP_MEDIAPLAY_PLAYERTYPE_ES\n");
	}
	else
	{
		HxLOG_Error ("Not Support in PlayerType: %d\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
	}

	*pulNumSpeed = (HINT32)ulCount;
#else
	hErr = SVC_MP_GetSupportedSpeeds(pstMediaPlay->pMediaPlayer, &ulCount, &nSpeedArray);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("Error:\n");
		return ERR_FAIL;
	}

	for ( i=0; i<ulCount; ++i )
		pSpeeds[i] = nSpeedArray[i];

	*pulNumSpeed = ulCount;
#endif
	return ERR_OK;
}

HERROR om_media_play_mediapb_SaveThumbnail( HUINT32 ulActionId, HCHAR * pszFilename, HUINT32 ulWidth, HUINT32 ulHeight )
{
	HERROR					 hErr;
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

	if(pstMediaPlay && pszFilename)
	{
//		if (TRUE == pstContext->bActionTaken)
		{
			HxLOG_Error("\n[SaveThumbnail] path=%s w=%d h=%d\n", pszFilename, ulWidth, ulHeight);
			hErr = MGR_MPlayer_SetSaveThumbnail(pstMediaPlay->pMediaPlayer, pszFilename, ulWidth, ulHeight);
			if (ERR_OK != hErr)
			{
				HxLOG_Error ("MGR_MPlayer_SetSaveThumbnail (pMediaPlayer:0x%08x)\n", (HUINT32)pstMediaPlay->pMediaPlayer);
				return ERR_FAIL;
			}
		}
	}

	return ERR_OK;
}

HERROR om_media_play_mediapb_SetSubtitleSyncTime (HUINT32 ulActionId, HINT32 nValue)
{
	omMediaPlay_Context_t			*pstContext 	=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR					 hErr	=	ERR_FAIL;

	if (NULL == pstContext) 				{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
#ifdef CONFIG_SUPPORT_IMMA
	if( NULL != pstMediaPlay )
	{
		pstMediaPlay->stSubttlInfo.ulSyncTime =	(HUINT32)nValue;
		hErr = om_media_play_mediapb_setSubtitleInfo(ulActionId, &(pstMediaPlay->stSubttlInfo));
		if( ERR_OK != hErr )
		{
			HxLOG_Error ("om_media_play_mediapb_setSubtitleInfo (pMediaPlayer:0x%08x)\n", (HUINT32)pstMediaPlay->pMediaPlayer);
			return ERR_FAIL;
		}
	}
#endif

	return hErr;
}

HERROR om_media_play_mediapb_SetSubtitleFontSize (HUINT32 ulActionId, HINT32 nValue)
{
	omMediaPlay_Context_t			*pstContext 	=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR					 hErr	=	ERR_FAIL;

	if (NULL == pstContext) 				{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
#ifdef CONFIG_SUPPORT_IMMA
	if( NULL != pstMediaPlay )
	{
		pstMediaPlay->stSubttlInfo.ulFontSize = (HUINT32)nValue;
		hErr = om_media_play_mediapb_setSubtitleInfo(ulActionId, &(pstMediaPlay->stSubttlInfo));
		if( ERR_OK != hErr )
		{
			HxLOG_Error ("om_media_play_mediapb_setSubtitleInfo (pMediaPlayer:0x%08x)\n", (HUINT32)pstMediaPlay->pMediaPlayer);
			return ERR_FAIL;
		}
	}
#endif

	return hErr;
}

HERROR om_media_play_mediapb_SetSubtitleTextPosition (HUINT32 ulActionId, HINT32 nValue)
{
	omMediaPlay_Context_t			*pstContext 	=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;
	HERROR					 hErr	=	ERR_FAIL;

	if (NULL == pstContext) 				{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension) 	{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
#ifdef CONFIG_SUPPORT_IMMA
	if( NULL != pstMediaPlay )
	{
		pstMediaPlay->stSubttlInfo.ulTextPosition = (HUINT32)nValue;
		hErr = om_media_play_mediapb_setSubtitleInfo(ulActionId, &(pstMediaPlay->stSubttlInfo));
		if( ERR_OK != hErr )
		{
			HxLOG_Error ("om_media_play_mediapb_setSubtitleInfo (pMediaPlayer:0x%08x)\n", (HUINT32)pstMediaPlay->pMediaPlayer);
			return ERR_FAIL;
		}
	}
#endif

	return hErr;
}


HERROR om_media_play_mediapb_SetBufferingScenario (HUINT32 ulActionId, HBOOL bAuto, HINT32 nThreshold)
{
#if !defined(CONFIG_SUPPORT_IMMA)
	HERROR					 hErr;
	omMPMediaPlayContext_t	 *pstMediaPlay;
#endif

	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);

	if (NULL == pstContext)					{ return ERR_FAIL; }
#if !defined(CONFIG_SUPPORT_IMMA)
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);
#endif

#ifdef CONFIG_SUPPORT_IMMA
	s_stMediaPlayerCache[CalcActionId(ulActionId)].bUnderflowAutoMode = bAuto;
	s_stMediaPlayerCache[CalcActionId(ulActionId)].nUnderflowThreshold = nThreshold;
#else
	hErr = SVC_MP_SetBufferingScenario(pstMediaPlay->pMediaPlayer, bAuto, nThreshold);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_MP_SetBufferingScenario)\n");
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}


HERROR om_media_play_mediapb_SetDrmLicenseData (HUINT32 ulActionId, HCHAR * pszData)
{
#if defined(CONFIG_SUPPORT_IMMA)
	unsigned int			 uiLength=0;
#else
	HERROR					 hErr;
#endif
	omMediaPlay_Context_t			*pstContext		=	om_media_play_GetContext (ulActionId);
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	if (NULL == pstContext)					{ return ERR_FAIL; }

	pstMediaPlay	=	(omMPMediaPlayContext_t*)(pstContext->pExtension);

#ifdef CONFIG_SUPPORT_IMMA
	if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData != NULL)
	{
		free(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData);
		s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData = NULL;
	}
	uiLength = VK_strlen(pszData);

	if ( uiLength == 0 )
	{
		HxLOG_Error ("There is no License Data)\n");
		return ERR_FAIL;
	}

	s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData = HLIB_STD_MemAlloc(uiLength+1);
	if(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData == NULL)
	{
		HxLOG_Error("Out of memory!\n");
		return ERR_FAIL;
	}
	HxSTD_memset(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData, 0x0, uiLength+1);
	HxSTD_memcpy(s_stMediaPlayerCache[CalcActionId(ulActionId)].pucLicenseData, pszData, uiLength);
#else
	/* TODO: Maybe pstMediaPlay is NULL, Because Can't Create */
	/* Will Change - use s_pucLicenseData */
	if(pstMediaPlay->pMediaPlayer == NULL)
	{
		HxLOG_Error ("pstMediaPlay->pMediaPlayer is NULL\n");
	}
	else
	{
		hErr = SVC_MP_SetDrmLicenseData(pstMediaPlay->pMediaPlayer, pszData);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("SVC_MP_SetDrmLicenseData\n");
			return ERR_FAIL;
		}
	}
#endif

	return ERR_OK;
}


HERROR om_media_play_mediapb_SetBrowserUserAgentData (HUINT32 ulActionId, HCHAR * pszUAData)
{
#ifdef CONFIG_SUPPORT_IMMA
	HUINT32 unLen = 0;
	HxLOG_Error("[%s](L:%d) Error - Not Implemented with IMMA\n", __FUNCTION__, __LINE__);
	if(pszUAData != NULL)
	{
		unLen = VK_strlen(pszUAData);
	}
	if( unLen > 0 && unLen < sizeof(s_BrowserUserAgent))
	{
		HxSTD_memcpy(s_BrowserUserAgent,pszUAData,unLen);
	}
	else
	{
		HxLOG_Error ("INVALID Length : %u (MAX %u)\n",unLen,sizeof(s_BrowserUserAgent));
	}
#else
	HERROR					 hErr;
	hErr = SVC_MP_SetBrowserUserAgentData( pszUAData );
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_MP_SetBrowserUserAgentData)\n");
		return ERR_FAIL;
	}
#endif
	return ERR_OK;
}

HERROR om_media_play_mediapb_ResourceMap (void)
{
	HERROR hErr = ERR_OK;
#ifdef CONFIG_SUPPORT_IMMA
	HUINT32 ulPumpListSize = 0;
	HUINT32 ulDmaListSize = 0;
	//void *pResourceMap = NULL;
	HUINT32 ulRPCResourceMapSize = 0;
	HERROR 			hResult;
	OxMediaPlay_RPCResourceMap_t	*pstRPCResourceMap = NULL;
	SvcCso_Resource_t	*pstTempResourceMap = NULL;

	/* TODO: ADD Copy Structure SVC_CSO to oapi */

	hResult = SVC_CSO_GetResourceMap(&pstTempResourceMap);
	if((hResult != ERR_OK) || (pstTempResourceMap == NULL))
	{
		HxLOG_Error("SVC_CSO_GetResourceMap failed!\n");
		return ERR_FAIL;
	}

	//pstTempResourceMap = (OxMediaPlay_ResourceMap_t *)pResourceMap;

	/* Pump, Dec Handle Resource List */
	ulPumpListSize = (sizeof(pstTempResourceMap->pulPlaypumpList) * pstTempResourceMap->ulNumOfPlaypump);
	ulDmaListSize = (sizeof(pstTempResourceMap->pulDmaList) * pstTempResourceMap->ulNumOfDma);
	ulRPCResourceMapSize = ulPumpListSize + ulDmaListSize;

	/* Malloc for RPC */
	pstRPCResourceMap = (OxMediaPlay_RPCResourceMap_t*)OxMGR_Calloc(sizeof(OxMediaPlay_RPCResourceMap_t));
	if(pstRPCResourceMap == NULL)
	{
		HxLOG_Error("Calloc failed!\n");
		hErr = ERR_FAIL;
		goto end;
	}
	pstRPCResourceMap->aResourceArray = (HCHAR *)OxMGR_Calloc(ulRPCResourceMapSize);
	if(pstRPCResourceMap->aResourceArray == NULL)
	{
		HxLOG_Error("Calloc failed!\n");
		hErr = ERR_FAIL;
		goto end;
	}

	/* For RPC Data*/
	HxLOG_Debug("ulDmaListSize:%d, ulPumpListSize:%d\n", ulDmaListSize, ulPumpListSize);
	HxSTD_MemCopy(&pstRPCResourceMap->aResourceArray[0], pstTempResourceMap->pulPlaypumpList, ulPumpListSize);
	HxSTD_MemCopy(&pstRPCResourceMap->aResourceArray[ulPumpListSize], pstTempResourceMap->pulDmaList, ulDmaListSize);

	pstRPCResourceMap->ulNumOfDma = pstTempResourceMap->ulNumOfDma;
	pstRPCResourceMap->ulNumOfPlaypump = pstTempResourceMap->ulNumOfPlaypump;
	pstRPCResourceMap->ulAudioMixerHandle = pstTempResourceMap->ulAudioMixerHandle;
#if 0
	HxLOG_Trace();
	HxLOG_Error("ulNumOfDma:%d, ulNumOfPlaypump:%d\n", pstTempResourceMap->ulNumOfDma, pstTempResourceMap->ulNumOfPlaypump);
	{
		int i=0;
		for(i=0; i<pstTempResourceMap->ulNumOfPlaypump; ++i)
		{
			HxLOG_Error("[%08X] \n", pstTempResourceMap->pulPlaypumpList[i]);
		}
	}
#endif
	om_media_play_mediapb_CbOnResourceMapChanged (pstRPCResourceMap, ulRPCResourceMapSize);

end:
	if(pstRPCResourceMap != NULL)
	{
		if(pstRPCResourceMap->aResourceArray != NULL)
		{
			OxMGR_Free( pstRPCResourceMap->aResourceArray );
			pstRPCResourceMap->aResourceArray = NULL;
		}
		OxMGR_Free( pstRPCResourceMap );
		pstRPCResourceMap = NULL;
	}
	if(pstTempResourceMap != NULL)
	{
		HLIB_STD_MemFree(pstTempResourceMap);
		pstTempResourceMap = NULL;
	}
#endif
	return hErr;
}

HERROR om_media_play_mediapb_SetDuration (HUINT32 ulActionId, HUINT64 ullDuration)
{
#ifdef CONFIG_SUPPORT_IMMA
	omMediaPlay_Context_t			*pstContext;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);

	pstMediaPlay->ullDuration = ullDuration;
	HxLOG_Debug("ullDuration %llu\n", ullDuration);
	(void)SVC_CSO_SetDuration(pstMediaPlay->pMediaPlayer, ullDuration);
#endif
	return ERR_OK;
}

HERROR om_media_play_mediapb_SetVmxCas (HUINT32 ulActionId, HUINT8 *pCasUrl, HUINT8 *pVendorName)
{
	HERROR hErr = ERR_FAIL;

	hErr = om_media_play_mediapb_CbOnSetVmxCas(ulActionId, pCasUrl, pVendorName);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("om_media_play_mediapb_CbOnSetVmxCas ERROR!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR om_media_play_mediapb_SetPlayerType (HUINT32 ulActionId, OxMediaPlay_PlayerType_e eMediaPlayerType)
{
#ifdef CONFIG_SUPPORT_IMMA
	/* TODO: Store PlayerType! */
	if((eOxMP_MEDIAPLAY_PLAYERTYPE_HMX != eMediaPlayerType) &&
			(eOxMP_MEDIAPLAY_PLAYERTYPE_MSE != eMediaPlayerType) &&
			(eOxMP_MEDIAPLAY_PLAYERTYPE_ES_DIRECT != eMediaPlayerType))
	{
		HxLOG_Error("Can't Support MediaPlayerType : %d\n");
		return ERR_FAIL;
	}
//	HxLOG_Debug(" OldMediaPlayerType : %d > NewMediaPlayerType : %d\n", s_eMediaPlayerType, eMediaPlayerType);

	/* TODO: Maximum ActionID? */
	s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType = eMediaPlayerType;

#endif
	return ERR_OK;
}

HERROR om_media_play_mediapb_AudioFlush (HUINT32 ulActionId, HUINT32 unTryToChangeAudioTrack) // mgr_appmgr_mctrl_mediapb_setAudioFlush
{
#ifdef CONFIG_SUPPORT_IMMA
	omMediaPlay_Context_t	*pstContext;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);

	if (s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType != eOxMP_MEDIAPLAY_PLAYERTYPE_MSE)
	{
		HxLOG_Error("Error> Non-MSE playback is not supported(%d)!\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		return ERR_FAIL;
	}

#if 0
	pstMediaPlay->ullDuration = ullDuration;
#else
	pstMediaPlay->ullDuration = unTryToChangeAudioTrack;
#endif
	HxLOG_Debug("unTryToChangeAudioTrack %u\n", unTryToChangeAudioTrack);
	(void)SVC_CSO_AudioFlush(pstMediaPlay->pMediaPlayer, unTryToChangeAudioTrack);
#endif
	return ERR_OK;
}

HERROR om_media_play_mediapb_AudioRestart (HUINT32 ulActionId)
{
#ifdef CONFIG_SUPPORT_IMMA
	omMediaPlay_Context_t			*pstContext;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);

	if (s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType != eOxMP_MEDIAPLAY_PLAYERTYPE_MSE)
	{
		HxLOG_Error("Error> Non-MSE playback is not supported(%d)!\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		return ERR_FAIL;
	}

	(void)SVC_CSO_AudioRestart(pstMediaPlay->pMediaPlayer);
#endif
	return ERR_OK;
}

HERROR om_media_play_mediapb_AudioCodecSwitch (HUINT32 ulActionId, HUINT32 unTryToChangeAudioTrack, HINT32 nAudioCodec)
{
#ifdef CONFIG_SUPPORT_IMMA
	omMediaPlay_Context_t			*pstContext;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);

	if (s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType != eOxMP_MEDIAPLAY_PLAYERTYPE_MSE)
	{
		HxLOG_Error("Error> Non-MSE playback is not supported(%d)!\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		return ERR_FAIL;
	}

	(void)SVC_CSO_AudioCodecSwitch (pstMediaPlay->pMediaPlayer, unTryToChangeAudioTrack, nAudioCodec);
#endif
	return ERR_OK;
}

HERROR om_media_play_mediapb_SetFade(HUINT32 ulActionId, HUINT32 unStartVolume, HUINT32 unTargetVolume, HINT32 nEase, HINT32 nEaseTimeElapsed, HINT32 nEaseTransitionFrames)
{
	HERROR hErr = ERR_OK;
#ifdef CONFIG_SUPPORT_IMMA
	omMediaPlay_Context_t			*pstContext;
	omMPMediaPlayContext_t	*pstMediaPlay	=	NULL;

	pstContext = om_media_play_GetContext (ulActionId);
	if (NULL == pstContext)					{ return ERR_FAIL; }
	if (NULL == pstContext->pExtension)		{ return ERR_FAIL; }

	pstMediaPlay = (omMPMediaPlayContext_t*)(pstContext->pExtension);

	if (s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType != eOxMP_MEDIAPLAY_PLAYERTYPE_MSE)
	{
		HxLOG_Error("Error> Non-MSE playback is not supported(%d)!\n", s_stMediaPlayerCache[CalcActionId(ulActionId)].eMediaPlayerType);
		hErr = ERR_FAIL;
		goto func_exit;
	}

	hErr = SVC_CSO_SetAudioFade(pstMediaPlay->pMediaPlayer, unStartVolume, unTargetVolume, nEase, nEaseTimeElapsed, nEaseTransitionFrames);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to set fade, hErr = %d\033[0m\n", __func__, __LINE__, hErr);
		goto func_exit;
	}

func_exit:
#endif
	return hErr;
}

#define _____RPC_INITIALIZATION_____
HERROR om_media_play_mediapb_InitRpcFunctions (HINT32 nRpcHandle)
{
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_Probe, 					"iis",	om_media_play_mediapb_CbProbe					, NULL, "Probe the media: (HUINT32 ulSessionId, HUINT32 ulViewId, HCHAR *pszUri)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetSubtitleSyncTime,		"iii",	om_media_play_mediapb_CbSetSubtitleSyncTime		, NULL, "SetSubtitleExternal: (HUINT32 ulViewId, HUINT32 ulValue)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetSubtitleFontSize,		"iii",	om_media_play_mediapb_CbSetSubtitleFontSize		, NULL, "SetSubtitleExternal: (HUINT32 ulViewId, HUINT32 ulValue)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetSubtitleTextPosition,	"iii",	om_media_play_mediapb_CbSetSubtitleTextPosition	, NULL, "SetSubtitleExternal: (HUINT32 ulViewId, HUINT32 ulValue)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SeBufferingScenario,		"iiii",	om_media_play_mediapb_CbSetBufferingScenario		, NULL, "SeBufferingScenario: (HUINT32 ulViewId, HBOOL bAuto, HINT32 nThreshold)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetDrmLicenseData,		"iis",	om_media_play_mediapb_CbSetDrmLicenseData		, NULL, "SetDrmLicenseData: (HUINT32 ulViewId, const HCHAR *pszData)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetBrowserUserAgentData,	"iis",	om_media_play_mediapb_CbSetBrowserUserAgentData	, NULL, "SetBrowserUserAgentData: (HUINT32 ulViewId, HCHAR *pszUAData)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_ResourceMap,				NULL,	om_media_play_mediapb_CbResourceMap				, NULL, "ResourceMap: (void)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetDuration,				"iil",	om_media_play_mediapb_CbSetDuration				, NULL, "SetDuration: (HUINT32 ulViewId, HUINT64 ullDuration)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetPlayerType,			"iii",	om_media_play_mediapb_CbSetPlayerType			, NULL, "SetPlayerType: (HUINT32 ulViewId, OxMediaPlay_PlayerType_e eMediaPlayPlayerType)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_AudioFlush,				"iii",	om_media_play_mediapb_CbAudioFlush				, NULL, "AudioFlush: (HUINT32 ulSessionId, HUINT32 ulViewId, HUINT32 unTryToChangeAudioTrack)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_AudioRestart,				"ii",	om_media_play_mediapb_CbAudioRestart			, NULL, "AudioRestart: (HUINT32 ulSessionId, HUINT32 ulViewId)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_AudioCodecSwitch,			"iiii",	om_media_play_mediapb_CbAudioCodecSwitch		, NULL, "AudioCodecSwitch: (HUINT32 ulSessionId, HUINT32 ulViewId, HUINT32 unTryToChangeAudioTrack, HINT32 nAudioCodec)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetFade,					"iiiiiii",	om_media_play_mediapb_CbSetFade				, NULL, "SetFade: (HUINT32 ulSessionId, HUINT32 ulViewId, HUINT32 unStartVolume, HUINT32 unTargetVolume, HINT32 nEase, HINT32 nEaseTimeElapsed, HINT32 nEaseTransitionFrames)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetPlayReadyInfo,			"issi",	om_media_play_mediapb_CbSetPlayReadyInfo		, NULL, "SetPlayReadyInfo: (HUINT32 ulViewId, HCHAR *pszLaUrl, HCHAR *pszCustomData, HBOOL bControlFlag)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_SetVmxCas,				"iss",	om_media_play_mediapb_CbSetVmxCas				, NULL,"SetVmxCas: (HUINT32 ulViewId, HCHAR *pszVCASUrl, HCHAR *pszVendorName)");
	HLIB_RPC_RegisterCall (nRpcHandle,RPC_OAPI_MEDIA_PLAY_UnsetVmxCas,				"i",	om_media_play_mediapb_CbUnsetVmxCas			, NULL,"SetVmxCas: (HUINT32 ulViewId)");

	HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_CAS_VERIMATRIX_RequestWebVersion, 	NULL, 	om_media_play_mediapb_CbGetVmxVersion,	NULL,	"om_media_play_mediapb_CbGetVmxVersion(void)");

	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnProbeErrored 			,NULL, "MEDIA ProbeErrored.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnStopErrored				,NULL, "MEDIA StopErrored.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBufferStateChanged		,"iiii", "MEDIA BuffStateChanged.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_BufferState_e eBufferState)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBufferInfoChanged		,"iiib", "MEDIA BuffInfoChanged.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_BufferInfo_t *pstBufferInfo)\n");
#if defined(CONFIG_TIMESTAMP_64)
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayInfo 				,"iiilli", "MEDIA PlayInfo.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT64 ullPlaytime, HUINT64 ullDuration, HUINT32 ulBitrate)\n");
#else
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayInfo				,"iiiiii", "MEDIA PlayInfo. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 ulPlaytime, HUINT32 ulDuration, HUINT32 ulBitrate)\n");
#endif
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPumpIndexChanged 		,"iiiii", "MEDIA PumpIndexChanged.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, HUINT32 ulPumpId)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSeekReady				,"iii", "MEDIA SeekReady. 		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioCodecSwitched		,"iii", "MEDIA AudioCodecSwitched.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioFlushCompleted		,"iii", "MEDIA AudioFlushCompleted.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioRestartCompleted	,"iii", "MEDIA AudioRestartCompleted.\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioFadeCompleted		,"iii", "MEDIA AudioFadeCompleted.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioFadeFailed			,"iii",	"MEDIA AudioFadeFailed.	\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioDecoderVolumeChanged,	"iiii",	"MEDIA AudioDecoderVolumeChanged. \n\t - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eMediaType, HUINT32 unVolume)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSBTLUpdate				,"iiib", "MEDIA SBTLUpdate.		\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_SubtitleStatus_t *pstStatus)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnResourceMapChanged		,"iiiib", "MEDIA ResMapChanged. 	\n\t   - (OxMediaPlay_MediaType_e eMediaType, HUINT32 ulNumOfPlaypump, HUINT32 ulNumOfDma, HUINT32 ulAudioMixerHandle, HCHAR *aResourceArray)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVmxDrmStatusChanged		,"iii", "MEDIA VmxDrmStatusChanged. \n\t - (HUINT32 ulViewId, HUINT32 ulSessionId, HINT32 nStatusCode)\n" );
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnVmxDrmErrorChanged		,"iii", "MEDIA VmxDrmErrorChanged. \n\t  - (HUINT32 ulViewId, HUINT32 ulSessionId, HINT32 nErrorCode)\n" );

	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleSyncTime 		,"ii", "SET.MEDIA SubtitleSyncTime.		\n\t   - (HUINT32 ulViewId, HINT32 nSubtitle_SyncTime)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleFontSize 		,"ii", "SET.MEDIA SubtitleFontSize.		\n\t   - (HUINT32 ulViewId, HINT32 nSubtitle_FontSize)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnSubtitleTextPosition 	,"ii", "SET.MEDIA SubtitleTextPosition.	\n\t   - (HUINT32 ulViewId, HINT32 nSubtitle_TextPosition)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBufferingScenario		,"i", "SET.MEDIA BuffScenario.			\n\t   - (HUINT32 ulViewId)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnDrmLicenseData			,"i", "SET.MEDIA DrmLicenseData.		\n\t   - (HUINT32 ulViewId)\n");
 	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnBrowserUserAgentData 	,"i", "SET.MEDIA BrowUserAgent.			\n\t   - (HUINT32 ulViewId)\n");
 	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnDuration 				,"i", "SET.MEDIA Duration.				\n\t   - (HUINT32 ulViewId)\n");
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnPlayerType				,"i", "SET.MEDIA PlayerType.			\n\t   - (HUINT32 ulViewId)\n");
	HLIB_RPC_CreateNotification	(nRpcHandle, RPC_OBAMA_CAS_VERIMATRIX_onWebVersionChanged, "s", "Verimatrix Cas parameters and status changed.\n\t   - (OxCasVerimatrixStatus_t *pstXomCasVerimatrixStatus)\n");
#if defined(CONFIG_MW_OUTPUT_HDAUDIO_SR_RESTRICT)
	HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_MEDIA_PLAY_OnAudioOutputRestrict				,"iii", "MEDIA SampleRate of Audio Output Error.			\n\t   - (HUINT32 ulViewId, HUINT32 ulSessionId)\n");
#endif
	s_nRpcHandle = nRpcHandle;

	return ERR_OK;
}

