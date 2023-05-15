/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   sstr_heuristics.h  $
 * Version:			$Revision:   $
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	     SSTR Manifest Controller for Streaming Header
 * Module:
 * Remarks:	
 */	

/**
 * @ingroup SSTR_HEURISTICS
 */

/**
 * @author YoungKi Kim(ykkim3@humaxdigital.com)
 * @date 28 Feb 2012
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
 * @file sstr_heuristics.h
 */


#ifndef __SSTR_HEURISTICS_H__
#define __SSTR_HEURISTICS_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include <sys/time.h> 	 /** for checking delay */
#include "sstr_buffermgr.h"
/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */

/*******************************************************************/
/************************ Constant Definition *************************/
/*******************************************************************/


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define SSTR_HEURISTICS_CONST_PAIR 2
#define SSTR_HEURISTICS_CONST_MILLISEC (1000)
#define SSTR_HEURISTICS_CONST_PADDING (1)

#define SSTR_HEURISTICS_CONFIG_CACHEDFACTOR 1
#define SSTR_HEURISTICS_CONFIG_WINDOWSIZE 5
#define SSTR_HEURISTICS_CONFIG_MAX_BANDWIDTH (1000000000)
#define SSTR_HEURISTICS_CONFIG_CORRECTIONVALUE 70
#define SSTR_HEURISTICS_CONFIG_ENC_DELAY_CORRECTIONVALUE 70

#define SSTR_HEURISTICS_CONFIG_UPPER_BUFFERFULLNESS 100
#define SSTR_HEURISTICS_CONFIG_LOWER_BUFFERFULLNESS 20
#define SSTR_HEURISTICS_CONFIG_PANIC_BUFFERFULLNESS 10
#define SSTR_HEURISTICS_CONFIG_BUFFER_HALFWAYFULL (SSTR_HEURISTICS_CONFIG_LOWER_BUFFERFULLNESS + ((SSTR_HEURISTICS_CONFIG_UPPER_BUFFERFULLNESS - SSTR_HEURISTICS_CONFIG_LOWER_BUFFERFULLNESS)/2))
#define SSTR_HEURISTICS_CONFIG_PANIC_DELAYTIME 85 /* percentage */
#define SSTR_HEURISTICS_CONFIG_HALF_DELAYTIME 60 /* percentage */
#define SSTR_HEURISTICS_CONFIG_HIGH_DELAYTIME 80 /* percentage */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/** 
 * Heuristics 운용을 위한 Callback Functions
 */
typedef HUINT32 (*SSTR_HEUR_GetCurDuration) (void *hCtrl); /**< Fragment Duration 반환 */
typedef HUINT32 (*SSTR_HEUR_GetCurBitrate) (void *hCtrl); /**< 현재 Bitrate 반환 */
typedef HBOOL (*SSTR_HEUR_IsContentProtected) (void *hCtrl); /**< DRM 여부 반환 */
typedef HBOOL (*SSTR_HEUR_GearDown) (void *hCtrl); /**< 현재 Bitrate 보다 한 단계 위로 Switching */
typedef HBOOL (*SSTR_HEUR_GearUp) (void *hCtrl); /**< 현재 Bitrate 보다 한 단계 아래로 Switching */
typedef HBOOL (*SSTR_HEUR_GearSet) (void *hCtrl, HUINT32 ulBitrate); /**< 특정 Bitrate 로 Switching */
typedef HBOOL (*SSTR_HEUR_GearLowest) (void *hCtrl); /**< 최저 Bitrate 로 Switching */
typedef HBOOL (*SSTR_HEUR_GearHighest) (void *hCtrl); /**< 최고 Bitrate 로 Switching */

/** 
 * SSTR HEURISTICS ERRORCODE
 */
typedef enum
{
	SSTR_HEUR_OK, /**< OK */
	SSTR_HEUR_SWITCH_FAIL, /**< Bitrate Switching Fail */
	SSTR_HEUR_FAIL /**< FAIL */
} SSTR_HEUR_ERR;

/** 
 * SSTR HEURISTICS STATE
 */
typedef enum
{
	SSTR_HEUR_STATE_TIMID, /**< TIMID 상태, Network 속도 내에서만 Bitrate Switching 적용 */
	SSTR_HEUR_STATE_GREEDY, /**< GREEDY 상태, Network 속도와 Bufferfullness 체크하여 Bitrate Switching 적용 */
	SSTR_HEUR_STATE_PANIC, /**< PANIC 상태, 최저 Bitrate로 Bitrate Switching */
	SSTR_HEUR_STATE_UNKNOWN	
} SSTR_HEUR_STATE_e;

/** 
 * SSTR HEURISTICS STARTUP MODE
 */
typedef enum
{
	SSTR_HEUR_STARTUP_LOWEST, /**< 최저 Bitrate로 Startup */
	SSTR_HEUR_STARTUP_HIGHEST, /**< 최고 Bitrate로 Startup */
	SSTR_HEUR_STARTUP_AUTO, /**< AUTO Mode (현재는 최저 Bitrate로 Startup) */
	SSTR_HEUR_STARTUP_MANUAL /**< 선택한 Track 에서 시작 */
} SSTR_HEUR_STATRTUP_MODE_e;

/** 
 * SSTR HEURISTICS DECISION MODE
 */
typedef enum
{
	SSTR_HEUR_DECISION_AUTO, /**< Heuristics Module 내부 Algorithm에 의해 Bitrate-Switching */
	SSTR_HEUR_DECISION_MANUAL, /**< User 선택에 의해 Bitrate-Switching (not supported yet) */
	SSTR_HEUR_DECISION_FIXED /**< Bitrate-Switching 하지 않음 */
} SSTR_HEUR_DECISION_MODE_e;

/** 
 * SSTR HEURISTICS CONFIGURATION PRESET 
 */
typedef enum
{
	SSTR_HEUR_CONFIG_PRESET_DEFAULT = 0, /**< Default Configuration */
	SSTR_HEUR_CONFIG_PRESET_SSTR, /**< Default Configuration for SSTR */
	SSTR_HEUR_CONFIG_PRESET_HLS /**< Default Configuration for HLS */
} SSTR_HEUR_CONFIG_PRESET_e;

/** 
 * SSTR HEURISTICS SAMPLE DATA 
 */
typedef struct SSTR_HEURISTICS_Sample_s
{
	HUINT64 ullSampleSize; /**< SAMPLE DATA 크기 (Fragment 크기) */
	HUINT64 ullMs;  /**< Download Delay */
} SSTR_HEURISTICS_Sample_t;

/** 
 * SSTR HEURISTICS CONFIGURATION 
 */
typedef struct SSTR_HEURISTICS_Config_s
{
	void *hCtrl; /**< Adaptive Controller Handle (Do not allocation) */	
	SSTR_BUFFERMGR_t *hBufferMgr; /**<  Buffer Manager Handle (Do not allocation) */

	SSTR_HEUR_STATRTUP_MODE_e eStartup; /**< SSTR HEURISTICS STARTUP MODE */
	SSTR_HEUR_DECISION_MODE_e eDecision; /**< SSTR HEURISTICS DECISION MODE */

	HBOOL bBehaviorReset; /**< Reset 시 Decision 이 Auto 인 경우 Reset 시 초기화 여부 */

	/* Controller Callbacks */
	SSTR_HEUR_GetCurBitrate cbCurBitrate; /**< 현재 Bitrate 반환 */
	SSTR_HEUR_GetCurDuration cbCurDuration; /**< Fragment Duration 반환 */
	SSTR_HEUR_IsContentProtected cbIsProtected; /**< DRM 여부 반환 */
	SSTR_HEUR_GearUp cbGearUp;  /**< 현재 Bitrate 보다 한 단계 위로 Switching */
	SSTR_HEUR_GearDown cbGearDown; /**< 현재 Bitrate 보다 한 단계 아래로 Switching */
	SSTR_HEUR_GearSet cbGearSet; /**< 특정 Bitrate 로 Switching */
	SSTR_HEUR_GearLowest cbGearLowest; /**< 최저 Bitrate 로 Switching */
	SSTR_HEUR_GearHighest cbGearHighest; /**< 최고 Bitrate 로 Switching */

	HUINT32 unPairCnt; /**< 초기 Bandwidth 를 측정하기 위한 Sample 개수 */
	HUINT32 ulDefaultDuration; /**< Default Fragment Duration (별도의 Fragment Duration이 없을 경우 사용) */
	HUINT32 ulDelayValue; /**< Correction Value for Delay (Delay 보정 값) */
	HUINT32 ulDecValue; /**< Correction Value for Decryption Delay (Decryption 이 필요한 경우의 Delay 보정 값)  */

	HUINT32 ulDelayLimitPerc; /**< Fragment Duration 대비 Delay 제한(%) */
	HUINT32 ulBufferPositivePerc; /**< Buffer 크기 대비 해당 % 이상 차있는 경우 Point + */
	HUINT32 ulDelayPositivePerc; /**< Fragment Duration 대비 해당 % 이하 Download Delay의 경우 Point + */
	HUINT32 ulEnterGreedyBufferPerc; /**< Greedy Mode 진입을 위한 Buffer 양 (%) */
	HUINT32 ulEnterPanicBufferPerc; /**< Panic Mode 진입을 위한 Buffer 양 (%) (ulEnterPanicDelayPerc 와 같이 사용됨)  */
	HUINT32 ulEnterPanicDelayPerc; /**< Panic Mode 진입을 위한 Fragment Duration 대비 Delay (%) (ulEnterPanicBufferPerc 와 같이 사용됨) */
	HUINT32 ulExitPanicBufferPerc; /**< Panic Mode 탈출을 위한 Buffer 양 (%) (ulExitPanicDelayPerc 와 같이 사용됨)  */
	HUINT32 ulExitPanicDelayPerc; /**< Panic Mode 탈출을 위한 Fragment Duration 대비 Delay (%) (ulExitPanicBufferPerc 와 같이 사용됨) */
	HUINT32 ulUnlockBufferPerc; /**<  Bitrate Lock을 해제하기 위한 Buffer의 양 (%) */
} SSTR_HEURISTICS_Config_t;

/** 
 * SSTR HEURISTICS HANDLE 
 */
typedef struct SSTR_HEURISTICS_s
{	
	SSTR_HEUR_STATE_e eState; /**< Heuristics State(TIMID,GREEDY,PANIC) */
	SSTR_HEURISTICS_Config_t tConfig; /**< Configuration */
	HBOOL	bInit;  /**< Initialize 여부 */
	HBOOL	bLocked; /**< Bitrate Lock 여부 */
	HUINT64 ullLockedBandwidth; /**< Lock 된 Bitrate */
	HUINT64 ullLockedDelay; /**< Lock 시 Download Delay */
	
	HUINT64 ullPairBandwidth; /**< 두 개의 Fragment로 측정된 Bandwidth */
	HINT32 nCursor;  /**< SAMPLE Array Cursor */
	HINT32 nSize; /**< 현재 SAMPLE 수 */
	HUINT64 ullBandwidth; /**< 평균 Bandwidth */
	SSTR_HEURISTICS_Sample_t tSample[SSTR_HEURISTICS_CONFIG_WINDOWSIZE]; /**< Sample Array */
} SSTR_HEURISTICS_t;

//typedef SSTR_BUFFERMGR_t SSTR_BUFFERMGR_HANDLE;

/* End typedef */
/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

/* Start global function prototypes */

/*! 
 *	\brief Heuristics 초기화 (Config 및 Controller Callbacks Setting)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config 및 Controller Callbacks
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR 		SSTR_HEURISTICS_Init (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEURISTICS_Config_t *pConfig);

/*! 
 *	\brief Config 설정 (Controller Callbacks 제외)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR 		SSTR_HEURISTICS_SetConfig (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEURISTICS_Config_t *pConfig);

/*! 
 *	\brief Config 반환 (Controller Callbacks 제외)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR 		SSTR_HEURISTICS_GetConfig (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEURISTICS_Config_t *pConfig);

/*! 
 *	\brief Default Config 반환 (Controller Callbacks 제외)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ePreset : SSTR_HEUR_CONFIG_PRESET_e (SSTR/HLS/...)
 *	\param pConfig : Config
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR		SSTR_HEURISTICS_GetDefaultConfig (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEUR_CONFIG_PRESET_e ePreset, SSTR_HEURISTICS_Config_t *pConfig);

/*! 
 *	\brief Heuristics 초기화 (Mode, Sample, Bandwidth ...)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return void
 *	\see
 */
void 				SSTR_HEURISTICS_Reset (SSTR_HEURISTICS_t *hHeuristics);

/*! 
 *	\brief Heuristics 실행 (DECISION MODE가 AUTO 가 아니면 Bitrate-Switching 하지 않음)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ullFragSize : Fragment 크기
 *	\param ullMs : Download Delay
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR		SSTR_HEURISTICS_Run(SSTR_HEURISTICS_t *hHeuristics, HUINT64 ullFragSize, HUINT64 ullMs);

/*! 
 *	\brief Pair Bandwidth 반환
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return Pair Bandwidth 
 *	\see
 */
HUINT64				SSTR_HEURISTICS_GetPacketPairBandwidth (SSTR_HEURISTICS_t *hHeuristics);

/*! 
 *	\brief 샘플 입력
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ullFragSize : Fragment 크기
 *	\param ullMs : Download Delay
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR 		SSTR_HEURISTICS_AddSample (SSTR_HEURISTICS_t *hHeuristics, HUINT64 ullSampleSize, HUINT64 ullMs);

/*! 
 *	\brief 평균 Bandwidth 반환
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return 평균 Bandwidth 
 *	\see
 */
HUINT64 			SSTR_HEURISTICS_GetBandwidth (SSTR_HEURISTICS_t *hHeuristics);

/*! 
 *	\brief 현재의 Heuristics State 반환
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return SSTR_HEUR_STATE_e
 *	\see
 */
SSTR_HEUR_STATE_e	SSTR_HEURISTICS_GetHeuristicsState (SSTR_HEURISTICS_t *hHeuristics);

/*! 
 *	\brief 현재의 Heuristics State 설정
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param eState : Heuristics State 
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR		SSTR_HEURISTICS_SetHeuristicsState (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEUR_STATE_e eState);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __SSTR_HEURISTICS_H__ */

/*@}*/

