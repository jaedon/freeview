/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_underflow.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	Tae-hoon Oh $
 * Current Author:	$Author: thoh@humaxdigital.com $
 * Date:			$Date: Fri March 29 15:12:48 KST 2013 $
 * File Description:	Check underflow	state
 * Module: 			    IPTUNER Underflow Routine
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
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "bstd.h"
#include "vfio.h"

#include "vkernel.h"
#include "taskdef.h"

#include "di_media.h"

#include "util_url.h"
#include "di_iptuner.h"
#include "di_iptuner_dnp.h"
#include "di_iptuner_stream.h"
#include "di_iptuner_http.h"
#include "di_iptuner_http_chnk.h"
#include "di_iptuner_mms.h"
#include "di_iptuner_underflow.h"
#ifdef USE_IPTUNER_SSTR
#include "di_iptuner_sstr.h" /* for SSTR */
#endif
#include "di_iptuner_hls.h" /* for HLS */

/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */

//#define DI_IPTUNER_UNDERFLOW_DEBUG
//#define DI_IPTUNER_UNDERFLOW_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG

#ifdef DI_IPTUNER_UNDERFLOW_DEBUG
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

#define UNUSED_PARAM(x)	 ((void)x)


#define MAX_IP_SIZE			(128)	/** bytes */
#define MAX_PORT_SIZE		(16)	/** bytes */
#define MAX_FILE_NAME_SIZE	(IPTUNER_CONST_URL_LENGTH)	/** bytes */
#define UNDERFLOW_ADJUST_FACTOR		4
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef enum {
	DI_IPTUNER_UnderflowMonitorStatus_Stop = 0,
	DI_IPTUNER_UnderflowMonitorStatus_Start,
	DI_IPTUNER_UnderflowMonitorStatus_Sleep,
	DI_IPTUNER_UnderflowMonitorStatus_MAX
} DI_IPTUNER_UnderflowMonitorStatus_e;

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
static HULONG s_ulUnderflowHlsMonitorMsgQId;
static HULONG s_ulUnderflowHlsMonitorTaskId;
static DI_IPTUNER_UNDERFLOW_MEDIAINFO_t s_tUnderflowHlsMediaInfo;
static fn_readystate_callback s_pUnderflowHlsReadyStateCallbackFunc = NULL;
static HUINT32 g_unHlsReadyStateCBDeviceId;
/*----------------------------------------------------------------------------------------------------------------*/
static DI_IPTUNER_UnderflowMonitorStatus_e s_eUnderflowHlsMonitorStatus = DI_IPTUNER_UnderflowMonitorStatus_Stop;
static DI_IPTUNER_UNDERFLOW_READYSTATE_e s_eUnderflowHlsReadyState;
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
static void IPTUNER_UnderflowHlsMonitorTask(void *pParam);
static DI_ERR_CODE P_IPTUNER_SendHlsReadyStateEvent(void);
static DI_ERR_CODE P_IPTUNER_CheckHlsBufferingTimeOver(HBOOL *pbCheckBufferingTimeOver, HULONG *pulStartBufferingClock, HBOOL *pbIsContinue);
static DI_ERR_CODE P_IPTUNER_CheckUnderflowHls(void);

/* enough threshold 보다 많으면 resume 한다. */
static DI_ERR_CODE P_IPTUNER_CheckUnderflowHls(void)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HUINT32 unVideoBufferDepth = 0, unAudioBufferDepth = 0, unUnderflowThreshold = 0, unEnoughThreshold = 0;
	HBOOL bIsUnderflow = FALSE;
	HUINT64 ullBufferedData = 0;
	HINT64 llCurrentReadPosition = 0;
	HUINT32	uPumpBufferDepth = 0;
	HUINT32 unMaxBufferSize = 0;
	HUINT32	ulPlayTime = 0;
	HBOOL isVideoPaused = FALSE, isAudioPaused = FALSE;

	PrintEnter;

	nRet = DI_MEDIA_GetVideoBufferDepth(&unVideoBufferDepth);
	if(nRet != DI_ERR_OK)
	{
		PrintError("@@@@@ [%s:%d:%d] Getting VideoBufferDepth is Error\n", __func__, __LINE__, nRet);
		goto FUNCTION_EXIT;
	}

	nRet = DI_MEDIA_GetAudioBufferDepth(&unAudioBufferDepth);
	if(nRet != DI_ERR_OK)
	{
		PrintError("@@@@@ [%s:%d:%d] Getting unAudioBufferDepth is Error\n", __func__, __LINE__, nRet);
		goto FUNCTION_EXIT;
	}

 	nRet = DI_MEDIA_GetPumpBufferDepth(&uPumpBufferDepth);
	if(nRet != DI_ERR_OK)
	{
		PrintError("@@@@@ [%s:%d:%d] Getting uPumpBufferDepth is Error\n", __func__, __LINE__, nRet);
		goto FUNCTION_EXIT;
	}

	nRet = DI_MEDIA_IsVideoDecoderPaused(&isVideoPaused);
	if (nRet != DI_ERR_OK)
	{
		PrintError("@@@@@ [%s:%d:%d] Getting Video Pause status is Error but continue\n", __func__, __LINE__, nRet);
		goto FUNCTION_EXIT;
	}

	nRet = DI_MEDIA_IsAudioDecoderPaused(&isAudioPaused);
	if (nRet != DI_ERR_OK)
	{
		PrintError("@@@@@ [%s:%d:%d] Getting Audio Pause status is Error but continue\n", __func__, __LINE__, nRet);
		goto FUNCTION_EXIT;
	}

	ullBufferedData = unVideoBufferDepth + unAudioBufferDepth;
	//1 HLS에서는 Max Rate아닌 Min Rate를 사용한다.
	/* Underflow 조건은 버퍼가 MaxByterate X 8한 값의 15퍼센트에 못미치는 경우이다. */
	unUnderflowThreshold = ((s_tUnderflowHlsMediaInfo.MaxBitrate >> 3) * CHECK_UNDERFLOW_SECOND_VALUE * CHECK_UNDERFLOW_PERCENT_VALUE) / 100;
	/* Resume 조건은 버퍼가 MaxByteRate X 5한 값, 즉 5초 정도 재생할 분량이 나오면 Resume한다. */
	unEnoughThreshold = (s_tUnderflowHlsMediaInfo.MaxBitrate >> 3) * CHECK_RESUME_SECOND_VALUE;

	nRet = DI_MEDIA_GetMaxBufferSize(&unMaxBufferSize);
	if(nRet != DI_ERR_OK)
	{
		PrintError("@@@@@ [%s:%d:%d] Getting unMaxBufferSize is Error but continue\n", __func__, __LINE__, nRet);
	}
	else if(((unEnoughThreshold / 100) * 80) > unMaxBufferSize)
	{
		PrintError("@@@@@ [%s:%d:%d] threshold is too high. re-calculating threshold.\n", __func__, __LINE__, nRet);
		PrintDebug("@@@@@ [%s:%d] unMaxBufferSize= %d\n", __func__, __LINE__, unMaxBufferSize);
		PrintDebug("@@@@@ [%s:%d] unUnderflowThreshold = %d\n", __func__, __LINE__, unUnderflowThreshold);
		PrintDebug("@@@@@ [%s:%d] unEnoughThreshold = %d\n\n", __func__, __LINE__, unEnoughThreshold);
		unEnoughThreshold = (unMaxBufferSize / 100) * 80;
		unUnderflowThreshold = (unMaxBufferSize / 100) * CHECK_UNDERFLOW_PERCENT_VALUE;
	}

	PrintDebug("@@@@@ [%s:%d] uPumpBufferDepth     = %d\n", __func__, __LINE__, uPumpBufferDepth);
	PrintDebug("@@@@@ [%s:%d] unVideoBufferDepth   = %d\n", __func__, __LINE__, unVideoBufferDepth);
	PrintDebug("@@@@@ [%s:%d] unAudioBufferDepth   = %d\n", __func__, __LINE__, unAudioBufferDepth);
	PrintDebug("@@@@@ [%s:%d] ullBufferedData      = %llu\n", __func__, __LINE__, ullBufferedData);
	PrintDebug("@@@@@ [%s:%d] unUnderflowThreshold = %d\n", __func__, __LINE__, unUnderflowThreshold);
	PrintDebug("@@@@@ [%s:%d] unEnoughThreshold    = %d\n", __func__, __LINE__, unEnoughThreshold);
	PrintDebug("@@@@@ [%s:%d] isVideoPaused             = %d\n", __func__, __LINE__, isVideoPaused);
	PrintDebug("@@@@@ [%s:%d] isAudioPaused             = %d\n", __func__, __LINE__, isAudioPaused);

	/*  추가로  디코더가 디코딩 중이면, enough 버퍼 상태이다.  */
	if((ullBufferedData > unEnoughThreshold) || ((isVideoPaused == FALSE || isAudioPaused == FALSE) && (ullBufferedData > (unUnderflowThreshold/UNDERFLOW_ADJUST_FACTOR))))
	{
		bIsUnderflow = FALSE;
	}
	else if(ullBufferedData < unUnderflowThreshold)
	{
		if (uPumpBufferDepth > unUnderflowThreshold)
		{
			bIsUnderflow = FALSE;
		}
		else
		{
			bIsUnderflow = TRUE;

		/* Live Stream이 아닌 경우  남은 시간이 약 1.2 초 미만이라면, */
	 	if(s_tUnderflowHlsMediaInfo.Duration != 0xFFFFFFFF) // live hls인 경우 0xFFFFFFFF로 정의 된다. @ DI_STREAM_HLS_GetMediaInfo ()
		{
			nRet = DI_MEDIA_MP_GetPosition(0, &ulPlayTime, DI_MEDIA_TIME_MSEC);
			PrintDebug("@@@@@ [%s:%d] CurPosition = %d\n", __func__, __LINE__, ulPlayTime);
			if(nRet == DI_ERR_OK)
			{
				 if((s_tUnderflowHlsMediaInfo.Duration - ulPlayTime) < 1200) /* 약 1.2초 미만.. */
				{
					PrintDebug("@@@@@ [%s:%d] Playback is almost ended\n", __func__, __LINE__);
					bIsUnderflow = FALSE;
				}
			}
	}
		}
	}
	else
	{
		if(s_eUnderflowHlsReadyState == DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING)
		{
			bIsUnderflow = TRUE;

			if (uPumpBufferDepth + ullBufferedData > unEnoughThreshold)
			{
				bIsUnderflow = FALSE;
			}
		}
	}

		PrintDebug("@@@@@ [%s:%d] bIsUnderflow 			= %d\n", __func__, __LINE__, bIsUnderflow);
		PrintDebug("@@@@@ [%s:%d] s_eUnderflowHlsReadyState 			= %d\n", __func__, __LINE__, s_eUnderflowHlsReadyState);

	if(bIsUnderflow == FALSE)	/* 현재 Underflow가 아닌 경우 */
	{
		if(s_eUnderflowHlsReadyState == DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING)
		{
			nRet = P_IPTUNER_SendHlsReadyStateEvent();
			if(nRet != DI_ERR_OK)
			{
				PrintError("@@@@@ [%s:%d:%d] Sending Event is Failed\n", __func__, __LINE__, nRet);
				goto FUNCTION_EXIT;
			}
		}
	}
	else	 /* 현재 Underflow인 경우 */
	{   /*  video 또는 audio 디코더가 pause 인 상태 추가 ... */
		if((s_eUnderflowHlsReadyState == DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED) && (isVideoPaused == TRUE || isAudioPaused == TRUE))
	{
			nRet = P_IPTUNER_SendHlsReadyStateEvent();
			if(nRet != DI_ERR_OK)
			{
				PrintError("@@@@@ [%s:%d:%d] Sending Event is Failed\n", __func__, __LINE__, nRet);
				goto FUNCTION_EXIT;
			}
		}
	}

FUNCTION_EXIT:
	PrintExit;
	return nRet;
}
/* bueffering timeout (대략 10초)가 나면 다시 resume 한다. */
static DI_ERR_CODE P_IPTUNER_CheckHlsBufferingTimeOver(HBOOL *pbCheckBufferingTimeOver, HULONG *pulStartBufferingClock, HBOOL *pbIsContinue)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HULONG ulEndBufferingClock = 0, ulSubtractBufferingClock = 0;
	HUINT64 ullBufferedData = 0;
	HUINT32 unVideoBufferDepth = 0, unAudioBufferDepth = 0, unUnderflowThreshold = 0;
	HBOOL isVideoPaused = FALSE, isAudioPaused = FALSE;

	PrintEnter;

	if(pbCheckBufferingTimeOver == NULL)
	{
		PrintError("@@@@@ [%s:%d] pbCheckBufferingTimeOver is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto FUNCTION_EXIT;
	}

	if(pulStartBufferingClock == NULL)
	{
		PrintError("@@@@@ [%s:%d] pulStartBufferingClock is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto FUNCTION_EXIT;
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

		if(s_eUnderflowHlsReadyState == DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING)
		{
			if(ulSubtractBufferingClock >= BUFFERING_TIMEOUT)
			{
				nRet = DI_MEDIA_GetVideoBufferDepth(&unVideoBufferDepth);
				if(nRet != DI_ERR_OK)
				{
					PrintError("@@@@@ [%s:%d:%d] Getting VideoBufferDepth is Error\n", __func__, __LINE__, nRet);
					goto FUNCTION_EXIT;
				}

				nRet = DI_MEDIA_GetAudioBufferDepth(&unAudioBufferDepth);
				if(nRet != DI_ERR_OK)
				{
					PrintError("@@@@@ [%s:%d:%d] Getting unAudioBufferDepth is Error\n", __func__, __LINE__, nRet);
					goto FUNCTION_EXIT;
				}

				nRet = DI_MEDIA_IsVideoDecoderPaused(&isVideoPaused);
				if (nRet != DI_ERR_OK)
				{
					PrintError("@@@@@ [%s:%d:%d] Getting Pause status is Error\n", __func__, __LINE__, nRet);
					goto FUNCTION_EXIT;
				}

				nRet = DI_MEDIA_IsAudioDecoderPaused(&isAudioPaused);
				if (nRet != DI_ERR_OK)
				{
					PrintError("@@@@@ [%s:%d:%d] Getting Pause status is Error\n", __func__, __LINE__, nRet);
					goto FUNCTION_EXIT;
				}

				ullBufferedData = unVideoBufferDepth + unAudioBufferDepth;
				unUnderflowThreshold = ((s_tUnderflowHlsMediaInfo.MaxBitrate >> 3) * CHECK_UNDERFLOW_SECOND_VALUE * CHECK_UNDERFLOW_PERCENT_VALUE) / 100;

				if ((isVideoPaused==FALSE||isAudioPaused==FALSE) && (ullBufferedData>(unUnderflowThreshold/UNDERFLOW_ADJUST_FACTOR)))
				{
					/*send state event. 만일 video or audio가 디코딩 중이고, 쌓인 버퍼가 일정 수준 이상이면, */
				}
				if(ullBufferedData < unUnderflowThreshold)
				{
					PrintDebug("@@@@@ [%s:%d] # ullBufferedData(%llu) is lower than Underflow Threshold(%d) and video decoder isn't decoding, can't resume..\n", __func__, __LINE__, ullBufferedData, unUnderflowThreshold);
					*pbIsContinue = TRUE;
					goto FUNCTION_EXIT;
				}

				/* BUFFERING_COMPLETED 으로 상태 전환 */
				nRet = P_IPTUNER_SendHlsReadyStateEvent();
				if(nRet != DI_ERR_OK)
				{
					PrintError("@@@@@ [%s:%d:%d] Sending Event is Failed\n", __func__, __LINE__, nRet);
					goto FUNCTION_EXIT;
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

FUNCTION_EXIT:
	PrintExit;
	return nRet;
}

static DI_ERR_CODE P_IPTUNER_SendHlsReadyStateEvent(void)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	PrintEnter;

	if(s_pUnderflowHlsReadyStateCallbackFunc)
	{
		switch(s_eUnderflowHlsReadyState)
		{
			case DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING:
				PrintDebug("@@@@@ [%s:%d] # PAUSE -> RESUME\n", __func__, __LINE__);
				s_pUnderflowHlsReadyStateCallbackFunc(g_unHlsReadyStateCBDeviceId, DI_MEDIA_EVENT_BUFFER_READY_STATE, DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED);
				s_eUnderflowHlsReadyState = DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED;
				break;
			case DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED:
				PrintDebug("@@@@@ [%s:%d] # RESUME -> PAUSE\n", __func__, __LINE__);
				s_pUnderflowHlsReadyStateCallbackFunc(g_unHlsReadyStateCBDeviceId, DI_MEDIA_EVENT_BUFFER_READY_STATE, DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING);
				s_eUnderflowHlsReadyState = DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING;
				break;
			default:
				break;
		}
	}
	else
	{
		PrintError("@@@@@ [%s:%d] s_pUnderflowHlsReadyStateCallbackFunc is not set\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	PrintExit;
	return nRet;
}

static void IPTUNER_UnderflowHlsMonitorTask(void *pParam)
{
	DI_IPTUNER_Msg_t stMsg;
	HINT32 nVKRet = ERR_FAIL;
	//HUINT32 unUnderflowThreshold = 0;
	DI_ERR_CODE nRet = DI_ERR_OK;
	HBOOL bCheckBufferingTimeOver = FALSE, bIsContinue = FALSE;
	HULONG ulStartBufferingClock = 0;

	s_eUnderflowHlsReadyState = DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING;

	PrintEnter;

	UNUSED_PARAM(pParam);

	//unUnderflowThreshold = ((s_tUnderflowMediaInfo.MaxBitrate >> 3) * CHECK_UNDERFLOW_SECOND_VALUE * CHECK_UNDERFLOW_PERCENT_VALUE) / 100;

	(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));

	s_eUnderflowHlsMonitorStatus = DI_IPTUNER_UnderflowMonitorStatus_Sleep;

	while (s_eUnderflowHlsMonitorStatus != DI_IPTUNER_UnderflowMonitorStatus_Stop)
	{
		nVKRet = VK_MSG_ReceiveTimeout (s_ulUnderflowHlsMonitorMsgQId, &stMsg, sizeof(DI_IPTUNER_Msg_t), 1000);
		if(nVKRet != VK_OK)
		{
			if(s_eUnderflowHlsMonitorStatus == DI_IPTUNER_UnderflowMonitorStatus_Sleep)
			{
				VK_TASK_Sleep(1);
			}
			else if(nVKRet == VK_TIMEOUT)
			{
#if 0 /*  BBC CATAL 테스트 에서 HLS는 trick 모드 지원 하지 않는다. 그래서 아래 코드 적용할 필요 없음. */
				if(s_tUnderflowHlsMediaInfo.TunerIndex == DI_IPTUNER_DNP_HTTP)
				{
					if(g_bPrdnForwardPlay == FALSE)
					{
						PrintDebug("@@@@@ [%s:%d] g_bPrdnForwardPlay is FALSE, do not operate underflow routine.\n", __func__, __LINE__);
						continue;
					}
				}
#endif

#if 0
				/* Download Speed, Bitrate Compare */
				if(s_tUnderflowMediaInfo.TunerIndex == DI_IPTUNER_DNP_HTTP)
				{
					PrintDebug("@@@@@ [%s:%d] Before P_IPTUNER_CompareDownloadSpeedWithBitrate()\n", __func__, __LINE__);
					nRet = P_IPTUNER_CompareDownloadSpeedWithBitrate(&eUnderflowState, &bIsContinue);
					if(nRet != DI_ERR_OK)
					{
						PrintError("@@@@@ [%s:%d:%d] Comparing Download speed with bitrate is failed\n", __func__, __LINE__, nRet);
						break;
					}
					if(bIsContinue)
					{
						bIsContinue = FALSE;
						continue;
					}
				}
#endif
				/* Check Buffer Time-over */
				nRet = P_IPTUNER_CheckHlsBufferingTimeOver(&bCheckBufferingTimeOver, &ulStartBufferingClock, &bIsContinue);
				if(nRet != DI_ERR_OK)
				{
					PrintError("@@@@@ [%s:%d:%d] Checking BufferingTimeOver is failed\n", __func__, __LINE__, nRet);
					break;
				}
				if(bIsContinue)
				{
					bIsContinue = FALSE;
					continue;
				}

				/* Check Underflow */
				nRet = P_IPTUNER_CheckUnderflowHls();
				if(nRet != DI_ERR_OK)
				{
					PrintError("@@@@@ [%s:%d:%d] Checking Underflow is failed\n", __func__, __LINE__, nRet);
					break;
				}
			}
			continue;
		}

		switch(stMsg.unMsg)
		{
			case UNDERFLOW_MONITOR_MSG_STOP_TASK:
				PrintDebug ("%s(%d) : UNDERFLOW_MONITOR_MSG_STOP_TASK Received. \n",__FUNCTION__,__LINE__);
				goto UNDERFLOW_MONITOR_EXIT;
				break;

			case UNDERFLOW_MONITOR_MSG_SLEEP_TASK:
				PrintDebug ("%s(%d) : UNDERFLOW_MONITOR_MSG_SLEEP_TASK Received. \n",__FUNCTION__,__LINE__);
				s_eUnderflowHlsMonitorStatus = DI_IPTUNER_UnderflowMonitorStatus_Sleep;
				break;

			case UNDERFLOW_MONITOR_MSG_AWAKE_TASK:
				PrintDebug ("%s(%d) : UNDERFLOW_MONITOR_MSG_AWAKE_TASK Received. \n",__FUNCTION__,__LINE__);
				s_eUnderflowHlsMonitorStatus = DI_IPTUNER_UnderflowMonitorStatus_Start;
				s_eUnderflowHlsReadyState = DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING;
				break;

			default:
				PrintError("@@@@@ [%s:%d] Unkonw UF Message..!!\n", __func__, __LINE__);
				break;
		}
	}

UNDERFLOW_MONITOR_EXIT:
	s_eUnderflowHlsMonitorStatus = DI_IPTUNER_UnderflowMonitorStatus_Stop;

	PrintExit;

	return;
}

HULONG DI_IPTUNER_UnderflowHlsGetTaskId(void)
{
	return (HULONG)s_ulUnderflowHlsMonitorTaskId;
}

HINT32 DI_IPTUNER_UnderflowHlsGetReadyState(void)
{
	return (HINT32)s_eUnderflowHlsReadyState;
}

void DI_IPTUNER_UnderflowHlsSetReadyState(HINT32 eUnderflowState)
{
	s_eUnderflowHlsReadyState = (DI_IPTUNER_UNDERFLOW_READYSTATE_e)eUnderflowState;

	/* Ready State가 BUFFERING COMPLETED인 경우, 해당 이벤트를 APP에 올려준다. */
	if(s_eUnderflowHlsReadyState == DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED
		&& s_pUnderflowHlsReadyStateCallbackFunc != NULL)
	{
		s_pUnderflowHlsReadyStateCallbackFunc(g_unHlsReadyStateCBDeviceId, DI_MEDIA_EVENT_BUFFER_READY_STATE, DI_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED);
	}

	return;
}

DI_ERR_CODE DI_IPTUNER_SetUnderflowHlsReadyStateCallback(HUINT32 unDeviceId, fn_readystate_callback pFn)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	if(pFn == NULL)
	{
		PrintError("@@@@@ [%s:%d] pFn is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto FUNCTION_EXIT;
	}
	s_pUnderflowHlsReadyStateCallbackFunc = (fn_readystate_callback)pFn;
	g_unHlsReadyStateCBDeviceId = unDeviceId;

FUNCTION_EXIT:
	return nRet;
}

HINT32 DI_IPTUNER_UnderflowHlsMonitorInitThread(DI_IPTUNER_Index_e nTunerIndex, DI_MEDIA_INFO_t *pstMediaInfo, HINT64 llFileLength)
{
	DI_IPTUNER_Msg_t stMsg;
	HINT32 nVKRet = ERR_OK;

	PrintEnter;

	if(pstMediaInfo->max_bitrate == 0)
	{
		PrintError ("%s (%d) Error> Invaild Media Bitrate!!\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	if(pstMediaInfo->duration <= 0)
	{
		PrintError("@@@@@ [%s:%d] duration <= 0\n", __func__, __LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	s_tUnderflowHlsMediaInfo.FileLength = 0; /* unused. it' HLS. */
	s_tUnderflowHlsMediaInfo.MaxBitrate = pstMediaInfo->min_bitrate;
	s_tUnderflowHlsMediaInfo.Duration = pstMediaInfo->duration;
	s_tUnderflowHlsMediaInfo.TunerIndex = nTunerIndex;

	if(s_eUnderflowHlsMonitorStatus != DI_IPTUNER_UnderflowMonitorStatus_Stop)
	{
		PrintError  ("%s (%d) Error>  Already initialized !\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_OK;
		goto exit;
	}

	/* create & init message que */
	if (s_ulUnderflowHlsMonitorMsgQId != 0)
	{
		PrintError("%s (%d) Error Msg was not destroyed.\n");
	}

	nVKRet = VK_MSG_Create(TUNER_UNDERFLOW_MSG_QUEUE_SIZE, sizeof(stMsg), "UNDERFLOW_MONITOR_HLS_MSGQ", &s_ulUnderflowHlsMonitorMsgQId, VK_SUSPENDTYPE_FIFO);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> Underflow HLS Monitoring Task msgQ creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
		nVKRet = ERR_FAIL;
		goto exit;
	}	/* Semaphore for shared data between main_task and timer_task */

	/* Create Download Task */
	if (s_ulUnderflowHlsMonitorTaskId != 0)
	{
		PrintError("%s (%d) Error Task was not destroyed.\n");
	}

	nVKRet = VK_TASK_Create ( IPTUNER_UnderflowHlsMonitorTask
			, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
			, UNDERFLOW_MONITOR_STACK_SIZE
			, "DecoderBuffer_HlsMonitor"
			, (void *)NULL
			, &s_ulUnderflowHlsMonitorTaskId
			, FALSE);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> DecoderBuffer Monitoring task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		nVKRet = ERR_FAIL;
		s_ulUnderflowHlsMonitorTaskId = 0;
		goto exit;
	}

	/** Start Monitoring Task */
	nVKRet = VK_TASK_Start(s_ulUnderflowHlsMonitorTaskId);
	if ( VK_OK != nVKRet )
	{
		PrintError ("%s (%d) Error> DecoderBuffer Monitoring task start fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	nVKRet = ERR_OK;

exit:
	if (nVKRet != ERR_OK)
	{
		if (s_ulUnderflowHlsMonitorMsgQId)
		{
			(void) VK_MSG_Destroy (s_ulUnderflowHlsMonitorMsgQId);
			s_ulUnderflowHlsMonitorMsgQId = 0;
		}
		if (s_ulUnderflowHlsMonitorTaskId)
		{
			(void)VK_TASK_Destroy(s_ulUnderflowHlsMonitorTaskId);
			s_ulUnderflowHlsMonitorTaskId = 0;
		}

		VK_memset(&s_tUnderflowHlsMediaInfo, 0x00, sizeof(DI_IPTUNER_UNDERFLOW_MEDIAINFO_t));
	}

	PrintExit;
	HAPPY(llFileLength);
	return nVKRet;
}

void DI_IPTUNER_UnderflowHlsMonitorTerminateThread(void)
{
	DI_IPTUNER_Msg_t stMsg;

	PrintEnter;

	if(s_eUnderflowHlsMonitorStatus == DI_IPTUNER_UnderflowMonitorStatus_Start)
	{
		(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));
		stMsg.unMsg = UNDERFLOW_MONITOR_MSG_STOP_TASK;
		if(VK_MSG_Send(s_ulUnderflowHlsMonitorMsgQId, (HUINT32 *)&stMsg, sizeof(DI_IPTUNER_Msg_t)) != VK_OK)
		{
			PrintError ("%s(%d) VK_MSG_Send ERROR!! Terminating Decoder Buffer Hls Monitoring Thread...\n",__FUNCTION__,__LINE__);
			goto exit;
		}

        while(s_eUnderflowHlsMonitorStatus != DI_IPTUNER_UnderflowMonitorStatus_Stop)
        {
            VK_TASK_Sleep(100);
        }

		VK_TASK_Destroy(s_ulUnderflowHlsMonitorTaskId);
		s_ulUnderflowHlsMonitorTaskId = 0;

		(void) VK_MSG_Destroy(s_ulUnderflowHlsMonitorMsgQId);
		s_ulUnderflowHlsMonitorMsgQId = 0;

		VK_memset(&s_tUnderflowHlsMediaInfo, 0x00, sizeof(DI_IPTUNER_UNDERFLOW_MEDIAINFO_t));
		s_pUnderflowHlsReadyStateCallbackFunc = NULL;
	}
	else
	{
		PrintError ("%s(%d) DI_IPTUNER_UnderflowMonitor is not started\n", __FUNCTION__, __LINE__);
	}

exit:
	PrintExit;

	return;
}

void 		DI_IPTUNER_UnderflowHlsMonitorSleepThread(void)
{
	DI_IPTUNER_Msg_t stMsg;

	PrintEnter;

	if((s_ulUnderflowHlsMonitorTaskId != 0) && (s_eUnderflowHlsMonitorStatus == DI_IPTUNER_UnderflowMonitorStatus_Start))
	{
		(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));
		stMsg.unMsg = UNDERFLOW_MONITOR_MSG_SLEEP_TASK;
		if(VK_MSG_Send(s_ulUnderflowHlsMonitorMsgQId, (HUINT32 *)&stMsg, sizeof(DI_IPTUNER_Msg_t)) != VK_OK)
		{
			PrintError("%s(%d) VK_MSG_Send ERROR!! Sleeping Buffer HLS Monitoring Thread...\n",__FUNCTION__,__LINE__);
		}
	}
	else
	{
		PrintError("%s(%d) DI_IPTUNER_UnderflowHlsMonitor is not started\n", __FUNCTION__, __LINE__);
	}

	PrintExit;
}

void 		DI_IPTUNER_UnderflowHlsMonitorAwakeThread(void)
{
	DI_IPTUNER_Msg_t stMsg;

	PrintEnter;

	if((s_ulUnderflowHlsMonitorTaskId != 0) && (s_eUnderflowHlsMonitorStatus == DI_IPTUNER_UnderflowMonitorStatus_Sleep))
	{
		(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));
		stMsg.unMsg = UNDERFLOW_MONITOR_MSG_AWAKE_TASK;
		if(VK_MSG_Send(s_ulUnderflowHlsMonitorMsgQId, (HUINT32 *)&stMsg, sizeof(DI_IPTUNER_Msg_t)) != VK_OK)
		{
			PrintError("%s(%d) VK_MSG_Send ERROR!! Awake Buffer Monitoring Thread...\n",__FUNCTION__,__LINE__);
		}
	}
	else
	{
		PrintError("%s(%d) DI_IPTUNER_UnderflowHlsMonitor is not sleep..\n", __FUNCTION__, __LINE__);
	}

	PrintExit;
}


