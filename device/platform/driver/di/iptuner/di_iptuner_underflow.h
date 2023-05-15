/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_underflow.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	Tae-hoon Oh $
 * Current Author:	$Author: thoh@humaxdigital.com $
 * Date:			$Date: Fri March 29 15:12:48 KST 2013 $
 * File Description:	Check underflow	state Header File
 * Module: 			    
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

#include "di_iptuner_config.h"

#include "htype.h"
#include "di_err.h"
#include "di_media.h"

#include "di_iptuner.h"
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
#define CANPLAY_TIMEOUT					(6000)
#define BUFFERING_TIMEOUT				(20000)
#define START_THRESHOLD					(10000)
#define CHECK_UNDERFLOW_PERCENT_VALUE	(15)
#define CHECK_UNDERFLOW_SECOND_VALUE	(8)
#define CHECK_RESUME_SECOND_VALUE		(5)
/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef enum
{
	UNDERFLOW_MONITOR_MSG_UNDERFLOW = 1,
	UNDERFLOW_MONITOR_MSG_STOP_TASK,
	UNDERFLOW_MONITOR_MSG_SLEEP_TASK,
	UNDERFLOW_MONITOR_MSG_AWAKE_TASK,
} PLAYPUMP_UNDERFLOW_MONITOR_MSG_e;

typedef struct
{
	HINT64	FileLength;
	HUINT32	MaxBitrate;
	HUINT32	Duration;
	DI_IPTUNER_Index_e TunerIndex;
} DI_IPTUNER_UNDERFLOW_MEDIAINFO_t;

typedef enum {
	DI_IPTUNER_UNDERFLOW_READYSTATE_CANPLAY,
	DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING = 31,
	DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED = 33
} DI_IPTUNER_UNDERFLOW_READYSTATE_e;

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
HINT32 DI_IPTUNER_UnderflowMonitorInitThread(DI_IPTUNER_Index_e nTunerIndex, DI_MEDIA_INFO_t *pstMediaInfo, HINT64 llFileLength);
void DI_IPTUNER_UnderflowMonitorTerminateThread(void);
void DI_IPTUNER_UnderflowMonitorSleepThread(void);
void DI_IPTUNER_UnderflowMonitorAwakeThread(void);

DI_ERR_CODE DI_IPTUNER_SetUnderflowReadyStateCallback(HUINT32 unDeviceId, fn_readystate_callback pFn);
HINT32 DI_IPTUNER_UnderflowGetReadyState(void);
void DI_IPTUNER_UnderflowSetReadyState(HINT32 eUnderflowState);
/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_UNDERFLOW_H__ */

