/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_media20.h  $
 * Version:         $Revision:   $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date: $
 * File Description:	     DI MEDIA Main Header File (for OCTO 2.0)
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2011 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_MEDIA_H__
#define __DI_MEDIA_H__

#ifdef  __cplusplus
extern "C" {
#endif

// #define PROBE_CANCEL

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include <di_err.h>

#include <di_video.h>
#include <di_audio.h>
#include <di_media_types.h>

/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */


// extern DI_IPTUNER_CONFIG_t g_Configuration;
/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */

#define DI_MEDIA_DRMINFO_MAXNUM (2)
#define DI_MEDIA_CRYPTO_KEY_LEN 16

#define DI_MEDIA_VIDEO_PES_ID                   (0xE0)  /* 0b 1110 xxxx */
#define DI_MEDIA_AUDIO_PES_ID                   (0xC0)  /* 0b 1100 xxxx */
#define DI_MEDIA_VIDEO_PID                      (0x100)
#define DI_MEDIA_AUDIO_PID                      (0x101)

#define DI_MEDIA_MAX_NUM                                (1)
#define DI_MEDIA_MAX_VIDEO_TRACK   8
#define DI_MEDIA_MAX_AUDIO_TRACK   8
#define DI_MEDIA_MAX_XSUB_TRACK    8

/* End Macro definition */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/* MEDIA Configuration */
typedef struct DI_MEDIA_CONFIG_s
{
	HBOOL		bInit;
	HBOOL		bUseStorage;				/* TRUE : enable downloading, FALSE : realtime Streaming Only. */
	HINT32		nStreamBufferingSecond;		/* 0 : pure real time,  N : N second prebuffering */
	HINT32		nChunkBaseExponent;			/* 0 : default 8kbytes, N : EXPONENT value of 2. */
	/* Other configuration */
} DI_MEDIA_CONFIG_t;
//typedef DI_IPTUNER_CONFIG_t DI_MEDIA_CONFIG_t;

typedef void* SESSION_HANDLE;

/************************************************************/
/* Start Enumuration */
/************************************************************/

/**
 * Define Timer Mode
 */
typedef enum
{
	DI_MEDIA_TIMERMODE_NONE                                 = 0x0,
	DI_MEDIA_TIMERMODE_UPDATE_FULLTIME,     /** Update full time */
	DI_MEDIA_TIMERMODE_UPDATE_EXCEPT_0      /** Update time except 0 */
}DI_MEDIA_TIMERMODE_e;

typedef enum
{
	DI_MEDIA_PLAY_START_MODE_BEGIN,
	DI_MEDIA_PLAY_START_MODE_LASTVIEW,
	DI_MEDIA_PLAY_START_MODE_END
}	DI_MEDIA_PLAY_START_MODE_e;

/**
 * Define media stream type
 */
typedef	enum
{
    DI_MEDIA_MPEG_TYPE_UNKNOWN,
    DI_MEDIA_MPEG_TYPE_ES,        /* Elementary stream */
    DI_MEDIA_MPEG_TYPE_BES,       /* Broadcom elementary stream */
    DI_MEDIA_MPEG_TYPE_PES,       /* Packetized elementary stream */
    DI_MEDIA_MPEG_TYPE_TS,        /* Transport stream(188) */
    DI_MEDIA_MPEG_TYPE_TS_192,    /* Transport stream(192) */
    DI_MEDIA_MPEG_TYPE_DSS_ES,    /* DSS with ES payload (used for SD) */
    DI_MEDIA_MPEG_TYPE_DSS_PES,   /* DSS with PES payload (used for HD) */
    DI_MEDIA_MPEG_TYPE_VOB,       /* video object, used with DVD */
    DI_MEDIA_MPEG_TYPE_ASF,       /* Advanced Systems Format */
    DI_MEDIA_MPEG_TYPE_AVI,       /* Audio Video Interleave */
    DI_MEDIA_MPEG_TYPE_MPEG1,     /* MPEG1 System Stream */
    DI_MEDIA_MPEG_TYPE_MP4,       /* MP4 (MPEG-4 Part12) container */
    DI_MEDIA_MPEG_TYPE_MP4FRAGMENT, /* Fragmented MP4 Container */
    DI_MEDIA_MPEG_TYPE_FLV,       /* Flash video container */
    DI_MEDIA_MPEG_TYPE_MKV,        /* Matroska container */
    DI_MEDIA_MPEG_TYPE_WAVE		/* WAVE audio container */
} DI_MEDIA_STREAM_TYPE_e;

/**
 * Media Divx DRM Auth State
 */
typedef enum
{
	DI_MEDIA_DIVX_DRM_NOT_AUTHORIZED,
	DI_MEDIA_DIVX_DRM_AUTHORIZED,
	DI_MEDIA_DIVX_DRM_RENTAL_EXPIRED,
	DI_MEDIA_DIVX_DRM_RENTAL_CHOICE,
	DI_MEDIA_DIVX_DRM_MAX
}DI_MEDIA_DIVX_DRM_AUTH_STATUS_e;

/**
 * Media Divx Subtitle Type
 */
typedef enum
{
	DI_MEDIA_DIVX_XSUB_TYPE_DXSA,		/**< XSUB */
	DI_MEDIA_DIVX_XSUB_TYPE_DXSB,		/**< XSUB + */
	DI_MEDIA_DIVX_XSUB_TYPE_MAX,		/**< MAX*/
}DI_MEDIA_DIVX_XSUB_TYPE_e;

typedef enum
{
	DI_MEDIA_ENCRYPTION_TYPE_NONE,
	DI_MEDIA_ENCRYPTION_TYPE_3DES,
	DI_MEDIA_ENCRYPTION_TYPE_AES,
	DI_MEDIA_ENCRYPTION_TYPE_CURRENT, /* Used for delayed record(or split) */
	DI_MEDIA_ENCRYPTION_TYPE_3DES_SECURED_KEY,
	DI_MEDIA_ENCRYPTION_TYPE_AES_SECURED_KEY
} DI_MEDIA_ENCRYPTION_TYPE_e;

typedef enum
{
	DI_MEDIA_BUFFERING_START,
	DI_MEDIA_BUFFERING_PAUSE,
	DI_MEDIA_BUFFERING_STOP,
	DI_MEDIA_BUFFERING_PROBE_START,
	DI_MEDIA_BUFFERING_PROBE_COMPLETE,
	DI_MEDIA_BUFFERING_COMPLETE,
} DI_MEDIA_BufferingStatus_e;

/* CANVAS BufferingConstraints */
typedef enum
{
	DI_MEDIA_MODE_UNLIMITED,
	DI_MEDIA_MODE_THRESHOLD,
	DI_MEDIA_MODE_FIXED,
} DI_MEDIA_BufferingMode_e;

typedef struct DI_MEDIA_BufferingModeParams_s
{
	HUINT32 nBufferThreshold; 
	HBOOL bFastFillIsLimited;
	HFLOAT64 dFastFillLimitMultiple;
	HFLOAT64 dManagedFillLimitMultiple;
} DI_MEDIA_BufferingModeParams_t;

typedef struct DI_MEDIA_PlaybackStartParams_s
{
	HUINT32 ulStartThreshold;
	HUINT32 ulStartConstant;
} DI_MEDIA_PlaybackStartParams_t;

/**
 * Define media Event Type : belows are generated in MEDIA function only (not iptuner)
 */
typedef enum
{
	DI_MEDIA_EVENT_BEGINOFSTREAM	= 0,
	DI_MEDIA_EVENT_ERROROFSTREAM,
	DI_MEDIA_EVENT_ENDOFSTREAM,
	DI_MEDIA_EVENT_CHANGEPLAYTIME,
	DI_MEDIA_EVENT_START,
	DI_MEDIA_EVENT_PAUSE,
	DI_MEDIA_EVENT_PLAYING,
	DI_MEDIA_EVENT_TRICK,
	DI_MEDIA_EVENT_STOP,
	DI_MEDIA_EVENT_CHANGETITLE,
	DI_MEDIA_EVENT_LIVE_BUFFERING_READY, /* buffering event by HTTP chunked live streaming */
	DI_MEDIA_EVENT_LIVE_BUFFERING_START  /* buffering event by HTTP chunked live streaming */
} DI_MEDIA_Event_e;

typedef enum
{
	DI_MEDIA_ADAPTIVE_NONE = 0,
	DI_MEDIA_ADAPTIVE_SSTR_ALL,
	DI_MEDIA_ADAPTIVE_SSTR_VIDEO_STREAM,
	DI_MEDIA_ADAPTIVE_SSTR_AUDIO_STREAM,
	DI_MEDIA_ADAPTIVE_SSTR_TEXT_STREAM,
	DI_MEDIA_ADAPTIVE_SSTR_VIDEO_TRACK,
	DI_MEDIA_ADAPTIVE_SSTR_AUDIO_TRACK,
	DI_MEDIA_ADAPTIVE_SSTR_TEXT_TRACK,
	DI_MEDIA_ADAPTIVE_HLS_ALL,
	DI_MEDIA_ADAPTIVE_HLS_PROGRAM,
	DI_MEDIA_ADAPTIVE_HLS_STREAM,
	DI_MEDIA_ADAPTIVE_MAX
} DI_MEDIA_ADAPTIVE_CONTROL_e;

typedef enum
{
	DI_MEDIA_ADAPTIVE_STARTUP_LOWEST, /**< ���� Bitrate�� Startup */
	DI_MEDIA_ADAPTIVE_STARTUP_HIGHEST, /**< �ְ� Bitrate�� Startup */
	DI_MEDIA_ADAPTIVE_STARTUP_AUTO, /**< AUTO Mode (����� ���� Bitrate�� Startup) */
} DI_MEDIA_ADAPTIVE_STARTUP_e;

/** 
 * SSTR HEURISTICS DECISION MODE
 */
typedef enum
{
	DI_MEDIA_ADAPTIVE_DECISION_AUTO, /**< Heuristics Module ���� Algorithm�� ���� Bitrate-Switching */
	DI_MEDIA_ADAPTIVE_DECISION_MANUAL, /**< User ���ÿ� ���� Bitrate-Switching (not supported yet) */
	DI_MEDIA_ADAPTIVE_DECISION_FIXED, /**< Bitrate-Switching ���� ���� */
} DI_MEDIA_ADAPTIVE_DECISION_e;


/**
 * Define Multicast Channel Tuner Type
 * DI_MEDIA_MULTICAST_ prefix string will be change with DI_MEDIA_CHANNEL_
 */
typedef enum
{
	DI_MEDIA_CHANNEL_TUNER_TYPE_SI,
	DI_MEDIA_CHANNEL_TUNER_TYPE_AV
} DI_MEDIA_ChannelTunerType_e;

/* The followings are generated in iptuner */

/*
 * Error Category
 */
typedef enum
{
	eMEDIA_ERROR_OTHER=0x0,
	eMEDIA_ERROR_SERVER=0x7, 	/* return codes or strings form each server */
	eMEDIA_ERROR_NETWORK, 	/* TCP/UDP/Socket Errors */
	eMEDIA_ERROR_DATA,		/* related to a specified media format */
	eMEDIA_ERROR_SYSTEM,		/* related to system call */
	eMEDIA_ERROR_IO,			/* related to posix io */
	eMEDIA_ERROR_DRM_MARLIN=0xF,	/* related to Marlin DRM */
	eMEDIA_ERROR_DRM_PLAYREADY,	/* related to MS PlayReady DRM */
} DI_MEDIA_ERROR_CATEGORY_e;

/*
 * event state for controlling ADP ram buffering under unlimted HTTP chunked live streaming(chunked-TE & no-range)
 */
typedef enum
{
	eMEDIA_LIVE_BUFFER_STATE_CANPLAY=0x0,
	eMEDIA_LIVE_BUFFER_STATE_UNDERFLOW=0x31,
	eMEDIA_LIVE_BUFFER_STATE_COMPLETED=0x33,
} DI_MEDIA_LIVE_BUFFER_STATE_e;

/*
 * Error Reason Types 
 */
typedef enum
{
	eMEDIA_ERROR_R_UNKNOWN=0,
	eMEDIA_ERROR_R_NETWORK_LINK_DOWN=100,
	eMEDIA_ERROR_R_NETWORK_DNSLOOKUP_FAIL,
	eMEDIA_ERROR_R_NETWORK_CONNECTION_REFUSED,
	eMEDIA_ERROR_R_NETWORK_IGMP_JOIN_FAIL,
	eMEDIA_ERROR_R_NETWORK_SELECT_TIMEOUT,
	eMEDIA_ERROR_R_NETWORK_SEEK_TIMEOUT,

	/* the following http status codes are defined according to the HTTP 1.1 Specification : Pls don't touch !!*/
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_CREATED							=201 , /* Section 10.2.2: Created */

	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_MULTIPLE_CHOICE					=300 , /* Section 10.3.1: Multiple Choices */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNUSED							=306 , /* Section 10.3.7: Unused field */
	
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_BAD_REQUEST						=400 , /* 400 , Section 10.4.1: Bad Request */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNAUTHORIZED						=401 , /* 401 , Section 10.4.2: Unauthorized */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PAYMENT_REQUIRED					=402 , /* 402 , Section 10.4.3: Payment Required */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_FORBIDDEN							=403 , /* 403 , Section 10.4.4: Forbidden */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_FOUND							=404 , /* 404 , Section 10.4.5: Not Found */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_METHOD_NOT_ALLOWED				=405 , /* 405 , Section 10.4.6: Method Not Allowed */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_ACCEPTABLE					=406 , /* 406 , Section 10.4.7: Not Acceptable */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PROXY_AUTH_REQUIRED				=407 , /* 407 , Section 10.4.8: Proxy Authentication Required */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_TIMEOUT					=408 , /* 408 , Section 10.4.9: Request Time-out */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_CONFILICT							=409 , /* 409 , Section 10.4.10: Conflict */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_GONE								=410 , /* 410 , Section 10.4.11: Gone */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_LENGTH_REQUIRED					=411 , /* 411 , Section 10.4.12: Length Required */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_PRECONDITION_FAILED				=412 , /* 412 , Section 10.4.13: Precondition Failed */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE			=413 , /* 413 , Section 10.4.14: Request Entity Too Large */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUEST_URI_TOO_LARGE				=414 , /* 414 , Section 10.4.15: Request-URI Too Large */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE			=415 , /* 415 , Section 10.4.16: Unsupported Media Type */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE 	=416 , /* 416 , Section 10.4.17: Requested range not satisfiable */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_EXPECTATION_FAILED				=417 , /* 417 , Section 10.4.18: Expectation Failed */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_INTERNAL_SERVER_ERROR				=500 , /* 500 , Section 10.5.1: Internal Server Error */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_NOT_IMPLEMENTED					=501 , /* 501 , Section 10.5.2: Not Implemented */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_BAD_GATEWAY						=502 , /* 502 , Section 10.5.3: Bad Gateway */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_SERVICE_UNAVAILABLE				=503 , /* 503 , Section 10.5.4: Service Unavailable */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_GATEWAY_TIMEOUT					=504 , /* 504 , Section 10.5.5: Gateway Time-out */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_VERSION_NOT_SUPPORTED				=505 , /* 505 , Section 10.5.6: HTTP Version not supported */
	eMEDIA_ERROR_R_SERVER_HTTP_STATUS_MHEG_UNKNOWN						=622 , /* Mheg Specific */

	eMEDIA_ERROR_R_SERVER_RTSP_ERROR=700,
	eMEDIA_ERROR_R_SERVER_MMS_ERROR,
	eMEDIA_ERROR_R_SERVER_IIS_ERROR,
	eMEDIA_ERROR_R_SYSTEM_NO_MEMORY,
	eMEDIA_ERROR_R_IO_READ_FAIL,
	eMEDIA_ERROR_R_IO_WRITE_FAIL,
	eMEDIA_ERROR_R_IO_SEEK_FAIL,
	eMEDIA_ERROR_R_DATA_MALFORMED_MANIFEST,
	eMEDIA_ERROR_R_DATA_MALFORMED_MP4,
	eMEDIA_ERROR_R_DATA_MALFORMED_TS,
	eMEDIA_ERROR_R_DATA_NO_INDEX_FILE,

} DI_MEDIA_ERROR_REASON_e;

/**
 * Define Multicast Channel Event
 * Tuner locking Event used at channel service like IPTV
 * Currently, only used at Multicast Live Channel
 */
typedef enum
{
	/**
	 * Event code which should be notified immediately when tuning function
	 * #DI_IPTUNER_Open() or #DI_MEDIA_SessionOpen() is called.
	 * If OCTO M/W don't use "TUNED" event, just ignore this event.
	 */
	DI_MEDIA_CHANNEL_TUNING_EVENT_TUNED = 0x0,
	/**
	 * Event code which should be notified when tuning has failed or the 
	 * current transport stream is lost. 
	 */
	DI_MEDIA_CHANNEL_TUNING_EVENT_UNLOCKED,
	/**
	 * Event code which should be notified when tuning is successful or
	 * previously lost connection is re-gained.
	 */
	DI_MEDIA_CHANNEL_TUNING_EVENT_LOCKED
}DI_MEDIA_ChannelTuningEvent_e;

/**
 * Define Channel Event or Error used at channel service like IPTV
 * This event callback has optional usage for processing popup dialog.
 * Currently, only used at Multicast Live Channel
 */
typedef enum
{
	/**
	 * Error for socket timeout 
	 */
	DI_MEDIA_CHANNEL_NOTIFY_EVENT_SOCKET_TIMEOUT,
	/**
	 * Error for invalid IP configuration
	 */
	DI_MEDIA_CHANNEL_NOTIFY_EVENT_INVALID_IP,
	/**
	 * Event for unplugging LAN cable
	 */
	DI_MEDIA_CHANNEL_NOTIFY_EVENT_LAN_UNPLUGGED,
	/**
	 * Event for re-plugging after unplugging LAN cable
	 */
	DI_MEDIA_CHANNEL_NOTIFY_EVENT_LAN_PLUGGED
}DI_MEDIA_ChannelNotifyEvent_e;

typedef enum {
	DI_MEDIA_DRM_TYPE_NONE,
	DI_MEDIA_DRM_TYPE_PLAYREADY,
	DI_MEDIA_DRM_TYPE_WMDRM,
	DI_MEDIA_DRM_TYPE_VIEWRIGHTWEB,
	DI_MEDIA_DRM_TYPE_MARLIN,
	DI_MEDIA_DRM_TYPE_MAX
} DI_MEDIA_DRM_Type;

typedef enum
{
	DI_MEDIA_UNDERFLOW_AUTO=0,
	DI_MEDIA_UNDERFLOW_THRESHOLD
}DI_MEDIA_UNDERFLOW_MODE;

/************************************************************/
/* End Enumuration */
/************************************************************/

/************************************************************/
/* Start Structure */
/************************************************************/

typedef struct DI_MEDIA_AdaptiveSettings_s
{
	/* Adaptive Mode */
	DI_MEDIA_ADAPTIVE_STARTUP_e eStartup;
	DI_MEDIA_ADAPTIVE_DECISION_e eDecision;
	DI_MEDIA_ADAPTIVE_CONTROL_e	eControl;
	
	/* for SSTR */
	HUINT32 ulVideoStreamIndex;
	HUINT32 ulAudioStreamIndex;
	HUINT32 ulTextStreamIndex;
	HUINT32 ulVideoTrackIndex;
	HUINT32 ulAudioTrackIndex;
	HUINT32 ulTextTrackIndex;

	/* for HLS */
	HUINT32 ulProgramIndex;
	HUINT32 ulStreamIndex;
	
	/* for HAS */	
	/* for DASH */
} DI_MEDIA_AdaptiveSettings_t;

/**
 * Media Divx DRM Information
 */
typedef struct DI_MEDIA_DivxDrmInfos_t_{
	HBOOL							bIsActivationStream;		/**< Is DRM Activation Stream */
	DI_MEDIA_DIVX_DRM_AUTH_STATUS_e eAuthStatus;		/**< DRM Authorization Status */
	HUINT8							ucRentalUsed;		/**< Rental Used */
	HUINT8							ucRentalLimit;		/**< Rental Remains */
} DI_MEDIA_DivxDrmInfos_t;

typedef struct
{
	DI_MEDIA_ENCRYPTION_TYPE_e eEncType;
	HUINT8 ucKey[DI_MEDIA_CRYPTO_KEY_LEN];
	HUINT8 ucIv[DI_MEDIA_CRYPTO_KEY_LEN];
	HUINT8 ucProtectionKey[DI_MEDIA_CRYPTO_KEY_LEN];
	HUINT32	vidDescId;
	HUINT32	audDescId;
} DI_MEDIA_CRYPTO_SETTINGS;

typedef struct DI_MEDIA_PROBE_INFO_s {
	DI_MEDIA_STREAM_TYPE_e		stream_type;
	HUINT32						max_bitrate;		/**< maximum stream bitreate, bps or 0 if unknown  */
	HUINT32						duration;       	/**< duration of stream, in milliseconds or 0 if unknown */
	HUINT32						nprograms;      	/**< total number of programs */
	HUINT32						ntracks;        	/**< total number of tracks */
	HUINT32						video_pes_id;
	HUINT32						video_stream_id;    /**< or Video PID */
	DI_VIDEO_FORMAT				video_codec;
	HUINT16 					usVideoWidth;		/**< Video Width */
	HUINT16 					usVideoHeight;		/**< Video Height */

	HUINT8						ucTotalAudioTrack;	/**< Total Audio Track */
	struct {
		HUINT32 					audio_pes_id;
		HUINT32 					audio_stream_id;/**< or Audio PID */
		DI_AUDIO_FORMAT				audio_codec;
		HUINT8						uacAudioLang[256];
	} stAudioTrack[DI_MEDIA_MAX_AUDIO_TRACK];

	HUINT8							ucTotalVideoTrack;	/**< Total Video Track */
	struct {
		HUINT32 					video_pes_id;
		HUINT32 					video_stream_id;/**< or Video PID */
		DI_VIDEO_FORMAT				video_codec;
	} stVideoTrack[DI_MEDIA_MAX_VIDEO_TRACK];

	HUINT8							ucTotalXsubTrack; /**< Total Xsub Track Number */
	struct {
		DI_MEDIA_DIVX_XSUB_TYPE_e	eXsubType;			/**< Xsub Type */
		HUINT8						uacXsubLang[256];	/**< Xsub Language */
	} stXsubTrack[DI_MEDIA_MAX_XSUB_TRACK];

	HUINT32						audio_pes_id;
	HUINT32						audio_stream_id;    /**< or Audio PID */
	DI_AUDIO_FORMAT				audio_codec;

	HUINT8						support_trick;
	HUINT8						support_seek;
	HUINT32						pcr_pid; /**< for TS stream */
	HUINT32						ca_pid; /* bbts ecm(=ksm) pid */
	DI_MEDIA_AUDIO_TYPE_e		audiodecodertype;
	DI_MEDIA_VIDEO_TYPE_e		videodecodertype;
	DI_MEDIA_PLAY_START_MODE_e	eStartCondition;
	HUINT32						ulStartTime;		/**< Specify start time  */
	HINT32						demuxId;
	HINT32						is_drm;
	DI_MEDIA_DRM_Type			eDrmType;
	HUINT8						is_expired;
	DI_MEDIA_DivxDrmInfos_t		stDivxDrmInfo;

#if 0
	HINT32						protoStatus;		/* Expansion of iptuner error status.(current http only) */
#endif
	HUINT8						isIcyProtocol;		/* HTTP Type(ICY? TRUE:FALSE) */
	HBOOL						isHttpChunked;		/* whether HTTP Chunked TE */
	HBOOL						isRangeAccepted; 	/* HTTP range support at response */
	/* HBOOL					isRangeResponse; */	/* Found the HTTP range support at response */
	HBOOL						bLiveUnderflowControl;  /* underflow control */

	HUINT32						sampleRate;
	HUINT32						bitsPerSample;
	HINT8						isStereo;

	HBOOL						bProbedFirstPts;
	HUINT32 					unProbeFirstPts;
/*************************
*	PlayReady
*************************/
	HUINT8						*pucWrmHeader;
	HINT32						nWrmHeaderLen;
	HUINT8						*pucIndex;
	HUINT32						uiIndexEntriesCount;
	HBOOL						bSimpleIndex;
	HUINT32						uiAsfPacketSize;
	HUINT64						ullAsfPreroll;
	HUINT64						ullAsfIndexTimeInterval;
	HUINT64						ullAsfDataOffset;
	HBOOL						bSuccesiveSeekMode;

	DI_MEDIA_CRYPTO_SETTINGS 	*pstDSC;

} DI_MEDIA_INFO_t;

typedef void (*DI_MEDIA_VerifyExternalCallback)(HUINT32 ulTunerId, void *pUserdata, void *pVerifyhandle, HINT32 *pnResult);

typedef struct DI_MEDIA_VerifyExternalCallback_s
{
	DI_MEDIA_VerifyExternalCallback		cbVerify;
	void 								*pUserdata;
} DI_MEDIA_VerifyExternalCallback_t;

/**
 * Media Tls Certi & Key Path Setting
 */
typedef struct DI_MEDIA_Tls_Setting_s
{
	HINT8						*pTls_ca_name;
	HINT8						*pTls_ca_path;
	HINT8						*pTls_certi_path;
	HINT8						*pTls_prikey_path;
	HINT8						*pTls_pripw;
	void								*pTls_instance;
	HINT32								npTls_value;
	DI_MEDIA_VerifyExternalCallback_t	cbTlsCertficate;
} DI_MEDIA_Tls_Setting_t;

typedef struct DI_MEDIA_PRDY_Settings_s
{
	HINT8 *pPrivateKeyPath;			/* for https playready licence request.*/
	HINT8 *pCertificatePath;		
}DI_MEDIA_PRDY_Settings_t;

typedef struct DI_MEDIA_Dlna_Setting_s
{
	HINT32		nByteRate;
	HINT64		nFileSize;
	HUINT32		unDuration;
	HUINT32		unSampleRate;
	HUINT32		unBitsPerSample;
	HBOOL		bIsStereo;
} DI_MEDIA_Dlna_Setting_t;

typedef struct DI_MEDIA_DtcpIp_Setting_s
{
	void		*AkeHandle;
} DI_MEDIA_DtcpIp_Setting_t;

typedef struct DI_MEDIA_SessionSettings_s
{
	/* not fixed(in progress) */
	DI_MEDIA_BufferingMode_e eBufferingMode;
	DI_MEDIA_BufferingModeParams_t tBufferingParams;
	DI_MEDIA_PlaybackStartParams_t tPlaybackStartParams;
	DI_MEDIA_UNDERFLOW_MODE eUnderflowMode;
	HUINT32 ulEndTime;
	HBOOL	bAccurateSeek;
	DI_MEDIA_AdaptiveSettings_t tAdaptiveSettings;
	DI_MEDIA_Tls_Setting_t	tTlsSetting;
	DI_MEDIA_PRDY_Settings_t tPRDYSetting;
	HUINT32						uiBlockSize;
	HUINT32						uiRambufferSize;
	HUINT32						uiProbebufferSize;
	DI_MEDIA_Dlna_Setting_t			tDlnaSetting;
	DI_MEDIA_DtcpIp_Setting_t		tDtcpIpSetting;
	HINT8							*pProtocoalInfo;
	HINT32						nDemuxId;
	HBOOL						bSuccesiveSeekForTrick;	
} DI_MEDIA_SessionSettings_t;

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
typedef struct DI_MEDIA_BufferStatus_s 
{

	HINT64 llBufferedBytes;
	HINT64 llTotalBytesRemaining;
	HINT64 llTotalStreamBytes;

	HINT32 nArrivalBytedPerSecond;
	HINT32 nStreamBytesPerSecond;
	HINT32 nBufferedMilliseconds;

/*	MAXDOME	*/
	HUINT32 uiBufferPercentage;
	HUINT32 uiRemainBufferSec;
} DI_MEDIA_BufferStatus_t;

/************************************************************/
/* End Structure */
/************************************************************/

/************************************************************/
/* Define Callback Function */
/************************************************************/

typedef void    (*MediaPlayerEvtCb_t)(HUINT32 id, HUINT32 event, HUINT32 p1, HUINT32 p2);

/**
 * Media Event Callback 
 * 	DI_MEDIA_EVENT_BEGINOFSTREAM = 0,
 *  DI_MEDIA_EVENT_ERROROFSTREAM,
 *	DI_MEDIA_EVENT_ENDOFSTREAM,
 *	DI_MEDIA_EVENT_CHANGEPLAYTIME,
 *	DI_MEDIA_EVENT_START,
 *	DI_MEDIA_EVENT_PAUSE,
 *	DI_MEDIA_EVENT_PLAYING,
 *	DI_MEDIA_EVENT_TRICK,
 *	DI_MEDIA_EVENT_STOP
 */
typedef void (*DI_MEDIA_EventCallback_t)(HUINT32 deviceId, HINT32 ev, HUINT32 param1);
typedef void (*DI_MEDIA_FirstPtsCallback_t)(HUINT32 deviceId, HINT32 param1);

/* Buffering EVENT */
typedef void (*DI_MEDIA_BufferStartCallback) (HUINT32 ulTunerId);
typedef void (*DI_MEDIA_BufferUnderflowCallback) (HUINT32 ulTunerId);
typedef void (*DI_MEDIA_BufferPauseCallback) (HUINT32 ulTunerId);
typedef void (*DI_MEDIA_BufferResumeCallback) (HUINT32 ulTunerId);
typedef void (*DI_MEDIA_BufferCompleteCallback) (HUINT32 ulTunerId);
typedef void (*DI_MEDIA_BufferCanPlayCallback) (HUINT32 ulTunerId);
typedef void (*DI_MEDIA_BufferFailureCallback) (HUINT32 ulTunerId);
typedef void (*DI_MEDIA_BufferAdaptiveStreamChangeCallback) (HUINT32 ulTunerId);

/* probing EVENT */
typedef void (*DI_MEDIA_ProbeCompleteCallback) (HUINT32 ulTunerId);
typedef void (*DI_MEDIA_ProbeFailureCallback) (HUINT32 ulTunerId);

/* Multicast Live Channel EVENT */
typedef void (*DI_MEDIA_ChannelTuningEventCallback) (HUINT32 ulTunerId, DI_MEDIA_ChannelTuningEvent_e eTuningEvent);
typedef void (*DI_MEDIA_ChannelNotifyEventCallback) (HUINT32 ulTunerId, DI_MEDIA_ChannelNotifyEvent_e eNotifyEvent); // <--

/* Error EVENT */
typedef void (*DI_MEDIA_ErrorEventCallback) (HUINT32 ulTunerId, DI_MEDIA_ERROR_CATEGORY_e eCate, DI_MEDIA_ERROR_REASON_e eReson);

/* Custom License Data */
typedef void (*DI_MEDIA_LicenseDataCallback_t)(SESSION_HANDLE media_session_handle, HUINT8 *pParam1, HUINT32 unParam2);

typedef struct DI_MEDIA_BufferCallback_s
{
	DI_MEDIA_BufferStartCallback				cbStart;
	DI_MEDIA_BufferUnderflowCallback			cbUnderflow;
	DI_MEDIA_BufferPauseCallback				cbPause;
	DI_MEDIA_BufferResumeCallback				cbResume;
	DI_MEDIA_BufferCompleteCallback				cbComplete;
	DI_MEDIA_BufferCanPlayCallback				cbCanPlay;
	DI_MEDIA_BufferFailureCallback				cbFailure;
	DI_MEDIA_BufferAdaptiveStreamChangeCallback	cbAdaptiveChange;
} DI_MEDIA_BufferCallback_t;

/* for Asynchronous Probing */
typedef struct DI_MEDIA_ProbeCallback_s
{
	DI_MEDIA_ProbeCompleteCallback				cbProbeComplete;
	DI_MEDIA_ProbeFailureCallback					cbProbeFailure;
} DI_MEDIA_ProbeCallback_t;

/* for Adaptive Event */
typedef struct DI_MEDIA_AdaptiveCallback_s
{
	/* TODO: must remove "Dummy"  */
	DI_MEDIA_EventCallback_t cbDummy;
} DI_MEDIA_AdaptiveCallback_t;

/* for Stream Event */
typedef struct DI_MEDIA_StreamEventCallback_s
{
	/* TODO: must remove "Dummy"  */
	DI_MEDIA_EventCallback_t cbDummy;
} DI_MEDIA_StreamEventCallback_t;

/* for IP based Live Channel Event: But only used at multicast  tuner */
typedef struct DI_MEDIA_ChannelEventCallback_s
{
	/* channel locking event */
	DI_MEDIA_ChannelTuningEventCallback		cbTuningEvent;
	/* channel related error or event */
	DI_MEDIA_ChannelNotifyEventCallback		cbNotifyEvent; // <--
} DI_MEDIA_ChannelEventCallback_t;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*
 * DI_MEDIA usage (for PAL_MEDIA ).
 *
 * 1. Simple Downloading 
 *   - DI_MEDIA_CreateSession			: Create tuner by URL.
 *   - DI_MEDIA_RegisterBufferingEventCallback: Register Buffering Event Callbacks for buf-information.
 *   - DI_MEDIA_StartBuffering	: start buffering task
 *   - DI_MEDIA_GetBufferingStatus		: get buffering progress information
 *   - if(EOF) Thread destroied automatically 
 *   - DI_MEDIA_UnregisterBufferingEventCallback: Unregister Buffering Event Callbacks
 *   - DI_MEDIA_DestroySession			: After complete event occurred, release iptuner.
 *
 * 2. Streamed Content Playing.
 *   - DI_MEDIA_CreateSession
 *   - DI_MEDIA_ProbeSession
 *   - DI_MEDIA_OpenSession -> (internal : DI_IPTUNER_StreamOpen)
 *   - DI_MEDIA_SetupSession
 *   - DI_MEDIA_RegisterMediaEventCallback
 *     + DI_MEDIA_RegisterLiveBufferEventCallback (if HTTP chunked live)
 *   - DI_MEDIA_StartSession
 *   - DI_MEDIA_SetPosition/SetSpeed
 *   - DI_MEDIA_StopSession
 *     + DI_MEDIA_UnregisterLiveBufferEventCallback (if HTTP chunked live)
 *   - DI_MEDIA_UnregisterMediaEventCallback
 *   - DI_MEDIA_ReleaseSession
 *   - DI_MEDIA_CloseSession -> (internal : DI_IPTUNER_StreamClose)
 *   - DI_MEDIA_DestroySession
 *
 * 3. Progressive Download Playing.
 *   - DI_MEDIA_CreateSession
 *   - DI_MEDIA_ProbeSession
 *   - DI_MEDIA_RegisterBufferingEventCallback
 *   - DI_MEDIA_StartBuffering -> (internal : DI_IPTUNER_DnpCreate)
 *   - DI_MEDIA_GetBufferingStatus	: get buffering progress information
 *   - DI_MEDIA_OpenSession -> (internal : DI_IPTUNER_DnpOpen)
 *   - DI_MEDIA_RegisterStreamEventCallback
 *   - DI_MEDIA_SetupSession 
 *   - DI_MEDIA_RegisterMediaEventCallback
 *   - DI_MEDIA_StartSession
 *   - DI_MEDIA_SetPosition/SetSpeed : Playback Seek & SetSpeed
 *   - DI_MEDIA_StopSession 
 *   - DI_MEDIA_UnregisterMediaEventCallback
 *   - DI_MEDIA_ReleaseSession 
 *   - DI_MEDIA_UnregisterStreamEventCallback
 *   - DI_MEDIA_CloseSession -> (internal : DI_IPTUNER_DnpClose)
 *   - DI_MEDIA_StopBuffering -> (internal : DI_IPTUNER_DnpDestroy)
 *   - DI_MEDIA_UnregisterBufferingEventCallback  
 *   - DI_MEDIA_DestroySession
 *
 * 4. Multicast Live Channel Tuning(not playing).
 *   - DI_MEDIA_CreateSession
 *   - DI_MEDIA_RegisterChannelEventCallback
 *   - DI_MEDIA_OpenSession  -> (internal : DI_IPTUNER_MulticastOpen)
 *   - DI_MEDIA_CloseSession -> (internal : DI_IPTUNER_MulticastClose)
 *   - DI_MEDIA_UnregisterChannelEventCallback
 *   - DI_MEDIA_DestroySession
 */

/* System ���۽� �ѹ� ȣ��Ǹ� Session Pool �� �����Ѵ�. */
DI_ERR_CODE DI_MEDIA_Init ( DI_MEDIA_CONFIG_t stConfig );

/* DTCP�����Ͽ� �ʱ�ȭ�� �����Ѵ�. */
DI_ERR_CODE DI_MEDIA_DTCP_Init ( void );

/* DTCP �κ� ������ �����Ѵ�. */
void DI_MEDIA_DTCP_Term ( void );

/* Media Player�� Default Configuration ���� ��ȯ�Ѵ�. */
DI_ERR_CODE DI_MEDIA_GetDefaultConfiguration( DI_MEDIA_CONFIG_t *pstConfig );

/* System ����� �ѹ� ȣ��Ǿ� �����Ǿ� �ִ� ��� Session Pool �� List�� �����Ѵ�. */
DI_ERR_CODE DI_MEDIA_Uninit (void);

/* �ϳ��� Session �� �����Ѵ�. 
 * ������ Session �� ������ Handle �� ������ Session Pool �� ��ϵȴ�.
 * �ԷµǴ� URL �� ���� Session �� type�� �����ȴ�. (default : streaming ���)
 */
DI_ERR_CODE DI_MEDIA_CreateSession (HINT8 *pURL, SESSION_HANDLE *pHSession);

/* ������ Session Handle�� �Է¹޾� �ش� Session �� ������ �� Session Pool ���� �����Ѵ�.*/
DI_ERR_CODE DI_MEDIA_DestroySession (SESSION_HANDLE pHSession);

/* 
 * ������ Session Handle �� �Է¹޾� Session Type �� �°� 
 * Playback �Ʒ��� VFIO �� Install �Ѵ�. 
 * Install �Ǵ� VFIO �� Playback �� �������� ����ϰ� �Ǹ�
 * index ���ϱ��� ���� Open �Ǵ� ��쿡 ���� ����Ǿ�� �Ѵ�.
 */ 
DI_ERR_CODE DI_MEDIA_OpenSession (SESSION_HANDLE pHSession);
/* 
 * ������ Session Handle �� �Է¹޾� �ش� Session�� Close �Ѵ�.
 * �̶� Install �Ǿ��� VFIO Set �� �����Ǿ�� �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_CloseSession (SESSION_HANDLE pHSession);

/* 
 * Session Play �� ���� ������ �����Ѵ�. ����� ������ Session�� �ͼӵȴ�.
 * Probe Event Callback Function �� Install �Ǿ� �ִ� ��� FAIL/COMPLETE event �� emit �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_ProbeSession (SESSION_HANDLE pHSession);

/* For 4oD js extension. */
DI_ERR_CODE DI_MEDIA_ProbeSessionComplete (SESSION_HANDLE pHSession);

/* 
 * Probing ������ ���� ��Ų��.
 */
DI_ERR_CODE DI_MEDIA_ProbeCancel (SESSION_HANDLE pHSession);

/* 
 * Session�� ����� Probe ������ ��ȯ�Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_GetProbeInfo (SESSION_HANDLE pHSession, DI_MEDIA_INFO_t *pstProbeInfo );

/* 
 * Resource�� �Ҵ��ϰ� �����Ѵ�. Session ���� Decoder (Demux) Path �� �����ȴ�.
 * Probe�� �� ���¿��� �Ѵ�.
 */ 
DI_ERR_CODE DI_MEDIA_SetupSession (SESSION_HANDLE pHSession, HUINT32 *pulMediaId); /* Media ID �ʿ��Ѱ�? */

/* 
 * Resource�� �����Ѵ�.
 */ 
DI_ERR_CODE DI_MEDIA_ReleaseSession (SESSION_HANDLE pHSession, HUINT32 unMediaId);

/* 
 * Audio Track�� �����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_SetAudioTrack(SESSION_HANDLE pHSession, HUINT32 unMediaId, HUINT8 ucAudioTrack);

/* 
 * Video Track�� �����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_SetVideoTrack(SESSION_HANDLE pHSession, HUINT32 unMediaId, HUINT8 ucVideoTrack);

/*
 * PMT Update �� �ݿ��� DI_MEDIA_INFO_t ����ü ������ �Ϻθ� s_tStreamInfo ���� ������ �����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_UpdatePmtInfo(SESSION_HANDLE pHSession, HUINT32 unMediaId, DI_MEDIA_INFO_t *pStreamInfo);

/* 
 * UserAgentString�� �����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_SetUserAgentString(SESSION_HANDLE pHSession, HINT8 *pUserAgentString, HINT32 nSize);

/* 
 * ������ Session Handle �� �Է¹޾� Seek(SetPosition) ����� �����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_SetPosition (SESSION_HANDLE pHSession, HUINT32 unMediaId, HUINT64 ullPosition);

/* 
 * ������ Session Handle �� �Է¹޾� ���� Position �� ��ȯ�Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_GetPosition (SESSION_HANDLE pHSession, HUINT32 unMediaId, HUINT64 *pullPosition);

/* 
 * ������ Session Handle �� �Է¹޾� ����� �����Ѵ�.
 * �ش� Session �� Play ����� �����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_SetSpeed (SESSION_HANDLE pHSession, HUINT32 unMediaId, DI_MEDIA_PlaySpeed_e eSpeed );

/* 
 * ������ Session Handle �� �Է¹޾� �ش� Session �� ���� Play ����� ��ȯ�Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_GetSpeed (SESSION_HANDLE pHSession, HUINT32 unMediaId, DI_MEDIA_PlaySpeed_e *peSpeed );


#if 0 /* YouView (CANVAS) */
/* ������ IPTUNER Handle �� �Է¹޾� Buffering Mode (specified on YouView) �� �����Ѵ�.
 * �Էµ� enum type �� ���� buffering speed �� size �� �����ϰ� �Ǹ�
 * ������ ��Ŀ� ���� IPTUNER Session speed �� �����ȴ�. 
 */
DI_ERR_CODE DI_IPTUNER_SetBufferingMode (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferingMode_e eConstraint, HINT32 value);
/* 
 * ������ IPTUNER handle �� �Է¹޾� Open�� iptuner �� PlayingPosition �� Buffering Position ������ ����
 * EndByte �� �����Ѵ�.
 */
DI_ERR_CODE DI_IPTUNER_SetEndByte (DI_IPTUNER_t *pHTuner, HINT64 ulOffset );
#endif 

/* 
 * Session�� ����Ѵ�.
 * Play�� ���ؼ��� Setup�� ����Ǿ�� �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_StartSession(SESSION_HANDLE pHSession, HUINT32 unMediaId, HULONG ulStartTime);

/* 
 * ��� ���� Session�� ���� �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_StopSession (SESSION_HANDLE pHSession, HUINT32 unMediaId);

/* 
 * ��� ���� Session�� �Ͻ� ���� �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_PauseSession (SESSION_HANDLE pHSession, HUINT32 unMediaId);

/* 
 * �Ͻ� ���� ������ Session�� ��� �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_ResumeSession (SESSION_HANDLE pHSession, HUINT32 unMediaId);

/* 
 * �� ���(SSTR/ABR/...)�� �ʿ��� Setting Parameter�� RunTime Ȥ�� SetupTime�� �����Ѵ�.
 * Parameter ������ ����
 */
DI_ERR_CODE DI_MEDIA_SetSessionSettings (SESSION_HANDLE pHSession, const DI_MEDIA_SessionSettings_t *pSettings);

/* 
 * �ش� Session�� Setting Parameter�� ��ȯ�Ѵ�.
 * Parameter ������ ����
 */
DI_ERR_CODE DI_MEDIA_GetSessionSettings (SESSION_HANDLE pHSession, DI_MEDIA_SessionSettings_t *pstSettings);

/* ������ Session �� Handle �� �Է¹޾� Buffering Session �� ��쿡 ����
 * ���� Buffer �� Status �� ������ �ش�. ������ Buffer structure �� ����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_GetBufferingStatus (SESSION_HANDLE pHSession, DI_MEDIA_BufferStatus_t *pstBufferStatus);

/* 
 * ������ Session �� Handle �� �Է¹޾� Buffering �� Start �ϰ� �ȴ�.
 * BufferType ���� RAMBUFFERING, HDDBuffering �ΰ����� ������ HDD Buffering���� �ش�ȴ�.
 * HTTP,RTSP �� ��� protocol controller �� ���� ������ ����� �����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_StartBuffering (SESSION_HANDLE pHSession, HINT8 *pSaveFileName);

/* 
 * ������ Session �� Handle �� �Է¹޾� Buffering Task �� ������Ų��.
 * ������ Task �� ���̻� Stream �� Buffering ���� �ʰ� �Ǹ�
 * �̿� ���� ������ Event �� ���� �˾Ƴ����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_PauseBuffering (SESSION_HANDLE pHSession);
/* 
 * ������ Session �� Handle �� �Է¹޾� Buffering Task �� ������Ѵ�.
 * ����۽� �������� ����� BufferingEvent �� ���� BUFFERING_START �� emit �ȴ�.
 * �̿� ���� ������ Event �� ���� �˾Ƴ����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_ResumeBuffering (SESSION_HANDLE pHSession);

/* 
 * ������ Session �� Handle �� �Է¹޾� Buffering Task �� �����Ѵ�.
 * ������� ���� content ������ ������ Buffering �� Stop ��Ű��
 * Buffering �� ����Ǹ� Playback �� install �� VFIO �� ���ؼ��� EOF �߻��ñ⸦ �����ؾ��Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_StopBuffering (SESSION_HANDLE pHSession);

/* 
 * ������ Session �� Media���� Event Callback�� ����Ѵ�.(BOF, EOF,ERROF)
 */
DI_ERR_CODE DI_MEDIA_RegisterMediaEventCallback (SESSION_HANDLE pHSession, HUINT32 unMediaId, DI_MEDIA_EventCallback_t MediaEventCallback);

/* 
 * ������ Session �� Media���� Event Callback�� �����Ѵ�.(BOF, EOF,ERROF)
 */
DI_ERR_CODE DI_MEDIA_UnregisterMediaEventCallback (SESSION_HANDLE pHSession, HUINT32 unMediaId);

/* 
 * ������ Session �� Live Buffering Event Callback�� ����Ѵ�.(isReady)
 */
DI_ERR_CODE DI_MEDIA_RegisterLiveBufferEventCallback (SESSION_HANDLE pHSession, HUINT32 unMediaId, DI_MEDIA_EventCallback_t MediaEventCallback);

/* 
 * ������ Session �� Live Buffering Event Callback�� �����Ѵ�.(isReady)
 */
DI_ERR_CODE DI_MEDIA_UnregisterLiveBufferEventCallback (SESSION_HANDLE pHSession, HUINT32 unMediaId);

/* 
 * ������ Session �� Handle �� �Է¹޾� Probe Event �� ������ �� �ֵ��� 
 * Callback Function �� install �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_RegisterProbeEventCallback (SESSION_HANDLE pHSession, DI_MEDIA_ProbeCallback_t *pProbeCallbacks);

/* 
 * Install �� Probe Event Callback Function�� ���� �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_UnregisterProbeEventCallback (SESSION_HANDLE pHSession);

/* ������ Session �� Handle �� �Է¹޾� Buffering Event �� ������ �� �ֵ��� 
 * Callback Function �� install �Ѵ�. �� Function �� Buffering Task ����
 * Ư�� Event �� �߻����� ������ ȣ�����ְ� �Ǹ� Event �� type �� 
 * enum ������ define �Ѵ�. 
 */
DI_ERR_CODE DI_MEDIA_RegisterBufferingEventCallback (SESSION_HANDLE pHSession, DI_MEDIA_BufferCallback_t *pBufferCallbacks);

/* 
 * Install �� Buffering Event Callback Function�� ���� �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_UnregisterBufferingEventCallback (SESSION_HANDLE pHSession);

/* 
 * Adaptive Specific Event�� ������ �� �ֵ��� 
 * Callback Function �� install �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_RegisterAdaptiveEventCallback (SESSION_HANDLE pHSession, DI_MEDIA_AdaptiveCallback_t *pAdaptiveCallbacks);

/* 
 * Install �� Adaptive Specific Event Callback Function�� ���� �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_UnregisterAdaptiveEventCallback (SESSION_HANDLE pHSession);

/* 
 * Streaming ���� Event(Underrun/Overrun/Start/Stop/...) �� ������ �� �ֵ��� 
 * Callback Function �� install �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_RegisterStreamEventCallback (SESSION_HANDLE pHSession, DI_MEDIA_StreamEventCallback_t *pStreamEventCallback);

/* 
 * Install �� Streaming Event Callback Function�� ���� �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_UnregisterStreamEventCallback (SESSION_HANDLE pHSession);

/* 
 * ������ Session �� Handle �� �Է¹޾� Channel Event �� ������ �� �ֵ��� 
 * Callback Function �� install �Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_RegisterChannelEventCallback(SESSION_HANDLE pHSession, 
                                                   DI_MEDIA_ChannelTunerType_e eTunerType, 
                                                   DI_MEDIA_ChannelEventCallback_t *pChannelEventCallbacks);
/* 
 * Install �� Channel Event Callback Function�� �����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_UnregisterChannelEventCallback(SESSION_HANDLE pHSession);

/* 
 * Tuner���� �߻��ϴ� Error Event�� �ޱ� ���� Callback�� Install�Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_RegisterErrorEventCB (SESSION_HANDLE pHSession, DI_MEDIA_ErrorEventCallback pErrEventCallback);

/* 
 * Tuner���� �߻��ϴ� Error Event�� �ޱ� ���� Callback�� �����Ѵ�.
 */
DI_ERR_CODE DI_MEDIA_UnregisterErrorEventCB (SESSION_HANDLE pHSession);

/*
* Register the function that receives the custom license data (Ex. Maxdome cookie).
*/
DI_ERR_CODE DI_MEDIA_RegisterLicenseDataCallback(SESSION_HANDLE pHSession, DI_MEDIA_LicenseDataCallback_t callback);

/*
* Unregister the function that receives the custom license data (Ex. Maxdome cookie).
*/
DI_ERR_CODE DI_MEDIA_UnregisterLicenseDataCallback(SESSION_HANDLE pHSession);

/*
* Receives the custome license data (Ex. Maxdome cookie).
*/
DI_ERR_CODE DI_MEDIA_SetLicenseData(SESSION_HANDLE pHSession, HUINT8 *pucLicenseData);

/* 
 * Tuner���� �߻��ϴ� Error Event �� Reason �� malloc �� string���� �����Ѵ�. 
 * Notice!! �� Function �� strdup �� ������ ������ �ϹǷ� ���ϵ� String ��
 * �ݵ�� memory ���� �Ǿ�� �Ѵ�. 
 */
void * DI_MEDIA_GetErrorReason_StrDup (DI_MEDIA_ERROR_REASON_e eReason);

DI_ERR_CODE DI_MEDIA_GetBufferInfo(SESSION_HANDLE pHSession, HUINT32 *uiChunks, HINT32 **iStartOffsets, HINT32 **iEndOffsets);


DI_ERR_CODE DI_MEDIA_SetPlayreadyLicenseURL (SESSION_HANDLE pHSession, HCHAR *licenseURL);
DI_ERR_CODE DI_MEDIA_GetPlayreadyNeedCallback (void);
DI_ERR_CODE DI_MEDIA_GetPlayreadyLicenseURL (SESSION_HANDLE pHSession, HCHAR *licenseURL);

/* For Calypso */
DI_ERR_CODE DI_MEDIA_GetCurrentStatus(SESSION_HANDLE pHSession, HBOOL* bBOF, HBOOL* bEOF);
DI_ERR_CODE DI_MEDIA_SetDecoderIndex (SESSION_HANDLE pHSession, HUINT32 nIdxVideoDecoder,  HUINT32 nIdxAudioDecoder);
DI_ERR_CODE DI_MEDIA_Flush(SESSION_HANDLE pHSession);
DI_ERR_CODE DI_MEDIA_TrickMode(SESSION_HANDLE pHSession, HBOOL bEnable);
DI_ERR_CODE DI_MEDIA_SetCurrentPlaypump(SESSION_HANDLE pHSession, HUINT32 uiIndex);
void DI_MEDIA_SetDuration(SESSION_HANDLE pHSession, HUINT64 ullDuration);
DI_ERR_CODE DI_MEDIA_SetActiveAudioTrack(SESSION_HANDLE pHSession, HUINT32 unAudioCodecId, HUINT32 unAudioStreamId);
DI_ERR_CODE DI_MEDIA_GetActiveAudioTrack(SESSION_HANDLE pHSession, HUINT32 *unAudioCodecId, HUINT32 *unAudioStreamId);
DI_ERR_CODE DI_MEDIA_SetActiveVideoTrack(SESSION_HANDLE pHSession, HUINT32 unVideoCodecId, HUINT32 unVideoStreamId);
DI_ERR_CODE DI_MEDIA_GetActiveVideoTrack(SESSION_HANDLE pHSession, HUINT32 *unVideoCodecId, HUINT32 *unVideoStreamId);
DI_ERR_CODE DI_MEDIA_DTCP_AcquireDtcpIpKey(HINT8 **pDtcpKey, HINT32 *nSize);
void DI_MEDIA_DTCP_ReleaseDtcpIpKey(HINT8 *pDtcpKey);
void DI_MEDIA_SetFirstPosition(SESSION_HANDLE pHSession, HUINT64 ullFirstPosition);

/* End global function prototypes */
#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_MEDIA20_H__ */
