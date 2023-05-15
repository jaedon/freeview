/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_dnp.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		SungYong Jeon $
 * Current Author:	$Author: junsy@humaxdigital.com $
 * Date:			$Date: Mon Apr 20 10:20:48 KST 2009 $
 * File Description:	Download & Play Tuner Body
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
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <inttypes.h>

#include "bstd.h"
#include "vfio.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_media.h"
#include "di_uart.h"

#include "util_url.h"
#include "di_iptuner.h"
#include "di_iptuner_config.h"
#include "di_iptuner_dnp.h"
#include "di_iptuner_http.h"
#include "di_iptuner_mms.h"
#include "di_iptuner_underflow.h"

/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */

//#define DI_IPTUNER_DNP_DEBUG
//#define DI_IPTUNER_DNP_CRITICAL_DEBUG
#ifdef IPTUNER_ALL_DEBUG

#ifdef DI_IPTUNER_DNP_DEBUG
//#define DEBUG_IO_FUNCTION
#define DI_IPTUNER_DNP_ENABLE_INTEGRITY_CHECK_COMPARING_WITH_LOCAL_FILE 0
#define DI_IPTUNER_DNP_INTEGRITY_CHECK_COMPARING_WITH_LOCAL_FILE_NAME "/mnt/hd2/avatar_hd.wmv"
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#define PrintError		DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#define PrintError		DI_UART_Print
#endif

#else

#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug		while (0) ((int (*)(char *, ...)) 0)
#define PrintError		while (0) ((int (*)(char *, ...)) 0)
#define PrintEnter
#define PrintExit
#endif

#define UNUSED_PARAM(x)		((void)x)


//#define	TUNER_DNP_STACK_SIZE				(32*1024)
#define	TUNER_DNP_STACK_SIZE					(256*1024)	/* 256Kbytes */
#define	TUNER_DNP_TASK_PRIORITY					(10)
#define	TUNER_DNP_MSG_QUEUE_SIZE				(10)
#define	TUNER_DNP_WAITEVTTIME					(60000)		/* 3000msec = 3sec */
#define	TUNER_DNP_CALLBACK_WAITCOUNT			(100000)		/* 20 * 100msec = 2sec */
#define TUNER_DNP_PROBE_SEEK_CALLBACK_WAITCOUNT (100000)	/* 100000 times X TUNER_DNP_CALLBACK_WAITTTIME */
#define	TUNER_DNP_SEEK_CALLBACK_WAITCOUNT		(100000)	/* 100000 times X TUNER_DNP_CALLBACK_WAITTTIME */
#define	TUNER_DNP_CALLBACK_WAITTTIME			(100)		/* 100msec = 0.1sec */
#define	TUNER_DNP_MSGQ_TIMEOUT				(500)


#define MAX_IP_SIZE			(128)	/** bytes */
#define MAX_PORT_SIZE		(16)	/** bytes */
#define MAX_FILE_NAME_SIZE	(IPTUNER_CONST_URL_LENGTH)	/** bytes */ /** FIXME : will be extended for URL size */


#define MAX_PDL_BUFFERING_PERCENT 99999

/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
static HULONG s_ulDNPTaskId;
static HULONG s_ulDNPMsgQId;
static HULONG s_ulDNPSema;
static HULONG s_ulDNP_ReadSema;
static VFIO_Handle_t stHandle;

/* End typedef */


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */

#ifdef ENABLE_MAPPED_DNP
/* PROGRESSIVE DOWNLOAD GLOBAL VARIABLES */
/* all of main data structure use 32bit variable for atomic set/fetch */
HINT32 g_nchunkoffPrdnEof;
HINT32 g_nchunkoffOldPrdnRequestedSeekPoint;
HINT32 g_nchunkoffPrdnRequestedSeekPoint;
HUINT8 *g_pucPrdnChunkDownloadStatusArray;

static HUINT64	s_ullDataReadOffset;
static HUINT64	s_ullLastReadSize;
static HINT32	s_nDownloadBytesPerSecond;
static HINT32	s_nArrayDownloadSpeed[DNP_DOWNLOAD_SPEED_COUNT];
static HINT32	s_nDownloadSpeedIndex;

HBOOL g_bPrdnForwardPlay;
#if 1	/* JWKIM, workaround: for Maxdome Service */
HBOOL g_bPrdnPlayFFTrick;
#endif
HBOOL g_bPrebufferingRequested;
HBOOL g_bPrdnDownCompleted;
HINT32 g_nchunkoffIsCompletedStart; /* avoid recheck valid region */

/* for PRDN INFO */
HINT32 g_nchunkoffCurrentReadPoint;
HINT32 g_nchunkoffLastPrebufferIdx;
HINT32 g_nPrdnInfoBufferingPercent;
HINT32 g_nPrdnInfoBufferingLeftTime; /* left sec that user must wait */
HUINT32 g_nPrdnInfoBufferingTime; /* content milisec to be buffered */
HINT32 g_nPrdnInfoDownloadChunkPerSecSpeed; /* chunk per sec */
HINT32 g_nPrdnInfoDownloadedChunkCount;
HINT32 g_nPrdnInfoBufferSizeInChunkCount;
fn_readystate_callback g_pReadyStateCallbackFunc = NULL;
HUINT32 g_nPrdnCBulDeviceId;
HULONG g_nPrdnInfoLastReadyStateSentTime;
#endif

#if DI_IPTUNER_DNP_ENABLE_INTEGRITY_CHECK_COMPARING_WITH_LOCAL_FILE
int gFdForDnpIntegrityCheck;
#endif
/* End global variable */


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
static HUINT8					s_ucQuit = FALSE;
static HUINT8					s_ucIsInitialized = FALSE;
static HBOOL					s_bDnpTaskFinished = FALSE;
/* TODO :: 2G 이상 file 지원 */
static HINT64					s_nFileLength = 0;
/* TODO :: int -> char ??? */

static URL_t					stURL;
static unsigned long 		s_ulTunerEventID;
static HBOOL 				s_bIsPlaybackStop=FALSE;
/* End static variable */





/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/

void P_IPTUNER_DnpSetCurrentDataReadOffset(HUINT64 ullCurrentDataReadOffset);
void P_IPTUNER_DnpSetLastReadSize(HUINT64 ullReadSize);
DI_ERR_CODE P_IPTUNER_DnpCheckUnderflow(HBOOL *bUnderflow, HUINT32 ulDuration);


#if __________task__________
#endif
static void IPTUNER_DnpTask (void *pParam)
{
	DI_IPTUNER_Msg_t s_Msg;
	HINT32 nVKRet;

	PrintEnter;
	s_bDnpTaskFinished = FALSE;

	UNUSED_PARAM (pParam);


	while ( TRUE != s_ucQuit )
	{
		nVKRet = VK_MSG_ReceiveTimeout (s_ulDNPMsgQId, &s_Msg, sizeof(DI_IPTUNER_Msg_t), TUNER_DNP_MSGQ_TIMEOUT);
		if ( nVKRet != VK_OK )
		{
			continue;
		}
		PrintDebug("%s(%d) =================== RECEIVE START  ======================\n",__FUNCTION__,__LINE__);
		/* else { continue } */

		if ( s_Msg.unMsg == TUNER_DNP_MSG_HTTP_PROBE )
		{
			PrintDebug ("%s(%d) : TUNER_DNP_MSG_HTTP_PROBE Received\n",__FUNCTION__,__LINE__);
			/* for test */
			/* TODO :: */
			(void) VK_SEM_Get(s_ulDNPSema);
			/* Data Read & Write */
			(void) DI_STREAM_HTTP_DnpReceive (stHandle);
			(void) VK_SEM_Release (s_ulDNPSema );
		}
		else if (s_Msg.unMsg == TUNER_DNP_MSG_MMS_PROBE)
		{
			PrintDebug ("%s(%d) : TUNER_DNP_MSG_MMS_PROBE Received\n",__FUNCTION__,__LINE__);
			(void) VK_SEM_Get(s_ulDNPSema);
			/* Data Read & Write */
			(void) DI_STREAM_MMS_DnpReceive (stHandle);
			(void) VK_SEM_Release (s_ulDNPSema);

		}
#if 0 /** XXX : in the future */
		else if (s_Msg.unMsg == TUNER_DNP_MSG_RTSP_PROBE)
		{
			(void) VK_SEM_Get(s_ulDNPSema);
			/* Data Read & Write */
			(void) DI_STREAM_RTSP_DnpReceive ();
			(void) VK_SEM_Release (s_ulDNPSema );

		}
		else if (s_Msg.unMsg == TUNER_DNP_MSG_FTP_PROBE)
		{
			(void) VK_SEM_Get(s_ulDNPSema);
			/* Data Read & Write */
			(void) DI_STREAM_FTP_DnpReceive ();
			(void) VK_SEM_Release (s_ulDNPSema );
			/* TODO :: */
			PrintError ("[%s] [%d] Error - not supported (FTP)\n",__FUNCTION__,__LINE__);
		}
#endif
		/* else { continue } */
	}


	/*s_ucQuit = TRUE;*/
	s_bDnpTaskFinished = TRUE;

	PrintExit;
	return;
}

#if __________p__________
#endif
static HINT32 P_IPTUNER_DnpInitThread (void)
{
	DI_IPTUNER_Msg_t s_Msg;
	HINT32 nVKRet;

	PrintEnter;

	if ( TRUE != s_ucIsInitialized )
	{
/* TODO : check */
		/* create & init message que */

		nVKRet = VK_MSG_Create(TUNER_DNP_MSG_QUEUE_SIZE, sizeof(s_Msg), "DNP_MSGQ", &s_ulDNPMsgQId, VK_SUSPENDTYPE_FIFO);
		if ( VK_OK != nVKRet )
		{
			PrintError ("[%s] [%d] DNP Task msgQ creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}	/* Semaphore for shared data between main_task and timer_task */


		nVKRet = VK_SEM_Create(&s_ulDNPSema, "DNPTunerSema", VK_SUSPENDTYPE_PRIORITY);
		if ( VK_OK != nVKRet )
		{
			PrintError ("[%s] [%d] DNP Timer sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}



		nVKRet = VK_SEM_Create(&s_ulDNP_ReadSema,  "DNPTunerReadSema", VK_SUSPENDTYPE_PRIORITY);
		if ( VK_OK != nVKRet )
		{
			PrintError ("[%s] [%d] DNP Read sema creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
			return -1;
		}

		s_ucQuit = FALSE;

		/** Create Download Task */
		nVKRet = VK_TASK_Create( IPTUNER_DnpTask
							, DOWNLOAD_TASK_PRIORITY	// temp priority TODO:
							, TUNER_DNP_STACK_SIZE
							, "DNP_Tuner"
							, NULL
							, &s_ulDNPTaskId
							, TRUE);
		if ( VK_OK != nVKRet )
		{
			PrintError ("%s (%d) Error> DNP Tuner task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
			return -1;
		}
		else
		{
			nVKRet = VK_EVENT_Create (&s_ulTunerEventID, "DNPTunerEvent");
			if ( VK_OK != nVKRet )
			{
				PrintError("%s (%d) Error> Error(0x%x) in VK_EVENT_Create()\n\r", __FUNCTION__,__LINE__,nVKRet);
				return -1;
			}
			else
			{
				s_ucIsInitialized = TRUE;
			}
		}

		/** Start Download Task */
		VK_TASK_Start(s_ulDNPTaskId);
	}
	else
	{
		PrintError  ("%s (%d) Error>  Already initialized !\n",__FUNCTION__,__LINE__);
	}

	PrintExit;

	return 0;
}


/** VFIO (for DNP) Open Function */
static int P_IPTUNER_DnpOpen (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle)
{
	int		ret=VFIO_ERROR_SUCCESS;
	int		fd;

	UNUSED_PARAM (mode);
	PrintEnter;

//	PrintData ("[%s] [%d] path=%s, mode=%s\n",__FUNCTION__,__LINE__, path, mode);
	if(ptHandle == NULL)
	{
		PrintError("ptHandle is NULL\n");
		ret = VFIO_ERROR_BAD_PARAMETER;
		goto error;
	}

	/* error check */
	if(stHandle.pDnpUserData == NULL)
	{
		PrintError("pDnpUserData is NULL\n");
		ptHandle->ucAvailable = 0;
		ret = VFIO_ERROR_IN_DEVICE;
		goto error;
	}

	ptHandle->pDnpUserData = stHandle.pDnpUserData;



	fd = open ( DI_IPTUNER_DNP_FILE_PATH , O_RDONLY|O_SYNC, 0666);
	if ( fd < 0 )
	{
		ptHandle->ucAvailable = 0;
		ret = VFIO_ERROR_IN_DEVICE;
		PrintError ("%s (%d) Error>  %s open Error, please check file path\n",__FUNCTION__,__LINE__,path);
	}
	else
	{
		ptHandle->ucAvailable = 1;
		ptHandle->tFile.callback.unHandle = (HUINT32)fd;
		PrintData ("%s (%d) callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
	}
error:

	PrintExit;

	return ret;
}

/** VFIO (for DNP) Close Function */
static int P_IPTUNER_DnpClose (VFIO_Handle_t tHandle)
{
	int		ret=VFIO_ERROR_SUCCESS;

	PrintEnter;

	tHandle.ucAvailable = 0;
	(void)close ((int)tHandle.tFile.callback.unHandle);

	PrintExit;

	return ret;
}

/** VFIO (for DNP) Read Function */
static int P_IPTUNER_DnpRead (VFIO_Handle_t tHandle, void *ptr, size_t size, size_t nmemb, size_t *ptReadSize)
{
	int		ret=VFIO_ERROR_SUCCESS;
	ssize_t	readCnt=0;
	size_t	unRequestSize=(size*nmemb);
	int		nLoopCnt=0;
	struct stat	sb;
	off_t		cur=0;
	off_t		pre_read_cur=0;

	(void) VK_SEM_Get(s_ulDNP_ReadSema);
#ifdef DEBUG_IO_FUNCTION
	PrintEnter;
	PrintData ("%s (%d) size=%d, nmemb=%d\n",__FUNCTION__,__LINE__, size, nmemb);
#endif

	*ptReadSize = 0;
	cur = lseek((int)tHandle.tFile.callback.unHandle, 0, SEEK_CUR);
	pre_read_cur = cur;
	if ( (off_t)-1 == cur )
	{
		PrintError ("%s (%d) Error>  lseek()\n",__FUNCTION__,__LINE__);
		ret = VFIO_ERROR_IN_DEVICE;
		*ptReadSize = 0;
	}
	else
	{
		//PrintDebug ("%s (%d) cur=%"PRId64" reqSize(%d)\n",__FUNCTION__,__LINE__, cur,unRequestSize);
		while ( nLoopCnt++ < TUNER_DNP_CALLBACK_WAITCOUNT )
		{
			if(s_bIsPlaybackStop==TRUE)
			{
				PrintError ("%s (%d) Error> read() is stopped by user.(stop event).\n",__FUNCTION__,__LINE__);
				ret = VFIO_ERROR_IN_DEVICE;
				*ptReadSize = 0;
				break;
			}

			if (fstat((int)tHandle.tFile.callback.unHandle, &sb)<0)
			{
				PrintError ("%s (%d) Error> fstat()\n",__FUNCTION__,__LINE__);
				ret = VFIO_ERROR_IN_DEVICE;
				break;
			}
			else
			{
#ifdef ENABLE_MAPPED_DNP
				if(cur >= s_nFileLength) /* bound check */
				{
					PrintError ("%s (%d) Error> cur is beyond the file length.\n",__FUNCTION__,__LINE__);
					ret = VFIO_ERROR_BAD_PARAMETER;
					*ptReadSize = 0;
					break;
				}

				if(tHandle.pDnpUserData == NULL)
				{
					PrintError("pDnpUserData is NULL\n");
					ret = VFIO_ERROR_IN_DEVICE; /* this value is transformed to 0(EOF) in b_stdio_read */
					break;
				}
				if(g_bPrdnForwardPlay == FALSE && ((DNP_USER_DATA_t *)tHandle.pDnpUserData)->unOpenFromNexusHandle == tHandle.tFile.callback.unHandle)
				{
					if(PRDN_IsRangeValid(PRDN_GET_CHUNK_OFF(cur), PRDN_GET_CHUNK_OFF(cur + unRequestSize - 1))) /* if last_pre_read_cur is consumed */
					{
						pre_read_cur = cur;
						PrintDebug("read from buffer\n");
					}
					else
					{
						pre_read_cur = cur - DEFAULT_PRDN_BACKTRICK_PREBUFFERING_SIZE;
						PrintDebug("pre buffering ...\n");
						g_nchunkoffLastPrebufferIdx = PRDN_GET_CHUNK_OFF(cur + unRequestSize - 1);
						if(g_nchunkoffLastPrebufferIdx > g_nchunkoffPrdnEof)
						{
							g_nchunkoffLastPrebufferIdx = g_nchunkoffPrdnEof;
						}
						g_bPrebufferingRequested = TRUE;
					}
					if(pre_read_cur < 0)
					{
						pre_read_cur = 0;
						PrintDebug("Set ZERO!!\n");
					}
				}
				else
				{
					pre_read_cur = cur;
				}
				if( ((DNP_USER_DATA_t *)tHandle.pDnpUserData)->unOpenFromNexusHandle == tHandle.tFile.callback.unHandle) /* unhandle for player. not indexer or prober */
				{
					DNP_USER_DATA_t *pDnpUserData = (DNP_USER_DATA_t *)tHandle.pDnpUserData;
					off_t displacement = pre_read_cur - (pDnpUserData->player_last_read_cursor + pDnpUserData->player_last_read_size);
					HINT32 nContinueousReadThreadshold = -8192;

					if(g_bPrdnForwardPlay == FALSE)
					{
						nContinueousReadThreadshold = -1024*1024; /* 1MB */
					}

					P_IPTUNER_DnpSetCurrentDataReadOffset((HUINT64)pre_read_cur);

					g_nchunkoffCurrentReadPoint = PRDN_GET_CHUNK_OFF(pre_read_cur); /* cursor of prdn buffering. meaningfull only for player */

					if(displacement >= nContinueousReadThreadshold && displacement <= 0) /* nexus tends to read overlappedly in normal play */
					{
						;//PrintDebug("Player Continueous Read disp=%d\n",(int)displacement);
					}
					else
					{
						if(g_nchunkoffOldPrdnRequestedSeekPoint != g_nchunkoffCurrentReadPoint)
						{
							PrintDebug("=====Player Request Seek displace is %d\n", (int)displacement);
							g_nchunkoffPrdnRequestedSeekPoint = g_nchunkoffCurrentReadPoint;
							g_nchunkoffOldPrdnRequestedSeekPoint = g_nchunkoffCurrentReadPoint; /* this will prevent duplicate seek request */
						}
					}


					pDnpUserData->player_last_read_cursor = pre_read_cur;
					pDnpUserData->player_last_read_size = size;

				}
				if(tHandle.pDnpUserData)
				{
					if ( ((DNP_USER_DATA_t *)tHandle.pDnpUserData)->bDNPNetworkError == TRUE )
					{
						PrintDebug ("%s (%d) Error> DNP Network Error, return VFIO_ERROR_IN_DEVICE\n",__FUNCTION__,__LINE__);
						ret = VFIO_ERROR_IN_DEVICE; /* this value is transformed to 0(EOF) in b_stdio_read */
						break;
					}
				}

				if(  PRDN_IsRangeValid(PRDN_GET_CHUNK_OFF(pre_read_cur), PRDN_GET_CHUNK_OFF(cur + unRequestSize - 1))  )
#else
				if ( ((cur+unRequestSize) <= sb.st_size) || (cur+unRequestSize >= s_nFileLength))
#endif
				{
					g_bPrebufferingRequested = FALSE;
					readCnt = read((int)tHandle.tFile.callback.unHandle, ptr, unRequestSize);
					if ( readCnt < 0 )
					{
						PrintError ("%s (%d) Error>  read()\n",__FUNCTION__,__LINE__);
						ret = VFIO_ERROR_IN_DEVICE;
						*ptReadSize = 0;
					}
					else
					{
						*ptReadSize = readCnt;
					}
#if DI_IPTUNER_DNP_ENABLE_INTEGRITY_CHECK_COMPARING_WITH_LOCAL_FILE
					{
						HUINT8 * testbuff = DD_MEM_Alloc(*ptReadSize);
						if(testbuff)
						{

							off_t fdcur = lseek(gFdForDnpIntegrityCheck,cur, SEEK_SET);
							size_t read_size = read(gFdForDnpIntegrityCheck, testbuff, *ptReadSize);

							if(fdcur != cur)
							{
								PrintError("Seek Value is error\n");
								PrintError ("file offset=%"PRId64", tuner offset=%"PRId64"\n", fdcur, cur);
								VK_Panic();
								return VFIO_ERROR_IN_DEVICE;
							}
							if(read_size != *ptReadSize)
							{
								PrintError("Read Size Error\n");
								VK_Panic();
								return VFIO_ERROR_IN_DEVICE;
							}
							if(VK_memcmp(ptr, testbuff, *ptReadSize) != 0)
							{
								PrintError("Read Data Intigrity Check failed\n");
								VK_Panic();
								return VFIO_ERROR_IN_DEVICE;
							}
							else
							{
								PrintDebug("Data Intigrity is OK\n");
							}
							DD_MEM_Free(testbuff);
							}
					}
#endif
					break;
				}
				else
				{
					PrintDebug ("%s (%d) Waiting> - Not enough data ! download size(%"PRId64"), request offset (%"PRId64")\n"
							, __FUNCTION__, __LINE__ , sb.st_size, (cur+unRequestSize) );

					if(tHandle.pDnpUserData == NULL)
					{
						PrintError("pDnpUserData is NULL\n");
						ret = VFIO_ERROR_IN_DEVICE; /* this value is transformed to 0(EOF) in b_stdio_read */
						break;
					}
					/* DNP Probe 시 Network 에러는 totoally fail처리 */
					if ( ((DNP_USER_DATA_t *)tHandle.pDnpUserData)->nPurpose ==  1
							&& ((DNP_USER_DATA_t *)tHandle.pDnpUserData)->bDNPNetworkError == TRUE )
					{
						PrintDebug ("%s (%d) Error> DNP Network Error, return VFIO_ERROR_IN_DEVICE\n",__FUNCTION__,__LINE__);
						ret = VFIO_ERROR_IN_DEVICE; /* this value is transformed to 0(EOF) in b_stdio_read */
						break;
					}


				}
			}
			(void) VK_TASK_Sleep (TUNER_DNP_CALLBACK_WAITTTIME);

		} /* end of while */
	}

#ifdef DEBUG_IO_FUNCTION
	PrintData("%s (%d) ret(%d), readCnt(%d)\n",__FUNCTION__,__LINE__, ret, *ptReadSize);

	PrintExit;
#endif

	if( ((DNP_USER_DATA_t *)tHandle.pDnpUserData)->unOpenFromNexusHandle == tHandle.tFile.callback.unHandle) /* unhandle for player. not indexer or prober */
	{
		P_IPTUNER_DnpSetLastReadSize(*ptReadSize);
	}

	(void) VK_SEM_Release (s_ulDNP_ReadSema);


	return ret;
}

/**
 * VFIO (for DNP) Seek Function
 * call by : DI_MEDIA_Probe
 *
 * Probe를 시도할 때 bmedia player단에서 요구하는 Probe Point까지
 * Download 받지 못한 경우 Seek을 통해 받을때까지 Check 하는 Function
 *
 * 해당 offset까지 Seek 이 성공하는 경우 bmedia_player는 비로소 Probe를 시도한다.
 */
static int P_IPTUNER_DnpSeek (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	int		ret=VFIO_ERROR_SUCCESS;
	off_t	curOffset;

	int		nLoopCnt=0;
	struct stat	sb;
	off_t		cur=0;

	int		nIsReady=FALSE;

#ifdef DEBUG_IO_FUNCTION
	PrintEnter;
	PrintData ("%s (%d) offset=%"PRId64", whence=%d\n",__FUNCTION__,__LINE__,offset, whence);
#endif

	cur = lseek((int)tHandle.tFile.callback.unHandle, 0, SEEK_CUR);
	if ( (off_t)-1 == cur )
	{
		PrintError ("%s (%d) Error>  lseek() - 1\n",__FUNCTION__,__LINE__);
		ret = -1;
	}
	else
	{
		while ( nLoopCnt++ < TUNER_DNP_PROBE_SEEK_CALLBACK_WAITCOUNT)
		{
			if(s_bIsPlaybackStop == TRUE)
			{
				PrintError ("%s (%d) Error>  s_bIsPlaybackStop() - 1\n",__FUNCTION__,__LINE__);
				ret = -1;
				break;
			}

			if (fstat((int)tHandle.tFile.callback.unHandle, &sb)<0)
			{
				PrintError ("%s (%d) Error> Fail fstat().\n",__FUNCTION__,__LINE__);
			}
			else
			{
				PrintDebug ("%s(%d)  SEEK_CUR (%d) SEEK_SET(%d) whence (%d) sb.st_size(%d) \n"
							,__FUNCTION__
							,__LINE__
							,SEEK_CUR
							,SEEK_SET
							,whence
							,sb.st_size);
#if 0
				PrintError ("%s(%d)  cur (%"PRId64") offset(%"PRId64") s_nFileLength (%"PRId64") sb.st_size(%"PRId64") \n"
							,__FUNCTION__
							,__LINE__
							,cur
							,offset
							,s_nFileLength
							,sb.st_size);
				PrintError ("%s(%d)  cur+offset+64K %"PRId64" offset+64K %"PRId64" \n"
							,__FUNCTION__,__LINE__,((cur+offset)+(6*1024)),((offset)+(6*1024)));
#endif

#ifdef ENABLE_MAPPED_DNP
				/* bound check를 해서 bound를 넘는 read호출을 방지 */
				if( SEEK_CUR == whence )
				{
					if(cur+offset >= s_nFileLength)
					{
						PrintDebug("Seek Request is beyond the file length\n");
						return -1;
					}
				}
				else if ( SEEK_SET == whence )
				{
					if(offset > s_nFileLength)
					{
						PrintDebug("Seek Request is beyond the file length\n");
						return -1;
					}
				}
				else if( SEEK_END == whence )
				{
					offset = s_nFileLength - 1; /* last index */
					whence = SEEK_SET;
				}
				else
				{
					PrintError("%s(%d) Error > Invalid whence enum\n", __FUNCTION__, __LINE__);
				}
#else

				if ( SEEK_CUR == whence )
				{
					if ((((cur+offset)+(6*1024)) <= s_nFileLength) && (((cur+offset)+(6*1024)) <= sb.st_size))
					{
						nIsReady = TRUE;
					}
					else if ( (((cur+offset)+(6*1024)) > s_nFileLength) && ((cur+offset) <= sb.st_size))
					{
						usleep(100000); /* brcm work around */
						nIsReady = TRUE;
					}
					/* else - { continue } */
				}
				else if ( SEEK_SET == whence )
				{
					if (((offset+(6*1024)) <= s_nFileLength) && (((offset)+(6*1024)) <= sb.st_size))
					{
						nIsReady = TRUE;
					}
					else if ( ((offset+(6*1024)) > s_nFileLength) && (offset <= sb.st_size))
					{
						usleep(100000); /* brcm work around */
						nIsReady = TRUE;
					}
					/* else - { continue } */
				}
				else
#endif
				{
					nIsReady = TRUE;
				}

				if ( TRUE == nIsReady )
				{
					curOffset = lseek((int)tHandle.tFile.callback.unHandle, offset, whence);
					if ( (off_t)-1 == curOffset )
					{
						PrintDebug ("%s (%d) Error>  lseek() - 2\n",__FUNCTION__,__LINE__);
						ret = -1;
					}
					/* else { continue } */

					break;
				}
				else
				{
						PrintDebug ("%s (%d) Seeking> - Not enough data ! real size(%"PRId64"), request size(%"PRId64")\n"
						,__FUNCTION__,__LINE__
						,sb.st_size, (offset));
				}
			}

			VK_TASK_Sleep (TUNER_DNP_CALLBACK_WAITTTIME);
		}
	}

#ifdef DEBUG_IO_FUNCTION
	PrintExit;
#endif

	return ret;
}

/** VFIO (for DNP) Tell Function */
static int P_IPTUNER_DnpTell (VFIO_Handle_t tHandle, off_t *plCurOffset)
{
	int		ret=VFIO_ERROR_SUCCESS;

#ifdef DEBUG_IO_FUNCTION
	PrintEnter;
#endif

	*plCurOffset = lseek((int)tHandle.tFile.callback.unHandle, 0, SEEK_CUR);
	if ( (off_t)-1 == *plCurOffset )
	{
		PrintError ("%s (%d) Error>  lseek()\n",__FUNCTION__,__LINE__);
		ret = -1;
	}
	/* else { continue } */

#ifdef DEBUG_IO_FUNCTION
	PrintDebug("%s(%d) plCurOffset (%"PRId64")\n",__FUNCTION__,__LINE__,*plCurOffset);

	PrintExit;
#endif

	return ret;
}

/** VFIO (for DNP) IsEOF Function */
static int P_IPTUNER_DnpIsEOF (VFIO_Handle_t tHandle)
{
	int			ret=VFIO_ERROR_SUCCESS;
	struct stat	sb;
	off_t		cur=0;

#ifdef DEBUG_IO_FUNCTION
	PrintEnter;
#endif

	if (fstat((int)tHandle.tFile.callback.unHandle, &sb)<0)
	{
		PrintError ("%s (%d) Error> fstat()\n",__FUNCTION__,__LINE__);
		ret = -1;
	}
	else
	{
		cur = lseek((int)tHandle.tFile.callback.unHandle, 0, SEEK_CUR);
		if ( (off_t)-1 == cur )
		{
			PrintError ("%s (%d) Error>  lseek()\n",__FUNCTION__,__LINE__);
			ret = -1;
		}
		else
		{
			/* TODO :: */
			if ( cur == sb.st_size )
			{
				ret = 1;
			}
			else
			{
				ret = 0;
			}
		}
	}

#ifdef DEBUG_IO_FUNCTION
	PrintExit;
#endif

	return ret;
}

/** VFIO (for DNP) OpenFromNexus Function */
static int P_IPTUNER_DnpOpenFromNexus (const HINT8 *path, const unsigned int flags, VFIO_Handle_t *ptHandle)
{
	int		ret=VFIO_ERROR_SUCCESS;
	int		fd;

#ifdef DEBUG_IO_FUNCTION
	PrintEnter;
	PrintData ("%s (%d)  path=%s, flags=%d\n",__FUNCTION__,__LINE__, path, flags);
#else
	/* warning: unused parameter 'path' */
	/* warning: unused parameter 'flags' */
	/* Just post the debug message instead of  UNUSED_PARAM() */
	/* Dont use UNUSED_PARAM(path); UNUSED_PARAM(flags); */
	PrintData("%s (%d)  path=%s, flags=%d\n",__FUNCTION__,__LINE__, path, flags);
#endif

	if(ptHandle == NULL)
	{
		PrintError("ptHandle is NULL\n");
		ret = VFIO_ERROR_BAD_PARAMETER;
		goto error;
	}

	/* error check */
	if(stHandle.pDnpUserData == NULL)
	{
		PrintError("pDnpUserData is NULL\n");
		ptHandle->ucAvailable = 0;
		ret = VFIO_ERROR_IN_DEVICE;
		goto error;
	}

	ptHandle->pDnpUserData = stHandle.pDnpUserData;


	fd = open(DI_IPTUNER_DNP_FILE_PATH, O_RDONLY|O_SYNC, 0666);
	if ( fd < 0 )
	{
		ptHandle->ucAvailable = 0;
		ret = VFIO_ERROR_IN_DEVICE;
	}
	else
	{
		ptHandle->ucAvailable = 1;
		ptHandle->tFile.callback.unHandle = (HUINT32)fd;
		if(((DNP_USER_DATA_t *)ptHandle->pDnpUserData)->unOpenFromNexusHandle == 0) /* first open if for player. second is for indexer */
		{
			PrintDebug("====\nOPEN FOR PLAYER\n");
			((DNP_USER_DATA_t *)ptHandle->pDnpUserData)->unOpenFromNexusHandle = ptHandle->tFile.callback.unHandle;
		}
		else
		{
			PrintDebug("====\nOPEN FOR INDEXER\n");
		}

		PrintData ("%s (%d)  callback handle=0x%x\n",__FUNCTION__,__LINE__, ptHandle->tFile.callback.unHandle);
	}
error:

#ifdef DEBUG_IO_FUNCTION
	PrintExit;
#endif

	return ret;
}

/** VFIO (for DNP) SeekFromNexus Function */
static off_t P_IPTUNER_DnpSeekForNexus (VFIO_Handle_t tHandle, off_t offset, int whence)
{
	off_t		curOffset=(off_t)-1;

	int		nLoopCnt=0;
	struct stat	sb;
	off_t		cur=0;

	int		nIsReady=FALSE;

#ifdef DEBUG_IO_FUNCTION
	PrintEnter;
	PrintData ("%s (%d) offset=%"PRId64", whence=%d\n",__FUNCTION__,__LINE__, offset, whence);
#endif

	cur = lseek((int)tHandle.tFile.callback.unHandle, 0, SEEK_CUR);
	if ( (off_t)-1 == cur )
	{
		PrintError ("%s (%d) Error>  lseek() - 1\n",__FUNCTION__,__LINE__);
		curOffset = (off_t)-1;
	}
	else
	{
		while ( nLoopCnt++ < TUNER_DNP_SEEK_CALLBACK_WAITCOUNT)
		{
			if(s_bIsPlaybackStop==TRUE)
			{
				PrintError ("%s (%d) Error>  s_bIsPlaybackStop() - 1\n",__FUNCTION__,__LINE__);
				curOffset = (off_t)-1;
				break;
			}

			if (fstat((int)tHandle.tFile.callback.unHandle, &sb)<0)
			{
				PrintError ("%s (%d) Error>  fstat() - 1\n",__FUNCTION__,__LINE__);
			}
			else
			{
#ifdef ENABLE_MAPPED_DNP
				/* no check */
#else
				if ( SEEK_CUR == whence )
				{
					if ( (cur + offset) <= sb.st_size )
					{
						nIsReady = TRUE;
					}
					/* else - { continue } */
				}
				else if ( SEEK_SET == whence )
				{
					if ( offset <= sb.st_size )
					{
						nIsReady = TRUE;
					}
					/* else - { continue } */
				}
				else
#endif
				{
					nIsReady = TRUE;
				}

				if ( TRUE == nIsReady )
				{
					curOffset = lseek((int)tHandle.tFile.callback.unHandle, offset, whence);

					break;
				}
				else
				{
					if ( SEEK_CUR == whence )
					{
						PrintDebug ("%s (%d) Seeking> - Not enough data ! real size(%"PRId64"), request size(%"PRId64")\n"
						,__FUNCTION__,__LINE__
						,sb.st_size, (cur+offset));
					}
					else
					{
						PrintDebug ("%s (%d) Seeking> - Not enough data ! real size(%"PRId64"), request size(%"PRId64")\n"
						,__FUNCTION__,__LINE__
						,sb.st_size, (offset));
					}
				}
			}

			VK_TASK_Sleep (TUNER_DNP_CALLBACK_WAITTTIME);
		} /* end of while */
	}

#ifdef DEBUG_IO_FUNCTION
	PrintData ("%s (%d)  curOffset=%"PRId64"\n",__FUNCTION__,__LINE__, curOffset);
	PrintExit;
#endif

	return curOffset;
}

/** VFIO (for DNP) BoundsFromNexus Function */
static int P_IPTUNER_DnpBoundsForNexus(VFIO_Handle_t tHandle, off_t *file_size)
{
	int		ret=VFIO_ERROR_SUCCESS;
#if 0
	struct stat	sb;
#endif
	UNUSED_PARAM(tHandle);

#if 0
	PrintEnter;
#endif

#if	0 /** need whole file size */
	if (fstat((int)tHandle.tFile.callback.unHandle, &sb)<0)
	{
		PrintError (("[P_IPTUNER_DnpBoundsForNexus] Error - fstat()\n"));
		ret = VFIO_ERROR_IN_DEVICE;

		*file_size = 0;
	}
	else
	{
		*file_size = sb.st_size;
	}
#else
	*file_size = s_nFileLength;
#endif

#if 0
	PrintError ("%s (%d)  ret(%d), file size=%"PRId64"\n",__FUNCTION__,__LINE__, ret, *file_size );
	PrintExit;
#endif

	return ret;
}

#if __________di__________
#endif
/*******************************************************************/
/************************ public funtions **************************/
/*******************************************************************/
DI_ERR_CODE DI_IPTUNER_DnpOpen (DI_IPTUNER_Index_e nTunerIndex, HINT8 *pUrl, DI_MEDIA_INFO_t *pstMediaInfo)
{
	HINT32	nRet = DI_ERR_OK;
	HINT32  nVKRet;
	HINT8 *dnpUrl=NULL;
	HINT32  nUrlLen;

	HINT32 i = 0;

	DI_IPTUNER_Msg_t s_Msg;
	VFIO_Callback_t	stCBFunction;

	HULONG	ulevent;


	PrintEnter;
	VK_memset (&stURL,0,sizeof(URL_t));

	s_bIsPlaybackStop = FALSE;

#if DI_IPTUNER_DNP_ENABLE_INTEGRITY_CHECK_COMPARING_WITH_LOCAL_FILE
	if(nRet == DI_ERR_OK)
	{
		gFdForDnpIntegrityCheck = open ( DI_IPTUNER_DNP_INTEGRITY_CHECK_COMPARING_WITH_LOCAL_FILE_NAME , O_RDONLY, 0666);
	}
#endif

	/* TODO :: 추후 driver entry point가 결정되면 해당 위치로 옮겨야 함. 현재 예상 위치는 DRV_IPTUNER_Init */

	/* Task가 init은 하고 다운로드는 멈춘 상태 */
	nRet = DI_IPTUNER_DnpStop(nTunerIndex);
	if ( TRUE != s_ucIsInitialized )
	{
		(void)P_IPTUNER_DnpInitThread();
	}

	/* else - { continue } */

	if ( NULL == pUrl )
	{
		PrintError ("%s (%d) Error>  pUrl is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}
	else if (pstMediaInfo == NULL)
	{
		PrintError ("%s (%d) Error> pstMediaInfo is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}
	/* else - { continue } */

	PrintData ("%s (%d)  pUrl=%s, nTunerIndex=%d\n",__FUNCTION__,__LINE__, pUrl, nTunerIndex);

	/* 1. VFIO에 callback 함수 등록 */
	(void)VK_memset (&stCBFunction, 0x0, sizeof(VFIO_Callback_t));

	stCBFunction.Open 			=(VFIOOpenCB)  P_IPTUNER_DnpOpen;
	stCBFunction.Close			=(VFIOCloseCB) P_IPTUNER_DnpClose;
	stCBFunction.Read			=(VFIOReadCB)  P_IPTUNER_DnpRead;
	stCBFunction.Seek			=(VFIOSeekCB)  P_IPTUNER_DnpSeek;
	stCBFunction.Tell			=(VFIOTellCB)  P_IPTUNER_DnpTell;
	stCBFunction.IsEOF			=(VFIOIsEOFCB) P_IPTUNER_DnpIsEOF;
	stCBFunction.OpenNexus		=(VFIOOpenFromNexusCB) P_IPTUNER_DnpOpenFromNexus;
	stCBFunction.SeekNexus		=(VFIOSeekForNexusCB) P_IPTUNER_DnpSeekForNexus;
	stCBFunction.BoundsNexus	=(VFIOBoundsForNexusCB) P_IPTUNER_DnpBoundsForNexus;
	(void) VFIO_RegisterCallback (VFIO_CALLBACK_TYPE_DNP, stCBFunction);

	/* 2. Parse URL */
	(void) VK_memset (&s_Msg, 0x0, sizeof(s_Msg));

	/** IPTUNER DNP 를 Open할때에는 [dnp]를 붙이지 않음. VFIO에서만 구별을 위해 [dnp]를 붙이나
	 * 추후 VFIO에서 현재 Type을 알아서 판단하도록 수정이 필요함. */
	(void) UTIL_UrlParse (&stURL,(const char*)pUrl,  0);
	switch (nTunerIndex)
	{
		case DI_IPTUNER_DNP_HTTP	:
			if( VFIO_ERROR_SUCCESS == DI_STREAM_HTTP_Open (pUrl, 0, &stHandle) )
			{
				s_Msg.unMsg = TUNER_DNP_MSG_HTTP_PROBE;
			}
			else
			{
				PrintError ("%s (%d) Error>  DI_STREAM_HTTP_Open Fail\n",__FUNCTION__,__LINE__);
				UTIL_UrlClean (&stURL);
				nRet = DI_ERR_ERROR;
				return nRet;
			}
			break;
		case DI_IPTUNER_DNP_MMS		:
			if( VFIO_ERROR_SUCCESS == DI_STREAM_MMS_Open (pUrl, 0, &stHandle) )
			{
				s_Msg.unMsg = TUNER_DNP_MSG_HTTP_PROBE;
			}
			else
			{
				PrintError ("%s (%d) Error>  DI_STREAM_MMS_Open Fail\n",__FUNCTION__,__LINE__);
				UTIL_UrlClean (&stURL);
				return -1;
			}
			s_Msg.unMsg = TUNER_DNP_MSG_MMS_PROBE;
			break;
		case DI_IPTUNER_DNP_RTSP	:
			s_Msg.unMsg = TUNER_DNP_MSG_RTSP_PROBE;
			break;
		case DI_IPTUNER_DNP_FTP		:
			s_Msg.unMsg = TUNER_DNP_MSG_FTP_PROBE;
			break;
		default :
			PrintError  ("%s (%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			UTIL_UrlClean (&stURL);
			nRet = DI_ERR_ERROR;
			return nRet;
	}


	for(i = 0; i < DNP_DOWNLOAD_SPEED_COUNT; i++)
	{
		s_nArrayDownloadSpeed[i] = -1;
	}
	s_nDownloadSpeedIndex = 0;


	/* pDnpUserData 할당(DnpClose에서 Free) 및 초기화 */
	{
		DNP_USER_DATA_t * pDnpUserData = DD_MEM_Alloc(sizeof(DNP_USER_DATA_t));
		pDnpUserData->nPurpose = 1; // TODO REPLACE WITH ENUM
		pDnpUserData->bDNPNetworkError = FALSE;
		pDnpUserData->unOpenFromNexusHandle = 0;
		pDnpUserData->uiMediaMaxBitrateInChunkPerSec = 0; /* 0 if unknown */
		pDnpUserData->bProbeDone = FALSE;
		stHandle.pDnpUserData = pDnpUserData; // HTTP OPEN과 DNP OPEN은 GneralPtr을 공유 ; owner = DNP
		pDnpUserData->uiContentDuration = 0;
		g_nPrdnInfoBufferingTime = 0;
		g_bPrebufferingRequested = FALSE;
	}

	/** 3. Start Download */
	nRet = DI_IPTUNER_DnpStart (nTunerIndex);
	if ( DI_ERR_OK != nRet )
	{
		PrintError ("%s (%d) Error> DI_IPTUNER_DnpStart fail\n",__FUNCTION__,__LINE__);
		UTIL_UrlClean(&stURL);
		return nRet;
	}

	/* IPTUNER_DnpTask에서 stHandle를 HTTP dnp receive task로 넘겨줌*/
	/* 이를 통해 dnp thread(reader)와 http thread(writer)가 pDnpUserData를 공유*/
	(void) VK_MSG_Send ( s_ulDNPMsgQId, (HUINT32 *)&s_Msg, sizeof(DI_IPTUNER_Msg_t) ) ;


	/* 4. wait event - 10 seconds */
	/* TODO :: 적당한 양의 Data가 수신될때 까지 기다린다.
	 * 6M 정도 수신후 DNP Task 에서 깨워준다.*/
 	/* DNP Task가 초기화되고 다운로드가 시작되면 이벤트를 던짐 */
	nVKRet = VK_EVENT_ReceiveTimeout (s_ulTunerEventID, &ulevent, TUNER_DNP_WAITEVTTIME);
	if (nVKRet != BERR_SUCCESS)
	{
		PrintError  ("%s (%d) Error> VK_EVENT_ReceiveTimeout (%x) !\n",__FUNCTION__,__LINE__, (int)nVKRet);
		nRet = DI_ERR_TIMEOUT;
	}
	else
	{
		/*XXX: For TEST DI_MEDIA_Probe를 위해서는 URL앞에 [dnp]를 붙여 줘야 한다.*/
		nUrlLen = VK_strlen((char*)pUrl)+6;
		dnpUrl = (HINT8*) DD_MEM_Alloc (nUrlLen);
		if(dnpUrl==NULL)
		{
			nRet = DI_ERR_NO_ALLOC;
			PrintError ("%s(%d) Error> Heap Memory Allocation Fail. size (%d) !!\n",__FUNCTION__,__LINE__,MAX_FILE_NAME_SIZE);
		}

		else
		{
			VK_memset (dnpUrl, 0, nUrlLen);

			nRet = VK_snprintf((char*)dnpUrl,nUrlLen,"[dnp]%s",pUrl);
			if(nRet<0)
			{
				PrintError ("%s(%d) Error> can't make url for dnp\n",__FUNCTION__,__LINE__);
				nRet = DI_ERR_ERROR;
			}
			else
			{
				/* 5. Probe Media */
				nRet = DI_MEDIA_Probe ((HUINT8*)dnpUrl, pstMediaInfo);
				((DNP_USER_DATA_t *)stHandle.pDnpUserData)->nPurpose = 0;
			}

#if DI_IPTUNER_DNP_ENABLE_INTEGRITY_CHECK_COMPARING_WITH_LOCAL_FILE
#if 0
			{
				int rc = 0;
				HINT32 i = 0;
				VFIO_Handle_t tHandleForTest;
				off_t request_read_point = 0;
				size_t request_size = 0;
				size_t read_size = 0;
				size_t request_size_max = 192*1024; /* 192 KB */
				size_t test_count = 100000;
				HUINT8* buf = 0;

				PrintError("Entering Simulational DNP Integrity Test Before Movie Play. Probe Will Return Error\n");
				buf = VK_malloc(request_size_max);
				if(buf == NULL)
				{
					PrintError("VK_malloc for simulate dnp integrity test\n");
				}
				else
				{
					/* init */
					srand(time(NULL));
					rc = VFIO_Open((HUINT8*)dnpUrl, "rb", &tHandleForTest);

					for(i = 0 ; i < test_count ; i++)
					{
						request_read_point = ((off_t)rand()+rand()+rand()+rand()) % s_nFileLength; /* RANDMAX IS 2G */
						request_size = rand() % request_size_max;
						rc = VFIO_Seek(tHandleForTest, request_read_point, SEEK_SET);
						if(rc < 0)
						{
							PrintError("VFIO Seek has Failed\n");
							VK_Panic();
							break;
						}
						rc = VFIO_Read(tHandleForTest, buf, 1, request_size, &read_size);
						if(rc < 0)
						{
							PrintError("VFIO Read has Failed\n");
							VK_Panic();
							break;
						}
						PrintError("Simulational Tested Count = %d\n", i);
					}

					/* deinit */
					VK_free(buf);
					VFIO_Close(tHandleForTest);
					PrintError("Leaving Simulational DNP Integrity Test Before Movie Play\n");
				}
				nRet = DI_ERR_ERROR;
				return nRet;

			}
#endif
#endif
			if (nRet != DI_ERR_OK)
			{
				PrintError ("%s (%d) Error> DI_MEDIA_Probe : %d\n",__FUNCTION__,__LINE__, nRet);
			}
			else
			{
#ifdef DI_IPTUNER_DNP_DEBUG
				(void) DI_MEDIA_PrintProbeInfo (pstMediaInfo);
#endif

#ifdef ENABLE_MAPPED_DNP
				nRet = DI_IPTUNER_UnderflowMonitorInitThread(nTunerIndex, pstMediaInfo, s_nFileLength);
				if(nRet != DI_ERR_OK)
				{
					PrintError("@@@@@ [%s:%d:%d] UnderflowMonitoring Thread can't create\n", __func__, __LINE__, nRet);
				}
#endif
				if(pstMediaInfo->max_bitrate == 0) /* unknown */
				{
					((DNP_USER_DATA_t *)stHandle.pDnpUserData)->uiMediaMaxBitrateInChunkPerSec = 3327106 / 8 / (HUINT32)PRDN_GET_RAW_OFF_FROM_CHUNK_OFF(1); /* default bitrate */
				}
				else
				{
					((DNP_USER_DATA_t *)stHandle.pDnpUserData)->uiMediaMaxBitrateInChunkPerSec = pstMediaInfo->max_bitrate / 8 / (HUINT32)PRDN_GET_RAW_OFF_FROM_CHUNK_OFF(1) + 1; /* round up */
				}
				((DNP_USER_DATA_t *)stHandle.pDnpUserData)->uiContentDuration = pstMediaInfo->duration;
				((DNP_USER_DATA_t *)stHandle.pDnpUserData)->bProbeDone = TRUE;
				if(((DNP_USER_DATA_t *)stHandle.pDnpUserData)->bDNPNetworkError == TRUE)
				{
					PrintError ("%s (%d) Error> DI_MEDIA_Probe Network Error: %d\n",__FUNCTION__,__LINE__, nRet);
					nRet = DI_ERR_ERROR;
				}
			}

			if(dnpUrl != NULL)
			{
				(void) DD_MEM_Free (dnpUrl);
				dnpUrl = NULL;
			}
		}
	}
	/* else - { continue } */

	if(nRet != DI_ERR_OK)
	{
		(void)DI_IPTUNER_DnpStop (nTunerIndex); /* TODO replace with DnpClose*/
		(void)VFIO_UnregisterCallback (VFIO_CALLBACK_TYPE_DNP);
	}

	UTIL_UrlClean (&stURL);
	PrintExit;

	return nRet;
}


DI_ERR_CODE DI_IPTUNER_DnpClose (DI_IPTUNER_Index_e nTunerIndex)
{
	HINT32	nRet = DI_ERR_OK;

	PrintEnter;

	UNUSED_PARAM (nTunerIndex);

	/* If DI_IPTUNER_DnpClose() is called twice, the crash will be occurred */
	/* TODO: For protecting the crash issue, let's use the semaphore instead of the global variabe  */
	if ( s_ucIsInitialized == FALSE )
	{
		return DI_ERR_ERROR;
	}

	DI_IPTUNER_DnpStop (nTunerIndex);
	s_ucQuit = TRUE;

#ifdef ENABLE_MAPPED_DNP
	DI_IPTUNER_UnderflowMonitorTerminateThread();
#endif

	/* instead task join, spin wait */
	while(s_bDnpTaskFinished == FALSE)
	{
		VK_TASK_Sleep(100);
	}

	/* VK_TASK_Join may induce bus error. let destroy handle pthread join */
	VK_TASK_Destroy(s_ulDNPTaskId);

	UTIL_UrlClean (&stURL);

	(void)VFIO_UnregisterCallback (VFIO_CALLBACK_TYPE_DNP);


	(void) VK_SEM_Destroy (s_ulDNP_ReadSema);


	VK_EVENT_Destroy(s_ulTunerEventID);


	 (void) VK_SEM_Destroy (s_ulDNPSema);
	VK_MSG_Destroy(s_ulDNPMsgQId);


 	switch (nTunerIndex)
	{
		case DI_IPTUNER_DNP_HTTP	:
			if( VFIO_ERROR_SUCCESS != DI_STREAM_HTTP_Close (stHandle) )
			{
				PrintError ("%s (%d) Error>  DI_STREAM_HTTP_Close Fail\n",__FUNCTION__,__LINE__);
				nRet = DI_ERR_ERROR;
			}
			break;
		case DI_IPTUNER_DNP_MMS		:
			if( VFIO_ERROR_SUCCESS != DI_STREAM_MMS_Close (stHandle) )
			{
				PrintError ("%s (%d) Error>  DI_STREAM_MMS_Close Fail\n",__FUNCTION__,__LINE__);
				nRet = -1;
			}

			break;
		case DI_IPTUNER_DNP_RTSP	:

		case DI_IPTUNER_DNP_FTP		:

		default :
			PrintError  ("%s (%d) Error> invalid Tuner Index !\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;

	}


	s_ucIsInitialized = FALSE;

	if(stHandle.pDnpUserData != NULL)
	{
		DD_MEM_Free(stHandle.pDnpUserData);
		stHandle.pDnpUserData = NULL;
	}

	/*DD_MEM_PrintList();*/

#ifdef ENABLE_MAPPED_DNP
	if(g_pucPrdnChunkDownloadStatusArray != NULL)
	{
		PrintDebug("@@@@@ [%s:%d] g_pucPrdnChunkDownloadStatusArray is Free\n", __func__, __LINE__);
		DD_MEM_Free(g_pucPrdnChunkDownloadStatusArray);
		g_pucPrdnChunkDownloadStatusArray = NULL;
	}
#endif

#if DI_IPTUNER_DNP_ENABLE_INTEGRITY_CHECK_COMPARING_WITH_LOCAL_FILE
	close(gFdForDnpIntegrityCheck);
#endif
	PrintExit;

	return nRet;
}

/** Download 가 Stop 된 경우 Resume의 용도로 사용한다. */
DI_ERR_CODE DI_IPTUNER_DnpStart (DI_IPTUNER_Index_e nTunerIndex)
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	/* TODO : junsy */
	PrintEnter;
	switch (nTunerIndex)
	{
		case DI_IPTUNER_DNP_HTTP :
			DI_STREAM_HTTP_DnpStartReceiving ();
			break;
		case DI_IPTUNER_DNP_MMS :
			DI_STREAM_MMS_DnpStartReceiving ();
			break;
		case DI_IPTUNER_DNP_RTSP :
//			DI_STREAM_RTSP_DnpStartReceiving ();
			PrintError ("%s(%d) Error>  -  Not Supported RTSP DNP Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
		case DI_IPTUNER_DNP_FTP  :
//			DI_STREAM_FTP_DnpStartReceiving ();
			PrintError ("%s(%d) Error>  -  Not Supported FTP DNP Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
		default  :
			nRet = DI_ERR_INVALID_PARAM;
			PrintError ("%s(%d) Error>  -  Not Supported Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;

	}
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_DnpStop (DI_IPTUNER_Index_e nTunerIndex)
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	/* TODO : junsy */

	PrintEnter;
	switch (nTunerIndex)
	{
		case DI_IPTUNER_DNP_HTTP :
			DI_STREAM_HTTP_DnpStopReceiving ();
			break;
		case DI_IPTUNER_DNP_MMS :
			DI_STREAM_MMS_DnpStopReceiving ();
			break;
		case DI_IPTUNER_DNP_RTSP :
//			DI_STREAM_RTSP_DnpStopReceiving ();
			PrintError ("%s(%d) Error>  -  Not Supported RTSP DNP Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
		case DI_IPTUNER_DNP_FTP  :
//			DI_STREAM_FTP_DnpStopReceiving ();
			PrintError ("%s(%d) Error>  -  Not Supported FTP DNP Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
		default  :
			nRet = DI_ERR_INVALID_PARAM;
			PrintError ("%s(%d) Error>  -  Not Supported Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
	}
	PrintExit;

	return nRet;
}

/** Download 가 Stop 된 경우 Resume의 용도로 사용한다. */
DI_ERR_CODE DI_IPTUNER_DnpPause (DI_IPTUNER_Index_e nTunerIndex)
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	/* TODO : junsy */
	PrintEnter;
	switch (nTunerIndex)
	{
		case DI_IPTUNER_DNP_HTTP :
			s_bIsPlaybackStop = TRUE;
			DI_STREAM_HTTP_DnpPauseReceiving ();
			break;
		case DI_IPTUNER_DNP_MMS :
			s_bIsPlaybackStop = TRUE;
			DI_STREAM_MMS_DnpPauseReceiving ();
			break;
		case DI_IPTUNER_DNP_RTSP :
//			DI_STREAM_RTSP_DnpStartReceiving ();
			PrintError ("%s(%d) Error>  -  Not Supported RTSP DNP Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
		case DI_IPTUNER_DNP_FTP  :
//			DI_STREAM_FTP_DnpStartReceiving ();
			PrintError ("%s(%d) Error>  -  Not Supported FTP DNP Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
		default  :
			nRet = DI_ERR_INVALID_PARAM;
			PrintError ("%s(%d) Error>  -  Not Supported Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;

	}
	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_DnpResume (DI_IPTUNER_Index_e nTunerIndex)
{
	DI_ERR_CODE nRet=DI_ERR_OK;
	/* TODO : junsy */

	PrintEnter;
	switch (nTunerIndex)
	{
		case DI_IPTUNER_DNP_HTTP :
			s_bIsPlaybackStop = FALSE;
			DI_STREAM_HTTP_DnpResumeReceiving ();
			break;
		case DI_IPTUNER_DNP_MMS :
			s_bIsPlaybackStop = FALSE;
			DI_STREAM_MMS_DnpResumeReceiving ();
			break;
		case DI_IPTUNER_DNP_RTSP :
//			DI_STREAM_RTSP_DnpStopReceiving ();
			PrintError ("%s(%d) Error>  -  Not Supported RTSP DNP Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
		case DI_IPTUNER_DNP_FTP  :
//			DI_STREAM_FTP_DnpStopReceiving ();
			PrintError ("%s(%d) Error>  -  Not Supported FTP DNP Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
		default  :
			nRet = DI_ERR_INVALID_PARAM;
			PrintError ("%s(%d) Error>  -  Not Supported Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
	}
	PrintExit;

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_DnpGetInfo (DI_IPTUNER_Index_e nTunerIndex, DI_IPTUNER_Info_t *pInfo)
{
	DI_ERR_CODE nRet=DI_ERR_OK;
#ifndef ENABLE_MAPPED_DNP
	struct stat	sb;
#endif
	/* TODO : junsy */

	PrintEnter;
	switch (nTunerIndex)
	{
		case DI_IPTUNER_DNP_HTTP :
		case DI_IPTUNER_DNP_MMS :
		case DI_IPTUNER_DNP_RTSP :
		case DI_IPTUNER_DNP_FTP  :

#ifdef ENABLE_MAPPED_DNP
			 pInfo->nDownloadLength = s_nFileLength;
#else
			/* nDownloadLength */
			if (stat(DI_IPTUNER_DNP_FILE_PATH, &sb)<0)
			{
				PrintError ("%s(%d) Error>  -  fstat Fail.\n",__FUNCTION__,__LINE__);
			}
			else
			{
				pInfo->nDownloadLength = sb.st_size;
				PrintData ("%s(%d) nDownloadLength (%"PRId64")bytes.\n",__FUNCTION__,__LINE__,pInfo->nDownloadLength);
			}
#endif

			pInfo->nContentLength = s_nFileLength;
			/* MAXDOME INFO */
			pInfo->nBufferingPercent = g_nPrdnInfoBufferingPercent;

			pInfo->nRemainBufferingSec = g_nPrdnInfoBufferingLeftTime; /* MAXDOME want content sec to be buffered */
			pInfo->pDownloadedChunkArray = g_pucPrdnChunkDownloadStatusArray;
			pInfo->nLastChunkIndex = g_nchunkoffPrdnEof;

			break;
		default  :
			nRet = DI_ERR_INVALID_PARAM;
			PrintError ("%s(%d) Error>  -  Not Supported Tuner(%d)\n",__FUNCTION__,__LINE__,nTunerIndex);
			break;
	}
	PrintExit;

	return nRet;
}

DI_ERR_CODE DI_IPTUNER_DnpControl (DI_IPTUNER_Index_e nTunerIndex,  DI_IPTUNER_CMD_e cmd, HUINT64 val)
{
	/* TODO : junsy */
	UNUSED_PARAM (nTunerIndex);
	switch (cmd)
	{
		case CMD_CONTROL_SPEED :
			PrintDebug("%s (%d) CMD_CONTROL_SPEED (val=%d)\n",__FUNCTION__,__LINE__,(HINT32 )val);

			if(DI_MEDIA_TRICK_1RWSTEP <= val && val <= DI_MEDIA_TRICK_16RWSTEP) /* rewind */
			{
				g_bPrdnForwardPlay = FALSE;
			}
			if(DI_MEDIA_TRICK_1FFSTEP <= val && val <= DI_MEDIA_TRICK_16FFSTEP) /* forward play */
			{
				g_bPrdnForwardPlay = TRUE;
			}
#if 1	/* JWKIM, workaround: for Maxdome Service */
			if (val >= DI_MEDIA_TRICK_2FFSTEP && val <= DI_MEDIA_TRICK_16FFSTEP)
			{
				g_bPrdnPlayFFTrick = TRUE;
			}
			else
			{
				g_bPrdnPlayFFTrick = FALSE;
			}
#endif
#if 1 /* Added becuase of warning: enumeration value 'xxxx' not handled in switch */
		case CMD_CONTROL_PAUSE  :
		case CMD_CONTROL_RESUME :
		case CMD_CONTROL_SEEK   :
		default :
			PrintError ("%s(%d) Error>  -  Unsupported\n",__FUNCTION__,__LINE__);
			break;
#endif
	}
	return DI_ERR_OK;
}

DI_ERR_CODE DI_IPTUNER_DnpSendEvent (void)
{
	PrintEnter;


	(void) VK_EVENT_Send (s_ulTunerEventID, (unsigned long)0);


	PrintExit;
	return DI_ERR_OK;
}

/* TODO :: 2G 이상 file 지원 - off_t를 써야 함. */
DI_ERR_CODE DI_IPTUNER_DnpPutFileLength (HINT64 nFileLength)
{
	PrintEnter;
	//PrintData ("%s(%d) length = %d(0x%x)\n",__FUNCTION__,__LINE__, nFileLength, nFileLength);

	s_nFileLength = nFileLength;

	PrintExit;
	return DI_ERR_OK;
}


#if __________prdn__________
#endif

#ifdef ENABLE_MAPPED_DNP
HBOOL PRDN_IsRangeValid(HINT32 nchunkoffStart, HINT32 nchunkoffEnd)
{
	register HINT32 i;

#ifdef DEBUG_DN_PROGRESS_ARRAY
	PrintEnter;
#endif

	/* block accessing Freed Download Progress Array */
	if(g_bPrdnDownCompleted == TRUE)
	{
		return TRUE;
	}

	/* invalid args check */
	if(nchunkoffStart > g_nchunkoffPrdnEof)
	{
		PrintError("invalid arg error : read request is beyond the file length chunk off %d * chunk_size %d\n", nchunkoffStart, (HINT32)PRDN_GET_RAW_OFF_FROM_CHUNK_OFF(1));
		return FALSE;
	}
	if(nchunkoffEnd > g_nchunkoffPrdnEof)
	{
		nchunkoffEnd = g_nchunkoffPrdnEof;
	}


	for(i = nchunkoffStart; i <= nchunkoffEnd; i++)
	{
		if(!PRDN_ISVALID(i))
		{
			if(g_nchunkoffOldPrdnRequestedSeekPoint != nchunkoffStart)
			{
				PrintDebug("=== REQ a seek point\n");
				g_nchunkoffPrdnRequestedSeekPoint = nchunkoffStart;
				g_nchunkoffOldPrdnRequestedSeekPoint = nchunkoffStart;
			}
#ifdef DEBUG_DN_PROGRESS_ARRAY
			PrintExit;
#endif

			return FALSE;
		}
	}

	return TRUE;

#ifdef DEBUG_DN_PROGRESS_ARRAY
	PrintExit;
#endif
}



HBOOL PRDN_IsCompleted(void)
{


#ifdef DEBUG_DN_PROGRESS_ARRAY
	PrintEnter;
#endif

#if 0 /* warning: statement with no effect */
	for(g_nchunkoffIsCompletedStart; g_nchunkoffIsCompletedStart <= g_nchunkoffPrdnEof; g_nchunkoffIsCompletedStart++)
#else

	for(/*g_nchunkoffIsCompletedStart*/; g_nchunkoffIsCompletedStart <= g_nchunkoffPrdnEof; g_nchunkoffIsCompletedStart++)
#endif
	{
		if(!PRDN_ISVALID(g_nchunkoffIsCompletedStart))
		{
			return FALSE;
		}
	}

	PrintDebug("===== PROGRESS DN COMPLETE =====\n");
	PrintDebug("===== PROGRESS DN COMPLETE =====\n");
	PrintDebug("===== PROGRESS DN COMPLETE =====\n");

	g_nPrdnInfoBufferingPercent = 100*1000;
	g_nPrdnInfoBufferingLeftTime = 0;

	{ /* wait ultil call back is registered */
		HUINT32 wait_counter = 0;
		while(TRUE)
		{
			if(g_pReadyStateCallbackFunc || wait_counter >= 7)
			{
				break;
			}
			VK_TASK_Sleep(1000);
			wait_counter++;
		}
	}

	if(g_pReadyStateCallbackFunc)
	{
		g_pReadyStateCallbackFunc(g_nPrdnCBulDeviceId, DI_MEDIA_EVENT_BUFFER_READY_STATE, DI_IPTUNER_DNP_READYSTATE_FULLDOWN_COMPLETED); /* 3 == DOWNLOAD COMPLETED */
		g_pReadyStateCallbackFunc = NULL;
	}

#ifdef DEBUG_DN_PROGRESS_ARRAY
	PrintExit;
#endif

	return TRUE;
}
#endif


DI_ERR_CODE DI_IPTUNER_DnpSetReadyStateCallback(HUINT32 ulDeviceId, fn_readystate_callback pFn)
{
	DI_ERR_CODE nRet=DI_ERR_OK;

	g_pReadyStateCallbackFunc = (fn_readystate_callback)pFn;
	//if(g_pReadyStateCallbackFunc)
	//{
	//		g_pReadyStateCallbackFunc(g_nPrdnCBulDeviceId, DI_MEDIA_EVENT_BUFFER_READY_STATE, g_nPrdnInfoReadyState);
	//}
	g_nPrdnCBulDeviceId = ulDeviceId;
	g_nPrdnInfoLastReadyStateSentTime = VK_TIMER_GetSystemTick() ; /* initial ready state callback must be sent 1 sec later */

	return nRet;
}

void P_IPTUNER_DnpSetCurrentDataReadOffset(HUINT64 ullCurrentDataReadOffset)
{
	s_ullDataReadOffset = ullCurrentDataReadOffset;
}

void P_IPTUNER_DnpSetLastReadSize(HUINT64 ullReadSize)
{
	s_ullLastReadSize = ullReadSize;
}


HUINT64 DI_IPTUNER_DnpGetCurrentDataReadOffset(void)
{
	return s_ullDataReadOffset;
}

HUINT64 DI_IPTUNER_DnpGetLastReadSize(void)
{
	return s_ullLastReadSize;
}


void P_IPTUNER_DnpSetDownloadBytesPerSecond(HINT32 nDownloadBytesPerSecond)
{
	s_nDownloadBytesPerSecond = nDownloadBytesPerSecond;
}

HINT32 DI_IPTUNER_DnpGetDownloadBytesPerSecond(void)
{
	return s_nDownloadBytesPerSecond;
}

DI_ERR_CODE DI_IPTUNER_DnpGetBufferedDataSize(HUINT64 *pullBufferedData, HBOOL *bIsFullChunk, HINT64 llFileLength)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HINT32 i = 0, nFirstIndex = 0, nEndIndex = 0, nDownloadArraySize = 0, nLastDataChunk = 0;
	HUINT64 ullCurrentDataReadOffset = 0;

	if(pullBufferedData == NULL)
	{
		PrintError("@@@@@ [%s:%d] pullBufferedData is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	if(bIsFullChunk == NULL)
	{
		PrintError("@@@@@ [%s:%d] bIsFullChunk is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	nDownloadArraySize = PRDN_GET_BYTE_OFF(g_nchunkoffPrdnEof) + 1;

	ullCurrentDataReadOffset = DI_IPTUNER_DnpGetCurrentDataReadOffset();

	nFirstIndex = (HINT32)(ullCurrentDataReadOffset >> PRDN_CHUNK_EXPONENT);
	nEndIndex = (HINT32)((llFileLength - 1) >> PRDN_CHUNK_EXPONENT);

	for(i = nFirstIndex; i <= nEndIndex; i++)
	{
		if(!(*((HUINT8 *)g_pucPrdnChunkDownloadStatusArray + (i >> 3)) & (1 << (i & 0x07))))
		{
			*bIsFullChunk = FALSE;
			break;
		}
	}

	if(i > nEndIndex)
	{
		*pullBufferedData += (llFileLength - ullCurrentDataReadOffset);
	}
	else
	{
		nLastDataChunk = i - 1;
		if(nLastDataChunk < nFirstIndex)
		{
			*pullBufferedData += 0;
		}
		else
		{
			*pullBufferedData += ((nLastDataChunk - nFirstIndex) << PRDN_CHUNK_EXPONENT);
		}
	}

exit:
	return nRet;
}

DI_ERR_CODE P_IPTUNER_DnpGetDownloadOffset(HUINT64 *pullDownloadOffset)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HINT32 nDownloadArraySize = 0, i = 0, nFirstIndex = 0, nEndIndex = 0;
	HUINT64 ullCurrentDataReadOffset = 0;
	HINT64 llFileLength = 0;
	HBOOL bIsFullChunk = FALSE;

	if(pullDownloadOffset == NULL)
	{
		PrintError("@@@@@ [%s:%d] pullDownloadOffset is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	if(s_nFileLength == 0)
	{
		PrintError("@@@@@ [%s:%d] s_nFileLength is 0\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}
	else
	{
		llFileLength = s_nFileLength;
	}

	if(g_nchunkoffPrdnEof == 0)
	{
		PrintError("[%s:%d] g_nchunkoffPrdnEof == %llu\n", __func__, __LINE__, g_nchunkoffPrdnEof);
		nRet = DI_ERR_ERROR;
		goto exit;
	}
	else
	{
		nDownloadArraySize = PRDN_GET_BYTE_OFF(g_nchunkoffPrdnEof) + 1;
	}

	if(g_pucPrdnChunkDownloadStatusArray == NULL)
	{
		PrintError("[%s:%d] g_pucPrdnChunkDownloadStatusArray is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	ullCurrentDataReadOffset = DI_IPTUNER_DnpGetCurrentDataReadOffset();

	nFirstIndex = (HINT32)(ullCurrentDataReadOffset >> PRDN_CHUNK_EXPONENT);
	nEndIndex = (HINT32)((llFileLength - 1) >> PRDN_CHUNK_EXPONENT);

	bIsFullChunk = TRUE;
	for(i = nFirstIndex; i <= nEndIndex; i++)
	{
		if(!(*((HUINT8 *)g_pucPrdnChunkDownloadStatusArray + (i >> 3)) & (1 << (i & 0x07))))
		{
			bIsFullChunk = FALSE;
			break;
		}
	}

	if(bIsFullChunk)
	{
		*pullDownloadOffset = llFileLength;
	}
	else
	{
		*pullDownloadOffset = (HUINT64)i << PRDN_CHUNK_EXPONENT;
	}

exit:
	return nRet;
}

DI_ERR_CODE P_IPTUNER_DnpGetDownloadedBytes(HUINT64 *pullDownloadedBytes)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HINT32 nDownloadArraySize = 0, i = 0, nEndIndex = 0;
	HUINT32 ulDownloadedChunks = 0;
	HINT64 llFileLength = 0;

	if(pullDownloadedBytes == NULL)
	{
		PrintError("@@@@@ [%s:%d] pullDownloadedBytes is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	if(s_nFileLength == 0)
	{
		PrintError("@@@@@ [%s:%d] s_nFileLength is 0\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}
	else
	{
		llFileLength = s_nFileLength;
	}

	if(g_nchunkoffPrdnEof == 0)
	{
		PrintError("[%s:%d] g_nchunkoffPrdnEof == %llu\n", __func__, __LINE__, g_nchunkoffPrdnEof);
		nRet = DI_ERR_ERROR;
		goto exit;
	}
	else
	{
		nDownloadArraySize = PRDN_GET_BYTE_OFF(g_nchunkoffPrdnEof) + 1;
	}

	if(g_pucPrdnChunkDownloadStatusArray == NULL)
	{
		PrintError("[%s:%d] g_pucPrdnChunkDownloadStatusArray is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	nEndIndex = (HINT32)((llFileLength - 1) >> PRDN_CHUNK_EXPONENT);

	for(i = 0; i <= nEndIndex; i++)
	{
		if(*((HUINT8 *)g_pucPrdnChunkDownloadStatusArray + (i >> 3)) & (1 << (i & 0x07)))
		{
			ulDownloadedChunks++;
		}
	}

	*pullDownloadedBytes = (HUINT64)ulDownloadedChunks << PRDN_CHUNK_EXPONENT;

exit:
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_DnpInitDownloadSpeed(HULONG *pulStartClock)
	{
	DI_ERR_CODE nRet = DI_ERR_OK;

	if(pulStartClock == NULL)
	{
		PrintError("@@@@@ [%s:%d] pulStartClock is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	*pulStartClock = VK_TIMER_GetSystemTick();

exit:
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_DnpUpdateDownloadSpeed(HULONG ulStartClock, HINT32 nDownloadBytes, HUINT32 ulLoopCount)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HULONG ulElapsedMilliSec = 0;
	HINT64 llDownloadSpeed = 0;
	static	HINT64	llAveDownloadSpeed = 0;

	ulElapsedMilliSec = VK_TIMER_GetSystemTick();
	ulElapsedMilliSec -= ulStartClock;

	if(ulElapsedMilliSec < DNP_DOWNLOAD_SPEED_CHECK_TIME_LIMIT)
	{
		PrintDebug("@@@@@ [%s:%d] ulElapsedMilliSec = %ld, can't divide\n", __func__, __LINE__, ulElapsedMilliSec);
		nRet = DI_ERR_INVALID_PARAM;
		goto exit;
	}

	PrintDebug("@@@@@ [%s:%d] ulLoopCount = %d, nDownloadBytes = %d, ulElapsedMilliSec = %ld\n", __func__, __LINE__, ulLoopCount, nDownloadBytes * ulLoopCount, ulElapsedMilliSec);

	llDownloadSpeed = ((HINT64)nDownloadBytes * ulLoopCount * 1000) / ulElapsedMilliSec;

	PrintDebug("@@@@@ [%s:%d] llDownloadSpeed = %lu\n", __func__, __LINE__, llDownloadSpeed);

	llAveDownloadSpeed = (llAveDownloadSpeed*6)/10 + (llDownloadSpeed*4)/10;

	PrintDebug("@@@@@ [%s:%d] llAveDownloadSpeed = %lu\n", __func__, __LINE__, llAveDownloadSpeed);
	P_IPTUNER_DnpSetDownloadBytesPerSecond(llAveDownloadSpeed);

	if(g_pReadyStateCallbackFunc)
	{
		PrintDebug("@@@@@ [%s:%d] # Updating Buffering Point.. #\n", __func__, __LINE__);
		g_pReadyStateCallbackFunc(g_nPrdnCBulDeviceId, DI_MEDIA_EVENT_BUFFER_READY_STATE, DI_IPTUNER_DNP_READYSTATE_BUFFERING_POINT_UPDATE);
	}
exit:
	return nRet;
	}

/**
 * @brief			Buffering Information(Buffering Percent, Left time)을 구하는 함수
 *
 * @param[In]		unDuration			Media Duration
 * @return			DI_ERR_CODE
 * @date 			2013/05/13
 * @author 			thoh@humaxdigital.com
 * @see
 */
DI_ERR_CODE DI_IPTUNER_DnpCalcBufferingInfo(HUINT32 unDuration)
	{
	DI_ERR_CODE nRet = DI_ERR_OK;

#ifdef ENABLE_MAPPED_DNP
	HUINT64 ullReferenceData = 0, ullBufferedData = 0, ullLeftDownloadData = 0;
	HUINT32 unVideoBufferDepth = 0, unAudioBufferDepth = 0;
	HINT32	nDownloadBytesPerSecond = 0;
	HBOOL	bIsFullChunk = FALSE;

	if(unDuration == 0)
	{
		PrintDebug("@@@@@ [%s:%d] unDuration is equals to %d\n", __func__, __LINE__, unDuration);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	if(s_nFileLength == 0)
	{
		PrintError("@@@@@ [%s:%d] s_nFileLength is equals to %d\n", __func__, __LINE__, s_nFileLength);
		nRet = DI_ERR_ERROR;
		goto exit;
	}

	ullReferenceData = (DNP_START_THRESHOLD + 1000) * (s_nFileLength / unDuration);
	if(ullReferenceData == 0)
	{
		PrintDebug("@@@@@ [%s:%d] ullReferenceData is equals to %llu\n", __func__, __LINE__, ullReferenceData);
		nRet = DI_ERR_ERROR;
		goto exit;
	}
	else
	{
		nRet = DI_MEDIA_GetVideoBufferDepth(&unVideoBufferDepth);
		if(nRet != DI_ERR_OK)
		{
			PrintError("@@@@@ [%s:%d:%d] Getting VideoBufferDepth is Error\n", __func__, __LINE__, nRet);
			goto exit;
		}
		ullBufferedData += unVideoBufferDepth;

		nRet = DI_MEDIA_GetAudioBufferDepth(&unAudioBufferDepth);
		if(nRet != DI_ERR_OK)
		{
			PrintError("@@@@@ [%s:%d:%d] Getting AudioBufferDepth is Error\n", __func__, __LINE__, nRet);
			goto exit;
		}
		ullBufferedData += unAudioBufferDepth;

		bIsFullChunk = TRUE;
		nRet = DI_IPTUNER_DnpGetBufferedDataSize(&ullBufferedData, &bIsFullChunk, s_nFileLength);
		if(nRet != DI_ERR_OK)
		{
			PrintError("@@@@@ [%s:%d:%d] Getting BufferedData is Error\n", __func__, __LINE__, nRet);
			goto exit;
		}

		if(ullReferenceData < ullBufferedData)
		{
			ullLeftDownloadData = 0;
			ullBufferedData = ullReferenceData;
		}
		else
		{
			ullLeftDownloadData = ullReferenceData - ullBufferedData;
		}

		g_nPrdnInfoBufferingPercent = ullBufferedData * 100000 / ullReferenceData;
		PrintDebug("@@@@@ [%s:%d] g_nPrdnInfoBufferingPercent = %d\n", __func__, __LINE__, g_nPrdnInfoBufferingPercent);

		if(g_nPrdnInfoBufferingPercent > MAX_PDL_BUFFERING_PERCENT)
		{
			g_nPrdnInfoBufferingLeftTime = 0;
		}
		else
		{
			nDownloadBytesPerSecond = DI_IPTUNER_DnpGetDownloadBytesPerSecond();
			if(nDownloadBytesPerSecond > 0)
			{
				g_nPrdnInfoBufferingLeftTime = ullLeftDownloadData * 1000 / nDownloadBytesPerSecond;
				PrintDebug("@@@@@ [%s:%d] g_nPrdnInfoBufferingLeftTime = %d\n", __func__, __LINE__, g_nPrdnInfoBufferingLeftTime);
			}
			else
			{
				PrintDebug("@@@@@ [%s:%d] nDownloadBytesPerSecond is equal to 0\n", __func__, __LINE__);
			}
		}
	}

#else
	PrintError("@@@@@ [%s:%d] Calcurating Buffering Information is NOT SUPPORT.(undefined ENABLE_MAPPED_DNP)\n", __func__, __LINE__);
	nRet = DI_ERR_ERROR;
#endif

exit:
	return nRet;
}

void DI_IPTUNER_DnpUnderflowMonitorAwake(void)
{
	DI_IPTUNER_UnderflowMonitorAwakeThread();
}

void DI_IPTUNER_DnpUnderflowMonitorSleep(void)
{
	DI_IPTUNER_UnderflowMonitorSleepThread();
}
