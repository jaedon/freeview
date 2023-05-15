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
 * $brcm_Workfile: ir_blaster.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/30/10 11:02a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/io/ir_blaster.c $
 * 
 * Hydra_Software_Devel/2   7/30/10 11:02a erickson
 * SW7400-2845: speed up
 *
 * Hydra_Software_Devel/1   7/30/10 10:55a erickson
 * SW7400-2845: add ir_blaster stress test
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_ir_blaster.h"
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(ir_blaster);

void transmit_complete(void *context, int param)
{
    NEXUS_IrBlasterHandle irBlaster = context;
    NEXUS_IrBlasterStatus status;
    NEXUS_Error rc;

    BSTD_UNUSED(param);
    rc = NEXUS_IrBlaster_GetStatus(irBlaster, &status);
    BDBG_ASSERT(!rc);

    BKNI_Sleep(rand() % 50);
}

#ifndef NEXUS_NUM_IR_BLASTERS
#define NEXUS_NUM_IR_BLASTERS 1
#endif

int main(void)
{
    NEXUS_Error rc;
    NEXUS_IrBlasterHandle irBlaster[NEXUS_NUM_IR_BLASTERS];
    unsigned i;

    srand(time(NULL));
    NEXUS_Platform_Init(NULL);
    BKNI_Memset(irBlaster, 0, sizeof(irBlaster));

    for (i=0;i<5000;i++) {
        unsigned index = rand() % NEXUS_NUM_IR_BLASTERS;
        switch (rand() % 7) {
        case 0:
            if (!irBlaster[index]) {
                if (rand() % 2) {
                    NEXUS_IrBlasterSettings settings;
                    NEXUS_IrBlaster_GetDefaultSettings(&settings);
                    settings.mode = rand() % NEXUS_IrBlasterMode_eMax;
                    irBlaster[index] = NEXUS_IrBlaster_Open(index, &settings);
                    BDBG_ASSERT(irBlaster[index]);
                }
                else {
                    irBlaster[index] = NEXUS_IrBlaster_Open(index, NULL);
                    BDBG_ASSERT(irBlaster[index]);
                }
                BDBG_WRN(("open IrBlaster[%d]", index));
            }
            break;
        case 1:
            if (irBlaster[index] && rand()%8==0) {
                NEXUS_IrBlaster_Close(irBlaster[index]);
                irBlaster[index] = NULL;
                BDBG_WRN(("close IrBlaster[%d]", index));
            }
            break;
        case 2:
            if (irBlaster[index]) {
                NEXUS_IrBlasterSettings settings;
                NEXUS_IrBlaster_GetSettings(irBlaster[index], &settings);
                settings.transmitComplete.callback = rand() %2 ? transmit_complete : NULL;
                settings.transmitComplete.context = irBlaster[index];
                settings.mode = rand() % NEXUS_IrBlasterMode_eMax;
                rc = NEXUS_IrBlaster_SetSettings(irBlaster[index], &settings);
                BDBG_ASSERT(!rc);
            }
            break;
        case 3:
            if (irBlaster[index]) {
                NEXUS_IrBlasterStatus status;
                rc = NEXUS_IrBlaster_GetStatus(irBlaster[index], &status);
                BDBG_ASSERT(!rc);
                BDBG_MSG(("status[%d]: busy=%d", index, status.busy));
            }
            break;
        case 4:
            if (irBlaster[index]) {
#if 0
                uint32_t data;
                rc = NEXUS_IrBlaster_Send(irBlaster[index], &data, rand() % 19, rand() % 2);
                BDBG_ASSERT(!rc);
#endif
            }
            break;
        case 5:
            if (irBlaster[index]) {
                rc = NEXUS_IrBlaster_Abort(irBlaster[index]);
                BDBG_ASSERT(!rc);
            }
            break;
        case 6:
            if (irBlaster[index]) {
                rc = NEXUS_IrBlaster_Resend(irBlaster[index]);
                BDBG_ASSERT(!rc);
            }
            break;
        }
        if (rand() % 10 == 0) {
            BKNI_Sleep(rand() % 100);
        }
    }

    for (i=0;i<NEXUS_NUM_IR_BLASTERS;i++) {
        if (irBlaster[i]) {
            NEXUS_IrBlaster_Close(irBlaster[i]);
        }
    }

    NEXUS_Platform_Uninit();
    printf("success\n");
    return 0;
}
