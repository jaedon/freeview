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
 * $brcm_Workfile: userdata.c $
 * $brcm_Revision: 22 $
 * $brcm_Date: 6/8/12 11:53a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/video/userdata.c $
 * 
 * 22   6/8/12 11:53a jtna
 * SW7231-843: add shutdown code
 * 
 * 21   1/26/12 11:23a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 20   11/15/10 3:51p erickson
 * SW7342-276: update for separate
 *  NEXUS_VideoInputVbiSettings.closedCaptionDataReady callback
 * 
 * 19   7/29/10 8:27a erickson
 * SW7405-4637: handle userdata ringbuffer wraps internally for easier app
 *  code
 *
 * 18   3/25/09 10:26a erickson
 * PR42679: added dtv support
 *
 * 17   3/5/09 3:26p erickson
 * PR52813: add sample call to NEXUS_VideoDecoder_SetUserDataFormatFilter
 *  for documentation
 *
 * 16   1/15/09 10:08a erickson
 * PR51121: use default userDataBufferSize. added comments re: proper
 *  userdata callback processing.
 *
 * 15   7/18/08 11:31a jgarrett
 * PR 44953: Removing -Wstrict-prototypes warnings
 *
 * 14   6/19/08 2:32p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 13   6/7/08 2:41p jgarrett
 * PR 43318: Allowing for platforms without composite outputs
 *
 * 12   5/20/08 11:04a erickson
 * PR41962: add larger userDataBufferSize
 *
 * 11   4/28/08 4:10p erickson
 * PR41533: unify EIA-608 and EIA-708 closed caption data capture in
 *  NEXUS_VideoInput_ReadClosedCaption
 *
 * 10   4/18/08 1:53p erickson
 * PR41533: impl EIA-708 parsing and capture
 *
 * 9   4/17/08 4:38p erickson
 * PR41533: rework NEXUS_VideoDecoder_GetUserDataBuffer to use
 *  NEXUS_UserDataHeader before payload
 *
 * 8   3/24/08 12:01p erickson
 * PR39000: added userDataEnabled
 *
 * 7   2/25/08 9:34a jgarrett
 * PR 39435: Fixing includes
 *
 * 6   2/1/08 3:21p erickson
 * PR38679: add vbi example
 *
 * 5   1/24/08 12:52p vsilyaev
 * PR 38682: fixed return type of NEXUS_Platform_GetStreamerInputBand
 *
 * 4   1/23/08 12:38p erickson
 * PR38919: rename NEXUS_Display_GetWindow to NEXUS_VideoWindow_Open
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
#include "nexus_display_vbi.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_video_window.h"
#include "nexus_video_input_vbi.h"

BDBG_MODULE(userdata);

/* the following define the input and its characteristics -- these will vary by input */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define VIDEO_PID 0x11

static void userdata(void *context, int param)
{
    NEXUS_Error rc;
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;

    BSTD_UNUSED(param);

    /* It's crucial that the callback process all userdata packets in the buffer.
    The app cannot assume that it can process one packet per callback. */
    while (1) {
        unsigned size;
        uint8_t *buffer;
        NEXUS_UserDataHeader *pHeader;

        /* get the header */
        rc = NEXUS_VideoDecoder_GetUserDataBuffer(videoDecoder, (void**)&buffer, &size);
        BDBG_ASSERT(!rc);

        /* The app can assume that userdata will be placed into the buffer in whole packets.
        The termination condition for the loop is that there are no more bytes in the buffer. */
        if (!size) break;

        /* guaranteed whole header + payload */
        pHeader = (NEXUS_UserDataHeader *)buffer;
        BDBG_ASSERT(size >= pHeader->blockSize);
        BDBG_ASSERT(pHeader->blockSize == sizeof(*pHeader) + pHeader->payloadSize);
        BDBG_ASSERT(pHeader->blockSize % 4 == 0); /* 32 bit aligned */

        printf("blockSize %d, payload %d\n", pHeader->blockSize, pHeader->payloadSize);

        NEXUS_VideoDecoder_UserDataReadComplete(videoDecoder, pHeader->blockSize);
    }
}


/* EIA-608/708 is captured in VideoInput so that there is a common API for analog & digital.
There's a little logic here, but it's just to print the # of contiguous 608 or 708 entries. */
static void closed_caption(void *context, int param)
{
    NEXUS_Error rc;
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)context;
    unsigned lasttype = 608, thistype;
    unsigned count = 0;

    BSTD_UNUSED(param);

    while (1) {
        NEXUS_ClosedCaptionData entry;
        unsigned numEntries;
        rc = NEXUS_VideoInput_ReadClosedCaption(NEXUS_VideoDecoder_GetConnector(videoDecoder), &entry, 1, &numEntries);
        BDBG_ASSERT(!rc);
        if (numEntries) {
            thistype = (entry.field >= 2)?708:608;
            count++;
        }

        if (thistype != lasttype || !numEntries) {
            if (count) {
                printf("  %d EIA-%d entries\n", count, lasttype);
            }
            lasttype = thistype;
            count = 0;
        }

        if (!numEntries) {
            break;
        }
    }
}


int main(void)
{
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
    NEXUS_DisplayVbiSettings displayVbiSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    NEXUS_Error rc;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
    videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_SCART_INPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[0]));
#if NEXUS_NUM_SCART_INPUTS>1
    NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(platformConfig.inputs.scart[1]));
#endif
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
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&vdOpenSettings);
    /* using default userDataBufferSize */
    vdecode = NEXUS_VideoDecoder_Open(0, &vdOpenSettings);
    NEXUS_VideoDecoder_GetSettings(vdecode, &videoDecoderSettings);
    videoDecoderSettings.userDataEnabled = true;
    videoDecoderSettings.appUserDataReady.callback = userdata;
    videoDecoderSettings.appUserDataReady.context = vdecode;
    NEXUS_VideoDecoder_SetSettings(vdecode, &videoDecoderSettings);

    /* Tell video decoder what type of userdata to extract if there are multiple formats. */
    NEXUS_VideoDecoder_SetUserDataFormatFilter(vdecode, NEXUS_UserDataFormat_eAny);

    /* set this for EIA-608 capture */
    {
    NEXUS_VideoInputVbiSettings viVbiSettings;
    NEXUS_VideoInput_GetVbiSettings(NEXUS_VideoDecoder_GetConnector(vdecode), &viVbiSettings);
    viVbiSettings.closedCaptionEnabled = true;
    viVbiSettings.closedCaptionBufferSize = 50;
    viVbiSettings.closedCaptionDataReady.callback = closed_caption;
    viVbiSettings.closedCaptionDataReady.context = vdecode;
    rc = NEXUS_VideoInput_SetVbiSettings(NEXUS_VideoDecoder_GetConnector(vdecode), &viVbiSettings);
    BDBG_ASSERT(!rc);
    }

    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

    NEXUS_Display_GetVbiSettings(display, &displayVbiSettings);
    /* Tell the VideoDecoder to extract user data, parse our CC, and route to Display */
    displayVbiSettings.vbiSource = NEXUS_VideoDecoder_GetConnector(vdecode);
    displayVbiSettings.closedCaptionRouting = true;
    /* Tell Display to encode incoming CC to VEC */
    displayVbiSettings.closedCaptionEnabled = true;
    NEXUS_Display_SetVbiSettings(display, &displayVbiSettings);

    /* Start Decoders */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);
 
    getchar();

    /* Shutdown */
    NEXUS_VideoDecoder_Stop(vdecode);
    NEXUS_VideoWindow_Close(window);
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_Display_Close(display);
    NEXUS_StcChannel_Close(videoProgram.stcChannel);
    NEXUS_PidChannel_Close(videoProgram.pidChannel);
    NEXUS_Platform_Uninit();
    return 0;
}

