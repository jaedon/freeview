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
 * $brcm_Workfile: tune_73xx.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/31/09 2:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/frontend/tune_73xx.c $
 * 
 * Hydra_Software_Devel/1   7/31/09 2:40p erickson
 * PR57239: add 73xx test
 *
 * 9   7/13/09 4:48p erickson
 * PR56773: added NEXUS_Frontend_GetDiseqcStatus
 *
 * 8   4/1/09 4:00p erickson
 * PR53078: clean up app
 *
 * 7   2/18/09 2:01p erickson
 * PR51841: remove examples_lib
 *
 * 5   8/5/08 1:14p erickson
 * PR45425: don't assume straight IB->PB mapping in example apps.
 *  explicitly map.
 *
 * 4   4/4/08 4:02p erickson
 * PR35551: separate qam and vsb tune examples
 *
 * 2   2/12/08 2:33p jgarrett
 * PR 39009: Unifying status values
 *
 *****************************************************************************/
/* Example to tune a satellite channel using nexus */

#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_frontend_73xx.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void lock_callback(void *context, int param)
{
    NEXUS_FrontendHandle frontend = (NEXUS_FrontendHandle)context;
    NEXUS_FrontendSatelliteStatus status;
    NEXUS_FrontendDiseqcStatus disqecStatus;
    NEXUS_Error rc;

    BSTD_UNUSED(param);

    fprintf(stderr, "Frontend(%p) - lock callback\n", (void*)frontend);

    rc = NEXUS_Frontend_GetSatelliteStatus(frontend, &status);
    BDBG_ASSERT(!rc);
    fprintf(stderr, "  demodLocked = %d\n", status.demodLocked);
    NEXUS_Frontend_GetDiseqcStatus(frontend, &disqecStatus);
    fprintf(stderr, "  diseqc tone = %d, voltage = %d\n", disqecStatus.toneEnabled, disqecStatus.voltage);
}

int main(void)
{
    NEXUS_FrontendUserParameters userParams;
    NEXUS_FrontendHandle frontend=NULL;
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendDiseqcSettings diseqcSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle videoDecoder;
    unsigned i;
    NEXUS_Error rc;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_VideoDecoderStartSettings videoProgram;
    unsigned loops = 50;

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    for ( i = 0; i < NEXUS_MAX_FRONTENDS; i++ )
    {
        NEXUS_FrontendCapabilities capabilities;
        frontend = platformConfig.frontend[i];
        if (frontend) {
            NEXUS_Frontend_GetCapabilities(frontend, &capabilities);
            /* Does this frontend support satellite? */
            if ( capabilities.satellite )
            {
                break;
            }
        }
    }

    if (NULL == frontend )
    {
        fprintf(stderr, "Unable to find satellite-capable frontend\n");
        return 0;
    }

    NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
    satSettings.frequency = 1119000000;
    satSettings.mode = NEXUS_FrontendSatelliteMode_eDvb;
    satSettings.lockCallback.callback = lock_callback;
    satSettings.lockCallback.context = frontend;
    NEXUS_Frontend_GetUserParameters(frontend, &userParams);

    /* Map a parser band to the demod's input band. */
    parserBand = NEXUS_ParserBand_e2; /* USER: Change this value to match your platform */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = userParams.param1;  /* Platform initializes this to input band */
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x31, NULL);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;

    NEXUS_StcChannel_GetSettings(stcChannel, &stcSettings);
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    rc = NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
    BDBG_ASSERT(!rc);

    while (--loops)
    {
        NEXUS_FrontendSatelliteStatus satStatus;
        NEXUS_73xxLnaStatus lnaStatus;
        NEXUS_73xxLnaSettings lnaSettings;
        NEXUS_I2cHandle i2c;
        uint32_t value;

        NEXUS_Frontend_GetDiseqcSettings(frontend, &diseqcSettings);
        diseqcSettings.toneEnabled = true;
        diseqcSettings.voltage = NEXUS_FrontendDiseqcVoltage_e13v;
        rc = NEXUS_Frontend_SetDiseqcSettings(frontend, &diseqcSettings);
        BDBG_ASSERT(!rc);

        printf("tune\n");
        rc = NEXUS_Frontend_TuneSatellite(frontend, &satSettings);
        BDBG_ASSERT(!rc);

        BKNI_Sleep(rand() % 2000);

        rc = NEXUS_Frontend_GetSatelliteStatus(frontend, &satStatus);
        BDBG_ASSERT(!rc);
        printf("status: symbolRate %d, locked %d\n",
            satStatus.settings.symbolRate, satStatus.demodLocked);

        rc = NEXUS_Frontend_Get73xxLnaStatus(frontend, &lnaStatus);
        BDBG_ASSERT(!rc);

        i2c = NEXUS_Frontend_Get73xxMasterI2c(frontend);
        BDBG_ASSERT(i2c);

        NEXUS_Frontend_Get73xxLnaSettings(frontend, &lnaSettings);
        rc = NEXUS_Frontend_Set73xxLnaSettings(frontend, &lnaSettings);
        BDBG_ASSERT(!rc);

        rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Frontend_ReadSatelliteRegister(frontend, 0, &value);
        BDBG_ASSERT(!rc);

        rc = NEXUS_Frontend_WriteSatelliteRegister(frontend, 0, value);
        BDBG_ASSERT(!rc);

        BKNI_Sleep(rand() % 2000);

        NEXUS_VideoDecoder_Stop(videoDecoder);
        if (rand() % 2) {
            NEXUS_Frontend_Untune(frontend);
        }
        BKNI_Sleep(rand() % 1000);
    }

    return 0;
}
