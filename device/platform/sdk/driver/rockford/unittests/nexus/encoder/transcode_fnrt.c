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
 * $brcm_Workfile: transcode_fnrt.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 10/3/12 4:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/encoder/transcode_fnrt.c $
 * 
 * Hydra_Software_Devel/8   10/3/12 4:51p randyjew
 * SW7425-3349: use NEXUS_MEMC0_MAIN_HEAP if NEXUS_MEMC0_DRIVER_HEAP not
 * defined.
 * 
 * Hydra_Software_Devel/7   9/20/12 11:20a hongtaoz
 * SW7425-3349: fixed typo;
 * 
 * Hydra_Software_Devel/6   9/20/12 11:12a hongtaoz
 * SW7425-3349: updated usage message for script mode;
 * 
 * Hydra_Software_Devel/5   9/20/12 10:41a hongtaoz
 * SW7425-3349: added non-interactive script test mode to redirect in test
 * script for automation;
 * 
 * Hydra_Software_Devel/4   9/6/12 10:00a hongtaoz
 * SW7425-3425, SW7425-3349: added FNRT TS mux test option;
 * 
 * Hydra_Software_Devel/3   8/31/12 2:54p hongtaoz
 * SW7425-3349: fixed compile warning;
 * 
 * Hydra_Software_Devel/2   8/31/12 2:21p hongtaoz
 * SW7425-3349: fixed non-debug build issue;
 * 
 * Hydra_Software_Devel/1   8/31/12 1:47p hongtaoz
 * SW7425-3425, SW7425-3349: merged FNRT to mainline; added FNRT unit
 * test;
 * 
 * Hydra_Software_Devel/FNRT_Demo/21   8/24/12 4:42p hongtaoz
 * SW7425-3349: added progress bars to simul display for demo;
 * 
 * Hydra_Software_Devel/FNRT_Demo/20   8/22/12 5:08p hongtaoz
 * SW7425-3349: added hdmi/component simul display from the 1st xcoder
 * source player;
 * 
 * Hydra_Software_Devel/FNRT_Demo/19   8/14/12 11:51a hongtaoz
 * SW7425-3349: added -audio and -bitrate options for autotest;
 * 
 * Hydra_Software_Devel/FNRT_Demo/18   8/13/12 3:53p hongtaoz
 * SW7425-3684: added mux duration status report and media probe; auto
 * finish audio and video threads only if both reach EOF;
 * 
 * Hydra_Software_Devel/FNRT_Demo/17   8/10/12 10:18p nilesh
 * SW7425-3349: Refactored video playpump thread chunking state machine
 * 
 * Hydra_Software_Devel/FNRT_Demo/16   8/9/12 1:06p hongtaoz
 * Sw7425-3349: use TPD-LAST-TPD BTP commands group for video transcoder
 * EOS;
 * 
 * Hydra_Software_Devel/FNRT_Demo/15   7/27/12 6:35p hongtaoz
 * SW7425-3424: move chunkDone callback to video decoder fnrtSettings;
 * 
 * Hydra_Software_Devel/FNRT_Demo/14   7/27/12 3:18p hongtaoz
 * SW7425-3349: increased video encoder buffer further for now to allow AV
 * fnrt to finish;
 * 
 * Hydra_Software_Devel/FNRT_Demo/13   7/27/12 2:41p hongtaoz
 * SW7425-3349: wait for both audio/video file feeding to complete;
 * 
 * Hydra_Software_Devel/FNRT_Demo/12   7/27/12 12:05a hongtaoz
 * SW7425-3349: fixed typo for audio program;
 * 
 * Hydra_Software_Devel/FNRT_Demo/11   7/26/12 7:35p hongtaoz
 * SW7425-3349: fixed typo; created audio mutex and default number of
 * audio to single thread;
 * 
 * Hydra_Software_Devel/FNRT_Demo/10   7/26/12 1:56p hongtaoz
 * SW7425-3349: audio threads count is separate from video;
 * 
 * Hydra_Software_Devel/FNRT_Demo/9   7/25/12 8:11p hongtaoz
 * SW7425-3349: removed FNRT enable setting; added audio thread;
 * 
 * Hydra_Software_Devel/FNRT_Demo/8   7/17/12 2:24p hongtaoz
 * SW7425-3349: increased encoder buffer allocation to work around fw
 * chunk end issue for now; disabled progressive download by default;
 * 
 * Hydra_Software_Devel/FNRT_Demo/7   7/16/12 4:37p hongtaoz
 * SW7425-3349: set default num of video xcoders to
 * NEXUS_NUM_VIDEO_ENCODERS; updated a few other encoder parameters;
 * 
 * Hydra_Software_Devel/FNRT_Demo/6   7/14/12 10:10p hongtaoz
 * SW7425-3349: exit chunk players at end of chunk and another player is
 * working on the last chunk; fixed segfault when polling status with
 * different transcoders;
 * 
 * Hydra_Software_Devel/FNRT_Demo/5   7/11/12 6:46p hongtaoz
 * SW7425-3349: set BTP command headend/tailend to 0/188 to avoid
 * immediate next I-frame being dropped;
 * 
 * Hydra_Software_Devel/FNRT_Demo/4   7/11/12 10:06a hongtaoz
 * SW7425-3349: use PICTURE_TAG BTP command instead of TIMING_MARKER for
 * chunk ID; align chunk file offset down to transport packet boundary
 * before inserting BTP packet; flush playpump and decoder at end of each
 * chunkDone;
 * 
 * Hydra_Software_Devel/FNRT_Demo/3   7/9/12 6:10p hongtaoz
 * SW7425-3349: increased video encoder CDB/ITB allocation for parallel
 * xcodes; changed chunkID BTP command payload to use dword[3] instead of
 * dword[9] to match TIMING_MARKER format; added option to set chunk
 * precharge count;
 * 
 * Hydra_Software_Devel/FNRT_Demo/2   7/6/12 5:45p hongtaoz
 * SW7425-3349: added FNRT configs;
 * 
 * Hydra_Software_Devel/FNRT_Demo/1   7/5/12 11:56a hongtaoz
 * SW7425-3349: initial version for single thread video only;
 * 
*
*****************************************************************************/
/* nexus unittest: transcode TS file source to mp4 file output in FNRT mode */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_parser_band.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_adj.h"
#include "nexus_video_input.h"
#include "nexus_core_utils.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_VIDEO_ENCODER && NEXUS_HAS_FILE_MUX
#include "nexus_playpump.h"
#include "nexus_file.h"
#include "nexus_video_encoder.h"
#include "nexus_audio_encoder.h"
#include "nexus_file_mux.h"
#include "nexus_stream_mux.h"
#include "nexus_recpump.h"
#include "nexus_record.h"
#include "b_os_lib.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_input.h"
#endif
#include "bcmplayer.h"
#include "bmedia_probe.h"
#include "bfile_stdio.h"
#include "progressbar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include "bstd.h"
#include "bkni.h"

/* num of audio threads */
#define BTST_NUM_AUDIO_THREADS       1

/* 2 audio xcoders per DSP core */
#define BTST_NUM_AUDIO_XCODE_PER_DSP 2

/* NRT a+v transcode STC av_window enable test */
#define BTST_ENABLE_NRT_STC_AV_WINDOW 1

/* TODO: set to 1 to disable the debug monitor for NRT mode source decode; set to 0 to enable it. */
#define BTST_DISABLE_NRT_DEBUG_DISPLAY 0

/* TODO: Set to 0 to enable auto stop of video using the LAST BTP */
#define BTST_DISABLE_AUTO_STOP 0

/***********************************
 *   Display assignments 
 */
#if NEXUS_NUM_DSP_VIDEO_ENCODERS
#ifndef NEXUS_ENCODER_DISPLAY_IDX
#define NEXUS_ENCODER_DISPLAY_IDX   1
#endif
/* source display locally as SD CVBS output by default */
#define BTST_SOURCE_DISPLAY_IDX     0
/* xcode loopback display as HD hdmi/component output by default */
#define BTST_LOOPBACK_DISPLAY_IDX   0
#else 
#ifndef NEXUS_ENCODER_DISPLAY_IDX
#define NEXUS_ENCODER_DISPLAY_IDX   3
#endif
/* source display locally as SD CVBS output by default */
#define BTST_SOURCE_DISPLAY_IDX     1
/* xcode loopback display as HD hdmi/component output by default */
#define BTST_LOOPBACK_DISPLAY_IDX   0
#endif


/***********************************
 *   STC assignments (2 per xcoder)
 */
#define BTST_SOURCE_STC_IDX                 0
#define BTST_ENCODER_STC_IDX                1
#define BTST_NRT_SOURCE_AUDIO_STC_IDX       BTST_ENCODER_STC_IDX

#define BTST_GET_STC_ID(baseId, xcodeId) (baseId + xcodeId * 2)

/***********************************
 *   Playpump assignments 
 */
#define BTST_SOURCE_PLAYPUMP_IDX            (0)

/***********************************
 *   Video Decoder assignments 
 */
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
#define BTST_XCODE_VID_DECODE_IDX 0
#else
#define BTST_XCODE_VID_DECODE_IDX       (NEXUS_NUM_VIDEO_DECODERS-1)
#endif

#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
#define BTST_INPUT_MAX_WIDTH 416
#define BTST_INPUT_MAX_HEIGHT 224
#endif

/***********************************
 *   Chunk parameters
 */
#define BTST_DEFAULT_CHUNK_PRECHARGE_COUNT     10 /* TODO: finalize the precharge count for MAD/ITFP cadence detection */
#define BTST_DEFAULT_CHUNK_DURATION            10000 /* 10 secs */
#define BTST_DEFAULT_CHUNK_ALLOC_MARGIN        (300) /* in percentage, alloc margin for CDB/ITB buffer to help performance */

BDBG_MODULE(transcode_fnrt);
BDBG_FILE_MODULE(chunk_fnrt);
BDBG_FILE_MODULE(btp_fnrt);
BDBG_FILE_MODULE(playpump_fnrt);
BDBG_FILE_MODULE(audio_playpump_fnrt);
BDBG_FILE_MODULE(chunk_fnrt_state);
BDBG_FILE_MODULE(fnrt_pbar);

#if NEXUS_HAS_VIDEO_ENCODER && NEXUS_HAS_STREAM_MUX
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
	NEXUS_VideoEncoderSettings videoEncoderConfig;
}  EncodeSettings;

typedef struct InputSettings{
	int                    resource;
	char                   fname[256];
	NEXUS_TransportType    eStreamType;
	NEXUS_VideoCodec       eVideoCodec;
	NEXUS_AudioCodec       eAudioCodec;
	NEXUS_AudioMultichannelFormat eMultiChanFmt;
	unsigned               iVideoPid;
	unsigned               iAudioPid;
	unsigned               iPcrPid;
	bool                   bAudioInput;
	bool                   bConfig;
} InputSettings;

typedef struct VideoChunk{
	long                       startRapIndex;
	long                       endRapIndex;
	BNAV_Player_Position       startPosition;
	BNAV_Player_Position       endPosition;
	uint32_t                   chunkId;
	off_t                      startOffset;
	off_t                      endOffset;
} VideoChunk;

typedef struct VideoTranscoder{
	NEXUS_VideoEncoderStartSettings vidEncoderStartSettings;
	NEXUS_VideoEncoderHandle   videoEncoder;
	NEXUS_DisplayHandle        displayTranscode;
	NEXUS_VideoWindowHandle    windowTranscode;
	NEXUS_StcChannelHandle     stcVideoChannel;
	NEXUS_PlaypumpHandle       playpump;
	BKNI_EventHandle           dataReadyEvent;
	BKNI_EventHandle           chunkDoneEvent;
	NEXUS_PidChannelHandle     videoPidChannel;
	NEXUS_VideoDecoderHandle   videoDecoder;
	NEXUS_VideoDecoderStartSettings vidProgram;
	int                        file;
	B_ThreadHandle             playpumpThread;
	B_MutexHandle              mutexStarted;
	bool                       bStarted;
	int                        xcodeId;
	VideoChunk                 chunk;
	VideoChunk                 prevChunk;
	off_t                      fileOffset;
} VideoTranscoder;

typedef struct AudioTranscoder{
	NEXUS_AudioMixerHandle     audioMixer;
	NEXUS_AudioEncoderHandle   audioEncoder;
	NEXUS_AudioMuxOutputHandle audioMuxOutput;
	NEXUS_AudioMuxOutputStartSettings audMuxStartSettings;
	NEXUS_StcChannelHandle     stcAudioChannel;
	int                        file;
	BKNI_EventHandle           dataReadyEvent;
	NEXUS_PlaypumpHandle       playpump;
	NEXUS_PidChannelHandle     audioPidChannel;
	NEXUS_AudioDecoderHandle   audioDecoder;
	NEXUS_AudioDecoderStartSettings audProgram;
	B_ThreadHandle             playpumpThread;
	B_MutexHandle              mutexStarted;
	bool                       bStarted;
	int                        xcodeId;
} AudioTranscoder;

typedef struct FnrtTranscodeContext {
	/* xcoder handles */
	VideoTranscoder            videoXcoder[NEXUS_NUM_VIDEO_ENCODERS];
	AudioTranscoder            audioXcoder[BTST_NUM_AUDIO_THREADS];

	/* mp4 file mux handles/settings */
	NEXUS_FileMuxStartSettings mp4MuxConfig;
	NEXUS_FileMuxHandle        fileMux;
	NEXUS_MuxFileHandle        mp4MuxOutput;
	BKNI_EventHandle           finishEvent;

	/* ts stream mux/record handles */
	NEXUS_PlaypumpHandle       playpumpTranscodeVideo;
	NEXUS_PidChannelHandle     pidChannelTranscodeVideo;
	NEXUS_PlaypumpHandle       playpumpTranscodeAudio;
	NEXUS_PidChannelHandle     pidChannelTranscodeAudio;
	NEXUS_StreamMuxHandle      streamMux;
	NEXUS_PlaypumpHandle       playpumpTranscodePcr;
	NEXUS_StcChannelHandle     stcChannelTranscode;
	NEXUS_FileRecordHandle     fileTranscode;
	NEXUS_RecpumpHandle        recpump;
	NEXUS_RecordHandle         record;
	NEXUS_PidChannelHandle     pidChannelTranscodePcr;
	NEXUS_StreamMuxStartSettings tsMuxConfig;

	/* source local decode/display handles */
	NEXUS_DisplayHandle        display;
	NEXUS_VideoWindowHandle    window;
	

	/* video encoder settings */
	EncodeSettings             encodeSettings;

	/* input settings */
	InputSettings              inputSettings;

	char                       indexfname[256];
	FILE                      *indexFile;
	BNAV_Player_Handle         bcmplayer;

	/* xcode system settings */

	int                       videoXcoderId;
	int                       audioXcoderId;

	int                       activeXcoders;
	B_MutexHandle             mutexActiveXcoders;

	VideoChunk                latestVideoChunk;
	B_MutexHandle             mutexChunk;

	unsigned                  duration;/* in ms */
	B_ThreadHandle            monitorThread;
	BKNI_EventHandle          monitorDone;
	bool                      bShutdown;

#if NEXUS_HAS_GRAPHICS2D /* for progress bar */
    NEXUS_SurfaceHandle       surface;
    NEXUS_Graphics2DHandle    gfx;
    BKNI_EventHandle          checkpointEvent, spaceAvailableEvent;
    ProgressBarHandle         pbar;/* mux progress bar */
    ProgressBarHandle         vidChunkPbar[NEXUS_NUM_VIDEO_ENCODERS];/* video chunk feeders progress bars */
#endif
} FnrtTranscodeContext;

/* global context 
 * function level context in this */
static FnrtTranscodeContext xcodeContext;

static NEXUS_PlatformConfiguration g_platformConfig;
static char g_keyReturn ='\0';
static bool g_bNoDspMixer = false;
static bool g_bMultiChanAudio = false;
static unsigned g_chunkPrechargeCount = BTST_DEFAULT_CHUNK_PRECHARGE_COUNT;
static unsigned g_vidCnt = NEXUS_NUM_VIDEO_ENCODERS;
static unsigned g_audCnt = 1/*NEXUS_NUM_AUDIO_DSP*/;
static unsigned g_chunkDuration = BTST_DEFAULT_CHUNK_DURATION; /* ms */
static unsigned g_relocBufSize = 1024*1024;/* the larger, the faster to finish mp4 */
static bool g_bProgressiveDownload = false;
static unsigned g_bitrate = 3000000; /* default video bitrate */
static B_EventHandle g_eotEvent = NULL;/* end of test event */
static off_t g_fileSize = 0;
static bool g_bTsMux = false;
static bool g_bScriptMode = false; /* script mode (mutual exclusive with interactive mode) */

#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
static int g_encodeDisplayId[NEXUS_NUM_VIDEO_ENCODERS] = {
    NEXUS_ENCODER_DISPLAY_IDX
};
#else
static int g_encodeDisplayId[NEXUS_NUM_VIDEO_ENCODERS] = {
	NEXUS_ENCODER0_DISPLAY_IDX,
	NEXUS_ENCODER1_DISPLAY_IDX,
#ifdef NEXUS_ENCODER2_DISPLAY_IDX
	NEXUS_ENCODER2_DISPLAY_IDX,
#endif
#ifdef NEXUS_ENCODER3_DISPLAY_IDX
	NEXUS_ENCODER3_DISPLAY_IDX,
#endif
};
#endif

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
			scanf("%d", &choice);windowMadSettings.enable22Pulldown = choice;
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


static void
transcoderFinishCallback(void *context, int param)
{
	BKNI_EventHandle finishEvent = (BKNI_EventHandle)context;

	BSTD_UNUSED(param);
	BDBG_MSG(("Finish callback invoked, now stop the file mux."));
	BKNI_SetEvent(finishEvent);
}

static void keyHandler( FnrtTranscodeContext  *pContext )
{
	char key[256];
	int choice;
	NEXUS_VideoEncoderStatus videoEncodeStatus;
	NEXUS_VideoDecoderStatus videoDecodeStatus;
	int rcInput;
	uint32_t ulSecToSleep;

	for(;;)
	{
		printf("Menu:\n");
		printf("14) Get video xcoder status\n");
		printf("15) Select xcoder context\n");
		printf("99) change DEBUG module setting\n");
		printf("100) sleep\n");
		printf("\nEnter 'q' to quit\n\n");
		rcInput = scanf("%s", key);
		if(!strcmp(key, "q")) 
		{
			g_keyReturn = 'q';
			break;
		}
		choice = strtoul(key, NULL, 0);
		B_Mutex_Lock(pContext->videoXcoder[pContext->videoXcoderId].mutexStarted);
		if(!pContext->videoXcoder[pContext->videoXcoderId].bStarted&& (choice!=99)) {
			printf("context %d not started.\n", pContext->videoXcoderId);
			B_Mutex_Unlock(pContext->videoXcoder[pContext->videoXcoderId].mutexStarted);
			continue;
		}
		switch(choice)
		{
			case 14: /* get video encoder status */
				{ NEXUS_VideoEncoderClearStatus clearStatus;
				NEXUS_VideoEncoder_GetStatus(pContext->videoXcoder[pContext->videoXcoderId].videoEncoder, &videoEncodeStatus);
				printf("\nVideo Encoder[%d] Status:\n", pContext->videoXcoderId);
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
				NEXUS_VideoEncoder_GetDefaultClearStatus(&clearStatus);
				NEXUS_VideoEncoder_ClearStatus(pContext->videoXcoder[pContext->videoXcoderId].videoEncoder, &clearStatus);
				}
				NEXUS_VideoDecoder_GetStatus(pContext->videoXcoder[pContext->videoXcoderId].videoDecoder, &videoDecodeStatus);
				printf("\nVideo Decoder[%d] Status:\n", pContext->videoXcoderId);
				printf("----------------------\n");
				printf("muted                = %d\n", videoDecodeStatus.muted);
				printf("pictureTag           = %lu\n", videoDecodeStatus.pictureTag);
				printf("current PTS          = 0x%x\n", videoDecodeStatus.pts);
				printf("fifo depth           = %u\n", videoDecodeStatus.fifoDepth);
				printf("fifo size            = %u\n", videoDecodeStatus.fifoSize);
				printf("queueDepth           = %u\n", videoDecodeStatus.queueDepth);
				printf("numDecoded           = %u\n", videoDecodeStatus.numDecoded);
				printf("numDisplayed         = %u\n", videoDecodeStatus.numDisplayed);
				printf("numDecodeErrors      = 0x%x\n", videoDecodeStatus.numDecodeErrors);
				printf("numDecodeOverflows   = 0x%x\n", videoDecodeStatus.numDecodeOverflows);
				printf("numDisplayErrors     = 0x%x\n", videoDecodeStatus.numDisplayErrors);
				printf("numDecodeDrops       = 0x%x\n", videoDecodeStatus.numDecodeDrops);
				printf("numPicturesReceived  = 0x%x\n", videoDecodeStatus.numPicturesReceived);
				printf("numDisplayDrops      = 0x%x\n", videoDecodeStatus.numDisplayDrops);
				printf("numDisplayUnderflows = 0x%x\n", videoDecodeStatus.numDisplayUnderflows);
				printf("PtsErrorCount        = 0x%x\n", videoDecodeStatus.ptsErrorCount);
				printf("----------------------\n\n");
				break;
			case 15: /* select xcoder context */
				B_Mutex_Unlock(pContext->videoXcoder[pContext->videoXcoderId].mutexStarted);
				printf("Please select Xcoder context to configure [0 ~ %d]: ", NEXUS_NUM_VIDEO_ENCODERS-1);
				scanf("%d", &pContext->videoXcoderId); 
				pContext->videoXcoderId = (pContext->videoXcoderId > NEXUS_NUM_VIDEO_ENCODERS-1) 
					? (NEXUS_NUM_VIDEO_ENCODERS-1):pContext->videoXcoderId;
				B_Mutex_Lock(pContext->videoXcoder[pContext->videoXcoderId].mutexStarted);
				break;
			case 100: 
				printf("\n How many seconds to sleep?");
				scanf("%u", &ulSecToSleep);
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
		B_Mutex_Unlock(pContext->videoXcoder[pContext->videoXcoderId].mutexStarted);
	}
	if(g_eotEvent) {
		B_Event_Set(g_eotEvent);
	}
}

static void xcode_index_filename(char *indexFile, const char *mediaFile )
{
	char *tmpPtr;
	strncpy(indexFile, mediaFile, strlen(mediaFile));
	tmpPtr = strstr(indexFile, ".");
	if (tmpPtr) {
	    strncpy(tmpPtr+1, "nav", 3);
	}
	else {
	    strncat(indexFile, ".nav", sizeof(indexFile)-1);
	}
	BDBG_MSG(("Media file name: %s, index file name %s", mediaFile, indexFile));
}

/* file i/o page size */
#define BTST_RECORD_PAGE_SIZE        (4096)

/*******************************
 * Set up stream_mux and record
 */
static void open_mp4mux( FnrtTranscodeContext  *pContext )
{
    NEXUS_FileMuxCreateSettings muxCreateSettings;

    BKNI_CreateEvent(&pContext->finishEvent);
    NEXUS_FileMux_GetDefaultCreateSettings(&muxCreateSettings);
    muxCreateSettings.finished.callback = transcoderFinishCallback;
    muxCreateSettings.finished.context = pContext->finishEvent;
    if(g_relocBufSize) muxCreateSettings.mp4.relocationBuffer = g_relocBufSize;

    pContext->fileMux = NEXUS_FileMux_Create(&muxCreateSettings);

    NEXUS_FileMux_GetDefaultStartSettings(&pContext->mp4MuxConfig, NEXUS_TransportType_eMp4);

    /* disable progressive download to speed up mp4 mux finish time */
    pContext->mp4MuxConfig.streamSettings.mp4.progressiveDownloadCompatible = g_bProgressiveDownload;

    pContext->mp4MuxConfig.video[0].track = 1;
    pContext->mp4MuxConfig.video[0].codec = pContext->videoXcoder[0].vidEncoderStartSettings.codec;
    pContext->mp4MuxConfig.video[0].encoder = pContext->videoXcoder[0].videoEncoder;

    snprintf(pContext->mp4MuxConfig.tempDir, sizeof(pContext->mp4MuxConfig.tempDir), "/data/videos");
    pContext->mp4MuxOutput = NEXUS_MuxFile_OpenPosix(pContext->encodeSettings.fname);
    if (!pContext->mp4MuxOutput) {
        fprintf(stderr, "can't open file:%s\n", pContext->encodeSettings.fname);
        BDBG_ASSERT(0);
    }
}

#define BTST_MUX_PCR_PID     0x11
#define BTST_MUX_VIDEO_PID   0x12
#define BTST_MUX_AUDIO_PID   0x13

static void open_tsmux( FnrtTranscodeContext  *pContext )
{
	NEXUS_StreamMuxCreateSettings muxCreateSettings;
	NEXUS_StreamMuxStartSettings muxConfig;
	NEXUS_PlaypumpOpenSettings playpumpConfig;
	NEXUS_RecpumpOpenSettings recpumpOpenSettings;
	NEXUS_RecordSettings recordSettings;

	NEXUS_Playpump_GetDefaultOpenSettings(&playpumpConfig);
	playpumpConfig.fifoSize = 16384; /* reduce FIFO size allocated for playpump */
	playpumpConfig.numDescriptors = 64; /* set number of descriptors */
	playpumpConfig.streamMuxCompatible = true;
	pContext->playpumpTranscodeVideo = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpConfig);
	assert(pContext->playpumpTranscodeVideo);
#if BTST_DISABLE_VIDEO_ENCODER_PES_PACING
	NEXUS_Playpump_SuspendPacing(pContext->playpumpTranscodeVideo, true);
#endif

	pContext->playpumpTranscodePcr = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpConfig);
	assert(pContext->playpumpTranscodePcr);

	BKNI_CreateEvent(&pContext->finishEvent);
	NEXUS_StreamMux_GetDefaultCreateSettings(&muxCreateSettings);
	muxCreateSettings.finished.callback = transcoderFinishCallback;
	muxCreateSettings.finished.context = pContext->finishEvent;
	pContext->streamMux = NEXUS_StreamMux_Create(&muxCreateSettings);
	NEXUS_StreamMux_GetDefaultStartSettings(&muxConfig);
	muxConfig.transportType = NEXUS_TransportType_eTs;
	muxConfig.stcChannel = pContext->videoXcoder[0].stcVideoChannel;
	muxConfig.nonRealTime = true;
	muxConfig.nonRealTimeRate = 32 * NEXUS_NORMAL_PLAY_SPEED; /* AFAP */
	muxConfig.servicePeriod = 50; /* MSP */

	muxConfig.video[0].pid = BTST_MUX_VIDEO_PID;
	muxConfig.video[0].encoder = pContext->videoXcoder[0].videoEncoder;
	muxConfig.video[0].playpump = pContext->playpumpTranscodeVideo;

	muxConfig.pcr.pid = BTST_MUX_PCR_PID;
	muxConfig.pcr.playpump = pContext->playpumpTranscodePcr;
	muxConfig.pcr.interval = 50;

	/******************************************
	 * Set up xcoder record output
	 */
	NEXUS_Recpump_GetDefaultOpenSettings(&recpumpOpenSettings);
	/******************************************
	 *  NRT workaround for the XPT band hold and data ready events sharing the same threshold: avoid band hold from occuring,
	 * otherwise, video stutter would happen!
	 * 1) The nexus_record timer fires at 250 ms interval to service record data as bottom line;
	 * 2) whenever nexus record timer fires, it'll consume up to Nx(3x(47x4096)) cdb data;
	 * 3) so if file i/o can keep up, band hold threshold = 2x(3x(47x4096)) = 1.1MB can sustain record bit rate up to
	 *       2 * 3 * 188K * 8 / 250ms = 36 Mbps without reaching band hold;
	 * 4) larger band hold threshold can sustain higher record bit rate throughput.
	 * NOTE: after SW7425-1663, recpump data ready threshold is decoupled with RAVE upper (band-hold) threshold, so
	 * we do not need to mess with data ready threshold any more!
	 */
	BDBG_MSG(("To open recpump with dataReadyThreshold = %d indexReadyThreshold=%d", 
		recpumpOpenSettings.data.dataReadyThreshold, recpumpOpenSettings.index.dataReadyThreshold));
	BDBG_MSG(("        recpump with data fifo size     = %d index fifo size    =%d", 
		recpumpOpenSettings.data.bufferSize, recpumpOpenSettings.index.bufferSize));
	pContext->recpump = NEXUS_Recpump_Open(NEXUS_ANY_ID, &recpumpOpenSettings);
	assert(pContext->recpump);

	pContext->record = NEXUS_Record_Create();
	assert(pContext->record);

	NEXUS_Record_GetSettings(pContext->record, &recordSettings);
	recordSettings.recpump = pContext->recpump;
	/* NOTE: enable band hold to allow recpump to stall TS mux to avoid ave data corruption in case file i/o is slow
	 * The encoders should make sure no output CDB/ITB buffer corruption (instead do frame drop) when output overflow! */
	recordSettings.recpumpSettings.bandHold = NEXUS_RecpumpFlowControl_eEnable;
	recordSettings.pollingTimer = 50;
	recordSettings.writeAllTimeout = 100;
	NEXUS_Record_SetSettings(pContext->record, &recordSettings);

	/* store the mux config */
	pContext->tsMuxConfig = muxConfig;

	/* open PidChannels */
	pContext->pidChannelTranscodePcr = NEXUS_Playpump_OpenPidChannel(pContext->playpumpTranscodePcr, muxConfig.pcr.pid, NULL);
	assert(pContext->pidChannelTranscodePcr);

	/* set record index file name and open the record file handle */
	if((pContext->encodeSettings.encoderVideoCodec==NEXUS_VideoCodec_eMpeg2) ||
	   (pContext->encodeSettings.encoderVideoCodec==NEXUS_VideoCodec_eH264))
	{
		xcode_index_filename(pContext->indexfname, pContext->encodeSettings.fname);
	}
	else BDBG_WRN(("no index record"));

}

static void xcode_audio_tsmux_record( FnrtTranscodeContext  *pContext )
{
	NEXUS_PlaypumpOpenSettings playpumpConfig;

	/* audio playpump here is for ts muxer */
	NEXUS_Playpump_GetDefaultOpenSettings(&playpumpConfig);
	playpumpConfig.fifoSize = 16384; /* reduce FIFO size allocated for playpump */
	playpumpConfig.numDescriptors = 64; /* set number of descriptors */
	playpumpConfig.streamMuxCompatible = true;
	pContext->playpumpTranscodeAudio = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpConfig);
	assert(pContext->playpumpTranscodeAudio);

	pContext->tsMuxConfig.audio[0].pid = BTST_MUX_AUDIO_PID;
	pContext->tsMuxConfig.audio[0].muxOutput = pContext->audioXcoder[0].audioMuxOutput;
	pContext->tsMuxConfig.audio[0].playpump = pContext->playpumpTranscodeAudio;
}

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


static void
vidSrcStreamChangedCallback(void *context, int param)
{
	NEXUS_VideoDecoderStreamInformation streamInfo;
	NEXUS_VideoDecoderHandle decoder = (NEXUS_VideoDecoderHandle)context;

	BSTD_UNUSED(param);
	NEXUS_VideoDecoder_GetStreamInformation(decoder, &streamInfo);
	BDBG_MSG(("Video Source Stream Change callback: %ux%u@%.3f%c", 
		streamInfo.sourceHorizontalSize,
		streamInfo.sourceVerticalSize,
		convert_RefreshRate_FromFrameRate(streamInfo.frameRate),
		streamInfo.streamProgressive? 'p' : 'i'));
}

static void
vidChunkDoneCallback(void *context, int param)
{
	BKNI_EventHandle event = (BKNI_EventHandle)context;

	BDBG_MSG(("Video[%d] chunk done callback...", param));
	BKNI_SetEvent(event);
}

static void seekNextChunk(VideoChunk *nextChunk /* output */)
{
    BNAV_Player_Position *startPosition = &xcodeContext.latestVideoChunk.startPosition;
    BNAV_Player_Position *endPosition   = &xcodeContext.latestVideoChunk.endPosition;
    unsigned long dpts = 0;
    long index;

    BDBG_ASSERT(nextChunk);

    /* the latest chunk is global to be protected by mutex */
    B_Mutex_Lock(xcodeContext.mutexChunk);
    index = BNAV_Player_FindIFrameFromIndex(xcodeContext.bcmplayer, xcodeContext.latestVideoChunk.startRapIndex, 1);
    BNAV_Player_GetPositionInformation(xcodeContext.bcmplayer, index, startPosition);
    /* find the end of chunk index */
    do {
        /* kludge: in case this function returns index backwards by 1, search 2 ahead */
        index = BNAV_Player_FindIFrameFromIndex(xcodeContext.bcmplayer, index+2, 1);
        if(index == -1) /* end of file */ {
            endPosition->offsetLo = (unsigned long) (-1);
            endPosition->offsetHi = (unsigned long) (-1);
            endPosition->index    = -1;
            break;
        }
        BNAV_Player_GetPositionInformation(xcodeContext.bcmplayer, index, endPosition);
        /* discontinuity may result in a short chunk; but it should be ok */
        dpts = (endPosition->pts >= startPosition->pts)? (endPosition->pts - startPosition->pts)
                : ((unsigned long)(-1) - startPosition->pts + 1 + endPosition->pts);
        BDBG_MODULE_MSG(chunk_fnrt, ("dpts[%lu], start[%ld]@pts=%08x, end[%ld] pts=%08x", dpts, startPosition->index, startPosition->pts, index, endPosition->pts));
    } while(dpts/45 < g_chunkDuration);

    /* round down offset to 188-byte TP aligned */
    startPosition->offsetLo = startPosition->offsetLo / 188 * 188;
    if(endPosition->index != -1) {/* end of file no need to round down */
        endPosition->offsetLo   = endPosition->offsetLo / 188 * 188;
    }
    BDBG_MSG(("Chunk start at index %ld, end at index %ld with duration %u ms\n",
        xcodeContext.latestVideoChunk.startRapIndex, index, dpts/45));
    xcodeContext.latestVideoChunk.endRapIndex = index;

    xcodeContext.latestVideoChunk.startOffset = xcodeContext.latestVideoChunk.startPosition.offsetLo + ((uint64_t)xcodeContext.latestVideoChunk.startPosition.offsetHi<<32);
    xcodeContext.latestVideoChunk.endOffset   = xcodeContext.latestVideoChunk.endPosition.offsetLo + ((uint64_t)xcodeContext.latestVideoChunk.endPosition.offsetHi<<32);

    /* copy the chunk info */
    *nextChunk = xcodeContext.latestVideoChunk;

    /* bump up chunk id to prepare the next chunk */
    xcodeContext.latestVideoChunk.chunkId++;
    xcodeContext.latestVideoChunk.startRapIndex = xcodeContext.latestVideoChunk.endRapIndex;
    B_Mutex_Unlock(xcodeContext.mutexChunk);
}

typedef enum BTPCommand
{
   BTPCommand_eChunkId = 0x0D, /* reuse PICTURE_TAG */
   BTPCommand_eLast = 0x82, /* LAST */
   BTPCommand_eEOS = 0x0A /* protocol agnostic EOS or so-called INLINE_FLUSH or TPD */
} BTPCommand;

static void insertBTP(void *buf, uint32_t chunkId, unsigned pid, BTPCommand command)
{
    int i;
    static uint8_t btp[188] = {
            0x47, 0x1f, 0xff, 0x20,
            0xb7, 0x02, 0x2d, 0x00,
            'B',  'R',  'C',  'M',  /* signature */
            0x00, 0x00, 0x00, 0x0d, /* CHUNK_ID command (reuse PICTURE_TAG) */
            0x00, 0x00, 0x00, 0x00, /* dwod 1 */
            0x00, 0x00, 0x00, 0x00, /* dwod 2 */
            0x00, 0x00, 0x00, 0x00, /* dwod 3 */
            0x00, 0x00, 0x00, 0x00, /* dwod 4 */
            0x00, 0x00, 0x00, 0x00, /* dwod 5 */
            0x00, 0x00, 0x00, 0x00, /* dwod 6 */
            0x00, 0x00, 0x00, 0x00, /* dwod 7 */
            0x00, 0x00, 0x00, 0xbc, /* dwod 8 =188 to avoid RAVE dropping following packets */
            0x00, 0x00, 0x00, 0x00, /* dwod 9: chunkID in big endian */
            /* rest of BTP = 0x00 */
        };

    /* mutex to protect atomic static array from multi-thread */
    B_Mutex_Lock(xcodeContext.mutexChunk);
    btp[1] = (pid >> 8) & 0x1f;
    btp[2] = pid & 0xff;  /* PID */

    btp[12 + 3] = command;

    switch(command)
    {
       case BTPCommand_eChunkId:
       case BTPCommand_eLast:
       case BTPCommand_eEOS:
        /* big endian dword[9] for CHUNK_ID BTP command's chunkID payload */
        btp[12+36] = (unsigned char) ((chunkId & 0xff000000) >> 24);
        btp[12+36+1] = (unsigned char) ((chunkId & 0x00ff0000) >> 16);
        btp[12+36+2] = (unsigned char) ((chunkId & 0x0000ff00) >> 8);
        btp[12+36+3] = (unsigned char) (chunkId & 0x000000ff);
        break;
    }
    BKNI_Memcpy(buf,(void *)btp,188);
    BDBG_MODULE_MSG(btp_fnrt, ("BTP:"));
    for(i=0; i<52; i+=4) {
        BDBG_MODULE_MSG(btp_fnrt, ("%02x %02x %02x %02x", btp[i], btp[i+1], btp[i+2], btp[i+3]));
    }
    B_Mutex_Unlock(xcodeContext.mutexChunk);
}

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

/* video chunk xcoder playpump feeder */
static void videoPlaypump_thread(
    VideoTranscoder  *pContext )
{
    typedef enum BTST_P_ChunkState {
        BTST_P_ChunkState_eGetNextChunk=0,
        BTST_P_ChunkState_eStart,
        BTST_P_ChunkState_eFeed,
        BTST_P_ChunkState_eToEnd,
        BTST_P_ChunkState_eWaitForDone,
        BTST_P_ChunkState_eEOF,
        BTST_P_ChunkState_eDone
    } BTST_P_ChunkState;

    /* while loop until EOF or quit */
    {
        BTST_P_ChunkState chunkState = BTST_P_ChunkState_eGetNextChunk;
        long startRapIndex;

        BDBG_MODULE_MSG(chunk_fnrt_state, ("[%u] START --> eGetNextChunk", pContext->xcodeId));

        /* 2) while loop for chunk feed */
        while ( (chunkState != BTST_P_ChunkState_eDone) && (g_keyReturn!= 'q') ) {
            void *buffer;
            size_t buffer_size;
            int n, playSize=0;
            NEXUS_Error rc;

            if (NEXUS_Playpump_GetBuffer(pContext->playpump, &buffer, &buffer_size)) {
               break;
            }
            if (buffer_size < ((chunkState==BTST_P_ChunkState_eEOF)? 3*188 : 188)) {
                BKNI_WaitForEvent(pContext->dataReadyEvent, 10000);
                continue;
            }

            /* The first call to get_buffer will return the entire playback buffer.
               If we use it, we're going to have to wait until the descriptor is complete,
               and then we're going to underflow. So use a max size. */
#define MAX_READ (188*1024)
            if (buffer_size > MAX_READ)
                buffer_size = MAX_READ;

            switch(chunkState) {
               case BTST_P_ChunkState_eGetNextChunk:
                  /* if reached end of index file by another thread, stop this thread now */
                  B_Mutex_Lock(xcodeContext.mutexChunk);
                  startRapIndex = xcodeContext.latestVideoChunk.startRapIndex;
                  B_Mutex_Unlock(xcodeContext.mutexChunk);
                  if(startRapIndex == -1)
                  {
                     chunkState = BTST_P_ChunkState_eEOF;
                     BDBG_MODULE_MSG(chunk_fnrt_state, ("[%u] eGetNextChunk --> eEOF", pContext->xcodeId));
                  }
                  else
                  {
                     /* 1) Seek the next chunk to start with; */
                     seekNextChunk(&pContext->chunk);
                     pContext->fileOffset = pContext->chunk.startOffset;
                     BDBG_MSG(("Xcoder[%d] to start chunk[%d] from %llu to %llu", pContext->xcodeId, pContext->chunk.chunkId, pContext->chunk.startOffset, pContext->chunk.endOffset));
                     lseek(pContext->file, pContext->chunk.startOffset, SEEK_SET);
                     chunkState = BTST_P_ChunkState_eStart;
                     BDBG_MODULE_MSG(chunk_fnrt_state, ("[%u] eGetNextChunk --> eStart", pContext->xcodeId));

                     /* flush playpump, rave and decoder to restart for next chunk */
                     NEXUS_VideoDecoder_Stop(pContext->videoDecoder);/* this flush RAVE and AVD buffers */
                     NEXUS_VideoDecoder_Start(pContext->videoDecoder, &pContext->vidProgram);
                  }
                  continue;
            /* insert start of chunk BTP */
            case BTST_P_ChunkState_eStart:
                insertBTP(buffer, pContext->chunk.chunkId, xcodeContext.inputSettings.iVideoPid, BTPCommand_eChunkId);
                buffer = (uint8_t*)buffer + 188; buffer_size -= 188; playSize = 188;
                chunkState = BTST_P_ChunkState_eFeed;
                BDBG_MODULE_MSG(chunk_fnrt_state, ("[%u] eStart --> eFeed", pContext->xcodeId));
                break;
            /* feed until end of chunk */
            case BTST_P_ChunkState_eFeed:
                if ((buffer_size + pContext->fileOffset > pContext->chunk.endOffset) && (pContext->chunk.endOffset != -1)) {
                    buffer_size = pContext->chunk.endOffset - pContext->fileOffset;
                    chunkState = BTST_P_ChunkState_eToEnd;
                    BDBG_MODULE_MSG(chunk_fnrt_state, ("[%u] eStart --> eFeed", pContext->xcodeId));
                }
                break;
            /* insert end of chunk BTP */
            case BTST_P_ChunkState_eToEnd:/* end with current chunkID+1 */
                insertBTP(buffer, pContext->chunk.chunkId + 1, xcodeContext.inputSettings.iVideoPid, BTPCommand_eChunkId);
                buffer = (uint8_t*)buffer + 188; buffer_size -= 188; playSize = 188;
                chunkState = BTST_P_ChunkState_eWaitForDone;
                BDBG_MODULE_MSG(chunk_fnrt_state, ("[%u] eToEnd --> eWaitForDone", pContext->xcodeId));
                break;
            /* continue to feed until ChunkDone callback if don't know exactly when to stop current chunk */
            case BTST_P_ChunkState_eWaitForDone:
                /* test chunkDone; if not, continue chunk feed, otherwise flush and start next chunk. */
                if(BKNI_WaitForEvent(pContext->chunkDoneEvent,1) == BERR_SUCCESS) {
                    BDBG_MSG(("Video xcoder[%d] chunk DONE!", pContext->xcodeId));
                    NEXUS_Playpump_Flush(pContext->playpump);
                    chunkState = BTST_P_ChunkState_eGetNextChunk;
                    BDBG_MODULE_MSG(chunk_fnrt_state, ("[%u] eWaitForDone --> eGetNextChunk", pContext->xcodeId));
                    continue;
                }
                break;
            case BTST_P_ChunkState_eEOF:
               insertBTP(buffer, 0, xcodeContext.inputSettings.iVideoPid, BTPCommand_eEOS);
               buffer = (uint8_t*)buffer + 188; buffer_size -= 188; playSize += 188;
               insertBTP(buffer, 0, xcodeContext.inputSettings.iVideoPid, BTPCommand_eLast);
               buffer = (uint8_t*)buffer + 188; buffer_size -= 188; playSize += 188;
               insertBTP(buffer, 0, xcodeContext.inputSettings.iVideoPid, BTPCommand_eEOS);
               buffer = (uint8_t*)buffer + 188; buffer_size -= 188; playSize += 188;
               rc = NEXUS_Playpump_WriteComplete(pContext->playpump, 0, playSize);
               chunkState = BTST_P_ChunkState_eDone;
               BDBG_MODULE_MSG(chunk_fnrt_state, ("[%u] eEOF --> eDone", pContext->xcodeId));
               continue;

            case BTST_P_ChunkState_eDone:
               continue;
            default:/* eDone */
                break;
            }

            n = read(pContext->file, buffer, buffer_size);
            if (n < 0) BDBG_ASSERT(0);
            playSize += n;
            pContext->fileOffset += n;/* update file offset */
            if (n == 0 && buffer_size) {
               chunkState = BTST_P_ChunkState_eEOF;
               BDBG_MODULE_MSG(chunk_fnrt_state, ("[%u] eEOF", pContext->xcodeId));
               BDBG_MSG(("Video[%u] to end playfile...", pContext->xcodeId));
            }
            rc = NEXUS_Playpump_WriteComplete(pContext->playpump, 0, playSize);
            BDBG_ASSERT(!rc);
            BDBG_MODULE_MSG(playpump_fnrt, ("V[%d] played %d bytes\n", pContext->xcodeId, n));
        }
    }

    /* decrement count */
    B_Mutex_Lock(xcodeContext.mutexActiveXcoders);
    xcodeContext.activeXcoders--;
    BDBG_MSG(("Video playpump[%d] is stopped. Active video pumps count = %d", pContext->xcodeId, xcodeContext.activeXcoders));
    if(g_eotEvent && xcodeContext.activeXcoders==0) {
        g_keyReturn = 'q';
        B_Event_Set(g_eotEvent);
    }
    B_Mutex_Unlock(xcodeContext.mutexActiveXcoders);
}

/* audio xcoder playpump feeder */
static void audioPlaypump_thread(
    AudioTranscoder  *pContext )
{
    bool endOfFile = false;

    /* while loop until EOF or quit */
    while (g_keyReturn!= 'q') {
        void *buffer;
        size_t buffer_size;
        int n;
        NEXUS_Error rc;

        if (NEXUS_Playpump_GetBuffer(pContext->playpump, &buffer, &buffer_size))
        break;
        if (buffer_size < 188) {
            BKNI_WaitForEvent(pContext->dataReadyEvent, 10000);
            continue;
        }

        /* insert LAST BTP */
        if(endOfFile) {
#if 0
            insertBTP(buffer, 0, xcodeContext.inputSettings.iAudioPid, true);
            rc = NEXUS_Playpump_WriteComplete(pContext->playpump, 0, 188);
            BDBG_ASSERT(!rc);
            g_endOfFile = true;
#endif
            break;
        }

        /* The first call to get_buffer will return the entire playback buffer.
           If we use it, we're going to have to wait until the descriptor is complete,
           and then we're going to underflow. So use a max size. */
#define MAX_READ (188*1024)
        if (buffer_size > MAX_READ)
            buffer_size = MAX_READ;

        n = read(pContext->file, buffer, buffer_size);
        if (n < 0) BDBG_ASSERT(0);
        if (n == 0 && buffer_size) {
            endOfFile = true;
            BDBG_MSG(("Audio[%u] to end playfile...", pContext->xcodeId));
        }
        rc = NEXUS_Playpump_WriteComplete(pContext->playpump, 0, n);
        BDBG_ASSERT(!rc);
        BDBG_MODULE_MSG(audio_playpump_fnrt, ("A[%d] played %d bytes\n", pContext->xcodeId, n));
    }

    /* decrement count */
    B_Mutex_Lock(xcodeContext.mutexActiveXcoders);
    xcodeContext.activeXcoders--;
    BDBG_MSG(("Audio playpump[%d] is stopped. Active audio pump count = %d", pContext->xcodeId, xcodeContext.activeXcoders));
    if(g_eotEvent && xcodeContext.activeXcoders==0) {
        g_keyReturn = 'q';
        B_Event_Set(g_eotEvent);
    }
    B_Mutex_Unlock(xcodeContext.mutexActiveXcoders);
}


static int open_video_transcode(
	int                    i )
{
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_VideoDecoderStartSettings videoProgram;
	NEXUS_VideoDecoderSettings videoDecodeSettings;
	NEXUS_VideoEncoderSettings videoEncoderConfig;
	NEXUS_VideoEncoderStartSettings videoEncoderStartConfig;
	NEXUS_VideoEncoderDelayRange videoDelay;
	NEXUS_PlaypumpSettings playpumpSettings;

#ifndef NEXUS_NUM_DSP_VIDEO_ENCODERS
	NEXUS_DisplayStgSettings stgSettings;
#endif
	NEXUS_DisplaySettings displaySettings;
	NEXUS_VideoWindowSettings windowSettings;
	NEXUS_VideoWindowScalerSettings scalerSettings;
	NEXUS_VideoFormatInfo fmtInfo;
	bool bInterlacedXcode;
	NEXUS_DisplayCustomFormatSettings customFormatSettings;
	NEXUS_Error rc;
	VideoTranscoder *pContext = &xcodeContext.videoXcoder[i];
	EncodeSettings  *pEncodeSettings = &xcodeContext.encodeSettings;
	InputSettings   *pInputSettings = &xcodeContext.inputSettings; 

	if(pContext->videoEncoder) {
		return 0;
	}

	/* each transcoder potentially should only use 2 STCs on the same timebase: 
	       1) NRT video decode STC; (RT/NRT)
	       2) NRT audio decode STC; (NRT)
	       3) encode/mux STC;  (RT)
	   NOTE: to avoid the 3rd one since NRT mode doesn't really need it, 
	         encoder/mux STC is only required in RT mode; and in RT mode, a/v decoders share the same STC.
	 */
	{
	NEXUS_StcChannel_GetDefaultSettings(BTST_GET_STC_ID(BTST_SOURCE_STC_IDX, i), &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0+i;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	/*stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eAudioMaster;*/
	pContext->stcVideoChannel = NEXUS_StcChannel_Open(BTST_GET_STC_ID(BTST_SOURCE_STC_IDX, i), &stcSettings);
	BDBG_MSG(("Transcoder%d opened source vSTC %d [%p].", i, BTST_GET_STC_ID(BTST_SOURCE_STC_IDX, i), pContext->stcVideoChannel));
	}

	/* bring up decoder and connect to local display */
	{
		NEXUS_VideoDecoderOpenSettings openSettings;
		NEXUS_VideoDecoder_GetDefaultOpenSettings(&openSettings);
		openSettings.avc51Enabled = false;
		openSettings.svc3dSupported = false;
		openSettings.excessDirModeEnabled = false;
		openSettings.enhancementPidChannelSupported = false;
		pContext->videoDecoder = NEXUS_VideoDecoder_Open(BTST_XCODE_VID_DECODE_IDX-i, &openSettings); /* take default capabilities */
		BDBG_MSG(("Transcoder%d opened source decoder %d.", i, BTST_XCODE_VID_DECODE_IDX-i));
		NEXUS_VideoDecoder_GetSettings(pContext->videoDecoder, &videoDecodeSettings);
		videoDecodeSettings.streamChanged.callback = vidSrcStreamChangedCallback;
		videoDecodeSettings.streamChanged.context  = pContext->videoDecoder;
		BKNI_Memset(videoDecodeSettings.supportedCodecs, 0, sizeof(videoDecodeSettings.supportedCodecs));
		videoDecodeSettings.supportedCodecs[0] = NEXUS_VideoCodec_eH264; /* it's for regular HD decode heap allocation; it covers mpeg2/h264/mpeg4 HD size */
		videoDecodeSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eMuteUntilFirstPicture;

		/* FNRT settings */
		BKNI_CreateEvent(&pContext->chunkDoneEvent);
		videoDecodeSettings.fnrtSettings.chunkDone.param = i;
		videoDecodeSettings.fnrtSettings.chunkDone.context  = pContext->chunkDoneEvent;
		videoDecodeSettings.fnrtSettings.chunkDone.callback = vidChunkDoneCallback;
		videoDecodeSettings.fnrtSettings.enable = true;
		videoDecodeSettings.fnrtSettings.preChargeCount = g_chunkPrechargeCount;/* TODO: make it configurable according to content */

		NEXUS_VideoDecoder_SetSettings(pContext->videoDecoder, &videoDecodeSettings);

		/****************************************
		 * set up xcoder source
		 */
		{
			/* use stdio for file I/O to keep the example simple. */
			pContext->file = open(pInputSettings->fname, O_RDONLY | O_LARGEFILE, 0666);
			if (!pContext->file) {
			    fprintf(stderr, "can't open source file:%s\n", pInputSettings->fname);
			    BDBG_ASSERT(0);;
			}
			fcntl(pContext->file, FD_CLOEXEC, 1);
			if(g_fileSize == 0) {/* get the source media file size */
				g_fileSize = lseek(pContext->file, 0, SEEK_END);
				lseek(pContext->file, 0, SEEK_SET);
				BDBG_MSG(("Source file size: %llu bytes", g_fileSize));
			}

			pContext->playpump = NEXUS_Playpump_Open(NEXUS_ANY_ID, NULL);
			assert(pContext->playpump);
			BDBG_MSG(("Transcoder%d opened source playpump.", i));
			BKNI_CreateEvent(&pContext->dataReadyEvent);
			NEXUS_Playpump_GetSettings(pContext->playpump, &playpumpSettings);
			playpumpSettings.dataCallback.callback = play_callback;
			playpumpSettings.dataCallback.context = pContext->dataReadyEvent;
			NEXUS_Playpump_SetSettings(pContext->playpump, &playpumpSettings);

			/* Open the video pid channel */
			pContext->videoPidChannel = NEXUS_Playpump_OpenPidChannel(pContext->playpump, pInputSettings->iVideoPid, NULL);
		}

		/* Set up decoder Start structures now. We need to know the audio codec to properly set up
		the audio outputs. */
		NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
		videoProgram.codec = (NEXUS_VideoCodec)pInputSettings->eVideoCodec;
		videoProgram.pidChannel = pContext->videoPidChannel;
		videoProgram.stcChannel = pContext->stcVideoChannel;
		videoProgram.nonRealTime = true;
		pContext->vidProgram = videoProgram;
	}

	/****************************************
	 * Bring up video display and outputs 
	 */

	/* NOTE: must open video encoder before display; otherwise open will init ViCE2 core
	* which might cause encoder display GISB error since encoder display would
	* trigger RDC to program mailbox registers in ViCE2;
	*/
	/* NOTE: increase encoder output CDB/ITB size to hold the chunk transcoder output while waiting for serial MUX */
	{
	NEXUS_VideoEncoderOpenSettings encoderOpenSettings;
	unsigned fifoSize;
	NEXUS_VideoEncoder_GetDefaultOpenSettings(&encoderOpenSettings);

	/* CDB size has to hold at least chunk duration worth of data */
	fifoSize = ((xcodeContext.encodeSettings.encoderBitrate/1000) * g_chunkDuration /100 * BTST_DEFAULT_CHUNK_ALLOC_MARGIN / 8) & (-32);
	if(encoderOpenSettings.data.fifoSize < fifoSize) encoderOpenSettings.data.fifoSize = fifoSize;

	/* ITB size could be 128-bytes per frame */
	fifoSize = ((unsigned)convert_RefreshRate_FromFrameRate(xcodeContext.encodeSettings.encoderFrameRate) * g_chunkDuration * 128 /100 * BTST_DEFAULT_CHUNK_ALLOC_MARGIN / 1000) & (-32);
	if(encoderOpenSettings.index.fifoSize < fifoSize) encoderOpenSettings.index.fifoSize = fifoSize;

	BDBG_MSG(("Video encoder[%d] allocating %u bytes CDB and %u ITB buffer.", 
		pContext->xcodeId, encoderOpenSettings.data.fifoSize, encoderOpenSettings.index.fifoSize));
	pContext->videoEncoder = NEXUS_VideoEncoder_Open(i, &encoderOpenSettings);
	assert(pContext->videoEncoder);
	}
	BDBG_MSG(("opened video encoder %d.", i));

	/* Bring up video encoder display */
	NEXUS_Display_GetDefaultSettings(&displaySettings);
	
#ifndef NEXUS_NUM_DSP_VIDEO_ENCODERS
	displaySettings.displayType = NEXUS_DisplayType_eAuto;
	displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eEncoder;
	displaySettings.frameRateMaster = NULL;/* disable frame rate tracking for now */
#endif

	if(!pEncodeSettings->bCustom)
	{
		displaySettings.format = (NEXUS_VideoFormat)pEncodeSettings->displayFormat;
		pContext->displayTranscode = NEXUS_Display_Open(g_encodeDisplayId[i], &displaySettings);
		assert(pContext->displayTranscode);
		BDBG_MSG(("Transcoder%d opened encoder display%d [%p].", i, g_encodeDisplayId[i], pContext->displayTranscode));
		pContext->windowTranscode = NEXUS_VideoWindow_Open(pContext->displayTranscode, 0);
		assert(pContext->windowTranscode);
		NEXUS_VideoFormat_GetInfo(displaySettings.format, &fmtInfo);
		bInterlacedXcode = fmtInfo.interlaced;
	}
	else
	{
		pContext->displayTranscode = NEXUS_Display_Open(NEXUS_ENCODER_DISPLAY_IDX-i, &displaySettings);
		assert(pContext->displayTranscode);
		BDBG_MSG(("Transcoder%d opened encoder display%d [%p].", i, NEXUS_ENCODER_DISPLAY_IDX-i, pContext->displayTranscode));
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

#ifndef NEXUS_NUM_DSP_VIDEO_ENCODERS
	/* NRT setup - AFAP mode */
	NEXUS_Display_GetStgSettings(pContext->displayTranscode, &stgSettings);
	stgSettings.enabled     = true;
	stgSettings.nonRealTime = true;
	NEXUS_Display_SetStgSettings(pContext->displayTranscode, &stgSettings);
#endif

	NEXUS_VideoWindow_GetSettings(pContext->windowTranscode, &windowSettings);
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
	windowSettings.position.width = BTST_INPUT_MAX_WIDTH;
	windowSettings.position.height = BTST_INPUT_MAX_HEIGHT;
	windowSettings.pixelFormat = NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08;    
	windowSettings.visible = false;
#else 
	/* set transcoder minimum display format before AddInput to avoid black frame transition during dynamic resolution change */	
	windowSettings.minimumDisplayFormat = NEXUS_VideoFormat_e1080p;
	windowSettings.scaleFactorRounding.enabled = false;
	windowSettings.scaleFactorRounding.horizontalTolerance = 0;
	windowSettings.scaleFactorRounding.verticalTolerance   = 0;
#endif
	NEXUS_VideoWindow_SetSettings(pContext->windowTranscode, &windowSettings);

	/* set transcoder window vnet mode bias to avoid black frame transition during dynamic resolution change for mfd source;
	   NOTE: HDMI input might dynamically switch to 1080p60 and xcode BVN path doesn't have bandwidth to capture 1080p60! */
	{
		NEXUS_VideoWindow_GetScalerSettings(pContext->windowTranscode, &scalerSettings);
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
		scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerBeforeCapture;
#else		
		scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerAfterCapture;
#endif
		scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
		NEXUS_VideoWindow_SetScalerSettings(pContext->windowTranscode, &scalerSettings);
	}


	/* connect same decoder to the encoder display;
	* NOTE: simul display + transcode mode might have limitation in audio path;
	* here is for video transcode bringup/debug purpose;
	*/
	NEXUS_VideoWindow_AddInput(pContext->windowTranscode, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));

	/* VDC madr assignment is fixed */
	window_mad(pContext->windowTranscode, false, true, false);

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
	videoEncoderStartConfig.stcChannel = pContext->stcVideoChannel;
	videoEncoderStartConfig.interlaced = bInterlacedXcode;
	videoEncoderStartConfig.nonRealTime = true;


	/************************************
	 * transcoder AV sync is handled by FNRT mux
	 * encode setting and startSetting to be set after end-to-end delay is determined */
	/* get/set end-to-end delay (Dee) for audio and video encoders based on minimum;
	 */
	/* NOTE: video encoder delay is in 27MHz ticks */
	NEXUS_VideoEncoder_GetDelayRange(pContext->videoEncoder, &videoEncoderConfig, &videoEncoderStartConfig, &videoDelay);
	BDBG_MSG(("\n\tVideo encoder end-to-end delay = [%u ~ %u] ms", videoDelay.min/27000, videoDelay.max/27000));

	videoEncoderConfig.encoderDelay = videoDelay.min;

	/* encoder setting is global */
	xcodeContext.encodeSettings.videoEncoderConfig = videoEncoderConfig;

	/* store per encoder start settings since each instance has different interface */
	pContext->vidEncoderStartSettings = videoEncoderStartConfig;

	return 0;
}

static void close_video_transcode(
	int                i)
{
	VideoTranscoder *pContext = &xcodeContext.videoXcoder[i];

	if(!pContext->videoEncoder) {
		return;
	}

	/************************************
	 * Bring down transcoder 
	 */
	{
		NEXUS_Playpump_CloseAllPidChannels(pContext->playpump);
		NEXUS_Playpump_Close(pContext->playpump);
		BKNI_DestroyEvent(pContext->dataReadyEvent);
		close(pContext->file);
	}

	/******************************************
	 * nexus kernel mode requires explicit remove/shutdown video inputs before close windows/display
	 */
	{
		NEXUS_VideoWindow_RemoveInput(pContext->windowTranscode, NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
		NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(pContext->videoDecoder));
		NEXUS_VideoDecoder_Close(pContext->videoDecoder);
		BKNI_DestroyEvent(pContext->chunkDoneEvent);
	}
	NEXUS_VideoWindow_Close(pContext->windowTranscode);
	NEXUS_Display_Close(pContext->displayTranscode);

	NEXUS_VideoEncoder_Close(pContext->videoEncoder);
	pContext->videoEncoder = NULL;
	
	NEXUS_StcChannel_Close(pContext->stcVideoChannel);
}

/* stop without close */
static void stop_video_transcode( int i )
{
	VideoTranscoder *pContext = &xcodeContext.videoXcoder[i];

	if(!pContext->bStarted) {
		return;
	}

	B_Mutex_Lock(pContext->mutexStarted);
	/**************************************************
	 * NOTE: stop sequence should be in front->back order
	 */
	NEXUS_Playpump_Stop(pContext->playpump);

	NEXUS_VideoDecoder_Stop(pContext->videoDecoder);
	NEXUS_VideoEncoder_Stop(pContext->videoEncoder, NULL);

	B_Thread_Destroy(pContext->playpumpThread);

	/* stopped */
	pContext->bStarted = false;
	B_Mutex_Unlock(pContext->mutexStarted);

	BDBG_MSG(("stopped video xcoder[%d], activeCnt=%d", i, xcodeContext.activeXcoders));
}

/* start without open */
static void start_video_transcode(
	int                i )
{
	VideoTranscoder *pContext = &xcodeContext.videoXcoder[i];
	if(pContext->bStarted) {
		BDBG_WRN(("Video xcoder[%d] was already started!", i));
		return;
	}

	B_Mutex_Lock(xcodeContext.mutexActiveXcoders);
	xcodeContext.activeXcoders++;
	B_Mutex_Unlock(xcodeContext.mutexActiveXcoders);
	BDBG_MSG(("starting video xcoder[%d], activeCnt=%d", i, xcodeContext.activeXcoders));

	/****************************
	 * start decoders
	 */
	/* Start decoder */
	NEXUS_VideoDecoder_Start(pContext->videoDecoder, &pContext->vidProgram);

	/****************************
	 * setup video encoder
	 */
	NEXUS_VideoEncoder_SetSettings(pContext->videoEncoder, &xcodeContext.encodeSettings.videoEncoderConfig);

	/************************************************
	 * Start video encoder
	 */
	NEXUS_VideoEncoder_Start(pContext->videoEncoder, &pContext->vidEncoderStartSettings);
	/* Start playpump thread */
	NEXUS_Playpump_Start(pContext->playpump);
	pContext->playpumpThread = B_Thread_Create("FNRT playpump thread", (B_ThreadFunc)videoPlaypump_thread, pContext, NULL);

	/* started */
	pContext->bStarted = true;
}

static int open_audio_transcode(
	int                    i )
{
	NEXUS_StcChannelSettings stcSettings;
	NEXUS_AudioEncoderSettings encoderSettings;
	NEXUS_AudioMixerSettings audioMixerSettings;
	NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;
	NEXUS_AudioMuxOutputDelayStatus audioDelayStatus;
	NEXUS_AudioMuxOutputStartSettings audioMuxStartSettings;
	NEXUS_PlaypumpSettings playpumpSettings;
	AudioTranscoder *pContext = &xcodeContext.audioXcoder[i];
	EncodeSettings  *pEncodeSettings = &xcodeContext.encodeSettings;
	InputSettings   *pInputSettings = &xcodeContext.inputSettings; 

	if(pContext->audioMuxOutput || !pInputSettings->bAudioInput) {
		return 0;
	}

	/* each transcoder potentially should only use 2 STCs on the same timebase: 
	       1) NRT video decode STC; (RT/NRT)
	       2) NRT audio decode STC; (NRT)
	       3) encode/mux STC;  (RT)
	   NOTE: to avoid the 3rd one since NRT mode doesn't really need it, 
	         encoder/mux STC is only required in RT mode; and in RT mode, a/v decoders share the same STC.
	 */
	{
	NEXUS_StcChannel_GetDefaultSettings(NEXUS_ANY_ID, &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0+i;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto;
	stcSettings.modeSettings.Auto.behavior = NEXUS_StcChannelAutoModeBehavior_eAudioMaster;
	pContext->stcAudioChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
	BDBG_MSG(("Transcoder%d opened source aSTC  [%p].", pContext->xcodeId, pContext->stcAudioChannel));
	}

	/****************************************
	 * set up xcoder source
	 */
	{
		/* use stdio for file I/O to keep the example simple. */
		pContext->file = open(pInputSettings->fname, O_RDONLY | O_LARGEFILE, 0666);
		if (!pContext->file) {
		    fprintf(stderr, "can't open source file:%s\n", pInputSettings->fname);
		    BDBG_ASSERT(0);;
		}
		fcntl(pContext->file, FD_CLOEXEC, 1);

		pContext->playpump = NEXUS_Playpump_Open(NEXUS_ANY_ID, NULL);
		assert(pContext->playpump);
		BDBG_MSG(("Audio transcoder%d opened source playpump.", i));
		BKNI_CreateEvent(&pContext->dataReadyEvent);
		NEXUS_Playpump_GetSettings(pContext->playpump, &playpumpSettings);
		playpumpSettings.dataCallback.callback = play_callback;
		playpumpSettings.dataCallback.context = pContext->dataReadyEvent;
		NEXUS_Playpump_SetSettings(pContext->playpump, &playpumpSettings);

		/* Open the video pid channel */
		pContext->audioPidChannel = NEXUS_Playpump_OpenPidChannel(pContext->playpump, pInputSettings->iAudioPid, NULL);
	}

	/* Open the audio decoder */
	/* Connect audio decoders to outputs */
	NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
	audioDecoderOpenSettings.dspIndex    = i;
	if(g_bMultiChanAudio) {
		audioDecoderOpenSettings.multichannelFormat = xcodeContext.inputSettings.eMultiChanFmt;
	}
	pContext->audioDecoder = NEXUS_AudioDecoder_Open(i, &audioDecoderOpenSettings);

	/* Set up decoder Start structures now. We need to know the audio codec to properly set up
	the audio outputs. */
	NEXUS_AudioDecoder_GetDefaultStartSettings(&pContext->audProgram);
	pContext->audProgram.codec = pInputSettings->eAudioCodec;
	pContext->audProgram.pidChannel = pContext->audioPidChannel;
	pContext->audProgram.nonRealTime= true;
	pContext->audProgram.stcChannel = pContext->stcAudioChannel;

	/* Open audio mixer.  The mixer can be left running at all times to provide continuous audio output despite input discontinuities.  */
	if(!g_bNoDspMixer) {
		NEXUS_AudioMixer_GetDefaultSettings(&audioMixerSettings);
		audioMixerSettings.mixUsingDsp = true;
		audioMixerSettings.dspIndex    = i;
		pContext->audioMixer = NEXUS_AudioMixer_Open(&audioMixerSettings);
		assert(pContext->audioMixer);
	}

	/* Open audio mux output */
{	/* increase audio mux output buffer size to match with parallel video encoders buffers at serial FNRT mux */
	NEXUS_AudioMuxOutputCreateSettings settings;
	NEXUS_AudioMuxOutput_GetDefaultCreateSettings(&settings);
	settings.data.fifoSize  *= g_vidCnt / g_audCnt;
	settings.index.fifoSize *= g_vidCnt / g_audCnt;
	pContext->audioMuxOutput = NEXUS_AudioMuxOutput_Create(&settings);
	assert(pContext->audioMuxOutput);
	BDBG_MSG(("Audio[%u] mux output= %p", i, pContext->audioMuxOutput));
}
	if(pEncodeSettings->bAudioEncode)
	{
		/* Open audio encoder */
		NEXUS_AudioEncoder_GetDefaultSettings(&encoderSettings);
		encoderSettings.codec = pEncodeSettings->encoderAudioCodec;
		pContext->audioEncoder = NEXUS_AudioEncoder_Open(&encoderSettings);
		assert(pContext->audioEncoder);

		if(g_bNoDspMixer) {
			/* Connect decoder to encoder */
			NEXUS_AudioEncoder_AddInput(pContext->audioEncoder,
				NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
			BDBG_WRN(("No DSP mixer..."));
		}
		else {
			/* Connect decoder to mixer and set as master */
			NEXUS_AudioMixer_AddInput(pContext->audioMixer,
				NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, 
					(xcodeContext.inputSettings.eMultiChanFmt > NEXUS_AudioMultichannelFormat_eStereo)?
					NEXUS_AudioDecoderConnectorType_eMultichannel : NEXUS_AudioDecoderConnectorType_eStereo));
			audioMixerSettings.master = NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, 
					(xcodeContext.inputSettings.eMultiChanFmt > NEXUS_AudioMultichannelFormat_eStereo)?
					NEXUS_AudioDecoderConnectorType_eMultichannel : NEXUS_AudioDecoderConnectorType_eStereo);
			NEXUS_AudioMixer_SetSettings(pContext->audioMixer, &audioMixerSettings);

			/* Connect mixer to encoder */
			NEXUS_AudioEncoder_AddInput(pContext->audioEncoder, 
				NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
		}

		/* Connect mux to encoder */
		NEXUS_AudioOutput_AddInput(
			NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput), 
			NEXUS_AudioEncoder_GetConnector(pContext->audioEncoder));
	}
	else
	{
		if(g_bNoDspMixer) {
			/* Connect decoder to mux out */
			NEXUS_AudioOutput_AddInput(
				NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput),
				NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
		}
		else {
			NEXUS_AudioMixer_AddInput(pContext->audioMixer,
				NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
			audioMixerSettings.master = NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed);
			NEXUS_AudioMixer_SetSettings(pContext->audioMixer, &audioMixerSettings);
			NEXUS_AudioOutput_AddInput(
				NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput),
				NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
		}
		pEncodeSettings->encoderAudioCodec = pContext->audProgram.codec;
	}

	/************************************
	 * transcoder AV sync is handled by FNRT mux
	 * encode setting and startSetting to be set after end-to-end delay is determined */
	/* get/set end-to-end delay (Dee) for audio and video encoders based on minimum;
	 */
	NEXUS_AudioMuxOutput_GetDelayStatus(pContext->audioMuxOutput, xcodeContext.encodeSettings.encoderAudioCodec, &audioDelayStatus);
	BDBG_MSG(("\n\tAudio codec %d end-to-end delay = %u ms", xcodeContext.encodeSettings.encoderAudioCodec, audioDelayStatus.endToEndDelay));

	/* Start audio mux output */
	NEXUS_AudioMuxOutput_GetDefaultStartSettings(&audioMuxStartSettings);
	/* audio NRT requires mux out to take NRT decode STC */
	audioMuxStartSettings.stcChannel        = pContext->stcAudioChannel ;
	audioMuxStartSettings.presentationDelay = audioDelayStatus.endToEndDelay/27000;/* in ms */
	audioMuxStartSettings.nonRealTime       = true;
	/* store per encoder start settings since each instance has different interface */
	pContext->audMuxStartSettings = audioMuxStartSettings;

	return 0;
}

static void close_audio_transcode(
	int                i)
{
	AudioTranscoder *pContext = &xcodeContext.audioXcoder[i];

	if(!pContext->audioMuxOutput) {
		return;
	}

	/************************************
	 * Bring down transcoder 
	 */
	{
		NEXUS_Playpump_CloseAllPidChannels(pContext->playpump);
		NEXUS_Playpump_Close(pContext->playpump);
		BKNI_DestroyEvent(pContext->dataReadyEvent);
		close(pContext->file);
	}

	/******************************************
	 * nexus kernel mode requires explicit remove/shutdown video inputs before close windows/display
	 */
	if(xcodeContext.encodeSettings.bAudioEncode)
	{
		NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput));
		NEXUS_AudioEncoder_RemoveAllInputs(pContext->audioEncoder);
		NEXUS_AudioInput_Shutdown(NEXUS_AudioEncoder_GetConnector(pContext->audioEncoder));
		NEXUS_AudioEncoder_Close(pContext->audioEncoder);
		if(!g_bNoDspMixer) {
			NEXUS_AudioMixer_RemoveAllInputs(pContext->audioMixer);
			NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
			NEXUS_AudioMixer_Close(pContext->audioMixer);
		}
		NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
		NEXUS_AudioDecoder_Close(pContext->audioDecoder);
	}
	else
	{
		NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput));
		if(!g_bNoDspMixer) {
			NEXUS_AudioMixer_RemoveAllInputs(pContext->audioMixer);
			NEXUS_AudioInput_Shutdown(NEXUS_AudioMixer_GetConnector(pContext->audioMixer));
			NEXUS_AudioMixer_Close(pContext->audioMixer);
		}
		NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pContext->audioDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
		NEXUS_AudioDecoder_Close(pContext->audioDecoder);
	}

	NEXUS_AudioOutput_Shutdown(NEXUS_AudioMuxOutput_GetConnector(pContext->audioMuxOutput));
	NEXUS_AudioMuxOutput_Destroy(pContext->audioMuxOutput);
	pContext->audioMuxOutput = NULL;
	
	NEXUS_StcChannel_Close(pContext->stcAudioChannel);
}

/* stop without close */
static void stop_audio_transcode( int i )
{
	AudioTranscoder *pContext = &xcodeContext.audioXcoder[i];

	if(!pContext->bStarted) {
		return;
	}

	/**************************************************
	 * NOTE: stop sequence should be in front->back order
	 */
	NEXUS_Playpump_Stop(pContext->playpump);

	NEXUS_AudioDecoder_Stop(pContext->audioDecoder);

	if(!g_bNoDspMixer) {
		NEXUS_AudioMixer_Stop(pContext->audioMixer);
	}
	NEXUS_AudioMuxOutput_Stop(pContext->audioMuxOutput);

	B_Thread_Destroy(pContext->playpumpThread);

	/* stopped */
	pContext->bStarted = false;

	BDBG_MSG(("stopped audio xcoder[%d], activeCnt=%d", i, xcodeContext.activeXcoders));
}

/* start without open */
static void start_audio_transcode(
	int                i )
{
	AudioTranscoder *pContext = &xcodeContext.audioXcoder[i];
	if(pContext->bStarted) {
		BDBG_WRN(("Audio xcoder[%d] was already started!", i));
		return;
	}

	B_Mutex_Lock(xcodeContext.mutexActiveXcoders);
	xcodeContext.activeXcoders++;
	B_Mutex_Unlock(xcodeContext.mutexActiveXcoders);
	BDBG_MSG(("starting audio xcoder[%d], activeCnt=%d", i, xcodeContext.activeXcoders));

	/****************************
	 * start decoders
	 */
	/* Start audio mux output */
	NEXUS_AudioMuxOutput_Start(pContext->audioMuxOutput, &pContext->audMuxStartSettings);
	/* Start audio mixer */
	if(!g_bNoDspMixer) {
		NEXUS_AudioMixer_Start(pContext->audioMixer);
	}
	NEXUS_AudioDecoder_Start(pContext->audioDecoder, &pContext->audProgram);

	/* Start playpump thread */
	NEXUS_Playpump_Start(pContext->playpump);
	pContext->playpumpThread = B_Thread_Create("FNRT audio playpump thread", (B_ThreadFunc)audioPlaypump_thread, pContext, NULL);

	/* started */
	pContext->bStarted = true;
}

static void start_mux(
	FnrtTranscodeContext  *pContext )
{
	if(g_bTsMux) {/* TS mux */
		NEXUS_StreamMuxOutput muxOutput;
		NEXUS_RecordPidChannelSettings recordPidSettings;

		NEXUS_StreamMux_Start(pContext->streamMux,&pContext->tsMuxConfig, &muxOutput);
		pContext->pidChannelTranscodeVideo = muxOutput.video[0];

		/* add multiplex data to the same record */
		/* configure the video pid for indexing */
		NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
		recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
		recordPidSettings.recpumpSettings.pidTypeSettings.video.index = /* RAVE only support mpeg2&avc indexing */
				(pContext->encodeSettings.encoderVideoCodec==NEXUS_VideoCodec_eMpeg2) ||
				(pContext->encodeSettings.encoderVideoCodec==NEXUS_VideoCodec_eH264);
		recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = pContext->encodeSettings.encoderVideoCodec;
		NEXUS_Record_AddPidChannel(pContext->record, pContext->pidChannelTranscodeVideo, &recordPidSettings);

		if(pContext->inputSettings.bAudioInput)
		{
			pContext->pidChannelTranscodeAudio = muxOutput.audio[0];
			NEXUS_Record_AddPidChannel(pContext->record, pContext->pidChannelTranscodeAudio, NULL);
		}
		NEXUS_Record_AddPidChannel(pContext->record, pContext->pidChannelTranscodePcr, NULL);

		/* Start record of stream mux output */
		pContext->fileTranscode = NEXUS_FileRecord_OpenPosix(pContext->encodeSettings.fname,
			pContext->indexfname[0]? pContext->indexfname : NULL);
		NEXUS_Record_Start(pContext->record, pContext->fileTranscode);
	} else {/* MP4 mux */
		NEXUS_FileMux_Start(pContext->fileMux,&pContext->mp4MuxConfig, pContext->mp4MuxOutput);
	}
}

static void finish_mux(
	FnrtTranscodeContext  *pContext )
{
	if(g_bTsMux) {
		NEXUS_StreamMux_Finish(pContext->streamMux);
		if(BKNI_WaitForEvent(pContext->finishEvent, BKNI_INFINITE)!=BERR_SUCCESS) {
			fprintf(stderr, "TIMEOUT\n");
		}
		NEXUS_Record_Stop(pContext->record);
		NEXUS_FileRecord_Close(pContext->fileTranscode);
		/*****************************************
		 * Note: remove all record PID channels before stream 
		 * mux stop since streammux would close the A/V PID channels 
		 */
		NEXUS_Record_RemoveAllPidChannels(pContext->record);
	} else {
		NEXUS_FileMux_Finish(pContext->fileMux);
		BDBG_MSG(("finishing file mux..."));
		if(BKNI_WaitForEvent(pContext->finishEvent, BKNI_INFINITE)!=BERR_SUCCESS) {
			fprintf(stderr, "TIMEOUT\n");
		}
	}
}

static void stop_mux(
	FnrtTranscodeContext  *pContext )
{
	if(g_bTsMux) {
		NEXUS_StreamMux_Stop(pContext->streamMux);
		NEXUS_StreamMux_Destroy(pContext->streamMux);
		NEXUS_Record_Destroy(pContext->record);
		NEXUS_Playpump_Close(pContext->playpumpTranscodePcr);
		NEXUS_Playpump_Close(pContext->playpumpTranscodeVideo);
		if(pContext->inputSettings.bAudioInput) NEXUS_Playpump_Close(pContext->playpumpTranscodeAudio);
	} else {
		NEXUS_FileMux_Stop(pContext->fileMux);
		NEXUS_MuxFile_Close(pContext->mp4MuxOutput);
		NEXUS_FileMux_Destroy(pContext->fileMux);
	}
}

#if !BTST_DISABLE_NRT_DEBUG_DISPLAY
static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void open_monitor_display (
	FnrtTranscodeContext  *pContext )
{
	NEXUS_DisplaySettings displaySettings;
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_VideoFormatInfo info;
    NEXUS_Error rc;
    ProgressBarSettings pbarSettings;
#endif

	/* bringup the simul display out of xcoder[0] */
	NEXUS_Display_GetDefaultSettings(&displaySettings);
	displaySettings.format = NEXUS_VideoFormat_e720p;
	pContext->display = NEXUS_Display_Open(0, &displaySettings);
	BDBG_MSG(("Xcoder[0] opened display 0 [%p] with component and hdmi outputs.", pContext->display));
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
	NEXUS_VideoWindow_AddInput(pContext->window, NEXUS_VideoDecoder_GetConnector(pContext->videoXcoder[0].videoDecoder));
#if 0/* force SCL after CAP? */
	{
		NEXUS_VideoWindowScalerSettings scalerSettings;
		NEXUS_VideoWindow_GetScalerSettings(pContext->window, &scalerSettings);
		scalerSettings.bandwidthEquationParams.bias = NEXUS_ScalerCaptureBias_eScalerAfterCapture;
		scalerSettings.bandwidthEquationParams.delta = 1*1000*1000;
		NEXUS_VideoWindow_SetScalerSettings(pContext->window, &scalerSettings);
	}
#endif

#if NEXUS_HAS_GRAPHICS2D
    /* draw progress bar surface */
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &info);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = info.width;
    createSettings.height = info.height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    pContext->surface = NEXUS_Surface_Create(&createSettings);

    BKNI_CreateEvent(&pContext->checkpointEvent);
    BKNI_CreateEvent(&pContext->spaceAvailableEvent);

    pContext->gfx = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Graphics2D_GetSettings(pContext->gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = pContext->checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = pContext->spaceAvailableEvent;
    NEXUS_Graphics2D_SetSettings(pContext->gfx, &gfxSettings);

    /* fill with black */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = pContext->surface;
    fillSettings.rect.width = createSettings.width;
    fillSettings.rect.height = createSettings.height;
    fillSettings.color = 0;
    NEXUS_Graphics2D_Fill(pContext->gfx, &fillSettings);
    
    rc = NEXUS_Graphics2D_Checkpoint(pContext->gfx, NULL); /* require to execute queue */
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        rc = BKNI_WaitForEvent(pContext->checkpointEvent, BKNI_INFINITE);
    }
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetGraphicsSettings(pContext->display, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(pContext->display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(pContext->display, pContext->surface);

    /* mux progress bar */
    pbarSettings.gfx = pContext->gfx;
    pbarSettings.surface = pContext->surface;
    pbarSettings.rect.x = 140;
    pbarSettings.rect.width = createSettings.width - 2* pbarSettings.rect.x;
    pbarSettings.rect.y = 620;
    pbarSettings.rect.height = 50;
    pbarSettings.maxProgress = 1000; /* work with 0 ~ 1000%o */
    pbarSettings.resetColor = 0xffffffff; /* white */
    pbarSettings.fillColor = 0xff0000ff; /* blue */
    pContext->pbar = ProgressBar_Open(&pbarSettings);
    BDBG_ASSERT(pContext->pbar);
    ProgressBar_Reset(pContext->pbar);

    /* video chunks progress bar: fill the background white */
    pbarSettings.gfx = pContext->gfx;
    pbarSettings.surface = pContext->surface;
    pbarSettings.rect.x = 140;
    pbarSettings.rect.width = createSettings.width - 2* pbarSettings.rect.x;
    pbarSettings.rect.y = 520;
    pbarSettings.rect.height = 50;
    pbarSettings.maxProgress = 1000; /* work with 0 ~ 1000%o */
    pbarSettings.resetColor = 0xffffffff; /* white */
    pbarSettings.fillColor = 0xff0000ff; /* blue */
    pContext->vidChunkPbar[0] = ProgressBar_Open(&pbarSettings);
    BDBG_ASSERT(pContext->vidChunkPbar[0]);
    ProgressBar_Reset(pContext->vidChunkPbar[0]);
    ProgressBar_Close(pContext->vidChunkPbar[0]);
    pContext->vidChunkPbar[0] = NULL;
#endif
}

static void close_monitor_display (
	FnrtTranscodeContext  *pContext )
{
#if NEXUS_HAS_GRAPHICS2D
    ProgressBar_Close(pContext->pbar);
    NEXUS_Graphics2D_Close(pContext->gfx);
    NEXUS_Surface_Destroy(pContext->surface);

    BKNI_DestroyEvent(pContext->checkpointEvent);
    BKNI_DestroyEvent(pContext->spaceAvailableEvent);
#endif

	/* shutdown the simul display */
	NEXUS_VideoWindow_RemoveInput(pContext->window, NEXUS_VideoDecoder_GetConnector(pContext->videoXcoder[0].videoDecoder));
	NEXUS_VideoWindow_Close(pContext->window);
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_StopCallbacks(g_platformConfig.outputs.hdmi[0]);
#endif
	NEXUS_Display_Close(pContext->display);
}
#endif
static void b_print_media_string(const bmedia_probe_stream *stream)
{
    char stream_info[512];
    bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
    printf( "\n\nMedia Probe:\n" "%s\n\n", stream_info);
}

static bool g_bProbe = false;
struct {
    NEXUS_VideoCodec nexus;
    bvideo_codec settop;
} g_videoCodec[] = {
    {NEXUS_VideoCodec_eUnknown, bvideo_codec_none},
    {NEXUS_VideoCodec_eUnknown, bvideo_codec_unknown},
    {NEXUS_VideoCodec_eMpeg1, bvideo_codec_mpeg1},
    {NEXUS_VideoCodec_eMpeg2, bvideo_codec_mpeg2},
    {NEXUS_VideoCodec_eMpeg4Part2, bvideo_codec_mpeg4_part2},
    {NEXUS_VideoCodec_eH263, bvideo_codec_h263},
    {NEXUS_VideoCodec_eH264, bvideo_codec_h264},
    {NEXUS_VideoCodec_eH264_Svc, bvideo_codec_h264_svc},
    {NEXUS_VideoCodec_eH264_Mvc, bvideo_codec_h264_mvc},
    {NEXUS_VideoCodec_eVc1, bvideo_codec_vc1},
    {NEXUS_VideoCodec_eVc1SimpleMain, bvideo_codec_vc1_sm},
    {NEXUS_VideoCodec_eDivx311, bvideo_codec_divx_311},
    {NEXUS_VideoCodec_eRv40, bvideo_codec_rv40},
    {NEXUS_VideoCodec_eVp6, bvideo_codec_vp6},
    {NEXUS_VideoCodec_eVp8, bvideo_codec_vp8},
    {NEXUS_VideoCodec_eSpark, bvideo_codec_spark},
    {NEXUS_VideoCodec_eMotionJpeg, bvideo_codec_mjpeg},
    {NEXUS_VideoCodec_eAvs, bvideo_codec_avs}
};

struct {
    NEXUS_AudioCodec nexus;
    baudio_format settop;
} g_audioCodec[] = {
   {NEXUS_AudioCodec_eUnknown, baudio_format_unknown},
   {NEXUS_AudioCodec_eMpeg, baudio_format_mpeg},
   {NEXUS_AudioCodec_eMp3, baudio_format_mp3},
   {NEXUS_AudioCodec_eAac, baudio_format_aac},
   {NEXUS_AudioCodec_eAacPlus, baudio_format_aac_plus},
   {NEXUS_AudioCodec_eAacPlusAdts, baudio_format_aac_plus_adts},
   {NEXUS_AudioCodec_eAacPlusLoas, baudio_format_aac_plus_loas},
   {NEXUS_AudioCodec_eAc3, baudio_format_ac3},
   {NEXUS_AudioCodec_eAc3Plus, baudio_format_ac3_plus},
   {NEXUS_AudioCodec_eDts, baudio_format_dts},
   {NEXUS_AudioCodec_eLpcmHdDvd, baudio_format_lpcm_hddvd},
   {NEXUS_AudioCodec_eLpcmBluRay, baudio_format_lpcm_bluray},
   {NEXUS_AudioCodec_eDtsHd, baudio_format_dts_hd},
   {NEXUS_AudioCodec_eWmaStd, baudio_format_wma_std},
   {NEXUS_AudioCodec_eWmaPro, baudio_format_wma_pro},
   {NEXUS_AudioCodec_eLpcmDvd, baudio_format_lpcm_dvd},
   {NEXUS_AudioCodec_eAvs, baudio_format_avs},
   {NEXUS_AudioCodec_eAmr, baudio_format_amr},
   {NEXUS_AudioCodec_eDra, baudio_format_dra},
   {NEXUS_AudioCodec_eCook, baudio_format_cook},
   {NEXUS_AudioCodec_ePcmWav, baudio_format_pcm},
   {NEXUS_AudioCodec_eAdpcm, baudio_format_adpcm},
   {NEXUS_AudioCodec_eAdpcm, baudio_format_dvi_adpcm},
   {NEXUS_AudioCodec_eVorbis, baudio_format_vorbis},
   {NEXUS_AudioCodec_eLpcm1394, baudio_format_lpcm_1394},
   {NEXUS_AudioCodec_eFlac, baudio_format_flac},
   {NEXUS_AudioCodec_eApe, baudio_format_ape},
   {NEXUS_AudioCodec_eDtsLegacy, baudio_format_dts_cd},
};

#define CONVERT(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].settop == settop_value) { \
            return g_struct[i].nexus; \
        } \
    } \
    printf("unable to find value %d in %s\n", settop_value, #g_struct); \
    return g_struct[0].nexus

NEXUS_VideoCodec b_videocodec2nexus(bvideo_codec settop_value)
{
    CONVERT(g_videoCodec);
}

NEXUS_AudioCodec b_audiocodec2nexus(baudio_format settop_value)
{
    CONVERT(g_audioCodec);
}
int media_probe(InputSettings *opts, const char *filename)
{
    int rc = 0;

    /* use media probe to set values */
    bmedia_probe_t probe = NULL;
    bmedia_probe_config probe_config;
    const bmedia_probe_stream *stream = NULL;
    const bmedia_probe_track *track = NULL;
    bfile_io_read_t fd = NULL;
    bool foundAudio = false, foundVideo = false;
    FILE *fin;

    probe = bmedia_probe_create();

    fin = fopen64(filename,"rb");
    if (!fin) {
        printf("can't open media file '%s' for probing\n", filename);
        rc = -1;
        goto done;
    }

    fd = bfile_stdio_read_attach(fin);

    bmedia_probe_default_cfg(&probe_config);
    probe_config.file_name = filename;
    probe_config.type = bstream_mpeg_type_unknown;
    stream = bmedia_probe_parse(probe, fd, &probe_config);

    /* now stream is either NULL, or stream descriptor with linked list of audio/video tracks */
    bfile_stdio_read_detach(fd);

    fclose(fin);
    if(!stream) {
        printf("media probe can't parse stream '%s'\n", filename);
        rc = -1;
        goto done;
    }

    b_print_media_string(stream);
    xcodeContext.duration = stream->duration;

    if (g_bProbe) {
        opts->eVideoCodec = NEXUS_VideoCodec_eUnknown;
        opts->eAudioCodec = NEXUS_AudioCodec_eUnknown;

        for(track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
            switch(track->type) {
                case bmedia_track_type_audio:
                    if(track->info.audio.codec != baudio_format_unknown && !foundAudio) {
                        opts->iAudioPid = track->number;
                        opts->eAudioCodec = b_audiocodec2nexus(track->info.audio.codec);
                        foundAudio = true;
                    }
                    break;
                case bmedia_track_type_video:
                    if (track->info.video.codec != bvideo_codec_unknown && !foundVideo) {
                        opts->iVideoPid = track->number;
                        opts->eVideoCodec = b_videocodec2nexus(track->info.video.codec);
                        foundVideo = true;
                    }
                    break;
                case bmedia_track_type_pcr:
                    opts->iPcrPid = track->number;
                    break;
                default:
                    break;
            }
        }
    }

done:
    if (probe) {
        if (stream) {
            bmedia_probe_stream_free(probe, stream);
        }
        bmedia_probe_destroy(probe);
    }

    return rc;
}

static struct timeval initTimeStamp;
void
BDBG_P_GetTimeStamp(char *timeStamp, int size_t)
{
    struct timeval currentTime;
    int hours, minutes, seconds;
    int milliseconds;
    int rc;

    rc = gettimeofday(&currentTime, NULL);
    if (rc!=0) {
        BDBG_P_PrintString("### debug: gettimeofday returned %d, ignored", rc);
    }

    if (currentTime.tv_usec < initTimeStamp.tv_usec)
    {
        milliseconds = (currentTime.tv_usec - initTimeStamp.tv_usec + 1000000)/1000;
        currentTime.tv_sec--;
    }
    else    {
        milliseconds = (currentTime.tv_usec - initTimeStamp.tv_usec)/1000;
    }

    /* Calculate the time   */
    hours = (currentTime.tv_sec - initTimeStamp.tv_sec)/3600;
    minutes = (((currentTime.tv_sec - initTimeStamp.tv_sec)/60))%60;
    seconds = (currentTime.tv_sec - initTimeStamp.tv_sec)%60;

    /* print the formatted time including the milliseconds  */
    rc = BKNI_Snprintf(timeStamp, size_t, "%02u:%02u:%02u.%03u", hours, minutes, seconds, milliseconds);
    return;

}

#define BTST_PBAR_SIZE(start, end, length, scale) ((uint16_t)(scale*(end-start)/length))
#define BTST_PBAR_COLOR(i) (\
	(i==0)? 0xffff0000 :\
	((i==1)? 0xff00ff00 :\
	((i==2)? 0xff808000 : 0xff800080)))
static void monitor_thread( void *pContext )
{
#if !BTST_DISABLE_NRT_DEBUG_DISPLAY && NEXUS_HAS_GRAPHICS2D
	unsigned i;
	uint16_t start, end;
	ProgressBarSettings pbarSettings;
#endif
	unsigned hrs, mins, secs, duration;
	char time[80];
	NEXUS_FileMuxStatus status;
	NEXUS_FileMuxHandle mux = (NEXUS_FileMuxHandle)pContext;
	NEXUS_StreamMuxStatus tsMuxStatus;
	NEXUS_StreamMuxHandle tsMux = (NEXUS_StreamMuxHandle)pContext;

	fprintf(stderr, "\n\n");
	do{
		BKNI_Sleep(1000);
		if(g_bTsMux) {
			NEXUS_StreamMux_GetStatus(tsMux, &tsMuxStatus);
			duration = tsMuxStatus.duration;
		} else {
			NEXUS_FileMux_GetStatus(mux, &status);
			duration = status.duration;
		}
		secs = duration/1000;
		mins = secs/60;
		hrs  = mins/60;
		mins = mins%60;
		secs = secs%60;
		BDBG_P_GetTimeStamp(time, 80);
		fprintf(stderr, " Elapsed: %s; Mux completed: %02u:%02u:%02u.%03u; %.1f percents\r", time,
			hrs, mins, secs, duration%1000, (float)100*duration/xcodeContext.duration);
#if !BTST_DISABLE_NRT_DEBUG_DISPLAY && NEXUS_HAS_GRAPHICS2D
		/* draw mux progress bar */
		if(xcodeContext.pbar) {
			end = BTST_PBAR_SIZE(0,duration,xcodeContext.duration, 1000);
			if(end > 0) ProgressBar_FillSimple(xcodeContext.pbar, end);
		}
		/* chunk feeders progress bars */
		for(i=0; i < g_vidCnt; i++) {
			if(xcodeContext.videoXcoder[i].chunk.endOffset != 0) {/* valid chunk */
				/* 1) if no chunk pbar, open it */
				if(xcodeContext.vidChunkPbar[i] == NULL) {
					pbarSettings.gfx = xcodeContext.gfx;
					pbarSettings.surface = xcodeContext.surface;
					pbarSettings.rect.x = 140;
					pbarSettings.rect.width = 1000;
					pbarSettings.rect.y = 520;
					pbarSettings.rect.height = 50;
					pbarSettings.maxProgress = 1000; 
					pbarSettings.resetColor = 0xffffffff; /* white */
					pbarSettings.fillColor = BTST_PBAR_COLOR(i); /* red */
					xcodeContext.vidChunkPbar[i] = ProgressBar_Open(&pbarSettings);
					BDBG_ASSERT(xcodeContext.vidChunkPbar[i]);
					BDBG_MODULE_MSG(fnrt_pbar, ("V[%u]chunk[%u] bar open: (%u, %u) %u x %u, color: 0x%x max: %u", i, xcodeContext.videoXcoder[i].chunk.chunkId,
					pbarSettings.rect.x, pbarSettings.rect.y, pbarSettings.rect.width, pbarSettings.rect.height, pbarSettings.fillColor, pbarSettings.maxProgress));
				}
				/* 2) else if prevchunk ID is different, fill up prevChunk pbar and draw a new pbar for current chunk */
				else if((xcodeContext.videoXcoder[i].prevChunk.endOffset != 0) && (xcodeContext.videoXcoder[i].chunk.chunkId != xcodeContext.videoXcoder[i].prevChunk.chunkId)) {
					start = BTST_PBAR_SIZE(0,xcodeContext.videoXcoder[i].prevChunk.startOffset,g_fileSize, 1000);
					end = BTST_PBAR_SIZE(0,xcodeContext.videoXcoder[i].prevChunk.endOffset,g_fileSize, 1000);
					BDBG_MODULE_MSG(fnrt_pbar, ("V[%u] fill chunk[%u] bar: %u->%u", i, xcodeContext.videoXcoder[i].prevChunk.chunkId,
						start, end));
					if(end > start) ProgressBar_FillSegment(xcodeContext.vidChunkPbar[i], start, end);
				}
				start = BTST_PBAR_SIZE(0,xcodeContext.videoXcoder[i].chunk.startOffset,g_fileSize, 1000);
				end = BTST_PBAR_SIZE(0,xcodeContext.videoXcoder[i].fileOffset,g_fileSize, 1000);
				BDBG_MODULE_MSG(fnrt_pbar, ("V[%u] chunk[%u] bar: %u->%u", i, xcodeContext.videoXcoder[i].chunk.chunkId,
					start, end));
				if(end > start) ProgressBar_FillSegment(xcodeContext.vidChunkPbar[i], start, end);

				xcodeContext.videoXcoder[i].prevChunk = xcodeContext.videoXcoder[i].chunk;
			}
		}
#endif
	} while (!xcodeContext.bShutdown);
	fprintf(stderr, "\n\n");

#if !BTST_DISABLE_NRT_DEBUG_DISPLAY && NEXUS_HAS_GRAPHICS2D
	for(i=0; i < g_vidCnt; i++) {
		if(xcodeContext.vidChunkPbar[i]) {
			ProgressBar_Close(xcodeContext.vidChunkPbar[i]);
			xcodeContext.vidChunkPbar[i] = NULL;
		}
	}
#endif
	BKNI_SetEvent(xcodeContext.monitorDone);
}

static void bringup_transcode(
	FnrtTranscodeContext  *pContext )
{
	NEXUS_Error rc;
	unsigned i;
	BNAV_Player_Settings settings;
	char srcIndexFileName[256] = {'\0',};

	xcode_index_filename(srcIndexFileName, pContext->inputSettings.fname);
	pContext->indexFile = fopen(srcIndexFileName, "r");
	if (!pContext->indexFile) {
		fprintf(stderr, "Source file %s has no index.\n", pContext->inputSettings.fname);
		BDBG_ASSERT(0);
	}

    if (media_probe(&pContext->inputSettings, pContext->inputSettings.fname)) {
        BERR_TRACE(NEXUS_UNKNOWN);
        return;
    }

	BNAV_Player_GetDefaultSettings(&settings);
	settings.videoPid = pContext->inputSettings.iVideoPid;
	settings.readCb = (BP_READ_CB)fread;
	settings.tellCb = (BP_TELL_CB)ftell;
	settings.seekCb = (BP_SEEK_CB)fseek;
	settings.filePointer = pContext->indexFile;
	rc = BNAV_Player_Open(&pContext->bcmplayer, &settings);
	BDBG_ASSERT(!rc);

	gettimeofday(&initTimeStamp,NULL);

	for(i=0; i < g_vidCnt; i++)
	{
		pContext->videoXcoder[i].xcodeId = i;
		pContext->videoXcoder[i].mutexStarted = B_Mutex_Create(NULL);

		/* open the transcode context */
		open_video_transcode(i);

		/************************************************
		 * Set up video xcoder mux and record
		 */
		if(g_bTsMux) {/* TS mux */
			if(i == 0) {/* open handles for the first video channel */
				open_tsmux(pContext);
			} else {/* assign the same PID etc to the rest FNRT video channels, except the encoder is different */
				pContext->tsMuxConfig.video[i].pid   = pContext->tsMuxConfig.video[0].pid;
				pContext->tsMuxConfig.video[i].pesId = pContext->tsMuxConfig.video[0].pesId;
				pContext->tsMuxConfig.video[i].playpump = pContext->tsMuxConfig.video[0].playpump;
				pContext->tsMuxConfig.video[i].encoder = pContext->videoXcoder[i].videoEncoder;
			}
		} else {/* MP4 mux */
			if(i == 0) {
				open_mp4mux(pContext);
			} else {
				pContext->mp4MuxConfig.video[i].codec = pContext->encodeSettings.encoderVideoCodec;
				pContext->mp4MuxConfig.video[i].track = pContext->mp4MuxConfig.video[0].track;
				pContext->mp4MuxConfig.video[i].encoder = pContext->videoXcoder[i].videoEncoder;
			}
		}
	}

	if(pContext->inputSettings.bAudioInput) {/* open audio xcoders */
		for(i=0; i < g_audCnt; i++)
		{
			open_audio_transcode(i);
			if(g_bTsMux) {/* set up TS mux audio channel */
				if(i==0) {
					xcode_audio_tsmux_record(pContext); continue;
				}
				pContext->tsMuxConfig.audio[i].muxOutput = pContext->audioXcoder[i].audioMuxOutput;
				pContext->tsMuxConfig.audio[i].pid   = pContext->tsMuxConfig.audio[0].pid;
				pContext->tsMuxConfig.audio[i].pesId = pContext->tsMuxConfig.audio[0].pesId;
				pContext->tsMuxConfig.audio[i].playpump = pContext->tsMuxConfig.audio[0].playpump;
			} else {/* MP4 mux */
				pContext->mp4MuxConfig.audio[i].codec = pContext->encodeSettings.encoderAudioCodec;
				pContext->mp4MuxConfig.audio[i].muxOutput = pContext->audioXcoder[i].audioMuxOutput;
				pContext->mp4MuxConfig.audio[i].track = 2;
			}
		}
	}

	/**************************************************
	 * NOTE: transcoder record/mux start
	 */
	/* start mux */
	start_mux(pContext);

#if !BTST_DISABLE_NRT_DEBUG_DISPLAY
	open_monitor_display(pContext);
#endif
	BKNI_CreateEvent(&xcodeContext.monitorDone);
	pContext->monitorThread = B_Thread_Create("FNRT monitor thread", (B_ThreadFunc)monitor_thread, g_bTsMux?(void*)pContext->streamMux:(void*)pContext->fileMux, NULL);

	for(i=0; i < g_vidCnt; i++)
	{
		/*******************************
		 * START transcoder 
		 */
		start_video_transcode(i);
	}
	if(pContext->inputSettings.bAudioInput) {/* start audio xcoders */
		for(i=0; i < g_audCnt; i++)
		{
			start_audio_transcode(i);
		}
	}
}

static void shutdown_transcode(
	FnrtTranscodeContext  *pContext )
{
	unsigned i=0;
	if(pContext->inputSettings.bAudioInput)
	{
		NEXUS_AudioDecoderStatus status;
		for(i=0; i<2; ) {
			NEXUS_AudioDecoder_GetStatus(pContext->audioXcoder[0].audioDecoder, &status);
			BDBG_MSG(("audio decoder0 has %u queued frames...", status.queuedFrames));
			if(status.queuedFrames < 3) {
				i++;
			}
			BKNI_Sleep(1000);
		}
	}
	if(pContext->inputSettings.bAudioInput) {/* start audio xcoders */
		for(i=0; i < g_audCnt; i++)
		{
			stop_audio_transcode(i);
		}
	}
#if BTST_DISABLE_AUTO_STOP
	for(i=0; i < g_vidCnt; i++)
	{
		stop_video_transcode(i);
	}
#endif
	finish_mux(pContext);

	pContext->bShutdown = true;
	while(BKNI_WaitForEvent(xcodeContext.monitorDone,BKNI_INFINITE) == BERR_TIMEOUT);
	BKNI_DestroyEvent(xcodeContext.monitorDone);
#if !BTST_DISABLE_NRT_DEBUG_DISPLAY
	close_monitor_display(pContext);
#endif
	B_Thread_Destroy(pContext->monitorThread);

	/* stop mux */
	BDBG_MSG(("stopping mux..."));
	stop_mux(pContext);
	BKNI_DestroyEvent(pContext->finishEvent);

#if !BTST_DISABLE_AUTO_STOP
	for(i=0; i < g_vidCnt; i++)
	{
	  stop_video_transcode(i);
	}
#endif

	if(pContext->inputSettings.bAudioInput) {
		for(i=0; i < g_audCnt; i++)
		{
			close_audio_transcode(i);
		}
	}
	for(i=0; i < g_vidCnt; i++)
	{
		/* close the transcode context */
		close_video_transcode(i);

		B_Mutex_Destroy(pContext->videoXcoder[i].mutexStarted);
	}
	BNAV_Player_Close(pContext->bcmplayer);
	fclose(pContext->indexFile);
}
#endif

static void config_xcoder_context (
	FnrtTranscodeContext *pContext )
{
	int choice;
	char input[80];
	pContext->inputSettings.bConfig = true;

	printf("\n source stream file: ");
	scanf("%s", pContext->inputSettings.fname);
	printf("\n source stream type:  (%d) Es (%d) Ts           ", 
		NEXUS_TransportType_eEs, NEXUS_TransportType_eTs);	   scanf("%d", (int32_t*)&pContext->inputSettings.eStreamType);
	pContext->inputSettings.eStreamType = NEXUS_TransportType_eTs;
	if(!g_bProbe) {
		printf("\n source stream codec: (%d) Mpeg2 (%d) H264      ",
			NEXUS_VideoCodec_eMpeg2, NEXUS_VideoCodec_eH264);	   
		scanf("%d", (int32_t*)&pContext->inputSettings.eVideoCodec);
		printf("\n Video pid:                                           ");	   scanf("%s", input); pContext->inputSettings.iVideoPid = strtoul(input, NULL, 0);
		printf("\n Pcr   pid:                                           ");	   scanf("%s", input); pContext->inputSettings.iPcrPid = strtoul(input, NULL, 0);
	}
	printf("\n Input Audio Parameters");
	printf("\n Enable Audio channel: (0) No (1) Yes             ");	   scanf("%d", &choice);
	pContext->inputSettings.bAudioInput = (choice!=0);
	if(pContext->inputSettings.bAudioInput)
	{
		if(!g_bProbe) {
			printf("\n Audio Pid:                                           "); scanf("%s", input); pContext->inputSettings.iAudioPid = strtoul(input, NULL, 0);

			printf("\n Audio Codec:\n"
				" (%d)Mpg\n"
				" (%d)AAC\n"
				" (%d)AAC+Loas\n"
				" (%d)AAC+Adts\n"
				" (%d)AC3\n"
				" (%d)AC3+\n",
				NEXUS_AudioCodec_eMpeg,
				NEXUS_AudioCodec_eAac,
				NEXUS_AudioCodec_eAacPlusLoas,
				NEXUS_AudioCodec_eAacPlusAdts,
				NEXUS_AudioCodec_eAc3,
				NEXUS_AudioCodec_eAc3Plus);
			scanf("%d", (int32_t*)&pContext->inputSettings.eAudioCodec);
			pContext->encodeSettings.encoderAudioCodec = pContext->inputSettings.eAudioCodec;
		}
		if(g_bMultiChanAudio) {
			printf("\n Multi-channel audio format: \n"
				" (%d)None\n"
				" (%d)Stereo\n"
				" (%d)5.1\n"
				" (%d)7.1\n",
				NEXUS_AudioMultichannelFormat_eNone,
				NEXUS_AudioMultichannelFormat_eStereo,
				NEXUS_AudioMultichannelFormat_e5_1,
				NEXUS_AudioMultichannelFormat_e7_1);
			scanf("%d", (int32_t*)&pContext->inputSettings.eMultiChanFmt);
		}
	}

	printf("\n****************************************************************\n");
	printf("\n Input Parameters\n");
	printf("\n filename:       %s", pContext->inputSettings.fname);

	if(pContext->inputSettings.bAudioInput)
	{
		printf("\n Enable Audio channel Pid: %#x\n audio codec: %d           ", pContext->inputSettings.iAudioPid, pContext->inputSettings.eAudioCodec);
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
	printf("\n Encode Video Codec: (%d) MPEG2 (%d) MPEG4 Part2 (%d) H264                            ", 
		NEXUS_VideoCodec_eMpeg2, NEXUS_VideoCodec_eMpeg4Part2, NEXUS_VideoCodec_eH264); 
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
		printf("\n Enable Audio transcode: (0) No (1) Yes                                                       "); scanf("%d", &choice);
		pContext->encodeSettings.bAudioEncode = choice;

		if(pContext->encodeSettings.bAudioEncode)
		{
			printf("\n Encode Audio Codec: (%d)Mpg (%d)Mp3 (%d)AAC (%d)AAC+ (%d)AAC+Loas (%d)AAC+Adts (%d)AC3                              ",
				NEXUS_AudioCodec_eMpeg,
				NEXUS_AudioCodec_eMp3,
				NEXUS_AudioCodec_eAac,
				NEXUS_AudioCodec_eAacPlus,
				NEXUS_AudioCodec_eAacPlusLoas,
				NEXUS_AudioCodec_eAacPlusAdts,
				NEXUS_AudioCodec_eAc3); 
			scanf("%d", (int32_t*)&pContext->encodeSettings.encoderAudioCodec);
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
}

void usage(void) {
			printf("\ntranscode_ts usage:\n");
			printf("  Without options, it transcodes default stream file /data/videos/avatar_AVC_15M.ts into TS file: /data/BAT/encode.ts\n");
			printf("\nOptions:\n");
			printf("  -h        - to print the usage info\n");
			printf("  -cfg      - to set the test configuration\n");
			printf("  -scr      - to run in script mode with config input by script file\n");
			printf("  -nodspmix - to disable audio DSP fw mixer.\n");
			printf("  -multichan - to enable audio multi-channel mode setting.\n");
			printf("  -vpipes N - to set max N parallel video xcoders.\n");
			printf("  -chunktime MSEC - to set video chunk duration in miliseconds.\n");
			printf("  -precharge N - to set video chunk precharge pictures count N for chunk overlap alg.\n");
			printf("  -pd         - to enable progressive download mp4 file mux. Default OFF.\n");
			printf("  -rbs BYTEs  - to specify relocation buffer size in bytes.\n");
			printf("  -probe      - to probe the source PIDs and codecs.\n");
			printf("  -bitrate N  - to specify video encode bitrate in bps.\n");
			printf("  -audio      - to enable audio transcode.\n");
}

int main(int argc, char **argv)  {

#if NEXUS_HAS_VIDEO_ENCODER && NEXUS_HAS_FILE_MUX

	NEXUS_PlatformSettings platformSettings;
	FnrtTranscodeContext *pContext = &xcodeContext;
	int i;
	bool bAudio = false;

	BSTD_UNUSED(argv);
	BKNI_Memset(pContext, 0, sizeof(xcodeContext));
	if ( argc >= 2) {
		for(i=1; i<argc; i++) {
			if(!strcmp("-h",argv[i])) {
				usage();
				return 0;
			}
			if(!strcmp("-cfg",argv[i])) {
				pContext->inputSettings.bConfig = true;
				fprintf(stderr, "Manually configure settings...\n");
			}
			if(!strcmp("-nodspmix",argv[i])) {
				g_bNoDspMixer = true;
				fprintf(stderr, "No DSP mixer for audio transcode...\n");
			}
			if(!strcmp("-multichan",argv[i])) {
				g_bMultiChanAudio = true;
				fprintf(stderr, "Multi-channel format for audio source. must have fw mixer..\n");
			}
			if(!strcmp("-vpipes",argv[i])) {
				g_vidCnt = strtoul(argv[++i], NULL, 0);
				if(g_vidCnt > NEXUS_NUM_VIDEO_ENCODERS) g_vidCnt = NEXUS_NUM_VIDEO_ENCODERS;
				fprintf(stderr, "Max Video Inputs = %u threads\n", g_vidCnt);
			}
			if(!strcmp("-chunktime",argv[i])) {
				g_chunkDuration = strtoul(argv[++i], NULL, 0);
				fprintf(stderr, "Video chunk duration = %u ms\n", g_chunkDuration);
			}
			if(!strcmp("-precharge",argv[i])) {
				g_chunkPrechargeCount = strtoul(argv[++i], NULL, 0);
				fprintf(stderr, "Video chunk precharge count = %u pictures\n", g_chunkPrechargeCount);
			}
			if(!strcmp("-pd",argv[i])) {
			    g_bProgressiveDownload = true;
			    fprintf(stderr, "Progressive download is enabled.\n");
			}
			if(!strcmp("-rbs",argv[i])) {
			    g_relocBufSize = strtoul(argv[++i], NULL, 0);
			    fprintf(stderr, "Relocation buffer size = %u bytes.\n", g_relocBufSize);
			}
			if(!strcmp("-bitrate",argv[i])) {
			    g_bitrate = strtoul(argv[++i], NULL, 0);
			    fprintf(stderr, "Video encode bitrate = %u bps.\n", g_bitrate);
			}
			if(!strcmp("-probe",argv[i])) {
			    g_bProbe = true;
			    fprintf(stderr, "Media probe is enabled.\n");
			}
			if(!strcmp("-audio",argv[i])) {
			    bAudio = true;
			    fprintf(stderr, "Audio is enabled.\n");
			}
			if(!strcmp("-tsmux",argv[i])) {
			    g_bTsMux = true;
			    fprintf(stderr, "TS mux is enabled.\n");
			}
			if(!strcmp("-scr",argv[i])) {
				g_bScriptMode = true;
				fprintf(stderr, "Enabled script mode and quit at end of FNRT...\n");
			}
		}
	}
	if(pContext->inputSettings.bConfig || g_bScriptMode) {
		config_xcoder_context(pContext);/* transcoder 0 context user settings */
	}
	else { /* default test config for auto test */
			usage();
			printf("\nYou're testing with the default configuration:\n");
			/* Input setting */
			BKNI_Snprintf(pContext->inputSettings.fname, 256, "/data/videos/avatar_AVC_15M.ts");
			pContext->inputSettings.eStreamType=NEXUS_TransportType_eTs;
			pContext->inputSettings.eVideoCodec=NEXUS_VideoCodec_eH264;
			pContext->inputSettings.eAudioCodec=NEXUS_AudioCodec_eAc3;
			pContext->inputSettings.iVideoPid=0x101;
			pContext->inputSettings.iAudioPid=0x104;
			pContext->inputSettings.iPcrPid=0x101;
			pContext->inputSettings.bAudioInput=bAudio;
			pContext->inputSettings.bConfig = false;
			pContext->inputSettings.eMultiChanFmt = NEXUS_AudioMultichannelFormat_eStereo;
			printf("\n****************************************************************\n");
			printf("\n Input Parameters\n");
			printf("\n filename:       %s", pContext->inputSettings.fname);

			{
				printf("\n Streamtype:  %d \n VideoCodec:  %d", pContext->inputSettings.eStreamType, pContext->inputSettings.eVideoCodec);
				printf("\n vPid:  %4d;  PcrPid:   %4d", pContext->inputSettings.iVideoPid, pContext->inputSettings.iPcrPid);
			}
			if(pContext->inputSettings.bAudioInput)
			{
				printf("\n Enable Audio channel Pid: %d\n audio codec: %d           ", pContext->inputSettings.iAudioPid, pContext->inputSettings.eAudioCodec);
			}
			printf("\n Config %1d", pContext->inputSettings.bConfig);
			printf("\n****************************************************************\n");
			

			/*Encode settings */
			BKNI_Snprintf(pContext->encodeSettings.fname, 256, g_bTsMux? "/data/videos/stream.ts":"/data/videos/stream.mp4");
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
			pContext->encodeSettings.displayFormat=NEXUS_VideoFormat_e480p;
#else
			pContext->encodeSettings.displayFormat=NEXUS_VideoFormat_e720p24hz;
#endif
			pContext->encodeSettings.encoderFrameRate=NEXUS_VideoFrameRate_e23_976;
			pContext->encodeSettings.encoderGopStructureFramesP=47;
			pContext->encodeSettings.encoderGopStructureFramesB=0;
			pContext->encodeSettings.encoderVideoCodec=NEXUS_VideoCodec_eH264;
#ifdef NEXUS_NUM_DSP_VIDEO_ENCODERS
			pContext->encodeSettings.encoderProfile=NEXUS_VideoCodecProfile_eBaseline;
			pContext->encodeSettings.encoderLevel=NEXUS_VideoCodecLevel_e30;
			pContext->encodeSettings.encoderBitrate=400000;
#else
			pContext->encodeSettings.encoderProfile=NEXUS_VideoCodecProfile_eHigh;
			pContext->encodeSettings.encoderLevel=NEXUS_VideoCodecLevel_e31;
			pContext->encodeSettings.encoderBitrate=g_bitrate;
#endif			 
			pContext->encodeSettings.bCustom=false;
			pContext->encodeSettings.bAudioEncode=true;
			pContext->encodeSettings.encoderAudioCodec=NEXUS_AudioCodec_eAac;

			printf("\n****************************************************************\n");
			printf("\n Encode Parameters\n");
			printf("\n filename:  %s", pContext->encodeSettings.fname);
			printf("\n display format:  %d", pContext->encodeSettings.displayFormat);
			printf("\n Frame Rate: %d\n Bit Rate: %d", pContext->encodeSettings.encoderFrameRate, pContext->encodeSettings.encoderBitrate);
			printf("\n P frames: %d;     B frames: %d", pContext->encodeSettings.encoderGopStructureFramesP, pContext->encodeSettings.encoderGopStructureFramesB);
			printf("\n Video Codec: %4d;  Profile: %4d;  Level: %4d", pContext->encodeSettings.encoderVideoCodec, pContext->encodeSettings.encoderProfile,pContext->encodeSettings.encoderLevel );
			if(pContext->encodeSettings.bAudioEncode)
				printf("\n Audio Codec: %4d", pContext->encodeSettings.encoderAudioCodec);
			printf("\nVideo threads: %u", g_vidCnt);
			printf("\n****************************************************************\n");
	}

	/* init platform */
	B_Os_Init();
	NEXUS_Platform_GetDefaultSettings(&platformSettings);
	/* enable frontend if the 1st xcoder needs it for now; not always enable to avoid slow frontend init for other cases;
	   TODO: init frontend in case 1st disable but 2nd enables frontend. */
	platformSettings.openFrontend = false;
	/* audio PI supports 4 by default; we need one extra mixers for each transcoders; */
	if(platformSettings.audioModuleSettings.maxAudioDspTasks < 2 * g_audCnt) {
		platformSettings.audioModuleSettings.maxAudioDspTasks = 2 * g_audCnt;
	}
	platformSettings.audioModuleSettings.numCompressedBuffers += g_audCnt;

	/* increase each encoder CDB/ITB allocation by 32MB as start point (TODO: it depends on max chunk duration and encode bitrate) */
	#ifndef NEXUS_MEMC0_DRIVER_HEAP
	platformSettings.heap[NEXUS_MEMC0_MAIN_HEAP].size += 32 * g_vidCnt * 1024 * 1024; /* linux boot option should increase bmem size */
	#else
	platformSettings.heap[NEXUS_MEMC0_DRIVER_HEAP].size += 32 * g_vidCnt * 1024 * 1024; /* linux boot option should increase bmem size */
	#endif
	NEXUS_Platform_Init(&platformSettings);
	NEXUS_Platform_GetConfiguration(&g_platformConfig);

	pContext->mutexChunk         = B_Mutex_Create(NULL);
	pContext->mutexActiveXcoders = B_Mutex_Create(NULL);

	/* bringup the transcode context */
	bringup_transcode(pContext);

	/****************************************************
	 *                       key handler                                                 *
	 *****************************************************/
	/* wait for 'q' to exit the test */
	if (pContext->inputSettings.bConfig && !g_bScriptMode)
	{
		B_ThreadSettings threadSettings;
		B_ThreadHandle keyHandle;

		g_eotEvent = B_Event_Create(NULL);
		B_Event_Reset(g_eotEvent);
		B_Thread_GetDefaultSettings(&threadSettings);
		threadSettings.terminate = true;
		keyHandle = B_Thread_Create("key handler", (B_ThreadFunc)keyHandler, &xcodeContext, &threadSettings);
		while(B_Event_Wait(g_eotEvent, B_WAIT_FOREVER)!=B_ERROR_SUCCESS);
		B_Thread_Destroy(keyHandle);
		B_Mutex_Lock(xcodeContext.mutexActiveXcoders);
		B_Event_Destroy(g_eotEvent);
		g_eotEvent = NULL;
		B_Mutex_Unlock(xcodeContext.mutexActiveXcoders);
	}
	while(xcodeContext.activeXcoders) BKNI_Sleep(50);/* wait for all video threads done */

	shutdown_transcode(pContext);

	B_Mutex_Destroy(pContext->mutexChunk);
	B_Mutex_Destroy(pContext->mutexActiveXcoders);

	/* uninit platform */
	NEXUS_Platform_Uninit();
	B_Os_Uninit();
#endif
	return 0;
}

/* End of file */
