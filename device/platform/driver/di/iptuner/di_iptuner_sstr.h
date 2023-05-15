/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   di_iptuner_sstr.h  $
 * Version:			1.5 (for OCTO 1.0 with 40nm)
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	     SSTR Manifest Controller for Streaming Header
 * Module:
 * Remarks:	
 */	

/**
 * @ingroup SSTR_VFIO_CONTROLLER
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
 * @file di_iptuner_sstr.h
 */

#ifndef __DI_IPTUNER_SSTR_H__
#define __DI_IPTUNER_SSTR_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include "sstr_controller.h"
#include "di_iptuner.h"
/* End Including Headers*/


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */
/* End Extern variable */

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
#define DI_IPTUNER_SSTR_CONST_SLEEP_FOR_WFD (10) /* sleep for 'WaitForDecision' */
#define DI_IPTUNER_SSTR_CONST_MP4BOX_BUFFERSIZE (1024 * 4) /* 4kbytes */
#define DI_IPTUNER_SSTR_CONST_RETRY_COUNT_ON_NET_ERROR 3
//#define DI_IPTUNER_SSTR_CONST_MAX_BANDWIDTH (1000000000) /* 1Gbps */
#define DI_IPTUNER_SSTR_CONST_RECV_BLOCKSIZE (120 * 1024)
#define DI_IPTUNER_SSTR_CONST_DEFAULT_FRAGBUFF (3 * 1024 * 1024) /* 3MB */
#define DI_IPTUNER_SSTR_CONST_MAX_STREAMS 8 /*  refer DI_MEDIA */
#define DI_IPTUNER_SSTR_CONST_LIVE_STARTDELAY 300000000 /* */
// #define DI_IPTUNER_SSTR_CONFIG_AUDIO_FIRST /* not supported (do not use this define)*/
// #define DI_IPTUNER_SSTR_CONFIG_MULTI_SOCKET  /* with DI_IPTUNER_SSTR_CONFIG_MULTI_AUDIOTRACK */ /* not supported(do not use this define */ 
// #define DI_IPTUNER_SSTR_CONFIG_TEXTSTREAM

// #ifdef USE_IPTUNER_SSTR
/** Seconds for Buffering */
#ifndef STREAM_SSTR_BUFFERING_SECS 
#define SSTR_CONFIG_BUFFERING_SECS (4)
#else
#define SSTR_CONFIG_BUFFERING_SECS STREAM_SSTR_BUFFERING_SECS
#endif
/** Maximum BufferSize */
#ifndef STREAM_SSTR_MAX_BUFFER
#define SSTR_CONFIG_MAX_BUFFER (5*1024*1024) 
#else
#define SSTR_CONFIG_MAX_BUFFER (STREAM_SSTR_MAX_BUFFER * 1024 * 1024)
#endif
/** Minimum BufferSize */
#ifndef STREAM_SSTR_MIN_BUFFER 
#define SSTR_CONFIG_MIN_BUFFER (2*1024*1024) 
#else
#define SSTR_CONFIG_MIN_BUFFER (STREAM_SSTR_MIN_BUFFER * 1024 * 1024) 
#endif
//#endif
/* endif USE_IPTUNER_SSTR */

/* End Macro definition */


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/** 
 * SSTR BUFFERING TASK�� State
 */
typedef enum {
	DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_READY,  /**< READY ����, Default  */
	DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_STARTED, /**< STARTED ����, Buffering ����  */
	DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_WAITING, /**< WAITING ����, Buffering �� ���� ��� */
	DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_KILLING, /**< KILLING ����, Buffering ������ */
	DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_DONE /**< DONE ����, Buffering ���� */
} DI_IPTUNER_SSTR_BUFFERING_TASK_STATE_e;

/** 
 * SSTR TEXTSTREAM TASK�� State
 */
typedef enum {
	DI_IPTUNER_SSTR_TEXT_TASK_STATE_READY,  /**< READY ����, Default  */
	DI_IPTUNER_SSTR_TEXT_TASK_STATE_STARTED, /**< STARTED ����, Download ����  */
	DI_IPTUNER_SSTR_TEXT_TASK_STATE_FLUSHING, /**< FLUSHING ���� */
//	DI_IPTUNER_SSTR_TEXT_TASK_STATE_KILLING, /**< KILLING ����, Download ������ */
	DI_IPTUNER_SSTR_TEXT_TASK_STATE_DONE /**< DONE ����, Download ���� */
} DI_IPTUNER_SSTR_TEXT_TASK_STATE_e;

/** 
 * SSTR Download Queue TASK�� State
 */
typedef enum {
	DI_IPTUNER_SSTR_QUEUE_TASK_STATE_READY,  /**< READY ����, Default  */
	DI_IPTUNER_SSTR_QUEUE_TASK_STATE_STARTED, /**< STARTED ���� */
	DI_IPTUNER_SSTR_QUEUE_TASK_STATE_FLUSHING, /**< FLUSHING ���� */
	DI_IPTUNER_SSTR_QUEUE_TASK_STATE_DONE /**< DONE ���� */
} DI_IPTUNER_SSTR_QUEUE_TASK_STATE_e;


/** 
 * SSTR LIVE UPDATE TASK�� State
 * ���� ������ (�̿�)
 */
typedef enum {
	DI_IPTUNER_SSTR_LIVE_UPDATE_READY, /**< READY ����, Default  */
	DI_IPTUNER_SSTR_LIVE_UPDATE_STARTED, /**< STARTED ����, Update Task ����  */
	DI_IPTUNER_SSTR_LIVE_UPDATE_UPDATING, /**< UPDATING ����, Update ��  */
	DI_IPTUNER_SSTR_LIVE_UPDATE_TERMINATED, /**< TERMINATED ����, Update ���� */
} DI_IPTUNER_SSTR_LIVE_UPDATE_e;

typedef void* SSTR_HANDLE;

/* End typedef */


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/** HTTP STREAM VFIO Register/Unregister */

/*! 
 *	\brief VFIO Callback �� ����Ѵ�.
 *	\return void
 *	\see 
 */
void DI_STREAM_SSTR_RegisterCallback (void);

/*! 
 *	\brief VFIO Callback �� �����Ѵ�.
 *	\return void
 *	\see 
 */
void DI_STREAM_SSTR_UnregisterCallback (void);

/*! 
 *	\brief SSTR Buffering ���� 
 *	\param pHSession : SESSION HANDLE
 *	\return errcode
 *	\see 
 */
int DI_STREAM_SSTR_PreBuffering (DI_IPTUNER_Index_e nTunerIndex);

/*! 
 *	\brief SSTR Live Update �� ���� Task ���� 
 *	\param pHSession : SESSION HANDLE
 *	\return errcode
 *	\see 
 */
int DI_STREAM_SSTR_CreateLiveUpdateTask (void);

/*! 
 *	\brief SSTR Live Update �� ���� Task ���� 
 *	\param pHSession : SESSION HANDLE
 *	\return errcode
 *	\see 
 */
int DI_STREAM_SSTR_DestroyLiveUpdateTask (void);

/*! 
 *	\brief Text Stream Download �� ���� Task ���� 
 *	\param pHSession : SESSION HANDLE
 *	\return errcode
 *	\see 
 */
int DI_STREAM_SSTR_CreateTextStreamTask (void);

/*! 
 *	\brief Text Stream Download �� ���� Task ���� 
 *	\param pHSession : SESSION HANDLE
 *	\return errcode
 *	\see 
 */
int DI_STREAM_SSTR_DestroyTextStreamTask (void);

/*! 
 *	\brief SSTR DownloadQueue �� ���� Task ���� 
 *	\param pHSession : SESSION HANDLE
 *	\return errcode
 *	\see 
 */
int DI_STREAM_SSTR_CreateQueueTask (void);

/*! 
 *	\brief SSTR DownloadQueue �� ���� Task ���� 
 *	\param pHSession : SESSION HANDLE
 *	\return errcode
 *	\see 
 */
int DI_STREAM_SSTR_DestroyQueueTask (void);

/*! 
 *	\brief SSTR Set Settings
 *	\param pHSession : SESSION HANDLE
 *	\param pSettings : Settings
 *	\return errcode
 *	\see 
 */
// SSTR_ERR DI_STREAM_SSTR_SetSettings(SSTR_HANDLE hSstr, DI_IPTUNER_AdaptiveSettings_t *pSettings);

/*! 
 *	\brief SSTR Control ( Server Control )
 *	\param pHSession : SESSION HANDLE
 *	\param cmd : Control Command
 *	\param val : Control Value
 *	\param val2 : Control Value 2
 *	\return errcode
 *	\see 
 */
DI_ERR_CODE DI_STREAM_SSTR_Control (DI_IPTUNER_CMD_e cmd, HUINT64 val, HUINT64 val2 );

/*! 
 *	\brief Manifest �� �̿��Ͽ� DI MEDIA Setup�� ���� ������ ä���.
 *	\param hSstr : SSTR HANDLE
 *	\param cmd : Control Command
 *	\param pstMediaInfo : DI MEDIA Info
 *	\return errcode
 *	\see 
 */
SSTR_ERR	DI_STREAM_SSTR_GetMediaInfo (DI_MEDIA_INFO_t *pstMediaInfo);

/*! 
 *	\brief SSTR CONTROLLER ����
 *	\param pUrl : Manifest Url
 *	\return errcode
 *	\see 
 */
int DI_STREAM_SSTR_OpenController (HINT8 *pUrl);

/*! 
 *	\brief SSTR CONTROLLER ����
 *	\return errcode
 *	\see 
 */
int DI_STREAM_SSTR_CloseController (void);

/*! 
 *	\brief Set Adaptive Settings
 *	\param hSstr : SSTR HANDLE
  *	\param pSettings : Adaptive Settings 
 *	\return errcode
 *	\see 
 */
SSTR_ERR DI_STREAM_SSTR_SetSettings(SSTR_HANDLE hSstr, DI_IPTUNER_AdaptiveSettings_t *pSettings);

/*! 
 *	\brief Get Adaptive Settings
 *	\param hSstr : SSTR HANDLE
  *	\param[out] pSettings : Adaptive Settings
 *	\return errcode
 *	\see 
 */
SSTR_ERR DI_STREAM_SSTR_GetSettings(SSTR_HANDLE hSstr, DI_IPTUNER_AdaptiveSettings_t *pSettings);

#if 0
//int DI_STREAM_SSTR_Open  (const HINT8 *path, const HINT8 *mode, VFIO_Handle_t *ptHandle );
//int DI_STREAM_SSTR_Close (VFIO_Handle_t tHandle);

/* external API */
// int DI_STREAM_SSTR_SetTimeForSeek(DI_IPTUNER_t *pTunerHandle, HUINT64 ullTime, DI_MEDIA_TIME_TYPE eTimeType);
#endif

#if 0
void DI_STREAM_SSTR_RegisterEventCallback (DI_IPTUNER_AdaptiveCallback_t *pCallback);
void DI_STREAM_SSTR_UnregisterEventCallback ();
#endif
#if 0
int DI_STREAM_SSTR_InitTextStreamTask (DI_IPTUNER_Index_e nTunerIndex);
int DI_STREAM_SSTR_DestroyTextStreamTask (DI_IPTUNER_Index_e nTunerIndex);
#endif

#if 1
HUINT32 DI_STREAM_SSTR_GetBufferingState(void);
void DI_STREAM_SSTR_SetBufferingState(HUINT32 nState);
void DI_STREAM_SSTR_StopBufferingTask(void);

int DI_STREAM_SSTR_SetProperty(DI_IPTUNER_ADAPTIVE_PROPERTY_e eCmd, DI_IPTUNER_ADAPTIVE_PROPERTY_DATA_t *pData);
int DI_STREAM_SSTR_GetProperty(DI_IPTUNER_ADAPTIVE_PROPERTY_e eCmd, DI_IPTUNER_ADAPTIVE_PROPERTY_DATA_t *pData);

DI_IPTUNER_SSTR_Info_t * DI_STREAM_SSTR_GetManifestInfo(void);
#endif

/*! 
 *	\brief Get starting time (from the loading manifest file to the decoding media)
 *	\param void
 *	\return time value(UNIT: ms), In case of error to return 0.
 *	\see 
 */
HULONG DI_STREAM_SSTR_TimerGetStartingTime(void);

/*! 
 *	\brief Get Loading time (from the loading manifest file to the parsing manifest file)
 *	\param void
 *	\return time value(UNIT: ms), In case of error to return 0.
 *	\see 
 */
HULONG DI_STREAM_SSTR_TimerGetLoadingTime(void);

/*! 
 *	\brief Get Bandwidth for the downloading fragment
 *	\param void
 *	\return Byte per second, In case of error to return 0.
 *	\see 
 */
HUINT32 DI_STREAM_SSTR_TimerGetBandwidth(void);

/*! 
 *	\brief Get max Bandwidth for the downloading fragment
 *	\param void
 *	\return KByte per second, In case of error to return 0.
 *	\see 
 */
HUINT32 DI_STREAM_SSTR_TimerGetMaxBandwidth(void);

/*! 
 *	\brief Get latest downloaded fragment Index
 *	\param void
 *	\return KByte per second, In case of error to return 0.
 *	\see 
 */
HUINT32 DI_STREAM_SSTR_GetCurFragmentIndex(void);

/*! 
 *	\brief Get latest downloaded fragment start time
 *	\param void
 *	\return ms, In case of error to return 0.
 *	\see 
 */
HUINT64 DI_STREAM_SSTR_GetCurFragmentTime(void);

/*! 
 *	\brief Get current bitrate
 *	\param void
 *	\return In case of error to return 0.
 *	\see 
 */
HUINT32 DI_STREAM_SSTR_GetCurBitrate(void);

/*! 
 *	\brief Get current Fragment URL
 *	\param char** pUrl	for return string of url
 *	\return return error code.
 *	\see 
 */
DI_ERR_CODE DI_STREAM_SSTR_GetCurFragmentUrl(char** pUrl);

/*! 
 *	\brief Get Buffering count
 *	\param void
 *	\return buffering count
 *	\see 
 */
HUINT32 DI_STREAM_SSTR_GetBufferingCount(void);

/*! 
 *	\brief Get Buffering time
 *	\param void
 *	\return buffering time
 *	\see 
 */
HUINT64 DI_STREAM_SSTR_GetBufferingTime(void);
#endif /* ! __DI_IPTUNER_SSTR_H__ */


