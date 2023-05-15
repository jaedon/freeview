/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_calypso.h  $
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

#ifndef __DI_CSO_H__
#define __DI_CSO_H__

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

#define DI_CSO_DRMINFO_MAXNUM (2)
#define DI_CSO_CRYPTO_KEY_LEN 16

#define DI_CSO_VIDEO_PES_ID                   (0xE0)  /* 0b 1110 xxxx */
#define DI_CSO_AUDIO_PES_ID                   (0xC0)  /* 0b 1100 xxxx */
#define DI_CSO_VIDEO_PID                      (0x100)
#define DI_CSO_AUDIO_PID                      (0x101)

#define DI_CSO_MAX_NUM                                (1)
#define DI_CSO_MAX_VIDEO_TRACK   8
#define DI_CSO_MAX_AUDIO_TRACK   8
#define DI_CSO_MAX_XSUB_TRACK    8

/* End Macro definition */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/* MEDIA Configuration */
typedef struct DI_CSO_CONFIG_s
{
	HBOOL		bInit;
	HBOOL		bUseStorage;				/* TRUE : enable downloading, FALSE : realtime Streaming Only. */
	HINT32		nStreamBufferingSecond;		/* 0 : pure real time,  N : N second prebuffering */
	HINT32		nChunkBaseExponent;			/* 0 : default 8kbytes, N : EXPONENT value of 2. */
	/* Other configuration */
} DI_CSO_CONFIG_t;
//typedef DI_IPTUNER_CONFIG_t DI_CSO_CONFIG_t;

typedef void* CSO_HANDLE;

/************************************************************/
/* Start Enumuration */
/************************************************************/

/**
 * Define Timer Mode
 */
typedef enum
{
	DI_CSO_TIMERMODE_NONE                                 = 0x0,
	DI_CSO_TIMERMODE_UPDATE_FULLTIME,     /** Update full time */
	DI_CSO_TIMERMODE_UPDATE_EXCEPT_0      /** Update time except 0 */
}DI_CSO_TIMERMODE_e;

typedef enum
{
	DI_CSO_PLAY_START_MODE_BEGIN,
	DI_CSO_PLAY_START_MODE_LASTVIEW,
	DI_CSO_PLAY_START_MODE_END
}	DI_CSO_PLAY_START_MODE_e;

/**
 * Define cso stream type
 */
typedef	enum
{
    DI_CSO_MPEG_TYPE_UNKNOWN,
    DI_CSO_MPEG_TYPE_ES,        /* Elementary stream */
    DI_CSO_MPEG_TYPE_BES,       /* Broadcom elementary stream */
    DI_CSO_MPEG_TYPE_PES,       /* Packetized elementary stream */
    DI_CSO_MPEG_TYPE_TS,        /* Transport stream(188) */
    DI_CSO_MPEG_TYPE_TS_192,    /* Transport stream(192) */
    DI_CSO_MPEG_TYPE_DSS_ES,    /* DSS with ES payload (used for SD) */
    DI_CSO_MPEG_TYPE_DSS_PES,   /* DSS with PES payload (used for HD) */
    DI_CSO_MPEG_TYPE_VOB,       /* video object, used with DVD */
    DI_CSO_MPEG_TYPE_ASF,       /* Advanced Systems Format */
    DI_CSO_MPEG_TYPE_AVI,       /* Audio Video Interleave */
    DI_CSO_MPEG_TYPE_MPEG1,     /* MPEG1 System Stream */
    DI_CSO_MPEG_TYPE_MP4,       /* MP4 (MPEG-4 Part12) container */
    DI_CSO_MPEG_TYPE_MP4FRAGMENT, /* Fragmented MP4 Container */
    DI_CSO_MPEG_TYPE_FLV,       /* Flash video container */
    DI_CSO_MPEG_TYPE_MKV,        /* Matroska container */
    DI_CSO_MPEG_TYPE_WAVE		/* WAVE audio container */
} DI_CSO_STREAM_TYPE_e;

/**
 * Define cso Audio Format
 */
typedef enum
{
	DI_CSO_AUDIO_FORMAT_NONE = 0,
	DI_CSO_AUDIO_FORMAT_MPEG,
	DI_CSO_AUDIO_FORMAT_MP3,
	DI_CSO_AUDIO_FORMAT_AAC,
	DI_CSO_AUDIO_FORMAT_AAC_ADTS,
	DI_CSO_AUDIO_FORMAT_AAC_LOAS,
	DI_CSO_AUDIO_FORMAT_AAC_PLUS,
	DI_CSO_AUDIO_FORMAT_AAC_PLUS_LOAS,
	DI_CSO_AUDIO_FORMAT_AAC_PLUS_ADTS,    /* AAC plus SBR. aka MPEG-4 High Efficiency (AAC-HE), with LOAS (Low Overhead Audio Stream) sync and LATM mux */
	DI_CSO_AUDIO_FORMAT_AC3,
	DI_CSO_AUDIO_FORMAT_AC3_PLUS,
	DI_CSO_AUDIO_FORMAT_PCM,
	DI_CSO_AUDIO_FORMAT_DTS,            /* Digital Digital Surround sound. */
	DI_CSO_AUDIO_FORMAT_LPCM_HD_DVD,      /* LPCM, HD-DVD mode */
	DI_CSO_AUDIO_FORMAT_LPCM_BLUERAY,     /* LPCM, Blu-Ray mode */
	DI_CSO_AUDIO_FORMAT_DTS_HD,          /* Digital Digital Surround sound, HD */
	DI_CSO_AUDIO_FORMAT_WMA_STD,         /* WMA Standard */
	DI_CSO_AUDIO_FORMAT_WMA_PRO,         /* WMA Professional */
	DI_CSO_AUDIO_FORMAT_FLAC,         /* FLAC */
	DI_CSO_AUDIO_FORMAT_LPCM_DVD,        /* LPCM, DVD mode */
	DI_CSO_AUDIO_FORMAT_PCMWAVE,                              /* PCM audio with Wave header - Used with streams containing PCM audio */
	DI_CSO_AUDIO_FORMAT_ADPCM,			/* ADPCM */
	DI_CSO_AUDIO_FORMAT_VORBIS,			/* OGG Vorbis */
	DI_CSO_AUDIO_FORMAT_OPUS,			/* OPUS */
	DI_CSO_AUDIO_FORMAT_APE,			/* Monkey's audio */
	DI_CSO_AUDIO_FORMAT_DRA
}DI_CSO_AUDIO_FORMAT_e;

/**
 * Cso Divx DRM Auth State
 */
typedef enum
{
	DI_CSO_DIVX_DRM_NOT_AUTHORIZED,
	DI_CSO_DIVX_DRM_AUTHORIZED,
	DI_CSO_DIVX_DRM_RENTAL_EXPIRED,
	DI_CSO_DIVX_DRM_RENTAL_CHOICE,
	DI_CSO_DIVX_DRM_MAX
}DI_CSO_DIVX_DRM_AUTH_STATUS_e;

/**
 * Cso Divx Subtitle Type
 */
typedef enum
{
	DI_CSO_DIVX_XSUB_TYPE_DXSA,		/**< XSUB */
	DI_CSO_DIVX_XSUB_TYPE_DXSB,		/**< XSUB + */
	DI_CSO_DIVX_XSUB_TYPE_MAX,		/**< MAX*/
}DI_CSO_DIVX_XSUB_TYPE_e;

typedef enum
{
	DI_CSO_ENCRYPTION_TYPE_NONE,
	DI_CSO_ENCRYPTION_TYPE_3DES,
	DI_CSO_ENCRYPTION_TYPE_AES,
	DI_CSO_ENCRYPTION_TYPE_CURRENT, /* Used for delayed record(or split) */
	DI_CSO_ENCRYPTION_TYPE_3DES_SECURED_KEY,
	DI_CSO_ENCRYPTION_TYPE_AES_SECURED_KEY
} DI_CSO_ENCRYPTION_TYPE_e;

typedef enum
{
	DI_CSO_BUFFERING_START,
	DI_CSO_BUFFERING_PAUSE,
	DI_CSO_BUFFERING_STOP,
	DI_CSO_BUFFERING_PROBE_START,
	DI_CSO_BUFFERING_PROBE_COMPLETE,
	DI_CSO_BUFFERING_COMPLETE,
} DI_CSO_BufferingStatus_e;

/* CANVAS BufferingConstraints */
typedef enum
{
	DI_CSO_MODE_UNLIMITED,
	DI_CSO_MODE_THRESHOLD,
	DI_CSO_MODE_FIXED,
} DI_CSO_BufferingMode_e;

typedef struct DI_CSO_BufferingModeParams_s
{
	HUINT32 nBufferThreshold; 
	HBOOL bFastFillIsLimited;
	HFLOAT64 dFastFillLimitMultiple;
	HFLOAT64 dManagedFillLimitMultiple;
} DI_CSO_BufferingModeParams_t;

typedef struct DI_CSO_PlaybackStartParams_s
{
	HUINT32 ulStartThreshold;
	HUINT32 ulStartConstant;
} DI_CSO_PlaybackStartParams_t;

/**
 * Define cso Event Type : belows are generated in MEDIA function only (not iptuner)
 */
typedef enum
{
	DI_CSO_EVENT_BEGINOFSTREAM	= 0,
	DI_CSO_EVENT_ERROROFSTREAM,
	DI_CSO_EVENT_ENDOFSTREAM,
	DI_CSO_EVENT_CHANGEPLAYTIME,
	DI_CSO_EVENT_START,
	DI_CSO_EVENT_PAUSE,
	DI_CSO_EVENT_PLAYING,
	DI_CSO_EVENT_TRICK,
	DI_CSO_EVENT_STOP,
	DI_CSO_EVENT_CHANGETITLE
} DI_CSO_Event_e;

typedef enum
{
	DI_CSO_ADAPTIVE_NONE = 0,
	DI_CSO_ADAPTIVE_SSTR_ALL,
	DI_CSO_ADAPTIVE_SSTR_VIDEO_STREAM,
	DI_CSO_ADAPTIVE_SSTR_AUDIO_STREAM,
	DI_CSO_ADAPTIVE_SSTR_TEXT_STREAM,
	DI_CSO_ADAPTIVE_SSTR_VIDEO_TRACK,
	DI_CSO_ADAPTIVE_SSTR_AUDIO_TRACK,
	DI_CSO_ADAPTIVE_SSTR_TEXT_TRACK,
	DI_CSO_ADAPTIVE_HLS_ALL,
	DI_CSO_ADAPTIVE_HLS_PROGRAM,
	DI_CSO_ADAPTIVE_HLS_STREAM,
	DI_CSO_ADAPTIVE_MAX
} DI_CSO_ADAPTIVE_CONTROL_e;

typedef enum
{
	DI_CSO_ADAPTIVE_STARTUP_LOWEST, /**< ���� Bitrate�� Startup */
	DI_CSO_ADAPTIVE_STARTUP_HIGHEST, /**< �ְ� Bitrate�� Startup */
	DI_CSO_ADAPTIVE_STARTUP_AUTO, /**< AUTO Mode (����� ���� Bitrate�� Startup) */
} DI_CSO_ADAPTIVE_STARTUP_e;

/** 
 * SSTR HEURISTICS DECISION MODE
 */
typedef enum
{
	DI_CSO_ADAPTIVE_DECISION_AUTO, /**< Heuristics Module ���� Algorithm�� ���� Bitrate-Switching */
	DI_CSO_ADAPTIVE_DECISION_MANUAL, /**< User ���ÿ� ���� Bitrate-Switching (not supported yet) */
	DI_CSO_ADAPTIVE_DECISION_FIXED, /**< Bitrate-Switching ���� ���� */
} DI_CSO_ADAPTIVE_DECISION_e;


/**
 * Define Multicast Channel Tuner Type
 * DI_CSO_MULTICAST_ prefix string will be change with DI_CSO_CHANNEL_
 */
typedef enum
{
	DI_CSO_CHANNEL_TUNER_TYPE_SI,
	DI_CSO_CHANNEL_TUNER_TYPE_AV
} DI_CSO_ChannelTunerType_e;

/* The followings are generated in iptuner */
#if 0
/*
 * Error Category
 */
typedef enum
{
	eMEDIA_ERROR_OTHER=0x0,
	eMEDIA_ERROR_SERVER=0x7, 	/* return codes or strings form each server */
	eMEDIA_ERROR_NETWORK, 	/* TCP/UDP/Socket Errors */
	eMEDIA_ERROR_DATA,		/* related to a specified cso format */
	eMEDIA_ERROR_SYSTEM,		/* related to system call */
	eMEDIA_ERROR_IO,			/* related to posix io */
	eMEDIA_ERROR_DRM_MARLIN=0xF,	/* related to Marlin DRM */
	eMEDIA_ERROR_DRM_PLAYREADY,	/* related to MS PlayReady DRM */
} DI_CSO_ERROR_CATEGORY_e;
#endif
/**
 * Define Multicast Channel Event
 * Tuner locking Event used at channel service like IPTV
 * Currently, only used at Multicast Live Channel
 */
typedef enum
{
	/**
	 * Event code which should be notified imcsotely when tuning function
	 * #DI_IPTUNER_Open() or #DI_CSO_SessionOpen() is called.
	 * If OCTO M/W don't use "TUNED" event, just ignore this event.
	 */
	DI_CSO_CHANNEL_TUNING_EVENT_TUNED = 0x0,
	/**
	 * Event code which should be notified when tuning has failed or the 
	 * current transport stream is lost. 
	 */
	DI_CSO_CHANNEL_TUNING_EVENT_UNLOCKED,
	/**
	 * Event code which should be notified when tuning is successful or
	 * previously lost connection is re-gained.
	 */
	DI_CSO_CHANNEL_TUNING_EVENT_LOCKED
}DI_CSO_ChannelTuningEvent_e;

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
	DI_CSO_CHANNEL_NOTIFY_EVENT_SOCKET_TIMEOUT,
	/**
	 * Error for invalid IP configuration
	 */
	DI_CSO_CHANNEL_NOTIFY_EVENT_INVALID_IP,
	/**
	 * Event for unplugging LAN cable
	 */
	DI_CSO_CHANNEL_NOTIFY_EVENT_LAN_UNPLUGGED,
	/**
	 * Event for re-plugging after unplugging LAN cable
	 */
	DI_CSO_CHANNEL_NOTIFY_EVENT_LAN_PLUGGED
}DI_CSO_ChannelNotifyEvent_e;

typedef enum {
	DI_CSO_DRM_TYPE_NONE,
	DI_CSO_DRM_TYPE_PLAYREADY,
	DI_CSO_DRM_TYPE_WMDRM,
	DI_CSO_DRM_TYPE_VIEWRIGHTWEB,
	DI_CSO_DRM_TYPE_MARLIN,
	DI_CSO_DRM_TYPE_MAX
} DI_CSO_DRM_Type;

typedef enum
{
	DI_CSO_UNDERFLOW_AUTO=0,
	DI_CSO_UNDERFLOW_THRESHOLD
}DI_CSO_UNDERFLOW_MODE;

typedef enum
{
	DI_CSO_PUMPTYPE_AV = 0,
	DI_CSO_PUMPTYPE_A,
	DI_CSO_PUMPTYPE_V,
	DI_CSO_PUMPTYPE_ENDOFCASE
} DI_CSO_PumpType_e;

typedef enum
{
	DI_CSO_INPUT_STREAMTYPE_TS_MUX,
	DI_CSO_INPUT_STREAMTYPE_ES_DIRECT,
	DI_CSO_INPUT_STREAMTYPE_TS_LIVE
} DI_CSO_InputStreamType_e;


/************************************************************/
/* End Enumuration */
/************************************************************/

/************************************************************/
/* Start Structure */
/************************************************************/

typedef struct DI_CSO_AdaptiveSettings_s
{
	/* Adaptive Mode */
	DI_CSO_ADAPTIVE_STARTUP_e eStartup;
	DI_CSO_ADAPTIVE_DECISION_e eDecision;
	DI_CSO_ADAPTIVE_CONTROL_e	eControl;
	
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
} DI_CSO_AdaptiveSettings_t;

/**
 * Cso Divx DRM Information
 */
typedef struct DI_CSO_DivxDrmInfos_t_{
	HBOOL							bIsActivationStream;		/**< Is DRM Activation Stream */
	DI_CSO_DIVX_DRM_AUTH_STATUS_e eAuthStatus;		/**< DRM Authorization Status */
	HUINT8							ucRentalUsed;		/**< Rental Used */
	HUINT8							ucRentalLimit;		/**< Rental Remains */
} DI_CSO_DivxDrmInfos_t;

typedef struct
{
	DI_CSO_ENCRYPTION_TYPE_e eEncType;
	HUINT8 ucKey[DI_CSO_CRYPTO_KEY_LEN];
	HUINT8 ucIv[DI_CSO_CRYPTO_KEY_LEN];
	HUINT8 ucProtectionKey[DI_CSO_CRYPTO_KEY_LEN];
	HUINT32	vidDescId;
	HUINT32	audDescId;
} DI_CSO_CRYPTO_SETTINGS;

typedef struct DI_CSO_PROBE_INFO_s {
	DI_CSO_STREAM_TYPE_e		stream_type;
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
	} stAudioTrack[DI_CSO_MAX_AUDIO_TRACK];

	HUINT8							ucTotalVideoTrack;	/**< Total Video Track */
	struct {
		HUINT32 					video_pes_id;
		HUINT32 					video_stream_id;/**< or Video PID */
		DI_VIDEO_FORMAT				video_codec;
	} stVideoTrack[DI_CSO_MAX_VIDEO_TRACK];

	HUINT8							ucTotalXsubTrack; /**< Total Xsub Track Number */
	struct {
		DI_CSO_DIVX_XSUB_TYPE_e	eXsubType;			/**< Xsub Type */
		HUINT8						uacXsubLang[256];	/**< Xsub Language */
	} stXsubTrack[DI_CSO_MAX_XSUB_TRACK];

	HUINT32						audio_pes_id;
	HUINT32						audio_stream_id;    /**< or Audio PID */
	DI_AUDIO_FORMAT				audio_codec;

	HUINT8						support_trick;
	HUINT8						support_seek;
	HUINT32						pcr_pid; /**< for TS stream */
	HUINT32						ca_pid; /* bbts ecm(=ksm) pid */
	DI_MEDIA_AUDIO_TYPE_e		audiodecodertype;
	DI_MEDIA_VIDEO_TYPE_e		videodecodertype;
	DI_CSO_PLAY_START_MODE_e	eStartCondition;
	HUINT32						ulStartTime;		/**< Specify start time  */
	HINT32						demuxId;
	HINT32						is_drm;
	DI_CSO_DRM_Type			eDrmType;
	HUINT8						is_expired;
	DI_CSO_DivxDrmInfos_t		stDivxDrmInfo;

#if 0
	HINT32						protoStatus;		/* Expansion of iptuner error status.(current http only) */
#endif
	HUINT8						isIcyProtocol;		/* HTTP Type(ICY? TRUE:FALSE) */

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

	DI_CSO_CRYPTO_SETTINGS 	*pstDSC;

} DI_CSO_INFO_t;

typedef void (*DI_CSO_VerifyExternalCallback)(HUINT32 ulTunerId, void *pUserdata, void *pVerifyhandle, HINT32 *pnResult);

typedef struct DI_CSO_VerifyExternalCallback_s
{
	DI_CSO_VerifyExternalCallback		cbVerify;
	void 								*pUserdata;
} DI_CSO_VerifyExternalCallback_t;

/**
 * Cso Tls Certi & Key Path Setting
 */
typedef struct DI_CSO_Tls_Setting_s
{
	HINT8						*pTls_ca_name;
	HINT8						*pTls_ca_path;
	HINT8						*pTls_certi_path;
	HINT8						*pTls_prikey_path;
	HINT8						*pTls_pripw;
	void								*pTls_instance;
	HINT32								npTls_value;
	DI_CSO_VerifyExternalCallback_t	cbTlsCertficate;
} DI_CSO_Tls_Setting_t;

typedef struct DI_CSO_PRDY_Settings_s
{
	HINT8 *pPrivateKeyPath;			/* for https playready licence request.*/
	HINT8 *pCertificatePath;		
}DI_CSO_PRDY_Settings_t;

typedef struct DI_CSO_Dlna_Setting_s
{
	HINT32		nByteRate;
	HINT64		nFileSize;
	HUINT32		unDuration;
	HUINT32		unSampleRate;
	HUINT32		unBitsPerSample;
	HBOOL		bIsStereo;
} DI_CSO_Dlna_Setting_t;

typedef struct DI_CSO_DtcpIp_Setting_s
{
	void		*AkeHandle;
} DI_CSO_DtcpIp_Setting_t;

typedef struct DI_CSO_SessionSettings_s
{
	/* not fixed(in progress) */
	DI_CSO_BufferingMode_e eBufferingMode;
	DI_CSO_BufferingModeParams_t tBufferingParams;
	DI_CSO_PlaybackStartParams_t tPlaybackStartParams;
	DI_CSO_UNDERFLOW_MODE eUnderflowMode;
	HUINT32 ulEndTime;
	HBOOL	bAccurateSeek;
	DI_CSO_AdaptiveSettings_t tAdaptiveSettings;
	DI_CSO_Tls_Setting_t	tTlsSetting;
	DI_CSO_PRDY_Settings_t tPRDYSetting;
	HUINT32						uiBlockSize;
	HUINT32						uiRambufferSize;
	HUINT32						uiProbebufferSize;
	DI_CSO_Dlna_Setting_t			tDlnaSetting;
	DI_CSO_DtcpIp_Setting_t		tDtcpIpSetting;
	HINT8							*pProtocoalInfo;
	HINT32						nDemuxId;
	HBOOL						bSuccesiveSeekForTrick;	
} DI_CSO_SessionSettings_t;

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
 *   in the CsoRouter's buffer over the last T seconds.
 * - Otherwise, arrivalBytesPerSecond shall be the mean rate of bytes arriving 
 *   in the CsoRouter's buffer over the last 2 seconds.
 *
 * (nStreamBytesPerSecond)
 * For progressive source streams
 * , streamBytesPerSecond shall be -1. 
 * For adaptive source streams
 * , it shall be the average playback bitrate of the stream that is currently being buffered.
 *
 * (nBufferedMilliseconds)
 * For both progressive and adaptive source streams, 
 * bufferedMilliseconds shall be the number of milliseconds of cso time 
 * from the play-position to the end of the current buffer region (as defined in section 7.3.1.2).
 */
typedef struct DI_CSO_BufferStatus_s 
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
} DI_CSO_BufferStatus_t;

/************************************************************/
/* End Structure */
/************************************************************/

/************************************************************/
/* Define Callback Function */
/************************************************************/

typedef void    (*CsoPlayerEvtCb_t)(HUINT32 id, HUINT32 event, HUINT32 p1, HUINT32 p2);

/**
 * Cso Event Callback 
 * 	DI_CSO_EVENT_BEGINOFSTREAM = 0,
 *  DI_CSO_EVENT_ERROROFSTREAM,
 *	DI_CSO_EVENT_ENDOFSTREAM,
 *	DI_CSO_EVENT_CHANGEPLAYTIME,
 *	DI_CSO_EVENT_START,
 *	DI_CSO_EVENT_PAUSE,
 *	DI_CSO_EVENT_PLAYING,
 *	DI_CSO_EVENT_TRICK,
 *	DI_CSO_EVENT_STOP
 */
typedef void (*DI_CSO_EventCallback_t)(HUINT32 deviceId, HINT32 ev, HUINT32 param1);
typedef void (*DI_CSO_FirstPtsCallback_t)(HUINT32 deviceId, HINT32 param1);

/* Buffering EVENT */
typedef void (*DI_CSO_BufferStartCallback) (HUINT32 ulTunerId);
typedef void (*DI_CSO_BufferUnderflowCallback) (HUINT32 ulTunerId);
typedef void (*DI_CSO_BufferPauseCallback) (HUINT32 ulTunerId);
typedef void (*DI_CSO_BufferResumeCallback) (HUINT32 ulTunerId);
typedef void (*DI_CSO_BufferCompleteCallback) (HUINT32 ulTunerId);
typedef void (*DI_CSO_BufferCanPlayCallback) (HUINT32 ulTunerId);
typedef void (*DI_CSO_BufferFailureCallback) (HUINT32 ulTunerId);
typedef void (*DI_CSO_BufferAdaptiveStreamChangeCallback) (HUINT32 ulTunerId);

/* probing EVENT */
typedef void (*DI_CSO_ProbeCompleteCallback) (HUINT32 ulTunerId);
typedef void (*DI_CSO_ProbeFailureCallback) (HUINT32 ulTunerId);

/* Multicast Live Channel EVENT */
typedef void (*DI_CSO_ChannelTuningEventCallback) (HUINT32 ulTunerId, DI_CSO_ChannelTuningEvent_e eTuningEvent);
typedef void (*DI_CSO_ChannelNotifyEventCallback) (HUINT32 ulTunerId, DI_CSO_ChannelNotifyEvent_e eNotifyEvent); // <--
#if 0
/* Error EVENT */
typedef void (*DI_CSO_ErrorEventCallback) (HUINT32 ulTunerId, DI_CSO_ERROR_CATEGORY_e eCate, DI_CSO_ERROR_REASON_e eReson);
#endif
/* Custom License Data */
typedef void (*DI_CSO_LicenseDataCallback_t)(CSO_HANDLE cso_CSO_HANDLE, HUINT8 *pParam1, HUINT32 unParam2);

typedef struct DI_CSO_BufferCallback_s
{
	DI_CSO_BufferStartCallback				cbStart;
	DI_CSO_BufferUnderflowCallback			cbUnderflow;
	DI_CSO_BufferPauseCallback				cbPause;
	DI_CSO_BufferResumeCallback				cbResume;
	DI_CSO_BufferCompleteCallback				cbComplete;
	DI_CSO_BufferCanPlayCallback				cbCanPlay;
	DI_CSO_BufferFailureCallback				cbFailure;
	DI_CSO_BufferAdaptiveStreamChangeCallback	cbAdaptiveChange;
} DI_CSO_BufferCallback_t;

/* for Asynchronous Probing */
typedef struct DI_CSO_ProbeCallback_s
{
	DI_CSO_ProbeCompleteCallback				cbProbeComplete;
	DI_CSO_ProbeFailureCallback					cbProbeFailure;
} DI_CSO_ProbeCallback_t;

/* for Adaptive Event */
typedef struct DI_CSO_AdaptiveCallback_s
{
	/* TODO: must remove "Dummy"  */
	DI_CSO_EventCallback_t cbDummy;
} DI_CSO_AdaptiveCallback_t;

/* for Stream Event */
typedef struct DI_CSO_StreamEventCallback_s
{
	/* TODO: must remove "Dummy"  */
	DI_CSO_EventCallback_t cbDummy;
} DI_CSO_StreamEventCallback_t;

/* for IP based Live Channel Event: But only used at multicast  tuner */
typedef struct DI_CSO_ChannelEventCallback_s
{
	/* channel locking event */
	DI_CSO_ChannelTuningEventCallback		cbTuningEvent;
	/* channel related error or event */
	DI_CSO_ChannelNotifyEventCallback		cbNotifyEvent; // <--
} DI_CSO_ChannelEventCallback_t;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*
 * DI_CSO usage (for PAL_MEDIA ).
 *
 * 1. Simple Downloading 
 *   - DI_CSO_CreateSession			: Create tuner by URL.
 *   - DI_CSO_RegisterBufferingEventCallback: Register Buffering Event Callbacks for buf-information.
 *   - DI_CSO_StartBuffering	: start buffering task
 *   - DI_CSO_GetBufferingStatus		: get buffering progress information
 *   - if(EOF) Thread destroied automatically 
 *   - DI_CSO_UnregisterBufferingEventCallback: Unregister Buffering Event Callbacks
 *   - DI_CSO_DestroySession			: After complete event occurred, release iptuner.
 *
 * 2. Streamed Content Playing.
 *   - DI_CSO_CreateSession
 *   - DI_CSO_ProbeSession
 *   - DI_CSO_OpenSession -> (internal : DI_IPTUNER_StreamOpen)
 *   - DI_CSO_SetupSession
 *   - DI_CSO_RegisterCsoEventCallback
 *   - DI_CSO_StartSession
 *   - DI_CSO_SetPosition/SetSpeed
 *   - DI_CSO_StopSession
 *   - DI_CSO_UnregisterCsoEventCallback
 *   - DI_CSO_ReleaseSession
 *   - DI_CSO_CloseSession -> (internal : DI_IPTUNER_StreamClose)
 *   - DI_CSO_DestroySession
 *
 * 3. Progressive Download Playing.
 *   - DI_CSO_CreateSession
 *   - DI_CSO_ProbeSession
 *   - DI_CSO_RegisterBufferingEventCallback
 *   - DI_CSO_StartBuffering -> (internal : DI_IPTUNER_DnpCreate)
 *   - DI_CSO_GetBufferingStatus	: get buffering progress information
 *   - DI_CSO_OpenSession -> (internal : DI_IPTUNER_DnpOpen)
 *   - DI_CSO_RegisterStreamEventCallback
 *   - DI_CSO_SetupSession 
 *   - DI_CSO_RegisterCsoEventCallback
 *   - DI_CSO_StartSession
 *   - DI_CSO_SetPosition/SetSpeed : Playback Seek & SetSpeed
 *   - DI_CSO_StopSession 
 *   - DI_CSO_UnregisterCsoEventCallback
 *   - DI_CSO_ReleaseSession 
 *   - DI_CSO_UnregisterStreamEventCallback
 *   - DI_CSO_CloseSession -> (internal : DI_IPTUNER_DnpClose)
 *   - DI_CSO_StopBuffering -> (internal : DI_IPTUNER_DnpDestroy)
 *   - DI_CSO_UnregisterBufferingEventCallback  
 *   - DI_CSO_DestroySession
 *
 * 4. Multicast Live Channel Tuning(not playing).
 *   - DI_CSO_CreateSession
 *   - DI_CSO_RegisterChannelEventCallback
 *   - DI_CSO_OpenSession  -> (internal : DI_IPTUNER_MulticastOpen)
 *   - DI_CSO_CloseSession -> (internal : DI_IPTUNER_MulticastClose)
 *   - DI_CSO_UnregisterChannelEventCallback
 *   - DI_CSO_DestroySession
 */

/* System ���۽� �ѹ� ȣ��Ǹ� Session Pool �� �����Ѵ�. */
DI_ERR_CODE DI_CSO_Init ( DI_CSO_CONFIG_t stConfig );

/* DTCP�����Ͽ� �ʱ�ȭ�� �����Ѵ�. */
DI_ERR_CODE DI_CSO_DTCP_Init ( void );

/* DTCP �κ� ������ �����Ѵ�. */
void DI_CSO_DTCP_Term ( void );

/* Cso Player�� Default Configuration ���� ��ȯ�Ѵ�. */
DI_ERR_CODE DI_CSO_GetDefaultConfiguration( DI_CSO_CONFIG_t *pstConfig );

/* System ����� �ѹ� ȣ��Ǿ� �����Ǿ� �ִ� ��� Session Pool �� List�� �����Ѵ�. */
DI_ERR_CODE DI_CSO_Uninit (void);

/* �ϳ��� Session �� �����Ѵ�. 
 * ������ Session �� ������ Handle �� ������ Session Pool �� ��ϵȴ�.
 * �ԷµǴ� URL �� ���� Session �� type�� �����ȴ�. (default : streaming ���)
 */
DI_ERR_CODE DI_CSO_CreateSession (HINT8 *pURL, CSO_HANDLE *pHSession);

/* ������ Session Handle�� �Է¹޾� �ش� Session �� ������ �� Session Pool ���� �����Ѵ�.*/
DI_ERR_CODE DI_CSO_DestroySession (CSO_HANDLE pHSession);

/* 
 * ������ Session Handle �� �Է¹޾� Session Type �� �°� 
 * Playback �Ʒ��� VFIO �� Install �Ѵ�. 
 * Install �Ǵ� VFIO �� Playback �� �������� ����ϰ� �Ǹ�
 * index ���ϱ��� ���� Open �Ǵ� ��쿡 ���� ����Ǿ�� �Ѵ�.
 */ 
DI_ERR_CODE DI_CSO_OpenSession (CSO_HANDLE pHSession);
/* 
 * ������ Session Handle �� �Է¹޾� �ش� Session�� Close �Ѵ�.
 * �̶� Install �Ǿ��� VFIO Set �� �����Ǿ�� �Ѵ�.
 */
DI_ERR_CODE DI_CSO_CloseSession (CSO_HANDLE pHSession);

/* 
 * Session Play �� ���� ������ �����Ѵ�. ����� ������ Session�� �ͼӵȴ�.
 * Probe Event Callback Function �� Install �Ǿ� �ִ� ��� FAIL/COMPLETE event �� emit �Ѵ�.
 */
DI_ERR_CODE DI_CSO_ProbeSession (CSO_HANDLE pHSession);

/* For 4oD js extension. */
DI_ERR_CODE DI_CSO_ProbeSessionComplete (CSO_HANDLE pHSession);

/* 
 * Probing ������ ���� ��Ų��.
 */
DI_ERR_CODE DI_CSO_ProbeCancel (CSO_HANDLE pHSession);

/* 
 * Session�� ����� Probe ������ ��ȯ�Ѵ�.
 */
DI_ERR_CODE DI_CSO_GetProbeInfo (CSO_HANDLE pHSession, DI_CSO_INFO_t *pstProbeInfo );

/* 
 * Resource�� �Ҵ��ϰ� �����Ѵ�. Session ���� Decoder (Demux) Path �� �����ȴ�.
 * Probe�� �� ���¿��� �Ѵ�.
 */ 
DI_ERR_CODE DI_CSO_SetupSession (CSO_HANDLE pHSession, HUINT32 *pulCsoId); /* Cso ID �ʿ��Ѱ�? */

/* 
 * Resource�� �����Ѵ�.
 */ 
DI_ERR_CODE DI_CSO_ReleaseSession (CSO_HANDLE pHSession, HUINT32 unCsoId);

/* 
 * Audio Track�� �����Ѵ�.
 */
DI_ERR_CODE DI_CSO_SetAudioTrack(CSO_HANDLE pHSession, HUINT32 unCsoId, HUINT8 ucAudioTrack);

/*
 * PMT Update �� �ݿ��� DI_CSO_INFO_t ����ü ������ �Ϻθ� s_tStreamInfo ���� ������ �����Ѵ�.
 */
DI_ERR_CODE DI_CSO_UpdatePmtInfo(CSO_HANDLE pHSession, HUINT32 unCsoId, DI_CSO_INFO_t *pStreamInfo);

/* 
 * UserAgentString�� �����Ѵ�.
 */
DI_ERR_CODE DI_CSO_SetUserAgentString(CSO_HANDLE pHSession, HINT8 *pUserAgentString, HINT32 nSize);

/* 
 * ������ Session Handle �� �Է¹޾� Seek(SetPosition) ����� �����Ѵ�.
 */
DI_ERR_CODE DI_CSO_SetPosition (CSO_HANDLE pHSession, HUINT32 unCsoId, HUINT64 ullPosition);

/* 
 * ������ Session Handle �� �Է¹޾� ���� display pic, decoded pic �� ��ȯ�Ѵ�.
 */
DI_ERR_CODE DI_CSO_GetDecodedPicture (CSO_HANDLE pHSession, HUINT32 unCsoId, HUINT32 *pulDecodedPic, HUINT32 *pulDisplayedPic );

/* 
 * ������ Session Handle �� �Է¹޾� ���� Position �� ��ȯ�Ѵ�.
 */
DI_ERR_CODE DI_CSO_GetPosition (CSO_HANDLE pHSession, HUINT32 unCsoId, HUINT64 *pullPosition);

/* 
 * ������ Session Handle �� �Է¹޾� ����� �����Ѵ�.
 * �ش� Session �� Play ����� �����Ѵ�.
 */
DI_ERR_CODE DI_CSO_SetSpeed (CSO_HANDLE pHSession, HUINT32 unCsoId, DI_MEDIA_PlaySpeed_e eSpeed );

/* 
 * ������ Session Handle �� �Է¹޾� �ش� Session �� ���� Play ����� ��ȯ�Ѵ�.
 */
DI_ERR_CODE DI_CSO_GetSpeed (CSO_HANDLE pHSession, HUINT32 unCsoId, DI_MEDIA_PlaySpeed_e *peSpeed );


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
DI_ERR_CODE DI_CSO_StartSession(CSO_HANDLE pHSession, HUINT32 unCsoId, HULONG ulStartTime);

/* 
 * ��� ���� Session�� ���� �Ѵ�.
 */
DI_ERR_CODE DI_CSO_StopSession (CSO_HANDLE pHSession, HUINT32 unCsoId);

/* 
 * ��� ���� Session�� �Ͻ� ���� �Ѵ�.
 */
DI_ERR_CODE DI_CSO_PauseSession (CSO_HANDLE pHSession, HUINT32 unCsoId);

/* 
 * �Ͻ� ���� ������ Session�� ��� �Ѵ�.
 */
DI_ERR_CODE DI_CSO_ResumeSession (CSO_HANDLE pHSession, HUINT32 unCsoId);

/* 
 * �� ���(SSTR/ABR/...)�� �ʿ��� Setting Parameter�� RunTime Ȥ�� SetupTime�� �����Ѵ�.
 * Parameter ������ ����
 */
DI_ERR_CODE DI_CSO_SetSessionSettings (CSO_HANDLE pHSession, const DI_CSO_SessionSettings_t *pSettings);

/* 
 * �ش� Session�� Setting Parameter�� ��ȯ�Ѵ�.
 * Parameter ������ ����
 */
DI_ERR_CODE DI_CSO_GetSessionSettings (CSO_HANDLE pHSession, DI_CSO_SessionSettings_t *pstSettings);

/* ������ Session �� Handle �� �Է¹޾� Buffering Session �� ��쿡 ����
 * ���� Buffer �� Status �� ������ �ش�. ������ Buffer structure �� ����Ѵ�.
 */
DI_ERR_CODE DI_CSO_GetBufferingStatus (CSO_HANDLE pHSession, DI_CSO_BufferStatus_t *pstBufferStatus);

/* 
 * ������ Session �� Handle �� �Է¹޾� Buffering �� Start �ϰ� �ȴ�.
 * BufferType ���� RAMBUFFERING, HDDBuffering �ΰ����� ������ HDD Buffering���� �ش�ȴ�.
 * HTTP,RTSP �� ��� protocol controller �� ���� ������ ����� �����Ѵ�.
 */
DI_ERR_CODE DI_CSO_StartBuffering (CSO_HANDLE pHSession, HINT8 *pSaveFileName);

/* 
 * ������ Session �� Handle �� �Է¹޾� Buffering Task �� ������Ų��.
 * ������ Task �� ���̻� Stream �� Buffering ���� �ʰ� �Ǹ�
 * �̿� ���� ������ Event �� ���� �˾Ƴ����Ѵ�.
 */
DI_ERR_CODE DI_CSO_PauseBuffering (CSO_HANDLE pHSession);
/* 
 * ������ Session �� Handle �� �Է¹޾� Buffering Task �� ������Ѵ�.
 * ����۽� �������� ����� BufferingEvent �� ���� BUFFERING_START �� emit �ȴ�.
 * �̿� ���� ������ Event �� ���� �˾Ƴ����Ѵ�.
 */
DI_ERR_CODE DI_CSO_ResumeBuffering (CSO_HANDLE pHSession);

/* 
 * ������ Session �� Handle �� �Է¹޾� Buffering Task �� �����Ѵ�.
 * ������� ���� content ������ ������ Buffering �� Stop ��Ű��
 * Buffering �� ����Ǹ� Playback �� install �� VFIO �� ���ؼ��� EOF �߻��ñ⸦ �����ؾ��Ѵ�.
 */
DI_ERR_CODE DI_CSO_StopBuffering (CSO_HANDLE pHSession);

/* 
 * ������ Session �� Cso���� Event Callback�� ����Ѵ�.(BOF, EOF,ERROF)
 */
DI_ERR_CODE DI_CSO_RegisterCsoEventCallback (CSO_HANDLE pHSession, HUINT32 unCsoId, DI_CSO_EventCallback_t CsoEventCallback);

/* 
 * ������ Session �� Cso���� Event Callback�� �����Ѵ�.(BOF, EOF,ERROF)
 */
DI_ERR_CODE DI_CSO_UnregisterCsoEventCallback (CSO_HANDLE pHSession, HUINT32 unCsoId);

/* 
 * ������ Session �� Handle �� �Է¹޾� Probe Event �� ������ �� �ֵ��� 
 * Callback Function �� install �Ѵ�.
 */
DI_ERR_CODE DI_CSO_RegisterProbeEventCallback (CSO_HANDLE pHSession, DI_CSO_ProbeCallback_t *pProbeCallbacks);

/* 
 * Install �� Probe Event Callback Function�� ���� �Ѵ�.
 */
DI_ERR_CODE DI_CSO_UnregisterProbeEventCallback (CSO_HANDLE pHSession);

/* ������ Session �� Handle �� �Է¹޾� Buffering Event �� ������ �� �ֵ��� 
 * Callback Function �� install �Ѵ�. �� Function �� Buffering Task ����
 * Ư�� Event �� �߻����� ������ ȣ�����ְ� �Ǹ� Event �� type �� 
 * enum ������ define �Ѵ�. 
 */
DI_ERR_CODE DI_CSO_RegisterBufferingEventCallback (CSO_HANDLE pHSession, DI_CSO_BufferCallback_t *pBufferCallbacks);

/* 
 * Install �� Buffering Event Callback Function�� ���� �Ѵ�.
 */
DI_ERR_CODE DI_CSO_UnregisterBufferingEventCallback (CSO_HANDLE pHSession);

/* 
 * Adaptive Specific Event�� ������ �� �ֵ��� 
 * Callback Function �� install �Ѵ�.
 */
DI_ERR_CODE DI_CSO_RegisterAdaptiveEventCallback (CSO_HANDLE pHSession, DI_CSO_AdaptiveCallback_t *pAdaptiveCallbacks);

/* 
 * Install �� Adaptive Specific Event Callback Function�� ���� �Ѵ�.
 */
DI_ERR_CODE DI_CSO_UnregisterAdaptiveEventCallback (CSO_HANDLE pHSession);

/* 
 * Streaming ���� Event(Underrun/Overrun/Start/Stop/...) �� ������ �� �ֵ��� 
 * Callback Function �� install �Ѵ�.
 */
DI_ERR_CODE DI_CSO_RegisterStreamEventCallback (CSO_HANDLE pHSession, DI_CSO_StreamEventCallback_t *pStreamEventCallback);

/* 
 * Install �� Streaming Event Callback Function�� ���� �Ѵ�.
 */
DI_ERR_CODE DI_CSO_UnregisterStreamEventCallback (CSO_HANDLE pHSession);

/* 
 * ������ Session �� Handle �� �Է¹޾� Channel Event �� ������ �� �ֵ��� 
 * Callback Function �� install �Ѵ�.
 */
DI_ERR_CODE DI_CSO_RegisterChannelEventCallback(CSO_HANDLE pHSession, 
                                                   DI_CSO_ChannelTunerType_e eTunerType, 
                                                   DI_CSO_ChannelEventCallback_t *pChannelEventCallbacks);
/* 
 * Install �� Channel Event Callback Function�� �����Ѵ�.
 */
DI_ERR_CODE DI_CSO_UnregisterChannelEventCallback(CSO_HANDLE pHSession);
#if 0
/* 
 * Tuner���� �߻��ϴ� Error Event�� �ޱ� ���� Callback�� Install�Ѵ�.
 */
DI_ERR_CODE DI_CSO_RegisterErrorEventCB (CSO_HANDLE pHSession, DI_CSO_ErrorEventCallback pErrEventCallback);

/* 
 * Tuner���� �߻��ϴ� Error Event�� �ޱ� ���� Callback�� �����Ѵ�.
 */
DI_ERR_CODE DI_CSO_UnregisterErrorEventCB (CSO_HANDLE pHSession);
#endif
/*
* Register the function that receives the custom license data (Ex. Maxdome cookie).
*/
DI_ERR_CODE DI_CSO_RegisterLicenseDataCallback(CSO_HANDLE pHSession, DI_CSO_LicenseDataCallback_t callback);

/*
* Unregister the function that receives the custom license data (Ex. Maxdome cookie).
*/
DI_ERR_CODE DI_CSO_UnregisterLicenseDataCallback(CSO_HANDLE pHSession);

/*
* Receives the custome license data (Ex. Maxdome cookie).
*/
DI_ERR_CODE DI_CSO_SetLicenseData(CSO_HANDLE pHSession, HUINT8 *pucLicenseData);
#if 0
/* 
 * Tuner���� �߻��ϴ� Error Event �� Reason �� malloc �� string���� �����Ѵ�. 
 * Notice!! �� Function �� strdup �� ������ ������ �ϹǷ� ���ϵ� String ��
 * �ݵ�� memory ���� �Ǿ�� �Ѵ�. 
 */
void * DI_CSO_GetErrorReason_StrDup (DI_CSO_ERROR_REASON_e eReason);
#endif
DI_ERR_CODE DI_CSO_GetBufferInfo(CSO_HANDLE pHSession, HUINT32 *uiChunks, HINT32 **iStartOffsets, HINT32 **iEndOffsets);


DI_ERR_CODE DI_CSO_SetPlayreadyLicenseURL (CSO_HANDLE pHSession, HCHAR *licenseURL);
DI_ERR_CODE DI_CSO_GetPlayreadyNeedCallback (void);
DI_ERR_CODE DI_CSO_GetPlayreadyLicenseURL (CSO_HANDLE pHSession, HCHAR *licenseURL);

/* For Calypso */
DI_ERR_CODE DI_CSO_GetCurrentStatus(CSO_HANDLE pHSession, HBOOL* bBOF, HBOOL* bEOF);
DI_ERR_CODE DI_CSO_SetDecoderIndex (CSO_HANDLE pHSession, HUINT32 nIdxVideoDecoder,  HUINT32 nIdxAudioDecoder);
DI_ERR_CODE DI_CSO_Flush(CSO_HANDLE pHSession);
DI_ERR_CODE DI_CSO_TrickMode(CSO_HANDLE pHSession, HBOOL bEnable);
DI_ERR_CODE DI_CSO_SetCurrentPlaypump(CSO_HANDLE pHSession, HUINT32 uiIndex);

/* Support Multi Playpump */
DI_ERR_CODE DI_CSO_SetPlaypumpType(CSO_HANDLE pHSession, HUINT32 ulIndex, DI_CSO_PumpType_e ePumpType);
void DI_CSO_SetInputStreamType(CSO_HANDLE pHSession, DI_CSO_InputStreamType_e eStreamType);

void DI_CSO_SetDuration(CSO_HANDLE pHSession, HUINT64 ullDuration);
DI_ERR_CODE DI_CSO_SetActiveAudioTrack(CSO_HANDLE pHSession, HUINT32 unAudioCodecId, HUINT32 unAudioStreamId);
DI_ERR_CODE DI_CSO_GetActiveAudioTrack(CSO_HANDLE pHSession, HUINT32 *unAudioCodecId, HUINT32 *unAudioStreamId);
DI_ERR_CODE DI_CSO_SetActiveVideoTrack(CSO_HANDLE pHSession, HUINT32 unVideoCodecId, HUINT32 unVideoStreamId);
DI_ERR_CODE DI_CSO_GetActiveVideoTrack(CSO_HANDLE pHSession, HUINT32 *unVideoCodecId, HUINT32 *unVideoStreamId);
DI_ERR_CODE DI_CSO_DTCP_AcquireDtcpIpKey(HINT8 **pDtcpKey, HINT32 *nSize);
void DI_CSO_DTCP_ReleaseDtcpIpKey(HINT8 *pDtcpKey);
DI_ERR_CODE DI_CSO_CleanPlaypumpPidChannel (CSO_HANDLE pHSession);

void DI_CSO_SetFirstPosition(CSO_HANDLE pHSession, HUINT64 ullFirstPosition);
void DI_CSO_SetDemuxId(CSO_HANDLE pHSession, HUINT32 ulDemuxId);

/* End global function prototypes */
#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_CSO20_H__ */
