/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_action.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_ACTION_H___
#define	___MGR_ACTION_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <svc_pipe.h>
#include <svc_ch.h>
#include <db_svc.h>
#include <svc_resources.h>
#if defined(CONFIG_MW_MM_PVR)
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>
#else
#include <svc_meta.h>
#endif
#include <svc_fs.h>
#include <svc_sbtl.h>

#include <bus.h>

#if defined(CONFIG_MW_MM_PVR)
#include <mgr_pvr_action.h>
#include <mgr_recorder.h>
#include <mgr_playback.h>
#else
#include <mgr_common.h>
#endif

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
// TODO: 이건 왜 여기에 있나.... 위치 이동해야함.
// TODO: new batch 완성되면, xmgr_schedule_ciplus_base 로 이동.
#if defined(CONFIG_MW_CI_PLUS)
#define	DESC_REC_START_TIME			1
#define	DESC_REC_END_TIME			5
#define	DESC_REC_AVAILABLE_TIME		(DESC_REC_END_TIME - DESC_REC_START_TIME)*60*60
#endif

#define	ACTION_PRIORITY_UPPER_MASK		(0xFFFF0000)
#define	ACTION_PRIORITY_LOWER_MASK		(0x0000FFFF)
#define	ACTION_PRIORITY_BIT_OFFSET		(16)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

enum
{
	eMEVT_ACTION				= eMEVT_ACTION_START,

	/* ============================================================================
	 * 		Action Manamger
	 * ============================================================================ */

	/* - Description : Action manager에게 SetupAction 를 통보하는 메시지.
	 				 이 메시지는 SetupAction이 시작하기 전에 먼저 불린다.
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : action type
			p2 : SvcHandle (p1 == eViewType_WTV),
				etc.
			p3 : TBD */
	eMEVT_ACTION_SETUP_REQUESTED,

	/* - Description : Action manager에게 SetupAction이 성공했음을 알리는 메시지
	 				 이 메시지는 SetupAction이 성공한 다음에 불린다.
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : action type
			p2 : SvcHandle (p1 == eViewType_WTV),
				etc.
			p3 : TBD */
	eMEVT_ACTION_SETUP_SUCCEEDED,

	 /* - Description : Action manager에게 TakeAction 를 통보하는 메시지.
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : action type
			p2 : TBD
			p3 : TBD */
	eMEVT_ACTION_NOTIFY_TAKEN,

	 /* - Description : Action manager에게 ReleaseAction 을 통보하는 메시지.
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : action type
			p2 : action priority
			p3 : TBD */
	eMEVT_ACTION_NOTIFY_RELEASED,

	 /* - Description : Action Start/Stop 없이 Action Type이 변했음을 알리는 메시지.
		- Parameters Usage :
			handle : Action handle.
			p1 : new action type
			p2 : old action type
			p3 : TBD */
	eMEVT_ACTION_NOTIFY_TYPE_CHANGED,

	/* - Description : Action Start/Stop 없이 Action Priority가 변했음을 알리는 메시지.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : new action priority
		   p2 : old action priority
		   p3 : action type */
	eMEVT_ACTION_NOTIFY_PRIORITY_CHANGED,

	/* - Description : Master action이 변경되었으니 각 action들은 반영하라고 통보하는 메시지.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_ACTION_NOTIFY_MAIN_CHANGED,

	 /* - Description : 각 사양, 시나리오별 background job등의 shutdown 작업의 리스트를 우선순의에 의해 진행하라는 메시지.
		- Parameters Usage :
			handle : N.A.
			p1 : TBD
			p2 : TBD
			p3 : TBD */
	eMEVT_ACTION_SHUTDOWN_NEXTPROCESS,

	 /* - Description : 다른 작업(Recording or etc)으로 인하여 shutdown 처리를 종료 하라는 메시지.
		- Parameters Usage :
			handle : N.A.
			p1 : TBD
			p2 : TBD
			p3 : TBD */
	eMEVT_ACTION_SHUTDOWN_DESTROY,


	/* - Description : 모든 manual item action 이 종료되면.. 들어오는 메시지.
		- Parameters Usage :
			handle : N.A.
			p1 : SUCCESS, FAIL
			p2 : TBD
			p3 : TBD */
	eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS,


	/* - Description : Background Job을 실행시켰을 때 그 Job이 끝났음을 알려주는 함수
	   - Parameters Usage :
		   handle : NULL.
		   p1 : Backgroun Job Callback Function Pointer
		   p2 : Return Error Value
		   p3 : NA */
	eMEVT_ACTION_NOTIFY_BGJOB_FINISHED,

	/* - Description : STB을 Reboot 한다. Reboot 시 Resource 처리 문제가 있으므로 MW_SYS_Reboot를 직접 부르지 말고
		   반드시 이 Command를 사용하도록 한다.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : 0
		   p2 : 0
		   p3 : 0 */
	eMEVT_ACTION_REBOOT_STB,

	/* - Description : 각 Module간 처리 후 result message를 보내는 용도로 사용.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : result event (필히 mgr_common.h 에 정의해서 사용해야 함.)
		   p2 : 0
		   p3 : 0 */

	eMEVT_ACTION_NOTIFY_RESULT,

	/* - Description : 각 Batch Proc 이 종료 되어지면 결과를 webapp 에 noti 하기 위한 용도로 사용.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : result event (필히 mgr_common.h 에 정의해서 사용해야 함.)
		   p2 : 0
		   p3 : 0 */

	eMEVT_ACTION_BATCH_RESULT,

	/* - Description : Main Application이 준비 되었음을 통보
	   - Parameters Usage :
		   handle : N.A.
		   p1 : 1
		   p2 : TBD
		   p3 : TBD */
	eMEVT_ACTION_NOTIFY_READY_MAIN_APPLICATION,

	/* - Description : Obama의 Init이 완료 되었음을 통보
	   - Parameters Usage :
		   handle : N.A.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_ACTION_NOTIFY_INIT_COMPLETE,
};

typedef enum
{
    eAmState_IDLE, 					/**< AM이 아무것도 안하는 상태 */
	eAmState_NORMAL,				/**< operation 중인 상태. */
	eAmState_ACTIVE_STANDBY			/**< shutdown중인 상태 */
} MgrAction_State_e;

typedef enum
{
    eSvcDBState_NotSet, 			/**< 미확인 상태  */
    eSvcDBState_IsSvcDB,			/**< Service DB가 있는 상태 */
    eSvcDBState_NoSvcDB				/**< Service DB가 없어 동작이 제한되어 있는 상태 */
} MgrAction_SvcDB_e;

typedef enum
{
	eAmMhegStatus_None			= 0,
	eAmMhegStatus_NotRunning,
	eAmMhegStatus_Starting,
	eAmMhegStatus_Running,
	eAmMhegStatus_Stopping
} MgrAction_MhegStatus_e;

/*
	ACTION이 Take될 때 그 Action의 중요도, 즉 타 Action을 희생하면서도 필요한 여부를 따지는 Priority.
	Priority가 높은 Action이 Take될 때에 Resource가 부족해서 실행될 수 없다면
	Priority가 낮은 Action을 죽이고 그 Resource를 써서 실행될 수 있다.
*/
typedef enum
{
	eAmPriority_NONE				= 0,

	eAmPriority_BGJOB				= (10<<ACTION_PRIORITY_BIT_OFFSET)|(ACTION_PRIORITY_LOWER_MASK),
	eAmPriority_TSR					= (11<<ACTION_PRIORITY_BIT_OFFSET)|(ACTION_PRIORITY_LOWER_MASK),
	eAmPriority_VIEW_ACTION			= (32<<ACTION_PRIORITY_BIT_OFFSET)|(ACTION_PRIORITY_LOWER_MASK),
	eAmPriority_RECORD				= (33<<ACTION_PRIORITY_BIT_OFFSET)|(ACTION_PRIORITY_LOWER_MASK),
	eAmPriority_SEARCH				= (48<<ACTION_PRIORITY_BIT_OFFSET)|(ACTION_PRIORITY_LOWER_MASK),

	eAmPriority_MAX					= (255<<ACTION_PRIORITY_BIT_OFFSET)|(ACTION_PRIORITY_LOWER_MASK),
} MgrAction_Priority_e;


/*******************************************************************************
Action 에는 세가지 종류가 있다.
	1. View Action : MW의 AV를 사용하여 screen을 점유하는 action
	2. Recording Action : AV를 사용하지 않고 HDD 녹화를 이용하는 action
	3. Background Action : AV 이외의 MW 리소스를 사용하는 action

튜너등의 리소스 제약이 없다면 세개의 action은 동시에 동작할 수 있다.
그러나 각각의 action에는 한번에 하나의 action type이 점유한다.
아래 enum은 view/rec/background 별로 각각 점유할 수 있는 action type들을 나타낸다.
각각의 action type 별로 해당하는 동작을 수행하는 application proc이 할당된다.
Action type의 종류는 사양에 따라 정해지고, action의 전환이나 공존은 시나리오에 따라 정해진다.
********************************************************************************/
typedef enum
{
	eActionType_NONE,			/**< Action Context에 아무것도 지정이 되어 있지 않음. */

/* VIEW Action Types */
	eViewType_WTV,				/**< View : Zapping on digital broadcasting services */
	eViewType_PIP,				/**< View : Zapping on digital broadcasting services (PIP)*/
	eViewType_FCC,				/**< View : Fast ch change */
	eViewType_ATV,				/**< View : Zapping on analog broadcasting services */
	eViewType_IP,				/**< View : IP 가 OSD 이외의 MW resource를 사용하는 경우. */
	eViewType_PB,				/**< View : Playing HDD recorded services */
	eViewType_TSR_PB,			/**< View : Playing a HDD TSR recorded service */
	eViewType_CHASE_PB,			/**< View : Playing a HDD recoding service like TSR */
	eViewType_TV_INPUT,			/**< View : Showing TV input signal other than broadcasting services */
	eViewType_MEDIA,			/**< View : JPEG with MP3 or JPEG with AV stop.  */
	eViewType_MOVIE_PB,			/**< View : Xvid(wmv) PLAYER */
	eViewType_RFVOD,			/**< View : RF VOD playback */

/* Recording Action Types */
	eRecType_REC,				/**< Rec : Recording a service in a recording file  */
	eRecType_TSR_REC,			/**< Rec : Recording a service in a TSR file */
	eRecType_EXT_REC,			/**< Rec : Recording a service to the external module via TS Line output */


/* Background Action Types */
	eActionType_SEARCH,			/**< View or Background : Search, OTA등 검색 관련 action */
	eActionType_HOMECHANNEL,	/**< View or Background : EPG gathering. */
	eActionType_CAS_TUNE,		/**< View or Background : Tuning for CI/CAS */
	eActionType_EIT_PF_TUNE,	/**< View or Background : Tuning for EIT P/F */
	eActionType_BG_TUNE	,	/**< View or Background : Background Tune Action for various purposes. */

	eBgType_TVTV,				/**< Only Background : Gathering TVTV EPG */

	eActionType_TRANSCODING_LIVE,
	eActionType_TRANSCODING_PLAYBACK,

	eActionType_SATIP, 			/**< Background : SATIP. */
	eActionType_NTLS, 			/**< Background : NTLS. */

	eActionType_MAX
} MgrAction_Type_e;

typedef enum
{
	eLiveStreamMode_ViaLiveView,
	eLiveStreamMode_ViaTsrPb,
	eLiveStreamMode_ViaPvrPb,

	eLiveStreamMode_EndOfCase
} MgrAction_LiveStreamMode_e;

typedef enum
{
	eZAP_OK,				/**< Zapping 허가. */
	eZAP_ACTION_CHANGED,	/**< 현재 action에서는 처리 불가함. 강제로 action 교체되었으므로 현재 action은 더 이상 동작 금지. Destroy도 할 필요 없음. */
	eZAP_REC_CONFLICT,		/**< Recording 시나리오상의 conflict. 자세한 conflict 는 별도 파라미터로 전달. */
	eZAP_TUNER_CONFLICT,	/**< 사용할 수 있는 tuner 리소스가 없음. */
	eZAP_NOT_ALLOWED,		/**< 시나리오상 허용되지 않음. */
	eZAP_ASSERT				/**< Assert 상황. */
} MgrAction_ZapCheckResult_e;


#if 0
typedef struct
{
	HUINT32		ulProcessMask;
	HUINT32		eCurrentProcess;
	HINT32		ulCurrentProcIndex;
} MgrAction_StepControl_t;

// eMEVT_BUS_CREATE
typedef BUS_Result_t (* MgrAction_InitCb_t)(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void **ppvContext, HINT32 p1, HINT32 p2, HINT32 p3);
// eMEVT_BUS_DESTROY
typedef HERROR (* MgrAction_DestroyCb_t)(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3);
// eMEVT_BUS_MGR_KILLED
typedef HERROR (* MgrAction_MgrKilledCb_t)(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3);
// eMEVT_ACTION_SHUTDOWN_NEXTPROCESS
typedef BUS_Result_t (* MgrAction_DoNextProcessCb_t)(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3);
// eMEVT_ACTION_SHUTDOWN_DESTROY
typedef HERROR (* MgrAction_BatchDestroyCb_t)(Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3);
// User Defined Message
typedef BUS_Result_t (* MgrAction_MsgProcessCb_t)(HINT32 nMsg, Handle_t hAction, HUINT32 eProcess, MgrAction_StepControl_t *pstStepControl, void *pvContext, HINT32 p1, HINT32 p2, HINT32 p3);


// eMEVT_BUS_CREATE
typedef HERROR (* MgrAction_ModuleInitCb_t)(Handle_t hAction, void *pvUserData, HINT32 p1, HINT32 p2, HINT32 p3);
// eMEVT_BUS_DESTROY, eMEVT_ACTION_SHUTDOWN_DESTROY, eMEVT_BUS_GO_OPERATION
typedef HERROR (* MgrAction_ModuleDestroyCb_t)(Handle_t hAction, MgrAction_StepControl_t *pstStepControl, void *pvUserData, HINT32 p1, HINT32 p2, HINT32 p3);

typedef struct
{
	MgrAction_InitCb_t				pfnInit;
	MgrAction_DestroyCb_t			pfnDestroy;
	MgrAction_MgrKilledCb_t			pfnMgrKilled;
	MgrAction_DoNextProcessCb_t		pfnDoProcess;
	MgrAction_BatchDestroyCb_t		pfnBatchDestroy;
	MgrAction_MsgProcessCb_t		pfnProcessMsg;
} MgrAction_BatchProcess_t;

typedef struct
{
	HUINT32							 eProcess; 		// Process Identifier with Priority: e.g. Process A : 0x00000001, Process B : 0x00001000, Process C : 0x000080000
	void							*pvContext;
	MgrAction_BatchProcess_t		 stCbList;
} MgrAction_BatchProcessItem_t;

typedef struct
{
	MgrAction_ModuleInitCb_t		 pfnInit;			// eMEVT_BUS_CREATE
	MgrAction_ModuleDestroyCb_t		 pfnDestroy;		// eMEVT_BUS_DESTROY
} MgrAction_BatchModuleCb_t;

typedef struct
{
	MgrAction_BatchModuleCb_t			stModuleCallback;

	HUINT32								ulNumProcess;
	MgrAction_BatchProcessItem_t		astProcessList[DxBATCH_PROC_MAX_COUNT];
} MgrAction_BatchCB_t;

#endif
typedef BUS_Callback_t (* MgrAction_BatchStartCB_t)(void **pvUserData, HUINT32 *ulUserDataLength, DxBatchType_e eBatchType);
typedef HERROR (* MgrAction_BatchStopCB_t)(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
typedef HERROR (* MgrAction_BatchRescheduleCB_t)(DxBatchType_e eBatchType);

typedef struct
{
	MgrAction_BatchStartCB_t		fnAppInit;
	MgrAction_BatchStopCB_t			fnAppDeinit;
	MgrAction_BatchRescheduleCB_t 	fnAppReSchedule;
} MgrAction_BatchProcCBList_t;

typedef struct
{
	DxBatchProc_b 					eProcType;
	HUINT32							ulCreateCount;
	UNIXTIME						ulStartTime;
	HUINT32							ulDuration;
	HBOOL 							bRetry;

	Handle_t 						handle;
	void							*pvUserData;
	HUINT32							ulUserDataLength;
	MgrAction_BatchProcCBList_t		stAppCBList;
	BUS_Callback_t  				fnAppProc;
} MgrAction_ProcInfo;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*		Normal Action
 */
HERROR			MGR_ACTION_StartNormal(HBOOL bOperation);
HBOOL			MGR_ACTION_IsAvailableNormal (void);


void MGR_ACTION_Init (void);

/**
  @remark   Action manager proc 생성
  @remark	type :

  @return	ERR_OK\n
  			ERR_FAIL : Fail to create Bus Proc.
*/
HERROR MGR_ACTION_Start(void);

/**
  @remark   Action manager의 동작 상태인 AM_State_t를 알아본다.
  @remark	type : Inline

  @return	MgrAction_State_e
*/
MgrAction_State_e MGR_ACTION_GetState (void);

/**
  @remark   Key 입력 및 full UI를 사용하는 action을 main action이라고 한다.
  			예를 들어, 여러개의 WatchTV가 동작중인 PIP 등에서 채널 변경키를 처리하는 WatchTV와 banner가 보여주는 WatchTV가 main action이다.
  			WatchTV와 playback이 PIP로 동시에 동작중일 때도 마찬가지로 master를 정해야 한다. 키 처리와 UI가 main와 master 이외의 action이 달라진다.
  			이 API를 호출하여 main가 달라지는 경우에는 MSG_APP_MAIN_ACTION_CHANGED가 broadcating된다. Main가 될 수 있는 각 action들은
  			이 메시지를 받으면 UI를 갱신해야 한다.
  @remark	type : Inline

  @return	void
*/
void MGR_ACTION_SetMainActionId (HUINT32 ulActionId);		/**< [in] Main으로 지정하려고 하는 action ID */

/**
  @remark   현재 main action ID를 알아본다.
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
HUINT32 MGR_ACTION_GetMainActionId (void);

/**
  @remark   현재 sub action ID를 알아본다.
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
HUINT32 MGR_ACTION_GetSubActionId (void);

/**
  @remark   현재 action ID가 view action인지를 알아본다.
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
HBOOL MGR_ACTION_IsViewAction (HUINT32 ulActionId);

/**
  @remark   현재 action ID가 background action인지를 알아본다.
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
HBOOL MGR_ACTION_IsBackGroundAction (HUINT32 ulActionId);

/**
  @remark   Action을 시작하면서 action type과 proc등 action manager가 관리하는데 필요한 action 정보를 등록하는 API
  @remark	type : Inline

  @return	ERR_OK : Action type과 proc이 등록됨.\n
  			ERR_FAIL : 파라미터 error.
*/
HERROR MGR_ACTION_Take (
						HUINT32				ulActionId,			/**< [in] Action 정보를 등록하려는 action ID */
						MgrAction_Type_e	actionType,			/**< [in] 등록하려는 action type */
						BUS_Callback_t		actionProc			/**< [in] 등록하려는 action proc의 function pointer */
						);

/**
  @remark   지정한 action을 stop하면서 관련 정보를 삭제하는 API
  @remark	type : Inline

  @return	ERR_OK : 정보 삭제됨.\n
  			ERR_FAIL : 파라미터 error.
*/
HERROR MGR_ACTION_Release (
						HUINT32					ulActionId,		/**< [in] Action 정보를 삭제하려는 action ID */
						MgrAction_Type_e		eActionType		/**< [in] Release하려는 action type */
						);

HERROR MGR_ACTION_ReleaseAll (void);

/**
  @remark   Action Type에 따라 현재 동작하는 Action 갯수를 세는 함수.
  @remark	type : Inline

  @return	ERR_OK : \n
  			ERR_FAIL : 파라미터 error.
*/
HERROR MGR_ACTION_CountByType (
						MgrAction_Type_e	 eActionType,
						HINT32				*pnActNum
						);

/**
  @remark   특정 Action Type에 해당하는 Action ID를 Index (0 ~ n-1, n=MGR_ACTION_CountByType)으로부터 얻는 함수.
  @remark	type : Inline

  @return	ERR_OK : \n
  			ERR_FAIL : 파라미터 error.
*/
HERROR MGR_ACTION_GetActionIdByActTypeAndIndex (
						MgrAction_Type_e	 eActionType,
						HINT32				 nIdx,
						ActionId_t			*peActionId
						);


/**
  @remark   지정한 Bg action을 stop 시킨다.
  @remark	type : Inline

  @return	ERR_OK : 정보 삭제됨.\n
  			ERR_FAIL : 파라미터 error.
*/
HERROR MGR_ACTION_DestoryBackGroundAction(
						HUINT32 			ulActionId
						);

/**
  @remark   Action ID의 action type을 알려준다.
  @remark	type : Inline

  @return	MgrAction_Type_e
*/
MgrAction_Type_e		MGR_ACTION_GetType (HUINT32 ulActionId);		/**< [in] Action type을 알고 싶은 action의 ID */

/**
  @remark   Action ID의 action Priority 을 알려준다.
  @remark	type : Inline

  @return	MgrAction_Type_e
*/
MgrAction_Priority_e	MGR_ACTION_GetPriority (HUINT32 ulActionId);
MgrAction_Priority_e	MGR_ACTION_GetPrePriority (HUINT32 ulActionId);

/**
  @remark   Action ID의 action proc을 알려준다.
  @remark	type : Inline

  @return	MgrAction_Type_e
*/
BUS_Callback_t			MGR_ACTION_GetActionProc (HUINT32 ulActionId);

HERROR MGR_ACTION_GetActionSetupCount (HUINT32 ulActionId, HUINT32 *pulCount);
// Delayed Recording 시 eRecType_TSR_REC ->  eRecType_REC 으로 변경한다.
HERROR MGR_ACTION_ChangeType (HUINT32 ulActionId, MgrAction_Type_e actionSrcType, MgrAction_Type_e actionTgtType);

HERROR MGR_ACTION_ChangePriority (HUINT32 ulActionId, HUINT32 ulActionPriority);

/**
  @remark   Action type을 string으로 얻어간다.
  @remark	type : Inline

  @return	MgrAction_Type_e 을 string으로 리턴.
*/
HINT8 *MGR_ACTION_GetTypeStr (MgrAction_Type_e eActionType);				/**< [in] String을 얻고 싶은 action type */

/**
  @remark   Zapping을 하기위하여 시나리오 확인하고 리소스 셋업하는 API.
  @remark	type : Inline

  @return	none
*/
HERROR MGR_ACTION_SetupLive (
						HINT32						 ulActionId,			/**< [in] Zapping을 수행할 action id */
						Handle_t				 	 hSvc,					/**< [in] Zapping할 service handle */
						MgrAction_Priority_e	 	 ePriority,				/**< [in] Action 잡을 때의 Priority */
						MgrAction_Type_e 			 eActionType			/**< [in] wtv, pip만 허용됨  */
						);

/**
  @remark   Live Streaming 을 하기위하여 시나리오 확인하고 리소스 셋업하는 API.
  @remark	type : Inline

  @return	none
*/
HERROR MGR_ACTION_SetupLiveStream (
						HINT32						 ulActionId,
						Handle_t					 hSvc,
						MgrAction_Priority_e		 ePriority,
						MgrAction_LiveStreamMode_e	 eLsMode
						);

/**
  @remark   Channel search를 시작하기 위하여 시나리오 확인하고 리소스 셋업하는 API.
  			Search는 메뉴상에서 시작하여 중간에 action으로 request되므로 API를 통하여 시나리오 및 리소스 정리를 해야 함.
  @remark	type : Inline

  @return	ERR_OK : 채널 search를 하기위한 준비가 되었음. 계속 진행해도 됨.
  			ERR_FAIL : 시나리오상으로 허가되지 않는 상황이거나 리소스가 할당되지 않음. 채널 search 진행 불가.
*/
HERROR MGR_ACTION_SetupSearch (
						HUINT32					 ulActionId,		/**< [in] Search를 수행할 action id */
						DxTuneParam_t			*tuningInfo,		/**< [in] Search를 수행할 튜닝 파라미터 정보 */
						MgrAction_Priority_e	 ePriority			/**< [in] Action 잡을 때의 Priority */
						);

/**
  @remark   Media action을 시작하기 위하여 시나리오 확인하고 리소스 셋업하는 API.
  @remark	type : Inline

  @return	ERR_OK : Media action을 하기위한 준비가 되었음. 계속 진행해도 됨.
  			ERR_FAIL : 시나리오상으로 허가되지 않는 상황이거나 리소스가 할당되지 않음. Media action 진행 불가.
*/
HERROR MGR_ACTION_SetupMedia (
						HUINT32					 ulActionId,		/**< [in] Media을 수행할 action id */
						MgrAction_Type_e		 eActionType,
						MgrAction_Priority_e	 ePriority			/**< [in] Action 잡을 때의 Priority */
						);

/**
  @remark   현재 동작 중인 리소스에 전혀 영향을 주지 않고, 튜너나 디먹스가 가용할 경우에만 BG Action과 리소스 셋업하는 API.
  @remark

  @return	ERR_OK : BG Job을 진행하기 위한 준비 완료. 이후, 작업 진행
  			ERR_FAIL : 시나리오상으로 허가되지 않는 상황이거나 리소스가 할당되지 않음. action 진행 불가.
*/
HERROR MGR_ACTION_SetupBgTune (
						HUINT32 				 ulActionId,		/**< [in] BG Tune를 수행할 action id */
						DxTuneParam_t			*tuningInfo,		/**< [in] BG Tune를 수행할 튜닝 파라미터 정보 */
						MgrAction_Priority_e	 ePriority			/**< [in] Action 잡을 때의 Priority */
						);

HERROR MGR_ACTION_SetupMediaProve (
						HUINT32 ulActionId,
						MgrAction_Priority_e	ePriority
						);

HERROR MGR_ACTION_SetupCasTune (
						HUINT32					ulActionId,			/**< [in] CAS Tune 을 수행할 action id */
						Handle_t				hSvc,				/**< [in] CAS Tune 할 service handle */
						MgrAction_Priority_e	ePriority 			/**< [in] Action 잡을 때의 Priority */
						);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
/**
  @remark   MHEG ICS 및 NDT를 위해 현재 action을 유지하면서 IP Streaming을 하기위한 리소스 셋업하는 API.
  @remark	type : Inline

  @return	ERR_OK : MHEG IP Streaming action을 하기위한 준비가 되었음. 계속 진행해도 됨.
  			ERR_FAIL : 시나리오상으로 허가되지 않는 상황이거나 리소스가 할당되지 않음.
*/

HERROR MGR_ACTION_SetupMHEGIPStreaming (HUINT32 ulActionId);
#endif

#if defined(CONFIG_3RD_MHEG)
void	MGR_ACTION_SetMhegStatus(MgrAction_MhegStatus_e eStatus);
MgrAction_MhegStatus_e	MGR_ACTION_GetMhegStatus(void);
#endif

#if defined(CONFIG_MW_MM_PVR)
HERROR	MGR_ACTION_SetupGetEitPfInfo (
						HUINT32 			 ulActionId,			/**< [in] EIT PF Check 을 수행할 action id */
						Handle_t			 hSvc,					/**< [in] EIT PF Check 할 service handle */
						MgrAction_Priority_e  ePriority				/**< [in] Action 잡을 때의 Priority */
						);


// TSR_REC, Instant_REC, Schedule_REC, Playback 을 하기 위해서 Resource 체크하는 함수들은 모두 actionmanager 에 위치한다.
HERROR	MGR_ACTION_SetupRecord (
						HUINT32 				 ulActionId,		/**< [in] Record 을 수행할 action id */
						MgrAction_Type_e		 eActionType,		/**< [in] Record Action Type */
						Handle_t				 hSvc,				/**< [in] Record 할 service handle */
						MgrRec_RecordType_e		 eRecordType,		/**< [in] Record Type */
						HUINT32					 ulPvrIdx,			/**< [in] Record 할 PVR Device Index */
						MgrAction_Priority_e	  ePriority			/**< [in] Action 잡을 때의 Priority */
						);

HERROR	MGR_ACTION_SetupPlayback (
						HUINT32 				 ulActionId,		/**< [in] PVR Playback 을 수행할 action id */
						MgrAction_Type_e 		 eActionType,		/**< [in] PVR Playback Action Type */
						MgrPb_PlayType_e	 ePlaybackType,		/**< [in] PVR Playback Type */
						MgrAction_Priority_e 	 ePriority			/**< [in] Action 잡을 때의 Priority */
						);
#endif
#if defined(CONFIG_MW_MM_MEDIA_XVID)
HERROR	MGR_ACTION_SetupMovie (
						HUINT32 				 ulActionId,		/**< [in] Media Playback 을 수행할 action id */
						MgrAction_Priority_e	  ePriority			/**< [in] Action 잡을 때의 Priority */
						);
#endif

HERROR MGR_ACTION_SetupRFVOD(
						HUINT32					 ulActionId,
						DxTuneParam_t			*pstTuningInfo,
						MgrAction_Type_e		 eActionType,
						MgrAction_Priority_e	 ePriority
						);

#if defined(CONFIG_MW_SI_HOME_CHANNEL)
HERROR	MGR_ACTION_SetupHomeChannel (
						HUINT32 				 ulActionId,		/**< [in] Home Channel Tuning 을 수행할 action id */
						Handle_t				 hSvc,				/**< [in] Home Channel Tuning 할 service handle */
						MgrAction_Priority_e 	 ePriority			/**< [in] Action 잡을 때의 Priority */
						);
#endif

#if defined(CONFIG_MW_EPG_TVTV)
HERROR	MGR_ACTION_SetupTvTv (
						HUINT32 				 ulActionId,		/**< [in] TVTV EPG 을 수행할 action id */
						Handle_t				 hSvc,				/**< [in] TVTV EPG 할 service handle */
						DxTuneParam_t 		*pTuningInfo,		/**< [in] TVTV EPG 를 수행할 튜닝 파라미터 정보 */
						MgrAction_Priority_e 	 ePriority			/**< [in] Action 잡을 때의 Priority */
						);
#endif

HERROR	MGR_ACTION_SetupSatIp (
						HUINT32 ulActionId,
						DxTuneParam_t *pstTuningInfo,
						MgrAction_Priority_e ePriority,
						HBOOL	bForce,
						HBOOL	bTranscode);
HERROR	MGR_ACTION_SetupNtls (
						HUINT32 ulActionId,
						Handle_t hSvc,
						MgrAction_Priority_e ePriority);
/**
  @remark   Try simulation of satip action
  @remark

  @return	ERR_OK : satip action is possible for given priority
		ERR_FAIL : satip action is not possible
*/
HERROR MGR_ACTION_TrySatIp (
						HUINT32 ulActionId,
						DxTuneParam_t *pstTuningInfo,
						MgrAction_Priority_e ePriority,
						HBOOL	bForce,
						HBOOL	bTranscode);

HINT8		*MGR_ACTION_GetStateStr (MgrAction_State_e state);
int			MGR_ACTION_PrintContext (void *arg);
void		MGR_ACTION_RequestShutdown (void);
HERROR 		MGR_ACTION_Start(void);

// NEW POWER MODULE

void		MGR_ACTION_RequestOperation(WakeUpReason_t wakeUpReason, WakeUpSecondReason_t wakeUpSecondReason);
void		MGR_ACTION_RebootSystem (void);
HERROR		MGR_ACTION_WaitShutDown(HUINT32 mili);
HERROR		MGR_ACTION_WaitReboot(HUINT32 mili);
void		MGR_ACTION_NotifySTBStatus (void);

HERROR		MGR_ACTION_GetEmptyBgActionId (HUINT32 *pulActionId);
HERROR 		MGR_ACTION_GetEmptyRecActionId (HUINT32 *pulActionId);
HERROR		MGR_ACTION_GetMasterSvcHandle (HUINT32 ulActionId, Handle_t *phSvc);
HERROR		MGR_ACTION_GetActualSvcHandle (HUINT32 ulActionId, Handle_t *phSvc);
HERROR		MGR_ACTION_GetSvcListGroup (HUINT32 ulActionId, ChListUiGroup_t *pSvcListGroup);

#if defined(CONFIG_MW_MM_PVR)
// 사용중인 TSR_REC ActionID 값을 가져온다.
HUINT32		MGR_ACTION_GetAvailableRecordActionId (void);
HUINT32		MGR_ACTION_GetRecordCount (void);
HUINT32		MGR_ACTION_GetSATIPCount (void);
HUINT32		MGR_ACTION_GetTranscoderCount (void);
HERROR		MGR_ACTION_GetRecordingActionIdByIdx (HINT32 nIdx, ActionId_t *pActionId);
HBOOL 		MGR_ACTION_IsRecordingActionId (HUINT32 ulActionId);
#endif

#define ____FAST_CH_CHANGE_FUNCTION____
HERROR 		MGR_ACTION_ResetViewInfo (void);
HERROR 		MGR_ACTION_GetActionIdByViewId(HUINT32 ulViewId, HUINT32 *pulActionId);
HERROR 		MGR_ACTION_GetViewIdByActionId(HUINT32 ulActionId, HUINT32 *pulViewid);
HERROR		MGR_ACTION_OverwriteViewIdTable (HUINT32 ulViewId, HUINT32 ulActionId);
HERROR 		MGR_ACTION_UpdateViewIdTable(HUINT32 ulViewId, HUINT32 ulActionId);


#define ____DEBUG_FUNCTION____

const HINT8 *MGR_ACTION_MakeStrZapCheckResType (MgrAction_ZapCheckResult_e type);


HERROR		MGR_ACTION_BgJobInit (void);
HERROR		MGR_ACTION_BgJobExecuteFunction (HERROR (*pfnFunc)(Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3), Handle_t hAction, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
#if 0
HERROR 		MGR_ACTION_ReinitBatch(void);
BUS_Callback_t	MGR_Action_BatchGetProc(void);

HERROR		MGR_Action_BatchSetUp(void *pvUserData, MgrAction_BatchCB_t *pstCallback);
HERROR		MGR_Action_BatchInstallProc(DxBatchType_e eBatchType, DxBatchProc_b *eBatchProc);
#endif
void 		MGR_ACTION_StartBatchProc(DxBatchType_e eBatchType);

HERROR		MGR_ACTION_AddBatchProc (DxBatchType_e eBatchType, DxBatchProc_b eBatchProc, MgrAction_BatchProcCBList_t fnAppCBList,
										UNIXTIME ulStartTime, HUINT32 ulDuration, HBOOL bForced);
HERROR		MGR_ACTION_InitContext(DxBatchType_e	eBatchType);
void* 		MGR_ACTION_GetBatchContext(void);
HERROR		MGR_ACTION_BatchInstallStandbyProc(void);
HERROR		MGR_ACTION_BatchInstallColdBootProc(void);
HERROR 		MGR_ACTION_BatchInstallUserProc(void);

HERROR		MGR_ACTION_BatchStart (DxBatchType_e eBatchType);
HERROR		MGR_ACTION_BatchStop (DxBatchType_e	eBatchType);
HERROR		MGR_Action_BatchInstallProc(DxBatchType_e eBatchType, DxBatchProc_b *eBatchProc);
BUS_Callback_t	MGR_ACTION_BatchGetProc(void);
DxBatchType_e	MGR_ACTION_GetBatchType(void);

typedef HERROR (* MgrAction_BatchGetFuncCB_t)(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
typedef HUINT32 (* MgrAction_BatchGetDataCB_t)(Handle_t *hHandle, void **pvUserData);
typedef HERROR (* MgrAction_BatchGetTimeCB_t)(UNIXTIME *ulStartTime, HUINT32 *ulDuration);

typedef struct
{
	DxBatchProc_b 					eProcType;
	MgrAction_BatchGetFuncCB_t		fnBatchGetCBList;
	MgrAction_BatchGetTimeCB_t		fnBatchGetTime;
} MgrAction_BatchInitInfo;



MgrAction_SvcDB_e	MGR_ACTION_GetSvcDbState (void);


#endif /* !___MGR_ACTION_H___ */

