/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_mheg_ics_iptuner_underflow.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	JGKim(Media Client) $
 * Current Author:	kimjg@humaxdigital.com $
 * Date:			February 2014
 * File Description:	Check underflow	state
 * Module: 			    PAL MEDIA MHEG IPTUNER Underflow Routine
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <inttypes.h>	/** for 64bit MACRO ex>PRId64 */


#include "bstd.h"
#include "taskdef.h"

/*
#include "di_iptuner_dnp.h"
#include "di_iptuner_stream.h"
#include "di_iptuner_http.h"
#include "di_iptuner_http_chnk.h"
#include "di_iptuner_mms.h"
*/

#include "di_mheg_ics.h"
#include "di_mheg_ics_iptuner.h"
#include "di_mheg_ics_iptuner_underflow.h"

#include "util_url.h"
#include "di_uart.h"


/* End Including Headers */

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */


//#define DI_MHEG_ICS_DEBUG

#ifdef DI_MHEG_ICS_DEBUG
#define MhegIcsData		DI_UART_Print
#define MhegIcsDebug		DI_UART_Print
#define MhegIcsError		DI_UART_Print
#else
#define MhegIcsData		while (0) ((int (*)(char *, ...)) 0)
#define MhegIcsDebug		while (0) ((int (*)(char *, ...)) 0)
#define MhegIcsError		DI_UART_Print
#endif

#ifdef DI_MHEG_ICS_DEBUG
#define MhegIcsFunctionEnter(a)		DI_UART_Print("(+)%s[%d]\n", __FUNCTION__, a)
#define MhegIcsFunctionExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define MhegIcsFunctionEnter(a)
#define MhegIcsFunctionExit
#endif


#define UNUSED_PARAM(x)	 ((void)x)


#define MAX_IP_SIZE			(128)	/** bytes */
#define MAX_PORT_SIZE		(16)	/** bytes */
#define MAX_FILE_NAME_SIZE	(2048)	/** bytes */
/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef enum {
	MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Stop = 0,
	MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Start,
	MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Sleep,
	MHEG_ICS_IPTUNER_UnderflowMonitorStatus_MAX
} MHEG_ICS_IPTUNER_UnderflowMonitorStatus_e;

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

static HULONG s_ulMhegIcsUFMonitorMsgQId[MAX_MHEG_ICS_IPTUNER_NUM];
static HULONG s_ulMhegIcsUFMonitorTaskId[MAX_MHEG_ICS_IPTUNER_NUM];
static MHEG_ICS_IPTUNER_UNDERFLOW_MEDIAINFO_t s_tMhegIcsUFMediaInfo[MAX_MHEG_ICS_IPTUNER_NUM];
static fn_readystate_callback s_pMhegIcsUFReadyStateCallbackFunc[MAX_MHEG_ICS_IPTUNER_NUM] = {NULL, };

static MHEG_ICS_IPTUNER_UnderflowMonitorStatus_e s_eMhegIcsUFMonitorStatus[MAX_MHEG_ICS_IPTUNER_NUM] = {MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Stop, };
static MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_e s_eMhegIcsUFReadyState[MAX_MHEG_ICS_IPTUNER_NUM];
/* End static variable */


/*******************************************************************/
/************************ static funtions **************************/
/*******************************************************************/
static void local_mheg_ics_iptuner_UnderflowMonitorTask(void *pParam);
#if 0
static DI_ERR_CODE local_mheg_ics_iptuner_CompareDownloadSpeedWithBitrate(MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_e *peUnderflowState, HBOOL *pbIsContinue);
#endif
static DI_ERR_CODE local_mheg_ics_iptuner_SendReadyStateEvent(HUINT32 ulHandle);
static DI_ERR_CODE local_mheg_ics_iptuner_CheckBufferingTimeOver(HUINT32 ulHandle, HBOOL *pbCheckBufferingTimeOver, HULONG *pulStartBufferingClock, HBOOL *pbIsContinue);
static DI_ERR_CODE local_mheg_ics_iptuner_CheckUnderflow(HUINT32 ulHandle);

static DI_ERR_CODE local_mheg_ics_iptuner_CheckUnderflow(HUINT32 ulHandle)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HUINT32 unVideoBufferDepth = 0, unAudioBufferDepth = 0, unUnderflowThreshold = 0, unEnoughThreshold = 0;
	HBOOL bIsUnderflow = FALSE;
	HUINT64 ullBufferedData = 0;
	HUINT64 ullCurrentReadPosition = 0;
	HUINT32	uPumpBufferDepth = 0;

	MhegIcsFunctionEnter(ulHandle);

	//nRet = DI_MHEG_MEDIA_GetVideoBufferDepth(ulHandle, &unVideoBufferDepth);
	nRet = DI_MEDIA_GetVideoBufferDepth(&unVideoBufferDepth);
	if(nRet != DI_ERR_OK)
	{
		MhegIcsError("@@@@@ [%s:%d:%d] Getting VideoBufferDepth is Error\n", __func__, __LINE__, nRet);
		goto FUNCTION_EXIT;
	}

	//nRet = DI_MHEG_MEDIA_GetAudioBufferDepth(ulHandle, &unAudioBufferDepth);
	nRet = DI_MEDIA_GetAudioBufferDepth(&unAudioBufferDepth);
	if(nRet != DI_ERR_OK)
	{
		MhegIcsError("@@@@@ [%s:%d:%d] Getting unAudioBufferDepth is Error\n", __func__, __LINE__, nRet);
		goto FUNCTION_EXIT;
	}

 	//nRet = DI_MHEG_MEDIA_GetPumpBufferDepth(ulHandle, &uPumpBufferDepth);
 	nRet = DI_MEDIA_GetPumpBufferDepth(&uPumpBufferDepth);
	if(nRet != DI_ERR_OK)
	{
		MhegIcsError("@@@@@ [%s:%d:%d] Getting uPumpBufferDepth is Error\n", __func__, __LINE__, nRet);
		goto FUNCTION_EXIT;
	}

	ullBufferedData = unVideoBufferDepth + unAudioBufferDepth;
	/* Underflow 조건은 버퍼가 MaxByterate X 8한 값의 15퍼센트에 못미치는 경우이다. */
	unUnderflowThreshold = ((s_tMhegIcsUFMediaInfo[ulHandle].MaxBitrate >> 3) * MHEG_ICS_CHECK_UNDERFLOW_SECOND_VALUE * MHEG_ICS_CHECK_UNDERFLOW_PERCENT_VALUE) / 100;
	/* Resume 조건은 버퍼가 MaxByteRate X 3한 값, 즉 3초 정도 재생할 분량이 나오면 Resume한다. */
	unEnoughThreshold = (s_tMhegIcsUFMediaInfo[ulHandle].MaxBitrate >> 3) * MHEG_ICS_CHECK_RESUME_SECOND_VALUE;

	MhegIcsDebug("@@@@@ [%s:%d] Media[%d]\n", __func__, __LINE__, ulHandle);
	MhegIcsDebug("@@@@@ \t uPumpBufferDepth     = %d\n", uPumpBufferDepth);
	MhegIcsDebug("@@@@@ \t unVideoBufferDepth   = %d\n", unVideoBufferDepth);
	MhegIcsDebug("@@@@@ \t unAudioBufferDepth   = %d\n", unAudioBufferDepth);
	MhegIcsDebug("@@@@@ \t ullBufferedData      = %llu\n", ullBufferedData);
	MhegIcsDebug("@@@@@ \t unUnderflowThreshold = %d\n", unUnderflowThreshold);
	MhegIcsDebug("@@@@@ \t unEnoughThreshold    = %d\n", unEnoughThreshold);

	if(ullBufferedData > unEnoughThreshold)
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

			ullCurrentReadPosition = (HUINT64)DI_MHEG_ICS_IPTUNER_DnpPrdn_GetCurrentDataReadOffset(ulHandle);			

			MhegIcsDebug("\n %s(%d) @@@@ ullCurrentReadPosition(%"PRId64")\n", __FUNCTION__, __LINE__, ullCurrentReadPosition);

			if(s_tMhegIcsUFMediaInfo[ulHandle].FileLength - ullCurrentReadPosition < unUnderflowThreshold)
			{
				MhegIcsDebug("@@@@@ [%s:%d] Remain data is lower than Underflow Threshold\n", __func__, __LINE__);
				bIsUnderflow = FALSE;
			}
		}
	}
	else
	{
		if(s_eMhegIcsUFReadyState[ulHandle] == MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING)
		{
			bIsUnderflow = TRUE;
			
			ullCurrentReadPosition = (HINT64)DI_MHEG_ICS_IPTUNER_DnpPrdn_GetCurrentDataReadOffset(ulHandle);
			
			if(s_tMhegIcsUFMediaInfo[ulHandle].FileLength - (HINT64)ullCurrentReadPosition <= (HINT64)ullBufferedData)
			{
				bIsUnderflow = FALSE;
			}

			if (uPumpBufferDepth + ullBufferedData > unEnoughThreshold)
			{
				bIsUnderflow = FALSE;
			}
		}
	}

	if(DI_MHEG_ICS_IPTUNER_DnpPrdn_IsReadCompleted(ulHandle) == TRUE)
	{
		MhegIcsDebug("\n %s(%d) @@@@ ReadCompleted.. => No underflow..!!\n", __FUNCTION__, __LINE__);
		bIsUnderflow = FALSE;
	}

	MhegIcsDebug("@@@@@ [%s:%d] Media[%d], bIsUnderflow(%d), ReadyState(%d)\n", __func__, __LINE__, ulHandle, bIsUnderflow, s_eMhegIcsUFReadyState[ulHandle]);
	if(bIsUnderflow == FALSE)	/* 현재 Underflow가 아닌 경우 */
	{
		if(s_eMhegIcsUFReadyState[ulHandle] == MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING)
		{
			nRet = local_mheg_ics_iptuner_SendReadyStateEvent(ulHandle);
			if(nRet != DI_ERR_OK)
			{
				MhegIcsError("@@@@@ [%s:%d:%d] Sending Event is Failed\n", __func__, __LINE__, nRet);
				goto FUNCTION_EXIT;
			}
		}
	}
	else	 /* 현재 Underflow인 경우 */
	{
		if(s_eMhegIcsUFReadyState[ulHandle] == MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED)
		{
			nRet = local_mheg_ics_iptuner_SendReadyStateEvent(ulHandle);
			if(nRet != DI_ERR_OK)
			{
				MhegIcsError("@@@@@ [%s:%d:%d] Sending Event is Failed\n", __func__, __LINE__, nRet);
				goto FUNCTION_EXIT;
			}
		}
	}

FUNCTION_EXIT:
	MhegIcsFunctionExit;
	return nRet;
}

static DI_ERR_CODE local_mheg_ics_iptuner_CheckBufferingTimeOver(HUINT32 ulHandle, HBOOL *pbCheckBufferingTimeOver, HULONG *pulStartBufferingClock, HBOOL *pbIsContinue)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HULONG ulEndBufferingClock = 0, ulSubtractBufferingClock = 0;
	HUINT64 ullBufferedData = 0;
	HUINT32 unVideoBufferDepth = 0, unAudioBufferDepth = 0, unUnderflowThreshold = 0;

	MhegIcsFunctionEnter(ulHandle);

	if(pbCheckBufferingTimeOver == NULL)
	{
		MhegIcsError("@@@@@ [%s:%d] pbCheckBufferingTimeOver is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto FUNCTION_EXIT;
	}

	if(pulStartBufferingClock == NULL)
	{
		MhegIcsError("@@@@@ [%s:%d] pulStartBufferingClock is NULL\n", __func__, __LINE__);
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

		if(s_eMhegIcsUFReadyState[ulHandle] == MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING)
		{
			if(ulSubtractBufferingClock >= MHEG_ICS_BUFFERING_TIMEOUT)
			{							
				//nRet = DI_MHEG_MEDIA_GetVideoBufferDepth(ulHandle, &unVideoBufferDepth);
				nRet = DI_MEDIA_GetVideoBufferDepth(&unVideoBufferDepth);
				if(nRet != DI_ERR_OK)
				{
					MhegIcsError("@@@@@ [%s:%d:%d] Getting VideoBufferDepth is Error\n", __func__, __LINE__, nRet);
					goto FUNCTION_EXIT;
				}

				//nRet = DI_MHEG_MEDIA_GetAudioBufferDepth(ulHandle, &unAudioBufferDepth);
				nRet = DI_MEDIA_GetAudioBufferDepth(&unAudioBufferDepth);
				if(nRet != DI_ERR_OK)
				{
					MhegIcsError("@@@@@ [%s:%d:%d] Getting unAudioBufferDepth is Error\n", __func__, __LINE__, nRet);
					goto FUNCTION_EXIT;
				}

				if(DI_MHEG_ICS_IPTUNER_DnpPrdn_IsReadCompleted(ulHandle) == TRUE)
				{
					pbCheckBufferingTimeOver = FALSE;
					goto FUNCTION_EXIT;
				}

				ullBufferedData = unVideoBufferDepth + unAudioBufferDepth;
				unUnderflowThreshold = ((s_tMhegIcsUFMediaInfo[ulHandle].MaxBitrate >> 3) * MHEG_ICS_CHECK_UNDERFLOW_SECOND_VALUE * MHEG_ICS_CHECK_UNDERFLOW_PERCENT_VALUE) / 100;
				//unUnderflowThreshold = ((1200000 >> 3) * MHEG_CHECK_UNDERFLOW_SECOND_VALUE * MHEG_ICS_CHECK_UNDERFLOW_PERCENT_VALUE) / 100;
				if(ullBufferedData < unUnderflowThreshold)
				{
					MhegIcsDebug("@@@@@ [%s:%d] [Media:%d]# ullBufferedData(%llu) is lower than Underflow Threshold(%d), can't resume..\n", __func__, __LINE__, ulHandle, ullBufferedData, unUnderflowThreshold);
					*pbIsContinue = TRUE;
					goto FUNCTION_EXIT;
				}

				nRet = local_mheg_ics_iptuner_SendReadyStateEvent(ulHandle);
				if(nRet != DI_ERR_OK)
				{
					MhegIcsError("@@@@@ [%s:%d:%d] Sending Event is Failed\n", __func__, __LINE__, nRet);
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
	MhegIcsFunctionExit;
	return nRet;
}

static DI_ERR_CODE local_mheg_ics_iptuner_SendReadyStateEvent(HUINT32 ulHandle)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	MhegIcsFunctionEnter(ulHandle);

	if(s_pMhegIcsUFReadyStateCallbackFunc[ulHandle])
	{
		switch(s_eMhegIcsUFReadyState[ulHandle])
		{
			case MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING:
				MhegIcsDebug("@@@@@ [%s:%d] # [Media:%d]PAUSE -> RESUME\n", __func__, __LINE__, ulHandle);
				s_pMhegIcsUFReadyStateCallbackFunc[ulHandle](ulHandle, DI_MEDIA_EVENT_BUFFER_READY_STATE, MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED);
				s_eMhegIcsUFReadyState[ulHandle] = MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED;
				break;
			case MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED:
				MhegIcsDebug("@@@@@ [%s:%d] # [Media:%d]RESUME -> PAUSE\n", __func__, __LINE__, ulHandle);
				s_pMhegIcsUFReadyStateCallbackFunc[ulHandle](ulHandle, DI_MEDIA_EVENT_BUFFER_READY_STATE, MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING);
				s_eMhegIcsUFReadyState[ulHandle] = MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING;
				break;
			default:
				break;
		}
	}
	else
	{
		MhegIcsError("@@@@@ [%s:%d] [Media:%d]s_pMhegIcsUFReadyStateCallbackFunc is not set\n", __func__, __LINE__, ulHandle);
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	MhegIcsFunctionExit;
	return nRet;
}

static void local_mheg_ics_iptuner_UnderflowMonitorTask(void *pParam)
{
	DI_IPTUNER_Msg_t stMsg;
	HINT32 nVKRet = ERR_FAIL;
	//HUINT32 unUnderflowThreshold = 0;
	DI_ERR_CODE nRet = DI_ERR_OK;
	HBOOL bCheckBufferingTimeOver = FALSE, bIsContinue = FALSE;
	HULONG ulStartBufferingClock = 0;

	HUINT32		ulHandle = (HUINT32)pParam;

	s_eMhegIcsUFReadyState[ulHandle] = MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING;

	MhegIcsFunctionEnter(ulHandle);


	//unUnderflowThreshold = ((s_tMhegIcsUFMediaInfo.MaxBitrate >> 3) * CHECK_UNDERFLOW_SECOND_VALUE * CHECK_UNDERFLOW_PERCENT_VALUE) / 100;

	(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));

	s_eMhegIcsUFMonitorStatus[ulHandle] = MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Sleep;

	while (s_eMhegIcsUFMonitorStatus[ulHandle] != MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Stop)
	{
		nVKRet = VK_MSG_ReceiveTimeout (s_ulMhegIcsUFMonitorMsgQId[ulHandle], &stMsg, sizeof(DI_IPTUNER_Msg_t), 1000);
		if(nVKRet != VK_OK)
		{
			if(s_eMhegIcsUFMonitorStatus[ulHandle] == MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Sleep)
			{
				//MhegIcsDebug("@@@@@ [%s:%d] UnderflowMonitorTask[%d] is now on sleep.\n", __func__, __LINE__, ulHandle);
				VK_TASK_Sleep(200);
			}
			else if(nVKRet == VK_TIMEOUT)
			{
#ifdef ENABLE_MAPPED_DNP
				//if(s_tMhegIcsUFMediaInfo.TunerIndex == DI_IPTUNER_DNP_HTTP)
				{
					if(DI_MHEG_ICS_IPTUNER_DnpPrdn_IsForwardPlay(ulHandle) == FALSE)
					{
						MhegIcsDebug("@@@@@ [%s:%d] Media[%d] g_bPrdnForwardPlay is FALSE, do not operate underflow routine.\n", __func__, __LINE__, ulHandle);
						continue;
					}
				}
#endif

#if 0
				/* Download Speed, Bitrate Compare */
				if(s_tMhegIcsUFMediaInfo.TunerIndex == DI_IPTUNER_DNP_HTTP)
				{
					MhegIcsDebug("@@@@@ [%s:%d] Before local_mheg_ics_iptuner_CompareDownloadSpeedWithBitrate()\n", __func__, __LINE__);
					nRet = local_mheg_ics_iptuner_CompareDownloadSpeedWithBitrate(&eUnderflowState, &bIsContinue);
					if(nRet != DI_ERR_OK)
					{
						MhegIcsError( ("@@@@@ [%s:%d:%d] Comparing Download speed with bitrate is failed\n", __func__, __LINE__, nRet);
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
				MhegIcsDebug("@@@@@ [%s:%d] Media[%d] CheckBufferingTimeOver =>State(%d)\n", __func__, __LINE__, ulHandle, s_eMhegIcsUFReadyState[ulHandle]);
				nRet = local_mheg_ics_iptuner_CheckBufferingTimeOver(ulHandle, &bCheckBufferingTimeOver, &ulStartBufferingClock, &bIsContinue);
				if(nRet != DI_ERR_OK)
				{
					MhegIcsError("@@@@@ [%s:%d:%d] Checking BufferingTimeOver is failed\n", __func__, __LINE__, nRet);
					break;
				}
				if(bIsContinue)
				{
					bIsContinue = FALSE;
					continue;
				}

				/* Check Underflow */
				MhegIcsDebug("@@@@@ [%s:%d] Media[%d] CheckUnderflow =>State(%d)\n", __func__, __LINE__, ulHandle, s_eMhegIcsUFReadyState[ulHandle]);
				nRet = local_mheg_ics_iptuner_CheckUnderflow(ulHandle);
				if(nRet != DI_ERR_OK)
				{
					MhegIcsError("@@@@@ [%s:%d:%d] Checking Underflow is failed\n", __func__, __LINE__, nRet);
					break;
				}
			}
			continue;
		}

		switch(stMsg.unMsg)
		{
			case MHEG_ICS_UNDERFLOW_MONITOR_MSG_STOP_TASK:			
				MhegIcsDebug ("%s(%d) Media[%d]: STOP_TASK\n",__FUNCTION__,__LINE__, ulHandle);
				s_eMhegIcsUFMonitorStatus[ulHandle] = MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Stop;
				break;
			
			case MHEG_ICS_UNDERFLOW_MONITOR_MSG_SLEEP_TASK:
				MhegIcsDebug ("%s(%d) Media[%d]: SLEEP_TASK\n",__FUNCTION__,__LINE__, ulHandle);
				s_eMhegIcsUFMonitorStatus[ulHandle] = MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Sleep;
				break;

			case MHEG_ICS_UNDERFLOW_MONITOR_MSG_AWAKE_TASK:
				MhegIcsDebug ("%s(%d) Media[%d]: AWAKE_TASK\n",__FUNCTION__,__LINE__, ulHandle);
				s_eMhegIcsUFMonitorStatus[ulHandle] = MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Start;
				s_eMhegIcsUFReadyState[ulHandle] = MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING;
				break;

			default:
				MhegIcsError("@@@@@ [%s:%d] Unkonw HTTP UF Message..!!\n", __func__, __LINE__);
				break;
		}
		
		
	}

	s_eMhegIcsUFMonitorStatus[ulHandle] = MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Stop;

	MhegIcsFunctionExit;

	return;
}

HINT32 DI_MHEG_ICS_IPTUNER_UnderflowGetReadyState(HUINT32 ulHandle)
{
	return (HINT32)s_eMhegIcsUFReadyState[ulHandle];
}

void DI_MHEG_ICS_IPTUNER_UnderflowSetReadyState(HUINT32 ulHandle, HINT32 eUnderflowState)
{
	if(s_eMhegIcsUFMonitorStatus[ulHandle] == MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Start)
	{
		s_eMhegIcsUFReadyState[ulHandle] = (MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_e)eUnderflowState;

		/* Ready State가 BUFFERING COMPLETED인 경우, 해당 이벤트를 APP에 올려준다. */
		if(s_eMhegIcsUFReadyState[ulHandle] == MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED
			&& s_pMhegIcsUFReadyStateCallbackFunc[ulHandle] != NULL)
		{
			s_pMhegIcsUFReadyStateCallbackFunc[ulHandle](ulHandle, DI_MEDIA_EVENT_BUFFER_READY_STATE, MHEG_ICS_IPTUNER_UNDERFLOW_READYSTATE_BUFFERING_COMPLETED);
		}
	}
	else
	{
		MhegIcsError("@@@@@ [%s:%d] # [Media:%d]MHEG_ICS_IPTUNER_UnderflowMonitor wasn't started..!!\n", __func__, __LINE__, ulHandle);
	}

	return;
}

DI_ERR_CODE DI_MHEG_ICS_IPTUNER_SetUnderflowReadyStateCallback(HUINT32 ulHandle, fn_readystate_callback pFn)
{
	DI_ERR_CODE nRet = DI_ERR_OK;

	if(pFn == NULL)
	{
		MhegIcsError("@@@@@ [%s:%d] pFn is NULL\n", __func__, __LINE__);
		nRet = DI_ERR_ERROR;
		goto FUNCTION_EXIT;
	}
	s_pMhegIcsUFReadyStateCallbackFunc[ulHandle] = (fn_readystate_callback)pFn;

FUNCTION_EXIT:
	return nRet;
}

HINT32 DI_MHEG_ICS_IPTUNER_UnderflowMonitorInitThread(HUINT32 ulHandle, DI_MEDIA_INFO_t *pstMediaInfo, HINT64 llFileLength)
{
	DI_IPTUNER_Msg_t stMsg;
	HINT32 nVKRet = ERR_OK;

	MhegIcsFunctionEnter(ulHandle);

	if(pstMediaInfo->max_bitrate == 0)
	{
		MhegIcsError("%s (%d) Error> Invaild Media Bitrate!!\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	if(pstMediaInfo->duration <= 0)
	{
		MhegIcsError("@@@@@ [%s:%d] duration <= 0\n", __func__, __LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	if(llFileLength == 0)
	{
		MhegIcsError("@@@@@ [%s:%d] s_nFileLength is 0\n", __func__, __LINE__);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	s_tMhegIcsUFMediaInfo[ulHandle].FileLength = llFileLength;
	s_tMhegIcsUFMediaInfo[ulHandle].MaxBitrate = pstMediaInfo->max_bitrate;
	s_tMhegIcsUFMediaInfo[ulHandle].Duration = pstMediaInfo->duration;
	//s_tMhegIcsUFMediaInfo[ulHandle].TunerIndex = nTunerIndex;

	if(s_eMhegIcsUFMonitorStatus[ulHandle] != MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Stop)
	{
		MhegIcsError("%s (%d) Error>  Already initialized !\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_OK;
		goto exit;
	}

	/* create & init message que */
	if (s_ulMhegIcsUFMonitorMsgQId[ulHandle] != 0)
	{
		MhegIcsError("%s (%d) Error> MsgQ was not destroyed.\n", __func__, __LINE__);
	}

	nVKRet = VK_MSG_Create(MHEG_ICS_IPTUNER_UNDERFLOW_MSG_QUEUE_SIZE, sizeof(stMsg), "MHEG_UNDERFLOW_MONITOR_MSGQ", &s_ulMhegIcsUFMonitorMsgQId[ulHandle], VK_SUSPENDTYPE_FIFO);
	if ( VK_OK != nVKRet )
	{
		MhegIcsError("%s (%d) Error> MHEG Underflow Monitoring Task msgQ creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
		nVKRet = ERR_FAIL;
		goto exit;
	}	/* Semaphore for shared data between main_task and timer_task */

	/* Create Download Task */
	if (s_ulMhegIcsUFMonitorTaskId[ulHandle] != 0)
	{
		MhegIcsError("%s (%d) Error Task[%d] was not destroyed.\n", __func__, __LINE__, ulHandle);
	}

	nVKRet = VK_TASK_Create ( local_mheg_ics_iptuner_UnderflowMonitorTask
			, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
			, MHEG_ICS_UNDERFLOW_MONITOR_STACK_SIZE
			, "MHEG_DecoderBuffer_Monitor"
			, (void *)ulHandle
			, &s_ulMhegIcsUFMonitorTaskId[ulHandle]
			, FALSE);
	if ( VK_OK != nVKRet )
	{
		MhegIcsError("%s (%d) Error> MHEG DecoderBuffer Monitoring task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		nVKRet = ERR_FAIL;
		s_ulMhegIcsUFMonitorTaskId[ulHandle] = 0;
		goto exit;
	}

	/** Start Monitoring Task */
	nVKRet = VK_TASK_Start(s_ulMhegIcsUFMonitorTaskId[ulHandle]);
	if ( VK_OK != nVKRet )
	{
		MhegIcsError("%s (%d) Error> MHEG DecoderBuffer Monitoring task start fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		nVKRet = ERR_FAIL;
		goto exit;
	}

	MhegIcsDebug("\t\t @@@ UFMonTask[%d] ID(0x%X) Create&Start..\n", ulHandle, s_ulMhegIcsUFMonitorTaskId[ulHandle]);
	

	nVKRet = ERR_OK;

exit:
	if (nVKRet != ERR_OK)
	{
		if (s_ulMhegIcsUFMonitorMsgQId[ulHandle])
		{
			(void) VK_MSG_Destroy (s_ulMhegIcsUFMonitorMsgQId[ulHandle]);
			s_ulMhegIcsUFMonitorMsgQId[ulHandle] = 0;
		}
		if (s_ulMhegIcsUFMonitorTaskId[ulHandle])
		{
			(void)VK_TASK_Destroy(s_ulMhegIcsUFMonitorTaskId[ulHandle]);
			s_ulMhegIcsUFMonitorTaskId[ulHandle] = 0;
		}

		VK_memset(&s_tMhegIcsUFMediaInfo[ulHandle], 0x00, sizeof(MHEG_ICS_IPTUNER_UNDERFLOW_MEDIAINFO_t));
	}

	MhegIcsFunctionExit;
	return nVKRet;
}

void DI_MHEG_ICS_IPTUNER_UnderflowMonitorTerminateThread(HUINT32 ulHandle)
{
	DI_IPTUNER_Msg_t stMsg;

	MhegIcsFunctionEnter(ulHandle);

	if(s_eMhegIcsUFMonitorStatus[ulHandle] != MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Stop)
	{
		(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));
		stMsg.unMsg = MHEG_ICS_UNDERFLOW_MONITOR_MSG_STOP_TASK;
		if(VK_MSG_Send(s_ulMhegIcsUFMonitorMsgQId[ulHandle], (HUINT32 *)&stMsg, sizeof(DI_IPTUNER_Msg_t)) != VK_OK)
		{
			MhegIcsError("%s(%d) VK_MSG_Send ERROR!! Terminating MHEG Decoder Buffer Monitoring Thread...\n",__FUNCTION__,__LINE__);
			goto forced_exit;
		}

        while(s_eMhegIcsUFMonitorStatus[ulHandle] != MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Stop)
        {
            VK_TASK_Sleep(100);
        }		
	}
	else
	{
		MhegIcsError("%s(%d) MHEG_ICS_IPTUNER_UnderflowMonitor is stoped\n", __FUNCTION__, __LINE__);
	}

forced_exit:	
	VK_TASK_Destroy(s_ulMhegIcsUFMonitorTaskId[ulHandle]);
	MhegIcsDebug("\t\t @@@ UFMonTask[%d] ID(0x%X) Destroy..\n", ulHandle, s_ulMhegIcsUFMonitorTaskId[ulHandle]);
	s_ulMhegIcsUFMonitorTaskId[ulHandle] = 0;

	(void) VK_MSG_Destroy(s_ulMhegIcsUFMonitorMsgQId[ulHandle]);
	s_ulMhegIcsUFMonitorMsgQId[ulHandle] = 0;

	VK_memset(&s_tMhegIcsUFMediaInfo[ulHandle], 0x00, sizeof(MHEG_ICS_IPTUNER_UNDERFLOW_MEDIAINFO_t));
	s_pMhegIcsUFReadyStateCallbackFunc[ulHandle] = NULL;

	MhegIcsDebug("%s(%d) DI_MHEG_ICS_IPTUNER_UnderflowMonitorTerminateThread[%d] ..Done!!\n", __FUNCTION__, __LINE__, ulHandle);

	MhegIcsFunctionExit;

	return;
}

void 		DI_MHEG_ICS_IPTUNER_UnderflowMonitorSleepThread(HUINT32 ulHandle)
{
	DI_IPTUNER_Msg_t stMsg;

	MhegIcsFunctionEnter(ulHandle);
	
	if((s_ulMhegIcsUFMonitorTaskId[ulHandle] != 0) && (s_eMhegIcsUFMonitorStatus[ulHandle] == MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Start))
	{
		(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));
		stMsg.unMsg = MHEG_ICS_UNDERFLOW_MONITOR_MSG_SLEEP_TASK;
		if(VK_MSG_Send(s_ulMhegIcsUFMonitorMsgQId[ulHandle], (HUINT32 *)&stMsg, sizeof(DI_IPTUNER_Msg_t)) != VK_OK)
		{
			MhegIcsError("%s(%d) VK_MSG_Send ERROR!! Sleeping MHEG Decoder Buffer Monitoring Thread...\n",__FUNCTION__,__LINE__);		
		}
	}
	else
	{
		MhegIcsError("%s(%d) MHEG_ICS_IPTUNER_UnderflowMonitor is not started\n", __FUNCTION__, __LINE__);
	}

	MhegIcsFunctionExit;
}

void 		DI_MHEG_ICS_IPTUNER_UnderflowMonitorAwakeThread(HUINT32 ulHandle)
{
	DI_IPTUNER_Msg_t stMsg;
	
	MhegIcsFunctionEnter(ulHandle);

	if((s_ulMhegIcsUFMonitorTaskId[ulHandle] != 0) && (s_eMhegIcsUFMonitorStatus[ulHandle] == MHEG_ICS_IPTUNER_UnderflowMonitorStatus_Sleep))
	{
		(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));
		stMsg.unMsg = MHEG_ICS_UNDERFLOW_MONITOR_MSG_AWAKE_TASK;
		if(VK_MSG_Send(s_ulMhegIcsUFMonitorMsgQId[ulHandle], (HUINT32 *)&stMsg, sizeof(DI_IPTUNER_Msg_t)) != VK_OK)
		{
			MhegIcsError("%s(%d) VK_MSG_Send ERROR!! Awake MHEG Decoder Buffer Monitoring Thread...\n",__FUNCTION__,__LINE__);		
		}
	}
	else
	{
		MhegIcsError("%s(%d) MHEG_ICS_IPTUNER_UnderflowMonitor is not sleep..\n", __FUNCTION__, __LINE__);
	}
	
	MhegIcsFunctionExit;
}


void 		DI_MHEG_ICS_IPTUNER_UnderflowMonitorUpdateEndPostion(HUINT32 ulHandle, HINT64 nEndPos)
{
	MhegIcsFunctionEnter(ulHandle);

	MhegIcsDebug("\n %s(%d) @@@@ Update EndPosition (%"PRId64") => (%"PRId64")\n", __FUNCTION__, __LINE__, s_tMhegIcsUFMediaInfo[ulHandle].FileLength, nEndPos);
	s_tMhegIcsUFMediaInfo[ulHandle].FileLength = nEndPos;	
	
	MhegIcsFunctionExit;
}


