/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_meta_schedule.h
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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

#ifndef ___APK_META_SCHEDULE_H___
#define ___APK_META_SCHEDULE_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define APKD_META_SCHED_MAX_ALTERNATIVE		10	//APKD_META_SCHED_MAX_CONFLICT
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eAPK_METASCH_MSG_None			= 0,

	eAPK_METASCH_MSG_SchedulesUpdated,
	eAPK_METASCH_MSG_ScheduleAdded,
	eAPK_METASCH_MSG_ScheduleChanged,
	eAPK_METASCH_MSG_ScheduleRemoved,
	eAPK_METASCH_MSG_TimeUpReady,
	eAPK_METASCH_MSG_TimeUpStart,
	eAPK_METASCH_MSG_TimeUpStop,
	eAPK_METASCH_MSG_ConflictScheduleAdded,
	eAPK_METASCH_MSG_ConflictInReady,
	eAPK_METASCH_MSG_AlternativeScheduleAdded,

	eAPK_METASCH_MSG_EndOfCase		= 0xFFFFFFFF
} APK_META_SCH_MsgType_e;

typedef enum
{
	eAPK_METASCH_CONFTYPE_None,

	eAPK_METASCH_CONFTYPE_Tp,
	eAPK_METASCH_CONFTYPE_OverCapacity,
	eAPK_METASCH_CONFTYPE_View,
	eAPK_METASCH_CONFTYPE_Duplicated,
	eAPK_METASCH_CONFTYPE_PaddingTime,
	eAPK_METASCH_CONFTYPE_Alternative,
	eAPK_METASCH_CONFTYPE_RfOta,
	eAPK_METASCH_CONFTYPE_DataOnly,
	eAPK_METASCH_CONFTYPE_Unknown,

	eAPK_METASCH_CONFTYPE_EndOfCase
} APK_META_SCH_Conflict_e;

typedef struct
{
	HUINT32					 ulSlot;

} APK_META_SCH_ConfItem_t;

typedef struct
{
	APK_META_SCH_Conflict_e	 eType;
	DxSchedule_t			 stTriedSchedule;
	HUINT32					 ulNumConflict;
	APK_META_SCH_ConfItem_t	*pstConfArray;
} APK_META_SCH_Conflict_t;

typedef struct
{
	HUINT32					ulSlot;
	HUINT32					ulAltEvtId;
	HUINT32					ulAltSvcUid;
} APK_META_SCH_AlternativeItem_t;

typedef struct
{
	HUINT32							ulNumAlternatives;
	APK_META_SCH_AlternativeItem_t	astAltItemArray[APKD_META_SCHED_MAX_ALTERNATIVE];
} APK_META_SCH_AlternativeItems_t;
typedef struct
{
	APK_META_SCH_Conflict_t			stConfResult;
	APK_META_SCH_AlternativeItems_t	stAltItems;
} APK_META_SCH_Alternative_t;

typedef void (*APK_META_SCH_NotiFunc_t) (void *pvUserData, APK_META_SCH_MsgType_e eMsgType, HUINT32 ulSlot, DxSchedule_t *pstSchedule);

typedef struct _META_SCHEDULER_t_* ApkMetaSched_Handle_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

HERROR	APK_META_SCHEDULE_RegisterNotifier (APK_META_SCH_NotiFunc_t pfNotifier, void *pvUserData);
HERROR	APK_META_SCHEDULE_UnregisterNotifier (APK_META_SCH_NotiFunc_t pfNotifier, void *pvUserData);
HERROR	APK_META_SCHEDULE_GetSchedule (HUINT32 ulSlot, DxSchedule_t *pstSchedule);
HERROR	APK_META_SCHEDULE_GetScheduleByIndex (HUINT32 ulIndex, DxSchedule_t *pstSchedule);
HERROR	APK_META_SCHEDULE_GetScheduleByType (DxRsvType_e eType, HUINT32 ulIndex, DxSchedule_t *pstSchedule);
HERROR	APK_META_SCHEDULE_CountSchedule (HUINT32 *pulNumSch);
HERROR	APK_META_SCHEDULE_CountScheduleByType (DxRsvType_e eType, HUINT32 *pulNumSch);
HERROR	APK_META_SCHEDULE_AddSchedule (DxSchedule_t *pstSchedule, HUINT32 *pulSlot);
HERROR	APK_META_SCHEDULE_UpdateSchedule (HUINT32 ulSlot, DxSchedule_t *pstSchedule);
HERROR	APK_META_SCHEDULE_UpdateSeries (void);
HERROR	APK_META_SCHEDULE_CancelSchedule (HUINT32 ulSlot);
HERROR	APK_META_SCHEDULE_StopSchedule (HUINT32 ulSlot);
HERROR	APK_META_SCHEDULE_PauseSchedule (HUINT32 ulSlot);
HERROR	APK_META_SCHEDULE_ResumeSchedule (HUINT32 ulSlot);
HERROR	APK_META_SCHEDULE_RemoveWholeSeries (ApkMetaSched_Handle_t hMeta, HINT32 uid);

extern APK_META_SCH_Conflict_t*	APK_META_SCHEDULE_GetConflictResult (HUINT32 ulSlot);
extern APK_META_SCH_Alternative_t *APK_META_SCHEDULE_GetAlternativeResult (HUINT32 ulSlot);
extern void						APK_META_SCHEDULE_FreeConflictResult (APK_META_SCH_Conflict_t *pstConflict);
extern void						APK_META_SCHEDULE_FreeAlternativeResult (APK_META_SCH_Alternative_t *pstAlternative);


//////////////////////////////////////
//  구 appkit
/////////////////////////////////////
//	deprecated
ApkMetaSched_Handle_t		APK_META_SCHEDULE_GetInstance (void);
HERROR			APK_META_SCHEDULE_Refresh (ApkMetaSched_Handle_t hMeta);
HBOOL			APK_META_IsEnableTBRPadding(void);
DxSchedule_Spec_e	APK_META_SCHEDULE_GetSpec(void);
HERROR			APK_META_SCHEDULE_Confirm (ApkMetaSched_Handle_t hMeta, HINT32 uid, HINT32 type, HINT32 check);

HERROR			APK_META_SCHEDULE_RemoveFirstItemOnSeries (ApkMetaSched_Handle_t hMeta, HINT32 uid);
HBOOL			APK_META_SCHEDULE_SetName (HINT32 sched_uid, HCHAR *name);

#endif
/* end of file */
