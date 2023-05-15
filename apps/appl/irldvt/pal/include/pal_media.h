/** **********************************************************************************************************
	@file 		pal_media.h

	@date		2012/10/22
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

#ifndef __PAL_MEDIA0_H__
#define __PAL_MEDIA0_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <irldvt_stb_params.h>
#include "htype.h"

/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */
/* End Extern variable */

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */

#define PAL_MEDIA_OK						0
#define PAL_MEDIA_DRMINFO_MAXNUM			(2)
#define PAL_MEDIA_CRYPTO_KEY_LEN			16

#define PAL_MAX_MEDIA_NUM					(1)
#define PAL_MEDIA_MAX_VIDEO_TRACK			8
#define PAL_MEDIA_MAX_AUDIO_TRACK			8
#define PAL_MEDIA_MAX_XSUB_TRACK			8
#define PAL_MEDIA_MAX_FILE_NAME_LENGTH		256
#define PAL_MEDIA_FRAMERATE_LENGTH			(8)

#define PAL_MEDIA_SUB_STR_MAX                   64
#define PAL_MEDIA_SUB_LANGUAGE_MAX              8
#define PAL_MEDIA_SUB_LANGUAGE_STRLEN_MAX       32

#define PAL_MEDIA_SUB_STRING_MAX				128
#define PAL_MEDIA_SUB_LINE_MAX					8

/* End Macro definition */
/************************************************************/
/* Enumuration */
/************************************************************/

/* MEDIA_EVENT */
typedef enum
{
	PAL_MEDIA_EVT_VIDEO_FRAMESTART,
	PAL_MEDIA_EVT_VIDEO_UNDERRUN,
	PAL_MEDIA_EVT_VIDEO_FORMAT_CHANGED,
	PAL_MEDIA_EVT_VIDEO_SEQUENCE_HEADER_INFO_CHANGED,
	PAL_MEDIA_EVT_VIDEO_AFD_CHANGED,
	PAL_MEDIA_EVT_VIDEO_ERROR_NONE,

	PAL_MEDIA_eEVT_AUDIO_NEWFRAME,
	PAL_MEDIA_eEVT_AUDIO_NODATA,
	PAL_MEDIA_eEVT_AUDIO_FORMAT_CHANGED,
	PAL_MEDIA_eEVT_AUDIO_STREAM_COMPLETE,

	PAL_MEDIA_EVT_MEDIA_BEGINOFSTREAM,
	PAL_MEDIA_EVT_MEDIA_ERROROFSTREAM,
	PAL_MEDIA_EVT_MEDIA_ENDOFSTREAM,
	PAL_MEDIA_EVT_MEDIA_CHANGEPLAYTIME,
	PAL_MEDIA_EVT_MEDIA_START,
	PAL_MEDIA_EVT_MEDIA_PAUSE,
	PAL_MEDIA_EVT_MEDIA_PLAYING,
	PAL_MEDIA_EVT_MEDIA_TRICK,
	PAL_MEDIA_EVT_MEDIA_STOP,

	PAL_MEDIA_EVT_MEDIA_PROBE_START,
	PAL_MEDIA_EVT_MEDIA_PROBE_COMPLETED,
	PAL_MEDIA_EVT_MEDIA_PROBE_FAILURE,

	PAL_MEDIA_EVT_MEDIA_BUFFER_START,
	PAL_MEDIA_EVT_MEDIA_BUFFER_CANPLAY,
	PAL_MEDIA_EVT_MEDIA_BUFFER_UNDERFLOW,
	PAL_MEDIA_EVT_MEDIA_BUFFER_ENOUGH,
	PAL_MEDIA_EVT_MEDIA_BUFFER_COMPLETE,
	PAL_MEDIA_EVT_MEDIA_BUFFER_FAILURE,
	PAL_MEDIA_EVT_MEDIA_BUFFER_ADAPTIVS_STREAM_CHANGE,

	PAL_MEDIA_EVT_DRM_OUTPUT_CONTROL,

	PAL_MEDIA_EVT_MEDIA_SUB_READY,
	PAL_MEDIA_EVT_MEDIA_SUB_UPDATE,

	PAL_MEDIA_EVT_MEDIA_ERROR_REASON,

	PAL_MEDIA_EVT_MAX,
} PAL_MEDIA_Event_e;


/* Define Audio Decoder Type */
typedef enum {
	PAL_MEDIA_AUDIO_DECODER_MAIN,       // Main decoder (MPEG, AC3 downmix .... )
	PAL_MEDIA_AUDIO_DECODER_SUB,        // Audio Description
	PAL_MEDIA_AUDIO_DECODER_SPDIF,      // AC3 bypass
	PAL_MEDIA_AUDIO_DECODER_MAX,
} PAL_MEDIA_AUDIO_DECODER_TYPE_e;

/* Define video Decoder Type */
typedef enum {
	PAL_MEDIA_VIDEO_DECODER_MAIN,       // Main decoder
	PAL_MEDIA_VIDEO_DECODER_SUB,        // Sub decoder
	PAL_MEDIA_VIDEO_DECODER_MAX,
} PAL_MEDIA_VIDEO_DECODER_TYPE_e;

typedef enum
{
	PAL_MEDIA_FULL_DOWNLOAD,        // only download
	PAL_MEDIA_PLAYABLE_DOWNLOAD,    // progressive download play
	PAL_MEDIA_STREAMING,            // local file playback, streaming
} PAL_MEDIA_DownloadType_e;

typedef enum
{
	PAL_MEDIA_PLAY_START_MODE_BEGIN,
	PAL_MEDIA_PLAY_START_MODE_LASTVIEW,
	PAL_MEDIA_PLAY_START_MODE_END
} PAL_MEDIA_PLAY_START_MODE_e;

typedef enum
{
	PAL_MEDIA_MODE_UNLIMITED,
	PAL_MEDIA_MODE_THRESHOLD,
	PAL_MEDIA_MODE_FIXED,
} PAL_MEDIA_BufferingMode_e;

typedef enum
{
	PAL_MEDIA_ERROR_UNKNOWN=0,
	PAL_MEDIA_ERROR_NETWORK_LINK_DOWN=100,
	PAL_MEDIA_ERROR_NETWORK_DNSLOOKUP_FAIL,
	PAL_MEDIA_ERROR_NETWORK_CONNECTION_REFUSED,
	PAL_MEDIA_ERROR_NETWORK_IGMP_JOIN_FAIL,
	PAL_MEDIA_ERROR_NETWORK_SELECT_TIMEOUT,
	PAL_MEDIA_ERROR_NETWORK_SEEK_TIMEOUT,

	/* the following http status codes are defined according to the HTTP 1.1 Specification : Pls don't touch !!*/
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_CREATED							=201 , /* Section 10.2.2: Created */

	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_MULTIPLE_CHOICE					=300 , /* Section 10.3.1: Multiple Choices */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_UNUSED							=306 , /* Section 10.3.7: Unused field */

	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_BAD_REQUEST						=400 , /* 400 , Section 10.4.1: Bad Request */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_UNAUTHORIZED						=401 , /* 401 , Section 10.4.2: Unauthorized */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_PAYMENT_REQUIRED					=402 , /* 402 , Section 10.4.3: Payment Required */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_FORBIDDEN							=403 , /* 403 , Section 10.4.4: Forbidden */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_NOT_FOUND							=404 , /* 404 , Section 10.4.5: Not Found */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_METHOD_NOT_ALLOWED				=405 , /* 405 , Section 10.4.6: Method Not Allowed */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_NOT_ACCEPTABLE					=406 , /* 406 , Section 10.4.7: Not Acceptable */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_PROXY_AUTH_REQUIRED				=407 , /* 407 , Section 10.4.8: Proxy Authentication Required */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_REQUEST_TIMEOUT					=408 , /* 408 , Section 10.4.9: Request Time-out */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_CONFILICT							=409 , /* 409 , Section 10.4.10: Conflict */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_GONE								=410 , /* 410 , Section 10.4.11: Gone */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_LENGTH_REQUIRED					=411 , /* 411 , Section 10.4.12: Length Required */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_PRECONDITION_FAILED				=412 , /* 412 , Section 10.4.13: Precondition Failed */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE			=413 , /* 413 , Section 10.4.14: Request Entity Too Large */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_REQUEST_URI_TOO_LARGE				=414 , /* 414 , Section 10.4.15: Request-URI Too Large */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE			=415 , /* 415 , Section 10.4.16: Unsupported Media Type */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE 	=416 , /* 416 , Section 10.4.17: Requested range not satisfiable */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_EXPECTATION_FAILED				=417 , /* 417 , Section 10.4.18: Expectation Failed */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_INTERNAL_SERVER_ERROR				=500 , /* 500 , Section 10.5.1: Internal Server Error */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_NOT_IMPLEMENTED					=501 , /* 501 , Section 10.5.2: Not Implemented */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_BAD_GATEWAY						=502 , /* 502 , Section 10.5.3: Bad Gateway */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_SERVICE_UNAVAILABLE				=503 , /* 503 , Section 10.5.4: Service Unavailable */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_GATEWAY_TIMEOUT					=504 , /* 504 , Section 10.5.5: Gateway Time-out */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_VERSION_NOT_SUPPORTED				=505 , /* 505 , Section 10.5.6: HTTP Version not supported */
	PAL_MEDIA_ERROR_SERVER_HTTP_STATUS_MHEG_UNKNOWN						=622 , /* Mheg Specific */

	PAL_MEDIA_ERROR_SERVER_RTSP_ERROR=700,
	PAL_MEDIA_ERROR_SERVER_MMS_ERROR,
	PAL_MEDIA_ERROR_SERVER_IIS_ERROR,
	PAL_MEDIA_ERROR_SYSTEM_NO_MEMORY,
	PAL_MEDIA_ERROR_IO_READ_FAIL,
	PAL_MEDIA_ERROR_IO_WRITE_FAIL,
	PAL_MEDIA_ERROR_IO_SEEK_FAIL,
	PAL_MEDIA_ERROR_DATA_MALFORMED_MANIFEST,
	PAL_MEDIA_ERROR_DATA_MALFORMED_MP4,
	PAL_MEDIA_ERROR_DATA_MALFORMED_TS,
	PAL_MEDIA_ERROR_DATA_NO_INDEX_FILE,

} PAL_MEDIA_ERROR_REASON_e;

typedef enum
{
	PAL_MEDIA_TRACK_TYPE_AUTIO,
	PAL_MEDIA_TRACK_TYPE_VIDEO,
	PAL_MEDIA_TRACK_TYPE_XSUB,
} PAL_MEDIA_TRACK_TYPE_e;

typedef struct PAL_MEDIA_BufferingModeParams_s
{
	HBOOL	bAuto;
	HUINT32 nBufferThreshold;
} PAL_MEDIA_BufferingModeParams_t;

typedef struct PAL_MEDIA_PlaybackStartParams_s
{
	HUINT32 ulStartThreshold;
	HUINT32 ulStartConstant;
} PAL_MEDIA_PlaybackStartParams_t;

typedef void (*PAL_MEDIA_VerifyExternalCallback)(HUINT32 ulTunerId, void *userdata, void *verifyhandle, HINT32 *result);

typedef struct PAL_MEDIA_VerifyExternalCallback_s
{
	PAL_MEDIA_VerifyExternalCallback	verifycallback;
	void 								*userdata;
} PAL_MEDIA_VerifyExternalCallback_t;

/************************************************************/
/* Structure */
/************************************************************/

/* MEDIA Configuration */
typedef struct PAL_MEDIA_CONFIG_s
{
	HBOOL		bInit;
	HBOOL		bUseStorage;				/* TRUE : enable downloading, FALSE : realtime Streaming Only. */
	HINT32		nStreamBufferingSecond;		/* 0 : pure real time,  N : N second prebuffering */
	HINT32		nChunkBaseExponent;			/* 0 : default 8kbytes, N : EXPONENT value of 2. */
	/* Other configuration */
} PAL_MEDIA_CONFIG_t;

typedef struct
{
	HBOOL		bUsed;
	HBOOL		bHdcp;
	HBOOL		bScms;
	HBOOL		bCgms;
	HBOOL		bMacrovision;
} PAL_MEDIA_OutputControl_t;

typedef struct PAL_MEDIA_INFO_s {
	eMEDIA_StreamType_t				stream_type;
	HUINT32							max_bitrate;		/**< maximum stream bitreate, bps or 0 if unknown  */
	HUINT32							duration;       	/**< duration of stream, in milliseconds or 0 if unknown */
	HUINT32							nprograms;      	/**< total number of programs */
	HUINT32							ntracks;        	/**< total number of tracks */

	/* video info */
	HUINT32							video_pes_id;
	HUINT32							video_stream_id;    /**< or Video PID */
	DxVideoCodec_e					video_codec;
	HUINT16 						usVideoWidth;		/**< Video Width */
	HUINT16 						usVideoHeight;		/**< Video Height */

	HUINT8							ucTotalVideoTrack;	/**< Total Video Track */
	struct {
		HUINT32 					video_pes_id;
		HUINT32 					video_stream_id;/**< or Video PID */
		DxVideoCodec_e				video_codec;
	} stVideoTrack[PAL_MEDIA_MAX_VIDEO_TRACK];

	HUINT8							ucTotalXsubTrack; /**< Total Xsub Track Number */
	struct {
		eMEDIA_DivxSubtitleType_t   eXsubType;			/**< Xsub Type */
		HUINT8						uacXsubLang[256];	/**< Xsub Language */
	} stXsubTrack[PAL_MEDIA_MAX_XSUB_TRACK];

	/* audio info */
	HUINT32							audio_pes_id;
	HUINT32							audio_stream_id;    /**< or Audio PID */
	DxAudioCodec_e					audio_codec;

	HUINT8							ucTotalAudioTrack;	/**< Total Audio Track */
	struct {
		HUINT32 					audio_pes_id;
		HUINT32 					audio_stream_id;/**< or Audio PID */
		DxAudioCodec_e				audio_codec;
		HUINT8						uacAudioLang[256];
	} stAudioTrack[PAL_MEDIA_MAX_AUDIO_TRACK];

	HUINT8							support_trick;
	HUINT8							support_seek;
	HUINT32							pcr_pid;        	/**< for TS stream */
	HINT32							demuxId;

	PAL_MEDIA_PLAY_START_MODE_e	eStartCondition;
	HUINT32							ulStartTime;		/**< Specify start time  */

	HINT32							is_drm;
	HUINT8							drminfo[PAL_MEDIA_DRMINFO_MAXNUM];
	HUINT8							is_expired;

	HINT32							protoStatus;		/* Expansion of iptuner error status.(current http only) */
	HUINT8							isIcyProtocol;		/* HTTP Type(ICY? TRUE:FALSE) */

	HBOOL							isSuccesiveSeekMode;
} PAL_MEDIA_INFO_t;

typedef struct PAL_MEDIA_s{
	void							*pIdentifier;

	PAL_MEDIA_DownloadType_e   	eDownloadType;

	HUINT32                    		ulMediaId;  // DI_MEDIA
	PAL_MEDIA_AUDIO_DECODER_TYPE_e eAudioDecoderType;
	PAL_MEDIA_VIDEO_DECODER_TYPE_e eVideoDecoderType;

	HINT8                      		*pUrl;
	HINT8                      		*pSaveFileName;

	PAL_MEDIA_INFO_t				tMediaInfo;

	HUINT32							ulVideoStreamWidth;		/* Video source information */
	HUINT32							ulVideoStreamHeight;		/* Video source information */
	DxAspectRatio_e             eVideoAspectRatio;

	HBOOL							bProbe;
	HBOOL							bVideoStarted;
	HBOOL							bAudioStarted;
	HBOOL							bDlna;
	void                            *pSubContext;

	PAL_MEDIA_ERROR_REASON_e 						uErrorlReason;
	PAL_MEDIA_VerifyExternalCallback_t cbVerifyExternal;	/* Callback about TLS for MHEG */

#if 0 /*  DMP_SI  */
	HINT32							nDemuxId;
#endif
	HCHAR							szDrmLicenseData[512];

	PAL_MEDIA_BufferingModeParams_t	stBufferingMode;
	PAL_MEDIA_OutputControl_t		stOutputControl;
} PAL_MEDIA_t;

typedef struct PAL_MEDIA_BufferStatus_s
{
	HINT64 nBufferedBytes;
	HINT64 nTotalBytesRemaining;
	HINT64 nTotalStreamBytes;
	HINT32 nArrivalBytedPerSecond;
	HINT32 nStreamBytesPerSecond;
	HINT32 nBufferedMilliseconds;
	HUINT32 uiBufferPercentage;
	HUINT32 uiRemainBufferSec;
} PAL_MEDIA_BufferStatus_t;

/** @brief Media Language Info. */
typedef struct tag_PAL_MEDIA_SUB_LANG_INFO
{
	HUINT32         ulTotalLang;
	HUINT32         ulCurIdx;
	HINT8           szDispLang[PAL_MEDIA_SUB_LANGUAGE_MAX][PAL_MEDIA_SUB_LANGUAGE_STRLEN_MAX];
	HINT8           szName[PAL_MEDIA_SUB_LANGUAGE_MAX][PAL_MEDIA_SUB_LANGUAGE_STRLEN_MAX];
} PAL_MEDIA_SUB_LangInfo_t;

typedef struct tag_PAL_MEDIA_SUB_Display_Packet
{
	HUINT32			ulFontSize;
	HUINT32			ulTextPosition;
	HUINT32         ulStrNum;
	HUINT8          pText[PAL_MEDIA_SUB_LINE_MAX][PAL_MEDIA_SUB_STR_MAX];
} PAL_MEDIA_SUB_Display_Packet_t;

typedef struct PAL_MEDIA_AudioTrack_s
{
	HUINT32 					video_pes_id;
	HUINT32 					video_stream_id;/**< or Video PID */
	DxVideoCodec_e				video_codec;
} PAL_MEDIA_VideoTrack_t;

typedef struct PAL_MEDIA_VideoTrack_s
{
	HUINT32 					audio_pes_id;
	HUINT32 					audio_stream_id;/**< or Audio PID */
	DxAudioCodec_e				audio_codec;
	HUINT8						uacAudioLang[256];
} PAL_MEDIA_AudioTrack_t;

typedef struct PAL_MEDIA_XSubTrack_s
{
	eMEDIA_DivxSubtitleType_t   eXsubType;			/**< Xsub Type */
	HUINT8						uacXsubLang[256];	/**< Xsub Language */
} PAL_MEDIA_XSubTrack_t;

typedef struct PAL_MEDIA_MultiTrack_s{
	PAL_MEDIA_TRACK_TYPE_e			type;
	HINT32							track_count;

	union{
		PAL_MEDIA_AudioTrack_t	audio[PAL_MEDIA_MAX_AUDIO_TRACK];
		PAL_MEDIA_VideoTrack_t	video[PAL_MEDIA_MAX_VIDEO_TRACK];
		PAL_MEDIA_XSubTrack_t	xsub[PAL_MEDIA_MAX_XSUB_TRACK];
	}track_info;
} PAL_MEDIA_MultiTrack_t;

typedef struct
{
	DxAudioCodec_e				eCodec;							/**< */
	DxStereoFormat_e 			eStereoType;					/**< */
	HUINT32						ulSampleRate;					/**< */
} PAL_MEDIA_AudioStreamInfo_t;

typedef PAL_MEDIA_t* PAL_MEDIA_HANDLE;

/************************************************************/
/* Define Callback Function */
/************************************************************/

typedef void (*PAL_MEDIA_EventCallback_t)(void *pIdentifier, PAL_MEDIA_Event_e event, HUINT32 param);

/************************************************************/
/* global function prototype */
/************************************************************/
HERROR PAL_MEDIA_Init(PAL_MEDIA_CONFIG_t *stConfig);

HERROR PAL_MEDIA_GetDefaultConfiguration(PAL_MEDIA_CONFIG_t *stConfig);

HERROR PAL_MEDIA_Uninit(void);

PAL_MEDIA_HANDLE PAL_MEDIA_Create (void);

HERROR PAL_MEDIA_Destroy (PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_CreateSession(PAL_MEDIA_HANDLE phMedia, const HINT8 *pURL);

HERROR PAL_MEDIA_DestroySession(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_OpenSession(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_CloseSession(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_ProbeSession(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_ProbeCancel(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_GetProbeInfo(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_INFO_t *pstSetup);

HERROR PAL_MEDIA_SetupSession(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_ReleaseSession(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_SetPosition(PAL_MEDIA_HANDLE phMedia, HUINT64 ullPosition);

HERROR PAL_MEDIA_GetPosition(PAL_MEDIA_HANDLE phMedia, HUINT64 *pullPosition);

HERROR PAL_MEDIA_GetDuration(PAL_MEDIA_HANDLE phMedia, HUINT64 *pullDuration);

HERROR PAL_MEDIA_SetSpeed(PAL_MEDIA_HANDLE phMedia, HINT32 nSpeed);

HERROR PAL_MEDIA_GetSpeed(PAL_MEDIA_HANDLE phMedia, HINT32 *pnSpeed);

HERROR PAL_MEDIA_GetSupportedSpeeds(PAL_MEDIA_HANDLE phMedia, HUINT32 *pulSpeedNum, HINT32 **ppnSpeedArray);

HERROR PAL_MEDIA_StartSession(PAL_MEDIA_HANDLE phMedia, HUINT32 ulStartTime);

HERROR PAL_MEDIA_StopSession(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_PauseSession(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_ResumeSession(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_StartBuffering(PAL_MEDIA_HANDLE phMedia, HINT8 *pSaveFileName);

HERROR PAL_MEDIA_PauseBuffering(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_ResumeBuffering(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_StopBuffering(PAL_MEDIA_HANDLE phMedia);

HERROR PAL_MEDIA_RegisterMediaEventCallback(PAL_MEDIA_EventCallback_t MediaEventCallback);

HERROR PAL_MEDIA_UnregisterMediaEventCallback(void);

HERROR PAL_MEDIA_GetBufferingChunkInfo(PAL_MEDIA_HANDLE phMedia, HUINT32 *pulChunks, HUINT32 **ulStartOffsets, HUINT32 **ulEndOffsets);

HERROR PAL_MEDIA_GetBufferingStatus(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_BufferStatus_t *pstBufferStatus);

HERROR PAL_MEDIA_SetBufferingMode(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_BufferingModeParams_t *pstParam);

HERROR PAL_MEDIA_SetUserAgentString(PAL_MEDIA_HANDLE phMedia, HINT8 *pUAString, HINT32 nSize);

HERROR PAL_MEDIA_SetDrmLicenseData(PAL_MEDIA_HANDLE phMedia, HINT8 *pString, HINT32 nSize);

HERROR PAL_MEDIA_GetOutputControlInfo(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_OutputControl_t *pstOutputControl);

HERROR PAL_MEDIA_SetAudioTrack(PAL_MEDIA_HANDLE phMedia, HUINT8 ucAudioTrack);

HERROR PAL_MEDIA_SetTLS(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_VerifyExternalCallback_t *VerifyExternalCb);

HUINT32 PAL_MEDIA_GetLastErrorReason(PAL_MEDIA_HANDLE phMedia);
#if 0
HERROR PAL_MEDIA_UpdateMultiTrack(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_MultiTrack_t *pTracks);
#endif
HERROR PAL_MEDIA_SetAudioStereoConfig(PAL_MEDIA_HANDLE phMedia, DxStereoSelect_e eStereoConfig);

HERROR PAL_MEDIA_GetAudioStreamInfo(PAL_MEDIA_HANDLE phMedia, PAL_MEDIA_AudioStreamInfo_t *pstStreamInfo);

/* End global function prototypes */
#ifdef  __cplusplus
}
#endif

#endif /* ! __PAL_MEDIA0_H__ */
