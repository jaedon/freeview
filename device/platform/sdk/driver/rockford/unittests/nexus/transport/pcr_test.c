/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: pcr_test.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/pcr_test.c $
 * 
 * Hydra_Software_Devel/10   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/* Nexus test app:
monitor input band and PCR status
*/

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_input_band.h"
#include "nexus_timebase.h"

unsigned g_count = 0;

void pcr_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    g_count++;
    printf("got PCR: %d\n", g_count);
}

void pcr_error_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    printf("got PCR error\n");
}

int main(void)
{
    NEXUS_Error rc;
    NEXUS_InputBand inputBand = NEXUS_InputBand_e5;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e5;
    NEXUS_Timebase timebase = NEXUS_Timebase_e0;
    NEXUS_TimebaseSettings timebaseSettings;
    unsigned short pcrPid = 0x11;
    unsigned loops = 30;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);

    NEXUS_Timebase_GetSettings(timebase, &timebaseSettings);
    timebaseSettings.sourceType = NEXUS_TimebaseSourceType_ePcr;
    timebaseSettings.sourceSettings.pcr.pidChannel = NEXUS_PidChannel_Open(parserBand, pcrPid, NULL);
    timebaseSettings.pcrErrorCallback.callback = pcr_error_callback;
    timebaseSettings.pcrCallback.callback = pcr_callback;
    rc = NEXUS_Timebase_SetSettings(timebase, &timebaseSettings);
    BDBG_ASSERT(!rc);

    g_count = 0;
    NEXUS_Timebase_ResetStatus(timebase);

    while (--loops) {
        NEXUS_InputBandStatus status;
        NEXUS_TimebaseStatus timebaseStatus;
        uint64_t fullPcr; /* 42 bit PCR */

        rc = NEXUS_InputBand_GetStatus(inputBand, &status);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Timebase_GetStatus(timebase, &timebaseStatus);
        BDBG_ASSERT(!rc);

        fullPcr = timebaseStatus.lastValue;
        fullPcr <<= 10;
        fullPcr |= timebaseStatus.lastValueLo;

        printf("IB%d: syncCount %03d, PCR pid %x: count %04d, value %08x (42bit=%x%08x), errors %04d\n",
            inputBand, status.syncCount,
            pcrPid, timebaseStatus.pcrCount,
            timebaseStatus.lastValue,
            (uint32_t)(fullPcr>>32), (uint32_t)(fullPcr&0xFFFFFFFF),
            timebaseStatus.pcrErrors);

        if (timebaseStatus.pcrCount != g_count) {
            printf("ERROR: missed pcrCallback (%d != %d)\n", timebaseStatus.pcrCount, g_count);
        }

        BKNI_Sleep(1000);
    }

    NEXUS_Platform_Uninit();
    printf("Success\n");
    return 0;
}
