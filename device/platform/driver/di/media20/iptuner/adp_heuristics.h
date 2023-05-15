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
 * Heuristics ����� ���� Callback Functions
 */
typedef HUINT32 (*ADP_HEUR_GetCurDuration) (void *hCtrl); /**< Fragment Duration ��ȯ */
typedef HUINT32 (*ADP_HEUR_GetCurBitrate) (void *hCtrl); /**< ���� Bitrate ��ȯ */
typedef HBOOL (*ADP_HEUR_IsContentProtected) (void *hCtrl); /**< DRM ���� ��ȯ */
typedef HBOOL (*ADP_HEUR_GearDown) (void *hCtrl); /**< ���� Bitrate ���� �� �ܰ� ���� Switching */
typedef HBOOL (*ADP_HEUR_GearUp) (void *hCtrl); /**< ���� Bitrate ���� �� �ܰ� �Ʒ��� Switching */
typedef HBOOL (*ADP_HEUR_GearSet) (void *hCtrl, HUINT32 ulBitrate); /**< Ư�� Bitrate �� Switching */
typedef HBOOL (*ADP_HEUR_GearLowest) (void *hCtrl); /**< ���� Bitrate �� Switching */
typedef HBOOL (*ADP_HEUR_GearHighest) (void *hCtrl); /**< �ְ� Bitrate �� Switching */


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
	ADP_HEUR_STATE_TIMID, /**< TIMID ����, Network �ӵ� �������� Bitrate Switching ���� */
	ADP_HEUR_STATE_GREEDY, /**< GREEDY ����, Network �ӵ��� Bufferfullness üũ�Ͽ� Bitrate Switching ���� */
	ADP_HEUR_STATE_PANIC, /**< PANIC ����, ���� Bitrate�� Bitrate Switching */
	ADP_HEUR_STATE_UNKNOWN	
} ADP_HEUR_STATE_e;

/** 
 * ADP HEURISTICS STARTUP MODE
 */
typedef enum
{
	ADP_HEUR_STARTUP_LOWEST, /**< ���� Bitrate�� Startup */
	ADP_HEUR_STARTUP_HIGHEST, /**< �ְ� Bitrate�� Startup */
	ADP_HEUR_STARTUP_AUTO, /**< AUTO Mode (����� ���� Bitrate�� Startup) */
	ADP_HEUR_STARTUP_MANUAL /**< ������ Track ���� ���� */
} ADP_HEUR_STATRTUP_MODE_e;

/** 
 * ADP HEURISTICS DECISION MODE
 */
typedef enum
{
	ADP_HEUR_DECISION_AUTO, /**< Heuristics Module ���� Algorithm�� ���� Bitrate-Switching */
	ADP_HEUR_DECISION_MANUAL, /**< User ���ÿ� ���� Bitrate-Switching (not supported yet) */
	ADP_HEUR_DECISION_FIXED /**< Bitrate-Switching ���� ���� */
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
	HUINT64 ullSampleSize; /**< SAMPLE DATA ũ�� (Fragment ũ��) */
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

	HBOOL bBehaviorReset; /**< Reset �� Decision �� Auto �� ��� Reset �� �ʱ�ȭ ���� */

	/* Controller Callbacks */
	ADP_HEUR_GetCurBitrate cbCurBitrate; /**< ���� Bitrate ��ȯ */
	ADP_HEUR_GetCurDuration cbCurDuration; /**< Fragment Duration ��ȯ */
	ADP_HEUR_IsContentProtected cbIsProtected; /**< DRM ���� ��ȯ */
	ADP_HEUR_GearUp cbGearUp;  /**< ���� Bitrate ���� �� �ܰ� ���� Switching */
	ADP_HEUR_GearDown cbGearDown; /**< ���� Bitrate ���� �� �ܰ� �Ʒ��� Switching */
	ADP_HEUR_GearSet cbGearSet; /**< Ư�� Bitrate �� Switching */
	ADP_HEUR_GearLowest cbGearLowest; /**< ���� Bitrate �� Switching */
	ADP_HEUR_GearHighest cbGearHighest; /**< �ְ� Bitrate �� Switching */

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
} ADP_HEURISTICS_CONFIG_t;

/** 
 * ADP HEURISTICS HANDLE 
 */
typedef struct ADP_HEURISTICS_s
{	
	ADP_HEUR_STATE_e eState; /**< Heuristics State(TIMID,GREEDY,PANIC) */
	ADP_HEURISTICS_CONFIG_t tConfig; /**< Configuration */
	HBOOL	bInit;  /**< Initialize ���� */
	HBOOL	bLocked; /**< Bitrate Lock ���� */
	HUINT64 ullLockedBandwidth; /**< Lock �� Bitrate */
	HUINT64 ullLockedDelay; /**< Lock �� Download Delay */
	
	HUINT64 ullPairBandwidth; /**< �� ���� Fragment�� ������ Bandwidth */
	HINT32 nCursor;  /**< SAMPLE Array Cursor */
	HINT32 nSize; /**< ���� SAMPLE �� */
	HUINT64 ullBandwidth; /**< ��� Bandwidth */
	ADP_HEURISTICS_Sample_t tSample[ADP_HEURISTICS_CONFIG_WINDOWSIZE]; /**< Sample Array */
} ADP_HEURISTICS_t;

typedef ADP_HEURISTICS_t* ADP_HEURISTICS_HANDLE_t;

/* End typedef */
/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

/* Start global function prototypes */

/*! 
 *	\brief Heuristics �ʱ�ȭ (Config �� Controller Callbacks Setting)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config �� Controller Callbacks
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR 		ADP_HEURISTICS_Init (ADP_HEURISTICS_HANDLE_t hHeuristics, ADP_HEURISTICS_CONFIG_t *pConfig);

/*! 
 *	\brief Config ���� (Controller Callbacks ����)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR 		ADP_HEURISTICS_SetConfig (ADP_HEURISTICS_HANDLE_t hHeuristics, ADP_HEURISTICS_CONFIG_t *pConfig);

/*! 
 *	\brief Config ��ȯ (Controller Callbacks ����)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param pConfig : Config
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR 		ADP_HEURISTICS_GetConfig (ADP_HEURISTICS_HANDLE_t hHeuristics, ADP_HEURISTICS_CONFIG_t *pConfig);

/*! 
 *	\brief Default Config ��ȯ (Controller Callbacks ����)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ePreset : ADP_HEUR_CONFIG_PRESET_e (SSTR/HLS/...)
 *	\param pConfig : Config
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR		ADP_HEURISTICS_GetDefaultConfig (ADP_HEURISTICS_HANDLE_t hHeuristics, ADP_HEUR_CONFIG_PRESET_e ePreset, ADP_HEURISTICS_CONFIG_t *pConfig);

/*! 
 *	\brief Heuristics �ʱ�ȭ (Mode, Sample, Bandwidth ...)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return void
 *	\see
 */
void 				ADP_HEURISTICS_Reset (ADP_HEURISTICS_HANDLE_t hHeuristics);

/*! 
 *	\brief Heuristics ���� (DECISION MODE�� AUTO �� �ƴϸ� Bitrate-Switching ���� ����)
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ullFragSize : Fragment ũ��
 *	\param ullMs : Download Delay
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR		ADP_HEURISTICS_Run(ADP_HEURISTICS_HANDLE_t hHeuristics, HUINT64 ullFragSize, HUINT64 ullMs);

/*! 
 *	\brief Pair Bandwidth ��ȯ
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return Pair Bandwidth 
 *	\see
 */
HUINT64				ADP_HEURISTICS_GetPacketPairBandwidth (ADP_HEURISTICS_HANDLE_t hHeuristics);

/*! 
 *	\brief ���� �Է�
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\param ullFragSize : Fragment ũ��
 *	\param ullMs : Download Delay
 *	\return ADP_HEUR_ERR
 *	\see
 */
ADP_HEUR_ERR 		ADP_HEURISTICS_AddSample (ADP_HEURISTICS_HANDLE_t hHeuristics, HUINT64 ullSampleSize, HUINT64 ullMs);

/*! 
 *	\brief ��� Bandwidth ��ȯ
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return ��� Bandwidth 
 *	\see
 */
HUINT64 			ADP_HEURISTICS_GetBandwidth (ADP_HEURISTICS_HANDLE_t hHeuristics);

/*! 
 *	\brief ������ Heuristics State ��ȯ
 *	\param hHeuristics : HEURISTICS HANDLE
 *	\return ADP_HEUR_STATE_e
 *	\see
 */
ADP_HEUR_STATE_e	ADP_HEURISTICS_GetHeuristicsState (ADP_HEURISTICS_HANDLE_t hHeuristics);

/*! 
 *	\brief ������ Heuristics State ����
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

