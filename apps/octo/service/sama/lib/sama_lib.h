/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_lib.h
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
#ifndef __SAMA_LIB_H__
#define __SAMA_LIB_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dapi.h>
#include <dlib.h>
#include <hapi.h>

#include <sama_commontypes.h> // should be removed.
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define	SAMA_SIGNAL_UDS					"/tmp/.sama_signal.tmp"
#define	SAMA_SIGNATURE					"sib"
#define	SAMA_EVENT_GETNAME(vEventString)	((HCHAR*)(vEventString == NULL? NULL:((DxEventString_t *)HLIB_VECTOR_First(vEventString))->string))


#define SAMA_EMPTY_SLOT_PER_PROCESS		8


#define SAMA_Malloc					HLIB_STD_MemAlloc
#define SAMA_Calloc					HLIB_STD_MemCalloc
#define	SAMA_Realloc				HLIB_STD_MemRealloc
#define	SAMA_StrDup					HLIB_STD_StrDup
#define	SAMA_MemDup					HLIB_STD_MemDup
#define SAMA_Free					HLIB_STD_MemFree
#define	SAMA_Malloc_CB				HLIB_STD_MemAlloc_CB
#define	SAMA_Free_CB				HLIB_STD_MemFree_CB
#define	SAMA_MemCpy					memcpy
#define	SAMA_MemSet					memset
#define	SAMA_Memcmp					memcmp
#define	SAMA_MAX(_a,_b)				(((_a)>(_b))? (_a):(_b))
#define	SAMA_MIN(_a,_b)				(((_a)<(_b))? (_a):(_b))



/* flags for EVENT_PERIOD */
#define SAMA_EVENT_BEING_RECORDED					0x00000001
#define SAMA_EVENT_BEING_MONITORED					0x00000002
#define SAMA_EVENT_UPDATED							0x00000004
#define SAMA_EVENT_ADDED							0x00000008	// TODO: flag로 구분할지 DxRSV_SLOT_NULL로 구분할지 정책을 정해야 한다.
#define SAMA_EVENT_BY_IP							0x00000010
#define SAMA_EVENT_BY_EIT_SC						0x00000020
#define SAMA_EVENT_BY_EIT_PF						0x00000040
#define SAMA_EVENT_BY_EIT_RUNNING_STATUS				0x00000080
#define	SAMA_EVENT_SEASON_EPISODE_SETTED			0X00000100



/* flags for INSTANCE_SET */
#define SAMA_INSTANCE_DO_NOT_FIND_ALTERNATIVE	0x1

/* flags for PROGRAM_SET */
#define SAMA_ALTERNATIVE_INSTANCE_SEARCHED		0x1
#define SAMA_MAX_SPLIT_EVENT_IN_ALT_INFO	3

/* TVA_ALTERNATIVE_INFO attribute */
#define SAMA_MAX_ALTERNATIVE_INSTANCE		20

#define MACRO_IS_TVA_SCHEDULE_TYPE(eRsvType)		(eRsvType == DxRSVTYPE_RECORD_SERIES_EBR \
													|| eRsvType == DxRSVTYPE_RECORD_EBR \
													|| eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR \
													|| eRsvType == DxRSVTYPE_WATCHING_EBR)
#define MACRO_IS_TVA_WATCH_TYPE(eRsvType)			(eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR \
														||eRsvType == DxRSVTYPE_WATCHING_EBR)
#define MACRO_IS_TVA_RECORD_TYPE(eRsvType)			(eRsvType == DxRSVTYPE_RECORD_SERIES_EBR \
													||eRsvType == DxRSVTYPE_RECORD_EBR)
#define MACRO_IS_TVA_SERIES_TYPE(eRsvType)		(eRsvType == DxRSVTYPE_RECORD_SERIES_EBR \
													||eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR)
#define MACRO_IS_TVA_SERIES_SET(pstSeriesSet)		((pstSeriesSet)->stCrid.type == SAMA_CRID_TYPE_SERIES || (pstSeriesSet)->stCrid.type == SAMA_CRID_TYPE_PROG)
#define MACRO_IS_TVA_SCHEDULE(pstRsv,pstSeriesSet)	((NULL != pstRsv) \
										&& (MACRO_IS_TVA_SCHEDULE_TYPE((pstRsv)->eRsvType))\
										&& ((SERIES_SET*)(pstSeriesSet) != NULL ) \
										&& MACRO_IS_TVA_SERIES_SET((SERIES_SET*)pstSeriesSet))
#if defined (CONFIG_OP_PVR_BASED_ON_TVANYTIME_ACCURATE_RECORD)
#define MACRO_IS_TRACKING_SCHEDULE(pstRsv) ((pstRsv) && ((pstRsv)->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR||(pstRsv)->eRsvType == DxRSVTYPE_RECORD_EBR ) && (pstRsv)->uExtInfo.stRec.ulPreOffsetTime == 0 && (pstRsv)->uExtInfo.stRec.ulPostOffsetTime == 0)
#else
#define MACRO_IS_TRACKING_SCHEDULE(pstRsv)	(FALSE)
#endif
#define MACRO_IS_CONFLICT_TYPE(eRsvType) (  eRsvType == DxRSVTYPE_RECORD_EBR \
											|| eRsvType == DxRSVTYPE_RECORD_TBR \
											|| eRsvType == DxRSVTYPE_RECORD_INSTANT \
											|| eRsvType == DxRSVTYPE_RECORD_DELAYED \
											|| eRsvType == DxRSVTYPE_RECORD_SERIES_EBR \
											|| eRsvType == DxRSVTYPE_WATCHING_EBR \
											|| eRsvType == DxRSVTYPE_WATCHING_TBR \
											|| eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR \
											|| eRsvType == DxRSVTYPE_NETSTREAMING_LIVE \
											|| eRsvType == DxRSVTYPE_NETSTREAMING_FILE )

#define	SAMA_TVA_TRACKING_TRANSITION_TIME	(40) /* 40 seconds */
#define SAMA_TVA_TRACKING_TIME	(900)	/* 15min */
#define SAMA_TVA_RUNAWAY_TIME	(7200) 	/* 2 hours */
#define	SAMA_TVA_TRACKING_PADDING_TIME	(0)		/* Magic padding time for meaning tracking */
#define	SAMA_TVA_REMOVED_PADDING_TIME	(1)		/* when remove padding, use this value for differentiation between tracking & padding*/
#define	SAMA_TVA_UPDATE_DELAYED_SCH_PERIOD		(60)	/* every 60 seconds */
#define	SAMA_TVA_UPDATE_DELAYED_SCH_DURATION	(120)	/* delay its sch time 90 sec*/
#define	SAMA_TVA_RETRY_READY_SCH_PERIOD			(10)	/* every 10 seconds */



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSAMA_SchWay_None,

	eSAMA_SchWay_OneWay,
	eSAMA_SchWay_TwoWay,

	eSAMA_SchWay_EndOfCase
} SAMA_ScheduleWay_e;


typedef enum
{
	eSAMA_Result_OK,
	eSAMA_Result_FAIL,

	eSAMA_Result_BreakLoop = 0x90000,

	eSAMA_Result_EndOfCase
} SAMA_ForEachResult_e;

// sapi_conflict 모듈이 sama로 옮겨오면서 아래 type들이 같이 옮겨옴.
//기존 concept은 SAPI에서 예약을 시도하는 순간 conflict check 후에 sama로 넘겨주었지만,
//각종 예외처리 등을 위해 sama에서도 conflict check 가 필요한 경우가 생겨 sama로 conflict check 모듈을 가져가고
//sapi에서는 sama의 conflict 모듈을 가져다 쓴다.

typedef enum
{
	eSAPI_ACTIONSTATE_None				= 0,

	eSAPI_ACTIONSTATE_PaddingStart,
	eSAPI_ACTIONSTATE_Running,
	eSAPI_ACTIONSTATE_PaddingStop,

	eSAPI_ACTIONSTATE_EndOfCase
} sama_ActionState_e;

typedef enum
{
	eSAPI_ACTIONPRIORITY_None				= 0,

	// 기본 scenario의 16.3.3.1 Rule of Padding Time에 기본해서 Padding Stop이 Padding Start 보다 높도록 해야 한다.

	eSAPI_ACTIONPRIORITY_PaddingStop		= 2,
	eSAPI_ACTIONPRIORITY_PaddingStart		= 1,
	eSAPI_ACTIONPRIORITY_Running			= 8,

	eSAPI_ACTIONPRIORITY_EndOfCase
} sama_ActionPriority_e;

typedef enum
{
	eSAPI_PADDINGRMV_None				= 0x00,

	eSAPI_PADDINGRMV_StartPad			= 0x01,
	eSAPI_PADDINGRMV_StopPad			= 0x02,

	eSAPI_PADDINGRMV_EndOfCase
} sama_PaddingRemoval_e;

typedef struct
{
	SAPI_TimerItemType_e	 eTimerType;
	UNIXTIME				 utTime;
	HBOOL					 bActionRelated;
	DxSchedule_t			*pstSchedule;
} sama_CnftTimeEvt_t;

typedef struct
{
	HUINT32					 ulSlot;
	sama_PaddingRemoval_e	 ePadRemoval;
} sama_PadRemoval_t;

typedef struct
{
	//HUINT32			ulSeriesId; // pstSchedule에 달린 것으로 사용. 여기는 그냥 일단 넣어둠.
	HUINT32			ulSeason;
	HUINT32			ulEpisodeNum;
	HUINT32			ulTotalNum;

#if 0
	HBOOL			bFoundCurrentItem;
	HBOOL			bNextScheduleSuccess;

	DxSchedule_t		stPreviousSeriesSchedule;
#endif
} sama_SeriesRecordInfo_t;

typedef struct
{
	HBOOL					 bAllocated;
	HUINT32					 ulActionId;
	HUINT32					 ulSlot;
	SAPI_ActionType_e		 eActType;
	sama_ActionState_e		 eActState;
	HUINT32					 ulPriority;

	// Resource Check:
	HBOOL					 bToBeDeleted;
} sama_ActionItem_t;

typedef struct
{
	HBOOL					 bInited;
	HUINT32					 ulSemId;

	HxList_t				*pstSvcList;			// DxService_t
	HxList_t				*pstTsList;				// DxTransponder_t
	HxList_t				*pstNetList;			// DxNetwork_t
	HxList_t				*pstAntInfoList;
} sama_ServiceCache_t;

// TODO: sama context에 달아 놓는다.
typedef struct
{
	SAPI_SpecOption_t		 stSpecOption;

	// TODO:  Tree 로 변경.
	HxList_t				*pstScheduleList;			// DxSchedule_t

	// for Conflict Check
	HxList_t				*pstCfltTimeLine;			// sama_CnftTimeEvt_t
	HUINT32					 ulActionNum;
	sama_ActionItem_t		*pstActionArray;
	HINT32					 anMaxActCnt[eSAPI_ACTION_TYPE_MAX];
//	HINT32					 anSameSvcDuplicatedActCnt[eSAPI_ACTION_TYPE_MAX];
	HxList_t				*pstPadRemoveInfoList;						// sama_PadRemoval_t
	sama_CnftTimeEvt_t		*pstCheckStartEvt, *pstCheckEndEvt;
	HBOOL					 bScheduleChange;

	// for series rec
//	sama_SeriesRecordInfo_t		stSeriesRecordInfo;


	// for Conflict Check Result:
	SAPI_Conflict_t			 stConflictResult;
	SAPI_AlternativeItems_t	 stAltItems;

	// Service, Transponder Cache:
	sama_ServiceCache_t		 stSvcCache;

	// Resource Info
	HBOOL					 bRscSet;
	Handle_t				 hRlibSession;
	SAPI_RscInfo_t			 stRscInfo;
	HINT32					 anRfInputAbility[RxRFINPUT_NUM_MAX];
	HBOOL					 bRfInputAbilitySet[RxRFINPUT_NUM_MAX];

} sama_ConflictCheck_t;


// tva series 관련 구조

// TS 102 323와 DBOOK에 정의된 CRID_TYPE
typedef enum {
	SAMA_CRID_TYPE_PROG				= DxCRID_TYPE_PROG,
	SAMA_CRID_TYPE_SERIES			= DxCRID_TYPE_SERIES,
	SAMA_CRID_TYPE_RECOMMEND		= DxCRID_TYPE_RECOMMEND,
	SAMA_CRID_TYPE_ALL				= DxCRID_TYPE_ALL
} SAMA_CRID_TYPE;

#define	SAMA_SERIES_CONTENT_KIND_NONE				0x00
#define	SAMA_SERIES_CONTENT_KIND_SINGLE				0x01
#define	SAMA_SERIES_CONTENT_KIND_SPLIT				0x02
#define	SAMA_SERIES_CONTENT_KIND_SERIES				0x04
#define	SAMA_SERIES_CONTENT_KIND_RECOMMEND			0x08
#define	SAMA_SERIES_CONTENT_KIND_ALTINST			0x10
#define SAMA_SERIES_CONTENT_KIND_HD					0x20
#define SAMA_SERIES_CONTENT_KIND_CRID				0x40
#define	SAMA_SERIES_CONTENT_KIND_MASK				0x1F

// make alternative schedule시에 sapi slot을 버리고 alternative slot 값을 사용한다.
#define	SAMA_SERIES_ALTERNATIVE_SLOT				(DxRSV_SLOT_NULL-0xff)

typedef enum {
	SAMA_SERIES_TYPE_RECORD = 0,
	SAMA_SERIES_TYPE_WATCH = 1,
} SAMA_SERIES_TYPE;

typedef struct
{
	HUINT8	type;
	HUINT32	auth[(DxRSV_CRID_AUTH_LEN + (sizeof(HUINT32) * 2 - 1)) / sizeof(HUINT32)];
	HUINT32	data[(DxRSV_CRID_DATA_LEN + (sizeof(HUINT32) * 2 - 1)) / sizeof(HUINT32)];
	HUINT32	imi;
} TVA_CONTENTID_T;

typedef struct tagEVENT_PERIOD {
	HCHAR					szEventName[DxRSV_EVENT_NAME_LEN];
	HUINT32					ulSlot;			// DxSchedule_t의 slot 정보를 그대로 끌고 내려온다. DxRSV_SLOT_NULL 인 경우에는 slot에 잡혀 있는 event가 아니라는 의미
	HUINT32					startTime;
	HUINT32					endTime;
	HUINT32					ulSvcUid;
	HUINT32					ulEventId;
	HUINT32					ulSeason;
	HUINT32					ulEpisode;
	HUINT32					flag;
	struct tagINSTANCE_SET	*pstInstSet;
	struct tagEVENT_PERIOD	*pNext;
} EVENT_PERIOD;

typedef struct tagINSTANCE_SET {
	EVENT_PERIOD			*pstEventList;
	HUINT32					imi;			// 3 byte string(6937 chater 3개)인  imi를 32bit ( 3+ null )에 담아서 사용한다.
	HUINT32					flag;
	struct tagPROGRAM_SET	*pstProgSet;
	struct tagINSTANCE_SET	*pNext;
} INSTANCE_SET;

typedef struct tagPROGRAM_SET {
	INSTANCE_SET			*pstInstList;
	INSTANCE_SET			*pstSelectedInst;	// selected instance to be recorded among pInstList
	INSTANCE_SET			*pstCandidateInst;	// currently examined instance. used only while checking conflict
	TVA_CONTENTID_T			stProgCrid;
	HUINT32					flag;
	struct tagSERIES_SET	*pstSeriesSet;
	struct tagPROGRAM_SET	*pNext;
} PROGRAM_SET;

typedef struct {
	TVA_CONTENTID_T		stProgCrid;
	HUINT32				ulSvcUid;
	HUINT32				startTime;
	HUINT32				endTime;
	HUINT32				ulEventId;
} EVENT_TO_RECORD;

typedef struct tagSERIES_SET
{
	TVA_CONTENTID_T		stCrid;													// DxSchedule_t의 crid로 db에 저장
	PROGRAM_SET			*pstProgList;
	HxVector_t			*pstRecordedProgCrids;									// TVA_CONTENTID_T
	DxVideoType_e		eHdType;
	DxRsvType_e			eRsvType;
	HUINT32				ulSeriesTrackingSlot;

#if defined(CONFIG_OP_TDC)
	HUINT8				searchFlag;			// 0x30: search to all service, 0x31: search to same service
	HUINT32				ulSvcUid;			// if( searchFlag == 0x31 ) then use this
	HUINT8				szSeriesTitle[DxRSV_CRID_SERIES_TITLE_LEN];
#endif
} SERIES_SET;

typedef struct tagEventContentID {
	HUINT8	cridType;
	HUINT8	szCridAuthority[DxRSV_CRID_AUTH_LEN + 1];
	HUINT8	szCridData[DxRSV_CRID_DATA_LEN + 1];
	HUINT8	szCridIMI[DxRSV_CRID_IMI_LEN + 1];
	struct tagEventContentID *pNext;
}EVENT_CONTENTID_T;

typedef struct {
	HUINT32 numAlternative;
	HERROR result;
	struct {
		HUINT32 slot;

		/* original events */
		HINT32 numEvent;
		Handle_t svcHandle[SAMA_MAX_SPLIT_EVENT_IN_ALT_INFO];
		HUINT32 ulEventId[SAMA_MAX_SPLIT_EVENT_IN_ALT_INFO];

		/* alternative events */
		HINT32 numAltEvent;
		Handle_t altSvcHandle[SAMA_MAX_SPLIT_EVENT_IN_ALT_INFO];
		HUINT32 ulAltEventId[SAMA_MAX_SPLIT_EVENT_IN_ALT_INFO];
	} instance[SAMA_MAX_ALTERNATIVE_INSTANCE];
} TVA_ALTERNATIVE_INFO;

typedef struct
{
	HUINT32 slot;
	EVENT_PERIOD *pEventPeriod;
} OVERLAPPED_EVENT;

typedef struct
{
	HUINT32 slot;
	//HUINT16 ts_index;
	PROGRAM_SET *pProgSet;
	EVENT_PERIOD *pEventPeriod;
} CONFLICT_LIST;

// TODO: tva schedule이 생김에 따라서 tva lib와 tracking lib를 tva 안으로 옮길 필요가 있다. enum의 convention은 그때 지키도록 한다.
typedef enum
{
	TRACKING_FAIL_REASON_NONE		= 0,
	TRACKING_FAIL_REASON_REC_DELAYED		= 1,
	TRACKING_FAIL_REASON_LIVE_STREAMING		= 2,

	TRACKING_FAIL_REASON_END
} TRACKING_FAIL_REASON_e;

typedef void (* PostMsgToSamaCb_t) (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Member Functions     ********************/
/*******************************************************************/


// sama_spec.c
extern void SAMALIB_SEPC_CheckScheduleSpec(DxSchedule_t	*pstSchedule);
extern DxSchedule_Spec_e 		SAMALIB_SPEC_Get (void);
extern HUINT32			SAMALIB_SPEC_GetExtraBootTime(void);
extern HBOOL			SAMALIB_SPEC_IsEnableTBRPadding(void);


// sama_time.c
extern HERROR			SAMALIB_TIME_GetPrevTime (DxSchedule_t *pstSchedule, UNIXTIME utSrcTime, UNIXTIME *putPrevTime);
extern HERROR			SAMALIB_TIME_GetNextTime (DxSchedule_t *pstSchedule, UNIXTIME utSrcTime, UNIXTIME *putNextTime);
extern HERROR			SAMALIB_TIME_GetRepeatedScheduleStartTime (DxSchedule_t *pstSchedule, UNIXTIME utBaseTime, UNIXTIME *putStartTime);
extern HERROR			SAMALIB_TIME_AdjustScheduleTime (DxSchedule_t *pstSchedule, UNIXTIME utBaseTime, HBOOL *pbUpdated);
extern HERROR			SAMALIB_TIME_GetPaddingStartEndTime (DxSchedule_t *pstSchedule, UNIXTIME utStartTime, UNIXTIME *putStartTime, UNIXTIME *putEndTime);

// sama_conflict.c
extern HERROR 		SAMALIB_CFLT_SetResourceInfo (sama_ConflictCheck_t *pstInst);
extern HERROR 		SAMALIB_CFLT_ClearConflictResult (SAPI_Conflict_t *pstConflict);
extern HERROR		SAMALIB_CFLT_SetRfInputAbility (sama_ConflictCheck_t *pstInst, HUINT32 ulRfInputId, HBOOL bAbility);
extern HERROR 		SAMALIB_CFLT_CheckConflictWithSchedule (sama_ConflictCheck_t *pstInst, DxSchedule_t *pstNewSched);
extern SAPI_ActionType_e SAMALIB_CFLT_GetSapiActionTypeBySchedule (DxSchedule_t *pstSchedule);
extern HERROR 		SAMALIB_CFLT_AppendConflictResult (SAPI_Conflict_t *pstConflict, SAPI_ConflictItem_t *pstItem);

// samalib_tvaseries.c
extern void 		SAMALIB_TVA_PrintSeriesSet(SERIES_SET *pSeriesSet);
extern void 		SAMALIB_TVA_PrintProgramSet(PROGRAM_SET *pstProgSet);
extern void 		SAMALIB_TVA_PrintInstanceSet(INSTANCE_SET *pstInstSet);
extern void 		SAMALIB_TVA_PrintAltInfo(TVA_ALTERNATIVE_INFO *pstAltInfo);
extern void 		SAMALIB_TVA_PrintAllSeriesSet(HERROR (*pfPrintFunc)(const HCHAR *format, ...));
extern void 		SAMALIB_TVA_PrintMemoryStatus(void);

extern HERROR		SAMALIB_TVA_Init(sama_ConflictCheck_t *pstInst);
extern HERROR		SAMALIB_TVA_GetDefaultAuthority (HUINT32 ulSvcUid, HCHAR *szDefaultAuthority);
extern HERROR		SAMALIB_TVA_FindCachedService (HUINT32 ulSvcUid);
extern HERROR		SAMALIB_TVA_RemovePastEventPeriod(SERIES_SET *pstSeriesSet, UNIXTIME cur_time, HBOOL bReplaceSelectedInst);
extern HERROR		SAMALIB_TVA_SetCurrentlyRecording(SERIES_SET *pstSeriesSet, HUINT32 ulSlot);
extern HUINT8 *		SAMALIB_TVA_ConvertCridToStr(TVA_CONTENTID_T *pstCrid);
extern HERROR		SAMALIB_TVA_ExtractEachPartCRID(HUINT8 *cridstr, HUINT32 cridLen, TVA_CONTENTID_T *tva_crid, int *auth);
extern HERROR		SAMALIB_TVA_ConstructScheduleWithSeries (DxSchedule_t *pstSchedule, EVENT_PERIOD *pstEventPeriod);
extern SERIES_SET *	SAMALIB_TVA_NewSeriesSet (HUINT8 *szCrid, HUINT8 ucCridType, HCHAR *szDefaultAuth);
extern HERROR		SAMALIB_TVA_DeleteSeriesSet(SERIES_SET *pstSeriesSet);
extern HxList_t	  *	SAMALIB_TVA_GetSeriesList(void);
extern SERIES_SET *	SAMALIB_TVA_GetSeriesSetByCRID (HUINT8 *szCrid, HUINT8 ucCridType, HCHAR *szDefaultAuth);
extern SERIES_SET *	SAMALIB_TVA_GetSeriesSetBySlotId(HUINT32 ulSlot);
extern SERIES_SET *	SAMALIB_TVA_GetTrackingSeriesSetBySlotId(HUINT32 ulSlot);
extern HUINT32		SAMALIB_TVA_GetContentKind(HxVector_t *vCrid);
extern HERROR		SAMALIB_TVA_RemoveNonSelectedInstance(SERIES_SET *pstSeriesSet);
extern HERROR		SAMALIB_TVA_RemoveMatchingEventPeriod(Handle_t hSvc, HUINT32 ulEventId, SERIES_SET *pstSeriesSet, TVA_CONTENTID_T *pstRemovedProgCrid);
extern HBOOL		SAMALIB_TVA_IsSeriesReservedItem(DxSchedule_t *pstScheduleItem);
extern HBOOL		SAMALIB_TVA_IsCurrentlyRecording (SERIES_SET *pstSeriesSet);
extern HERROR		SAMALIB_TVA_AddDxEventToSeriesSet(DxEvent_t *event, SERIES_SET *pstSeriesSet, UNIXTIME ulTimeAfter, HBOOL bInitial, HUINT32 ulSlot);
extern HERROR		SAMALIB_TVA_AddDxEventsToSeriesSet (HxList_t *pstSeriesEventsList, SERIES_SET *pstSeriesSet);
extern HERROR		SAMALIB_TVA_AddRecordedCridToSeriesSet (SERIES_SET *pstSeriesSet, TVA_CONTENTID_T *pstProgCrid);
extern HERROR		SAMALIB_TVA_UpdateEventTimeBySlot(HUINT32 ulSlot, HUINT32 ulStartTime, HUINT32 ulEndTime);
extern SAMA_ForEachResult_e SAMALIB_TVA_UpdateSeriesSetFromDbToRsv(DxSchedule_t *pDbItem, HINT32 nArgc, HUINT32 *pulArgv);

extern HCHAR *		SAMALIB_TVA_MakeCridQuery (HCHAR *szCridKeyword, SERIES_SET *pstSeriesSet);
extern HERROR		SAMALIB_TVA_BuildProgramSet (SERIES_SET *pstSeriesSet);
extern HERROR		SAMALIB_TVA_BuildSeriesSet (SERIES_SET *pstSeriesSet);
extern SAPI_ConflictType_e SAMALIB_TVA_CheckScheduleConflict (SERIES_SET* pstSeriesSet, SAPI_Conflict_t *pConfInfo);
extern HERROR		SAMALIB_TVA_PrepareConflictChecking (HUINT32 ulSchedTotal, DxSchedule_t *pstScheduleArray);
extern HERROR		SAMALIB_TVA_GetNextAlternativeInfo (SERIES_SET *pstSeriesSet, DxSchedule_t *pstTarget, TVA_ALTERNATIVE_INFO *pAltInfo);
extern HERROR		SAMALIB_TVA_ConcludeConflictChecking(SERIES_SET *pstSeriesSet, HBOOL cancel);
extern HERROR		SAMALIB_TVA_SetSelectedAsCandidate (SERIES_SET *pstSeriesSet);
extern HERROR		SAMALIB_TVA_SetCandidateAsSelected (SERIES_SET *pstSeriesSet);
extern HERROR		SAMALIB_TVA_IsTVAScheduleRemovable(DxSchedule_t *pstScheduleItem, HBOOL bCallByUpdate);
extern HERROR		SAMALIB_TVA_ClearAlternativeItems (SAPI_AlternativeItems_t *pstAltItems);
extern HERROR		SAMALIB_TVA_AppendAlternativeItem (SAPI_AlternativeItems_t *pstAltItems, SAPI_AlternativeItem_t *pstItem);
extern void			SAMALIB_TVA_InitAlternativeSeriesSet (void);
extern SERIES_SET	*SAMALIB_TVA_GetAlternativeSeriesSet (HUINT32 ulEvtId, HUINT32 ulSvcId);
extern HERROR		SAMALIB_TVA_SetAlternativeSeriesSet (HUINT32 ulEvtId, HUINT32 ulSvcId, SERIES_SET *pstSeriesSet);


// samalib_tracking.c
extern void 		SAMALIB_TRACKING_PrintTrackingStatus (void);
extern HERROR		SAMALIB_TRACKING_RegisterNotifier (PostMsgToSamaCb_t fnPostFunc);
extern HERROR		SAMALIB_TRACKING_StartTracking (HINT32 nSvcUid, HINT32 nEventId, HUINT32 ulSlot,UNIXTIME	utStartTime);
extern HERROR		SAMALIB_TRACKING_StopTracking (HINT32 nSvcUid, HINT32 nEventId);
extern HERROR		SAMALIB_TRACKING_TrackEvent (DxEvent_t	*pstEvent);
extern HBOOL		SAMALIB_TRACKING_IsTrackingBySlotId (HUINT32 ulSlot);
extern HBOOL		SAMALIB_TRACKING_IsTrackingBySvcUid (HINT32 nSvcUid);
extern UNIXTIME		SAMALIB_TRACKING_GetTransitionTimeBySlotId (HUINT32 ulSlot);
extern HERROR		SAMALIB_TRACKING_GetRunningStatus (HUINT32 ulSlot, HUINT8 *pucRunningStatus);
extern HERROR		SAMALIB_TRACKING_GetPending (HUINT32 ulSlot, HBOOL *isPending);
extern HERROR		SAMALIB_TRACKING_SetPending (HUINT32 ulSlot, HBOOL isPending, TRACKING_FAIL_REASON_e eFailReason);
extern HERROR		SAMALIB_TRACKING_RestartPendingSchedules (void);

// samalib_service.c
extern HERROR		SAMALIB_SVC_GetService (sama_ConflictCheck_t *pstInst, HUINT32 ulSvcUid, DxService_t *pstService);
extern HERROR		SAMALIB_SVC_ClearCache (sama_ConflictCheck_t *pstInst);



// 그냥 단순 유틸리티성 함수..
extern HBOOL SAMALIB_CFLT_FindScheduleListUsingFilter(HxList_t *pstScheduleList, DxSchedule_FindFilterAttr_t *pstFilter, HxList_t **pstFoundScheduleList);
extern void SAMALIB_CFLT_FindScheduleListFilterInit(DxSchedule_FindFilterAttr_t *pstFilter);

#endif	// __SAMA_LIB_H__
