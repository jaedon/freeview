#ifndef __CSO_DEF_H__
#define __CSO_DEF_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

/**
 * @defgroup CSO definition
 * @brief
 */

/**
 * @author
 * @brief
 */

/**
 * @note
 * Copyright (C) 2013 Humax Corporation. All Rights Reserved.
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

/*@{*/

/**
 * @file cso_def.h
 */

/* CONFIG */
#ifdef CONFIG_ANDROID
	//#define CONFIG_SUPPORT_BYPASSING 	1
	#define off_t off64_t		/* on Android, off_t is always 32bit */
//	#define CONFIG_FIXED_BBTS_SUPPORT
//	#define CONFIG_FIXED_AES128
//	#define CONFIG_FIXED_DASH_ENC_SUPPORT
#else
	#define CONFIG_SUPPORT_BYPASSING 	1
//	#define CONFIG_FIXED_BBTS_SUPPORT
//	#define CONFIG_FIXED_AES128
//	#define CONFIG_FIXED_DASH_ENC_SUPPORT
#endif

#if 0
typedef enum CSO_BOOL_Type {
	FALSE,
	TRUE
} bool_t;

//#define __bool bool_t;
#else
typedef int8_t bool_t;
#define TRUE 1
#define FALSE 0

/* parkjh4 : for backward compatiability : When om_media_play_mediapb is updated with new error callback, this must be removed! */
//#define CSO_ERROR_CATEGORY_e CSO_EVENT_e

typedef int32_t RETURN_CODE;
/*****************************************************************************
 * Error values (shouldn't be exposed) for IPTUNER/TRANSPORT/UTIL/CONTROLS Layer
 *****************************************************************************/
#define RETURN_SUCCESS          (-0)                 /**< No error */
#define RETURN_ENOMEM           (-1)                 /**< Not enough memory */
#define RETURN_ETHREAD          (-2)                 /**< Thread error */
#define RETURN_ETIMEOUT         (-3)                 /**< Timeout */
#define RETURN_ENETUNREACH	    (-4)					/**< Network unreachable error by transport */
#define RETURN_ENOMOD          (-10)                 /**< Module not found */
#define RETURN_ENOVAR          (-30)                 /**< Variable not found */
#define RETURN_EBADVAR         (-31)                 /**< Bad variable value */
#define RETURN_ENOTSUPPORT     (-40)                 /**< not supported feature */
#define RETURN_ENOITEM         (-50)                 /**< Item not found */
#define RETURN_ERETRY		   (-51)				 /**< Retry */
#define RETURN_EDUPLICATED	   (-52)				 /**< Duplicated item found */
#define RETURN_EEXIT          (-255)                 /**< Program exited */

#define RETURN_VAR_SECTION	  (-500) 				 /**< Variable return value section : this could be redefined by specific sources */
#define RETURN_PROBE_CANCELED (-992)				 /**< Probe canceled by User */
#define RETURN_ERECONN_NEEDED (-993)				 /**< Reconnection is needed */
#define RETURN_EREQFAIL		  (-994)				 /**< Generic Request failed */
#define RETURN_EREQFAIL4xx	  (-995)				 /**< Request failed <4xx error> <in case of http, all of req error case will return this */
#define RETURN_EREQFAIL5xx	  (-996)				 /**< Request failed <5xx error> */
#define RETURN_ECONNFAIL1	  (-997)				 /**< Connection failed 1 (used in INT_HTTP_Connect) */
#define RETURN_ECONNFAIL2	  (-998)				 /**< connection failed 2 (used in INT_HTTP_Connect) */
#define RETURN_ERROR          (-999)                 /**< Generic error */

#endif

#define CSO_MAX_VIDEO_TRACK   32	/**< Default Value of Max VideoTracks */
#define CSO_MAX_AUDIO_TRACK   32 	/**< Default Value of Max AudioTracks */
#define CSO_MAX_SUB_TRACK    32	/**< Default Value of Max SubtitleTracks */

#define CSO_MAX_HTTP_CUSTOMHEADER   8	/**< Default Value of Max HTTP CustomHeader */

#define CSO_DTCPKEY_SIZE	348

#define CSO_EXPOSE_DURATION_INFINITY 0x0
#define CSO_INTERNAL_DURATION_INFINITY 0xFFFFFFFFFFFFFF	/* This must be the same as FEEDER_DURATION_INFINITY */

typedef enum
{
	CSO_ERR_OK = 0,  /**< Success */
	CSO_ERR_FAIL,	/**< Failure */
	CSO_ERR_INVALID_SESSION,	/**< Session ID is invalid */
	CSO_ERR_INVALID_PARAM,	/**< Parameter is invalid */
	CSO_ERR_NULL_PARAM,		/**< Parameter is NULL */
	CSO_ERR_MEMORY_ALLOC,	/**< Memory allocation fail */
	CSO_ERR_SESSION_ALLOC,	/**< Session allocation fail */
	CSO_ERR_SESSION_FULL,	/**< All sessions already has been allocated */
	CSO_ERR_SETTINGS_FAIL,	/**< Setting fail */
	CSO_ERR_PROBE_FAIL,	/**< Probing fail */
	CSO_ERR_SETUP_FAIL,	/**< Setup fail */
	CSO_ERR_RELEASE_FAIL,	/**< Release fail */
	CSO_ERR_START_FAIL,		/**< Start playback fail */
	CSO_ERR_STOP_FAIL,	/**< Stop playback fail */
	CSO_ERR_CLOSE_FAIL,		/**< Close session fail */
	CSO_ERR_DESTROY_FAIL,	/**< Destroy session fail */
	CSO_ERR_ALREADY_STARTED, 	/**< Playback already started */
	CSO_ERR_NOT_READY, 	/**< Session has not been probed yet */
	CSO_ERR_BUFFERING_FAIL,	/**< Buffering fail */
	CSO_ERR_NO_INIT,	/**< CSO has not been initialize yet */
	CSO_ERR_CRITICAL,	/**< Critical error occured */
	CSO_ERR_MAX,
} CSO_ERR;

#define CSO_MEDIATYPE_DEFINE_TO_STR(e) #e

typedef enum
{
    CSO_MEDIATYPE_UNKNOWN = 0,
    CSO_MEDIATYPE_VIDEO,
    CSO_MEDIATYPE_AUDIO,
    CSO_MEDIATYPE_VIDEO_NOMUX_DIRECTFEED,
    CSO_MEDIATYPE_AUDIO_NOMUX_DIRECTFEED,
    CSO_MEDIATYPE_MAX
} CSO_MEDIATYPE;

typedef enum
{
	CSO_ATTACH_SOURCE = 0,
	CSO_ATTACH_SINK,
} CSO_ATTACHTYPE;

typedef enum
{
	CSO_VCODEC_UNKNOWN = 0,
	CSO_VCODEC_H264,
	CSO_VCODEC_H265, /* HEVC */
	CSO_VCODEC_MPEG1,
	CSO_VCODEC_MPEG2,
	CSO_VCODEC_MPEG4Part2,
	CSO_VCODEC_VC1,
	CSO_VCODEC_WMV2,
	CSO_VCODEC_VC1SimpleMain,
	CSO_VCODEC_MSMPEGV3,
	CSO_VCODEC_VP8,
	CSO_VCODEC_VP9
} CSO_VCODEC;

typedef enum
{
	CSO_ACODEC_UNKNOWN = 0,
	CSO_ACODEC_MP3,
	CSO_ACODEC_AAC,
	CSO_ACODEC_AAC_LOAS,
	CSO_ACODEC_AAC_PLUS,
	CSO_ACODEC_AAC_PLUS_LOAS,
	CSO_ACODEC_AAC_PLUS_ADTS,
	CSO_ACODEC_AC3,
	CSO_ACODEC_DTS,
	CSO_ACODEC_AC3_PLUS,
	CSO_ACODEC_MPEG,
	CSO_ACODEC_WMA_STD,
	CSO_ACODEC_WMA_PRO,
	CSO_ACODEC_FLAC,
	CSO_ACODEC_PCM_S16BE,
	CSO_ACODEC_PCM_S16LE,
	CSO_ACODEC_PCM_S24BE,
	CSO_ACODEC_PCM_S24LE,
	CSO_ACODEC_PCM_S32BE,
	CSO_ACODEC_PCM_S32LE,
	CSO_ACODEC_PCM_U8,
	CSO_ACODEC_PCM_S8,
	CSO_ACODEC_PCM_MULAW,
	CSO_ACODEC_GSM_MS,
	CSO_ACODEC_TRUESPEECH,
	CSO_ACODEC_ADPCM_IMA_WAV,
    CSO_ACODEC_ADPCM_MS,
	CSO_ACODEC_PCM_DVD,
	CSO_ACODEC_VORBIS,
	CSO_ACODEC_OPUS,
} CSO_ACODEC;

/**
 *	CSO Event Code
 */
typedef enum
{
	CSO_EVENT_UNKNOWN=0,			/**< Unknown Event */

	CSO_EVENT_PROBE_CANCEL=10,		/**< Probe is canceled */
	CSO_EVENT_PROBE_COMPLETE,		/**< Probe is complete */
	CSO_EVENT_PROBE_FAILURE,		/**< Probe is failure */
	CSO_EVENT_PROBE_UPDATED,		/**< Probe info is updated */
	
	CSO_EVENT_BUFFERING_START=50,	/**< Buffering has been started */
	CSO_EVENT_BUFFERING_ENOUGH,		/**< Buffered data is enough */
	CSO_EVENT_BUFFERING_NOTENOUGH,	/**< Buffered data is NOT enough */
	CSO_EVENT_BUFFERING_COMPLETE,	/**< Buffering is complete */
	CSO_EVENT_BUFFERING_FAILURE,	/**< Buffering is failure */

	CSO_EVENT_MEDIA_RESUME=100,		/**< Media playback is resume */
	CSO_EVENT_MEDIA_PAUSE,			/**< Media playback is paused */

	CSO_EVENT_MEDIA_BEGINOFSTREAM=150,	/**< Media playback has been reached begin of stream */
	CSO_EVENT_MEDIA_ENDOFSTREAM,		/**< Media playback has been reached end of stream */
	CSO_EVENT_MEDIA_ERROROFSTREAM,		/**< Error has occured during playback */
	CSO_EVENT_MEDIA_VIDEO_NEWFRAME,
	CSO_EVENT_MEDIA_AUDIO_NEWFRAME,
	
	CSO_EVENT_MEDIA_SEEK_COMPLETE=200,
	CSO_EVENT_MEDIA_TRICK_COMPLETE,
	CSO_EVENT_MEDIA_TRICK_CHANGE_RATE,

	CSO_EVENT_MEDIA_NEEDMOREDATA=250,
	CSO_EVENT_MEDIA_NEEDRESTART, 					/**< Cso have to do re-probe */
	CSO_EVENT_MEDIA_SELECTED_VIDEO_TRACK_COMPLETE, 	/**< Video track is changed */
	CSO_EVENT_MEDIA_SELECTED_AUDIO_TRACK_COMPLETE, 	/**< Audio track is changed */
	CSO_EVENT_MEDIA_SELECTED_SUBTITLE_TRACK_COMPLETE, 	/**< Audio track is changed */
	CSO_EVENT_MEDIA_NEED_SEEK,
	CSO_EVENT_MEDIA_DURATION_CHANGED,
	CSO_EVENT_MEDIA_AUDIO_FLUSH,
	CSO_EVENT_MEDIA_AUDIO_RESTART,
	CSO_EVENT_MEDIA_UPDATE_BASE_POSITION,			/* If while seek/trick, base position is needed to update immeditely */
	
	CSO_EVENT_MEDIA_BUFFER_UNDERRUN=300,
	CSO_EVENT_MEDIA_BUFFER_ENOUGH,

	CSO_EVENT_MEDIA_EXTRADATA_UPDATED=350,
	
	CSO_EVENT_ERROR_SEEK_FAIL=500,
	CSO_EVENT_ERROR_TRICK_FAIL,
	CSO_EVENT_ERROR_SELECTED_FAIL,

	/**< Advanced error event : this will be propagated via ErrorEventCallback */
	/* Below Error evnets include reason as parameter */
	CSO_EVENT_ADVANCED_ERROR_OTHER=550,
	CSO_EVENT_ADVANCED_ERROR_SERVER,			/* return codes or strings form each server */
	CSO_EVENT_ADVANCED_ERROR_RECOVERED_SERVER,	/* recovered from server error */
	CSO_EVENT_ADVANCED_ERROR_NETWORK,			/* TCP/UDP/Socket Errors */
	CSO_EVENT_ADVANCED_ERROR_RECOVERED_NETWORK,	/* recovered from network error */
	CSO_EVENT_ADVANCED_ERROR_DATA,				/* related to a specified media format */
	CSO_EVENT_ADVANCED_ERROR_RECOVERED_DATA,	/* recovered from data error */
	CSO_EVENT_ADVANCED_ERROR_SYSTEM,			/* related to system call */
	CSO_EVENT_ADVANCED_ERROR_RECOVERED_SYSTEM,	/* recovered from system error */
	CSO_EVENT_ADVANCED_ERROR_IO,				/* related to posix io */
	CSO_EVENT_ADVANCED_ERROR_RECOVERED_IO,		/* recovered from io error */
	CSO_EVENT_ADVANCED_ERROR_DRM_MARLIN,		/* related to Marlin DRM */
	CSO_EVENT_ADVANCED_ERROR_DRM_PLAYREADY,		/* related to MS PlayReady DRM */
	CSO_EVENT_ADVANCED_ERROR_DRM_VIEWRIGHT,		/* related to Viewrightweb DRM */
	CSO_EVENT_ADVANCED_ERROR_RECOVERED_DRM,		/* recovered from DRM error */
	CSO_EVENT_ADVANCED_ERROR_END,

	CSO_EVENT_DRM_VIEWRIGHTWEB=650,
	CSO_EVENT_DRM_PLAYREADY_UPDATE_OUTPUTCONTROL,
	CSO_EVENT_DRM_NAGRAPRM_REQUESTKEY,

	CSO_EVENT_MULTICAST_TUNED=700,
	CSO_EVENT_MULTICAST_LOCKED_TS,
	CSO_EVENT_MULTICAST_LOCKED_TTS,
	CSO_EVENT_MULTICAST_UNLOCKED,

	CSO_EVENT_MAX,
} CSO_EVENT_e;

/*
 * CSO Error Reasons
 */
typedef enum
{
	CSO_ERROR_R_UNKNOWN=0,	/**< Unknown Error */
	CSO_ERROR_R_NETWORK_LINK_DOWN=100,	/**< Network Link Down */
	CSO_ERROR_R_NETWORK_DNSLOOKUP_FAIL,	/**< DNS Lookup fail */
	CSO_ERROR_R_NETWORK_CONNECTION_REFUSED,	/**< Connection has refused */
	CSO_ERROR_R_NETWORK_IGMP_JOIN_FAIL,	/**< IGMP join fail */
	CSO_ERROR_R_NETWORK_SELECT_TIMEOUT,	/**< Select() timeout */
	CSO_ERROR_R_NETWORK_SEEK_TIMEOUT,	/**< Seek timeout */
	CSO_ERROR_R_NETWORK_UNREACHABLE,	/**< Unreachable remote */

	/* the following http status codes are defined according to the HTTP 1.1 Specification : Pls don't touch !!*/
	CSO_ERROR_R_SERVER_HTTP_STATUS_CREATED							=201 , /* Section 10.2.2: Created */

	CSO_ERROR_R_SERVER_HTTP_STATUS_MULTIPLE_CHOICE					=300 , /* Section 10.3.1: Multiple Choices */
	CSO_ERROR_R_SERVER_HTTP_STATUS_UNUSED							=306 , /* Section 10.3.7: Unused field */

	CSO_ERROR_R_SERVER_HTTP_STATUS_BAD_REQUEST						=400 , /* 400 , Section 10.4.1: Bad Request */
	CSO_ERROR_R_SERVER_HTTP_STATUS_UNAUTHORIZED						=401 , /* 401 , Section 10.4.2: Unauthorized */
	CSO_ERROR_R_SERVER_HTTP_STATUS_PAYMENT_REQUIRED					=402 , /* 402 , Section 10.4.3: Payment Required */
	CSO_ERROR_R_SERVER_HTTP_STATUS_FORBIDDEN							=403 , /* 403 , Section 10.4.4: Forbidden */
	CSO_ERROR_R_SERVER_HTTP_STATUS_NOT_FOUND							=404 , /* 404 , Section 10.4.5: Not Found */
	CSO_ERROR_R_SERVER_HTTP_STATUS_METHOD_NOT_ALLOWED				=405 , /* 405 , Section 10.4.6: Method Not Allowed */
	CSO_ERROR_R_SERVER_HTTP_STATUS_NOT_ACCEPTABLE					=406 , /* 406 , Section 10.4.7: Not Acceptable */
	CSO_ERROR_R_SERVER_HTTP_STATUS_PROXY_AUTH_REQUIRED				=407 , /* 407 , Section 10.4.8: Proxy Authentication Required */
	CSO_ERROR_R_SERVER_HTTP_STATUS_REQUEST_TIMEOUT					=408 , /* 408 , Section 10.4.9: Request Time-out */
	CSO_ERROR_R_SERVER_HTTP_STATUS_CONFILICT							=409 , /* 409 , Section 10.4.10: Conflict */
	CSO_ERROR_R_SERVER_HTTP_STATUS_GONE								=410 , /* 410 , Section 10.4.11: Gone */
	CSO_ERROR_R_SERVER_HTTP_STATUS_LENGTH_REQUIRED					=411 , /* 411 , Section 10.4.12: Length Required */
	CSO_ERROR_R_SERVER_HTTP_STATUS_PRECONDITION_FAILED				=412 , /* 412 , Section 10.4.13: Precondition Failed */
	CSO_ERROR_R_SERVER_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE			=413 , /* 413 , Section 10.4.14: Request Entity Too Large */
	CSO_ERROR_R_SERVER_HTTP_STATUS_REQUEST_URI_TOO_LARGE				=414 , /* 414 , Section 10.4.15: Request-URI Too Large */
	CSO_ERROR_R_SERVER_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE			=415 , /* 415 , Section 10.4.16: Unsupported Media Type */
	CSO_ERROR_R_SERVER_HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE 	=416 , /* 416 , Section 10.4.17: Requested range not satisfiable */
	CSO_ERROR_R_SERVER_HTTP_STATUS_EXPECTATION_FAILED				=417 , /* 417 , Section 10.4.18: Expectation Failed */
	CSO_ERROR_R_SERVER_HTTP_STATUS_INTERNAL_SERVER_ERROR				=500 , /* 500 , Section 10.5.1: Internal Server Error */
	CSO_ERROR_R_SERVER_HTTP_STATUS_NOT_IMPLEMENTED					=501 , /* 501 , Section 10.5.2: Not Implemented */
	CSO_ERROR_R_SERVER_HTTP_STATUS_BAD_GATEWAY						=502 , /* 502 , Section 10.5.3: Bad Gateway */
	CSO_ERROR_R_SERVER_HTTP_STATUS_SERVICE_UNAVAILABLE				=503 , /* 503 , Section 10.5.4: Service Unavailable */
	CSO_ERROR_R_SERVER_HTTP_STATUS_GATEWAY_TIMEOUT					=504 , /* 504 , Section 10.5.5: Gateway Time-out */
	CSO_ERROR_R_SERVER_HTTP_STATUS_VERSION_NOT_SUPPORTED				=505 , /* 505 , Section 10.5.6: HTTP Version not supported */
	CSO_ERROR_R_SERVER_HTTP_STATUS_MHEG_UNKNOWN						=622 , /* Mheg Specific */

	CSO_ERROR_R_SERVER_RTSP_ERROR=700,	/**< RTSP Error */
	CSO_ERROR_R_SERVER_MMS_ERROR,		/**< MMS Error */
	CSO_ERROR_R_SERVER_IIS_ERROR,		/**< IIS Error */
	CSO_ERROR_R_SERVER_OUT_OF_WINDOW,	/** Out of window related errors : abstracted (such as DASH request the segment that is out of timeshift buffer */
	CSO_ERROR_R_SYSTEM_NO_MEMORY,	/**< Memory allocation fail */
	CSO_ERROR_R_IO_READ_FAIL,	/**< Read I/O Fail */
	CSO_ERROR_R_IO_WRITE_FAIL,	/**< Write I/O Fail */
	CSO_ERROR_R_IO_SEEK_FAIL,	/**< Seek I/O Fail */
	CSO_ERROR_R_DATA_MALFORMED_GENERIC,	/**< Manifest is malformed */
	CSO_ERROR_R_DATA_MALFORMED_MANIFEST,	/**< Manifest is malformed */
	CSO_ERROR_R_DATA_MALFORMED_MP4,	/**< MP4 Container is malformed */
	CSO_ERROR_R_DATA_MALFORMED_TS,	/**< TS Container is malformed */
	CSO_ERROR_R_DATA_NO_INDEX_FILE,	/**< No index in file */
	CSO_ERROR_R_DATA_MALFORMED_MHEG_X_KEY,	/**< MHEG X-KEY is malformed */

} CSO_ERROR_REASON_e;

typedef struct CSO_Status_s
{
	bool_t	bInit;
	int32_t nOpened;
//	int32_t nProbing;
	int32_t nPlaying;
} CSO_Status_t;

/**
 *	CSO Event Category
 */
typedef enum
{
	CSO_EVENT_CATEGORY_UNKNOWN=0,	/**< Unknown Event */
	CSO_EVENT_CATEGORY_BUFFERING,	/**< Events for Buffering */
	CSO_EVENT_CATEGORY_MEDIA,	/**< Events for Media playback */
	CSO_EVENT_CATEGORY_DRM,	/**< Events for DRM */
	CSO_EVENT_CATEGORY_RESULT,	/**< The result of the function call */
	CSO_EVENT_CATEGORY_ERROR, 	/**< Events for error */
	CSO_EVENT_CATEGORY_ADVANCED_ERROR,	/**< Events for advanced error */
	CSO_EVENT_CATEGORY_MULTICAST /**< Events for Multicast */
} CSO_EVENT_CATEGORY_e;


/** 
  * CSO Error Categor
  */
typedef enum
{
	CSO_ERROR_OTHER=0x0,
	CSO_ERROR_SERVER=0x7, 	/* return codes or strings form each server */
	CSO_ERROR_NETWORK, 	/* TCP/UDP/Socket Errors */
	CSO_ERROR_DATA,		/* related to a specified media format */
	CSO_ERROR_SYSTEM,		/* related to system call */
	CSO_ERROR_IO,			/* related to posix io */
	CSO_ERROR_DRM_MARLIN=0xF,	/* related to Marlin DRM */
	CSO_ERROR_DRM_PLAYREADY,	/* related to MS PlayReady DRM */
	CSO_ERROR_DRM_VIEWRIGHTWEB,	/* related to Viewrightweb DRM */
} CSO_ERROR_CATEGORY_e;

/**
 * Media Subtitle Type
 */
typedef enum
{
	eCSO_SUBTITLE_NONE = 0, /**< None */
	eCSO_SUBTITLE_DVB, /**< DVB Subtitle */
	eCSO_SUBTITLE_EBU, /**< DVB Subtitle */
	eCSO_SUBTITLE_XSUB_DXSA, /**< Divx XSUB */
	eCSO_SUBTITLE_XSUB_DXSB, /**< Divx XSUB + */
	eCSO_SUBTITLE_TTML, /**< TTML */
	eCSO_SUBTITLE_TEXT, /**< Raw TEXT */
} CSO_SUBTITLE_TYPE_e;
/**
 *	CSO Playback Mode
 */
typedef enum
{
	eCSO_MODE_PLAY, /* Real Time Streaming Play, Local File Play */
	eCSO_MODE_DNP, /* Download and Play */
	eCSO_MODE_DOWNLOAD, /* Download */
} CSO_PlayMode_e;

/**
 *	CSO PlayState
 */
typedef enum
{
	eCSO_STATE_NONE = 0,	/**< Not ready */
	eCSO_STATE_READY = 0x0001,	/**< Ready  */
	eCSO_STATE_BUFFERING = 0x0010,	/**< Buffering */
	eCSO_STATE_PLAY = 0x0100,	/**< Playing */
	eCSO_STATE_DNP = 0x0110,	/**< Progressive Download */
} CSO_PlayState_e;

/**
 *	User Timebase
 */
typedef enum
{
	eCSO_TIMEBASE_MSEC = 0, 	/**< Millisecs */
	eCSO_TIMEBASE_USEC,	/**< Mircosecs */
	eCSO_TIMEBASE_SEC,	/**< Seconds */
} CSO_TimeBase_e;

/**
 *	CSO Control Commands
 */
typedef enum
{
	eCSO_CTRL_STARTBUFFERING, 	/**< Start buffering */
	eCSO_CTRL_STOPBUFFERING, 	/**< Stop buffering */
	eCSO_CTRL_PAUSEBUFFERING,	/**< Pause buffering */
	eCSO_CTRL_RESUMEBUFFERING,	/**< Resume buffering */
	eCSO_CTRL_SEEK, 	/**< Seek */
	eCSO_CTRL_SEEK_SEGMENT, /**< Seek */
	eCSO_CTRL_TRICK,	/**< Trick */
	eCSO_CTRL_PAUSE,	/**< Pause playback */
	eCSO_CTRL_RESUME,	/**< resume playback */
	eCSO_CTRL_SELECT_VIDEO_TRACK,	/**< Select video track */
	eCSO_CTRL_SELECT_AUDIO_TRACK,	/**< Select audio track */
	eCSO_CTRL_SELECT_SUBTITLE_TRACK, /**< Select subtitle track */
} CSO_CTRL_e;

/**
 *  Resume target for eCSO_CTRL_RESUME's parameter
 */
typedef enum
{
	eCSO_RESUME_ALL = 0,
	eCSO_RESUME_PUMPER,
	eCSO_RESUME_OPERATOR
} CSO_CTRL_RESUME_TARGET_e;

/**
 *	CSO Control Parameters
 */
typedef struct CSO_CTRL_PARAM_s
{
	int nTrick; /**< The rate enumeration for trick operation */
	uint64_t ullPosition; /**< The timeoffset for seek operation */

	int nParam; /**< Integer Param for Control */
	uint32_t ulParam1; /**< uint32 Param for Control */
	uint64_t ullParam2;  /**< uint64 Param for Control */

	void* pParam3;  /**< void pointer param for Control */
} CSO_CTRL_PARAM_t;

#define CSO_PIPE_TRANSFER_TYPE_RECEIVER_MASK	0xF0	/** Receiver Mask */

typedef enum
{
	CSO_DECRYPTOR_TYPE_UNKNOWN = 0,	/**< UNKNOWN */
	CSO_DECRYPTOR_TYPE_AES_128_CTR,
	CSO_DECRYPTOR_TYPE_AES_128_CBC,
	CSO_DECRYPTOR_TYPE_AES_128_ECB,
	CSO_DECRYPTOR_TYPE_AES_128_CTR_EX,
	CSO_DECRYPTOR_TYPE_WMDrmPd,
	CSO_DECRYPTOR_TYPE_RC4,
	CSO_DECRYPTOR_TYPE_MAX
} CSO_DECRYPTOR_Type_e;

/**
 *	CSO DRM Types
 */
typedef enum
{
	eCSO_DRM_NONE = 0,
	eCSO_DRM_PLAYREADY,
	eCSO_DRM_WMDRM,
	eCSO_DRM_VIEWRIGHTWEB,
	eCSO_DRM_NAGRAPRM,
	eCSO_DRM_MARLIN,
} CSO_DRMType_e;

/**	
 *	CSO Buffering Types
 */	
typedef enum
{
	eCSO_RAMBUFFERING_ENHANCED = 0,
	eCSO_RAMBUFFERING_PREBUFFERING,
	eCSO_RAMBUFFERING_NOTUSED, // not supported	
} CSO_RamBufferingMode_e;

typedef struct CSO_PLAYREADY_MINIMUM_OUTPUT_PROTECTION_LEVELS
{
    uint16_t wCompressedDigitalVideo;
    uint16_t wUncompressedDigitalVideo;
    uint16_t wAnalogVideo;
    uint16_t wCompressedDigitalAudio;
    uint16_t wUncompressedDigitalAudio;
} CSO_PlayReady_Minimum_OutputControl_t;

typedef struct CSO_PLAYREADY_GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8];
} CSO_PlayReady_GUID;

typedef struct CSO_PLAYREADY_OPL_OUTPUT_IDS
{
    uint16_t cIds;
    CSO_PlayReady_GUID *rgIds;
} CSO_PlayReady_OPL_OUTPUT_IDS;

typedef struct CSO_PlayReady_OUTPUT_PROTECTION_EX
{
    uint32_t dwVersion;
    CSO_PlayReady_GUID guidId;
    uint32_t dwConfigData;
} CSO_PlayReady_OUTPUT_PROTECTION_EX;

typedef CSO_PlayReady_OUTPUT_PROTECTION_EX CSO_PlayReady_VIDEO_OUTPUT_PROTECTION_EX;
typedef CSO_PlayReady_OUTPUT_PROTECTION_EX CSO_PlayReady_AUDIO_OUTPUT_PROTECTION_EX;

typedef struct CSO_PLAYREADY_VIDEO_OUTPUT_PROTECTION_IDS_EX
{
    uint32_t dwVersion;
    uint16_t cEntries;
    CSO_PlayReady_VIDEO_OUTPUT_PROTECTION_EX *rgVop;
} CSO_PlayReady_VIDEO_OUTPUT_PROTECTION_IDS_EX;

typedef struct CSO_PLAYREADY_AUDIO_OUTPUT_PROTECTION_IDS_EX
{
    uint32_t dwVersion;
    uint16_t cEntries;
    CSO_PlayReady_AUDIO_OUTPUT_PROTECTION_EX *rgAop;
} CSO_PlayReady_AUDIO_OUTPUT_PROTECTION_IDS_EX;

typedef struct CSO_PLAYREADY_OUTPUTCONTROL {
	uint32_t dwVersion;
	CSO_PlayReady_Minimum_OutputControl_t minOPL;
	CSO_PlayReady_OPL_OUTPUT_IDS oplIdReserved;
	CSO_PlayReady_VIDEO_OUTPUT_PROTECTION_IDS_EX vopi;
	CSO_PlayReady_AUDIO_OUTPUT_PROTECTION_IDS_EX aopi;
} CSO_Playready_OutputControl_t;

typedef struct CSO_OUTPUTCONTTOL {
	CSO_DRMType_e drm_type;

	union {
		CSO_Playready_OutputControl_t playready;
	} data;
} CSO_OutputControl_t;

typedef struct CSO_DrmVmxInfo_s
{
	uint32_t unVmxUniqueIdLen;
	int8_t *pVmxUniqueId;
	int8_t *pVmxVersion;
} CSO_DrmVmxInfo_t;

typedef struct CSO_DrmNagraPRMInfo_s
{
	uint32_t unPrmsyntaxLen;
	uint8_t *prmsyntax;
} CSO_DrmNagraPRMInfo_t;

typedef struct CSO_DrmInfo_s
{
	CSO_DRMType_e eCurDrmType;
	CSO_DrmVmxInfo_t vmxInfo;
	CSO_DrmNagraPRMInfo_t nagraPRMInfo;
	// CSO_OutputControl_t outputctrl;
	/* TBD */
} CSO_DrmInfo_t;

/**
 *	CSO PipeType for PUMPER
 */
typedef enum
{
	eCSO_PIPE_TRANSFER_TYPE_UNKNOWN = -1, 	/**< UNKNOWN */
	eCSO_PIPE_TRANSFER_TYPE_MEMORYBUFFER = 0,	/**< Memory Copy */
	eCSO_PIPE_TRANSFER_TYPE_SHAREDMEM,	/**< Sharedmemory */
	eCSO_PIPE_TRANSFER_TYPE_PIPE,	/**< Named pipe */
	eCSO_PIPE_TRANSFER_TYPE_SOCKET,	/**< Socket */
} CSO_PipeType_e;

/**
 *	CSO Transcoding Mode
 */
typedef enum
{
	eCSO_TRANSMODE_RAW,	/**< Do not transcode */
	eCSO_TRANSMODE_TS,	/**< to MpegTS */
	eCSO_TRANSMODE_PES	/**< to MpegPES */
} CSO_TranscodeMode_e;

/**
 *	CSO Buffering Mode
 */
typedef enum
{
	eCSO_BUFFERING_UNLIMITED,	/**< Unlimited */
	eCSO_BUFFERING_THRESHOLD,	/**< Threshold */
	eCSO_BUFFERING_FIXED,	/**< Fixed */
} CSO_BufferingMode_e;

typedef enum
{
	eCSO_SOC_MEMORY_PROCESS_HEAP,
	eCSO_SOC_MEMORY_HEAP,
	eCSO_SOC_MEMORY_SECURE,
} CSO_SOC_MEMORY_TYPE_e;

typedef struct CSO_BufferingModeParams_s
{
	uint32_t nBufferThreshold;
	bool_t bFastFillIsLimited;
	double dFastFillLimitMultiple;
	double dManagedFillLimitMultiple;
} CSO_BufferingModeParams_t;

typedef struct CSO_PlaybackStartParams_s
{
	uint32_t ulStartThreshold;
	uint32_t ulStartConstant;
} CSO_PlaybackStartParams_t;

typedef enum
{
	eCSO_UNDERFLOW_NONSTOP=0,
	eCSO_UNDERFLOW_THRESHOLD
} CSO_UnderflowMode_e;

#define MAKE_LANG(c1,c2,c3) ( ((uint32_t)c1) | ((uint32_t)c2<<8) | ((uint32_t)c3<<16) | (0 << 24) )
typedef enum CSO_LangCode_s
{
	  CSO_LANG_NONE		= 0x00000000

	, CSO_LANG_Albanian		= MAKE_LANG('a','l','b')
	, CSO_LANG_Arabic		= MAKE_LANG('a','r','a')
	, CSO_LANG_Australian	= MAKE_LANG('a','u','s')
	, CSO_LANG_Basque		= MAKE_LANG('b','a','q')
	, CSO_LANG_Basaa		= MAKE_LANG('b','a','s')
	, CSO_LANG_Bulgarian	= MAKE_LANG('b','u','l')
	, CSO_LANG_Catalan		= MAKE_LANG('c','a','t')
	, CSO_LANG_Czech		= MAKE_LANG('c','z','e')
	, CSO_LANG_Danish		= MAKE_LANG('d','a','b')
	, CSO_LANG_Dutch		= MAKE_LANG('d','u','t')
	, CSO_LANG_English		= MAKE_LANG('e','n','g')
	, CSO_LANG_Estonian		= MAKE_LANG('e','s','t')
	, CSO_LANG_Faroese		= MAKE_LANG('f','a','o')
	, CSO_LANG_Finnish		= MAKE_LANG('f','i','n')
	, CSO_LANG_French		= MAKE_LANG('f','r','e')
	, CSO_LANG_German		= MAKE_LANG('g','e','r')
	, CSO_LANG_Gaelic		= MAKE_LANG('g','l','a')
	, CSO_LANG_Irish		= MAKE_LANG('g','l','e')
	, CSO_LANG_Galician		= MAKE_LANG('g','l','g')
	, CSO_LANG_Greek		= MAKE_LANG('g','r','e')
	, CSO_LANG_Hebrew		= MAKE_LANG('h','e','b')
	, CSO_LANG_Hindi		= MAKE_LANG('h','i','n')
	, CSO_LANG_Hungarian	= MAKE_LANG('h','u','n')
	, CSO_LANG_Icelandic	= MAKE_LANG('i','c','e')
	, CSO_LANG_Italian		= MAKE_LANG('i','t','a')
	, CSO_LANG_Japanese		= MAKE_LANG('j','p','n')
	, CSO_LANG_Korean		= MAKE_LANG('k','o','r')
	, CSO_LANG_Latvian		= MAKE_LANG('l','a','v')
	, CSO_LANG_Lithuanian	= MAKE_LANG('l','i','t')
	, CSO_LANG_Narration	= MAKE_LANG('n','a','r')
	, CSO_LANG_Norwegian	= MAKE_LANG('n','o','r')
	, CSO_LANG_Persian		= MAKE_LANG('p','e','r')
	, CSO_LANG_Polish		= MAKE_LANG('p','o','l')
	, CSO_LANG_Portuguese	= MAKE_LANG('p','o','r')
	, CSO_LANG_OriginalAudio= MAKE_LANG('q','a','a')
	, CSO_LANG_Romanian		= MAKE_LANG('r','u','m')
	, CSO_LANG_Russian		= MAKE_LANG('r','u','s')
	, CSO_LANG_Croatian		= MAKE_LANG('s','c','r')
	, CSO_LANG_Slovak		= MAKE_LANG('s','l','o')
	, CSO_LANG_Slovenian	= MAKE_LANG('s','l','v')
	, CSO_LANG_Spanish		= MAKE_LANG('s','p','a')
	, CSO_LANG_Serbian		= MAKE_LANG('s','r','p')
	, CSO_LANG_Swedish		= MAKE_LANG('s','w','e')
	, CSO_LANG_Thai			= MAKE_LANG('t','h','a')
	, CSO_LANG_Turkish		= MAKE_LANG('t','u','r')
	, CSO_LANG_Ukrainian	= MAKE_LANG('u','k','r')
	, CSO_LANG_Undefined	= MAKE_LANG('u','n','d')
	, CSO_LANG_Vietnamese	= MAKE_LANG('v','i','e')
	, CSO_LANG_Welsh		= MAKE_LANG('w','e','l')
	, CSO_LANG_Miscellaneous= MAKE_LANG('m','i','s')
	, CSO_LANG_Multiple		= MAKE_LANG('m','u','l')

	, CSO_LANG_MAX			= 0x00FFFFFF
} CSO_LangCode_t;

typedef void (*CSO_MediaEventCallback) (uint32_t ulSessionId, int32_t ev, int64_t param1);
typedef void (*CSO_SessionMediaEventCallback) (void *object, uint32_t ulSessionId, int32_t ev, int64_t param1);
typedef void (*CSO_ErrorEventCallback) (uint32_t ulSessionId, CSO_EVENT_e eErrorEvent, int64_t /*CSO_ERROR_REASON_e*/ nReason);

typedef void (*CSO_VerifyExternalCallback)(uint32_t ulSessionId, void *pUserdata, void *pVerifyhandle, int32_t *pnResult);

typedef struct CSO_VerifyExternalCallback_s
{
	CSO_VerifyExternalCallback			cbVerify;
	void 								*pUserdata;
} CSO_VerifyExternalCallback_t;


/* For android amlogic */
typedef RETURN_CODE (*CSO_DrmGetLicenseCallback) (void *owner_handle, void *pData, int nLen, int8_t *pUrl /* Manifest */);
typedef RETURN_CODE (*CSO_DrmVideoDecryptorCallback) (void *owner_handle, void *Data, int nLen, void *pDecSecureBuffer, uint32_t nSampleOffset, uint32_t nClearDataSize, uint8_t *pIv);
typedef RETURN_CODE (*CSO_DrmAudioDecryptorCallback) (void *owner_handle, void *pData, int nLen, uint8_t *pIv);
typedef RETURN_CODE (*CSO_DrmGetDecryptBufferCallback) (void *owner_handle, void **pRetBuffer, int *pLen);
typedef struct CSO_DrmActuatorCallback_s
{
	void *pOwnerHandle;
	CSO_DrmGetLicenseCallback		cbGetLicense;
	CSO_DrmVideoDecryptorCallback 	cbVideoDecryptor;
	CSO_DrmAudioDecryptorCallback 	cbAudioDecryptor;
	CSO_DrmGetDecryptBufferCallback	cbGetDecryptBuffer;
} CSO_DrmActuatorCallback_t;

/**
 * Media Tls Certi & Key Path Setting
 */
typedef struct CSO_Tls_Settings_s
{
	int8_t						*pTls_ca_name;
	int8_t						*pTls_ca_path;
	int8_t						*pTls_certi_path;
	int8_t						*pTls_prikey_path;
	int8_t						*pTls_pripw;
	void						*pTls_instance;
	int32_t						npTls_value;
	CSO_VerifyExternalCallback_t	cbTlsCertificate;
} CSO_Tls_Settings_t;

typedef struct CSO_PRDY_Settings_s
{
	int8_t *pPrivateKeyPath;			/* for https playready licence request.*/
	int8_t *pCertificatePath;
	uint8_t *pucLAURL;
	uint8_t *pucLicenseData;
	uint8_t *pucCustomData;
	bool_t	bIsLiveTV;
} CSO_PRDY_Settings_t;

typedef struct CSO_VRW_Settings_s
{
	uint8_t *pCompanyName;
	uint8_t *pVCASBootAddr;
} CSO_VRW_Settings_t;

typedef struct CSO_NAGRAPRM_Settings_s
{
	uint32_t hHdl;
	uint16_t usEmi;
	uint32_t ulKeySize;
	uint8_t	ucEncryptKey[128];
} CSO_NAGRAPRM_Settings_t;

typedef struct CSO_Dlna_Setting_s
{
	int32_t		nByteRate;
	int64_t		nFileSize;
	uint32_t		unDuration;
	uint32_t		unSampleRate;
	uint32_t		unBitsPerSample;
	bool_t		bIsStereo;
} CSO_Dlna_Setting_t;

typedef struct CSO_Raop_Setting_s
{
	uint32_t		unDestIpAddress;
	uint8_t			ucAesKey[16];
	uint8_t			ucAesIv[16];
	uint8_t			ucFmtStr[64];
	uint8_t			ucAudioCodec[8];
	uint32_t		unDataPort;
	uint32_t		unControlPort;
	uint32_t		unTimingPort;
} CSO_Raop_Setting_t;

typedef struct CSO_DtcpIp_Setting_s
{
	void		*AkeHandle;
} CSO_DtcpIp_Setting_t;

typedef struct CSO_HttpCustomHeader_s
{
	uint8_t *pName;
	uint8_t *pValue;
} CSO_HttpCustomHeader_t;

typedef struct CSO_Settings_s
{
	uint32_t					nMaxSessions;
	unsigned char*				pSaveFilePath;
	CSO_TimeBase_e				eUserTimeBase;
	CSO_BufferingMode_e 		eBufferingMode;
	CSO_BufferingModeParams_t 	tBufferingParams;
	CSO_PlaybackStartParams_t 	tPlaybackStartParams;
	CSO_UnderflowMode_e 		eUnderflowMode;
	uint64_t 					ullStartTime;
	uint64_t					ullInternalSeekedTime;
	uint32_t 					ulEndTime;
	bool_t						bEnableSMP;					/* Enable Secure Media Path */
	bool_t						bAccurateSeek;
	int32_t 					nDecoderRate;
	bool_t						bAsyncProbe;
	bool_t						bBypassing;
	bool_t						bSelectAudioDescription;
	CSO_LangCode_t				ePreperredAudioLanguage;	/* This is for adaptive streaming : which language of track should we select? */
//	DI_MEDIA_AdaptiveSettings_t tAdaptiveSettings;
	CSO_Tls_Settings_t			tTlsSetting;
	CSO_PRDY_Settings_t 		tPRDYSetting;
	CSO_VRW_Settings_t 			tVRWSetting;
	CSO_NAGRAPRM_Settings_t 	tNAGRAPRMSetting;

	/* for Adaptive Streaming */
	bool_t						bStartLowestStream;
	uint64_t					ullStartStream; 
	bool_t						bExitLiveBeforeUpdate;/* If this flag is on and it played last segment in current playlist on live mode, the playback will be exit after last segment. */
	uint32_t					unDelayThresholdForGearUp; /* This value is ratio. Even if the heuristics calculates any result, when the delay of segment download is beyond the segment duration that applied this ratio, cannot up switching bitrate. */
	
	/* for Ram Buffering & Caching */
	CSO_RamBufferingMode_e		eRamBufferingMode;
	uint32_t					uiBlockSize; /* when used in 16 bytes, performance was good. */
	uint32_t					uiRambufferSize;
	uint32_t					uiProbebufferSize;

	/* for Multicast */
	unsigned char*				pChangeUrl; /* for multicast re-tune */

	uint32_t					unHttpCustomHeaders;
	CSO_HttpCustomHeader_t		tHttpCustomHeader[CSO_MAX_HTTP_CUSTOMHEADER];

	CSO_Raop_Setting_t			tRaopSetting;
	CSO_Dlna_Setting_t			tDlnaSetting;
	CSO_DtcpIp_Setting_t		tDtcpIpSetting;
	uint8_t						*pDtcpKey;
	CSO_PipeType_e				ePipeType;
	CSO_TranscodeMode_e			eTransMode;
	CSO_DRMType_e				eDrmType;
	int8_t						*pProtocolInfo;

	uint8_t						*pUserAgentString;
//	int32_t						nDemuxId;

	CSO_MediaEventCallback		cbMediaEvent;
	CSO_ErrorEventCallback		cbErrorEvent;
	CSO_DrmActuatorCallback_t	cbDrmActuatorSet;
	
	uint64_t					ullDecThresholdSW;
} CSO_Settings_t;
/*
 * *****************************
 * BUFFERING STATUS Snapshot
 * *****************************
 * derived from YouView Project
 *
 * (llBufferedBytes)
 * For both progressive and adaptive source streams,
 * bufferedBytes shall contain the number of bytes
 * in the buffer from the current play point
 * to the end of the current buffer region (as defined in section 7.3.1.2).
 *
 * (llTotalBytesRemaining)
 * If the source stream has no defined endpoint, this attribute shall be -1.
 * If the source stream does have a defined endpoint, then:
 * - For progressive source streams,
 *   this attribute shall contain the number of bytes
 *   from the current play-position to the end of the stream.
 * - For adaptive source streams, this attribute shall be -1.
 *
 * (llTotalStreamBytes)
 * If the source stream has no defined endpoint,
 * the totalStreamBytes attribute shall be -1.
 * If the source stream does have a defined endpoint, then:
 * - For progressive source streams,
 *   this attribute shall contain the total number of bytes in the source stream.
 * - For adaptive source streams, this attribute shall be -1.
 *
 * (nArrivalBytedPerSecond)
 * For both progressive and adaptive source streams:
 * Given T = elapsed time since buffering last started,
 * - If T < 2 seconds
 *   , arrivalBytesPerSecond shall be the mean rate of bytes arriving
 *   in the MediaRouter's buffer over the last T seconds.
 * - Otherwise, arrivalBytesPerSecond shall be the mean rate of bytes arriving
 *   in the MediaRouter's buffer over the last 2 seconds.
 *
 * (nStreamBytesPerSecond)
 * For progressive source streams
 * , streamBytesPerSecond shall be -1.
 * For adaptive source streams
 * , it shall be the average playback bitrate of the stream that is currently being buffered.
 *
 * (nBufferedMilliseconds)
 * For both progressive and adaptive source streams,
 * bufferedMilliseconds shall be the number of milliseconds of media time
 * from the play-position to the end of the current buffer region (as defined in section 7.3.1.2).
 */

typedef struct CSO_BufferStatus_s
{
	int64_t llBufferedBytes;
	int64_t llTotalBytesRemaining;
	int64_t llTotalStreamBytes;

	int32_t nArrivalBytedPerSecond;
	int32_t nStreamBytesPerSecond;
	int32_t nBufferedMilliseconds;

/*	MAXDOME	*/
	uint32_t uiBufferPercentage;
	uint32_t uiRemainBufferSec;
} CSO_BufferStatus_t;

typedef struct CSO_BufferChunkStatus_s
{
	uint32_t uiChunks;
	union {
		struct {
			uint64_t *pullStartOffsets;
			uint64_t *pullEndOffsets;
		};
		
		/* parkjh4 :: [2015/09/08] Backward compatibility for calypso users. buffer info type is changed from 32bit to 64bit. */
		struct {
			uint64_t *iStartOffsets;
			uint64_t *iEndOffsets;
		};
	};
} CSO_BufferChunkStatus_t;

typedef struct CSO_SeekableRangeStatus_s
{
	uint32_t uiChunks;
	uint64_t *pullSeekableStart;
	uint64_t *pullSeekableEnd;
} CSO_SeekableRangeStatue_t;

typedef struct CSO_VideoInfo_s
{
	uint64_t ullPlayTime;
	uint64_t pts;
	int32_t nDecodedFrames;
	int32_t nDropFrames;	
	int32_t nErrorFrames;
	int32_t nWidth;
	int32_t nHeight;
	int64_t llBufferedSize;
	int32_t nBufferedCount;
} CSO_VideoInfo_t;

typedef struct CSO_AudioInfo_s
{
	uint64_t ullPlayTime;
	uint64_t pts;
	int32_t nDecodedFrames;
	int32_t nDropFrames;
	int32_t nErrorFrames;
	int32_t nSampleRate;
	int32_t nChannels;	
	int64_t llBufferedSize;
	int32_t nBufferedCount;
} CSO_AudioInfo_t;

typedef struct CSO_PlayInfo_s
{
	uint64_t ullPlaybackTime;
	CSO_VideoInfo_t vinfo;
	CSO_AudioInfo_t ainfo;
} CSO_PlayInfo_t;

typedef struct CSO_BufferInfo_s
{
	uint64_t ullLastDecodeTime;
	CSO_BufferStatus_t tBufferStatus;
	CSO_BufferChunkStatus_t tBufferChunkStatus;
	CSO_SeekableRangeStatue_t tSeekableRange;
} CSO_BufferInfo_t;

typedef struct _CSO_SBTL_Ebu_s
{
	uint8_t	ucMagazineNum;	/**< Magazine Number */
	uint8_t	ucPageNum;	/**< Page Number */
} CSO_SBTL_Ebu_t;

typedef struct _CSO_SBTL_Dvb_s
{
	uint16_t usCompositionPageId;	/**< Composition Page ID */
	uint16_t usAncillaryPageId;	/**< Ancillary Page ID */
} CSO_SBTL_Dvb_t;

typedef struct CSO_ProbeInfo_s {
//	DI_CSO_STREAM_TYPE_e		stream_type;
	uint32_t						max_bitrate;		/**< maximum stream bitreate, bps or 0 if unknown  */
	uint64_t						duration;       	/**< duration of stream, in milliseconds or 0 if unknown */
	uint32_t						nprograms;      	/**< total number of programs */
	uint32_t						ntracks;        	/**< total number of tracks */
	uint32_t						video_pes_id;
	uint32_t						video_stream_id;    /**< or Video PID */
	uint32_t				video_codec;
	uint16_t 					usVideoWidth;		/**< Video Width */
	uint16_t 					usVideoHeight;		/**< Video Height */
	
	int32_t 						nCurrentVideoTrack; 	/**< Current Video track num */
	int32_t 						nCurrentAudioTrack; 	/**< Current Audio track num */
	uint8_t						ucTotalAudioTrack;	/**< Total Audio Track */	
	struct {
		uint32_t 					audio_stream_id;	/**< Audio PID */
		uint32_t					audio_stream_srcid;	/**< Audio Source PID */
		uint32_t					audio_stream_identifier; /**< Audio stream identifier : in case of DASH ,this will be adaptation ID */
		uint32_t					audio_codec;
		uint32_t					bitrate;
		uint32_t					profile;
		uint32_t					sample_rate;
		uint32_t					channels;
		uint32_t					codec_id;
		uint32_t					blockalign;
		uint32_t					extradata_size;
		uint8_t						*extradata;
		uint8_t						uacAudioRole[256];	/* Audio Role (Added for DASH) */
		uint8_t						uacAudioLang[256];
		uint8_t						uacAudioName[256];	/* Also track ID for DASH */
	} stAudioTrack[CSO_MAX_AUDIO_TRACK];

	uint8_t							ucTotalVideoTrack;	/**< Total Video Track */
	struct {
		uint32_t 					video_stream_id;	/**< Video PID */
		uint32_t					video_stream_srcid; /**< Video Source PID */
		uint32_t					video_stream_identifier; /**< Video stream identifier : in case of DASH,this will be adaptation ID */
		uint32_t					video_codec;
		uint32_t					width;
		uint32_t					height;
		uint32_t					extradata_size;
		uint8_t						*extradata;
		uint8_t						uacVideoName[256];
		uint8_t						uacVideoRole[256];	/* Video Role (Added for DASH) */
	} stVideoTrack[CSO_MAX_VIDEO_TRACK];

	uint8_t							ucTotalSubTrack; /**< Total Subtitle Track Number */
	struct {
		uint32_t 					stream_id;		/**< or PID */
		uint32_t					stream_srcid;	/**< Audio Source PID */
		uint32_t					stream_identifier; /**< Audio stream identifier : in case of DASH ,this will be adaptation ID */
		CSO_SUBTITLE_TYPE_e				eSubType;		/**< Subtitle Type */
		CSO_SBTL_Dvb_t					stDvb;
		CSO_SBTL_Ebu_t					stEbu;
		uint8_t						uacSubLang[256];	/**< Subtitle Language */
	} stSubTrack[CSO_MAX_SUB_TRACK];

	uint32_t						audio_pes_id;
	uint32_t						audio_stream_id;    /**< or Audio PID */
	uint32_t				audio_codec;

	uint8_t						support_trick;
	uint8_t						support_seek;
	uint32_t						pcr_pid; /**< for TS stream */
	uint32_t						ca_pid; /* bbts ecm(=ksm) pid */

	uint64_t					ullStartTime;			/**< Specify start time of given contents */
	uint64_t					ullInternalSeekedTime;	/**< Position after internally seeked by iptuner policy : e.g. DASH anchor */
	uint8_t						uacAvailabilityStartTime[64];	/**< This is for DASH information */

	uint8_t						is_adaptivestreaming;
//	int32_t						demuxId;

	CSO_DRMType_e				eVideoProtectionType;
	CSO_DRMType_e				eAudioProtectionType;
	
	/* Non IPC delivered area */
	uint8_t 					is_extradata_postponed;

} CSO_ProbeInfo_t;

typedef struct CSO_RESOURCE_MAP_s
{
	uint32_t *hPlaypumpList;
	uint32_t *hDmaList;
	uint32_t uiNumOfPlaypump;
	uint32_t uiNumOfDma;
} CSO_RESOURCE_MAP_t;

#ifdef  __cplusplus
}
#endif

#endif /* __CSO_DEF_H__ */
