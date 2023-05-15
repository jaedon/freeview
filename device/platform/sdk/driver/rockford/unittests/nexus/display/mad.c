/***************************************************************************
*     (c)2007-2009 Broadcom Corporation
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
* $brcm_Workfile: mad.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 7/29/10 10:59a $
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/mad.c $
* 
* Hydra_Software_Devel/2   7/29/10 10:59a calvinho
* SW7405-4653: Added Ip Streamer input support
* 
* Hydra_Software_Devel/1   2/10/10 2:34p erickson
* SW7405-3829: add mad stress test
*
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_video_adj.h"
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

BDBG_MODULE(mad);

int main(void)
{
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    bool connected = false;
    BERR_Code rc;
    unsigned loops = 2000;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    srand(time(NULL));

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* bring up display */
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    display = NEXUS_Display_Open(0, NULL);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = platformConfig.outputs.audioDacs[0];
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);

    /* must connect before start */
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    BDBG_ASSERT(!rc);
    connected = true;

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);

    NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
    videoDecoderSettings.manualPowerState = true;
    rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
    BDBG_ASSERT(!rc);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = videoDecoder;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif
    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);

    while (--loops) {
        NEXUS_VideoWindowMadSettings madSettings;

        if (loops % 100 == 0) BDBG_WRN(("loops: %d", loops));

        switch (rand() % 4) {
        case 0:
            NEXUS_VideoWindow_GetMadSettings(window, &madSettings);
            madSettings.deinterlace = rand() % 2;
            madSettings.enable32Pulldown = rand() % 2;
            madSettings.enable22Pulldown = rand() % 2;

            /* only set game modes that apply to all chips */
            switch (rand() % 4) {
            case 0: madSettings.gameMode = NEXUS_VideoWindowGameMode_eOff; break;
            case 1: madSettings.gameMode = NEXUS_VideoWindowGameMode_e5Fields_1Delay; break;
            case 2: madSettings.gameMode = NEXUS_VideoWindowGameMode_e5Fields_0Delay; break;
            case 3: madSettings.gameMode = NEXUS_VideoWindowGameMode_e5Fields_ForceSpatial; break;
            }

            madSettings.pixelFormat = rand() % NEXUS_PixelFormat_eMax;
            madSettings.shrinkWidth = rand() % 2;
            rc = NEXUS_VideoWindow_SetMadSettings(window, &madSettings);
            BDBG_ASSERT(!rc);
            break;
        case 1:
            if (!connected) {
                rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
                BDBG_ASSERT(!rc);
                connected = true;
                BDBG_WRN(("AddInput"));
            }
            break;
        case 2:
            if (connected) {
                rc = NEXUS_VideoWindow_RemoveInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
                BDBG_ASSERT(!rc);
                connected = false;
                BDBG_WRN(("RemoveInput"));

                if (rand() % 2) {
                    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
                }
            }
            break;
        case 3:
            BKNI_Sleep(rand() % 2000);
            break;
        }
    }

    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_Display_Close(display);
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif    
    NEXUS_Platform_Uninit();
    return 0;
}
