/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_iptuner_http_chnk.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  Fri Apr 17 03:08:21 KST 2009 $
 * File Description:	     DI Stream (HTTP) Header
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_IPTUNER_HTTP_CHNK_H__
#define __DI_IPTUNER_HTTP_CHNK_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "vfio.h"
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
int DI_STREAM_HTTP_CHNK_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle );
int DI_STREAM_HTTP_CHNK_Close (VFIO_Handle_t tHandle);
int DI_STREAM_HTTP_CHNK_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize);
int DI_STREAM_HTTP_CHNK_Seek (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_HTTP_CHNK_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset);
int DI_STREAM_HTTP_CHNK_IsEOF (VFIO_Handle_t tHandle);
int DI_STREAM_HTTP_CHNK_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle);
off_t DI_STREAM_HTTP_CHNK_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_HTTP_CHNK_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size);
int DI_STREAM_HTTP_CHNK_PreBuffering (DI_IPTUNER_Index_e nTunerIndex);

/*DI_ERR_CODE DI_STREAM_HTTP_CHNK_Control ( DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 );*/

void DI_STREAM_HTTP_CHNK_RegisterCallback (void);
void DI_STREAM_HTTP_CHNK_UnregisterCallback (void);

/* STREAM_BUFFERING */
void DI_STREAM_HTTP_CHNK_Buffering (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_STREAM_HTTP_CHNK_RegisterBufferingCallback (DI_IPTUNER_PlayCallback_t *pCallback);
void DI_STREAM_HTTP_CHNK_UnregisterBufferingCallback (void);
DI_ERR_CODE DI_STREAM_HTTP_CHNK_CreateBuffer (DI_MEDIA_INFO_t *pstMediaInfo);
DI_ERR_CODE DI_STREAM_HTTP_CHNK_FreeBuffer (void);
/* End global function prototypes */
#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_IPTUNER_HTTP_CHNK_H__ */
