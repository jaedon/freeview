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
 * $brcm_Workfile: nexus_asrc.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/27/08 1:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/bluetooth/nexus_asrc.c $
 * 
 * Hydra_Software_Devel/3   8/27/08 1:52p erickson
 * PR42789: update
 *
 * Hydra_Software_Devel/2   8/27/08 10:46a erickson
 * PR45723: move audio capture complexity into BT hook code
 *
 * Hydra_Software_Devel/1   8/27/08 7:21a erickson
 * PR42789: add bluetooth unittest
 *
 *****************************************************************************/
#include <stdio.h>
#include "nexus_platform.h"
#include "nexus_platform_features.h"
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "nexus_pid_channel.h"
#include "nexus_stc_channel.h"
#include "nexus_parser_band.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_capture.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_core_utils.h"
#include "bt_hook.h"
#include <string.h>
#include <unistd.h>

int audio_source_get_buffer(void **data, unsigned *size, void *context)
{
    NEXUS_Error rc;

    rc = NEXUS_AudioCapture_GetBuffer(context, data, size);
    BDBG_ASSERT(!rc);

    return rc;
}

int audio_source_write_complete(unsigned size, void *context)
{
    NEXUS_Error rc;

    rc = NEXUS_AudioCapture_WriteComplete(context, size);
    BDBG_ASSERT(!rc);

    return rc;
}

int main(int argc, char **argv)
{
    NEXUS_PlatformConfiguration config;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_AudioDecoderHandle decoder;
    NEXUS_AudioCaptureHandle capture;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_AudioDecoderStartSettings decoderSettings;
    NEXUS_AudioCaptureStartSettings captureSettings;
    NEXUS_Error rc;
    BtHookInitSettings btInitSettings;
    BtHookAudioSourceSettings audioSourceSettings;
    const char *bd_addr_str = "00:07:A4:F3:51:72"; /* Developer's Motorola HT820. Override on cmdline. */

    if (argc > 1) {
        bd_addr_str = argv[1];
    }

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&config);

    decoder = NEXUS_AudioDecoder_Open(0, NULL);
    capture = NEXUS_AudioCapture_Open(0, NULL);

    /* Connect DAC to decoder */
    NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(config.outputs.audioDacs[0]),
                               NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(config.outputs.spdif[0]),
                               NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo));
    /* Connect capture to decoder */
    NEXUS_AudioOutput_AddInput(NEXUS_AudioCapture_GetConnector(capture),
                               NEXUS_AudioDecoder_GetConnector(decoder, NEXUS_AudioDecoderConnectorType_eStereo));
    /* Start the capture -- no data will be received until the decoder starts */
    NEXUS_AudioCapture_GetDefaultStartSettings(&captureSettings);
    NEXUS_AudioCapture_Start(capture, &captureSettings);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the StcChannel to do TSM with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Start the audio decoder */
    NEXUS_AudioDecoder_GetDefaultStartSettings(&decoderSettings);
    decoderSettings.codec = NEXUS_AudioCodec_eAc3;
    decoderSettings.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);
    decoderSettings.stcChannel = stcChannel;
    NEXUS_AudioDecoder_Start(decoder, &decoderSettings);

    /* bring up bluetooth stack */
    BtHook_GetDefaultInitSettings(&btInitSettings);
    rc = BtHook_Init(&btInitSettings);
    BDBG_ASSERT(!rc);

    rc = BtHook_ConnectHeadset(bd_addr_str);
    BDBG_ASSERT(!rc);

    /* connect the capture-from-decode to the audio source hook in the BT sw */
    BtHook_GetAudioSourceSettings(&audioSourceSettings);
    audioSourceSettings.getBuffer = audio_source_get_buffer;
    audioSourceSettings.writeComplete = audio_source_write_complete;
    audioSourceSettings.callbackContext = capture;
    rc = BtHook_SetAudioSourceSettings(&audioSourceSettings);
    BDBG_ASSERT(!rc);

    fprintf(stderr, "Started streaming audio to headset. Press enter to stop.\n");
    getchar();

    NEXUS_AudioCapture_Stop(capture);
    NEXUS_AudioDecoder_Stop(decoder);
    BtHook_Uninit();

    return 0;
}

