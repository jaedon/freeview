/*******************************************************************/
/**
 * @file	di_video_enc.c
 *
 * Source Code for Video Encoder Module
 *
 * @author	HyunSeok Choi (hs.choi@humaxdigital.com)
 * @data	2011/12/21
 * @attention	Copyright (c) 2011 Humax - All rights reserved
 *
 **/
/*******************************************************************/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include "bstd.h"
#include "bkni.h"
#include "linden_trace.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_display_vbi.h"
#include "nexus_video_input.h"
#include "nexus_video_output.h"
#include "nexus_video_window.h"
#include "nexus_video_encoder.h"
#include "nexus_video_encoder_output.h"
#include "nexus_video_window.h"
#include "nexus_video_adj.h"
#include "nexus_core_utils.h"

/* include  di headers */
#include "drv_err.h"
#include "di_video.h"
#include "drv_video.h"
#include "di_uart.h"
#include "drv_video_enc.h"
#include "vkernel.h"
#include "taskdef.h"
#include "di_demux.h"
#include "drv_demux.h"
#include "drv_pvr.h"
#include "di_config.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_DI_VIDEO
//#define DI_VIDEO_ENC_DEBUG
//#define DI_VIDEO_ENC_ENTER_LEAVE_DEBUG
//#define DI_VIDEO_ENC_SEE_PARAMETERS
#define PARAM_COLOR_ON		"\033[01;35m"
#define PARAM_COLOR_OFF	"\033[00m"

#ifndef DEBUG_MODULE
#ifdef DI_VIDEO_ENC_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#endif

#ifdef DI_VIDEO_ENC_ENTER_LEAVE_DEBUG
#define PrintEnter()	DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()	DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter()
#define PrintExit()
#endif
#endif /* #ifndef DEBUG_MODULE */

#define PrintStatus(...)	DI_UART_Print (__VA_ARGS__)

#define NUM_VID_ENC	2
#define DEF_VID_INST	0
#define VID_ENC_DISP_IDX	    NEXUS_ENCODER1_DISPLAY_IDX
#define VID_ENC_DISP_IDX_EXT	NEXUS_ENCODER0_DISPLAY_IDX
#define VID_ENC_WIN_IDX		0

#define DI_NUM_VID_ENC_DISPS	2
#define DI_NUM_VID_ENC_WINS	1

#define VID_MESSAGE_QUEUE_SIZE 10
#define VID_ENC_TASK_CHECK_STATE_TIME 100

#define UNREFENCED_PARAM(x) (x=x)

#define MAX_NUM_OF_FRAMEP 0xFFFF
#define MAX_NUM_OF_FRAMEB 3

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef struct VidEncInst
{
	NEXUS_VideoDecoderHandle hDecoder;
	NEXUS_VideoEncoderHandle hEncoder;
	NEXUS_DisplayHandle hDisplay;
	NEXUS_VideoWindowHandle  hWindow;
	NEXUS_VideoInput hInput;
	NEXUS_VideoDecoderStartSettings startSettings;
	NEXUS_DisplaySettings displaySettings;
	NEXUS_DisplayCustomFormatSettings customFormatSettings;

	NEXUS_VideoEncoderSettings	videoEncoderConfig;
	NEXUS_VideoEncoderStartSettings	videoEncoderStartConfig;
	NEXUS_VideoEncoderDelayRange 	videoDelay;

	HBOOL	bInitialized;
	HBOOL   bDecoding;
	HBOOL 	bEncoding;
	HBOOL	bWatching;
	HBOOL	bFileOut;
	unsigned long 	ulSemId_DRV;

} DI_VidEncInst_t;

typedef struct VidEncMsg
{
	unsigned long ulParam0;
	unsigned long ulParam1;

} DI_VidEncMsg_t;

typedef struct VidEncProfileLevel
{
	NEXUS_VideoFormat eFormat;
	NEXUS_VideoFrameRate eFrameRate;
	NEXUS_VideoCodecLevel eLevel;
	NEXUS_VideoCodecProfile eProfile;
	HUINT32	MaxBitrate;
	HUINT32 Pframe;
	HUINT32 Bframe;
} DI_VidEncProfileLevel_t;

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DI_VidEncInst_t	s_stVideoEnc[NUM_VID_ENC];
static pfnDRV_VidEncCallback_Notify VideoEncCallback;
//static unsigned long ulVideoMonMsgQueueId;

static DI_VidEncProfileLevel_t s_stVidEncMpeg2PL[] = {
	{NEXUS_VideoFormat_eNtsc, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_eMain, NEXUS_VideoCodecProfile_eMain, 187500,10,2},
	{NEXUS_VideoFormat_ePal, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_eMain, NEXUS_VideoCodecProfile_eMain, 187500,8,2},
	{NEXUS_VideoFormat_e576p, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_eMain, NEXUS_VideoCodecProfile_eMain, 187500,8,2},
	{NEXUS_VideoFormat_e720p, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 7500000,10,2},
	{NEXUS_VideoFormat_e720p, NEXUS_VideoFrameRate_e60, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 10000000,20,2},
	{NEXUS_VideoFormat_e720p50hz, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 7500000,8,2},
	{NEXUS_VideoFormat_e720p50hz, NEXUS_VideoFrameRate_e50, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 10000000,17,2},
	{NEXUS_VideoFormat_e1080i, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 10000000,29,0},
	{NEXUS_VideoFormat_e1080i50hz, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_eHigh, NEXUS_VideoCodecProfile_eMain, 10000000,24,0}
};

/* Apply The Kush Gauge rule for deciding Max bitrate
* Motion factor High = 4, Middle = 2, Low = 1
* Width * Height * FrameRate * 0.07 * Motion factor (1,2 and 4)
* To do : Create to calculate Max bitrate based on the Kush Gauge
*/
static DI_VidEncProfileLevel_t s_stVidEncH264PL[] = {
	{NEXUS_VideoFormat_eNtsc, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_e30, NEXUS_VideoCodecProfile_eHigh, 2768554,10,2},
	{NEXUS_VideoFormat_ePal, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_e30, NEXUS_VideoCodecProfile_eHigh, 2768554,8,2},
	{NEXUS_VideoFormat_e576p, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_e30, NEXUS_VideoCodecProfile_eHigh, 2768554,8,2},
	{NEXUS_VideoFormat_e720p, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_e31, NEXUS_VideoCodecProfile_eHigh, 7382812,10,2},
	{NEXUS_VideoFormat_e720p, NEXUS_VideoFrameRate_e60, NEXUS_VideoCodecLevel_e32, NEXUS_VideoCodecProfile_eHigh, 1476562,20,2},
	{NEXUS_VideoFormat_e720p50hz, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_e31, NEXUS_VideoCodecProfile_eHigh, 6152343,8,2},
	{NEXUS_VideoFormat_e720p50hz, NEXUS_VideoFrameRate_e50, NEXUS_VideoCodecLevel_e32, NEXUS_VideoCodecProfile_eHigh, 12304687,17,2},
	{NEXUS_VideoFormat_e1080i, NEXUS_VideoFrameRate_e30, NEXUS_VideoCodecLevel_e40, NEXUS_VideoCodecProfile_eHigh, 16611328,29,0},
	{NEXUS_VideoFormat_e1080i50hz, NEXUS_VideoFrameRate_e25, NEXUS_VideoCodecLevel_e40, NEXUS_VideoCodecProfile_eHigh, 13842773,24,0}
};

/*******************************************************************/
/*********************** extern function prototypes ****************/
/*******************************************************************/

/*******************************************************************/
/*********************** Static function prototypes ****************/
/*******************************************************************/
#if 0
static void P_VIDEO_EncBuffMonitorTask(void *vpParam);
#endif

/*******************************************************************/
/*********************** Function Description **********************/
/*******************************************************************/

/**
 * @param[in] void *context
 * @param[in] int param
 * @return not valid
 * @par requirements
 * @see APIs
*/
static void P_VIDEO_DecSourceChangedCallBack(void *context, int param)
{
	HUINT32 ulDeviceId = param;
	UNREFENCED_PARAM(context);
	UNREFENCED_PARAM(ulDeviceId);
}

/**
 * @param[in] void *context
 * @param[in] int param
 * @return not valid
 * @par requirements
 * @see APIs
*/
static void P_VIDEO_DecStreamChangedCallBack(void *context, int param)
{
	HUINT32 ulDeviceId = param;
	UNREFENCED_PARAM(context);
	VideoEncCallback(ulDeviceId,DRV_VIDEO_NOTIFY_ENC_DEC_SEQ_INFO, NULL);
}

/**
 * @param[in] void *context
 * @param[in] int param
 * @return not valid
 * @par requirements
 * @see APIs
*/
static void P_VIDEO_DecFirstPtsPassedCallBack(void *context, int param)
{
	HUINT32 ulDeviceId = param;
	UNREFENCED_PARAM(context);
	VideoEncCallback(ulDeviceId,DRV_VIDEO_NOTIFY_ENC_DEC_PROBE, NULL);
}

/**
 * @param[in] DI_VIDEO_AspectRatio_t eAspectRatio
 * @return NEXUS_AspectRatio
 * @par requirements
 * @see APIs
*/
static NEXUS_AspectRatio P_VIDEO_EncCovert2NexusAspectRatio(DI_VIDEO_AspectRatio_t eAspectRatio)
{
	NEXUS_AspectRatio eConvertAspectRatio = NEXUS_AspectRatio_eUnknown;

	switch(eAspectRatio)
	{
		case DI_VIDEO_ASPECTRATIO_4_3:
			eConvertAspectRatio = NEXUS_DisplayAspectRatio_e4x3;
			break;
		case DI_VIDEO_ASPECTRATIO_16_9:
			eConvertAspectRatio = NEXUS_DisplayAspectRatio_e16x9;
			break;
		case DI_VIDEO_ASPECTRATIO_SAR:
			eConvertAspectRatio = NEXUS_DisplayAspectRatio_eSar;
			break;
		default:
			eConvertAspectRatio = NEXUS_DisplayAspectRatio_eAuto;
			break;
	}
	return eConvertAspectRatio;
}

/**
 * @param[in] DI_TRANSCODER_VIDEO_CODEC_LEVEL eLevel
 * @return NEXUS_VideoCodecLevel
 * @par requirements
 * @see APIs
*/
static NEXUS_VideoCodecLevel P_VIDEO_EncCovert2NexusCodecLevel(DI_TRANSCODER_VIDEO_CODEC_LEVEL eLevel)
{
	NEXUS_VideoCodecLevel eCodecLevel = NEXUS_VideoCodecLevel_eMax;

	switch(eLevel)
	{
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_00:
			eCodecLevel = NEXUS_VideoCodecLevel_e00;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_10:
			eCodecLevel = NEXUS_VideoCodecLevel_e10;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_1B:
			eCodecLevel = NEXUS_VideoCodecLevel_e1B;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_11:
			eCodecLevel = NEXUS_VideoCodecLevel_e11;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_12:
			eCodecLevel = NEXUS_VideoCodecLevel_e12;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_13:
			eCodecLevel = NEXUS_VideoCodecLevel_e13;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_20:
			eCodecLevel = NEXUS_VideoCodecLevel_e20;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_21:
			eCodecLevel = NEXUS_VideoCodecLevel_e21;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_22:
			eCodecLevel = NEXUS_VideoCodecLevel_e22;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_30:
			eCodecLevel = NEXUS_VideoCodecLevel_e30;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_31:
			eCodecLevel = NEXUS_VideoCodecLevel_e31;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_32:
			eCodecLevel = NEXUS_VideoCodecLevel_e32;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_40:
			eCodecLevel = NEXUS_VideoCodecLevel_e40;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_41:
			eCodecLevel = NEXUS_VideoCodecLevel_e41;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_42:
			eCodecLevel = NEXUS_VideoCodecLevel_e42;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_50:
			eCodecLevel = NEXUS_VideoCodecLevel_e50;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_51:
			eCodecLevel = NEXUS_VideoCodecLevel_e51;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_60:
			eCodecLevel = NEXUS_VideoCodecLevel_e60;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_62:
			eCodecLevel = NEXUS_VideoCodecLevel_e62;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_LOW:
			eCodecLevel = NEXUS_VideoCodecLevel_eLow;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_MAIN:
			eCodecLevel = NEXUS_VideoCodecLevel_eMain;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_HIGH:
			eCodecLevel = NEXUS_VideoCodecLevel_eHigh;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_LEVEL_HIGH1440:
			eCodecLevel = NEXUS_VideoCodecLevel_eHigh1440;
			break;
		default:
			eCodecLevel = NEXUS_VideoCodecLevel_eMax;
			break;
	}
	return eCodecLevel;
}

/**
 * @param[in] NEXUS_VideoCodecLevel eLevel
 * @return DI_TRANSCODER_VIDEO_CODEC_LEVEL
 * @par requirements
 * @see APIs
*/
static DI_TRANSCODER_VIDEO_CODEC_LEVEL P_VIDEO_EncCovert2DICodecLevel(NEXUS_VideoCodecLevel eLevel)
{
	DI_TRANSCODER_VIDEO_CODEC_LEVEL eCodecLevel;

	switch(eLevel)
	{
		case NEXUS_VideoCodecLevel_e00:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_00;
			break;
		case NEXUS_VideoCodecLevel_e10:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_10;
			break;
		case NEXUS_VideoCodecLevel_e1B:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_1B;
			break;
		case NEXUS_VideoCodecLevel_e11:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_11;
			break;
		case NEXUS_VideoCodecLevel_e12:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_12;
			break;
		case NEXUS_VideoCodecLevel_e13:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_13;
			break;
		case NEXUS_VideoCodecLevel_e20:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_20;
			break;
		case NEXUS_VideoCodecLevel_e21:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_21;
			break;
		case NEXUS_VideoCodecLevel_e22:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_22;
			break;
		case NEXUS_VideoCodecLevel_e30:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_30;
			break;
		case NEXUS_VideoCodecLevel_e31:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_31;
			break;
		case NEXUS_VideoCodecLevel_e32:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_32;
			break;
		case NEXUS_VideoCodecLevel_e40:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_40;
			break;
		case NEXUS_VideoCodecLevel_e41:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_41;
			break;
		case NEXUS_VideoCodecLevel_e42:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_42;
			break;
		case NEXUS_VideoCodecLevel_e50:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_50;
			break;
		case NEXUS_VideoCodecLevel_e51:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_51;
			break;
		case NEXUS_VideoCodecLevel_e60:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_60;
			break;
		case NEXUS_VideoCodecLevel_e62:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_62;
			break;
		case NEXUS_VideoCodecLevel_eLow:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_LOW;
			break;
		case NEXUS_VideoCodecLevel_eMain:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_MAIN;
			break;
		case NEXUS_VideoCodecLevel_eHigh:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_HIGH;
			break;
		case NEXUS_VideoCodecLevel_eHigh1440:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_HIGH1440;
			break;
		default:
			eCodecLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_MAX;
			break;
	}
	return eCodecLevel;
}

/**
 * @param[in] DI_TRANSCODER_VIDEO_CODEC_PROFILE eProfile
 * @return NEXUS_VideoCodecProfile
 * @par requirements
 * @see APIs
*/
static NEXUS_VideoCodecProfile P_VIDEO_EncCovert2NexusCodecProfile(DI_TRANSCODER_VIDEO_CODEC_PROFILE eProfile)
{
	NEXUS_VideoCodecProfile eCodecProfile = NEXUS_VideoCodecProfile_eUnknown;

	switch(eProfile)
	{
		case DI_TRANSCODER_VIDEO_CODEC_PROFILE_SIMPLE:
			eCodecProfile = NEXUS_VideoCodecProfile_eSimple;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAIN:
			eCodecProfile = NEXUS_VideoCodecProfile_eMain;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_PROFILE_HIGH:
			eCodecProfile = NEXUS_VideoCodecProfile_eHigh;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_PROFILE_ADVANCED:
			eCodecProfile = NEXUS_VideoCodecProfile_eAdvanced;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_PROFILE_JIZHUN:
			eCodecProfile = NEXUS_VideoCodecProfile_eJizhun;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_PROFILE_SNRSCALABLE:
			eCodecProfile = NEXUS_VideoCodecProfile_eSnrScalable;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_PROFILE_SPATIALLYSCALABLE:
			eCodecProfile = NEXUS_VideoCodecProfile_eSpatiallyScalable;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_PROFILE_ADVANCEDSIMPLE:
			eCodecProfile = NEXUS_VideoCodecProfile_eAdvancedSimple;
			break;
		case DI_TRANSCODER_VIDEO_CODEC_PROFILE_BASELINE:
			eCodecProfile = NEXUS_VideoCodecProfile_eBaseline;
			break;
		default:
			eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAX;
			break;
	}
	return eCodecProfile;
}

/**
 * @param[in] NEXUS_VideoCodecProfile eProfile
 * @return DI_TRANSCODER_VIDEO_CODEC_PROFILE
 * @par requirements
 * @see APIs
*/
static DI_TRANSCODER_VIDEO_CODEC_PROFILE P_VIDEO_EncCovert2DICodecProfile(NEXUS_VideoCodecProfile eProfile)
{
	DI_TRANSCODER_VIDEO_CODEC_PROFILE eCodecProfile;

	switch(eProfile)
	{
		case NEXUS_VideoCodecProfile_eSimple:
			eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_SIMPLE;
			break;
		case NEXUS_VideoCodecProfile_eMain:
			eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAIN;
			break;
		case NEXUS_VideoCodecProfile_eHigh:
			eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_HIGH;
			break;
		case NEXUS_VideoCodecProfile_eAdvanced:
			eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_ADVANCED;
			break;
		case NEXUS_VideoCodecProfile_eJizhun:
			eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_JIZHUN;
			break;
		case NEXUS_VideoCodecProfile_eSnrScalable:
			eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_SNRSCALABLE;
			break;
		case NEXUS_VideoCodecProfile_eSpatiallyScalable:
			eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_SPATIALLYSCALABLE;
			break;
		case NEXUS_VideoCodecProfile_eAdvancedSimple:
			eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_ADVANCEDSIMPLE;
			break;
		case NEXUS_VideoCodecProfile_eBaseline:
			eCodecProfile = DI_TRANSCODER_VIDEO_CODEC_PROFILE_BASELINE;
			break;
		default:
			eCodecProfile = NEXUS_VideoCodecProfile_eMax;
			break;
	}
	return eCodecProfile;
}

/**
 * @param[in] NEXUS_DisplayAspectRatio eAspectRatio
 * @return DI_VIDEO_AspectRatio_t
 * @par requirements
 * @see APIs
*/
static DI_VIDEO_AspectRatio_t P_VIDEO_EncConvert2DIAspectRatio(NEXUS_AspectRatio eAspectRatio)
{
	DI_VIDEO_AspectRatio_t eDiAspectRatio;

	switch(eAspectRatio)
	{
		case NEXUS_AspectRatio_e4x3:
			eDiAspectRatio = DI_VIDEO_ASPECTRATIO_4_3;
			break;
		case NEXUS_AspectRatio_e16x9:
			eDiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
			break;
		case NEXUS_AspectRatio_eSar:
			eDiAspectRatio = DI_VIDEO_ASPECTRATIO_SAR;
			break;
		default:
			eDiAspectRatio = DI_VIDEO_ASPECTRATIO_16_9;
			break;
	}

	return eDiAspectRatio;
}

/**
 * @param[in] NEXUS_DisplaySettings *displayCfg
 * @param[in] NEXUS_VideoEncoderSettings *videoEncoderConfig
 * @param[in] NEXUS_VideoEncoderStartSettings *videoEncoderStartConfig
 * @return int
 * @par requirements
 * @see APIs
*/
static int P_VIDEO_EncGetPLB(NEXUS_DisplaySettings *displayCfg, NEXUS_VideoEncoderSettings *videoEncoderConfig, NEXUS_VideoEncoderStartSettings *videoEncoderStartConfig)
{
	HUINT32 idx=0, i=0;
	DI_VidEncProfileLevel_t *ePL;

	switch(videoEncoderStartConfig->codec)
	{
		case NEXUS_VideoCodec_eMpeg2:
			idx=sizeof(s_stVidEncMpeg2PL)/sizeof(DI_VidEncProfileLevel_t);
			ePL = s_stVidEncMpeg2PL;
			break;
		case NEXUS_VideoCodec_eH264:
			idx=sizeof(s_stVidEncH264PL)/sizeof(DI_VidEncProfileLevel_t);
			ePL = s_stVidEncH264PL;
			break;
		default:
			idx=0;
			break;
	}

	if(idx <= 1)
		return -1;

	for(i=0;i<idx;i++)
	{
		if(ePL[i].eFormat == displayCfg->format && ePL[i].eFrameRate == videoEncoderConfig->frameRate)
		{
			videoEncoderConfig->bitrateMax = ePL[i].MaxBitrate;
			videoEncoderStartConfig->profile = ePL[i].eProfile;
			videoEncoderStartConfig->level= ePL[i].eLevel;
			videoEncoderConfig->streamStructure.framesP=ePL[i].Pframe;
			videoEncoderConfig->streamStructure.framesB=ePL[i].Bframe;
			return 0;
		}
	}

	return -1;
}

/**
 * @param[in] NEXUS_DisplaySettings *displayCfg
 * @param[in] NEXUS_VideoEncoderSettings *videoEncoderConfig
 * @param[in] NEXUS_VideoEncoderStartSettings *videoEncoderStartConfig
 * @return int
 * @par requirements
 * @see APIs
*/
static int P_VIDEO_EncGetB(NEXUS_DisplaySettings *displayCfg, NEXUS_VideoEncoderSettings *videoEncoderConfig, NEXUS_VideoEncoderStartSettings *videoEncoderStartConfig)
{
	HUINT32 idx=0, i=0;
	DI_VidEncProfileLevel_t *ePL;

	switch(videoEncoderStartConfig->codec)
	{
		case NEXUS_VideoCodec_eMpeg2:
			idx=sizeof(s_stVidEncMpeg2PL)/sizeof(DI_VidEncProfileLevel_t);
			ePL = s_stVidEncMpeg2PL;
			break;
		case NEXUS_VideoCodec_eH264:
			idx=sizeof(s_stVidEncH264PL)/sizeof(DI_VidEncProfileLevel_t);
			ePL = s_stVidEncH264PL;
			break;
		default:
			idx=0;
			break;
	}

	if(idx <= 1)
		return -1;

	for(i=0;i<idx;i++)
	{
		if(ePL[i].eFormat == displayCfg->format && ePL[i].eFrameRate == videoEncoderConfig->frameRate)
		{
			videoEncoderConfig->bitrateMax = ePL[i].MaxBitrate;
			videoEncoderConfig->streamStructure.framesP=ePL[i].Pframe;
			videoEncoderConfig->streamStructure.framesB=ePL[i].Bframe;
			return 0;
		}
	}

	return -1;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] NEXUS_DisplayAspectRatio eAspectRatio
 * @return DI_VIDEO_AspectRatio_t
 * @par requirements
 * @see APIs
*/
DI_VIDEO_AspectRatio_t DRV_VIDEO_EncGetDiAspectRatio(HUINT32 ulDeviceId, NEXUS_AspectRatio eAspectRatio)
{
	DI_VIDEO_AspectRatio_t eDiAspectRatio;

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized == FALSE|| VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR_INVALID_PARAMETER;

	eDiAspectRatio=P_VIDEO_EncConvert2DIAspectRatio(eAspectRatio);

	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return eDiAspectRatio;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] NEXUS_VideoCodecLevel eCodecLevel
 * @return DI_TRANSCODER_VIDEO_CODEC_LEVEL
 * @par requirements
 * @see APIs
*/
DI_TRANSCODER_VIDEO_CODEC_LEVEL DRV_VIDEO_EncGetDiCodecLevel(HUINT32 ulDeviceId, NEXUS_VideoCodecLevel eCodecLevel)
{
	DI_TRANSCODER_VIDEO_CODEC_LEVEL eLevel = DI_TRANSCODER_VIDEO_CODEC_LEVEL_MAX;
	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized == FALSE|| VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR_INVALID_PARAMETER;

	eLevel = P_VIDEO_EncCovert2DICodecLevel(eCodecLevel);

	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return eLevel;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] NEXUS_VideoCodecProfile eCodecProfile
 * @return DI_TRANSCODER_VIDEO_CODEC_PROFILE
 * @par requirements
 * @see APIs
*/
DI_TRANSCODER_VIDEO_CODEC_PROFILE DRV_VIDEO_EncGetDiCodecProfile(HUINT32 ulDeviceId, NEXUS_VideoCodecProfile eCodecProfile)
{
	DI_TRANSCODER_VIDEO_CODEC_PROFILE eProfile=DI_TRANSCODER_VIDEO_CODEC_PROFILE_MAX;

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized == FALSE|| VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR_INVALID_PARAMETER;

	eProfile = P_VIDEO_EncCovert2DICodecProfile(eCodecProfile);

	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return eProfile;
}
/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] HUINT32 *min
 * @param[in] HUINT32 *max
 * @return success DRV_OK, error DRV_ERR_INVALID_PARAMETER
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_GetEncDelayRange(HUINT32 ulDeviceId, HUINT32 *min, HUINT32 *max)
{
	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized == FALSE|| VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR_INVALID_PARAMETER;

	NEXUS_VideoEncoder_GetDelayRange(s_stVideoEnc[ulDeviceId].hEncoder, &s_stVideoEnc[ulDeviceId].videoEncoderConfig,
			&s_stVideoEnc[ulDeviceId].videoEncoderStartConfig, &s_stVideoEnc[ulDeviceId].videoDelay);

	*min=s_stVideoEnc[ulDeviceId].videoDelay.min;
	*max=s_stVideoEnc[ulDeviceId].videoDelay.max;

	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return DRV_OK;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[out] NEXUS_VideoEncoderHandle *hEncoder
 * @return success DRV_OK, error DRV_ERR_INVALID_PARAMETER
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_GetEncHandle(HUINT32 ulDeviceId, NEXUS_VideoEncoderHandle *hEncoder)
{
	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE || VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR_INVALID_PARAMETER;

	*hEncoder = s_stVideoEnc[ulDeviceId].hEncoder;

	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return DRV_OK;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DI_ERR_OK, error DI_ERR_ERROR
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_EncInit(HUINT32 ulDeviceId)
{
	DRV_Error nRet = DRV_OK;
	char ucSemName[20];

	if(ulDeviceId>=NUM_VID_ENC)
	{
		PrintError("[%s] Required ulDeviceId =%d ",__func__,ulDeviceId);
		PrintError("is same or over than the number of video encoder %d\n",NUM_VID_ENC);
		return DRV_ERR;
	}

	if(s_stVideoEnc[ulDeviceId].bInitialized == TRUE)
	{
		PrintDebug("[%s] Current Device Id = %d is already Initialized\n",__func__,ulDeviceId);
		return DRV_ERR;
	}

	VK_MEM_Memset(ucSemName, 0x00, 20);
	VK_snprintf(ucSemName, 19, "drv_video_enc%d_drv", ulDeviceId);
	nRet = VK_SEM_Create(&s_stVideoEnc[ulDeviceId].ulSemId_DRV, ucSemName, DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("[%s] SEM create failed.\n\r", __func__);
		return DRV_ERR;
	}

	s_stVideoEnc[ulDeviceId].bEncoding=FALSE;
	s_stVideoEnc[ulDeviceId].bInitialized=TRUE;
	s_stVideoEnc[ulDeviceId].bFileOut=FALSE;
	s_stVideoEnc[ulDeviceId].bWatching=FALSE;
	s_stVideoEnc[ulDeviceId].hEncoder = NULL;

	return DRV_OK;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_EncUnInit(HUINT32 ulDeviceId)
{
	VK_SEM_Destroy(s_stVideoEnc[ulDeviceId].ulSemId_DRV);

	s_stVideoEnc[ulDeviceId].bInitialized=FALSE;

	return DRV_OK;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] HUINT32 ulDelay
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_EncStart(HUINT32 ulDeviceId, HUINT32 ulDelay)
{
	DRV_Error	nDrvErr=DRV_OK;
	NEXUS_Error	nexusError;
	DRV_DEMUX_PLAYBACK_MODE ePlaybackMode;
	HBOOL	bIsPlayback = FALSE;

	PrintEnter();

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		s_stVideoEnc[ulDeviceId].bEncoding == TRUE || VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR;

	if(s_stVideoEnc[ulDeviceId].bWatching == FALSE)
	{
		nexusError=NEXUS_VideoDecoder_Start(s_stVideoEnc[ulDeviceId].hDecoder , &s_stVideoEnc[ulDeviceId].startSettings);
		if(nexusError != NEXUS_SUCCESS)
		{
			PrintError("%s->NEXUS_VideoDecoder_Start(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nexusError, __LINE__, __FILE__);
			nDrvErr=DRV_ERR;
			goto done;
		}
		else
		{
			PrintDebug("%s->NEXUS_VideoDecoder_Start(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		}
	}

	NEXUS_VideoEncoder_GetSettings(s_stVideoEnc[ulDeviceId].hEncoder, &s_stVideoEnc[ulDeviceId].videoEncoderConfig);
	s_stVideoEnc[ulDeviceId].videoEncoderConfig.encoderDelay = ulDelay;
	NEXUS_VideoEncoder_SetSettings(s_stVideoEnc[ulDeviceId].hEncoder, &s_stVideoEnc[ulDeviceId].videoEncoderConfig);

	nexusError=NEXUS_VideoEncoder_Start(s_stVideoEnc[ulDeviceId].hEncoder, &s_stVideoEnc[ulDeviceId].videoEncoderStartConfig);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_VideoEncoder_Start(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nexusError, __LINE__, __FILE__);
		nDrvErr=DRV_ERR;
		goto done;
	}
	else
	{
		PrintDebug("%s->NEXUS_VideoEncoder_Start(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		s_stVideoEnc[ulDeviceId].bEncoding=TRUE;
	}

	bIsPlayback=DRV_DEMUX_IsPlaybackPidChannel(s_stVideoEnc[ulDeviceId].startSettings.pidChannel, &ePlaybackMode);

	if(bIsPlayback == TRUE)
		DRV_PVR_PLAY_VideoPidChannelSetting(s_stVideoEnc[ulDeviceId].startSettings.pidChannel, s_stVideoEnc[ulDeviceId].hDecoder, s_stVideoEnc[ulDeviceId].startSettings.codec);


done:
	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);

	return nDrvErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_EncStop(HUINT32 ulDeviceId)
{
	DRV_Error	nDrvErr=DRV_OK;
	NEXUS_VideoEncoderStopSettings stopSettings;

	PrintEnter();

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		s_stVideoEnc[ulDeviceId].bEncoding == FALSE || VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
	{
		PrintError("[%s] ulDeviceId error\n", __FUNCTION__);
		return DRV_ERR;
	}

	if(s_stVideoEnc[ulDeviceId].bWatching == FALSE)
	{
		NEXUS_VideoDecoder_Stop(s_stVideoEnc[ulDeviceId].hDecoder);
	}
#if (NEXUS_VERSION < 1100)
	NEXUS_VideoEncoder_Stop(s_stVideoEnc[ulDeviceId].hEncoder);
#else
	NEXUS_VideoEncoder_GetDefaultStopSettings(&stopSettings);
	stopSettings.mode= NEXUS_VideoEncoderStopMode_eNormal;
	NEXUS_VideoEncoder_Stop(s_stVideoEnc[ulDeviceId].hEncoder,&stopSettings);
#endif


	s_stVideoEnc[ulDeviceId].bEncoding=FALSE;
	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return nDrvErr;

}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_EncRestart(HUINT32 ulDeviceId,HUINT32 ulDemuxId)
{
	NEXUS_Error	nexusError;
	HBOOL	bIsPlayback = FALSE;
	DRV_DEMUX_PLAYBACK_MODE ePlaybackMode;
	DRV_Error	nDrvErr=DRV_OK;

	PrintEnter();

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		s_stVideoEnc[ulDeviceId].bEncoding == FALSE || VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
	{
		return DRV_ERR;
	}

	NEXUS_VideoDecoder_Stop(s_stVideoEnc[ulDeviceId].hDecoder);

	if(s_stVideoEnc[ulDeviceId].bWatching == FALSE)
	{
		if(ulDeviceId == 0)
		{
			s_stVideoEnc[ulDeviceId].startSettings.pidChannel = DRV_DEMUX_GetPidChannelForTranscode(ulDemuxId, DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_1);
		}
		else
		{
			s_stVideoEnc[ulDeviceId].startSettings.pidChannel = DRV_DEMUX_GetPidChannelForTranscode(ulDemuxId, DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_2);
		}
		s_stVideoEnc[ulDeviceId].startSettings.stcChannel = DRV_DEMUX_GetSTCChannel(ulDeviceId+1);
	}

	nexusError=NEXUS_VideoDecoder_Start(s_stVideoEnc[ulDeviceId].hDecoder , &s_stVideoEnc[ulDeviceId].startSettings);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_VideoDecoder_Start(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nexusError, __LINE__, __FILE__);

		nDrvErr=DRV_ERR;
		goto done;
	}

	bIsPlayback=DRV_DEMUX_IsPlaybackPidChannel(s_stVideoEnc[ulDeviceId].startSettings.pidChannel, &ePlaybackMode);

	if(bIsPlayback == TRUE)
	{
		DRV_PVR_PLAY_VideoPidChannelSetting(s_stVideoEnc[ulDeviceId].startSettings.pidChannel, s_stVideoEnc[ulDeviceId].hDecoder, s_stVideoEnc[ulDeviceId].startSettings.codec);
	}

done:
	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return nDrvErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DI_VIDEO_ENC_Set_t *Settings
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_EncSettings(HUINT32 ulDeviceId, DRV_VIDEO_ENC_Set_t *Settings)
{
	DRV_Error	nDrvErr=DRV_OK;
	NEXUS_VideoWindowMadSettings windowMadSettings;
	int			ret = 0;
	HUINT32		ulDecoderIndex = DRV_VIDEO_DECODER_INDEX_0;

	PrintEnter();

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR;

	NEXUS_Display_GetDefaultSettings(&s_stVideoEnc[ulDeviceId].displaySettings);
	s_stVideoEnc[ulDeviceId].displaySettings.displayType = NEXUS_DisplayType_eAuto;
	s_stVideoEnc[ulDeviceId].displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eEncoder;
	s_stVideoEnc[ulDeviceId].displaySettings.format = DRV_VIDEO_GetFormatType(0,Settings->eRes);
	s_stVideoEnc[ulDeviceId].displaySettings.frameRateMaster = NULL;
	s_stVideoEnc[ulDeviceId].displaySettings.aspectRatio = P_VIDEO_EncCovert2NexusAspectRatio(Settings->eAspectRatio);

	s_stVideoEnc[ulDeviceId].hEncoder = NEXUS_VideoEncoder_Open(ulDeviceId,NULL);

	if(ulDeviceId == 0)
	{
		s_stVideoEnc[ulDeviceId].hDisplay = NEXUS_Display_Open(ulDeviceId+VID_ENC_DISP_IDX_EXT,&s_stVideoEnc[ulDeviceId].displaySettings);
		s_stVideoEnc[ulDeviceId].hWindow = NEXUS_VideoWindow_Open(s_stVideoEnc[ulDeviceId].hDisplay, VID_ENC_WIN_IDX);

		if(Settings->eTransType == DI_TRANSCODER_SOURCE_WATCH)
		{
			DRV_VIDEO_GetVideoInputHandle(DRV_VIDEO_DECODER_INDEX_0, &s_stVideoEnc[ulDeviceId].hInput);
			s_stVideoEnc[ulDeviceId].bWatching = TRUE;
		}
		else
		{
			ulDecoderIndex = DRV_VIDEO_DECODER_INDEX_2;
			s_stVideoEnc[ulDeviceId].hDecoder = NEXUS_VideoDecoder_Open(ulDecoderIndex, NULL);
			s_stVideoEnc[ulDeviceId].hInput=NEXUS_VideoDecoder_GetConnector(s_stVideoEnc[ulDeviceId].hDecoder);
			s_stVideoEnc[ulDeviceId].bWatching = FALSE;
		}
	}
	else
	{
		s_stVideoEnc[ulDeviceId].hDisplay=NEXUS_Display_Open(VID_ENC_DISP_IDX,&s_stVideoEnc[ulDeviceId].displaySettings);
		s_stVideoEnc[ulDeviceId].hWindow=NEXUS_VideoWindow_Open(s_stVideoEnc[ulDeviceId].hDisplay, VID_ENC_WIN_IDX);
		if(Settings->eTransType == DI_TRANSCODER_SOURCE_WATCH)

		{
			DRV_VIDEO_GetVideoInputHandle(DRV_VIDEO_DECODER_INDEX_0, &s_stVideoEnc[ulDeviceId].hInput);
			s_stVideoEnc[ulDeviceId].bWatching = TRUE;
		}
		else
		{
#if defined(CONFIG_2ND_ENCODER_USE_2ND_VIDEO_CORE)
			ulDecoderIndex = DRV_VIDEO_DECODER_INDEX_3;
#else
			ulDecoderIndex = DRV_VIDEO_DECODER_INDEX_1;
#endif
			s_stVideoEnc[ulDeviceId].hDecoder = NEXUS_VideoDecoder_Open(ulDecoderIndex, NULL);
			s_stVideoEnc[ulDeviceId].hInput = NEXUS_VideoDecoder_GetConnector(s_stVideoEnc[ulDeviceId].hDecoder);
			s_stVideoEnc[ulDeviceId].bWatching = FALSE;
		}
	}

	PrintDebug("%s->NEXUS_VideoDecoder_Open(): Decoder index(%d), hDecoder(0x%x)\n", __FUNCTION__, s_stVideoEnc[ulDeviceId].hDecoder, ulDecoderIndex);

	NEXUS_VideoWindow_AddInput(s_stVideoEnc[ulDeviceId].hWindow, s_stVideoEnc[ulDeviceId].hInput);

	if(s_stVideoEnc[ulDeviceId].bWatching == FALSE)
	{
		NEXUS_VideoDecoder_GetDefaultStartSettings(&s_stVideoEnc[ulDeviceId].startSettings);
		s_stVideoEnc[ulDeviceId].startSettings.codec = DRV_VIDEO_GetCodecType(0,Settings->eDecCodec);

		if(ulDeviceId == 0)
			s_stVideoEnc[ulDeviceId].startSettings.pidChannel = DRV_DEMUX_GetPidChannelForTranscode(Settings->ulDemuxId, DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_1);
		else
			s_stVideoEnc[ulDeviceId].startSettings.pidChannel = DRV_DEMUX_GetPidChannelForTranscode(Settings->ulDemuxId, DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_2);

		s_stVideoEnc[ulDeviceId].startSettings.stcChannel = DRV_DEMUX_GetSTCChannel(ulDeviceId+1);
	}

	NEXUS_VideoWindow_GetMadSettings(s_stVideoEnc[ulDeviceId].hWindow,&windowMadSettings);
	windowMadSettings.deinterlace = true;
	NEXUS_VideoWindow_SetMadSettings(s_stVideoEnc[ulDeviceId].hWindow,&windowMadSettings);

	NEXUS_VideoEncoder_GetSettings(s_stVideoEnc[ulDeviceId].hEncoder, &s_stVideoEnc[ulDeviceId].videoEncoderConfig);
	NEXUS_VideoEncoder_GetDefaultStartSettings(&s_stVideoEnc[ulDeviceId].videoEncoderStartConfig);

	s_stVideoEnc[ulDeviceId].videoEncoderConfig.variableFrameRate = false;
	s_stVideoEnc[ulDeviceId].videoEncoderConfig.frameRate = DRV_VIDEO_GetFrameRateType(0,Settings->eFrameRate);
#if (NEXUS_VERSION < 1200)
	s_stVideoEnc[ulDeviceId].videoEncoderConfig.streamStructure.trackInput = false;
#endif
	s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.codec = DRV_VIDEO_GetCodecType(0,Settings->eCodec);
	s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.input = s_stVideoEnc[ulDeviceId].hDisplay;
	s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.stcChannel = DRV_DEMUX_GetSTCChannelTranscode(ulDeviceId);
	if( (Settings->eRes == DI_VIDEO_HD_RESOLUTION_480P) || (Settings->eRes == DI_VIDEO_HD_RESOLUTION_576P)
		|| (Settings->eRes == DI_VIDEO_HD_RESOLUTION_720P) || (Settings->eRes == DI_VIDEO_HD_RESOLUTION_720P_50HZ)
		|| (Settings->eRes == DI_VIDEO_HD_RESOLUTION_1080P) || (Settings->eRes == DI_VIDEO_HD_RESOLUTION_1080P_24HZ)
		|| (Settings->eRes == DI_VIDEO_HD_RESOLUTION_1080P_25HZ) || (Settings->eRes == DI_VIDEO_HD_RESOLUTION_1080P_30HZ)
		|| (Settings->eRes == DI_VIDEO_HD_RESOLUTION_1080P_50HZ) )
	{
	    s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.interlaced = false;
	}
	else
	{
		s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.interlaced = true;
	}

	if(Settings->stCustomRes.bCustom == TRUE)
	{
		NEXUS_Display_GetDefaultCustomFormatSettings(&s_stVideoEnc[ulDeviceId].customFormatSettings);
		s_stVideoEnc[ulDeviceId].customFormatSettings.width = Settings->stCustomRes.ulWidth;
		s_stVideoEnc[ulDeviceId].customFormatSettings.height = Settings->stCustomRes.ulHeight;
		s_stVideoEnc[ulDeviceId].customFormatSettings.refreshRate = Settings->stCustomRes.ulRefreshRate;
		s_stVideoEnc[ulDeviceId].customFormatSettings.interlaced = Settings->stCustomRes.bInterlaced;
		s_stVideoEnc[ulDeviceId].customFormatSettings.aspectRatio = P_VIDEO_EncCovert2NexusAspectRatio(Settings->eAspectRatio);
		s_stVideoEnc[ulDeviceId].customFormatSettings.dropFrameAllowed = true;
		if(s_stVideoEnc[ulDeviceId].customFormatSettings.aspectRatio == NEXUS_DisplayAspectRatio_eSar)
		{
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.x = Settings->stCustomRes.ulSampleAspectRatioX;
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.y = Settings->stCustomRes.ulSampleAspectRatioY;
		}
		else if(s_stVideoEnc[ulDeviceId].customFormatSettings.aspectRatio == NEXUS_DisplayAspectRatio_e4x3)
		{
			s_stVideoEnc[ulDeviceId].customFormatSettings.aspectRatio = NEXUS_DisplayAspectRatio_eSar;
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.x = 8;
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.y = 9;
		}
		else
		{
			s_stVideoEnc[ulDeviceId].customFormatSettings.aspectRatio = NEXUS_DisplayAspectRatio_eSar;
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.x = 32;
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.y = 27;
		}
		NEXUS_Display_SetCustomFormatSettings(s_stVideoEnc[ulDeviceId].hDisplay, NEXUS_VideoFormat_eCustom2, &s_stVideoEnc[ulDeviceId].customFormatSettings);

	}
	else
	{
		NEXUS_Display_GetSettings(s_stVideoEnc[ulDeviceId].hDisplay,&s_stVideoEnc[ulDeviceId].displaySettings);
		s_stVideoEnc[ulDeviceId].displaySettings.format = DRV_VIDEO_GetFormatType(0,Settings->eRes);
		s_stVideoEnc[ulDeviceId].displaySettings.aspectRatio = DRV_VIDEO_GetAspectRatio(0,Settings->eAspectRatio);
		NEXUS_Display_SetSettings(s_stVideoEnc[ulDeviceId].hDisplay,&s_stVideoEnc[ulDeviceId].displaySettings);
	}

	if(Settings->bDefaultProfile == TRUE)
	{
		ret=P_VIDEO_EncGetPLB(&s_stVideoEnc[ulDeviceId].displaySettings, &s_stVideoEnc[ulDeviceId].videoEncoderConfig, &s_stVideoEnc[ulDeviceId].videoEncoderStartConfig);
		if(ret == -1)
		{
			PrintError("%s->Not support framerate and video format (%d) (%s)\n", __FUNCTION__, __LINE__, __FILE__);
			nDrvErr=DRV_ERR;
			goto done;
		}
	}
	else
	{

		s_stVideoEnc[ulDeviceId].videoEncoderConfig.bitrateMax = Settings->ulMaxBitRate;
		if(Settings->stCustomIPB.bCustomIPB == TRUE)
		{
			s_stVideoEnc[ulDeviceId].videoEncoderConfig.streamStructure.framesP = Settings->stCustomIPB.ulFrameP;
			s_stVideoEnc[ulDeviceId].videoEncoderConfig.streamStructure.framesB = Settings->stCustomIPB.ulFrameB;
		}
		s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.profile = P_VIDEO_EncCovert2NexusCodecProfile(Settings->eProfile);
		s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.level = P_VIDEO_EncCovert2NexusCodecLevel(Settings->eLevel);
	}

#if (NEXUS_VERSION >= 1502)
#if defined (VIDEO_ENC_RATE_BUFFER_DELAY)
	/* It's defined in di_config.h */
	s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.rateBufferDelay = VIDEO_ENC_RATE_BUFFER_DELAY;
#endif
#endif
	NEXUS_VideoEncoder_SetSettings(s_stVideoEnc[ulDeviceId].hEncoder, &s_stVideoEnc[ulDeviceId].videoEncoderConfig);

done:
	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return nDrvErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_EncUnsettings(HUINT32 ulDeviceId)
{
	NEXUS_VideoWindow_RemoveInput(s_stVideoEnc[ulDeviceId].hWindow, s_stVideoEnc[ulDeviceId].hInput);

	if(s_stVideoEnc[ulDeviceId].bWatching == FALSE)
	{
		NEXUS_VideoInput_Shutdown(s_stVideoEnc[ulDeviceId].hInput);
		NEXUS_VideoDecoder_Close(s_stVideoEnc[ulDeviceId].hDecoder);
	}

	NEXUS_VideoWindow_Close(s_stVideoEnc[ulDeviceId].hWindow);
	NEXUS_Display_Close(s_stVideoEnc[ulDeviceId].hDisplay);
	NEXUS_VideoEncoder_Close(s_stVideoEnc[ulDeviceId].hEncoder);
	s_stVideoEnc[ulDeviceId].hEncoder = NULL;
	return DRV_OK;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DRV_VIDEO_ENC_Set_t *Settings
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_UpdateEncSettings(HUINT32 ulDeviceId, DRV_VIDEO_ENC_Set_t *Settings)
{

	DRV_Error	nDrvErr=DRV_OK;
	NEXUS_VideoFormat eFormat;
	NEXUS_VideoFrameRate eFrameRate;
	int ret=0;

	PrintEnter();

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		s_stVideoEnc[ulDeviceId].bEncoding == FALSE ||
		VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR;

	if( (Settings->eRes == DI_VIDEO_HD_RESOLUTION_480P) || (Settings->eRes == DI_VIDEO_HD_RESOLUTION_576P)
		|| (Settings->eRes == DI_VIDEO_HD_RESOLUTION_720P) || (Settings->eRes == DI_VIDEO_HD_RESOLUTION_720P_50HZ)
		|| (Settings->eRes == DI_VIDEO_HD_RESOLUTION_1080P) || (Settings->eRes == DI_VIDEO_HD_RESOLUTION_1080P_24HZ)
		|| (Settings->eRes == DI_VIDEO_HD_RESOLUTION_1080P_25HZ) || (Settings->eRes == DI_VIDEO_HD_RESOLUTION_1080P_30HZ)
		|| (Settings->eRes == DI_VIDEO_HD_RESOLUTION_1080P_50HZ) )
	{
		s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.interlaced = false;
	}
	else
	{
		s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.interlaced = true;
	}

	/* Update Display settings for Encoder */
	if(Settings->stCustomRes.bCustom == TRUE)
	{
		NEXUS_Display_GetDefaultCustomFormatSettings(&s_stVideoEnc[ulDeviceId].customFormatSettings);
		s_stVideoEnc[ulDeviceId].customFormatSettings.width = Settings->stCustomRes.ulWidth;
		s_stVideoEnc[ulDeviceId].customFormatSettings.height = Settings->stCustomRes.ulHeight;
		s_stVideoEnc[ulDeviceId].customFormatSettings.refreshRate = Settings->stCustomRes.ulRefreshRate;
		s_stVideoEnc[ulDeviceId].customFormatSettings.interlaced = Settings->stCustomRes.bInterlaced;
		s_stVideoEnc[ulDeviceId].customFormatSettings.aspectRatio = P_VIDEO_EncCovert2NexusAspectRatio(Settings->eAspectRatio);
		s_stVideoEnc[ulDeviceId].customFormatSettings.dropFrameAllowed = true;
		if(s_stVideoEnc[ulDeviceId].customFormatSettings.aspectRatio == NEXUS_DisplayAspectRatio_eSar)
		{
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.x = Settings->stCustomRes.ulSampleAspectRatioX;
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.y = Settings->stCustomRes.ulSampleAspectRatioY;
		}
		else if(s_stVideoEnc[ulDeviceId].customFormatSettings.aspectRatio == NEXUS_DisplayAspectRatio_e4x3)
		{
			s_stVideoEnc[ulDeviceId].customFormatSettings.aspectRatio = NEXUS_DisplayAspectRatio_eSar;
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.x = 8;
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.y = 9;
		}
		else
		{
			s_stVideoEnc[ulDeviceId].customFormatSettings.aspectRatio = NEXUS_DisplayAspectRatio_eSar;
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.x = 32;
			s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.y = 27;
		}
		NEXUS_Display_SetCustomFormatSettings(s_stVideoEnc[ulDeviceId].hDisplay, NEXUS_VideoFormat_eCustom2, &s_stVideoEnc[ulDeviceId].customFormatSettings);

	}
	else
	{
		NEXUS_Display_GetSettings(s_stVideoEnc[ulDeviceId].hDisplay,&s_stVideoEnc[ulDeviceId].displaySettings);
		eFormat = DRV_VIDEO_GetFormatType(0,Settings->eRes);
		if(s_stVideoEnc[ulDeviceId].displaySettings.format != eFormat)
		{
			s_stVideoEnc[ulDeviceId].displaySettings.format = eFormat;
			NEXUS_Display_SetSettings(s_stVideoEnc[ulDeviceId].hDisplay,&s_stVideoEnc[ulDeviceId].displaySettings);
		}
	}

	/* Update Encoder settings */
	NEXUS_VideoEncoder_GetSettings(s_stVideoEnc[ulDeviceId].hEncoder, &s_stVideoEnc[ulDeviceId].videoEncoderConfig);

	if(Settings->bDefaultProfile == TRUE)
	{
		ret=P_VIDEO_EncGetB(&s_stVideoEnc[ulDeviceId].displaySettings, &s_stVideoEnc[ulDeviceId].videoEncoderConfig, &s_stVideoEnc[ulDeviceId].videoEncoderStartConfig);
		if(ret == -1)
		{
			PrintError("%s->Not support framerate and video format (%d) (%s)\n", __FUNCTION__, __LINE__, __FILE__);
			nDrvErr=DRV_ERR;
		}
	}
	else
	{
		/* Change FrameRate */
		eFrameRate = DRV_VIDEO_GetFrameRateType(0,Settings->eFrameRate);
		if(s_stVideoEnc[ulDeviceId].videoEncoderConfig.frameRate != eFrameRate)
			s_stVideoEnc[ulDeviceId].videoEncoderConfig.frameRate = eFrameRate;

		/* Change BitRate */
		if(s_stVideoEnc[ulDeviceId].videoEncoderConfig.bitrateMax != Settings->ulMaxBitRate)
			s_stVideoEnc[ulDeviceId].videoEncoderConfig.bitrateMax = Settings->ulMaxBitRate;

		/* Change P/B counter */
		if(Settings->stCustomIPB.bCustomIPB == TRUE)
		{
			if((s_stVideoEnc[ulDeviceId].videoEncoderConfig.streamStructure.framesP != Settings->stCustomIPB.ulFrameP)
					&& Settings->stCustomIPB.ulFrameB <= MAX_NUM_OF_FRAMEP)
				s_stVideoEnc[ulDeviceId].videoEncoderConfig.streamStructure.framesP = Settings->stCustomIPB.ulFrameP;

			if((s_stVideoEnc[ulDeviceId].videoEncoderConfig.streamStructure.framesB != Settings->stCustomIPB.ulFrameB)
					&& Settings->stCustomIPB.ulFrameB < MAX_NUM_OF_FRAMEB)
				s_stVideoEnc[ulDeviceId].videoEncoderConfig.streamStructure.framesB = Settings->stCustomIPB.ulFrameB;
		}
	}

	NEXUS_VideoEncoder_SetSettings(s_stVideoEnc[ulDeviceId].hEncoder, &s_stVideoEnc[ulDeviceId].videoEncoderConfig);

	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return nDrvErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] DRV_VIDEO_ENC_Set_t *Settings
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_UpdateEncGetSettings(HUINT32 ulDeviceId, DRV_VIDEO_ENC_Set_t *Settings)
{
	PrintEnter();

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		s_stVideoEnc[ulDeviceId].bEncoding == FALSE ||
		VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR;

	Settings->ulMaxBitRate=s_stVideoEnc[ulDeviceId].videoEncoderConfig.bitrateMax;
	Settings->stCustomIPB.ulFrameP=s_stVideoEnc[ulDeviceId].videoEncoderConfig.streamStructure.framesP;
	Settings->stCustomIPB.ulFrameB=s_stVideoEnc[ulDeviceId].videoEncoderConfig.streamStructure.framesB;

	/* Update Display settings for Encoder */
	if(Settings->stCustomRes.bCustom == TRUE)
	{
		Settings->stCustomRes.ulWidth =	s_stVideoEnc[ulDeviceId].customFormatSettings.width;
		Settings->stCustomRes.ulHeight = s_stVideoEnc[ulDeviceId].customFormatSettings.height;
		Settings->stCustomRes.ulRefreshRate = s_stVideoEnc[ulDeviceId].customFormatSettings.refreshRate;
		Settings->stCustomRes.bInterlaced = s_stVideoEnc[ulDeviceId].customFormatSettings.interlaced;
		Settings->stCustomRes.ulSampleAspectRatioX = s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.x;
		Settings->stCustomRes.ulSampleAspectRatioY = s_stVideoEnc[ulDeviceId].customFormatSettings.sampleAspectRatio.y;

	}

	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return DRV_OK;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] HUINT32 ulDemuxId
 * @param[in] DI_VIDEO_FORMAT eVideoCodec
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_DecProbeStart(HUINT32 ulDeviceId, HUINT32 ulDemuxId, DI_VIDEO_FORMAT eVideoCodec)
{

	DRV_Error	nDrvErr = DRV_OK;
	NEXUS_Error	nexusError;
	HUINT32		ulDeviceIdForTranscoder = DRV_VIDEO_DECODER_INDEX_0;
	NEXUS_VideoDecoderSettings decoderSettings;

	PrintEnter();

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR;

	if(ulDeviceId == 0)
	{
		ulDeviceIdForTranscoder = DRV_VIDEO_DECODER_INDEX_2; /* Third Video Decoder is first decoder for Transcoding */
	}
	else
	{
#if defined(CONFIG_2ND_ENCODER_USE_2ND_VIDEO_CORE)
		ulDeviceIdForTranscoder = DRV_VIDEO_DECODER_INDEX_3;
#else
		ulDeviceIdForTranscoder = DRV_VIDEO_DECODER_INDEX_1;
#endif
	}

	NEXUS_Display_GetDefaultSettings(&s_stVideoEnc[ulDeviceId].displaySettings);
	s_stVideoEnc[ulDeviceId].displaySettings.displayType = NEXUS_DisplayType_eAuto;
	s_stVideoEnc[ulDeviceId].displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eEncoder;
	s_stVideoEnc[ulDeviceId].displaySettings.frameRateMaster=NULL;

	if(ulDeviceId == 0)
		s_stVideoEnc[ulDeviceId].hDisplay=NEXUS_Display_Open(ulDeviceId+VID_ENC_DISP_IDX_EXT,&s_stVideoEnc[ulDeviceId].displaySettings);
	else
		s_stVideoEnc[ulDeviceId].hDisplay=NEXUS_Display_Open(VID_ENC_DISP_IDX,&s_stVideoEnc[ulDeviceId].displaySettings);

	s_stVideoEnc[ulDeviceId].hWindow=NEXUS_VideoWindow_Open(s_stVideoEnc[ulDeviceId].hDisplay, VID_ENC_WIN_IDX);

	s_stVideoEnc[ulDeviceId].hDecoder=NEXUS_VideoDecoder_Open(ulDeviceIdForTranscoder, NULL);

	s_stVideoEnc[ulDeviceId].hInput=NEXUS_VideoDecoder_GetConnector(s_stVideoEnc[ulDeviceId].hDecoder);

	nexusError=NEXUS_VideoWindow_AddInput(s_stVideoEnc[ulDeviceId].hWindow, s_stVideoEnc[ulDeviceId].hInput);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_VideoWindow_AddInput(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nexusError, __LINE__, __FILE__);
		nDrvErr=DRV_ERR;
		goto err_input;
	}
	else
	{
		PrintDebug("%s->NEXUS_VideoWindow_AddInput(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	NEXUS_VideoDecoder_GetDefaultStartSettings(&s_stVideoEnc[ulDeviceId].startSettings);

	s_stVideoEnc[ulDeviceId].startSettings.codec = DRV_VIDEO_GetCodecType(0,eVideoCodec);

	if(ulDeviceId == 0)
		s_stVideoEnc[ulDeviceId].startSettings.pidChannel = DRV_DEMUX_GetPidChannelForTranscode(ulDemuxId, DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_1);
	else
		s_stVideoEnc[ulDeviceId].startSettings.pidChannel = DRV_DEMUX_GetPidChannelForTranscode(ulDemuxId, DRV_DEMUX_CHANNELTYPE_VIDEO_TRANS_2);

	s_stVideoEnc[ulDeviceId].startSettings.stcChannel = DRV_DEMUX_GetSTCChannel(ulDeviceId+1);

	NEXUS_VideoDecoder_GetSettings(s_stVideoEnc[ulDeviceId].hDecoder, &decoderSettings);
	decoderSettings.sourceChanged.callback=P_VIDEO_DecSourceChangedCallBack;
	decoderSettings.sourceChanged.param=ulDeviceId;
	decoderSettings.streamChanged.callback=P_VIDEO_DecStreamChangedCallBack;
	decoderSettings.streamChanged.param=ulDeviceId;
	decoderSettings.firstPtsPassed.callback=P_VIDEO_DecFirstPtsPassedCallBack;
	decoderSettings.firstPtsPassed.param=ulDeviceId;
	nexusError=NEXUS_VideoDecoder_SetSettings(s_stVideoEnc[ulDeviceId].hDecoder, &decoderSettings);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_VideoDecoder_SetSetting(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nexusError, __LINE__, __FILE__);
		nDrvErr=DRV_ERR;
		goto err_decoder;
	}
	else
	{
		PrintDebug("%s->NEXUS_VideoDecoder_SetSettings(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}
	nexusError=NEXUS_VideoDecoder_Start(s_stVideoEnc[ulDeviceId].hDecoder , &s_stVideoEnc[ulDeviceId].startSettings);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("%s->NEXUS_VideoDecoder_Start(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, nexusError, __LINE__, __FILE__);
		nDrvErr=DRV_ERR;
		goto err_decoder;
	}
	else
	{
		PrintDebug("%s->NEXUS_VideoDecoder_Start(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return nDrvErr;

err_decoder:
	NEXUS_VideoWindow_RemoveInput(s_stVideoEnc[ulDeviceId].hWindow, s_stVideoEnc[ulDeviceId].hInput);
err_input:
	NEXUS_VideoInput_Shutdown(s_stVideoEnc[ulDeviceId].hInput);
	NEXUS_VideoDecoder_Close(s_stVideoEnc[ulDeviceId].hDecoder);
	NEXUS_VideoWindow_Close(s_stVideoEnc[ulDeviceId].hWindow);
	NEXUS_Display_Close(s_stVideoEnc[ulDeviceId].hDisplay);
	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return nDrvErr;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_DecProbeStop(HUINT32 ulDeviceId)
{
	PrintEnter();

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR;

	NEXUS_VideoDecoder_Stop(s_stVideoEnc[ulDeviceId].hDecoder);

	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return DRV_OK;

}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DRV_ERR, error DRV_OK
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_DecProbeClose(HUINT32 ulDeviceId)
{
	PrintEnter();
	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR;

	NEXUS_VideoWindow_RemoveInput(s_stVideoEnc[ulDeviceId].hWindow, s_stVideoEnc[ulDeviceId].hInput);
	NEXUS_VideoInput_Shutdown(s_stVideoEnc[ulDeviceId].hInput);
	NEXUS_VideoDecoder_Close(s_stVideoEnc[ulDeviceId].hDecoder);
	NEXUS_VideoWindow_Close(s_stVideoEnc[ulDeviceId].hWindow);
	NEXUS_Display_Close(s_stVideoEnc[ulDeviceId].hDisplay);

	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return DRV_OK;

}

/**
 * @param[in] HUINT32 ulDeviceId
 * @param[in] NEXUS_VideoDecoderStreamInformation *StreamInfo
 * @param[in] NEXUS_VideoDecoderStatus *Status
 * @return no return value
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_GetDecSequenceInfo(HUINT32 ulDeviceId, NEXUS_VideoDecoderStreamInformation *StreamInfo, NEXUS_VideoDecoderStatus *Status)
{
	DRV_Error	nDrvErr=DRV_OK;
	NEXUS_Error nexusError;

	PrintEnter();

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		VK_SEM_Get(s_stVideoEnc[ulDeviceId].ulSemId_DRV)!=VK_OK)
		return DRV_ERR;


	nexusError = NEXUS_VideoDecoder_GetStreamInformation(s_stVideoEnc[ulDeviceId].hDecoder, StreamInfo);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] can't get stream information.. (%d)\n",__func__,nexusError);
		nDrvErr=DRV_ERR;
		goto done;
	}
	nexusError = NEXUS_VideoDecoder_GetStatus(s_stVideoEnc[ulDeviceId].hDecoder, Status);
	if(nexusError != NEXUS_SUCCESS)
	{
		PrintError("[%s] can't get decoder status.. (%d)\n",__func__,nexusError);
		nDrvErr=DRV_ERR;
	}

done:
	PrintExit();
	VK_SEM_Release(s_stVideoEnc[ulDeviceId].ulSemId_DRV);
	return nDrvErr;

}

/**
 * @param[in] pfnDRV_VidEncCallback_Notify pfnNotify
 * @return void
 * @par requirements
 * @see APIs
*/
void DRV_VIDEO_EncRegisterEventCallback(pfnDRV_VidEncCallback_Notify pfnNotify)
{
	VideoEncCallback = pfnNotify;
}

/**
 * @param[in] HUINT32 ulDeviceId
 * @return success DRV_OK, error DRV_ERR
 * @par requirements
 * @see APIs
*/
DRV_Error DRV_VIDEO_EncShowStatus(HUINT32 ulDeviceId)
{
	NEXUS_VideoEncoderStatus	EncoderStatus;
	NEXUS_VideoEncoderSettings	videoEncoderConfig;
	NEXUS_DisplaySettings displaySettings;
	char* strFrameRate[NEXUS_VideoFrameRate_eMax]={
		"NEXUS_VideoFrameRate_eUnknown = 0",
		"NEXUS_VideoFrameRate_e23_976",
		"NEXUS_VideoFrameRate_e24",
		"NEXUS_VideoFrameRate_e25",
		"NEXUS_VideoFrameRate_e29_97",
		"NEXUS_VideoFrameRate_e30",
		"NEXUS_VideoFrameRate_e50",
		"NEXUS_VideoFrameRate_e59_94",
		"NEXUS_VideoFrameRate_e60",
		"NEXUS_VideoFrameRate_e14_985",
		"NEXUS_VideoFrameRate_e7_493",
		"NEXUS_VideoFrameRate_e10",
		"NEXUS_VideoFrameRate_e15",
		"NEXUS_VideoFrameRate_e20"
    };
	char* strCodec[NEXUS_VideoCodec_eMax]={
		"NEXUS_VideoCodec_eUnknown = 0",     /* unknown/not supported video codec */
		"NEXUS_VideoCodec_eMpeg1",           /* MPEG-1 Video (ISO/IEC 11172-2) */
		"NEXUS_VideoCodec_eMpeg2",           /* MPEG-2 Video (ISO/IEC 13818-2) */
		"NEXUS_VideoCodec_eMpeg4Part2",      /* MPEG-4 Part 2 Video */
		"NEXUS_VideoCodec_eH263",            /* H.263 Video. The value of the enum is not based on PSI standards. */
		"NEXUS_VideoCodec_eH264",            /* H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC */
		"NEXUS_VideoCodec_eH264_Svc",        /* Scalable Video Codec extension of H.264 */
		"NEXUS_VideoCodec_eH264_Mvc",        /* Multi View Coding extension of H.264 */
		"NEXUS_VideoCodec_eVc1",             /* VC-1 Advanced Profile */
		"NEXUS_VideoCodec_eVc1SimpleMain",   /* VC-1 Simple & Main Profile */
		"NEXUS_VideoCodec_eDivx311",         /* DivX 3.11 coded video */
		"NEXUS_VideoCodec_eAvs",             /* AVS coded video */
		"NEXUS_VideoCodec_eRv40",            /* RV 4.0 coded video */
		"NEXUS_VideoCodec_eVp6",             /* VP6 coded video */
		"NEXUS_VideoCodec_eVp7",             /* VP7 coded video */
		"NEXUS_VideoCodec_eVp8",             /* VP8 coded video */
		"NEXUS_VideoCodec_eSpark",           /* H.263 Sorenson Spark coded video */
		"NEXUS_VideoCodec_eMotionJpeg"      /* Motion Jpeg video codec */
	};

	char* strCodecProfile[NEXUS_VideoCodecProfile_eMax]={
	   "NEXUS_VideoCodecProfile_eUnknown",
	   "NEXUS_VideoCodecProfile_eSimple",
	   "NEXUS_VideoCodecProfile_eMain",
	   "NEXUS_VideoCodecProfile_eHigh",
	   "NEXUS_VideoCodecProfile_eAdvanced",
	   "NEXUS_VideoCodecProfile_eJizhun",
	   "NEXUS_VideoCodecProfile_eSnrScalable",
	   "NEXUS_VideoCodecProfile_eSpatiallyScalable",
	   "NEXUS_VideoCodecProfile_eAdvancedSimple",
	   "NEXUS_VideoCodecProfile_eBaseline"
	};

	char* strCodecLevel[NEXUS_VideoCodecLevel_eMax]={
	   "NEXUS_VideoCodecLevel_eUnknown",
	   "NEXUS_VideoCodecLevel_e00",
	   "NEXUS_VideoCodecLevel_e10",
	   "NEXUS_VideoCodecLevel_e1B",
	   "NEXUS_VideoCodecLevel_e11",
	   "NEXUS_VideoCodecLevel_e12",
	   "NEXUS_VideoCodecLevel_e13",
	   "NEXUS_VideoCodecLevel_e20",
	   "NEXUS_VideoCodecLevel_e21",
	   "NEXUS_VideoCodecLevel_e22",
	   "NEXUS_VideoCodecLevel_e30",
	   "NEXUS_VideoCodecLevel_e31",
	   "NEXUS_VideoCodecLevel_e32",
	   "NEXUS_VideoCodecLevel_e40",
	   "NEXUS_VideoCodecLevel_e41",
	   "NEXUS_VideoCodecLevel_e42",
	   "NEXUS_VideoCodecLevel_e50",
	   "NEXUS_VideoCodecLevel_e51",
	   "NEXUS_VideoCodecLevel_e60",
	   "NEXUS_VideoCodecLevel_e62",
	   "NEXUS_VideoCodecLevel_eLow",
	   "NEXUS_VideoCodecLevel_eMain",
	   "NEXUS_VideoCodecLevel_eHigh",
	   "NEXUS_VideoCodecLevel_eHigh1440"
	};

	char* strDisplayType[NEXUS_DisplayType_eMax]={
		"NEXUS_DisplayType_eAuto",
		"NEXUS_DisplayType_eLvds",
		"NEXUS_DisplayType_eDvo",
		"NEXUS_DisplayType_eBypass"
	};

	char* strDisplayTimingGenerator[NEXUS_DisplayTimingGenerator_eMax]={
		"NEXUS_DisplayTimingGenerator_ePrimaryInput",
		"NEXUS_DisplayTimingGenerator_eSecondaryInput",
		"NEXUS_DisplayTimingGenerator_eTertiaryInput",
		"NEXUS_DisplayTimingGenerator_eHdmiDvo",
		"NEXUS_DisplayTimingGenerator_e656Output",
		"NEXUS_DisplayTimingGenerator_eEncoder",
		"NEXUS_DisplayTimingGenerator_eAuto"
	};

	char* strDisplayAspectRatio[NEXUS_DisplayAspectRatio_eMax]={
		"NEXUS_DisplayAspectRatio_eAuto",
		"NEXUS_DisplayAspectRatio_e4x3",
		"NEXUS_DisplayAspectRatio_e16x9",
		"NEXUS_DisplayAspectRatio_eSar"
	};

	if (ulDeviceId>=NUM_VID_ENC || s_stVideoEnc[ulDeviceId].bInitialized==FALSE ||
		s_stVideoEnc[ulDeviceId].bEncoding == FALSE)
		return DRV_ERR;

	NEXUS_VideoEncoder_GetStatus(s_stVideoEnc[ulDeviceId].hEncoder, &EncoderStatus);
	PrintStatus("Video Encoder[%d]:\n",ulDeviceId);
	PrintStatus("================== \n");
	PrintStatus("data buffer base address 		= %p\n",EncoderStatus.bufferBase);
	PrintStatus("metadata buffer base address 	= %p\n",EncoderStatus.metadataBufferBase);
	PrintStatus("error flags				= 0x%x\n",EncoderStatus.errorFlags);
	PrintStatus("error count				= %d\n",EncoderStatus.errorCount);
	PrintStatus("pics drops by error			= %d\n",EncoderStatus.picturesDroppedErrors);
	PrintStatus("pics drops by FRC			= %d\n",EncoderStatus.picturesDroppedFRC);
	PrintStatus("pics Encoded			= %d\n",EncoderStatus.picturesEncoded);
	PrintStatus("pics Received			= %d\n",EncoderStatus.picturesReceived);
	PrintStatus("pics id last encoded 		= 0x%x\n",EncoderStatus.pictureIdLastEncoded);
	PrintStatus("================== \n\n");

	NEXUS_VideoEncoder_GetSettings(s_stVideoEnc[ulDeviceId].hEncoder, &videoEncoderConfig);
	PrintStatus("================== Video Encoder Settings \n");
	PrintStatus("bitrateMax              = 0x%x\n",videoEncoderConfig.bitrateMax);
	PrintStatus("variableFrameRate       = %s\n",(videoEncoderConfig.variableFrameRate==1)? "yes":"no");
	PrintStatus("enableFieldPairing      = %s\n",(videoEncoderConfig.enableFieldPairing==1)? "yes":"no");

	PrintStatus("frameRate               = %s\n",strFrameRate[videoEncoderConfig.frameRate]);
	PrintStatus("streamStructure.framesB = 0x%x\n",videoEncoderConfig.streamStructure.framesB);
	PrintStatus("streamStructure.framesP = 0x%x\n",videoEncoderConfig.streamStructure.framesP);
	PrintStatus("streamStructure.openGop = %s\n",(videoEncoderConfig.streamStructure.openGop==1)? "yes":"no");
	PrintStatus("encoderDelay            = 0x%x\n",videoEncoderConfig.encoderDelay);
	PrintStatus("================== \n\n");

	PrintStatus("================== Video Encoder Start Settings \n");
	PrintStatus("videoEncoderStartConfig.interlaced           = %s \n",(s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.interlaced==1) ? "yes":"no");
	PrintStatus("videoEncoderStartConfig.nonRealTime          = %s \n",(s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.nonRealTime==1) ? "yes":"no");
	PrintStatus("videoEncoderStartConfig.lowDelayPipeline     = %s \n",(s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.lowDelayPipeline==1) ? "yes":"no");
	PrintStatus("videoEncoderStartConfig.encodeUserData       = %s \n",(s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.encodeUserData==1)? "yes":"no");
	PrintStatus("videoEncoderStartConfig.adaptiveLowDelayMode = %s \n",(s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.adaptiveLowDelayMode==1)? "yes":"no");
	PrintStatus("videoEncoderStartConfig.codec                = %s \n",strCodec[s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.codec]);
	PrintStatus("videoEncoderStartConfig.profile              = %s \n",strCodecProfile[s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.profile]);
	PrintStatus("videoEncoderStartConfig.level                = %s \n",strCodecLevel[s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.level]);
	PrintStatus("videoEncoderStartConfig.rateBufferDelay      = 0x%x \n",s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.rateBufferDelay);
	PrintStatus("videoEncoderStartConfig.bounds.outputFrameRate.min  = %s \n",strFrameRate[s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.bounds.outputFrameRate.min]);
	PrintStatus("videoEncoderStartConfig.bounds.outputFrameRate.max  = %s \n",strFrameRate[s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.bounds.outputFrameRate.max]);
	PrintStatus("videoEncoderStartConfig.bounds.inputFrameRate.min   = %s \n",strFrameRate[s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.bounds.inputFrameRate.min]);
	PrintStatus("videoEncoderStartConfig.bounds.inputDimension.max.width  = %d \n",s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.bounds.inputDimension.max.width);
	PrintStatus("videoEncoderStartConfig.bounds.inputDimension.max.height = %d \n",s_stVideoEnc[ulDeviceId].videoEncoderStartConfig.bounds.inputDimension.max.height);
	PrintStatus("================== \n\n");

	NEXUS_Display_GetSettings(s_stVideoEnc[ulDeviceId].hDisplay,&displaySettings);
	PrintStatus("================== Video Encoder Display Settings \n");
	PrintStatus("videoEncoderStartConfig.interlaced       = %s \n",strDisplayType[displaySettings.displayType]);
	PrintStatus("videoEncoderStartConfig.timingGenerator  = %s \n",strDisplayTimingGenerator[displaySettings.timingGenerator]);
	PrintStatus("videoEncoderStartConfig.vecIndex         = 0x%x\n",displaySettings.vecIndex);
	PrintStatus("videoEncoderStartConfig.format           = 0x%x\n",displaySettings.format);
	PrintStatus("videoEncoderStartConfig.aspectRatio      = %s\n",strDisplayAspectRatio[displaySettings.aspectRatio]);
	PrintError("================== \n\n");
	return DRV_OK;
}
