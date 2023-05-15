#ifndef	___MGR_PLAYBACK_H___
#define	___MGR_PLAYBACK_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <svc_pb.h>
#include <svc_rec.h>
#include <svc_meta.h>
#include <svc_si.h>

#include <mgr_common.h>


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eMEVT_PLAYBACK		= eMEVT_PLAYBACK_START,

	/* - Description : player app의 재생 시작을 알림
	   - Parameters Usage :
		   handle : action handle
		   p1 : play type
		   p2 : cotentId
		   p3 : NA */
	eMEVT_PVR_NOTIFY_PLAYBACK_STARTED,


	/* - Description : player app의 재생 종료를 알림
	   - Parameters Usage :
		   handle : action handle
		   p1 : play type
		   p2 : cotentId
		   p3 : NA */
	eMEVT_PVR_NOTIFY_PLAYBACK_STOPPED,

	/* - Description : player app의 에러에 의한 재생 강제 종료를 알림
	   - Parameters Usage :
		   handle : NA
		   p1 : error type
		   p2 : cotentId
		   p3 : NA */
	eMEVT_PVR_NOTIFY_PLAYBACK_ERROR_STOPPED,

	/* - Description : PB 종료후 Live 로 돌아가야함을 알림.
	   - Parameters Usage :
		   handle : action handle
		   p1 : play type
		   p2 : cotentId
		   p3 : NA */
	eMEVT_PVR_NOTIFY_PLAYBACK_CHANGE_TO_LIVE,


	/*	- Description : pin code dialog 상태가 변경됨 -  일
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : bBlocked
			p2 : ulRatingValue
			p3 : NA*/
	eMEVT_PVR_NOTIFY_PLAYBACK_EVENT_RATING_CTRL_CHANGED,

	/*	- Description : DRM 상태에 따라 AV가 block되는 상황의 UI 처리를 위한 message
		- Parameters Usage :
			handle : Action handle. Session version은 0
			p1 : bBlocked
			p2 : NA
			p3 : NA*/
	eMEVT_PVR_PLAYBACK_EVENT_DRM_BLOCK,

	/*	- Description : Playback State Changed
			- Parameters Usage :
				handle : Action handle. Session version은 0
				p1 : State Type
				p2 : NA
				p3 : NA*/
	eMEVT_PVR_PLAYBACK_STATE_CHANGED,


	/* - Description : Playback 을 정지시킨다. (TSR PB/Chase PB/Normal PB 모두 포함)
				(MSG_PVR_PLAYBACK_STOP은 Playback이 정지되었음을 notify하는 것이고, 이것은 request 하는 것이다.)
	   - Parameters Usage :
		   handle : action handle (HANDLE_NULL이면 그냥 Main Action)
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_PVR_PLAYBACK_STOP,
};

typedef enum
{
	eMgrPb_CasType_None				= 0x00000000,

	eMgrPb_CasType_CiPlus 			= 0x00000001,
	eMgrPb_CasType_Nagra 			= 0x00000002,
	eMgrPb_CasType_NagraSES			= 0x00000004,
	eMgrPb_CasType_Irdeto 			= 0x00000008,
	eMgrPb_CasType_Viaccess			= 0x00000010,
	eMgrPb_CasType_Conax			= 0x00000020,

	eMgrPb_CasType_End
} MgrPb_CasType_e;

typedef enum
{
	eMgrPb_PlayType_Normal			= 0x00000001,				/**< recorded playback */
	eMgrPb_PlayType_Chase			= 0x00000002,				/**< recording playback */
	eMgrPb_PlayType_Tsr				= 0x00000004,				/**< tsr playback */
	//
	eMgrPb_PlayType_Transcoder_SectionGathering	= 0x00000010,	/* TrRec을 하기 전에 PMT 등의 첫번째 Section 을 얻기위한 과정  */
	eMgrPb_PlayType_Transcoder		= 0x00000020,
	//
	eMgrPb_PlayType_NoNeedCasCi		= 0x10000000,				/**< descrambler 가 필요 없음. FTA이거나, Descramble 된 스트림임 */
	eMgrPb_PlayType_NeedCasCi		= 0x20000000,				/**< descrambler 가 필요함. */
	//
	eMgrPb_PlayType_RecentPos		= 0x01000000,				/**< 가장 최근에 Play하던 위치부터 재생 시작  */
	eMgrPb_PlayType_EndPos			= 0x02000000,				/**< End Position 위치에서 Pause/FR/Slow 을 시작 */
	eMgrPb_PlayType_FirstPos		= 0x04000000,				/**< 처음 위치부터 재생 시작 */
	eMgrPb_PlayType_PosMask			= 0x0F000000,

	eMgrPb_PlayType_END
} MgrPb_PlayType_e;

typedef enum
{
	eMgrPb_TickState_STOP	= 0,
	eMgrPb_TickState_PLAY,
	eMgrPb_TickState_PAUSE,
	eMgrPb_TickState_SLOW,
	eMgrPb_TickState_FF,
	eMgrPb_TickState_FR,
} MgrPvr_TrickState_e;


typedef enum
{
	eMgrPb_Speed_X1 = 0,
	eMgrPb_Speed_X2,
	eMgrPb_Speed_X3,
	eMgrPb_Speed_X4,
	eMgrPb_Speed_X5,	// 2, 4, 8, 16, 32 배속으로 5단계
	eMgrPb_Speed_X6,
} MgrPb_TrickSpeed_e;

typedef enum
{
	eMgrPb_MIN_REQUIRED_PLAYBACK_TIME			= 0,
} MgrPb_ParamType_e;

typedef enum
{
	eMgrPb_NONE			= 0,		/**< 정상 시청 상태. No error */
	eMgrPb_CAS,						/**< CAS block 상태. 실제 메시지는 CAS application이 담당함 */
	eMgrPb_LOCK_PIN,					/**< 유저 lock PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eMgrPb_LOCK_PIN_DENIED,			/**< 유저 lock PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eMgrPb_RATING_PIN,				/**< Rating PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eMgrPb_RATING_PIN_DENIED,		/**< Rating PIN code 상태. 실제 PIN code 입력은 PIN code application이 담당함. */
	eMgrPb_PB_LOCK,					/**< Medialist 에서 Playback 시작시 Lock 이 걸린 경우 */
	eMgrPb_PB_LOCK_DENIED,			/**< Medialist 에서 Playback 시작시 Lock 이 걸린 경우 */
	eMgrPb_AV_UNDER_RUN,				/**< Scrambled 또는 잘못된 PID의 경우와 같이 AV stream이 들어오지 않는 상태 */
	eMgrPb_HDMI_BLOCKED,				/**< Authentication failure로 인해 HDMI에서 AV block되었음. */
	eMgrPb_NO_AV,					/**< Audio, Video PID가 모두 없는 경우 알림. */
	eMgrPb_NO_VIDEO,					/**< Video가 없는 TV 채널에서 black 화면 방지용 메시지. */
} MgrPb_State_e;

typedef enum
{
	eMgrPb_Option_None				= 0x0000,
	eMgrPb_Option_StartPaused		= 0x0001,
	eMgrPb_Option_UseLatestPbTime	= 0x0002,

	eMgrPb_Option_All				= 0xffff
} MgrPb_Option_e;

typedef struct
{
	MgrPb_PlayType_e	ePlayType;
	MgrPvr_TrickState_e 	eTrickState;
	HUINT32					ulStartTime;
	HUINT32					ulContentId;
	Handle_t 				hCurrSvc;
	Handle_t 				hMasterSvc;
	ChListUiGroup_t			eSvcListGroup;
	HBOOL					bMakeThumbnail;
	HBOOL					bShowMediaList;
	HBOOL					bIsUsbFlashRecord;
	HBOOL					bStartPaused;
} MgrPb_PlaySetup_t;

typedef struct
{
	Handle_t				hAction;
	HUINT32					ulContentId;

	UNIXTIME				ulStartTime;
	UNIXTIME				ulPlayTime;
	UNIXTIME				ulEndTime;

	HINT32					nPlaySpeed;
	MgrPvr_TrickState_e		eTrickState;
	MgrPb_TrickSpeed_e		eTrickSpeed;

	Handle_t 				hSvc;
	Handle_t 				hMasterSvc;
	ChListUiGroup_t 		eSvcListGroup;

	HBOOL					bPinLock;

	HBOOL					bPbStart;
	HINT32					nPbState;

	HBOOL					bDrmValid;
	DxCopyrightControl_t				stDrmInfo;
} MgrPb_Info_t;


HERROR			MGR_PLAYBACK_Init(void);
HERROR			MGR_PLAYBACK_GetPbSiSpec (SvcSi_PbSpec_t *pstSiSpec);
HERROR 			MGR_PLAYBACK_GetPbCasTypes (HUINT32 *pulCasTypes);
HERROR 			MGR_PLAYBACK_GetMediaPbSiSpec (SvcSi_MediaPbSpec_t *pstSiSpec);
HERROR			MGR_PLAYBACK_SetPlaySetup (HUINT32 ulActionId, MgrPb_PlaySetup_t *pstPbSetup);
BUS_Callback_t	MGR_PLAYBACK_GetProcFunc(HUINT32 ulActionId);

HERROR			MGR_PLAYBACK_SetPlaySpeed (HUINT32 ulActionId, HINT32 nPlaySpeed);
HERROR			MGR_PLAYBACK_SetJumpPlay(HUINT32 ulActionId, HUINT32 ulTimeToPlay);
HERROR			MGR_PLAYBACK_GetPlaybackInfo(HUINT32 ulActionId, MgrPb_Info_t *pstInfo);
HERROR			MGR_PLAYBACK_GetPlaybackTimeInfo(HUINT32 ulActionId, MgrPb_Info_t *pstInfo);
HERROR			MGR_PLAYBACK_StopPlayback (HUINT32 ulActionId);
HERROR			MGR_PLAYBACK_SetAudioInfo (HUINT32 ulActionId, HUINT32 ulIndex, DxStereoSelect_e eSoundMode);
HERROR			MGR_PLAYBACK_SetSubTitleDisplay (HUINT32 ulActionId, HBOOL bDisplay);
HERROR			MGR_PLAYBACK_SetSubtitleLanguage (HUINT32 ulActionId, HUINT32 ulIndex);
HERROR			MGR_PLAYBACK_GetSubtitleDisplay (HUINT32 ulActionId, HBOOL *pbDisplay);
HERROR			MGR_PLAYBACK_GetServiceInfo (HUINT32 ulActionId, SvcMeta_Service_t *pstService);
HERROR			MGR_PLAYBACK_SetServiceInfo (HUINT32 ulActionId, SvcMeta_Service_t *pstService, HBOOL bUpdate);
HUINT32			MGR_PLAYBACK_GetPlaybackParam (MgrPb_ParamType_e eType);
HBOOL			MGR_PLAYBACK_IsPlaybackId(HUINT32 ulContentId, MgrPb_Info_t *pstInfo);

HERROR			MGR_PLAYBACK_UpdateServiceInfo (Handle_t hAction, HUINT32 ulSvcId, SvcMeta_Service_t *pstSrcSvcInfo, SvcMeta_Service_t *pstDstSvcInfo, HBOOL *pbChanged);

HERROR 			MGR_PLAYBACK_GetMenuSubtitleLangIndex(DbParam_Item_e eSubtitleItem, SvcSi_SubtitleList_t *pstSubtitleList, HINT32 *pnIndex);

#endif /*___MGR_PLAYBACK_H___*/
