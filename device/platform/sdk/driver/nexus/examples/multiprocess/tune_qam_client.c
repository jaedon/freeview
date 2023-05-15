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
 * $brcm_Workfile: tune_qam_client.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 10/16/12 4:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/tune_qam_client.c $
 * 
 * 10   10/16/12 4:42p erickson
 * SW7425-3782: rework mtsif
 * 
 * 9   8/10/12 10:25a erickson
 * SW7425-2734: rework examples to use NEXUS_ClientResources for decoder
 *  ids
 * 
 * 8   8/2/12 10:10a erickson
 * SW7429-227: add app ipc for refsw_server support
 * 
 * 7   8/1/12 1:50p erickson
 * SW7231-391: add #if NEXUS_HAS_FRONTEND
 * 
 * 6   6/20/12 4:23p erickson
 * SW7420-2078: use NEXUS_Frontend_Acquire, add MTSIF support, support
 *  ctrl-c
 * 
 * 5   6/11/12 10:48a erickson
 * SW7425-3174: add example GetSoftDecisions call
 * 
 * 4   2/9/12 12:38p erickson
 * SW7420-2237: remove NEXUS_SimpleVideoDecoder_SetWindowSettings
 * 
 * 3   11/9/11 11:10a erickson
 * SW7420-1992: update
 * 
 * 2   11/8/11 2:40p bandrews
 * SW7231-391: parser band mp api changed
 * 
 * 1   10/31/11 6:05p bandrews
 * SW7231-391: added example for QAM tuning
 * 
 * SW7420-2078/1   10/6/11 9:30p bandrews
 * SW7231-391: qam tuning client
 * 
 *****************************************************************************/
/* Example to tune a QAM channel using nexus as a client in protected mode */

#if NEXUS_HAS_FRONTEND
#include "nexus_platform_client.h"
#include "nexus_frontend.h"
#include "nexus_platform.h" /* required for NEXUS_Platform_GetStreamerInputBand */
#include "nexus_simple_video_decoder.h"
#include "nexus_simple_audio_decoder.h"
#include "nexus_parser_band.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

BDBG_MODULE(tune_qam_client);

#include "refsw_session_simple_client.h"

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendQamStatus qamStatus;

    BSTD_UNUSED(param);

    fprintf(stderr, "Lock callback, frontend 0x%08x\n", (unsigned)frontend);

    NEXUS_Frontend_GetQamStatus(frontend, &qamStatus);
    fprintf(stderr, "QAM Lock callback, frontend 0x%08x - lock status %d, %d\n", (unsigned)frontend,
            qamStatus.fecLock, qamStatus.receiverLock);
}

int main(int argc, char **argv)
{
    NEXUS_ClientConfiguration clientConfig;
    NEXUS_ClientAuthenticationSettings authSettings;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendQamSettings qamSettings;
    NEXUS_SimpleVideoDecoderHandle videoDecoder;
    NEXUS_SimpleVideoDecoderStartSettings videoProgram;
    NEXUS_SimpleAudioDecoderHandle audioDecoder;
    NEXUS_SimpleAudioDecoderStartSettings audioProgram;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_PidChannelHandle audioPidChannel;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    /* default freq & qam mode */
    unsigned freq = 765;
    NEXUS_FrontendQamMode qamMode = NEXUS_FrontendQamMode_e64;
    NEXUS_FrontendAcquireSettings settings;

    /* allow cmdline freq & qam mode for simple test */
    if (argc > 1) {
        freq = atoi(argv[1]);
    }
    if (argc > 2) {
        unsigned mode = atoi(argv[2]);
        switch (mode) {
        case 64: qamMode = NEXUS_FrontendQamMode_e64; break;
        case 256: qamMode = NEXUS_FrontendQamMode_e256; break;
        case 1024: qamMode = NEXUS_FrontendQamMode_e1024; break;
        default: printf("unknown qam mode %d\n", mode); return -1;
        }
    }

    rc = simple_client_init(argv[0], &authSettings);
    printf("using certificate:'%s'\n", (char *)authSettings.certificate.data);
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) {
        printf("cannot join: %d\n", rc);
        return -1;
    }
    NEXUS_Platform_GetClientConfiguration(&clientConfig);

    NEXUS_Frontend_GetDefaultAcquireSettings(&settings);
    settings.capabilities.qam = true;
    frontend = NEXUS_Frontend_Acquire(&settings);
    if (NULL == frontend )
    {
        fprintf(stderr, "Unable to find QAM-capable frontend\n");
        return 0;
    }

    NEXUS_Frontend_GetDefaultQamSettings(&qamSettings);
    qamSettings.frequency = freq * 1000000;
    qamSettings.mode = qamMode;
    switch (qamMode) {
    default:
    case NEXUS_FrontendQamMode_e64: qamSettings.symbolRate = 5056900; break;
    case NEXUS_FrontendQamMode_e256: qamSettings.symbolRate = 5360537; break;
    case NEXUS_FrontendQamMode_e1024: qamSettings.symbolRate = 0; /* TODO */ break; 
    }
    qamSettings.annex = NEXUS_FrontendQamAnnex_eB;
    qamSettings.bandwidth = NEXUS_FrontendQamBandwidth_e6Mhz;
    qamSettings.lockCallback.callback = lock_callback;
    qamSettings.lockCallback.context = frontend;

    NEXUS_Frontend_GetUserParameters(frontend, &userParams);

    /* Map a parser band to the demod's input band. */
    parserBand = NEXUS_ParserBand_Open(NEXUS_ANY_ID);
    if (!parserBand) goto err_acquire_parser;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    if (userParams.isMtsif) {
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
        parserBandSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(frontend); /* NEXUS_Frontend_TuneXyz() will connect this frontend to this parser band */
    }
    else {
        parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
        parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    }
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Bring up audio decoders and outputs */
    if (clientConfig.resources.simpleVideoDecoder.total) {
        videoDecoder = NEXUS_SimpleVideoDecoder_Acquire(clientConfig.resources.simpleVideoDecoder.id[0]);
    }
    else {
        videoDecoder = NULL;
    }
    if (!videoDecoder) {
        BDBG_ERR(("video decoder not available"));
        return -1;
    }
    if (clientConfig.resources.simpleAudioDecoder.total) {
        audioDecoder = NEXUS_SimpleAudioDecoder_Acquire(clientConfig.resources.simpleAudioDecoder.id[0]);
    }
    else {
        audioDecoder = NULL;
    }
    if (!audioDecoder) {
        BDBG_ERR(("audio decoder not available"));
        return -1;
    }

    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    audioPidChannel = NEXUS_PidChannel_Open(parserBand, 0x14, NULL);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.mode = NEXUS_StcChannelMode_ePcr;
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel;
    stcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
    BDBG_ASSERT(stcChannel);

    /* Set up decoder Start structures now. We need to know the audio codec to properly set up
    the audio outputs. */
    NEXUS_SimpleVideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.settings.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.settings.pidChannel = videoPidChannel;
    videoProgram.settings.stcChannel = stcChannel;
    NEXUS_SimpleAudioDecoder_GetDefaultStartSettings(&audioProgram);
    audioProgram.primary.codec = NEXUS_AudioCodec_eAc3;
    audioProgram.primary.pidChannel = audioPidChannel;
    audioProgram.primary.stcChannel = stcChannel;

    NEXUS_Frontend_TuneQam(frontend, &qamSettings);

    /* Start decoders */
    rc = NEXUS_SimpleVideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);

    NEXUS_SimpleAudioDecoder_Start(audioDecoder, &audioProgram);
    /* decode may fail if audio codec not supported */

#if 0
    while (1) {
#define NUM_SOFT_DECISIONS 10
        NEXUS_FrontendSoftDecision decisions[NUM_SOFT_DECISIONS];
        unsigned i;
        BKNI_Memset(decisions, 0xAA, sizeof(decisions));
        rc = NEXUS_Frontend_GetSoftDecisions(frontend, decisions, NUM_SOFT_DECISIONS);
        BDBG_ASSERT(!rc);
        for (i=0;i<NUM_SOFT_DECISIONS;i++) {
            printf("i %d, q %d\n", decisions[i].i, decisions[i].q);
        }
        BKNI_Sleep(1000);
    }
#else
    printf("Press enter to stop.\n");
    getchar();
#endif

    NEXUS_SimpleVideoDecoder_Stop(videoDecoder);
    NEXUS_SimpleAudioDecoder_Stop(audioDecoder);

    NEXUS_PidChannel_Close(videoPidChannel);
    NEXUS_PidChannel_Close(audioPidChannel);

    NEXUS_SimpleVideoDecoder_Release(videoDecoder);
    NEXUS_SimpleAudioDecoder_Release(audioDecoder);

    NEXUS_ParserBand_Close(parserBand);
    NEXUS_Frontend_Release(frontend);

err_acquire_parser:

    simple_client_shutdown();
    NEXUS_Platform_Uninit();
    return 0;
}
#else
#include <stdio.h>
int main(void)
{
    printf("ERROR: frontend not supported\n");
    return -1;
}
#endif
