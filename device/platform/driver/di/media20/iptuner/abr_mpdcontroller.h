/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   abr_mpdcontroller.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			 
 * File Description:    Adaptive Bitrate Streaming MPD Controller Header File
 * Module:
 * Remarks:	
 */	

/*******************************************************************/ 	
/* Copyright (c) 2011 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/

#ifndef __ABR_MPDCONTROLLER_H__
#define __ABR_MPDCONTROLLER_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include <sys/time.h> 	 /** for checking delay */
#include "abr_queue.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */

/*******************************************************************/
/************************ Constant Definition *************************/
/*******************************************************************/
#ifdef DI_IPTUNER_URL_LENGTH
#define ABR_MPD_CONST_URL_LENGTH DI_IPTUNER_URL_LENGTH
#else
#define ABR_MPD_CONST_URL_LENGTH (512)
#endif
#define ABR_MPD_CONST_ID_LENGTH (32)
#define ABR_MPD_CONST_HTTP_RECV_BUFFSIZE (2048*3)
#define ABR_MPD_CONST_PADDING (1)
#define ABR_MPD_CONST_DEFAULT_QUEUESIZE (5)
#define ABR_MPD_CONST_DEFAULT_INTERVAL (50000)
#define ABR_MPD_CONST_MICROSEC (1000000)
#define ABR_MPD_CONST_MILLISEC (1000)
#define ABR_MPD_CONST_DEFAULT_STACK_SIZE	(256*1024)	/* 256Kbytes */

#define ABR_MPD_CONST_NO_BUFFER 0
#define ABR_MPD_CONST_MEMORY_BUFFER 1
#define ABR_MPD_CONST_FILE_BUFFER 2

#define ABR_MPD_CONST_HIGHEST_BITRATE 0
#define ABR_MPD_CONST_LOWEST_BITRATE 1
#define ABR_MPD_CONST_HISTORICAL_BITRATE 2
#define ABR_MPD_CONST_PRECALC_BITRATE 3

#define ABR_MPD_CONST_ALGO_PROC_BITRATE_AVG 0
#define ABR_MPD_CONST_ALGO_PROC_BITRATE_AVG_EXCEPT_LOW 1
#define ABR_MPD_CONST_ALGO_PROC_BITRATE_AVG_EXCEPT_HL 2
#define ABR_MPD_CONST_ALGO_PROC_BITRATE_HIGHEST 3
#define ABR_MPD_CONST_ALGO_PROC_BITRATE_LOWEST 4

/*******************************************************************/
/************************ Configuration Definition *************************/
/*******************************************************************/
#define ABR_MPD_CONFIG_TEMP_CACHE "/home/mpd-cache"
#define ABR_MPD_CONFIG_MEMORY_PARSE 1
#define ABR_MPD_CONFIG_ABR_AVGCOUNT 6
#define ABR_MPD_CONFIG_BUFFER_TYPE ABR_MPD_CONST_NO_BUFFER
#define ABR_MPD_CONFIG_AUTO_BITRATE_TYPE ABR_MPD_CONST_LOWEST_BITRATE
#define ABR_MPD_CONFIG_SWITCHING_ALGO_TYPE ABR_MPD_CONST_ALGO_PROC_BITRATE_AVG_EXCEPT_HL
/*******************************************************************/
/************************ Definition *************************/
/*******************************************************************/
#define ABR_MPD_TEMPLATE_REPRESENTID "$RepresentationID$"
#define ABR_MPD_TEMPLATE_INDEX "$Index$"
#define ABR_MPD_TEMPLATE_D_DOLLAR "$$"
#define ABR_MPD_TEMPLATE_DOLLAR "$"

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

typedef struct timeval TIMEVAL_t;

typedef void* ABR_HANDLE;
/* End typedef */

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

ABR_HANDLE 		ABR_MPD_Open (	HINT8	*pMpdBuff, HINT32 nContentLength);
ABR_HANDLE 		ABR_MPD_MemOpen (HINT8 *szBuf, HINT32 nBufSize);
ABR_HANDLE 		ABR_MPD_FileOpen (HINT8 *szPath);
ABR_ERR 		ABR_MPD_Close (ABR_HANDLE hMpd);
//ABR_ERR 		ABR_MPD_SetThroughput(ABR_HANDLE hMpd, HINT32 nRecv, HUINT32 ulRTD);
ABR_ERR 		ABR_MPD_SetThroughput(ABR_HANDLE hMpd, HINT32 nRecv, TIMEVAL_t tvstart, TIMEVAL_t tvend);
ABR_ERR 		ABR_MPD_SetBaseUrl (ABR_HANDLE hMpd, HINT8* pBaseUrl);
ABR_ERR 		ABR_MPD_SetPeriod (ABR_HANDLE hMpd, HINT32 nPeriod);
ABR_ERR 		ABR_MPD_SetBitrate (ABR_HANDLE hMpd, HUINT64 ullBitrate);
ABR_ERR 		ABR_MPD_SetLowerBitrate (ABR_HANDLE hMpd, HUINT64 ullBitrate);
ABR_ERR 		ABR_MPD_SetRepresent (ABR_HANDLE hMpd, HINT32 nRepresent);
ABR_ERR		ABR_MPD_SetSegmentByTime(ABR_HANDLE hMpd, HUINT64 *pullTime);
HINT32			ABR_MPD_GetSegmentByTime(ABR_HANDLE hMpd, HUINT64 ullTime);
ABR_ERR 		ABR_MPD_SetSegment (ABR_HANDLE hMpd, HINT32 nSegment);
ABR_ERR 		ABR_MPD_NextSegment (ABR_HANDLE hMpd);
ABR_ERR 		ABR_MPD_PrevSegment (ABR_HANDLE hMpd);
ABR_ERR 		ABR_MPD_GetSegmentUrl (ABR_HANDLE hMpd, HINT8 *szBuf, int nBufSize);
HUINT32		 	ABR_MPD_GetSegmentRange (ABR_HANDLE hMpd);
HUINT32			ABR_MPD_GetSegmentDuration(ABR_HANDLE hMpd);
ABR_ERR		ABR_MPD_GetInitSegUrl (ABR_HANDLE hMpd, HINT8 *szBuf, int nBufSize);
HINT32 			ABR_MPD_GetTotalPeriod (ABR_HANDLE hMpd);
HINT32 			ABR_MPD_GetTotalRepresent (ABR_HANDLE hMpd, HINT32 nPeriod);
HINT32		 	ABR_MPD_GetTotalSegment (ABR_HANDLE hMpd, HINT32 nPeriod, HINT32 nRepresent);
HUINT64 		ABR_MPD_GetCurrentBitrate (ABR_HANDLE hMpd);
HUINT64		 	ABR_MPD_GetMaxBitrate (ABR_HANDLE hMpd, HINT32 nPeriod);
HUINT64		 	ABR_MPD_GetMinBitrate (ABR_HANDLE hMpd, HINT32 nPeriod);
HINT32 			ABR_MPD_GetCurrentTotalSegment (ABR_HANDLE hMpd);
HINT32 			ABR_MPD_GetCurrentPeriodId (ABR_HANDLE hMpd);
HINT32 			ABR_MPD_GetCurrentRepresentId (ABR_HANDLE hMpd);
HINT32 			ABR_MPD_GetCurrentSegmentId (ABR_HANDLE hMpd);
HUINT32		 	ABR_MPD_GetMediaDuration (ABR_HANDLE hMpd);
HUINT64			ABR_MPD_GetCurrentBandwidth (ABR_HANDLE hMpd);
ABR_ERR 		ABR_MPD_MeasureBandwidth(ABR_HANDLE hMpd, HUINT64 *pullBandwidth);

ABR_QUEUE_t*	ABR_MPD_GetQueue(ABR_HANDLE hMpd);

#if 0 /* ykkim */
ABR_ERR	 	ABR_MPD_Queue_Reset(ABR_HANDLE hMpd, HINT64 nBase);
ABR_ERR 		ABR_MPD_Queue_PushBack(ABR_HANDLE hMpd, HINT32 nId, HINT64 sz_file, HUINT64 ullBitrate);
// ABR_ERR 		ABR_MPD_Queue_PushFront(ABR_HANDLE hMpd, HINT32 nId, HINT64 sz_file, HUINT64 ullBitrate);
ABR_ERR		ABR_MPD_Queue_Replace(ABR_HANDLE hMpd, HINT64 sz_Base, HINT32 nId, HINT64 sz_file, HUINT64 ullBitrate);
ABR_ERR		ABR_MPD_Queue_GetNodeInfo(ABR_HANDLE hMpd, const HINT64 sz_offset, HINT32 *nId, HINT64 *sz_voffset, HUINT64 *ullBitrate);
ABR_ERR 		ABR_MPD_Queue_GetCurNodeInfo(ABR_HANDLE hMpd, HINT32 *nNodeId, HINT64 *sz_voffset, HUINT64 *ullBitrate);
ABR_ERR 		ABR_MPD_Queue_GetNodeInfoBySegment(ABR_HANDLE hMpd, HINT32 nId, HINT32 *nNodeId, HINT64 *sz_voffset, HUINT64 *ullBitrate);
#endif

ABR_ERR 		ABR_MPD_SetTimeForSeek(ABR_HANDLE hMpd, HUINT64 ullTime);
HUINT64 		ABR_MPD_GetTimeForSeek(ABR_HANDLE hMpd);
ABR_ERR 		ABR_MPD_SetStreamLength(ABR_HANDLE hMpd, HINT64 nLength);
HINT64 			ABR_MPD_GetStreamLength(ABR_HANDLE hMpd);
ABR_ERR 		ABR_MPD_SetCurrentPosition(ABR_HANDLE hMpd, HINT64 nPosition);
HINT64 			ABR_MPD_GetCurrentPosition(ABR_HANDLE hMpd);


//ABR_ERR 		ABR_MPD_Lock(ABR_HANDLE hMpd);
//ABR_ERR 		ABR_MPD_Unlock(ABR_HANDLE hMpd);
/* End global function prototypes */



#ifdef  __cplusplus
}
#endif

#endif /* ! __ABR_MPDCONTROLLER_H__ */

