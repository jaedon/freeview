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
 * $brcm_Workfile: playpump_encrypted.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 1/26/12 3:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/dvr/playpump_encrypted.c $
 * 
 * 8   1/26/12 3:00p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 7   5/12/11 3:44p jtna
 * SW7550-739: replace all instances of 'NEXUS_HAS_DMA &&
 *  NEXUS_HAS_SECURITY' with 'NEXUS_ENCRYPTED_DVR_WITH_M2M'. replace some
 *  instances of 'NEXUS_HAS_DMA' with 'NEXUS_NUM_DMA_CHANNELS'
 * 
 * 6   8/10/10 3:34p erickson
 * SW7420-934: rename NEXUS_Playpump_ReadComplete to
 *  NEXUS_Playpump_WriteComplete
 * 
 * 5   6/7/10 6:23p mphillip
 * SW7550-398: Update examples and transport code to support non-DMA
 *  encrypted PVR
 * 
 * 4   3/22/10 2:33a randyjew
 * SW7468-143: Support for boards without component and/or composite
 * 
 * 3   2/19/09 3:18p erickson
 * PR51841: remove examples_lib
 *
 * 5   2/4/09 12:48p erickson
 * PR51841: refactor example
 *
 *****************************************************************************/
/* Nexus example app: playpump and decode */

#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_playpump.h"
#include "nexus_security.h"
#include "nexus_dma.h"

#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#if BCHP_CHIP == 7550
#define USE_CPD 1
#endif

/* the following define the input file and its characteristics -- these will vary by input file */
#define FILE_NAME "videos/stream_3des.mpg"
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define VIDEO_PID 0x21

static void play_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(void)
{
    FILE *file;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_PlaypumpHandle playpump;
    BKNI_EventHandle event;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_KeySlotHandle decKeyHandle;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;
    NEXUS_DmaHandle dma;
    NEXUS_SecurityKeySlotSettings keySettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Error rc;
#endif
    const char *fname = FILE_NAME;
    static const uint8_t keys[16] = { 0x4e, 0x61, 0xbc, 0x00, 0xa8, 0x7f, 0x39, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    dma = NEXUS_Dma_Open(0, NULL);

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
#if USE_CPD
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eCp;
#else
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
#endif
    decKeyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!decKeyHandle) {
        printf("\nAllocate enc keyslot failed \n");
        return 1;
    }
    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
#if USE_CPD
    algConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCpd;
#else
    /* Only used with m2m engine, use of CPS/CPD determines whether it encrypts or decrypts */
    algConfig.operation = NEXUS_SecurityOperation_eDecrypt;
#endif
    algConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
    algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    NEXUS_Security_ConfigAlgorithm(decKeyHandle, &algConfig);

    /* Load AV keys */
    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = sizeof(keys);
    BKNI_Memcpy(key.keyData, keys, sizeof(keys));
    if (NEXUS_Security_LoadClearKey(decKeyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }

    BKNI_CreateEvent(&event);

    playpump = NEXUS_Playpump_Open(0, NULL);

    /* use stdio for file I/O to keep the example simple. */
    file = fopen(fname, "rb");
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", fname);
        goto error;
    }

    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.dataCallback.callback = play_callback;
    playpumpSettings.dataCallback.context = event;
    playpumpSettings.securityDma = dma;
    playpumpSettings.securityContext = decKeyHandle;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_Playpump_Start(playpump);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_eAuto;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    videoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, VIDEO_PID, NULL);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    /* Bring up video decoder and display. No audio to keep the example simple. */
    display = NEXUS_Display_Open(0, NULL);
    window = NEXUS_VideoWindow_Open(display, 0);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
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

    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);

    while (1) {
        void *buffer;
        size_t buffer_size;
        int n;
        NEXUS_Error rc;

        if (NEXUS_Playpump_GetBuffer(playpump, &buffer, &buffer_size))
            break;
        if (buffer_size == 0) {
            BKNI_WaitForEvent(event, BKNI_INFINITE);
            continue;
        }

        /* The first call to get_buffer will return the entire playback buffer.
        If we use it, we're going to have to wait until the descriptor is complete,
        and then we're going to underflow. So use a max size. */
#define MAX_READ (188*1024)
        if (buffer_size > MAX_READ)
            buffer_size = MAX_READ;

        n = fread(buffer, 1, buffer_size, file);
        if (n < 0) goto error;
        if (n == 0) {
            BKNI_Sleep(1000); /* This is a simplistic way of making sure that decoder buffers are empty before flushing. */
            fseek(file, 0, SEEK_SET);
            NEXUS_Playpump_Flush(playpump);
        }
        else {
            rc = NEXUS_Playpump_WriteComplete(playpump, 0, n);
            BDBG_ASSERT(!rc);
            printf("played %d bytes\n", n);
        }
    }
    return 0;

error:
    return 1;
}
