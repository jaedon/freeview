/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sapi.h
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

#ifndef __SAPI_H__
#define __SAPI_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <hlib.h>
#include <dlib.h>
#include <rlib.h>

#include <sama_commontypes.h>
/*******************************************************************/
/********************      Declaration         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/






typedef enum
{
	eSAPI_NOTIFY_TYPE_None			= 0,

	eSAPI_NOTIFY_TYPE_SchedulesLoaded,
	eSAPI_NOTIFY_TYPE_ScheduleAdded,
	eSAPI_NOTIFY_TYPE_ScheduleChanged,
	eSAPI_NOTIFY_TYPE_ScheduleRemoved,
	eSAPI_NOTIFY_TYPE_TimeUp,

	eSAPI_NOTIFY_TYPE_ConflictScheduleAdded,

	// 추후 사양에 따른 특수 Notify는 밑에 넣는다.
	eSAPI_NOTIFY_TYPE_AlternativeScheduleAdded,
	eSAPI_NOTIFY_TYPE_SystemTimeChanged,

	eSAPI_NOTIFY_TYPE_EndOfCase		= 0xFFFFFFFF
} SAPI_NotifyType_e;


// LXC의 경우 nRequestPid 와 SAPI_UniqueProcessIndicator_e 를 비교해야함.
// 다른 컨테이너에 쌓인 process 가 같은 PID 를 가지는 경우가 있으므로, 그에 대한 대비책.
typedef struct
{
	SAPI_NotifyType_e		 eNotifyType;
	HUINT32					 ulSchNum;
	DxSchedule_t			*pstSchArray;
	HUINT16				nRequestPid;
} SAPI_NotiInfo_SchLoaded_t;

typedef struct
{
	SAPI_NotifyType_e		 eNotifyType;
	HUINT32					 ulSlotId;
	DxSchedule_t			*pstSchedule;
	HUINT16				nRequestPid;
} SAPI_NotiInfo_SchAdded_t;

typedef struct
{
	SAPI_NotifyType_e		 eNotifyType;
	HUINT32					 ulSlotId;
	DxSchedule_t			*pstSchedule;
	HUINT16				nRequestPid;
} SAPI_NotiInfo_SchChanged_t;

typedef struct
{
	SAPI_NotifyType_e		 eNotifyType;
	HUINT32					 ulSlotId;
	DxSchedule_t			*pstSchedule;
	HUINT16				nRequestPid;
} SAPI_NotiInfo_SchRemoved_t;

typedef struct
{
	SAPI_NotifyType_e		 eNotifyType;
	HUINT32					 ulSlotId;
	SAPI_TimerItemType_e	 eTimerType;
	DxSchedule_t			*pstSchedule;
} SAPI_NotiInfo_TimeUp_t;

typedef struct
{
	SAPI_NotifyType_e		 eNotifyType;
	SAPI_Conflict_t			 stConflicted;
	HUINT16				nRequestPid;
} SAPI_NotiInfo_ConfSchAdded_t;

typedef struct
{
	SAPI_NotifyType_e		 eNotifyType;
	SAPI_Alternative_t		 stAlternative;
	HUINT16				nRequestPid;
} SAPI_NotiInfo_AltSchAdded_t;

typedef struct
{
	SAPI_NotifyType_e		eNotifyType;
	SAPI_SystemTimeChangedType_e	eSystemTimeChangedType;
} SAPI_NotiInfo_SystemTimeChanged_t;

typedef union
{
	SAPI_NotifyType_e				 eNotifyType;
	SAPI_NotiInfo_SchLoaded_t		 stSchLoaded;
	SAPI_NotiInfo_SchAdded_t		 stSchAdded;
	SAPI_NotiInfo_SchChanged_t		 stSchChanged;
	SAPI_NotiInfo_SchRemoved_t		 stSchRemoved;
	SAPI_NotiInfo_TimeUp_t			 stTimeUp;
	SAPI_NotiInfo_ConfSchAdded_t	 stConfSchAdded;
	SAPI_NotiInfo_AltSchAdded_t		 stAltSchAdded;
	SAPI_NotiInfo_SystemTimeChanged_t	 stSystemTimeChanged;
} SAPI_NotiInfo_t;

typedef HERROR (*SAPI_NotiFunc_t) (void *pvUserData, SAPI_NotiInfo_t *pstNotify);



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
extern HERROR	SAPI_RegisterNotifier (void *pvUserData, SAPI_NotiFunc_t pfNotifier);
extern HERROR	SAPI_UnregisterNotifier (void *pvUserData, SAPI_NotiFunc_t pfNotifier);
extern HERROR	SAPI_GetResourceInfo (SAPI_RscInfo_t *pstRscInfo);
extern HERROR	SAPI_SetResourceInfo (SAPI_RscInfo_t *pstRscInfo);
extern HERROR	SAPI_SetRfInputAbility (HUINT32 ulRfInputId, HBOOL bAbility);
extern HERROR	SAPI_GetSpecOptions (SAPI_SpecOption_t *pstSpec);
extern HERROR	SAPI_GetSchedule (HUINT32 ulSlotId, DxSchedule_t *pstSchedule);
extern HERROR	SAPI_GetScheduleByIndex (HUINT32 ulIndex, DxSchedule_t *pstSchedule);
extern HERROR	SAPI_GetScheduleByType (DxRsvType_e eType, HUINT32 ulIndex, DxSchedule_t *pstSchedule);
extern HERROR	SAPI_CountSchedule (HUINT32 *pulNumSch);
extern HERROR	SAPI_CountScheduleByType (DxRsvType_e eType, HUINT32 *pulNumSch);
extern HERROR SAPI_GetAllSchedulesByRsvType (HUINT32 ulArrayMax, HUINT32 *pulNumSch, DxSchedule_t *pstSchArray, DxRsvType_e eRsvType);
extern HERROR	SAPI_GetEarliestSchedule (DxSchedule_t *pstSchedule);
extern HERROR	SAPI_GetEarliestScheduleWithPaddingAndReadyTime (DxSchedule_t *pstSchedule, UNIXTIME *putStartTimeWithPaddingAndReady);
extern HERROR	SAPI_GetAllSchedules (HUINT32 ulArrayMax, HUINT32 *pulNumSch, DxSchedule_t *pstSchArray);
extern HERROR	SAPI_GetAvailableSlot (HUINT32 *pulSlotId);
extern HERROR	SAPI_MakeSchedule (HUINT32 ulSlotId, DxSchedule_t *pstSchedule, SAPI_Conflict_t *pstConflict);
extern HERROR SAPI_CheckScheduleConflict (HUINT32 ulSlotId, DxSchedule_t *pstSchedule, SAPI_Conflict_t *pstConflict);
extern HERROR	SAPI_UpdateSchedule (HUINT32 ulSlotId, DxSchedule_t *pstSchedule, SAPI_Conflict_t *pstConflict, HBOOL bDisableConflictCheck, HBOOL bDisableConflictNotify);
extern HERROR	SAPI_CancelSchedule (HUINT32 ulSlotId);
extern HERROR	SAPI_ChangeSeriesScheduleToNext (HUINT32 ulSlotId);
extern HERROR	SAPI_CancelWholeSeries (HUINT32 ulSlotId);
extern HERROR	SAPI_StopSchedule (HUINT32 ulSlotId);
extern HERROR	SAPI_PauseSchedule (HUINT32 ulSlotId);
extern HERROR	SAPI_ResumeSchedule (HUINT32 ulSlotId);
// 상위 eRemoveReason에 대한 sub eDetailRemovedInfo가 필요할 경우. 향 별로 다양한 값을 쓸 수 있도록. 어차피 값을 가지고 판단하는 쪽은 web ap가 될 것이다.
extern HERROR SAPI_IncompleteSchedule (HUINT32 ulSlotId, DxSchedule_RemovedInfo_t *pstRemovedInfo);
extern HERROR	SAPI_GetConflictResult (HUINT32 ulSlotId, SAPI_Conflict_t *pstConflict);
extern HERROR SAPI_ChangePaddingTime(HUINT32 ulOffsetTime, HBOOL bIsPreOffset);
extern HERROR SAPI_GetAlternativeResult (HUINT32 ulSlotId, SAPI_Alternative_t *pstAlternative);
extern HERROR	SAPI_UpdateSeries (void);
extern HERROR	SAPI_GetValidCrid (HUINT8 *szSrcCridStr, HUINT32 ulSrcCridLen, HUINT8 *szDefaultAuth, HUINT8 *szValidCridStr);
extern HUINT32 SAPI_GetExtraBootTime(void);
extern HBOOL	SAPI_IsEnableTBRPadding(void);


extern void	SAPI_FindScheduleFilterInit(DxSchedule_FindFilterAttr_t	*pstFilter);
// 이 함수 call 하면 SAPI_FindScheduleUsingFilterFreeList()로 free 해줘야 함.!!
extern HERROR	SAPI_FindScheduleUsingFilter(DxSchedule_FindFilterAttr_t *pstFilter, HUINT32 *pulNumSch, DxSchedule_t **ppstSchArray);
extern void	SAPI_FindScheduleUsingFilterFreeList(DxSchedule_t *pstSchArray);
extern HERROR SAPI_GetPaddingStartEndTime(DxSchedule_t *pstSchedule, UNIXTIME *putStartTime, UNIXTIME *putEndTime);
extern SAPI_ActionType_e SAPI_GetSapiActionTypeBySchedule (DxSchedule_t *pstSchedule);
extern HBOOL SAPI_IsScheduleLoaded(void);
extern void SAPI_RefreshScheduleData (void);
extern HERROR	SAPI_Wait (HUINT32 ulSlot, HUINT32 timeout);
#ifdef CONFIG_DEBUG
extern HERROR SAPI_DbgPrintSchedule(DxSchedule_t *pstSchedule);
#endif


#ifdef __cplusplus
}
#endif
#endif	// __SAPI_H__
