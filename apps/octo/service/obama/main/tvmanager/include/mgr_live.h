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

	/* - Description : �� Live Module�� Start ���
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : Service Handle (Supplementary, if it is Master/Supplementary Service Channel)
		   p2 : Master Service Handle
		   p3 : Play Type (0=Main, 1=PIP, 2=Live Streaming...) */
	eMEVT_LIVE_START_SVC,			// Command ���

	/* - Description : �� Live Module�� Stop ���
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : N.A
		   p2 : N.A
		   p3 : N.A */
	eMEVT_LIVE_STOP_SVC,			// Command ���

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

	/*	- Description : LIVE���� ���� ������ �Ͼ�� �˸��� �̺�Ʈ �޽���.
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : service handle
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_SVC_CHANGED,				// Notifier

	/*	- Description : LIVE���� PMT Update�� ���� ���� Svc �� PID (A, V, PCR) �� �ϳ��� ������
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : hSvcHandle
			p2 : tuning mode
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_CURRENT_SVC_PID_CHANGED,	// Notifier

	/*	- Description : LIVE���� MHEG�� quiet ���� Ʃ���� ��û�ϴ� �޽���. MSG_LIVE_START_SVC�� �޸�
					Replacement service ó�� LIVE�� ���� �޾Ƽ� ó���Ѵ�.
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : service handle.
			p2 : TBD
			p3 : TBD */
	eMEVT_LIVE_MHEG_QUIET_TUNE,

	/*	- Description : Live view moduel���� UI state ������ �Ͼ���� �˸��� �̺�Ʈ �޽���.
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : AppState
			p2 : UI state
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED,

	/*	- Description : Live Lock module���� UI state ������ �Ͼ���� �˸��� �̺�Ʈ �޽���.
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : AppState
			p2 : UI state
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_LOCK_STATE_CHANGED,

	/*	- Description : Complementary proc�� complementary start�� ��û�ϴ� �޽���
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : Complementary type
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_START_COMPLEMENTARY,

	/*	- Description : Complementary proc�� complementary stop�� ��û�ϴ� �޽���
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : xMGR_Complementary_e
			p2 : ApiSyncMode_t
			p3 : TBD
	*/
	eMEVT_LIVE_STOP_COMPLEMENTARY,

	/*	- Description : LIVE�� rating passed flag ������ �䱸��.
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : TRUE, FALSE
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_SET_RATING_PASSED_FLAG, //30

	/*	- Description : event relay�� �߻��߰� �����̵Ǵ� ä�� ���� ���� �߻�
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : hSvcHandle
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_EVENT_RELAY,

	/*	- Description : ��� �溸 ����� �߻��߰� �ش� ä�� ���� ���� �߻�
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : hSvcHandle
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_EVENT_EWS,

	/*	- Description : pin code dialog ���°� ����� -  ��
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : hSvcHandle
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_EVENT_RATING_CTRL_CHANGED,

	/*	- Description : svc lock �� ���¸� view proc�� �˸�
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : TBD
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_SVC_LOCKED,

	/*	- Description : svc lock �� ���¿��� correct pin�� �ԷµǾ� svc unlock��
		- Parameters Usage :
			handle : Action handle. Session version�� 0
			p1 : TBD
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_SVC_UNLOCKED,

	/*	- Description : set main action(pip swap)�� �� live proc(view/lock)�� �˸��� msg
		- Parameters Usage :
			handle : N/A
			p1 : new main action id
			p2 : old main action id
			p3 : TBD
	*/
	eMEVT_LIVE_SET_MAINVIEW_ACTION,

	/*	- Description : unselect componet�� �����ϴ� msg. Audio/Video�� ���� ����
		- Parameters Usage :
			handle : action Handle
			p1 : bUnselectVideoFlag
			p2 : bUnselectAudioFlag
			p3 : TBD
	*/
	eMEVT_LIVE_SET_UNSELECT_COMPONENT,

	/* - Description : MHEG���� ������ ��ۿ��� PID ������ ��û�ϴ� �޽���. DB���� PID ������ ������ ���� TEMP PID�̹Ƿ� DB�� �����ϸ� �ȵ�.
	   - Parameters Usage :
		   handle : Action handle
		   p1 :
				Video codec(high 16bit ���. 1. Valid DxVideoCodec_e => ���� 2. CODEC_CURRENT => No change)
		   		Video PID (low 16bit ���. 1. Valid PID => ���� 2. PID_NULL => stop 3. PID_CURRENT => No change)
		   p2 :
				Video codec(high 16bit ���. 1. Valid DxAudioCodec_e => ���� 2. CODEC_CURRENT => No change)
		   		Audio PID (low 16bit ���. 1. Valid PID => ���� 2. PID_NULL => stop 3. PID_CURRENT => No change)
		   p3 : PCR PID (low 16bit ���. 1. Valid PID => ���� 2. PID_NULL => stop 3. PID_CURRENT => No change) */
	eMEVT_LIVE_CHANGE_TEMP_PID,

	/*	- Description : fcc svc�� main live svc�� ��ȯ�Ѵ�
		- Parameters Usage :
			handle : new main live action Handle (fcc -> main)
			p1 : old main live action id (main -> fcc)
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_SET_FCC_MAINVIEW_ACTION,

	/*	- Description : fcc svc -> main svc�� ��ȯ�ÿ� last tune status�� �˸���
		- Parameters Usage :
			handle : action Handle
			p1 : tune status
			p2 : TBD
			p3 : TBD
	*/
	eMEVT_LIVE_NOTIFY_FCC_TUNE_STATUS,

	/*	- Description : fcc svc -> main svc�� ��ȯ�ÿ� live view���� �����ϴ� mw svc�� ������� �ʿ��� svc���� ������Ų��.
		- Parameters Usage :
			handle : action Handle
			p1 : current main action id
			p2 : new main action id
			p3 : TBD
	*/
	eMEVT_LIVE_STOP_MW_SVC_FOR_FCC,

	/*	- Description : hbbtv red button���� prm hdplus�� ���Խ� browser���� �˸������� �����.
		- Parameters Usage :
			handle : action Handle
			p1 : TBD
			p2 : TBD
			p3 : hdplusredbutton
	*/
	eMEVT_LIVE_SET_HBBTV_ON_HDPLUS_REDBUTTON,

	/*	- Description : Mheg app �� status �� �˸���.
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
	eLiveViewSession_MHEG_QUIET,	/**< MHEG, MHP�� data ��ۿ��� ��û�� silent tuning */
	eLiveViewSession_REPLACE,		/**< Free CA mode/running status change�� ���� replacement service tuning */
	eLiveViewSession_SUBFEED,		/**< Sub-feed service session */
	eLiveViewSession_TSMF_LOW,		/**< �Ϻ��� TSMF ������ �̵� ���� */
} MgrLiveView_SessionType_t;

typedef enum
{
	eNO_IPLATE				 = 0x01,	/**< i-plate display ���� Ʃ����. */
	ePINCODE_ON_TOP			 = 0x02,	/**< PIN code�� exit������ �޴� ���� ��. Exit�ϸ� �ٴڿ� ��.*/
	ePINCODE_PASSED			 = 0x04,	/**< PIN code�� �ܺο��� pass ����.. pin code pass�� ����. */
	eKEEP_PINPASS			 = 0x08,	/**< PIN code pass flag�� ���� session ��� �����϶�. (ex. subfeed���� �̵�... ) */
	eFORCE_TUNE				 = 0x10,	/**< ���� Ʃ������ ���� �ڵ��̶� ������ �� Ʃ�� ��Ű���� �Ѵ�. */
	eNDT_TUNE				 = 0x20,	/**< MHEG�� Non Destructive Tune�� ���� mode�� ���� Video�� Audio�� ���� �ʰ� ����� redkey�� ������ service handle �� MHEG manager�� �����ϱ� ����. */
	eCA_ALTER_TUNE			 = 0x40,	/**< CA ��ü ���񽺿� ���� Ʃ�׵�.	*/
	eEWS_TUNE				 = 0x80,	/**< EWS������ Ʃ�׵�.	*/
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
	HINT32			nHdVideoBitrateKbps;	// Live�� HD�̸�, �ٸ� Bitrate���� �ʿ�� �ϸ� ���
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
  *	������ MediaCtrl_LiveView_State_e �� �ݵ�� sync ����� ��
**/
typedef enum
{
	eLiveViewUi_OK,					/**< ���� ��û ����. No error */
	eLiveViewUi_NONE,				/**< initial state */
	eLiveViewUi_EMPTY_SVC,			/**< ���� �ߴ� �޽��� */
	eLiveViewUi_NO_SIGNAL,			/**< No Signal ���� �޽��� */
	eLiveViewUi_NOK_ANTENNA,		/** <Aantenna fail����(������ ��� short-circuit) */
	eLiveViewUi_MOTOR_MOVING,	/**< Motor antenna �̵� �޽��� */
	eLiveViewUi_CAS,					/**< CAS block ����. ���� �޽����� CAS application�� ����� */
	eLiveViewUi_SVC_LOCKED,			/**< ���� lock PIN code ����. pin �Է� ó���� lock proc���� ����ϸ� unlock�� msg�� ���� ���� */
	eLiveViewUi_AV_UNDER_RUN,		/**< Scrambled �Ǵ� �߸��� PID�� ���� ���� AV stream�� ������ �ʴ� ���� */
	eLiveViewUi_HDMI_BLOCKED,		/**< Authentication failure�� ���� HDMI���� AV block�Ǿ���. */
	eLiveViewUi_NO_VIDEO,			/**< Video�� ���� TV ä�ο��� black ȭ�� ������ �޽���. */
	eLiveViewUi_NO_AV,				/**< Audio, Video PID�� ��� ���� ��� �˸�. */
	eLiveViewUi_NOT_RUNNING,		/**< SDT�� Running Status �� NOT running���� �ٲ�� ��� �޼��� . */
	eLiveViewUi_NO_SIGNAL_TIMEOUT,	/**< eLiveViewUi_NO_SIGNAL���°� 30�� �̻� ���ӵɶ�. WTV_UiState_t�� �ٲ�� app_state_t�� ���Ծ��� */
	eLiveViewUi_REMOVE_CH,			/** < ��û ���� ä���� ������ ��� **/

	eLiveViewUi_MAX
} MgrLiveView_UiState_e;

/**
  *	������ MediaCtrl_LiveLock_State_e �� �ݵ�� sync ����� ��
**/
typedef enum
{
	eLockUi_OK,						/**< ���� ��û ����. No error */
	eLockUi_NONE,					/**< initial state */
	eLockUi_LOCK_PIN,				/**< ���� lock PIN code ����. ���� PIN code �Է��� PIN code application�� �����. */
	eLockUi_LOCK_PIN_DENIED,		/**< ���� lock PIN code ����. ���� PIN code �Է��� PIN code application�� �����. */
	eLockUi_RATING_PIN,				/**< Rating PIN code ����. ���� PIN code �Է��� PIN code application�� �����. */
	eLockUi_RATING_PIN_DENIED,		/**< Rating PIN code ����. ���� PIN code �Է��� PIN code application�� �����. */
	eLockUi_SC_PIN,					/**< Smart card PIN code ����. ���� PIN code �Է��� PIN code application�� �����. <<- live module���� ������� ���� */
	eLockUi_SC_PIN_DENIED,			/**< Rating PIN code ����. ���� PIN code �Է��� PIN code application�� �����. <<- live module���� ������� ���� */
	eLockUi_NO_EIT,					/**< EIT ���� ����. */

	eLockUi_MAX
} MgrLiveLock_UiState_e;

typedef enum
{
	eLiveViewPlaySvc_ACTUAL,		/**< Play ���� Service�ν� Action SVC (PMT based, PID based)   */
	eLiveViewPlaySvc_MASTER 		/**< Play ���� Service�ν� Action SVC (EIT based, Title based)   */
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
  @remark	zapping ��� ���� �ʱ�ȭ.\n
  @remark   API Type : \n

  @return   HERROR
*/
extern void	MGR_LIVE_ZapInit(void);

/**
  @remark	zapping ��� ���� �ʱ�ȭ.\n
  @remark   API Type : \n

  @return   HERROR
*/
extern void	MGR_LIVE_ResetZappingInfo (HBOOL bLoadSetup);


/**
  @remark   Channel service ������ last service info�� ����Ѵ�. ���� ���񽺰� ��ϵǸ� ���õȴ�.
  			�� ���񽺰� ��ϵǸ� ���� ���������� service type�� delivery type���� �з��Ǿ� ���� ä�η� ��ϵȴ�.
  @remark	type : Inline

  @return	none
*/
extern void	MGR_LIVE_RegisterLastSvcInfo (Handle_t			hSvc);			/**< [in] ����� ���� ���� �ڵ� */

/**
  @remark   Last service handle�� ��.
  @remark	type : Inline

  @return	ERR_OK : Valid last service info.
  			ERR_FAIL : ����� last service ������ ����.
*/
extern HERROR	MGR_LIVE_GetPrevSvcHandle (Handle_t			*phPrevSvc);			/**< [out] Last service handle�� ��ư� ������ ������. */


/**
  @remark   Item�� �����Ͽ� last service handle�� ��.
  @remark	type : Inline

  @return	ERR_OK : Valid last service handle.
  			ERR_FAIL : �Է� �Ķ���� ����.
*/
extern HERROR	MGR_LIVE_GetLastSvcHandle (MgrLive_LastSvcItem_t	eLastSvcItem,		/**< [in] �о last service handle item */
											Handle_t				*phLastSvc			/**< [out] Last service handle�� ��ư� ������ ������ */
											);

/**
  @remark   Svc Lock Pass �� ���� �ִ� Svc�ΰ�?
  @remark	type : Inline

  @return	TRUE : Passed
*/
extern HBOOL	MGR_LIVE_SvcLockIsPassed (HUINT32 ulActionId, Handle_t hSvc);

/**
  @remark   Svc Lock Pass �� ���� Svc�� ����?
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL :
*/
extern HERROR	MGR_LIVE_SvcLockSetPassed (HUINT32 ulActionId, Handle_t hSvc);

/**
  @remark   Svc Lock Passed ���� Reset
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL :
*/
extern HERROR	MGR_LIVE_SvcLockResetPassed (HUINT32 ulActionId);

/**
  @remark   ���� service handle�� group�� delivery type�� ��.
  @remark	type : Inline

  @return	ERR_OK : Valid last service info.
  			ERR_FAIL : ����� last service ������ ����.
*/
extern HERROR	MGR_LIVE_CurrentSvcInfoGet(HUINT32				ulActionId,			/**< [in] ������ ���ϴ� action ID */
											Handle_t			*phCurSvc 			/**< [out] ������ service handle */
											);
/**
  @remark   ���� service handle�� group�� ������.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL :
*/
extern HERROR	MGR_LIVE_CurrentSvcInfoSet(HUINT32			ulActionId,
										Handle_t			hCurSvc
										);


/**
  @remark   �ӽ� PID play.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL : �Է� �Ķ���� ����.
*/
extern HERROR	MGR_LIVE_CmdChangeTempPid(Handle_t			hAction,
										DxVideoCodec_e		eVideoCodec,
										HUINT16				usVideoPid,
										DxAudioCodec_e		eAudioCodec,
										HUINT16				usAudioPid,
										HUINT16				usPcrPid
										);

/**
  @remark   Main view �� ���Ͽ� service start ��Ŵ. Send message�� ���� sync ������ �Ͼ.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL : �Է� �Ķ���� ����.
*/
extern HERROR	MGR_LIVE_CmdSendServiceStartMainView(Handle_t hSvc, ChListUiGroup_t eChListUiGroup, HUINT32 ulTuningMode);

/**
  @remark   Main view �� ���Ͽ� service start ��Ŵ. Post message�� ���� async ������ �Ͼ.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL : �Է� �Ķ���� ����.
*/
extern HERROR	MGR_LIVE_CmdPostServiceStartMainView(Handle_t hSvc, ChListUiGroup_t eChListUiGroup, HUINT32 ulTuningMode);

/**
  @remark   Action ID�� �����Ͽ� service start ��Ŵ. Send message�� ���� sync ������ �Ͼ.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL : �Է� �Ķ���� ����.
*/
extern HERROR	MGR_LIVE_CmdSendServiceStartByActionId(HUINT32 ulActionId, Handle_t hSvc);

/**
  @remark   Action ID�� �����Ͽ� service start ��Ŵ. Post message�� ���� async ������ �Ͼ.
  @remark	type : Inline

  @return	ERR_OK :
  			ERR_FAIL : �Է� �Ķ���� ����.
*/
extern HERROR	MGR_LIVE_CmdPostServiceStartByActionId(HUINT32 ulActionId, Handle_t hSvc);


//		mgr_live_pincode.c
// Live, Tsr Playback, Chase Playback ���߿� Pincode �� Ǯ��, �� ������ eventid �� parental rate�� ����Ʈ�� �����Ѵ�.
extern HERROR	MGR_LIVE_PincodeSetRatingPassedEventId(Handle_t hSvc, HINT32 nEvtId, HUINT8 ucPRate);

// Live, Tsr Playback, Chase Playback ���߿� ���ο� event �� ������, �� event �� ������ ���� �ִ��� Ȯ���Ѵ�.
extern HERROR	MGR_LIVE_PincodeIsEventIdRatingPassed(Handle_t hSvc, HINT32	nEvtId, HUINT8 *ucPRate, HBOOL *bIsRatingPassed);

// Standby �� ������ ��, ������ Pincode ���� ����Ʈ�� clear �Ѵ�.
extern HERROR	MGR_LIVE_PincodeDeleteAllServiceHandle(void);

#if 0
extern HERROR	MGR_LIVE_GetSiUpdate (SvcSi_UpdateEs_t *pstUpdate, DbSvc_Info_t *pstOrgInfo, DbSvc_Info_t *pstDestInfo);
extern HBOOL	MGR_LIVE_CompareSvc (const void *pvInfo1, const void *pvInfo2, HUINT32 ulSize);
extern HERROR	MGR_LIVE_UpdateSvc (const SvcSi_PmtEsInfo_t *pstPmtEsInfo, void *pvSvcInfo);
extern HERROR	MGR_LIVE_UpdateEs (SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, void *pvSvcInfo, SvcSi_SelectedEs_t *pstSelected);
#endif
extern HERROR	MGR_LIVE_UpdateServiceInfo (Handle_t hAction, HUINT32 ulSvcId, DbSvc_Info_t *pstSrcSvcInfo, DbSvc_Info_t *pstDstSvcInfo, HBOOL *pbChanged);

#endif /* !___MGR_LIVE_H___ */

