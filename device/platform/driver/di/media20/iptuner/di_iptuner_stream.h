/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_stream.h  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Mon May 20 10:20:48 KST 2009 $
 * File Description:	Real-Time STREAMMING IP Tuner Header File
 * Module: 		
 * Remarks:	
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef	__DI_IPTUNER_STREAM_H__
#define	__DI_IPTUNER_STREAM_H__

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
unsigned long 		s_ulStreamTunerEventID;
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
DI_ERR_CODE DI_IPTUNER_StreamOpen (DI_IPTUNER_t *pHTuner);
DI_ERR_CODE DI_IPTUNER_StreamClose (DI_IPTUNER_t *pHTuner);
DI_ERR_CODE DI_IPTUNER_StreamProbe (DI_IPTUNER_t *pHTuner);
#if 0
DI_ERR_CODE DI_IPTUNER_StreamStart (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_StreamStop (DI_IPTUNER_Index_e nTunerIndex);
#endif
DI_ERR_CODE DI_IPTUNER_StreamGetInfo (DI_IPTUNER_t *pHTuner, DI_IPTUNER_Info_t *pInfo);
DI_ERR_CODE DI_IPTUNER_StreamControl (DI_IPTUNER_t *pHTuner, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2);

DI_ERR_CODE DI_IPTUNER_StreamSendEvent (void);

#if 0 //#ifdef SUPPORT_TITLE
DI_ERR_CODE DI_IPTUNER_ICY_SreamRegisterEventCallback (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_EventCallback_t *pCallback);
void DI_IPTUNER_ICY_StreamUnregisterEventCallback (void);
#endif

/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_STREAM_H__ */


