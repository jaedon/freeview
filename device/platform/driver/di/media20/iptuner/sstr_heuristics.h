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
 * Heuristics ����� ���� Callback Functions
 */
typedef HUINT32 (*SSTR_HEUR_GetCurDuration) (void *hCtrl); /**< Fragment Duration ��ȯ */
typedef HUINT32 (*SSTR_HEUR_GetCurBitrate) (void *hCtrl); /**< ���� Bitrate ��ȯ */
typedef HBOOL (*SSTR_HEUR_IsContentProtected) (void *hCtrl); /**< DRM ���� ��ȯ */
typedef HBOOL (*SSTR_HEUR_GearDown) (void *hCtrl); /**< ���� Bitrate ���� �� �ܰ� ���� Switching */
typedef HBOOL (*SSTR_HEUR_GearUp) (void *hCtrl); /**< ���� Bitrate ���� �� �ܰ� �Ʒ��� Switching */
typedef HBOOL (*SSTR_HEUR_GearSet) (void *hCtrl, HUINT32 ulBitrate); /**< Ư�� Bitrate �� Switching */
typedef HBOOL (*SSTR_HEUR_GearLowest) (void *hCtrl); /**< ���� Bitrate �� Switching */
typedef HBOOL (*SSTR_HEUR_GearHighest) (void *hCtrl); /**< �ְ� Bitrate �� Switching */

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
	SSTR_HEUR_STATE_TIMID, /**< TIMID ����, Network �ӵ� �������� Bitrate Switching ���� */
	SSTR_HEUR_STATE_GREEDY, /**< GREEDY ����, Network �ӵ��� Bufferfullness üũ�Ͽ� Bitrate Switching ���� */
	SSTR_HEUR_STATE_PANIC, /**< PANIC ����, ���� Bitrate�� Bitrate Switching */
	SSTR_HEUR_STATE_UNKNOWN	
} SSTR_HEUR_STATE_e;

/** 
 * SSTR HEURISTICS STARTUP MODE
 */
typedef enum
{
	SSTR_HEUR_STARTUP_LOWEST, /**< ���� Bitrate�� Startup */
	SSTR_HEUR_STARTUP_HIGHEST, /**< �ְ� Bitrate�� Startup */
	SSTR_HEUR_STARTUP_AUTO, /**< AUTO Mode (����� ���� Bitrate�� Startup) */
	SSTR_HEUR_STARTUP_MANUAL /**< ������ Track ���� ���� */
} SSTR_HEUR_STATRTUP_MODE_e;

/** 
 * SSTR HEURISTICS DECISION MODE
 */
typedef enum
{
	SSTR_HEUR_DECISION_AUTO, /**< Heuristics Module ���� Algorithm�� ���� Bitrate-Switching */
	SSTR_HEUR_DECISION_MANUAL, /**< User ���ÿ� ���� Bitrate-Switching (not supported yet) */
	SSTR_HEUR_DECISION_FIXED /**< Bitrate-Switching ���� ���� */
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
	HUINT64 ullSampleSize; /**< SAMPLE DATA ũ�� (Fragment ũ��) */
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

	HBOOL bBehaviorReset; /**< Reset �� Decision �� Auto �� ��� Reset �� �ʱ�ȭ ���� */

	/* Controller Callbacks */
	SSTR_HEUR_GetCurBitrate cbCurBitrate; /**< ���� Bitrate ��ȯ */
	SSTR_HEUR_GetCurDuration cbCurDuration; /**< Fragment Duration ��ȯ */
	SSTR_HEUR_IsContentProtected cbIsProtected; /**< DRM ���� ��ȯ */
	SSTR_HEUR_GearUp cbGearUp;  /**< ���� Bitrate ���� �� �ܰ� ���� Switching */
	SSTR_HEUR_GearDown cbGearDown; /**< ���� Bitrate ���� �� �ܰ� �Ʒ��� Switching */
	SSTR_HEUR_GearSet cbGearSet; /**< Ư�� Bitrate �� Switching */
	SSTR_HEUR_GearLowest cbGearLowest; /**< ���� Bitrate �� Switching */
	SSTR_HEUR_GearHighest cbGearHighest; /**< �ְ� Bitrate �� Switching */

	HUINT32 unPairCnt; /**< �ʱ� Bandwidth �� �����ϱ� ���� Sample ���� */
	HUINT32 ulDefaultDuration; /**< Default Fragment Duration (������ Fragment Duration�� ���� ��� ���) */
	HUINT32 ulDelayValue; /**< Correction Value for Delay (Delay ���� ��) */
	HUINT32 ulDecValue; /**< Correction Value for Decryption Delay (Decryption �� �ʿ��� ����� Delay ���� ��)  */

	HUINT32 ulDelayLimitPerc; /**< Fragment Duration ��� Delay ����(%) */
	HUINT32 ulBufferPositivePerc; /**< Buffer ũ�� ��� �ش� % �̻� ���ִ� ��� Point + */
	HUINT32 ulDelayPositivePerc; /**< Fragment Duration ��� �ش� % ���� Download Delay�� ��� Point + */
	HUINT32 ulEnterGreedyBufferPerc; /**< Greedy Mode ������ ���� Buffer �� (%) */
	HUINT32 ulEnterPanicBufferPerc; /**< Panic Mode ������ ���� Buffer �� (%) (ulEnterPanicDelayPerc �� ���� ����)  */
	HUINT32 ulEnterPanicDelayPerc; /**< Panic Mode ������ ���� Fragment Duration ��� Delay (%) (ulEnterPanicBufferPerc �� ���� ����) */
	HUINT32 ulExitPanicBufferPerc; /**< Panic Mode Ż���� ���� Buffer �� (%) (ulExitPanicDelayPerc �� ���� ����)  */
	HUINT32 ulExitPanicDelayPerc; /**< Panic Mode Ż���� ���� Fragment Duration ��� Delay (%) (ulExitPanicBufferPerc �� ���� ����) */
	HUINT32 ulUnlockBufferPerc; /**<  Bitrate Lock�� �����ϱ� ���� Buffer�� �� (%) */
} SSTR_HEURISTICS_Config_t;

/** 
 * SSTR HEURISTICS HANDLE 
 */
typedef struct SSTR_HEURISTICS_s
{	
	SSTR_HEUR_STATE_e eState; /**< Heuristics State(TIMID,GREEDY,PANIC) */
	SSTR_HEURISTICS_Config_t tConfig; /**< Configuration */
	HBOOL	bInit;  /**< Initialize ���� */
	HBOOL	bLocked; /**< Bitrate Lock ���� */
	HUINT64 ullLockedBandwidth; /**< Lock �� Bitrate */
	HUINT64 ullLockedDelay; /**< Lock �� Download Delay */
	
	HUINT64 ullPairBandwidth; /**< �� ���� Fragment�� ������ Bandwidth */
	HINT32 nCursor;  /**< SAMPLE Array Cursor */
	HINT32 nSize; /**< ���� SAMPLE �� */
	HUINT64 ullBandwidth; /**< ��� Bandwidth */
	SSTR_HEURISTICS_Sample_t tSample[SSTR_HEURISTICS_CONFIG_WINDOWSIZE]; /**< Sample Array */
} SSTR_HEURISTICS_t;

//typedef SSTR_BUFFERMGR_t SSTR_BUFFERMGR_HANDLE;

/* End typedef */
/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

/* Start global function prototypes */

/*! 
 *	\brief Heuristics �ʱ�ȭ (Config �� Controller Callbacks Setting)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config �� Controller Callbacks
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR 		SSTR_HEURISTICS_Init (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEURISTICS_Config_t *pConfig);

/*! 
 *	\brief Config ���� (Controller Callbacks ����)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR 		SSTR_HEURISTICS_SetConfig (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEURISTICS_Config_t *pConfig);

/*! 
 *	\brief Config ��ȯ (Controller Callbacks ����)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR 		SSTR_HEURISTICS_GetConfig (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEURISTICS_Config_t *pConfig);

/*! 
 *	\brief Default Config ��ȯ (Controller Callbacks ����)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ePreset : SSTR_HEUR_CONFIG_PRESET_e (SSTR/HLS/...)
 *	\param pConfig : Config
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR		SSTR_HEURISTICS_GetDefaultConfig (SSTR_HEURISTICS_t *hHeuristics, SSTR_HEUR_CONFIG_PRESET_e ePreset, SSTR_HEURISTICS_Config_t *pConfig);

/*! 
 *	\brief Heuristics �ʱ�ȭ (Mode, Sample, Bandwidth ...)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return void
 *	\see
 */
void 				SSTR_HEURISTICS_Reset (SSTR_HEURISTICS_t *hHeuristics);

/*! 
 *	\brief Heuristics ���� (DECISION MODE�� AUTO �� �ƴϸ� Bitrate-Switching ���� ����)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ullFragSize : Fragment ũ��
 *	\param ullMs : Download Delay
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR		SSTR_HEURISTICS_Run(SSTR_HEURISTICS_t *hHeuristics, HUINT64 ullFragSize, HUINT64 ullMs);

/*! 
 *	\brief Pair Bandwidth ��ȯ
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return Pair Bandwidth 
 *	\see
 */
HUINT64				SSTR_HEURISTICS_GetPacketPairBandwidth (SSTR_HEURISTICS_t *hHeuristics);

/*! 
 *	\brief ���� �Է�
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ullFragSize : Fragment ũ��
 *	\param ullMs : Download Delay
 *	\return SSTR_HEUR_ERR
 *	\see
 */
SSTR_HEUR_ERR 		SSTR_HEURISTICS_AddSample (SSTR_HEURISTICS_t *hHeuristics, HUINT64 ullSampleSize, HUINT64 ullMs);

/*! 
 *	\brief ��� Bandwidth ��ȯ
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return ��� Bandwidth 
 *	\see
 */
HUINT64 			SSTR_HEURISTICS_GetBandwidth (SSTR_HEURISTICS_t *hHeuristics);

/*! 
 *	\brief ������ Heuristics State ��ȯ
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return SSTR_HEUR_STATE_e
 *	\see
 */
SSTR_HEUR_STATE_e	SSTR_HEURISTICS_GetHeuristicsState (SSTR_HEURISTICS_t *hHeuristics);

/*! 
 *	\brief ������ Heuristics State ����
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

