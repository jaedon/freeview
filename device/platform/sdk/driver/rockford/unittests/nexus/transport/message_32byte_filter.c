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
 * $brcm_Workfile: message_32byte_filter.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/12/10 4:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/message_32byte_filter.c $
 * 
 * Hydra_Software_Devel/1   7/12/10 4:50p erickson
 * SW7405-4561: add 32 byte filter test
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
#include <stdlib.h>

BDBG_MODULE(message);

void message_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(int argc, char **argv) {
    NEXUS_PlatformSettings platformSettings;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_PidChannelHandle pidChannel;
    NEXUS_MessageHandle msg;
    NEXUS_MessageSettings settings;
    NEXUS_MessageStartSettings startSettings;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidChannelSettings;
    const uint8_t *buffer;
    size_t size;
    BKNI_EventHandle event;
    unsigned loops = 1000;
    NEXUS_Error rc;
    unsigned index = 15;
    unsigned value = 0x3e;

    if (argc > 1) {
        index = strtoul(argv[1], NULL, 0);
    }
    if (argc > 2) {
        value = strtoul(argv[2], NULL, 0);
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);

    /* You must compile with export BXPT_FILTER_32=y for this to succeed */
    BDBG_CASSERT(NEXUS_MESSAGE_FILTER_SIZE == 32);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.requireMessageBuffer = true;
    pidChannel = NEXUS_PidChannel_Open(parserBand, 0x0, &pidChannelSettings);

    BKNI_CreateEvent(&event);

    NEXUS_Message_GetDefaultSettings(&settings);
    settings.dataReady.callback = message_callback;
    settings.dataReady.context = event;
    msg = NEXUS_Message_Open(&settings);

    NEXUS_Message_GetDefaultStartSettings(msg, &startSettings);
    startSettings.pidChannel = pidChannel;
    BKNI_Printf("\nfiltering for value=%#x at index %d\n", value, index);
    BDBG_ASSERT(index < NEXUS_MESSAGE_FILTER_SIZE-1);
    if (index == 2) {
        BDBG_ERR(("unable to filter for byte 2 (message size)"));
        return 1;
    }
    if (index > 1) index--;
    startSettings.filter.mask[index] = 0x00;
    startSettings.filter.coefficient[index] = value;
    rc = NEXUS_Message_Start(msg, &startSettings);
    BDBG_ASSERT(!rc);

    while (loops--) {
        unsigned message_length, i;

        rc = NEXUS_Message_GetBuffer(msg, (const void **)&buffer, &size);
        BDBG_ASSERT(!rc);

        if (!size) {
            rc = BKNI_WaitForEvent(event, 5 * 1000); /* wait 5 seconds */
            BDBG_ASSERT(!rc);
            continue;
        }

#define TS_READ_16( BUF ) ((uint16_t)((BUF)[0]<<8|(BUF)[1]))
#define TS_PSI_GET_SECTION_LENGTH( BUF )    (uint16_t)(TS_READ_16( &(BUF)[1] ) & 0x0FFF)

        /* We should always get whole PAT's because maxContiguousMessageSize is 4K */
        message_length = TS_PSI_GET_SECTION_LENGTH(buffer) + 3;
        BDBG_ASSERT(size >= (size_t)message_length);

        BKNI_Printf("message size %d: ", message_length);
        for (i=0;i<message_length;i++) BKNI_Printf("%02x", buffer[i]);
        BKNI_Printf("\n");

        /* XPT HW is configured to pad all messages to 4 bytes. If we are calling NEXUS_Message_ReadComplete
        based on message length and not the size returned by NEXUS_Message_GetBuffer, then we must add that pad.
        If we are wrong, NEXUS_Message_ReadComplete will fail. */
        if (message_length % 4) {
            message_length += 4 - (message_length % 4);
        }

        /* only complete one PAT */
        rc = NEXUS_Message_ReadComplete(msg, message_length);
        BDBG_ASSERT(!rc);
    }

    /* All consumed */
    NEXUS_Message_ReadComplete(msg, size);
    NEXUS_Message_Stop(msg);
    NEXUS_Message_Close(msg);
    NEXUS_PidChannel_Close(pidChannel);
    BKNI_DestroyEvent(event);

    NEXUS_Platform_Uninit();
    return 0;
}
