/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_iptuner20.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  May 20 03:08:21 KST 2009 $
 * File Description:	     IP Tuner Main Header File
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_IPTUNER_H__
#define __DI_IPTUNER_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "di_media20.h"
/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */
/* IPTUNER Configuration */
typedef struct DI_IPTUNER_CONFIG_s
{
	HBOOL		bInit;
	HBOOL		bUseStorage;				/* TRUE : enable downloading, FALSE : realtime Streaming Only. */
	HINT32		nStreamBufferingSecond;		/* 0 : pure real time,  N : N second prebuffering */
	HINT32		nChunkBaseExponent;			/* 0 : default 8kbytes, N : EXPONENT value of 2. */
	/* Other configuration */
} DI_IPTUNER_CONFIG_t;
extern DI_IPTUNER_CONFIG_t g_Configuration;
/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
/* Real Time Patch Configuration */
#define CHUNK_BUUFER_DEFAULT_SIZE 64*1024
#define BROADCOM_PLAYBACK_READ_BLOCK 60*1024
#define REALTIME_PLAYBUFFER_CHUNK_TOTAL 50
#define REALTIME_PROBEBUFFER_CHUNK_TOTAL 50
#define MP4MOOV_BLOCK_MAX 6*1024*1024
#define DEFAULT_DATA_BLOCK_SIZE 8*1024
#define DEFAULT_PROBE_BUFFER_SIZE 6*1024*1024
#define DEFAULT_RAM_BUFFER_SIZE 24*1024*1024

#define DTCP_BASE_DECRYPT_BLOCK_SIZE		(16)
/* End Macro definition */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/**
 * @brief 	IPTUNER Type
 * 			Download 방식을 결정한다.
 */
typedef enum
{
	/** The Content-Item must be fully downloaded and stored before playback. */
//	DI_IPTUNER_FULL_DOWNLOAD,
	DI_IPTUNER_FILE,		/* implemented */
	/** The Content-Item is available for playback whilst it is being downloaded and stored. */
//	DI_IPTUNER_PLAYABLE_DOWNLOAD,
	DI_IPTUNER_DOWNLOADING,
	/** The Content-Item is streamed and should not be stored. */
	DI_IPTUNER_STREAMING,
	/** The Content-Item is multicast based streaming (IPTV) and should not be stored. */
	DI_IPTUNER_MULTICAST,
#if 0
	/** DLNA */
	DI_IPTUNER_DLNA
#endif
} DI_IPTUNER_DownloadType_e;


/* CANVAS BufferingConstraints */
typedef enum
{
	DI_IPTUNER_MODE_UNLIMITED,
	DI_IPTUNER_MODE_THRESHOLD,
	DI_IPTUNER_MODE_FIXED,
} DI_IPTUNER_BufferingMode_e;

typedef enum
{
	DI_IPTUNER_UNDERFLOW_AUTO,
	DI_IPTUNER_UNDERFLOW_THRESHOLD
} DI_IPTUNER_UnderflowMode;

typedef enum
{
	DI_IPTUNER_UNDERFLOW_INIT=0,
	DI_IPTUNER_UNDERFLOW_CANPLAY,
	DI_IPTUNER_UNDERFLOW_PAUSE,
	DI_IPTUNER_UNDERFLOW_RESUME
} DI_IPTUNER_UnderflowState;

/**
 * @brief 	IPTUNER Protocols enum.
 * 			Download & Streaming Protocol type
 */
typedef enum
{
	DI_IPTUNER_PROTOCOL_FILE,		/* implemented */
	DI_IPTUNER_PROTOCOL_HTTP,		/* implemented */
	DI_IPTUNER_PROTOCOL_HTTPS,		/* implemented */
	DI_IPTUNER_PROTOCOL_ABR,		/* not implemented yet */
	DI_IPTUNER_PROTOCOL_SSTR,		/* not implemented yet */
	DI_IPTUNER_PROTOCOL_HLS,		/* not implemented yet */
	DI_IPTUNER_PROTOCOL_DASH,
	DI_IPTUNER_PROTOCOL_RTSP,		/* not implemented yet */
	DI_IPTUNER_PROTOCOL_MMS,		/* implemented */
	DI_IPTUNER_PROTOCOL_UDP,		/* UDP/unicast   : unsupported */
	DI_IPTUNER_PROTOCOL_UDP_MCAST,		/* UDP/multicast : implemented */
	DI_IPTUNER_PROTOCOL_RTP,		/* RTP/unicast   : unsupported */
	DI_IPTUNER_PROTOCOL_RTP_MCAST,		/* RTP/multicast : implemented */
	DI_IPTUNER_PROTOCOL_FTP,		/* not implemented yet */
	DI_IPTUNER_PROTOCOL_SMB,		/* not implemented yet */
	DI_IPTUNER_PROTOCOL_UNKNOWN,
} DI_IPTUNER_ProtocolType_e;

typedef enum
{
	DI_IPTUNER_BUFFERING_START,
	DI_IPTUNER_BUFFERING_PAUSE,
	DI_IPTUNER_BUFFERING_STOP,
	DI_IPTUNER_BUFFERING_COMPLETE,
} DI_IPTUNER_Status_e;

typedef enum 
{
	DI_IPTUNER_TSINJECTOR_STATUS_STOP,
	DI_IPTUNER_TSINJECTOR_STATUS_START
} DI_IPTUNER_TSInjectorStatus_e;

typedef enum
{
	DI_IPTUNER_MULTICAST_STATUS_LEAVE,
	DI_IPTUNER_MULTICAST_STATUS_JOIN,
} DI_IPTUNER_MulticastJoinStatus_e;

typedef enum 
{
	DI_IPTUNER_TIMELINE_STREAM,
	DI_IPTUNER_TIMELINE_INDEX_FILE,
	DI_IPTUNER_TIMELINE_BYTESPERSECOND,
} DI_IPTUNER_TimeLine_e;

/* enum of Probing STATUS */
typedef enum
{
	DI_IPTUNER_PROBE_INIT,
	DI_IPTUNER_PROBE_START,
	DI_IPTUNER_PROBE_END,
} DI_IPTUNER_ProbeStatus_e;

typedef enum 
{
	DI_IPTUNER_SSTR_TEXT_UNKNOWN = 0,
	DI_IPTUNER_SSTR_TEXT_SCMD,
	DI_IPTUNER_SSTR_TEXT_CHAP,
	DI_IPTUNER_SSTR_TEXT_SUBT,
	DI_IPTUNER_SSTR_TEXT_CAPT,	
	DI_IPTUNER_SSTR_TEXT_DESC,
	DI_IPTUNER_SSTR_TEXT_CTRL,
	DI_IPTUNER_SSTR_TEXT_DATA
} DI_IPTUNER_SSTR_TEXT_e;

typedef enum
{
	DI_IPTUNER_ADAPTIVE_NONE = 0,
	DI_IPTUNER_ADAPTIVE_INIT,
	DI_IPTUNER_ADAPTIVE_SSTR_ALL,
	DI_IPTUNER_ADAPTIVE_SSTR_VIDEO_STREAM,
	DI_IPTUNER_ADAPTIVE_SSTR_AUDIO_STREAM,
	DI_IPTUNER_ADAPTIVE_SSTR_TEXT_STREAM,
	DI_IPTUNER_ADAPTIVE_SSTR_VIDEO_TRACK,
	DI_IPTUNER_ADAPTIVE_SSTR_AUDIO_TRACK,
	DI_IPTUNER_ADAPTIVE_SSTR_TEXT_TRACK,
	
	DI_IPTUNER_ADAPTIVE_SSTR_UNSET_AUDIO_STREAM,
	
	DI_IPTUNER_ADAPTIVE_HLS_ALL,
	DI_IPTUNER_ADAPTIVE_HLS_PROGRAM,
	DI_IPTUNER_ADAPTIVE_HLS_STREAM
	
} DI_IPTUNER_ADAPTIVE_CONTROL_e;

/** 
 * HEURISTICS DECISION MODE
 */
typedef enum
{
	DI_IPTUNER_ADAPTIVE_DECISION_AUTO, /**< Heuristics Module 내부 Algorithm에 의해 Bitrate-Switching */
	DI_IPTUNER_ADAPTIVE_DECISION_MANUAL, /**< User 선택에 의해 Bitrate-Switching (not supported yet) */
	DI_IPTUNER_ADAPTIVE_DECISION_FIXED /**< Bitrate-Switching 하지 않음 */
} DI_IPTUNER_ADAPTIVE_DECISION_e;

/** 
 * ADAPTIVE STARTUP MODE
 */
typedef enum
{
	DI_IPTUNER_ADAPTIVE_STARTUP_LOWEST, /**< 최저 Bitrate로 Startup */
	DI_IPTUNER_ADAPTIVE_STARTUP_HIGHEST, /**< 최고 Bitrate로 Startup */
	DI_IPTUNER_ADAPTIVE_STARTUP_AUTO, /**< AUTO Mode (현재는 최저 Bitrate로 Startup) */
	DI_IPTUNER_ADAPTIVE_STARTUP_MANUAL /**< Manual Mode 선택한 Track에서 시작*/
} DI_IPTUNER_ADAPTIVE_STARTUP_e;

/* ProtocolInfo: header (string)
 *     "http-get:" 	HTTP
 *     "rtp-get" 	RTP
 */
typedef enum {
  DI_IPTUNER_DLNA_PROTOCOL_TYPE_UNKNOWN,
  DI_IPTUNER_DLNA_PROTOCOL_TYPE_HTTP,
  DI_IPTUNER_DLNA_PROTOCOL_TYPE_RTP,
} DI_IPTUNER_DLNA_PROTOCOLTYPE_e;

/* DLNA.ORG_OP: operations parameter (string)
 *     "00" (or "0") neither time seek range nor byte range supported
 *     "01" byte range supported
 *     "10" time seek range supported
 *     "11" both time seek range and byte range supported
 */
typedef enum {
  DI_IPTUNER_DLNA_SEEKMODE_NONE                  = 0x00,
  DI_IPTUNER_DLNA_SEEKMODE_BYTESEEK                 = 0x01,
  DI_IPTUNER_DLNA_SEEKMODE_TIMESEEK              = 0x10,
} DI_IPTUNER_DLNA_SEEKMODE_e;

/* DLNA.ORG_PS: play speed parameter (integer)
 *     0 invalid play speed
 *     1 normal play speed
 *     2 trick mode
 */
typedef enum {
  DI_IPTUNER_DLNA_SPEED_INVALID = 0,
  DI_IPTUNER_DLNA_SPEED_NORMAL = 1,
  DI_IPTUNER_DLNA_SPEED_TRICK = 2,
} DI_IPTUNER_DLNA_SPEED_e;

/* DLNA.ORG_CI: conversion indicator parameter (integer)
 *     0 not transcoded
 *     1 transcoded
 */
typedef enum {
  DI_IPTUNER_DLNA_CONVERSION_NONE = 0,
  DI_IPTUNER_DLNA_CONVERSION_TRANSCODED = 1,
} DI_IPTUNER_DLNA_CONVERSION_e;

/* DLNA Version */
typedef enum {
  DI_IPTUNER_DLNA_VERSION_10 = 0,
  DI_IPTUNER_DLNA_VERSION_15 = 1,
} DI_IPTUNER_DLNA_VERSION_e;

/* DLNA CLASS TYPE
 *     AV, AUDIO, IMAGE
 */
typedef enum {
  DI_IPTUNER_DLNA_CLASS_UNKNOWN,
  DI_IPTUNER_DLNA_CLASS_IMAGE,
  DI_IPTUNER_DLNA_CLASS_AUDIO,
  DI_IPTUNER_DLNA_CLASS_AV,
  DI_IPTUNER_DLNA_CLASS_COLLECTION
} DI_IPTUNER_MEDIA_CLASS_e;


/* DLNA CLASS TYPE
 *     STREAMING
 *     INTERACTIVE
 *     BACKGROUND
 */
typedef enum {
  DI_IPTUNER_DLNA_STREAMING_TRANSFER_MODE,
  DI_IPTUNER_DLNA_INTERACTIVE_TRANSFERT_MODE,
  DI_IPTUNER_DLNA_BACKGROUND_TRANSFERT_MODE
} DI_IPTUNER_TRANSFER_MODE_e;

/**
 * Internet radio type
 */
typedef enum
{
	DI_IPTUNER_INETRADIO_TYPE_NONE,		/* unknown or not internet radio */
	DI_IPTUNER_INETRADIO_TYPE_GENERIC,		/* generic inet radio */
	DI_IPTUNER_INETRADIO_TYPE_ICY			/* inet radio is ICY */
} DI_IPTUNER_INETRADIO_e;

/**
 * Buffering status for HTTP chunked live streaming (CLS)
 */
typedef enum 
{
	DI_IPTUNER_CLS_BUFSTATE_INVALID = 0,
	/* DI_IPTUNER_CLS_BUFSTATE_CANPLAY, */
	DI_IPTUNER_CLS_BUFSTATE_PREPARING  = 30, /* prepare buffering after playback_start() */
	DI_IPTUNER_CLS_BUFSTATE_BUFFERING  = 31, /* underflow buffering  */
	DI_IPTUNER_CLS_BUFSTATE_COMPLETED = 33,
} DI_IPTUNER_CLS_BUFSTATE_e;

typedef enum 
{
	DI_IPTUNER_CLS_MONITOR_STOPPED = 0,
	DI_IPTUNER_CLS_MONITOR_STARTED,
	DI_IPTUNER_CLS_MONITOR_MAX,
} DI_IPTUNER_CLS_MONITOR_STATUS_e;

/* Buffering EVENT */
typedef void (*DI_IPTUNER_BufferStartCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_BufferUnderflowCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_BufferPauseCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_BufferResumeCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_BufferCompleteCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_BufferCanPlayCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_BufferFailureCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_BufferAdaptiveStreamChangeCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_BufferStatusCallback_t)(HUINT32 ulTunerId, HUINT32* uiSize, HUINT32* uiFifoSize);

typedef struct DI_IPTUNER_BufferCallback_s
{
	DI_IPTUNER_BufferStartCallback					cbStart;
	DI_IPTUNER_BufferUnderflowCallback				cbUnderflow;
	DI_IPTUNER_BufferPauseCallback					cbPause;
	DI_IPTUNER_BufferResumeCallback					cbResume;
	DI_IPTUNER_BufferCompleteCallback				cbComplete;
	DI_IPTUNER_BufferCanPlayCallback				cbCanPlay;
	DI_IPTUNER_BufferFailureCallback				cbFailure;
	DI_IPTUNER_BufferAdaptiveStreamChangeCallback	cbAdaptiveChange;
} DI_IPTUNER_BufferCallback_t;

/* Control of Playback */
typedef void (*DI_IPTUNER_PlayerPauseCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_PlayerResumeCallback) (HUINT32 ulTunerId);
typedef struct DI_IPTUNER_PlayerControlCallback_s
{
	DI_IPTUNER_PlayerPauseCallback					cbPause;
	DI_IPTUNER_PlayerResumeCallback					cbResume;
} DI_IPTUNER_PlayerControlCallback_t;

/* probing EVENT */
typedef void (*DI_IPTUNER_ProbeCompleteCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_ProbeFailureCallback) (HUINT32 ulTunerId);
/* for Asynchronous Probing */
typedef struct DI_IPTUNER_ProbeCallback_s
{
	DI_IPTUNER_ProbeCompleteCallback				cbProbeComplete;
	DI_IPTUNER_ProbeFailureCallback					cbProbeFailure;
} DI_IPTUNER_ProbeCallback_t;

typedef void (*DI_IPTUNER_DelayedStartCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_DelayedSeekCallback) (HUINT32 ulTunerId);
typedef void (*DI_IPTUNER_ErrorEventCallback) (HUINT32 ulTunerId, DI_MEDIA_ERROR_CATEGORY_e eCate, DI_MEDIA_ERROR_REASON_e eReason);
typedef void (*DI_IPTUNER_InetRadioChangeTitleCallback) (HUINT32 ulTunerId);

/* for HTTP chunked live streaming(CLS)'s buffering status */
typedef void (*DI_IPTUNER_LiveBufferStateCallback) (HUINT32 ulTunerId, DI_IPTUNER_CLS_BUFSTATE_e eBufferState);

/* This callback is moved into di_media and unused at di_iptuner */
/* Multicast: Internal TS Injection Callback between di_media and di_iptuner (Foward received packets to di_media:TSInjector */
typedef void (*DI_IPTUNER_DataReceivedCallback) (HUINT32 uInjID, HUINT8 *pBuffer, HUINT32 unBufLen);

typedef struct DI_IPTUNER_TSInjectorCallback_s
{
	DI_IPTUNER_DataReceivedCallback					cbDataReceived;
} DI_IPTUNER_TSInjectorCallback_t;

/* TLS Certficate Callback */
typedef void (*DI_IPTUNER_VerifyExternalCallback) (HUINT32 ulTunerId, void *pUserdata, void *pVerifyhandle, HINT32 *pnResult);

typedef struct DI_IPTUNER_VerifyExternalCallback_s
{
	DI_IPTUNER_VerifyExternalCallback		cbVerify;
	void 									*pUserdata;
} DI_IPTUNER_VerifyExternalCallback_t;

/*
 * *****************************
 * BUFFERING STATUS Snapshot
 * *****************************
 * According to mediaRouter Specification 1125-S.
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
typedef struct DI_IPTUNER_Buffer_s /* for buffering in CANVAS */
{
	HINT64 llBufferedBytes;
	HINT64 llTotalBytesRemaining;
	HINT64 llTotalStreamBytes;

	HINT32 nArrivalBytedPerSecond;
	HINT32 nStreamBytesPerSecond;
	HINT32 nBufferedMilliseconds;

/*	MAXDOME */
	HUINT32 uiBufferPercentage;
	HUINT32 uiRemainBufferSec;
} DI_IPTUNER_Buffer_t;

typedef struct DI_IPTUNER_AdaptiveSettings_s
{
	/* Adaptive Mode */
	DI_IPTUNER_ADAPTIVE_STARTUP_e eStartup;
	DI_IPTUNER_ADAPTIVE_DECISION_e eDecision;
	DI_IPTUNER_ADAPTIVE_CONTROL_e eControl;

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
} DI_IPTUNER_AdaptiveSettings_t;

typedef struct DI_IPTUNER_INETRADIO_Info_s
{
	DI_AUDIO_FORMAT	eInetRadioCodec;			/* InetRadio codec type */
	HINT32	nInetRadioBitrate;					/* Bitrate info */
	unsigned long	ulInetRadioSemId;			/* InetRadio Semaphore Id */
	HINT32	nReadUnit;							/* Unit of read(in case of Icy, this could be MetaInt value) */
	HINT64	lInetRadioOffset;					/* Current Read Offset */
	HINT32	nInetRadioReadSize;					/* Read Data Size */
	HUINT32	nStartOffset;				/* Start offset in Read Buffer */
	HUINT32	nEndOffset;					/* End offset in Read Buffer */
	HUINT32	nCurrentBufferOffset;		/* Current offset in Read Buffer */
	HUINT8	*pcInetRadioReadBuffer;				/* Read InetRadio Data Buffer */
	HUINT8	*pcInetRadioReadTempBuffer;			/* Temp InetRadio Data Buffer */
} DI_IPTUNER_INETRADIO_Info_t;

typedef struct
{
	HUINT8						*pUrl;
	HBOOL						bDataReadBlock;
	HUINT64 					ullCurrent_ProbeOffset;
	HUINT64 					ullCurrent_IndexOffset;	
	HUINT64 					ullCurrent_PlayOffset;
	void						*pBufferMgr;
} RT_STREAMING_INSTANCE_t;

/* Support Play Speed (Server Side) */
typedef struct DI_IPTUNER_DLNA_PLAYSPPED_s
{
	HBOOL			bForward1x;				/* Normal Speed */
	HBOOL			bForward1_2x;
	HBOOL			bForward1_4x;
	HBOOL			bForward2x;
	HBOOL			bForward4x;
	HBOOL			bBackward1x;
	HBOOL			bBackward1_2x;
	HBOOL			bBackward1_4x;
	HBOOL			bBackward2x;
	HBOOL			bBackward4x;
} DI_IPTUNER_DLNA_PLAYSPPED_t;

typedef struct DI_IPTUNER_DlnaSettings_s
{
	HBOOL									bDlna;
	DI_IPTUNER_DLNA_PROTOCOLTYPE_e			eProtocoltype;
	DI_IPTUNER_DLNA_SEEKMODE_e				eSeekMode;
	DI_IPTUNER_DLNA_SPEED_e					eSpeed;
	DI_IPTUNER_DLNA_CONVERSION_e 			eConversion;
	DI_IPTUNER_TRANSFER_MODE_e				eTransferMode;
	HINT64									nFileSize;
	HINT32									nByteRate;
	HUINT32									unDuration;
	HBOOL									bPlaySingle;
	HINT32									nMaxSpeedMajor;
	HINT32									nMaxSpeedMinor;
	HINT8									*pcProfile;
	HBOOL									bSenderPaced;
	HBOOL									bLimitedTimeBasedSeek;
	HBOOL									bLimitedRangeBasedSeek;
	HBOOL									bDlnaPlayContainer;
	HBOOL									bS0_Increasing;
	HBOOL									bS1_Increasing;
	HBOOL									bRtspPause;
	HBOOL									bStreamingTransfer;
	HBOOL									bInteractiveTransfer;
	HBOOL									bBackgroundTransfer;
	HBOOL									bLinkProtection;
	HBOOL									bClearTextFullSeek;
	HBOOL									bClearTextLimitedSeek;
	HBOOL									bConnectionStall;
	DI_IPTUNER_DLNA_VERSION_e				eDlnaVersion;
	DI_IPTUNER_DLNA_PLAYSPPED_t				tPlaySpeed;
} DI_IPTUNER_DlnaSettings_t;

typedef struct DI_IPTUNER_DtcpIpSettings_s
{
	HBOOL									bDtcpIp;
	char									*pcDTCP1HOST;
	char									*pcDTCP1PORT;
	void									*AkeHandle;
	HINT32									nLastKeepAliveTime;		/* msec */
} DI_IPTUNER_DtcpIpSettings_t;

/**
 * @brief 	IPTUNER Structure
 * 			IPTUNER Open을 위한 structure defination
 * 			mediaInfo는 Open된 Stream의 MEDIA 정보를 제공한다.
 */
/* TODO : This function has to replace to proper position */
//typedef void (*fn_readystate_callback)(HUINT32 deviceId, HINT32 ev, HUINT32 param1);
typedef struct DI_IPTUNER_s
{
	/* common parameters for all iptuner types */
	HINT8 *pUrl;									/* This is pointer that is allocated in heap , So After using this it must be free. */
	HBOOL isAvailable;		/* TRUE or FALSE State */ /* TODO : */
	
	DI_IPTUNER_DownloadType_e 	eDownloadType;		/* FULLDOWNLOAD, PLAYABLEDOWNLOAD, STREAMING , Live Multicast(TBD)*/
	DI_IPTUNER_ProtocolType_e 	eProtocolType;		/* HTTP, RTSP, HTTPS, MMS(MMSR,MMSTU,MMSH), MULTICAST */
	DI_MEDIA_INFO_t				tMediaInfo;			/* Stream Information for Streaming */

//	DI_IPTUNER_PLAYBACK_START_PARAMS_t tStartParams;

	/* download tuner information for multi tasking */
	HINT8						*pSaveFileName;

	/* Buffering Mode */
	DI_IPTUNER_BufferingMode_e	eBufferingMode;		/* UNLIMITED, THRESHOLD, FIXED */
	HBOOL						bOverFlow;
	HUINT32						uiPauseGauge;
	/* Underflow control parameters */
	DI_IPTUNER_UnderflowMode	eUnderflowMode;		/* AUTO, THRESHOLD */
	DI_IPTUNER_UnderflowState	eUnderflowState;
	/* Underflow Handler : DI_IPTUNER_UNDERFLOW_t handler in di_iptuner_underflow.h */
	void						*ptUnderflow;
	HUINT32						uiChunksToBuffer;
	HUINT64						ullPauseCheckOffset;
	HUINT32						ulStartThreshold;	/* in Milliseconds : How much filled in buffer content data to resume or pause the playback.*/
	HUINT32						ulStartConstant;	/* in Milliseconds : The increase of buffering to determine the playback to resume or pause the playback*/
	HULONG						ulStartTime;
	/* In the case of THRESHOLD/FIXED Buffering mode */
	HUINT32						nBufferThreshold;
	HBOOL						bFastFillIsLimited;	    /* boolean parameter which limit the buffering speed in threshold location */
	HFLOAT64					dFastFillLimitMultiple;
	HFLOAT64					dManagedFillLimitMultiple;

	/* for iptuner */
	HULONG						ulIPTUNERSema;

	/* download Task Control Parameters */
	HULONG						ulDNPTaskId;
	HULONG						ulDelayedStartTaskId;
	HULONG						ulDNPSema;
	HULONG						ulDNPWriteSema;
	HULONG						ulDNPMsgQId;
	HULONG						ulDelayedStartMsgQId;
	HBOOL						bDNPTaskFinished;
	HBOOL						bDNPTaskQuit;
	HBOOL						bDelayedStartTaskFinished;
	HBOOL						bIsPlaybackStop;
	HBOOL						bExitDownload;	/* if true, a receiving function will be escape the loop */ 
	HBOOL						bExitRead;	
	HBOOL						bExitIndexRead;	
	HBOOL						bPauseDownload; /* if true, a receiving function will be slept until the flag is disabled.*/
	HBOOL						bProbeCancel; /* if true, a receiving function will be slept until the flag is disabled.*/
	HBOOL						bUnderflow;
#if 1 /* dragon test */
	HBOOL						bEscapeUnderflow; /* when a playback seek in underflow pause state, a playback should be escaped paused status to prevent PlaybackWaitingIo*/
	HBOOL						bDelayedStart;
	HBOOL						bDelayedSeek;
#endif

	/* this field support first pts value which based nexus playback (b_play_getpts).
	 * if you getting more exact first pts value
	 */
	HUINT32						unFirstPts; 
	HBOOL                       bPtsInstalled;

	/* Buffering Parameter for smartdownloading */
	DI_IPTUNER_Status_e			eBufferingStatus;	/* BUFFERING_START, BUFFERING_PAUSED, BUFFERING_COMPLETE ....etc. not defined yet.*/
	DI_IPTUNER_ProbeStatus_e	eProbeStatus;	    /* PROBE_START, PROBE_END : TEST*/


	void 						*pOLT;				/* The Offset LookUp Table Pointer for Progressive Downloader */
	HUINT64						ulDNPFileLength; 	/* TODO : should be reanmed because this field is often used in RealTime Tuner */
	HUINT32						uiBlockSize;
	HUINT32						uiRambufferSize;
	HUINT32						uiProbebufferSize;

	/* YouView TimeLine Params */
	DI_IPTUNER_TimeLine_e		eTimeLine;			/* YouView TimeLines */
	HINT8						*pIndexTimeLineUrl;	/* if eTimeLine is DI_IPTUNER_TIMELINE_INDEXFILE */
	HUINT32						nTimeLineBPS;		/* if eTimeLine is DI_IPTUNER_TIMELIN_BYTEPERSECOND */
#if 1
	HUINT8						*pIndexRoom;
#else
	HINT8						*pIndexRoom;
#endif
	HINT32						nIndexRoomSize;

	HINT8						*pUserAgentString;	/* This is used by protocol core controller (mms,http,rtsp etc) to announce what kind of client try to connect the server. */ 
	/* Endtime for Buffeirng or Playing events. */
	HUINT32						ulEndTime;			/* This msecs doesn't include margin like below ulEndByte */
	HUINT64						ulEndByte;          /* This byteoff includes some margin time (about 500msecs) to getting EOF Event from Playback */

	/* Adaptive Bitrate Streaming Parameter set */
	void						*hAbr;				/* ABR Handle */
	HBOOL						bAbrTaskInit;
	HBOOL						bAbrTaskQuit;
	HBOOL 						bAbrTaskFinished;
	HULONG						ulAbrTaskId;
	
	HULONG						ulAbrTaskSema;
	HULONG						ulAbrTaskMsgQueue;

	DI_IPTUNER_AdaptiveSettings_t tAdaptiveSettings;
	
	HBOOL						bIsProbeNeeded;
	HBOOL						bIsProbeDone;

	
	/* Internet Radio parameter */
	DI_IPTUNER_INETRADIO_Info_t	tInetRadioInfo;
	DI_IPTUNER_INETRADIO_e		eInetRadioType;
	HINT8						*pInetRadioTitle;
	HUINT32						nInetRadioAdjustOffset;
	
	/* Modified Real Time Instance */
	void						*pRealTimeInstance;				/* ABR Handle */

	HBOOL						bRealTimeTaskInit;
	HBOOL						bRealTimeTaskQuit;
	HBOOL 						bRealTimeTaskFinished;
	HULONG						ulRealTimeTaskId;	

	HULONG						ulRealTimeTaskSema;
	HULONG						ulRealTimeTaskMsgQueue;
	
	/* HTTP chunked live streaming(CLS) : buffer monitoring */
	HBOOL						bHttpChunked;
	HBOOL						bRangeAccepted;
	HBOOL						bUseClsMonitor;
	HULONG 						ulClsMonitorQid;
	HULONG						ulClsMonitorTid;
	DI_IPTUNER_CLS_BUFSTATE_e	eClsBufferState;
	DI_IPTUNER_CLS_MONITOR_STATUS_e	eClsMonitorStatus;
	DI_IPTUNER_LiveBufferStateCallback		tLiveBufferStateCallback;
	
    /*
     * We must store a STREAM Handler in iptuner handle .
     * If not so, we can't know correct stream handle in vfio function set.(VFIO_Read/VFIO_xxx etc)
     *
     * iptuner_handler --  VFIO Handler (include file and socket descriptor)
     *                 --  STREAM Handler ( include protocol controller)
     */
	/* Stream Handler : STREAM_ACCESS_t handler in stream_global.h for Probe*/
	void						*pHStream;
	/* Stream Handler : STREAM_ACCESS_t handler in stream_global.h for Data*/
	void						*pHStreamData;
	/* Stream Handler : STREAM_ACCESS_t handler in stream_global.h for Index*/
	void						*pHStreamIndex;
	/* VFIO Handler   : IPTUNER should be able to access playback io function handler */
	HBOOL						bUseVfio;
	void						*pVfioHandle;		/* for dnp local playback functions */

	DI_IPTUNER_VerifyExternalCallback_t	cbTlsCertificate;
	HINT8						*pTls_ca_name;
	HINT8						*pTls_ca_path;
	HINT8						*pTls_certi_path;
	HINT8						*pTls_prikey_path;
	HINT8						*pTls_pripw;

	HINT8						*pProtocolinfo;
	DI_IPTUNER_DlnaSettings_t	tDlnaSetting;
	DI_IPTUNER_DtcpIpSettings_t	tDtcpIpSetting;
	HBOOL						bDtcpIp;

	HINT32						nDemuxId;
	HBOOL						bSuccesiveSeekForTrick;
#ifdef CONFIG_DLNA_CERTIFICATION
	HBOOL						bDisconnectedForPause;
#endif
	/* VFIO indicators */
	unsigned int				nData;
	unsigned int				nIndex;
	unsigned int				nProbe;
	HBOOL bVFIOTypeSet;
	/* The error flags and reason */
#if 0
	DI_IPTUNER_ERROR_EXIT_REASON_e	eExitReason;
#endif
	HBOOL							bErrorExit;
	HBOOL bForceUpdateBP;

	/* Buffering Status Event */
	DI_IPTUNER_BufferCallback_t			*pBufferCallbacks;
	/* Playback Control Callback */
	DI_IPTUNER_PlayerControlCallback_t	*pPlayerControlCallbacks;
	/* Probing Status Event */
	DI_IPTUNER_ProbeCallback_t			*pProbeCallbacks;
	/* for startplayback params */
	DI_IPTUNER_DelayedStartCallback 	tDelayedStartCallback;
	DI_IPTUNER_DelayedSeekCallback 		tDelayedSeekCallback;
	DI_IPTUNER_ErrorEventCallback 		tErrorEventCallback;
	/* for Change title event in Internet radio */
	DI_IPTUNER_InetRadioChangeTitleCallback		tInetRadioChangeTitleCallback;

	DI_IPTUNER_BufferStatusCallback_t tVideoBufferCallback;
	DI_IPTUNER_BufferStatusCallback_t tAudioBufferCallback;
	DI_IPTUNER_BufferStatusCallback_t tPumpBufferCallback;

	/* Multicast interface for (locking, ...) events */
	DI_MEDIA_ChannelEventCallback_t     *pChannelCallbacks;
	
	/* Multicast Internal TS injector 
	   (Forward received data from di_iptuner to di_media 
	    and pumping to decoder) */
	HBOOL                               bUseTSInjInstance; /* Set if multicast */
	HBOOL                               bUseTSInjTask;
	HUINT32                             uTSInjID;
	HULONG                              ulTSInjTaskId;
	DI_MEDIA_ChannelTunerType_e         eChannelType;
	
	/* di_media: Internal callback between di_media and di_iptuner */
	DI_IPTUNER_TSInjectorCallback_t     *pTSInjectorCallbacks; /* TS Injector in di_media20 */
	DI_IPTUNER_TSInjectorStatus_e       eInjectorStatus;
	
	/* stream_multicast: Internal status for multicast IGMP joining */
	DI_IPTUNER_MulticastJoinStatus_e    eMulticastJoinStatus;
} DI_IPTUNER_t;

/**
 * @brief 	IPTUNER infomation
 *
 */
typedef struct DI_IPTUNER_Info_s
{
	DI_IPTUNER_t *ptIPTuner;
	HUINT64  nDownloadLength;	/* for DNP tuner : bytes*/
	HUINT64  nContentLength;	/* for DNP tuner : bytes*/

	/* MAXDOME DNP INFO */
	HUINT32 nBufferingPercent;
	HUINT32 nRemainBufferingSec;
	void * pDownloadedChunkArray;
	HUINT32 nLastChunkIndex;

}DI_IPTUNER_Info_t;

typedef struct
{
	HUINT32 unMsg;
	HUINT32 unCmd;
	HUINT32 unParam1;
	HUINT32 unParam2;
} DI_IPTUNER_Msg_t;

typedef enum
{
	CMD_CONTROL_PAUSE,	/** param no */
	CMD_CONTROL_RESUME,	/** param no */
	CMD_CONTROL_SPEED,	/** param yes (HUINT64)*/
	CMD_CONTROL_SEEK	/** param yes (HUINT64)*/
}DI_IPTUNER_CMD_e;

#if STREAM_BUFFERING
typedef void (*DI_IPTUNER_PlayerPauseCallback) (DI_IPTUNER_Index_e nTunerIndex);
typedef void (*DI_IPTUNER_PlayerPlayCallback) (DI_IPTUNER_Index_e nTunerIndex);

typedef enum {
	PLAYER_BUFFERING_CBTYPE_PLAY,
	PLAYER_BUFFERING_CBTYPE_PAUSE,
	PLAYER_BUFFERING_CBTYPE_SEEK,
	PLAYER_BUFFERING_CBTYPE_TRICK,
} DI_IPTUNER_PBCallbackType_e;

typedef void (*DI_IPTUNER_PlayerBufferingCallback) (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_PBCallbackType_e type);

typedef struct 
{
	DI_IPTUNER_PlayerPauseCallback		cbPauseCallback;
	DI_IPTUNER_PlayerPlayCallback	    cbPlayCallback;
	DI_IPTUNER_PlayerBufferingCallback	cbBufferCallback;
} DI_IPTUNER_PlayCallback_t;
#endif

#if 0 /* OLD */
typedef void (*DI_IPTUNER_ChangeTitleCallback) (DI_IPTUNER_Index_e nTunerIndex, HINT8 *pcTitleString);

typedef struct 
{
	DI_IPTUNER_ChangeTitleCallback	    cbChangeTitleCallback;
} DI_IPTUNER_EventCallback_t;
#endif


#if 0 /* we don't need to use this functions and structure in YouView Project */
typedef struct DLNA_FILECANCLE_TAG
{
	unsigned char cancel_code;

} DI_IPTUNER_DLNA_FILECANCEL_OP  ;

typedef struct DLNA_FILEDESC_TAG
{
	char *url;   // read only 로만 쓰시오

	long
			access_type;
	long			linear_access;
	DI_IPTUNER_DLNA_FILECANCEL_OP *cancel_op;
	long			option;
	unsigned char	user_data[32];
} DI_IPTUNER_DLNA_FILEDESC;

typedef struct DLNA_FILEHANDLE_TAG
{
	void		*apis;
	DI_IPTUNER_DLNA_FILEDESC desc;
	long		file_type;

	DI_IPTUNER_DLNA_FILECANCEL_OP static_cancel_op;
	DI_IPTUNER_DLNA_FILECANCEL_OP *cancel_op;
	long		error;
	long		eof;
	long		linear_access;
	long long	filesize;
	void *extra_data;
} *DI_IPTUNER_DLNA_FILEHANDLE;


typedef HUINT32 (*DLNA_OpenFile) (DI_IPTUNER_DLNA_FILEDESC *ptFileDescription, DI_IPTUNER_DLNA_FILEHANDLE *ptDLNAHandle);
typedef HUINT32 (*DLNA_CloseFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle);

typedef HUINT32 (*DLNA_ReadFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle, void *pBuffer, HUINT32 unBuffSize, HUINT32 *punReadSize);
typedef HUINT32 (*DLNA_SeekFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle, HINT64 offset, HINT32 Origin);
typedef HUINT32 (*DLNA_TellFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle, HUINT64 *plCurOffset);
typedef HUINT32 (*DLNA_IsEOFFile) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle);
typedef HUINT32 (*DLNA_GetFileSize) (DI_IPTUNER_DLNA_FILEHANDLE tDLNAHandle, HINT64 *pulFileSize);

typedef struct {
	DLNA_OpenFile			Open;
	DLNA_CloseFile			Close;
	DLNA_ReadFile			Read;
	DLNA_SeekFile			Seek;
	DLNA_TellFile			Tell;
	DLNA_IsEOFFile			IsEOF;
	DLNA_GetFileSize		GetFileSize;
	DLNA_OpenFile			OpenForNexus;
	DLNA_SeekFile			SeekForNexus;
	DLNA_GetFileSize		BoundsForNexus;
} DLNA_FileFunction_t;

#endif
/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*
 * IPTUNER usage (for PAL_MEDIA ).
 *
 * 1. Simple Downloading 
 *   - DI_IPTUNER_Create 			: Create tuner by URL.
 *   - DI_IPTUNER_RegisterBufEventCB: Register Buffering Event Callbacks for buf-information.
 *   - DI_IPTUNER_StartBuffering	: start buffering task
 *   - DI_IPTUNER_GetBufStatus		: get buffering progress information
 *   - if(EOF) Thread destroied automatically
 *   - DI_IPTUNER_Destroy			: After complete event occurred, release iptuner.
 *
 * 2. Streamed Content Playing.
 *   - DI_IPTUNER_Create
 *   - DI_IPTUNER_Open -> DI_IPTUNER_StreamOpen
 *   - DI_IPTUNER_Seek/SetSpeed
 *   - DI_IPTUNER_Close -> DI_IPTUNER_StreamClose
 *   - DI_IPTUNER_Destroy
 *
 * 3. Progressive Download Playing.
 *   - DI_IPTUNER_Create
 *   - DI_IPTUNER_RegisterBufEventCB
 *   - DI_IPTUNER_StartBuffering -> DI_IPTUNER_DnpCreate
 *   - DI_IPTUNER_GetBufStatus
 *   - DI_IPTUNER_Open -> DI_IPTUNER_DnpOpen
 *   - Playback Seek & SetSpeed
 *   - DI_IPTUNER_StopBuffering -> DI_IPTUNER_DnpDestroy
 *   - DI_IPTUNER_Close -> DI_IPTUNER_DnpClose
 *   - DI_IPTUNER_Destroy
 *
 * 4. Multicast IPTV Tuning(not Playing).
 *   - DI_IPTUNER_Create
 *   - DI_IPTUNER_RegisterChannelEventCB
 *   - DI_IPTUNER_Open -> DI_IPTUNER_MulticastOpen -> IGMP Join, not VFIO installation
 *   - DI_IPTUNER_Close -> DI_IPTUNER_MulticastClose -> IGMP Leave, not VFIO uninstallation
 *   - DI_IPTUNER_UnregisterChannelEventCB
 *   - DI_IPTUNER_Destroy
 */

/* IPTUNER Pool List */

/* System 시작시 한번 호출되며 IPTUNER Pool 을 생성한다. */
DI_ERR_CODE DI_IPTUNER_Init ( DI_IPTUNER_CONFIG_t *config );
/* System 종료시 한번 호출되어 생성되어 있는 모든 IPTUNER list 을 삭제한다. */
DI_ERR_CODE DI_IPTUNER_Uninit (void);

/* 하나의 IPTUNER 를 생성한다. 
 * 생성된 IPTUNER 는 고유의 handle 을 가지며 IPTUNER Pool 에 등록된다.
 * 입력되는 URL 을 통해 IPTUNER 의 type이 결정된다. (default : streaming 방식)
 */
DI_ERR_CODE DI_IPTUNER_Create (HINT8 *URL, DI_IPTUNER_t **pHTuner);

/* 생성된 IPTUNER handle을 입력받아 해당 IPTUNER를 삭제한 후 IPTUNER Pool 에서 삭제한다.*/
DI_ERR_CODE DI_IPTUNER_Destroy (DI_IPTUNER_t *pHTuner);

/* 
 * 생성된 IPTUNER handle 을 입력받아 Tuner의 type 에 맞게 
 * Playback 아래에 VFIO 를 Install 한다. 
 * Install 되는 VFIO 는 Playback 이 전용으로 사용하게 되며
 * index 파일까지 같이 Open 되는 경우에 대해 고려되어야 한다.
 * Multicast Live Channel의 경우, VFIO를 사용하지 않으며
 *   IGMP Join 동작을 수행한다.
 */ 
DI_ERR_CODE DI_IPTUNER_Open (DI_IPTUNER_t *pHTuner);
/* 
 * 생성된 IPTUNER handle 을 입력받아 해당 tuner 를 Close 한다.
 * 이때 Install 되었던 VFIO Set 이 삭제되어야 한다.
 * Multicast Live Channel의 경우, VFIO를 고려하지 않으며
 *   IGMP Leave 동작을 수행한다.
 */
DI_ERR_CODE DI_IPTUNER_Close (DI_IPTUNER_t *pHTuner);

/* 
 * Playback으로 Control 하지 못하는 Server Side Seek / Trick과 같은 동작을 제어한다.
 * ex) Adaptive Streaming Seek Function 
 */
DI_ERR_CODE DI_IPTUNER_Control  (DI_IPTUNER_t *pHTuner, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2);

/* 
 * 생성된 IPTUNER handle 을 입력받아 Seek 기능을 수행한다.
 * 이 Seek 기능은 Playback Seek 이 아닌 Server 단 Seek 기능을 위해 사용한다.
 * 따라서 이 함수는 Streaming Content 에 전용으로 할당된다.
 */
DI_ERR_CODE DI_IPTUNER_Seek (DI_IPTUNER_t *pHTuner, HUINT64 nPosition);

DI_ERR_CODE DI_IPTUNER_ForceMoveBufferingPosition (DI_IPTUNER_t *pHTuner);

void DI_IPTUNER_EscapeUnderflow (DI_IPTUNER_t *pHTuner);

#if 1 /* test */
HBOOL DI_IPTUNER_CheckBufferRegion (DI_IPTUNER_t *pHTuner, HUINT32 unSeekTime, HUINT32 unInterval);
#if 0
DI_ERR_CODE DI_IPTUNER_DelayedSeekTrigger (DI_IPTUNER_t *pHTuner);
#endif

#endif
/* 
 * 생성된 IPTUNER handle 을 입력받아 Seek 기능을 수행한다.
 * 이 SetSpeed 기능은 Playback Speed 설정이 아닌 Server 단 Trick 기능을 위해 사용한다.
 * 따라서 이 함수는 Streaming Content 에 전용으로 할당된다.
 */
DI_ERR_CODE DI_IPTUNER_SetSpeed (DI_IPTUNER_t *pHTuner, HINT32 speed );

/* 생성된 IPTUNER Handle 을 입력받아 Buffering Mode (specified on YouView) 를 설정한다.
 * 입력된 enum type 에 따라 buffering speed 및 size 를 결정하게 되며
 * 결정된 방식에 따라 IPTUNER Session speed 가 조절된다. 
 */
DI_ERR_CODE DI_IPTUNER_SetBufferingMode (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferingMode_e eConstraint);
DI_ERR_CODE DI_IPTUNER_GetBufferingMode (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferingMode_e *peConstraint);


/* 생성된 IPTUNER Handle 을 입력받아 Buffering Mode Parameter(specified on YouView) 를 설정한다.  */
DI_ERR_CODE DI_IPTUNER_SetBufferingModeParams (DI_IPTUNER_t *pHTuner, HUINT32 nBufferThreshold, HBOOL bFastFillIsLimited, HFLOAT64 dFastFillLimitMultiple, HFLOAT64 dManagedFillLimitMultiple);
DI_ERR_CODE DI_IPTUNER_GetBufferingModeParams (DI_IPTUNER_t *pHTuner, HUINT32 *punBufferThreshold, HBOOL *pbFastFillIsLimited, HFLOAT64 *pdFastFillLimitMultiple, HFLOAT64 *pdManagedFillLimitMultiple);

/* 
 * 생성된 IPTUNER handle 을 입력받아 Open된 iptuner 의 PlayingPosition 및 Buffering Position 관리를 위해
 * EndByte 를 설정한다.
 */
DI_ERR_CODE DI_IPTUNER_SetEndTime (DI_IPTUNER_t *pHTuner, HUINT32 ulTime );
DI_ERR_CODE DI_IPTUNER_GetEndTime (DI_IPTUNER_t *pHTuner, HUINT32 *pulTime );

/* 
 * 생성된 IPTUNER handle 을 입력받아 Tuner Structure 를 전달해준다.
 */
DI_ERR_CODE DI_IPTUNER_GetTunerInfo (DI_IPTUNER_t *pHTuner);

/* 
 * 생성된 IPTUNER 의 list 를 모두 제공한다.
 */
//DI_ERR_CODE DI_IPTUNER_GetTunerList (DI_IPTUNER_SET_t *pTunerList);

/* 생성된 IPTUNER의 handle 을 입력받아 Buffering Tuner 인 경우에 대해
 * 현재 Buffer 의 Status 를 전달해 준다. 정보는 buffer structure 를 사용한다.
 */
DI_ERR_CODE DI_IPTUNER_GetBufferStatus (DI_IPTUNER_t *pHTuner,HUINT32 nHiddenPumpBuf, HUINT32 nHiddenDecBuf , DI_IPTUNER_Buffer_t *pStatus);



DI_ERR_CODE DI_IPTUNER_GetDuration (DI_IPTUNER_t *pHTuner, HUINT32 *pulDuration);

/* 
 * 생성된 IPTUNER의 handle 을 입력받아 Buffering 을 Start 하게 된다.
 * BufferType 에는 RAMBUFFERING, HDDBuffering 두가지가 있으며
 * HTTP,RTSP 등 모든 protocol controller 에 대해 동일한 기능을 수행한다.
 */
DI_ERR_CODE DI_IPTUNER_StartBuffering (DI_IPTUNER_t *pHTuner, HINT8 *pSaveFileName);

/* 
 * 생성된 IPTUNER 의 handle 을 입력받아 Buffering Task 를 정지시킨다.
 * 정지된 Task 는 더이상 Stream 을 buffering 하지 않게 되며
 * 이에 대한 정보는 Event 를 통해 알아내야한다.
 */
DI_ERR_CODE DI_IPTUNER_PauseBuffering (DI_IPTUNER_t *pHTuner);
/* 
 * 생성된 IPTUNER 의 handle 을 입력받아 Buffering Task 를 재시작한다.
 * 재시작시 상위에서 등록한 BufferingEvent 를 통해 BUFFERING_START 가 emit 된다.
 * 이에 대한 정보는 Event 를 통해 알아내야한다.
 */
DI_ERR_CODE DI_IPTUNER_ResumeBuffering (DI_IPTUNER_t *pHTuner);

/* 
 * YouView Specific SeekOperation 
 */
DI_ERR_CODE DI_IPTUNER_ResumeBufferingBySeek (DI_IPTUNER_t *pHTuner);


/* 
 * 생성된 IPTUNER 의 handle 을 입력받아 Buffering Task 를 종료한다.
 * 종료되지 않은 content 일지라도 강제로 Buffering 을 Stop 시키며
 * Buffering 이 종료되면 Playback 에 install 된 VFIO 를 통해서도 EOF 발생시기를 조절해야한다.
 */
DI_ERR_CODE DI_IPTUNER_StopBuffering (DI_IPTUNER_t *pHTuner);

/* 
 * 생성된 IPTUNER 의 handle 을 입력받아 
 * HTTP Request Header 에 전달될 User Agent String 을 설정한다.
 * Session 에 UserAgent String 이 없으면 default User Agent String 을 설정한다.
 */
DI_ERR_CODE DI_IPTUNER_SetUserAgentString (DI_IPTUNER_t *pHTuner, HINT8 *pUserAgent, HINT32 nSize);

/* 
 * 생성된 IPTUNER 의 Buffering 상태에 따라 Playback 을 Pause/Play 하기 위한 기준값을 설정한다.
 * Playback Starting 과 Underflow 상태에 모두 관계되는 Parameter들이며
 * 설정된 값은 언제라도 바뀔수 있어야 한다.
 */
DI_ERR_CODE DI_IPTUNER_SetPlaybackStartParams (DI_IPTUNER_t *pHTuner, HUINT32 ulStartThreshold, HUINT32 ulStartConstant);
DI_ERR_CODE DI_IPTUNER_GetPlaybackStartParams (DI_IPTUNER_t *pHTuner, HUINT32 *pulStartThreshold, HUINT32 *pulStartConstant);

/* 
 * 생성된 IPTUNER 의 handle 을 입력받아 Probe Event 를 수신할 수 있도록 
 * Callback Function 을 install 한다. 이 Function 은 Probing Function 에서
 */
DI_ERR_CODE DI_IPTUNER_RegisterProbeEventCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_ProbeCallback_t *pCallbacks);
DI_ERR_CODE DI_IPTUNER_UnregisterProbeEventCB (DI_IPTUNER_t *pHTuner);

/* 
 * 생성된 IPTUNER 의 handle 을 입력받아 Buffering Event 를 수신할 수 있도록 
 * Callback Function 을 install 한다. 이 Function 은 Buffering Task 에서
 * 특정 Event 가 발생했을 때마다 호출해주게 되며 Event 의 type 은 
 * enum 값으로 define 한다. 
 */
DI_ERR_CODE DI_IPTUNER_RegisterBufEventCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferCallback_t *pCallbacks);
DI_ERR_CODE DI_IPTUNER_UnregisterBufEventCB (DI_IPTUNER_t *pHTuner);

/* 
 * Playback Starting 시 또는 Buffering Underflow 발생하는 경우 
 * Playback Pause/Resume 기능을 통해 Buffer Control을 수행한다.
 */
DI_ERR_CODE DI_IPTUNER_RegisterPlayerControlCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_PlayerControlCallback_t *pCallbacks);
DI_ERR_CODE DI_IPTUNER_UnregisterPlayerControlCB (DI_IPTUNER_t *pHTuner);

DI_ERR_CODE DI_IPTUNER_RegisterDelayedStartCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_DelayedStartCallback Callback);
DI_ERR_CODE DI_IPTUNER_UnregisterDelayedStartCB (DI_IPTUNER_t *pHTuner);

DI_ERR_CODE DI_IPTUNER_RegisterDelayedSeekCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_DelayedSeekCallback Callback);
DI_ERR_CODE DI_IPTUNER_UnregisterDelayedSeekCB (DI_IPTUNER_t *pHTuner);

DI_ERR_CODE DI_IPTUNER_RegisterErrorEventCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_ErrorEventCallback pCallback);
DI_ERR_CODE DI_IPTUNER_UnregisterErrorEventCB (DI_IPTUNER_t *pHTuner);

/* 
 * 생성된 IPTUNER 의 handle 을 입력받아 Probing 한다.
 * probeCallback function 이 iptuner handle 에 있는 경우 FAIL/COMPLETE event 를 emit 한다.
 */
DI_ERR_CODE DI_IPTUNER_Probe (DI_IPTUNER_t *pHTuner );
DI_ERR_CODE DI_IPTUNER_ProbeCancel (DI_IPTUNER_t *pHTuner );

DI_ERR_CODE DI_IPTUNER_OnExitReadFlag(DI_IPTUNER_t *pHTuner);
DI_ERR_CODE DI_IPTUNER_OffExitReadFlag(DI_IPTUNER_t *pHTuner);

/* End global function prototypes */

/* 
 * Adaptive Settings 정보를 Set/Get 한다.
 */
DI_ERR_CODE DI_IPTUNER_SetAdaptiveSettings (DI_IPTUNER_t *pHTuner, const DI_MEDIA_AdaptiveSettings_t *pAdaptiveSettings);
DI_ERR_CODE DI_IPTUNER_GetAdaptiveSettings (DI_IPTUNER_t *pHTuner, DI_MEDIA_AdaptiveSettings_t *pAdaptiveSettings);

/* Callback between di_media and di_iptuner */
DI_ERR_CODE DI_IPTUNER_RegisterTSInjectorCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_TSInjectorCallback_t *pCallbacks);
DI_ERR_CODE DI_IPTUNER_UnregisterTSInjectorCB (DI_IPTUNER_t *pHTuner);

/* 생성된 IPTUNER 의 handle 을 입력받아 Multicast Channel Event 를 수신할 수 있도록 
 * Callback Function 을 install 한다. 이 Function 은 DI_IPTUNER_Open() 수행시나 이후
 * 발생되는 Channel Locking Event를 전달한다.
 */
DI_ERR_CODE DI_IPTUNER_RegisterChannelEventCB (DI_IPTUNER_t *pHTuner, DI_MEDIA_ChannelTunerType_e eTunerType, DI_MEDIA_ChannelEventCallback_t *pCallbacks);
DI_ERR_CODE DI_IPTUNER_UnregisterChannelEventCB (DI_IPTUNER_t *pHTuner);
DI_ERR_CODE DI_IPTUNER_PrintOLTSnapshot (DI_IPTUNER_t *pHTuner);

DI_ERR_CODE DI_IPTUNER_GetBufferInfo(DI_IPTUNER_t *pHTuner, HUINT32 *uiChunks, HINT32 **iStartOffsets, HINT32 **iEndOffsets);

DI_ERR_CODE DI_IPTUNER_INETRADIO_Probe(DI_IPTUNER_t *pHTuner, DI_MEDIA_INFO_t *stMediaInfo, HBOOL bNeedHttpConnect);
DI_ERR_CODE DI_IPTUNER_RegisterInetRadioChangeTitleCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_InetRadioChangeTitleCallback callback);
DI_ERR_CODE DI_IPTUNER_UnregisterInetRadioChangeTitleCB (DI_IPTUNER_t *pHTuner);

DI_ERR_CODE DI_IPTUNER_RegisterTlsCertificateCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_VerifyExternalCallback_t *pCallback);
DI_ERR_CODE DI_IPTUNER_UnregisterTlsCertificateCB (DI_IPTUNER_t *pHTuner);

DI_ERR_CODE DI_IPTUNER_RegisterVideoBufferCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferStatusCallback_t callback);
DI_ERR_CODE DI_IPTUNER_UnregisterVideoBufferCB (DI_IPTUNER_t *pHTuner);

DI_ERR_CODE DI_IPTUNER_RegisterAudioBufferCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferStatusCallback_t callback);
DI_ERR_CODE DI_IPTUNER_UnregisterAudioBufferCB (DI_IPTUNER_t *pHTuner);

DI_ERR_CODE DI_IPTUNER_RegisterPumpBufferCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_BufferStatusCallback_t callback);
DI_ERR_CODE DI_IPTUNER_UnregisterPumpBufferCB (DI_IPTUNER_t *pHTuner);

DI_ERR_CODE DI_IPTUNER_SetCurrentPlayTime(DI_IPTUNER_t *pHTuner, HUINT32 uiTime);
DI_ERR_CODE DI_IPTUNER_CheckUnderrun(DI_IPTUNER_t* pHTuner);

/* for HTTP chunked live streaming(CLS)'s buffering state event */
DI_ERR_CODE DI_IPTUNER_RegisterLiveBufferEventCB (DI_IPTUNER_t *pHTuner, DI_IPTUNER_LiveBufferStateCallback callback);
DI_ERR_CODE DI_IPTUNER_UnregisterLiveBufferEventCB (DI_IPTUNER_t *pHTuner);
DI_ERR_CODE DI_IPTUNER_GetLiveBufferState (DI_IPTUNER_t *pHTuner, DI_IPTUNER_CLS_BUFSTATE_e *pnLiveBufferState);
DI_ERR_CODE DI_IPTUNER_SetLiveBufferState (DI_IPTUNER_t *pHTuner, DI_IPTUNER_CLS_BUFSTATE_e eLiveBufferState);

#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_IPTUNER_H__ */
