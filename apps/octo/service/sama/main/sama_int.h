/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_int.h
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
#ifndef __SAMA_INT_H__
#define __SAMA_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <sapi.h>
#include <dapi.h>
#include <dlib.h>
#include <papi.h>

#include <sama_lib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#define SAMA_DEFAULT_HDD_UUID		37


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

// TODO: 정리.
typedef	enum
{
	SAMAMESSAGE_NONE,

	SAMAMESSAGE_RPCCALLRECEIVED,

	// series rec 관련 동작은, dapi query 를 하기 때문에, data 날라오는 시간 동안 sama thread를 세울 순 없고,, msg 형식으로 처리..
	SAMAMESSAGE_CHECK_SERIES_RECORD, // p1: slot of previeous series item
	SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL,
	SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED,
#ifdef SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST
	SAMAMESSAGE_CHECK_SAME_SERIES_RECORD_EXIST_RESULT,
#endif
	SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM,
	SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE,
	// UKDTT를 위한 message. series 관련 동작이 dapi callback에서 일어나는데, 이 안에서 dapi를 못 부르므로 msg를 추가 한다.
	SAMAMESSAGE_CHECK_SERIES_DONE,
	SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED_DONE,



	// sama main thread가 아니라 uapi task 에서의 event 이므로, msg 처리하여 sama main thread에서 처리하기 위함.
	SAMAMESSAGE_CHECKPVRREADYSTATUS,

	// sama main thread가 아니라 sama timer task 에서의 event 이므로, msg 처리하여 sama main thread에서 처리하기 위함.
	SAMAMESSAGE_PROCESSONEWAYTIMER,
	SAMAMESSAGE_PROCESSTWOWAYTIMER,

	// sama main thread가 아니라 sama timer task 에서의 event 이므로, msg 처리하여 sama main thread에서 처리하기 위함.
	SAMAMESSAGE_PROCESSAFTERSYSTEMTIMEAVAILABLE,

	// sama main thread가 아니라 hapi task 에서의 event 이므로, msg 처리하여 sama main thread에서 처리하기 위함.
	SAMAMESSAGE_PROCESSHAPISYSTEMSTATECHANGED,

	// sama main thread가 아니라 oapi task 에서의 event 이므로, msg 처리하여 sama main thread에서 처리하기 위함.
	SAMAMESSAGE_PROCESSOAPIRECCHANGED,

	// sama main thread가 아니라 oapi task 에서의 event 이므로, msg 처리하여 sama main thread에서 처리하기 위함.
	SAMAMESSAGE_PROCESSDAPIEITPFCHANGED,

	// sama main thread가 아니라 oapi task 에서의 event 이므로, msg 처리하여 sama main thread에서 처리하기 위함.
	SAMAMESSAGE_PROCESSDAPIEITSCCHANGED,

	SAMAMESSAGE_DEFAULTHDDINFOCHANGED,

	SAMAMESSAGE_ANTENA_UPDATED,

	SAMAMESSAGE_MAX
} SAMA_Message_t;

typedef	struct	tagMessageType
{
	SAMA_Message_t		message;
	HINT32				p1;
	HINT32				p2;
	HINT32				p3;
	HINT32				p4;
	HINT32				p5;
	HINT32				p6;

} Sama_InternalMsg_t;


typedef	struct
{
	DxSchedule_t			*pstSchedule; // pointer만 달고 다니는 놈.

	HUINT32 				svcuid;
	HCHAR				tripleid[32];

	HUINT32 				ulSeason;
	HUINT32 				ulEpisodeNum;
	HUINT32 				ulTotalNum;

	HBOOL				bExistSameSeriesItem;
} Sama_SameSeriesRecListCheckData_t;

typedef struct
{
	HINT32			nClientSock;
	HUINT32			ulRequestId;
	void				*pvData;
	HUINT32			ulSize;
} Sama_RpcProtocolJob_t;

typedef struct
{
	HUINT32				ulSlot;
	DxEvent_t				stEvent;
}Sama_SeriesRecordEventInfo_t;



typedef struct
{

	HUINT16				nSamaPid;

	SAPI_SpecOption_t	 stSpecOption;

	HBOOL			 bSama_ScheduleLoaded;
	HBOOL			 bSama_PvrDeviceMounted;
	HBOOL			 bSama_TvaInited;
	HCHAR			 ucSama_DefaultHddUuid[SAMA_DEFAULT_HDD_UUID];
	HUINT32			 ulSama_DefaultHddMaxRec;


	sama_ConflictCheck_t		stSamaConflictCheck;

//	HxList_t			*pSeriesRecordNextEventList;
} SAMA_Context_t;

typedef void (*SAMA_CbTimerExec) (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime);

typedef	struct tag_sama_timer_item
{
	HUINT32					 ulSlot;
	unsigned long			 ulTime;
	SAPI_TimerItemType_e	 eItemType;
	SAMA_CbTimerExec		 pfCbFunc;
	HBOOL					 bPause;
	DxRsvType_e				 eRsvType;

	// Context Free List에서 보관하기 위한 Link: Alloc/Free 외에 사용하면 안된다!
	struct tag_sama_timer_item	*pstNextItem;
} SAMA_TimerItem_t;

typedef struct
{
	HUINT32					 ulAbsDiffTime;				// 시간 변한 크기 (항상 양수)
	HINT32					 nDiffSign;					// 시간 변한 정도 ( -1 : 줄어들음 1: 늘어남 )

	HBOOL 					bFirstTimeUpdateAfterBoot;
	HBOOL					bSync;

	HxList_t				*pstToBeRemoveSched;		// 시간 변화에 따라 삭제될 Schedules
} SAMA_TimerUpdateInfo_t;



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/
// sama.c
extern HERROR		SAMA_GetRpcHandle (HINT32 *pnRpcHandle);
extern SAMA_Context_t*	SAMA_GetContext (void);
extern HINT32	SAMA_GetVirtualRequestId(void);
extern HERROR SAMA_PostMessage(HINT32 message, HINT32 p1, HINT32 p2, HINT32 p3, HINT32 p4, HINT32 p5, HINT32 p6);

// sama_timer.c
extern HERROR	SAMA_TIMER_ProcessAfterSystemTimeAvailable(HUINT32 ulAbsDiff, HINT32 nDiffSign, HBOOL bFirstTimeUpdateAfterBoot);
extern HERROR SAMA_TIMER_ProcessHapiSystemStateChanged (PxSTATUS_e ePapiStatus);
extern HERROR			SAMA_TIMER_InitTimer (void);
extern HERROR 		SAMA_TIMER_GetTimerTime (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long *pulTime);
extern HERROR 			SAMA_TIMER_InsertTimer (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, DxRsvType_e eRsvType, unsigned long ulTime, SAMA_CbTimerExec pfCallback, HBOOL bPause);
extern HERROR			SAMA_TIMER_RemoveTimer (HUINT32 ulSlot);
extern HERROR			SAMA_TIMER_ExecuteTimer (void);
extern HUINT32			SAMA_TIMER_CountRemainingTimer (HUINT32 ulSlot);
extern HERROR			SAMA_TIMER_ChangeTimerTime (DxSchedule_t *pstSchedule);
extern HERROR			SAMA_TIMER_ForEach (SAMA_ForEachResult_e (*pfCbFunc)(SAMA_TimerItem_t *pstTimer, HINT32 nArgc, HUINT32 *pulArgv), HINT32 nArgc, HUINT32 *pulArgv);
#if defined(CONFIG_DEBUG)
extern HERROR			SAMA_TIMER_PrintAllTimer (HERROR (*pfPrintFunc)(const HCHAR *format, ...));
extern HERROR SAMA_TIMER_PrintLogAllTimer (void);
#endif


// sama_schedule.c
extern HERROR			SAMA_SCHEDULE_Initialize (void);
extern void 			SAMA_SCHEDULE_ProcessTwoWayTimer (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime);
extern HBOOL 			SAMA_SCHEDULE_KeepRemoveSchedule (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, DxSchedule_RemovedReason_e eRemoveReason);
extern HBOOL 			SAMA_SCHEDULE_IsSeriesRecord (DxSchedule_t *pstSchedule);
extern HERROR SAMA_SCHEDULE_GetSeriesRecordCheckTime (DxSchedule_t *pstSchedule, UNIXTIME *putTime);
#ifdef SAMA_SERIES_RECORD_CHECK_SAME_RECORD_ITEM_EXIST
extern HERROR SAMA_SCHEDULE_CheckSameSeriesRecordExist (DxSchedule_t *pstSchedule, HUINT32 ulSeason, HUINT32 ulEpisodeNum, HUINT32 ulTotalNum);
#endif
extern HERROR SAMA_SCHEDULE_MakeNextSeriesRecord (DxSchedule_t *pstSchedule, HxList_t *pSeriesRecordNextEventList, HBOOL bExistSameSeriesItem);
extern HERROR 		SAMA_SCHEDULE_GetNextSeriesRecord (DxSchedule_t *pstSchedule, HUINT32 ulSeason, HUINT32 ulEpisodeNum, HUINT32 ulTotalNum, HBOOL bExistSameSeriesItem);
extern HERROR 		SAMA_SCHEDULE_FindCurrentSeriesRecordInfo (DxSchedule_t *pstSchedule, HUINT32 p2);
extern HxTREE_t 		*SAMA_SCHEDULE_GetScheduleTree (void);
extern HINT32			SAMA_SCHEDULE_TreeValueToList (HxTREE_t *tree, HxList_t **list);
extern HINT32			SAMA_SCHEDULE_TreeRunningValueToList (HxTREE_t *tree, HxList_t **list);
extern HBOOL 			SAMA_SCHEDULE_RemovePadding(SAMA_Context_t *pstInst, HBOOL bReinsertTimer, sama_PaddingRemoval_e eUpdateFlag);
extern HERROR			SAMA_SCHEDULE_LoadScheduleData (HBOOL bIsFirstLaunch);
extern HERROR			SAMA_SCHEDULE_StartTimer (HBOOL bIsFirstLaunch);
extern HERROR 			SAMA_SCHEDULE_ReleaseSlotForSama (HUINT32 ulSlotId);
extern HERROR			SAMA_SCHEDULE_ReleaseSlotForSapi (HUINT32 ulSlotId);
extern HERROR			SAMA_SCHEDULE_GetEmptySlotForSama (HUINT32 *pulSlotId);
extern HERROR			SAMA_SCHEDULE_GetEmptySlot (HUINT32 *pulSlotId);
extern HERROR			SAMA_SCHEDULE_GetEmptySlotsForSapi (HINT32 nPid, HUINT32 *pulNumSlot, HUINT32 *pulSlotArray);
extern DxSchedule_t*	SAMA_SCHEDULE_GetSchedule (HUINT32 ulSlot);
extern HERROR			SAMA_SCHEDULE_GetAllSchedules (HUINT32 *pulNumSch, DxSchedule_t **ppstSchArray);
extern HUINT32			SAMA_SCHEDULE_CountScheduleBySvcUid (HUINT32 ulSvcUid);
extern HUINT32			SAMA_SCHEDULE_CountScheduleByType (DxRsvType_e eRsvType);
extern DxSchedule_t*	SAMA_SCHEDULE_GetScheduleBySvcUid (HUINT32 ulSvcUid, HUINT32 ulIndex);
extern DxSchedule_t*	SAMA_SCHEDULE_GetScheduleByType (DxRsvType_e eRsvType, HUINT32 ulIndex);
extern DxSchedule_t*	SAMA_SCHEDULE_GetEarliestSchedule (void);
extern DxSchedule_t *SAMA_SCHEDULE_GetEarliestScheduleWithPaddingAndReadyTime (UNIXTIME *putStartTimeWithPaddingAndReady);
extern HERROR			SAMA_SCHEDULE_AddSchedule (HUINT32 ulSlot, DxSchedule_t *pstSchedule, HBOOL bFromSama, HINT32 nRequestId);
extern HERROR			SAMA_SCHEDULE_RemoveSchedule (HUINT32 ulSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo, HBOOL bForcedRemove, HINT32 nRequestId, HBOOL bSync);
extern HERROR			SAMA_SCHEDULE_ChangeNextSeriesSchedule (HUINT32 ulSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo, HINT32 nRequestId);
extern HERROR			SAMA_SCHEDULE_StopSchedule (HUINT32 ulSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo, HINT32 nRequestId);
extern HERROR 			SAMA_SCHEDULE_UpdateScheduleEx (HUINT32 ulSlot, HBOOL bReinsertTimer, DxSchedule_t *pstSchedule, HINT32 nRequestId, HBOOL bSync, HBOOL bOnlyChangePadding);
extern HERROR			SAMA_SCHEDULE_PauseSchedule (HUINT32 ulSlot, HINT32 nRequestId);
extern HERROR			SAMA_SCHEDULE_ResumeSchedule (HUINT32 ulSlot, HINT32 nRequestId);
extern HERROR			SAMA_SCHEDULE_UpdateSeries (HINT32 nRequestId);
extern HBOOL			SAMA_SCHEDULE_IsScheduleVolatile (DxSchedule_t *pstSchedule);
extern HERROR			SAMA_SCHEDULE_AdjustTime (DxSchedule_t *pstSchedule, HBOOL *pbUpdated);
extern HERROR			SAMA_SCHEDULE_InsertScheduleToTimer (DxSchedule_t *pstSchedule);
extern HERROR			SAMA_SCHEDULE_SendAllScheduleToSapi (HINT32 nRequestId);
extern HERROR			SAMA_SCHEDULE_ForEach (SAMA_ForEachResult_e (*pfCbFunc)(DxSchedule_t *pstSchedule, HINT32 nArgc, HUINT32 *pulArgv), HINT32 nArgc, HUINT32 *pulArgv);
extern HERROR			SAMA_SCHEDULE_PrintSchedule (DxSchedule_t *pstSchedule);
extern HERROR			SAMA_SCHEDULE_StrPrintSchedule (HCHAR *szString, HUINT32 ulStrLen, DxSchedule_t *pstSchedule);
extern HERROR			SAMA_SCHEDULE_PrintSlotStatus (HERROR (*pfPrintFunc)(const HCHAR *format, ...));
extern HERROR			SAMA_SCHEDULE_PrintAllSchedules (HERROR (*pfPrintFunc)(const HCHAR *format, ...));
extern HERROR			SAMA_SCHEDULE_PrintLogAllSchedules (void);
extern HCHAR*			SAMA_SCHEDULE_GetStringOfUnixTime (UNIXTIME utTime);					// for Debugging:
extern HCHAR*			SAMA_SCHEDULE_GetStringOfDateTime (HxDATETIME_t *pstDateTime);		// for Debugging:
extern HERROR			SAMA_SCHEDULE_AddCurrentSeriesEvent (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, DxEvent_t *pstCurrentSeriesEvent);
extern HERROR			SAMA_SCHEDULE_AddSeriesEvents (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, SERIES_SET *pstSeriesSet, HxList_t *pstSeriesEventList);
extern HERROR 			SAMA_SCHEDULE_UpdateRecordedProgramme (HUINT32 ulContentId, HBOOL bIsLoadAll);
extern HERROR 			SAMA_SCHEDULE_UpdateAndTrackEitPf (HUID* uidlist, HUINT32 size);
extern HERROR 			SAMA_SCHEDULE_UpdateEitSc (HUID* uidlist, HUINT32 size);
extern HUINT32			SAMA_SCHEDULE_CountSeries (DxSchedule_t *pstSchedule);
extern HERROR 			SAMA_SCHEDULE_ChangePaddingTime (HUINT32 ulOffsetTime, HBOOL bIsPreOffset, HINT32 nRequestId);

// sama_protocol.c
extern HERROR			SAMA_PROTOCOL_RegisterRpcProtocols (HINT32 nRpcHandle);
extern HERROR			SAMA_PROTOCOL_NotifyRscInfoSet (SAPI_RscInfo_t *pstRscInfo, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifyRfInputAbilitySet (HUINT32 ulRfInputId, HBOOL bAbility, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifyAvailableSlotsChanged (HINT32 nPid, HUINT32 ulSlotNum, HUINT32 *pulSlotArray, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifySchedulesLoaded (HUINT32 ulSchNum, DxSchedule_t *pstSchArray, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifyScheduleAdded (HUINT32 ulSlot, DxSchedule_t *pstSchedule, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifyScheduleModified (HUINT32 ulSlot, DxSchedule_t *pstSchedule, HBOOL bExcuteNotifier, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifyScheduleRemoved (HUINT32 ulSlot, DxSchedule_RemovedInfo_t *pstRemovedInfo, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifySchedulePaused (HUINT32 ulSlot, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifyScheduleResumed (HUINT32 ulSlot, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifyConflictAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, HINT32 nNumConflict, SAPI_ConflictItem_t *pstConfArray, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifyTimeUp (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, DxSchedule_t *pstSchedule);
extern HERROR			SAMA_PROTOCOL_NotifyAlternativeAddTried (DxSchedule_t *pstSchedule, SAPI_ConflictType_e eType, HINT32 nNumConflict, SAPI_ConflictItem_t *pstConfArray, SAPI_AlternativeItems_t *pstAltItems, HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifySystemTimeChanged (SAPI_SystemTimeChangedType_e eSystemTimeChangedType);
extern HERROR			SAMA_PROTOCOL_NotifyResourceRefreshed (HINT32 nRequestId);
extern HERROR			SAMA_PROTOCOL_NotifyScheduleModifiedEx (HUINT32 ulSlot, SAPI_ScheduleChanged_t *pstChangedSchedule, HINT32 nRequestId);

// sama_resource.c
extern HERROR			SAMA_RSC_SetResourceInfo (SAPI_RscInfo_t *pstRscInfo, HINT32 nRequestId);
extern HERROR			SAMA_RSC_SetRfInputAbility (HUINT32 ulRfInputId, HBOOL bAbility, HINT32 nRequestId);
extern HERROR			SAMA_RSC_RefreshResource (HINT32 nRequestId);



// sama_debug.c
extern HERROR			SAMA_DBG_Init (void);

#define SAMA_SCHEDULE_UpdateSchedule(ulSlot, bReinsertTimer, pstSchedule, nRequestId, bSync)\
	SAMA_SCHEDULE_UpdateScheduleEx(ulSlot, bReinsertTimer, pstSchedule, nRequestId, bSync, FALSE)

// Deviated Headers:
#include <sama_int_base.h>

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
#include <sama_int_TvAnyTime.h>
#else

#endif

#endif	// __SAMA_INT_H__
