/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_iptuner_http.h  $
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

#ifndef __DI_IPTUNER_HTTP_H__
#define __DI_IPTUNER_HTTP_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "vfio20.h"
#include "di_iptuner_config.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* CANVAS TODO: DRAGON> need to rename this structure */
typedef struct
{
	HINT32 	nPurpose;	  /* FOR PROBE? OR FOR NEXUS FILE PLAY? */
	HUINT32 uiMediaMaxBitrateInChunkPerSec;
	off_t	player_last_read_cursor;
	off_t	player_last_seek_cursor;
	off_t	align_buffer_start_index;
	off_t	align_buffer_end_index;
	HUINT8*	p_align_buffer;
	off_t	indexer_last_seek_cursor;
	size_t 	player_last_read_size;
	HBOOL 	bDNPNetworkError;
#if 0 /* for CANVAS : dragon */
	HBOOL 	bProbeDone;
	HUINT32 uiContentDuration;
#endif
	HUINT32 unOpenFromNexusHandle;
	
} DI_STREAM_HTTP_USER_DATA_t;

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
int DI_STREAM_HTTP_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle );
int DI_STREAM_HTTP_Close (VFIO_Handle_t tHandle);
int DI_STREAM_HTTP_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize);
int DI_STREAM_HTTP_Seek (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_HTTP_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset);
int DI_STREAM_HTTP_IsEOF (VFIO_Handle_t tHandle);
int DI_STREAM_HTTP_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle);
off_t DI_STREAM_HTTP_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_HTTP_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size);
int DI_STREAM_HTTP_ChangeUrl (VFIO_Handle_t tHandle, const HINT8 *pUrl);

DI_ERR_CODE DI_STREAM_HTTP_Control ( DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 );

int DI_STREAM_HTTP_GetSmallFile ( const HINT8 *pUrl, const HINT8 *pUserAgentString , HUINT8 **pBuf, HINT32 *pSize);
int DI_STREAM_HTTP_ForcedExit ( void* pHStream ); /* Back-Off Algo. */		

int DI_STREAM_SIMPLE_HTTP_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle );
int DI_STREAM_SIMPLE_HTTP_Close (VFIO_Handle_t tHandle);
int DI_STREAM_SIMPLE_HTTP_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, ssize_t *ptReadSize);
int DI_STREAM_SIMPLE_HTTP_Seek (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_SIMPLE_HTTP_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset);
int DI_STREAM_SIMPLE_HTTP_IsEOF (VFIO_Handle_t tHandle);
int DI_STREAM_SIMPLE_HTTP_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle);
off_t DI_STREAM_SIMPLE_HTTP_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_SIMPLE_HTTP_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size);

#if REAL_TIME_DOUBLESOCKET_SUPPORT
int DI_STREAM_HTTP_SetDrmMode (HUINT8 bIsDRM);
#endif

/** HTTP STREAM VFIO Register/Unregister */
void DI_STREAM_HTTP_RegisterCallback (const char *pUrl);
void DI_STREAM_HTTP_UnregisterCallback (const char *pUrl);
/* End global function prototypes */

#if 0
DI_ERR_CODE DI_STREAM_HTTP_GetProtoStatusCode (HINT32 *nHttpCode);
#endif
#if defined (CONFIG_PLAYREADY)
HBOOL DI_STREAM_HTTP_GetASFWrmHeader(DI_IPTUNER_t* pHTuner);
HBOOL DI_STREAM_HTTP_GetSimpleIndex(DI_IPTUNER_t* pHTuner);
#endif

DI_ERR_CODE DI_STREAM_HTTP_GetStreamInfo(DI_IPTUNER_t* pHTuner, HINT64 *file_pos, HINT64 *file_size);
#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_IPTUNER_HTTP_H__ */
