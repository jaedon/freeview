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
 * $brcm_Workfile: message_dss.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 10/17/12 5:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/message_dss.c $
 * 
 * Hydra_Software_Devel/6   10/17/12 5:25p mward
 * SW7435-411:  Can no longer assume value of pidChannelIndex.
 * 
 * Hydra_Software_Devel/5   2/14/11 5:24p erickson
 * SW7420-1120: modify test based on NEXUS_PidChannel_Open returning
 * different handles
 *
 * Hydra_Software_Devel/4   11/9/10 6:29p randyjew
 * SW7400-2559: Modify to run Not_Message_Capable test if Num_Pid_Channels
 * greater than 128.
 *
 * Hydra_Software_Devel/2   10/20/09 2:23p erickson
 * SW7400-2559: add DSS message smoke test
 *
 * Hydra_Software_Devel/1   10/19/09 11:25a erickson
 * SW7400-2559: add test app
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

BDBG_MODULE(message_dss);

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
    NEXUS_PidChannelHandle pidChannel[3];
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_PidChannelSettings pidChannelSettings;
    NEXUS_PidChannelStatus pidChannelStatus;
    NEXUS_Error rc;
    unsigned i;
    NEXUS_MessageHandle msg;
    NEXUS_MessageStartSettings startSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
#endif

    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eDssEs;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = NULL;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    /* test1: pidChannelIndex NEXUS_PID_CHANNEL_OPEN options */
#if NEXUS_NUM_PID_CHANNELS > 128
    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_NOT_MESSAGE_CAPABLE;
    pidChannel[0] = NEXUS_PidChannel_Open(parserBand, 0x0, &pidChannelSettings);
    NEXUS_PidChannel_GetStatus(pidChannel[0], &pidChannelStatus);
#endif
    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_MESSAGE_CAPABLE;
    pidChannel[1] = NEXUS_PidChannel_Open(parserBand, 0x1, &pidChannelSettings);
    NEXUS_PidChannel_GetStatus(pidChannel[1], &pidChannelStatus);
	BDBG_WRN(("pidChannelStatus.pidChannelIndex = %d",pidChannelStatus.pidChannelIndex));

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.pidChannelIndex = NEXUS_PID_CHANNEL_OPEN_ANY;
    pidChannel[2] = NEXUS_PidChannel_Open(parserBand, 0x3, &pidChannelSettings);
    NEXUS_PidChannel_GetStatus(pidChannel[2], &pidChannelStatus);

#if NEXUS_NUM_PID_CHANNELS > 128
    NEXUS_PidChannel_Close(pidChannel[0]);
    pidChannel[0] = NULL;
#endif
    NEXUS_PidChannel_Close(pidChannel[1]);
    pidChannel[1] = NULL;
    NEXUS_PidChannel_Close(pidChannel[2]);
    pidChannel[2] = NULL;

    /* test2: dssHdFilter */

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    BDBG_ASSERT(pidChannelSettings.dssHdFilter == NEXUS_PidChannelDssHdFilter_eDisabled);
    pidChannelSettings.dssHdFilter = NEXUS_PidChannelDssHdFilter_eAux;
    pidChannel[0] = NEXUS_PidChannel_Open(parserBand, 0x1, &pidChannelSettings);
    NEXUS_PidChannel_GetStatus(pidChannel[0], &pidChannelStatus);

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.dssHdFilter = NEXUS_PidChannelDssHdFilter_eNonAux;
    BDBG_WRN(("expect failure")); /* can't change settings on the same HW pid channel */
    pidChannel[1] = NEXUS_PidChannel_Open(parserBand, 0x1, &pidChannelSettings);
    BDBG_ASSERT(!pidChannel[1]);

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.dssHdFilter = NEXUS_PidChannelDssHdFilter_eDisabled;
    pidChannel[1] = NEXUS_PidChannel_Open(parserBand, 0x0, &pidChannelSettings);
    BDBG_ASSERT(pidChannel[0] != pidChannel[1]); /* we can open a dup and get a different handle */

    NEXUS_PidChannel_GetDefaultSettings(&pidChannelSettings);
    pidChannelSettings.dssHdFilter = NEXUS_PidChannelDssHdFilter_eDisabled;
    pidChannel[2] = NEXUS_PidChannel_Open(parserBand, 0x0, &pidChannelSettings);
    BDBG_ASSERT(pidChannel[1] != pidChannel[2]); /* SW returns new handle, internal refcnt of HW pid channel */

    for (i=0;i<3;i++) {
        NEXUS_PidChannel_Close(pidChannel[i]);
        pidChannel[i] = NULL;
    }

    /* test3: DSS message filter */
    msg = NEXUS_Message_Open(NULL);
    BDBG_ASSERT(msg);

    pidChannel[0] = NEXUS_PidChannel_Open(parserBand, 0x1, NULL);
    NEXUS_Message_GetDefaultStartSettings(msg, &startSettings);
    startSettings.pidChannel = pidChannel[0];
    startSettings.dssMessageType = NEXUS_DssMessageType_eMpt;
    rc = NEXUS_Message_Start(msg, &startSettings);
    BDBG_ASSERT(!rc);

    BKNI_Sleep(1000);

    NEXUS_Message_Stop(msg);
    NEXUS_Message_Close(msg);
    NEXUS_PidChannel_Close(pidChannel[0]);
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    NEXUS_Platform_Uninit();
    printf("success\n");
    return 0;
}
