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
 * $brcm_Workfile: resize_window.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 10/25/10 5:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/display/resize_window.c $
 * 
 * Hydra_Software_Devel/4   10/25/10 5:36p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/3   7/29/10 11:08a calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/2   3/23/09 1:31p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   8/13/08 1:25p erickson
 * PR45640: added lower right PIP resize test
 *
******************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_video_window.h"
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

int main(void)
{
#if !NEXUS_DTV_PLATFORM
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Error rc;
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
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
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

    /* Open the pid channels */
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

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
    window = NEXUS_VideoWindow_Open(display, 0);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif	


#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = NULL;
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    vdecode = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);
    
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
    BDBG_ASSERT(!rc);

#ifdef IP_STREAMER_SUBST
        ipsStartSettings.vDecoder = vdecode;
        ipsStartSettings.aDecoder = NULL;
        ipsStartSettings.stcChannel = videoProgram.stcChannel;
        IpsStart(ips, &ipsStartSettings);
#endif

    /* Start Decoders */
    rc = NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
    BDBG_ASSERT(!rc);

    BKNI_Sleep(2000);
    /* lower right corner */
    NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    windowSettings.position.x = videoFormatInfo.width / 2;
    windowSettings.position.y = videoFormatInfo.height / 2;
    windowSettings.position.width = videoFormatInfo.width / 2;
    windowSettings.position.height = videoFormatInfo.height / 2;
    rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);

    BKNI_Sleep(2000);
    NEXUS_Display_GetSettings(display, &displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtscJapan;
    rc = NEXUS_Display_SetSettings(display, &displaySettings);
    BDBG_ASSERT(!rc);

    BKNI_Sleep(2000);
    /* lower right corner */
    NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    windowSettings.position.x = videoFormatInfo.width / 2;
    windowSettings.position.y = videoFormatInfo.height / 2;
    windowSettings.position.width = videoFormatInfo.width / 2;
    windowSettings.position.height = videoFormatInfo.height / 2;
    rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);

    BKNI_Sleep(2000);
    NEXUS_Display_GetSettings(display, &displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    rc = NEXUS_Display_SetSettings(display, &displaySettings);
    BDBG_ASSERT(!rc);

    BKNI_Sleep(2000);

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
#endif

    return 0;
}
