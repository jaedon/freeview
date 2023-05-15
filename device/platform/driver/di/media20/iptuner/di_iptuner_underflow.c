/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_underflow.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:	Byung-min Ha $
 * Current Author:	$Author: bmha@humaxdigital.com $
 * Date:			$Date: Fri November 1 17:40:00 KST 2013 $
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

#include "di_iptuner_underflow.h"

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
#define PrintError		VK_printf
#define PrintEnter
#define PrintExit
#endif /* END DEBUG DEFINE */

#define UNUSED_PARAM(x)	 ((void)x)

/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

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
static void P_IPTUNER_UnderflowMonitorTask(void *pParam);

void P_IPTUNER_PrintUnderflowInfo(DI_IPTUNER_UNDERFLOW_t *ptUnderflow)
{
	PrintDebug("[%s:%d] ptHandle = %x\n",__FUNCTION__,__LINE__, (HUINT32)ptUnderflow->ptHandle);
	PrintDebug("[%s:%d] pHTuner = %x\n",__FUNCTION__,__LINE__, (HUINT32)ptUnderflow->pHTuner);
	PrintDebug("[%s:%d] pMediaInfo = %x\n",__FUNCTION__,__LINE__, (HUINT32)ptUnderflow->pMediaInfo);
	PrintDebug("[%s:%d] pAccess = %x\n",__FUNCTION__,__LINE__, (HUINT32)ptUnderflow->pAccess);
	PrintDebug("[%s:%d] vDecoder = %x\n",__FUNCTION__,__LINE__, (HUINT32)ptUnderflow->vDecoder);
	PrintDebug("[%s:%d] aDecoder = %x\n",__FUNCTION__,__LINE__, (HUINT32)ptUnderflow->aDecoder);

	PrintDebug("[%s:%d] ulUnderflowMonitorMsgQId = %lu\n",__FUNCTION__,__LINE__, ptUnderflow->ulUnderflowMonitorMsgQId);
	PrintDebug("[%s:%d] ulUnderflowMonitorTaskId = %lu\n",__FUNCTION__,__LINE__, ptUnderflow->ulUnderflowMonitorTaskId);

	PrintDebug("[%s:%d] unUnderflowCount = %d\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.unUnderflowCount);
	PrintDebug("[%s:%d] unEnoughCount = %d\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.unEnoughCount);
	PrintDebug("[%s:%d] ulTotalPausedTime = %llu\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.ulTotalPausedTime);

	PrintDebug("[%s:%d] unMaxBufferSize = %d\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.unMaxBufferSize);
	PrintDebug("[%s:%d] unSizeofOneSec = %d\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.unSizeofOneSec);
	PrintDebug("[%s:%d] unUnderflowThreshold = %d\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.unUnderflowThreshold);
	PrintDebug("[%s:%d] unEnoughThreshold = %d\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.unEnoughThreshold);

	PrintDebug("[%s:%d] eThresholdMode = %d\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.eThresholdMode);
	PrintDebug("[%s:%d] eUnderflowMonitorStatus = %d\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.eUnderflowMonitorStatus);
}

DI_ERR_CODE P_IPTUNER_GetMaxBufferDepth(DI_IPTUNER_UNDERFLOW_t *ptUnderflow)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	NEXUS_Error nNexusError = NEXUS_SUCCESS;
	NEXUS_VideoDecoderStatus tvDecoderStatus;
	NEXUS_AudioDecoderStatus taDecoderStatus;

	if(ptUnderflow->pMediaInfo->ucTotalVideoTrack > 0)
	{
		(void)VK_memset(&tvDecoderStatus, 0x00, sizeof(NEXUS_VideoDecoderStatus));
		nNexusError = NEXUS_VideoDecoder_GetStatus(ptUnderflow->vDecoder, &tvDecoderStatus);
		if(nNexusError == NEXUS_SUCCESS)
		{
			ptUnderflow->tUnderflowInfo.unMaxBufferSize = tvDecoderStatus.fifoSize;				/* size in bytes of the compressed buffer */
		}
		else
		{
			nRet = DI_ERR_ERROR;
		}
	}
	PrintDebug("[%s:%d] tvDecoderStatus.fifoSize = %u\n",__FUNCTION__,__LINE__, tvDecoderStatus.fifoSize);

	if(ptUnderflow->pMediaInfo->ucTotalAudioTrack > 0)
	{
		(void)VK_memset(&taDecoderStatus, 0x00, sizeof(NEXUS_AudioDecoderStatus));
		nNexusError = NEXUS_AudioDecoder_GetStatus(ptUnderflow->aDecoder, &taDecoderStatus);
		if(nNexusError == NEXUS_SUCCESS)
		{
			ptUnderflow->tUnderflowInfo.unMaxBufferSize += taDecoderStatus.fifoSize;			/* depth in bytes of the compressed buffer */
		}
		else
		{
			nRet = DI_ERR_ERROR;
		}
	}
	PrintDebug("[%s:%d] taDecoderStatus.fifoSize = %u\n",__FUNCTION__,__LINE__, taDecoderStatus.fifoSize);
	PrintDebug("[%s:%d] ptUnderflow->tUnderflowInfo.unMaxBufferSize = %d\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.unMaxBufferSize);
	return nRet;
}

DI_ERR_CODE P_IPTUNER_GetVideoBufferDepth(DI_IPTUNER_UNDERFLOW_t *ptUnderflow, HUINT32 *ulSize)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	NEXUS_Error nNexusError = NEXUS_SUCCESS;
	NEXUS_VideoDecoderStatus tDecoderStatus;

	if(ulSize == NULL)
	{
		PrintError("[%s:%d] ulSize is NULL\n", __FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		goto P_IPTUNER_GetVideoBufferDepth_Exit;
	}

	(void)VK_memset(&tDecoderStatus, 0x00, sizeof(NEXUS_VideoDecoderStatus));

	// vDecoder 가 NULL 이면 init 에서 fail 되어야 한다.
	nNexusError = NEXUS_VideoDecoder_GetStatus(ptUnderflow->vDecoder, &tDecoderStatus);
	if(nNexusError == NEXUS_SUCCESS)
	{
		*ulSize = tDecoderStatus.fifoDepth;				/* depth in bytes of the compressed buffer */
	}
	else
	{
		nRet = DI_ERR_ERROR;
	}

P_IPTUNER_GetVideoBufferDepth_Exit:
	return nRet;
}

DI_ERR_CODE P_IPTUNER_GetAudioBufferDepth(DI_IPTUNER_UNDERFLOW_t *ptUnderflow, HUINT32 *ulSize)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	NEXUS_Error nNexusError = NEXUS_SUCCESS;
	NEXUS_AudioDecoderStatus tDecoderStatus;

	if(ulSize == NULL)
	{
		PrintError("[%s:%d] ulSize is NULL\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		goto P_IPTUNER_GetAudioBufferDepth_Exit;
	}

	(void)VK_memset(&tDecoderStatus, 0x00, sizeof(NEXUS_AudioDecoderStatus));

	// aDecoder 가 NULL 이면 init 에서 fail 되어야 한다.
	nNexusError = NEXUS_AudioDecoder_GetStatus(ptUnderflow->aDecoder, &tDecoderStatus);
	if(nNexusError == NEXUS_SUCCESS)
	{
		*ulSize = tDecoderStatus.fifoDepth;			/* depth in bytes of the compressed buffer */
	}
	else
	{
		nRet = DI_ERR_ERROR;
	}

P_IPTUNER_GetAudioBufferDepth_Exit:
	return nRet;
}

DI_ERR_CODE P_IPTUNER_GetDecoderBufferDepth(DI_IPTUNER_UNDERFLOW_t *ptUnderflow, HUINT32 *ulSize)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HUINT32 unVideoBufferDepth = 0, unAudioBufferDepth = 0;

	PrintEnter;

	// Video Track 이 있을 때만 video decoder 검사
	if(ptUnderflow->pMediaInfo->ucTotalVideoTrack > 0)
	{
		nRet = P_IPTUNER_GetVideoBufferDepth(ptUnderflow, &unVideoBufferDepth);
		if(nRet != DI_ERR_OK)
		{
			PrintError("[%s:%d] Getting VideoBufferDepth is Error\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			goto GetDecoderBufferDepth_Exit;
		}
		PrintDebug("[%s:%d] unVideoBufferDepth = %d\n",__FUNCTION__,__LINE__, unVideoBufferDepth);
	}

	// Audio Track 이 있을 때만 audio decoder 검사
	if(ptUnderflow->pMediaInfo->ucTotalAudioTrack > 0)
	{
		nRet = P_IPTUNER_GetAudioBufferDepth(ptUnderflow, &unAudioBufferDepth);
		if(nRet != DI_ERR_OK)
		{
			PrintError("[%s:%d] Getting unAudioBufferDepth is Error\n",__FUNCTION__,__LINE__);
			nRet = DI_ERR_ERROR;
			goto GetDecoderBufferDepth_Exit;
		}
		PrintDebug("[%s:%d] unAudioBufferDepth = %d\n",__FUNCTION__,__LINE__, unAudioBufferDepth);
	}

	*ulSize = unVideoBufferDepth + unAudioBufferDepth;

GetDecoderBufferDepth_Exit:
	PrintExit;
	return nRet;
}


DI_ERR_CODE P_IPTUNER_CheckBuffer(DI_IPTUNER_UNDERFLOW_t *ptUnderflow, HUINT32 Threshold, HBOOL *bUnderflow, HBOOL *bEnough)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	HUINT32 unBufferedData = 0;

	PrintEnter;

	if(P_IPTUNER_GetDecoderBufferDepth(ptUnderflow, &unBufferedData) != DI_ERR_OK)
	{
		PrintDebug("[%s:%d] P_IPTUNER_GetDecoderBufferDepth Fail.\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		goto P_IPTUNER_CheckBuffer_Exit;
	}

	PrintDebug("[%s:%d] Threshold = %d\n",__FUNCTION__,__LINE__, Threshold);
	PrintDebug("[%s:%d] unBufferedData = %d\n",__FUNCTION__,__LINE__, unBufferedData);

	if(unBufferedData > Threshold)
	{
		*bUnderflow = FALSE;
		*bEnough = TRUE;
	}
	else // ullBufferedData < Threshold
	{
		*bEnough = FALSE;

		// EOF 근처에서의 underflow 처리.
		PrintDebug("[%s:%d] i_size = %lld, i_pos = %lld \n",__FUNCTION__,__LINE__, ptUnderflow->pAccess->info.i_size, ptUnderflow->pAccess->info.i_pos);
		if(ptUnderflow->pAccess->info.i_size - ptUnderflow->pAccess->info.i_pos < ptUnderflow->tUnderflowInfo.unMaxBufferSize)
		{
			PrintDebug("[%s:%d] Remain data is lower than unMaxBufferSize\n",__FUNCTION__,__LINE__);
			*bUnderflow = FALSE;
			*bEnough = TRUE;
		}
		else
		{
			*bUnderflow = TRUE;
		}
	}
	PrintDebug("[%s:%d] bUnderflow = %s, bEnough = %s\n", __FUNCTION__,__LINE__, *bUnderflow?"TRUE":"FALSE", *bEnough?"TRUE":"FALSE");
P_IPTUNER_CheckBuffer_Exit:

	PrintExit;
	return nRet;
}

void P_IPTUNER_CheckBufferingTimeOut(DI_IPTUNER_UNDERFLOW_t *ptUnderflow, HBOOL *pbBufferingTimeOver, HBOOL *pbIsChangeMaxBufferSize)
{
	HULONG ulCurrentClock = 0;
	HUINT32 unBufferedData = 0;

	ulCurrentClock = VK_TIMER_GetSystemTick();

	if(ulCurrentClock - ptUnderflow->tUnderflowInfo.ulStartBufferingClock > ptUnderflow->tUnderflowInfo.ulBufferingTimeout)
	{
		// timeout
 		*pbBufferingTimeOver = TRUE;
	}
	else
	{
		*pbBufferingTimeOver = FALSE;
		if(ptUnderflow->tUnderflowInfo.eThresholdMode == DI_IPTUNER_Underflow_Threshold_Mode_Auto)
		{
			if(P_IPTUNER_GetDecoderBufferDepth(ptUnderflow, &unBufferedData) != DI_ERR_OK)
			{
				PrintDebug("[%s:%d] P_IPTUNER_GetDecoderBufferDepth Fail.\n", __FUNCTION__,__LINE__);
			}
			else
			{
				if(ptUnderflow->tUnderflowInfo.unPrevBufferedSize == unBufferedData)
				{
					ptUnderflow->tUnderflowInfo.unSameSizeCount++;
				}
				else
				{
					ptUnderflow->tUnderflowInfo.unSameSizeCount = 0;
					ptUnderflow->tUnderflowInfo.unPrevBufferedSize = unBufferedData;
				}
			}
		}
	}

	// Max buffersize 를 조절해야 하나?
	if(ptUnderflow->tUnderflowInfo.unSameSizeCount > 3)
	{
		*pbIsChangeMaxBufferSize = TRUE;
	}
	else
	{
		*pbIsChangeMaxBufferSize = FALSE;
	}
	PrintDebug("[%s:%d] pbBufferingTimeOver = %s, pbIsChangeMaxBufferSize = %s\n", __FUNCTION__,__LINE__, *pbBufferingTimeOver?"TRUE":"FALSE", *pbIsChangeMaxBufferSize?"TRUE":"FALSE");
}

void P_IPTUNER_Do_Check_Underflow(DI_IPTUNER_UNDERFLOW_t *ptUnderflow)
{
	HBOOL bUnderflow = FALSE, bEnough = FALSE, bTimeout = FALSE, bIsChangeMaxBufferSize = FALSE;
	PrintEnter;

	if(ptUnderflow->pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_INIT ||
		ptUnderflow->pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_PAUSE)
	{
		// DI_IPTUNER_UNDERFLOW_INIT : contetents 시작 직후. start threshold 를 보고 enough check 해야 함
		// DI_IPTUNER_UNDERFLOW_PAUSE : Underflow 상태.  계산된 unEnoughThreshold 를 보고 enough check 해야 함
		if(P_IPTUNER_CheckBuffer(ptUnderflow, ptUnderflow->tUnderflowInfo.unEnoughThreshold, &bUnderflow, &bEnough) != DI_ERR_OK)
		{
			PrintError ("[%s:%d] P_IPTUNER_CheckBuffer Fail!! \n",__FUNCTION__,__LINE__);
		}
		else
		{
			if(bEnough == FALSE)
			{
				P_IPTUNER_CheckBufferingTimeOut(ptUnderflow, &bTimeout, &bIsChangeMaxBufferSize);
			}
			if(bTimeout == TRUE)
			{
				PrintDebug ("[%s:%d] Buffering Timeout. \n",__FUNCTION__,__LINE__);
				bEnough = TRUE;
				if(bIsChangeMaxBufferSize == TRUE)
				{
					PrintDebug ("[%s:%d] Change max buffer size. \n",__FUNCTION__,__LINE__);
					if(P_IPTUNER_GetDecoderBufferDepth(ptUnderflow, &(ptUnderflow->tUnderflowInfo.unMaxBufferSize)) != DI_ERR_OK)
					{
						if(P_IPTUNER_GetMaxBufferDepth(ptUnderflow) != DI_ERR_OK)
						{
							PrintError ("[%s:%d] Warning > P_IPTUNER_GetMaxBufferDepth Error. \n",__FUNCTION__,__LINE__);
							ptUnderflow->tUnderflowInfo.unMaxBufferSize = ptUnderflow->tUnderflowInfo.unSizeofOneSec * 10; // Default : 10 sec
						}
					}

					// resize 된 buffer size 로 threshold 다시 계산
					ptUnderflow->pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_CANPLAY;
					goto P_IPTUNER_Do_Check_Underflow_Exit;
				}
			}

			if(bEnough == TRUE)
			{
				if(ptUnderflow->pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_INIT)
				{
					// 원래 Canplay Event 를 올려줘야 하나 Realtime 인 경우 probe 이후 Canplay 가 호출되어야 buffering 이 시작된다.
					// 때문에 Canplay 는 무시한다.
					PrintDebug ("[%s:%d] Change Status. INIT -> CANPLAY \n",__FUNCTION__,__LINE__);
					ptUnderflow->pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_CANPLAY;
				}
				else // ptUnderflow->pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_PAUSE
				{
					if(ptUnderflow->pHTuner->pBufferCallbacks != NULL && ptUnderflow->pHTuner->pBufferCallbacks->cbResume != NULL)
					{
						ptUnderflow->pHTuner->pBufferCallbacks->cbResume((HUINT32)(ptUnderflow->pHTuner));
					}
					PrintDebug ("[%s:%d] Change Status. PAUSE -> RESUME \n",__FUNCTION__,__LINE__);
					ptUnderflow->pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_RESUME;
					ptUnderflow->tUnderflowInfo.unEnoughCount++;
					ptUnderflow->tUnderflowInfo.ulTotalPausedTime += VK_TIMER_GetSystemTick() - ptUnderflow->tUnderflowInfo.ulStartBufferingClock;
				}
			}
		}
	}
	else if(ptUnderflow->pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_CANPLAY)
	{
		// start threshold 넘었음. threshold 값 변경
		if(ptUnderflow->pHTuner->nBufferThreshold == 0)
		{
			ptUnderflow->pHTuner->nBufferThreshold = ptUnderflow->pHTuner->ulStartThreshold;
		}
		ptUnderflow->tUnderflowInfo.unUnderflowThreshold = (ptUnderflow->tUnderflowInfo.unSizeofOneSec * (ptUnderflow->pHTuner->nBufferThreshold / 1000)) / 10;
		ptUnderflow->tUnderflowInfo.unEnoughThreshold = ptUnderflow->tUnderflowInfo.unSizeofOneSec * (ptUnderflow->pHTuner->nBufferThreshold / 1000);

		if(ptUnderflow->tUnderflowInfo.unMaxBufferSize < 1000)
		{
			PrintError ("[%s:%d] unMaxBufferSize is too small. \n",__FUNCTION__,__LINE__);
			if(P_IPTUNER_GetDecoderBufferDepth(ptUnderflow, &(ptUnderflow->tUnderflowInfo.unMaxBufferSize)) != DI_ERR_OK)
			{
				PrintError ("[%s:%d] Warning > P_IPTUNER_GetDecoderBufferDepth Error. \n",__FUNCTION__,__LINE__);
			}
		}

		// unUnderflowThreshold 가 unMaxBufferSize 의 10% 이하이면 pause 가 깔끔하게 안됨.
		if(ptUnderflow->tUnderflowInfo.unUnderflowThreshold < ptUnderflow->tUnderflowInfo.unMaxBufferSize / 10)
		{
			ptUnderflow->tUnderflowInfo.unUnderflowThreshold = (HUINT32)(ptUnderflow->tUnderflowInfo.unMaxBufferSize / 10);
		}
		else if(ptUnderflow->tUnderflowInfo.unUnderflowThreshold > (ptUnderflow->tUnderflowInfo.unMaxBufferSize / 10) * 2 ||
			ptUnderflow->tUnderflowInfo.unUnderflowThreshold > ptUnderflow->tUnderflowInfo.unMaxBufferSize)
		{
			ptUnderflow->tUnderflowInfo.unUnderflowThreshold = (HUINT32)(ptUnderflow->tUnderflowInfo.unMaxBufferSize / 10) * 2;
		}

		// unEnoughThreshold 값이 unMaxBufferSize 의 90% 이상이면 enough 가 안될수 있다.
		if(ptUnderflow->tUnderflowInfo.unEnoughThreshold > (ptUnderflow->tUnderflowInfo.unMaxBufferSize / 10) * 9)
		{
			ptUnderflow->tUnderflowInfo.unEnoughThreshold = (ptUnderflow->tUnderflowInfo.unMaxBufferSize / 10) * 9;
		}

		P_IPTUNER_PrintUnderflowInfo(ptUnderflow);

		if(ptUnderflow->pHTuner->pBufferCallbacks != NULL && ptUnderflow->pHTuner->pBufferCallbacks->cbResume != NULL)
		{
			ptUnderflow->pHTuner->pBufferCallbacks->cbResume((HUINT32)(ptUnderflow->pHTuner));
		}
		PrintDebug ("[%s:%d] Change Status. CANPLAY -> RESUME \n",__FUNCTION__,__LINE__);
		ptUnderflow->pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_RESUME;
	}
	else if(ptUnderflow->pHTuner->eUnderflowState == DI_IPTUNER_UNDERFLOW_RESUME)
	{
		// contents play 중. Underflow check 해야 함.
		if(P_IPTUNER_CheckBuffer(ptUnderflow, ptUnderflow->tUnderflowInfo.unUnderflowThreshold, &bUnderflow, &bEnough) != DI_ERR_OK)
		{
			PrintError ("[%s:%d] P_IPTUNER_CheckBuffer Fail!! \n",__FUNCTION__,__LINE__);
		}
		else
		{
			if(bUnderflow == TRUE)
			{
				// DI callback 중 underflow 는 세팅안됨. underflow 가 pause 에 연결됨. PAL_MEDIA_StartBuffering 참고
				if(ptUnderflow->pHTuner->pBufferCallbacks != NULL && ptUnderflow->pHTuner->pBufferCallbacks->cbPause != NULL)
				{
					ptUnderflow->pHTuner->pBufferCallbacks->cbPause((HUINT32)(ptUnderflow->pHTuner));
				}
				PrintDebug ("[%s:%d] Change Status. RESUME -> PAUSE \n",__FUNCTION__,__LINE__);
				ptUnderflow->pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_PAUSE;
				ptUnderflow->tUnderflowInfo.unUnderflowCount++;
				ptUnderflow->tUnderflowInfo.ulStartBufferingClock = VK_TIMER_GetSystemTick();
			}
		}
	}
P_IPTUNER_Do_Check_Underflow_Exit:
	PrintExit;
	return ;
}


static void P_IPTUNER_UnderflowMonitorTask(void *pParam)
{
	DI_IPTUNER_Msg_t stMsg;
	DI_IPTUNER_UNDERFLOW_t	*ptUnderflow = NULL;
	HINT32 nVKRet = ERR_FAIL;
	HINT32	nTrickRate = 0;

	PrintEnter;

	if(pParam == NULL)
	{
		PrintError ("[%s:%d] pParam is NULL!!\n", __FUNCTION__, __LINE__);
		goto UNDERFLOW_MONITOR_EXIT;
	}
	(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));

	ptUnderflow = (DI_IPTUNER_UNDERFLOW_t *)pParam;

	// Contents max bitrate 로 1초 분량 크기 구함.
	ptUnderflow->tUnderflowInfo.unSizeofOneSec = ptUnderflow->pMediaInfo->max_bitrate >> 3;
	ptUnderflow->tUnderflowInfo.ulBufferingTimeout = BUFFERING_TIMEOUT;
	ptUnderflow->tUnderflowInfo.eThresholdMode = DI_IPTUNER_Underflow_Threshold_Mode_Auto;

	if(P_IPTUNER_GetMaxBufferDepth(ptUnderflow) != DI_ERR_OK)
	{
		PrintError ("[%s:%d] Warning > P_IPTUNER_GetMaxBufferDepth Error. \n",__FUNCTION__,__LINE__);
		ptUnderflow->tUnderflowInfo.unMaxBufferSize = ptUnderflow->tUnderflowInfo.unSizeofOneSec * 10; // Default : 10 sec
	}

	ptUnderflow->pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_INIT;
	if(ptUnderflow->pHTuner->pBufferCallbacks != NULL && ptUnderflow->pHTuner->pBufferCallbacks->cbPause != NULL)
	{
		ptUnderflow->pHTuner->pBufferCallbacks->cbPause((HUINT32)(ptUnderflow->pHTuner));
	}
	ptUnderflow->tUnderflowInfo.ulStartBufferingClock = VK_TIMER_GetSystemTick();

	// 처음에는 start threshold 값을 사용...
	ptUnderflow->tUnderflowInfo.unEnoughThreshold = ptUnderflow->tUnderflowInfo.unSizeofOneSec * (ptUnderflow->pHTuner->ulStartThreshold / 1000);
	// unEnoughThreshold 값이 unMaxBufferSize 의 90% 이상이면 enough 가 안될수 있다.
	if(ptUnderflow->tUnderflowInfo.unEnoughThreshold > (ptUnderflow->tUnderflowInfo.unMaxBufferSize / 10) * 9)
	{
		if(ptUnderflow->tUnderflowInfo.unMaxBufferSize < 1000)
		{
			PrintError ("[%s:%d] unMaxBufferSize is too small. \n",__FUNCTION__,__LINE__);
		}
		else
		{
			ptUnderflow->tUnderflowInfo.unEnoughThreshold = (ptUnderflow->tUnderflowInfo.unMaxBufferSize / 10) * 9;
		}
	}

	// Main loop
	while (1)
	{
		nVKRet = VK_MSG_ReceiveTimeout (ptUnderflow->ulUnderflowMonitorMsgQId, &stMsg, sizeof(DI_IPTUNER_Msg_t), 1000);
		if(nVKRet == VK_OK)
		{
			switch(stMsg.unMsg)
			{
				case DI_IPTUNER_UnderflowMonitor_Msg_Start_Task:
					PrintDebug ("[%s:%d] DI_IPTUNER_UnderflowMonitor_Msg_Start_Task Received. \n",__FUNCTION__,__LINE__);
					// pause 였다가 다시 시작하는 경우 threshold 값을 다시 계산한다.
					if(ptUnderflow->tUnderflowInfo.eUnderflowMonitorStatus == DI_IPTUNER_UnderflowMonitorStatus_Pause)
					{
						ptUnderflow->pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_CANPLAY;
					}
					ptUnderflow->tUnderflowInfo.eUnderflowMonitorStatus = DI_IPTUNER_UnderflowMonitorStatus_Start;
					break;
				case DI_IPTUNER_UnderflowMonitor_Msg_Pause_Task:
					PrintDebug ("[%s:%d] DI_IPTUNER_UnderflowMonitor_Msg_Pause_Task Received. \n",__FUNCTION__,__LINE__);
					break;
				case DI_IPTUNER_UnderflowMonitor_Msg_Stop_Task:
					PrintDebug ("[%s:%d] DI_IPTUNER_UnderflowMonitor_Msg_Stop_Task Received. \n",__FUNCTION__,__LINE__);
					ptUnderflow->tUnderflowInfo.eUnderflowMonitorStatus = DI_IPTUNER_UnderflowMonitorStatus_Stop;
					goto UNDERFLOW_MONITOR_EXIT;
					break;
			}
		}
		else if(ptUnderflow->tUnderflowInfo.eUnderflowMonitorStatus == DI_IPTUNER_UnderflowMonitorStatus_Start && nVKRet == VK_TIMEOUT)
		{
#if 0 // trick mode 지원을 위해서는 get speed 방식 변화 필요. youtube test 에서 deadlock 걸림. 관련이슈 : #77669
			if(DI_MEDIA_GetSpeed(ptUnderflow->pHTuner, 0, &nTrickRate) != DI_ERR_OK)
			{
				PrintError ("[%s:%d] DI_MEDIA_GetSpeed Error!! \n",__FUNCTION__,__LINE__);
			}
			else
#endif
			{
				// trick mode 에서는 동작하지 않음...
				if( eDI_MEDIA_Speed_Pause <= nTrickRate || nTrickRate < eDI_MEDIA_Speed_FF_X4)
				{
					P_IPTUNER_Do_Check_Underflow(ptUnderflow);
				}
			}
		}
		else
		{
			// VK_MSG_ReceiveTimeout Fail... Msg Queue 가 destroy 되었을 수 있다.
			PrintError ("[%s:%d] VK_MSG_ReceiveTimeout Fail!! stop thread...\n",__FUNCTION__,__LINE__);
			// DI_IPTUNER_UnderflowMonitorTerminateThread 가 수행되었으면 ptUnderflow 는 free 되었으므로 access 금지
			//ptUnderflow->tUnderflowInfo.eUnderflowMonitorStatus = DI_IPTUNER_UnderflowMonitorStatus_Stop;
			goto UNDERFLOW_MONITOR_EXIT;
		}
	}

UNDERFLOW_MONITOR_EXIT:
	PrintExit;
	return;
}

HINT32 DI_IPTUNER_UnderflowMonitorInitThread(VFIO_Handle_t *ptHandle)
{
	DI_IPTUNER_Msg_t stMsg;
	DI_IPTUNER_UNDERFLOW_t *ptUnderflow = NULL;
	HINT32 nVKRet = ERR_OK;

	PrintEnter;

	if(ptHandle == NULL)
	{
		PrintError ("[%s:%d] ptHandle is NULL!!\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;
	}

	ptUnderflow = VK_MEM_Alloc(sizeof(DI_IPTUNER_UNDERFLOW_t));
	if(ptUnderflow == NULL)
	{
		PrintError ("[%s:%d] ptUnderflow is NULL!!\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;
	}
	VK_memset(ptUnderflow, 0x00, sizeof(DI_IPTUNER_UNDERFLOW_t));

	ptUnderflow->ptHandle = ptHandle;

	ptUnderflow->pHTuner = ptHandle->pTunerHandle;
	if (ptUnderflow->pHTuner == NULL)
	{
		PrintError ("[%s:%d] Invalid Tuner Handle in VFIO Handle\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;
	}

	ptUnderflow->pMediaInfo = &(ptUnderflow->pHTuner->tMediaInfo);
	if (ptUnderflow->pMediaInfo == NULL)
	{
		PrintError ("[%s:%d] Invalid Media Info in VFIO Handle\n",__FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;
	}

	ptUnderflow->pAccess = (STREAM_ACCESS_t *)ptUnderflow->pHTuner->pHStreamData;
	if(ptUnderflow->pAccess->info.i_size == 0)
	{
		PrintError("[%s:%d] File Length is 0\n", __FUNCTION__,__LINE__);
		nVKRet = ERR_FAIL;
		goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;
	}

	if(ptUnderflow->pMediaInfo->ucTotalVideoTrack > 0)
	{
		// Video Decoder Handle 을 항상 0으로 가져온다.
		// Video Decoder 를 2개 이상 사용할 때는 문제가 될 수 있다.
		DRV_VIDEO_GetVideoHandle(0, &(ptUnderflow->vDecoder));
		if(ptUnderflow->vDecoder == NULL)
		{
			PrintError("[%s:%d] vDecoder is NULL!\n", __FUNCTION__,__LINE__);
			nVKRet = ERR_FAIL;
			goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;
		}
	}

	if(ptUnderflow->pMediaInfo->ucTotalAudioTrack > 0)
	{
		// Audio Decoder Handle 을 항상 0으로 가져온다.
		// Audio Decoder 를 Sub 로 사용할 때는 문제가 된다.
		DRV_AUDIO_GetDecoderHandle(0, &(ptUnderflow->aDecoder));
		if(ptUnderflow->aDecoder == NULL)
		{
			PrintError("[%s:%d] aDecoder is NULL!\n", __FUNCTION__,__LINE__);
			nVKRet = ERR_FAIL;
			goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;
		}
	}

	nVKRet = VK_MSG_Create(TUNER_UNDERFLOW_MSG_QUEUE_SIZE, sizeof(stMsg), "UNDERFLOW_MONITOR_MSGQ", &(ptUnderflow->ulUnderflowMonitorMsgQId), VK_SUSPENDTYPE_FIFO);
	if ( VK_OK != nVKRet )
	{
		PrintError ("[%s:%d] Underflow Monitoring Task msgQ creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet );
		nVKRet = ERR_FAIL;
		goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;;
	}

	nVKRet = VK_TASK_Create ( P_IPTUNER_UnderflowMonitorTask
			, PLAYBACK_LOAD_TASK_PRIORITY	// Buffering TASK Priority 는 Playback 만큼 또는 그 이상 높아야 한다.
			, UNDERFLOW_MONITOR_STACK_SIZE
			, "Underflow_Monitor_Task"
			, (void *)ptUnderflow
			, &(ptUnderflow->ulUnderflowMonitorTaskId)
			, FALSE);
	if ( VK_OK != nVKRet )
	{
		PrintError ("[%s:%d] DecoderBuffer Monitoring task creation fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		nVKRet = ERR_FAIL;
		ptUnderflow->ulUnderflowMonitorTaskId = 0;
		goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;
	}

	/** Start Monitoring Task */
	nVKRet = VK_TASK_Start(ptUnderflow->ulUnderflowMonitorTaskId);
	if ( VK_OK != nVKRet )
	{
		PrintError ("[%s:%d] DecoderBuffer Monitoring task start fail err = 0x%x\n",__FUNCTION__,__LINE__, nVKRet);
		nVKRet = ERR_FAIL;
		goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;
	}

	nVKRet = ERR_OK;

DI_IPTUNER_UnderflowMonitorInitThread_Exit:
	if (nVKRet != ERR_OK)
	{
		if(ptUnderflow != NULL)
		{
			if (ptUnderflow->ulUnderflowMonitorMsgQId)
			{
				(void) VK_MSG_Destroy (ptUnderflow->ulUnderflowMonitorMsgQId);
				ptUnderflow->ulUnderflowMonitorMsgQId = 0;
			}
			if (ptUnderflow->ulUnderflowMonitorTaskId)
			{
				(void)VK_TASK_Destroy(ptUnderflow->ulUnderflowMonitorTaskId);
				ptUnderflow->ulUnderflowMonitorTaskId = 0;
			}
			VK_MEM_Free(ptUnderflow);
		}
	}
	else
	{
		(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));
		stMsg.unMsg = DI_IPTUNER_UnderflowMonitor_Msg_Start_Task;
		if(VK_MSG_Send(ptUnderflow->ulUnderflowMonitorMsgQId, (HUINT32 *)&stMsg, sizeof(DI_IPTUNER_Msg_t)) != VK_OK)
		{
			PrintError ("[%s:%d] VK_MSG_Send ERROR!! Something wrong!!\n",__FUNCTION__,__LINE__);
			nVKRet = ERR_FAIL;
			goto DI_IPTUNER_UnderflowMonitorInitThread_Exit;
		}
		((DI_IPTUNER_t *)(ptHandle->pTunerHandle))->ptUnderflow = ptUnderflow;

		P_IPTUNER_PrintUnderflowInfo(ptUnderflow);
	}

	PrintExit;
	return nVKRet;
}

void DI_IPTUNER_UnderflowMonitorTerminateThread(VFIO_Handle_t *ptHandle)
{
	DI_IPTUNER_Msg_t stMsg;
	DI_IPTUNER_UNDERFLOW_t *ptUnderflow = NULL;
	HUINT32	unMaxWaitingCount = 100;

	PrintEnter;

	if(ptHandle == NULL)
	{
		PrintError ("[%s:%d] ptHandle is NULL!!\n",__FUNCTION__,__LINE__);
		goto DI_IPTUNER_UnderflowMonitorTerminateThread_Exit;
	}

	ptUnderflow = ((DI_IPTUNER_t *)(ptHandle->pTunerHandle))->ptUnderflow;

	if(ptUnderflow == NULL)
	{
		PrintError ("[%s:%d] ptUnderflow is NULL!!\n",__FUNCTION__,__LINE__);
		goto DI_IPTUNER_UnderflowMonitorTerminateThread_Exit;
	}

	P_IPTUNER_PrintUnderflowInfo(ptUnderflow);

	if(ptUnderflow->tUnderflowInfo.eUnderflowMonitorStatus > DI_IPTUNER_UnderflowMonitorStatus_Init)
	{
		PrintDebug ("[%s:%d] eUnderflowMonitorStatus = %d\n",__FUNCTION__,__LINE__, ptUnderflow->tUnderflowInfo.eUnderflowMonitorStatus);
		(void)VK_memset(&stMsg, 0x00, sizeof(DI_IPTUNER_Msg_t));
		stMsg.unMsg = DI_IPTUNER_UnderflowMonitor_Msg_Stop_Task;
		if(VK_MSG_Send(ptUnderflow->ulUnderflowMonitorMsgQId, (HUINT32 *)&stMsg, sizeof(DI_IPTUNER_Msg_t)) != VK_OK)
		{
			PrintError ("[%s:%d] VK_MSG_Send ERROR!! Terminating Decoder Buffer Monitoring Thread...\n",__FUNCTION__,__LINE__);
		}

		while(unMaxWaitingCount && ptUnderflow->tUnderflowInfo.eUnderflowMonitorStatus != DI_IPTUNER_UnderflowMonitorStatus_Stop)
		{
			// 최대 10초 대기...
			unMaxWaitingCount--;
			if(!(unMaxWaitingCount % 10))
			{
				if(VK_MSG_Send(ptUnderflow->ulUnderflowMonitorMsgQId, (HUINT32 *)&stMsg, sizeof(DI_IPTUNER_Msg_t)) != VK_OK)
				{
					PrintError ("[%s:%d] VK_MSG_Send ERROR!! Terminating Decoder Buffer Monitoring Thread...\n",__FUNCTION__,__LINE__);
				}
			}
			VK_TASK_Sleep(100);
		}

		if(!unMaxWaitingCount)
		{
			PrintError ("[%s:%d] Underflow Monitoring Task does not Terminated!!\n",__FUNCTION__,__LINE__);
		}

		VK_TASK_Destroy(ptUnderflow->ulUnderflowMonitorTaskId);
		ptUnderflow->ulUnderflowMonitorTaskId = 0;

		(void) VK_MSG_Destroy(ptUnderflow->ulUnderflowMonitorMsgQId);
		ptUnderflow->ulUnderflowMonitorMsgQId = 0;

		VK_MEM_Free(ptUnderflow);
	}
	else
	{
		PrintError ("[%s:%d] DI_IPTUNER_UnderflowMonitor is not started\n", __FUNCTION__, __LINE__);
	}

DI_IPTUNER_UnderflowMonitorTerminateThread_Exit:
	PrintExit;
	return;
}

DI_ERR_CODE DI_IPTUNER_GetUnderflowInfo(VFIO_Handle_t *ptHandle, DI_IPTUNER_UNDERFLOW_INFO_t *ptUnderflowInfo)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	DI_IPTUNER_UNDERFLOW_t *ptUnderflow = NULL;

	PrintEnter;

	if(ptHandle == NULL)
	{
		PrintError ("[%s:%d] ptHandle is NULL!!\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		goto DI_IPTUNER_GetUnderflowInfo_Exit;
	}

	ptUnderflow = ((DI_IPTUNER_t *)(ptHandle->pTunerHandle))->ptUnderflow;

	if(ptUnderflow == NULL)
	{
		PrintError ("[%s:%d] ptUnderflow is NULL!!\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		goto DI_IPTUNER_GetUnderflowInfo_Exit;
	}

	ptUnderflowInfo = &(ptUnderflow->tUnderflowInfo);

DI_IPTUNER_GetUnderflowInfo_Exit:
	if(nRet != DI_ERR_ERROR)
	{
		ptUnderflowInfo = NULL;
	}

	PrintExit;
	return nRet;
}

DI_ERR_CODE DI_IPTUNER_SetUnderflowInfo(VFIO_Handle_t *ptHandle, DI_IPTUNER_UNDERFLOW_INFO_t *ptUnderflowInfo)
{
	DI_ERR_CODE nRet = DI_ERR_OK;
	DI_IPTUNER_UNDERFLOW_t *ptUnderflow = NULL;

	PrintEnter;

	if(ptHandle == NULL)
	{
		PrintError ("[%s:%d] ptHandle is NULL!!\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		goto DI_IPTUNER_SetUnderflowInfo_Exit;
	}

	ptUnderflow = ((DI_IPTUNER_t *)(ptHandle->pTunerHandle))->ptUnderflow;

	if(ptUnderflow == NULL)
	{
		PrintError ("[%s:%d] ptUnderflow is NULL!!\n",__FUNCTION__,__LINE__);
		nRet = DI_ERR_ERROR;
		goto DI_IPTUNER_SetUnderflowInfo_Exit;
	}

	if(ptUnderflow->tUnderflowInfo.unMaxBufferSize != ptUnderflowInfo->unMaxBufferSize)
	{
		ptUnderflow->tUnderflowInfo.unMaxBufferSize = ptUnderflowInfo->unMaxBufferSize;
		// 변경된 정보로 threshold 다시 계산
		ptUnderflow->pHTuner->eUnderflowState = DI_IPTUNER_UNDERFLOW_CANPLAY;
	}
	if(ptUnderflow->tUnderflowInfo.unUnderflowThreshold != ptUnderflowInfo->unUnderflowThreshold)
	{
		ptUnderflow->tUnderflowInfo.unUnderflowThreshold = ptUnderflowInfo->unUnderflowThreshold;
	}
	if(ptUnderflow->tUnderflowInfo.unEnoughThreshold != ptUnderflowInfo->unEnoughThreshold)
	{
		ptUnderflow->tUnderflowInfo.unEnoughThreshold = ptUnderflowInfo->unEnoughThreshold;
	}
	if(ptUnderflow->tUnderflowInfo.ulBufferingTimeout != ptUnderflowInfo->ulBufferingTimeout)
	{
		ptUnderflow->tUnderflowInfo.ulBufferingTimeout = ptUnderflowInfo->ulBufferingTimeout;
	}
	if(ptUnderflow->tUnderflowInfo.eThresholdMode != ptUnderflowInfo->eThresholdMode)
	{
		ptUnderflow->tUnderflowInfo.eThresholdMode = ptUnderflowInfo->eThresholdMode;
	}

DI_IPTUNER_SetUnderflowInfo_Exit:
	PrintExit;
	return nRet;
}

