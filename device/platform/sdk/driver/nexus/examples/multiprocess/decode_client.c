/******************************************************************************
 *    (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: decode_client.c $
 * $brcm_Revision: 34 $
 * $brcm_Date: 11/27/12 1:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/decode_client.c $
 * 
 * 34   11/27/12 1:40p erickson
 * SW7435-421: undo previous change. IPC comes up before nexus join and
 *  comes down after nexus uninit.
 * 
 * 33   10/24/12 10:08a erickson
 * SW7425-2734: don't start audio decode if no audio pid
 * 
 * 32   10/16/12 4:42p erickson
 * SW7435-421: fix memory leak
 * 
 * 31   10/11/12 9:53a erickson
 * SW7425-2734: default audioDecoder = NULL because it may not be acquired
 * 
 * 30   8/10/12 10:25a erickson
 * SW7425-2734: rework examples to use NEXUS_ClientResources for decoder
 *  ids
 * 
 * 29   7/18/12 3:46p erickson
 * SW7425-2981: exit after end of stream
 * 
 * 28   7/9/12 1:57p erickson
 * SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
 * 
 * 27   7/9/12 8:46a jgarrett
 * SW7231-872: Merge to main branch
 * 
 * SW7231-872/1   7/2/12 11:37a marcusk
 * SW7231-872: update with a few fixes and improvements.  Supports multi-
 *  channel PCM output over HDMI
 * 
 * 26   6/13/12 3:06p erickson
 * SW7425-2981: improve error recovery
 * 
 * 25   6/12/12 4:54p erickson
 * SW7425-2981: use media probe
 *
 * 24   6/6/12 11:40a erickson
 * SW7425-2981: audio secondary settings not required if only one pid is
 *  used
 *
 * 23   5/2/12 4:31p erickson
 * SW7425-2981: add -video/video_type/audio/audio_type cmd line options
 *
 * 22   4/20/12 10:08a erickson
 * SW7425-2882: create fully mapped heap and notify using app ipc
 *
 * 21   1/25/12 4:06p erickson
 * SW7425-1795: client must use driver-accessible heap for playpump
 *
 * 20   1/3/12 3:51p erickson
 * SW7425-1364: allow decode_client to work with refsw_server
 *
 * 19   12/8/11 9:17a erickson
 * SW7231-521: add AVC stream, cmdline stream selection, parameterized
 *  maxwidth/height
 *
 * 18   11/29/11 11:41a erickson
 * SW7420-2129: use NULL for default settings now that default heap can be
 *  obtained via NEXUS_P_DefaultHeap
 *
 * 17   11/18/11 1:24p erickson
 * SW7420-1148: add -timeout for automation
 *
 * 16   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 *
 * 15   9/22/11 4:43p erickson
 * SW7420-1965: add simple decoder support to nexus playback module
 *
 * 14   9/7/11 11:20a erickson
 * SW7425-1258: shut down client cleanly if any failure occurs
 *
 * 13   8/25/11 10:54a erickson
 * SW7420-1148: default server to using untrusted clients
 *
 * 12   8/22/11 11:32a erickson
 * SW7125-946: NEXUS_SimpleAudioDecoder_Start may fail if audio codec not
 *  supported
 *
 * 11   8/10/11 3:03p erickson
 * SW7420-1123: move NEXUS_Platform_Uninit to nexus_platform_client.h so
 *  clients don't have to #include nexus_platform.h
 *
 * 10   5/25/11 2:28p erickson
 * SW7420-1135: audio decoder not provided with mosaic_decode_server. run
 *  video only.
 *
 * 9   4/11/11 11:46a erickson
 * SW7420-1671: use BDBG_ASSERT
 *
 * 8   2/28/11 1:50p erickson
 * SW7420-1123: link audio & video decoders
 *
 * 7   2/18/11 10:43a erickson
 * SW7420-1123: add debug
 *
 * 6   2/10/11 5:08p erickson
 * SW7420-1123: remove srand
 *
 * 5   11/1/10 2:29p erickson
 * SW7420-1135: add TSM support
 *
 * 4   10/29/10 4:33p erickson
 * SW7420-1135: add -loop. fix local uninit.
 *
 * 3   10/22/10 11:19a erickson
 * SW7420-1135: update
 *
 * 2   10/20/10 4:05p erickson
 * SW7420-1135: add audio impl
 *
 * 1   10/13/10 10:34a erickson
 * SW7420-1135: add
 *
 *****************************************************************************/
#include "nexus_platform_client.h"
#include "nexus_simple_video_decoder.h"
#include "nexus_simple_audio_decoder.h"
#include "nexus_playback.h"
#include "nexus_file.h"
#include "nexus_core_utils.h"
#include "refsw_session_simple_client.h"
#include "../../utils/namevalue.h"

/* media probe */
#include "bmedia_probe.h"
#include "bmpeg2ts_probe.h"
#include "bfile_stdio.h"
#if B_HAS_ASF
#include "basf_probe.h"
#endif
#if B_HAS_AVI
#include "bavi_probe.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(decode_client);

/* TEMP */
#include "../../utils/namevalue.c"

static void print_usage(void)
{
    printf(
        "Usage: nexus.client decode_client OPTIONS filename [indexfile]\n"
        "Options:\n"
        "  -video TRACK_NUM\n"
        "  -audio TRACK_NUM\n"
        );
}

static void b_print_media_string(const bmedia_probe_stream *stream)
{
    char stream_info[512];
    bmedia_stream_to_string(stream, stream_info, sizeof(stream_info));
    printf( "Media Probe:\n" "%s\n\n", stream_info);
}

BKNI_EventHandle g_endOfStreamEvent;

static void endOfStreamCallback(void *context, int param)
{
    NEXUS_PlaybackLoopMode endOfStreamAction = param;
    BSTD_UNUSED(context);

    BDBG_WRN(("end of stream"));
    if (endOfStreamAction != NEXUS_PlaybackLoopMode_eLoop) {
        BKNI_SetEvent(g_endOfStreamEvent);
    }
}

int main(int argc, char **argv)  {
    NEXUS_ClientConfiguration clientConfig;
    NEXUS_ClientAuthenticationSettings authSettings;
    NEXUS_SimpleVideoDecoderHandle videoDecoder;
    NEXUS_SimpleVideoDecoderStartSettings videoProgram;
    NEXUS_SimpleAudioDecoderHandle audioDecoder = NULL;
    NEXUS_SimpleAudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle file;
    NEXUS_PlaypumpOpenSettings playpumpOpenSettings;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_PidChannelHandle pcrPidChannel = NULL;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_Error rc = 0;
    unsigned timeout = 0;
    int curarg = 1;
    const char *filename = NULL;
    const char *indexname = NULL;
    unsigned videoTrack = 0;
    unsigned audioTrack = 0;
    unsigned maxWidth = 0; /* TODO */
    unsigned maxHeight = 0;
    bmedia_probe_t probe = NULL;
    const bmedia_probe_stream *stream = NULL;
    const bmedia_probe_track *track;
    NEXUS_TransportType transportType = NEXUS_TransportType_eTs;
    NEXUS_TransportTimestampType tsTimestampType = NEXUS_TransportTimestampType_eNone;
    bool detectAvcExtension = false;
    NEXUS_PlaybackLoopMode beginningOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    NEXUS_PlaybackLoopMode endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;

    BKNI_CreateEvent(&g_endOfStreamEvent);

    while (argc > curarg) {
        if (!strcmp(argv[curarg], "-timeout") && argc>curarg+1) {
            timeout = strtoul(argv[++curarg], NULL, 0);
        }
        else if (!strcmp(argv[curarg], "-bof") && curarg+1<argc) {
            beginningOfStreamAction=lookup(g_endOfStreamActionStrs, argv[++curarg]);
        }
        else if (!strcmp(argv[curarg], "-eof") && curarg+1<argc) {
            endOfStreamAction=lookup(g_endOfStreamActionStrs, argv[++curarg]);
        }
        else if (!strcmp(argv[curarg], "-video") && curarg+1<argc) {
            videoTrack = strtoul(argv[++curarg], NULL, 10);
        }
        else if (!strcmp(argv[curarg], "-audio") && curarg+1<argc) {
            audioTrack = strtoul(argv[++curarg], NULL, 10);
        }
        else if (!strcmp(argv[curarg], "-detect_avc_extension")) {
            detectAvcExtension = true;
        }
        else if (!filename) {
            filename = argv[curarg];
        }
        else if (!indexname) {
            indexname = argv[curarg];
        }
        curarg++;
    }
    if (!filename) {
        print_usage();
        return -1;
    }
    
    rc = simple_client_init(argv[0], &authSettings);
    if (!rc) {
        /* use app ipc value w/ refsw_server */
    }
    printf("using certificate:'%s'\n", (char *)authSettings.certificate.data);
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) {
        printf("cannot join: %d\n", rc);
        return -1;
    }
    NEXUS_Platform_GetClientConfiguration(&clientConfig);

    /* probe the stream */
    {
        bfile_io_read_t fd = NULL;
        FILE *fin;
        bmedia_probe_config probe_config;

        probe = bmedia_probe_create();

        fin = fopen64(filename, "rb");
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
            return -1;
        }
    }

    b_print_media_string(stream);
    transportType = b_mpegtype2nexus(stream->type);
    if (stream->type == bstream_mpeg_type_ts) {
        if ((((bmpeg2ts_probe_stream*)stream)->pkt_len) == 192) {
            tsTimestampType = NEXUS_TransportTimestampType_eMod300;
        }
    }
    if (!indexname) {
        if (stream->index == bmedia_probe_index_available || stream->index == bmedia_probe_index_required) {
            /* if user didn't specify an index, use the file as index if probe indicates */
            indexname = filename;
        }
    }

    file = NEXUS_FilePlay_OpenPosix(filename, indexname);
    if (!file) {
        BDBG_ERR(("can't open files: %s %s", filename, indexname));
        rc = -1;
        goto done;
    }

    NEXUS_Playpump_GetDefaultOpenSettings(&playpumpOpenSettings);
    playpump = NEXUS_Playpump_Open(NEXUS_ANY_ID, &playpumpOpenSettings);
    BDBG_ASSERT(playpump);
    playback = NEXUS_Playback_Create();
    BDBG_ASSERT(playback);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
    stcSettings.modeSettings.Auto.transportType = transportType;
    stcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
    BDBG_ASSERT(stcChannel);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = transportType;
    playbackSettings.stcChannel = stcChannel;
    playbackSettings.stcTrick = true;
    playbackSettings.beginningOfStreamAction = beginningOfStreamAction;
    playbackSettings.endOfStreamAction = endOfStreamAction;
    playbackSettings.endOfStreamCallback.callback = endOfStreamCallback;
    playbackSettings.endOfStreamCallback.param = endOfStreamAction;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* Bring up audio decoders and outputs */
    if (clientConfig.resources.simpleVideoDecoder.total) {
        videoDecoder = NEXUS_SimpleVideoDecoder_Acquire(clientConfig.resources.simpleVideoDecoder.id[0]);
    }
    else {
        videoDecoder = NULL;
    }
    if (!videoDecoder) {
        BDBG_ERR(("video decoder not available"));
        rc = -1;
        goto done;
    }
    if (clientConfig.resources.simpleAudioDecoder.total) {
        audioDecoder = NEXUS_SimpleAudioDecoder_Acquire(clientConfig.resources.simpleAudioDecoder.id[0]);
        if (!audioDecoder) {
            BDBG_WRN(("audio decoder not available"));
            /* for this example, audio is optional */
        }
    }

    /* open pid channels and configure start settings based on probe */
    NEXUS_SimpleVideoDecoder_GetDefaultStartSettings(&videoProgram);
    NEXUS_SimpleAudioDecoder_GetDefaultStartSettings(&audioProgram);
    for(track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
        switch(track->type) {
            case bmedia_track_type_audio:
                if(track->info.audio.codec != baudio_format_unknown) {
                    if (audioDecoder && !audioTrack--) {
                        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
                        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
                        playbackPidSettings.pidTypeSettings.audio.simpleDecoder = audioDecoder;
                        audioProgram.primary.pidChannel = NEXUS_Playback_OpenPidChannel(playback, track->number, &playbackPidSettings);

                        audioProgram.primary.codec = b_audiocodec2nexus(track->info.audio.codec);
                        audioProgram.primary.stcChannel = stcChannel;
                    }
                }
                break;
            case bmedia_track_type_video:
                if(track->info.video.codec == bvideo_codec_h264_svc || track->info.video.codec == bvideo_codec_h264_mvc) {
                    if (detectAvcExtension && videoProgram.settings.pidChannel && videoTrack == (unsigned)-1) {
                        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
                        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
                        playbackPidSettings.pidTypeSettings.video.codec = b_videocodec2nexus(track->info.video.codec);
                        playbackPidSettings.pidTypeSettings.video.index = true;
                        playbackPidSettings.pidTypeSettings.video.simpleDecoder = videoDecoder;
                        videoProgram.settings.enhancementPidChannel = NEXUS_Playback_OpenPidChannel(playback, track->number, &playbackPidSettings);

                        videoProgram.settings.codec = b_videocodec2nexus(track->info.video.codec); /* overwrite */
                    }
                    break;
                }
                else if (track->info.video.codec != bvideo_codec_unknown) {
                    if (!videoTrack--) {
                        NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
                        playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
                        playbackPidSettings.pidTypeSettings.video.codec = b_videocodec2nexus(track->info.video.codec);
                        playbackPidSettings.pidTypeSettings.video.index = true;
                        playbackPidSettings.pidTypeSettings.video.simpleDecoder = videoDecoder;
                        videoProgram.settings.pidChannel = NEXUS_Playback_OpenPidChannel(playback, track->number, &playbackPidSettings);

                        videoProgram.settings.codec = b_videocodec2nexus(track->info.video.codec);
                        videoProgram.settings.stcChannel = stcChannel;
                        videoProgram.settings.timestampMode = track->info.video.timestamp_order;
                        if (maxWidth && maxHeight) {
                            videoProgram.maxWidth = maxWidth;
                            videoProgram.maxHeight = maxHeight;
                        }
                    }
                }
                break;
#if 0
/* TODO: need playback to handle duplicate w/ different settings in the case of eOther */
            case bmedia_track_type_pcr:
                NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
                playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eOther;
                pcrPidChannel = NEXUS_Playback_OpenPidChannel(playback, track->number, &playbackPidSettings);
                break;
#endif
            default:
                break;
        }
    }

#if B_HAS_AVI
    if (stream->type == bstream_mpeg_type_avi && ((bavi_probe_stream *)stream)->video_framerate) {
        NEXUS_LookupFrameRate(((bavi_probe_stream *)stream)->video_framerate, &videoProgram.settings.frameRate);
    }
#endif

    /* Start decoders */
    if (videoProgram.settings.pidChannel) {
        rc = NEXUS_SimpleVideoDecoder_Start(videoDecoder, &videoProgram);
        BDBG_ASSERT(!rc);
    }
    if (audioDecoder && audioProgram.primary.pidChannel) {
        NEXUS_SimpleAudioDecoder_Start(audioDecoder, &audioProgram);
        /* decode may fail if audio codec not supported */
    }
    rc = NEXUS_Playback_Start(playback, file, NULL);
    BDBG_ASSERT(!rc);

    if (endOfStreamAction == NEXUS_PlaybackLoopMode_eLoop && !timeout) {
        printf("Press ENTER to stop\n");
        getchar();
    }
    else {
        if (timeout == 0) {
            timeout = BKNI_INFINITE;
        }
        else {
            timeout = timeout*1000;
        }
        /* auto close after timeout or at end of stream */
        BKNI_WaitForEvent(g_endOfStreamEvent, timeout);
    }

    /* Bring down system */
    NEXUS_SimpleVideoDecoder_Stop(videoDecoder);
    if (audioDecoder) {
        NEXUS_SimpleAudioDecoder_Stop(audioDecoder);
    }
    NEXUS_Playback_Stop(playback);

    if (videoProgram.settings.pidChannel) {
        NEXUS_Playback_ClosePidChannel(playback, videoProgram.settings.pidChannel);
    }
    if (audioProgram.primary.pidChannel) {
        NEXUS_Playback_ClosePidChannel(playback, audioProgram.primary.pidChannel);
    }
    if (pcrPidChannel) {
        NEXUS_Playback_ClosePidChannel(playback, pcrPidChannel);
    }

    NEXUS_SimpleVideoDecoder_Release(videoDecoder);
    if (audioDecoder) {
        NEXUS_SimpleAudioDecoder_Release(audioDecoder);
    }

    if (playback) NEXUS_Playback_Destroy(playback);
    if (playpump) NEXUS_Playpump_Close(playpump);
    
done:
    if (file) NEXUS_FilePlay_Close(file);
    if (probe) {
        if (stream) {
            bmedia_probe_stream_free(probe, stream);
        }
        bmedia_probe_destroy(probe);
    }
    NEXUS_Platform_Uninit();
    simple_client_shutdown();
    return rc;
}
