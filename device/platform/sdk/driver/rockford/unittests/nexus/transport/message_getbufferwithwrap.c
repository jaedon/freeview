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
 * $brcm_Workfile: message_getbufferwithwrap.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/3/10 11:57a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/message_getbufferwithwrap.c $
 * 
 * Hydra_Software_Devel/1   9/3/10 11:57a erickson
 * SW35230-1172: add NEXUS_Message_GetBufferWithWrap test
 *
 * Hydra_Software_Devel/6   7/29/10 12:11p calvinho
 * SW7405-4653: Added Ip Streamer input support
 *
 * Hydra_Software_Devel/5   5/8/09 2:27p erickson
 * PR42679: fix leak, add asserts
 *
 * Hydra_Software_Devel/4   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/2   5/14/08 12:29p erickson
 * PR42679: make test cruisecontrol friendly
 *
 * Hydra_Software_Devel/1   3/4/08 3:38p erickson
 * PR40080: updated tests
 *
 * 3   2/25/08 9:34a jgarrett
 * PR 39435: Fixing includes
 *
 * 2   1/24/08 12:52p vsilyaev
 * PR 38682: fixed return type of NEXUS_Platform_GetStreamerInputBand
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_message.h"

#include "bstd.h"
#include "bkni.h"

#include <stdio.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

BDBG_MODULE(message);

void message_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

void overflow(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    BDBG_WRN(("overflow"));
}

void psi_length_error(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    BDBG_WRN(("psi_length_error"));
}

int main(void) {
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_PidChannelHandle pidChannel;
    NEXUS_MessageHandle msg;
    NEXUS_MessageSettings settings;
    NEXUS_MessageStartSettings startSettings;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidChannelSettings;
    BKNI_EventHandle event;
    unsigned loops = 1000;
    NEXUS_Error rc;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);


    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.requireMessageBuffer = true;
    pidChannel = NEXUS_PidChannel_Open(parserBand, 0x0, &pidChannelSettings);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    BKNI_CreateEvent(&event);

    NEXUS_Message_GetDefaultSettings(&settings);
    settings.dataReady.callback = message_callback;
    settings.dataReady.context = event;
    settings.overflow.callback = overflow;
    settings.psiLengthError.callback = psi_length_error;
    settings.maxContiguousMessageSize = 0; /* required for NEXUS_Message_GetBufferWithWrap */
    settings.bufferSize = 1024;
    msg = NEXUS_Message_Open(&settings);

    NEXUS_Message_GetDefaultStartSettings(msg, &startSettings);
    startSettings.pidChannel = pidChannel;
    rc = NEXUS_Message_Start(msg, &startSettings);
    BDBG_ASSERT(!rc);

    while (loops--) {
        const void *buffer, *buffer2;
        size_t size, size2;

        rc = NEXUS_Message_GetBufferWithWrap(msg, &buffer, &size, &buffer2, &size2);
        BDBG_ASSERT(!rc);

        if (!size) {
            BDBG_ASSERT(!size2);
            rc = BKNI_WaitForEvent(event, 5 * 1000); /* wait 5 seconds */
            BDBG_ASSERT(!rc);
            continue;
        }
        rc = NEXUS_Message_ReadComplete(msg, size + size2);
        BDBG_ASSERT(!rc);
        printf("Read %d+%d=%d bytes of PAT on pid 0\n", size, size2, size+size2);
    }

    /* All consumed */
    NEXUS_Message_Stop(msg);
    NEXUS_Message_Close(msg);
    NEXUS_PidChannel_Close(pidChannel);
    BKNI_DestroyEvent(event);
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    NEXUS_Platform_Uninit();
    return 0;
}
