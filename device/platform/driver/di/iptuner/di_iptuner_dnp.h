/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_dnp.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Mon Apr 20 10:20:48 KST 2009 $
 * File Description:	DNP(Download & Play) IP Tuner Header File
 * Module: 		
 * Remarks:	
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DI_IPTUNER_DNP_H__
#define	__DI_IPTUNER_DNP_H__

#ifdef	__cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "di_err.h"
#include "di_media.h"

#include "di_iptuner.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */
#ifdef ENABLE_MAPPED_DNP
extern HINT32 g_nchunkoffPrdnEof;
extern HINT32 g_nchunkoffOldPrdnRequestedSeekPoint;
extern HINT32 g_nchunkoffPrdnRequestedSeekPoint;
extern HUINT8 *g_pucPrdnChunkDownloadStatusArray;

extern HBOOL g_bPrdnForwardPlay;
#if 1	/* JWKIM, workaround: for Maxdome Service */
extern HBOOL g_bPrdnPlayFFTrick;
#endif
extern HBOOL g_bPrebufferingRequested;
extern HBOOL g_bPrdnDownCompleted;
extern HINT32 g_nchunkoffIsCompletedStart; /* avoid recheck valid region */

/* for PRDN INFO */
extern HINT32 g_nchunkoffCurrentReadPoint;
extern HINT32 g_nchunkoffLastPrebufferIdx;
extern HINT32 g_nPrdnInfoReadyState;
extern HINT32 g_nPrdnInfoBufferingPercent;
extern HINT32 g_nPrdnInfoBufferingLeftTime; /* left sec that user must wait */
extern HUINT32 g_nPrdnInfoBufferingTime; /* content milisec to be buffered */
extern HINT32 g_nPrdnInfoDownloadChunkPerSecSpeed; /* chunk per sec */
extern HINT32 g_nPrdnInfoDownloadedChunkCount;
extern HINT32 g_nPrdnInfoBufferSizeInChunkCount;

#if 0 /* Moved into di_iptuner.h */
typedef void (*fn_readystate_callback)(HUINT32 deviceId, HINT32 ev, HUINT32 param1);
#endif
extern fn_readystate_callback g_pReadyStateCallbackFunc;

extern HUINT32 g_nPrdnCBulDeviceId;
extern HULONG g_nPrdnInfoLastReadyStateSentTime;
#endif
/* End Extern variable */


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
#define	TUNER_DNP_MSG_HTTP_PROBE		(1)
#define	TUNER_DNP_MSG_FTP_PROBE			(2)
#define	TUNER_DNP_MSG_RTSP_PROBE		(3)
#define	TUNER_DNP_MSG_MMS_PROBE			(4)
#define DI_IPTUNER_DNP_FILE_PATH        "/mnt/hd3/Streamer_down_file"


#ifdef ENABLE_MAPPED_DNP
/* default chunk size is 2^13 = 8KB */
/* Progress Array 1MB can handle 64GB. dual-layer DVD(8.5GB) only needs Progress Array 133KB */
#define PRDN_CHUNK_EXPONENT 				(16)

#define PRDN_GET_RAW_OFF_FROM_CHUNK_OFF(chunk_offset) 	((HINT64)(chunk_offset) << PRDN_CHUNK_EXPONENT)
#define PRDN_GET_CHUNK_OFF(raw_offset) 			((raw_offset) >> PRDN_CHUNK_EXPONENT)
/* >> 3 means    divide by 2^3=8 */
#define PRDN_GET_BYTE_OFF(chunk_offset) 			((chunk_offset) >> 3)
/* & 7(bitmask 111) means     we use only lower 3bit */
#define PRDN_GET_BIT_OFF(chunk_offset) 			((chunk_offset) & 7)

#define PRDN_ISVALID(chunk_offset) 		((*(g_pucPrdnChunkDownloadStatusArray + PRDN_GET_BYTE_OFF(chunk_offset))) & (1 << PRDN_GET_BIT_OFF(chunk_offset)))
#define PRDN_SETVALID(chunk_offset) 	*(g_pucPrdnChunkDownloadStatusArray + PRDN_GET_BYTE_OFF(chunk_offset)) |= (HUINT8)(1 << PRDN_GET_BIT_OFF(chunk_offset))


#define DNP_START_THRESHOLD			10000	/* 10000ms = 10sec */
#define DNP_DOWNLOAD_SPEED_COUNT	15
#define DNP_CANPLAY_BUFFERING_TIMEOUT	6000
#define DNP_UNDERFLOW_BUFFERING_TIMEOUT	20000

#define DNP_DOWNLOAD_SPEED_CHECK_GUARD	10

#define DNP_DOWNLOAD_SPEED_CHECK_TIME_LIMIT	1000

#endif

#define DEFAULT_PRDN_BUFFERING_SIZE ((HINT64)1024*1024*10)   /* 10MB */
#define DEFAULT_PRDN_BACKTRICK_PREBUFFERING_SIZE ((HINT64)1024*1024*1)  /* 1MB */

#define DI_IPTUNER_DNP_RETRY_COUNT_ON_NET_ERROR (30)


/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef struct
{
	HINT32 nPurpose;	  /* FOR PROBE? OR FOR NEXUS FILE PLAY? */
	HUINT32 uiMediaMaxBitrateInChunkPerSec;
	off_t		player_last_read_cursor;
	off_t		player_last_seek_cursor;
	off_t		align_buffer_start_index;
	off_t		align_buffer_end_index;
	HUINT8*		p_align_buffer;
	off_t		indexer_last_seek_cursor;
	size_t player_last_read_size;
	HBOOL bDNPNetworkError;
	HBOOL bProbeDone;
	HUINT32 uiContentDuration;
	HUINT32 unOpenFromNexusHandle;
	
} DNP_USER_DATA_t;
typedef enum {
	DI_IPTUNER_DNP_READYSTATE_BUFFERING_POINT_UPDATE = 3,
	DI_IPTUNER_DNP_READYSTATE_BUFFERING = 31,
	DI_IPTUNER_DNP_READYSTATE_BUFFERING_COMPLETED = 33,
	DI_IPTUNER_DNP_READYSTATE_FULLDOWN_COMPLETED = 34
} DI_IPTUNER_DNP_READYSTATE_e;


typedef enum {
	DI_IPTUNER_DNP_MEDIA_STATE_NO_SETUP,
	DI_IPTUNER_DNP_MEDIA_STATE_READY,
	DI_IPTUNER_DNP_MEDIA_STATE_PLAY,
	DI_IPTUNER_DNP_MEDIA_STATE_PAUSE,
	DI_IPTUNER_DNP_MEDIA_STATE_STOP,
	DI_IPTUNER_DNP_MEDIA_STATE_TRICK
} eDI_IPTUNER_DNP_MEDIA_STATE;

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
DI_ERR_CODE DI_IPTUNER_DnpOpen (DI_IPTUNER_Index_e nTunerIndex, HINT8 *pUrl, DI_MEDIA_INFO_t *pstMediaInfo);
DI_ERR_CODE DI_IPTUNER_DnpClose (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_DnpStart (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_DnpStop (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_DnpGetInfo (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_Info_t *pInfo);
DI_ERR_CODE DI_IPTUNER_DnpControl (DI_IPTUNER_Index_e nTunerIndex,  DI_IPTUNER_CMD_e cmd, HUINT64 val);
DI_ERR_CODE DI_IPTUNER_DnpResume (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_DnpPause (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_DnpSendEvent (void);
/* TODO :: 2G 이상 file 지원 - off_t를 써야 함. */ 
DI_ERR_CODE DI_IPTUNER_DnpPutFileLength (HINT64 nFileLength);
DI_ERR_CODE DI_IPTUNER_DnpSetReadyStateCallback(HUINT32 ulDeviceId, fn_readystate_callback pFn);
DI_ERR_CODE DI_IPTUNER_DnpInitDownloadSpeed(HULONG *pulStartClock);
DI_ERR_CODE DI_IPTUNER_DnpUpdateDownloadSpeed(HULONG ulStartClock, HINT32 nDownloadBytes, HUINT32 ulLoopCount);
HINT32 DI_IPTUNER_DnpGetDownloadBytesPerSecond(void);
HUINT64 DI_IPTUNER_DnpGetCurrentDataReadOffset(void);
HUINT64 DI_IPTUNER_DnpGetLastReadSize(void);

DI_ERR_CODE DI_IPTUNER_DnpGetBufferedDataSize(HUINT64 *pullBufferedData, HBOOL *bIsFullChunk, HINT64 llFileLength);

/**
 * @brief			Buffering Information(Buffering Percent, Left time)을 구하는 함수
 *
 * @param[In]		unDuration			Media Duration
 * @return			DI_ERR_CODE
 * @date 			2013/05/13
 * @author 			thoh@humaxdigital.com
 * @see 			
 */
DI_ERR_CODE DI_IPTUNER_DnpCalcBufferingInfo(HUINT32 unDuration);

void DI_IPTUNER_DnpUnderflowMonitorAwake(void);
void DI_IPTUNER_DnpUnderflowMonitorSleep(void);	


#ifdef ENABLE_MAPPED_DNP
HBOOL PRDN_IsRangeValid(HINT32 chunk_offset, HINT32 requested_chunk_count);
HBOOL PRDN_IsCompleted(void);
#endif
/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_DNP_H__ */


