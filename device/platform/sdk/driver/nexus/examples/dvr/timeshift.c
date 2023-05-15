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
 * $brcm_Workfile: timeshift.c $
 * $brcm_Revision: 23 $
 * $brcm_Date: 2/21/12 3:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/dvr/timeshift.c $
 * 
 * 23   2/21/12 3:44p erickson
 * SW7425-2439: remove requirement that timeshifting playback must wait
 *  for first picture to be indexed
 * 
 * 22   2/14/12 4:07p erickson
 * SW7425-2363: change file suffix to avoid confusing with linear TS or
 *  NAV files
 * 
 * 21   2/6/12 1:23p erickson
 * SWDEPRECATED-3955: switch to live decode when timeshifted playback hits
 *  end of stream
 * 
 * 20   2/3/12 4:16p erickson
 * SW7425-2282: add '9' to jump to end, bound fwd jump, use DBG for
 *  timestamps
 * 
 * 19   1/26/12 3:00p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 18   1/7/11 3:20p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 17   3/22/10 1:17p randyjew
 * SW7468-143: Support for boards without component and/or composite
 * 
 * 17   3/22/10 2:34a randyjew
 * SW7468-143: Support for boards without component and/or composite
 * 
 * 16   3/1/10 2:12p mphillip
 * SW7550-277: Guard fifo examples for platforms lacking record/playback
 * 
 * 15   2/23/10 5:16p erickson
 * SW7400-2684: switch beginningOfStreamAction to
 *  NEXUS_PlaybackLoopMode_ePause, which is more likely. add code to gap
 *  appropriately.
 *
 * 14   6/23/09 4:29p erickson
 * PR56173: set timebase to PCR in recording stream to prevent drift
 *  during timeshifting
 *
 * 13   4/27/09 2:37p erickson
 * PR54129: expand FileFifo size. don't jump to absolute beginning to
 *  avoid false beginningOfStream actions.
 *
 * 12   4/22/09 1:35p erickson
 * PR54129: fix warnings
 *
 * 11   4/20/09 2:52p erickson
 * PR54129: added 0 command to jump to beginning, added more DBG
 *
 * 10   4/16/09 1:30p erickson
 * PR54129: add seek commands +/- for test
 *
 * 9   3/5/09 5:46p vsilyaev
 * PR 52579: Updated circular FIFO/timeshifting code
 *
 * 8   2/23/09 11:09a erickson
 * PR52034: poll on NEXUS_RecordStatus.picturesIndexed before starting
 *  timeshift playback
 *
 * 7   2/19/09 11:54a erickson
 * PR51841: refactor app
 *
 * 6   2/4/09 2:07p erickson
 * PR51841: fix dtv
 *
 * 5   2/4/09 2:00p erickson
 * PR51841: use FileFifo, fix shutdown
 *
 *****************************************************************************/
/* Nexus example app: timeshifting with FileFifo */

#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_timebase.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_RECORD
#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_file_fifo.h"
#include "nexus_record.h"
#endif

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(timeshift);

/* the following define the input file and its characteristics -- these will vary by input file */
/* don't use .mpg and .nav because a fifo file is not a regular TS or NAV file */
#define DATA_FILE_NAME "videos/stream.fifompg"
#define INDEX_FILE_NAME "videos/stream.fifonav"
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x31
#define AUDIO_PID 0x34

/* global app state makes the callbacks easier to implement. in a real app, this state
should be passed into the callbacks by reference. */
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_RECORD
typedef struct b_app_context {
    enum {
        decode_state_stopped,
        decode_state_live,
        decode_state_playback
    } decode_state;
    int rate;
    NEXUS_FifoRecordSettings fifoRecordSettings;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackLoopMode beginningOfStreamAction;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel, pcrPidChannel;
    NEXUS_FilePlayHandle playbackfile;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_RecordHandle record;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_ParserBand parserBand;
    uint32_t startAtTimestamp;
} b_app_context;

void jump_to_beginning(b_app_context *app)
{
    unsigned pos;
    NEXUS_Error rc;
    NEXUS_PlaybackStatus playbackStatus;

    rc = NEXUS_Playback_GetStatus(app->playback, &playbackStatus);
    BDBG_ASSERT(!rc);
    pos = playbackStatus.first;
    if (playbackStatus.last - playbackStatus.first > app->fifoRecordSettings.interval * 1000 / 2) {
        /* once we've reached half of the timeshift buffer, we should not jump to the absolute beginning.
        the beginning of the file is truncated in blocks.
        if we jump to the absolute beginning, we might get an undesired beginningOfStream action when that truncation occurs. */
        pos += 5000;
    }
    printf("Jump to beginning %u, %u...%u\n", pos, (unsigned)playbackStatus.first, (unsigned)playbackStatus.last);
    (void)NEXUS_Playback_Seek(app->playback, pos);
}

static void stop_decode(b_app_context *app);

static void start_live_decode(b_app_context *app)
{
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_StcChannelSettings stcSettings;
    int rc;
    
    if (app->decode_state == decode_state_live) {
        return;
    }
    else if (app->decode_state != decode_state_stopped) {
        stop_decode(app);
    }
    
    app->videoPidChannel = NEXUS_PidChannel_Open(app->parserBand, VIDEO_PID, NULL);
    app->audioPidChannel = NEXUS_PidChannel_Open(app->parserBand, AUDIO_PID, NULL);

    NEXUS_StcChannel_GetSettings(app->stcChannel, &stcSettings);
    stcSettings.mode = NEXUS_StcChannelMode_ePcr;
    stcSettings.modeSettings.pcr.pidChannel = app->pcrPidChannel;
    rc = NEXUS_StcChannel_SetSettings(app->stcChannel, &stcSettings);
    BDBG_ASSERT(!rc);
    
    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = app->videoPidChannel;
    videoProgram.stcChannel = app->stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = AUDIO_CODEC;
    audioProgram.pidChannel = app->audioPidChannel;
    audioProgram.stcChannel = app->stcChannel;

    /* Start decoders */
    rc = NEXUS_VideoDecoder_Start(app->videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioDecoder_Start(app->audioDecoder, &audioProgram);
    BDBG_ASSERT(!rc);
    
    app->decode_state = decode_state_live;
}

static void start_playback_decode(b_app_context *app)
{
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_StcChannelSettings stcSettings;
    int rc;
    
    if (app->decode_state == decode_state_playback) {
        return;
    }
    else if (app->decode_state != decode_state_stopped) {
        stop_decode(app);
    }
    
    NEXUS_StcChannel_GetSettings(app->stcChannel, &stcSettings);
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    rc = NEXUS_StcChannel_SetSettings(app->stcChannel, &stcSettings);
    BDBG_ASSERT(!rc);
    
    /* Open the audio and video pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = VIDEO_CODEC; /* must be told codec for correct handling */
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = app->videoDecoder;
    app->videoPidChannel = NEXUS_Playback_OpenPidChannel(app->playback, VIDEO_PID, &playbackPidSettings);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    playbackPidSettings.pidTypeSettings.audio.primary = app->audioDecoder;
    app->audioPidChannel = NEXUS_Playback_OpenPidChannel(app->playback, AUDIO_PID, &playbackPidSettings);
    
    NEXUS_Playback_GetSettings(app->playback, &playbackSettings);
    playbackSettings.startPaused = (app->rate == 0);
    rc = NEXUS_Playback_SetSettings(app->playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = app->videoPidChannel;
    videoProgram.stcChannel = app->stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = AUDIO_CODEC;
    audioProgram.pidChannel = app->audioPidChannel;
    audioProgram.stcChannel = app->stcChannel;

    /* Start decoders */
    rc = NEXUS_VideoDecoder_Start(app->videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioDecoder_Start(app->audioDecoder, &audioProgram);
    BDBG_ASSERT(!rc);

    /* Linking Playback to Record allows Playback to sleep until Record writes data. Avoids a busyloop near live. */
    rc = NEXUS_Record_AddPlayback(app->record, app->playback);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Playback_Start(app->playback, app->playbackfile, NULL);
    BDBG_ASSERT(!rc);
    
    app->decode_state = decode_state_playback;
    
    if (app->startAtTimestamp) {
        /* we are starting from a stopped live decode state, so we need to seek */
        BDBG_MSG(("started at %#x", app->startAtTimestamp));
        NEXUS_Playback_Seek(app->playback, app->startAtTimestamp);
        app->startAtTimestamp = 0;
    }
}

static void stop_decode(b_app_context *app)
{
    if (app->decode_state == decode_state_stopped) {
        return;
    }
    
    if (app->decode_state == decode_state_playback) {
        NEXUS_Playback_Stop(app->playback);
        NEXUS_Record_RemovePlayback(app->record, app->playback);
    }
    else {
        NEXUS_RecordStatus recordStatus;
        NEXUS_Record_GetStatus(app->record, &recordStatus);
        app->startAtTimestamp = recordStatus.lastTimestamp;
        BDBG_MSG(("stopped live at %#x", app->startAtTimestamp));
    }
    
    NEXUS_VideoDecoder_Stop(app->videoDecoder);
    NEXUS_AudioDecoder_Stop(app->audioDecoder);
    
    if (app->decode_state == decode_state_playback) {
        NEXUS_Playback_ClosePidChannel(app->playback, app->videoPidChannel);
        NEXUS_Playback_ClosePidChannel(app->playback, app->audioPidChannel);
    }
    else {
        NEXUS_PidChannel_Close(app->videoPidChannel);
        NEXUS_PidChannel_Close(app->audioPidChannel);
    }
    
    app->decode_state = decode_state_stopped;
}
#endif

void beginning_of_stream(void *context, int param)
{
    b_app_context *app = context;
    BSTD_UNUSED(param);
    printf("beginning_of_stream: rate %d\n", app->rate);

    if (app->beginningOfStreamAction == NEXUS_PlaybackLoopMode_ePause) {
        if (app->rate >= 1) {
            /* if you get a bos action when going forward, you have just fallen off the beginning.
            use jump_to_beginning to gap more, then keep going. */
            start_playback_decode(app);
            jump_to_beginning(app);
            app->rate = 1;
            NEXUS_Playback_Play(app->playback);
        }
        else {
            app->rate = 0;
        }
    }
    else {
        app->rate = 1;
    }
}

void end_of_stream(void *context, int param)
{
    b_app_context *app = context;
    BSTD_UNUSED(param);
    printf("end_of_stream\n");
    if (app->rate) {
        app->rate = 1;
        start_live_decode(app);
    }
}

int main(void)
{
#if NEXUS_HAS_PLAYBACK && NEXUS_HAS_RECORD
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_DisplaySettings displaySettings;
#endif
    NEXUS_FifoRecordHandle fifofile;
    NEXUS_FileRecordHandle recordfile;
    NEXUS_RecpumpHandle recpump;
    NEXUS_RecordPidChannelSettings pidSettings;
    NEXUS_RecordSettings recordSettings;
    NEXUS_PidChannelHandle pidChannel[2];
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_Error rc;
    const char *datafilename = DATA_FILE_NAME;
    const char *indexfilename = INDEX_FILE_NAME;
    b_app_context appinstance, *app = &appinstance;

    rc = NEXUS_Platform_Init(NULL);
    if (rc) return -1;
    
    NEXUS_Platform_GetConfiguration(&platformConfig);
    BKNI_Memset(app, 0, sizeof(*app));
    app->rate = 1;

    /******************************
    * start record
    **/

    app->parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(app->parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(app->parserBand, &parserBandSettings);

    recpump = NEXUS_Recpump_Open(0, NULL);
    app->record = NEXUS_Record_Create();
    NEXUS_Record_GetSettings(app->record, &recordSettings);
    recordSettings.recpump = recpump;
    NEXUS_Record_SetSettings(app->record, &recordSettings);

    fifofile = NEXUS_FifoRecord_Create(datafilename, indexfilename);

    NEXUS_FifoRecord_GetSettings(fifofile, &app->fifoRecordSettings);
    app->fifoRecordSettings.interval = 60;
    rc = NEXUS_FifoRecord_SetSettings(fifofile, &app->fifoRecordSettings);
    BDBG_ASSERT(!rc);

    recordfile = NEXUS_FifoRecord_GetFile(fifofile);

    NEXUS_Record_GetDefaultPidChannelSettings(&pidSettings);
    pidSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
    pidSettings.recpumpSettings.pidTypeSettings.video.index = true;
    pidSettings.recpumpSettings.pidTypeSettings.video.codec = VIDEO_CODEC;

    pidChannel[0] = NEXUS_PidChannel_Open(app->parserBand, VIDEO_PID, NULL);
    rc = NEXUS_Record_AddPidChannel(app->record, pidChannel[0], &pidSettings);
    BDBG_ASSERT(!rc);

    pidChannel[1] = NEXUS_PidChannel_Open(app->parserBand, AUDIO_PID, NULL);
    rc = NEXUS_Record_AddPidChannel(app->record, pidChannel[1], NULL);
    BDBG_ASSERT(!rc);

    BDBG_WRN(("start record"));
    rc = NEXUS_Record_Start(app->record, recordfile);
    BDBG_ASSERT(!rc);

    /******************************
    * open playback, but don't start
    **/

    app->playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(app->playpump);
    app->playback = NEXUS_Playback_Create();
    BDBG_ASSERT(app->playback);

    app->playbackfile = NEXUS_FifoPlay_Open(datafilename, indexfilename, fifofile);
    if (!app->playbackfile) {
        BDBG_ERR(("can't open files:%s %s", datafilename, indexfilename));
        return -1;
    }

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.autoConfigTimebase = false; /* by default, NEXUS_StcChannelMode_eAuto would put the timebase into eFreeRun mode.
                                               autoConfigTimebase = false allows the app to configure the DPCR to track the PCR's rate, yet still do PVR TSM. */
    app->stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* PCR pid channel is from live parser band, even when timeshifting */
    app->pcrPidChannel = pidChannel[0];
    
    NEXUS_Timebase_GetSettings(stcSettings.timebase, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
    timebaseSettings.sourceSettings.pcr.pidChannel = app->pcrPidChannel;
    rc = NEXUS_Timebase_SetSettings(stcSettings.timebase, &timebaseSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Playback_GetSettings(app->playback, &playbackSettings);
    playbackSettings.playpump = app->playpump;
    playbackSettings.playpumpSettings.transportType = TRANSPORT_TYPE;
    playbackSettings.beginningOfStreamCallback.callback = beginning_of_stream;
    playbackSettings.beginningOfStreamCallback.context = app;
#if 1
    /* switching to pause at the beginning is the more likely scenario, but it's a little harder to code.
    the app is responsible to gap the beginning correctly. */
    playbackSettings.beginningOfStreamAction = NEXUS_PlaybackLoopMode_ePause;
#else
    /* when play hits the beginning, switch back to play. this is easier to implement in the app. */
    playbackSettings.beginningOfStreamAction = NEXUS_PlaybackLoopMode_ePlay;
#endif
    app->beginningOfStreamAction = playbackSettings.beginningOfStreamAction;
    playbackSettings.endOfStreamCallback.callback = end_of_stream;
    playbackSettings.endOfStreamCallback.context = app;
    playbackSettings.endOfStreamAction = NEXUS_PlaybackLoopMode_ePlay; /* when play hits the end, wait for record */
    playbackSettings.timeshifting = true;
    playbackSettings.stcChannel = app->stcChannel;
    playbackSettings.stcTrick = true;
    rc = NEXUS_Playback_SetSettings(app->playback, &playbackSettings);
    BDBG_ASSERT(!rc);

    /* Bring up audio decoders and outputs */
    app->audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(app->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(app->audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif

    /* Bring up video display and outputs */
    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif 
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
        }
    }
#endif

    /* bring up decoder and connect to display */
    app->videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoDecoder_GetSettings(app->videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.channelChangeMode = NEXUS_VideoDecoder_ChannelChangeMode_eHoldUntilFirstPicture;
    NEXUS_VideoDecoder_SetSettings(app->videoDecoder, &videoDecoderSettings);
    
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(app->videoDecoder));

    start_live_decode(app);

#if 1
    for(;;) {
        NEXUS_PlaybackStatus playbackStatus;
        NEXUS_VideoDecoderStatus videoStatus;
        NEXUS_AudioDecoderStatus audioStatus;
        uint32_t stc;
        NEXUS_FilePosition first, last;
        char cmd[16];
        unsigned i;
        bool quit=false;
        NEXUS_PlaybackTrickModeSettings trickmode_settings;


        printf("CMD:>");
        if(fgets(cmd, sizeof(cmd)-1, stdin)==NULL) {
            break;
        }
        for(i=0;cmd[i]!='\0';i++) {
            switch(cmd[i]) {
            case '?':
                printf("? - this help\n"
                       "f - Fast Forward\n"
                       "r - Rewind\n"
                       "s - pauSe\n"
                       "p - Play\n"
                       "a - frame Advance\n"
                       "w - Wait 30msec\n"
                       "+ - Jump forward 5 seconds\n"
                       "- - Jump backward 5 seconds\n"
                       "0 - Jump to the beginning\n"
                       "9 - Jump to the end (catchup to live)\n"
                       "q - Quit\n"
                       );
                break;
            case 's':
                printf( "pause\n" );
                app->rate = 0;
                if (app->decode_state == decode_state_live) {
                    
                    /* if live, just stop decode and keep the last picture visible. next operation will start playback. */
                    stop_decode(app);
                }
                else {
                    start_playback_decode(app);
                    NEXUS_Playback_Pause(app->playback);
                }
                break;
            case 'p':
                {
                    bool stopped_live = (app->decode_state == decode_state_stopped);
                    printf( "play\n" );
                    app->rate = 1;
                    start_playback_decode(app);
                    if (!stopped_live) {
                        NEXUS_Playback_Play(app->playback);
                    }
                }
                break;
            case 'a':
                printf( "frame advance\n" );
                start_playback_decode(app);
                NEXUS_Playback_FrameAdvance(app->playback, app->rate >= 0);
                break;
            case 'f':
                start_playback_decode(app);
                NEXUS_Playback_GetDefaultTrickModeSettings( &trickmode_settings );
                if(app->rate<=0) {
                    app->rate = 1;
                }
                app->rate *=2;
                trickmode_settings.rate = app->rate*NEXUS_NORMAL_PLAY_SPEED;
                printf( "fastward %d\n", trickmode_settings.rate );
                NEXUS_Playback_TrickMode(app->playback, &trickmode_settings );
                break;
            case 'r':
                start_playback_decode(app);
                NEXUS_Playback_GetDefaultTrickModeSettings( &trickmode_settings );
                if(app->rate>=0) {
                    app->rate = -1;
                }
                else {
                    app->rate *=2;
                }
                trickmode_settings.rate = app->rate*NEXUS_NORMAL_PLAY_SPEED;
                printf( "rewind %d\n", trickmode_settings.rate );
                NEXUS_Playback_TrickMode(app->playback, &trickmode_settings );
                break;
            case 'w':
                BKNI_Sleep(30);
                break;
            case '-':
                start_playback_decode(app);
                rc = NEXUS_Playback_GetStatus(app->playback, &playbackStatus);
                BDBG_ASSERT(!rc);
                if (playbackStatus.position >= 5*1000) {
                    playbackStatus.position -= 5*1000;
                    /* it's normal for a Seek to fail if it goes past the beginning */
                    printf("Jump to %u, %u...%u\n", (unsigned)playbackStatus.position, (unsigned)playbackStatus.first, (unsigned)playbackStatus.last);
                    (void)NEXUS_Playback_Seek(app->playback, playbackStatus.position);
                }
                break;
            case '+':
                start_playback_decode(app);
                rc = NEXUS_Playback_GetStatus(app->playback, &playbackStatus);
                BDBG_ASSERT(!rc);
                /* it's normal for a Seek to fail if it goes past the end */
                playbackStatus.position += 5*1000;
                if (playbackStatus.position > playbackStatus.last) {
                    playbackStatus.position = playbackStatus.last;
                }
                printf("Jump to %u, %u...%u\n", (unsigned)playbackStatus.position, (unsigned)playbackStatus.first, (unsigned)playbackStatus.last);
                (void)NEXUS_Playback_Seek(app->playback, playbackStatus.position);
                break;
            case '0':
                {
                    bool startNormalPlay = (app->rate != 1);
                    app->rate = 1;
                    start_playback_decode(app);
                    jump_to_beginning(app);
                    if (startNormalPlay) {
                        NEXUS_Playback_Play(app->playback);
                    }
                }
                break;
            case '9':
                app->rate = 1;
                start_live_decode(app);
                break;
            case '\n':
            case ' ':
                NEXUS_StcChannel_GetStc(app->stcChannel, &stc);
                NEXUS_VideoDecoder_GetStatus(app->videoDecoder, &videoStatus);
                NEXUS_AudioDecoder_GetStatus(app->audioDecoder, &audioStatus);

                printf("video %u%% %ux%u (%#x:%#x:%d), audio %u%% %uHz (%#x:%#x:%d)\n",
                    videoStatus.fifoSize ? (videoStatus.fifoDepth * 100) / videoStatus.fifoSize : 0,
                    videoStatus.source.width, videoStatus.source.height, videoStatus.pts, stc, videoStatus.ptsStcDifference,
                    audioStatus.fifoSize ? (audioStatus.fifoDepth * 100) / audioStatus.fifoSize : 0,
                    audioStatus.sampleRate, audioStatus.pts, stc, audioStatus.ptsStcDifference
                    );
                if (app->decode_state == decode_state_playback) {
                    rc = NEXUS_Playback_GetStatus(app->playback, &playbackStatus);
                    BDBG_ASSERT(!rc);
                    NEXUS_FifoRecord_GetPosition(fifofile, &first, &last);
                    printf("rate=%d file %u:%u, playback %u%% position=%u.%03u sec\n",
                        (int)playbackStatus.trickModeSettings.rate,
                        (unsigned)first.mpegFileOffset,  (unsigned)last.mpegFileOffset,
                        playbackStatus.fifoSize ? (playbackStatus.fifoDepth * 100) / playbackStatus.fifoSize : 0,
                        (unsigned)playbackStatus.position/1000, (unsigned)playbackStatus.position%1000
                        );
                }
                break;
            case 'q':
                quit = true;
                break;
            default:
                break;
            }
        }
        if(quit)  {
            break;
        }


    }
#else
    printf("timeshifting playback started. press ENTER to stop.\n");
    getchar();
#endif

    /* stop decode & playback */
    stop_decode(app);
    /* stop record */
    NEXUS_Record_Stop(app->record);
    NEXUS_Record_RemoveAllPidChannels(app->record);
    NEXUS_PidChannel_Close(pidChannel[0]);
    NEXUS_PidChannel_Close(pidChannel[1]);

    NEXUS_FilePlay_Close(app->playbackfile);
    NEXUS_FileRecord_Close(recordfile);
    NEXUS_Playback_Destroy(app->playback);
    NEXUS_Playpump_Close(app->playpump);
    NEXUS_Record_Destroy(app->record);
    NEXUS_Recpump_Close(recpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(app->videoDecoder));
    NEXUS_VideoDecoder_Close(app->videoDecoder);
    NEXUS_AudioDecoder_Close(app->audioDecoder);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

#else
    printf("This application is not supported on this platform!\n");
#endif
    return 0;
}

