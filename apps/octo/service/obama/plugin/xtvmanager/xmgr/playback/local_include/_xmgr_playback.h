#ifndef	__XMGR_PLAYBACK_INT_H__
#define	__XMGR_PLAYBACK_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <svc_si.h>
#include <mgr_playback.h>


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/* local macros using in proc msg  */
#define PB_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5)	\
			{													\
				(void)arg1;										\
				(void)arg2;										\
				(void)arg3;										\
				(void)arg4;				 						\
				(void)arg5;				 						\
			}

#define PB_POINTER_INVALID_CHECK(pArg, returnValue)				\
			if(pArg == NULL)									\
			{													\
				HxLOG_Critical("\n\n");							\
				return returnValue;								\
			}

#define PB_HERROR_INVALID_CHECK(pArg, returnValue)				\
			if(pArg != ERR_OK)									\
			{													\
				HxLOG_Critical("\n\n");							\
				return returnValue;								\
			}

#define CHECK_PB_ACTION_HANDLE()	\
	do {																	\
		HUINT32			 ulMsgActionId = SVC_PIPE_GetActionId (hAction);		\
		if (SVC_PIPE_GetActionId (pstContext->hAction) != ulMsgActionId)		\
		{																	\
			return MESSAGE_PASS;											\
		}																	\
	} while(0)

#define SCREEN_SAVER_TIMER_ID				200
#define PB_EOF_TIMER_ID						201
#define RADIO_BACKGROUND_IMAGE_ID			202
#define PB_RESERVED_TRICK_START_TIMER_ID	203
#define PB_RESERVED_JUMP_START_TIMER_ID		204
#define PB_EXTRACT_THUMB_TIMER_ID			205

#define PB_EOF_TIMEOUT_DURATION				(30*1000)	/* Timebar 가 30 secs 가 아무 입력 없으면 사라진다. */
#define PB_RESERVED_TRICK_START_TIMEOUT		(850)		// 0.85초
#define PB_RESERVED_JUMP_START_TIMEOUT		(10)		// 0.85초
#define PB_EXTRACT_THUMB_TIME					(500)
#define BOOKMARK_AVAILABLE_POSITION			4

#define PANEL_STRING_TIMER_ID				0x1001		// "      PLAY      " 를 2초간 보여줘야 한다.
#define PANEL_STRING_TIME_OUT				2000		// 2 초
#define JUMP_SKIP_TIME						120
#define JUMP_REPLAY_TIME					15

#define MAX_PB_THUMB					100		// 속도 문제가 있으니 일정한 간격으로 정해진 개수만큼만 thumbnail을 추출한다.

// for CI+ v1.3
/** @brief [CI+ DRM state: CI+ DRM이 걸려 A/V가 막힐 수 있는 상태 List] */
// or 연산을 이용하여 여러개가 동시에 관리 됨.
typedef enum
{
	eXmgrPbCiPlusDrmState_None			= 0,

	eXmgrPbCiPlusDrmState_RL			= 0x0001,	// Retention Limit에 걸려있는 상태
	eXmgrPbCiPlusDrmState_PC			= 0x0002,	// CI+ 1.3 spec의 Parental Control 시나리오에 걸려있는 상태
	eXmgrPbCiPlusDrmState_License		= 0x0004,	// CI+ 1.3 spec의 license 권한에 걸려있는 상태
} xmgrPb_CiPlusDrmState_e;

typedef enum
{
	eXmgrPbState_IDLE		= 0,
	eXmgrPbState_STARTED,				/**<  */
	eXmgrPbState_WAIT_PMT,				/**< PMT Pid 에 변경이 발생하여 PMT 를 다시 걸고 정보를 기다리는 상태 */
	eXmgrPbState_STOPPED,
	eXmgrPbState_WAIT_PB_LOCK,			/**< Player 가 Lock 해제를 대기하는 상태 */
	eXmgrPbState_WAIT_PB_SETUP,			/**< Player 가 setup 되기를 기다리는 상태 */
	eXmgrPbState_WAIT_AV_STARTED,		/**< A/V 가 start 되기를 기다리는 상태 */
	eXmgrPbState_WAIT_RATING,			/**< Rating정보를 기다리는 상태 */
	eXmgrPbState_LOCK_PIN, 				/**< 유저 Lock된 서비스의 PIN code 입력 대기 상태 */
	eXmgrPbState_RATING_PIN,			/**< Rating PIN code 입력 대기 상태 */
	eXmgrPbState_AV,					/**< AV가 나오는 정상적인 TV시청 상태 */
	eXmgrPbState_CAS_BLOCKED,			/**< CAS에 의하여 시청 제한되어 있는 상태 */
} xmgrPb_State_e;

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	MgrPb_PlaySetup_t		 stSetup;

	HUINT32				ulActionId;					/**< action Id */
	Handle_t			hAction;					/**< 이 context의 현재 zapping session version이 포함된 action handle */
	HUINT32				ulVideoDecId;
	HUINT32				ulPlayerId;
	HUINT32				ulDemuxId;
	HUINT8				ucPlayIdx;					/**< actual recorder instance number : 0, 1 */
	HUINT32				ulContentId;				/**< booting 이후의 ts 파일 재생/녹화를 위한 고유의 ID */
	xmgrPb_State_e		ePbState;					/**< context status */

	/*
		Trick을 위해 eTrickState 와 eTrickSpeed 를 차후 없애야 하고 최대한 nPlaySpeed 를 쓰도록 고친다.
	*/
	HINT32				nPlaySpeed;
	//
	//
	MgrPvr_TrickState_e		eFirstTrickState;			/**< surfer 상에서 처음 trick 진입하고자 한 trickstate 임. Pause, Slow, FR, play */
	MgrPvr_TrickState_e		eTrickState;				/**< 재생의 Trick 상태 */
	MgrPb_TrickSpeed_e		eTrickSpeed;				/**< 재생의 Trick 상태 */
	MgrPvr_TrickState_e		ePrevTrickState;			/**< PMT Update전의 Trick Status */
	MgrPb_TrickSpeed_e		ePrevTrickSpeed;			/**< PMT Update전의 Trick Status */
	HBOOL					eValidPrevTrick;			/**< PMT Update시 해당 Flag Set */

	MgrPb_PlayType_e	ePbType;					/**< 한 파일에 대한 재생 방식 */
	MgrPb_State_e		uiState;					/* 이 context의 UI state */
	Handle_t 			hSvc;						/**< TSR_Playback, Chase_Playback 시작시 WTV 의 service Handle 값을 copy 한다 */
	Handle_t 			hMasterSvc;					/**< TSR_Playback, Chase_Playback 시작시 WTV 의 master service Handle 값을 copy 한다 */
	ChListUiGroup_t 	eSvcListGroup;				/**< TSR_Playback, Chase_Playback 시작시 WTV 의 ChListUiGroup_t 값을 copy 한다 */
	UNIXTIME			ulStartTime;				/**< 재생 start time */
	UNIXTIME			ulEndTime;					/**< 재생 end time */
	UNIXTIME			ulPlayTime;					/**< 현재 재생되는 시간 */
	UNIXTIME			ulOrgStartTime;				/**< Recording 되었을 때의 start time */
	UNIXTIME 			ulStartTimeByDrm;			/** <allowed start time by drm*/
	HINT32 				nChNum;						/**< channel service number */
	SvcMeta_Service_t		svcInfo;					/**< 녹화중에 저장된 서비스 information 으로, playback 시 사용한다. */
	HBOOL				bRadioBackground;			/* True if radio background is enabled */
	HBOOL				bSubtitleAllowed;			/* Subtitle 시작해도 된다는 flag. MENU에서 AUTO이거나 subtitle list popup이 start한 경우 TRUE */
	HBOOL				bTtxVbiStarted;				/* TELETEXT가 VBI insert용으로 시작됨. Page show가 아니면 subtitle은 enable될 수 있음.
														단, teletext는 한 session당 한번만 start 되어야 함. */
	HBOOL				bTtxPageEnabled;			/* TELETEXT page를 보여주고 있음. Subtitle은 enable될 수 없음. */
	SvcSi_SubtitleInfo_t	subtitleInfo;
	HBOOL				bSubTitleOn;				// Subtitle Running ?
	HBOOL				bPmtValid;					/**< PMT가 전달됨. */
	HBOOL				bRestartAV;
	HBOOL				bLockPinPassed;				/* 현재의 session에서 lock PIN code가 pass 되었는가를 나타냄. */
	HBOOL				bRatingPinPassed;			/* 현재의 session에서 rating PIN code가 pass 되었는가를 나타냄. */
	HBOOL				bRatingPassByLockPass;		/* Lock pass에 의하여 rating도 pass로 처리하라는 flag */
	HINT8				*pszPinAppStr;				/* PIN code applicatin에서 UI로 보여주는 메시지 스트링 */

	HUINT32				ulEventId;					/* Current event ID */
	HBOOL				bRatingValid;				/* EIT에 rating값이 들어왔거나 EIT timeout이 발생하여 Rating value를 사용가능함 */
	HUINT32				ulRatingValue;				/* 적용할 Rating 값 */
	HBOOL				bAVHide;					/* AV Hide */
	HBOOL				bAlreadyExistEit;			/* trick 중에 eit timeout 나와도 무시하기 위함. */
	HBOOL				bPBStart;
	HBOOL				bFirstPmt;
	HBOOL				bFirstTimecode;

	HBOOL				bIsEOF;						/* EOF 인지 체크 */

/* AV decoder management */
	HBOOL				bVideoStarted;
	HBOOL				bAudioStarted;
	HBOOL				bAvUnderrun;				/* AV underrun flag */
	HBOOL				bHdmiBlocked;				/* HDMI was blocked by HDCP fail or etc. */

/* CAS management */
	HBOOL				bCasOk;						/* CAM 에게 시청 허가를 받았는가? */
	HBOOL				bSCPinCodeValid;			/* Smart card Pin code가 사용 가능한가? */
	HBOOL				bSCPinPassed;				/* Smart card PIN passed flag. 사양에 따라 필요하면 사용. */
	HINT32				nCasErrCode;				/**< CAS error code 저장. 용도는? */
	HBOOL				bCasStarted;
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	HBOOL				bCnfPbLicense;			/* CI+ 1.3 : playback license cnf 상태설정. Host(meta) -> Cam 으로 license 전달 후 Cam -> Host 로 license confirm 할 경우 true. 초기 값은 true 설정*/
	xmgrPb_CiPlusDrmState_e	eCiPlusDrmState;			/* CI+ 1.3을 지원하는 모델에서 PB할 때 사용. CI+ 동작에 의해 A/V가 막혀야 하는 경우 그 상태를 기록. or 로 여러 state를 동시에 check */
#endif
#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	DxCAS_GroupId_e		nCasSystemId;				/* EVT_CAS_FAIL메시지의 nParam1으로 전달되는 CAS system ID (Nagra, Ir, Conax, ...) */
	void				*pCasInfo;					/* EVT_CAS_FAIL메시지의 nParam2로 전달되는 CAS proc에서 사용할 data point */
	HINT32				nCasEvt;					/* CAS proc에게 넘겨줄 CAS event (eSEVT_CAS_OK, eSEVT_CAS_FAIL) */
#endif
#if defined(CONFIG_MW_CAS_CONAX)
  #if defined(CONFIG_MW_CAS_CONAX_URI)
	DxCopyrightControl_ConaxDrmInfo_t 		stCxUri;
	HBOOL				bUriValid;
  #endif
	HUINT8				ucPrevProgLevel;
	UNIXTIME			ulCxNextUriTime;			/* next uri start time */
	UNIXTIME			ulUnixStartTime;			/* start time (ex 2014.07.02 22:10:34) */
	UNIXTIME			ulUnixEndTime;				/* end time (ex 2014.07.02 23:10:54) */
	HBOOL				bAvStarted; 				/* if check av started */
	HBOOL				bAllUriExpired; 			/* if check all content period are expired */
	HBOOL				bCardDisabled;				/* if card inserted or removed */

	UNIXTIME			ulPrevPlayTime;				/**< 이전 재생되는 시간 */
	HUINT8				ucLiveProgramLevel;
	HBOOL				bPauseByCAS;
#endif
	HBOOL				bSpeedReserved;					/* AV Enable 되기 전에 Speed 가 세팅됨 */
	HINT32				nReservedSpeed;					/* AV Enable 되기 전에 세팅된 Speed */

	HBOOL				bJumpReserved;					/* AV Enable 되기 전에 Jump 가 세팅됨 */
	HINT32				nReservedJump;					/* AV Enable 되기 전에 세팅된 Jump */


/* DRM & Copy protection management */
	HBOOL				bDrmValid;				/* Copy protection 정보가 detect 됨. */
	DxCopyrightControl_t			stDrmInfo;					/**< drm info by metafile(.hmt) */
	HUINT32				ulDrmUriTime;			/**< drm license 가 발생한 시간 정보 by metafile(.hjtsm) */
	HUINT32				ulDrmLicenseTime;			/**< drm license 가 발생한 시간 정보 by metafile(.hjtsm) */

/* thumbnail extraction */
	HUINT32				ulThumbInterval;
	HUINT32				ulThumbNum;
	HUINT8				aucThumb[MAX_PB_THUMB];
	HUINT32				ulNextThumbIdx;

	HBOOL				bIsPbStopByError;		/* 재생중 Error가 발생 되었을 때 MSG_PVR_PLAYBACK_NOTIFY_ERROR_STOPPED를 전달 하기 위해 */

	/* timeStamp of Meta */
	SvcMeta_TimestampList_t stTimeStampList;

} xmgrPbContext_t;


#define _________INTERNAL_FUNC__________
/*	xmgr_playback.c
 */
HUINT32 xmgr_playback_GetContextIndexFromActionId (HUINT32 ulActionId);
void xmgr_playback_SetCopyProtection(xmgrPbContext_t *pstContext);

#define _________PLUGIN_FUNC__________
/* BASE */
HERROR			xmgr_playback_Init_Base (void);
HERROR			xmgr_playback_SetPlaySetup_Base (HUINT32 ulActionId, MgrPb_PlaySetup_t *pstPbSetup);
BUS_Callback_t	xmgr_playback_GetProcFunc_Base (HUINT32 ulActionId);
HUINT32			xmgr_playback_GetPlaybackParam_Base (MgrPb_ParamType_e eType);
HERROR			xmgr_playback_SetPlaySpeed_Base (HUINT32 ulActionId, HINT32 nPlaySpeed);
HERROR			xmgr_playback_SetJumpPlay_Base (HUINT32 ulActionId, HUINT32 ulTimeToPlay);
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR			xmgr_playback_SetPlaySpeed_Ses(HUINT32 ulActionId, HINT32 nPlaySpeed);
HERROR 			xmgr_playback_SetJumpPlay_Ses(HUINT32 ulActionId, HUINT32 ulTimeToPlay);
#endif
HERROR			xmgr_playback_GetPlaybackInfo_Base (HUINT32 ulActionId, MgrPb_Info_t *pstInfo);
HERROR			xmgr_playback_GetPlaybackTimeInfo_Base (HUINT32 ulActionId, MgrPb_Info_t *pstInfo);
HERROR			xmgr_playback_StopPlayback_Base (HUINT32 ulActionId);
HERROR			xmgr_playback_SetAudioInfo_Base (HUINT32 ulActionId, HUINT32 ulIndex, DxStereoSelect_e eSoundMode);
HERROR			xmgr_playback_SetSubTitleDisplay_Base (HUINT32 ulActionId, HBOOL bDisplay);
HERROR			xmgr_playback_SetSubtitleLanguage_Base (HUINT32 ulActionId, HUINT32 ulIndex);
HERROR			xmgr_playback_GetSubtitleDisplay_Base (HUINT32 ulActionId, HBOOL *pbDisplay);
HERROR			xmgr_playback_GetServiceInfo_Base (HUINT32 ulActionId, SvcMeta_Service_t *pstService);
HERROR			xmgr_playback_SetServiceInfo_Base (HUINT32 ulActionId, SvcMeta_Service_t *pstService, HBOOL bUpdate);
HBOOL			xmgr_playback_IsPlaybackId_Base (HUINT32 ulContentId, MgrPb_Info_t *pstInfo);

HERROR 			xmgr_playback_GetMenuSubtitleLangIndex(DbParam_Item_e eSubtitleItem, SvcSi_SubtitleList_t *pstSutitleList, HINT32 *pnIndex);

/* Sub Module CIPLUS BASE */
BUS_Callback_t	xmgr_playback_CiPlusGetProcFunc_Base (HUINT32 ulActionId);

/* Sub Module IRDETO BASE */
BUS_Callback_t	xmgr_playback_IrdetoGetProcFunc_Base (HUINT32 ulActionId);

/* Sub Module NAGRA BASE */
BUS_Callback_t	xmgr_playback_NagraGetProcFunc_Base (HUINT32 ulActionId);

/* Sub Module CONAX BASE */
HERROR			xmgr_playback_ConaxInit_Base(void);
BUS_Callback_t	xmgr_playback_ConaxGetProcFunc_Base (HUINT32 ulActionId);
HERROR			xmgr_playback_ConaxSetJumpPlay_Base (HUINT32 ulActionId, HUINT32 ulTimeToPlay);

/* Sub Module VERIMATRIX BASE */
BUS_Callback_t	xmgr_playback_VerimatrixGetProcFunc_Base (HUINT32 ulActionId);

/* End Of BASE  */


/* SES */
BUS_Callback_t	xmgr_playback_GetProcFunc_Ses (HUINT32 ulActionId);


#endif /* __XMGR_PLAYBACK_INT_H__ */
