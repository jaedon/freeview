/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_adaptive.h  $
 * Version:			$Revision:   $
 * Original Author:	Young Ki Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	     Adaptive Streaming Tuner Header
 * Module:
 * Remarks:	
 */	

/*******************************************************************/ 	
/* Copyright (c) 2011 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


#ifndef	__DI_IPTUNER_ADAPTIVE_H__
#define	__DI_IPTUNER_ADAPTIVE_H__

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
#include "di_media20.h"

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
#define	TUNER_ADAPTIVE_STACK_SIZE			(256*1024)
#define	TUNER_ADAPTIVE_MSG_QUEUE_SIZE		(10)

/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef enum
{
	TUNER_ADAPTIVE_MSG_SSTR_BUFFERING = 1,
	TUNER_ADAPTIVE_MSG_HLS_BUFFERING,
	TUNER_ADAPTIVE_MSG_STOP_BUFFERING
} TUNER_ADAPTIVE_MSG_e;

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */
DI_ERR_CODE DI_IPTUNER_AdaptiveOpen (DI_IPTUNER_t *pHTuner);
DI_ERR_CODE DI_IPTUNER_AdaptiveClose (DI_IPTUNER_t *pHTuner);
DI_ERR_CODE DI_IPTUNER_AdaptiveProbe (DI_IPTUNER_t *pHTuner);
DI_ERR_CODE DI_IPTUNER_AdaptiveDestroy (DI_IPTUNER_t *pHTuner);
DI_ERR_CODE DI_IPTUNER_AdaptiveGetInfo (DI_IPTUNER_t *pHTuner, DI_IPTUNER_Info_t *pInfo);
DI_ERR_CODE DI_IPTUNER_AdaptiveControl (DI_IPTUNER_t *pHTuner, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2);
DI_ERR_CODE DI_IPTUNER_AdaptiveSetSettings (DI_IPTUNER_t *pHTuner, DI_IPTUNER_AdaptiveSettings_t *pAdaptiveSettings);
DI_ERR_CODE DI_IPTUNER_AdaptiveGetSettings (DI_IPTUNER_t *pHTuner, DI_IPTUNER_AdaptiveSettings_t *pAdaptiveSettings);

/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_STREAM_H__ */
