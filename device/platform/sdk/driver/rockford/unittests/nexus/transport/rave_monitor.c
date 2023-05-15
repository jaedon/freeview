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
 * $brcm_Workfile: rave_monitor.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/30/09 9:30a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/transport/rave_monitor.c $
 * 
 * Hydra_Software_Devel/1   3/30/09 9:30a erickson
 * PR53662: added rave_monitor test code
 *
 *************************************************************************/

#include "nexus_parser_band.h"
#include "nexus_recpump.h"

/**
rave_monitor allows an app to view transport data using additional rave contexts simultaneous to an application's use of transport.
The rave contexts can be connected to the same parser band or a different band than the app is using.
This allows us to isolate whether a problem is happening in a RAVE context, caused by a decoder connected to a RAVE context,
or is caused by the parser band (i.e. the RS and XC buffers).

The rave_monitor uses recpump, but does not store any data to disk. It inspects every transport packet in the CDB.
It does not inspect ITB.
**/

int b_start_monitor(unsigned recpumpIndex, NEXUS_ParserBand parserBand, NEXUS_InputBand inputBand);
void b_add_monitor_pid_channel(int handle, NEXUS_PidChannelHandle pidChannel);
void b_remove_monitor_pid_channel(int handle, NEXUS_PidChannelHandle pidChannel);

static struct {
    NEXUS_RecpumpHandle recpump;
    unsigned recpumpIndex;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    struct {
        unsigned short pid;
        NEXUS_PidChannelHandle pidChannel;
        int cc;
        unsigned ccErrors;
#define MAX_PIDS_PER_PB 5
    } pid[MAX_PIDS_PER_PB];
    unsigned packetCount;
#define MAX_MONITORS 2
} g_monitor[MAX_MONITORS];

static int g_totalMonitors = 0;
static unsigned g_lastStatusTime = 0;

#define NEXT_CC(cc) (((cc) == 15)?0:(cc)+1)

static unsigned b_get_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
}

static void print_status(void)
{
    unsigned i,j;
    unsigned now = b_get_time();

    now -= now % 1000;
    if (now <= g_lastStatusTime + 5000) {
        return;
    }
    g_lastStatusTime = now;

    for (j=0;j<MAX_MONITORS;j++) {
        BDBG_WRN(("Parser Band %d", g_monitor[j].parserBand));
        for (i=0;i<MAX_PIDS_PER_PB;i++) {
            if (g_monitor[j].pid[i].pidChannel) {
                BDBG_WRN(("  %d bad cc on pid %#x", g_monitor[j].pid[i].ccErrors, g_monitor[j].pid[i].pid));
            }
        }
        BDBG_WRN(("  %d total packets processed", g_monitor[j].packetCount));
    }
}

static void dataready_callback(void *callback, int handle)
{
    const void *buffer;
    unsigned size;
    unsigned index;

    BSTD_UNUSED(callback);
    if (NEXUS_Recpump_GetDataBuffer(g_monitor[handle].recpump, &buffer, &size) || !size)
        return;

    size -= size%188;

    for (index = 0; index < size; index += 188) {
        unsigned i;
        uint8_t *packet = &((uint8_t*)buffer)[index];
        int cc = packet[3] & 0x0F;
        int adaptation_field_control = (packet[3] & 0x30) >> 4;
        unsigned short pid = ((((unsigned short)packet[1]&0x1f) << 8) | packet[2]);

        if (adaptation_field_control == 0 || adaptation_field_control == 2) continue;

        for (i=0;i<MAX_PIDS_PER_PB;i++) {
            if (g_monitor[handle].pid[i].pidChannel && g_monitor[handle].pid[i].pid == pid) {
                if (g_monitor[handle].pid[i].cc != -1) {
                    if (cc != NEXT_CC(g_monitor[handle].pid[i].cc)) {
                        g_monitor[handle].pid[i].ccErrors++;
                    }
                }
                g_monitor[handle].pid[i].cc = cc;
            }
        }
        g_monitor[handle].packetCount++;
    }

    NEXUS_Recpump_DataWriteComplete(g_monitor[handle].recpump, size);

    print_status();
}

int b_start_monitor(unsigned recpumpIndex, NEXUS_ParserBand parserBand, NEXUS_InputBand inputBand)
{
    int handle;
    unsigned i;
    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_Error rc;

    for (i=0;i<MAX_MONITORS;i++) {
        if (g_monitor[i].recpump && g_monitor[i].recpumpIndex == recpumpIndex) {
            return i;
        }
    }
    handle = g_totalMonitors++;

    BDBG_WRN(("b_start_monitor %d %d %d", recpumpIndex, parserBand, inputBand));
    g_monitor[handle].inputBand = inputBand;
    g_monitor[handle].parserBand = parserBand;

    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);

    g_monitor[handle].recpump = NEXUS_Recpump_Open(recpumpIndex, NULL);
    g_monitor[handle].recpumpIndex = recpumpIndex;

    NEXUS_Recpump_GetSettings(g_monitor[handle].recpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = dataready_callback;
    recpumpSettings.data.dataReady.param = handle;
    rc = NEXUS_Recpump_SetSettings(g_monitor[handle].recpump, &recpumpSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Recpump_Start(g_monitor[handle].recpump);
    BDBG_ASSERT(!rc);

    return handle;
}

void b_add_monitor_pid_channel(int handle, NEXUS_PidChannelHandle pidChannel)
{
    NEXUS_Error rc;
    unsigned i;
    NEXUS_PidChannelStatus status;

    NEXUS_PidChannel_GetStatus(pidChannel, &status);
    if (status.pid == 0 || status.pid > 0x1000) return;

    for (i=0;i<MAX_PIDS_PER_PB;i++) {
        if (!g_monitor[handle].pid[i].pidChannel) {

            BDBG_WRN(("b_add_monitor_pid %d: add pid %#x to slot %d", handle, status.pid, i));
            g_monitor[handle].pid[i].pid = status.pid;
            g_monitor[handle].pid[i].pidChannel = pidChannel;
            rc = NEXUS_Recpump_AddPidChannel(g_monitor[handle].recpump, pidChannel, NULL);
            BDBG_ASSERT(!rc);
            g_monitor[handle].pid[i].cc = -1;
            return;
        }
    }

    BDBG_WRN(("b_add_monitor_pid %d: unable to add pidChannel %p", handle, (void*)pidChannel));
}

void b_remove_monitor_pid_channel(int handle, NEXUS_PidChannelHandle pidChannel)
{
    NEXUS_Error rc;
    unsigned i;
    NEXUS_PidChannelStatus status;

    NEXUS_PidChannel_GetStatus(pidChannel, &status);
    if (status.pid == 0 || status.pid > 0x1000) return;

    for (i=0;i<MAX_PIDS_PER_PB;i++) {
        if (g_monitor[handle].pid[i].pidChannel == pidChannel) {
            BDBG_WRN(("b_remove_monitor_pid_channel %d: remove pid %#x to slot %d", handle, g_monitor[handle].pid[i].pid, i));
            rc = NEXUS_Recpump_RemovePidChannel(g_monitor[handle].recpump, g_monitor[handle].pid[i].pidChannel);
            BDBG_ASSERT(!rc);
            g_monitor[handle].pid[i].pidChannel = NULL;
            return;
        }
    }
    BDBG_WRN(("b_remove_monitor_pid %d: bad pidChannel %p", handle, (void*)pidChannel));
}
