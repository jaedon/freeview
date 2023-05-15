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
 * $brcm_Workfile: fcc_demo.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 7/28/10 4:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/fcc_demo.c $
 * 
 * Hydra_Software_Devel/3   7/28/10 4:50p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/2   8/20/09 10:44a erickson
 * PR54880: handle ir key repeats
 *
 * Hydra_Software_Devel/1   8/12/09 4:42p erickson
 * PR54880: add fcc demo apps
 *
 * 3   7/30/09 10:54a erickson
 * PR54880: enable TSM
 *
 * 2   6/29/09 3:15p erickson
 * PR54880: add #include
 *
 * 1   5/19/09 10:31a erickson
 * PR54880: add video_primer
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_primer.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_stc_channel.h"
#include "nexus_timebase.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_ir_input.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

#define TOTAL_PRIMERS 3 
unsigned short g_pid[TOTAL_PRIMERS] = {0x11, 0x21, 0x41};

static void ir_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(void)
{
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderPrimerHandle primer[TOTAL_PRIMERS];
    NEXUS_VideoDecoderStartSettings videoProgram[TOTAL_PRIMERS];
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Timebase timebase = NEXUS_Timebase_e0;
    unsigned index = 0;
    unsigned i;
    NEXUS_IrInputHandle irHandle;
    NEXUS_IrInputSettings irSettings;
    BKNI_EventHandle event;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex=0;
    ipsOpenSettings.remuxIndex=0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* Open the pid channels */
    for (i=0;i<TOTAL_PRIMERS;i++) {
        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[i]);
        videoProgram[i].codec = NEXUS_VideoCodec_eMpeg2;
        videoProgram[i].pidChannel = NEXUS_PidChannel_Open(parserBand, g_pid[i], NULL);

        NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
        stcSettings.autoConfigTimebase = false; /* must do it manually */
        stcSettings.timebase = timebase;
        stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
        stcSettings.modeSettings.pcr.pidChannel = videoProgram[i].pidChannel;
        stcSettings.modeSettings.pcr.offsetThreshold = 0;
        stcSettings.stcIndex = 0;
#ifdef IP_STREAMER_SUBST
        IpsAddStcSettings(ips, &stcSettings);
#endif
        videoProgram[i].stcChannel = NEXUS_StcChannel_Open(i, &stcSettings);
    }

    /* bring up display */
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");
#else
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = NULL;
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif


    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    for (i=0;i<TOTAL_PRIMERS;i++) {
        primer[i] = NEXUS_VideoDecoder_OpenPrimer(videoDecoder);
        NEXUS_VideoDecoder_StartPrimer(videoDecoder, primer[i], &videoProgram[i]);
    }

    BKNI_CreateEvent(&event);

    NEXUS_IrInput_GetDefaultSettings(&irSettings);
    irSettings.mode = NEXUS_IrInputMode_eRemoteA;
    irSettings.dataReady.callback = ir_callback;
    irSettings.dataReady.context = event;
    irHandle = NEXUS_IrInput_Open(0, &irSettings);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = videoDecoder;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif
    
    while (1) {
        NEXUS_TimebaseSettings timebaseSettings;

        NEXUS_Timebase_GetSettings(timebase, &timebaseSettings);
        timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
        timebaseSettings.sourceSettings.pcr.pidChannel = videoProgram[index].pidChannel;
        timebaseSettings.sourceSettings.pcr.maxPcrError = 0xff;
        timebaseSettings.sourceSettings.pcr.trackRange = NEXUS_TimebaseTrackRange_e61ppm;
#ifdef IP_STREAMER_SUBST
        IpsAddTimebaseSettings(ips, &timebaseSettings);
#endif
        NEXUS_Timebase_SetSettings(timebase, &timebaseSettings);

        NEXUS_VideoDecoder_StartDecodeWithPrimer(videoDecoder, primer[index]);

        while (1) {
            NEXUS_IrInputEvent irEvent;
            unsigned num;
            bool overflow;

            NEXUS_IrInput_GetEvents(irHandle, &irEvent, 1, &num, &overflow);
            if (num == 0) {
                BKNI_WaitForEvent(event, 0xFFFFFFFF);
            }
            else if (!irEvent.repeat) {
                break;
            }
        }

        NEXUS_VideoDecoder_Stop(videoDecoder);
        NEXUS_VideoDecoder_StartPrimer(videoDecoder, primer[index], &videoProgram[index]);

        if (++index == TOTAL_PRIMERS) index = 0;
    }

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    return 0;
}
