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
 * $brcm_Workfile: codec_test.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 4/17/11 12:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/audio/codec_test.c $
 * 
 * Hydra_Software_Devel/5   4/17/11 12:27p jgarrett
 * SW7344-46: Fixing resource leaks
 * 
 * Hydra_Software_Devel/4   7/29/10 10:46a calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/3   5/7/10 6:35p mward
 * SW7125-253:  Allow compilation with C++, eliminate "invalid conversion"
 * errors.
 * 
 * Hydra_Software_Devel/2   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   5/14/08 2:59p erickson
 * PR42685: added codec_test
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_stc_channel.h"
#include "nexus_audio_input.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "bstd.h"
#include "bkni.h"

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

int main(void) {
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_InputBand inputBand;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle audioPidChannel, pcrPidChannel;
    NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
    NEXUS_AudioDecoderStartSettings audioProgram;
    unsigned loops = 500;
    NEXUS_Error rc;
    NEXUS_AudioDecoderOpenSettings audioDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* Open the audio and video pid channels */
    pcrPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* PVR */
    stcSettings.modeSettings.pcr.pidChannel = pcrPidChannel;
#ifdef IP_STREAMER_SUBST
    IpsAddStcSettings(ips, &stcSettings);
#endif
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* Bring up audio decoders and outputs */

    NEXUS_AudioDecoder_GetDefaultOpenSettings(&audioDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddAudioDecoderOpenSettings(ips, &audioDecoderOpenSettings);
#endif
    pcmDecoder = NEXUS_AudioDecoder_Open(0, &audioDecoderOpenSettings);
    compressedDecoder = NEXUS_AudioDecoder_Open(1, &audioDecoderOpenSettings);
    rc = NEXUS_AudioOutput_AddInput(
        NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]),
        NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    BDBG_ASSERT(!rc);
    rc = NEXUS_AudioOutput_AddInput(
        NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]),
        NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    BDBG_ASSERT(!rc);

#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = platformConfig.outputs.audioDacs[0];
    ipsTimebaseSettings.display = NULL;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif
#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = pcmDecoder;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    while (--loops) {
        NEXUS_AudioDecoder_GetDefaultStartSettings(&audioProgram);
        audioProgram.codec = (NEXUS_AudioCodec) (loops % NEXUS_AudioCodec_eMax); /* iterate through every defined codec */
        audioProgram.pidChannel = audioPidChannel;
        audioProgram.stcChannel = stcChannel;

        /**
        The purpose of this test is not that all codecs must work. It is perfectly fine that Start fails for a codec.
        However, if Start fails, it should unwind correctly and allow starting another codec.
        If Start succeeds, we should be able to Stop, then Start another codec.
        As a result, you may see a lot of errors on the console, but Nexus should keep going, not crash, and have no memory leaks.
        Codecs that should work should keep on working.
        **/

        printf("starting codec %d\n", audioProgram.codec);
        rc = NEXUS_AudioDecoder_Start(pcmDecoder, &audioProgram);
        if (!rc) {
            rc = NEXUS_AudioDecoder_Start(compressedDecoder, &audioProgram);
            if (!rc) {
                BKNI_Sleep(rand() % 1000);
                NEXUS_AudioDecoder_Stop(compressedDecoder); /* must stop this first to avoid PCRlib errors */
            }
            NEXUS_AudioDecoder_Stop(pcmDecoder);
        }
    }

    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
    NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(platformConfig.outputs.spdif[0]));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
    NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(compressedDecoder, NEXUS_AudioDecoderConnectorType_eCompressed));
    NEXUS_AudioDecoder_Close(pcmDecoder);
    NEXUS_AudioDecoder_Close(compressedDecoder);
    NEXUS_StcChannel_Close(stcChannel);
    NEXUS_PidChannel_Close(audioPidChannel);
    NEXUS_PidChannel_Close(pcrPidChannel);

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    NEXUS_Platform_Uninit();
    return 0;
}
