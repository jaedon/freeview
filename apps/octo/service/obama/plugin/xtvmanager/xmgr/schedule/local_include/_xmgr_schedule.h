/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_schedule.h
	@brief

	Description:  									\n
	Module: OBAMA / plugin / tvmanager / schedule		\n

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

#ifndef __XMGR_SCHEDULE_BASE_H__
#define __XMGR_SCHEDULE_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Declaration         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT32			 ulSlot;
	HUINT32			 ulActionId;
	Handle_t		 hRingBuf;
} XmgrSch_RunningRec_t;

typedef struct
{
	HUINT32			 ulSlot;
} XmgrSch_ReadyRec_t;

typedef struct
{
	HxList_t		*pstRunRecList;				// 	XmgrSch_RunningRec_t
	HxList_t		*pstReadyRecList;			// XmgrSch_ReadyRec_t
} XmgrSch_BaseContext_t;


typedef struct
{
	HUINT32			ulSlot;
	Handle_t		hSvc;
	UNIXTIME		utStartTime;
	UNIXTIME		utEndTime;
} XmgrSch_TrackingSch_t;

typedef struct
{
	XmgrSch_BaseContext_t	stBaseContext;
	HxList_t				*pstTrackingSchList;	// XmgrSch_TrackingSch_t
	HxList_t				*pstReadyRecList;		// XmgrSch_ReadyRec_t
} XmgrSch_TvAnyTimeContext_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Private Functions     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions       ******************/
/*******************************************************************/

/*			BASE
 */
extern HERROR					xmgr_schedule_Init_Base (void);
extern HERROR 				xmgr_schedule_CheckFrontPannelDisplay_Base(void);
extern XmgrSch_BaseContext_t*	xmgr_schedule_GetContext_Base (void);
extern HERROR					xmgr_schedule_StartLive_Base (void *pvContext, DxSchedule_t *pstSchedule);
extern HERROR					xmgr_schedule_StartMemRecord_Base (void *pvContext, DxSchedule_t *pstSchedule);
extern HERROR					xmgr_schedule_StopMemRecord_Base (void *pvContext, DxSchedule_t *pstSchedule);
extern HERROR					xmgr_schedule_StartRecord_Base (void *pvContext, DxSchedule_t *pstSchedule);
extern HERROR					xmgr_schedule_StopRecord_Base (void *pvContext, DxSchedule_t *pstSchedule);
extern HERROR					xmgr_schedule_RecordStopPadding_Base (void *pvContext, DxSchedule_t *pstSchedule);

extern BUS_Result_t				xmgr_schedule_MsgTimeUp_Base (void *pvContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t				xmgr_schedule_MsgSatIpChangePid_Base (void *pvContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t				xmgr_schedule_MsgNtlsChangePid_Base (void *pvContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t				xmgr_schedule_MsgHlsChangePlayPosition (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

extern BUS_Result_t				xmgr_schedule_MsgDelayedRecordSetupResult_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t				xmgr_schedule_MsgRecordSetupFail_Base (void *pvContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t				xmgr_schedule_MsgSatipStartFailed_Base (void *pvContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t				xmgr_schedule_MsgNtlsStartFailed_Base (void *pvContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t				xmgr_schedule_MsgPvrPlaybackStop_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t				xmgr_schedule_MsgNtlsStopped_Base (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

extern HERROR 					xmgr_schedule_SetRecordFailReason_Base (HUINT32 ulSlot, DxSchedule_RemovedReason_e eRemovedReason, HUINT32 eDetailedInfo);
extern BUS_Result_t 				xmgr_schedule_MsgRecordSetupMaxInstance_Base(void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern void						xmgr_schedule_reminder_base(void);
#if defined(CONFIG_DEBUG)
extern HINT32 xmgr_schedule_base_get(void *szArgList);
extern HINT32 xmgr_schedule_base_start(void *szArgList);
extern HINT32 xmgr_schedule_base_stop(void *szArgList);
extern HINT32 xmgr_schedule_base_startMediaServer(void *szArgList);
extern HINT32 xmgr_schedule_base_makeTBRSchedule(void *szArgList);
#endif

/*			SES
 */
extern HERROR 	xmgr_schedule_SetScheduleInCancelDB_Ses(HUINT32 nRequestID, DxSchedule_t *pstSchedule);
extern HERROR	xmgr_schedule_RescheduleBurstUpdate_ses(void);
extern HERROR	xmgr_schedule_CheckConflictSchedule_ses(void);

/*			Tv-AnyTime
*/
extern HERROR					xmgr_schedule_Init_tva (void);
extern XmgrSch_TvAnyTimeContext_t * xmgr_schedule_GetContext_tva (void);
extern Handle_t					xmgr_schedule_GetEarliestTrackingSvc_tva (void);
extern HERROR					xmgr_schedule_MakeRegularOta_tva (void);
extern HERROR					xmgr_schedule_CheckConflictSchedule_tva (void);

extern BUS_Result_t				xmgr_schedule_MsgTimeUp_tva (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t				xmgr_schedule_MsgUpdateNcdSchedule_tva (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
extern BUS_Result_t				xmgr_schedule_MsgScheduleEventOccured_tva (void *pvContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
/*			CIPLUS BASE
 */
extern HERROR  xmgr_schedule_CiPlusStartDescRecordFinedItem_Base(DxRecListData_t	*pstRecData);
extern HERROR		xmgr_schedule_CiPlusRescheduleDescrambleRecordBatch_Base(void);
#ifdef __cplusplus
}
#endif
#endif	// __XMGR_SCHEDULE_BASE_H__
