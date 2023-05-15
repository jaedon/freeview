/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: mosaic_decode.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 12/21/11 1:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/mosaic_decode.c $
 * 
 * Hydra_Software_Devel/12   12/21/11 1:23p erickson
 * SW7425-1971: extend unittest that would have caught this bug
 * 
 * Hydra_Software_Devel/11   7/15/11 11:16a jtna
 * SW7420-1959: added clearRect test case
 * 
 * Hydra_Software_Devel/10   7/15/11 10:03a erickson
 * SW7420-1973: add missing break;
 * 
 * Hydra_Software_Devel/9   7/12/11 11:03a erickson
 * SW7420-1973: add test that would have caught this
 * 
 * Hydra_Software_Devel/8   7/11/11 1:07p erickson
 * SWDEPRECATED-2425: add display/heaps
 * 
 * Hydra_Software_Devel/7   6/3/11 12:16p erickson
 * SW7425-466: make PIP test chip-generic. convert to BKNI_Printf for
 * properly interleaved output. add srand for better coverage.
 * 
 * Hydra_Software_Devel/6   7/28/10 4:56p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/5   4/16/10 2:13p erickson
 * SWDEPRECATED-3044: fix warnings
 *
 * Hydra_Software_Devel/4   12/9/09 5:01p erickson
 * SW7405-3394: allow variable # of loops, clean up shutdown
 *
 * Hydra_Software_Devel/3   12/9/09 3:15p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * Hydra_Software_Devel/2   5/6/09 8:42a erickson
 * PR42679: fix for non-mosaic systems
 *
 * Hydra_Software_Devel/1   5/5/09 4:29p erickson
 * PR54844: add test for opening and starting mosaic channels in any order
 *
******************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_video_input.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#if NEXUS_NUM_MOSAIC_DECODES
#include "nexus_mosaic_video_decoder.h"
#include "nexus_mosaic_display.h"
#endif
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

BDBG_MODULE(mosaic_decode);

#define NUM_MOSAICS 6

#if NEXUS_NUM_VIDEO_WINDOWS > 1
#define TEST_PIP 1
#endif

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

int main(int argc, char **argv)
{
#if NEXUS_NUM_MOSAIC_DECODES
    NEXUS_VideoDecoderHandle mosaicDecoder[NUM_MOSAICS];
    bool mosaicDecoderStarted[NUM_MOSAICS];
    NEXUS_VideoDecoderStartSettings videoProgram[NUM_MOSAICS];
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoWindowHandle mosaicWindow[NUM_MOSAICS];
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
#if TEST_PIP
    NEXUS_VideoDecoderHandle pip = NULL;
    NEXUS_VideoWindowHandle pipWindow;
#endif
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif
    unsigned i;
    unsigned loops = 2000;
    NEXUS_Error rc;
    
    srand(time(NULL));

    if (argc > 1) loops = atoi(argv[1]);

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_Memset(mosaicDecoder, 0, sizeof(mosaicDecoder));
    BKNI_Memset(mosaicDecoderStarted, 0, sizeof(mosaicDecoderStarted));
    BKNI_Memset(mosaicWindow, 0, sizeof(mosaicWindow));

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex=0;
    ipsOpenSettings.remuxIndex=0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* bring up display */
    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);
#if TEST_PIP
    pipWindow = NEXUS_VideoWindow_Open(display, 1);
#endif
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = NULL; 
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    while (--loops) {
        unsigned i = rand() % NUM_MOSAICS;
        unsigned state = rand() % 11;

        switch (state) {
        case 0:
            if (!mosaicDecoder[i]) {
                NEXUS_VideoDecoderOpenMosaicSettings openSettings;

                BKNI_Printf("open mosaic decoder %d\n", i);
                NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[i]);
                videoProgram[i].codec = NEXUS_VideoCodec_eH264;
                videoProgram[i].pidChannel = NEXUS_PidChannel_Open(parserBand, 0x1001 + i, NULL);

                NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings(&openSettings);
                openSettings.maxWidth = 352; /* CIF */
                openSettings.maxHeight = 288; /* CIF */

                mosaicDecoder[i] = NEXUS_VideoDecoder_OpenMosaic(0, i, &openSettings);
                BDBG_ASSERT(mosaicDecoder[i]);
            }
            break;
        case 1:
            if (mosaicDecoder[i] && !mosaicWindow[i]) {
                BKNI_Printf("close mosaic decoder %d\n", i);
                NEXUS_VideoDecoder_Close(mosaicDecoder[i]);
                mosaicDecoder[i] = NULL;
                NEXUS_PidChannel_Close(videoProgram[i].pidChannel);
                videoProgram[i].pidChannel = NULL;
            }
            break;
        case 2:
            if (mosaicDecoder[i] && !mosaicWindow[i]) {
                NEXUS_VideoWindowSettings windowSettings;

                BKNI_Printf("open mosaic window %d\n", i);
                mosaicWindow[i] = NEXUS_VideoWindow_OpenMosaic(window, i);

                NEXUS_VideoWindow_GetSettings(mosaicWindow[i], &windowSettings);
                windowSettings.position.x = rand() % 620;
                windowSettings.position.y = rand() % 380;
                windowSettings.position.width = 50 + rand() % 50;
                windowSettings.position.height = 50 + rand() % 50;
                windowSettings.zorder = rand() % 16;
                rc = NEXUS_VideoWindow_SetSettings(mosaicWindow[i], &windowSettings);
                BDBG_ASSERT(!rc);

                rc = NEXUS_VideoWindow_AddInput(mosaicWindow[i], NEXUS_VideoDecoder_GetConnector(mosaicDecoder[i]));
                BDBG_ASSERT(!rc);
            }
            break;
        case 3:
            if (mosaicWindow[i] && !mosaicDecoderStarted[i]) {
                BKNI_Printf("close mosaic window %d\n", i);
                if (rand() % 2) {
                    NEXUS_VideoWindow_RemoveAllInputs(mosaicWindow[i]);
                    if (rand() % 2) {
                        NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(mosaicDecoder[i]));
                    }
                }
                NEXUS_VideoWindow_Close(mosaicWindow[i]);
                mosaicWindow[i] = NULL;
            }
            break;
        case 4:
            if (mosaicWindow[i] && !mosaicDecoderStarted[i]) {
                BKNI_Printf("start mosaic %d\n", i);
                rc = NEXUS_VideoDecoder_Start(mosaicDecoder[i], &videoProgram[i]);
                BDBG_ASSERT(!rc);
                mosaicDecoderStarted[i] = true;
            }
            break;
        case 5:
            if (mosaicDecoderStarted[i]) {
                BKNI_Printf("stop mosaic %d\n", i);
                NEXUS_VideoDecoder_Stop(mosaicDecoder[i]);
                mosaicDecoderStarted[i] = false;
            }
            break;
        case 6:
            if (mosaicWindow[i]) {
                NEXUS_VideoWindowSettings windowSettings;
                NEXUS_VideoWindow_GetSettings(mosaicWindow[i], &windowSettings);
                windowSettings.position.x = rand() % 620;
                windowSettings.position.y = rand() % 380;
                windowSettings.position.width = 50 + rand() % 50;
                windowSettings.position.height = 50 + rand() % 50;
                windowSettings.zorder = rand() % 24;
                windowSettings.visible = rand() % 2;
                rc = NEXUS_VideoWindow_SetSettings(mosaicWindow[i], &windowSettings);
                BDBG_ASSERT(!rc);
            }
            break;
#if TEST_PIP
        case 7:
            if (pip) {
                BKNI_Printf("close pip\n");
                NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(pip));
                NEXUS_VideoWindow_RemoveAllInputs(pipWindow);
                NEXUS_VideoDecoder_Close(pip);
                pip = NULL;
            }
            else {
                BKNI_Printf("open pip\n");
                pip = NEXUS_VideoDecoder_Open(1, NULL);
                rc = NEXUS_VideoWindow_AddInput(pipWindow, NEXUS_VideoDecoder_GetConnector(pip));
                BDBG_ASSERT(!rc);
            }
            break;
#endif
        case 8:
            for (i=0;i<NUM_MOSAICS;i++) {
                if (mosaicWindow[i]) break;
            }
            if (i == NUM_MOSAICS) {
                /* window is not in mosaic mode */
                NEXUS_VideoWindowSettings windowSettings;
                NEXUS_VideoWindow_GetSettings(window, &windowSettings);
                windowSettings.visible = rand() % 2;
                rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
                BDBG_ASSERT(!rc);
            }
            else if (rand() % 10 == 0) {
                /* window is in mosaic mode. try to set settings on the parent. */
                NEXUS_VideoWindowSettings windowSettings;
                NEXUS_VideoWindow_GetSettings(window, &windowSettings);
                windowSettings.visible = rand() % 2;
                rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
                BDBG_ASSERT(rc); /* expected failure */
            }    
            break;
        case 9:
            for (i=0;i<NUM_MOSAICS;i++) {
                if (mosaicWindow[i]) break;
            }
            BKNI_Printf("clearRect (mosaic mode %d)\n", (i == NUM_MOSAICS));
            if (i == NUM_MOSAICS) {
                /* window is not in mosaic mode */
                NEXUS_VideoWindowSettings windowSettings;
                NEXUS_VideoWindow_GetSettings(window, &windowSettings);
                windowSettings.clearRect.enabled = rand() % 2;
                windowSettings.clearRect.position.x = rand() % 620;
                windowSettings.clearRect.position.y = rand() % 380;
                windowSettings.clearRect.position.width = 50 + rand() % 50;
                windowSettings.clearRect.position.height = 50 + rand() % 50;
                windowSettings.clearRect.color = 0xff104080;
                rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
                BDBG_ASSERT(!rc);
            }
            break;
        case 10:
            if (rand() % 20 == 0) {
                /* close all mosaic windows and use each mosaic decoder with main window. */
                for (i=0;i<NUM_MOSAICS;i++) {
                    if (mosaicWindow[i]) {
                        NEXUS_VideoWindow_Close(mosaicWindow[i]);
                        mosaicWindow[i] = NULL;
                        mosaicDecoderStarted[i] = false;
                    }
                }
                for (i=0;i<NUM_MOSAICS;i++) {
                    if (mosaicDecoder[i]) {
                        /* it's more likely to connect the main decoder to the main window, but this works. 
                        this verifies that internal mosaic state was shutdown correctly. */
                        BKNI_Printf("connect/disconnect decoder[%d] to main window\n", i);
                        rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(mosaicDecoder[i]));
                        BDBG_ASSERT(!rc);
                        rc = NEXUS_VideoWindow_RemoveInput(window, NEXUS_VideoDecoder_GetConnector(mosaicDecoder[i]));
                        BDBG_ASSERT(!rc);
                        /* only one mosaic can be connected at a time to non-mosaic window. */
                        NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(mosaicDecoder[i]));
                    }
                }
            }
            break;
        default:
            break;
        }

        BKNI_Sleep(rand() % 100);
    }

    BDBG_WRN(("shutdown"));
    for (i=0;i<NUM_MOSAICS;i++) {
        if (mosaicDecoderStarted[i]) {
            NEXUS_VideoDecoder_Stop(mosaicDecoder[i]);
        }
        if (mosaicWindow[i]) {
            NEXUS_VideoWindow_RemoveAllInputs(mosaicWindow[i]);
            NEXUS_VideoWindow_Close(mosaicWindow[i]);
        }
        if (mosaicDecoder[i]) {
            NEXUS_VideoDecoder_Close(mosaicDecoder[i]);
            NEXUS_PidChannel_Close(videoProgram[i].pidChannel);
        }
    }
#if TEST_PIP
    if (pip) {
        NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(pip));
        NEXUS_VideoWindow_RemoveAllInputs(pipWindow);
        NEXUS_VideoDecoder_Close(pip);
    }
#endif
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();
#else
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);
#endif /* NEXUS_NUM_MOSAIC_DECODES */

    return 0;
}
