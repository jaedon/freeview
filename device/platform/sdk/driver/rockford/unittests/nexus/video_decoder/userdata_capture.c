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
 * $brcm_Workfile: userdata_capture.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 7/29/10 10:39a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/userdata_capture.c $
 * 
 * Hydra_Software_Devel/5   7/29/10 10:39a erickson
 * SW7405-4637: handle userdata ringbuffer wraps internally for easier app
 * code
 * 
 * Hydra_Software_Devel/4   7/28/10 5:25p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/3   5/26/10 1:03p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/2   6/1/09 11:57a erickson
 * PR55042: add afdChanged support
 *
 * Hydra_Software_Devel/1   2/10/09 10:17a erickson
 * PR42679: added userdata_capture test
 *
******************************************************************************/
/* Nexus example app: capture userdata from video decoder */

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

BDBG_MODULE(userdata_capture);

unsigned total = 0;
FILE *outputfile;

void userdata(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;

    BSTD_UNUSED(param);

    /* It's crucial that the callback process all userdata packets in the buffer.
    The app cannot assume that it can process one packet per callback. */
    while (1) {
        NEXUS_UserDataHeader *pHeader;
        unsigned size;
        NEXUS_Error rc;

        /* First, get the size of the userdata field. */
        rc = NEXUS_VideoDecoder_GetUserDataBuffer(videoDecoder, (void**)&pHeader, &size);
            BDBG_ASSERT(!rc);
        if (size == 0) break;

        BDBG_ASSERT(size >= pHeader->blockSize);
        BDBG_ASSERT(pHeader->blockSize == sizeof(*pHeader) + pHeader->payloadSize);
        BDBG_ASSERT(pHeader->blockSize % 4 == 0); /* 32 bit aligned */

        /* write payload to file */
        fwrite((uint8_t*)pHeader + sizeof(*pHeader), 1, pHeader->payloadSize, outputfile);
        printf("blockSize %d, payload %d\n", pHeader->blockSize, pHeader->payloadSize);
        total += pHeader->blockSize;

        NEXUS_VideoDecoder_UserDataReadComplete(videoDecoder, pHeader->blockSize);
    }
}

static void afd_changed(void *context, int param)
{
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    NEXUS_VideoDecoderUserDataStatus status;

    BSTD_UNUSED(param);

    NEXUS_VideoDecoder_GetUserDataStatus(videoDecoder, &status);
    printf("AFD %x\n", status.afdValue);
}

int main(void)
{
    NEXUS_Error rc;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderOpenSettings vdOpenSettings;
    NEXUS_VideoDecoderSettings videoDecoderSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    unsigned lasttotal;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    outputfile = fopen("userdata.dat", "w+");

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
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x21, NULL);

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
#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
 #if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif
#endif

#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = platformConfig.outputs.audioDacs[0];
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    /* bring up decoder and connect to display */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&vdOpenSettings);
    /* using default userDataBufferSize */
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &vdOpenSettings);
#endif
    vdecode = NEXUS_VideoDecoder_Open(0, &vdOpenSettings);
    NEXUS_VideoDecoder_GetSettings(vdecode, &videoDecoderSettings);
    videoDecoderSettings.userDataEnabled = true;
    videoDecoderSettings.appUserDataReady.callback = userdata;
    videoDecoderSettings.appUserDataReady.context = vdecode;
    videoDecoderSettings.afdChanged.callback = afd_changed;
    videoDecoderSettings.afdChanged.context = vdecode;
    NEXUS_VideoDecoder_SetSettings(vdecode, &videoDecoderSettings);

    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

#ifdef IP_STREAMER_SUBST
        ipsStartSettings.vDecoder = vdecode;
        ipsStartSettings.aDecoder = NULL;
        ipsStartSettings.stcChannel = videoProgram.stcChannel;
        IpsStart(ips, &ipsStartSettings);
#endif

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);

    lasttotal = 0;
    while (1) {
        BKNI_Sleep(10 * 1000);

        /* if there's no data for 10 seconds, exit */
        if (total == lasttotal) break;
        lasttotal = total;
    }

    fclose(outputfile);
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    return 0;
}

