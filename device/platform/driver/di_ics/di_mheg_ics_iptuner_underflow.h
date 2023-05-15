/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   mheg_ics_iptuner_underflow.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	JGKim(Media Client) $
 * Current Author:	kimjg@humaxdigital.com $
 * Date:			February 2014
 * File Description:	Check underflow	state Header File
 * Module: 			    
 * Remarks:
 */


/*******************************************************************/
/* Copyright (c) 2013 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__MHEG_ICS_IPTUNER_UNDERFLOW_H__
#define	__MHEG_ICS_IPTUNER_UNDERFLOW_H__

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

//#include "di_iptuner.h"
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
#define MHEG_ICS_IPTUNER_UNDERFLOW_MSG_QUEUE_SIZE 	(10)
#define MHEG_ICS_UNDERFLOW_MONITOR_STACK_SIZE	(32*1024)
#define MHEG_ICS_CANPLAY_TIMEOUT					(6000)
#define MHEG_ICS_BUFFERING_TIMEOUT				(20000)
#define MHEG_ICS_START_THRESHOLD					(10000)
#define MHEG_ICS_CHECK_UNDERFLOW_PERCENT_VALUE	(15) //(15) /* for ICS003 Scene2 */
#define MHEG_ICS_CHECK_UNDERFLOW_SECOND_VALUE	(8)
#define MHEG_ICS_CHECK_RESUME_SECOND_VALUE		(3)
/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef enum
{
	MHEG_ICS_UNDERFLOW_MONITOR_MSG_UNDERFLOW = 1,
	MHEG_ICS_UNDERFLOW_MONITOR_MSG_STOP_TASK,
	MHEG_ICS_UNDERFLOW_MONITOR_MSG_SLEEP_TASK,
	MHEG_ICS_UNDERFLOW_MONITOR_MSG_AWAKE_TASK,
} MHEG_ICS_UNDERFLOW_MONITOR_MSG_e;

typedef struct
{
	HINT64	FileLength;
	HUINT32	MaxBitrate;
	HUINT32	Duration;
	//HUINT32 ulHandle;
} MHEG_ICS_IPTUNER_UNDERFLOW_MEDIAINFO_t;

typedef enum {
	MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_CANPLAY,
	MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING = 31,
	MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED = 33
} MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_e;

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
HINT32 		DI_MHEG_ICS_IPTUNER_UnderflowMonitorInitThread(HUINT32 ulHandle, DI_MEDIA_INFO_t *pstMediaInfo, HINT64 llFileLength);
void 		DI_MHEG_ICS_IPTUNER_UnderflowMonitorTerminateThread(HUINT32 ulHandle);
//void 		DI_MHEG_ICS_IPTUNER_UnderflowMonitorSleepThread(HUINT32 ulHandle);
//void 		DI_MHEG_ICS_IPTUNER_UnderflowMonitorAwakeThread(HUINT32 ulHandle);

DI_ERR_CODE DI_MHEG_ICS_IPTUNER_SetUnderflowReadyStateCallback(HUINT32 ulHandle, fn_readystate_callback pFn);
HINT32 		DI_MHEG_ICS_IPTUNER_UnderflowGetReadyState(HUINT32 ulHandle);
void 		DI_MHEG_ICS_IPTUNER_UnderflowSetReadyState(HUINT32 ulHandle, HINT32 eUnderflowState);
/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__MHEG_ICS_IPTUNER_UNDERFLOW_H__ */

