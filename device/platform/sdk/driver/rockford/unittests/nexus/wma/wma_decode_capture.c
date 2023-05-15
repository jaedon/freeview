/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: wma_decode_capture.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 9/6/12 6:02p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/wma/wma_decode_capture.c $
 * 
 * Hydra_Software_Devel/4   9/6/12 6:02p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_stc_channel.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_audio_capture.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"

#include "nexus_surface.h"

#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif

#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include "../../../../nexus/utils/cmdline_args.h"
#include "../../../../nexus/utils/fileio_custom.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "bstd.h"
#include "bkni.h"


#include "bmedia_probe.h"
#include "bmpeg2ts_probe.h"

#if B_HAS_ASF
#include "basf_probe.h"
#endif

#if B_HAS_AVI
#include "bavi_probe.h"
#endif

#include "bfile_stdio.h"

BDBG_MODULE(playback);
static const namevalue_t g_transportTypeStrs[] = {
    {"ts",  NEXUS_TransportType_eTs},
    {"pes", NEXUS_TransportType_eMpeg2Pes},
    {"es",  NEXUS_TransportType_eEs},
    {"vob", NEXUS_TransportType_eVob},
    {"mp4", NEXUS_TransportType_eMp4},
    {"mkv", NEXUS_TransportType_eMkv},
    {"avi", NEXUS_TransportType_eAvi},
    {"asf", NEXUS_TransportType_eAsf},
    {"wav", NEXUS_TransportType_eWav},
    {NULL, 0}
};


static const namevalue_t g_audioCodecStrs[] = {
    {"mpeg", NEXUS_AudioCodec_eMpeg},
    {"mp3", NEXUS_AudioCodec_eMp3},
    {"ac3", NEXUS_AudioCodec_eAc3},
    {"ac3plus", NEXUS_AudioCodec_eAc3Plus},
    {"aac", NEXUS_AudioCodec_eAac},
    {"aacplus", NEXUS_AudioCodec_eAacPlus},
    {"wma", NEXUS_AudioCodec_eWmaStd},
    {"wmastd", NEXUS_AudioCodec_eWmaStd},
    {"wmapro", NEXUS_AudioCodec_eWmaPro},
    {"pcm", NEXUS_AudioCodec_ePcmWav},
    {"dra", NEXUS_AudioCodec_eDra},
    {"dts", NEXUS_AudioCodec_eDts},
    {"dtshd", NEXUS_AudioCodec_eDtsHd},
    {NULL, 0}
};

static const namevalue_t g_stcChannelMasterStrs[] = {
    {"first", NEXUS_StcChannelAutoModeBehavior_eFirstAvailable},
    {"video", NEXUS_StcChannelAutoModeBehavior_eVideoMaster},
    {"audio", NEXUS_StcChannelAutoModeBehavior_eAudioMaster},
    {NULL, 0}
};

static const namevalue_t g_endOfStreamActionStrs[] = {
    {"loop",  NEXUS_PlaybackLoopMode_eLoop},
    {"pause", NEXUS_PlaybackLoopMode_ePause},
    {"play",  NEXUS_PlaybackLoopMode_ePlay},
    {NULL, 0}
};

static const namevalue_t g_tsTimestampType[] = {
    {"none", NEXUS_TransportTimestampType_eNone},
    {"mod300", NEXUS_TransportTimestampType_eMod300},
    {"binary", NEXUS_TransportTimestampType_eBinary},
    {NULL, 0}
};

static const namevalue_t g_contentModeStrs[] = {
    {"zoom", NEXUS_VideoWindowContentMode_eZoom},
    {"box", NEXUS_VideoWindowContentMode_eBox},
    {"panscan", NEXUS_VideoWindowContentMode_ePanScan},
    {"full", NEXUS_VideoWindowContentMode_eFull},
    {"nonlinear", NEXUS_VideoWindowContentMode_eFullNonLinear},
    {NULL, 0}
};

static const namevalue_t g_panelStrs[] = {
    {"wxga", 0},
    {"wxga50", 1},
    {"fhd", 2},
    {"fhd50", 3},
    {"off", 0xff},
    {NULL, 0}
};



static unsigned lookup(const namevalue_t *table, const char *name)
{
    unsigned i;
    unsigned value;
    for (i=0;table[i].name;i++) {
        if (!strcasecmp(table[i].name, name)) {
            return table[i].value;
        }
    }
    errno = 0; /* there is only way to know that strtol failed is to look at errno, so clear it first */
    value = strtol(name, NULL, 0);
    if(errno != 0) {
        errno = 0;
        value = table[0].value;
    }
    printf("Unknown cmdline param '%s', using %u as value\n", name, value);
    return value;
}

	  
/*--------------------------------------------------------------------------Command line parsing------------------------------------------------------------------------------------------------- */
int wma_cmdline_parse(int argc, const char *argv[], struct util_opts_t *opts)
{
    int i;

	int loopback=0;

    memset(opts,0,sizeof(*opts));
    opts->transportType = NEXUS_TransportType_eTs;
    opts->audioCodec = NEXUS_AudioCodec_eMpeg;
    opts->compressedAudio = false;
    opts->decodedAudio = true;
#if NEXUS_DTV_PLATFORM
    opts->usePanelOutput = true;
    opts->displayType = NEXUS_DisplayType_eLvds;
#else
    opts->useCompositeOutput = true;
    opts->useComponentOutput = true;
    opts->displayFormat = NEXUS_VideoFormat_eNtsc;
    opts->displayType = NEXUS_DisplayType_eAuto;
#endif
    opts->stcChannelMaster = NEXUS_StcChannelAutoModeBehavior_eFirstAvailable;
    opts->tsTimestampType = NEXUS_TransportTimestampType_eNone;
    opts->beginningOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    opts->endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    opts->playpumpTimestampReordering = true;
    opts->customFileIo = false;
    opts->playbackMonitor = false;

    for (i=1;i<argc;i++) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
           printf("\nRun the app by running the following command:\n");
		   printf("\n ./\ nexus wma_decode_capture -probe streamname -loopback \n");
           printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");
			
        }
        else if (!strcmp(argv[i], "-probe")) {
            opts->probe = true;
        }
        else if (!strcmp(argv[i], "-loopback")) {
            loopback = 1;
        }

		else if (!opts->filename) {
            opts->filename = argv[i];
        }
        else if (!opts->indexname) {
            opts->indexname = argv[i];
        }
        else {
            printf("unknown param %s\n", argv[i]);
            return -1;
        }
    }

    if (opts->usePanelOutput) {
        opts->displayType = NEXUS_DisplayType_eLvds;
        opts->useComponentOutput = false;
        opts->useCompositeOutput = false;
    }
    else {
        opts->displayType = NEXUS_DisplayType_eAuto;
    }

    if (opts->transportType == NEXUS_TransportType_eEs && !opts->audioPid) {
           opts->audioPid = 1;
       }

    return loopback;
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*-----------------------------------------------------------------------PCM Capture callback------------------------------------------------------------------------------------------------------ */

static void capture_callback(void *pParam, int param)
{
    NEXUS_AudioCaptureHandle capture = pParam;
    FILE *pFile = (FILE *)param;
    NEXUS_Error errCode;

    for ( ;; )
    {
        void *pBuffer;
        size_t bufferSize;

        /* Check available buffer space */
        errCode = NEXUS_AudioCapture_GetBuffer(capture, (void **)&pBuffer, &bufferSize);
        if ( errCode )
        {
            fprintf(stderr, "Error getting capture buffer\n");
            NEXUS_AudioCapture_Stop(capture);
            return;
        }

        if ( bufferSize > 0 )
        {
            /* Write samples to disk */
            if ( 1 != fwrite(pBuffer, bufferSize, 1, pFile) )
            {
                fprintf(stderr, "Error writing to disk\n");
                NEXUS_AudioCapture_Stop(capture);
                return;
            }

            /*fprintf(stderr, "Data callback - Wrote %d bytes\n", (int)bufferSize);*/
            errCode = NEXUS_AudioCapture_WriteComplete(capture, bufferSize);
            if ( errCode )
            {
                fprintf(stderr, "Error committing capture buffer\n");
                NEXUS_AudioCapture_Stop(capture);
                return;
            }
        }
        else
        {
            break;
        }
    }
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*----------------------------------------------------------- Functions for starting and stopping audio decoder and capture-------------------------------------------------------------------------*/
static void start_audio(const struct util_opts_t *opts, NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderHandle compressedDecoder, NEXUS_AudioDecoderStartSettings *audioProgram,NEXUS_AudioCaptureHandle capture, FILE *pFile)
{
    NEXUS_Error rc;
    NEXUS_AudioCaptureStartSettings captureSettings;
    NEXUS_AudioCaptureSettings audiocaptureSettings;

    audioProgram->targetSyncEnabled = false;/*Disabling Target sync to capture the last frame data correctly*/

    
    if (opts->audioPid) {
#if B_HAS_ASF
        /* if DRC for WMA pro is available apply now */
        if(audioProgram->codec == NEXUS_AudioCodec_eWmaPro && opts->dynamicRangeControlValid ){

            NEXUS_AudioDecoderCodecSettings codecSettings;

            BDBG_WRN(("wma-pro drc enabled,ref peak %d,ref target %d,ave peak %d, ave target %d",
                      opts->dynamicRangeControl.peakReference,opts->dynamicRangeControl.peakTarget,
                      opts->dynamicRangeControl.averageReference,opts->dynamicRangeControl.averageTarget));
            NEXUS_AudioDecoder_GetCodecSettings(audioDecoder, audioProgram->codec, &codecSettings);
            codecSettings.codec = audioProgram->codec;
            codecSettings.codecSettings.wmaPro.dynamicRangeControlValid = true;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.peakReference = opts->dynamicRangeControl.peakReference;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.peakTarget = opts->dynamicRangeControl.peakTarget;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.averageReference = opts->dynamicRangeControl.averageReference;
            codecSettings.codecSettings.wmaPro.dynamicRangeControl.averageTarget = opts->dynamicRangeControl.averageTarget;
            NEXUS_AudioDecoder_SetCodecSettings(audioDecoder,&codecSettings);
            }
#endif

  /* Start the capture -- no data will be received until the decoder starts */
    NEXUS_AudioCapture_GetDefaultStartSettings(&captureSettings);
    captureSettings.dataCallback.callback = capture_callback;
    captureSettings.dataCallback.context = capture;
    captureSettings.dataCallback.param = (int)pFile;

	NEXUS_AudioCapture_GetSettings(capture,&audiocaptureSettings);
	audiocaptureSettings.format=NEXUS_AudioCaptureFormat_e24BitStereo;
	NEXUS_AudioCapture_SetSettings(capture,&audiocaptureSettings);

    NEXUS_AudioCapture_Start(capture, &captureSettings);

        if(opts->decodedAudio) {
            rc = NEXUS_AudioDecoder_Start(audioDecoder, audioProgram);
            BDBG_ASSERT(!rc);
        }
        if(compressedDecoder) {
            rc = NEXUS_AudioDecoder_Start(compressedDecoder, audioProgram);
            BDBG_ASSERT(!rc);
        }
	}
    return;
}

static void stop_audio(const struct util_opts_t *opts, NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderHandle compressedDecoder, NEXUS_AudioCaptureHandle capture)
{
	fprintf(stderr, "Stopping capture\n");
	NEXUS_StopCallbacks(capture);
	NEXUS_AudioCapture_Stop(capture);

    if (opts->audioPid) {
        if(opts->decodedAudio) {
            NEXUS_AudioDecoder_Stop(audioDecoder);
			NEXUS_AudioCapture_Stop(capture);
        }
        if(compressedDecoder) {
            NEXUS_AudioDecoder_Stop(compressedDecoder);
        }
    }
    return;
}
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*--------------------------------------------------Monitoring the playback------------------------------------------------------------------------------------------------------------------------ */
#define  B_HAS_PLAYBACK_MONITOR 0
#if B_HAS_PLAYBACK_MONITOR
#include <pthread.h>
#include "bkni_multi.h"

typedef struct PlaybackMonitorState {
    bool terminate;
    pthread_t thread;
    BKNI_MutexHandle lock;
    const struct util_opts_t *opts;
    NEXUS_PlaybackHandle playback;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderHandle compressedDecoder;
    const NEXUS_AudioDecoderStartSettings *audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_FilePlayHandle customFile;
    NEXUS_FilePlayHandle stickyFile;
    const NEXUS_PlaybackStartSettings *playbackStartSettings;
	
} PlaybackMonitorState;

static void *
monitor_thread(void *state_)
{  
    const PlaybackMonitorState *state=state_;
    while(!state->terminate) {
		
        NEXUS_PlaybackStatus status;
        NEXUS_PlaybackSettings playbackSettings;
        BERR_Code rc;
        bool failed;

        rc = NEXUS_Playback_GetStatus(state->playback, &status);
        BDBG_ASSERT(!rc);
        BKNI_Sleep(1000);
		printf("\n\n the playback status is:%d \n\n",status.state);
        FileIoSticky_GetFailBit(state->stickyFile, &failed);
        if(!failed) {
            continue;
        }
        BDBG_WRN(("restarting from %u", status.position));
        BKNI_AcquireMutex(state->lock);
        NEXUS_Playback_Stop(state->playback);
        FileIoSticky_ClearFailBit(state->stickyFile);
        if(state->customFile) {
            FileIoCustomProbabilities probabilities;
            FileIoCustom_GetProbabilities(state->customFile, NULL, &probabilities);
            probabilities.error = 0;
            probabilities.nodata = 0;
            probabilities.partial_read = 0;
            FileIoCustom_SetProbabilities(state->customFile, &probabilities, &probabilities);
        }

        stop_audio(state->opts, state->audioDecoder, state->compressedDecoder,capture);

        NEXUS_Playback_GetSettings(state->playback, &playbackSettings);
        playbackSettings.startPaused = true;
        rc = NEXUS_Playback_SetSettings(state->playback, &playbackSettings);
        BDBG_ASSERT(!rc);

        /* Start decoders */
        start_audio(state->opts, state->audioDecoder, state->compressedDecoder, state->audioProgram,capture,pFile);


        /* start playback  */
        rc = NEXUS_Playback_Start(state->playback, state->file, state->playbackStartSettings);
        BDBG_ASSERT(!rc);

        /* seek into desired location */
        rc = NEXUS_Playback_Seek(state->playback, status.position);
        BDBG_ASSERT(!rc);

        /* start playing */
        rc = NEXUS_Playback_Play(state->playback);
        BDBG_ASSERT(!rc);
        BKNI_ReleaseMutex(state->lock);

		
    }
    return NULL;
}

static void 
monitor_thread_start(PlaybackMonitorState *state)
{
    int rc;
    BKNI_CreateMutex(&state->lock);
    state->terminate = false;
    rc = pthread_create(&state->thread, NULL, monitor_thread, state);
    BDBG_ASSERT(rc==0);
    return;
}

static void 
monitor_thread_stop(PlaybackMonitorState *state)
{
    state->terminate = true;
    pthread_join(state->thread, NULL);
}
#endif /* B_HAS_PLAYBACK_MONITOR */


/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

int main(int argc, const char *argv[]) {
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle audioPidChannel = NULL, pcrPidChannel;
    NEXUS_AudioDecoderHandle audioDecoder, compressedDecoder=NULL;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file,customFile=NULL, stickyFile=NULL;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_PlaybackStartSettings playbackStartSettings;
	NEXUS_PlaybackStatus pstatus;
    
    NEXUS_Error rc;

    NEXUS_AudioCaptureHandle capture;

	
    const char *pFileName = "audio_capture.dat";
	FILE *pFile;

    struct util_opts_t opts;
	bool exit1;
    int loopback;

/*-------------------------------------------------------------Initialization---------------------------------------------------------------------------------------------------------------------- */   
    loopback = wma_cmdline_parse(argc, argv, &opts);
        
   /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

	 pFile = fopen(pFileName, "wb+");
        if ( NULL == pFile )
		{
			fprintf(stderr, "Unable to open file '%s' for writing captured data\n", pFileName);
			return 0;
		}
	if(loopback == 1) {
		opts.endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
	}
	else{
		opts.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay;
	}

    if (cmdline_probe(&opts)) {
        return 1;
    }

    if (!opts.filename) {
        printf("\n**********************************************************************************************************************************************************\n");
		printf("\nRun the app by running the following command:\n");
		printf("\n ./\ nexus wma_decode_capture -probe streamname -config configfilename -loopback \n");
		printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");
        printf("\n**********************************************************************************************************************************************************\n");
        return 0;
    }

    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    if ((opts.indexname && !strcmp(opts.indexname, "same")) ||
        opts.transportType == NEXUS_TransportType_eMkv ||
        opts.transportType == NEXUS_TransportType_eMp4
        )
    {
        opts.indexname = opts.filename;
    }

    file = NEXUS_FilePlay_OpenPosix(opts.filename, opts.indexname);
    if (!file) {
        fprintf(stderr, "can't open files:%s %s\n", opts.filename, opts.indexname);
        return -1;
    }
    if(opts.customFileIo) {
        customFile = file = FileIoCustom_Attach(file);
        BDBG_ASSERT(file);
    }
    if(opts.playbackMonitor) {
        stickyFile = file = FileIoSticky_Attach(file);
        BDBG_ASSERT(file);
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.modeSettings.Auto.behavior = opts.stcChannelMaster;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = opts.transportType;
    playbackSettings.playpumpSettings.timestamp.pacing = false;
    playbackSettings.playpumpSettings.timestamp.type = opts.tsTimestampType;

    playbackSettings.stcChannel = stcChannel;
    playbackSettings.stcTrick = opts.stcTrick;
    playbackSettings.beginningOfStreamAction = opts.beginningOfStreamAction;
    playbackSettings.endOfStreamAction = opts.endOfStreamAction;
    playbackSettings.enableStreamProcessing = opts.streamProcessing;
    rc = NEXUS_Playback_SetSettings(playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    
    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    capture = NEXUS_AudioCapture_Open(0, NULL);

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*-----------------------------------------------------------Adding inputs to the corresponding output ports--------------------------------------------------------------------------------------- */
    BDBG_ASSERT(audioDecoder);

    rc = NEXUS_AudioOutput_AddInput(
         NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    
	BDBG_ASSERT(!rc);

	rc =NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(capture),
							   NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    BDBG_ASSERT(!rc);

   
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*-------------------------------------------------------------------------Playback settings--------------------------------------------------------------------------------------------- */
    if (opts.audioCodec != NEXUS_AudioCodec_eUnknown) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
        playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = opts.audioCodec;
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.audioPid, &playbackPidSettings);
    }

    if (opts.pcrPid && opts.pcrPid!=opts.audioPid) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther;
        pcrPidChannel = NEXUS_Playback_OpenPidChannel(playback, opts.pcrPid, &playbackPidSettings);
    }

    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = opts.audioCodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------------------------- Start decoders ------------------------------------------------------------------------------------*/
   start_audio(&opts, audioDecoder, compressedDecoder, &audioProgram,capture,pFile);
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*------------------------------------------------------------------------------------------ Start playback---------------------------------------------------------------------------------------- */
    NEXUS_Playback_GetDefaultStartSettings(&playbackStartSettings);
    if(opts.autoBitrate) {
        playbackStartSettings.mode = NEXUS_PlaybackMode_eAutoBitrate;
    }
    rc = NEXUS_Playback_Start(playback, file, &playbackStartSettings);
    BDBG_ASSERT(!rc);
#if B_HAS_PLAYBACK_MONITOR
    {
        PlaybackMonitorState monitorState;
        BKNI_Memset(&monitorState, 0, sizeof(monitorState));
        monitorState.opts = &opts;
        monitorState.playback = playback;
        monitorState.audioDecoder = audioDecoder;
        monitorState.compressedDecoder = compressedDecoder;
        monitorState.audioProgram = &audioProgram;
        monitorState.file = file;
        monitorState.stickyFile = stickyFile;
        monitorState.customFile = customFile;
        monitorState.playbackStartSettings = &playbackStartSettings;
		
        if(stickyFile) {
           monitor_thread_start(&monitorState);
        }
#endif
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*------------------------------------------------Run time modifications in playback if loopback is enabled-------------------------------------------------------------------------- */
if(loopback==1) {
		  for(exit1=false;!exit1;) {
	
			char buffer[256];
				char *buf;
                
            #if B_HAS_PLAYBACK_MONITOR
					if(stickyFile) {
						BKNI_AcquireMutex(monitorState.lock);
					}
			#endif
					do {

					
					
							printf("wma_decode>"); fflush(stdout);
	
							fgets(buffer, 256, stdin);
                            
                           if(feof(stdin)) {
								break;
							}
							
							buffer[strlen(buffer)-1] = 0;
					
							buf = strtok(buffer, ";");
							if (!buf) continue;
		
						if (!strcmp(buf, "?")) {
							printf("Current set values are:\n");
                            printf("\nRun the app by running the following command:\n");
							printf("\n**********************************************************************************************************************************************************\n");
							printf("\n ./\ nexus wma_decode_capture -probe streamname -loopback \n");
                            printf("\n\n To change the value of any config parameters, type the command as 'CONFIG PARAMETER NAME= VALUE' \n Type 'quit' to terminate the application. \n\n\n");
							printf("\n**********************************************************************************************************************************************************\n");
			
			
						}
						
						else if (!strcmp(buf, "quit")) {
							exit1 = true;
							break;
						}
						
						 
	
			   else if (!*buf) {
					/* allow blank line */
				}
				else {
					printf("unknown command: %s\n", buf);
				}
			}
			while ((buf = strtok(NULL, ";"))&&(pstatus.state!=2));
	#if B_HAS_PLAYBACK_MONITOR
			if(stickyFile) {
				BKNI_ReleaseMutex(monitorState.lock);
			}
	#endif
		}
 }
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
/*------------------------------------------------------------------------Program stops at the end of the playback if loopback is not enabled------------------------------------------------------ */
  else{
	 #if B_HAS_PLAYBACK_MONITOR
					if(stickyFile) {
						BKNI_AcquireMutex(monitorState.lock);
					}
     #endif

	rc = NEXUS_Playback_GetStatus(playback, &pstatus);
	BDBG_ASSERT(!rc);
	while(pstatus.state!=2) {
		sleep(1);
		rc = NEXUS_Playback_GetStatus(playback, &pstatus);
		BDBG_ASSERT(!rc);
     }
    
    #if B_HAS_PLAYBACK_MONITOR
			if(stickyFile) {
				BKNI_ReleaseMutex(monitorState.lock);
			}
	#endif

  }
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */

/*------------------------------------------------------------------Stopping, Closing and Removing the used resources at the end of the run-------------------------------------------------------- */
#if B_HAS_PLAYBACK_MONITOR
    if(stickyFile) {
        monitor_thread_stop(&monitorState);
    }
    }
#endif
	NEXUS_Playback_Stop(playback);
   
    stop_audio(&opts, audioDecoder, compressedDecoder,capture);

    NEXUS_Playback_CloseAllPidChannels(playback);
    NEXUS_FilePlay_Close(file);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);


       NEXUS_AudioOutput_RemoveInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

		NEXUS_AudioOutput_RemoveInput(
		NEXUS_AudioCapture_GetConnector(capture),
							   NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    if(compressedDecoder) {
        NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
        NEXUS_AudioDecoder_Close(compressedDecoder);
    }
    NEXUS_Platform_Uninit(); 
#endif
/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
    return 0;
}

