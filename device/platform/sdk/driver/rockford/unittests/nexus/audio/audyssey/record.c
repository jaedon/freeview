/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: record.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/18/11 2:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/audio/audyssey/record.c $
 * 
 * Hydra_Software_Devel/2   10/18/11 2:20p gmullen
 * SW7425-1383: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1383/1   10/14/11 4:12p gmullen
 * SW7425-1383: Updated allPass usages to use new API
 * 
 * Hydra_Software_Devel/1   6/22/10 1:52p nvu
 * SW3548-2991: submit Audyssey certification application.
 * 
 * 6   5/12/10 11:26a erickson
 * SW7550-159: fix warning
 *
 * 5   1/20/10 5:08p erickson
 * SW7550-159: update record util for threshold and buffer control
 *
 * 4   12/8/09 3:00p gmohile
 * SW7408-1 : Add support for platforms with no frontend
 *
 * 3   10/8/09 10:43a jtna
 * SWDEPRECATED-3793: more prints and examples
 *
 * 2   6/18/09 4:30p jtna
 * PR54802: add frontend support to record
 *
 * 1   6/16/09 5:09p jtna
 * PR54802: initial version
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_surface.h"
#include "nexus_core_utils.h"
#if NEXUS_HAS_FRONTEND
#include "nexus_frontend.h"
#endif
#include "nexus_record.h"
#include <sys/types.h>
#include <sys/stat.h>

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys/time.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "cmdline_args.h"

#define ALLPASS_PLAYMUMP_INDEX 1

typedef enum RecordSource {
    RecordSource_eStreamer,
    RecordSource_ePlayback,
    RecordSource_eVsb,
    RecordSource_eQam,
    RecordSource_eSat
} RecordSource;

static int64_t get_filesize(const char* fname)
{
    struct stat buf;
    if (stat(fname, &buf)) {
        return -1;
    }
    else {
        return (int64_t)buf.st_size;
    }
}

struct timeval startTime;
void get_recordtime(char* timestamp, int size_t)
{
    struct timeval currentTime;
    int hours, minutes, seconds;
    int rc;

    rc = gettimeofday(&currentTime, NULL);
    if (rc!=0) {
        timestamp = "00:00:00";
        return;
    }

    hours = (currentTime.tv_sec - startTime.tv_sec)/3600;
    minutes = (((currentTime.tv_sec - startTime.tv_sec)/60))%60;
    seconds = (currentTime.tv_sec - startTime.tv_sec)%60;

    rc = BKNI_Snprintf(timestamp, size_t, "%02u:%02u:%02u", hours, minutes, seconds);
    return;
}

#if NEXUS_HAS_FRONTEND
BKNI_EventHandle lockEvent;
static void lock_callback(void *context, int param)
{
#if 0 /* TODO: for now, assume frontend locked upon entrance to this function */
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    RecordSource source = (int)param;
    bool locked = false;

    if (source==RecordSource_eVsb) {
        NEXUS_FrontendVsbStatus status;
        NEXUS_Frontend_GetVsbStatus(frontend, &status);
        locked = status.receiverLock;
    }
    else if (source==RecordSource_eQam) {
        NEXUS_FrontendQamStatus status;
        NEXUS_Frontend_GetQamStatus(frontend, &status);
        locked = status.receiverLock;
    }
    else if (source==RecordSource_eSat) {
        NEXUS_FrontendSatelliteStatus status;
        NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
        locked = status.demodLocked;
    }

    if (locked) {
        BKNI_SetEvent(lockEvent);
    }
#else
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    BKNI_SetEvent(lockEvent);
#endif
    return;
}
#endif

int main(int argc, const char *argv[])
{
#if NEXUS_HAS_RECORD
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
#if NEXUS_HAS_FRONTEND
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand0, parserBand1;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidSettings;
#endif
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle pcrPidChannel, videoPidChannel, audioPidChannel, allpassPidChannel, otherPidChannels[MAX_RECORD_PIDS];
    NEXUS_PlaypumpHandle playpump0, playpump1;
    NEXUS_PlaybackHandle playback0, playback1;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_FilePlayHandle playfile;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecpumpOpenSettings recpumpOpenSettings;
    NEXUS_RecordHandle record;
    NEXUS_RecordSettings recordSettings;
    NEXUS_RecordPidChannelSettings recordPidSettings;
    NEXUS_FileRecordHandle recfile;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendHandle frontend = NULL;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendVsbSettings vsbSettings;
    NEXUS_FrontendQamSettings qamSettings;
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendDiseqcSettings diseqcSettings;
#endif
    struct util_opts_record_t opts;
    RecordSource source;
    unsigned i,j;
    bool exit;
    NEXUS_Error rc;

    BKNI_Memset(otherPidChannels, 0, sizeof(otherPidChannels));

    if (cmdline_parse_record(argc, argv, &opts)) {
        return 0;
    }

    /* determine source */
    if (opts.streamer) {
        source = RecordSource_eStreamer;
    }
#if NEXUS_HAS_FRONTEND
    else if (opts.vsbMode!=NEXUS_FrontendVsbMode_eMax) {
        source = RecordSource_eVsb;
    }
    else if (opts.qamMode!=NEXUS_FrontendQamMode_eMax) {
        source = RecordSource_eQam;
    }
    else if (opts.satMode!=NEXUS_FrontendSatelliteMode_eMax) {
        source = RecordSource_eSat;
    }
#endif
    else if (opts.playfname) {
        source = RecordSource_ePlayback;
    }
    else {
        source = RecordSource_eStreamer;
    }

    /* bring up platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    if (source==RecordSource_eStreamer || source==RecordSource_ePlayback) {
        platformSettings.openFrontend = false;
    }
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_HAS_FRONTEND
    /* check frontend capabilities */
    if (platformSettings.openFrontend) {
        for (i=0; i<NEXUS_MAX_FRONTENDS; i++) {
            NEXUS_FrontendCapabilities capabilities;
            frontend = platformConfig.frontend[i];
            if (frontend) {
                NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
                if (source==RecordSource_eVsb && capabilities.vsb) { break; }
                else if (source==RecordSource_eQam && capabilities.qam) { break; }
                else if (source==RecordSource_eSat && capabilities.satellite) { break; }
                else {
                    frontend = NULL;
                }
            }
        }

        if (!frontend) {
            printf("> Unable to find capable frontend\n");
            return 1;
        }
    }

    BKNI_CreateEvent(&lockEvent);
#endif


    /* media probe */
    if (source==RecordSource_ePlayback && opts.probe) {
        opts.opts.probe = opts.probe;
        opts.opts.filename = opts.playfname;
        if (cmdline_probe(&opts.opts)) {
            return 1;
        }
        if (opts.probe) {
            opts.videoPid = opts.opts.videoPid;
            opts.audioPid = opts.opts.audioPid;
            opts.videoCodec = opts.opts.videoCodec;
            opts.audioCodec = opts.opts.audioCodec;
            opts.transportType = opts.opts.transportType;
        }
    }
    printf("\n> data_file: %s, index_file %s\n", opts.recfname, opts.recidxname);

    /* check options */
    if (opts.allpass && opts.pids[0]) {
        printf("> Allpass record specified with -pids list. -pids will be ignored.\n");
        /* continue */
    }
    if (opts.recidxname && (!opts.videoPid || !opts.videoCodec)) {
        printf("> For record with index, you must explicitly specify a video PID and video_type.\n");
        return 1;
    }
    if (source!=RecordSource_eStreamer && source!=RecordSource_ePlayback
#if NEXUS_HAS_FRONTEND
	&& opts.freq==0
#endif
	) {
        printf("> For frontend source, you must specify a frequency\n");
        return 1;
    }
    if ((!opts.recfname) ||
        (!opts.allpass && !opts.videoPid && !opts.audioPid && !opts.pids[0])) {
        printf("> Missing destination file or pids; See usage.\n\n");
        print_usage_record(argv[0]);
        return 1;
    }

    /* we have everything we need. start setting stuff up */
#if NEXUS_HAS_FRONTEND
    if (source!=RecordSource_ePlayback) {
        /* setup frontend settings */
        switch (source) {
            case RecordSource_eVsb:
                NEXUS_Frontend_GetDefaultVsbSettings(&vsbSettings);
                vsbSettings.frequency = opts.freq * 1000000;
                vsbSettings.mode = opts.vsbMode;
                vsbSettings.lockCallback.callback = lock_callback;
                vsbSettings.lockCallback.context = frontend;
                vsbSettings.lockCallback.param = (int)source;
                break;
            case RecordSource_eQam:
                NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
                qamSettings.frequency = opts.freq * 1000000;
                qamSettings.mode = opts.qamMode;
                qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
                qamSettings.lockCallback.callback = lock_callback;
                qamSettings.lockCallback.context = frontend;
                qamSettings.lockCallback.param = (int)source;
                break;
            case RecordSource_eSat:
                NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
                satSettings.frequency = opts.freq * 1000000;
                satSettings.mode = opts.satMode;
                satSettings.lockCallback.callback = lock_callback;
                satSettings.lockCallback.context = frontend;
                satSettings.lockCallback.param = (int)source;

                if (opts.satMode==NEXUS_FrontendSatelliteMode_eDvb &&
                    opts.transportType!=NEXUS_TransportType_eTs) {
                    printf("> mpeg_type for Sat_DVB was not set to TS\n");
                }
                if (opts.satMode==NEXUS_FrontendSatelliteMode_eDss &&
                    opts.transportType!=NEXUS_TransportType_eDssEs) {
                    printf("> mpeg_type for Sat_DSS was not set to DSS_ES\n");
                }

                NEXUS_Frontend_GetDiseqcSettings(frontend, &diseqcSettings);
                if (opts.satMode==NEXUS_FrontendSatelliteMode_eDvb) {
                    diseqcSettings.toneEnabled = true;
                    diseqcSettings.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
                }
                else if (opts.satMode==NEXUS_FrontendSatelliteMode_eDss) {
                    diseqcSettings.toneEnabled = false;
                    diseqcSettings.voltage = NEXUS_FrontendDiseqcVoltage_e18v;
                }
                NEXUS_Frontend_SetDiseqcSettings(frontend, &diseqcSettings);
                break;
            default:
                BDBG_ASSERT(source==RecordSource_eStreamer);
                break;
        }

        /* map a parser band to the streamer/demod input band.
           pidChannels for the decoders and non-allpass records are opened from PB0 */
        parserBand0 = NEXUS_ParserBand_e0;
        NEXUS_ParserBand_GetSettings(parserBand0, &parserBandSettings);
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        if (source==RecordSource_eStreamer) {
            rc = NEXUS_Platform_GetStreamerInputBand(0, &inputBand);
            BDBG_ASSERT(!rc);
            parserBandSettings.sourceTypeSettings.inputBand = inputBand;
        }
        else {
            rc = NEXUS_Frontend_GetUserParameters(frontend, &userParams);
            BDBG_ASSERT(!rc);
            parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;
        }
        parserBandSettings.transportType = opts.transportType;
        rc = NEXUS_ParserBand_SetSettings(parserBand0, &parserBandSettings);
        BDBG_ASSERT(!rc);

        /* use a separate parserBand for allpass record */
        parserBand1 = NEXUS_ParserBand_e1;
        NEXUS_ParserBand_GetSettings(parserBand1, &parserBandSettings);
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        if (source==RecordSource_eStreamer) {
            parserBandSettings.sourceTypeSettings.inputBand = inputBand;
        }
        else {
            parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;
        }
        parserBandSettings.transportType = opts.transportType;
        parserBandSettings.allPass = true;
        parserBandSettings.acceptNullPackets = opts.acceptNullPackets;
        rc = NEXUS_ParserBand_SetSettings(parserBand1, &parserBandSettings);
        BDBG_ASSERT(!rc);
    }
    else
#endif
      if (source==RecordSource_ePlayback) {
        /* pidChannels for the decoders and non-allpass records are opened from playpump0 */
        playpump0 = NEXUS_Playpump_Open(0, NULL);
        BDBG_ASSERT(playpump0);
        /* use a separate playpump for allpass record */
        playpump1 = NEXUS_Playpump_Open(ALLPASS_PLAYMUMP_INDEX, NULL);
        BDBG_ASSERT(playpump1);
        playback0 = NEXUS_Playback_Create();
        BDBG_ASSERT(playback0);
        playback1 = NEXUS_Playback_Create();
        BDBG_ASSERT(playback1);

        playfile = NEXUS_FilePlay_OpenPosix(opts.playfname, NULL);
        if (!playfile) {
            printf("Can't open file: %s\n", opts.playfname);
            return -1;
        }
      }

#if NEXUS_HAS_FRONTEND
    if (source!=RecordSource_ePlayback) {
        if (opts.allpass) {
            /* opening the allpass pidChannel first with a specific index avoids conflicts later */
            NEXUS_PidChannel_GetDefaultSettings(&pidSettings);
            NEXUS_ParserBand_GetAllPassPidChannelIndex(parserBand1, (unsigned *) &pidSettings.pidChannelIndex);
            allpassPidChannel = NEXUS_PidChannel_Open(parserBand1, 0x0, &pidSettings); /* pid is redundant */
        }

        /* open the audio and video pid channels. open even if pids are null; check is performed before adding to record */
        videoPidChannel = NEXUS_PidChannel_Open(parserBand0, opts.videoPid, NULL);
        audioPidChannel = NEXUS_PidChannel_Open(parserBand0, opts.audioPid, NULL);
        /* open pid channels from list. don't add if duplicate of video or audio pid channel */
        for (i=0,j=0; opts.pids[i] && i<MAX_RECORD_PIDS; i++) {
            if (opts.pids[i]!=opts.videoPid && opts.pids[i]!=opts.audioPid) {
                otherPidChannels[j++] = NEXUS_PidChannel_Open(parserBand0, opts.pids[i], NULL);
            }
            else {
                printf("> Removing duplicate pid %#x from -pids list\n", opts.pids[i]);
            }
        }

        pcrPidChannel = NEXUS_PidChannel_Open(parserBand0, opts.pcrPid?opts.pcrPid:opts.videoPid, NULL);
    } /* for playback, pidChannels are opened after decoders are opened */
#endif

    /* open the stcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    if (source!=RecordSource_ePlayback) {
        stcSettings.mode = NEXUS_StcChannelMode_ePcr;
        stcSettings.modeSettings.pcr.pidChannel = pcrPidChannel;
    }
    else {
        stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    }
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    if (source==RecordSource_ePlayback) {
        /* set playback settings */
        NEXUS_Playback_GetSettings(playback0, &playbackSettings);
        playbackSettings.playpump = playpump0;
        playbackSettings.playpumpSettings.transportType = opts.transportType;
        playbackSettings.stcChannel = stcChannel;
        NEXUS_Playback_SetSettings(playback0, &playbackSettings);

        NEXUS_Playback_GetSettings(playback1, &playbackSettings);
        playbackSettings.playpump = playpump1;
        playbackSettings.playpumpSettings.transportType = opts.transportType;
        playbackSettings.stcChannel = stcChannel;
        playbackSettings.playpumpSettings.allPass = true;
        playbackSettings.playpumpSettings.acceptNullPackets = opts.acceptNullPackets;
        NEXUS_Playback_SetSettings(playback1, &playbackSettings);
    }

    /* bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    rc = NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    BDBG_ASSERT(!rc);

    /* bring up display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = opts.displayType;
    displaySettings.format = opts.displayFormat;
    display = NEXUS_Display_Open(0, &displaySettings);
    if (opts.useCompositeOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
        BDBG_ASSERT(!rc);
    }
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if (opts.useComponentOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
        BDBG_ASSERT(!rc);
    }
#endif
#if NEXUS_NUM_PANEL_OUTPUTS
    if (opts.usePanelOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
        BDBG_ASSERT(!rc);
        NEXUS_BoardCfg_ConfigurePanel(true, true, true);
        printf("> Panel on\n");
    }
#endif
#if NEXUS_NUM_SCART_INPUTS
    if (opts.useScart1CompositeOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
        BDBG_ASSERT(!rc);
        printf("> Turning on: scart 1 composite output\n");
    }
#endif

    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    windowSettings.contentMode = opts.contentMode;
    rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
    BDBG_ASSERT(!rc);

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    BDBG_ASSERT(!rc);

    /* open playback pidChannels */
    if (source==RecordSource_ePlayback) {
        if (opts.allpass) {
            NEXUS_PlaybackSettings playbackSettings;

            NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
            NEXUS_Playback_GetSettings(playback1, &playbackSettings);
            NEXUS_Playpump_GetAllPassPidChannelIndex(playbackSettings.playpump, (unsigned *) &playbackPidSettings.pidSettings.pidSettings.pidChannelIndex );
            allpassPidChannel = NEXUS_Playback_OpenPidChannel(playback1, 0x0, &playbackPidSettings); /* pidNo is redundant */
        }

        /* open the audio and video pid channels. open even if pids are null; check is performed before adding to record */
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
        playbackPidSettings.pidTypeSettings.video.codec = opts.videoCodec;
        playbackPidSettings.pidTypeSettings.video.index = true;
        videoPidChannel = NEXUS_Playback_OpenPidChannel(playback0, opts.videoPid, &playbackPidSettings);

        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
        playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = opts.audioCodec;
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback0, opts.audioPid, &playbackPidSettings);

        /* open pid channels from list. don't add if duplicate of video or audio pid channel */
        for (i=0,j=0; opts.pids[i] && i<MAX_RECORD_PIDS; i++) {
            if (opts.pids[i]!=opts.videoPid && opts.pids[i]!=opts.audioPid) {
                otherPidChannels[j++] = NEXUS_Playback_OpenPidChannel(playback0, opts.pids[i], NULL);
            }
            else {
                printf("> Removing duplicate pid %#x from -pids list\n", opts.pids[i]);
            }
        }
    }

    /* setup record/recpump */
    NEXUS_Recpump_GetDefaultOpenSettings(&recpumpOpenSettings);
    if (opts.data.bufferSize) {
        recpumpOpenSettings.data.bufferSize = opts.data.bufferSize;
    }
    if (opts.data.dataReadyThreshold) {
        recpumpOpenSettings.data.dataReadyThreshold = opts.data.dataReadyThreshold;
        recpumpOpenSettings.data.dataReadyThreshold = 0;
    }
    if (opts.index.bufferSize) {
        recpumpOpenSettings.index.bufferSize = opts.index.bufferSize;
    }
    if (opts.index.dataReadyThreshold) {
        recpumpOpenSettings.index.dataReadyThreshold = opts.index.dataReadyThreshold;
    }
    recpump = NEXUS_Recpump_Open(0, &recpumpOpenSettings);
    record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(record, &recordSettings);
    recordSettings.recpump = recpump;
    NEXUS_Record_SetSettings(record, &recordSettings);
    recfile = NEXUS_FileRecord_OpenPosix(opts.recfname, opts.recidxname);
    BDBG_ASSERT(recfile);

    /* add the pidChannels to record */
    if (!opts.allpass) {
        if (opts.videoPid) {
            /* configure the video pid for indexing */
            NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
            recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
            recordPidSettings.recpumpSettings.pidTypeSettings.video.index = true;
            recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = opts.videoCodec;
            NEXUS_Record_AddPidChannel(record, videoPidChannel, opts.recidxname?(&recordPidSettings):NULL);
        }
        if (opts.audioPid) {
            /* the audio pid requires no special configuration */
            NEXUS_Record_AddPidChannel(record, audioPidChannel, NULL);
        }

        for (i=0; otherPidChannels[i]; i++) { /* add other unindexed pids */
            NEXUS_Record_AddPidChannel(record, otherPidChannels[i], NULL);
        }
    }
    else {
        NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
        recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
        recordPidSettings.recpumpSettings.pidTypeSettings.video.index = true;
        recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = opts.videoCodec;
        /* for allpass record, perform indexing using pid value, not pid channel */
        recordPidSettings.recpumpSettings.pidTypeSettings.video.pid = opts.videoPid;
        NEXUS_Record_AddPidChannel(record, allpassPidChannel,
            (opts.recidxname&&opts.videoPid)?(&recordPidSettings):NULL);
    }

    /* tune frontend */
#if NEXUS_HAS_FRONTEND
    if (platformSettings.openFrontend) {
        switch (source) {
            case RecordSource_eVsb:
                NEXUS_Frontend_TuneVsb(frontend, &vsbSettings);
                break;
            case RecordSource_eQam:
                NEXUS_Frontend_TuneQam(frontend, &qamSettings);
                break;
            case RecordSource_eSat:
            default:
                NEXUS_Frontend_TuneSatellite(frontend, &satSettings);
                break;
        }

        printf("> Waiting for frontend lock...");
        fflush(stdout);
        if (BKNI_WaitForEvent(lockEvent, 5000)) {
            printf(" Failed\n");
            return 1;
        }
        printf(" Locked\n");
    }
#endif

    /* setup decoder start structures */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = opts.videoCodec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = opts.audioCodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* start decoders */
    if (opts.videoPid && opts.decode) {
        rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
        BDBG_ASSERT(!rc);
    }
    if (opts.audioPid && opts.decode) {
        rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
        BDBG_ASSERT(!rc);
    }

    /* start playback */
    if (source==RecordSource_ePlayback) {
        if (!opts.allpass ||
            (opts.decode && (opts.videoPid || opts.audioPid))) { /* for allpass record, start playback0 only if decode is requested */
            rc = NEXUS_Playback_Start(playback0, playfile, NULL);
            BDBG_ASSERT(!rc);
        }
        if (opts.allpass) {
            rc = NEXUS_Playback_Start(playback1, playfile, NULL);
            BDBG_ASSERT(!rc);
        }
    }

    gettimeofday(&startTime, NULL);
    rc = NEXUS_Record_Start(record, recfile);
    BDBG_ASSERT(!rc);
    /* Nexus is now recording to disk */
    printf("> Record started\n");

    for(exit=false;!exit;) {
        char buffer[256];
        char *buf;
        printf("record>"); fflush(stdout);
        fgets(buffer, 256, stdin);
        buffer[strlen(buffer)-1] = 0;

        buf = strtok(buffer, ";");
        if (!buf) continue;
        do {
            if (!strcmp(buf, "?")) {
                printf(
                "Commands:\n"
                "  q  - stop record and quit\n"
                "  st - print status\n"
                );
            }
            else if (!strcmp(buf, "q")) {
                exit = true;
                break;
            }
            else if (!strcmp(buf, "st")) {
                NEXUS_RecordStatus rstatus;
                char timestamp[20];
                get_recordtime(timestamp, sizeof(timestamp));
                rc = NEXUS_Record_GetStatus(record, &rstatus);
                BDBG_ASSERT(!rc);
                printf("(%s) data: %uKB, index: %uKB, lastTimestamp: %ums\n",
                    timestamp,
                    (unsigned)rstatus.recpumpStatus.data.bytesRecorded/1024,
                    (unsigned)rstatus.recpumpStatus.index.bytesRecorded/1024,
                    (unsigned)rstatus.lastTimestamp /* in ms */);
            }
            else if (!*buf) {
                /* allow blank line */
            }
            else {
                printf("unknown command: %s\n", buf);
            }
        }
        while ((buf = strtok(NULL, ";")));
    }
#endif

    NEXUS_Record_Stop(record);

    printf("> Recorded %u KB\n", (unsigned)get_filesize(opts.recfname)/1024);

    /* shutdown */
#if NEXUS_HAS_FRONTEND
    BKNI_DestroyEvent(lockEvent);
#endif
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_Record_RemoveAllPidChannels(record);

    if (source==RecordSource_ePlayback) {
        NEXUS_Playback_Stop(playback0);
        NEXUS_Playback_Stop(playback1);
    }
    NEXUS_StcChannel_Close(stcChannel);
#if NEXUS_HAS_FRONTEND
    if (source!=RecordSource_ePlayback) {
        NEXUS_PidChannel_CloseAll(parserBand0);
        if (opts.allpass) {
            NEXUS_PidChannel_Close(allpassPidChannel);
        }
    }
    else
#endif
    {
        NEXUS_Playback_ClosePidChannel(playback0, videoPidChannel);
        NEXUS_Playback_ClosePidChannel(playback0, audioPidChannel);
        if (opts.allpass) {
            NEXUS_Playback_ClosePidChannel(playback1, allpassPidChannel);
        }

        NEXUS_FilePlay_Close(playfile);
        NEXUS_Playback_Destroy(playback0);
        NEXUS_Playpump_Close(playpump0);
        NEXUS_Playback_Destroy(playback1);
        NEXUS_Playpump_Close(playpump1);
    }
    NEXUS_FileRecord_Close(recfile);
    NEXUS_Record_Destroy(record);
    NEXUS_Recpump_Close(recpump);

    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_Platform_Uninit();

    return 0;
}



/*************************************************

examples / test cases

#basic record from streamer input with decode
nexus record -video 0x21 -audio 0x24 -audio_type ac3 videos/test.mpg
    #with index
nexus record -video 0x21 -audio 0x24 -video_type mpeg2 -audio_type ac3 videos/test.mpg videos/test.nav
    #without decode
nexus record -decode off -video 0x21 -audio 0x24 -video_type mpeg2 videos/test.mpg videos/test.nav


#video-only record with index
nexus record -video 0x21 -video_type mpeg2 videos/test.mpg videos/test.nav
#audio-only record
nexus record -audio 0x24 videos/test.mpg


#record allpass from streamer input
nexus record -allpass videos/test.mpg
    #with index (0x21) and decode (0x21,0x24)
nexus record -allpass -video 0x21 -audio 0x24 -video_type mpeg2 -audio_type ac3 videos/test.mpg videos/test.nav


#record allpass from playback input
nexus record -allpass -playfile videos/test.mpg videos/test2.mpg
    #with index (0x21) and decode (0x21,0x24); the decode while record is of no use here
nexus record -allpass -playfile videos/test.mpg -video 0x21 -audio 0x24 -video_type mpeg2 -audio_type ac3 videos/test2.mpg videos/test2.nav


#record specified pids while indexing video pid
nexus record -video 0x11 -video_type mpeg2 -pids 0x14,0x21,0x24,0x41,0x44 videos/test.mpg

#allpass record from QAM
nexus record -qam 256 -freq 573 -allpass videos/qam_allpass.mpg

*************************************************
*/
