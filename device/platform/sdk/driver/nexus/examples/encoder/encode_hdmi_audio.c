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
 * $brcm_Workfile: encode_hdmi_audio.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 1/30/12 3:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/encoder/encode_hdmi_audio.c $
 * 
 * 4   1/30/12 3:15p rjlewis
 * SW7425-1136: added alternate main for non-supported platforms.
 * 
 * 3   1/27/12 5:55p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 2   4/17/11 12:54p jgarrett
 * SW7425-146, SW7425-165: Adding HDMI video
 * 
 * 1   4/16/11 10:51p hongtaoz
 * SW7425-146, SW7425-165: add hdmi input transcode examples with audio;
 *  add qam transcode;
 * 
 * 1   4/16/11 8:38p hongtaoz
 * SW7425-146: add hdmi input transcode examples;
 * 
 * 1   4/16/11 8:26p hongtaoz
 * SW7425-146: add hdmi input transcode examples;
 * 
 * 2   2/25/11 3:08p vsilyaev
 * SW7425-39, SW7425-40: Merged examples
 * 
 * SW7425_bringup/6   2/20/11 4:44p hongtaoz
 * SW7425-85: add AC3->AAC transcode test;
 * 
 * SW7425_bringup/5   2/7/11 9:09p hongtaoz
 * SW7425-41: adapted to the new API;
 * 
 * SW7425_bringup/4   1/28/11 6:06p hongtaoz
 * SW7425-41: added STC channel config to audio transcoder;
 * 
 * SW7425_bringup/3   1/19/11 8:08p hongtaoz
 * SW7425-41: init descs number in for loop;
 * 
 * SW7425_bringup/2   1/18/11 7:46p hongtaoz
 * SW7425-41: added audio descriptor capture to the test;
 * 
 * SW7425_bringup/1   1/17/11 11:21p hongtaoz
 * SW7425-41: add encode_audio example to transcode audio es output;
 * 
 * 2   1/3/11 11:14a jgarrett
 * SW7425-41: Fixing playpump assignment
 * 
 * 1   12/30/10 12:10p jgarrett
 * SW7425-41: Adding mux output example
 * 
 * 5   12/29/10 12:32p vsilyaev
 * SW7425-39: Updated shutdown sequence
 * 
 * 4   12/28/10 5:45p vsilyaev
 * SW7425-39: Open Audio/Video PidChannel inside the stream mux module
 * 
 * 3   12/16/10 12:39p vsilyaev
 * SW7425-39, SW7425-40: Merged into mainline
 * 
 * SW7425_bringup/2   12/15/10 10:51p hongtaoz
 * SW7425-39, SW7425-40: updated examples with missed calls and re-ordered
 *  encoder/display open sequence;
 * 
 * SW7425_bringup/1   12/15/10 5:34p hongtaoz
 * SW7425-39, SW7425-40: updated default encoder display index and encoder
 *  settings for 7425 bringup; fixed typo for PCR playpump PID channel
 *  open;
 * 
 * 2   12/13/10 6:59p vsilyaev
 * SW7425-39: Added support for MUX input
 * 
 * 1   12/13/10 3:37p vsilyaev
 * SW7425-40: Video encoder examples
 * 
 *
 *****************************************************************************/
/* Nexus example app: playback and decode */

#include "nexus_platform.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_spdif_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#if NEXUS_NUM_HDMI_INPUTS && NEXUS_HAS_AUDIO_MUX_OUTPUT
#include "nexus_hdmi_input.h"
#include "nexus_audio_mux_output.h"
#include "nexus_audio_encoder.h"
#include "nexus_audio_decoder.h"
#endif

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

#if !(NEXUS_NUM_HDMI_INPUTS && NEXUS_HAS_AUDIO_MUX_OUTPUT)
int main(void)
{
	printf("This application is not supported on this platform!\n");
	return 0;
}
#else

void source_changed(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    printf("source changed\n");
}

void avmute_changed(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    printf("avmute changed\n");
}

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_StcChannelHandle stcChannel, stcChannelTranscode;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif

    NEXUS_HdmiInputHandle hdmiInput;
    NEXUS_HdmiInputSettings hdmiInputSettings;
    NEXUS_TimebaseSettings timebaseSettings;
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;

    NEXUS_AudioMuxOutputHandle audioMuxOutput;
    NEXUS_AudioMuxOutputStatus audioMuxStatus;
    NEXUS_AudioMuxOutputStartSettings audioMuxStartSettings;
    NEXUS_AudioEncoderSettings encoderSettings;
    NEXUS_AudioEncoderHandle audioEncoder;
    NEXUS_VideoWindowHandle videoWindow;
    size_t bytes;
    FILE *fout, *fdesc;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Timebase_GetSettings(NEXUS_Timebase_e0, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_eHdDviIn;
    NEXUS_Timebase_SetSettings(NEXUS_Timebase_e0, &timebaseSettings);

    NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
    hdmiInputSettings.timebase = NEXUS_Timebase_e0;
    hdmiInputSettings.sourceChanged.callback = source_changed;
    hdmiInputSettings.avMuteChanged.callback = avmute_changed;
    hdmiInput = NEXUS_HdmiInput_Open(0, &hdmiInputSettings);
    if(!hdmiInput) {
        fprintf(stderr, "Can't get hdmi input\n");
        return -1;
    }

    /* encoders/mux require different STC broadcast mode from decoder */
    NEXUS_StcChannel_GetDefaultSettings(1, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;/* should be the same timebase for end-to-end locking */
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcSettings.pcrBits = NEXUS_StcChannel_PcrBits_eFull42;/* ViCE2 requires 42-bit STC broadcast */
    stcChannelTranscode = NEXUS_StcChannel_Open(1, &stcSettings);

    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e480p;
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
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

    videoWindow = NEXUS_VideoWindow_Open(display, 0);
    assert(videoWindow);

    NEXUS_VideoWindow_AddInput(videoWindow, NEXUS_HdmiInput_GetVideoConnector(hdmiInput));

    /* Open the audio decoder */
    audioDecoder = NEXUS_AudioDecoder_Open(0, NULL);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.input = NEXUS_HdmiInput_GetAudioConnector(hdmiInput);
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.autoConfigTimebase = false;
    audioProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Connect audio decoders to outputs */
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif

    /* Open audio mux output */
    audioMuxOutput = NEXUS_AudioMuxOutput_Create(NULL);
    assert(audioMuxOutput);

    /* Open audio encoder */
    NEXUS_AudioEncoder_GetDefaultSettings(&encoderSettings);
    encoderSettings.codec = NEXUS_AudioCodec_eAac;
    audioEncoder = NEXUS_AudioEncoder_Open(&encoderSettings);
    assert(audioEncoder);

    /* Connect encoder to decoder */
    NEXUS_AudioEncoder_AddInput(audioEncoder,
        NEXUS_AudioDecoder_GetConnector(audioDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    /* Connect mux to encoder */
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioMuxOutput_GetConnector(audioMuxOutput), NEXUS_AudioEncoder_GetConnector(audioEncoder));

    /* Start audio mux output */
    NEXUS_AudioMuxOutput_GetDefaultStartSettings(&audioMuxStartSettings);
    audioMuxStartSettings.stcChannel = stcChannelTranscode;
    NEXUS_AudioMuxOutput_Start(audioMuxOutput, &audioMuxStartSettings);

    /* Start decoders */
    NEXUS_AudioDecoder_Start(audioDecoder, &audioProgram);

    NEXUS_AudioMuxOutput_GetStatus(audioMuxOutput, &audioMuxStatus);
    fprintf(stderr, "audioMuxStatus.pBufferBase: 0x%p\n", audioMuxStatus.pBufferBase);

    /* open file for ES capture */
    fprintf(stderr, "\n\tencode hdmi audio file: videos/hdmi_audio.aac\n\n");
    fout = fopen("videos/hdmi_audio.aac","wb");
    fdesc = fopen("videos/audio_desc.txt","w");
    fprintf(fdesc, "flags(h) origPts(h)         pts(Lh)     escr(h) ticksPerBit(u) shr(d) offset(h) length(h) protocol(u)\n");
    for(;;) {
        size_t size[2];
        const NEXUS_AudioMuxOutputFrame *desc[2];
        unsigned i,j;
        unsigned descs;
        
        NEXUS_AudioMuxOutput_GetBuffer(audioMuxOutput, &desc[0], &size[0], &desc[1], &size[1]);
        if(size[0]==0 && size[1]==0) {
            NEXUS_AudioDecoderStatus astatus;

            NEXUS_AudioDecoder_GetStatus(audioDecoder, &astatus);
            fflush(fout);
            fprintf(stderr, "written %u bytes.... decode:%u\t\r", bytes, astatus.pts);
            BKNI_Sleep(30);
            continue;
        }
        for(descs=0,j=0;j<2;j++) {
            descs+=size[j];
            for(i=0;i<size[j];i++) {
                if(desc[j][i].length > 0)
                {
                    fwrite((const uint8_t *)audioMuxStatus.pBufferBase + desc[j][i].offset, desc[j][i].length, 1, fout);
                    fprintf(fdesc, "%8x %8x   %08x%08x %8x %5u %5d %8x %8x\n", desc[j][i].flags, desc[j][i].originalPts, 
                        (uint32_t)(desc[j][i].pts>>32), (uint32_t)(desc[j][i].pts & 0xffffffff), desc[j][i].escr, 
                        desc[j][i].ticksPerBit, desc[j][i].shr, desc[j][i].offset, desc[j][i].length);
                }
                bytes+= desc[j][i].length;
            }
        }

        NEXUS_AudioMuxOutput_ReadComplete(audioMuxOutput, descs);
        fflush(fout);
        fflush(fdesc);
    }

    /* Playback state machine is driven from inside Nexus. */
    printf("Press ENTER to quit\n");
    getchar();

    /* Bring down system */
    NEXUS_AudioDecoder_Stop(audioDecoder);
    NEXUS_AudioMuxOutput_Destroy(audioMuxOutput);
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(stcChannel);
    
    NEXUS_AudioDecoder_Close(audioDecoder);
    NEXUS_Platform_Uninit();

    return 0;
}

#endif


