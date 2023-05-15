/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_http_chnk.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	SungYong Jeon
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date:   Tue May 20 21:49:01 KST 2009 $
 * File Description:		 HTTP DNP Receiver & HTTP VFIO for Streaming
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd. 								*/
/* All rights reserved.												*/
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include "di_iptuner_config.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"
#include "di_iptuner.h"
#include "di_iptuner_stream.h"
#include "di_iptuner_http_chnk.h"
#include "di_iptuner_dnp.h" /* DNP_USER_DATA_t */
#include "vfio.h"
#include "stream_http.h"
#include "drv_err.h"
/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define DI_IPTUNER_HTTP_CHNK_DEBUG

/* DEBUG DEFINE */
#ifdef IPTUNER_ALL_DEBUG
#ifdef DI_IPTUNER_HTTP_CHNK_DEBUG
#define PrintData		VK_printf
#define PrintDebug		VK_printf
#define PrintEnter		VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit		VK_printf("(-)%s\n", __FUNCTION__)
#define PrintError		VK_printf
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		VK_printf
#endif
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif /* END DEBUG DEFINE */

#define UNUSED_PARAM(x)  ((void)x)

typedef enum {
	CHNK_PREBUFFERING_TASK_STATE_STARTED,
	CHNK_PREBUFFERING_TASK_STATE_KILLING,
	CHNK_PREBUFFERING_TASK_STATE_DONE
} CHNK_PREBUFFERING_TASK_STATE_e;

#define _NEXUS_ALIGN_BUFFER_SIZE		(1024*10)
#define _CHNK_SEEK_RETRY_COUNT 		(3)
#define _CHNK_READ_BLOCK_SIZE 		(64*1024) // 64Kbytes
#define _PREBUFFERING_POOL_SIZE 		(5*1024*1024)
#define _PREBUFFERING_MAX_SIZE 			(30*1024*1024)
#define _PREBUFFERING_ONOFF_THRESHOLD	(15*1024*1024)

#define _CHNK_NETWORK_ERROR_RETRY_COUNT (10)
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef struct CHNK_BufferManager_s
{
	HUINT8 	*pBPool;
	HUINT32 uBPoolTotalSize;
	HINT32	iHighThreshold;
	HINT32 	iStartPos;
	HINT32 	iEndPos;
	HUINT8 	*pWritePtr;
	HUINT8 	*pReadPtr;
	HINT64	illStartOffset;
	HINT64	illEndOffset;

	unsigned long ulLock;
	CHNK_PREBUFFERING_TASK_STATE_e eTaskState;
	CHNK_PREBUFFERING_TASK_STATE_e eChangeTaskState;

	HBOOL bNetworkError;
	HINT64 illRequestedSeekOffset; /* depend on following bSupportSeek */
} CHNK_BufferManager_t;

typedef struct HTTP_CHNK_s
{
	HBOOL bUseBufferPool;            /* will be changed w/ setting param */
	HBOOL bOpenedForProbe;
	HBOOL bSupportSeek;              /* default false; */
	HINT16 iOpenFromNexusCounter;
	HINT16 iRegisterCounter;
	unsigned long ulSemId;

	STREAM_ACCESS_t stStreamAccess;
	DI_IPTUNER_PlayCallback_t stHttpBufferingCallback;
	CHNK_BufferManager_t *bufmgr;
} HTTP_CHNK_t;
/* End typedef */

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
HTTP_CHNK_t *_ghHttpChunkedTE = NULL;
/* End global variable */

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
extern HULONG s_ulStreamMsgQId;
extern HULONG s_ulBufSema;
/* End static variable */

/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
#if __________p__________
#endif

static HTTP_CHNK_t *P_STREAM_CHNK_OpenHandle(void);
static void P_STREAM_CHNK_ReleaseHandle(HTTP_CHNK_t *pChunked);


static HTTP_CHNK_t *_CHNK_GetHandle(void)
{
	return _ghHttpChunkedTE;
}

static CHNK_BufferManager_t *_CHNK_GetBufferManager(HTTP_CHNK_t *pChunked)
{
	if (pChunked != NULL)
	{
		return pChunked->bufmgr;
	}

	return NULL;
}

static void _CHNK_Lock(HTTP_CHNK_t *pChunked)
{
	if (pChunked != NULL)
	{
		VK_SEM_Get(pChunked->ulSemId);
	}
}

static void _CHNK_Unlock(HTTP_CHNK_t *pChunked)
{
	if (pChunked != NULL)
	{
		VK_SEM_Release(pChunked->ulSemId);
	}
}

static void _BufferPool_Lock(CHNK_BufferManager_t *pBufferMgr)
{
	if (pBufferMgr != NULL)
	{
		VK_SEM_Get(pBufferMgr->ulLock);
	}
}

static void _BufferPool_Unlock(CHNK_BufferManager_t *pBufferMgr)
{
	if (pBufferMgr != NULL)
	{
		VK_SEM_Release(pBufferMgr->ulLock);
	}
}

static void _ExternalBuffer_Lock(void)
{
	/*VK_SEM_Get(s_ulBufSema);*/
	return;
}

static void _ExternalBuffer_Unlock(void)
{
	/*VK_SEM_Release(s_ulBufSema);*/
	return;
}

#if __________task__________
#endif

#if __________di__________
#endif

/** Open From User Application and bmedia*/
int DI_STREAM_HTTP_CHNK_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle )
{
	HINT32 nRet = DI_IPT_SUCCESS;
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	HTTP_CHNK_t *pChunked = NULL;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;

	PrintEnter;
	UNUSED_PARAM (mode);

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunk handle!\n", __FUNCTION__, __LINE__);
		ptHandle->ucAvailable = 0;
		vRet = VFIO_ERROR_IN_DEVICE;
		goto func_error;
	}

	pChunked->bOpenedForProbe = TRUE;
	if (pChunked->bUseBufferPool)
	{
		/* 이 function은 Probe 를 위한 bmedia 에 의해 호출된다. */
#if 0
		PrintDebug("%s(%d) Info> Probe Open\n", __FUNCTION__, __LINE__);
		DNP_USER_DATA_t * pDnpUserData = DD_MEM_Alloc(sizeof(DNP_USER_DATA_t));
		pDnpUserData->indexer_last_seek_cursor = 0;
		pDnpUserData->p_align_buffer = NULL;
		pDnpUserData->align_buffer_start_index = -1;
		pDnpUserData->align_buffer_end_index = -1;
		ptHandle->pDnpUserData = pDnpUserData;
#endif
	}

	VK_memset(&pChunked->stStreamAccess, 0, sizeof (STREAM_ACCESS_t));
	pChunked->stStreamAccess.psz_path = VK_StrDup((char *)path);
	if (pChunked->stStreamAccess.psz_path == NULL)
	{
		PrintError("%s(%d) Error> Out of memory!\n", __FUNCTION__, __LINE__);
		ptHandle->ucAvailable = 0;
		vRet = VFIO_ERROR_IN_DEVICE;
		goto func_error;
	}

	nRet = STREAM_HTTP_Open (&pChunked->stStreamAccess);
	if (nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) Error> HTTP_Open Fail.\n", __FUNCTION__, __LINE__);
		pChunked->stStreamAccess.b_alive = TRUE;
		ptHandle->ucAvailable = 0;

		if (pChunked->stStreamAccess.psz_path != NULL)
		{
			DD_MEM_Free(pChunked->stStreamAccess.psz_path);
			pChunked->stStreamAccess.psz_path =NULL;
		}
		vRet = VFIO_ERROR_IN_DEVICE;
		goto func_error;
	}
	PrintData("%s(%d) i_update       = %d \n", __FUNCTION__, __LINE__,pChunked->stStreamAccess.info.i_update);
	PrintData("%s(%d) i_size         = %"PRId64"\n", __FUNCTION__, __LINE__,pChunked->stStreamAccess.info.i_size);
	PrintData("%s(%d) i_pos          = %"PRId64"\n", __FUNCTION__, __LINE__,pChunked->stStreamAccess.info.i_pos);
	PrintData("%s(%d) b_eof          = %d \n", __FUNCTION__, __LINE__,pChunked->stStreamAccess.info.b_eof);
	PrintData("%s(%d) i_title        = %d \n", __FUNCTION__, __LINE__,pChunked->stStreamAccess.info.i_title);
	PrintData("%s(%d) i_seekpoint    = %d \n", __FUNCTION__, __LINE__,pChunked->stStreamAccess.info.i_seekpoint);
	PrintData("%s(%d) b_prebuffered  = %d \n", __FUNCTION__, __LINE__,pChunked->stStreamAccess.info.b_prebuffered);

#if 0
	if ((pChunked->stStreamAccess.info.i_size > 0) && (pChunked->stStreamAccess.info.i_size < 4*1024))
	{
		PrintError("%s(%d) Error> HTTP_Open Fail.\n", __FUNCTION__, __LINE__);
		PrintError("%s(%d) Error> Invalid File Size(File Size = %"PRId64".\n", __FUNCTION__, __LINE__, pChunked->stStreamAccess.info.i_size);
		pChunked->stStreamAccess.b_alive = TRUE;
		ptHandle->ucAvailable = 0;

		(void)STREAM_HTTP_Close (&pChunked->stStreamAccess);
		pChunked->stStreamAccess.b_alive = FALSE;

		if (pChunked->stStreamAccess.psz_path != NULL)
		{
			DD_MEM_Free(pChunked->stStreamAccess.psz_path);
			pChunked->stStreamAccess.psz_path =NULL;
		}
		vRet = VFIO_ERROR_IN_DEVICE;
		goto func_error;
	}
#endif


	pstHttpSys = (HTTP_ACCESS_SYS_t*) pChunked->stStreamAccess.p_sys;
	ptHandle->ucAvailable = 1;
	ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd; /** http socket descriptor */
	PrintData("%s(%d) callback handle=0x%x\n", __FUNCTION__, __LINE__, ptHandle->tFile.callback.unHandle);

	nRet = VK_SEM_Create(&pChunked->ulSemId,"chnk_sema", DEFAULT_SUSPEND_TYPE);
	if ( nRet != VK_OK )
	{
		PrintError("%s(%d) Error> Create(0x%x) \n\r", __FUNCTION__, __LINE__, nRet);

		(void)STREAM_HTTP_Close (&pChunked->stStreamAccess);
		pChunked->stStreamAccess.b_alive = FALSE;

		if (pChunked->stStreamAccess.psz_path != NULL)
		{
			DD_MEM_Free(pChunked->stStreamAccess.psz_path);
			pChunked->stStreamAccess.psz_path =NULL;
		}
		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

func_error:
	PrintExit;
	return vRet;
}

/* Don't use internal lock; Locking will done externally */
static int P_STREAM_CHNK_GetBufferSize (CHNK_BufferManager_t *pBufferMgr)
{
	int nRet = 0;

	if (pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL arg!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if ((int)pBufferMgr->pWritePtr > (int)pBufferMgr->pReadPtr)
	{
		nRet = (int)pBufferMgr->pWritePtr - (int)pBufferMgr->pReadPtr;
	}
	else if ((int)pBufferMgr->pWritePtr == (int)pBufferMgr->pReadPtr)
	{
		/** 초기 state를 제외하고는 pBufferMgr->pWritePtr 와 ptrR이 동일할 수 없다. */
		nRet = 0;
	}
	else
	{
		nRet = pBufferMgr->uBPoolTotalSize - ((int)pBufferMgr->pReadPtr - (int)pBufferMgr->pWritePtr);
	}

func_error:
	return nRet;
}

/* Reset bufferPool caused by out_of_bound seek() */
static int P_STREAM_CHNK_ClearBuffer (CHNK_BufferManager_t *pBufferMgr, HINT64 off_start)
{
	int nRet = 0;

	PrintEnter;
	if (pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL arg!\n", __FUNCTION__, __LINE__);
		return (-1);
	}

	_ExternalBuffer_Lock();
	if (NULL != pBufferMgr->pBPool)
	{
		//VK_memset(pBufferMgr->pBPool, 0, sizeof(pBufferMgr->uBPoolTotalSize));
		pBufferMgr->pWritePtr = (HUINT8 *)pBufferMgr->iStartPos;
		pBufferMgr->pReadPtr = (HUINT8 *)pBufferMgr->iStartPos;
	}
	else
	{
		nRet = -1;
	}
	pBufferMgr->illStartOffset = off_start;
	pBufferMgr->illEndOffset = off_start;

	_ExternalBuffer_Unlock();

	PrintExit;
	return nRet;
}

/* Don't use internal lock here! */
static int P_STREAM_CHNK_WriteBuffer (HTTP_CHNK_t *pChunked, HUINT8 *pBuf, size_t blen)
{
	size_t space =0;
	int size1=0;
	int size2=0;
	int i=0;
	CHNK_BufferManager_t *pBufferMgr = NULL;

	//PrintEnter;
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	pBufferMgr = _CHNK_GetBufferManager(pChunked);
	if (pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL bufmgr!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	space = (pBufferMgr->uBPoolTotalSize - P_STREAM_CHNK_GetBufferSize(pBufferMgr)) - 1;
	if (space < blen)
	{
		while ((space = (pBufferMgr->uBPoolTotalSize - P_STREAM_CHNK_GetBufferSize(pBufferMgr)) -1) < blen )
		{
			if (pChunked->stStreamAccess.b_alive == FALSE)
			{
				PrintError("%s(%d) Error> Session is canceled.\n", __FUNCTION__, __LINE__);
				return -1;
			}

			if (pChunked->stStreamAccess.info.b_prebuffered == TRUE)
			{
				PrintDebug("%s(%d) Reset Prebuffering\n", __FUNCTION__, __LINE__);
				return -1;
			}

			if (pBufferMgr->eChangeTaskState == CHNK_PREBUFFERING_TASK_STATE_KILLING)
			{
				PrintDebug("Break PreBuffering Main While\n");
				return -1;
			}

			PrintDebug("%s(%d) HTTP_WRITE Waiting IO for %d sec.\n", __FUNCTION__, __LINE__,i++);
			VK_TASK_Sleep(100); /* 1000 -> 100 for better respose time */
		}
	}

	_ExternalBuffer_Lock();

	if ( ((int)pBufferMgr->pWritePtr + (int)blen) > (int)pBufferMgr->iEndPos)
	{
		size1 = (int)pBufferMgr->iEndPos - (int)pBufferMgr->pWritePtr;
		VK_memcpy(pBufferMgr->pWritePtr,pBuf,size1);

		size2 = (int)blen-(int)size1;
		VK_memcpy((void*)pBufferMgr->iStartPos, pBuf+size1,size2);

		pBufferMgr->pWritePtr = (HUINT8*)(pBufferMgr->iStartPos + size2);
	}
	else
	{
		VK_memcpy(pBufferMgr->pWritePtr,pBuf,blen);
		pBufferMgr->pWritePtr += blen;
	}

	/* Update the END offset of window */
	pBufferMgr->illEndOffset += blen;

	_ExternalBuffer_Unlock();

//	PrintData("%s(%d) Write Buffer s(%"PRId64") e(%"PRId64")\n", __FUNCTION__, __LINE__,pBufferMgr->illStartOffset, pBufferMgr->illEndOffset);

	//PrintExit;
	return blen;
}

/** Buffering Function: Called by IPTUNER_StreamTask() */
int DI_STREAM_HTTP_CHNK_PreBuffering (DI_IPTUNER_Index_e nTunerIndex)
{
	HUINT8 bufferBlock[_CHNK_READ_BLOCK_SIZE];
	int nRead=0;
	HUINT32 nNetworkRetryCount = 0;
	HTTP_CHNK_t *pChunked = NULL;
	CHNK_BufferManager_t *pBufferMgr = NULL;

	PrintEnter;

	PrintDebug("%s(%d) Tuner Type(%d)\n", __FUNCTION__, __LINE__,nTunerIndex);
	if (1)
	{
		if (nTunerIndex != DI_IPTUNER_STREAM_HTTP_CHNK)
		{
			PrintError("%s(%d) Error> invalid Tuner Type(%d)\n", __FUNCTION__, __LINE__,nTunerIndex);
			;
		}
	}

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	pBufferMgr = _CHNK_GetBufferManager(pChunked);
	if (pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL bufmgr!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	pBufferMgr->eTaskState = CHNK_PREBUFFERING_TASK_STATE_STARTED;
	pBufferMgr->eChangeTaskState = CHNK_PREBUFFERING_TASK_STATE_STARTED;

	while (1)
	{
		/* Don't use buffer_pool lock here! */
reset_buffer:
		//_BufferPool_Lock(pBufferMgr);
		if (pBufferMgr->pBPool == NULL)
		{
			PrintError("%s(%d) Error> Buffer pool is NULL\n", __FUNCTION__, __LINE__);
			break; /* FreeBuffer is called by stream close */
		}

		if (pBufferMgr->eChangeTaskState == CHNK_PREBUFFERING_TASK_STATE_KILLING)
		{
			PrintDebug("Break PreBuffering Main While\n");
			break;
		}

#if defined(CONFIG_MHEG_SUPPORT) && defined(CONFIG_ICS)
		/* for task switching */
		VK_TASK_Sleep(10);
#endif

		if (0)
		{
			if (pBufferMgr->illRequestedSeekOffset != -1) /* bSupportSeek */
			{
				_BufferPool_Lock(pBufferMgr);
				PrintDebug("==================\n============\nRequestedSeekPoint Received\n==============\n");

				if (0) /* Can't support seek at chunked_live_streaming: no-range */
				{
					P_STREAM_CHNK_ClearBuffer(pBufferMgr, pBufferMgr->illRequestedSeekOffset); /* reset window pos */

					nNetworkRetryCount = 0;
					while(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( &pChunked->stStreamAccess, pBufferMgr->illStartOffset )) /// <--
					{
						nNetworkRetryCount++;
						PrintError("%s(%d) Error> STREAM_HTTP_Seek fail Retrying...\n", __FUNCTION__, __LINE__);
						VK_TASK_Sleep(100);
						if (nNetworkRetryCount > _CHNK_NETWORK_ERROR_RETRY_COUNT)
						{
							PrintError("%s(%d) Error> Too many retries(%d)\n", __FUNCTION__, __LINE__, nNetworkRetryCount);
							pBufferMgr->bNetworkError = TRUE;
							_BufferPool_Unlock(pBufferMgr);
							goto exit_stop;
						}
					}
				}

				pBufferMgr->illRequestedSeekOffset = -1; /* seek request is done */
				_BufferPool_Unlock(pBufferMgr);
				PrintDebug("===============\nSeek Done\n");
			}
		}

		if (pBufferMgr->iHighThreshold > P_STREAM_CHNK_GetBufferSize(pBufferMgr))
		{
			/***********************************************************************
			 *  1. General Receiving Control
			 *	 VFIO 가 Data를 소비하는 도중에 Buffer를 유동적으로 채워주는 루틴.
			 *	 ReadBuffer 와 동시에 호출된다.
			 ***********************************************************************/
			VK_memset(bufferBlock,0,_CHNK_READ_BLOCK_SIZE);
			{
				if (pChunked->stStreamAccess.info.i_size < pChunked->stStreamAccess.info.i_pos)
				{
					PrintError("%s(%d) Error> Invalid pos: i_size = %"PRId64", i_pos = %"PRId64"\n", __FUNCTION__, __LINE__, pChunked->stStreamAccess.info.i_size, pChunked->stStreamAccess.info.i_pos);
					break;
				}

				/* If FreeBufferPool() is called while STREAM_HTTP_Read(),  */
				nNetworkRetryCount = 0;
				while ((nRead = STREAM_HTTP_Read(&pChunked->stStreamAccess, bufferBlock, _CHNK_READ_BLOCK_SIZE)) <= 0) /* retry for network or socket error */
				{
					/*off_t cur = pChunked->stStreamAccess.info.i_pos;*/
#ifdef	CONFIG_TEMP_DTG_HDR2000T
					if (0)
#else
					if (pChunked->stStreamAccess.info.b_eof == TRUE)
#endif
					{
						PrintError("%s(%d) Error(info)> Finish Buffering (End of Stream) (%d)\n", __FUNCTION__, __LINE__,nRead);
						goto reset_buffer;
					}
					if (pBufferMgr->eChangeTaskState == CHNK_PREBUFFERING_TASK_STATE_KILLING)
					{
						PrintError("%s(%d) Error(info)> Break PreBuffering Main While\n", __FUNCTION__, __LINE__);
						goto exit_stop;
						//break;
					}
					nNetworkRetryCount++;
					PrintError("%s(%d) Error> STREAM_HTTP_Read fail Retrying... ( %d ) (%"PRId64" : %"PRId64")\n",
							__FUNCTION__,__LINE__,
							nNetworkRetryCount,pChunked->stStreamAccess.info.i_pos,pChunked->stStreamAccess.info.i_size);
	#if 0
					if (DI_IPT_SUCCESS !=STREAM_HTTP_Seek ( &pChunked->stStreamAccess, cur )) // <--
					{
						PrintError("%s(%d) Error> STREAM_HTTP_Seek fail...\n", __FUNCTION__, __LINE__);
					}
	#endif
					VK_TASK_Sleep(100);

					if (nNetworkRetryCount > _CHNK_NETWORK_ERROR_RETRY_COUNT)
					{
						PrintError("%s(%d) Error> Send Failed code To Buffering Reader(Player,Indexer)\n", __FUNCTION__, __LINE__);
						pBufferMgr->bNetworkError = TRUE;
						goto exit_stop;
					}
				}
			}

			/* Don't use internal lock at WriteBuffer() */
			(void) P_STREAM_CHNK_WriteBuffer(pChunked, bufferBlock, nRead);

			if (pChunked->stStreamAccess.b_alive == FALSE)
			{
				PrintError("%s(%d) Error(info)> Session is canceled.\n", __FUNCTION__, __LINE__);
				break;
			}
			if (pChunked->stStreamAccess.info.b_eof == TRUE)
			{
				PrintDebug("%s(%d) Finish Buffering (End of Stream) (%d)\n", __FUNCTION__, __LINE__,nRead);
				VK_TASK_Sleep(100);
				continue;
			}

			if (pChunked->stStreamAccess.info.b_prebuffered == TRUE)
			{
				pChunked->stStreamAccess.info.b_prebuffered = FALSE;
				PrintError("%s(%d) Error(info)> --> reset_buffer(%d)\n", __FUNCTION__, __LINE__, nRead);
				//_BufferPool_Unlock(pBufferMgr);  //--------------------------------  Critical Section
				//VK_TASK_Sleep(100);
				goto reset_buffer;
			}

		}
	} /* end while */

	//_BufferPool_Unlock(pBufferMgr); 	//--------------------------------	Critical Section
exit_stop:
	PrintError("%s(%d) Error(no)> buffering_loop exit!\n", __FUNCTION__, __LINE__);
	pBufferMgr->eTaskState = CHNK_PREBUFFERING_TASK_STATE_DONE;
	PrintExit;
	return 0;
}


/* Blocking Read */
ssize_t P_STREAM_CHNK_ReadBuffer (STREAM_ACCESS_t *pAccess, CHNK_BufferManager_t *pBufferMgr, HUINT8 *pBuf, size_t i_blen)
{
	int size1=0;
	int size2=0;
	int iCurBufferSize=0;

	//PrintEnter;

	if ((pAccess == NULL) || (pBufferMgr == NULL))
	{
		PrintError("%s(%d) Error> Invalid args!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	iCurBufferSize = P_STREAM_CHNK_GetBufferSize(pBufferMgr);
	if ( iCurBufferSize < (int)i_blen )
	{
		if (pAccess->info.b_eof == TRUE)
		{
			i_blen = iCurBufferSize;
		}
		else
		{
			while ((iCurBufferSize = P_STREAM_CHNK_GetBufferSize(pBufferMgr)) < (int)i_blen)
			{
				if (pBufferMgr->bNetworkError || (pAccess->info.i_size <= (pAccess->info.i_pos + iCurBufferSize)) ||
					(pBufferMgr->eChangeTaskState == CHNK_PREBUFFERING_TASK_STATE_KILLING))
				{
					if (iCurBufferSize < (int)i_blen)
					{
						i_blen = iCurBufferSize;
					}

					PrintDebug("%s(%d) Info> !!!!!!!! Exit Waiting !!!!!!!! \n", __FUNCTION__,__LINE__);
					break;
				}

				PrintError("%s(%d) Error(info)> Waiting> PreBuffer hasn't enough data .... szContent(%"PRId64") szBuffer(%"PRId64"), buf_gap:%d, i_blen:%ld\n"
							,__FUNCTION__,__LINE__
							,pAccess->info.i_size,pAccess->info.i_pos+iCurBufferSize
							,iCurBufferSize,i_blen);

				/* We need to wait some period.*/
				VK_TASK_Sleep(100);
			} /* end of while */
		}
	}

	/* The Belows are in Critical Section */
	_ExternalBuffer_Lock();
	if ( (int)(pBufferMgr->pReadPtr+i_blen) > (int)pBufferMgr->iEndPos)
	{
		size1 = (int)pBufferMgr->iEndPos - (int)pBufferMgr->pReadPtr;
		VK_memcpy(pBuf, pBufferMgr->pReadPtr, size1);
		VK_memset(pBufferMgr->pReadPtr,0,size1);
		size2 = (i_blen-size1);

		VK_memcpy((pBuf+size1), (const void*)pBufferMgr->iStartPos, size2);
		VK_memset((void*)pBufferMgr->iStartPos,0,size2);

		pBufferMgr->pReadPtr = (HUINT8 *)(pBufferMgr->iStartPos+size2);
	}
	else
	{
		VK_memcpy(pBuf, pBufferMgr->pReadPtr, i_blen);
		VK_memset(pBufferMgr->pReadPtr,0,i_blen);

		pBufferMgr->pReadPtr += i_blen;
	}

	/* Update the start offset of window */
	pBufferMgr->illStartOffset += i_blen;

	_ExternalBuffer_Unlock();
	//PrintExit;
	PrintDebug("%s(%d) Exit Info> EOF : i_blen (%d)\n", __FUNCTION__, __LINE__, i_blen);
	return i_blen;
}


/* Called by DI_IPTUNER_StreamOpen() */
DI_ERR_CODE DI_STREAM_HTTP_CHNK_CreateBuffer (DI_MEDIA_INFO_t *pstMediaInfo)
{
	DI_ERR_CODE nRet= DI_ERR_OK;
	HINT64 nTotalSize = 0;
	HTTP_CHNK_t *pChunked = NULL;
	CHNK_BufferManager_t *pBufferMgr = NULL;

	PrintEnter;
	UNUSED_PARAM(pstMediaInfo);

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	pBufferMgr = _CHNK_GetBufferManager(pChunked);
	if (pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL bufmgr!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	nTotalSize = pChunked->stStreamAccess.info.i_size;
	pBufferMgr->bNetworkError = FALSE;

	if ( nTotalSize < _PREBUFFERING_ONOFF_THRESHOLD )
	{
		PrintDebug("%s(%d) Info> This content has lower size than %d Mbytes. So we should switchover the buffering mode to RealTime!!\n"
				,__FUNCTION__
				,__LINE__
				,_PREBUFFERING_ONOFF_THRESHOLD);
				return DI_ERR_ERROR;
	}

	if ((pBufferMgr->uBPoolTotalSize == 0) || (pBufferMgr->uBPoolTotalSize > _PREBUFFERING_MAX_SIZE))
	{
		pBufferMgr->uBPoolTotalSize = _PREBUFFERING_POOL_SIZE;
	}

	nRet = VK_SEM_Create(&pBufferMgr->ulLock,"httpPreBufferingSema", DEFAULT_SUSPEND_TYPE);
	if ( nRet != VK_OK )
	{
		PrintError("%s(%d) Error> (0x%x) \n\r", __FUNCTION__, __LINE__, nRet);
	}

	pBufferMgr->pBPool = (HUINT8 *)DD_MEM_Alloc ((size_t)pBufferMgr->uBPoolTotalSize);
	if (pBufferMgr->pBPool == NULL)
	{
		PrintError("%s(%d) Error> Out of memory!(%d)\n", __FUNCTION__, __LINE__, pBufferMgr->uBPoolTotalSize);
		return DI_ERR_ERROR;
	}

	/** Assign Pointer & Thresholds value*/
	pBufferMgr->uBPoolTotalSize = pBufferMgr->uBPoolTotalSize;
	pBufferMgr->pWritePtr = pBufferMgr->pBPool;
	pBufferMgr->pReadPtr = pBufferMgr->pBPool;
	pBufferMgr->iStartPos = (HINT32)pBufferMgr->pBPool;
	pBufferMgr->iEndPos = (HINT32) pBufferMgr->pBPool + pBufferMgr->uBPoolTotalSize - 1 ;
	pBufferMgr->iHighThreshold = pBufferMgr->uBPoolTotalSize * 90 / 100;
	pBufferMgr->illStartOffset = 0;
	pBufferMgr->illEndOffset = 0;

	PrintData ("--------------------------------------------------\n");
	PrintData ("BufferPool Size : %d bytes\n",pBufferMgr->uBPoolTotalSize);
	PrintData ("writePtr        : %p\n",pBufferMgr->pWritePtr);
	PrintData ("readPtr         :  %p\n",pBufferMgr->pReadPtr);
	PrintData ("startPos        : 0x%x\n",pBufferMgr->iStartPos);
	PrintData ("endPos          : 0x%x\n",pBufferMgr->iEndPos);
	PrintData ("highThreshHold  : %d bytes\n",pBufferMgr->iHighThreshold);
	PrintData ("--------------------------------------------------\n");

	PrintExit;
	return nRet;
}

void P_STREAM_CHNK_FreeBufferPool(HTTP_CHNK_t *pChunked)
{
	CHNK_BufferManager_t *pBufferMgr = NULL;
	HUINT32 nWaitingTime = 0;
	HUINT32 uWaitingCount = 0;

	PrintEnter;

	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	pBufferMgr = _CHNK_GetBufferManager(pChunked);
	if (pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL bufmgr!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	if (pBufferMgr->pBPool == NULL ) /* in the case of realtime streaming */
	{
		PrintError("%s(%d) Error> Invalid NULL bufpool!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

	pBufferMgr->eChangeTaskState = CHNK_PREBUFFERING_TASK_STATE_KILLING;
	while(pBufferMgr->eTaskState != CHNK_PREBUFFERING_TASK_STATE_DONE)
	{
		PrintDebug("%s(%d) Wait(%d) For Killing Prebuffering Task(%d,%d)\n", __FUNCTION__, __LINE__, uWaitingCount, pBufferMgr->eChangeTaskState, pBufferMgr->eChangeTaskState);
		pChunked->stStreamAccess.info.b_prebuffered = TRUE; /* break out WriteBuffer Waiting State */
		if (nWaitingTime == 2000)
		{
			PrintError("%s(%d) Error> Force to killing Prebuffering Task(%d,%d)\n", __FUNCTION__, __LINE__, pBufferMgr->eChangeTaskState, pBufferMgr->eChangeTaskState);
			pBufferMgr->eChangeTaskState = CHNK_PREBUFFERING_TASK_STATE_DONE;
			nWaitingTime = 0;
		}
		else
		{
			VK_TASK_Sleep(100);
			nWaitingTime=nWaitingTime+100;
		}
		uWaitingCount++;
	}
	PrintDebug("%s(%d) --> PREBUFFERING_TASK_STATE_DONE\n", __FUNCTION__, __LINE__);
	(void)VK_SEM_Destroy(pBufferMgr->ulLock);

	if (pBufferMgr->pBPool != NULL)
	{
		DD_MEM_Free (pBufferMgr->pBPool);
		pBufferMgr->pBPool = NULL;
	}

func_error:
	PrintExit;
	return;
}

/* Called by DI_IPTUNER_StreamClose() */
DI_ERR_CODE DI_STREAM_HTTP_CHNK_FreeBuffer (void)
{
	DI_ERR_CODE diErr = DI_ERR_OK;
	HTTP_CHNK_t *pChunked = NULL;

	PrintEnter;

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		diErr = DI_ERR_ERROR;
		goto func_error;
	}

	/* Freeze buffer pool */
	if (pChunked->bUseBufferPool == TRUE)
	{
		P_STREAM_CHNK_FreeBufferPool(pChunked);

		if (pChunked->stStreamAccess.b_alive != FALSE)
		{
			(void) STREAM_HTTP_Close ( &pChunked->stStreamAccess);
			pChunked->stStreamAccess.b_alive = FALSE;
		}

		if (pChunked->stStreamAccess.psz_path != NULL)
		{
			DD_MEM_Free(pChunked->stStreamAccess.psz_path);
			pChunked->stStreamAccess.psz_path = NULL;
		}
		(void)VK_SEM_Destroy(pChunked->ulSemId);
	}

func_error:
	PrintExit;
	return diErr;
}

/** */
int DI_STREAM_HTTP_CHNK_Close (VFIO_Handle_t tHandle)
{
	HINT32 vRet = VFIO_ERROR_SUCCESS;
	HTTP_CHNK_t *pChunked = NULL;
	CHNK_BufferManager_t *pBufferMgr = NULL;
	HUINT32 nWaitingTime = 0;
	HUINT32 uWaitingCount = 0;

	PrintEnter;
	UNUSED_PARAM (tHandle);

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunk handle!\n", __FUNCTION__, __LINE__);
		goto func_next;
	}

	if (pChunked->bUseBufferPool == TRUE)
	{
		pBufferMgr = _CHNK_GetBufferManager(pChunked);
		if ((pBufferMgr != NULL) && (pBufferMgr->pBPool != NULL ))
		{
			/* Prevent stream_read() while buffering */
			pBufferMgr->eChangeTaskState = CHNK_PREBUFFERING_TASK_STATE_KILLING;
			while(pBufferMgr->eTaskState != CHNK_PREBUFFERING_TASK_STATE_DONE)
			{
				PrintDebug("%s(%d) Wait(%d) For Killing Prebuffering Task(%d,%d)\n", __FUNCTION__, __LINE__, uWaitingCount, pBufferMgr->eChangeTaskState, pBufferMgr->eChangeTaskState);
				pChunked->stStreamAccess.info.b_prebuffered = TRUE; /* break out WriteBuffer Waiting State */
				if (nWaitingTime == 2000)
				{
					PrintError("%s(%d) Error> Force to killing Prebuffering Task(%d,%d)\n", __FUNCTION__, __LINE__, pBufferMgr->eChangeTaskState, pBufferMgr->eChangeTaskState);
					pBufferMgr->eChangeTaskState = CHNK_PREBUFFERING_TASK_STATE_DONE;
					nWaitingTime = 0;
				}
				else
				{
					VK_TASK_Sleep(100);
					nWaitingTime=nWaitingTime+100;
				}
				uWaitingCount++;
			}
			PrintDebug("%s(%d) Will stream_close; Wait(%d) For Task(%d,%d)\n", __FUNCTION__, __LINE__, uWaitingCount, pBufferMgr->eChangeTaskState, pBufferMgr->eChangeTaskState);
		}
	}

	if (pChunked->stStreamAccess.b_alive != FALSE)
	{
		(void) STREAM_HTTP_Close ( &pChunked->stStreamAccess);
		pChunked->stStreamAccess.b_alive = FALSE;
	}

func_next:
	if (tHandle.pDnpUserData != NULL)
	{
		if (((DNP_USER_DATA_t *)tHandle.pDnpUserData)->p_align_buffer)
		{
			DD_MEM_Free(((DNP_USER_DATA_t *)tHandle.pDnpUserData)->p_align_buffer);
			((DNP_USER_DATA_t *)tHandle.pDnpUserData)->p_align_buffer = NULL;
		}

		DD_MEM_Free(tHandle.pDnpUserData);
		tHandle.pDnpUserData = NULL;
	}

	PrintExit;
	return vRet;
}

/** */
int DI_STREAM_HTTP_CHNK_Read (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize)
{
	size_t sz_Real=0;
	size_t sz_Try = size*nmemb;
	int nVfioErr = VFIO_ERROR_SUCCESS;
	HBOOL isRealtime = FALSE;
	HTTP_CHNK_t *pChunked = NULL;
	CHNK_BufferManager_t *pBufferMgr;
	off_t offset;
	DNP_USER_DATA_t *pDnpUserData = NULL;
	HUINT32 nNetworkRetryCount = 0;

	PrintEnter;
	UNUSED_PARAM (tHandle);

	*ptReadSize = 0;

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		return nVfioErr;
	}

	pBufferMgr = _CHNK_GetBufferManager(pChunked);
	if (pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL bufmgr!\n", __FUNCTION__, __LINE__);
		return nVfioErr;
	}

	_CHNK_Lock(pChunked);

	/**/
	/* DNP Tuner 와 Probe OPEN 인경우는 Real-time Reading */
	/**/
	if ((pBufferMgr->pBPool == NULL) || (pChunked->bOpenedForProbe == TRUE))
	{
		isRealtime = TRUE;
		PrintDebug("%s(%d) Info> Probe(%d) read\n", __FUNCTION__, __LINE__, pChunked->bOpenedForProbe);
		PrintDebug("%s(%d) Info> i_size(%lld) read\n", __FUNCTION__, __LINE__, pChunked->stStreamAccess.info.i_size);
		PrintDebug("%s(%d) Info> i_pos(%lld) read\n", __FUNCTION__, __LINE__, pChunked->stStreamAccess.info.i_pos);
		PrintDebug("%s(%d) Info> +sz_Try(%d) read\n", __FUNCTION__, __LINE__, sz_Try);
		if (pChunked->stStreamAccess.info.i_size < pChunked->stStreamAccess.info.i_pos + sz_Try)
		{
			sz_Try = pChunked->stStreamAccess.info.i_size - pChunked->stStreamAccess.info.i_pos;
		}
		PrintDebug("%s(%d) Info> -sz_Try(%d) read\n", __FUNCTION__, __LINE__, sz_Try);

		do
		{
			PrintDebug("%s(%d) --> +STREAM_HTTP_Read(in:%d) read\n", __FUNCTION__, __LINE__, (sz_Try - sz_Real));
			sz_Real += STREAM_HTTP_Read ( &pChunked->stStreamAccess, (HUINT8 *)ptr+sz_Real, sz_Try - sz_Real );
			PrintDebug("%s(%d) --> -STREAM_HTTP_Read(out:%d) read\n", __FUNCTION__, __LINE__, (sz_Real));
			if (pChunked->stStreamAccess.info.b_eof == TRUE )
			{
				PrintDebug("%s(%d) Info> Cursor reached in end of the file.\n", __FUNCTION__, __LINE__);
				break;
			}
		}
		while (sz_Real < sz_Try);
		*ptReadSize = sz_Real;

		/* Removed goto success; */
		PrintDebug("%s(%d) handle (0x%x) tryRead(%d) realRead(%d)\n", __FUNCTION__, __LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
		_CHNK_Unlock(pChunked);
		PrintExit;
		return nVfioErr;
	}


	/* Skipping the indexer Read : Real-time read from Indexer Connection */


	/* Playback Read : Buffered reading */
	if (1) /* (bOpenedForProbe == FALSE) */
	{
		//PrintDebug("%s(%d) Info> Playback read\n", __FUNCTION__, __LINE__);

		if (pBufferMgr->bNetworkError == TRUE)
		{
			PrintError("%s(%d) Error> Playback network error\n", __FUNCTION__, __LINE__);
			*ptReadSize = 0;
			nVfioErr = VFIO_ERROR_IN_DEVICE;
			goto error;
		}

		pDnpUserData = (DNP_USER_DATA_t *)tHandle.pDnpUserData;
		if (pDnpUserData == NULL)
		{
			PrintError("%s(%d) Error> pDnpUserData is NULL\n", __FUNCTION__, __LINE__);
			*ptReadSize = 0;
			nVfioErr = VFIO_ERROR_IN_DEVICE;
			goto error;
		}

		offset = pDnpUserData->player_last_seek_cursor;
		PrintDebug("%s(%d) player_last_seek_cursor[offset:%lld]\n", __FUNCTION__, __LINE__, offset);

		/* READ FROM ALIGN BUFFER FOR AVOID FREQUENTLY CLEAR THE BUFFER on Nexus Overlapped Request*/
		if (pDnpUserData->p_align_buffer)
		{
			HINT32 nDisplacement = (HINT32)(pDnpUserData->player_last_read_cursor + pDnpUserData->player_last_read_size - offset);

			PrintDebug("%s(%d) Displacement is %d[read(cursor:%lld, size:%lld), offset:%ld]\n",
						__FUNCTION__, __LINE__,
						nDisplacement, pDnpUserData->player_last_read_cursor,
						pDnpUserData->player_last_read_size, offset);
			if (pDnpUserData->align_buffer_start_index <= offset && offset <= pDnpUserData->align_buffer_end_index)
			{
				HINT32 nCopyableBytesCount = 0;
				//PrintDebug("Nexus Overlapped Request Detected. Use align buffer for avoid outside seek\n");
				nCopyableBytesCount = nDisplacement;
				if ((HINT32)sz_Try < nCopyableBytesCount)
				{
					nCopyableBytesCount = sz_Try;
				}
				PrintDebug("reuse start = (%"PRId64"), reuse end = (%"PRId64")\n", pDnpUserData->align_buffer_end_index - (nDisplacement - 1),
						pDnpUserData->align_buffer_end_index );
				if ( ((_NEXUS_ALIGN_BUFFER_SIZE - nDisplacement) >= 0) &&  ((_NEXUS_ALIGN_BUFFER_SIZE - nDisplacement) < _NEXUS_ALIGN_BUFFER_SIZE))
				{
					VK_memcpy(ptr, pDnpUserData->p_align_buffer + _NEXUS_ALIGN_BUFFER_SIZE - nDisplacement, nCopyableBytesCount);
					PrintDebug("Reused bytes from align buffer= %d\n", nCopyableBytesCount);

					offset += nCopyableBytesCount;
					sz_Real += nCopyableBytesCount;
				}
			}
		}

		/* SEEK inside Buffer */
		if ( (offset >= (off_t)pBufferMgr->illStartOffset) && (offset <= (off_t)pBufferMgr->illEndOffset) )
		{
			/* read till seek point */
			HINT64 prior_data_len = offset - pBufferMgr->illStartOffset;
			HUINT8 buffer[1024];

			PrintDebug("%s(%d) [prior_data_len:%lld, offset:%lld, illStartOffset:%lld]\n", __FUNCTION__, __LINE__, prior_data_len, offset, pBufferMgr->illStartOffset);
			if (prior_data_len > 1024)
			{
				PrintError("%s(%d) Too large[prior_data_len:%lld, offset:%lld, illStartOffset:%lld]\n", __FUNCTION__, __LINE__, prior_data_len, offset, pBufferMgr->illStartOffset);
				// What todo?
				;
			}

			while(prior_data_len > 0)
			{
				int readcount = 0;

				if (prior_data_len > 1024)
				{
					readcount = P_STREAM_CHNK_ReadBuffer(&pChunked->stStreamAccess, pBufferMgr, buffer, 1024);
				}
				else
				{
					readcount = P_STREAM_CHNK_ReadBuffer(&pChunked->stStreamAccess, pBufferMgr, buffer, prior_data_len);
				}

				if (prior_data_len < readcount)
				{
					PrintError("%s(%d) Too large[prior_data_len:%lld, readcount:%d]\n", __FUNCTION__, __LINE__, prior_data_len, readcount);
					// What todo?
				}

				prior_data_len -= readcount;
			}
		}
		else /* SEEK Outside Prebuffer */
		{
			PrintData("%s(%d) Error(info)> offset=%"PRId64", s(%"PRId64") e(%"PRId64") OUTSIDE SEEK..\n"
					,__FUNCTION__
					,__LINE__
					, offset
					,pBufferMgr->illStartOffset
					,pBufferMgr->illEndOffset);

			pChunked->stStreamAccess.info.b_prebuffered = TRUE;
			_BufferPool_Lock(pBufferMgr);
			pBufferMgr->illRequestedSeekOffset = offset; /* 64bit assign is not atomic. so protect it */
			_BufferPool_Unlock(pBufferMgr);

			nNetworkRetryCount = 0;
			while(TRUE)
			{
				nNetworkRetryCount++;
				_BufferPool_Lock(pBufferMgr);
				if (pBufferMgr->illRequestedSeekOffset == -1)
				{
					_BufferPool_Unlock(pBufferMgr);
					break;
				}
				_BufferPool_Unlock(pBufferMgr);

				if (pChunked->stStreamAccess.info.b_eof == TRUE)
				{
					PrintError("%s(%d) Error(info)> (End of Stream)\n", __FUNCTION__, __LINE__);
					*ptReadSize = 0;
					_BufferPool_Unlock(pBufferMgr);
#ifdef	CONFIG_TEMP_DTG_HDR2000T
					nVfioErr = 0; /* EOF! */
#else
					nVfioErr = VFIO_ERROR_IN_DEVICE; /* VFIO_ERROR_SUCCESS */
#endif
					goto error;
				}

				if (pBufferMgr->bNetworkError == TRUE)
				{
					*ptReadSize = 0;
					PrintError("%s(%d) Error> Network failed(%d)\n", __FUNCTION__, __LINE__, nNetworkRetryCount);
					_BufferPool_Unlock(pBufferMgr);
					nVfioErr = VFIO_ERROR_IN_DEVICE;
					goto error;
				}

				if (nNetworkRetryCount > (_CHNK_NETWORK_ERROR_RETRY_COUNT*2))
				{
					PrintError("%s(%d) Error> Too many retries(%d)\n", __FUNCTION__, __LINE__, nNetworkRetryCount);
					*ptReadSize = 0;
					pBufferMgr->bNetworkError = TRUE;
					_BufferPool_Unlock(pBufferMgr);
					nVfioErr = VFIO_ERROR_IN_DEVICE;
					goto error;
				}

				PrintDebug("%s(%d) Wait SEEK in SeekForNexus\n", __FUNCTION__, __LINE__); /* wait until seek is done */
				(void) VK_TASK_Sleep (50);
			}
		}

		/* READ 에서 return 을 안해주게 되면 NEXUS Playback 에서 waitingIO 가 걸린다. */
		sz_Real += P_STREAM_CHNK_ReadBuffer ( &pChunked->stStreamAccess, pBufferMgr, (HUINT8 *)ptr + sz_Real, sz_Try - sz_Real);
		*ptReadSize = sz_Real;

		pDnpUserData->player_last_read_cursor = pDnpUserData->player_last_seek_cursor;
		pDnpUserData->player_last_read_size = size * nmemb;
		pDnpUserData->player_last_seek_cursor += pDnpUserData->player_last_read_size; /* increase current seek cursor */

		PrintDebug("read cur = (%"PRId64"), size = %d\n", pDnpUserData->player_last_read_cursor, pDnpUserData->player_last_read_size);


		/* SAVE to ALIGN BUFFER */
		if (pDnpUserData->p_align_buffer)
		{
			size_t align_buffering_size = pDnpUserData->player_last_read_size;
			if (align_buffering_size > _NEXUS_ALIGN_BUFFER_SIZE)
			{
				align_buffering_size = _NEXUS_ALIGN_BUFFER_SIZE;
			}

			pDnpUserData->align_buffer_start_index = pDnpUserData->player_last_read_cursor + pDnpUserData->player_last_read_size - align_buffering_size;
			pDnpUserData->align_buffer_end_index = pDnpUserData->align_buffer_start_index + align_buffering_size - 1;
			if ( ((_NEXUS_ALIGN_BUFFER_SIZE - (int)align_buffering_size) >= 0)
					&&  ((_NEXUS_ALIGN_BUFFER_SIZE - (int)align_buffering_size) < _NEXUS_ALIGN_BUFFER_SIZE)
				&& (((pDnpUserData->player_last_read_size - (pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1)) + ((pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1))) <= sz_Try)
				&& ((pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1) + (_NEXUS_ALIGN_BUFFER_SIZE - align_buffering_size) <= _NEXUS_ALIGN_BUFFER_SIZE ))
			{
				VK_memcpy(pDnpUserData->p_align_buffer + _NEXUS_ALIGN_BUFFER_SIZE - align_buffering_size,
						(HUINT8*)ptr + pDnpUserData->player_last_read_size - (pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1),
						pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1);
			}

			PrintDebug("align buf start = (%"PRId64"), align buf end = (%"PRId64")\n", pDnpUserData->align_buffer_start_index, pDnpUserData->align_buffer_end_index);
			PrintDebug("buff size is %d\n", (HINT32)(pDnpUserData->align_buffer_end_index - pDnpUserData->align_buffer_start_index + 1));
		}
	}/* End Of PlayBack Reading */

error:

	PrintDebug("%s(%d) handle (0x%x) tryRead(%d) realRead(%d)\n", __FUNCTION__, __LINE__, tHandle.tFile.callback.unHandle,sz_Try, sz_Real);
	_CHNK_Unlock(pChunked);

	PrintExit;

	return nVfioErr;
}

/** This Function called by bmedia probe only.*/
int DI_STREAM_HTTP_CHNK_Seek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HINT32 nFailCounter = 0;
	HTTP_CHNK_t *pChunked = NULL;

	PrintEnter;
	UNUSED_PARAM (tHandle);
	PrintData("[%s:%d] : offset=%"PRId64", whence=%d\n", __FUNCTION__, __LINE__, offset, whence);

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		PrintExit;
		return VFIO_ERROR_SUCCESS; /* <---- */
	}

	_CHNK_Lock(pChunked);

	if (whence == SEEK_END)
	{
		offset = (off_t)pChunked->stStreamAccess.info.i_size;
		_CHNK_Unlock(pChunked);
		return VFIO_ERROR_SUCCESS;
	}

	if (pChunked->stStreamAccess.info.i_pos == offset)
	{
		PrintExit;
		_CHNK_Unlock(pChunked);
		return VFIO_ERROR_SUCCESS;
	}

	if (pChunked->bSupportSeek == TRUE)
	{
		nFailCounter = 0;
		while(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( &pChunked->stStreamAccess, offset ))
		{
			nFailCounter++;
			PrintError("%s(%d) Error> STREAM_HTTP_Seek fail Retrying...\n", __FUNCTION__, __LINE__);

			if (nFailCounter > _CHNK_SEEK_RETRY_COUNT)
			{
				PrintError("%s(%d) Error> STREAM_HTTP_Seek fail count exceed threadsholuld. Return Error...\n", __FUNCTION__, __LINE__);
				_CHNK_Unlock(pChunked);
				PrintExit;
				return -1;
			}
		}
	}
	else
	{
		PrintError("%s(%d) Error> Can't support seek()!\n", __FUNCTION__, __LINE__);
	}

	_CHNK_Unlock(pChunked);
	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/** */
int DI_STREAM_HTTP_CHNK_Tell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	HTTP_CHNK_t *pChunked = NULL;

	PrintEnter;
	UNUSED_PARAM (tHandle);

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		*plCurOffset = 0;
		PrintExit;
		return nRet;
	}

	*plCurOffset = pChunked->stStreamAccess.info.i_pos;
	PrintDebug("%s(%d) i_pos (%"PRId64") nRet(%d)\n", __FUNCTION__, __LINE__,*plCurOffset, nRet);
	PrintExit;
	return nRet;
}

/** if EOF is true, must be return -1 like feof*/
int DI_STREAM_HTTP_CHNK_IsEOF (VFIO_Handle_t tHandle)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	HTTP_CHNK_t *pChunked = NULL;

	PrintEnter;
	UNUSED_PARAM (tHandle);

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		nRet = -1;
		goto func_error;
	}

	if (TRUE == pChunked->stStreamAccess.info.b_eof)
	{
		nRet = -1; /* must be -1 like feof*/
	}

func_error:
	PrintExit;
	return nRet;
}

/* Open From Nexus */
int DI_STREAM_HTTP_CHNK_OpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	HINT32 nRet = DI_IPT_SUCCESS;
	HTTP_ACCESS_SYS_t *pstHttpSys=NULL;
	DI_IPTUNER_Msg_t tIPTunerMsg;
	HTTP_CHNK_t *pChunked = NULL;
	CHNK_BufferManager_t *pBufferMgr;
	DNP_USER_DATA_t * pDnpUserData;

	PrintEnter;
	UNUSED_PARAM (flags);

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		nRet = VFIO_ERROR_IN_DEVICE;
		PrintExit;
		return nRet;
	}

	pBufferMgr = _CHNK_GetBufferManager(pChunked);
	if (pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL bufmgr!\n", __FUNCTION__, __LINE__);
		nRet = VFIO_ERROR_IN_DEVICE;
		PrintExit;
		return nRet;
	}

	/* Stream Buffering 시 Probe 를 위한 Open 과 Playback을 위한 Open 을 구분하기 위한 용도 */
	pChunked->bOpenedForProbe = FALSE;

	if (1)
	{
		pDnpUserData = DD_MEM_Alloc(sizeof(DNP_USER_DATA_t));
		if (pDnpUserData == NULL)
		{
			PrintError("%s(%d) Error> Out of memory!\n", __FUNCTION__, __LINE__);
			nRet = VFIO_ERROR_IN_DEVICE;
			PrintExit;
			return nRet;
		}

		pChunked->iOpenFromNexusCounter++;
		pDnpUserData->nPurpose = pChunked->iOpenFromNexusCounter; /* don't use non-zero counter value like 2(open for indexer) */
		pDnpUserData->indexer_last_seek_cursor = 0;
		pDnpUserData->p_align_buffer = NULL;
		pDnpUserData->align_buffer_start_index = -1;
		pDnpUserData->align_buffer_end_index = -1;
		ptHandle->pDnpUserData = pDnpUserData;
	}

	pChunked->stStreamAccess.psz_path = (char *)VK_StrDup ((char*) path);
	if (pChunked->stStreamAccess.psz_path == NULL)
	{
		PrintError("%s(%d) Error> Out of memory!\n", __FUNCTION__, __LINE__);
		ptHandle->ucAvailable = 0;

		if (pChunked->stStreamAccess.psz_path != NULL)
		{
			DD_MEM_Free(pChunked->stStreamAccess.psz_path);
			pChunked->stStreamAccess.psz_path =NULL;
		}
		nRet = VFIO_ERROR_IN_DEVICE;
		PrintExit;
		return nRet;
	}

	nRet = STREAM_HTTP_Open (&pChunked->stStreamAccess);
	if (nRet != DI_IPT_SUCCESS)
	{
		PrintError("%s(%d) Error> STREAM_HTTP_Open Fail.\n", __FUNCTION__, __LINE__);
		ptHandle->ucAvailable = 0;

		if (pChunked->stStreamAccess.psz_path != NULL)
		{
			DD_MEM_Free(pChunked->stStreamAccess.psz_path);
			pChunked->stStreamAccess.psz_path =NULL;
		}

		nRet = VFIO_ERROR_IN_DEVICE;
		PrintExit;
		return nRet;
	}

	pstHttpSys = (HTTP_ACCESS_SYS_t *)pChunked->stStreamAccess.p_sys;
	ptHandle->tFile.callback.unHandle = (HUINT32) pstHttpSys->fd;
	ptHandle->ucAvailable = 1;
	PrintData("%s(%d) callback handle=0x%x\n", __FUNCTION__, __LINE__, ptHandle->tFile.callback.unHandle);

	nRet = VK_SEM_Create(&pChunked->ulSemId,"chnk_sema", DEFAULT_SUSPEND_TYPE);
	if ( nRet != VK_OK )
	{
		PrintError("%s(%d) Error> SEM_Create() failed (0x%x) \n\r", __FUNCTION__, __LINE__, nRet);

		(void)STREAM_HTTP_Close (&pChunked->stStreamAccess);
		pChunked->stStreamAccess.b_alive = FALSE;

		if (pChunked->stStreamAccess.psz_path != NULL)
		{
			DD_MEM_Free(pChunked->stStreamAccess.psz_path);
			pChunked->stStreamAccess.psz_path =NULL;
		}

		PrintExit;
		return VFIO_ERROR_IN_DEVICE;
	}

	if (pChunked->bUseBufferPool == TRUE)
	{
		if (pBufferMgr->pBPool != NULL)
		{
			PrintData("%s(%d) Buffering Start from OpenNexusCallback\n", __FUNCTION__, __LINE__);
			(void) VK_memset(&tIPTunerMsg, 0x0, sizeof(tIPTunerMsg));
			tIPTunerMsg.unMsg = TUNER_STREAM_MSG_HTTP_CHNK_BUFFERING; /* buffering pool for HTTP chunked_live_streaming */
			tIPTunerMsg.unParam1 = DI_IPTUNER_STREAM_HTTP_CHNK;
			(void) VK_MSG_Send ( s_ulStreamMsgQId, (HUINT32 *)&tIPTunerMsg, sizeof(DI_IPTUNER_Msg_t) ) ;
		}
		else
		{
			PrintError("%s(%d) Error> Buffering Start from OpenNexusCallback\n", __FUNCTION__, __LINE__);
		}

		pDnpUserData->p_align_buffer = DD_MEM_Alloc(_NEXUS_ALIGN_BUFFER_SIZE);
		if (pDnpUserData->p_align_buffer == NULL)
		{
			PrintError("%s(%d) Error> p_align_buffer Mem alloc error\n", __FUNCTION__, __LINE__);

			if (pChunked->stStreamAccess.psz_path != NULL)
			{
				DD_MEM_Free(pChunked->stStreamAccess.psz_path);
				pChunked->stStreamAccess.psz_path =NULL;
			}

			PrintExit;
			return VFIO_ERROR_IN_DEVICE;
		}
	}

	PrintExit;
	return VFIO_ERROR_SUCCESS;
}

/* Seek From Nexus : Long jump 인 경우 어느정도는 block 되어도 된다.
 * 단 Read 함수에서는 block 되면 waiting IO error, 0 return 시 endofStream, -1 return시 errorofstream 이 뜬다.*/
off_t DI_STREAM_HTTP_CHNK_SeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	HUINT32 nNetworkRetryCount = 0;
	HTTP_CHNK_t *pChunked = NULL;
	CHNK_BufferManager_t *pBufferMgr;

	PrintEnter;
	UNUSED_PARAM (whence);

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL callback\n", __FUNCTION__, __LINE__);
		return -1;
	}

	pBufferMgr = _CHNK_GetBufferManager(pChunked);
	if (pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL bufmgr!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	_CHNK_Lock(pChunked);

	/* null check */
	if (tHandle.pDnpUserData == NULL)
	{
		PrintError("%s(%d) Error> pDnpUserData is NULL\n", __FUNCTION__, __LINE__);
		_CHNK_Unlock(pChunked);
		return -1;
	}

	/* Realtime SeekForNexus */
	if (pChunked->bUseBufferPool == FALSE)
	{
		if (pChunked->stStreamAccess.info.i_pos != offset)
		{
			/* Currently seek() is unsupported; Return the arg(offset) */
			if (pChunked->bSupportSeek == TRUE)
			{
				PrintData("%s(%d) handle(0x%x) offset=%"PRId64", whence=%d\n", __FUNCTION__, __LINE__, tHandle.tFile.callback.unHandle, offset, whence);

				nNetworkRetryCount = 0;
				while(DI_IPT_SUCCESS != STREAM_HTTP_Seek ( &pChunked->stStreamAccess, offset ))
				{
					nNetworkRetryCount++;
					PrintError("%s(%d) Error> STREAM_HTTP_Seek fail Retrying...\n", __FUNCTION__, __LINE__);
					VK_TASK_Sleep(100);
					if (nNetworkRetryCount > _CHNK_NETWORK_ERROR_RETRY_COUNT)
					{
						PrintError("%s(%d) Error> Too many retries(%d)\n", __FUNCTION__, __LINE__, nNetworkRetryCount);
						pBufferMgr->bNetworkError = TRUE;
						offset = (off_t)-1;
						break;
					}
				}
			}
			else
			{
				PrintError("%s(%d) Error> unsupported seek! handle(0x%x) offset=%"PRId64", whence=%d\n", __FUNCTION__, __LINE__, tHandle.tFile.callback.unHandle, offset, whence);
			}
		}

		//	PrintExit;
		_CHNK_Unlock(pChunked);
		return offset;
	}

	/* Dont care about Indexer SeekForNexus */

	/* Player SeekForNexus */
	PrintDebug("%s(%d) handle(0x%x) offset=%"PRId64", whence=%d\n", __FUNCTION__, __LINE__, tHandle.tFile.callback.unHandle, offset, whence);

	/* calc absolute offset */
	if ( SEEK_CUR == whence )
	{
		//PrintError("%s(%d) Error> == Seek CUR\n", __FUNCTION__, __LINE__);
		((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor += offset;
	}
	else if ( SEEK_SET == whence )
	{
		//PrintError("%s(%d) Error> == Seek SET\n", __FUNCTION__, __LINE__);
		((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor = offset;
	}
	else if ( SEEK_END == whence )
	{
		((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor = pChunked->stStreamAccess.info.i_size - 1;
	}
	else
	{
		PrintError("%s(%d) Error> Invalid whence Parameter %d\n", __FUNCTION__, __LINE__, whence);
		((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor = offset;
	}

	/* bound check */
	if (((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor > pChunked->stStreamAccess.info.i_size - 1)
	{
		((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor = pChunked->stStreamAccess.info.i_size - 1;
	}

	_CHNK_Unlock(pChunked);
	PrintDebug("%s(%d) handle(0x%x) offset=%"PRId64"\n", __FUNCTION__, __LINE__, tHandle.tFile.callback.unHandle, ((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor);
	return ((DNP_USER_DATA_t *)tHandle.pDnpUserData)->player_last_seek_cursor;
}

/* Bounds From Nexus */
int DI_STREAM_HTTP_CHNK_BoundsForNexus (VFIO_Handle_t tHandle, off_t *file_size)
{
	HINT32 nRet = VFIO_ERROR_SUCCESS;
	HTTP_CHNK_t *pChunked = NULL;

	//PrintEnter;
	UNUSED_PARAM (tHandle);

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL handle\n", __FUNCTION__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto func_error;
	}

	*file_size = pChunked->stStreamAccess.info.i_size;
	//PrintData("%s(%d) i_size=%"PRId64"\n", __FUNCTION__, __LINE__, pChunked->stStreamAccess.info.i_size);
	//PrintExit;
	return nRet;

func_error:
	*file_size = 0;
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_STREAM_HTTP_CHNK_RegisterBufferingCallback (DI_IPTUNER_PlayCallback_t *pCallback)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HTTP_CHNK_t *pChunked = NULL;

	PrintEnter;

	if (pCallback == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL callback\n", __FUNCTION__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto func_error;
	}

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL callback\n", __FUNCTION__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto func_error;
	}
	VK_memcpy(&pChunked->stHttpBufferingCallback, pCallback, sizeof(DI_IPTUNER_PlayCallback_t));

func_error:
	PrintExit;
	return nRet;
}

void DI_STREAM_HTTP_CHNK_UnregisterBufferingCallback (void)
{
	HTTP_CHNK_t *pChunked = NULL;

	PrintEnter;

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL callback\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	VK_memset(&pChunked->stHttpBufferingCallback, 0x0, sizeof(DI_IPTUNER_PlayCallback_t));

func_error:
	PrintExit;
	return;
}

static void P_STREAM_CHNK_SetDefaultBufferPool(CHNK_BufferManager_t *pBufferMgr) /* Don't allocate; Just fill default-set */
{
	if (pBufferMgr == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL bufmgr!\n", __FUNCTION__, __LINE__);
		return;
	}

	pBufferMgr->pBPool = NULL; /* BufferPool will be allocated by DI_IPTUNER_StreamOpen() */
	pBufferMgr->uBPoolTotalSize = _PREBUFFERING_POOL_SIZE;

	pBufferMgr->iStartPos = 0;
	pBufferMgr->iEndPos = 0;
	pBufferMgr->pWritePtr = NULL;
	pBufferMgr->pReadPtr = NULL;
	pBufferMgr->illStartOffset = 0;
	pBufferMgr->illEndOffset = 0;
	pBufferMgr->iHighThreshold = 0;

	pBufferMgr->ulLock = 0;
	pBufferMgr->illRequestedSeekOffset = -1;
	pBufferMgr->eTaskState = CHNK_PREBUFFERING_TASK_STATE_DONE;
	pBufferMgr->eChangeTaskState = CHNK_PREBUFFERING_TASK_STATE_DONE;
	pBufferMgr->bNetworkError = FALSE;
	return;
}

static void P_STREAM_CHNK_FillDefaultSet(HTTP_CHNK_t *pChunked) /* Don't allocate; Just fill default-set */
{
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL handle!\n", __FUNCTION__, __LINE__);
		return;
	}
	pChunked->bUseBufferPool = TRUE;
	pChunked->bSupportSeek = FALSE;
	pChunked->bOpenedForProbe = TRUE;
	pChunked->iOpenFromNexusCounter = 0;
	pChunked->iRegisterCounter = 0;
	pChunked->ulSemId = 0;

	VK_memset(&pChunked->stStreamAccess, 0x0, sizeof (STREAM_ACCESS_t));
	VK_memset(&pChunked->stHttpBufferingCallback, 0x0, sizeof(DI_IPTUNER_PlayCallback_t));
	pChunked->bufmgr = NULL;

	return;
}

static HTTP_CHNK_t *P_STREAM_CHNK_OpenHandle(void)
{
	HTTP_CHNK_t *pChunked = NULL;

	PrintEnter;

	pChunked = (HTTP_CHNK_t *)DD_MEM_Alloc(sizeof(HTTP_CHNK_t));
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Out of memory!\n", __FUNCTION__, __LINE__);
		PrintExit;
		return NULL;
	}

	VK_memset(pChunked, 0, sizeof(HTTP_CHNK_t));
	P_STREAM_CHNK_FillDefaultSet(pChunked);

	if (pChunked->bUseBufferPool == TRUE)
	{
		pChunked->bufmgr = (CHNK_BufferManager_t *)DD_MEM_Alloc(sizeof(CHNK_BufferManager_t));
		if (pChunked->bufmgr == NULL)
		{
			PrintError("%s(%d) Error> Out of memory!\n", __FUNCTION__, __LINE__);
			if (pChunked != NULL)
			{
				DD_MEM_Free(pChunked);
				pChunked = NULL;
			}
			PrintExit;
			return NULL;
		}
		P_STREAM_CHNK_SetDefaultBufferPool(pChunked->bufmgr);
	}
	PrintExit;
	return pChunked;
}

void DI_STREAM_HTTP_CHNK_RegisterCallback (void)
{
	VFIO_Callback_t stCBFunction;
	HTTP_CHNK_t *pChunked = NULL;

	PrintEnter;

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset(&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			= (VFIOOpenCB)	DI_STREAM_HTTP_CHNK_Open;
	stCBFunction.Close			= (VFIOCloseCB)	DI_STREAM_HTTP_CHNK_Close;
	stCBFunction.Read			= (VFIOReadCB)	DI_STREAM_HTTP_CHNK_Read;
	stCBFunction.Seek			= (VFIOSeekCB) 	DI_STREAM_HTTP_CHNK_Seek;
	stCBFunction.Tell			= (VFIOTellCB) 	DI_STREAM_HTTP_CHNK_Tell;
	stCBFunction.IsEOF			= (VFIOIsEOFCB) DI_STREAM_HTTP_CHNK_IsEOF;
	stCBFunction.OpenNexus		= (VFIOOpenFromNexusCB)	 DI_STREAM_HTTP_CHNK_OpenFromNexus;
	stCBFunction.SeekNexus		= (VFIOSeekForNexusCB) 	 DI_STREAM_HTTP_CHNK_SeekForNexus;
	stCBFunction.BoundsNexus	= (VFIOBoundsForNexusCB) DI_STREAM_HTTP_CHNK_BoundsForNexus;

	(void) VFIO_RegisterCallback (VFIO_CALLBACK_TYPE_STREAMING_HTTP, stCBFunction);
	pChunked = P_STREAM_CHNK_OpenHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	_ghHttpChunkedTE = pChunked;

func_error:
	PrintExit;
	return;
}

static void P_STREAM_CHNK_ReleaseHandle(HTTP_CHNK_t *pChunked)
{
	PrintEnter;

	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	else
	{
		if (pChunked->bufmgr != NULL)
		{
			/* BufferPool will be deallocated by DI_IPTUNER_StreamClose() */
			DD_MEM_Free(pChunked->bufmgr);
			pChunked->bufmgr = NULL;
		}

		DD_MEM_Free(pChunked);
		pChunked = NULL;
	}

func_error:
	PrintExit;
	return;
}

void DI_STREAM_HTTP_CHNK_UnregisterCallback (void) /* Called by DI_IPTUNER_StreamClose() */
{
	HTTP_CHNK_t *pChunked = NULL;

	PrintEnter;

	(void)VFIO_UnregisterCallback (VFIO_CALLBACK_TYPE_STREAMING_HTTP);

	pChunked = _CHNK_GetHandle();
	if (pChunked == NULL)
	{
		PrintError("%s(%d) Error> Invalid NULL chunked handle!\n", __FUNCTION__, __LINE__);
		goto func_error;
	}
	else
	{
		P_STREAM_CHNK_ReleaseHandle(pChunked);
	}

func_error:
	PrintExit;
	return;
}

