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

#ifndef	__DI_IPTUNER_UNDERFLOW_HLS_H__
#define	__DI_IPTUNER_UNDERFLOW_HLS_H__

#ifdef	__cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */

#include "di_iptuner_config.h"
#include "di_iptuner_underflow.h"


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
HINT32 DI_IPTUNER_UnderflowHlsMonitorInitThread(DI_IPTUNER_Index_e nTunerIndex, DI_MEDIA_INFO_t *pstMediaInfo, HINT64 llFileLength);
void DI_IPTUNER_UnderflowHlsMonitorTerminateThread(void);
void DI_IPTUNER_UnderflowHlsMonitorSleepThread(void);
void DI_IPTUNER_UnderflowHlsMonitorAwakeThread(void);

DI_ERR_CODE DI_IPTUNER_SetUnderflowHlsReadyStateCallback(HUINT32 unDeviceId, fn_readystate_callback pFn);

HULONG DI_IPTUNER_UnderflowHlsGetTaskId(void);
HINT32 DI_IPTUNER_UnderflowHlsGetReadyState(void);
void DI_IPTUNER_UnderflowHlsSetReadyState(HINT32 eUnderflowState);


/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_UNDERFLOW_HLS_H__ */

