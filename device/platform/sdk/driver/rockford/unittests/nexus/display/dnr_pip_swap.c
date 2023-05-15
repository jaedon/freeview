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
* $brcm_Workfile: dnr_pip_swap.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 7/29/10 10:52a $
*
* API Description:
*   API name: Platform
*    Specific APIs to initialze the a board.
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/dnr_pip_swap.c $
* 
* Hydra_Software_Devel/2   7/29/10 10:52a calvinho
* SW7405-4653: Added Ip Streamer input support
* 
* Hydra_Software_Devel/1   7/8/09 2:40p erickson
* PR56634: added test
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
#include "nexus_composite_input.h"
#include "nexus_analog_video_decoder.h"
#endif
#include "nexus_component_output.h"
#include "nexus_video_window.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_video_adj.h"
#include "nexus_video_input.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

int main(void)
{
#if NEXUS_NUM_VIDEO_WINDOWS > 1 /* if not, no PIP capability */
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window0, window1;
    NEXUS_VideoInput videoInput0, videoInput1;
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    NEXUS_AnalogVideoDecoderHandle analogVideoDecoder;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
#endif
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_VideoWindowDnrSettings dnrSettings;
    NEXUS_Error rc;
    unsigned i;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    NEXUS_ParserBand parserBand1 = NEXUS_ParserBand_e1;
    NEXUS_VideoDecoderHandle vdecode1;
    NEXUS_VideoDecoderStartSettings videoProgram1;
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    /* Bring up all modules for a platform in a default configuration for this platform */
    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* platformConfig contains handles for CompositeInput and ComponentOutput. It knows how to set the DAC's and ADC's for this Platform. */

    /* Bring up display for 720p component output only */
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = NULL;
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
    printf("Using IP Streamer\n");
#endif

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x31, NULL);

    printf("Starting digital main\n");
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    vdecode = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);

    window0 = NEXUS_VideoWindow_Open(display, 0);
    videoInput0 = NEXUS_VideoDecoder_GetConnector(vdecode);
    rc = NEXUS_VideoWindow_AddInput(window0, videoInput0);
    BDBG_ASSERT(!rc);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = vdecode;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    BDBG_ASSERT(!rc);

    window1 = NEXUS_VideoWindow_Open(display, 1);
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    analogVideoDecoder = NEXUS_AnalogVideoDecoder_Open(0, NULL);
    videoInput1 = NEXUS_AnalogVideoDecoder_GetConnector(analogVideoDecoder);
    NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(platformConfig.inputs.composite[0]), &analogVideoDecoderSettings);
    NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
#else
    videoInput1 = NULL; /* TODO: add digital for settops */
#ifdef IP_STREAMER_SUBST
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram1);
    videoProgram1.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram1.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x31, NULL);

    printf("Starting digital secondary\n");
    vdecode1 = NEXUS_VideoDecoder_Open(1, NULL); /* take default capabilities */
    videoInput1 = NEXUS_VideoDecoder_GetConnector(vdecode1);
#endif
#endif
    rc = NEXUS_VideoWindow_AddInput(window1, videoInput1);
    BDBG_ASSERT(!rc);

    /* turn on DNR for window 0
    This test is DNR-specific because DNR is the only VideoAdj API which internally applies to the BVDC_Source, not BVDC_Window. */
    NEXUS_VideoWindow_GetDnrSettings(window0, &dnrSettings);
    dnrSettings.dcr.mode = NEXUS_VideoWindowFilterMode_eEnable;
    rc = NEXUS_VideoWindow_SetDnrSettings(window0, &dnrSettings);
    BDBG_ASSERT(!rc);
    
    for (i=0;i<60;i++) {
        printf("iteration %d\n", i);
        /* now swap and make sure DNR gets reapplied to the other BVDC_Source. */
        NEXUS_VideoDecoder_Stop(vdecode);
        NEXUS_VideoWindow_RemoveAllInputs(window0);
        NEXUS_VideoWindow_RemoveAllInputs(window1);
        rc = NEXUS_VideoWindow_AddInput(window0, i%2?videoInput0:videoInput1);
        BDBG_ASSERT(!rc);
        rc = NEXUS_VideoWindow_AddInput(window1, i%2?videoInput1:videoInput0);
        BDBG_ASSERT(!rc);
        rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
        BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
        rc = NEXUS_VideoDecoder_Start(vdecode1, &videoProgram1);
        BDBG_ASSERT(!rc);
#endif
        BKNI_Sleep(rand() % 1000);
    }

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

#endif /* NEXUS_NUM_VIDEO_WINDOWS > 1 */
    return 0;
}
