/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   sstr_heuristics.c  $
 * Version:			$Revision:   1.0  $
 * Original Author:		YoungKi Kim $
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date: 2012.02.28
 * File Description:	SSTR Heuristics
 * Module:
 * Remarks:
 */

/**
 * @defgroup SSTR_HEURISTICS
 * @brief SSTR Heuristics Module 이며 Bitrate Switching 을 운용한다. <br> 현재 본 모듈은 HLS에서도 사용된다.
 */

/**
 * @author YoungKi Kim (ykkim3@humaxdigital.com)
 * @date 28 Feb 2012
 */

/**
 * @brief SSTR Heuristics Module 이며 Bitrate Switching 을 운용한다. <br> 현재 본 모듈은 HLS에서도 사용된다.
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file sstr_heuristics.c
 */


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

#include <inttypes.h>	 /** for 64bit MACRO ex>PRId64 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "di_iptuner_config.h"
#include "sstr_heuristics.h"

#include "htype.h"
#include "di_uart.h"

#include "vkernel.h"
#include "taskdef.h"
#include "util_url.h"

/* End Including Headers */
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
/* Start #define */
//#define SSTR_HEURISTICS_DEBUG
//#define DI_IPTUNER_CRITICAL_DEBUG

#ifdef IPTUNER_ALL_DEBUG
#ifdef SSTR_HEURISTICS_DEBUG
#define PrintData		DI_UART_Print
#define PrintDebug		DI_UART_Print
#define PrintError		DI_UART_Print
#define PrintEnter		DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit		DI_UART_Print("(-)%s\n", __FUNCTION__)
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

#ifdef DI_IPTUNER_CRITICAL_DEBUG
#define PrintCriticalDebug		DI_UART_Print
#else
#define PrintCriticalDebug		while (0) ((int (*)(char *, ...)) 0)
#endif

#define UNUSED_PARAM(x)  ((void)x)

/*----------------------------------------------------------------------------------------------
 *	MACRO
 *---------------------------------------------------------------------------------------------*/

void SSTR_HEURISTICS_Reset (SSTR_HEURISTICS_t *hHeuristics)
{
	if(hHeuristics == NULL)
	{
		return;
	}
	if(hHeuristics->tConfig.bBehaviorReset == TRUE && hHeuristics->tConfig.eDecision == SSTR_HEUR_DECISION_AUTO)
	{
		hHeuristics->eState = SSTR_HEUR_STATE_TIMID;
//		hHeuristics->ullPairBandwidth = 0;
		hHeuristics->nCursor = 0;
		hHeuristics->nSize = 0;
		hHeuristics->ullBandwidth = 0;
		hHeuristics->tConfig.cbGearLowest(hHeuristics->tConfig.hCtrl);
	}
	return;
}

HUINT64 SSTR_HEURISTICS_GetPacketPairBandwidth (SSTR_HEURISTICS_t *hHeuristics)
{
	HUINT64 ullBandwidth = 0;
	if(hHeuristics == NULL)
	{
		return 0;
	}
	ullBandwidth = hHeuristics->ullPairBandwidth * SSTR_HEURISTICS_CONFIG_CORRECTIONVALUE /* hHeuristics->tConfig.ulDelayValue */ / 100;
	return ullBandwidth;
}

SSTR_HEUR_ERR SSTR_HEURISTICS_AddSample (SSTR_HEURISTICS_t *hHeuristics, HUINT64 ullSampleSize, HUINT64 ullMs)
{
	HUINT64 ullTotalSampleSize = 0;
	HUINT64 ullTotalTime = 0; /* millisec */
	PrintEnter;
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	if(ullSampleSize == 0 && ullMs <= 10)
	{
		return SSTR_HEUR_OK;
	}
	hHeuristics->tSample[hHeuristics->nCursor].ullSampleSize = ullSampleSize;
	hHeuristics->tSample[hHeuristics->nCursor].ullMs = ullMs;
	if(hHeuristics->nSize < SSTR_HEURISTICS_CONFIG_WINDOWSIZE)
	{
		hHeuristics->nSize++;
		if(hHeuristics->nSize == (HINT32)hHeuristics->tConfig.unPairCnt)
		{
			for(int i = 0  ; i < (HINT32)hHeuristics->tConfig.unPairCnt ; i++)
			{
				ullTotalSampleSize += hHeuristics->tSample[i].ullSampleSize;
				ullTotalTime += hHeuristics->tSample[i].ullMs;
			}
			if( ullTotalTime == 0 )
			{
				/* GearUp ? */
				ullTotalTime = 10;
			}
			hHeuristics->ullPairBandwidth = ullTotalSampleSize / ullTotalTime * 8 * 1000; /* Millisec */
			if(hHeuristics->ullPairBandwidth > SSTR_HEURISTICS_CONFIG_MAX_BANDWIDTH)
			{
				PrintError("%s(%d) Debug> PairBandwidth Over 1Gps : %"PRId64"\n",__FUNCTION__,__LINE__,hHeuristics->ullPairBandwidth);
				hHeuristics->ullPairBandwidth = SSTR_HEURISTICS_CONFIG_MAX_BANDWIDTH;
			}
		}
	}
	else
	{
		for(int i = 0; i < hHeuristics->nSize; i++)
		{
			ullTotalSampleSize += hHeuristics->tSample[i].ullSampleSize;
			ullTotalTime += hHeuristics->tSample[i].ullMs;
		}
		if( ullTotalTime == 0 )
		{
			/* GearUp ? */
			ullTotalTime = 100;
		}
		hHeuristics->ullBandwidth = ullTotalSampleSize / ullTotalTime * 8 * 1000; /* Millisec */
		if(hHeuristics->ullBandwidth > SSTR_HEURISTICS_CONFIG_MAX_BANDWIDTH)
		{
			PrintError("%s(%d) Debug> Bandwidth Over 1Gps : %"PRId64"\n",__FUNCTION__,__LINE__,hHeuristics->ullBandwidth);
			hHeuristics->ullBandwidth = SSTR_HEURISTICS_CONFIG_MAX_BANDWIDTH;
		}
	}
	if(++hHeuristics->nCursor >= SSTR_HEURISTICS_CONFIG_WINDOWSIZE)
	{
		hHeuristics->nCursor = 0;
	}
	PrintExit;
	return SSTR_HEUR_OK;
}
HUINT64 SSTR_HEURISTICS_GetBandwidth (SSTR_HEURISTICS_t *hHeuristics)
{
	HUINT64 ullBandwidth = 0;
	PrintEnter;
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	ullBandwidth = hHeuristics->ullBandwidth * SSTR_HEURISTICS_CONFIG_CORRECTIONVALUE /* hHeuristics->tConfig.ulDelayValue */ / 100;
	PrintExit;
	return ullBandwidth;
}
SSTR_HEUR_STATE_e SSTR_HEURISTICS_GetHeuristicsState (SSTR_HEURISTICS_t *hHeuristics)
{
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_STATE_UNKNOWN;
	}
	return hHeuristics->eState;
}
SSTR_HEUR_ERR SSTR_HEURISTICS_SetHeuristicsState (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEUR_STATE_e eState)
{
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	hHeuristics->eState = eState;
	return SSTR_HEUR_OK;
}

SSTR_HEUR_ERR SSTR_HEURISTICS_Init (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEURISTICS_Config_t *pConfig)
{
	/* NOT SUPPORTED */
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	if(pConfig == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	if(hHeuristics->bInit == TRUE)
	{
		PrintDebug("%s(%d) : Debug> Already Initialized \n",__FUNCTION__,__LINE__);
		return SSTR_HEUR_OK;
	}
	VK_MEM_Memcpy(&hHeuristics->tConfig, pConfig, sizeof(SSTR_HEURISTICS_Config_t));
	switch(hHeuristics->tConfig.eStartup)
	{
		case SSTR_HEUR_STARTUP_HIGHEST :
			if(hHeuristics->tConfig.cbGearHighest!= NULL)
			{
				hHeuristics->tConfig.cbGearHighest(hHeuristics->tConfig.hCtrl);
				PrintDebug("%s(%d) : Debug> STARTUP : HIGHEST \n",__FUNCTION__,__LINE__);
			}
			break;
		case SSTR_HEUR_STARTUP_AUTO :
		default:
		case SSTR_HEUR_STARTUP_LOWEST:
			if(hHeuristics->tConfig.cbGearLowest != NULL)
			{
				hHeuristics->tConfig.cbGearLowest(hHeuristics->tConfig.hCtrl);
				PrintDebug("%s(%d) : Debug> STARTUP : LOWEST \n",__FUNCTION__,__LINE__);
			}
			break;
	}
	hHeuristics->bInit = TRUE;
	return SSTR_HEUR_OK;
}

SSTR_HEUR_ERR SSTR_HEURISTICS_SetConfig (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEURISTICS_Config_t *pConfig)
{
	/* NOT SUPPORTED */
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	if(pConfig == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	// VK_MEM_Memcpy(&hHeuristics->tConfig, pConfig, sizeof(SSTR_HEURISTICS_Config_t));
	hHeuristics->tConfig.eStartup = pConfig->eStartup;
	hHeuristics->tConfig.eDecision = pConfig->eDecision;

	hHeuristics->tConfig.ulDefaultDuration = pConfig->ulDefaultDuration;
	hHeuristics->tConfig.ulDelayValue = pConfig->ulDelayValue;
	hHeuristics->tConfig.ulDecValue = pConfig->ulDecValue;
	hHeuristics->tConfig.ulDelayLimitPerc = pConfig->ulDelayLimitPerc;
	hHeuristics->tConfig.ulBufferPositivePerc = pConfig->ulBufferPositivePerc;
	hHeuristics->tConfig.ulDelayPositivePerc = pConfig->ulDelayPositivePerc;
	hHeuristics->tConfig.ulEnterGreedyBufferPerc = pConfig->ulEnterGreedyBufferPerc;
	hHeuristics->tConfig.ulEnterPanicBufferPerc = pConfig->ulEnterPanicBufferPerc;
	hHeuristics->tConfig.ulEnterPanicDelayPerc = pConfig->ulEnterPanicDelayPerc;
	hHeuristics->tConfig.ulExitPanicBufferPerc = pConfig->ulExitPanicBufferPerc;
	hHeuristics->tConfig.ulExitPanicDelayPerc = pConfig->ulExitPanicDelayPerc;
	hHeuristics->tConfig.ulUnlockBufferPerc = pConfig->ulUnlockBufferPerc;
	return SSTR_HEUR_OK;
}

SSTR_HEUR_ERR SSTR_HEURISTICS_GetConfig (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEURISTICS_Config_t *pConfig)
{
	/* NOT SUPPORTED */
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	if(pConfig == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	// VK_MEM_Memcpy(pConfig, &hHeuristics->tConfig, sizeof(SSTR_HEURISTICS_Config_t));
	pConfig->eStartup = hHeuristics->tConfig.eStartup;
	pConfig->eDecision = hHeuristics->tConfig.eDecision;

	pConfig->ulDefaultDuration = hHeuristics->tConfig.ulDefaultDuration;
	pConfig->ulDelayValue = hHeuristics->tConfig.ulDelayValue;
	pConfig->ulDecValue = hHeuristics->tConfig.ulDecValue;
	pConfig->ulDelayLimitPerc = hHeuristics->tConfig.ulDelayLimitPerc;
	pConfig->ulBufferPositivePerc = hHeuristics->tConfig.ulBufferPositivePerc;
	pConfig->ulDelayPositivePerc = hHeuristics->tConfig.ulDelayPositivePerc;
	pConfig->ulEnterGreedyBufferPerc = hHeuristics->tConfig.ulEnterGreedyBufferPerc;
	pConfig->ulEnterPanicBufferPerc = hHeuristics->tConfig.ulEnterPanicBufferPerc;
	pConfig->ulEnterPanicDelayPerc = hHeuristics->tConfig.ulEnterPanicDelayPerc;
	pConfig->ulExitPanicBufferPerc = hHeuristics->tConfig.ulExitPanicBufferPerc;
	pConfig->ulExitPanicDelayPerc = hHeuristics->tConfig.ulExitPanicDelayPerc;
	pConfig->ulUnlockBufferPerc = hHeuristics->tConfig.ulUnlockBufferPerc;
	return SSTR_HEUR_OK;
}

SSTR_HEUR_ERR SSTR_HEURISTICS_GetDefaultConfig (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEUR_CONFIG_PRESET_e ePreset, SSTR_HEURISTICS_Config_t *pConfig)
{
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	if(pConfig == NULL)
	{
		return SSTR_HEUR_FAIL;
	}

	pConfig->eStartup = SSTR_HEUR_STARTUP_LOWEST;
	pConfig->eDecision = SSTR_HEUR_DECISION_AUTO;
	pConfig->bBehaviorReset = TRUE;
	pConfig->unPairCnt = 2;
	pConfig->ulDefaultDuration = 2;
	pConfig->ulDecValue = SSTR_HEURISTICS_CONFIG_ENC_DELAY_CORRECTIONVALUE;
	pConfig->ulDelayValue = SSTR_HEURISTICS_CONFIG_CORRECTIONVALUE;
	pConfig->ulDelayLimitPerc = 100;
	pConfig->ulBufferPositivePerc = SSTR_HEURISTICS_CONFIG_BUFFER_HALFWAYFULL;
	pConfig->ulDelayPositivePerc = SSTR_HEURISTICS_CONFIG_HALF_DELAYTIME;
	pConfig->ulEnterGreedyBufferPerc = SSTR_HEURISTICS_CONFIG_BUFFER_HALFWAYFULL - SSTR_HEURISTICS_CONFIG_LOWER_BUFFERFULLNESS;
	pConfig->ulEnterPanicBufferPerc = SSTR_HEURISTICS_CONFIG_PANIC_BUFFERFULLNESS;
	pConfig->ulEnterPanicDelayPerc = SSTR_HEURISTICS_CONFIG_PANIC_DELAYTIME;
	pConfig->ulExitPanicBufferPerc = SSTR_HEURISTICS_CONFIG_LOWER_BUFFERFULLNESS;
	pConfig->ulExitPanicDelayPerc = SSTR_HEURISTICS_CONFIG_HALF_DELAYTIME;
	pConfig->ulUnlockBufferPerc = 180;

	switch(ePreset)
	{
		case SSTR_HEUR_CONFIG_PRESET_SSTR:
			pConfig->unPairCnt = 2;
			pConfig->ulDefaultDuration = 2;
			pConfig->ulDelayValue = 80;
			//	tHeurConfig.ulDecValue = 100;
			pConfig->ulDelayLimitPerc = 120;
			//	tHeurConfig.ulBufferPositivePerc = 60;
			pConfig->ulDelayPositivePerc = 90;
			// 	pConfig->ulEnterGreedyBufferPerc = 30;
			//	tHeurConfig.ulEnterPanicBufferPerc = 10;
			//	tHeurConfig.ulEnterPanicDelayPerc = 85;
			//	tHeurConfig.ulExitPanicBufferPerc = 20;
			//	tHeurConfig.ulExitPanicDelayPerc = 60;
			break;
		case SSTR_HEUR_CONFIG_PRESET_HLS:
			pConfig->unPairCnt = 1;
			pConfig->ulDefaultDuration = 10;
			pConfig->ulDelayValue = 100;
			pConfig->ulDecValue = 80;
			pConfig->ulDelayLimitPerc = 120;
			pConfig->ulBufferPositivePerc = 60;
			pConfig->ulDelayPositivePerc = 90;
			pConfig->ulEnterGreedyBufferPerc = 30;
			pConfig->ulEnterPanicBufferPerc = 10;
			pConfig->ulEnterPanicDelayPerc = 85;
			pConfig->ulExitPanicBufferPerc = 20;
			pConfig->ulExitPanicDelayPerc = 60;
			break;
		case SSTR_HEUR_CONFIG_PRESET_DEFAULT:
		default:
			break;
	}
	return SSTR_HEUR_OK;
}

SSTR_HEUR_ERR	SSTR_HEURISTICS_Run(SSTR_HEURISTICS_t *hHeuristics, HUINT64 ullFragSize, HUINT64 ullMs)
{
	HINT32 nPoint = 0;
	HUINT64 ullBufferSize = 0;
	HUINT64 ullReadableSize = 0;
	HUINT64 ullBandwidth = 0;
	HUINT32 ulCurrentBitrate = 0;
	HUINT32 ulDuration = 0;
	HINT32 nBonusUp = 0;
	//	sstrMedia_t *pMedia = NULL;
	PrintEnter;
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	ullBufferSize = SSTR_BUFFERMGR_GetBufferSize(hHeuristics->tConfig.hBufferMgr);
	ullReadableSize = SSTR_BUFFERMGR_GetReadableSize(hHeuristics->tConfig.hBufferMgr);
	if(hHeuristics->tConfig.cbCurBitrate != NULL)
	{
		ulCurrentBitrate = hHeuristics->tConfig.cbCurBitrate(hHeuristics->tConfig.hCtrl);
	}
	else
	{
		return SSTR_HEUR_FAIL;
	}
	if(hHeuristics->tConfig.cbCurDuration != NULL)
	{
		ulDuration = hHeuristics->tConfig.cbCurDuration(hHeuristics->tConfig.hCtrl);
		if(ulDuration == 0)
		{
			return hHeuristics->tConfig.ulDefaultDuration;
		}
	}
	else
	{
		ulDuration = hHeuristics->tConfig.ulDefaultDuration;
	}

	if(ulDuration == 0)
	{
		return SSTR_HEUR_FAIL;
	}
	if(ullMs == 0)
	{
		ullMs = 10;
	}
	PrintDebug("%s(%d) Debug> Bufferfullness : %"PRId64"\n",__FUNCTION__,__LINE__,ullReadableSize);
	PrintDebug("%s(%d) Debug> ullBufferSize : %"PRId64"\n",__FUNCTION__,__LINE__,ullBufferSize);
	PrintDebug("%s(%d) Debug> ullFragSize : %"PRId64"\n",__FUNCTION__,__LINE__,ullFragSize);
	PrintDebug("%s(%d) Debug> Time : %"PRId64"\n",__FUNCTION__,__LINE__,ullMs);
	if(hHeuristics->tConfig.eDecision != SSTR_HEUR_DECISION_AUTO)
	{
		SSTR_HEURISTICS_AddSample(hHeuristics, ullFragSize, ullMs);
		return SSTR_HEUR_OK;
	}
	if( ullMs > ( ulDuration * SSTR_HEURISTICS_CONST_MILLISEC * hHeuristics->tConfig.ulDelayLimitPerc / 100))
	{
		SSTR_HEURISTICS_AddSample(hHeuristics, ullFragSize, ullMs);
		hHeuristics->bLocked = TRUE;
		hHeuristics->ullLockedBandwidth = ulCurrentBitrate;
		hHeuristics->ullLockedDelay = ullMs;
		PrintDebug("%s(%d) : Debug> Locked : %u ( %"PRId64" msec)\n",__FUNCTION__,__LINE__,ulCurrentBitrate, ullMs);
		nPoint--;
	}
	else
	{
		switch(SSTR_HEURISTICS_GetHeuristicsState(hHeuristics))
		{
			case SSTR_HEUR_STATE_TIMID :
				SSTR_HEURISTICS_AddSample(hHeuristics, ullFragSize, ullMs);
				ullBandwidth = SSTR_HEURISTICS_GetBandwidth(hHeuristics);
				if(ullBandwidth == 0)
				{
					ullBandwidth = SSTR_HEURISTICS_GetPacketPairBandwidth(hHeuristics);
				}
				PrintDebug("%s(%d) Debug> Bandwidth : %"PRId64"\n",__FUNCTION__,__LINE__,ullBandwidth);

				if(hHeuristics->tConfig.cbIsProtected != NULL)
				{
					if(hHeuristics->tConfig.cbIsProtected(hHeuristics->tConfig.hCtrl))
					{
						ullBandwidth = (ullBandwidth * hHeuristics->tConfig.ulDecValue / 100); /* Apply Correction Value for Decryption Delay */
					}
				}

				PrintDebug("%s(%d) Debug> Bandwidth(for Enc) : %"PRId64"\n",__FUNCTION__,__LINE__,ullBandwidth);

				/* if(ullReadableSize >= (ulDuration + SSTR_HEURISTICS_CONST_PADDING) * (ulCurrentBitrate / 8)) */
				{
					if(hHeuristics->tConfig.cbGearSet != NULL)
					{
						if(!hHeuristics->tConfig.cbGearSet(hHeuristics->tConfig.hCtrl,ullBandwidth)) /* set closest bitrate */
						{
							PrintError("%s(%d) : Error> GearSet Fail \n",__FUNCTION__,__LINE__);
							return SSTR_HEUR_SWITCH_FAIL;
						}
					}
				}
				nBonusUp = ((ulDuration * SSTR_HEURISTICS_CONST_MILLISEC) * SSTR_HEURISTICS_CONFIG_CORRECTIONVALUE / 100 / ullMs) - 1;
				for(HINT32 i = 0 ; i < nBonusUp ; i++)
				{

					if(hHeuristics->tConfig.cbGearUp != NULL)
					{
						if(!hHeuristics->tConfig.cbGearUp(hHeuristics->tConfig.hCtrl))
						{
							PrintError("%s(%d) : Error> GearUp Fail \n",__FUNCTION__,__LINE__);
							return SSTR_HEUR_SWITCH_FAIL;
						}
						PrintDebug("%s(%d) : Debug> Additional GEAR UP \n",__FUNCTION__,__LINE__);
					}
				}
				if((ullBufferSize * hHeuristics->tConfig.ulEnterGreedyBufferPerc / 100) <= ullReadableSize )
				{
					SSTR_HEURISTICS_SetHeuristicsState(hHeuristics, SSTR_HEUR_STATE_GREEDY);
					PrintDebug("%s(%d) Debug> Change to STEADY : %"PRId64"\n",__FUNCTION__,__LINE__,SSTR_BUFFERMGR_GetReadableSize(hHeuristics->tConfig.hBufferMgr));
				}
				break;

			case SSTR_HEUR_STATE_GREEDY :
				SSTR_HEURISTICS_AddSample(hHeuristics, ullFragSize, ullMs);
				PrintDebug("%s(%d) : Debug> SSTR_HEUR_STATE_GREEDY\n",__FUNCTION__,__LINE__);
				if((ullBufferSize * hHeuristics->tConfig.ulEnterPanicBufferPerc / 100) >= ullReadableSize
				&& (ullMs >= ( ( ulDuration * SSTR_HEURISTICS_CONST_MILLISEC * hHeuristics->tConfig.ulEnterPanicDelayPerc / 100))))
				{
					SSTR_HEURISTICS_SetHeuristicsState(hHeuristics, SSTR_HEUR_STATE_PANIC);
					// Gear Lowest
					if(hHeuristics->tConfig.cbGearLowest != NULL)
					{
						if(!hHeuristics->tConfig.cbGearLowest(hHeuristics->tConfig.hCtrl))
						{
							PrintError("%s(%d) : Error> LowestGear Fail \n",__FUNCTION__,__LINE__);
							return SSTR_HEUR_SWITCH_FAIL;
						}
						PrintDebug("%s(%d) : Debug> STEADY : GEAR FALL \n",__FUNCTION__,__LINE__);
					}
					PrintError("%s(%d) Debug> Change to PANIC : %"PRId64"\n",__FUNCTION__,__LINE__,SSTR_BUFFERMGR_GetReadableSize(hHeuristics->tConfig.hBufferMgr));
				}
				else
				{
					/**
					 * 1st Buffer Depth Check
					**/
					if((ullBufferSize * hHeuristics->tConfig.ulBufferPositivePerc / 100) <= ullReadableSize)
					{
						nPoint++;
						PrintDebug("%s(%d) : Debug> 1st Beffer Depth : Positive \n",__FUNCTION__,__LINE__);
					}
					else
					{
						nPoint--;
						PrintDebug("%s(%d) : Debug> 1st Beffer Depth : Negative \n",__FUNCTION__,__LINE__);
					}

					/**
					 * 2nd Delay Check
					**/
					if(ullMs < ( ulDuration * SSTR_HEURISTICS_CONST_MILLISEC * hHeuristics->tConfig.ulDelayPositivePerc / 100))
					{
						nPoint++;
						PrintDebug("%s(%d) : Debug> 2nd Delay : Positive \n",__FUNCTION__,__LINE__);
					}
					else
					{
						nPoint--;
						PrintDebug("%s(%d) : Debug> 2nd Delay : Negative \n",__FUNCTION__,__LINE__);
					}

					/**
					 * 3rd Buffer + Delay Check
					**/
/*					if(ullMs < ulDuration) */
					{
						HUINT32 ulNeededSize = 0;
						ulNeededSize = ullMs * (ulCurrentBitrate / 8) / SSTR_HEURISTICS_CONST_MILLISEC;
						if(ulNeededSize < ullReadableSize)
						{
							nPoint++;
							PrintDebug("%s(%d) : Debug> 3rd Buffer + Delay : Positive \n",__FUNCTION__,__LINE__);
						}
						else
						{
							nPoint--;
							PrintDebug("%s(%d) : Debug> 3rd Buffer + Delay : Negative \n",__FUNCTION__,__LINE__);
						}
					}
				}
				break;
			case SSTR_HEUR_STATE_PANIC :
				PrintError("%s(%d) : Debug> SSTR_HEUR_STATE_PANIC\n",__FUNCTION__,__LINE__);
				if((ullBufferSize * hHeuristics->tConfig.ulExitPanicBufferPerc / 100) < ullReadableSize
				|| (ullMs <= ( ( ulDuration * SSTR_HEURISTICS_CONST_MILLISEC * hHeuristics->tConfig.ulExitPanicDelayPerc / 100))))
				{
					SSTR_HEURISTICS_SetHeuristicsState(hHeuristics, SSTR_HEUR_STATE_TIMID);
					PrintDebug("%s(%d) Debug> Change to GREEDY : %"PRId64"\n",__FUNCTION__,__LINE__,SSTR_BUFFERMGR_GetReadableSize(hHeuristics->tConfig.hBufferMgr));
				}
				break;
			default:
				break;
		}
	}
	if(nPoint > 0)
	{
		if(hHeuristics->tConfig.cbGearUp != NULL)
		{
			if(!hHeuristics->tConfig.cbGearUp(hHeuristics->tConfig.hCtrl))
			{
				PrintError("%s(%d) : Error> GearUp Fail \n",__FUNCTION__,__LINE__);
				return SSTR_HEUR_SWITCH_FAIL;
			}
			PrintDebug("%s(%d) : Debug> GEAR UP \n",__FUNCTION__,__LINE__);
		}
		if(hHeuristics->bLocked)
		{
			if(hHeuristics->ullLockedBandwidth <= hHeuristics->tConfig.cbCurBitrate(hHeuristics->tConfig.hCtrl))
			{
				HUINT32 ulNeededSize = 0;
				ulNeededSize = (hHeuristics->ullLockedDelay * (hHeuristics->ullLockedBandwidth / 8) / SSTR_HEURISTICS_CONST_MILLISEC) * hHeuristics->tConfig.ulUnlockBufferPerc / 100;
				if(ulNeededSize >= ullBufferSize)
				{
					if(ullBufferSize * 90 / 100 <= SSTR_BUFFERMGR_GetReadableSize(hHeuristics->tConfig.hBufferMgr))
					{
						hHeuristics->bLocked = FALSE;
						hHeuristics->ullLockedBandwidth = 0;
						hHeuristics->ullLockedDelay = 0;
						PrintDebug("%s(%d) : Debug> Unlocked : ReadableSize : %"PRId64" \n",__FUNCTION__,__LINE__,SSTR_BUFFERMGR_GetReadableSize(hHeuristics->tConfig.hBufferMgr));
					}
				}
				else if(ulNeededSize < SSTR_BUFFERMGR_GetReadableSize(hHeuristics->tConfig.hBufferMgr))
				{
					hHeuristics->bLocked = FALSE;
					hHeuristics->ullLockedBandwidth = 0;
					hHeuristics->ullLockedDelay = 0;
					PrintDebug("%s(%d) : Debug> Unlocked : NeededBufferSize : %u \n",__FUNCTION__,__LINE__,ulNeededSize);
				}
				else
				{
					if(hHeuristics->tConfig.cbGearDown != NULL)
					{
						if(!hHeuristics->tConfig.cbGearDown(hHeuristics->tConfig.hCtrl))
						{
							PrintError("%s(%d) : Error> GearDown Fail \n",__FUNCTION__,__LINE__);
							return SSTR_HEUR_SWITCH_FAIL;
						}
						PrintDebug("%s(%d) : Debug> GEAR Down \n",__FUNCTION__,__LINE__);
					}
				}
			}
		}
	}
	else if(nPoint < 0)
	{
		if(hHeuristics->tConfig.cbGearDown != NULL)
		{
			if(!hHeuristics->tConfig.cbGearDown(hHeuristics->tConfig.hCtrl))
			{
				PrintError("%s(%d) : Error> GearDown Fail \n",__FUNCTION__,__LINE__);
				return SSTR_HEUR_SWITCH_FAIL;
			}
			PrintDebug("%s(%d) : Debug> GEAR Down \n",__FUNCTION__,__LINE__);
		}
	}
#if 0
	if(hHeuristics->tConfig.cbCurBitrate != NULL)
	{
		ulCurrentBitrate = hHeuristics->tConfig.cbCurBitrate(hHeuristics->tConfig.hCtrl);
		PrintDebug ("%s(%d) : Debug> Selected Bitrate : %u \n",__FUNCTION__,__LINE__,ulCurrentBitrate);
	}
#endif
	PrintExit;
	return SSTR_HEUR_OK;
}

#if 0 /* do not use (xxx) */

SSTR_HEUR_ERR SSTR_HEURISTICS_SetPacketPairBandwidth (SSTR_HEURISTICS_t *hHeuristics, HUINT64 ullBandwidth)
{
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	hHeuristics->ullPairBandwidth = ullBandwidth;
	return SSTR_HEUR_OK;

}

SSTR_HEUR_ERR SSTR_HEURISTICS_AddDownloadBandwidth (SSTR_HEURISTICS_t *hHeuristics, HUINT64 ullBandwidth)
{
	PrintEnter;
	HUINT64 ullSumOfBandwidth = 0;
	if(hHeuristics == NULL)
	{
		return SSTR_HEUR_FAIL;
	}
	hHeuristics->ullBandwidth[hHeuristics->nCursor] = ullBandwidth;
	if(hHeuristics->nSize < SSTR_HEURISTICS_CONFIG_WINDOWSIZE)
	{
		hHeuristics->nSize++;
	}
	for(int i = 0; i < hHeuristics->nSize; i++)
	{
		ullSumOfBandwidth += hHeuristics->ullBandwidth[i];
	}
	if(ullSumOfBandwidth != 0)
	{
		hHeuristics->ullAvgBandwidth = ullSumOfBandwidth / hHeuristics->nSize;
	}
	//hHeuristics->nCursor++;
	if(++hHeuristics->nCursor >= SSTR_HEURISTICS_CONFIG_WINDOWSIZE)
	{
		hHeuristics->nCursor = 0;
	}
	PrintExit;
	return SSTR_HEUR_OK;
}

HUINT64 SSTR_HEURISTICS_GetAvgDownloadBandwidth (SSTR_HEURISTICS_t *hHeuristics)
{
	// TODO:
	if(hHeuristics == NULL)
	{
		return 0;
	}
	return hHeuristics->ullAvgBandwidth;
}

#endif

/*@}*/

