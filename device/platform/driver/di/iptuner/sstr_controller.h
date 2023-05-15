/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   sstr_controller.h  $
 * Version:			$Revision:   $
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	     SSTR Manifest Controller for Streaming Header
 * Module:
 * Remarks:	
 */	

/**
 * @ingroup SSTR_CONTROLLER
 */

/**
 * @author YoungKi Kim(ykkim3@humaxdigital.com)
 * @date 28 Feb 2012
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file sstr_controller.h
 */


#ifndef __SSTR_CONTROLLER_H__
#define __SSTR_CONTROLLER_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include <sys/time.h> 	 /** for checking delay */
#include "sstr_manifestparser.h"
#include "sstr_buffermgr.h"
#include "sstr_heuristics.h"

/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */


/* End Extern variable */

/*******************************************************************/
/************************ Constant Definition *************************/
/*******************************************************************/
#ifdef CONFIG_DI_MAX_URL_LENGTH
#define SSTR_CONST_URL_LENGTH CONFIG_DI_MAX_URL_LENGTH
#else
#define SSTR_CONST_URL_LENGTH (2048)
#endif
//#define SSTR_CONST_ID_LENGTH (32)
#define SSTR_CONST_VALUEBUFFER_LENGTH (512)
#define SSTR_CONST_DEFAULT_TIMESCALE 10000000
#define SSTR_CONST_HTTP_RECV_BUFFSIZE (2048*3) // 6Kbytes
#define SSTR_CONST_HTTP_BUFFERING_BLOCK (1024*60) // 60Kbytes
#define SSTR_CONST_TEXTSTREAM_DEFAULTSIZE (1024) // 1Kbyte

#define SSTR_CONST_PADDING (1)
#define SSTR_CONST_URL_PADDING (128)
#define SSTR_CONST_DEFAULT_QUEUESIZE (5)
#define SSTR_CONST_DEFAULT_INTERVAL (50000)
#define SSTR_CONST_MICROSEC (10000000)
#define SSTR_CONST_MILLISEC (1000)

#define SSTR_CONST_DEFAULT_DURATION (2) /* 2sec */
#define SSTR_CONST_DEFAULT_STACK_SIZE	(256*1024)	/* 256Kbytes */
#define SSTR_CONST_CODECPRIVDATA_BUFFER (512)
#define SSTR_CONST_EXTRADATA_BUFFER (18)
#define SSTR_CONST_HIGHEST_BUFFERING_THRESHOLD 90 / 100
#define SSTR_CONST_LOWEST_BUFFERING_THRESHOLD 10 / 100

#define SSTR_CONST_NO_BUFFER 0
#define SSTR_CONST_MEMORY_BUFFER 1
#define SSTR_CONST_FILE_BUFFER 2

#define SSTR_CONST_HIGHEST_BITRATE 0
#define SSTR_CONST_LOWEST_BITRATE 1
#define SSTR_CONST_HISTORICAL_BITRATE 2
#define SSTR_CONST_PRECALC_BITRATE 3

#define SSTR_CONST_ALGO_PROC_BITRATE_AVG 0
#define SSTR_CONST_ALGO_PROC_BITRATE_AVG_EXCEPT_LOW 1
#define SSTR_CONST_ALGO_PROC_BITRATE_AVG_EXCEPT_HL 2
#define SSTR_CONST_ALGO_PROC_BITRATE_HIGHEST 3
#define SSTR_CONST_ALGO_PROC_BITRATE_LOWEST 4

#define SSTR_CONST_DEFAULT_VIDEO_NAME "video"
#define SSTR_CONST_DEFAULT_AUDIO_NAME "audio"

#define SSTR_CONST_VIDEO_STREAM_ID 0x200
#define SSTR_CONST_AUDIO_STREAM_ID 0x100

#define SSTR_CONST_MAX_VIDEO_STREAM 8
#define SSTR_CONST_MAX_AUDIO_STREAM 8

//#define SSTR_CONST_VIDEO_STREAM_ID 2
//#define SSTR_CONST_AUDIO_STREAM_ID 1

#define SSTR_CONST_FOURCC_H264 "H264"
#define SSTR_CONST_FOURCC_WVC1 "WVC1"
#define SSTR_CONST_FOURCC_AVC1 "AVC1"
#define SSTR_CONST_FOURCC_AACL "AACL"
#define SSTR_CONST_FOURCC_AACH "AACH"
#define SSTR_CONST_FOURCC_WMAP "WMAP"
#define SSTR_CONST_FOURCC_WMA2 "WMA2"
#define SSTR_CONST_FOURCC_AC_3 "AC-3"
#define SSTR_CONST_FOURCC_EC_3 "EC-3"
#define SSTR_CONST_FOURCC_WMA "WMA"

#define SSTR_CONST_FOURCC_SCMD "SCMD"
#define SSTR_CONST_FOURCC_CHAP "CHAP"
#define SSTR_CONST_FOURCC_SUBT "SUBT"
#define SSTR_CONST_FOURCC_CAPT "CAPT"
#define SSTR_CONST_FOURCC_DESC "DESC"
#define SSTR_CONST_FOURCC_CTRL "CTRL"
#define SSTR_CONST_FOURCC_DATA "DATA"

/*******************************************************************/
/************************ Configuration Definition *************************/
/*******************************************************************/
#define SSTR_CONFIG_TEMP_CACHE "/tmp/sstr-cache"
#define SSTR_CONFIG_MEMORY_PARSE 1
//#define SSTR_CONFIG_BUFFER_TYPE SSTR_CONST_MEMORY_BUFFER
//#define SSTR_CONFIG_BUFFER_TYPE SSTR_CONST_NO_BUFFER
#define SSTR_CONFIG_AUTO_BITRATE_TYPE SSTR_CONST_LOWEST_BITRATE
#define SSTR_CONFIG_SWITCHING_ALGO_TYPE SSTR_CONST_ALGO_PROC_BITRATE_AVG_EXCEPT_HL

/*******************************************************************/
/************************ Definition *************************/
/*******************************************************************/
#define SSTR_TEMPLATE_BITRATE "{bitrate}"
#define SSTR_TEMPLATE_UPPERBITRATE "{Bitrate}"
#define SSTR_TEMPLATE_STARTTIME "{start time}"
#define SSTR_TEMPLATE_START_TIME "{start_time}"
#define SSTR_TEMPLATE_CUSTOMATTR "{CustomAttributes}"
#define SSTR_TEMPLATE_UPPERMANIFEST "Manifest"
#define SSTR_TEMPLATE_MANIFEST "manifest"
#if 0
#define SSTR_TEMPLATE_QUALITYLEVELS "QualityLevels("
#define SSTR_TEMPLATE_FRAGMENTS ")/Fragments("
#endif

//#define SSTR_TEMPLATE_INDEX "$Index$"
//#define SSTR_TEMPLATE_D_DOLLAR "$$"
//#define SSTR_TEMPLATE_DOLLAR "$"

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define SSTR_CONVERT_TO_MILLISEC(x) (x/10000)

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/** 
 * SSTR CONTROLLER HANDLE
 */
typedef void* SSTR_CTRL;

/** 
 * SSTR STARTUP MODE 
 */
typedef enum
{
	SSTR_STARTUP_LOWEST, /**< 최저 Bitrate 에서 시작 (Default) */
	SSTR_STARTUP_HIGHEST, /**< 최고 Bitrate 에서 시작 */
	SSTR_STARTUP_AUTO, /**< 자동 선택 (현재는 Lowest) */	
	SSTR_STARTUP_MANUAL, /**< 선택한 Track 에서 시작 */
} SSTR_STATRTUP_MODE_e;

/** 
 * SSTR DECISION MODE
 */
typedef enum
{
	SSTR_DECISION_AUTO, /**< 자동으로 Heuristics Module을 통해 Track Change */
	SSTR_DECISION_MANUAL, /**< User 입력에 의해 Track Change (not supported yet) */
	SSTR_DECISION_FIXED, /**< Track Change 하지 않음 */
} SSTR_DECISION_MODE_e;

typedef enum
{	
	SSTR_CONTENTS_NONE = 0, /**< NOAV Content */
	SSTR_CONTENTS_AUDIO = 1, /**< Audio Only Content */
	SSTR_CONTENTS_VIDEO = 2, /**< Video Only Content */
	SSTR_CONTENTS_ALL = 3, /**< AV Content */	
} SSTR_CONTENTS_TYPE_e;

/** 
 * Stream Type (VIDEO/AUDIO/TEXT)
 */
typedef sstrStreamType_e SSTR_STREAM_TYPE_e;

/** 
 * SSTR ERRCODE
 */
typedef sstrErr_e SSTR_ERR;

typedef struct SSTR_URL_s
{
	HINT8 *url;
	HINT8 *swap;
	HUINT32 unBufSize;
} SSTR_URL_t;

/** 
 * SSTR Manifest 요약 정보 
 * Custom Attribute
 */
typedef struct SSTR_CUSTOMATTR_Info_s
{
	HINT32 nNameLen;
	HINT8* pName;
	HINT32 nValueLen;
	HINT8* pValue;
} SSTR_CUSTOMATTR_Info_t;

/** 
 * SSTR Manifest 요약 정보 
 * Track
 */
typedef struct SSTR_TRACK_Info_s
{
	HUINT32 ulIndex;
	HUINT32 ulBitrate;
	HINT8 *pFourCC;
	HINT32 nCustomAttrCnt;
	SSTR_CUSTOMATTR_Info_t *pCustomAttr;
} SSTR_TRACK_Info_t;

/** 
 * SSTR Manifest 요약 정보 
 * Stream
 */
typedef struct SSTR_STREAM_Info_s
{
	HUINT32 ulIndex;
	SSTR_STREAM_TYPE_e eType;
	HINT32 nNameLen;
	HINT8 *pName;
	HINT32 nLangLen;
	HINT8 *pLanguage;	
	HINT32 nTrackCnt;
	SSTR_TRACK_Info_t *pTrack;
} SSTR_STREAM_Info_t;

/** 
 * SSTR Manifest 요약 정보 
 * Stream
 */
typedef struct SSTR_MANIFEST_Info_s
{
	HINT32 nStreamCnt;
	SSTR_STREAM_Info_t *pStream;
} SSTR_MANIFEST_Info_t;

/** 
 * SSTR Manifest 요약 정보 for AP/MW 
 */
typedef struct SSTR_SUMMARY_s {
	/* Current Info */
//	HUINT32 ulCurFragment;
	SSTR_STREAM_TYPE_e eCurDownloadType;
	
	/* Stream Index(is not Real Index in Manifest, it is just address in Stream Array) */
	HINT32 nCurVideoIndex;
	HINT32 nCurAudioIndex;

	/* Track Index(is not Real Index in Manifest, it is just address in Track Array)*/
	HINT32 nCurVideoTrack;
	HINT32 nCurAudioTrack;

	HUINT32 ulCurVideoBitrate;
	HUINT32 ulCurAudioBitrate; /* not supported yet */

	HUINT32 ulBandwidth;
	SSTR_HEUR_STATE_e eHeurState;

} SSTR_SUMMARY_t;


/* End typedef */

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*! 
 *	\brief Controller 생성
 *	\param pManifest : Manifest를 담은 Buffer (Memory Parsing 시 사용, File Parsing 시 사용 안 함)
 *	\param nContentLength : Manifest의 크기 (Memory Parsing 시 사용, File Parsing 시 사용 안 함)
 *	\return SSTR CONTROLLER HANDLE(실패시 NULL)
 *	\see SSTR_CONTROLLER_Close
 */
SSTR_CTRL	 		SSTR_CONTROLLER_Open (HINT8	*pManifest, HINT64 nContentLength);

/*! 
 *	\brief Controller 제거
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return SSTR_ERR
 *	\see SSTR_CONTROLLER_Open
 */
SSTR_ERR 			SSTR_CONTROLLER_Close (SSTR_CTRL hCtrl);

/*! 
 *	\brief Controller Update( Merging ) (for Live Streaming, not supported yet) 
 *	\param hDstCtrl : Update 할 SSTR CONTROLLER HANDLE
 *	\param pManifest : Manifest를 담은 Buffer (Memory Parsing 시 사용, File Parsing 시 사용 안 함)
 *	\param nContentLength : Manifest의 크기 (Memory Parsing 시 사용, File Parsing 시 사용 안 함)
 *	\return SSTR_ERR
 *	\see SSTR_CONTROLLER_Open
 */
SSTR_ERR 			SSTR_CONTROLLER_Update(SSTR_CTRL hDstCtrl, HINT8 *pManifest, HINT64 nContentLength);

/*! 
 *	\brief Fragment 를 추가한다.
 *	\param hCtrl : Update 할 SSTR CONTROLLER HANDLE
 *	\param unStream : Target Stream
 *	\param unStartTime : 추가할 Fragment의 StartTime
 *	\param unDuration : 추가할 Fragment의 Duration 
 *	\return SSTR_ERR
 *	\see 
 */
SSTR_ERR	 		SSTR_CONTROLLER_AddFragment(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullStartTime, HUINT64 ullDuration);

/*! 
 *	\brief Controller Update( 해당 Fragment 이전 Fragment를 제거한다) (for Live Streaming, not supported yet) 
 *	\param hCtrl : Update 할 SSTR CONTROLLER HANDLE
 *	\param unTargetStream : Target Stream
 *	\param unFragment : Target Fragment
 *	\return SSTR_ERR
 *	\see SSTR_CONTROLLER_Open
 */
SSTR_ERR 			SSTR_CONTROLLER_DiscardFragments(SSTR_CTRL hCtrl, HUINT32 unTargetStream, HUINT32 unFragment);

/*! 
 *	\brief Base Url 지정 
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param pBaseUrl : Fragment Download 시 사용할 BaseUrl (Manifest 위치)
 *	\return SSTR_ERR
 *	\see SSTR_CONTROLLER_GetFragmentUrl
 */
SSTR_ERR 			SSTR_CONTROLLER_SetBaseUrl (SSTR_CTRL hCtrl, HINT8* pBaseUrl);

/*! 
 *	\brief Bitrate를 통해 Video Track 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulBitrate : Bitrate
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR	 		SSTR_CONTROLLER_SetBitrate (SSTR_CTRL hCtrl, HUINT32 ulBitrate);

/*! 
 *	\brief 입력한 Bitrate 와 가장 근접한 Video Track 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulBitrate : Bitrate
 *	\return Boolean
 *	\see
 */
HBOOL 				SSTR_CONTROLLER_SetClosestBitrate (SSTR_CTRL hCtrl, HUINT32 ulBitrate);

/*! 
 *	\brief 최고 Bitrate를 갖는 Video Track 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Boolean
 *	\see
 */
HBOOL				SSTR_CONTROLLER_SetHighestBitrate (SSTR_CTRL hCtrl);

/*! 
 *	\brief 최저 Bitrate를 갖는 Video Track 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Boolean
 *	\see
 */
HBOOL				SSTR_CONTROLLER_SetLowestBitrate (SSTR_CTRL hCtrl);

/*! 
 *	\brief 현재 Track의 Bitrate 보다 한 단계 위의 Bitrate를 갖는 Video Track 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Boolean
 *	\see
 */
HBOOL				SSTR_CONTROLLER_BitrateUp (SSTR_CTRL hCtrl);

/*! 
 *	\brief 현재 Track의 Bitrate 보다 한 단계 아래의 Bitrate를 갖는 Video Track 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Boolean
 *	\see
 */
HBOOL				SSTR_CONTROLLER_BitrateDown (SSTR_CTRL hCtrl);

/*! 
 *	\brief Manifest의 요약 정보를 전달 (for AP/MW , not supported yet)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param pSummary : Manifest 요약 정보 
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_GetSummary(SSTR_CTRL hCtrl, SSTR_SUMMARY_t *pSummary);

/*! 
 *	\brief 현재 사용중인 Stream 내의 Track Array Index를 통해 Track 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\param nTrackIndex : 사용할 Track Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetTrackByIndex(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HUINT32 nTrackIndex);

/*! 
 *	\brief 첫 A/V Stream을 Current Stream으로 설정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return SSTR_CONTENTS_TYPE_e
 *	\see
 */
SSTR_CONTENTS_TYPE_e	SSTR_CONTROLLER_SetDefaultAVStream(SSTR_CTRL hCtrl);

/*! 
 *	\brief 사용할 Stream 을 Array Index를 통해 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\param nStreamIndex : 사용할 Stream Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetStreamByArrayIndex(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HUINT32 nStreamIndex);

/*! 
 *	\brief 사용할 Stream 을 Index를 통해 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\param nStreamIndex : 사용할 Stream Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetStreamByIndex(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HUINT32 nStreamIndex);

/*! 
 *	\brief 사용할 Stream 을 Stream Name을 통해 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\param pStreamName : 사용할 Stream의 Name
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetStreamByName(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HINT8 *pStreamName);

/*! 
 *	\brief 특정 시간에 해당 하는 Fragment의 Array Index를 반환 한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param ullTime : 특정 시간
 *	\param *punFragment : Fragment Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_GetFragment(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullTime, HUINT32 *punFragment);

/*! 
 *	\brief 특정 시간에 해당 하는 Fragment의 Array Index를 반환 한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param ullTime : 특정 시간
 *	\param *punFragment : Fragment Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_GetFragmentByTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullTime, HUINT32 *punFragment);

/*! 
 *	\brief 특정 시작 시간에 해당 하는 Fragment의 Array Index를 반환 한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param ullTime : 특정 시작 시간
 *	\param *punFragment : Fragment Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_GetFragmentByStartTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullStartTime, HUINT32 *punFragment);

/*! 
 *	\brief 특정 스트림에 사용할 Fragment를 지정 한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param ulFragment : 사용할 Fragment
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetFragment (SSTR_CTRL hCtrl, HUINT32 ulStream, HUINT32 ulFragment);

/*! 
 *	\brief 특정 스트림의 Fragment를 다음 Fragment로 지정한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_NextFragment (SSTR_CTRL hCtrl, HUINT32 ulStream);

/*! 
 *	\brief 특정 스트림의 Fragment를 이전 Fragment로 지정한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_PrevFragment (SSTR_CTRL hCtrl, HUINT32 ulStream);

/*! 
 *	\brief 특정 스트림의 전체 Fragment 수를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\return Fragment 수
 *	\see
 */
HUINT32				SSTR_CONTROLLER_GetTotalFragment (SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief 특정 트랙의 CodecPrivateData를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Array Index
 *	\param unTrack : Track Array Index
 *	\return CodecPrivateData 
 *	\see
 */
HINT8* 				SSTR_CONTROLLER_GetTrackCodecPrivate(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack);

/*! 
 *	\brief 특정 Track의 Index 를 반환 (Index Field)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Array Index
 *	\param unTrack : Track Array Index 
 *	\return Track Index
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetTrackIndex(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack);

/*! 
 *	\brief 특정 Track의 Bitrate 를 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Array Index
 *	\param unTrack : Track Array Index 
 *	\return Bitrate
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetTrackBitrate(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack);

/*! 
 *	\brief 현재 사용중인 Video or Audio Stream의 CodecPrivateData를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return CodecPrivateData 
 *	\see
 */
HINT8*				SSTR_CONTROLLER_GetCurTrackCodecPrivate(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief 특정 Video or Audio Stream의 Track의 FourCC를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Index
 *	\param unTrack : Track Index 
 *	\return FourCC 
 *	\see
 */
HINT8* 				SSTR_CONTROLLER_GetTrackFourCC(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack);

/*! 
 *	\brief 현재 사용중인 Video or Audio Stream의 FourCC를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return FourCC 
 *	\see
 */
HINT8* 				SSTR_CONTROLLER_GetCurTrackFourCC(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief 특정 Video or Audio Stream의 SubType 을 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Index
 *	\return SubType 
 *	\see
 */
HINT8* 				SSTR_CONTROLLER_GetStreamSubtype(SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief 현재 사용중인 Video or Audio Stream의 SubType 을 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return SubType 
 *	\see
 */
HINT8* 				SSTR_CONTROLLER_GetCurStreamSubtype(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief 현재 사용중인 Audio Stream의 AudioSamplingRate 를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioSamplingRate 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioSamplingRate(SSTR_CTRL hCtrl);

/*! 
 *	\brief 현재 사용중인 Audio Stream의 PacketSize 를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioSamplingRate 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioPacketSize(SSTR_CTRL hCtrl);

/*! 
 *	\brief 현재 사용중인 Audio Stream의 AudioChannels 를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioChannels 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioChannels(SSTR_CTRL hCtrl);

/*! 
 *	\brief 현재 사용중인 Audio Stream의 AudioTag 를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioTag
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioTag(SSTR_CTRL hCtrl);

/*! 
 *	\brief 현재 사용중인 Audio Stream의 AudioBitrate 를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioBitrate
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioBitrate(SSTR_CTRL hCtrl);

/*! 
 *	\brief 현재 사용중인 Audio Stream의 AudioBitsPerSample 를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioBitsPerSample
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioBitsPerSample(SSTR_CTRL hCtrl);

/*! 
 *	\brief 현재 사용중인 Video Track의 Bitrate 를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Bitrate 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurBitrate (SSTR_CTRL hCtrl);

/*! 
 *	\brief 특정 스트림의 현재 사용중인 Fragment Index 를 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStream : Stream Array Index
 *	\return  Fragment Index  
 *	\see
 */
HUINT32				SSTR_CONTROLLER_GetCurFragment(SSTR_CTRL hCtrl, HUINT32 ulStream);

/*! 
 *	\brief 현재 사용중인 Audio or Video 스트림의 Fragment Start Time을 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return  Start Time
 *	\see
 */
HUINT64 			SSTR_CONTROLLER_GetCurFragmentStartTime(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief 현재 사용중인 Audio or Video 스트림의 Fragment End Time을 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return  End Time
 *	\see
 */
HUINT64 			SSTR_CONTROLLER_GetCurFragmentEndTime(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief 현재 사용중인 Audio or Video 스트림의 Fragment Duration을 반환한다.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return  Duration
 *	\see
 */
HUINT64 			SSTR_CONTROLLER_GetCurFragmentDuration(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief Heuristics 에서 사용하기 위해 현재 Video Stream의 Fragment Duration을 반환한다. (sec)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Duration (Sec)
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurFragmentDurationForHeur(SSTR_CTRL hCtrl);

/*! 
 *	\brief 특정 Stream 내 특정 Fragment의 Start Time 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param nFragment : Fragment Array Index
 *	\param *pullStartTime : 반환될 Start Time
 *	\return  SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_GetFragmentStartTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unFragment, HUINT64 *pullStartTime);

/*! 
 *	\brief 특정 Stream 내 특정 Fragment의 End Time 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param nFragment : Fragment Array Index
 *	\param *pullEndTime : 반환될 End Time
 *	\return  SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_GetFragmentEndTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unFragment, HUINT64 *pullEndTime);

/*! 
 *	\brief 현재 사용중인 Audio or Video Stream 의 Fragment Url 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\param pUrl : SSTR URL
 *	\return  SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_GetFragmentUrl (SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, SSTR_URL_t *pUrl);

/*! 
 *	\brief 특정 Fragment Url 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nIndex : Stream Index
 *	\param nTrack : Track Index
 *	\param nFragment : Fragment Index
 *	\param pUrl : SSTR URL
 *	\return  SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_Get_N_FragmentUrl (SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack ,HUINT32 unFragment, SSTR_URL_t *pUrl);

/*! 
 *	\brief Media의 전체 Duration 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Duration
 *	\see
 */
HUINT64		 		SSTR_CONTROLLER_GetMediaDuration (SSTR_CTRL hCtrl);

/*! 
 *	\brief Media 내 첫 Fragment의 시작 시간 반환 
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Start Time
 *	\see
 */
HUINT64				SSTR_CONTROLLER_GetMediaStartTime (SSTR_CTRL hCtrl);

/*! 
 *	\brief DRM 유무 확인
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Boolean
 *	\see
 */
HBOOL 				SSTR_CONTROLLER_IsProtected (SSTR_CTRL hCtrl);

/*! 
 *	\brief Live 여부 확인
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Boolean
 *	\see
 */
HBOOL 				SSTR_CONTROLLER_IsLive (SSTR_CTRL hCtrl);

/*! 
 *	\brief DVR Window 크기 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  DVR Window 크기
 *	\see
 */
HUINT64 			SSTR_CONTROLLER_GetDVRWindowLength (SSTR_CTRL hCtrl);

/*! 
 *	\brief Protection Header 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Protection Header
 *	\see
 */
HUINT8* 			SSTR_CONTROLLER_GetProtectionHeader (SSTR_CTRL hCtrl);

/*! 
 *	\brief Base64 Decoded Protection Header 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Decoded Protection Header
 *	\see
 */
HUINT8* 			SSTR_CONTROLLER_GetDecodedProtectionHeader (SSTR_CTRL hCtrl);

/*! 
 *	\brief Protection Header 크기 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Protection Header 크기
 *	\see
 */
HINT32 				SSTR_CONTROLLER_GetProtectionHeaderLen (SSTR_CTRL hCtrl);

/*! 
 *	\brief Base64 Decoded Protection Header 크기 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Base64 Decoded Protection Header 크기
 *	\see
 */
HINT32 				SSTR_CONTROLLER_GetDecodedProtectionHeaderLen (SSTR_CTRL hCtrl);

/*! 
 *	\brief 전체 Stream 수 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return 전체 Stream 수
 *	\see
 */
HINT32				SSTR_CONTROLLER_GetTotalStreams (SSTR_CTRL hCtrl);

/*! 
 *	\brief 특정 Stream 내의 전체 Track 수 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return 전체 Track 수
 *	\see
 */
HINT32				SSTR_CONTROLLER_GetTotalTracks (SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief 사용할 Type(AUDIO/VIDEO) 지정
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return SSTR_ERR
 *	\see SSTR_CONTROLLER_GetFragmentUrl
 */
SSTR_ERR 			SSTR_CONTROLLER_SetCurStreamType(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief 사용중인 Type(AUDIO/VIDEO) 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return SSTR_STREAM_TYPE_e
 *	\see SSTR_CONTROLLER_GetFragmentUrl
 */
SSTR_STREAM_TYPE_e 	SSTR_CONTROLLER_GetCurStreamType(SSTR_CTRL hCtrl);

/*! 
 *	\brief 특정 Stream의 Type(AUDIO/VIDEO) 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return SSTR_STREAM_TYPE_e
 *	\see
 */
SSTR_STREAM_TYPE_e 	SSTR_CONTROLLER_GetStreamType(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief 특정 Stream의 Index 를 반환 (Index Field)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return Stream Index
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetStreamIndex(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief 특정 Stream의 Name 길이를 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return Length of Name
 *	\see
 */
HINT32 			SSTR_CONTROLLER_GetStreamNameLen(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief 특정 Stream의 Name 을 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return Stream Name
 *	\see
 */
HINT8*	 			SSTR_CONTROLLER_GetStreamName(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief 특정 Stream의 Language 길이를 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return Length of Language
 *	\see
 */
HINT32 			SSTR_CONTROLLER_GetStreamLangLen(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief 특정 Stream의 Language 를 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return Stream Language
 *	\see
 */
HINT8*	 			SSTR_CONTROLLER_GetStreamLang(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief 특정 Stream의 FourCC 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\param pFourCC : 반환될 FourCC Buffer
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_GetSubType(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex, HINT8* pFourCC);

/*! 
 *	\brief 사용중인 Audio or Video Stream의 Array Index 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return Stream Array Index 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAVStream(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief 사용중인 Audio or Video Track의 Array Index 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return Track Array Index 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAVTrack(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief 특정 Stream의 Track Index 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Array Index
 *	\return Track Array Index 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetTrack(SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief 사용중인 Video Stream 내의 최고 Bitrate 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Max Bitrate
 *	\see
 */
HUINT32		 		SSTR_CONTROLLER_GetMaxBitrate (SSTR_CTRL hCtrl);

/*! 
 *	\brief 사용중인 Video Stream 내의 최저 Bitrate 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Min Bitrate
 *	\see
 */
HUINT32		 		SSTR_CONTROLLER_GetMinBitrate (SSTR_CTRL hCtrl);

/*! 
 *	\brief 특정 Stream 내의 최고 Bitrate 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream idx
 *	\return Max Bitrate
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetMaxBitrateFromStream(SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief 특정 Stream 내의 최저 Bitrate 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream idx
 *	\return Min Bitrate
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetMinBitrateFromStream(SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief 특정 Stream 내의 최저 Bitrate 를 갖는 Track Idx 반환 
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream idx
 *	\return Min Bitrate Track Idx
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetMinBitrateTrack(SSTR_CTRL hCtrl, HUINT32 unStream);


/*! 
 *	\brief 사용중인 Video Stream 내의 최고 Resolution Height 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Max Height
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetMaxHeight(SSTR_CTRL hCtrl);

/*! 
 *	\brief 사용중인 Video Stream 내의 최고 Resolution Width 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Max Width
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetMaxWidth(SSTR_CTRL hCtrl);

/*! 
 *	\brief Stream 내의 최고 Resolution Width 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Index
 *	\return Max Width
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetStreamMaxWidth(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief Stream 내의  최고 Resolution Height 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Index
 *	\return Max Height
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetStreamMaxHeight(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);


/*! 
 *	\brief Manifest 요약 정보 반환 (not supported yet)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Manifest 정보
 *	\see
 */
SSTR_MANIFEST_Info_t*	SSTR_CONTROLLER_GetManifestInfo (SSTR_CTRL hCtrl);

/*! 
 *	\brief Buffer Manager Open
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ullBufferSize : Buffer 크기 
 *	\param nBufferType : not supported yet
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_OpenBuffer(SSTR_CTRL hCtrl, HUINT64 ullBufferSize, HINT32 nBufferType );

/*! 
 *	\brief Buffer Manager Close
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_CloseBuffer(SSTR_CTRL hCtrl);

/*! 
 *	\brief Buffer Manager 설정 (not supported)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param hBufferMgr : Buffer Manager
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetBuffer(SSTR_CTRL hCtrl, SSTR_BUFFERMGR_t* hBufferMgr);

/*! 
 *	\brief Buffer Manager 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Buffer Manager
 *	\see
 */
SSTR_BUFFERMGR_t*	SSTR_CONTROLLER_GetBuffer(SSTR_CTRL hCtrl);

/*! 
 *	\brief Heuristics Handle 반환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Heuristics Handle
 *	\see
 */
SSTR_HEURISTICS_t*	SSTR_CONTROLLER_GetHeuristics(SSTR_CTRL hCtrl);

/*! 
 *	\brief Heuristics Reset
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return void
 *	\see
 */
void				SSTR_CONTROLLER_ResetHeuristics(SSTR_CTRL hCtrl);

/*! 
 *	\brief Heuristics 가 Panic 상태인지 체크하고 Panic 상태이면 Panic Mode 전환
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ullMs : Download Delay 
 *	\return Boolean
 *	\see
 */
HBOOL				SSTR_CONTROLLER_CheckHeuristicsPanic(SSTR_CTRL hCtrl, HUINT64 ullMs);


#if 0
SSTR_CONFIG_t		SSTR_CONTROLLER_GetConfig(SSTR_CTRL hCtrl);
SSTR_ERR			SSTR_CONTROLLER_SetConfig(SSTR_CTRL hCtrl, SSTR_CONFIG_t stConfig);
#endif
#if 0
HBOOL 				SSTR_CONTROLLER_IsChangeFragment(SSTR_CTRL hCtrl);
SSTR_ERR 			SSTR_CONTROLLER_SetChangeFragment(SSTR_CTRL hCtrl,HBOOL bRequest);

SSTR_ERR 			SSTR_CONTROLLER_SetTimeForSeek(SSTR_CTRL hCtrl, HUINT64 ullTime);
HUINT64 			SSTR_CONTROLLER_GetTimeForSeek(SSTR_CTRL hCtrl);

SSTR_ERR 			SSTR_CONTROLLER_SetRequestSeek(SSTR_CTRL hCtrl,HBOOL bRequest);

HBOOL 				SSTR_CONTROLLER_IsSeekRequested(SSTR_CTRL hCtrl);
HBOOL				SSTR_CONTROLLER_IsValidTimeForSeek(SSTR_CTRL hCtrl, HUINT64 ullTime);

#if SSTR_CONFIG_WRITE_FRAGMENT_FOR_TEST
SSTR_ERR			SSTR_CONTROLLER_WriteFrag (SSTR_CTRL hCtrl,HINT8* pBuf, HINT32 nBufPos, HINT32 size); /* for test */
#endif

HBOOL 				SSTR_CONTROLLER_GetDiscard(SSTR_CTRL hCtrl);
SSTR_ERR 			SSTR_CONTROLLER_SetDiscard(SSTR_CTRL hCtrl, HBOOL bDiscard);

HBOOL 				SSTR_CONTROLLER_GetDecision(SSTR_CTRL hCtrl);
SSTR_ERR 			SSTR_CONTROLLER_SetDecision(SSTR_CTRL hCtrl, HBOOL bDecision);

SSTR_ERR 			SSTR_CONTROLLER_SetStreamLength(SSTR_CTRL hCtrl, HINT64 nLength);
HINT64 				SSTR_CONTROLLER_GetStreamLength(SSTR_CTRL hCtrl);

SSTR_ERR 			SSTR_CONTROLLER_SetCurrentPosition(SSTR_CTRL hCtrl, HINT64 nPosition);
HINT64 				SSTR_CONTROLLER_GetCurrentPosition(SSTR_CTRL hCtrl);
#endif

/* End global function prototypes */



#ifdef  __cplusplus
}
#endif

#endif /* ! __SSTR_CONTROLLER_H__ */

/*@}*/

