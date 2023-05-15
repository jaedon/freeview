/***************************************************************************
*     (c)2004-2008 Broadcom Corporation
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
* $brcm_Workfile: analog_digital_channel_change.c $
* $brcm_Revision: Hydra_Software_Devel/12 $
* $brcm_Date: 1/12/11 2:41p $
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/general/analog_digital_channel_change.c $
* 
* Hydra_Software_Devel/12   1/12/11 2:41p mward
* SW7422-2:  NEXUS_Memory_GetStatus() eliminated.  Use
* NEXUS_Heap_GetStatus().
* 
* Hydra_Software_Devel/11   7/29/10 11:11a calvinho
* SW7405-4653: Added Ip Streamer input support
* 
* Hydra_Software_Devel/10   6/1/09 12:00p erickson
* PR42679: fix app
*
* Hydra_Software_Devel/9   1/12/09 3:45p erickson
* PR42679: update
*
* Hydra_Software_Devel/8   12/15/08 12:02p erickson
* PR47030: add additional buffer for digital
*
* Hydra_Software_Devel/7   11/4/08 4:26p erickson
* PR47030: must reset heap pointer
*
* Hydra_Software_Devel/6   11/4/08 12:39p erickson
* PR47030: added required Shutdown calls
*
* Hydra_Software_Devel/5   11/3/08 2:26p erickson
* PR47030: added hdmi input heap
*
* Hydra_Software_Devel/4   10/30/08 9:06p erickson
* PR47030: rework analog/digital memory sharing
*
* Hydra_Software_Devel/3   10/17/08 3:21p erickson
* PR47030: change NEXUS_Memory_GetStatus, added comment
*
* Hydra_Software_Devel/2   10/17/08 3:06p erickson
* PR47030: set explicit sharedAnalogDigitalMemory size
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_parser_band.h"
#include "nexus_video_adj.h"
#include "nexus_memory.h"
#if NEXUS_NUM_HDMI_INPUTS
#include "nexus_hdmi_input.h"
#endif
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_analog_video_decoder.h"
#endif

#if NEXUS_DTV_PLATFORM
#define USE_SHARED_ANALOG_DIGITAL_HEAP 1
#endif

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
    NEXUS_VideoDecoderHandle vdecode;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoderHandle adecode;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
#endif
    NEXUS_VideoInput videoInput;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_MemoryStatus memStatus;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_HeapHandle sharedAnalogDigitalHeap = NULL;
    NEXUS_VideoWindowAnrSettings anrSettings;
#if NEXUS_NUM_HDMI_INPUTS
    NEXUS_HdmiInputHandle hdmiInput;
    NEXUS_HdmiInputSettings hdmiInputSettings;
#endif
#if USE_SHARED_ANALOG_DIGITAL_HEAP
    NEXUS_DisplayHeapSettings analogHeapSettings, digitalHeapSettings;
#endif
    unsigned i;
    BERR_Code rc;
    unsigned loops = 20;

#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);

    /* This is an example of how to do shared analog/digital heap. Change the next line to #if 0 to disable sharing. */
#if USE_SHARED_ANALOG_DIGITAL_HEAP
    /* First, VideoDecoder must be given enough general heap space to do still decoder.
    This assumes we are doing still decode in the background while analog decode is happening in main.
    These numbers come from 3548_Memory_Worksheet.xls. */
    platformSettings.videoDecoderModuleSettings.heapSize.general = 4 * 1024 * 1024;
    platformSettings.videoDecoderModuleSettings.heapSize.secure = 0;  /* not using SVP */
    platformSettings.videoDecoderModuleSettings.heapSize.picture = 0; /* only for non-UMA */

    /* Next, create a second heap for shared analog/digital memory. 3548_Memory_Worksheet.xls says it should be 64 MB for config1. */
    platformSettings.memc[0].heapSize[1] = 64 * 1024 * 1024;

    /* The remainder goes to the general heap */
    platformSettings.memc[0].heapSize[0] = 128 * 1024 * 1024;

    /* Create different heap settings for analog and digital */
    NEXUS_Heap_GetDefaultDisplayHeapSettings(&analogHeapSettings);
    analogHeapSettings.fullHdBuffers.count = 4;
    analogHeapSettings.fullHdBuffers.pixelFormat = NEXUS_PixelFormat_eX2_Cr10_Y10_Cb10; /* 10bit 444 */

    NEXUS_Heap_GetDefaultDisplayHeapSettings(&digitalHeapSettings);
    digitalHeapSettings.fullHdBuffers.count = 5;
    digitalHeapSettings.hdBuffers.count = 1;
    digitalHeapSettings.hdBuffers.pixelFormat = NEXUS_PixelFormat_eYCbCr422_10bit; /* 10bit 422 */

    /* Don't allocate a display module heap. This app is exclusively per-window heap. */
    platformSettings.displayModuleSettings.fullHdBuffers.count = 0;
    platformSettings.displayModuleSettings.hdBuffers.count = 0;
    platformSettings.displayModuleSettings.sdBuffers.count = 0;
#endif

    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);
#if USE_SHARED_ANALOG_DIGITAL_HEAP
    sharedAnalogDigitalHeap = platformConfig.heap[1];
#endif

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);
    rc = NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    BDBG_ASSERT(!rc);
#else
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
    window = NEXUS_VideoWindow_Open(display, 0);

#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = NULL;
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    /* set analog/digital heap for this window */
    NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    windowSettings.heap = sharedAnalogDigitalHeap;
    rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
    BDBG_ASSERT(!rc);

    NEXUS_VideoWindow_GetAnrSettings(window, &anrSettings);
    anrSettings.anr.mode = NEXUS_VideoWindowFilterMode_eEnable;
    rc = NEXUS_VideoWindow_SetAnrSettings(window, &anrSettings);
    BDBG_ASSERT(!rc);

    /* open decoders at init time */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
    videoDecoderOpenSettings.heap = sharedAnalogDigitalHeap;
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    vdecode = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    /* set analog/digital heap for this source */
    NEXUS_AnalogVideoDecoder_GetDefaultSettings(&analogVideoDecoderSettings);
    analogVideoDecoderSettings.heap = sharedAnalogDigitalHeap;
    adecode = NEXUS_AnalogVideoDecoder_Open(0, &analogVideoDecoderSettings);
#endif

#if NEXUS_NUM_HDMI_INPUTS
    NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
    hdmiInputSettings.heap = sharedAnalogDigitalHeap;
    hdmiInput = NEXUS_HdmiInput_Open(0, &hdmiInputSettings);
#endif

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    /* have to set analog heap before connecting videoInput to composite[0] */
    if (sharedAnalogDigitalHeap) {
        rc = NEXUS_Heap_SetDisplayHeapSettings(sharedAnalogDigitalHeap, &analogHeapSettings);
        BDBG_ASSERT(!rc);
    }

    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
    analogVideoDecoderSettings.heap = sharedAnalogDigitalHeap; /* must reset the heap pointer here */
    /* TODO: consider having NEXUS_AnalogVideoDecoderOpenSettings so that app doesn't have to reset the heap
    on every NEXUS_AnalogVideoDecoder_SetSettings call. */
    rc = NEXUS_AnalogVideoDecoder_SetSettings(adecode, &analogVideoDecoderSettings);
    BDBG_ASSERT(!rc);
#endif

#if USE_SHARED_ANALOG_DIGITAL_HEAP
    rc = NEXUS_Heap_GetStatus(sharedAnalogDigitalHeap,&memStatus);
    BDBG_ASSERT(!rc);
#else
    rc = NEXUS_Heap_GetStatus(platformConfig.heap[0],&memStatus);
    BDBG_ASSERT(!rc);
#endif
    printf("Heap size=%d, free=%d\n", memStatus.size, memStatus.free);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = vdecode;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    /* digital channel change could be only Stop/Start or could include RemoveInput/AddInput.
    these can occur in any combination. */
    for (i=0;i<loops;i++) {
        NEXUS_StcChannelSettings stcSettings;
        NEXUS_VideoDecoderStartSettings videoProgram;

        switch (rand() % 3) {
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
        case 0:
#if USE_SHARED_ANALOG_DIGITAL_HEAP
            /* this must be called before NEXUS_VideoWindow_AddInput */
            rc = NEXUS_Heap_SetDisplayHeapSettings(sharedAnalogDigitalHeap, &analogHeapSettings);
            BDBG_ASSERT(!rc);
#endif
            videoInput = NEXUS_AnalogVideoDecoder_GetConnector(adecode);
            /* This shutdown is required so that the heap can be recreated */
            NEXUS_VideoInput_Shutdown(videoInput);
            rc = NEXUS_VideoWindow_AddInput(window, videoInput);
            BDBG_ASSERT(!rc);

            BKNI_Sleep(rand() % 4000);
            printf("analog decode\n");

            NEXUS_VideoWindow_RemoveAllInputs(window);
            NEXUS_VideoInput_Shutdown(videoInput);
            break;
#endif

        case 1:
#if USE_SHARED_ANALOG_DIGITAL_HEAP
            /* this must be called before NEXUS_VideoWindow_AddInput */
            rc = NEXUS_Heap_SetDisplayHeapSettings(sharedAnalogDigitalHeap, &digitalHeapSettings);
            BDBG_ASSERT(!rc);
#endif
            videoInput = NEXUS_VideoDecoder_GetConnector(vdecode);
            /* This shutdown is required so that the heap can be recreated */
            NEXUS_VideoInput_Shutdown(videoInput);
            rc = NEXUS_VideoWindow_AddInput(window, videoInput);
            BDBG_ASSERT(!rc);

            NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
            videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
            videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x31, NULL);

            /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
            NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
            stcSettings.timebase = NEXUS_Timebase_e0;
            stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
            stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
#ifdef IP_STREAMER_SUBST
            IpsAddStcSettings(ips, &stcSettings);
#endif
            videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

            rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
            BDBG_ASSERT(!rc);

            printf("digital decode\n");
            BKNI_Sleep(rand() % 4000);

            NEXUS_VideoDecoder_Stop(vdecode);
            NEXUS_StcChannel_Close(videoProgram.stcChannel);
            NEXUS_PidChannel_Close(videoProgram.pidChannel);
            NEXUS_VideoWindow_RemoveAllInputs(window);
            NEXUS_VideoInput_Shutdown(videoInput);
            break;

#if NEXUS_NUM_HDMI_INPUTS
        case 2:
#if USE_SHARED_ANALOG_DIGITAL_HEAP
            /* this must be called before NEXUS_VideoWindow_AddInput */
            rc = NEXUS_Heap_SetDisplayHeapSettings(sharedAnalogDigitalHeap, &analogHeapSettings);
            BDBG_ASSERT(!rc);
#endif
            videoInput = NEXUS_HdmiInput_GetVideoConnector(hdmiInput);
            /* This shutdown is required so that the heap can be recreated */
            NEXUS_VideoInput_Shutdown(videoInput);
            rc = NEXUS_VideoWindow_AddInput(window, videoInput);
            BDBG_ASSERT(!rc);

            BKNI_Sleep(rand() % 4000);
            printf("hdmi input\n");

            NEXUS_VideoWindow_RemoveAllInputs(window);
            NEXUS_VideoInput_Shutdown(videoInput);
            break;
#endif
        }
    }

#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoder_Close(adecode);
#endif
    NEXUS_VideoDecoder_Close(vdecode);
#if NEXUS_NUM_HDMI_INPUTS
    NEXUS_HdmiInput_Close(hdmiInput);
#endif
    NEXUS_Display_Close(display);

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    NEXUS_Platform_Uninit();
    return 0;
}
