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
#ifdef ICY_PROTOCOL
#define MAX_METAINT_NUM_IN_BUFFER (10)

#ifdef SUPPORT_TITLE
#define MAX_TITLE_LENGTH	(256)
#endif
#endif

#if REAL_TIME_MULTISOCKET_SUPPORT
#define CHUNK_BUUFER_DEFAULT_SIZE 64*1024
#define BROADCOM_PLAYBACK_READ_BLOCK 60*1024
#define REALTIME_PLAYBUFFER_CHUNK_TOTAL 50
#define REALTIME_PROBEBUFFER_CHUNK_TOTAL 50
#define MP4MOOV_BLOCK_MAX 6*1024*1024
#endif

#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT) || defined (USE_MOOVBOX_CACHING)
#define MOOVBOX_CACHING_READ_BLOCK 60*1024
#define MP4BOX_DEFAULT_SIZE 8
#define CACHING_ERROR_RETRYCOUNT 10
#endif
/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
#ifdef ICY_PROTOCOL
typedef struct
{
	unsigned long	ulICYSemId;				/* Icy Protocol Semaphore Id */
	HINT32	nIcyMetaInt;					/* MetaInt Value */
	HINT64	lIcyOffset;						/* Current Read Offset */
	HINT32	nIcyReadSize;					/* Read Data Size */
	HUINT32	nStartOffset;				/* Start offset in Read Buffer */
	HUINT32	nEndOffset;					/* End offset in Read Buffer */
	HUINT32	nCurrentBufferOffset;		/* Current offset in Read Buffer */
	HUINT8	*pcIcyReadBuffer;				/* Read Icy Data Buffer */
	HUINT8	*pcIcyReadTempBuffer;			/* Temp Icy Data Buffer */
} STREAMING_ICYInstance_t;
#endif

#if REAL_TIME_MULTISOCKET_SUPPORT
typedef struct
{
	HINT32	ProbeHandle;
	HINT32	IndexHandle;
	HINT32	PlayerHandle;
	HINT32	DrmHandle;
} STREAMING_MULTISOCKET_HANDLE_t;

typedef struct
{
	HINT8			*pcData;
	HINT64			ulStartindex;
	HUINT32			unData_Length;
	HINT64			lBuffer_ReadPosition;
	HINT64			lBuffer_CursorPosition;
	HUINT32			unTemp;
} STREAMING_MEDIA_INASTANCE_t;
#endif

#if defined(USE_RAMBUFFERING_MOOVBOX_CACHING_SUPPORT) || defined(USE_MOOVBOX_CACHING)
typedef enum
{
	MOOVBOX_CACHING_STATE_NONE = 0x00001000,
	MOOVBOX_CACHING_STATE_READY,
	MOOVBOX_CACHING_STATE_CACHED,	
	MOOVBOX_CACHING_STATE_UNKNOWN,
} MOOVBOX_STATE_e;

typedef struct
{
	MOOVBOX_STATE_e eState;
	HUINT8			*pucMoov;
	HUINT64			ulStartOffset;
	HUINT64			unDataLength;
	HINT64			lCursorPos;
} MOOVBOX_INSTANCE_t;
typedef struct
{
	HUINT32 nBoxLen;
	HUINT8 cBoxType[4]; 
} BOX_INSTANCE_t;
#endif
/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/** MMS VFIO Functions (for DNP or STREAM) */
int DI_STREAM_HTTP_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle );
int DI_STREAM_HTTP_Close (VFIO_Handle_t tHandle);
int DI_STREAM_HTTP_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize);
int DI_STREAM_HTTP_Seek (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_HTTP_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset);
int DI_STREAM_HTTP_IsEOF (VFIO_Handle_t tHandle);
int DI_STREAM_HTTP_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle);
off_t DI_STREAM_HTTP_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_HTTP_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size);
void STREAM_HTTP_PreBuffering (DI_IPTUNER_Index_e nTunerIndex);

DI_ERR_CODE DI_STREAM_HTTP_Control ( DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 );

#if REAL_TIME_MULTISOCKET_SUPPORT
int DI_STREAM_HTTP_SetDrmMode (HUINT8 bIsDRM);
int DI_STREAM_HTTP_SetContentFormat (DI_MEDIA_STREAM_TYPE_e eStream_type);
#endif

#ifdef ICY_PROTOCOL
int DI_STREAM_HTTP_ICY_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle );
int DI_STREAM_HTTP_ICY_Close (VFIO_Handle_t tHandle);
int DI_STREAM_HTTP_ICY_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize);
int DI_STREAM_HTTP_ICY_Seek (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_HTTP_ICY_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset);
int DI_STREAM_HTTP_ICY_IsEOF (VFIO_Handle_t tHandle);
int DI_STREAM_HTTP_ICY_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle);
off_t DI_STREAM_HTTP_ICY_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence);
int DI_STREAM_HTTP_ICY_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size);
#endif


/** HTTP DNP Control Functions */
void DI_STREAM_HTTP_DnpStartReceiving (void);
void DI_STREAM_HTTP_DnpStopReceiving (void);
void DI_STREAM_HTTP_DnpPauseReceiving (void);
void DI_STREAM_HTTP_DnpResumeReceiving (void);
int DI_STREAM_HTTP_DnpReceive (VFIO_Handle_t tHandle);

/** HTTP STREAM VFIO Register/Unregister */
DI_ERR_CODE DI_STREAM_HTTP_ICYProbe (const HINT8 *path,  DI_MEDIA_INFO_t *pstMediaInfo, HBOOL *pbIsICY);
void DI_STREAM_HTTP_RegisterCallback (void);
void DI_STREAM_HTTP_IcyVfioRegisterCallback (void);
void DI_STREAM_HTTP_UnregisterCallback (void);
#if STREAM_BUFFERING
void STREAM_HTTP_Buffering (DI_IPTUNER_Index_e nTunerIndex);
DI_ERR_CODE DI_STREAM_HTTP_RegisterBufferingCallback (DI_IPTUNER_PlayCallback_t *pCallback);
void DI_STREAM_HTTP_UnregisterBufferingCallback (void);
DI_ERR_CODE DI_STREAM_HTTP_CreateBuffer (DI_MEDIA_INFO_t *pstMediaInfo);
DI_ERR_CODE DI_STREAM_HTTP_FreeBuffer (void);
#endif
/* End global function prototypes */

#ifdef SUPPORT_TITLE
/*DI_ERR_CODE DI_STREAM_ICY_HTTP_RegisterEventCallback (DI_IPTUNER_EventCallback_t *pCallback);*/
DI_ERR_CODE DI_STREAM_HTTP_ICY_RegisterEventCallback (DI_IPTUNER_EventCallback_t *pCallback);
void DI_STREAM_HTTP_ICY_UnregisterEventCallback (void);
#endif

DI_ERR_CODE DI_STREAM_HTTP_GetProtoStatusCode (HINT32 *nHttpCode);
#if defined (CONFIG_PLAYREADY)
HBOOL DI_STREAM_HTTP_GetASFWrmHeader(HUINT8* pucUrl, HUINT8** pucWrmHeader, HUINT32* puiSize, HBOOL *bCocktail);
#endif

HINT64 DI_STREAM_HTTP_GetStreamSize(void);
HINT64 DI_STREAM_HTTP_GetCurrentReadPosition(void);


#ifdef  __cplusplus
}
#endif

#endif /* ! __DI_IPTUNER_HTTP_H__ */
