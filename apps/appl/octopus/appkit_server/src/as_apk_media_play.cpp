#include <apk.h>
#include <oapi_media_play.h>

#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_media_play.h>

#define MAX_BUFFSIZE	2048

char const * const cApkMediaPlay::BUSNAME_APK_MEDIA_PLAY = "Octopus.Appkit.Media.Play";
char const * const cApkMediaPlay::BUSPATH_APK_MEDIA_PLAY = "/Octopus/Appkit/Media/Play";

STATIC const char * _apk_GetEventString (OxMediaPlay_Event_e eEvent)
{
	switch (eEvent)
	{
		ENUM_TO_STR (eOxMP_EVENT_TUNE_Locked)
		ENUM_TO_STR (eOxMP_EVENT_TUNE_NoSignal)

		ENUM_TO_STR (eOxMP_EVENT_TUNE_AntennaOk)
		ENUM_TO_STR (eOxMP_EVENT_TUNE_AntennaNok)

		ENUM_TO_STR (eOxMP_EVENT_LIVE_SvcChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_SvcStopped)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_ConflictChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_EventRelay)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_Ews)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_TSRTimecodeChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_BsCtrlStarted)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_PinCtrlChangedByBCAS)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_ViewStateChanged)
		ENUM_TO_STR (eOxMP_EVENT_LIVE_LockStateChanged)

		ENUM_TO_STR (eOxMP_EVENT_MEDIA_ProbeErrored)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_StopErrored)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_BufferStateChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_BufferInfoChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_PlayInfo)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_PumpIndexChanged)
		ENUM_TO_STR (eOxMP_EVENT_MEDIA_SeekReady)


		ENUM_TO_STR (eOxMP_EVENT_BOF)
		ENUM_TO_STR (eOxMP_EVENT_EOF)
		ENUM_TO_STR (eOxMP_EVENT_STARTED)
		ENUM_TO_STR (eOxMP_EVENT_STOPPED)
		ENUM_TO_STR (eOxMP_EVENT_PLAYING)
		ENUM_TO_STR (eOxMP_EVENT_PAUSE)
		ENUM_TO_STR (eOxMP_EVENT_TRICK)
		ENUM_TO_STR (eOxMP_EVENT_RATING_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_AV_STARTED)
		ENUM_TO_STR (eOxMP_EVENT_AV_STOPPED)
		ENUM_TO_STR (eOxMP_EVENT_SPEED_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_AV_MEDIARECT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SupportedSpeedChanged)
		ENUM_TO_STR (eOxMP_EVENT_PinCtrlChanged)

		ENUM_TO_STR (eOxMP_EVENT_SI_PMT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_EIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_SIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_AIT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_RCT_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_SI_NETWORK_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_VIDEO_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_AUDIO_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_SUBTITLE_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_COMPONENT_SELECTED)

		ENUM_TO_STR (eOxMP_EVENT_DRM_BLOCK)

		ENUM_TO_STR (eOxMP_EVENT_RESOURCE_TAKEN)
		ENUM_TO_STR (eOxMP_EVENT_RESOURCE_LOST)

		ENUM_TO_STR (eOxMP_EVENT_PLAYERROR)
		ENUM_TO_STR (eOxMP_EVENT_PLAYSTATE)
		ENUM_TO_STR (eOxMP_EVENT_PLAYTIME)

		ENUM_TO_STR (eOxMP_EVENT_RADIOBG_CHANGED)
		ENUM_TO_STR (eOxMP_EVENT_TTX_UPDATE)
		ENUM_TO_STR (eOxMP_EVENT_SBTL_UPDATE)


		ENUM_TO_STR (eOxMP_EVENT_TrickRestrictModeChanged)


	default:
		break;
	}

	return "Unknown";
}

static void _apk_msg_task(void *arg)
{
FILE *fp1 = NULL;
char buf[1024];
fp1 = fopen("/test.txt", "w");
	while (TRUE)
	{
		FILE* fp = stdout;
		fgets(buf, 1024, fp);


		fputs(buf, fp1);


		if (APK_EVENT_Dispatch() == 0)
			HLIB_STD_TaskSleep(100);
	}


}

void cApkMediaPlay::callback_Event (HUINT32 ulViewId, HUINT32 ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData)
{
	cApkMediaPlay *pThis = (cApkMediaPlay *)pvUserData;
	std::string	eventStr(_apk_GetEventString(eEvent));

	uint32_t param1=0, param2=0, param3=0;

	/* emit signal */
	if (NULL != pstNotifyData)
	{
		param1 = (uint32_t)pstNotifyData->stEventNotify.ulParam1;
		param2 = (uint32_t)pstNotifyData->stEventNotify.ulParam2;
		param3 = (uint32_t)pstNotifyData->stEventNotify.ulParam3;
	}

	pThis->event ((uint32_t)eEvent, eventStr, param1, param2, param3);
}

cApkMediaPlay::cApkMediaPlay(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_MEDIA_PLAY)
{
	HUINT32 ulTaskId;
	VK_TASK_Create(_apk_msg_task, 40, SIZE_16K, "OctopusApk", NULL, (unsigned long *)&ulTaskId, 0);
	VK_TASK_Start(ulTaskId);

	/* apkit media play */
    {
    	HUINT32		ulViewId;

		APK_MEDIA_PLAY_Init();
		APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
		APK_MEDIA_PLAY_RegisterNotifier(ulViewId, cApkMediaPlay::callback_Event, NULL, this);
    }
}

uint32_t cApkMediaPlay::GetViewNumber()
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulViewNum = 0;


	hErr = APK_MEDIA_PLAY_GetViewNumber (&ulViewNum);

	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ulViewNum;
}

void cApkMediaPlay::SetMainViewId(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetMainViewId ((HUINT32)viewId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

uint32_t cApkMediaPlay::GetMainViewId()
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulViewId = 0;

	hErr = APK_MEDIA_PLAY_GetMainViewId (&ulViewId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulViewId;
}

uint32_t cApkMediaPlay::GetPlayType(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_MediaType_e eType;

	hErr = APK_MEDIA_PLAY_GetPlayType ((HUINT32)viewId, &eType);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)eType;

	/*
	 eOxMP_MEDIATYPE_NONE			=	0
	 eOxMP_MEDIATYPE_LIVE			=	1
	 eOxMP_MEDIATYPE_PVRPLAYBACK	=	2
	 eOxMP_MEDIATYPE_TSRPLAYBACK	=	3
	 eOxMP_MEDIATYPE_MEDIAPLAY		=	4
	 */
}


bool cApkMediaPlay::GetNeedRadioBg(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HBOOL bNeed;

	hErr = APK_MEDIA_PLAY_GetNeedRadioBg ((HUINT32)viewId, &bNeed);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (bool)bNeed;
}


uint32_t cApkMediaPlay::GetSessionId(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulSessionId;

	hErr = APK_MEDIA_PLAY_GetSessionId ((HUINT32)viewId, &ulSessionId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulSessionId;
}


uint32_t cApkMediaPlay::GetRequestId(const uint32_t& viewId, const uint32_t& playType)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulRequestId;

	hErr = APK_MEDIA_PLAY_GetRequestId ((HUINT32)viewId, (OxMediaPlay_MediaType_e)playType, &ulRequestId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ulRequestId;
}


uint32_t cApkMediaPlay::GetStreamAspectRatio(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	DxAspectRatio_e eAspectRatio;

	hErr = APK_MEDIA_PLAY_GetStreamAspectRatio ((HUINT32)viewId, &eAspectRatio);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)eAspectRatio;

	/*
		eDxASPECTRATIO_UNKNOWN	=	0
		eDxASPECTRATIO_4X3		=	1
		eDxASPECTRATIO_16X9		=	2
		eDxASPECTRATIO_14X9		=	3
		eDxASPECTRATIO_1X1		=	4
		eDxASPECTRATIO_MAX_NUM	=	5
	*/
}


::DBus::Struct< int32_t, int32_t > cApkMediaPlay::GetMhegDisplayPoint(const uint32_t& viewId, const int32_t& refWidth, const int32_t& refHeight, const int32_t& videoPointX, const int32_t& videoPointY)
{
	::DBus::Struct< int32_t, int32_t > reval;

	HERROR hErr = ERR_FAIL;
	HINT32 scaledX = 0;
	HINT32 scaledY = 0;

	hErr = APK_MEDIA_PLAY_GetOsdDisplayPoint (	(HUINT32)viewId,
												(HINT32)refWidth, (HINT32)refHeight,
												(HINT32)videoPointX, (HINT32)videoPointY,
												(HINT32*)&scaledX, (HINT32*)&scaledY);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	{
		reval._1 = scaledX;
		reval._2 = scaledY;
	}

	return reval;
}


void cApkMediaPlay::StartAudioClip(const uint32_t& requestId, const std::vector< uint32_t >& buffer, const uint32_t& bufferBytes, const uint32_t& audioCodec, const uint32_t& loopCount)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_StartAudioClip (	(HUINT32)requestId,
											(HUINT8*)&buffer[0],
											(HUINT32)bufferBytes,
											(DxAudioCodec_e)audioCodec,
											(HUINT32)loopCount);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	/*
		eDxAUDIO_CODEC_UNKNOWN			=	0
		eDxAUDIO_CODEC_MPEG				=	1
		eDxAUDIO_CODEC_MP3				=	2
		eDxAUDIO_CODEC_DOLBY_AC3		=	3
		eDxAUDIO_CODEC_DOLBY_AC3P		=	4
		eDxAUDIO_CODEC_PCM				=	5
		eDxAUDIO_CODEC_AAC				=	6
		eDxAUDIO_CODEC_AAC_LOAS			=	7
		eDxAUDIO_CODEC_AAC_PLUS			=	8
		eDxAUDIO_CODEC_AAC_PLUS_ADTS	=	9
		eDxAUDIO_CODEC_DTS				=	10
		eDxAUDIO_CODEC_DTS_HD			=	11
		eDxAUDIO_CODEC_LPCM_DVD			=	12
		eDxAUDIO_CODEC_LPCM_HD_DVD		=	13
		eDxAUDIO_CODEC_LPCM_BLUERAY		=	14
		eDxAUDIO_CODEC_WMA_STD			=	15
		eDxAUDIO_CODEC_WMA_PRO			=	16
		eDxAUDIO_CODEC_PCMWAVE			=	17
		eDxAUDIO_CODEC_DRA				=	18
		eDxAUDIO_CODEC_AIFF				=	19
		eDxAUDIO_CODEC_RIFF				=	20
	*/

	return ;
}


void cApkMediaPlay::PauseAudioClip(const uint32_t& requestId)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_PauseAudioClip ((HUINT32)requestId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}


void cApkMediaPlay::ResumeAudioClip(const uint32_t& requestId)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_ResumeAudioClip ((HUINT32)requestId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}


void cApkMediaPlay::StopAudioClip(const uint32_t& requestId)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_StopAudioClip ((HUINT32)requestId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}


void cApkMediaPlay::SetVideoFreeze(const uint32_t& viewId, const bool& freeze)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_SetVideoFreeze ((HUINT32)viewId, (HBOOL)freeze);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}


void cApkMediaPlay::SetAudioFreeze(const uint32_t& viewId, const bool& freeze)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetAudioFreeze ((HUINT32)viewId, (HBOOL)freeze);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}


void cApkMediaPlay::SetComponentIndex(const uint32_t& viewId, const uint32_t& compType, const int32_t& compIndex)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetComponentIndex ((HUINT32)viewId, (OxMediaPlay_Component_e)compType, (HINT32)compIndex);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	/*
		eOxMP_COMPONENT_NONE			=	0
		eOxMP_COMPONENT_VIDEO			=	1
		eOxMP_COMPONENT_AUDIO			=	2
		eOxMP_COMPONENT_SUBTITLE		=	3
		eOxMP_COMPONENT_TELETEXT		=	4
		eOxMP_COMPONENT_CLOSEDCAPTION	=	5
		eOxMP_COMPONENT_DATA			=	6
	*/

	return ;
}

int32_t cApkMediaPlay::GetComponentNum(const uint32_t& viewId, const uint32_t& compType)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulCompNum;

	hErr = APK_MEDIA_PLAY_GetComponentNum ((HUINT32)viewId, (OxMediaPlay_Component_e)compType, &ulCompNum);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (int32_t )ulCompNum;
}

::DBus::Struct< int32_t, int32_t, uint8_t, int32_t, int32_t, int32_t, int32_t, std::string > cApkMediaPlay::GetSubtitleComponent(const uint32_t& viewId, const int32_t& compIndex)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_Component_t stComponet;
	::DBus::Struct< int32_t, int32_t, uint8_t, int32_t, int32_t, int32_t, int32_t, std::string > reval;

	hErr = APK_MEDIA_PLAY_GetComponent ((HUINT32)viewId, eOxMP_COMPONENT_SUBTITLE, (HINT32)compIndex, &stComponet);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	reval._1 = stComponet.stSubtitle.nPid;
	reval._2 = stComponet.stSubtitle.nComponentTag;
	reval._3 = stComponet.stSubtitle.eSubtitleType;

	switch (stComponet.stSubtitle.eSubtitleType)
	{
		case eOxMP_SUBTITLETYPE_Dvb:
		{
			reval._4 = stComponet.stSubtitle.utArg.stDvbInfo.nTypeValue;
			reval._5 = stComponet.stSubtitle.utArg.stDvbInfo.nComponentTag;
			reval._6 = stComponet.stSubtitle.utArg.stDvbInfo.nCompositionPageId;
			reval._7 = stComponet.stSubtitle.utArg.stDvbInfo.nAncillaryPageId;
			reval._8 = stComponet.stSubtitle.utArg.stDvbInfo.szLangCode;

			break;
		}
		case eOxMP_SUBTITLETYPE_Ebu:
		{
			reval._4 = stComponet.stSubtitle.utArg.stEbuInfo.nTypeValue;
			reval._5 = stComponet.stSubtitle.utArg.stEbuInfo.nComponentTag;
			reval._6 = stComponet.stSubtitle.utArg.stEbuInfo.nMagazineNum;
			reval._7 = stComponet.stSubtitle.utArg.stEbuInfo.nPageNum;
			reval._8 = stComponet.stSubtitle.utArg.stEbuInfo.szLangCode;

			break;
		}
		case eOxMP_SUBTITLETYPE_Media:
		{
			reval._4 = 0;
			reval._5 = 0;
			reval._6 = 0;
			reval._7 = 0;
			reval._8 = stComponet.stSubtitle.utArg.stMediaInfo.szLangCode;
			break;
		}
		default :
			break;
	}

	return reval;
}

int32_t cApkMediaPlay::GetComponentIndex(const uint32_t& viewId, const uint32_t& compType)
{
	HERROR hErr = ERR_FAIL;
	HINT32 nCompIndex;
	HINT32 nCompSubIndex;

	hErr = APK_MEDIA_PLAY_GetComponentIndex ((HUINT32)viewId, (OxMediaPlay_Component_e)compType, &nCompIndex, &nCompSubIndex);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (int32_t)nCompIndex;
}

uint32_t cApkMediaPlay::StartLive(const uint32_t& viewId, const ::DBus::Struct< int32_t, int32_t, int32_t, int32_t, int32_t >& startInfo)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_StartInfo_t unStart;
	HUINT32 ulSessionId;

	unStart.stLive.eType			=	eOxMP_MEDIATYPE_LIVE;
	unStart.stLive.eLiveType		=	(OxMediaPlay_LiveType_e)startInfo._1;
	unStart.stLive.ulMasterSvcUid	=	startInfo._2;
	unStart.stLive.ulSuppleSvcUid	=	startInfo._3;
	unStart.stLive.eSuppSvcType		=	(OxMediaPlay_SuppleSvcType_e)startInfo._4;
	unStart.stLive.ulMajorCHNum		=	startInfo._5;

	hErr = APK_MEDIA_PLAY_Start ((HUINT32)viewId, &unStart, &ulSessionId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ulSessionId;
}

uint32_t cApkMediaPlay::StartPvrPb(const uint32_t& viewId, const ::DBus::Struct< int32_t, int32_t >& startInfo)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_StartInfo_t unStart;
	HUINT32 ulSessionId;

	unStart.stPvrPb.eType			=	eOxMP_MEDIATYPE_PVRPLAYBACK;
	unStart.stPvrPb.ulContentId		=	startInfo._1;
	unStart.stPvrPb.ulStartTime		=	startInfo._2;

	hErr = APK_MEDIA_PLAY_Start ((HUINT32)viewId, &unStart, &ulSessionId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ulSessionId;
}

uint32_t cApkMediaPlay::StartTsrPb(const uint32_t& viewId, const ::DBus::Struct< int32_t, int32_t, int32_t, int32_t >& startInfo)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_StartInfo_t unStart;
	HUINT32 ulSessionId;

	unStart.stTsrPb.eType			=	eOxMP_MEDIATYPE_TSRPLAYBACK;
	unStart.stTsrPb.ulMasterSvcUid	=	startInfo._1;
	unStart.stTsrPb.ulSuppleSvcUid	=	startInfo._2;
	unStart.stTsrPb.eSuppSvcType	=	(OxMediaPlay_SuppleSvcType_e)startInfo._4;
	unStart.stTsrPb.ulStartTime		=	startInfo._4;

	hErr = APK_MEDIA_PLAY_Start ((HUINT32)viewId, &unStart, &ulSessionId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ulSessionId;
}

uint32_t cApkMediaPlay::StartMedia(const uint32_t& viewId, const ::DBus::Struct< int32_t, int32_t, int32_t, int32_t, int32_t, std::string >& startInfo)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_StartInfo_t unStart;
	HUINT32 ulSessionId;

	unStart.stMediaPlay.eType			=	eOxMP_MEDIATYPE_MEDIAPLAY;
	unStart.stMediaPlay.eVideoCodec		=	(DxVideoCodec_e)startInfo._1;
	unStart.stMediaPlay.eAudioCodec		=	(DxAudioCodec_e)startInfo._2;
	unStart.stMediaPlay.ulVideoPid		=	startInfo._3;
	unStart.stMediaPlay.ulAudioPid		=	startInfo._4;
	unStart.stMediaPlay.ulStartTime		=	startInfo._5;

	strcpy(unStart.stMediaPlay.szUrl, &(startInfo._6)[0]);

	hErr = APK_MEDIA_PLAY_Start ((HUINT32)viewId, &unStart, &ulSessionId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ulSessionId;
}

void cApkMediaPlay::Stop(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_Stop ((HUINT32)viewId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::SetPlaySpeed(const uint32_t& viewId, const int32_t& speed)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetPlaySpeed ((HUINT32)viewId, (HINT32)speed);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}


void cApkMediaPlay::SetPlayPosition(const uint32_t& viewId, const uint32_t& position)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetPlayPosition ((HUINT32)viewId, (HUINT32)position);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}


int32_t cApkMediaPlay::GetPlaySpeed(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HINT32 nSpeed;

	hErr = APK_MEDIA_PLAY_GetPlaySpeed ((HUINT32)viewId, &nSpeed);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (int32_t)nSpeed;
}

uint32_t cApkMediaPlay::GetPlayPosition(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulPosition;

	hErr = APK_MEDIA_PLAY_GetPlayPosition ((HUINT32)viewId, &ulPosition);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulPosition;
}

uint32_t cApkMediaPlay::GetPlayState(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_PlayState_e eState;

	hErr = APK_MEDIA_PLAY_GetPlayState ((HUINT32)viewId, &eState);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)eState;
}

uint32_t cApkMediaPlay::GetPlayError(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_PlayError_e eError;
	OxMediaPlay_ErrorReason_e eReason;

	hErr = APK_MEDIA_PLAY_GetPlayError ((HUINT32)viewId, &eError, &eReason);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)eError;
}

uint32_t cApkMediaPlay::GetBufferedTime(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulBufferred;

	hErr = APK_MEDIA_PLAY_GetBufferedTime ((HUINT32)viewId, &ulBufferred);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulBufferred;
}

uint32_t cApkMediaPlay::GetDurationTime(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulDurationTime;

	hErr = APK_MEDIA_PLAY_GetDurationTime ((HUINT32)viewId, &ulDurationTime);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulDurationTime;
}

uint32_t cApkMediaPlay::GetTsrStartTime(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulTsrStartTime;

	hErr = APK_MEDIA_PLAY_GetTsrStartTime ((HUINT32)viewId, &ulTsrStartTime);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulTsrStartTime;
}

std::vector< int32_t > cApkMediaPlay::GetSupportedSpeeds(const uint32_t& viewId, const uint32_t& maxNumSpeed)
{
	std::vector< int32_t > reval;

	HERROR hErr = ERR_FAIL;
	HINT32 i, numSpeed=0;

	HINT32 * pSpeedList = (HINT32 *)HLIB_MEM_Malloc (maxNumSpeed * sizeof(HINT32));
	if (NULL == pSpeedList)
		return reval;

	hErr = APK_MEDIA_PLAY_GetSupportedSpeeds ((HUINT32)viewId, (HUINT32 *)&numSpeed, (HUINT32)maxNumSpeed, (HINT32*)pSpeedList);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	for (i=0 ; i<numSpeed ; i++)
	{
		reval.push_back(pSpeedList[i]);
	}

	if (NULL != pSpeedList)
	{
		HLIB_MEM_Free (pSpeedList);
	}

	return reval;
}

uint32_t cApkMediaPlay::GetTrickRestrictMode(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulTrickRestrictMode;

	hErr = APK_MEDIA_PLAY_GetTrickRestrictMode ((HUINT32)viewId, &ulTrickRestrictMode);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulTrickRestrictMode;
}

::DBus::Struct< int32_t, int32_t, int32_t, int32_t > cApkMediaPlay::GetVideoSize(const uint32_t& viewId)
{
	::DBus::Struct< int32_t, int32_t, int32_t, int32_t > reval;

	HERROR hErr = ERR_FAIL;
	HINT32 startX	=	0;
	HINT32 startY	=	0;
	HINT32 width	=	0;
	HINT32 height	=	0;

	hErr = APK_MEDIA_PLAY_GetVideoSize ((HUINT32)viewId, (HINT32*)&startX, (HINT32*)&startY, (HINT32*)&width, (HINT32*)&height);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	{
		reval._1 =	startX;
		reval._2 =	startY;
		reval._3 =	width;
		reval._4 =	height;
	}

	return reval;
}

void cApkMediaPlay::SetVideoSize(const uint32_t& viewId, const int32_t& startX, const int32_t& startY, const int32_t& width, const int32_t& height)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetVideoSize ((HUINT32)viewId, (HINT32)startX, (HINT32)startY, (HINT32)width, (HINT32)height);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::ResetVideoSize(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_ResetVideoSize ((HUINT32)viewId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::SetPigRect(const uint32_t& viewId, const uint32_t& zOrder, const int32_t& isX, const int32_t& isY, const int32_t& iW, const int32_t& iH, const int32_t& irW, const int32_t& irH, const int32_t& osX, const int32_t& osY, const int32_t& oW, const int32_t& oH, const int32_t& orW, const int32_t& orH, const uint32_t& osdAspectRatio, const uint32_t& dfc, const uint32_t& pigAspectRatio)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetPigRect (	(HUINT32)viewId, (HUINT32)zOrder,
										(HINT32)isX,(HINT32)isY, (HINT32)iW, (HINT32)iH, (HINT32)irW, (HINT32)irH,
										(HINT32)osX, (HINT32)osY, (HINT32)oW, (HINT32)oH, (HINT32)orW, (HINT32)orH,
										(DxAspectRatio_e)osdAspectRatio, (DxDfcSelect_e)dfc, (DxAspectRatio_e)pigAspectRatio );
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::SetTSREnable(const bool& enable)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetTSREnable ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::SetSubtitleEnable(const uint32_t& viewId, const bool& enable)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetSubtitleEnable ((HUINT32)viewId, (HBOOL)enable);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::SetVideoHide(const uint32_t& viewId, const bool& hide)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetVideoHide ((HUINT32)viewId, (HBOOL)hide);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

bool cApkMediaPlay::GetTSREnable()
{
	HERROR hErr = ERR_FAIL;
	HBOOL bEnable;

	hErr = APK_MEDIA_PLAY_GetTSREnable (&bEnable);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (bool)bEnable;
}

bool cApkMediaPlay::GetSubtitleEnable(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HBOOL bEnable;

	hErr = APK_MEDIA_PLAY_GetSubtitleEnable ((HUINT32)viewId, &bEnable);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (bool)bEnable;
}

std::vector< uint8_t > cApkMediaPlay::GetEventInfo(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	DxEvent_t stEventInfo;
	std::vector< uint8_t > reval;

	hErr = APK_MEDIA_PLAY_GetEventInfo ((HUINT32)viewId, &stEventInfo);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return reval;
}

void cApkMediaPlay::SaveThumbnail(const uint32_t& viewId, const std::string& filename, const uint32_t& width, const uint32_t& height)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SaveThumbnail ((HUINT32)viewId, (HCHAR*)&filename[0], (HUINT32)width, (HUINT32)height);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

uint32_t cApkMediaPlay::GetMajorChannel(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulMajorCHNum;

	hErr = APK_MEDIA_PLAY_GetMajorChannel ((HUINT32)viewId, &ulMajorCHNum);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulMajorCHNum;
}

bool cApkMediaPlay::CheckPlayChangable(const uint32_t& viewId, const uint32_t& playType, const std::vector< uint8_t >& startInfo)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_StartInfo_t unStart;
	HBOOL bChangable;

	hErr = APK_MEDIA_PLAY_CheckPlayChangable ((HUINT32)viewId, (OxMediaPlay_MediaType_e)playType, &unStart, &bChangable);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (bool)bChangable;
}

uint32_t cApkMediaPlay::GetViewState(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_LiveViewState_e eViewState;

	hErr = APK_MEDIA_PLAY_GetViewState ((HUINT32)viewId, &eViewState);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)eViewState;
}

uint32_t cApkMediaPlay::GetLockState(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	OxMediaPlay_LiveLockState_e eLockState;

	hErr = APK_MEDIA_PLAY_GetLockState ((HUINT32)viewId, &eLockState);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)eLockState;
}

void cApkMediaPlay::MemorizeMediaState(const uint32_t& viewId, const bool& stopMedia)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_MemorizeMediaState ((HUINT32)viewId, (HBOOL)stopMedia);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::RestoreMediaState(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_RestoreMediaState ((HUINT32)viewId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

uint32_t cApkMediaPlay::GetThumbnail(const uint32_t& viewId, const uint32_t& pos)
{
	HCHAR * pReval;
	HUINT32 ulCaptureTime;

	pReval = APK_MEDIA_PLAY_GetThumbnail ((HUINT32)viewId, (HUINT32)pos, &ulCaptureTime);
	if (NULL == pReval)
	{
		/* error */
	}

	return (uint32_t)ulCaptureTime;
}

std::string cApkMediaPlay::GetURI(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_MEDIA_PLAY_GetURI ((HUINT32)viewId, szBuff);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return std::string(szBuff);
}

void cApkMediaPlay::Probe(const uint32_t& viewId, const std::string& uri)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_Probe ((HUINT32)viewId, (HCHAR *)&uri[0]);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::SetSubtitleSyncTime(const uint32_t& viewId, const uint32_t& subtitleSyncTime)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_SetSubtitleSyncTime ((HUINT32)viewId, (HUINT32)subtitleSyncTime);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::SetSubtitleFontSize(const uint32_t& viewId, const uint32_t& subtitleFontSize)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_SetSubtitleFontSize ((HUINT32)viewId, (HUINT32)subtitleFontSize);
	if (ERR_OK != hErr)
	{
		/* error */
	}


}

void cApkMediaPlay::SetSubtitleTextPosition(const uint32_t& viewId, const uint32_t& subtitleTextPosition)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_SetSubtitleTextPosition ((HUINT32)viewId, (HUINT32)subtitleTextPosition);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

uint32_t cApkMediaPlay::GetSubtitleSyncTime(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	 HUINT32 nSubtitle_SyncTime = 0;

	hErr = APK_MEDIA_PLAY_GetSubtitleSyncTime ((HUINT32)viewId, &nSubtitle_SyncTime);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)nSubtitle_SyncTime;
}

uint32_t cApkMediaPlay::GetSubtitleFontSize(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	HUINT32 nSubtitle_FontSize = 0;

	hErr = APK_MEDIA_PLAY_GetSubtitleFontSize ((HUINT32)viewId, &nSubtitle_FontSize);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)nSubtitle_FontSize;
}

uint32_t cApkMediaPlay::GetSubtitleTextPosition(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	HUINT32 nSubtitle_TextPosition = 0;

	hErr = APK_MEDIA_PLAY_GetSubtitleTextPosition ((HUINT32)viewId, &nSubtitle_TextPosition);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)nSubtitle_TextPosition;
}

::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t > cApkMediaPlay::GetSubtitleStatus(const uint32_t& viewId)
{
	::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t > reval;

	HERROR hErr = ERR_FAIL;

	OxMediaPlay_SubtitleStatus_t stStatus;
	HxSTD_memset (&stStatus, 0, sizeof(OxMediaPlay_SubtitleStatus_t));

	hErr = APK_MEDIA_PLAY_GetSubtitleStatus ((HUINT32)viewId, &stStatus);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	{
		reval._1	=	(uint32_t)stStatus.eState;

		reval._2	=	(uint32_t)stStatus.ulSharedKey;
		reval._3	=	(uint32_t)stStatus.ulSharedSize;
		reval._4	=	(uint32_t)stStatus.ulLastRetrievingOffset;
		reval._5	=	(uint32_t)stStatus.ulLastPacketSize;
	}

	return reval;
}

void cApkMediaPlay::SetBufferingScenario(const uint32_t& viewId, const bool& isAuto, const int32_t& threshold)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_SeBufferingScenario ((HUINT32)viewId, (HBOOL)isAuto, (HINT32)threshold);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

::DBus::Struct< uint32_t, std::vector< uint32_t >, std::vector< uint32_t > > cApkMediaPlay::GetBufferingChunkInfo(const uint32_t& viewId)
{
	::DBus::Struct< uint32_t, std::vector< uint32_t >, std::vector< uint32_t > > reval;

	HERROR hErr = ERR_FAIL;

	HUINT32 nChunks = 0;
	HUINT32 *pulStartOffsets;
	HUINT32 *pulEndOffsets;

	hErr = APK_MEDIA_PLAY_GetBufferingChunkInfo ((HUINT32)viewId, &nChunks, &pulStartOffsets, &pulEndOffsets);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	{
		HINT32 i;
		for (i=0 ; i<OxMP_MEDIAPB_CHUNKS_MAX ; i++)
		{
			reval._2.push_back((uint32_t)pulStartOffsets[i]);
			reval._3.push_back((uint32_t)pulEndOffsets[i]);
		}

		reval._1 = (uint32_t)nChunks;
	}

	return reval;
}

uint32_t cApkMediaPlay::GetBufferingRemainedTime(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulBufferRemaindTime = 0;

	hErr = APK_MEDIA_PLAY_GetBufferingRemainedTime ((HUINT32)viewId, &ulBufferRemaindTime);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulBufferRemaindTime;
}

uint32_t cApkMediaPlay::GetBufferingPercent(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulBufferPercent = 0;

	hErr = APK_MEDIA_PLAY_GetBufferingPercent ((HUINT32)viewId, &ulBufferPercent);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulBufferPercent;
}

uint32_t cApkMediaPlay::GetBufferState(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	OxMediaPlay_BufferState_e eBufferState = eOxMP_BUFFERSTATE_None;

	hErr = APK_MEDIA_PLAY_GetBufferState ((HUINT32)viewId, &eBufferState);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)eBufferState;
}

void cApkMediaPlay::SetDrmLicenseData(const uint32_t& viewId, const std::vector< uint8_t >& aData)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_SetDrmLicenseData ((HUINT32)viewId, (const HCHAR *)&aData[0]);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::SetBrowserUserAgentData(const uint32_t& viewId, const std::vector< ::DBus::Variant >& aUAData)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_SetBrowserUserAgentData ((HUINT32)viewId, (HCHAR *)&aUAData[0]);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

::DBus::Struct< std::vector< uint32_t >, std::vector< uint32_t >, uint32_t, uint32_t > cApkMediaPlay::GetResourceMap()
{
	::DBus::Struct< std::vector< uint32_t >, std::vector< uint32_t >, uint32_t, uint32_t > reval;

	HERROR hErr = ERR_FAIL;

	OxMediaPlay_ResourceMap_t stResourceMap;
	HxSTD_memset (&stResourceMap, 0, sizeof(OxMediaPlay_ResourceMap_t));

	hErr = APK_MEDIA_PLAY_GetResourceMap (&stResourceMap);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	{
		HINT32 i;
		for (i=0 ; i<stResourceMap.ulNumOfPlaypump ; i++)
			reval._1.push_back((uint32_t)stResourceMap.pulPlaypumpList[i]);

		for (i=0 ; i<stResourceMap.ulNumOfDma ; i++)
			reval._2.push_back((uint32_t)stResourceMap.pulDmaList[i]);

		reval._3	=	(uint32_t)stResourceMap.ulNumOfPlaypump;
		reval._4	=	(uint32_t)stResourceMap.ulNumOfDma;
	}

	return reval;
}

uint32_t cApkMediaPlay::GetPumpIndex(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulPumpId	=	0;

	hErr = APK_MEDIA_PLAY_GetPumpIndex ((HUINT32)viewId, &ulPumpId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (uint32_t)ulPumpId;
}

void cApkMediaPlay::SetDuration(const uint32_t& viewId, const int64_t& duration)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_SetDuration ((HUINT32)viewId, (HUINT64)duration);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::SetPlayerType(const uint32_t& viewId, const uint32_t& playerType)
{
	HERROR hErr = ERR_FAIL;

	hErr = APK_MEDIA_PLAY_SetPlayerType ((HUINT32)viewId, (OxMediaPlay_PlayerType_e)playerType);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::SetPlayReadyInfo(const uint32_t& viewId, const std::string& laURL, const std::vector< uint8_t >& customData, const bool& controlFlag)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_SetPlayReadyInfo ((HUINT32)viewId, (HCHAR *)&laURL[0], (HCHAR *)&customData[0], (HBOOL)controlFlag);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

std::vector< uint8_t > cApkMediaPlay::GetVmxIdentifier()
{
	std::vector< uint8_t > reval;

	HERROR hErr = ERR_FAIL;
	const HINT32 IDEN_COUNT	=	128; //oapi_media_play.h Âü°í
	HUINT8 *pIdentifier;


	hErr = APK_MEDIA_PLAY_GetVmxIdentifier (&pIdentifier);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	{
		HINT32 i;
		for (i=0 ; i<IDEN_COUNT ; i++)
		{
			reval.push_back((uint32_t)pIdentifier[i]);
		}
	}

	return reval;
}

int32_t cApkMediaPlay::GetVmxStatus(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nStatus = 0;

	hErr = APK_MEDIA_PLAY_GetVmxStatus ((HUINT32)viewId, &nStatus);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (int32_t)nStatus;
}

int32_t cApkMediaPlay::GetVmxError(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;

	HINT32 nErrorCode = 0;

	hErr = APK_MEDIA_PLAY_GetVmxError ((HUINT32)viewId, &nErrorCode);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return (int32_t)nErrorCode;
}

void cApkMediaPlay::SetVmxCas(const uint32_t& viewId, const std::string& VCASUrl, const std::string& vendorName)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_SetVmxCas ((HUINT32)viewId, (HCHAR *)&VCASUrl[0], (HCHAR *)&vendorName[0]);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}

void cApkMediaPlay::UnsetVmxCas(const uint32_t& viewId)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_MEDIA_PLAY_UnsetVmxCas ((HUINT32)viewId);
	if (ERR_OK != hErr)
	{
		/* error */
	}

	return ;
}
