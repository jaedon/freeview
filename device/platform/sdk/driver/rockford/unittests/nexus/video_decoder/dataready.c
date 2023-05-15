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
 * $brcm_Workfile: dataready.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/dataready.c $
 * 
 * Hydra_Software_Devel/8   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/* Nexus example app: capture userdata from video decoder */

#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_parser_band.h"

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

unsigned callback_count = 0;

void dataready(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    NEXUS_VideoDecoderExtendedStatus exStatus;
    NEXUS_Error rc;

    BSTD_UNUSED(param);
    ++callback_count;
    rc = NEXUS_VideoDecoder_GetExtendedStatus(videoDecoder, &exStatus);
    BDBG_ASSERT(!rc);
#if 0
    printf("status count %d, callback count %d\n", exStatus.dataReadyCount, callback_count);
#endif
    /* These two counts will naturally differ because XVD will fire dataReady interrupts even when not started,
    but we will clear NEXUS_VideoDecoderExtendedStatus.dataReadyCount when started.
    But during normal decode, they will generally increment at the same rate.
    Under heavy system load, it's possible that NEXUS_VideoDecoderExtendedStatus.dataReadyCount may increase by one than
    with a single callback. If an application is doing timing work, it can take this into account. */
}

int main(void)
{
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformConfiguration platformConfig;
    unsigned i,j;
    BERR_Code rc;
    unsigned loops = 10;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    for (i=0;i<loops;i++) {
        NEXUS_VideoDecoderStatus status;
        NEXUS_VideoDecoderExtendedSettings exSettings;

        /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
        NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
        parserBandSettings.transportType = NEXUS_TransportType_eTs;
        rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
        BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
        ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
        ipsOpenSettings.playpumpIndex=0;
        ipsOpenSettings.remuxIndex=0;
        ips = IpsOpen(&ipsOpenSettings);
#endif

        /* bring up display */
#if NEXUS_DTV_PLATFORM
        display = NEXUS_Display_Open(0, NULL);
        NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
        NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
        display = NEXUS_Display_Open(0, NULL);
#if NEXUS_NUM_COMPONENT_OUTPUTS
        rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
        BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
        rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
        BDBG_ASSERT(!rc);
#endif	
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
        vdecode = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);

        NEXUS_VideoDecoder_GetExtendedSettings(vdecode, &exSettings);
        exSettings.dataReadyCallback.callback = dataready;
        exSettings.dataReadyCallback.context = vdecode;
        rc = NEXUS_VideoDecoder_SetExtendedSettings(vdecode, &exSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
        BDBG_ASSERT(!rc);

#ifdef IP_STREAMER_SUBST
        ipsStartSettings.vDecoder = vdecode;
        ipsStartSettings.aDecoder = NULL;
        ipsStartSettings.stcChannel = NULL;
        IpsStart(ips, &ipsStartSettings);
#endif

        for (j=0;j<3;j++) {
            NEXUS_StcChannelSettings stcSettings;
            NEXUS_VideoDecoderStartSettings videoProgram;
            unsigned sleeptime;

            NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
            videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
            videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);

            /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
            NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
            stcSettings.timebase = NEXUS_Timebase_e0;
            stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
            stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
#ifdef IP_STREAMER_SUBST
            IpsAddStcSettings(ips, &stcSettings);
#endif
            videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

            callback_count = 0;

            rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
            BDBG_ASSERT(!rc);
            printf("started decode %d, %d\n", i, j);

            sleeptime = rand() % 5000;
            BKNI_Sleep(sleeptime);

            rc = NEXUS_VideoDecoder_GetStatus(vdecode, &status);
            BDBG_ASSERT(!rc);
            printf("status %d/%d cdb, %d pic, %d cabac\n",
                status.fifoDepth, status.fifoSize, status.queueDepth, status.cabacBinDepth);

            /* we should get between 24 and 60 per second,
            but this doesn't doesn't have to be exact */
            printf("got %d callbacks in %d msec\n", callback_count, sleeptime);
            BDBG_ASSERT(callback_count > sleeptime * 24 / 1000);

            NEXUS_VideoDecoder_Stop(vdecode);
            NEXUS_StcChannel_Close(videoProgram.stcChannel);
            NEXUS_PidChannel_Close(videoProgram.pidChannel);

            BKNI_Sleep(100);
        }

        NEXUS_VideoWindow_RemoveAllInputs(window);
        NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
        NEXUS_VideoDecoder_Close(vdecode);
        NEXUS_Display_Close(display);
#ifdef IP_STREAMER_SUBST
        IpsClose(ips);
#endif
    }

    NEXUS_Platform_Uninit();
    return 0;
}
