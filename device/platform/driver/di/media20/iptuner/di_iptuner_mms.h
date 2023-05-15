/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_mms.h  $
 * Version:			$Revision:   1.0  $
 * Original Author: $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: 2012.07.30
 * File Description:	DI Stream (MMS) Header
 * Module:
 * Remarks:
 */	

/**
 * @ingroup MMS_VFIO_CONTROLLER
 */

/**
 * @author Jin Lee(jlee@humaxdigital.com)
 * @date 30 Jul 2012
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
 * @file di_iptuner_mms.h
 */

#ifndef __DI_IPTUNER_MMS_H__
#define __DI_IPTUNER_MMS_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "di_iptuner_config.h"
#include "htype.h"
#include "vfio20.h"
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

/*! 
 *	\brief Open MMS probe connection
 *	\param path: URL path, mode: open mode(unused), pHTuner: tuner handle
 *	\return int: VFIO_ERROR_SUCCESS if success
 *	\see 
 */
int DI_STREAM_MMS_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle );

/*! 
 *	\brief Close MMS connection
 *	\param tHandle: VFIO_Handle_t
 *	\return int: VFIO_ERROR_SUCCESS if success
 *	\see 
 */
int DI_STREAM_MMS_Close (VFIO_Handle_t tHandle);

/*! 
 *	\brief Read for MMS probe and nexus playback
 *	\param tHandle: VFIO_Handle_t, ptr: allocated buffer, size: size_t, nmemb: size_t, ptReadSize: return size(have ssize_t type)
 *	\return int
 *	\see 
 */
int DI_STREAM_MMS_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize);

/*! 
 *	\brief Position Seek for MMS probe
 *	\param tHandle: VFIO_Handle_t, offset: seek position, whence: whence mode for seek
 *	\return int
 *	\see 
 */
int DI_STREAM_MMS_Seek (VFIO_Handle_t tHandle, off_t offset, int whence);

/*! 
 *	\brief Current Position Tell function for MMS probe and nexus playback
 *	\param tHandle: VFIO_Handle_t, plCurOffset: current position offset for return
 *	\return int
 *	\see 
 */
int DI_STREAM_MMS_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset);

/*! 
 *	\brief Check stream endness
 *	\param tHandle: VFIO_Handle_t
 *	\return int
 *	\see 
 */
int DI_STREAM_MMS_IsEOF (VFIO_Handle_t tHandle);

/*! 
 *	\brief Open connection for nexus playback
 *	\param path: URL path, flags: open mode(unused), pHTuner: tuner handle
 *	\return int: VFIO_ERROR_SUCCESS if success
 *	\see 
 */
int DI_STREAM_MMS_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle);

/*! 
 *	\brief Position Seek for nexus playback
 *	\param tHandle: VFIO_Handle_t, offset: seek position, whence: whence mode for seek
 *	\return int
 *	\see 
 */
off_t DI_STREAM_MMS_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence);

/*! 
 *	\brief Get current received stream size for nexus playback
 *	\param tHandle: VFIO_Handle_t
 *	\return int
 *	\see 
 */
int DI_STREAM_MMS_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size);

/*! 
 *	\brief Probe media streaming
 *	\param path: URL path, pstMediaInfo: probed media info has DI_MEDIA_INFO_t type
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_STREAM_MMS_Probe (const HINT8 *path,  DI_MEDIA_INFO_t *pstMediaInfo);

/*! 
 *	\brief Control for nexus playback
 *	\param cmd: DI_IPTUNER_CMD_e
 *	\return DI_ERR_CODE
 *	\see 
 */
DI_ERR_CODE DI_STREAM_MMS_Control ( DI_IPTUNER_CMD_e cmd, HUINT64 val , HUINT64 val2);

/*! 
 *	\brief Register MMS VFIO functions.
 *	\param pUrl: URL path
 *	\return void
 *	\see 
 */
void DI_STREAM_MMS_RegisterCallback (const char *pUrl);

/*! 
 *	\brief Unregister MMS VFIO functions.
 *	\param pUrl: URL path
 *	\return void
 *	\see 
 */
void DI_STREAM_MMS_UnregisterCallback (const char *pUrl);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_IPTUNER_MMS_H__ */