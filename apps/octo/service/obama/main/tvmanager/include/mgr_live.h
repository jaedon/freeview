/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_live.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_LIVE_H___
#define	___MGR_LIVE_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>
#include <svc_si.h>

#include <mgr_common.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eMEVT_LIVE		= eMEVT_LIVE_START,

	/* - Description : 신 Live Module의 Start 명령
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : Service Handle (Supplementary, if it is Master/Supplementary Service Channel)
		   p2 : Master Service Handle
		   p3 : Play Type (0=Main, 1=PIP, 2=Live Streaming...) */
	eMEVT_LIVE_START_SVC,			// Command 방식

	/* - Description : 신 Live Module의 Stop 명령
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : N.A
		   p2 : N.A
		   p3 : N.A */
	eMEVT_LIVE_STOP_SVC,			// Command 방식

	/* - Description :
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : N.A
		   p2 : N.A
		   p3 : N.A */
	eMEVT_LIVE_NOTIFY_SVC_STARTED,				// Notifier

	/* - Description :
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : N.A
		   p2 : N.A
		   p3 : N.A */
	eMEVT_LIVE_NOTIFY_SVC_STOPPED,				// Notifier

	/*	- Description : LIVE에서 서비스 변경이 일어남을 알리는 이벤트 메시지.
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : service handle
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_SVC_CHANGED,				// Notifier

	/*	- Description : LIVE에서 PMT Update에 의해 현재 Svc 의 PID (A, V, PCR) 중 하나를 변경함
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : hSvcHandle
			p2 : tuning mode
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_CURRENT_SVC_PID_CHANGED,	// Notifier

	/*	- Description : LIVE에게 MHEG용 quiet 서비스 튜닝을 요청하는 메시지. MSG_LIVE_START_SVC와 달리
					Replacement service 처럼 LIVE가 직접 받아서 처리한다.
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : service handle.
			p2 : TBD
			p3 : TBD */
	eMEVT_LIVE_MHEG_QUIET_TUNE,

	/*	- Description : Live view moduel에서 UI state 변경이 일어났음을 알리는 이벤트 메시지.
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : AppState
			p2 : UI state
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED,

	/*	- Description : Live Lock module에서 UI state 변경이 일어났음을 알리는 이벤트 메시지.
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : AppState
			p2 : UI state
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_LOCK_STATE_CHANGED,

	/*	- Description : Complementary proc에 complementary start를 요청하는 메시지
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : Complementary type
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_START_COMPLEMENTARY,

	/*	- Description : Complementary proc에 complementary stop을 요청하는 메시지
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : xMGR_Complementary_e
			p2 : ApiSyncMode_t
			p3 : TBD
	*/
	eMEVT_LIVE_STOP_COMPLEMENTARY,

	/*	- Description : LIVE에 rating passed flag 변경을 요구함.
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : TRUE, FALSE
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_SET_RATING_PASSED_FLAG, //30

	/*	- Description : event relay가 발생했고 릴레이되는 채널 정보 같이 발생
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : hSvcHandle
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_EVENT_RELAY,

	/*	- Description : 긴급 경보 방송이 발생했고 해당 채널 정보 같이 발생
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : hSvcHandle
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_EVENT_EWS,

	/*	- Description : pin code dialog 상태가 변경됨 -  일
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : hSvcHandle
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_EVENT_RATING_CTRL_CHANGED,

	/*	- Description : svc lock 인 상태를 view proc에 알림
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : TBD
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_SVC_LOCKED,

	/*	- Description : svc lock 인 상태에서 correct pin이 입력되어 svc unlock됨
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : TBD
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_SVC_UNLOCKED,

	/*	- Description : set main action(pip swap)을 각 live proc(view/lock)에 알리는 msg
		- Parameters Usage :
			handle : N/A
			p1 : new main action id
			p2 : old main action id
			p3 : TBD
	*/
	eMEVT_LIVE_SET_MAINVIEW_ACTION,

	/*	- Description : unselect componet를 설정하는 msg. Audio/Video에 설정 가능
		- Parameters Usage :
			handle : action Handle
			p1 : bUnselectVideoFlag
			p2 : bUnselectAudioFlag
			p3 : TBD
	*/
	eMEVT_LIVE_SET_UNSELECT_COMPONENT,

	/* - Description : MHEG등의 데이터 방송에서 PID 변경을 요청하는 메시지. DB상의 PID 정보는 변경이 없는 TEMP PID이므로 DB를 참조하면 안됨.
	   - Parameters Usage :
		   handle : Action handle
		   p1 :
				Video codec(high 16bit 사용. 1. Valid DxVideoCodec_e => 변경 2. CODEC_CURRENT => No change)
		   		Video PID (low 16bit 사용. 1. Valid PID => 변경 2. PID_NULL => stop 3. PID_CURRENT => No change)
		   p2 :
				Video codec(high 16bit 사용. 1. Valid DxAudioCodec_e => 변경 2. CODEC_CURRENT => No change)
		   		Audio PID (low 16bit 사용. 1. Valid PID => 변경 2. PID_NULL => stop 3. PID_CURRENT => No change)
		   p3 : PCR PID (low 16bit 사용. 1. Valid PID => 변경 2. PID_NULL => stop 3. PID_CURRENT => No change) */
	eMEVT_LIVE_CHANGE_TEMP_PID,

	/*	- Description : fcc svc를 main live svc로 전환한다
		- Parameters Usage :
			handle : new main live action Handle (fcc -> main)
			p1 : old main live action id (main -> fcc)
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_SET_FCC_MAINVIEW_ACTION,

	/*	- Description : fcc svc -> main svc로 전환시에 last tune status를 알린다
		- Parameters Usage :
			handle : action Handle
			p1 : tune status
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_FCC_TUNE_STATUS,

	/*	- Description : fcc svc -> main svc로 전환시에 live view에서 관리하는 mw svc중 재시작이 필요한 svc들을 중지시킨다.
		- Parameters Usage :
			handle : action Handle
			p1 : current main action id
			p2 : new main action id
			p3 : TBD
	*/
	eMEVT_LIVE_STOP_MW_SVC_FOR_FCC,

	/*	- Description : hbbtv red button으로 prm hdplus를 진입시 browser에게 알리기위해 사용함.
		- Parameters Usage :
			handle : action Handle
			p1 : TBD
			p2 : TBD
			p3 : hdplusredbutton
	*/
	eMEVT_LIVE_SET_HBBTV_ON_HDPLUS_REDBUTTON,

	/*	- Description : Mheg app 의 status 를 알린다.
		- Parameters Usage :
			handle : action Handle
			p1 : status
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_SET_MHEG_STATUS_CHANGE,
	
	/*	- Description : Mheg BackGroud Start
		- Parameters Usage :
			handle : TBD
			p1 : service handle
			p2 : view Id
			p3 : TBD
	*/
	eMEVT_MHEG_BG_START,
};

typedef enum
{
	eMgrLiveMode_None			= 0,

	eMgrLiveMode_MainLive,
	eMgrLiveMode_PipLive,
	eMgrLiveMode_LiveStreaming,
	eMgrLiveMode_FccLive,			// fast ch change

	eMgrLiveMode_EndOfCase
} MgrLive_LiveMode_e;


typedef enum
{
	eLiveViewSession_NORMAL,		/**< Normal WatchTV session */
	eLiveViewSession_MHEG_QUIET,	/**< MHEG, MHP등 data 방송에서 요청한 silent tuning */
	eLiveViewSession_REPLACE,		/**< Free CA mode/running status change에 의한 replacement service tuning */
	eLiveViewSession_SUBFEED,		/**< Sub-feed service session */
	eLiveViewSession_TSMF_LOW,		/**< 일본향 TSMF 저계층 이동 상태 */
} MgrLiveView_SessionType_t;

typedef enum
{
	eNO_IPLATE				 = 0x01,	/**< i-plate display 없이 튜닝함. */
	ePINCODE_ON_TOP			 = 0x02,	/**< PIN code는 exit전까지 메뉴 위에 뜸. Exit하면 바닥에 깔림.*/
	ePINCODE_PASSED			 = 0x04,	/**< PIN code는 외부에서 pass 했음.. pin code pass로 간주. */
	eKEEP_PINPASS			 = 0x08,	/**< PIN code pass flag를 현재 session 대로 유지하라. (ex. subfeed끼리 이동... ) */
	eFORCE_TUNE				 = 0x10,	/**< 현재 튜닝중인 서비스 핸들이라도 강제로 재 튜닝 시키도록 한다. */
	eNDT_TUNE				 = 0x20,	/**< MHEG의 Non Destructive Tune을 위한 mode로 이전 Video와 Audio를 끊지 않고 변경될 redkey가 변경할 service handle 만 MHEG manager로 전달하기 위함. */
	eCA_ALTER_TUNE			 = 0x40,	/**< CA 대체 서비스에 의해 튜닝됨.	*/
	eEWS_TUNE				 = 0x80,	/**< EWS에의해 튜닝됨.	*/
	eTUNING_MODE_MAX
} MgrLive_TuningMode_t;

typedef enum
{
	eLiveProc_View,
	eLiveProc_Lock,

	eLiveProc_MAX
} MgrLive_ProcType_e;


typedef struct
{
	HUINT32					ulMajorCHNum;		/*LCN*/
	MgrLive_TuningMode_t		eTuneMode;		/*TuneMode*/

} MgrLive_MainLiveArg_t;

typedef struct
{
	HUINT32					 ulReserved;
} MgrLive_PipLiveArg_t;

typedef struct
{
	DxVideoCodec_e	eVideoCodec;
	DxAudioCodec_e	eAudioCodec;

	HINT32			nAudioBitrateKbps;
	HINT32			nVideoBitrateKbps;
	HINT32			nHdVideoBitrateKbps;	// Live가 HD이면, 다른 Bitrate으로 필요로 하면 사용
	HINT32			nFrameRate;
	HINT32			nScreenWidth;
	HINT32			nScreenHeight;

	HBOOL			bInterlaceMode;

	DxAspectRatio_e eVideoAspectRatio;

	/*video-codec extension*/
	DxVideoEncodeProfile_e eVideoCodecProfile;
	DxVideoEncodeLevel_e eVideoCodecLevel;

	/*need probe-info from live*/
	HBOOL	bUsingLiveAudioCodec;	//(true : bypass, false : using eAudioCodec)
	HBOOL	bUsingLiveAspectRatio;	//(true : bypass, false : using eVideoAspectRatio)

} MgrLive_StreamProfile;

typedef struct
{
	Handle_t				hReqHandle;
	Handle_t				hRingBuf;
	HINT32					nProfileId;

	HBOOL					bPcrTsPacketInsert;	/*include PCR Ts packet*/
	HBOOL					bTranscode;

	MgrLive_StreamProfile	stProfile;

} MgrLive_LiveStreamArg_t;

typedef struct
{
	MgrLive_LiveMode_e				 eLiveMode;
	Handle_t						 hService;
	Handle_t						hMasterSvc;	// Master Svc Handle
	Handle_t						hSuppleSvc;		// supplementary Svc Handle
		
	MgrLiveView_SessionType_t		eLiveSessionType;
	
	union
	{
		MgrLive_MainLiveArg_t		 stMainLiveArg;
		MgrLive_PipLiveArg_t		 stPipLiveArg;
		MgrLive_LiveStreamArg_t		 stLiveStreamArg;
	} unExtArg;
} MgrLive_START_t;


/**
  *	수정시 MediaCtrl_LiveView_State_e 와 반드시 sync 맞춰야 함
**/
typedef enum
{
	eLiveViewUi_OK,					/**< 정상 시청 상태. No error */
	eLiveViewUi_NONE,				/**< initial state */
	eLiveViewUi_EMPTY_SVC,			/**< 서비스 중단 메시지 */
	eLiveViewUi_NO_SIGNAL,			/**< No Signal 에러 메시지 */
	eLiveViewUi_NOK_ANTENNA,		/** <Aantenna fail상태(지상파 경우 short-circuit) */
	eLiveViewUi_MOTOR_MOVING,	/**< Motor antenna 이동 메시지 */
	eLiveViewUi_CAS,					/**< CAS block 상태. 실제 메시지는 CAS application이 담당함 */
	eLiveViewUi_SVC_LOCKED,			/**< 유저 lock PIN code 상태. pin 입력 처리는 lock proc에서 담당하며 unlock시 msg를 보낼 것임 */
	eLiveViewUi_AV_UNDER_RUN,		/**< Scrambled 또는 잘못된 PID의 경우와 같이 AV stream이 들어오지 않는 상태 */
	eLiveViewUi_HDMI_BLOCKED,		/**< Authentication failure로 인해 HDMI에서 AV block되었음. */
	eLiveViewUi_NO_VIDEO,			/**< Video가 없는 TV 채널에서 black 화면 방지용 메시지. */
	eLiveViewUi_NO_AV,				/**< Audio, Video PID가 모두 없는 경우 알림. */
	eLiveViewUi_NOT_RUNNING,		/**< SDT의 Running Status 가 NOT running으로 바뀌는 경우 메세지 . */
	eLiveViewUi_NO_SIGNAL_TIMEOUT,	/**< eLiveViewUi_NO_SIGNAL상태가 30초 이상 지속될때. WTV_UiState_t만 바뀌고 app_state_t는 변함없음 */
	eLiveViewUi_REMOVE_CH,			/** < 시청 중인 채널이 삭제된 경우 **/

	eLiveViewUi_MAX
} MgrLiveView_UiState_e;

/**
  *	수정시 MediaCtrl_LiveLock_State_e 와 반드시 sync 맞춰야 함
**/
typedef enum
{
	eLockUi_OK,						/**< 정상 시청 상태. No error */
	eLockUi_NONE,					/**< initial state */
	eLockUi_LOCK_PIN,				/**< 유저 lock PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eLockUi_LOCK_PIN_DENIED,		/**< 유저 lock PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eLockUi_RATING_PIN,				/**< Rating PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eLockUi_RATING_PIN_DENIED,		/**< Rating PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eLockUi_SC_PIN,					/**< Smart card PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. <<- live module에서 사용하지 않음 */
	eLockUi_SC_PIN_DENIED,			/**< Rating PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. <<- live module에서 사용하지 않음 */
	eLockUi_NO_EIT,					/**< EIT 정보 없음. */

	eLockUi_MAX
} MgrLiveLock_UiState_e;

typedef enum
{
	eLiveViewPlaySvc_ACTUAL,		/**< Play 중인 Service로써 Action SVC (PMT based, PID based)   */
	eLiveViewPlaySvc_MASTER 		/**< Play 중인 Service로써 Action SVC (EIT based, Title based)   */
} MgrLiveView_PlaySvc_e;


/*	Live Zapping
 */
typedef enum
{
	eLastSvcItem_TV,
	eLastSvcItem_RADIO,
	eLastSvcItem_SAT,
	eLastSvcItem_TER,
	eLastSvcItem_CAB,
	eLastSvcItem_DTV,
	eLastSvcItem_ANA,
	eLastSvcItem_Freesat,
	eLastSvcItem_NotFreesat
} MgrLive_LastSvcItem_t;



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern HERROR	MGR_LIVE_Init (void);
extern HERROR	MGR_LIVE_Start (MgrLive_START_t *pstStartArg, HUINT32 *pulActionId);
extern HERROR	MGR_LIVE_Stop (HUINT32 ulActionId);
#if defined(CONFIG_SUPPORT_FCC)
extern HERROR 	MGR_LIVE_FindFccActionIdBySvc(Handle_t hService, HUINT32 *pulActionId);
extern HUINT32  MGR_LIVE_GetFccActionId(void);
extern HERROR 	MGR_LIVE_ActionNotifyReleased(HUINT32 ulActionId);
#endif

extern HERROR 	MGR_LIVE_StopFccSvc(HUINT32 ulFccActionId);
extern HERROR 	MGR_LIVE_StopAllFccSvc(void);

extern HERROR	MGR_LIVE_GetLiveMode(HUINT32 ulActionId, MgrLive_LiveMode_e *peLiveMode);
extern HERROR 	MGR_LIVE_SetLiveMode(HUINT32 ulActionId, MgrLive_LiveMode_e eNewLiveMode);
extern HERROR 	MGR_LIVE_SetMainMedia (HUINT32 ulNewMainActionId, HUINT32 ulCurrMainActId);
extern HERROR 	MGR_LIVE_SetVideoHideByAppkit (HUINT32 ulActionId, HBOOL bMuteFlag);
extern HERROR 	MGR_LIVE_UnselectVideo (HUINT32 ulActionId);
extern HERROR 	MGR_LIVE_UnselectAudio (HUINT32 ulActionId);

#if 0
extern HERROR 	MGR_LIVE_SetAudioMuteAll (HUINT32 ulActionId, HBOOL bMuteFlag);
extern HERROR 	MGR_LIVE_SetVideoMuteAll (HUINT32 ulActionId, HBOOL bMuteFlag);
extern HERROR 	MGR_LIVE_SetAvMuteAll (HUINT32 ulActionId, HBOOL bMuteFlag);
#endif

extern HERROR 	MGR_LIVE_GetServiceHandle (HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, Handle_t *phSvc);
extern HERROR 	MGR_LIVE_GetSessionType (HUINT32 ulActionId, MgrLiveView_SessionType_t *pSessionType);
extern HERROR 	MGR_LIVE_GetSvcInfo (HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, DbSvc_Info_t *pMasterSvcInfo);
extern HERROR 	MGR_LIVE_GetTsInfo (HUINT32 ulActionId, DbSvc_TsInfo_t *pTsInfo);
extern HERROR	MGR_LIVE_GetRating (HUINT32 ulActionId, DxRatingAge_e *peRatingValue);
extern HERROR	MGR_LIVE_SetRatingPassedFlag(HUINT32 ulActionId, HBOOL bPassedFlag);


extern HERROR 	MGR_LIVE_SetAudioIndex (HUINT32 ulActionId, HUINT32 ulIndex);
extern HERROR 	MGR_LIVE_SetDualMonoConfig (HUINT32 ulActionId, DxDualMonoSelect_e eDualMonoConfig);
extern HERROR 	MGR_LIVE_GetRingBufHandle (HUINT32 ulActionId, Handle_t *phRingBuf);
extern HERROR	MGR_LIVE_GetProfileFromJSON(const HCHAR *pProfileString, MgrLive_LiveStreamArg_t *pstDestArg);

extern HERROR	MGR_LIVE_ResetUiStateView (HUINT32 ulActionId, HBOOL bSvcStart);
extern HERROR	MGR_LIVE_ResetUiStateLock (HUINT32 ulActionId, HBOOL bSvcStart);
extern HERROR 	MGR_LIVE_GetState (HUINT32 ulActionId, MgrLive_ProcType_e eProcType, HUINT32 *pulUiState);
#if 0
extern HERROR	MGR_LIVE_GetUiStateView (HUINT32 ulActionId, MgrLiveView_UiState_e *peState);
extern HERROR	MGR_LIVE_GetUiStateLock (HUINT32 ulActionId, MgrLiveView_UiState_e *peState);
#endif
extern HERROR	MGR_LIVE_GetLiveSiSpec (MgrLive_START_t *pstStart, SvcSi_LiveSpec_t *pstSiSpec);
extern HERROR	MGR_LIVE_GetSatRecSiSpec (SvcSi_SatRecordSpec_t *pstSiSpec);
extern HERROR 	MGR_LIVE_CheckFccLiveContext (void);

// Debug
extern HUINT8* MGR_LIVE_GetLiveModeStr (MgrLive_LiveMode_e eLiveMode);
extern void	MGR_LIVE_PrintContext (void);


/*******************************************************************/
/********************      Public Functions     ********************/
/********************      ZAPPING              ********************/
/*******************************************************************/


/**
  @remark	zapping 기능 관련 초기화.\n
  @remark   API Type : \n

  @return   HERROR
*/
extern void	MGR_LIVE_ZapInit(void);

/**
  @remark	zapping 기능 관련 초기화.\n
  @remark   API Type : \n

  @return   HERROR
*/
extern void	MGR_LIVE_ResetZappingInfo (HBOOL bLoadSetup);


/**
  @remark   Channel service 정보를 last service info에 등록한다. 같은 서비스가 등록되면 무시된다.
  			새 서비스가 등록되면 기존 서비스정보는 service type과 delivery type별로 분류되어 이전 채널로 등록된다.
  @remark	type : Inline

  @return	none
*/
extern void	MGR_LIVE_RegisterLastSvcInfo (Handle_t			hSvc);			/**< [in] 등록할 현재 서비스 핸들 */

/**
  @remark   Last service handle을 얻어감.
  @remark	type : Inline

  @return	ERR_OK : Valid last service info.
  			ERR_FAIL : 저장된 last service 정보가 없음.
*/
extern HERROR	MGR_LIVE_GetPrevSvcHandle (Handle_t			*phPrevSvc);			/**< [out] Last service handle을 담아갈 변수의 포인터. */


/**
  @remark   Item을 지정하여 last service handle을 얻어감.
  @remark	type : Inline

  @return	ERR_OK : Valid last service handle.
  			ERR_FAIL : 입력 파라미터 에러.
*/
extern HERROR	MGR_LIVE_GetLastSvcHandle (MgrLive_LastSvcItem_t	eLastSvcItem,		/**< [in] 읽어갈 last service handle item */
											Handle_t				*phLastSvc			/**< [out] Last service handle을 담아갈 변수의 포인터 */
											);

/**
  @remark   Svc Lock Pass 된 적이 있는 Svc인가?
  @remark	type : Inline

  @return	TRUE : Passed
*/
extern HBOOL	MGR_LIVE_SvcLockIsPassed (HUINT32 ulActionId, Handle_t hSvc);

/**
  @remark   Svc Lock Pass 된 적이 Svc로 저장?
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL :
*/
extern HERROR	MGR_LIVE_SvcLockSetPassed (HUINT32 ulActionId, Handle_t hSvc);

/**
  @remark   Svc Lock Passed 정보 Reset
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL :
*/
extern HERROR	MGR_LIVE_SvcLockResetPassed (HUINT32 ulActionId);

/**
  @remark   현재 service handle과 group과 delivery type을 얻어감.
  @remark	type : Inline

  @return	ERR_OK : Valid last service info.
  			ERR_FAIL : 저장된 last service 정보가 없음.
*/
extern HERROR	MGR_LIVE_CurrentSvcInfoGet(HUINT32				ulActionId,			/**< [in] 정보를 원하는 action ID */
											Handle_t			*phCurSvc 			/**< [out] 현재의 service handle */
											);
/**
  @remark   현재 service handle과 group을 저장함.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL :
*/
extern HERROR	MGR_LIVE_CurrentSvcInfoSet(HUINT32			ulActionId,
										Handle_t			hCurSvc
										);


/**
  @remark   임시 PID play.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL : 입력 파라미터 에러.
*/
extern HERROR	MGR_LIVE_CmdChangeTempPid(Handle_t			hAction,
										DxVideoCodec_e		eVideoCodec,
										HUINT16				usVideoPid,
										DxAudioCodec_e		eAudioCodec,
										HUINT16				usAudioPid,
										HUINT16				usPcrPid
										);

/**
  @remark   Main view 에 대하여 service start 시킴. Send message에 의해 sync 동작이 일어남.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL : 입력 파라미터 에러.
*/
extern HERROR	MGR_LIVE_CmdSendServiceStartMainView(Handle_t hSvc, ChListUiGroup_t eChListUiGroup, HUINT32 ulTuningMode);

/**
  @remark   Main view 에 대하여 service start 시킴. Post message에 의해 async 동작이 일어남.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL : 입력 파라미터 에러.
*/
extern HERROR	MGR_LIVE_CmdPostServiceStartMainView(Handle_t hSvc, ChListUiGroup_t eChListUiGroup, HUINT32 ulTuningMode);

/**
  @remark   Action ID를 지정하여 service start 시킴. Send message에 의해 sync 동작이 일어남.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL : 입력 파라미터 에러.
*/
extern HERROR	MGR_LIVE_CmdSendServiceStartByActionId(HUINT32 ulActionId, Handle_t hSvc);

/**
  @remark   Action ID를 지정하여 service start 시킴. Post message에 의해 async 동작이 일어남.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL : 입력 파라미터 에러.
*/
extern HERROR	MGR_LIVE_CmdPostServiceStartByActionId(HUINT32 ulActionId, Handle_t hSvc);


//		mgr_live_pincode.c
// Live, Tsr Playback, Chase Playback 도중에 Pincode 를 풀면, 그 해제한 eventid 와 parental rate를 리스트에 저장한다.
extern HERROR	MGR_LIVE_PincodeSetRatingPassedEventId(Handle_t hSvc, HINT32 nEvtId, HUINT8 ucPRate);

// Live, Tsr Playback, Chase Playback 도중에 새로운 event 를 만나면, 그 event 를 해제한 적인 있는지 확인한다.
extern HERROR	MGR_LIVE_PincodeIsEventIdRatingPassed(Handle_t hSvc, HINT32	nEvtId, HUINT8 *ucPRate, HBOOL *bIsRatingPassed);

// Standby 로 진입할 때, 해제한 Pincode 정보 리스트를 clear 한다.
extern HERROR	MGR_LIVE_PincodeDeleteAllServiceHandle(void);

#if 0
extern HERROR	MGR_LIVE_GetSiUpdate (SvcSi_UpdateEs_t *pstUpdate, DbSvc_Info_t *pstOrgInfo, DbSvc_Info_t *pstDestInfo);
extern HBOOL	MGR_LIVE_CompareSvc (const void *pvInfo1, const void *pvInfo2, HUINT32 ulSize);
extern HERROR	MGR_LIVE_UpdateSvc (const SvcSi_PmtEsInfo_t *pstPmtEsInfo, void *pvSvcInfo);
extern HERROR	MGR_LIVE_UpdateEs (SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected);
#endif
extern HERROR	MGR_LIVE_UpdateServiceInfo (Handle_t hAction, HUINT32 ulSvcId, DbSvc_Info_t *pstSrcSvcInfo, DbSvc_Info_t *pstDstSvcInfo, HBOOL *pbChanged);

#endif /* !___MGR_LIVE_H___ */

