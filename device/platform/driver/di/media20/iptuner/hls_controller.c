/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   hls_controller.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		Yang Hyun Uk $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2011.07.22
 * File Description:	HTTP Live Streaming
 * Module:
 * Remarks:
 */

/**
 * @defgroup Controller
 * @brief HLS_Controller 이며 HLS 운용에 필요한 기능을 총괄한다. <br> M3U8_Parser에 의해 구조화된 데이터를 사용한다.
 */

/**
 * @author Hyunuk Yang(huyang@humaxdigital.com), YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 22 July 2011
 */

/**
 * @brief HLS_Controller 이며 HLS 운용에 필요한 기능을 총괄한다. <br> M3U8_Parser에 의해 구조화된 데이터를 사용한다.
 */

/**
 * @note
 * Copyright (C) 2011 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file hls_controller.c
 */

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* Start Including Header Files */
#include "di_iptuner_config.h"
#include "di_iptuner_common.h"
#include "di_uart.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */
#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"

#include "stream_global.h"
#include "hls_m3u8_parser.h"
#include "hls_controller.h"

//#include "http_live_streaming.h"
/* End Including Headers */


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
//#define HLS_CONTROLLER_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef HLS_CONTROLLER_DEBUG
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

/** HLS 에서 사용되는 총 PLAY QUEUE 의 개수 */
#define HLS_TOTAL_NUMBER_OF_PLAY_QUEUE	6
/** Download Task 에게 보내는 MSG QUEUE 의 개수 */
#define HLS_DOWNLOADER_MSG_QUEUE_COUNT	10
/** DOWNLOAD TASK PRIORITY */
#define HLS_DOWNLOAD_TASK_PRIORITY 	(VK_TASK_PRIORITY_DI_REF + 1*3)
/** DOWNLOAD TASK Stack 사이즈 */
#define HLS_DOWNLOAD_TASK_STACK_SIZE 	(256*1024)


/* End #define */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/* Start typedef */


/**
 * HLS 운용을 위한 Controller
 */
typedef struct HLS_CONTROLLER_s
{
	m3u8_t *m3u8; /**< M3U8 Handle */
	HUINT32 unProgram; /**< 현재의 Program Sequencial Number */
	HUINT32 unStream; /**< 현재의 Stream Sequencial Number */
	HUINT64 ullSegment;  /**< 현재의 Segment Sequencial Number (based on sequence number) */

	m3u8Node_t *pCurProg; /**< 현재의 Program Data Node */
	m3u8Node_t *pCurStream;	/**< 현재의 Stream Data Node */
	m3u8Node_t *pCurSegment; /**< 현재의 Segment Data Node */

	/** Heuristic */
	SSTR_HEURISTICS_t *hHeuristics;
	/** BufferManager */
	SSTR_BUFFERMGR_t *hBufferMgr;

	/** for HLS Update Event Nofity */
	void *hHLS;
	HLS_CONTROLLER_RequestUpdateCallback cbRequestUpdate;

} HLS_CONTROLLER_t;

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
// @cond STATIC_HIDING
static inline HLS_CONTROLLER_t* __controller(HLS_CTRL hCtrl) { return (HLS_CONTROLLER_t*)hCtrl; }
// @endcond

#if 0
static void HLS_DownloadTask (void *pParam);
static HLS_ERROR_e HLS_SetHeuristicMode(HUINT8 on, HUINT32 min_bitrates, HUINT32 max_bitrates);
HLS_PlayQueue_t * HLS_CreatePlayQueue(HUINT8 numberQcount);
HLS_ERROR_e HLS_DestroyPlayQueue(HLS_PlayQueue_t * playqueue);
HLS_ERROR_e HLS_SetQueueDMZ(HLS_PlayQueue_t * playqueue);
HLS_ERROR_e HLS_UnSetQueueDMZ(void);
unsigned int HLS_GetHeuristicBitrates(void);
HLS_ERROR_e HLS_SetDownloadBitratesToHeuristic(HUINT32 Bitrates);
#endif

HLS_CTRL HLS_CONTROLLER_Open(m3u8_t *m3u8)
{
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = (HLS_CONTROLLER_t*)DD_MEM_Alloc(sizeof(HLS_CONTROLLER_t));
	if(pController == NULL)
	{
		PrintError ("%s(%d) HLS Controller Alloc Fail. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}

	VK_memset(pController,0x00,sizeof(HLS_CONTROLLER_t));
	pController->m3u8 = m3u8;
	if(HLS_CONTROLLER_Setup(pController,0,0,0) != HLS_OK)
	{
		if(pController != NULL)
		{
			DD_MEM_Free(pController);
		}
		PrintExit;
		return NULL;
	}
	pController->hHeuristics = (SSTR_HEURISTICS_t*)DD_MEM_Alloc(sizeof(SSTR_HEURISTICS_t));
	if(pController->hHeuristics == NULL)
	{
		if(pController != NULL)
		{
			DD_MEM_Free(pController);
		}
		return NULL;
	}
	VK_MEM_Memset(pController->hHeuristics,0x00,sizeof(SSTR_HEURISTICS_t));
	pController->hHeuristics->eState = SSTR_HEUR_STATE_TIMID;
//	pController->hHeuristics->nCursor = 0;
//	pController->hHeuristics->nSize = 0;
//	pController->hHeuristics->ullAvgBandwidth = 0;
//	pController->hHeuristics->ullPairBandwidth = 0;
	PrintExit;
	return pController;
}

HLS_ERROR_e HLS_CONTROLLER_Close(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Controller is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	if(pController == NULL)
	{
		PrintError ("%s(%d) HLS Controller is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}

#if 0
	if(pController->m3u8 != NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is not NULL. MUST destroy m3u8. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
#endif
	if(pController->hHeuristics != NULL)
	{
		VK_MEM_Free(pController->hHeuristics);
	}
	if(pController != NULL)
	{
		DD_MEM_Free(pController);
	}
	PrintExit;
	return HLS_OK;

#if 0
	/* DEBUG */
	if ( s_tTop_M3u8 != NULL )
	{
		(void)HLS_SetHeuristicMode(TRUE, 0, 0);

		bandwidth = HLS_GetHeuristicBitrates();

		/* topm3u8 , bandwidth 는 Heuristic 으로 부터, segment number 는 Controller 로부터 결국 Segment 의 URL 을 얻어 온다.*/
		url = UTIL_GetUrlFromM3U8(s_tTop_M3u8, bandwidth, 0);

		return url;
		//PrintTopM3U8(s_tTop_M3u8);
	}
	else
	{
		return NULL;
	}
#endif

}

HLS_ERROR_e HLS_CONTROLLER_Setup(HLS_CTRL hCtrl, HUINT32 unProgram, HUINT32 unStream, HUINT32 unSegment)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pProgNode = NULL;
	m3u8Node_t* pStreamNode = NULL;
	m3u8Node_t* pSegNode = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pProgNode = UTIL_DLLIST_Get_n_Node(&m3u8->programSet,unProgram);
	if(pProgNode == NULL)
	{
		PrintError ("%s(%d) Program(%u) is INVALID \n",__FUNCTION__,__LINE__,unProgram);
		PrintExit;
		return HLS_FAIL;
	}
	pProgram = (m3u8Program_t*)pProgNode->pData;
	if(pProgram == NULL)
	{
		PrintError ("%s(%d) Program Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pStreamNode = UTIL_DLLIST_Get_n_Node(&pProgram->streamSet,unStream);
	if(pStreamNode == NULL)
	{
		PrintError ("%s(%d) Stream(%u) is INVALID \n",__FUNCTION__,__LINE__,unStream);
		PrintExit;
		return HLS_FAIL;
	}
	pStream = (m3u8Stream_t*)pStreamNode->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pSegNode = UTIL_DLLIST_Get_n_Node(&pStream->segmentSet,unSegment);
	if(pSegNode == NULL)
	{
		PrintError ("%s(%d) Segment(%u) is INVALID \n",__FUNCTION__,__LINE__,unSegment);
		PrintExit;
		return HLS_FAIL;
	}
	pSegment = (m3u8Segment_t*)pSegNode->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController->pCurProg = pProgNode;
	pController->pCurStream = pStreamNode;
	pController->pCurSegment = pSegNode;
	pController->unProgram = unProgram;
	pController->unStream = unStream;
	pController->ullSegment = pSegment->ullNum;
	PrintExit;
	return HLS_OK;
}

HLS_ERROR_e	HLS_CONTROLLER_OpenBuffer(HLS_CTRL hCtrl, HUINT64 ullBufferSize, HINT32 nBufferType )
{
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	if(pController->hBufferMgr != NULL)
	{
		PrintError("%s(%d) Err> hBufferMgr is exist\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController->hBufferMgr = SSTR_BUFFERMGR_Open (ullBufferSize, nBufferType);
	if(pController->hBufferMgr == NULL)
	{
		PrintError("%s(%d) Err> hBufferMgr Open Fail\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	return HLS_OK;
}

HLS_ERROR_e	HLS_CONTROLLER_CloseBuffer(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	(void)SSTR_BUFFERMGR_Close (pController->hBufferMgr);
	if(pController->hBufferMgr != NULL)
	{
		VK_MEM_Free(pController->hBufferMgr);
		pController->hBufferMgr = NULL;
	}
	return HLS_OK;
}

HLS_ERROR_e	HLS_CONTROLLER_SetSegment(HLS_CTRL hCtrl, HUINT64 ullSegment)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pSegNode = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;

		if( pSegment->ullNum == ullSegment)
		{
			pController->ullSegment = pSegment->ullNum;
			pController->pCurSegment = pSegNode;
			PrintExit;
			return HLS_OK;
		}
                pSegNode = UTIL_DLLIST_NextNode(pSegNode);
	}
	PrintError("%s(%d) Err> SegmentId(%"PRId64") is INVALID\n",__FUNCTION__,__LINE__,ullSegment);
//	P_SSTR_CONTROLLER_Unlock(hSstr);
	PrintExit;
	return HLS_FAIL;
}

HLS_ERROR_e	HLS_CONTROLLER_SetSegmentUntilDiscontinued(HLS_CTRL hCtrl, HUINT64 ullSegment)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pSegNode = NULL;
	m3u8Node_t* pTempSegNode = NULL;
	HUINT64 ullTempSegNum = 0;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		if(pSegment != NULL)
		{
			if( pSegment->ullNum == ullSegment)
			{
				pController->ullSegment = pSegment->ullNum;
				pController->pCurSegment = pSegNode;
				PrintExit;
				return HLS_OK;
			}
			else if( pSegment->bDiscontinuity == TRUE)
			{
				if(pTempSegNode != NULL)
				{
					pController->pCurSegment = pTempSegNode;
					pController->ullSegment = ullTempSegNum;
				}
				else
				{
					pController->ullSegment = pSegment->ullNum;
					pController->pCurSegment = pSegNode;
				}
				PrintError("%s(%d) Stream has dicontinuity (Segment Changed %llu -> %llu)\n",__FUNCTION__,__LINE__,ullSegment,pController->ullSegment);
				PrintExit;
				return HLS_OK;
			}
			pTempSegNode = pSegNode;
			ullTempSegNum = pSegment->ullNum;
		}
		pSegNode = UTIL_DLLIST_NextNode(pSegNode);
	}
	PrintError("%s(%d) Err> SegmentId(%"PRId64") is INVALID\n",__FUNCTION__,__LINE__,ullSegment);
//	P_SSTR_CONTROLLER_Unlock(hSstr);
	PrintExit;
	return HLS_FAIL;
}



HLS_ERROR_e HLS_CONTROLLER_NextSegment(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
//	m3u8Node_t* pProgNode = NULL;
//	m3u8Node_t* pStreamNode = NULL;
	m3u8Node_t* pSegNode = NULL;
//	m3u8Program_t *pProgram = NULL;
//	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pSegNode = UTIL_DLLIST_NextNode(pController->pCurSegment);
	if(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		if(pSegment == NULL)
		{
			PrintError ("%s(%d) Segment Data is NULL \n",__FUNCTION__,__LINE__);
			PrintExit;
			return HLS_FAIL;
		}
//		pController->pCurStream = pStreamNode;
		pController->pCurSegment = pSegNode;
		pController->ullSegment = pSegment->ullNum;
	}
	else
	{
		PrintDebug ("%s(%d) NextSegment is NULL (EOS)\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	PrintExit;
	return HLS_OK;
}

HLS_ERROR_e HLS_CONTROLLER_PrevSegment(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
//	m3u8Node_t* pProgNode = NULL;
//	m3u8Node_t* pStreamNode = NULL;
	m3u8Node_t* pSegNode = NULL;
//	m3u8Program_t *pProgram = NULL;
//	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pSegNode = UTIL_DLLIST_PrevNode(pController->pCurSegment);
	if(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		if(pSegment == NULL)
		{
			PrintError ("%s(%d) Segment Data is NULL \n",__FUNCTION__,__LINE__);
			PrintExit;
			return HLS_FAIL;
		}
//		pController->pCurStream = pStreamNode;
		pController->pCurSegment = pSegNode;
		pController->ullSegment = pSegment->ullNum;
	}
	else
	{
		PrintError ("%s(%d) PrevSegment is NULL (BOS)\n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	PrintExit;
	return HLS_OK;
}

HFLOAT32 HLS_CONTROLLER_GetTimeGapBetweenCurAndLast(HLS_CTRL hCtrl)
{
	m3u8Segment_t* pSegment = NULL;
	m3u8Node_t* pSegNode = NULL;
	HFLOAT32 flTimeGap = 0;
	HFLOAT32 flCurTime = 0;
	HFLOAT32 flLastTime = 0;
	HLS_CONTROLLER_t *pController = NULL;
	m3u8_t *m3u8 = NULL;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) Controller is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegNode = pController->pCurSegment;
	if(pSegNode != NULL && pSegNode->pData != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		flCurTime = pSegment->starttime_of_segment;
		while(pSegNode != NULL)
		{
			pSegment = (m3u8Segment_t*)pSegNode->pData;
			if(pSegment != NULL)
			{
				if( pSegment->bDiscontinuity == TRUE)
				{
					PrintError("%s(%d) Stream has dicontinuity\n",__FUNCTION__,__LINE__);
					break;
				}
				flLastTime = pSegment->starttime_of_segment;
			}
			pSegNode = UTIL_DLLIST_NextNode(pSegNode);
		}
		if( flLastTime > flCurTime )
		{
			flTimeGap = flLastTime - flCurTime;
			PrintDebug("%s(%d): CheckTime gap(%f)\n", __FUNCTION__, __LINE__, flTimeGap);
			PrintExit;
			return flTimeGap;
		}
		else
		{
			PrintDebug("%s(%d): CheckTime gap(%f)\n", __FUNCTION__, __LINE__, flTimeGap);
			PrintExit;
			return 0;
		}
	}
	PrintDebug("%s(%d): CheckTime gap(%f)\n", __FUNCTION__, __LINE__, flTimeGap);
	PrintExit;
	return 0;
}

HBOOL HLS_CONTROLLER_GearUp(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
//	m3u8Node_t* pProgNode = NULL;
	m3u8Node_t* pStreamNode = NULL;
	m3u8Node_t* pSegNode = NULL;
//	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pStreamNode = UTIL_DLLIST_NextNode(pController->pCurStream);
	/** TODO:  같은 Bandwidth Stream Skipping 처리 */
	if(pStreamNode != NULL)
	{
		pStream = (m3u8Stream_t*)pStreamNode->pData;
		if(pStream == NULL)
		{
			PrintError ("%s(%d) Higher Stream is NULL \n",__FUNCTION__,__LINE__);
			PrintExit;
			return FALSE;
		}
#if 0
		if( pController->ullSegment >= pStream->ullSequenceNum
		&& pController->ullSegment <= (pStream->ullSequenceNum + UTIL_DLLIST_Size(&pStream->segmentSet)))
		{
			pSegNode = UTIL_DLLIST_Get_n_Node(&pStream->segmentSet,pController->ullSegment - pStream->ullSequenceNum);
			if(pSegNode == NULL)
			{
				PrintError ("%s(%d) [DANGER!!] Illegal Case : Segment(%"PRId64") is INVALID \n",__FUNCTION__,__LINE__,pController->ullSegment);
				PrintExit;
				return FALSE;
			}
			pSegment = (m3u8Segment_t*)pSegNode->pData;
			if(pSegment == NULL)
			{
				PrintError ("%s(%d) [DANGER!!] Illegal Case : Segment Data is NULL \n",__FUNCTION__,__LINE__);
				PrintExit;
				return FALSE;
			}
			if(pSegment->ullNum != pController->ullSegment)
			{
				PrintError ("%s(%d) [DANGER!!] Illegal Case : \n",__FUNCTION__,__LINE__);
				PrintExit;
				return FALSE;
			}

		}
		else
		{
			PrintError ("%s(%d) Segment(%"PRId64") is not exist \n",__FUNCTION__,__LINE__,pController->ullSegment);
			if(pController->cbRequestUpdate != NULL)
			{
				pController->cbRequestUpdate(pController->hHLS, pController->unProgram ,pController->unStream + 1, 0, TRUE);
			}
			PrintExit;
			return FALSE;
		}
		pController->pCurStream = pStreamNode;
		pController->pCurSegment = pSegNode;
		pController->unStream++;
#else
		HBOOL bChanged = FALSE;
		if( pController->ullSegment >= pStream->ullSequenceNum )
		{
			pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
			while(pSegNode != NULL)
			{
				if(pSegNode == NULL)
				{
					PrintError("%s(%d) [DANGER!!] Illegal Case : Segment(%"PRId64") is INVALID \n",__FUNCTION__,__LINE__,pController->ullSegment);
					PrintExit;
					return FALSE;
				}
				pSegment = (m3u8Segment_t*)pSegNode->pData;
				if(pSegment == NULL)
				{
					PrintError("%s(%d) [DANGER!!] Illegal Case : Segment Data is NULL \n",__FUNCTION__,__LINE__);
					PrintExit;
					return FALSE;
				}
				if(pSegment->ullNum == pController->ullSegment)
				{
					bChanged = TRUE;
					break;
				}
				pSegNode = UTIL_DLLIST_NextNode(pSegNode);
			}
		}
		if(bChanged)
		{
			pController->pCurStream = pStreamNode;
			pController->pCurSegment = pSegNode;
			pController->unStream++;
		}
		else
		{
			PrintError("%s(%d) Segment(%"PRId64") is not exist \n",__FUNCTION__,__LINE__,pController->ullSegment);
			if(pController->cbRequestUpdate != NULL)
			{
				pController->cbRequestUpdate(pController->hHLS, pController->unProgram ,pController->unStream + 1, 0, TRUE);
			}
			PrintExit;
			return FALSE;
		}
#endif
	}
	PrintExit;
	return TRUE;
}

HBOOL HLS_CONTROLLER_GearDown(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
//	m3u8Node_t* pProgNode = NULL;
	m3u8Node_t* pStreamNode = NULL;
	m3u8Node_t* pSegNode = NULL;
//	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pStreamNode = UTIL_DLLIST_PrevNode(pController->pCurStream);
	/** TODO:  같은 Bandwidth Stream Skipping 처리 */
	if(pStreamNode != NULL)
	{
		pStream = (m3u8Stream_t*)pStreamNode->pData;
		if(pStream == NULL)
		{
			PrintError ("%s(%d) Lower Stream is NULL \n",__FUNCTION__,__LINE__);
			PrintExit;
			return FALSE;
		}
#if 0
		if( pController->ullSegment >= pStream->ullSequenceNum
		&& pController->ullSegment <= (pStream->ullSequenceNum + UTIL_DLLIST_Size(&pStream->segmentSet)))
		{
			pSegNode = UTIL_DLLIST_Get_n_Node(&pStream->segmentSet,pController->ullSegment - pStream->ullSequenceNum);
			if(pSegNode == NULL)
			{
				PrintError ("%s(%d) [DANGER!!] Illegal Case : Segment(%"PRId64") is INVALID \n",__FUNCTION__,__LINE__,pController->ullSegment);
				PrintExit;
				return FALSE;
			}
			pSegment = (m3u8Segment_t*)pSegNode->pData;
			if(pSegment == NULL)
			{
				PrintError ("%s(%d) [DANGER!!] Illegal Case : Segment Data is NULL \n",__FUNCTION__,__LINE__);
				PrintExit;
				return FALSE;
			}
			if(pSegment->ullNum != pController->ullSegment)
			{
				PrintError ("%s(%d) [DANGER!!] Illegal Case : \n",__FUNCTION__,__LINE__);
				PrintExit;
				return FALSE;
			}

		}
		else
		{
			PrintError ("%s(%d) Segment(%"PRId64") is not exist \n",__FUNCTION__,__LINE__,pController->ullSegment);
			if(pController->cbRequestUpdate != NULL)
			{
				pController->cbRequestUpdate(pController->hHLS, pController->unProgram ,pController->unStream - 1, 0, TRUE);
			}
			PrintExit;
			return FALSE;
		}
		pController->pCurStream = pStreamNode;
		pController->pCurSegment = pSegNode;
		pController->unStream--;
#else
		HBOOL bChanged = FALSE;
		if( pController->ullSegment >= pStream->ullSequenceNum )
		{
			pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
			while(pSegNode != NULL)
			{
				if(pSegNode == NULL)
				{
					PrintError("%s(%d) [DANGER!!] Illegal Case : Segment(%"PRId64") is INVALID \n",__FUNCTION__,__LINE__,pController->ullSegment);
					PrintExit;
					return FALSE;
				}
				pSegment = (m3u8Segment_t*)pSegNode->pData;
				if(pSegment == NULL)
				{
					PrintError("%s(%d) [DANGER!!] Illegal Case : Segment Data is NULL \n",__FUNCTION__,__LINE__);
					PrintExit;
					return FALSE;
				}
				if(pSegment->ullNum == pController->ullSegment)
				{
					bChanged = TRUE;
					break;
				}
				pSegNode = UTIL_DLLIST_NextNode(pSegNode);
			}
		}
		if(bChanged)
		{
			pController->pCurStream = pStreamNode;
			pController->pCurSegment = pSegNode;
			pController->unStream--;
		}
		else
		{
			PrintError("%s(%d) Segment(%"PRId64") is not exist \n",__FUNCTION__,__LINE__,pController->ullSegment);
			if(pController->cbRequestUpdate != NULL)
			{
				pController->cbRequestUpdate(pController->hHLS, pController->unProgram ,pController->unStream - 1, 0, TRUE);
			}
			PrintExit;
			return FALSE;
		}
#endif
	}
	PrintExit;
	return TRUE;
}

HBOOL HLS_CONTROLLER_GearLowest(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
//	m3u8Node_t* pProgNode = NULL;
	m3u8Node_t* pStreamNode = NULL;
	m3u8Node_t* pSegNode = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurProg == NULL)
	{
		PrintError ("%s(%d) Current Program is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pProgram = (m3u8Program_t*)pController->pCurProg->pData;
	if(pProgram == NULL)
	{
		PrintError ("%s(%d) Current Program Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pStreamNode = UTIL_DLLIST_GetHeadNode(&pProgram->streamSet);
	/** TODO:  같은 Bandwidth Stream Skipping 처리 */
	if(pStreamNode != NULL)
	{
		pStream = (m3u8Stream_t*)pStreamNode->pData;
		if(pStream == NULL)
		{
			PrintError ("%s(%d) Lowest Stream is NULL \n",__FUNCTION__,__LINE__);
			PrintExit;
			return FALSE;
		}
#if 0
		if( pController->ullSegment >= pStream->ullSequenceNum
		&& pController->ullSegment <= (pStream->ullSequenceNum + UTIL_DLLIST_Size(&pStream->segmentSet)))
		{
			pSegNode = UTIL_DLLIST_Get_n_Node(&pStream->segmentSet,pController->ullSegment - pStream->ullSequenceNum);
			if(pSegNode == NULL)
			{
				PrintError("%s(%d) [DANGER!!] Illegal Case : Segment (%"PRId64") is INVALID\n",__FUNCTION__,__LINE__,pController->ullSegment);
				PrintExit;
				return FALSE;
			}
			pSegment = (m3u8Segment_t*)pSegNode->pData;
			if(pSegment == NULL)
			{
				PrintError("%s(%d) [DANGER!!] Illegal Case : Segment Data is NULL \n",__FUNCTION__,__LINE__);
				PrintExit;
				return FALSE;
			}
			if(pSegment->ullNum != pController->ullSegment)
			{
				PrintError("%s(%d) [DANGER!!] Illegal Case : \n",__FUNCTION__,__LINE__);
				PrintExit;
				return FALSE;
			}

		}
		else
		{
			PrintError ("%s(%d) Segment(%"PRId64") is not exist \n",__FUNCTION__,__LINE__,pController->ullSegment);
			if(pController->cbRequestUpdate != NULL)
			{
				pController->cbRequestUpdate(pController->hHLS, pController->unProgram, 0, 0, TRUE);
			}
			PrintExit;
			return FALSE;
		}
		pController->pCurStream = pStreamNode;
		pController->pCurSegment = pSegNode;
		pController->unStream = 0;
#else
		HBOOL bChanged = FALSE;
		if( pController->ullSegment >= pStream->ullSequenceNum )
		{
			pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
			while(pSegNode != NULL)
			{
				if(pSegNode == NULL)
				{
					PrintError("%s(%d) [DANGER!!] Illegal Case : Segment(%"PRId64") is INVALID \n",__FUNCTION__,__LINE__,pController->ullSegment);
					PrintExit;
					return FALSE;
				}
				pSegment = (m3u8Segment_t*)pSegNode->pData;
				if(pSegment == NULL)
				{
					PrintError("%s(%d) [DANGER!!] Illegal Case : Segment Data is NULL \n",__FUNCTION__,__LINE__);
					PrintExit;
					return FALSE;
				}
				if(pSegment->ullNum == pController->ullSegment)
				{
					bChanged = TRUE;
					break;
				}
				pSegNode = UTIL_DLLIST_NextNode(pSegNode);
			}
		}
		if(bChanged)
		{
			pController->pCurStream = pStreamNode;
			pController->pCurSegment = pSegNode;
			pController->unStream = 0;
		}
		else
		{
			PrintError("%s(%d) Segment(%"PRId64") is not exist \n",__FUNCTION__,__LINE__,pController->ullSegment);
			if(pController->cbRequestUpdate != NULL)
			{
				pController->cbRequestUpdate(pController->hHLS, pController->unProgram, 0, 0, TRUE);
			}
			PrintExit;
			return FALSE;
		}
#endif
	}
	PrintExit;
	return TRUE;
}

HBOOL HLS_CONTROLLER_GearHighest(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
//	m3u8Node_t* pProgNode = NULL;
	m3u8Node_t* pStreamNode = NULL;
	m3u8Node_t* pSegNode = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurProg == NULL)
	{
		PrintError ("%s(%d) Current Program is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pProgram = (m3u8Program_t*)pController->pCurProg->pData;
	if(pProgram == NULL)
	{
		PrintError ("%s(%d) Current Program Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pStreamNode = UTIL_DLLIST_GetTailNode(&pProgram->streamSet);
	/** TODO:  같은 Bandwidth Stream Skipping 처리 */
	if(pStreamNode != NULL)
	{
		pStream = (m3u8Stream_t*)pStreamNode->pData;
		if(pStream == NULL)
		{
			PrintError ("%s(%d) Highest Stream is NULL \n",__FUNCTION__,__LINE__);
			PrintExit;
			return FALSE;
		}
#if 0
		if( pController->ullSegment >= pStream->ullSequenceNum
		&& pController->ullSegment <= (pStream->ullSequenceNum + UTIL_DLLIST_Size(&pStream->segmentSet)))
		{
			pSegNode = UTIL_DLLIST_Get_n_Node(&pStream->segmentSet,pController->ullSegment - pStream->ullSequenceNum);
			if(pSegNode == NULL)
			{
				PrintError ("%s(%d) [DANGER!!] Illegal Case : Segment(%"PRId64") is INVALID \n",__FUNCTION__,__LINE__,pController->ullSegment);
				PrintExit;
				return FALSE;
			}
			pSegment = (m3u8Segment_t*)pSegNode->pData;
			if(pSegment == NULL)
			{
				PrintError ("%s(%d) [DANGER!!] Illegal Case : Segment Data is NULL \n",__FUNCTION__,__LINE__);
				PrintExit;
				return FALSE;
			}
			if(pSegment->ullNum != pController->ullSegment)
			{
				PrintError ("%s(%d) [DANGER!!] Illegal Case : \n",__FUNCTION__,__LINE__);
				PrintExit;
				return FALSE;
			}

		}
		else
		{
			if(pController->cbRequestUpdate != NULL)
			{
				pController->cbRequestUpdate(pController->hHLS, pController->unProgram ,UTIL_DLLIST_Size(&pProgram->streamSet) - 1, 0,TRUE);
			}
			PrintError ("%s(%d) Segment(%"PRId64") is not exist \n",__FUNCTION__,__LINE__,pController->ullSegment);
			PrintExit;
			return FALSE;
		}
		pController->pCurStream = pStreamNode;
		pController->pCurSegment = pSegNode;
		pController->unStream = UTIL_DLLIST_Size(&pProgram->streamSet) - 1;
#else
		HBOOL bChanged = FALSE;
		if( pController->ullSegment >= pStream->ullSequenceNum )
		{
			pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
			while(pSegNode != NULL)
			{
				if(pSegNode == NULL)
				{
					PrintError("%s(%d) [DANGER!!] Illegal Case : Segment(%"PRId64") is INVALID \n",__FUNCTION__,__LINE__,pController->ullSegment);
					PrintExit;
					return FALSE;
				}
				pSegment = (m3u8Segment_t*)pSegNode->pData;
				if(pSegment == NULL)
				{
					PrintError("%s(%d) [DANGER!!] Illegal Case : Segment Data is NULL \n",__FUNCTION__,__LINE__);
					PrintExit;
					return FALSE;
				}
				if(pSegment->ullNum == pController->ullSegment)
				{
					bChanged = TRUE;
					break;
				}
				pSegNode = UTIL_DLLIST_NextNode(pSegNode);
			}
		}
		if(bChanged)
		{
			pController->pCurStream = pStreamNode;
			pController->pCurSegment = pSegNode;
			pController->unStream = UTIL_DLLIST_Size(&pProgram->streamSet) - 1;
		}
		else
		{
			PrintError("%s(%d) Segment(%"PRId64") is not exist \n",__FUNCTION__,__LINE__,pController->ullSegment);
			if(pController->cbRequestUpdate != NULL)
			{
				pController->cbRequestUpdate(pController->hHLS, pController->unProgram ,UTIL_DLLIST_Size(&pProgram->streamSet) - 1, 0, TRUE);
			}
			PrintExit;
			return FALSE;
		}
#endif
	}
	PrintExit;
	return TRUE;
}


HBOOL HLS_CONTROLLER_SetClosestBandwidth(HLS_CTRL hCtrl, HUINT32 unBandwidth)
{
	HUINT32 unStream = 0;
	HUINT32 unIdx = 0;
	m3u8_t *m3u8 = NULL;
	HUINT32 unLowerBandwidth = 0;
	m3u8Node_t* pStreamNode = NULL;
	m3u8Node_t* pSegNode = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurProg == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pProgram = (m3u8Program_t*)pController->pCurProg->pData;
	if(pProgram == NULL)
	{
		PrintError ("%s(%d) Program Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pStreamNode = UTIL_DLLIST_GetHeadNode(&pProgram->streamSet);
	if(pStreamNode == NULL)
	{
		PrintError ("%s(%d) Head Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	/** TODO:  같은 Bandwidth Stream Skipping 처리 */
	while(pStreamNode != NULL)
	{
		pStream = (m3u8Stream_t*)pStreamNode->pData;
		if(pStream->unBandwidth < unBandwidth && pStream->unBandwidth > unLowerBandwidth)
		{
			unLowerBandwidth = pStream->unBandwidth;
			unStream = unIdx;
		}
		unIdx++;
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
	}
	if(unLowerBandwidth > 0)
	{
		pStreamNode = UTIL_DLLIST_Get_n_Node(&pProgram->streamSet, unStream);
		if(pStreamNode != NULL && pStreamNode->pData != NULL)
		{
			pStream = (m3u8Stream_t*)pStreamNode->pData;
#if 0
			if( pController->ullSegment >= pStream->ullSequenceNum
			&& pController->ullSegment <= (pStream->ullSequenceNum + UTIL_DLLIST_Size(&pStream->segmentSet)))
			{
				pSegNode = UTIL_DLLIST_Get_n_Node(&pStream->segmentSet,pController->ullSegment - pStream->ullSequenceNum);
				if(pSegNode == NULL)
				{
					PrintError ("%s(%d) [DANGER!!] Illegal Case : Segment(%"PRId64") is INVALID \n",__FUNCTION__,__LINE__,pController->ullSegment);
					PrintExit;
					return FALSE;
				}
				pSegment = (m3u8Segment_t*)pSegNode->pData;
				if(pSegment == NULL)
				{
					PrintError ("%s(%d) [DANGER!!] Illegal Case : Segment Data is NULL \n",__FUNCTION__,__LINE__);
					PrintExit;
					return FALSE;
				}
				if(pSegment->ullNum != pController->ullSegment)
				{
					PrintError ("%s(%d) [DANGER!!] Illegal Case : \n",__FUNCTION__,__LINE__);
					PrintExit;
					return FALSE;
				}

			}
			else
			{
				PrintError ("%s(%d) Segment(%"PRId64") is not exist \n",__FUNCTION__,__LINE__,pController->ullSegment);
				if(pController->cbRequestUpdate != NULL)
				{
					pController->cbRequestUpdate(pController->hHLS, pController->unProgram, unStream, 0, TRUE);
				}
				PrintExit;
				return FALSE;
			}
			pController->pCurStream = pStreamNode;
			pController->pCurSegment = pSegNode;
			pController->unStream = unStream;
#else
			HBOOL bChanged = FALSE;
			if( pController->ullSegment >= pStream->ullSequenceNum )
			{
				pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
				while(pSegNode != NULL)
				{
					if(pSegNode == NULL)
					{
						PrintError("%s(%d) [DANGER!!] Illegal Case : Segment(%"PRId64") is INVALID \n",__FUNCTION__,__LINE__,pController->ullSegment);
						PrintExit;
						return FALSE;
					}
					pSegment = (m3u8Segment_t*)pSegNode->pData;
					if(pSegment == NULL)
					{
						PrintError("%s(%d) [DANGER!!] Illegal Case : Segment Data is NULL \n",__FUNCTION__,__LINE__);
						PrintExit;
						return FALSE;
					}
					if(pSegment->ullNum == pController->ullSegment)
					{
						bChanged = TRUE;
						break;
					}
					pSegNode = UTIL_DLLIST_NextNode(pSegNode);
				}
			}
			if(bChanged)
			{
				pController->pCurStream = pStreamNode;
				pController->pCurSegment = pSegNode;
				pController->unStream = unStream;
			}
			else
			{
				PrintError("%s(%d) Segment(%"PRId64") is not exist \n",__FUNCTION__,__LINE__,pController->ullSegment);
				if(pController->cbRequestUpdate != NULL)
				{
					pController->cbRequestUpdate(pController->hHLS, pController->unProgram ,pController->unStream, 0, TRUE);
				}
				PrintExit;
				return FALSE;
			}
#endif
		}
	}
	else
	{
		HLS_CONTROLLER_GearLowest(hCtrl);
	}
	PrintExit;
	return TRUE;
}

HBOOL HLS_CONTROLLER_DiscardSegmentsAndKeysUntilCurrent(HLS_CTRL hCtrl)
{
	m3u8Segment_t *pSegment = NULL;
	m3u8Stream_t *pStream = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}

	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(HLS_M3U8_DiscardSegmentsAndKeysUntilBaseSegment(pStream,pSegment) == M3U8_ERR_OK)
	{
		pStream->ullSequenceNum = pSegment->ullNum;
		PrintExit;
		return TRUE;
	}
	PrintExit;
	return FALSE;
}

HBOOL HLS_CONTROLLER_DiscardCurHighestStream(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pStreamNode = NULL;
	m3u8Program_t *pProgram = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurProg == NULL)
	{
		PrintError ("%s(%d) Current Program is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pProgram = (m3u8Program_t*)pController->pCurProg->pData;
	pStreamNode = UTIL_DLLIST_ExtractTail(&pProgram->streamSet);
	if(pStreamNode == NULL)
	{
		PrintError ("%s(%d) Highest Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(HLS_M3U8_DiscardStream(pStreamNode) != M3U8_ERR_OK)
	{
		PrintError ("%s(%d) Delete Stream Fail \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	PrintExit;
	return TRUE;
}

SSTR_BUFFERMGR_t* HLS_CONTROLLER_GetBufferMgr(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
//	m3u8Program_t *pProgram = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	if(pController->hBufferMgr == NULL)
	{
		PrintError ("%s(%d) BufferMgr is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
//	PrintExit;
	return pController->hBufferMgr;
}

SSTR_HEURISTICS_t* HLS_CONTROLLER_GetHeuristics(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
//	m3u8Program_t *pProgram = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	if(pController->hHeuristics == NULL)
	{
		PrintError ("%s(%d) Heuristics is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pController->hHeuristics;
}


m3u8_t* HLS_CONTROLLER_GetM3U8Handle(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
//	m3u8Program_t *pProgram = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pController->m3u8;
}

HBOOL HLS_CONTROLLER_IsCurStreamEncrypted(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
//	m3u8Node_t* pStreamNode = NULL;
	m3u8Stream_t *pStream = NULL;
	HLS_CONTROLLER_t *pController = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
//	PrintExit;
	return pStream->bEncrypted;
}

HBOOL HLS_CONTROLLER_IsCurSegmentDiscontinuity(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	PrintExit;
	return pSegment->bDiscontinuity;
}

HBOOL HLS_CONTROLLER_IsCurSegmentEndList(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return FALSE;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return FALSE;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return FALSE;
	}
//	PrintExit;
	return pSegment->bLastSegment;
}

HBOOL HLS_CONTROLLER_IsCurStreamEndList(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Stream_t *pStream = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return FALSE;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL. \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return FALSE;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream  == NULL)
	{
		PrintError ("%s(%d) Stream Data is NULL. \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return FALSE;
	}
//	PrintExit;
	return pStream->bEndList;
}


HLONG HLS_CONTROLLER_GetCurSegmentDateTime(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	PrintExit;
	return pSegment->datetime;
}

HINT8* HLS_CONTROLLER_GetCurKeyUrl(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	if(pSegment->pKey == NULL)
	{
		PrintError ("%s(%d) Key is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pSegment->pKey->key_uri;
}

HUINT8* HLS_CONTROLLER_GetCurIV(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	if(pSegment->pKey == NULL)
	{
		PrintError ("%s(%d) Key is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pSegment->pKey->pIV;
}

m3u8Method_e HLS_CONTROLLER_GetCurKeyMethod(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_METHOD_UNKNOWN;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_METHOD_UNKNOWN;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_METHOD_UNKNOWN;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_METHOD_UNKNOWN;
	}
	if(pSegment->pKey == NULL)
	{
		PrintError ("%s(%d) Key is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_METHOD_UNKNOWN;
	}
	PrintExit;
	return pSegment->pKey->eMethod;
}

m3u8EncPadding_e HLS_CONTROLLER_GetCurPaddingMethod(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_METHOD_UNKNOWN;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_METHOD_UNKNOWN;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_METHOD_UNKNOWN;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_METHOD_UNKNOWN;
	}
	if(pSegment->pKey == NULL)
	{
		PrintError ("%s(%d) Key is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return M3U8_METHOD_UNKNOWN;
	}
	PrintExit;
	return pSegment->pKey->eEncPadding;
}


HINT8* HLS_CONTROLLER_GetCurSegmentUrl(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return NULL;
	}
	PrintExit;
	return pSegment->segment_url;
}

HLS_ERROR_e HLS_CONTROLLER_GetCurProgramIndex(HLS_CTRL hCtrl, HUINT32 *unNum)
{
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	*unNum = pController->unProgram;
	PrintExit;
	return HLS_OK;
}

HLS_ERROR_e HLS_CONTROLLER_GetCurStreamIndex(HLS_CTRL hCtrl, HUINT32 *unNum)
{
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	*unNum = pController->unStream;
	PrintExit;
	return HLS_OK;
}

HLS_ERROR_e HLS_CONTROLLER_GetCurSegmentId(HLS_CTRL hCtrl, HUINT64 *ullNum)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	*ullNum = pSegment->ullNum;
	PrintExit;
	return HLS_OK;
}

HUINT32 HLS_CONTROLLER_GetCurSegmentDuration(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
//	PrintExit;
	return pSegment->duration_of_segment;
}

HLS_ERROR_e HLS_CONTROLLER_GetCurUpdateDelay(HLS_CTRL hCtrl , HUINT32 *unDelay)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pSegmentNode = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return HLS_FAIL;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return HLS_FAIL;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return HLS_FAIL;
	}
	pSegmentNode = UTIL_DLLIST_GetTailNode(&pStream->segmentSet);
	if(pSegmentNode == NULL || pSegmentNode->pData == NULL)
	{
		PrintError ("%s(%d) Last Segment is NULL \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return HLS_FAIL;
	}
	pSegment = (m3u8Segment_t*)pSegmentNode->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
//		PrintExit;
		return HLS_FAIL;
	}
//	PrintExit;
	*unDelay = pSegment->duration_of_segment;
	return HLS_OK;
}


HUINT32 HLS_CONTROLLER_GetCurSegmentStartTime(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	PrintExit;
	return pSegment->starttime_of_segment;
}

HFLOAT32 HLS_CONTROLLER_GetCurStreamStartTime(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pSegmentNode = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegmentNode = UTIL_DLLIST_GetHeadNode( &pStream->segmentSet );
	if(pSegmentNode == NULL)
	{
		PrintError ("%s(%d) Head Segment is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pSegmentNode->pData == NULL)
	{
		PrintError ("%s(%d) Head Segment Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegment = (m3u8Segment_t*)pSegmentNode->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Head Segment is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	PrintExit;
	return pSegment->starttime_of_segment;
}

HFLOAT32 HLS_CONTROLLER_GetCurStreamEndTime(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pSegmentNode = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegmentNode = UTIL_DLLIST_GetTailNode( &pStream->segmentSet );
	if(pSegmentNode == NULL)
	{
		PrintError ("%s(%d) Tail Segment is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pSegmentNode->pData == NULL)
	{
		PrintError ("%s(%d) Tail Segment Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegment = (m3u8Segment_t*)pSegmentNode->pData;
	if(pSegment == NULL)
	{
		PrintError ("%s(%d) Tail Segment is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	PrintError ("%s(%d) Start Time %lf - Duration %lf \n",__FUNCTION__,__LINE__,pSegment->starttime_of_segment,pSegment->duration_of_segment);
	PrintExit;
	return pSegment->starttime_of_segment + pSegment->duration_of_segment;
}

HUINT32 HLS_CONTROLLER_GetCurStreamDuration(HLS_CTRL hCtrl)
{
/*
	m3u8_t *m3u8 = NULL;
//	m3u8Node_t* pStreamNode = NULL;
//	m3u8Stream_t *pStream = NULL;
	HLS_CONTROLLER_t *pController = NULL;
*/
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	/*
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}

	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	*/
	PrintExit;
	return (HUINT32)HLS_CONTROLLER_GetCurStreamEndTime(hCtrl);
}

HUINT64 HLS_CONTROLLER_GetCurStreamSequenceNum(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
//	m3u8Node_t* pStreamNode = NULL;
	m3u8Stream_t *pStream = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	PrintExit;
	return pStream->ullSequenceNum;
}

HUINT32 HLS_CONTROLLER_GetCurMaxBitrate(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pStreamNode = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurProg == NULL)
	{
		PrintError ("%s(%d) Current Program is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pProgram = (m3u8Program_t*)pController->pCurProg->pData;
	pStreamNode = UTIL_DLLIST_GetTailNode(&pProgram->streamSet);
	if(pStreamNode == NULL)
	{
		PrintError ("%s(%d) Highest Stream Node is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pStream = (m3u8Stream_t*)pStreamNode->pData;
	PrintExit;
	return pStream->unBandwidth;
}

HUINT32 HLS_CONTROLLER_GetCurMinBitrate(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pStreamNode = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurProg == NULL)
	{
		PrintError ("%s(%d) Current Program is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pProgram = (m3u8Program_t*)pController->pCurProg->pData;
	pStreamNode = UTIL_DLLIST_GetHeadNode(&pProgram->streamSet);
	if(pStreamNode == NULL)
	{
		PrintError ("%s(%d) Lowest Stream Node is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pStream = (m3u8Stream_t*)pStreamNode->pData;
	PrintExit;
	return pStream->unBandwidth;
}


HUINT32 HLS_CONTROLLER_GetCurBitrate(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
//	m3u8Node_t* pStreamNode = NULL;
	m3u8Stream_t *pStream = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	PrintExit;
	return pStream->unBandwidth;
}

HLS_ERROR_e	HLS_CONTROLLER_GetSegmentByTime(HLS_CTRL hCtrl, HUINT64 ullTime, HUINT64 *pullSegmentId)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pSegNode = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	HUINT32 ulStartTime = 0;
	HUINT32 ulDuration = 0;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		ulStartTime = pSegment->starttime_of_segment;
		ulDuration = pSegment->duration_of_segment;
		if( ulStartTime <= ullTime &&
 (ulDuration + ulStartTime) > ullTime )
		{
//			P_SSTR_CONTROLLER_Unlock(hSstr);
			*pullSegmentId = pSegment->ullNum;
			PrintExit;
			return HLS_OK;
		}
                pSegNode = UTIL_DLLIST_NextNode(pSegNode);
}
	PrintError("%s(%d) Err> ullTime(%"PRId64") is INVALID\n",__FUNCTION__,__LINE__,ullTime);
//	P_SSTR_CONTROLLER_Unlock(hSstr);
	PrintExit;
	return HLS_FAIL;
}

HLS_ERROR_e	HLS_CONTROLLER_SetSegmentByTime(HLS_CTRL hCtrl, HUINT32 ulTime)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pSegNode = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	HFLOAT32 fStartTime = 0;
	HFLOAT32 fDuration = 0;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pSegNode = UTIL_DLLIST_GetHeadNode(&pStream->segmentSet);
	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		fStartTime = pSegment->starttime_of_segment;
		fDuration = pSegment->duration_of_segment;
//		PrintError("%s(%d) Err> fStartTime(%u) ~ (%u) to %u\n",__FUNCTION__,__LINE__,fStartTime,fDuration + fStartTime,ulTime);
		if( fStartTime <= ulTime &&
		(fDuration + fStartTime) > ulTime )
		{
//			P_SSTR_CONTROLLER_Unlock(hSstr);
			pController->ullSegment = pSegment->ullNum;
			pController->pCurSegment = pSegNode;
			PrintExit;
			return HLS_OK;
		}
		pSegNode = UTIL_DLLIST_NextNode(pSegNode);

}
	PrintError("%s(%d) Err> ulTime(%u) is INVALID\n",__FUNCTION__,__LINE__,ulTime);
//	P_SSTR_CONTROLLER_Unlock(hSstr);
	PrintExit;
	return HLS_FAIL;
}

void HLS_CONTROLLER_ResetHeuristics(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t* pController = NULL;
	//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}
	pController = __controller(hCtrl);
	if(pController->hHeuristics == NULL)
	{
		PrintError("%s(%d) Err> hHeuristics is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}
	#if 0
	(void)SSTR_HEURISTICS_Reset(pController->hHeuristics);
	#else
	(void)SSTR_HEURISTICS_SetHeuristicsState(pController->hHeuristics,SSTR_HEUR_STATE_TIMID);
	#endif
	PrintExit;
	return;
}

HBOOL	HLS_CONTROLLER_CheckHeuristicsPanic(HLS_CTRL hCtrl, HUINT64 ullMs)
{
	HLS_CONTROLLER_t* pController = NULL;
	HUINT64 ullBufferSize = 0;
	HUINT64 ullReadableSize = 0;
	//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	if(pController->hHeuristics == NULL)
	{
		PrintError("%s(%d) Err> hHeuristics is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	ullBufferSize = SSTR_BUFFERMGR_GetBufferSize(pController->hBufferMgr);
	ullReadableSize = SSTR_BUFFERMGR_GetReadableSize(pController->hBufferMgr);
	PrintDebug("%s(%d) Debug> Bufferfullness : %"PRId64"\n",__FUNCTION__,__LINE__,ullReadableSize);
	PrintDebug("%s(%d) Debug> Time : %"PRId64"\n",__FUNCTION__,__LINE__,ullMs);
	if((ullBufferSize * SSTR_HEURISTICS_CONFIG_PANIC_BUFFERFULLNESS / 100) >= ullReadableSize
	&& (ullMs >= ( ( (HUINT64)HLS_CONTROLLER_GetCurSegmentDuration(hCtrl) * (HUINT64)HLS_CONST_MILLISEC * 2))))
	{
		if(HLS_CONTROLLER_GetCurBitrate(hCtrl) != HLS_CONTROLLER_GetCurMinBitrate(hCtrl))
		{
			SSTR_HEURISTICS_SetHeuristicsState(pController->hHeuristics, SSTR_HEUR_STATE_PANIC);
			HLS_CONTROLLER_GearLowest(hCtrl); /* set lowest bitrate */
			PrintError("%s(%d) Debug> Heuristics PANIC Detected !! \n",__FUNCTION__,__LINE__);
			PrintError("%s(%d) Debug> Change to PANIC : %"PRId64"\n",__FUNCTION__,__LINE__,SSTR_BUFFERMGR_GetReadableSize(pController->hBufferMgr));
			return TRUE;
		}
	}
	PrintExit;
	return FALSE;
}

void HLS_CONTROLLER_RegisterRequestUpdateCallback(HLS_CTRL hCtrl, void* hHLS, HLS_CONTROLLER_RequestUpdateCallback cbRequestUpdate)
{
	HLS_CONTROLLER_t* pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError("%s(%d) Err> Controller is NULL\n",__FUNCTION__,__LINE__);
		PrintExit;
		return;
	}
	pController = __controller(hCtrl);
	pController->hHLS = hHLS;
	pController->cbRequestUpdate = cbRequestUpdate;
	PrintExit;
	return;
}

#if 1 // __SUPPORT_BYTERANGE__
HBOOL HLS_CONTROLLER_GetCurSegmentByteRangeSupport(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
//	PrintExit;
	return pSegment->bByteRangeSupport;
}
HUINT64 HLS_CONTROLLER_GetCurSegmentByteRangeLength(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
//	PrintExit;
	return pSegment->ullByteRangeLength;
}
HUINT64 HLS_CONTROLLER_GetCurSegmentByteRangeOffset(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
//	PrintExit;
	return pSegment->ullByteRangeOffset;
}

HUINT64 HLS_CONTROLLER_GetCurSegmentByteRangeIndex(HLS_CTRL hCtrl)
{
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Segment_t *pSegment = NULL;
//	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	if(pController->m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 Handle is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegment = (m3u8Segment_t*)pController->pCurSegment->pData;
	if(pSegment == NULL)
	{
		PrintError("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
//	PrintExit;
	return pSegment->ullByteRangeIndex;
}
#endif

// 모든 media playlist 내의 ext-x-media-sequence 값 전의 segment 를 제거한다.
HBOOL HLS_CONTROLLER_DiscardSegmentsAndKeysUntilPlayListSequenceNum(HLS_CTRL hCtrl, HUINT64 ullPlayListSeqNum, HUINT32 unStream)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pStreamNode = NULL;
	m3u8Node_t* pProgNode = NULL;
	m3u8Segment_t *pSegment = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pCurStream = NULL;
	m3u8Stream_t* pTargetStream = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	m3u8Node_t* pSegNode = NULL;
	HBOOL bFound = FALSE;
	HUINT32 unStreamIdx = 0;
	HUINT64 ullCurSegmentNum = 0;
	PrintEnter;

	if(hCtrl == NULL)
	{
		PrintError("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pProgNode = UTIL_DLLIST_GetHeadNode(&m3u8->programSet);
	if(pProgNode == NULL)
	{
		PrintError("%s(%d) Program is INVALID \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pProgram = (m3u8Program_t*)pProgNode->pData;
	if(pProgram == NULL)
	{
		PrintError("%s(%d) Program Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pCurStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pCurStream == NULL)
	{
		PrintError("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}

	if(pController->pCurSegment == NULL)
	{
		PrintError("%s(%d) Current Segment is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	/* 현재 세그먼트 체크 */
	pSegNode = pController->pCurSegment;
	if(pSegNode == NULL)
	{
		PrintError("%s(%d) Segment Node is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pSegment = (m3u8Segment_t*)pSegNode->pData;
	if(pSegment == NULL)
	{
		PrintError("%s(%d) Segment Data is NULL. \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}



	pStreamNode = UTIL_DLLIST_GetHeadNode(&pProgram->streamSet);
	while(pStreamNode != NULL && pStreamNode->pData != NULL )
	{
		if(unStreamIdx == unStream)
		{
			pTargetStream = pStreamNode->pData;
			break;
		}
		unStreamIdx++;
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
	}


	PrintError("%s(%d) playlist %"PRId64" : current segment number %"PRId64" \n",__FUNCTION__,__LINE__, ullPlayListSeqNum, pSegment->ullNum);

#if 1

	/* network 상황이 느려 MEDIA-SEQUENCE 값보다 느린 segment 를 재생해도 냄기기.....  */
	if( ullPlayListSeqNum >= pSegment->ullNum )
	{   /* 만일 PlayList Sequence Number가 현재 Segment 과 같거나 보다 최신 것이라면,  현재 것 바로 전 것 까지만 버린다.  */
		PrintError("%s(%d) the current segment number is less than or to that of playlist !!! %"PRId64" >= %"PRId64" \n",__FUNCTION__,__LINE__, ullPlayListSeqNum, pSegment->ullNum);
#if 0 // 갱신하려는 playlist 만 제거
		return HLS_CONTROLLER_DiscardSegmentsAndKeysUntilCurrent(hCtrl);
#else
		return HLS_M3U8_DiscardSegmentsAndKeys(pTargetStream,pSegment->ullNum);
#endif
	}

	pSegNode = UTIL_DLLIST_GetHeadNode( &pTargetStream->segmentSet );
	if(pSegNode != NULL)
		pSegment = (m3u8Segment_t *)pSegNode->pData;
	else
		return FALSE;

	if( ullPlayListSeqNum < pSegment->ullNum )
	{   /* 만일 PlayList Sequence Number가 sgement list의 첫번째 것보다 작다면. 버릴 것이 없으므로 return FALSE */
		/* + 모든 Segment들이 사라지는 경우를 방지하지 위해 체크  */
		PrintError("%s(%d) the smallest number of segment is bigger than playlist !!! %"PRId64" < %"PRId64" \n",__FUNCTION__,__LINE__, ullPlayListSeqNum, pSegment->ullNum);
		PrintExit;
		return FALSE;
	}

	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t *)pSegNode->pData;
		if( pSegment->ullNum == ullPlayListSeqNum)
		{
			/* 모든 Segment들이 사라지는 경우를 방지하지 위해 체크  */
 			bFound = TRUE;
			break;
		}
		pSegNode = UTIL_DLLIST_NextNode(pSegNode);
	}
	if((pSegNode == NULL) || (bFound != TRUE))
	{
		PrintError("%s(%d) Cannot Found PlayList Sequence Segment \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}

	/* 만일 PlayList Sequence Number 바로 전 Segment 까지만 버린다.  */
	if(HLS_M3U8_DiscardSegmentsAndKeysUntilBaseSegment(pTargetStream,pSegment) == M3U8_ERR_OK)
	{
		pTargetStream->ullSequenceNum = pSegment->ullNum;
		PrintExit;
		return TRUE;
	}
#else

	/* network 상황이 느려 MEDIA-SEQUENCE 값보다 느린 segment 를 재생해도 냄기기.....  */
	if( ullPlayListSeqNum >= pSegment->ullNum )
	{   /* 만일 PlayList Sequence Number가 현재 Segment 과 같거나 보다 최신 것이라면,  현재 것 바로 전 것 까지만 버린다.  */
		PrintError("%s(%d) the current segment number is less than or to that of playlist !!! %"PRId64" >= %"PRId64" \n",__FUNCTION__,__LINE__, ullPlayListSeqNum, pSegment->ullNum);
		//return HLS_CONTROLLER_DiscardSegmentsAndKeysUntilCurrent(hCtrl);

		if(!HLS_CONTROLLER_DiscardSegmentsAndKeysUntilCurrent(hCtrl))
		{
			PrintError("%s(%d) : Error> HLS_M3U8_Update(Discard) Fail\n",__FUNCTION__,__LINE__);
		}
		else
		{
			HUINT32 unCurStream = 0;
			HUINT32 unStreamIdx = 0;
			HUINT64 ullCurSegmentNum = 0;

			PrintError("%s(%d) : Debug> HLS_M3U8_Update(Discard)\n",__FUNCTION__,__LINE__);

			if(HLS_CONTROLLER_GetCurStreamIndex(hCtrl, &unCurStream) == HLS_OK)
			{
				if(HLS_CONTROLLER_GetCurSegmentId(hCtrl, &ullCurSegmentNum) == HLS_OK)
				{
					pStreamNode = UTIL_DLLIST_GetHeadNode(&pProgram->streamSet);
					while(pStreamNode != NULL && pStreamNode->pData != NULL )
					{
						if(unStreamIdx != unCurStream)
						{
							m3u8Stream_t* pTargetStream = pStreamNode->pData;
							(void)HLS_M3U8_DiscardSegmentsAndKeys(pTargetStream,ullCurSegmentNum);
						}
					unStreamIdx++;
					pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
					}
				}
			}
		}
		return TRUE;
	}

#if 0
	if( ullPlayListSeqNum < pSegment->ullNum )
	{   /* 만일 PlayList Sequence Number가 sgement list의 첫번째 것보다 작다면. 버릴 것이 없으므로 return FALSE */
		/* + 모든 Segment들이 사라지는 경우를 방지하지 위해 체크  */
		PrintError("%s(%d) the smallest number of segment is bigger than playlist !!! %"PRId64" < %"PRId64" \n",__FUNCTION__,__LINE__, ullPlayListSeqNum, pSegment->ullNum);
		PrintExit;
		return FALSE;
	}
#endif

	pStreamNode = UTIL_DLLIST_GetHeadNode(&pProgram->streamSet);
	if(pStreamNode == NULL)
	{
		PrintError("%s(%d) Head Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	while(pStreamNode != NULL)
	{
		pStream = (m3u8Stream_t*)pStreamNode->pData;
		if(pStream == NULL)
		{
			PrintError("%s(%d) Lowest Stream is NULL \n",__FUNCTION__,__LINE__);
			PrintExit;
			return FALSE;
		}

		nNumOfVideoName = m3u8->alternativeRenditionNameTable.nCntVideoName;
		nNumOfAudioName = m3u8->alternativeRenditionNameTable.nCntAudioName;
		if(HLS_M3U8_IsSupportTag(M3U8_TAG_EXT_X_MEDIA, m3u8) && nNumOfVideoName != 0 )
		{
			if(NULL != pStream->pCurVideoRendition)
			{
				cso_log(CSO_LOG_INFO,"%s(%d) Stream uses alternative rendition\n",__FUNCTION__,__LINE__);
				bUsingVideoAlternativeRendition = TRUE;
				pAlternativeRendition = pStream->pCurVideoRendition;
			}
			else
			{
				cso_log(CSO_LOG_INFO,"%s(%d) Stream doesn't use alternative rendition\n",__FUNCTION__,__LINE__);
			}
		}
		pSegNode = UTIL_DLLIST_GetHeadNode( (bUsingVideoAlternativeRendition?&pAlternativeRendition->segmentSet:&pStream->segmentSet) );
		while(pSegNode != NULL)
		{
			pSegment = (m3u8Segment_t *)pSegNode->pData;
			if( pSegment->ullNum == ullPlayListSeqNum)
			{
				/* 모든 Segment들이 사라지는 경우를 방지하지 위해 체크  */
	 			bFound = TRUE;
				break;
			}
			pSegNode = UTIL_DLLIST_NextNode(pSegNode);
		}
		if((pSegNode == NULL) || (bFound != TRUE))
		{
			PrintError("%s(%d) Cannot Found PlayList Sequence Segment \n",__FUNCTION__,__LINE__);
			pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
			continue;
			//PrintExit;
			//return FALSE;
		}
		PrintError("%s(%d) Found segment releated playlist sequence num(%llu) \n",__FUNCTION__,__LINE__,pSegment->ullNum);

		PrintError("%s(%d) pCurSegment num(%llu) \n",__FUNCTION__,__LINE__,pSegment->ullNum);
		/* 만일 PlayList Sequence Number 바로 전 Segment 까지만 버린다.  */
		if(HLS_M3U8_DiscardSegmentsAndKeysUntilBaseSegment(pStream,pSegment) == M3U8_ERR_OK)
		{
			pStream->ullSequenceNum = pSegment->ullNum;
			//PrintExit;
			//return TRUE;
		}
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);

	}
#endif

	PrintExit;
	return FALSE;
}

HUINT32 HLS_CONTROLLER_GetCurStreamTargetDuration(HLS_CTRL hCtrl)
{
	m3u8_t *m3u8 = NULL;
	m3u8Node_t* pSegmentNode = NULL;
	m3u8Stream_t *pStream = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	PrintError ("%s(%d) Debug> Target Duration %d\n", __FUNCTION__,__LINE__, pStream->unTargetDuration);
	PrintExit;
	return pStream->unTargetDuration;
}

HLS_ERROR_e	HLS_CONTROLLER_SetSegmentToLive(HLS_CTRL hCtrl)
{
	HFLOAT32 fEndTime = 0;
	HFLOAT32 fLiveTime = 0;
	HUINT32 unThreeTargetDuration = 0;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	fEndTime = HLS_CONTROLLER_GetCurStreamEndTime(hCtrl);
	unThreeTargetDuration = 3 * HLS_CONTROLLER_GetCurStreamTargetDuration(hCtrl);
	fLiveTime = fEndTime - unThreeTargetDuration;

	if (HLS_CONTROLLER_SetSegmentByTime(hCtrl, fLiveTime) == HLS_OK)
	{
		PrintDebug ("%s(%d) Catching up the Live is success! LiveTime %lf\n",__FUNCTION__,__LINE__, fLiveTime);
		PrintExit;
		return HLS_OK;
	}

	PrintError ("%s(%d) FAIL \n",__FUNCTION__,__LINE__);
	PrintExit;
	return HLS_FAIL;
}

#if 0
HLS_ERROR_e	HLS_CONTROLLER_SetStreamToProbe(HLS_CTRL hCtrl)
{
	m3u8Node_t* pStreamNode = NULL;
	m3u8Node_t* pSegNode = NULL;
	m3u8Program_t *pProgram = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	HBOOL b_video_exist = FALSE;
	HBOOL b_audio_exist = FALSE;
	HINT32 nStreamNum = 0;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pController = __controller(hCtrl);
	if(pController->pCurProg == NULL)
	{
		PrintError ("%s(%d) Current Program is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	pProgram = (m3u8Program_t*)pController->pCurProg->pData;
	if(pProgram == NULL)
	{
		PrintError ("%s(%d) Current Program Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return FALSE;
	}
	/* 전체 스트림 중 에서  Video Audio가 있는지 확인 */
	pStreamNode = UTIL_DLLIST_GetHeadNode(&pProgram->streamSet);
    while(pStreamNode != NULL)
    {
    	pStream = (m3u8Stream_t*)pStreamNode->pData;
		if (pStream->eCodec & HLS_VIDEO_CODECS)
		{
			b_video_exist |= TRUE;
		}
		if (pStream->eCodec & HLS_AUDIO_CODECS)
		{
			b_audio_exist |= TRUE;
		}
		/* Codec 정보가 없으면, Video/Audio 정보가 모두 있는 것이다.  */
		if (pStream->eCodec == HLS_CODEC_NONE)
		{
			b_video_exist |= TRUE;;
			b_audio_exist |= TRUE;
		}

    	pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
    }
	if(b_video_exist) {
 		PrintDebug ("%s(%d) some or all streams have video \n",__FUNCTION__,__LINE__);
	} else {
		PrintDebug ("%s(%d) All streams do not have video \n",__FUNCTION__,__LINE__);
	}
	if(b_audio_exist) {
 		PrintDebug ("%s(%d) some or all streams have audio \n",__FUNCTION__,__LINE__);
	} else {
		PrintDebug ("%s(%d) All streams do not have audio \n",__FUNCTION__,__LINE__);
	}

	/* 스트림에 Audio/Video가 모두 존재한다면, AV가 있는 stream을 probing 용으로 선택.  */
	pStreamNode = UTIL_DLLIST_GetHeadNode(&pProgram->streamSet);
    while(pStreamNode != NULL)
    {
    	pStream = (m3u8Stream_t*)pStreamNode->pData;
		if ( pStream == NULL)
		{
			PrintError ("%s(%d) pStream is NULL!!!\n",__FUNCTION__,__LINE__);
			PrintExit;
			return FALSE;
		}
		if (((!!(pStream->eCodec & HLS_VIDEO_CODECS)) == b_video_exist) && ((!!(pStream->eCodec & HLS_AUDIO_CODECS)) == b_audio_exist))
		{
			break;
		}
		/* Codec 정보가 없으면, Video/Audio 정보가 모두 있는 것이다.  */
		if (pStream->eCodec == HLS_CODEC_NONE)
		{
			break;

		}
		pStreamNode = UTIL_DLLIST_NextNode(pStreamNode);
		nStreamNum++;
    }

	if (nStreamNum >= UTIL_DLLIST_Size(&pProgram->streamSet))
	{
		PrintError ("%s(%d) Cannot find a proper stream to probe, so choose the lowest one.\n",__FUNCTION__,__LINE__);
		HLS_CONTROLLER_Setup(hCtrl,0,0,0);
		PrintExit;
		return FALSE;
	}

	PrintDebug ("%s(%d) The choosed stream is %d .\n",__FUNCTION__,__LINE__, nStreamNum);
	HLS_CONTROLLER_Setup(hCtrl,0,nStreamNum,0);
	PrintExit;
	return TRUE;
}
#endif

HLS_ERROR_e	HLS_CONTROLLER_GetCurStreamSequenceTime(HLS_CTRL hCtrl, HUINT32 *puSequenceTime)
{
	float fEndTime = 0;
	float fLiveTime = 0;
	m3u8_t *m3u8 = NULL;
	HUINT32 unThreeTargetDuration = 0;
	m3u8Node_t* pSegNode = NULL;
	m3u8Stream_t *pStream = NULL;
	m3u8Segment_t *pSegment = NULL;
	HLS_CONTROLLER_t *pController = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	if(pController->pCurStream == NULL)
	{
		PrintError ("%s(%d) Current Stream is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pStream = (m3u8Stream_t*)pController->pCurStream->pData;
	if(pStream == NULL)
	{
		PrintError ("%s(%d) Current Stream Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}

	pSegNode = UTIL_DLLIST_GetHeadNode( &pStream->segmentSet );
	while(pSegNode != NULL)
	{
		pSegment = (m3u8Segment_t *)pSegNode->pData;
		if( pSegment->ullNum == HLS_CONTROLLER_GetCurStreamSequenceNum(hCtrl) )
		{
			break;
		}
		pSegNode = UTIL_DLLIST_NextNode(pSegNode);
	}
	*puSequenceTime  = (HUINT32)pSegment->starttime_of_segment;

	PrintExit;
	return HLS_OK;
}

HLS_ERROR_e	HLS_CONTROLLER_GetTimeToLive(HLS_CTRL hCtrl, HFLOAT32 *pfTimeToLive)
{
	HFLOAT32 fEndTime = 0;
	HFLOAT32 fLiveTime = 0;
	HUINT32 unThreeTargetDuration = 0;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) HLS Handle is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return HLS_FAIL;
	}
	fEndTime = HLS_CONTROLLER_GetCurStreamEndTime(hCtrl);
	unThreeTargetDuration = 3 * HLS_CONTROLLER_GetCurStreamTargetDuration(hCtrl);

	fLiveTime = fEndTime - unThreeTargetDuration;
	*pfTimeToLive = fLiveTime;

	PrintError ("%s(%d) pGetSeekTime : %lu \n",__FUNCTION__,__LINE__, *pfTimeToLive);

	PrintExit;
	return HLS_OK;
}


HFLOAT32 HLS_CONTROLLER_GetTimeGapBetweenCurAndLastForLiveRestart(HLS_CTRL hCtrl, HBOOL bOnLiveRestart)
{
	m3u8Segment_t* pSegment = NULL;
	m3u8Node_t* pSegNode = NULL;
	float fTimeGap = 0;
	float fCurTime = 0;
	float fLastTime = 0;
	HLS_CONTROLLER_t *pController = NULL;
	m3u8_t *m3u8 = NULL;
	PrintEnter;
	if(hCtrl == NULL)
	{
		PrintError ("%s(%d) Controller is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pController = __controller(hCtrl);
	m3u8 = pController->m3u8;
	if(m3u8 == NULL)
	{
		PrintError ("%s(%d) M3U8 Data is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	if(pController->pCurSegment == NULL)
	{
		PrintError ("%s(%d) Current Segment is NULL \n",__FUNCTION__,__LINE__);
		PrintExit;
		return 0;
	}
	pSegNode = pController->pCurSegment;
	if(pSegNode != NULL && pSegNode->pData != NULL)
	{
		pSegment = (m3u8Segment_t*)pSegNode->pData;
		fCurTime = pSegment->starttime_of_segment;

		/* MEDIA-SEQUENCE 값을 가지는 segment */
		if(bOnLiveRestart)
		{
			HLS_CONTROLLER_GetCurStreamSequenceTime(hCtrl, &fLastTime);
		}
		/* LIVE 시점 segment */
		else
		{
			HLS_CONTROLLER_GetTimeToLive(hCtrl, &fLastTime);
		}

		PrintError ("JWJI >> %s(%d): fCurTime(%f) fLastTime (%f) \n", __FUNCTION__, __LINE__, fCurTime, fLastTime);

		if( fLastTime > fCurTime )
		{
			fTimeGap = fLastTime - fCurTime;
			PrintError ("%s(%d): CheckTime gap(%f)\n", __FUNCTION__, __LINE__, fTimeGap);
			PrintExit;
			return fTimeGap;
		}
		else
		{
			fTimeGap = fLastTime - fCurTime;
			PrintError ("JWJI >> %s(%d): CheckTime gap(%f) return 0\n", __FUNCTION__, __LINE__, fTimeGap);
			PrintExit;
			return 0;
		}
	}
	PrintError ("%s(%d): CheckTime gap(%f)\n", __FUNCTION__, __LINE__, fTimeGap);
	PrintExit;
	return 0;
}

/*@}*/

