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

/**
 * @ingroup DI_IPTUNER_DNP
 */

/**
 * @author Sunwoo Lee(swlee2@humaxdigital.com)
 * @date 02 Nov 2012
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file di_iptuner_dnp.h
 */

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
#include "di_media20.h"

#include "di_iptuner20.h"
#include "di_iptuner_config.h"
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
/** Download Triggering Message */
#define	IPTUNER_DOWNLOAD_TRIGGER_HTTP		(1)
#define	IPTUNER_DOWNLOAD_TRIGGER_FTP		(2)
#define	IPTUNER_DOWNLOAD_TRIGGER_RTSP		(3)
#define	IPTUNER_DOWNLOAD_TRIGGER_MMS		(4)

#define DI_IPTUNER_DNP_RETRY_COUNT (10) /* times */
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

/*! 
 *	\brief Create tasks, semaphores and messages.
 *	\param pHTuner: tuner handle
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpCreate (DI_IPTUNER_t *pHTuner);

/*! 
 *	\brief Destroy tasks, semaphores and messages.
 *	\param pHTuner: tuner handle
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpDestroy (DI_IPTUNER_t *pHTuner);

/*! 
 *	\brief Register VFIO functions.
 *	\param pHTuner: tuner handle
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpOpen (DI_IPTUNER_t *pHTuner);

/*! 
 *	\brief Unregister VFIO functions.
 *	\param pHTuner: tuner handle
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpClose (DI_IPTUNER_t *pHTuner);

/*! 
 *	\brief Pause to download contents.
 *	\param pHTuner: tuner handle
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpPause (DI_IPTUNER_t *pHTuner);

/*! 
 *	\brief Resume to download contents.
 *	\param pHTuner: tuner handle
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpResume (DI_IPTUNER_t *pHTuner);

/*! 
 *	\brief Stop downloading contents.
 *	\param pHTuner: tuner handle
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpStop (DI_IPTUNER_t *pHTuner);

/*! 
 *	\brief Return Iptuner information related to downloading. [Not being used]
 *	\param pHTuner: tuner handle, pInfo: information related to downloading
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpGetInfo (DI_IPTUNER_t *pHTuner, DI_IPTUNER_Info_t *pInfo);

/*! 
 *	\brief Control the dnp function [Not being used]
 *	\param pHTuner: tuner handle, cmd: command enum, val: seek position or trick speed
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpControl (DI_IPTUNER_t *pHTuner,  DI_IPTUNER_CMD_e cmd, HUINT64 val);

/*! 
 *	\brief Get buffering information.
 *	\param pHTuner: tuner handle, nHiddenPumpBuf: The amount of data in playpump buffer, nHiddenDecBuf: The amount of data in decoder buffer, pStatus: current buffering information
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpGetBufferStatus (DI_IPTUNER_t *pHTuner,HUINT32 nHiddenPumpBuf, HUINT32 nHiddenDecBuf,  DI_IPTUNER_Buffer_t *pStatus);

/*! 
 *	\brief Moov the buffering offset
 *	\param pHTuner: tuner handle, llOffset: target offset to move
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpUpdateBufferingPosition(DI_IPTUNER_t *pHTuner, HINT64 llOffset);

/*! 
 *	\brief Convert the time value to the byte offset.
 *	\param pHTuner: tuner handle, ulTime: time offset pEndByte: converted byte offset
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpConvertTimeToByteOffset(DI_IPTUNER_t *pHTuner, HUINT32 ulTime , HUINT64 *pEndByte);

/*! 
 *	\brief Get the current system time and keep it into the tuner handle.
 *	\param pHTuner: tuner handle
 *	\return void
 *	\see 
 */
void DI_IPTUNER_DnpRegisterStartTimeMsec (DI_IPTUNER_t *pHTuner);


/*! 
 *	\brief Remove the time value which is kept in tuner handle.
 *	\param pHTuner: tuner handle
 *	\return void
 *	\see 
 */
void DI_IPTUNER_DnpUnRegisterStartTimeMsec (DI_IPTUNER_t *pHTuner);

/*! 
 *	\brief Check if the designated area is downloaded or not.
 *	\param pHTuner: tuner handle, ullStart: Start offset of the target area, ullEnd: End offset of the target area.
 *	\return True if the target area is downloaded.
 *	\see 
 */
HBOOL DI_IPTUNER_DnpCheckBufferRegion (DI_IPTUNER_t *pHTuner, HUINT64 ullStart, HUINT64 ullEnd);

/*! 
 *	\brief Force the stucked read function to return.
 *	\param pHTuner: tuner handle
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpCancelBlockedRead  (DI_IPTUNER_t *pHTuner);

/*! 
 *	\brief Set the end time offset to be downloaded.
 *	\param pHTuner: tuner handle, ulOffset: the target offset.
 *	\return True if the time setting is ok.
 *	\see 
 */
HBOOL DI_IPTUNER_DnpSetOLTEndTime (DI_IPTUNER_t *pHTuner, HUINT64 ulOffset);

/*! 
 *	\brief Return the downloaded chunk information.
 *	\param pHTuner: tuner handle, uiChunks: The number of chunks, iStartOffsets: the array of the start offsets of all chunks, iEndOffsets: the array of the end offsets of all chunks.
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_IPTUNER_DnpGetBufferInfo(DI_IPTUNER_t *pHTuner, HUINT32 *uiChunks, HINT32 **iStartOffsets, HINT32 **iEndOffsets);
DI_ERR_CODE DI_IPTUNER_DnpSetCurrentPlayTime(DI_IPTUNER_t *pHTuner, HUINT32 uiTime);
DI_ERR_CODE DI_IPTUNER_DnpCheckUnderrun(DI_IPTUNER_t *pHTuner);

/* End global function prototypes */


#ifdef	__cplusplus
}
#endif

#endif /* !__DI_IPTUNER_DNP_H__ */


