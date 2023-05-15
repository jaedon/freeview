/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_cls_monitor.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	Tae-hoon Oh $
 * Current Author:	$Author: thoh@humaxdigital.com $
 * Date:			$Date: Fri March 29 15:12:48 KST 2013 $
 * File Description:	underflow monitor for HTTP chunked live streaming (CLS)
 * Module: 			    
 * Remarks:
 */


/*******************************************************************/
/* Copyright (c) 2013 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DI_IPTUNER_CLS_MONITOR_H__
#define	__DI_IPTUNER_CLS_MONITOR_H__

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
#include "di_iptuner20.h"
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
/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
HINT32 DI_IPTUNER_CLS_MonitorStart(DI_IPTUNER_t *pHTuner);
void DI_IPTUNER_CLS_MonitorStop(DI_IPTUNER_t *pHTuner);
void DI_IPTUNER_CLS_SetLiveBufferState(DI_IPTUNER_t *pHTuner, DI_IPTUNER_CLS_BUFSTATE_e eBufferState);
/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_CLS_MONITOR_H__ */

