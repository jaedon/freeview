/*******************************************************************
	File Description
********************************************************************/
/**
	@file	pal_scaler.c
	@brief	pal_scaler.c

	Description: video, audio 등의 output에 대한 정의 및 처리.		\n
	Module: PAL /SCALER												\n

	Copyright (c) 2009 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/
#if defined(CONFIG_NO_DRIVER)
#undef CONFIG_PACKAGE_DIRECTFB
#endif

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#if defined(CONFIG_PACKAGE_DIRECTFB)
#include <directfb.h>
#endif

#include <di_video.h>
#include <di_hdmi.h>
#include <di_nvram.h>

#include <get_enum_str.h>
#include <hlib.h>

#include <pal_scaler.h>
#include <pal_output.h>
#include <pal_scart.h>
#include <pal_hdmi.h>
#include <pal_video.h>
#include <pal_sys.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/*****************      DEBUG Defines         **********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define MAKE_EVEN_NUM(X)	((X) & 0xFFFFFFFE)

#if defined(CONFIG_PACKAGE_DIRECTFB)
#if defined(WIN32)
#define	DFBCHECK(x)				\
	{								\
		DFBResult err = x;			\
		if (err != DFB_OK)			\
		{							\
			HxLOG_Error("%s <%d>:\n\t", __FILE__, __LINE__);	\
			DirectFBErrorFatal(#x, err);				\
		}							\
	}
IDirectFB			*_dfb = NULL;
IDirectFBScreen 	*_primary_screen = NULL;
IDirectFBDisplayLayer	*secondary_layer = NULL;

#else
#define	DFBCHECK(x...)				\
	{								\
		DFBResult err = x;			\
		if (err != DFB_OK)			\
		{							\
			HxLOG_Error("%s <%d>:\n\t", __FILE__, __LINE__);	\
			DirectFBErrorFatal(#x, err);				\
		}							\
	}
IDirectFB			*_dfb = NULL;
IDirectFBScreen 	*_primary_screen = NULL;
IDirectFBDisplayLayer	*secondary_layer = NULL;
#endif
#endif

#define	palScaler_ENTER_CRITICAL	VK_SEM_Get(s_ulPalSclSemId)
#define	palScaler_LEAVE_CRITICAL	VK_SEM_Release(s_ulPalSclSemId)

#if defined(CONFIG_SUPPORT_FCC)
#define FCC_PAL_MAIN_DECODER_ID				(0)
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HBOOL						bInfoValid;
	PalScaler_VideoUsage_e		eVideoUsage;

	HUINT32 					ulFrameHorizontalSize;		/* Horizontal pixel size of video frame */
	HUINT32 					ulFrameVerticalSize;		/* Vertical pixel size of video frame */

	DxAspectRatio_e 			eFrameAspectRatio;			/* Aspect ratio of video frame */
	VIDEO_Afd_t 				eAfd;						/* Active Format Description */

	HBOOL 						bProgressive;				/* Progressive(1), Interlaced(0) */
	VIDEO_FrameRate_t 			eFrameRate;

	DxVideo3DFormat_e			e3DFormat;
} palScaler_VideoAttribute_t;

/* OSD layer별 video output setting. User video output setting에 우선 함. */
typedef struct
{
	DxAspectRatio_e				eOsdAspectRatio;	/* OSD layer가 원하는 비디오 출력 aspect ratio */
	DxDfcSelect_e				eDfc;				/* OSD layer가 원하는 DFC override */
	PIG_Rect_t					stPigRect;			/* PIG 위치 및 size */
	DxAspectRatio_e				ePigAspectRatio;
} palScaler_OsdSetting_t;

/* User video output setting. OSD layer에 setting이 없을 경우 raw video에 적용됨. */
typedef struct
{
	DxCompositeStandard_e		eCompositeStandard;
	DxAspectRatio_e				eTvAspectRatio;
	DxDfcSelect_e				eDfc;
	DxResolutionSelect_e		eResolutionSelect;
	DxHdmi3DSelect_e			e3DSelect;

	HBOOL						bUseEdidAspectRatio;
	HBOOL						bWideSignalLetterbox;
	HBOOL						bHdmiAfd;
} palScaler_UserSetting_t;

/* HDMI, Composite이 따로 설정될 수 있음 */
typedef struct
{
	/* Final Output resolution */
	VIDEO_Timing_t				stOutputTiming;		/* 최종 출력 video timing */
	VIDEO_Afd_t					eOutputAfd;
	VIDEO_Wss_t					eOutputWss;

	DxVideo3DFormat_e			e3DStatus;
	DxDfcStatus_b				eScreenOsdShrink;	/* No, Letterbox, PillarBox 만 유효 */
} palScaler_OutputStatus_t;

typedef struct
{
	DxRect_t					stRect;
	HUINT32						ulRefW;
	HUINT32						ulRefH;
} palScaler_MediaRect_t;

typedef struct
{
	HUINT32						ulWindowId;

	HBOOL						bSubScreenMedia;	/* Full screen 이 아닌 media rect */
	DxRect_t					stScreenMediaRect;	/* 계산된 media rect */

	DxDfcStatus_b				ulDfcStatus;

	DxRect_t					stVideoInRect;		/* 계산된 in rect */
	DxRect_t					stVideoOutRect; 	/* 계산된 out rect */
	HBOOL						bRoundOff;              /* Default false; Netflix-only for getting uppper rounded_ratio_calculation */
	HBOOL						bForceSourceBase;       /* Default false; Netflix-only for handling videoResize() under DASH with various source-change */

/**************************** DI status ***************************/

	DI_VIDEO_RECT_t				stDiVideoInRect;	/* 반영된 DI in rect */
	DI_VIDEO_RECT_t				stDiVideoOutRect;	/* 반영된 DI in rect */
	DI_VIDEO_RECT_t				stDiHdClearRect;		/* 반영된 DI media clear rect */
} palScaler_Status_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HULONG						s_ulPalSclSemId;
STATIC HBOOL							s_bSupportScalerByVideoFrame;


/* OSD 출력 setting */
#define NUM_MAX_OSD_PLANE			10 /* 넉넉하게 10개. 모자랄리가 없지만 모자라면 늘려라. */
STATIC palScaler_OsdSetting_t		s_stOsdLayerSetting[NUM_MAX_OSD_PLANE];
STATIC palScaler_OsdSetting_t		s_stOsdSetting;
STATIC HBOOL						s_bPigInRect, s_bPigOutRect;

/* User Video Setting */
STATIC palScaler_UserSetting_t		s_stUserSetting;

/* 비디오 출력 파라미터 */
STATIC palScaler_OutputStatus_t		s_stOutputStatus;

/* 디코더별 파라미터 */
STATIC HUINT32						s_ulMainDecId;
STATIC palScaler_VideoAttribute_t	s_stVideoAttribute[NUM_VIEW_ACTION];
STATIC palScaler_Status_t			s_stScalerStatus[NUM_VIEW_ACTION];
STATIC palScaler_MediaRect_t		s_stMediaRect[NUM_VIEW_ACTION];

/* HDMI 해상도 제한 */
STATIC HBOOL						s_bResolutionLimit;

STATIC HUINT8						s_ucBgColor_R, s_ucBgColor_G, s_ucBgColor_B;
/* DI 출력 status */
STATIC DI_VIDEO_HD_RESOLUTION		s_eDiHdResolution = -1;

/*
	stb scenario 상, input window handling을 요청하는 경우는 mheg 등의 3rd party.
	input window 설정 시 osd resolution에 대항하는 상대 좌표로 전달해야 한다.
	그럼 mw에서는 이를 비율료 변환하여, scaler에 전달한다.
*/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define _____convert_APIs________________________________________________________________

STATIC DI_VIDEO_HD_RESOLUTION pal_scaler_ConvertResolutionPal2Di(VIDEO_Timing_t stVideoTiming)
{
	DI_VIDEO_HD_RESOLUTION di_res = 0xfff;

	switch (stVideoTiming.eResolution)
	{
		case eDxRESOLUTION_STATUS_480I:
			di_res = DI_VIDEO_HD_RESOLUTION_480I;
			break;

		case eDxRESOLUTION_STATUS_480P:
			di_res = DI_VIDEO_HD_RESOLUTION_480P;
			break;

		case eDxRESOLUTION_STATUS_576I:
			di_res = DI_VIDEO_HD_RESOLUTION_576I;
			break;

		case eDxRESOLUTION_STATUS_576P:
			di_res = DI_VIDEO_HD_RESOLUTION_576P;
			break;

		case eDxRESOLUTION_STATUS_720P:
			if(stVideoTiming.ePictureRate == eDxPICTURERATE_60P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_720P;
			}
			else if(stVideoTiming.ePictureRate == eDxPICTURERATE_50P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_720P_50HZ;
			}
			else
			{
				HxLOG_Error("\t%s() : Picture rate error (%d) in eDxRESOLUTION_STATUS_720P\n\n", __FUNCTION__, stVideoTiming.ePictureRate);
			}
			break;

		case eDxRESOLUTION_STATUS_1080I:
			if(stVideoTiming.ePictureRate == eDxPICTURERATE_60I)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_1080I;
			}
			else if(stVideoTiming.ePictureRate == eDxPICTURERATE_50I)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_1080I_50HZ;
			}
			else
			{
				HxLOG_Error("\t%s() : Picture rate error (%d) in eDxRESOLUTION_STATUS_1080I\n\n", __FUNCTION__, stVideoTiming.ePictureRate);
			}
			break;

		case eDxRESOLUTION_STATUS_1080P:
			if(stVideoTiming.ePictureRate == eDxPICTURERATE_60P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_1080P;
			}
			else if(stVideoTiming.ePictureRate == eDxPICTURERATE_30P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_1080P_30HZ;
			}
			else if(stVideoTiming.ePictureRate == eDxPICTURERATE_50P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_1080P_50HZ;
			}
			else if(stVideoTiming.ePictureRate == eDxPICTURERATE_25P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_1080P_25HZ;
			}
			else if(stVideoTiming.ePictureRate == eDxPICTURERATE_24P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_1080P_24HZ;
			}
			else
			{
				HxLOG_Error("\t%s() : Picture rate error (%d) in eDxRESOLUTION_STATUS_1080P\n\n", __FUNCTION__, stVideoTiming.ePictureRate);
			}
			break;

#if defined(CONFIG_MW_OUTPUT_2160P)
		case eDxRESOLUTION_STATUS_2160P:
			if(stVideoTiming.ePictureRate == eDxPICTURERATE_60P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_2160P;
			}
			else if(stVideoTiming.ePictureRate == eDxPICTURERATE_30P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_2160P_30HZ;
			}
			else if(stVideoTiming.ePictureRate == eDxPICTURERATE_50P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_2160P_50HZ;
			}
			else if(stVideoTiming.ePictureRate == eDxPICTURERATE_25P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_2160P_25HZ;
			}
			else if(stVideoTiming.ePictureRate == eDxPICTURERATE_24P)
			{
				di_res = DI_VIDEO_HD_RESOLUTION_2160P_24HZ;
			}
			else
			{
				HxLOG_Error("\t%s() : Picture rate error (%d) in eDxRESOLUTION_STATUS_2160P\n\n", __FUNCTION__, stVideoTiming.ePictureRate);
			}
			break;
#endif
		case eDxRESOLUTION_STATUS_UNKNOWN:
		default:
			HxLOG_Error("\t%s() : ERROR in stVideoTiming.eResolution(%d)\n\n", __FUNCTION__, stVideoTiming.eResolution);

#if defined(CONFIG_MW_OUTPUT_ANALOG_STANDARD_NTSC)
			di_res = DI_VIDEO_HD_RESOLUTION_480P;
#else
			di_res = DI_VIDEO_HD_RESOLUTION_576P;
#endif
			break;
	}

	return di_res;
}

STATIC DI_VIDEO_3D_FORMAT pal_scaler_Convert3DFormat_Pal2Di(DxVideo3DFormat_e e3DFormat)
{
	switch(e3DFormat)
	{
		case eDxVIDEO_3DFORMAT_SIDEBYSIDE:
			return DI_VIDEO_3D_FORMAT_SIDEBYSIDE;

		case eDxVIDEO_3DFORMAT_TOPANDBOTTOM:
			return DI_VIDEO_3D_FORMAT_TOPANDBOTTOM;

		case eDxVIDEO_3DFORMAT_FRAMEPACKING:
			return DI_VIDEO_3D_FORMAT_FRAMEPACKING;

		default:
			return DI_VIDEO_3D_FORMAT_NONE;
	}
}

STATIC DxResolutionSelect_e pal_scaler_ConvertDiNvramVideoFormatToPalResolutionSelect(DI_NVRAM_VIDEO_RESOLUTION_E eNvramVideoFormat)
{
	DxResolutionSelect_e eResolutionSelect = eDxRESOLUTION_SELECT_UNKNOWN;
	switch (eNvramVideoFormat)
	{
		case DI_NVRAM_RESOLUTION_720x480I:
		case DI_NVRAM_RESOLUTION_2880x480I:
			eResolutionSelect = eDxRESOLUTION_SELECT_480I;
			break;
		case DI_NVRAM_RESOLUTION_640x480P:
		case DI_NVRAM_RESOLUTION_720x480P:
		case DI_NVRAM_RESOLUTION_1440x480P:
		case DI_NVRAM_RESOLUTION_2880x480P:
			eResolutionSelect = eDxRESOLUTION_SELECT_480P; 
			break;
		case DI_NVRAM_RESOLUTION_720x576I:
		case	DI_NVRAM_RESOLUTION_2880x576I:
			eResolutionSelect = eDxRESOLUTION_SELECT_576I;
			break;
		case DI_NVRAM_RESOLUTION_720x576P:
		case DI_NVRAM_RESOLUTION_1440x576P:
		case DI_NVRAM_RESOLUTION_2880x576P:
			eResolutionSelect = eDxRESOLUTION_SELECT_576P ;
			break;
		case DI_NVRAM_RESOLUTION_1080x720P:
		case DI_NVRAM_RESOLUTION_1280x720P:
			eResolutionSelect = eDxRESOLUTION_SELECT_720P;
			break;
		case DI_NVRAM_RESOLUTION_1920x1080I:
			eResolutionSelect = eDxRESOLUTION_SELECT_1080I;
			break;
		case DI_NVRAM_RESOLUTION_1920x1080P:
			eResolutionSelect = eDxRESOLUTION_SELECT_1080P;
			break;
		case DI_NVRAM_RESOLUTION_3840x2160P:
			eResolutionSelect = eDxRESOLUTION_SELECT_2160P;\
			break;
		default:
			HxLOG_Error("[%s]Hd video format covert is not supported \n",__func__);
			HxLOG_Error("[%s]default setting -> 1920x1080I \n",__func__);
			eResolutionSelect = eDxRESOLUTION_SELECT_1080I;
			break;

	}
	return eResolutionSelect;
}

#define _____local_APIs________________________________________________________________

STATIC void pal_scaler_PrintDfcStatus(DxDfcStatus_b ulDfcStatus)
{
	HxLOG_Debug("\t%s(0x%x)\n", __FUNCTION__, ulDfcStatus);

 	if(ulDfcStatus & eDxDFC_STATUS_LETTERBOX)
 	{
		HxLOG_Debug("\t\t=== eDxDFC_STATUS_LETTERBOX\n");
 	}

 	if(ulDfcStatus & eDxDFC_STATUS_LETTERBOX149)
 	{
		HxLOG_Debug("\t\t=== eDxDFC_STATUS_LETTERBOX149\n");
 	}

 	if(ulDfcStatus & eDxDFC_STATUS_PILLARBOX)
 	{
		HxLOG_Debug("\t\t=== eDxDFC_STATUS_PILLARBOX\n");
 	}

 	if(ulDfcStatus & eDxDFC_STATUS_PILLARBOX149)
 	{
		HxLOG_Debug("\t\t=== eDxDFC_STATUS_PILLARBOX149\n");
 	}

 	if(ulDfcStatus & eDxDFC_STATUS_ZOOM)
 	{
		HxLOG_Debug("\t\t=== eDxDFC_STATUS_ZOOM\n");
 	}

 	if(ulDfcStatus & eDxDFC_STATUS_ZOOM149)
 	{
		HxLOG_Debug("\t\t=== eDxDFC_STATUS_ZOOM149\n");
 	}

 	if(ulDfcStatus & eDxDFC_STATUS_SIDECUT)
 	{
		HxLOG_Debug("\t\t=== eDxDFC_STATUS_SIDECUT\n");
 	}

 	if(ulDfcStatus & eDxDFC_STATUS_SIDECUT149)
 	{
		HxLOG_Debug("\t\t=== eDxDFC_STATUS_SIDECUT149\n");
 	}
}

STATIC void pal_scaler_PrintVideoAttribute(HUINT32 ulDecId)
{
	palScaler_VideoAttribute_t	*pstVideoAttribute = &s_stVideoAttribute[ulDecId];

	HLIB_CMD_Printf("========== Video Attribute [%d] ==========\n", ulDecId);
	HLIB_CMD_Printf("\tInformation : %d\n", pstVideoAttribute->bInfoValid);
	HLIB_CMD_Printf("\tFrame Horizontal Size : %d\n", pstVideoAttribute->ulFrameHorizontalSize);
	HLIB_CMD_Printf("\tFrame Vertical Size : %d\n", pstVideoAttribute->ulFrameVerticalSize);
	HLIB_CMD_Printf("\tFrame Aspect Ratio : %s\n", OTL_ENUM2STR_AspectRatio(pstVideoAttribute->eFrameAspectRatio));
	HLIB_CMD_Printf("\tAFD : %s\n", OTL_ENUM2STR_Afd(pstVideoAttribute->eAfd));
	HLIB_CMD_Printf("\tProgressive Flag : %d\n", pstVideoAttribute->bProgressive);
	HLIB_CMD_Printf("\tFrame Rate : %s\n", OTL_ENUM2STR_VideoFrameRate(pstVideoAttribute->eFrameRate));
	HLIB_CMD_Printf("\t3D Format : %s\n", OTL_ENUM2STR_Video3DFormat(pstVideoAttribute->e3DFormat));
	HLIB_CMD_Printf("\tVideo Usage : %d\n", pstVideoAttribute->eVideoUsage);
	HLIB_CMD_Printf("================================\n");
}

STATIC HERROR pal_scaler_GetBestSourceTiming(HUINT32 ulDecId, VIDEO_Timing_t *pstTiming)
{
	palScaler_VideoAttribute_t	*pstVideoAttribute = &s_stVideoAttribute[ulDecId];

	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, ulDecId);

	if(pstVideoAttribute->bInfoValid != TRUE)
	{
		HxLOG_Debug("\t\tNo video[%d] attribute information. exit.\n", ulDecId);
		return ERR_FAIL;
	}

	if(pstVideoAttribute->ulFrameVerticalSize <= 576)
	{
		if(pstVideoAttribute->bProgressive == TRUE)
		{
			if(pstVideoAttribute->eFrameRate == eFrameRate_50)
			{
				pstTiming->eResolution = eDxRESOLUTION_STATUS_576P;
				pstTiming->ePictureRate = eDxPICTURERATE_50P;
			}
			else if(pstVideoAttribute->eFrameRate == eFrameRate_60)
			{
				pstTiming->eResolution = eDxRESOLUTION_STATUS_480P;
				pstTiming->ePictureRate = eDxPICTURERATE_60P;
			}
			else
			{
				if(s_stUserSetting.eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
				{
					pstTiming->eResolution = eDxRESOLUTION_STATUS_480P;
					pstTiming->ePictureRate = eDxPICTURERATE_60P;
				}
				else
				{
					pstTiming->eResolution = eDxRESOLUTION_STATUS_576P;
					pstTiming->ePictureRate = eDxPICTURERATE_50P;
				}
			}
		}
		else // progressive == FALSE
		{
			if(pstVideoAttribute->eFrameRate == eFrameRate_50)
			{
				pstTiming->eResolution = eDxRESOLUTION_STATUS_576I;
				pstTiming->ePictureRate = eDxPICTURERATE_50I;
			}
			else if(pstVideoAttribute->eFrameRate == eFrameRate_60)
			{
				pstTiming->eResolution = eDxRESOLUTION_STATUS_480I;
				pstTiming->ePictureRate = eDxPICTURERATE_60I;
			}
			else
			{
				if(s_stUserSetting.eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
				{
					pstTiming->eResolution = eDxRESOLUTION_STATUS_480I;
					pstTiming->ePictureRate = eDxPICTURERATE_60I;
				}
				else
				{
					pstTiming->eResolution = eDxRESOLUTION_STATUS_576I;
					pstTiming->ePictureRate = eDxPICTURERATE_50I;
				}
			}
		}
	}
	else if(pstVideoAttribute->ulFrameVerticalSize <= 720)
	{
		pstTiming->eResolution = eDxRESOLUTION_STATUS_720P;

		if(pstVideoAttribute->eFrameRate == eFrameRate_50)
		{
			pstTiming->ePictureRate = eDxPICTURERATE_50P;
		}
		else if(pstVideoAttribute->eFrameRate == eFrameRate_60)
		{
			pstTiming->ePictureRate = eDxPICTURERATE_60P;
		}
		else
		{
			if(s_stUserSetting.eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
			{
				pstTiming->ePictureRate = eDxPICTURERATE_60P;
			}
			else
			{
				pstTiming->ePictureRate = eDxPICTURERATE_50P;
			}
		}
	}
	else if (pstVideoAttribute->ulFrameVerticalSize > 1080)
	{
		pstTiming->eResolution = eDxRESOLUTION_STATUS_2160P;
		pstTiming->ePictureRate = eDxPICTURERATE_50P;
		if(pstVideoAttribute->eFrameRate == eFrameRate_24)
		{
			pstTiming->ePictureRate = eDxPICTURERATE_24P;
		}
		else if(pstVideoAttribute->eFrameRate == eFrameRate_25)
		{
			pstTiming->ePictureRate = eDxPICTURERATE_25P;
		}
		else if(pstVideoAttribute->eFrameRate == eFrameRate_30)
		{
			pstTiming->ePictureRate = eDxPICTURERATE_30P;
		}
		else if(pstVideoAttribute->eFrameRate == eFrameRate_50)
		{
			pstTiming->ePictureRate = eDxPICTURERATE_50P;
		}
		else if(pstVideoAttribute->eFrameRate == eFrameRate_60)
		{
			pstTiming->ePictureRate = eDxPICTURERATE_60P;
		}

	}
	else
	{
		if(pstVideoAttribute->bProgressive == TRUE)
		{
			pstTiming->eResolution = eDxRESOLUTION_STATUS_1080P;

			if(pstVideoAttribute->eFrameRate == eFrameRate_24)
			{
				pstTiming->ePictureRate = eDxPICTURERATE_24P;
			}
			else if(pstVideoAttribute->eFrameRate == eFrameRate_25)
			{
				pstTiming->ePictureRate = eDxPICTURERATE_25P;
			}
			else if(pstVideoAttribute->eFrameRate == eFrameRate_30)
			{
				pstTiming->ePictureRate = eDxPICTURERATE_30P;
			}
			else if(pstVideoAttribute->eFrameRate == eFrameRate_50)
			{
				pstTiming->ePictureRate = eDxPICTURERATE_50P;
			}
			else if(pstVideoAttribute->eFrameRate == eFrameRate_60)
			{
				pstTiming->ePictureRate = eDxPICTURERATE_60P;
			}
			else
			{
				if(s_stUserSetting.eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
				{
					pstTiming->ePictureRate = eDxPICTURERATE_60P;
				}
				else
				{
					pstTiming->ePictureRate = eDxPICTURERATE_50P;
				}
			}
		}
		else
		{
			pstTiming->eResolution = eDxRESOLUTION_STATUS_1080I;

			if(pstVideoAttribute->eFrameRate == eFrameRate_25)
			{
				pstTiming->ePictureRate = eDxPICTURERATE_50I;
			}
			else if(pstVideoAttribute->eFrameRate == eFrameRate_30)
			{
				pstTiming->ePictureRate = eDxPICTURERATE_60I;
			}
			else
			{
				if(s_stUserSetting.eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
				{
					pstTiming->ePictureRate = eDxPICTURERATE_60I;
				}
				else
				{
					pstTiming->ePictureRate = eDxPICTURERATE_50I;
				}
			}
		}
	}

	if( (eDxASPECTRATIO_UNKNOWN < pstVideoAttribute->eFrameAspectRatio)
		&& (pstVideoAttribute->eFrameAspectRatio < eDxASPECTRATIO_MAX_NUM)
	)
	{
		pstTiming->eAspectRatio = pstVideoAttribute->eFrameAspectRatio;
	}
	else
	{
		pstTiming->eAspectRatio = s_stUserSetting.eTvAspectRatio;
	}

	HxLOG_Debug("\t\tResolution : %s\n", OTL_ENUM2STR_ResolutionStatus(pstTiming->eResolution));
	HxLOG_Debug("\t\tPicture Rate : %s\n", OTL_ENUM2STR_VideoPictureRate(pstTiming->ePictureRate));
	HxLOG_Debug("\t\tAspect Ratio : %s\n", OTL_ENUM2STR_AspectRatio(pstTiming->eAspectRatio));

	return ERR_OK;
}

STATIC HERROR pal_scaler_GetOutputResSize(DxResolutionStatus_e eResol, HUINT32 *width, HUINT32 *height)
{
	HUINT32 outputWidth = 0, outputHeight = 0;

	//HxLOG_Debug("\t%s(%s)\n", __FUNCTION__, OTL_ENUM2STR_ResolutionStatus(eResol));

	switch(eResol)
	{
		case eDxRESOLUTION_STATUS_1080I:
		case eDxRESOLUTION_STATUS_1080P:
			outputWidth = 1920;
			outputHeight = 1080;
			break;

		case eDxRESOLUTION_STATUS_720P:
			outputWidth = 1280;
			outputHeight = 720;
			break;

		case eDxRESOLUTION_STATUS_576I:
		case eDxRESOLUTION_STATUS_576P:
			outputWidth = 720;
			outputHeight = 576;
			break;

		case eDxRESOLUTION_STATUS_480I:
		case eDxRESOLUTION_STATUS_480P:
			outputWidth = 720;
			outputHeight = 480;
			break;

		case eDxRESOLUTION_STATUS_2160P:
			outputWidth = 3840;
			outputHeight = 2160;
			break;

		default:
			HxLOG_Error("\n!!! ERROR in %s => eResol(%d)\n\n", __FUNCTION__, eResol);
			outputWidth = 0;
			outputHeight = 0;
			break;
	}

	if(width != NULL)
	{
		*width = outputWidth;
	}

	if(height != NULL)
	{
		*height = outputHeight;
	}

	//HxLOG_Debug("\t\twidth:%d, height:%d\n", outputWidth, outputHeight);

	return ERR_OK;
}

STATIC HERROR pal_scaler_GetScaledRect(
								DxRect_t	stRatioRect,
								HINT32		nRatioRefW,
								HINT32		nRatioRefH,
								DxRect_t	stTargetRect,
								DxRect_t	*pstScaledRect
								)
{
	HxLOG_Debug("\t%s() - Ratio(%d, %d, %d, %d)/(%d, %d) - Target(%d, %d, %d, %d)\n"
				, __FUNCTION__
				, stRatioRect.nX
				, stRatioRect.nY
				, stRatioRect.nW
				, stRatioRect.nH
				, nRatioRefW
				, nRatioRefH
				, stTargetRect.nX
				, stTargetRect.nY
				, stTargetRect.nW
				, stTargetRect.nH
				);

	if( (nRatioRefW == 0) || (nRatioRefH == 0) )
	{
		HxLOG_Error("\t\t================ INVALID VALUE ==============\n\n");

		return ERR_FAIL;
	}

	pstScaledRect->nX = stRatioRect.nX * stTargetRect.nW / nRatioRefW + stTargetRect.nX;
	pstScaledRect->nY = stRatioRect.nY * stTargetRect.nH / nRatioRefH + stTargetRect.nY;
	pstScaledRect->nW = stTargetRect.nW * stRatioRect.nW / nRatioRefW;
	pstScaledRect->nH = stTargetRect.nH * stRatioRect.nH / nRatioRefH;

	HxLOG_Debug("\t\tScaled(%d, %d, %d, %d)\n"
				, pstScaledRect->nX
				, pstScaledRect->nY
				, pstScaledRect->nW
				, pstScaledRect->nH
				);

	return ERR_OK;
}

STATIC HERROR pal_scaler_GetShrinkedRect(
								DxDfcStatus_b			ulDfcStatus,
								DxRect_t				stFullRectOrg,
								DxRect_t				*pstFullRect,
								DxRect_t				*pstSubRect
								)
{
	HUINT32		ulShrinkW = 0, ulShrinkH = 0;

	HxLOG_Debug("\t%s(%d, %d, %d, %d)\n", __FUNCTION__
									, stFullRectOrg.nX
									, stFullRectOrg.nY
									, stFullRectOrg.nW
									, stFullRectOrg.nH
									);

	if( (stFullRectOrg.nW == 0) || (stFullRectOrg.nH == 0) )
	{
		HxLOG_Error("\t\tINVALID Parameter !!! Exit...\n");
		return ERR_FAIL;
	}

	if(pstFullRect != NULL)
	{
		if(ulDfcStatus & eDxDFC_STATUS_LETTERBOX)
		{
			ulShrinkH += pstFullRect->nH / 8;
		}
		else if(ulDfcStatus & eDxDFC_STATUS_LETTERBOX149)
		{
			ulShrinkH += pstFullRect->nH / 16;
		}

		if(ulDfcStatus & eDxDFC_STATUS_PILLARBOX)
		{
			ulShrinkW += pstFullRect->nW / 8;
		}
		else if(ulDfcStatus & eDxDFC_STATUS_PILLARBOX149)
		{
			ulShrinkW += pstFullRect->nW / 16;
		}

		pstFullRect->nX = stFullRectOrg.nX + ulShrinkW;
		pstFullRect->nY = stFullRectOrg.nY + ulShrinkH;
		pstFullRect->nW = stFullRectOrg.nW - ulShrinkW*2;
		pstFullRect->nH = stFullRectOrg.nH - ulShrinkH*2;
	}

	if(pstSubRect != NULL && pstFullRect != NULL)
	{
		/* 1. Orginal full rect offset 을 빼고 */
		pstSubRect->nX -= stFullRectOrg.nX;
		pstSubRect->nY -= stFullRectOrg.nY;

		/* 2. Sub rect 에 대해 full rect scale을 적용하고 */
		pstSubRect->nX = pstSubRect->nX * pstFullRect->nW / stFullRectOrg.nW;
		pstSubRect->nY = pstSubRect->nY * pstFullRect->nH / stFullRectOrg.nH;
		pstSubRect->nW = pstFullRect->nW / stFullRectOrg.nW;
		pstSubRect->nH = pstFullRect->nH / stFullRectOrg.nH;

		/* 3. 새로운 full rect offset 을 더한다. 이게 sub rect의 새로운 자리다. */
		pstSubRect->nX += pstFullRect->nX;
		pstSubRect->nY += pstFullRect->nY;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_FindEdidMaxResolution(DxResolutionStatus_e *peEdidMaxRes)
{
	HERROR					err;
	VIDEO_Timing_t			stVideoTiming;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	stVideoTiming.eAspectRatio = eDxASPECTRATIO_UNKNOWN;
	stVideoTiming.ePictureRate = eDxPICTURERATE_UNKNOWN;

#if defined(CONFIG_MW_OUTPUT_2160P)
	stVideoTiming.eResolution = eDxRESOLUTION_STATUS_2160P;
	err = PAL_HDMI_CheckVideoTimingAvailable(stVideoTiming);
	if(err == ERR_OK)
	{
		goto EXIT;
	}
#endif

#if defined(CONFIG_MW_OUTPUT_1080P)
/* 1920x1080p 검사 */
	stVideoTiming.eResolution = eDxRESOLUTION_STATUS_1080P;
	err = PAL_HDMI_CheckVideoTimingAvailable(stVideoTiming);
	if(err == ERR_OK)
	{
		goto EXIT;
	}
#endif

/* 1920x1080i 검사 */
	stVideoTiming.eResolution = eDxRESOLUTION_STATUS_1080I;
	err = PAL_HDMI_CheckVideoTimingAvailable(stVideoTiming);
	if(err == ERR_OK)
	{
		goto EXIT;
	}

/* 1280x720p 검사 */
	stVideoTiming.eResolution = eDxRESOLUTION_STATUS_720P;
	err = PAL_HDMI_CheckVideoTimingAvailable(stVideoTiming);
	if(err == ERR_OK)
	{
		goto EXIT;
	}

/* 720x576/480p 검사 */
	if(s_stUserSetting.eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
	{
		stVideoTiming.eResolution = eDxRESOLUTION_STATUS_480P;
	}
	else
	{
		stVideoTiming.eResolution = eDxRESOLUTION_STATUS_576P;
	}

	err = PAL_HDMI_CheckVideoTimingAvailable(stVideoTiming);
	if(err == ERR_OK)
	{
		goto EXIT;
	}

/* 720x576/480i 검사 */
	if(s_stUserSetting.eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
	{
		stVideoTiming.eResolution = eDxRESOLUTION_STATUS_480I;
	}
	else
	{
		stVideoTiming.eResolution = eDxRESOLUTION_STATUS_576I;
	}

	err = PAL_HDMI_CheckVideoTimingAvailable(stVideoTiming);
	if(err == ERR_OK)
	{
		goto EXIT;
	}

EXIT:

	*peEdidMaxRes = stVideoTiming.eResolution;

	if(err == ERR_OK)
	{
		HxLOG_Debug("\t\tEDID max resolution is [%s]\n", OTL_ENUM2STR_ResolutionStatus(stVideoTiming.eResolution));
	}
	else
	{
		HxLOG_Debug("\t\tERROR !!! EDID max resolution is set to [%s]\n", OTL_ENUM2STR_ResolutionStatus(stVideoTiming.eResolution));
	}

	return err;
}

STATIC HERROR pal_scaler_SelectResolution(DxResolutionSelect_e		eVideoResSelect,
									DxResolutionStatus_e			eSourceRes,
									DxResolutionStatus_e			*peOutputRes)
{
	HxLOG_Debug("\t%s(%s, SRC:%s)\n", __FUNCTION__,
							OTL_ENUM2STR_ResolutionSelect(eVideoResSelect),
							OTL_ENUM2STR_ResolutionStatus(eSourceRes));

	switch(eVideoResSelect)
	{
		/* 해상도 fix에서는 셋업된 값에 맞춰 결정 */
 		case eDxRESOLUTION_SELECT_480I:
			*peOutputRes = eDxRESOLUTION_STATUS_480I;
			break;

 		case eDxRESOLUTION_SELECT_576I:
			*peOutputRes = eDxRESOLUTION_STATUS_576I;
			break;

 		case eDxRESOLUTION_SELECT_480P:
			*peOutputRes = eDxRESOLUTION_STATUS_480P;
			break;

 		case eDxRESOLUTION_SELECT_576P:
			*peOutputRes = eDxRESOLUTION_STATUS_576P;
			break;

 		case eDxRESOLUTION_SELECT_720P:
			*peOutputRes = eDxRESOLUTION_STATUS_720P;
			break;

 		case eDxRESOLUTION_SELECT_1080I:
			*peOutputRes = eDxRESOLUTION_STATUS_1080I;
			break;

 		case eDxRESOLUTION_SELECT_1080P:
			*peOutputRes = eDxRESOLUTION_STATUS_1080P;
			break;

		case eDxRESOLUTION_SELECT_2160P:
			*peOutputRes = eDxRESOLUTION_STATUS_2160P;
			break;

 		case eDxRESOLUTION_SELECT_EDID_MAX:
			pal_scaler_FindEdidMaxResolution(peOutputRes);
			break;

		case eDxRESOLUTION_SELECT_ORIGINAL:
			*peOutputRes = eSourceRes;
			break;

		default:
			HxLOG_Error("\n!!! ERROR in %s => eVideoResSelect(%d)\n\n", __FUNCTION__, eVideoResSelect);
			*peOutputRes = s_stOutputStatus.stOutputTiming.eResolution;
			break;
	}

	HxLOG_Debug("\t\tSelected output resolution:%s\n", OTL_ENUM2STR_ResolutionStatus(*peOutputRes));

	return ERR_OK;
}

STATIC HERROR pal_scaler_FindCompositeTiming(void)
{
	if(s_stUserSetting.eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
	{
		s_stOutputStatus.stOutputTiming.eResolution = eDxRESOLUTION_STATUS_480I;
		s_stOutputStatus.stOutputTiming.ePictureRate = eDxPICTURERATE_60I;
	}
	else
	{
		s_stOutputStatus.stOutputTiming.eResolution = eDxRESOLUTION_STATUS_576I;
		s_stOutputStatus.stOutputTiming.ePictureRate = eDxPICTURERATE_50I;
	}

	s_stOutputStatus.stOutputTiming.eAspectRatio = s_stUserSetting.eTvAspectRatio;

	HxLOG_Debug("\t%s() : Composite output timing is Fixed to (%s, %s, %s)\n", __FUNCTION__,
							OTL_ENUM2STR_ResolutionStatus(s_stOutputStatus.stOutputTiming.eResolution),
							OTL_ENUM2STR_AspectRatio(s_stOutputStatus.stOutputTiming.eAspectRatio),
							OTL_ENUM2STR_VideoPictureRate(s_stOutputStatus.stOutputTiming.ePictureRate));

	return ERR_OK;
}

STATIC HERROR pal_scaler_FindHdmiTiming(void)
{
	HERROR					err;
	PalHdmi_Status_t		stHdmiStatus;
	VIDEO_Timing_t			stSourceTiming;
	DxResolutionStatus_e	eOutResolution = eDxRESOLUTION_STATUS_UNKNOWN;
	DxAspectRatio_e			eOutAspectRatio = eDxASPECTRATIO_UNKNOWN;
	DxPictureRate_e			eOutPictureRate = eDxPICTURERATE_UNKNOWN;
	VIDEO_Timing_t			stCheckTiming;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

/* 1. 소스 출력 비디오 timing 설정 */
	err = pal_scaler_GetBestSourceTiming(s_ulMainDecId, &stSourceTiming);
	if(err == ERR_OK)
	{
		HxLOG_Debug("\t\tCheck video source [%s, %s, %s]\n",
								OTL_ENUM2STR_ResolutionStatus(stSourceTiming.eResolution),
								OTL_ENUM2STR_AspectRatio(stSourceTiming.eAspectRatio),
								OTL_ENUM2STR_VideoPictureRate(stSourceTiming.ePictureRate));
	}
	else
	{ /* 비디오 안돌고 있음. 현재 output timing으로 검사. */
		stSourceTiming = s_stOutputStatus.stOutputTiming;

		HxLOG_Debug("\t\tNo video info. Check current output timing [%s, %s, %s]\n",
								OTL_ENUM2STR_ResolutionStatus(stSourceTiming.eResolution),
								OTL_ENUM2STR_AspectRatio(stSourceTiming.eAspectRatio),
								OTL_ENUM2STR_VideoPictureRate(stSourceTiming.ePictureRate));
	}

/* 2. HDMI 검사를 위한 조건 검사 */
	err = PAL_HDMI_GetStatus(&stHdmiStatus);
	if(err != ERR_OK)
	{
		HxLOG_Debug("\t\tHDMI is NOT plugged in by PAL_HDMI_GetStatus()\n");
		return ERR_FAIL;
	}
	else if(stHdmiStatus.bPlug != TRUE)
	{
		HxLOG_Debug("\t\tHDMI is NOT plugged in by bPlug\n");
		return ERR_FAIL;
	}
	else if(s_bResolutionLimit == TRUE)
	{ /* HDCP block등으로 인하여 해상도를 SD로 고정 */
		HxLOG_Debug("\t\t=========== Resolution Limited. Fixed to SD resolution. =========\n");
		pal_scaler_FindCompositeTiming();

		return ERR_OK;
	}
	else
	{
		/* 계속 진행 */
	}

/* 3. 해상도 결정 */
	pal_scaler_SelectResolution(s_stUserSetting.eResolutionSelect, stSourceTiming.eResolution, &eOutResolution);

	stCheckTiming.eResolution = eOutResolution;
	stCheckTiming.eAspectRatio = eDxASPECTRATIO_UNKNOWN;
	stCheckTiming.ePictureRate = eDxPICTURERATE_UNKNOWN;

	err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
	if(err != ERR_OK)
	{ /* 안 맞으면 EDID max 로... */
		pal_scaler_FindEdidMaxResolution(&eOutResolution);
		stCheckTiming.eResolution = eOutResolution;
	}

/* 4. Frame rate 결정 */
	stCheckTiming.ePictureRate = stSourceTiming.ePictureRate;

	err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
	if(err == ERR_OK)
	{ /* 소스 picture rate 지원함. */
		eOutPictureRate = stCheckTiming.ePictureRate;
		HxLOG_Debug("\t\t\tHDMI support source [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
	}
	else
	{ /* 소스 picture rate 지원하지 않는다. 대안을 찾아야... */
		if(s_stUserSetting.eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
		{
			stCheckTiming.ePictureRate = eDxPICTURERATE_60P;
			err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
			if(err == ERR_OK)
			{
				eOutPictureRate = stCheckTiming.ePictureRate;
				HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
			}
			else
			{
				stCheckTiming.ePictureRate = eDxPICTURERATE_60I;
				err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
				if(err == ERR_OK)
				{
					eOutPictureRate = stCheckTiming.ePictureRate;
					HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
				}
				else
				{
					stCheckTiming.ePictureRate = eDxPICTURERATE_50P;
					err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
					if(err == ERR_OK)
					{
						eOutPictureRate = stCheckTiming.ePictureRate;
						HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
					}
					else
					{
						stCheckTiming.ePictureRate = eDxPICTURERATE_50I;
						err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
						if(err == ERR_OK)
						{
							eOutPictureRate = stCheckTiming.ePictureRate;
							HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
						}
						else
						{
							stCheckTiming.ePictureRate = eDxPICTURERATE_30P;
							err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
							if(err == ERR_OK)
							{
								eOutPictureRate = stCheckTiming.ePictureRate;
								HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
							}
							else
							{
								stCheckTiming.ePictureRate = eDxPICTURERATE_25P;
								err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
								if(err == ERR_OK)
								{
									eOutPictureRate = stCheckTiming.ePictureRate;
									HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
								}
								else
								{
									stCheckTiming.ePictureRate = eDxPICTURERATE_24P;
									err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
									if(err == ERR_OK)
									{
										eOutPictureRate = stCheckTiming.ePictureRate;
										HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
									}
									else
									{
										HxLOG_Error("\t\t\tCAN'T decide video output timing !!!!\n");
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			stCheckTiming.ePictureRate = eDxPICTURERATE_50P;
			err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
			if(err == ERR_OK)
			{
				eOutPictureRate = stCheckTiming.ePictureRate;
				HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
			}
			else
			{
				stCheckTiming.ePictureRate = eDxPICTURERATE_50I;
				err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
				if(err == ERR_OK)
				{
					eOutPictureRate = stCheckTiming.ePictureRate;
					HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
				}
				else
				{
					stCheckTiming.ePictureRate = eDxPICTURERATE_60P;
					err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
					if(err == ERR_OK)
					{
						eOutPictureRate = stCheckTiming.ePictureRate;
						HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
					}
					else
					{
						stCheckTiming.ePictureRate = eDxPICTURERATE_60I;
						err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
						if(err == ERR_OK)
						{
							eOutPictureRate = stCheckTiming.ePictureRate;
							HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
						}
						else
						{
							stCheckTiming.ePictureRate = eDxPICTURERATE_25P;
							err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
							if(err == ERR_OK)
							{
								eOutPictureRate = stCheckTiming.ePictureRate;
								HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
							}
							else
							{
								stCheckTiming.ePictureRate = eDxPICTURERATE_30P;
								err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
								if(err == ERR_OK)
								{
									eOutPictureRate = stCheckTiming.ePictureRate;
									HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
								}
								else
								{
									stCheckTiming.ePictureRate = eDxPICTURERATE_24P;
									err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
									if(err == ERR_OK)
									{
										eOutPictureRate = stCheckTiming.ePictureRate;
										HxLOG_Debug("\t\t\tHDMI support [%s]\n", OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));
									}
									else
									{
										HxLOG_Error("\t\t\tCAN'T decide video output timing !!!!\n");
									}
								}
							}
						}
					}
				}
			}
		}
	}

/* 5. Aspect ratio 결정 */
	if(s_stUserSetting.bUseEdidAspectRatio == TRUE)
	{
		HxLOG_Debug("\t\tCheck EDID aspect ratio for source [%s]\n", OTL_ENUM2STR_AspectRatio(stSourceTiming.eAspectRatio));


		if(s_stOsdSetting.eOsdAspectRatio != eDxASPECTRATIO_UNKNOWN)
			stCheckTiming.eAspectRatio = s_stOsdSetting.eOsdAspectRatio;
		else
			stCheckTiming.eAspectRatio = stSourceTiming.eAspectRatio;

		err = PAL_HDMI_CheckVideoTimingAvailable(stCheckTiming);
		if(err == ERR_OK)
		{
			eOutAspectRatio = stCheckTiming.eAspectRatio;
		}
		else
		{
			if(stCheckTiming.eAspectRatio == eDxASPECTRATIO_16X9)
			{
				eOutAspectRatio = eDxASPECTRATIO_4X3;
			}
			else
			{
				eOutAspectRatio = eDxASPECTRATIO_16X9;
			}

			HxLOG_Debug("\t\tForce Aspect Ratio to [%s]\n", OTL_ENUM2STR_AspectRatio(eOutAspectRatio));
		}
	}
	else
	{
		eOutAspectRatio = s_stUserSetting.eTvAspectRatio;
		HxLOG_Debug("\t\tApply TV aspect ratio (%s)\n", OTL_ENUM2STR_AspectRatio(eOutAspectRatio));
	}

#if defined(CONFIG_MW_OUTPUT_STATIC_VIDEO_FREQUENCY)
	switch(eOutResolution)
	{
	// Interlace:
	case eDxRESOLUTION_STATUS_480I:
	case eDxRESOLUTION_STATUS_576I:
	case eDxRESOLUTION_STATUS_1080I:
		if (CONFIG_MW_OUTPUT_VIDEO_FREQUENCY == 50)
		{
			eOutPictureRate = eDxPICTURERATE_50I;
		}
		else if (CONFIG_MW_OUTPUT_VIDEO_FREQUENCY == 60)
		{
			eOutPictureRate = eDxPICTURERATE_60I;
		}
		else	// Default:
		{
			eOutPictureRate = eDxPICTURERATE_50I;
		}

		break;

	// Progressive:
	case eDxRESOLUTION_STATUS_480P:
	case eDxRESOLUTION_STATUS_576P:
	case eDxRESOLUTION_STATUS_720P:
	case eDxRESOLUTION_STATUS_1080P:
	case eDxRESOLUTION_STATUS_2160P:
	default:
		if (CONFIG_MW_OUTPUT_VIDEO_FREQUENCY == 50)
		{
			eOutPictureRate = eDxPICTURERATE_50P;
		}
		else if (CONFIG_MW_OUTPUT_VIDEO_FREQUENCY == 60)
		{
			eOutPictureRate = eDxPICTURERATE_60P;
		}
		else if (CONFIG_MW_OUTPUT_VIDEO_FREQUENCY == 25)
		{
			eOutPictureRate = eDxPICTURERATE_25P;
		}
		else if (CONFIG_MW_OUTPUT_VIDEO_FREQUENCY == 24)
		{
			eOutPictureRate = eDxPICTURERATE_24P;
		}
		else if (CONFIG_MW_OUTPUT_VIDEO_FREQUENCY == 30)
		{
			eOutPictureRate = eDxPICTURERATE_30P;
		}
		else	// Default:
		{
			eOutPictureRate = eDxPICTURERATE_50P;
		}

		break;
	}
#endif

	HxLOG_Debug("\t\tDecided Video Output Timing is [%s, %s, %s]\n",
							OTL_ENUM2STR_ResolutionStatus(eOutResolution),
							OTL_ENUM2STR_AspectRatio(eOutAspectRatio),
							OTL_ENUM2STR_VideoPictureRate(eOutPictureRate));

	s_stOutputStatus.stOutputTiming.eResolution = eOutResolution;
	s_stOutputStatus.stOutputTiming.eAspectRatio = eOutAspectRatio;
	s_stOutputStatus.stOutputTiming.ePictureRate = eOutPictureRate;

	return ERR_OK;
}

STATIC HERROR pal_scaler_ProcessAfd_43(DxAspectRatio_e	eFrameAspectRatio,
								DxAspectRatio_e			eTvAspectRatio,
								DxDfcSelect_e 			eDfc,
								VIDEO_Wss_t 			*peOutputWss,
								VIDEO_Afd_t 			*peOutputAfd,
								DxDfcStatus_b			*pePixelProcess
								)
{
	VIDEO_Wss_t 				eOutputWss;
	VIDEO_Afd_t 				eOutputAfd;

	HxLOG_Debug("\t%s(Src:%s, TV:%s, %s)\n", __FUNCTION__
										, OTL_ENUM2STR_AspectRatio(eFrameAspectRatio)
										, OTL_ENUM2STR_AspectRatio(eTvAspectRatio)
										, OTL_ENUM2STR_DfcSelect(eDfc));

	if(eFrameAspectRatio == eDxASPECTRATIO_4X3)
	{ /* 4:3 소스 */
		if(eTvAspectRatio == eDxASPECTRATIO_4X3)
		{ /* 4:3 TV - OK */
			eOutputWss = eWss_43_Full;
			eOutputAfd = eAfd_43;
			*pePixelProcess = eDxDFC_STATUS_FULL;
		}
		else
		{ /* 16:9 TV - NOT OK */
			switch(eDfc)
			{
				case eDxDFC_SELECT_WIDESIGNAL:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_43;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;

				case eDxDFC_SELECT_DEFAULT:
				case eDxDFC_SELECT_SQUEEZE:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_43;
					*pePixelProcess = eDxDFC_STATUS_PILLARBOX;
					break;

				case eDxDFC_SELECT_SQUEEZE149:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_PILLARBOX149;
					break;

				case eDxDFC_SELECT_CENTERCUTOUT:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_169;
					*pePixelProcess = eDxDFC_STATUS_ZOOM;
					break;

				case eDxDFC_SELECT_CENTERCUTOUT149:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_ZOOM149|eDxDFC_STATUS_PILLARBOX149;
					break;

				default:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_Full;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;
			}
		}
	}
	else if(eFrameAspectRatio == eDxASPECTRATIO_16X9)
	{
		if(eTvAspectRatio == eDxASPECTRATIO_16X9)
		{ /* 16:9 OK */
			eOutputWss = eWss_169_Full;
			eOutputAfd = eAfd_43;
			*pePixelProcess = eDxDFC_STATUS_FULL;
		}
		else
		{ /* 4:3 AFD OK */
			eOutputWss = eWss_43_Full;
			eOutputAfd = eAfd_43;
			*pePixelProcess = eDxDFC_STATUS_SIDECUT;
		}
	}
	else
	{
		return ERR_FAIL;
	}

	if(peOutputWss != NULL)
	{
		*peOutputWss = eOutputWss;
	}

	if(peOutputAfd != NULL)
	{
		*peOutputAfd = eOutputAfd;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_ProcessAfd_169(DxAspectRatio_e	eFrameAspectRatio,
								DxAspectRatio_e			eTvAspectRatio,
								DxDfcSelect_e 			eDfc,
								VIDEO_Wss_t 			*peOutputWss,
								VIDEO_Afd_t 			*peOutputAfd,
								DxDfcStatus_b			*pePixelProcess
								)
{
	VIDEO_Wss_t 				eOutputWss;
	VIDEO_Afd_t 				eOutputAfd;

	HxLOG_Debug("\t%s(Src:%s, TV:%s, %s)\n", __FUNCTION__
										, OTL_ENUM2STR_AspectRatio(eFrameAspectRatio)
										, OTL_ENUM2STR_AspectRatio(eTvAspectRatio)
										, OTL_ENUM2STR_DfcSelect(eDfc));

	if(eFrameAspectRatio == eDxASPECTRATIO_4X3)
	{ /* 4:3 소스 */
		if(eTvAspectRatio == eDxASPECTRATIO_4X3)
		{ /* 4:3 OK */
			eOutputWss = eWss_43_LB169_Center;
			eOutputAfd = eAfd_169;
			*pePixelProcess = eDxDFC_STATUS_FULL;
		}
		else
		{ /* 16:9 AFD OK */
			eOutputWss = eWss_169_Full;
			eOutputAfd = eAfd_169;
			*pePixelProcess = eDxDFC_STATUS_ZOOM;
		}
	}
	else if(eFrameAspectRatio == eDxASPECTRATIO_16X9)
	{ /* 16:9 소스 */
		if(eTvAspectRatio == eDxASPECTRATIO_16X9)
		{ /* 16:9 TV - OK */
			eOutputWss = eWss_169_Full;
			eOutputAfd = eAfd_169;
			*pePixelProcess = eDxDFC_STATUS_FULL;
		}
		else
		{ /* 4:3 TV - NOT OK */
			switch(eDfc)
			{
				case eDxDFC_SELECT_WIDESIGNAL:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_169;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;

				case eDxDFC_SELECT_DEFAULT:
				case eDxDFC_SELECT_SQUEEZE:
					eOutputWss = eWss_43_LB169_Center;
					eOutputAfd = eAfd_169;
					*pePixelProcess = eDxDFC_STATUS_LETTERBOX;
					break;

				case eDxDFC_SELECT_SQUEEZE149:
					eOutputWss = eWss_43_LB149_Center;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_LETTERBOX149;
					break;

				case eDxDFC_SELECT_CENTERCUTOUT:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_43;
					*pePixelProcess = eDxDFC_STATUS_SIDECUT;
					break;

				case eDxDFC_SELECT_CENTERCUTOUT149:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_43;
					*pePixelProcess = eDxDFC_STATUS_SIDECUT149;
					break;

				default:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_Full;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;
			}		}
	}
	else
	{
		return ERR_FAIL;
	}

	if(peOutputWss != NULL)
	{
		*peOutputWss = eOutputWss;
	}

	if(peOutputAfd != NULL)
	{
		*peOutputAfd = eOutputAfd;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_ProcessAfd_149(DxAspectRatio_e	eFrameAspectRatio,
								DxAspectRatio_e			eTvAspectRatio,
								DxDfcSelect_e 			eDfc,
								VIDEO_Wss_t 			*peOutputWss,
								VIDEO_Afd_t 			*peOutputAfd,
								DxDfcStatus_b			*pePixelProcess
								)
{
	VIDEO_Wss_t 				eOutputWss;
	VIDEO_Afd_t 				eOutputAfd;

	HxLOG_Debug("\t%s(Src:%s, TV:%s, %s)\n", __FUNCTION__
										, OTL_ENUM2STR_AspectRatio(eFrameAspectRatio)
										, OTL_ENUM2STR_AspectRatio(eTvAspectRatio)
										, OTL_ENUM2STR_DfcSelect(eDfc));

	if(eFrameAspectRatio == eDxASPECTRATIO_4X3)
	{
		if(eTvAspectRatio == eDxASPECTRATIO_4X3)
		{ /* 4:3 OK */
			eOutputWss = eWss_43_LB149_Center;
			eOutputAfd = eAfd_149;
			*pePixelProcess = eDxDFC_STATUS_FULL;
		}
		else
		{ /* 16:9 NOT OK */
			switch(eDfc)
			{
				case eDxDFC_SELECT_WIDESIGNAL:
					eOutputWss = eWss_43_LB149_Center;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;

				case eDxDFC_SELECT_DEFAULT:
				/* AFD 추출하여 계산하면 다음 2경우는 같은 처리를 해야 함. */
				case eDxDFC_SELECT_SQUEEZE:
				case eDxDFC_SELECT_SQUEEZE149:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_ZOOM149|eDxDFC_STATUS_PILLARBOX149;
					break;

				/* AFD 추출하여 계산하면 다음 2경우는 같은 처리를 해야 함. */
				case eDxDFC_SELECT_CENTERCUTOUT:
				case eDxDFC_SELECT_CENTERCUTOUT149:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_169;
					*pePixelProcess = eDxDFC_STATUS_ZOOM;
					break;

				default:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_Full;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;
			}
		}
	}
	else if(eFrameAspectRatio == eDxASPECTRATIO_16X9)
	{
		if(eTvAspectRatio == eDxASPECTRATIO_16X9)
		{ /* 16:9 OK */
			eOutputWss = eWss_169_Full;
			eOutputAfd = eAfd_149;
			*pePixelProcess = eDxDFC_STATUS_FULL;
		}
		else
		{ /* 4:3 NOT OK */
			switch(eDfc)
			{
				case eDxDFC_SELECT_WIDESIGNAL:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;

				case eDxDFC_SELECT_DEFAULT:
				/* AFD 추출하여 계산하면 다음 2경우는 같은 처리를 해야 함. */
				case eDxDFC_SELECT_SQUEEZE:
				case eDxDFC_SELECT_SQUEEZE149:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_SIDECUT149|eDxDFC_STATUS_LETTERBOX149;
					break;

				/* AFD 추출하여 계산하면 다음 2경우는 같은 처리를 해야 함. */
				case eDxDFC_SELECT_CENTERCUTOUT:
				case eDxDFC_SELECT_CENTERCUTOUT149:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_43;
					*pePixelProcess = eDxDFC_STATUS_SIDECUT;
					break;

				default:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_Full;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;
			}
		}
	}
	else
	{
		return ERR_FAIL;
	}

	if(peOutputWss != NULL)
	{
		*peOutputWss = eOutputWss;
	}

	if(peOutputAfd != NULL)
	{
		*peOutputAfd = eOutputAfd;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_ProcessAfd_43_SnP149(DxAspectRatio_e		eFrameAspectRatio,
										DxAspectRatio_e			eTvAspectRatio,
										DxDfcSelect_e 				eDfc,
										VIDEO_Wss_t 				*peOutputWss,
										VIDEO_Afd_t 				*peOutputAfd,
										DxDfcStatus_b				*pePixelProcess
										)
{
	VIDEO_Wss_t 				eOutputWss;
	VIDEO_Afd_t 				eOutputAfd;

	HxLOG_Debug("\t%s(Src%s, TV:%s, %s)\n", __FUNCTION__
										, OTL_ENUM2STR_AspectRatio(eFrameAspectRatio)
										, OTL_ENUM2STR_AspectRatio(eTvAspectRatio)
										, OTL_ENUM2STR_DfcSelect(eDfc));

	if(eFrameAspectRatio == eDxASPECTRATIO_4X3)
	{
		if(eTvAspectRatio == eDxASPECTRATIO_4X3)
		{ /* 4:3 OK */
			eOutputWss = eWss_43_Full;
			eOutputAfd = eAfd_43_SnP149;
			*pePixelProcess = eDxDFC_STATUS_FULL;
		}
		else
		{ /* 16:9 NOT OK */
			switch(eDfc)
			{
				case eDxDFC_SELECT_WIDESIGNAL:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_43_SnP149;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;

				case eDxDFC_SELECT_DEFAULT:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_ZOOM149|eDxDFC_STATUS_PILLARBOX149;
					break;

				case eDxDFC_SELECT_SQUEEZE:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_43;
					*pePixelProcess = eDxDFC_STATUS_PILLARBOX;
					break;

				case eDxDFC_SELECT_SQUEEZE149:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_PILLARBOX149;
					break;

				case eDxDFC_SELECT_CENTERCUTOUT:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_169;
					*pePixelProcess = eDxDFC_STATUS_ZOOM;
					break;

				case eDxDFC_SELECT_CENTERCUTOUT149:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_169;
					*pePixelProcess = eDxDFC_STATUS_ZOOM149;
					break;

				default:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_Full;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;
			}
		}
	}
	else if(eFrameAspectRatio == eDxASPECTRATIO_16X9)
	{
		if(eTvAspectRatio == eDxASPECTRATIO_16X9)
		{ /* 16:9 OK but SnP 고려 */
				eOutputWss = eWss_169_Full;
				eOutputAfd = eAfd_43_SnP149;
				*pePixelProcess = eDxDFC_STATUS_FULL;
		}
		else
		{ /* 4:3 AFD OK */
			eOutputWss = eWss_43_SnP149;
			eOutputAfd = eAfd_43_SnP149;
			*pePixelProcess = eDxDFC_STATUS_SIDECUT;
		}
	}
	else
	{
		return ERR_FAIL;
	}

	if(peOutputWss != NULL)
	{
		*peOutputWss = eOutputWss;
	}

	if(peOutputAfd != NULL)
	{
		*peOutputAfd = eOutputAfd;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_ProcessAfd_169_SnP149(DxAspectRatio_e		eFrameAspectRatio,
										DxAspectRatio_e			eTvAspectRatio,
										DxDfcSelect_e 				eDfc,
										VIDEO_Wss_t 				*peOutputWss,
										VIDEO_Afd_t 				*peOutputAfd,
										DxDfcStatus_b				*pePixelProcess
										)
{
	VIDEO_Wss_t 				eOutputWss;
	VIDEO_Afd_t 				eOutputAfd;

	HxLOG_Debug("\t%s(Src:%s, TV:%s, %s)\n", __FUNCTION__
										, OTL_ENUM2STR_AspectRatio(eFrameAspectRatio)
										, OTL_ENUM2STR_AspectRatio(eTvAspectRatio)
										, OTL_ENUM2STR_DfcSelect(eDfc));

	if(eFrameAspectRatio == eDxASPECTRATIO_4X3)
	{
		if(eTvAspectRatio == eDxASPECTRATIO_4X3)
		{
			switch(eDfc)
			{
				case eDxDFC_SELECT_DEFAULT:
					/* 4:3 OK but SnP 고려 */
					eOutputWss = eWss_43_LB149_Center;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_ZOOM149|eDxDFC_STATUS_SIDECUT149;
					break;

				default:
					/* 4:3 OK */
					eOutputWss = eWss_43_LB169_Center;
					eOutputAfd = eAfd_169_SnP149;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;
			}
		}
		else
		{ /* 16:9 AFD OK */
			eOutputWss = eWss_169_Full;
			eOutputAfd = eAfd_169_SnP149;
			*pePixelProcess = eDxDFC_STATUS_ZOOM;
		}
	}
	else if(eFrameAspectRatio == eDxASPECTRATIO_16X9)
	{
		if(eTvAspectRatio == eDxASPECTRATIO_16X9)
		{ /* 16:9 OK */
			eOutputWss = eWss_169_Full;
			eOutputAfd = eAfd_169_SnP149;
			*pePixelProcess = eDxDFC_STATUS_FULL;
		}
		else
		{ /* 4:3 NOT OK */
			switch(eDfc)
			{
				case eDxDFC_SELECT_WIDESIGNAL:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_169_SnP149;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;

				case eDxDFC_SELECT_DEFAULT:
					eOutputWss = eWss_43_LB149_Center;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_SIDECUT149|eDxDFC_STATUS_LETTERBOX149;
					break;

				case eDxDFC_SELECT_SQUEEZE:
					eOutputWss = eWss_43_LB169_Center;
					eOutputAfd = eAfd_169;
					*pePixelProcess = eDxDFC_STATUS_LETTERBOX;
					break;

				case eDxDFC_SELECT_SQUEEZE149:
					eOutputWss = eWss_43_LB149_Center;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_LETTERBOX149;
					break;

				case eDxDFC_SELECT_CENTERCUTOUT:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_43;
					*pePixelProcess = eDxDFC_STATUS_SIDECUT;
					break;

				case eDxDFC_SELECT_CENTERCUTOUT149:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_43;
					*pePixelProcess = eDxDFC_STATUS_SIDECUT149;
					break;

				default:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_Full;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;
			}
		}
	}
	else
	{
		return ERR_FAIL;
	}

	if(peOutputWss != NULL)
	{
		*peOutputWss = eOutputWss;
	}

	if(peOutputAfd != NULL)
	{
		*peOutputAfd = eOutputAfd;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_ProcessAfd_169_SnP43(DxAspectRatio_e	eFrameAspectRatio,
									DxAspectRatio_e			eTvAspectRatio,
									DxDfcSelect_e 				eDfc,
									VIDEO_Wss_t 				*peOutputWss,
									VIDEO_Afd_t 				*peOutputAfd,
									DxDfcStatus_b				*pePixelProcess
									)
{
	VIDEO_Wss_t 				eOutputWss;
	VIDEO_Afd_t 				eOutputAfd;

	HxLOG_Debug("\t%s(Src:%s, TV:%s, %s)\n", __FUNCTION__
										, OTL_ENUM2STR_AspectRatio(eFrameAspectRatio)
										, OTL_ENUM2STR_AspectRatio(eTvAspectRatio)
										, OTL_ENUM2STR_DfcSelect(eDfc));

	if(eFrameAspectRatio == eDxASPECTRATIO_4X3)
	{
		if(eTvAspectRatio == eDxASPECTRATIO_4X3)
			{
			switch(eDfc)
			{
				case eDxDFC_SELECT_DEFAULT:
					/* 4:3 OK but SnP 고려 */
				eOutputWss = eWss_43_Full;
				eOutputAfd = eAfd_43;
				*pePixelProcess = eDxDFC_STATUS_ZOOM|eDxDFC_STATUS_SIDECUT;
					break;

				default:
					/* 4:3 OK 고려 */
					eOutputWss = eWss_43_LB169_Center;
					eOutputAfd = eAfd_169_SnP43;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;
			}
		}
		else
		{ /* 16:9 AFD OK */
				eOutputWss = eWss_169_Full;
				eOutputAfd = eAfd_169_SnP43;
				*pePixelProcess = eDxDFC_STATUS_ZOOM;
			}
	}
	else if(eFrameAspectRatio == eDxASPECTRATIO_16X9)
	{
		if(eTvAspectRatio == eDxASPECTRATIO_16X9)
		{ /* 16:9 OK */
			eOutputWss = eWss_169_Full;
			eOutputAfd = eAfd_169_SnP43;
			*pePixelProcess = eDxDFC_STATUS_FULL;
		}
		else
		{ /* 4:3 NOT OK */
			switch(eDfc)
			{
				case eDxDFC_SELECT_WIDESIGNAL:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_169_SnP43;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;

				case eDxDFC_SELECT_SQUEEZE:
					eOutputWss = eWss_43_LB169_Center;
					eOutputAfd = eAfd_169_SnP43;
					*pePixelProcess = eDxDFC_STATUS_LETTERBOX;
					break;

				case eDxDFC_SELECT_SQUEEZE149:
					eOutputWss = eWss_43_LB149_Center;
					eOutputAfd = eAfd_149;
					*pePixelProcess = eDxDFC_STATUS_LETTERBOX149;
					break;

				case eDxDFC_SELECT_DEFAULT:
				case eDxDFC_SELECT_CENTERCUTOUT:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_43;
					*pePixelProcess = eDxDFC_STATUS_SIDECUT;
					break;

				case eDxDFC_SELECT_CENTERCUTOUT149:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_43;
					*pePixelProcess = eDxDFC_STATUS_SIDECUT149;
					break;

				default:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_Full;
					*pePixelProcess = eDxDFC_STATUS_FULL;
					break;
			}
		}
	}
	else
	{
		return ERR_FAIL;
	}

	if(peOutputWss != NULL)
	{
		*peOutputWss = eOutputWss;
	}

	if(peOutputAfd != NULL)
	{
		*peOutputAfd = eOutputAfd;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_ProcessAfd_Full(DxAspectRatio_e	eFrameAspectRatio,
								DxAspectRatio_e			eTvAspectRatio,
								DxDfcSelect_e 				eDfc,
								VIDEO_Wss_t 				*peOutputWss,
								VIDEO_Afd_t 				*peOutputAfd,
								DxDfcStatus_b				*pePixelProcess
								)
{
	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	if(eFrameAspectRatio == eDxASPECTRATIO_4X3)
	{
		pal_scaler_ProcessAfd_43(eFrameAspectRatio,
							eTvAspectRatio,
							eDfc,
							peOutputWss,
							peOutputAfd,
							pePixelProcess);

		if(eTvAspectRatio == eDxASPECTRATIO_4X3)
		{ /* 4:3 OK의 경우는 output AFD만 eAfd_Full로 변경 */
			if(peOutputAfd != NULL)
			{
				*peOutputAfd = eAfd_Full;
			}
		}
	}
	else if(eFrameAspectRatio == eDxASPECTRATIO_16X9)
	{
		pal_scaler_ProcessAfd_169(eFrameAspectRatio,
							eTvAspectRatio,
							eDfc,
							peOutputWss,
							peOutputAfd,
							pePixelProcess);

		if(eTvAspectRatio == eDxASPECTRATIO_16X9)
		{ /* 16:9 OK의 경우는 output AFD만 eAfd_Full로 변경 */
			if(peOutputAfd != NULL)
			{
				*peOutputAfd = eAfd_Full;
			}
		}
	}
	else
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_ApplyPixelCutOut(DxRect_t *pstVideoInRect, DxRect_t *pstVideoOutRect, HBOOL bRoundOff) /* Added the rounding-off flag for netflix VideoResize requirements */
{
	float		fInRatio, fOutRatio;
	HUINT32 	ulOffset, ulLength;
	float 		fOffset, fLength; /* For calculating rounging-off */

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	if(pstVideoInRect->nW == 0)
	{
		HxLOG_Error("\n\n!!!!! CRITICAL in %s : pstVideoInRect->nW is 0 !!!!!\n\n");
		return ERR_FAIL;
	}

	if(pstVideoInRect->nH == 0)
	{
		HxLOG_Error("\n\n!!!!! CRITICAL in %s : pstVideoInRect->nH is 0 !!!!!\n\n");
		return ERR_FAIL;
	}

	fInRatio = (float)pstVideoInRect->nH/(float)pstVideoInRect->nW;
	fOutRatio = (float)pstVideoOutRect->nH/(float)pstVideoOutRect->nW;

	HxLOG_Debug("\t\tIn(%d, %d, %d, %d), Out(%d, %d, %d, %d)\n",
		pstVideoInRect->nX, pstVideoInRect->nY, pstVideoInRect->nW, pstVideoInRect->nH,
		pstVideoOutRect->nX, pstVideoOutRect->nY, pstVideoOutRect->nW, pstVideoOutRect->nH);
	HxLOG_Debug("\t\tfInRatio : %f, fOutRatio : %f\n", fInRatio, fOutRatio);

	if(fInRatio < fOutRatio)
	{ /* In window의 수평 size를 out ratio 만큼 되도록 줄여야 한다.
		Hi/newWi = fOutRatio
		newWi = Hi/fOutRatio */
		if (bRoundOff == TRUE) /* Default false; only for Netflix requirements */
		{
			fLength = ((float)pstVideoInRect->nH / fOutRatio) + 0.5f; /* Getting upper size by rounding off by +0.5 */
			fOffset = (((float)pstVideoInRect->nW - fLength) / 2.0f) + 0.5f; /* Getting upper size by rounding off by +0.5 */
			ulLength = (HUINT32)fLength;
			ulOffset = (HUINT32)fOffset;
		}
		else /* Freesat/Freeview requirements */
		{
			ulLength = (float)pstVideoInRect->nH / fOutRatio;
			ulOffset = (pstVideoInRect->nW - ulLength) / 2;
		}

		pstVideoInRect->nX += ulOffset;
		pstVideoInRect->nW = ulLength;
	}
	else if(fInRatio > fOutRatio)
	{ /* In window의 수직 size를 out ratio 만큼 되도록 줄여야 한다.
		newHi/Wi = fOutRatio
		newHi = fInRatio*Wi */
		if (bRoundOff == TRUE) /* Default false; only for Netflix requirements */
		{
			fLength = ((float)pstVideoOutRect->nW * fInRatio) + 0.5f;
			fOffset = (((float)pstVideoOutRect->nH - fLength) / 2.0f) + 0.5f;
			ulLength = (HUINT32)fLength;
			ulOffset = (HUINT32)fOffset;
		}
		else /* Freesat/Freeview requirements */
		{
			ulLength = (float)pstVideoOutRect->nW * fInRatio;
			ulOffset = (pstVideoOutRect->nH - ulLength) / 2;
		}

		pstVideoOutRect->nY += ulOffset;
		pstVideoOutRect->nH = ulLength;
	}
	else
	{ /* 그냥 나감 */
		return ERR_OK;
	}

	HxLOG_Debug("\t\tAdjusted OUT rect (%d, %d, %d, %d)\n",
					pstVideoOutRect->nX,
					pstVideoOutRect->nY,
					pstVideoOutRect->nW,
					pstVideoOutRect->nH);

	return ERR_OK;
}


STATIC HERROR pal_scaler_ApplyPixelSqueeze(DxRect_t *pstVideoInRect, DxRect_t *pstVideoOutRect, HBOOL bRoundOff) /* Added the rounding-off flag for netflix VideoResize requirements */
{
	float		fInRatio, fOutRatio;
	HUINT32 	ulOffset, ulLength;
	float 		fOffset, fLength; /* For calculating rounging-off */

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	if(pstVideoInRect->nW == 0)
	{
		HxLOG_Error("\n\n!!!!! CRITICAL in %s : pstVideoInRect->nW is 0 !!!!!\n\n");
		return ERR_FAIL;
	}

	if(pstVideoInRect->nH == 0)
	{
		HxLOG_Error("\n\n!!!!! CRITICAL in %s : pstVideoInRect->nH is 0 !!!!!\n\n");
		return ERR_FAIL;
	}

	fInRatio = (float)pstVideoInRect->nH/(float)pstVideoInRect->nW;
	fOutRatio = (float)pstVideoOutRect->nH/(float)pstVideoOutRect->nW;

	HxLOG_Debug("\t\tIn(%d, %d, %d, %d), Out(%d, %d, %d, %d)\n",
		pstVideoInRect->nX, pstVideoInRect->nY, pstVideoInRect->nW, pstVideoInRect->nH,
		pstVideoOutRect->nX, pstVideoOutRect->nY, pstVideoOutRect->nW, pstVideoOutRect->nH);
	HxLOG_Debug("\t\tfInRatio : %f, fOutRatio : %f\n", fInRatio, fOutRatio);

	if(fInRatio < fOutRatio)
	{ /* Out window의 수직 size를 in ratio 만큼 되도록 줄여야 한다.
		newHo/Wo = fInRatio
		newHo = fInRatio*Wo */
		if (bRoundOff == TRUE) /* Default false; only for Netflix requirements */
		{
			fLength = ((float)pstVideoOutRect->nW * fInRatio) + 0.5f; /* rounding-off by +0.5 for getting upper size */
			fOffset = (((float)pstVideoOutRect->nH - fLength) / 2.0f) + 0.5f; /* rounding-off by +0.5 for getting upper size */
			ulLength = (HUINT32)fLength;
			ulOffset = (HUINT32)fOffset;
		}
		else
		{
			ulLength = (float)pstVideoOutRect->nW * fInRatio;
			ulOffset = (pstVideoOutRect->nH - ulLength) / 2;
		}

		pstVideoOutRect->nY += ulOffset;
		pstVideoOutRect->nH = ulLength;
	}
	else if(fInRatio > fOutRatio)
	{ /* Out window의 수평 size를 in ratio 만큼 되도록 줄여야 한다.
		Ho/newWo = fInRatio
		newHo = Ho/fInRatio */
		if (bRoundOff == TRUE) /* Default false; only for Netflix requirements */
		{
			fLength = ((float)pstVideoOutRect->nH / fInRatio) + 0.5f;
			fOffset = (((float)pstVideoOutRect->nW - fLength) / 2.0f) + 0.5f;
			ulLength = (HUINT32)fLength;
			ulOffset = (HUINT32)fOffset;
		}
		else
		{
			ulLength = (float)pstVideoOutRect->nH / fInRatio;
			ulOffset = (pstVideoOutRect->nW - ulLength) / 2;
		}

		pstVideoOutRect->nX += ulOffset;
		pstVideoOutRect->nW = ulLength;
	}
	else
	{ /* 그냥 나감 */
		return ERR_OK;
	}

	HxLOG_Debug("\t\tAdjusted OUT rect (%d, %d, %d, %d)\n",
					pstVideoOutRect->nX,
					pstVideoOutRect->nY,
					pstVideoOutRect->nW,
					pstVideoOutRect->nH);

	return ERR_OK;
}

STATIC HERROR pal_scaler_ProcessDfc(DxDfcStatus_b ulDfcStatus, DxRect_t *pstVideoInRect, DxRect_t *pstVideoOutRect)
{
	HxLOG_Debug("\t%s(0x%x)\n", __FUNCTION__, ulDfcStatus);

	pal_scaler_PrintDfcStatus(ulDfcStatus);

 	if(ulDfcStatus & eDxDFC_STATUS_LETTERBOX)
 	{
		pstVideoOutRect->nY += pstVideoOutRect->nH / 8;
		pstVideoOutRect->nH = pstVideoOutRect->nH * 6 / 8;
	}

	if(ulDfcStatus & eDxDFC_STATUS_LETTERBOX149)
	{
		pstVideoOutRect->nY += pstVideoOutRect->nH / 16;
		pstVideoOutRect->nH = pstVideoOutRect->nH * 14 / 16;
	}

	if(ulDfcStatus & eDxDFC_STATUS_PILLARBOX)
	{
		pstVideoOutRect->nX += pstVideoOutRect->nW / 8;
		pstVideoOutRect->nW = pstVideoOutRect->nW * 6 / 8;
	}

	if(ulDfcStatus & eDxDFC_STATUS_PILLARBOX149)
	{
		pstVideoOutRect->nX += pstVideoOutRect->nW / 16;
		pstVideoOutRect->nW = pstVideoOutRect->nW * 14 / 16;
	}

	if(ulDfcStatus & eDxDFC_STATUS_ZOOM)
	{
		pstVideoInRect->nY += pstVideoInRect->nH / 8;
		pstVideoInRect->nH = pstVideoInRect->nH * 6 / 8;
	}

	if(ulDfcStatus & eDxDFC_STATUS_ZOOM149)
	{
		pstVideoInRect->nY += pstVideoInRect->nH / 16;
		pstVideoInRect->nH = pstVideoInRect->nH * 14 / 16;
	}

	if(ulDfcStatus & eDxDFC_STATUS_SIDECUT)
	{
		pstVideoInRect->nX += pstVideoInRect->nW / 8;
		pstVideoInRect->nW = pstVideoInRect->nW * 6 / 8;
	}

	if(ulDfcStatus & eDxDFC_STATUS_SIDECUT149)
	{
		pstVideoInRect->nX += pstVideoInRect->nW / 16;
		pstVideoInRect->nW = pstVideoInRect->nW * 14 / 16;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_ApplyDfc(HUINT32				ulDecId,
							DxAspectRatio_e			eOutRectAspectRatio,
							DxDfcSelect_e			eDfc,
							DxRect_t				*pstVideoInRect,
							DxRect_t				*pstVideoOutRect,
							DxDfcStatus_b			*pulDfcStatus,
							VIDEO_Wss_t				*peOutputWss,
							VIDEO_Afd_t				*peOutputAfd)
{
	HERROR				err = ERR_OK;
	VIDEO_Wss_t			eOutputWss;
	VIDEO_Afd_t			eOutputAfd;
	DxDfcStatus_b		ulDfcStatus = eDxDFC_STATUS_FULL;
	palScaler_VideoAttribute_t	*pstVideoAttribute = &s_stVideoAttribute[ulDecId];

	HxLOG_Debug("\t%s(%d, OutRect:%s, %s)\n", __FUNCTION__
				, ulDecId
				, OTL_ENUM2STR_AspectRatio(eOutRectAspectRatio)
				, OTL_ENUM2STR_DfcSelect(eDfc)
				);

	/* WSS/AFD를 일단 DFC full 로 초기화. */
	if(eOutRectAspectRatio == eDxASPECTRATIO_4X3)
	{
		eOutputWss = eWss_43_Full;
	}
	else
	{
		eOutputWss = eWss_169_Full;
	}

	eOutputAfd = eAfd_Full;

	switch(eDfc)
	{
		/* Full video aspect ratio adjust 의 강제 모드 처리 */
		case eDxDFC_SELECT_FULLSCREEN:
		case eDxDFC_SELECT_UNKNOWN:
			ulDfcStatus = eDxDFC_STATUS_FULL;
			break;

		case eDxDFC_SELECT_LETTERBOX:
			ulDfcStatus = eDxDFC_STATUS_LETTERBOX;
			break;

		case eDxDFC_SELECT_LETTERBOX149:
			ulDfcStatus = eDxDFC_STATUS_LETTERBOX149;
			break;

		case eDxDFC_SELECT_PILLARBOX:
			ulDfcStatus = eDxDFC_STATUS_PILLARBOX;
			break;

		case eDxDFC_SELECT_PILLARBOX149:
			ulDfcStatus = eDxDFC_STATUS_LETTERBOX149;
			break;

		case eDxDFC_SELECT_ZOOM:
			ulDfcStatus = eDxDFC_STATUS_ZOOM;
			break;

		case eDxDFC_SELECT_ZOOM149:
			ulDfcStatus = eDxDFC_STATUS_ZOOM149;
			break;

		case eDxDFC_SELECT_SIDECUT:
			ulDfcStatus = eDxDFC_STATUS_SIDECUT;
			break;

		case eDxDFC_SELECT_SIDECUT149:
			ulDfcStatus = eDxDFC_STATUS_SIDECUT149;
			break;

		/* Full video aspect ratio adjust 자동 모드 처리. D-Book Part A v 1 문서의 24절 참조 */
		default:
			if(s_stUserSetting.bWideSignalLetterbox == TRUE)
			{
				if(s_stOutputStatus.stOutputTiming.eAspectRatio == eDxASPECTRATIO_16X9)
				{
					if( (pstVideoAttribute->eFrameAspectRatio == eDxASPECTRATIO_4X3) && (pstVideoAttribute->eAfd == eAfd_169) )
					{
						eDfc = eDxDFC_SELECT_WIDESIGNAL;
						HxLOG_Debug("\t\tWide Signal Letterbox Mode. DFC is %s\n", OTL_ENUM2STR_DfcSelect(eDfc));
					}
				}
			}

			switch(pstVideoAttribute->eAfd)
			{
				case eAfd_Full:
					err = pal_scaler_ProcessAfd_Full(pstVideoAttribute->eFrameAspectRatio,
												eOutRectAspectRatio,
												eDfc,
												&eOutputWss,
												&eOutputAfd,
												&ulDfcStatus);
					break;

				case eAfd_43:
					err = pal_scaler_ProcessAfd_43(pstVideoAttribute->eFrameAspectRatio,
												eOutRectAspectRatio,
												eDfc,
												&eOutputWss,
												&eOutputAfd,
												&ulDfcStatus);
					break;

				case eAfd_169:
					err = pal_scaler_ProcessAfd_169(pstVideoAttribute->eFrameAspectRatio,
												eOutRectAspectRatio,
												eDfc,
												&eOutputWss,
												&eOutputAfd,
												&ulDfcStatus);
					break;

				case eAfd_149:
					err = pal_scaler_ProcessAfd_149(pstVideoAttribute->eFrameAspectRatio,
												eOutRectAspectRatio,
												eDfc,
												&eOutputWss,
												&eOutputAfd,
												&ulDfcStatus);
					break;

				case eAfd_43_SnP149:
					err = pal_scaler_ProcessAfd_43_SnP149(pstVideoAttribute->eFrameAspectRatio,
												eOutRectAspectRatio,
												eDfc,
												&eOutputWss,
												&eOutputAfd,
												&ulDfcStatus);
					break;

				case eAfd_169_SnP149:
					err = pal_scaler_ProcessAfd_169_SnP149(pstVideoAttribute->eFrameAspectRatio,
												eOutRectAspectRatio,
												eDfc,
												&eOutputWss,
												&eOutputAfd,
												&ulDfcStatus);
					break;

				case eAfd_169_SnP43:
					err = pal_scaler_ProcessAfd_169_SnP43(pstVideoAttribute->eFrameAspectRatio,
												eOutRectAspectRatio,
												eDfc,
												&eOutputWss,
												&eOutputAfd,
												&ulDfcStatus);
					break;

				default:
					HxLOG_Error("\n\nERROR!!! AFD should be identified !!!\n\n");
					err = ERR_FAIL;
					break;
			}

			break;
	}

	if(err == ERR_OK)
	{ /* 검색된 DFC에 따라 screen rect 계산 */
		pal_scaler_ProcessDfc(ulDfcStatus, pstVideoInRect, pstVideoOutRect);
		*pulDfcStatus = ulDfcStatus;
	}
	else
	{
		*pulDfcStatus = eDxDFC_STATUS_FULL;
	}

	if(peOutputAfd != NULL)
	{
		if(s_stUserSetting.bHdmiAfd == TRUE)
		{
			*peOutputAfd = eOutputAfd;
		}
		else
		{
			*peOutputAfd = eAfd_Full;
		}
	}

	if(peOutputWss != NULL)
	{
		*peOutputWss = eOutputWss;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_FindOsdSetting(void)
{
	HUINT32				i;
	PIG_Rect_t			stPigRect;
	HBOOL				bPigInRect;
	HBOOL				bPigOutRect;

	HxSTD_MemSet(&stPigRect, 0x00, sizeof(PIG_Rect_t));
	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	s_stOsdSetting.eOsdAspectRatio = eDxASPECTRATIO_UNKNOWN;
	s_stOsdSetting.eDfc = eDxDFC_SELECT_UNKNOWN;
	s_stOsdSetting.ePigAspectRatio = eDxASPECTRATIO_UNKNOWN;
	s_bPigInRect = s_bPigOutRect = FALSE;

	for(i = 0; i < NUM_MAX_OSD_PLANE; i++)
	{ /* Find top PIG rect */
		stPigRect = s_stOsdLayerSetting[i].stPigRect;

		/* PIG in rect 검사 */
		if( (stPigRect.ulInRefWidth == 0) || (stPigRect.ulInRefHeight == 0) )
		{ /* Ref. W/H 가 하나라도 0이면 full임. */
			bPigInRect = FALSE;
		}
		else if( (stPigRect.stInRect.nW == 0) || (stPigRect.stInRect.nH == 0) )
		{ /* InRect W/H 가 하나라도 0이면 full 임. */
			bPigInRect = FALSE;
		}
		else if( (stPigRect.stInRect.nX == 0) && (stPigRect.stInRect.nY == 0)
			&& (stPigRect.stInRect.nW == stPigRect.ulInRefWidth) && (stPigRect.stInRect.nH == stPigRect.ulInRefHeight)
		)
		{ /* Full 영역이면 당연히 full 임. */
			bPigInRect = FALSE;
		}
		else
		{ /* PIG IN is set. But further check is needed. */
			if( (stPigRect.stInRect.nX >= 0) && (stPigRect.stInRect.nX <= stPigRect.ulInRefWidth)
				&& (stPigRect.stInRect.nY >= 0) && (stPigRect.stInRect.nY <= stPigRect.ulInRefHeight)
				&& (stPigRect.stInRect.nX+stPigRect.stInRect.nW <= stPigRect.ulInRefWidth)
				&& (stPigRect.stInRect.nY+stPigRect.stInRect.nH <= stPigRect.ulInRefHeight)
			)
			{ /* X, Y, hor. end, ver. end 가 active 영역에 있어야 in rect 가 유효함. */
				bPigInRect = TRUE;
			}
			else
			{ /* 삐져나가면 full 임. */
				bPigInRect = FALSE;
			}
		}

		/* PIG out rect 검사 */
		if( (stPigRect.ulOutRefWidth == 0) || (stPigRect.ulOutRefHeight == 0) )
		{ /* Ref. W/H 가 하나라도 0이면 full임. */
			bPigOutRect = FALSE;
		}
		else if( (stPigRect.stOutRect.nW == 0) || (stPigRect.stOutRect.nH == 0) )
		{ /* OutRect W/H 가 하나라도 0이면 full 임. */
			bPigOutRect = FALSE;
		}
		else if( (stPigRect.stOutRect.nX == 0) && (stPigRect.stOutRect.nY == 0)
			&& (stPigRect.stOutRect.nW == stPigRect.ulOutRefWidth) && (stPigRect.stOutRect.nH == stPigRect.ulOutRefHeight)
		)
		{ /* Full 영역이면 당연히 full 임. */
			bPigOutRect = FALSE;
		}
		else
		{
			if( (stPigRect.stOutRect.nW <= stPigRect.ulOutRefWidth) && (stPigRect.stOutRect.nH <= stPigRect.ulOutRefHeight) )
			{ /* Ref size 보다는 작아야 out rect 유효함. */
				bPigOutRect = TRUE;
			}
			else
			{ /* Ref size 보다 크면 full로 처리함. */
				bPigOutRect = FALSE;
			}

			bPigOutRect = TRUE;
		}

		if( (bPigInRect == TRUE) || (bPigOutRect == TRUE) )
		{ /* PIG가 셋팅되면 모든 파라미터가 이 priority를 따라간다. */
			s_bPigInRect = bPigInRect;
			s_bPigOutRect = bPigOutRect;
			s_stOsdSetting.stPigRect = stPigRect;
			s_stOsdSetting.ePigAspectRatio = s_stOsdLayerSetting[i].ePigAspectRatio;
			s_stOsdSetting.eOsdAspectRatio = s_stOsdLayerSetting[i].eOsdAspectRatio;
			s_stOsdSetting.eDfc = s_stOsdLayerSetting[i].eDfc;

			goto exit;
		}
	}

	for(i = 0; i < NUM_MAX_OSD_PLANE; i++)
	{ /* Find top OSD aspect ratio */
		if(s_stOsdLayerSetting[i].eOsdAspectRatio != eDxASPECTRATIO_UNKNOWN)
		{
			s_stOsdSetting.eOsdAspectRatio = s_stOsdLayerSetting[i].eOsdAspectRatio;
			s_stOsdSetting.eDfc = s_stOsdLayerSetting[i].eDfc;

			goto exit;
		}
	}

	for(i = 0; i < NUM_MAX_OSD_PLANE; i++)
	{ /* Find top DFC */
		if(s_stOsdLayerSetting[i].eDfc != eDxDFC_SELECT_UNKNOWN)
		{
			s_stOsdSetting.eDfc = s_stOsdLayerSetting[i].eDfc;

			goto exit;
		}
	}

exit:
	if(i != NUM_MAX_OSD_PLANE)
	{
		HxLOG_Debug("\t\tLayer[%d] IN-RECT[%d] (%d, %d, %d, %d)/(%d, %d)\n"
									, i
									, s_bPigInRect
									, stPigRect.stInRect.nX
									, stPigRect.stInRect.nY
									, stPigRect.stInRect.nW
									, stPigRect.stInRect.nH
									, stPigRect.ulInRefWidth
									, stPigRect.ulInRefHeight
									);
		HxLOG_Debug("\t\tLayer[%d] OUT-RECT[%d] (%d, %d, %d, %d)/(%d, %d)\n"
									, i
									, s_bPigOutRect
									, stPigRect.stOutRect.nX
									, stPigRect.stOutRect.nY
									, stPigRect.stOutRect.nW
									, stPigRect.stOutRect.nH
									, stPigRect.ulOutRefWidth
									, stPigRect.ulOutRefHeight
									);
		HxLOG_Debug("\t\tLayer[%d] PIG Aspect ratio (%s)\n", i, OTL_ENUM2STR_AspectRatio(s_stOsdSetting.ePigAspectRatio));
		HxLOG_Debug("\t\tLayer[%d] OSD Aspect Ratio <%s>\n", i, OTL_ENUM2STR_AspectRatio(s_stOsdSetting.eOsdAspectRatio));
		HxLOG_Debug("\t\tLayer[%d] OSD DFC <%s>\n", i, OTL_ENUM2STR_DfcSelect(s_stOsdSetting.eDfc));
	}
	else
	{
		HxLOG_Debug("\t\tNO OSD layer setting.\n");
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_ApplyPigRect(
								DxRect_t		*pstVideoInRect,
								DxRect_t		*pstVideoOutRect,
								VIDEO_Wss_t		*peOutputWss,
								VIDEO_Afd_t		*peOutputAfd,
								HBOOL			*pbRoundOff
								)

{
	PIG_Rect_t			stPigRect;
	DxRect_t			stSourceRect;
	palScaler_Status_t		*pstScalerStatus = &s_stScalerStatus[s_ulMainDecId];

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	stPigRect = s_stOsdSetting.stPigRect;
	stSourceRect = *pstVideoInRect;

	/* round-off 여부 체크 */
	*pbRoundOff = stPigRect.bRoundOff;
	pstScalerStatus->bRoundOff        = stPigRect.bRoundOff;
	pstScalerStatus->bForceSourceBase = stPigRect.bForceSourceBase;
	
	/* In rect 적용. */
	if(s_bPigInRect == TRUE)
	{
		pal_scaler_GetScaledRect(stPigRect.stInRect, stPigRect.ulInRefWidth, stPigRect.ulInRefHeight, stSourceRect, pstVideoInRect);

		HxLOG_Debug("\t\tVideo IN rect is SET(%d, %d, %d, %d)/(%d, %d) => ACTUAL(%d, %d, %d, %d)\n"
						, stPigRect.stInRect.nX
						, stPigRect.stInRect.nY
						, stPigRect.stInRect.nW
						, stPigRect.stInRect.nH
						, stPigRect.ulInRefWidth
						, stPigRect.ulInRefHeight
						, pstVideoInRect->nX
						, pstVideoInRect->nY
						, pstVideoInRect->nW
						, pstVideoInRect->nH
						);
	}
	else
	{ /* in rect가 지정되지 않으면 그대로 사용 */
		HxLOG_Debug("\t\tVideo IN rect is NOT set.\n");
	}

	/*Out rect 적용 */
	if(s_bPigOutRect == TRUE)
	{
		pal_scaler_GetScaledRect(stPigRect.stOutRect, stPigRect.ulOutRefWidth, stPigRect.ulOutRefHeight, pstScalerStatus->stScreenMediaRect, pstVideoOutRect);

		HxLOG_Debug("\t\tVideo OUT rect is SET(%d, %d, %d, %d)/(%d, %d) => ACTUAL(%d, %d, %d, %d)\n"
						, stPigRect.stOutRect.nX
						, stPigRect.stOutRect.nY
						, stPigRect.stOutRect.nW
						, stPigRect.stOutRect.nH
						, stPigRect.ulOutRefWidth
						, stPigRect.ulOutRefHeight
						, pstVideoOutRect->nX
						, pstVideoOutRect->nY
						, pstVideoOutRect->nW
						, pstVideoOutRect->nH
						);

		if(s_stOutputStatus.stOutputTiming.eAspectRatio == eDxASPECTRATIO_4X3)
		{
			*peOutputWss = eWss_43_Full;
		}
		else
		{
			*peOutputWss = eWss_169_Full;
		}

		*peOutputAfd = eAfd_Full;

		HxLOG_Debug("\t\tWSS[%s], AFD[%s]\n", OTL_ENUM2STR_Wss(*peOutputWss), OTL_ENUM2STR_Afd(*peOutputAfd));

		return ERR_OK;
	}
	else
	{
		HxLOG_Debug("\t\tVideo OUT rect is NOT set.\n");

		return ERR_FAIL;
	}
}

STATIC HERROR pal_scaler_ClipScreen(HUINT32 ulDecId)
{
	DxRect_t	stVideoInRect = s_stScalerStatus[ulDecId].stVideoInRect;
	DxRect_t	stVideoOutRect = s_stScalerStatus[ulDecId].stVideoOutRect;
	HINT32		nScreenW, nScreenH;
	HUINT32 	ulInOffset, ulOutOffset;

	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, ulDecId);

	pal_scaler_GetOutputResSize(s_stOutputStatus.stOutputTiming.eResolution, &nScreenW, &nScreenH);

	/* Out rect 에 대한 clipping 처리 */
	if(stVideoOutRect.nX < 0)
	{ /* 음수 값이면 그 비율 만큼 in rect를 조정한다. */
		ulOutOffset = abs(stVideoOutRect.nX);
		ulInOffset = (ulOutOffset * stVideoInRect.nW) / stVideoOutRect.nW;

		stVideoInRect.nX += ulInOffset;
		stVideoInRect.nW -= ulInOffset;

		stVideoOutRect.nX = 0;
	}

	if(stVideoOutRect.nX >= nScreenW)
	{ /* Screen 오른쪽으로 완전히 나갔으면 오른쪽에 붙은채로 고정. */
		stVideoInRect.nW = 2;
	}
	else if( (stVideoOutRect.nX + stVideoOutRect.nW) > nScreenW )
	{ /* Screen 오른쪽으로 걸쳐서 삐져나갔으면 그 비율만큼 in rect를 조정한다. */
		ulOutOffset = (stVideoOutRect.nX + stVideoOutRect.nW) - nScreenW;
		ulInOffset = (ulOutOffset * stVideoInRect.nW) / stVideoOutRect.nW;

		stVideoInRect.nW -= ulInOffset;
		stVideoOutRect.nW -= ulOutOffset;
	}

	if(stVideoOutRect.nY < 0)
	{ /* 음수 값이면 그 비율 만큼 in rect를 조정한다. */
		ulOutOffset = abs(stVideoOutRect.nY);
		ulInOffset = (ulOutOffset * stVideoInRect.nH) / stVideoOutRect.nH;

		stVideoInRect.nY += ulInOffset;
		stVideoInRect.nH -= ulInOffset;

		stVideoOutRect.nY = 0;
	}

	if(stVideoOutRect.nY >= nScreenH)
	{ /* Screen 아래로 완전히 나갔으면 아래에 붙은채로 고정. */
		stVideoInRect.nH = 2;
	}
	else if( (stVideoOutRect.nY + stVideoOutRect.nH) > nScreenH )
	{ /* Screen OSD rect 오른쪽으로 걸쳐서 삐져나갔으면 그 비율만큼 in rect를 조정한다. */
		ulOutOffset = (stVideoOutRect.nY + stVideoOutRect.nH) - nScreenH;
		ulInOffset = (ulOutOffset * stVideoInRect.nH) / stVideoOutRect.nH;

		stVideoInRect.nH -= ulInOffset;
		stVideoOutRect.nH -= ulOutOffset;
	}

	HxLOG_Debug("\t\tScreen clipped Video InRect(%d, %d, %d, %d)\n",
								stVideoInRect.nX,
								stVideoInRect.nY,
								stVideoInRect.nW,
								stVideoInRect.nH);
	HxLOG_Debug("\t\tScreen clipped Video OutRect(%d, %d, %d, %d)\n",
								stVideoOutRect.nX,
								stVideoOutRect.nY,
								stVideoOutRect.nW,
								stVideoOutRect.nH);

	s_stScalerStatus[ulDecId].stVideoInRect = stVideoInRect;
	s_stScalerStatus[ulDecId].stVideoOutRect = stVideoOutRect;

	return ERR_OK;
}

STATIC HERROR pal_scaler_FindScreenOsdShrink(VIDEO_Wss_t *peOutputWss, VIDEO_Afd_t *peOutputAfd)
{
	VIDEO_Wss_t 				eOutputWss;
	VIDEO_Afd_t 				eOutputAfd;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	s_stOutputStatus.eScreenOsdShrink = eDxDFC_STATUS_FULL;

	/* 소스와 출력 비교하여 WSS, AFD 조정 */
	if(s_stOsdSetting.eOsdAspectRatio == eDxASPECTRATIO_4X3)
	{ /* 4:3 소스 */
		if(s_stOutputStatus.stOutputTiming.eAspectRatio == eDxASPECTRATIO_4X3)
		{ /* 4:3 출력 - OK */
			eOutputWss = eWss_43_Full;
			eOutputAfd = eAfd_Full;
		}
		else
		{ /* 16:9 출력 - NOT OK */
			switch(s_stUserSetting.eDfc)
			{
				case eDxDFC_SELECT_DEFAULT:
				case eDxDFC_SELECT_WIDESIGNAL:
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_43;
					break;

				case eDxDFC_SELECT_SQUEEZE: /* 169 pillar box */
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_43;

					s_stOutputStatus.eScreenOsdShrink = eDxDFC_STATUS_PILLARBOX;

					break;

				case eDxDFC_SELECT_SQUEEZE149: /* 149 pillar box */
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_149;

					s_stOutputStatus.eScreenOsdShrink = eDxDFC_STATUS_PILLARBOX149;

					break;

				default: /* 나머지는 모두 full 처리 */
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_Full;
					break;
			}
		}
	}
	else if(s_stOsdSetting.eOsdAspectRatio == eDxASPECTRATIO_16X9)
	{ /* 16:9 소스 */
		if(s_stOutputStatus.stOutputTiming.eAspectRatio == eDxASPECTRATIO_16X9)
		{ /* 16:9 출력 - OK */
			eOutputWss = eWss_169_Full;
			eOutputAfd = eAfd_Full;
		}
		else
		{ /* 4:3 출력 - NOT OK */
			switch(s_stUserSetting.eDfc)
			{
				case eDxDFC_SELECT_DEFAULT:
				case eDxDFC_SELECT_WIDESIGNAL:
					eOutputWss = eWss_169_Full;
					eOutputAfd = eAfd_169;
					break;

				case eDxDFC_SELECT_SQUEEZE: /* 169 letter box */
					eOutputWss = eWss_43_LB169_Center;
					eOutputAfd = eAfd_169;

					s_stOutputStatus.eScreenOsdShrink = eDxDFC_STATUS_LETTERBOX;

					break;

				case eDxDFC_SELECT_SQUEEZE149: /* 149 letter box */
					eOutputWss = eWss_43_LB149_Center;
					eOutputAfd = eAfd_149;

					s_stOutputStatus.eScreenOsdShrink = eDxDFC_STATUS_LETTERBOX149;

					break;

				default: /* 나머지는 모두 full 처리 */
					eOutputWss = eWss_43_Full;
					eOutputAfd = eAfd_Full;
					break;
			}
		}
	}
	else
	{
		HxLOG_Debug("\t\tOSD AR NOT SET.\n");

		*peOutputWss = eWss_Unknown;
		*peOutputAfd = eAfd_None;

		return ERR_FAIL;
	}

	pal_scaler_PrintDfcStatus(s_stOutputStatus.eScreenOsdShrink);
	HxLOG_Debug("\t\tOSD Aspect Ratio WSS(%s), AFD(%s)\n", OTL_ENUM2STR_Wss(eOutputWss), OTL_ENUM2STR_Afd(eOutputAfd));

	if(peOutputWss != NULL)
	{
		*peOutputWss = eOutputWss;
	}

	if(peOutputAfd != NULL)
	{
		*peOutputAfd = eOutputAfd;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_FindMainVideoScaling(VIDEO_Wss_t *peOutputWss, VIDEO_Afd_t *peOutputAfd)
{
	HERROR					err;
	DxDfcSelect_e 			eDfc;
	DxAspectRatio_e			eOutRectAspectRatio;
	DxRect_t				stVideoInRect;
	DxRect_t				stVideoOutRect;
	palScaler_VideoAttribute_t		*pstVideoAttrubute = &s_stVideoAttribute[s_ulMainDecId];
	palScaler_Status_t			*pstScalerStatus = &s_stScalerStatus[s_ulMainDecId];
	VIDEO_Wss_t				eWss;
	VIDEO_Afd_t				eAfd;
	HBOOL					bRoundOff = FALSE;

	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, s_ulMainDecId);

	*peOutputWss = eWss_Unknown;
	*peOutputAfd = eAfd_None;

	if(pstVideoAttrubute->bInfoValid != TRUE)
	{ /* 비디오 안돌고 있음.  */
		HxLOG_Debug("\t\tNo Video info. Exit ...\n");
		return ERR_FAIL;
	}

/* video in/out rect 초기화 */
	stVideoInRect.nX = 0;
	stVideoInRect.nY = 0;
	stVideoInRect.nW = pstVideoAttrubute->ulFrameHorizontalSize;
	stVideoInRect.nH = pstVideoAttrubute->ulFrameVerticalSize;

	if(pstVideoAttrubute->eVideoUsage == ePAL_SCALER_VIDEOUSAGE_CAPTURE)
	{ /* Capture 인 경우는 무조건 full screen으로 환원하고 모든 설정 무시. */
		HxLOG_Debug("\t\tCapture Mode. Full screen.\n");

		pstScalerStatus->stVideoInRect = stVideoInRect;

		stVideoOutRect.nX = stVideoOutRect.nY = 0;
		pal_scaler_GetOutputResSize(s_stOutputStatus.stOutputTiming.eResolution, &stVideoOutRect.nW, &stVideoOutRect.nH);

		pstScalerStatus->stVideoOutRect = stVideoOutRect;

		s_bPigInRect = s_bPigOutRect = FALSE;

		return ERR_OK;
	}
	else
	{
		stVideoOutRect = pstScalerStatus->stScreenMediaRect;
	}

/* PIG 적용된 video in/out rect 및 wss, afd 계산  */
	err = pal_scaler_ApplyPigRect(&stVideoInRect, &stVideoOutRect, &eWss, &eAfd, &bRoundOff);
	if(err == ERR_OK)
	{
		*peOutputWss = eWss;
		*peOutputAfd = eAfd;
	}

/* DFC 적용된 in/out rect 및 wss, afd 계산 */
	/* DFC priority에 따라 WSS/AFD 결정 및 in/out rect 조정. */
	if(pstVideoAttrubute->eVideoUsage == ePAL_SCALER_VIDEOUSAGE_STILL)
	{
		eDfc = eDxDFC_SELECT_FULLSCREEN;
		HxLOG_Debug("\t\tStill picture DFC(%s)\n", OTL_ENUM2STR_DfcSelect(eDfc));
	}
	else if( (s_stUserSetting.e3DSelect == eDxHDMI_3DSELECT_SIDEBYSIDE)
			|| (s_stUserSetting.e3DSelect == eDxHDMI_3DSELECT_TOPANDBOTTOM)
	)
	{
		eDfc = eDxDFC_SELECT_FULLSCREEN;
		HxLOG_Debug("\t\t3D Content DFC(%s)\n", OTL_ENUM2STR_DfcSelect(eDfc));
	}
	else if(s_stOsdSetting.eDfc != eDxDFC_SELECT_UNKNOWN)
	{
		eDfc = s_stOsdSetting.eDfc;
		HxLOG_Debug("\t\tOSD Layer DFC(%s)\n", OTL_ENUM2STR_DfcSelect(eDfc));
	}
	else
	{
		eDfc = s_stUserSetting.eDfc;
		HxLOG_Debug("\t\tUser Setting DFC(%s)\n", OTL_ENUM2STR_DfcSelect(eDfc));
	}

	/* OutRect에 적용할 aspect ratio 설정 */
	if(s_stOsdSetting.ePigAspectRatio != eDxASPECTRATIO_UNKNOWN)
	{ /* OSD layer가 설정한 aspect ratio 우선 반영 */
		eOutRectAspectRatio = s_stOsdSetting.ePigAspectRatio;
		HxLOG_Debug("\t\tPIG Aspect Ratio (%s)\n", OTL_ENUM2STR_AspectRatio(eOutRectAspectRatio));
	}
	else
	{ /* OSD layer에 설정이 없으면 비디오 출력 aspect ratio 사용 */
		eOutRectAspectRatio = s_stOutputStatus.stOutputTiming.eAspectRatio;
		HxLOG_Debug("\t\tOutput Aspect Ratio (%s)\n", OTL_ENUM2STR_AspectRatio(eOutRectAspectRatio));
	}

	if(pstVideoAttrubute->eFrameAspectRatio == eDxASPECTRATIO_1X1)
	{
		HxLOG_Debug("\t\t1:1 Aspect ratio DFC(%s)\n", OTL_ENUM2STR_DfcSelect(eDfc));

		switch(eDfc)
		{
			case eDxDFC_SELECT_CENTERCUTOUT:
			case eDxDFC_SELECT_CENTERCUTOUT149:
				pal_scaler_ApplyPixelCutOut(&stVideoInRect, &stVideoOutRect, bRoundOff);
				break;

			default:
				pal_scaler_ApplyPixelSqueeze(&stVideoInRect, &stVideoOutRect, bRoundOff);
				break;
		}

		s_stScalerStatus[s_ulMainDecId].ulDfcStatus = eDxDFC_STATUS_FULL;

		if(eOutRectAspectRatio == eDxASPECTRATIO_4X3)
		{
			eWss = eWss_43_Full;
		}
		else
		{
			eWss = eWss_169_Full;
		}

		eAfd = eAfd_Full;
	}
	else
	{
		pal_scaler_ApplyDfc(s_ulMainDecId,
					eOutRectAspectRatio,
					eDfc,
					&stVideoInRect,
					&stVideoOutRect,
					&s_stScalerStatus[s_ulMainDecId].ulDfcStatus,
					&eWss,
					&eAfd);
	}

	if(*peOutputWss == eWss_Unknown)
	{
		*peOutputWss = eWss;
		*peOutputAfd = eAfd;
	}

	/* 최종 video in/out rect 기록 */
	pstScalerStatus->stVideoInRect = stVideoInRect;
	pstScalerStatus->stVideoOutRect = stVideoOutRect;

	pal_scaler_ClipScreen(s_ulMainDecId);

	return ERR_OK;
}

STATIC HERROR pal_scaler_FindSubVideoScaling(HUINT32 ulDecId)
{
	palScaler_Status_t		*pstScalerStatus = &s_stScalerStatus[ulDecId];
	palScaler_VideoAttribute_t	*pstVideoAttrubute = &s_stVideoAttribute[ulDecId];
	DxRect_t			stVideoInRect;
	DxRect_t			stVideoOutRect;
	DxDfcSelect_e			eDfc;
	HBOOL				bRoundOff = FALSE;

	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, ulDecId);

	if(pstVideoAttrubute->bInfoValid != TRUE)
	{ /* 비디오 안돌고 있음. */
		HxLOG_Debug("\t\tNo video info. Exit...\n");
		return ERR_FAIL;
	}

/* Video in/out rect를 full screen으로 초기화 */
	stVideoInRect.nX = 0;
	stVideoInRect.nY = 0;
	stVideoInRect.nW = pstVideoAttrubute->ulFrameHorizontalSize;
	stVideoInRect.nH = pstVideoAttrubute->ulFrameVerticalSize;

	stVideoOutRect = pstScalerStatus->stScreenMediaRect;

/* DFC priority에 따라 WSS/AFD 결정 및 in/out rect 조정. */
	if(pstVideoAttrubute->eVideoUsage == ePAL_SCALER_VIDEOUSAGE_STILL)
	{
		eDfc = eDxDFC_SELECT_FULLSCREEN;
		HxLOG_Debug("\t\tStill picture DFC(%s)\n", OTL_ENUM2STR_DfcSelect(eDfc));
	}
	else if( (s_stUserSetting.e3DSelect == eDxHDMI_3DSELECT_SIDEBYSIDE)
			|| (s_stUserSetting.e3DSelect == eDxHDMI_3DSELECT_TOPANDBOTTOM)
	)
	{
		eDfc = eDxDFC_SELECT_FULLSCREEN;
		HxLOG_Debug("\t\t3D Content DFC(%s)\n", OTL_ENUM2STR_DfcSelect(eDfc));
	}
	else
	{
		eDfc = s_stUserSetting.eDfc;
		HxLOG_Debug("\t\tUser Setting DFC(%s)\n", OTL_ENUM2STR_DfcSelect(eDfc));
	}

	/* rounding-off 여부(used at Netflix)  */
	bRoundOff = pstScalerStatus->bRoundOff;
	
/* Full screen 기준 DFC 적용된 in/out rect 및 wss, afd 계산 */
	if(pstVideoAttrubute->eFrameAspectRatio == eDxASPECTRATIO_1X1)
	{
		HxLOG_Debug("\t\t1:1 Aspect ratio DFC(%s)\n", OTL_ENUM2STR_DfcSelect(eDfc));

		switch(eDfc)
		{
			case eDxDFC_SELECT_CENTERCUTOUT:
			case eDxDFC_SELECT_CENTERCUTOUT149:
				pal_scaler_ApplyPixelCutOut(&stVideoInRect, &stVideoOutRect, bRoundOff);
				break;

			default:
				pal_scaler_ApplyPixelSqueeze(&stVideoInRect, &stVideoOutRect, bRoundOff);
				break;
		}

		s_stScalerStatus[ulDecId].ulDfcStatus = eDxDFC_STATUS_FULL;
	}
	else
	{
		pal_scaler_ApplyDfc(ulDecId,
					s_stOutputStatus.stOutputTiming.eAspectRatio,
					eDfc,
					&stVideoInRect,
					&stVideoOutRect,
					&s_stScalerStatus[s_ulMainDecId].ulDfcStatus,
					NULL,
					NULL);
	}

/* 최종 video in/out rect 기록 */
	pstScalerStatus->stVideoInRect = stVideoInRect;
	pstScalerStatus->stVideoOutRect = stVideoOutRect;

	pal_scaler_ClipScreen(ulDecId);

	return ERR_OK;
}

STATIC HERROR pal_scaler_CommitMainMediaRect(void)
{
	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, s_ulMainDecId);

	if(s_stScalerStatus[s_ulMainDecId].stDiHdClearRect.ulWidth != 0) /* width의 validity를 flag로 사용중 */
	{ /* Main은 clear rect 없음 */
		HxLOG_Debug("\t\tClear main decoder clear rect.\n");
#if 0
		DI_VIDEO_SetClearRect(s_ulMainDecId, FALSE, NULL, NULL, DI_VIDEO_WIN_ALL);
#endif
		HxSTD_memset(&s_stScalerStatus[s_ulMainDecId].stDiHdClearRect, 0, sizeof(DI_VIDEO_RECT_t));
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_CommitSubMediaRect(HUINT32 ulDecId)
{
	palScaler_Status_t		*pstScalerStatus = &s_stScalerStatus[ulDecId];
	HUINT32				ulHdOutputW, ulHdOutputH;
	DxRect_t			stSdScreenMediaRect;

	DI_ERR_CODE			diErr = DI_ERR_OK;
	DI_VIDEO_COLOR_t	stDiColor;
	DI_VIDEO_RECT_t		stDiClearRect;

	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, ulDecId);

	if(pstScalerStatus->bSubScreenMedia == TRUE)
	{
		stDiClearRect.ulX = MAKE_EVEN_NUM(pstScalerStatus->stScreenMediaRect.nX);
		stDiClearRect.ulY = MAKE_EVEN_NUM(pstScalerStatus->stScreenMediaRect.nY);
		stDiClearRect.ulWidth = MAKE_EVEN_NUM(pstScalerStatus->stScreenMediaRect.nW);
		stDiClearRect.ulHeight = MAKE_EVEN_NUM(pstScalerStatus->stScreenMediaRect.nH);
	}
	else
	{
		HxSTD_memset(&stDiClearRect, 0, sizeof(DI_VIDEO_RECT_t));
	}

	if(HxSTD_memcmp(&stDiClearRect, &pstScalerStatus->stDiHdClearRect, sizeof(DI_VIDEO_RECT_t)) != 0)
	{
		if(stDiClearRect.ulWidth == 0)
		{
			HxLOG_Debug("\t\tClear sub decoder clear rect.\n");
#if 0
			DI_VIDEO_SetClearRect(s_ulMainDecId, FALSE, NULL, NULL, DI_VIDEO_WIN_ALL);
#endif
		}
		else
		{
			stDiColor.a = 0xff;
			stDiColor.r = s_ucBgColor_R;
			stDiColor.g = s_ucBgColor_G;
			stDiColor.b = s_ucBgColor_B;
			NOT_USED_PARAM(stDiColor);

			HxLOG_Debug("\t\tSet sub decoder HD clear rect(%d, %d, %d, %d).\n"
						, stDiClearRect.ulX
						, stDiClearRect.ulY
						, stDiClearRect.ulWidth
						, stDiClearRect.ulHeight
						);

#if 0
			diErr = DI_VIDEO_SetClearRect(s_ulMainDecId, TRUE, &stDiClearRect, &stDiColor, DI_VIDEO_WIN_HD);
#endif
			if(diErr == DI_ERR_OK)
			{
				pstScalerStatus->stDiHdClearRect = stDiClearRect;

				/* SD screen 도 설정 */
				stSdScreenMediaRect.nX = stSdScreenMediaRect.nY = 0;
				stSdScreenMediaRect.nW = 720;
#if defined(CONFIG_MW_OUTPUT_ANALOG_STANDARD_NTSC)
				stSdScreenMediaRect.nH = 480;
#else
				stSdScreenMediaRect.nH = 576;
#endif

				pal_scaler_GetOutputResSize(s_stOutputStatus.stOutputTiming.eResolution, &ulHdOutputW, &ulHdOutputH);
				pal_scaler_GetScaledRect(pstScalerStatus->stScreenMediaRect, ulHdOutputW, ulHdOutputH, stSdScreenMediaRect, &stSdScreenMediaRect);

				stDiClearRect.ulX = MAKE_EVEN_NUM(stSdScreenMediaRect.nX);
				stDiClearRect.ulY = MAKE_EVEN_NUM(stSdScreenMediaRect.nY);
				stDiClearRect.ulWidth = MAKE_EVEN_NUM(stSdScreenMediaRect.nW);
				stDiClearRect.ulHeight = MAKE_EVEN_NUM(stSdScreenMediaRect.nH);

				HxLOG_Debug("\t\tSet sub decoder SD clear rect(%d, %d, %d, %d).\n"
							, stDiClearRect.ulX
							, stDiClearRect.ulY
							, stDiClearRect.ulWidth
							, stDiClearRect.ulHeight
							);

#if 0
				DI_VIDEO_SetClearRect(s_ulMainDecId, TRUE, &stDiClearRect, &stDiColor, DI_VIDEO_WIN_SD);
#endif
			}
			else
			{
				HxLOG_Error("!!! ERROR : DI_VIDEO_SetClearRect() ERROR !!!\n\n");
				pstScalerStatus->stDiHdClearRect.ulWidth = -1;

				return ERR_FAIL;
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_CommitVideoRect(HUINT32 ulDecId)
{
	palScaler_Status_t				*pstScalerStatus = &s_stScalerStatus[ulDecId];
	HUINT32						ulHdOuputW, ulHdOutputH;
	HUINT32						ulSdOutputH;
	HUINT32					     ulDiDecId;

	/* DI params. */
	DI_VIDEO_RECT_t				stDiInRect = {0, };
	DI_VIDEO_RECT_t				stDiOutRect = {0, };
	HBOOL 						bTouched = 0;
	DI_ERR_CODE					diErr;
	HBOOL						bForceSourceBase = FALSE;

	HxLOG_Debug("\t%s(%d)\n", __FUNCTION__, ulDecId);

	if(s_stVideoAttribute[ulDecId].bInfoValid != TRUE)
	{
		HxLOG_Debug("\t\tNo video info. exit ...\n");
		return ERR_FAIL;
	}

// 임시로 만듦 , PIP 가 enable되면 어딘가에서  실제 DI Dec ID가져오는 부분 만들어야 함
#if defined(CONFIG_SUPPORT_FCC)
	ulDiDecId = FCC_PAL_MAIN_DECODER_ID;
#else
	ulDiDecId = ulDecId;
#endif

	/* Check whether this scaling is for videoResize under DASH streaming */
	bForceSourceBase = pstScalerStatus->bForceSourceBase;

	// MAKE_EVEN_NUM macro를 사용하지 않도록 수정.
	// MAKE_EVEN_NUM을 사용시에 pixel이 틀어지는 문제가 있습니다.
	stDiInRect.ulX = (pstScalerStatus->stVideoInRect.nX);
	stDiInRect.ulY = (pstScalerStatus->stVideoInRect.nY);
	stDiInRect.ulWidth = (pstScalerStatus->stVideoInRect.nW);
	stDiInRect.ulHeight = (pstScalerStatus->stVideoInRect.nH);

	stDiOutRect.ulX = (pstScalerStatus->stVideoOutRect.nX);
	stDiOutRect.ulY = (pstScalerStatus->stVideoOutRect.nY);
	stDiOutRect.ulWidth = (pstScalerStatus->stVideoOutRect.nW);
	stDiOutRect.ulHeight = (pstScalerStatus->stVideoOutRect.nH);

	if( (HxSTD_memcmp(&stDiInRect, &pstScalerStatus->stDiVideoInRect, sizeof(DI_VIDEO_RECT_t)) != 0)
	|| (HxSTD_memcmp(&stDiOutRect, &pstScalerStatus->stDiVideoOutRect, sizeof(DI_VIDEO_RECT_t)) != 0)
	)
	{ /* 윈도우 크기 변경됨. 다시 DI 설정 */
		HxLOG_Debug("\t\tdi_in x(%d), y(%d), w(%d), h(%d) \n", stDiInRect.ulX, stDiInRect.ulY, stDiInRect.ulWidth, stDiInRect.ulHeight);
		HxLOG_Debug("\t\tdi_out x(%d), y(%d), w(%d), h(%d) \n", stDiOutRect.ulX, stDiOutRect.ulY, stDiOutRect.ulWidth, stDiOutRect.ulHeight);

		/* 
		 * [Netflix NTS VIDEO TC 수정사항]
		 *  DASH resize 시 순간적인 stream-source change로 pal_scaler에서 계산한 inRect과 DI_VIDEO 내부 참조 스트림 정보가 상이해서
                 *  NEXUS_VideoWindow_SetSettings() error 유발할 수 있음.
		 *  full clipping의 경우 DRV_VIDEO_SetDisplayWindow() 안에서 clipBase/clipRect이 모두 source stream을 참조하도록 (0,0,0,0) 설정
		 */
		if (bForceSourceBase == TRUE)
		{
			HxSTD_MemSet(&stDiInRect, 0x0, sizeof(DI_VIDEO_RECT_t));
		}

		diErr = DI_VIDEO_SetInOutWindow(ulDiDecId, &stDiInRect, &stDiOutRect, &bTouched, DI_VIDEO_WIN_HD);
		if(diErr == DI_ERR_OK)
		{
			HxSTD_memcpy(&pstScalerStatus->stDiVideoInRect, &stDiInRect, sizeof(DI_VIDEO_RECT_t));
			HxSTD_memcpy(&pstScalerStatus->stDiVideoOutRect, &stDiOutRect, sizeof(DI_VIDEO_RECT_t));

			pal_scaler_GetOutputResSize(s_stOutputStatus.stOutputTiming.eResolution, &ulHdOuputW, &ulHdOutputH);

#if defined(CONFIG_MW_OUTPUT_ANALOG_STANDARD_NTSC)
			ulSdOutputH = 480;
#else
			ulSdOutputH = 576;
#endif

			// MAKE_EVEN_NUM macro를 사용하지 않도록 수정.
			// MAKE_EVEN_NUM을 사용시에 pixel이 틀어지는 문제가 있습니다.
			stDiOutRect.ulX = (stDiOutRect.ulX * 720 / ulHdOuputW);
			stDiOutRect.ulWidth = (stDiOutRect.ulWidth * 720 / ulHdOuputW);
			stDiOutRect.ulY = (stDiOutRect.ulY * ulSdOutputH / ulHdOutputH);
			stDiOutRect.ulHeight = (stDiOutRect.ulHeight * ulSdOutputH / ulHdOutputH);

			DI_VIDEO_SetInOutWindow(ulDiDecId, &stDiInRect, &stDiOutRect, &bTouched, DI_VIDEO_WIN_SD);
		}
		else
		{
			HxLOG_Error("!!! ERROR : DI_VIDEO_SetInOutWindow() ERROR !!!\n\n");

			pstScalerStatus->stDiVideoInRect.ulX = -1;
			pstScalerStatus->stDiVideoOutRect.ulX = -1;

			return ERR_FAIL;
		}
	}
	else
	{
		HxLOG_Debug("\t\tSame ... Exit ...\n");
	}

	return ERR_OK;
}

STATIC void pal_scaler_CommitOsdRect(void)
{
	HxLOG_Debug("\t%s()\n", __FUNCTION__);

}

STATIC HERROR pal_scaler_ChangeDfbVideo(DI_VIDEO_HD_RESOLUTION eRes)
{
#if defined(CONFIG_PACKAGE_DIRECTFB)
	//DFBSurfaceDescription	dsc;
	DFBScreenEncoderConfig	encoderCfg;

	HxLOG_Debug("%s(0x%X)\n",__FUNCTION__, eRes);

	if(NULL == _primary_screen)
	{	// for debugging
		HxLOG_Error(HxANSI_COLOR_CYAN("_primary_screen is NULL !!!!!!!!!!!\n"));
		HxLOG_Error(HxANSI_COLOR_CYAN("_primary_screen is NULL !!!!!!!!!!!\n"));
		HxLOG_Error(HxANSI_COLOR_CYAN("_primary_screen is NULL !!!!!!!!!!!\n"));
		HxLOG_Error(HxANSI_COLOR_CYAN("_primary_screen is NULL !!!!!!!!!!!\n"));
		HxLOG_Error(HxANSI_COLOR_CYAN("_primary_screen is NULL !!!!!!!!!!!\n"));
		HxLOG_Assert(NULL == _primary_screen);
		return ERR_FAIL;
	}

	DFBCHECK(_primary_screen->GetEncoderConfiguration( _primary_screen, 0, &encoderCfg ));

//	dump_encoder_config((const DFBScreenEncoderConfig *)&encoder)
	encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS |
				DSECONF_SCANMODE | DSECONF_FREQUENCY | DSECONF_RESOLUTION );
	encoderCfg.tv_standard = DSETV_DIGITAL;
	encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#if defined(CONFIG_MW_OUTPUT_COMPONENT)
	encoderCfg.out_connectors |= (DFBScreenOutputConnectors)(DSOC_COMPONENT);
#endif

	switch (eRes)
	{
		case DI_VIDEO_HD_RESOLUTION_480I:
		case DI_VIDEO_HD_RESOLUTION_480I_J:
			encoderCfg.frequency = DSEF_60HZ;
			encoderCfg.scanmode = DSESM_INTERLACED;
			encoderCfg.resolution = DSOR_720_480;
			encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS);
			encoderCfg.tv_standard = DSETV_NTSC;
			break;

		case DI_VIDEO_HD_RESOLUTION_480P:
			encoderCfg.frequency = DSEF_60HZ;
			encoderCfg.scanmode = DSESM_PROGRESSIVE;
			encoderCfg.resolution = DSOR_720_480;
//			encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS);
//			encoderCfg.tv_standard = DSETV_NTSC;
			break;

		case DI_VIDEO_HD_RESOLUTION_576I:
			encoderCfg.frequency	= DSEF_50HZ;
			encoderCfg.scanmode		= DSESM_INTERLACED;
			encoderCfg.resolution	= DSOR_720_576;
			encoderCfg.flags		= (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS);
			encoderCfg.tv_standard	= DSETV_PAL;
			break;

		case DI_VIDEO_HD_RESOLUTION_576P:
			encoderCfg.frequency	= DSEF_50HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_720_576;
//			encoderCfg.flags		= (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS);
//			encoderCfg.tv_standard	= DSETV_PAL;
			break;

		case DI_VIDEO_HD_RESOLUTION_720P:
			encoderCfg.frequency	= DSEF_60HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_1280_720;
			break;

		case DI_VIDEO_HD_RESOLUTION_720P_50HZ:
			encoderCfg.frequency	= DSEF_50HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_1280_720;
			break;

		case DI_VIDEO_HD_RESOLUTION_1080I:
			encoderCfg.frequency	= DSEF_60HZ;
			encoderCfg.scanmode		= DSESM_INTERLACED;
			encoderCfg.resolution	= DSOR_1920_1080;
			break;

		case DI_VIDEO_HD_RESOLUTION_1080P:
			encoderCfg.frequency	= DSEF_60HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_1920_1080;
			break;

		case DI_VIDEO_HD_RESOLUTION_1080I_50HZ:
			encoderCfg.frequency	= DSEF_50HZ;
			encoderCfg.scanmode		= DSESM_INTERLACED;
			encoderCfg.resolution	= DSOR_1920_1080;
			break;

		case DI_VIDEO_HD_RESOLUTION_1080P_50HZ:
			encoderCfg.frequency	= DSEF_50HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_1920_1080;
			break;

		case DI_VIDEO_HD_RESOLUTION_1080P_24HZ:
			encoderCfg.frequency	= DSEF_24HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_1920_1080;
			break;

		case DI_VIDEO_HD_RESOLUTION_1080P_25HZ:
			encoderCfg.frequency	= DSEF_25HZ;
			encoderCfg.scanmode 	= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_1920_1080;
			break;

		case DI_VIDEO_HD_RESOLUTION_1080P_30HZ:
			encoderCfg.frequency	= DSEF_30HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_1920_1080;
			break;

#if defined(CONFIG_MW_OUTPUT_2160P)
		case DI_VIDEO_HD_RESOLUTION_2160P_24HZ:
			encoderCfg.frequency	= DSEF_24HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_3840_2160;
			break;

		case DI_VIDEO_HD_RESOLUTION_2160P_25HZ:
			encoderCfg.frequency	= DSEF_25HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_3840_2160;
			break;

		case DI_VIDEO_HD_RESOLUTION_2160P_30HZ:
			encoderCfg.frequency	= DSEF_30HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_3840_2160;
			break;

		case DI_VIDEO_HD_RESOLUTION_2160P_50HZ:
			encoderCfg.frequency	= DSEF_50HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_3840_2160;
			break;

		case DI_VIDEO_HD_RESOLUTION_2160P:
			encoderCfg.frequency	= DSEF_60HZ;
			encoderCfg.scanmode		= DSESM_PROGRESSIVE;
			encoderCfg.resolution	= DSOR_3840_2160;
			break;
#endif
		default:
			HxLOG_Error("\n\n\nHEEEEEEECK !!! Unsupported DI video resolution (%d) !!!\n\n\n", eRes);
			break;
	}

	DFBCHECK(_primary_screen->SetEncoderConfiguration(_primary_screen, 0, &encoderCfg));
//	_dfb->Release( _dfb );
#endif

	return ERR_OK;
}

void pal_scaler_Change3DFormat(DxVideo3DFormat_e e3DFormat)
{
#if defined(CONFIG_PACKAGE_DIRECTFB)
	DFBScreenEncoderConfig	sEncoderCfg;

	HxLOG_Debug("\t%s(0x%x)\n", __FUNCTION__, e3DFormat);

	sEncoderCfg.flags = (DFBScreenEncoderConfigFlags)DSECONF_FRAMING;

	switch(e3DFormat)
	{
		case eDxVIDEO_3DFORMAT_TOPANDBOTTOM:
			sEncoderCfg.framing = DSEPF_STEREO_TOP_AND_BOTTOM;
			break;
		case eDxVIDEO_3DFORMAT_SIDEBYSIDE:
			sEncoderCfg.framing = DSEPF_STEREO_SIDE_BY_SIDE_HALF;
			break;
		default:		// MONO
			sEncoderCfg.framing = DSEPF_MONO;
			break;
	}

	DFBCHECK(_primary_screen->SetEncoderConfiguration(_primary_screen, 0, &sEncoderCfg));
#endif
}

STATIC HERROR pal_scaler_CommitHdOutputTiming(void)
{
	DI_VIDEO_HD_RESOLUTION		eDiHdResolution;
	DI_ERR_CODE					diErr;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

/* 해상도 변경 검사 */
	eDiHdResolution = pal_scaler_ConvertResolutionPal2Di(s_stOutputStatus.stOutputTiming);

#if 0//defined(CONFIG_OP_UK_DTT)
	// Workaround FVP-4000T CR Issue : In case of 1080P 25Hz, frame rates should be fixed to 50hz. (http://svn.humaxdigital.com:3000/issues/100779)
	if(DI_VIDEO_HD_RESOLUTION_1080P_25HZ == eDiHdResolution)
	{
		eDiHdResolution = DI_VIDEO_HD_RESOLUTION_1080P_50HZ;
	}
#endif

	if(eDiHdResolution != s_eDiHdResolution)
	{
		HxLOG_Debug("\t\tChange DI HD Output Timing from(%d) to (%d) \n", s_eDiHdResolution, eDiHdResolution);

		pal_scaler_ChangeDfbVideo(eDiHdResolution);

		diErr = DI_VIDEO_SetDisplayStandard(eDiHdResolution);
		if(diErr == DI_ERR_OK)
		{
			/* Save Last HD resolution and frame rate to nvram for Loader output */
			PAL_SYS_SetResolution(s_stOutputStatus.stOutputTiming.eResolution);
			PAL_SYS_SetFrameRate(s_stOutputStatus.stOutputTiming.ePictureRate);
		}
		else
		{
			HxLOG_Error("\n\n!!! ERROR from DI_VIDEO_SetDisplayStandard()\n\n");
		}

		s_eDiHdResolution = eDiHdResolution;
	}
	else
	{
		HxLOG_Debug("\t\tSame with current.\n");
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_Commit3DFormat(void)
{
	DxVideo3DFormat_b		ul3DCapa;
	DxVideo3DFormat_e		e3DFormat;
	HUINT32					i;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

/* 3D format 설정. 3D mode로 초기화 하고 아래 조건 한가지라도 걸리면 3D 취소 */
	switch(s_stUserSetting.e3DSelect)
	{
		case eDxHDMI_3DSELECT_AUTO:
			if(s_stVideoAttribute[s_ulMainDecId].bInfoValid == TRUE)
			{
				e3DFormat = s_stVideoAttribute[s_ulMainDecId].e3DFormat;
			}
			else
			{
				e3DFormat = eDxVIDEO_3DFORMAT_NONE;
			}
			break;

		case eDxHDMI_3DSELECT_SIDEBYSIDE:
			e3DFormat = eDxVIDEO_3DFORMAT_SIDEBYSIDE;
			break;

		case eDxHDMI_3DSELECT_TOPANDBOTTOM:
			e3DFormat = eDxVIDEO_3DFORMAT_TOPANDBOTTOM;
			break;

		default:
			e3DFormat = eDxVIDEO_3DFORMAT_NONE;
			break;
	}

	if(e3DFormat != eDxVIDEO_3DFORMAT_NONE)
	{ /* 3D가 걸려야 하는 경우 추가 조사 */
		for(i = 0; i < NUM_VIEW_ACTION; i++)
		{
			if(s_stScalerStatus[i].bSubScreenMedia == TRUE)
			{
				HxLOG_Debug("\t\tCancel HDMI 3D by media rect[%d]\n", i);
				e3DFormat = eDxVIDEO_3DFORMAT_NONE;
			}
		}

		if(s_bPigInRect == TRUE)
		{
			HxLOG_Debug("\t\tCancel HDMI 3D by PIG IN-Rect\n");
			e3DFormat = eDxVIDEO_3DFORMAT_NONE;
		}

		if(s_bPigOutRect == TRUE)
		{
			HxLOG_Debug("\t\tCancel HDMI 3D by PIG OUT-Rect\n");
			e3DFormat = eDxVIDEO_3DFORMAT_NONE;
		}

		PAL_HDMI_Get3DFormatCapability(&ul3DCapa);

		if( (ul3DCapa & e3DFormat) == 0)
		{
			HxLOG_Debug("\t\tTV does not support this 3D format.\n");
			e3DFormat = eDxVIDEO_3DFORMAT_NONE;
		}
	}

	if(e3DFormat == s_stOutputStatus.e3DStatus)
	{
		HxLOG_Debug("\t\tSame as current. Exit...\n");
	}
	else
	{
		pal_scaler_Change3DFormat(e3DFormat);

		for(i = 0; i < NUM_VIEW_ACTION; i++)
		{
			DI_VIDEO_Set3DFormat(i, pal_scaler_Convert3DFormat_Pal2Di(e3DFormat));
		}

		s_stOutputStatus.e3DStatus = e3DFormat;
	}

	return ERR_OK;
}

STATIC HERROR pal_scaler_CommitWideSignal(void)
{
	VIDEO_Timing_t		stVideoTiming = s_stOutputStatus.stOutputTiming;
	HERROR				err = ERR_OK;
	DxAspectRatio_e	eHdmiAspectRatio;
	VIDEO_Afd_t			eHdmiAfd;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

	stVideoTiming.ePictureRate = eDxPICTURERATE_UNKNOWN;

	err = PAL_HDMI_CheckVideoTimingAvailable(stVideoTiming);
	if(err == ERR_OK)
	{
		eHdmiAspectRatio = s_stOutputStatus.stOutputTiming.eAspectRatio;

		if(s_stUserSetting.eResolutionSelect == eDxRESOLUTION_SELECT_EDID_MAX)
		{
			eHdmiAfd = eAfd_Full;
		}
		else
		{
			eHdmiAfd = s_stOutputStatus.eOutputAfd;
		}
	}
	else
	{
		eHdmiAspectRatio = eDxASPECTRATIO_UNKNOWN;
		eHdmiAfd = eAfd_None;
	}

	PAL_HDMI_SetAviInfoFrame(eHdmiAspectRatio, eHdmiAfd);
	PAL_OUT_SetVideoBlankAspectRatio(s_stOutputStatus.eOutputWss);

#if defined(CONFIG_MW_OUTPUT_SCART)
	PAL_SCART_SetAspecRatio(s_stOutputStatus.stOutputTiming.eAspectRatio);
#endif

	return ERR_OK;
}

STATIC void pal_scaler_LoadMediaRect(HUINT32 ulDecId)
{
	palScaler_Status_t		*pstScalerStatus = &s_stScalerStatus[ulDecId];
	palScaler_MediaRect_t			*pstMediaRect = &s_stMediaRect[pstScalerStatus->ulWindowId];
	HBOOL				bSubScreenMedia;
	DxRect_t			stFullScreenRect = {0,};

	HxLOG_Debug("\t%s(%d, window DI:%d)\n", __FUNCTION__, ulDecId, pstScalerStatus->ulWindowId);

	if( (pstMediaRect->ulRefW == 0) || (pstMediaRect->ulRefH == 0) )
	{ /* Ref. W/H 가 하나라도 0이면 full */
		bSubScreenMedia = FALSE;
	}
	else if( (pstMediaRect->stRect.nW == 0) || (pstMediaRect->stRect.nW == 0) )
	{
		bSubScreenMedia = FALSE;
	}
	else if( (pstMediaRect->stRect.nW == pstMediaRect->ulRefW)
		&& (pstMediaRect->stRect.nH == pstMediaRect->ulRefH)
	)
	{ /* Media rect 설정되어 있지 않음. */
		bSubScreenMedia = FALSE;
	}
	else
	{
		bSubScreenMedia = TRUE;
	}

	pal_scaler_GetOutputResSize(s_stOutputStatus.stOutputTiming.eResolution, &stFullScreenRect.nW, &stFullScreenRect.nH);

	if(bSubScreenMedia == TRUE)
	{ /* Sub screen 이면 scale 적용 */
		pal_scaler_GetScaledRect(pstMediaRect->stRect, pstMediaRect->ulRefW, pstMediaRect->ulRefH, stFullScreenRect, &pstScalerStatus->stScreenMediaRect);
	}
	else
	{ /* Sub screen 이 아니면 full screen */
		pstScalerStatus->stScreenMediaRect = stFullScreenRect;
	}

	pstScalerStatus->bSubScreenMedia = bSubScreenMedia;
}

STATIC void pal_scaler_FindMediaRect(VIDEO_Wss_t *peOutputWss, VIDEO_Afd_t *peOutputAfd)
{
	HERROR			err;
	DxRect_t		stOrgMainMediaRect = {0,}, *pstMainMediaRect=NULL, *pstSubMediaRect=NULL;
	HUINT32			i;

	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{ /* Window(Action) 정보에서 해당 decoder id의 scaler 로 load */
		pal_scaler_LoadMediaRect(i);
	}

	*peOutputWss = eWss_Unknown;
	*peOutputAfd = eAfd_None;

	if(s_stScalerStatus[s_ulMainDecId].bSubScreenMedia == TRUE)
	{
		if(s_stOutputStatus.stOutputTiming.eAspectRatio == eDxASPECTRATIO_4X3)
		{
			*peOutputWss = eWss_43_Full;
		}
		else
		{
			*peOutputWss = eWss_169_Full;
		}

		*peOutputAfd = eAfd_Full;

		HxLOG_Debug("\t\tDecide WSS[%s], AFD[%s] by main video's sub screen media rect.\n"
					, OTL_ENUM2STR_Wss(*peOutputWss)
					, OTL_ENUM2STR_Afd(*peOutputAfd)
					);

		s_stOutputStatus.eScreenOsdShrink = eDxDFC_STATUS_FULL;
	}
	else
	{ /* Main decoder media rect가 full screen. OSD aspect ratio 적용 */
		err = pal_scaler_FindScreenOsdShrink(peOutputWss, peOutputAfd);
		if(err == ERR_OK)
		{ /* OSD aspect ratio 지정됨. OSD wss, afd 반영해야 하고 media rect 도 scale 해야 함. */
			for(i = 0; i < NUM_VIEW_ACTION; i++)
			{
				if(i == s_ulMainDecId)
				{
					stOrgMainMediaRect = s_stScalerStatus[i].stScreenMediaRect;
					pstMainMediaRect = &s_stScalerStatus[i].stScreenMediaRect;
				}
				else
				{
					pstSubMediaRect = &s_stScalerStatus[i].stScreenMediaRect;
				}
			}

			if(pstMainMediaRect !=NULL || pstSubMediaRect !=NULL)
			{
				pal_scaler_GetShrinkedRect(s_stOutputStatus.eScreenOsdShrink, stOrgMainMediaRect, pstMainMediaRect,pstSubMediaRect);
			}
			HxLOG_Debug("\t\tDecide WSS[%s], AFD[%s] by sub screen OSD rect.\n"
						, OTL_ENUM2STR_Wss(*peOutputWss)
						, OTL_ENUM2STR_Afd(*peOutputAfd)
						);
		}
  	}
}

STATIC void pal_scaler_GetSizeByResoluntion(DxResolutionSelect_e eResolutionSelect, HUINT32 *pulOsdWidth, HUINT32 *pulOsdHeight)
{
	HUINT32	ulOutputWidth, ulOutputHeight;

	switch(eResolutionSelect)
	{
	case	eDxRESOLUTION_SELECT_480I:
	case	eDxRESOLUTION_SELECT_480P:
		ulOutputWidth = 640;
		ulOutputHeight = 480;
		break;
	case	eDxRESOLUTION_SELECT_576I:
	case	eDxRESOLUTION_SELECT_576P:
		ulOutputWidth = 720;
		ulOutputHeight = 576;
		break;
	case	eDxRESOLUTION_SELECT_720P:
		ulOutputWidth = 1280;
		ulOutputHeight = 720;
		break;
	case	eDxRESOLUTION_SELECT_1080I:
	case	eDxRESOLUTION_SELECT_1080P:
		ulOutputWidth = 1920;
		ulOutputHeight = 1080;
		break;
	case	eDxRESOLUTION_SELECT_2160P:
		ulOutputWidth = 3840 ;
		ulOutputHeight = 2160;
		break;
	default:
		ulOutputWidth = 0;
		ulOutputHeight = 0;
		break;
	}

	*pulOsdWidth = ulOutputWidth;
	*pulOsdHeight = ulOutputHeight;

	return;

}

STATIC void pal_scaler_ChangeVideoOutputByVideoFrame(void)
{
	HUINT32	ulOsdWidth, ulOsdHeight;
	HUINT32 	ulOutputWidth, ulOutputHeight;
	HUINT32	pigW = 0, pigH = 0, pigX = 0, pigY = 0;
	HBOOL	bFastCalc = FALSE;
	HFLOAT32 fVideoAspectRatio, fPigAspectRatio, fDiff;
	DxRect_t	stVideoInRect;

//	위ㅇ에서 알려주는 맥스 (osd)
#if 0
	pal_scaler_GetSizeByResoluntion(eDxRESOLUTION_SELECT_720P, &ulOsdWidth, &ulOsdHeight);
	if ((0 == ulOsdWidth) || (0 == ulOsdHeight))
	{
		goto END_FUNC;
	}
#endif
	ulOsdWidth = s_stOsdLayerSetting[0].stPigRect.ulOutRefWidth;
	ulOsdHeight = s_stOsdLayerSetting[0].stPigRect.ulOutRefHeight;

//	시스템이 제공하는 Max (output)
	pal_scaler_GetSizeByResoluntion(s_stUserSetting.eResolutionSelect, &ulOutputWidth, &ulOutputHeight);
	if ((0 == ulOutputWidth) || (0 == ulOutputHeight))
	{
		goto END_FUNC;
	}

	HxLOG_Debug("ulOutputWidth(%d) ulOutputHeight(%d) ulOsdWidth(%d) ulOsdHeight(%d) \n", ulOutputWidth , ulOutputHeight , ulOsdWidth , ulOsdHeight);

//  시스템이 제공하는 Max (output) /위ㅇ에서 알려주는 맥스 (osd)
	pigX	= 	s_stOsdLayerSetting[0].stPigRect.stOutRect.nX * ulOutputWidth / ulOsdWidth;
	pigY = 	s_stOsdLayerSetting[0].stPigRect.stOutRect.nY * ulOutputHeight / ulOsdHeight;
	pigW = 	s_stOsdLayerSetting[0].stPigRect.stOutRect.nW* ulOutputWidth / ulOsdWidth;
	pigH = 	s_stOsdLayerSetting[0].stPigRect.stOutRect.nH * ulOutputHeight / ulOsdHeight;

	HxLOG_Debug("stInRect( %d, %d, %d, %d) stOutRect( %d, %d, %d, %d) \n",
		s_stOsdLayerSetting[0].stPigRect.stInRect.nX, s_stOsdLayerSetting[0].stPigRect.stInRect.nY, s_stOsdLayerSetting[0].stPigRect.stInRect.nW, s_stOsdLayerSetting[0].stPigRect.stInRect.nH,
		s_stOsdLayerSetting[0].stPigRect.stOutRect.nX, s_stOsdLayerSetting[0].stPigRect.stOutRect.nY, s_stOsdLayerSetting[0].stPigRect.stOutRect.nW, s_stOsdLayerSetting[0].stPigRect.stOutRect.nH);
	HxLOG_Debug("PigX(%d) PigY(%d) PigW(%d) PigH(%d) \n",pigX , pigY, pigW , pigH );

	if ((0 == pigW) || (0 == pigH))
	{
		goto END_FUNC;
	}

	if(pigW == ulOutputWidth && pigH == ulOutputHeight)
	{
		fVideoAspectRatio = (0 != s_stVideoAttribute[s_ulMainDecId].ulFrameVerticalSize) ? ((float)s_stVideoAttribute[s_ulMainDecId].ulFrameHorizontalSize / (float)s_stVideoAttribute[s_ulMainDecId].ulFrameVerticalSize) : 0.0;

		switch(s_stUserSetting.eTvAspectRatio)
		{
		case eDxASPECTRATIO_4X3:
			switch(s_stVideoAttribute[s_ulMainDecId].eFrameAspectRatio)
			{
			case eDxASPECTRATIO_4X3:
				bFastCalc = TRUE;
				break;

			case eDxASPECTRATIO_14X9:
				bFastCalc = TRUE;
				pigY += (pigH / 14);
				pigH  = (pigH * 6) / 7;
				break;

			case eDxASPECTRATIO_16X9:
				bFastCalc = TRUE;
				if ( fVideoAspectRatio >= 2.2 && fVideoAspectRatio < 2.5 )			// 16:9라고 오면서 실은 21:9로 오는 스트림....
				{
					pigY += (pigH * 3 / 14);
					pigH  = (pigH * 4) / 7;
				}
				else
				{
					pigY += (pigH >> 3);
					pigH  = (pigH * 3) >> 2;
				}
				break;

			default:
				break;
			}
			break;
		case eDxASPECTRATIO_16X9:
			switch (s_stVideoAttribute[s_ulMainDecId].eFrameAspectRatio)
			{
			case eDxASPECTRATIO_16X9:
				bFastCalc = TRUE;
				if (fVideoAspectRatio >= 2.2 && fVideoAspectRatio < 2.5)			// 16:9라고 오면서 실은 21:9로 오는 스트림....
				{
					pigY = (pigH * 5 / 42);
					pigH = (pigH * 16) / 21;
				}
				break;

			case eDxASPECTRATIO_14X9:
				bFastCalc = TRUE;
				pigX = pigW >> 4;
				pigW = (pigW * 7) >> 3;
				break;

			case eDxASPECTRATIO_4X3:
				bFastCalc = TRUE;
				pigX = pigW >> 3;
				pigW = (pigW * 3) >> 2;
				break;

			default:
				break;
			}
			break;
		default:
			break;
		}

		if (TRUE == bFastCalc)
		{
			goto END_FUNC;
		}
	}

	HxLOG_Debug("VideoFrameAspectRatio (%s) \n", OTL_ENUM2STR_AspectRatio(s_stVideoAttribute[s_ulMainDecId].eFrameAspectRatio));

	switch (s_stVideoAttribute[s_ulMainDecId].eFrameAspectRatio)
	{
	case eDxASPECTRATIO_16X9:
		fVideoAspectRatio = (float)(16.0 / 9.0);
		break;

	case eDxASPECTRATIO_4X3:
		fVideoAspectRatio = (float)(4.0 / 3.0);
		break;

	case eDxASPECTRATIO_14X9:
		fVideoAspectRatio = (float)(14.0 / 9.0);
		break;

	default:
		if ((0 == s_stVideoAttribute[s_ulMainDecId].ulFrameHorizontalSize) || (0 == s_stVideoAttribute[s_ulMainDecId].ulFrameVerticalSize))
		{
			goto END_FUNC;
		}

		fVideoAspectRatio =((float)s_stVideoAttribute[s_ulMainDecId].ulFrameHorizontalSize / (float)s_stVideoAttribute[s_ulMainDecId].ulFrameVerticalSize);
		break;
	}

	HxLOG_Debug("UserSettingAspectRatio ( %s ) \n",OTL_ENUM2STR_AspectRatio(s_stUserSetting.eTvAspectRatio));

	switch (s_stUserSetting.eTvAspectRatio)			// TV Ratio
	{
	case	eDxASPECTRATIO_4X3: 		fPigAspectRatio = ((float)4.0 / (float)3.0); 			break;
	case	eDxASPECTRATIO_16X9:		fPigAspectRatio = ((float)16.0 / (float)9.0);			break;
	default:						fPigAspectRatio = ((float)16.0 / (float)9.0);			break;			// 세팅이 없을 경우엔 16:9 TV로 간주....
	}

	fPigAspectRatio *= (((float)pigW / (float)pigH) * ((float)ulOsdHeight / (float)ulOsdWidth));

	HxLOG_Debug("fPigAspectRatio (%f) fVideoAspectRatio(%f) \n",fPigAspectRatio, fVideoAspectRatio );

	// 2-3. PIG A/R과 Video A/R의 대소 차이에 따라 조정하는 값과 방식이 달라진다.
	if ( fPigAspectRatio >= fVideoAspectRatio )
	{
		// 2-3-1. PIG A/R이 Video A/R 보다 크다 : (ex: PIG는 16/9, Video는 4/3)
		// 이 경우는 PIG_Y, PIG_H는 처음 잡은 사이즈와 동일하고, X와 W만 조정되어야 한다.
		fDiff = (fPigAspectRatio - fVideoAspectRatio) / fPigAspectRatio;

		pigX += (HUINT32)((float)pigW * fDiff / 2.0);		// X는 기존 위치에서 이동해야 하므로 +가 되어야 한다.
		pigW  = (HUINT32)((float)pigW * (1.0 - fDiff));
	}
	else
	{
		// 2-3-2. PIG A/R이 Video A/R 보다 작다 : (ex: PIG는 16/9, Video는 21/9)
		// 이 경우는 PIG_X, PIG_W는 처음 잡은 사이즈와 동일하고, Y와 H만 조정되어야 한다.
		fPigAspectRatio = (float)1.0 / fPigAspectRatio;				// A/R는 W/H 인데, 여기서는 같은 Width에 Height가 변하는 것이라 H/W 로 고치기 위해 1/AR을 만든다.
		fVideoAspectRatio = (float)1.0 / fVideoAspectRatio;
		fDiff = (fPigAspectRatio - fVideoAspectRatio) / fPigAspectRatio;
		pigY += (HUINT32)((float)pigH * fDiff / 2.0);		// Y는 기존 위치에서 이동해야 하므로 +가 되어야 한다.
		pigH = (HUINT32)((float)pigH * (1.0 - fDiff));
	}

END_FUNC:

	// caculator InRect
	stVideoInRect.nX = 0;
	stVideoInRect.nY = 0;
	stVideoInRect.nW = s_stVideoAttribute[s_ulMainDecId].ulFrameHorizontalSize;
	stVideoInRect.nH = s_stVideoAttribute[s_ulMainDecId].ulFrameVerticalSize;

	pal_scaler_GetScaledRect(s_stOsdLayerSetting[0].stPigRect.stInRect,
							s_stOsdLayerSetting[0].stPigRect.ulInRefWidth,
							s_stOsdLayerSetting[0].stPigRect.ulInRefHeight,
							stVideoInRect,
							&(s_stScalerStatus[s_ulMainDecId].stVideoInRect));

	s_stScalerStatus[s_ulMainDecId].stVideoOutRect.nX = pigX;
	s_stScalerStatus[s_ulMainDecId].stVideoOutRect.nY = pigY;
	s_stScalerStatus[s_ulMainDecId].stVideoOutRect.nW = pigW;
	s_stScalerStatus[s_ulMainDecId].stVideoOutRect.nH = pigH;

	pal_scaler_CommitVideoRect(s_ulMainDecId);
}

STATIC void pal_scaler_ChangeVideoOutput(void)
{
	HUINT32					i;
	VIDEO_Wss_t				eMediaRectWss, eVideoWss;
	VIDEO_Afd_t				eMediaRectAfd, eVideoAfd;
	HERROR					err;

	HxLOG_Debug("\t%s()\n", __FUNCTION__);

/* 1. OSD setting 계산 */
	pal_scaler_FindOsdSetting();

/* 2. 비디오 출력 타이밍 계산 */
	err = pal_scaler_FindHdmiTiming();
	if(err != ERR_OK)
	{ /* HDMI plug-out 에서는 TV aspect ratio 만 반영 */
		s_stOutputStatus.stOutputTiming.eAspectRatio = s_stUserSetting.eTvAspectRatio;
	}

/* 3. Media / OSD rect 계산 */
	pal_scaler_FindMediaRect(&eMediaRectWss, &eMediaRectAfd);

/* 4. PIG, DFC, OSD aspect ratio 적용한 main video scaling 계산 */
	pal_scaler_FindMainVideoScaling(&eVideoWss, &eVideoAfd);

/* 5. Sub video scaling 계산. */
	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		if(i != s_ulMainDecId)
		{
			pal_scaler_FindSubVideoScaling(i);
		}
	}

/* 6. 최종 WSS/AFD 선택 */
	if(eMediaRectWss != eWss_Unknown)
	{
		HxLOG_Debug("\t\tFinal WSS/AFD is by Media Rect / OSD AspectRatio\n");
		s_stOutputStatus.eOutputWss = eMediaRectWss;
		s_stOutputStatus.eOutputAfd = eMediaRectAfd;
	}
	else if(eVideoWss != eWss_Unknown)
	{
		HxLOG_Debug("\t\tFinal WSS/AFD is by Video / PIG\n");
		s_stOutputStatus.eOutputWss = eVideoWss;
		s_stOutputStatus.eOutputAfd = eVideoAfd;
	}

/* 7. 계산 결과를 DI에 반영. 아래 commit 순서 지킬것. */
	pal_scaler_CommitHdOutputTiming();
	pal_scaler_Commit3DFormat();
	pal_scaler_CommitVideoRect(s_ulMainDecId);
	pal_scaler_CommitMainMediaRect();

	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		if(i != s_ulMainDecId)
		{
			pal_scaler_CommitVideoRect(i);
			pal_scaler_CommitSubMediaRect(i);
		}
	}

	pal_scaler_CommitOsdRect();
	pal_scaler_CommitWideSignal();
}

STATIC void pal_scaler_Invalidate(void)
{
	HUINT32		i;

	/* DI 파라미터는 차이가 있는 경우에만 호출하기 때문에,
		맨 처음에는 무조건 호출하기 위해 -1과 같은 이상한 값으로 초기화 한다. */
	s_eDiHdResolution = -1;

	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		s_stMediaRect[i].stRect.nX = 0;
		s_stMediaRect[i].stRect.nY = 0;
		s_stMediaRect[i].stRect.nW = 0;
		s_stMediaRect[i].stRect.nH = 0;
		s_stMediaRect[i].ulRefW = 0;
		s_stMediaRect[i].ulRefH = 0;

		s_stScalerStatus[i].ulWindowId = i;

		s_stScalerStatus[i].stDiVideoInRect.ulX = -1;
		s_stScalerStatus[i].stDiVideoOutRect.ulX = -1;
	}
}

#if defined(CONFIG_PACKAGE_DIRECTFB)
STATIC void pal_scaler_EnableCompositorOSD(void)
{
	DFBDisplayLayerConfig	layerCfg;
	IDirectFBSurface		*secondary;

	if(secondary_layer == NULL)
	{
		DFBCHECK(_dfb->GetDisplayLayer(_dfb, DLID_PRIMARY + 1, &secondary_layer));
		DFBCHECK(_dfb->GetScreen(_dfb, DSCID_PRIMARY, &_primary_screen));

		if(NULL == _primary_screen)
		{	// for debugging
			HxLOG_Error(HxANSI_COLOR_CYAN("_primary_screen is NULL !!!!!!!!!!!\n"));
			HxLOG_Error(HxANSI_COLOR_CYAN("_primary_screen is NULL !!!!!!!!!!!\n"));
			HxLOG_Error(HxANSI_COLOR_CYAN("_primary_screen is NULL !!!!!!!!!!!\n"));
			HxLOG_Error(HxANSI_COLOR_CYAN("_primary_screen is NULL !!!!!!!!!!!\n"));
			HxLOG_Error(HxANSI_COLOR_CYAN("_primary_screen is NULL !!!!!!!!!!!\n"));
		}

		DFBCHECK(secondary_layer->SetCooperativeLevel(secondary_layer, DLSCL_ADMINISTRATIVE));
		layerCfg.flags = DLCONF_SOURCE|DLCONF_WIDTH|DLCONF_HEIGHT;
		layerCfg.source = DLSID_SURFACE;

		layerCfg.width = 720;

		if(s_stUserSetting.eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
		{
			layerCfg.height = 480;
		}
		else
		{
			layerCfg.height = 576;
		}

		secondary_layer->SetConfiguration(secondary_layer, &layerCfg);
		secondary_layer->GetSurface(secondary_layer, &secondary);
#if defined(CONFIG_PROD_CDIG2000C) || defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	/*
		From: 최현석(Hyun Seok Choi)
		Sent: Friday, February 13, 2015 2:10 PM
		To: 이호(Tim Yi); 김종원(Jongwon Kim, SW); 최성민(Sungmin Choi)
		Cc: 지승용(Seung-Yong Chi); 강태호(Teho Kang); 박준호(Junho Park); 강희완(Heewan Kang)
		Subject: RE: CDIG-2000C compile error 건

		최현석입니다.

		Full_head로 다시 받아서 Install Wizard까지 확인을 하였습니다.
		가능하시면 full_head로 새로 받아서 작업 부탁 드립니다.

		추가로 2가지 사항을 App에 적용해야 합니다.

		1. pal_scaler
				   - cdig2000c 는 HDMI 출력만 있어서 secondary를 사용하면 안되는데 설정하는 부분을 다 막으면 directfb에서 죽는 이슈가 있습니다.
				   - debugging은 해봤지만 아직 point을 찾지 못한 상태이고 단 Flip만 막으면 정상 booting 합니다. 임시로 변경 작업 부탁 드립니다.

		2. directfbrc
				   - directfb 1.7.6을 사용하기 위해서 directfbrc에 추가 option을 넣어야 합니다.
				   - app/make/rootfs/product/cdig2000c/dbg/usr/etc/ 및 rel/usr/etc를 만드시고 넣으시면 됩니다.

	*/
#if 0  /* hs.choi */
		secondary->Flip(secondary, NULL, DSFLIP_NONE);
#endif

#else
		secondary->Flip(secondary, NULL, DSFLIP_NONE);
#endif

		DFBCHECK (secondary->Release(secondary));
	// This'll disable composite OSD		DFBCHECK (secondary_layer->Release(secondary_layer));
	}
	return;
}


STATIC void pal_scaler_DisableCompositorOSD(void)
{
	if(secondary_layer != NULL)
	{
		secondary_layer->Release( secondary_layer );
		secondary_layer = NULL;
	}
	return;
}
#endif

#if defined(CONFIG_DEBUG)
STATIC void pal_scaler_PrintStatus(void)
{
	HUINT32			i;

	HLIB_CMD_Printf("=========================== OSD Scaler Setting ============================\n");
	for(i = 0; i < NUM_MAX_OSD_PLANE; i++)
	{
		HLIB_CMD_Printf("\t---------- OSD Layer[%d] ----------\n", i);
		HLIB_CMD_Printf("\t\tOsd Output Aspect Ratio : %s\n", OTL_ENUM2STR_AspectRatio(s_stOsdLayerSetting[i].eOsdAspectRatio));
		HLIB_CMD_Printf("\t\tOSD Dfc : %s\n", OTL_ENUM2STR_DfcSelect(s_stOsdLayerSetting[i].eDfc));
		HLIB_CMD_Printf("\t\tPIG in(%d, %d, %d, %d)/(%d, %d), out(%d, %d, %d, %d)/(%d, %d)\n",
						s_stOsdLayerSetting[i].stPigRect.stInRect.nX,
						s_stOsdLayerSetting[i].stPigRect.stInRect.nY,
						s_stOsdLayerSetting[i].stPigRect.stInRect.nW,
						s_stOsdLayerSetting[i].stPigRect.stInRect.nH,
						s_stOsdLayerSetting[i].stPigRect.ulInRefWidth,
						s_stOsdLayerSetting[i].stPigRect.ulInRefHeight,
						s_stOsdLayerSetting[i].stPigRect.stOutRect.nX,
						s_stOsdLayerSetting[i].stPigRect.stOutRect.nY,
						s_stOsdLayerSetting[i].stPigRect.stOutRect.nW,
						s_stOsdLayerSetting[i].stPigRect.stOutRect.nH,
						s_stOsdLayerSetting[i].stPigRect.ulOutRefWidth,
						s_stOsdLayerSetting[i].stPigRect.ulOutRefHeight);
	}

	HLIB_CMD_Printf("\t--------------------- Final OSD Scaler Setting ---------------------\n", i);
	HLIB_CMD_Printf("\t\t\tOsd Output Aspect Ratio : %s\n", OTL_ENUM2STR_AspectRatio(s_stOsdSetting.eOsdAspectRatio));
	HLIB_CMD_Printf("\t\t\tOSD Dfc : %s\n", OTL_ENUM2STR_DfcSelect(s_stOsdSetting.eDfc));
	HLIB_CMD_Printf("\t\t\tPIG in(%d, %d, %d, %d)/(%d, %d), out(%d, %d, %d, %d)/(%d, %d)\n",
					s_stOsdSetting.stPigRect.stInRect.nX,
					s_stOsdSetting.stPigRect.stInRect.nY,
					s_stOsdSetting.stPigRect.stInRect.nW,
					s_stOsdSetting.stPigRect.stInRect.nH,
					s_stOsdSetting.stPigRect.ulInRefWidth,
					s_stOsdSetting.stPigRect.ulInRefHeight,
					s_stOsdSetting.stPigRect.stOutRect.nX,
					s_stOsdSetting.stPigRect.stOutRect.nY,
					s_stOsdSetting.stPigRect.stOutRect.nW,
					s_stOsdSetting.stPigRect.stOutRect.nH,
					s_stOsdSetting.stPigRect.ulOutRefWidth,
					s_stOsdSetting.stPigRect.ulOutRefHeight);
	HLIB_CMD_Printf("========================== User Setting ============================\n\n");

	HLIB_CMD_Printf("\tComposite : %s\n", OTL_ENUM2STR_CompositeStandard(s_stUserSetting.eCompositeStandard));
	HLIB_CMD_Printf("\tTV Aspect Ratio : %s\n", OTL_ENUM2STR_AspectRatio(s_stUserSetting.eTvAspectRatio));
	HLIB_CMD_Printf("\tDFC : %s\n", OTL_ENUM2STR_DfcSelect(s_stUserSetting.eDfc));
	HLIB_CMD_Printf("\tVideo Resolution Select : %s\n", OTL_ENUM2STR_ResolutionSelect(s_stUserSetting.eResolutionSelect));
	HLIB_CMD_Printf("\tEDID Aspect Ratio : %d\n", s_stUserSetting.bUseEdidAspectRatio);
	HLIB_CMD_Printf("\tWide Signal Letterbox : %d\n", s_stUserSetting.bWideSignalLetterbox);

	HLIB_CMD_Printf("========================== Video Output Status ============================\n\n");

	HLIB_CMD_Printf("\tVideo Output Timing : %s, %s, %s\n",
						OTL_ENUM2STR_ResolutionStatus(s_stOutputStatus.stOutputTiming.eResolution),
						OTL_ENUM2STR_AspectRatio(s_stOutputStatus.stOutputTiming.eAspectRatio),
						OTL_ENUM2STR_VideoPictureRate(s_stOutputStatus.stOutputTiming.ePictureRate));
	HLIB_CMD_Printf("\tAFD : %s\n", OTL_ENUM2STR_Afd(s_stOutputStatus.eOutputAfd));
	HLIB_CMD_Printf("\tWSS : %s\n", OTL_ENUM2STR_Wss(s_stOutputStatus.eOutputWss));
	pal_scaler_PrintDfcStatus(s_stOutputStatus.eScreenOsdShrink);

	for(i = 0; i < NUM_VIEW_ACTION; i++)
	{
		pal_scaler_PrintVideoAttribute(i);

		HLIB_CMD_Printf("========================== Window Media Rect [%d] ============================\n\n", i);
		HLIB_CMD_Printf("\tMedia Rect : (%d, %d, %d, %d) / (%d, %d)\n"
							, s_stMediaRect[i].stRect.nX
							, s_stMediaRect[i].stRect.nY
							, s_stMediaRect[i].stRect.nW
							, s_stMediaRect[i].stRect.nH
							, s_stMediaRect[i].ulRefW
							, s_stMediaRect[i].ulRefW
							);

		HLIB_CMD_Printf("========================== Scaler Status [%d] ============================\n\n", i);
		HLIB_CMD_Printf("\tWindow ID : %d\n", s_stScalerStatus[i].ulWindowId);

		HLIB_CMD_Printf("\tVideo In Rect : (%d, %d, %d, %d)\n",
							s_stScalerStatus[i].stVideoInRect.nX,
							s_stScalerStatus[i].stVideoInRect.nY,
							s_stScalerStatus[i].stVideoInRect.nW,
							s_stScalerStatus[i].stVideoInRect.nH);
		HLIB_CMD_Printf("\tVideo Out Rect : (%d, %d, %d, %d)\n",
							s_stScalerStatus[i].stVideoOutRect.nX,
							s_stScalerStatus[i].stVideoOutRect.nY,
							s_stScalerStatus[i].stVideoOutRect.nW,
							s_stScalerStatus[i].stVideoOutRect.nH);
		HLIB_CMD_Printf("=================================================================\n");
	}

}

STATIC int pal_scaler_ProcessCommand_status(void *szArgList)
{
	pal_scaler_PrintStatus();

	return HxCMD_OK;
}

STATIC int pal_scaler_ProcessCommand_mhegpoint(void *szArgList)
{
	char			*aucArg=NULL;
	int				refW, refH, x, y, px, py;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &x) != 0 )
		{
			aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( aucArg != NULL )
			{
				if( sscanf(aucArg, "%d", &y) != 0 )
				{
					aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
					if( aucArg != NULL )
					{
						if( sscanf(aucArg, "%d", &refW) != 0 )
						{
							aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
							if( aucArg != NULL )
							{
								if( sscanf(aucArg, "%d", &refH) != 0 )
								{
									HxLOG_Print("\n\n\t\t ===== MHEG Display Point (%d, %d)/(%d, %d)\n\n", x, y, refW, refH);
									PAL_SCALER_GetOsdDisplayPoint(refW, refH, x, y, &px, &py);
									HxLOG_Print("\n\n\t\t ===== Actual Output Point (%d, %d)\n\n", px, py);
								}
							}
						}
					}
				}
			}
		}
	}

	return HxCMD_OK;
}

STATIC int pal_scaler_ProcessCommand_pig(void *szArgList)
{
	char			*aucArg=NULL;
	int 			x, y, w, h, refW, refH;
	PIG_Rect_t		stPigRect;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &x) != 0 )
		{
			aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( aucArg != NULL )
			{
				if( sscanf(aucArg, "%d", &y) != 0 )
				{
					aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
					if( aucArg != NULL )
					{
						if( sscanf(aucArg, "%d", &w) != 0 )
						{
							aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
							if( aucArg != NULL )
							{
								if( sscanf(aucArg, "%d", &h) != 0 )
								{
									aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
									if( aucArg != NULL )
									{
										if( sscanf(aucArg, "%d", &refW) != 0 )
										{
											aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
											if( aucArg != NULL )
											{
												if( sscanf(aucArg, "%d", &refH) != 0 )
												{
													HxLOG_Print("\n\n\t\t ===== PIG (%d, %d, %d, %d)/(%d, %d)\n\n", x, y, w, h, refW, refH);

													stPigRect.ulInRefWidth = stPigRect.ulInRefHeight = 0;

													stPigRect.stOutRect.nX = x;
													stPigRect.stOutRect.nY = y;
													stPigRect.stOutRect.nW = w;
													stPigRect.stOutRect.nH = h;
													stPigRect.ulOutRefWidth = refW;
													stPigRect.ulOutRefHeight = refH;
													stPigRect.bRoundOff = FALSE; /* only for Netflix */
													stPigRect.bForceSourceBase = FALSE;

													PAL_SCALER_SetOsdLayerOutput(eOsdZOrder_Ui, eDxASPECTRATIO_UNKNOWN, eDxDFC_SELECT_UNKNOWN, stPigRect, eDxASPECTRATIO_UNKNOWN);
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
		}
	}

	return HxCMD_OK;
}

STATIC int pal_scaler_ProcessCommand_bgcolor(void *szArgList)
{
	char			*aucArg=NULL;
	HUINT32			r, g, b;

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%d", &r) != 0 )
		{
			aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
			if( aucArg != NULL )
			{
				if( sscanf(aucArg, "%d", &g) != 0 )
				{
					aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
					if( aucArg != NULL )
					{
						if( sscanf(aucArg, "%d", &b) != 0 )
						{
							PAL_SCALER_SetBackgroundColor(r, g, b);
						}
						else
						{
							return HxCMD_ERR;
						}
					}
					else
					{
						return HxCMD_ERR;
					}
				}
				else
				{
					return HxCMD_ERR;
				}
			}
			else
			{
				return HxCMD_ERR;
			}

		}
		else
		{
			return HxCMD_ERR;
		}
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

STATIC int pal_scaler_ProcessCommand_3dmode(void *szArgList)
{
	char			 *aucArg=NULL;
	char			 aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%s", aucStr) != 0 )
		{
			if(strcmp(aucStr, "off") == 0)
			{
				PAL_SCALER_SetHdmi3DSelect(eDxHDMI_3DSELECT_OFF);
			}
			else if(strcmp(aucStr, "sidebyside") == 0)
			{
				PAL_SCALER_SetHdmi3DSelect(eDxHDMI_3DSELECT_SIDEBYSIDE);
			}
			else if(strcmp(aucStr, "topbottom") == 0)
			{
				PAL_SCALER_SetHdmi3DSelect(eDxHDMI_3DSELECT_TOPANDBOTTOM);
			}
			else if(strcmp(aucStr, "auto") == 0)
			{
				PAL_SCALER_SetHdmi3DSelect(eDxHDMI_3DSELECT_AUTO);
			}
			else
			{
				return HxCMD_ERR;
			}
		}
		else
		{
			return HxCMD_ERR;
		}
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

STATIC int pal_scaler_ProcessCommand_ping(void *szArgList)
{
	HLIB_CMD_Printf("Ping pal_scaler API semaphore ...\n");

	palScaler_ENTER_CRITICAL;

	HLIB_CMD_Printf("\t\ts_ulPalSclSemId alive ...\n");

	palScaler_LEAVE_CRITICAL;

	return HxCMD_OK;
}

STATIC int pal_scaler_ProcessCommand_osd(void *szArgList)
{
	char	*aucArg=NULL;
	char	aucStr[20];
	HBOOL	bFlag = FALSE;
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%s", aucStr) != 0 )
		{
			if(strcmp(aucStr, "compositor") == 0)
			{
				aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
				if( sscanf(aucArg, "%s", aucStr) != 0 )
				{
					if(strcmp(aucStr, "on") == 0)
					{
						PAL_SCALER_SetCompositorOsdOutput(TRUE);
						bFlag = TRUE;
					}
					else if(strcmp(aucStr, "off") == 0)
					{
						PAL_SCALER_SetCompositorOsdOutput(FALSE);
						bFlag = TRUE;
					}
				}
			}
		}
	}

	if(bFlag == FALSE)
	{
		return HxCMD_ERR;
	}
	return HxCMD_OK;
}

#define	hCmdHandle		"pal_scaler"

STATIC void pal_scaler_RegisterCommand(void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_scaler_ProcessCommand_status,
						"status",
						"Print video output scaling status.",
						"status");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_scaler_ProcessCommand_mhegpoint,
						"mhegpoint",
						"Caculate & show the actual screen position from MHEG point",
						"mhegpoint [x] [y] [reference width] [reference height]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_scaler_ProcessCommand_pig,
						"pig",
						"Set PIG rect.",
						"pig [x] [y] [w] [h] [reference width] [reference height]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_scaler_ProcessCommand_bgcolor,
						"bgcolor",
						"Set video back ground color.",
						"bgcolor [red] [green] [blue]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_scaler_ProcessCommand_3dmode,
						"3dmode",
						"Set HDMI 3D signal.",
						"3dmode [off|sidebyside|topbottom|auto]");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_scaler_ProcessCommand_ping,
						"ping",
						"Probe if semaphore is alive.",
						"ping");

	HLIB_CMD_RegisterWord(hCmdHandle,
						pal_scaler_ProcessCommand_osd,
						"osd",
						"osd compositor on/off",
						"osd compositor [on/off]");
}
#endif

/********************************************************************
 Global Function
 ********************************************************************/

void PAL_SCALER_Init(void)
{
	HUINT32			err;

	err = VK_SEM_Create(&s_ulPalSclSemId, "PalScl", VK_SUSPENDTYPE_PRIORITY);
	HxLOG_Assert(err == ERR_OK);

	pal_scaler_Invalidate();

#if defined(CONFIG_DEBUG)
	pal_scaler_RegisterCommand();
#endif

	return;
}

void PAL_SCALER_Invalidate(void)
{
	palScaler_ENTER_CRITICAL;

	pal_scaler_Invalidate();

	palScaler_LEAVE_CRITICAL;
}

#ifdef CONFIG_DEBUG

STATIC HCHAR* pal_scaler_GetEnumStr_DxCompositeStandard_e(DxCompositeStandard_e eValue)
{
	switch(eValue)
	{
	ENUM_TO_STR(eDxCOMPOSITE_STANDARD_UNKNOWN);
	ENUM_TO_STR(eDxCOMPOSITE_STANDARD_PAL);
	ENUM_TO_STR(eDxCOMPOSITE_STANDARD_PAL_M);
	ENUM_TO_STR(eDxCOMPOSITE_STANDARD_PAL_N);
	ENUM_TO_STR(eDxCOMPOSITE_STANDARD_NTSC);
	ENUM_TO_STR(eDxCOMPOSITE_STANDARD_SECAM);
	ENUM_TO_STR(eDxCOMPOSITE_STANDARD_MAX_NUM);
	default:
		return "??";
	};
}

STATIC HCHAR* pal_scaler_GetEnumStr_DxAspectRatio_e(DxAspectRatio_e eValue)
{
	switch(eValue)
	{
	ENUM_TO_STR(eDxASPECTRATIO_UNKNOWN);
	ENUM_TO_STR(eDxASPECTRATIO_4X3);
	ENUM_TO_STR(eDxASPECTRATIO_16X9);
	ENUM_TO_STR(eDxASPECTRATIO_14X9);
	ENUM_TO_STR(eDxASPECTRATIO_1X1);
	ENUM_TO_STR(eDxASPECTRATIO_MAX_NUM);
	default:
		return "??";
	};
}

STATIC HCHAR* pal_scaler_GetEnumStr_DxDfcSelect_e(DxDfcSelect_e eValue)
{
	switch(eValue)
	{
	ENUM_TO_STR(eDxDFC_SELECT_UNKNOWN);
/* 방송 사양에서 정의한 방법에 의해 아래 자동 모드 중 하나로 치환될 것임. 사양별로 config 필요. */
	ENUM_TO_STR(eDxDFC_SELECT_DEFAULT);
/* Aspect ratio 파라미터 간의 계산에 의한 자동 조정 모드 */
	ENUM_TO_STR(eDxDFC_SELECT_WIDESIGNAL);			/**< scaling 없이 WSS나 AFD 등의 wide signal을 사용한다. */
	ENUM_TO_STR(eDxDFC_SELECT_SQUEEZE);				/**< 16:9 소스는 4:3 출력에 4:3 Letter box); 4:3소스는 16:9출력에 16:9 pillar box */
	ENUM_TO_STR(eDxDFC_SELECT_SQUEEZE149);			/**< 16:9 소스는 4:3 출력에 14:9 Letter box); 4:3소스는 16:9출력에 14:9 pillar box */
	ENUM_TO_STR(eDxDFC_SELECT_CENTERCUTOUT); 		/**< 16:9 소스는 4:3 출력에 4:3 side cut); 4:3소스는 16:9출력에 16:9 zoom */
	ENUM_TO_STR(eDxDFC_SELECT_CENTERCUTOUT149); 		/**< 16:9 소스는 4:3 출력에 14:9 side cut); 4:3소스는 16:9출력에 14:9 zoom */
/* DFC 직접 지정에 의한 강제 조정 모드 */
	ENUM_TO_STR(eDxDFC_SELECT_FULLSCREEN); 			/**< 출력 aspect ratio에 full stretch. */
	ENUM_TO_STR(eDxDFC_SELECT_LETTERBOX);
	ENUM_TO_STR(eDxDFC_SELECT_LETTERBOX149);
	ENUM_TO_STR(eDxDFC_SELECT_PILLARBOX);
	ENUM_TO_STR(eDxDFC_SELECT_PILLARBOX149);
	ENUM_TO_STR(eDxDFC_SELECT_ZOOM);
	ENUM_TO_STR(eDxDFC_SELECT_ZOOM149);
	ENUM_TO_STR(eDxDFC_SELECT_SIDECUT);
	ENUM_TO_STR(eDxDFC_SELECT_SIDECUT149);
	ENUM_TO_STR(eDxDFC_SELECT_MAX_NUM);
	default:
		return "??";
	};
}

STATIC HCHAR* pal_scaler_GetEnumStr_DxResolutionSelect_e(DxResolutionSelect_e eValue)
{
	switch(eValue)
	{
	ENUM_TO_STR(eDxRESOLUTION_SELECT_UNKNOWN);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_480I);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_576I);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_480P);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_576P);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_720P);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_1080I);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_1080P);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_2160P);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_ORIGINAL);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_EDID_MAX);
	ENUM_TO_STR(eDxRESOLUTION_SELECT_MAX_NUM);
	default:
		return "??";
	};
}

STATIC HCHAR* pal_scaler_GetEnumStr_DxHdmi3DSelect_e(DxHdmi3DSelect_e eValue)
{
	switch(eValue)
	{
	ENUM_TO_STR(eDxHDMI_3DSELECT_OFF);
	ENUM_TO_STR(eDxHDMI_3DSELECT_SIDEBYSIDE);
	ENUM_TO_STR(eDxHDMI_3DSELECT_TOPANDBOTTOM);
	ENUM_TO_STR(eDxHDMI_3DSELECT_FRAMEPACKING);
	ENUM_TO_STR(eDxHDMI_3DSELECT_AUTO);
	ENUM_TO_STR(eDxHDMI_3DSELECT_MAX_NUM);
	default:
		return "??";
	};
}

#endif

void PAL_SCALER_SetInitParameters(
							DxCompositeStandard_e	eCompositeStandard,
							DxDfcSelect_e			eDfc,
							DxResolutionSelect_e	eResolutionSelect,
							DxAspectRatio_e			eTvAspectRatio,
							DxHdmi3DSelect_e		e3DSelect,
							HBOOL					bUseEdidAspectRatio,
							HBOOL					bWideSignaloLetterBox,
							HBOOL					bEnableHdmiAfd
							)
{
	s_stUserSetting.eCompositeStandard = eCompositeStandard;
	s_stUserSetting.eDfc = eDfc;
	s_stUserSetting.eResolutionSelect = eResolutionSelect;
	s_stUserSetting.eTvAspectRatio = eTvAspectRatio;
	s_stUserSetting.e3DSelect = e3DSelect;
	s_stUserSetting.bUseEdidAspectRatio = bUseEdidAspectRatio;
	s_stUserSetting.bWideSignalLetterbox = bWideSignaloLetterBox;
	s_stUserSetting.bHdmiAfd = bEnableHdmiAfd;

	s_stOutputStatus.stOutputTiming.eResolution = eDxRESOLUTION_STATUS_1080I;
	s_stOutputStatus.stOutputTiming.eAspectRatio = eTvAspectRatio;

	if(eCompositeStandard == eDxCOMPOSITE_STANDARD_NTSC)
	{
		s_stOutputStatus.stOutputTiming.ePictureRate = eDxPICTURERATE_60I;
	}
	else
	{
		s_stOutputStatus.stOutputTiming.ePictureRate = eDxPICTURERATE_50I;
	}

#ifdef CONFIG_DEBUG
	HxLOG_Print(HxANSI_COLOR_YELLOW("s_stUserSetting"
		"\n\t eCompositeStandard    = %s"
		"\n\t eDfc                  = %s"
		"\n\t eResolutionSelect     = %s"
		"\n\t eTvAspectRatio        = %s"
		"\n\t e3DSelect             = %s"
		"\n\t bUseEdidAspectRatio   = %d"
		"\n\t bWideSignaloLetterBox = %d"
		"\n\t bEnableHdmiAfd        = %d"
		)
		, pal_scaler_GetEnumStr_DxCompositeStandard_e(eCompositeStandard)
		, pal_scaler_GetEnumStr_DxDfcSelect_e(eDfc)
		, pal_scaler_GetEnumStr_DxResolutionSelect_e(eResolutionSelect)
		, pal_scaler_GetEnumStr_DxAspectRatio_e(eTvAspectRatio)
		, pal_scaler_GetEnumStr_DxHdmi3DSelect_e(e3DSelect)
		, bUseEdidAspectRatio
		, bWideSignaloLetterBox
		, bEnableHdmiAfd
		);
#endif

#if defined(CONFIG_PACKAGE_DIRECTFB)
	DFBCHECK(DirectFBInit(0, NULL));
	DFBCHECK(DirectFBCreate(&_dfb));

	//	SCART/COMPOSITE Output
	pal_scaler_EnableCompositorOSD();
#endif

	return;
}


HERROR PAL_SCALER_SetCompositorOsdOutput(HBOOL bShow)
{
	palScaler_ENTER_CRITICAL;

	if(bShow == TRUE)
	{
#if defined(CONFIG_PACKAGE_DIRECTFB)
		pal_scaler_EnableCompositorOSD();
#endif
	}
	else
	{
#if defined(CONFIG_PACKAGE_DIRECTFB)
		pal_scaler_DisableCompositorOSD();
#endif
	}


	palScaler_LEAVE_CRITICAL;
	return ERR_OK;
}

HERROR PAL_SCALER_SetOsdLayerOutput(OsdZOrder_t		ulZOrder,
							DxAspectRatio_e		eOsdAspectRatio,
							DxDfcSelect_e 			eDfc,
							PIG_Rect_t				stPigRect,
							DxAspectRatio_e		ePigAspectRatio
							)
{
	HxLOG_Debug("%s(%d, OSD:%s, %s, PIG:%s)\n", __FUNCTION__, ulZOrder,
										OTL_ENUM2STR_AspectRatio(eOsdAspectRatio),
										OTL_ENUM2STR_DfcSelect(eDfc),
										OTL_ENUM2STR_AspectRatio(ePigAspectRatio)
										);
	HxLOG_Debug("\t\tin{%d, %d, %d, %d}/{%d, %d}, out{%d, %d, %d, %d}/{%d, %d}\n",
									stPigRect.stInRect.nX,
									stPigRect.stInRect.nY,
									stPigRect.stInRect.nW,
									stPigRect.stInRect.nH,
									stPigRect.ulInRefWidth,
									stPigRect.ulInRefHeight,
									stPigRect.stOutRect.nX,
									stPigRect.stOutRect.nY,
									stPigRect.stOutRect.nW,
									stPigRect.stOutRect.nH,
									stPigRect.ulOutRefWidth,
									stPigRect.ulOutRefHeight);

	if(ulZOrder >= NUM_MAX_OSD_PLANE)
	{
		HxLOG_Error("\n\n!!! ERROR Parameter.\n\n");
		return ERR_FAIL;
	}

	palScaler_ENTER_CRITICAL;

	if( (eOsdAspectRatio != s_stOsdLayerSetting[ulZOrder].eOsdAspectRatio)
		|| (eDfc != s_stOsdLayerSetting[ulZOrder].eDfc)
		|| (HxSTD_memcmp(&stPigRect, &(s_stOsdLayerSetting[ulZOrder].stPigRect), sizeof(PIG_Rect_t)) != 0)
		|| (ePigAspectRatio != s_stOsdLayerSetting[ulZOrder].ePigAspectRatio)
	)
	{
		VideoStreamInfo_t 	stStreamInfo;

		s_stOsdLayerSetting[ulZOrder].eOsdAspectRatio = eOsdAspectRatio;
		s_stOsdLayerSetting[ulZOrder].eDfc = eDfc;
		s_stOsdLayerSetting[ulZOrder].stPigRect = stPigRect;
		s_stOsdLayerSetting[ulZOrder].ePigAspectRatio = ePigAspectRatio;

		HxSTD_MemSet(&stStreamInfo, 0x00, sizeof(VideoStreamInfo_t));

		if((s_bSupportScalerByVideoFrame == TRUE) && (PAL_VIDEO_GetStreamInfo(s_ulMainDecId, &stStreamInfo) == ERR_OK))
			pal_scaler_ChangeVideoOutputByVideoFrame();
		else
		pal_scaler_ChangeVideoOutput();
	}
	else
	{
		HxLOG_Debug("\tSame as current. Exit...\n");
	}

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_GetOsdLayerOutput(OsdZOrder_t		ulZOrder,
							DxAspectRatio_e 	*peOsdAspectRatio,
							DxDfcSelect_e 			*peDfc,
							PIG_Rect_t				*pstPigRect,
							DxAspectRatio_e 	*pePigAspectRatio
							)
{
	if(ulZOrder >= NUM_MAX_OSD_PLANE)
	{
		HxLOG_Error("\n\n!!! ERROR Parameter.\n\n");
		return ERR_FAIL;
	}

	palScaler_ENTER_CRITICAL;

	if(peOsdAspectRatio != NULL)
	{
		*peOsdAspectRatio = s_stOsdLayerSetting[ulZOrder].eOsdAspectRatio;
	}

	if(peDfc != NULL)
	{
		*peDfc = s_stOsdLayerSetting[ulZOrder].eDfc;
	}

	if(pstPigRect != NULL)
	{
		*pstPigRect = s_stOsdLayerSetting[ulZOrder].stPigRect;
	}

	if(pePigAspectRatio != NULL)
	{
		*pePigAspectRatio = s_stOsdLayerSetting[ulZOrder].ePigAspectRatio;
	}

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_SetMediaRect(HUINT32 ulWindowId, DxRect_t stMediaRect, HUINT32 ulRefW, HUINT32 ulRefH)
{
	palScaler_MediaRect_t			*pstMediaRect = &s_stMediaRect[ulWindowId];

	if(ulWindowId >= NUM_VIEW_ACTION)
	{
		HxLOG_Error("%s(Invalid Action ID[%d]\n", __FUNCTION__, ulWindowId);

		return ERR_FAIL;
	}

	palScaler_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d) (%d, %d, %d, %d)/(%d, %d)\n"
				, __FUNCTION__
				, ulWindowId
				, stMediaRect.nX
				, stMediaRect.nY
				, stMediaRect.nW
				, stMediaRect.nH
				, ulRefW
				, ulRefH
			);

	if( (HxSTD_memcmp(&stMediaRect, &pstMediaRect->stRect, sizeof(DxRect_t)) != 0)
		|| (ulRefW != pstMediaRect->ulRefW)
		|| (ulRefH != pstMediaRect->ulRefH)
	)
	{ /* 현재 rect와 다르면 반영 */
		pstMediaRect->stRect = stMediaRect;

		pstMediaRect->ulRefW = ulRefW;
		pstMediaRect->ulRefH = ulRefH;

		pal_scaler_ChangeVideoOutput();
	}
	else
	{
		HxLOG_Debug("\tSame as current. Exit...\n");
	}

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_GetMediaRect(HUINT32 ulWindowId, DxRect_t *pstMediaRect, HUINT32 *pulRefW, HUINT32 *pulRefH)
{
	if(ulWindowId >= NUM_VIEW_ACTION)
	{
		HxLOG_Error("%s(Invalid media rect ID[%d]\n", __FUNCTION__, ulWindowId);

		return ERR_FAIL;
	}

	palScaler_ENTER_CRITICAL;

	*pstMediaRect = s_stMediaRect[ulWindowId].stRect;
	*pulRefW = s_stMediaRect[ulWindowId].ulRefW;
	*pulRefH = s_stMediaRect[ulWindowId].ulRefH;

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_SetVideoUsage(HUINT32 ulDecId, PalScaler_VideoUsage_e eVideoUsage)
{
	palScaler_ENTER_CRITICAL;

	s_stVideoAttribute[ulDecId].eVideoUsage = eVideoUsage;

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_SetVideoAttribute(HUINT32			ulDecId,
							HUINT32					ulWindowId,
							HBOOL					bMainWindow,
							HUINT32					ulFrameHorizontalSize,
							HUINT32					ulFrameVerticalSize,
							DxAspectRatio_e 	eFrameAspectRatio,
							VIDEO_Afd_t 			eAfd,
							HBOOL					bProgressive,
							VIDEO_FrameRate_t		eFrameRate,
							DxVideo3DFormat_e		e3DFormat
							)
{
	HUINT32		i;

	palScaler_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulDecId);
	HxLOG_Debug("\tWindow ID[%d] - [%s]\n", ulWindowId, bMainWindow ? "MAIN" : "SUB");
	HxLOG_Debug("\tFrame Horizontal Size : %d\n", ulFrameHorizontalSize);
	HxLOG_Debug("\tFrame Vertical Size : %d\n", ulFrameVerticalSize);
	HxLOG_Debug("\tFrame Aspect Ratio : %s\n", OTL_ENUM2STR_AspectRatio(eFrameAspectRatio));
	HxLOG_Debug("\tAFD : %s\n", OTL_ENUM2STR_Afd(eAfd));
	HxLOG_Debug("\tProgressive Flag : %d\n", bProgressive);
	HxLOG_Debug("\tFrame Rate : %s\n", OTL_ENUM2STR_VideoFrameRate(eFrameRate));
	HxLOG_Debug("\t3D Format : %s\n", OTL_ENUM2STR_Video3DFormat(e3DFormat));
	HxLOG_Debug("================================\n");

	if ((0 == ulFrameHorizontalSize) || (0 == ulFrameVerticalSize))
	{
		s_stVideoAttribute[ulDecId].bInfoValid = FALSE;

		palScaler_LEAVE_CRITICAL;

		for(i = 0; i < 10; i++)
		{
			HxLOG_Error("%s() Wrong Parameter (Hor:%d, Ver:%d)\n", __FUNCTION__, ulFrameHorizontalSize, ulFrameVerticalSize);
		}

		return ERR_FAIL;
	}

	s_stVideoAttribute[ulDecId].bInfoValid = TRUE;
	s_stVideoAttribute[ulDecId].ulFrameHorizontalSize = ulFrameHorizontalSize;
	s_stVideoAttribute[ulDecId].ulFrameVerticalSize = ulFrameVerticalSize;
	s_stVideoAttribute[ulDecId].eFrameAspectRatio = eFrameAspectRatio;
	s_stVideoAttribute[ulDecId].eAfd = eAfd;
	s_stVideoAttribute[ulDecId].bProgressive = bProgressive;
	s_stVideoAttribute[ulDecId].eFrameRate = eFrameRate;
	s_stVideoAttribute[ulDecId].e3DFormat = e3DFormat;
	s_stScalerStatus[ulDecId].ulWindowId = ulWindowId;

	if(bMainWindow == TRUE)
	{
		s_ulMainDecId = ulDecId;
	}

#if defined(CONFIG_PROD_HGS1000S)
	/* Temporarily, not to use pal_scaler_ChangeVideoOutputByVideoFrame() */
#else
	if(s_bSupportScalerByVideoFrame == TRUE)
		pal_scaler_ChangeVideoOutputByVideoFrame();
	else
#endif
		pal_scaler_ChangeVideoOutput();

	palScaler_LEAVE_CRITICAL;


	return ERR_OK;
}

void PAL_SCALER_ClearVideoAttribute(HUINT32 ulDecId)
{
	palScaler_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, ulDecId);

	s_stVideoAttribute[ulDecId].bInfoValid = FALSE;

	s_stScalerStatus[ulDecId].stDiVideoInRect.ulX = -1;
	s_stScalerStatus[ulDecId].stDiVideoOutRect.ulX = -1;

	pal_scaler_ChangeVideoOutput();

	palScaler_LEAVE_CRITICAL;

	return;
}

void PAL_SCALER_SetEdidAspectRatio(HBOOL bEnable)
{
	s_stUserSetting.bUseEdidAspectRatio = bEnable;
}

void PAL_SCALER_SetWideSignalLetterbox(HBOOL bEnable)
{
	s_stUserSetting.bWideSignalLetterbox = bEnable;
}

HERROR PAL_SCALER_SetTvAspectRatio(DxAspectRatio_e eTvAspectRatio, HBOOL bSyncNow)
{
	palScaler_ENTER_CRITICAL;

	HxLOG_Debug("%s(%s, %d)\n", __FUNCTION__, OTL_ENUM2STR_AspectRatio(eTvAspectRatio), bSyncNow);

	if(eTvAspectRatio != s_stUserSetting.eTvAspectRatio)
	{
		s_stUserSetting.eTvAspectRatio = eTvAspectRatio;

		if(eTvAspectRatio == eDxASPECTRATIO_4X3)
		{
			s_stOutputStatus.eOutputWss = eWss_43_Full;
		}
		else
		{
			s_stOutputStatus.eOutputWss = eWss_169_Full;
		}

		s_stOutputStatus.eOutputAfd = eAfd_Full;


		if(bSyncNow == TRUE)
		{
			pal_scaler_ChangeVideoOutput();
		}
	}
	else
	{
		HxLOG_Debug("\tSame as current. Exit...\n");
	}

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_GetTvAspectRatio(DxAspectRatio_e *peTvAspectRatio)
{
	palScaler_ENTER_CRITICAL;

	*peTvAspectRatio = s_stUserSetting.eTvAspectRatio;

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_SetDfcSelect(DxDfcSelect_e eDfc, HBOOL bSyncNow)
{
	palScaler_ENTER_CRITICAL;

	HxLOG_Debug("%s(%s, %d)\n", __FUNCTION__, OTL_ENUM2STR_DfcSelect(eDfc), bSyncNow);

	if(eDfc != s_stUserSetting.eDfc)
	{
		s_stUserSetting.eDfc = eDfc;

		if(bSyncNow == TRUE)
		{
			pal_scaler_ChangeVideoOutput();
		}
	}
	else
	{
		HxLOG_Debug("\tSame as current. Exit...\n");
	}

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;

}

HERROR PAL_SCALER_GetDfcSelect(DxDfcSelect_e *peDfc)
{
	palScaler_ENTER_CRITICAL;

	*peDfc = s_stUserSetting.eDfc;

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_SetResolutionLimit(HBOOL bEnable)
{
	palScaler_ENTER_CRITICAL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	if(bEnable != s_bResolutionLimit)
	{
		s_bResolutionLimit = bEnable;
		pal_scaler_ChangeVideoOutput();
	}
	else
	{
		HxLOG_Debug("\tSame as current. Exit...\n");
	}

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect, HBOOL bSyncNow)
{
	palScaler_ENTER_CRITICAL;

	HxLOG_Debug("%s(%s, %d)\n", __FUNCTION__, OTL_ENUM2STR_ResolutionSelect(eResolutionSelect), bSyncNow);

	if(eResolutionSelect != s_stUserSetting.eResolutionSelect)
	{
		s_stUserSetting.eResolutionSelect = eResolutionSelect;

		if(bSyncNow == TRUE)
		{
			pal_scaler_ChangeVideoOutput();
		}
	}
	else
	{
		HxLOG_Debug("\tSame as current. Exit...\n");
	}

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_GetResolutionSelect(DxResolutionSelect_e *peResolutionSelect)
{
	DI_NVRAM_VIDEO_RESOLUTION_E	eDiNvramHdResolution = DI_NVRAM_RESOLUTION_Unknown;
	DI_ERR_CODE						diErr = DI_ERR_ERROR;
	palScaler_ENTER_CRITICAL;	

	diErr = DI_NVRAM_GetField (DI_NVRAM_FIELD_HD_RESOULTION, 0, (void *)&eDiNvramHdResolution, sizeof(DI_NVRAM_VIDEO_RESOLUTION_E));
	*peResolutionSelect = pal_scaler_ConvertDiNvramVideoFormatToPalResolutionSelect(eDiNvramHdResolution);

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_SyncVideoSetting(void)
{
	palScaler_ENTER_CRITICAL;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	pal_scaler_ChangeVideoOutput();

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_GetOsdDisplayPoint(HUINT32	ulRefWidth,
							HUINT32			ulRefHeight,
							HINT32			nX,
							HINT32			nY,
							HINT32			*pnScaledX,
							HINT32			*pnScaledY)
{
#if 1 // 오차를 줄이기 위해 float 연산하도록 함.
	palScaler_VideoAttribute_t		*pstVideoAttrubute = &s_stVideoAttribute[s_ulMainDecId];
	palScaler_Status_t			*pstScalerStatus = &s_stScalerStatus[s_ulMainDecId];
	HFLOAT32				fScaledX, fScaledY;
	HUINT32 				ulOutputW, ulOutputH;

	palScaler_ENTER_CRITICAL;

	if( (ulRefWidth == 0)
		|| (ulRefHeight == 0)
		|| (pstVideoAttrubute->ulFrameHorizontalSize == 0)
		|| (pstVideoAttrubute->ulFrameVerticalSize == 0)
	)
	{
		palScaler_LEAVE_CRITICAL;
		HxLOG_Error("\n\n");
		return ERR_FAIL;
	}

	/* In rect 로 계산하기 위하여 비디오 size 로 좌표 확대 */
	fScaledX = nX * (HFLOAT32)pstVideoAttrubute->ulFrameHorizontalSize / (HFLOAT32)ulRefWidth;
	fScaledY = nY * (HFLOAT32)pstVideoAttrubute->ulFrameVerticalSize / (HFLOAT32)ulRefHeight;
//	printf("nScaledX:%f, nScaledY:%f\n", fScaledX, fScaledY);

	/* In rect 확대하기 위하여 중심 좌표로 변환 */
	fScaledX -= pstScalerStatus->stVideoInRect.nX + (pstScalerStatus->stVideoInRect.nW/2);
	fScaledY -= pstScalerStatus->stVideoInRect.nY + (pstScalerStatus->stVideoInRect.nH/2);
//	printf("nScaledX:%f, nScaledY:%f\n", fScaledX, fScaledY);

	/* In rect를 비디오 원본 size로 확대하는 비율로 scaled x, y 확대 */
	if( (pstScalerStatus->stVideoInRect.nW != 0) && (pstScalerStatus->stVideoInRect.nH != 0) )
	{
		fScaledX = fScaledX * (HFLOAT32)pstVideoAttrubute->ulFrameHorizontalSize / pstScalerStatus->stVideoInRect.nW;
		fScaledY = fScaledY * (HFLOAT32)pstVideoAttrubute->ulFrameVerticalSize / pstScalerStatus->stVideoInRect.nH;
	}
//	printf("nScaledX:%f, nScaledY:%f\n", fScaledX, fScaledY);

	/* Out rect로의 확대율 적용 */
	fScaledX = fScaledX * pstScalerStatus->stVideoOutRect.nW /pstVideoAttrubute->ulFrameHorizontalSize;
	fScaledY = fScaledY * pstScalerStatus->stVideoOutRect.nH / pstVideoAttrubute->ulFrameVerticalSize;
//	printf("nScaledX:%f, nScaledY:%f\n", fScaledX, fScaledY);

	/* Out rect offset 적용 */
	pal_scaler_GetOutputResSize(s_stOutputStatus.stOutputTiming.eResolution, &ulOutputW, &ulOutputH);

	fScaledX += (pstScalerStatus->stVideoOutRect.nX + pstScalerStatus->stVideoOutRect.nW/2) - (int)ulOutputW/2;
	fScaledY += (pstScalerStatus->stVideoOutRect.nY + pstScalerStatus->stVideoOutRect.nH/2) - (int)ulOutputH/2;
//	printf("nScaledX:%f, nScaledY:%f\n", fScaledX, fScaledY);

	/* 다시 ref size 로 변경하고  */
	fScaledX = fScaledX * ulRefWidth /ulOutputW;
	fScaledY = fScaledY * ulRefHeight /ulOutputH;
//	printf("nScaledX:%f, nScaledY:%f\n", fScaledX, fScaledY);

	/* 원점 좌표로 변환 */
	*pnScaledX = (HINT32)(ulRefWidth/2 + fScaledX);
	*pnScaledY = (HINT32)(ulRefHeight/2 + fScaledY);
//	printf("nScaledX:%d, nScaledY:%d\n", *pnScaledX, *pnScaledY);

#else
	palScaler_VideoAttribute_t		*pstVideoAttrubute = &s_stVideoAttribute[s_ulMainDecId];
	palScaler_Status_t			*pstScalerStatus = &s_stScalerStatus[s_ulMainDecId];
	HINT32					nScaledX, nScaledY;
	HUINT32 				ulOutputW, ulOutputH;

	palScaler_ENTER_CRITICAL;

	if( (ulRefWidth == 0)
		|| (ulRefHeight == 0)
		|| (pstVideoAttrubute->ulFrameHorizontalSize == 0)
		|| (pstVideoAttrubute->ulFrameVerticalSize == 0)
	)
	{
		palScaler_LEAVE_CRITICAL;
		HxLOG_Error("\n\n");
		return ERR_FAIL;
	}

	/* In rect 로 계산하기 위하여 비디오 size 로 좌표 확대 */
	nScaledX = nX * (int)pstVideoAttrubute->ulFrameHorizontalSize / (int)ulRefWidth;
	nScaledY = nY * (int)pstVideoAttrubute->ulFrameVerticalSize / (int)ulRefHeight;
//printf("nScaledX:%d, nScaledY:%d\n", nScaledX, nScaledY);

	/* In rect 확대하기 위하여 중심 좌표로 변환 */
	nScaledX -= pstScalerStatus->stVideoInRect.nX + pstScalerStatus->stVideoInRect.nW/2;
	nScaledY -= pstScalerStatus->stVideoInRect.nY + pstScalerStatus->stVideoInRect.nH/2;
//printf("nScaledX:%d, nScaledY:%d\n", nScaledX, nScaledY);

	/* In rect를 비디오 원본 size로 확대하는 비율로 scaled x, y 확대 */
	if( (pstScalerStatus->stVideoInRect.nW != 0) && (pstScalerStatus->stVideoInRect.nH != 0) )
	{
		nScaledX = nScaledX * (int)pstVideoAttrubute->ulFrameHorizontalSize / pstScalerStatus->stVideoInRect.nW;
		nScaledY = nScaledY * (int)pstVideoAttrubute->ulFrameVerticalSize / pstScalerStatus->stVideoInRect.nH;
	}
//printf("nScaledX:%d, nScaledY:%d\n", nScaledX, nScaledY);

	/* Out rect로의 확대율 적용 */
	nScaledX = nScaledX * pstScalerStatus->stVideoOutRect.nW / (int)pstVideoAttrubute->ulFrameHorizontalSize;
	nScaledY = nScaledY * pstScalerStatus->stVideoOutRect.nH / (int)pstVideoAttrubute->ulFrameVerticalSize;
//printf("nScaledX:%d, nScaledY:%d\n", nScaledX, nScaledY);

	/* Out rect offset 적용 */
	pal_scaler_GetOutputResSize(s_stOutputStatus.stOutputTiming.eResolution, &ulOutputW, &ulOutputH);

	nScaledX += (pstScalerStatus->stVideoOutRect.nX + pstScalerStatus->stVideoOutRect.nW/2) - (int)ulOutputW/2;
	nScaledY += (pstScalerStatus->stVideoOutRect.nY + pstScalerStatus->stVideoOutRect.nH/2) - (int)ulOutputH/2;
//printf("nScaledX:%d, nScaledY:%d\n", nScaledX, nScaledY);

	/* 다시 ref size 로 변경하고  */
	nScaledX = nScaledX * (int)ulRefWidth / (int)ulOutputW;
	nScaledY = nScaledY * (int)ulRefHeight / (int)ulOutputH;
//printf("nScaledX:%d, nScaledY:%d\n", nScaledX, nScaledY);

	/* 원점 좌표로 변환 */
	*pnScaledX = ulRefWidth/2 + nScaledX;
	*pnScaledY = ulRefHeight/2 + nScaledY;
//printf("nScaledX:%d, nScaledY:%d\n", *pnScaledX, *pnScaledY);
#endif


	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_GetActiveVideoRect(HUINT32 *pulScreenWidth, HUINT32 *pulScreenHeight, DxRect_t *pstActiveRect, HBOOL *pbPig)
{
	palScaler_ENTER_CRITICAL;

	//HxLOG_Debug("%s()\n", __FUNCTION__);

	pal_scaler_GetOutputResSize(s_stOutputStatus.stOutputTiming.eResolution, pulScreenWidth, pulScreenHeight);
	*pstActiveRect = s_stScalerStatus[s_ulMainDecId].stVideoOutRect;

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_SetBackgroundColor(HUINT8 ucR, HUINT8 ucG, HUINT8 ucB)
{
	DI_ERR_CODE				diErr;
	DI_VIDEO_COLOR_t		stBgColor;

	HxLOG_Debug("%s(R:%d, G:%d, B:%d)\n", __FUNCTION__, ucR, ucG, ucB);

	stBgColor.a = 0xff;
	s_ucBgColor_R = stBgColor.r = ucR;
	s_ucBgColor_G = stBgColor.g = ucG;
	s_ucBgColor_B = stBgColor.b = ucB;

	diErr = DI_VIDEO_SetBackground(0, stBgColor);
	if(diErr != DI_ERR_OK)
	{
		HxLOG_Error("\tERROR DI_VIDEO_SetBackground ret :0x%X\n",diErr);

		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_SCALER_GetBackgroundColor(HUINT8 *pucR, HUINT8 *pucG, HUINT8 *pucB)
{
	palScaler_ENTER_CRITICAL;

	*pucR = s_ucBgColor_R;
	*pucG = s_ucBgColor_G;
	*pucB = s_ucBgColor_B;

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_SetHdmi3DSelect(DxHdmi3DSelect_e e3DSelect)
{
	palScaler_ENTER_CRITICAL;

	s_stUserSetting.e3DSelect = e3DSelect;

	pal_scaler_ChangeVideoOutput();

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_GetHdmi3DSelect(DxHdmi3DSelect_e *pe3DSelect)
{
	palScaler_ENTER_CRITICAL;

	*pe3DSelect = s_stUserSetting.e3DSelect;

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_GetOutputStatus(
							VIDEO_Timing_t				*pstOutputTiming,
							DxDfcStatus_b				*pulDfcStatus,
							DxVideo3DFormat_e			*pe3DStatus
							)
{
	if(pstOutputTiming != NULL)
	{
		*pstOutputTiming = s_stOutputStatus.stOutputTiming;
	}

	if(pulDfcStatus != NULL)
	{
		*pulDfcStatus = s_stScalerStatus[s_ulMainDecId].ulDfcStatus;
	}

	if(pe3DStatus != NULL)
	{
		*pe3DStatus = s_stOutputStatus.e3DStatus;
	}

	return ERR_OK;
}

HERROR PAL_SCALER_SetCompositeStandard(DxCompositeStandard_e eCompositeStandard)
{
	DI_ERR_CODE		diErr;
	HUINT32			ulCurrentCompositeStandard;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eCompositeStandard);

	palScaler_ENTER_CRITICAL;

	s_stUserSetting.eCompositeStandard = eCompositeStandard;

	pal_scaler_ChangeVideoOutput();

	/* Loader에 알려주기 위함. */
	diErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_TV_STANDARD, 0, (void*)&ulCurrentCompositeStandard, sizeof(HUINT32));
	if( (diErr == DI_ERR_OK) && (ulCurrentCompositeStandard != eCompositeStandard) )
	{
		DI_NVRAM_SetField (DI_NVRAM_FIELD_TV_STANDARD, 0, (void *)&eCompositeStandard, sizeof(HUINT32) );
	}

	palScaler_LEAVE_CRITICAL;

	return DI_ERR_OK;
}

HERROR PAL_SCALER_GetCompositeStandard(DxCompositeStandard_e *peCompositeStandard)
{
	palScaler_ENTER_CRITICAL;

	*peCompositeStandard = s_stUserSetting.eCompositeStandard;

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR PAL_SCALER_SetSupportVideoFrame(HBOOL bEnable)
{
	palScaler_ENTER_CRITICAL;

	s_bSupportScalerByVideoFrame = bEnable;

	palScaler_LEAVE_CRITICAL;

	return ERR_OK;
}



/*********************** End of File ******************************/
