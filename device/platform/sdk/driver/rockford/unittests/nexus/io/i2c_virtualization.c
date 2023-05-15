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
 * $brcm_Workfile: i2c_virtualization.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/6/12 11:22a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/io/i2c_virtualization.c $
 * 
 * Hydra_Software_Devel/1   8/6/12 11:22a erickson
 * SW7231-929: add unittest for i2c_virtualization
 * 
 * Hydra_Software_Devel/1   2/18/09 9:37a erickson
 * PR52223: add i2c app
 *
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_i2c.h"
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(i2c_virtualization);

int main(void)
{
    NEXUS_Error rc;
    NEXUS_PlatformSettings platformSettings;
    unsigned loops = 20000;
    unsigned refcnt[NEXUS_NUM_I2C_CHANNELS]; /* keep an external refcnt so we can anticipate SetSettings success/failure */
    struct {
        NEXUS_I2cHandle i2c;
        unsigned channelIndex;
        bool first;
#define MAX_HANDLES 1000
    } g_handles[MAX_HANDLES];
    unsigned index;
    struct {
        unsigned open, close, set, read;
    } stats; 

    srand(time(NULL));
    memset(g_handles, 0, sizeof(g_handles));
    memset(&stats, 0, sizeof(stats));
    memset(&refcnt, 0, sizeof(refcnt));
    
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);

    while (loops--) {
        index = rand() % MAX_HANDLES;
        switch (rand() % 4) {
        case 0:
            if (!g_handles[index].i2c) {
                g_handles[index].channelIndex = rand() % NEXUS_NUM_I2C_CHANNELS;
                g_handles[index].i2c = NEXUS_I2c_Open(g_handles[index].channelIndex, NULL);
                BDBG_ASSERT(g_handles[index].i2c);
                g_handles[index].first = (refcnt[g_handles[index].channelIndex]++ == 0);
                stats.open++;
            }
            break;
        case 1:
            if (g_handles[index].i2c) {
                NEXUS_I2c_Close(g_handles[index].i2c);
                g_handles[index].i2c = NULL;
                stats.close++;
                BDBG_ASSERT(refcnt[g_handles[index].channelIndex]);
                refcnt[g_handles[index].channelIndex]--;
            }
            break;
        case 2:
            if (g_handles[index].i2c) {
                NEXUS_I2cSettings settings;
                NEXUS_I2c_GetSettings(g_handles[index].i2c, &settings);
                settings.clockRate = rand() % NEXUS_I2cClockRate_eMax;
                settings.interruptMode = rand() % 2;
                settings.burstMode = rand() % 2;
                settings.fourByteXferMode = rand() % 2;
                if (g_handles[g_handles[index].channelIndex].first) {
                    rc = NEXUS_I2c_SetSettings(g_handles[index].i2c, &settings);
                    BDBG_ASSERT(!rc);
                } 
                else if (rand() % 200 == 0) { /* don't flood console with failures */
                    BDBG_WRN(("expect failure"));
                    rc = NEXUS_I2c_SetSettings(g_handles[index].i2c, &settings);
                    BDBG_ASSERT(rc);
                }
                stats.set++;
            }
            break;
        case 3:
            if (g_handles[index].i2c) {
                uint8_t data[1];
                rc = NEXUS_I2c_Read(g_handles[index].i2c, 0, 0, data, 1);
                /* rc may fail */
                stats.read++;
            }
            break;
        }
    }
    for (index=0;index<MAX_HANDLES;index++) {
        if (g_handles[index].i2c) {
            NEXUS_I2c_Close(g_handles[index].i2c);
            stats.close++;
        }
    }
    
    BDBG_WRN(("stats: %d opens, %d closes, %d sets, %d reads", stats.open, stats.close, stats.set, stats.read));

    NEXUS_Platform_Uninit();
    printf("success\n");
    return 0;
}
