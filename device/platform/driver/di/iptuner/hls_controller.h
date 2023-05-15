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
 * HLS 전반의 ERROR CASE 하나로 묶음.
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
 *	\brief HLS Controller 생성
 *	\param *m3u8 : parsing 된 M3U8 Handle
 *	\return HLS Controller Handle
 *	\see
 */
HLS_CTRL 			HLS_CONTROLLER_Open(m3u8_t *m3u8);

/*!
 *	\brief HLS Controller 제거
 *	\param hCtrl : HLS Controller Handle
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_Close(HLS_CTRL hCtrl);

/*!
 *	\brief 재생할 Program, Stream, Segment를 설정한다.
 *	\param hCtrl : HLS Controller Handle
 *	\param unProgram : 설정할 Program
 *	\param unStream : 설정할 Stream
 *	\param unSegment : 설정할 Segment
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_Setup(HLS_CTRL hCtrl, HUINT32 unProgram, HUINT32 unStream, HUINT32 unSegment);

/*!
 *	\brief HLS Controller에 설정된 M3U8 Handle을 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return M3U8 Handle
 *	\see
 */
m3u8_t* 			HLS_CONTROLLER_GetM3U8Handle(HLS_CTRL hCtrl);

/*!
 *	\brief Buffer Manager를 생성한다.
 *	\param hCtrl : HLS Controller Handle
 *	\param ullBufferSize : 생성할 버퍼 크기
 *	\param nBufferType : 버퍼의 형태 (not supported yet)
 *	\return ErrorCode
 *	\see SSTR_BufferMgr 생성 (SSTR_BUFFERMGR_Open)
 */
HLS_ERROR_e			HLS_CONTROLLER_OpenBuffer(HLS_CTRL hCtrl, HUINT64 ullBufferSize, HINT32 nBufferType );

/*!
 *	\brief Buffer Manager를 제거한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return ErrorCode
 *	\see SSTR_BufferMgr 제거 (SSTR_BUFFERMGR_Close)
 */
HLS_ERROR_e			HLS_CONTROLLER_CloseBuffer(HLS_CTRL hCtrl);

/*!
 *	\brief Buffer Manager Handle을 반환
 *	\param hCtrl : HLS Controller Handle
 *	\return Buffer Manager Handle
 *	\see SSTR_BUFFERMGR 참조
 */
SSTR_BUFFERMGR_t* 	HLS_CONTROLLER_GetBufferMgr(HLS_CTRL hCtrl);

/*!
 *	\brief Heuristics Handle을 반환
 *	\param hCtrl : HLS Controller Handle
 *	\return Heuristics Handle
 *	\see SSTR_HEURISTICS 참조
 */
SSTR_HEURISTICS_t* 	HLS_CONTROLLER_GetHeuristics(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Stream 보다 Bitrate가 높은 Stream으로 변경한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 		HLS_CONTROLLER_GearUp(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Stream 보다 Bitrate가 낮은 Stream으로 변경한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 		HLS_CONTROLLER_GearDown(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Stream을 최저 Bandwidth Stream 으로 변경한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 		HLS_CONTROLLER_GearLowest(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Stream을 최고 Bandwidth Stream 으로 변경한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 		HLS_CONTROLLER_GearHighest(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Stream을 특정 Bandwidth에 해당되는 Bitrate의 Stream으로 변경한다.
 *	\param hCtrl : HLS Controller Handle
 *	\param unBandwidth : Bandwidth
 *	\return Boolean
 *	\see
 */
HBOOL 		HLS_CONTROLLER_SetClosestBandwidth(HLS_CTRL hCtrl, HUINT32 unBandwidth);

/*!
 *	\brief 현재 재생중인 Stream의 Bitrate(Bandwidth)를 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Current Bitrate
 *	\see
 */
HUINT32				HLS_CONTROLLER_GetCurBitrate(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Program 내의 Stream 중 가장 높은 Bitrate를 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Max Bitrate
 *	\see
 */
HUINT32 			HLS_CONTROLLER_GetCurMaxBitrate(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Program 내의 Stream 중 가장 낮은 Bitrate를 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Max Bitrate
 *	\see
 */
HUINT32 			HLS_CONTROLLER_GetCurMinBitrate(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Stream의 Duration을 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Current Stream Duration
 *	\see
 */
HUINT32				HLS_CONTROLLER_GetCurStreamDuration(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Stream의 Encryption 여부를 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 				HLS_CONTROLLER_IsCurStreamEncrypted(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 Segment의 Discontinuity 여부를 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 				HLS_CONTROLLER_IsCurSegmentDiscontinuity(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 Segment 가 마지막 Segment 인지 TRUE/FALSE를 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 				HLS_CONTROLLER_IsCurSegmentEndList(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 Stream 에 ENDLIST 포함 여부를 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 				HLS_CONTROLLER_IsCurStreamEndList(HLS_CTRL hCtrl);

/*!
 *	\brief 해당 Stream의 마지막 Segment Duration 을 반환한다. (Update Delay).
 *	\param hCtrl : HLS Controller Handle
 *	\param[out] *unDelay : Delay
 *	\return HLS_ERROR_e
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_GetCurUpdateDelay(HLS_CTRL hCtrl , HUINT32 *unDelay);

/*!
 *	\brief 현재 Segment의 Program DateTime 을 반환한다. (UTC Time)
 *	\param hCtrl : HLS Controller Handle
 *	\return Long (UTC Time)
 *	\see
 */
HLONG 				HLS_CONTROLLER_GetCurSegmentDateTime(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Segment의 Key Url을 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Key Url
 *	\see
 */
HINT8* 				HLS_CONTROLLER_GetCurKeyUrl(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Segment의 Key Method(Encryption Algorithm)을 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Key Method
 *	\see m3u8Method_e
 */
m3u8Method_e 		HLS_CONTROLLER_GetCurKeyMethod(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Segment의 Key Method(Encryption Algorithm)을 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Key Method
 *	\see m3u8Method_e
 */
m3u8EncPadding_e 	HLS_CONTROLLER_GetCurPaddingMethod(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Segment의 IV를 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Key Url
 *	\see
 */
HUINT8* 			HLS_CONTROLLER_GetCurIV(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Segment의 URL을 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Segment Url
 *	\see
 */
HINT8* 				HLS_CONTROLLER_GetCurSegmentUrl(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Segment의 Id (Sequence Number)
 *	\param hCtrl : HLS Controller Handle
 *	\param[out] *ullNum : Current Segment Id
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_GetCurSegmentId(HLS_CTRL hCtrl, HUINT64 *ullNum);

/*!
 *	\brief 현재 재생중인 Segment의 Start Time
 *	\param hCtrl : HLS Controller Handle
 *	\return Current Segment의 Start Time
 *	\see
 */
HUINT32 			HLS_CONTROLLER_GetCurSegmentStartTime(HLS_CTRL hCtrl);

/*!
 *	\brief 특정 Position(시간)이 해당되는 SegmentId 반환
 *	\param hCtrl : HLS Controller Handle
 *	\param ullTime : Segment를 탐색하기 위한 Position(시간)
 *	\param[out] *pullSegmentId : Segment Id
 *	\return Current Segment의 Start Time
 *	\see
 */
HLS_ERROR_e			HLS_CONTROLLER_GetSegmentByTime(HLS_CTRL hCtrl, HUINT64 ullTime, HUINT64 *pullSegmentId);

/*!
 *	\brief 현재 Stream의 첫 Segment의 StartTime 을 반환한다. (Stream 의 시작 시간)
 *	\param hCtrl : HLS Controller Handle
 *	\return Head Segment의 Start Time
 *	\see
 */
HFLOAT32 			HLS_CONTROLLER_GetCurStreamStartTime(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 Stream의 마지막 Segment의 EndTime 을 반환한다. (Stream 의 종료 시간)
 *	\param hCtrl : HLS Controller Handle
 *	\return Tail Segment의 End Time
 *	\see
 */
HFLOAT32 			HLS_CONTROLLER_GetCurStreamEndTime(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Segment의 Duration을 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Current Segment Duration
 *	\see
 */
HUINT32 			HLS_CONTROLLER_GetCurSegmentDuration(HLS_CTRL hCtrl);


/*!
 *	\brief 현재 재생중인 Stream의 Sequence Number 을 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Current Segment Duration
 *	\see
 */
HUINT64 			HLS_CONTROLLER_GetCurStreamSequenceNum(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Program의 Index Number 를 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\param[out] *unNum : Program Index Number
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_GetCurProgramIndex(HLS_CTRL hCtrl, HUINT32 *unNum);

/*!
 *	\brief 현재 재생중인 Stream의 Index Number 를 반환한다.
 *	\param hCtrl : HLS Controller Handle
 *	\param[out] *unNum : Stream Index Number
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_GetCurStreamIndex(HLS_CTRL hCtrl, HUINT32 *unNum);

/*!
 *	\brief 현재 재생중인 Segment의 다음 Segment를 Controller에 설정
 *	\param hCtrl : HLS Controller Handle
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_NextSegment(HLS_CTRL hCtrl);

/*!
 *	\brief 현재 재생중인 Segment의 이전 Segment를 Controller에 설정
 *	\param hCtrl : HLS Controller Handle
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_PrevSegment(HLS_CTRL hCtrl);

/*!
 *	\brief 특정 Segment를 Controller에 설정
 *	\param hCtrl : HLS Controller Handle
 *	\param ullSegment : 설정할 Segment Id
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e			HLS_CONTROLLER_SetSegment(HLS_CTRL hCtrl, HUINT64 ullSegment);

/*!
 *	\brief 특정 Position(시간)에 해당하는 Segment를 Controller에 설정
 *	\param hCtrl : HLS Controller Handle
 *	\param ulTime : Segment를 탐색하기 위한 Position(시간)
 *	\return ErrorCode
 *	\see
 */
HLS_ERROR_e 		HLS_CONTROLLER_SetSegmentByTime(HLS_CTRL hCtrl, HUINT32 ulTime);

HFLOAT32			HLS_CONTROLLER_GetTimeGapBetweenCurAndLast(HLS_CTRL hCtrl);

/*!
 *	\brief Highest Bitrate를 가진 Stream을 제거한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see Highest Stream이 사용 불가능한 경우( m3u8 내의 정보가 부정확한 경우 ) 가 발생하면 사용하지 않도록 제거
 */
HBOOL 				HLS_CONTROLLER_DiscardCurHighestStream(HLS_CTRL hCtrl);

#ifdef CONFIG_HLS_LIVE_RESTART
/*!
 *	\brief  인자로 주어진 PlayList Sequence number까지 segment를 삭제한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL 				HLS_CONTROLLER_DiscardSegmentsAndKeysUntilPlayListSequenceNum(HLS_CTRL hCtrl, HUINT64 playListSeqNum);
#endif

HBOOL				HLS_CONTROLLER_DiscardSegmentsAndKeysUntilCurrent(HLS_CTRL hCtrl);

/*!
 *	\brief Heuritics 모듈을 초기화 한다.
 *	\param hCtrl : HLS Controller Handle
 *	\param hHLS : HLS HANDLE (Event Notify를 위한 HLS HANDLE)
 *	\param cbRequestUpdate : Request Update Callback
 *	\return void
 *	\see
 */
void 				HLS_CONTROLLER_RegisterRequestUpdateCallback(HLS_CTRL hCtrl, void* hHLS, HLS_CONTROLLER_RequestUpdateCallback cbRequestUpdate);

#if 1 // __SUPPORT_BYTERANGE__
/*!
 *	\brief ByteRange 지원 여부 반환 ( true - 지원 O, false - 지원 X )
 *	\param hCtrl : HLS Controller Handle
 *	\return TRUE, FALSE
 *	\see
 */
HBOOL 				HLS_CONTROLLER_GetCurSegmentByteRangeSupport(HLS_CTRL hCtrl);

/*!
 *	\brief 부분적 ByteRange 길이(단위가 큰 하나의 segment 의 부분. 작은 segment 단위로 생각해도 무방)를 반환
 *	\param hCtrl : HLS Controller Handle
 *	\return 부분적 ByteRange 길이
 *	\see
 */
HUINT64 			HLS_CONTROLLER_GetCurSegmentByteRangeLength(HLS_CTRL hCtrl);

/*!
 *	\brief 부분적 ByteRagne 시작 위치. 단위가 큰 하나의 segment 의 부분적인 시작 위치 ( m3u8 파일을 보는걸 추천 )
 *	\param hCtrl : HLS Controller Handle
 *	\return 부분적 ByteRagne 시작 위치
 *	\see
 */
HUINT64				HLS_CONTROLLER_GetCurSegmentByteRangeOffset(HLS_CTRL hCtrl);

/*!
 *	\brief 하나의 segment 를 구별하는 index.
 *	\param hCtrl : HLS Controller Handle
 *	\return index
 *	\see
 */
HUINT64 			HLS_CONTROLLER_GetCurSegmentByteRangeIndex(HLS_CTRL hCtrl);
#endif

/*!
 *	\brief Heuritics 모듈을 초기화 한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return ErrorCode
 *	\see Seek 발생 시 사용
 */
void 				HLS_CONTROLLER_ResetHeuristics(HLS_CTRL hCtrl);

/*!
 *	\brief Panic 상황인지 체크하여 패닉이면 Lowest Bitrate로 변경한다.
 *	\param hCtrl : HLS Controller Handle
 *	\param ullMs : Segment를 전송에 걸린 시간
 *	\return ErrorCode
 *	\see SSTR_HEURISTICS
 */
HBOOL				HLS_CONTROLLER_CheckHeuristicsPanic(HLS_CTRL hCtrl, HUINT64 ullMs);

#if defined(CONFIG_SES_HDPLUS_SMARTTV)
/*!
 *	\brief 특정 SegmentID가 해당되는 Position(시간)반환
 *	\param hCtrl : HLS Controller Handle
  *	\param pullSegmentId : position을 탐색하기 위한 Segment Id
 *	\param ullTime[out]* : Position(시간)
 *	\return Error
 *	\see
 */
HLS_ERROR_e	HLS_CONTROLLER_GetTimeBySegment(HLS_CTRL hCtrl, const HUINT64 pullSegmentId, HUINT64 * ullTime);
#endif

/*!
 *	\brief 하나의 m3u8 list에서 다른 TS로 전환된지 체크한다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HBOOL HLS_CONTROLLER_IsCurSegmentChangedTs(HLS_CTRL hCtrl);

#ifdef CONFIG_HLS_LIVE_RESTART
/*!
 *	\brief 이용 가능한 가장 최신인 Segment를 선택한다. for LiveStreaming.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HLS_ERROR_e	HLS_CONTROLLER_SetSegmentToLive(HLS_CTRL hCtrl);


/*!
 *	\brief 현재 스트림의 TargetDuration을 얻는다.
 *	\param hCtrl : HLS Controller Handle
 *	\return Boolean
 *	\see
 */
HUINT32 HLS_CONTROLLER_GetCurStreamTargetDuration(HLS_CTRL hCtrl);


/*!
 *	\brief Probe할 stream을 선택한다. 가능한 가장 낮은 bit rate을 가진
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
