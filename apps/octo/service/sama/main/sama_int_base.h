/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_int_base.h
	@brief

	Description:  									\n
	Module: SAMA		\n

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
#ifndef __SAMA_INT_BASE_H__
#define __SAMA_INT_BASE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/
// sama_protocol_base.c
extern HERROR		SAMA_PROTOCOL_BASE_RegisterRpcProtocols (HINT32 nRpcHandle);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyRscInfoSet (SAMA_Context_t *pstContext, SAPI_RscInfo_t *pstRscInfo, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyRfInputAbilitySet (SAMA_Context_t *pstContext, HUINT32 ulRfInputId, HBOOL bAbility, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyAvailableSlotsChanged (SAMA_Context_t *pstContext, HINT32 nPid, HUINT32 ulSlotNum, HUINT32 *pulSlotArray, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifySchedulesLoaded (SAMA_Context_t *pstContext, HUINT32 ulSchNum, DxSchedule_t *pstSchArray, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyScheduleAdded (SAMA_Context_t *pstContext, HUINT32 ulSlot, DxSchedule_t *pstSchedule, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyScheduleModified (SAMA_Context_t *pstContext, HUINT32 ulSlot, DxSchedule_t *pstSchedule, HBOOL bExcuteNotifier, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyScheduleRemoved (SAMA_Context_t *pstContext, HUINT32 ulSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifySchedulePaused (SAMA_Context_t *pstContext, HUINT32 ulSlot, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyScheduleResumed (SAMA_Context_t *pstContext, HUINT32 ulSlot, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyConflictAddTried (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, HINT32 nNumConflict, SAPI_ConflictItem_t *pstConfArray, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyTimeUp (SAMA_Context_t *pstContext, HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, DxSchedule_t *pstSchedule);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyResourceRefreshed (SAMA_Context_t *pstContext, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_BASE_NotifyScheduleModifiedEx (SAMA_Context_t *pstContext, HUINT32 ulSlot, SAPI_ScheduleChanged_t *pstChangedSchedule, HINT32 nRequestId);

// sama_schedule_base.c
extern void 		SAMA_SCHEDULE_BASE_ProcessTwoWayTimer (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime);
extern void 		SAMA_SCHEDULE_BASE_ProcessOneWayTimer (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime);
extern HBOOL		SAMA_SCHEDULE_BASE_KeepRemoveSchedule (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, DxSchedule_RemovedReason_e eRemoveReason);
//extern void 		SAMA_SCHEDULE_BASE_CheckSeriesRecord (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule);
extern HBOOL		SAMA_SCHEDULE_BASE_IsSeriesRecord (DxSchedule_t *pstSchedule);
#ifdef SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST
extern HERROR		SAMA_SCHEDULE_BASE_CheckSameSeriesRecordExist (DxSchedule_t *pstSchedule, HUINT32 ulSeason, HUINT32 ulEpisodeNum, HUINT32 ulTotalNum);
#endif
extern HERROR		SAMA_SCHEDULE_BASE_MakeNextSeriesRecord (DxSchedule_t *pstSchedule, HxList_t *pSeriesRecordNextEventList, HBOOL bExistSameSeriesItem);
extern HERROR		SAMA_SCHEDULE_BASE_GetNextSeriesRecord (DxSchedule_t *pstSchedule, HUINT32 ulSeason, HUINT32 ulEpisodeNum, HUINT32 ulTotalNum, HBOOL bExistSameSeriesItem);
extern HERROR		SAMA_SCHEDULE_BASE_FindCurrentSeriesRecordInfo (DxSchedule_t *pstSchedule);
extern HERROR		SAMA_SCHEDULE_BASE_InsertScheduleToTimer (DxSchedule_t *pstSchedule);
extern HERROR		SAMA_SCHEDULE_BASE_CountSeries (DxSchedule_t *pstSchedule);


#endif	// __SAMA_INT_BASE_H__
