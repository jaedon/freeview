/***************************************************************************
 *     (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: driver.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 5/10/12 4:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/linuxkernel/driver.c $
 * 
 * 3   5/10/12 4:46p erickson
 * SW7231-800: update
 * 
 * 2   1/30/12 3:23p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 1   5/12/11 1:16p erickson
 * SW7420-1819: move linuxkernel_example into subdir and reimplement with
 *  kbuild
 * 
 * 5   1/7/11 3:21p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 *  PTS - STC difference
 * 
 * 4   9/11/09 2:30p erickson
 * SW7420-228: convert to autoconf.h
 *
 * 3   9/1/09 9:49a erickson
 * SWDEPRECATED-3858: added stub functions for platform linuxkernel code
 *
 * 2   12/18/08 1:56p erickson
 * PR48694: added comment to clarify purpose of app
 *
 * 1   11/5/08 12:19p vsilyaev
 * PR 48694: Example application to boot strap nexus in the LinuxKernel
 *  mode
 *
 ************************************************************/
#include "bstd.h"
#include "bkni.h"
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif
#include <linux/module.h>
#include <linux/fs.h>
#include "nexus_base.h"
#include "nexus_generic_driver_impl.h"

BDBG_MODULE(linuxkernel_main);

/* the following define the input and its characteristics -- these will vary by input */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define AUDIO_CODEC NEXUS_AudioCodec_eAc3
#define VIDEO_PID 0x11
#define AUDIO_PID 0x14

/**
This example application demonstrates how nexus can be run in kernel mode
with no proxy. This code can be compiled as a linux module. It is an example and has no API.

You can build and run as follows:

    # on build server
    cd nexus/examples/linuxkernel
    export B_REFSW_OS=linuxkernel
    make

    # on settop/DTV
    insmod linuxkernel_example.ko

Please read nexus/docs/Nexus_Usage.pdf for a description of various linux modes including
user mode, kernel mode with proxy and kernel mode without proxy.
**/


#ifdef MODULE
int     init_module(void);
void    cleanup_module(void);
#else
#error "Not supported"
#endif

int linuxkernel_main(int argc, const char *argv[]);

static char b_env[256]="";
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
module_param_string(config, b_env, sizeof(b_env), 0);
#endif

static void
b_parse_env(char *env)
{
    char *s;
    const char *name;
    const char *value;
    /* traverse  string, and split it to name/value pairs */
    for(s=env, name=env, value=NULL;;s++) {
        switch(*s) {
        case '\0':
            goto done;
        case '=':
            *s = '\0';
            value = s+1;
            break;
        case ' ':
        case ':':
        case ';':
            *s = '\0';
            if (value==NULL) {
                value=s;
            }
            NEXUS_SetEnv(name,value);
            name = s+1;
            value = NULL;
            break;
        default:
            break;
        }
    }
done:
    if(*name) {
        if (value==NULL) {
            value=s;
        }
        NEXUS_SetEnv(name,value);
    }
    return;
}

struct nexus_driver_state nexus_driver_state;

static int
nexus_driver_dev_init(void)
{
    int rc;
    unsigned i;
    struct nexus_generic_driver_init_settings settings;

    rc = BKNI_Init();
    if (rc) return -ENODEV;
    rc = BDBG_Init();
    if (rc) return -ENODEV;
    rc = NEXUS_Base_Core_Init();
    if (rc) return -ENODEV;
    
    memset(&settings, 0, sizeof(settings));
    /* populate nexus_driver_state as required by platform code */
    for (i=0;i<sizeof(settings.region)/sizeof(settings.region[0]);i++) {
        unsigned long offset, size;
        rc = bmem_region_info(i, &offset, &size);
        if (rc) break;
        settings.region[i].offset = offset;
        settings.region[i].size = size;
    }
    
    /* instead of calling nexus_generic_driver_init(&settings), we assign directly */
    nexus_driver_state.settings = settings;

    return 0;
}

MODULE_LICENSE("Proprietary");

int
init_module(void)
{
    int rc;

    printk("init_module\n");
    rc = nexus_driver_dev_init();
    if(rc!=0) {return rc;}

    BDBG_WRN((">>nexus_driver_init"));
    b_parse_env(b_env);

    return linuxkernel_main(0, NULL);
}

void
cleanup_module(void)
{
    BDBG_WRN(("shutdown completed"));
    NEXUS_Base_Core_Uninit();
    BDBG_Uninit();
    BKNI_Uninit();
    return;
}

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "bstd.h"
#include "bkni.h"

int 
linuxkernel_main(int argc, const char *argv[])
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error rc;
#endif

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the audio and video pid channels */
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, AUDIO_PID, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up the audio outputs. */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.codec = AUDIO_CODEC;
    audioProgram.pidChannel = audioPidChannel;
    audioProgram.stcChannel = stcChannel;

    /* Bring up audio decoders and outputs */
    pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
    NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    }
    else
    {
        NEXUS_AudioOutput_AddInput(
            NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
            NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    }
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_AudioOutput_AddInput(
        NEXUS_HdmiOutput_GetAudioConnector(platformConfig.outputs.hdmi[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
#endif

    /* Bring up video display and outputs */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
#if NEXUS_NUM_COMPONENT_OUTPUTS
    displaySettings.format = NEXUS_VideoFormat_e720p;
#else
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
#endif
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
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

    /* bring up decoder and connect to display */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
    if ( audioProgram.codec == NEXUS_AudioCodec_eAc3 )
    {
        /* Only pass through AC3 */
        NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
    }

#if 1
    /* Print status while decoding */
    for (;;) {
        NEXUS_VideoDecoderStatus status;
        NEXUS_AudioDecoderStatus audioStatus;
        uint32_t stc;

        NEXUS_VideoDecoder_GetStatus(vdecode, &status);
        NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);
        printk("decode %.4dx%.4d, pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference, status.fifoSize?(status.fifoDepth*100)/status.fifoSize:0);
        NEXUS_AudioDecoder_GetStatus(pcmDecoder, &audioStatus);
        printk("audio0            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
            audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
        NEXUS_AudioDecoder_GetStatus(compressedDecoder, &audioStatus);
        if ( audioStatus.started )
        {
            printk("audio1            pts %#x, stc %#x (diff %d) fifo=%d%%\n",
                audioStatus.pts, stc, audioStatus.ptsStcDifference, audioStatus.fifoSize?(audioStatus.fifoDepth*100)/audioStatus.fifoSize:0);
        }
        BKNI_Sleep(1000);
    }
#else
    printk("Press ENTER to stop decode\n");
    BKNI_Sleep(10000);

    /* example shutdown */
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}

/* stub functions for platform linuxkernel code */
BERR_Code
nexus_driver_scheduler_init(void)
{
    /* no scheduler needed because non-proxy kernel code has direct callbacks */
    return 0;
}

void nexus_driver_scheduler_uninit(void)
{
}

void
nexus_driver_module_init_enum_cb(void *cntx, NEXUS_ModuleHandle handle, const char *name, const NEXUS_ModuleSettings *settings)
{
}

void
nexus_driver_module_uninit_enum_cb(void *cntx, NEXUS_ModuleHandle handle, const char *name, const NEXUS_ModuleSettings *settings)
{
}
