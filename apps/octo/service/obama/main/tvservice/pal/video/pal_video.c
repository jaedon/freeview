/*******************************************************************
	File Description
********************************************************************/
/**
	@file	pal_video.c
	@brief	pal_video.c

	Description: VIDEO Module. Application으로 부터 요청되는 ViDEO 관련 Command를 처리한다. 			\n
	Module: PAL /Video 												\n

	Copyright (c) 2008 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <di_video.h>
#include <di_demux.h>
#include <di_scart.h>
#include <di_osd.h>
#include <di_init.h>

#include <get_enum_str.h>
#include <hlib.h>

#include <pal_video.h>
#include <pal_sef.h>
#include <pal_hdmi.h>
#include <pal_scart.h>
#include <pal_output.h>
#include <pal_scaler.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define FCC_DEBUG
#if defined(FCC_DEBUG)
#define HxLOG_FCC			MMMP
#else
#define HxLOG_FCC			HxLOG_Debug
#endif

#define	palVideo_ENTER_CRITICAL		{VK_SEM_Get(s_ulPalVideoSemId); /*HxLOG_Debug("*** Enter C_PAL_VIDEO line %d\n", __LINE__);*/}
#define	palVideo_LEAVE_CRITICAL		{VK_SEM_Release(s_ulPalVideoSemId); /*HxLOG_Debug("*** Leave C_PAL_VIDEO line %d\n", __LINE__);*/}

//#define __VIDEO_SHOW_CALL_TRACE__
#ifdef __VIDEO_SHOW_CALL_TRACE__
#define pal_video_Show(a, b) 	_pal_video_Show(a, b, __FUNCTION__, __LINE__)
#endif

//#define __VIDEO_PRIMER_CALL_TRACE__
#ifdef __VIDEO_PRIMER_CALL_TRACE__
#define VPC_Print	HxLOG_Warning
#else
#define VPC_Print
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/* @brief 	Video Device Context의 State ..... */

typedef struct
{
	DecoderState_t 				eState;
	HBOOL 						bFreezed;

	HBOOL						bStillPicture;

	VideoStreamInfo_t			stStreamInfo; 					/**< MPEG Input의 Header 정보들을 기록 */
	HBOOL 						bStreamInfoValid;

	AV_HideClient_t				ulHideRequest;

	HUINT32						ulDiDecId;

	HBOOL						bDiShow;

#if defined(CONFIG_SUPPORT_FCC)
	HBOOL						bPrimerStarted;				/* video primer state */
#endif

} palVideo_Context_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG							s_ulPalVideoSemId;

STATIC HULONG							s_ulPalVideoEventMsgQId;
STATIC HULONG							s_ulPalVideoEventTaskId;

STATIC HULONG							s_ulPalVideoErrorInfoMsgQId;
STATIC HULONG							s_ulPalVideoErrorInfoTaskId;

STATIC palVideo_Context_t 				s_stVideoContext[MAX_VIDEO_CONTEXT];

STATIC HUINT32 							s_ulMaxVideoDecoder;
STATIC DI_VIDEO_CAP_t 					*s_pstDiVideoCapability;
STATIC HBOOL							s_ulMainDiDecId;
STATIC PalVideo_ChannelChangeMode_e		s_ePalChannelChangeMode;
STATIC HBOOL							s_bShowHideControlEnabled = TRUE;

STATIC void (* s_pfnEventCallback)(HUINT32, PalVideo_Event_e);
STATIC void (* s_pfnEventCallbackForCas)(HUINT32, PalVideo_Event_e);
STATIC void (* s_pfnEventCallbackForVideoError)(HUINT32, HUINT32);

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

const HINT8 *PAL_VIDEO_GetStrDeviceEvt(PalVideo_Event_e eVideoDeviceEvent)
{
	switch (eVideoDeviceEvent)
	{
		ENUM_TO_STR(eVIDEO_EVENT_NONE);
		ENUM_TO_STR(eVIDEO_EVENT_PARAM_CHANGED);
		ENUM_TO_STR(eVIDEO_EVENT_FRAME_START);
		ENUM_TO_STR(eVIDEO_EVENT_UNDERRUN);
		default: break;
	}
	return ("Unknown");
}

STATIC const HINT8 *pal_video_GetStrContextState(DecoderState_t eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eDECODER_STATE_STOPPED);
		ENUM_TO_STR(eDECODER_STATE_STARTING);
		ENUM_TO_STR(eDECODER_STATE_DECODING);
		ENUM_TO_STR(eDECODER_STATE_UNDERRUN);
		default: break;
	}
	return ("Unknown");
}

STATIC const HINT8 *pal_video_GetStrDiEvent(DI_VIDEO_NOTIFY_EVT eDiVideoNotifyEvt)
{
	switch (eDiVideoNotifyEvt)
	{
		ENUM_TO_STR(DI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED);
		ENUM_TO_STR(DI_VIDEO_NOTIFY_EVT_FORMAT_CHANGED);
		ENUM_TO_STR(DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED);
		ENUM_TO_STR(DI_VIDEO_NOTIFY_EVT_AFD_CHANGED);
		ENUM_TO_STR(DI_VIDEO_NOTIFY_EVT_ATV_SYNC_CHANGED);
		ENUM_TO_STR(DI_VIDEO_NOTIFY_EVT_3D_INFO_CHANGED);
		ENUM_TO_STR(DI_VIDEO_NOTIFY_EVT_END);
		default: break;
	}
	return ("Unknown");
}

#define _____________CONVERT___________________________________________________________________________________________________________________________

STATIC HERROR pal_video_ConvertPal2Di_Codec(DxVideoCodec_e ePalCodec, DI_VIDEO_FORMAT *peDiCodec)
{
	if(peDiCodec == NULL)
	{
		return ERR_FAIL;
	}

	/* Codec Conversion */
	switch(ePalCodec)
	{
		case eDxVIDEO_CODEC_MPEG1:
			*peDiCodec = DI_VIDEO_FORMAT_MPEG1;
			break;
		case eDxVIDEO_CODEC_MPEG2:
			*peDiCodec = DI_VIDEO_FORMAT_MPEG2;
			break;
		case eDxVIDEO_CODEC_MPEG4_PART2:
			*peDiCodec = DI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case eDxVIDEO_CODEC_H263:
			*peDiCodec = DI_VIDEO_FORMAT_H263;
			break;
		case eDxVIDEO_CODEC_H264:
			*peDiCodec = DI_VIDEO_FORMAT_H264;
			break;
		case eDxVIDEO_CODEC_H265:
			*peDiCodec = DI_VIDEO_FORMAT_H265;
			break;
		case eDxVIDEO_CODEC_H264_SVC:
			// TODO: DI_VIDEO.h Have to be changed.....
			*peDiCodec = DI_VIDEO_FORMAT_NONE;
			break;
		case eDxVIDEO_CODEC_H264_MVC:
			// TODO: DI_VIDEO.h Have to be changed.....
			*peDiCodec = DI_VIDEO_FORMAT_NONE;
			break;
		case eDxVIDEO_CODEC_VC1:
			*peDiCodec = DI_VIDEO_FORMAT_VC1;
			break;
		case eDxVIDEO_CODEC_VC1_SIMPLEMAIN:
			*peDiCodec = DI_VIDEO_FORMAT_VC1SimpleMain;
			break;
		case eDxVIDEO_CODEC_AVS:
			*peDiCodec = DI_VIDEO_FORMAT_AVS;
			break;
		case eDxVIDEO_CODEC_RV40:
			// TODO: DI_VIDEO.h Have to be changed.....
			*peDiCodec = DI_VIDEO_FORMAT_NONE;
			break;
		case eDxVIDEO_CODEC_VP6:
			// TODO: DI_VIDEO.h Have to be changed.....
			*peDiCodec = DI_VIDEO_FORMAT_NONE;
			break;
		case eDxVIDEO_CODEC_DIVX311:
			*peDiCodec = DI_VIDEO_FORMAT_DIVX_311;
			break;

		default:
			*peDiCodec = DI_VIDEO_FORMAT_MPEG2;
			HxLOG_Error("not defined or supported pal codec (%s) \n", OTL_ENUM2STR_VideoCodec(ePalCodec));
			break;
	}

	return ERR_OK;
}

STATIC HERROR pal_video_ConvertDi2Pal_FrameRate(DI_VIDEO_FrameRate_t eDiFrameRate, VIDEO_FrameRate_t *pePalFrameRate)
{
	if(pePalFrameRate == NULL)
	{
		return ERR_FAIL;
	}

	switch(eDiFrameRate)
	{
		case DI_VIDEO_FRAMERATE_24:
			*pePalFrameRate = eFrameRate_24;
			break;
		case DI_VIDEO_FRAMERATE_25:
			*pePalFrameRate = eFrameRate_25;
			break;
		case DI_VIDEO_FRAMERATE_30:
			*pePalFrameRate = eFrameRate_30;
			break;
		case DI_VIDEO_FRAMERATE_50:
			*pePalFrameRate = eFrameRate_50;
			break;
		case DI_VIDEO_FRAMERATE_60:
			*pePalFrameRate = eFrameRate_60;
			break;
		default:
			*pePalFrameRate = eFrameRate_Unknown;
			break;
	}

	return ERR_OK;
}

STATIC HERROR pal_video_ConvertDi2Pal_AspectRatio(DI_VIDEO_AspectRatio_t eDiAspectRatio, DxAspectRatio_e *pePalAspectRatio)
{
	if(pePalAspectRatio == NULL)
	{
		return ERR_FAIL;
	}

	switch(eDiAspectRatio)
	{
		case DI_VIDEO_ASPECTRATIO_16_9:
			*pePalAspectRatio = eDxASPECTRATIO_16X9;
			break;
		case DI_VIDEO_ASPECTRATIO_4_3:
			*pePalAspectRatio = eDxASPECTRATIO_4X3;
			break;
		case DI_VIDEO_ASPECTRATIO_14_9:
			*pePalAspectRatio = eDxASPECTRATIO_14X9;
			break;
		case DI_VIDEO_ASPECTRATIO_1_1:
			*pePalAspectRatio = eDxASPECTRATIO_1X1;
			break;
		default:
			*pePalAspectRatio = eDxASPECTRATIO_UNKNOWN;
			break;
	}

	return ERR_OK;
}

STATIC HERROR pal_video_ConvertDi2Pal_Afd(DI_VIDEO_ACTIVE_FORMAT eDiAfd, VIDEO_Afd_t *pePalAfd)
{
	if(pePalAfd == NULL)
	{
		return ERR_FAIL;
	}

	switch(eDiAfd)
	{
		case DI_VIDEO_AFD_SAME_AS_CODED_FRAME:
			*pePalAfd = eAfd_Full;
			break;
		case DI_VIDEO_AFD_RESERVED:
			*pePalAfd = eAfd_Full;
			break;
		case DI_VIDEO_AFD_4_3_CENTRE:
			*pePalAfd = eAfd_43;
			break;
		case DI_VIDEO_AFD_16_9_CENTRE:
			*pePalAfd = eAfd_169;
			break;
		case DI_VIDEO_AFD_14_9_CENTRE:
			*pePalAfd = eAfd_149;
			break;
		case DI_VIDEO_AFD_4_3_SNP_14_9_CENTRE:
			*pePalAfd = eAfd_43_SnP149;
			break;
		case DI_VIDEO_AFD_16_9_SNP_14_9_CENTRE:
			*pePalAfd = eAfd_169_SnP149;
			break;
		case DI_VIDEO_AFD_16_9_SNP_4_3_CENTRE:
			*pePalAfd = eAfd_169_SnP43;
			break;
		default:
			*pePalAfd = eAfd_None;
			break;
	}

	return ERR_OK;
}

STATIC HERROR pal_video_ConvertDi2Pal_3DFormat(DI_VIDEO_3D_FORMAT eDi3DFormat, DxVideo3DFormat_e *pe3DFormat)
{
	switch(eDi3DFormat)
	{
		case DI_VIDEO_3D_FORMAT_SIDEBYSIDE:
			*pe3DFormat = eDxVIDEO_3DFORMAT_SIDEBYSIDE;
			break;

		case DI_VIDEO_3D_FORMAT_TOPANDBOTTOM:
			*pe3DFormat = eDxVIDEO_3DFORMAT_TOPANDBOTTOM;
			break;

		case DI_VIDEO_3D_FORMAT_FRAMEPACKING:
			*pe3DFormat = eDxVIDEO_3DFORMAT_FRAMEPACKING;
			break;

		default:
			*pe3DFormat = eDxVIDEO_3DFORMAT_NONE;
			break;
	}

	return ERR_OK;
}

#define _____DEVICE_EVENT_________________________________________________________________________________________________
STATIC HERROR pal_video_CheckDecId(HUINT32 ulDecId)
{
	if(ulDecId < MAX_VIDEO_CONTEXT)
	{
		return ERR_OK;
	}
	else
	{
		HxLOG_Error("**** Invalid decoder ID [%d]\n", ulDecId);
		return ERR_FAIL;
	}
}

STATIC palVideo_Context_t *pal_video_GetVideoContext(HUINT32 ulDecId)
{
	return &s_stVideoContext[ulDecId];
}

STATIC HERROR pal_video_UpdateStreamInfo(HUINT32 ulDecId, HBOOL *pbParamChanged)
{
	palVideo_Context_t				*pstVideoContext;
	VideoStreamInfo_t			stStreamInfo = {0, };

	DI_VIDEO_StreamInfo_t 		stDiStreamInfo;
	DI_VIDEO_ACTIVE_FORMAT		eDiAfdMode;
	DI_VIDEO_3D_FORMAT			eDi3DFormat;
	DI_ERR_CODE 				diErr;
	HUINT32						ulDiDecId = ulDecId;

	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, ulDecId);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		return ERR_FAIL;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

#if defined(CONFIG_SUPPORT_FCC)
	ulDiDecId = pstVideoContext->ulDiDecId;
#endif

	/* 각 Group의 main dec 에 대해서만 처리 함. */
	diErr = DI_VIDEO_GetSequenceHeaderInfo(ulDiDecId, &stDiStreamInfo);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\t\tError from DI_VIDEO_GetSequenceHeaderInfo()\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("\t\tW(%d), H(%d), DW(%d), DH(%d), AR(%d), FR(%d), CODEC(%d), PROG(%d)\n"
									,stDiStreamInfo.ulWidth
									,stDiStreamInfo.ulHeight
									,stDiStreamInfo.ulDisplayWidth
									,stDiStreamInfo.ulDisplayHeight
									,stDiStreamInfo.eAspectRatio
									,stDiStreamInfo.eFrameRate
									,stDiStreamInfo.eVideoFormat
									,stDiStreamInfo.bProgressive
									);

	if( (stDiStreamInfo.ulWidth == 0) || (stDiStreamInfo.ulHeight == 0) )
	{
		HxLOG_Error("\n\n\t\t============== PANIC !!! Invalid Sequence Header Params ===============\n\n");
		HxLOG_Error("\t\t\t\tWidth(%d), Height(%d)\n\n", stDiStreamInfo.ulWidth, stDiStreamInfo.ulHeight);
		HxLOG_Error("\t\t============== PANIC !!! Invalid Sequence Header Params ===============\n\n");

		return ERR_FAIL;
	}

	stStreamInfo.ulHorizontalSize = stDiStreamInfo.ulWidth;
	stStreamInfo.ulVerticalSize = stDiStreamInfo.ulHeight;
	stStreamInfo.ulDisplayHorizontalSize = stDiStreamInfo.ulDisplayWidth;
	stStreamInfo.ulDisplayVerticalSize = stDiStreamInfo.ulDisplayHeight;
	stStreamInfo.bProgressive = stDiStreamInfo.bProgressive;

	pal_video_ConvertDi2Pal_FrameRate(stDiStreamInfo.eFrameRate, &stStreamInfo.eFrameRate);
	pal_video_ConvertDi2Pal_AspectRatio(stDiStreamInfo.eAspectRatio, &stStreamInfo.eAspectRatio);

	diErr = DI_VIDEO_GetAfdInfo(ulDiDecId, &eDiAfdMode);
	if(diErr == ERR_OK)
	{
		pal_video_ConvertDi2Pal_Afd(eDiAfdMode, &stStreamInfo.eAfd);
	}
	else
	{
		stStreamInfo.eAfd = eAfd_None;
	}

	if( (stStreamInfo.eAfd == eAfd_None) || (stStreamInfo.eAfd == eAfd_Full) )
	{ /* AFD가 전송되지 않았거나 사양에 없는 경우의 sequence display extenstion 처리 */
		if(stStreamInfo.eAspectRatio == eDxASPECTRATIO_16X9)
		{
			if( (stStreamInfo.ulDisplayVerticalSize > 0)
				&& (stStreamInfo.ulVerticalSize > stStreamInfo.ulDisplayVerticalSize + 8)
			)
			{ /* 1920x1088을 제외하고 720x360과 같이 16x9 display 영역을 갖는 경우 */
				stStreamInfo.eAspectRatio = eDxASPECTRATIO_4X3;
				stStreamInfo.eAfd = eAfd_169;
			}
			else
			{
				stStreamInfo.eAfd = eAfd_Full;
			}
		}
		else if(stStreamInfo.eAspectRatio == eDxASPECTRATIO_4X3)
		{
			if( (stStreamInfo.ulDisplayHorizontalSize > 0)
				&& (stStreamInfo.ulHorizontalSize > stStreamInfo.ulDisplayHorizontalSize + 16)
			)
			{ /* 720(540,480)x480, 1920(1440,1080)x1080, 1280(960)x720 AR(4:3)case 처리. */
				stStreamInfo.eAspectRatio = eDxASPECTRATIO_16X9;
				stStreamInfo.eAfd = eAfd_43;
			}
			else
			{
				stStreamInfo.eAfd = eAfd_Full;
			}
		}
		else
		{
			stStreamInfo.eAfd = eAfd_Full;
		}
	}
	else
	{
		/* AFD가 available. 그대로 사용. */
	}

	stStreamInfo.ulChromaFormat = stDiStreamInfo.ucChromaFormat;
	stStreamInfo.bColorPrimarie = stDiStreamInfo.ucColorPrimarie;
	stStreamInfo.bAlpha = stDiStreamInfo.ucAlpha;

	diErr = DI_VIDEO_Get3DInfo(ulDiDecId, &eDi3DFormat);
	if(diErr == DI_ERR_OK)
	{
		pal_video_ConvertDi2Pal_3DFormat(eDi3DFormat, &stStreamInfo.e3DFormat);
	}
	else
	{
		HxLOG_Error("\t\tERROR from DI_VIDEO_Get3DInfo() !!!\n");
		stStreamInfo.e3DFormat = eDxVIDEO_3DFORMAT_NONE;
	}

	/* 파라미터 변경 확인 */
	*pbParamChanged = FALSE;

	if(stStreamInfo.ulHorizontalSize != pstVideoContext->stStreamInfo.ulHorizontalSize)
	{
		HxLOG_Debug("\t\tulHorizontalSize [%d] => [%d]\n"
				, pstVideoContext->stStreamInfo.ulHorizontalSize
				, stStreamInfo.ulHorizontalSize
				);

		*pbParamChanged = TRUE;
	}

	if(stStreamInfo.ulVerticalSize != pstVideoContext->stStreamInfo.ulVerticalSize)
	{
		HxLOG_Debug("\t\tulVerticalSize [%d] => [%d]\n"
				, pstVideoContext->stStreamInfo.ulVerticalSize
				, stStreamInfo.ulVerticalSize
				);

		*pbParamChanged = TRUE;
	}

	if(stStreamInfo.eAspectRatio != pstVideoContext->stStreamInfo.eAspectRatio)
	{
		HxLOG_Debug("\t\teAspectRatio [%s] => [%s]\n"
				, OTL_ENUM2STR_AspectRatio(pstVideoContext->stStreamInfo.eAspectRatio)
				, OTL_ENUM2STR_AspectRatio(stStreamInfo.eAspectRatio)
				);

		*pbParamChanged = TRUE;
	}

	if(stStreamInfo.eAfd != pstVideoContext->stStreamInfo.eAfd)
	{
		HxLOG_Debug("\t\teAfd [%s] => [%s]\n"
				, OTL_ENUM2STR_Afd(pstVideoContext->stStreamInfo.eAfd)
				, OTL_ENUM2STR_Afd(stStreamInfo.eAfd)
				);

		*pbParamChanged = TRUE;
	}

	if(stStreamInfo.eFrameRate != pstVideoContext->stStreamInfo.eFrameRate)
	{
		HxLOG_Debug("\t\teFrameRate [%s] => [%s]\n"
				, OTL_ENUM2STR_VideoFrameRate(pstVideoContext->stStreamInfo.eFrameRate)
				, OTL_ENUM2STR_VideoFrameRate(stStreamInfo.eFrameRate)
				);

		*pbParamChanged = TRUE;
	}

	if(stStreamInfo.bProgressive != pstVideoContext->stStreamInfo.bProgressive)
	{
		HxLOG_Debug("\t\tbProgressive [%d] => [%d]\n"
				, pstVideoContext->stStreamInfo.bProgressive
				, stStreamInfo.bProgressive
				);

		*pbParamChanged = TRUE;
	}

	if(stStreamInfo.e3DFormat != pstVideoContext->stStreamInfo.e3DFormat)
	{
		HxLOG_Debug("\t\te3DFormat [%d] => [%d]\n"
				, pstVideoContext->stStreamInfo.e3DFormat
				, stStreamInfo.e3DFormat
				);

		*pbParamChanged = TRUE;
	}

	pstVideoContext->bStreamInfoValid = TRUE;
	pstVideoContext->stStreamInfo = stStreamInfo;

	return ERR_OK;
}

STATIC void pal_video_NotifyEvent(HUINT32 ulDecId, PalVideo_Event_e eVideoEvent)
{
	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, ulDecId);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		return;
	}

	/* AV Service 에서 걸어놓은 callback 호출 */
	if(s_pfnEventCallback != NULL)
	{
		s_pfnEventCallback(ulDecId, eVideoEvent);
	}

#ifdef CONFIG_MW_CI
	if(eVideoEvent == eVIDEO_EVENT_UNDERRUN || eVideoEvent == eVIDEO_EVENT_FRAME_START)
	{
		if(s_pfnEventCallbackForCas != NULL)
		{
			s_pfnEventCallbackForCas(ulDecId, eVideoEvent);
		}
	}
#endif
}

STATIC void pal_video_EventTask(void)
{
	HUINT32							nResult = 0;
	HERROR							err;
	HUINT32							ulDecId;
	HBOOL							bStreamInfoChanged;
	palVideo_Context_t					*pstVideoContext;
	PalVideo_Event_e				eVideoEvent;

	DI_VIDEO_CALLBACK_DATA_t		stDiCallbackData;

	while(1)
	{
		nResult = (HUINT32)VK_MSG_Receive(s_ulPalVideoEventMsgQId, &stDiCallbackData, sizeof(DI_VIDEO_CALLBACK_DATA_t));
		if(nResult == ERR_OK)
		{
			palVideo_ENTER_CRITICAL;

#if defined(CONFIG_SUPPORT_FCC)
			ulDecId = s_ulMainDiDecId;
#else
			ulDecId = stDiCallbackData.ulDecId;
#endif
			HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, ulDecId);

			pstVideoContext = pal_video_GetVideoContext(ulDecId);

			HxLOG_Debug("\t\tDI event [%s] in [%s] state.\n"
						, pal_video_GetStrDiEvent(stDiCallbackData.eEvent)
						, pal_video_GetStrContextState(pstVideoContext->eState)
						);

			eVideoEvent = eVIDEO_EVENT_NONE;

			switch (stDiCallbackData.eEvent)
			{
				case DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED:
					switch( ((DI_VIDEO_RUNNING_STATE_EVT_t *)stDiCallbackData.pvEventData)->state )
					{
						case DI_VIDEO_RUNNING_STATE_DECODING :
							HxLOG_Debug("\t\t\tDI Running State <DI_VIDEO_RUNNING_STATE_DECODING>\n");

							if( (pstVideoContext->eState == eDECODER_STATE_STARTING)
								|| (pstVideoContext->eState == eDECODER_STATE_UNDERRUN)
							)
							{ /* STARTING은 디코딩 시작 후 딱 한번만 들어 갈 수 있다. */
								err = pal_video_UpdateStreamInfo(ulDecId, &bStreamInfoChanged);
								if(err == ERR_OK)
								{
									pstVideoContext->eState = eDECODER_STATE_DECODING;
									HxLOG_Debug("\t\tChange video state to [%s]\n", pal_video_GetStrContextState(pstVideoContext->eState));

									eVideoEvent = eVIDEO_EVENT_FRAME_START;
								}
								else
								{ /* 디코딩이 시작되었는데 stream info가 valid 하지 않다. */
									HxLOG_Debug("\t\tDecoding started but still invalid stream info. Should be wait more ...\n");
								}
							}
							else
							{
								HxLOG_Debug("\t\tInvalid DI event delivered in [%s]. Exit ...\n", pal_video_GetStrContextState(pstVideoContext->eState));
							}

							break;

						case DI_VIDEO_RUNNING_STATE_NODATA :
							HxLOG_Debug("\t\t\tDI Running State <DI_VIDEO_RUNNING_STATE_NODATA>\n");

							if( (pstVideoContext->eState == eDECODER_STATE_STARTING)
								|| (pstVideoContext->eState == eDECODER_STATE_DECODING)
							)
							{
								pstVideoContext->eState = eDECODER_STATE_UNDERRUN;
								HxLOG_Debug("\t\tChange video state to [%s]\n", pal_video_GetStrContextState(pstVideoContext->eState));

								eVideoEvent = eVIDEO_EVENT_UNDERRUN;
							}
							else
							{
								HxLOG_Debug("\t\tInvalid DI event delivered in [%s]. Exit ...\n", pal_video_GetStrContextState(pstVideoContext->eState));
							}

							break;

						default :
							HxLOG_Error("\t\t<DI_VIDEO_RUNNING_STATE_UNKNOWN> DI event delivered in [%s]. Exit ...\n", pal_video_GetStrContextState(pstVideoContext->eState));
							break;
					}

					break;

				case DI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED:
				case DI_VIDEO_NOTIFY_EVT_FORMAT_CHANGED:
				case DI_VIDEO_NOTIFY_EVT_AFD_CHANGED:
				case DI_VIDEO_NOTIFY_EVT_3D_INFO_CHANGED:

					if(pstVideoContext->eState == eDECODER_STATE_DECODING)
					{
						err = pal_video_UpdateStreamInfo(ulDecId, &bStreamInfoChanged);
						if(err == ERR_OK)
						{
							if(bStreamInfoChanged == TRUE)
							{
								eVideoEvent = eVIDEO_EVENT_PARAM_CHANGED;
							}
						}
						else
						{ /* 파라미터가 바뀌었다고 하는데 stream info가 valid 하지 않다. */
							HxLOG_Error("\t\tWhy ERROR from DI stream info -2- !!!\n");
						}
					}
					else
					{
						HxLOG_Debug("\t\tInvalid DI event delivered in [%s]. Exit ...\n", pal_video_GetStrContextState(pstVideoContext->eState));
					}

					break;

				default:
					break;

			}

			palVideo_LEAVE_CRITICAL;

			if(eVideoEvent != eVIDEO_EVENT_NONE)
			{
				pal_video_NotifyEvent(ulDecId, eVideoEvent);
			}
		}
	}

	return;
}

STATIC void pal_video_ErrorInfoTask(void)
{
	HUINT32						nResult = 0;
	HUINT32						ulDecodeError = 0;
	HUINT32						ulDisplayError = 0;

	DI_VIDEO_CALLBACK_DATA_t	stDiCallbackData;
	DI_VIDEO_ERROR_INFO_EVT_t	*pstVideoErrorInfo = NULL;

	while(1)
	{
		nResult = (HUINT32)VK_MSG_Receive(s_ulPalVideoErrorInfoMsgQId, &stDiCallbackData, sizeof(DI_VIDEO_CALLBACK_DATA_t));
		if(nResult == ERR_OK)
		{
			pstVideoErrorInfo = (DI_VIDEO_ERROR_INFO_EVT_t*)stDiCallbackData.pvEventData;
			if(pstVideoErrorInfo == NULL)
			{
				HxLOG_Error("\t%sVideoDevice Callback Pointer is NULL\n", __FUNCTION__);
				return;
			}

			ulDecodeError = pstVideoErrorInfo->ulDecodeError;
			ulDisplayError = pstVideoErrorInfo->ulDisplayError;

			/* Log Capter 에서 걸어놓은 callback 호출 */
			if(s_pfnEventCallbackForVideoError != NULL)
			{
				s_pfnEventCallbackForVideoError(ulDecodeError, ulDisplayError);
			}
		}
	}

	return;
}

STATIC void pal_video_DiEventCallback(void *pParam)
{
	HUINT32							nResult = 0;
	HUINT32							ulDecId;
	DI_VIDEO_CALLBACK_DATA_t		stDiVideoCallbackData;

	if(pParam == NULL)
	{
		HxLOG_Error("\t%s() VideoDevice Callback Pointer is NULL\n", __FUNCTION__);
		return;
	}

	stDiVideoCallbackData = *(DI_VIDEO_CALLBACK_DATA_t *)pParam;

	ulDecId = stDiVideoCallbackData.ulDecId;
	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		HxLOG_Error("\t%s(%d) Invalid decoder ID. Exit ...\n", __FUNCTION__, ulDecId);
		return;
	}
	else
	{
		HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, ulDecId);
	}

	nResult = (HUINT32)VK_MSG_SendTimeout(s_ulPalVideoEventMsgQId, &stDiVideoCallbackData, sizeof(DI_VIDEO_CALLBACK_DATA_t), 0);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("\n");
	}
}

STATIC HERROR pal_video_ResetContext(HUINT32 ulDecId)
{
	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, ulDecId);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		return ERR_FAIL;
	}

	s_stVideoContext[ulDecId].eState = eDECODER_STATE_STOPPED;
	s_stVideoContext[ulDecId].bFreezed = FALSE;
	s_stVideoContext[ulDecId].bStillPicture = FALSE;

	HxSTD_memset(&s_stVideoContext[ulDecId].stStreamInfo, 0x00, sizeof(VideoStreamInfo_t));
	s_stVideoContext[ulDecId].bStreamInfoValid = FALSE;

	return ERR_OK;
}

#ifdef __VIDEO_SHOW_CALL_TRACE__
STATIC void _pal_video_Show(HUINT32 ulDeviceId, HBOOL bShow, HCHAR *szFunc, HINT32 nLine)
{
	HxLOG_Print("call from <%s:%d>\n", szFunc, nLine);

	if( s_bShowHideControlEnabled == FALSE )
		return;

	if(eChannelChangeMode_MuteUntilFirstPicture != s_ePalChannelChangeMode)
	{
		HxLOG_Print("call DI_VIDEO_Show(), ulDeviceId(%d), bShow(%d)\n", ulDeviceId, bShow);
		(void)DI_VIDEO_Show(ulDeviceId, bShow);
	}
	else
	{
		HxLOG_Print(HxANSI_COLOR_YELLOW("NO CALL(%d), DI_VIDEO_Show(), ulDeviceId(%d), bShow(%d)\n"), s_ePalChannelChangeMode, ulDeviceId, bShow);
	}
}
#else
STATIC void pal_video_Show(HUINT32 ulDeviceId, HBOOL bShow)
{
	if( s_bShowHideControlEnabled == FALSE )
		return;

	if(eChannelChangeMode_MuteUntilFirstPicture != s_ePalChannelChangeMode)
	{
		(void)DI_VIDEO_Show(ulDeviceId, bShow);
	}
}
#endif

#if defined(CONFIG_DEBUG)
STATIC void pal_video_PrintStreamInfo(HUINT32 ulDecId)
{
	palVideo_Context_t *pstVideoContext = &s_stVideoContext[ulDecId];

	/* 입력한 Context number에 해당하는 Context의 모든 정보를 출력한다. */
	HLIB_CMD_Printf("===================== Input Video Stream Info. of VIDEO[%d] ============= \n", ulDecId);
	HLIB_CMD_Printf("\t videoCodec (%s)\n", 				OTL_ENUM2STR_VideoCodec(pstVideoContext->stStreamInfo.eVideoCodec));
	HLIB_CMD_Printf("\t horizontalSize (%d)\n",			pstVideoContext->stStreamInfo.ulHorizontalSize);
	HLIB_CMD_Printf("\t verticalSize (%d)\n", 				pstVideoContext->stStreamInfo.ulVerticalSize);
	HLIB_CMD_Printf("\t ulDisplayHorizontalSize (%d)\n",	pstVideoContext->stStreamInfo.ulDisplayHorizontalSize);
	HLIB_CMD_Printf("\t ulDisplayVerticalSize (%d)\n", 	pstVideoContext->stStreamInfo.ulDisplayVerticalSize);
	HLIB_CMD_Printf("\t progressiveFlag (%d)\n",			pstVideoContext->stStreamInfo.bProgressive);
	HLIB_CMD_Printf("\t aspectRatio (%s)\n",				OTL_ENUM2STR_AspectRatio(pstVideoContext->stStreamInfo.eAspectRatio));
	HLIB_CMD_Printf("\t eAfd (%s)\n",						OTL_ENUM2STR_Afd(pstVideoContext->stStreamInfo.eAfd));
	HLIB_CMD_Printf("\t frameRate (%s)\n",					OTL_ENUM2STR_VideoFrameRate(pstVideoContext->stStreamInfo.eFrameRate));
	HLIB_CMD_Printf("\t compositeStandard (%d)\n",			pstVideoContext->stStreamInfo.eCompositeStandard);
	HLIB_CMD_Printf("\t colorPrimarie (%d)\n",				pstVideoContext->stStreamInfo.bColorPrimarie);
	HLIB_CMD_Printf("\t alpha (%d)\n",						pstVideoContext->stStreamInfo.bAlpha);
	HLIB_CMD_Printf("\t 3D (%d)\n",						pstVideoContext->stStreamInfo.e3DFormat);
	HLIB_CMD_Printf("=============================================================== \n\n");

	return;
}

STATIC void pal_video_PrintContextStatus(void)
{
	HUINT32 i;


	for(i =0; i<MAX_VIDEO_CONTEXT; i++)
	{
		HLIB_CMD_Printf("----------- PAL_VIDEO Decoder[%d] Status -------------------------- \n", i);
		HLIB_CMD_Printf("eState : %s\n", pal_video_GetStrContextState(s_stVideoContext[i].eState));
		HLIB_CMD_Printf("bFreezed : %d\n", s_stVideoContext[i].bFreezed);
		HLIB_CMD_Printf("bStillPicture : %d\n", s_stVideoContext[i].bStillPicture);
		HLIB_CMD_Printf("Hide Request : 0x%x\n", s_stVideoContext[i].ulHideRequest);
		HLIB_CMD_Printf("bStreamInfoValid : %d\n", s_stVideoContext[i].bStreamInfoValid);
		HLIB_CMD_Printf("ulDiDecId : %d\n", s_stVideoContext[i].ulDiDecId);

		if(s_stVideoContext[i].bStreamInfoValid == TRUE)
		{
			pal_video_PrintStreamInfo(i);
		}
		HLIB_CMD_Printf("   \n");
	}

}

STATIC int pal_video_ProcessCommand_status(void *szArgList)
{
	pal_video_PrintContextStatus();

	return HxCMD_OK;
}

STATIC int pal_video_ProcessCommand_ping(void *szArgList)
{
	HLIB_CMD_Printf("Ping pal_video API semaphore ...\n");

	palVideo_ENTER_CRITICAL;

	HLIB_CMD_Printf("\t\ts_ulPalVideoSemId alive ...\n");

	palVideo_LEAVE_CRITICAL;

	return HxCMD_OK;
}

#define	hCmdHandle		"pal_video"

STATIC void pal_video_RegisterCommand(void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_video_ProcessCommand_status,
						"status",
						"Print video output scaling status.",
						"status");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_video_ProcessCommand_ping,
						"ping",
						"Probe if semaphore is alive.",
						"ping");
}
#endif

/********************************************************************
 Global Function
 ********************************************************************/

#define _____PUBLIC_APIs________________________________________________________________________________________________

HERROR PAL_VIDEO_Init(void)
{
	DI_ERR_CODE		diErr;
	HUINT32			i;
	HUINT32			callback_id = 0;												/* DI 요청사항. 실제 사용 안함 */
	HINT32			nResult;

	s_ePalChannelChangeMode = eChannelChangeMode_Mute;

	diErr = DI_VIDEO_GetCapability(&s_ulMaxVideoDecoder, &s_pstDiVideoCapability);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tCritical Eror!!!\n");
	}

	nResult = VK_SEM_Create(&s_ulPalVideoSemId, "PalVideoSem", VK_SUSPENDTYPE_PRIORITY);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("\tERROR VK_SEM_Create ret:0x%X\n",nResult);
	}

	/* Message Q & task for DI event callback */
	if(VK_MSG_Create(64, sizeof(DI_VIDEO_CALLBACK_DATA_t), "PalVideoEventMsgQ", &s_ulPalVideoEventMsgQId, VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	if(VK_TASK_Create((void*)pal_video_EventTask,
					PAL_DICALLBACK_TASK_PRIORITY,
					PAL_VIDEO_TASK_STACK_SIZE,
					"PalVideoEventTask",
					NULL,
					&s_ulPalVideoEventTaskId,
					0) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	if(VK_TASK_Start(s_ulPalVideoEventTaskId) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	/* Message Q & task for DI error info callback */
	if(VK_MSG_Create(64, sizeof(DI_VIDEO_CALLBACK_DATA_t), "PalVideoErrorInfoMsgQ", &s_ulPalVideoErrorInfoMsgQId, VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	if(VK_TASK_Create((void*)pal_video_ErrorInfoTask,
					VK_TASK_PRIORITY_MW_REF,
					PAL_VIDEO_TASK_STACK_SIZE,
					"PalVideoErrorInfoTask",
					NULL,
					&s_ulPalVideoErrorInfoTaskId,
					0) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	if(VK_TASK_Start(s_ulPalVideoErrorInfoTaskId) != ERR_OK)
	{
		HxLOG_Critical(" \n");
		return ERR_FAIL;
	}

	for(i = 0; i < s_ulMaxVideoDecoder; i++)
	{
		pal_video_ResetContext(i);

		s_stVideoContext[i].ulDiDecId = i;
		s_stVideoContext[i].bDiShow = -1;

		if(s_pstDiVideoCapability[i].eDecoderType == DI_VIDEO_DECODER_MAIN)
		{
			s_ulMainDiDecId = i;
		}

		DI_VIDEO_RegisterEventCallback(i, DI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED, pal_video_DiEventCallback, &callback_id);
		DI_VIDEO_RegisterEventCallback(i, DI_VIDEO_NOTIFY_EVT_FORMAT_CHANGED, pal_video_DiEventCallback, &callback_id);
		DI_VIDEO_RegisterEventCallback(i, DI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED, pal_video_DiEventCallback, &callback_id);
		DI_VIDEO_RegisterEventCallback(i, DI_VIDEO_NOTIFY_EVT_AFD_CHANGED, pal_video_DiEventCallback, &callback_id);
		DI_VIDEO_RegisterEventCallback(i, DI_VIDEO_NOTIFY_EVT_3D_INFO_CHANGED, pal_video_DiEventCallback, &callback_id);
	}

#if defined(CONFIG_SUPPORT_FCC)
	for(i = s_ulMaxVideoDecoder; i < MAX_VIDEO_CONTEXT; i++)
	{
		pal_video_ResetContext(i);

		s_stVideoContext[i].ulDiDecId = 0;	// primer idx는 달라도 dec id는 모두 main이다
		s_stVideoContext[i].bDiShow = -1;
	}

	s_ulMainDiDecId = 0;
#endif

#if defined(CONFIG_DEBUG)
	pal_video_RegisterCommand();
#endif

	return ERR_OK;
}

HERROR PAL_VIDEO_RegisterEventCallback(void (*pfnEventCallback)(HUINT32 ulDecId, PalVideo_Event_e eVideoEvent))
{
	s_pfnEventCallback = pfnEventCallback;

	return ERR_OK;
}

HERROR PAL_VIDEO_RegisterEventCallbackForCas(void (*pfnEventCallback)(HUINT32 ulDecId, PalVideo_Event_e eVideoEvent))
{
	s_pfnEventCallbackForCas = pfnEventCallback;

	return ERR_OK;
}

HERROR PAL_VIDEO_StartDecoding(HUINT32 ulDecId, DxVideoCodec_e eVideoCodec)
{
	HERROR 				err = ERR_OK;
	DI_VIDEO_FORMAT 	eDiVideoFormat;
	palVideo_Context_t		*pstVideoContext;

	palVideo_ENTER_CRITICAL;

#if  defined(CONFIG_SUPPORT_FCC)
	// FCC경우 PrimerStartDecoding에서 설정한 DecId가 현재 watching중인 Dec이므로 아래와 같이 설정한다.
 	HxLOG_Print(HxANSI_COLOR_GREEN("Setting s_ulMainDiDecId to ulDecId\n"));
 	s_ulMainDiDecId = ulDecId;
#endif

	HxLOG_Debug("%s(%d, %s)\n", __FUNCTION__, ulDecId, OTL_ENUM2STR_VideoCodec(eVideoCodec));

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(pstVideoContext->eState != eDECODER_STATE_STOPPED)
	{
		HxLOG_Error("\tVideo decoder is NOT stopped yet. PLEASE CHECK YOUR REQUEST !!!! *****\n");
		err = ERR_FAIL;
		goto exit;
	}

	pal_video_ConvertPal2Di_Codec(eVideoCodec, &eDiVideoFormat);

	if( (s_pstDiVideoCapability[pstVideoContext->ulDiDecId].eSupportedFormat & eDiVideoFormat) == 0)
	{
		HxLOG_Error("\tVideo codec is not supported. ulDecId : %d\n", ulDecId);
		err = ERR_FAIL;
		goto exit;
	}

	VPC_Print(HxANSI_COLOR_YELLOW("++ (%d)\n"), pstVideoContext->ulDiDecId);

#if defined(CONFIG_SUPPORT_FCC)
	err = DI_VIDEO_Start(pstVideoContext->ulDiDecId, eDiVideoFormat);
#else
	err = DI_VIDEO_Start(ulDecId, eDiVideoFormat);
#endif

	if (err != ERR_OK)
	{
		HxLOG_Error("\tError(0x%X) occurred in DI_VIDEO_Start().\n", err);
		err = ERR_FAIL;
		goto exit;
	}

	pstVideoContext->eState = eDECODER_STATE_STARTING;

	PAL_SCALER_SetVideoUsage(ulDecId, ePAL_SCALER_VIDEOUSAGE_NORMAL);

exit:
	palVideo_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_VIDEO_StopDecoding(HUINT32 ulDecId)
{
	HERROR				err = ERR_OK;
	DI_ERR_CODE 		diErr;
	palVideo_Context_t		*pstVideoContext;

	palVideo_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulDecId);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(pstVideoContext->eState == eDECODER_STATE_STOPPED)
	{
		HxLOG_Debug("\tVideo decoder already stopped. Ignored *****\n");
		err = ERR_FAIL;
		goto exit;
	}
	else if(pstVideoContext->bStillPicture == TRUE)
	{
		HxLOG_Debug("\tVideo decoder is IN STILL PICTURE mode. Ignored *****\n");
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pal_video_ResetContext(ulDecId);

		VPC_Print(HxANSI_COLOR_YELLOW("-- (%d)\n"), pstVideoContext->ulDiDecId);

#if defined(CONFIG_SUPPORT_FCC)
		diErr = DI_VIDEO_Stop(pstVideoContext->ulDiDecId);
#else
		diErr = DI_VIDEO_Stop(ulDecId);
#endif

		if (diErr != DI_ERR_OK)
		{
			HxLOG_Error("\tError(0x%X) occurred in DI_VIDEO_Stop().\n", diErr);
			err = ERR_FAIL;
			goto exit;
		}
	}

exit:
	palVideo_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_VIDEO_SetFreeze(HUINT32 ulDecId, HBOOL bFreeze)
{
	HERROR				err = ERR_OK;
	DI_ERR_CODE			diErr;
	palVideo_Context_t		*pstVideoContext;

	palVideo_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d, Freeze:%d)\n", __FUNCTION__, ulDecId, bFreeze);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(pstVideoContext->eState == eDECODER_STATE_STOPPED)
	{
		HxLOG_Error("\tVideo state stopped. Exit.\n");
		err = ERR_FAIL;
		goto exit;
	}

	if(bFreeze == TRUE)
	{
		if(pstVideoContext->bFreezed != TRUE)
		{
#if defined(CONFIG_SUPPORT_FCC)
			diErr = DI_VIDEO_Pause(pstVideoContext->ulDiDecId, TRUE);
#else
			diErr = DI_VIDEO_Pause(ulDecId, TRUE);
#endif
			if (diErr != DI_ERR_OK)
			{
				HxLOG_Error("\tError from DI_VIDEO_Pause()\n");
			}
		}
	}
	else
	{
		if(pstVideoContext->bFreezed != FALSE)
		{
#if defined(CONFIG_SUPPORT_FCC)
			diErr = DI_VIDEO_Pause(pstVideoContext->ulDiDecId, FALSE);
#else
			diErr = DI_VIDEO_Pause(ulDecId, FALSE);
#endif
			if (diErr != DI_ERR_OK)
			{
				HxLOG_Error("\tError from DI_VIDEO_Pause()\n");
			}
		}
	}

	pstVideoContext->bFreezed = bFreeze;

exit:
	palVideo_LEAVE_CRITICAL;

	return err;
}

HBOOL PAL_VIDEO_IsFreezed(HUINT32 ulDecId)
{
	palVideo_Context_t		*pstVideoContext;

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		return FALSE;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	return pstVideoContext->bFreezed;
}

HERROR PAL_VIDEO_StartStillPicture(HUINT32 ulDecId, HUINT8 *pucBuffer, HUINT32 ulBufferBytes, DxVideoCodec_e eVideoCodec, HBOOL bForCapture)
{
	HERROR				err = ERR_OK;
	DI_ERR_CODE			diErr;
	DI_VIDEO_FORMAT		eDiStillFormat;
	palVideo_Context_t		*pstVideoContext;

	palVideo_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d, buf:0x%x, bytes:%d, %s, Capture:%d)\n", __FUNCTION__,
						ulDecId,
						pucBuffer,
						ulBufferBytes,
						OTL_ENUM2STR_VideoCodec(eVideoCodec),
						bForCapture
						);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(pstVideoContext->eState != eDECODER_STATE_STOPPED)
	{
		HxLOG_Error("\tVideo decoder is NOT stopped yet. PLEASE CHECK YOUR REQUEST !!!! *****\n");
		err = ERR_FAIL;
		goto exit;
	}

	if ( (pucBuffer == NULL) || (ulBufferBytes == 0) )
	{
		HxLOG_Error("\tERROR pucBuffer : 0x%x, ulBufferBytes:%d\n", pucBuffer, ulBufferBytes);
		err = ERR_FAIL;
		goto exit;
	}

	switch(eVideoCodec)
	{
		case eDxVIDEO_CODEC_H264:
			eDiStillFormat = DI_VIDEO_FORMAT_STILL_H264;
			break;
		case eDxVIDEO_CODEC_VC1:
			eDiStillFormat = DI_VIDEO_FORMAT_STILL_VC1;
			break;
		case eDxVIDEO_CODEC_MPEG2:
			/* go through */
		default:
			eDiStillFormat = DI_VIDEO_FORMAT_STILL_MPEG2;
			break;
	}

	HxLOG_Debug("\teDiStillFormat(0x%x)\n", eDiStillFormat);

	diErr = DI_VIDEO_PlayMemoryStream(
#if defined(CONFIG_SUPPORT_FCC)
										pstVideoContext->ulDiDecId,
#else
										ulDecId,
#endif
										DI_VIDEO_STREAM_TYPE_ES,
										eDiStillFormat,
										pucBuffer,
										ulBufferBytes,
										(void*)NULL);

	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tError from DI_VIDEO_PlayMemoryStream()\n");
		err = ERR_FAIL;
		goto exit;
	}

#if defined(CONFIG_SUPPORT_FCC)
	pal_video_Show(pstVideoContext->ulDiDecId, TRUE);
#else
	pal_video_Show(ulDecId, TRUE);
#endif
	pstVideoContext->bDiShow = TRUE;

	pstVideoContext->bStillPicture = TRUE;
	pstVideoContext->eState = eDECODER_STATE_STARTING;

	if(bForCapture == TRUE)
	{
		PAL_SCALER_SetVideoUsage(ulDecId, ePAL_SCALER_VIDEOUSAGE_CAPTURE);
	}
	else
	{
		PAL_SCALER_SetVideoUsage(ulDecId, ePAL_SCALER_VIDEOUSAGE_STILL);
	}

exit:
	palVideo_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_VIDEO_StopStillPicture(HUINT32 ulDecId)
{
	HERROR					err = ERR_OK;
	DI_ERR_CODE				diErr;
	palVideo_Context_t			*pstVideoContext;

	palVideo_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulDecId);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(pstVideoContext->eState == eDECODER_STATE_STOPPED)
	{
		HxLOG_Debug("\tVideo decoder already stopped. Ignored *****\n");
		err = ERR_FAIL;
		goto exit;
	}
	else if(pstVideoContext->bStillPicture != TRUE)
	{
		HxLOG_Debug("\tNOT in still picture decoding. Ignored *****\n");
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pal_video_ResetContext(ulDecId);

		PAL_SCALER_SetVideoUsage(ulDecId, ePAL_SCALER_VIDEOUSAGE_NORMAL);

#if defined(CONFIG_SUPPORT_FCC)
		diErr = DI_VIDEO_StopMemoryStream(pstVideoContext->ulDiDecId);
#else
		diErr = DI_VIDEO_StopMemoryStream(ulDecId);
#endif
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("\tError from DI_VIDEO_StopMemoryStream()\n");
			err = ERR_FAIL;
			goto exit;
		}
	}

exit:
	palVideo_LEAVE_CRITICAL;

	return err;
}

// TODO: 이거 초기화 옵션 파생으로 이동
HERROR PAL_VIDEO_SetEvtTimeOut(HUINT32 msec)
{
	DI_ERR_CODE		diErr;
	HUINT32  		i;

	HxLOG_Debug("%s(%d ms)\n", __FUNCTION__, msec);

	for (i = 0; i < s_ulMaxVideoDecoder; i++)
	{
		diErr = DI_VIDEO_SetEventTimeOut(i, msec);
		if(diErr != DI_ERR_OK)
		{
			HxLOG_Error("DI_VIDEO_SetEventTimeOut() failed..!! decId(%d) msec(%d) \n", i, msec);
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

HERROR PAL_VIDEO_CreateVideoFrameCapture(
													HUINT32 ulDecId, 				/* video device id */
													HUINT32 ulWidth, 					/* target video window size */
													HUINT32 ulHeight, 					/* target video window size */
													PalVideo_FrameBuffer_t *pFrame			/* captured video frame */
													)
{
	DI_ERR_CODE				diErr;
	DI_OSD_HANDLE 			hOsd;
	DI_OSD_PIXEL_FORMAT		eDiOSDFormat = DI_OSD_PIXEL_FORMAT_ARGB_8888;
	HUINT32 				*pBuffer;

#if defined(CONFIG_SUPPORT_FCC)
	palVideo_Context_t		*pstVideoContext;
#endif

	HxLOG_Debug("%s(%d, w:%d, h:%d, frame buf:0x%x)\n", __FUNCTION__, ulDecId, ulWidth, ulHeight, pFrame);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(pFrame == NULL)
	{
		HxLOG_Error("\tNULL frame buffer. Exit...\n");
		return ERR_FAIL;
	}

	diErr = DI_OSD_CreateFrameBuffer(
									ulWidth,
									ulHeight,
									eDiOSDFormat,//DI_OSD_PIXEL_FORMAT format,
									&hOsd//DI_OSD_HANDLE * phOsd
									);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_OSD_CreateFrameBuffer()\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_SUPPORT_FCC)
	pstVideoContext = pal_video_GetVideoContext(ulDecId);
#endif

	diErr = DI_VIDEO_StartCapture(
#if defined(CONFIG_SUPPORT_FCC)
									pstVideoContext->ulDiDecId,
#else
									ulDecId,
#endif
									0,//HUINT16 tgtWidth,
									0,//HUINT16 tgtHeight,
									(HUINT8*)&hOsd//HUINT8 * CapturedBitmap
									);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_VIDEO_StartCapture()\n");
		return ERR_FAIL;
	}

	pFrame->ulWidth = ulWidth;
	pFrame->ulHeight = ulHeight;
	if(pFrame->pFrameBuffer == NULL)
	{
		return ERR_FAIL;
	}

	diErr = DI_OSD_GetFrameBuffer(hOsd, (void **)&pBuffer);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_OSD_GetFrameBuffer()\n");
		return ERR_FAIL;
	}

	HxSTD_memcpy(pFrame->pFrameBuffer, pBuffer, sizeof(HUINT32) * ulWidth * ulHeight);

	diErr = DI_OSD_DeleteFrameBuffer(hOsd);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_OSD_DeleteFrameBuffer()\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR PAL_VIDEO_GetThumbNail(
													HUINT32 ulDecId, 				/* video device id */
													HUINT32 ulWidth, 					/* target video window size */
													HUINT32 ulHeight, 					/* target video window size */
													PalVideo_FrameBuffer_t *pFrame			/* captured video frame */
													)
{
	DI_ERR_CODE				diErr;
	DI_OSD_HANDLE 			hOsd;
	DI_OSD_PIXEL_FORMAT		eDiOSDFormat = DI_OSD_PIXEL_FORMAT_ARGB_8888;
	HUINT32 				*pBuffer;

#if defined(CONFIG_SUPPORT_FCC)
	palVideo_Context_t		*pstVideoContext;
#endif

	HxLOG_Debug("%s(%d, w:%d, h:%d, frame buf:0x%x)\n", __FUNCTION__, ulDecId, ulWidth, ulHeight, pFrame);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(pFrame == NULL)
	{
		HxLOG_Error("\tNULL frame buffer. Exit...\n");
		return ERR_FAIL;
	}

	diErr = DI_OSD_CreateFrameBuffer(
									ulWidth,
									ulHeight,
									eDiOSDFormat,//DI_OSD_PIXEL_FORMAT format,
									&hOsd//DI_OSD_HANDLE * phOsd
									);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_OSD_CreateFrameBuffer()\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_SUPPORT_FCC)
	pstVideoContext = pal_video_GetVideoContext(ulDecId);
#endif

	diErr = DI_VIDEO_GetThumbnail(
#if defined(CONFIG_SUPPORT_FCC)
									pstVideoContext->ulDiDecId,
#else
									ulDecId,
#endif
									0,//HUINT16 tgtWidth,
									0,//HUINT16 tgtHeight,
									(HUINT8*)&hOsd//HUINT8 * CapturedBitmap
									);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_VIDEO_GetThumbnail()\n");
		return ERR_FAIL;
	}

	pFrame->ulWidth = ulWidth;
	pFrame->ulHeight = ulHeight;
	if(pFrame->pFrameBuffer == NULL)
	{
		return ERR_FAIL;
	}

	diErr = DI_OSD_GetFrameBuffer(hOsd, (void **)&pBuffer);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_OSD_GetFrameBuffer()\n");
		return ERR_FAIL;
	}

	HxSTD_memcpy(pFrame->pFrameBuffer, pBuffer, sizeof(HUINT32) * ulWidth * ulHeight);

	diErr = DI_OSD_DeleteFrameBuffer(hOsd);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_OSD_DeleteFrameBuffer()\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_VIDEO_CaptureVideoFrame(			HUINT32 ulDecId, 				/* video device id */
												HUINT32 ulWidth, 					/* target video window size */
												HUINT32 ulHeight, 					/* target video window size */
												HUINT32 *handle						/* captured video frame */
												)
{
	DI_ERR_CODE				diErr;
	DI_OSD_PIXEL_FORMAT		eDiOSDFormat = DI_OSD_PIXEL_FORMAT_ARGB_8888;
#if defined(CONFIG_SUPPORT_FCC)
	palVideo_Context_t		*pstVideoContext;
#endif

	HxLOG_Debug("%s(%d, w:%d, h:%d, handle:0x%x)\n", __FUNCTION__, ulDecId, ulWidth, ulHeight, handle);

	if(handle == NULL)
	{
		HxLOG_Error("\tNULL handle. Exit....\n");
		return ERR_FAIL;
	}

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		return ERR_FAIL;
	}

	diErr = DI_OSD_CreateFrameBuffer(
									ulWidth,
									ulHeight,
									eDiOSDFormat,	//DI_OSD_PIXEL_FORMAT format,
									(DI_OSD_HANDLE*)handle		//DI_OSD_HANDLE * phOsd
									);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_OSD_CreateFrameBuffer()\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_SUPPORT_FCC)
	pstVideoContext = pal_video_GetVideoContext(ulDecId);
#endif

	diErr = DI_VIDEO_StartCapture(
#if defined(CONFIG_SUPPORT_FCC)
									pstVideoContext->ulDiDecId,
#else
									ulDecId,
#endif
									0,//HUINT16 tgtWidth,
									0,//HUINT16 tgtHeight,
									(HUINT8*)handle//HUINT8 * CapturedBitmap
									);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR from DI_VIDEO_StartCapture()\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


// Decoder Number를 입력 받아서, State를알려준다.  Context State와 다름.
HBOOL PAL_VIDEO_IsRunning(HUINT32 ulDecId)
{
	palVideo_Context_t 		*pstVideoContext;

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		return FALSE;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(pstVideoContext->eState == eDECODER_STATE_DECODING)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

DecoderState_t PAL_VIDEO_GetState(HUINT32 ulDecId)
{
	return s_stVideoContext[ulDecId].eState;
}

HERROR PAL_VIDEO_GetSourceTiming(HUINT32 ulDecId, VIDEO_Timing_t *pstVideoSourceTiming)
{
	HERROR					err = ERR_OK;
	HUINT32 				ulWidth, ulHeight;
	HBOOL					bProgressive;
	VIDEO_FrameRate_t		eFrameRate;
	DxAspectRatio_e 	eAspectRatio;
	palVideo_Context_t 		*pstVideoContext;
	DxResolutionStatus_e	eVidResol;
	DxPictureRate_e		ePictureRate = eDxPICTURERATE_UNKNOWN;

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		return ERR_FAIL;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(NULL == pstVideoSourceTiming)
	{
		HxLOG_Error("\t\n");
		return ERR_FAIL;
	}

	pstVideoSourceTiming->eResolution = eDxRESOLUTION_STATUS_UNKNOWN;
	pstVideoSourceTiming->ePictureRate = eDxPICTURERATE_UNKNOWN;
	pstVideoSourceTiming->eAspectRatio = eDxASPECTRATIO_UNKNOWN;

	palVideo_ENTER_CRITICAL;

	if(pstVideoContext->bStreamInfoValid != TRUE)
	{
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		ulWidth = pstVideoContext->stStreamInfo.ulHorizontalSize;
		ulHeight = pstVideoContext->stStreamInfo.ulVerticalSize;
		eFrameRate = pstVideoContext->stStreamInfo.eFrameRate;
		bProgressive = pstVideoContext->stStreamInfo.bProgressive;
		eAspectRatio = pstVideoContext->stStreamInfo.eAspectRatio;
	}

	if(ulHeight == 0)
	{
		eVidResol = eDxRESOLUTION_STATUS_UNKNOWN;
	}
	else if(ulHeight <= 480)
	{
		if(bProgressive == TRUE)
		{
			eVidResol = eDxRESOLUTION_STATUS_480P;
			ePictureRate = eDxPICTURERATE_60P;
		}
		else
		{
			eVidResol = eDxRESOLUTION_STATUS_480I;
			ePictureRate = eDxPICTURERATE_60I;
		}
	}
	else if(ulHeight <= 576)
	{
		if(bProgressive == TRUE)
		{
			eVidResol = eDxRESOLUTION_STATUS_576P;
			ePictureRate = eDxPICTURERATE_50P;
		}
		else
		{
			eVidResol = eDxRESOLUTION_STATUS_576I;
			ePictureRate = eDxPICTURERATE_50I;
		}
	}
	else if(ulHeight <= 720)
	{
		eVidResol = eDxRESOLUTION_STATUS_720P;

		if(eFrameRate == eFrameRate_50)
		{
			ePictureRate = eDxPICTURERATE_50P;
		}
		else if(eFrameRate == eFrameRate_60)
		{
			ePictureRate = eDxPICTURERATE_60P;
		}
		else
		{
			HxLOG_Error("%s() : ERROR -- Invalid frame rate (%d) for 720p video. Default to 60P\n", __FUNCTION__, eFrameRate);
			ePictureRate = eDxPICTURERATE_60P;
		}
	}
	else if(ulHeight <= 1080)
	{
		if(bProgressive == TRUE)
		{
			eVidResol = eDxRESOLUTION_STATUS_1080P;

			if(eFrameRate == eFrameRate_24)
			{
				ePictureRate = eDxPICTURERATE_24P;
			}
			else if(eFrameRate == eFrameRate_25)
			{
				ePictureRate = eDxPICTURERATE_25P;
			}
			else if(eFrameRate == eFrameRate_30)
			{
				ePictureRate = eDxPICTURERATE_30P;
			}
			else if(eFrameRate == eFrameRate_50)
			{
				ePictureRate = eDxPICTURERATE_50P;
			}
			else if(eFrameRate == eFrameRate_60)
			{
				ePictureRate = eDxPICTURERATE_60P;
			}
			else
			{
				HxLOG_Error("%s() : ERROR -- Invalid frame rate (%d) for 1080p video. Default to 60P\n", __FUNCTION__, eFrameRate);
				ePictureRate = eDxPICTURERATE_60P;
			}
		}
		else
		{
			eVidResol = eDxRESOLUTION_STATUS_1080I;

			if(eFrameRate == eFrameRate_25)
			{
				ePictureRate = eDxPICTURERATE_50I;
			}
			else if(eFrameRate == eFrameRate_30)
			{
				ePictureRate = eDxPICTURERATE_60I;
			}
			else
			{
				HxLOG_Error("%s() : ERROR -- Invalid frame rate (%d) for 1080i video. Default to 60I\n", __FUNCTION__, eFrameRate);
				ePictureRate = eDxPICTURERATE_60I;
			}
		}
	}
#if defined(CONFIG_PROD_HMX4KBBC)
		else if(ulHeight <= 2160)
		{
			eVidResol = eDxRESOLUTION_STATUS_2160P;
			if(eFrameRate == eFrameRate_24)
			{
				ePictureRate = eDxPICTURERATE_24P;
			}
			else if(eFrameRate == eFrameRate_25)
			{
				ePictureRate = eDxPICTURERATE_25P;
			}
			else if(eFrameRate == eFrameRate_30)
			{
				ePictureRate = eDxPICTURERATE_30P;
			}
			else if(eFrameRate == eFrameRate_50)
			{
				ePictureRate = eDxPICTURERATE_50P;
			}
			else if(eFrameRate == eFrameRate_60)
			{
				ePictureRate = eDxPICTURERATE_60P;
			}
			else
			{
				HxLOG_Error("%s() : ERROR -- Invalid frame rate (%d) for 2160p video. Default to 60P\n", __FUNCTION__, eFrameRate);
				ePictureRate = eDxPICTURERATE_60P;
			}
		}
#endif
	else
	{
		eVidResol = eDxRESOLUTION_STATUS_UNKNOWN;
		ePictureRate = eDxPICTURERATE_UNKNOWN;
	}

	pstVideoSourceTiming->eResolution = eVidResol;
	pstVideoSourceTiming->ePictureRate = ePictureRate;
	pstVideoSourceTiming->eAspectRatio = eAspectRatio;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulDecId);
	HxLOG_Debug("\t%s() : width(%d), height(%d), bPrograssive(%d), eFrameRate(%s), eAspectRatio(%s) => resolution (%s), picture rate(%s)\n"
									, __FUNCTION__
									, ulWidth
									, ulHeight
									, bProgressive
									, OTL_ENUM2STR_VideoFrameRate(eFrameRate)
									, OTL_ENUM2STR_AspectRatio(eAspectRatio)
									, OTL_ENUM2STR_ResolutionStatus(eVidResol)
									, OTL_ENUM2STR_VideoPictureRate(ePictureRate)
									);

exit:
	palVideo_LEAVE_CRITICAL;

	return err;
}


HERROR PAL_VIDEO_GetStreamInfo(HUINT32 ulDecId, VideoStreamInfo_t *pstVideoStreamInfo)
{
	HERROR					err = ERR_OK;
	palVideo_Context_t 		*pstVideoContext;

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(pstVideoStreamInfo == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	palVideo_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulDecId);

	if(pstVideoContext->bStreamInfoValid == TRUE)
	{
		VK_MEM_Memcpy(pstVideoStreamInfo, &(pstVideoContext->stStreamInfo), sizeof(VideoStreamInfo_t));
	}
	else
	{
		err = ERR_FAIL;
	}

	palVideo_LEAVE_CRITICAL;

	return err;
}

HUINT32 PAL_VIDEO_GetSupportNumberOfDecoder(void)
{
	return s_ulMaxVideoDecoder;
}

VIDEO_DecType_t PAL_VIDEO_GetDecoderType(HUINT32 ulDecId)
{
#if defined(CONFIG_SUPPORT_FCC)
	if (ulDecId >= s_ulMaxVideoDecoder)
	{
		// fcc에서는 가상으로 2개의 video decoder를 사용하므로 따로 처리해줌
		return eVIDEO_Dec_Main;
	}
#endif

 	if(s_pstDiVideoCapability[ulDecId].eDecoderType == DI_VIDEO_DECODER_SUB)
 	{
		return eVIDEO_Dec_Sub;
 	}
	else
	{
		return eVIDEO_Dec_Main;
	}
}

HERROR PAL_VIDEO_SetHide(HUINT32 ulDecId, AV_HideClient_t ulClientMask, HBOOL bHide)
{
	HERROR					err = ERR_OK;
	palVideo_Context_t 		*pstVideoContext;
	HBOOL					bDiShow;
	HUINT32					ulDiDecId = ulDecId;

	palVideo_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d, ulClientMask[0x%x], bHide[%d])\n", __FUNCTION__, ulDecId, ulClientMask, bHide);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}


	if(bHide == TRUE)
	{
		pstVideoContext->ulHideRequest |= ulClientMask;
	}
	else
	{
		pstVideoContext->ulHideRequest &= ~ulClientMask;
	}

#if defined(CONFIG_SUPPORT_FCC)
	if (ulDecId != s_ulMainDiDecId)
	{
		// main action id가 아니네.. 무시하자
		err = ERR_OK;
		goto exit;
	}
#endif

	if(pstVideoContext->bStillPicture == TRUE)
	{
		bDiShow = TRUE;
	}
	else if(pstVideoContext->ulHideRequest == 0)
	{
		bDiShow = TRUE;
	}
	else
	{
		bDiShow = FALSE;
	}

#if defined(CONFIG_SUPPORT_FCC)
	ulDiDecId = pstVideoContext->ulDiDecId;
#endif

	if( (pstVideoContext->bDiShow != TRUE) && (bDiShow == TRUE) )
	{
		HxLOG_Debug("\toooooo Showing DI Video[%d]\n", ulDecId);
		pstVideoContext->bDiShow = TRUE;
		pal_video_Show(ulDiDecId, TRUE);
	}
	else if( (pstVideoContext->bDiShow != FALSE) && (bDiShow == FALSE) )
	{
		HxLOG_Debug("\txxxxxx Hiding DI Video[%d]\n", ulDecId);
		pstVideoContext->bDiShow = FALSE;
		pal_video_Show(ulDiDecId, FALSE);
	}
	else
	{
		/* 아무 변경 없음 */
	}

exit:
	palVideo_LEAVE_CRITICAL;

	return err;
}
HERROR PAL_VIDEO_RegisterEventCallbackForVideoError(void (*pfnEventCallback)(HUINT32 ulDecoder, HUINT32 ulDisplay))
{
	s_pfnEventCallbackForVideoError = pfnEventCallback;

	return ERR_OK;
}

// pal_pb에서 thumbnail 처리시 codec값 변경이 필요하여 추가
HERROR PAL_VIDEO_ConvertPal2Di_Codec(DxVideoCodec_e ePalCodec, HUINT32 *peDiCodec)
{
	return pal_video_ConvertPal2Di_Codec(ePalCodec, (DI_VIDEO_FORMAT*)peDiCodec);
}



HERROR PAL_VIDEO_SetOutputEnable(HUINT32 ulDeviceId, HUINT32 ulDisplayId, HBOOL bEnable)
{
	DI_ERR_CODE				diErr;
	HUINT32					ulDiDecId;
	palVideo_Context_t		*pstVideoContext;

	// ulDeviceID 0: main decoder,
	// ulDeviceID 1: sub decoder(PIP, Mosaic)

	// ulDisplayId 0: hd
	// ulDisplayId 1: sd

	pstVideoContext = pal_video_GetVideoContext(ulDeviceId);
	ulDiDecId = pstVideoContext->ulDiDecId;

	HxLOG_Debug("ulDeviceId: %d, ulDisplayId: %d, bEnable: %d\n", ulDeviceId, ulDisplayId, bEnable);

	if(ulDisplayId > 1)
	{
		HxLOG_Error("invalid display ID: %d \n", ulDisplayId);
		return ERR_OK;
	}

	palVideo_ENTER_CRITICAL;

	diErr = DI_VIDEO_ForceDisableOutput(ulDiDecId, ulDisplayId, !bEnable);
	if (diErr != DI_ERR_OK)
	{
		HxLOG_Error("DI_VIDEO_ForceDisableOutput failed. decId(%d) displayId(%d) enable(%d) \n",ulDeviceId, ulDisplayId, bEnable);
	}

	palVideo_LEAVE_CRITICAL;

	return ERR_OK;
}

#if defined(CONFIG_SUPPORT_FCC)
HERROR PAL_VIDEO_StartPrimerDecoding(HUINT32 ulDecId, DxVideoCodec_e eVideoCodec, HBOOL bMainAV, HUINT32 ulPrimerIdx)
{
	HERROR 				err = ERR_OK;
	DI_VIDEO_FORMAT 	eDiVideoFormat;
	palVideo_Context_t		*pstVideoContext;
	DI_VIDEO_PRIMER_t 	stPrimerSetting;
	HUINT32				ulDiDecId;

	palVideo_ENTER_CRITICAL;

	AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n"));
	AvFccTrace(HxANSI_COLOR_YELLOW("[AV_FCC_TRACE] (FCC) ulDecId(%d), %s\n"), ulDecId, OTL_ENUM2STR_VideoCodec(eVideoCodec));
	AvFccTrace(HxANSI_COLOR_YELLOW("-------------------------------------------------------\n\n"));


	HxLOG_Debug("%s(%d, %s)\n", __FUNCTION__, ulDecId, OTL_ENUM2STR_VideoCodec(eVideoCodec));

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		HxLOG_Error("\tInvalid video dec id(%d)  *****\n", ulDecId);
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(pstVideoContext->eState != eDECODER_STATE_STOPPED)
	{
		HxLOG_Error("\tVideo decoder is NOT stopped yet. PLEASE CHECK YOUR REQUEST !!!! *****\n");
		err = ERR_FAIL;
		goto exit;
	}

	pal_video_ConvertPal2Di_Codec(eVideoCodec, &eDiVideoFormat);

	if( (s_pstDiVideoCapability[pstVideoContext->ulDiDecId].eSupportedFormat & eDiVideoFormat) == 0)
	{
		HxLOG_Error("\tVideo codec is not supported. ulDecId : %d\n", ulDecId);
		err = ERR_FAIL;
		goto exit;
	}

	VPC_Print(HxANSI_COLOR_YELLOW("++ (%d) - bPrimerStarted(%d) bMainAV(%d) ulPrimerIdx(%d)\n"), pstVideoContext->ulDiDecId, pstVideoContext->bPrimerStarted, bMainAV, ulPrimerIdx);

	ulDiDecId = pstVideoContext->ulDiDecId;
	HxSTD_MemSet(&stPrimerSetting, 0x00, sizeof(DI_VIDEO_PRIMER_t));
	stPrimerSetting.ulPrimerIndex = ulPrimerIdx;
	stPrimerSetting.eFormat = eDiVideoFormat;

	if (pstVideoContext->bPrimerStarted != TRUE)
	{
		//	HxLOG_FCC("ulDecId(%d) ulPrimerIndex(%d) eFormat(%d)", ulDecId, stPrimerSetting.ulPrimerIndex, stPrimerSetting.eFormat);
		HxLOG_FCC(" >>>>>>> DI_VIDEO_StartPrimer decId(%d) idx(%d) <<<<<<<<<<<<<<\n", ulDiDecId, stPrimerSetting.ulPrimerIndex);

		err = DI_VIDEO_StartPrimer(ulDiDecId, &stPrimerSetting);
		if (err != ERR_OK)
		{
			HxLOG_Error("\tError(0x%X) occurred in DI_VIDEO_StartPrimer().\n", err);
			err = ERR_FAIL;
			goto exit;
		}

		pstVideoContext->bPrimerStarted = TRUE;
	}

	if (bMainAV == TRUE)
	{
		HxLOG_FCC(" >>>>>>> DI_VIDEO_SelectPrimer decId(%d) ulPrimerIdx(%d) <<<<<<<<<<<<<<\n", ulDiDecId, ulPrimerIdx);
		err = DI_VIDEO_SelectPrimer(ulDiDecId, ulPrimerIdx);
		if (err != ERR_OK)
		{
			HxLOG_Error("\tError(0x%X) occurred in DI_VIDEO_SelectPrimer().\n", err);
			err = ERR_FAIL;
			goto exit;
		}

		HxLOG_FCC(" >>>>>>> DI_VIDEO_Start decId(%d) ulPrimerIdx(%d)  <<<<<<<<<<<<<<\n", ulDiDecId, ulPrimerIdx);
		err = DI_VIDEO_Start(ulDiDecId, eDiVideoFormat);
		if (err != ERR_OK)
		{
			HxLOG_Error("\tError(0x%X) occurred in DI_VIDEO_Start().\n", err);
			err = ERR_FAIL;
			goto exit;
		}

		s_ulMainDiDecId = ulDecId;

		pstVideoContext->eState = eDECODER_STATE_STARTING;
	}

	PAL_SCALER_SetVideoUsage(ulDecId, ePAL_SCALER_VIDEOUSAGE_NORMAL);

exit:
	palVideo_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_VIDEO_StopPrimerDecoding(HUINT32 ulDecId, HUINT32 ulPrimerIdx)
{
	HERROR				err = ERR_OK;
	DI_ERR_CODE 		diErr;
	palVideo_Context_t		*pstVideoContext;
	DI_VIDEO_PRIMER_t 	stPrimerSetting;
	HUINT32				ulDiDecId;


	palVideo_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulDecId);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		HxLOG_Error("\tInvalid video dec id(%d)  *****\n", ulDecId);
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(pstVideoContext->eState == eDECODER_STATE_STOPPED)
	{
		HxLOG_Debug("\tVideo decoder already stopped. Ignored *****\n");
		err = ERR_FAIL;
		goto exit;
	}
	else if(pstVideoContext->bStillPicture == TRUE)
	{
		HxLOG_Debug("\tVideo decoder is IN STILL PICTURE mode. Ignored *****\n");
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		VPC_Print(HxANSI_COLOR_YELLOW("-- (%d) - ulPrimerIdx(%d)\n"), pstVideoContext->ulDiDecId, ulPrimerIdx);

		pal_video_ResetContext(ulDecId);

		ulDiDecId = pstVideoContext->ulDiDecId;

		VPC_Print(HxANSI_COLOR_RED("-- (%d) \n"), pstVideoContext->ulDiDecId);

		HxSTD_MemSet(&stPrimerSetting, 0x00, sizeof(DI_VIDEO_PRIMER_t));
		stPrimerSetting.ulPrimerIndex = ulPrimerIdx;
		stPrimerSetting.eFormat = 0; // 사용하지 않는듯..

		HxLOG_FCC(" >>>>>>> DI_VIDEO_SelectPrimer decId(%d) ulPrimerIndex(%d) <<<<<<<<<<<<<<\n", ulDiDecId, stPrimerSetting.ulPrimerIndex);
		diErr = DI_VIDEO_SelectPrimer(ulDiDecId, stPrimerSetting.ulPrimerIndex);
		if (diErr != DI_ERR_OK)
		{
			HxLOG_Error("\tError(0x%X) occurred in DI_VIDEO_SelectPrimer().\n", diErr);
			err = ERR_FAIL;
			goto exit;
		}

		HxLOG_FCC(" >>>>>>> DI_VIDEO_Stop decId(%d) idx(%d) <<<<<<<<<<<<<<\n", ulDiDecId, stPrimerSetting.ulPrimerIndex);
		diErr = DI_VIDEO_Stop(ulDiDecId);
		if (diErr != DI_ERR_OK)
		{
			HxLOG_Error("\tError(0x%X) occurred in DI_VIDEO_Stop().\n", diErr);
			err = ERR_FAIL;
			goto exit;
		}
	}

exit:
	palVideo_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_VIDEO_StopPrimer(HUINT32 ulDecId, HUINT32 ulPrimerIdx)
{
	HERROR				err = ERR_OK;
	DI_ERR_CODE 		diErr;
	palVideo_Context_t		*pstVideoContext;
	DI_VIDEO_PRIMER_t 	stPrimerSetting;
	HUINT32				ulDiDecId;


	palVideo_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulDecId);

	if(pal_video_CheckDecId(ulDecId) != ERR_OK)
	{
		HxLOG_Error("\tInvalid video dec id(%d)  *****\n", ulDecId);
		err = ERR_FAIL;
		goto exit;
	}
	else
	{
		pstVideoContext = pal_video_GetVideoContext(ulDecId);
	}

	if(pstVideoContext->bPrimerStarted == FALSE)
	{
		HxLOG_Debug("\tVideo primer already stopped. Ignored *****\n");
		err = ERR_FAIL;
		goto exit;
	}

	VPC_Print(HxANSI_COLOR_YELLOW("-- (%d) - ulPrimerIdx(%d)\n"), pstVideoContext->ulDiDecId, ulPrimerIdx);

	ulDiDecId = pstVideoContext->ulDiDecId;

	HxSTD_MemSet(&stPrimerSetting, 0x00, sizeof(DI_VIDEO_PRIMER_t));
	stPrimerSetting.ulPrimerIndex = ulPrimerIdx;
	stPrimerSetting.eFormat = 0; // 사용하지 않는듯..


	HxLOG_FCC(" >>>>>>> DI_VIDEO_StopPrimer decId(%d)  idx(%d) <<<<<<<<<<<<<<\n", ulDiDecId, stPrimerSetting.ulPrimerIndex);
	diErr = DI_VIDEO_StopPrimer(ulDiDecId, &stPrimerSetting);
	if (diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tError(0x%X) occurred in DI_VIDEO_StopPrimer().\n", diErr);
		err = ERR_FAIL;
		goto exit;
	}

	pstVideoContext->bPrimerStarted = FALSE;

exit:
	palVideo_LEAVE_CRITICAL;

	return err;
}

HERROR PAL_VIDEO_SetMainDecIdForFcc(HUINT32 ulDecId)
{
	HERROR	hError = ERR_FAIL;

	if (ulDecId < MAX_VIDEO_CONTEXT)
	{
		s_ulMainDiDecId = ulDecId;
		hError = ERR_OK;
	}
	else
	{
		HxLOG_Error("Error! Invalid decId(%d) !!! \n", ulDecId);
	}

	return hError;
}
#endif

HERROR PAL_VIDEO_SetChannelChangeMode(HUINT32 ulDecId, PalVideo_ChannelChangeMode_e eMode)
{
	DI_ERR_CODE			eDiErr = DI_ERR_ERROR;
	DI_VIDEO_CC_MODE	eDiCcm = DI_VIDEO_CC_MUTE;

	switch(eMode)
	{
	case eChannelChangeMode_Mute:					eDiCcm = DI_VIDEO_CC_MUTE;	break;
	case eChannelChangeMode_HoldUntilTsmLock:		eDiCcm = DI_VIDEO_CC_HOLD_UNTIL_TSMLOCK;	break;
	case eChannelChangeMode_MuteUntilFirstPicture:	eDiCcm = DI_VIDEO_CC_MUTE_UNTIL_FIRST_PICTURE;	break;
	case eChannelChangeMode_HoldUntilFirstPicture:	eDiCcm = DI_VIDEO_CC_HOLD_UNTIL_FIRST_PICTURE;	break;
	default:
		HxLOG_Warning("ulDecId(%d) - Unknown Channel Change Mode(%d)\n", ulDecId, eMode);
		return ERR_FAIL;
	}

	if(eChannelChangeMode_MuteUntilFirstPicture == eMode)
	{	/* windows are always visible */
		(void)DI_VIDEO_SetWindowVisible(ulDecId, TRUE);
	}

	eDiErr = DI_VIDEO_SetChannelChangeMode(ulDecId, eDiCcm);
	s_ePalChannelChangeMode = eDiCcm;

	HxLOG_Print("DI_CCM(%d), MW_CCM(%d), ulDecId(%d), eDiErr(%d)\n", eDiCcm, eMode, ulDecId, eDiErr);

	return (DI_ERR_OK != eDiErr) ? ERR_FAIL : ERR_OK;
}

void PAL_VIDEO_SetShowHideControl(HBOOL bShowHideControlEnabled)
{
	s_bShowHideControlEnabled = bShowHideControlEnabled;
}

#define _____DEBUG_API______________________________________________________________________________________________________________________________


/*********************** End of File ******************************/
