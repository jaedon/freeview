/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_media_rec.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __OAPI_MEDIAREC_H__
#define __OAPI_MEDIAREC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MEDIAREC_STORAGEID_LEN					63

// APPKIT -> OBAMA
#define RPC_OAPI_MEDIA_REC_Init				"oapi.media.rec.Init"
#define RPC_OAPI_MEDIA_REC_Start			"oapi.media.rec.Start"
#define RPC_OAPI_MEDIA_REC_Stop				"oapi.media.rec.Stop"
#define RPC_OAPI_MEDIA_REC_TsrStart			"oapi.media.rec.TsrStart"
#define RPC_OAPI_MEDIA_REC_TsrStop			"oapi.media.rec.TsrStop"

// OBAMA -> APPKIT
//#define RPC_OAPI_MEDIA_REC_OnInit				"obama.media.rec.OnInit"
#define RPC_OAPI_MEDIA_REC_OnStarted			"obama.media.rec.OnStarted"
#define RPC_OAPI_MEDIA_REC_OnStopped			"obama.media.rec.OnStopped"
#define RPC_OAPI_MEDIA_REC_OnStateChanged		"obama.media.rec.OnStateChanged"
#define RPC_OAPI_MEDIA_REC_OnTimecodeChanged	"obama.media.rec.OnTimecodeChanged"
#define RPC_OAPI_MEDIA_REC_OnEventNotify		"obama.media.rec.OnEventNotify"
#define RPC_OAPI_MEDIA_REC_OnTsrBufferCopyEnd	"obama.media.rec.OnTsrBufferCopyEnd"
#define RPC_OAPI_MEDIA_REC_OnMaxInstanceChanged "obama.media.rec.OnMaxInstanceChanged"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eOxMR_MEDIATYPE_NONE			= 0,

	eOxMR_MEDIATYPE_PVRREC,
	eOxMR_MEDIATYPE_TSRREC,

	eOxMR_MEDIATYPE_ENDOFCASE
} OxMediaRec_MediaType_e;

typedef enum
{
	eOxMR_EVENT_NONE				= 0,

	eOxMR_EVENT_REC_STARTED,
	eOxMR_EVENT_REC_STOPPED,
	eOxMR_EVENT_REC_TIME_CHANGED,
	eOxMR_EVENT_REC_STATE_CHANGED,

	eOxMR_EVENT_REC_CONTENTS_CHANGED,
	eOxMR_EVENT_REC_TSRBUFFER_COPYEND,

	eOxMR_EVENT_ENDOFCASE
} OxMediaRec_Event_e;

typedef enum
{
	eOxMR_RECSTATE_NONE		= 0,

	eOxMR_RECSTATE_STOPPING,
	eOxMR_RECSTATE_STOPPED,
	eOxMR_RECSTATE_REQUESTED,
	eOxMR_RECSTATE_CONNECTING,
	eOxMR_RECSTATE_RECORDING,

	eOxMR_RECSTATE_ENDOFCASE
} OxMediaRec_RecState_e;

typedef enum
{
	eOxMR_TargetStorage_Manual = 0,
	eOxMR_TargetStorage_IntHdd,
	eOxMR_TargetStorage_IntSubHdd,
	eOxMR_TargetStorage_ExtHdd
} OxMediaRec_TargetStorage_e;


typedef struct
{
	HUINT32						 ulMasterSvcUid;
	HUINT32						 ulSuppleSvcUid;
	OxMediaPlay_SuppleSvcType_e	 eSuppSvcType;

	HINT32						 nStartOffset;				// -1: Normal Record, >=0 : Delayed Record
	HxDATETIME_t				 stStartTime;
	HUINT32						 ulDuration;

	OxMediaRec_TargetStorage_e	 eStorageType;
	HCHAR						 szStorageId[MEDIAREC_STORAGEID_LEN + 1];

	HINT32						 nEvtId;
	HCHAR						 szName[DxNAME_LEN];
} OxMediaRec_StartInfo_t;

typedef union
{
	OxMediaRec_StartInfo_t		 stRecStarted;				// eOxMR_EVENT_REC_STARTED
} OxMediaRec_NotifierData_t;

typedef struct
{
	HUINT32						 ulMaxRec;
	HUINT32						 ulMaxRecId;
	HUINT32						 ulMaxTsrNum;
} OxMediaRec_Capability_t;

typedef void (*OxMediaRec_Notifier_t) (
											HUINT32 ulSvcUid,
											HUINT32 ulSlot,
											OxMediaRec_Event_e eEvent,
											OxMediaRec_NotifierData_t *pstNotifyData,
											void *pvUserData
											);

typedef void (*OxMediaRec_ActivatedCb_t) (void);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
extern HERROR	OAPI_MEDIA_REC_Init (OxMediaRec_ActivatedCb_t pfActivated);
extern HERROR	OAPI_MEDIA_REC_IsRecording (HUINT32 ulMasterSvcUid, HBOOL *pIsRecording);
extern HERROR	OAPI_MEDIA_REC_GetCID (HUINT32 ulMasterSvcUid, HUINT32 *pCID);
extern HERROR	OAPI_MEDIA_REC_GetCapability (OxMediaRec_Capability_t *pstCap);
extern HERROR	OAPI_MEDIA_REC_SetListener (OxMediaRec_Notifier_t pfListener, void *pvUserData);
extern HERROR	OAPI_MEDIA_REC_ResetListener (OxMediaRec_Notifier_t pfListener, void *pvUserData);
extern HERROR	OAPI_MEDIA_REC_Start (OxMediaRec_StartInfo_t *pstStart, HUINT32 ulSlotId);
extern Handle_t	OAPI_MEDIA_REC_StartTsr (OxMediaRec_StartInfo_t *pstStart);
extern HERROR	OAPI_MEDIA_REC_Stop (HUINT32 ulSlotId);
extern HERROR	OAPI_MEDIA_REC_GetCount (HINT32 *pRecCount);
extern HERROR	OAPI_MEDIA_REC_GetSVCUid (HUINT32 ulSessionId, HUINT32 *pulSvcUid);
extern HERROR	OAPI_MEDIA_REC_GetStartTime (HUINT32 ulSessionId, HUINT32 *pulStartTime);
extern HERROR	OAPI_MEDIA_REC_GetRecTime (HUINT32 ulSessionId, HUINT32 *pulRecTime);
extern HERROR	OAPI_MEDIA_REC_GetContentId (HUINT32 ulSessionId, HUINT32 *pnContentId);
#endif // __OAPI_MEDIAREC_H__
