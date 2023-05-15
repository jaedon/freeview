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
	SSTR_STARTUP_LOWEST, /**< ���� Bitrate ���� ���� (Default) */
	SSTR_STARTUP_HIGHEST, /**< �ְ� Bitrate ���� ���� */
	SSTR_STARTUP_AUTO, /**< �ڵ� ���� (����� Lowest) */	
	SSTR_STARTUP_MANUAL, /**< ������ Track ���� ���� */
} SSTR_STATRTUP_MODE_e;

/** 
 * SSTR DECISION MODE
 */
typedef enum
{
	SSTR_DECISION_AUTO, /**< �ڵ����� Heuristics Module�� ���� Track Change */
	SSTR_DECISION_MANUAL, /**< User �Է¿� ���� Track Change (not supported yet) */
	SSTR_DECISION_FIXED, /**< Track Change ���� ���� */
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
 * SSTR Manifest ��� ���� 
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
 * SSTR Manifest ��� ���� 
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
 * SSTR Manifest ��� ���� 
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
 * SSTR Manifest ��� ���� 
 * Stream
 */
typedef struct SSTR_MANIFEST_Info_s
{
	HINT32 nStreamCnt;
	SSTR_STREAM_Info_t *pStream;
} SSTR_MANIFEST_Info_t;

/** 
 * SSTR Manifest ��� ���� for AP/MW 
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
 *	\brief Controller ����
 *	\param pManifest : Manifest�� ���� Buffer (Memory Parsing �� ���, File Parsing �� ��� �� ��)
 *	\param nContentLength : Manifest�� ũ�� (Memory Parsing �� ���, File Parsing �� ��� �� ��)
 *	\return SSTR CONTROLLER HANDLE(���н� NULL)
 *	\see SSTR_CONTROLLER_Close
 */
SSTR_CTRL	 		SSTR_CONTROLLER_Open (HINT8	*pManifest, HINT64 nContentLength);

/*! 
 *	\brief Controller ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return SSTR_ERR
 *	\see SSTR_CONTROLLER_Open
 */
SSTR_ERR 			SSTR_CONTROLLER_Close (SSTR_CTRL hCtrl);

/*! 
 *	\brief Controller Update( Merging ) (for Live Streaming, not supported yet) 
 *	\param hDstCtrl : Update �� SSTR CONTROLLER HANDLE
 *	\param pManifest : Manifest�� ���� Buffer (Memory Parsing �� ���, File Parsing �� ��� �� ��)
 *	\param nContentLength : Manifest�� ũ�� (Memory Parsing �� ���, File Parsing �� ��� �� ��)
 *	\return SSTR_ERR
 *	\see SSTR_CONTROLLER_Open
 */
SSTR_ERR 			SSTR_CONTROLLER_Update(SSTR_CTRL hDstCtrl, HINT8 *pManifest, HINT64 nContentLength);

/*! 
 *	\brief Fragment �� �߰��Ѵ�.
 *	\param hCtrl : Update �� SSTR CONTROLLER HANDLE
 *	\param unStream : Target Stream
 *	\param unStartTime : �߰��� Fragment�� StartTime
 *	\param unDuration : �߰��� Fragment�� Duration 
 *	\return SSTR_ERR
 *	\see 
 */
SSTR_ERR	 		SSTR_CONTROLLER_AddFragment(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullStartTime, HUINT64 ullDuration);

/*! 
 *	\brief Controller Update( �ش� Fragment ���� Fragment�� �����Ѵ�) (for Live Streaming, not supported yet) 
 *	\param hCtrl : Update �� SSTR CONTROLLER HANDLE
 *	\param unTargetStream : Target Stream
 *	\param unFragment : Target Fragment
 *	\return SSTR_ERR
 *	\see SSTR_CONTROLLER_Open
 */
SSTR_ERR 			SSTR_CONTROLLER_DiscardFragments(SSTR_CTRL hCtrl, HUINT32 unTargetStream, HUINT32 unFragment);

/*! 
 *	\brief Base Url ���� 
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param pBaseUrl : Fragment Download �� ����� BaseUrl (Manifest ��ġ)
 *	\return SSTR_ERR
 *	\see SSTR_CONTROLLER_GetFragmentUrl
 */
SSTR_ERR 			SSTR_CONTROLLER_SetBaseUrl (SSTR_CTRL hCtrl, HINT8* pBaseUrl);

/*! 
 *	\brief Bitrate�� ���� Video Track ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulBitrate : Bitrate
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR	 		SSTR_CONTROLLER_SetBitrate (SSTR_CTRL hCtrl, HUINT32 ulBitrate);

/*! 
 *	\brief �Է��� Bitrate �� ���� ������ Video Track ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulBitrate : Bitrate
 *	\return Boolean
 *	\see
 */
HBOOL 				SSTR_CONTROLLER_SetClosestBitrate (SSTR_CTRL hCtrl, HUINT32 ulBitrate);

/*! 
 *	\brief �ְ� Bitrate�� ���� Video Track ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Boolean
 *	\see
 */
HBOOL				SSTR_CONTROLLER_SetHighestBitrate (SSTR_CTRL hCtrl);

/*! 
 *	\brief ���� Bitrate�� ���� Video Track ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Boolean
 *	\see
 */
HBOOL				SSTR_CONTROLLER_SetLowestBitrate (SSTR_CTRL hCtrl);

/*! 
 *	\brief ���� Track�� Bitrate ���� �� �ܰ� ���� Bitrate�� ���� Video Track ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Boolean
 *	\see
 */
HBOOL				SSTR_CONTROLLER_BitrateUp (SSTR_CTRL hCtrl);

/*! 
 *	\brief ���� Track�� Bitrate ���� �� �ܰ� �Ʒ��� Bitrate�� ���� Video Track ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Boolean
 *	\see
 */
HBOOL				SSTR_CONTROLLER_BitrateDown (SSTR_CTRL hCtrl);

/*! 
 *	\brief Manifest�� ��� ������ ���� (for AP/MW , not supported yet)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param pSummary : Manifest ��� ���� 
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_GetSummary(SSTR_CTRL hCtrl, SSTR_SUMMARY_t *pSummary);

/*! 
 *	\brief ���� ������� Stream ���� Track Array Index�� ���� Track ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\param nTrackIndex : ����� Track Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetTrackByIndex(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HUINT32 nTrackIndex);

/*! 
 *	\brief ù A/V Stream�� Current Stream���� ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return SSTR_CONTENTS_TYPE_e
 *	\see
 */
SSTR_CONTENTS_TYPE_e	SSTR_CONTROLLER_SetDefaultAVStream(SSTR_CTRL hCtrl);

/*! 
 *	\brief ����� Stream �� Array Index�� ���� ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\param nStreamIndex : ����� Stream Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetStreamByArrayIndex(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HUINT32 nStreamIndex);

/*! 
 *	\brief ����� Stream �� Index�� ���� ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\param nStreamIndex : ����� Stream Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetStreamByIndex(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HUINT32 nStreamIndex);

/*! 
 *	\brief ����� Stream �� Stream Name�� ���� ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\param pStreamName : ����� Stream�� Name
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetStreamByName(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, HINT8 *pStreamName);

/*! 
 *	\brief Ư�� �ð��� �ش� �ϴ� Fragment�� Array Index�� ��ȯ �Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param ullTime : Ư�� �ð�
 *	\param *punFragment : Fragment Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_GetFragment(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullTime, HUINT32 *punFragment);

/*! 
 *	\brief Ư�� �ð��� �ش� �ϴ� Fragment�� Array Index�� ��ȯ �Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param ullTime : Ư�� �ð�
 *	\param *punFragment : Fragment Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_GetFragmentByTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullTime, HUINT32 *punFragment);

/*! 
 *	\brief Ư�� ���� �ð��� �ش� �ϴ� Fragment�� Array Index�� ��ȯ �Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param ullTime : Ư�� ���� �ð�
 *	\param *punFragment : Fragment Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_GetFragmentByStartTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT64 ullStartTime, HUINT32 *punFragment);

/*! 
 *	\brief Ư�� ��Ʈ���� ����� Fragment�� ���� �Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param ulFragment : ����� Fragment
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetFragment (SSTR_CTRL hCtrl, HUINT32 ulStream, HUINT32 ulFragment);

/*! 
 *	\brief Ư�� ��Ʈ���� Fragment�� ���� Fragment�� �����Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_NextFragment (SSTR_CTRL hCtrl, HUINT32 ulStream);

/*! 
 *	\brief Ư�� ��Ʈ���� Fragment�� ���� Fragment�� �����Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_PrevFragment (SSTR_CTRL hCtrl, HUINT32 ulStream);

/*! 
 *	\brief Ư�� ��Ʈ���� ��ü Fragment ���� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\return Fragment ��
 *	\see
 */
HUINT32				SSTR_CONTROLLER_GetTotalFragment (SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief Ư�� Ʈ���� CodecPrivateData�� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Array Index
 *	\param unTrack : Track Array Index
 *	\return CodecPrivateData 
 *	\see
 */
HINT8* 				SSTR_CONTROLLER_GetTrackCodecPrivate(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack);

/*! 
 *	\brief Ư�� Track�� Index �� ��ȯ (Index Field)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Array Index
 *	\param unTrack : Track Array Index 
 *	\return Track Index
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetTrackIndex(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack);

/*! 
 *	\brief Ư�� Track�� Bitrate �� ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Array Index
 *	\param unTrack : Track Array Index 
 *	\return Bitrate
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetTrackBitrate(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack);

/*! 
 *	\brief ���� ������� Video or Audio Stream�� CodecPrivateData�� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return CodecPrivateData 
 *	\see
 */
HINT8*				SSTR_CONTROLLER_GetCurTrackCodecPrivate(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief Ư�� Video or Audio Stream�� Track�� FourCC�� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Index
 *	\param unTrack : Track Index 
 *	\return FourCC 
 *	\see
 */
HINT8* 				SSTR_CONTROLLER_GetTrackFourCC(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unTrack);

/*! 
 *	\brief ���� ������� Video or Audio Stream�� FourCC�� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return FourCC 
 *	\see
 */
HINT8* 				SSTR_CONTROLLER_GetCurTrackFourCC(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief Ư�� Video or Audio Stream�� SubType �� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Index
 *	\return SubType 
 *	\see
 */
HINT8* 				SSTR_CONTROLLER_GetStreamSubtype(SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief ���� ������� Video or Audio Stream�� SubType �� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return SubType 
 *	\see
 */
HINT8* 				SSTR_CONTROLLER_GetCurStreamSubtype(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief ���� ������� Audio Stream�� AudioSamplingRate �� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioSamplingRate 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioSamplingRate(SSTR_CTRL hCtrl);

/*! 
 *	\brief ���� ������� Audio Stream�� PacketSize �� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioSamplingRate 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioPacketSize(SSTR_CTRL hCtrl);

/*! 
 *	\brief ���� ������� Audio Stream�� AudioChannels �� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioChannels 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioChannels(SSTR_CTRL hCtrl);

/*! 
 *	\brief ���� ������� Audio Stream�� AudioTag �� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioTag
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioTag(SSTR_CTRL hCtrl);

/*! 
 *	\brief ���� ������� Audio Stream�� AudioBitrate �� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioBitrate
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioBitrate(SSTR_CTRL hCtrl);

/*! 
 *	\brief ���� ������� Audio Stream�� AudioBitsPerSample �� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return AudioBitsPerSample
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAudioBitsPerSample(SSTR_CTRL hCtrl);

/*! 
 *	\brief ���� ������� Video Track�� Bitrate �� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Bitrate 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurBitrate (SSTR_CTRL hCtrl);

/*! 
 *	\brief Ư�� ��Ʈ���� ���� ������� Fragment Index �� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStream : Stream Array Index
 *	\return  Fragment Index  
 *	\see
 */
HUINT32				SSTR_CONTROLLER_GetCurFragment(SSTR_CTRL hCtrl, HUINT32 ulStream);

/*! 
 *	\brief ���� ������� Audio or Video ��Ʈ���� Fragment Start Time�� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return  Start Time
 *	\see
 */
HUINT64 			SSTR_CONTROLLER_GetCurFragmentStartTime(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief ���� ������� Audio or Video ��Ʈ���� Fragment End Time�� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return  End Time
 *	\see
 */
HUINT64 			SSTR_CONTROLLER_GetCurFragmentEndTime(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief ���� ������� Audio or Video ��Ʈ���� Fragment Duration�� ��ȯ�Ѵ�.
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return  Duration
 *	\see
 */
HUINT64 			SSTR_CONTROLLER_GetCurFragmentDuration(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief Heuristics ���� ����ϱ� ���� ���� Video Stream�� Fragment Duration�� ��ȯ�Ѵ�. (sec)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Duration (Sec)
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurFragmentDurationForHeur(SSTR_CTRL hCtrl);

/*! 
 *	\brief Ư�� Stream �� Ư�� Fragment�� Start Time ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param nFragment : Fragment Array Index
 *	\param *pullStartTime : ��ȯ�� Start Time
 *	\return  SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_GetFragmentStartTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unFragment, HUINT64 *pullStartTime);

/*! 
 *	\brief Ư�� Stream �� Ư�� Fragment�� End Time ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param nStream : Stream Array Index
 *	\param nFragment : Fragment Array Index
 *	\param *pullEndTime : ��ȯ�� End Time
 *	\return  SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_GetFragmentEndTime(SSTR_CTRL hCtrl, HUINT32 unStream, HUINT32 unFragment, HUINT64 *pullEndTime);

/*! 
 *	\brief ���� ������� Audio or Video Stream �� Fragment Url ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\param pUrl : SSTR URL
 *	\return  SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_GetFragmentUrl (SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType, SSTR_URL_t *pUrl);

/*! 
 *	\brief Ư�� Fragment Url ��ȯ
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
 *	\brief Media�� ��ü Duration ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Duration
 *	\see
 */
HUINT64		 		SSTR_CONTROLLER_GetMediaDuration (SSTR_CTRL hCtrl);

/*! 
 *	\brief Media �� ù Fragment�� ���� �ð� ��ȯ 
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Start Time
 *	\see
 */
HUINT64				SSTR_CONTROLLER_GetMediaStartTime (SSTR_CTRL hCtrl);

/*! 
 *	\brief DRM ���� Ȯ��
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Boolean
 *	\see
 */
HBOOL 				SSTR_CONTROLLER_IsProtected (SSTR_CTRL hCtrl);

/*! 
 *	\brief Live ���� Ȯ��
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Boolean
 *	\see
 */
HBOOL 				SSTR_CONTROLLER_IsLive (SSTR_CTRL hCtrl);

/*! 
 *	\brief DVR Window ũ�� ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  DVR Window ũ��
 *	\see
 */
HUINT64 			SSTR_CONTROLLER_GetDVRWindowLength (SSTR_CTRL hCtrl);

/*! 
 *	\brief Protection Header ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return  Protection Header
 *	\see
 */
HUINT8* 			SSTR_CONTROLLER_GetProtectionHeader (SSTR_CTRL hCtrl);

/*! 
 *	\brief Base64 Decoded Protection Header ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Decoded Protection Header
 *	\see
 */
HUINT8* 			SSTR_CONTROLLER_GetDecodedProtectionHeader (SSTR_CTRL hCtrl);

/*! 
 *	\brief Protection Header ũ�� ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Protection Header ũ��
 *	\see
 */
HINT32 				SSTR_CONTROLLER_GetProtectionHeaderLen (SSTR_CTRL hCtrl);

/*! 
 *	\brief Base64 Decoded Protection Header ũ�� ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Base64 Decoded Protection Header ũ��
 *	\see
 */
HINT32 				SSTR_CONTROLLER_GetDecodedProtectionHeaderLen (SSTR_CTRL hCtrl);

/*! 
 *	\brief ��ü Stream �� ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return ��ü Stream ��
 *	\see
 */
HINT32				SSTR_CONTROLLER_GetTotalStreams (SSTR_CTRL hCtrl);

/*! 
 *	\brief Ư�� Stream ���� ��ü Track �� ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return ��ü Track ��
 *	\see
 */
HINT32				SSTR_CONTROLLER_GetTotalTracks (SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief ����� Type(AUDIO/VIDEO) ����
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return SSTR_ERR
 *	\see SSTR_CONTROLLER_GetFragmentUrl
 */
SSTR_ERR 			SSTR_CONTROLLER_SetCurStreamType(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief ������� Type(AUDIO/VIDEO) ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return SSTR_STREAM_TYPE_e
 *	\see SSTR_CONTROLLER_GetFragmentUrl
 */
SSTR_STREAM_TYPE_e 	SSTR_CONTROLLER_GetCurStreamType(SSTR_CTRL hCtrl);

/*! 
 *	\brief Ư�� Stream�� Type(AUDIO/VIDEO) ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return SSTR_STREAM_TYPE_e
 *	\see
 */
SSTR_STREAM_TYPE_e 	SSTR_CONTROLLER_GetStreamType(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief Ư�� Stream�� Index �� ��ȯ (Index Field)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return Stream Index
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetStreamIndex(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief Ư�� Stream�� Name ���̸� ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return Length of Name
 *	\see
 */
HINT32 			SSTR_CONTROLLER_GetStreamNameLen(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief Ư�� Stream�� Name �� ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return Stream Name
 *	\see
 */
HINT8*	 			SSTR_CONTROLLER_GetStreamName(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief Ư�� Stream�� Language ���̸� ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return Length of Language
 *	\see
 */
HINT32 			SSTR_CONTROLLER_GetStreamLangLen(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief Ư�� Stream�� Language �� ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\return Stream Language
 *	\see
 */
HINT8*	 			SSTR_CONTROLLER_GetStreamLang(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief Ư�� Stream�� FourCC ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Array Index
 *	\param pFourCC : ��ȯ�� FourCC Buffer
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR 			SSTR_CONTROLLER_GetSubType(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex, HINT8* pFourCC);

/*! 
 *	\brief ������� Audio or Video Stream�� Array Index ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return Stream Array Index 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAVStream(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief ������� Audio or Video Track�� Array Index ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param eType : SSTR TYPE (VIDEO/AUDIO)
 *	\return Track Array Index 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetCurAVTrack(SSTR_CTRL hCtrl, SSTR_STREAM_TYPE_e eType);

/*! 
 *	\brief Ư�� Stream�� Track Index ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream Array Index
 *	\return Track Array Index 
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetTrack(SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief ������� Video Stream ���� �ְ� Bitrate ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Max Bitrate
 *	\see
 */
HUINT32		 		SSTR_CONTROLLER_GetMaxBitrate (SSTR_CTRL hCtrl);

/*! 
 *	\brief ������� Video Stream ���� ���� Bitrate ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Min Bitrate
 *	\see
 */
HUINT32		 		SSTR_CONTROLLER_GetMinBitrate (SSTR_CTRL hCtrl);

/*! 
 *	\brief Ư�� Stream ���� �ְ� Bitrate ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream idx
 *	\return Max Bitrate
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetMaxBitrateFromStream(SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief Ư�� Stream ���� ���� Bitrate ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream idx
 *	\return Min Bitrate
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetMinBitrateFromStream(SSTR_CTRL hCtrl, HUINT32 unStream);

/*! 
 *	\brief Ư�� Stream ���� ���� Bitrate �� ���� Track Idx ��ȯ 
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param unStream : Stream idx
 *	\return Min Bitrate Track Idx
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetMinBitrateTrack(SSTR_CTRL hCtrl, HUINT32 unStream);


/*! 
 *	\brief ������� Video Stream ���� �ְ� Resolution Height ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Max Height
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetMaxHeight(SSTR_CTRL hCtrl);

/*! 
 *	\brief ������� Video Stream ���� �ְ� Resolution Width ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Max Width
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetMaxWidth(SSTR_CTRL hCtrl);

/*! 
 *	\brief Stream ���� �ְ� Resolution Width ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Index
 *	\return Max Width
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetStreamMaxWidth(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);

/*! 
 *	\brief Stream ����  �ְ� Resolution Height ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ulStreamIndex : Stream Index
 *	\return Max Height
 *	\see
 */
HUINT32 			SSTR_CONTROLLER_GetStreamMaxHeight(SSTR_CTRL hCtrl, HUINT32 ulStreamIndex);


/*! 
 *	\brief Manifest ��� ���� ��ȯ (not supported yet)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Manifest ����
 *	\see
 */
SSTR_MANIFEST_Info_t*	SSTR_CONTROLLER_GetManifestInfo (SSTR_CTRL hCtrl);

/*! 
 *	\brief Buffer Manager Open
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param ullBufferSize : Buffer ũ�� 
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
 *	\brief Buffer Manager ���� (not supported)
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\param hBufferMgr : Buffer Manager
 *	\return SSTR_ERR
 *	\see
 */
SSTR_ERR			SSTR_CONTROLLER_SetBuffer(SSTR_CTRL hCtrl, SSTR_BUFFERMGR_t* hBufferMgr);

/*! 
 *	\brief Buffer Manager ��ȯ
 *	\param hCtrl : SSTR CONTROLLER HANDLE
 *	\return Buffer Manager
 *	\see
 */
SSTR_BUFFERMGR_t*	SSTR_CONTROLLER_GetBuffer(SSTR_CTRL hCtrl);

/*! 
 *	\brief Heuristics Handle ��ȯ
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
 *	\brief Heuristics �� Panic �������� üũ�ϰ� Panic �����̸� Panic Mode ��ȯ
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

