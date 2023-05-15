/*************************************************************
 * @file		mheg_port.c
 * @date		2013/11/16
 * @brief		Humax SnT Mheg Player
 *
 * http://www.humaxdigital.com
 *************************************************************/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <silib.h>
#include <hapi.h>
#include <papi.h>
#include <apk.h>
#include <vkernel.h>

#include "mheg_int.h"
#include "mheg_sef.h"
#include "mheg_si.h"
#include "mheg_port_itk.h"

#include <port_itk_os.h>
#include <port_itk_main.h>
#include <port_itk_display.h>
#include <port_itk_image.h>
#include <port_itk_service.h>
#include <port_itk_audioclip.h>
#include <port_itk_misc.h>
#include <port_itk_font.h>
#include <port_itk_http.h>
#include <port_itk_crypto.h>
#include <port_itk_zlib.h>
#include <port_itk_nvs.h>
#include <port_itk_appmmi.h>
#include <port_itk_avstream.h>
#include <port_itk_demux.h>
#include <port_itk_dsmccfs.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


#define ITK_VERSION_INFO_MAX	10

#define	IS_MHEG_REQUEST_TUNE(mode)					(mode == ITK_SERVICE_TUNE_STOP_ITK)
#define	IS_MHEG_REQUEST_QUIET_TUNE(mode, flag)			((mode == ITK_SERVICE_TUNE_STOP_ITK) && (flag & (ITK_TUNE_KEEP_CONTEXT|ITK_TUNE_NO_BANNER)))
#define	IS_MHEG_REQUEST_NDT_TUNE(mode, flag)			((mode == ITK_SERVICE_TUNE_STOP_ITK) && (flag & (ITK_TUNE_KEEP_DISPLAY|ITK_TUNE_NO_BANNER)))
#define	IS_MHEG_REQUEST_NDT_QUIET_TUNE(mode, flag)		((mode == ITK_SERVICE_TUNE_STOP_ITK) && (flag & (ITK_TUNE_KEEP_CONTEXT|ITK_TUNE_KEEP_DISPLAY|ITK_TUNE_NO_BANNER)))

#define ITK_START_RETRY_LIMIT_NUM		15


#define SESSION_URI_FORM	"hbuffer://cso_session/%d"

#define MAX_DOWNLOAD_SESSION_NUM	5

//#define	ICS_PROBE
#define		ICS_PLAY_MEDIA					0xFFFD
#define		ICS_PLAY_LIVE					0xFFFE
#define		ICS_NO_PLAY					0xFFFF

#define 	SUBTITLE_OFF					(-1) /* subtitle index를 -1로 주면 subtitle task가 멈춘다. */
#define 	COMPONENT_OFF					(-1) /* Component index를 -1로 주면 해당 component는 사용되지 않는다.  */
#define 	DISABLE_INDEX					(0)  /* MHEG에서 audio/video index가 0는 disable을 의미 한다.  */
#define 	TS_PACKET_BYTES				188   /* MHEG의 position이 packet 단위로 내려와서 변환시 필요하다. */


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eMHEG_START_FAIL,
	eMHEG_START_SUCCESS,
	eMHEG_START_NOT_REQUIRE		/* Australia Freeview를 위한 UI사양 */
} MHEG_START_ERR_CODE_t;

typedef enum
{
	eNO_IPLATE 			= 0x1,		/**< i-plate display 없이 튜닝함. */
	ePINCODE_ON_TOP 	= 0x2,		/**< PIN code는 exit전까지 메뉴 위에 뜸. Exit하면 바닥에 깔림.*/
	ePINCODE_PASSED 	= 0x4,		/**< PIN code는 외부에서 pass 했음.. pin code pass로 간주. */
	eKEEP_PINPASS 		= 0x8,		/**< PIN code pass flag를 현재 session 대로 유지하라. (ex. subfeed끼리 이동... ) */
	eFORCE_TUNE 		= 0x10,		/**< 현재 튜닝중인 서비스 핸들이라도 강제로 재 튜닝 시키도록 한다. */
	eNDT_TUNE 			= 0x20,		/**< MHEG의 Non Destructive Tune을 위한 mode로 이전 Video와 Audio를 끊지 않고 변경될 redkey가 변경할 service handle 만 MHEG manager로 전달하기 위함. */
} MHEG_TUNING_MODE_t;

typedef enum{
	eMHEG_AV_SPEED_STOP	= 0x00,
	eMHEG_AV_SPEED_PLAY,
	eMHEG_AV_SPEED_PAUSE,
	eMHEG_AV_SPEED_REWIND,
	eMHEG_AV_SPEED_FAST_FWD,
}MHEG_AV_SPEED_t;

typedef struct tagMHEG_Context_Attribute_t
{
	HUINT32							ulSessionId;			/* Session Id */
	HINT32							nDefSvcUid;			/* default Service handle */
	HINT32							nCurSvcUid;			/* current service Handle */
	redkey_state_t					eStatus;				/* mheg status defined at ITK */
	redkey_mheg_app_state_t			eAppStatus;
	HBOOL							bMhegAvailable;

	itk_service_tune_event_t			eTuneEvent;			/* Tuning action originated from HOST or Redkey */
	HUINT32							nTuneFlag;			/* itk_service_tune_flag_t */

	HINT32							nCurVideoPid;			/* current AV PIDs which are different with default Pid for Default service */
	HINT32							nCurAudioPid;
	HINT32							nCurPcrPid;
	DxVideoCodec_e					eCurVideoCodec;
	DxAudioCodec_e					eCurAudioCodec;

	// ITK_DIsplay_State_t				displayAVStatus;		/* current image, audioclip of display is visible or not */
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HUINT8							avMode;				/* AV mode status for media (e.g Interaction Channel Streaming) */
	HUINT8							avStatus;				/* AV mode status for media (e.g Interaction Channel Streaming) */
	Handle_t							hPrevSvc;			/* Previous service handle before performing deep link */
	HINT32							nDefaultAudioIndex;
	HINT32							nDefaultVideoIndex;
#endif
	HBOOL							imageDisplayFlag;
	ITK_ImageHandle_t*				hImage;

	HCHAR							*pMainContentUri;		/* Main Content URL */
	HBOOL							bPlayAndSeek;

	HBOOL							bSubtitleEnableStatus;	/* subtitle flag by ITK */
	HBOOL							bSubtitleEnableByVideo;/* refer to MHEG_MGR_SYNC_VIDEO_SUBTITLE define */

	// MW_MediaPlayer_t					*player;
	ITK_AvstreamHandle_t				*pstViewStreamHandler;
	// AP_MPlayerVerifyExternalCallback_t	*cbTlsCertificate;

	// HBOOL							bMHEGStopByHost;
}ITK_Context_Att_t;

typedef struct tagMheg_Hide_t
{
#if defined(CONFIG_MW_MM_PVR)
	HBOOL		bTrickEntering;
	HBOOL		bTrickExiting;
#endif
	HBOOL		bQuiteTuneMode;
	HINT32		nLastMasterSvcUid;
	HINT32		nLastSuppleSvcUid;
}ITK_Hide_t;

typedef struct tagMHEG_ITKContext_t
{
	redkey_profile_t			eItkProfile;

	itk_service_tune_event_t	eTuneEvent;				/* Tuning action originated from HOST or Redkey */
	HUINT32					nTuneFlag;				/* itk_service_tune_flag_t */

	HBOOL					bIsReadyAV;
	HBOOL					bIsReadyPMT;

	HUINT32					ulViewId;				/* View Id */
	ITK_Context_Att_t			stAtt;					/* Attributes of each view id : <for furture> view id 에 따른 속성이 필요할 경우에 확장해야 함 */
	ITK_Hide_t				stHide;
} MHEG_ITKContext_t;

/*******************************************************************/
/********************      variable        *************************/
/*******************************************************************/
static MHEG_ITKContext_t	s_stItkContext;
static PostMsgCB_t			s_pfnPostMsgCallback;
static HCHAR				s_szItkVersion[ITK_VERSION_INFO_MAX];

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static HUINT32				s_bMhegSbtlEnable = TRUE;
static ITK_AvstreamHandle_t *s_pstAvStreamHandlerList[MAX_DOWNLOAD_SESSION_NUM] = {0,};
static HxList_t				*s_pDownloadList = NULL;
#endif

static void  				*g_pvCurAvstream_V = NULL;
static void  				*g_pvCurAvstream_A = NULL;

static HUINT8				szRctIconUrl[DxRCT_MAX_IMAGE_ICON_URL_LEN];

static HBOOL				s_bBroadCastInterruptFlag = FALSE;
static HBOOL				s_bBlockMheg = FALSE;
static PxSTATUS_e			s_ePapiStatus = ePAMA_STATUS_Undefined;
static HUINT32				s_ulRctLinkCount = 0;
static HUINT32				s_ulCurPlayIdx =ICS_NO_PLAY;
static HBOOL				s_bChangeAVwithTune = FALSE; /* appication 재 진입을 막기 위한 flag. context를 유지한 상태로, tuning을 할 때 */

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
static HERROR	mheg_itk_Key2RedKeyCode(HINT32 symbol, HINT32 code, HUINT32 *pulKeyCode);
static HERROR	mheg_itk_sendKeyToClient(HINT32 key_symbol, HINT32 key_code);
static HERROR	mheg_itk_sendKeyToHost(HINT32 key_symbol, HINT32 key_code);
static void	mheg_itk_ChangeSubtitleStatus(HUINT32 nViewId,HBOOL bEnable);
static void	mheg_itk_ControlSubtitle(HUINT32 nViewId, HBOOL bEnable);
static void	mheg_itk_RetryStartMhegAppl(HUINT32 ulViewId, HINT32 nSleepTime, HINT32 nMsgSleepTime, HINT32 nMsgRetryCnt);
static void	mheg_itk_RestartMhegAppl(HUINT32 ulViewId);
static void	mheg_itk_StopMhegAppl(HUINT32 ulViewId, itk_service_tune_event_t eTuneEvent);
static MHEG_START_ERR_CODE_t mheg_itk_StartMhegAppl(void);
static HERROR	mheg_itk_NotifyTableReceived(HINT32 nViewId, SiTableType_t eSiTableType);
#if defined (CONFIG_MW_MM_PVR)
static HBOOL	mheg_itk_IsRecording(Handle_t hCurSvc);
#endif
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
static HERROR	mheg_itk_avsSetContextAvMode(HINT32 nViewId, HUINT8 avMode);
static void	mheg_itk_avsSetPosition(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t position);
static HERROR	mheg_itk_avsCreateStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler);
static HERROR	mheg_itk_avsDestroyStreamingAv(ITK_AvstreamHandle_t *pstStreamHandler);
static HERROR	mheg_itk_avsPlayStreamingAVRequestProbe(ITK_AvstreamHandle_t *pstStreamHandler, HUINT32 ulStartPos);
static HERROR	mheg_itk_avsStopStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler);
static HERROR	mheg_itk_avsPauseStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler);
static HERROR	mheg_itk_avsResumeStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler);
static HERROR	mheg_itk_avsControlStreamingSubtitle(HINT32 nViewId, HBOOL bOn);
static HERROR	mheg_itk_avsSetPlayTimeStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler, HUINT32 ulTime);
static void	mheg_itk_avsEofPositionStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler);
static HUINT8	mheg_itk_avsGetStreamingAvStatus(ITK_AvstreamHandle_t *pstStreamHandler);
static void	mheg_itk_avsSetStreamingAvStatus(ITK_AvstreamHandle_t *pstStreamHandler, HUINT8	avStatus);
static HBOOL	mheg_itk_avsIsSettingStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler);
static HBOOL	mheg_itk_avsIsSetupStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler);
static HBOOL	mheg_itk_avsIsPlayingStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler);
static HERROR	mheg_itk_avsRemoveStreamHandler(ITK_AvstreamHandle_t *pstStreamHandler);
static HERROR	mheg_itk_avsAddStreamHandler(ITK_AvstreamHandle_t *pstStreamHandler);
static ITK_AvstreamHandle_t* mheg_itk_avsFindStreamHandlerByDownloadHandle(Handle_t hDownload);
static HERROR	mheg_itk_avsAddDownloadQueue(ITK_AvstreamHandle_t *pstStreamHandler);
static HERROR	mheg_itk_avsStartDownload(ITK_AvstreamHandle_t *pstStreamHandler);
static HERROR	mheg_itk_avsRemoveDownloadQueue(ITK_AvstreamHandle_t *pstStreamHandler);
static HERROR	mheg_itk_avsDiscardDownloadBuffer(void);
static HERROR mheg_itk_avsChangeVideoTrack(ITK_AvstreamHandle_t *pstStreamHandler, HINT32 nComponentIndex);
static HERROR mheg_itk_avsChangeAudioTrack(ITK_AvstreamHandle_t *pstStreamHandler, HINT32 nComponentIndex);
static HERROR mheg_itk_avsGetSelectedAudioComponent(ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t	nComponentIndex, OxMediaPlay_Component_t *pstAudioComponet);
static HERROR mheg_itk_avsGetSelectedVideoComponent(ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t	nComponentIndex, OxMediaPlay_Component_t *pstVideoComponet);
static void	mheg_itk_NotifiyNetworkStatus(HBOOL bForce);
#endif


#if defined(CONFIG_DEBUG)
STATIC const HINT8 * mheg_itk_MakeStrMhegMsgEvent(MHEG_MSG_e eMsgEvent)
{
	switch (eMsgEvent)
	{
		ENUM_TO_STR(eMHEG_ENGINE_START);
		ENUM_TO_STR(eMHEG_NOTIFY_STATUS);
		ENUM_TO_STR(eMHEG_NOTIFY_APPSTATUS);
		ENUM_TO_STR(eMHEG_RESTART_APPLICATION);
		ENUM_TO_STR(eMHEG_DISPLAY_SUBTITLE_CONTROL);
		ENUM_TO_STR(eMHEG_DISPLAY_IMAGE_RELEASE);
		ENUM_TO_STR(eMHEG_DISPLAY_IMAGE_SHOW);
		ENUM_TO_STR(eMHEG_DISPLAY_IMAGE_HIDE);
		ENUM_TO_STR(eMHEG_HTTP_REGISTER_CALLBACK);
		ENUM_TO_STR(eMHEG_AVSTREAM_PRELOAD);
		ENUM_TO_STR(eMHEG_AVSTREAM_PLAY);
		ENUM_TO_STR(eMHEG_AVSTREAM_STOP);
		ENUM_TO_STR(eMHEG_AVSTREAM_RELEASE);
		ENUM_TO_STR(eMHEG_AVSTREAM_SELECT_AUDIO);
		ENUM_TO_STR(eMHEG_AVSTREAM_SELECT_VIDEO);
		ENUM_TO_STR(eMHEG_AVSTREAM_DESELECT_AUDIO);
		ENUM_TO_STR(eMHEG_AVSTREAM_DESELECT_VIDEO);
		ENUM_TO_STR(eMHEG_AVSTREAM_SET_SPEED);
		ENUM_TO_STR(eMHEG_AVSTREAM_SET_TRIGGER);
		ENUM_TO_STR(eMHEG_AVSTREAM_CLEAR_TRIGGER);
		ENUM_TO_STR(eMHEG_AVSTREAM_SET_POSITION);
		ENUM_TO_STR(eMHEG_AVSTREAM_SET_END_POSITION);
		ENUM_TO_STR(eMHEG_AVSTREAM_DOWNLOAD_START);
		ENUM_TO_STR(eMHEG_AVSTREAM_DOWNLOAD_COMPLETE);
		ENUM_TO_STR(eMHEG_AVSTREAM_DOWNLOAD_FAIL);
		ENUM_TO_STR(eMHEG_AVSTREAM_DOWNLOAD_PROGRESS);
		ENUM_TO_STR(eMHEG_AVSTREAM_DOWNLOAD_PROBE_SUCCESS);
		ENUM_TO_STR(eMHEG_AVSTREAM_DOWNLOAD_PROBE_FAILED);
		ENUM_TO_STR(eMHEG_AVSTREAM_DOWNLOAD_INFO_CHANGED);
		ENUM_TO_STR(eMHEG_NETWORK_EVENT);
		ENUM_TO_STR(eMHEG_DEMUX_SITABLE_NOTIFY);
		ENUM_TO_STR(eMHEG_MISC_PROMPT_FOR_GUIDANCE);
		ENUM_TO_STR(eMHEG_MISC_LAUNCH_APPLICATION);
		ENUM_TO_STR(eMHEG_DSMCCFS_FILE_DATA);
		ENUM_TO_STR(eMHEG_SERVICE_CHANGE_SVC);
		ENUM_TO_STR(eMHEG_APK_SVCCHANGED);
		ENUM_TO_STR(eMHEG_APK_SVCSTOPPED);
		ENUM_TO_STR(eMHEG_APK_AVSTARTED);
		ENUM_TO_STR(eMHEG_APK_AVSTOPPED);
		ENUM_TO_STR(eMHEG_APK_AVRESUME);
		ENUM_TO_STR(eMHEG_APK_AVUNDERRUN);
		ENUM_TO_STR(eMHEG_APK_MONITORSTARTED);
		ENUM_TO_STR(eMHEG_APK_MONITORSTOPPED);
		ENUM_TO_STR(eMHEG_APK_DMXCHANGED);
		ENUM_TO_STR(eMHEG_APK_PMTCHANGED);
		ENUM_TO_STR(eMHEG_APK_NOSIGNAL);
		ENUM_TO_STR(eMHEG_APK_LOCKED);
		ENUM_TO_STR(eMHEG_APK_SUBTITLECHANGED);
		ENUM_TO_STR(eMHEG_APK_VIDEO_STATUS);
		ENUM_TO_STR(eMHEG_APK_PFG_RESULT);
		ENUM_TO_STR(eMHEG_APK_SET_BROADCAST_INTERRUPTION);
		ENUM_TO_STR(eMHEG_APK_DSMCCFS_REQUEST_FILE);
		ENUM_TO_STR(eMHEG_APK_AUDIOCLIPCOMPLETED);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_STARTED);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_PLAYTIME);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_PLAYSTATE);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_PLAYERROR);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_MEDIA_BufferStateChanged);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_MEDIA_BufferInfoChanged);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_VIDEOSTREAMINFO);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_STOPPED);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_SPEED_CHANGED);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_SupportedSpeedChanged);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_MEDIA_PumpIndexChanged);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_MEDIA_SeekReady);
		ENUM_TO_STR(eMHEG_APK_MEDIA_EVENT_PlayInfo);
#endif
		ENUM_TO_STR(eMHEG_APK_FACTORY_DEFAULT);
		ENUM_TO_STR(eMHEG_APK_SVCLISTUPDATED);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		ENUM_TO_STR(eMHEG_SI_IP_STREAMING_PMT);
#endif
		ENUM_TO_STR(eMHEG_KEY_DOWN);
		ENUM_TO_STR(eMHEG_ENGINE_START_RETRY);
		ENUM_TO_STR(eMHEG_APK_RCTCHANGED);
		ENUM_TO_STR(eMHEG_SI_PMTCHANGED);
		ENUM_TO_STR(eMHEG_NOTIFY_APPSTATUS_CHANGED);
		ENUM_TO_STR(eMHEG_UNKNOWN);
	}
	return ("Unknown");

}
#endif

static HERROR	mheg_itk_initContext(void)
{
	HUINT32		ulViewId =0, ulSessionId =0, ulRequestId =0 ;

	HxSTD_MemSet(&s_stItkContext, 0, sizeof(MHEG_ITKContext_t));

	s_stItkContext.stAtt.eStatus			= REDKEY_UNINITIALISED;
	s_stItkContext.stAtt.eAppStatus		= REDKEY_MHEG_APP_NOT_RUNNING;
	s_stItkContext.stAtt.bMhegAvailable	= FALSE;
	s_stItkContext.stAtt.eTuneEvent		= ITK_SERVICE_TUNE_STOP_HOST;
	s_stItkContext.eTuneEvent			= ITK_SERVICE_TUNE_STOP_HOST;

	s_stItkContext.stAtt.nCurVideoPid 	= PID_NULL;
	s_stItkContext.stAtt.nCurAudioPid 	= PID_NULL;
	s_stItkContext.stAtt.nCurPcrPid		= PID_NULL;
	s_stItkContext.stAtt.eCurVideoCodec	= eDxVIDEO_CODEC_UNKNOWN;
	s_stItkContext.stAtt.eCurAudioCodec	= eDxAUDIO_CODEC_UNKNOWN;

	s_stItkContext.stAtt.bSubtitleEnableByVideo = FALSE;

	s_stItkContext.stHide.nLastMasterSvcUid	= 0;
	s_stItkContext.stHide.nLastSuppleSvcUid	= 0;
	s_stItkContext.stHide.bQuiteTuneMode 	= FALSE;

	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	APK_MEDIA_PLAY_GetSessionId(ulViewId, &ulSessionId);
	APK_MEDIA_PLAY_GetRequestId(ulViewId, eOxMP_MEDIATYPE_LIVE, &ulRequestId);

	s_stItkContext.ulViewId 			= ulViewId;
	s_stItkContext.stAtt.ulSessionId		= (HINT32)ulSessionId;
	s_stItkContext.stAtt.nDefSvcUid		= (HINT32)ulRequestId;
	s_stItkContext.stAtt.nCurSvcUid		= (HINT32)ulRequestId;

	s_stItkContext.bIsReadyAV			= FALSE;
	s_stItkContext.bIsReadyPMT			= FALSE;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	s_stItkContext.stAtt.avMode				= MHEG_AV_MODE_BROADCAST;
	s_stItkContext.stAtt.hPrevSvc			= HANDLE_NULL;
	s_stItkContext.stAtt.pMainContentUri 	= NULL;
	s_stItkContext.stAtt.pstViewStreamHandler	= NULL;
#endif

	PORT_ITK_SetSvcUid(ulViewId, ulSessionId, (HINT32)ulRequestId, (HINT32)ulRequestId);

	return ERR_OK;
}

static MHEG_ITKContext_t*	mheg_itk_getContext(void)
{
	return &s_stItkContext;
}

static HERROR	mheg_itk_registerEngineProfile(MHEG_EngineProfile_e *peEngineProfile, redkey_profile_t *pItkProfile, HINT32 *pnItkMemSize)
{
	MHEG_EngineProfile_e	eEngineProfile = MHEG_ENGINE_PROFILE_NONE;
	redkey_profile_t		eItkProfile = REDKEY_PROFILE_NULL;
	HINT32				nItkMemSize;

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

#if defined(CONFIG_3RD_MHEG_UKFREESAT_PROFILE)
	eEngineProfile 	|= MHEG_ENGINE_PROFILE_UK_FSAT;
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

	if (eEngineProfile == MHEG_ENGINE_PROFILE_NONE)
	{
		HxLOG_Error("[%s:%d] Can't find mheg engine profile...\n", __FUNCTION__, __LINE__);
		HxLOG_Assert(0);
	}

	/* set memory size */
	if (eEngineProfile & (MHEG_ENGINE_PROFILE_UK | MHEG_ENGINE_PROFILE_NZ))
	{
		if (eItkProfile == REDKEY_PROFILE_UK_SD)
			nItkMemSize = 4 * 1024 * 1024;
		else
			nItkMemSize = 18 * 1024 * 1024;
	}
	else if (eEngineProfile & MHEG_ENGINE_PROFILE_UK_FSAT)
	{
		nItkMemSize = 8 * 1024 * 1024;	// same as FOXSAT. To be optimize (5MB -> 8MB)
	}
	else if (eEngineProfile & MHEG_ENGINE_PROFILE_AUSTRALIA)
	{
		/*
			2010.05.28. 6.2.4.D항목의 application text를 동작시키려면 아래 size필요
		*/
		nItkMemSize = 20 * 1024 * 1024; //16 * 1024 * 1024;	// 호주향은 MHEG EPG를 위해 memory를 많이(16M) 필요로 한다
	}
	else if (eEngineProfile & MHEG_ENGINE_PROFILE_CI_PLUS)
		nItkMemSize =  4 * 1024 * 1024;
	else
		nItkMemSize = 16 * 1024 * 1024;

	s_stItkContext.eItkProfile = eItkProfile;

	PORT_ITK_SetEngineProfile(eEngineProfile);
	PORT_ITK_SetItkMemSize(nItkMemSize);

	*peEngineProfile = eEngineProfile;
	*pItkProfile = eItkProfile;
	*pnItkMemSize = nItkMemSize;

	return ERR_OK;
}

static void	mheg_itk_callbackRedKeyState(void *context, redkey_state_t state)
{
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"viewId(0x%x) -> change RedKey State to 0x%x [0:uninit, 1:stop, 2:start, 3:lost]"C_CODE_RESET"\n", s_stItkContext.ulViewId, state);

	if (s_stItkContext.stAtt.eStatus == state)
	{
		HxLOG_Debug("Skip Same Redkey state!\n");
		return;
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"RedKey State : %x -> %x  "C_CODE_RESET"\n",  s_stItkContext.stAtt.eStatus, state);

	s_stItkContext.stAtt.eStatus = state;

	PORT_ITK_SetMhegRunningStatus((state == REDKEY_RUNNING) ? TRUE : FALSE);

	// 목적에 따라 사용 : s_pfnPostMsgCallback(eMHEG_NOTIFY_STATUS, 0, state, 0, 0, 0, NULL, 0);
}

static void	mheg_itk_callbackRedkeyAppState(void *context, redkey_mheg_app_state_t appState)
{
	OxMediaPlay_MhegStatus_e  eOmMhegStatus = eMhegStatus_None ;

	HxLOG_Debug("\t"C_CODE_F_BLACK_B_GREEN"viewId(0x%x) -> change RedKey App State to 0x%x [0:not running, 1:starting, 2:running, 3:stopping]"C_CODE_RESET"\n", s_stItkContext.ulViewId, appState);

	if (s_stItkContext.stAtt.eAppStatus == appState)
	{
		HxLOG_Debug("Skip Same Redkey App state!\n");
		return;
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"RedKey App Status : %x -> %x  "C_CODE_RESET"\n", s_stItkContext.stAtt.eStatus, appState);

	s_stItkContext.stAtt.eAppStatus = appState;

	if(s_stItkContext.stAtt.eAppStatus == REDKEY_MHEG_APP_NOT_RUNNING)
		eOmMhegStatus = eMhegStatus_NotRunning;
	else if(s_stItkContext.stAtt.eAppStatus == REDKEY_MHEG_APP_STARTING)
		eOmMhegStatus = eMhegStatus_Starting;
	else if(s_stItkContext.stAtt.eAppStatus == REDKEY_MHEG_APP_RUNNING)
		eOmMhegStatus = eMhegStatus_Running;
	else if(s_stItkContext.stAtt.eAppStatus == REDKEY_MHEG_APP_STOPPING)
		eOmMhegStatus = eMhegStatus_Stopping;
	else
		eOmMhegStatus = eMhegStatus_None;

	PORT_ITK_PostMsg(eMHEG_NOTIFY_APPSTATUS_CHANGED, 0, eOmMhegStatus, 0, 0,0);

	HxLOG_Debug("\t"C_CODE_F_BLACK_B_GREEN"[%s, %d]  eOmMhegStatus : %x  [0:None, 1:NotRunning, 2:Starting, 3:Running, 4:Stopping]"C_CODE_RESET"\n\n",
		__FUNCTION__, __LINE__, eOmMhegStatus);

	s_pfnPostMsgCallback(eMHEG_NOTIFY_APPSTATUS, 0, appState, 0, 0, 0, NULL, 0);
}

static void mheg_itk_changeAV(HUINT32 ulViewId, DxVideoCodec_e eVideoCodec, HINT32 nVideoPid, DxAudioCodec_e eAudioCodec, HINT32 nAudioPid, HINT32 nPcrPid)
{
	HBOOL	bSubtitleEnable;

	/* 	MHEG Profile Section 6, 6.3.5B, 6.3.5C,
		Video가 멈추면 (Video PID가 0이면) Subtitle을 멈춘다.
	*/
	bSubtitleEnable = (nVideoPid == PID_NULL) ? FALSE : TRUE;

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"bSubtitleEnable (%d)"C_CODE_RESET"\n", bSubtitleEnable);

	if (bSubtitleEnable)
	{
		if(s_stItkContext.stAtt.bSubtitleEnableStatus)
		{
			mheg_itk_ControlSubtitle(ulViewId, TRUE);
		}
	}
	else
	{
		mheg_itk_ControlSubtitle(ulViewId, FALSE);
	}

	APK_MEDIA_PLAY_SetTempAvPid(ulViewId, eVideoCodec, nVideoPid, eAudioCodec, nAudioPid, nPcrPid);
}

/* IC Stream을 재생하고 나면, live action이 죽어버려 changeAV로 av 변경이 되지 않다.
    즉, IC Stream 재생후 Live 방송이 나오지 않는 문제 발생.
    아래 함수는 IC Stream을 재생 후 av 변경시, Tune을 하고 av 변경을 하기 위해 만든 함수이다.
*/
static HERROR mheg_itk_changeAVwithTune(HUINT32 nViewId, HINT32 nCurSvcUid)
{
	HINT32 nSvcUid				= nCurSvcUid;
	HBOOL bQuiet 				= TRUE ;
	HBOOL bNdt 					= FALSE;
	HBOOL bNoBanner				= TRUE;
	OxMediaPlay_TuningMode_e	eTuneMode;
	OxMediaPlay_StartInfo_t 	unStart;
	HUINT32						ulSessionId = 0;

	HxLOG_Debug("=======(+) \n");

	eTuneMode = (bNoBanner == TRUE) ? eOxMP_TUNEMODE_NO_IPLATE|eOxMP_TUNEMODE_FORCE_TUNE : eOxMP_TUNEMODE_FORCE_TUNE;
	eTuneMode = (bNdt == TRUE) ? eOxMP_TUNEMODE_NDT_TUNE|eTuneMode : eTuneMode;

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Request change Service"C_CODE_RESET"\n");
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"nSvcUid(0x%x) , bQuiet(%d)  bNdt(%d) , bNoBanner(%d) eTuneMode(0x%x)"C_CODE_RESET"\n", nSvcUid, bQuiet, bNdt, bNoBanner, eTuneMode);

	unStart.stLive.eType				= eOxMP_MEDIATYPE_LIVE;
	unStart.stLive.eLiveType			= eOxMP_LIVETYPE_MAIN;
	unStart.stLive.eTuneMode			= eTuneMode;
	unStart.stLive.ulMasterSvcUid		= nSvcUid;
	unStart.stLive.ulSuppleSvcUid		= nSvcUid;
	unStart.stLive.eSuppSvcType			= (bQuiet == TRUE) ? eOxMP_SUPPLESVCTYPE_SilentTune : eOxMP_SUPPLESVCTYPE_Normal;

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"nSvcUid(0x%x) eTuneMode(0x%x)  eSuppSvcType(0x%x)"C_CODE_RESET"\n", nSvcUid, eTuneMode, unStart.stLive.eSuppSvcType);

	if(bQuiet == TRUE)
	{
		if(!(eTuneMode & eOxMP_TUNEMODE_NDT_TUNE))
		{
			HERROR hErr = ERR_FAIL;

			hErr = APK_MEDIA_PLAY_Start(nViewId, &unStart, &ulSessionId);
			HxLOG_Debug("APK_MEDIA_PLAY_Start  hErr(0x%x) \n", hErr);
			if (hErr == ERR_OK)
			{
				HxLOG_Debug("=======(-) \n");
				return ERR_OK;
			}
		}
	}

	HxLOG_Debug("=======(-) \n");
	return ERR_FAIL;
}

static HBOOL mheg_itk_IsRunning(void)
{
	return (s_stItkContext.stAtt.eStatus == REDKEY_RUNNING) ? TRUE : FALSE;
}

static HBOOL mheg_itk_IsAvailable(void)
{
	return s_stItkContext.stAtt.bMhegAvailable;
}

static HERROR mheg_itk_SetContextCurPesInfo(HINT32 nVideoPid, HINT32 nAudioPid, HINT32 nPcrPid, DxVideoCodec_e eVideoCodec, DxAudioCodec_e eAudioCodec)
{
	HxLOG_Debug("Set Context PID:V/A/P(0x%x, 0x%x, 0x%x), Type:V/A(%d, %d)\n", nVideoPid, nAudioPid, nPcrPid, eVideoCodec, eAudioCodec);

	if ((nVideoPid != 0) && (nVideoPid != PID_CURRENT))
	{
		s_stItkContext.stAtt.nCurVideoPid	= nVideoPid;
	}
	if ((nAudioPid != 0) && (nAudioPid != PID_CURRENT))
	{
		s_stItkContext.stAtt.nCurAudioPid	= nAudioPid;
	}
	if ((nPcrPid != 0) && (nPcrPid != PID_CURRENT))
	{
		s_stItkContext.stAtt.nCurPcrPid = nPcrPid;
	}
	if (eVideoCodec)
	{
		s_stItkContext.stAtt.eCurVideoCodec = eVideoCodec;
	}
	if (eAudioCodec)
	{
		s_stItkContext.stAtt.eCurAudioCodec = eAudioCodec;
	}

	PORT_ITK_SetMasterMhegPesInfo(nVideoPid, nAudioPid, nPcrPid, (HUINT8)eVideoCodec, (HUINT8)eAudioCodec);

	s_stItkContext.stAtt.bSubtitleEnableByVideo = (nVideoPid == PID_NULL) ? FALSE : TRUE;

	return ERR_OK;
}

static HERROR mheg_itk_GetContextCurPesInfo(HINT32* pnVideoPid, HINT32* pnAudioPid, HINT32* pnPcrPid, DxVideoCodec_e* peVideoCodec, DxAudioCodec_e* peAudioCodec)
{
	*pnVideoPid		= s_stItkContext.stAtt.nCurVideoPid;
	*pnAudioPid		= s_stItkContext.stAtt.nCurAudioPid;
	*pnPcrPid		= s_stItkContext.stAtt.nCurPcrPid;
	*peVideoCodec	= s_stItkContext.stAtt.eCurVideoCodec;
	*peAudioCodec	= s_stItkContext.stAtt.eCurAudioCodec;

	return ERR_OK;
}

static HERROR mheg_itk_GetContextSvcHandle(HINT32* pnDefSvcUid, HINT32* pnCurSvcUid)
{
	*pnDefSvcUid = s_stItkContext.stAtt.nDefSvcUid;
	*pnCurSvcUid = s_stItkContext.stAtt.nCurSvcUid;

	return ERR_OK;
}

static HERROR mheg_itk_SetContextCurSvcUid(HINT32 nCurSvcUid)
{
	s_stItkContext.stAtt.nCurSvcUid = nCurSvcUid;

	PORT_ITK_SetMastCurService(nCurSvcUid);

	return ERR_OK;
}

static void mheg_itk_avsEventCallback(ITK_AvstreamHandle_t *pstStreamHandler, itk_media_event_t evt, itk_media_event_data_t *pMediaEvt)
{
	if (pstStreamHandler && pstStreamHandler->callback)
	{
		HxLOG_Debug(" pstStreamHandler(%p) event(%d)[0=PRE, 1=PLAY, 2=STOP, 3=TRI, 4=STA, 5=RES, 6=U_S]\n", pstStreamHandler, evt);
		pstStreamHandler->callback(pstStreamHandler->callback_context, (itk_avstream_t*)pstStreamHandler, evt, pMediaEvt);
	}
}

static void	*mheg_itk_avsGetCurrentHandleV(void)
{
	return g_pvCurAvstream_V;
}

static void	mheg_itk_avsSetCurrentHandleV(void *pvCurAvstream)
{
	g_pvCurAvstream_V = pvCurAvstream;
}

static void	*mheg_itk_avsGetCurrentHandleA(void)
{
	return g_pvCurAvstream_A;
}

static void	mheg_itk_avsSetCurrentHandleA(void *pvCurAvstream)
{
	g_pvCurAvstream_A = pvCurAvstream;
}

static void	*mheg_itk_avsGetCurHandle(void)
{
	// If the presented audio and video components are from different services
	// the service referencing the video components takes precedence.
	return (g_pvCurAvstream_V == NULL) ? g_pvCurAvstream_A : g_pvCurAvstream_V ;
}

static HERROR mheg_itk_avsPlayStream(struct ITK_AvstreamHandle_t* pstStreamHandler, HUINT32 ulStartPosition)
{
	HINT32			nCurVideoPid = PID_NULL, nCurAudioPid = PID_NULL, nCurPcrPid = PID_NULL;
	HINT32			nVideoPid = PID_CURRENT, nAudioPid = PID_CURRENT, nPcrPid = PID_CURRENT ;
	DxVideoCodec_e	eCurVideoCodec = eDxVIDEO_CODEC_UNKNOWN, eVideoCodec = MHEG_CODEC_CURRENT;
	DxAudioCodec_e	eCurAudioCodec = eDxAUDIO_CODEC_UNKNOWN, eAudioCodec = MHEG_CODEC_CURRENT;
	HERROR			ulErr = ERR_OK;
	HUINT32		ulViewId = 0;

	HxLOG_Debug("===== (+) \n");

	if(PORT_ITK_GetMhegInternalRunningStatus() == FALSE)
	{
		HxLOG_Error("Redkey Engine should not be running!\r\n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error(" ERROR : input parameter is NULL\n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	ulViewId = pstStreamHandler->ulViewId;

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"pstStreamHandler : %p, ulViewId(%d) "C_CODE_RESET"\n",  pstStreamHandler, ulViewId);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	if (mheg_itk_avsIsSettingStreamingAV(pstStreamHandler) == TRUE)
	{
		if (port_itk_avs_GetHandle(pstStreamHandler) == NULL)
		{
			HxLOG_Error("Already released !\n");
			return ERR_FAIL;
		}
	}
#endif

	/* get current AV Pes information from Context */
	mheg_itk_GetContextCurPesInfo(&nCurVideoPid, &nCurAudioPid, &nCurPcrPid, &eCurVideoCodec, &eCurAudioCodec);

	HxLOG_Debug(C_CODE_F_WHITE_B_BLUE" [CurPes] V/A/PCR PID(0x%x/0x%x/0x%x) VideoCodec(0x%x) AudioCodec(0x%x)"C_CODE_RESET"\n",
					nCurVideoPid, nCurAudioPid, nCurPcrPid, eCurVideoCodec, eCurAudioCodec);
	HxLOG_Debug(C_CODE_F_WHITE_B_BLUE"[AvStream(%p)] V/A/PCR PID(0x%x/0x%x/0x%x) V/A Component(0x%lx/0x%lx) V/A Status(%d/%d) V/A type(%d/%d) mediaStatus(0x%x) eStreamRef(%d)"C_CODE_RESET"\n",
					pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus,
					pstStreamHandler->videoType, pstStreamHandler->audioType, pstStreamHandler->mediaStatus, pstStreamHandler->eStreamRef);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HxLOG_Debug("avMode(%d)\n", pstStreamHandler->avMode);
#endif

	if((pstStreamHandler->mediaStatus & 0x0F) != ITK_DISPLAY_AV_STATUS_PLAYING)
	{
		if (pstStreamHandler->videoComponent == 0 && pstStreamHandler->audioComponent == 0) // select video/audio 없이 play 되는 경우 (section6 : 6.6, 6.8.2B, 6.8.2C)
		{
			if (nCurVideoPid != pstStreamHandler->videoPid	|| nCurAudioPid != pstStreamHandler->audioPid)
			{
				pstStreamHandler->videoComponent = -1;
				pstStreamHandler->audioComponent = -1;
			}
		}

		/* set video pid */
		if (pstStreamHandler->videoComponent)
		{
			if((nCurVideoPid != pstStreamHandler->videoPid) ||
				(pstStreamHandler->videoComponent == -1) ||
				((pstStreamHandler->videoComponent) && (nCurVideoPid == pstStreamHandler->videoPid) && (pstStreamHandler->videoStatus == FALSE || (pstStreamHandler->videoStatus == TRUE && (pstStreamHandler->mediaStatus & ITK_DISPLAY_AV_STATUS_PRELOAD)))))
			{
				nVideoPid = pstStreamHandler->videoPid;
				eVideoCodec = pstStreamHandler->videoType;
				if (nVideoPid != PID_NULL)
				{
					pstStreamHandler->videoStatus = TRUE; // refer to "Michael Kuegler [michael.kuegler@s-and-t.com]" comment as below
				}
			}
		}

		/* set audio pid */
		if (pstStreamHandler->audioComponent)
		{
			if((nCurAudioPid != pstStreamHandler->audioPid) ||
				(pstStreamHandler->audioComponent == -1) ||
				((pstStreamHandler->audioComponent) && (nCurAudioPid == pstStreamHandler->audioPid) && (pstStreamHandler->audioStatus == FALSE || (pstStreamHandler->audioStatus == TRUE && (pstStreamHandler->mediaStatus & ITK_DISPLAY_AV_STATUS_PRELOAD)))))
			{
				nAudioPid= pstStreamHandler->audioPid;
				eAudioCodec = pstStreamHandler->audioType;
				if (nAudioPid != PID_NULL)
				{
					pstStreamHandler->audioStatus = TRUE; // refer to "Michael Kuegler [michael.kuegler@s-and-t.com]" comment as below
				}

			}
		}

		/* set pcr pid */
		if (pstStreamHandler->pcrPid != PID_NULL && pstStreamHandler->pcrPid != 0)
		{
			nPcrPid = pstStreamHandler->pcrPid;
		}
		else
		{
			if (pstStreamHandler->videoPid != PID_NULL)
			{
				nPcrPid = pstStreamHandler->videoPid;
			}
			else if (nCurPcrPid != PID_NULL && nCurPcrPid != 0)
			{
				nPcrPid = nCurPcrPid;
			}
			else
			{
				nPcrPid = pstStreamHandler->audioPid;
			}
		}

		if(nVideoPid == PID_CURRENT && nAudioPid == PID_CURRENT)
		{
			/* in case there is no change in AV component --> skip */
			HxLOG_Debug("\tSkip AV Play!! [Cur] V/A PID (0x%x/0x%x) [Stream handler] V/A PID(0x%x/0x%x)\n",
								nCurVideoPid, nCurAudioPid, pstStreamHandler->videoPid, pstStreamHandler->audioPid);
			/* If audio, video and pcr pid is zero, return as ERR_OK without setting callback fn to play AV -- mhoh*/
			//return ERR_OK;
		}
		else
		{
			if (pstStreamHandler->videoComponent == 0 && pstStreamHandler->audioComponent == 0)
			{
				if (nVideoPid == PID_CURRENT)
				{
					nVideoPid = nCurVideoPid;
					eVideoCodec = eCurVideoCodec;
				}
				if (nAudioPid == PID_CURRENT)
				{
					nAudioPid = nCurAudioPid;
					eAudioCodec = eCurAudioCodec;
				}
			}

			/* change av track */
			if (nVideoPid != PID_NULL && nVideoPid != PID_CURRENT)
			{
				if (s_stItkContext.stAtt.imageDisplayFlag == TRUE)
				{
					//PORT_ITK_Display_StopIFrameByOther(hAction);
					HxLOG_Debug("\033[30;46m Need to implementation -  PORT_ITK_Display_StopIFrameByOther \033[0m\n");
				}
			}

			HxLOG_Debug("\tPlay stream : change pid --> hAction(0x%x), av,pcrPID(0x%x / 0x%x / 0x%x) av param(0x%x / 0x%x)\n",
							0, nAudioPid, nVideoPid, nPcrPid, (eAudioCodec << 16)|nAudioPid, (eVideoCodec << 16)|nVideoPid);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
			{
				if(pstStreamHandler->position == 0)
					pstStreamHandler->position = ulStartPosition;
				if (mheg_itk_avsIsSetupStreamingAV(pstStreamHandler) == TRUE)
					mheg_itk_avsPlayStreamingAVRequestProbe(pstStreamHandler, pstStreamHandler->position);
				else
					pstStreamHandler->autoStart = TRUE;
			}
			else
			{
				HERROR hErr = ERR_FAIL;
				OxMediaPlay_MediaType_e	eMediaType;

				s_ulCurPlayIdx = ICS_PLAY_LIVE;

				hErr = APK_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);
				if (hErr == ERR_OK && eMediaType == eOxMP_MEDIATYPE_LIVE)
				{
					mheg_itk_changeAV(ulViewId, eVideoCodec, nVideoPid, eAudioCodec, nAudioPid, nPcrPid);
				}
				else
				{
					//eOxMP_MEDIATYPE_PVRPLAYBACK, eOxMP_MEDIATYPE_TSRPLAYBACK 동작하지 않도록 수정..
					if(eMediaType ==  eOxMP_MEDIATYPE_NONE)
					{
						VK_TASK_Sleep(100);

						hErr = mheg_itk_changeAVwithTune(ulViewId, pstStreamHandler->svcUid);
						if(hErr == ERR_OK)
						{
							s_bChangeAVwithTune = TRUE;
						}
					}
				}
			}
#else

			mheg_itk_changeAV(ulViewId, eVideoCodec, nVideoPid, eAudioCodec, nAudioPid, nPcrPid);
#endif
			/* notify av track changed to ITK */
			mheg_itk_SetContextCurPesInfo(nVideoPid, nAudioPid, nPcrPid, pstStreamHandler->videoType, pstStreamHandler->audioType);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			mheg_itk_avsSetContextAvMode(ulViewId, pstStreamHandler->avMode);
#endif
		}

		if (nVideoPid != PID_NULL && nVideoPid != PID_CURRENT)
		{
			mheg_itk_avsSetCurrentHandleV((void*)pstStreamHandler);
		}
		if (nAudioPid != PID_NULL && nAudioPid != PID_CURRENT)
		{
			mheg_itk_avsSetCurrentHandleA((void*)pstStreamHandler);
		}

		if (pstStreamHandler->eStreamRef < eITK_REFSVC_FSI)
		{
			PORT_ITK_SetMastCurService(pstStreamHandler->svcUid);
		}
	}

	HLIB_STD_TaskSleep(0);	/* for rescheduling OS */

	HxLOG_Debug("videoStatus(%d) audioStatus(%d)\n", pstStreamHandler->videoStatus, pstStreamHandler->audioStatus);

	if (pstStreamHandler->videoStatus == TRUE || pstStreamHandler->audioStatus == TRUE)
	{
		pstStreamHandler->mediaStatus = (pstStreamHandler->mediaStatus & 0xF0) | ITK_DISPLAY_AV_STATUS_PLAYING;
		mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PLAYING, NULL);
	}

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		if (pstStreamHandler->avMode == MHEG_AV_MODE_BROADCAST)
		{
			//printf("\033[30;46m[%s : %d] Need to implementation -   \033[0m\n", __FUNCTION__, __LINE__);
		}
#endif

	return ulErr;
}

static HERROR mheg_itk_avsStopStream(ITK_AvstreamHandle_t* pstStreamHandler, HUINT8 ucIsTrackOption)
{
	HERROR		ulErr = ERR_OK;
	HUINT32 		ulViewId = 0;

	(void)ucIsTrackOption;		/* unused variable temprarly */

	HxLOG_Debug("=====(+) \n");

	if (pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is NULL\n" );
		HxLOG_Debug("=====(-) \n");
		return ERR_FAIL;

	}

	ulViewId = pstStreamHandler->ulViewId;

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"pstStreamHandler : %p, mediaStatus (0x%x)"C_CODE_RESET"\n", __FUNCTION__, __LINE__, pstStreamHandler, pstStreamHandler->mediaStatus);

	/* check media status */
	if(((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING) || ((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_CREATED))
	{
		HINT32				nCurVideoPid = PID_NULL, nCurAudioPid = PID_NULL, nCurPcrPid= PID_NULL;
		DxVideoCodec_e		eCurVideoCodec = eDxVIDEO_CODEC_UNKNOWN;
		DxAudioCodec_e		eCurAudioCodec = eDxAUDIO_CODEC_UNKNOWN;
		ITK_AvstreamHandle_t 	*prevAVS_V = (ITK_AvstreamHandle_t *)mheg_itk_avsGetCurrentHandleV();
		ITK_AvstreamHandle_t 	*prevAVS_A = (ITK_AvstreamHandle_t *)mheg_itk_avsGetCurrentHandleA();

		//printf("\tStream handler = %p, prevAVS_A/V (%p, %p)\n", pstStreamHandler, prevAVS_A, prevAVS_V);

		mheg_itk_GetContextCurPesInfo(&nCurVideoPid, &nCurAudioPid, &nCurPcrPid, &eCurVideoCodec, &eCurAudioCodec);

		HxLOG_Debug(C_CODE_F_WHITE_B_BLUE"[CurPES] V/A/PCR PID(0x%x/0x%x/0x%x) VideoCodec(0x%x) AudioCodec(0x%x)"C_CODE_RESET"\n",
						nCurVideoPid, nCurAudioPid, nCurPcrPid, eCurVideoCodec, eCurAudioCodec);
		HxLOG_Debug(C_CODE_F_WHITE_B_BLUE"[AvStream(%p)] V/A/PCR PID(0x%x/0x%x/0x%x) V/A Component(0x%lx/0x%lx) V/A Status(%d/%d) V/A type(%d/%d) mediaStatus(0x%x) eStreamRef(%d)"C_CODE_RESET"\n",
						pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus,
						pstStreamHandler->videoType, pstStreamHandler->audioType, pstStreamHandler->mediaStatus, pstStreamHandler->eStreamRef);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		HxLOG_Debug(C_CODE_F_WHITE_B_BLUE"avMode(%d)"C_CODE_RESET"\n", pstStreamHandler->avMode);
#endif

		if((pstStreamHandler == prevAVS_V) || ((prevAVS_V == NULL) && (pstStreamHandler == prevAVS_A)))
		{
			if (pstStreamHandler->bVideoFreeze == TRUE || pstStreamHandler->bAudioFreeze == TRUE)
			{
				APK_MEDIA_PLAY_SetVideoFreeze(ulViewId, FALSE);
				APK_MEDIA_PLAY_SetAudioFreeze(ulViewId, FALSE);
				pstStreamHandler->bVideoFreeze = FALSE;
				pstStreamHandler->bAudioFreeze = FALSE;
				HLIB_STD_TaskSleep(0);
			}
		}

		if (prevAVS_V == prevAVS_A)
		{
			if (((pstStreamHandler->videoPid == nCurVideoPid || pstStreamHandler->audioPid == nCurAudioPid) && (prevAVS_V == pstStreamHandler)) || !prevAVS_V)
			{
				/* change av track */
				HxLOG_Debug("Stop stream : change pid --> hAction(0x%x), videoPid(0x%x) audioPid(0x%x), pcrPid(0x%x)\n", 0, PID_NULL, PID_NULL, PID_NULL);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
				{
					if (mheg_itk_avsStopStreamingAV(pstStreamHandler) == ERR_OK)
					{
						//ICS009 027 029
						//pstStreamHandler->position = 0;
					}
				}
				else
				{
					s_ulCurPlayIdx = ICS_NO_PLAY;
					mheg_itk_changeAV(ulViewId, MHEG_CODEC_CURRENT, PID_NULL, MHEG_CODEC_CURRENT, PID_NULL, PID_NULL);
				}
#else

				mheg_itk_changeAV(ulViewId, MHEG_CODEC_CURRENT, PID_NULL, MHEG_CODEC_CURRENT, PID_NULL, PID_NULL);
#endif
				/* notify av track changed to ITK */
				mheg_itk_SetContextCurPesInfo(PID_NULL, PID_NULL, PID_NULL, MHEG_CODEC_CURRENT, MHEG_CODEC_CURRENT);
				pstStreamHandler->videoStatus = FALSE;
				pstStreamHandler->audioStatus = FALSE;
				ulErr = ERR_OK;
			}
		}
		else
		{
			HUINT16		usVideoPid = PID_CURRENT, usAudioPid = PID_CURRENT, usPcrPid = PID_CURRENT;

			if ((pstStreamHandler->videoPid == nCurVideoPid) && (!prevAVS_V || prevAVS_V == pstStreamHandler))
			{
				/* change av track */
				HxLOG_Debug("Stop stream Video: change pid --> hAction(0x%x), videoPid(0x%x) audioPid(0x%x), pcrPid(0x%x)\n", 0, PID_NULL, usAudioPid, usPcrPid);

				usVideoPid = PID_NULL;
				pstStreamHandler->videoStatus = FALSE;
				ulErr = ERR_OK;
			}

			if ((pstStreamHandler->audioPid == nCurAudioPid) && (!prevAVS_A || prevAVS_A == pstStreamHandler))
			{
				/* change av track */
				HxLOG_Debug("Stop stream Audio: change pid --> hAction(0x%x), videoPid(0x%x) audioPid(0x%x), pcrPid(0x%x)\n", 0, usVideoPid, PID_NULL, usPcrPid);
				usAudioPid = PID_NULL;
				pstStreamHandler->audioStatus = FALSE;
				ulErr = ERR_OK;
			}

			if (usVideoPid == PID_NULL && usAudioPid == PID_NULL)
			{
				if (pstStreamHandler->pcrPid == nCurPcrPid)
				{
					usPcrPid = PID_NULL;
				}
			}

			if (usVideoPid == PID_NULL || usAudioPid == PID_NULL)
			{
				HxLOG_Debug("Stop stream : change pid --> hAction(0x%x), av pcrPID(0x%x / 0x%x / 0x%x) av param(0x%x / 0x%x\n",
								0, usAudioPid, usVideoPid, usPcrPid, (MHEG_CODEC_CURRENT << 16)|usAudioPid, (MHEG_CODEC_CURRENT << 16)|usVideoPid);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
				{
					if (mheg_itk_avsStopStreamingAV(pstStreamHandler) == ERR_OK)
					{
						//ICS009 027 029
						//pstStreamHandler->position = 0;
					}
				}
				else
				{
					mheg_itk_changeAV(ulViewId, MHEG_CODEC_CURRENT, usVideoPid, MHEG_CODEC_CURRENT, usAudioPid, usPcrPid);
				}
#else
				mheg_itk_changeAV(ulViewId, MHEG_CODEC_CURRENT, usVideoPid, MHEG_CODEC_CURRENT, usAudioPid, usPcrPid);
#endif
				/* notify av track changed to ITK */
				mheg_itk_SetContextCurPesInfo(usVideoPid, usAudioPid, usPcrPid, MHEG_CODEC_CURRENT, MHEG_CODEC_CURRENT);
				pstStreamHandler->videoStatus = FALSE;
				pstStreamHandler->audioStatus = FALSE;
				ulErr = ERR_OK;
			}
		}
	}
	else
	{
		HxLOG_Debug("ERROR : Can not stop AV Stream, mediaStatus(0x%x)\n", pstStreamHandler->mediaStatus);
		ulErr = ERR_FAIL;
	}

	if(ulErr == ERR_OK)
	{
		pstStreamHandler->mediaStatus = ITK_DISPLAY_AV_STATUS_STOPPED;
		mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_STOPPED, NULL);

		// defined in oplvideobroadcasthandle_def.h
		HAPI_BroadcastSignal("signal:MhegEventNotify", 2, "ii", 34 /*OPLBROADCAST_EVENT_TYPE_MHEG_ICS_STATUS*/, FALSE);
	}

	HxLOG_Debug("=====(-) \n");

	return ulErr;
}

static void	mheg_itk_avsReleaseStream(ITK_AvstreamHandle_t *pstStreamHandler, ITK_RefSvc_t eStreamRef)
{
	ITK_AvstreamHandle_t *prevAVS_V = NULL;
	ITK_AvstreamHandle_t *prevAVS_A = NULL;

	HxLOG_Debug("=====(+) \n");

	if (pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler  NULL\r\n");
		HxLOG_Debug("=====(-) \n");
		return;
	}

	prevAVS_V = (ITK_AvstreamHandle_t *)mheg_itk_avsGetCurrentHandleV();
	prevAVS_A = (ITK_AvstreamHandle_t *)mheg_itk_avsGetCurrentHandleA();

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Stream handler (%p), eStreamRef (%d), prevAVS_V (%p), prevAVS_A (%p)"C_CODE_RESET"\n", pstStreamHandler, eStreamRef, prevAVS_V, prevAVS_A);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI)
	{
 		if((pstStreamHandler == prevAVS_V) || ((prevAVS_V == NULL) && (pstStreamHandler == prevAVS_A)))
		{
			mheg_itk_avsDestroyStreamingAv(pstStreamHandler);
		}

		mheg_itk_avsRemoveStreamHandler(pstStreamHandler);
		mheg_itk_avsRemoveDownloadQueue(pstStreamHandler);
	}
#endif

	if (pstStreamHandler == prevAVS_V)
	{
		mheg_itk_avsSetCurrentHandleV(NULL);
	}
	if (pstStreamHandler == prevAVS_A)
	{
		mheg_itk_avsSetCurrentHandleA(NULL);
	}

	HLIB_STD_MemFree(pstStreamHandler);
	pstStreamHandler = NULL;

	HxLOG_Debug("=====(-) \n");
}

static void	mheg_itk_avsPreloadStream(ITK_AvstreamHandle_t *pstStreamHandler)
{
	itk_media_event_data_t		stMediaEventData;
	itk_int32_t				preload = ITK_PRESENT;
	itk_int32_t				stream_length = -1;

	/**
	* Preloads an avstream
	*
	* Preloading causes the host to put the stream in a suitable state for "immediate"
	* presentation. This may mean connecting to a media server and buffering the stream
	* or dereferencing the stream reference to a stream object.
	*/

	HxLOG_Debug("=====(+) \n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is NULL \n");
		HxLOG_Debug("=====(-) \n");
		return;
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Stream handler (%p) -> eStreamRef(0x%x), VPid(0x%x), APid(0x%x), svcUid(0x%x), SvcId(0x%x) BufferPriority(0x%x) "C_CODE_RESET"\n",
					pstStreamHandler, pstStreamHandler->eStreamRef,
					pstStreamHandler->videoPid, pstStreamHandler->audioPid,
					pstStreamHandler->svcUid, pstStreamHandler->serviceId, pstStreamHandler->bufferPriority);

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
		if (mheg_itk_avsIsSettingStreamingAV(pstStreamHandler) == TRUE)
		{
			if (port_itk_avs_GetHandle(pstStreamHandler) == NULL)
			{
				HxLOG_Error("Already released !\n");
				preload = ITK_NOT_PRESENT;
				break;
			}
			/* ICS를 위한 media action을 이전 action으로 변경함 */
			mheg_itk_avsDestroyStreamingAv(pstStreamHandler);
		}

		//media play -> broadcast preloading 일 경우 WatchTV Action으로 변경해줌 (octo 2.0에서는 action간 자동전환 시나리오가 없다)
		//if(AP_AM_GetActionType(MWC_RM_GetActionId(pstStreamHandler->ulViewId)) != eViewType_WTV)
		//{
		//	APC_ZAP_SendServiceStartByActionId(MWC_RM_GetActionId(pstStreamHandler->ulViewId), pstStreamHandler->svcHandle, eChListUiGroup_CURRENT, eNO_IPLATE);
		//}
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
			if (pstStreamHandler->svcUid == 0)
			{
				ITK_AvstreamHandle_t *prevAVS_V = (ITK_AvstreamHandle_t *)mheg_itk_avsGetCurrentHandleV();
				ITK_AvstreamHandle_t *prevAVS_A = (ITK_AvstreamHandle_t *)mheg_itk_avsGetCurrentHandleA();

				HxLOG_Debug("===========> NOT PRESENT pstStreamHandler (%p), eStreamRef (0x%x)\n", pstStreamHandler, pstStreamHandler->eStreamRef);
				stMediaEventData.preloadData.result	= ITK_NOT_PRESENT;
				stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_NOT_FOUND;

				/* Preload event callback을 Stop event callback보다 나중에 해도 되지만, 순서상 이게 맞는것 같다 */
				mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);

				if (prevAVS_V != NULL)
				{
					mheg_itk_avsStopStream(prevAVS_V, 0);
					if ((prevAVS_A != NULL) && (prevAVS_V != prevAVS_A))
					{
						mheg_itk_avsStopStream(prevAVS_A, 0);
					}
				}
			}
			else
			{
				/* make Media Event Data */
				stMediaEventData.preloadData.result = preload;		// <== check point #51
				stMediaEventData.preloadData.resultData.streamLength =  stream_length;
				pstStreamHandler->mediaStatus = ((pstStreamHandler->mediaStatus & 0xF0) | ITK_DISPLAY_AV_STATUS_PRELOAD);
				HxLOG_Debug("pstStreamHandler->callback %x\n",(HUINT32)pstStreamHandler->callback);

				mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
			}
			break;
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
			{
				HERROR			hErr = ERR_OK;

				HxLOG_Debug("Streaming AV(%p) - avComp(%ld/%ld), avMode(%d), reference(%s) !\n",
						pstStreamHandler, pstStreamHandler->audioComponent, pstStreamHandler->videoComponent, pstStreamHandler->avMode, pstStreamHandler->reference);

				if (port_itk_avs_GetHandle(pstStreamHandler) == NULL)
				{
					HxLOG_Error("Already released !\n");
					preload = ITK_NOT_PRESENT;
					stMediaEventData.preloadData.result = preload;
					stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_NOT_FOUND;
					mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
					break;
				}

				hErr = mheg_itk_avsCreateStreamingAV(pstStreamHandler);
				if (hErr != ERR_OK)
				{
					/* MEDIA 2.0에서 이 경우는 AP_MPlayer_Create()가 FAIL 일 경우만 해당됨 */
					mheg_itk_avsDestroyStreamingAv(pstStreamHandler);

					HxLOG_Error("Fail to create Streaming AV(0x%x) !\n", hErr);
					preload = ITK_NOT_PRESENT;
					stMediaEventData.preloadData.result = preload;
					stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_NOT_FOUND;
					mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
					break;
				}
			}
			break;
#endif

		default:
			HxLOG_Debug("Stream handler (%p) -> StreamRef(%d)\n", pstStreamHandler, pstStreamHandler->eStreamRef);
			stMediaEventData.preloadData.result	= ITK_NOT_PRESENT;
			stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_NOT_FOUND;

			mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
			HxLOG_Assert(0);
			break;
	}

	HxLOG_Debug("Stream handler (%p) PreLoaded (%s)!!!\n", pstStreamHandler, (preload == ITK_PRESENT) ? "ITK_PRESENT" : "ITK_NOT_PRESENT");
	HxLOG_Debug("===== (-) \n");

}

// eMHEG_ENGINE_START
static HERROR mheg_itk_EngineStart(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_MediaType_e	eMediaType;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;


	HxLOG_Debug("===== (+) \n");

	hErr = APK_MEDIA_PLAY_GetPlayType(nViewId, &eMediaType);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetPlayType Error \n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"eMediaType (%d ) , MhegIsRunning (%d) \n"C_CODE_RESET"\n", eMediaType, mheg_itk_IsRunning());

	//TODO: (process work) 필요시 APK_MEDIA_PLAY_GetPlayType  으로 구분
	if (eMediaType == eOxMP_MEDIATYPE_LIVE)
	{
		if (mheg_itk_IsRunning() == FALSE)
		{
			if ((s_stItkContext.eItkProfile == REDKEY_PROFILE_CIPLUS)
				||((s_stItkContext.bIsReadyAV == TRUE) && (s_stItkContext.bIsReadyPMT == TRUE)))
			{
				if (mheg_itk_StartMhegAppl() == eMHEG_START_FAIL)
				{
					mheg_itk_RetryStartMhegAppl(nViewId, 2, 10, 1);
				}
			}
		}
	}

	HxLOG_Debug("===== (-) \n");
	return ERR_OK;
}

// eMHEG_NOTIFY_STATUS
static HERROR mheg_itk_NotifyStatus(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	redkey_state_t state = (redkey_state_t)nParam1;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	(void)state;

	return ERR_OK;
}

// eMHEG_NOTIFY_APPSTATUS
static HERROR mheg_itk_NotifyAppStatus(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	redkey_mheg_app_state_t appState = (redkey_mheg_app_state_t)nParam1;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	if (context->eItkProfile == REDKEY_PROFILE_CIPLUS)
		PORT_ITK_APPMMI_NotifyEngineStatus(appState);

	return ERR_OK;
}

// eMHEG_RESTART_APPLICATION
static HERROR mheg_itk_RestartApplication(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
#if defined (CONFIG_MW_MM_PVR)
	Handle_t		hLastSvcHandle = HANDLE_NULL;
	Handle_t		hNewSvcHandle = HANDLE_NULL;
	HBOOL		bConflict = FALSE;
#endif
	HBOOL		bStartFromDeepLink = FALSE;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	{
		// TODO: (process work) last 채널 정보 필요
		//MGR_LIVE_GetLastSvcHandle (eLastSvcItem_TV, &hLastSvcHandle);
	}

#if defined (CONFIG_MW_MM_PVR)
	bConflict = mheg_itk_IsTuningConflict(hLastSvcHandle, &hNewSvcHandle);
	if(bConflict == TRUE)
	{
		hLastSvcHandle = hNewSvcHandle;
	}
#endif

	/* Deepl link로 부터 시작하고 eMEVT_MHEG_RESTART_MHEG_APPLICATION 메세지가 올라온 경우는 EXIT가 눌린 경우임 */
	/* 이 경우 Web Application으로 EVENT를 날려줌 (appstate = 0, userData = 2) */
	if(bStartFromDeepLink == TRUE)
	{
		#if 0 // TODO: (process work) : Deep Link
		BUS_PostMessageToOM(NULL, eMEVT_MHEG_TYPE_MHEG_APPSTATE, HANDLE_NULL, 0, 2, 0);
		#endif
	}

	if( IS_MHEG_REQUEST_QUIET_TUNE(s_stItkContext.stAtt.eTuneEvent, s_stItkContext.stAtt.nTuneFlag)

	#if 0 // TODO: (process work) 필요시 APK_MEDIA_PLAY_GetPlayType  으로 구분
	|| (MGR_ACTION_GetType(ulActionId) != eViewType_WTV))
	#else
	)
	#endif
	{
		if(s_stItkContext.stHide.nLastMasterSvcUid == s_stItkContext.stHide.nLastSuppleSvcUid)
		{
			// 이 경우는 Quite tune 된 service에서 원래의 service로 돌아오는 경우임 -> MHEG Restart only!!
			mheg_itk_RestartMhegAppl(nViewId);
		}
		else
		{
			// TODO: (process work) : last service 변경
			// MGR_LIVE_CmdSendServiceStartByActionId(SVC_PIPE_GetActionId(hAction), hLastSvcHandle);
		}
	}
	else
	{
		mheg_itk_RestartMhegAppl(nViewId);
	}

	return ERR_OK;
}

// eMHEG_DISPLAY_SUBTITLE_CONTROL
static HERROR mheg_itk_Display_SubtitleControl(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	HxLOG_Debug("===== (+) \n");
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"bSubtitleEnableStatus (%d )"C_CODE_RESET"\n", nParam1);

	/* 아래 flag는 MHEG engine 또는 Appmmi의 CI+ SSM  control할때만 세팅한다 */
	s_stItkContext.stAtt.bSubtitleEnableStatus = (HBOOL)nParam1;

	mheg_itk_ControlSubtitle(nViewId, nParam1);
	HxLOG_Debug("===== (-) \n");

	return ERR_OK;
}

itk_present_t	AP_Mheg_IsExistSubtitle(void)
{
	return (s_stItkContext.stAtt.bSubtitleEnableStatus == TRUE) ? ITK_PRESENT : ITK_NOT_PRESENT;
}

// eMHEG_DISPLAY_IMAGE_SHOW
static HERROR mheg_itk_Display_ImageShow(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	ITK_ImageHandle_t		*image = (ITK_ImageHandle_t*)nParam1;
	HUINT8				plane = (HUINT8)nParam2;
	HINT32				nByOther = (HINT32)nParam3;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	HxLOG_Debug("===== (+) \n");
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"image (%p), plane(%d), nByOther(0x%x)"C_CODE_RESET"\n", image, plane, nByOther);

	if (nByOther == 1 && s_stItkContext.stAtt.hImage == NULL)
	{
		// PORT_ITK_Display_ShowIFrameByOther에 의해 iframe이 display 될 경우 redkey에 의해서 이미 iframe이 release 된 경우 crash 발생가능.
		HxLOG_Debug("Skip iFrame !!!\n");
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	if(PORT_ITK_GetMhegInternalRunningStatus() == FALSE)
	{
		HxLOG_Debug("Redkey Engine should not be call this function - ignore (%s)\n", __FUNCTION__);
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	s_stItkContext.stAtt.imageDisplayFlag = TRUE;
	s_stItkContext.stAtt.hImage = image;

	PORT_ITK_DISPLAY_ShowImage(plane, image->content, image->content_size, image->format);

	HxLOG_Debug("===== (-) \n");

	return ERR_OK;
}

// eMHEG_DISPLAY_IMAGE_HIDE
static HERROR mheg_itk_Display_ImageHide(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	ITK_ImageHandle_t		*image = (ITK_ImageHandle_t*)nParam1;
	HUINT8				plane = (HUINT8)nParam2;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;
	(void)image;
	HxLOG_Debug("===== (+) \n");
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"plane(%d)"C_CODE_RESET"\n", plane);

	s_stItkContext.stAtt.imageDisplayFlag = FALSE;
	PORT_ITK_DISPLAY_HideImage(plane);

	HxLOG_Debug("===== (-) \n");

	return ERR_OK;
}

// eMHEG_DISPLAY_IMAGE_RELEASE
static HERROR mheg_itk_Display_ImageRelease(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	ITK_ImageHandle_t	*image = (ITK_ImageHandle_t*)nParam1;
	HUINT8				plane = (HUINT8)nParam2;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;
	(void)image;

	HxLOG_Debug("===== (+) \n");
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"plane(%d)"C_CODE_RESET"\n", plane);

	s_stItkContext.stAtt.imageDisplayFlag = FALSE;
	s_stItkContext.stAtt.hImage = NULL;
	PORT_ITK_DISPLAY_HideImage(plane);

	HxLOG_Debug("===== (-) \n");

	return ERR_OK;
}

// eMHEG_AVSTREAM_SELECT_AUDIO
static void mheg_itk_Avstream_SelectAudioComponent(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	ITK_AvstreamHandle_t		*pstStreamHandler = (ITK_AvstreamHandle_t*)nParam1;
	itk_int32_t				component = (itk_int32_t)nParam2;
	HUINT8					ucStreamType;
	HINT32					nSelAudioPid = PID_NULL;
	DxAudioCodec_e			eSelAudioCodec = eDxAUDIO_CODEC_UNKNOWN;
	HINT32 					nDefSvcUid, nCurSvcUid;
	HINT32					nCurVideoPid = PID_NULL, nCurAudioPid = PID_NULL, nCurPcrPid = PID_NULL;
	DxVideoCodec_e				eCurVideoCodec = eDxVIDEO_CODEC_UNKNOWN;
	DxAudioCodec_e				eCurAudioCodec = eDxAUDIO_CODEC_UNKNOWN;
	HUINT8						ucPcrType;
	HERROR						hErr;
	DxStereoSelect_e				eStereoSelect = eDxSTEREO_SELECT_UNKNOWN;
	HUINT32						ulViewId;

	HxLOG_Debug("===== (+) \n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	ulViewId = pstStreamHandler->ulViewId;

	/* get current Audio information from MHEG context */
	mheg_itk_GetContextSvcHandle(&nDefSvcUid, &nCurSvcUid);

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Select Audio component (0x%lx), nDefSvcUid (0x%x), nCurSvcUid (0x%x)"C_CODE_RESET"\n", component, nDefSvcUid, nCurSvcUid);
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Stream handler (%p) -> V/A/PCR Pid (0x%x, 0x%x, 0x%x), V/A Component (0x%lx, 0x%lx), V/A Status (%d, %d)"C_CODE_RESET"\n",
						pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus);
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Stream handler -> mediaStatus (0x%x), eStreamRef (%d)"C_CODE_RESET"\n", pstStreamHandler->mediaStatus | ITK_DISPLAY_AV_STATUS_A_SELECTED, pstStreamHandler->eStreamRef);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	if (mheg_itk_avsIsSettingStreamingAV(pstStreamHandler) == TRUE)
	{
		if (port_itk_avs_GetHandle(pstStreamHandler) == NULL)
		{
			HxLOG_Error("Already released !\n");
			HxLOG_Debug("===== (-) \n");
			return;
		}
	}
#endif

	pstStreamHandler->mediaStatus |= ITK_DISPLAY_AV_STATUS_A_SELECTED;
	if(pstStreamHandler->audioComponent != component)
	{
		if (pstStreamHandler->eStreamRef == eITK_REFSVC_CUR && component == -1)
		{
			mheg_itk_GetContextCurPesInfo(&nCurVideoPid, &nCurAudioPid, &nCurPcrPid, &eCurVideoCodec, &eCurAudioCodec);

			HxLOG_Debug("\tStream handler -> audioPid (0x%x), Cur -> AudioPid (0x%x)\n", pstStreamHandler->audioPid, nCurAudioPid);
			nSelAudioPid = nCurAudioPid;
		}
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		else if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI)
			//&& component == -1) // ?
		{
			OxMediaPlay_Component_t stAudioComponet;
			//HINT32					index;

			hErr= mheg_itk_avsGetSelectedAudioComponent(pstStreamHandler, component, &stAudioComponet);
			if (hErr == ERR_OK)
			{
				// set PID
				nSelAudioPid = stAudioComponet.stAudio.nPid;
				if (nSelAudioPid == PID_NULL) // video PID
				{
					eSelAudioCodec = eDxAUDIO_CODEC_UNKNOWN; // default
				}
				else
				{
					eSelAudioCodec = stAudioComponet.stAudio.eCodec;
				}
			}
			HxLOG_Debug("Streaming Av -> audioPid (0x%x), Type (0x%x)\n", pstStreamHandler->audioPid, pstStreamHandler->audioType);
 		}
#endif
		else
		{
			/* get audio component from PMT Raw data */
			HxLOG_Debug("Stream handler -> serviceId (0x%x)\n", pstStreamHandler->serviceId);

			if (s_stItkContext.eItkProfile == REDKEY_PROFILE_CIPLUS)
			{
				OxMediaPlay_Component_t stComponet;
				HINT32 ulCompIndex = (component == -1) ? 0 : component;

#if defined (CONFIG_DEBUG)
				{
					HUINT32 i, ulCompNum;

					OAPI_MEDIA_PLAY_GetComponentNum(ulViewId, eOxMP_COMPONENT_AUDIO, &ulCompNum);

					HxLOG_Debug("\t ulViewId (%d), Component Index (%d), Num (%d)\n", ulViewId, ulCompIndex, ulCompNum);

					HxLOG_Debug("\t[PMT info +]\n");
					for ( i = 0; i < ulCompNum; i++ )
					{
						OAPI_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_AUDIO, i, &stComponet);
						HxLOG_Debug("\t\tAudio PID = (%d) : 0x%x\n", stComponet.stAudio.nPid, stComponet.stAudio.nPid);
						HxLOG_Debug("\t\tAudio Codec = (%d)\n", stComponet.stAudio.eCodec);
						HxLOG_Debug("\t\tAudio Component Tag = (%d)\n", stComponet.stAudio.nComponentTag);
					}
				}
#endif
				hErr = OAPI_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_AUDIO, ulCompIndex, &stComponet);
				if ( hErr == ERR_OK )
				{
					nSelAudioPid = stComponet.stAudio.nPid;
					eSelAudioCodec = stComponet.stAudio.eCodec;
					APK_OUTPUT_AUDIO_GetStereoSelect(&eStereoSelect);
				}
				else
				{
					HxLOG_Error("\tdo not match component !!!\n");
				}
			}
			else
			{
				HUINT16 		usTsIdx;
				DxService_t	*pstSvcInfo;

				pstSvcInfo = APK_META_SVC_GetService(nCurSvcUid);
				if (pstSvcInfo == NULL)
				{
					HxLOG_Error("APK_META_SVC_GetService Error (nCurSvcUid : %d ) \n", nCurSvcUid);
					HxLOG_Debug("===== (-) \n");
					return ;
				}
				usTsIdx = pstSvcInfo->tsuid;
				APK_META_SVC_Delete(pstSvcInfo);

				hErr = MHEG_RAW_GetComponentInfo(usTsIdx, pstStreamHandler->serviceId, component, ITK_ELEMENT_INDEX_AUDIO, &nSelAudioPid, (HUINT8 *)&eSelAudioCodec, &ucStreamType, &eStereoSelect);
				if(hErr != ERR_OK)
				{
					/* if we can not retrieve audio component , we try to find it from SVC DB */
					HxLOG_Error("\tdo not match component in Rawdata, so try to find at SVC DB\n");

					if (component != -1)
					{
						pstStreamHandler->audioPid = nSelAudioPid;
						pstStreamHandler->audioType = eDxAUDIO_CODEC_UNKNOWN;
					}
				}

				if (pstStreamHandler->pcrPid == PID_NULL) // video에 의해서 설정되어 있으면 pcr를 설정하지 않음
				{
					hErr = MHEG_RAW_GetComponentInfo(usTsIdx, pstStreamHandler->serviceId, component, ITK_ELEMENT_INDEX_PCR, &nCurPcrPid, &ucPcrType, &ucStreamType, &eStereoSelect);
					if(hErr != ERR_OK)
					{
						nCurPcrPid = PID_NULL;
					}
				}
			}
		}

		HxLOG_Debug("\tSelect -> audioPid (0x%x), Type (0x%x)\n", nSelAudioPid, eSelAudioCodec);

		if(nSelAudioPid < PID_NULL && nSelAudioPid != 0)
		{
			// Selected Audio PID가 valid 한 값이면..
			pstStreamHandler->audioPid = nSelAudioPid;
			pstStreamHandler->audioType = eSelAudioCodec;
			pstStreamHandler->audioStatus = TRUE;

			if (pstStreamHandler->pcrPid == PID_NULL) // video에 의해서 설정되어 있으면 pcr를 설정하지 않음
			{
				if (nCurPcrPid == PID_NULL)
				{
					pstStreamHandler->pcrPid = nSelAudioPid;
				}
				else
				{
					pstStreamHandler->pcrPid = nCurPcrPid;
				}
			}

			/* check  if current AV Object status is 'immediate' or not */
			HxLOG_Debug("\tStream handler -> mediaStatus (0x%x)\n", pstStreamHandler->mediaStatus);
			//if((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING)
			if ( (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_CREATED
				|| (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PRELOAD
				|| (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_STOPPED )
			{
				HxLOG_Debug("\tselect audio -> do not change audio pid !!!\n");
			}
			else
			{
				/* change av track */
				HxLOG_Debug("\tselect Audio component : change pid --> hAction(0x%x), av pcrPID(0x%x / 0x%x / 0x%x) av param(0x%x / 0x%x\n",
								0, nSelAudioPid, 0, 0, (eSelAudioCodec << 16)|nSelAudioPid, PID_CURRENT);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
				{
					if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
					{
						mheg_itk_avsChangeAudioTrack(pstStreamHandler, component);
					}
				}
				else
				{
					mheg_itk_changeAV(ulViewId, MHEG_CODEC_CURRENT, PID_CURRENT, eSelAudioCodec, nSelAudioPid, pstStreamHandler->pcrPid);
				}
#else

				mheg_itk_changeAV(ulViewId, MHEG_CODEC_CURRENT, PID_CURRENT, eSelAudioCodec, nSelAudioPid, pstStreamHandler->pcrPid);
#endif
				APK_OUTPUT_AUDIO_SetStereoSelect(eStereoSelect);

				/* notify av track changed to ITK */
				s_stItkContext.stAtt.nCurAudioPid = nSelAudioPid;
				s_stItkContext.stAtt.eCurAudioCodec = eSelAudioCodec;
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				mheg_itk_avsSetContextAvMode(ulViewId, pstStreamHandler->avMode);
#endif
				mheg_itk_avsSetCurrentHandleA((void*)pstStreamHandler);

				mheg_itk_SetContextCurSvcUid(pstStreamHandler->svcUid);
			}
		}
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		else if ((pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST) && (component == 0))
		{
			if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
			{
				mheg_itk_avsChangeAudioTrack(pstStreamHandler, component);
			}
		}
#endif
		else
		{
			HxLOG_Debug("\tinvalid Audio Pid from SvcHandle(0x%x) pstStreamHandler->audioPid(0x%x)\n", nCurSvcUid, pstStreamHandler->audioPid);
		}

		pstStreamHandler->audioComponent = component;
	}
	else
	{
		HxLOG_Debug("\tsame audio component(%d) --> ignored\n", (int)component);
	}

	HxLOG_Debug("===== (-) \n");
	return;

}

// eMHEG_AVSTREAM_SELECT_VIDEO
static void mheg_itk_Avstream_SelectVideoComponent(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	ITK_AvstreamHandle_t	*pstStreamHandler = (ITK_AvstreamHandle_t*)nParam1;
	itk_int32_t				component = (itk_int32_t)nParam2;
	HINT32 					nSelVideoPid = PID_NULL;
	DxVideoCodec_e 			eSelVideoCodec = 0;
	HUINT8					ucStreamType;
	HINT32 					nDefSvcUid, nCurSvcUid;
	HINT32					nCurVideoPid = PID_NULL, nCurAudioPid = PID_NULL, nCurPcrPid = PID_NULL;
	DxVideoCodec_e 			eCurVideoCodec = eDxVIDEO_CODEC_UNKNOWN;
	DxAudioCodec_e 			eCurAudioCodec = eDxAUDIO_CODEC_UNKNOWN;
	HUINT8 					ucPcrType;
	HERROR					hErr;
	DxStereoSelect_e 		eStereoSelect;
	HUINT32 				ulViewId;

	HxLOG_Debug("===== (+) \n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	ulViewId = pstStreamHandler->ulViewId;

	/* get current Audio information from MHEG context */
	mheg_itk_GetContextSvcHandle(&nDefSvcUid, &nCurSvcUid);

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Select Video component (0x%lx), hDefSvc (0x%x), hCurSvc (0x%x)"C_CODE_RESET"\n", component, nDefSvcUid, nCurSvcUid);
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Stream handler (%p) -> V/A/PCR Pid (0x%x, 0x%x, 0x%x), V/A Component(0x%lx, 0x%lx), V/A Stauts (%d, %d)"C_CODE_RESET"\n",
						pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus);
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Stream handler -> mediaStatus (0x%x), eStreamRef (%d)"C_CODE_RESET"\n", pstStreamHandler->mediaStatus | ITK_DISPLAY_AV_STATUS_V_SELECTED, pstStreamHandler->eStreamRef);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	if (mheg_itk_avsIsSettingStreamingAV(pstStreamHandler) == TRUE)
	{
		if (port_itk_avs_GetHandle(pstStreamHandler) == NULL)
		{
			HxLOG_Error(("Already released !\n"));
			HxLOG_Debug("===== (-) \n");
			return;
		}
	}
#endif

	pstStreamHandler->mediaStatus |= ITK_DISPLAY_AV_STATUS_V_SELECTED;
	if(pstStreamHandler->videoComponent != component)
	{
		if (pstStreamHandler->eStreamRef == eITK_REFSVC_CUR && component == -1)
		{
			mheg_itk_GetContextCurPesInfo(&nCurVideoPid, &nCurAudioPid, &nCurPcrPid, &eCurVideoCodec, &eCurAudioCodec);

			HxLOG_Debug("\tStream handler -> videoPid (0x%x), Cur -> VideoPid (0x%x)\n", pstStreamHandler->videoPid, nCurVideoPid);
			nSelVideoPid = nCurVideoPid;
		}
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		else if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI)
			//&& component == -1) // ?
		{
			OxMediaPlay_Component_t stVideoComponet;
			//HINT32					index;

			hErr= mheg_itk_avsGetSelectedVideoComponent(pstStreamHandler, component, &stVideoComponet);
			if (hErr == ERR_OK)
			{
				// set PID
				nSelVideoPid = stVideoComponet.stVideo.nPid;
				if (nSelVideoPid == PID_NULL) // video PID
				{
					eSelVideoCodec = eDxVIDEO_CODEC_MPEG2; // default
				}
				else
				{
					eSelVideoCodec = stVideoComponet.stVideo.eCodec;
				}
			}
			HxLOG_Debug("Streaming Av -> audioPid (0x%x), Type (0x%x)\n", pstStreamHandler->audioPid, pstStreamHandler->audioType);
		}
#endif
		else
		{
			HxLOG_Debug("\tStream handler -> serviceId (0x%x)\n", pstStreamHandler->serviceId);

			if (s_stItkContext.eItkProfile == REDKEY_PROFILE_CIPLUS)
			{
				OxMediaPlay_Component_t stComponet;
				HINT32 ulCompIndex = (component == -1) ? 0 : component;

#if defined (CONFIG_DEBUG)
				{
					HUINT32 i, ulCompNum;

					OAPI_MEDIA_PLAY_GetComponentNum(ulViewId, eOxMP_COMPONENT_VIDEO, &ulCompNum);

					HxLOG_Debug("\t ulViewId (%d), Component Index (%d), Num (%d)\n", ulViewId, ulCompIndex, ulCompNum);

					HxLOG_Debug("\t[PMT info +]\n");
					for ( i = 0; i < ulCompNum; i++ )
					{
						OAPI_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_VIDEO, i, &stComponet);
						HxLOG_Debug("\t\tVideo PID = (%d) : 0x%x\n", stComponet.stVideo.nPid, stComponet.stVideo.nPid);
						HxLOG_Debug("\t\tVideo Codec = (%d)\n", stComponet.stVideo.eCodec);
						HxLOG_Debug("\t\tVideo Component Tag = (%d)\n", stComponet.stVideo.nComponentTag);
						HxLOG_Debug("\t\tVideo CasFound = (%d)\n\n", stComponet.stVideo.nCasFound);
					}
				}
#endif
				hErr = OAPI_MEDIA_PLAY_GetComponent(ulViewId, eOxMP_COMPONENT_VIDEO, ulCompIndex, &stComponet);
				if ( hErr == ERR_OK )
				{
					nSelVideoPid = stComponet.stVideo.nPid;
					eSelVideoCodec = stComponet.stVideo.eCodec;
					APK_OUTPUT_AUDIO_GetStereoSelect(&eStereoSelect);
				}
				else
				{
					HxLOG_Error("\tdo not match component !!!\n");
				}
			}
			else
			{
				HUINT16 		usTsIdx;
				DxService_t	*pstSvcInfo;

				pstSvcInfo = APK_META_SVC_GetService(nCurSvcUid);
				if (pstSvcInfo == NULL)
				{
					HxLOG_Error("APK_META_SVC_GetService Error (nCurSvcUid : %d) \n", nCurSvcUid);
					HxLOG_Debug("===== (-) \n");
					return ;
				}
				usTsIdx = pstSvcInfo->tsuid;
				APK_META_SVC_Delete(pstSvcInfo);

				hErr = MHEG_RAW_GetComponentInfo(usTsIdx, pstStreamHandler->serviceId, component, ITK_ELEMENT_INDEX_VIDEO, &nSelVideoPid, (HUINT8*)&eSelVideoCodec, &ucStreamType, &eStereoSelect);
				if(hErr != ERR_OK)
				{
					/* if we can not retrieve video component , we try to find it from SVC DB */
					HxLOG_Error("\tdo not match component in Rawdata, so try to find at SVC DB\n");

					if (component != -1)
					{
						pstStreamHandler->videoPid = nSelVideoPid;
						pstStreamHandler->videoType = eDxVIDEO_CODEC_UNKNOWN;
					}
				}
				else
				{
					if (ucStreamType == ISO_PRIVDATA_2)
					{
						DxService_t		*pstSvcInfo;

						/* DB의 video type을 보고 SD인지 HD인지 결정하여 codec 설정 */
						eSelVideoCodec = eDxVIDEO_CODEC_MPEG2;

						pstSvcInfo = APK_META_SVC_GetService(pstStreamHandler->svcUid);
						if (pstSvcInfo)
						{
							// 해당 point 에 대한 수정이 필요함 -> SD H.264 service에 대한 고려가 없음
	#if 0
							if( 0 != (stSvcInfo.eUserFlag1 & eSvcUserFlag_27_reserved) )
							{
								eSelVideoCodec = eDxVIDEO_CODEC_H264;
							}
							else
							{
								eSelVideoCodec = eDxVIDEO_CODEC_MPEG2;
							}
	#else
							if (pstSvcInfo->highDefinition == eDxVIDEO_TYPE_HD)
							{
								eSelVideoCodec = eDxVIDEO_CODEC_H264;
							}
							else
							{
								eSelVideoCodec = eDxVIDEO_CODEC_MPEG2;
							}
	#endif
							APK_META_SVC_Delete(pstSvcInfo);
						}
					}
				}

				hErr = MHEG_RAW_GetComponentInfo(usTsIdx, pstStreamHandler->serviceId, component, ITK_ELEMENT_INDEX_PCR, &nCurPcrPid, &ucPcrType, &ucStreamType, &eStereoSelect);
				if(hErr != ERR_OK)
				{
					nCurPcrPid = PID_NULL;
				}
			}
		}

		HxLOG_Debug("\tSelect -> videoPid (0x%x), Type (0x%x)\n", nSelVideoPid, eSelVideoCodec);

		if(nSelVideoPid < PID_NULL && nSelVideoPid != 0)
		{
			// Selected Video PID가 valid 한 값이면..
			pstStreamHandler->videoPid = nSelVideoPid;
			pstStreamHandler->videoType = eSelVideoCodec;
			if (nCurPcrPid == PID_NULL)
			{
				pstStreamHandler->pcrPid = nSelVideoPid;
			}
			else
			{
				pstStreamHandler->pcrPid = nCurPcrPid;
			}

			pstStreamHandler->videoStatus = TRUE;

			/* check if current AV Object status is 'immediate' or not */
			HxLOG_Debug("\tStream handler -> mediaStatus (0x%x)\n", pstStreamHandler->mediaStatus);
//			if((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING)
			if ( (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_CREATED
				|| (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PRELOAD
				|| (pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_STOPPED )
			{
				HxLOG_Debug("\tselect video -> do not change video pid !!!\n");
			}
			else
			{
				#if 0 // TODO: (process work) still
				if (s_MHEGContext.context[ulActionId].imageDisplayFlag == TRUE)
				{
					/* change av track */
					PORT_ITK_Display_StopIFrameByOther(pstStreamHandler->hAction);
				}
				#endif

				HxLOG_Debug("\tselect Video component : change pid --> hAction(0x%x), av pcrPID(0x%x / 0x%x / 0x%x) av param(0x%x / 0x%x)\n",
								0, 0, nSelVideoPid, pstStreamHandler->pcrPid, (eSelVideoCodec << 16)|nSelVideoPid, PID_CURRENT);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
				{
					if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
					{
						mheg_itk_avsChangeVideoTrack(pstStreamHandler, component);
					}
				}
				else
				{
					mheg_itk_changeAV(ulViewId, eSelVideoCodec, nSelVideoPid, MHEG_CODEC_CURRENT, PID_CURRENT, pstStreamHandler->pcrPid);
				}
#else

				mheg_itk_changeAV(ulViewId, eSelVideoCodec, nSelVideoPid, MHEG_CODEC_CURRENT, PID_CURRENT, pstStreamHandler->pcrPid);
#endif
				/* notify av track changed to ITK */
				s_stItkContext.stAtt.nCurVideoPid = nSelVideoPid;
				s_stItkContext.stAtt.eCurVideoCodec = eSelVideoCodec;
				s_stItkContext.stAtt.bSubtitleEnableByVideo = (nSelVideoPid == PID_NULL) ? FALSE : TRUE;
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
				mheg_itk_avsSetContextAvMode(nViewId, pstStreamHandler->avMode);
#endif
				mheg_itk_avsSetCurrentHandleV((void*)pstStreamHandler);

				mheg_itk_SetContextCurSvcUid(pstStreamHandler->svcUid);
			}
		}
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		else if ((pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST) && (component == 0))
		{
			if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
			{
				mheg_itk_avsChangeVideoTrack(pstStreamHandler, component);
			}
		}
#endif
		else
		{
			HxLOG_Debug("\tinvalid video Pid from CurSvcUid(0x%x) pstStreamHandler->videoPid(0x%x)\n", (unsigned int)nCurSvcUid, pstStreamHandler->videoPid);
		}

		pstStreamHandler->videoComponent = component;
	}
	else
	{
		HxLOG_Debug("\tsame video component(%ld) -> ignored\n",component);
	}

	HxLOG_Debug("===== (-) \n");
	return;
}

// eMHEG_AVSTREAM_DESELECT_AUDIO
static void mheg_itk_Avstream_DeselectAudioComponent(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	ITK_AvstreamHandle_t		*pstStreamHandler = (ITK_AvstreamHandle_t*)nParam1;
	itk_int32_t				component = (itk_int32_t)nParam2;
	HINT32 					nCurVideoPid = PID_NULL, nCurAudioPid = PID_NULL, nCurPcrPid = PID_NULL;
	DxVideoCodec_e 			eCurVideoCodec = eDxVIDEO_CODEC_UNKNOWN;
	DxAudioCodec_e 			eCurAudioCodec = eDxAUDIO_CODEC_UNKNOWN;
	HINT32 					nPcrPid = PID_CURRENT;
	HBOOL 					bChangePid = FALSE;
	HUINT32 					ulViewId;

	HxLOG_Debug("===== (+) \n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	ulViewId  = pstStreamHandler->ulViewId;
	mheg_itk_GetContextCurPesInfo(&nCurVideoPid, &nCurAudioPid, &nCurPcrPid, &eCurVideoCodec, &eCurAudioCodec);

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Audio component (0x%lx"C_CODE_RESET"\n", component);
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"[CurPES] V/A/PCR PID(0x%x/0x%x/0x%x) VideoType(0x%x) AudioType(0x%x)"C_CODE_RESET"\n",
					nCurVideoPid, nCurAudioPid, nCurPcrPid, eCurVideoCodec, eCurAudioCodec);
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"[AvStream(%p)] V/A/PCR PID(0x%x/0x%x/0x%x) V/A Component(0x%lx/0x%lx) V/A Status(%d/%d) V/A type(%d/%d) mediaStatus(0x%x) eStreamRef(%d)"C_CODE_RESET"\n",
					pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus,
					pstStreamHandler->videoType, pstStreamHandler->audioType, pstStreamHandler->mediaStatus, pstStreamHandler->eStreamRef);

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"avMode(%d)"C_CODE_RESET"\n", pstStreamHandler->avMode);
#endif

	if( ( ((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING)
		&& ((pstStreamHandler->mediaStatus & 0xF0) & ITK_DISPLAY_AV_STATUS_A_SELECTED) )
		|| ((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_CREATED) )
	{
		if (pstStreamHandler->audioComponent
			|| ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_CREATED) )
		{
			if (pstStreamHandler->audioPid == nCurAudioPid)
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
					nPcrPid = PID_NULL;
				}
			}

			HxLOG_Debug("deselect audio : change pid --> hAction(0x%x), videoPid(0x%x) audioPid(0x%x), pcrPid(0x%x)\n", 0, (MHEG_CODEC_CURRENT << 16) |PID_CURRENT, PID_NULL, nPcrPid);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
			{
				if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
				{
					mheg_itk_avsChangeAudioTrack(pstStreamHandler, DISABLE_INDEX);
				}
			}
			else
			{
				mheg_itk_changeAV(ulViewId, MHEG_CODEC_CURRENT, PID_CURRENT, MHEG_CODEC_CURRENT, PID_NULL, nPcrPid);
			}
#else

			mheg_itk_changeAV(ulViewId, MHEG_CODEC_CURRENT, PID_CURRENT, MHEG_CODEC_CURRENT, PID_NULL, nPcrPid);
#endif
		}

		//pstStreamHandler->audioPid = PID_NULL;
		pstStreamHandler->audioComponent = 0;
	}

	pstStreamHandler->mediaStatus &= ~ITK_DISPLAY_AV_STATUS_A_SELECTED;

	HxLOG_Debug("deselect audio component !! \r\n");
	HxLOG_Debug("===== (-) \n");
}

// eMHEG_AVSTREAM_DESELECT_VIDEO
static void mheg_itk_Avstream_DeselectVideoComponent(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	ITK_AvstreamHandle_t		*pstStreamHandler = (ITK_AvstreamHandle_t*)nParam1;
	itk_int32_t				component = (itk_int32_t)nParam2;
	HINT32 					nCurVideoPid = PID_NULL, nCurAudioPid = PID_NULL, nCurPcrPid = PID_NULL;
	DxVideoCodec_e 			eCurVideoCodec = eDxVIDEO_CODEC_UNKNOWN;
	DxAudioCodec_e 			eCurAudioCodec = eDxAUDIO_CODEC_UNKNOWN;
	HINT32 					nPcrPid = PID_CURRENT;
	HBOOL 					bChangePid = FALSE;
	HUINT32 					ulViewId;

	HxLOG_Debug("===== (+) \n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	ulViewId  = pstStreamHandler->ulViewId;
	mheg_itk_GetContextCurPesInfo(&nCurVideoPid, &nCurAudioPid, &nCurPcrPid, &eCurVideoCodec, &eCurAudioCodec);

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Video component (0x%lx)\n"C_CODE_RESET"\n", component);
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"[CurPES] V/A/PCR PID(0x%x/0x%x/0x%x) VideoType(0x%x) AudioType(0x%x)"C_CODE_RESET"\n",
					nCurVideoPid, nCurAudioPid, nCurPcrPid, eCurVideoCodec, eCurAudioCodec);
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"[AvStream(%p)] V/A/PCR PID(0x%x/0x%x/0x%x) V/A Component(0x%lx/0x%lx) V/A Status(%d/%d) V/A type(%d/%d) mediaStatus(0x%x) eStreamRef(%d)"C_CODE_RESET"\n",
					pstStreamHandler, pstStreamHandler->videoPid, pstStreamHandler->audioPid, pstStreamHandler->pcrPid, pstStreamHandler->videoComponent, pstStreamHandler->audioComponent, pstStreamHandler->videoStatus, pstStreamHandler->audioStatus,
					pstStreamHandler->videoType, pstStreamHandler->audioType, pstStreamHandler->mediaStatus, pstStreamHandler->eStreamRef);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"avMode(%d)"C_CODE_RESET"\n", pstStreamHandler->avMode);
#endif

	if( ( ((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING)
		&& ((pstStreamHandler->mediaStatus & 0xF0) & ITK_DISPLAY_AV_STATUS_V_SELECTED) )
		|| ((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_CREATED) )
	{
		if (pstStreamHandler->videoComponent
			|| ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_CREATED) )
		{
			if (pstStreamHandler->videoPid == nCurVideoPid)
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
					nPcrPid = PID_NULL;
				}
			}

			HxLOG_Debug("deselect video : change pid --> hAction(0x%x), videoPid(0x%x) audioPid(0x%x), pcrPid(0x%x)\n", 0, PID_NULL, (MHEG_CODEC_CURRENT << 16) | PID_CURRENT, nPcrPid);
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
			if (pstStreamHandler->avMode > MHEG_AV_MODE_BROADCAST)
			{
				if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
				{
					mheg_itk_avsChangeVideoTrack(pstStreamHandler, DISABLE_INDEX);
				}
			}
			else
			{
				/* video pid가 0이면 subtitle을 disable 한다. */
				s_stItkContext.stAtt.bSubtitleEnableByVideo = FALSE;
				mheg_itk_changeAV(ulViewId, MHEG_CODEC_CURRENT, PID_NULL, MHEG_CODEC_CURRENT, PID_CURRENT, nPcrPid);
			}
#else

			mheg_itk_changeAV(ulViewId, MHEG_CODEC_CURRENT, PID_NULL, MHEG_CODEC_CURRENT, PID_CURRENT, nPcrPid);
#endif
		}

		//pstStreamHandler->videoPid = PID_NULL;
		pstStreamHandler->videoComponent = 0;
	}

	pstStreamHandler->mediaStatus &= ~ITK_DISPLAY_AV_STATUS_V_SELECTED;

	HxLOG_Debug("deselect video component !! \r\n");
	HxLOG_Debug("===== (-) \n");
}

// eMHEG_AVSTREAM_SET_SPEED
static void mheg_itk_Avstream_SetSpeed(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	ITK_AvstreamHandle_t		*pstStreamHandler = (ITK_AvstreamHandle_t*)nParam1;
	itk_int32_t				numerator = (itk_int32_t)nParam2;
	itk_int32_t				denominator = (itk_int32_t)nParam3;
	MHEG_AV_SPEED_t			eMhegAvSpeed = eMHEG_AV_SPEED_STOP;
	HUINT8					bEnableSubtitle = TRUE;
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	HERROR					hErr = ERR_FAIL;
#endif

	HxLOG_Debug("===== (+) \n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"pstStreamHandler(%p), avStatus(%d, %d), speed (%ld, %ld), eStreamRef (%d), s_bMHEGFreezedA/V (%d, %d)"C_CODE_RESET"\n",
					pstStreamHandler, pstStreamHandler->audioStatus, pstStreamHandler->videoStatus, numerator, denominator, pstStreamHandler->eStreamRef,
					pstStreamHandler->bAudioFreeze, pstStreamHandler->bVideoFreeze);

	if(numerator == 0 && denominator == 1)
	{
		eMhegAvSpeed = eMHEG_AV_SPEED_PAUSE;
	}
	else if (numerator == 1 && denominator == 1)
	{
		eMhegAvSpeed = eMHEG_AV_SPEED_PLAY;
	}
	else if (numerator < 0 && denominator == 1)
	{
		eMhegAvSpeed = eMHEG_AV_SPEED_REWIND;
	}
	else if (numerator > 1 && denominator == 1)
	{
		/*
		<ICS011 procedure2> Verify the stream continues to be played at the same speed. from HDR2000T
		*/
		//eMhegAvSpeed = eMHEG_AV_SPEED_FAST_FWD;
		eMhegAvSpeed = eMHEG_AV_SPEED_PLAY;
	}

	HxLOG_Debug("set mheg av speed (%d)\n", eMhegAvSpeed);

	/* subtitle control */
	switch(eMhegAvSpeed)
	{
	case eMHEG_AV_SPEED_PLAY:
		bEnableSubtitle = TRUE;
		break;
	case eMHEG_AV_SPEED_PAUSE:
	case eMHEG_AV_SPEED_STOP:
	case eMHEG_AV_SPEED_REWIND:
		bEnableSubtitle = FALSE;
		break;
	case eMHEG_AV_SPEED_FAST_FWD:
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

	mheg_itk_ControlSubtitle(nViewId, bEnableSubtitle);

	switch (pstStreamHandler->eStreamRef)
	{
	case eITK_REFSVC_DVB:
	case eITK_REFSVC_DEF:
	case eITK_REFSVC_LCN:
	case eITK_REFSVC_FSI:
	case eITK_REFSVC_CUR:
		{
			switch(eMhegAvSpeed)
			{
			case eMHEG_AV_SPEED_PLAY:
			case eMHEG_AV_SPEED_FAST_FWD:
				if (pstStreamHandler->bVideoFreeze == TRUE && pstStreamHandler->videoStatus == TRUE)
				{
					pstStreamHandler->bVideoFreeze = FALSE;
					APK_MEDIA_PLAY_SetVideoFreeze(pstStreamHandler->ulViewId, FALSE);
				}

				if (pstStreamHandler->bAudioFreeze == TRUE && pstStreamHandler->audioStatus == TRUE)
				{
					pstStreamHandler->bAudioFreeze = FALSE;
					APK_MEDIA_PLAY_SetAudioFreeze(pstStreamHandler->ulViewId, FALSE);
				}
				break;

			case eMHEG_AV_SPEED_PAUSE:
			case eMHEG_AV_SPEED_REWIND:
				if (pstStreamHandler->bVideoFreeze == FALSE && pstStreamHandler->videoStatus == TRUE)
				{
					pstStreamHandler->bVideoFreeze = TRUE;
					APK_MEDIA_PLAY_SetVideoFreeze(pstStreamHandler->ulViewId, TRUE);
				}

				if (pstStreamHandler->bAudioFreeze == FALSE && pstStreamHandler->audioStatus == TRUE)
				{
					pstStreamHandler->bAudioFreeze = TRUE;
					APK_MEDIA_PLAY_SetAudioFreeze(pstStreamHandler->ulViewId, TRUE);
				}
				break;
			case eMHEG_AV_SPEED_STOP:
				pstStreamHandler->bAudioFreeze = TRUE;
				pstStreamHandler->bVideoFreeze = TRUE;
				APK_MEDIA_PLAY_SetVideoFreeze(pstStreamHandler->ulViewId, TRUE);
				APK_MEDIA_PLAY_SetAudioFreeze(pstStreamHandler->ulViewId, TRUE);
				break;
			}
		}
		break;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	case eITK_REFSVC_HTTP:
	case eITK_REFSVC_RTSP:
		{
			switch(eMhegAvSpeed)
			{
			case eMHEG_AV_SPEED_PLAY:
				mheg_itk_avsResumeStreamingAV(pstStreamHandler);
				break;

			case eMHEG_AV_SPEED_PAUSE:
				mheg_itk_avsPauseStreamingAV(pstStreamHandler);
				break;

			case eMHEG_AV_SPEED_REWIND:
			case eMHEG_AV_SPEED_FAST_FWD:
#if 1
				/* 현재는 rewind, fast forward 기능이 지원되지 않으므로 skip 기능으로 대체함. 추후에 동작시 #else 기능을 사용할 것 !! */
				{
					HUINT32 ulTime;

					hErr = APK_MEDIA_PLAY_GetPlayPosition(pstStreamHandler->ulViewId, &ulTime);
					if(hErr == ERR_OK)
					{
						if (numerator >= 0)
						{
							ulTime += numerator;
						}
						else
						{
							ulTime = ulTime < -numerator ? 0 : ulTime + numerator;
						}

						mheg_itk_avsSetPosition(pstStreamHandler, (itk_int32_t)ulTime);
					}
				}
#else
				local_mheg_Display_AVS_setSpeedStreamingAV(hAction, numerator);
#endif
				break;
			case eMHEG_AV_SPEED_STOP:
				mheg_itk_avsStopStreamingAV(pstStreamHandler);
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
		HxLOG_Debug("TODO : eITK_REFSVC_PREV, eITK_REFSVC_PVR !!!!!!!\n");
		break;
	}

	HxLOG_Debug("===== (-) \n");
}

static void mheg_itk_Demux_SiTable_Notify(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	HERROR					hErr;
	HINT32					nMainViewId;

	hErr = APK_MEDIA_PLAY_GetMainViewId(&nMainViewId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetMainViewId Error \n");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	if (PORT_ITK_GetMonitoringMode(nViewId) == eMHEG_MONITOR_MODE_Running)
	{
		//HxLOG_Debug("Call PORT_ITK_DEMUX_ProcessFilter() \n");

		PORT_ITK_DEMUX_ProcessFilter(ITK_DEMUX_SITABLE_NOTIFY, nParam1, nParam2);
	}

	//HxLOG_Debug("===== (-) \n");
}

static HERROR mheg_itk_ChangeSvc(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	HINT32 nSvcUid				= nParam1;
	HBOOL bQuiet 				= (nParam2 & ITK_TUNE_KEEP_CONTEXT) ? TRUE : FALSE;
	HBOOL bNdt 					= (nParam2 & ITK_TUNE_KEEP_DISPLAY) ? TRUE : FALSE;
	HBOOL bNoBanner				= (nParam2 & ITK_TUNE_NO_BANNER) ? TRUE : FALSE;
//	HBOOL bBroadcastInterrupt	= (nParam2 & ITK_TUNE_KEEP_BROADCAST_INTERRUPT) ? TRUE : FALSE;
	HBOOL bSameTs				= (HBOOL)nParam3;
	OxMediaPlay_TuningMode_e	eTuneMode;
	OxMediaPlay_StartInfo_t 	unStart;
	HUINT32						ulSessionId = 0;
	HINT32 nCurSvcUid, nDefSvcUid;

	HxLOG_Debug("=======(+) \n");

	// NDT 일지라도 TP가 달라 Tune을 해야하는 경우 flag는 유지하고 채널전환을 위한 임시변수의 값면 변경하도록 함.
	if (bNdt == TRUE)
	{
		bNdt = (bSameTs == FALSE) ? FALSE : TRUE;
	}

	eTuneMode = (bNoBanner == TRUE) ? eOxMP_TUNEMODE_NO_IPLATE|eOxMP_TUNEMODE_FORCE_TUNE : eOxMP_TUNEMODE_FORCE_TUNE;
	eTuneMode = (bNdt == TRUE) ? eOxMP_TUNEMODE_NDT_TUNE|eTuneMode : eTuneMode;

	s_stItkContext.eTuneEvent = ITK_SERVICE_TUNE_STOP_ITK;
	s_stItkContext.nTuneFlag = nParam2;

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Request change Service"C_CODE_RESET"\n");
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"nSvcUid(0x%x) , bQuiet(%d)  bNdt(%d) , bNoBanner(%d) eTuneMode(0x%x)"C_CODE_RESET"\n", nSvcUid, bQuiet, bNdt, bNoBanner, eTuneMode);

// TODO: 확인  필요...
#if 0
	if((s_ulCurPlayIdx < MAX_ICS_CONTENT_NUM) && (s_stICSContents[s_ulCurPlayIdx].pstStreamHandler != NULL)
		&& (local_mheg_Display_AVS_isPlayingStreamingAV() == TRUE))
	{
		s_stICSContents[s_ulCurPlayIdx].eStatus &= 0xF0;
		s_stICSContents[s_ulCurPlayIdx].eStatus |= eMHEG_ICS_STATE_STOP;
		ApMhegMsgPrint(DBG_TRACE, ("\t @@@ Stop ICS[%d] (by MSG_MHEG_SVC_CHANGESVC)\n", s_ulCurPlayIdx));

		(void)local_mheg_Display_AVS_stopStream(s_ulCurPlayIdx, s_stICSContents[s_ulCurPlayIdx].pstStreamHandler, 0);
	}

	if(bQuiet == TRUE)
	{
		HxLOG_Error("Not implemented...\n");
		// TODO: 구현 필요...
		//GWM_SendMessage(NULL, MSG_WTV_MHEG_QUIET_TUNE, hAction, p1, 0, eTuneMode);
	}
	else
	{
		HxLOG_Error("Not implemented...\n");
		// TODO: 구현 필요...
		//GWM_SendMessage(NULL, MSG_WTV_START_SVC_BY_ACTION_ID, hAction, p1, CURRENT_SVC_LIST_GROUP, eTuneMode);
	}
#endif

	PORT_ITK_GetSvcUid(&nDefSvcUid, &nCurSvcUid);

	HxLOG_Debug(C_CODE_F_WHITE_B_PURPLE"Request change Service"C_CODE_RESET"\n");
	HxLOG_Debug(C_CODE_F_WHITE_B_PURPLE"nDefSvcUid(0x%x) , nCurSvcUid(0x%x) , nSvcUid(0x%x),  bQuiet(%d)  bNdt(%d) , bNoBanner(%d) eTuneMode(0x%x) "C_CODE_RESET"\n",
		nDefSvcUid, nCurSvcUid, nSvcUid, bQuiet, bNdt, bNoBanner, eTuneMode);

	unStart.stLive.eType			= eOxMP_MEDIATYPE_LIVE;
	unStart.stLive.eLiveType			= eOxMP_LIVETYPE_MAIN;
	unStart.stLive.eTuneMode		= eTuneMode;
	unStart.stLive.ulMasterSvcUid		= nDefSvcUid;
	unStart.stLive.ulSuppleSvcUid		= nSvcUid;
	unStart.stLive.eSuppSvcType		= (bQuiet == TRUE) ? eOxMP_SUPPLESVCTYPE_SilentTune : eOxMP_SUPPLESVCTYPE_Normal;

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"nDefSvcUid(0x%x) , nCurSvcUid(0x%x) , nSvcUid(0x%x),  eTuneMode(0x%x)  , eSuppSvcType(0x%x)"C_CODE_RESET"\n", nDefSvcUid, nCurSvcUid, nSvcUid, eTuneMode, unStart.stLive.eSuppSvcType);

	if(bQuiet == TRUE)
	{
		if(eTuneMode & eOxMP_TUNEMODE_NDT_TUNE)
		{
			MHEG_SendMessage(eMHEG_APK_SVCCHANGED, nViewId, eOxMP_MEDIATYPE_LIVE, (HINT32)nSvcUid, 0, 0);
		}
		else
		{
			HERROR hErr = ERR_FAIL;

#if defined(CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY )
			OxMediaPlay_MediaType_e eMediaType = eOxMP_MEDIATYPE_NONE;

			if(context->stAtt.pstViewStreamHandler != NULL)
			{
				hErr = APK_MEDIA_PLAY_GetPlayType(context->stAtt.pstViewStreamHandler->ulViewId, &eMediaType);
				if((hErr == ERR_OK)
					&& (eOxMP_MEDIATYPE_MEDIAPLAY == eMediaType)
					&& (context->stAtt.pstViewStreamHandler->avStatus  & ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING) )
				{
					HxLOG_Debug("ViewID(%d) is not	eOxMP_MEDIATYPE_MEDIAPLAY. eMediaType(%d)\n", context->stAtt.pstViewStreamHandler->ulViewId, eMediaType);
					hErr = APK_MEDIA_PLAY_MhegBgStart(nViewId, &unStart, &ulSessionId);
					if(hErr != ERR_OK)
					{
						HxLOG_Debug("APK_MEDIA_PLAY_MhegBgStart  hErr(0x%x) \n", hErr);
					}

					PORT_ITK_SetSvcUid(nViewId, ulSessionId, nSvcUid, nSvcUid );
					MHEG_SendMessage(eMHEG_APK_SVCCHANGED, nViewId, eOxMP_MEDIATYPE_LIVE, (HINT32)nSvcUid, 0, 0);

					HxLOG_Debug("=======(-) \n");

					return ERR_OK;
				}
			}
#endif
			// prevent 183985
			hErr = APK_MEDIA_PLAY_Start(nViewId, &unStart, &ulSessionId);

			HxLOG_Debug("APK_MEDIA_PLAY_Start  hErr(0x%x) \n", hErr);
		}
	}
	else
	{
		//Oplbroadcast.c 에서 받음..opl_broadcase_mhegNonQuietTune
		HAPI_BroadcastSignal("signal:MhegEventNotify", 3, "iii", 30 /*OPLBROADCAST_EVENT_TYPE_MHEG_REQUEST_TUNE*/, nSvcUid, eTuneMode);
	}

	HxLOG_Debug("=======(-) \n");

	return ERR_OK;
}


// eMHEG_APK_SVCCHANGED
static HERROR mheg_itk_SvcChanged(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	itk_service_tune_event_t	eOrgTuneEvent;
	DxService_t 				*pstSvcInfo = NULL;
	OxMediaPlay_MediaType_e 	eMediaType = eOxMP_MEDIATYPE_NONE;
	HINT32		nOldTsUniqId = -1, nNewTsUniqId = -1;
	HINT32		nPreDefSvcUid = 0, nSvcUid = 0;
	HUINT32 		nTuneFlag;
	HUINT32		ulSessionId, ulMainViewSessionId;
	HINT32		nMainViewId;
	HERROR		hErr;

	nSvcUid = nParam2;
	APK_MEDIA_PLAY_GetMainViewId(&nMainViewId);
	APK_MEDIA_PLAY_GetSessionId(nMainViewId, &ulMainViewSessionId);
	APK_MEDIA_PLAY_GetSessionId(nViewId, &ulSessionId);

	HxLOG_Debug("=======(+) \n");

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"eMHEG_APK_SVCCHANGED - eType [%d], uid [%d], p3 [%d]\n"C_CODE_RESET, nParam1, nSvcUid, nParam3);
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"eMHEG_APK_SVCCHANGED - ViewId [%d], MainViewId[%d] ulSessionId[%d]  ulMainViewSessionId[%d]\n"C_CODE_RESET, __FUNCTION__, __LINE__, nViewId, nMainViewId, ulSessionId, ulMainViewSessionId);

	#if 0 // TODO: (process work) 필요시 sessionid 사용
	if(ulActionId != ulMainActionId)
	{
		/* Master action Id가 아니면 반응하지 않는다. */
		HxLOG_Warning("Ignore service change : old action handle !!\n");
		//return MESSAGE_PASS;
	}
	#endif

	if(ulMainViewSessionId != ulSessionId)
	{
		/* Master action Id가 아니면 반응하지 않는다. */
		HxLOG_Warning("Ignore service change : old action handle !!\n");
	}

	//TODO: (process work) 필요시 APK_MEDIA_PLAY_GetPlayType  으로 구분
	hErr = APK_MEDIA_PLAY_GetPlayType(nViewId, &eMediaType);
	if( hErr!= ERR_OK || eMediaType == eOxMP_MEDIATYPE_NONE)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetPlayType Error (hErr : %d ,  eMediaType: %d) \n", hErr, eMediaType);
		HxLOG_Debug("=======(-) \n");
		return ERR_FAIL;
	}

	eOrgTuneEvent 	= s_stItkContext.eTuneEvent;
	nTuneFlag 		= s_stItkContext.nTuneFlag;
	nPreDefSvcUid 		= s_stItkContext.stAtt.nDefSvcUid;

	HxLOG_Debug("pre-view/session id(%d/0x%x) -> view/session id(%d/0x%x), bSubtitleEnableStatus(%d), tune Event/Flag(%d/0x%x), nSvcUid(0x%x)\n",
		s_stItkContext.ulViewId, s_stItkContext.stAtt.ulSessionId, nViewId, ulSessionId, s_stItkContext.stAtt.bSubtitleEnableStatus, eOrgTuneEvent, nTuneFlag, nSvcUid);

	if(nTuneFlag & ITK_TUNE_KEEP_DISPLAY)
	{
		// TODO: (process work) : demux
#if defined(CONFIG_3RD_MHEG_CIPLUS_PROFILE1)
	/* Not use SI monitoring function for CI Plus */
#else
		pstSvcInfo = APK_META_SVC_GetService(nSvcUid);
		if (pstSvcInfo)
		{
			HxLOG_Debug("Send CMD_MHEG_SI_REQUEST_MONITOR_PMT (pmtPid : 0x%x)\n", pstSvcInfo->pmtPid);
			MHEG_SI_SendSectionMessage (CMD_MHEG_SI_REQUEST_MONITOR_PMT, nViewId, 0, (HUINT32)TRUE, pstSvcInfo->pmtPid);
			APK_META_SVC_Delete(pstSvcInfo);
		}
#endif
	}

	if (!IS_MHEG_REQUEST_NDT_QUIET_TUNE(eOrgTuneEvent, nTuneFlag)) // NDT quiet tune 가 아닐 경우에만
	{
		HxLOG_Debug("Reset subtitle control by iTK\n");
		if (s_stItkContext.stAtt.bSubtitleEnableStatus == TRUE)
		{
			mheg_itk_ControlSubtitle(nViewId, TRUE);
		}

		s_stItkContext.bIsReadyAV = FALSE;
		s_stItkContext.bIsReadyPMT = FALSE;
	}

	/* Mheg을 stop 하기전에 호출해야 PORT_ITK_DEMUX_NewSiTable 함수에서 hAction 체크 시 오류가 발생하지 않음 */
	/* MHEG_HIDE_ENABLE flag에 의해서 SVC_CHANGED message가 break 된 경우에도 아래 함수가 호출 되어야 hAction 체크 시 오류가 발생하지 않음 */
	PORT_ITK_SetSvcUid(nViewId, ulSessionId, nSvcUid, nSvcUid);

#if 0
// TODO: (process work) : PVR

	//if(nParam3 == eLiveViewSession_MHEG_QUIET)
	if(s_stItkContext.nTuneFlag & ITK_TUNE_KEEP_CONTEXT)
	{
		s_stItkContext.stHide.bQuiteTuneMode = TRUE;
	}
	else
	{
		s_stItkContext.stHide.nLastMasterSvcUid = nSvcUid;
		s_stItkContext.stHide.nLastSuppleSvcUid = 0;
		s_stItkContext.stHide.bQuiteTuneMode = FALSE;
	}

	if(s_stItkContext.stHide.nLastMasterSvcUid == s_stItkContext.stHide.nLastSuppleSvcUid)
	{
		s_stItkContext.stHide.bQuiteTuneMode = FALSE; // 제자리로 돌아온것임. 다시 엔진을 시작해도 무방함
		HxLOG_Debug("eMHEG_APK_SVCCHANGED:The target to retune service was same with last wtv svc(master svc)!\n");
	}
#endif

#if 0
#if defined(CONFIG_MW_MM_PVR)
	if(eViewType_TSR_PB != actionType)
	{
		s_stItkContext.stHide.bTrickEntering = FALSE;
	}
	else
	{
		s_stItkContext.stHide.bTrickEntering = TRUE;
	}

	HxLOG_Debug("eMHEG_APK_SVCCHANGED:tsr_entering:%d, s_stItkContext.stHide.bTrickExiting:%d, quite_mode: %d, nTuneFlag=0x%X\n",
		s_stItkContext.stHide.bTrickEntering,	s_stItkContext.stHide.bTrickExiting, s_stItkContext.stHide.bQuiteTuneMode,	s_stItkContext.nTuneFlag );

	if( s_stItkContext.stHide.bTrickEntering == TRUE && s_stItkContext.stHide.bQuiteTuneMode == TRUE)
	{// this cond. should not to be met in real env.
		s_stMhegHide.bTrickEntering = FALSE;
		HxLOG_Debug("eMHEG_APK_SVCCHANGED: ASSERT: Breaking proc!(entering tsr) \n");
		break;
	}
	else if( s_stItkContext.stHide.bTrickExiting == TRUE && s_stItkContext.stHide.bQuiteTuneMode == TRUE)
	{
		s_stItkContext.stHide.bTrickExiting = FALSE;
		HxLOG_Debug("eMHEG_APK_SVCCHANGED: Breaking proc!(exitting from tsr) \n");
		break;
	}
	else
	{
		s_stItkContext.stHide.bTrickExiting = FALSE;
		s_stItkContext.stHide.bTrickEntering = FALSE;
		HxLOG_Debug("eMHEG_APK_SVCCHANGED: Stop RedK Engine & Set operation flags with FALSE \n");
	}
#endif
#endif

	//if(mheg_itk_IsRunning() == TRUE)
	{
		/* stop redkey */
		HxLOG_Debug("call  mheg_itk_StopMhegAppl (MhegIsRunning : %d, eOrgTuneEvent : 0x%x)\n", mheg_itk_IsRunning(), eOrgTuneEvent);

		mheg_itk_StopMhegAppl(nViewId, eOrgTuneEvent);
	}

	/* update MHEG context */
	s_stItkContext.ulViewId 		= nViewId;
	s_stItkContext.stAtt.ulSessionId 	= ulSessionId;
	s_stItkContext.stAtt.nDefSvcUid 	= nSvcUid;
	s_stItkContext.stAtt.nCurSvcUid 	= nSvcUid;
	s_stItkContext.stAtt.eTuneEvent 	= eOrgTuneEvent;
	s_stItkContext.stAtt.nTuneFlag	= nTuneFlag;
#if defined(CONFIG_3RD_MHEG_CIPLUS_PROFILE1)
	/* Not use SI monitoring function for CI Plus */
	UNUSED(nPreDefSvcUid);
	UNUSED(nNewTsUniqId);
	UNUSED(nOldTsUniqId);
#else
	pstSvcInfo = APK_META_SVC_GetService(nPreDefSvcUid);
	if(pstSvcInfo == NULL)
	{
		HxLOG_Warning("APK_META_SVC_GetService Error ( nPreDefSvcUid : %d ) \n", nPreDefSvcUid);
	}
	else
	{
		nOldTsUniqId = pstSvcInfo->tsuid;
		APK_META_SVC_Delete(pstSvcInfo);
	}

	pstSvcInfo = APK_META_SVC_GetService(nSvcUid);
	if(pstSvcInfo == NULL)
	{
		HxLOG_Warning("APK_META_SVC_GetService Error ( nSvcUid : %d ) \n", nSvcUid);
	}
	else
	{
		nNewTsUniqId = pstSvcInfo->tsuid;
		APK_META_SVC_Delete(pstSvcInfo);
	}

	HxLOG_Debug("Old (SvcUid : 0x%x, TsUid : 0x%x) New (SvcUid : 0x%x, TsUid : 0x%x)\n", nPreDefSvcUid, nOldTsUniqId, nSvcUid, nNewTsUniqId);

	if (nOldTsUniqId != nNewTsUniqId)
	{
		HxLOG_Debug("MHEG_SI_SendSectionMessage(CMD_MHEG_SI_START_SEARCH), nSvcUid = 0x%x, nNewTsUniqId = 0x%x\n", nSvcUid, nNewTsUniqId);
		MHEG_SI_SendSectionMessage (CMD_MHEG_SI_START_SEARCH, nViewId, 0, (HUINT32)nSvcUid, nNewTsUniqId);
	}
	else
	{
		HxLOG_Debug("Same TS - don't need to retry section filter\n");
	}
#endif
	if (!IS_MHEG_REQUEST_NDT_QUIET_TUNE(eOrgTuneEvent, nTuneFlag)) // NDT quiet가 아닐 경우에만 설정함, NDT는 이전 pid 상태를 유지함
	{
		/* set current service PES info */
		pstSvcInfo = APK_META_SVC_GetService(nSvcUid);
		if(pstSvcInfo)
		{
			mheg_itk_SetContextCurPesInfo(pstSvcInfo->videoPid, pstSvcInfo->audioPid, pstSvcInfo->pcrPid, (HUINT8)pstSvcInfo->videoCodec, (HUINT8)pstSvcInfo->audioCodec);
			APK_META_SVC_Delete(pstSvcInfo);
		}
		else
		{
			HxLOG_Warning("Invalid SVC Handle(0x%x)\n", nSvcUid);
			mheg_itk_SetContextCurPesInfo(PID_NULL, PID_NULL, PID_NULL, eDxVIDEO_CODEC_UNKNOWN, eDxAUDIO_CODEC_UNKNOWN);
		}
#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		mheg_itk_avsSetContextAvMode(nViewId, MHEG_AV_MODE_BROADCAST);
#endif
	}

	if (IS_MHEG_REQUEST_NDT_QUIET_TUNE(eOrgTuneEvent, nTuneFlag)) // NDT quiet mode에서는 AV / SI를 stop -> start 하지 않고 service change 하기 때문에 redkey start를 이 메시지를 받았을 때 해야 함.
	{
		if (mheg_itk_StartMhegAppl() == eMHEG_START_FAIL)
		{
			mheg_itk_RetryStartMhegAppl(nViewId, 2, 10, 1);
		}
	}

	/* make tune status defaulted -> Stop by Host */
	s_stItkContext.eTuneEvent = ITK_SERVICE_TUNE_STOP_HOST;
	s_stItkContext.nTuneFlag = 0;

	//Live Section filter 를 걸어준다.
	//MHEG_SI_SendSectionMessage (CMD_MHEG_SI_START_LIVE_SECTION, nViewId, 0, nSvcUid, 0);

	s_bBroadCastInterruptFlag = FALSE;

	HxLOG_Debug("=======(-) \n");

	return ERR_OK;
}

// eMHEG_APK_SVCSTOPPED
static HERROR mheg_itk_SvcStopped(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	HxLOG_Debug("=======(+) \n");

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Svc Stopped : mheg_itk_IsRunning : %d)"C_CODE_RESET"\n",mheg_itk_IsRunning());

	/* stop redkey */
	if(mheg_itk_IsRunning() == TRUE)
	{
		mheg_itk_StopMhegAppl(nViewId, ITK_SERVICE_TUNE_STOP_HOST);
	}
	HxLOG_Debug("=======(-) \n");

	return ERR_OK;
}

// eMHEG_APK_AVSTARTED
static HERROR mheg_itk_AvStarted(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	HBOOL	bMhegRunning = mheg_itk_IsRunning(), bMhegAvailable = mheg_itk_IsAvailable();
	HERROR	hErr;
	OxMediaPlay_MediaType_e eMediaType;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	HxLOG_Debug("=======(+) \n");

	//TODO: (process work) 필요시 APK_MEDIA_PLAY_GetPlayType  으로 구분
	hErr = APK_MEDIA_PLAY_GetPlayType(nViewId, &eMediaType);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetPlayType Error \n");
		HxLOG_Debug("=======(-) \n");
		return ERR_FAIL;
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"AV started : [bMhegRunning(%d) bMhegAvailable(%d) eMediaType(%d) TuneEvent/flag (%d/0x%x) PMT Ready(%d)]"C_CODE_RESET"\n",
					bMhegRunning, bMhegAvailable, eMediaType, s_stItkContext.stAtt.eTuneEvent, s_stItkContext.stAtt.nTuneFlag, s_stItkContext.bIsReadyPMT);

	if (eMediaType == eOxMP_MEDIATYPE_LIVE)
	{
		if (s_stItkContext.bIsReadyPMT == TRUE)
		{
			if (IS_MHEG_REQUEST_TUNE(s_stItkContext.stAtt.eTuneEvent) // redkey가 retune한 경우 redkey stop시에 callback state를 호출하지 않는 경우가 발생하여 callback state를 고려하지 않고 start 하도록 함
				&& !IS_MHEG_REQUEST_NDT_QUIET_TUNE(s_stItkContext.stAtt.eTuneEvent, s_stItkContext.stAtt.nTuneFlag)) // ndt quiet tune 일 경우 MSG_WTV_SVC_CHANGED 메시지에서 start를 하기 때문에 여기서는 할 필요없음
			{
				if (mheg_itk_StartMhegAppl() == eMHEG_START_FAIL)
				{
					mheg_itk_RetryStartMhegAppl(nViewId, 2, 10, 1);
				}

				//if (IS_MHEG_REQUEST_NDT_TUNE(s_stItkContext.stAtt.eTuneEvent, s_stItkContext.stAtt.nTuneFlag))
				if(s_stItkContext.stAtt.nTuneFlag & ITK_TUNE_KEEP_DISPLAY)
				{
					#if 0 // TODO: (process work) still
					// i-frame 상태에서 non-destructive normally 일 경우 i-frame 다시 display 하도록 함 (normal tune 일 경우 zapping에 i-frame hide되기 때문, 추후에 watch tv 부분을 수정해야...)
					if (s_MHEGContext.context[ulActionId].hImage != NULL)
					{
						PORT_ITK_Display_StopIFrameByOther(hAction);
						PORT_ITK_Display_ShowIFrameByOther(hAction, s_MHEGContext.context[ulActionId].hImage);
					}
					#endif
				}
			}
			else if (bMhegAvailable == TRUE)
			{
				if(bMhegRunning == FALSE)
				{
					if (mheg_itk_StartMhegAppl() == eMHEG_START_FAIL)
					{
						mheg_itk_RetryStartMhegAppl(nViewId, 2, 10, 1);
					}
				}
				else
				{
					if (s_stItkContext.bIsReadyAV == FALSE)
					{
						mheg_itk_RetryStartMhegAppl(nViewId, 2, 10, 1);
					}
				}
			}
		}
	}

	s_stItkContext.bIsReadyAV = TRUE;
	HxLOG_Debug("=======(-) \n");

	return ERR_OK;
}

// eMHEG_APK_AVSTOPPED
static HERROR mheg_itk_AvStopped(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
#if 0
	HBOOL	bMhegRunning = FALSE,  bNdtTune = FALSE, bNdtQuietTune = FALSE;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	HxLOG_Debug("=======(+) \n");

	bMhegRunning		= mheg_itk_IsRunning();
	bNdtTune			= IS_MHEG_REQUEST_NDT_TUNE(s_stItkContext.stAtt.eTuneEvent, s_stItkContext.stAtt.nTuneFlag);
	bNdtQuietTune		= IS_MHEG_REQUEST_NDT_QUIET_TUNE(s_stItkContext.stAtt.eTuneEvent, s_stItkContext.stAtt.nTuneFlag);

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN" AV stopped :: bMhegRunning (%d), bNdtTune(%d) , bNdtQuietTune(%d)"C_CODE_RESET"\n", bMhegRunning, bNdtTune, bNdtQuietTune);

	if( 1)//(bMhegRunning == TRUE) && (bNdtTune == FALSE)  && (bNdtQuietTune == FALSE))
	{
		mheg_itk_StopMhegAppl(nViewId, ITK_SERVICE_TUNE_STOP_HOST);
	}

	s_stItkContext.bIsReadyAV = FALSE;
	HxLOG_Debug("=======(-) \n");
#endif

	return ERR_OK;
}

//   REMARK : SI task에서 PMT를 받거나 PMT가 변경이 되면, EVT_SI_PMT를 WatchTV로 보내게 되고,
//   MHEG Manager를 이를 받아서 PMT가 Received된것을 ITK에 알린다. 이 함수에 의해서
//   ITK는 PMT Raw Data를 관리하게 됨. PMT를 받으면 PAT 데이타도 같이 저장 관리하고,
//   PMT, PAT 데이터 리스트는 Redkey가 다시 구동되지 않으면 지속적으로 관리 된다.
//   이 작업은 MHEG Manager task(GWM)에 의해서 이루어 진다

static HERROR mheg_itk_NotifyTableReceived(HINT32 nViewId, SiTableType_t eSiTableType)
{
	HUINT32		ulMasterViewId, ulIndex;
	HINT32		nDefSvcUid, nCurSvcUid, ulSecNum;
	HERROR		hError;
	HUINT16 		usSvcId, usPmtSvcId;
	HUINT8		*pucRawData = NULL, *pTmpRaw = NULL;
	DxService_t	*pstSvcInfo = NULL;
	HUINT32		ulSecLen;
	//HUINT16 		usTsUniqId;

	HxLOG_Debug(" notify %s Received !!! ViewId(0x%x)\n", eSiTableType == eSI_TableType_PAT ?"PAT":"PMT", nViewId);

	PORT_ITK_GetMastViewId(&ulMasterViewId);
	if((ulMasterViewId != nViewId) ||
		((eSiTableType != eSI_TableType_PAT ) && (eSiTableType != eSI_TableType_PMT)))
	{
		HxLOG_Error(" different View Id(0x%x : 0x%x ) Table (%s) \n",ulMasterViewId, nViewId, eSiTableType == eSI_TableType_PAT ?"PAT":"PMT");
		return ERR_FAIL;
	}

	PORT_ITK_GetSvcUid(&nDefSvcUid, &nCurSvcUid);

	pstSvcInfo = APK_META_SVC_GetService(nDefSvcUid);
	if(pstSvcInfo == NULL)
	{
		HxLOG_Error("APK_META_SVC_GetService Error \n");
		return ERR_FAIL;
	}

	//usTsUniqId = pstSvcInfo->tsuid;
	usSvcId = pstSvcInfo->svcid;
	APK_META_SVC_Delete(pstSvcInfo);

	if(eSiTableType == eSI_TableType_PAT)
	{
		hError = APK_MEDIA_PLAY_CountMhegPatSection (ulMasterViewId, &ulSecNum);
	}
	else
	{
		hError = APK_MEDIA_PLAY_CountMhegPmtSection (ulMasterViewId, &ulSecNum);
	}

	if(hError != ERR_OK || ulSecNum == 0)
	{
		HxLOG_Error("APK_MEDIA_PLAY_CountMheg%sSection Error (hError = 0x%x, TableId ulSecNum = %d) \n", eSiTableType == eSI_TableType_PAT ?"Pat":"Pmt", hError, ulSecNum);
		HxLOG_Debug("===== (-) \n");
		return ERR_FAIL;
	}

	if(eSiTableType == eSI_TableType_PAT)
	{
		for(ulIndex = 0; ulIndex <ulSecNum; ulIndex++ )
		{
			hError = APK_MEDIA_PLAY_GetMhegPatSection(ulMasterViewId, ulIndex, (HUINT8**)&pTmpRaw, &ulSecLen);
			if(hError == ERR_OK)
			{
				pucRawData = (HUINT8*)HLIB_STD_MemAlloc(sizeof(HUINT8) * ulSecLen);
				if(pucRawData != NULL)
				{
					HxSTD_memcpy(pucRawData, pTmpRaw, sizeof(HUINT8) * ulSecLen);
					hError = ERR_OK;

					//prevent #220584
					APK_MEDIA_PLAY_FreeMhegPatSection(pTmpRaw);
					break;
				}

				APK_MEDIA_PLAY_FreeMhegPatSection(pTmpRaw);
			}
		}
	}
	else
	{
		for(ulIndex = 0; ulIndex <ulSecNum; ulIndex++ )
		{
			hError = APK_MEDIA_PLAY_GetMhegPmtSection(ulMasterViewId, ulIndex, (HUINT8**)&pTmpRaw, &ulSecLen);
			if(hError == ERR_OK)
			{
				usPmtSvcId = HxMACRO_Get16Bit(pTmpRaw + 3);

				if(usPmtSvcId == usSvcId)
				{
					pucRawData = (HUINT8*)HLIB_STD_MemAlloc(sizeof(HUINT8) * ulSecLen);
					if(pucRawData != NULL)
					{
						HxSTD_memcpy(pucRawData, pTmpRaw, sizeof(HUINT8) * ulSecLen);
						hError = ERR_OK;

						//prevent #220584
						APK_MEDIA_PLAY_FreeMhegPatSection(pTmpRaw);
						break;
					}
				}

				APK_MEDIA_PLAY_FreeMhegPmtSection(pTmpRaw);
			}
		}
	}

	if (pucRawData && hError == ERR_OK)
	{
		MHEG_SI_SendSectionMessage (CMD_MHEG_CURRENT_TABLE_ADD, nViewId, nDefSvcUid, (HUINT32)pucRawData, eSiTableType);
	}

	return hError;
}

static HBOOL	mheg_itk_IsMhegAvaliable(HINT32 nViewId)
{
	HERROR	hError = ERR_OK;
	HBOOL	bMhegAvaliable = FALSE;
	HUINT8	*pucRaw = NULL;
	HUINT16 	usSvcId;
	HUINT32	ulSecNum = 0, ulSecLen = 0, ulSecIndex =0;
	HUINT32	ulElementCnt = 0, ulElememtIndx = 0, ulDesCnt = 0 , ulDesIndex = 0 ;

	SIxDataBroadcastIdDes_t *pDBIdes = NULL;

	hError =APK_MEDIA_PLAY_CountMhegPmtSection ((HUINT32)nViewId, &ulSecNum );
	if((hError == ERR_OK) && (ulSecNum  > 0))
	{
		for(ulSecIndex = 0; ulSecIndex < ulSecNum ; ulSecIndex++)
		{
			hError = APK_MEDIA_PLAY_GetMhegPmtSection((HUINT32)nViewId, ulSecIndex, &pucRaw, &ulSecLen);
			if(hError != ERR_OK)	{ continue;	}

			usSvcId = HxMACRO_Get16Bit(pucRaw + 3);

			hError = MHEG_SI_CountPmtElementFromRaw(pucRaw, &ulElementCnt);
			if(hError != ERR_OK || ulElementCnt == 0)
			{
				APK_MEDIA_PLAY_FreeMhegPmtSection(pucRaw);
				continue;
			}

			HxLOG_Debug("Live (%d)  th PMT Section  %p [svcid : %x]!!  \n", ulSecIndex, pucRaw, usSvcId);
			HxLOG_Debug("Num of PMT Element : %d!!  \n", ulElementCnt);

			for(ulElememtIndx = 0; ulElememtIndx < ulElementCnt; ulElememtIndx++)
			{
				ulDesCnt = 0;

				hError = MHEG_SI_CountPmtElementDescriptorFromRaw(pucRaw, ulElememtIndx, eSIxDESCTAG_DATA_BROADCAST_ID, &ulDesCnt);
				if(hError !=ERR_OK || ulDesCnt == 0)		{continue; }

				HxLOG_Debug("eSIxDESCTAG_DATA_BROADCAST_ID descriptor Count: %d!!  \n", ulDesCnt);

				for(ulDesIndex = 0; ulDesIndex < ulDesCnt; ulDesIndex++)
				{
					pDBIdes = NULL;

					hError = MHEG_SI_GetPmtElementDescriptorFromRaw(pucRaw, ulElememtIndx, eSIxDESCTAG_DATA_BROADCAST_ID, 0, (void**)&pDBIdes);
					if ((hError == ERR_OK) && (pDBIdes != NULL))
					{
						if(pDBIdes->usDataBroadcastId == UK_DTT_MHEG_DATA_BROADCAST_ID && pDBIdes->pucIdSelector)
						{
							HUINT8 	ucIdx;
							HUINT16 	usAppTypeCode;
							HUINT8	ucAppSpecificDataLen;

							for(ucIdx = 0; ucIdx < pDBIdes->usSelectorLen; ucIdx++)
							{
								usAppTypeCode = (pDBIdes->pucIdSelector[ucIdx] << 8) | pDBIdes->pucIdSelector[ucIdx+1];

								HxLOG_Debug("usAppTypeCode (0x%04x) \n", usAppTypeCode);

								if ((usAppTypeCode == UK_DTT_MHEG_APPLICATION_CODE1) || (usAppTypeCode == UK_DTT_MHEG_APPLICATION_CODE2))
								{
									HxLOG_Debug("Ok Data Broadcast ID : SvcId (0x%x) \n", usSvcId);
									bMhegAvaliable = TRUE;
								}
								ucIdx += 3;
								ucAppSpecificDataLen = pDBIdes->pucIdSelector[ucIdx];

								ucIdx++;
								ucIdx += ucAppSpecificDataLen;
							}
						}
						SILIB_DESC_Delete((SIxDescriptor_t *)pDBIdes);
						pDBIdes = NULL;
					}
				}
			}

			APK_MEDIA_PLAY_FreeMhegPmtSection(pucRaw);
		}
	}

	HxLOG_Debug("bMhegAvaliable = %d \n", bMhegAvaliable);

	return bMhegAvaliable;
}

static HERROR mheg_itk_PmtChanged(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	HBOOL	bMhegRunning = mheg_itk_IsRunning();
	HBOOL	bMhegAvailable =FALSE;
	DxService_t 	*pstSvcInfo = NULL;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	HxLOG_Debug("=======(+) \n");
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"bMhegRunning(%d) TuneEvent/flag(%d/%d) AV Ready(%d) MhegPmtStatus(%d) "C_CODE_RESET"\n",
					bMhegRunning, s_stItkContext.stAtt.eTuneEvent, s_stItkContext.stAtt.nTuneFlag, s_stItkContext.bIsReadyAV, nParam3);
/*
	// oapi_media_play.h 에 정의 되어 있음.
	eMhegPmtStatus_None			= 0,
	eMhegPmtStatus_Received,
	eMhegPmtStatus_Timeout,
	eMhegPmtStatus_PidChanged,
	eMhegPmtStatus_PidRemoved
*/

	if(nParam3 !=eMhegPmtStatus_Received && nParam3 !=eMhegPmtStatus_PidChanged)
	{
		if(bMhegRunning == TRUE)
		{
			mheg_itk_StopMhegAppl(s_stItkContext.ulViewId, ITK_SERVICE_TUNE_STOP_HOST);
			HxLOG_Debug("=======(-) \n");
		}
		return ERR_OK;
	}

/* set current service PES info */
	pstSvcInfo = APK_META_SVC_GetService(s_stItkContext.stAtt.nCurSvcUid);
	if(pstSvcInfo)
	{
		if(s_stItkContext.bIsReadyPMT == FALSE)
		{
			mheg_itk_SetContextCurPesInfo(pstSvcInfo->videoPid, pstSvcInfo->audioPid, pstSvcInfo->pcrPid, (HUINT8)pstSvcInfo->videoCodec, (HUINT8)pstSvcInfo->audioCodec);
		}
		APK_META_SVC_Delete(pstSvcInfo);
	}
	else
	{
		HxLOG_Warning("Invalid SVC Handle(0x%x) \r\n", s_stItkContext.stAtt.nCurSvcUid);
		mheg_itk_SetContextCurPesInfo(PID_NULL, PID_NULL, PID_NULL, eDxVIDEO_CODEC_UNKNOWN, eDxAUDIO_CODEC_UNKNOWN);
	}

	if (PORT_ITK_CheckEngineProfile(MHEG_ENGINE_PROFILE_UK | MHEG_ENGINE_PROFILE_UK_FSAT | MHEG_ENGINE_PROFILE_AUSTRALIA))
	{
		// TODO: (process work) : Demux
		HERROR					hErr;
		OxMediaPlay_MediaType_e	eMediaType;

		bMhegAvailable = mheg_itk_IsMhegAvaliable(nViewId);
		HxLOG_Debug("bMhegAvailable(%d) \n", bMhegAvailable);

		/* check mheg status */
		mheg_itk_NotifyTableReceived(nViewId, eSI_TableType_PAT);
		mheg_itk_NotifyTableReceived(nViewId, eSI_TableType_PMT);

		HxLOG_Debug("Redkey status (%d), AV Ready (%d)\n", s_stItkContext.stAtt.eStatus, s_stItkContext.bIsReadyAV);

		hErr = APK_MEDIA_PLAY_GetPlayType(nViewId, &eMediaType);
		if (hErr == ERR_OK && eMediaType == eOxMP_MEDIATYPE_LIVE)
		{
			if (s_stItkContext.bIsReadyAV == TRUE)
			{
				if (IS_MHEG_REQUEST_TUNE(s_stItkContext.stAtt.eTuneEvent) // redkey가 retune한 경우 redkey stop시에 callback state를 호출하지 않는 경우가 발생하여 callback state를 고려하지 않고 start 하도록 함
				&& !IS_MHEG_REQUEST_NDT_QUIET_TUNE(s_stItkContext.stAtt.eTuneEvent, s_stItkContext.stAtt.nTuneFlag)) // ndt quiet tune 일 경우 MSG_WTV_SVC_CHANGED 메시지에서 start를 하기 때문에 여기서는 할 필요없음
				{
					if (mheg_itk_StartMhegAppl() == eMHEG_START_FAIL)
					{
						HxLOG_Error("Retry Redkey\n");
						/* wait 2 ticks and retry to start Redkey */
						mheg_itk_RetryStartMhegAppl(nViewId, 2, 10, 1);
					}

					if(s_stItkContext.stAtt.nTuneFlag & ITK_TUNE_KEEP_DISPLAY)
					{
						// i-frame 상태에서 non-destructive normally 일 경우 i-frame 다시 display 하도록 함 (normal tune 일 경우 zapping에 i-frame hide되기 때문, 추후에 watch tv 부분을 수정해야...)
						if (s_stItkContext.stAtt.hImage != NULL)
						{
							// TODO: (process work) : IFrame
							MHEG_NOT_IMPLEMENTED;
							//PORT_ITK_Display_StopIFrameByOther(hAction);
							//PORT_ITK_Display_ShowIFrameByOther(hAction, s_MHEGContext.context[ulActionId].hImage);
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
						if (mheg_itk_StartMhegAppl() == eMHEG_START_FAIL)
						{
							HxLOG_Error("Retry Redkey\n");
							/* wait 2 ticks and retry to start Redkey */
							mheg_itk_RetryStartMhegAppl(nViewId, 2, 10, 1);
						}
					}
					else
					{
						if (s_stItkContext.bIsReadyPMT == FALSE)
						{
							HxLOG_Error("Retry Redkey\n");
							/* wait 2 ticks and retry to start Redkey */
							mheg_itk_RetryStartMhegAppl(nViewId, 2, 10, 1);
						}
						else
						{
#if 0// // 확인 후 다시 enable 할 예정
							// retrive 하는 코드 필요 (mheg 화면이 변하지 않음 )   //hbbtv-cexistence Test ( DB8-COEX-N0-0160 / DB8-COEX-N0-0170)
							HxLOG_Debug("[%s:%d] \n", __FUNCTION__,__LINE__);
							mheg_itk_RetryStartMhegAppl(nViewId, 2, 10, 1);
//							mheg_itk_RestartMhegAppl(nViewId);
#endif
						}
					}
				}
			}
		}
	}

	s_stItkContext.bIsReadyPMT = TRUE;
	s_stItkContext.stAtt.bMhegAvailable = bMhegAvailable;
	HxLOG_Debug("=======(-) \n");

	return ERR_OK;
}

// eMHEG_APK_VIDEO_STATUS
static HERROR mheg_itk_VideoStatus(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	HxLOG_Debug("=======(+) \n");
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"bMhegRunning(%d)"C_CODE_RESET"\n", mheg_itk_IsRunning());

	if (mheg_itk_IsRunning() == TRUE)
	{
		PORT_ITK_DISPLAY_UpdateVideoStatus();
	}
	HxLOG_Debug("=======(-) \n");

	return ERR_OK;
}

// eMHEG_APK_AUDIOCLIPCOMPLETED
static HERROR mheg_itk_AudioClipCompleted(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	ITK_AudioclipHandle_t *ac = (ITK_AudioclipHandle_t*)PORT_ITK_AUDIOCLIP_getCurHandle();
	ITK_AudioclipHandle_t *stopped_ac = (ITK_AudioclipHandle_t*)PORT_ITK_AUDIOCLIP_getStoppedHandle();

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	HxLOG_Debug("=======(+) \n");

	if(ac == NULL)
	{
		HxLOG_Debug("PORT_ITK_AUDIOCLIP_getCurHandle is Null.\n");
		HxLOG_Debug("=======(-) \n");
		return ERR_OK;
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"ac(%p), stopped_ac(%p)"C_CODE_RESET"\n", ac, stopped_ac);

	if (ac->callback)
	{
		if (stopped_ac != NULL)
		{
			HxLOG_Debug("ITK_MEDIA_STOPPED stopped_ac->ucAudioEv = %d\n", stopped_ac->ucAudioEv);
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
			HxLOG_Debug("ITK_MEDIA_STOPPED ac->ucAudioEv = %d\n", ac->ucAudioEv);
			if (ac->ucAudioEv != eITK_DISPLAY_EVENT_STOP)
			{
				PORT_ITK_AUDIOCLIP_setCurHandle(NULL);

				//stop audio clip
				APK_MEDIA_PLAY_StopAudioClip(0);
				ac->ucAudioEv = eITK_DISPLAY_EVENT_STOP;
				ac->callback(ac->callback_context, (itk_audioclip_t *)ac, ITK_MEDIA_STOPPED);
			}
			else
			{
				HxLOG_Debug("audio clip is already stopped.\n");
			}
		}
	}
	else
	{
		HxLOG_Debug("audio clip is already released.\n");
	}
	HxLOG_Debug("=======(-) \n");

	return ERR_OK;
}

// eMHEG_KEY_DOWN
static HERROR mheg_itk_KeyDown(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4, void *pData, HUINT32 ulDataSize)
{
	HINT32	key_symbol, key_code;
	HInputEvent_t	inputEvent;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	if (ulDataSize != sizeof(HInputEvent_t) || pData == NULL)
	{
		return ERR_FAIL;
	}
	HxSTD_MemCopy(&inputEvent, (HInputEvent_t*)pData, sizeof(HInputEvent_t));
	key_symbol = inputEvent.key_symbol;
	key_code   = inputEvent.key_code;

	//	TODO: (process work)  pvr
	//	if MHEG is not running
	//	HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_PASS, inputEvent);

	//hErr = APK_MEDIA_PLAY_GetPlayType(nViewId, &eMediaType);
	//if (hErr == ERR_OK && eMediaType == eOxMP_MEDIATYPE_LIVE)
#if defined (CONFIG_MW_MM_PVR)
	if( IS_MHEG_REQUEST_QUIET_TUNE(s_stItkContext.stAtt.eTuneEvent, s_stItkContext.stAtt.nTuneFlag)
			&& (mheg_itk_IsRecording(s_stItkContext.stAtt.nDefSvcUid)) /*&& (actionType == eViewType_WTV)*/
			&& (s_stItkContext.stHide.nLastSuppleSvcUid != HANDLE_NULL) && (s_stItkContext.stHide.nLastMasterSvcUid != s_stItkContext.stHide.nLastSuppleSvcUid) )
	{
		HxLOG_Debug("skip key(down) in Quite tune and Recording mode \n");
		HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_PASS, &inputEvent);
	}
#endif

	if (mheg_itk_sendKeyToClient(key_symbol, key_code) == ERR_OK)
	{
		HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_CONSUMED, &inputEvent);
	}
	else
	{
		if (mheg_itk_sendKeyToHost(key_symbol, key_code) == ERR_OK)
		{
			HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_CONSUMED, &inputEvent);
		}
		else
		{
			HAPI_ReturnKeyEvent(eAWINDOW_MHEGPLAYER, eHINPUTRET_PASS, &inputEvent);
		}
	}

	return ERR_OK;
}

// eMHEG_ENGINE_START_RETRY
static HERROR mheg_itk_EngineStartRetry(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	HBOOL		bCheckRedkey = FALSE,bNoRetry = FALSE;
	HINT32 		nRetryCnt;
	HUINT32		ulSleepTime;
	HERROR		hErr;
	OxMediaPlay_MediaType_e eMediaType;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	ulSleepTime = (HUINT32)nParam1;
	nRetryCnt = nParam2;

	HLIB_STD_TaskSleep(ulSleepTime);			/* in case of Error */

	HxLOG_Debug("=======(+) \n");

	// TODO: (process work) 필요시 APK_MEDIA_PLAY_GetPlayType  으로 구분
	hErr = APK_MEDIA_PLAY_GetPlayType(nViewId, &eMediaType);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("APK_MEDIA_PLAY_GetPlayType Error \n");
		HxLOG_Debug("=======(-) \n");
		return ERR_FAIL;
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"ulSleepTime(%d), nRetryCnt(%d), eMediaType(0x%x)"C_CODE_RESET"\n", ulSleepTime, nRetryCnt, eMediaType);

	if (eMediaType == eOxMP_MEDIATYPE_LIVE)
	{
		if (mheg_itk_IsRunning() == FALSE)
		{
			if ((s_stItkContext.eItkProfile == REDKEY_PROFILE_CIPLUS)
				||((s_stItkContext.bIsReadyAV == TRUE) && (s_stItkContext.bIsReadyPMT == TRUE)))
			{
				bNoRetry = (mheg_itk_StartMhegAppl() == eMHEG_START_FAIL) ? FALSE : TRUE;
				bCheckRedkey = TRUE;
			}
		}

		/* Retry중에 running state로 바뀌면 retry를 중지한다 */
		if(bNoRetry == FALSE)
		{
			HxLOG_Debug("Redkey retry count (%d)\n", nRetryCnt);
			if(nRetryCnt < ITK_START_RETRY_LIMIT_NUM)
			{
				mheg_itk_RetryStartMhegAppl(nViewId, 0, (HINT32)(bCheckRedkey == TRUE) ? 10 : 5, nRetryCnt + 1);
			}
		}
	}

	HxLOG_Debug("=======(-) \n");

	return ERR_OK;
}

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
// eMHEG_SI_IP_STREAMING_PMT
static HERROR mheg_itk_StreamingPmt(MHEG_ITKContext_t *context, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4)
{
	HERROR		hErr = ERR_FAIL;
	HINT32		nAudioCnt, nSubtitleCnt;//, val;
	//HUINT16 	usAudioIdx;

	(void)context;
	(void)nViewId;
	(void)nParam1;
	(void)nParam2;
	(void)nParam3;
	(void)nParam4;

	nAudioCnt = nParam1;
	nSubtitleCnt = nParam2;

	HxLOG_Debug(C_CODE_F_YELLOW"s_bMhegSbtlEnable(%d) nAudioCnt(%d), nSubtitleCnt(%d)"C_CODE_RESET"\n", s_bMhegSbtlEnable, nAudioCnt, nSubtitleCnt);
	if (s_bMhegSbtlEnable == TRUE)
	{
		if (nSubtitleCnt > 0)
		{
		}
	}

	if (nAudioCnt > 1)
	{
		OxMediaPlay_Component_t stAudioComponet;
		HINT32					index;

		hErr = APK_MEDIA_PLAY_GetComponentIndex( nViewId, eOxMP_COMPONENT_AUDIO, &index, NULL);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("APK_MEDIA_PLAY_GetComponentIndex fail!!\n");
			return hErr;
		}

		hErr = APK_MEDIA_PLAY_GetComponent (nViewId, eOxMP_COMPONENT_AUDIO, index, &stAudioComponet);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("APK_MEDIA_PLAY_GetComponent fail!!\n");
			return hErr;
		}

		hErr = APK_MEDIA_PLAY_SetComponentIndex(nViewId, eOxMP_COMPONENT_AUDIO, index);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("APK_MEDIA_PLAY_SetComponentIndex fail!!\n");
			return hErr;
		}
	}

	return ERR_OK;
}

/* MHEG의 position이 packet 단위로 내려와서 시간으로 변환해 주는 scale value를 계산하는 함수  */
static HUINT32 mheg_itk_avsFileSizeToContentLength(HUINT32 ulFileSize)
{
	return ulFileSize/188;
}

static void mheg_itk_avsSetScaleValue(HUINT32 time, HUINT32 maxBitrates, HUINT32 *time2pos, HUINT32 *pos2time)
{
	if((time > 0 ) && (maxBitrates > 0))
	{
		// bitrate 이므로 packet 당 188 byte * 8 (bit로 변환 해서 나눠준다.)
		//maxPacket = (time * maxBitrates / (188*8));
		*time2pos = (HUINT32)((maxBitrates / (188*8)) + 0.5);
		*pos2time = (HUINT32)(((188 * 8 * 1000 * 1000) / maxBitrates) + 0.5);
	}
	else
	{
		*time2pos = 1000;
		*pos2time = 1000;
	}

	HxLOG_Debug("\033[30;43m Set scale : %d, %d  \033[0m\n",  *time2pos, *pos2time);

	return;
}

static HUINT32 mheg_itk_avsPos2time(HINT32 pos, HUINT32 pos2time)
{
	HUINT32	ulTime = 0;

	ulTime = (HUINT32)(pos * pos2time / 1000);

	return ulTime;
}

static itk_int32_t mheg_itk_avsTime2pos(HUINT32 time, HUINT32 time2pos)
{
	itk_int32_t itk_pos = 0;

	itk_pos = (itk_int32_t)(time * time2pos / 1000);

	return itk_pos;
}

static HERROR	mheg_itk_avsAddStreamHandler(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HUINT8	i;

	for(i=0; i<MAX_DOWNLOAD_SESSION_NUM; i++)
	{
		if(s_pstAvStreamHandlerList[i] == NULL)
		{
			s_pstAvStreamHandlerList[i] = pstStreamHandler;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


static HERROR	mheg_itk_avsRemoveStreamHandler(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HUINT8	i;

	for(i=0; i<MAX_DOWNLOAD_SESSION_NUM; i++)
	{
		if(s_pstAvStreamHandlerList[i] == pstStreamHandler)
		{
			s_pstAvStreamHandlerList[i] = NULL;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

static ITK_AvstreamHandle_t* mheg_itk_avsFindStreamHandlerByDownloadHandle(Handle_t hDownload)
{
	HUINT8	i;

	for(i=0; i<MAX_DOWNLOAD_SESSION_NUM; i++)
	{
		if(s_pstAvStreamHandlerList[i] && s_pstAvStreamHandlerList[i]->downloadHandle == hDownload)
		{
			return s_pstAvStreamHandlerList[i];
		}
	}
	HxLOG_Error("ERROR find stream handler \n");
	return NULL;
}
static HERROR mheg_itk_avsSetContextAvMode(HINT32 nViewId, HUINT8 avMode)
{
	HxLOG_Debug(" Set Context avMode(%d -> %d)\n", s_stItkContext.stAtt.avMode, avMode);

	s_stItkContext.stAtt.avMode = avMode;
	PORT_ITK_SetMastMhegAvMode(avMode);

	return ERR_OK;
}

static void mheg_itk_avsSetTrigger(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t trigger, itk_int32_t triggerValue)
{
	ITK_Media_Triggers_t *pTrigger = NULL;

	pTrigger = pstStreamHandler->pHead;

	while (pTrigger)
	{
		if (pTrigger->trigger == trigger)
		{
			pTrigger->triggerValue = mheg_itk_avsPos2time(triggerValue, pstStreamHandler->pos2time);

			HxLOG_Debug("replace : trigger(%ld), value(%ld) !\n", trigger, triggerValue);
			return;
		}

		pTrigger = pTrigger->pNext;
	}

	pTrigger = (ITK_Media_Triggers_t *)HLIB_STD_MemAlloc(sizeof(*pTrigger));
	if (!pTrigger)
	{
		HxLOG_Error("out of memory : pTrigger is NULL !\n");
		return;
	}
	pTrigger->trigger = trigger;
	pTrigger->triggerValue = mheg_itk_avsPos2time(triggerValue, pstStreamHandler->pos2time);
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

	HxLOG_Debug("new : trigger(%ld), value(%ld) !\n", trigger, triggerValue);
}

static void mheg_itk_avsClearTrigger(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t trigger)
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
		HxLOG_Error("not found : pTrigger is NULL !\n");
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

	HxLOG_Debug("clear : trigger(%ld) !\n", trigger);
}

#define TRIGGER_TOLERANCE	300

static ITK_Media_Triggers_t *mheg_itk_avsFindTrigger(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t triggerValue)
{
	ITK_Media_Triggers_t *pTrigger = NULL;
	HUINT32				margin = 0;

	margin = mheg_itk_avsTime2pos(TRIGGER_TOLERANCE, pstStreamHandler->time2pos);

	pTrigger = pstStreamHandler->pHead;

	while (pTrigger)
	{
		if(triggerValue < margin) 		triggerValue = margin;
		if ((pTrigger->triggerValue >= (triggerValue-margin)) && (pTrigger->triggerValue < (triggerValue + margin)))
		{
			HxLOG_Debug("found : trigger(%ld), value(%ld), stream_pos(%ld) !\n", pTrigger->trigger, pTrigger->triggerValue, triggerValue);
			break;
		}

		pTrigger = pTrigger->pNext;
	}

	return pTrigger;
}

static void mheg_itk_avsSetPosition(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t position)
{
	HUINT32	ulTime		= 0;
	HERROR	hErr			= ERR_FAIL;

	HxLOG_Debug("===== (+) \n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("===== (-) \n");
		return;
	}

	ulTime = mheg_itk_avsPos2time(position, pstStreamHandler->pos2time);

	HxLOG_Debug("\033[30;43m pstStreamHandler [%p] : position(%ld)  ulTime(0x%x) \033[0m\n",pstStreamHandler, position, ulTime);
	HxLOG_Debug("\033[30;43m media status : %d  \033[0m\n", pstStreamHandler->mediaStatus & 0x0F);
	//Sets the playback position so that playback commences from this position.

	switch (pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
			if((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_PLAYING)
			{
				pstStreamHandler->position = position;
				hErr = mheg_itk_avsSetPlayTimeStreamingAV(pstStreamHandler, ulTime);
				HxLOG_Debug("\033[30;43m end position : %ld  \033[0m\n",pstStreamHandler->endPosition);
				if(pstStreamHandler->endPosition != 0 && position > pstStreamHandler->endPosition)
				{
					/* set play time이 ERR_OK가 아닌 경우는 set 하려는 position이 end position 보다 큰경우임
					 이런 경우 progress bar가 정상적으로 동작하지 않음 -> EOF 처리함  */
					 pstStreamHandler->position = pstStreamHandler->endPosition;
					 mheg_itk_avsEofPositionStreamingAV(pstStreamHandler);
				}
			}

	 		// MHEG ICS011 Trick play/stop 후 play가 제대로 안되는 이슈 수정 (아래는 2000T에서 잘못 포팅된 코드임), #if 0 일때 주석이 의미 있음, 수정되면 삭제할 주석 ...(코드는 문제없다.)
			else if((pstStreamHandler->mediaStatus & 0x0F) == ITK_DISPLAY_AV_STATUS_STOPPED)
			{
				pstStreamHandler->position = position;
				if(pstStreamHandler->position == 0)
				{
					// clear buffer & setup status.
					HxLOG_Debug("\033[30;43m  Discard buffering  \033[0m\n");
					mheg_itk_avsRemoveDownloadQueue(pstStreamHandler);
					pstStreamHandler->discardBuffer = TRUE;
					mheg_itk_avsDiscardDownloadBuffer();
					mheg_itk_avsSetStreamingAvStatus(pstStreamHandler, (mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) & ~ITK_DISPLAY_AV_STREAMING_STATUS_SETUP));
					mheg_itk_avsStartDownload(pstStreamHandler);
					pstStreamHandler->autoStart = TRUE;
					pstStreamHandler->setPosZero = TRUE; // preload event 시, download를 다시 진행할 필요 없이 preload event만  mheg으로 다시 보내주도록 ...
				}
			}
			else if(mheg_itk_avsIsSetupStreamingAV(pstStreamHandler) == TRUE)
			{
				pstStreamHandler->position = position;
			}

			if(pstStreamHandler->bufferPriority == ITK_MEDIA_BUFFER_PRIMARY)
			{
				// clear secondray buffer
				HxLOG_Debug("\033[30;43m clear secondray buffer  \033[0m\n");
			}
			break;

		default:
		{
			HxLOG_Error("unknown or not supported reference ! [%p]\n", pstStreamHandler);
		}
		return;
	}

 	HxLOG_Debug("Sets position(%ld) : audioPid(0x%x) videoPid(0x%x) pcrPid(0x%x) avStatus(0x%x/0x%x) avComp(%ld/%ld) (ref:%d) (0x%x/0x%x) avMode(%d)! [%p]\n", position,
		pstStreamHandler->audioPid, pstStreamHandler->videoPid, pstStreamHandler->pcrPid, pstStreamHandler->audioStatus, pstStreamHandler->videoStatus,
		pstStreamHandler->audioComponent, pstStreamHandler->videoComponent, pstStreamHandler->eStreamRef, pstStreamHandler->svcUid, pstStreamHandler->serviceId, pstStreamHandler->avMode, pstStreamHandler);
}

static void mheg_itk_avsSetEndPosition(struct ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t end_position)
{
	//HERROR			hErr;
	HUINT32			ulViewId 	= 0;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return;
	}
	/**
	* Sets the position to play to in a stream. When the position is reached,
	* presentation stops and the host generates an ITK_MEDIA_STOPPED event.
	*/

	ulViewId = pstStreamHandler->ulViewId;

	HxLOG_Debug("\033[30;43m pstStreamHandler [%p] : ulViewId(%d) end_position(%ld)  \033[0m\n", pstStreamHandler, ulViewId, end_position);

	switch (pstStreamHandler->eStreamRef)
	{
		case eITK_REFSVC_HTTP:
		case eITK_REFSVC_RTSP:
			if(end_position != -1)		// Freview HD general test (the space test)
			{
				pstStreamHandler->endPosition = end_position;
			}
			else if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_STOPPED)
			{
				pstStreamHandler->endPosition = 0;
			}
			break;

		default:
		{
			HxLOG_Error("unknown or not supported reference ! [%p]\n", pstStreamHandler);
		}
		return;
	}

 	HxLOG_Debug("Sets endPosition(%ld) : audioPid(0x%x) videoPid(0x%x) pcrPid(0x%x) avStatus(0x%x/0x%x) avComp(%ld/%ld) (ref:%d) (0x%x/0x%x) avMode(%d)! [%p]\n", end_position,
		pstStreamHandler->audioPid, pstStreamHandler->videoPid, pstStreamHandler->pcrPid, pstStreamHandler->audioStatus, pstStreamHandler->videoStatus,
		pstStreamHandler->audioComponent, pstStreamHandler->videoComponent, pstStreamHandler->eStreamRef, pstStreamHandler->svcUid, pstStreamHandler->serviceId, pstStreamHandler->avMode, pstStreamHandler);
}

/**
* @brief
*
* @retval ERR_OK success
* @retval ERR_FAIL fail
*/

static HERROR mheg_itk_avsCreateStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HERROR		hErr = ERR_FAIL;
	HINT32		nViewId;
	char 			*pucStreamPath;
	HUINT8		avMode;
	//DxDownload_Start_t	stDownloadStart;
	//Handle_t				hDownload;
	//HxTRANSPORT_Http_t 	*pHttp = NULL;
	//HUINT32				ulHttpCode 	= 0;
	//HFLOAT64			fDownloadSize;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	nViewId			= pstStreamHandler->ulViewId;
	pucStreamPath	= pstStreamHandler->reference;
	avMode			= pstStreamHandler->avMode;

	HxLOG_Debug("+++ Streaming AV Event (0x%x)\n",mheg_itk_avsGetStreamingAvStatus(pstStreamHandler));

	if (avMode > MHEG_AV_MODE_BROADCAST)
	{
		if (pucStreamPath == NULL)
		{
			HxLOG_Error("Invalid File Path !\n");
			return ERR_FAIL;
		}

		HxLOG_Debug("Stream Path(%s)\n", pucStreamPath);

		//if ((mheg_itk_av sIsSetupStreamingAV(pstStreamHandler) == TRUE) || (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE))
		//{
		//	HxLOG_Error("AV Already Loaded, So Should Destroy Before Loading !\n");
		//	mheg_itk_avsDestroyStreamingAv(pstStreamHandler);
		//}
		// 이미 Preload 가 된 경우
		// Content Available 을 바로 날리거나
		// Destroy 후에 다시 probe 시도.
		// 일단 전자로 진행.
#if 1 // MHEG ICS011 Trick play/stop 후 play가 제대로 안되는 이슈 수정
		if((mheg_itk_avsIsSetupStreamingAV(pstStreamHandler) == TRUE) || (pstStreamHandler->setPosZero == TRUE))
#else
		if(mheg_itk_avsIsSetupStreamingAV(pstStreamHandler) == TRUE)
#endif
		{
			itk_media_event_data_t	stMediaEventData;
			HUINT32					stream_length = 0;

			stream_length = mheg_itk_avsTime2pos(pstStreamHandler->duration, pstStreamHandler->time2pos);
			stMediaEventData.preloadData.result = ITK_PRESENT;
			stMediaEventData.preloadData.resultData.streamLength = (itk_int32_t)stream_length;
			mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
			pstStreamHandler->setPosZero = FALSE;
			return ERR_OK;
		}

		mheg_itk_avsSetStreamingAvStatus(pstStreamHandler, (mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) | ITK_DISPLAY_AV_STREAMING_STATUS_SETTING));

#if defined(ICS_PROBE) // For eHxTP_HTTP_RESCODE_INTERNAL_SERVER_ERROR
		// Test HTTP HEAD request instead of Probing for preload
		pHttp = HLIB_TRANSPORT_HttpOpen();
		if ( pHttp == NULL )
		{
			goto __ERROR;
		}

		hErr = HLIB_TRANSPORT_HttpSetPeerVeryfication(pHttp, FALSE);
		if ( hErr != ERR_OK )
		{
			HLIB_TRANSPORT_HttpClose(pHttp);
			goto __ERROR;
		}

		hErr = HLIB_TRANSPORT_HttpHead(pHttp, pucStreamPath);
		if ( hErr != ERR_OK )
		{
			HLIB_TRANSPORT_HttpClose(pHttp);
			goto __ERROR;
		}

		hErr = HLIB_TRANSPORT_HttpGetHttpResponseCode(pHttp, &ulHttpCode);
		HxLOG_Debug("\033[30;46m HTTP RESPONSE CODE : %d  \033[0m\n",  ulHttpCode);
		if(hErr != ERR_OK ||
			(ulHttpCode != eHxTP_HTTP_RESCODE_OK &&
				ulHttpCode != eHxTP_HTTP_RESCODE_MOVED_PERMANENTLY &&
				ulHttpCode != eHxTP_HTTP_RESCODE_FOUND &&
				ulHttpCode != eHxTP_HTTP_RESCODE_SEE_OTHER &&
				ulHttpCode != eHxTP_HTTP_RESCODE_TEMP_REDIRECT))
		{
			HLIB_TRANSPORT_HttpClose(pHttp);
			goto __ERROR;
		}

		hErr = HLIB_TRANSPORT_HttpGetContentLength(pHttp, &fDownloadSize);
		if( hErr != ERR_OK )
		{
			HLIB_TRANSPORT_HttpClose(pHttp);
			goto __ERROR;
		}
		pstStreamHandler->contentLength = mheg_itk_avsFileSizeToContentLength((HUINT32)fDownloadSize);
		HxLOG_Debug("\033[30;46m CONTENT LENGTH : %d  \033[0m\n",  (HUINT32)fDownloadSize);

		HLIB_TRANSPORT_HttpClose(pHttp);
#else

#endif
		hErr = mheg_itk_avsAddStreamHandler(pstStreamHandler);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("mheg_itk_avsAddStreamHandler Error !\n");
			goto __ERROR;
		}

		if(pstStreamHandler->bufferPriority == ITK_MEDIA_BUFFER_PRIMARY)
		{
			mheg_itk_avsDiscardDownloadBuffer();
			mheg_itk_avsStartDownload(pstStreamHandler);
		}
		else
		{
			mheg_itk_avsAddDownloadQueue(pstStreamHandler);
		}

		HxLOG_Debug("--- Completed AV Load ! avMode(0x%x)\n", avMode);
		/* Media 관련 동작 (Async) 처리를 위해서 Stream Handler를 임시로 저장,
			Preload() 에서 등록하고 Destroy() 에서 NULL로 초기화 함
			(local_mheg_Display_AVS_getCurHandle() 함수는 Play() 이후에만 사용 가능함) */

	}

	return ERR_OK;

__ERROR:
	mheg_itk_avsSetStreamingAvStatus(pstStreamHandler, (mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) & ~ITK_DISPLAY_AV_STREAMING_STATUS_SETTING));

	return ERR_FAIL;
}

static HERROR mheg_itk_avsDestroyStreamingAv(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HERROR	hErr = ERR_OK;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null \n");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

/* media 2.0 에서는 media player destroy를 안해도 됨. (get active media player를 하기 때문에)
   Preload만 한 상태에서 exit키를 누를 경우 setting 상태 이므로 이 경우에도 media player stop을 해주기 위해서
   check 루틴을 변경함 */

	if(mheg_itk_avsIsSettingStreamingAV(pstStreamHandler) == TRUE)
	{
		OxMediaPlay_MediaType_e eType = eOxMP_MEDIATYPE_NONE;

		hErr = APK_MEDIA_PLAY_GetPlayType(pstStreamHandler->ulViewId, &eType);
		if(hErr == ERR_OK && eType == eOxMP_MEDIATYPE_MEDIAPLAY)
		{
			hErr = APK_MEDIA_PLAY_Stop(pstStreamHandler->ulViewId);
			if(hErr != ERR_OK)
			{
				//
			}
		}
	}

	HxLOG_Debug("Completed AV stop !\n");

	mheg_itk_avsSetStreamingAvStatus(pstStreamHandler, (mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) & ~(ITK_DISPLAY_AV_STREAMING_STATUS_SETTING | ITK_DISPLAY_AV_STREAMING_STATUS_SETUP | ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING)));

	HxLOG_Debug("=====(-)\n");

	return hErr;
}

static HERROR mheg_itk_avsPlayStreamingAVRequestProbe(ITK_AvstreamHandle_t *pstStreamHandler, HUINT32 ulStartPos)
{
	HERROR			hErr = ERR_FAIL;
	OxMediaPlay_StartInfo_t unStart;
	HUINT32			ulStartTime;
	HUINT32			ulPlaySessionId = 0;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	mheg_itk_avsStopStreamingAV(pstStreamHandler);

	ulStartTime = mheg_itk_avsPos2time(ulStartPos, pstStreamHandler->pos2time);

	unStart.stMediaPlay.eType = eOxMP_MEDIATYPE_MEDIAPLAY;
#if defined(CONFIG_TIMESTAMP_64)
	unStart.stMediaPlay.ullStartTime = ulStartTime;
#else
	unStart.stMediaPlay.ulStartTime = ulStartTime;
#endif
	snprintf(unStart.stMediaPlay.szUrl , OxMP_MEDIAPLAY_URL, SESSION_URI_FORM, pstStreamHandler->downloadSessionId);

	// status change to play
	HxLOG_Debug("\033[30;43m Play Streaming [%p] : %s  \033[0m\n", pstStreamHandler, unStart.stMediaPlay.szUrl);
	hErr = APK_MEDIA_PLAY_Probe(pstStreamHandler->ulViewId, unStart.stMediaPlay.szUrl);

	HxLOG_Debug("ulStartTime : %d\n", ulStartTime);

	if (hErr != ERR_OK)
	{
		HxLOG_Error("while calling AP_MPlayer_Play(0x%x) !\n", hErr);
	}
	else
	{
		HxLOG_Debug("Play Streaming AV !!!\n");
		s_stItkContext.stAtt.ulSessionId = ulPlaySessionId;
		s_stItkContext.stAtt.pstViewStreamHandler = pstStreamHandler;
	}

	HxLOG_Debug("[#105313] s_stItkContext.stAtt.ulSessionId=0x%x\n", s_stItkContext.stAtt.ulSessionId);
	HxLOG_Debug("---\n");

	return hErr;
}

/* audio/video selection을 해서 ic stream의 probe과 play를 분리함. */
static HERROR mheg_itk_avsPlayStreamingAVPlay(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HERROR			hErr = ERR_FAIL;
	OxMediaPlay_StartInfo_t unStart;
	HUINT32			ulStartTime;
	HUINT32			ulPlaySessionId = 0;
	HUINT32			ulTotalVideoTrack = 0;
	HUINT32			ulTotalAudioTrack = 0;
//	HINT32			nVideoTrack = 0;
//	HINT32			nAudioTrack = 0;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	APK_MEDIA_PLAY_GetComponentNum(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, &ulTotalVideoTrack);
	APK_MEDIA_PLAY_GetComponentNum(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, &ulTotalAudioTrack);

	/* Video Track Change */
	if(pstStreamHandler->videoComponent == -1)
	{
		/* use default video track or the current track*/
	}
	else if (pstStreamHandler->videoComponent == 0)
	{
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, COMPONENT_OFF);
	}
	else if ((pstStreamHandler->videoComponent >= 1) && (pstStreamHandler->videoComponent <= ulTotalVideoTrack))
	{
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, (pstStreamHandler->videoComponent - 1));
	}
	else
	{
		HxLOG_Error("Unknown videoComponent(%d), Just disable video component \n", pstStreamHandler->videoComponent);
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, COMPONENT_OFF);
	}

	/* Audio Track Change */
	if(pstStreamHandler->audioComponent == -1)
	{
		/* use default aduio  track or the current track*/
	}
	else if (pstStreamHandler->audioComponent == 0)
	{
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, COMPONENT_OFF);
	}
	else if ((pstStreamHandler->audioComponent > ulTotalVideoTrack) && (pstStreamHandler->audioComponent <= (ulTotalVideoTrack+ulTotalAudioTrack)))
	{
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, (pstStreamHandler->audioComponent - ulTotalVideoTrack - 1));
	}
	else
	{
		HxLOG_Error("Unknown audioComponent(%d), Just disable audio component \n", pstStreamHandler->audioComponent);
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, COMPONENT_OFF);
	}

	/* position은 play start 또는 set postion command 에 의해서 미리 설정 된다. */
	ulStartTime = mheg_itk_avsPos2time(pstStreamHandler->position, pstStreamHandler->pos2time);
	unStart.stMediaPlay.eType = eOxMP_MEDIATYPE_MEDIAPLAY;
#if defined(CONFIG_TIMESTAMP_64)
	unStart.stMediaPlay.ullStartTime = ulStartTime;
#else
	unStart.stMediaPlay.ulStartTime = ulStartTime;
#endif
	snprintf(unStart.stMediaPlay.szUrl, OxMP_MEDIAPLAY_URL, SESSION_URI_FORM, pstStreamHandler->downloadSessionId);
	hErr = APK_MEDIA_PLAY_Start (pstStreamHandler->ulViewId, &unStart, &ulPlaySessionId);

	HxLOG_Debug("ulStartTime : %d\n", ulStartTime);

	if (hErr != ERR_OK)
	{
		HxLOG_Error("while calling AP_MPlayer_Play(0x%x) !\n", hErr);
	}
	else
	{
		HxLOG_Debug("Play Streaming AV !!!\n");
		s_stItkContext.stAtt.ulSessionId = ulPlaySessionId;
		s_stItkContext.stAtt.pstViewStreamHandler = pstStreamHandler;
		mheg_itk_avsSetStreamingAvStatus(pstStreamHandler, (mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) | ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING));
		pstStreamHandler->mediaStatus = (pstStreamHandler->mediaStatus & 0xF0) | ITK_DISPLAY_AV_STATUS_PLAYING;

		// defined in oplvideobroadcasthandle_def.h
		HAPI_BroadcastSignal("signal:MhegEventNotify", 2, "ii", 34 /*OPLBROADCAST_EVENT_TYPE_MHEG_ICS_STATUS*/, TRUE);

		mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PLAYING, NULL);
		s_ulCurPlayIdx = ICS_PLAY_MEDIA;
		if (pstStreamHandler->videoComponent == 0)
		{
			mheg_itk_ControlSubtitle(pstStreamHandler->ulViewId, FALSE);
		}
		else
		{
			mheg_itk_ControlSubtitle(pstStreamHandler->ulViewId, TRUE);
		}
	}

	HxLOG_Debug("[#105313] s_stItkContext.stAtt.ulSessionId=0x%x\n", s_stItkContext.stAtt.ulSessionId);
	HxLOG_Debug("---\n");

	return hErr;
}

/* ic stream 재생 중 video selection을 해주는 함수 */
static HERROR mheg_itk_avsChangeVideoTrack(ITK_AvstreamHandle_t *pstStreamHandler, HINT32 nComponentIndex)
{
	HERROR			hErr = ERR_FAIL;
//	OxMediaPlay_StartInfo_t unStart;
//	HUINT32			ulStartTime;
//	HUINT32			ulPlaySessionId = 0;
	HUINT32			ulTotalVideoTrack = 0;
	HINT32			nVideoTrack = 0;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	APK_MEDIA_PLAY_GetComponentNum(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, &ulTotalVideoTrack);

	/* Video Track Change */
	if(nComponentIndex == -1)
	{
		/* use default video track or the current track*/
		nVideoTrack = s_stItkContext.stAtt.nDefaultVideoIndex;
		if ( (nVideoTrack >= COMPONENT_OFF) && (nVideoTrack < ulTotalVideoTrack))
		{
			 APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, nVideoTrack);
		}
		else
		{
			HxLOG_Error("defaultVideoTrack(%d) is out of range(-1, %n) \n", nVideoTrack, ulTotalVideoTrack);
		}

	}
	else if (nComponentIndex == 0)
	{
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, COMPONENT_OFF);
	}
	else if ((nComponentIndex >= 1) && (nComponentIndex <= ulTotalVideoTrack))
	{
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, (nComponentIndex - 1));
	}
	else
	{
		HxLOG_Error("Unknown videoComponent(%d), Just disable video component \n", nComponentIndex);
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, COMPONENT_OFF);
	}

	if (nComponentIndex == 0)
	{
		mheg_itk_ControlSubtitle(pstStreamHandler->ulViewId, FALSE);
	}
	else
	{
		mheg_itk_ControlSubtitle(pstStreamHandler->ulViewId, TRUE);
	}

	HxLOG_Debug("---\n");

	return hErr;
}

/* ic stream 재생 중 audio selection을 해주는 함수 */
static HERROR mheg_itk_avsChangeAudioTrack(ITK_AvstreamHandle_t *pstStreamHandler, HINT32 nComponentIndex)
{
	HERROR			hErr = ERR_FAIL;
//	OxMediaPlay_StartInfo_t unStart;
//	HUINT32			ulStartTime;
//	HUINT32			ulPlaySessionId = 0;
	HUINT32			ulTotalAudioTrack = 0;
	HUINT32			ulTotalVideoTrack = 0;
	HINT32			nAudioTrack = 0;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	/* MHEG ICS026 Scene6: audio/video decoder가 둘 다 deselect 이면, audio decoder가 빨리 stop이 되는데, obama에서 end of stream event 2초 가량 늦게 올라온다.
	   이 2초 사이에 audio track을 변경 해주려면, 아래와 같이 해주어야 한다. */
	if ( (nComponentIndex != 0) && (pstStreamHandler->audioComponent == 0 && pstStreamHandler->videoComponent == 0))
	{
		HUINT32 		time_pos = 0;
		APK_MEDIA_PLAY_GetPlayPosition(pstStreamHandler->ulViewId, &time_pos);
		if(time_pos == 0)
		{
			mheg_itk_avsSetPosition(pstStreamHandler, 0);
			VK_TASK_Sleep(200); // 200ms sleep, audio track doesn't change while seeking.
		}
	}

	APK_MEDIA_PLAY_GetComponentNum(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, &ulTotalAudioTrack);
	APK_MEDIA_PLAY_GetComponentNum(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, &ulTotalVideoTrack);

	/* Audio Track Change */
	if(nComponentIndex == -1)
	{
		/* use default aduio  track or the current track*/
		nAudioTrack = s_stItkContext.stAtt.nDefaultAudioIndex;
		if ( (nAudioTrack >= COMPONENT_OFF) && (nAudioTrack < ulTotalAudioTrack))
		{
			 APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, nAudioTrack);
		}
		else
		{
			HxLOG_Error("defaultVideoTrack(%d) is out of range(-1, %n) \n", nAudioTrack, ulTotalAudioTrack);
		}

	}
	else if (nComponentIndex == 0)
	{
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, COMPONENT_OFF);
	}
	else if ((nComponentIndex > ulTotalVideoTrack) && (nComponentIndex <= (ulTotalVideoTrack+ulTotalAudioTrack)))
	{
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, (nComponentIndex - ulTotalVideoTrack - 1));
	}
	else
	{
		HxLOG_Error("Unknown audioComponent(%d), Just disable Audio component \n", nComponentIndex);
		APK_MEDIA_PLAY_SetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, COMPONENT_OFF);
	}

	HxLOG_Debug("---\n");

	return hErr;
}

/* ic stream probe 시 기본으로 설정된 audio/video index를 저장하는 함수  */
static HERROR mheg_itk_avsSaveDefaultAudioVideo(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HINT32			nVideoTrack = 0;
	HINT32			nAudioTrack = 0;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	// prevent 수정 #223422
	hErr=APK_MEDIA_PLAY_GetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, &nAudioTrack, NULL);
	hErr!=APK_MEDIA_PLAY_GetComponentIndex(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, &nVideoTrack, NULL);
	if(hErr != ERR_OK)
	{
		HxLOG_Warning("APK_MEDIA_PLAY_GetComponentIndex Error (Video : %d, Audio : %d) \n", nAudioTrack, nVideoTrack);
	}

	s_stItkContext.stAtt.nDefaultAudioIndex = nAudioTrack;
	s_stItkContext.stAtt.nDefaultVideoIndex = nVideoTrack;

#if 1 // workaround  MHEG ICS010 TC를 두번째 돌리 때부터 Video Handle이 등록되지 않았을 때, Work Around
	if (mheg_itk_avsGetCurrentHandleV() == NULL)
	{
		HINT32			nVideoTotalTrack = 0;
		OxMediaPlay_Component_t stVideoComponet;

		HxSTD_memset(&stVideoComponet, 0x00, sizeof(OxMediaPlay_Component_t));
		stVideoComponet.stAudio.nPid = PID_NULL;

		hErr=APK_MEDIA_PLAY_GetComponentNum(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, &nVideoTotalTrack);
		if (pstStreamHandler->videoComponent == -1) // use default video track
		{
			hErr=APK_MEDIA_PLAY_GetComponent(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, s_stItkContext.stAtt.nDefaultVideoIndex, &stVideoComponet);
			if ((stVideoComponet.stAudio.nPid != PID_NULL)&& (hErr == ERR_OK)){
				mheg_itk_avsSetCurrentHandleV((void*)pstStreamHandler);
			}
		}
		else if (pstStreamHandler->videoComponent == 0) // don't use video, so  it isn't need to set the stream handler.
		{
		}
		else if ( pstStreamHandler->videoComponent >= 1 && pstStreamHandler->videoComponent <= nVideoTotalTrack && (hErr == ERR_OK)) // if total video number is correct,
		{
			hErr=APK_MEDIA_PLAY_GetComponent(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, (pstStreamHandler->videoComponent-1), &stVideoComponet);
			if ((stVideoComponet.stAudio.nPid != PID_NULL)&& (hErr == ERR_OK)){
				mheg_itk_avsSetCurrentHandleV((void*)pstStreamHandler);
			}
		}
	}
#endif

	HxLOG_Debug("---\n");

	return ERR_OK;
}

/* 인자로 주어진 video component index에 따라 video 정보를 돌려줌.  */
static HERROR mheg_itk_avsGetSelectedVideoComponent(ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t	nComponentIndex, OxMediaPlay_Component_t *pstVideoComponet)
{
	HERROR			hErr = ERR_FAIL;
//	OxMediaPlay_StartInfo_t unStart;
//	HUINT32			ulStartTime;
//	HUINT32			ulPlaySessionId = 0;
	HUINT32			ulTotalVideoTrack = 0;
	HINT32			nVideoTrack = 0;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	if(pstVideoComponet == NULL)
	{
		HxLOG_Error("pstVideoComponet is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	APK_MEDIA_PLAY_GetComponentNum(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, &ulTotalVideoTrack);

	/* Video Track Change */
	if(nComponentIndex == -1)
	{
		/* use default video track or the current track*/
		nVideoTrack = s_stItkContext.stAtt.nDefaultVideoIndex;
		if ( (nVideoTrack >= COMPONENT_OFF) && (nVideoTrack < ulTotalVideoTrack))
		{
			hErr =  APK_MEDIA_PLAY_GetComponent(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, nVideoTrack, pstVideoComponet);
			if(hErr != ERR_OK)
			{
				HxLOG_Warning("APK_MEDIA_PLAY_GetComponent Error ( nVideo Track : %d , hErr : %d) \n", nVideoTrack, hErr);
			}
		}
		else
		{
			HxLOG_Error("defaultVideoTrack(%d) is out of range(-1, %d) \n", nVideoTrack, ulTotalVideoTrack);
			HxLOG_Debug("=====(-)\n");
			return ERR_FAIL;
		}

	}
	else if (nComponentIndex == 0)
	{
		HxLOG_Error("disable audio track, because nComponentIndex is 0\n");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}
	else if ((nComponentIndex >= 1) && (nComponentIndex <= ulTotalVideoTrack))
	{
		hErr = APK_MEDIA_PLAY_GetComponent(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, (nComponentIndex - 1), pstVideoComponet);
		if(hErr != ERR_OK)
		{
			HxLOG_Warning("APK_MEDIA_PLAY_GetComponent Error ( nVideo Track : %d , hErr : %d) \n", nComponentIndex - 1, hErr);
		}
	}
	else
	{
		HxLOG_Error("nComponentIndex(%d) is out of range(%d, %d) \n", nComponentIndex, 1, ulTotalVideoTrack);
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("---\n");

	return ERR_OK;
}


static HERROR mheg_itk_avsGetSelectedAudioComponent(ITK_AvstreamHandle_t *pstStreamHandler, itk_int32_t	nComponentIndex, OxMediaPlay_Component_t *pstAudioComponet)
{
	HERROR			hErr = ERR_FAIL;
//	HUINT32 		ulPlaySessionId = 0;
	HUINT32 		ulTotalAudioTrack = 0;
	HUINT32 		ulTotalVideoTrack = 0;
	HINT32			nAudioTrack = 0;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	if(pstAudioComponet == NULL)
	{
		HxLOG_Error("pstAudioComponet is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

		APK_MEDIA_PLAY_GetComponentNum(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, &ulTotalAudioTrack);
		APK_MEDIA_PLAY_GetComponentNum(pstStreamHandler->ulViewId, eOxMP_COMPONENT_VIDEO, &ulTotalVideoTrack);

	/* Audio Track Change */
	if(nComponentIndex == -1)
	{
		/* get default audio */
		nAudioTrack = s_stItkContext.stAtt.nDefaultAudioIndex;
		if ( (nAudioTrack >= COMPONENT_OFF) && (nAudioTrack < ulTotalAudioTrack))
		{
			 hErr =  APK_MEDIA_PLAY_GetComponent(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, nAudioTrack, pstAudioComponet);
			 if(hErr != ERR_OK)
			 {
				 HxLOG_Warning("APK_MEDIA_PLAY_GetComponent Error ( nAudioTrack Track : %d , hErr : %d) \n", nAudioTrack, hErr);
			 }
		}
		else
		{
			HxLOG_Error("defaultAudioTrack(%d) is out of range(-1, %d) \n", nAudioTrack, ulTotalAudioTrack);
			HxLOG_Debug("=====(-)\n");
			return ERR_FAIL;
		}
	}
	else if (nComponentIndex == 0)
	{
		HxLOG_Error("disable audio track, because nComponentIndex is 0\n");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}
	else if ((nComponentIndex > ulTotalVideoTrack) && (nComponentIndex <= (ulTotalVideoTrack+ulTotalAudioTrack)))
	{
		hErr = APK_MEDIA_PLAY_GetComponent(pstStreamHandler->ulViewId, eOxMP_COMPONENT_AUDIO, (nComponentIndex - ulTotalVideoTrack - 1), pstAudioComponet);
		if(hErr != ERR_OK)
		{
			HxLOG_Warning("APK_MEDIA_PLAY_GetComponent Error ( nAudioTrack Track : %d , hErr : %d) \n", nComponentIndex - ulTotalVideoTrack - 1, hErr);
		}
	}
	else
	{
		HxLOG_Error("nComponentIndex(%d) is out of range(%d, %d) \n", nComponentIndex, ulTotalVideoTrack, (ulTotalVideoTrack+ulTotalAudioTrack));
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("---\n");
	return ERR_OK;
}


static HERROR mheg_itk_avsStopStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HERROR	hErr = ERR_FAIL;

	// TODO: video, audio 선택적으로 stop을 해야하는 것인지 ?

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
	{
		mheg_itk_avsControlStreamingSubtitle(pstStreamHandler->ulViewId, FALSE);
		hErr = APK_MEDIA_PLAY_Stop(pstStreamHandler->ulViewId);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("ERROR : while calling MW_MEDIA_Stop(0x%x) !\n", hErr);
		}
		else
		{
			HxLOG_Debug("Stop Streaming AV (pstStreamHandler : %p)!!!\n", pstStreamHandler);
			mheg_itk_avsSetStreamingAvStatus(pstStreamHandler, (mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) & ~ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING));
			pstStreamHandler->mediaStatus = ITK_DISPLAY_AV_STATUS_STOPPED;
			s_ulCurPlayIdx = ICS_NO_PLAY;

			// workaround  MHEG ICS018 추후에 다시 download 할 수 있으므로, pause가 된 download는  관리 구조에서 제거하고 초기화(waiting) 시켜 준다.  create시 다시 download 받도록 수정됨.
			if (pstStreamHandler->downloadStatus == MHEG_STREAMING_STATUS_PAUSE)
			{
				mheg_itk_avsRemoveDownloadQueue(pstStreamHandler);
				pstStreamHandler->downloadStatus = MHEG_STREAMING_STATUS_WAITING;
				/* stream을 삭제 했으니, setup status도 해제. */
				mheg_itk_avsSetStreamingAvStatus(pstStreamHandler, (mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) & ~ITK_DISPLAY_AV_STREAMING_STATUS_SETUP));
			}

			mheg_itk_ControlSubtitle(pstStreamHandler->ulViewId, FALSE);
			/* default audio/video 초기화.  */
			s_stItkContext.stAtt.nDefaultAudioIndex = 0;
			s_stItkContext.stAtt.nDefaultVideoIndex = 0;
		}
	}

	HxLOG_Debug("---\n");

	return hErr;
}

static HERROR mheg_itk_avsPauseStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nViewId;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	nViewId = pstStreamHandler->ulViewId;

	if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
	{
		hErr = APK_MEDIA_PLAY_SetPlaySpeed(nViewId, 0);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("while calling MW_MEDIA_Pause(0x%x) !\n", hErr);
		}
	}

	HxLOG_Debug("---\n");
	return hErr;
}

static HERROR mheg_itk_avsResumeStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nViewId;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	nViewId = pstStreamHandler->ulViewId;

	if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
	{
		hErr = APK_MEDIA_PLAY_SetPlaySpeed(nViewId, 100);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("while calling MW_MEDIA_Resume(0x%x) !\n", hErr);
		}
	}

	HxLOG_Debug("---\n");
	return hErr;
}

static HERROR mheg_itk_avsControlStreamingSubtitle(HINT32 nViewId, HBOOL bOn)
{
	HERROR				hErr = ERR_FAIL;

#if 0	/* jwkim: subtitle function is not implemented in new media 2.0 yet. */
	HUINT32 			ulActionId;
	HUINT16				usUniqueId;
	MW_MEDIA_INFO_t		stMediaPlayInfo;

	ulActionId = MWC_RM_GetActionId(hAction);
	hErr = MW_MEDIA_GetCurrentStreamInfo(ulActionId, &stMediaPlayInfo); // MW_MEDIA_GetPlayInfo 사용하지 말 것 (방식이 다름)

#ifdef CONFIG_DEBUG
	mheg_itk_Debug_Print_PlayInfo(&stMediaPlayInfo);
#endif
	usUniqueId = DB_SVC_GetTempUniqueId();

	if (bOn == TRUE)
	{
		hErr = MW_SI_MHEG_StartStreaming(hAction, HANDLE_NULL, usUniqueId, stMediaPlayInfo.ulDemuxId);
	}
	else
	{
		hErr = MW_SI_MHEG_StopStreaming(hAction, usUniqueId);
// ToDo:		MW_SBTL_StopService(hAction, eSyncMode);
	}
#endif

	return hErr;
}

static HERROR mheg_itk_avsSetPlayTimeStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler, HUINT32 ulTime)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nViewId;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	nViewId = pstStreamHandler->ulViewId;

	if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
	{
		HxLOG_Debug("\033[30;43mSet play position : %d  \033[0m\n",  ulTime);
#if defined(CONFIG_TIMESTAMP_64)
		hErr = APK_MEDIA_PLAY_SetPlayPosition(nViewId, (HUINT64)ulTime);
#else
		hErr = APK_MEDIA_PLAY_SetPlayPosition(nViewId, ulTime);
#endif
		if (hErr != ERR_OK)
		{
			HxLOG_Error("while calling MW_MEDIA_SetPlayTime(0x%x) !\n", hErr);
		}
	}

	HxLOG_Debug("---\n");
	return hErr;
}
static void mheg_itk_avsChangePositionStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	itk_media_event_t evt = ITK_MEDIA_EVENT;
	itk_media_event_data_t evt_data, *ec = NULL;
	HUINT32	stream_pos = 0;
	HUINT32	time_pos = 0;
	HINT32 nViewId = 0;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ;//ERR_FAIL;
	}

	nViewId = pstStreamHandler->ulViewId;

	if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
	{
		if (pstStreamHandler != NULL)
		{
			if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI && pstStreamHandler->presentation_count > 0)
			{
				APK_MEDIA_PLAY_GetPlayPosition(nViewId, &time_pos);

				stream_pos = mheg_itk_avsTime2pos(time_pos, pstStreamHandler->time2pos);

				HxLOG_Debug("pstStreamHandler (%p) -> eStreamRef (%d), presentation_count (%ld), position (%ld), "C_CODE_F_BLACK_B_YELLOW"curPosition(%ld)"C_CODE_RESET"\n",
								pstStreamHandler, pstStreamHandler->eStreamRef, pstStreamHandler->presentation_count, pstStreamHandler->position, stream_pos);

				if (pstStreamHandler->position != stream_pos)
				{
					if (pstStreamHandler->endPosition != 0 && pstStreamHandler->endPosition <= stream_pos)
					{
						evt = ITK_MEDIA_STOPPED;
						pstStreamHandler->position = pstStreamHandler->endPosition;
					}
					else
					{
						ITK_Media_Triggers_t *pTrigger = NULL;

						pTrigger = mheg_itk_avsFindTrigger(pstStreamHandler, stream_pos);
						if (pTrigger)
						{
							evt = ITK_MEDIA_TRIGGER;
							evt_data.trigger = pTrigger->trigger;
							ec = (itk_media_event_data_t *)&evt_data;
						}

						pstStreamHandler->position = stream_pos;
					}
#if 1 // ICS009 Scene 3 Trigger Event 가 가끔 안올라오는 이슈 수정
					{
						static HINT32 s_prev_stream_pos	= -1;
						static HINT32 s_prev_trigger 	= -1;

						/* 중복된 trigger event는 보내지 않는다. */
						if((evt == ITK_MEDIA_TRIGGER) && (s_prev_trigger == evt_data.trigger))
						{
							evt = ITK_MEDIA_EVENT;
							HxLOG_Debug("WA: same trigger point!!! so doesn't send prev_trigger(%ld), evt_data.trigger(%ld)!\n", s_prev_trigger, evt_data.trigger);
						}
						/* 이전  trigger event를 저장한다. */
						if(evt == ITK_MEDIA_TRIGGER)
						{
							s_prev_trigger			= evt_data.trigger;
						}
						/* stop이 되면 이전  trigger event를  지운다. */
						/* 또는 seek 등의 이유로 이전 trigger event보다 앞으로 이동하면 이전  trigger event를  지운다. */
						if((evt == ITK_MEDIA_STOPPED) || (stream_pos < s_prev_stream_pos))
						{
							s_prev_trigger			= -1;
						}

						s_prev_stream_pos = stream_pos;
					}
#endif
				}

				if (evt < ITK_MEDIA_EVENT)
				{
					if (evt == ITK_MEDIA_STOPPED)
					{
						pstStreamHandler->vtbl.stop((itk_avstream_t *)pstStreamHandler);
					}
					else
					{
						mheg_itk_avsEventCallback(pstStreamHandler, evt, ec);
					}
				}
			}
		}
	}
}

static void mheg_itk_avsStartStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	if (pstStreamHandler != NULL)
	{
		HxLOG_Debug("pstStreamHandler (%p) -> eStreamRef (%d), presentation_count (%ld), position (%ld)\n",
						pstStreamHandler, pstStreamHandler->eStreamRef, pstStreamHandler->presentation_count, pstStreamHandler->position);

		if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI && pstStreamHandler->presentation_count > 0)
		{
			if ((pstStreamHandler->mediaStatus & 0x0f) != ITK_DISPLAY_AV_STATUS_PLAYING)
			{
				HxLOG_Debug("\033[30;46mMheg Event Callback : ITK_MEDIA_PLAYING  \033[0m\n");
				pstStreamHandler->mediaStatus = ITK_DISPLAY_AV_STATUS_PLAYING;
				mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PLAYING, NULL);
			}
		}
	}
}

static void mheg_itk_avsEofPositionStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
//	HINT32 nViewId;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ;//ERR_FAIL;
	}

	if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
	{
		HxLOG_Debug("pstStreamHandler (%p) -> eStreamRef (%d), presentation_count (%ld), position (%ld), mediaTerm (%d)\n",
						pstStreamHandler, pstStreamHandler->eStreamRef, pstStreamHandler->presentation_count, pstStreamHandler->position, pstStreamHandler->mediaTerm);

		if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI)
		{
			if (pstStreamHandler->presentation_count == 1)
			{
				pstStreamHandler->mediaStatus = ITK_DISPLAY_AV_STATUS_STOPPED;
				mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_STOPPED, NULL);

				// defined in oplvideobroadcasthandle_def.h
				HAPI_BroadcastSignal("signal:MhegEventNotify", 2, "ii", 34 /*OPLBROADCAST_EVENT_TYPE_MHEG_ICS_STATUS*/, FALSE);

				if (pstStreamHandler->mediaTerm == ITK_MEDIA_BLANK)
				{

					//	<ICS009>
					//	At the end of the stream playout (30 seconds) the progress bar must aso display that it has reached its completion.
					//	position을 0으로 하지 않음

					//pstStreamHandler->position = 0;
					mheg_itk_avsStopStreamingAV(pstStreamHandler);
				}
			}
			else if (pstStreamHandler->presentation_count > 1)
			{
				// <ICS009 - Scene3> position triggering을 위해 position 0으로 초기화 함.
				pstStreamHandler->presentation_count--;
				pstStreamHandler->position = 0;
				//mheg_itk_avsPlayStreamingAVRequestProbe(pstStreamHandle, 0);
				mheg_itk_avsSetPlayTimeStreamingAV(pstStreamHandler, 0);
			}
			else
			{
				//<ICS006, ICS007, and ICS012> If presentation_count is 0, then loop stream..
				pstStreamHandler->position = 0;
				//mheg_itk_avsPlayStreamingAVRequestProbe(pstStreamHandler, 0);
				mheg_itk_avsSetPlayTimeStreamingAV(pstStreamHandler, 0);
			}
		}
	}
}

static void mheg_itk_avsUnderflowResumeStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ;//ERR_FAIL;
	}

	if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
	{
		if (pstStreamHandler != NULL)
		{
			HxLOG_Debug("pstStreamHandler (%p) -> eStreamRef (%d), presentation_count (%ld), pstStreamHandler->mediaStatus(0x%x)\n", pstStreamHandler, pstStreamHandler->eStreamRef, pstStreamHandler->presentation_count, pstStreamHandler->mediaStatus);

			if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI && pstStreamHandler->presentation_count > 0)
			{
				if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PLAYING)
				{
					if ((pstStreamHandler->mediaStatus & 0xf0) & ITK_DISPLAY_AV_STATUS_STALLED)
					{
						pstStreamHandler->mediaStatus &= ~ITK_DISPLAY_AV_STATUS_STALLED;
						mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_RESUMED, NULL);
					}
				}
			}
		}
	}
}

static void mheg_itk_avsUnderflowStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ;//ERR_FAIL;
	}

	if (mheg_itk_avsIsPlayingStreamingAV(pstStreamHandler) == TRUE)
	{
		if (pstStreamHandler != NULL)
		{
			HxLOG_Debug("pstStreamHandler (%p) -> eStreamRef (%d), presentation_count (%ld)\n", pstStreamHandler, pstStreamHandler->eStreamRef, pstStreamHandler->presentation_count);

			if (pstStreamHandler->eStreamRef > eITK_REFSVC_FSI && pstStreamHandler->presentation_count > 0)
			{
				if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PLAYING)
				{
					if (!((pstStreamHandler->mediaStatus & 0xf0) & ITK_DISPLAY_AV_STATUS_STALLED))
					{
						pstStreamHandler->mediaStatus |= ITK_DISPLAY_AV_STATUS_STALLED;
						mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_STALLED, NULL);
					}
				}
			}
		}
	}

}

static HUINT8 mheg_itk_avsGetStreamingAvStatus(ITK_AvstreamHandle_t *pstStreamHandler)
{
	return	pstStreamHandler->avStatus;
}

static void mheg_itk_avsSetStreamingAvStatus(ITK_AvstreamHandle_t *pstStreamHandler, HUINT8	avStatus)
{
	HxLOG_Debug("+++ avStatus(0x%x)\n", avStatus);

	pstStreamHandler->avStatus = avStatus;
	//PORT_ITK_SetMastMhegAvStatus(avStatus);
}

static HBOOL mheg_itk_avsIsSettingStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HxLOG_Debug("+++ avStatus(0x%x)\n", mheg_itk_avsGetStreamingAvStatus(pstStreamHandler));

	return (HBOOL)(mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) & (ITK_DISPLAY_AV_STREAMING_STATUS_SETTING)) ? TRUE : FALSE;
}

static HBOOL mheg_itk_avsIsSetupStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HxLOG_Debug("+++ avStatus(0x%x)\n", mheg_itk_avsGetStreamingAvStatus(pstStreamHandler));

	return (HBOOL)(mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) & ITK_DISPLAY_AV_STREAMING_STATUS_SETUP) ? TRUE : FALSE;;
}

static HBOOL mheg_itk_avsIsPlayingStreamingAV(ITK_AvstreamHandle_t *pstStreamHandler)
{
	HERROR hErr;
	OxMediaPlay_MediaType_e eMediaType = eOxMP_MEDIATYPE_NONE;
	HxLOG_Debug("+++ avStatus(0x%x)\n", mheg_itk_avsGetStreamingAvStatus(pstStreamHandler));

	hErr = APK_MEDIA_PLAY_GetPlayType(pstStreamHandler->ulViewId, &eMediaType);
	if(hErr == ERR_OK && 	eOxMP_MEDIATYPE_MEDIAPLAY != eMediaType)
	{
		HxLOG_Debug("ViewID(%d) is not  eOxMP_MEDIATYPE_MEDIAPLAY. eMediaType(%d)\n", pstStreamHandler->ulViewId, eMediaType);
		return FALSE;
	}

	return (HBOOL)(mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) & ITK_DISPLAY_AV_STREAMING_STATUS_PLAYING) ? TRUE : FALSE;
}

static HERROR mheg_itk_avsProcessMPEventProbe(ITK_AvstreamHandle_t *pstStreamHandler, HUINT32 downloadSessionId, HUINT32 duration, HUINT32 maxBitrate, HBOOL bProbeSuccess)
{
	HERROR hErr = ERR_FAIL;
//	OxMediaPlay_Component_t stVideoComponet;
//	OxMediaPlay_Component_t stAudioComponet;

	HxLOG_Debug("=====(+)\n");

	if(pstStreamHandler == NULL)
	{
		HxLOG_Error("pstStreamHandler is Null");
		HxLOG_Debug("=====(-)\n");
		return ERR_FAIL;
	}

	if(bProbeSuccess == TRUE)
	{
		pstStreamHandler->downloadSessionId = downloadSessionId;
		pstStreamHandler->duration			= duration;
		pstStreamHandler->maxBitrate		= maxBitrate;
		mheg_itk_avsSetScaleValue(pstStreamHandler->duration, pstStreamHandler->maxBitrate, &(pstStreamHandler->time2pos), &(pstStreamHandler->pos2time));

		mheg_itk_avsSetStreamingAvStatus(pstStreamHandler, (mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) | ITK_DISPLAY_AV_STREAMING_STATUS_SETUP));
		// TODO: Language selection(subtitle & audio) 를 위해 si raw data처리가 필요함
#if defined(ICS_PROBE)

#else
		pstStreamHandler->contentLength = mheg_itk_avsTime2pos(pstStreamHandler->duration, pstStreamHandler->time2pos);
		if(pstStreamHandler->discardBuffer == TRUE)
		{
			pstStreamHandler->discardBuffer = FALSE;
		}
		else
		{
			itk_media_event_data_t	stMediaEventData;

			stMediaEventData.preloadData.result = ITK_PRESENT;
			stMediaEventData.preloadData.resultData.streamLength = (itk_int32_t)pstStreamHandler->contentLength;
			mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
		}
#endif
		HxLOG_Debug("\033[30;43mProbe success : %p  \033[0m\n",  pstStreamHandler);
		if(pstStreamHandler->autoStart)
			mheg_itk_avsPlayStreamingAVRequestProbe(pstStreamHandler, pstStreamHandler->position);

		return ERR_OK;
	}
	else
	{
		goto __ERROR;
	}

__ERROR:
	//mheg_itk_avsDestroyStreamingAv(nViewId);

	HxLOG_Error("Fail to create Streaming AV(0x%x) pstStreamHandler(%p)!\n", hErr, pstStreamHandler);

	mheg_itk_avsSetStreamingAvStatus(pstStreamHandler, (mheg_itk_avsGetStreamingAvStatus(pstStreamHandler) & ~ITK_DISPLAY_AV_STREAMING_STATUS_SETTING));

	return hErr;
}

static HERROR mheg_itk_avsProcessMPEventError(ITK_AvstreamHandle_t *pstStreamHandler)
{
	if (pstStreamHandler == NULL)
	{
		HxLOG_Error("Already released !\n");
		return ERR_FAIL;
	}

	HxLOG_Error("mediaStatus : 0x%x\n", pstStreamHandler->mediaStatus);

	if ((pstStreamHandler->mediaStatus & 0x0f) == ITK_DISPLAY_AV_STATUS_PLAYING)
	{
		pstStreamHandler->mediaStatus = ITK_DISPLAY_AV_STATUS_STOPPED;
		mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_STOPPED, NULL);
		return ERR_OK;
	}

	return ERR_FAIL;
}
static void mheg_itk_avsDownloadStart(ITK_AvstreamHandle_t *pstStreamHandler)
{
	DxDownload_Start_t		stDownloadStart;

	if(pstStreamHandler)
	{
		HxSTD_memset(&stDownloadStart, 0, sizeof(DxDownload_Start_t));
		stDownloadStart.eType = eDxDNLD_TYPE_IpMediaStream;
		stDownloadStart.stIpMedia.eType = eDxDNLD_TYPE_IpMediaStream;
		stDownloadStart.stIpMedia.bContentVerification = TRUE;
		stDownloadStart.stIpMedia.bMhegIcsStream = TRUE;
		HxSTD_StrNCpy(stDownloadStart.stIpMedia.szUrl, pstStreamHandler->reference, DxDNLD_MEDIAPLAY_URL);
		pstStreamHandler->downloadHandle = APK_DOWNLOAD_Start(pstStreamHandler->downloadHandle, &stDownloadStart);

		pstStreamHandler->downloadStatus = MHEG_STREAMING_STATUS_DOWNLOADING;
#if defined(ICS_PROBE)
		if(pstStreamHandler->discardBuffer == TRUE)
		{
			pstStreamHandler->discardBuffer = FALSE;
		}
		else
		{
			itk_media_event_data_t	stMediaEventData;

			stMediaEventData.preloadData.result = ITK_PRESENT;
			stMediaEventData.preloadData.resultData.streamLength = (itk_int32_t)pstStreamHandler->contentLength;
			mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
		}
#endif
		HxLOG_Debug("\033[30;43m Download Start : %p [handle %x]  \033[0m\n",  pstStreamHandler, pstStreamHandler->downloadHandle);
	}
}

static HERROR mheg_itk_avsAddDownloadQueue(ITK_AvstreamHandle_t *pstStreamHandler)
{
//	DxDownload_Start_t		stDownloadStart;
	HxList_t				*pvIter;
	ITK_AvstreamHandle_t 	*pstHandle;

	s_pDownloadList = HLIB_LIST_Append(s_pDownloadList, (void*)pstStreamHandler);

	for (pvIter = HLIB_LIST_First(s_pDownloadList); pvIter != NULL; pvIter = HLIB_LIST_Next(pvIter))
	{
		pstHandle = (ITK_AvstreamHandle_t*)HLIB_LIST_Data(pvIter);
		if(pstHandle == NULL)
			continue;
		if(pstHandle->downloadStatus == MHEG_STREAMING_STATUS_DONE)
			continue;
		if(pstHandle->downloadStatus == MHEG_STREAMING_STATUS_PAUSE)
			continue;
		if(pstHandle->downloadStatus == MHEG_STREAMING_STATUS_DOWNLOADING)
			return ERR_OK;
		if(pstHandle->downloadStatus == MHEG_STREAMING_STATUS_WAITING)
		{
			if(pstHandle == pstStreamHandler)
			{
				mheg_itk_avsDownloadStart(pstStreamHandler);
			}
			else
				return ERR_OK;
		}
	}

	return ERR_OK;
}

static HERROR mheg_itk_avsStartDownload(ITK_AvstreamHandle_t *pstStreamHandler)
{
	//ITK_AvstreamHandle_t	*pstFirst;

	s_pDownloadList = HLIB_LIST_Insert(s_pDownloadList, pstStreamHandler, 0);
	mheg_itk_avsDownloadStart(pstStreamHandler);
	return ERR_OK;
}

static HERROR mheg_itk_avsFinishDownload(ITK_AvstreamHandle_t *pstStreamHandler)
{
//	DxDownload_Start_t		stDownloadStart;
	HxList_t				*pvIter;
	ITK_AvstreamHandle_t 	*pstHandle;

	pstStreamHandler->downloadStatus = MHEG_STREAMING_STATUS_DONE;
	for (pvIter = HLIB_LIST_First(s_pDownloadList); pvIter != NULL; pvIter = HLIB_LIST_Next(pvIter))
	{
		pstHandle = (ITK_AvstreamHandle_t*)HLIB_LIST_Data(pvIter);
		if(pstHandle->downloadStatus == MHEG_STREAMING_STATUS_WAITING)
		{
			mheg_itk_avsDownloadStart(pstHandle);
			break;
		}
	}
	return ERR_OK;
}

static HERROR mheg_itk_avsPauseDownload(ITK_AvstreamHandle_t *pstStreamHandler)
{
//	DxDownload_Start_t		stDownloadStart;
	HxList_t				*pvIter;
	ITK_AvstreamHandle_t 	*pstHandle;

	pstStreamHandler->downloadStatus = MHEG_STREAMING_STATUS_PAUSE;
	for (pvIter = HLIB_LIST_First(s_pDownloadList); pvIter != NULL; pvIter = HLIB_LIST_Next(pvIter))
	{
		pstHandle = (ITK_AvstreamHandle_t*)HLIB_LIST_Data(pvIter);
		if(pstHandle->downloadStatus == MHEG_STREAMING_STATUS_WAITING)
		{
			mheg_itk_avsDownloadStart(pstHandle);
			break;
		}
	}

	return ERR_OK;
}

static HERROR mheg_itk_avsRemoveDownloadQueue(ITK_AvstreamHandle_t *pstStreamHandler)
{
//	DxDownload_Start_t		stDownloadStart;
//	ITK_AvstreamHandle_t 	*pstFirst;

	s_pDownloadList = HLIB_LIST_Remove(s_pDownloadList, (void*)pstStreamHandler);

	if(pstStreamHandler->downloadHandle)
	{
		APK_DOWNLOAD_Stop(pstStreamHandler->downloadHandle);
		pstStreamHandler->downloadHandle = 0;
	}

	return ERR_OK;
}

static HERROR mheg_itk_avsDiscardDownloadBuffer(void)
{
//	DxDownload_Start_t		stDownloadStart;
	HxList_t				*pvIter = NULL;
	ITK_AvstreamHandle_t 	*pstHandle = NULL;

	for (pvIter = HLIB_LIST_First(s_pDownloadList); pvIter != NULL; pvIter = HLIB_LIST_Next(pvIter))
	{
		pstHandle = (ITK_AvstreamHandle_t*)HLIB_LIST_Data(pvIter);

#if 1
		if(pstHandle!=NULL)
		{
			if(pstHandle->downloadHandle != HANDLE_NULL)
			{
				pstHandle->downloadHandle = 0;
				pstHandle->discardBuffer = TRUE;
			}
			pstHandle->downloadStatus = MHEG_STREAMING_STATUS_WAITING;
		}
#else
		if(pstHandle && pstHandle->downloadHandle)
		{
			APK_DOWNLOAD_Stop(pstHandle->downloadHandle);
			pstHandle->downloadHandle = 0;
			pstHandle->discardBuffer = TRUE;
		}
		pstHandle->downloadStatus = MHEG_STREAMING_STATUS_WAITING;
#endif
	}

	return ERR_OK;
}

#endif

static void mheg_itk_nvsResetFs(Mheg_Nvs_Fs_Type_t MhegFsType)
{
	//if (ITK_getManager(ITK_NVS))
	{
		/* mheg이 running중일때 nvs에 변화를 주면 안된다 */
		if ((s_stItkContext.stAtt.eStatus != REDKEY_RUNNING) || (MhegFsType == eMHEG_NVS_FACTORY_RESET))
		{
			PORT_ITK_Nvs_ResetFs(MhegFsType);
		}
	}
}

static void mheg_itk_miscShowPromptForGuidance(const char *restriction)
{
	HERROR				hErr;
	HBOOL				bLock = FALSE;
	DxGuidancePolicy_e	eGP = eDxGUIDANCEPOLICY_OFF;
	//DxRatingAge_e		eGP;

	hErr = APK_SCENARIO_GetGuidancePolicy(&eGP);
	//hErr = APK_SCENARIO_GetMaturityRating(&eGP);
	if (hErr == ERR_OK)
	{
		HxLOG_Debug(C_CODE_F_BLACK_B_YELLOW" GUIDANCE POLICY : %d "C_CODE_RESET"\n", eGP);
		bLock = (eGP == eDxGUIDANCEPOLICY_OFF) ? FALSE : TRUE;

		if (bLock == TRUE)
		{
			if (restriction == NULL)
			{
				bLock = FALSE;
			}
			else
			{
				APK_MEDIA_PLAY_RequestPincode(0, eOxMP_MEDIATYPE_LIVE, (HUINT8*)restriction);
				HxLOG_Debug(C_CODE_F_BLACK_B_YELLOW"block : %s  "C_CODE_RESET"\n",restriction);
			}
		}
	}

	if (bLock == FALSE)
	{
		mheg_itk_port_miscClosePincodeForPFG(TRUE);
	}
}

STATIC void mheg_itk_DsmccFsReplaceStr(HUINT8* strSource, HUINT8* strBefore, HUINT8* strAfter)
{
	HUINT8 	tmp[DxRCT_MAX_IMAGE_ICON_URL_LEN+1]="", *p;
	HUINT32	lenBefore  = HxSTD_StrLen(strBefore);
	HUINT32 	lenAfter = HxSTD_StrLen(strAfter);

	p = HxSTD_StrStr(strSource, strBefore);
	if(p != NULL)
	{
		HxSTD_StrNCpy(tmp, p + lenBefore, (DxRCT_MAX_IMAGE_ICON_URL_LEN+1));
		tmp[DxRCT_MAX_IMAGE_ICON_URL_LEN] = '\0';
		HxSTD_snprintf(p, (DxRCT_MAX_IMAGE_ICON_URL_LEN+1+lenAfter), "%s%s",strAfter, tmp);
	}
}

static void	mheg_itk_RequestDsmccFsFileData(HUINT32 ulViewId, HUINT8	*pucUrlName)
{
	if(pucUrlName == NULL)
		return;

	HxSTD_memset(szRctIconUrl, 0x00, DxRCT_MAX_IMAGE_ICON_URL_LEN);
	HxSTD_memcpy(szRctIconUrl, pucUrlName,  DxRCT_MAX_IMAGE_ICON_URL_LEN);

	if(HxSTD_StrLen(pucUrlName) > 0 && (HLIB_STD_StrCaseStr(pucUrlName, "dvb://") || HLIB_STD_StrCaseStr(pucUrlName, "dvb:/") ))
	{
		HUINT8	*pucPtr = NULL;

		if(HLIB_STD_StrCaseStr(pucUrlName, "dvb://"))
		{
			mheg_itk_DsmccFsReplaceStr(pucUrlName,(HUINT8 *)"dvb://",(HUINT8 *)"TEMPDVB");
		}

		HxLOG_Debug("Original Url (%s) ->Change Url (%s) \n",  szRctIconUrl, pucUrlName);

		pucPtr = HxSTD_StrChar(pucUrlName, '/');
		if(pucPtr!=NULL)
		{
			HxSTD_snprintf(szRctIconUrl,DxRCT_MAX_IMAGE_ICON_URL_LEN,"%s%s","DSM:/", pucPtr);

			HxLOG_Debug("[%s][%d] Request Url (%s) \n", __FUNCTION__, __LINE__, szRctIconUrl);

			if(PORT_ITK_DSMCCFS_LoadFile((HUINT8 *)szRctIconUrl) == NULL)
			{
				return ;
				// 따로 처리할 것이 없다..
				//MWC_UTIL_PostMsgToAp(MSG_MHEG_DSMCCFS_FILE_DATA, hAction, 0, 0, 0);
			}
		}
	}
}

static void	mheg_itk_CloseDsmccFsFile(HUINT32 ulViewId, void *pvFileData, HUINT32 ulFileDataSize, HINT32 nFileHandle )
{
	HCHAR	pszSrcPath[DxRCT_IMAGE_PATH_LEN];

	HxSTD_memset(pszSrcPath, 0x0, DxRCT_IMAGE_PATH_LEN);
	HxSTD_snprintf(pszSrcPath, DxRCT_IMAGE_PATH_LEN,"%s.%s", DxRCT_IMAGE_NAME, "dvb");

	if(ulFileDataSize != 0)
	{
		HERROR	hErr = ERR_OK;
		hErr = HLIB_FILE_WriteFile(pszSrcPath, pvFileData, ulFileDataSize);

		HxLOG_Debug(" HLIB_FILE_WriteFile hErr (%d) \n", hErr);
	}

	PORT_ITK_DSMCCFS_Release((ITK_DsmccFile_t *)nFileHandle);
}

#define ________MHEG_MESSAGE_HANDLER_______

static HERROR	mheg_itk_sendKeyToClient(HINT32 key_symbol, HINT32 key_code)
{
	HUINT32	ulRedKeyCode;
	itk_validation_t	eValidation;

	if (mheg_itk_Key2RedKeyCode(key_symbol, key_code, &ulRedKeyCode) == ERR_OK)
	{
		PORT_ITK_DISPLAY_SendKey(ulRedKeyCode, &eValidation);
		if (eValidation == ITK_VALID)
			return ERR_OK;
	}

	HxLOG_Error("MHEG doesn't consumed [%d] key\n", ulRedKeyCode);

	return ERR_FAIL;
}

static HERROR	mheg_itk_sendKeyToHost(HINT32 key_symbol, HINT32 key_code)
{
	MHEG_ITKContext_t	*context = mheg_itk_getContext();

	if (context->eItkProfile == REDKEY_PROFILE_CIPLUS)
		return ERR_FAIL;

	// 추후 Freesat 사양으로 확인 필요함..
	if (key_symbol == DIKS_EXIT)
	{
		s_pfnPostMsgCallback(eMHEG_RESTART_APPLICATION, 0, 0, 0, 0, 0, NULL, 0);
		return ERR_OK;
	}

	return ERR_FAIL;
}

static HERROR	mheg_itk_Key2RedKeyCode(HINT32 symbol, HINT32 code, HUINT32 *pulKeyCode)
{
	HUINT32					ulItkKeyCode = 0;

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
		case	DIKS_YELLOW:				ulItkKeyCode = ITK_KEY_YELLOW;			break;
		case	DIKS_BLUE:				ulItkKeyCode = ITK_KEY_BLUE;			break;

		case	DIKS_ENTER:				ulItkKeyCode = ITK_KEY_SELECT;			break;
		case	DIKS_EXIT:				ulItkKeyCode = ITK_KEY_CANCEL;		break;
		case	DIKS_TEXT:				ulItkKeyCode = ITK_KEY_TEXT;			break;
		case	DIKS_BACK:				ulItkKeyCode = ITK_KEY_CANCEL;		break;

		case	DIKS_INFO:				ulItkKeyCode = ITK_KEY_INFO;			break;
		case	DIKS_STOP:				ulItkKeyCode = ITK_KEY_STOP;			break;
		case	DIKS_PLAY:				ulItkKeyCode = ITK_KEY_PLAY;			break;
		case	DIKS_PAUSE:				ulItkKeyCode = ITK_KEY_PAUSE;			break;
		case	DIKS_PREVIOUS:			ulItkKeyCode = ITK_KEY_SKIP_BACK;		break;
		case	DIKS_NEXT:				ulItkKeyCode = ITK_KEY_SKIP_FORWARD;	break;
		case	DIKS_FASTFORWARD:		ulItkKeyCode = ITK_KEY_FAST_FORWARD;	break;
		case	DIKS_REWIND:				ulItkKeyCode = ITK_KEY_REWIND;			break;

		case	DIKS_CURSOR_UP:			ulItkKeyCode = ITK_KEY_UP;				break;
		case	DIKS_CURSOR_DOWN:		ulItkKeyCode = ITK_KEY_DOWN;			break;
		case	DIKS_CURSOR_LEFT:		ulItkKeyCode = ITK_KEY_LEFT;			break;
		case	DIKS_CURSOR_RIGHT:		ulItkKeyCode = ITK_KEY_RIGHT;			break;

		case	DIKS_EPG:				ulItkKeyCode = ITK_KEY_GUIDE;			break;
		case	DIKS_MENU:				ulItkKeyCode = ITK_KEY_MENU;			break;

		case	DIKS_HELP:				ulItkKeyCode = ITK_KEY_HELP;			break;

		default:
			HxLOG_Error("symbole (%d, %d) not converted..\n", symbol, code);
			break;
	}

	// RCT 존재하는 경우에 대한 처리.
	if((s_bBroadCastInterruptFlag == TRUE) && (ulItkKeyCode == ITK_KEY_GREEN))
	{
		if(s_ulRctLinkCount >0)
		{
			HxLOG_Debug("\033[30;43m(s_bBroadCastInterruptFlag == TRUE) &&  KEY_GREEN && s_ulRctLinkCount(%d) > 0 , So Key Pass  \033[0m\n", s_ulRctLinkCount);
			return ERR_FAIL;
		}
		else
		{
			HxLOG_Debug("\033[30;43m (s_bBroadCastInterruptFlag == TRUE) &&  KEY_GREEN && s_ulRctLinkCount(%d) <= 0 , So Key Consumed  \033[0m\n", s_ulRctLinkCount);
		}
	}

	*pulKeyCode = ulItkKeyCode;

	return ERR_OK;
}

static MHEG_START_ERR_CODE_t mheg_itk_StartMhegAppl(void)
{
	HERROR hResult = ERR_FAIL;

	HxLOG_Debug("===== (+) \n");

	hResult = PORT_ITK_SVC_Start();
	if (hResult != ERR_OK)
	{
		HxLOG_Error("ERROR : START REDKEY !!! \r\n");
		HxLOG_Debug("===== (-) \n");
		return eMHEG_START_FAIL;
	}

	APK_MEDIA_PLAY_SetMhegStatus(eMhegStatus_Running);
//	PORT_ITK_PostMsg(eMHEG_NOTIFY_APPSTATUS_CHANGED, 0, eMhegStatus_Running, 0, 0,0);

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"START REDKEY !!! (eMhegStatus_Running)"C_CODE_RESET"\n");
	HxLOG_Debug("===== (-) \n");

	return eMHEG_START_SUCCESS;
}

static void	mheg_itk_StopMhegAppl(HUINT32 ulViewId, itk_service_tune_event_t eTuneEvent)
{
	ITK_AvstreamHandle_t *pstStreamHandler = NULL;

	HxLOG_Debug("===== (+) \n");

	APK_MEDIA_PLAY_SetMhegStatus(eMhegStatus_NotRunning);
//	PORT_ITK_PostMsg(eMHEG_NOTIFY_APPSTATUS_CHANGED, 0, eMhegStatus_Stop, 0, 0,0);

	pstStreamHandler = port_itk_avs_GetHandle(mheg_itk_avsGetCurHandle());
	if(pstStreamHandler == NULL)
	{
		HxLOG_Warning("pstStreamHandler is Null \n");
		//return;
	}

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"pstStreamHandler(%p) eTuneEvent(0x%x)"C_CODE_RESET"\n", pstStreamHandler, eTuneEvent);
	HxLOG_Debug("eTuneEvent = 0x%x, nTuneFlag = 0x%x \n", eTuneEvent, s_stItkContext.nTuneFlag);

	if ((s_stItkContext.nTuneFlag & ITK_TUNE_KEEP_DISPLAY) == 0) // Non destructive tune 일 경우는 redkey만 stop하고 display는 그대로 유지하도록 함
	{
#if defined(CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		if(pstStreamHandler != NULL)
		{
			HxLOG_Debug("Cleaning IP Streaming resources !\n");
			mheg_itk_avsDestroyStreamingAv(pstStreamHandler);
		}
#endif

		PORT_ITK_SetMhegInternalRunningStatus(FALSE);

		/* restore demux resource before stop */
		//if ( ITK_getManager(ITK_DEMUX) && !(s_stItkContext.nTuneFlag & ITK_TUNE_KEEP_CONTEXT) ) // quite tune 일 경우 redkey 가 reset 하도록 함
		if (PORT_ITK_CheckEngineProfile(MHEG_ENGINE_PROFILE_UK | MHEG_ENGINE_PROFILE_UK_FSAT | MHEG_ENGINE_PROFILE_AUSTRALIA))
		{
			PORT_ITK_DEMUX_Restore(s_stItkContext.nTuneFlag);
		}

		// TODO: (process work) : still
		//PORT_ITK_Display_StopIFrameByOther(hAction);
		PORT_ITK_AUDIOCLIP_Restore();

		if(pstStreamHandler != NULL)
		{
			if (pstStreamHandler->bVideoFreeze == TRUE || pstStreamHandler->bAudioFreeze == TRUE)
			{
				HUINT32 ulViewId = pstStreamHandler->ulViewId;

				APK_MEDIA_PLAY_SetVideoFreeze(ulViewId, FALSE);
				APK_MEDIA_PLAY_SetAudioFreeze(ulViewId, FALSE);
				pstStreamHandler->bVideoFreeze = FALSE;
				pstStreamHandler->bAudioFreeze = FALSE;
			}
		}

		PORT_ITK_DISPLAY_Restore(eTuneEvent);

		PORT_ITK_DISPLAY_InitVideoScalingInfo();
	}

	HxLOG_Debug("Request Redkey Engine stop\n");

	if (s_stItkContext.eItkProfile == REDKEY_PROFILE_CIPLUS)
	{
		PORT_ITK_APPMMI_STOP(eTuneEvent);
	}

	PORT_ITK_SVC_Stop(eTuneEvent);

	HxLOG_Debug("===== (-) \n");
}

static void	mheg_itk_RetryStartMhegAppl(HUINT32 ulViewId, HINT32 nSleepTime, HINT32 nMsgSleepTime, HINT32 nMsgRetryCnt)
{
	HxLOG_Debug("===== (+) \n");

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"nSleepTime(%d) nMsgSleepTime(%d) nMsgRetryCnt(%d) "C_CODE_RESET"\n", nSleepTime,  nMsgSleepTime,  nMsgRetryCnt);

	if (nSleepTime > 0)
		HLIB_STD_TaskSleep(nSleepTime);

	nMsgSleepTime = (nMsgSleepTime >= 10) ? 10 /* in case of Error */ : 5 /* in case of waiting change of redkey status */;

	/* wait 2 ticks and retry to start Redkey */
	PORT_ITK_PostMsg(eMHEG_ENGINE_START_RETRY, ulViewId, nMsgSleepTime, nMsgRetryCnt, 0, 0);

	HxLOG_Debug("===== (-) \n");
}

static void mheg_itk_RestartMhegAppl(HUINT32 ulViewId)
{
	HxLOG_Debug("===== (+) \n");

	if(s_stItkContext.stAtt.eStatus != REDKEY_STOPPED)
	{
		HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Call mheg_itk_StopMhegAppl()"C_CODE_RESET"\n");
		mheg_itk_StopMhegAppl(ulViewId, ITK_SERVICE_TUNE_STOP_HOST);
#if 0 /* MHEG ICS006 Subtitle Control for Streaming 때문에 막음. */
		s_stItkContext.stAtt.bSubtitleEnableStatus = TRUE; // TODO: ITK Engine에서 설정해야하는데 왜 여기서 manual 하게 값을 변경하는지 확인 필요
#endif
	}

	HLIB_STD_TaskSleep(1000);			// for graphic display

	if(s_stItkContext.stAtt.eStatus == REDKEY_STOPPED)
	{
		if (mheg_itk_StartMhegAppl() == eMHEG_START_FAIL)
		{
			mheg_itk_RetryStartMhegAppl(ulViewId, 2, 5, 1);
		}
	}
	else
	{
		mheg_itk_RetryStartMhegAppl(ulViewId, 300, 5, 1);
	}

	HxLOG_Debug("===== (-) \n");
}

static void mheg_itk_ChangeSubtitleStatus(HUINT32 nViewId, HBOOL bEnable)
{
	HxLOG_Debug(C_CODE_F_WHITE_B_RED"\tSubtitle %s"C_CODE_RESET"\n", bEnable ? "ON" : "OFF");

	/* Subtitle / TTX */

	if (bEnable == FALSE)
	{
		HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", 0);
	}
	else
	{
		//	TODO: (process work) MHEG Subtitle Status확인후 on/off설정. 추가로 우선순위에 대한 기능 구현
		HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", 1);
	}
}

static void mheg_itk_ControlSubtitle(HUINT32 nViewId, HBOOL bEnable)
{
	HxLOG_Debug("===== (+) \n");
	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"subtitle : enable(%d), current status(%d)"C_CODE_RESET"\n", bEnable, s_stItkContext.stAtt.bSubtitleEnableStatus);

#if 0 /* HDR2000T에 의하면, change av일 때만  Video가 멈추면 (Video PID가 0이면) Subtitle을 멈추어 이곳에 또 체크할 필요 없다. */
	if (bEnable == TRUE)
	{
		/* Video가 멈추면 (Video PID가 0이면) Subtitle을 멈춘다.  */
		if (s_stItkContext.stAtt.bSubtitleEnableByVideo == FALSE)
		{
			HxLOG_Warning("skip subtitle control by video pid \n");
			HxLOG_Debug("===== (-) \n");
			return;
		}
	}
#endif

	if (bEnable && (s_ulCurPlayIdx != ICS_NO_PLAY))
	{
		HERROR		hErr;
		HBOOL		bSubtitleEnable=FALSE;

		hErr = APK_SCENARIO_GetSubtitleSetting(&bSubtitleEnable);

		if ( hErr == ERR_OK && (bSubtitleEnable == TRUE))
		{
			/* start subtitle. */
			mheg_itk_ChangeSubtitleStatus(nViewId, TRUE);
		}
		else
		{
			/* stop subtitle. */
			mheg_itk_ChangeSubtitleStatus(nViewId, FALSE);
		}
	}
	else
	{   /* stop subtitle. */
		mheg_itk_ChangeSubtitleStatus(nViewId, FALSE);
	}
	HxLOG_Debug("===== (-) \n");
}

#if defined (CONFIG_MW_MM_PVR)
static HBOOL mheg_itk_IsTuningConflict(Handle_t hOrgSvcHandle, Handle_t *hNewSvcHandle)
{
#if 0 // TODO: (process work) pvr
	HERROR						lResult = ERR_FAIL;
	MgrAction_ConflictInfo_t	cftInfo;

	lResult = MGR_ACTION_CheckTuningConflict(hOrgSvcHandle, &c
		ftInfo);
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
#else
	return FALSE;
#endif
}

static HBOOL mheg_itk_IsRecording(Handle_t hCurSvc)
{
#if 0 // TODO: (process work) pvr
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
#else
	return FALSE;
#endif
}
#endif


static HERROR mheg_itk_ChangeMhegStatusByOtherAppl(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HBOOL	bStartMheg = TRUE, bMhegRunning = FALSE;
	HUINT32	ulViewId;
	HERROR	hErr = ERR_OK;

	bStartMheg = HxOBJECT_INT(apArgv[0]);
	bMhegRunning	= mheg_itk_IsRunning();
	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	HxLOG_Debug("hErr(%d) ulViewId (%d) bStartMheg(%d) bMhegRunning(%d) s_ePapiStatus(%d) \n\n", hErr, ulViewId, bStartMheg, bMhegRunning, s_ePapiStatus);

	if(hErr == ERR_OK)
	{
		if(bStartMheg == TRUE)
		{

			if (s_ePapiStatus == ePAMA_STATUS_Operation)
			{
				if((bMhegRunning == FALSE) && (s_bBlockMheg == FALSE))
				{
#if 1
					MHEG_ITKContext_t		*pstContext = NULL;
					OxMediaPlay_MediaType_e	eMediaType = eOxMP_MEDIATYPE_NONE;

					pstContext = mheg_itk_getContext();
					if(pstContext != NULL)
					{
						hErr = APK_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);
						if (hErr == ERR_OK && eMediaType != eOxMP_MEDIATYPE_LIVE && pstContext->stAtt.nDefSvcUid != 0)
						{
							mheg_itk_RestartMhegAppl(ulViewId);
							if(mheg_itk_StartMhegAppl() == eMHEG_START_FAIL)
							{
								HxLOG_Error("mheg_itk_StartMhegAppl Fail \n");
							}
						}
						else
						{
							mheg_itk_PmtChanged(pstContext, ulViewId, 0, 0, eMhegPmtStatus_Received, 0);
						}
					}
#else

					mheg_itk_RestartMhegAppl(ulViewId);
					if(mheg_itk_StartMhegAppl() == eMHEG_START_FAIL)
					{
						HxLOG_Error("mheg_itk_StartMhegAppl Fail \n");
					}
#endif
				}
			}
			else
			{
				HxLOG_Debug("Call mheg_itk_StopMhegAppl ( ulViewId : %d) \n\n", ulViewId);
				mheg_itk_StopMhegAppl(ulViewId, ITK_SERVICE_TUNE_STOP_HOST);
			}
		}
		else
		{
			itk_service_tune_event_t	eTuneType;

			eTuneType = (itk_service_tune_event_t)HxOBJECT_INT(apArgv[1]);

			HxLOG_Debug("Call mheg_itk_StopMhegAppl( ulViewId :%d,  eTuneType:%d) \n\n", ulViewId, eTuneType);

			mheg_itk_StopMhegAppl(ulViewId, eTuneType);
		}
	}

	return ERR_OK;
}

static HERROR mheg_itk_BlockMheg(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	s_bBlockMheg = (HBOOL)HxOBJECT_INT(apArgv[0]);

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"Mheg is %s "C_CODE_RESET"\n", s_bBlockMheg == TRUE?"Block" : "UnBlock");

	return ERR_OK;
}

static HERROR mheg_itk_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HERROR		hErr;
	HUINT32		ulViewId;

	s_ePapiStatus = (HBOOL)HxOBJECT_INT(apArgv[0]);

	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	HxLOG_Debug(C_CODE_F_BLACK_B_GREEN"hErr(%d) ulViewId(%d) s_ePapiStatus is (%d) "C_CODE_RESET"\n", hErr, ulViewId, s_ePapiStatus );

	if( (hErr == ERR_OK) && (s_ePapiStatus == ePAMA_STATUS_Shutdown))
	{
		mheg_itk_StopMhegAppl(ulViewId, ITK_SERVICE_TUNE_STOP_HOST);
	}

	return ERR_OK;
}

#if defined(CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY )
static HERROR	mheg_itk_OnSubtitleStateChekForMhegIcs(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	/*	MHEG Profile Section 6, 6.3.5B, 6.3.5C,
		Video가 멈추면 (Video PID가 0이면) Subtitle을 멈춘다.
	*/
	if ((s_stItkContext.stAtt.avMode == MHEG_AV_MODE_BROADCAST) && (s_stItkContext.stAtt.bSubtitleEnableByVideo == FALSE))
	{
		mheg_itk_ControlSubtitle(s_stItkContext.ulViewId, FALSE);
	} else if ((s_stItkContext.stAtt.avMode > MHEG_AV_MODE_BROADCAST) &&
			  ((s_stItkContext.stAtt.pstViewStreamHandler != NULL) && (s_stItkContext.stAtt.pstViewStreamHandler->videoComponent == 0)))
	{	/* IC Stream 재생시, video track을 재생하지 않으면, dvb-subtitle를 보여 주지 않는다. */
		mheg_itk_ControlSubtitle(s_stItkContext.ulViewId, FALSE);
	}

	return ERR_OK;
}
#endif

static HERROR	mheg_itk_OnSubtitleState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HxLOG_Assert(nArgc == 1);

	PORT_ITK_MISC_NotifySubtitleChangedEvent();

	return ERR_OK;

}

static HERROR	mheg_itk_OnAudioDescriptionState(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{

	HxLOG_Assert(nArgc == 1);

	PORT_ITK_MISC_NotifyAudioDescriptionChangedEvent();

	return ERR_OK;

}

#ifdef	CONFIG_DEBUG
static void		mheg_itk_debug_InitCommandTools(void)
{
	HLIB_CMD_RegisterWord("demux", PORT_ITK_DEMUX_DEBUG_ShowRawTbInfo,"show_table","show raw table info","show_table");
	HLIB_CMD_RegisterWord("demux", PORT_ITK_DEMUX_DEBUG_GetXXX,"getxxx","test section filtering","getxxx");
	HLIB_CMD_RegisterWord("demux", PORT_ITK_DEMUX_DEBUG_ShowSiFilterList,"show_filter","show si filter list","show_filter");
	HLIB_CMD_RegisterWord("demux", PORT_ITK_DEMUX_DEBUG_ShowSiMonitorList,"show_monitor","show si monitor list","show_monitor");
}
#endif

HERROR	MHEG_PORT_ProcessMsg(MHEG_MSG_e eMsg, HINT32 nViewId, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3, HINT32 nParam4, void *pData, HUINT32 ulDataSize)
{
	MHEG_ITKContext_t	*context = mheg_itk_getContext();

#if defined(CONFIG_DEBUG)
	//HxLOG_Debug(C_CODE_F_BLACK_B_YELLOW"nViewId[%d]  [%s] "C_CODE_RESET"\n", nViewId, mheg_itk_MakeStrMhegMsgEvent(eMsg) );
#endif

	switch (eMsg)
	{
		case eMHEG_ENGINE_START:
			mheg_itk_EngineStart(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_NOTIFY_STATUS:
			mheg_itk_NotifyStatus(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_NOTIFY_APPSTATUS:
			mheg_itk_NotifyAppStatus(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;
		case eMHEG_NOTIFY_APPSTATUS_CHANGED:
			HAPI_BroadcastSignal("signal:onMhegNotifyAppStatus", 1, "i", nParam1);
			break;

		case eMHEG_RESTART_APPLICATION:
			mheg_itk_RestartApplication(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_DISPLAY_SUBTITLE_CONTROL:
			if(mheg_itk_IsRunning() == TRUE)
				mheg_itk_Display_SubtitleControl(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_DISPLAY_IMAGE_SHOW:
			mheg_itk_Display_ImageShow(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_DISPLAY_IMAGE_HIDE:
			mheg_itk_Display_ImageHide(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_DISPLAY_IMAGE_RELEASE:
			mheg_itk_Display_ImageRelease(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_HTTP_REGISTER_CALLBACK:
			HxLOG_Debug("eMHEG_HTTP_REGISTER_CALLBACK\n");
			break;

		case eMHEG_AVSTREAM_PRELOAD:
			HxLOG_Debug("eMHEG_AVSTREAM_PRELOAD\n");
			mheg_itk_avsPreloadStream((ITK_AvstreamHandle_t*)nParam1);
			break;

		case eMHEG_AVSTREAM_PLAY:
			HxLOG_Debug("eMHEG_AVSTREAM_PLAY\n");
			mheg_itk_avsPlayStream((ITK_AvstreamHandle_t*)nParam1, 0);
			break;

		case eMHEG_AVSTREAM_STOP:
			HxLOG_Debug("eMHEG_AVSTREAM_STOP\n");
			mheg_itk_avsStopStream((ITK_AvstreamHandle_t*)nParam1, 0);
			break;

		case eMHEG_AVSTREAM_RELEASE:
			mheg_itk_avsReleaseStream((ITK_AvstreamHandle_t*)nParam1, (ITK_RefSvc_t)nParam2);
			break;

		case eMHEG_AVSTREAM_SELECT_AUDIO:
			HxLOG_Debug("eMHEG_AVSTREAM_SELECT_AUDIO\n");
			mheg_itk_Avstream_SelectAudioComponent(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_AVSTREAM_SELECT_VIDEO:
			HxLOG_Debug("eMHEG_AVSTREAM_SELECT_VIDEO\n");
			mheg_itk_Avstream_SelectVideoComponent(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_AVSTREAM_DESELECT_AUDIO:
			mheg_itk_Avstream_DeselectAudioComponent(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_AVSTREAM_DESELECT_VIDEO:
			mheg_itk_Avstream_DeselectVideoComponent(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_AVSTREAM_SET_SPEED:
			HxLOG_Debug("eMHEG_AVSTREAM_SET_SPEED\n");
			mheg_itk_Avstream_SetSpeed(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eMHEG_AVSTREAM_SET_TRIGGER:
			HxLOG_Debug("eMHEG_AVSTREAM_SET_TRIGGER\n");
			mheg_itk_avsSetTrigger((ITK_AvstreamHandle_t *)nParam1, (itk_int32_t)nParam2, (itk_int32_t)nParam3);
			break;

		case eMHEG_AVSTREAM_CLEAR_TRIGGER:
			HxLOG_Debug("eMHEG_AVSTREAM_CLEAR_TRIGGER\n");
			mheg_itk_avsClearTrigger((ITK_AvstreamHandle_t *)nParam1, (itk_int32_t)nParam2);
			break;

		case eMHEG_AVSTREAM_SET_POSITION:
			HxLOG_Debug("eMHEG_AVSTREAM_SET_POSITION\n");
			mheg_itk_avsSetPosition((ITK_AvstreamHandle_t *)nParam1, (itk_int32_t)nParam2);
			break;

		case eMHEG_AVSTREAM_SET_END_POSITION:
			HxLOG_Debug("eMHEG_AVSTREAM_SET_END_POSITION\n");
			mheg_itk_avsSetEndPosition((ITK_AvstreamHandle_t *)nParam1, (itk_int32_t)nParam2);
			break;
#endif

		case eMHEG_DEMUX_SITABLE_NOTIFY:
			mheg_itk_Demux_SiTable_Notify(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_SERVICE_CHANGE_SVC:
			mheg_itk_ChangeSvc(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_MISC_PROMPT_FOR_GUIDANCE:
			mheg_itk_miscShowPromptForGuidance((const char*)nParam1);
			break;

		case eMHEG_APK_PFG_RESULT:
			mheg_itk_port_miscClosePincodeForPFG((HBOOL)nParam1);
			break;

		case eMHEG_MISC_LAUNCH_APPLICATION:
			HAPI_BroadcastSignal("signal:onMhegLaunchApplication", 1, "s", (HINT8 *)nParam3);
			break;

		case eMHEG_APK_SVCCHANGED:
			if (s_bChangeAVwithTune)
			{   /* change av with tune 이면, 현재 떠 있는 application을 이용한다. */
				s_bChangeAVwithTune = FALSE;
			}
			else
			{
				mheg_itk_SvcChanged(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			}
			break;

		case eMHEG_APK_SVCSTOPPED:
			mheg_itk_SvcStopped(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_APK_AVSTARTED:
			mheg_itk_AvStarted(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_APK_AVSTOPPED:
			mheg_itk_AvStopped(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_APK_PMTCHANGED:
			if(s_bBlockMheg == FALSE)
				mheg_itk_PmtChanged(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_APK_DMXCHANGED:
			if (PORT_ITK_GetDemuxId(nViewId) != nParam3)
			{
				PORT_ITK_SetDemuxId(nViewId, nParam3);
			}
			break;

		case eMHEG_APK_MONITORSTARTED:
			PORT_ITK_SetMonitoringMode(nViewId, eMHEG_MONITOR_MODE_Running);
			break;

		case eMHEG_APK_MONITORSTOPPED:
			PORT_ITK_SetMonitoringMode(nViewId, eMHEG_MONITOR_MODE_None);
			break;

		case eMHEG_APK_NOSIGNAL:
			if(mheg_itk_IsRunning() == TRUE)
			{
				mheg_itk_StopMhegAppl(s_stItkContext.ulViewId, ITK_SERVICE_TUNE_STOP_HOST);
			}
			break;

		case eMHEG_APK_LOCKED:
			s_stItkContext.stAtt.bMhegAvailable = FALSE;
			break;

		case eMHEG_APK_SUBTITLECHANGED:
			break;

		case eMHEG_APK_VIDEO_STATUS:
#if defined(CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eMHEG_APK_MEDIA_EVENT_VIDEOSTREAMINFO:
#endif
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("[%s]\t\t ulSessionId(0x%x) %s nParam1(0x%x)\n",
				mheg_itk_MakeStrMhegMsgEvent(eMsg), s_stItkContext.stAtt.ulSessionId, s_stItkContext.stAtt.ulSessionId == nParam1 ? "==":"!=" , nParam1);
#endif
			if(s_stItkContext.stAtt.ulSessionId == nParam1)
			{
				mheg_itk_VideoStatus(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			}
			break;

		case eMHEG_APK_SVCLISTUPDATED:
			break;

		case eMHEG_APK_SET_BROADCAST_INTERRUPTION:
			if(ITK_DISABLE == nParam1)
				s_bBroadCastInterruptFlag = FALSE;
			else
				s_bBroadCastInterruptFlag = TRUE;

			HAPI_BroadcastSignal("signal:MhegEventNotify", 2, "ii", 31 /*OPLBROADCAST_EVENT_TYPE_MHEG_INTERRUPTIBLE*/, s_bBroadCastInterruptFlag);
			APK_MEDIA_PLAY_SetMhegInterruptibleFlag((HUINT32) nViewId, s_bBroadCastInterruptFlag);

			break;

		case eMHEG_APK_AUDIOCLIPCOMPLETED:
			mheg_itk_AudioClipCompleted(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

#if defined (CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
		case eMHEG_SI_IP_STREAMING_PMT:
			mheg_itk_StreamingPmt(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_APK_MEDIA_EVENT_STARTED:
			//HxLOG_Debug("[#105313][not implemented] %s\n", mheg_itk_MakeStrMhegMsgEvent(eMsg));
			break;

		case eMHEG_APK_MEDIA_EVENT_PLAYTIME:
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("[%s]\t\t ulSessionId(0x%x) %s nParam1(0x%x)\n",
				mheg_itk_MakeStrMhegMsgEvent(eMsg), s_stItkContext.stAtt.ulSessionId, s_stItkContext.stAtt.ulSessionId == nParam1 ? "==":"!=" , nParam1);
#endif
			if(s_stItkContext.stAtt.pstViewStreamHandler == NULL)
				break;

			if(s_stItkContext.stAtt.ulSessionId == nParam1)
			{
				if ((s_stItkContext.stAtt.avMode > MHEG_AV_MODE_BROADCAST)
					 &&(mheg_itk_avsIsSetupStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler) == TRUE))
				{
					mheg_itk_avsChangePositionStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler);
				}
			}
			break;

		case eMHEG_APK_MEDIA_EVENT_PLAYSTATE:
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("[%s]\t\t ulSessionId(0x%x) %s nParam1(0x%x)\n",
				mheg_itk_MakeStrMhegMsgEvent(eMsg), s_stItkContext.stAtt.ulSessionId, s_stItkContext.stAtt.ulSessionId == nParam1 ? "==":"!=" , nParam1);
#endif
			if(s_stItkContext.stAtt.ulSessionId == nParam1)
			{
				OxMediaPlay_PlayState_e ePlayState;
				ITK_AvstreamHandle_t	*pstStreamHandler = NULL;

				APK_MEDIA_PLAY_GetPlayState( nViewId, &ePlayState);

				pstStreamHandler = s_stItkContext.stAtt.pstViewStreamHandler;
				if(pstStreamHandler == NULL)
					break;

				switch(ePlayState)
				{
					case eOxMP_PLAYSTATE_PLAYING:
						HxLOG_Debug("eOxMP_PLAYSTATE_PLAYING\n");
						if ((s_stItkContext.stAtt.avMode > MHEG_AV_MODE_BROADCAST)
				 			&&(mheg_itk_avsIsSetupStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler) == TRUE))
						{
							mheg_itk_avsUnderflowResumeStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler); // For DTG Test
							mheg_itk_avsControlStreamingSubtitle(nViewId, TRUE);
							mheg_itk_avsStartStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler);
						}
						break;

					case eOxMP_PLAYSTATE_PAUSE:
					case eOxMP_PLAYSTATE_STOPPED:
						HxLOG_Debug("eOxMP_PLAYSTATE_PAUSE/eOxMP_PLAYSTATE_STOPPED\n");
						if (s_stItkContext.stAtt.avMode > MHEG_AV_MODE_BROADCAST)
						{
							mheg_itk_avsControlStreamingSubtitle(nViewId, FALSE);

							if(ePlayState == eOxMP_PLAYSTATE_STOPPED)
							{
								if(mheg_itk_avsProcessMPEventError(s_stItkContext.stAtt.pstViewStreamHandler) == ERR_OK)
								{

								}
							}
						}
						break;

					case eOxMP_PLAYSTATE_EOS:
						HxLOG_Debug("eOxMP_PLAYSTATE_EOS\n");
						if ((s_stItkContext.stAtt.avMode > MHEG_AV_MODE_BROADCAST)
							 &&(mheg_itk_avsIsSetupStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler) == TRUE))
						{
							//mheg_itk_avsUnderflowResumeStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler); // For DTG Test
							mheg_itk_avsEofPositionStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler);
							mheg_itk_avsControlStreamingSubtitle(nViewId, FALSE);
						}
						break;
					default:
						HxLOG_Debug("eMHEG_APK_MEDIA_EVENT_PLAYSTATE : Not Implemeted (ePlayState : %d ) \n", ePlayState);
						break;
				}
			}
			break;

		case eMHEG_APK_MEDIA_EVENT_PLAYERROR:
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("[%s]\t\t ulSessionId(0x%x) %s nParam1(0x%x)\n",
				mheg_itk_MakeStrMhegMsgEvent(eMsg), s_stItkContext.stAtt.ulSessionId, s_stItkContext.stAtt.ulSessionId == nParam1 ? "==":"!=" , nParam1);
#endif
			if(s_stItkContext.stAtt.ulSessionId == nParam1)
			{
				OxMediaPlay_PlayError_e	ePlayError;
				OxMediaPlay_ErrorReason_e eReason;
				ITK_AvstreamHandle_t	*pstStreamHandler;

				pstStreamHandler = s_stItkContext.stAtt.pstViewStreamHandler;
				if(pstStreamHandler == NULL)
					break;

				APK_MEDIA_PLAY_GetPlayError( nViewId, &ePlayError, &eReason);
				switch(ePlayError)
				{
					case eOxMP_PLAYERROR_PROBE_FAILED:
						HxLOG_Debug("eOxMP_PLAYERROR_PROBE_FAILED\n");
						if (eReason == eOxMP_ERROR_REASON_X_KEY)
						{
							/* to do */
						}
						if (mheg_itk_avsIsSettingStreamingAV(pstStreamHandler) == TRUE)
						{
							//mheg_itk_avsProcessMPEventProbe(nViewId, FALSE);
						}
						//break;
						// probe fail 처리후 아래 play error 도 같이 처리 해야 하는가?
					default:
						HxLOG_Debug("Error : %x\n", ePlayError);
						if(mheg_itk_avsIsSettingStreamingAV(pstStreamHandler) == TRUE)
						{
							mheg_itk_avsProcessMPEventError(pstStreamHandler);
						}
						break;
				}
			}
			break;
		case eMHEG_APK_MEDIA_EVENT_MEDIA_BufferStateChanged:
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("[%s]\t\t ulSessionId(0x%x) %s nParam1(0x%x)\n",
				mheg_itk_MakeStrMhegMsgEvent(eMsg), s_stItkContext.stAtt.ulSessionId, s_stItkContext.stAtt.ulSessionId == nParam1 ? "==":"!=" , nParam1);
#endif
			if(s_stItkContext.stAtt.ulSessionId == nParam1)
			{
				OxMediaPlay_BufferState_e eBufferState;
				ITK_AvstreamHandle_t	*pstStreamHandler;

				pstStreamHandler = s_stItkContext.stAtt.pstViewStreamHandler;
				if(pstStreamHandler == NULL)
					break;

				APK_MEDIA_PLAY_GetBufferState( nViewId, &eBufferState);
				switch(eBufferState)
				{
					case eOxMP_BUFFERSTATE_CanPlay:
						HxLOG_Debug("eOxMP_BUFFERSTATE_CanPlay\n");
						if (mheg_itk_avsIsSettingStreamingAV(pstStreamHandler) == TRUE)
						{
							//mheg_itk_avsProcessMPEventProbe(nViewId, TRUE);
						}
						break;
					case eOxMP_BUFFERSTATE_Enough:
						HxLOG_Debug("eOxMP_BUFFERSTATE_Enough\n");
						if ((s_stItkContext.stAtt.avMode > MHEG_AV_MODE_BROADCAST)
							 &&(mheg_itk_avsIsSetupStreamingAV(pstStreamHandler) == TRUE))
						{
							mheg_itk_avsUnderflowResumeStreamingAV(pstStreamHandler);
						}
						break;

					case eOxMP_BUFFERSTATE_NotEnough:
						HxLOG_Debug("eOxMP_BUFFERSTATE_NotEnough\n");
						if ((s_stItkContext.stAtt.avMode > MHEG_AV_MODE_BROADCAST)
							 &&(mheg_itk_avsIsSetupStreamingAV(pstStreamHandler) == TRUE))
						{
							mheg_itk_avsControlStreamingSubtitle(nViewId, FALSE);
							mheg_itk_avsUnderflowStreamingAV(pstStreamHandler);
						}
						break;

					default:
						break;
				}
			}
			break;
		case eMHEG_APK_AVRESUME:

			if(s_stItkContext.stAtt.pstViewStreamHandler == NULL)
				break;

			if ((s_stItkContext.stAtt.avMode > MHEG_AV_MODE_BROADCAST)
				 &&(mheg_itk_avsIsSetupStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler) == TRUE))
			{
				mheg_itk_avsUnderflowResumeStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler);
			}
			break;
		case eMHEG_APK_AVUNDERRUN:
			if(s_stItkContext.stAtt.pstViewStreamHandler == NULL)
				break;

			if ((s_stItkContext.stAtt.avMode > MHEG_AV_MODE_BROADCAST)
				 &&(mheg_itk_avsIsSetupStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler) == TRUE))
			{
				mheg_itk_avsControlStreamingSubtitle(nViewId, FALSE);
				mheg_itk_avsUnderflowStreamingAV(s_stItkContext.stAtt.pstViewStreamHandler);
			}
			break;
		case eMHEG_APK_MEDIA_EVENT_MEDIA_BufferInfoChanged:
			//HxLOG_Debug("[#105313][not implemented] %s\n", mheg_itk_MakeStrMhegMsgEvent(eMsg));
			break;

		case eMHEG_APK_MEDIA_EVENT_STOPPED:
			//HxLOG_Debug("[#105313][not implemented] %s\n", mheg_itk_MakeStrMhegMsgEvent(eMsg));
			break;

		case eMHEG_APK_MEDIA_EVENT_SPEED_CHANGED:
			//HxLOG_Debug("[#105313][not implemented] %s\n", mheg_itk_MakeStrMhegMsgEvent(eMsg));
			break;

		case eMHEG_APK_MEDIA_EVENT_SupportedSpeedChanged:
			//HxLOG_Debug("[#105313][not implemented] %s\n", mheg_itk_MakeStrMhegMsgEvent(eMsg));
			break;

		case eMHEG_APK_MEDIA_EVENT_PlayInfo:
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("[%s]\t\t ulSessionId(0x%x) %s nParam1(0x%x)\n",
				mheg_itk_MakeStrMhegMsgEvent(eMsg), s_stItkContext.stAtt.ulSessionId, s_stItkContext.stAtt.ulSessionId == nParam1 ? "==":"!=" , nParam1);
#endif
			if(s_stItkContext.stAtt.pstViewStreamHandler == NULL)
				break;

			if(s_stItkContext.stAtt.pstViewStreamHandler->downloadHandle != 0)
			{
 				mheg_itk_avsSaveDefaultAudioVideo(s_stItkContext.stAtt.pstViewStreamHandler);
 				mheg_itk_avsPlayStreamingAVPlay(s_stItkContext.stAtt.pstViewStreamHandler);
			}
			break;

		case eMHEG_APK_MEDIA_EVENT_MEDIA_PumpIndexChanged:
			//HxLOG_Debug("[#105313][not implemented] %s\n", mheg_itk_MakeStrMhegMsgEvent(eMsg));
			break;

		case eMHEG_APK_MEDIA_EVENT_MEDIA_SeekReady:
			//HxLOG_Debug("[#105313][not implemented] %s\n", mheg_itk_MakeStrMhegMsgEvent(eMsg));
			break;

		case eMHEG_AVSTREAM_DOWNLOAD_START:
			break;

		case eMHEG_AVSTREAM_DOWNLOAD_COMPLETE:
			{
				ITK_AvstreamHandle_t *pstStreamHandler;

				pstStreamHandler = mheg_itk_avsFindStreamHandlerByDownloadHandle(nParam1);
				if(pstStreamHandler)
				{
					/* pause시에 다음 stream을 다운로드 하므로, 여기서 처리하지 않는다. */
					if (pstStreamHandler->downloadStatus != MHEG_STREAMING_STATUS_PAUSE)
					{
						mheg_itk_avsFinishDownload(pstStreamHandler);
					}
				}
			}
			break;

		case eMHEG_AVSTREAM_DOWNLOAD_PROBE_SUCCESS:
			{
				ITK_AvstreamHandle_t *pstStreamHandler;

				pstStreamHandler = mheg_itk_avsFindStreamHandlerByDownloadHandle(nParam1);
				if(pstStreamHandler)
				{
					if(mheg_itk_avsIsSetupStreamingAV(pstStreamHandler) == FALSE)
					{
						mheg_itk_avsProcessMPEventProbe(pstStreamHandler, (HUINT32)nParam2, (HUINT32)nParam3, (HUINT32)nParam4, TRUE);
					}
				}
			}
			break;

		case eMHEG_AVSTREAM_DOWNLOAD_PROBE_FAILED:
			{
				ITK_AvstreamHandle_t 	*pstStreamHandler;
				itk_media_event_data_t	stMediaEventData;
				itk_int32_t			preload = ITK_PRESENT;

				pstStreamHandler = mheg_itk_avsFindStreamHandlerByDownloadHandle(nParam1);
				if(pstStreamHandler)
				{
					if (mheg_itk_avsIsSettingStreamingAV(pstStreamHandler) == TRUE)
					{
						OxMediaPlay_ErrorReason_e eReason = (OxMediaPlay_ErrorReason_e)nParam2;
						mheg_itk_avsProcessMPEventProbe(pstStreamHandler, (HUINT32)NULL, (HUINT32)NULL, (HUINT32)NULL, FALSE);

						/* MEDIA 2.0에서 이 경우는 AP_MPlayer_Create()가 FAIL 일 경우만 해당됨 */
						mheg_itk_avsDestroyStreamingAv(pstStreamHandler);
						mheg_itk_avsRemoveDownloadQueue(pstStreamHandler);
						pstStreamHandler->downloadStatus = MHEG_STREAMING_STATUS_DONE; // 실패로 download 종료, 그래서 done으로 표시한다.

						HxLOG_Error("Fail to create Streaming AV!!!\n");
						preload = ITK_NOT_PRESENT;
						stMediaEventData.preloadData.result = preload;
						HxLOG_Error("ProbeFail - reason(%d)!!!\n", eReason);
						if (eReason == eOxMP_ERROR_REASON_X_KEY)
						{
							stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_KEY_ERROR;
						}
						else
						{
							stMediaEventData.preloadData.resultData.error = (itk_preload_error_t)ITK_MEDIA_NOT_FOUND;
						}
						mheg_itk_avsEventCallback(pstStreamHandler, ITK_MEDIA_PRELOAD, &stMediaEventData);
					}
				}
			}
			break;

		case eMHEG_AVSTREAM_DOWNLOAD_INFO_CHANGED:
			{
				ITK_AvstreamHandle_t *pstStreamHandler;

				pstStreamHandler = mheg_itk_avsFindStreamHandlerByDownloadHandle(nParam1);
				if(pstStreamHandler && (mheg_itk_avsIsSetupStreamingAV(pstStreamHandler) == TRUE)) // Probe 까지 되었다면....
				{
					if (pstStreamHandler->downloadStatus == MHEG_STREAMING_STATUS_DOWNLOADING)
					{
						HINT64	llBufferedBytes = 0;
						HUINT32 ulBufferedTsPackets = 0;

						APK_DOWNLOAD_GetBufferedBytes(nParam1, &llBufferedBytes);
						ulBufferedTsPackets = (HUINT32) (llBufferedBytes/TS_PACKET_BYTES);

						if ((pstStreamHandler->endPosition > 0 ) && (ulBufferedTsPackets >= pstStreamHandler->endPosition))
						{
							HxLOG_Debug("downloadHandle(%x) ulBufferedTsPackets(%d) is bigger than endPosition(%d) \n",  nParam1, ulBufferedTsPackets, pstStreamHandler->endPosition);
							/* pause 를 시키면, 다음 스트림을 다운 받음.  */
							APK_DOWNLOAD_Pause(nParam1);
							mheg_itk_avsPauseDownload(pstStreamHandler);
						}
					}
#if 0 // for debug
					{
						HINT64 llBufferedBytes = 0;
						HUINT32 ulBufferedTsPackets = 0;
						APK_DOWNLOAD_GetBufferedBytes(nParam1, &llBufferedBytes);
						ulBufferedTsPackets = (HUINT32) (llBufferedBytes/TS_PACKET_BYTES);
						printf("[%s : %d] downloadHandle(%x) llBufferedTsPackets(%d)  endPosition(%d) Position(%d)\n", __FUNCTION__, __LINE__,
							nParam1, ulBufferedTsPackets, pstStreamHandler->endPosition, pstStreamHandler->position);
					}
#endif
				}
			}
			break;
#endif

		case eMHEG_NETWORK_EVENT:
#if defined(CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY )
			mheg_itk_NotifiyNetworkStatus(FALSE);
#endif
			break;

		case eMHEG_KEY_DOWN:
			mheg_itk_KeyDown(context, nViewId, nParam1, nParam2, nParam3, nParam4, pData, ulDataSize);
			break;

		case eMHEG_ENGINE_START_RETRY:
			mheg_itk_EngineStartRetry(context, nViewId, nParam1, nParam2, nParam3, nParam4);
			break;

		case eMHEG_APK_FACTORY_DEFAULT:
			if(mheg_itk_IsRunning() == TRUE)
			{
				mheg_itk_StopMhegAppl(s_stItkContext.ulViewId, ITK_SERVICE_TUNE_STOP_HOST);
			}
			mheg_itk_nvsResetFs(eMHEG_NVS_FACTORY_RESET);
			break;

		case eMHEG_APK_DSMCCFS_REQUEST_FILE:
			/*nParam3 = file url  */
			HxLOG_Debug("eMHEG_APK_DSMCCFS_REQUEST_FILE ==== url (%s)\n", (HUINT8 *)nParam3);
			mheg_itk_RequestDsmccFsFileData(nViewId, (HUINT8*)nParam3);
			break;

		case eMHEG_DSMCCFS_FILE_DATA:
			/*nParam1 = filedata, nParam2 =  filesize, nParam3 = filehandle*/
			HxLOG_Debug("eMHEG_DSMCCFS_FILE_DATA ==== FileHandle(0x%x) FileData(%d) url (%s) \n",nParam3, nParam2, (HUINT8 *)szRctIconUrl);
			mheg_itk_CloseDsmccFsFile(nViewId, (void *)nParam1, nParam2, nParam3);
			break;
		case eMHEG_APK_RCTCHANGED:
			HxLOG_Debug("s_ulRctLinkCount (%d)\n",  s_ulRctLinkCount);
			s_ulRctLinkCount = nParam1;
			break;
		default:
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	MHEG_PORT_Init(PostMsgCB_t pfnPostMsgCallback)
{
	MHEG_EngineProfile_e	eEngineProfile;
	redkey_profile_t		eItkProfile;
	HINT32				nItkMemSize;
	itk_errcode_t			errCode;

	HxLOG_Debug("<.....\n");
	s_pfnPostMsgCallback = pfnPostMsgCallback;

	if (PORT_ITK_GetEngineVersion(s_szItkVersion, ITK_VERSION_INFO_MAX) != ERR_OK)
		HxLOG_Assert(0);

	HxLOG_Debug("Initiated Redkey version (%s)\n", s_szItkVersion);

	PORT_ITK_RegisterCallback(pfnPostMsgCallback);

	/* initialize MHEG Context */
	mheg_itk_initContext();

	/* Register engine profile : we must set engine profile first !!!!!! */
	mheg_itk_registerEngineProfile(&eEngineProfile, &eItkProfile, &nItkMemSize);

	/* initializes OS ....... */
	PORT_ITK_OS_Init();
	PORT_ITK_OS_CreateObject();

	/* initializes DISPLAY ....... */
	PORT_ITK_DISPLAY_CreateObject(eItkProfile);
	PORT_ITK_DISPLAY_InitVideoScalingInfo();
	PORT_ITK_DISPLAY_Initialize();

	/* initializes SVC MANAGER ....... */
	PORT_ITK_SVC_Init();
	PORT_ITK_SVC_CreateObject(eItkProfile);

	/* initializes MISC ....... */
	PORT_ITK_MISC_CreateObject(eItkProfile); // TODO: subtitle, Audio Description, Pin setting, ValidPIN 등 추가작업필요

	/* initializes FONT Driver ....... */
	PORT_ITK_FONTDRIVER_CreateObject();

	if (eEngineProfile & (MHEG_ENGINE_PROFILE_UK | MHEG_ENGINE_PROFILE_UK_FSAT | MHEG_ENGINE_PROFILE_AUSTRALIA))
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
		MHEG_SEF_Init();
		PORT_ITK_DEMUX_Init();
		MHEG_SI_Init();
		PORT_ITK_DEMUX_CreateObject();

		/* initiate Zlib */
		PORT_ITK_ZLIB_CreateObject();
	}

	/* initializes Nvs ....... */
	PORT_ITK_Nvs_CreateObject(eItkProfile);

#if defined(CONFIG_3RD_MHEG_USE_PVR_FUNCTIONALITY)
	PORT_ITK_Dvr_CreateObject();
#endif

	if (eEngineProfile & MHEG_ENGINE_PROFILE_CI_PLUS)
	{
		/* initialize CI AppMMI */
		PORT_ITK_APPMMI_Init();
		PORT_ITK_APPMMI_CreateObject();
	}

	/* initializes ITK Environment */
	if(ITK_init() == ITKE_OK)
	{
		HxLOG_Debug("ITK_init is Ok.\r\n");
	}
	else
	{
		HxLOG_Error("Fail to call ITK_init() !!!\r\n");
		return ERR_FAIL;
	}

	if((errCode = RedKey_init(eItkProfile, nItkMemSize,
				mheg_itk_callbackRedKeyState, NULL,
				mheg_itk_callbackRedkeyAppState, NULL)) != ITKE_OK)
	{
		HxLOG_Error("Fail to call RedKey_init() ___ profile(%d) - errcode [%d]!!! \r\n", eItkProfile, errCode);
		return ERR_FAIL;
	}
	HxLOG_Debug("Success initializing ITK Task !!\r\n");
#if defined(CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY)
	/* initiate DSMCC-File System (Note. This interface can only be used when RedKey is in state REDKEY_RUNNING) */
	PORT_ITK_DSMCCFS_Init();
#endif

#ifdef	CONFIG_DEBUG
	mheg_itk_debug_InitCommandTools();
#endif

#if defined(CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY )
	mheg_itk_NotifiyNetworkStatus(TRUE);
#endif

	HAPI_ConnectSignal("signal:ChangeMhegStatusByOtherAppl",    NULL, mheg_itk_ChangeMhegStatusByOtherAppl);
	HAPI_ConnectSignal("signal:onAudioDescriptionState",     		NULL, mheg_itk_OnAudioDescriptionState);
 	HAPI_ConnectSignal("signal:onSubtitleStateOplLocalSystem",  	NULL, mheg_itk_OnSubtitleState);
	HAPI_ConnectSignal("signal:RequestMhegBlock",    			NULL, mheg_itk_BlockMheg);
	HAPI_ConnectSignal("signal:onSystemStateChanged",		NULL, mheg_itk_onSystemStateChanged);
#if defined(CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY )
 	HAPI_ConnectSignal("signal:onSubtitleStateChekForMhegIcs",  	NULL, mheg_itk_OnSubtitleStateChekForMhegIcs);	 // ICS023 Scene 5 버그 수정. (video hide 상태에서 subtitle on 이면 subtitle 나오는 문제 수정...
#endif
	return ERR_OK;
}


#if defined(CONFIG_3RD_MHEG_IP_STREAMING_FUNCTIONALITY )


/**
* This function is used to notify the network status (Connected or Disconnected (inlcuding LAN and WLAN)) to MHEG APP
* @param bForce 	(IN Parameter) FALSE: just notify if the network status is changed, TRUE: always notify
* @return void
*/

static void	mheg_itk_NotifiyNetworkStatus(HBOOL bForce)
{
	static HBOOL bNetworkConnected = FALSE;
	HINT32 numOfDev = 0, i = 0;
	APKS_NETDEVINFO_t stDevInfo;
	HxSTD_MemSet(&stDevInfo, 0x00, sizeof(APKS_NETDEVINFO_t));

	if (APK_NETWORK_CONF_GetNumOfNetDev(&numOfDev) != ERR_OK)
	{
		HxLOG_Error("Getting the number of Network Interfaces is failed.\n");
	}

	for ( i = 0; i <numOfDev; i++ )
	{
		if (APK_NETWORK_CONF_GetDevInfo( i, &stDevInfo ) != ERR_OK)
		{
			HxLOG_Error("Getting the information of Network Interface is failed.\n");
		}
		else if (stDevInfo.bNetActive) // When LAN or WLAN connection is active (received IP address)
		{
			if(bForce || !bNetworkConnected)
			{
				bNetworkConnected = TRUE;
				PORT_ITK_Http_Notify(TRUE);
			}
			return;
		}
	}
	if(bForce || bNetworkConnected)
	{
		bNetworkConnected = FALSE;
		PORT_ITK_Http_Notify(FALSE);
	}
}

#endif

