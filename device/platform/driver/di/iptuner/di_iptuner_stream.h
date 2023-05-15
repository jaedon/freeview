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
//static HINT32					s_nFileLength = 0;
//#define	TUNER_STREAM_STACK_SIZE			(32*1024)
#define	TUNER_STREAM_STACK_SIZE			(256*1024)
#define	TUNER_STREAM_TASK_PRIORITY			(10)
#define	TUNER_STREAM_MSG_QUEUE_SIZE		(10)
#define	TUNER_STREAM_WAITEVTTIME			(10000)		/* 3000msec = 3sec */
#define	TUNER_STREAM_CALLBACK_WAITCOUNT	(20)		/* 20 * 100msec = 2sec */
#define	TUNER_STREAM_CALLBACK_WAITTTIME	(100)		/* 100msec = 0.1sec */

typedef enum
{
	TUNER_STREAM_MSG_HTTP_BUFFERING = 1,
	TUNER_STREAM_MSG_MMS_BUFFERING,
	TUNER_STREAM_MSG_SSTR_BUFFERING,
	TUNER_STREAM_MSG_HLS_BUFFERING,
	TUNER_STREAM_MSG_HTTP_CHNK_BUFFERING, /* HTTP chunked live streaming */
	TUNER_STREAM_MSG_STOP_BUFFERING
} TUNER_STREAM_MSG_e;

extern HULONG s_ulStreamMsgQId;
extern HULONG s_ulBufSema;

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
DI_ERR_CODE DI_IPTUNER_StreamOpen (DI_IPTUNER_Index_e nTunerIndex, HINT8 *pUrl, DI_MEDIA_INFO_t *pstMediaInfo);
DI_ERR_CODE DI_IPTUNER_StreamClose (DI_IPTUNER_Index_e nTunerIndex);
#if 0
DI_ERR_CODE DI_IPTUNER_StreamStart (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_IPTUNER_StreamStop (DI_IPTUNER_Index_e nTunerIndex);
#endif
DI_ERR_CODE DI_IPTUNER_StreamGetInfo (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_Info_t *pInfo);
DI_ERR_CODE DI_IPTUNER_StreamControl (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2);
DI_ERR_CODE DI_IPTUNER_StreamSendEvent (void);
DI_ERR_CODE DI_IPTUNER_StreamRegisterCallback (DI_IPTUNER_Index_e nTunerIndex ,DI_IPTUNER_PlayCallback_t *pCallback);
void DI_IPTUNER_StreamUnregisterCallback (DI_IPTUNER_Index_e nTunerIndex);

#ifdef SUPPORT_TITLE
DI_ERR_CODE DI_IPTUNER_ICY_SreamRegisterEventCallback (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_EventCallback_t *pCallback);
void DI_IPTUNER_ICY_StreamUnregisterEventCallback (void);
#endif

/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_STREAM_H__ */


