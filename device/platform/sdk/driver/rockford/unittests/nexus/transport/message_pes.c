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
 * $brcm_Workfile: message_pes.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 7/29/10 12:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/message_pes.c $
 * 
 * Hydra_Software_Devel/9   7/29/10 12:22p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/8   10/7/09 4:20p jtna
 * SW7405-3153: added standard comment header. Changed
 * startSettings.format that was no longer mapping to the correct XPT
 * option after the Nexus enum in nexus_message.h changed
 * 
 *****************************************************************************/

/* Nexus test app:
Simultaneously capture two PES streams using Message
*/

#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "nexus_platform.h"
#include "nexus_message.h"
#include "nexus_parser_band.h"

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

BDBG_MODULE(message);

void message_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

void overflow_callback(void *context, int param)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(param);
    printf("overflow detected\n");
}

int main(void)
{
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    unsigned pid0 = 0x11;
    unsigned pid1 = 0x21;
    NEXUS_PidChannelHandle pidChannel;
    NEXUS_MessageHandle msg0, msg1;
    NEXUS_MessageSettings settings;
    NEXUS_MessageStartSettings startSettings;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidChannelSettings;
    BKNI_EventHandle event;
    NEXUS_Error rc;
    FILE *foutput0, *foutput1;
    unsigned loops = 1000;
    const char *file0 = "videos/test0.pes";
    const char *file1 = "videos/test1.pes";
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    foutput0 = fopen(file0, "w+");
    if (!foutput0) {
        printf("Unable to open file %s for write\n", file0);
        return -1;
    }
    foutput1 = fopen(file1, "w+");
    if (!foutput1) {
        printf("Unable to open file %s for write\n", file1);
        return -1;
    }

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_Init(NULL);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    BKNI_CreateEvent(&event);

    NEXUS_Message_GetDefaultSettings(&settings);
    settings.dataReady.callback = message_callback;
    settings.dataReady.context = event;
    settings.overflow.callback = overflow_callback;
    settings.bufferSize = 512 * 1024; /* maximum size */
    msg0 = NEXUS_Message_Open(&settings);
    msg1 = NEXUS_Message_Open(&settings);

    /* get common defaults */
    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.requireMessageBuffer = true;

    NEXUS_Message_GetDefaultStartSettings(msg0, &startSettings);
    startSettings.format = NEXUS_MessageFormat_ePesSaveAll;

    /* open two pid channels & message filters */
    pidChannel = NEXUS_PidChannel_Open(parserBand, pid0, &pidChannelSettings);
    startSettings.pidChannel = pidChannel;
    rc = NEXUS_Message_Start(msg0, &startSettings);
    BDBG_ASSERT(!rc);

    pidChannel = NEXUS_PidChannel_Open(parserBand, pid1, &pidChannelSettings);
    startSettings.pidChannel = pidChannel;
    rc = NEXUS_Message_Start(msg1, &startSettings);
    BDBG_ASSERT(!rc);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    while (--loops) {
        const void *buffer0, *buffer1;
        size_t size0, size1;

        rc = NEXUS_Message_GetBuffer(msg0, &buffer0, &size0);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Message_GetBuffer(msg1, &buffer1, &size1);
        BDBG_ASSERT(!rc);

        if (!size0 && !size1) {
            rc = BKNI_WaitForEvent(event, 20 * 1000);
            BDBG_ASSERT(!rc);
            continue;
        }

        if (size0) {
            fwrite(buffer0, size0, 1, foutput0);
            rc = NEXUS_Message_ReadComplete(msg0, size0);
            BDBG_ASSERT(!rc);
        }
        if (size1) {
            fwrite(buffer1, size1, 1, foutput1);
            rc = NEXUS_Message_ReadComplete(msg1, size1);
            BDBG_ASSERT(!rc);
        }
        printf("complete %d %d\n", size0, size1);
    }

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    return 0;
}
