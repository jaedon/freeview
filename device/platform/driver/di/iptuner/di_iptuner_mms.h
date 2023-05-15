/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_iptuner_mms.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  Fri May 20 03:08:21 KST 2009 $
 * File Description:	     DI Stream (MMS) Header
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

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
#include "vfio.h"
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

/** MMS VFIO Functions (for DNP or STREAM) */
int DI_STREAM_MMS_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle );
int DI_STREAM_MMS_Close (VFIO_Handle_t tHandle);
int DI_STREAM_MMS_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize);
int DI_STREAM_MMS_Seek (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_MMS_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset);
int DI_STREAM_MMS_IsEOF (VFIO_Handle_t tHandle);
int DI_STREAM_MMS_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle);
off_t DI_STREAM_MMS_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_MMS_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size);

/** MMS DNP Control Functions */
void DI_STREAM_MMS_DnpStartReceiving (void);
void DI_STREAM_MMS_DnpStopReceiving (void);
void DI_STREAM_MMS_DnpPauseReceiving (void);
void DI_STREAM_MMS_DnpResumeReceiving (void);
int DI_STREAM_MMS_DnpReceive (VFIO_Handle_t tHandle);

DI_ERR_CODE DI_STREAM_MMS_Probe (const HINT8 *path,  DI_MEDIA_INFO_t *pstMediaInfo);
DI_ERR_CODE DI_STREAM_MMS_Control ( DI_IPTUNER_CMD_e cmd, HUINT64 val , HUINT64 val2);

/** MMS STREAM VFIO Register/Unregister */
void DI_STREAM_MMS_RegisterCallback (void);
void DI_STREAM_MMS_UnregisterCallback (void);
#if STREAM_BUFFERING
void STREAM_MMS_Buffering (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_STREAM_MMS_RegisterBufferingCallback (DI_IPTUNER_PlayCallback_t *pCallback);
void DI_STREAM_MMS_UnregisterBufferingCallback (void);
DI_ERR_CODE DI_STREAM_MMS_CreateBuffer (DI_MEDIA_INFO_t *pstMediaInfo);
DI_ERR_CODE DI_STREAM_MMS_FreeBuffer (void);
#endif

/* End global function prototypes */
#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_IPTUNER_MMS_H__ */
