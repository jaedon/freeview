/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   adp_heuristics.h  $
 * Version:			$Revision:   $
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	     ADP Heuristics header
 * Module:
 * Remarks:	
 */	

/**
 * @ingroup ADP_HEURISTICS
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
 * @file adp_heuristics.h
 */


#ifndef __ADP_HEURISTICS_H__
#define __ADP_HEURISTICS_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include <sys/time.h> 	 /** for checking delay */
#include "adp_buffermgr.h"
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
#define ADP_HEURISTICS_CONST_PAIR 2
#define ADP_HEURISTICS_CONST_MILLISEC (1000)
#define ADP_HEURISTICS_CONST_PADDING (1)

#define ADP_HEURISTICS_CONFIG_CACHEDFACTOR 1
#define ADP_HEURISTICS_CONFIG_WINDOWSIZE 5
#define ADP_HEURISTICS_CONFIG_MAX_BANDWIDTH (1000000000)
#define ADP_HEURISTICS_CONFIG_CORRECTIONVALUE 70
#define ADP_HEURISTICS_CONFIG_ENC_DELAY_CORRECTIONVALUE 70

#define ADP_HEURISTICS_CONFIG_UPPER_BUFFERFULLNESS 100
#define ADP_HEURISTICS_CONFIG_LOWER_BUFFERFULLNESS 20
#define ADP_HEURISTICS_CONFIG_PANIC_BUFFERFULLNESS 10
#define ADP_HEURISTICS_CONFIG_BUFFER_HALFWAYFULL (ADP_HEURISTICS_CONFIG_LOWER_BUFFERFULLNESS + ((ADP_HEURISTICS_CONFIG_UPPER_BUFFERFULLNESS - ADP_HEURISTICS_CONFIG_LOWER_BUFFERFULLNESS)/2))
#define ADP_HEURISTICS_CONFIG_PANIC_DELAYTIME 85 /* percentage */
#define ADP_HEURISTICS_CONFIG_HALF_DELAYTIME 60 /* percentage */
#define ADP_HEURISTICS_CONFIG_HIGH_DELAYTIME 80 /* percentage */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/** 
 * Heuristics 운용을 위한 Callback Functions
 */
typedef HUINT32 (*ADP_HEUR_GetCurDuration) (void *hCtrl); /**< Fragment Duration 반환 */
typedef HUINT32 (*ADP_HEUR_GetCurBitrate) (void *hCtrl); /**< 현재 Bitrate 반환 */
typedef HBOOL (*ADP_HEUR_IsContentProtected) (void *hCtrl); /**< DRM 여부 반환 */
typedef HBOOL (*ADP_HEUR_GearDown) (void *hCtrl); /**< 현재 Bitrate 보다 한 단계 위로 Switching */
typedef HBOOL (*ADP_HEUR_GearUp) (void *hCtrl); /**< 현재 Bitrate 보다 한 단계 아래로 Switching */
typedef HBOOL (*ADP_HEUR_GearSet) (void *hCtrl, HUINT32 ulBitrate); /**< 특정 Bitrate 로 Switching */
typedef HBOOL (*ADP_HEUR_GearLowest) (void *hCtrl); /**< 최저 Bitrate 로 Switching */
typedef HBOOL (*ADP_HEUR_GearHighest) (void *hCtrl); /**< 최고 Bitrate 로 Switching */


/** 
 * ADP HEURISTICS ERRORCODE
 */
typedef enum
{
	ADP_HEUR_OK, /**< OK */
	ADP_HEUR_SWITCH_FAIL, /**< Bitrate Switching Fail */
	ADP_HEUR_FAIL /**< FAIL */
} ADP_HEUR_ERR;

/** 
 * ADP HEURISTICS STATE
 */
typedef enum
{
	ADP_HEUR_STATE_TIMID, /**< TIMID 상태, Network 속도 내에서만 Bitrate Switching 적용 */
	ADP_HEUR_STATE_GREEDY, /**< GREEDY 상태, Network 속도와 Bufferfullness 체크하여 Bitrate Switching 적용 */
	ADP_HEUR_STATE_PANIC, /**< PANIC 상태, 최저 Bitrate로 Bitrate Switching */
	ADP_HEUR_STATE_UNKNOWN	
} ADP_HEUR_STATE_e;

/** 
 * ADP HEURISTICS STARTUP MODE
 */
typedef enum
{
	ADP_HEUR_STARTUP_LOWEST, /**< 최저 Bitrate로 Startup */
	ADP_HEUR_STARTUP_HIGHEST, /**< 최고 Bitrate로 Startup */
	ADP_HEUR_STARTUP_AUTO, /**< AUTO Mode (현재는 최저 Bitrate로 Startup) */
	ADP_HEUR_STARTUP_MANUAL /**< 선택한 Track 에서 시작 */
} ADP_HEUR_STATRTUP_MODE_e;

/** 
 * ADP HEURISTICS DECISION MODE
 */
typedef enum
{
	ADP_HEUR_DECISION_AUTO, /**< Heuristics Module 내부 Algorithm에 의해 Bitrate-Switching */
	ADP_HEUR_DECISION_MANUAL, /**< User 선택에 의해 Bitrate-Switching (not supported yet) */
	ADP_HEUR_DECISION_FIXED /**< Bitrate-Switching 하지 않음 */
} ADP_HEUR_DECISION_MODE_e;

/** 
 * ADP HEURISTICS CONFIGURATION PRESET 
 */
typedef enum
{
	ADP_HEUR_CONFIG_PRESET_DEFAULT = 0, /**< Default Configuration */
	ADP_HEUR_CONFIG_PRESET_SSTR, /**< Default Configuration for SSTR */
	ADP_HEUR_CONFIG_PRESET_HLS, /**< Default Configuration for HLS */
	ADP_HEUR_CONFIG_PRESET_DASH, /**< Default Configuration for DASH */
} ADP_HEUR_CONFIG_PRESET_e;

/** 
 * ADP HEURISTICS SAMPLE DATA 
 */
typedef struct ADP_HEURISTICS_Sample_s
{
	HUINT64 ullSampleSize; /**< SAMPLE DATA 크기 (Fragment 크기) */
	HUINT64 ullMs;  /**< Download Delay */
} ADP_HEURISTICS_Sample_t;

/** 
 * ADP HEURISTICS CONFIGURATION 
 */
typedef struct ADP_HEURISTICS_CONFIG_s
{
	void *hCtrl; /**< Adaptive Controller Handle (Do not allocation) */	
	ADP_BUFFERMGR_t *hBufferMgr; /**<  Buffer Manager Handle (Do not allocation) */

	ADP_HEUR_STATRTUP_MODE_e eStartup; /**< ADP HEURISTICS STARTUP MODE */
	ADP_HEUR_DECISION_MODE_e eDecision; /**< ADP HEURISTICS DECISION MODE */

	HBOOL bBehaviorReset; /**< Reset 시 Decision 이 Auto 인 경우 Reset 시 초기화 여부 */

	/* Controller Callbacks */
	ADP_HEUR_GetCurBitrate cbCurBitrate; /**< 현재 Bitrate 반환 */
	ADP_HEUR_GetCurDuration cbCurDuration; /**< Fragment Duration 반환 */
	ADP_HEUR_IsContentProtected cbIsProtected; /**< DRM 여부 반환 */
	ADP_HEUR_GearUp cbGearUp;  /**< 현재 Bitrate 보다 한 단계 위로 Switching */
	ADP_HEUR_GearDown cbGearDown; /**< 현재 Bitrate 보다 한 단계 아래로 Switching */
	ADP_HEUR_GearSet cbGearSet; /**< 특정 Bitrate 로 Switching */
	ADP_HEUR_GearLowest cbGearLowest; /**< 최저 Bitrate 로 Switching */
	ADP_HEUR_GearHighest cbGearHighest; /**< 최고 Bitrate 로 Switching */

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
} ADP_HEURISTICS_CONFIG_t;

/** 
 * ADP HEURISTICS HANDLE 
 */
typedef struct ADP_HEURISTICS_s
{	
	ADP_HEUR_STATE_e eState; /**< Heuristics State(TIMID,GREEDY,PANIC) */
	ADP_HEURISTICS_CONFIG_t tConfig; /**< Configuration */
	HBOOL	bInit;  /**< Initialize 여부 */
	HBOOL	bLocked; /**< Bitrate Lock 여부 */
	HUINT64 ullLockedBandwidth; /**< Lock 된 Bitrate */
	HUINT64 ullLockedDelay; /**< Lock 시 Download Delay */
	
	HUINT64 ullPairBandwidth; /**< 두 개의 Fragment로 측정된 Bandwidth */
	HINT32 nCursor;  /**< SAMPLE Array Cursor */
	HINT32 nSize; /**< 현재 SAMPLE 수 */
	HUINT64 ullBandwidth; /**< 평균 Bandwidth */
	ADP_HEURISTICS_Sample_t tSample[ADP_HEURISTICS_CONFIG_WINDOWSIZE]; /**< Sample Array */
} ADP_HEURISTICS_t;

typedef ADP_HEURISTICS_t* ADP_HEURISTICS_HANDLE_t;

/* End typedef */
/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

/* Start global function prototypes */

/*! 
 *	\brief Heuristics 초기화 (Config 및 Controller Callbacks Setting)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config 및 Controller Callbacks
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR 		ADP_HEURISTICS_Init (ADP_HEURISTICS_HANDLE_t hHeuristics, ADP_HEURISTICS_CONFIG_t *pConfig);

/*! 
 *	\brief Config 설정 (Controller Callbacks 제외)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR 		ADP_HEURISTICS_SetConfig (ADP_HEURISTICS_HANDLE_t hHeuristics, ADP_HEURISTICS_CONFIG_t *pConfig);

/*! 
 *	\brief Config 반환 (Controller Callbacks 제외)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR 		ADP_HEURISTICS_GetConfig (ADP_HEURISTICS_HANDLE_t hHeuristics, ADP_HEURISTICS_CONFIG_t *pConfig);

/*! 
 *	\brief Default Config 반환 (Controller Callbacks 제외)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ePreset : ADP_HEUR_CONFIG_PRESET_e (SSTR/HLS/...)
 *	\param pConfig : Config
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR		ADP_HEURISTICS_GetDefaultConfig (ADP_HEURISTICS_HANDLE_t hHeuristics, ADP_HEUR_CONFIG_PRESET_e ePreset, ADP_HEURISTICS_CONFIG_t *pConfig);

/*! 
 *	\brief Heuristics 초기화 (Mode, Sample, Bandwidth ...)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return void
 *	\see
 */
void 				ADP_HEURISTICS_Reset (ADP_HEURISTICS_HANDLE_t hHeuristics);

/*! 
 *	\brief Heuristics 실행 (DECISION MODE가 AUTO 가 아니면 Bitrate-Switching 하지 않음)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ullFragSize : Fragment 크기
 *	\param ullMs : Download Delay
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR		ADP_HEURISTICS_Run(ADP_HEURISTICS_HANDLE_t hHeuristics, HUINT64 ullFragSize, HUINT64 ullMs);

/*! 
 *	\brief Pair Bandwidth 반환
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return Pair Bandwidth 
 *	\see
 */
HUINT64				ADP_HEURISTICS_GetPacketPairBandwidth (ADP_HEURISTICS_HANDLE_t hHeuristics);

/*! 
 *	\brief 샘플 입력
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ullFragSize : Fragment 크기
 *	\param ullMs : Download Delay
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR 		ADP_HEURISTICS_AddSample (ADP_HEURISTICS_HANDLE_t hHeuristics, HUINT64 ullSampleSize, HUINT64 ullMs);

/*! 
 *	\brief 평균 Bandwidth 반환
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return 평균 Bandwidth 
 *	\see
 */
HUINT64 			ADP_HEURISTICS_GetBandwidth (ADP_HEURISTICS_HANDLE_t hHeuristics);

/*! 
 *	\brief 현재의 Heuristics State 반환
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return ADP_HEUR_STATE_e
 *	\see
 */
ADP_HEUR_STATE_e	ADP_HEURISTICS_GetHeuristicsState (ADP_HEURISTICS_HANDLE_t hHeuristics);

/*! 
 *	\brief 현재의 Heuristics State 설정
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param eState : Heuristics State 
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR		ADP_HEURISTICS_SetHeuristicsState (ADP_HEURISTICS_HANDLE_t hHeuristics, ADP_HEUR_STATE_e eState);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __ADP_HEURISTICS_H__ */

/*@}*/

