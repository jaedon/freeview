/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_video.h"
#include "fdi_osd.h"

#include "di_err.h"
#include "di_video.h"
#include "di_osd.h"
#include "di_audio.h"
#include "di_demux.h"
#include "di_hdmi.h"

#if (TV_STANDARD_OUTPUT_TYPE == TV_STANDARD_OUTPUT_NTSC)
#define MAX_RESOLUTION		DI_VIDEO_HD_RESOLUTION_1080I
#define MAX_HDMI_FORMAT		DI_HDMI_VFORMAT_1080I
#else
#define MAX_RESOLUTION		DI_VIDEO_HD_RESOLUTION_1080I_50HZ
#define MAX_HDMI_FORMAT		DI_HDMI_VFORMAT_1080I_50HZ
#endif
/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */
extern DI_OSD_HANDLE g_FactoryOsdHandle;

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
FDI_VIDEO_EventCallback g_pfnVideoEventCallback;
FDI_VIDEO_RESOLUTION eLastVideoResolution;

void INTRN_FDI_VIDEO_Callback(void *pData);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_GetNumOfDevice(unsigned long *pulNumOfDevice)
{
	DI_VIDEO_CAP_t *pCapInfo;

	DI_VIDEO_GetCapability((HUINT32*)pulNumOfDevice, &pCapInfo);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_GetCapability()
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_Open(unsigned long ulDeviceID)
{
 	DI_ERR_CODE eDiErr;
	DI_VIDEO_AOUT_MODE_t tDiOutputMode;

	tDiOutputMode.bRGB = tDiOutputMode.bSVHS = tDiOutputMode.bYPbPr = tDiOutputMode.bYPbPr_Downscale = FALSE;

	/* set output */
	#if (FDI_VIDEO_ANALOG_OUTPUT & ANALOG_VIDEO_OUTPUT_CVBS)
	tDiOutputMode.bCVBS = TRUE;
	#endif
	#if (FDI_VIDEO_ANALOG_OUTPUT & ANALOG_VIDEO_OUTPUT_RGB)
	tDiOutputMode.bRGB = TRUE;
	#endif
	#if (FDI_VIDEO_ANALOG_OUTPUT & ANALOG_VIDEO_OUTPUT_SVHS)
	tDiOutputMode.bSVHS = TRUE;
	#endif
	#if (FDI_VIDEO_ANALOG_OUTPUT & ANALOG_VIDEO_OUTPUT_YPBPR)
	tDiOutputMode.bYPbPr = TRUE;
	#endif
	#if (FDI_VIDEO_ANALOG_OUTPUT & ANALOG_VIDEO_OUTPUT_YPBPR_DNSCALED)
	tDiOutputMode.bYPbPr_Downscale = TRUE;
	#endif

	eDiErr = DI_VIDEO_SetOutputSettings(tDiOutputMode);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] DI_VIDEO_SetOutputSettings Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_Start(unsigned long ulDeviceID, FDI_VIDEO_FORMAT eVideoFormat)
{
 	DI_ERR_CODE eDiErr;
	DI_VIDEO_FORMAT eCodec;


	switch(eVideoFormat)
	{
		case FDI_VIDEO_FORMAT_MPEG1 :
			eCodec = DI_VIDEO_FORMAT_MPEG1;
			break;
		case FDI_VIDEO_FORMAT_MPEG2 :
			eCodec = DI_VIDEO_FORMAT_MPEG2;
			break;
#if defined(CONFIG_DI10)
		case FDI_VIDEO_FORMAT_MPEG2DTV :
			eCodec = DI_VIDEO_FORMAT_MPEG2DTV;
			break;
		case FDI_VIDEO_FORMAT_MPEG2DTV_PES :
			eCodec = DI_VIDEO_FORMAT_MPEG2DTV_PES;
			break;
#endif
		case FDI_VIDEO_FORMAT_MPEG4Part2 :
			eCodec = DI_VIDEO_FORMAT_MPEG4Part2;
			break;
		case FDI_VIDEO_FORMAT_H261 :
			eCodec = DI_VIDEO_FORMAT_H261;
			break;
		case FDI_VIDEO_FORMAT_H263 :
			eCodec = DI_VIDEO_FORMAT_H263;
			break;
		case FDI_VIDEO_FORMAT_H264 :
			eCodec = DI_VIDEO_FORMAT_H264;
			break;
		case FDI_VIDEO_FORMAT_VC1SimpleMain :
			eCodec = DI_VIDEO_FORMAT_VC1SimpleMain;
			break;
		case FDI_VIDEO_FORMAT_VC1 :
			eCodec = DI_VIDEO_FORMAT_VC1;
			break;
		case FDI_VIDEO_FORMAT_AVS :
			eCodec = DI_VIDEO_FORMAT_AVS;
			break;
		case FDI_VIDEO_FORMAT_H265:
			eCodec = DI_VIDEO_FORMAT_H265;
			break;			
		default :
			eCodec = DI_VIDEO_FORMAT_MPEG2;
	}

	eDiErr = DI_VIDEO_Start((unsigned char)ulDeviceID, eCodec);
	if(eDiErr != DI_ERR_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_Stop(unsigned long ulDeviceID)
{
	DI_ERR_CODE eDiErr;

	eDiErr = DI_VIDEO_Stop((unsigned char)ulDeviceID);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_Stop] DI_VIDEO_Stop Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_Show(unsigned long ulDeviceID, BOOL bShow)
{
	DI_ERR_CODE eDiErr;

	eDiErr = DI_VIDEO_Show(ulDeviceID, bShow);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_Show] DI_VIDEO_Show Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_DisplayColorbar(unsigned long ulDeviceID, BOOL bOnOff)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_SetOutWindow(unsigned long ulDeviceID, FDI_VIDEO_RECT *pstOutRect)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_SetAnalogOutput(unsigned long ulDeviceID, FDI_VIDEO_OUT_MODE tVideoOuputMode)
{
	DI_ERR_CODE eDiErr;
	DI_VIDEO_AOUT_MODE_t tDiOutputMode;

	tDiOutputMode.bCVBS = tVideoOuputMode.bCVBS;
	tDiOutputMode.bRGB = tVideoOuputMode.bRGB;
	tDiOutputMode.bSVHS = tVideoOuputMode.bSVHS;
	tDiOutputMode.bYPbPr = tVideoOuputMode.bYPbPr;
	tDiOutputMode.bYPbPr_Downscale = tVideoOuputMode.bYPbPr_Downscale;

	eDiErr = DI_VIDEO_SetOutputSettings(tDiOutputMode);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] DI_VIDEO_SetOutputSettings Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_GetAnalogOutput(unsigned long ulDeviceID, FDI_VIDEO_OUT_MODE *tVideoOuputMode)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_SetDisplayStandard(unsigned long ulDeviceID, FDI_VIDEO_RESOLUTION eVideoResolution)
{
	DI_ERR_CODE eDiErr;
	DI_VIDEO_HD_RESOLUTION eHDOutput;
	DI_HDMI_VIDEO_FORMAT	eHDMIFormat;
	DI_HDMI_AVAILABLE_FORMATS	avaiable;
	DI_OSD_RECT tOSDRect;

	eLastVideoResolution = eVideoResolution;

	switch(eVideoResolution)
	{
		case FDI_VIDEO_RESOLUTION_1080I:
			eHDOutput = DI_VIDEO_HD_RESOLUTION_1080I;
			eHDMIFormat = DI_HDMI_VFORMAT_1080I;
			break;
		case FDI_VIDEO_RESOLUTION_1080I_50HZ:
			eHDOutput = DI_VIDEO_HD_RESOLUTION_1080I_50HZ;
			eHDMIFormat = DI_HDMI_VFORMAT_1080I_50HZ;
			break;
		case FDI_VIDEO_RESOLUTION_720P:
			eHDOutput = DI_VIDEO_HD_RESOLUTION_720P;
			eHDMIFormat = DI_HDMI_VFORMAT_720P;
			break;
		case FDI_VIDEO_RESOLUTION_720P_50HZ:
			eHDOutput = DI_VIDEO_HD_RESOLUTION_720P_50HZ;
			eHDMIFormat = DI_HDMI_VFORMAT_720P_50HZ;
			break;
		case FDI_VIDEO_RESOLUTION_576P:
			eHDOutput = DI_VIDEO_HD_RESOLUTION_576P;
			eHDMIFormat = DI_HDMI_VFORMAT_576P;
			break;
		case FDI_VIDEO_RESOLUTION_576I:
			eHDOutput = DI_VIDEO_HD_RESOLUTION_576I;
			eHDMIFormat = DI_HDMI_VFORMAT_576I | DI_HDMI_VFORMAT_576I_4_3;
			break;
		case FDI_VIDEO_RESOLUTION_480P:
			eHDOutput = DI_VIDEO_HD_RESOLUTION_480P;
			eHDMIFormat = DI_HDMI_VFORMAT_480P;
			break;
		case FDI_VIDEO_RESOLUTION_480I:
			eHDOutput = DI_VIDEO_HD_RESOLUTION_480I;
			eHDMIFormat = DI_HDMI_VFORMAT_480I;
			break;

		case FDI_VIDEO_RESOLUTION_1080P:
			eHDOutput = DI_VIDEO_HD_RESOLUTION_1080P;
			eHDMIFormat = DI_HDMI_VFORMAT_1080P;
			break;

		case FDI_VIDEO_RESOLUTION_1080P50:
			eHDOutput = DI_VIDEO_HD_RESOLUTION_1080P_50HZ;
			eHDMIFormat = DI_HDMI_VFORMAT_1080P_50HZ;
			break;

		case FDI_VIDEO_RESOLUTION_MAX:
		default:
			eHDOutput = MAX_RESOLUTION;
			eHDMIFormat = MAX_HDMI_FORMAT;
			break;
	}

	/* EDID Check */
	eDiErr = DI_HDMI_GetCapability(&avaiable);

	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] DI_HDMI_GetCapability  result %x, setting %x, available %x\n", eDiErr, eHDMIFormat, avaiable.ulVideoFormats));
	if( eDiErr == DI_ERR_OK )
	{
		if(!(avaiable.ulVideoFormats & eHDMIFormat))
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] not support %x\n", eHDOutput));
			/* 다른 포멧으로 전환 필요.. */
			switch(eHDOutput)
			{
				case DI_VIDEO_HD_RESOLUTION_576I :
					eHDOutput = DI_VIDEO_HD_RESOLUTION_576P;
					FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] replace 576I to 576P\n"));
					break;
				case DI_VIDEO_HD_RESOLUTION_1080P_50HZ :
					eHDOutput = DI_VIDEO_HD_RESOLUTION_1080I_50HZ;
					FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] replace 1080P_50HZ to 1080I_50HZ\n"));
					break;
				case DI_VIDEO_HD_RESOLUTION_480I :
					eHDOutput = DI_VIDEO_HD_RESOLUTION_480P;
					FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] replace 480I to 480P\n"));
					break;
				case DI_VIDEO_HD_RESOLUTION_1080P:
					eHDOutput = DI_VIDEO_HD_RESOLUTION_1080I;
					FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] replace 1080P to 1080I\n"));
					break;
			}
		}
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] error at DI_HDMI_GetCapability\n"));
	}

	eDiErr = DI_VIDEO_SetDisplayStandard(eHDOutput);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] DI_VIDEO_SetOutputSettings Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	/* OSD refresh */
	tOSDRect.x = 0;
	tOSDRect.y = 0;
	tOSDRect.width = FDI_OSD_MAX_WIDTH;
	tOSDRect.height = FDI_OSD_MAX_HEIGHT;

	eDiErr = DI_OSD_Flush(g_FactoryOsdHandle, &tOSDRect);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_SetOutputSetting] DI_OSD_Flush Error : %d\n", eDiErr));
		return FDI_ERR;
	}
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_ResetDisplayStandard(void)
{
	return FDI_VIDEO_SetDisplayStandard(0, eLastVideoResolution);

}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_RegisterEventCallback(unsigned long ulDeviceId,
											 FDI_VIDEO_EventCallback fnVideoCallback)
{
	DI_ERR_CODE eDiErr;
	HUINT32 unCallerId = 0;

	g_pfnVideoEventCallback = fnVideoCallback;

	eDiErr = DI_VIDEO_RegisterEventCallback(ulDeviceId, FDI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED, INTRN_FDI_VIDEO_Callback, &unCallerId);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_VIDEO_RegisterEventCallback] DI_VIDEO_RegisterEventCallback Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_VIDEO_ControlDPort
* description : dport control (scan_line, iorp, ar)
* input : resolution, aspect ratio
* output : none
* return : FDI_ERR_CODE
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_ControlDPort(unsigned long ulDeviceId, FDI_VIDEO_RESOLUTION eHdResolution, FDI_VIDEO_AspectRatio eAspectRatio)
{
	FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
	DI_ERR_CODE	eDiError = DI_ERR_OK;
	DI_VIDEO_HD_RESOLUTION eResolution = FDI_VIDEO_RESOLUTION_480I;
	DI_VIDEO_AspectRatio_t eAR = FDI_VIDEO_ASPECTRATIO_4_3;

	switch(eHdResolution)
	{
		case FDI_VIDEO_RESOLUTION_1080I:
		case FDI_VIDEO_RESOLUTION_1080I_50HZ:
			eResolution = DI_VIDEO_HD_RESOLUTION_1080I;
			break;
		case FDI_VIDEO_RESOLUTION_720P:
		case FDI_VIDEO_RESOLUTION_720P_50HZ:
			eResolution = DI_VIDEO_HD_RESOLUTION_720P;
			break;
		case FDI_VIDEO_RESOLUTION_480P:
		case FDI_VIDEO_RESOLUTION_576P:
			eResolution = DI_VIDEO_HD_RESOLUTION_480P;
			break;
		case FDI_VIDEO_RESOLUTION_480I:
		case FDI_VIDEO_RESOLUTION_480I_J:
		case FDI_VIDEO_RESOLUTION_576I:
			eResolution = DI_VIDEO_HD_RESOLUTION_480I;
			break;
		default:
			eFdiError = FDI_ERR_RESOLUTION;
			break;
	}

	switch(eAspectRatio)
	{
		case FDI_VIDEO_ASPECTRATIO_16_9:
			eAR = DI_VIDEO_ASPECTRATIO_16_9;
			break;
		case FDI_VIDEO_ASPECTRATIO_4_3:
			eAR = DI_VIDEO_ASPECTRATIO_4_3;
			break;
		case FDI_VIDEO_ASPECTRATIO_4_3_LETTER:
			eAR = DI_VIDEO_ASPECTRATIO_4_3_LETTER;
			break;
		case FDI_VIDEO_ASPECTRATIO_14_9:
			eAR = DI_VIDEO_ASPECTRATIO_14_9;
			break;
		case FDI_VIDEO_ASPECTRATIO_1_1:
			eAR = DI_VIDEO_ASPECTRATIO_1_1;
			break;
		default:
			eFdiError = FDI_ERR_ASPECT;
			break;
	}

	if(eFdiError == FDI_NO_ERR)
	{
		eDiError = DI_VIDEO_ControlDPort(ulDeviceId, eResolution, eAR);
		if(eDiError != DI_ERR_OK)
		{
			eFdiError = FDI_ERR;
		}
	}
	return eFdiError;
}

/*******************************************************************************
* function : FDI_VIDEO_SetSaspectVoltage
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_SetSaspectVoltage()
{
	return FDI_FEATURE_NOT_SUPPORTED;
}


/*******************************************************************************
* function : INTRN_FDI_VIDEO_Callback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FDI_VIDEO_Callback(void *pData)
{
	FDI_VIDEO_CALLBACK_DATA_t *ptCallbackData;
	FDI_VIDEO_RUNNING_STATE_EVT_t *ptVideoRunningState;

	ptCallbackData = (FDI_VIDEO_CALLBACK_DATA_t *)pData;
	ptVideoRunningState = (FDI_VIDEO_RUNNING_STATE_EVT_t *)ptCallbackData->pvEventData;

	if( g_pfnVideoEventCallback != NULL )
	{
		g_pfnVideoEventCallback(ptCallbackData->eEvent, ptVideoRunningState->eState);
	}
}


