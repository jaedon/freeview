/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_int_TvAnyTime.h
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
#ifndef __SAMA_INT_TVANYTIME_H__
#define __SAMA_INT_TVANYTIME_H__

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
// sama_protocol_tva.c
extern HERROR		SAMA_PROTOCOL_TVA_RegisterRpcProtocols (HINT32 nRpcHandle);
extern HERROR		SAMA_PROTOCOL_TVA_NotifyAlternativeAddTried (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, HINT32 nNumConflict, SAPI_ConflictItem_t *pstConfArray, SAPI_AlternativeItems_t *pstAltItems, HINT32 nRequestId);
extern HERROR		SAMA_PROTOCOL_TVA_NotifySystemTimeChanged (SAMA_Context_t *pstContext, SAPI_SystemTimeChangedType_e eSystemTimeChangedType);

// sama_schedule_tva.c
extern HERROR		SAMA_SCHEDULE_TVA_Initialize (void);
extern void 		SAMA_SCHEDULE_TVA_ProcessTwoWayTimer (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime);
extern HERROR		SAMA_SCHEDULE_TVA_InsertScheduleToTimer (DxSchedule_t *pstSchedule);
extern HERROR		SAMA_SCHEDULE_TVA_AddCurrentSeriesEvent (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, DxEvent_t *pstCurrentSeriesEvent);
extern HERROR		SAMA_SCHEDULE_TVA_AddSeriesEvents (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, SERIES_SET *pstSeriesSet, HxList_t *pstSeriesEventList);
extern HERROR		SAMA_SCHEDULE_TVA_GetEventBySchedule(DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex);
extern HERROR		SAMA_SCHEDULE_TVA_GetEventsByCrid (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, SERIES_SET *pstSeriesSet);
extern HERROR		SAMA_SCHEDULE_TVA_MakeCridSchedule (DxSchedule_t *pstSchedule);
extern HERROR		SAMA_SCHEDULE_TVA_MakeSeriesSchedules (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, SERIES_SET *pstSeriesSet);
extern HERROR		SAMA_SCHEDULE_TVA_MakeAlternativeSeriesSchedules (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, SERIES_SET *pstSeriesSet);
extern HERROR		SAMA_SCHEDULE_TVA_RemoveSchedule (HUINT32 ulSlot,DxSchedule_t *pstSchedule, DxSchedule_RemovedInfo_t *pstRemovedInfo);
extern HERROR		SAMA_SCHEDULE_TVA_UpdateSeries (void);
extern HBOOL		SAMA_SCHEDULE_TVA_KeepRemoveSchedule (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, DxSchedule_RemovedReason_e eRemoveReason);
extern HERROR		SAMA_SCHEDULE_TVA_UpdateRecordedProgramme (HUINT32 ulContentId, HBOOL bIsLoadAll);
extern HERROR		SAMA_SCHEDULE_TVA_TrackEitPf (HUID* uidlist, HUINT32 size);
extern HERROR		SAMA_SCHEDULE_TVA_UpdateEitPf (HUID* uidlist, HUINT32 size);
extern HERROR		SAMA_SCHEDULE_TVA_UpdateEitSc (HUID* uidlist, HUINT32 size);
extern HERROR		SAMA_SCHEDULE_TVA_ResumeSchedule (HUINT32 ulSlot);


#endif	// __SAMA_INT_TVANYTIME_H__