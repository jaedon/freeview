/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_commontypes.h
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

#ifndef __SAMA_COMMONTYPES_H__
#define __SAMA_COMMONTYPES_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <rlib.h>

/*******************************************************************/
/********************      Declaration         *************************/
/*******************************************************************/
#define SAMA_TIME_AVAIL_MIN				(3 * 365 * 24 * 60 * 60)	// 3 Year:1974.1.1
#define SAMA_MAX_ALTERNATIVE			10							// APKD_META_SCHED_MAX_CONFLICT

// 예약 시도시의 padding conflict 는 무시하고 그냥 예약 걸고, 예약 시간이 되었을 때 다시한번 check 하여 padding conflict이면 padding을 날린다.
#define SAMA_LAZY_PADDING_CONFLICT_REMOVE	1
#define	SAMA_CHANGE_PADDING_TIME_IN_SAMA	1

#if  !defined(CONFIG_OP_UK_DTT) && !defined(CONFIG_OP_NORDIG)
#define SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST	1
#endif


#define SAMA_DO_NOT_RESTART_RUNNING_SCHEDUEL_AFTER_BOOT	1


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


// TODO: 추후 정리되어야 할 type들.
//현재 어쩔 수 없이 samalib이 봐야할 type들만 옮겨왔다.
//추후 가능하다면 sama_lib.h 가 이 header를 include 하지 않도록 수정해야 한다.
//이 header에는 오로지 type 들만 넣어둔다.

#define SAMA_PADDING_READY_TIME	(20)

#if defined( CONFIG_OP_NORDIG )
#define SAMA_PADDING_STOP_TIME	(1)
#else
#define SAMA_PADDING_STOP_TIME	(7)
#endif

#define SAMA_CHECK_SERIES_RECORD_READY_TIME	(SAMA_PADDING_READY_TIME+20)



typedef enum
{
	eSAPI_TIMER_TYPE_None					= 0,

	eSAPI_TIMER_TYPE_Ready					= 1,
	eSAPI_TIMER_TYPE_Start					= 2,
	eSAPI_TIMER_TYPE_Stop					= 3,

	// SAMA_LAZY_PADDING_CONFLICT_REMOVE에만 사용.
	//SAMA 내부에서만 사용됨. 즉, notify 되지 않는다.
	eSAPI_TIMER_TYPE_PaddingStartReady		= 4,
	eSAPI_TIMER_TYPE_PaddingStopReady		= 5,

	// Series record check 하는 시점을 결정.
	//SAMA 내부에서만 사용됨. 즉, notify 되지 않는다.
	eSAPI_TIMER_TYPE_CheckSeriesRecordReady	= 6,

	// For TimeLineCheck:, SAMA 내부에서만 사용됨. 즉, notify 되지 않는다.
	eSAPI_TIMER_TYPE_TL_PaddingStart		= 7,
	eSAPI_TIMER_TYPE_TL_Start				= 8,
	eSAPI_TIMER_TYPE_TL_Stop				= 9,
	eSAPI_TIMER_TYPE_TL_PaddingEnd			= 10,

	// Stop과 동일하게 취급한다. notify 시에는 stop으로 변경하여 notify 한다.
	eSAPI_TIMER_TYPE_StopPendingBroken		= 11,
	eSAPI_TIMER_TYPE_StopRunaway			= 12,

	// Base에서 Ready는 Live Tuner conflict에서 사용되어야 하므로, tracking ready를 따로 만든다.
	eSAPI_TIMER_TYPE_ReadyTracking			= 13,

	// tracking 중인 schedule의 endtime 쯤에 시간을 계속 update 해준다. notify 하지 않는다.
	eSAPI_TIMER_TYPE_UpdateDelayed			= 14,


	eSAPI_TIMER_TYPE_EndOfCase
} SAPI_TimerItemType_e;

typedef enum
{
	eSAPI_ACTION_TYPE_None,

	eSAPI_ACTION_TYPE_LiveView,
	eSAPI_ACTION_TYPE_Record,

	eSAPI_ACTION_TYPE_NetStreaming,

	eSAPI_ACTION_TYPE_MAX
} SAPI_ActionType_e;


#if defined(CONFIG_ENHANCED_SECURITY)
typedef enum
{
	eSAPI_UNIQUE_PROCESS_Indicator_None			= 0,

	eSAPI_UNIQUE_PROCESS_Indicator_Sama,
	eSAPI_UNIQUE_PROCESS_Indicator_Obama,
	eSAPI_UNIQUE_PROCESS_Indicator_Homma,
	eSAPI_UNIQUE_PROCESS_Indicator_Corsair,
	eSAPI_UNIQUE_PROCESS_Indicator_Ipepg,
	eSAPI_UNIQUE_PROCESS_Indicator_Nova,

	eSAPI_UNIQUE_PROCESS_Indicator_Unknown

} SAPI_UniqueProcessIndicator_e;
#endif


#if 0
typedef enum
{
	eDxSchedule_Spec_None  = 0,

	eDxSchedule_Spec_Base,

	eDxSchedule_Spec_EndOfCase
} SAPI_Spec_e;
#endif

typedef struct
{
	DxSchedule_Spec_e			 eSpec;

	HUINT32				 ulShutdownWaitTime;
} SAPI_SpecOption_t;


typedef enum
{
	eSAPI_CONFLICT_TYPE_None,

	eSAPI_CONFLICT_TYPE_Tp,
	eSAPI_CONFLICT_TYPE_OverCapacity,
	eSAPI_CONFLICT_TYPE_View,
	eSAPI_CONFLICT_TYPE_Duplicated,
	eSAPI_CONFLICT_TYPE_PaddingTime,
	eSAPI_CONFLICT_TYPE_Alternative,
	eSAPI_CONFLICT_TYPE_RfOta,
	eSAPI_CONFLICT_TYPE_DataOnly,
	eSAPI_CONFLICT_TYPE_Unknown,

	eSAPI_CONFLICT_TYPE_EndOfCase
} SAPI_ConflictType_e;

// 새로 시도학 예약 기준으로 생각. 즉, 새로 추가될 예약의 pre 가 remove 되어야 한다면 0x1로 올려줌.
// 이 부분은 SAPI_ConflictType_e 로 들어가도 되지만 APK enum 도 추가해도 될지 판단 후 옮긴다.
typedef enum
{
	eSAPI_PADDING_CONFLICT_TYPE_None				= 0x0,

	eSAPI_PADDING_CONFLICT_TYPE_PaddingTimePre		= 0x1,
	eSAPI_PADDING_CONFLICT_TYPE_PaddingTimePost	= 0x2,

	eSAPI_PADDING_CONFLICT_TYPE_PaddingTimeBoth	= 0x3,

	eSAPI_PADDING_CONFLICT_TYPE_EndOfCase			= 0xf
} SAPI_PaddingConflictType_e;

typedef enum
{
	eSAPI_SYSTEM_TIME_CHANGED_TYPE_None				= 0,

	eSAPI_SYSTEM_TIME_CHANGED_TYPE_First_Available,

	eSAPI_SYSTEM_TIME_CHANGED_TYPE_EndOfCase		= 0xFFFFFFFF
} SAPI_SystemTimeChangedType_e;


typedef struct
{
	HUINT32						 ulSlot;			// 충돌난 기존의 Schedule Slot
} SAPI_ConflictItem_t;

typedef struct
{
	DxSchedule_t				 stTriedSchedule;	// 새로 예약을 시도한 Schedule 내용:
	SAPI_ConflictType_e			 eType;				// 충돌 내역:
	SAPI_PaddingConflictType_e	ePaddingConflictType;  // SAPI_ConflictType_e로 들어가게 되면 삭제할 것.

	HUINT32						 ulNumConflict, ulConflictArraySize;
	SAPI_ConflictItem_t			*pstConflictArray;
} SAPI_Conflict_t;

typedef struct
{
	HUINT32						ulSlot;				// 충돌 나고, alternative가 존재하는 기존의 schedule slot
	HUINT32						ulAltEvtId;
	HUINT32						ulAltSvcUid;
} SAPI_AlternativeItem_t;

typedef struct
{
	HUINT32						ulNumAlternatives;
	SAPI_AlternativeItem_t		astAltItemArray[SAMA_MAX_ALTERNATIVE];
} SAPI_AlternativeItems_t;

typedef struct
{
	SAPI_Conflict_t			stSapiConflict;
	SAPI_AlternativeItems_t	stAltItems;
} SAPI_Alternative_t;

typedef struct
{
	HINT32				 anActionNum[eSAPI_ACTION_TYPE_MAX];
	HINT32				 anMaxActionNum[eSAPI_ACTION_TYPE_MAX];
	HINT32				 anRscNum[eRxRSCTYPE_MAX];

	RxRfInfoCapacity_t	 astRfInfoCap[RxRFINPUT_NUM_MAX];
	DxDeliveryType_e	 aeTunerSupportDeliType[RxTUNER_NUM_MAX];
} SAPI_RscInfo_t;

typedef struct
{
	HBOOL				bStartPaddingChanged;
	HUINT32				ulStartPadding;
	HBOOL				bEndPaddingChanged;
	HUINT32				ulEndPadding;
	HBOOL				bReadyChanged;
	HUINT32				ulReady;
} SAPI_ScheduleChanged_t;

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

#ifdef __cplusplus
}
#endif
#endif	// __SAMA_COMMONTYPES_H__
