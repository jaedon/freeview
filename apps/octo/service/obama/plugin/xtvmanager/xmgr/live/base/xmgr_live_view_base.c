/**
	@file     xmgr_live_view_base.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/**
  * Character encoding.
  *
  * MS949
  *
  * This source file that uses MS949 encoding.
  * MS949 encoding 을 사용하는 source file 입니다.
  * MS949 encodingを使用して source fileです。
  * Quelldatei, die MS949-Codierung verwendet.
  *
**/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <vkernel.h>

#include <db_param.h>
#include <db_svc.h>

#include <svc_ch.h>
#include <svc_si.h>
#include <xsvc_si.h>
#include <svc_av.h>
#include <svc_sys.h>
#include <svc_cas.h>
#include <svc_sbtl.h>
#include <svc_ttx.h>
#include <svc_transcoder.h>
#if defined(CONFIG_MW_LOGCAPTURE)
#include <svc_logcapture.h>
#endif

//#include <si_util.h>
#include <get_enum_str.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_pg.h>
#include <mgr_output.h>
#include <mgr_svclist.h>
#include <mgr_rscmap.h>
#include <xmgr_cas.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <xmgr_spec.h>
#include <xmgr_zapping.h>
#include <xmgr_live.h>
#include <mgr_copyrightcontrol.h>
#include <xmgr_module_search.h>
#include <mgr_3rdparty.h>

// Local/Member Header Files
#include "../local_include/_xmgr_live.h"
#include "../local_include/_xmgr_live_view.h"
#include "../local_include/_xmgr_live_comp.h"

#include <otl.h>

#define ___LOCAL_DEFINITIONS___
#define ___LOCAL_DATATYPES___
#define ___LOCAL_VARIABLES___

#if defined(CONFIG_MW_CAS_IRDETO) && (CONFIG_MW_CAS_NUM_EMBEDDED == 1)
#define NO_SIGNAL_TIMER_ID				400
#define NO_SIGNAL_TIMEOUT				(30*1000)		//30sec
#endif

#define	XmgrLive_SUBTITLE_EBU		eSI_SUBTITLE_EBU
#define	XmgrLive_SUBTITLE_DVB		eSI_SUBTITLE_DVB

#define	XmgrLive_SubtitleInfo			SvcSi_SubtitleInfo_t

#define XmgrLive_SUBTITLE_INFO_Clear(pstSubtitleInfo)		HxSTD_MemSet(pstSubtitleInfo, 0xff, sizeof(XmgrLive_SubtitleInfo));
#define	XmgrLive_SUBTITLE_INFO_Comp(pstInfo1, pstInfo2)		HxSTD_MemCmp(pstInfo1, pstInfo2, sizeof(XmgrLive_SubtitleInfo))

#define _SECTION_INSERTION_PERIOD	100		//org:300ms
//#define _DUMP_SECTION_
#define CONFIG_TRANSCODER_FIXED_PCRPID	0x1FFE

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC XmgrLiveView_Context_t	s_astXmgrLiveView_Context[eActionId_MAX];



#define ___LOCAL_FUNCTION_PROTOTYPE___
INLINE STATIC XmgrLiveView_Context_t *_mgr_LiveView_base_getContext (HUINT32 ulActionId);
STATIC HERROR _mgr_LiveView_base_initContext (void);
//INLINE STATIC HERROR _mgr_LiveView_clearContext (XmgrLiveView_Context_t *pstContext);


STATIC HERROR _mgr_LiveView_base_StartMasterSession(XmgrLiveView_Context_t *pstContext, Handle_t hSvc, ChListUiGroup_t svcListGroup, MgrLive_TuningMode_t tuningMode);
STATIC HERROR _mgr_LiveView_base_StartSuppleSession(XmgrLiveView_Context_t *pstContext, Handle_t hSuppleSvc, MgrLiveView_SessionType_t sessionType, MgrLive_TuningMode_t tuningMode);
STATIC HERROR _mgr_LiveView_base_StartMwServices (XmgrLiveView_Context_t *pstContext, HBOOL bNeedChTune);
STATIC HERROR _mgr_LiveView_base_StopMwServices (XmgrLiveView_Context_t *pstContext, HBOOL bForced, ApiSyncMode_t eSync);
STATIC HERROR _mgr_LiveView_base_StopMwService(XmgrLiveView_Context_t *pstContext, XmgrLive_MwSvcType_e eMwSvcType, ApiSyncMode_t eSync);

STATIC void _mgr_LiveView_base_ChangePid_SvcDb(XmgrLiveView_Context_t *pstContext);
#if defined(CONFIG_MW_SUBTITLE)
STATIC void _mgr_LiveView_base_StartSubtitle(XmgrLiveView_Context_t *pstContext);
#endif

STATIC void _mgr_LiveView_base_ShowProgram(XmgrLiveView_Context_t *pstContext);
STATIC void _mgr_LiveView_base_StartComplementaries(XmgrLiveView_Context_t *pstContext);
STATIC void _mgr_LiveView_base_SetCopyProtection(XmgrLiveView_Context_t *pstContext);
STATIC void _mgr_LiveView_base_SetAppState(XmgrLiveView_Context_t *pstContext, XmgrLiveView_State_e eAppState);
STATIC void _mgr_LiveView_base_SetUiState(XmgrLiveView_Context_t *pstContext, MgrLiveView_UiState_e eUiState);
STATIC void _mgr_LiveView_base_SetFrontPanel(XmgrLiveView_Context_t *pstContext);
STATIC void _mgr_LiveView_base_BackUpCurrentService(XmgrLiveView_Context_t *pstContext);
STATIC void _mgr_LiveView_base_CheckRadioBackground(XmgrLiveView_Context_t *pstContext);
STATIC void _mgr_LiveView_base_CheckAV(XmgrLiveView_Context_t *pstContext);
STATIC void _mgr_LiveView_base_CheckDolbyLed(XmgrLiveView_Context_t *pstContext);
STATIC Handle_t _mgr_LiveView_base_GetMasterSvcHandle(XmgrLiveView_Context_t *pstContext);
STATIC DbSvc_Info_t *_mgr_LiveView_base_GetMasterSvcInfoPtr(XmgrLiveView_Context_t *pstContext);
STATIC HBOOL _mgr_LiveView_base_CheckComplementariesStartCondition(XmgrLiveView_Context_t *pstContext);

#if (CONFIG_MW_SYS_7SEG)
STATIC void	_mgr_LiveView_base_DisplayPanelSetAsCurrentChNumber(XmgrLiveView_Context_t *pstContext);
#endif // #if (CONFIG_MW_SYS_7SEG)

STATIC HINT8* _mgr_LiveView_base_GetUiStateStr(MgrLiveView_UiState_e eUiState);
STATIC HINT8* _mgr_LiveView_base_GetAppStateStr(XmgrLiveView_State_e eAppState);
STATIC HUINT8 *_mgr_LiveView_base_GetSessionTypeStr(MgrLiveView_SessionType_t sessionType);
STATIC HUINT8 *_mgr_LiveView_base_GetMwSvcTypeStr(XmgrLive_MwSvcType_e eMwSvcType);
//STATIC HUINT8 *_mgr_LiveView_base_GetMwSvcStateStr(XmgrLive_MwSvcState_e eServicesState);
//STATIC HUINT8 *_mgr_LiveView_base_GetMwSvcExecPhaseStr(XmgrLive_MwSvcExecPhase_e eServicesStartPhase);

STATIC void		_mgr_LiveView_base_EvtDbCheckSvcDbChgFrontPanel(XmgrLiveView_Context_t *pContext, DbSvc_Info_t  *pstSvcInfo);
STATIC HERROR	_mgr_LiveView_base_EvtDbCheckSvcDbByDeleteSvc(HUINT32 ulActionId, ChListUiGroup_t svcListGroup, DxDeliveryType_e eOrgDeliType, Handle_t hNextSvcHandle);


STATIC HUINT16 _mgr_LiveView_base_GetVideoPid(XmgrLiveView_Context_t *pstContext, HUINT16 usCurVideoPid);
STATIC HUINT16 _mgr_LiveView_base_GetAudioPid(XmgrLiveView_Context_t *pstContext, HUINT16 usCurAudioPid);

STATIC HERROR 	_mgr_LiveView_base_SetLedOnOff(HUINT32 ulActionId, DxLedId_e eLedId, HBOOL bOnOff);


#define _____CONTEXT_FUNCTIONS_____
INLINE STATIC XmgrLiveView_Context_t *_mgr_LiveView_base_getContext (HUINT32 ulActionId)
{
	if (((ulActionId >= eActionId_VIEW_FIRST) && (ulActionId <= eActionId_VIEW_LAST)) ||
		((ulActionId >= eActionId_BG_FIRST) && (ulActionId <= eActionId_BG_LAST)))
	{
		return s_astXmgrLiveView_Context + ulActionId;
	}

	return NULL;
}

STATIC HERROR _mgr_LiveView_base_initContext (void)
{
	HUINT32					 ulActionId;

	HxSTD_MemSet (s_astXmgrLiveView_Context, 0, sizeof(XmgrLiveView_Context_t) * eActionId_MAX);

	for (ulActionId = 0; ulActionId < eActionId_MAX; ulActionId++)
	{
		XmgrLiveView_Context_t			*pstContext = s_astXmgrLiveView_Context + ulActionId;

		pstContext->ulActionId = ulActionId;
		pstContext->eUiState = eLiveViewUi_NONE;
	}

	return ERR_OK;
}

STATIC HERROR _mgr_LiveView_base_resetUiState(HUINT32 ulActionId, HBOOL bSvcStart)
{
	HERROR							hError = ERR_FAIL;
	XmgrLiveView_Context_t			*pstContext = NULL;

	if (ulActionId < eActionId_MAX)
	{
		pstContext = &s_astXmgrLiveView_Context[ulActionId];

		if (bSvcStart != FALSE)
		{
			// Start
			pstContext->eUiState = eLiveViewUi_OK;
		}
		else
		{
			// Stop
			pstContext->eUiState = eLiveViewUi_NONE;
		}

		hError = ERR_OK;
	}

	return hError;
}

void _mgr_LiveView_base_ResetPidRunningStatus(XmgrLiveView_Context_t *pstContext)
{
/* CAS 관련 status 초기화 */
#ifdef CONFIG_MW_CAS
	pstContext->bCasOk = FALSE;
#else
	/* Embeded CAS 없는 제품은 항상 TRUE */
	pstContext->bCasOk = TRUE;
#endif
	pstContext->pCasInfo = NULL;
	pstContext->nCasEvt = 0;
	pstContext->nCasShow = eCasShowType_Both;
	pstContext->bCasDrmValid = FALSE;

/* AV 관련 status 초기화 */
	pstContext->bAvUnderrun = FALSE;
	pstContext->bHdmiBlocked = FALSE;

/* PID 관련 데이터 초기화
	- Temp PID를 위한 original PID backup.
	- 0은 original PID가 backup되지 않았다는 의미이므로 PID_NULL로 하지 말것.
	- PID_NULL은 valid한 backup 값이다.*/
	pstContext->usOrgVideoPid = 0;
	pstContext->eOrgVideoCodec = eDxVIDEO_CODEC_MAX_NUM;
	pstContext->usOrgPcrPid = 0;
	pstContext->usOrgAudioPid = 0;
	pstContext->usOrgDolbyPid = 0;
	pstContext->eOrgAudioCodec = eDxAUDIO_CODEC_MAX_NUM;
	pstContext->eOrgDolbyCodec = eDxAUDIO_CODEC_MAX_NUM;
}

INLINE STATIC HERROR _mgr_LiveView_base_ResetCommonContextFlags (XmgrLiveView_Context_t *pstContext, Handle_t hNewSvc)
{
	HERROR		hErr = ERR_FAIL;

	/* subtitle 정보 초기화 */
	pstContext->bSubtitleAllowed = FALSE;
	pstContext->bTtxPageEnabled = FALSE;
	XmgrLive_SUBTITLE_INFO_Clear(&pstContext->subtitleInfo);

	/* MW service stop용 flag 초기화 */
	pstContext->bSiStopped = FALSE;
	pstContext->bCasStopped = FALSE;
	pstContext->bSubtitle_CC_Stopped = FALSE;
	pstContext->bTeletextStopped = TRUE;

	/* CAS용 정보 초기화 */
	pstContext->nCasEvt = 0;

	/* unselectComponent 정보 초기화 */
	pstContext->bUnselectVideo = FALSE;
	pstContext->bUnselectAudio = FALSE;

	_mgr_LiveView_base_ResetPidRunningStatus(pstContext);

	pstContext->hSvc = hNewSvc; /* 비교 루틴들이 있으므로 hSvc는 맨 마지막에 update */

	hErr = MGR_RSC_GetResourceInfo(pstContext->ulActionId, &pstContext->stRscInfo);
	if (hErr)
	{
		HxLOG_Error("MGR_RSC_GetResourceInfo ret :0x%X\n", hErr);
	}
	return ERR_OK;
}

#define _____LOCAL_FUNCTION_____
STATIC BUS_Result_t _mgr_LiveView_base_SendBusMessage(BUS_Callback_t appCallback, HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
#if defined(CONFIG_SUPPORT_FCC)
	HUINT32 	ulActionId = SVC_PIPE_GetActionId(hAction);

	if (ulActionId != MGR_ACTION_GetMainActionId())
	{
		// main action(main live)인 경우에만 msg를 날리자
		return ERR_BUS_NO_ERROR;
	}
#endif

	return BUS_SendMessage(appCallback, lMessage, hAction, lParam1, lParam2, lParam3);
}

STATIC HCHAR *_mgr_LiveView_base_ProfileCvtVideoCodecToAscii_Base(DxVideoCodec_e codec)
{
	switch (codec)
	{
	case eDxVIDEO_CODEC_H264:	return "h264";
/*
		ENUM_TO_STR(eDxVIDEO_CODEC_UNKNOWN);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG1);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG2);
		ENUM_TO_STR(eDxVIDEO_CODEC_MPEG4_PART2);
		ENUM_TO_STR(eDxVIDEO_CODEC_H263);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264_SVC);
		ENUM_TO_STR(eDxVIDEO_CODEC_H264_MVC);
		ENUM_TO_STR(eDxVIDEO_CODEC_VC1);
		ENUM_TO_STR(eDxVIDEO_CODEC_VC1_SIMPLEMAIN);
		ENUM_TO_STR(eDxVIDEO_CODEC_AVS);
		ENUM_TO_STR(eDxVIDEO_CODEC_RV40);
		ENUM_TO_STR(eDxVIDEO_CODEC_VP6);
		ENUM_TO_STR(eDxVIDEO_CODEC_DIVX311);
		ENUM_TO_STR(eDxVIDEO_CODEC_FromLIVE);
*/
	default:
		break;
	}
	return NULL;
}

STATIC DxVideoCodec_e _mgr_LiveView_base_ProfileCvtAsciiToVideoCodec_Base(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxVIDEO_CODEC_MAX_NUM;
	HCHAR		*text = NULL;

	for(i=0; i < max; i++)
	{
		text = _mgr_LiveView_base_ProfileCvtVideoCodecToAscii_Base((DxVideoCodec_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return (DxVideoCodec_e)i;
		}
	}

	return eDxVIDEO_CODEC_UNKNOWN;
}

STATIC HCHAR *_mgr_LiveView_base_ProfileCvtAudioCodecToAscii_Base(DxAudioCodec_e codec)
{
	switch (codec)
	{
	 case eDxAUDIO_CODEC_MP3: return "mp3";
	case eDxAUDIO_CODEC_AAC: return "aac";
/*
		ENUM_TO_STR(eDxAUDIO_CODEC_UNKNOWN);
		ENUM_TO_STR(eDxAUDIO_CODEC_MPEG);
		ENUM_TO_STR(eDxAUDIO_CODEC_MP3);
		ENUM_TO_STR(eDxAUDIO_CODEC_DOLBY_AC3);
		ENUM_TO_STR(eDxAUDIO_CODEC_DOLBY_AC3P);
		ENUM_TO_STR(eDxAUDIO_CODEC_PCM);

		ENUM_TO_STR(eDxAUDIO_CODEC_AAC);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_LOAS);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_PLUS);
		ENUM_TO_STR(eDxAUDIO_CODEC_AAC_PLUS_ADTS);
		ENUM_TO_STR(eDxAUDIO_CODEC_DTS);
		ENUM_TO_STR(eDxAUDIO_CODEC_DTS_HD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_DVD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_HD_DVD);
		ENUM_TO_STR(eDxAUDIO_CODEC_LPCM_BLUERAY);
		ENUM_TO_STR(eDxAUDIO_CODEC_WMA_STD);
		ENUM_TO_STR(eDxAUDIO_CODEC_WMA_PRO);
		ENUM_TO_STR(eDxAUDIO_CODEC_PCMWAVE);
		ENUM_TO_STR(eDxAUDIO_CODEC_DRA);

		ENUM_TO_STR(eDxAUDIO_CODEC_AIFF);
		ENUM_TO_STR(eDxAUDIO_CODEC_RIFF);
		ENUM_TO_STR(eDxAUDIO_CODEC_FromLIVE);
*/
	default:
		break;
	}
	return NULL;
}

STATIC HBOOL _mgr_LiveView_base_ProfileCvtAsciiToCheckAsterisk_Base(const HCHAR *ascii)
{
	/* if property is '*', bUsingLiveXXX is TRUE */
	if(HxSTD_StrCmp("*", ascii) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC DxAudioCodec_e _mgr_LiveView_base_ProfileCvtAsciiToAudioCodec_Base(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxAUDIO_CODEC_MAX_NUM;
	HCHAR		*text = NULL;

	for(i=0; i < max; i++)
	{
		text = _mgr_LiveView_base_ProfileCvtAudioCodecToAscii_Base((DxAudioCodec_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return (DxAudioCodec_e)i;
		}
	}

	return eDxVIDEO_CODEC_UNKNOWN;
}

STATIC HCHAR *_mgr_LiveView_base_ProfileCvtVideoAspectRatioToAscii_Base(DxAspectRatio_e eAspectRatio)
{
	switch (eAspectRatio)
	{
	case eDxASPECTRATIO_4X3:	return "4:3";
	case eDxASPECTRATIO_14X9:	return "14:9";
	case eDxASPECTRATIO_16X9:	return "16:9";
/*
		ENUM_TO_STR(eDxASPECTRATIO_UNKNOWN);
		ENUM_TO_STR(eDxASPECTRATIO_4X3);
		ENUM_TO_STR(eDxASPECTRATIO_16X9);
		ENUM_TO_STR(eDxASPECTRATIO_1X1);
		ENUM_TO_STR(eDxASPECTRATIO_14X9);
		ENUM_TO_STR(eHxASPECTRATIO_FromLIVE);
		ENUM_TO_STR(eDxASPECTRATIO_MAX_NUM);
*/
		default: break;
	}

	return NULL;
}

STATIC DxAspectRatio_e _mgr_LiveView_base_ProfileCvtAsciiToVideoAspectRatio_Base(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxASPECTRATIO_MAX_NUM;
	HCHAR		*text = NULL;

	for(i=0; i < max; i++)
	{
		text = _mgr_LiveView_base_ProfileCvtVideoAspectRatioToAscii_Base((DxAspectRatio_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return (DxAspectRatio_e)i;
		}
	}

	return eDxVIDEO_CODEC_UNKNOWN;
}


STATIC HCHAR *_mgr_LiveView_base_ProfileCvtVidoeEncCodecLevelToAscii_Base(DxVideoEncodeLevel_e level)
{
	switch (level)
	{
	case eDxVIDEO_ENC_LEVEL_30:	return "30";
	case eDxVIDEO_ENC_LEVEL_31:	return "31";
/*
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_00);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_10);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_1B);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_11);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_12);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_13);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_20);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_21);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_22);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_30);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_31);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_32);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_40);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_41);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_42);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_50);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_51);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_60);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_62);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_LOW);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_MAIN);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_HIGH);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_HIGH1440);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_AUTO);
		ENUM_TO_STR(eDxVIDEO_ENC_LEVEL_NONE);
*/
		default: break;
	}

	return NULL;
}


STATIC DxVideoEncodeLevel_e _mgr_LiveView_base_ProfileCvtAsciiToVidoeEncCodecLevel_Base(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxVIDEO_ENC_LEVEL_MAX;
	HCHAR		*text = NULL;

	for(i=0; i < max; i++)
	{
		text = _mgr_LiveView_base_ProfileCvtVidoeEncCodecLevelToAscii_Base((DxVideoEncodeLevel_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return (DxVideoEncodeLevel_e)i;
		}
	}

	return eDxVIDEO_ENC_LEVEL_30;
}



STATIC HCHAR *_mgr_LiveView_base_ProfileCvtVidoeEncCodecProfileToAscii_Base(DxVideoEncodeProfile_e level)
{
	switch (level)
	{
		case eDxVIDEO_ENC_PROFILE_MAIN:		return "main";
		case eDxVIDEO_ENC_PROFILE_BASELINE:	return "baseline";
/*
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_SIMPLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_MAIN);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_HIGH);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_ADVANCED);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_JIZHUN);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_SNRSCALABLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_SPATIALLYSCALABLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_ADVANCEDSIMPLE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_BASELINE);
		ENUM_TO_STR(eDxVIDEO_ENC_PROFILE_AUTO);
*/
		default: break;
	}

	return NULL;
}


STATIC DxVideoEncodeProfile_e _mgr_LiveView_base_ProfileCvtAsciiToVidoeEncCodecProfile_Base(const HCHAR *ascii)
{
	HINT32		i = 0;
	HINT32		max = (HINT32)eDxVIDEO_ENC_PROFILE_MAX_NUM;
	HCHAR		*text = NULL;

	for(i=0; i < max; i++)
	{
		text = _mgr_LiveView_base_ProfileCvtVidoeEncCodecProfileToAscii_Base((DxVideoEncodeProfile_e)i);
		if(text == NULL)
		{
			continue;
		}

		if(HxSTD_StrCmp(text, ascii) == 0)
		{
			return (DxVideoEncodeProfile_e)i;
		}
	}

	return eDxVIDEO_ENC_PROFILE_BASELINE;
}

STATIC HERROR _mgr_LiveView_base_ProfileGetItem(const HCHAR *pProfileString, MgrLive_LiveStreamArg_t *pstDestArg)
{
	if ((NULL == pProfileString) || (NULL == pstDestArg))
		return ERR_FAIL;

	/* parsing */
	{
		const HINT32	DEFAULT_VIDEO_BITRATE	=	1120;
		const HINT32	DEFAULT_SCREEN_WIDTH	=	640;
		const HINT32	DEFAULT_SCREEN_HEIGHT	=	360;
		const HINT32	DEFAULT_AUDIO_BITRATE	=	64;
		const HINT32	DEFAULT_FRAMERATE		=	50;

		HxJSON_t pJsonRoot = NULL;
		pJsonRoot = HLIB_JSON_Decode(pProfileString);

		if (NULL != pJsonRoot)
		{
			HINT32	nVideoBitrateKbps		= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "video_bitrate_kbps", DEFAULT_VIDEO_BITRATE);
//			HINT32	nSdVideoBitrateKbps 	= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "sd_video_bitrate_kbps", nVideoBitrateKbps);
			HINT32	nHdVideoBitrateKbps 	= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "hd_video_bitrate_kbps", nVideoBitrateKbps);
			HINT32	nScreenWidth			= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "screen_width", DEFAULT_SCREEN_WIDTH);
			HINT32	nScreenHeight			= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "screen_height", DEFAULT_SCREEN_HEIGHT);

			HBOOL	bInterlaceMode			= HLIB_JSON_Object_GetBoolean(pJsonRoot, "interlace_mode");

			HINT32	nAudioBitrateKbps	= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "audio_bitrate_kbps", DEFAULT_AUDIO_BITRATE);
			HINT32	nFrameRate			= HLIB_JSON_Object_GetIntegerWithDefaultValue(pJsonRoot, "frame_rate", DEFAULT_FRAMERATE);
//			HBOOL	bUseTranscoder		= HLIB_JSON_Object_GetBoolean(pJsonRoot, "use_transcoding");

			DxAspectRatio_e eVideoAspectRatio = 0;
			DxVideoCodec_e	eVideoCodec = 0;
			DxAudioCodec_e	eAudioCodec = 0;
			HBOOL bUsingLiveAudioCodec = 0;
			DxVideoEncodeLevel_e eVideoCodecLevel = 0;
			DxVideoEncodeProfile_e eVideoCodecProfile = 0;
			HBOOL	bUsingLiveAspectRatio = 0;

			/* string to value */
			{
				const HCHAR *json_str = NULL;

				json_str = HLIB_JSON_Object_GetString(pJsonRoot, "aspect_ratio");
				HxLOG_Assert(json_str);
				eVideoAspectRatio		= _mgr_LiveView_base_ProfileCvtAsciiToVideoAspectRatio_Base(json_str);
				bUsingLiveAspectRatio	= _mgr_LiveView_base_ProfileCvtAsciiToCheckAsterisk_Base(json_str);

				json_str = HLIB_JSON_Object_GetString(pJsonRoot, "video_enc_codec");
				HxLOG_Assert(json_str);
				eVideoCodec = _mgr_LiveView_base_ProfileCvtAsciiToVideoCodec_Base(json_str);

				json_str = HLIB_JSON_Object_GetString(pJsonRoot, "audio_enc_codec");
				HxLOG_Assert(json_str);
				eAudioCodec 		= _mgr_LiveView_base_ProfileCvtAsciiToAudioCodec_Base(json_str);
				bUsingLiveAudioCodec	= _mgr_LiveView_base_ProfileCvtAsciiToCheckAsterisk_Base(json_str);

				json_str = HLIB_JSON_Object_GetString(pJsonRoot, "video_enc_codec_level");
				HxLOG_Assert(json_str);
				eVideoCodecLevel		= _mgr_LiveView_base_ProfileCvtAsciiToVidoeEncCodecLevel_Base(json_str);

				json_str = HLIB_JSON_Object_GetString(pJsonRoot, "video_enc_codec_profile");
				HxLOG_Assert(json_str);
				eVideoCodecProfile	= _mgr_LiveView_base_ProfileCvtAsciiToVidoeEncCodecProfile_Base(json_str);
			}

			pstDestArg->stProfile.eVideoCodec			=	eVideoCodec;
			pstDestArg->stProfile.eAudioCodec			=	eAudioCodec;
			pstDestArg->stProfile.nAudioBitrateKbps 	=	nAudioBitrateKbps;
			pstDestArg->stProfile.nVideoBitrateKbps 	=	nVideoBitrateKbps;
			pstDestArg->stProfile.nHdVideoBitrateKbps	=	nHdVideoBitrateKbps;
			pstDestArg->stProfile.nFrameRate			=	nFrameRate;
			pstDestArg->stProfile.nScreenWidth			=	nScreenWidth;
			pstDestArg->stProfile.nScreenHeight 		=	nScreenHeight;
			pstDestArg->stProfile.bInterlaceMode		=	bInterlaceMode;
			pstDestArg->stProfile.eVideoAspectRatio 	=	eVideoAspectRatio;
			pstDestArg->stProfile.eVideoCodecProfile	=	eVideoCodecProfile;
			pstDestArg->stProfile.eVideoCodecLevel		=	eVideoCodecLevel;
			pstDestArg->stProfile.bUsingLiveAudioCodec	=	bUsingLiveAudioCodec;
			pstDestArg->stProfile.bUsingLiveAspectRatio =	bUsingLiveAspectRatio;
		}
		else
		{
			HxLOG_Error("Error. \n");
			return ERR_FAIL;
		}

		if (NULL != pJsonRoot)
		{
			HLIB_JSON_Delete(pJsonRoot);
			pJsonRoot = NULL;
		}
	}

	return ERR_OK;
}

STATIC HERROR _mgr_LiveView_base_SetupAction(HINT32 ulActionId, Handle_t hSvc, MgrAction_Priority_e ePriority, MgrLive_LiveMode_e eLiveMode)
{
	HERROR hErr = ERR_FAIL;

	switch (eLiveMode)
	{
		case eMgrLiveMode_MainLive:
			HxLOG_Print(" [eMgrLiveMode_MainLive] \n");
			hErr = MGR_ACTION_SetupLive(ulActionId, hSvc, ePriority, eViewType_WTV);
			break;

		case eMgrLiveMode_PipLive:
			HxLOG_Print(" [eMgrLiveMode_PipLive] \n");
			hErr = MGR_ACTION_SetupLive(ulActionId, hSvc, ePriority, eViewType_PIP);
			break;

		case eMgrLiveMode_LiveStreaming:
			HxLOG_Print(" [eMgrLiveMode_LiveStreaming] \n");
			hErr = MGR_ACTION_SetupLiveStream (ulActionId, hSvc, ePriority, eLiveStreamMode_ViaLiveView);
			break;

		case eMgrLiveMode_FccLive:
			HxLOG_Print(" [eMgrLiveMode_FccLive] \n");
			hErr = MGR_ACTION_SetupLive (ulActionId, hSvc, eAmPriority_BGJOB, eViewType_FCC);
			break;

		default:
			HxLOG_Error("[AP:LiveView(%d)] invalid live mode(%d). set MainLive  \n",ulActionId, eLiveMode);
			hErr = MGR_ACTION_SetupLive(ulActionId, hSvc, ePriority, eViewType_WTV);
			break;
	}

	return hErr;
}

STATIC HERROR _mgr_LiveView_startSiService (XmgrLiveView_Context_t *pstContext)
{
	SvcSi_LiveSpec_t	 stSiSpec;
	HERROR				 hErr = ERR_FAIL;

	pstContext->bPmtValid = FALSE;

	hErr = MGR_LIVE_GetLiveSiSpec (&(pstContext->stStartArg), &stSiSpec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_LIVE_GetLiveSiSpec err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->hSvc);
		return ERR_FAIL;
	}

	hErr = SVC_SI_StartLive (pstContext->hAction, &stSiSpec, pstContext->hSvc, FALSE,
#if defined(CONFIG_SUPPORT_FCC)
		((pstContext->eLiveMode == eMgrLiveMode_FccLive) ? TRUE : FALSE)
#else
		FALSE
#endif
		);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_SI_StartLive err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->hSvc);
		return ERR_FAIL;
	}

	pstContext->aeServicesState[eLiveMwSvcType_Si] = eLiveMwSvcState_ReqStart;
	return ERR_OK;
}

STATIC HERROR _mgr_LiveView_startCasService (XmgrLiveView_Context_t *pstContext)
{
	HERROR			 hErr = ERR_FAIL;

	HxLOG_Print ("hSvc(0x%X) dmxId(%d) \n", pstContext->hSvc, pstContext->stRscInfo.astRscItem[eRxRSCTYPE_DEMUX].ulResourceId);

	hErr = MGR_CAS_StartService (pstContext->hAction, pstContext->hSvc, eCAS_ActionType_Live, eDxCAS_GROUPID_NONE, NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_CAS_StartService err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->hSvc);
		return ERR_FAIL;
	}

	hErr = SVC_CAS_SetPid (pstContext->hAction,
						eCAS_ActionType_Live,
						DbSvc_GetVideoPid(&pstContext->stSvcInfo),
						DbSvc_GetAudioPid(&pstContext->stSvcInfo),
						DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo),
						DbSvc_GetDolbyPid(&pstContext->stSvcInfo),
						PID_NULL,
						PID_NULL);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_CAS_SetPid err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->hSvc);
		return ERR_FAIL;
	}

	pstContext->aeServicesState[eLiveMwSvcType_Cas] = eLiveMwSvcState_ReqStart;
	return ERR_OK;
}

STATIC HERROR _mgr_LiveView_startAvService (XmgrLiveView_Context_t *pstContext)
{
	HERROR				hErr = ERR_FAIL;
	DxDualMonoSelect_e	eDualMonoConfig = eDxDUALMONO_SELECT_BOTH;

	HxLOG_Print("hAction(0x%X) avDecId(%d) dmxId(%d))\n", pstContext->hAction, pstContext->stRscInfo.astRscItem[eRxRSCTYPE_AV_DEC].ulResourceId, pstContext->stRscInfo.astRscItem[eRxRSCTYPE_DEMUX].ulResourceId);
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_DUALMONO_SELECT, (HUINT32 *)&eDualMonoConfig, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_DUALMONO_SELECT) Error!!!\n");
	}

#if !defined(SvcAv_START_SERVICE_NEW)
	{
		DxStereoSelect_e	eStereoSelect = DbSvc_GetStereoSelect(&pstContext->stSvcInfo);

		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_STEREO_SELECT, (HUINT32 *)&eStereoSelect, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_STEREO_SELECT) Error!!!\n");
		}

		hErr = SVC_AV_SetStereoSelect(pstContext->ulActionId, eStereoSelect);
		if (hErr)
		{
			HxLOG_Error("SVC_AV_SetStereoSelect ret :0x%X\n",hErr);
		}
	}
#endif

	eDualMonoConfig = DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo);
	hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, eDualMonoConfig);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
	}

	{
		HUINT16			 usAudioPid = PID_NULL, usAudioAuxPid = PID_NULL;
		HUINT16			 usDolbyPid = PID_NULL, usVideoPid = PID_NULL;

		usAudioPid = _mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioPid(&pstContext->stSvcInfo));
		usAudioAuxPid = _mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo));
		usDolbyPid = _mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetDolbyPid(&pstContext->stSvcInfo));
		usVideoPid = _mgr_LiveView_base_GetVideoPid(pstContext, DbSvc_GetVideoPid(&pstContext->stSvcInfo));

		if (((0 == usAudioPid) || (PID_NULL == usAudioPid)) &&
			((0 == usAudioAuxPid) || (PID_NULL == usAudioAuxPid)) &&
			((0 == usDolbyPid) || (PID_NULL == usDolbyPid)) &&
			((0 == usVideoPid) || (PID_NULL == usVideoPid)))
		{
			HxLOG_Error("No PID at all... AV Service cannot be started\n");
			return ERR_FAIL;
		}

		HxLOG_Info("PID info exists... Start the AV service\n");
	}

#if defined(SvcAv_START_SERVICE_NEW)
	hErr = SVC_AV_StartService (pstContext->hAction,
								pstContext->stRscInfo.astRscItem[eRxRSCTYPE_AV_DEC].ulResourceId,
								pstContext->stRscInfo.astRscItem[eRxRSCTYPE_DEMUX].ulResourceId,
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetVideoPid(pstContext, DbSvc_GetVideoPid(&pstContext->stSvcInfo)),
								DbSvc_GetPcrPid(&pstContext->stSvcInfo),
								DbSvc_GetAudioCodec(&pstContext->stSvcInfo),
								DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo),
								DbSvc_GetVideoCodec(&pstContext->stSvcInfo),
								FALSE,
#if defined(CONFIG_SUPPORT_FCC)
								TRUE
#else
								FALSE
#endif
								);

#else
	hErr = SVC_AV_StartService (pstContext->hAction,
								pstContext->stRscInfo.astRscItem[eRxRSCTYPE_AV_DEC].ulResourceId,
								pstContext->stRscInfo.astRscItem[eRxRSCTYPE_DEMUX].ulResourceId,
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetDolbyPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetVideoPid(pstContext, DbSvc_GetVideoPid(&pstContext->stSvcInfo)),
								DbSvc_GetPcrPid(&pstContext->stSvcInfo),
								DbSvc_GetAudioCodec(&pstContext->stSvcInfo),
								DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo),
								DbSvc_GetDolbyCodec(&pstContext->stSvcInfo),
								DbSvc_GetVideoCodec(&pstContext->stSvcInfo),
								DbSvc_GetDolbyFlag(&pstContext->stSvcInfo),
								DbSvc_GetStereoSelect(&pstContext->stSvcInfo),
								eDualMonoConfig,
								FALSE,
#if defined(CONFIG_SUPPORT_FCC)
								TRUE
#else
								FALSE
#endif
								);
#endif
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_AV_StartService err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->hSvc);
		return ERR_FAIL;
	}

	hErr = SVC_AV_SetAudioHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_View,  FALSE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetAudioHide ret :0x%X\n", hErr);
	}

	hErr = SVC_AV_SetVideoHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_View, FALSE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetVideoHide ret :0x%X\n", hErr);
	}

	hErr = SVC_AV_SetVideoOutputEnable(SVC_PIPE_GetActionId(pstContext->hAction), eSvcAv_DisplayType_Hd, TRUE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetVideoOutputEnable ret :0x%X\n", hErr);
	}

	hErr = SVC_AV_SetVideoOutputEnable(SVC_PIPE_GetActionId(pstContext->hAction), eSvcAv_DisplayType_Sd, TRUE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetVideoOutputEnable ret :0x%X\n", hErr);
	}


	pstContext->aeServicesState[eLiveMwSvcType_Av] = eLiveMwSvcState_ReqStart;
	return ERR_OK;
}

STATIC HERROR _mgr_LiveView_startTranscoderService (XmgrLiveView_Context_t *pstContext)
{
#if defined(CONFIG_MW_MM_TRANSCODING)

	HERROR						hErr = ERR_FAIL;
#if 0
	DbSvc_Info_t		stSvcInfo;
#endif
	SvcTranscoder_Settings_t 	stSettings;
	HBOOL 						bValidPID	=	FALSE;
	HUINT16 					usVideoPid, usAudioPid;
	DxAudioCodec_e 				eAudioCodec;
	SvcSi_MakeSectionSpec_t		stMakeSecSpec;

	HxLOG_Print("hAction(0x%X) avDecId(%d) dmxId(%d))\n", pstContext->hAction, pstContext->stRscInfo.astRscItem[eRxRSCTYPE_AV_DEC].ulResourceId, pstContext->stRscInfo.astRscItem[eRxRSCTYPE_DEMUX].ulResourceId);

	HxSTR_Memset(&stSettings, 0x00, sizeof(SvcTranscoder_Settings_t));
	usVideoPid = usAudioPid = PID_NULL;

	switch (pstContext->eLiveMode)
	{
		case eMgrLiveMode_LiveStreaming:
			// HLS test를 위해 TRUE로 잠시 바꿈 - param으로 전달 받은 후에 다시 수정
			stSettings.bTranscode 			=  TRUE;//	pstContext->stStartArg.unExtArg.stLiveStreamArg.bTranscode;

			usVideoPid = DbSvc_GetVideoPid(&pstContext->stSvcInfo);
			usAudioPid = DbSvc_GetAudioPid(&pstContext->stSvcInfo);
			eAudioCodec = DbSvc_GetAudioCodec(&pstContext->stSvcInfo);

			if( PID_NULL == usAudioPid )
			{
				usAudioPid = DbSvc_GetDolbyPid(&pstContext->stSvcInfo);
				eAudioCodec = DbSvc_GetDolbyCodec(&pstContext->stSvcInfo);
			}

			/*include PCR Ts packet*/
			stSettings.bPcrTsPacketInsert		=	pstContext->stStartArg.unExtArg.stLiveStreamArg.bPcrTsPacketInsert;

			/*status*/
			switch(DbSvc_GetSvcType(&pstContext->stSvcInfo))
			{
			case eDxSVC_TYPE_TV:
				if( PID_NULL != usVideoPid )
				{
					stSettings.eTransEncodeMode 	=	eSVC_TRANSCODER_ENCODEMODE_AV;
					bValidPID = TRUE;
				}
				break;

			case eDxSVC_TYPE_RADIO:
				if( PID_NULL != usAudioPid )
				{
					stSettings.eTransEncodeMode 	=	eSVC_TRANSCODER_ENCODEMODE_AUDIO;
					bValidPID = TRUE;
				}
				break;

			default:
				HxLOG_Critical("is broadcast service?? handle(0x%x) stSvcInfo.eSvcType(%d)\r\n", pstContext->stStartArg.hService, DbSvc_GetSvcType(&pstContext->stSvcInfo));
				bValidPID = FALSE;
				break;
			}

			/*pid check*/
			if( FALSE == bValidPID )
			{
				HxLOG_Critical("Should be checked AV PID! \r\n");
				return ERR_FAIL;
			}

			stSettings.eTransOutputType		=	eSVC_TRANSCODER_OUTPUTTYPE_BUFFER;
			stSettings.eTransSourceType		=	eSVC_TRANSCODER_SRCTYPE_Live;

#if 0 // File save Test
			stSettings.eTransOutputType = eSVC_TRANSCODER_OUTPUTTYPE_FILE;
			memcpy(&stSettings.acuFilename, "/test", 8);
#endif
			/*resource id*/
			stSettings.ulRecDeviceId		= 	pstContext->stRscInfo.astRscItem[eRxRSCTYPE_HDD_REC].ulResourceId;
			stSettings.ulDeviceId			=	pstContext->stRscInfo.astRscItem[eRxRSCTYPE_TRANSCODER].ulResourceId;
			stSettings.ulDemuxId			=	pstContext->stRscInfo.astRscItem[eRxRSCTYPE_DEMUX].ulResourceId;

			/*src*/
			stSettings.stAvPid.usVideoPid	=	usVideoPid;
			stSettings.stAvPid.usAudioPid	=	usAudioPid;
			stSettings.stAvPid.usPcrPid		=	DbSvc_GetPcrPid(&pstContext->stSvcInfo);
			stSettings.eVideoCodec			=	DbSvc_GetVideoCodec(&pstContext->stSvcInfo);
			stSettings.eAudioCodec			=	eAudioCodec;

			/*profile*/
			stSettings.nTranscodeProfileId				=	-1;
			stSettings.stProfile.eVideoCodec			=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.eVideoCodec;
			stSettings.stProfile.eAudioCodec			=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.eAudioCodec;
			stSettings.stProfile.ucVideoStreamType		=	0;//pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.;
			stSettings.stProfile.ucAudioStreamType		=	0;//pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.;

			stSettings.stProfile.nAudioBitrate_kbps		=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.nAudioBitrateKbps;
			stSettings.stProfile.nVideoBitrate_kbps		=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.nVideoBitrateKbps;
			stSettings.stProfile.nHdVideoBitrate_kbps	=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.nHdVideoBitrateKbps;
			stSettings.stProfile.nScreenWidth			=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.nScreenWidth;
			stSettings.stProfile.nScreenHeight			=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.nScreenHeight;
			stSettings.stProfile.nFrameRate				=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.nFrameRate;

			stSettings.stProfile.bIsInteraceMode		=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.bInterlaceMode;

			stSettings.stProfile.eVideoAspectRatio		=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.eVideoAspectRatio;

			stSettings.stProfile.eVideoCodecProfile			=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.eVideoCodecProfile;
			stSettings.stProfile.eVideoCodecLevel			=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.eVideoCodecLevel;
			stSettings.stProfile.bUsingLiveAudioCodec		=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.bUsingLiveAudioCodec;
			stSettings.stProfile.bUsingLiveAspectRatio		=	pstContext->stStartArg.unExtArg.stLiveStreamArg.stProfile.bUsingLiveAspectRatio;

			/*mem buffer handle*/
			stSettings.hRingBuf				= 	pstContext->stStartArg.unExtArg.stLiveStreamArg.hRingBuf;

			break;

		default:
			HxLOG_Error ("invalid eLiveMode(%d) \n", pstContext->eLiveMode);
			/* fall through */
	}

	hErr = SVC_TRANSCODER_Open(pstContext->hAction, &stSettings, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("SVC_TRANSCODER_Open err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->hSvc);
		return ERR_FAIL;
	}

	/*PSI setting*/
	{
		SvcSi_MakeSecArg_t 			 unArg;
		SvcTranscoder_PSITable_t	*pstTrPsiTable;

		pstTrPsiTable = (SvcTranscoder_PSITable_t *)OxAP_Malloc (sizeof(SvcTranscoder_PSITable_t));
		if (NULL != pstTrPsiTable)
		{
			// PAT
			unArg.eSiType			=	eSIxTABLETYPE_Pat;
			unArg.stPat.eSiType		=	eSIxTABLETYPE_Pat;
			unArg.stPat.usTsId		=	DbSvc_GetTsId(&pstContext->stSvcInfo); //SVC_SI_UTIL_GetTsUniqIdFromSvcHandle (pstContext->hSvc);
			unArg.stPat.usSvcId		=	DbSvc_GetSvcId(&pstContext->stSvcInfo);
			unArg.stPat.usPmtPid 	=	DbSvc_GetPmtPid(&pstContext->stSvcInfo);

			pstTrPsiTable->ulRawSectionSize	=	0;
			HxSTD_MemSet(pstTrPsiTable->aucRawSectionBuffer, 0, MAX_TRANS_RAWSECTION_LEN);

			hErr = MGR_TRANSCODER_GetMakeSectionSiSpec(&stMakeSecSpec);
			if (hErr == ERR_OK)
			{
				hErr = SVC_SI_MakeSiSection(pstContext->ulActionId, &stMakeSecSpec, &unArg, pstTrPsiTable->aucRawSectionBuffer, &(pstTrPsiTable->ulRawSectionSize));
			}

#ifdef _DUMP_SECTION_
			HxLOG_DecIntDump(hErr);
			HxLOG_DecIntDump(unArg.stPat.eSiType);
			HxLOG_HexIntDump(unArg.stPat.usTsId);
			HxLOG_HexIntDump(unArg.stPat.usSvcId);
			HxLOG_HexIntDump(unArg.stPat.usPmtPid);
			HxLOG_DecIntDump(pstTrPsiTable->ulRawSectionSize);
			HLIB_LOG_Dump(pstTrPsiTable->aucRawSectionBuffer, 64, 0, 0);
#endif

			if ((ERR_OK == hErr) && (pstTrPsiTable->ulRawSectionSize > 0))
			{
				pstTrPsiTable->bContinue			=	TRUE;
				pstTrPsiTable->usPeriodTime			=	_SECTION_INSERTION_PERIOD;
				pstTrPsiTable->usPid				=	PAT_PID;
				pstTrPsiTable->eSiType				=	eSI_TableType_PAT;

				hErr = SVC_TRANSCODER_SetPSITable (pstContext->hAction, pstTrPsiTable, eSyncMode);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_TRANSCODER_SetPSITable err: PAT\n");
				}
			}
			else
			{
				HxLOG_Error ("SVC_SI_MakeSiSection: siType:PAT, ulActionId(%d)\n", pstContext->ulActionId);
			}

			// PMT
			unArg.eSiType				=	eSIxTABLETYPE_Pmt;
			unArg.stPmt.eSiType			=	eSIxTABLETYPE_Pmt;
			unArg.stPmt.usTsUniqId		=	pstContext->tsInfo.usUniqueId;
			unArg.stPmt.usSvcId			=	DbSvc_GetSvcId(&pstContext->stSvcInfo);
			unArg.stPmt.hSvc			=	pstContext->hSvc;

			if( TRUE == stSettings.bTranscode )
			{
				// 위에서 SVC_TRANSCODER_Open 에 넣은 Setting과 같은 PID를 넣어 주어야 한다.
				unArg.stPmt.eMakeSrcType 	=	eSVCSI_MAKESEC_PMT_SetByArgs;
				unArg.stPmt.unArgs.stSettingArg.ucVideoStreamType	= ISO_VIDEO_4;			// H.264로 Transcoding 했을 것이므로 ISO_VIDEO_4로
				unArg.stPmt.unArgs.stSettingArg.usVideoPid			= usVideoPid;
				unArg.stPmt.unArgs.stSettingArg.ucAudioStreamType	= ISO_AUDIO_AAC;		// AAC이므로 ISO_AUDIO_4 혹은 ISO_AUDIO_AAC
				unArg.stPmt.unArgs.stSettingArg.usAudioPid			= usAudioPid;
#if CONFIG_TRANSCODER_FIXED_PCRPID
				unArg.stPmt.unArgs.stSettingArg.usPcrPid 			= CONFIG_TRANSCODER_FIXED_PCRPID;
#else
				unArg.stPmt.unArgs.stSettingArg.usPcrPid 			= DbSvc_GetPcrPid(&pstContext->stSvcInfo);
#endif
			}
			else
			{
				unArg.stPmt.eMakeSrcType 	=	eSVCSI_MAKESEC_PMT_SiSections;
			}

			pstTrPsiTable->ulRawSectionSize	=	0;
			HxSTD_MemSet(pstTrPsiTable->aucRawSectionBuffer, 0, MAX_TRANS_RAWSECTION_LEN);
			hErr = MGR_TRANSCODER_GetMakeSectionSiSpec(&stMakeSecSpec);
			if (hErr == ERR_OK)
			{
				hErr = SVC_SI_MakeSiSection(pstContext->ulActionId, &stMakeSecSpec, &unArg, pstTrPsiTable->aucRawSectionBuffer, &(pstTrPsiTable->ulRawSectionSize));
			}

#ifdef _DUMP_SECTION_
			HxLOG_DecIntDump(hErr);
			HxLOG_DecIntDump(stSettings.bTranscode);
			HxLOG_DecIntDump(unArg.stPmt.eSiType);
			HxLOG_HexIntDump(unArg.stPmt.usTsUniqId);
			HxLOG_HexIntDump(unArg.stPmt.usSvcId);
			HxLOG_HexIntDump(unArg.stPmt.hSvc);
			HxLOG_HexIntDump(unArg.stPmt.unArgs.stSettingArg.usVideoPid);
			HxLOG_HexIntDump(unArg.stPmt.unArgs.stSettingArg.usAudioPid);
			HxLOG_HexIntDump(unArg.stPmt.unArgs.stSettingArg.usPcrPid);
			HxLOG_DecIntDump(pstTrPsiTable->ulRawSectionSize);
			HLIB_LOG_Dump(pstTrPsiTable->aucRawSectionBuffer, 64, 0, 0);
#endif

			if ((ERR_OK == hErr) && (pstTrPsiTable->ulRawSectionSize > 0))
			{
				pstTrPsiTable->bContinue		=	TRUE;
				pstTrPsiTable->usPeriodTime		=	_SECTION_INSERTION_PERIOD;
				pstTrPsiTable->usPid			=	DbSvc_GetPmtPid(&pstContext->stSvcInfo);
				pstTrPsiTable->eSiType			=	eSI_TableType_PMT;

				hErr = SVC_TRANSCODER_SetPSITable (pstContext->hAction, pstTrPsiTable, eSyncMode);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_TRANSCODER_SetPSITable err: PMT\n");
				}
			}
			else
			{
				HxLOG_Error ("SVC_SI_MakeSiSection: siType:PMT, ulActionId(%d)\n", pstContext->ulActionId);
			}

			// SIT : 지원하면 하고, 안 하면 스킵되도록 :
			unArg.eSiType				=	eSIxTABLETYPE_Sit;
			unArg.stSit.eSiType			=	eSIxTABLETYPE_Sit;
			unArg.stSit.usTsUniqId		= 	pstContext->tsInfo.usUniqueId;
			unArg.stSit.usOnId			=	DbSvc_GetOnId(&pstContext->stSvcInfo);
			unArg.stSit.usTsId			=	DbSvc_GetTsId(&pstContext->stSvcInfo);
			unArg.stSit.usSvcId			=	DbSvc_GetSvcId(&pstContext->stSvcInfo);

			pstTrPsiTable->ulRawSectionSize	=	0;
			HxSTD_MemSet(pstTrPsiTable->aucRawSectionBuffer, 0, MAX_TRANS_RAWSECTION_LEN);
			hErr = MGR_TRANSCODER_GetMakeSectionSiSpec(&stMakeSecSpec);
			if (hErr == ERR_OK)
			{
				hErr = SVC_SI_MakeSiSection(pstContext->ulActionId, &stMakeSecSpec, &unArg, pstTrPsiTable->aucRawSectionBuffer, &(pstTrPsiTable->ulRawSectionSize));
			}

#ifdef _DUMP_SECTION_
			HxLOG_DecIntDump(hErr);
			HxLOG_DecIntDump(unArg.stSit.eSiType);
			HxLOG_HexIntDump(unArg.stSit.usTsUniqId);
			HxLOG_HexIntDump(unArg.stSit.usOnId);
			HxLOG_HexIntDump(unArg.stSit.usTsId);
			HxLOG_HexIntDump(unArg.stSit.usSvcId);
			HxLOG_DecIntDump(pstTrPsiTable->ulRawSectionSize);
			HLIB_LOG_Dump(pstTrPsiTable->aucRawSectionBuffer, 64, 0, 0);
#endif

			if ((ERR_OK == hErr) && (pstTrPsiTable->ulRawSectionSize > 0))
			{
				pstTrPsiTable->bContinue		=	TRUE;
				pstTrPsiTable->usPeriodTime 	=	1000;
				pstTrPsiTable->usPid			=	SIT_PID;
				pstTrPsiTable->eSiType			=	eSI_TableType_SIT;

				hErr = SVC_TRANSCODER_SetPSITable (pstContext->hAction, pstTrPsiTable, eSyncMode);
				if (ERR_OK != hErr)
				{
					HxLOG_Error ("SVC_TRANSCODER_SetPSITable err: SIT\n");
				}
			}
			else
			{
				HxLOG_Error ("SVC_SI_MakeSiSection: siType:SIT, ulActionId(%d)\n", pstContext->ulActionId);
			}

			OxAP_Free (pstTrPsiTable);
		}
	}

	hErr = SVC_TRANSCODER_StartService(pstContext->hAction, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("OxSVC_Transcoder_Start err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->hSvc);
		return ERR_FAIL;
	}

	pstContext->aeServicesState[eLiveMwSvcType_Transcoder] = eLiveMwSvcState_ReqStart;
	return ERR_OK;

#else
	return ERR_FAIL;
#endif
}

#if defined(CONFIG_MW_SUBTITLE)
STATIC void _mgr_LiveView_base_StartSubtitle(XmgrLiveView_Context_t *pstContext)
{
	HUINT32				ulActionId = pstContext->ulActionId;

	HxLOG_Info("[AP:LiveView(%d)] SubtitleAllowed(%d), TtxPageEnable(%d), AppState(%d) \n\n",ulActionId,
			pstContext->bSubtitleAllowed, pstContext->bTtxPageEnabled, pstContext->eAppState);

	if ((_mgr_LiveView_base_CheckComplementariesStartCondition(pstContext) == TRUE)
		&& (pstContext->eAppState >= eLiveViewState_AV)
	)
	{
		/* Subtitle 허가되고 teletext 상태가 아니면 subtitle activate한다. */
		XMGR_LiveComp_BASE_StartSubtitle(pstContext->hAction);
	}
	else
	{
		HxLOG_Print ("[AP:LiveView(%d)] \t=> Subtitle is not allowed.\n", ulActionId);
	}

	return;
}
#endif

/********************************************************************/
/********************      Local Functions	*************************/
/********************************************************************/

STATIC BUS_Result_t _mgr_LiveView_base_SetupMainLive(XmgrLiveView_Context_t *pstContext)
{
	HERROR		hErr = ERR_FAIL;

	hErr = MGR_ACTION_SetupLive (pstContext->ulActionId, pstContext->hSvc, eAmPriority_VIEW_ACTION, eViewType_WTV);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_ACTION_SetupLive err:\n");
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_Take (pstContext->ulActionId, eViewType_WTV, BUS_MGR_GetMgrCallback (BUS_MGR_GetThis()));
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("(%d) MGR_ACTION_Take err:0x%08x\n", pstContext->ulActionId, hErr);
		return MESSAGE_PASS;
	}

	pstContext->aeServicesStartPhase[eLiveMwSvcType_Si] = eLiveMwSvcExecPhase_AfterChannelTuned;
	pstContext->aeServicesStartPhase[eLiveMwSvcType_Av] = eLiveMwSvcExecPhase_AfterChannelTuned;
	pstContext->aeServicesStartPhase[eLiveMwSvcType_Cas] = eLiveMwSvcExecPhase_AfterChannelTuned;

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t _mgr_LiveView_base_SetupPipLive(XmgrLiveView_Context_t *pstContext)
{
	HERROR		hErr = ERR_FAIL;

	hErr = MGR_ACTION_SetupLive (pstContext->ulActionId, pstContext->hSvc, eAmPriority_VIEW_ACTION, eViewType_PIP);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_ACTION_SetupLive err:\n");
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_Take (pstContext->ulActionId, eViewType_PIP, BUS_MGR_GetMgrCallback (BUS_MGR_GetThis()));
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("(%d) MGR_ACTION_Take err:0x%08x\n", pstContext->ulActionId, hErr);
		return MESSAGE_PASS;
	}

	pstContext->aeServicesStartPhase[eLiveMwSvcType_Si] = eLiveMwSvcExecPhase_AfterChannelTuned;
	pstContext->aeServicesStartPhase[eLiveMwSvcType_Av] = eLiveMwSvcExecPhase_AfterChannelTuned;
	pstContext->aeServicesStartPhase[eLiveMwSvcType_Cas] = eLiveMwSvcExecPhase_AfterChannelTuned;

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t _mgr_LiveView_base_SetupFccLive(XmgrLiveView_Context_t *pstContext)
{
	HERROR		hErr = ERR_FAIL;

	hErr = MGR_ACTION_SetupLive (pstContext->ulActionId, pstContext->hSvc, eAmPriority_BGJOB, eViewType_FCC);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_ACTION_SetupLive err:\n");
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_Take (pstContext->ulActionId, eViewType_FCC, BUS_MGR_GetMgrCallback (BUS_MGR_GetThis()));
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("(%d) MGR_ACTION_Take err:0x%08x\n", pstContext->ulActionId, hErr);
		return MESSAGE_PASS;
	}

	pstContext->aeServicesStartPhase[eLiveMwSvcType_Si] = eLiveMwSvcExecPhase_AfterChannelTuned;
	pstContext->aeServicesStartPhase[eLiveMwSvcType_Av] = eLiveMwSvcExecPhase_AfterChannelTuned;
	pstContext->aeServicesStartPhase[eLiveMwSvcType_Cas] = eLiveMwSvcExecPhase_AfterChannelTuned;

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t _mgr_LiveView_base_SetupLiveStreaming(XmgrLiveView_Context_t *pstContext)
{
	MgrAction_Priority_e		 eActPriority = eAmPriority_RECORD;
	DxLiveStreamPriority_e		 eLsPriority = eDxLsPriority_EQUAL_REC;
	HERROR						 hErr = ERR_FAIL;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LIVESTREAM_PRIORITY, (HUINT32 *)&eLsPriority, 0);
	if (ERR_OK == hErr)
	{
		switch (eLsPriority)
		{
		case eDxLsPriority_EQUAL_REC:
			eActPriority = eAmPriority_RECORD;
			break;

		case eDxLsPriority_EQUAL_VIEW:
			eActPriority = eAmPriority_VIEW_ACTION;
			break;

		case eDxLsPriority_BELOW_VIEW:
			eActPriority = (eAmPriority_VIEW_ACTION - (1<<ACTION_PRIORITY_BIT_OFFSET));
			break;

		case eDxLsPriority_UPPER_REC:
			eActPriority = (eAmPriority_RECORD + (1<<ACTION_PRIORITY_BIT_OFFSET));
			break;

		default:
			break;
		}
	}

	hErr = MGR_ACTION_SetupLiveStream (pstContext->ulActionId, pstContext->hSvc, eActPriority, eLiveStreamMode_ViaLiveView);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("MGR_ACTION_SetupLiveStream err:\n");
		return MESSAGE_PASS;
	}

	hErr = MGR_ACTION_Take (pstContext->ulActionId, eActionType_TRANSCODING_LIVE, BUS_MGR_GetMgrCallback (BUS_MGR_GetThis()));
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("(%d) MGR_ACTION_Take err:0x%08x\n", pstContext->ulActionId, hErr);
		return MESSAGE_PASS;
	}

	pstContext->aeServicesStartPhase[eLiveMwSvcType_Si]			= eLiveMwSvcExecPhase_AfterChannelTuned;
	pstContext->aeServicesStartPhase[eLiveMwSvcType_Transcoder]	= eLiveMwSvcExecPhase_AfterPmtReceived;
	pstContext->aeServicesStartPhase[eLiveMwSvcType_Cas]		= eLiveMwSvcExecPhase_AfterChannelTuned;

	return MESSAGE_BREAK;
}

HERROR _mgr_LiveView_base_StartMasterSession(XmgrLiveView_Context_t *pstContext, Handle_t hSvc, ChListUiGroup_t svcListGroup, MgrLive_TuningMode_t tuningMode)
{
	HUINT32				ulActionId = pstContext->ulActionId;
	HERROR				hErr;
	DbSvc_Info_t			stSvcInfo;
	//SvcRsc_Info_t			stRscInfo;

	HxLOG_Info ("[AP:LiveView(%d)] (hSvc[0x%08x], [%s], mode:[0x%x])\n",ulActionId,
									hSvc, xmgr_live_GetSvcListGroupStr(svcListGroup), tuningMode);
	/* Session version 증가 */
	pstContext->hAction = SVC_PIPE_IncreaseActionVersion(ulActionId);

	/**
	  *	eMEVT_LIVE_NOTIFY_SVC_CHANGED 는 여기서 할 것 다 하고 나서 마지막에 보내야 한다.
	  *	이를 위해 block flag 설정을 하는데, 이 flag는 반드시 이 함수 내에서 TRUE, FALSE 설정이 이루어져야 한다
	**/
	pstContext->bBlockUiStateNoti = TRUE;

	hErr = DB_SVC_GetTsInfoByServiceHandle(hSvc, &pstContext->tsInfo);
	if (hErr)
	{
		HxLOG_Error("DB_SVC_GetTsInfoByServiceHandle ret :0x%X\n",hErr);
	}

/* +++++++++++++++++++++++++++ Context 초기화 ++++++++++++++++++++++++++++ */
	_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_NONE);

	pstContext->sessionType = eLiveViewSession_NORMAL;
	pstContext->svcListGroup = svcListGroup;
	MGR_LIVE_CurrentSvcInfoSet(ulActionId, pstContext->hSvc);

	/* 새 service info update */
	HxSTD_memset(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", hSvc);
	}

	pstContext->stSvcInfo = stSvcInfo;

	DB_SVC_GetServiceNumber(hSvc, &pstContext->nChNum);

	HxLOG_Print("\t============= Start New Service (Digital) ===========\n");
	HxLOG_Print("\tService : [%d] %s\n", pstContext->nChNum, DbSvc_GetName(&stSvcInfo));
	HxLOG_Print("\tService Type : %s\n", (DbSvc_GetSvcType(&stSvcInfo) == eDxSVC_TYPE_TV) ? "TV" :	((DbSvc_GetSvcType(&stSvcInfo) == eDxSVC_TYPE_RADIO) ? "Radio" : "Unkown"));
	HxLOG_Print("\tVideo Type : %s\n", (DbSvc_GetVideoType(&stSvcInfo) == eDxVIDEO_TYPE_SD) ? "SD" :
												((DbSvc_GetVideoType(&stSvcInfo) == eDxVIDEO_TYPE_HD) ? "HD" : "Unkown"));
	HxLOG_Print("\tCAS Type : %d\n", DbSvc_GetCasType(&stSvcInfo));
	HxLOG_Print("\tVideo PID : 0x%X, Codec : 0x%X\n", DbSvc_GetVideoPid(&stSvcInfo), DbSvc_GetVideoCodec(&stSvcInfo));
	HxLOG_Print("\tAudio PID : 0x%X, Codec : 0x%X\n", DbSvc_GetAudioPid(&stSvcInfo), DbSvc_GetAudioCodec(&stSvcInfo));
	HxLOG_Print("\tAux PID : 0x%X, Codec : 0x%X\n", DbSvc_GetAudioAuxPid(&stSvcInfo), DbSvc_GetAudioAuxCodec(&stSvcInfo));
	HxLOG_Print("\tDolby PID : 0x%X, Dobly Flag : 0x%X\n", DbSvc_GetDolbyPid(&stSvcInfo), DbSvc_GetDolbyFlag(&stSvcInfo));
	HxLOG_Print("\tPCR PID : 0x%X\n", DbSvc_GetPcrPid(&stSvcInfo));
	HxLOG_Print("\tTeletext PID : 0x%X\n", DbSvc_GetTtxPid(&stSvcInfo));
	HxLOG_Print("\tSubtitle Index : %d\n", DbSvc_GetSubttlIdx(&stSvcInfo));
	HxLOG_Print("\tStereo Config : %d\n", DbSvc_GetStereoSelect(&stSvcInfo));
	HxLOG_Print("\t=====================================================\n");

	/* Common pstContext flags 초기화 */
	_mgr_LiveView_base_ResetCommonContextFlags(pstContext, hSvc);

	/* Master session만 초기화 하는 flags */
	pstContext->ulReplaceMessage = (HUINT32)-1; /* Replacement service event message 백업 invalidate */
	pstContext->bSiDrmValid = FALSE;

/* -------------------------------- Context 초기화 ------------------------------------ */

	/* Ch service stop(Async) */
	/* context 초기화 전에 stop하던 것을 초기화 이후에 stop하도록 수정  */
	/* context 초기화전에 ch stop을 하면 hSvc로 svcInfo를 가져오기 전에 stop msg받고 다른 처리를 하는 경우가 발생한다.. */
	HxLOG_Print ("[AP:LiveView(%d)] \t=> Going to eLiveViewState_CH_STOP state\n",ulActionId);
#if defined(CONFIG_3RD_MHEG)
	pstContext->eAppState = eLiveViewState_CH_STOP;
#else
	_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_CH_STOP);
#endif

	hErr = SVC_CH_StopTune(pstContext->hAction, pstContext->tsInfo.usUniqueId, eAsyncMode);
	if (hErr)
	{
		HxLOG_Error("SVC_CH_StopTune ret :0x%X\n",hErr);
	}

	pstContext->bBlockUiStateNoti = FALSE;

	/* 서비스 변경 공지 */
	_mgr_LiveView_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_SVC_CHANGED, pstContext->hAction, pstContext->hSvc, tuningMode, 0);

	/* Create radio background image if this is radio channel */
	_mgr_LiveView_base_CheckRadioBackground(pstContext);

	/* 두 값은 초기화 되어야 한다.*/
	pstContext->bCasOk = FALSE;
	pstContext->nCasEvt = 0;


	/* Front에 채널 정보 (채널 번호/채널명등) display */
	_mgr_LiveView_base_SetFrontPanel(pstContext);

	return ERR_OK;
}


HERROR _mgr_LiveView_base_StartSuppleSession(XmgrLiveView_Context_t *pstContext, Handle_t hSuppleSvc, MgrLiveView_SessionType_t sessionType, MgrLive_TuningMode_t tuningMode)
{
	HUINT32				ulActionId = pstContext->ulActionId;
	HERROR				hErr;
	DbSvc_Info_t			stSvcInfo;

	HxLOG_Info ("[AP:LiveView(%d)] _mgr_LiveView_base_StartSuppleSession()\n",ulActionId);

	if(pstContext->sessionType == eLiveViewSession_NORMAL)
	{ /* 현재 normal session으로 돌고 있는 경우에만 현재의 hSvc를 backup */
		pstContext->hMasterSvc = pstContext->hSvc;
		pstContext->stMasterSvcInfo = pstContext->stSvcInfo;
	}
	else
	{
		/* supplementary -> supplementary 의 경우에는 backup하면 안됨. */
	}

	/* Session version 증가 */
	pstContext->hAction = SVC_PIPE_IncreaseActionVersion(ulActionId);

	hErr = DB_SVC_GetTsInfoByServiceHandle(hSuppleSvc, &pstContext->tsInfo);
	if (hErr)
	{
		HxLOG_Error("DB_SVC_GetTsInfoByServiceHandle ret :0x%X\n",hErr);
	}

	/* Ch service stop(Async) */
	hErr = SVC_CH_StopTune(pstContext->hAction, pstContext->tsInfo.usUniqueId, eAsyncMode);
	if (hErr)
	{
		HxLOG_Error("SVC_CH_StopTune ret :0x%X\n",hErr);
	}

	/* WTV app state 초기화. 동작중의 state 변경과 다르므로 _mgr_LiveView_base_SetAppState() 사용하지 말것 !!! */
	HxLOG_Print("[AP:LiveView(%d)] \t=> Going to eLiveViewState_CH_STOP state\n",ulActionId);
	_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_CH_STOP);

/* +++++++++++++++++++++++++++ Context 초기화 ++++++++++++++++++++++++++++ */
#if defined(CONFIG_3RD_MHEG)
	pstContext->eAppState = eLiveViewState_CH_STOP;
#else
	_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_CH_STOP);
#endif

	pstContext->sessionType = sessionType;

	/* 새 service info update */
	HxSTD_memset(&stSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(hSuppleSvc, &stSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", hSuppleSvc);
	}

	pstContext->stSvcInfo = stSvcInfo;

	HxLOG_Print("\t============= Start Supplement Service for [%s] ===========\n", _mgr_LiveView_base_GetSessionTypeStr(sessionType));
	HxLOG_Print("\tService Name : %s\n", DbSvc_GetName(&stSvcInfo));
	HxLOG_Print("\tService Type : %s\n", (DbSvc_GetSvcType(&stSvcInfo) == eDxSVC_TYPE_TV) ? "TV" :
												((DbSvc_GetSvcType(&stSvcInfo) == eDxSVC_TYPE_RADIO) ? "Radio" : "Unkown"));
	HxLOG_Print("\tVideo Type : %s\n", (DbSvc_GetVideoType(&stSvcInfo) == eDxVIDEO_TYPE_SD) ? "SD" :
												((DbSvc_GetVideoType(&stSvcInfo) == eDxVIDEO_TYPE_HD) ? "HD" : "Unkown"));
	HxLOG_Print("\tCAS Type : %d\n", DbSvc_GetCasType(&stSvcInfo));
	HxLOG_Print("\tVideo PID : 0x%X, Codec : 0x%X\n", DbSvc_GetVideoPid(&stSvcInfo), DbSvc_GetVideoCodec(&stSvcInfo));
	HxLOG_Print("\tAudio PID : 0x%X, Codec : 0x%X\n", DbSvc_GetAudioPid(&stSvcInfo), DbSvc_GetAudioCodec(&stSvcInfo));
	HxLOG_Print("\tAux PID : 0x%X, Codec : 0x%X\n", DbSvc_GetAudioAuxPid(&stSvcInfo), DbSvc_GetAudioAuxCodec(&stSvcInfo));
	HxLOG_Print("\tDolby PID : 0x%X, Dobly Flag : 0x%X\n", DbSvc_GetDolbyPid(&stSvcInfo), DbSvc_GetDolbyFlag(&stSvcInfo));
	HxLOG_Print("\tPCR PID : 0x%X\n", DbSvc_GetPcrPid(&stSvcInfo));
	HxLOG_Print("\tTeletext PID : 0x%X\n", DbSvc_GetTtxPid(&stSvcInfo));
	HxLOG_Print("\tSubtitle Index : %d\n", DbSvc_GetSubttlIdx(&stSvcInfo));
	HxLOG_Print("\tStereo Config : %d\n", DbSvc_GetStereoSelect(&stSvcInfo));
	HxLOG_Print("\t=========================================================\n");

	/* Common pstContext flags 초기화 */
	tuningMode |= eKEEP_PINPASS; /* Supple session에서는 pin pass flag 유지해야 한다. */
	_mgr_LiveView_base_ResetCommonContextFlags(pstContext, hSuppleSvc);


/* -------------------------------- Context 초기화 ------------------------------------ */
#if defined(CONFIG_3RD_MHEG)
	// SDT Running Status 가 Change 된 경우에 Xmgr_mheg_base.c 의 MhegAction이 release 되기 때문에 새로 생성해 줘야 됨. 우선은 temporary 코드임.
	/* 서비스 변경 공지 */
	_mgr_LiveView_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_SVC_CHANGED, pstContext->hAction, hSuppleSvc, tuningMode, 0);
#endif

	/* Create radio background image if this is radio channel */
	_mgr_LiveView_base_CheckRadioBackground(pstContext);

	/* 서비스 변경 공지 */
	//_mgr_LiveView_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_SVC_CHANGED, pstContext->hAction, hSuppleSvc, tuningMode, 0);

	return ERR_OK;
}

STATIC HERROR _mgr_LiveView_base_StartMwServices (XmgrLiveView_Context_t *pstContext, HBOOL bNeedChTune)
{
	HUINT32						ulIndex;
	DxTuneParam_t				stTuneInfo;
	XmgrLive_MwSvcExecPhase_e	eCurrMwSvcPhase = eLiveMwSvcExecPhase_None;
	HERROR						hErr = ERR_FAIL;

	HxLOG_Print("hAction(0x%X) avDecId(%d) dmxId(%d))\n",
				pstContext->hAction, pstContext->stRscInfo.astRscItem[eRxRSCTYPE_AV_DEC].ulResourceId, pstContext->stRscInfo.astRscItem[eRxRSCTYPE_DEMUX].ulResourceId);

	if (bNeedChTune != FALSE)
	{
		//HxLOG_Print("[%s:%d] ch tune ~~ \n", __FUNCTION__, __LINE__);
		hErr = OTL_CONV_DbTuningInfo2DxTuneParam((const DbSvc_TsInfo_t *)&(pstContext->tsInfo), &stTuneInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("OTL_CONV_DbTuningInfo2DxTuneParam fail\n");
			return ERR_FAIL;
		}

		hErr = SVC_CH_StartTune (pstContext->hAction, &(pstContext->stRscInfo), pstContext->tsInfo.usUniqueId, &stTuneInfo);
		if (ERR_OK == hErr)
		{
			pstContext->aeServicesState[eLiveMwSvcType_Channel] = eLiveMwSvcState_ReqStart;
		}
		else
		{
			HxLOG_Error ("SVC_CH_StartTune fail\n");
			return ERR_FAIL;
		}
	}

	switch (pstContext->eAppState)
	{
	case eLiveViewState_SVC_STOP:
	case eLiveViewState_SVC_LOCKED:
	case eLiveViewState_NO_SIGNAL:
		eCurrMwSvcPhase = eLiveMwSvcExecPhase_AfterChannelTuned;
		break;

	case eLiveViewState_AV:
		eCurrMwSvcPhase = eLiveMwSvcExecPhase_AfterAvOut;
		break;

	case eLiveViewState_EMPTY_SVC:
	case eLiveViewState_PMT_PID_CHANGED:
		eCurrMwSvcPhase = eLiveMwSvcExecPhase_AfterPmtReceived;
		break;

	// CAS가 잡고 있어도 SI는 시작 해야 하는데......
	case eLiveViewState_CAS:
		eCurrMwSvcPhase = eLiveMwSvcExecPhase_AfterChannelTuned;
		break;

	default:
		HxLOG_Error ("cannot start MW services in the current Live state (%s)\n", _mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
		return ERR_FAIL;
	}

	if( (pstContext->bPmtValid == TRUE) && (pstContext->eAppState >= eLiveViewState_PID_START ) )
	{
		eCurrMwSvcPhase = eLiveMwSvcExecPhase_AfterPmtReceived;
	}

	for (ulIndex = 0; ulIndex < eLiveMwSvcType_MAX; ulIndex++)
	{
		/*
		HxLOG_Print("[%s, %d] idx(%d) \n :: aeServicesStartPhase(%s) eCurrMwSvcPhase(%s)\n\n",
						__FUNCTION__, __LINE__, ulIndex,
						_mgr_LiveView_base_GetMwSvcExecPhaseStr(pstContext->aeServicesStartPhase[ulIndex]),
						_mgr_LiveView_base_GetMwSvcExecPhaseStr(eCurrMwSvcPhase));
		*/

		if ((eLiveMwSvcExecPhase_None != pstContext->aeServicesStartPhase[ulIndex])
			&& (eCurrMwSvcPhase >= pstContext->aeServicesStartPhase[ulIndex]))
		{
			/*
			HxLOG_Print("===[%s, %d] idx(%d) aeServicesState(%s) \n",
							__FUNCTION__, __LINE__, ulIndex,
							_mgr_LiveView_base_GetMwSvcStateStr(pstContext->aeServicesState[ulIndex]));
			*/

			if ((eLiveMwSvcState_ReqStart != pstContext->aeServicesState[ulIndex]) &&
				(eLiveMwSvcState_Started != pstContext->aeServicesState[ulIndex]))
			{
				switch (ulIndex)
				{
				case eLiveMwSvcType_Si:
					HxLOG_Print("=== idx(%d) eLiveMwSvcType_Si  start !\n", ulIndex);
					hErr = _mgr_LiveView_startSiService (pstContext);
					break;

				case eLiveMwSvcType_Cas:
					HxLOG_Print("=== idx(%d) eLiveMwSvcType_Cas start !\n", ulIndex);
					hErr = _mgr_LiveView_startCasService (pstContext);
					break;

				case eLiveMwSvcType_Av:
					HxLOG_Print("=== idx(%d) eLiveMwSvcType_Av start !\n", ulIndex);
					hErr = _mgr_LiveView_startAvService (pstContext);
					break;

				case eLiveMwSvcType_Transcoder:
					HxLOG_Print("=== idx(%d) eLiveMwSvcType_Transcoder start !\n", ulIndex);
					hErr = _mgr_LiveView_startTranscoderService (pstContext);
					break;

				default:
					continue;
				}

				if (ERR_OK == hErr)
				{
					pstContext->aeServicesState[ulIndex] = eLiveMwSvcState_ReqStart;
				}
				else
				{
					HxLOG_Error ("MW Service Start Failed: hAction(0x%08x), hSvc(0x%08x), MwSvcType(%d)\n", pstContext->hAction, pstContext->hSvc, ulIndex);
				}
			}
		}
	}

	// EPG, TTX 등의 complementary들을 PMT 수신 이후 or av 나온 이후에 start 시키자
	if ((eCurrMwSvcPhase == eLiveMwSvcExecPhase_AfterAvOut)
		|| (eCurrMwSvcPhase == eLiveMwSvcExecPhase_AfterPmtReceived))
	{
		_mgr_LiveView_base_StartComplementaries(pstContext);
	}

	_mgr_LiveView_base_ShowProgram(pstContext);

	return ERR_OK;
}

STATIC HERROR _mgr_LiveView_base_StopMwServices (XmgrLiveView_Context_t *pstContext, HBOOL bForced, ApiSyncMode_t eSync)
{
	HUINT32				ulIndex;
	HERROR				hErr;

	SvcEpg_EpgKind_e			eEpgKind = eEPG_KIND_DVB;

	for (ulIndex = 0; ulIndex < eLiveMwSvcType_MAX; ulIndex++)
	{
		if (eLiveMwSvcExecPhase_None != pstContext->aeServicesStartPhase[ulIndex])
		{
			if ((TRUE == bForced) ||
				(eLiveMwSvcState_ReqStart == pstContext->aeServicesState[ulIndex]) ||
				(eLiveMwSvcState_Started == pstContext->aeServicesState[ulIndex]))
			{
				switch (ulIndex)
				{
				case eLiveMwSvcType_Si:
					hErr = SVC_SI_StopLive (pstContext->hAction, pstContext->hSvc, (eSyncMode == eSync));
					if (hErr)
					{
						HxLOG_Error("Error ! SVC_SI_StopLive()\n");
					}

					if (XMGR_LiveComp_BASE_StopEpg(eEpgKind, eEPG_EVT_KIND_ALL, pstContext->hAction) != ERR_OK)
					{
						HxLOG_Error("Error ! XMGR_LiveComp_BASE_StopEpg()\n");
					}
					break;

				case eLiveMwSvcType_Cas:
					hErr = MGR_CAS_StopService(pstContext->hAction, pstContext->hSvc, eSync);
					break;

				case eLiveMwSvcType_Av:
					hErr = SVC_AV_StopService (pstContext->ulActionId, eSync);
					break;

#if defined(CONFIG_MW_MM_TRANSCODING)
				case eLiveMwSvcType_Transcoder:
					/**
					  * author : hsseo
					  * note : ( TBD : Transcoder Start Status를 DI Evt 로 응답 받아 설정 하도록 구현 해야함.)
					  *		 OxSVC_TranscoderStartService() 후출 직후 아래 Stop이 호출됨.
					  *		 이를 막기 위해 State를 보고 Stop 명령 가도록 함.
					**/
					if( pstContext->eAppState >= eLiveViewState_PID_START )
					{
						hErr = SVC_TRANSCODER_StopService (pstContext->hAction, eSyncMode);
						break;
					}
#endif
				default:
					continue;
				}

				if (ERR_OK == hErr)
				{
					pstContext->aeServicesState[ulIndex] = (eSyncMode == eSync) ? eLiveMwSvcState_Stopped : eLiveMwSvcState_ReqStop;

					/**
					  *	av는 sync, async에 상관없이 stop을 호출하면 stopped로 간주한다. (EVT_AV_STOPPED 메시지 삭제됨)
					  *	그 외 다른 mw svc type도 이렇게 처리될 수 있다
					**/
					if (ulIndex == eLiveMwSvcType_Av)
					{
						pstContext->aeServicesState[ulIndex] = eLiveMwSvcState_Stopped;
					}
				}
				else
				{
					HxLOG_Error ("MW Service Stop Failed: hAction(0x%08x) MwSvcType(%d)\n", pstContext->hAction, ulIndex);
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR _mgr_LiveView_base_StopMwService(XmgrLiveView_Context_t *pstContext, XmgrLive_MwSvcType_e eMwSvcType, ApiSyncMode_t eSync)
{
	HERROR				 hErr = ERR_FAIL;
	SvcEpg_EpgKind_e	 eEpgKind = eEPG_KIND_DVB;

	HxLOG_Print ("eMwSvcType(%s)  \n", _mgr_LiveView_base_GetMwSvcTypeStr(eMwSvcType));
	HxLOG_Print ("eSync(%d)  aeServicesStartPhase(%d) aeServicesState(%d) \n", eSync,
																			pstContext->aeServicesStartPhase[eMwSvcType],
																			pstContext->aeServicesState[eMwSvcType]);

	if (eLiveMwSvcExecPhase_None != pstContext->aeServicesStartPhase[eMwSvcType])
	{
		if ((eLiveMwSvcState_ReqStart == pstContext->aeServicesState[eMwSvcType])
			||(eLiveMwSvcState_Started == pstContext->aeServicesState[eMwSvcType]))
		{
			switch (eMwSvcType)
			{
				case eLiveMwSvcType_Si:
					hErr = SVC_SI_StopLive (pstContext->hAction, pstContext->hSvc, (eSyncMode == eSync));
					if (hErr != ERR_OK)
					{
						HxLOG_Error("Error ! SVC_SI_StopLive()\n");
					}

					if (XMGR_LiveComp_BASE_StopEpg(eEpgKind, eEPG_EVT_KIND_ALL, pstContext->hAction) != ERR_OK)
					{
						HxLOG_Error("Error ! XMGR_LiveComp_BASE_StopEpg() failed..! \n");
					}
					break;

				case eLiveMwSvcType_Cas:
					hErr = MGR_CAS_StopService(pstContext->hAction, pstContext->hSvc, eSync);
					break;

				case eLiveMwSvcType_Av:
					hErr = SVC_AV_StopService (pstContext->ulActionId, eSync);
					break;

#if defined(CONFIG_MW_MM_TRANSCODING)
				case eLiveMwSvcType_Transcoder:
					hErr = SVC_TRANSCODER_StopService (pstContext->hAction, eSyncMode);
					break;
#endif

				default:
					HxLOG_Error ("invalid eMwSvcType(%d)\n", eMwSvcType);
					break;
			}

			if (ERR_OK == hErr)
			{
				pstContext->aeServicesState[eMwSvcType] = (eSyncMode == eSync) ? eLiveMwSvcState_Stopped : eLiveMwSvcState_ReqStop;

				/**
				  *	av는 sync, async에 상관없이 stop을 호출하면 stopped로 간주한다. (EVT_AV_STOPPED 메시지 삭제됨)
				  *	그 외 다른 mw svc type도 이렇게 처리될 수 있다
				**/
				if (eMwSvcType == eLiveMwSvcType_Av)
				{
					pstContext->aeServicesState[eMwSvcType] = eLiveMwSvcState_Stopped;
				}
			}
			else
			{
				HxLOG_Error ("MW Service Stop Failed: hAction(0x%08x), hSvc(0x%08x), MwSvcType(%d)\n", pstContext->hAction, pstContext->hSvc, eMwSvcType);
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR _mgr_LiveView_base_CheckAllMwServiceStopCompleted (XmgrLiveView_Context_t *pstContext)
{
	HUINT32			 ulIndex;
	HERROR			 hErr;

	for (ulIndex = 0; ulIndex < eLiveMwSvcType_MAX; ulIndex++)
	{
		if (eLiveMwSvcExecPhase_None != pstContext->aeServicesStartPhase[ulIndex])
		{
			if (eLiveMwSvcState_ReqStop == pstContext->aeServicesState[ulIndex])
			{
				HxLOG_Print("idx(%d) eLiveMwSvcState_ReqStop \n", ulIndex);
				return ERR_OK;
			}
		}
	}

#if 0
	if (_mgr_LiveView_base_GetMasterSvcInfoPtr(pstContext)->bLocked == TRUE)
	{
		_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_SVC_LOCKED);
	}
	else
#endif
	{
		hErr = _mgr_LiveView_base_StartMwServices (pstContext, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("XMGR_LIVE_BASE_StartMwServices err:\n");
			return ERR_FAIL;
		}
	}


	return ERR_OK;
}

void _mgr_LiveView_base_ChangePid_SvcDb(XmgrLiveView_Context_t *pstContext)
{
	HUINT32			ulActionId = pstContext->ulActionId;
	HUINT16			usCompareVideoPid, usCompareAudioPid, usCompareDolbyPid, usComparePcrPid;
	DxVideoCodec_e	eCompareVideoCodec;
	DxAudioCodec_e	eCompareAudioCodec;
	DxAudioCodec_e	eCompareDolbyCodec;
	HBOOL			bAudioChanged = FALSE;
	HBOOL			bVideoChanged = FALSE;
	HBOOL			bPcrChanged = FALSE;
	HBOOL			bCompareDolbyFlag = FALSE;
	DbSvc_Info_t		newSvcInfo;
	HERROR			hErr;

	HxLOG_Info ("[AP:LiveView(%d)] _mgr_LiveView_base_ChangePid_SvcDb()\n",ulActionId);

	/* Service info를 다시 찾아 봄. PID관련이므로 actual service사용. */
	HxSTD_memset(&newSvcInfo, 0, sizeof(DbSvc_Info_t));
	hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &newSvcInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
	}

	/********************** Video PID 변경 검사 *********************/
	if(pstContext->usOrgVideoPid != 0)
	{ /* Temp PID 모드에서는 backup된 original PID로 검사해야 함. */
		HxLOG_Print(HxANSI_COLOR_GREEN("[AP:LiveView(%d)] \t=> Video is temp pid mode. Check with backup PID.\n"),ulActionId);
		usCompareVideoPid = pstContext->usOrgVideoPid;
	}
	else
	{
		HxLOG_Print ("[AP:LiveView(%d)] \t=> Video is DB pid mode. Check with local service info PID's.\n",ulActionId);
		usCompareVideoPid = DbSvc_GetVideoPid(&pstContext->stSvcInfo);
	}

	if(pstContext->eOrgVideoCodec != eDxVIDEO_CODEC_MAX_NUM)
	{ /* Temp PID 모드에서는 backup된 original codec으로 검사해야 함. */
		HxLOG_Print(HxANSI_COLOR_GREEN("[AP:LiveView(%d)] \t=> Video codec is temp pid mode. Check with backup codec.\n"),ulActionId);
		eCompareVideoCodec = pstContext->eOrgVideoCodec;
	}
	else
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> Video codec is DB pid mode. Check with local service info codec.\n",ulActionId);
		eCompareVideoCodec = DbSvc_GetVideoCodec(&pstContext->stSvcInfo);
	}

	if( (usCompareVideoPid != DbSvc_GetVideoPid(&newSvcInfo)) || (eCompareVideoCodec != DbSvc_GetVideoCodec(&newSvcInfo)) )
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> VIDEO PID [0x%X]->[0x%X]\n",ulActionId, usCompareVideoPid, DbSvc_GetVideoPid(&newSvcInfo));
		HxLOG_Print("[AP:LiveView(%d)] \t=> VIDEO Codec [0x%X]->[0x%X]\n",ulActionId, eCompareVideoCodec, DbSvc_GetVideoCodec(&newSvcInfo));

		bVideoChanged = TRUE;

		DbSvc_SetVideoPid(&pstContext->stSvcInfo, DbSvc_GetVideoPid(&newSvcInfo));
		DbSvc_SetVideoCodec(&pstContext->stSvcInfo, DbSvc_GetVideoCodec(&newSvcInfo));

		pstContext->usOrgVideoPid = 0; /* Service DB의 PID로 다시 돌아갔으니 backup PID는 clear해야 함. */
		pstContext->eOrgVideoCodec = eDxVIDEO_CODEC_MAX_NUM;
	}

	/********************** Audio PID 변경 검사 *********************/
	if(pstContext->usOrgAudioPid != 0)
	{
		HxLOG_Print(HxANSI_COLOR_GREEN("[AP:LiveView(%d)] \t=> Audio is temp pid mode. Check with backup PID.\n"),ulActionId);
		usCompareAudioPid = pstContext->usOrgAudioPid;
	}
	else
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> Audio is DB pid mode. Check with local service info PID's.\n",ulActionId);
		usCompareAudioPid = DbSvc_GetAudioPid(&pstContext->stSvcInfo);
	}

	if(pstContext->eOrgAudioCodec != eDxAUDIO_CODEC_MAX_NUM)
	{ /* Temp PID 모드에서는 backup된 original codec으로 검사해야 함. */
		HxLOG_Print(HxANSI_COLOR_GREEN("[AP:LiveView(%d)] \t=> Audio codec is temp pid mode. Check with backup codec.\n"),ulActionId);
		eCompareAudioCodec = pstContext->eOrgAudioCodec;
	}
	else
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> Audio codec is DB pid mode. Check with local service info codec.\n",ulActionId);
		eCompareAudioCodec = DbSvc_GetAudioCodec(&pstContext->stSvcInfo);
	}

	if(pstContext->usOrgDolbyPid != 0)
	{
		HxLOG_Print(HxANSI_COLOR_GREEN("[AP:LiveView(%d)] \t=> Dolby is temp pid mode. Check with backup PID.\n"),ulActionId);
		usCompareDolbyPid = pstContext->usOrgDolbyPid;
	}
	else
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> Dolby is DB pid mode. Check with local service info PID's.\n",ulActionId);
		usCompareDolbyPid = DbSvc_GetDolbyPid(&pstContext->stSvcInfo);
	}

	/* Dolby PID가 존재했다면 Dolby Flag = TRUE */
	if (usCompareDolbyPid != PID_NULL)
	{
		bCompareDolbyFlag = DbSvc_GetDolbyFlag(&pstContext->stSvcInfo);
	}

	if(pstContext->eOrgDolbyCodec != eDxAUDIO_CODEC_MAX_NUM)
	{ /* Temp PID 모드에서는 backup된 dolby original codec으로 검사해야 함. */
		HxLOG_Print(HxANSI_COLOR_GREEN("[AP:LiveView(%d)] \t=> Dolby codec is temp pid mode. Check with backup codec.\n"),ulActionId);
		eCompareDolbyCodec = pstContext->eOrgDolbyCodec;
	}
	else
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> Dolby codec is DB pid mode. Check with local service info codec.\n",ulActionId);
		eCompareDolbyCodec = DbSvc_GetDolbyCodec(&pstContext->stSvcInfo);
	}

	if( (usCompareAudioPid != DbSvc_GetAudioPid(&newSvcInfo)) ||												// Audio PID
		(usCompareDolbyPid != DbSvc_GetDolbyPid(&newSvcInfo)) ||											// Dolby PID
		(DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo) != DbSvc_GetAudioAuxPid(&newSvcInfo)) ||				// Audio AUX PID
		(eCompareAudioCodec != DbSvc_GetAudioCodec(&newSvcInfo)) ||											// Audio Codec
		(eCompareDolbyCodec != DbSvc_GetDolbyCodec(&newSvcInfo)) ||											// Dolby Codec
		(DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo) != DbSvc_GetAudioAuxCodec(&newSvcInfo)) ||			// Audio AUX Codec
		(bCompareDolbyFlag != DbSvc_GetDolbyFlag(&newSvcInfo)) ||											// Dolby Flag
		(DbSvc_GetStereoSelect(&pstContext->stSvcInfo) != DbSvc_GetStereoSelect(&newSvcInfo)) ||			// Audio Stereo Select
		(DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo) != DbSvc_GetDualmonoSelect(&newSvcInfo)) 			// Audio Dualmono Select
		)
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> Main PID [0x%X]->[0x%X]\n",ulActionId, usCompareAudioPid, DbSvc_GetAudioPid(&newSvcInfo));
		HxLOG_Print("[AP:LiveView(%d)] \t=> Aux PID [0x%X]->[0x%X]\n", ulActionId, DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo), DbSvc_GetAudioAuxPid(&newSvcInfo));
		HxLOG_Print("[AP:LiveView(%d)] \t=> Dolby PID [0x%X]->[0x%X]\n", ulActionId,usCompareDolbyPid, DbSvc_GetDolbyPid(&newSvcInfo));
		HxLOG_Print("[AP:LiveView(%d)] \t=> Main Codec [0x%X]->[0x%X]\n",ulActionId, eCompareAudioCodec, DbSvc_GetAudioCodec(&newSvcInfo));
		HxLOG_Print("[AP:LiveView(%d)] \t=> Aux Codec [0x%X]->[0x%X]\n",ulActionId, DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo), DbSvc_GetAudioAuxCodec(&newSvcInfo));
		HxLOG_Print("[AP:LiveView(%d)] \t=> Dolby Codec [0x%X]->[0x%X]\n", ulActionId,eCompareDolbyCodec, DbSvc_GetDolbyCodec(&newSvcInfo));
		HxLOG_Print("[AP:LiveView(%d)] \t=> Dolby Flag [0x%X]->[0x%X]\n", ulActionId,bCompareDolbyFlag, DbSvc_GetDolbyFlag(&newSvcInfo));
		HxLOG_Print("[AP:LiveView(%d)] \t=> Sound mode [0x%X]->[0x%X]\n",ulActionId, DbSvc_GetStereoSelect(&pstContext->stSvcInfo), DbSvc_GetStereoSelect(&newSvcInfo));
		HxLOG_Print("[AP:LiveView(%d)] \t=> Dualmono Select [0x%X]->[0x%X]\n",ulActionId, DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo), DbSvc_GetDualmonoSelect(&newSvcInfo));

		bAudioChanged = TRUE;

		DbSvc_SetAudioPid(&pstContext->stSvcInfo, DbSvc_GetAudioPid(&newSvcInfo));
		DbSvc_SetAudioAuxPid(&pstContext->stSvcInfo, DbSvc_GetAudioAuxPid(&newSvcInfo));
		DbSvc_SetDolbyPid(&pstContext->stSvcInfo, DbSvc_GetDolbyPid(&newSvcInfo));
		DbSvc_SetAudioCodec(&pstContext->stSvcInfo, DbSvc_GetAudioCodec(&newSvcInfo));
		DbSvc_SetDolbyCodec(&pstContext->stSvcInfo, DbSvc_GetDolbyCodec(&newSvcInfo));
		DbSvc_SetAudioAuxCodec(&pstContext->stSvcInfo, DbSvc_GetAudioAuxCodec(&newSvcInfo));
		DbSvc_SetDolbyFlag(&pstContext->stSvcInfo, DbSvc_GetDolbyFlag(&newSvcInfo));
		DbSvc_SetStereoSelect(&pstContext->stSvcInfo, DbSvc_GetStereoSelect(&newSvcInfo));
		DbSvc_SetDualmonoSelect(&pstContext->stSvcInfo, DbSvc_GetDualmonoSelect(&newSvcInfo));
		DbSvc_SetAudioLanguage(&pstContext->stSvcInfo, DbSvc_GetAudioLanguage(&newSvcInfo));
		DbSvc_SetAudUserSelected(&pstContext->stSvcInfo, DbSvc_GetAudUserSelected(&newSvcInfo));

		pstContext->usOrgAudioPid = 0; /* Service DB의 PID로 다시 돌아갔으니 backup PID는 clear해야 함. */
		pstContext->usOrgDolbyPid = 0;
		pstContext->eOrgAudioCodec = eDxAUDIO_CODEC_MAX_NUM;
		pstContext->eOrgDolbyCodec = eDxAUDIO_CODEC_MAX_NUM;
	}

	/********************** PCR PID 변경 검사 *********************/
	if(pstContext->usOrgPcrPid != 0)
	{
		HxLOG_Print ("[AP:LiveView(%d)] \t=> PCR is temp pid mode. Check with backup PID.\n",ulActionId);
		usComparePcrPid = pstContext->usOrgPcrPid;
	}
	else
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> PCR is DB pid mode. Check with local service info PID's.\n",ulActionId);
		usComparePcrPid = DbSvc_GetPcrPid(&pstContext->stSvcInfo);
	}

	if(usComparePcrPid != DbSvc_GetPcrPid(&newSvcInfo))
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> PCR PID [0x%X]->[0x%X]\n",ulActionId, usComparePcrPid, DbSvc_GetPcrPid(&newSvcInfo));

		bPcrChanged = TRUE;

		DbSvc_SetPcrPid(&pstContext->stSvcInfo, DbSvc_GetPcrPid(&newSvcInfo));
		pstContext->usOrgPcrPid = 0; /* Service DB의 PID로 다시 돌아갔으니 backup PID는 clear해야 함. */
	}

	/********************** AV PID or user setting 변경 검사 *********************/
	if( (bAudioChanged == TRUE) || (bVideoChanged == TRUE)  || (bPcrChanged == TRUE) )
	{
		if(pstContext->eAppState > eLiveViewState_PID_START)
		{ /* PID running states였다면 AV를 다시 시작한다. */

			/* CAS에 PID를 다시 셋팅해야 한다. */
			hErr = SVC_CAS_SetPid(pstContext->hAction,
									eCAS_ActionType_Live,
									DbSvc_GetVideoPid(&pstContext->stSvcInfo),
									DbSvc_GetAudioPid(&pstContext->stSvcInfo),
									DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo),
									DbSvc_GetDolbyPid(&pstContext->stSvcInfo),
									PID_CURRENT,
									PID_CURRENT);
			if (hErr)
			{
				HxLOG_Error("SVC_CAS_SetPid ret :0x%X\n",hErr);
			}

			if (DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo) != eDxDUALMONO_SELECT_UNKNOWN)
			{
#if !defined(SvcAv_START_SERVICE_NEW)
				hErr = SVC_AV_SetStereoSelect(pstContext->ulActionId, eDxSTEREO_FORMAT_DUALMONO);
				if (hErr)
				{
					HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
				}
#endif
				hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo));
				if (hErr)
				{
					HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
				}
			}

			HxLOG_Print ("[AP:LiveView(%d)] \t=> Restart AV.\n",ulActionId);
#if defined(SvcAv_START_SERVICE_NEW)
			hErr = SVC_AV_ChangePid(pstContext->ulActionId,
									_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioPid(&pstContext->stSvcInfo)),
									_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo)),
									_mgr_LiveView_base_GetVideoPid(pstContext, DbSvc_GetVideoPid(&pstContext->stSvcInfo)),
									DbSvc_GetPcrPid(&pstContext->stSvcInfo),
									DbSvc_GetAudioCodec(&pstContext->stSvcInfo),
									DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo),
									DbSvc_GetVideoCodec(&pstContext->stSvcInfo));
#else
			hErr = SVC_AV_ChangePid(pstContext->ulActionId,
									_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioPid(&pstContext->stSvcInfo)),
									_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo)),
									_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetDolbyPid(&pstContext->stSvcInfo)),
									_mgr_LiveView_base_GetVideoPid(pstContext, DbSvc_GetVideoPid(&pstContext->stSvcInfo)),
									DbSvc_GetPcrPid(&pstContext->stSvcInfo),
									DbSvc_GetAudioCodec(&pstContext->stSvcInfo),
									DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo),
									DbSvc_GetDolbyCodec(&pstContext->stSvcInfo),
									DbSvc_GetVideoCodec(&pstContext->stSvcInfo),
									DbSvc_GetDolbyFlag(&pstContext->stSvcInfo),
									DbSvc_GetStereoSelect(&pstContext->stSvcInfo));
#endif
			if (hErr)
			{
				HxLOG_Error("SVC_AV_ChangePid ret :0x%X\n",hErr);
			}
		}
		else
		{ /* 다른 state에서는 변동없다. */
			HxLOG_Print ("[AP:LiveView(%d)] \t=> Not in PID running states. Don't restart AV.\n", ulActionId);
		}

		BUS_PostMessage(NULL, eMEVT_LIVE_NOTIFY_CURRENT_SVC_PID_CHANGED, pstContext->hAction, 0, 0, 0);
	}

	if(pstContext->eAppState == eLiveViewState_AV)
	{ /* AV state에서만 check 해야 함. PMT valid가 아니면 no AV 안그리기 때문에 PID 변경하고 상관없이 check해야 함.*/
		_mgr_LiveView_base_CheckAV(pstContext);
	}

	/* 혹시 video PID가 살아나거나 죽었을까봐 다시 한번 체크해 본다. */
	_mgr_LiveView_base_CheckRadioBackground(pstContext);
}

void _mgr_LiveView_base_MakeAvShow(XmgrLiveView_Context_t *pstContext)
{
	switch(pstContext->nCasShow)
	{
		case eCasShowType_None:
			pstContext->bAudioShow = FALSE;
			pstContext->bVideoShow = FALSE;
			break;

		case eCasShowType_AudioOnly:
			pstContext->bAudioShow = TRUE;
			pstContext->bVideoShow = FALSE;
			break;

		case eCasShowType_VideoOnly:
			pstContext->bAudioShow = FALSE;
			pstContext->bVideoShow = TRUE;
			break;

		case eCasShowType_Both:
			pstContext->bAudioShow = TRUE;
			pstContext->bVideoShow = TRUE;
			break;

		default:
			HxLOG_Critical("\n\n");
			pstContext->bAudioShow = FALSE;
			pstContext->bVideoShow = FALSE;
			break;
	}
}

STATIC void _mgr_LiveView_base_ShowProgram(XmgrLiveView_Context_t *pstContext)
{
	HBOOL				bPCService = DbSvc_GetParentalControlFlag(&pstContext->stSvcInfo);
	HUINT32				ulActionId = pstContext->ulActionId;
	HUINT32				ulParentalSetup = 0;
	XmgrLiveView_State_e		wtvAppState;

	HxLOG_Info("[AP:LiveView(%d)] _mgr_LiveView_base_ShowProgram()\n",ulActionId);
	/* 참고: WTV state diagram 보면 도움이 많이 됨. */

	HxLOG_Print ("[AP:LiveView(%d)] Rating Evaluation --------------------------\n \
					\t\t\t bPCService[%d]\n \
					\t\t\t nCasEvt[0x%x]\n \
					\t\t\t bSCPinCodeValid[%d]\n \
					\t\t\t eSvcOpType[%d]\n \
					\t\t\t ulParentalSetup[%d]\n \
					\t\t\t eCasType[%d]\n \
					\t\t\t nCasShow[%d]\n \
					\t\t\t bCasOk[%d]\n \
					\t\t\t nCasSystemId[%d]\n ",
					ulActionId,
					bPCService,
					pstContext->nCasEvt,
					pstContext->bSCPinCodeValid,
					DbSvc_GetOpType(&(pstContext->stSvcInfo)),
					ulParentalSetup,
					DbSvc_GetCasType(&(pstContext->stSvcInfo)),
					pstContext->nCasShow,
					pstContext->bCasOk,
					pstContext->nCasSystemId
					);

	wtvAppState = eLiveViewState_AV;
	if(pstContext->nCasEvt != 0)
	{ /* CAS check/fail event가 있으면 CAS state가 최종 state임. */
		HxLOG_Print ("[AP:LiveView(%d)] \t=> Final state is <%s>. But CAS event exists! Force to CAS state.\n",ulActionId,
											_mgr_LiveView_base_GetAppStateStr(wtvAppState));

		/* 최종 결정 state가 AV인 경우는 CAS가 원하는 show를 해준다. */
		HxLOG_Print("[AP:LiveView(%d)] \t=> Apply CasShow.\n",ulActionId);
		_mgr_LiveView_base_MakeAvShow(pstContext);

		if ((pstContext->bAudioShow == TRUE) || (pstContext->bVideoShow == TRUE))
		{
			// AV 중에 하나라도 보이는 것이 있다면 cas preview 상태로 보고, rating 적용을 위해 ui state는 ok로 올린다
			wtvAppState = eLiveViewState_CAS_PREVIEW;
		}
		else
		{
			wtvAppState = eLiveViewState_CAS;
		}
	}

	/* 새로 결정된 state로 이동 */
	_mgr_LiveView_base_SetAppState(pstContext, wtvAppState);
}

/* Teletext, subtitle, CC, EPG등 AV 이외에 곁다리들 시작. */
void _mgr_LiveView_base_StartComplementaries(XmgrLiveView_Context_t *pstContext)
{
	HUINT32				ulActionId = pstContext->ulActionId;
	HERROR				hErr;
	SvcEpg_EpgKind_e	eEpgKind = eEPG_KIND_DVB;
#if defined(CONFIG_MW_EPG_GATHERING_BARKER)
	DbSvc_TripleId_t 	stChannelInfo;
	HINT32				nOrgDeliType = eDxDELIVERY_TYPE_ALL;
	HBOOL 				bValid = FALSE;
#endif


	if (_mgr_LiveView_base_CheckComplementariesStartCondition(pstContext) != TRUE)
	{
		HxLOG_Print ("[AP:LiveView(%d)] \t=> support main view only  \n",ulActionId);
		return;
	}

#if defined(CONFIG_DEBUG)
	XMGR_LiveView_BASE_DBG_DumpContext(pstContext);
#endif

	/***
	  **		EPG Contents, PF, SCH
	***/
	hErr = XMGR_LiveComp_BASE_StartEpgContent(eEpgKind, pstContext->hAction, pstContext->hSvc);
	if (hErr)
	{
		HxLOG_Error("Error ! XMGR_LiveComp_BASE_StartEpgContent()\n");
	}

	hErr = XMGR_LiveComp_BASE_StartEpg(eEpgKind, eEPG_EVT_KIND_PF_ALL, pstContext->hAction, pstContext->hSvc, 0, 0, 0);
	if (hErr)
	{
		HxLOG_Error("Error ! XMGR_LiveComp_BASE_StartEpg()\n");
	}

#if defined(CONFIG_MW_EPG_GATHERING_BARKER)
	// CONFIG_MW_EPG_GATHERING_BARKER를 사용하는 모델이라도 Barker를 dynamic 하게 사용할 수 있다
	hErr = SVC_SI_HomeChannelGetChannelInfo(&stChannelInfo, &nOrgDeliType, &bValid);
	if((ERR_OK == hErr) && (bValid == TRUE))
	{
		HxLOG_Print("Epg gathering by barker channel\n");
	}
	else
	{
		hErr = XMGR_LiveComp_BASE_StartEpg(eEpgKind, eEPG_EVT_KIND_SCH_ALL, pstContext->hAction, pstContext->hSvc, 0, 0, 0);
		if (hErr)
		{
			HxLOG_Error("Error ! XMGR_LiveComp_BASE_StartEpg()\n");
		}
	}
#else
	hErr = XMGR_LiveComp_BASE_StartEpg(eEpgKind, eEPG_EVT_KIND_SCH_ALL, pstContext->hAction, pstContext->hSvc, 0, 0, 0);
	if (hErr)
	{
		HxLOG_Error("Error ! XMGR_LiveComp_BASE_StartEpg()\n");
	}
#endif


// TODO: subtitle도 이렇게 하면 좋을텐데...

	/***
	  **		Teletext
	***/


#if defined(CONFIG_MW_TELETEXT)
	XMGR_LiveComp_BASE_StartTeletext(pstContext->hAction, DbSvc_GetTtxPid(&pstContext->stSvcInfo));
#endif	//여기서 start teletext를 해주지 않으면 teletext 가 시작하지 않는 문제 발생하여 다시 코드 원복함.

	return;
}

void _mgr_LiveView_base_SetCopyProtection(XmgrLiveView_Context_t *pstContext)
{
	MgrCopyrightControl_OutputControlInfo_t	stCpInfo;
	HERROR					hError;

	HxLOG_Print("eAppState(%d) bSiDrmValid(%d) bCasDrmValid(%d)\n",
		pstContext->eAppState, pstContext->bSiDrmValid,pstContext->bCasDrmValid);

	if(pstContext->eAppState >= eLiveViewState_AV)
	{
		pstContext->drmInfo.bSiCc = pstContext->bSiDrmValid;
		pstContext->drmInfo.bCasCc = pstContext->bCasDrmValid;
		hError = MGR_COPYRIGHTCONTROL_GetCopyProtectInfo(FALSE,
												(const DxCopyrightControl_t *)&pstContext->drmInfo,
												&stCpInfo);
		if (hError != ERR_OK)
		{
			HxLOG_Error("AP_SPEC_GetCopyProtectFlag ret :0x%X\n",hError);
			return;
		}

		HxLOG_Print("bSetMacrovision[%d][%s]\n", stCpInfo.bSetMacrovision, OTL_ENUM2STR_Macrovision(stCpInfo.eMacrovision));
		HxLOG_Print("bSetCgmsMacrovision[%d][%s]\n", stCpInfo.bSetCgmsMacrovision, OTL_ENUM2STR_Macrovision(stCpInfo.eCgmsMacrovision));
		HxLOG_Print("bSetHdcp[%d][%d]\n", stCpInfo.bSetHdcp, stCpInfo.bHdcp);
		HxLOG_Print("bSetScmsCopyRight[%d][%s]\n", stCpInfo.bSetScmsCopyRight, OTL_ENUM2STR_CopyRight(stCpInfo.eScmsCopyRight));
		HxLOG_Print("bSetCgmsCopyRight[%d][%s]\n", stCpInfo.bSetCgmsCopyRight, OTL_ENUM2STR_CopyRight(stCpInfo.eCgmsCopyRight));
		HxLOG_Print("bOutputMute[%d]\n", stCpInfo.bOutputMute);

		MGR_OUTPUT_SetCopyProtect(pstContext->ulActionId, stCpInfo);
	}
	else
	{
		/* 다른 state에서는 no action */
	}
}

void _mgr_LiveView_base_SetAppState(XmgrLiveView_Context_t *pstContext, XmgrLiveView_State_e eAppState)
{
	HUINT32		ulActionId = pstContext->ulActionId;
	HERROR		hErr;

/*
	HxLOG_Print("[%s:%d] [AP:LiveView(%d)] pstContext->eAppState(%s) eAppState(%s)\n",
			__FUNCTION__, __LINE__, ulActionId, _mgr_LiveView_base_GetAppStateStr(pstContext->eAppState), _mgr_LiveView_base_GetAppStateStr(eAppState));
*/

/* 1. 현재 state에서 벗어나면서 처리할 것들 ... AV stop/hide/show 처리 */
	if( (pstContext->eAppState != eLiveViewState_CAS) && (eAppState == pstContext->eAppState) )
	{ /* 같은 state 끼리의 전환은 무시.  */
		HxLOG_Print("[AP:LiveView(%d)] \t=> Same WTV state. Do nothing.\n",ulActionId);
		return;
	}
	else if( (pstContext->eAppState > eLiveViewState_PID_START) && (eAppState < eLiveViewState_PID_START) )
	{ /* PID running -> PID not running. AV 및 subtitle stop 해야 함. */
		HxLOG_Print ("[AP:LiveView(%d)] \t=> Going to PID invalid states. Stop AV and subtitle.\n",ulActionId);

		/* 1. mw service stop */
		_mgr_LiveView_base_StopMwService(pstContext, eLiveMwSvcType_Av, eSyncMode);
		_mgr_LiveView_base_StopMwService(pstContext, eLiveMwSvcType_Cas, eSyncMode);
		_mgr_LiveView_base_StopMwService(pstContext, eLiveMwSvcType_Transcoder, eSyncMode);

		XMGR_LiveComp_BASE_StopSubtitle(pstContext->hAction, eSyncMode);
		XMGR_LiveComp_BASE_StopCloseCaption(pstContext->hAction, eSyncMode);

#if defined(CONFIG_3RD_BML)
		XMGR_LiveComp_BASE_StopBML(pstContext->hAction, eAsyncMode);
#endif

#if defined(CONFIG_MW_TELETEXT)
		/* 4. teletext stop */
		if(eAppState != eLiveViewState_NO_SIGNAL)
		{ /* NO signal일 경우에도 ttx page는 여전히 보여줘야 하므로 ttc 는 stop안하고 남겨 놓는다.
			다시 locking되었을 경우 이전 상태를 유지하기 위하여 PID running status도 reset 하면 안된다. */
			XMGR_LiveComp_BASE_StopTeletext(pstContext->hAction, eSyncMode);
			_mgr_LiveView_base_ResetPidRunningStatus(pstContext);
		}
#endif

		/* 5. AV stop notify. Maybe teletext, data will be stopped at this */
		_mgr_LiveView_base_SendBusMessage(NULL, eMEVT_OUTPUT_AV_STOPPED, pstContext->hAction, 0, 0, 0);
	}
	else if( (pstContext->eAppState > eLiveViewState_PROGRAM_SHOW) && (eAppState < eLiveViewState_PROGRAM_SHOW) )
	{ /* Show Program -> Hide Program */
		/* 1. AV hide */
		HxLOG_Print ("[AP:LiveView(%d)] \t=> Show program -> Hide program\n",ulActionId);

		hErr = SVC_AV_SetAudioHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_View,  TRUE);
		if (hErr)
		{
			HxLOG_Error("SVC_AV_SetAudioHide ret :0x%X\n", hErr);
		}

		hErr = SVC_AV_SetVideoHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_View, TRUE);
		if (hErr)
		{
			HxLOG_Error("SVC_AV_SetVideoHide ret :0x%X\n", hErr);
		}

#if defined(CONFIG_3RD_BML)
		XMGR_LiveComp_BASE_StopBML(pstContext->hAction, eAsyncMode);
#endif

		/* 2. Subtitle stop */
		_mgr_LiveView_base_StopMwService(pstContext, eLiveMwSvcType_Subtitle, eSyncMode);

		/* 2. Closed caption stop */
		_mgr_LiveView_base_StopMwService(pstContext, eLiveMwSvcType_CloseCaption, eSyncMode);

		/* 3. AV stop notify. Maybe teletext, data will be stopped at this */
		_mgr_LiveView_base_SendBusMessage(NULL, eMEVT_OUTPUT_AV_STOPPED, pstContext->hAction, 0, 0, 0);
	}

/* 2. 새 state 등록. context는 변경해 놔야 아래 루틴들이 작업시 반영함. */
	HxLOG_Print("[AP:LiveView(%d)] \t=> State transition <%s> => <%s>\n",ulActionId,
									_mgr_LiveView_base_GetAppStateStr(pstContext->eAppState),
									_mgr_LiveView_base_GetAppStateStr(eAppState));
	pstContext->eAppState = eAppState;

/* 3. 새로운 state로 진입하면서 처리할 것들 ... */
	switch(eAppState)
	{
		case eLiveViewState_SVC_NOT_RUNNING :
			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_NOT_RUNNING);
			break;

		case eLiveViewState_EMPTY_SVC :
			pstContext->bPmtValid = FALSE;
			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_EMPTY_SVC);
			break;

		case eLiveViewState_SVC_LOCKED:
			/**
			  *	view proc에선 svcLock 상태가 되면 av, si, cas 등을 stop시켜야 한다. (svc locked 에 들어가 화면이 안 나오는 상태에서 cas preview time 등의 소모를 막기 위해)
			  *	lock proc에서 svc lock 상태를 체크하여 view proc으로 msg를 보내서 알리고
			  *	view proc은 이를 받아 lock이면 appState를 eLiveViewState_SVC_LOCKED 로 변경하여 av 등을 stop 시킨다
			  *	이때 uiState는 ok로 하는데,  svc lock state를 OPL에 noti.하는 것은 lock proc에서 담당하기 때문이다
			**/
			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_OK);
			break;

		case eLiveViewState_NO_SIGNAL :
			/* SI가 time out 안내도록 no signal인 경우 SI까지 stop한다. 특히 motor인 경우에 유용하다. */
			_mgr_LiveView_base_StopMwService(pstContext, eLiveMwSvcType_Si, eSyncMode);

#if defined(CONFIG_3RD_BML)
			XMGR_LiveComp_BASE_StopBML(pstContext->hAction, eAsyncMode);
#endif
			/* PID running status는 그대로 유지해야 함. _mgr_LiveView_base_ResetPidRunningStatus() 부르지 않는다. */

			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_NO_SIGNAL);


			break;

		case eLiveViewState_PMT_PID_CHANGED :
			/* PMT PID가 바뀌면 모든 PID를 clear시켜야 하므로 SI와 CAS, TTX를 stop 시킨다.
				AV, subtitle은 eLiveViewState_AV탈출시 이미 stop됨.
				EIT PID는 일정하므로 stop 필요 없음. */
			_mgr_LiveView_base_StopMwService(pstContext, eLiveMwSvcType_Si, eSyncMode);
			_mgr_LiveView_base_StartMwServices(pstContext, FALSE);
			break;


/*------------------- PIN NOT Running States Above ------------------*/

/*------------------- PIN Running States Below ----------------------*/

		case eLiveViewState_AV :
			if(pstContext->bAvUnderrun == TRUE)
			{ /* Underrun 예약 되어 있음 */
				_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_AV_UNDER_RUN);
			}
			else if(pstContext->bHdmiBlocked == TRUE)
			{ /* HDMI blocked 예약 되어 있음. */
				_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_HDMI_BLOCKED);
			}
			else
			{ /* NO AV 메시지 체크. */
				_mgr_LiveView_base_CheckAV(pstContext);
			}

			/* DRM 반영하고 AV start. */
			_mgr_LiveView_base_SetCopyProtection(pstContext);
			hErr = SVC_AV_SetAudioHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_View,  FALSE);
			if (hErr)
			{
				HxLOG_Error("SVC_AV_SetAudioHide ret :0x%X\n", hErr);
			}

			hErr = SVC_AV_SetVideoHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_View, FALSE);
			if (hErr)
			{
				HxLOG_Error("SVC_AV_SetVideoHide ret :0x%X\n", hErr);
			}

#ifdef CONFIG_3RD_BML
			XMGR_LiveComp_BASE_StartBML(pstContext->hAction);
#endif

#if defined(CONFIG_MW_SUBTITLE)
			/* Subtitle start ... */
			_mgr_LiveView_base_StartSubtitle(pstContext);
#endif

			/* Application들에게 AV 시작을 알림. 이 action이 main action이라면 teletext, data방송등이 시작될 것임. */
			_mgr_LiveView_base_SendBusMessage(NULL, eMEVT_OUTPUT_AV_STARTED, pstContext->hAction, pstContext->hSvc, 0, 0);
			break;

		case eLiveViewState_CAS_PREVIEW :
		case eLiveViewState_CAS :
			// TODO: shko 여기 어딘가 cas ui 를 넣어줘야 ...? ,, web ap 에서 cas status 일 경우 배너는 막아달라고 요청하고..?
			/* CAS UI 생성 */
			if (pstContext->eUiState != eLiveViewUi_CAS)
			{
				if (eAppState == eLiveViewState_CAS_PREVIEW)
				{
					// CAS preview에서는 rating 등이 적용되어야 하니 ok로 올린다.
					_mgr_LiveView_base_SetUiState (pstContext, eLiveViewUi_OK);
				}
				else
				{
					_mgr_LiveView_base_SetUiState (pstContext, eLiveViewUi_CAS);
				}
			}

			HxLOG_Print ("[AP:ZAP(%d)] \t=> [eWtvState_CAS] bVideoShow (%d), bAudioShow (%d)\n",ulActionId, pstContext->bVideoShow, pstContext->bAudioShow);

			/* DRM 반영하고 AV start. */
			_mgr_LiveView_base_SetCopyProtection(pstContext);

			if(MGR_ACTION_GetMainActionId() == ulActionId)
			{
				/* set av hide */
				hErr = SVC_AV_SetAudioHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_View,  !(pstContext->bAudioShow));
				if (hErr)
				{
					HxLOG_Error("SVC_AV_SetAudioHide ret :0x%X\n", hErr);
				}

				hErr = SVC_AV_SetVideoHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_View, !(pstContext->bVideoShow));
				if (hErr)
				{
					HxLOG_Error("SVC_AV_SetVideoHide ret :0x%X\n", hErr);
				}
			}

#if defined(CONFIG_3RD_BML)
			XMGR_LiveComp_BASE_StopBML(pstContext->hAction, eAsyncMode);
#endif
			if( (pstContext->bVideoShow == TRUE) && (pstContext->bAudioShow == TRUE) )
			{ /* Both show에서만 정식으로 AV 시작된 것으로 본다.. */
#if defined(CONFIG_MW_SUBTITLE)
				/* Subtitle start ... */
				_mgr_LiveView_base_StartSubtitle(pstContext);
#endif
				/* Application들에게 AV 시작을 알림. 이 action이 main action이라면 teletext, data방송등이 시작될 것임. */
				_mgr_LiveView_base_SendBusMessage(NULL, eMEVT_OUTPUT_AV_STARTED, pstContext->hAction, pstContext->hSvc, 0, 0);
			}
			break;

		case eLiveViewState_SVC_STOP:
			XMGR_LiveComp_BASE_StopSubtitle(pstContext->hAction, eSyncMode);
			break;

		default :
			/* 위 case 말고 다른 state 나 WTV state가 아닌 값은 이 API를 쓰면 안된다. */
			HxLOG_Warning("eAppState(%s, %d)\n", _mgr_LiveView_base_GetAppStateStr(eAppState), eAppState);
			break;
	}
}

void _mgr_LiveView_base_SetUiState(XmgrLiveView_Context_t *pstContext, MgrLiveView_UiState_e eUiState)
{
//	HERROR					hErr;
	HUINT32					ulActionId = pstContext->ulActionId;

	HxLOG_Info ("[AP:LiveView(%d)] _mgr_LiveView_base_SetUiState(%s)\n", ulActionId,_mgr_LiveView_base_GetUiStateStr(eUiState));

#if 0//defined(CONFIG_3RD_MHEG)
	if(MGR_ACTION_GetMhegStatus() == eAmMhegStatus_Running && eUiState != eLiveViewUi_NO_SIGNAL)
	{
		HxLOG_Print("[AP:LiveView(%d)] Mheg is Available in Current Channel, So eUiState is always eLiveViewUi_OK \n\n",ulActionId);

		eUiState = eLiveViewUi_OK;
	}
#endif

	if(eUiState != pstContext->eUiState)
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> State UI transition <%s> => <%s>\n",ulActionId,
									_mgr_LiveView_base_GetUiStateStr(pstContext->eUiState),
									_mgr_LiveView_base_GetUiStateStr(eUiState));

		// 기존 코드가 UI Destroy가 불리면 UI 내에서 무조건 CLOSE CAS를 부르게 되어 있었다.
		// 나중에 경우에 따라서만 동작해야 하는지 확인해야 한다.

		if(eUiState == eLiveViewUi_AV_UNDER_RUN ||
			eUiState == eLiveViewUi_HDMI_BLOCKED ||
			eUiState == eLiveViewUi_NO_AV)
		{
			HLIB_CMD_CaptureLog("LiveView,UiStateString=%s,eUiState=%d",_mgr_LiveView_base_GetUiStateStr(eUiState),eUiState);
		}
#if defined(CONFIG_MW_LOGCAPTURE)
		BUS_PostMessage(NULL, eSEVT_LOGCAPTURE_CHANNEL_STATE, HANDLE_NULL, (HINT32)eUiState, (HINT32)eLiveViewUi_NO_SIGNAL, 0);
#endif
#if defined(CONFIG_3RD_MHEG)
		// mheg 채널인 경우에는 no av 를 출력하지 않음.(AV is not Available)
		if(MGR_ACTION_GetMhegStatus() == eAmMhegStatus_Running && (eUiState == eLiveViewUi_NO_AV || eUiState == eLiveViewUi_AV_UNDER_RUN))
		{
			return;
		}
#endif
		/* Update UI state */
		pstContext->eUiState = eUiState;

		HxLOG_Debug("send VIEW state (%s)  \n", _mgr_LiveView_base_GetUiStateStr(eUiState));

		if (pstContext->bBlockUiStateNoti == FALSE)
		{
			_mgr_LiveView_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_VIEW_STATE_CHANGED, pstContext->hAction, 0, 0, 0);	//<< param에다 uiState를 안 넣어줘도 알아서 가져간다
		}
	}
	else
	{
		/* 같은 state다. 그냥 있으면 되겠네... */
		HxLOG_Print("[AP:LiveView(%d)] \t=> Same UI state(%d) \n",ulActionId, eUiState);
	}
}

STATIC void _mgr_LiveView_base_CheckRadioBackground(XmgrLiveView_Context_t *pstContext)
{
	HUINT32			 ulActionId = pstContext->ulActionId;
	DbSvc_Info_t	*pstSvcInfo = NULL;

	pstSvcInfo = _mgr_LiveView_base_GetMasterSvcInfoPtr(pstContext);

	HxLOG_Info("[AP:LiveView(%d)] _mgr_LiveView_base_CheckRadioBackground(eSvcType[%d], usVideoPid[%x], bRadioBackground[%d]\n",ulActionId,
		DbSvc_GetSvcType(pstSvcInfo), DbSvc_GetVideoPid(&pstContext->stSvcInfo), pstContext->bRadioBackground);

	if( (DbSvc_GetSvcType(pstSvcInfo) == eDxSVC_TYPE_RADIO) && /* Radio service 이고 ... */
		(DbSvc_GetVideoPid(&pstContext->stSvcInfo) >= PID_NULL) /* Video PID가 NULL 이면 background 생성. */
	)
	{
		HxLOG_Print ("[AP:LiveView(%d)] OK! to draw RADIO BACKGROUND !!\n",ulActionId);

		if(pstContext->bRadioBackground == FALSE)
		{ /* Radio background가 안 떴으면 띄운다. */
			HxLOG_Print ("[AP:LiveView(%d)] \tActually drawing RADIO BACKGROUND!!\n",ulActionId);

			pstContext->bRadioBackground = TRUE; /* Drawing 중에 이 flag를 본다. */

			_mgr_LiveView_base_SendBusMessage (NULL, eMEVT_SVCLIST_RADIOBG_CHANGED, pstContext->hAction, (HINT32)pstContext->bRadioBackground, 0, 0);
		}
		else
		{
			/* 이미 떠 있음... 무시 */
		}
	}
	else
	{
		HxLOG_Print ("[AP:LiveView(%d)] \tNot in the radio background condition.\n",ulActionId);

		if(pstContext->bRadioBackground == TRUE)
		{ /* Radio background가 떠 있으면 죽인다. */
			HxLOG_Print ("[AP:LiveView(%d)] \tErase already drawn radio background.\n",ulActionId);

			pstContext->bRadioBackground = FALSE; /* Drawing 중에 이 flag를 본다. */

			_mgr_LiveView_base_SendBusMessage (NULL, eMEVT_SVCLIST_RADIOBG_CHANGED, pstContext->hAction, (HINT32)pstContext->bRadioBackground, 0, 0);
		}
	}
}


STATIC void	_mgr_LiveView_base_EvtDbCheckSvcDbChgFrontPanel(XmgrLiveView_Context_t *pContext, DbSvc_Info_t *pstSvcInfo)
{
#if defined(CONFIG_MW_SYS_7SEG)
	HINT32		ulChnum;
#endif

	if (!pstSvcInfo)
	{
		HxLOG_Error("\t==> Invalid service pointer.\n");
		return;
	}

#if defined(CONFIG_MW_SYS_7SEG)
	ulChnum = pContext->nChNum;
#endif
#if 0
	DB_SVC_GetServiceNumber(hMasterSvc, &pContext->nChNum);
#else
	pContext->nChNum = DbSvc_GetLcn(pstSvcInfo);
#endif

#if defined(CONFIG_SUPPORT_FCC)
	if (pContext->ulActionId != MGR_ACTION_GetMainActionId())
	{
		// FCC action id로는 front string setting하지 않는다
		return;
	}
#endif
	if(MWC_UTIL_DvbStrncmp(DbSvc_GetName(&pContext->stSvcInfo), DbSvc_GetName(pstSvcInfo), DbSvc_GetNameLength()) != 0)
	{ /* Service 이름 변경됨. Front display 변경. */
		HxLOG_Print("\t==> Service name changed.\n");
#if !defined(CONFIG_MW_SYS_7SEG)
		SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, DbSvc_GetName(pstSvcInfo), eDxPANEL_ALIGN_LEFT);
#endif
	}

#if defined(CONFIG_MW_SYS_7SEG)
	if(pContext->nChNum != ulChnum)
	{ /* Service NUM 변경됨. Front display 변경. */
		HxLOG_Print("\t==> Service number changed.\n");
		HxLOG_Print("\t==> pContext->nChNum = %d, ulChnum = %d .\n",pContext->nChNum, ulChnum);

		_mgr_LiveView_base_DisplayPanelSetAsCurrentChNumber(pContext);
	}
#endif
}

STATIC HERROR	_mgr_LiveView_base_EvtDbCheckSvcDbByDeleteSvc(HUINT32 ulActionId, ChListUiGroup_t svcListGroup, DxDeliveryType_e eOrgDeliType, Handle_t hNextSvcHandle)
{
	Handle_t		hNewSvc = HANDLE_NULL;
	HERROR			hError = ERR_FAIL;
	DbSvc_Group_e 	svcGroup = eSvcGroup_All;
	DbSvc_Attr_t		svcAttr;

	switch (svcListGroup)
	{
		case eChListUiGroup_FAV_1:
		case eChListUiGroup_FAV_2:
		case eChListUiGroup_FAV_3:
		case eChListUiGroup_FAV_4:
		case eChListUiGroup_FAV_5:
		case eChListUiGroup_HDTV:
		case eChListUiGroup_RECENT:
			MGR_SVCLIST_GetSvcDbParamsFromGroup(svcListGroup, &svcGroup, &svcAttr);
			svcAttr.eOrgDeliType = eOrgDeliType;

			if (hNextSvcHandle != HANDLE_NULL)
			{
				hError = DB_SVC_CheckValidServiceHandle(hNextSvcHandle);
				if (hError == ERR_OK)
				{
					hNewSvc = hNextSvcHandle;
				}
				else
				{
					hError = DB_SVC_FindServiceHandle(svcGroup, &svcAttr, 0, eSvcFindDirection_First, &hNewSvc);
				}
			}
			else
			{
				hError = DB_SVC_FindServiceHandle(svcGroup, &svcAttr, 0, eSvcFindDirection_First, &hNewSvc);
			}

			if (hError == ERR_OK)
			{ /* delivery type 까지 맞춰본다. */
				HxLOG_Print("\t==> My service handle was deleted. Re-tune to the 1st service of %s of MY delivery type.\n",
															xmgr_live_GetSvcListGroupStr(svcListGroup) );
				MGR_LIVE_CmdSendServiceStartByActionId(ulActionId, hNewSvc);
			}
			else
			{ /* delivery type 포기하고 chlist ui group에서 찾아본다. */
				MGR_SVCLIST_GetSvcDbParamsFromGroup(svcListGroup, &svcGroup, &svcAttr);

				hError = DB_SVC_FindServiceHandle(svcGroup, &svcAttr, 0, eSvcFindDirection_First, &hNewSvc);
				if (hError == ERR_OK)
				{
					HxLOG_Print("\t==> My service handle was deleted. Re-tune to the 1st service of %s of MY Service type.\n",
																xmgr_live_GetSvcListGroupStr(svcListGroup) );
					MGR_LIVE_CmdSendServiceStartByActionId(ulActionId, hNewSvc);
				}
				else
				{ /* Channel list ui group 포기하고 전체에서 다시 찾아본다. */
					MGR_SVCLIST_GetSvcDbParamsFromGroup(eChListUiGroup_All, &svcGroup, &svcAttr);

					hError = DB_SVC_FindServiceHandle(svcGroup, &svcAttr, 0, eSvcFindDirection_First, &hNewSvc);
					if (hError == ERR_OK)
					{
						HxLOG_Print("\t==> My service handle was deleted. Re-tune to the 1st service of %s of ALL delivery type.\n",
																	xmgr_live_GetSvcListGroupStr(eChListUiGroup_All));
						MGR_LIVE_CmdSendServiceStartByActionId(ulActionId, hNewSvc);
					}
					else
					{
						HxLOG_Error("\t==> No Radio? No TV? Where should I go???.\n");
					}
				}
			}
			return	ERR_OK;

		default:
			// Else case 는 해당 프로젝트 에서 알아서..ㅋㅋ
			HxLOG_Assert(0);
			return	ERR_FAIL;
	}
}

#if defined (CONFIG_MW_SYS_7SEG)
STATIC void	_mgr_LiveView_base_DisplayPanelSetAsCurrentChNumber(XmgrLiveView_Context_t *pstContext)
{
	HUINT8	ucChNumber[10];

#if defined(CONFIG_APUI_FAV_ORDERING_NUMBER)
	{
		HINT32		nFavChNum = 0;
		HERROR		hError = ERR_FAIL;

		hError = xmgr_live_GetFavouriteCHNumber(pstContext->svcListGroup, pstContext->hSvc, &nFavChNum);
		if ((hError == ERR_OK) && (nFavChNum != 0))
		{
			snprintf(ucChNumber, 10, "%4d", nFavChNum);
		}
		else
		{
			DbSvc_Info_t	*pstSvcInfo = _mgr_LiveView_base_GetMasterSvcInfoPtr(pstContext);

			snprintf(ucChNumber, 10, "%4d", DbSvc_GetLcn(pstSvcInfo);
		}
	}
#else
	/*
	 * LCN이 없을 경우, MajorChannelNumer를 통해서 채널 번호를 출력한다.
	 */
	snprintf(ucChNumber, 10, "%04d", pstContext->stStartArg.unExtArg.stMainLiveArg.ulMajorCHNum);
#endif

	SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, ucChNumber, eDxPANEL_ALIGN_LEFT);
}
#endif // #if defined (CONFIG_MW_SYS_7SEG)

void _mgr_LiveView_base_SetFrontPanel(XmgrLiveView_Context_t *pstContext)
{
	HUINT32		ulActionId = pstContext->ulActionId;
//	HUINT32		ulPanelStringLen	= 0;

	HxLOG_Info ("[AP:LiveView(%d)] _mgr_LiveView_base_SetFrontPanel()\n",ulActionId);

	if ( MGR_ACTION_GetState() == eAmState_ACTIVE_STANDBY )
	{
		HxLOG_Debug("[AP:LiveView(%d)] Action State eAmState_ACTIVE_STANDBY.. we will not SetFrontPanel\n", ulActionId);
		return;
	}

	if(ulActionId == MGR_ACTION_GetMainActionId())
	{
		DbSvc_Info_t *pstSvcInfo = _mgr_LiveView_base_GetMasterSvcInfoPtr(pstContext);

		/* Front panel string display */

#if defined (CONFIG_MW_SYS_7SEG)
		_mgr_LiveView_base_DisplayPanelSetAsCurrentChNumber(pstContext);
#else
{

		/* Front panel string display */
		SVC_SYS_DisplayPanelString (ePANEL_STR_PRIORITY_VIEW_ACTION, DbSvc_GetName(pstSvcInfo), eDxPANEL_ALIGN_LEFT);
}
#endif // #if (CONFIG_MW_SYS_7SEG)

		/* Front panel LED display */
		switch(DbSvc_GetSvcType(pstSvcInfo))
		{
			case eDxSVC_TYPE_TV :
				_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_TV, TRUE);
				_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_RADIO, FALSE);
				break;

			case eDxSVC_TYPE_RADIO :
				_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_TV, FALSE);
				_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_RADIO, TRUE);
				break;

			default :
				HxLOG_Print("\t=> Unknown service type : %d\n", DbSvc_GetSvcType(pstSvcInfo));
				break;
		}
	}
}

void _mgr_LiveView_base_BackUpCurrentService(XmgrLiveView_Context_t *pstContext)
{
	HUINT32			ulActionId = pstContext->ulActionId;

	HxLOG_Info("[AP:LiveView(%d)] _mgr_LiveView_base_BackUpCurrentService()\n",ulActionId);

	if(ulActionId == MGR_ACTION_GetMainActionId())
	{ /* Main action의 master service handle만 last service로 백업한다. */
		MGR_LIVE_RegisterLastSvcInfo(_mgr_LiveView_base_GetMasterSvcHandle(pstContext));
	}
	else
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> NOT Focused! Backup ignored ...\n",ulActionId);
	}
}


STATIC void _mgr_LiveView_base_CheckAV(XmgrLiveView_Context_t *pstContext)
{
	HUINT32			 ulActionId = pstContext->ulActionId;
	DbSvc_Info_t	*pstSvcInfo = _mgr_LiveView_base_GetMasterSvcInfoPtr(pstContext);

	HxLOG_Info("[AP:LiveView(%d)] _mgr_LiveView_base_CheckAV()\n",ulActionId);

	if(pstContext->bPmtValid == TRUE)
	{
		if(	(DbSvc_GetAudioPid(pstSvcInfo) >= PID_NULL) &&
			(DbSvc_GetVideoPid(pstSvcInfo) >= PID_NULL) &&
			(DbSvc_GetDolbyPid(pstSvcInfo) >= PID_NULL) &&
			(SvcSi_OrgSvcType_IS_DATA(DbSvc_GetOrgSvcType(pstSvcInfo)) != TRUE)
			)
		{
			/* PMT까지 들어온 상황에서 PID가 모두 NULL이면 NO_AV ui state로 감. PMT 이전에는 유보... */
			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_NO_AV);

		}
		else if((SvcSi_OrgSvcType_IS_TV(DbSvc_GetOrgSvcType(pstSvcInfo)) )
				&&(DbSvc_GetVideoPid(pstSvcInfo) >= PID_NULL) )
		{ /* TV service에서 Video PID가 없을 경우 */
			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_NO_VIDEO);
		}
		else
		{
			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_OK);
		}
	}
	else
	{
		_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_OK);
	}
}


STATIC void _mgr_LiveView_base_CheckDolbyLed(XmgrLiveView_Context_t *pstContext)
{
	if	(DbSvc_GetDolbyFlag(&pstContext->stSvcInfo) == TRUE)
	{
		_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_DOLBY, TRUE);
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T)
		if (DbSvc_GetDolbyCodec(&pstContext->stSvcInfo) == eDxAUDIO_CODEC_DOLBY_AC3P)
		{
			_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_DOLBY_PLUS, TRUE);
		}
#endif
	}
	else
	{
		_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_DOLBY, FALSE);
	}

	if	(DbSvc_GetVideoType(&pstContext->stSvcInfo) == eDxVIDEO_TYPE_HD)
	{
		_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_HD, TRUE);
	}
	else
	{
		_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_HD, FALSE);
	}
}

STATIC Handle_t _mgr_LiveView_base_GetMasterSvcHandle(XmgrLiveView_Context_t *pstContext)
{
	if(pstContext->sessionType == eLiveViewSession_NORMAL)
	{
		return pstContext->hSvc;
	}
	else
	{
		return pstContext->hMasterSvc;
	}
}

STATIC DbSvc_Info_t *_mgr_LiveView_base_GetMasterSvcInfoPtr(XmgrLiveView_Context_t *pstContext)
{
	if(pstContext->sessionType == eLiveViewSession_NORMAL)
	{
		return &(pstContext->stSvcInfo);
	}
	else
	{
		return &(pstContext->stMasterSvcInfo);
	}
}

STATIC HBOOL _mgr_LiveView_base_CheckComplementariesStartCondition(XmgrLiveView_Context_t *pstContext)
{
	// live, transcoding(?)에서만 start 허용
	switch(pstContext->eLiveMode)
	{
	case eMgrLiveMode_PipLive:
	case eMgrLiveMode_LiveStreaming:
		return FALSE;

	default:
		break;
	}

	return TRUE;
}

STATIC HUINT16 _mgr_LiveView_base_GetVideoPid(XmgrLiveView_Context_t *pstContext, HUINT16 usCurVideoPid)
{
	if (pstContext == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_GREEN("[AP:LiveView] invalid parameter !! "));
		return usCurVideoPid;
	}

	if (pstContext->bUnselectVideo == TRUE)
	{
		HxLOG_Debug(HxANSI_COLOR_GREEN("[AP:LiveView(%d)] Video unselected : video pid (0x%X -> PID_NULL) \n"), pstContext->ulActionId, usCurVideoPid);
		return PID_NULL;
	}

	return usCurVideoPid;
}

STATIC HUINT16 _mgr_LiveView_base_GetAudioPid(XmgrLiveView_Context_t *pstContext, HUINT16 usCurAudioPid)
{
	if (pstContext == NULL)
	{
		HxLOG_Error(HxANSI_COLOR_GREEN("[AP:LiveView] invalid parameter !! "));
		return usCurAudioPid;
	}

	if (pstContext->bUnselectAudio == TRUE)
	{
		HxLOG_Debug(HxANSI_COLOR_GREEN("[AP:LiveView(%d)] Audio unselected : audio pids (0x%X -> PID_NULL) \n"), pstContext->ulActionId, usCurAudioPid);
		return PID_NULL;
	}

	return usCurAudioPid;
}

STATIC HERROR _mgr_LiveView_base_SetLedOnOff(HUINT32 ulActionId, DxLedId_e eLedId, HBOOL bOnOff)
{
#if defined(CONFIG_SUPPORT_FCC)
	if (ulActionId != MGR_ACTION_GetMainActionId())
	{
		// FCC action들은 front setting하지 못한다
		return ERR_OK;
	}
#endif

	return SVC_SYS_SetLedOnOff(eLedId, bOnOff);
}

#define _____PROTOTYPE_MSG_METHODS_____
BUS_Result_t XMGR_LiveView_BASE_MsgBusCreate(XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				 ulActionId = SVC_PIPE_GetActionId(hAction);
	BUS_Result_t		 eBusResult = ERR_BUS_INITIAL_FAILED;
	MgrLive_START_t	*pstStart = (MgrLive_START_t *)p1;
	HERROR				 hErr = ERR_FAIL;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstContext->ulActionId = ulActionId;
	pstContext->hAction = hAction;
	pstContext->hSvc = pstStart->hService;
	pstContext->hMasterSvc = pstStart->hMasterSvc;
	pstContext->hSuppleSvc = pstStart->hSuppleSvc;
	pstContext->eLiveMode = pstStart->eLiveMode;
	pstContext->sessionType = pstStart->eLiveSessionType;
	pstContext->ulMainActionId = MGR_ACTION_GetMainActionId(); /* 생성 당시의 main action id를 기억해서 나중에 swap 할때 사용 */
	pstContext->eAppState = eLiveViewState_IDLE;
	pstContext->eUiState = eLiveViewUi_NONE;
	pstContext->bRadioBackground = FALSE;
	HxSTD_memcpy (&(pstContext->stStartArg), pstStart, sizeof(MgrLive_START_t));

	switch(pstContext->eLiveMode)
	{
		case eMgrLiveMode_MainLive:
			eBusResult = _mgr_LiveView_base_SetupMainLive(pstContext);
			break;

		case eMgrLiveMode_PipLive:
			eBusResult = _mgr_LiveView_base_SetupPipLive(pstContext);
			break;

		case eMgrLiveMode_LiveStreaming:
			eBusResult = _mgr_LiveView_base_SetupLiveStreaming(pstContext);
			break;

		case eMgrLiveMode_FccLive:
			eBusResult = _mgr_LiveView_base_SetupFccLive(pstContext);
			break;

		default:
			HxLOG_Error ("invalid eLiveMode(%d) \n", pstContext->eLiveMode);
			eBusResult = _mgr_LiveView_base_SetupMainLive(pstContext);
			break;
	}

	if (eBusResult != MESSAGE_BREAK)
	{
		HxLOG_Error ("setup failed.. suicide now ! - eLiveMode(%d) eBusResult(%d) \n", pstContext->eLiveMode, eBusResult);
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}
	else
	{
		hErr = MGR_RSC_GetResourceInfo(pstContext->ulActionId, &pstContext->stRscInfo);
		if (hErr)
		{
			HxLOG_Error("MGR_RSC_GetResourceInfo ret :0x%X\n",hErr);
		}
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return eBusResult;
}

BUS_Result_t XMGR_LiveView_BASE_MsgBusDestroy(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr = ERR_FAIL;
	MgrAction_Type_e 	eActionType = eViewType_WTV;
	SvcEpg_EpgKind_e		eEpgKind = eEPG_KIND_DVB;

#if defined(CONFIG_SUPPORT_FCC)
	BUS_Callback_t		pfActionProc = NULL;
	HBOOL				bActionTaken = FALSE;
#endif


#if defined(CONFIG_MW_CAS_IRDETO) && (CONFIG_MW_CAS_NUM_EMBEDDED == 1)
	if(pstContext->eUiState == eLiveViewUi_NO_SIGNAL)
	{
		BUS_KillTimer(NO_SIGNAL_TIMER_ID);
	}
#endif

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/* 마지막으로 UI를 clear 하고 */
	_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_OK);

	/* 모든 MW 리소스를 릴리즈 함. Sync로 해야 함. */
	_mgr_LiveView_base_StopMwServices(pstContext, TRUE, eSyncMode);

	// stop ch tune
	hErr = SVC_CH_StopTune(pstContext->hAction, (HUINT16)-1, eAsyncMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_CH_StopTune ret :0x%X\n",hErr);
	}

#if defined(CONFIG_3RD_BML)
	XMGR_LiveComp_BASE_StopBML(pstContext->hAction, eAsyncMode);
#endif

	hErr = XMGR_LiveComp_BASE_StopEpg(eEpgKind, eEPG_EVT_KIND_ALL, pstContext->hAction);
	if (hErr)
	{
		HxLOG_Error("Error ! XMGR_LiveComp_BASE_StopEpg()\n");
	}

	/* 혹시 걸려있을 copy protection은 풀고 나가야 한다. 뒤에 오는 action이 처리하지 않는 경우가 대부분이다. */
#if defined(CONFIG_SUPPORT_FCC)
	pfActionProc = MGR_ACTION_GetActionProc(ulActionId);
	if ((pfActionProc != NULL) && (pfActionProc != (BUS_Callback_t)-1))
	{
		bActionTaken = TRUE;
	}

	if (bActionTaken == TRUE)
#endif
	{
		MGR_OUTPUT_ResetCopyProtect(ulActionId);
	}

	/* 최종적으로 IDLE state 설정. API의 validity check 용. */
	pstContext->eAppState = eLiveViewState_IDLE;

	/* disable radio still */
	if (pstContext->bRadioBackground)
	{
		pstContext->bRadioBackground = FALSE;
		_mgr_LiveView_base_SendBusMessage (NULL, eMEVT_SVCLIST_RADIOBG_CHANGED, pstContext->hAction, (HINT32)pstContext->bRadioBackground, 0, 0);
	}

	/* Action 을 릴리즈 함. */
	eActionType = MGR_ACTION_GetType(ulActionId);
	if (eActionType == eActionType_NONE)
	{
		eActionType = eViewType_WTV;	// default WTV
		HxLOG_Error("MGR_ACTION_GetType() failed..! \n");
	}

	hErr = MGR_ACTION_Release(ulActionId, eActionType);
	if (hErr)
	{
		HxLOG_Error("MGR_ACTION_Release ret :0x%X eActionType(%d) \n",hErr, eActionType);
	}

#if defined(CONFIG_SUPPORT_FCC)
	if (bActionTaken == TRUE)
#endif
	{
		hErr = SVC_AV_SetVideoHide(pstContext->ulActionId, eAvMuteFlag_All, FALSE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_AV_SetVideoHide(%d) \n",hErr);
		}
		hErr = SVC_AV_SetAudioHide(pstContext->ulActionId, eAvMuteFlag_All, FALSE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_AV_SetAudioHide(%d) \n",hErr);
		}

		SVC_AV_SetVideoOutputEnable(pstContext->ulActionId, eSvcAv_DisplayType_Hd, TRUE);
		SVC_AV_SetVideoOutputEnable(pstContext->ulActionId, eSvcAv_DisplayType_Sd, TRUE);
	}

	_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_DOLBY, FALSE);

#if defined(CONFIG_SUPPORT_FCC)
	if (ulActionId == MGR_ACTION_GetMainActionId())
	{
		// main live stop 되었으니 fcc live도 stop해주자
		MGR_LIVE_StopAllFccSvc();
	}
#endif

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return MESSAGE_BREAK;
}

BUS_Result_t XMGR_LiveView_BASE_MsgLiveStartSvc (XmgrLiveView_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32						 ulActionId = SVC_PIPE_GetActionId (hHandle);
	ChListUiGroup_t				 eSvcListGroup = eChListUiGroup_TV;
	MgrLive_TuningMode_t		 eTuningMode = eNO_IPLATE;
	MgrLive_START_t				*pstStart = (MgrLive_START_t *)p1;
	MgrAction_Priority_e		 eActPriority;
	HERROR						 hErr = ERR_FAIL;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if defined(CONFIG_MW_CAS_IRDETO) && (CONFIG_MW_CAS_NUM_EMBEDDED == 1)
	BUS_ResetTimer(NO_SIGNAL_TIMER_ID);
#endif

	if (SVC_PIPE_GetActionId (pstContext->hAction) != ulActionId)
	{
		HxLOG_Error ("Action Handle not available\n");
		return MESSAGE_PASS;
	}

	pstContext->hSvc = pstStart->hService;
	pstContext->hMasterSvc = pstStart->hMasterSvc;
	pstContext->hSuppleSvc 	= pstStart->hSuppleSvc;
	pstContext->eLiveMode = pstStart->eLiveMode;
	pstContext->sessionType = pstStart->eLiveSessionType;

	HxSTD_memcpy (&(pstContext->stStartArg), pstStart, sizeof(MgrLive_START_t));

	if (eMgrLiveMode_LiveStreaming == pstContext->eLiveMode)
	{
		DxLiveStreamPriority_e		 eLsPriority = eDxLsPriority_EQUAL_REC;

		eActPriority = eAmPriority_RECORD;

		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LIVESTREAM_PRIORITY, (HUINT32 *)&eLsPriority, 0);
		if (ERR_OK == hErr)
		{
			switch (eLsPriority)
			{
			case eDxLsPriority_EQUAL_REC:
				eActPriority = eAmPriority_RECORD;
				break;

			case eDxLsPriority_EQUAL_VIEW:
				eActPriority = eAmPriority_VIEW_ACTION;
				break;

			case eDxLsPriority_BELOW_VIEW:
				eActPriority = (eAmPriority_VIEW_ACTION - (1<<ACTION_PRIORITY_BIT_OFFSET));
				break;

			case eDxLsPriority_UPPER_REC:
				eActPriority = (eAmPriority_RECORD + (1<<ACTION_PRIORITY_BIT_OFFSET));
				break;

			default:
				break;
			}
		}
	}
	else
	{
		eActPriority = eAmPriority_VIEW_ACTION;
	}

	hErr = _mgr_LiveView_base_SetupAction(ulActionId, pstContext->hSvc, eActPriority, pstContext->eLiveMode);
	if (ERR_OK != hErr)
	{
		// svc start 도중 action setup에 실패했으므로 suicide
		HxLOG_Error ("setup failed.. suicide now ! - actionId(%d) eLiveMode(%d) hErr(%d) \n", ulActionId, pstContext->eLiveMode, hErr);

		// 종료
		BUS_MGR_Destroy (NULL);
		return ERR_BUS_INITIAL_FAILED;
	}

	HxLOG_Debug("pstContext->eLiveMode(%d) pstContext->sessionType(%d) MasterSvcUid(0x%x), SuppleSvcUid(0x%x) \n",
		pstContext->eLiveMode, pstContext->sessionType, DB_SVC_GetServiceUID(pstContext->hSvc),  DB_SVC_GetServiceUID(pstContext->hSuppleSvc));

	if(pstContext->sessionType == eLiveViewSession_MHEG_QUIET)
	{
		/* Tune to master service handle. To setup master info and show banner and front display. */
		_mgr_LiveView_base_StartSuppleSession(pstContext, pstContext->hSuppleSvc, eLiveViewSession_MHEG_QUIET, 0);
	}
	else
	{
#if defined(CONFIG_3RD_MHEG_SI_MONITORING)
		eTuningMode = pstContext->stStartArg.unExtArg.stMainLiveArg.eTuneMode;
#endif
		/* Tune to master service handle. To setup master info and show banner and front display. */
		_mgr_LiveView_base_StartMasterSession(pstContext, pstContext->hSvc, eSvcListGroup, eTuningMode);
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);

	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveView_BASE_MsgLiveStopSvc (XmgrLiveView_Context_t *pstContext, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			 ulActionId = SVC_PIPE_GetActionId (hHandle);
	HERROR			 hErr;

	if (SVC_PIPE_GetActionId (pstContext->hAction) != ulActionId)
	{
		HxLOG_Error ("Action Handle not available\n");
		return MESSAGE_PASS;
	}

	/* disable radio still */
	if (pstContext->bRadioBackground)
	{
		pstContext->bRadioBackground = FALSE;
		_mgr_LiveView_base_SendBusMessage (NULL, eMEVT_SVCLIST_RADIOBG_CHANGED, pstContext->hAction, (HINT32)pstContext->bRadioBackground, 0, 0);
	}

	// Stop Service
	// ToDo:

	// av unmute
	hErr = SVC_AV_SetAudioHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_View,  FALSE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetAudioHide ret :0x%X\n", hErr);
	}

	hErr = SVC_AV_SetVideoHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_View, FALSE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetVideoHide ret :0x%X\n", hErr);
	}

	hErr = SVC_AV_SetVideoOutputEnable(SVC_PIPE_GetActionId(pstContext->hAction), eSvcAv_DisplayType_Hd, TRUE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetVideoOutputEnable ret :0x%X\n", hErr);
	}

	hErr = SVC_AV_SetVideoOutputEnable(SVC_PIPE_GetActionId(pstContext->hAction), eSvcAv_DisplayType_Sd, TRUE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetVideoOutputEnable ret :0x%X\n", hErr);
	}

	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveView_BASE_MsgLiveLockSvcLocked (XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId = SVC_PIPE_GetActionId (hAction);

	if (SVC_PIPE_GetActionId (pstContext->hAction) != ulActionId)
	{
		return MESSAGE_PASS;
	}

	if (ERR_OK != SVC_PIPE_IsActionHandleLatest (hAction))
	{
		HxLOG_Error ("ActionHandle not Latest\n");
		return MESSAGE_PASS;
	}

	if (pstContext->eAppState > eLiveViewState_IDLE)
	{
		_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_SVC_LOCKED);
	}
	else
	{
		HxLOG_Error ("(%s) -  eMEVT_LIVE_NOTIFY_SVC_UNLOCKED received. \n", _mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	}


	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveView_BASE_MsgLiveLockSvcUnlocked (XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId = SVC_PIPE_GetActionId (hAction);
	HERROR		hErr = ERR_FAIL;

	if (SVC_PIPE_GetActionId (pstContext->hAction) != ulActionId)
	{
		return MESSAGE_PASS;
	}

	if (ERR_OK != SVC_PIPE_IsActionHandleLatest (hAction))
	{
		HxLOG_Error ("ActionHandle not Latest\n");
		return MESSAGE_PASS;
	}

	if (pstContext->eAppState == eLiveViewState_SVC_LOCKED)
	{
		hErr = _mgr_LiveView_base_StartMwServices (pstContext, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("XMGR_LIVE_BASE_StartMwServices err:\n");
			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Error ("(%s) -  eMEVT_LIVE_NOTIFY_SVC_UNLOCKED received. \n", _mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	}

	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveView_BASE_EvtChStopped(XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulActionId = SVC_PIPE_GetActionId (hAction);
	HERROR		hErr;

	if (SVC_PIPE_GetActionId (pstContext->hAction) != ulActionId)
	{
		return MESSAGE_PASS;
	}

	if (ERR_OK != SVC_PIPE_IsActionHandleLatest (hAction))
	{
		HxLOG_Error ("ActionHandle not Latest\n");
		return MESSAGE_PASS;
	}

	if (eLiveViewState_CH_STOP == pstContext->eAppState)
	{
		hErr = _mgr_LiveView_base_StopMwServices (pstContext, TRUE, eAsyncMode);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("_mgr_LiveView_base_StopMwServices Err: hAction(0x%08x), hSvc(0x%08x)\n", pstContext->hAction, pstContext->hSvc);
		}

		_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_SVC_STOP);
	}
	else
	{
		HxLOG_Error ("(%s) -  eSEVT_CH_STOPPED received. \n", _mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	}

 	LIVE_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);

	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveView_BASE_EvtSiStopped(XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			 ulActionId = SVC_PIPE_GetActionId (hAction);

	if (SVC_PIPE_GetActionId (pstContext->hAction) != ulActionId)
	{
		return MESSAGE_PASS;
	}

#if defined(CONFIG_OP_NORDIG)
	if((pstContext->sessionType == eLiveViewSession_REPLACE) && (pstContext->eLiveMode == eMgrLiveMode_FccLive))
	{
		HERROR		hErr;

		/* Replacement service sesssin에서는 orginal service로 silent하게 복귀. */
		hErr = _mgr_LiveView_base_SetupAction(ulActionId, pstContext->hMasterSvc, eAmPriority_VIEW_ACTION, pstContext->eLiveMode);
		if(hErr == ERR_OK)
		{
			/* ERR_OK를 받아야만 이동하고 못 받으면 현재 채널에 머물러야 한다. */
			hErr = _mgr_LiveView_base_StartMasterSession(pstContext, pstContext->hMasterSvc, pstContext->svcListGroup, eNO_IPLATE|eKEEP_PINPASS);
		}
		else
		{
			HxLOG_Print("[AP:LiveView(%d)] Can't tune to org. service from replacement service by hErr[%d]\n",ulActionId, hErr);
		}
	}
#endif

	if (eLiveViewState_SVC_STOP == pstContext->eAppState)
	{
		pstContext->aeServicesState[eLiveMwSvcType_Si] = eLiveMwSvcState_Stopped;
		_mgr_LiveView_base_CheckAllMwServiceStopCompleted (pstContext);
	}
	else
	{
		HxLOG_Error ("(%s) - eSEVT_SI_STOPPED received.\n", _mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	}

 	LIVE_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);

	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveView_BASE_EvtCasStopped(XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32			 ulActionId = SVC_PIPE_GetActionId (hAction);

	if (SVC_PIPE_GetActionId (pstContext->hAction) != ulActionId)
	{
		return MESSAGE_PASS;
	}

	if (eLiveViewState_SVC_STOP == pstContext->eAppState)
	{
		pstContext->aeServicesState[eLiveMwSvcType_Cas] = eLiveMwSvcState_Stopped;
		_mgr_LiveView_base_CheckAllMwServiceStopCompleted (pstContext);
	}
	else
	{
		HxLOG_Error ("(%s) - eSEVT_CAS_STOPPED received.\n", _mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	}

	LIVE_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);

	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveView_BASE_EvtChNoSignal(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_NO_SIGNAL);

	/* WTV state는 NO_SIGNAL 로 하고 UI는 바로 MOTOR_MOVING으로 바꿈. */
	HxLOG_Print("[AP:LiveView(%d)] \t=> Actually, this message is eSEVT_CH_NO_SIGNAL\n",ulActionId);
	_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_NO_SIGNAL);

	/* Last Channel 을 백업한다. 원래는 PMT를 받으면 저장하는 것인데, No Signal 인 경우에는
		PMT가 안 들어오기 때문에 일단 백업한다. PMT받은 다음에 no signal이 되었을 수도 있는데,
		아래 API는 중복 저장등을 알아서 걸러내므로 확인차 자주 불러도 상관없다. */
	_mgr_LiveView_base_BackUpCurrentService(pstContext);

	//--------------------------------------------------------

#if defined(CONFIG_MW_CAS_IRDETO) && (CONFIG_MW_CAS_NUM_EMBEDDED == 1)
	BUS_SetTimer(NO_SIGNAL_TIMER_ID, NO_SIGNAL_TIMEOUT);
#endif

	pstContext->ulLastTuneStatus = eSEVT_CH_NO_SIGNAL;

 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtChMovingMotor(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_NO_SIGNAL);

	/* WTV state는 NO_SIGNAL 로 하고 UI는 바로 MOTOR_MOVING으로 바꿈. */
	HxLOG_Print("[AP:LiveView(%d)] \t=> Actually, this message is eSEVT_CH_MOVING_MOTOR\n",ulActionId);
	_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_MOTOR_MOVING);

	/* Last Channel 을 백업한다. 원래는 PMT를 받으면 저장하는 것인데, No Signal 인 경우에는
		PMT가 안 들어오기 때문에 일단 백업한다. PMT받은 다음에 no signal이 되었을 수도 있는데,
		아래 API는 중복 저장등을 알아서 걸러내므로 확인차 자주 불러도 상관없다. */
	_mgr_LiveView_base_BackUpCurrentService(pstContext);

	pstContext->ulLastTuneStatus = eSEVT_CH_MOVING_MOTOR;

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtChLocked(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(CONFIG_MW_CAS_IRDETO) && (CONFIG_MW_CAS_NUM_EMBEDDED == 1)
	BUS_KillTimer(NO_SIGNAL_TIMER_ID);
#endif

	if(pstContext->eAppState == eLiveViewState_NO_SIGNAL)
	{
		_mgr_LiveView_base_StartMwServices(pstContext , FALSE);
	}
	else
	{
		/* 다른 state에서는 반응하지 않는다. */
	}

	pstContext->ulLastTuneStatus = eSEVT_CH_LOCKED;

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_EvtSiDrmSignalled(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCopyrightControl_t		stCcInfo = {0, };

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	HLIB_STD_MemCpySafe(&(stCcInfo), sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	pstContext->bSiDrmValid = TRUE;
	HLIB_STD_MemCpySafe(&(pstContext->drmInfo.stSiCcInfo), sizeof(DxCopyrightControl_SiCcInfo_t), &(stCcInfo.stSiCcInfo), sizeof(DxCopyrightControl_SiCcInfo_t));

	if(pstContext->bCasDrmValid == FALSE)
	{
		_mgr_LiveView_base_SetCopyProtection(pstContext);
	}
	else
	{
		/* CAS가 먼저 왔으면 SI는 반영되면 안됨. */
	}

	//--------------------------------------------------------
	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS)
STATIC BUS_Result_t XMGR_LiveView_EvtCasDrmSignalled(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCopyrightControl_t		stCcInfo = {0, };

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	HLIB_STD_MemCpySafe(&(stCcInfo), sizeof(DxCopyrightControl_t), (void *)p3, sizeof(DxCopyrightControl_t));

	pstContext->bCasDrmValid = TRUE;

	if(stCcInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_CiPlus)
	{
		switch(stCcInfo.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType)
		{
			case eDxCopyrightControl_CiPlusDrmType_Uri:
				HLIB_STD_MemCpySafe(&(pstContext->drmInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t), &(stCcInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t));
				break;
			case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:
				break;
			default:
				HxLOG_Info("CI+ DRM Type[%02x] is not handled at xmgr_live_view_base module...\n", stCcInfo.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType);
				break;
		}
	}
	else if((stCcInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_Verimatrix)
		|| (stCcInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_NagraSes))
	{
		HLIB_STD_MemCpySafe(&(pstContext->drmInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t), &(stCcInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t));
	}
	else if (stCcInfo.stCasCcInfo.eCasCcType == eDxCopyrightControl_CasCcType_Conax)
	{
		HLIB_STD_MemCpySafe(&(pstContext->drmInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t), &(stCcInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t));
	}
	else
	{
		HxLOG_Error("ERROR: DRM Type should be CI+... check it...\n");
		// HLIB_STD_MemCpySafe(&(pstContext->drmInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t), &(stCcInfo.stCasCcInfo), sizeof(DxCopyrightControl_CasCcInfo_t));
	}

	_mgr_LiveView_base_SetCopyProtection(pstContext);

	//--------------------------------------------------------
	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}
#endif	// #if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS)

BUS_Result_t XMGR_LiveView_BASE_EvtSiPmt(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				 hErr;
	HBOOL			 	 bChanged = FALSE;
	DbSvc_Info_t		*pstOrgSvcInfo = NULL, *pstDstSvcInfo = NULL;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstOrgSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));
	pstDstSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));

	if ((NULL != pstOrgSvcInfo) && (NULL != pstDstSvcInfo))
	{
		HUINT16				 usSvcId = 0;

		hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, pstOrgSvcInfo);
		{
			if (ERR_OK != hErr)
			{
				HxLOG_Error("DB_SVC_GetServiceInfo Error !\n");
			}
		}

		hErr = DB_SVC_GetServiceIdFromSvcHandle (pstContext->hSvc, &usSvcId);
		if (ERR_OK == hErr)
		{
			hErr = MGR_LIVE_UpdateServiceInfo(pstContext->hAction, (HUINT32)usSvcId, pstOrgSvcInfo, pstDstSvcInfo, &bChanged);
			HxLOG_Info("pstOrgSvcInfo!=pstDstSvcInfo,  bChanged: %d \n", bChanged);
			HxLOG_Info("org video PID: 0x%x, audio PID: 0x%x, audio Aux PID: 0x%x, usTtxPid: 0x%x,  ucSubttlIdx: 0x%x\n", DbSvc_GetVideoPid(pstOrgSvcInfo), DbSvc_GetAudioPid(pstOrgSvcInfo), DbSvc_GetAudioAuxPid(pstOrgSvcInfo), DbSvc_GetTtxPid(pstOrgSvcInfo), DbSvc_GetSubttlIdx(pstOrgSvcInfo));
			HxLOG_Info("dst video PID: 0x%x, audio PID: 0x%x, audio Aux PID: 0x%x, usTtxPid: 0x%x,  ucSubttlIdx: 0x%x\n", DbSvc_GetVideoPid(pstDstSvcInfo), DbSvc_GetAudioPid(pstDstSvcInfo), DbSvc_GetAudioAuxPid(pstDstSvcInfo), DbSvc_GetTtxPid(pstDstSvcInfo), DbSvc_GetSubttlIdx(pstDstSvcInfo));
			if (DbSvc_GetVideoPid(pstDstSvcInfo) == PID_NULL)
			{
				/* ignore codec-changed caused by removal of video pid  */
				DbSvc_SetVideoCodec(pstDstSvcInfo, DbSvc_GetVideoCodec(pstOrgSvcInfo));
			}
			if (ERR_OK == hErr)
			{
				if (TRUE == bChanged)
				{
					HxLOG_Info("update svc info \n");
					DB_SVC_UpdateServiceInfo (pstContext->hSvc, pstDstSvcInfo);
					BUS_PostMessage (NULL, eSEVT_DB_CHECK_SVC_DB, HANDLE_NULL, eCheckDbType_SvcInfoUpdated, 0, 0);
				}
			}
			else
			{
				HxLOG_Error ("MGR_LIVE_UpdateServiceInfo err: usSvcId(%d)\n");
			}
		}
		else
		{
			HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle() err \n");
		}
	}

	if (NULL != pstOrgSvcInfo)				{ OxMGR_Free (pstOrgSvcInfo); }
	if (NULL != pstDstSvcInfo)				{ OxMGR_Free (pstDstSvcInfo); }

	if(pstContext->bPmtValid != TRUE)
	{ /* 첫 번째 PMT들어오는 경우 처리... */
		/* 먼저 PMT valid 표시를 해야 subtitle을 시작할 수 있다. */
		pstContext->bPmtValid = TRUE;
	}
	else
	{
	}

	HxLOG_Print("eAppState(%s) \n", _mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));

	if (pstContext->eAppState == eLiveViewState_EMPTY_SVC)
	{
		/* 채널이 서비스 중단 상태에 있다가 다시 시작된 경우임. */
		hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &pstContext->stSvcInfo); /* New PMT 반영된 service info 다시 읽음. */
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
		}

		if(pstContext->eAppState < eLiveViewState_PID_START)
		{ /* PID start 전이면 PID start 로 진행 */
			_mgr_LiveView_base_StartMwServices(pstContext , FALSE);
		}
	}
	else if (pstContext->eAppState == eLiveViewState_PMT_PID_CHANGED)
	{
		/* PMT를 기다렸다가 진행해야 하는 경우에 걸려 있다. PMT를 받았으니 이제 진행하면 됨. */
		hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &pstContext->stSvcInfo); /* New PMT 반영된 service info 다시 읽음. */
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
		}

		if(pstContext->eAppState < eLiveViewState_PID_START)
		{ /* PID start 전이면 PID start 로 진행 */
			_mgr_LiveView_base_StartMwServices(pstContext, FALSE);
		}
	}
	else
	{
		/* AV/Teletext PID 변경 검사. pstContext->svcInfo를 먼저 update하면 안됨. 그러면 변경 확인이 안되니 변경 확인 후 update하도록 해야 함. */
		/* DB의 AV PID 변경을 메시지로 처리함. */
		_mgr_LiveView_base_ChangePid_SvcDb(pstContext);
		hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &pstContext->stSvcInfo); /* New PMT 반영된 service info 다시 읽음. */
		HxLOG_Info("new video pid: 0x%x, new audio pid: 0x%x new Aux Pid : 0x%x,  New DualMono : %d\n",
			DbSvc_GetVideoPid(&pstContext->stSvcInfo), DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo), DbSvc_GetAudioPid(&pstContext->stSvcInfo), DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo));
		if (hErr != ERR_OK)
		{
			HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
		}

		/**
		  *	transcoder 는 PMT 이후 진행 되야함
		**/
		if (pstContext->eAppState >= eLiveViewState_PID_START)
		{
			_mgr_LiveView_base_StartMwServices(pstContext, FALSE);
		}

#if defined(CONFIG_MW_SI_PMT_RATING)
		if (pstContext->eAppState > eLiveViewState_PID_START)
		{ /* PID 시작 이후에서는 rating check 한번 돌림. */
			_mgr_LiveView_base_ShowProgram(pstContext);
		}
#endif
	}

	/* PID말고 다른 처리할 것이 있으면 여기다 하라. */
	_mgr_LiveView_base_CheckDolbyLed(pstContext);

	/* Last Channel 을 백업한다. 채널 변경 직후가 아니라 PMT 들어오고 나서 백업한다.
	사용자가 PMT도 받기전에 채널 전환을 하는 경우는 skip될 것이다. */
	_mgr_LiveView_base_BackUpCurrentService(pstContext);

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtSiPmtTimeout(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if (pstContext->eAppState == eLiveViewState_NO_SIGNAL)
	{
		HxLOG_Print("[AP:LiveView(%d)] \n\nno signal!! --> Ignore PMT TImeout\n\n",ulActionId);
		return ERR_FAIL;
	}

	_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_EMPTY_SVC);

	/* Last Channel 을 백업한다. 채널 변경 직후가 아니라 PMT 들어오고 나서 백업하려고 했는데,
	PMT가 안들어오면 아예 백업이 안되므로 PMT timeout이 나면 일단 PMT 입력때와 마찬가지로 백업한다. */
	_mgr_LiveView_base_BackUpCurrentService(pstContext);

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtSiPmtRemoved(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DbSvc_Info_t		*pstSvcInfo = &(pstContext->stSvcInfo);

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if (pstContext->eAppState == eLiveViewState_NO_SIGNAL)
	{
		HxLOG_Print("[AP:LiveView(%d)] \n\n no signal!! --> Ignore PMT Removed\n\n",ulActionId);
		return ERR_FAIL;
	}

	DbSvc_SetPmtPid(pstSvcInfo, PID_NULL);

	_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_EMPTY_SVC);

	/* Last Channel 을 백업한다. 채널 변경 직후가 아니라 PMT 들어오고 나서 백업하려고 했는데,
	PMT가 안들어오면 아예 백업이 안되므로 PMT timeout이 나면 일단 PMT 입력때와 마찬가지로 백업한다. */
	_mgr_LiveView_base_BackUpCurrentService(pstContext);

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtSiPmtChanged(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT16				 usPmtPid = PID_NULL;
	HERROR				 hErr = ERR_FAIL;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	usPmtPid = DbSvc_GetPmtPid(&pstContext->stSvcInfo);

	hErr = SVC_SI_GetPmtPidBySvcId(pstContext->hAction, DbSvc_GetSvcId(&pstContext->stSvcInfo), &usPmtPid);
	if ((ERR_OK == hErr) && (DbSvc_GetPmtPid(&pstContext->stSvcInfo) != usPmtPid))
	{
		DbSvc_Info_t		*pstSvcInfo = &(pstContext->stSvcInfo);

		DbSvc_SetPmtPid(pstSvcInfo, usPmtPid);
		DbSvc_SetPcrPid(pstSvcInfo, PID_NULL);
		DbSvc_SetTtxPid(pstSvcInfo, PID_NULL);
		DbSvc_SetVideoPid(pstSvcInfo, PID_NULL);
		DbSvc_SetAudioPid(pstSvcInfo, PID_NULL);
		DbSvc_SetDolbyPid(pstSvcInfo, PID_NULL);
		DbSvc_SetAudioAuxPid(pstSvcInfo, PID_NULL);

		hErr = DB_SVC_UpdateServiceInfo(pstContext->hSvc, pstSvcInfo);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("DB_SVC_UpdateServiceInfo failed: hSvc(0x%08x)\n", pstContext->hSvc);
		}

		_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_PMT_PID_CHANGED);

	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtSiSdt(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		 hResult = ERR_BUS_INITIAL_FAILED;
	HERROR				 hErr = ERR_FAIL;
	DbSvc_Info_t		*pstOrgSvcInfo = NULL, *pstDstSvcInfo = NULL;
	HUINT16 			 usSvcId = 0;
	HBOOL				 bChanged = FALSE;
	HBOOL				 bChangedOthers = FALSE;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstOrgSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));
	pstDstSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));
	if ((pstOrgSvcInfo == NULL) || (pstDstSvcInfo == NULL))
	{
		HxLOG_Error("mem alloc failed.. \n");
		goto END_FUNC;
	}

#if defined(CONFIG_MW_SI_UPDATE_ALLSDT)
	SVC_SI_UpdateAllService_inSDT(ulActionId, &(pstContext->stSvcInfo), &bChangedOthers);
#endif

	HxSTD_MemCopy (pstOrgSvcInfo, &(pstContext->stSvcInfo), sizeof(DbSvc_Info_t));

	hErr = DB_SVC_GetServiceIdFromSvcHandle(pstContext->hSvc, &usSvcId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle() failed.. \n");
		goto END_FUNC;
	}

	hErr = SVC_SI_GetLiveServiceInfo(pstContext->hAction, (HUINT32)usSvcId, pstOrgSvcInfo, pstDstSvcInfo, &bChanged);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SVC_SI_GetLiveServiceInfo() failed.. \n");
		goto END_FUNC;
	}

	if (TRUE == bChanged)
	{
		HxLOG_Debug("onId (0x%X -> 0x%X) tsId(0x%X -> 0x%X) svcName(%s -> %s) \n",
																				DbSvc_GetOnId(pstOrgSvcInfo), DbSvc_GetOnId(pstDstSvcInfo),
																				DbSvc_GetTsId(pstOrgSvcInfo), DbSvc_GetTsId(pstDstSvcInfo),
																				DbSvc_GetName(pstOrgSvcInfo), DbSvc_GetName(pstDstSvcInfo));

		hErr = DB_SVC_UpdateServiceInfo (pstContext->hSvc, pstDstSvcInfo);

		BUS_PostMessage (NULL, eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);

#if 0
		hErr |=DB_SVC_Sync(eSyncMode);
		if(hErr == ERR_OK)
		{
			// 여기서 아래 msg를 날리면 ui상 svc name이 변경은 되는데 first svc로 zapping된다.
			// 안 날리면 zapping은 안 되지만 ui상 svc name이 변경되지 않는다..
			//BUS_PostMessage (NULL, eSEVT_DB_CHECK_SVC_DB, HANDLE_NULL, eCheckDbType_SvcNameUpdated, 0, 0);
		}
#endif
	}
	else if (TRUE == bChangedOthers)
	{
		BUS_PostMessage (NULL, eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);
	}

	hResult = ERR_BUS_NO_ERROR;

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);

END_FUNC:
	if (pstOrgSvcInfo != NULL)		{ OxMGR_Free(pstOrgSvcInfo); }
	if (pstDstSvcInfo != NULL)		{ OxMGR_Free(pstDstSvcInfo); }

	return hResult;

}

BUS_Result_t XMGR_LiveView_BASE_EvtSiNit(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DbSvc_Info_t	stSvcInfo ;

	HxSTD_memset(&stSvcInfo, 0, sizeof(DbSvc_Info_t));

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if	defined(CONFIG_MW_SI_HOME_CHANNEL)
	if(DB_SVC_GetServiceInfo(pstContext->hSvc, &stSvcInfo) == ERR_OK)
	{
		SVC_SI_HomeChannelProcessNit(pstContext->hAction, DbSvc_GetDeliType(stSvcInfo), DbSvc_GetTsIdx(&stSvcInfo), ACT_HEAD);
	}
#endif
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);

	return ERR_BUS_NO_ERROR;

}

BUS_Result_t XMGR_LiveView_BASE_EvtSiServiceMove(XmgrLiveView_Context_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	Handle_t		 hOrgSvc  = (Handle_t)p1;
	Handle_t		 hNextSvc = (Handle_t)p2;
	BUS_Result_t	 result;

	(void)hOrgSvc;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(pstContext->ulActionId != MGR_ACTION_GetMainActionId())
	{ /* 이 메시지는 main view action이 아니면 처리하지 않는다. */
		result = ERR_BUS_NO_ERROR;
	}
	else
	{
		XMGR_LiveView_BASE_MsgLiveStartSvc(pstContext, hAction, (HINT32)hNextSvc, 0, 0);
		result = ERR_BUS_NO_ERROR;
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, hAction, p1, p2, p3);
	return result;
}

#if defined(CONFIG_OP_UK_DTT)
BUS_Result_t XMGR_LiveView_BASE_EvtSiEit(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				 hErr;
	HBOOL			 	 bChanged = FALSE;
	DbSvc_Info_t		*pstOrgSvcInfo = NULL, *pstDstSvcInfo = NULL;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstOrgSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));
	pstDstSvcInfo = (DbSvc_Info_t *)OxMGR_Malloc (sizeof(DbSvc_Info_t));

	if ((NULL != pstOrgSvcInfo) && (NULL != pstDstSvcInfo))
	{
		HUINT16				 usSvcId = 0;

		HxSTD_MemCopy (pstOrgSvcInfo, &(pstContext->stSvcInfo), sizeof(DbSvc_Info_t));

		hErr = DB_SVC_GetServiceIdFromSvcHandle (pstContext->hSvc, &usSvcId);
		if (ERR_OK == hErr)
		{
			hErr = MGR_LIVE_UpdateServiceInfo(pstContext->hAction, (HUINT32)usSvcId, pstOrgSvcInfo, pstDstSvcInfo, &bChanged);
			HxLOG_Info("pstOrgSvcInfo!=pstDstSvcInfo,  bChanged: %d \n", bChanged);
			HxLOG_Info("org video PID: 0x%x, audio PID: 0x%x, usTtxPid: 0x%x,  ucSubttlIdx: 0x%x\n", DbSvc_GetVideoPid(pstOrgSvcInfo), DbSvc_GetAudioPid(pstOrgSvcInfo), DbSvc_GetTtxPid(pstOrgSvcInfo), DbSvc_GetSubttlIdx(pstOrgSvcInfo));
			HxLOG_Info("dst video PID: 0x%x, audio PID: 0x%x, usTtxPid: 0x%x,  ucSubttlIdx: 0x%x\n", DbSvc_GetVideoPid(pstDstSvcInfo), DbSvc_GetAudioPid(pstDstSvcInfo), DbSvc_GetTtxPid(pstDstSvcInfo), DbSvc_GetSubttlIdx(pstDstSvcInfo));
			if (DbSvc_GetVideoPid(pstDstSvcInfo) == PID_NULL)
			{
				/* ignore codec-changed caused by removal of video pid  */
				DbSvc_SetVideoCodec(pstDstSvcInfo, DbSvc_GetVideoCodec(pstOrgSvcInfo));
			}

			if (ERR_OK == hErr)
			{
				if (TRUE == bChanged)
				{
					HxLOG_Info("update svc info \n");
					hErr = DB_SVC_UpdateServiceInfo (pstContext->hSvc, pstDstSvcInfo);
					BUS_PostMessage (NULL, eSEVT_DB_CHECK_SVC_DB, HANDLE_NULL, eCheckDbType_SvcInfoUpdated, 0, 0);

					hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &pstContext->stSvcInfo); /* New EIT 반영된 service info 다시 읽음. */
					if (hErr != ERR_OK)
					{
						HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
					}

					if(hErr == ERR_OK)
					{
						// prevent 200099
						hErr = SVC_AV_SetDualMonoSelect(ulActionId, DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo));
						if (hErr)
						{
							HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
						}
					}

#if 0
					_mgr_LiveView_base_ChangePid_SvcDb(pstContext);
					HxLOG_Info("new video pid: 0x%x, new audio pid: 0x%x new Aux Pid : 0x%x, New DualMono : %d \n",
						DbSvc_GetVideoPid(&pstContext->stSvcInfo), DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo), DbSvc_GetAudioPid(&pstContext->stSvcInfo), DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo));
					if (hErr != ERR_OK)
					{
						HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
					}
#endif
				}
			}
			else
			{
				HxLOG_Error ("MGR_LIVE_UpdateServiceInfo err: usSvcId(%d)\n");
			}
		}
		else
		{
			HxLOG_Error("DB_SVC_GetServiceIdFromSvcHandle() err \n");
		}
	}

	if (NULL != pstOrgSvcInfo)				{ OxMGR_Free (pstOrgSvcInfo); }
	if (NULL != pstDstSvcInfo)				{ OxMGR_Free (pstDstSvcInfo); }

 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}
#endif

BUS_Result_t XMGR_LiveView_BASE_EvtAvVideoUnderrun(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(DbSvc_GetVideoPid(&pstContext->stSvcInfo) < PID_NULL)
	{
		/* video PID가 valid일 때만 반응  */
		pstContext->bAvUnderrun = TRUE;

		/* AV state에서는 UI를 그림 */
		if(pstContext->eAppState == eLiveViewState_AV)
		{
			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_AV_UNDER_RUN);
		}
	}
	else
	{
		HxLOG_Print ("[AP:LiveView(%d)] \t=> Unexpected message in this state(%s). Ignore this message ...\n", ulActionId,
											_mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtAvVideoStarted(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(DbSvc_GetVideoPid(&pstContext->stSvcInfo) < PID_NULL)
	{ /* video PID가 valid일 때만 반응  */
		VIDEO_Timing_t stVideoSourceTiming;
		HxSTD_MemSet(&stVideoSourceTiming, 0, sizeof(VIDEO_Timing_t));
		if(SVC_AV_GetVideoSourceTiming(ulActionId, &stVideoSourceTiming) == ERR_OK)
		{
			if(stVideoSourceTiming.eResolution >= eDxRESOLUTION_STATUS_576P)
			{
				_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_HD, TRUE);
			}
			else
			{
				_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_HD, FALSE);
			}

			if(stVideoSourceTiming.eResolution == eDxRESOLUTION_STATUS_1080P)
			{
				_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_1080P, TRUE);
			}
			else
			{
				_mgr_LiveView_base_SetLedOnOff(pstContext->ulActionId, eDxLED_ID_1080P, FALSE);
			}
		}

		pstContext->bAvUnderrun = FALSE;

  		if( (pstContext->eAppState == eLiveViewState_AV) && (pstContext->eUiState == eLiveViewUi_AV_UNDER_RUN) )
		{ /* AV state에서 UNDERRUN UI에서만 반응. */
			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_OK);
		}
	}
	else
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> Unexpected message in this state(%s). Ignore this message ...\n", ulActionId,
											_mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	}


	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtAvAudioUnderrun(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if( (DbSvc_GetVideoPid(&pstContext->stSvcInfo) >= PID_NULL) && (DbSvc_GetAudioPid(&pstContext->stSvcInfo) < PID_NULL) )
	{
		/* radio PID만 있을 때만 반응 */
		pstContext->bAvUnderrun = TRUE;

		/* AV state에서는 UI를 그림  */
		if(pstContext->eAppState == eLiveViewState_AV)
		{
			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_AV_UNDER_RUN);
		}
	}
	else
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> Unexpected message in this state(%s). Ignore this message ...\n", ulActionId,
											_mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtAvAudioStarted(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if( (DbSvc_GetVideoPid(&pstContext->stSvcInfo) >= PID_NULL) && (DbSvc_GetAudioPid(&pstContext->stSvcInfo) < PID_NULL) )
	{
		/* radio PID만 있을 때만 반응 */
		pstContext->bAvUnderrun = FALSE;

		if( (pstContext->eAppState == eLiveViewState_AV) && (pstContext->eUiState == eLiveViewUi_AV_UNDER_RUN) )
		{ /* AV state에서 UNDERRUN UI에서만 반응. */
			_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_OK);
		}
	}
	else
	{
		HxLOG_Print("[AP:LiveView(%d)] \t=> Unexpected message in this state(%s). Ignore this message ...\n", ulActionId,
											_mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtDbCheckSvcDb(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32				nSvcCnt;
	Handle_t				hNewSvc = HANDLE_NULL;
	Handle_t				hMasterSvc = _mgr_LiveView_base_GetMasterSvcHandle(pstContext);
	Handle_t				hWatchingSvc = HANDLE_NULL;
	HERROR			hErr;
	DbSvc_Info_t			stNewSvcInfo;
	DbCheckType_e		eCheckDbType = eCheckDbType_Nothing;
	DxDeliveryType_e		eOrgDeliType;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	nSvcCnt = MGR_SVCLIST_CountSvcInDB(MGR_SVCLIST_GetSvcTypeForListCount());
	if(nSvcCnt == 0)
	{ /* Service DB was deleted. Release action and wait for the action manager's treatment. */
		HxLOG_Print("[AP:LiveView(%d)] \t==> No service found in DB. Quit WTV action.\n", pstContext->ulActionId);
		BUS_MGR_Destroy(0);
	}
	else
	{ /* Check if there is any change for my current service handle. */
		DbSvc_Info_t	*pstSvcInfo = _mgr_LiveView_base_GetMasterSvcInfoPtr(pstContext);

		/* 1. 현재 서비스의 handle이 변경되었는지 검사. */
		hErr = DB_SVC_GetServiceHandle(DbSvc_GetSvcSection(&(pstContext->stSvcInfo)), DbSvc_GetTsIdx(&pstContext->stSvcInfo), DbSvc_GetSvcId(&pstContext->stSvcInfo), &hNewSvc);

		/* Change watching service handle to supplement service if view session is quiet tune */
		hWatchingSvc = hMasterSvc;
		if(pstContext->sessionType == eLiveViewSession_MHEG_QUIET)
			hWatchingSvc = pstContext->hSuppleSvc;

		if( (hErr == ERR_OK) && (hWatchingSvc != hNewSvc) )
		{ /* 현재 보고 있는 service의 service handle이 edit등에 의하여 변경됨. 새 handle로 다시 zapping. */
			HxLOG_Print("[AP:LiveView(%d)] \t==> service handle is changed. Re-tune to the same service.\n", pstContext->ulActionId);

			MGR_LIVE_ResetZappingInfo(FALSE);
			MGR_LIVE_CmdSendServiceStartByActionId(pstContext->ulActionId, hNewSvc);

			return ERR_BUS_NO_ERROR;
		}
		else
		{
			DbSvc_TsInfo_t	stNewTsInfo;

			/* svcHandle은 동일하고 tuning param이 변경되었나 확인 */
			hErr = DB_SVC_GetTsInfoByServiceHandle(hWatchingSvc, &stNewTsInfo);
			if (hErr == ERR_OK)
			{
				HINT32 lDiff = 0;

				lDiff = DB_SVC_UtilCompareDbTuningParam((HUINT32)pstContext->tsInfo.eDeliType, &pstContext->tsInfo.stTuningParam, &stNewTsInfo.stTuningParam);
				if (lDiff != 0)
				{
					MGR_LIVE_ResetZappingInfo(FALSE);
					MGR_LIVE_CmdSendServiceStartByActionId(pstContext->ulActionId, hNewSvc);

					return ERR_BUS_NO_ERROR;
				}
			}


		}

		/* 2. Master handle delete 검사. */
		hErr = DB_SVC_CheckValidServiceHandle(hMasterSvc);
		if(hErr == ERR_OK)
		{ /* Service handle 이상 없음. */
			/* 2.1 Master service 자체의 변화는 없지만, svc name 등 정보는 변경되었을 가능성이 있다. svc info는 새로 update 하자... */
			HxLOG_Print("[AP:LiveView(%d)] \t==> Nothing happened for my MASTER service handle.\n",pstContext->ulActionId);

			HxSTD_memset(&stNewSvcInfo, 0, sizeof(DbSvc_Info_t));
			hErr = DB_SVC_GetServiceInfo(hMasterSvc, &stNewSvcInfo);
			if (hErr != ERR_OK)
			{
				HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", hMasterSvc);
			}

			/*
				Master Service 는 살아 있으나 Master를 참조하는 FAV, RECENT, HD, 등에 대해서는 채널 리스트 삭제시 커서가 맨처름으로 가는 문제 또한 그릅이 꼬인다.
				일부 Type만 적용 되어 있음 모델에 따라서 추가 필요. 현재 (FAV, RECENT) 적용.
			*/
			eCheckDbType = (DbCheckType_e)p1;
			// zapping이 필요한 경우에만 p2에 next svc handle이 들어온다.
			if (eCheckDbType == eCheckDbType_SvcDeleted && p2 != HANDLE_NULL && p2 != 0)
			{
				eOrgDeliType = DbSvc_GetDeliTypePtr(pstSvcInfo);
				hErr = _mgr_LiveView_base_EvtDbCheckSvcDbByDeleteSvc(pstContext->ulActionId, pstContext->svcListGroup, eOrgDeliType, (Handle_t)p2);
				if (hErr == ERR_FAIL)
				{
					_mgr_LiveView_base_EvtDbCheckSvcDbChgFrontPanel(pstContext, &stNewSvcInfo);
				}
			}
			else
			{
				_mgr_LiveView_base_EvtDbCheckSvcDbChgFrontPanel(pstContext, &stNewSvcInfo);
			}

			/* 2.2 master handle이 이상이 없으면 2차로 sup service handle 검사. */
			if(pstContext->sessionType != eLiveViewSession_NORMAL)
			{
				HxLOG_Print ("[AP:LiveView(%d)] \t==> Additional check for the supplementary service handle.\n",pstContext->ulActionId);

				hErr = DB_SVC_CheckValidServiceHandle(pstContext->hSvc);
				if(hErr == ERR_OK)
				{ /* Service 자체의 변화는 없지만, svc name 등 정보는 변경되었을 가능성이 있다. svc info는 새로 update 하자... */
					HxLOG_Print	("[AP:LiveView(%d)] \t==> Nothing happened for my SUP service handle, but update svc info to cover any change in it.\n",pstContext->ulActionId);

					hErr = DB_SVC_GetServiceInfo(pstContext->hSvc, &pstContext->stSvcInfo);
					if (hErr != ERR_OK)
					{
						HxLOG_Critical("DB_SVC_GetServiceInfo error Handle(0x%x)\r\n", pstContext->hSvc);
					}

					#if 0
					DB_SVC_GetServiceNumber(pstContext->hSvc, &pstContext->nChNum);
					#else
					pstContext->nChNum = DbSvc_GetLcn(&pstContext->stSvcInfo);
					#endif
				}
				else
				{
					/* Service 없어짐. Sup session 에서는 master로 돌아간다. */
					MgrAction_Priority_e		 eActPriority;

					if (eMgrLiveMode_LiveStreaming == pstContext->eLiveMode)
					{
						DxLiveStreamPriority_e		 eLsPriority = eDxLsPriority_EQUAL_REC;

						eActPriority = eAmPriority_RECORD;

						hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LIVESTREAM_PRIORITY, (HUINT32 *)&eLsPriority, 0);
						if (ERR_OK == hErr)
						{
							switch (eLsPriority)
							{
							case eDxLsPriority_EQUAL_REC:
								eActPriority = eAmPriority_RECORD;
								break;

							case eDxLsPriority_EQUAL_VIEW:
								eActPriority = eAmPriority_VIEW_ACTION;
								break;

							case eDxLsPriority_BELOW_VIEW:
								eActPriority = (eAmPriority_VIEW_ACTION - (1<<ACTION_PRIORITY_BIT_OFFSET));
								break;

							case eDxLsPriority_UPPER_REC:
								eActPriority = (eAmPriority_RECORD + (1<<ACTION_PRIORITY_BIT_OFFSET));
								break;

							default:
								break;
							}
						}
					}
					else
					{
						eActPriority = eAmPriority_VIEW_ACTION;
					}

					hErr = _mgr_LiveView_base_SetupAction(ulActionId, pstContext->hMasterSvc, eActPriority, pstContext->eLiveMode);
					if(hErr == ERR_OK)
					{
						/* ERR_OK를 받아야만 이동하고 못 받으면 현재 recording 서비스로 이동한다. */
						hErr = _mgr_LiveView_base_StartMasterSession(pstContext, pstContext->hMasterSvc, pstContext->svcListGroup, eNO_IPLATE|eKEEP_PINPASS);
					}
				}
			}
		}
		else
		{ /* Can't find the service handle. My service handle must have been deleted from the service DB.
			Go to the last DTV channel in my current delivery type. */
			ChListUiGroup_t	chListUiGroup;
			DbSvc_Group_e 	svcGroup;
			DbSvc_Attr_t		svcAttr;

			if(DbSvc_GetSvcType(pstSvcInfo) == eDxSVC_TYPE_RADIO)
			{
				chListUiGroup = eChListUiGroup_RADIO;
			}
			else
			{
				chListUiGroup = eChListUiGroup_TV;
			}

			MGR_SVCLIST_GetSvcDbParamsFromGroup(chListUiGroup, &svcGroup, &svcAttr);

			svcAttr.eOrgDeliType = DbSvc_GetDeliTypePtr(pstSvcInfo);
			hErr = DB_SVC_FindServiceHandle(svcGroup, &svcAttr, 0, eSvcFindDirection_First, &hNewSvc);
			if(hErr == ERR_OK)
			{ /* delivery type 까지 맞춰본다. */
				HxLOG_Print("[AP:LiveView(%d)] \t==> My service handle was deleted. Re-tune to the 1st service of %s of MY delivery type.\n", pstContext->ulActionId,
															xmgr_live_GetSvcListGroupStr(chListUiGroup) );
				MGR_LIVE_CmdSendServiceStartByActionId(pstContext->ulActionId, hNewSvc);
			}
			else
			{ /* delivery type 포기하고 chlist ui group에서 찾아본다. */
				MGR_SVCLIST_GetSvcDbParamsFromGroup(chListUiGroup, &svcGroup, &svcAttr);

				hErr = DB_SVC_FindServiceHandle(svcGroup, &svcAttr, 0, eSvcFindDirection_First, &hNewSvc);
				if(hErr == ERR_OK)
				{
					HxLOG_Print ("[AP:LiveView(%d)] \t==> My service handle was deleted. Re-tune to the 1st service of %s of MY Service type.\n",pstContext->ulActionId,
																xmgr_live_GetSvcListGroupStr(chListUiGroup) );
					MGR_LIVE_CmdSendServiceStartByActionId(pstContext->ulActionId, hNewSvc);
				}
				else
				{ /* Channel list ui group 포기하고 전체에서 다시 찾아본다. */
					MGR_SVCLIST_GetSvcDbParamsFromGroup(eChListUiGroup_All, &svcGroup, &svcAttr);

					hErr = DB_SVC_FindServiceHandle(svcGroup, &svcAttr, 0, eSvcFindDirection_First, &hNewSvc);
					if(hErr == ERR_OK)
					{
						HxLOG_Print("[AP:LiveView(%d)] \t==> My service handle was deleted. Re-tune to the 1st service of %s of ALL delivery type.\n", pstContext->ulActionId,
																	xmgr_live_GetSvcListGroupStr(eChListUiGroup_All));
						MGR_LIVE_CmdSendServiceStartByActionId(pstContext->ulActionId, hNewSvc);
					}
					else
					{
						HxLOG_Print("[AP:LiveView(%d)] \t==> No Radio? No TV? Where should I go???.\n", pstContext->ulActionId);
					}
				}
			}
		}
	}

 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_MsgMgrSvcPidChanged(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	_mgr_LiveView_base_ChangePid_SvcDb(pstContext);
	_mgr_LiveView_base_CheckDolbyLed(pstContext);

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_MsgMgrChangeTempPid(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR			hErr;
	DxVideoCodec_e	eVideoCodec;
	DxAudioCodec_e	eAudioCodec;
	HUINT16			usVideoPid, usAudioPid, usPcrPid;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	eVideoCodec = (p1 & 0xffff0000) >> 16;
	eAudioCodec = (p2 & 0xffff0000) >> 16;

	usVideoPid = (HUINT16)(p1 & 0xffff);
	usAudioPid = (HUINT16)(p2 & 0xffff);
	usPcrPid = (HUINT16)(p3 & 0xffff);
#if 0
	if (eAudioCodec != CODEC_CURRENT)
	{
		// Audio Codec이 CODEC_CURRENT 인 경우에는 기존의 Dolby Flag 값을 사용
		if (eAudioCodec == eDxAUDIO_CODEC_DOLBY_AC3 || eAudioCodec == eDxAUDIO_CODEC_DOLBY_AC3P)
		{
			DbSvc_SetDolbyFlag(&pstContext->stSvcInfo, TRUE);
		}
		else
		{
			DbSvc_SetDolbyFlag(&pstContext->stSvcInfo, FALSE);
		}
	}

	if(DbSvc_GetDolbyFlag(&pstContext->stSvcInfo) == TRUE)
	{
		usDolbyPid = (HUINT16)(p2 & 0xffff);
		usAudioPid = PID_CURRENT;
	}
	else
	{
		usDolbyPid = PID_CURRENT;
		usAudioPid = (HUINT16)(p2 & 0xffff);
	}

	usPcrPid = (HUINT16)(p3 & 0xffff);

	HxLOG_Print("\t=> Temp Video Codec : 0x%x, Temp Video PID : 0x%x\n", eVideoCodec, usVideoPid);
	HxLOG_Print("\t=> Temp Audio Codec : 0x%x, Temp Audio PID : 0x%x\n", eAudioCodec, usAudioPid);
	HxLOG_Print("\t=> Temp PCR PID : 0x%x, Temp Dolby PID : 0x%x\n", usPcrPid, usDolbyPid);
	HxLOG_Print("\t=> pstContext->stSvcInfo.eDolbyCodec : 0x%x, pstContext->eOrgDolbyCodec : 0x%x\n", DbSvc_GetDolbyCodec(&pstContext->stSvcInfo), pstContext->eOrgDolbyCodec);
	HxLOG_Print("\t=> pstContext->stSvcInfo.eAudioCodec : 0x%x, pstContext->eOrgAudioCodec : 0x%x\n", DbSvc_GetAudioCodec(&pstContext->stSvcInfo), pstContext->eOrgAudioCodec);
	HxLOG_Print("\t=> pstContext->stSvcInfo.usDolbyPid : 0x%x, pstContext->usOrgDolbyPid : 0x%x, pstContext->stSvcInfo.bDolbyFlag : %d\n", DbSvc_GetDolbyPid(&pstContext->stSvcInfo), pstContext->usOrgDolbyPid, DbSvc_GetDolbyFlag(&pstContext->stSvcInfo));
	HxLOG_Print("\t=> pstContext->stSvcInfo.usAudioPid : 0x%x, pstContext->usOrgAudioPid : 0x%x\n", DbSvc_GetAudioPid(&pstContext->stSvcInfo), pstContext->usOrgAudioPid);
	HxLOG_Print("\t=> pstContext->stSvcInfo.usVideoPid : 0x%x, pstContext->usOrgVideoPid : 0x%x\n", DbSvc_GetVideoPid(&pstContext->stSvcInfo), pstContext->usOrgVideoPid);
	HxLOG_Print("\t=> pstContext->stSvcInfo.usPcrPid : 0x%x, pstContext->usOrgPcrPid : 0x%x\n", DbSvc_GetPcrPid(&pstContext->stSvcInfo), pstContext->usOrgPcrPid);

	if (eAudioCodec != CODEC_CURRENT)
	{
		if (DbSvc_GetDolbyFlag(&pstContext->stSvcInfo) == TRUE)
		{
			if (eAudioCodec != DbSvc_GetDolbyCodec(&pstContext->stSvcInfo))
			{
				if (pstContext->eOrgDolbyCodec == eDxAUDIO_CODEC_MAX_NUM)
				{
					pstContext->eOrgDolbyCodec = DbSvc_GetDolbyCodec(&pstContext->stSvcInfo);
				}
				DbSvc_SetDolbyCodec(&pstContext->stSvcInfo, eAudioCodec);	/* 새 Codec으로 갱신 */
			}
		}
		else
		{
			if (eAudioCodec != DbSvc_GetAudioCodec(&pstContext->stSvcInfo))
			{
				if (pstContext->eOrgAudioCodec == eDxAUDIO_CODEC_MAX_NUM)
				{
					pstContext->eOrgAudioCodec = DbSvc_GetAudioCodec(&pstContext->stSvcInfo);
				}
				DbSvc_SetAudioCodec(&pstContext->stSvcInfo, eAudioCodec);	/* 새 Codec으로 갱신 */
			}
		}
	}

	if (DbSvc_GetDolbyFlag(&pstContext->stSvcInfo) == TRUE)
	{
		if (usDolbyPid != PID_CURRENT)
		{
			if (usDolbyPid != DbSvc_GetDolbyPid(&pstContext->stSvcInfo))
			{
				if (pstContext->usOrgDolbyPid == 0)
				{
					pstContext->usOrgDolbyPid = DbSvc_GetDolbyPid(&pstContext->stSvcInfo);
				}
				DbSvc_SetDolbyPid(&pstContext->stSvcInfo, usDolbyPid);	/* 새 PID로 갱신 */
			}
		}
	}
	else
	{
		if (usAudioPid != PID_CURRENT)
		{
			if (usAudioPid != DbSvc_GetAudioPid(&pstContext->stSvcInfo))
			{
				if (pstContext->usOrgAudioPid == 0)
				{
					pstContext->usOrgAudioPid = DbSvc_GetAudioPid(&pstContext->stSvcInfo);
				}
				DbSvc_SetAudioPid(&pstContext->stSvcInfo, usAudioPid);	/* 새 PID로 갱신 */
			}
		}
	}

	if (eVideoCodec != CODEC_CURRENT)
	{
		if (eVideoCodec != DbSvc_GetVideoCodec(&pstContext->stSvcInfo))
		{
			if (pstContext->eOrgVideoCodec == eDxVIDEO_CODEC_MAX_NUM)
			{
				pstContext->eOrgVideoCodec = DbSvc_GetVideoCodec(&pstContext->stSvcInfo);
			}
			DbSvc_SetVideoCodec(&pstContext->stSvcInfo, eVideoCodec);	/* 새 Codec으로 갱신 */
		}
	}

	if (usVideoPid != PID_CURRENT)
	{
		if (usVideoPid != DbSvc_GetVideoPid(&pstContext->stSvcInfo))
		{
			if (pstContext->usOrgVideoPid == 0)
			{
				pstContext->usOrgVideoPid = DbSvc_GetVideoPid(&pstContext->stSvcInfo);
			}
			DbSvc_SetVideoPid(&pstContext->stSvcInfo, usVideoPid);	/* 새 PID로 갱신 */
		}
	}

	if (usPcrPid != PID_CURRENT)
	{
		if (usPcrPid != DbSvc_GetPcrPid(&pstContext->stSvcInfo))
		{
			if (pstContext->usOrgPcrPid == 0)
			{
				pstContext->usOrgPcrPid = DbSvc_GetPcrPid(&pstContext->stSvcInfo);
			}
			DbSvc_SetPcrPid(&pstContext->stSvcInfo, usPcrPid);	/* 새 PID로 갱신 */
		}
	}
#endif

	if(pstContext->eAppState > eLiveViewState_PID_START)
	{ /* AV state 였다면 audio, video PID를 다시 내린다.
		주의할 점은 audio track 변경등은 빠른 동작으로 version cancel이 일어날 수 있으니
		변경이 없을 경우 PID_CURRENT를 내리지 말고 update된 svcInfo의 PID를 내려야 한다. */
		HxLOG_Print("\t=> ################# SVC_AV_ChangePid #################\n");
#if !defined(SvcAv_START_SERVICE_NEW)
		HxLOG_Print("\t=> pstContext->stSvcInfo.eDolbyCodec : 0x%x, pstContext->eOrgDolbyCodec : 0x%x\n", DbSvc_GetDolbyCodec(&pstContext->stSvcInfo), pstContext->eOrgDolbyCodec);
#endif
		HxLOG_Print("\t=> pstContext->stSvcInfo.eAudioCodec : 0x%x, pstContext->eOrgAudioCodec : 0x%x, pstContext->stSvcInfo.eAudioAuxCodec : 0x%x\n", DbSvc_GetAudioCodec(&pstContext->stSvcInfo), pstContext->eOrgAudioCodec, DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo));
#if !defined(SvcAv_START_SERVICE_NEW)
		HxLOG_Print("\t=> pstContext->stSvcInfo.usDolbyPid : 0x%x, pstContext->usOrgDolbyPid : 0x%x, pstContext->stSvcInfo.bDolbyFlag : %d\n", DbSvc_GetDolbyPid(&pstContext->stSvcInfo), pstContext->usOrgDolbyPid, DbSvc_GetDolbyFlag(&pstContext->stSvcInfo));
#endif
		HxLOG_Print("\t=> pstContext->stSvcInfo.usAudioPid : 0x%x, pstContext->usOrgAudioPid : 0x%x, pstContext->stSvcInfo.eSoundMode : 0x%x\n", DbSvc_GetAudioPid(&pstContext->stSvcInfo), pstContext->usOrgAudioPid, DbSvc_GetStereoSelect(&pstContext->stSvcInfo));
		HxLOG_Print("\t=> pstContext->stSvcInfo.usVideoPid : 0x%x, pstContext->usOrgVideoPid : 0x%x\n", DbSvc_GetVideoPid(&pstContext->stSvcInfo), pstContext->usOrgVideoPid);
		HxLOG_Print("\t=> pstContext->stSvcInfo.usPcrPid : 0x%x, pstContext->usOrgPcrPid : 0x%x\n", DbSvc_GetPcrPid(&pstContext->stSvcInfo), pstContext->usOrgPcrPid);

		// Action이 FCC이면 CAS를 걸지 않는다.
		if (MGR_ACTION_GetType(pstContext->ulActionId) != eViewType_FCC)
		{
#if 1
			hErr = SVC_CAS_SetPid(pstContext->hAction,
								eCAS_ActionType_Live,
								usVideoPid,
								usAudioPid,
								PID_CURRENT,
								PID_CURRENT,
								PID_CURRENT,
								PID_CURRENT);

#else
			hErr = SVC_CAS_SetPid(pstContext->hAction,
								eCAS_ActionType_Live,
								DbSvc_GetVideoPid(&pstContext->stSvcInfo),
								DbSvc_GetAudioPid(&pstContext->stSvcInfo),
								PID_CURRENT,
								PID_CURRENT,
								PID_CURRENT,
								PID_CURRENT);
#endif
			if (hErr)
			{
				HxLOG_Error("SVC_CAS_SetPid ret :0x%X\n",hErr);
			}
		}
#if 1
		if (DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo) != eDxDUALMONO_SELECT_UNKNOWN)
		{
#if !defined(SvcAv_START_SERVICE_NEW)
			hErr = SVC_AV_SetStereoSelect(pstContext->ulActionId, eDxSTEREO_FORMAT_DUALMONO);
			if (hErr)
			{
				HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
			}
#endif
			hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo));
			if (hErr)
			{
				HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
			}
		}
#if 0
		hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, eDxDUALMONO_SELECT_UNKNOWN);
		if (hErr)
		{
			HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
		}
#endif

#else
		hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo));
		if (hErr)
		{
			HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
		}
#endif
#if defined(SvcAv_START_SERVICE_NEW)
#if 1
		hErr = SVC_AV_ChangePid(pstContext->ulActionId,
								usAudioPid,
								PID_CURRENT,
								usVideoPid,
								usPcrPid,
								eAudioCodec,
								CODEC_CURRENT,
								eVideoCodec
								);

#else
		hErr = SVC_AV_ChangePid(pstContext->ulActionId,
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetAudioPid(pstContext, PID_CURRENT),
								_mgr_LiveView_base_GetVideoPid(pstContext, DbSvc_GetVideoPid(&pstContext->stSvcInfo)),
								DbSvc_GetPcrPid(&pstContext->stSvcInfo),
								DbSvc_GetAudioCodec(&pstContext->stSvcInfo),
								DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo),
								DbSvc_GetVideoCodec(&pstContext->stSvcInfo)
								);
#endif
#else
		hErr = SVC_AV_ChangePid(pstContext->ulActionId,
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetAudioPid(pstContext, PID_CURRENT),
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetDolbyPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetVideoPid(pstContext, DbSvc_GetVideoPid(&pstContext->stSvcInfo)),
								DbSvc_GetPcrPid(&pstContext->stSvcInfo),
								DbSvc_GetAudioCodec(&pstContext->stSvcInfo),
								DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo),
								DbSvc_GetDolbyCodec(&pstContext->stSvcInfo),
								DbSvc_GetVideoCodec(&pstContext->stSvcInfo),
								DbSvc_GetDolbyFlag(&pstContext->stSvcInfo),
								DbSvc_GetStereoSelect(&pstContext->stSvcInfo)
								);
#endif
		if (hErr)
		{
			HxLOG_Error("SVC_AV_ChangePid ret :0x%X\n",hErr);
		}
	}
	else
	{ /* 다른 state에서는 변동없다. */
		HxLOG_Print("\t=> Not in eLiveViewState_AV. Don't restart AV.\n");
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_MsgMgrActionMainChanged(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}



BUS_Result_t XMGR_LiveView_BASE_EvtSiSdtRunningStatusChanged(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR							hErr;
	Handle_t							hSupSvc = (Handle_t)p1;
	SvcSi_SdtRunningStatusProcessType_e	eRunningStatusProcessType = (SvcSi_SdtRunningStatusProcessType_e)p2;
	SvcSi_SdtRunningStatusType_e			eRunningStatus = (SvcSi_SdtRunningStatusType_e)p3;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	HxLOG_Debug("hSupSvc(0x%08x) eRunningStatusProcessType(%d) eRunningStatus(%d) \n",hSupSvc, eRunningStatusProcessType, eRunningStatus);

	switch(eRunningStatusProcessType)
	{
		case eSdtRunningStatusChanged_ByZiggo:
		case eSdtRunningStatusChanged_ByMiddleEast:
		case eSdtRunningStatusChanged_ByDigiturk:
		case eSdtRunningStatusChanged_ByTrueVisions:
		{
			if(eRunningStatus == eSdtRunningStatusType_NOT_RUNNING)
			{
				// AV Block
				_mgr_LiveView_base_SetAppState(pstContext, eLiveViewState_SVC_NOT_RUNNING);
			}
			else if( (eRunningStatus == eSdtRunningStatusType_RUNNING) &&
					(pstContext->eAppState == eLiveViewState_SVC_NOT_RUNNING) )
			{
				// AV Block 해제
				_mgr_LiveView_base_StartMwServices(pstContext, FALSE);
			}
			break;
		}

		case eSdtRunningStatusChanged_ByHumaxRule:
		case eSdtRunningStatusChanged_ByNordig:
		case eSdtRunningStatusChanged_ByUkDtt:
		default:
		{
			MgrAction_Priority_e		 eActPriority;

			if (eMgrLiveMode_LiveStreaming == pstContext->eLiveMode)
			{
				DxLiveStreamPriority_e		 eLsPriority = eDxLsPriority_EQUAL_REC;

				eActPriority = eAmPriority_RECORD;

				hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LIVESTREAM_PRIORITY, (HUINT32 *)&eLsPriority, 0);
				if (ERR_OK == hErr)
				{
					switch (eLsPriority)
					{
					case eDxLsPriority_EQUAL_REC:
						eActPriority = eAmPriority_RECORD;
						break;

					case eDxLsPriority_EQUAL_VIEW:
						eActPriority = eAmPriority_VIEW_ACTION;
						break;

					case eDxLsPriority_BELOW_VIEW:
						eActPriority = (eAmPriority_VIEW_ACTION - (1<<ACTION_PRIORITY_BIT_OFFSET));
						break;

					case eDxLsPriority_UPPER_REC:
						eActPriority = (eAmPriority_RECORD + (1<<ACTION_PRIORITY_BIT_OFFSET));
						break;

					default:
						break;
					}
				}
			}
			else
			{
				eActPriority = eAmPriority_VIEW_ACTION;
			}

			if(pstContext->sessionType == eLiveViewSession_NORMAL)
			{
				/* Normal session 에서 ... */
				/* eSEVT_SI_SDT_RUNNING_STATUS_CHANGED 에서는 무조건 replacement service로 튜닝한다. */
				hErr = _mgr_LiveView_base_SetupAction(ulActionId, hSupSvc, eActPriority, pstContext->eLiveMode);
				if (hErr == ERR_OK)
				{
					/* ERR_OK를 받아야만 이동하고 못 받으면 현재 채널에 머물러야 한다. */
					pstContext->ulReplaceMessage = eSEVT_SI_SDT_RUNNING_STATUS_CHANGED; /* Replacement service 정보 저장 */
					hErr = _mgr_LiveView_base_StartSuppleSession(pstContext, hSupSvc, eLiveViewSession_REPLACE, 0);
				}
				else
				{
					HxLOG_Print("[AP:LiveView(%d)] Can't tune to replacement service by hErr[%d]\n",ulActionId, hErr);
				}
			}
			else if(pstContext->sessionType == eLiveViewSession_REPLACE)
			{
				/* Replacement service sesssin에서는 orginal service로 silent하게 복귀. */
				hErr = _mgr_LiveView_base_SetupAction(ulActionId, pstContext->hMasterSvc, eActPriority, pstContext->eLiveMode);
				if(hErr == ERR_OK)
				{
					/* ERR_OK를 받아야만 이동하고 못 받으면 현재 채널에 머물러야 한다. */
					hErr = _mgr_LiveView_base_StartMasterSession(pstContext, pstContext->hMasterSvc, pstContext->svcListGroup, eNO_IPLATE|eKEEP_PINPASS);
				}
				else
				{
					HxLOG_Print("[AP:LiveView(%d)] Can't tune to org. service from replacement service by hErr[%d]\n",ulActionId, hErr);
				}
			}
			else
			{
				HxLOG_Critical("\n\n");
			}
			break;
		}
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtSiNetworkChangedNotify(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_MsgLiveSetMainviewAction(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 					ulOldMainActionId, ulNewMainActionId;
	HERROR 					hErr = ERR_FAIL;
	MgrLive_LiveMode_e	 	eNewLiveMode = eMgrLiveMode_MainLive;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	if ((pstContext->ulActionId < eActionId_VIEW_FIRST)
		|| (pstContext->ulActionId > eActionId_VIEW_LAST))
	{
		HxLOG_Error("[AP:LiveView(%d)] invalid context action id(%d)\n",pstContext->ulActionId, pstContext->ulActionId);
		return ERR_BUS_NO_ERROR;
	}

	ulNewMainActionId = p1;
	ulOldMainActionId = p2;

	HxLOG_Debug("[AP:LiveView(%d)] Context :: LiveMode(%s) actionId(%d) \n", pstContext->ulActionId, (pstContext->eLiveMode == eMgrLiveMode_MainLive) ? "MAIN" : "PIP", pstContext->ulActionId);
	HxLOG_Debug("[AP:LiveView(%d)] oldMainActionId(%d) newMainActionId(%d) \n",pstContext->ulActionId, ulOldMainActionId, ulNewMainActionId);

	if ((pstContext->eLiveMode == eMgrLiveMode_MainLive)
		&& (pstContext->ulActionId == ulOldMainActionId))
	{
		// Main -> PIP
		HxLOG_Debug("[AP:LiveView(%d)] MainView -> Pip \n", pstContext->ulActionId);

		eNewLiveMode = eMgrLiveMode_PipLive;
	}
	else if ((pstContext->eLiveMode == eMgrLiveMode_PipLive)
			&& (pstContext->ulActionId == ulNewMainActionId))
	{
		// PIP -> Main
		HxLOG_Debug("[AP:LiveView(%d)] Pip -> MainView \n", pstContext->ulActionId);

		eNewLiveMode = eMgrLiveMode_MainLive;
	}
	else
	{
		HxLOG_Critical("\n\n");
		return ERR_BUS_NO_ERROR;
	}

	HxLOG_Debug("[AP:LiveView(%d)] LiveMode trasition (%d -> %d) \n",pstContext->ulActionId, pstContext->eLiveMode, eNewLiveMode);

	// update view context
	pstContext->eLiveMode = eNewLiveMode;

	// update xmgr_live context
	hErr = MGR_LIVE_SetLiveMode(pstContext->ulActionId, eNewLiveMode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[AP:LiveView(%d)] MGR_LIVE_SetLiveMode() failed..! \n", pstContext->ulActionId);
		return ERR_BUS_NO_ERROR;
	}

 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_MsgLiveSetFccMainviewAction(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 					hErr = ERR_FAIL;
	HUINT32					ulOldMainActionId = (HUINT32)p1;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_BUS_NO_ERROR);

	HxLOG_Print("[AP:LiveView(%d)] Context :: LiveMode(%s) actionId(%d) \n", pstContext->ulActionId, (pstContext->eLiveMode == eMgrLiveMode_MainLive) ? "MAIN" : "FCC", pstContext->ulActionId);
	HxLOG_Print("[AP:LiveView(%d)] oldMainActionId(%d) newMainActionId(%d) \n",pstContext->ulActionId, ulOldMainActionId, ulActionId);

	if ((pstContext->eLiveMode == eMgrLiveMode_MainLive) && (pstContext->ulActionId == ulOldMainActionId))
	{
		// Main -> Fcc
		HxLOG_Print("[AP:LiveView(%d)] MainView -> Fcc \n", pstContext->ulActionId);

			pstContext->eLiveMode = eMgrLiveMode_FccLive;

			// set mgr live context
			hErr = MGR_LIVE_SetLiveMode(pstContext->ulActionId, eMgrLiveMode_FccLive);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("[AP:LiveView(%d)] MGR_LIVE_SetLiveMode() failed..! \n", pstContext->ulActionId);
			}

			return ERR_BUS_NO_ERROR;
		}
	else if ((pstContext->eLiveMode == eMgrLiveMode_FccLive)
			&& (pstContext->ulActionId == ulActionId))
	{
		// Fcc -> Main
		HxLOG_Print("[AP:LiveView(%d)] Fcc -> MainView \n", pstContext->ulActionId);
	}
	else
	{
		// nothing to do..
		HxLOG_Print("[AP:LiveView(%d)] nothing to do~ \n", pstContext->ulActionId);
		return ERR_BUS_NO_ERROR;
	}

	// start fcc av dec
	SVC_AV_SetMainFccMedia(ulActionId);

	// hide a/v
	hErr = SVC_AV_SetAudioHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_Fcc,  TRUE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetAudioHide ret :0x%X\n", hErr);
	}

	hErr = SVC_AV_SetVideoHide(SVC_PIPE_GetActionId(pstContext->hAction), eAvMuteFlag_Fcc, TRUE);
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetVideoHide ret :0x%X\n", hErr);
	}

	// update view context
	HxLOG_Print("[AP:LiveView(%d)] LiveMode trasition (%s -> %s) \n",pstContext->ulActionId, MGR_LIVE_GetLiveModeStr(pstContext->eLiveMode), MGR_LIVE_GetLiveModeStr(eMgrLiveMode_MainLive));
	pstContext->eLiveMode = eMgrLiveMode_MainLive;

	// update xmgr_live context
	hErr = MGR_LIVE_SetLiveMode(pstContext->ulActionId, eMgrLiveMode_MainLive);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[AP:LiveView(%d)] MGR_LIVE_SetLiveMode() failed..! \n", pstContext->ulActionId);
	}

	/* 서비스 변경 공지 */
	_mgr_LiveView_base_SendBusMessage(NULL, eMEVT_LIVE_NOTIFY_SVC_CHANGED, pstContext->hAction, pstContext->hSvc, 0, 0);

	// start mw svc
	_mgr_LiveView_base_StartMwServices(pstContext, FALSE);

	/* Front에 채널 정보 (채널 번호/채널명등) display */
	_mgr_LiveView_base_SetFrontPanel(pstContext);

 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_FccTuneStatus(XmgrLiveView_Context_t  *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 	ulTuneMsg = (HUINT32)p1;

	switch (ulTuneMsg)
	{
		case eSEVT_CH_NO_SIGNAL:
			XMGR_LiveView_BASE_EvtChNoSignal(pstContext, ulActionId, p1, p2, p3);
			break;

		default:
			// do nothing
			HxLOG_Error("Invalid tuner state - (%d) \n", ulTuneMsg);
			break;
	}

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_StopMwServiceForFcc(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
#if defined(CONFIG_SUPPORT_FCC)
	HERROR			hErr = ERR_FAIL;
	HUINT32			ulCurMainActionId, ulNewMainActionId;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_BUS_INITIAL_FAILED);

	ulCurMainActionId = (HUINT32)p1;
	ulNewMainActionId = (HUINT32)p2;

	if ((ulActionId == ulCurMainActionId) || (ulActionId == ulNewMainActionId))
	{
		/***   MAIN -> FCC or FCC -> MAIN  ***/
		/* FCC only get PMT/PAT filtered data and MainView get all filtered data */
		/* So, need to re-start MW SI service when changing MainView */
		hErr =  _mgr_LiveView_base_StopMwService(pstContext, eLiveMwSvcType_Si, eSyncMode);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("_mgr_LiveView_base_StopMwServiceeLiveMwSvcType_Si() failed..! actionId(%d) \n", ulActionId);
		}
	}
	else
	{
		/***   FCC -> FCC   ***/
		/* nothing to do */
	}
#endif

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_MsgLiveUnselectComponent(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 		hErr = ERR_FAIL;
	HBOOL		bUnselectVideo, bUnselectAudio;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	if ((pstContext->ulActionId < eActionId_VIEW_FIRST)
		|| (pstContext->ulActionId > eActionId_VIEW_LAST))
	{
		HxLOG_Error("[AP:LiveView(%d)] invalid context action id(%d)\n",pstContext->ulActionId, pstContext->ulActionId);
		return ERR_BUS_NO_ERROR;
	}

	bUnselectVideo = (HBOOL)p1;
	bUnselectAudio = (HBOOL)p2;

	// check parameter
	if ((bUnselectVideo == FALSE) && (bUnselectAudio == FALSE))
	{
		// Audio/Video 중 하나는 true여야 함
		HxLOG_Error("[AP:LiveView(%d)] invalid parameters\n",pstContext->ulActionId);
		return ERR_BUS_NO_ERROR;
	}

	if (((bUnselectVideo == TRUE) && (pstContext->bUnselectVideo == TRUE))		//<< 이미 video unselected임
		|| ((bUnselectAudio == TRUE) && (pstContext->bUnselectAudio == TRUE)))	//<< 이미 audio unselected임
	{
		HxLOG_Error("[AP:LiveView(%d)] Already unselected.. video(%d->%d) audio(%d->%d)\n", bUnselectVideo, pstContext->bUnselectVideo, bUnselectAudio, pstContext->bUnselectAudio);
		return ERR_BUS_NO_ERROR;
	}

	// update flag
	if (bUnselectVideo == TRUE)
	{
		pstContext->bUnselectVideo = TRUE;
	}
	if (bUnselectAudio == TRUE)
	{
		pstContext->bUnselectAudio = TRUE;
	}
	HxLOG_Print("[AP:LiveView(%d)] New setting : bUnselectVideo(%d) bUnselectAudio(%d) \n",pstContext->ulActionId, bUnselectVideo, bUnselectAudio);

	hErr = SVC_AV_SetDualMonoSelect(pstContext->ulActionId, DbSvc_GetDualmonoSelect(&pstContext->stSvcInfo));
	if (hErr)
	{
		HxLOG_Error("SVC_AV_SetDualMonoSelect ret :0x%X\n",hErr);
	}

#if defined(SvcAv_START_SERVICE_NEW)
	hErr = SVC_AV_ChangePid(pstContext->ulActionId,
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetVideoPid(pstContext, DbSvc_GetVideoPid(&pstContext->stSvcInfo)),
								DbSvc_GetPcrPid(&pstContext->stSvcInfo),
								DbSvc_GetAudioCodec(&pstContext->stSvcInfo),
								DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo),
								DbSvc_GetVideoCodec(&pstContext->stSvcInfo)
								);
#else
	hErr = SVC_AV_ChangePid(pstContext->ulActionId,
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetAudioAuxPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetAudioPid(pstContext, DbSvc_GetDolbyPid(&pstContext->stSvcInfo)),
								_mgr_LiveView_base_GetVideoPid(pstContext, DbSvc_GetVideoPid(&pstContext->stSvcInfo)),
								DbSvc_GetPcrPid(&pstContext->stSvcInfo),
								DbSvc_GetAudioCodec(&pstContext->stSvcInfo),
								DbSvc_GetAudioAuxCodec(&pstContext->stSvcInfo),
								DbSvc_GetDolbyCodec(&pstContext->stSvcInfo),
								DbSvc_GetVideoCodec(&pstContext->stSvcInfo),
								DbSvc_GetDolbyFlag(&pstContext->stSvcInfo),
								DbSvc_GetStereoSelect(&pstContext->stSvcInfo)
								);
#endif
	if (hErr)
	{
		HxLOG_Error("SVC_AV_ChangePid ret :0x%X\n",hErr);
	}


 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_MsgBusGoStandby(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	switch (pstContext->eLiveMode)
	{
	case eMgrLiveMode_LiveStreaming:
		HxLOG_Info ("The Live module works for the live streaming: It shall not be killed by 'GoStandby!'\n");
		break;

	case eMgrLiveMode_MainLive:
	case eMgrLiveMode_PipLive:
		BUS_MGR_Destroy (NULL);
		break;

	default:
		HxLOG_Error ("Invalid LiveMode(%d)... Destroy the procedure anyway.\n", pstContext->eLiveMode);
		BUS_MGR_Destroy (NULL);
		break;
	}

	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveView_BASE_MsgBusTimer(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	switch (p1)
	{
#if defined(CONFIG_MW_CAS_IRDETO) && (CONFIG_MW_CAS_NUM_EMBEDDED == 1)
		case NO_SIGNAL_TIMER_ID:
			if (pstContext->eUiState == eLiveViewUi_NO_SIGNAL)
			{
				_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_NO_SIGNAL_TIMEOUT);
			}
			break;
#endif

		default:
			break;
	}

	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return MESSAGE_PASS;
}

BUS_Result_t XMGR_LiveView_BASE_MsgMhegStatusChange(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

//	HLIB_CMD_Printf("\033[30;43m[%s : %d] Set Mheg Status : %d  \033[0m\n", __FUNCTION__, __LINE__, p1);

#if defined(CONFIG_3RD_MHEG)
	if(p1 == eAmMhegStatus_Running)
	{
		_mgr_LiveView_base_SetUiState(pstContext, eLiveViewUi_OK);
		return MESSAGE_PASS;
	}
#endif

	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return MESSAGE_PASS;
}



#if defined(CONFIG_MW_CAS)
BUS_Result_t XMGR_LiveView_BASE_EvtCasOk(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstContext->nCasEvt = 0;
	pstContext->nCasSystemId = p1;
	pstContext->pCasInfo = NULL;
	pstContext->nCasShow = eCasShowType_Both;
	pstContext->bCasOk = TRUE;

	if(pstContext->eAppState == eLiveViewState_CAS || pstContext->eAppState == eLiveViewState_CAS_PREVIEW)
	{
		/* CAS에서 OK로 나갈 때는 다시 디코딩이 시작될 것이므로 먼저 와 있는underrun flag를 reset해야 함. */
		pstContext->bAvUnderrun = FALSE;
	}

	if (pstContext->eAppState == eLiveViewState_CAS || pstContext->eAppState == eLiveViewState_CAS_PREVIEW)
	{
		_mgr_LiveView_base_ShowProgram(pstContext);
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtCasFail(XmgrLiveView_Context_t *pstContext, HUINT32 event, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	pstContext->nCasEvt = event;
	pstContext->nCasSystemId = p1;
	pstContext->pCasInfo = (void*)p2;
	pstContext->nCasShow = p3;
	pstContext->bCasOk = FALSE;

	if(pstContext->eAppState > eLiveViewState_PID_START)
	{
		_mgr_LiveView_base_ShowProgram(pstContext);
	}
	else
	{
		HxLOG_Print("[AP:ZAP(%d)] \tUnexpected event message in state(%s)\n", ulActionId,
											_mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	}

	//--------------------------------------------------------
 	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtCasSystemInfoUpdated(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxCAS_GroupId_e eCasId;

	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	eCasId = (DxCAS_GroupId_e)p1;

	if (pstContext->eAppState > eLiveViewState_PID_START
#if defined(CONFIG_OP_SES)
		|| (p3 == eMEVT_CAS_SYSINFO_UPDATED && pstContext->eAppState == eLiveViewState_NO_SIGNAL)
#endif
	)
	{
		switch (eCasId)
		{
		case eDxCAS_GROUPID_NA:
			if (SVC_CAS_CheckSmartCardValidity(eCasId) == TRUE)
			{
				if(pstContext->bSCPinCodeValid != TRUE)
				{ /* SC-PIN-Code invalid 였는데... */
					HxLOG_Error("SC-PIN-Code is now valid ! \n");

					pstContext->bSCPinCodeValid = TRUE;

					/* 현재 STB-PIN-Code로 돌고 있을 것이므로 다시 SC-PIN-Code로 바꾼다. */
					HxLOG_Error("Check if SC-PIN-Code should be activated. \n");
					_mgr_LiveView_base_ShowProgram(pstContext);
				}

#if defined(CONFIG_OP_SES)
				// SES 사양일 경우, SEARCH 중이 아니면 HD Simulcast를 실행할 수 있다.
				if (MGR_ACTION_GetType(ulActionId) != eActionType_SEARCH)
				{
					XMGR_SEARCH_LCNUPDATE_SimulcastPreferredList(p3);
				}
#endif
			}

			break;

		default:
			break;
		}
	}

	//--------------------------------------------------------
	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtCasDecodeForceTune(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{		//	p1: NW ID, p2:TS ID, p3:Svc id
	HINT32					nCntSvc = 0;
	Handle_t*				pSvcHandleList = NULL, hTuneSvc = HANDLE_NULL;
	DbSvc_TripleId_t			triple_id;
	DxDeliveryType_e 		eOrgDeliType;
	HUINT32					ulTunerNum;

	triple_id.usOnId = p1;
	triple_id.usTsId = p2;
	triple_id.usSvcId = p3;

	SVC_CH_GetNumberOfTunerGroup(&ulTunerNum);
	if(ulTunerNum > 1)
	{
		eOrgDeliType = eDxDELIVERY_TYPE_ALL;
	}
	else
	{
		SVC_CH_GetTunerType(0, (HUINT32 *)&eOrgDeliType);
	}

	DB_SVC_FindServiceListByTripleId(eOrgDeliType, triple_id, &nCntSvc, &pSvcHandleList);

	if (pSvcHandleList == NULL)
	{
		return 1;
	}

	hTuneSvc = pSvcHandleList[0];

	if(pSvcHandleList)
	{
		DB_SVC_FreeServiceList(pSvcHandleList);
	}

	MGR_LIVE_CmdPostServiceStartMainView(hTuneSvc, eChListUiGroup_All , eFORCE_TUNE);

	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtCasScRemoved(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if 0	// sc pin valid check가 필요한가??
	if(pstContext->bSCPinCodeValid == TRUE)
	{ /* SC-PIN-Code valid 였는데... */
		if(SVC_CAS_CheckSmartCardValidity(eDxCAS_GROUPID_NA) != TRUE)
		{ /* SC-PIN-Code 검증 기능을 사용할 수 없게 되었다. */
			HxLOG_Print("[AP:LiveCas(%d)] SC-PIN-Code is no longer valid !\n", ulActionId);

			pstContext->bSCPinCodeValid = FALSE;
			pstContext->bSCPinPassed = FALSE; /* SC PIN Pass flag를 reset하고... */
			pstContext->stSvcInfo.ulRatingPassedEventId = (HUINT32)-1; /* passed event ID 기록도 삭제한다. */
		}
	}
#endif

	//--------------------------------------------------------
	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

BUS_Result_t XMGR_LiveView_BASE_EvtCasScInserted(XmgrLiveView_Context_t *pstContext, HUINT32 ulActionId, HINT32 p1, HINT32 p2, HINT32 p3)
{
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if defined(CONFIG_OP_SES)
	// SES 사양일 경우, SEARCH 중이 아니면 HD Simulcast를 실행할 수 있다.
	if (MGR_ACTION_GetType(ulActionId) != eActionType_SEARCH)
	{
		XMGR_SEARCH_LCNUPDATE_SimulcastPreferredList(p3);
	}
#endif
	//--------------------------------------------------------
	LIVE_PARAM_WARNING_REMOVE(pstContext, ulActionId, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}
#endif

#define _____CONTEXT_FUNC_____

XmgrLiveView_Context_t *XMGR_LiveView_BASE_GetContext (HUINT32 ulActionId)
{
	return _mgr_LiveView_base_getContext (ulActionId);
}

HERROR XMGR_LiveView_BASE_InitContext (void)
{
	return _mgr_LiveView_base_initContext();
}

HERROR XMGR_LiveView_BASE_ResetUiState(HUINT32 ulActionId, HBOOL bSvcStart)
{
	return _mgr_LiveView_base_resetUiState(ulActionId, bSvcStart);
}

#define _____BASE_FUNC____
HERROR XMGR_LiveView_BASE_GetLiveMode (HUINT32 ulActionId, MgrLive_LiveMode_e *peLiveMode)
{
	XmgrLiveView_Context_t			*pstContext = NULL;

	if (NULL == peLiveMode)
	{
		HxLOG_Error("[AP:LiveView(%d)] Invalid parameter \n", ulActionId);
		return ERR_FAIL;
	}

	pstContext = _mgr_LiveView_base_getContext(ulActionId);
	if (NULL == pstContext)
		return ERR_FAIL;

	*peLiveMode = pstContext->eLiveMode;

	return ERR_OK;
}

HERROR XMGR_LiveView_BASE_GetServiceHandle(HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, Handle_t *phSvc)
{
	XmgrLiveView_Context_t			*pstContext = NULL;

	if (phSvc == NULL)
	{
		HxLOG_Error("[AP:LiveView(%d)] Invalid parameter \n", ulActionId);
		return ERR_FAIL;
	}

	pstContext = _mgr_LiveView_base_getContext(ulActionId);
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	*phSvc =  pstContext->hSvc;

	switch (eSvcType)
	{
		case eLiveViewPlaySvc_MASTER:
		*phSvc =  pstContext->hMasterSvc;
		break;

		case eLiveViewPlaySvc_ACTUAL:
		default:
		*phSvc =  pstContext->hSvc;
		break;
	}

	return ERR_OK;
}

HERROR XMGR_LiveView_BASE_GetSessionType(HUINT32 ulActionId, MgrLiveView_SessionType_t *pSessionType)
{
	XmgrLiveView_Context_t			*pstContext = NULL;

	if(MGR_ACTION_GetType(ulActionId) != eViewType_WTV)
	{
		HxLOG_Print("[AP:VIEW(%d)] XMGR_LiveView_BASE_GetSessionType() was called with NON-wtv action ID(%d) type is [%d]\n",ulActionId,ulActionId,MGR_ACTION_GetType(ulActionId));
		return ERR_FAIL;
	}

	pstContext = _mgr_LiveView_base_getContext(ulActionId);
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);
	*pSessionType = pstContext->sessionType;

	return ERR_OK;
}

HERROR XMGR_LiveView_BASE_GetRingBufHandle(HUINT32 ulActionId, Handle_t *phRingBuf)
{
	XmgrLiveView_Context_t			*pstContext = NULL;

	pstContext = _mgr_LiveView_base_getContext(ulActionId);
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	switch(pstContext->stStartArg.eLiveMode)
	{
	case eMgrLiveMode_LiveStreaming:
		*phRingBuf = pstContext->stStartArg.unExtArg.stLiveStreamArg.hRingBuf;
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR XMGR_LiveView_BASE_GetProfileFromJSON(const HCHAR *pProfileString, MgrLive_LiveStreamArg_t *pstDestArg)
{
	return _mgr_LiveView_base_ProfileGetItem (pProfileString, pstDestArg);
}

HERROR XMGR_LiveView_BASE_GetSvcInfo (HUINT32 ulActionId, MgrLiveView_PlaySvc_e eSvcType, DbSvc_Info_t *pstMasterSvcInfo)
{
	XmgrLiveView_Context_t			*pstContext = NULL;

	if(MGR_ACTION_GetType(ulActionId) != eViewType_WTV)
	{
		HxLOG_Print("[AP:VIEW(%d)] XMGR_LiveView_BASE_GetSvcInfo() was called with NON-wtv action ID(%d)\n", ulActionId, ulActionId);
		return ERR_FAIL;
	}

	pstContext = _mgr_LiveView_base_getContext(ulActionId);
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	if(pstContext->eAppState == eLiveViewState_IDLE)
	{
		HxLOG_Print("[AP:VIEW(%d)] XMGR_LiveView_BASE_GetSvcInfo() was called in IDLE state\n", ulActionId);
		return ERR_FAIL;
	}

	switch (eSvcType)
	{
		case eLiveViewPlaySvc_MASTER:
			*pstMasterSvcInfo = *(_mgr_LiveView_base_GetMasterSvcInfoPtr(pstContext));
			break;

		case eLiveViewPlaySvc_ACTUAL:
		default:
			*pstMasterSvcInfo = pstContext->stSvcInfo;
			break;
	}

	return ERR_OK;
}

HERROR XMGR_LiveView_BASE_GetTsInfo(HUINT32 ulActionId, DbSvc_TsInfo_t *pTsInfo)
{
	XmgrLiveView_Context_t			*pstContext = NULL;

	if(MGR_ACTION_GetType(ulActionId) != eViewType_WTV)
	{
		HxLOG_Print("[AP:LIVE(%d)] XMGR_LiveView_BASE_GetTsInfo() was called with NON-wtv action ID(%d)\n", ulActionId, ulActionId);
		return ERR_FAIL;
	}

	pstContext = _mgr_LiveView_base_getContext(ulActionId);
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	if(pstContext->eAppState == eLiveViewState_IDLE)
	{
		HxLOG_Print("[AP:LIVE(%d)] XMGR_LiveView_BASE_GetTsInfo() was called in IDLE state\n", ulActionId);
		return ERR_FAIL;
	}

	*pTsInfo = pstContext->tsInfo;

	return ERR_OK;
}

HERROR XMGR_LiveView_BASE_GetLastTuneStatus(HUINT32 ulActionId, HUINT32 *pulLastTuneStatus)
{
	XmgrLiveView_Context_t			*pstContext = NULL;

	if(MGR_ACTION_GetType(ulActionId) != eViewType_WTV)
	{
		HxLOG_Print("[AP:LIVE(%d)] XMGR_LiveView_BASE_GetTsInfo() was called with NON-wtv action ID(%d)\n", ulActionId, ulActionId);
		return ERR_FAIL;
	}

	pstContext = _mgr_LiveView_base_getContext(ulActionId);
	LIVE_POINTER_INVALID_CHECK(pstContext, ERR_FAIL);

	*pulLastTuneStatus = pstContext->ulLastTuneStatus;

	return ERR_OK;
}


Handle_t XMGR_LiveView_BASE_GetMasterSvcHandle(HUINT32 ulActionId)
{
	XmgrLiveView_Context_t		*pstContext = _mgr_LiveView_base_getContext (ulActionId);

	if (pstContext == NULL)
	{
		HxLOG_Error("[AP:LIVE(%d)] XMGR_LiveView_BASE_GetMasterSvcHandle() error ! \n", ulActionId);
		return HANDLE_NULL;
	}

	return _mgr_LiveView_base_GetMasterSvcHandle(pstContext);
}

void XMGR_LiveView_BASE_ShowProgram(XmgrLiveView_Context_t *pstContext)
{
	return _mgr_LiveView_base_ShowProgram(pstContext);
}

HERROR XMGR_LiveView_BASE_StopMwService(XmgrLiveView_Context_t *pstContext, XmgrLive_MwSvcType_e eMwSvcType, ApiSyncMode_t eSync)
{
	return _mgr_LiveView_base_StopMwService(pstContext, eMwSvcType, eSync);
}

void XMGR_LiveView_BASE_SetAppState(XmgrLiveView_Context_t *pstContext, XmgrLiveView_State_e eAppState)
{
	return _mgr_LiveView_base_SetAppState(pstContext, eAppState);
}


#define _____TEMP_PROTOTYPE_PROCEDURE_____
// protoProc 정리를 위해 다른 proc으로 분기될 msg 처리를 담당하는 임시 proc 들
BUS_Result_t XMGR_LiveView_BASE_OpSpecificProc (XmgrLiveView_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	HUINT32			ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		/**
		  *	SDT의 running status 변경으로 replacement service로 변경해야 함.
		**/
		case eSEVT_SI_SDT_RUNNING_STATUS_CHANGED :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_SDT_RUNNING_STATUS_CHANGED\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtSiSdtRunningStatusChanged(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_SI_NETWORK_CHANGED_NOTIFY:	// UK
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_NETWORK_CHANGED_NOTIFY\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtSiNetworkChangedNotify(pstContext, ulActionId, p1, p2, p3);
			break;

#if	defined(CONFIG_MW_CH_ANTENNA_5V)
		case eSEVT_CH_ANTENNA_FAULT :
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eSEVT_CH_ANTENNA_FAULT\n", ulActionId);
			break;

		case eSEVT_CH_ANTENNA_OK :
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CH_ANTENNA_OK\n", ulActionId);
			break;
#endif

		default:
			break;
	}

	return eBusResult;
}

#define _____PROTOTYPE_PROCEDURE_____
BUS_Result_t XMGR_LiveView_BASE_ProtoProc (XmgrLiveView_Context_t *pstContext, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	HUINT32				ulActionId = SVC_PIPE_GetActionId(hAction);

	switch (message)
	{
		/********************************************
			WatchTV application 생성됨. 초기화 작업 수행
		*********************************************/
		case eMEVT_BUS_CREATE :
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eMEVT_BUS_CREATE \n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgBusCreate(pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eMEVT_BUS_DESTROY\n",pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgBusDestroy(pstContext, pstContext->ulActionId, p1, p2, p3);
			break;

		case eMEVT_LIVE_START_SVC:
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eMEVT_LIVE_START_SVC \n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgLiveStartSvc (pstContext, hAction, p1, p2, p3);
			break;

		case eMEVT_LIVE_STOP_SVC:
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eMEVT_LIVE_STOP_SVC \n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgLiveStopSvc (pstContext, hAction, p1, p2, p3);
			break;

		/********************************************
			Event msg
		*********************************************/
		case eSEVT_CH_STOPPED :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eSEVT_CH_STOPPED\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtChStopped(pstContext, hAction, p1, p2, p3);
#if defined(CONFIG_3RD_BML)
			XMGR_LiveComp_BASE_StopBML(pstContext->hAction, eAsyncMode);
#endif
			break;

		case eSEVT_SI_STOPPED:
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_STOPPED\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtSiStopped(pstContext, hAction, p1, p2, p3);
			break;

		case eSEVT_CAS_STOPPED:
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CAS_STOPPED\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtCasStopped(pstContext, hAction, p1, p2, p3);
			break;

		/********************************************
			Channel, Tuner, Antenna msg
		*********************************************/
		case eSEVT_CH_NO_SIGNAL :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CH_NO_SIGNAL\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtChNoSignal(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CH_MOVING_MOTOR :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eSEVT_CH_MOVING_MOTOR\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtChMovingMotor(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CH_LOCKED :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CH_LOCKED\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtChLocked(pstContext, ulActionId, p1, p2, p3);
			break;


		/**********************************************
			DRM (SI/CAS)
		***********************************************/
		case eSEVT_SI_DRM_SIGNALLED:
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_DRM_SIGNALLED\n", ulActionId);
			eBusResult = XMGR_LiveView_EvtSiDrmSignalled(pstContext, ulActionId, p1, p2, p3);
			break;

#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS)
		case eSEVT_CAS_DRM_SIGNALLED:
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CAS_DRM_SIGNALLED\n", ulActionId);
			eBusResult = XMGR_LiveView_EvtCasDrmSignalled(pstContext, ulActionId, p1, p2, p3);
			break;
#endif
		/********************************************
			SI msg
		*********************************************/
		case eSEVT_SI_PMT :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_PMT\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtSiPmt(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_SI_PMT_PIDREMOVED:
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_PMT_PIDREMOVED\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtSiPmtRemoved(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_SI_PMT_TIMEOUT :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_PMT_TIMEOUT\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtSiPmtTimeout(pstContext, ulActionId, p1, p2, p3);
			break;

		/**
		  *	SI service로부터의 PMT PID 변경 이벤트. PID가 다른 값으로 바뀜.
		  *	새 PMT PID가 AV나 다른 콤포넌트의 PID와 겹칠 수도 있기 때문에 다 털어내야 한다.
		  *	SI는 PMT를 자체적으로 걸지 말고 이 이벤트만 날리고 wtv의 처분을 기다려야 한다.
		  *	!!! SI가 그렇게 하고 있는지 확인해 볼 것.
		**/
		case eSEVT_SI_PMT_PIDCHANGED :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_PMT_PIDCHANGED\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtSiPmtChanged(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_SI_SDT:
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_SDT\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtSiSdt(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_SI_NIT:
			LIVE_CHECK_ACTION_HANDLE();
			if(pstContext->eLiveMode == eMgrLiveMode_MainLive)
			{
				eBusResult = XMGR_LiveView_BASE_EvtSiNit(pstContext, ulActionId, p1, p2, p3);
				HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_NIT\n", ulActionId);
 			}
			break;

		case eSEVT_SI_SERVICE_MOVE :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_SERVICE_MOVE\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtSiServiceMove (pstContext, hAction, p1, p2, p3);
			break;

#if defined(CONFIG_OP_UK_DTT)
		case eSEVT_SI_EIT:
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_SI_EIT\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtSiEit(pstContext, ulActionId, p1, p2, p3);
			break;
#endif
		/********************************************
			AV msg
		*********************************************/
		case eSEVT_AV_VIDEO_UNDERRUN :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eSEVT_AV_VIDEO_UNDERRUN\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtAvVideoUnderrun(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_AV_VIDEO_STARTED :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_AV_VIDEO_STARTED\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtAvVideoStarted(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_AV_AUDIO_UNDERRUN :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_AV_AUDIO_UNDERRUN\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtAvAudioUnderrun(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_AV_AUDIO_STARTED :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_AV_AUDIO_STARTED\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtAvAudioStarted(pstContext, ulActionId, p1, p2, p3);
			break;

		/********************************************
			cas msg
		*********************************************/
#if defined(CONFIG_MW_CAS)
		/**********************************************
			CAS service로부터의 시청 허가 이벤트
		***********************************************/
		case eSEVT_CAS_OK :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CAS_OK\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtCasOk(pstContext, ulActionId, p1, p2, p3);
			break;

		/**************************************
			CAS service로부터의 CAS 제어 이벤트
		***************************************/
		case eSEVT_CAS_FAIL :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CAS_FAIL\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtCasFail(pstContext, message, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CAS_CHECK :
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CAS_CHECK\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtCasFail(pstContext, message, ulActionId, p1, p2, p3);
			break;

		/********************************************
			CAS 삽입됨.
		*********************************************/
		// TODO: 임시 case eSEVT_CAS_SC_INSERTED: 이 이벤트가 확실하지 않아서 아래 것으로 임시 사용.
		case eMEVT_CAS_SYSINFO_UPDATED:
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eMEVT_CAS_SYSINFO_UPDATED\n", pstContext->ulActionId);
#if defined(CONFIG_OP_SES)
			p3 = message;
#endif
			eBusResult = XMGR_LiveView_BASE_EvtCasSystemInfoUpdated(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CAS_DECODE_FORCE_TUNE:
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CAS_DECODE_FORCE_TUNE\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtCasDecodeForceTune(pstContext, ulActionId, p1, p2, p3);
			break;

		/********************************************
			CAS 빠짐.
		*********************************************/
		case eSEVT_CAS_SC_REMOVED:
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CAS_SC_REMOVED\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtCasScRemoved(pstContext, ulActionId, p1, p2, p3);
			break;

		case eSEVT_CAS_SC_INSERTED:
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_CAS_SC_INSERTED\n", pstContext->ulActionId);
#if defined(CONFIG_OP_SES)
			p3 = message;
#endif
			eBusResult = XMGR_LiveView_BASE_EvtCasScInserted(pstContext, ulActionId, p1, p2, p3);
			break;
#endif	// #if defined(CONFIG_MW_CAS)

		/********************************************
			etc msg
		*********************************************/
		case eMEVT_LIVE_NOTIFY_SVC_LOCKED:
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eMEVT_LIVE_NOTIFY_SVC_LOCKED\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgLiveLockSvcLocked(pstContext, hAction, p1, p2, p3);
			break;


		case eMEVT_LIVE_NOTIFY_SVC_UNLOCKED:
			LIVE_CHECK_ACTION_HANDLE();
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eMEVT_LIVE_NOTIFY_SVC_UNLOCKED\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgLiveLockSvcUnlocked(pstContext, hAction, p1, p2, p3);
			break;

		/**
		  *	There are some changes in the service DB. Current handle should be checked.
		**/
		case eSEVT_DB_CHECK_SVC_DB :
			HxLOG_Debug("[AP:LiveView(%d)] MESSAGE : eSEVT_DB_CHECK_SVC_DB\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_EvtDbCheckSvcDb(pstContext, ulActionId, p1, p2, p3);
			break;

		/**
		  *	DB의 AV PID가 바뀌었으므로 각 action들은 변경사항을 반영하라는 이벤트 메시지
		**/
		case eMEVT_SVCLIST_SVC_PID_CHANGED :
			LIVE_CHECK_ACTION_ID();
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eMEVT_SVCLIST_SVC_PID_CHANGED\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgMgrSvcPidChanged(pstContext, ulActionId, p1, p2, p3);
			break;

		/**
		  *	MHEG등의 데이터 방송이 임시 AV PID로 변경을 요청함. 이제부터는 DB의 AV PID 참조하지 않음.
		**/
		case eMEVT_LIVE_CHANGE_TEMP_PID :
			LIVE_CHECK_ACTION_ID();
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eMEVT_LIVE_CHANGE_TEMP_PID\n", ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgMgrChangeTempPid(pstContext, ulActionId, p1, p2, p3);
			break;

		/**
		  *	Main Action이 바뀜. UI show/hide 처리.
		**/
		case eMEVT_ACTION_NOTIFY_MAIN_CHANGED:
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eMEVT_ACTION_NOTIFY_MAIN_CHANGED\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgMgrActionMainChanged(pstContext, ulActionId, p1, p2, p3);
			break;

		/**
		  *	main view가 변경됨 (PIP swap)
		**/
		case eMEVT_LIVE_SET_MAINVIEW_ACTION:
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eMEVT_LIVE_SET_MAINVIEW_ACTION\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgLiveSetMainviewAction(pstContext, ulActionId, p1, p2, p3);
			break;

		/**********************************************************
		 ***	Fast ch change (FCC)
		 ***********************************************************/
		/**
		 *	main view가 변경됨 (FCC svc -> Main svc)
		 **/
		case eMEVT_LIVE_SET_FCC_MAINVIEW_ACTION:
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eMEVT_LIVE_SET_FCC_MAINVIEW_ACTION\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgLiveSetFccMainviewAction(pstContext, ulActionId, p1, p2, p3);
			break;

		/**
		 *  tune state 변경
		*/
		case eMEVT_LIVE_NOTIFY_FCC_TUNE_STATUS:
			LIVE_CHECK_ACTION_ID();
			eBusResult = XMGR_LiveView_BASE_FccTuneStatus(pstContext, ulActionId, p1, p2, p3);
			break;

		case eMEVT_LIVE_STOP_MW_SVC_FOR_FCC:
			LIVE_CHECK_ACTION_ID();
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eMEVT_LIVE_STOP_MW_SVC_FOR_FCC\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_StopMwServiceForFcc(pstContext, ulActionId, p1, p2, p3);
			break;

		/**
		  *	unselectComponet가 전달됨. Audio/Video에 적용 가능하고 이를 받으면 rendering stop해야 함 (OIPF spec)
		**/
		case eMEVT_LIVE_SET_UNSELECT_COMPONENT:
			LIVE_CHECK_ACTION_ID();
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eMEVT_LIVE_SET_UNSELECT_COMPONENT\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgLiveUnselectComponent(pstContext, ulActionId, p1, p2, p3);
			break;

		/**********************************************************
		  ***	STANDBY
		***********************************************************/
		case eMEVT_BUS_READY_SHUTDOWN:
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eMEVT_BUS_READY_SHUTDOWN\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgBusGoStandby(pstContext, ulActionId, p1, p2, p3);
			break;


		/**********************************************************
		  ***	TIMER
		***********************************************************/
		case eMEVT_BUS_TIMER:
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eMEVT_BUS_TIMER\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgBusTimer(pstContext, ulActionId, p1, p2, p3);
			break;

#if defined(CONFIG_3RD_MHEG)
		case eMEVT_LIVE_SET_MHEG_STATUS_CHANGE:
			HxLOG_Debug ("[AP:LiveView(%d)] MESSAGE : eMEVT_LIVE_SET_MHEG_STATUS_CHANGE\n", pstContext->ulActionId);
			eBusResult = XMGR_LiveView_BASE_MsgMhegStatusChange(pstContext, pstContext->ulActionId, p1, p2, p3);
			break;
#endif

		default:
			break;
	}

	// 다른 proc들 정리가 완료되기 전에는 임시로 이렇게 처리하자..
	if (eBusResult == ERR_BUS_NO_ERROR)
	{
		eBusResult = XMGR_LiveView_BASE_OpSpecificProc(pstContext, message, hAction, p1, p2, p3);
	}

	return eBusResult;
}



#define _____PROCEDURES_____
STATIC BUS_Result_t  _mgr_LiveView_base_procView0 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrLiveView_Context_t			*pstContext = _mgr_LiveView_base_getContext (eActionId_VIEW_FIRST + 0);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveView_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t  _mgr_LiveView_base_procView1 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrLiveView_Context_t			*pstContext = _mgr_LiveView_base_getContext (eActionId_VIEW_FIRST + 1);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveView_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

#if defined(CONFIG_SUPPORT_FCC)
STATIC BUS_Result_t  _mgr_LiveView_base_procView2 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrLiveView_Context_t			*pstContext = _mgr_LiveView_base_getContext (eActionId_VIEW_FIRST + 2);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveView_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t  _mgr_LiveView_base_procView3 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrLiveView_Context_t			*pstContext = _mgr_LiveView_base_getContext (eActionId_VIEW_FIRST + 3);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveView_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}
#endif

STATIC BUS_Result_t  _mgr_LiveView_base_procBg0 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrLiveView_Context_t			*pstContext = _mgr_LiveView_base_getContext (eActionId_BG_FIRST + 0);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveView_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t  _mgr_LiveView_base_procBg1 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrLiveView_Context_t			*pstContext = _mgr_LiveView_base_getContext (eActionId_BG_FIRST + 1);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveView_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t  _mgr_LiveView_base_procBg2 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrLiveView_Context_t			*pstContext = _mgr_LiveView_base_getContext (eActionId_BG_FIRST + 2);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveView_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

STATIC BUS_Result_t  _mgr_LiveView_base_procBg3 (HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrLiveView_Context_t			*pstContext = _mgr_LiveView_base_getContext (eActionId_BG_FIRST + 3);

	if (eMEVT_BUS_CREATE == message)
	{
		BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
	}

	return XMGR_LiveView_BASE_ProtoProc (pstContext, message, hHandle, p1, p2, p3);
}

BUS_Callback_t XMGR_LiveView_BASE_GetProcByActionId (HUINT32 ulActionId)
{
	XmgrLiveView_Context_t		*pstContext = _mgr_LiveView_base_getContext (ulActionId);
	BUS_Callback_t			 pfLiveProc = NULL;

	if (NULL != pstContext)
	{
		switch (ulActionId)
		{
		case (eActionId_VIEW_FIRST + 0):
			pfLiveProc = _mgr_LiveView_base_procView0;
			break;

		case (eActionId_VIEW_FIRST + 1):
			pfLiveProc = _mgr_LiveView_base_procView1;
			break;

#if defined(CONFIG_SUPPORT_FCC)
		case (eActionId_VIEW_FIRST + 2):
			pfLiveProc = _mgr_LiveView_base_procView2;
			break;

		case (eActionId_VIEW_FIRST + 3):
			pfLiveProc = _mgr_LiveView_base_procView3;
			break;
#endif

		case (eActionId_BG_FIRST + 0):
			pfLiveProc = _mgr_LiveView_base_procBg0;
			break;

		case (eActionId_BG_FIRST + 1):
			pfLiveProc = _mgr_LiveView_base_procBg1;
			break;

		case (eActionId_BG_FIRST + 2):
			pfLiveProc = _mgr_LiveView_base_procBg2;
			break;

		case (eActionId_BG_FIRST + 3):
			pfLiveProc = _mgr_LiveView_base_procBg3;
			break;

		default:
			break;
		}
	}

	return pfLiveProc;

}

HERROR XMGR_LiveView_BASE_Init (void)
{
	_mgr_LiveView_base_initContext();

	return ERR_OK;
}

HERROR XMGR_LiveView_BASE_GetState(HUINT32 ulActionId, MgrLiveView_UiState_e *peViewState)
{
	XmgrLiveView_Context_t		*pstContext = NULL;

	if ((ulActionId >= eActionId_MAX) || (peViewState == NULL))
	{
		HxLOG_Debug("[AP:LiveView(%d)] invalid parameter \n");
		return ERR_FAIL;
	}

	pstContext = _mgr_LiveView_base_getContext (ulActionId);
	*peViewState = pstContext->eUiState;

	return ERR_OK;
}

#define _____ZAPPER_MEMBER_FUNCTIONS_____

STATIC HINT8* _mgr_LiveView_base_GetUiStateStr(MgrLiveView_UiState_e eUiState)
{
	switch (eUiState)
	{
		ENUM_TO_STR(eLiveViewUi_OK);
		ENUM_TO_STR(eLiveViewUi_NONE);
		ENUM_TO_STR(eLiveViewUi_EMPTY_SVC);
		ENUM_TO_STR(eLiveViewUi_NO_SIGNAL);
		ENUM_TO_STR(eLiveViewUi_MOTOR_MOVING);
		ENUM_TO_STR(eLiveViewUi_CAS);
		ENUM_TO_STR(eLiveViewUi_SVC_LOCKED);
		ENUM_TO_STR(eLiveViewUi_AV_UNDER_RUN);
		ENUM_TO_STR(eLiveViewUi_HDMI_BLOCKED);
		ENUM_TO_STR(eLiveViewUi_NO_VIDEO);
		ENUM_TO_STR(eLiveViewUi_NO_AV);
		ENUM_TO_STR(eLiveViewUi_NOT_RUNNING);
		ENUM_TO_STR(eLiveViewUi_MAX);
		ENUM_TO_STR(eLiveViewUi_NO_SIGNAL_TIMEOUT);
		ENUM_TO_STR(eLiveViewUi_REMOVE_CH);
		default:		break;
	}

	return "Unknown eUiState";
}

STATIC HINT8* _mgr_LiveView_base_GetAppStateStr(XmgrLiveView_State_e eAppState)
{
	switch(eAppState)
	{
		ENUM_TO_STR(eLiveViewState_IDLE);
		ENUM_TO_STR(eLiveViewState_EMPTY_SVC);
		ENUM_TO_STR(eLiveViewState_CH_STOP);
		ENUM_TO_STR(eLiveViewState_SVC_STOP);
		ENUM_TO_STR(eLiveViewState_SVC_LOCKED);

		ENUM_TO_STR(eLiveViewState_NO_SIGNAL);
		ENUM_TO_STR(eLiveViewState_PMT_PID_CHANGED);

		ENUM_TO_STR(eLiveViewState_PID_START);

		ENUM_TO_STR(eLiveViewState_AV);
		ENUM_TO_STR(eLiveViewState_SVC_NOT_RUNNING);

		ENUM_TO_STR(eLiveViewState_CAS);
		ENUM_TO_STR(eLiveViewState_CAS_PREVIEW);
		ENUM_TO_STR(eLiveViewState_PROGRAM_SHOW);


		default:		break;
	}

	return "Unknown eAppState";
}

STATIC HUINT8 *_mgr_LiveView_base_GetSessionTypeStr(MgrLiveView_SessionType_t sessionType)
{
	switch(sessionType)
	{
		ENUM_TO_STR(eLiveViewSession_NORMAL)
		ENUM_TO_STR(eLiveViewSession_MHEG_QUIET)
		ENUM_TO_STR(eLiveViewSession_REPLACE)
		ENUM_TO_STR(eLiveViewSession_SUBFEED)
		ENUM_TO_STR(eLiveViewSession_TSMF_LOW)
		default:			break;
	}

	HxLOG_Print("Unkown WTV session type [%d]\n", sessionType);

	return "UNKNOWN";
}

STATIC HUINT8 *_mgr_LiveView_base_GetMwSvcTypeStr(XmgrLive_MwSvcType_e eMwSvcType)
{
	switch(eMwSvcType)
	{
		ENUM_TO_STR(eLiveMwSvcType_Channel)
		ENUM_TO_STR(eLiveMwSvcType_Si)
		ENUM_TO_STR(eLiveMwSvcType_Cas)
		ENUM_TO_STR(eLiveMwSvcType_Av)
		ENUM_TO_STR(eLiveMwSvcType_Transcoder)
		ENUM_TO_STR(eLiveMwSvcType_Subtitle)
		ENUM_TO_STR(eLiveMwSvcType_Teletext)
		ENUM_TO_STR(eLiveMwSvcType_CloseCaption)
		default:			break;
	}

	HxLOG_Print("Unkown mw service type [%d]\n", eMwSvcType);

	return "UNKNOWN";
}

/*
STATIC HUINT8 *_mgr_LiveView_base_GetMwSvcStateStr(XmgrLive_MwSvcState_e eServicesState)
{
	switch(eServicesState)
	{
		ENUM_TO_STR(eLiveMwSvcState_Unknown)
		ENUM_TO_STR(eLiveMwSvcState_ReqStop)
		ENUM_TO_STR(eLiveMwSvcState_Stopped)
		ENUM_TO_STR(eLiveMwSvcState_ReqStart)
		ENUM_TO_STR(eLiveMwSvcState_Started)
		default:			break;
	}

	HxLOG_Print("Unkown XmgrLive_MwSvcState_e [%d]\n", eServicesState);

	return "UNKNOWN";
}

STATIC HUINT8 *_mgr_LiveView_base_GetMwSvcExecPhaseStr(XmgrLive_MwSvcExecPhase_e eServicesStartPhase)
{
	switch(eServicesStartPhase)
	{
		ENUM_TO_STR(eLiveMwSvcExecPhase_None)
		ENUM_TO_STR(eLiveMwSvcExecPhase_AfterChannelTuned)
		ENUM_TO_STR(eLiveMwSvcExecPhase_AfterAvOut)
		ENUM_TO_STR(eLiveMwSvcExecPhase_AfterPmtReceived)
		default:			break;
	}

	HxLOG_Print("Unkown XmgrLive_MwSvcExecPhase_e [%d]\n", eServicesStartPhase);

	return "UNKNOWN";
}
*/

#if defined(CONFIG_DEBUG)
int XMGR_LiveView_BASE_DBG_DumpContext(void 	*context)
{
	DbSvc_Info_t				 stSvcInfo;

	XmgrLiveView_Context_t	*pstContext = (XmgrLiveView_Context_t *)context;
	if(pstContext  == NULL)
	{
		return -1;
	}

	HLIB_CMD_Printf("\t\tLive Mode [%d, %s]\n", pstContext->eLiveMode, MGR_LIVE_GetLiveModeStr(pstContext->eLiveMode));
	HLIB_CMD_Printf("\t\tAction ID : %d\n", pstContext->ulActionId);
	HLIB_CMD_Printf("\t\tAction Handle : 0x%08X\n", pstContext->hAction);
	HLIB_CMD_Printf("\t\tLiveView_Base State : %s\n", _mgr_LiveView_base_GetAppStateStr(pstContext->eAppState));
	HLIB_CMD_Printf("\t\tUI State : %s\n", _mgr_LiveView_base_GetUiStateStr(pstContext->eUiState));
	HLIB_CMD_Printf("\t\tSession Type : %d\n", pstContext->sessionType);
	//HLIB_CMD_Printf("\t\tService Handle : 0x%X, hMasterSvc : 0x%X\n", pstContext->hSvc, pstContext->hMasterSvc);
	//HLIB_CMD_Printf("\t\tService List Group : %s\n", xmgr_live_GetSvcListGroupStr(pstContext->svcListGroup));

	if (DB_SVC_GetServiceInfo(pstContext->hSvc, &stSvcInfo) != ERR_OK)
	{
		HLIB_CMD_Printf(" pstContext->hSvc(0x%X) ::  DB_SVC_GetServiceInfo() failed.. \n", pstContext->hSvc);
	}
	else
	{
		HLIB_CMD_Printf(" pstContext->hSvc(0x%X) :: svcName(%s) lcn(%d) \n", pstContext->hSvc, DbSvc_GetName(&stSvcInfo), DbSvc_GetLcn(&stSvcInfo));
	}

	if (DB_SVC_GetServiceInfo(pstContext->hMasterSvc, &stSvcInfo) != ERR_OK)
	{
		HLIB_CMD_Printf(" pstContext->hMasterSvc(0x%X) ::  DB_SVC_GetServiceInfo() failed.. \n", pstContext->hMasterSvc);
	}
	else
	{
		HLIB_CMD_Printf(" pstContext->hMasterSvc(0x%X) :: svcName(%s) lcn(%d) \n", pstContext->hMasterSvc, DbSvc_GetName(&stSvcInfo), DbSvc_GetLcn(&stSvcInfo));
	}
	/*
	HLIB_CMD_Printf("\t\tbSubtitleAllowed : %d\n", pstContext->bSubtitleAllowed);
	HLIB_CMD_Printf("\t\tbTtxPageEnabled : %d\n", pstContext->bTtxPageEnabled);
	HLIB_CMD_Printf("\t\tbSiStopped : %d\n", pstContext->bSiStopped);
	HLIB_CMD_Printf("\t\tbCasStopped : %d\n", pstContext->bCasStopped);
	HLIB_CMD_Printf("\t\tbSubtitle_CC_Stopped : %d\n", pstContext->bSubtitle_CC_Stopped);
	HLIB_CMD_Printf("\t\tbTeletextStopped : %d\n", pstContext->bTeletextStopped);
	HLIB_CMD_Printf("\t\tbPmtValid : %d\n", pstContext->bPmtValid);
	HLIB_CMD_Printf("\t\tbCasOk : %d\n", pstContext->bCasOk);
	HLIB_CMD_Printf("\t\tbSCPinCodeValid : %d\n", pstContext->bSCPinCodeValid);
	HLIB_CMD_Printf("\t\tnCasSystemId : %d\n", pstContext->nCasSystemId);
	HLIB_CMD_Printf("\t\tpCasInfo : 0x%x\n", (HINT32)pstContext->pCasInfo);
	HLIB_CMD_Printf("\t\tnCasEvt : %d\n", pstContext->nCasEvt);
	HLIB_CMD_Printf("\t\tnCasShow : %d\n", pstContext->nCasShow);
	HLIB_CMD_Printf("\t\tbAudioShow : %d\n", pstContext->bAudioShow);
	HLIB_CMD_Printf("\t\tbVideoShow : %d\n", pstContext->bVideoShow);
	HLIB_CMD_Printf("\t\tbRadioBackground : %d\n", pstContext->bRadioBackground);
	*/
	HLIB_CMD_Printf("\n\t======================================================\n\n");

	return 0;
}


int XMGR_LiveView_BASE_DBG_PrintContext(void)
{
	HUINT32		i;

	HLIB_CMD_Printf(">> Live view Context <<\n");

	for (i = 0; i < NUM_VIEW_ACTION; i++)
	{

		XmgrLiveView_Context_t 	*pstContext = _mgr_LiveView_base_getContext (i);
		if (NULL == pstContext) 				{ continue; }

		HLIB_CMD_Printf("\n\t======================================================\n");
		HLIB_CMD_Printf("\tLive View Module Context[%d]\n", i);
		XMGR_LiveView_BASE_DBG_DumpContext(pstContext);
	}

	return 0;
}

int XMGR_LiveView_BASE_DBG_PrintCasDrmInfo(void)
{
	HUINT32		i;

	HLIB_CMD_Printf(">> Live view CAS DRM Info <<\n");

	for (i = 0; i < NUM_VIEW_ACTION; i++)
	{

		XmgrLiveView_Context_t 	*pstContext = _mgr_LiveView_base_getContext (i);
		if (NULL == pstContext) 				{ continue; }

		HLIB_CMD_Printf("\n\t======================================================\n");
		HLIB_CMD_Printf("\tLive View Module Context[%d]\n", i);
		if(pstContext->bCasDrmValid == TRUE)
		{
			HLIB_CMD_Printf("\t bCasCc\t: [%d]\n", pstContext->drmInfo.bCasCc);
			if(pstContext->drmInfo.bCasCc == TRUE)
			{
				HLIB_CMD_Printf("\t eCasCcType\t: [%d]\n", pstContext->drmInfo.stCasCcInfo.eCasCcType);
				switch(pstContext->drmInfo.stCasCcInfo.eCasCcType)
				{
					case eDxCopyrightControl_CasCcType_NagraSes:
						HLIB_CMD_Printf("\t eDxCopyrightControl_CasCcType_NagraSes...\n");
						break;
					case eDxCopyrightControl_CasCcType_CiPlus:
						HLIB_CMD_Printf("\t eDxCopyrightControl_CasCcType_CiPlus...\n");
						HLIB_CMD_Printf("\t eCiPlusDrmType\t: [%d]\n", pstContext->drmInfo.stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType);
						break;
					case eDxCopyrightControl_CasCcType_Nagra:
						HLIB_CMD_Printf("\t eDxCopyrightControl_CasCcType_Nagra...\n");
						break;
					case eDxCopyrightControl_CasCcType_Irdeto:
						HLIB_CMD_Printf("\t eDxCopyrightControl_CasCcType_Irdeto...\n");
						break;
					case eDxCopyrightControl_CasCcType_Viaccess:
						HLIB_CMD_Printf("\t eDxCopyrightControl_CasCcType_Viaccess...\n");
						break;
					case eDxCopyrightControl_CasCcType_Verimatrix:
						HLIB_CMD_Printf("\t eDxCopyrightControl_CasCcType_Verimatrix...\n");
						break;
					case eDxCopyrightControl_CasCcType_Conax:
						HLIB_CMD_Printf("\t eDxCopyrightControl_CasCcType_Conax...\n");
						break;
					default:
						break;
				}
			}
		}
	}

	return 0;
}


int XMGR_LiveView_BASE_DBG_PmtReload(HUINT32 ulActionId, HBOOL bPmtTimeout)
{
#if 0
	if (bPmtTimeout)
	{
		BUS_PostMessage(NULL, eSEVT_SI_PMT_TIMEOUT, 	SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);
	}

	BUS_PostMessage(NULL, eSEVT_SI_PMT, 	SVC_PIPE_GetActionHandle(ulActionId), 0, 0, 0);
#endif

	return ERR_BUS_NO_ERROR;
}


#endif

/* End Of File */
