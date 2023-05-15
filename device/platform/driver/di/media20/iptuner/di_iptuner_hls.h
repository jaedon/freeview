/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_hls.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:		Yang Hyun Uk, YoungKi Kim $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2011.08.06
 * File Description:	HTTP Live Streaming
 * Module:
 * Remarks:
 */

/**
 * @defgroup HLS_IPTUNER HLS IPTUNER
 * @ingroup HLS
 */

/**
 * @author Hyunuk Yang(huyang@humaxdigital.com) YoungKi Kim(ykkim3@humaxdigital.com)
 * @date 6 Auguest 2011
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
 * @file di_iptuner_hls.h
 */

#ifndef __DI_STREAM_HLS_H__
#define __DI_STREAM_HLS_H__

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
#include "htype.h"
#include "vfio20.h"
#include "di_iptuner_config.h"
#include "hls_controller.h"
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
#define HLS_CONFIG_MEMORY_PARSE 1

#define HLS_CONST_NO_BUFFER 0
#define HLS_CONST_MEMORY_BUFFER 1
#define HLS_CONST_FILE_BUFFER 2
#define HLS_CONST_DEFAULT_SEGBUFF (1024 * 1024 * 3)
#define HLS_CONST_DEFAULT_DMABLOCK (188 * 4096) /* 0.77 MB * 2 = 1.54 MB */
#define HLS_CONFIG_TEMP_CACHE "/tmp/hls-cache"
#define HLS_CONST_HTTP_RECV_BUFFSIZE (2048*3) /* 6kb */
#define HLS_CONST_RECV_BLOCKSIZE (60*1024) /* 6kb */
#define HLS_CONST_WAITING (100)
#define HLS_CONST_DEFAULT_UPDATEDELAY (4000)
#define HLS_CONST_DEFAULT_RETRY_UPDATEDELAY (1000)

#define HLS_CONST_HIGHEST_BUFFERING_THRESHOLD 90 / 100
#define HLS_CONST_LOWEST_BUFFERING_THRESHOLD 10 / 100

#ifdef DI_IPTUNER_URL_LENGTH
#define HLS_CONST_URL_LENGTH DI_IPTUNER_URL_LENGTH
#else
#define HLS_CONST_URL_LENGTH (512)
#endif
#define HLS_CONST_HTTP_BUFFERING_BLOCK (1024*8) // 8Kbytes
#define HLS_CONST_RETRY_COUNT_ON_NET_ERROR (3)

typedef enum {
	DI_IPTUNER_HLS_BUFFERING_TASK_STATE_READY,
	DI_IPTUNER_HLS_BUFFERING_TASK_STATE_STARTED,
	DI_IPTUNER_HLS_BUFFERING_TASK_STATE_WAITING,
	DI_IPTUNER_HLS_BUFFERING_TASK_STATE_KILLING,
	DI_IPTUNER_HLS_BUFFERING_TASK_STATE_DONE
} DI_IPTUNER_HLS_BUFFERING_TASK_STATE_e;

/**
 * HLS LIVE UPDATE TASK의 State
 */
typedef enum {
	DI_IPTUNER_HLS_LIVE_UPDATE_READY, /**< READY 상태, Default  */
	DI_IPTUNER_HLS_LIVE_UPDATE_STARTED, /**< STARTED 상태, Update Task 시작  */
	DI_IPTUNER_HLS_LIVE_UPDATE_TRIGGERED, /**< TRIGGER 상태, Update 준비  */
	DI_IPTUNER_HLS_LIVE_UPDATE_UPDATING, /**< UPDATING 상태, Update 중  */
	DI_IPTUNER_HLS_LIVE_UPDATE_TERMINATED, /**< TERMINATED 상태, Update 종료 */
} DI_IPTUNER_HLS_LIVE_UPDATE_e;


typedef struct timeval HLS_TIMEVAL_t;

/** Seconds for Buffering */
#ifndef STREAM_HLS_BUFFERING_SEGMENTS
#define HLS_CONFIG_BUFFERING_SEGMENTS (2)
#else
#define HLS_CONFIG_BUFFERING_SEGMENTS STREAM_HLS_BUFFERING_SEGMENTS
#endif
/** Maximum BufferSize */
#ifndef STREAM_HLS_MAX_BUFFER
#define HLS_CONFIG_MAX_BUFFER (10*1024*1024)
#else
#define HLS_CONFIG_MAX_BUFFER (STREAM_HLS_MAX_BUFFER * 1024 * 1024)
#endif
/** Minimum BufferSize */
#ifndef STREAM_HLS_MIN_BUFFER
#define HLS_CONFIG_MIN_BUFFER (2*1024*1024)
#else
#define HLS_CONFIG_MIN_BUFFER (STREAM_HLS_MIN_BUFFER * 1024 * 1024)
#endif
/* endif USE_IPTUNER_HLS */

/* End Macro definition */



/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef void* HLS_HANDLE;

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/* End global function prototypes */

HINT8* 		DI_STREAM_HLS_InitSegUrl(HLS_HANDLE hHls);
HLS_ERROR_e	DI_STREAM_HLS_GetMediaInfo (HLS_HANDLE hHls,DI_MEDIA_INFO_t *pstMediaInfo);

HUINT32		DI_STREAM_HLS_GetBufferingState(void);
void		DI_STREAM_HLS_SetBufferingState(HUINT32 nState);

void 		DI_STREAM_HLS_RegisterCallback (const char *pUrl);
void 		DI_STREAM_HLS_UnregisterCallback (const char *pUrl);

HLS_ERROR_e DI_STREAM_HLS_SetTimeForSeek(HLS_HANDLE hHls, HUINT64 ullTime);

DI_ERR_CODE DI_STREAM_HLS_Control (HLS_HANDLE hHls, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 );

HLS_ERROR_e DI_STREAM_HLS_SetSettings(HLS_HANDLE hHls, DI_IPTUNER_AdaptiveSettings_t *pSettings);
HLS_ERROR_e DI_STREAM_HLS_GetSettings(HLS_HANDLE hHls, DI_IPTUNER_AdaptiveSettings_t *pSettings);

HLS_HANDLE 	DI_STREAM_HLS_OpenController(HINT8 * pUrl);
int 		DI_STREAM_HLS_CloseController(HLS_HANDLE hHls);

int 		DI_STREAM_HLS_CreateLiveUpdateTask (SESSION_HANDLE pHSession);
int 		DI_STREAM_HLS_DestroyLiveUpdateTask (SESSION_HANDLE pHSession);

int 		DI_STREAM_HLS_PreBuffering (SESSION_HANDLE pHSesssion);

#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_STREAM_HLS_H__ */



/*@}*/
