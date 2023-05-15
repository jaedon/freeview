/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: playback_slow_io.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/2/09 7:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/playback/playback_slow_io.c $
 * 
 * Hydra_Software_Devel/1   6/2/09 7:36p vsilyaev
 * PR 55417: Unit test for slow I/O
 * 
 *
 *****************************************************************************/
/* 
 * This test verifies support of the slow IO, in the nexus playback code. 
 * Slow I/O could return BFILE_ERROR_NO_DATA result, and it should be transparent for the application.
 * One of examples of slow I/O is a I/O over network. Also playback supports subset of the partial reads, 
 * however due to intrinsic details of O_DIRECT support  partial read should N*4096, where N>=1
 */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback.h"
#include "nexus_file.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
#include "nexus_file.h"
#include "bfile_io.h"
#include "priv/nexus_file_pvr.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(playback_slow_io);

struct b_test_slow_fileio_read {
	struct bfile_io_read ops; /* shall be the first member */
    unsigned count;
    unsigned delay_factor;
    off_t off;
    bfile_io_read_t master;
};

static ssize_t 
b_test_slow_read(bfile_io_read_t fd, void *buf, size_t length)
{
    ssize_t result;
	struct b_test_slow_fileio_read *f=(void *)fd;
    f->count ++;
    if(f->count > f->delay_factor) {
        BDBG_WRN(("read: %u:%u -> NO DATA", (unsigned)f->off, length));
        f->count = 0;
        return BFILE_ERROR_NO_DATA;
    }
#if 1
    if(f->count == f->delay_factor/2) {
        size_t new_length = length/2;
        new_length = new_length - new_length%BIO_BLOCK_SIZE;
        if(new_length>2*BIO_BLOCK_SIZE) {
            BDBG_WRN(("read: %u:%u -> %u", (unsigned)f->off, length, new_length));
            length=new_length;
        }
    }
#endif
	result = f->master->read(f->master, buf, length );
    BDBG_MSG(("read: %u:%u -> %u", (unsigned)f->off, length, result));
    if(result>0) {
        f->off += result;
    }
    return result;
}

static off_t 
b_test_slow_seek(bfile_io_read_t fd, off_t offset, int whence)
{
	struct b_test_slow_fileio_read *f=(void *)fd;
	f->off = f->master->seek(f->master, offset, whence);
    return f->off;
}

static int 
b_test_slow_bounds(bfile_io_read_t fd, off_t *first, off_t *last)
{
	struct b_test_slow_fileio_read *f=(void *)fd;
	return f->master->bounds(f->master, first, last);
}

static const struct bfile_io_read b_test_slow_fileio_read_ops = {
	b_test_slow_read,
	b_test_slow_seek,
	b_test_slow_bounds,
	BIO_DEFAULT_PRIORITY
};

static bfile_io_read_t 
b_test_slow_read_attach(struct b_test_slow_fileio_read *file, bfile_io_read_t master, unsigned factor)
{
    file->ops = b_test_slow_fileio_read_ops;
    file->master = master;
    file->count = 0;
    file->delay_factor = factor;
    file->off = 0;
    return &file->ops;
}

static const struct stream_info {
    const char *fname;
    NEXUS_TransportType type;
    NEXUS_VideoCodec videoCodec;
    unsigned videoTrack;
    NEXUS_AudioCodec audioCodec;
    unsigned audioTrack;
} streams[] = {
    {"videos/divx_file.avi", NEXUS_TransportType_eAvi, NEXUS_VideoCodec_eDivx311, 1, NEXUS_AudioCodec_eMp3, 2},
    {"videos/mp4_file.mp4", NEXUS_TransportType_eMp4, NEXUS_VideoCodec_eH264, 1, NEXUS_AudioCodec_eAac, 2},
    {"videos/asf_file.asf", NEXUS_TransportType_eAsf, NEXUS_VideoCodec_eVc1, 2, NEXUS_AudioCodec_eWmaStd, 1}
};

int main(int argc, char **argv)  {
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    NEXUS_FilePlayHandle fileNexus, file;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_VideoCodec video_codec;
    struct NEXUS_FilePlay fileLocal;
    struct b_test_slow_fileio_read slow_file;
    const struct stream_info *info = &streams[2];
    const char *fname = info->fname;

    if (argc > 1) fname = argv[1];

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    playback = NEXUS_Playback_Create();
    assert(playback);

    fileNexus = NEXUS_FilePlay_OpenPosix(fname, fname);
    if (!fileNexus) {
        fprintf(stderr, "can't open file:%s\n", fname);
        return -1;
    }
    fileLocal = *fileNexus;
    file = &fileLocal;
    fileLocal.file.data = b_test_slow_read_attach(&slow_file, fileNexus->file.data, 10);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_Playback_GetSettings(playback, &playbackSettings);
    playbackSettings.playpump = playpump;
    playbackSettings.playpumpSettings.transportType = info->type;
    playbackSettings.stcChannel = stcChannel;
    NEXUS_Playback_SetSettings(playback, &playbackSettings);

    /* Bring up audio decoders and outputs */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));

    /* Bring up video display and outputs */
    display = NEXUS_Display_Open(0, NULL);
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

    /* bring up decoder and connect to display */
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    video_codec = info->videoCodec;

    /* Open the audio and video pid channels */
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = video_codec;
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.decoder = videoDecoder;
    /* 1 is the AVI stream id for video. See above for deriving this. */
    videoPidChannel = NEXUS_Playback_OpenPidChannel(playback, info->videoTrack, &playbackPidSettings);

    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eAudio;
    /* 2 is the AVI stream id for audio. See above for deriving this. */
    audioPidChannel = NEXUS_Playback_OpenPidChannel(playback, info->audioTrack, &playbackPidSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = video_codec;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = info->audioCodec;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Start decoders */
    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

    /* Start playback */
    NEXUS_Playback_Start(playback, file, NULL);

    /* Playback state machine is driven from inside Nexus. */
    printf("Press ENTER to quit\n");
    getchar();

    /* Bring down system */
    NEXUS_VideoDecoder_Stop(videoDecoder);
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_Playback_Stop(playback);
    NEXUS_FilePlay_Close(fileNexus);
    NEXUS_Playback_Destroy(playback);
    NEXUS_Playpump_Close(playpump);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_Platform_Uninit();

#endif
    return 0;
}
