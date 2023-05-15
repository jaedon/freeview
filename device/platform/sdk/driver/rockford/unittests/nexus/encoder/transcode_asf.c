/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: transcode_asf.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 10/11/12 11:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/encoder/transcode_asf.c $
 * 
 * Hydra_Software_Devel/20   10/11/12 11:38p hongtaoz
 * SW7425-3782: use NEXUS_FrontendConnector;
 * 
 * Hydra_Software_Devel/19   8/31/12 3:01p hongtaoz
 * SW7429-254: fixed compile warning for non-debug build;
 * 
 * Hydra_Software_Devel/18   8/28/12 11:31p hongtaoz
 * SW7429-254: fixed compile error for non-ViCE2 video encoder;
 * 
 * Hydra_Software_Devel/17   8/16/12 3:47p hongtaoz
 * SW7429-241: compiled for platform without frontend;
 * 
 * Hydra_Software_Devel/16   6/6/12 11:33a hongtaoz
 * SW7425-3167: removed trackInput from NEXUS_VideoEncoderStreamStructure;
 * 
 * Hydra_Software_Devel/15   1/27/12 10:03p hongtaoz
 * SW7425-2266: Added [optional] parameter to the NEXUS_VideoEncoder_Stop;
 * 
 * Hydra_Software_Devel/14   12/20/11 1:56p hongtaoz
 * SW7425-2036: type matching in the scanf;
 * 
 * Hydra_Software_Devel/13   12/13/11 5:46p hongtaoz
 * SW7425-1046: cleaned the usage print;
 * 
 * Hydra_Software_Devel/12   12/12/11 1:53p jgarrett
 * SW7425-1406: Disabling seamless transcode for WMA
 * 
 * Hydra_Software_Devel/11   12/12/11 11:54a hongtaoz
 * SW7425-1476: added WMA encode option; added a few other test options;
 * 
 * Hydra_Software_Devel/9   11/8/11 11:13a hongtaoz
 * SW7425-1046: added configurable support for transcode_asf; added dual
 * asf transcode; Note transcoder output is not looped back to display;
 * 
 * Hydra_Software_Devel/8   11/7/11 11:35a hongtaoz
 * SW7425-608: removed reference to 7.493fps;
 * 
 * Hydra_Software_Devel/7   9/1/11 1:32p hongtaoz
 * SW7425-937: added h264 payload case for ASF muxer test;
 * 
 * Hydra_Software_Devel/6   9/1/11 1:07p hongtaoz
 * SW7425-165: VC1 encode only supports up to 480p;
 * 
 * Hydra_Software_Devel/5   8/17/11 5:04p hongtaoz
 * SW7425-651: added finer control API for video encoder delay;
 * 
 * Hydra_Software_Devel/4   7/6/11 12:02p nilesh
 * SW7425-447: Fixed typo for 7.493 fps nexus frame rate constant
 * 
 * Hydra_Software_Devel/3   6/27/11 5:09p hongtaoz
 * SW7425-447: replaced 720p with CIF format encode since vc1 is not
 * required to support HD;
 * 
 * Hydra_Software_Devel/2   6/13/11 4:01p hongtaoz
 * SW7425-447: added auto testa and interactive dynamic change test for
 * transcode_asf;
 * 
 * Hydra_Software_Devel/1   5/23/11 3:20p hongtaoz
 * SW7425-447: merge the asf muxer test to mainline;
 * 
 * Hydra_Software_Devel/1   5/23/11 3:15p hongtaoz
 * SW7425-447: merge the asf muxer test to mainline;
 * 
 * Hydra_Software_Devel/SW7425-447/3   5/23/11 4:33p nilesh
 * SW7425-447: Reverted NEXUS_FileMux_GetDefaultCreateSettings() prototype
 * 
 * Hydra_Software_Devel/SW7425-447/2   5/19/11 5:49p nilesh
 * SW7425-447: Fixed VC1 encode parameters
 * 
 * Hydra_Software_Devel/SW7425-447/1   5/18/11 3:16p nilesh
 * SW7425-447: Fixed ASF Support
 * 
 *****************************************************************************/
/* Nexus example app: playback and decode */

#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_video_adj.h"
#include "nexus_core_utils.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_FILE_MUX
#include "nexus_playback.h"
#include "nexus_record.h"
#include "nexus_file.h"
#include "nexus_video_encoder.h"
#include "nexus_audio_encoder.h"
#include "nexus_audio_mixer.h"
#include "nexus_file_mux.h"
#endif
#if NEXUS_NUM_HDMI_INPUTS
#include "nexus_hdmi_input.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel.h"
#endif
#include "b_os_lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
#include "bdbg.h"


BDBG_MODULE(transcode_asf);

/* NRT a+v transcode STC av_window enable test */
#define BTST_ENABLE_NRT_STC_AV_WINDOW 1

/* TODO: set to 0 to enable the debug monitor for NRT mode source decode; set to 1 to disable it. */
#define BTST_DISABLE_NRT_DEBUG_DISPLAY 1

#if  NEXUS_HAS_FRONTEND
#define BTST_SUPPORT_FRONTEND    1
#else
#define BTST_SUPPORT_FRONTEND    0
#endif

#if NEXUS_HAS_SYNC_CHANNEL
#define BTST_ENABLE_AV_SYNC 1
#else
#define BTST_ENABLE_AV_SYNC 0
#endif

/* If true, enable the DSP mixer for seamless wraparound handling */
#define BTST_ENABLE_DSP_MIXER 0

/***********************************
 *   Display assignments 
 */
#ifndef NEXUS_ENCODER_DISPLAY_IDX
#define NEXUS_ENCODER_DISPLAY_IDX   3
#endif
/* source display locally as HD hdmi/component output by default */
#define BTST_SOURCE_DISPLAY_IDX     0


/***********************************
 *   Source ID assignments 
 */
#define BTST_RESOURCE_FILE       0
#define BTST_RESOURCE_HDMI       1
#define BTST_RESOURCE_QAM        2

/***********************************
 *   STC assignments 
 */
#define BTST_NRT_SOURCE_AUDIO_STC_IDX       3
#define BTST_ENCODER_STC_IDX                2
#define BTST_SOURCE_STC_IDX                 1

/***********************************
 *   Playpump assignments 
 */
#define BTST_SOURCE_PLAYPUMP_IDX            (NEXUS_NUM_VIDEO_ENCODERS+3)

/***********************************
 *   Video Decoder assignments 
 */
#define BTST_XCODE_VID_DECODE_IDX       2

static void 
transcoderFinishCallback(void *context, int param)
{
    BKNI_EventHandle finishEvent = (BKNI_EventHandle)context;

    BSTD_UNUSED(param);
    BDBG_WRN(("Finish callback invoked, now stop the stream mux."));
    BKNI_SetEvent(finishEvent);
}
#define TEST_ITERATIONS 3
struct cmdSettings {
    NEXUS_VideoFormat displayFormat;
    NEXUS_VideoFrameRate encoderFrameRate;
    unsigned encoderBitrate;
    unsigned encoderGopStructureFramesP;
    unsigned encoderGopStructureFramesB;
    NEXUS_VideoCodec encoderVideoCodec;
    NEXUS_VideoCodecProfile encoderProfile;
    NEXUS_VideoCodecLevel encoderLevel;
}cmdSettings[TEST_ITERATIONS] = {
   {NEXUS_VideoFormat_e720p, NEXUS_VideoFrameRate_e29_97, 6*1000*1000, 14, 0, NEXUS_VideoCodec_eH264, NEXUS_VideoCodecProfile_eBaseline, NEXUS_VideoCodecLevel_e31},
   {NEXUS_VideoFormat_e480p, NEXUS_VideoFrameRate_e29_97, 1*1000*1000, 29, 0, NEXUS_VideoCodec_eH264, NEXUS_VideoCodecProfile_eBaseline, NEXUS_VideoCodecLevel_e31},
   {NEXUS_VideoFormat_e480p, NEXUS_VideoFrameRate_e29_97, 1*1000*1000, 29, 0, NEXUS_VideoCodec_eVc1SimpleMain, NEXUS_VideoCodecProfile_eMain, NEXUS_VideoCodecLevel_eMain}
};

typedef struct EncodeSettings {
	char                    fname[256];
	NEXUS_VideoFormat       displayFormat;
	NEXUS_VideoFrameRate    encoderFrameRate;
	int                     encoderBitrate;
	int                     encoderGopStructureFramesP;
	int                     encoderGopStructureFramesB;
	NEXUS_VideoCodec        encoderVideoCodec;
	NEXUS_VideoCodecProfile encoderProfile;
	NEXUS_VideoCodecLevel   encoderLevel;
	NEXUS_DisplayCustomFormatSettings customFormatSettings;
	bool                    bCustom;
	bool                    bAudioEncode;
	NEXUS_AudioCodec        encoderAudioCodec;

	bool                    enableFieldPairing;

	/* 0 to use default 750ms rate buffer delay; allow user to adjust it to lower encode delay at cost of quality reduction! */
	unsigned                rateBufferDelay;

	/* to allow 23.976p passthru; allow user to configure minimum framerate to achieve lower delay!
	 * Note: lower minimum framerate means longer encode delay */
	NEXUS_VideoFrameRate    inputFrameRateMin;

	/* to allow 15 ~ 60p dynamic frame rate coding; allow user to config higher minimum frame rate for lower delay! */
	NEXUS_VideoFrameRate    outputFrameRateMin;

	/* max encode size allows 1080p encode; allow user to choose lower max resolution for lower encode delay */
	unsigned                inputMaxWidth;
	unsigned                inputMaxHeight;
}  EncodeSettings;

typedef struct InputSettings{
	int                    resource;
	char                   fname[256];
	NEXUS_TransportType    eStreamType;
	NEXUS_VideoCodec       eVideoCodec;
	NEXUS_AudioCodec       eAudioCodec;
	bool                   bPcmAudio;
	int                    iVideoPid;
	int                    iAudioPid;
	int                    iPcrPid;
	int                    freq;
#if BTST_SUPPORT_FRONTEND
	NEXUS_FrontendQamMode  qamMode;
#endif
	bool                   bAudioInput;
	bool                   bConfig;
} InputSettings;

typedef struct TranscodeContext {
	/* xcoder handles */
	NEXUS_VideoEncoderHandle   videoEncoder;
	NEXUS_DisplayHandle        displayTranscode;
	NEXUS_VideoWindowHandle    windowTranscode;
#if BTST_ENABLE_DSP_MIXER
    NEXUS_AudioMixerHandle     audioMixer;
#endif
	NEXUS_AudioEncoderHandle   audioEncoder;
	NEXUS_AudioMuxOutputHandle audioMuxOutput;
	NEXUS_FileMuxHandle        fileMux;
	BKNI_EventHandle           finishEvent;
	NEXUS_StcChannelHandle     stcChannelTranscode;

	/* xcoder mux file handles */
	NEXUS_MuxFileHandle        muxFile;

	/* source local decode/display handles */
	NEXUS_StcChannelHandle     stcVideoChannel;
	NEXUS_StcChannelHandle     stcAudioChannel;
	NEXUS_DisplayHandle        display;
	NEXUS_VideoWindowHandle    window;
	
#if NEXUS_HAS_HDMI_INPUT
	NEXUS_HdmiInputHandle      hdmiInput;
#endif
	NEXUS_FilePlayHandle       file;
	NEXUS_PlaypumpHandle       playpump;
	NEXUS_PlaybackHandle       playback;
	NEXUS_PidChannelHandle     videoPidChannel;
	NEXUS_VideoDecoderHandle   videoDecoder;
	NEXUS_PidChannelHandle     audioPidChannel;
	NEXUS_AudioDecoderHandle   audioDecoder, audioPassthrough;
#if BTST_ENABLE_AV_SYNC
	NEXUS_SyncChannelHandle    syncChannel;
#endif

	/* video encoder settings */
	EncodeSettings             encodeSettings;

	/* input settings */
	InputSettings              inputSettings;

#if BTST_SUPPORT_FRONTEND
	NEXUS_FrontendHandle       frontend;
#endif

	/* xcode system settings */
	bool                      bNonRealTime;
	bool                      bLowDelay;
	bool                      bCustomizeDelay;
	B_EventHandle             eofEvent;
	B_ThreadHandle            nrtEofHandle;
	B_MutexHandle             mutexStarted;

	int                       contextId;
	bool                      bStarted;
} TranscodeContext;

/* global context 
 * function level context in this */
static TranscodeContext xcodeContext[NEXUS_NUM_VIDEO_ENCODERS];

static NEXUS_PlatformConfiguration g_platformConfig;
static char g_keyReturn ='\0';
static int g_selectedXcodeContextId = 0;
static bool g_bNonRealTimeWrap = false;
static bool g_bAutoQuit = false;
static NEXUS_VideoEncoderType g_eVideoEncoderType = NEXUS_VideoEncoderType_eMulti;
static B_EventHandle g_eotEvent = NULL;/* end of test event */
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
static int g_encodeDisplayId[NEXUS_NUM_VIDEO_ENCODERS] = {
    NEXUS_ENCODER_DISPLAY_IDX
};
#else
static int g_encodeDisplayId[NEXUS_NUM_VIDEO_ENCODERS] = {
	NEXUS_ENCODER0_DISPLAY_IDX,
	NEXUS_ENCODER1_DISPLAY_IDX,
};
#endif
static int start_transcode(TranscodeContext  *pContext);
static void stop_transcode(TranscodeContext  *pContext);

/* xcode window PQ setting */
static void window_dnr( 
	NEXUS_VideoWindowHandle  hWindow,
	bool bCustom )
{
	NEXUS_VideoWindowDnrSettings windowDnrSettings;

	NEXUS_VideoWindow_GetDnrSettings(hWindow, &windowDnrSettings);
	if(bCustom)
	{
		printf("DNR settings:\n");
		printf("MNR mode: (%d) Disable; (%d) Bypass; (%d) Enable; ",
			NEXUS_VideoWindowFilterMode_eDisable, NEXUS_VideoWindowFilterMode_eBypass, NEXUS_VideoWindowFilterMode_eEnable);
		scanf("%d", (int*)&windowDnrSettings.mnr.mode);
		if(windowDnrSettings.mnr.mode == NEXUS_VideoWindowFilterMode_eEnable)
		{
			printf("MNR level (valid range -100 ... 2^31; for filtering, adjQp = Qp * (level + 100) / 100: ");
			scanf("%d", (int*)&windowDnrSettings.mnr.level);
		}
		printf("BNR mode: (%d) Disable; (%d) Bypass; (%d) Enable; ",
			NEXUS_VideoWindowFilterMode_eDisable, NEXUS_VideoWindowFilterMode_eBypass, NEXUS_VideoWindowFilterMode_eEnable);
		scanf("%d", (int*)&windowDnrSettings.bnr.mode);
		if(windowDnrSettings.bnr.mode == NEXUS_VideoWindowFilterMode_eEnable)
		{
			printf("BNR level (valid range -100 ... 2^31): ");
			scanf("%d", (int*)&windowDnrSettings.bnr.level);
		}
		printf("DCR mode: (%d) Disable; (%d) Bypass; (%d) Enable; ",
			NEXUS_VideoWindowFilterMode_eDisable, NEXUS_VideoWindowFilterMode_eBypass, NEXUS_VideoWindowFilterMode_eEnable);
		scanf("%d", (int*)&windowDnrSettings.dcr.mode);
		if(windowDnrSettings.dcr.mode == NEXUS_VideoWindowFilterMode_eEnable)
		{
			printf("DCR level (valid range -100 ... 2^31): ");
			scanf("%d", (int*)&windowDnrSettings.dcr.level);
		}
		if((windowDnrSettings.dcr.mode == NEXUS_VideoWindowFilterMode_eEnable) ||
		   (windowDnrSettings.bnr.mode == NEXUS_VideoWindowFilterMode_eEnable) ||
		   (windowDnrSettings.mnr.mode == NEXUS_VideoWindowFilterMode_eEnable))
		{
			printf("User Qp (default 0 means calculated from decoder source, non-zero is user override): ");
			scanf("%u", &windowDnrSettings.qp);
		}
	}
	else /* default */
	{
		windowDnrSettings.mnr.mode = NEXUS_VideoWindowFilterMode_eEnable;
		windowDnrSettings.bnr.mode = NEXUS_VideoWindowFilterMode_eEnable;
		windowDnrSettings.dcr.mode = NEXUS_VideoWindowFilterMode_eEnable;
		windowDnrSettings.mnr.level = 0;
		windowDnrSettings.bnr.level = 0;
		windowDnrSettings.dcr.level = 0;
		windowDnrSettings.qp = 0;
	}
	NEXUS_VideoWindow_SetDnrSettings(hWindow, &windowDnrSettings);
}

static void window_mad( 
	NEXUS_VideoWindowHandle  hWindow,
	bool bCustom,
	bool bEnable,
	bool bLowDelay)
{
	int choice;
	NEXUS_VideoWindowMadSettings windowMadSettings;
	
	NEXUS_VideoWindow_GetMadSettings(hWindow, &windowMadSettings);
	windowMadSettings.deinterlace = bEnable;
	if(bCustom)
	{
		if(windowMadSettings.deinterlace)
		{
			printf("3:2 reverse pulldown? (0) Disable; (1) Enable; ");
			scanf("%d", &choice); windowMadSettings.enable32Pulldown = choice;
			printf("2:2 reverse pulldown? (0) Disable; (1) Enable; ");
			scanf("%d", &choice); windowMadSettings.enable22Pulldown = choice;
			printf("Game Mode: (%d) Off (2-field delay, best quality); (%d) 1-field delay; (%d) 0-field delay; ",
				NEXUS_VideoWindowGameMode_eOff, NEXUS_VideoWindowGameMode_e4Fields_1Delay, NEXUS_VideoWindowGameMode_e4Fields_0Delay);
			scanf("%d", (int*)&windowMadSettings.gameMode);
		}
	}
	else /* default */
	{
		windowMadSettings.enable32Pulldown = true;
		windowMadSettings.enable22Pulldown = true;
		windowMadSettings.gameMode = bLowDelay?NEXUS_VideoWindowGameMode_e4Fields_0Delay : NEXUS_VideoWindowGameMode_eOff;
	}
	NEXUS_VideoWindow_SetMadSettings(hWindow, &windowMadSettings);
}

static void config_xcoder_context (
	TranscodeContext *pContext )
{
	int choice;
	pContext->inputSettings.bConfig = true;
	printf("\n choose source (0) File playback (1) HDMI (2) QAM	  ");scanf("%d", (int32_t*)&pContext->inputSettings.resource);

	switch (pContext->inputSettings.resource)
	{
	case (BTST_RESOURCE_HDMI):
#if !NEXUS_HAS_HDMI_INPUT
		fprintf(stderr, "HDMI input is not supported!\n");
		return;
#endif
		break;
	case (BTST_RESOURCE_QAM):
#if BTST_SUPPORT_FRONTEND
		{
			printf("\n Front End QAM freq (Mhz): ");						     scanf("%d", (int32_t*)&pContext->inputSettings.freq);
			printf("\n Front End QAM Mode:\n"
				" (%d) 64\n"
				" (%d) 256\n"
				" (%d) 1024\n", 
				NEXUS_FrontendQamMode_e64, NEXUS_FrontendQamMode_e256, NEXUS_FrontendQamMode_e1024); 
			scanf("%d", (int32_t*)&pContext->inputSettings.qamMode);
			printf("\n source stream type: (2) Ts             "); pContext->inputSettings.eStreamType = NEXUS_TransportType_eTs;
			printf("\n source stream codec: (%d) Mpeg2 (%d) H264",
				NEXUS_VideoCodec_eMpeg2, NEXUS_VideoCodec_eH264); 
			scanf("%d", (int32_t*)&pContext->inputSettings.eVideoCodec);
			printf("\n Video pid:                                     "); scanf("%d", &pContext->inputSettings.iVideoPid);
			printf("\n Pcr   pid:                                     "); scanf("%d", &pContext->inputSettings.iPcrPid);
			break;
		}
#else
		BDBG_ERR(("This platform doesn't support frontend! Select file input..."));
#endif
	case (BTST_RESOURCE_FILE):
	default:
		{
			printf("\n source stream file: ");
			scanf("%s", pContext->inputSettings.fname);
			printf("\n source stream type:  (%d) Es (%d) Ts           ", 
				NEXUS_TransportType_eEs, NEXUS_TransportType_eTs);	   scanf("%d", (int32_t*)&pContext->inputSettings.eStreamType);
			printf("\n source stream codec: (%d) Mpeg2 (%d) H264      ",
				NEXUS_VideoCodec_eMpeg2, NEXUS_VideoCodec_eH264);	   
			scanf("%d", (int32_t*)&pContext->inputSettings.eVideoCodec);
			printf("\n Video pid:                                           ");	   scanf("%d", &pContext->inputSettings.iVideoPid);
			printf("\n Pcr   pid:                                           ");	   scanf("%d", &pContext->inputSettings.iPcrPid);
		}
	}

	printf("\n Input Audio Parameters");
	printf("\n Enable Audio channel: (0) No (1) Yes             ");	   scanf("%d", &choice);
	pContext->inputSettings.bAudioInput = (choice!=0);
	if(pContext->inputSettings.bAudioInput)
	{
		if (pContext->inputSettings.resource == BTST_RESOURCE_HDMI)
		{
			printf("\n Is HDMI input audio uncompressed? (0)No; (1)Yes    ");   scanf("%d", &choice);
			pContext->inputSettings.bPcmAudio = choice;
		}
		else
		{
			printf("\n Audio Pid:                                           ");   scanf("%d", &pContext->inputSettings.iAudioPid);
		}

		/* PCM audio must be encoded */
		if(pContext->inputSettings.bPcmAudio)
		{
			pContext->encodeSettings.bAudioEncode = true;
		}
		else
		{
			printf("\n Audio Codec:\n"
				" (%d)Mpg\n"
				" (%d)AAC\n"
				" (%d)AAC+Loas\n"
				" (%d)AAC+Adts\n"
				" (%d)AC3\n",
				NEXUS_AudioCodec_eMpeg,
				NEXUS_AudioCodec_eAac,
				NEXUS_AudioCodec_eAacPlusLoas,
				NEXUS_AudioCodec_eAacPlusAdts,
				NEXUS_AudioCodec_eAc3);   
			scanf("%d", (int32_t*)&pContext->inputSettings.eAudioCodec);
			pContext->encodeSettings.encoderAudioCodec = pContext->inputSettings.eAudioCodec;
		}
	}

	printf("\n****************************************************************\n");
	printf("\n Input Parameters\n");
	printf("\n SourceType:   %d", pContext->inputSettings.resource);
	if(pContext->inputSettings.resource == BTST_RESOURCE_FILE)
		printf("\n filename:       %s", pContext->inputSettings.fname);

#if BTST_SUPPORT_FRONTEND
	if(pContext->inputSettings.resource == BTST_RESOURCE_QAM)
		printf("\n freq: %d MHz;  qam mode: %d", pContext->inputSettings.freq, pContext->inputSettings.qamMode);
#endif
	if(pContext->inputSettings.resource != BTST_RESOURCE_HDMI)
	{
		printf("\n Streamtype:  %d \n encoderVideoCodec:  %d", pContext->inputSettings.eStreamType, pContext->inputSettings.eVideoCodec);
		printf("\n vPid:  %4d;  PcrPid:   %4d", pContext->inputSettings.iVideoPid, pContext->inputSettings.iPcrPid);
	}
	if(pContext->inputSettings.bAudioInput)
	{
		printf("\n Enable Audio channel Pid: %d\n audio codec: %d           ", pContext->inputSettings.iAudioPid, pContext->inputSettings.eAudioCodec);
	}
	printf("\n Config %d", pContext->inputSettings.bConfig);
	printf("\n****************************************************************\n");

	printf("\n\n Encode Settings");
	printf("\n encode stream:                                                                        "); scanf("%s", pContext->encodeSettings.fname);
	printf("\n custom format: (0) No (1) Yes                                                         "); scanf("%d", &choice);
	pContext->encodeSettings.bCustom = choice;
	if(!pContext->encodeSettings.bCustom)
	{
		printf("\n enocder displayFormat:\n"
			" (%2d) 480i\n"
			" (%2d) 576i\n"
			" (%2d) 480p\n"
			" (%2d) 576p\n"
			" (%2d) 1080i60\n"
			" (%2d) 1080i50\n"
			" (%2d) 1080p24\n"
			" (%2d) 1080p25\n"
			" (%2d) 1080p30\n"
			" (%2d) 1080p50\n"
			" (%2d) 1080p60\n"
			" (%2d) 720p60\n"
			" (%2d) 720p50\n"
			" (%2d) 720p24\n"
			" (%2d) 720p25\n"
			" (%2d) 720p30Hz:\n",
			(NEXUS_VideoFormat_eNtsc),
			(NEXUS_VideoFormat_ePal),
			(NEXUS_VideoFormat_e480p),
			(NEXUS_VideoFormat_e576p),
			(NEXUS_VideoFormat_e1080i),
			(NEXUS_VideoFormat_e1080i50hz),
			(NEXUS_VideoFormat_e1080p24hz),
			(NEXUS_VideoFormat_e1080p25hz),
			(NEXUS_VideoFormat_e1080p30hz),
			(NEXUS_VideoFormat_e1080p50hz),
			(NEXUS_VideoFormat_e1080p60hz),
			(NEXUS_VideoFormat_e720p),
			(NEXUS_VideoFormat_e720p50hz),
			(NEXUS_VideoFormat_e720p24hz),
			(NEXUS_VideoFormat_e720p25hz),
			(NEXUS_VideoFormat_e720p30hz)); 
		scanf("%d", (int32_t*)&pContext->encodeSettings.displayFormat);
	}
	else
	{
		pContext->encodeSettings.displayFormat = NEXUS_VideoFormat_eCustom2;
		printf("\n Resolution width & height:                                                        ");
		scanf("%d %d", (int32_t*)&pContext->encodeSettings.customFormatSettings.width, (int32_t*)&pContext->encodeSettings.customFormatSettings.height);
		printf("\n Interlaced? (0) No (1) Yes	"); 
		scanf("%d", &choice); pContext->encodeSettings.customFormatSettings.interlaced = choice;
		printf("\n Vertical refresh rate (in 1/1000th Hz):                                           "); scanf("%u", (unsigned*)&pContext->encodeSettings.customFormatSettings.refreshRate);
		printf("\n Aspect Ratio:\n"
			" (%d) Auto\n"
			" (%d) 4x3\n"
			" (%d) 16x9\n"
			" (%d) SAR\n",
			NEXUS_DisplayAspectRatio_eAuto,
			NEXUS_DisplayAspectRatio_e4x3,
			NEXUS_DisplayAspectRatio_e16x9,
			NEXUS_DisplayAspectRatio_eSar); 
		scanf("%d", (int32_t*)&pContext->encodeSettings.customFormatSettings.aspectRatio);
		if(NEXUS_DisplayAspectRatio_eSar == pContext->encodeSettings.customFormatSettings.aspectRatio)
		{
			printf("Please enter Sample Aspect Ratio X and Y: \n");
			scanf("%u %u", &pContext->encodeSettings.customFormatSettings.sampleAspectRatio.x,
				&pContext->encodeSettings.customFormatSettings.sampleAspectRatio.y);
		}
	}

	printf("\n Frame rate:\n"
		" (%d) 23.976\n"
		" (%d) 24\n"
		" (%d) 25\n"
		" (%d) 29.97Hz\n"
		" (%d) 30 Hz\n"
		" (%d) 50 Hz\n"
		" (%d) 59.94\n"
		" (%d) 60\n"
		" (%d) 14.985\n",
		NEXUS_VideoFrameRate_e23_976,
		NEXUS_VideoFrameRate_e24,
		NEXUS_VideoFrameRate_e25,
		NEXUS_VideoFrameRate_e29_97,
		NEXUS_VideoFrameRate_e30,
		NEXUS_VideoFrameRate_e50,
		NEXUS_VideoFrameRate_e59_94,
		NEXUS_VideoFrameRate_e60,
		NEXUS_VideoFrameRate_e14_985);
	scanf("%d", (int*)&pContext->encodeSettings.encoderFrameRate);
	printf("\n Bitrate (bps):                                                                       "); scanf("%d", (int*)&pContext->encodeSettings.encoderBitrate);
	printf("\n # of P frames per GOP:                                                               "); scanf("%d", (int*)&pContext->encodeSettings.encoderGopStructureFramesP);
	printf("\n # of B frames between reference I or P frames:                                       "); scanf("%d", (int*)&pContext->encodeSettings.encoderGopStructureFramesB);
	printf("\n Encode Video Codec: (%d) H264 (%d) VC1                            ", 
		NEXUS_VideoCodec_eH264, NEXUS_VideoCodec_eVc1SimpleMain); 
	scanf("%d", (int*)&pContext->encodeSettings.encoderVideoCodec);
	printf("\n Profile: (%d) Simple (%d) AdvancedSimple (%d) Main (%d) High (%d) Baseline                               ",
		NEXUS_VideoCodecProfile_eSimple, NEXUS_VideoCodecProfile_eAdvancedSimple, NEXUS_VideoCodecProfile_eMain, NEXUS_VideoCodecProfile_eHigh, NEXUS_VideoCodecProfile_eBaseline); 
	scanf("%d", (int*)&pContext->encodeSettings.encoderProfile);
	printf("\n Level:\n"
		" (%2d) 1.0\n"
		" (%2d) 2.0\n"
		" (%2d) 2.1\n"
		" (%2d) 2.2\n"
		" (%2d) 3.0\n"
		" (%2d) 3.1\n"
		" (%2d) 3.2\n"
		" (%2d) 4.0\n"
		" (%2d) 4.1\n"
		" (%2d) 4.2\n"
		" (%2d) 5.0\n"
		" (%2d) Low\n"
		" (%2d) Main\n"
		" (%2d) High\n",
		NEXUS_VideoCodecLevel_e10, NEXUS_VideoCodecLevel_e20, NEXUS_VideoCodecLevel_e21, NEXUS_VideoCodecLevel_e22, NEXUS_VideoCodecLevel_e30,
		NEXUS_VideoCodecLevel_e31, NEXUS_VideoCodecLevel_e32, NEXUS_VideoCodecLevel_e40, NEXUS_VideoCodecLevel_e41,
		NEXUS_VideoCodecLevel_e42, NEXUS_VideoCodecLevel_e50, NEXUS_VideoCodecLevel_eLow, NEXUS_VideoCodecLevel_eMain, NEXUS_VideoCodecLevel_eHigh); 
	scanf("%d", (int*)&pContext->encodeSettings.encoderLevel);

	if(pContext->inputSettings.bAudioInput)
	{
		if(pContext->inputSettings.bPcmAudio)
		{
			pContext->encodeSettings.bAudioEncode = true;
		}
		else
		{
			printf("\n Enable Audio transcode: (0) No (1) Yes                                                       "); scanf("%d", &choice);
			pContext->encodeSettings.bAudioEncode = choice;
		}

		if(pContext->encodeSettings.bAudioEncode)
		{
			printf("\n Encode Audio Codec: (%d)Mpg (%d)Mp3 (%d)AAC (%d)AAC+ (%d)AAC+Loas (%d)AAC+Adts (%d)WMA          ",
				NEXUS_AudioCodec_eMpeg,
				NEXUS_AudioCodec_eMp3,
				NEXUS_AudioCodec_eAac,
				NEXUS_AudioCodec_eAacPlus,
				NEXUS_AudioCodec_eAacPlusLoas,
				NEXUS_AudioCodec_eAacPlusAdts,
				NEXUS_AudioCodec_eWmaStd); 
			scanf("%d", (int32_t*)&pContext->encodeSettings.encoderAudioCodec);
		}
	}
	if(pContext->inputSettings.resource == BTST_RESOURCE_FILE)
	{
		int choice;
		printf("\nNon-Realtime transcode? [1=y/0=n]\n");
		scanf("%d", &choice);
		pContext->bNonRealTime = choice;
	}

	if(!pContext->bNonRealTime)
	{
		unsigned iChoice;
		printf("\n Customize video encoder delay setting? [1=y/0=n]\n");
		scanf("%u", &iChoice);
		pContext->bCustomizeDelay = iChoice;
		if(pContext->bCustomizeDelay)
		{
			printf("\n enable Inverse Telecine Field Pairing coding? [1=y/0=n]\n");
			scanf("%u", &iChoice);
			pContext->encodeSettings.enableFieldPairing = iChoice;

			printf("\n video encoder rate buffer delay (ms):\n");
			scanf("%u", &iChoice);
			pContext->encodeSettings.rateBufferDelay = iChoice;

			printf("\n video encode display minimum frame rate:\n"
				" (%d) 23.976\n"
				" (%d) 24\n"
				" (%d) 25\n"
				" (%d) 29.97Hz\n"
				" (%d) 30 Hz\n"
				" (%d) 50 Hz\n"
				" (%d) 59.94\n"
				" (%d) 60\n"
				" (%d) 14.985\n",
				NEXUS_VideoFrameRate_e23_976,
				NEXUS_VideoFrameRate_e24,
				NEXUS_VideoFrameRate_e25,
				NEXUS_VideoFrameRate_e29_97,
				NEXUS_VideoFrameRate_e30,
				NEXUS_VideoFrameRate_e50,
				NEXUS_VideoFrameRate_e59_94,
				NEXUS_VideoFrameRate_e60,
				NEXUS_VideoFrameRate_e14_985);
			scanf("%d", (int*)&pContext->encodeSettings.inputFrameRateMin);

			printf("\n video encoder output minimum frame rate:\n"
				" (%d) 23.976\n"
				" (%d) 24\n"
				" (%d) 25\n"
				" (%d) 29.97Hz\n"
				" (%d) 30 Hz\n"
				" (%d) 50 Hz\n"
				" (%d) 59.94\n"
				" (%d) 60\n"
				" (%d) 14.985\n",
				NEXUS_VideoFrameRate_e23_976,
				NEXUS_VideoFrameRate_e24,
				NEXUS_VideoFrameRate_e25,
				NEXUS_VideoFrameRate_e29_97,
				NEXUS_VideoFrameRate_e30,
				NEXUS_VideoFrameRate_e50,
				NEXUS_VideoFrameRate_e59_94,
				NEXUS_VideoFrameRate_e60,
				NEXUS_VideoFrameRate_e14_985);
			scanf("%d", (int*)&pContext->encodeSettings.outputFrameRateMin);

			printf("\n video encode maximum resolution width:\n");
			scanf("%u", &pContext->encodeSettings.inputMaxWidth);
			printf("\n video encode maximum resolution height:\n");
			scanf("%u", &pContext->encodeSettings.inputMaxHeight);
			printf("\n video encoder pipeline low delay? [1=Y/0=N]:\n");
			scanf("%d", &choice);
			pContext->bLowDelay = choice;
		}
	}
	printf("\n****************************************************************\n");
	printf("\n Encode Parameters\n");
	printf("\n filename:  %s", pContext->encodeSettings.fname);
	printf("\n display format:  %d", pContext->encodeSettings.displayFormat);
	printf("\n Frame Rate: %d\n Bit Rate: %d", pContext->encodeSettings.encoderFrameRate, pContext->encodeSettings.encoderBitrate);
	printf("\n P frames: %d;     B frames: %d", pContext->encodeSettings.encoderGopStructureFramesP, pContext->encodeSettings.encoderGopStructureFramesB);
	printf("\n Video Codec: %4d;  Profile: %4d;  Level: %4d", pContext->encodeSettings.encoderVideoCodec, pContext->encodeSettings.encoderProfile,pContext->encodeSettings.encoderLevel );
	if(pContext->encodeSettings.bAudioEncode)
		printf("\n Audio Codec: %4d", pContext->encodeSettings.encoderAudioCodec);
	printf("\n****************************************************************\n");
	printf("\n%s transcode...%s \n", pContext->bNonRealTime? "Non-Realtime":"Real time", 
		pContext->bLowDelay? "Low Delay pipeline Mode": "Normal delay pipeline Mode");
}

/************************************
 * Set up encoder AV sync.
 * encode setting and startSetting to be set after end-to-end delay is determined */
/* get end-to-end delay (Dee) for audio and video encoders;
* TODO: match AV delay! In other words,
*   if (aDee > vDee) {
*       vDee' = aDee' = aDee;
*   }
*   else {
*       vDee' = aDee' = vDee;
*   }
*/
static void xcode_av_sync( 
	TranscodeContext  *pContext,
	NEXUS_VideoEncoderSettings *pVideoEncoderConfig,
	NEXUS_VideoEncoderStartSettings *pVideoEncoderStartConfig )
{
	NEXUS_VideoEncoderDelayRange videoDelay;
	NEXUS_AudioMuxOutputDelayStatus audioDelayStatus;
	NEXUS_AudioMuxOutputStartSettings audioMuxStartSettings;
	unsigned Dee;

	/******************************************
	 * add configurable delay to video path 
	 */
	/* disable Inverse Telecine Field Pairing for extreme low delay mode
	 * NOTE: ITFP is encoder feature to detect and lock on 3:2/2:2 cadence in the video content to help 
	 * efficient coding for interlaced formats; disabling ITFP will impact the bit efficiency but reduce the encode delay. */
	if(pContext->bCustomizeDelay)
	{
		pVideoEncoderConfig->enableFieldPairing = pContext->encodeSettings.enableFieldPairing;

		/* 0 to use default 750ms rate buffer delay; TODO: allow user to adjust it to lower encode delay at cost of quality reduction! */
		pVideoEncoderStartConfig->rateBufferDelay = pContext->encodeSettings.rateBufferDelay;

		/* to allow 23.976p passthru; TODO: allow user to configure minimum framerate to achieve lower delay!
		 * Note: lower minimum framerate means longer encode delay */
		pVideoEncoderStartConfig->bounds.inputFrameRate.min = pContext->encodeSettings.inputFrameRateMin;

		/* to allow 15 ~ 60p dynamic frame rate coding TODO: allow user to config higher minimum frame rate for lower delay! */
		pVideoEncoderStartConfig->bounds.outputFrameRate.min = pContext->encodeSettings.outputFrameRateMin;
		pVideoEncoderStartConfig->bounds.outputFrameRate.max = NEXUS_VideoFrameRate_e60;

		/* max encode size allows 1080p encode; TODO: allow user to choose lower max resolution for lower encode delay */
		pVideoEncoderStartConfig->bounds.inputDimension.max.width = pContext->encodeSettings.inputMaxWidth;
		pVideoEncoderStartConfig->bounds.inputDimension.max.height = pContext->encodeSettings.inputMaxHeight;
	}
	else
	{
		pVideoEncoderConfig->enableFieldPairing = true;

		/* 0 to use default 750ms rate buffer delay; TODO: allow user to adjust it to lower encode delay at cost of quality reduction! */
		pVideoEncoderStartConfig->rateBufferDelay = 0;

		/* to allow 23.976p passthru; TODO: allow user to configure minimum framerate to achieve lower delay!
		 * Note: lower minimum framerate means longer encode delay */
		pVideoEncoderStartConfig->bounds.inputFrameRate.min = NEXUS_VideoFrameRate_e23_976;

		/* to allow 15 ~ 60p dynamic frame rate coding TODO: allow user to config higher minimum frame rate for lower delay! */
		pVideoEncoderStartConfig->bounds.outputFrameRate.min = NEXUS_VideoFrameRate_e14_985;
		pVideoEncoderStartConfig->bounds.outputFrameRate.max = NEXUS_VideoFrameRate_e60;

		/* max encode size allows 1080p encode; TODO: allow user to choose lower max resolution for lower encode delay */
		pVideoEncoderStartConfig->bounds.inputDimension.max.width = 1920;
		pVideoEncoderStartConfig->bounds.inputDimension.max.height = 1088;
	}

	/*****************************************
	 * calculate video encoder A2P delay 
	 */
	/* NOTE: video encoder delay is in 27MHz ticks */
	NEXUS_VideoEncoder_GetDelayRange(pContext->videoEncoder, pVideoEncoderConfig, pVideoEncoderStartConfig, &videoDelay);
	BDBG_WRN(("\n\tVideo encoder end-to-end delay = [%u ~ %u] ms", videoDelay.min/27000, videoDelay.max/27000));

	if(pContext->inputSettings.bAudioInput)
	{
		NEXUS_AudioMuxOutput_GetDelayStatus(pContext->audioMuxOutput, pContext->encodeSettings.encoderAudioCodec, &audioDelayStatus);
		BDBG_WRN(("\n\tAudio codec %d end-to-end delay = %u ms", pContext->encodeSettings.encoderAudioCodec, audioDelayStatus.endToEndDelay));

		Dee = audioDelayStatus.endToEndDelay * 27000; /* in 27MHz ticks */
		if(Dee > videoDelay.min)
		{
			if(Dee > videoDelay.max)
			{
				BDBG_ERR(("Audio Dee is way too big! Use video Dee max!"));
				Dee = videoDelay.max;
			}
			else
			{
				BDBG_WRN(("Use audio Dee %u ms %u ticks@27Mhz!", Dee/27000, Dee));
			}
		}
		else
		{
			Dee = videoDelay.min;
			BDBG_WRN(("Use video Dee %u ms %u ticks@27Mhz!", Dee/27000, Dee));
		}
		pVideoEncoderConfig->encoderDelay = Dee;

		/* Start audio mux output */
		NEXUS_AudioMuxOutput_GetDefaultStartSettings(&audioMuxStartSettings);
		/* audio NRT requires mux out to take NRT decode STC */
		audioMuxStartSettings.stcChannel        = pContext->bNonRealTime?
			pContext->stcAudioChannel : pContext->stcChannelTranscode;
		audioMuxStartSettings.presentationDelay = Dee/27000;/* in ms */
		audioMuxStartSettings.nonRealTime       = pContext->bNonRealTime;
		NEXUS_AudioMuxOutput_Start(pContext->audioMuxOutput, &audioMuxStartSettings);

#if BTST_ENABLE_DSP_MIXER
        /* Start audio mixer */
        NEXUS_AudioMixer_Start(pContext->audioMixer);
#endif
	}
	else
		pVideoEncoderConfig->encoderDelay = videoDelay.min;
}


/*******************************
 * Set up stream_mux and record
 */
static void xcode_setup_mux_record( TranscodeContext  *pContext )
{
	NEXUS_FileMuxCreateSettings muxCreateSettings;
	NEXUS_FileMuxStartSettings muxConfig;

    BKNI_CreateEvent(&pContext->finishEvent);
    NEXUS_FileMux_GetDefaultCreateSettings(&muxCreateSettings);
    muxCreateSettings.finished.callback = transcoderFinishCallback;
    muxCreateSettings.finished.context = pContext->finishEvent;
    pContext->fileMux = NEXUS_FileMux_Create(&muxCreateSettings);
    NEXUS_FileMux_GetDefaultStartSettings(&muxConfig, NEXUS_TransportType_eAsf);

    muxConfig.video[0].track = 1;
    muxConfig.video[0].codec = pContext->encodeSettings.encoderVideoCodec;
    muxConfig.video[0].encoder = pContext->videoEncoder;
    muxConfig.audio[0].track = 2;
    muxConfig.audio[0].codec = pContext->encodeSettings.encoderAudioCodec;
    muxConfig.audio[0].muxOutput = pContext->audioMuxOutput;

    snprintf(muxConfig.tempDir, sizeof(muxConfig.tempDir), "videos");
    pContext->muxFile = NEXUS_MuxFile_OpenPosix(pContext->encodeSettings.fname);
    if (!pContext->muxFile) {
        fprintf(stderr, "can't open file:%s\n", pContext->encodeSettings.fname);
        BDBG_ASSERT(0);
    }

    /* start mux */
    NEXUS_FileMux_Start(pContext->fileMux,&muxConfig, pContext->muxFile);

}

#ifdef BDBG_DEBUG_BUILD
static const struct {
    double frequency;
    NEXUS_VideoFrameRate nexusFramerate;
} verticalfrequency[NEXUS_VideoFrameRate_eMax] = {
    {0, NEXUS_VideoFrameRate_eUnknown},
    {23.976, NEXUS_VideoFrameRate_e23_976},
    {24, NEXUS_VideoFrameRate_e24},
    {25, NEXUS_VideoFrameRate_e25},
    {29.970, NEXUS_VideoFrameRate_e29_97},
    {30, NEXUS_VideoFrameRate_e30},
    {50, NEXUS_VideoFrameRate_e50},
    {59.940, NEXUS_VideoFrameRate_e59_94},
    {60, NEXUS_VideoFrameRate_e60},
    {14.985, NEXUS_VideoFrameRate_e14_985}};

static double convert_RefreshRate_FromFrameRate(NEXUS_VideoFrameRate frameRate)
{
    unsigned i;
    for(i=0;i<sizeof(verticalfrequency)/sizeof(*verticalfrequency);i++) {
        if (frameRate == verticalfrequency[i].nexusFramerate) {
            return verticalfrequency[i].frequency;
        }
    }
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return 0; /* NEXUS_VideoFrameRate_eUnknown */
}
#endif

#if BTST_SUPPORT_FRONTEND
static void lock_callback(void *context, int param)
{
	NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
	NEXUS_FrontendQamStatus qamStatus;

	BSTD_UNUSED(param);

	fprintf(stderr, "Lock callback, frontend 0x%08x\n", (unsigned)frontend);

	NEXUS_Frontend_GetQamAsyncStatus(frontend, &qamStatus);
	fprintf(stderr, "QAM Lock callback, frontend 0x%08x - lock status %d, %d\n", (unsigned)frontend,
		qamStatus.fecLock, qamStatus.receiverLock);
}
#endif

#if NEXUS_HAS_HDMI_INPUT
static uint8_t SampleEDID[] = 
{
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x6D, 0x74, 0x22, 0x05, 0x01, 0x11, 0x20,
	0x00, 0x14, 0x01, 0x03, 0x80, 0x00, 0x00, 0x78, 0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29,
	0x17, 0x49, 0x4B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
	0x45, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
	0x58, 0x2C, 0x25, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x42,
	0x43, 0x4D, 0x37, 0x34, 0x32, 0x32, 0x2F, 0x37, 0x34, 0x32, 0x35, 0x0A, 0x00, 0x00, 0x00, 0xFD,
	0x00, 0x17, 0x3D, 0x0F, 0x44, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x89,

	0x02, 0x03, 0x3C, 0x71, 0x7F, 0x03, 0x0C, 0x00, 0x40, 0x00, 0xB8, 0x2D, 0x2F, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xE3, 0x05, 0x1F, 0x01, 0x49, 0x90, 0x05, 0x20, 0x04, 0x03, 0x02, 0x07,
	0x06, 0x01, 0x29, 0x09, 0x07, 0x01, 0x11, 0x07, 0x00, 0x15, 0x07, 0x00, 0x01, 0x1D, 0x00, 0x72,
	0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A,
	0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x18,
	0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0x0B, 0x88, 0x21, 0x00,
	0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D
};

static void
avInfoFrameChangedCallback(void *context, int param)
{
	NEXUS_HdmiInputStatus status;
	NEXUS_HdmiInputHandle hdmiInput = (NEXUS_HdmiInputHandle)context;

	BSTD_UNUSED(param);
	NEXUS_HdmiInput_GetStatus(hdmiInput, &status);
	BDBG_WRN(("HDMI Source AV InfoFrame Change callback: video format %ux%u@%.3f%c", 
		status.avWidth,
		status.avHeight,
		(double)status.vertFreq/1000,
		status.interlaced? 'i' : 'p'));
}
#endif

static void
vidSrcStreamChangedCallback(void *context, int param)
{
	NEXUS_VideoDecoderStreamInformation streamInfo;
	NEXUS_VideoDecoderHandle decoder = (NEXUS_VideoDecoderHandle)context;

	BSTD_UNUSED(param);
	NEXUS_VideoDecoder_GetStreamInformation(decoder, &streamInfo);
	BDBG_WRN(("Video Source Stream Change callback: %ux%u@%.3f%c", 
		streamInfo.sourceHorizontalSize,
		streamInfo.sourceVerticalSize,
		convert_RefreshRate_FromFrameRate(streamInfo.frameRate),
		streamInfo.streamProgressive? 'p' : 'i'));
}

static void play_endOfStreamCallback(void *context, int param)
{
	TranscodeContext  *pContext = (TranscodeContext  *)context;
	BSTD_UNUSED(param);

	printf("Context%d endOfStream\n", pContext->contextId);
	if(pContext->bNonRealTime && !g_bNonRealTimeWrap)
	{
		/* terminate the NRT context */
		B_Event_Set(pContext->eofEvent);
	}
	return;
}

static void nrt_endOfStreamHandler(void *context)
{
	int i;
	TranscodeContext  *pContext = (TranscodeContext  *)context;

	while(B_Event_Wait(pContext->eofEvent, B_WAIT_FOREVER)!=B_ERROR_SUCCESS);
	fprintf(stderr, "Context%d NRT mode ending...\n", pContext->contextId);

	/* stop the NRT context at EOS event */
	B_Mutex_Lock(pContext->mutexStarted);
	if(pContext->bStarted) {
		stop_transcode(pContext);
	}
	if(g_bAutoQuit) { /* when all contexts are stopped, quit automatically */
		for(i=0; i<NEXUS_NUM_VIDEO_ENCODERS; i++) {
			if(xcodeContext[i].bStarted) return;
		}
		if(g_eotEvent) {
			g_keyReturn = 'q';
			B_Event_Set(g_eotEvent);
		}
	}
	B_Mutex_Unlock(pContext->mutexStarted);
	return;
}


static int start_transcode(
	TranscodeContext  *pContext )
{
#if BTST_SUPPORT_FRONTEND
	unsigned i;
	NEXUS_FrontendUserParameters userParams;
	NEXUS_FrontendQamSettings qamSettings;
	NEXUS_ParserBand parserBand;
	NEXUS_ParserBandSettings parserBandSettings;
#endif
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_VideoDecoderSettings videoDecodeSettings;
	NEXUS_VideoEncoderSettings videoEncoderConfig;
	NEXUS_VideoEncoderOpenSettings videoEncoderOpenSettings;
	NEXUS_VideoEncoderStartSettings videoEncoderStartConfig;

	NEXUS_AudioDecoderStartSettings audioProgram;
	NEXUS_AudioEncoderSettings encoderSettings;
	NEXUS_AudioCodec audioCodec;

	NEXUS_PlaybackSettings playbackSettings;
	NEXUS_PlaybackPidChannelSettings playbackPidSettings;

#if NEXUS_HAS_HDMI_INPUT
	NEXUS_HdmiInputSettings hdmiInputSettings;
	NEXUS_TimebaseSettings timebaseSettings;
#endif

#if BTST_ENABLE_AV_SYNC
	NEXUS_SyncChannelSettings syncChannelSettings;
#if BTST_ENABLE_NRT_STC_AV_WINDOW
	NEXUS_StcChannelPairSettings stcAudioVideoPair;
#endif
#endif

	NEXUS_DisplayStgSettings stgSettings;
	NEXUS_DisplaySettings displaySettings;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_VideoWindowScalerSettings scalerSettings;
	NEXUS_VideoFormatInfo fmtInfo;
	bool bInterlacedXcode;
	NEXUS_DisplayCustomFormatSettings customFormatSettings;
	NEXUS_Error rc;
	EncodeSettings  *pEncodeSettings = &pContext->encodeSettings;
	InputSettings   *pInputSettings = &pContext->inputSettings; 

#if NEXUS_HAS_HDMI_INPUT
	if(pInputSettings->resource == BTST_RESOURCE_HDMI) {
		NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0+pContext->contextId, &timebaseSettings);
		timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eHdDviIn;
		NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0+pContext->contextId, &timebaseSettings);
		BDBG_MSG(("Transcoder%d set timebase %d.", pContext->contextId, NEXUS_Timebase_e0+pContext->contextId));

		NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
		hdmiInputSettings.timebase = NEXUS_Timebase_e0+pContext->contextId;
		/* use NEXUS_HdmiInput_OpenWithEdid ()
		    if EDID PROM (U1304 and U1305) is NOT installed; 
		    reference boards usually have the PROMs installed.
		    this example assumes Port1 EDID has been removed 
		*/

		/* all HDMI Tx/Rx combo chips have EDID RAM */
		hdmiInputSettings.useInternalEdid = true ;
		pContext->hdmiInput = NEXUS_HdmiInput_OpenWithEdid(0, &hdmiInputSettings, 
		    &SampleEDID[0], (uint16_t) sizeof(SampleEDID));
		if(!pContext->hdmiInput) {
			fprintf(stderr, "Can't get hdmi input\n");
			return -1;
		}
		NEXUS_HdmiInput_GetSettings(pContext->hdmiInput, &hdmiInputSettings);
		hdmiInputSettings.aviInfoFrameChanged.callback = avInfoFrameChangedCallback;
		hdmiInputSettings.aviInfoFrameChanged.context  = pContext->hdmiInput;
		NEXUS_HdmiInput_SetSettings(pContext->hdmiInput, &hdmiInputSettings);
	}
	else /* TODO: add sync channel support for hdmi source */
#endif
	{
#if BTST_ENABLE_AV_SYNC
		/* create a sync channel */
		NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
		pContext->syncChannel = NEXUS_SyncChannel_Create(&syncChannelSettings);
#endif
	}
	/* each transcoder potentially could use 3 STCs on the same timebase: 
	       1) NRT video decode STC;
	       2) NRT audio decode STC;
	       3) encode/mux STC; 
	   TODO: avoid the 3rd one since NRT mode doesn't really need it, and 
	         encoder/mux STC is only required in RT mode; and in RT mode, a/v decoders share the same STC.
	 */
	NEXUS_StcChannel_GetDefaultSettings(BTST_SOURCE_STC_IDX+pContext->contextId*3, &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0+pContext->contextId;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	/*stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eAudioMaster;*/
	pContext->stcVideoChannel = NEXUS_StcChannel_Open(BTST_SOURCE_STC_IDX+pContext->contextId*3, &stcSettings);
	BDBG_MSG(("Transcoder%d opened source vSTC %d [%p].", pContext->contextId, BTST_SOURCE_STC_IDX+pContext->contextId*3, pContext->stcVideoChannel));

	/* NRT mode uses separate STCs for audio and video decoders; */
	pContext->stcAudioChannel = (pContext->bNonRealTime && pInputSettings->bAudioInput)?
		NEXUS_StcChannel_Open(BTST_NRT_SOURCE_AUDIO_STC_IDX+pContext->contextId*3, &stcSettings) :
		pContext->stcVideoChannel;
	BDBG_MSG(("Transcoder%d opened source aSTC  [%p].", pContext->contextId, pContext->stcAudioChannel));

	/* encoders/mux require different STC broadcast mode from decoder */
	NEXUS_StcChannel_GetDefaultSettings(BTST_ENCODER_STC_IDX+pContext->contextId*3, &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0+pContext->contextId;/* should be the same timebase for end-to-end locking */
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
	pContext->stcChannelTranscode = NEXUS_StcChannel_Open(BTST_ENCODER_STC_IDX+pContext->contextId*3, &stcSettings);
	BDBG_MSG(("Transcoder%d opened encoder STC %d [%p].", pContext->contextId, BTST_ENCODER_STC_IDX+pContext->contextId*3, pContext->stcChannelTranscode));
	
	/* bring up decoder and connect to local display */
	if(pInputSettings->resource != BTST_RESOURCE_HDMI) {
		NEXUS_VideoDecoderOpenSettings openSettings;
		NEXUS_VideoDecoder_GetDefaultOpenSettings(&openSettings);
		openSettings.avc51Enabled = false;
		openSettings.svc3dSupported = false;
		openSettings.excessDirModeEnabled = false;
		openSettings.enhancementPidChannelSupported = false;
		pContext->videoDecoder = NEXUS_VideoDecoder_Open(BTST_XCODE_VID_DECODE_IDX-pContext->contextId, &openSettings); /* take default capabilities */
		BDBG_MSG(("Transcoder%d opened source decoder %d.", pContext->contextId, BTST_XCODE_VID_DECODE_IDX-pContext->contextId));
		NEXUS_VideoDecoder_GetSettings(pContext->videoDecoder, &videoDecodeSettings);
		videoDecodeSettings.streamChanged.callback = vidSrcStreamChangedCallback;
		videoDecodeSettings.streamChanged.context  = pContext->videoDecoder;
		BKNI_Memset(videoDecodeSettings.supportedCodecs, 0, sizeof(videoDecodeSettings.supportedCodecs));
		videoDecodeSettings.supportedCodecs[0] = NEXUS_VideoCodec_eH264; /* it's for regular HD decode heap allocation; it covers mpeg2/h264/mpeg4 HD size */
		NEXUS_VideoDecoder_SetSettings(pContext->videoDecoder, &videoDecodeSettings);

		/****************************************
		 * set up xcoder source
		 */
#if BTST_SUPPORT_FRONTEND
		if(pInputSettings->resource == BTST_RESOURCE_QAM)
		{
			for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
			{
				NEXUS_FrontendCapabilities capabilities;
				pContext->frontend = g_platformConfig.frontend[i];
				if (pContext->frontend) {
					NEXUS_Frontend_GetCapabilities(pContext->frontend, &capabilities);
					/* Does this frontend support qam? */
					if ( capabilities.qam )
					{
						break;
					}
				}
			}
			
			if (NULL == pContext->frontend )
			{
				fprintf(stderr, "Unable to find QAM-capable frontend\n");
				return -1;
			}
			
			NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
			qamSettings.frequency = pInputSettings->freq* 1000000;
			qamSettings.mode = pInputSettings->qamMode;
			switch (pInputSettings->qamMode) {
						case NEXUS_FrontendQamMode_e64: qamSettings.symbolRate = 5056900; break;
						case NEXUS_FrontendQamMode_e256: qamSettings.symbolRate = 5360537; break;
						case NEXUS_FrontendQamMode_e1024: qamSettings.symbolRate = 0; /* TODO */ break;
						default: fprintf(stderr, "Invalid QAM mode!\n"); return -1;
			}
			qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
			qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e6Mhz;
			qamSettings.lockCallback.callback = lock_callback;
			qamSettings.lockCallback.context = pContext->frontend;
			
			NEXUS_Frontend_GetUserParameters(pContext->frontend, &userParams);
			
			/* Map a parser band to the demod's input band. */
			parserBand = NEXUS_ParserBand_e0+pContext->contextId;
			NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
			if (userParams.isMtsif) {
				parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
				parserBandSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(pContext->frontend); /* NEXUS_Frontend_TuneXyz() will connect this frontend to this parser band */
			}
			else {
				parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
				parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
			}
			parserBandSettings.transportType = NEXUS_TransportType_eTs;
			NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
			pContext->videoPidChannel = NEXUS_PidChannel_Open(parserBand, pInputSettings->iVideoPid, NULL);
			BDBG_MSG(("Transcoder%d opened PID channel for parser band %d.", pContext->contextId, parserBand));

			/* Tuner input has live PCR locked timebase */
			NEXUS_StcChannel_GetSettings(pContext->stcVideoChannel, &stcSettings);
			stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
			stcSettings.modeSettings.pcr.pidChannel = pContext->videoPidChannel; /* PCR happens to be on video pid */
			NEXUS_StcChannel_SetSettings(pContext->stcVideoChannel, &stcSettings);/* live input must use RT mode and a/v decoders share the same STC */
			NEXUS_Frontend_TuneQam(pContext->frontend, &qamSettings);
		}
		else
#endif
		{
			/* 4 playpumps per transcoder context: 1x src + 3x mux playpumps; */
			pContext->playpump = NEXUS_Playpump_Open(BTST_SOURCE_PLAYPUMP_IDX+pContext->contextId*4, NULL);
			assert(pContext->playpump);
			BDBG_MSG(("Transcoder%d opened source playpump %d.", pContext->contextId, BTST_SOURCE_PLAYPUMP_IDX+pContext->contextId*4));
			pContext->playback = NEXUS_Playback_Create();
			assert(pContext->playback);

			pContext->file = NEXUS_FilePlay_OpenPosix(pInputSettings->fname, NULL);
			if (!pContext->file) {
				fprintf(stderr, "can't open file:%s\n", pInputSettings->fname);
				return -1;
			}
			
			NEXUS_Playback_GetSettings(pContext->playback, &playbackSettings);
			playbackSettings.playpump = pContext->playpump;
			/* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
			playbackSettings.playpumpSettings.transportType = (NEXUS_TransportType)pInputSettings->eStreamType;
			playbackSettings.stcChannel = pContext->bNonRealTime? 
				pContext->stcChannelTranscode : /* NRT mode won't use playback stc channel; just assign it something to pass nexus validation */
				pContext->stcVideoChannel; /* RT mode playback shares the sasme STC channel as a/v decoders */

			/* NRT mode file transcode doesn not need loop */
			playbackSettings.endOfStreamAction = (pContext->bNonRealTime && !g_bNonRealTimeWrap)? NEXUS_PlaybackLoopMode_ePause : NEXUS_PlaybackLoopMode_eLoop; /* when play hits the end, wait for record */
			playbackSettings.endOfStreamCallback.callback = play_endOfStreamCallback;
			playbackSettings.endOfStreamCallback.context  = pContext;
			NEXUS_Playback_SetSettings(pContext->playback, &playbackSettings);

			/* Open the video pid channel */
			NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
			playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
			playbackPidSettings.pidTypeSettings.video.codec = (NEXUS_VideoCodec)pInputSettings->eVideoCodec; /* must be told codec for correct handling */
			playbackPidSettings.pidTypeSettings.video.index = true;
			playbackPidSettings.pidTypeSettings.video.decoder = pContext->videoDecoder;
			pContext->videoPidChannel = NEXUS_Playback_OpenPidChannel(pContext->playback, pInputSettings->iVideoPid, &playbackPidSettings);
		}

		/* Set up decoder Start structures now. We need to know the audio codec to properly set up
		the audio outputs. */
		NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
		videoProgram.codec = (NEXUS_VideoCodec)pInputSettings->eVideoCodec;
		videoProgram.pidChannel = pContext->videoPidChannel;
		videoProgram.stcChannel = pContext->stcVideoChannel;
		videoProgram.nonRealTime = pContext->bNonRealTime;
	}

	/****************************************
	 * Bring up video display and outputs 
	 */
	if(pContext->contextId == 0) {/* only simul display the 1st context source for now */
		NEXUS_Display_GetDefaultSettings(&displaySettings);
		displaySettings.format = 
			(pEncodeSettings->encoderFrameRate == NEXUS_VideoFrameRate_e25 || 
			 pEncodeSettings->encoderFrameRate == NEXUS_VideoFrameRate_e50) ?
			NEXUS_VideoFormat_e576p: NEXUS_VideoFormat_e480p;
		pContext->display = NEXUS_Display_Open(BTST_SOURCE_DISPLAY_IDX, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
	if(g_platformConfig.outputs.component[0]){
		NEXUS_Display_AddOutput(pContext->display, NEXUS_ComponentOutput_GetConnector(g_platformConfig.outputs.component[0]));
	}
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
		if(g_platformConfig.outputs.hdmi[0]){
			NEXUS_Display_AddOutput(pContext->display, NEXUS_HdmiOutput_GetVideoConnector(g_platformConfig.outputs.hdmi[0]));
		}
#endif
		pContext->window = NEXUS_VideoWindow_Open(pContext->display, 0);
	}

	/* NOTE: must open video encoder before display; otherwise open will init ViCE2 core
	* which might cause encoder display GISB error since encoder display would
	* trigger RDC to program mailbox registers in ViCE2;
	*/
	NEXUS_VideoEncoder_GetDefaultOpenSettings(&videoEncoderOpenSettings);
	videoEncoderOpenSettings.type = g_eVideoEncoderType;
	pContext->videoEncoder = NEXUS_VideoEncoder_Open(pContext->contextId, &videoEncoderOpenSettings);
	assert(pContext->videoEncoder);
	BDBG_MSG(("Transcoder%d opened video encoder %d.", pContext->contextId, pContext->contextId));

	/* Bring up video encoder display */
	NEXUS_Display_GetDefaultSettings(&displaySettings);
	displaySettings.displayType = NEXUS_DisplayType_eAuto;
	displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eEncoder;
	displaySettings.frameRateMaster = NULL;/* disable frame rate tracking for now */

	if(!pEncodeSettings->bCustom)
	{
		displaySettings.format = (NEXUS_VideoFormat)pEncodeSettings->displayFormat;
		pContext->displayTranscode = NEXUS_Display_Open(g_encodeDisplayId[pContext->contextId], &displaySettings);
		assert(pContext->displayTranscode);
		BDBG_MSG(("Transcoder%d opened encoder display%d [%p].", pContext->contextId, NEXUS_ENCODER_DISPLAY_IDX-pContext->contextId, pContext->displayTranscode));
		pContext->windowTranscode = NEXUS_VideoWindow_Open(pContext->displayTranscode, 0);
		assert(pContext->windowTranscode);
		NEXUS_VideoFormat_GetInfo(displaySettings.format, &fmtInfo);
		bInterlacedXcode = fmtInfo.interlaced;
	}
	else
	{
		pContext->displayTranscode = NEXUS_Display_Open(NEXUS_ENCODER_DISPLAY_IDX-pContext->contextId, &displaySettings);
		assert(pContext->displayTranscode);
		BDBG_MSG(("Transcoder%d opened encoder display%d [%p].", pContext->contextId, NEXUS_ENCODER_DISPLAY_IDX-pContext->contextId, pContext->displayTranscode));
		pContext->windowTranscode = NEXUS_VideoWindow_Open(pContext->displayTranscode, 0);
		assert(pContext->windowTranscode);

		NEXUS_Display_GetDefaultCustomFormatSettings(&customFormatSettings);
		customFormatSettings.width = pEncodeSettings->customFormatSettings.width;
		customFormatSettings.height = pEncodeSettings->customFormatSettings.height;
		customFormatSettings.refreshRate = pEncodeSettings->customFormatSettings.refreshRate;
		customFormatSettings.interlaced = pEncodeSettings->customFormatSettings.interlaced;
		customFormatSettings.aspectRatio = pEncodeSettings->customFormatSettings.aspectRatio;
		customFormatSettings.sampleAspectRatio.x = pEncodeSettings->customFormatSettings.sampleAspectRatio.x;
		customFormatSettings.sampleAspectRatio.y = pEncodeSettings->customFormatSettings.sampleAspectRatio.y;
		customFormatSettings.dropFrameAllowed = true;
		rc = NEXUS_Display_SetCustomFormatSettings(pContext->displayTranscode, NEXUS_VideoFormat_eCustom2, &customFormatSettings);
		assert(!rc);
		bInterlacedXcode = customFormatSettings.interlaced;
	}
	/* NRT setup - AFAP mode */
	NEXUS_Display_GetStgSettings(pContext->displayTranscode, &stgSettings);
	stgSettings.enabled     = true;
	stgSettings.nonRealTime = pContext->bNonRealTime;
	NEXUS_Display_SetStgSettings(pContext->displayTranscode, &stgSettings);

	/* set transcoder minimum display format before AddInput to avoid black frame transition during dynamic resolution change */
	NEXUS_VideoWindow_GetSettings(pContext->windowTranscode, &windowSettings);
	windowSettings.minimumDisplayFormat = NEXUS_VideoFormat_e1080p;
	windowSettings.scaleFactorRounding.enabled = false;
	windowSettings.scaleFactorRounding.horizontalTolerance = 0;
	windowSettings.scaleFactorRounding.verticalTolerance   = 0;
	NEXUS_VideoWindow_SetSettings(pContext->windowTranscode, &windowSettings);

	/* set transcoder window vnet mode bias to avoid black frame transition during dynamic resolution change */
	NEXUS_VideoWindow_GetScalerSettings(pContext->windowTranscode, &scalerSettings);
	scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerAfterCapture;
	scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
	NEXUS_VideoWindow_SetScalerSettings(pContext->windowTranscode, &scalerSettings);

	/* connect same decoder to the encoder display;
	* NOTE: simul display + transcode mode might have limitation in audio path;
	* here is for video transcode bringup/debug purpose;
	*/
	if(pInputSettings->resource == BTST_RESOURCE_HDMI) {
#if NEXUS_HAS_HDMI_INPUT
		NEXUS_VideoWindow_AddInput(pContext->windowTranscode, NEXUS_HdmiInput_GetVideoConnector(pContext->hdmiInput));
#endif
	}
	else
	{
		NEXUS_VideoWindow_AddInput(pContext->windowTranscode, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
	}

	/* xcode window PQ default setting */
	window_dnr(pContext->windowTranscode, false);

	/* VDC madr assignment is fixed */
	window_mad(pContext->windowTranscode, false, true, pContext->bLowDelay?true:false);

	/* temporarily disable simul display for NRT mode since the nexus sync channel tried to add vsync delay to balance two BVN path delays
	   which caused forced capture on NRT window that's not right; 
	   TODO: remove this when sync channel is fixed for NRT mode. */
#if BTST_DISABLE_NRT_DEBUG_DISPLAY
	if(!pContext->bNonRealTime)
#endif
	{
		if(pContext->contextId == 0) {/* only simul display the 1st context source for now */
			if(pInputSettings->resource == BTST_RESOURCE_HDMI) {
#if NEXUS_HAS_HDMI_INPUT
				NEXUS_VideoWindow_AddInput(pContext->window, NEXUS_HdmiInput_GetVideoConnector(pContext->hdmiInput));
#endif
			}
			else {
				NEXUS_VideoWindow_AddInput(pContext->window, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
			}
		}
	}
#if !BTST_DISABLE_NRT_DEBUG_DISPLAY /* SW workaround for simul debug display path with NRT mode transcode */
	if(pContext->bNonRealTime && (pContext->contextId == 0)) /* crop debug display to force the BVN eop order so that STG eop always fire later than mfd and simul capture path. */
	{
		NEXUS_VideoWindow_GetSettings(pContext->window, &windowSettings);
		windowSettings.sourceClip.bottom = 10; /* source cropping on debug monitor path may work around early EOP trigger issue */
		windowSettings.scaleFactorRounding.enabled = false;/* disable SFR to avoid it cancelling the source crop */
		NEXUS_VideoWindow_SetSettings(pContext->window, &windowSettings);
	}
#endif

	/**************************************
	 * encoder settings 
	 */
	NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
	videoEncoderConfig.variableFrameRate = false;
	videoEncoderConfig.frameRate = pEncodeSettings->encoderFrameRate;
	videoEncoderConfig.bitrateMax = pEncodeSettings->encoderBitrate;
	videoEncoderConfig.streamStructure.framesP = pEncodeSettings->encoderGopStructureFramesP;
	videoEncoderConfig.streamStructure.framesB = pEncodeSettings->encoderGopStructureFramesB;

	NEXUS_VideoEncoder_GetDefaultStartSettings(&videoEncoderStartConfig);
	videoEncoderStartConfig.codec = pEncodeSettings->encoderVideoCodec;
	videoEncoderStartConfig.profile = pEncodeSettings->encoderProfile;
	videoEncoderStartConfig.level = pEncodeSettings->encoderLevel;
	videoEncoderStartConfig.input = pContext->displayTranscode;
	videoEncoderStartConfig.stcChannel = pContext->stcChannelTranscode;
	videoEncoderStartConfig.interlaced = bInterlacedXcode;
	videoEncoderStartConfig.nonRealTime = pContext->bNonRealTime;
	videoEncoderStartConfig.lowDelayPipeline = pContext->bLowDelay;

	/*********************************
	 * Set up audio xcoder 
	 */
	if(pInputSettings->bAudioInput)
	{
#if BTST_ENABLE_DSP_MIXER
		NEXUS_AudioMixerSettings audioMixerSettings;
#endif
		/* Open the audio decoder */
		/* Connect audio decoders to outputs */
		if(pEncodeSettings->bAudioEncode || (pInputSettings->resource == BTST_RESOURCE_HDMI))
		{
			pContext->audioDecoder = NEXUS_AudioDecoder_Open(pContext->contextId, NULL);
		}
		else
		{
			pContext->audioPassthrough = NEXUS_AudioDecoder_Open(pContext->contextId, NULL);
		}

		/* Open the audio and pcr pid channel */
#if BTST_SUPPORT_FRONTEND
		if(pInputSettings->resource == BTST_RESOURCE_QAM)
		{
			pContext->audioPidChannel = NEXUS_PidChannel_Open(parserBand, pInputSettings->iAudioPid, NULL);
		}
		else 
#endif
		if(pInputSettings->resource == BTST_RESOURCE_FILE)
		{
			NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
			playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
			playbackPidSettings.pidTypeSettings.audio.primary = pEncodeSettings->bAudioEncode?
				pContext->audioDecoder: /* must be told codec for correct handling */
				pContext->audioPassthrough;
			pContext->audioPidChannel = NEXUS_Playback_OpenPidChannel(pContext->playback, pInputSettings->iAudioPid, &playbackPidSettings);
		}

		/* Set up decoder Start structures now. We need to know the audio codec to properly set up
		the audio outputs. */
		NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
		if(pInputSettings->resource == BTST_RESOURCE_HDMI) {
#if NEXUS_HAS_HDMI_INPUT
			audioProgram.input = NEXUS_HdmiInput_GetAudioConnector(pContext->hdmiInput);
#endif
		} else {
			audioProgram.codec = pInputSettings->eAudioCodec;
			audioProgram.pidChannel = pContext->audioPidChannel;
			audioProgram.nonRealTime= pContext->bNonRealTime;
		}
		audioProgram.stcChannel = pContext->stcAudioChannel;

#if BTST_ENABLE_DSP_MIXER
		/* Open audio mixer.  The mixer can be left running at all times to provide continuous audio output despite input discontinuities.  */
		NEXUS_AudioMixer_GetDefaultSettings(&audioMixerSettings);
		audioMixerSettings.mixUsingDsp = true;
		pContext->audioMixer = NEXUS_AudioMixer_Open(&audioMixerSettings);
		assert(pContext->audioMixer);
#endif

		/* Open audio mux output */
		pContext->audioMuxOutput = NEXUS_AudioMuxOutput_Create(NULL);
		assert(pContext->audioMuxOutput);

		if(pEncodeSettings->bAudioEncode || (pInputSettings->resource == BTST_RESOURCE_HDMI))
		{
			/* Open audio encoder */
			NEXUS_AudioEncoder_GetDefaultSettings(&encoderSettings);
			encoderSettings.codec = pEncodeSettings->encoderAudioCodec;
			audioCodec = pEncodeSettings->encoderAudioCodec;
			pContext->audioEncoder = NEXUS_AudioEncoder_Open(&encoderSettings);
			assert(pContext->audioEncoder);
#if BTST_ENABLE_DSP_MIXER
			/* Connect decoder to mixer and set as master */
			NEXUS_AudioMixer_AddInput(pContext->audioMixer,
				NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
			audioMixerSettings.master = NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
			NEXUS_AudioMixer_SetSettings(pContext->audioMixer, &audioMixerSettings);
			/* Connect mixer to encoder */
			NEXUS_AudioEncoder_AddInput(pContext->audioEncoder, NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
#else
            /* Connect decoder to encoder */
            NEXUS_AudioEncoder_AddInput(pContext->audioEncoder, 
                NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif
			/* Connect mux to encoder */
			NEXUS_AudioOutput_AddInput(
				NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput), 
				NEXUS_AudioEncoder_GetConnector(pContext->audioEncoder));
		}
		else
		{
#if BTST_ENABLE_DSP_MIXER
			NEXUS_AudioMixer_AddInput(pContext->audioMixer,
				NEXUS_AudioDecoder_GetConnector(pContext->audioPassthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
			audioMixerSettings.master = NEXUS_AudioDecoder_GetConnector(pContext->audioPassthrough, NEXUS_AudioDecoderConnectorType_eCompressed);
			NEXUS_AudioMixer_SetSettings(pContext->audioMixer, &audioMixerSettings);
			NEXUS_AudioOutput_AddInput(
				NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput),
				NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
#else
            NEXUS_AudioOutput_AddInput(
                NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput),
                NEXUS_AudioDecoder_GetConnector(pContext->audioPassthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
#endif
			audioCodec = audioProgram.codec;
		}

		/* Attach outputs for real-time transcoding */
		if(!pContext->bNonRealTime)
		{
#if BTST_ENABLE_DSP_MIXER
			if(pEncodeSettings->bAudioEncode || (pInputSettings->resource == BTST_RESOURCE_HDMI))
			{
				NEXUS_AudioOutput_AddInput(
					NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]),
					NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
			}
			else
			{
				NEXUS_AudioOutput_AddInput(
					NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]),
					NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
			}
#else
            if(pEncodeSettings->bAudioEncode || (pInputSettings->resource == BTST_RESOURCE_HDMI))
            {
                NEXUS_AudioOutput_AddInput(
                    NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]),
                    NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
            }
            else
            {
                NEXUS_AudioOutput_AddInput(
                    NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]),
                    NEXUS_AudioDecoder_GetConnector(pContext->audioPassthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
			}
#endif
		}
	}

	/************************************
	 * Set up encoder AV sync.
	 * encode setting and startSetting to be set after end-to-end delay is determined */
	/* get end-to-end delay (Dee) for audio and video encoders;
	* TODO: match AV delay! In other words,
	*   if (aDee > vDee) {
	*       vDee' = aDee' = aDee;
	*   }
	*   else {
	*       vDee' = aDee' = vDee;
	*   }
	*/
	xcode_av_sync(pContext, &videoEncoderConfig, &videoEncoderStartConfig);

	/****************************8
	 * start video encoder
	 */
	NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
#ifdef BTST_FIXED_SW7425_139
	NEXUS_VideoEncoder_Start(pContext->videoEncoder, &videoEncoderStartConfig);
#endif
	/************************************************
	 * Set up xcoder stream_mux and record
	 */
	xcode_setup_mux_record(pContext);

	/****************************
	 * set up decoder AV sync
	 */
	if(pInputSettings->resource != BTST_RESOURCE_HDMI) {
#if BTST_ENABLE_AV_SYNC
		/* connect sync channel */
		NEXUS_SyncChannel_GetSettings(pContext->syncChannel, &syncChannelSettings);
		syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder);
		if(pInputSettings->bAudioInput)
		{
			if(pEncodeSettings->bAudioEncode)
				syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo);
			else
				syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(pContext->audioPassthrough, NEXUS_AudioDecoderConnectorType_eCompressed);
#if BTST_ENABLE_NRT_STC_AV_WINDOW
			/* NRT mode pairs AV stc channels */
			if(pContext->bNonRealTime) {
				NEXUS_StcChannel_GetDefaultPairSettings(&stcAudioVideoPair);
				stcAudioVideoPair.connected = true;
				stcAudioVideoPair.window = 300; /* 300ms AV window means when source discontinuity occurs, up to 300ms transition could occur with NRT transcoded stream */
				NEXUS_StcChannel_SetPairSettings(pContext->stcVideoChannel, pContext->stcAudioChannel, &stcAudioVideoPair);

				/* disable these flags for NRT mode to avoid startup mute */
				syncChannelSettings.enableMuteControl = false;
				syncChannelSettings.enablePrecisionLipsync = false;
			}
#endif
		}

		NEXUS_SyncChannel_SetSettings(pContext->syncChannel, &syncChannelSettings);
#endif

		/* Start decoder */
		NEXUS_VideoDecoder_Start(pContext->videoDecoder, &videoProgram);
	}
	if(pInputSettings->bAudioInput)
	{
		if(pEncodeSettings->bAudioEncode || (pInputSettings->resource == BTST_RESOURCE_HDMI))
			NEXUS_AudioDecoder_Start(pContext->audioDecoder, &audioProgram);
		else
			NEXUS_AudioDecoder_Start(pContext->audioPassthrough, &audioProgram);
	}
	
	/* Start playback */
	if(pInputSettings->resource == BTST_RESOURCE_FILE)
	{
		NEXUS_Playback_Start(pContext->playback, pContext->file, NULL);
	}

#ifndef BTST_FIXED_SW7425_139
	NEXUS_VideoEncoder_Start(pContext->videoEncoder, &videoEncoderStartConfig);
#endif
	/* started */
	pContext->bStarted = true;

	/* increment NRT reference count */
	if(pContext->bNonRealTime && (NULL==pContext->eofEvent)) {
		pContext->eofEvent = B_Event_Create(NULL);
		B_Event_Reset(pContext->eofEvent);
		pContext->nrtEofHandle = B_Thread_Create("NRT mode EOF handler", (B_ThreadFunc)nrt_endOfStreamHandler, pContext, NULL);
	}
	return 0;
}

static void stop_transcode(
	TranscodeContext  *pContext )
{
	/************************************
	 * Bring down system 
	 */
	if(!pContext->bStarted) {
		return;
	}

	/**************************************************
	 * NOTE: stop sequence should be in front->back order
	 */
	if(pContext->inputSettings.resource == BTST_RESOURCE_FILE)
	{
		NEXUS_Playback_Stop(pContext->playback);
	}
	if(pContext->inputSettings.resource != BTST_RESOURCE_HDMI) {
		NEXUS_VideoDecoder_Stop(pContext->videoDecoder);
		if(pContext->inputSettings.bAudioInput)
		{
		
			if(pContext->encodeSettings.bAudioEncode)
				NEXUS_AudioDecoder_Stop(pContext->audioDecoder);
			else
				NEXUS_AudioDecoder_Stop(pContext->audioPassthrough);
#if BTST_ENABLE_DSP_MIXER
			NEXUS_AudioMixer_Stop(pContext->audioMixer);
#endif
			NEXUS_AudioMuxOutput_Stop(pContext->audioMuxOutput);
		}
#if BTST_ENABLE_AV_SYNC
		/* disconnect sync channel after decoders stop */
		NEXUS_SyncChannel_Destroy(pContext->syncChannel);
#endif
	}
	NEXUS_VideoEncoder_Stop(pContext->videoEncoder, NULL);
	NEXUS_FileMux_Finish(pContext->fileMux);
	if(BKNI_WaitForEvent(pContext->finishEvent, 4000)!=BERR_SUCCESS) {
		fprintf(stderr, "TIMEOUT\n");
	}
	BKNI_DestroyEvent(pContext->finishEvent);
	/*****************************************
	 * Note:  
	 * mux stop  
	 */
	NEXUS_FileMux_Stop(pContext->fileMux);

	NEXUS_MuxFile_Close(pContext->muxFile);

	if(pContext->inputSettings.resource == BTST_RESOURCE_FILE)
	{
		if(pContext->inputSettings.bAudioInput)
			NEXUS_Playback_ClosePidChannel(pContext->playback, pContext->audioPidChannel);
		NEXUS_Playback_ClosePidChannel(pContext->playback, pContext->videoPidChannel);
		NEXUS_FilePlay_Close(pContext->file);
		NEXUS_Playback_Destroy(pContext->playback);
		NEXUS_Playpump_Close(pContext->playpump);
	}
	else if(pContext->inputSettings.resource == BTST_RESOURCE_QAM)
	{
		if(pContext->inputSettings.bAudioInput)
			NEXUS_PidChannel_Close(pContext->audioPidChannel);
		NEXUS_PidChannel_Close(pContext->videoPidChannel);
	}

	/******************************************
	 * nexus kernel mode requires explicit remove/shutdown video inputs before close windows/display
	 */
#if NEXUS_HAS_HDMI_INPUT
	if(pContext->inputSettings.resource == BTST_RESOURCE_HDMI) {
		NEXUS_VideoInput_Shutdown(NEXUS_HdmiInput_GetVideoConnector(pContext->hdmiInput));
	}
	else 
#endif
	{
#if BTST_DISABLE_NRT_DEBUG_DISPLAY
		if(!pContext->bNonRealTime && (pContext->contextId == 0))
#endif
		{
			NEXUS_VideoWindow_RemoveInput(pContext->window, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
		}
		NEXUS_VideoWindow_RemoveInput(pContext->windowTranscode, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
		NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
		NEXUS_VideoDecoder_Close(pContext->videoDecoder);
	}
	if(pContext->contextId == 0)
		NEXUS_VideoWindow_Close(pContext->window);
	NEXUS_VideoWindow_Close(pContext->windowTranscode);
	if(pContext->contextId == 0)
		NEXUS_Display_Close(pContext->display);
	NEXUS_Display_Close(pContext->displayTranscode);

	NEXUS_FileMux_Destroy(pContext->fileMux);

	NEXUS_VideoEncoder_Close(pContext->videoEncoder);
	
	if(pContext->inputSettings.bAudioInput)
	{
		if(pContext->encodeSettings.bAudioEncode || (pContext->inputSettings.resource == BTST_RESOURCE_HDMI))
		{
			NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput));
			NEXUS_AudioEncoder_RemoveAllInputs(pContext->audioEncoder);
			NEXUS_AudioInput_Shutdown(NEXUS_AudioEncoder_GetConnector(pContext->audioEncoder));
			NEXUS_AudioEncoder_Close(pContext->audioEncoder);
#if BTST_ENABLE_DSP_MIXER
			NEXUS_AudioMixer_RemoveAllInputs(pContext->audioMixer);
#endif
			if(!pContext->bNonRealTime) {
				NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]));
				NEXUS_AudioOutput_Shutdown(NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]));
			}
#if BTST_ENABLE_DSP_MIXER
			NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
			NEXUS_AudioMixer_Close(pContext->audioMixer);
#endif
			NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
			NEXUS_AudioDecoder_Close(pContext->audioDecoder);
		}
		else
		{
			NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput));
#if BTST_ENABLE_DSP_MIXER
			NEXUS_AudioMixer_RemoveAllInputs(pContext->audioMixer);
#endif
			if(!pContext->bNonRealTime) {
				NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]));
				NEXUS_AudioOutput_Shutdown(NEXUS_AudioDummyOutput_GetConnector(g_platformConfig.outputs.audioDummy[pContext->contextId]));
			}
#if BTST_ENABLE_DSP_MIXER
			NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
			NEXUS_AudioMixer_Close(pContext->audioMixer);
#endif
			NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pContext->audioPassthrough, NEXUS_AudioDecoderConnectorType_eCompressed));
			NEXUS_AudioDecoder_Close(pContext->audioPassthrough);
		}
	
		NEXUS_AudioOutput_Shutdown(NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput));
		NEXUS_AudioMuxOutput_Destroy(pContext->audioMuxOutput);

		if(pContext->bNonRealTime)
		{
			NEXUS_StcChannel_Close(pContext->stcAudioChannel);
		}
	}

	NEXUS_StcChannel_Close(pContext->stcVideoChannel);
	NEXUS_StcChannel_Close(pContext->stcChannelTranscode);

#if NEXUS_HAS_HDMI_INPUT
	if(pContext->inputSettings.resource == BTST_RESOURCE_HDMI) {
		NEXUS_HdmiInput_Close(pContext->hdmiInput);
	}
#endif
	/* started */
	pContext->bStarted = false;
}

static void keyHandler( TranscodeContext  xcodeContext[] )
{
	NEXUS_VideoEncoderSettings videoEncoderConfig;
	NEXUS_VideoWindowSplitScreenSettings splitSettings;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_VideoWindowCoefficientIndexSettings stCoeffIndex;
	NEXUS_DisplaySettings displaySettings;
	NEXUS_DisplayCustomFormatSettings customFormatSettings;
	char key[256];
	int choice, input;
	bool enableMad;
	NEXUS_VideoEncoderSettingsOnInputChange encodeSettingsOnInputChange;
	NEXUS_VideoEncoderStatus videoEncodeStatus;
	NEXUS_VideoDecoderSettings videoDecoderSettings;
	NEXUS_Error rc;
	uint32_t ulSecToSleep;
	TranscodeContext *pContext = &xcodeContext[g_selectedXcodeContextId];

	g_keyReturn = 'q';
	for(;;)
	{
		printf("Menu:\n");
		printf(" 1) change video encoder resolution\n");
		printf(" 2) change video encoder bitrate\n");
		printf(" 3) change video encoder frame rate\n");
		printf(" 4) change xcode video window MAD settings\n");
		printf(" 5) change xcode video window DNR settings\n");
		printf(" 7) change xcode window DNR demo mode\n");
		printf("11) change source video decoder scanout mode\n");
		printf("12) change xcode aspect ratio correction mode\n");
		printf("13) change xcode window scaler coeff index\n");
		printf("14) Get video encoder status\n");
		printf("15) Select xcoder context to config\n");
		printf("20) Start selected transcoder\n");
		printf("21) Stop  selected transcoder\n");
		printf("99) change DEBUG module setting\n");
		printf("100) sleep\n");
		printf(" c) Enter 'c' to stop/start the next iteration\n\n");
		printf("\nEnter 'q' to quit\n\n");
		scanf("%s", key);
		if(!strcmp(key, "q")) 
		{
			g_keyReturn = 'q';
			break;
		}
		else if(!strcmp(key, "c")) {
			g_keyReturn = 'c';
			break;
		}
		choice = strtoul(key, NULL, 0);
		B_Mutex_Lock(pContext->mutexStarted);
		if(!pContext->bStarted && (choice!=20) && (choice!=15)) {
			printf("context %d not started yet.\n", pContext->contextId);
			B_Mutex_Unlock(pContext->mutexStarted);
			continue;
		}
		switch(choice)
		{
			case 1: /* resolution change */
				printf("xcode resolution change:   ");

				/* typically resolution change combined with bit rate change */
				printf("Do you want to change bit rate synchronously as well? 0)No 1)Yes\n");
				scanf("%d", &input);
				if(input)
				{
					printf("Bit rate change from %u to:   ", pContext->encodeSettings.encoderBitrate);
					printf("\n Bitrate (bps)	"); 
					scanf("%d", (int32_t*)&pContext->encodeSettings.encoderBitrate);

					/* turn on the synchronous change feature first before resolution/bitrate change! */
					NEXUS_VideoEncoder_GetSettingsOnInputChange(pContext->videoEncoder, &encodeSettingsOnInputChange);
					encodeSettingsOnInputChange.bitrateMax = pContext->encodeSettings.encoderBitrate;
					NEXUS_VideoEncoder_SetSettingsOnInputChange(pContext->videoEncoder, &encodeSettingsOnInputChange);
				}

				printf("\n custom format (0) No (1) Yes "); 
				scanf("%d", &choice); pContext->encodeSettings.bCustom = choice;
				if(!pContext->encodeSettings.bCustom)
				{
					printf("\n displayFormat:\n"
						" (%2d) 480i\n"
						" (%2d) 576i\n"
						" (%2d) 480p\n"
						" (%2d) 576p\n"
						" (%2d) 1080i60\n"
						" (%2d) 1080i50\n"
						" (%2d) 1080p24\n"
						" (%2d) 1080p25\n"
						" (%2d) 1080p30\n"
						" (%2d) 1080p50\n"
						" (%2d) 1080p60\n"
						" (%2d) 720p60\n"
						" (%2d) 720p50\n"
						" (%2d) 720p24\n"
						" (%2d) 720p25\n"
						" (%2d) 720p30Hz:\n",
						(NEXUS_VideoFormat_eNtsc),
						(NEXUS_VideoFormat_ePal),
						(NEXUS_VideoFormat_e480p),
						(NEXUS_VideoFormat_e576p),
						(NEXUS_VideoFormat_e1080i),
						(NEXUS_VideoFormat_e1080i50hz),
						(NEXUS_VideoFormat_e1080p24hz),
						(NEXUS_VideoFormat_e1080p25hz),
						(NEXUS_VideoFormat_e1080p30hz),
						(NEXUS_VideoFormat_e1080p50hz),
						(NEXUS_VideoFormat_e1080p60hz),
						(NEXUS_VideoFormat_e720p),
						(NEXUS_VideoFormat_e720p50hz),
						(NEXUS_VideoFormat_e720p24hz),
						(NEXUS_VideoFormat_e720p25hz),
						(NEXUS_VideoFormat_e720p30hz)); 
					scanf("%d", (int32_t*)&pContext->encodeSettings.displayFormat);
					NEXUS_Display_GetSettings(pContext->displayTranscode, &displaySettings);
					displaySettings.format = pContext->encodeSettings.displayFormat;
					NEXUS_Display_SetSettings(pContext->displayTranscode, &displaySettings);
				}
				else
				{
					printf("\n Resolution width & height	");
					scanf("%d %d", (int32_t*)&pContext->encodeSettings.customFormatSettings.width, (int32_t*)&pContext->encodeSettings.customFormatSettings.height);
					printf("\n Interlaced? (0) No (1) Yes	"); 
					scanf("%d", &choice); pContext->encodeSettings.customFormatSettings.interlaced = choice;
					printf("\n vertical refresh rate (in 1/1000th Hz) 	"); 
					scanf("%d", (int32_t*)&pContext->encodeSettings.customFormatSettings.refreshRate);
					printf("\n Aspect Ratio:\n"
						" (%d) Auto\n"
						" (%d) 4x3\n"
						" (%d) 16x9\n"
						" (%d) SAR\n",
						NEXUS_DisplayAspectRatio_eAuto,
						NEXUS_DisplayAspectRatio_e4x3,
						NEXUS_DisplayAspectRatio_e16x9,
						NEXUS_DisplayAspectRatio_eSar); 
					scanf("%d", (int32_t*)&pContext->encodeSettings.customFormatSettings.aspectRatio);
					if(NEXUS_DisplayAspectRatio_eSar == pContext->encodeSettings.customFormatSettings.aspectRatio)
					{
						printf("Please enter Sample Aspect Ratio X and Y: \n");
						scanf("%u %u", &pContext->encodeSettings.customFormatSettings.sampleAspectRatio.x,
							&pContext->encodeSettings.customFormatSettings.sampleAspectRatio.y);
					}
					NEXUS_Display_GetDefaultCustomFormatSettings(&customFormatSettings);
					customFormatSettings.width = pContext->encodeSettings.customFormatSettings.width;
					customFormatSettings.height = pContext->encodeSettings.customFormatSettings.height;
					customFormatSettings.refreshRate = pContext->encodeSettings.customFormatSettings.refreshRate;
					customFormatSettings.interlaced = pContext->encodeSettings.customFormatSettings.interlaced;
					customFormatSettings.aspectRatio = pContext->encodeSettings.customFormatSettings.aspectRatio;
					customFormatSettings.sampleAspectRatio.x = pContext->encodeSettings.customFormatSettings.sampleAspectRatio.x;
					customFormatSettings.sampleAspectRatio.y = pContext->encodeSettings.customFormatSettings.sampleAspectRatio.y;
					customFormatSettings.dropFrameAllowed = true;
					rc = NEXUS_Display_SetCustomFormatSettings(pContext->displayTranscode, NEXUS_VideoFormat_eCustom2, &customFormatSettings);
					assert(!rc);
				}
				/* resolution change typically could come with bit rate change; separate for now */
				break;
			case 2: /* bitrate change */ 
				printf("Bit rate change from %u to:   ", pContext->encodeSettings.encoderBitrate);
				printf("\n Bitrate (bps)	"); 
				scanf("%d", (int32_t*)&pContext->encodeSettings.encoderBitrate);
				NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
				videoEncoderConfig.bitrateMax = pContext->encodeSettings.encoderBitrate;
				NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
				break;
			case 3: /* frame rate change */
				printf("frame rate change:   ");
				printf("\n Frame rate\n"
					" (%d) 23.976\n"
					" (%d) 24\n"
					" (%d) 25\n"
					" (%d) 29.97Hz\n"
					" (%d) 30 Hz\n"
					" (%d) 50 Hz\n"
					" (%d) 59.94\n"
					" (%d) 60\n"
					" (%d) 14.985\n",
					NEXUS_VideoFrameRate_e23_976,
					NEXUS_VideoFrameRate_e24,
					NEXUS_VideoFrameRate_e25,
					NEXUS_VideoFrameRate_e29_97,
					NEXUS_VideoFrameRate_e30,
					NEXUS_VideoFrameRate_e50,
					NEXUS_VideoFrameRate_e59_94,
					NEXUS_VideoFrameRate_e60,
					NEXUS_VideoFrameRate_e14_985);
				scanf("%d", (int32_t*)&pContext->encodeSettings.encoderFrameRate);
				NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
				printf("Current encode variable frame rate mode: %d\n", videoEncoderConfig.variableFrameRate);
				printf("Change encode variable frame rate mode: [0=N/1=Y]   ");
				scanf("%d", &choice); videoEncoderConfig.variableFrameRate = choice;
				videoEncoderConfig.frameRate = pContext->encodeSettings.encoderFrameRate;
				NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
				break;
			case 4: /* MAD setting */
				/* window PQ custom setting */
				printf("MAD settings:\n");
				printf("Enable? (0) Disable; (1) Enable; ");
				scanf("%d", &choice); enableMad = choice;
				window_mad(pContext->windowTranscode, true, enableMad, false);
				break;
			case 5: /* DNR setting */
				/* window PQ custom setting */
				window_dnr(pContext->windowTranscode, true);
				break;
			case 7: /* xcode window demo mode */
				NEXUS_VideoWindow_GetSplitScreenSettings(pContext->windowTranscode, &splitSettings);
				printf("\n xcode window DNR demo mode: (%d)Disable (%d)Left (%d)Right	",
					NEXUS_VideoWindowSplitScreenMode_eDisable, 
					NEXUS_VideoWindowSplitScreenMode_eLeft,
					NEXUS_VideoWindowSplitScreenMode_eRight); 
				scanf("%d", (int32_t*)&splitSettings.dnr);
				NEXUS_VideoWindow_SetSplitScreenSettings(pContext->windowTranscode, &splitSettings);
				break;
			case 11: /* change 1080p24/30 source video decoder's scan mode progressive 3:2/2:2 ITFP test */
				printf("\n Force 1080p24/30 source video decoder progressive scan for 3:2/2:2 ITFP test : (0)No (1)Yes	"); 
				NEXUS_VideoDecoder_GetSettings(pContext->videoDecoder, &videoDecoderSettings);
				scanf("%d", (int *)&videoDecoderSettings.scanMode);
				NEXUS_VideoDecoder_SetSettings(pContext->videoDecoder, &videoDecoderSettings);
				break;
			case 12: /* change xcoder aspect ratio correction mode */
				printf("\n Please select transcoder aspect ratio correction mode:\n"
					" (%d) Zoom\n"
					" (%d) Box\n"
					" (%d) Pan and Scan\n"
					" (%d) Bypass aspect ratio correction\n"
					" (%d) PanScan without additional aspect ratio correction\n",
					(NEXUS_VideoWindowContentMode_eZoom),
					(NEXUS_VideoWindowContentMode_eBox),
					(NEXUS_VideoWindowContentMode_ePanScan),
					(NEXUS_VideoWindowContentMode_eFull),
					(NEXUS_VideoWindowContentMode_ePanScanWithoutCorrection));
				NEXUS_VideoWindow_GetSettings(pContext->windowTranscode, &windowSettings);
				scanf("%d", (int *)&windowSettings.contentMode);
				NEXUS_VideoWindow_SetSettings(pContext->windowTranscode, &windowSettings);
				break;
			case 13: /* change xcoder window scaler coeffs */
				NEXUS_VideoWindow_GetCoefficientIndexSettings(pContext->windowTranscode, &stCoeffIndex);
				printf("\nPlease set transcoder window scaler coefficients index:\n");
				printf("current scaler coeff index = %d;   new index [101~126] = ", stCoeffIndex.sclHorzLuma);
				scanf("%u", (uint32_t *)&stCoeffIndex.sclHorzLuma);
				NEXUS_VideoWindow_SetCoefficientIndexSettings(pContext->windowTranscode, &stCoeffIndex);
				break;
			case 14: /* get video encoder status */
				NEXUS_VideoEncoder_GetStatus(pContext->videoEncoder, &videoEncodeStatus);
				printf("\nVideo Encoder[%d] Status:\n", pContext->contextId);
				printf("----------------------\n");
				printf("data buffer base address     = %p\n", videoEncodeStatus.bufferBase);
				printf("metadata buffer base address = %p\n", videoEncodeStatus.metadataBufferBase);
				printf("error flags                  = 0x%x\n", videoEncodeStatus.errorFlags);
				printf("error count                  = %u\n", videoEncodeStatus.errorCount);
				printf("picture drops due to error   = %u\n", videoEncodeStatus.picturesDroppedErrors);
				printf("picture drops due to FRC     = %u\n", videoEncodeStatus.picturesDroppedFRC);
				printf("pictures Encoded             = %u\n", videoEncodeStatus.picturesEncoded);
				printf("pictures Received            = %u\n", videoEncodeStatus.picturesReceived);
				printf("picture Id Last Encoded      = 0x%x\n", videoEncodeStatus.pictureIdLastEncoded);
				printf("----------------------\n\n");
				break;
			case 15: /* select xcoder context */
				printf("Please select Xcoder context to configure [0 ~ %d]: ", NEXUS_NUM_VIDEO_ENCODERS-1);
				scanf("%d", &g_selectedXcodeContextId); 
				g_selectedXcodeContextId = (g_selectedXcodeContextId > NEXUS_NUM_VIDEO_ENCODERS-1) 
					? (NEXUS_NUM_VIDEO_ENCODERS-1):g_selectedXcodeContextId;
				B_Mutex_Unlock(pContext->mutexStarted);
				pContext = &xcodeContext[g_selectedXcodeContextId];
				B_Mutex_Lock(pContext->mutexStarted);
				break;
			case 20: /* start 2nd xcoder context */
				B_Mutex_Unlock(pContext->mutexStarted);
				pContext = &xcodeContext[g_selectedXcodeContextId];
				B_Mutex_Lock(pContext->mutexStarted);
				if(pContext->bStarted) {
					printf("context %d already started.\n", pContext->contextId);
					break;
				}
				printf("To start the xcoder%d...\n", pContext->contextId);
				if(!pContext->inputSettings.bConfig) config_xcoder_context(pContext);
				else {
					printf("Reconfigure the transcoder%d? [1=y/0=n] ", pContext->contextId);
					scanf("%d", &input); 
					if(input) config_xcoder_context(pContext);
				}	
				BDBG_ASSERT(start_transcode(pContext) == 0);
				break;
			case 21: /* stop 2nd xcoder context */
				B_Mutex_Unlock(pContext->mutexStarted);
				printf("To stop xcoder%d...\n", pContext->contextId);
				pContext = &xcodeContext[g_selectedXcodeContextId];
				B_Mutex_Lock(pContext->mutexStarted);
				stop_transcode(pContext);
				if(g_bAutoQuit) { 
					int i;
					for(i=0; i<NEXUS_NUM_VIDEO_ENCODERS; i++) {
						if(xcodeContext[i].bStarted) break;
					}
					/* when all contexts are stopped, quit the test */
					if(i == NEXUS_NUM_VIDEO_ENCODERS) {
						g_keyReturn = 'q';
						B_Mutex_Unlock(pContext->mutexStarted);
						goto Done;
					}
				}
				break;
			case 100: 
				printf("\n How many seconds to sleep?");
				scanf("%d", (int *)&ulSecToSleep);
				sleep(ulSecToSleep);
				break;
			case 99: /* debug module setting */
#if BDBG_DEBUG_BUILD
{
				int  iDbgLevel;
				char achName[256];
				printf("\nPlease enter the debug module name: ");
				scanf("%s", achName);
				printf("(%d)Trace (%d)Message (%d)Warning (%d)Error\n",
					BDBG_eTrace, BDBG_eMsg, BDBG_eWrn, BDBG_eErr);
				printf("Which debug level do you want to set it to? ");
				scanf("%d", &iDbgLevel);
				BDBG_SetModuleLevel(achName, (BDBG_Level)iDbgLevel); 
}
#endif				
				break;
			default:
				break;
		}
		B_Mutex_Unlock(pContext->mutexStarted);
	}
Done:
	if(g_eotEvent) {
		B_Event_Set(g_eotEvent);
	}
}

void print_usage(void) {
			printf("\ntranscode_asf usage:\n");
			printf("  Without options, it transcodes default stream file /data/videos/avatar_AVC_15M.ts into ASF file: /data/videos/stream.asf\n");
			printf("\nOptions:\n");
			printf("  -h        - to print the usage info\n");
			printf("  -cfg      - to set the test configuration\n");
			printf("  -NRTwrap  - to loop source file for long term Non Real Time transcode test\n");
			printf("  -autoquit - to quit the test automatically when all transcoders are stopped\n");
			printf("  -type {single|multi|multiNRT}\n");
			printf("     Note: 1) single type supports single encode, low delay mode and up to 1080p30 realtime encode\n");
			printf("           2) multiNRT type supports up to 1080p30 with dual NRT encodes\n");
			printf("           3) multi type is the default, supports up to 2 encodes with combined throughput up to 1080p30\n");
}

int main(int argc, char **argv)
{
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_FILE_MUX
    NEXUS_PlatformSettings platformSettings;
    int i = 0;
    int iteration = 1;

    TranscodeContext *pContext = xcodeContext;

	BSTD_UNUSED(argv);
	BKNI_Memset(pContext, 0, sizeof(xcodeContext));

	if (argc == 1){ /* default test config for auto test */
			print_usage();
			printf("\nYou're testing with the default configuration:\n");
			/* Input setting */
			pContext->inputSettings.resource = BTST_RESOURCE_FILE;
			BKNI_Snprintf(pContext->inputSettings.fname, 256, "/data/videos/avatar_AVC_15M.ts");
			pContext->inputSettings.eStreamType=NEXUS_TransportType_eTs;
			pContext->inputSettings.eVideoCodec=NEXUS_VideoCodec_eH264;
			pContext->inputSettings.eAudioCodec=NEXUS_AudioCodec_eAc3;
			pContext->inputSettings.bPcmAudio=false;
			pContext->inputSettings.iVideoPid=0x101;
			pContext->inputSettings.iAudioPid=0x104;
			pContext->inputSettings.iPcrPid=0x101;
			pContext->inputSettings.freq=0;
			pContext->inputSettings.bAudioInput=true;
			pContext->inputSettings.bConfig = false;

			printf("\n****************************************************************\n");
			printf("\n Input Parameters\n");
			printf("\n SourceType:   %d", pContext->inputSettings.resource);
			printf("\n filename:       %s", pContext->inputSettings.fname);

			printf("\n Streamtype:  %d \n encoderVideoCodec:  %d", pContext->inputSettings.eStreamType, pContext->inputSettings.eVideoCodec);
			printf("\n vPid:  %4d;  PcrPid:   %4d", pContext->inputSettings.iVideoPid, pContext->inputSettings.iPcrPid);

			printf("\n Enable Audio channel Pid: %d\n audio codec: %d           ", pContext->inputSettings.iAudioPid, pContext->inputSettings.eAudioCodec);
			printf("\n Config %1d", pContext->inputSettings.bConfig);
			printf("\n****************************************************************\n");
			

			/*Encode settings */
			BKNI_Snprintf(pContext->encodeSettings.fname, 256, "/data/videos/stream.asf");
			pContext->encodeSettings.displayFormat=NEXUS_VideoFormat_e480p;
			pContext->encodeSettings.encoderFrameRate=NEXUS_VideoFrameRate_e29_97;
			pContext->encodeSettings.encoderBitrate=2000000;
			pContext->encodeSettings.encoderGopStructureFramesP=29;
			pContext->encodeSettings.encoderGopStructureFramesB=0;
			pContext->encodeSettings.encoderVideoCodec=NEXUS_VideoCodec_eH264;
			pContext->encodeSettings.encoderProfile=NEXUS_VideoCodecProfile_eMain;
			pContext->encodeSettings.encoderLevel=NEXUS_VideoCodecLevel_e40;
			 
			pContext->encodeSettings.bCustom=false;
			pContext->encodeSettings.bAudioEncode=true;
			pContext->encodeSettings.encoderAudioCodec=NEXUS_AudioCodec_eWmaStd;

			printf("\n****************************************************************\n");
			printf("\n Encode Parameters\n");
			printf("\n filename:  %s", pContext->encodeSettings.fname);
			printf("\n display format:  %d", pContext->encodeSettings.displayFormat);
			printf("\n Frame Rate: %d\n Bit Rate: %d", pContext->encodeSettings.encoderFrameRate, pContext->encodeSettings.encoderBitrate);
			printf("\n P frames: %d;     B frames: %d", pContext->encodeSettings.encoderGopStructureFramesP, pContext->encodeSettings.encoderGopStructureFramesB);
			printf("\n Video Codec: %4d;  Profile: %4d;  Level: %4d", pContext->encodeSettings.encoderVideoCodec, pContext->encodeSettings.encoderProfile,pContext->encodeSettings.encoderLevel );
			printf("\n Audio Codec: %4d", pContext->encodeSettings.encoderAudioCodec);
			printf("\n****************************************************************\n");
	}
	if ( argc >= 2) {
		for(i=1; i<argc; i++) {
			if(!strcmp("-h",argv[i])) {
				print_usage();
				return 0;
			}
			if(!strcmp("-NRTwrap",argv[i])) {
				g_bNonRealTimeWrap = true;
				fprintf(stderr, "Enabled NRT wraparound...\n");
			}
			if(!strcmp("-autoquit",argv[i])) {
				g_bAutoQuit= true;
				fprintf(stderr, "Enabled auto quit...\n");
			}
			if(!strcmp("-type",argv[i])) {
				i++;
				if(!strcmp("single",argv[i])) {
					g_eVideoEncoderType = NEXUS_VideoEncoderType_eSingle;
				}
				else if(!strcmp("multiNRT",argv[i])) {
					g_eVideoEncoderType = NEXUS_VideoEncoderType_eMultiNonRealTime;
				}
				else {
					g_eVideoEncoderType = NEXUS_VideoEncoderType_eMulti;
				}
				fprintf(stderr, "Set video encoder type %d...\n", g_eVideoEncoderType);
			}
		}
		config_xcoder_context(pContext);/* transcoder 0 context user settings */
	}

	/* init platform */
	B_Os_Init();
	NEXUS_Platform_GetDefaultSettings(&platformSettings);
	/* enable frontend if the 1st xcoder needs it for now; not always enable to avoid slow frontend init for other cases;
	   TODO: init frontend in case 1st disable but 2nd enables frontend. */
#if BTST_SUPPORT_FRONTEND
	platformSettings.openFrontend = (pContext->inputSettings.resource == BTST_RESOURCE_QAM);
#endif
	/* audio PI supports 4 by default; we need two extra mixers for dual transcodes; */
	platformSettings.audioModuleSettings.maxAudioDspTasks = 6;
	NEXUS_Platform_Init(&platformSettings);
	NEXUS_Platform_GetConfiguration(&g_platformConfig);
	for(i=0; i< NEXUS_NUM_VIDEO_ENCODERS; i++) {
		xcodeContext[i].mutexStarted = B_Mutex_Create(NULL);
		xcodeContext[i].contextId = i;
	}

again:
	/* start the transcode test with context 0 */
	if(start_transcode(pContext) < 0)
		return -1;

	/****************************************************
	 *                       key handler                                                 *
	 *****************************************************/
	/* wait for 'q' to exit the test */
	if (pContext->inputSettings.bConfig)
	{
		B_ThreadSettings threadSettings;
		B_ThreadHandle keyHandle;

		g_eotEvent = B_Event_Create(NULL);
		B_Event_Reset(g_eotEvent);
		B_Thread_GetDefaultSettings(&threadSettings);
		threadSettings.terminate = true;
		keyHandle = B_Thread_Create("key handler", (B_ThreadFunc)keyHandler, xcodeContext, &threadSettings);
		while(B_Event_Wait(g_eotEvent, B_WAIT_FOREVER)!=B_ERROR_SUCCESS);
		B_Thread_Destroy(keyHandle);
		B_Event_Destroy(g_eotEvent);
	}
	else
	{
		NEXUS_VideoEncoderSettings videoEncoderConfig;
		NEXUS_DisplaySettings displaySettings;
		BDBG_WRN(("Auto test iteration: %d\n", iteration));
		BKNI_Sleep(30000);/* 30 seconds */
		/* To Do add BVN status and ViCE status check */
		/* Enable MADR*/
		window_mad(pContext->windowTranscode, false, true, false);
		/* change resolution */
		NEXUS_Display_GetSettings(pContext->displayTranscode, &displaySettings);
		displaySettings.format = NEXUS_VideoFormat_e480p;
		NEXUS_Display_SetSettings(pContext->displayTranscode, &displaySettings);
		BDBG_WRN(("format switch to 480p60"));
		NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
		videoEncoderConfig.bitrateMax = 2000000;
		NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
		BDBG_WRN(("bitrate switch to 2Mbps"));
		NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
		videoEncoderConfig.variableFrameRate = false;
		videoEncoderConfig.frameRate = NEXUS_VideoFrameRate_e30;
		videoEncoderConfig.streamStructure.framesP = 29;
		videoEncoderConfig.streamStructure.framesB = 2;
		NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
		BDBG_WRN(("frame rate switch to 30fps\n"));
		BKNI_Sleep(30000);/* 30 seconds */

		/* change resolution */
		NEXUS_Display_GetSettings(pContext->displayTranscode, &displaySettings);
		displaySettings.format = NEXUS_VideoFormat_e720p;
		NEXUS_Display_SetSettings(pContext->displayTranscode, &displaySettings);
		BDBG_WRN(("format switch to 720p60"));
		NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
		videoEncoderConfig.bitrateMax = 10000000; /* 10Mbps */
		NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
		BDBG_WRN(("bitrate switch to 10 Mbps"));
		NEXUS_VideoEncoder_GetSettings(pContext->videoEncoder, &videoEncoderConfig);
		videoEncoderConfig.variableFrameRate = false;
		videoEncoderConfig.frameRate = NEXUS_VideoFrameRate_e60;
		videoEncoderConfig.streamStructure.framesP = 29;
		videoEncoderConfig.streamStructure.framesB = 0;
		NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &videoEncoderConfig);
		BDBG_WRN(("frame rate switch to 60fps"));
		BKNI_Sleep(30000);
		g_keyReturn = iteration > TEST_ITERATIONS ? 'q' : 'c'; /* continue looping until quit */
	}

	for(i = 0; i < NEXUS_NUM_VIDEO_ENCODERS; i++) {
		B_Mutex_Lock(xcodeContext[i].mutexStarted);
		if(xcodeContext[i].bStarted) {
			stop_transcode(&xcodeContext[i]);
		}
		if(xcodeContext[i].nrtEofHandle) {
			B_Event_Set(xcodeContext[i].eofEvent);
			BKNI_Sleep(1);
			B_Thread_Destroy(xcodeContext[i].nrtEofHandle);
			xcodeContext[i].nrtEofHandle = NULL;
		}
		if(xcodeContext[i].eofEvent) {
			B_Event_Destroy(xcodeContext[i].eofEvent);
			xcodeContext[i].eofEvent = NULL;
		}
		B_Mutex_Unlock(xcodeContext[i].mutexStarted);
	}

	if(g_keyReturn != 'q')
	{
		iteration++;
		BDBG_WRN(("Start %d iteration.....", iteration));
		pContext = &xcodeContext[0];
		goto again;
	}

	for(i=1; i< NEXUS_NUM_VIDEO_ENCODERS; i++) {
		B_Mutex_Destroy(xcodeContext[i].mutexStarted);
	}
	/* uninit platform */
	NEXUS_Platform_Uninit();
	B_Os_Uninit();
#endif
    return 0;
}


