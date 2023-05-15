/**
	@file     ap_mheg_mgr.c
	@brief    MHEG engine 제어 application

	Description: MHEG엔진을 제어하는 application. Key와 system event를 받아 MHEG엔진이 필요한 형태로 가공하여 \n
				전달하고, MHEG이 발행하는 call 중 지연이나 통제가 필요한 이벤트들을 받아서 대신 처리한다.
	Module: AP/MHEG			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <hapi.h>

#include <octo_common.h>

#include <db_common.h>

#include <bus.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <mgr_svclist.h>
#include <ap_mheg_mgr.h>
#include <util.h>
#include <db_svc.h>
#include <db_param.h>
//#include "si_module.h"
//#include "si_process.h"
//#include "si_mheg.h"
#include <svc_si.h>
#include <svc_av.h>
#include <svc_pipe.h>
#include "svc_sbtl.h"
#include "svc_ttx.h"
#include "svc_output.h"
#include "port_itk_main.h"
#include "port_itk_service.h"
#include "port_itk_display.h"
#include "port_itk_avstream.h"
#include "port_itk_demux.h"
#include "port_itk_dsmccfs.h"
#include "port_itk_os.h"
#include "port_itk_font.h"
#include "port_itk_misc.h"
#include "port_itk_zlib.h"
#include "port_itk_http.h"
#include "port_itk_nvs.h"
#include "port_itk_crypto.h"
#include "port_itk_audioclip.h"
#include "port_itk_dvr.h"
#include "port_itk_appmmi.h"
#include "port_itk_console.h"

#include <mgr_pg.h>
#include <mgr_search.h>
#include <mgr_live.h>
#include <mgr_output.h>
#include <mgr_network.h>
#include <xmgr_spec.h>



#if defined (MHEG_UI_ORDER_BY_RUNNING_STATUS)
#include "ui_definition.h"
#endif
#if defined (CONFIG_MWC_OSD)
#include "osd_gfx.h"
#endif

//#include <pmgr_3rdparty.h>

#include <mgr_live.h>

#include <svc_mplayer.h>
#include <mgr_media.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MHEG_MGR_AVSTREAM_DEBUG		FALSE
#define MHEG_MGR_SCREEN_SAVER_DEBUG	FALSE
#define MHEG_MGR_STREAMING_AV_DEBUG	FALSE




#define ApMhegDbgPrint	HxLOG_Print
#define ApMhegAssert	HxLOG_Assert


#ifdef CONFIG_DEBUG
#define MhegMgrLogTrace(message) 	(HxLOG_Print(C_CODE_F_PURPLE"[MHEG_MGR](%d)"C_CODE_RESET, __LINE__), (HxLOG_Print message))
#define MhegMgrLogPrint(message)	(HxLOG_Print(C_CODE_F_PURPLE"[MHEG_MGR](%d)"C_CODE_RESET, __LINE__), (HxLOG_Print message))
#define MhegMgrLogFunc(message)	 	(HxLOG_Print(C_CODE_F_PURPLE"[MHEG_MGR](%d)"C_CODE_RESET, __LINE__), (HxLOG_Print message))
#define MhegMgrLogAssert(message) 	(HxLOG_Warning(C_CODE_F_PURPLE"[MHEG_MGR](%d)"C_CODE_RESET, __LINE__), (HxLOG_Warning message))
#define MhegMgrLogError(message) 	(HxLOG_Error(C_CODE_F_PURPLE"[MHEG_MGR](%d)"C_CODE_RESET, __LINE__), (HxLOG_Error message))
#define MhegMgrLogFatal(c) 			((c) ? 0 : (HxLOG_Critical("\n"C_CODE_F_RED"itk %s fatal on [%s:%d])"C_CODE_RESET"\n\n", __FUNCTION__, __LINE__))); //VK_TASK_Sleep(1000); //ITK_Panic();
#define MhegMgrLogAvStream(c,message) 		((c == TRUE) ? MhegMgrLogPrint(message) : 0)
#define MhegMgrLogStreamingAv(c,message) 	((c == TRUE) ? MhegMgrLogPrint(message) : 0)
#define MhegMgrLogScreenSaver(c,message)	((c == TRUE) ? MhegMgrLogPrint(message) : 0)
#define MhegMgrHide(message)	(HxLOG_Print(C_CODE_F_WHITE_B_BLUE"[MHEG_MGR][HIDE](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HxLOG_Print message))
#else
#define MhegMgrLogTrace(message)
#define MhegMgrLogPrint(message)
#define MhegMgrLogFunc(message)
#define MhegMgrLogAssert(message)
#define MhegMgrLogError(message)
#define MhegMgrLogFatal(c)
#define MhegMgrLogAvStream(c,message)
#define MhegMgrLogStreamingAv(c,message)
#define MhegMgrLogScreenSaver(c,message)
#define MhegMgrHide(message)
#endif

/*
	Video 상태에 따라 Subtitle도 연동하여 control하려면 아래 pre-define을 활성화 해야 한다
*/
#define MHEG_MGR_SYNC_VIDEO_SUBTITLE
#define MHEG_MGR_PID_INIT_VALUE	0xFFFF

#define CHECK_ACTION_ID()	\
	if(s_stMhegMng.curActionId != ulActionId) \
	{ \
		return MESSAGE_PASS; \
	}

#define CHECK_ACTION_HANDLE() \
	CHECK_ACTION_ID() \
	if(SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK) \
	{ \
		MhegMgrLogError(("MESSAGE[0x%08X](0x%08x) is expired. Expected 0x%08X\n", message, hAction, s_stMhegMng.curActionId)); \
		return MESSAGE_PASS; \
	}

#define		MHEG_NUM_MAX_SEARCH									(1)

#define ITK_RED_KEY_START_ERR_LIMIT		15

#if defined (MHEG_UI_ORDER_BY_RUNNING_STATUS)
#define MHEG_FRAME_RECT_ID		100
#endif

#define IS_MHEG_REQUEST_TUNE(mode)	(mode == ITK_SERVICE_TUNE_STOP_ITK)
#define IS_MHEG_REQUEST_QUIET_TUNE(mode, flag)	(IS_MHEG_REQUEST_TUNE(mode) && (flag & ITK_TUNE_KEEP_CONTEXT))
#define IS_MHEG_REQUEST_NDT_QUIET_TUNE(mode, flag)	(IS_MHEG_REQUEST_QUIET_TUNE(mode, flag) && (flag & ITK_TUNE_KEEP_DISPLAY))
#define IS_MHEG_REQUEST_NDT_TUNE(mode, flag)	(!IS_MHEG_REQUEST_QUIET_TUNE(mode, flag) && (flag & ITK_TUNE_KEEP_DISPLAY))

// Streaming view type
#define MHEG_STREAMING_VIEW_TYPE	eViewType_MEDIA

/* MHEG AV Speed */
#define MHEG_AV_SPEED_STOP		0
#define MHEG_AV_SPEED_PLAY		1
#define MHEG_AV_SPEED_PAUSE		2
#define MHEG_AV_SPEED_REWIND	3
#define MHEG_AV_SPEED_FAST_FWD	4

#define ITK_VERSION_INFO_MAX	10

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eMHEG_START_FAIL,
	eMHEG_START_SUCCESS,
	eMHEG_START_NOT_REQUIRE		/* Australia Freeview를 위한 UI사양 */
} MHEG_START_ERR_CODE_t;

typedef struct tagMhegMng
{
	HUINT32							curActionId;
	itk_service_tune_event_t		statusTune;
	HUINT32							tuneFlag;	// itk_service_tune_flag_t
}MHEG_Mng_t;

typedef struct tagMHEG_Attrib_t
{
	Handle_t						hAction;				/* Action handle */
	Handle_t						hDefSvc;				/* default Service handle */
	Handle_t						hCurSvc;				/* current service Handle */
	redkey_state_t					status;					/* mheg status defined at ITK */
	redkey_mheg_app_state_t			appStatus;
	itk_service_tune_event_t		statusTune;				/* Tuning action originated from HOST or Redkey */
	HUINT32							tuneFlag;				/* itk_service_tune_flag_t */

	HUINT16							curVideoPid;			/* current AV PIDs which are different with default Pid for Default service */
	HUINT16							curAudioPid;
	HUINT16							curPcrPid;

	HUINT8							curVideoType;
	HUINT8							curAudioType;

	ITK_DIsplay_State_t				displayAVStatus;		/* current image, audioclip of display is visible or not */
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HUINT8							avMode;					/* AV mode status for media (e.g Interaction Channel Streaming) */
	HUINT8							avStatus;				/* AV mode status for media (e.g Interaction Channel Streaming) */
	Handle_t						hPrevSvc;				/* Previous service handle before performing deep link */
#endif
	HBOOL							imageDisplayFlag;
	ITK_ImageHandle_t*				hImage;

	HCHAR							*pMainContentUri;		/* Main Content URL */
	HBOOL							bPlayAndSeek;

#if defined(MHEG_MGR_SYNC_VIDEO_SUBTITLE)
	HUINT16							ucChangedVideoPid;
#endif

	SvcMp_MediaPlayer_t					*player;
	ITK_AvstreamHandle_t				*pstTempStreamHandler;		/* for probe async mode */
	SvcMp_VerifyExternalCallback_t		*cbTlsCertificate;

	HBOOL							bMHEGStopByHost;
}MHEG_Attrib_t;

typedef struct tagMheg_Context
{
	HUINT32				mastActionId;						/* master action Id */
	HBOOL				bSubtitleEnable;						/* subtitle flag by ITK */
	redkey_profile_t 		itkProfile;
	MHEG_Attrib_t		context[NUM_MAX_ACTION];
}MHEG_Context_t;

typedef struct tagMheg_Hide
{
#if defined(CONFIG_MW_MM_PVR)
	HBOOL		bTrickEntering;
	HBOOL		bTrickExiting;
#endif
	HBOOL		bQuiteTuneMode;
	Handle_t	hLastMasterSvc;
	Handle_t	hLastSuppleSvc;
}MHEG_Hide_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static char szItkVersion[ITK_VERSION_INFO_MAX];
static MHEG_Mng_t					s_stMhegMng;
static MHEG_Hide_t					s_stMhegHide;
static MHEG_Context_t				s_MHEGContext;
static HBOOL						s_bMHEGIsReadyAV = FALSE;
static HBOOL						s_bMHEGIsReadyPMT = FALSE;

static PostMsgToMgrCb_t	s_pfncMhegMgrPostMsgCallback;

/* <1> pause() -> <2> newAVStream() -> <2> pause() -> <1> stop() 의 상황에서
    <1> 의 freeze 상태에 의해서 <2> 의 freeze 동작이 정상적으로 이루어지 지 않음.
     freeze 변수를 global이 아닌 stream handler 별로 관리하도록 변경함. */
//static HBOOL						s_bMHEGFreezedV = FALSE, s_bMHEGFreezedA = FALSE;

/**
* PORT_ITK_AVS_NewAvstream가 불리고 PORT_ITK_AVS_PlayAvStream가 불린 다음에
* 이전 av에 대한 PORT_ITK_AVS_StopAvStream이 와서 AV가 안나오는 경우가 있음.
* sync를 맞추기 위해 사용
*/
void  								*g_pvCurAvstream_V = NULL;
void  								*g_pvCurAvstream_A = NULL;

/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
void local_mheg_CallbackRedkeyState(void* context, redkey_state_t state);
void local_mheg_CallbackRedkeyAppState(void* context, redkey_mheg_app_state_t appState);
static HERROR local_mheg_Display_AVS_stopStream(ITK_AvstreamHandle_t* pstStreamHandler, HUINT8 ucIsTrackOption);
static void  local_mheg_initMhegContext(void);
static void local_mheg_initMhegHideContext(void);
static HERROR local_mheg_SetContextCurPesInfo(HUINT32 ulActionId, HUINT16 usVideoPid, HUINT16 usAudioPid, HUINT16 usPcrPid, HUINT8 ucVideoType, HUINT8 ucAudioType);
static HERROR local_mheg_SendKeyToClient(DFBInputDeviceKeySymbol key_symbol, HINT32 key_code);
static HERROR local_mheg_SendKeyToHost(DFBInputDeviceKeySymbol key_symbol, HINT32 key_code);
static HERROR local_mheg_Display_AVS_release(struct ITK_AvstreamHandle_t* pstStreamHandler);
static void local_mheg_Display_AVS_preload(struct ITK_AvstreamHandle_t* pstStreamHandler);
static HERROR local_mheg_Display_AVS_playStream(struct ITK_AvstreamHandle_t* pstStreamHandler, HUINT32 ulStartTime);
static void local_mheg_Display_AVS_deselectAudioComponent(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t component);
static void local_mheg_Display_AVS_deselectVideoComponent(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t component);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static HERROR local_mheg_Display_AVS_processMPEventProbe(Handle_t hAction, HBOOL bProbeSuccess);
static HERROR local_mheg_Display_AVS_processMPEventError(Handle_t hAction);
#endif
static void local_mheg_Display_AVS_setSpeed(Handle_t hAction, struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t numerator, itk_int32_t denominator);
static void local_mheg_Display_AVS_showImage(Handle_t hAction, ITK_ImageHandle_t	*image, HUINT8 plane, HINT32 nByOther);
static void local_mheg_Display_AVS_stopImage(Handle_t hAction, ITK_ImageHandle_t	*image, HUINT8 plane);
static void local_mheg_Display_AVS_releaseImage(Handle_t hAction, ITK_ImageHandle_t	*image, HUINT8 plane);
static void *local_mheg_Display_AVS_getCurHandle_V(void);
static void local_mheg_Display_AVS_setCurHandle_V(void *pvCurAvstream);
static void *local_mheg_Display_AVS_getCurHandle_A(void);
static void local_mheg_Display_AVS_setCurHandle_A(void *pvCurAvstream);
static void	*local_mheg_Display_AVS_getCurHandle(void);
static void local_mheg_Display_controlSubtitle(HUINT32 ulActionId, HBOOL bEnable);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static HERROR local_mheg_Display_AVS_setContextAvMode(HUINT32 ulActionId, HUINT8 avMode);
static void local_mheg_Display_AVS_setTrigger(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t trigger, itk_int32_t trigger_value);
static void local_mheg_Display_AVS_clearTrigger(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t trigger);
static ITK_Media_Triggers_t *local_mheg_Display_AVS_findTrigger(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t triggerValue);
static void local_mheg_Display_AVS_setPosition(Handle_t hAction, struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t position);
static void local_mheg_Display_AVS_setEndPosition(Handle_t hAction, struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t end_position);
static HERROR local_mheg_Display_AVS_createStreamingAV(Handle_t hAction, char *pucStreamPath, HUINT8 avMode);
static HERROR	local_mheg_Display_AVS_destroyStreamingAv(Handle_t hAction);
static HERROR local_mheg_Display_AVS_playStreamingAV(Handle_t hAction, char *pucStreamPath, HUINT32 ulStartTime);
static HERROR local_mheg_Display_AVS_stopStreamingAV(Handle_t hAction);
static HERROR local_mheg_Display_AVS_pauseStreamingAV(Handle_t hAction);
static HERROR local_mheg_Display_AVS_resumeStreamingAV(Handle_t hAction);
static HERROR local_mheg_Display_AVS_setPlayTimeStreamingAV(Handle_t hAction, HUINT32 ulTime);
static void local_mheg_Display_AVS_changePositionStreamingAV(Handle_t hAction);
static void local_mheg_Display_AVS_startStreamingAV(void);
static void local_mheg_Display_AVS_eofPositionStreamingAV(Handle_t hAction);
static void local_mheg_Display_AVS_underflowResumeStreamingAV(void);
static void local_mheg_Display_AVS_underflowStreamingAV(void);
static HUINT8	local_mheg_Display_AVS_getStreamingAvStatus(void);
static void	local_mheg_Display_AVS_setStreamingAvStatus(HUINT8 avEvent);
static HBOOL local_mheg_Display_AVS_isSettingStreamingAV(void);
static HBOOL local_mheg_Display_AVS_isSetupStreamingAV(void);
static HBOOL local_mheg_Display_AVS_isPlayingStreamingAV(void);
static HERROR local_mheg_Display_AVS_controlStreamingSubtitle(Handle_t hAction, HBOOL bOn);
#endif
static void local_mheg_Misc_showPromptForGuidance(const char *restriction);
static void local_mheg_RegisterEngineProfile(MHEG_Engine_Profile_t *pEngineProfile, redkey_profile_t	*pItkProfile, size_t *pulItkMemSize);
static HERROR local_mheg_SetContextCurSvcHandle(HUINT32 ulActionId, Handle_t pCurSvcHandle);
static HERROR local_mheg_SetMastMhegContext(HUINT32 ulActionId);
static HERROR local_mheg_GetContextMastActionId(HUINT32* pMastActionId);
static HERROR local_mheg_GetContextMastActionHandle(Handle_t* phAction);
#if 0		// not used
static HERROR local_mheg_GetContextActionHandle(HUINT32 ulActionId, Handle_t* phAction);
#endif
static HERROR local_mheg_GetContextSvcHandle(HUINT32 ulActionId, Handle_t* phDefSvcHadle, Handle_t* pCurSvcHandle);
static HERROR local_mheg_GetContextCurPesInfo(HUINT32 ulActionId, HUINT16* pusVideoPid, HUINT16* pusAudioPid, HUINT16* pusPcrPid, HUINT8* pucVideoType, HUINT8* pucAudioType);
static void	local_mheg_AUDIOCLIP_EventCallback(Handle_t hAction, itk_media_event_t audioclipEvent);
#if 0
static void local_mheg_ReceiveHamaData(HSIGNAL_e eSignal, HSIGNALTYPE_e eSignalType, HINT32 size, void *data);
#endif
static HERROR local_mheg_MsgMhegStop (Handle_t hAction);
static MHEG_START_ERR_CODE_t		local_mheg_StartMhegAppl(void);
static void	local_mheg_StopMhegAppl(Handle_t hAction, itk_service_tune_event_t eTuneEvent);
static void	local_mheg_RestartMhegAppl(Handle_t hAction);
static HBOOL local_mheg_CheckMHEGIsRunning(void);
static HBOOL local_mheg_CheckMHEGIsAvailable(Handle_t hAction);
#if defined (CONFIG_MW_MM_PVR)
static HBOOL local_mheg_CheckIsTuningConflict(Handle_t hOrgSvcHandle, Handle_t *hNewSvcHandle);
static HBOOL local_mheg_CheckIsRecording(Handle_t hCurSvc);
#endif

#if defined (MHEG_UI_ORDER_BY_RUNNING_STATUS)
static BUS_Result_t local_mheg_draw_background(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1);
#endif
static HERROR local_mheg_NotifyTableReceived(Handle_t hAction,SiTableType_e eSiTableType);
HERROR MHEG_RAW_GetComponentInfo(HUINT16 usTSUniqId, HUINT16 usSvcId, HINT32 nComponent, HUINT8 ucElementType, HINT32 *pnCompPid, HUINT8 *pucCodecType, HUINT8 *pucStreamType, DxStereoSelect_e *pSoundMode);
static void local_mheg_ResetNvsFs(Handle_t hAction, Mheg_Nvs_Fs_Type_t MhegFsType);
static void local_mheg_ResetDvr(Handle_t hAction);
static void local_mheg_GetVersionInfo(Handle_t hAction);
static void local_mheg_ChangeAv(Handle_t hAction, DxVideoCodec_e eVideoCodec, HUINT16 usVideoPid, DxAudioCodec_e eAudioCodec, HUINT16 usAudioPid, HUINT16 usPcrPid);

#if 0
static void local_mheg_Debug_Print_PlayInfo(MW_MEDIA_INFO_t* pstMediaPlayInfo);
#endif

/*******************************************************************/
/********************	Application Proc	*************************/
/*******************************************************************/
HINT32 local_mheg_PostMsgToAp(HINT32 msg, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{

	if (s_pfnNotifyMsgToMgr)
	{
		return (*s_pfnNotifyMsgToMgr)(msg, hAct, p1, p2, p3);
	}

	return 0;
}

STATIC void local_mheg_RegisterPostMsgToApCallBack(PostMsgToMgrCb_t pfncPostMsgCallback)
{

	if(pfncPostMsgCallback == NULL)
	{
		HxLOG_Critical("[%s:%d] Critical Eror!!!\n", __FUNCTION__, __HxLINE__);
	}

	s_pfncMhegMgrPostMsgCallback = pfncPostMsgCallback;
}


HERROR AP_Mheg_Init(HUINT32 nWndId, HUINT32 nStillWndId, PostMsgToMgrCb_t pfncPostMsgCallback)
{
	MHEG_Engine_Profile_t	usEngineProfile;
	redkey_profile_t	eItkProfile;
	size_t			ulItkMemSize;

	local_mheg_RegisterPostMsgToApCallBack(pfncPostMsgCallback);

	if (PORT_ITK_GetEngineVersion(szItkVersion, ITK_VERSION_INFO_MAX) == ERR_OK)
	{
		MhegMgrLogTrace(("Initate Redkey Version(%s)", szItkVersion));
	}
	else
	{
		MhegMgrLogAssert(("Cannot get the version of the engine!\n"));
	}

	/* initialize MHEG Context */
	local_mheg_initMhegContext();

	/* Register engine profile : we must set engine profile first !!!!!! */
	local_mheg_RegisterEngineProfile(&usEngineProfile, &eItkProfile, &ulItkMemSize);

	/* initializes OS ....... */
	PORT_ITK_OS_Init();
	PORT_ITK_OS_CreateObject();

	/* initializes DISPLAY ....... */
	PORT_ITK_Display_CreateObject(eItkProfile);
	PORT_ITK_Display_InitVideoScalingInfo();
	PORT_ITK_Display_Initialize(nWndId, nStillWndId);

	/* initializes SVC MANAGER ....... */
	PORT_ITK_SVC_Init();
	PORT_ITK_SVC_CreateObject(eItkProfile);

	/* initializes MISC ....... */
	PORT_ITK_MISC_CreateObject(eItkProfile); // TODO: subtitle, Audio Description, Pin setting, ValidPIN 등 추가작업필요

	/* initializes FONT Driver ....... */
	PORT_ITK_FONTDRIVER_CreateObject();

	if (usEngineProfile & (MHEG_ENGINE_PROFILE_UK | MHEG_ENGINE_PROFILE_UK_FSAT | MHEG_ENGINE_PROFILE_AUSTRALIA))
	{
		if ((eItkProfile == REDKEY_PROFILE_UK_HD)
			|| (eItkProfile == REDKEY_PROFILE_INTERNAL5)
			|| (eItkProfile == REDKEY_PROFILE_UK_FSAT)
			|| (eItkProfile == REDKEY_PROFILE_FREESATG2)
			|| (eItkProfile == REDKEY_PROFILE_AUSTRALIA) )
		{
			/* initializes Http ....... */
			PORT_ITK_Http_CreateObject();

			/* initializes Crypto ....... */
			PORT_ITK_Crypto_CreateObject(eItkProfile);
		}

		/* initializes DEMUX ....... */
		PORT_ITK_DEMUX_Init();
		XSVC_SI_MHEG_Init();
		PORT_ITK_DEMUX_CreateObject();

		/* initiate Zlib */
		PORT_ITK_ZLIB_CreateObject();

		/* initializes Nvs ....... */
		PORT_ITK_Nvs_CreateObject(eItkProfile);
	}


#if defined(CONFIG_3RD_MHEG_USE_PVR_FUNCTIONALITY)
	PORT_ITK_Dvr_CreateObject();
#endif

	if (usEngineProfile & MHEG_ENGINE_PROFILE_CI_PLUS)
	{
		/* initialize CI AppMMI */
		PORT_ITK_APPMMI_Init();
		PORT_ITK_APPMMI_CreateObject();
	}

	/* initializes ITK Environment */
	if(ITK_init() == ITKE_OK)
	{
		MhegMgrLogTrace(("ITK_init is Ok.\r\n"));
	}
	else
	{
		MhegMgrLogError(("Fail to call ITK_init() !!!\r\n"));
		return ERR_FAIL;
	}

	if(RedKey_init(eItkProfile, ulItkMemSize,
				local_mheg_CallbackRedkeyState, NULL,
				local_mheg_CallbackRedkeyAppState, NULL) != ITKE_OK)
	{
		MhegMgrLogError(("Fail to call RedKey_init() ___ profile(%d) !!! \r\n", eItkProfile));
		return ERR_FAIL;
	}

#if defined(CONFIG_3RD_MHEG_USE_PVR_FUNCTIONALITY)
	/* initiate DSMCC-File System (Only required for PVRs) */
	PORT_ITK_DSMCCFS_Init();
#endif

	PORT_ITK_AVS_initSem();

	MhegMgrLogTrace(("Success initializing ITK Task !!\r\n"));

	return ERR_OK;
}

BUS_Result_t xproc_mheg_Manager(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT32				ulMainActionId = MGR_ACTION_GetMainActionId();
	HERROR				lResult;
	MgrAction_Type_e		actionType = eActionType_NONE;

	switch (message)
	{
		case eMEVT_BUS_CREATE :
#if 0
			HAPI_ConnectSignal(eHSIG_ShowSubtitle);
			HAPI_Register("MhegItkHapii", 40, 0x4000, local_mheg_ReceiveHamaData);
#endif
			MhegMgrLogFunc(("eMEVT_BUS_CREATE\n"));

#if defined (MHEG_UI_ORDER_BY_RUNNING_STATUS)
			GWM_APP_SetAppArea(0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
#endif
			/* initialize and create MHEG ITK modules */
			s_stMhegMng.curActionId = ulActionId;
			s_stMhegMng.statusTune = ITK_SERVICE_TUNE_STOP_HOST;
			s_stMhegMng.tuneFlag = 0;
			break;

		case eMEVT_ACTION_NOTIFY_TAKEN:
				MhegMgrLogFunc(("eMEVT_ACTION_NOTIFY_TAKEN(hAction = 0x%X, p1=%d, p2=%d, p3=%d) \n", hAction, p1, p2, p3));
				MhegMgrLogTrace(("action started - mode (%d)\n", p1));

#if defined(CONFIG_MW_MM_PVR)
			if( (eViewType_TSR_PB == p1) || (eViewType_CHASE_PB == p1) )
			{
				s_stMhegHide.bTrickEntering = TRUE;
			}
			else
			{
				s_stMhegHide.bTrickEntering = FALSE;
			}
			MhegMgrHide(("MSG_ACTION_NOTIFY_TAKEN : Set s_stMhegHide.bTrickEntering = %d\n", s_stMhegHide.bTrickEntering));
#endif
			break;

		case eMEVT_ACTION_NOTIFY_RELEASED:
			{
				HBOOL	bMhegRunning = local_mheg_CheckMHEGIsRunning();
				MhegMgrLogFunc(("eMEVT_ACTION_NOTIFY_RELEASED(hAction = 0x%X, p1=%d, p2=%d, p3=%d),bMhegRunning=%d \n",hAction, p1, p2, p3, bMhegRunning));
				MhegMgrLogTrace(("action stopped - mode (%d), mheg running (%d)\n", p1, bMhegRunning));
#if defined(CONFIG_MW_MM_PVR)
				if( (eViewType_TSR_PB == p1) || (eViewType_CHASE_PB == p1) )
				{
					s_stMhegHide.bTrickEntering = FALSE;
					s_stMhegHide.bTrickExiting = TRUE;
				}
				else
				{
					s_stMhegHide.bTrickExiting = FALSE;
				}
				MhegMgrHide(("eMEVT_ACTION_NOTIFY_RELEASED : Set s_stMhegHide.bTrickEntering = %d, s_stMhegHide.bTrickExiting= %d\n",
					s_stMhegHide.bTrickEntering, s_stMhegHide.bTrickExiting));
#endif
#if 0
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)

				if ((p1 == eViewType_WTV) && (bMhegRunning == TRUE))
				{
					if (local_mheg_Display_AVS_isSettingStreamingAV() == FALSE
						&& local_mheg_Display_AVS_isSetupStreamingAV() == FALSE
						&& local_mheg_Display_AVS_isPlayingStreamingAV() == FALSE)
					{
						local_mheg_MsgMhegStop(hAction);
					}
				}
				else if ((p1 == eViewType_MEDIA) && (bMhegRunning == TRUE))
				{
					if (local_mheg_Display_AVS_isSetupStreamingAV() == TRUE)
					{
						local_mheg_Display_AVS_setStreamingAvStatus(ITK_DISPLAY_AV_STREAMING_STATUS_ALL_OFF);
						local_mheg_MsgMhegStop(hAction);
					}
				}
#else
				if ((p1 == eViewType_WTV) && (bMhegRunning == TRUE))
				{
					local_mheg_MsgMhegStop(hAction);
				}
#endif
#endif
			}
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			{
				itk_service_tune_event_t	usOrgTune;
				DbSvc_Info_t	stSvcInfo = {0, };
				HUINT16		usOldTsUniqId, usNewTsUniqId;
				Handle_t	hPreDefSvc, hSvcHandle = (Handle_t)p1;
				HUINT32		tuneFlag;

				MhegMgrLogFunc(("eMEVT_LIVE_NOTIFY_SVC_CHANGED\n"));
				if(ulActionId != ulMainActionId)
				{
					/* Master action Id가 아니면 반응하지 않는다. */
					MhegMgrLogAssert(("Ignore service change : old action handle !!\n"));
					//return MESSAGE_PASS;
				}

				actionType = MGR_ACTION_GetType(ulMainActionId);
				if (actionType == eViewType_ATV ||
					actionType == eActionType_NONE)
				{
					MhegMgrLogTrace(("Can not start MHEG in ATV & NO SDB mode, (mode:%d)\n", actionType));
					break;
				}

				usOrgTune = s_stMhegMng.statusTune;
				tuneFlag = s_stMhegMng.tuneFlag;
				hPreDefSvc = s_MHEGContext.context[s_MHEGContext.mastActionId].hDefSvc;

				MhegMgrLogTrace(("hPreAction (0x%x) -> hAction(0x%x), hActionId(%d), bSubtitleEnable(%d), tune Status/Flag(%d/%d), hSvcHandle(0x%x)\n",
					s_MHEGContext.context[ulActionId].hAction, hAction, ulActionId, s_MHEGContext.bSubtitleEnable, usOrgTune, tuneFlag, hSvcHandle));

				if (ITK_getManager(ITK_DEMUX))
				{
					if(tuneFlag & ITK_TUNE_KEEP_DISPLAY)
					{
						if (DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo) == ERR_OK)
							XSVC_SI_MHEG_RequestMonitorPMT(hAction,stSvcInfo.usPmtPid,TRUE);
					}
				}

				if (!IS_MHEG_REQUEST_NDT_QUIET_TUNE(usOrgTune, tuneFlag)) // NDT quiet tune 가 아닐 경우에만
				{
					MhegMgrLogTrace(("Reset subtitle control by iTK\n"));
					if (s_MHEGContext.bSubtitleEnable == TRUE)
						local_mheg_Display_controlSubtitle(ulMainActionId, TRUE);

					s_bMHEGIsReadyAV = FALSE;
					s_bMHEGIsReadyPMT = FALSE;
				}

				/* Mheg을 stop 하기전에 호출해야 PORT_ITK_DEMUX_NewSiTable 함수에서 hAction 체크 시 오류가 발생하지 않음 */
				/* MHEG_HIDE_ENABLE flag에 의해서 SVC_CHANGED message가 break 된 경우에도 아래 함수가 호출 되어야 hAction 체크 시 오류가 발생하지 않음 */
				PORT_ITK_SetMhegHandle(ulActionId, hAction, hSvcHandle, hSvcHandle);

				//see WTV_SessionType_t
				MhegMgrHide(("eMEVT_LIVE_NOTIFY_SVC_CHANGED:actType:0x%X, p2:h%X,p3:h%X(%s),hLastMasterSvc=0x%X,hLastSuppleSvc=0x%X\n",
					actionType, p2,	p3,
					(p3 == eLiveViewSession_MHEG_QUIET)? "eLiveViewSession_MHEG_QUIET" : (p3 == eLiveViewSession_NORMAL)? "eLiveViewSession_NORMAL" : "CHECKIT" ,
					 s_stMhegHide.hLastMasterSvc, s_stMhegHide.hLastSuppleSvc));

				if(p3 == eLiveViewSession_MHEG_QUIET)
				{
					s_stMhegHide.bQuiteTuneMode = TRUE;
				}
				else
				{
					s_stMhegHide.hLastMasterSvc = hSvcHandle;
					s_stMhegHide.hLastSuppleSvc = HANDLE_NULL;
					s_stMhegHide.bQuiteTuneMode = FALSE;
				}

				if(s_stMhegHide.hLastMasterSvc == s_stMhegHide.hLastSuppleSvc)
				{
					s_stMhegHide.bQuiteTuneMode = FALSE; // 제자리로 돌아온것임. 다시 엔진을 시작해도 무방함
					MhegMgrHide(("eMEVT_LIVE_NOTIFY_SVC_CHANGED:The target to retune service was same with last wtv svc(master svc)!\n"));
				}

#if defined(CONFIG_MW_MM_PVR)
				if(eViewType_TSR_PB != actionType)
				{
					s_stMhegHide.bTrickEntering = FALSE;
				}
				else
				{
					s_stMhegHide.bTrickEntering = TRUE;
				}

				MhegMgrHide(("eMEVT_LIVE_NOTIFY_SVC_CHANGED:tsr_entering:%d, s_stMhegHide.bTrickExiting:%d, quite_mode: %d, tuneFlag=0x%X\n",
					s_stMhegHide.bTrickEntering,	s_stMhegHide.bTrickExiting,	s_stMhegHide.bQuiteTuneMode,	s_stMhegMng.tuneFlag ));

				if( s_stMhegHide.bTrickEntering == TRUE && s_stMhegHide.bQuiteTuneMode == TRUE)
				{// this cond. should not to be met in real env.
					s_stMhegHide.bTrickEntering = FALSE;
					MhegMgrHide(("eMEVT_LIVE_NOTIFY_SVC_CHANGED: ASSERT: Breaking proc!(entering tsr) \n"));
					break;
				}
				else if( s_stMhegHide.bTrickExiting == TRUE && s_stMhegHide.bQuiteTuneMode == TRUE)
				{
					s_stMhegHide.bTrickExiting = FALSE;
					MhegMgrHide(("eMEVT_LIVE_NOTIFY_SVC_CHANGED: Breaking proc!(exitting from tsr) \n"));
					break;
				}
				else
				{
					s_stMhegHide.bTrickExiting = FALSE;
					s_stMhegHide.bTrickEntering = FALSE;
					MhegMgrHide(("eMEVT_LIVE_NOTIFY_SVC_CHANGED: Stop RedK Engine & Set operation flags with FALSE \n"));
				}
#endif

				/* stop redkey */
				local_mheg_StopMhegAppl(hAction, usOrgTune);

				/* update MHEG context */
				s_MHEGContext.mastActionId						= ulActionId;
				s_MHEGContext.context[ulActionId].hAction		= hAction;
				s_MHEGContext.context[ulActionId].hDefSvc		= hSvcHandle;
				s_MHEGContext.context[ulActionId].hCurSvc		= hSvcHandle;
				s_MHEGContext.context[ulActionId].statusTune	= usOrgTune;
				s_MHEGContext.context[ulActionId].tuneFlag		= tuneFlag;

				if (!(tuneFlag & ITK_TUNE_KEEP_BROADCAST_INTERRUPT))
				{
					PORT_ITK_SetBroadcastInterruptionStatus(ITK_ENABLE);
				}
				PORT_ITK_SetRctStatus(ITK_DISABLE);

				if (ITK_getManager(ITK_DEMUX))
				{
					DB_SVC_GetUniqueIdFromSvcHandle(hPreDefSvc, &usOldTsUniqId);
					DB_SVC_GetUniqueIdFromSvcHandle(hSvcHandle, &usNewTsUniqId);

					MhegMgrLogTrace(("oldTS(0x%x/0x%x) newTS(0x%x/0x%x)\n", hPreDefSvc, usOldTsUniqId, hSvcHandle, usNewTsUniqId));

					if (usOldTsUniqId != usNewTsUniqId)
					{
						MhegMgrLogTrace(("XSVC_SI_MHEG_StartSearch()\n"));

						lResult = XSVC_SI_MHEG_StartSearch(hAction,
													hSvcHandle,
													usNewTsUniqId,
													0);
					}
				}

				if (!IS_MHEG_REQUEST_NDT_QUIET_TUNE(usOrgTune, tuneFlag)) // NDT quiet가 아닐 경우에만 설정함, NDT는 이전 pid 상태를 유지함
				{
					/* set current service PES info */
					if(DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo) == ERR_OK)
					{
						local_mheg_SetContextCurPesInfo(ulActionId, stSvcInfo.usVideoPid, stSvcInfo.usAudioPid, stSvcInfo.usPcrPid,
													(HUINT8)stSvcInfo.eVideoCodec, (HUINT8)stSvcInfo.eAudioCodec);
					}
					else
					{
						MhegMgrLogAssert(("Invalid SVC Handle(0x%x)\n", hSvcHandle));
						local_mheg_SetContextCurPesInfo(ulActionId, PID_NULL, PID_NULL, PID_NULL, eDxVIDEO_CODEC_UNKNOWN, eDxAUDIO_CODEC_UNKNOWN);
					}
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
					local_mheg_Display_AVS_setContextAvMode(ulActionId, MHEG_AV_MODE_BROADCAST);
#endif
				}

				if (IS_MHEG_REQUEST_NDT_QUIET_TUNE(usOrgTune, tuneFlag)) // NDT quiet mode에서는 AV / SI를 stop -> start 하지 않고 service change 하기 때문에 redkey start를 이 메시지를 받았을 때 해야 함.
				{
					if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
					{
						MhegMgrLogAssert(("Retry Redkey\n"));
						VK_TASK_Sleep(2);
						/* wait 2 ticks and retry to start Redkey */
						BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
					}
				}
			}

			/* make tune status defaulted -> Stop by Host */
			s_stMhegMng.curActionId = ulActionId;
			s_stMhegMng.statusTune = ITK_SERVICE_TUNE_STOP_HOST;
			s_stMhegMng.tuneFlag = 0;
			break;

		/* AV가 없는 Data only채널에서도 본 이벤트는 올라온다 */
		case eMEVT_OUTPUT_AV_STARTED : /* AV 시작됨. MHEG show 허용됨. 이 이벤트전에는 MHEG 보여주면 안됨. */
			{
				HBOOL	bMhegRunning = local_mheg_CheckMHEGIsRunning(),
						bMhegAvailable = local_mheg_CheckMHEGIsAvailable(hAction);

				CHECK_ACTION_ID(); 		/* 현재 처리중인 action ID에게만 동작. */
				MhegMgrLogFunc(("eMEVT_OUTPUT_AV_STARTED\n"));
				MhegMgrLogTrace((C_CODE_F_YELLOW"AV started : [bMhegRunning(%d) bMhegAvailable(%d) statusTune/flag (%d/%d) PMT Ready(%d)]"C_CODE_RESET"\n",
								bMhegRunning, bMhegAvailable, s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag, s_bMHEGIsReadyPMT));

				if (MGR_ACTION_GetType(ulMainActionId) == eViewType_WTV)
				{
					if (s_bMHEGIsReadyPMT == TRUE)
					{
						if (IS_MHEG_REQUEST_TUNE(s_MHEGContext.context[ulActionId].statusTune) // redkey가 retune한 경우 redkey stop시에 callback state를 호출하지 않는 경우가 발생하여 callback state를 고려하지 않고 start 하도록 함
							&& !IS_MHEG_REQUEST_NDT_QUIET_TUNE(s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag)) // ndt quiet tune 일 경우 eMEVT_LIVE_NOTIFY_SVC_CHANGED 메시지에서 start를 하기 때문에 여기서는 할 필요없음
						{
							if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
							{
								MhegMgrLogAssert(("Retry Redkey\n"));
								VK_TASK_Sleep(2);
								/* wait 2 ticks and retry to start Redkey */
								BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
							}

							if (IS_MHEG_REQUEST_NDT_TUNE(s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag))
							{
								// i-frame 상태에서 non-destructive normally 일 경우 i-frame 다시 display 하도록 함 (normal tune 일 경우 zapping에 i-frame hide되기 때문, 추후에 watch tv 부분을 수정해야...)
								if (s_MHEGContext.context[ulActionId].hImage != NULL)
								{
									PORT_ITK_Display_StopIFrameByOther(hAction);
									PORT_ITK_Display_ShowIFrameByOther(hAction, s_MHEGContext.context[ulActionId].hImage);
								}
							}
						}
						else if (bMhegAvailable == TRUE)
						{
							if(bMhegRunning == FALSE)
							{
								if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
								{
									MhegMgrLogAssert(("Retry Redkey\n"));
									VK_TASK_Sleep(2);
									/* wait 2 ticks and retry to start Redkey */
									BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
								}
							}
							else
							{
								if (s_bMHEGIsReadyAV == FALSE)
								{
									MhegMgrLogAssert(("Retry Redkey\n"));
									VK_TASK_Sleep(2);
									/* wait 2 ticks and retry to start Redkey */
									BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 0, 1, 0);
								}
							}
						}
					}
				}
				s_bMHEGIsReadyAV = TRUE;
			}
			break;

		case eMEVT_OUTPUT_AV_STOPPED : /* Rating, CAS등의 요인에 의하여 AV 중단됨. MHEG hide되어야 함. */
			MhegMgrLogFunc(("eMEVT_OUTPUT_AV_STOPPED\n"));
			MhegMgrLogTrace((C_CODE_F_RED"Redkey status (%d)"C_CODE_RESET"\n", s_MHEGContext.context[ulActionId].status));
			CHECK_ACTION_ID(); /* 현재 처리중인 action ID에게만 동작. */

			if( (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
				&& !(IS_MHEG_REQUEST_NDT_QUIET_TUNE(s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag)) )
			{
				local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
			}

			s_bMHEGIsReadyAV = FALSE;
			break;

		case eMEVT_SEARCH_ACTION_START:
			MhegMgrLogFunc(("eMEVT_SEARCH_ACTION_START\n"));
			MhegMgrLogTrace(("Search Start !!!\n"));
			s_bMHEGIsReadyAV = FALSE;
			s_bMHEGIsReadyPMT = FALSE;

			/* restart Redkey, it' not service changes, just restart Redkey by Mheg Manager */
			if(s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
			}

			break;

		case eMEVT_ACTION_NOTIFY_MAIN_CHANGED:
			{
				MhegMgrLogFunc(("eMEVT_ACTION_NOTIFY_MAIN_CHANGED\n"));
				MhegMgrLogTrace(("hAction(0x%x), ulMainActionId(%d) ulActionId(%d)\n", hAction, ulMainActionId, ulActionId));
				if (MGR_ACTION_GetType(ulMainActionId) != eViewType_WTV)
				{
					if(local_mheg_CheckMHEGIsRunning() == TRUE)
					{
						local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
					}
				}
				else
				{
					HBOOL	bMhegRunning = local_mheg_CheckMHEGIsRunning(),
							bMhegAvailable = local_mheg_CheckMHEGIsAvailable(hAction);

					MhegMgrLogTrace(("\t"C_CODE_F_YELLOW"[bMhegRunning(%d) bMhegAvailable(%d) statusTune/flag(%d/%d) PMT Ready(%d) AV Ready(%d)]"C_CODE_RESET"\n",
								bMhegRunning, bMhegAvailable, s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag, s_bMHEGIsReadyPMT, s_bMHEGIsReadyAV));

					if ((s_bMHEGIsReadyAV == TRUE) && (s_bMHEGIsReadyPMT == TRUE))
					{
						if (IS_MHEG_REQUEST_TUNE(s_MHEGContext.context[ulActionId].statusTune) // redkey가 retune한 경우 redkey stop시에 callback state를 호출하지 않는 경우가 발생하여 callback state를 고려하지 않고 start 하도록 함
							&& !IS_MHEG_REQUEST_NDT_QUIET_TUNE(s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag)) // ndt quiet tune 일 경우 eMEVT_LIVE_NOTIFY_SVC_CHANGED 메시지에서 start를 하기 때문에 여기서는 할 필요없음
						{
							if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
							{
								MhegMgrLogAssert(("Retry Redkey\n"));
								VK_TASK_Sleep(2);
								/* wait 2 ticks and retry to start Redkey */
								BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
							}
						}
						else if (bMhegAvailable == TRUE)
						{
							if(bMhegRunning == FALSE)
							{
								if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
								{
									MhegMgrLogAssert(("Retry Redkey\n"));
									VK_TASK_Sleep(2);
									/* wait 2 ticks and retry to start Redkey */
									BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
								}
							}
						}
					}

					local_mheg_SetMastMhegContext(ulMainActionId);
				}
			}
			break;

#if 0
		// appmheg에서 key 처리시에 사용하던 Message임 (현재 사용하지 않음)
		// 추후에라도 재 사용시 해당 Message의 입력 Parameter 변경되어야 함
		case eOEVT_MHEG_KEYDOWN:
#endif
		case eMEVT_MHEG_DISPLAY_KEYDOWN:
			{
				MgrAction_Type_e actionType = MGR_ACTION_GetType(ulMainActionId);
				DFBInputDeviceKeySymbol	keySymbol;
				HINT32			keyCode;
#if defined (USE_KEY_PROCESS_BY_HAMA)
				HInputEvent_t		*pEvtWindow = (HInputEvent_t*)p3;
#else
				DFBWindowEvent		*pEvtWindow = (DFBWindowEvent*)p3;
#endif
				if(pEvtWindow == NULL)
				{
					break;
				}

				keySymbol = (DFBInputDeviceKeySymbol)pEvtWindow ->key_symbol;
				keyCode = (HINT32)pEvtWindow ->key_code;

				MhegMgrLogFunc(("eOEVT_MHEG_KEYDOWN or eMEVT_MHEG_DISPLAY_KEYDOWN key_code [%d]\n", keyCode));

				if ((actionType != eViewType_WTV && actionType != eViewType_MOVIE_PB && actionType != eActionType_NONE)
					|| (s_bMHEGIsReadyAV == FALSE))
				{
					MhegMgrLogTrace(("skip key(down) in non-WTV, IP Streaming (NONE) mode, or NOT ready AV (actionType : %d, s_bMHEGIsReadyAV : %d)\n", actionType, s_bMHEGIsReadyAV));
#if defined (USE_KEY_PROCESS_BY_HAMA)
					HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_PASS, pEvtWindow);
#else
					HAPI_ReturnDFBKeyEvent((void*)pEvtWindow, FALSE);
#endif
					break;
				}

				if( IS_MHEG_REQUEST_QUIET_TUNE(s_MHEGContext.context[ulMainActionId].statusTune, s_MHEGContext.context[ulMainActionId].tuneFlag)
						&& (local_mheg_CheckIsRecording(s_MHEGContext.context[ulMainActionId].hDefSvc)) && (actionType == eViewType_WTV)
						&& (s_stMhegHide.hLastSuppleSvc != HANDLE_NULL) && (s_stMhegHide.hLastMasterSvc != s_stMhegHide.hLastSuppleSvc) )
				{
					MhegMgrLogTrace(("skip key(down) in Quite tune and Recording mode (%d)\n", actionType));
#if defined (USE_KEY_PROCESS_BY_HAMA)
					HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_PASS, pEvtWindow);
#else
					HAPI_ReturnDFBKeyEvent((void*)pEvtWindow, FALSE);
#endif
					break;
				}
				else
				{
					MhegMgrLogTrace(("Key sent to Client!! action type : %d\n", actionType));
				}

				if(local_mheg_SendKeyToClient(keySymbol, keyCode) == ERR_OK)
				{
#if defined (USE_KEY_PROCESS_BY_HAMA)
					HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_CONSUMED, pEvtWindow);
#else
					HAPI_ReturnDFBKeyEvent((void*)pEvtWindow, TRUE);
#endif
					return MESSAGE_BREAK;
				}
				else
				{
					if(local_mheg_SendKeyToHost(keySymbol, keyCode) == ERR_OK)
					{
#if defined (USE_KEY_PROCESS_BY_HAMA)
						HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_CONSUMED, pEvtWindow);
#else
						HAPI_ReturnDFBKeyEvent((void*)pEvtWindow, TRUE);
#endif
						return MESSAGE_BREAK;
					}
					else
					{
#if defined (USE_KEY_PROCESS_BY_HAMA)
						HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_PASS, pEvtWindow);
#else
						HAPI_ReturnDFBKeyEvent((void*)pEvtWindow, FALSE);
#endif
					}
				}
			}
			break;

		case eSEVT_DB_CHECK_SVC_DB :
			MhegMgrLogFunc(("eSEVT_DB_CHECK_SVC_DB\n"));
			/*
				2010.03.13. NVS를 위한 고려사항
				1. Factory reset, Auto search등 service가 모두 삭제되었을 경우 NVS모두 삭제
				2. Service가 update되었을 경우, FSA만 삭제. NVM은 engine이 처리하도록
				3. 그 외 DVR은 PVR모델에서 고려
			*/
			if (MGR_SVCLIST_CountSvcInDB(MGR_SVCLIST_GetSvcTypeForListCount()) == 0)
			{
				s_bMHEGIsReadyAV = FALSE;
				s_bMHEGIsReadyPMT = FALSE;

				/* restart Redkey, it' not service changes, just restart Redkey by Mheg Manager */
				if(s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
				{
					Handle_t hMhegAction;

					local_mheg_GetContextMastActionHandle(&hMhegAction);

					local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
					BUS_SendMessage (xproc_mheg_Manager, eMEVT_MHEG_NVM_INITIALIZE_RETRY, hMhegAction, 1, 1, 0);
				}
				else
				{
					/* Reboot되지 않는다면 절대 NVM은 지워서는 안된다(eMHEG_NVS_FACTORY_RESET만 아니면 됨) */
					local_mheg_ResetNvsFs(hAction, eMHEG_NVS_FS_DVR | eMHEG_NVS_FS_NVM | eMHEG_NVS_FS_FSA | eMHEG_NVS_FS_RAM);
					local_mheg_ResetDvr(hAction);
				}
			}
/*
	2010.05.09.
	MHEG이 running중인데 dynamic update등이 일어나므로, service가 존재할때는 NVS에 변화를 주지 않는다
*/
#if 0
			else
			{
				DbCheckType_e dbUpdateType = (DbCheckType_e)p1;
				/*
					FSA이외의 file system은 사용성을 보고 넣도록 해야 한다. 호주향은 FSA만 사용
					Service가 모두 삭제후에는 이곳에 또 들어오므로 reset이 2번 되지만, 이미 FS는 모두 삭제되었으므로 문제는 없다
					Dynamic update시에만 Check DB Type이 ALL로 온다
				*/
				if (dbUpdateType == eCheckDbType_ALL)
				{
					local_mheg_ResetNvsFs(hAction, eMHEG_NVS_FS_FSA);
				}
			}
#endif
			break;

		case eMEVT_BUS_DESTROY :
			MhegMgrLogFunc(("eMEVT_BUS_DESTROY\n"));
			local_mheg_MsgMhegStop(hAction);
#if 0
			HAPI_DisconnectSignal(eHSIG_ShowSubtitle);
			HAPI_UnRegisterNotifier(local_mheg_ReceiveHamaData);
#endif
			break;
#if 0
		case MSG_GWM_QUIT_UI:
			MhegMgrLogFunc(("MSG_GWM_QUIT_UI\n"));
			return MESSAGE_PASS;
#endif
		case eMEVT_BUS_TIMER:
			MhegMgrLogFunc(("eMEVT_BUS_TIMER\n"));
			break;

		case eMEVT_BUS_READY_SHUTDOWN:
			MhegMgrLogFunc(("eMEVT_BUS_READY_SHUTDOWN\n"));
			BUS_PostMessage(xproc_mheg_Manager, eMEVT_MHEG_ENGINE_STOP, hAction, 0, 0, 0);
			return MESSAGE_PASS;

		case eSEVT_SI_PMT :
			{
				HBOOL	bMhegRunning = local_mheg_CheckMHEGIsRunning(),
						bMhegAvailable = local_mheg_CheckMHEGIsAvailable(hAction);
				DbSvc_Info_t	stSvcInfo = {0, };

				MhegMgrLogFunc(("eSEVT_SI_PMT \n"));
				MhegMgrLogTrace((C_CODE_F_YELLOW"bMhegRunning(%d) bMhegAvailable(%d) statusTune/flag(%d/%d) AV Ready(%d)"C_CODE_RESET"\n",
								bMhegRunning, bMhegAvailable, s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag, s_bMHEGIsReadyAV));

				CHECK_ACTION_HANDLE(); /* 현재 처리중인 action ID로 최신 handle인지 검사 ... */

				if (s_MHEGContext.context[ulActionId].hAction != hAction)
				{
					MhegMgrLogAssert(("different Action handle(0x%x : 0x%x )\n", s_MHEGContext.context[ulActionId].hAction, hAction));
					break;
				}

				/* set current service PES info */
				if(DB_SVC_GetServiceInfo(s_MHEGContext.context[ulActionId].hCurSvc, &stSvcInfo) == ERR_OK)
				{
					local_mheg_SetContextCurPesInfo(ulActionId, stSvcInfo.usVideoPid, stSvcInfo.usAudioPid, stSvcInfo.usPcrPid,
												(HUINT8)stSvcInfo.eVideoCodec, (HUINT8)stSvcInfo.eAudioCodec);
				}
				else
				{
					MhegMgrLogAssert(("Invalid SVC Handle(0x%x) \r\n", s_MHEGContext.context[ulActionId].hCurSvc));
					local_mheg_SetContextCurPesInfo(ulActionId, PID_NULL, PID_NULL, PID_NULL, eDxVIDEO_CODEC_UNKNOWN, eDxAUDIO_CODEC_UNKNOWN);
				}

				if (ITK_getManager(ITK_DEMUX))
				{
					/* check mheg status */
					local_mheg_NotifyTableReceived(hAction,eSI_TableType_PAT);
					local_mheg_NotifyTableReceived(hAction,eSI_TableType_PMT);

					MhegMgrLogTrace(("Redkey status (%d), AV Ready (%d)\n", s_MHEGContext.context[ulActionId].status, s_bMHEGIsReadyAV));
					if (MGR_ACTION_GetType(ulMainActionId) == eViewType_WTV)
					{
						if (s_bMHEGIsReadyAV == TRUE)
						{
							if (IS_MHEG_REQUEST_TUNE(s_MHEGContext.context[ulActionId].statusTune) // redkey가 retune한 경우 redkey stop시에 callback state를 호출하지 않는 경우가 발생하여 callback state를 고려하지 않고 start 하도록 함
							&& !IS_MHEG_REQUEST_NDT_QUIET_TUNE(s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag)) // ndt quiet tune 일 경우 eMEVT_LIVE_NOTIFY_SVC_CHANGED 메시지에서 start를 하기 때문에 여기서는 할 필요없음
							{
								if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
								{
									MhegMgrLogAssert(("Retry Redkey\n"));
									VK_TASK_Sleep(2);
									/* wait 2 ticks and retry to start Redkey */
									BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
								}

								if (IS_MHEG_REQUEST_NDT_TUNE(s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag))
								{
									// i-frame 상태에서 non-destructive normally 일 경우 i-frame 다시 display 하도록 함 (normal tune 일 경우 zapping에 i-frame hide되기 때문, 추후에 watch tv 부분을 수정해야...)
									if (s_MHEGContext.context[ulActionId].hImage != NULL)
									{
										PORT_ITK_Display_StopIFrameByOther(hAction);
										PORT_ITK_Display_ShowIFrameByOther(hAction, s_MHEGContext.context[ulActionId].hImage);
									}
								}
							}
							else if (bMhegAvailable == TRUE)
							{
								/*
									mheg running중일때는 PAT, PMT만 저장후 별다른 동작을 안한다.
									변경된 PAT, PMT는 porting layer demux모듈의 timer에 의해 주기적으로 notify해준다.
									만약 현 TP의 모든 PMT의 변경을 추적해야 한다면, MHEG_SI를 다시 start해야 한다.
								*/
								if (bMhegRunning == FALSE)
								{
									if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
									{
										MhegMgrLogAssert(("Retry Redkey\n"));
										VK_TASK_Sleep(2);
										/* wait 2 ticks and retry to start Redkey */
										BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
									}
								}
								else
								{
									if (s_bMHEGIsReadyPMT == FALSE)
									{
										MhegMgrLogAssert(("Retry Redkey\n"));
										VK_TASK_Sleep(2);
										/* wait 2 ticks and retry to start Redkey */
										BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 0, 1, 0);
									}
								}
							}
						}
					}
				}
				s_bMHEGIsReadyPMT = TRUE;
			}
			break;
		case eSEVT_SI_IP_STREAMING_PMT:
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			{
				HINT32		nAudioCnt, nSubtitleCnt, ulValue;
				HUINT16		usAudioIdx;
#if 0
				HUINT16		usPid, usCompositionPageId, usAncillaryPageId;
#endif

				MhegMgrLogFunc(("eSEVT_SI_IP_STREAMING_PMT\n"));

				nAudioCnt = p1;
				nSubtitleCnt = p2;

				MhegMgrLogTrace((C_CODE_F_YELLOW"bSubtitleEnable(%d) nAudioCnt(%d), nSubtitleCnt(%d)"C_CODE_RESET"\n", s_MHEGContext.bSubtitleEnable, nAudioCnt, nSubtitleCnt));
				if (s_MHEGContext.bSubtitleEnable == TRUE)
				{
					if (nSubtitleCnt > 0)
					{
#if 0
						// 메뉴 subtitle 값을 우선으로 동작한다.
						ulValue = MWC_PARAM_GetSubtitleLanguage(0);
						lResult = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0, (HUINT32 *)&ulValue, 0);
						if (ERR_OK != lResult)
						{
							HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_SUBTITLE0) Error!!\n");
						}

						MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_YELLOW"Start Subtitle for IP Streaming -> Lang Id(%d) "C_CODE_RESET"\n", ulValue));

						lResult = XSVC_SI_MHEG_GetSubtitleInfo(hAction, ulValue, &usPid, &usCompositionPageId, &usAncillaryPageId);
						if (lResult != ERR_OK)
						{
							MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("MHEG_Streaming_SI_GetSubtitleInfo fail (0x%x)\n", lResult));
						}

						lResult = SVC_SBTL_StartService(hAction,	usPid, usCompositionPageId, usAncillaryPageId);
						MhegMgrLogFatal(lResult == ERR_OK);
#endif
					}
				}

				if (nAudioCnt > 1)
				{
					SvcMp_AudioTrack_t stAudioTrack;

					// 메뉴 Audio 값을 우선으로 동작한다.
//					ulValue = MWC_PARAM_GetAudioLanguage(0);
					lResult = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0, (HUINT32 *)&ulValue, 0);
					if (ERR_OK != lResult)
					{
						HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_AUDIO0) Error!!\n");
					}

					MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_YELLOW"Start Audio for IP Streaming -> Lang Id(%d) "C_CODE_RESET"\n", ulValue));

					lResult = SVC_MP_GetActiveAudioTrack(s_MHEGContext.context[ulActionId].player, (MgrMplayer_AudioTrack_t*)&stAudioTrack);
					if ( lResult != ERR_OK )
					{
						HxLOG_Error("There is no active audio track in current playback.\n");
						break;
					}

					lResult = MHEG_Streaming_SI_GetAudioInfo(hAction, (HxLANG_Id_e)ulValue, stAudioTrack.ulAudioStreamId, &usAudioIdx);
					if (lResult != ERR_OK)
					{
						HxLOG_Error("MHEG_Streaming_SI_GetAudioInfo fail (0x%x) \n", lResult);
						break;
					}

					lResult = SVC_MP_GetAudioTrack(s_MHEGContext.context[ulActionId].player, usAudioIdx, &stAudioTrack);
					if (lResult != ERR_OK)
					{
						HxLOG_Error("Getting the audio track is failed (0x%x) \n", lResult);
						break;
					}

					lResult = SVC_MP_SetActiveAudioTrack(s_MHEGContext.context[ulActionId].player, &stAudioTrack);
					if (lResult != ERR_OK)
					{
						HxLOG_Error("Setting the audio track is failed (0x%x) \n", lResult);
						break;
					}
					MhegMgrLogFatal(lResult == ERR_OK);
				}
			}
#endif
			break;

		case eSEVT_SI_PMT_PIDREMOVED:
			MhegMgrLogFunc(("eSEVT_SI_PMT_PIDREMOVED\n"));
			CHECK_ACTION_HANDLE();
			break;

		case eSEVT_SI_PMT_PIDCHANGED:
			MhegMgrLogFunc(("eSEVT_SI_PMT_PIDCHANGED\n"));
			CHECK_ACTION_HANDLE();
			/* check mheg status */

			if (ITK_getManager(ITK_DEMUX))
			{
				local_mheg_NotifyTableReceived(hAction,eSI_TableType_PAT);

				/* restart Redkey, it' not service changes, just restart Redkey by Mheg Manager */
				if (SVC_PIPE_IsActionHandleLatest(hAction) == ERR_OK)
				{
					if( (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
						&& !(IS_MHEG_REQUEST_NDT_QUIET_TUNE(s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag)) )
					{
						local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
					}
				}
				else
				{
					MhegMgrLogAssert(("ignore eSEVT_SI_PMT_PIDCHANGED command : different ActionHandle (0x%x)\n", hAction));
				}
			}

			/* and then, Mheg manager waits PMT Received event from SI */
			break;

		/*
			>> FREESAT specific <<
			FREESAT에서만 아래 event가 수신되며, restriction descriptor가 존재한다
			다만, CONFIG에 dependent하지 않으므로 별도 ifdef를 사용하지 않는다
		*/
		case eSEVT_SI_T_BAT:
			{
				HUINT16		usTsUniqId;
				DbSvc_Info_t	stSvcInfo = {0, };
				HERROR		nErr;
				HBOOL		bRestrict = FALSE;
				redkey_profile_t eItkProfile = s_MHEGContext.itkProfile;

				MhegMgrLogFunc(("eSEVT_SI_T_BAT\n"));
				if (ITK_getManager(ITK_DEMUX))
				{
					if ( (eItkProfile == REDKEY_PROFILE_UK_FSAT)
						|| (eItkProfile == REDKEY_PROFILE_FREESATG2) )
					{
						/* restart Redkey, it' not service changes, just restart Redkey by Mheg Manager */
						if(SVC_PIPE_IsActionHandleLatest(hAction) == ERR_OK)
						{
							HBOOL	bMhegRunning = local_mheg_CheckMHEGIsRunning(),
								bMhegAvailable = local_mheg_CheckMHEGIsAvailable(hAction),
								bIsQuietTune = XSVC_SI_MHEG_IsQuietTune();

							MhegMgrLogTrace((C_CODE_F_YELLOW"bMhegRunning(%d) bMhegAvailable(%d) bIsQuietTune(%d) AV Ready(%d)"C_CODE_RESET"\n", bMhegRunning, bMhegAvailable, bIsQuietTune, s_bMHEGIsReadyAV));

							DB_SVC_GetUniqueIdFromSvcHandle(s_MHEGContext.context[s_MHEGContext.mastActionId].hCurSvc, &usTsUniqId);
							DB_SVC_GetServiceInfo(s_MHEGContext.context[ulActionId].hCurSvc, &stSvcInfo);
							nErr = XSVC_SI_MHEG_GetServiceInteractiveRestrictInfo(hAction, usTsUniqId, stSvcInfo.usOnId, stSvcInfo.usTsId, stSvcInfo.usSvcId, &bRestrict);
							if ((nErr == ERR_OK) && (bRestrict == TRUE))
							{
								/*
									* restrict인데 Running중인 경우		=> mheg stop
									* restrict인데 Not Running중인 경우  	=> no change
									* no restrict인데 Running중인 경우		=> no change
									* no restrict인데 Not Running중인 경우	=> mheg available 체크후 running결정

									mheg retry중인데 BAT가 변경(restrict)된 경우 timing issue존재 가능
								*/
								/* MHEG stop */
								if (bMhegRunning == TRUE)
								{
									local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
								}
							}
							/*
								non-restrict 서비스의 경우 error return한다
								restrict -> non-restrict로 변경된 경우에 대한 체크
							*/
							else
							{
								/* MHEG start */
								if(MGR_ACTION_GetType(ulMainActionId) == eViewType_WTV)
								{
									if (bMhegRunning == FALSE)
									{
										if ((s_bMHEGIsReadyAV == TRUE) && (s_bMHEGIsReadyPMT == TRUE))
										{
											if (bMhegAvailable == TRUE || bIsQuietTune == TRUE)
											{
												if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
												{
													MhegMgrLogAssert(("Retry Redkey\n"));
													VK_TASK_Sleep(2);
													/* wait 2 ticks and retry to start Redkey */
													BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			break;

		case eSEVT_CH_NO_SIGNAL:
			MhegMgrLogFunc(("eSEVT_CH_NO_SIGNAL\n"));
#if 0
			/* stop redkey */
			if(s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
			}
#endif

#if defined (MHEG_UI_ORDER_BY_RUNNING_STATUS)
			GWM_Obj_Destroy(MHEG_FRAME_RECT_ID);
			GWM_APP_SetAttribute(GWM_Basic_App);
#endif
			break;

		case eMEVT_MHEG_RESTART_MHEG_APPLICATION:
			{
				Handle_t 			hLastSvcHandle = HANDLE_NULL, hNewSvcHandle = HANDLE_NULL;
				HBOOL			bStartFromDeepLink = FALSE;
				HBOOL			bConflict = FALSE;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				if(s_MHEGContext.context[ulActionId].hPrevSvc != HANDLE_NULL)
				{
					bStartFromDeepLink = TRUE;
					hLastSvcHandle = s_MHEGContext.context[ulActionId].hPrevSvc;
					s_MHEGContext.context[ulActionId].hPrevSvc = HANDLE_NULL;
				}
				else
#endif
				{
					MGR_LIVE_GetLastSvcHandle (eLastSvcItem_TV, &hLastSvcHandle);
				}

#if defined (CONFIG_MW_MM_PVR)
				bConflict = local_mheg_CheckIsTuningConflict(hLastSvcHandle, &hNewSvcHandle);
				if(bConflict == TRUE)
				{
					hLastSvcHandle = hNewSvcHandle;
				}
#endif

				/* Deepl link로 부터 시작하고 eMEVT_MHEG_RESTART_MHEG_APPLICATION 메세지가 올라온 경우는 EXIT가 눌린 경우임 */
				/* 이 경우 Web Application으로 EVENT를 날려줌 (appstate = 0, userData = 2) */
				if(bStartFromDeepLink == TRUE)
				{
					BUS_PostMessageToOM(NULL, eMEVT_MHEG_TYPE_MHEG_APPSTATE, HANDLE_NULL, 0, 2, 0);
				}

				if( IS_MHEG_REQUEST_QUIET_TUNE(s_MHEGContext.context[ulActionId].statusTune, s_MHEGContext.context[ulActionId].tuneFlag)
					|| (MGR_ACTION_GetType(ulActionId) != eViewType_WTV))
				{
					if(s_stMhegHide.hLastMasterSvc == s_stMhegHide.hLastSuppleSvc)
					{
						// 이 경우는 Quite tune 된 service에서 원래의 service로 돌아오는 경우임 -> MHEG Restart only!!
						local_mheg_RestartMhegAppl(hAction);
					}
					else
					{
						MGR_LIVE_CmdSendServiceStartByActionId(SVC_PIPE_GetActionId(hAction), hLastSvcHandle); // < 확인 필요 >  eNO_IPLATE|eFORCE_TUNE 처리는 어떻게?
					}
				}
				else
				{
					local_mheg_RestartMhegAppl(hAction);
				}
			}
			break;

		/* MHEG ITK 에서 Service 전환을 요구하는 Event -> 다시 이것을 Watch TV에 Toss 해야 함 */
		case eMEVT_MHEG_SVC_RETUNE:
			{
				Handle_t		hSvc				= (Handle_t)p1;
				HBOOL 			bQuiet 				= (p2 & ITK_TUNE_KEEP_CONTEXT) ? TRUE : FALSE;
				HBOOL 			bNdt 				= (p2 & ITK_TUNE_KEEP_DISPLAY) ? TRUE : FALSE;
				HBOOL 			bNoBanner			= (p2 & ITK_TUNE_NO_BANNER) ? TRUE : FALSE;
				HBOOL 			bBroadcastInterrupt	= (p2 & ITK_TUNE_KEEP_BROADCAST_INTERRUPT) ? TRUE : FALSE;
				HBOOL 			bSameTs				= (HBOOL)p3;
				MgrLive_TuningMode_t	eTuneMode;


				MhegMgrLogFunc(("eMEVT_MHEG_SVC_RETUNE\n"));
				MhegMgrLogTrace(("Request Service change, KEEP_CONTEXT(Quiet):(%d), KEEP_DISPLAY(NDT):(%d), NO_BANNER:(%d), BROADCAST_INTERRUPT:(%d), Same Ts (%d)\n", bQuiet, bNdt, bNoBanner, bBroadcastInterrupt, bSameTs));

				if((bNdt == TRUE) && (bQuiet == TRUE))
				{
					// NDT, Quite Tune시 다른 TP로 tunning 될 경우 처리
					bNdt = (bSameTs == TRUE) ? TRUE : FALSE;
					MhegMgrLogTrace(("NDT Flag is changed from TRUE to FALSE because of tuinning to Another TP!!"));
				}

				eTuneMode = (bNoBanner == TRUE) ? eNO_IPLATE|eFORCE_TUNE : eFORCE_TUNE;
				eTuneMode = (bNdt == TRUE) ? eNDT_TUNE|eTuneMode : eTuneMode;

				s_stMhegMng.statusTune = ITK_SERVICE_TUNE_STOP_ITK;
				s_stMhegMng.tuneFlag = p2;

				// backup last supple service handle
				s_stMhegHide.hLastSuppleSvc = hSvc;

				if(bQuiet == TRUE)
				{
					BUS_SendMessage(NULL, eMEVT_LIVE_MHEG_QUIET_TUNE, hAction, hSvc, 0, eTuneMode);
				}
				else
				{
					MGR_LIVE_CmdSendServiceStartByActionId(SVC_PIPE_GetActionId(hAction), hSvc); // < 확인 필요 > eTuneMode 어떻게?
				}
			}
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_SUBTITLE_CONTROL:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_SUBTITLE_CONTROL\n"));

			/* 아래 flag는 MHEG engine 또는 Appmmi의 CI+ SSM  control할때만 세팅한다 */
			s_MHEGContext.bSubtitleEnable = (HBOOL)p1;

			local_mheg_Display_controlSubtitle(ulMainActionId, s_MHEGContext.bSubtitleEnable);

			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_SUBTITLE_CHANGED:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_SUBTITLE_CHANGED\n"));

			if (local_mheg_CheckMHEGIsRunning() == TRUE)
			{
				PORT_ITK_Display_NotifySubtitleSettingChangedEvent();
			}
			return MESSAGE_BREAK;


		case eMEVT_MHEG_START_REDKEY_RETRY:
			{
				HBOOL		bCheckRedkey = FALSE,
							bNoRetry = FALSE;
				HUINT32 	ulRetryCnt = p2;

				MhegMgrLogFunc(("eMEVT_MHEG_START_REDKEY_RETRY\n"));

				if(SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK)
				{
					MhegMgrLogAssert(("Old Action handle(0x%x), so force returned \r\n", hAction));
					return MESSAGE_BREAK;
				}

				if(p1)
					VK_TASK_Sleep(10);			/* in case of Error */
				else
					VK_TASK_Sleep(5);			/* in case of waiting change of redkey status */

				if (MGR_ACTION_GetType(ulMainActionId) == eViewType_WTV)
				{
					if((local_mheg_CheckMHEGIsRunning() == FALSE) && (s_bMHEGIsReadyAV == TRUE) && (s_bMHEGIsReadyPMT == TRUE))
					{
						bNoRetry = (local_mheg_StartMhegAppl() == eMHEG_START_FAIL) ? FALSE : TRUE;
						bCheckRedkey = TRUE;
					}

					/* Retry중에 running state로 바뀌면 retry를 중지한다 */
					if(bNoRetry == FALSE)
					{
						MhegMgrLogAssert(("Redkey retry count (%d)\n", ulRetryCnt));
						if(ulRetryCnt < ITK_RED_KEY_START_ERR_LIMIT)
						{
							BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, (HINT32)(bCheckRedkey == TRUE) ? 1 : 0, ulRetryCnt + 1, 0);
						}
					}
				}
			}
			return MESSAGE_BREAK;

		case eMEVT_MHEG_HTTP_REGISTER_CALLBACK:
			MhegMgrLogFunc(("eMEVT_MHEG_HTTP_REGISTER_CALLBACK\n"));
			s_MHEGContext.context[ulActionId].cbTlsCertificate = (MgrMplayer_VerifyExternalCallback_t*)p1;
			break;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_PRELOAD:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_PRELOAD\n"));
			MhegMgrLogTrace(("Avstream Preload : Redkey status(%d), Stream handler(0x%x)\n", s_MHEGContext.context[ulActionId].status, p3));
			CHECK_ACTION_HANDLE();
			local_mheg_Display_AVS_preload((ITK_AvstreamHandle_t*)p3);
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_PLAY:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_PLAY\n"));
			MhegMgrLogTrace(("Avstream play : Redkey status(%d), Stream handler(0x%x)\n", s_MHEGContext.context[ulActionId].status, p3));
			CHECK_ACTION_HANDLE();
//			if (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				lResult = local_mheg_Display_AVS_playStream((ITK_AvstreamHandle_t*)p3, 0);
				MhegMgrLogTrace(("(play) eMEVT_MHEG_DISPLAY_AVSTREAM_PLAY - lResult = %d\n", lResult));
			}
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_STOP:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_STOP\n"));
			MhegMgrLogTrace(("Avstream stop : Redkey status(%d), Stream handler(0x%x)\n", s_MHEGContext.context[ulActionId].status, p3));
//			if (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				lResult = local_mheg_Display_AVS_stopStream((ITK_AvstreamHandle_t*)p3, 0);
				MhegMgrLogTrace(("AV stream stop lResult = %d\n", lResult));
			}
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_SELECT_AUDIO:
#if 0//
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_SELECT_AUDIO\n"));
			MhegMgrLogTrace(("Select audio : Redkey status(%d), Stream handler(0x%x)\n", s_MHEGContext.context[ulActionId].status, p3));
			CHECK_ACTION_HANDLE();
//			if (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				local_mheg_Display_AVS_selectAudioComponent((ITK_AvstreamHandle_t*)p3, p2);
			}
#endif
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_SELECT_VIDEO:
#if 0
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_SELECT_VIDEO\n"));
			MhegMgrLogTrace(("Select video : Redkey status(%d), Stream handler(0x%x)\n", s_MHEGContext.context[ulActionId].status, p3));
			CHECK_ACTION_HANDLE();
//			if (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				local_mheg_Display_AVS_selectVideoComponent((ITK_AvstreamHandle_t*)p3, p2);
			}
#endif
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_DESELECT_AUDIO:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_DESELECT_AUDIO\n"));
			MhegMgrLogTrace(("Deselect audio : Redkey status(%d), Stream handler(0x%x)\n", s_MHEGContext.context[ulActionId].status, p3));
//			if (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				local_mheg_Display_AVS_deselectAudioComponent((ITK_AvstreamHandle_t*)p3, p2);
			}
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_DESELECT_VIDEO:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_DESELECT_VIDEO\n"));
			MhegMgrLogTrace(("Deselect video : Redkey status(%d), Stream handler(0x%x)\n", s_MHEGContext.context[ulActionId].status, p3));
//			if (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				local_mheg_Display_AVS_deselectVideoComponent((ITK_AvstreamHandle_t*)p3, p2);
			}
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_RELEASE:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_RELEASE\n"));
			MhegMgrLogTrace(("Avstream release : Redkey status(%d), Stream handler (0x%x)\n", s_MHEGContext.context[ulActionId].status, p1));
//			if (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				lResult = local_mheg_Display_AVS_release((ITK_AvstreamHandle_t*) p3);
				MhegMgrLogTrace(("(release) eMEVT_MHEG_DISPLAY_AVSTREAM_RELEASE - lResult = %d\n", lResult));
			}
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_SET_SPEED:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_SET_SPEED\n"));
			MhegMgrLogTrace(("Set Speed: Redkey status(%d), Stream handler (0x%x)\n", s_MHEGContext.context[ulActionId].status, p3));
//			if (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				local_mheg_Display_AVS_setSpeed(hAction, (ITK_AvstreamHandle_t*) p3, (itk_int32_t)p1, (itk_int32_t)p2);
			}
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DISPLAY_IMAGE_PLAY:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_IMAGE_PLAY\n"));
			MhegMgrLogTrace(("Image play : plane (%d), By Other (%d)\n", p2, p3));
			local_mheg_Display_AVS_showImage(hAction, (ITK_ImageHandle_t*)p3, (HUINT8)p1, p2);
			break;

		case eMEVT_MHEG_DISPLAY_IMAGE_STOP:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_IMAGE_STOP\n"));
			MhegMgrLogTrace(("plane (%d)\n", p2));
			local_mheg_Display_AVS_stopImage(hAction, (ITK_ImageHandle_t*)p1, (HUINT8)p2);
			break;

		case eMEVT_MHEG_DISPLAY_IMAGE_RELEASE:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_IMAGE_RELEASE\n"));
			MhegMgrLogTrace(("Image release : plane(%d)\n", p2));
			local_mheg_Display_AVS_releaseImage(hAction, (ITK_ImageHandle_t*)p1, (HUINT8)p2);
			break;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_SET_TRIGGER:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_SET_TRIGGER\n"));
			MhegMgrLogTrace(("avstream(%p), set trigger, trigger_value (%d, %d)\n", (ITK_AvstreamHandle_t *)p3, p1, p2));
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			local_mheg_Display_AVS_setTrigger((ITK_AvstreamHandle_t *)p3, (itk_int32_t)p1, (itk_int32_t)p2);
#endif
			break;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_CLEAR_TRIGGER:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_CLEAR_TRIGGER\n"));
			MhegMgrLogTrace(("avstream(%p), clear trigger(%d)\n", (ITK_AvstreamHandle_t *)p3, p2));
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			local_mheg_Display_AVS_clearTrigger((ITK_AvstreamHandle_t *)p3, (itk_int32_t)p2);
#endif
			break;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_SET_POSITION:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_SET_POSITION\n"));
			MhegMgrLogTrace(("avstream(%p) position(%d)\n", (ITK_AvstreamHandle_t *)p3, p2));
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			local_mheg_Display_AVS_setPosition(hAction, (ITK_AvstreamHandle_t *)p3, (itk_int32_t)p2);
#endif
			break;

		case eMEVT_MHEG_DISPLAY_AVSTREAM_SET_END_POSITION:
			MhegMgrLogFunc(("eMEVT_MHEG_DISPLAY_AVSTREAM_SET_END_POSITION\n"));
			MhegMgrLogTrace(("avstream(%p)- end position (%d)\n", (ITK_AvstreamHandle_t *)p3, p2));
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			local_mheg_Display_AVS_setEndPosition(hAction, (ITK_AvstreamHandle_t *)p3, (itk_int32_t)p2);
#endif
			break;

		case eMEVT_MHEG_MISC_PROMPT_FOR_GUIDANCE:
			MhegMgrLogFunc(("eMEVT_MHEG_MISC_PROMPT_FOR_GUIDANCE\n"));
			MhegMgrLogTrace(("restriction (%s)\n", (const char*)p3));
			local_mheg_Misc_showPromptForGuidance((const char*)p3);
			break;

		case eMEVT_MHEG_MISC_LAUNCH_APPLICATION:
			MhegMgrLogFunc(("eMEVT_MHEG_MISC_LAUNCH_APPLICATION\n"));
			MhegMgrLogTrace(("application launch (%s)\n", (const char*)p3));
			BUS_PostDataToOM(NULL, eMEVT_LCNUPDATE_START_OK, HANDLE_NULL, 0, 0, p3, strlen(p3));
			break;

		case eMEVT_MHEG_MISC_FLIP_WINDOW:
			MhegMgrLogFunc(("eMEVT_MHEG_MISC_FLIP_WINDOW\n"));
			BUS_PostDataToOM(NULL, eMEVT_MHEG_TYPE_FLIP_WINDOW, HANDLE_NULL, p1, 0, (void*) p3, sizeof(DxRect_t));
			break;

		case eSEVT_AV_AUDIO_CLIP_STARTED:
			MhegMgrLogFunc(("eSEVT_AV_AUDIO_CLIP_STARTED\n"));
			local_mheg_AUDIOCLIP_EventCallback(hAction, ITK_MEDIA_PLAYING);
			return MESSAGE_BREAK;

		case eSEVT_AV_AUDIO_CLIP_COMPLETED:
			MhegMgrLogFunc(("eSEVT_AV_AUDIO_CLIP_COMPLETED\n"));
			local_mheg_AUDIOCLIP_EventCallback(hAction, ITK_MEDIA_STOPPED);
			return MESSAGE_BREAK;

		case eMEVT_MHEG_SITABLE_NOTIFY:
			//MhegMgrLogFunc(("eMEVT_MHEG_SITABLE_NOTIFY\n"));
			if (MGR_ACTION_GetType(ulMainActionId) == eViewType_WTV)
			{
				CHECK_ACTION_HANDLE();
				PORT_ITK_DEMUX_ProcessFilter(ITK_DEMUX_SITABLE_NOTIFY, p1, p2);
			}
			return MESSAGE_BREAK;

		case eSEVT_AV_CHECK_VIDEO_STATUS:
			MhegMgrLogFunc(("eSEVT_AV_CHECK_VIDEO_STATUS\n"));
			if (local_mheg_CheckMHEGIsRunning() == TRUE)
			{
				PORT_ITK_Display_UpdateVideoStatus();
			}
			return MESSAGE_BREAK;

		case eSEVT_OUT_DISPLAYFORMAT_CHANGED_BY_MENU:
			MhegMgrLogFunc(("EVT_TV_ASPECT_RATIO_CHANGED\n"));
			if (local_mheg_CheckMHEGIsRunning() == TRUE)
			{
				PORT_ITK_Display_NotifyAspectRatioChangedEvent();
			}
			return MESSAGE_BREAK;

#if 1		// have to check (yjlim)
		case eSEVT_DB_PARAM_UPDATED:
			MhegMgrLogFunc(("eSEVT_DB_PARAM_UPDATED -> eDB_PARAM_ITEM_SUBTITLE_SETTING!!\n"));
			if (local_mheg_CheckMHEGIsRunning() == TRUE)
			{
				if(p1 == eDB_PARAM_ITEM_SUBTITLE_SETTING)
				{
					PORT_ITK_Display_NotifySubtitleSettingChangedEvent();
				}
			}
			break;
#endif

#if defined (MHEG_UI_ORDER_BY_RUNNING_STATUS)
		case eMEVT_MHEG_STATUS_NOTIFY:
			{
				OSD_Rect_t	rc;
				MhegMgrLogFunc(("eMEVT_MHEG_STATUS_NOTIFY\n"));
				if (p1 == TRUE) // key list 가 존재하므로 MHEG UI가 존재한다고 판단
				{
					OSD_GFX_SetRect(&rc, 0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
					GWM_Frame_Create(MHEG_FRAME_RECT_ID, &rc, NULL);
					GWM_APP_SetObjectDrawMethod(MHEG_FRAME_RECT_ID, local_mheg_draw_background);
				}
				else
				{
					GWM_Obj_Destroy(MHEG_FRAME_RECT_ID);
					GWM_APP_SetAttribute(GWM_Basic_App);
					/*
						engine으로 부터 stop callback을 늦게 받아서 engine start -> retry횟수를 모두 시도해도 engine start를 못하는 경우가 발생한다.
						그래서 engine으로부터 stopped callback을 받으면, 다시한번 모든 상태 체크후 engine start를 시도한다.
						다시 시도하는건 여기서 StartMhegAppl()을 호출하지 말고, retry message를 날려서 시도한다(count 1부터 다시 시작)
					*/
					if (s_bMHEGIsReadyPMT == TRUE && s_bMHEGIsReadyAV == TRUE && local_mheg_CheckMHEGIsAvailable(hAction))
					{
						MhegMgrLogAssert(("Retry Redkey\n"));
						VK_TASK_Sleep(2);
						/* wait 2 ticks and retry to start Redkey */
						BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
					}
				}
			}
			return MESSAGE_BREAK;
#endif
		case eMEVT_NETWORK_INTERFACE_CONNECT:
			MhegMgrLogFunc(("eMEVT_NETWORK_INTERFACE_CONNECT\n"));
//			if (local_mheg_CheckMHEGIsRunning() == TRUE)
			{
				PORT_ITK_Http_Notify(TRUE);
			}
			break;

		case eMEVT_NETWORK_INTERFACE_DISCONNECT:
			MhegMgrLogFunc(("eMEVT_NETWORK_INTERFACE_DISCONNECT\n"));
//			if (local_mheg_CheckMHEGIsRunning() == TRUE)
			{
				PORT_ITK_Http_Notify(FALSE);
			}
			break;

		case eMEVT_MHEG_NVM_INITIALIZE:
			if (ITK_getManager(ITK_NVS))
			{
				Handle_t hMhegAction;
				MhegMgrLogFunc(("eMEVT_MHEG_NVM_INITIALIZE\n"));

				local_mheg_GetContextMastActionHandle(&hMhegAction);

				MhegMgrLogTrace((" Do initialize NVS!\n"));

				/* stop redkey */
				if(s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
				{
					local_mheg_StopMhegAppl(hMhegAction, ITK_SERVICE_TUNE_STOP_HOST);
					BUS_PostMessage (NULL, eMEVT_MHEG_NVM_INITIALIZE_RETRY, hMhegAction, 1, 1, 0);
				}
				else
				{
					local_mheg_ResetNvsFs(hMhegAction, eMHEG_NVS_FS_DVR | eMHEG_NVS_FS_NVM | eMHEG_NVS_FS_FSA | eMHEG_NVS_FS_RAM);
					local_mheg_ResetDvr(hMhegAction);

					BUS_PostMessage (NULL, eMEVT_MHEG_NVM_INITIALIZE_SUCCESS, hAction, 0, 0, 0);
				}
			}
			break;

		case eMEVT_MHEG_NVM_INITIALIZE_RETRY:
			if (ITK_getManager(ITK_NVS))
			{
				HUINT32 		ulRetryCnt = p2;
				MhegMgrLogFunc(("eMEVT_MHEG_NVM_INITIALIZE_RETRY\n"));

				if (s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
				{
					if (ulRetryCnt > 30)
					{
						MhegMgrLogAssert((" Failed to initialize NVM\n"));
					}
					else
					{
						MhegMgrLogAssert((" Retry Redkey to stop(%d)\n", ulRetryCnt));

						VK_TASK_Sleep(20);
						BUS_PostMessage (NULL, eMEVT_MHEG_NVM_INITIALIZE_RETRY, hAction, 1, ulRetryCnt + 1, 0);
					}
				}
				else
				{
					/* Stop이 되었으므로 NVM reset을 한후 다시 MHEG service를 구동 시킨다 */
					local_mheg_ResetNvsFs(hAction, eMHEG_NVS_FS_DVR | eMHEG_NVS_FS_NVM | eMHEG_NVS_FS_FSA | eMHEG_NVS_FS_RAM);
					local_mheg_ResetDvr(hAction);

					BUS_PostMessage (NULL, eMEVT_MHEG_NVM_INITIALIZE_SUCCESS, hAction, 0, 0, 0);

					if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
					{
						MhegMgrLogTrace(("Start Redkey\n"));
						VK_TASK_Sleep(2);
						/* wait 2 ticks and retry to start Redkey */
						BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
					}
				}
			}
			break;

		case eMEVT_MHEG_REQUEST_VERSION:
			local_mheg_GetVersionInfo(hAction);
			break;

		case eMEVT_MHEG_ENGINE_START:
			MhegMgrLogFunc(("MSG_APP_MHEG_START_EVENT or eMEVT_MHEG_ENGINE_START\n"));
			/* Australia Freeview사양 */
			MhegMgrLogTrace(("ENGINE START by MSG!!\n"));

			if (MGR_ACTION_GetType(ulMainActionId) == eViewType_WTV)
			{
				if (local_mheg_CheckMHEGIsRunning() == FALSE)
				{
					if ((s_MHEGContext.itkProfile == REDKEY_PROFILE_CIPLUS)
						||((s_bMHEGIsReadyAV == TRUE) && (s_bMHEGIsReadyPMT == TRUE)))
					{
						if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
						{
							VK_TASK_Sleep(2);
							/* wait 2 ticks and retry to start Redkey */
							BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 1, 1, 0);
						}
					}
				}
			}
			break;

		case eMEVT_LIVE_NOTIFY_SVC_STOPPED:
		case eMEVT_MHEG_ENGINE_STOP:
			MhegMgrLogFunc(("eMEVT_LIVE_NOTIFY_SVC_STOPPED or eMEVT_MHEG_ENGINE_STOP(%s)\n", (message == eMEVT_LIVE_NOTIFY_SVC_STOPPED) ? "eMEVT_LIVE_NOTIFY_SVC_STOPPED" : "eMEVT_MHEG_ENGINE_STOP" ));

			/* Australia Freeview사양 */
			MhegMgrLogTrace(("ENGINE STOP by MSG!!\n"));

			/* stop redkey */
			if(s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
			}
			break;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eSVC_MP_EVENT_PROBE_FINISHED:
			if (local_mheg_Display_AVS_isSettingStreamingAV() == TRUE)
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_PROBE_FINISHED : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3));

				local_mheg_Display_AVS_processMPEventProbe(hAction, TRUE);

				return MESSAGE_BREAK;
			}
			break;

		case eSVC_MP_EVENT_PROBE_FAILED:
			if (local_mheg_Display_AVS_isSettingStreamingAV() == TRUE)
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_PROBE_FAILED : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3));

				local_mheg_Display_AVS_processMPEventProbe(hAction, FALSE);

				return MESSAGE_BREAK;
			}
			break;

		case eSVC_MP_EVENT_ERROR:
			if(local_mheg_Display_AVS_isSettingStreamingAV() == TRUE)
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_ERROR : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3));

				local_mheg_Display_AVS_processMPEventError(hAction);

				return MESSAGE_BREAK;
			}
			break;

		case eSVC_MP_EVENT_CONNECTING:
			if (local_mheg_Display_AVS_isSettingStreamingAV() == TRUE)
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_CONNECTING : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3));

				return MESSAGE_BREAK;
			}
			break;

#if 0 // for future
		case eSVC_MP_EVENT_GETPRLICENSEURL:
			if (local_mheg_Display_AVS_isSettingStreamingAV() == TRUE)
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_GETPRLICENSEURL : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3));

				return MESSAGE_BREAK;
			}
			break;
#endif

		case eSVC_MP_EVENT_BUFFERING_NOT_ENOUGH:
			if ((s_MHEGContext.context[s_MHEGContext.mastActionId].avMode > MHEG_AV_MODE_BROADCAST)
				 &&(local_mheg_Display_AVS_isSetupStreamingAV() == TRUE))
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_BUFFERING_NOT_ENOUGH : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3));

				return MESSAGE_BREAK;
			}
			break;

		case eSVC_MP_EVENT_PLAYING:
			if ((s_MHEGContext.context[s_MHEGContext.mastActionId].avMode > MHEG_AV_MODE_BROADCAST)
				 &&(local_mheg_Display_AVS_isSetupStreamingAV() == TRUE))
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_PLAYING : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3));
				local_mheg_Display_AVS_controlStreamingSubtitle(hAction, TRUE);
				local_mheg_Display_AVS_startStreamingAV();

				return MESSAGE_BREAK;
			}
			break;

		case eSVC_MP_EVENT_VIDEO_NEW_FRAME:
			if ((s_MHEGContext.context[s_MHEGContext.mastActionId].avMode > MHEG_AV_MODE_BROADCAST)
				 &&(local_mheg_Display_AVS_isSetupStreamingAV() == TRUE))
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_VIDEO_NEW_FRAME : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3));
				local_mheg_Display_AVS_underflowResumeStreamingAV();

				return MESSAGE_BREAK;
			}
			break;

		case eSVC_MP_EVENT_VIDEO_UNDERRUN:
			if ((s_MHEGContext.context[s_MHEGContext.mastActionId].avMode > MHEG_AV_MODE_BROADCAST)
				 &&(local_mheg_Display_AVS_isSetupStreamingAV() == TRUE))
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_VIDEO_UNDERRUN : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3));
				local_mheg_Display_AVS_controlStreamingSubtitle(hAction, FALSE);
				local_mheg_Display_AVS_underflowStreamingAV();

				return MESSAGE_BREAK;
			}
			break;

		case eSVC_MP_EVENT_PLAY_POSITION_CHANGED:
			if ((s_MHEGContext.context[s_MHEGContext.mastActionId].avMode > MHEG_AV_MODE_BROADCAST)
				 &&(local_mheg_Display_AVS_isSetupStreamingAV() == TRUE))
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_PLAY_POSITION_CHANGED : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3));

				local_mheg_Display_AVS_changePositionStreamingAV(hAction);
				return MESSAGE_BREAK;
			}
			break;

		case eSVC_MP_EVENT_END_OF_STREAM:
			if ((s_MHEGContext.context[s_MHEGContext.mastActionId].avMode > MHEG_AV_MODE_BROADCAST)
				 &&(local_mheg_Display_AVS_isSetupStreamingAV() == TRUE))
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_END_OF_STREAM : p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3)); // p1이 media Action handle 임

				local_mheg_Display_AVS_eofPositionStreamingAV(hAction);
				local_mheg_Display_AVS_controlStreamingSubtitle(hAction, FALSE);

				return MESSAGE_BREAK;
			}
			break;

		case eSVC_MP_EVENT_PAUSE:
		case eSVC_MP_EVENT_STOPPED:
			if (s_MHEGContext.context[s_MHEGContext.mastActionId].avMode > MHEG_AV_MODE_BROADCAST)
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_CYAN"eSVC_MP_EVENT_PAUSE or eSVC_MP_EVENT_STOPPED: p1(%d), p2(%d), p3(%d)"C_CODE_RESET"\n", p1, p2, p3)); // p1이 media Action handle 임
				local_mheg_Display_AVS_controlStreamingSubtitle(hAction, FALSE);

				if(message == eSVC_MP_EVENT_STOPPED)
				{
					if(local_mheg_Display_AVS_processMPEventError(hAction) == ERR_OK)
					{
						return MESSAGE_BREAK;
					}
				}
			}
			break;
#endif
		case eMEVT_PG_PIN_CHECK_OK:
			MhegMgrLogFunc(("eMEVT_PG_PIN_CHECK_OK\n"));
			PORT_ITK_MISC_ClosePincodeForPFG(TRUE);
			break;

		case eMEVT_PG_PIN_CHECK_DENIED:
			MhegMgrLogFunc(("eMEVT_PG_PIN_CHECK_DENIED\n"));
			PORT_ITK_MISC_ClosePincodeForPFG(FALSE);
			break;

#if 0
		case MSG_APP_FS_FORMAT_DEVICE:
			/* DNP 동작 도중 Format하려 할 경우 Resource 반환 해야 함. */
			MhegMgrLogFunc(("MSG_APP_FS_FORMAT_DEVICE"));

			if(s_MHEGContext.context[s_MHEGContext.mastActionId].status == REDKEY_RUNNING)
			{
				local_mheg_StopMhegAppl(s_MHEGContext.context[s_MHEGContext.mastActionId].hAction, ITK_SERVICE_TUNE_STOP_HOST);
			}
			break;
#endif

		case eMEVT_MHEG_DO_FACTORY_RESET:
			if(s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
			{
				local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
			}

			/* Reboot되지 않는다면 절대 NVM은 지워서는 안된다 */
			local_mheg_ResetNvsFs(hAction, eMHEG_NVS_FACTORY_RESET);
			local_mheg_ResetDvr(hAction);
			break;

#if 0			// TODO : have to check (yjlim)
// RCT 관련, rct proc이 create 될때 조건 확인해서 Message 보내줌
		case eMEVT_MHEG_DSMCCFS_REQUEST_FILE:
			MhegMgrLogFunc(("eMEVT_MHEG_DSMCCFS_REQUEST_FILE\n"));
			if(PORT_ITK_DSMCCFS_LoadFile((char *)p1) == NULL)
			{
				BUS_PostMessage(xproc_mheg_Manager, eMEVT_MHEG_DSMCCFS_FILE_DATA, hAction, 0, 0, 0);
			}
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DSMCCFS_RELEASE_FILE:
			MhegMgrLogFunc(("eMEVT_MHEG_DSMCCFS_RELEASE_FILE\n"));
			PORT_ITK_DSMCCFS_Release((ITK_DsmccFile_t *)p1);
			return MESSAGE_BREAK;

		case eMEVT_MHEG_DSMCCFS_FILE_DATA:
			MhegMgrLogFunc(("eMEVT_MHEG_DSMCCFS_FILE_DATA\n"));
			BUS_PostMessage(xproc_mheg_Manager, eMEVT_MHEG_DSMCCFS_RELEASE_FILE, hAction, p1, 0, 0);
			return MESSAGE_BREAK;
#endif
		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

HBOOL	xmgr_mheg_GetSubtitleSetting(void)
{
	Handle_t		hService;
	DbSvc_Info_t		svcInfo;

	/* svchandle을 가져온다. */
	if (MGR_LIVE_GetServiceHandle(MGR_ACTION_GetMainActionId(), eLiveViewPlaySvc_ACTUAL, &hService) != ERR_OK)
	{
		return FALSE;
	}

	/* DB에서 svcInfo를 가져온다. */
	if (DB_SVC_GetServiceInfo (hService, &svcInfo) != ERR_OK)
	{
		return FALSE;
	}

	if (svcInfo.ucSubttlIdx == 0xff)
	{
		return FALSE;
	}

	return TRUE;
}

HUINT32	xmgr_mheg_GetTuneFlag(Handle_t hAction)
{
	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);

	return s_MHEGContext.context[ulActionId].tuneFlag;
}

#if 0
static	void local_mheg_ReceiveHamaData(HSIGNAL_e eSignal, HSIGNALTYPE_e eSignalType, HINT32 size, void *data)
{
	HBOOL		bValue = 0;
	static HBOOL	s_bSubtitleEnable = FALSE;

 	MhegMgrLogTrace(("ReceiveHamaData : signal [%d] Size [%d] \n", eSignal, size));

	switch(eSignal)
	{
		case eHSIG_AppsReady:
			/* To to something */
			break;

		case eHSIG_SystemBootupState:
			/* To to something */
			break;

		case eHSIG_GoShutDown:
			/* To to something */
			break;

		case eHSIG_GoReboot:
			/* To to something */
			break;

		case eHSIG_GoOperation:
			/* To to something */
			break;

		case eHSIG_ShowTeleText:
			/* To to something */
			break;

		case eHSIG_ShowSubtitle:
			bValue = ((HUINT32)data == 0) ? FALSE : TRUE;
			if (s_bSubtitleEnable != bValue)
			{
				s_bSubtitleEnable = bValue;
				local_mheg_PostMsgToAp(eMEVT_MHEG_DISPLAY_SUBTITLE_CHANGED, HANDLE_NULL, bValue, 0, 0);
			}
			break;

		case eHSIG_ReadyMainApplication:
			/* To to something */
			break;

		case eHSIG_StartFullBrowser:
			/* To to something */
			break;

		default :
			break;
	}

}
#endif

static HERROR local_mheg_MsgMhegStop (Handle_t hAction)
{
	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);

	if(s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)
	{
		/* Mheg을 stop 하기전에 호출해야 PORT_ITK_DEMUX_NewSiTable 함수에서 hAction 체크 시 오류가 발생하지 않음 */
		PORT_ITK_SetMhegHandle(ulActionId, hAction, HANDLE_NULL, HANDLE_NULL);

		local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
		/* make tune status defaulted -> Stop by Host */

		s_bMHEGIsReadyAV = FALSE;
		s_bMHEGIsReadyPMT = FALSE;

		s_MHEGContext.context[ulActionId].hAction		= hAction;
		s_MHEGContext.context[ulActionId].hDefSvc		= HANDLE_NULL;
		s_MHEGContext.context[ulActionId].hCurSvc		= HANDLE_NULL;
		s_MHEGContext.context[ulActionId].statusTune	= ITK_SERVICE_TUNE_STOP_HOST;
		s_MHEGContext.context[ulActionId].tuneFlag		= 0;

		if( s_MHEGContext.context[ulActionId].pMainContentUri != NULL)
		{
			HLIB_STD_MemFree(s_MHEGContext.context[ulActionId].pMainContentUri);
			s_MHEGContext.context[ulActionId].pMainContentUri = NULL;
		}

		PORT_ITK_SetBroadcastInterruptionStatus(ITK_ENABLE);
		PORT_ITK_SetRctStatus(ITK_DISABLE);

		local_mheg_SetContextCurPesInfo(ulActionId, PID_NULL, PID_NULL, PID_NULL, eDxVIDEO_CODEC_UNKNOWN, eDxAUDIO_CODEC_UNKNOWN);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		local_mheg_Display_AVS_setContextAvMode(ulActionId, MHEG_AV_MODE_BROADCAST);
#endif
	}
	return ERR_OK;
}

HERROR AP_Mheg_SetDeepLinkURL(const char *pszURL, HINT32 prevLcn)
{
	Handle_t	hSvc = HANDLE_NULL;
	HERROR		hErr = ERR_FAIL;
	char		tmpBuf[512] = {0,};
	int			nBufLen = sizeof(tmpBuf);

	MhegMgrLogTrace(("Deep Link URL : %s, prevLcn : %d\n", pszURL, prevLcn));

	/* save original servie information before deep link */
	hErr = DB_SVC_GetServiceHandleByNumber(eDxDELIVERY_TYPE_SAT, eSvcType_All, prevLcn, &hSvc);
	if(hErr == ERR_OK)
	{
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		s_MHEGContext.context[s_MHEGContext.mastActionId].hPrevSvc = hSvc;
#endif
	}

	/* setLaunchArgs */
	if(pszURL)
	{
		strncpy(tmpBuf, pszURL, nBufLen - 1);
		tmpBuf[nBufLen - 1] = '\0';

		RedKey_setLaunchArgs(tmpBuf, (itk_uint16_t)strlen(tmpBuf));
	}

	return ERR_OK;
}

HERROR AP_Mheg_SetMhegOperation(HBOOL bControl)
{
	MhegMgrLogTrace(("AP_Mheg_SetMhegOperation : %s\n", bControl == TRUE? "START":"STOP" ));

	if(bControl == TRUE)
	{
		local_mheg_PostMsgToAp(eMEVT_MHEG_ENGINE_START, (Handle_t)NULL, 0, 0, 0);
	}
	else
	{
		local_mheg_PostMsgToAp(eMEVT_MHEG_ENGINE_STOP, (Handle_t)NULL, 0, 0, 0);
	}

	return ERR_OK;
}

static HERROR local_mheg_GetContextCurPesInfo(HUINT32 ulActionId, HUINT16* pusVideoPid, HUINT16* pusAudioPid, HUINT16* pusPcrPid, HUINT8* pucVideoType, HUINT8* pucAudioType)
{
	MhegMgrLogFatal(ulActionId < NUM_MAX_ACTION);

	if(ulActionId < NUM_MAX_ACTION)
	{
		*pusVideoPid 	= s_MHEGContext.context[ulActionId].curVideoPid;
		*pusAudioPid	= s_MHEGContext.context[ulActionId].curAudioPid;
		*pusPcrPid		= s_MHEGContext.context[ulActionId].curPcrPid;
		*pucVideoType	= s_MHEGContext.context[ulActionId].curVideoType;
		*pucAudioType	= s_MHEGContext.context[ulActionId].curAudioType;
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_mheg_GetContextMastActionId(HUINT32* pMastActionId)
{
	MhegMgrLogFatal(s_MHEGContext.mastActionId != 0xFFFFFFFF && s_MHEGContext.mastActionId < NUM_MAX_ACTION);

	*pMastActionId = s_MHEGContext.mastActionId;

	return ERR_OK;
}

static HERROR local_mheg_GetContextMastActionHandle(Handle_t* phAction)
{
	Handle_t	hMastActionId;

	hMastActionId = s_MHEGContext.mastActionId;
	*phAction = s_MHEGContext.context[hMastActionId].hAction;

	return ERR_OK;
}

#if 0		// not used
static HERROR local_mheg_GetContextActionHandle(HUINT32 ulActionId, Handle_t* phAction)
{
	MhegMgrLogFatal(ulActionId < NUM_MAX_ACTION);

	if(phAction && ulActionId < NUM_MAX_ACTION)
		*phAction = s_MHEGContext.context[ulActionId].hAction;

	return ERR_OK;
}
#endif

static HERROR local_mheg_GetContextSvcHandle(HUINT32 ulActionId, Handle_t* phDefSvcHadle, Handle_t* pCurSvcHandle)
{
	MhegMgrLogFatal(ulActionId < NUM_MAX_ACTION);

	if(ulActionId < NUM_MAX_ACTION)
	{
		*phDefSvcHadle = s_MHEGContext.context[ulActionId].hDefSvc;
		*pCurSvcHandle	= s_MHEGContext.context[ulActionId].hCurSvc;
	}

	return ERR_OK;
}

static HERROR local_mheg_SetContextCurSvcHandle(HUINT32 ulActionId, Handle_t pCurSvcHandle)
{
	MhegMgrLogFatal(ulActionId < NUM_MAX_ACTION);

	if(ulActionId < NUM_MAX_ACTION)
	{
		s_MHEGContext.context[ulActionId].hCurSvc = pCurSvcHandle;
	}
	PORT_ITK_SetMastCurService(pCurSvcHandle);

	return ERR_OK;
}


static HERROR local_mheg_SetMastMhegContext(HUINT32 ulActionId)
{
	MhegMgrLogFatal(ulActionId < NUM_MAX_ACTION);

	if(ulActionId < NUM_MAX_ACTION)
	{
		s_MHEGContext.mastActionId = ulActionId;
		PORT_ITK_SetMhegHandle(ulActionId, s_MHEGContext.context[ulActionId].hAction, s_MHEGContext.context[ulActionId].hDefSvc, s_MHEGContext.context[ulActionId].hCurSvc);
		PORT_ITK_SetMastMhegCurPesInfo(s_MHEGContext.context[ulActionId].curVideoPid, s_MHEGContext.context[ulActionId].curAudioPid, s_MHEGContext.context[ulActionId].curPcrPid,
							s_MHEGContext.context[ulActionId].curVideoType, s_MHEGContext.context[ulActionId].curAudioType);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		PORT_ITK_SetMastMhegAvMode(s_MHEGContext.context[ulActionId].avMode);
#endif
		MhegMgrLogTrace(("change Master Action Id -> %d \r\n", ulActionId));
		return ERR_OK;
	}
	else
	{
		MhegMgrLogError(("Error : ulActionId >= NUM_MAX_ACTION\n"));
		return ERR_FAIL;
	}

}


static HBOOL local_mheg_CheckMHEGIsRunning(void)
{
	HUINT32 ulMastActionId = s_MHEGContext.mastActionId;
	if ((ulMastActionId < NUM_MAX_ACTION) && (s_MHEGContext.context[ulMastActionId].status == REDKEY_RUNNING))
		return TRUE;
	else
		return FALSE;
}

static HBOOL local_mheg_CheckMHEGIsAvailable(Handle_t hAction)
{
	HERROR				hErr = ERR_FAIL;
	HBOOL				bAvailable;
	XmgrFsatMode_e	eFreesatMode;

	hErr = XMGR_SPEC_GetOperationMode_Freesat(&eFreesatMode);
	if(hErr == ERR_OK)					// CONFIG_OP_FREESAT feature : on
	{
		if(eFreesatMode == eFreesatModeFreesat)
		{
			bAvailable = XSVC_SI_MHEG_IsMhegAppAvailable(hAction);
		}
		else
		{
			/* If STB Mode changes to [Non-freesat], there is no INT_FSAT_CheckMhegAvailable() check roution,
			    because it is not freesat mode. So I always set the bAvailable value to FALSE. (by yjlim) */
			bAvailable = FALSE;
		}
	}
	else								// CONFIG_OP_FREESAT feature : off
	{
		bAvailable = XSVC_SI_MHEG_IsMhegAppAvailable(hAction);
	}

	return bAvailable;
}

#if defined (CONFIG_MW_MM_PVR)
static HBOOL local_mheg_CheckIsTuningConflict(Handle_t hOrgSvcHandle, Handle_t *hNewSvcHandle)
{
	HERROR						lResult = ERR_FAIL;
	MgrAction_ConflictInfo_t	cftInfo;

	lResult = MGR_ACTION_CheckTuningConflict(hOrgSvcHandle, &cftInfo);
	if(lResult != ERR_OK)          HxLOG_Critical("[%s:%d] Critical Eror!!!\n", __FUNCTION__, __HxLINE__);

	/* Conflict 확인. */
	if(cftInfo.eType != eAM_Conflict_Nothing)
	{
		HUINT32					ulRecActId;
		MgrRec_RecInfo_t		recInfo;

		for (ulRecActId = eActionId_REC_FIRST; ulRecActId <= eActionId_REC_LAST; ulRecActId++)
		{
			if ( MGR_ACTION_IsRecordingActionId (ulRecActId) == TRUE )
			{
				lResult = MGR_RECORDER_GetRecordingSvcInfo(ulRecActId, &recInfo);
				if (lResult == ERR_OK)
				{
					*hNewSvcHandle = recInfo.hSvc;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

static HBOOL local_mheg_CheckIsRecording(Handle_t hCurSvc)
{
	HERROR					lResult = ERR_FAIL;
	HUINT32					ulRecActId;
	MgrRec_RecInfo_t		recInfo;

	for (ulRecActId = eActionId_REC_FIRST; ulRecActId <= eActionId_REC_LAST; ulRecActId++)
	{
		if ( MGR_ACTION_IsRecordingActionId (ulRecActId) == TRUE )
		{
			lResult = MGR_RECORDER_GetRecordingSvcInfo(ulRecActId, &recInfo);
			if (lResult == ERR_OK)
			{
				if(hCurSvc == recInfo.hSvc)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}
#endif

#ifdef CONFIG_DEBUG
void PORT_ITK_DEBUG_ShowMhegContext(void)
{
	HUINT32			ulActionId;
	char			t_strBuf[512]={0,};
	int				p=0;


	ulActionId = s_MHEGContext.mastActionId;

	MhegMgrLogPrint(("---------------- MHEG Mng Info -------------------\n"));

	if( 0 == s_stMhegMng.tuneFlag )
		p = sprintf(t_strBuf,"|FLAG_NULL||");
	else
	{
		if( s_stMhegMng.tuneFlag & ITK_TUNE_KEEP_CONTEXT)
			p = sprintf(t_strBuf,"|ITK_TUNE_KEEP_CONTEXT|");
		if( s_stMhegMng.tuneFlag & ITK_TUNE_KEEP_DISPLAY )
			p += sprintf(t_strBuf+p,"|ITK_TUNE_KEEP_DISPLAY|");
		if( s_stMhegMng.tuneFlag & ITK_TUNE_NO_BANNER )
			p += sprintf(t_strBuf+p,"|ITK_TUNE_NO_BANNER|");
		if( s_stMhegMng.tuneFlag & ITK_TUNE_KEEP_BROADCAST_INTERRUPT )
			p += sprintf(t_strBuf+p,"|ITK_TUNE_KEEP_BROADCAST_INTERRUPT|");
	}

	MhegMgrLogPrint(("\nAct Id=%d,  \ntuneEvt = %s, \ntuneFlag = %s\n\n",
		s_stMhegMng.curActionId,
		(s_stMhegMng.statusTune == ITK_SERVICE_TUNE_START)? "ITK_SERVICE_TUNE_START" :
			(s_stMhegMng.statusTune == ITK_SERVICE_TUNE_STOP_HOST)? "ITK_SERVICE_TUNE_STOP_HOST" : "ITK_SERVICE_TUNE_STOP_ITK",
		t_strBuf		));


	MhegMgrLogPrint(("---------------- MHEG Cxt Info -------------------\n"));
	MhegMgrLogPrint((" Act Id=%d, Act Handle=0x%x\n", ulActionId, s_MHEGContext.context[ulActionId].hAction));

	MhegMgrLogPrint((" hDefSvc(0x%x), hCurSvc(0x%x)\n",
		s_MHEGContext.context[ulActionId].hDefSvc,
		s_MHEGContext.context[ulActionId].hCurSvc));

	MhegMgrLogPrint(("\n\tredkey status(0x%x : %s)\n\tapp status (0x%x : %s)\n\ttune status(0x%x : %s)\n\ttune flag (0x%x)\n\n",
		s_MHEGContext.context[ulActionId].status,
		(s_MHEGContext.context[ulActionId].status == REDKEY_UNINITIALISED)?"REDKEY_UNINITIALISED" :
			(s_MHEGContext.context[ulActionId].status == REDKEY_STOPPED)?"REDKEY_STOPPED" :
				(s_MHEGContext.context[ulActionId].status == REDKEY_RUNNING)?"REDKEY_RUNNING" : "REDKEY_LOST" ,

		s_MHEGContext.context[ulActionId].appStatus,
		(s_MHEGContext.context[ulActionId].appStatus == REDKEY_MHEG_APP_NOT_RUNNING)?"REDKEY_MHEG_APP_NOT_RUNNING" :
			(s_MHEGContext.context[ulActionId].appStatus == REDKEY_MHEG_APP_STARTING)?"REDKEY_MHEG_APP_STARTING" :
				(s_MHEGContext.context[ulActionId].appStatus == REDKEY_MHEG_APP_RUNNING)?"REDKEY_MHEG_APP_RUNNING" :"REDKEY_MHEG_APP_STOPPING" ,

		s_MHEGContext.context[ulActionId].statusTune,
		(s_MHEGContext.context[ulActionId].statusTune == ITK_SERVICE_TUNE_START)?"ITK_SERVICE_TUNE_START" :
			(s_MHEGContext.context[ulActionId].statusTune == ITK_SERVICE_TUNE_STOP_HOST)?"ITK_SERVICE_TUNE_STOP_HOST" : "ITK_SERVICE_TUNE_STOP_ITK",

		s_MHEGContext.context[ulActionId].tuneFlag));

	MhegMgrLogPrint((" mheg BroadcastInterruption(%s) RCT(%s)\n",
		PORT_ITK_GetBroadcastInterruptionStatus() == ITK_DISABLE ? "ITK_DISABLE" : "ITK_ENABLE",
		PORT_ITK_GetRctStatus() == ITK_DISABLE ? "ITK_DISABLE" : "ITK_ENABLE" ));

	MhegMgrLogPrint((" curVideotype(0x%x), curVideoPid(0x%x) / curAudioType(0x%x), curAudioPid(0x%x) / curPcrPid(0x%x)\n",
		s_MHEGContext.context[ulActionId].curVideoType,
		s_MHEGContext.context[ulActionId].curVideoPid,
		s_MHEGContext.context[ulActionId].curAudioType,
		s_MHEGContext.context[ulActionId].curAudioPid,
		s_MHEGContext.context[ulActionId].curPcrPid));

	MhegMgrLogPrint((" display AV status(0x%x)\n", s_MHEGContext.context[ulActionId].displayAVStatus));

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	MhegMgrLogPrint(("IPSTREAMING FUNC:\n\tavMode(%d)\n\tavStatus(0x%x)\n\thPrevSvc(0x%x)\n",
		s_MHEGContext.context[ulActionId].avMode,
		s_MHEGContext.context[ulActionId].avStatus,
		s_MHEGContext.context[ulActionId].hPrevSvc));
#endif

	MhegMgrLogPrint(("Current main Act ID = 0x%X\n",  MGR_ACTION_GetMainActionId() ));
	MhegMgrLogPrint(("Current Act type(main ActId = %d) = 0x%X\n",  MGR_ACTION_GetMainActionId(), MGR_ACTION_GetType(MGR_ACTION_GetMainActionId()) ));

	MhegMgrLogPrint(("----------------------------------------------\n)"));
#if defined(CONFIG_MW_MM_PVR)
	MhegMgrLogPrint(("s_stMhegHide.bTrickEntering = %s\n", s_stMhegHide.bTrickEntering?"TRUE":"FALSE"));
	MhegMgrLogPrint(("s_stMhegHide.bTrickExiting = %s\n", s_stMhegHide.bTrickExiting?"TRUE":"FALSE"));
#endif
	MhegMgrLogPrint(("s_stMhegHide.bQuiteTuneMode = %s\n", s_stMhegHide.bQuiteTuneMode?"TRUE":"FALSE"));
	MhegMgrLogPrint(("s_stMhegHide.hLastMasterSvc = 0x%X\n", s_stMhegHide.hLastMasterSvc ));
	MhegMgrLogPrint(("s_stMhegHide.hLastSuppleSvc = 0x%X\n", s_stMhegHide.hLastSuppleSvc ));

	MhegMgrLogPrint(("==============================================\n"));
}

void PORT_ITK_DEBUG_SetupMhegMediaResource(void)
{
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HUINT32	ulActionId;
	Handle_t hAction;

	MhegMgrLogPrint(("================ Setup MHEG Media Resource ===================\n"));

	ulActionId = s_MHEGContext.mastActionId;
	hAction = s_MHEGContext.context[ulActionId].hAction;
	ulActionId = SVC_PIPE_GetActionId(hAction);

	MGR_ACTION_SetupMedia(ulActionId, MHEG_STREAMING_VIEW_TYPE, eAmPriority_VIEW_ACTION);
	MGR_ACTION_Take(ulActionId, MHEG_STREAMING_VIEW_TYPE, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
#endif
}

#endif

static void  local_mheg_initMhegContext()
{
	HUINT16		usAction;

	s_MHEGContext.mastActionId = eActionId_VIEW_0;		/* default master action Id */
	s_MHEGContext.bSubtitleEnable = FALSE;

	for(usAction = 0; usAction < NUM_MAX_ACTION; usAction++)
	{
		s_MHEGContext.context[usAction].hAction			= HANDLE_NULL;
		s_MHEGContext.context[usAction].hDefSvc			= HANDLE_NULL;
		s_MHEGContext.context[usAction].hCurSvc			= HANDLE_NULL;
		s_MHEGContext.context[usAction].status			= REDKEY_UNINITIALISED;
		s_MHEGContext.context[usAction].statusTune		= ITK_SERVICE_TUNE_STOP_HOST;
		s_MHEGContext.context[usAction].tuneFlag		= 0;

		s_MHEGContext.context[usAction].curVideoPid		= PID_NULL;
		s_MHEGContext.context[usAction].curAudioPid		= PID_NULL;
		s_MHEGContext.context[usAction].curPcrPid		= PID_NULL;
		s_MHEGContext.context[usAction].curVideoType	= eDxVIDEO_CODEC_UNKNOWN;
		s_MHEGContext.context[usAction].curAudioType	= eDxAUDIO_CODEC_UNKNOWN;
		s_MHEGContext.context[usAction].displayAVStatus = eITK_DISPLAY_EVENT_STOP;
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		s_MHEGContext.context[usAction].avMode			= MHEG_AV_MODE_BROADCAST;
		s_MHEGContext.context[usAction].hPrevSvc			= HANDLE_NULL;
		s_MHEGContext.context[usAction].player			= NULL;
		s_MHEGContext.context[usAction].pMainContentUri	= NULL;
		s_MHEGContext.context[usAction].pstTempStreamHandler	= NULL;
		s_MHEGContext.context[usAction].cbTlsCertificate		= NULL;
#endif
#if defined(MHEG_MGR_SYNC_VIDEO_SUBTITLE)
		s_MHEGContext.context[usAction].ucChangedVideoPid = MHEG_MGR_PID_INIT_VALUE;
#endif

		s_MHEGContext.context[usAction].imageDisplayFlag = FALSE;
		s_MHEGContext.context[usAction].hImage = NULL;

		s_MHEGContext.context[usAction].bMHEGStopByHost = FALSE;
	}

	local_mheg_initMhegHideContext();
	PORT_ITK_SetBroadcastInterruptionStatus(ITK_ENABLE);
	PORT_ITK_SetRctStatus(ITK_DISABLE);
}

static void local_mheg_initMhegHideContext(void)
{
#if defined(CONFIG_MW_MM_PVR)
	s_stMhegHide.bTrickEntering = FALSE;
	s_stMhegHide.bTrickExiting = FALSE;
#endif
	s_stMhegHide.bQuiteTuneMode = FALSE;
	s_stMhegHide.hLastMasterSvc = HANDLE_NULL;
	s_stMhegHide.hLastSuppleSvc = HANDLE_NULL;
}

static HERROR local_mheg_SetContextCurPesInfo(HUINT32 ulActionId, HUINT16 usVideoPid, HUINT16 usAudioPid, HUINT16 usPcrPid,
											HUINT8 ucVideoType, HUINT8 ucAudioType)
{
	MhegMgrLogFatal(ulActionId < NUM_MAX_ACTION);
	MhegMgrLogTrace(("Set Context PID:V/A/P(0x%x, 0x%x, 0x%x), Type:V/A(%d, %d)\n", usVideoPid, usAudioPid, usPcrPid, ucVideoType, ucAudioType));

	if(ulActionId < NUM_MAX_ACTION)
	{
		if ((usVideoPid != 0) && (usVideoPid != PID_CURRENT))
		{
			s_MHEGContext.context[ulActionId].curVideoPid	= usVideoPid;
		}
		if ((usAudioPid != 0) && (usAudioPid != PID_CURRENT))
		{
			s_MHEGContext.context[ulActionId].curAudioPid	= usAudioPid;
		}
		if ((usPcrPid != 0) && (usPcrPid != PID_CURRENT))
		{
			s_MHEGContext.context[ulActionId].curPcrPid		= usPcrPid;
		}
		if (ucVideoType)
		{
			s_MHEGContext.context[ulActionId].curVideoType	= ucVideoType;
		}
		if (ucAudioType)
		{
			s_MHEGContext.context[ulActionId].curAudioType	= ucAudioType;
		}
	}

	if (ulActionId == s_MHEGContext.mastActionId)
	{
		PORT_ITK_SetMastMhegCurPesInfo(usVideoPid, usAudioPid, usPcrPid, ucVideoType, ucAudioType);
	}

#if defined(MHEG_MGR_SYNC_VIDEO_SUBTITLE)
	if(ulActionId < NUM_MAX_ACTION)
	{
		if ((usVideoPid == PID_NULL) && (usAudioPid == PID_NULL) && (usPcrPid == PID_NULL) &&
			(ucVideoType == eDxVIDEO_CODEC_UNKNOWN) && (ucAudioType == eDxAUDIO_CODEC_UNKNOWN))
		{
			s_MHEGContext.context[ulActionId].ucChangedVideoPid = MHEG_MGR_PID_INIT_VALUE;
		}
		else
		{
			s_MHEGContext.context[ulActionId].ucChangedVideoPid = usVideoPid;
		}
	}
#endif

	return ERR_OK;
}

void local_mheg_CallbackRedkeyState(void* context, redkey_state_t state)
{
#if defined (MHEG_UI_ORDER_BY_RUNNING_STATUS)
	Handle_t hAction = SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());
#endif
	HBOOL	bMhegRunning;

	MhegMgrLogTrace((C_CODE_F_BLACK_B_GREEN"actionId(0x%x) -> change MHEG Status to 0x%x [0:uninit, 1:stop, 2:start, 3:lost]"C_CODE_RESET"\n", s_MHEGContext.mastActionId, state));

	if (s_MHEGContext.context[s_MHEGContext.mastActionId].status == state)
	{
		MhegMgrLogAssert(("Skip Same Redkey state!\n"));
		return;
	}

	s_MHEGContext.context[s_MHEGContext.mastActionId].status = state;
	bMhegRunning = (state == REDKEY_RUNNING) ? TRUE : FALSE;
	PORT_ITK_SetMhegRunningStatus(bMhegRunning);

#if defined (MHEG_UI_ORDER_BY_RUNNING_STATUS)
	// MHEG 을 위한 OSD object 를 만들지 결정
	if (state == REDKEY_RUNNING)
	{
		BUS_PostMessage(NULL, eMEVT_MHEG_STATUS_NOTIFY, hAction, TRUE, 0, 0);
	}
	else
	{
		BUS_PostMessage(NULL, eMEVT_MHEG_STATUS_NOTIFY, hAction, FALSE, 0, 0);
	}
#endif
}

void local_mheg_CallbackRedkeyAppState(void* context, redkey_mheg_app_state_t appState)
{
	HUINT32	p1 = 0, p2 = 0;

	MhegMgrLogTrace((C_CODE_F_BLACK_B_GREEN"actionId(0x%x) -> change MHEG App Status to 0x%x [0:not running, 1:starting, 2:running, 3:stopping]"C_CODE_RESET"\n", s_MHEGContext.mastActionId, appState));

	if (s_MHEGContext.context[s_MHEGContext.mastActionId].appStatus == appState)
	{
		MhegMgrLogAssert(("Skip Same Redkey App state!\n"));
		return;
	}

	s_MHEGContext.context[s_MHEGContext.mastActionId].appStatus = appState;

	p1 =  (HUINT32)appState;

	if(appState == REDKEY_MHEG_APP_NOT_RUNNING)
	{
		p2 = (HUINT32)s_MHEGContext.context[s_MHEGContext.mastActionId].bMHEGStopByHost;

		if(s_MHEGContext.context[s_MHEGContext.mastActionId].bMHEGStopByHost)
		{
			s_MHEGContext.context[s_MHEGContext.mastActionId].bMHEGStopByHost = FALSE;
		}
	}

	if (s_MHEGContext.itkProfile == REDKEY_PROFILE_CIPLUS)
		PORT_ITK_APPMMI_NotifyEngineStatus(appState);

	MhegMgrLogTrace((C_CODE_F_YELLOW"Send MHEG app state to web application, p1 : %d, p2 : %d"C_CODE_RESET"\n", p1, p2));

	BUS_PostMessageToOM(NULL, eMEVT_MHEG_TYPE_MHEG_APPSTATE, HANDLE_NULL, p1, p2, 0);
}

HERROR local_mheg_ConvertDfbKeyToRedkeyCode(DFBInputDeviceKeySymbol symbol, HINT32 code, HUINT32 *pKeyCode)
{
	HUINT32					ulItkKeyCode = 0;
	HERROR					hErr = ERR_OK;

	switch(symbol)
	{
		case	DIKS_0:					ulItkKeyCode = ITK_KEY_0;				break;
		case	DIKS_1:					ulItkKeyCode = ITK_KEY_1;				break;
		case	DIKS_2:					ulItkKeyCode = ITK_KEY_2;				break;
		case	DIKS_3:					ulItkKeyCode = ITK_KEY_3;				break;
		case	DIKS_4:					ulItkKeyCode = ITK_KEY_4;				break;
		case	DIKS_5:					ulItkKeyCode = ITK_KEY_5;				break;
		case	DIKS_6:					ulItkKeyCode = ITK_KEY_6;				break;
		case	DIKS_7:					ulItkKeyCode = ITK_KEY_7;				break;
		case	DIKS_8:					ulItkKeyCode = ITK_KEY_8;				break;
		case	DIKS_9:					ulItkKeyCode = ITK_KEY_9;				break;

		case	DIKS_RED:				ulItkKeyCode = ITK_KEY_RED;				break;
		case	DIKS_GREEN:				ulItkKeyCode = ITK_KEY_GREEN;			break;
		case	DIKS_YELLOW:			ulItkKeyCode = ITK_KEY_YELLOW;			break;
		case	DIKS_BLUE:				ulItkKeyCode = ITK_KEY_BLUE;			break;

		case	DIKS_ENTER:				ulItkKeyCode = ITK_KEY_SELECT;			break;
//		case	DIKS_EXIT:				ulItkKeyCode = ITK_KEY_CANCEL;			break;
		case	DIKS_LAST:				ulItkKeyCode = ITK_KEY_CANCEL;			break;
		case	DIKS_TEXT:				ulItkKeyCode = ITK_KEY_TEXT;			break;

		case	DIKS_INFO:				ulItkKeyCode = ITK_KEY_INFO;			break;
		case	DIKS_STOP:				ulItkKeyCode = ITK_KEY_STOP;			break;
		case	DIKS_PLAY:				ulItkKeyCode = ITK_KEY_PLAY;			break;
		case	DIKS_PAUSE:				ulItkKeyCode = ITK_KEY_PAUSE;			break;
		case	DIKS_PREVIOUS:			ulItkKeyCode = ITK_KEY_SKIP_BACK;		break;
		case	DIKS_NEXT:				ulItkKeyCode = ITK_KEY_SKIP_FORWARD;	break;
		case	DIKS_FASTFORWARD:		ulItkKeyCode = ITK_KEY_FAST_FORWARD;	break;
		case	DIKS_REWIND:			ulItkKeyCode = ITK_KEY_REWIND;			break;

		case	DIKS_CURSOR_UP:			ulItkKeyCode = ITK_KEY_UP;				break;
		case	DIKS_CURSOR_DOWN:		ulItkKeyCode = ITK_KEY_DOWN;			break;
		case	DIKS_CURSOR_LEFT:		ulItkKeyCode = ITK_KEY_LEFT;			break;
		case	DIKS_CURSOR_RIGHT:		ulItkKeyCode = ITK_KEY_RIGHT;			break;

		case	DIKS_EPG:				ulItkKeyCode = ITK_KEY_GUIDE;			break;
		case	DIKS_MENU:				ulItkKeyCode = ITK_KEY_MENU;			break;

		case	DIKS_HELP:				ulItkKeyCode = ITK_KEY_HELP;			break;

		default:
			MhegMgrLogTrace(("===> symbol(0x%X, %d)(0x%X, %d)\n", symbol, symbol, code, code));
			hErr = ERR_FAIL;
			break;
	}

	*pKeyCode = ulItkKeyCode;

	return hErr;
}

static HERROR local_mheg_SendKeyToClient(DFBInputDeviceKeySymbol key_symbol, HINT32 key_code)
{
	HUINT32					ulItkKeyCode;
	itk_validation_t		eIsAvailable;
	HERROR					hErr = ERR_FAIL;

	hErr = local_mheg_ConvertDfbKeyToRedkeyCode(key_symbol, key_code, &ulItkKeyCode);
	if (hErr == ERR_OK)
	{
		/* Send key to redkey engine */
		PORT_ITK_Display_SendKey(ulItkKeyCode, &eIsAvailable);
		if(eIsAvailable == ITK_VALID)
		{
			MhegMgrLogTrace(("MHEG consumed [%d] key!!\n", ulItkKeyCode));
			hErr = ERR_OK;
		}
		else
		{
			MhegMgrLogTrace(("MHEG doesn't consumed [%d] key!!\n", ulItkKeyCode));
			hErr = ERR_FAIL;
		}
	}

	return hErr;
}

static HERROR local_mheg_SendKeyToHost(DFBInputDeviceKeySymbol key_symbol, HINT32 key_code)
{
	HERROR		hErr = ERR_FAIL;
	redkey_profile_t	eItkProfile = s_MHEGContext.itkProfile;

	if(eItkProfile == REDKEY_PROFILE_CIPLUS)
		return hErr;

	switch(key_symbol)
	{
		case DIKS_EXIT:
			MhegMgrLogTrace(("HOST consumed [%d] key (EXIT KEY)!!\n", key_code));
			local_mheg_PostMsgToAp(eMEVT_MHEG_RESTART_MHEG_APPLICATION, 0, 0, 0, 0);
			hErr = ERR_OK;
			break;

		default:
			hErr = ERR_FAIL;
			break;
	}

	return hErr;
}

static void local_mheg_Display_AVS_EventCallback(ITK_AvstreamHandle_t *pstStreamHandler, itk_media_event_t evt, itk_media_event_data_t *pMediaEvt)
{
	if (pstStreamHandler && pstStreamHandler->callback)
	{
		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (" pstStreamHandler(%p) event(%d)[0=PRE, 1=PLAY, 2=STOP, 3=TRI, 4=STA, 5=RES, 6=U_S]\n", pstStreamHandler, evt));
		pstStreamHandler->callback(pstStreamHandler->callback_context, (itk_avstream_t*)pstStreamHandler, evt, pMediaEvt);
	}
}

static HERROR local_mheg_Display_AVS_release(struct ITK_AvstreamHandle_t* pstStreamHandler)
{
	HERROR	ulErr = ERR_OK;
	ITK_AvstreamHandle_t *prevAVS_V = (ITK_AvstreamHandle_t *)local_mheg_Display_AVS_getCurHandle_V();
	ITK_AvstreamHandle_t *prevAVS_A = (ITK_AvstreamHandle_t *)local_mheg_Display_AVS_getCurHandle_A();

	if (pstStreamHandler == NULL)
	{
		MhegMgrLogAssert(("input parameter is invalid NULL\r\n"));
		return ERR_FAIL;
	}

	MhegMgrLogTrace(("Stream handler (%p), eStreamRef (%d), prevAVS_V (%p), prevAVS_A (%p)\n", pstStreamHandler, pstStreamHandler->eStreamRef, prevAVS_V, prevAVS_A));

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI)
	{
		if((pstStreamHandler == prevAVS_V) || ((prevAVS_V == NULL) && (pstStreamHandler == prevAVS_A)))
		{
			local_mheg_Display_AVS_destroyStreamingAv(pstStreamHandler->hAction);
		}
	}
#endif

	if (pstStreamHandler == prevAVS_V)
	{
		local_mheg_Display_AVS_setCurHandle_V(NULL);
	}
	if (pstStreamHandler == prevAVS_A)
	{
		local_mheg_Display_AVS_setCurHandle_A(NULL);
	}

	//HLIB_STD_MemFree(pstStreamHandler);

	return ulErr;
}

static void local_mheg_Display_AVS_preload(struct ITK_AvstreamHandle_t *pstStreamHandler)
{
	itk_media_event_data_t	stMediaEventData;
	itk_int32_t				preload = ITK_PRESENT;
	itk_int32_t				stream_length = -1;

	/**
	* Preloads an avstream
	*
	* Preloading causes the host to put the stream in a suitable state for "immediate"
	* presentation. This may mean connecting to a media server and buffering the stream
	* or dereferencing the stream reference to a stream object.
	*/
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("Stream handler (%p) -> eStreamRef(0x%x), videoPid(0x%x), audioPid(0x%x), hService(0x%x), ServiceId(0x%x)\n",
					pstStreamHandler, pstStreamHandler->eStreamRef,
					pstStreamHandler->videoPid, pstStreamHandler->audioPid,
					pstStreamHandler->svcHandle, pstStreamHandler->serviceId));

	HxSTD_memset(&stMediaEventData, 0x00, sizeof(itk_media_event_data_t));

	switch(pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_DVB:
		case eITK_REFSVC_DEF:
		case eITK_REFSVC_LCN:
		case eITK_REFSVC_FSI:
		case eITK_REFSVC_CUR:

		#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		/*
			eITK_REFSVC_HTTP type의 streamhander가 생성된 이후에 AV를 stop하기 위해서 eITK_REFSVC_DVB type의 streamhander가 생성되는 경우가 있다.
			이런 경우 local_mheg_Display_AVS_leaveStreamingView 함수의 호출을 해서는 안되며 이미 local_mheg_Display_AVS_enterStreamingView 함수가 호출되면서
			Action이 변경되면서 이전 AV는 stop되므로 skip하도록 해야 한다.
		*/
		if (local_mheg_Display_AVS_isSettingStreamingAV() == TRUE)
		{
			if (PORT_ITK_AVS_GetHandle(pstStreamHandler) == NULL)
			{
				MhegMgrLogAssert(("Already released !\n"));
				preload = ITK_NOT_PRESENT;
				break;
			}
			/* ICS를 위한 media action을 이전 action으로 변경함 */
			local_mheg_Display_AVS_destroyStreamingAv(pstStreamHandler->hAction);
		}

		if(MGR_ACTION_GetType(SVC_PIPE_GetActionId(pstStreamHandler->hAction)) != eViewType_WTV)
		{
			MGR_LIVE_CmdSendServiceStartByActionId(SVC_PIPE_GetActionId(pstStreamHandler->hAction), pstStreamHandler->svcHandle); // TODO: < 확인 필요 > eNO_IPLATE 에 대한 처리는 적용되었는지
		}
		#endif
/*
	DTG 3.13.7 Secne 2/4 issue
	dvb://FFFF..FFFF와 같이 invalid reference가 오더라도 avstream은 created되어야 한다.
	Preload될때 기존 av stop하고 ITK_NOT_PRESENT event callback 으로 처리한다.

	This test case fails due to you failing to create an avstream with the dvb://FFFF..FFFF.
	This call should only fail in cases such as being unable to allocate memory etc.
	If the requested stream does not exist however as is the case with dvb://FFFF..FFFF then the operation should fail at the preload stage.
	We will add additional notes to the documentation to clarify this behaviour.
	You should stop the previous AV and display blank video.
*/
#if 1
			if (pstStreamHandler->svcHandle == HANDLE_NULL)
			{
				ITK_AvstreamHandle_t *prevAVS_V = (ITK_AvstreamHandle_t *)local_mheg_Display_AVS_getCurHandle_V();
				ITK_AvstreamHandle_t *prevAVS_A = (ITK_AvstreamHandle_t *)local_mheg_Display_AVS_getCurHandle_A();

				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("===========> NOT PRESENT pstStreamHandler (%p), eStreamRef (0x%x)\n", pstStreamHandler, pstStreamHandler->eStreamRef));
				stMediaEventData.preloadData.result	= ITK_NOT_PRESENT;
				stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_NOT_FOUND;

				/* Preload event callback을 Stop event callback보다 나중에 해도 되지만, 순서상 이게 맞는것 같다 */
				local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);

				if (prevAVS_V != NULL)
				{
					local_mheg_Display_AVS_stopStream(prevAVS_V, 0);
					if ((prevAVS_A != NULL) && (prevAVS_V != prevAVS_A))
					{
						local_mheg_Display_AVS_stopStream(prevAVS_A, 0);
					}
				}
			}
			else
#endif
			{
				/* make Media Event Data */
				stMediaEventData.preloadData.result = preload;		// <== check point #51
				stMediaEventData.preloadData.resultData.streamLength =  stream_length;
				pstStreamHandler->mediaStatus = ((pstStreamHandler->mediaStatus & 0xF0) | ITK_DISPLAY_AV_STATUS_PRELOAD);

				local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
			}
			break;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
			{
				HERROR			hErr = ERR_OK;

				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("Streaming AV(%p) - avComp(%ld/%ld), avMode(%d), reference(%s) !\n",
						pstStreamHandler, pstStreamHandler->audioComponent, pstStreamHandler->videoComponent, pstStreamHandler->avMode, pstStreamHandler->reference));

				if (PORT_ITK_AVS_GetHandle(pstStreamHandler) == NULL)
				{
					MhegMgrLogAssert(("Already released !\n"));
					preload = ITK_NOT_PRESENT;
					break;
				}

				/* Media 관련 동작 (Async) 처리를 위해서 Stream Handler를 임시로 저장,
				    Preload() 에서 등록하고 Destroy() 에서 NULL로 초기화 함
				    (local_mheg_Display_AVS_getCurHandle() 함수는 Play() 이후에만 사용 가능함) */
				s_MHEGContext.context[s_MHEGContext.mastActionId].pstTempStreamHandler = pstStreamHandler;

				hErr = local_mheg_Display_AVS_createStreamingAV(pstStreamHandler->hAction, pstStreamHandler->reference, pstStreamHandler->avMode);
				if (hErr != ERR_OK)
				{
					/* MEDIA 2.0에서 이 경우는 MGR_MPlayer_Create()가 FAIL 일 경우만 해당됨 */
					local_mheg_Display_AVS_destroyStreamingAv(pstStreamHandler->hAction);

					MhegMgrLogError(("Fail to create Streaming AV(0x%x) !\n", hErr));
					preload = ITK_NOT_PRESENT;
					stMediaEventData.preloadData.result = preload;
					stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_NOT_FOUND;
					local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
				}
			}
			break;
#endif
		default:
			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("Stream handler (%p) -> StreamRef(%d)\n", pstStreamHandler, pstStreamHandler->eStreamRef));
			stMediaEventData.preloadData.result	= ITK_NOT_PRESENT;
			stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_NOT_FOUND;

			local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
			MhegMgrLogFatal(0);
			break;
	}

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("--- Stream handler (%p) PreLoaded (%s)!!!\n", pstStreamHandler, (preload == ITK_PRESENT) ? "ITK_PRESENT" : "ITK_NOT_PRESENT"));

}

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static HERROR local_mheg_Display_AVS_processMPEventProbe(Handle_t hAction, HBOOL bProbeSuccess)
{
	HERROR hErr = ERR_FAIL;
	ITK_AvstreamHandle_t 		*pstStreamHandler = NULL;

	HUINT32						ulActionId = SVC_PIPE_GetActionId(hAction);

	SvcMp_VideoTrack_t	stVideoTrack;
	SvcMp_AudioTrack_t	stAudioTrack;
	MGR_MPlayer_t				*player = NULL;
	HUINT64						stream_length = -1;

	itk_media_event_data_t		stMediaEventData;
	itk_int32_t					preload = ITK_PRESENT;
	SvcMp_Error_e		eError = eSVC_MP_ERROR_UNKNOWN;

	pstStreamHandler = s_MHEGContext.context[ulActionId].pstTempStreamHandler;
	player = s_MHEGContext.context[ulActionId].player;

	if (pstStreamHandler == NULL)
	{
		MhegMgrLogAssert(("Already released !\n"));
		return ERR_OK;
	}

	if(bProbeSuccess == TRUE)
	{
		hErr = SVC_MP_GetActiveVideoTrack(player, &stVideoTrack);
		if (hErr != ERR_OK)
		{
			MhegMgrLogError(("while calling SVC_MP_GetActiveVideoTrack(0x%x) !\n", hErr));
			goto __ERROR;
		}

		hErr = SVC_MP_GetActiveAudioTrack(player, &stAudioTrack);
		if (hErr != ERR_OK)
		{
			MhegMgrLogError(("while calling SVC_MP_GetActiveAudioTrack(0x%x) !\n", hErr));
			goto __ERROR;
		}

		hErr = SVC_MP_GetDuration(player, &stream_length);
		if (hErr != ERR_OK)
		{
			MhegMgrLogError(("while calling SVC_MP_GetDuration(0x%x) !\n", hErr));
			goto __ERROR;
		}

		/* IPL001 Content Format 관련 수정 video codec : MPEG-2*/
		if ( (stVideoTrack.eVideoCodec != eDxVIDEO_CODEC_H264 && stVideoTrack.eVideoCodec != eDxVIDEO_CODEC_MPEG2)
				|| (stAudioTrack.eAudioCodec != eDxAUDIO_CODEC_AAC && stAudioTrack.eAudioCodec != eDxAUDIO_CODEC_AAC_PLUS && stAudioTrack.eAudioCodec != eDxAUDIO_CODEC_UNKNOWN) )
		{
			MhegMgrLogError(("Media settup is not initialized, because of Non-linear stream format (0x%x), Codec AV (%d/%d)\n", hAction, stAudioTrack.eAudioCodec, stVideoTrack.eVideoCodec));

			goto __ERROR;
		}

		local_mheg_Display_AVS_setStreamingAvStatus(local_mheg_Display_AVS_getStreamingAvStatus() | ITK_DISPLAY_AV_STREAMING_STATUS_SETUP);

		// TODO: Language selection(subtitle & audio) 를 위해 si raw data처리가 필요함
		preload = ITK_PRESENT;

		stMediaEventData.preloadData.result = preload;
		pstStreamHandler->mediaStatus = ((pstStreamHandler->mediaStatus & 0xF0) | ITK_DISPLAY_AV_STATUS_PRELOAD);
		stMediaEventData.preloadData.resultData.streamLength = (itk_int32_t)stream_length;

		MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("Preloads AV(%p) - duration (%ld)\n", pstStreamHandler, stream_length));

		local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);

//		s_MHEGContext.context[ulActionId].pstTempStreamHandler = NULL;

		return hErr;
	}
	else
	{
		goto __ERROR;
	}

__ERROR:
	local_mheg_Display_AVS_destroyStreamingAv(pstStreamHandler->hAction);

	MhegMgrLogError(("Fail to create Streaming AV(0x%x) !\n", hErr));

	local_mheg_Display_AVS_setStreamingAvStatus(local_mheg_Display_AVS_getStreamingAvStatus() & ~ITK_DISPLAY_AV_STREAMING_STATUS_SETTING);

	preload = ITK_NOT_PRESENT;

	stMediaEventData.preloadData.result = preload;

	eError = SVC_MP_GetLastErrorReason(player);
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("SVC_MP_GetLastErrorReason(), Error : %d\n", eError));

	switch(eError)
	{
		case eSVC_MP_ERROR_SERVER_HTTP_STATUS_UNAUTHORIZED:
		case eSVC_MP_ERROR_SERVER_HTTP_STATUS_FORBIDDEN:
		case eSVC_MP_ERROR_SERVER_HTTP_STATUS_PROXY_AUTH_REQUIRED:
			stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_NOT_ALLOWED;
			break;

		default:
			stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_NOT_FOUND;
			break;
	}

	local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);

//	s_MHEGContext.context[ulActionId].pstTempStreamHandler = NULL;

	return hErr;
}

static HERROR local_mheg_Display_AVS_processMPEventError(Handle_t hAction)
{
	ITK_AvstreamHandle_t *pstStreamHandler;

	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);

	pstStreamHandler = s_MHEGContext.context[ulActionId].pstTempStreamHandler;

	if (pstStreamHandler == NULL)
	{
		MhegMgrLogAssert(("Already released !\n"));
		return ERR_FAIL;
	}

	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("mediaStatus : 0x%x\n", pstStreamHandler->mediaStatus));

	if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PLAYING)
	{
		pstStreamHandler->mediaStatus = ITK_DISPLAY_AV_STATUS_STOPPED;
		local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_STOPPED, NULL);
		return ERR_OK;
	}

	return ERR_FAIL;
}
#endif
/*
	PID_NULL(0x2000)		-> 해당 component를 stop
	PID_CURRENT(0x2001)	-> 현재 재생중인 component를 유지
*/
static HERROR local_mheg_Display_AVS_playStream(struct ITK_AvstreamHandle_t* pstStreamHandler, HUINT32 ulStartTime)
{
	HUINT16		usCurVideoPid = PID_NULL, usCurAudioPid = PID_NULL, usCurPcrPid = PID_NULL;
	HUINT8		ucCurVideoType = eDxVIDEO_CODEC_UNKNOWN, ucCurAudioType = eDxAUDIO_CODEC_UNKNOWN;
	HUINT32		ulActionId = 0;
	HUINT32		ulPcrPid = PID_CURRENT;
	HERROR		ulErr = ERR_OK;
	HUINT16		usVideoPid = PID_CURRENT, usAudioPid = PID_CURRENT;
	HUINT16		usVideoType = CODEC_CURRENT, usAudioType = CODEC_CURRENT;
	Handle_t	hAction;

	if(PORT_ITK_GetSubMhegRunningStatus() == FALSE) {
		MhegMgrLogAssert(("Redkey Engine should not be running!\r\n"));
		return ERR_FAIL;
	}

	if(pstStreamHandler == NULL)
	{
		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (" ERROR : input parameter is NULL\n"));
		return ERR_FAIL;
	}

	hAction = pstStreamHandler->hAction;

	if(SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK)
	{
		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (" Old ActionHandle(0x%x) --> dump\n", hAction));
		return ERR_FAIL;
	}

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	if (local_mheg_Display_AVS_isSettingStreamingAV() == TRUE)
	{
		if (PORT_ITK_AVS_GetHandle(pstStreamHandler) == NULL)
		{
			MhegMgrLogAssert(("Already released !\n"));
			return ERR_FAIL;
		}
	}

	if(s_MHEGContext.context[ulActionId].pMainContentUri != NULL)
	{
		if( (s_MHEGContext.context[ulActionId].bPlayAndSeek == FALSE)
			&& (strstr(pstStreamHandler->reference, s_MHEGContext.context[ulActionId].pMainContentUri) != NULL))
		{
			MhegMgrLogAvStream(MHEG_MGR_STREAMING_AV_DEBUG, ("Play and Seek -> May be go to the Seeked Play !\n"));
			s_MHEGContext.context[ulActionId].bPlayAndSeek = TRUE;
			return ERR_FAIL;
		}
	}
#endif

	/* get current AV Pes information from Context */
	local_mheg_GetContextMastActionId(&ulActionId);
	if (local_mheg_GetContextCurPesInfo(ulActionId, &usCurVideoPid, &usCurAudioPid, &usCurPcrPid, &ucCurVideoType, &ucCurAudioType) != ERR_OK)
	{
		MhegMgrLogAssert(("Error : get current PID \n"));
	}

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (C_CODE_F_WHITE_B_BLUE" [CurPES] V/A/PCR PID(0x%x/0x%x/0x%x) VideoType(0x%x) AudioType(0x%x)"C_CODE_RESET"\n",
					usCurVideoPid, usCurAudioPid, usCurPcrPid, ucCurVideoType, ucCurAudioType));
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("[AvStream(%p)] V/A/PCR PID(0x%x/0x%x/0x%x) V/A Component(0x%lx/0x%lx) V/A Status(%d/%d) V/A type(%d/%d) mediaStatus(0x%x) eStreamRef(%d)\n",
					pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus,
					pstStreamHandler->videoType, pstStreamHandler->audioType, pstStreamHandler->mediaStatus, pstStreamHandler->eStreamRef));
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("avMode(%d)\n", pstStreamHandler->avMode));
#endif

	if((pstStreamHandler->mediaStatus & 0x0F) != ITK_DISPLAY_AV_STATUS_PLAYING)
	{
		if (pstStreamHandler->videoComponent == 0 && pstStreamHandler->audioComponent == 0) // select video/audio 없이 play 되는 경우 (section6 : 6.6, 6.8.2B, 6.8.2C)
		{
			if (usCurVideoPid != pstStreamHandler->videoPid	|| usCurAudioPid != pstStreamHandler->audioPid)
			{
				pstStreamHandler->videoComponent = -1;
				pstStreamHandler->audioComponent = -1;
			}
		}

		/* set video pid */
		if (pstStreamHandler->videoComponent)
		{
			if((usCurVideoPid != pstStreamHandler->videoPid) ||
				(pstStreamHandler->videoComponent == -1) ||
				((pstStreamHandler->videoComponent) && (usCurVideoPid == pstStreamHandler->videoPid) && (pstStreamHandler->videoStatus == FALSE || (pstStreamHandler->videoStatus == TRUE && (pstStreamHandler->mediaStatus & ITK_DISPLAY_AV_STATUS_PRELOAD)))))
			{
				usVideoPid = pstStreamHandler->videoPid;
				usVideoType = pstStreamHandler->videoType;
				if (usVideoPid != PID_NULL)
				{
					pstStreamHandler->videoStatus = TRUE; // refer to "Michael Kuegler [michael.kuegler@s-and-t.com]" comment as below
				}
			}
		}

		/* set audio pid */
		if (pstStreamHandler->audioComponent)
		{
			if((usCurAudioPid != pstStreamHandler->audioPid) ||
				(pstStreamHandler->audioComponent == -1) ||
				((pstStreamHandler->audioComponent) && (usCurAudioPid == pstStreamHandler->audioPid) && (pstStreamHandler->audioStatus == FALSE || (pstStreamHandler->audioStatus == TRUE && (pstStreamHandler->mediaStatus & ITK_DISPLAY_AV_STATUS_PRELOAD)))))
			{
				usAudioPid = pstStreamHandler->audioPid;
				usAudioType = pstStreamHandler->audioType;
				if (usAudioPid != PID_NULL)
				{
					pstStreamHandler->audioStatus = TRUE; // refer to "Michael Kuegler [michael.kuegler@s-and-t.com]" comment as below
				}

			}
		}

		/* set pcr pid */
		if (pstStreamHandler->pcrPid != PID_NULL && pstStreamHandler->pcrPid != 0)
		{
			ulPcrPid = pstStreamHandler->pcrPid;
		}
		else
		{
			if (pstStreamHandler->videoPid != PID_NULL)
			{
				ulPcrPid = pstStreamHandler->videoPid;
			}
			else if (usCurPcrPid != PID_NULL && usCurPcrPid != 0)
			{
				ulPcrPid = usCurPcrPid;
			}
			else
			{
				ulPcrPid = pstStreamHandler->audioPid;
			}
		}

		if(usVideoPid == PID_CURRENT && usAudioPid == PID_CURRENT)
		{
			/* in case there is no change in AV component --> skip */
			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tSkip AV Play!! [Cur] V/A PID (0x%x/0x%x) [Stream handler] V/A PID(0x%x/0x%x)\n",
								usCurVideoPid, usCurAudioPid, pstStreamHandler->videoPid, pstStreamHandler->audioPid));
			/* If audio, video and pcr pid is zero, return as ERR_OK without setting callback fn to play AV -- mhoh*/
			//return ERR_OK;
		}
		else
		{
			if (pstStreamHandler->videoComponent == 0 && pstStreamHandler->audioComponent == 0)
			{
				if (usVideoPid == PID_CURRENT)
				{
					usVideoPid = usCurVideoPid;
					usVideoType = ucCurVideoType;
				}
				if (usAudioPid == PID_CURRENT)
				{
					usAudioPid = usCurAudioPid;
					usAudioType = ucCurAudioType;
				}
			}

			/* change av track */
			if (usVideoPid != PID_NULL && usVideoPid != PID_CURRENT)
			{
				if (s_MHEGContext.context[ulActionId].imageDisplayFlag == TRUE)
				{
					PORT_ITK_Display_StopIFrameByOther(hAction);
				}
			}

			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tPlay stream : change pid --> hAction(0x%x), av,pcrPID(0x%x / 0x%x / 0x%x) av param(0x%x / 0x%x)\n",
							hAction, usAudioPid, usVideoPid, ulPcrPid, (usAudioType << 16)|usAudioPid, (usVideoType << 16)|usVideoPid));

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
			{
				local_mheg_Display_AVS_playStreamingAV(pstStreamHandler->hAction, pstStreamHandler->reference, ulStartTime);
			}
			else
			{
				local_mheg_ChangeAv(hAction, usVideoType, usVideoPid, usAudioType, usAudioPid, ulPcrPid);
			}
#else
			local_mheg_ChangeAv(hAction, usVideoType, usVideoPid, usAudioType, usAudioPid, ulPcrPid);
#endif
			/* notify av track changed to ITK */
			local_mheg_SetContextCurPesInfo(ulActionId, usVideoPid, usAudioPid, ulPcrPid, pstStreamHandler->videoType, pstStreamHandler->audioType);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			local_mheg_Display_AVS_setContextAvMode(ulActionId, pstStreamHandler->avMode);
#endif
		}

		if (usVideoPid != PID_NULL && usVideoPid != PID_CURRENT)
		{
			local_mheg_Display_AVS_setCurHandle_V((void*)pstStreamHandler);
		}
		if (usAudioPid != PID_NULL && usAudioPid != PID_CURRENT)
		{
			local_mheg_Display_AVS_setCurHandle_A((void*)pstStreamHandler);
		}

		if (pstStreamHandler->eStreamRef < eITK_REFSVC_FSI)
		{
			local_mheg_SetContextCurSvcHandle(ulActionId, pstStreamHandler->svcHandle);
		}
	}

	VK_TASK_Sleep(0);	/* for rescheduling OS */

	if (pstStreamHandler->videoStatus == TRUE || pstStreamHandler->audioStatus == TRUE)
	{
		pstStreamHandler->mediaStatus = (pstStreamHandler->mediaStatus & 0xF0) | ITK_DISPLAY_AV_STATUS_PLAYING;
		local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_PLAYING, NULL);
	}
#if 0 // from Michael Kuegler [michael.kuegler@s-and-t.com]
	else
	{
		/* from Michael Kuegler [michael.kuegler@s-and-t.com]
		While investigating the outstanding issues with test NDT004, scenes 1, 4 and 8 we have come to the conclusion that the current description/implementation of itk_media_event_t events is not sufficient. In particular, the way these events are currently generated does not consider whether the selected elementary components are actually present in the referenced stream or not, which is causing a problem.
		The current documentation states that the host should generate an ITK_MEDIA_PLAYING event in response to itk_avstream_t::play(). Similarly, an ITK_MEDIA_STOPPED event is generated in response to itk_avstream_t::stop().
		This behaviour will need to be changed in order to reflect the actual content presentation, i.e. whether the components selected via itk_avstream_t::selectXXXComponent() are present in the stream. In other words, if the MHEG engine calls itk_avstream_t::play(), but _none_ of the currently selected components can be found in the PMT, the host should _not_ generate the ITK_MEDIA_PLAYING event. Instead, the host should generate an ITK_MEDIA_STOPPED event.
		Only if at least one of the components can be identified in the PMT (either audio or video) then the host should generate an ITK_MEDIA_PLAYING event.
		The current use of ITK_MEDIA_STOPPED events remains valid: they should be generated in response to itk_avstream_t::stop() or when presentation stops at a later point, i.e. if all components disappear from the PMT.
		*/

		ITK_AvstreamHandle_t *prevAVS_V = (ITK_AvstreamHandle_t *)local_mheg_Display_AVS_getCurHandle_V();
		ITK_AvstreamHandle_t *prevAVS_A = (ITK_AvstreamHandle_t *)local_mheg_Display_AVS_getCurHandle_A();

		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("Currently selected components(%p) is not present(eStreamRef:0x%x), so stop previous avstream (V/A: %p, %p)!!!!!\n", pstStreamHandler, pstStreamHandler->eStreamRef, prevAVS_V, prevAVS_A));

		if ((prevAVS_V != NULL) && (prevAVS_V == prevAVS_A))
		{
			local_mheg_Display_AVS_stopStream(prevAVS_V, 0);
		}
		else
		{
			if (prevAVS_V != NULL)
			{
				local_mheg_Display_AVS_stopStream(prevAVS_V, 0);
			}

			if (prevAVS_A != NULL)
			{
				local_mheg_Display_AVS_stopStream(prevAVS_A, 0);
			}

		}

		pstStreamHandler->mediaStatus = (pstStreamHandler->mediaStatus & 0xF0) | ITK_DISPLAY_AV_STATUS_STOPPED;
		local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_STOPPED, NULL);
	}
#endif
	return ulErr;
}


static HERROR local_mheg_Display_AVS_stopStream(ITK_AvstreamHandle_t* pstStreamHandler, HUINT8 ucIsTrackOption)
{
	HERROR		ulErr = ERR_OK;
	Handle_t	hAction = pstStreamHandler->hAction;

	(void)ucIsTrackOption;		/* unused variable temprarly */

	MhegMgrLogFatal(pstStreamHandler != NULL);

	if(SVC_PIPE_IsActionHandleLatest(hAction) != ERR_OK)
	{
		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG,  (" Old ActionHandle(0x%x) --> dump\n", hAction));
		return ERR_FAIL;
	}

	/* check media status */
	if(((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING) || ((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_CREATED))
	{
		HUINT16 	usCurVideoPid = PID_NULL, usCurAudioPid = PID_NULL, usCurPcrPid = PID_NULL;
		HUINT8		ucCurVideoType = eDxVIDEO_CODEC_UNKNOWN, ucCurAudioType = eDxAUDIO_CODEC_UNKNOWN;
		HUINT16		usVideoPid = PID_CURRENT, usAudioPid = PID_CURRENT, usPcrPid = PID_CURRENT;
		HUINT32 	ulActionId = 0;
		ITK_AvstreamHandle_t *prevAVS_V = (ITK_AvstreamHandle_t *)local_mheg_Display_AVS_getCurHandle_V();
		ITK_AvstreamHandle_t *prevAVS_A = (ITK_AvstreamHandle_t *)local_mheg_Display_AVS_getCurHandle_A();

		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStream handler = %p, prevAVS_A/V (%p, %p)\n", pstStreamHandler, prevAVS_A, prevAVS_V));

		local_mheg_GetContextMastActionId(&ulActionId);
		if (local_mheg_GetContextCurPesInfo(ulActionId, &usCurVideoPid, &usCurAudioPid, &usCurPcrPid, &ucCurVideoType, &ucCurAudioType) != ERR_OK)
		{
			MhegMgrLogAssert(("Error : get current PID \n"));
		}

		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (C_CODE_F_WHITE_B_BLUE" [CurPES] V/A/PCR PID(0x%x/0x%x/0x%x) VideoType(0x%x) AudioType(0x%x)"C_CODE_RESET"\n",
						usCurVideoPid, usCurAudioPid, usCurPcrPid, ucCurVideoType, ucCurAudioType));
		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("[AvStream(%p)] V/A/PCR PID(0x%x/0x%x/0x%x) V/A Component(0x%lx/0x%lx) V/A Status(%d/%d) V/A type(%d/%d) mediaStatus(0x%x) eStreamRef(%d)\n",
						pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus,
						pstStreamHandler->videoType, pstStreamHandler->audioType, pstStreamHandler->mediaStatus, pstStreamHandler->eStreamRef));
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("avMode(%d)\n", pstStreamHandler->avMode));
#endif

		if((pstStreamHandler == prevAVS_V) || ((prevAVS_V == NULL) && (pstStreamHandler == prevAVS_A)))
		{
			if (pstStreamHandler->bVideoFreeze == TRUE || pstStreamHandler->bAudioFreeze == TRUE)
			{
				SVC_AV_SetVideoFreeze(ulActionId, FALSE);
				SVC_AV_SetAudioFreeze(ulActionId, FALSE);
				pstStreamHandler->bVideoFreeze = FALSE;
				pstStreamHandler->bAudioFreeze = FALSE;
				VK_TASK_Sleep(0);
			}
		}

		if (prevAVS_V == prevAVS_A)
		{
			if (((pstStreamHandler->videoPid == usCurVideoPid || pstStreamHandler->audioPid == usCurAudioPid) && (prevAVS_V == pstStreamHandler)) || !prevAVS_V)
			{
				/* change av track */
				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStop stream : change pid --> hAction(0x%x), videoPid(0x%x) audioPid(0x%x), pcrPid(0x%x)\n", hAction, PID_NULL, PID_NULL, PID_NULL));

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
				{
					if (local_mheg_Display_AVS_stopStreamingAV(hAction) == ERR_OK)
					{
						pstStreamHandler->position = 0;
					}
				}
				else
				{
					local_mheg_ChangeAv(hAction, CODEC_CURRENT, PID_NULL, CODEC_CURRENT, PID_NULL, PID_NULL);
				}
#else
				local_mheg_ChangeAv(hAction, CODEC_CURRENT, PID_NULL, CODEC_CURRENT, PID_NULL, PID_NULL);
#endif
				pstStreamHandler->videoStatus = FALSE;
				pstStreamHandler->audioStatus = FALSE;
				ulErr = ERR_OK;
			}
		}
		else
		{
			if ((pstStreamHandler->videoPid == usCurVideoPid) && (!prevAVS_V || prevAVS_V == pstStreamHandler))
			{
				/* change av track */
				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStop stream Video: change pid --> hAction(0x%x), videoPid(0x%x) audioPid(0x%x), pcrPid(0x%x)\n", hAction, PID_NULL, usAudioPid, usPcrPid));

				usVideoPid = PID_NULL;
				pstStreamHandler->videoStatus = FALSE;
				ulErr = ERR_OK;
			}

			if ((pstStreamHandler->audioPid == usCurAudioPid) && (!prevAVS_A || prevAVS_A == pstStreamHandler))
			{
				/* change av track */
				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStop stream Audio: change pid --> hAction(0x%x), videoPid(0x%x) audioPid(0x%x), pcrPid(0x%x)\n", hAction, usVideoPid, PID_NULL, usPcrPid));
				usAudioPid = PID_NULL;
				pstStreamHandler->audioStatus = FALSE;
				ulErr = ERR_OK;
			}

			if (usVideoPid == PID_NULL && usAudioPid == PID_NULL)
			{
				if (pstStreamHandler->pcrPid == usCurPcrPid)
				{
					usPcrPid = PID_NULL;
				}
			}

			if (usVideoPid == PID_NULL || usAudioPid == PID_NULL)
			{
				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStop stream : change pid --> hAction(0x%x), av pcrPID(0x%x / 0x%x / 0x%x) av param(0x%x / 0x%x\n",
								hAction, usAudioPid, usVideoPid, usPcrPid, (CODEC_CURRENT << 16)|usAudioPid, (CODEC_CURRENT << 16)|usVideoPid));
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
				{
					if (local_mheg_Display_AVS_stopStreamingAV(hAction) == ERR_OK)
					{
						pstStreamHandler->position = 0;
					}
				}
				else
				{
					local_mheg_ChangeAv(hAction, CODEC_CURRENT, usVideoPid, CODEC_CURRENT, usAudioPid, usPcrPid);
				}
#else
				local_mheg_ChangeAv(hAction, CODEC_CURRENT, usVideoPid, CODEC_CURRENT, usAudioPid, usPcrPid);
#endif
			}
		}
	}
	else
	{
		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tERROR : Can not stop AV Stream, mediaStatus(0x%x)\n", pstStreamHandler->mediaStatus));
		ulErr = ERR_FAIL;
	}

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	if(pstStreamHandler->avMode == MHEG_AV_MODE_BROADCAST)
#endif
	{
		pstStreamHandler->mediaStatus = ITK_DISPLAY_AV_STATUS_STOPPED;
		local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_STOPPED, NULL);
	}

	return ulErr;
}

#if 0
static void local_mheg_Display_AVS_selectAudioComponent(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t component)
{
	HUINT32		ulActionId;
	HUINT16		usSelAudioPid = PID_NULL;
	HUINT8		ucSelAudioType = eDxAUDIO_CODEC_UNKNOWN;
	HUINT8		ucStreamType;
	Handle_t	hDefSvc, hCurSvc;
	HUINT16		usCurVideoPid = PID_NULL, usCurAudioPid = PID_NULL, usCurPcrPid = PID_NULL;
	HUINT8		ucCurVideoType = eDxVIDEO_CODEC_UNKNOWN, ucCurAudioType = eDxAUDIO_CODEC_UNKNOWN, ucPcrType;
	HUINT16		usUniqId;
	HERROR		lResult;
	DxStereoSelect_e	eSoundMode = eDxSTEREO_SELECT_UNKNOWN;

	/* get current Audio information from MHEG context */
	local_mheg_GetContextMastActionId(&ulActionId);
	local_mheg_GetContextSvcHandle(ulActionId, &hDefSvc, &hCurSvc);
	DB_SVC_GetUniqueIdFromSvcHandle(hCurSvc, &usUniqId);
	//PORT_ITK_GetContextCurPesInfo(ulActionId, &usCurVideoPid, &usCurAudioPid, &usCurPcrPid, &ucCurVideoType, &ucCurAudioType);

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (" Select Audio component (0x%lx), hDefSvc (0x%x), hCurSvc (0x%x), usUniqId (0x%x)\n", component, hDefSvc, hCurSvc, usUniqId));
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (" Stream handler (%p) -> videoPid (0x%x), audioPid (0x%x), pcrPid (0x%x), videoComponent (0x%lx), audioComponent (0x%lx), videoStatus (%d), audioStatus (%d)\n",
						pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus));
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (" Stream handler -> mediaStatus (0x%x), eStreamRef (%d)\n", pstStreamHandler->mediaStatus | ITK_DISPLAY_AV_STATUS_A_SELECTED, pstStreamHandler->eStreamRef));

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	if (local_mheg_Display_AVS_isSettingStreamingAV() == TRUE)
	{
		if (PORT_ITK_AVS_GetHandle(pstStreamHandler) == NULL)
		{
			MhegMgrLogAssert(("Already released !\n"));
			return;
		}
	}
#endif

	pstStreamHandler->mediaStatus |= ITK_DISPLAY_AV_STATUS_A_SELECTED;
	if(pstStreamHandler->audioComponent != component)
	{
		if (pstStreamHandler->eStreamRef == eITK_REFSVC_CUR && component == -1)
		{
			if (local_mheg_GetContextCurPesInfo(ulActionId, &usCurVideoPid, &usCurAudioPid, &usCurPcrPid, &ucCurVideoType, &ucCurAudioType) == ERR_OK)
			{
				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStream handler -> audioPid (0x%x), Cur -> AudioPid (0x%x)\n", pstStreamHandler->audioPid, usCurAudioPid));
				usSelAudioPid = usCurAudioPid;
			}
		}
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		else if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI)
			//&& component == -1) // ?
		{
			SvcMp_AudioTrack_t stAudioTrack;

			lResult = SVC_MP_GetActiveAudioTrack(s_MHEGContext.context[ulActionId].player, &stAudioTrack);
			if (lResult == ERR_OK)
			{
#if 0
				local_mheg_Debug_Print_PlayInfo(&stMediaPlayInfo);
#endif
				// set PID
				usSelAudioPid = stAudioTrack.ulAudioStreamId;
				if (usSelAudioPid == PID_NULL) // video PID
				{
					ucSelAudioType = eDxAUDIO_CODEC_UNKNOWN; // default
				}
				else
				{
					ucSelAudioType = stAudioTrack.eAudioCodec;
				}
			}

			MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("Streaming Av -> audioPid (0x%x), Type (0x%x)\n", pstStreamHandler->audioPid, pstStreamHandler->audioType));
		}
#endif
		else
		{
			/* get audio component from PMT Raw data */
			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStream handler -> serviceId (0x%x)\n", pstStreamHandler->serviceId));

			if (s_MHEGContext.itkProfile == REDKEY_PROFILE_CIPLUS)
			{
				PmtInfo_t pmtInfo;

				lResult = SVC_SI_SVC_UpdatePmtInfo (ulActionId, usUniqId, pstStreamHandler->serviceId, &pmtInfo);
				if ( lResult == ERR_SI_RESULT_OK )
				{
					HINT32 nSelectIndex = (component == -1) ? 0 : component;

					MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG,("\t Select Index (%d / %d)\n", nSelectIndex, pmtInfo.usAudioNum));

					if (nSelectIndex < pmtInfo.usAudioNum)
					{
#if defined (CONFIG_DEBUG)
						HINT32 i;
						MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG,("\t[PMT info +]\n"));
						for ( i = 0; i < pmtInfo.usAudioNum; i++ )
						{
							MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\t\tAudio PID = (%d) : 0x%x\n", pmtInfo.stAudioInfo[i].usPid, pmtInfo.stAudioInfo[i].usPid));
							MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\t\tAudio Codec = (%d)\n", pmtInfo.stAudioInfo[i].eAudCodec));
							MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\t\tAudio Component Tag = (%d)\n", pmtInfo.stAudioInfo[i].ucComponentTag));
							MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\t\tAudio bCaDesFound = (%d)\n\n", pmtInfo.stAudioInfo[i].bCaDesFound));
						}
#endif
						usSelAudioPid = pmtInfo.stAudioInfo[nSelectIndex].usPid;
						ucSelAudioType =pmtInfo.stAudioInfo[nSelectIndex].eAudCodec;
					}
					else
					{
						MhegMgrLogError(("\tdo not match component !!!\n"));
					}
				}
			}
			else
			{
				lResult = MHEG_RAW_GetComponentInfo(usUniqId, pstStreamHandler->serviceId, component, ITK_ELEMENT_INDEX_AUDIO, &usSelAudioPid, &ucSelAudioType, &ucStreamType, &eSoundMode);
				if(lResult != ERR_OK)
				{
					/* if we can not retrieve audio component , we try to find it from SVC DB */

					MhegMgrLogAssert(("\tdo not match component in Rawdata, so try to find at SVC DB\n"));

					if (component != -1)
					{
						pstStreamHandler->audioPid = usSelAudioPid;
						pstStreamHandler->audioType = eDxAUDIO_CODEC_UNKNOWN;
					}
				}

				if (pstStreamHandler->pcrPid == PID_NULL) // video에 의해서 설정되어 있으면 pcr를 설정하지 않음
				{
					lResult = MHEG_RAW_GetComponentInfo(usUniqId, pstStreamHandler->serviceId, component, ITK_ELEMENT_INDEX_PCR, &usCurPcrPid, &ucPcrType, &ucStreamType, &eSoundMode);
					if(lResult != ERR_OK)
					{
						usCurPcrPid = PID_NULL;
					}
				}
			}
		}

		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tSelect -> audioPid (0x%x), Type (0x%x)\n", usSelAudioPid, ucSelAudioType));

		if(usSelAudioPid < PID_NULL && usSelAudioPid != 0)
		{
			// Selected Audio PID가 valid 한 값이면..
			pstStreamHandler->audioPid = usSelAudioPid;
			pstStreamHandler->audioType = ucSelAudioType;
			pstStreamHandler->audioStatus = TRUE;

			if (pstStreamHandler->pcrPid == PID_NULL) // video에 의해서 설정되어 있으면 pcr를 설정하지 않음
			{
				if (usCurPcrPid == PID_NULL)
				{
					pstStreamHandler->pcrPid = usSelAudioPid;
				}
				else
				{
					pstStreamHandler->pcrPid = usCurPcrPid;
				}
			}

			/* check  if current AV Object status is 'immediate' or not */
			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStream handler -> mediaStatus (0x%x)\n", pstStreamHandler->mediaStatus));
			//if((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING)
			if ( (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_CREATED
				|| (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PRELOAD
				|| (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_STOPPED )
			{
				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tselect audio -> do not change audio pid !!!\n"));
			}
			else
			{
				/* change av track */
				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tselect Audio component : change pid --> hAction(0x%x), av pcrPID(0x%x / 0x%x / 0x%x) av param(0x%x / 0x%x\n",
								pstStreamHandler->hAction, usSelAudioPid, 0, 0, (ucSelAudioType << 16)|usSelAudioPid, PID_CURRENT));

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
				{
					local_mheg_Display_AVS_playStreamingAV(pstStreamHandler->hAction, pstStreamHandler->reference, 0);
				}
				else
				{
					local_mheg_ChangeAv(pstStreamHandler->hAction, CODEC_CURRENT, PID_CURRENT, ucSelAudioType, usSelAudioPid, pstStreamHandler->pcrPid);
				}
#else
				local_mheg_ChangeAv(pstStreamHandler->hAction, CODEC_CURRENT, PID_CURRENT, ucSelAudioType, usSelAudioPid, pstStreamHandler->pcrPid);
#endif
				SVC_AV_SetStereoSelect(ulActionId, eSoundMode);

				/* notify av track changed to ITK */
				s_MHEGContext.context[ulActionId].curAudioPid = usSelAudioPid;
				s_MHEGContext.context[ulActionId].curAudioType = ucSelAudioType;
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				local_mheg_Display_AVS_setContextAvMode(ulActionId, pstStreamHandler->avMode);
#endif
				local_mheg_Display_AVS_setCurHandle_A((void*)pstStreamHandler);

				local_mheg_SetContextCurSvcHandle(ulActionId, pstStreamHandler->svcHandle);
			}
		}
		else
		{
			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tinvalid Audio Pid from SvcHandle(0x%x) pstStreamHandler->audioPid(0x%x)\n", hCurSvc, pstStreamHandler->audioPid));
		}

		pstStreamHandler->audioComponent = component;
	}
	else
		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tsame component(%d) --> ignored\n", (int)component));
}

static void local_mheg_Display_AVS_selectVideoComponent(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t component)
{
	HUINT32		ulActionId;
	HUINT16		usSelVideoPid = PID_NULL;
	HUINT8		ucSelVideoType = 0;
	HUINT8		ucStreamType;
	Handle_t	hDefSvc, hCurSvc;
	HUINT16		usCurVideoPid = PID_NULL, usCurAudioPid = PID_NULL, usCurPcrPid = PID_NULL;
	HUINT8		ucCurVideoType = eDxVIDEO_CODEC_UNKNOWN, ucCurAudioType = eDxAUDIO_CODEC_UNKNOWN, ucPcrType;
	HUINT16		usUniqId;
	HERROR		lResult;
	DxStereoSelect_e		eSoundMode;

	/* get current Audio information from MHEG context */
	local_mheg_GetContextMastActionId(&ulActionId);
	local_mheg_GetContextSvcHandle(ulActionId, &hDefSvc, &hCurSvc);
	DB_SVC_GetUniqueIdFromSvcHandle(hCurSvc, &usUniqId);
	//PORT_ITK_GetContextCurPesInfo(ulActionId, &usCurVideoPid, &usCurAudioPid, &usCurPcrPid, &ucCurVideoType, &ucCurAudioType);

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (" Select Video component (0x%lx), hDefSvc (0x%x), hCurSvc (0x%x), usUniqId (0x%x)\n", component, hDefSvc, hCurSvc, usUniqId));
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (" Stream handler (%p) -> videoPid (0x%x), audioPid (0x%x), pcrPid (0x%x), videoComponent (0x%lx), audioComponent (0x%lx), videoStatus (%d), audioStatus (%d)\n",
						pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus));
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (" Stream handler -> mediaStatus (0x%x), eStreamRef (%d)\n", pstStreamHandler->mediaStatus | ITK_DISPLAY_AV_STATUS_V_SELECTED, pstStreamHandler->eStreamRef));

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	if (local_mheg_Display_AVS_isSettingStreamingAV() == TRUE)
	{
		if (PORT_ITK_AVS_GetHandle(pstStreamHandler) == NULL)
		{
			MhegMgrLogAssert(("Already released !\n"));
			return;
		}
	}
#endif

	pstStreamHandler->mediaStatus |= ITK_DISPLAY_AV_STATUS_V_SELECTED;
	if(pstStreamHandler->videoComponent != component)
	{
		if (pstStreamHandler->eStreamRef == eITK_REFSVC_CUR && component == -1)
		{
			if (local_mheg_GetContextCurPesInfo(ulActionId, &usCurVideoPid, &usCurAudioPid, &usCurPcrPid, &ucCurVideoType, &ucCurAudioType) == ERR_OK)
			{
				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStream handler -> videoPid (0x%x), Cur -> VideoPid (0x%x)\n", pstStreamHandler->videoPid, usCurVideoPid));
				usSelVideoPid = usCurVideoPid;
			}
		}
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		else if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI)
			//&& component == -1) // ?
		{

			SvcMp_VideoTrack_t stVideoTrack;

			lResult = SVC_MP_GetActiveVideoTrack(s_MHEGContext.context[ulActionId].player, &stVideoTrack);
			if (lResult == ERR_OK)
			{
#if 0
				local_mheg_Debug_Print_PlayInfo(&stMediaPlayInfo);
#endif
				// set PID
				usSelVideoPid = stVideoTrack.ulVideoStreamId;
				//usPcrPid = stMediaPlayInfo.pcr_pid;
				if (usSelVideoPid == PID_NULL) // video PID
				{
					ucSelVideoType = eDxVIDEO_CODEC_MPEG2; // default
				}
				else
				{
					ucSelVideoType = stVideoTrack.eVideoCodec;
				}
			}

			MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("Streaming Av -> videoPid (0x%x), pcrPid (0x%x), Type (0x%x)\n", usSelVideoPid, usCurPcrPid, ucSelVideoType));
		}
#endif
		else
		{
			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStream handler -> serviceId (0x%x)\n", pstStreamHandler->serviceId));

			if (s_MHEGContext.itkProfile == REDKEY_PROFILE_CIPLUS)
			{
				PmtInfo_t pmtInfo;

				lResult = SVC_SI_SVC_UpdatePmtInfo (ulActionId, usUniqId, pstStreamHandler->serviceId, &pmtInfo);
				if ( lResult == ERR_SI_RESULT_OK )
				{
					HINT32 nSelectIndex = (component == -1) ? 0 : component;

					MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG,("\t Select Index (%d / %d)\n", nSelectIndex, pmtInfo.usVideoNum));

					if (nSelectIndex < pmtInfo.usVideoNum)
					{
#if defined (CONFIG_DEBUG)
						HINT32 i;
						MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG,("\t[PMT info +]\n"));
						for ( i = 0; i < pmtInfo.usVideoNum; i++ )
						{
							MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\t\tVideo PID = (%d) : 0x%x\n", pmtInfo.stVideoInfo[i].usPid, pmtInfo.stVideoInfo[i].usPid));
							MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\t\tVideo Codec = (%d)\n", pmtInfo.stVideoInfo[i].eVideoCodec));
							MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\t\tVideo Component Tag = (%d)\n", pmtInfo.stVideoInfo[i].ucComponentTag));
							MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\t\tVideo bCaDesFound = (%d)\n\n", pmtInfo.stVideoInfo[i].bCaDesFound));
						}
#endif
						usSelVideoPid = pmtInfo.stVideoInfo[nSelectIndex].usPid;
						ucSelVideoType =pmtInfo.stVideoInfo[nSelectIndex].eVideoCodec;
					}
					else
					{
						MhegMgrLogError(("\tdo not match component !!!\n"));
					}
				}
			}
			else
			{
				lResult = MHEG_RAW_GetComponentInfo(usUniqId, pstStreamHandler->serviceId, component, ITK_ELEMENT_INDEX_VIDEO, &usSelVideoPid, &ucSelVideoType, &ucStreamType, &eSoundMode);
				if(lResult != ERR_OK)
				{
					/* if we can not retrieve video component , we try to find it from SVC DB */
					MhegMgrLogAssert(("\tdo not match component in Rawdata, so try to find at SVC DB\n"));

					if (component != -1)
					{
						pstStreamHandler->videoPid = usSelVideoPid;
						pstStreamHandler->videoType = eDxVIDEO_CODEC_UNKNOWN;
					}
				}
				else
				{
					if (ucStreamType == ISO_PRIVDATA_2)
					{
						DbSvc_Info_t 	stSvcInfo= {0, };

						/* DB의 video type을 보고 SD인지 HD인지 결정하여 codec 설정 */
						lResult = DB_SVC_GetServiceInfo(pstStreamHandler->svcHandle, &stSvcInfo);
						if (lResult != ERR_OK)
						{
							MhegMgrLogAssert((" Ooops!!! fail to retrieve SvcInfo By hSvc(0x%x)\n", pstStreamHandler->svcHandle));
							/* Set to default */
							ucSelVideoType = eDxVIDEO_CODEC_MPEG2;
						}
						else
						{
							// 해당 point 에 대한 수정이 필요함 -> SD H.264 service에 대한 고려가 없음
	#if 0
							if( 0 != (stSvcInfo.eUserFlag1 & eSvcUserFlag_27_reserved) )
							{
								ucSelVideoType = eDxVIDEO_CODEC_H264;
							}
							else
							{
								ucSelVideoType = eDxVIDEO_CODEC_MPEG2;
							}
	#else
							if (stSvcInfo.eVideoType == eVideoType_Hd)
							{
								ucSelVideoType = eDxVIDEO_CODEC_H264;
							}
							else
							{
								ucSelVideoType = eDxVIDEO_CODEC_MPEG2;
							}
	#endif
						}
					}
				}

				lResult = MHEG_RAW_GetComponentInfo(usUniqId, pstStreamHandler->serviceId, component, ITK_ELEMENT_INDEX_PCR, &usCurPcrPid, &ucPcrType, &ucStreamType, &eSoundMode);
				if(lResult != ERR_OK)
				{
					usCurPcrPid = PID_NULL;
				}
			}
		}

		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tSelect -> videoPid (0x%x), Type (0x%x)\n", usSelVideoPid, ucSelVideoType));

		if(usSelVideoPid < PID_NULL && usSelVideoPid != 0)
		{
			// Selected Video PID가 valid 한 값이면..
			pstStreamHandler->videoPid = usSelVideoPid;
			pstStreamHandler->videoType = ucSelVideoType;
			if (usCurPcrPid == PID_NULL)
			{
				pstStreamHandler->pcrPid = usSelVideoPid;
			}
			else
			{
				pstStreamHandler->pcrPid = usCurPcrPid;
			}

			pstStreamHandler->videoStatus = TRUE;

			/* check if current AV Object status is 'immediate' or not */
			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tStream handler -> mediaStatus (0x%x)\n", pstStreamHandler->mediaStatus));
//			if((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING)
			if ( (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_CREATED
				|| (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PRELOAD
				|| (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_STOPPED )
			{
				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tselect video -> do not change video pid !!!\n"));
			}
			else
			{
				if (s_MHEGContext.context[ulActionId].imageDisplayFlag == TRUE)
				{
					/* change av track */
					PORT_ITK_Display_StopIFrameByOther(pstStreamHandler->hAction);
				}

				MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tselect Video component : change pid --> hAction(0x%x), av pcrPID(0x%x / 0x%x / 0x%x) av param(0x%x / 0x%x)\n",
								pstStreamHandler->hAction, 0, usSelVideoPid, pstStreamHandler->pcrPid, (ucSelVideoType << 16)|usSelVideoPid, PID_CURRENT));

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
				{
					local_mheg_Display_AVS_playStreamingAV(pstStreamHandler->hAction, pstStreamHandler->reference, 0);
				}
				else
				{
					local_mheg_ChangeAv(pstStreamHandler->hAction, ucSelVideoType, usSelVideoPid, CODEC_CURRENT, PID_CURRENT, pstStreamHandler->pcrPid);
				}
#else
				local_mheg_ChangeAv(pstStreamHandler->hAction, ucSelVideoType, usSelVideoPid, CODEC_CURRENT, PID_CURRENT, pstStreamHandler->pcrPid);
#endif
				/* notify av track changed to ITK */
				s_MHEGContext.context[ulActionId].curVideoPid = usSelVideoPid;
				s_MHEGContext.context[ulActionId].curVideoType = ucSelVideoType;
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				local_mheg_Display_AVS_setContextAvMode(ulActionId, pstStreamHandler->avMode);
#endif
				local_mheg_Display_AVS_setCurHandle_V((void*)pstStreamHandler);

				local_mheg_SetContextCurSvcHandle(ulActionId, pstStreamHandler->svcHandle);
			}
		}
		else
		{
			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tinvalid video Pid from SvcHandle(0x%x) pstStreamHandler->videoPid(0x%x)\n", (unsigned int)hCurSvc, pstStreamHandler->videoPid));
		}

		pstStreamHandler->videoComponent = component;
	}
	else
		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("\tsame video component(%ld) -> ignored\n",component));

}
#endif

static void local_mheg_Display_AVS_deselectAudioComponent(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t component)
{
	HUINT16		usCurVideoPid = PID_NULL, usCurAudioPid = PID_NULL, usCurPcrPid = PID_NULL;
	HUINT8		ucCurVideoType = eDxVIDEO_CODEC_UNKNOWN, ucCurAudioType = eDxAUDIO_CODEC_UNKNOWN;
	HUINT16		usPcrPid = PID_CURRENT;
	HUINT32		ulActionId;
	HBOOL		bChangePid = FALSE;

	local_mheg_GetContextMastActionId(&ulActionId);
	if (local_mheg_GetContextCurPesInfo(ulActionId, &usCurVideoPid, &usCurAudioPid, &usCurPcrPid, &ucCurVideoType, &ucCurAudioType) != ERR_OK)
	{
		MhegMgrLogAssert(("Error : get current PID \n"));
	}

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("Audio component (0x%lx)\n", component));
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (C_CODE_F_WHITE_B_BLUE" [CurPES] V/A/PCR PID(0x%x/0x%x/0x%x) VideoType(0x%x) AudioType(0x%x)"C_CODE_RESET"\n",
					usCurVideoPid, usCurAudioPid, usCurPcrPid, ucCurVideoType, ucCurAudioType));
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("[AvStream(%p)] V/A/PCR PID(0x%x/0x%x/0x%x) V/A Component(0x%lx/0x%lx) V/A Status(%d/%d) V/A type(%d/%d) mediaStatus(0x%x) eStreamRef(%d)\n",
					pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus,
					pstStreamHandler->videoType, pstStreamHandler->audioType, pstStreamHandler->mediaStatus, pstStreamHandler->eStreamRef));
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("avMode(%d)\n", pstStreamHandler->avMode));
#endif

	if( ( ((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING)
		&& ((pstStreamHandler->mediaStatus & 0xF0) & ITK_DISPLAY_AV_STATUS_A_SELECTED) )
		|| ((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_CREATED) )
	{
		if (pstStreamHandler->audioComponent
			|| ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_CREATED) )
		{
			if (pstStreamHandler->audioPid == usCurAudioPid)
			{
				bChangePid = TRUE;
			}
		}

		if (bChangePid == TRUE)
		{
			pstStreamHandler->audioStatus = FALSE;
			if (pstStreamHandler->videoStatus == FALSE)
			{
				if (pstStreamHandler->videoPid == PID_NULL)
				{
					usPcrPid = PID_NULL;
				}
			}

			MhegMgrLogTrace(("deselect audio : change pid --> hAction(0x%x), videoPid(0x%x) audioPid(0x%x), pcrPid(0x%x)\n", pstStreamHandler->hAction, (CODEC_CURRENT << 16) |PID_CURRENT, PID_NULL, usPcrPid));

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
			{
				if (local_mheg_Display_AVS_stopStreamingAV(pstStreamHandler->hAction) == ERR_OK)
				{
					pstStreamHandler->position = 0;
				}
			}
			else
			{
				local_mheg_ChangeAv(pstStreamHandler->hAction, CODEC_CURRENT, PID_CURRENT, CODEC_CURRENT, PID_NULL, usPcrPid);
			}
#else
			local_mheg_ChangeAv(pstStreamHandler->hAction, CODEC_CURRENT, PID_CURRENT, CODEC_CURRENT, PID_NULL, usPcrPid);
#endif
		}

		//pstStreamHandler->audioPid = PID_NULL;
		pstStreamHandler->audioComponent = 0;
	}

	pstStreamHandler->mediaStatus &= ~ITK_DISPLAY_AV_STATUS_A_SELECTED;

	MhegMgrLogTrace(("deselect audio component !! \r\n"));
}

static void local_mheg_Display_AVS_deselectVideoComponent(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t component)
{
	HUINT16		usCurVideoPid = PID_NULL, usCurAudioPid = PID_NULL, usCurPcrPid = PID_NULL;
	HUINT8		ucCurVideoType = eDxVIDEO_CODEC_UNKNOWN, ucCurAudioType = eDxAUDIO_CODEC_UNKNOWN;
	HUINT16		usPcrPid = PID_CURRENT;
	HUINT32		ulActionId;
	HBOOL		bChangePid = FALSE;

	local_mheg_GetContextMastActionId(&ulActionId);
	if (local_mheg_GetContextCurPesInfo(ulActionId, &usCurVideoPid, &usCurAudioPid, &usCurPcrPid, &ucCurVideoType, &ucCurAudioType) != ERR_OK)
	{
		MhegMgrLogAssert(("Error : get current PID \n"));
	}

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("Video component (0x%lx)\n", component));
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, (C_CODE_F_WHITE_B_BLUE" [CurPES] V/A/PCR PID(0x%x/0x%x/0x%x) VideoType(0x%x) AudioType(0x%x)"C_CODE_RESET"\n",
					usCurVideoPid, usCurAudioPid, usCurPcrPid, ucCurVideoType, ucCurAudioType));
	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("[AvStream(%p)] V/A/PCR PID(0x%x/0x%x/0x%x) V/A Component(0x%lx/0x%lx) V/A Status(%d/%d) V/A type(%d/%d) mediaStatus(0x%x) eStreamRef(%d)\n",
					pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus,
					pstStreamHandler->videoType, pstStreamHandler->audioType, pstStreamHandler->mediaStatus, pstStreamHandler->eStreamRef));
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("avMode(%d)\n", pstStreamHandler->avMode));
#endif

	if( ( ((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING)
		&& ((pstStreamHandler->mediaStatus & 0xF0) & ITK_DISPLAY_AV_STATUS_V_SELECTED) )
		|| ((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_CREATED) )
	{
		if (pstStreamHandler->videoComponent
			|| ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_CREATED) )
		{
			if (pstStreamHandler->videoPid == usCurVideoPid)
			{
				bChangePid = TRUE;
			}
		}

		if (bChangePid == TRUE)
		{
			pstStreamHandler->videoStatus = FALSE;
			if (pstStreamHandler->audioStatus == FALSE)
			{
				if (pstStreamHandler->audioPid == PID_NULL)
				{
					usPcrPid = PID_NULL;
				}
			}

			MhegMgrLogTrace(("deselect video : change pid --> hAction(0x%x), videoPid(0x%x) audioPid(0x%x), pcrPid(0x%x)\n", pstStreamHandler->hAction, PID_NULL, (CODEC_CURRENT << 16) | PID_CURRENT, usPcrPid));

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
			{
				if (local_mheg_Display_AVS_stopStreamingAV(pstStreamHandler->hAction) == ERR_OK)
				{
					pstStreamHandler->position = 0;
				}
			}
			else
			{
				local_mheg_ChangeAv(pstStreamHandler->hAction, CODEC_CURRENT, PID_NULL, CODEC_CURRENT, PID_CURRENT, usPcrPid);
			}
#else
			local_mheg_ChangeAv(pstStreamHandler->hAction, CODEC_CURRENT, PID_NULL, CODEC_CURRENT, PID_CURRENT, usPcrPid);
#endif
		}

		//pstStreamHandler->videoPid = PID_NULL;
		pstStreamHandler->videoComponent = 0;
	}

	pstStreamHandler->mediaStatus &= ~ITK_DISPLAY_AV_STATUS_V_SELECTED;

	MhegMgrLogTrace(("deselect video component !! \r\n"));
}

static void local_mheg_Display_AVS_setSpeed(Handle_t hAction, struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t numerator, itk_int32_t denominator)
{
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);
	HUINT8			ucSpeed = MHEG_AV_SPEED_STOP;
	HUINT8			bEnableSubtitle = TRUE;

	MhegMgrLogTrace(("[AVstream(%p)] -> avStatus(%d, %d), speed (%ld, %ld), eStreamRef (%d), s_bMHEGFreezedA/V (%d, %d)\n",
					pstStreamHandler, pstStreamHandler->audioStatus, pstStreamHandler->videoStatus, numerator, denominator, pstStreamHandler->eStreamRef,
					pstStreamHandler->bAudioFreeze, pstStreamHandler->bVideoFreeze));

	if(numerator == 0 && denominator == 1)
	{
		ucSpeed = MHEG_AV_SPEED_PAUSE;
	}
	else if (numerator == 1 && denominator == 1)
	{
		ucSpeed = MHEG_AV_SPEED_PLAY;
	}
	else if (numerator < 0 && denominator == 1)
	{
		ucSpeed = MHEG_AV_SPEED_REWIND;
	}
	else if (numerator > 1 && denominator == 1)
	{
		ucSpeed = MHEG_AV_SPEED_FAST_FWD;
	}

	MhegMgrLogTrace(("set speed (%d)\n", ucSpeed));

	/* subtitle control */
	switch(ucSpeed)
	{
	case MHEG_AV_SPEED_PLAY:
		bEnableSubtitle = TRUE;
		break;
	case MHEG_AV_SPEED_PAUSE:
	case MHEG_AV_SPEED_STOP:
	case MHEG_AV_SPEED_REWIND:
		bEnableSubtitle = FALSE;
		break;
	case MHEG_AV_SPEED_FAST_FWD:
		if ((pstStreamHandler->eStreamRef== eITK_REFSVC_HTTP) || (pstStreamHandler->eStreamRef == eITK_REFSVC_RTSP))
		{
			bEnableSubtitle = FALSE;
		}
		else
		{
			bEnableSubtitle = TRUE;
		}
		break;
	}
	local_mheg_Display_controlSubtitle(ulActionId, bEnableSubtitle);

	switch (pstStreamHandler->eStreamRef)
	{
	case eITK_REFSVC_DVB:
	case eITK_REFSVC_DEF:
	case eITK_REFSVC_LCN:
	case eITK_REFSVC_FSI:
	case eITK_REFSVC_CUR:
		{
			switch(ucSpeed)
			{
			case MHEG_AV_SPEED_PLAY:
			case MHEG_AV_SPEED_FAST_FWD:
				if (pstStreamHandler->bVideoFreeze == TRUE && pstStreamHandler->videoStatus == TRUE)
				{
					pstStreamHandler->bVideoFreeze = FALSE;
					SVC_AV_SetVideoFreeze(ulActionId, pstStreamHandler->bVideoFreeze);
				}

				if (pstStreamHandler->bAudioFreeze == TRUE && pstStreamHandler->audioStatus == TRUE)
				{
					pstStreamHandler->bAudioFreeze = FALSE;
					SVC_AV_SetAudioFreeze(ulActionId, pstStreamHandler->bAudioFreeze);
				}
				break;

			case MHEG_AV_SPEED_PAUSE:
			case MHEG_AV_SPEED_REWIND:
				if (pstStreamHandler->bVideoFreeze == FALSE && pstStreamHandler->videoStatus == TRUE)
				{
					pstStreamHandler->bVideoFreeze = TRUE;
					SVC_AV_SetVideoFreeze(ulActionId, pstStreamHandler->bVideoFreeze);
				}

				if (pstStreamHandler->bAudioFreeze == FALSE && pstStreamHandler->audioStatus == TRUE)
				{
					pstStreamHandler->bAudioFreeze = TRUE;
					SVC_AV_SetAudioFreeze(ulActionId, pstStreamHandler->bAudioFreeze);
				}
			}
		}
		break;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	case eITK_REFSVC_HTTP:
	case eITK_REFSVC_RTSP:
		{
			switch(ucSpeed)
			{
			case MHEG_AV_SPEED_PLAY:
				local_mheg_Display_AVS_resumeStreamingAV(hAction);
				break;

			case MHEG_AV_SPEED_PAUSE:
				local_mheg_Display_AVS_pauseStreamingAV(hAction);
				break;

			case MHEG_AV_SPEED_REWIND:
			case MHEG_AV_SPEED_FAST_FWD:
#if 1
				/* 현재는 rewind, fast forward 기능이 지원되지 않으므로 skip 기능으로 대체함. 추후에 동작시 #else 기능을 사용할 것 !! */
				{
					HINT32 ulTime;

					SVC_MP_GetPlayPosition(s_MHEGContext.context[ulActionId].player, (HUINT64*)&ulTime);
					if (numerator >= 0)
					{
						ulTime += numerator;
					}
					else
					{
						ulTime = ulTime < -numerator ? 0 : ulTime + numerator;
					}

					local_mheg_Display_AVS_setPosition(hAction, pstStreamHandler, (itk_int32_t)ulTime);
				}
#else
				local_mheg_Display_AVS_setSpeedStreamingAV(hAction, numerator);
#endif
				break;
			}
		}
		break;
#endif
	case eITK_REFSVC_PREV:
	case eITK_REFSVC_PVR:
	case eITK_REFSVC_UNKNOWN:
	default:
		// TODO
		MhegMgrLogAssert(("TODO : eITK_REFSVC_PREV, eITK_REFSVC_PVR !!!!!!!\n"));
		break;
	}
}
static void local_mheg_Display_AVS_showImage(Handle_t hAction, ITK_ImageHandle_t	*image, HUINT8 plane, HINT32 nByOther)
{
	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);

	if (nByOther == 1 && s_MHEGContext.context[ulActionId].hImage == NULL)
	{
		// PORT_ITK_Display_ShowIFrameByOther에 의해 iframe이 display 될 경우 redkey에 의해서 이미 iframe이 release 된 경우 crash 발생가능.
		MhegMgrLogTrace(("Skip iFrame !!!\n"));
		return;
	}

	if(PORT_ITK_GetSubMhegRunningStatus() == FALSE)
	{
		MhegMgrLogAssert(("Redkey Engine should not be call this function - ignore\r\n"));
		return;
	}

	s_MHEGContext.context[ulActionId].imageDisplayFlag = TRUE;
	s_MHEGContext.context[ulActionId].hImage = image;
	PORT_ITK_Display_StartStill(hAction, plane, image->content, image->content_size, image->format);
}

static void local_mheg_Display_AVS_stopImage(Handle_t hAction, ITK_ImageHandle_t	*image, HUINT8 plane)
{
	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);
	UNUSED(image);

	s_MHEGContext.context[ulActionId].imageDisplayFlag = FALSE;
	PORT_ITK_Display_StopStill(hAction, plane);
}

static void local_mheg_Display_AVS_releaseImage(Handle_t hAction, ITK_ImageHandle_t	*image, HUINT8 plane)
{
	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);
	UNUSED(image);

	s_MHEGContext.context[ulActionId].imageDisplayFlag = FALSE;
	s_MHEGContext.context[ulActionId].hImage = NULL;
	PORT_ITK_Display_StopStill(hAction, plane);
}

static void	*local_mheg_Display_AVS_getCurHandle_V(void)
{
	return g_pvCurAvstream_V;
}

static void	local_mheg_Display_AVS_setCurHandle_V(void *pvCurAvstream)
{
	g_pvCurAvstream_V = pvCurAvstream;
}

static void	*local_mheg_Display_AVS_getCurHandle_A(void)
{
	return g_pvCurAvstream_A;
}

static void	local_mheg_Display_AVS_setCurHandle_A(void *pvCurAvstream)
{
	g_pvCurAvstream_A = pvCurAvstream;
}

static void	*local_mheg_Display_AVS_getCurHandle(void)
{
	// If the presented audio and video components are from different services
	// the service referencing the video components takes precedence.
	return (g_pvCurAvstream_V == NULL) ? g_pvCurAvstream_A : g_pvCurAvstream_V ;
}

static void local_mheg_Display_subtitleEnable(HBOOL bEnable)
{
	// Handle_t	hAction = SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId());

	MhegMgrLogTrace((C_CODE_F_WHITE_B_RED"\tSubtitle %s"C_CODE_RESET"\n", bEnable ? "ON" : "OFF"));

	/* Subtitle / TTX */
	// TODO : < HMS1000s branch 작업내용 > CI+ v1.3의 subtitle 관련 우선순위 내용 적용해야 함
	// MW_SBTL_BlockService(hAction, !bEnable);
	HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", bEnable ? 1 : 0);
}

static void local_mheg_Display_controlSubtitle(HUINT32 ulActionId, HBOOL bEnable)
{
	MhegMgrLogTrace( ("subtitle : enable(%d), current status(%d)\n", bEnable, s_MHEGContext.bSubtitleEnable));

	if (bEnable == TRUE)
	{
#if defined(MHEG_MGR_SYNC_VIDEO_SUBTITLE)
		if ((s_MHEGContext.context[ulActionId].ucChangedVideoPid == PID_NULL)
			|| (s_MHEGContext.context[ulActionId].ucChangedVideoPid == MHEG_MGR_PID_INIT_VALUE))
		{
			MhegMgrLogAssert( ("skip subtitle control by video pid \n"));
			return;
		}
#endif
	}

	local_mheg_Display_subtitleEnable(bEnable);
}

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static HERROR local_mheg_Display_AVS_setContextAvMode(HUINT32 ulActionId, HUINT8 avMode)
{
	MhegMgrLogFatal(ulActionId < NUM_MAX_ACTION);
	MhegMgrLogTrace((" Set Context avMode(%d -> %d)\n",s_MHEGContext.context[ulActionId].avMode, 	avMode));

	if(ulActionId < NUM_MAX_ACTION)
	{
		s_MHEGContext.context[ulActionId].avMode = avMode;
	}

	PORT_ITK_SetMastMhegAvMode(avMode);

	return ERR_OK;
}

static void local_mheg_Display_AVS_setTrigger(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t trigger, itk_int32_t triggerValue)
{
	ITK_Media_Triggers_t *pTrigger = NULL;

	pTrigger = pstStreamHandler->pHead;

	while (pTrigger)
	{
		if (pTrigger->trigger == trigger)
		{
			pTrigger->triggerValue = triggerValue;

			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("replace : trigger(%ld), value(%ld) !\n", trigger, triggerValue));
			return;
		}

		pTrigger = pTrigger->pNext;
	}

	pTrigger = (ITK_Media_Triggers_t *)HLIB_STD_MemAlloc(sizeof(*pTrigger));
	if (!pTrigger)
	{
		MhegMgrLogError(("out of memory : pTrigger is NULL !\n"));
		return;
	}
	pTrigger->trigger = trigger;
	pTrigger->triggerValue = triggerValue;
	//pTrigger->pPrev = NULL;
	//pTrigger->pNext = NULL;

	if (pstStreamHandler->pHead == NULL)
	{
		pstStreamHandler->pHead = pTrigger;
		pTrigger->pPrev = NULL;
	}
	else
	{
		pstStreamHandler->pTail->pNext = pTrigger;
		pTrigger->pPrev = pstStreamHandler->pTail;
	}
	pstStreamHandler->pTail = pTrigger;
	pTrigger->pNext = NULL;

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("new : trigger(%ld), value(%ld) !\n", trigger, triggerValue));
}

static void local_mheg_Display_AVS_clearTrigger(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t trigger)
{
	ITK_Media_Triggers_t *pTrigger = NULL;

	pTrigger = pstStreamHandler->pHead;

	while (pTrigger)
	{
		if (pTrigger->trigger == trigger)
		{
			break;
		}

		pTrigger = pTrigger->pNext;
	}

	if (!pTrigger)
	{
		MhegMgrLogAssert(("not found : pTrigger is NULL !\n"));
		return;
	}

	if (pTrigger->pPrev == NULL)
	{
		pstStreamHandler->pHead = pTrigger->pNext;
	}
	else
	{
		pTrigger->pPrev->pNext = pTrigger->pNext;
	}

	if (pTrigger->pNext == NULL)
	{
		pstStreamHandler->pTail = pTrigger->pPrev;
	}
	else
	{
		pTrigger->pNext->pPrev = pTrigger->pPrev;
	}

	HLIB_STD_MemFree(pTrigger);
	//pTrigger = NULL;

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("clear : trigger(%ld) !\n", trigger));
}

static ITK_Media_Triggers_t *local_mheg_Display_AVS_findTrigger(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t triggerValue)
{
	ITK_Media_Triggers_t *pTrigger = NULL;

	pTrigger = pstStreamHandler->pHead;

	while (pTrigger)
	{
		if (pTrigger->triggerValue == triggerValue)
		{
			MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("found : trigger(%ld), value(%ld) !\n", pTrigger->trigger, pTrigger->triggerValue));
			break;
		}

		pTrigger = pTrigger->pNext;
	}

	return pTrigger;
}

static void local_mheg_Display_AVS_setPosition(Handle_t hAction, struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t position)
{
	HUINT32	ulTime		= 0;
	HUINT32	ulActionId	= SVC_PIPE_GetActionId(hAction);
	HERROR	hErr		= ERR_FAIL;

	ulTime = position;

	/**
	* Sets the playback position so that playback commences from this position.
	*/
	switch (pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
			if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PLAYING)
			{
				if (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE)
				{
					#if 0 // TODO: (ICS) subtitle refresh 작업 필요
					if (pstStreamHandler->subtitle == ITK_PRESENT)
						AX_MHEG_RefreshSubtitle();
					#endif

					MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_YELLOW"normal play!!"C_CODE_RESET"\n"));
					hErr = local_mheg_Display_AVS_setPlayTimeStreamingAV(hAction, ulTime);
					if( (hErr != ERR_OK) && (ulTime > pstStreamHandler->endPosition) )
					{
						/* set play time이 ERR_OK가 아닌 경우는 set 하려는 position이 end position 보다 큰경우임
						 이런 경우 progress bar가 정상적으로 동작하지 않음 -> EOF 처리함  */
						 local_mheg_Display_AVS_eofPositionStreamingAV(hAction);
					}
				}
			}
			else if((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PRELOAD)
			{
				if (local_mheg_Display_AVS_isSetupStreamingAV() == TRUE)
				{
					if(s_MHEGContext.context[ulActionId].bPlayAndSeek == TRUE)
					{
						MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_YELLOW"seeked play!!"C_CODE_RESET"\n"));
						hErr = local_mheg_Display_AVS_playStream(pstStreamHandler, ulTime);
						if(hErr == ERR_OK)
						{
							s_MHEGContext.context[ulActionId].bPlayAndSeek = FALSE;
						}
					}
					else
					{
						MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, (C_CODE_F_YELLOW"incorrect status!!"C_CODE_RESET"\n"));
					}
				}
			}
			break;

		default:
		{
			MhegMgrLogAssert(("unknown or not supported reference ! [%p]\n", pstStreamHandler));
		}
		return;
	}

 	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("Sets position(%ld) : audioPid(0x%x) videoPid(0x%x) pcrPid(0x%x) avStatus(0x%x/0x%x) avComp(%ld/%ld) (ref:%d) (0x%x/0x%x) avMode(%d)! [%p]\n", position,
		pstStreamHandler->audioPid, pstStreamHandler->videoPid, pstStreamHandler->pcrPid, pstStreamHandler->audioStatus, pstStreamHandler->videoStatus,
		pstStreamHandler->audioComponent, pstStreamHandler->videoComponent, pstStreamHandler->eStreamRef, pstStreamHandler->svcHandle, pstStreamHandler->serviceId, pstStreamHandler->avMode, pstStreamHandler));
}

static void local_mheg_Display_AVS_setEndPosition(Handle_t hAction, struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t end_position)
{
	HUINT32			ulActionId;
	itk_int32_t		stream_length;
	HERROR			hErr;

	/**
	* Sets the position to play to in a stream. When the position is reached,
	* presentation stops and the host generates an ITK_MEDIA_STOPPED event.
	*/
	switch (pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
			ulActionId = SVC_PIPE_GetActionId(hAction);
			hErr = SVC_MP_GetDuration(s_MHEGContext.context[ulActionId].player, (HUINT64*)&stream_length);
			if (hErr == ERR_OK)
			{
				MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("stream_length(%ld), end position(%ld)\n", stream_length, end_position));

				if (stream_length < end_position)
				{
					pstStreamHandler->endPosition = stream_length;
				}
				else
				{
					pstStreamHandler->endPosition = end_position;
				}

				// set end position이 들어오는 시점이 Main content 라고 가정함
				if(pstStreamHandler->reference != NULL)
				{
					if(s_MHEGContext.context[ulActionId].pMainContentUri != NULL)
					{
						HLIB_STD_MemFree(s_MHEGContext.context[ulActionId].pMainContentUri);
						s_MHEGContext.context[ulActionId].pMainContentUri = NULL;
					}
					s_MHEGContext.context[ulActionId].pMainContentUri = HLIB_STD_StrDup(pstStreamHandler->reference);
				}
			}
			break;

		default:
		{
			MhegMgrLogAssert(("unknown or not supported reference ! [%p]\n", pstStreamHandler));
		}
		return;
	}

 	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("Sets endPosition(%ld) : audioPid(0x%x) videoPid(0x%x) pcrPid(0x%x) avStatus(0x%x/0x%x) avComp(%ld/%ld) (ref:%d) (0x%x/0x%x) avMode(%d)! [%p]\n", end_position,
		pstStreamHandler->audioPid, pstStreamHandler->videoPid, pstStreamHandler->pcrPid, pstStreamHandler->audioStatus, pstStreamHandler->videoStatus,
		pstStreamHandler->audioComponent, pstStreamHandler->videoComponent, pstStreamHandler->eStreamRef, pstStreamHandler->svcHandle, pstStreamHandler->serviceId, pstStreamHandler->avMode, pstStreamHandler));
}

/**
* @brief
*
* @retval ERR_OK success
* @retval ERR_FAIL fail
*/

static HERROR local_mheg_Display_AVS_createStreamingAV(Handle_t hAction, char *pucStreamPath, HUINT8 avMode)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulActionId;
	MGR_MPlayer_t *player = NULL;

	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("+++ Streaming AV Event (0x%x)\n", local_mheg_Display_AVS_getStreamingAvStatus()));

	if (avMode > MHEG_AV_MODE_BROADCAST)
	{
		if (pucStreamPath == NULL)
		{
			MhegMgrLogAssert(("Invalid File Path !\n"));
			return ERR_FAIL;
		}

		MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("Stream Path(%s)\n", pucStreamPath));

		if ((local_mheg_Display_AVS_isSetupStreamingAV() == TRUE) || (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE))
		{
			MhegMgrLogAssert(("AV Already Loaded, So Should Destroy Before Loading !\n"));
			local_mheg_Display_AVS_destroyStreamingAv(s_MHEGContext.context[s_MHEGContext.mastActionId].hAction);
		}

		local_mheg_Display_AVS_setStreamingAvStatus(local_mheg_Display_AVS_getStreamingAvStatus() | ITK_DISPLAY_AV_STREAMING_STATUS_SETTING);

		ulActionId = SVC_PIPE_GetActionId(hAction);
		player = MGR_MPlayer_GetActivePlayer(ulActionId);
		if ( player == NULL )
		{
			MhegMgrLogError(("while calling MGR_MPlayer_Create(0x%x) !\n", hErr));
			goto __ERROR;
		}

		hErr = SVC_MP_SetTLS(player, s_MHEGContext.context[ulActionId].cbTlsCertificate);
		if (hErr != ERR_OK)
		{
			MhegMgrLogError(("while calling SVC_MP_SetTLS(0x%x) !\n", hErr));
			goto __ERROR;
		}

		hErr = SVC_MP_Peek(player, eSVC_MP_PLAYTYPE_AUTO, pucStreamPath);
		if (hErr != ERR_OK)
		{
			MhegMgrLogError(("while calling SVC_MP_Peek(0x%x) !\n", hErr));
			goto __ERROR;
		}

		s_MHEGContext.context[ulActionId].player = player;

	}

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("--- Completed AV Load ! avMode(0x%x)\n", avMode));

	return hErr;

__ERROR:
	local_mheg_Display_AVS_setStreamingAvStatus(local_mheg_Display_AVS_getStreamingAvStatus() & ~ITK_DISPLAY_AV_STREAMING_STATUS_SETTING);

	return hErr;
}

static HERROR local_mheg_Display_AVS_destroyStreamingAv(Handle_t hAction)
{
	HERROR	hErr = ERR_OK;
	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("+++\n"));

	/* media 2.0 에서는 media player destroy를 안해도 됨. (get active media player를 하기 때문에)
	   Preload만 한 상태에서 exit키를 누를 경우 setting 상태 이므로 이 경우에도 media player stop을 해주기 위해서
	   check 루틴을 변경함 */
	if(local_mheg_Display_AVS_isSettingStreamingAV() == TRUE)
	{
		hErr = MGR_MPlayer_Stop(s_MHEGContext.context[ulActionId].player);
		if(hErr != ERR_OK)
		{
			//
		}

		s_MHEGContext.context[ulActionId].pstTempStreamHandler = NULL;
	}

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("Completed AV stop !\n"));

	local_mheg_Display_AVS_setStreamingAvStatus(local_mheg_Display_AVS_getStreamingAvStatus() & ~(ITK_DISPLAY_AV_STREAMING_STATUS_SETTING | ITK_DISPLAY_AV_STREAMING_STATUS_SETUP | ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING));

	MhegMgrLogAvStream(MHEG_MGR_AVSTREAM_DEBUG, ("---\n"));

	return hErr;
}

static HERROR local_mheg_Display_AVS_playStreamingAV(Handle_t hAction, char *pucStreamPath, HUINT32 ulStartTime)
{
	HERROR				hErr = ERR_FAIL;
	HUINT32				ulActionId = SVC_PIPE_GetActionId(hAction);

	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("+++\n"));

	local_mheg_Display_AVS_stopStreamingAV(hAction);

	if (local_mheg_Display_AVS_isSetupStreamingAV() == TRUE)
	{
		hErr = MGR_MPlayer_Probe(s_MHEGContext.context[ulActionId].player, pucStreamPath, eMPLAYER_PLAYTYPE_AUTO);
		if ( hErr != ERR_OK )
		{
			MhegMgrLogError(("while calling MGR_MPlayer_Play(0x%x) !\n", hErr));
			return hErr;
		}
		hErr = MGR_MPlayer_Play(s_MHEGContext.context[ulActionId].player, ulStartTime);
		MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("ulStartTime : %d\n", ulStartTime));
		if (hErr != ERR_OK)
		{
			MhegMgrLogError(("while calling MGR_MPlayer_Play(0x%x) !\n", hErr));
		}
		else
		{
			MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("Play Streaming AV !!!\n"));
			local_mheg_Display_AVS_setStreamingAvStatus(local_mheg_Display_AVS_getStreamingAvStatus() | ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING);
			// TODO: multi subtitle / audio selection 지원시에 menu의 prefer language를 참조하여 선택할 수 있도록 추가구현이 되어야 함
		}
	}

	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("---\n"));

	return hErr;
}

static HERROR local_mheg_Display_AVS_stopStreamingAV(Handle_t hAction)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);

	// TODO: video, audio 선택적으로 stop을 해야하는 것인지 ?

	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("+++\n"));

	if (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE)
	{
		local_mheg_Display_AVS_controlStreamingSubtitle(hAction, FALSE);

		hErr = MGR_MPlayer_Stop(s_MHEGContext.context[ulActionId].player);
		if (hErr != ERR_OK)
		{
			MhegMgrLogAssert(("ERROR : while calling MW_MEDIA_Stop(0x%x) !\n", hErr));
		}
		else
		{
			MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("Stop Streaming AV !!!\n"));
			local_mheg_Display_AVS_setStreamingAvStatus(local_mheg_Display_AVS_getStreamingAvStatus() & ~ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING);
		}
	}

	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("---\n"));

	return hErr;
}

static HERROR local_mheg_Display_AVS_pauseStreamingAV(Handle_t hAction)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("+++\n"));

	if (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE)
	{
		hErr = SVC_MP_SetPlaySpeed(s_MHEGContext.context[ulActionId].player, 0);
		if (hErr != ERR_OK)
		{
			MhegMgrLogError(("while calling MW_MEDIA_Pause(0x%x) !\n", hErr));
		}
	}

	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("---\n"));
	return hErr;
}

static HERROR local_mheg_Display_AVS_resumeStreamingAV(Handle_t hAction)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("+++\n"));

	if (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE)
	{
		hErr = SVC_MP_SetPlaySpeed(s_MHEGContext.context[ulActionId].player, 100);
		if (hErr != ERR_OK)
		{
			MhegMgrLogError(("while calling MW_MEDIA_Resume(0x%x) !\n", hErr));
		}
	}

	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("---\n"));
	return hErr;
}

static HERROR local_mheg_Display_AVS_controlStreamingSubtitle(Handle_t hAction, HBOOL bOn)
{
	HERROR				hErr = ERR_FAIL;

#if 0	/* jwkim: subtitle function is not implemented in new media 2.0 yet. */
	HUINT32 			ulActionId;
	HUINT16				usUniqueId;
	MW_MEDIA_INFO_t		stMediaPlayInfo;

	ulActionId = SVC_PIPE_GetActionId(hAction);
	hErr = MW_MEDIA_GetCurrentStreamInfo(ulActionId, &stMediaPlayInfo); // MW_MEDIA_GetPlayInfo 사용하지 말 것 (방식이 다름)

#ifdef CONFIG_DEBUG
	local_mheg_Debug_Print_PlayInfo(&stMediaPlayInfo);
#endif
	usUniqueId = DB_SVC_GetTempUniqueId();

	if (bOn == TRUE)
	{
		hErr = XSVC_SI_MHEG_StartStreaming(hAction, HANDLE_NULL, usUniqueId, stMediaPlayInfo.ulDemuxId);
	}
	else
	{
		hErr = XSVC_SI_MHEG_StopStreaming(hAction, usUniqueId);
// ToDo:		SVC_MP_SetPlayPosition(hAction, eSyncMode);
	}
#endif

	return hErr;
}

static HERROR local_mheg_Display_AVS_setPlayTimeStreamingAV(Handle_t hAction, HUINT32 ulTime)
{
	HERROR	hErr = ERR_FAIL;
	HUINT32	ulActionId = SVC_PIPE_GetActionId(hAction);
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("+++\n"));

	if (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE)
	{
		hErr = SVC_MP_SetPlayPosition(s_MHEGContext.context[ulActionId].player, ulTime);
		if (hErr != ERR_OK)
		{
			MhegMgrLogError(("while calling MW_MEDIA_SetPlayTime(0x%x) !\n", hErr));
		}
	}

	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("---\n"));
	return hErr;
}

static void local_mheg_Display_AVS_changePositionStreamingAV(Handle_t hAction)
{
	itk_int32_t position = 0;
	itk_media_event_t evt = ITK_MEDIA_EVENT;
	itk_media_event_data_t evt_data, *ec = NULL;
	HUINT64	curPosition = 0;
	HUINT32 ulActionId = SVC_PIPE_GetActionId(hAction);

	if (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE)
	{
		ITK_AvstreamHandle_t *pstStreamHandler = PORT_ITK_AVS_GetHandle(local_mheg_Display_AVS_getCurHandle());

		if (pstStreamHandler != NULL)
		{
			if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI && pstStreamHandler->presentation_count > 0)
			{
				if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PLAYING)
				{
					SVC_MP_GetPlayPosition(s_MHEGContext.context[ulActionId].player, &curPosition);
					MhegMgrLogTrace(("pstStreamHandler (%p) -> eStreamRef (%d), presentation_count (%ld), position (%ld), "C_CODE_F_BLACK_B_YELLOW"curPosition(%ld)"C_CODE_RESET"\n",
									pstStreamHandler, pstStreamHandler->eStreamRef, pstStreamHandler->presentation_count, pstStreamHandler->position, curPosition));

					position = (itk_int32_t)curPosition;
					if (pstStreamHandler->position != position)
					{
						pstStreamHandler->position = position;
						if (pstStreamHandler->endPosition != 0 && pstStreamHandler->endPosition <= position)
						{
							evt = ITK_MEDIA_STOPPED;
						}
						else
						{
							ITK_Media_Triggers_t *pTrigger = NULL;

							pTrigger = local_mheg_Display_AVS_findTrigger(pstStreamHandler, position);
							if (pTrigger)
							{
								evt = ITK_MEDIA_TRIGGER;
								evt_data.trigger = pTrigger->trigger;
								ec = (itk_media_event_data_t *)&evt_data;
							}
						}
					}

					if (evt < ITK_MEDIA_EVENT)
					{
						if (evt == ITK_MEDIA_STOPPED)
						{
							pstStreamHandler->vtbl.stop((itk_avstream_t *)pstStreamHandler);
						}
						else
						{
							local_mheg_Display_AVS_EventCallback(pstStreamHandler, evt, ec);
						}
					}
				}
			}
		}
	}
}

static void local_mheg_Display_AVS_startStreamingAV(void)
{
	ITK_AvstreamHandle_t *pstStreamHandler = PORT_ITK_AVS_GetHandle(local_mheg_Display_AVS_getCurHandle());

	if (pstStreamHandler != NULL)
	{
		MhegMgrLogTrace(("pstStreamHandler (%p) -> eStreamRef (%d), presentation_count (%ld), position (%ld)\n",
						pstStreamHandler, pstStreamHandler->eStreamRef, pstStreamHandler->presentation_count, pstStreamHandler->position));

		if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI && pstStreamHandler->presentation_count > 0)
		{
			if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_STOPPED)
			{
				pstStreamHandler->mediaStatus = ITK_DISPLAY_AV_STATUS_PLAYING;
				local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_PLAYING, NULL);
			}
		}
	}
}

static void local_mheg_Display_AVS_eofPositionStreamingAV(Handle_t hAction)
{
	if (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE)
	{
		ITK_AvstreamHandle_t *pstStreamHandler = PORT_ITK_AVS_GetHandle(local_mheg_Display_AVS_getCurHandle());

		if (pstStreamHandler != NULL)
		{
			MhegMgrLogTrace(("pstStreamHandler (%p) -> eStreamRef (%d), presentation_count (%ld), position (%ld), mediaTerm (%d)\n",
							pstStreamHandler, pstStreamHandler->eStreamRef, pstStreamHandler->presentation_count, pstStreamHandler->position, pstStreamHandler->mediaTerm));

			if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI)
			{
				if (pstStreamHandler->presentation_count == 1)
				{
					if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PLAYING)
					{
						pstStreamHandler->mediaStatus = ITK_DISPLAY_AV_STATUS_STOPPED;
						local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_STOPPED, NULL);
						if (pstStreamHandler->mediaTerm == ITK_MEDIA_BLANK)
						{
							/*
								<ICS009>
								At the end of the stream playout (30 seconds) the progress bar must aso display that it has reached its completion.

								position을 0으로 하지 않음
							*/
							//pstStreamHandler->position = 0;
							local_mheg_Display_AVS_stopStreamingAV(hAction);
						}
					}
				}
				else if (pstStreamHandler->presentation_count > 1)
				{
					pstStreamHandler->presentation_count--;
					local_mheg_Display_AVS_playStreamingAV(hAction, pstStreamHandler->reference, 0);
				}
			}
		}
	}
}

static void local_mheg_Display_AVS_underflowResumeStreamingAV(void)
{
	if (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE)
	{
		ITK_AvstreamHandle_t *pstStreamHandler = PORT_ITK_AVS_GetHandle(local_mheg_Display_AVS_getCurHandle());

		if (pstStreamHandler != NULL)
		{
			MhegMgrLogTrace(("pstStreamHandler (%p) -> eStreamRef (%d), presentation_count (%ld), pstStreamHandler->mediaStatus(0x%x)\n", pstStreamHandler, pstStreamHandler->eStreamRef, pstStreamHandler->presentation_count, pstStreamHandler->mediaStatus));

			if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI && pstStreamHandler->presentation_count > 0)
			{
				if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PLAYING)
				{
					if ((pstStreamHandler->mediaStatus & 0xf0) & ITK_DISPLAY_AV_STATUS_STALLED)
					{
						pstStreamHandler->mediaStatus &= ~ITK_DISPLAY_AV_STATUS_STALLED;
						local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_RESUMED, NULL);
					}
				}
			}
		}
	}

}

static void local_mheg_Display_AVS_underflowStreamingAV(void)
{
	if (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE)
	{
		ITK_AvstreamHandle_t *pstStreamHandler = PORT_ITK_AVS_GetHandle(local_mheg_Display_AVS_getCurHandle());

		if (pstStreamHandler != NULL)
		{
			MhegMgrLogTrace(("pstStreamHandler (%p) -> eStreamRef (%d), presentation_count (%ld)\n", pstStreamHandler, pstStreamHandler->eStreamRef, pstStreamHandler->presentation_count));

			if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI && pstStreamHandler->presentation_count > 0)
			{
				if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PLAYING)
				{
					if (!((pstStreamHandler->mediaStatus & 0xf0) & ITK_DISPLAY_AV_STATUS_STALLED))
					{
						pstStreamHandler->mediaStatus |= ITK_DISPLAY_AV_STATUS_STALLED;
						local_mheg_Display_AVS_EventCallback(pstStreamHandler, ITK_MEDIA_STALLED, NULL);
					}
				}
			}
		}
	}

}

static HUINT8 local_mheg_Display_AVS_getStreamingAvStatus(void)
{
	return	s_MHEGContext.context[s_MHEGContext.mastActionId].avStatus;
}

static void local_mheg_Display_AVS_setStreamingAvStatus(HUINT8	avStatus)
{
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("+++ avStatus(0x%x)\n", avStatus));

	s_MHEGContext.context[s_MHEGContext.mastActionId].avStatus = avStatus;
	PORT_ITK_SetMastMhegAvStatus(avStatus);
}

static HBOOL local_mheg_Display_AVS_isSettingStreamingAV(void)
{
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("+++ avEvent(0x%x)\n", local_mheg_Display_AVS_getStreamingAvStatus()));

	return (HBOOL)(local_mheg_Display_AVS_getStreamingAvStatus() & (ITK_DISPLAY_AV_STREAMING_STATUS_SETTING)) ? TRUE : FALSE;
}

static HBOOL local_mheg_Display_AVS_isSetupStreamingAV(void)
{
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("+++ avEvent(0x%x)\n", local_mheg_Display_AVS_getStreamingAvStatus()));

	return (HBOOL)(local_mheg_Display_AVS_getStreamingAvStatus() & ITK_DISPLAY_AV_STREAMING_STATUS_SETUP) ? TRUE : FALSE;
}

static HBOOL local_mheg_Display_AVS_isPlayingStreamingAV(void)
{
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("+++ avEvent(0x%x)\n", local_mheg_Display_AVS_getStreamingAvStatus()));

	return (HBOOL)(local_mheg_Display_AVS_getStreamingAvStatus() & ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING) ? TRUE : FALSE;
}
#endif

static void local_mheg_Misc_showPromptForGuidance(const char *restriction)
{
	HERROR		hErr;
	HBOOL		bLock = FALSE;
	HINT32		nParantRateValue;

	MhegMgrLogFunc(("+++\n"));
//	MhegMgrLogTrace(("restriction(%s), this App(%p), this App Callback(%p)\n", restriction, GWM_APP_GetThis(), GWM_APP_GetAppCallback(GWM_APP_GetThis())));

#if 1 // defined (CONFIG_OP_UK_MENU)
//	nParantRateValue = MWC_PARAM_GetGuidancePolicy();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_GUIDANCE_POLICY, (HUINT32 *)&nParantRateValue, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_GUIDANCE_POLICY) Error!!!\n");
	}
#else
	hErr = ERR_OK;
	nParantRateValue = MWC_PARAM_GetMaturityRating();
#endif

	if (hErr == ERR_OK)
	{
#if 1 // defined (CONFIG_OP_UK_MENU)
		bLock = (nParantRateValue == eDxGUIDANCEPOLICY_OFF) ? FALSE : TRUE;
#else
		bLock = (nParantRateValue == eRATING_VIEW_ALL) ? FALSE : TRUE;
#endif
	}

	if (bLock == FALSE)
	{
		PORT_ITK_MISC_ClosePincodeForPFG(TRUE);
	}

	MhegMgrLogFunc(("---\n"));
}

static void local_mheg_RegisterEngineProfile(MHEG_Engine_Profile_t *pEngineProfile, redkey_profile_t *pItkProfile, size_t *pulItkMemSize)
{
	MHEG_Engine_Profile_t 	eEngineProfile = 0;
	redkey_profile_t	eItkProfile;
	size_t			ulItkMemSize;

#if defined(CONFIG_3RD_MHEG_CIPLUS_PROFILE1) || defined(CONFIG_MW_CI_PLUS)
	eEngineProfile 	|= MHEG_ENGINE_PROFILE_CI_PLUS;
	eItkProfile 		= REDKEY_PROFILE_CIPLUS;			/* ITK profile is same to CIPlus profiles in Redkey */
#endif

#if defined(CONFIG_3RD_MHEG_UK_PROFILE1)
	eEngineProfile 	|= MHEG_ENGINE_PROFILE_UK;
	eItkProfile 		= REDKEY_PROFILE_UK_SD;
#endif

#if defined(CONFIG_3RD_MHEG_UK_PROFILE_HD)
	eEngineProfile 	|= MHEG_ENGINE_PROFILE_UK;
	eItkProfile 		= REDKEY_PROFILE_UK_HD;
#endif

#if defined(CONFIG_3RD_MHEG_UKFEESAT_PROFILE)
	eEngineProfile 	|= MHEG_ENGINE_PROFILE_UK_FSAT;
//	eItkProfile 		= REDKEY_PROFILE_UK_FSAT; // TODO:  or REDKEY_PROFILE_FREESATG2
	eItkProfile 		= REDKEY_PROFILE_FREESATG2;
#endif

#if defined(CONFIG_3RD_MHEG_AUSTRALIA_PROFILE)
	eEngineProfile 	|= MHEG_ENGINE_PROFILE_AUSTRALIA;
	eItkProfile 		= REDKEY_PROFILE_AUSTRALIA;
#endif

#if defined(CONFIG_3RD_MHEG_NZ_PROFILE1)
	eEngineProfile 	|= MHEG_ENGINE_PROFILE_NZ;
	eItkProfile 		= REDKEY_PROFILE_NZ;
#endif

	/* set memory size */
	if (eEngineProfile & (MHEG_ENGINE_PROFILE_UK | MHEG_ENGINE_PROFILE_NZ))
	{
		if (eItkProfile == REDKEY_PROFILE_UK_SD)
			ulItkMemSize = 4 * 1024 * 1024;
		else
			ulItkMemSize = 18 * 1024 * 1024;
	}
	else if (eEngineProfile & MHEG_ENGINE_PROFILE_UK_FSAT)
	{
		ulItkMemSize = 8 * 1024 * 1024;	// same as FOXSAT. To be optimize (5MB -> 8MB)
	}
	else if (eEngineProfile & MHEG_ENGINE_PROFILE_AUSTRALIA)
	{
		/*
			2010.05.28. 6.2.4.D항목의 application text를 동작시키려면 아래 size필요
		*/
		ulItkMemSize = 20 * 1024 * 1024; //16 * 1024 * 1024;	// 호주향은 MHEG EPG를 위해 memory를 많이(16M) 필요로 한다
	}
	else if (eEngineProfile & MHEG_ENGINE_PROFILE_CI_PLUS)
		ulItkMemSize =  4 * 1024 * 1024;
	else
		ulItkMemSize = 16 * 1024 * 1024;

	s_MHEGContext.itkProfile = eItkProfile;

	*pEngineProfile = eEngineProfile;
	*pItkProfile = eItkProfile;
	*pulItkMemSize = ulItkMemSize;
}

/*
* DI_AUDIO_StopMemoryStream() 함수에 의해 전달되는 이벤트 메세지는 어떤 audio clip 에 대한 stop 인지 알 수 없어 sync가 맞지 않는 경우 발생함
 (DI에서 context ID 같은 구분인자 필요)
*/
static void	local_mheg_AUDIOCLIP_EventCallback(Handle_t hAction, itk_media_event_t audioclipEvent)
{
	Handle_t hMasterAction;
	ITK_AudioclipHandle_t *ac = (ITK_AudioclipHandle_t*)PORT_ITK_AUDIOCLIP_getCurHandle();
	ITK_AudioclipHandle_t *stopped_ac = (ITK_AudioclipHandle_t*)PORT_ITK_AUDIOCLIP_getStoppedHandle();

	if (ac)
	{
		local_mheg_GetContextMastActionHandle(&hMasterAction);

		MhegMgrLogTrace(("hAction (0x%x), Master hAction (0x%x), audioclipEvent (%d), ac (%p), stopped_ac (%p) \n", hAction, hMasterAction, audioclipEvent, ac, stopped_ac));

		if (ac->callback)
		{
			switch (audioclipEvent)
			{
				case ITK_MEDIA_PLAYING:
					MhegMgrLogFunc(("ITK_MEDIA_PLAYING\n"));
// MW단에서 callback이 늦게 와서 AudioClip play시 그냥 먼저 callback을 던져주도록 수정
//					ac->callback(ac->callback_context, (itk_audioclip_t *)ac, audioclipEvent);
					break;

				case ITK_MEDIA_STOPPED:
					MhegMgrLogFunc(("ITK_MEDIA_STOPPED\n"));
					if (stopped_ac != NULL)
					{
						// 명시적으로 audio clip stop이 이루어진 경우 -> engine에게 callback을 줄 필요 없음
						MhegMgrLogTrace(("stopped_ac->ucAudioEv = %d\n", stopped_ac->ucAudioEv));
#if 0
						if (stopped_ac->ucAudioEv == eITK_DISPLAY_EVENT_STOP)
						{
							PORT_ITK_AUDIOCLIP_setStoppedHandle(NULL);
							stopped_ac->callback(stopped_ac->callback_context, (itk_audioclip_t *)stopped_ac, audioclipEvent);
						}
#else
						PORT_ITK_AUDIOCLIP_setStoppedHandle(NULL);
#endif
					}
					else
					{
						// Clip finished에 의해서 audio clip stop이 이루어진 경우
						MhegMgrLogTrace(("ac->ucAudioEv = %d\n", ac->ucAudioEv));
						if (ac->ucAudioEv != eITK_DISPLAY_EVENT_STOP)
						{
							PORT_ITK_AUDIOCLIP_setCurHandle(NULL);

							//stop audio clip
							SVC_AV_StopAudioClip(0); // TODO: Request ID 가 생겼으니 활용하세요.
							ac->ucAudioEv = eITK_DISPLAY_EVENT_STOP;
							ac->callback(ac->callback_context, (itk_audioclip_t *)ac, audioclipEvent);
						}
						else
						{
							MhegMgrLogTrace(("ac is already stopped.\n"));
						}
					}
					break;
				default:
					break;
			}
		}
	}
	else
	{
		MhegMgrLogTrace(("audio clip is not present or already released.\n"));
	}
}

static MHEG_START_ERR_CODE_t local_mheg_StartMhegAppl(void)
{
	HERROR hResult = ERR_FAIL;

	hResult = PORT_ITK_SVC_Start();
	if (hResult == ERR_OK)
	{
		MhegMgrLogTrace(("START REDKEY !!! \r\n"));
		PORT_ITK_Display_Show();

		return eMHEG_START_SUCCESS;
	}
	else
	{
		MhegMgrLogError(("START REDKEY !!! \r\n"));
		return eMHEG_START_FAIL;
	}
}

static void	local_mheg_StopMhegAppl(Handle_t hAction, itk_service_tune_event_t eTuneEvent)
{
	ITK_AvstreamHandle_t *pstStreamHandler = PORT_ITK_AVS_GetHandle(local_mheg_Display_AVS_getCurHandle());

	MhegMgrLogTrace(("hAction(0x%x), eTuneEvent(%d)\n", hAction, eTuneEvent));

#if defined(CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	MhegMgrLogTrace(("Cleaning IP Streaming resources !\n"));
	local_mheg_Display_AVS_destroyStreamingAv(hAction);
#endif

	if ((s_stMhegMng.tuneFlag & ITK_TUNE_KEEP_DISPLAY) == 0) // Non destructive tune 일 경우는 redkey만 stop하고 display는 그대로 유지하도록 함
	{
		PORT_ITK_SetInternalMhegRunningStatus(FALSE);

		/* restore demux resource before stop */
		if ( ITK_getManager(ITK_DEMUX) && !(s_stMhegMng.tuneFlag & ITK_TUNE_KEEP_CONTEXT) ) // quite tune 일 경우 redkey 가 reset 하도록 함
		{
			PORT_ITK_DEMUX_Restore();
		}

		PORT_ITK_Display_StopIFrameByOther(hAction);
		PORT_ITK_AUDIOCLIP_Restore(hAction);

		if(pstStreamHandler != NULL)
		{
			if (pstStreamHandler->bVideoFreeze == TRUE || pstStreamHandler->bAudioFreeze == TRUE)
			{
				SVC_AV_SetVideoFreeze(SVC_PIPE_GetActionId(hAction), FALSE);
				SVC_AV_SetAudioFreeze(SVC_PIPE_GetActionId(hAction), FALSE);
				pstStreamHandler->bVideoFreeze = FALSE;
				pstStreamHandler->bAudioFreeze = FALSE;
			}
		}

		PORT_ITK_Display_Restore(eTuneEvent);

	//	local_mheg_Display_AVS_setCurHandle_V(NULL);
	//	local_mheg_Display_AVS_setCurHandle_A(NULL);

		PORT_ITK_Display_InitVideoScalingInfo();
	}

	MhegMgrLogTrace(("Request Redkey Engine stop\n"));

	if (s_MHEGContext.itkProfile == REDKEY_PROFILE_CIPLUS)
	{
		PORT_ITK_APPMMI_STOP(eTuneEvent);
	}

	PORT_ITK_SVC_Stop(eTuneEvent);

	s_MHEGContext.context[s_MHEGContext.mastActionId].bMHEGStopByHost = TRUE;
}

static void local_mheg_RestartMhegAppl(Handle_t hAction)
{
	HUINT32		ulActionId = SVC_PIPE_GetActionId(hAction);

	if(s_MHEGContext.context[ulActionId].status != REDKEY_STOPPED)
	{
		local_mheg_StopMhegAppl(hAction, ITK_SERVICE_TUNE_STOP_HOST);
		s_MHEGContext.bSubtitleEnable = TRUE; // TODO: ITK Engine에서 설정해야하는데 왜 여기서 manual 하게 값을 변경하는지 확인 필요
	}

	VK_TASK_Sleep(1000);			// for graphic display

	if(s_MHEGContext.context[ulActionId].status == REDKEY_STOPPED)
	{
		if (local_mheg_StartMhegAppl() == eMHEG_START_FAIL)
		{
			MhegMgrLogAssert(("Retry Redkey\n"));
			VK_TASK_Sleep(2);
			/* wait 2 ticks and retry to start Redkey */
			BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 0, 1, 0);
		}
	}
	else
	{
		MhegMgrLogAssert(("Retry Redkey\n"));
		VK_TASK_Sleep(300);
		/* wait 2 ticks and retry to start Redkey */
		BUS_PostMessage (NULL, eMEVT_MHEG_START_REDKEY_RETRY, hAction, 0, 1, 0);
	}
}

#if defined (MHEG_UI_ORDER_BY_RUNNING_STATUS)
static BUS_Result_t local_mheg_draw_background(OSD_Screen_t *screen, OSD_Area_t *area, OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	HINT32	x, y, w, h;
	OSD_Color_t color;

	if (local_mheg_CheckMHEGIsRunning() == FALSE)
	{
		return ERR_OK;
	}

	x = area->x1 + rect->x;
	y = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	// draw background
	color = COL_TRANSPARENT;
	OSD_GFX_DrawRectFill(screen, x, y, x + w - 1, y + h - 1, color);

	return ERR_OK;
}
#endif



/*
   REMARK : SI task에서 PMT를 받거나 PMT가 변경이 되면, EVT_SI_PMT를 WatchTV로 보내게 되고,
            MHEG Manager를 이를 받아서 PMT가 Received된것을 ITK에 알린다. 이 함수에 의해서
            ITK는 PMT Raw Data를 관리하게 됨. PMT를 받으면 PAT 데이타도 같이 저장 관리하고,
            PMT, PAT 데이터 리스트는 Redkey가 다시 구동되지 않으면 지속적으로 관리 된다.
            이 작업은 MHEG Manager task(GWM)에 의해서 이루어 진다. */
HERROR local_mheg_NotifyTableReceived(Handle_t hAction,SiTableType_e eSiTableType)
{
#if 0
	Handle_t				hActionMheg, hDefSvc, hCurSvc;
	HERROR					ulRet = 0;
	HUINT16 				usTsUniqId;
	HUINT16 				usSvcId;
	HUINT16					usSecIndex;
	HUINT32 				ulSecCnt = 0;
	HUINT8*					pucRawData = NULL, *pTmpRaw = NULL;

//	if(s_bIsObjectAvailable == FALSE)
//		return ERR_FAIL;

	PORT_ITK_GetMastActionHandle(&hActionMheg);
	MhegMgrLogTrace((" notify PMT / PAT Received !!! hAction(0x%x)\n",hAction));

	if(hActionMheg != hAction)
	{
		MhegMgrLogAssert((" different Action handle(0x%x : 0x%x )\n",hActionMheg, hAction));
		return ERR_FAIL;
	}

	PORT_ITK_GetSvcHandle(&hDefSvc, &hCurSvc);
	usTsUniqId = SVC_SI_UTIL_GetTsUniqIdFromSvcHandle(hDefSvc);
	usSvcId = SVC_SI_UTIL_GetSvcIdFromSvcHandle(hDefSvc);

	ulRet = MW_SI_CountRawSection(usTsUniqId, eSiTableType,((eSiTableType == eSI_TableType_PAT)?FALSE:TRUE), usSvcId, &ulSecCnt);
	MhegMgrLogTrace(("ulErr = %d, usSecCnt = %d, usSvcId = 0x%x\n", ulRet, ulSecCnt, usSvcId));
	if (ulRet != ERR_OK)
	{
		MhegMgrLogAssert(("usTsUniqId(0x%x), usSvcId(0x%x), Error(0x%x), eSiTableType(%d)\n", usTsUniqId, usSvcId, ulRet, eSiTableType));
		return ERR_FAIL;
	}

	for(usSecIndex = 0; usSecIndex < ulSecCnt; usSecIndex++)
	{
		ulRet = MW_SI_GetRawSection(usTsUniqId, eSiTableType, ((eSiTableType == eSI_TableType_PAT)?FALSE:TRUE), usSvcId, usSecIndex, &pTmpRaw);
		if (ulRet == ERR_OK)
		{
			pucRawData = pTmpRaw;
			break;
		}
	}

	if (pucRawData && ulRet == ERR_OK)
	{
		XSVC_SI_MHEG_AddCurTableFromBaseSI(hAction, pucRawData, hDefSvc,eSiTableType);
	}

	return ulRet;
#endif
}

static void local_mheg_ResetNvsFs(Handle_t hAction, Mheg_Nvs_Fs_Type_t MhegFsType)
{
	if (ITK_getManager(ITK_NVS))
	{
		HUINT32 ulActionId = SVC_PIPE_GetActionId(hAction);

		/* mheg이 running중일때 nvs에 변화를 주면 안된다 */
		if ((s_MHEGContext.context[ulActionId].status != REDKEY_RUNNING) || (MhegFsType == eMHEG_NVS_FACTORY_RESET))
		{
			PORT_ITK_Nvs_ResetFs(MhegFsType);
		}
	}
}

static void local_mheg_ResetDvr(Handle_t hAction)
{
#if defined(CONFIG_3RD_MHEG_USE_PVR_FUNCTIONALITY)
	(void)hAction;
	PORT_ITK_Dvr_Reset();
#endif
}

static void local_mheg_GetVersionInfo(Handle_t hAction)
{
	BUS_PostMessage(NULL, eMEVT_MHEG_VERSION_INFO, hAction, (HINT32)szItkVersion, 0, 0);
}

static void local_mheg_ChangeAv(Handle_t hAction, DxVideoCodec_e eVideoCodec, HUINT16 usVideoPid, DxAudioCodec_e eAudioCodec, HUINT16 usAudioPid, HUINT16 usPcrPid)
{
#if defined(MHEG_MGR_SYNC_VIDEO_SUBTITLE)
	HUINT32 ulActionId;

	MhegMgrLogTrace(("v(0x%x, 0x%x) a(0x%x, 0x%x) p(0x%x)\n", eVideoCodec, usVideoPid, eAudioCodec, usAudioPid, usPcrPid));

	local_mheg_GetContextMastActionId(&ulActionId);

	s_MHEGContext.context[ulActionId].ucChangedVideoPid = usVideoPid;

	if (usVideoPid == PID_NULL)
	{
		/* Engine에 의해 video stop되었다면, subtitle도 꺼준다 */
		local_mheg_Display_subtitleEnable(FALSE);
	}
	else
	{
		/* Engine이 subtitle off했다면, Host가 on하지 않는다 */
		if (s_MHEGContext.bSubtitleEnable == TRUE)
		{
			local_mheg_Display_subtitleEnable(TRUE);
		}
	}
#endif

	MGR_LIVE_CmdChangeTempPid(hAction,eVideoCodec, usVideoPid, eAudioCodec, usAudioPid, usPcrPid);
}

#if 0
static void local_mheg_Debug_Print_PlayInfo(MW_MEDIA_INFO_t* pstMediaPlayInfo)
{
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("##### Media Play Info #####\n" \
				"\t stream_type (%d), max_bitrate (%d), duration (%d)\n" \
				"\t nprograms (%d), ntracks (%d)\n" \
				"\t video_pes_id (0x%x), video_stream_id (0x%x), video_codec (%d)\n" \
				"\t usVideoWidth (%d), usVideoHeight (%d)\n" \
				"\t ucTotalAudioTrack (%d), [0].audio_pes_id (0x%x), [0].audio_stream_id (0x%x)\n" \
				"\t [0].audio_codec (%d), [0].uacAudioLang[0] (%s)\n" \
				"\t ucTotalVideoTrack (%d), [0].video_pes_id (0x%x), [0].video_stream_id (0x%x)\n" \
				"\t [0].video_codec[0] (%d)\n" \
				"\t audio_pes_id (0x%x), audio_stream_id (0x%x), audio_codec (%d)\n" \
				"\t support_trick (%d), support_seek (%d), pcr_pid (0x%x)\n" \
				"\t audiodecodertype (%d), videodecodertype (%d), eStartCondition (%d), ulStartTime (%d)\n" \
				"\t demuxId (%d)\n"
				,pstMediaPlayInfo->stream_type, pstMediaPlayInfo->max_bitrate, pstMediaPlayInfo->duration
				,pstMediaPlayInfo->nprograms, pstMediaPlayInfo->ntracks
				,pstMediaPlayInfo->video_pes_id, pstMediaPlayInfo->video_stream_id, pstMediaPlayInfo->video_codec
				,pstMediaPlayInfo->usVideoWidth, pstMediaPlayInfo->usVideoHeight
				,pstMediaPlayInfo->ucTotalAudioTrack, pstMediaPlayInfo->stAudioTrack[0].audio_pes_id, pstMediaPlayInfo->stAudioTrack[0].audio_stream_id
				,pstMediaPlayInfo->stAudioTrack[0].audio_codec, pstMediaPlayInfo->stAudioTrack[0].uacAudioLang
				,pstMediaPlayInfo->ucTotalVideoTrack, pstMediaPlayInfo->stVideoTrack[0].video_pes_id, pstMediaPlayInfo->stVideoTrack[0].video_stream_id
				,pstMediaPlayInfo->stVideoTrack[0].video_codec
				,pstMediaPlayInfo->audio_pes_id, pstMediaPlayInfo->audio_stream_id, pstMediaPlayInfo->audio_codec
				,pstMediaPlayInfo->support_trick, pstMediaPlayInfo->support_seek, pstMediaPlayInfo->pcr_pid
				,pstMediaPlayInfo->audiodecodertype, pstMediaPlayInfo->videodecodertype, pstMediaPlayInfo->eStartCondition, pstMediaPlayInfo->ulStartTime
				,pstMediaPlayInfo->demuxId
				));

#if SUPPORT_MSDRM_PD
	MhegMgrLogStreamingAv(MHEG_MGR_STREAMING_AV_DEBUG, ("\t is_drm (%d)\n", pstMediaPlayInfo->is_drm));
#endif

}
#endif
