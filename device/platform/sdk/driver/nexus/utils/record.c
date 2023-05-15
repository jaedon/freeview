/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Revision: 26 $
 * $brcm_Date: 7/30/12 3:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/utils/record.c $
 * 
 * 26   7/30/12 3:44p erickson
 * SW7425-2130: add #if !BDBG_NO_WRN
 * 
 * 25   2/15/12 2:34p vsilyaev
 * SW7425-2266: Updated call to NEXUS_VideoEncoder_Stop
 * 
 * 24   1/31/12 5:07p rjlewis
 * SW7425-1136: Added HDMI support (and some cleanup).
 * 
 * 23   10/18/11 4:33p gmullen
 * SW7425-1383: Merged to main
 * 
 * SW7425-1383/3   10/18/11 4:26p gmullen
 * SW7425-1383: Use Nexus API to get allPass PID channel number
 * 
 * SW7425-1383/2   10/11/11 3:31p gmullen
 * SW7425-1383: Removed test code
 * 
 * SW7425-1383/1   10/11/11 2:35p gmullen
 * SW7425-1383: Changed allPass PID channel assignment
 * 
 * 22   8/18/11 5:23p vsilyaev
 * SW7425-651: Removed deprecated control over the encoder delay
 * 
 * 21   4/22/11 5:46p vsilyaev
 * SW7422-14: Added code to set depth of the video decoder CDB
 * 
 * 20   4/8/11 6:54p vsilyaev
 * SW7422-14: Added support for recording SVC/MVC content
 * 
 * 19   3/28/11 3:36p vsilyaev
 * SW7425-159:  Fixed shutdown sequence
 * 
 * 18   3/23/11 5:41p vsilyaev
 * SW7422-406: Unified parsing of command line parameters
 * 
 * 17   3/11/11 4:08p vsilyaev
 * SW7425-159: Use BDBG_WRN to get nice timestamps
 * 
 * 16   3/11/11 3:47p vsilyaev
 * SW7425-159: add encode profile/level etc settings; add clean shutdown
 *  sequence for transcoder; add sample usage for transcoder;
 * 
 * SW7425-159/1   3/9/11 8:05p hongtaoz
 * SW7425-159: add encode profile/level etc settings; add clean shutdown
 *  sequence for transcoder; add sample usage for transcoder;
 * 
 * 15   3/7/11 11:08a erickson
 * SW7425-40: allow 7425 compilation w/o hdmi_input
 *
 * 14   3/4/11 11:43a vsilyaev
 * SW7425-137: Removed min and target bitrates
 *
 * SW7425_bringup/3   3/4/11 1:27a hongtaoz
 * SW7425-137: removed minimum and target bit rate;
 *
 * 13   2/22/11 3:44p vsilyaev
 * SW7425-40: adapted to new API;
 *
 * SW7425_bringup/2   2/15/11 9:52p hongtaoz
 * SW7425-40: adapted to new API;
 *
 * 12   12/30/10 1:50p vsilyaev
 * SW7425-39: Updated to match StreamMux API
 *
 * 11   12/30/10 12:43p vsilyaev
 * SW7425-39: Updated to match StreamMux API
 *
 * 10   12/22/10 1:16p vsilyaev
 * SW7425-39, SW7425-40: merged hdmi_input source and transcoder util
 *  test;
 *
 * SW7425_bringup/1   12/22/10 12:42a hongtaoz
 * SW7425-39, SW7425-40: add hdmi_input source and transcoder util test;
 *
 * 9   9/8/10 12:06p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * 8   8/10/10 12:15p erickson
 * SW7405-4735: merge
 *
 * SW7405-4735/2   8/9/10 4:31p jtna
 * SW7405-4735: change printf back to BDBG_WRN
 *
 * SW7405-4735/1   8/9/10 3:22p jtna
 * SW7405-4735: rename opts.pids to opts.otherPids. allow pid 0 in
 *  opts.otherPids. start only one playback during allpass record.
 *
 * 7   8/9/10 6:34a erickson
 * SW3548-3042: don't use NEXUS_HAS_FRONTEND to eliminate streamer. if
 *  stremer is not available, another #define should be used. cleaned up
 *  decoder stop. don't configure all pass parser band unless actually in
 *  use. convert errors to BDBG_ERR.
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
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_surface.h"
#include "nexus_core_utils.h"
#if NEXUS_HAS_FRONTEND
#include "nexus_frontend.h"
#endif
#if NEXUS_HAS_HDMI_INPUT
#include "nexus_hdmi_input.h"
#endif
#if NEXUS_HAS_VIDEO_ENCODER
#include "nexus_stream_mux.h"
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

#include "cmdline_args.h"

BDBG_MODULE(record);

#if !NEXUS_HAS_RECORD
int main(void)
{
    printf("This application is not supported on this platform (needs Record)!\n");
    return 0;
}
#else

#define ALLPASS_PLAYMUMP_INDEX 1
#define VIDEO_MUX_PLAYMUMP_INDEX 2
#define AUDIO_MUX_PLAYMUMP_INDEX 3
#define SYSTEM_MUX_PLAYMUMP_INDEX 4

typedef enum RecordSource {
    RecordSource_eStreamer,
    RecordSource_ePlayback,
    RecordSource_eVsb,
    RecordSource_eQam,
    RecordSource_eSat,
    RecordSource_eHdmiInput
} RecordSource;

#if !BDBG_NO_WRN
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
#endif

struct timeval startTime;
static void get_recordtime(char* timestamp, int size_t)
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

#if NEXUS_HAS_STREAM_MUX
static void 
transcoderFinishCallback(void *context, int param)
{
    BKNI_EventHandle finishEvent = (BKNI_EventHandle)context;

    BSTD_UNUSED(param);
    BDBG_WRN(("Finish callback invoked, now stop the stream mux."));
    BKNI_SetEvent(finishEvent);
}
#endif

int main(int argc, const char *argv[])
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand0, parserBand1;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle pcrPidChannel, videoPidChannel, audioPidChannel, allpassPidChannel, otherPidChannels[MAX_RECORD_PIDS], videoExtPidChannel=NULL;
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
#if NEXUS_HAS_HDMI_INPUT
    NEXUS_HdmiInputHandle hdmiInput;
    NEXUS_HdmiInputSettings hdmiInputSettings;
#endif
#if NEXUS_HAS_VIDEO_ENCODER
    NEXUS_DisplayHandle displayTranscode;
    NEXUS_VideoWindowHandle windowTranscode;
    NEXUS_VideoEncoderHandle videoEncoder;
    NEXUS_VideoEncoderSettings videoEncoderConfig;
    NEXUS_VideoEncoderStartSettings videoEncoderStartConfig;
    NEXUS_StcChannelHandle stcChannelTranscode;
    NEXUS_StreamMuxHandle streamMux;
    NEXUS_StreamMuxCreateSettings muxCreateSettings;
    BKNI_EventHandle finishEvent;
    NEXUS_StreamMuxStartSettings muxConfig;
    NEXUS_PlaypumpOpenSettings playpumpConfig;
    NEXUS_PlaypumpHandle playpumpTranscodeVideo;
    NEXUS_PlaypumpHandle playpumpTranscodePcr;
    NEXUS_PidChannelHandle pidChannelTranscodeVideo;
    NEXUS_PidChannelHandle pidChannelTranscodePcr;
#endif
    struct util_opts_record_t opts;
    RecordSource source;
    unsigned i,j;
    bool exit;
    NEXUS_Error rc;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;

    BKNI_Memset(otherPidChannels, 0, sizeof(otherPidChannels));
    videoPidChannel = NULL;
    audioPidChannel = NULL;

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
#if NEXUS_HAS_HDMI_INPUT
    else if (opts.hdmiInput) {
        source = RecordSource_eHdmiInput;
    }
#endif
    else if (opts.playfname) {
        source = RecordSource_ePlayback;
        BDBG_MSG(("playback %s", opts.playfname));
    }
    else {
        source = RecordSource_eStreamer;
    }

    /* bring up platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    if (source==RecordSource_eStreamer || source==RecordSource_ePlayback || source==RecordSource_eHdmiInput) {
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
            BDBG_ERR(("Unable to find capable frontend"));
            return 1;
        }
    }

    BKNI_CreateEvent(&lockEvent);
#endif

    /* setup decoder start structures */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);

    /* media probe */
    if (source==RecordSource_ePlayback && opts.common.probe && !opts.allpass) {
        if (cmdline_probe(&opts.common, opts.playfname, &opts.recidxname)) {
            return 1;
        }
    }
    BDBG_WRN(("data_file: %s, index_file %s", opts.recfname, opts.recidxname));

    /* check options */
    if (opts.allpass && opts.numOtherPids) {
        BDBG_WRN(("Allpass record specified with -pids list. -pids will be ignored."));
        /* continue */
    }
    if (opts.recidxname && (!opts.common.videoPid || !opts.common.videoCodec)) {
        BDBG_ERR(("For record with index, you must explicitly specify a video PID and video_type."));
        return 1;
    }
    if (source!=RecordSource_eStreamer && source!=RecordSource_ePlayback && source!=RecordSource_eHdmiInput
#if NEXUS_HAS_FRONTEND
    && opts.freq==0
#endif
    ) {
        BDBG_ERR(("For frontend source, you must specify a frequency"));
        return 1;
    }
    if ((!opts.recfname) ||
        (!opts.allpass && !opts.common.videoPid && !opts.common.audioPid && !opts.numOtherPids)) {
        BDBG_ERR(("Missing destination file or pids; See usage."));
        print_usage_record(argv[0]);
        return 1;
    }
#if !NEXUS_HAS_VIDEO_ENCODER
    if(source==RecordSource_eHdmiInput) {
        BDBG_ERR(("For hdmi source, you must have video encoder to record"));
        return 1;
    }
#endif
    /* we have everything we need. start setting stuff up */
    if (source==RecordSource_eHdmiInput) {
#if NEXUS_HAS_HDMI_INPUT
        NEXUS_TimebaseSettings timebaseSettings;
        NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eHdDviIn;
        NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);

        NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
        hdmiInputSettings.timebase = NEXUS_Timebase_e0;
        hdmiInput = NEXUS_HdmiInput_Open(0, &hdmiInputSettings);
        if(!hdmiInput) {
            BDBG_ERR(("Can't get hdmi input"));
            return -1;
        }
#endif
    }
    else if (source!=RecordSource_ePlayback) {
        /* setup frontend settings */
        switch (source) {
#if NEXUS_HAS_FRONTEND
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
                    opts.common.transportType!=NEXUS_TransportType_eTs) {
                    BDBG_WRN(("mpeg_type for Sat_DVB was not set to TS"));
                }
                if (opts.satMode==NEXUS_FrontendSatelliteMode_eDss &&
                    opts.common.transportType!=NEXUS_TransportType_eDssEs) {
                    BDBG_WRN(("mpeg_type for Sat_DSS was not set to DSS_ES"));
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
#endif
            case RecordSource_eStreamer:
                BDBG_ASSERT(source==RecordSource_eStreamer);
                break;
            default:
                BDBG_ERR(("unknown source: %d", source));
                print_usage_record(argv[0]);
                return 1;
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
#if NEXUS_HAS_FRONTEND
        else {
            rc = NEXUS_Frontend_GetUserParameters(frontend, &userParams);
            BDBG_ASSERT(!rc);
            parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;
        }
#endif
        parserBandSettings.transportType = opts.common.transportType;
        rc = NEXUS_ParserBand_SetSettings(parserBand0, &parserBandSettings);
        BDBG_ASSERT(!rc);

        if (opts.allpass) {
            /* use a separate parserBand for allpass record */
            parserBand1 = NEXUS_ParserBand_e1;
            NEXUS_ParserBand_GetSettings(parserBand1, &parserBandSettings);
            parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
            if (source==RecordSource_eStreamer) {
                parserBandSettings.sourceTypeSettings.inputBand = inputBand;
            }
#if NEXUS_HAS_FRONTEND
            else {
                parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;
            }
#endif
            parserBandSettings.transportType = opts.common.transportType;
            parserBandSettings.allPass = true;
            parserBandSettings.acceptNullPackets = opts.acceptNullPackets;
            rc = NEXUS_ParserBand_SetSettings(parserBand1, &parserBandSettings);
            BDBG_ASSERT(!rc);
        }
    }
    else if (source==RecordSource_ePlayback) {
        /* pidChannels for the decoders and non-allpass records are opened from playpump0 */
        playpump0 = NEXUS_Playpump_Open(0, NULL);
        BDBG_ASSERT(playpump0);
        playback0 = NEXUS_Playback_Create();
        BDBG_ASSERT(playback0);
        if (opts.allpass) {
        /* use a separate playpump for allpass record */
        playpump1 = NEXUS_Playpump_Open(ALLPASS_PLAYMUMP_INDEX, NULL);
        BDBG_ASSERT(playpump1);
        playback1 = NEXUS_Playback_Create();
        BDBG_ASSERT(playback1);
        }

        playfile = NEXUS_FilePlay_OpenPosix(opts.playfname, NULL);
        if (!playfile) {
            BDBG_ERR(("Can't open file: %s", opts.playfname));
            return -1;
        }
    }

    if (source!=RecordSource_ePlayback && source!=RecordSource_eHdmiInput) {
        if (opts.allpass) {
            /* opening the allpass pidChannel first with a specific index avoids conflicts later */
            NEXUS_PidChannel_GetDefaultSettings(&pidSettings);
            NEXUS_ParserBand_GetAllPassPidChannelIndex(parserBand1, (unsigned *) &pidSettings.pidChannelIndex);
            allpassPidChannel = NEXUS_PidChannel_Open(parserBand1, 0x0, &pidSettings); /* pid is redundant */
        }

        /* open the audio and video pid channels */
        if (opts.common.videoPid) {
            videoPidChannel = NEXUS_PidChannel_Open(parserBand0, opts.common.videoPid, NULL);
        }
        if (opts.common.audioPid) {
            audioPidChannel = NEXUS_PidChannel_Open(parserBand0, opts.common.audioPid, NULL);
        }
        /* open pid channels from list. don't add if duplicate of video or audio pid channel */
        for (i=0,j=0; i<opts.numOtherPids; i++) {
            if (!opts.otherPids[i] || (opts.otherPids[i]!=opts.common.videoPid && opts.otherPids[i]!=opts.common.audioPid)) {
                otherPidChannels[j++] = NEXUS_PidChannel_Open(parserBand0, opts.otherPids[i], NULL);
            }
            else {
                BDBG_WRN(("Ignoring duplicate pid %#x from -pids list", opts.otherPids[i]));
            }
        }

        pcrPidChannel = NEXUS_PidChannel_Open(parserBand0, opts.common.pcrPid?opts.common.pcrPid:opts.common.videoPid, NULL);
    } /* for playback, pidChannels are opened after decoders are opened */

    /* open the stcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    if(source==RecordSource_eHdmiInput) {
        stcSettings.autoConfigTimebase = false;
    }
    else if (source!=RecordSource_ePlayback) {
        stcSettings.mode = NEXUS_StcChannelMode_ePcr;
        stcSettings.modeSettings.pcr.pidChannel = pcrPidChannel;
    }
    else {
        stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    }
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

#if NEXUS_HAS_VIDEO_ENCODER
    /* encoders/mux require different STC broadcast mode from decoder */
    NEXUS_StcChannel_GetDefaultSettings(1, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;/* should be the same timebase for end-to-end locking */
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
    stcChannelTranscode = NEXUS_StcChannel_Open(1, &stcSettings);
#endif

    if (source==RecordSource_ePlayback) {
        /* set playback settings */
        NEXUS_Playback_GetSettings(playback0, &playbackSettings);
        playbackSettings.playpump = playpump0;
        playbackSettings.playpumpSettings.transportType = opts.common.transportType;
        playbackSettings.stcChannel = stcChannel;
        NEXUS_Playback_SetSettings(playback0, &playbackSettings);

        if (opts.allpass) {
        NEXUS_Playback_GetSettings(playback1, &playbackSettings);
        playbackSettings.playpump = playpump1;
        playbackSettings.playpumpSettings.transportType = opts.common.transportType;
        playbackSettings.stcChannel = stcChannel;
        playbackSettings.playpumpSettings.allPass = true;
        playbackSettings.playpumpSettings.acceptNullPackets = opts.acceptNullPackets;
        NEXUS_Playback_SetSettings(playback1, &playbackSettings);
    }
    }

    /* bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
#if 0
    if(source==RecordSource_eHdmiInput) {
        audioProgram.input = NEXUS_HdmiInput_GetAudioConnector(hdmiInput);

        audioEncoder = NEXUS_AudioEncoder_Open(NULL);
        NEXUS_AudioEncoder_AddInput(audioEncoder, NEXUS_AudioDecoder_GetConnector(audioDecoder));
        audioMuxOutput = NEXUS_AudioMuxOutput_Create(NULL);
        NEXUS_AudioOutput_AddInput(NEXUS_AudioMuxOutput_GetConnector(audioMuxOutput),
            NEXUS_AudioEncoder_GetConnector(audioEncoder));
    }
    else
#endif
    {
        rc = NEXUS_AudioOutput_AddInput(
            NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
        BDBG_ASSERT(!rc);
        rc = NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
        BDBG_ASSERT(!rc);
    }
#if NEXUS_NUM_HDMI_OUTPUTS
    if (opts.common.useHdmiOutput ) {
        rc = NEXUS_AudioOutput_AddInput(
            NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
            NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
        BDBG_ASSERT(!rc);
    }
#endif

    /* bring up display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = opts.common.displayType;
    displaySettings.format = opts.common.displayFormat;
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if (opts.common.useCompositeOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
        BDBG_ASSERT(!rc);
    }
#endif
#if NEXUS_NUM_COMPONENT_OUTPUTS
    if (opts.common.useComponentOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
        BDBG_ASSERT(!rc);
    }
#endif
#if NEXUS_NUM_PANEL_OUTPUTS
    if (opts.common.usePanelOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
        BDBG_ASSERT(!rc);
        NEXUS_BoardCfg_ConfigurePanel(true, true, true);
        BDBG_WRN(("Panel on"));
    }
#endif
#if NEXUS_NUM_SCART_INPUTS
    if (opts.common.useScart1CompositeOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
        BDBG_ASSERT(!rc);
        BDBG_WRN(("Turning on: scart 1 composite output"));
    }
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    if (opts.common.useHdmiOutput) {
        NEXUS_HdmiOutputStatus hdmiStatus;
        rc = NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
        BDBG_ASSERT(!rc);
        rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
        if ( !rc && hdmiStatus.connected )
        {
            /* the app can choose to switch to the preferred format, but it's not required. */
            NEXUS_Display_GetSettings(display, &displaySettings);
            if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
                fprintf(stderr, "Current format not supported by attached monitor -- switching to preferred format (%d)\n", hdmiStatus.preferredVideoFormat);
                displaySettings.format = hdmiStatus.preferredVideoFormat;
                NEXUS_Display_SetSettings(display, &displaySettings);
            }
        }
    }
#endif

    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    if(opts.common.videoCdb) {
        videoDecoderOpenSettings.fifoSize = opts.common.videoCdb*1024;
    }

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(opts.common.videoDecoder, &videoDecoderOpenSettings);

    /* NOTE: must open video encoder before display; otherwise open will init ViCE2 core
     * which might cause encoder display GISB error since encoder display would
     * trigger RDC to program mailbox registers in ViCE2;
     */
#if NEXUS_HAS_VIDEO_ENCODER
    if (opts.encoder) {
        BDBG_MSG(("To open video encoder..."));
        videoEncoder = NEXUS_VideoEncoder_Open(0, NULL);
        BDBG_ASSERT(videoEncoder);

        /* Bring up video encoder display */
        NEXUS_Display_GetDefaultSettings(&displaySettings);
        displaySettings.displayType = NEXUS_DisplayType_eAuto;
        displaySettings.timingGenerator = NEXUS_DisplayTimingGenerator_eEncoder;
        displaySettings.format = opts.encodeFormat;/* bring up 480p first */
        displayTranscode = NEXUS_Display_Open(NEXUS_ENCODER_DISPLAY_IDX, &displaySettings);/* cmp3 for transcoder */
        BDBG_ASSERT(displayTranscode);

        windowTranscode = NEXUS_VideoWindow_Open(displayTranscode, 0);
        BDBG_ASSERT(windowTranscode);

        /* connect same decoder to the encoder display;
         * NOTE: simul display + transcode mode might have limitation in audio path;
         * here is for video transcode bringup purpose;
         */
#if NEXUS_HAS_HDMI_INPUT
        if(source==RecordSource_eHdmiInput) {
            NEXUS_VideoWindow_AddInput(windowTranscode, NEXUS_HdmiInput_GetVideoConnector(hdmiInput));
        }
        else
#endif
        {
            NEXUS_VideoWindow_AddInput(windowTranscode, NEXUS_VideoDecoder_GetConnector(videoDecoder));
        }
    }
#endif /*NEXUS_HAS_VIDEO_ENCODER*/
    
    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    windowSettings.contentMode = opts.common.contentMode;
    rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    BDBG_ASSERT(!rc);

    /* open playback pidChannels */
    if (source==RecordSource_ePlayback) 
    {
        if (opts.allpass) 
        {
            NEXUS_PlaybackSettings playbackSettings;

            NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
            NEXUS_Playback_GetSettings(playback1, &playbackSettings);
            NEXUS_Playpump_GetAllPassPidChannelIndex(playbackSettings.playpump, (unsigned *) &playbackPidSettings.pidSettings.pidSettings.pidChannelIndex );
            allpassPidChannel = NEXUS_Playback_OpenPidChannel(playback1, 0x0, &playbackPidSettings); /* pidNo is redundant */
        }

        /* open the audio and video pid channels */
        if (opts.common.videoPid) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
        playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
        playbackPidSettings.pidTypeSettings.video.codec = opts.common.videoCodec;
        playbackPidSettings.pidTypeSettings.video.index = true;
        videoPidChannel = NEXUS_Playback_OpenPidChannel(playback0, opts.common.videoPid, &playbackPidSettings);
        }
        if (opts.common.extVideoCodec != NEXUS_VideoCodec_eNone && opts.common.extVideoPid!=0) {
            NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
            playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
            playbackPidSettings.pidTypeSettings.video.index = true;
            playbackPidSettings.pidSettings.allowTimestampReordering = opts.common.playpumpTimestampReordering;
            playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
            playbackPidSettings.pidTypeSettings.video.codec = opts.common.extVideoCodec;
            videoExtPidChannel = NEXUS_Playback_OpenPidChannel(playback0, opts.common.extVideoPid, &playbackPidSettings);
        }

        if (opts.common.audioPid) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
        playbackPidSettings.pidTypeSettings.audio.primary = audioDecoder;
        playbackPidSettings.pidSettings.pidTypeSettings.audio.codec = opts.common.audioCodec;
        audioPidChannel = NEXUS_Playback_OpenPidChannel(playback0, opts.common.audioPid, &playbackPidSettings);
        }

        /* open pid channels from list. don't add if duplicate of video or audio pid channel */
        for (i=0,j=0; i<opts.numOtherPids; i++) {
            if (!opts.otherPids[i] || (opts.otherPids[i]!=opts.common.videoPid && opts.otherPids[i]!=opts.common.audioPid)) {
                otherPidChannels[j++] = NEXUS_Playback_OpenPidChannel(playback0, opts.otherPids[i], NULL);
            }
            else {
                BDBG_WRN(("Ignoring duplicate pid %#x from -pids list", opts.otherPids[i]));
            }
        }
    }

#if NEXUS_HAS_VIDEO_ENCODER
    if (opts.encoder) {
        NEXUS_StreamMuxOutput muxOutput;
        NEXUS_VideoEncoder_GetSettings(videoEncoder, &videoEncoderConfig);
        videoEncoderConfig.bitrateMax  = opts.maxVideoBitRate;
        videoEncoderConfig.frameRate   = opts.encodeFrameRate;
        videoEncoderConfig.variableFrameRate = opts.variableFrameRate;
        videoEncoderConfig.streamStructure.framesP = opts.gopFrameP;
        videoEncoderConfig.streamStructure.framesB = opts.gopFrameB;

        NEXUS_VideoEncoder_GetDefaultStartSettings(&videoEncoderStartConfig);
        videoEncoderStartConfig.codec = opts.videoTranscodec;
        videoEncoderStartConfig.input = displayTranscode;
        videoEncoderStartConfig.stcChannel = stcChannelTranscode;
        videoEncoderStartConfig.profile = opts.videoProfile;
        videoEncoderStartConfig.level   = opts.videoLevel;

        /* encode setting and startSetting to be set after end-to-end delay is determined */
        
        /* get end-to-end delay (Dee) for audio and video encoders; 
         * TODO: match AV delay! In other words,
         *   if (aDee > vDee) {
         *       vDee' = aDee' = aDee;
         *   }
         *   else {
         *       vDee' = aDee' = vDee;
         *   }
         */
        {
            NEXUS_VideoEncoderDelayRange videoDelay;
#if NEXUS_HAS_AUDIO_MUX_OUTPUT_TEST    
            unsigned Dee;
#endif
            /* NOTE: video encoder delay is in 27MHz ticks */
            NEXUS_VideoEncoder_GetDelayRange(videoEncoder, &videoEncoderConfig, &videoEncoderStartConfig, &videoDelay);
            BDBG_WRN(("Video encoder end-to-end delay = [%u ~ %u] ms", videoDelay.min/27000, videoDelay.max/27000));
        
#if NEXUS_HAS_AUDIO_MUX_OUTPUT_TEST    
            NEXUS_AudioMuxOutput_GetDelayStatus(audioMuxOutput, audioCodec, &audioDelayStatus);
            BDBG_WRN(("\n\tAudio codec %d end-to-end delay = %u ms", audioCodec, audioDelayStatus.endToEndDelay));
    
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
            videoEncoderConfig.encoderDelay = Dee;
    
            /* Start audio mux output */
            NEXUS_AudioMuxOutput_GetDefaultStartSettings(&audioMuxStartSettings);
            audioMuxStartSettings.stcChannel = stcChannelTranscode;
            audioMuxStartSettings.presentationDelay = Dee/27000;/* in ms */
            NEXUS_AudioMuxOutput_Start(audioMuxOutput, &audioMuxStartSettings);
#else
            videoEncoderConfig.encoderDelay = videoDelay.min;
#endif
        }

        /* must set settings after Dee is determined */
        NEXUS_VideoEncoder_SetSettings(videoEncoder, &videoEncoderConfig);
        NEXUS_VideoEncoder_Start(videoEncoder, &videoEncoderStartConfig);

        NEXUS_Playpump_GetDefaultOpenSettings(&playpumpConfig);
        playpumpConfig.fifoSize = 16384; /* reduce FIFO size allocated for playpump */
        playpumpConfig.numDescriptors = 64; /* set number of descriptors */
        playpumpConfig.streamMuxCompatible = true;
        playpumpTranscodeVideo = NEXUS_Playpump_Open(VIDEO_MUX_PLAYMUMP_INDEX, &playpumpConfig);
        BDBG_ASSERT(playpumpTranscodeVideo);

        playpumpTranscodePcr = NEXUS_Playpump_Open(SYSTEM_MUX_PLAYMUMP_INDEX, &playpumpConfig);
        BDBG_ASSERT(playpumpTranscodePcr);

        BKNI_CreateEvent(&finishEvent);
        NEXUS_StreamMux_GetDefaultCreateSettings(&muxCreateSettings);
        muxCreateSettings.finished.callback = transcoderFinishCallback;
        muxCreateSettings.finished.context = finishEvent;
        streamMux = NEXUS_StreamMux_Create(&muxCreateSettings);

        NEXUS_StreamMux_GetDefaultStartSettings(&muxConfig);
        muxConfig.transportType = NEXUS_TransportType_eTs;
        muxConfig.latencyTolerance = opts.muxLatencyTolerance;
        muxConfig.stcChannel = stcChannelTranscode;

        muxConfig.video[0].pid = 0x12;
        muxConfig.video[0].encoder = videoEncoder;
        muxConfig.video[0].playpump = playpumpTranscodeVideo;
        muxConfig.pcr.pid = 0x11;
        muxConfig.pcr.playpump = playpumpTranscodePcr;
        muxConfig.pcr.interval = 50;

        /* open PidChannels */
        pidChannelTranscodePcr = NEXUS_Playpump_OpenPidChannel(playpumpTranscodePcr, muxConfig.pcr.pid, NULL);
        BDBG_ASSERT(pidChannelTranscodePcr);

        /* start mux */
        NEXUS_StreamMux_Start(streamMux, &muxConfig, &muxOutput);
        pidChannelTranscodeVideo = muxOutput.video[0];
        BDBG_ASSERT(pidChannelTranscodeVideo);
    }
#endif /* NEXUS_HAS_VIDEO_ENCODER */

    /* setup record/recpump */
    NEXUS_Recpump_GetDefaultOpenSettings(&recpumpOpenSettings);
    if (opts.data.bufferSize) {
        recpumpOpenSettings.data.bufferSize = opts.data.bufferSize;
    } else if (opts.common.extVideoCodec != NEXUS_VideoCodec_eNone && opts.common.extVideoPid!=0) {
        recpumpOpenSettings.data.bufferSize *=2;
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
#if NEXUS_HAS_VIDEO_ENCODER
    if(opts.encoder) {
        /* add multiplex data to the same record */
        /* configure the video pid for indexing */
        NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
        recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
        recordPidSettings.recpumpSettings.pidTypeSettings.video.index = true;
        recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = opts.videoTranscodec;
        NEXUS_Record_AddPidChannel(record, pidChannelTranscodeVideo, opts.recidxname?(&recordPidSettings):NULL);
        NEXUS_Record_AddPidChannel(record, pidChannelTranscodePcr, NULL);
    }
    else
#endif
    if (!opts.allpass) {
        if (opts.common.videoPid) {
            /* configure the video pid for indexing */
            NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
            recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
            recordPidSettings.recpumpSettings.pidTypeSettings.video.index = true;
            recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = opts.common.videoCodec;
            NEXUS_Record_AddPidChannel(record, videoPidChannel, opts.recidxname?(&recordPidSettings):NULL);
        }
        if(videoExtPidChannel) {
            /* configure the video pid for indexing */
            NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
            recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
            recordPidSettings.recpumpSettings.pidTypeSettings.video.index = true;
            recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = opts.common.extVideoCodec;
            NEXUS_Record_AddPidChannel(record, videoExtPidChannel, opts.recidxname?(&recordPidSettings):NULL);
        }
        if (opts.common.audioPid) {
            /* the audio pid requires no special configuration */
            NEXUS_Record_AddPidChannel(record, audioPidChannel, NULL);
        }

        for (i=0; i<opts.numOtherPids; i++) { /* add other unindexed pids */
            NEXUS_Record_AddPidChannel(record, otherPidChannels[i], NULL);
        }
    }
    else {
        NEXUS_Record_GetDefaultPidChannelSettings(&recordPidSettings);
        recordPidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
        recordPidSettings.recpumpSettings.pidTypeSettings.video.index = true;
        recordPidSettings.recpumpSettings.pidTypeSettings.video.codec = opts.common.videoCodec;
        /* for allpass record, perform indexing using pid value, not pid channel */
        recordPidSettings.recpumpSettings.pidTypeSettings.video.pid = opts.common.videoPid;
        NEXUS_Record_AddPidChannel(record, allpassPidChannel,
            (opts.recidxname&&opts.common.videoPid)?(&recordPidSettings):NULL);
    }

    gettimeofday(&startTime, NULL);
    rc = NEXUS_Record_Start(record, recfile);
    BDBG_ASSERT(!rc);
    /* Nexus is now recording to disk */
    BDBG_WRN(("Record started"));

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

        BDBG_WRN(("Waiting for frontend lock..."));
        if (BKNI_WaitForEvent(lockEvent, 5000)) {
            BDBG_ERR(("Failed"));
            return 1;
        }
        BDBG_WRN(("Locked"));
    }
#endif

    /* start decoders */
    if (opts.common.videoPid && opts.decode) {
        videoProgram.codec = opts.common.videoCodec;
        videoProgram.pidChannel = videoPidChannel;
        videoProgram.stcChannel = stcChannel;
        if(videoExtPidChannel) {
            videoProgram.enhancementPidChannel = videoExtPidChannel;
            videoProgram.codec = opts.common.extVideoCodec;
        }
        rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
        BDBG_ASSERT(!rc);
    }
    if (opts.common.audioPid && opts.decode) {
        audioProgram.codec = opts.common.audioCodec;
        audioProgram.pidChannel = audioPidChannel;
        audioProgram.stcChannel = stcChannel;
        rc = NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);
        BDBG_ASSERT(!rc);
    }

    /* start playback */
    if (source==RecordSource_ePlayback) {
        if (!opts.allpass) {
            rc = NEXUS_Playback_Start(playback0, playfile, NULL);
            BDBG_ASSERT(!rc);
        }
        else {
            rc = NEXUS_Playback_Start(playback1, playfile, NULL);
            BDBG_ASSERT(!rc);
        }
    }

    for (exit=false;!exit;)
    {
        char buffer[256];
        char *buf;

        printf("record>"); fflush(stdout);
        fgets(buffer, 256, stdin);
        if (feof(stdin)) break;
        buffer[strlen(buffer)-1] = 0; /* chop off \n */

        buf = strtok(buffer, ";");
        if (!buf) continue;

        do {
            if (!strcmp(buf, "?") || !strcmp(buf,"help")) {
                printf(
                "Commands:\n"
                "  st - print status\n"
                "  q  - stop record and quit\n"
                );
            }
            else if (!strcmp(buf, "q") || !strcmp(buf,"quit")) {
                exit = true;
                break;
            }
            else if (!strcmp(buf, "st")) {
                NEXUS_RecordStatus rstatus;
                char timestamp[20];
                get_recordtime(timestamp, sizeof(timestamp));
                rc = NEXUS_Record_GetStatus(record, &rstatus);
                BDBG_ASSERT(!rc);
                printf("(%s) data: %u(%u)KB, index: %u(%u)KB, lastTimestamp: %ums\n",
                    timestamp,
                    (unsigned)rstatus.recpumpStatus.data.bytesRecorded/1024,
                    (unsigned)rstatus.recpumpStatus.data.fifoDepth/1024,
                    (unsigned)rstatus.recpumpStatus.index.bytesRecorded/1024,
                    (unsigned)rstatus.recpumpStatus.index.fifoDepth/1024,
                    (unsigned)rstatus.lastTimestamp /* in ms */);
            }
            else if (!*buf) {
                /* allow blank line */
            }
            else {
                printf("unknown command: '%s' (use '?' for list)\n", buf);
            }
        }
        while ((buf = strtok(NULL, ";")));
    }

    /* shutdown */
#if NEXUS_HAS_FRONTEND
    BKNI_DestroyEvent(lockEvent);
#endif
    if (source==RecordSource_ePlayback) {
        NEXUS_Playback_Stop(playback0);
        if (opts.allpass) {
        NEXUS_Playback_Stop(playback1);
        }
    }
    if (opts.common.videoPid && opts.decode) {
        NEXUS_VideoDecoder_Stop(videoDecoder);
    }
    if (opts.common.audioPid && opts.decode) {
        NEXUS_AudioDecoder_Stop(audioDecoder);
    }
#if NEXUS_HAS_VIDEO_ENCODER
    if (opts.encoder) {
        NEXUS_VideoEncoder_Stop(videoEncoder, NULL);
        NEXUS_StreamMux_Finish(streamMux);
        if(BKNI_WaitForEvent(finishEvent, 2000)!=BERR_SUCCESS) {
            BDBG_WRN(("TIMEOUT"));
        }
    }
#endif
    NEXUS_Record_Stop(record);

    BDBG_WRN(("Recorded %u KB", (unsigned)get_filesize(opts.recfname)/1024));

    NEXUS_Record_RemoveAllPidChannels(record);
#if NEXUS_HAS_VIDEO_ENCODER
    if (opts.encoder) {
        BKNI_DestroyEvent(finishEvent);
        /* stream mux must be stop after PID channel is removed from record since stream_mux stop would close the pid channel */
        NEXUS_StreamMux_Stop(streamMux);
    }
#endif

    NEXUS_StcChannel_Close(stcChannel);
    if (source!=RecordSource_ePlayback) {
        NEXUS_PidChannel_CloseAll(parserBand0);
        if (opts.allpass) {
            NEXUS_PidChannel_Close(allpassPidChannel);
        }
    }
    else {
        if (opts.common.videoPid) {
            NEXUS_Playback_ClosePidChannel(playback0, videoPidChannel);
        }
        if(videoExtPidChannel) {
            NEXUS_Playback_ClosePidChannel(playback0, videoExtPidChannel);
        }
        if (opts.common.audioPid) {
            NEXUS_Playback_ClosePidChannel(playback0, audioPidChannel);
        }
        if (opts.allpass) {
            NEXUS_Playback_ClosePidChannel(playback1, allpassPidChannel);
        }

        NEXUS_FilePlay_Close(playfile);
        NEXUS_Playback_Destroy(playback0);
        NEXUS_Playpump_Close(playpump0);
        if (opts.allpass) {
            NEXUS_Playback_Destroy(playback1);
            NEXUS_Playpump_Close(playpump1);
        }
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

#if NEXUS_HAS_VIDEO_ENCODER
    if (opts.encoder) {
    NEXUS_Display_Close(displayTranscode);
    NEXUS_StreamMux_Destroy(streamMux);
    NEXUS_Playpump_Close(playpumpTranscodeVideo);
    NEXUS_Playpump_Close(playpumpTranscodePcr);
    NEXUS_VideoEncoder_Close(videoEncoder);
    }
#endif
    /* video decoder must be closed after the video input is removed from the display */
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_Platform_Uninit();

    return 0;
}

#endif

/*
************************************************

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

#record specified pids while indexing video pid and transcoded into ts stream
nexus record -mpeg_type ts -audio 0x104 -audio_type ac3 -video 0x101 -video_type avc -playfile videos/avatar_AVC_15M.ts -component on -probe -decode on -encode -encode_video_type avc -encode_format 720p  -encode_frame_rate 24 -video_bit_rate 6000000 -gop_frameP 23 -gop_frameB 0 videos/record.mpg videos/record.nav
nexus record -mpeg_type ts -audio 0x104 -audio_type ac3 -video 0x101 -video_type avc -playfile videos/avatar_AVC_15M.ts -component on -probe -decode on videos/record.mpg videos/record.nav

#allpass record from QAM
nexus record -qam 256 -freq 573 -allpass videos/qam_allpass.mpg

*************************************************
*/

