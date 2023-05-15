/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_underflow.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	Byung-min Ha $
 * Current Author:	$Author: bmha@humaxdigital.com $
 * Date:			$Date: Fri November 1 17:40:00 KST 2013 $
 * File Description:	Check underflow	state
 * Module: 			    IPTUNER Underflow Routine
 * Remarks:
 */


/*******************************************************************/
/* Copyright (c) 2013 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DI_IPTUNER_UNDERFLOW_H__
#define	__DI_IPTUNER_UNDERFLOW_H__

#ifdef	__cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "htype.h"
#include "di_err.h"
#include "vfio20.h"
#include "vkernel.h"
#include "taskdef.h"

#include "di_iptuner_config.h"
#include "di_media20.h"
#include "di_iptuner20.h"
#include "drv_video.h"
#include "drv_audio.h"

#include "nexus_video_decoder.h"
#include "nexus_audio_decoder.h"

#include "stream_global.h"

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
#define TUNER_UNDERFLOW_MSG_QUEUE_SIZE 	(10)
#define UNDERFLOW_MONITOR_STACK_SIZE	(32*1024)
#define BUFFERING_TIMEOUT				(10000)
/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef enum
{
	DI_IPTUNER_UnderflowMonitor_Msg_Start_Task = 0,
	DI_IPTUNER_UnderflowMonitor_Msg_Pause_Task,
	DI_IPTUNER_UnderflowMonitor_Msg_Stop_Task,
} DI_IPTUNER_UnderflowMonitor_Msg_e;

typedef enum {
	DI_IPTUNER_UnderflowMonitorStatus_None = 0,
	DI_IPTUNER_UnderflowMonitorStatus_Init,
	DI_IPTUNER_UnderflowMonitorStatus_Start,
	DI_IPTUNER_UnderflowMonitorStatus_Pause,
	DI_IPTUNER_UnderflowMonitorStatus_Stop,
	DI_IPTUNER_UnderflowMonitorStatus_MAX
} DI_IPTUNER_UnderflowMonitorStatus_e;

typedef enum
{
	DI_IPTUNER_Underflow_Threshold_Mode_None,
	DI_IPTUNER_Underflow_Threshold_Mode_Auto,
} DI_IPTUNER_Underflow_Threshold_Mode_e;


typedef struct
{
	HUINT32	unUnderflowCount;
	HUINT32	unEnoughCount;
	HUINT64	ulTotalPausedTime;

	HUINT32 unMaxBufferSize;
	HUINT32	unPrevBufferedSize;
	HUINT32	unSameSizeCount;

	HUINT32 unSizeofOneSec;
	HUINT32 unUnderflowThreshold;
	HUINT32 unEnoughThreshold;

	HULONG ulBufferingTimeout;
	HULONG ulStartBufferingClock;

	DI_IPTUNER_Underflow_Threshold_Mode_e	eThresholdMode;
	DI_IPTUNER_UnderflowMonitorStatus_e 	eUnderflowMonitorStatus;
}DI_IPTUNER_UNDERFLOW_INFO_t;

typedef struct
{
	DI_IPTUNER_UNDERFLOW_INFO_t tUnderflowInfo;

	VFIO_Handle_t 				*ptHandle;
	DI_IPTUNER_t				*pHTuner;
	DI_MEDIA_INFO_t				*pMediaInfo;
	STREAM_ACCESS_t 			*pAccess;

	NEXUS_VideoDecoderHandle	vDecoder;
	NEXUS_AudioDecoderHandle	aDecoder;

	HULONG 						ulUnderflowMonitorMsgQId;
	HULONG 						ulUnderflowMonitorTaskId;
} DI_IPTUNER_UNDERFLOW_t;

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
HINT32 DI_IPTUNER_UnderflowMonitorInitThread(VFIO_Handle_t *ptHandle);
void DI_IPTUNER_UnderflowMonitorTerminateThread(VFIO_Handle_t *ptHandle);
/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_UNDERFLOW_H__ */

