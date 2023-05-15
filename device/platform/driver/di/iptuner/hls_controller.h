/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   hls_controller.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: Yang Hyun Uk,YoungKi Kim $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2011.07.22
 * File Description:	HLS Controller
 * Module:
 * Remarks:
 */

/**
 * @ingroup Controller
 */

/**
 * @author Hyunuk Yang(huyang@humaxdigital.com), YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 22 July 2011
 */

/**
 * @note
 * Copyright (C) 2011 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file hls_controller.h
 */

#ifndef __HLS_CONTROLLER_H__
#define __HLS_CONTROLLER_H__

#ifdef  __cplusplus
extern "C" {
#endif


/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include <unistd.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif

#include "hls_m3u8_parser.h"
#include "sstr_buffermgr.h"
#include "sstr_heuristics.h"

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
#define HLS_CONST_PADDING (1)
#define HLS_CONST_MICROSEC (1000000)
#define HLS_CONST_MILLISEC (1000)
#define HLS_CONST_MAX_BANDWIDTH (1000000000) /* 1Gbps */
/* End Macro definition */



/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/**
 * HLS Controller Handle
 */
typedef void* HLS_CTRL;

/**
 * HLS ������ ERROR CASE �ϳ��� ����.
 */
typedef enum
{
	HLS_OK, 							/**< HLS OK */
	HLS_FAIL, 							/**< HLS FAIL */
	HLS_DOWNLOAD_FAIL,				/**< HLS Download from URL is failed */
	HLS_DOWNLOAD_STOP_FORCE,			/**< HLS Download is stopded Forcely */
	HLS_M3U8_NOT_CHANGED,				/**< HLS M3U8 IS NOT CHANGED */
	HLS_M3U8_FAIL,						/**< HLS M3U8 failed */
	HLS_NO_URL_TO_DOWNLOAD,			/**< HLS No URL to Download */
	HLS_TIMEOUT,						/**< HLS TIMEOUT */
	HLS_PANIC_DETECTED, 					/**< HLS PANIC */
	HLS_PARAMETER_ERROR,				/**< HLS Parameter Error */
	HLS_RETRIEVE_KEY_FAIL,
	HLS_DECRYPTION_FAIL,
	HLS_SEEK_REQUESTE,
}HLS_ERROR_e;

/* End typedef */

typedef void (*HLS_CONTROLLER_RequestUpdateCallback) (void *hHls, HUINT32 unProgramId, HUINT32 unStreamId, HUINT32 unDelay, HBOOL bForce);


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*!
 *	\brief HLS Controller ����
 *	\param *m3u8 : parsing �� M3U8 Handle
 *	\return HLS Controller Handle
 *	\see
 */
HLS_CTRL 			HLS_CONTROLLER_Open(m3u8_t *m3u8);

/*!
 *	\brief HLS Controller ����
 *	\param hCtrl : HLS Controller Handle
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_Close(HLS_CTRL hCtrl);

/*!
 *	\brief ����� Program, Stream, Segment�� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\param unProgram : ������ Program
 *	\param unStream : ������ Stream
 *	\param unSegment : ������ Segment
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_Setup(HLS_CTRL hCtrl, HUINT32 unProgram, HUINT32 unStream, HUINT32 unSegment);

/*!
 *	\brief HLS Controller�� ������ M3U8 Handle�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return M3U8 Handle
 *	\see
 */
m3u8_t* 			HLS_CONTROLLER_GetM3U8Handle(HLS_CTRL hCtrl);

/*!
 *	\brief Buffer Manager�� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\param ullBufferSize : ������ ���� ũ��
 *	\param nBufferType : ������ ���� (not supported yet)
 *	\return ErrorCode
 *	\see SSTR_BufferMgr ���� (SSTR_BUFFERMGR_Open)
 */
HLS_ERROR_e			HLS_CONTROLLER_OpenBuffer(HLS_CTRL hCtrl, HUINT64 ullBufferSize, HINT32 nBufferType );

/*!
 *	\brief Buffer Manager�� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return ErrorCode
 *	\see SSTR_BufferMgr ���� (SSTR_BUFFERMGR_Close)
 */
HLS_ERROR_e			HLS_CONTROLLER_CloseBuffer(HLS_CTRL hCtrl);

/*!
 *	\brief Buffer Manager Handle�� ��ȯ
 *	\param hCtrl : HLS Controller Handle
 *	\return Buffer Manager Handle
 *	\see SSTR_BUFFERMGR ����
 */
SSTR_BUFFERMGR_t* 	HLS_CONTROLLER_GetBufferMgr(HLS_CTRL hCtrl);

/*!
 *	\brief Heuristics Handle�� ��ȯ
 *	\param hCtrl : HLS Controller Handle
 *	\return Heuristics Handle
 *	\see SSTR_HEURISTICS ����
 */
SSTR_HEURISTICS_t* 	HLS_CONTROLLER_GetHeuristics(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Stream ���� Bitrate�� ���� Stream���� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 		HLS_CONTROLLER_GearUp(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Stream ���� Bitrate�� ���� Stream���� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 		HLS_CONTROLLER_GearDown(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Stream�� ���� Bandwidth Stream ���� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 		HLS_CONTROLLER_GearLowest(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Stream�� �ְ� Bandwidth Stream ���� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 		HLS_CONTROLLER_GearHighest(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Stream�� Ư�� Bandwidth�� �ش�Ǵ� Bitrate�� Stream���� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\param unBandwidth : Bandwidth
 *	\return Boolean
 *	\see
 */
HBOOL 		HLS_CONTROLLER_SetClosestBandwidth(HLS_CTRL hCtrl, HUINT32 unBandwidth);

/*!
 *	\brief ���� ������� Stream�� Bitrate(Bandwidth)�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Current Bitrate
 *	\see
 */
HUINT32				HLS_CONTROLLER_GetCurBitrate(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Program ���� Stream �� ���� ���� Bitrate�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Max Bitrate
 *	\see
 */
HUINT32 			HLS_CONTROLLER_GetCurMaxBitrate(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Program ���� Stream �� ���� ���� Bitrate�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Max Bitrate
 *	\see
 */
HUINT32 			HLS_CONTROLLER_GetCurMinBitrate(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Stream�� Duration�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Current Stream Duration
 *	\see
 */
HUINT32				HLS_CONTROLLER_GetCurStreamDuration(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Stream�� Encryption ���θ� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 				HLS_CONTROLLER_IsCurStreamEncrypted(HLS_CTRL hCtrl);

/*!
 *	\brief ���� Segment�� Discontinuity ���θ� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 				HLS_CONTROLLER_IsCurSegmentDiscontinuity(HLS_CTRL hCtrl);

/*!
 *	\brief ���� Segment �� ������ Segment ���� TRUE/FALSE�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 				HLS_CONTROLLER_IsCurSegmentEndList(HLS_CTRL hCtrl);

/*!
 *	\brief ���� Stream �� ENDLIST ���� ���θ� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 				HLS_CONTROLLER_IsCurStreamEndList(HLS_CTRL hCtrl);

/*!
 *	\brief �ش� Stream�� ������ Segment Duration �� ��ȯ�Ѵ�. (Update Delay).
 *	\param hCtrl : HLS Controller Handle
 *	\param[out] *unDelay : Delay
 *	\return HLS_ERROR_e
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_GetCurUpdateDelay(HLS_CTRL hCtrl , HUINT32 *unDelay);

/*!
 *	\brief ���� Segment�� Program DateTime �� ��ȯ�Ѵ�. (UTC Time)
 *	\param hCtrl : HLS Controller Handle
 *	\return Long (UTC Time)
 *	\see
 */
HLONG 				HLS_CONTROLLER_GetCurSegmentDateTime(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Segment�� Key Url�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Key Url
 *	\see
 */
HINT8* 				HLS_CONTROLLER_GetCurKeyUrl(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Segment�� Key Method(Encryption Algorithm)�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Key Method
 *	\see m3u8Method_e
 */
m3u8Method_e 		HLS_CONTROLLER_GetCurKeyMethod(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Segment�� Key Method(Encryption Algorithm)�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Key Method
 *	\see m3u8Method_e
 */
m3u8EncPadding_e 	HLS_CONTROLLER_GetCurPaddingMethod(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Segment�� IV�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Key Url
 *	\see
 */
HUINT8* 			HLS_CONTROLLER_GetCurIV(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Segment�� URL�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Segment Url
 *	\see
 */
HINT8* 				HLS_CONTROLLER_GetCurSegmentUrl(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Segment�� Id (Sequence Number)
 *	\param hCtrl : HLS Controller Handle
 *	\param[out] *ullNum : Current Segment Id
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_GetCurSegmentId(HLS_CTRL hCtrl, HUINT64 *ullNum);

/*!
 *	\brief ���� ������� Segment�� Start Time
 *	\param hCtrl : HLS Controller Handle
 *	\return Current Segment�� Start Time
 *	\see
 */
HUINT32 			HLS_CONTROLLER_GetCurSegmentStartTime(HLS_CTRL hCtrl);

/*!
 *	\brief Ư�� Position(�ð�)�� �ش�Ǵ� SegmentId ��ȯ
 *	\param hCtrl : HLS Controller Handle
 *	\param ullTime : Segment�� Ž���ϱ� ���� Position(�ð�)
 *	\param[out] *pullSegmentId : Segment Id
 *	\return Current Segment�� Start Time
 *	\see
 */
HLS_ERROR_e			HLS_CONTROLLER_GetSegmentByTime(HLS_CTRL hCtrl, HUINT64 ullTime, HUINT64 *pullSegmentId);

/*!
 *	\brief ���� Stream�� ù Segment�� StartTime �� ��ȯ�Ѵ�. (Stream �� ���� �ð�)
 *	\param hCtrl : HLS Controller Handle
 *	\return Head Segment�� Start Time
 *	\see
 */
HFLOAT32 			HLS_CONTROLLER_GetCurStreamStartTime(HLS_CTRL hCtrl);

/*!
 *	\brief ���� Stream�� ������ Segment�� EndTime �� ��ȯ�Ѵ�. (Stream �� ���� �ð�)
 *	\param hCtrl : HLS Controller Handle
 *	\return Tail Segment�� End Time
 *	\see
 */
HFLOAT32 			HLS_CONTROLLER_GetCurStreamEndTime(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Segment�� Duration�� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Current Segment Duration
 *	\see
 */
HUINT32 			HLS_CONTROLLER_GetCurSegmentDuration(HLS_CTRL hCtrl);


/*!
 *	\brief ���� ������� Stream�� Sequence Number �� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Current Segment Duration
 *	\see
 */
HUINT64 			HLS_CONTROLLER_GetCurStreamSequenceNum(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Program�� Index Number �� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\param[out] *unNum : Program Index Number
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_GetCurProgramIndex(HLS_CTRL hCtrl, HUINT32 *unNum);

/*!
 *	\brief ���� ������� Stream�� Index Number �� ��ȯ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\param[out] *unNum : Stream Index Number
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_GetCurStreamIndex(HLS_CTRL hCtrl, HUINT32 *unNum);

/*!
 *	\brief ���� ������� Segment�� ���� Segment�� Controller�� ����
 *	\param hCtrl : HLS Controller Handle
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_NextSegment(HLS_CTRL hCtrl);

/*!
 *	\brief ���� ������� Segment�� ���� Segment�� Controller�� ����
 *	\param hCtrl : HLS Controller Handle
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_PrevSegment(HLS_CTRL hCtrl);

/*!
 *	\brief Ư�� Segment�� Controller�� ����
 *	\param hCtrl : HLS Controller Handle
 *	\param ullSegment : ������ Segment Id
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e			HLS_CONTROLLER_SetSegment(HLS_CTRL hCtrl, HUINT64 ullSegment);

/*!
 *	\brief Ư�� Position(�ð�)�� �ش��ϴ� Segment�� Controller�� ����
 *	\param hCtrl : HLS Controller Handle
 *	\param ulTime : Segment�� Ž���ϱ� ���� Position(�ð�)
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_SetSegmentByTime(HLS_CTRL hCtrl, HUINT32 ulTime);

HFLOAT32			HLS_CONTROLLER_GetTimeGapBetweenCurAndLast(HLS_CTRL hCtrl);

/*!
 *	\brief Highest Bitrate�� ���� Stream�� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see Highest Stream�� ��� �Ұ����� ���( m3u8 ���� ������ ����Ȯ�� ��� ) �� �߻��ϸ� ������� �ʵ��� ����
 */
HBOOL 				HLS_CONTROLLER_DiscardCurHighestStream(HLS_CTRL hCtrl);

#ifdef CONFIG_HLS_LIVE_RESTART
/*!
 *	\brief  ���ڷ� �־��� PlayList Sequence number���� segment�� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 				HLS_CONTROLLER_DiscardSegmentsAndKeysUntilPlayListSequenceNum(HLS_CTRL hCtrl, HUINT64 playListSeqNum);
#endif

HBOOL				HLS_CONTROLLER_DiscardSegmentsAndKeysUntilCurrent(HLS_CTRL hCtrl);

/*!
 *	\brief Heuritics ����� �ʱ�ȭ �Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\param hHLS : HLS HANDLE (Event Notify�� ���� HLS HANDLE)
 *	\param cbRequestUpdate : Request Update Callback
 *	\return void
 *	\see
 */
void 				HLS_CONTROLLER_RegisterRequestUpdateCallback(HLS_CTRL hCtrl, void* hHLS, HLS_CONTROLLER_RequestUpdateCallback cbRequestUpdate);

#if 1 // __SUPPORT_BYTERANGE__
/*!
 *	\brief ByteRange ���� ���� ��ȯ ( true - ���� O, false - ���� X )
 *	\param hCtrl : HLS Controller Handle
 *	\return TRUE, FALSE
 *	\see
 */
HBOOL 				HLS_CONTROLLER_GetCurSegmentByteRangeSupport(HLS_CTRL hCtrl);

/*!
 *	\brief �κ��� ByteRange ����(������ ū �ϳ��� segment �� �κ�. ���� segment ������ �����ص� ����)�� ��ȯ
 *	\param hCtrl : HLS Controller Handle
 *	\return �κ��� ByteRange ����
 *	\see
 */
HUINT64 			HLS_CONTROLLER_GetCurSegmentByteRangeLength(HLS_CTRL hCtrl);

/*!
 *	\brief �κ��� ByteRagne ���� ��ġ. ������ ū �ϳ��� segment �� �κ����� ���� ��ġ ( m3u8 ������ ���°� ��õ )
 *	\param hCtrl : HLS Controller Handle
 *	\return �κ��� ByteRagne ���� ��ġ
 *	\see
 */
HUINT64				HLS_CONTROLLER_GetCurSegmentByteRangeOffset(HLS_CTRL hCtrl);

/*!
 *	\brief �ϳ��� segment �� �����ϴ� index.
 *	\param hCtrl : HLS Controller Handle
 *	\return index
 *	\see
 */
HUINT64 			HLS_CONTROLLER_GetCurSegmentByteRangeIndex(HLS_CTRL hCtrl);
#endif

/*!
 *	\brief Heuritics ����� �ʱ�ȭ �Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return ErrorCode
 *	\see Seek �߻� �� ���
 */
void 				HLS_CONTROLLER_ResetHeuristics(HLS_CTRL hCtrl);

/*!
 *	\brief Panic ��Ȳ���� üũ�Ͽ� �д��̸� Lowest Bitrate�� �����Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\param ullMs : Segment�� ���ۿ� �ɸ� �ð�
 *	\return ErrorCode
 *	\see SSTR_HEURISTICS
 */
HBOOL				HLS_CONTROLLER_CheckHeuristicsPanic(HLS_CTRL hCtrl, HUINT64 ullMs);

#if defined(CONFIG_SES_HDPLUS_SMARTTV)
/*!
 *	\brief Ư�� SegmentID�� �ش�Ǵ� Position(�ð�)��ȯ
 *	\param hCtrl : HLS Controller Handle
  *	\param pullSegmentId : position�� Ž���ϱ� ���� Segment Id
 *	\param ullTime[out]* : Position(�ð�)
 *	\return Error
 *	\see
 */
HLS_ERROR_e	HLS_CONTROLLER_GetTimeBySegment(HLS_CTRL hCtrl, const HUINT64 pullSegmentId, HUINT64 * ullTime);
#endif

/*!
 *	\brief �ϳ��� m3u8 list���� �ٸ� TS�� ��ȯ���� üũ�Ѵ�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL HLS_CONTROLLER_IsCurSegmentChangedTs(HLS_CTRL hCtrl);

#ifdef CONFIG_HLS_LIVE_RESTART
/*!
 *	\brief �̿� ������ ���� �ֽ��� Segment�� �����Ѵ�. for LiveStreaming.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HLS_ERROR_e	HLS_CONTROLLER_SetSegmentToLive(HLS_CTRL hCtrl);


/*!
 *	\brief ���� ��Ʈ���� TargetDuration�� ��´�.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HUINT32 HLS_CONTROLLER_GetCurStreamTargetDuration(HLS_CTRL hCtrl);


/*!
 *	\brief Probe�� stream�� �����Ѵ�. ������ ���� ���� bit rate�� ����
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HLS_ERROR_e	HLS_CONTROLLER_SetStreamToProbe(HLS_CTRL hCtrl);

#endif

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* __HLS_CONTROLLER_H__ */
/*@}*/
