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
// TODO: �̰� �� ���⿡ �ֳ�.... ��ġ �̵��ؾ���.
// TODO: new batch �ϼ��Ǹ�, xmgr_schedule_ciplus_base �� �̵�.
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

	/* - Description : Action manager���� SetupAction �� �뺸�ϴ� �޽���.
	 				 �� �޽����� SetupAction�� �����ϱ� ���� ���� �Ҹ���.
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : action type
			p2 : SvcHandle (p1 == eViewType_WTV),
				etc.
			p3 : TBD */
	eMEVT_ACTION_SETUP_REQUESTED,

	/* - Description : Action manager���� SetupAction�� ���������� �˸��� �޽���
	 				 �� �޽����� SetupAction�� ������ ������ �Ҹ���.
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : action type
			p2 : SvcHandle (p1 == eViewType_WTV),
				etc.
			p3 : TBD */
	eMEVT_ACTION_SETUP_SUCCEEDED,

	 /* - Description : Action manager���� TakeAction �� �뺸�ϴ� �޽���.
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : action type
			p2 : TBD
			p3 : TBD */
	eMEVT_ACTION_NOTIFY_TAKEN,

	 /* - Description : Action manager���� ReleaseAction �� �뺸�ϴ� �޽���.
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : action type
			p2 : action priority
			p3 : TBD */
	eMEVT_ACTION_NOTIFY_RELEASED,

	 /* - Description : Action Start/Stop ���� Action Type�� �������� �˸��� �޽���.
		- Parameters Usage :
			handle : Action handle.
			p1 : new action type
			p2 : old action type
			p3 : TBD */
	eMEVT_ACTION_NOTIFY_TYPE_CHANGED,

	/* - Description : Action Start/Stop ���� Action Priority�� �������� �˸��� �޽���.
	   - Parameters Usage :
		   handle : Action handle.
		   p1 : new action priority
		   p2 : old action priority
		   p3 : action type */
	eMEVT_ACTION_NOTIFY_PRIORITY_CHANGED,

	/* - Description : Master action�� ����Ǿ����� �� action���� �ݿ��϶�� �뺸�ϴ� �޽���.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_ACTION_NOTIFY_MAIN_CHANGED,

	 /* - Description : �� ���, �ó������� background job���� shutdown �۾��� ����Ʈ�� �켱���ǿ� ���� �����϶�� �޽���.
		- Parameters Usage :
			handle : N.A.
			p1 : TBD
			p2 : TBD
			p3 : TBD */
	eMEVT_ACTION_SHUTDOWN_NEXTPROCESS,

	 /* - Description : �ٸ� �۾�(Recording or etc)���� ���Ͽ� shutdown ó���� ���� �϶�� �޽���.
		- Parameters Usage :
			handle : N.A.
			p1 : TBD
			p2 : TBD
			p3 : TBD */
	eMEVT_ACTION_SHUTDOWN_DESTROY,


	/* - Description : ��� manual item action �� ����Ǹ�.. ������ �޽���.
		- Parameters Usage :
			handle : N.A.
			p1 : SUCCESS, FAIL
			p2 : TBD
			p3 : TBD */
	eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS,


	/* - Description : Background Job�� ��������� �� �� Job�� �������� �˷��ִ� �Լ�
	   - Parameters Usage :
		   handle : NULL.
		   p1 : Backgroun Job Callback Function Pointer
		   p2 : Return Error Value
		   p3 : NA */
	eMEVT_ACTION_NOTIFY_BGJOB_FINISHED,

	/* - Description : STB�� Reboot �Ѵ�. Reboot �� Resource ó�� ������ �����Ƿ� MW_SYS_Reboot�� ���� �θ��� ����
		   �ݵ�� �� Command�� ����ϵ��� �Ѵ�.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : 0
		   p2 : 0
		   p3 : 0 */
	eMEVT_ACTION_REBOOT_STB,

	/* - Description : �� Module�� ó�� �� result message�� ������ �뵵�� ���.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : result event (���� mgr_common.h �� �����ؼ� ����ؾ� ��.)
		   p2 : 0
		   p3 : 0 */

	eMEVT_ACTION_NOTIFY_RESULT,

	/* - Description : �� Batch Proc �� ���� �Ǿ����� ����� webapp �� noti �ϱ� ���� �뵵�� ���.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : result event (���� mgr_common.h �� �����ؼ� ����ؾ� ��.)
		   p2 : 0
		   p3 : 0 */

	eMEVT_ACTION_BATCH_RESULT,

	/* - Description : Main Application�� �غ� �Ǿ����� �뺸
	   - Parameters Usage :
		   handle : N.A.
		   p1 : 1
		   p2 : TBD
		   p3 : TBD */
	eMEVT_ACTION_NOTIFY_READY_MAIN_APPLICATION,

	/* - Description : Obama�� Init�� �Ϸ� �Ǿ����� �뺸
	   - Parameters Usage :
		   handle : N.A.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_ACTION_NOTIFY_INIT_COMPLETE,
};

typedef enum
{
    eAmState_IDLE, 					/**< AM�� �ƹ��͵� ���ϴ� ���� */
	eAmState_NORMAL,				/**< operation ���� ����. */
	eAmState_ACTIVE_STANDBY			/**< shutdown���� ���� */
} MgrAction_State_e;

typedef enum
{
    eSvcDBState_NotSet, 			/**< ��Ȯ�� ����  */
    eSvcDBState_IsSvcDB,			/**< Service DB�� �ִ� ���� */
    eSvcDBState_NoSvcDB				/**< Service DB�� ���� ������ ���ѵǾ� �ִ� ���� */
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
	ACTION�� Take�� �� �� Action�� �߿䵵, �� Ÿ Action�� ����ϸ鼭�� �ʿ��� ���θ� ������ Priority.
	Priority�� ���� Action�� Take�� ���� Resource�� �����ؼ� ����� �� ���ٸ�
	Priority�� ���� Action�� ���̰� �� Resource�� �Ἥ ����� �� �ִ�.
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
Action ���� ������ ������ �ִ�.
	1. View Action : MW�� AV�� ����Ͽ� screen�� �����ϴ� action
	2. Recording Action : AV�� ������� �ʰ� HDD ��ȭ�� �̿��ϴ� action
	3. Background Action : AV �̿��� MW ���ҽ��� ����ϴ� action

Ʃ�ʵ��� ���ҽ� ������ ���ٸ� ������ action�� ���ÿ� ������ �� �ִ�.
�׷��� ������ action���� �ѹ��� �ϳ��� action type�� �����Ѵ�.
�Ʒ� enum�� view/rec/background ���� ���� ������ �� �ִ� action type���� ��Ÿ����.
������ action type ���� �ش��ϴ� ������ �����ϴ� application proc�� �Ҵ�ȴ�.
Action type�� ������ ��翡 ���� ��������, action�� ��ȯ�̳� ������ �ó������� ���� ��������.
********************************************************************************/
typedef enum
{
	eActionType_NONE,			/**< Action Context�� �ƹ��͵� ������ �Ǿ� ���� ����. */

/* VIEW Action Types */
	eViewType_WTV,				/**< View : Zapping on digital broadcasting services */
	eViewType_PIP,				/**< View : Zapping on digital broadcasting services (PIP)*/
	eViewType_FCC,				/**< View : Fast ch change */
	eViewType_ATV,				/**< View : Zapping on analog broadcasting services */
	eViewType_IP,				/**< View : IP �� OSD �̿��� MW resource�� ����ϴ� ���. */
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
	eActionType_SEARCH,			/**< View or Background : Search, OTA�� �˻� ���� action */
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
	eZAP_OK,				/**< Zapping �㰡. */
	eZAP_ACTION_CHANGED,	/**< ���� action������ ó�� �Ұ���. ������ action ��ü�Ǿ����Ƿ� ���� action�� �� �̻� ���� ����. Destroy�� �� �ʿ� ����. */
	eZAP_REC_CONFLICT,		/**< Recording �ó��������� conflict. �ڼ��� conflict �� ���� �Ķ���ͷ� ����. */
	eZAP_TUNER_CONFLICT,	/**< ����� �� �ִ� tuner ���ҽ��� ����. */
	eZAP_NOT_ALLOWED,		/**< �ó������� ������ ����. */
	eZAP_ASSERT				/**< Assert ��Ȳ. */
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
  @remark   Action manager proc ����
  @remark	type :

  @return	ERR_OK\n
  			ERR_FAIL : Fail to create Bus Proc.
*/
HERROR MGR_ACTION_Start(void);

/**
  @remark   Action manager�� ���� ������ AM_State_t�� �˾ƺ���.
  @remark	type : Inline

  @return	MgrAction_State_e
*/
MgrAction_State_e MGR_ACTION_GetState (void);

/**
  @remark   Key �Է� �� full UI�� ����ϴ� action�� main action�̶�� �Ѵ�.
  			���� ���, �������� WatchTV�� �������� PIP ��� ä�� ����Ű�� ó���ϴ� WatchTV�� banner�� �����ִ� WatchTV�� main action�̴�.
  			WatchTV�� playback�� PIP�� ���ÿ� �������� ���� ���������� master�� ���ؾ� �Ѵ�. Ű ó���� UI�� main�� master �̿��� action�� �޶�����.
  			�� API�� ȣ���Ͽ� main�� �޶����� ��쿡�� MSG_APP_MAIN_ACTION_CHANGED�� broadcating�ȴ�. Main�� �� �� �ִ� �� action����
  			�� �޽����� ������ UI�� �����ؾ� �Ѵ�.
  @remark	type : Inline

  @return	void
*/
void MGR_ACTION_SetMainActionId (HUINT32 ulActionId);		/**< [in] Main���� �����Ϸ��� �ϴ� action ID */

/**
  @remark   ���� main action ID�� �˾ƺ���.
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
HUINT32 MGR_ACTION_GetMainActionId (void);

/**
  @remark   ���� sub action ID�� �˾ƺ���.
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
HUINT32 MGR_ACTION_GetSubActionId (void);

/**
  @remark   ���� action ID�� view action������ �˾ƺ���.
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
HBOOL MGR_ACTION_IsViewAction (HUINT32 ulActionId);

/**
  @remark   ���� action ID�� background action������ �˾ƺ���.
  @remark	type : Inline

  @return	ERR_OK\n
  			ERR_FAIL : Invalid actionId.
*/
HBOOL MGR_ACTION_IsBackGroundAction (HUINT32 ulActionId);

/**
  @remark   Action�� �����ϸ鼭 action type�� proc�� action manager�� �����ϴµ� �ʿ��� action ������ ����ϴ� API
  @remark	type : Inline

  @return	ERR_OK : Action type�� proc�� ��ϵ�.\n
  			ERR_FAIL : �Ķ���� error.
*/
HERROR MGR_ACTION_Take (
						HUINT32				ulActionId,			/**< [in] Action ������ ����Ϸ��� action ID */
						MgrAction_Type_e	actionType,			/**< [in] ����Ϸ��� action type */
						BUS_Callback_t		actionProc			/**< [in] ����Ϸ��� action proc�� function pointer */
						);

/**
  @remark   ������ action�� stop�ϸ鼭 ���� ������ �����ϴ� API
  @remark	type : Inline

  @return	ERR_OK : ���� ������.\n
  			ERR_FAIL : �Ķ���� error.
*/
HERROR MGR_ACTION_Release (
						HUINT32					ulActionId,		/**< [in] Action ������ �����Ϸ��� action ID */
						MgrAction_Type_e		eActionType		/**< [in] Release�Ϸ��� action type */
						);

HERROR MGR_ACTION_ReleaseAll (void);

/**
  @remark   Action Type�� ���� ���� �����ϴ� Action ������ ���� �Լ�.
  @remark	type : Inline

  @return	ERR_OK : \n
  			ERR_FAIL : �Ķ���� error.
*/
HERROR MGR_ACTION_CountByType (
						MgrAction_Type_e	 eActionType,
						HINT32				*pnActNum
						);

/**
  @remark   Ư�� Action Type�� �ش��ϴ� Action ID�� Index (0 ~ n-1, n=MGR_ACTION_CountByType)���κ��� ��� �Լ�.
  @remark	type : Inline

  @return	ERR_OK : \n
  			ERR_FAIL : �Ķ���� error.
*/
HERROR MGR_ACTION_GetActionIdByActTypeAndIndex (
						MgrAction_Type_e	 eActionType,
						HINT32				 nIdx,
						ActionId_t			*peActionId
						);


/**
  @remark   ������ Bg action�� stop ��Ų��.
  @remark	type : Inline

  @return	ERR_OK : ���� ������.\n
  			ERR_FAIL : �Ķ���� error.
*/
HERROR MGR_ACTION_DestoryBackGroundAction(
						HUINT32 			ulActionId
						);

/**
  @remark   Action ID�� action type�� �˷��ش�.
  @remark	type : Inline

  @return	MgrAction_Type_e
*/
MgrAction_Type_e		MGR_ACTION_GetType (HUINT32 ulActionId);		/**< [in] Action type�� �˰� ���� action�� ID */

/**
  @remark   Action ID�� action Priority �� �˷��ش�.
  @remark	type : Inline

  @return	MgrAction_Type_e
*/
MgrAction_Priority_e	MGR_ACTION_GetPriority (HUINT32 ulActionId);
MgrAction_Priority_e	MGR_ACTION_GetPrePriority (HUINT32 ulActionId);

/**
  @remark   Action ID�� action proc�� �˷��ش�.
  @remark	type : Inline

  @return	MgrAction_Type_e
*/
BUS_Callback_t			MGR_ACTION_GetActionProc (HUINT32 ulActionId);

HERROR MGR_ACTION_GetActionSetupCount (HUINT32 ulActionId, HUINT32 *pulCount);
// Delayed Recording �� eRecType_TSR_REC ->  eRecType_REC ���� �����Ѵ�.
HERROR MGR_ACTION_ChangeType (HUINT32 ulActionId, MgrAction_Type_e actionSrcType, MgrAction_Type_e actionTgtType);

HERROR MGR_ACTION_ChangePriority (HUINT32 ulActionId, HUINT32 ulActionPriority);

/**
  @remark   Action type�� string���� ����.
  @remark	type : Inline

  @return	MgrAction_Type_e �� string���� ����.
*/
HINT8 *MGR_ACTION_GetTypeStr (MgrAction_Type_e eActionType);				/**< [in] String�� ��� ���� action type */

/**
  @remark   Zapping�� �ϱ����Ͽ� �ó����� Ȯ���ϰ� ���ҽ� �¾��ϴ� API.
  @remark	type : Inline

  @return	none
*/
HERROR MGR_ACTION_SetupLive (
						HINT32						 ulActionId,			/**< [in] Zapping�� ������ action id */
						Handle_t				 	 hSvc,					/**< [in] Zapping�� service handle */
						MgrAction_Priority_e	 	 ePriority,				/**< [in] Action ���� ���� Priority */
						MgrAction_Type_e 			 eActionType			/**< [in] wtv, pip�� ����  */
						);

/**
  @remark   Live Streaming �� �ϱ����Ͽ� �ó����� Ȯ���ϰ� ���ҽ� �¾��ϴ� API.
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
  @remark   Channel search�� �����ϱ� ���Ͽ� �ó����� Ȯ���ϰ� ���ҽ� �¾��ϴ� API.
  			Search�� �޴��󿡼� �����Ͽ� �߰��� action���� request�ǹǷ� API�� ���Ͽ� �ó����� �� ���ҽ� ������ �ؾ� ��.
  @remark	type : Inline

  @return	ERR_OK : ä�� search�� �ϱ����� �غ� �Ǿ���. ��� �����ص� ��.
  			ERR_FAIL : �ó����������� �㰡���� �ʴ� ��Ȳ�̰ų� ���ҽ��� �Ҵ���� ����. ä�� search ���� �Ұ�.
*/
HERROR MGR_ACTION_SetupSearch (
						HUINT32					 ulActionId,		/**< [in] Search�� ������ action id */
						DxTuneParam_t			*tuningInfo,		/**< [in] Search�� ������ Ʃ�� �Ķ���� ���� */
						MgrAction_Priority_e	 ePriority			/**< [in] Action ���� ���� Priority */
						);

/**
  @remark   Media action�� �����ϱ� ���Ͽ� �ó����� Ȯ���ϰ� ���ҽ� �¾��ϴ� API.
  @remark	type : Inline

  @return	ERR_OK : Media action�� �ϱ����� �غ� �Ǿ���. ��� �����ص� ��.
  			ERR_FAIL : �ó����������� �㰡���� �ʴ� ��Ȳ�̰ų� ���ҽ��� �Ҵ���� ����. Media action ���� �Ұ�.
*/
HERROR MGR_ACTION_SetupMedia (
						HUINT32					 ulActionId,		/**< [in] Media�� ������ action id */
						MgrAction_Type_e		 eActionType,
						MgrAction_Priority_e	 ePriority			/**< [in] Action ���� ���� Priority */
						);

/**
  @remark   ���� ���� ���� ���ҽ��� ���� ������ ���� �ʰ�, Ʃ�ʳ� ��Խ��� ������ ��쿡�� BG Action�� ���ҽ� �¾��ϴ� API.
  @remark

  @return	ERR_OK : BG Job�� �����ϱ� ���� �غ� �Ϸ�. ����, �۾� ����
  			ERR_FAIL : �ó����������� �㰡���� �ʴ� ��Ȳ�̰ų� ���ҽ��� �Ҵ���� ����. action ���� �Ұ�.
*/
HERROR MGR_ACTION_SetupBgTune (
						HUINT32 				 ulActionId,		/**< [in] BG Tune�� ������ action id */
						DxTuneParam_t			*tuningInfo,		/**< [in] BG Tune�� ������ Ʃ�� �Ķ���� ���� */
						MgrAction_Priority_e	 ePriority			/**< [in] Action ���� ���� Priority */
						);

HERROR MGR_ACTION_SetupMediaProve (
						HUINT32 ulActionId,
						MgrAction_Priority_e	ePriority
						);

HERROR MGR_ACTION_SetupCasTune (
						HUINT32					ulActionId,			/**< [in] CAS Tune �� ������ action id */
						Handle_t				hSvc,				/**< [in] CAS Tune �� service handle */
						MgrAction_Priority_e	ePriority 			/**< [in] Action ���� ���� Priority */
						);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
/**
  @remark   MHEG ICS �� NDT�� ���� ���� action�� �����ϸ鼭 IP Streaming�� �ϱ����� ���ҽ� �¾��ϴ� API.
  @remark	type : Inline

  @return	ERR_OK : MHEG IP Streaming action�� �ϱ����� �غ� �Ǿ���. ��� �����ص� ��.
  			ERR_FAIL : �ó����������� �㰡���� �ʴ� ��Ȳ�̰ų� ���ҽ��� �Ҵ���� ����.
*/

HERROR MGR_ACTION_SetupMHEGIPStreaming (HUINT32 ulActionId);
#endif

#if defined(CONFIG_3RD_MHEG)
void	MGR_ACTION_SetMhegStatus(MgrAction_MhegStatus_e eStatus);
MgrAction_MhegStatus_e	MGR_ACTION_GetMhegStatus(void);
#endif

#if defined(CONFIG_MW_MM_PVR)
HERROR	MGR_ACTION_SetupGetEitPfInfo (
						HUINT32 			 ulActionId,			/**< [in] EIT PF Check �� ������ action id */
						Handle_t			 hSvc,					/**< [in] EIT PF Check �� service handle */
						MgrAction_Priority_e  ePriority				/**< [in] Action ���� ���� Priority */
						);


// TSR_REC, Instant_REC, Schedule_REC, Playback �� �ϱ� ���ؼ� Resource üũ�ϴ� �Լ����� ��� actionmanager �� ��ġ�Ѵ�.
HERROR	MGR_ACTION_SetupRecord (
						HUINT32 				 ulActionId,		/**< [in] Record �� ������ action id */
						MgrAction_Type_e		 eActionType,		/**< [in] Record Action Type */
						Handle_t				 hSvc,				/**< [in] Record �� service handle */
						MgrRec_RecordType_e		 eRecordType,		/**< [in] Record Type */
						HUINT32					 ulPvrIdx,			/**< [in] Record �� PVR Device Index */
						MgrAction_Priority_e	  ePriority			/**< [in] Action ���� ���� Priority */
						);

HERROR	MGR_ACTION_SetupPlayback (
						HUINT32 				 ulActionId,		/**< [in] PVR Playback �� ������ action id */
						MgrAction_Type_e 		 eActionType,		/**< [in] PVR Playback Action Type */
						MgrPb_PlayType_e	 ePlaybackType,		/**< [in] PVR Playback Type */
						MgrAction_Priority_e 	 ePriority			/**< [in] Action ���� ���� Priority */
						);
#endif
#if defined(CONFIG_MW_MM_MEDIA_XVID)
HERROR	MGR_ACTION_SetupMovie (
						HUINT32 				 ulActionId,		/**< [in] Media Playback �� ������ action id */
						MgrAction_Priority_e	  ePriority			/**< [in] Action ���� ���� Priority */
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
						HUINT32 				 ulActionId,		/**< [in] Home Channel Tuning �� ������ action id */
						Handle_t				 hSvc,				/**< [in] Home Channel Tuning �� service handle */
						MgrAction_Priority_e 	 ePriority			/**< [in] Action ���� ���� Priority */
						);
#endif

#if defined(CONFIG_MW_EPG_TVTV)
HERROR	MGR_ACTION_SetupTvTv (
						HUINT32 				 ulActionId,		/**< [in] TVTV EPG �� ������ action id */
						Handle_t				 hSvc,				/**< [in] TVTV EPG �� service handle */
						DxTuneParam_t 		*pTuningInfo,		/**< [in] TVTV EPG �� ������ Ʃ�� �Ķ���� ���� */
						MgrAction_Priority_e 	 ePriority			/**< [in] Action ���� ���� Priority */
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
// ������� TSR_REC ActionID ���� �����´�.
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

