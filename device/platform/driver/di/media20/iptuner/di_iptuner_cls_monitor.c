/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_cls_monitor.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	Tae-hoon Oh $
 * Current Author:	$Author: thoh@humaxdigital.com $
 * Date:			$Date: Fri March 29 15:12:48 KST 2013 $
 * File Description:	Check underflow	state
 * Module: 			    IPTUNER Underflow Control for HTTP chunked live streaming(CLS)
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2013 HUMAX Co., Ltd. 								*/
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
#include "bfile_types.h"
#include "htype.h"
#include "di_uart.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_iptuner20.h"
#include "di_iptuner_pool.h"
//#include "di_iptuner_dnp.h"
#include "di_iptuner_stream.h" /* unsupported dnp */
#include "di_iptuner_http.h"
#include "di_iptuner_cls_monitor.h"
#include "util_sys.h"
/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define IPTUNER_ALL_DEBUG
#define DI_IPTUNER_CLS_MONITOR_DEBUG

/* DEBUG DEFINE */
#ifdef IPTUNER_ALL_DEBUG

#ifdef DI_IPTUNER_CLS_MONITOR_DEBUG
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
#endif /* END DEBUG DEFINE */

#define UNUSED_PARAM(x)  ((void)x)


#define MAX_IP_SIZE			(128)	/** bytes */
#define MAX_PORT_SIZE		(16)	/** bytes */
#define MAX_FILE_NAME_SIZE	(IPTUNER_CONST_URL_LENGTH)	/** bytes */
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef enum
{
	BUFFER_MON_MSG_UNDERFLOW = 1,
	BUFFER_MON_MSG_STOP
} BUFFER_MON_MSG_e;
/* End typedef */

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
/* Start global variable */
/* End global variable */

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
/* Start static variable */
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
static DI_ERR_CODE P_IPTUNER_CLS_SendReadyStateEvent(DI_IPTUNER_t *pHTuner, HBOOL isUnderflow);


static DI_ERR_CODE P_IPTUNER_CLS_CheckUnderflow(DI_IPTUNER_t *pHTuner)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HUINT32 unVideoBufferDepth = 0, unVideoFifoSize = 0;
	HUINT32 unAudioBufferDepth = 0, unAudioFifoSize = 0;
	HUINT32	uPumpBufferDepth = 0, uPumpFifoSize = 0;
	HUINT32 unUnderflowThreshold = 0, unEnoughThreshold = 0;
	HBOOL isUnderflow = FALSE;
	HUINT64 uAVBufferDepth = 0;
	HUINT64 uTotalBufferedData = 0;
	/*
	HINT64 i_pos = 0;
	HINT64 i_size = 0;
	*/
	PrintEnter;

	if (pHTuner == NULL)
	{
		PrintError("[%s][%d] Error> Invalid parameter!\n", __FUNCTION__,__LINE__);
		return DI_ERR_ERROR;
	}

	if (pHTuner->eProtocolType != DI_IPTUNER_PROTOCOL_HTTP)
	{
		PrintError("@@@@@ [%s:%d:%d] Error> unsupproted protocol(%d)!\n", __FUNCTION__, __LINE__, pHTuner->eProtocolType);
		return DI_ERR_ERROR;
	}

	/* get video decoder fifo depth */
	if (pHTuner->tVideoBufferCallback != NULL)
	{
		(void)pHTuner->tVideoBufferCallback((HUINT32)pHTuner, &unVideoBufferDepth, &unVideoFifoSize);
		uTotalBufferedData += unVideoBufferDepth;
	}
	else
	{
		PrintError("@@@@@ [%s:%d:%d] Error> No registered videoBufferCallback!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	/* get audio decoder fifo depth */
	if (pHTuner->tAudioBufferCallback != NULL)
	{
		(void)pHTuner->tAudioBufferCallback((HUINT32)pHTuner, &unAudioBufferDepth, &unAudioFifoSize);
		/* uTotalBufferedData += unAudioBufferDepth; */
	}
	else
	{
		PrintError("@@@@@ [%s:%d:%d] Error> No registered audioBufferCallback!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	/* get playpump fifo depth */
	if (pHTuner->tPumpBufferCallback != NULL)
	{
		(void)pHTuner->tPumpBufferCallback((HUINT32)pHTuner, &uPumpBufferDepth, &uPumpFifoSize);
		uTotalBufferedData += uPumpBufferDepth;
	}
	else
	{
		PrintError("@@@@@ [%s:%d:%d] Error> No registered pumpBufferCallback!\n", __FUNCTION__, __LINE__);
		return DI_ERR_ERROR;
	}

	uAVBufferDepth = unVideoBufferDepth + unAudioBufferDepth;

	if (0)
	{
		if (pHTuner->eProtocolType == DI_IPTUNER_PROTOCOL_HTTP)
		{
			/* Underflow 조건은 버퍼가 0.25초 정도 재생할 분량에 못미치는 경우이다. */
			unUnderflowThreshold = (pHTuner->tMediaInfo.max_bitrate >> 5);
			/* Resume 조건은 버퍼가 0.5초 정도 재생할 분량이 나오면 Resume한다. */
			unEnoughThreshold = (pHTuner->tMediaInfo.max_bitrate >> 4);

			PrintDebug("@@@@@ [%s:%d] MaxBitrate(MBytes/S) = %d\n", __FUNCTION__, __LINE__, (pHTuner->tMediaInfo.max_bitrate >> 3));
		}
		else
		{
			/* Underflow 조건은 버퍼가 MaxByterate X 8한 값의 15퍼센트에 못미치는 경우이다. */
			unUnderflowThreshold = ((pHTuner->tMediaInfo.max_bitrate >> 3) * CHECK_UNDERFLOW_SECOND_VALUE * CHECK_UNDERFLOW_PERCENT_VALUE) / 100;
			/* Resume 조건은 버퍼가 MaxByteRate X 5한 값, 즉 5초 정도 재생할 분량이 나오면 Resume한다. */
			unEnoughThreshold = (pHTuner->tMediaInfo.max_bitrate >> 3) * CHECK_RESUME_SECOND_VALUE;
		}
	}
	else
	{
		/*
		 * HTTP chunked live streaming (CLS) don't use max_bitrate for determining underflow
		 *     because max_bitrate after media probe() can't be trusted.
		 * Instead, use playpump fifo size and decoder's underflow event for referencing underflow/overflow.
		 * Check playpump fifo size : (1.5.x*2) == 3.x Mbyte; (2 Mbps == 262,144 Bytes/sec)
		 */
		if (pHTuner->eClsBufferState == DI_IPTUNER_CLS_BUFSTATE_PREPARING)
		{
			unUnderflowThreshold = (uPumpFifoSize/2); // 1310720; // (2 Mbps == 262,144 Bytes/sec) * 5 sec
			unEnoughThreshold = (uPumpFifoSize/4)*3; // 26,214,400 == 2 * 1024 * 1024 <-- 3/4
		}
		else
		{
			unUnderflowThreshold = (uPumpFifoSize/3); // (2 Mbps == 262,144 Bytes/sec) * 5 sec
			unEnoughThreshold = (uPumpFifoSize/3)*2; // 26,214,400 == 2 * 1024 * 1024 <-- 3/4
		}

		/* Also, use decode_status.underflow_event */
	}
	PrintDebug("@@@@@ [%s:%d] State(%d), UnderflowThreshold:%d(fifo:%d), AVBufferDepth:%llu(fifo:%d), EnoughThreshold:%d\n", __FUNCTION__, __LINE__, pHTuner->eClsBufferState, unUnderflowThreshold, uPumpFifoSize, uAVBufferDepth, uPumpFifoSize, unEnoughThreshold);

	if(uAVBufferDepth > unEnoughThreshold)
	{
		isUnderflow = FALSE;
	}
	else if(uAVBufferDepth < unUnderflowThreshold)
	{
		if (uPumpBufferDepth > unUnderflowThreshold)
		{
			isUnderflow = FALSE;
		}
		else
		{
			isUnderflow = TRUE;
			/* No meaning for chunked live streaming */
			/*
			if (0)
			{
				if (DI_STREAM_HTTP_GetStreamInfo(pHTuner, &i_pos, &i_size) != ERR_OK)
				{
					i_pos = 0;
					i_size = 0
				}
				if (i_size < 0) i_size = 0;
				if (i_pos < 0) i_pos = 0;

				if (i_size - i_pos < unUnderflowThreshold)
				{
					PrintDebug("@@@@@ [%s:%d] Remain data is lower than Underflow Threshold\n", __FUNCTION__, __LINE__);
					isUnderflow = FALSE;
				}
			}
			*/
		}
	}
	else
	{
		if ((pHTuner->eClsBufferState == DI_IPTUNER_CLS_BUFSTATE_BUFFERING) || (pHTuner->eClsBufferState == DI_IPTUNER_CLS_BUFSTATE_PREPARING))
		{
			isUnderflow = TRUE;
			/* No meaning for chunked live streaming */
			/*
			if (0)
			{
				if (i_size - i_pos <= (HINT64)uAVBufferDepth)
				{
					isUnderflow = FALSE;
				}

				if (uPumpBufferDepth + uAVBufferDepth > unEnoughThreshold)
				{
					isUnderflow = FALSE;
				}
			}
			*/
		}
	}

	if(isUnderflow == FALSE)	/* 현재 Underflow가 아닌 경우 */
	{
		if ((pHTuner->eClsBufferState == DI_IPTUNER_CLS_BUFSTATE_BUFFERING) || (pHTuner->eClsBufferState == DI_IPTUNER_CLS_BUFSTATE_PREPARING))
		{
			nRet = P_IPTUNER_CLS_SendReadyStateEvent(pHTuner, isUnderflow);
			if(nRet != DI_ERR_OK)
			{
				PrintError("@@@@@ [%s:%d:%d] Sending Event is Failed\n", __FUNCTION__, __LINE__, nRet);
				goto func_error;
			}
		}
	}
	else	 /* 현재 Underflow인 경우 */
	{
		if (pHTuner->eClsBufferState == DI_IPTUNER_CLS_BUFSTATE_COMPLETED)
		{
			nRet = P_IPTUNER_CLS_SendReadyStateEvent(pHTuner, isUnderflow);
			if(nRet != DI_ERR_OK)
			{
				PrintError("@@@@@ [%s:%d:%d] Sending Event is Failed\n", __FUNCTION__, __LINE__, nRet);
				goto func_error;
			}
		}
	}

func_error:
	PrintDebug("@@@@@ [%s:%d] Underflow(%d), State(%d), UnderflowThreshold:%d(fifo:%d), AVBufferDepth:%llu(fifo:%d), EnoughThreshold:%d\n", __FUNCTION__, __LINE__, isUnderflow, pHTuner->eClsBufferState, unUnderflowThreshold, uPumpFifoSize, uAVBufferDepth, uPumpFifoSize, unEnoughThreshold);
	PrintExit;
	return nRet;
}
static DI_ERR_CODE P_IPTUNER_CLS_CheckBufferingTimeOver(DI_IPTUNER_t *pHTuner, HBOOL *pbCheckBufferingTimeOver, HULONG *pulStartBufferingClock, HBOOL *pbIsContinue)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HULONG ulEndBufferingClock = 0, ulSubtractBufferingClock = 0;
	HUINT64 uAVBufferDepth = 0;
	HUINT32 unVideoBufferDepth = 0, unVideoFifoSize = 0;
	HUINT32 unAudioBufferDepth = 0, unAudioFifoSize = 0;
	HUINT32 unUnderflowThreshold = 0;
	HBOOL isUnderflow = FALSE;
	PrintEnter;

	if ( NULL == pHTuner )
	{
		PrintError("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	if(pbCheckBufferingTimeOver == NULL)
	{
		PrintError("@@@@@ [%s:%d] pbCheckBufferingTimeOver is NULL\n", __FUNCTION__, __LINE__);
		return DI_ERR_INVALID_PARAM;
	}

	if(pulStartBufferingClock == NULL)
	{
		PrintError("@@@@@ [%s:%d] pulStartBufferingClock is NULL\n", __FUNCTION__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto func_error;
	}

	if(*pbCheckBufferingTimeOver == FALSE)
	{
		*pulStartBufferingClock = VK_TIMER_GetSystemTick();
		*pbCheckBufferingTimeOver = TRUE;
	}
	else
	{
		ulEndBufferingClock = VK_TIMER_GetSystemTick();
		ulSubtractBufferingClock = ulEndBufferingClock - *pulStartBufferingClock;

		if ((pHTuner->eClsBufferState == DI_IPTUNER_CLS_BUFSTATE_BUFFERING) || (pHTuner->eClsBufferState == DI_IPTUNER_CLS_BUFSTATE_PREPARING))
		{
			if(ulSubtractBufferingClock >= BUFFERING_TIMEOUT)
			{
				if (pHTuner->tVideoBufferCallback != NULL)
				{
					pHTuner->tVideoBufferCallback((HUINT32)pHTuner, &unVideoBufferDepth, &unVideoFifoSize);
				}
				else
				{
					PrintError("@@@@@ [%s:%d:%d] Getting VideoBufferDepth is Error\n", __FUNCTION__, __LINE__, nRet);
					goto func_error;
				}

				if (pHTuner->tAudioBufferCallback != NULL)
				{
					pHTuner->tAudioBufferCallback((HUINT32)pHTuner, &unAudioBufferDepth, &unAudioFifoSize);
				}
				else
				{
					PrintError("@@@@@ [%s:%d:%d] Getting unAudioBufferDepth is Error\n", __FUNCTION__, __LINE__, nRet);
					goto func_error;
				}

				uAVBufferDepth = unVideoBufferDepth + unAudioBufferDepth;
				if (pHTuner->eProtocolType == DI_IPTUNER_PROTOCOL_HTTP)
				{
					/* Underflow 조건은 버퍼가 0.25초 정도 재생할 분량에 못미치는 경우이다. */
					unUnderflowThreshold = (pHTuner->tMediaInfo.max_bitrate >> 5);
				}
				else
				{
					unUnderflowThreshold = ((pHTuner->tMediaInfo.max_bitrate >> 3) * CHECK_UNDERFLOW_SECOND_VALUE * CHECK_UNDERFLOW_PERCENT_VALUE) / 100;
				}
				PrintDebug("@@@@@ [%s:%d] State(%d), UnderflowThreshold:%d(fifo:%d), AVBufferDepth:%llu(fifo:%d)\n", __FUNCTION__, __LINE__, pHTuner->eClsBufferState, unUnderflowThreshold, unVideoFifoSize, uAVBufferDepth, unAudioFifoSize);

				if(uAVBufferDepth < unUnderflowThreshold)
				{
					PrintDebug("@@@@@ [%s:%d] # uAVBufferDepth(%llu) is lower than Underflow Threshold(%d), can't resume..\n", __FUNCTION__, __LINE__, uAVBufferDepth, unUnderflowThreshold);
					*pbIsContinue = TRUE;
					isUnderflow = TRUE;
					goto func_error;
				}
				else
				{
					isUnderflow = FALSE;
				}

				nRet = P_IPTUNER_CLS_SendReadyStateEvent(pHTuner, isUnderflow);
				if(nRet != DI_ERR_OK)
				{
					PrintError("@@@@@ [%s:%d:%d] Error> Sending Event is Failed\n", __FUNCTION__, __LINE__, nRet);
					goto func_error;
				}
				else
				{
					*pbIsContinue = TRUE;
					*pbCheckBufferingTimeOver = FALSE;
				}
			}
		}
		else
		{
			*pbCheckBufferingTimeOver = FALSE;
		}
	}

func_error:
	PrintExit;
	return nRet;
}

static DI_ERR_CODE P_IPTUNER_CLS_SendReadyStateEvent(DI_IPTUNER_t *pHTuner, HBOOL isUnderflow)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	PrintEnter;

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_INVALID_PARAM;
		return nRet;
	}

	if (pHTuner->tLiveBufferStateCallback)
	{
		switch(pHTuner->eClsBufferState)
		{
			case DI_IPTUNER_CLS_BUFSTATE_PREPARING:
			case DI_IPTUNER_CLS_BUFSTATE_BUFFERING:
				PrintDebug("\033[37;45m" "@@@@@ [%s:%d] # [bstate:%d, isUnderflow:%d] for PAUSE -> RESUME\n" "\033[0m", __FUNCTION__, __LINE__, pHTuner->eClsBufferState, isUnderflow);
				//if (isUnderflow == FALSE)
				pHTuner->tLiveBufferStateCallback((HUINT32)pHTuner, DI_IPTUNER_CLS_BUFSTATE_COMPLETED);
				pHTuner->eClsBufferState = DI_IPTUNER_CLS_BUFSTATE_COMPLETED;
				break;

			case DI_IPTUNER_CLS_BUFSTATE_COMPLETED:
				PrintDebug("\033[37;45m" "@@@@@ [%s:%d] # [bstate:%d, isUnderflow:%d] for RESUME -> PAUSE\n" "\033[0m", __FUNCTION__, __LINE__, pHTuner->eClsBufferState, isUnderflow);
				//if (isUnderflow == TRUE)
				pHTuner->tLiveBufferStateCallback((HUINT32)pHTuner, DI_IPTUNER_CLS_BUFSTATE_BUFFERING);
				pHTuner->eClsBufferState = DI_IPTUNER_CLS_BUFSTATE_BUFFERING;
				break;

			default:
				break;
		}
	}
	else
	{
		PrintError("@@@@@ [%s:%d] Error> LiveBufferStateCallback is not set\n", __FUNCTION__, __LINE__);
		goto func_error;
	}

func_error:
	PrintExit;
	return nRet;
}

static void P_IPTUNER_CLS_MonitorTask(void *pParam)
{
	DI_IPTUNER_Msg_t stMsg;
	HINT32 nVKRet = ERR_FAIL;
	HUINT32 unUnderflowThreshold = 0;
	DI_ERR_CODE nRet = DI_ERR_OK;
	HBOOL bCheckBufferingTimeOver = FALSE, bIsContinue = FALSE;
	HULONG ulStartBufferingClock = 0;
	DI_IPTUNER_t *pHTuner = (DI_IPTUNER_t *)pParam;
	PrintEnter;

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		return;
	}

	if (pHTuner->eClsBufferState != DI_IPTUNER_CLS_BUFSTATE_PREPARING)
	{
		pHTuner->eClsBufferState = DI_IPTUNER_CLS_BUFSTATE_PREPARING;
	}

	unUnderflowThreshold = ((pHTuner->tMediaInfo.max_bitrate >> 3) * CHECK_UNDERFLOW_SECOND_VALUE * CHECK_UNDERFLOW_PERCENT_VALUE) / 100;

	(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));

	pHTuner->eClsMonitorStatus = DI_IPTUNER_CLS_MONITOR_STARTED;

	while (pHTuner->eClsMonitorStatus != DI_IPTUNER_CLS_MONITOR_STOPPED)
	{
		nVKRet = VK_MSG_ReceiveTimeout (pHTuner->ulClsMonitorQid, &stMsg, sizeof(DI_IPTUNER_Msg_t), 1000);
		if(nVKRet == VK_OK)
		{
			if ( stMsg.unMsg == BUFFER_MON_MSG_STOP )
			{
				PrintDebug ("%s(%d) : BUFFER_MON_MSG_STOP Received. \n",__FUNCTION__,__LINE__);
				goto UNDERFLOW_MONITOR_EXIT;
			}
		}
		else if(nVKRet == VK_TIMEOUT)
		{
			/* Check Buffer Time-over */
			PrintDebug("@@@@@ [%s:%d] Before P_IPTUNER_CLS_CheckBufferingTimeOver()\n", __FUNCTION__, __LINE__);
			nRet = P_IPTUNER_CLS_CheckBufferingTimeOver(pHTuner, &bCheckBufferingTimeOver, &ulStartBufferingClock, &bIsContinue);
			if(nRet != DI_ERR_OK)
			{
				PrintError("@@@@@ [%s:%d:%d] Error> BufferingTimeOver() failed\n", __FUNCTION__, __LINE__, nRet);
				continue;
			}

			if(bIsContinue)
			{
				bIsContinue = FALSE;
				continue;
			}

			/* Check Underflow */
			PrintDebug("@@@@@ [%s:%d] Before P_IPTUNER_CLS_CheckUnderflow()\n", __FUNCTION__, __LINE__);
			nRet = P_IPTUNER_CLS_CheckUnderflow(pHTuner);
			if(nRet != DI_ERR_OK)
			{
				PrintError("@@@@@ [%s:%d:%d] Error> Checking Underflow is failed\n", __FUNCTION__, __LINE__, nRet);
			}
		}
	}

UNDERFLOW_MONITOR_EXIT:
	pHTuner->eClsMonitorStatus = DI_IPTUNER_CLS_MONITOR_STOPPED;
	PrintExit;
	return;
}

void DI_IPTUNER_CLS_SetLiveBufferState(DI_IPTUNER_t *pHTuner, DI_IPTUNER_CLS_BUFSTATE_e eUnderflowState)
{
	PrintEnter;

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		return;
	}

	pHTuner->eClsBufferState = (DI_IPTUNER_CLS_BUFSTATE_e)eUnderflowState;

	/* Ready State가 BUFFERING COMPLETED인 경우, 해당 이벤트를 APP에 올려준다. */
	if (pHTuner->eClsBufferState == DI_IPTUNER_CLS_BUFSTATE_COMPLETED && pHTuner->tLiveBufferStateCallback != NULL)
	{
		pHTuner->tLiveBufferStateCallback((HUINT32)pHTuner, DI_IPTUNER_CLS_BUFSTATE_COMPLETED);
	}

	return;
}

HINT32 DI_IPTUNER_CLS_MonitorStart(DI_IPTUNER_t *pHTuner)
{
	DI_IPTUNER_Msg_t stMsg;
	HINT32 nVKRet = ERR_OK;

	PrintEnter;

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}
	PrintData("%s(%d) TunerHandle=[%p] URL=[%s]\n",__FUNCTION__,__LINE__, pHTuner, pHTuner->pUrl);

	/* check Tuner url */
	if ( NULL == pHTuner->pUrl)
	{
		PrintError("%s(%d) Error> Tuner Handle has invalid url (pUrl is NULL)\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	if (pHTuner->bIsProbeDone != TRUE)
	{
		PrintError("%s(%d) Error> Tuner Handle has invalid url (pUrl is NULL)\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	if (pHTuner->eProtocolType != DI_IPTUNER_PROTOCOL_HTTP)
	{
		PrintError("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}
	else
	{
		if (pHTuner->tMediaInfo.max_bitrate == 0)
		{
			pHTuner->tMediaInfo.max_bitrate = 2500000;	/* Assume 2.5 Mbps */
		}
		if (pHTuner->tMediaInfo.duration == 0)
		{
			pHTuner->tMediaInfo.duration = 0xFFFFFFFF;
		}
	}

	if (pHTuner->tMediaInfo.max_bitrate == 0)
	{
		PrintError ("%s (%d) Error> Invaild Media Bitrate!!\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	if (pHTuner->tMediaInfo.duration <= 0)
	{
		PrintError("@@@@@ [%s:%d] Error> invalid duration <= 0\n", __FUNCTION__, __LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	/*
	if(llFileLength == 0)
	{
		PrintError("@@@@@ [%s:%d] Error> file length is 0\n", __FUNCTION__, __LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}
	*/

	//i_size = llFileLength;

	if (pHTuner->eClsMonitorStatus != DI_IPTUNER_CLS_MONITOR_STOPPED)
	{
		PrintError  ("%s (%d) Error>  Already initialized !\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_OK;
		goto exit;
	}

	/* create & init message que */
	if (pHTuner->ulClsMonitorQid != 0x0)
	{
		PrintError("%s(%d) Error> Msg was not destroyed.\n",__FUNCTION__, __LINE__);
	}

	nVKRet = VK_MSG_Create(TUNER_UNDERFLOW_MSG_QUEUE_SIZE, sizeof(stMsg), "CLS_MONITOR_MSGQ", &pHTuner->ulClsMonitorQid, VK_SUSPENDTYPE_FIFO);
	if ( VK_OK != nVKRet )
	{
		PrintError("%s(%d) Error> CLS_Monitor msgQ creation() failed. err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
		nVKRet = ERR_FAIL;
		goto exit;;
	}	/* Semaphore for shared data between main_task and timer_task */

	/* Create Download Task */
	if (pHTuner->ulClsMonitorTid != 0x0)
	{
		PrintError("%s(%d) Error> Task was not destroyed.\n");
	}

	nVKRet = VK_TASK_Create ( P_IPTUNER_CLS_MonitorTask
			, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
			, UNDERFLOW_MONITOR_STACK_SIZE
			, "DecoderBuffer_Monitor"
			, (void *)pHTuner
			, &pHTuner->ulClsMonitorTid
			, FALSE);
	if ( VK_OK != nVKRet )
	{
		PrintError("%s(%d) Error> CLS_Monitor creation() failed. err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		nVKRet = ERR_FAIL;
		pHTuner->ulClsMonitorTid = 0x0;
		goto exit;
	}

	/** Start Monitoring Task */
	nVKRet = VK_TASK_Start(pHTuner->ulClsMonitorTid);
	if ((VK_OK != nVKRet) || (pHTuner->ulClsMonitorTid == 0x0))
	{
		PrintError("%s(%d) Error> CLS_Monitor start() failed. err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet, pHTuner->ulClsMonitorTid);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	nVKRet = ERR_OK;

exit:
	if (nVKRet != ERR_OK)
	{
		if (pHTuner)
		{
			if (pHTuner->ulClsMonitorQid)
			{
				(void) VK_MSG_Destroy (pHTuner->ulClsMonitorQid);
				pHTuner->ulClsMonitorQid = 0;
			}
			if (pHTuner->ulClsMonitorTid)
			{
				(void)VK_TASK_Destroy(pHTuner->ulClsMonitorTid);
				pHTuner->ulClsMonitorTid = 0;
			}
		}
	}

	PrintExit;
	return nVKRet;
}

void DI_IPTUNER_CLS_MonitorStop(DI_IPTUNER_t *pHTuner)
{
	DI_IPTUNER_Msg_t stMsg;
	//HINT32 nVKRet = ERR_OK;

	PrintEnter;

	/* check Tuner Handle */
	if ( NULL == pHTuner )
	{
		PrintError ("%s(%d) Error> Tuner Handle is NULL !\n",__FUNCTION__,__LINE__);
		goto exit;
	}
	PrintData ("%s(%d) TunerHandle=[%p] URL=[%s]\n",__FUNCTION__,__LINE__, pHTuner, pHTuner->pUrl);

	if ((pHTuner->eClsMonitorStatus == DI_IPTUNER_CLS_MONITOR_STARTED) && (pHTuner->ulClsMonitorQid != 0x0))
	{
		(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));
		stMsg.unMsg = BUFFER_MON_MSG_STOP;
		if (VK_MSG_Send(pHTuner->ulClsMonitorQid, (HUINT32 *)&stMsg, sizeof(DI_IPTUNER_Msg_t)) != VK_OK)
		{
			PrintError("%s(%d) Error> VK_MSG_Send failed! CLS_Monitor terminated\n",__FUNCTION__,__LINE__);
			goto exit;
		}

		while(pHTuner->eClsMonitorStatus != DI_IPTUNER_CLS_MONITOR_STOPPED)
		{
			VK_TASK_Sleep(100);
		}

		VK_TASK_Destroy(pHTuner->ulClsMonitorTid);
		pHTuner->ulClsMonitorTid = 0x0;

		(void) VK_MSG_Destroy(pHTuner->ulClsMonitorQid);
		pHTuner->ulClsMonitorQid = 0x0;

		if (pHTuner->tLiveBufferStateCallback != NULL) pHTuner->tLiveBufferStateCallback = NULL;
	}
	else
	{
		PrintError("%s(%d) Error> CLS_Monitor is not started\n", __FUNCTION__, __LINE__);
	}

exit:
	PrintExit;
	return;
}

