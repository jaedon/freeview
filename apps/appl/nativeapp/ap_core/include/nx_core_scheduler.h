/*
 * (c) 2011-2015 Humax Co., Ltd.
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

/******************************************************************************/
/**
 * @file	  		nx_core_scheduler.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_CORE_SCHEDULER_H__
#define __NX_CORE_SCHEDULER_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>
#include <clib.h>

#include <nx_common.h>
#include <nx_core_message.h>
#include <nx_port.h>
#include <nx_core_metadata_programmes.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/

/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/
#define	NX_CORE_SCHEDULE_SUPPORTED_SCHEDULE_NUM						CONFIG_APUI_SET_RESERVATION_MAX_SLOT		// 지원되는 최대 schedule num
#define	NX_CORE_SCHEDULE_SLOTID_NULL								0xFFFF


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef enum
{
	eNxSchedle_ModeRecord = 0,
	eNxSchedle_ModeWatching,
	eNxSchedle_ModePowerOn,
	eNxSchedle_ModePowerOff,
	eNxSchedle_ModeUnKnow
} Nx_ScheduleMode_t;

typedef struct
{
	APK_META_SCH_Conflict_e	eConflictType;
	DxSchedule_t	*pstTriedSchedule;

	HINT32			listNum;
	HxVector_t		*conflictList;
}NX_ScheConflict_t;

typedef struct
{
	NX_MetaSched_Handle_t	schedule;
	NX_ScheConflict_t	stConflict;

} NX_Scheduler_t;

/*******************************************************************/
/********************	Private Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/
HERROR 	NX_SCHEDULER_Init(void);
NX_ScheConflict_t	*NX_SCHEDULER_GetConflictList(HUINT32 ulSlot);
HERROR	NX_SCHEDULER_CancelAllScheduleByType(DxRsvType_e eRsvType);
HERROR	NX_SCHEDULER_StopAllScheduleByType(DxRsvType_e eRsvType);
HERROR	NX_SCHEDULER_AddSchedule(DxSchedule_t *pstSchedule);
HERROR	NX_SCHEDULER_CancelSchedule(HUINT32 ulSlot);
HERROR	NX_SCHEDULER_StopSchedule(HUINT32 ulSlot);
HERROR	NX_SCHEDULER_UpdateSchedule(DxSchedule_t *pstSchedule);

HERROR	NX_SCHEDULER_CreateScheduleWithProgrammeInfo(DxRsvType_e eRsvType, NX_Programme_t *srcProgramme, DxSchedule_t *dstSchedule);
HERROR	NX_SCHEDULER_CreateScheduleWithTime(DxRsvType_e eRsvType, HUINT32 ulSvcUid, HINT32 nLcn, HUINT32 ulStartTime, HUINT32 ulDuration, HINT32 nVolume, DxSchedule_t *dstSchedule);
HERROR	NX_SCHEDULER_SetPowerOn(HUINT32 ulSvcUid, HUINT32 ulWakeupTime, HINT32  nRepeatDays, HINT32 nVolume);
HERROR	NX_SCHEDULER_SetPowerOff(HUINT32 ulWakeupTime, HINT32  nRepeatDays);
HBOOL	NX_SCHEDULER_IsSeries(DxSchedule_t *pstSchedule, HUINT32 *ulSeriesId);
HERROR	NX_SCHEDULER_GetSeriveUidBySlotID(HUINT32 ulSlotId, HUINT32 *pulSvcUid);
HERROR	NX_SCHEDULER_GetEventIDBySlotID(HUINT32 ulSlotId, HINT32 *plEventId);
HERROR	NX_SCHEDULER_GetDuration(DxSchedule_t *pstSchedule, HUINT32 *ulDuration);
HERROR	NX_SCHEDULER_GetDurationWithDataTime(DxSchedule_t *pstSchedule, HxDATETIME_t *stDateTime);
HERROR	NX_SCHEDULER_CountScheduleAll(HUINT32 *pulNumSch);
HERROR	NX_SCHEDULER_CountSchedule(HUINT32 *pulNumSch);
HERROR	NX_SCHEDULER_CountScheduleByType(DxRsvType_e eType, HUINT32 *pulNumSch);
HERROR	NX_SCHEDULER_GetScheduleBySlotID(HUINT32 ulSlotId, DxSchedule_t *pstSchedule);
HERROR	NX_SCHEDULER_GetScheduleList(HxVector_t *stScheduleList, HINT32 *lNum);
HERROR	NX_SCHEDULER_GetScheduleListByType(DxRsvType_e eType, HxVector_t *stScheduleList, HINT32 *lNum);
HERROR	NX_SCHEDULER_GetScheduleListBySeriesIdAndServiceUid(HUINT32 ulSeriesId, HUINT32 ulSvcUid, HxVector_t *stScheduleList, HINT32 *lNum);
HERROR	NX_SCHEDULER_GetScheduleListByProgrammeInfo(NX_Programme_t *pstProgramme, HxVector_t *pstScheduleList, HINT32 *lNum);

#endif // __NX_CORE_SCHEDULER_H__


