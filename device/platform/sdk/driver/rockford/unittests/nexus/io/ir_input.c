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
 * $brcm_Workfile: ir_input.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/4/10 11:27a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/io/ir_input.c $
 * 
 * Hydra_Software_Devel/1   8/4/10 11:27a erickson
 * SW7400-2843: add ir_input test apps
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_ir_input.h"
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(ir_input);

#ifndef NEXUS_NUM_IR_INPUTS
#define NEXUS_NUM_IR_INPUTS 10
#endif

#define NUM_CHANNELS 10

static void rand_settings(NEXUS_IrInputSettings *pSettings)
{
    pSettings->mode = rand() % NEXUS_IrInputMode_eMax;
    pSettings->eventQueueSize = rand() % 20 + 1;
    pSettings->channel_number = rand() % NUM_CHANNELS;
    pSettings->useCustomSettings = (rand()%10 == 0);
    pSettings->repeatFilterTime = rand()%1000;
}

int main(void)
{
    NEXUS_Error rc;
    NEXUS_IrInputHandle irInput[NEXUS_NUM_IR_INPUTS];
    unsigned i;

    srand(time(NULL));
    NEXUS_Platform_Init(NULL);
    BKNI_Memset(irInput, 0, sizeof(irInput));

    for (i=0;i<5000;i++) {
        unsigned index = rand() % NEXUS_NUM_IR_INPUTS;
        switch (rand() % 5) {
        case 0:
            if (!irInput[index]) {
                BDBG_WRN(("open IrInput[%d]", index));
                if (rand() % 2) {
                    NEXUS_IrInputSettings settings;
                    NEXUS_IrInput_GetDefaultSettings(&settings);
                    rand_settings(&settings);
                    irInput[index] = NEXUS_IrInput_Open(index, &settings);
                    /* we expect failure here because of internal interruptDevice collisions */
                }
                else {
                    irInput[index] = NEXUS_IrInput_Open(index, NULL);
                    /* we expect failure here because of internal interruptDevice collisions */
                }
            }
            break;
        case 1:
            if (irInput[index]) {
                NEXUS_IrInput_Close(irInput[index]);
                irInput[index] = NULL;
                BDBG_WRN(("close IrInput[%d]", index));
            }
            break;
        case 2:
            if (irInput[index]) {
                NEXUS_IrInputSettings settings;
                NEXUS_IrInput_GetSettings(irInput[index], &settings);
                rand_settings(&settings);
                rc = NEXUS_IrInput_SetSettings(irInput[index], &settings);
                /* we expect failure here because of internal interruptDevice collisions */
            }
            break;
        case 3:
            if (irInput[index]) {
#define MAX_EVENTS 10
                NEXUS_IrInputEvent events[MAX_EVENTS];
                unsigned num;
                bool overflow;

                rc = NEXUS_IrInput_GetEvents(irInput[index], events, rand() % MAX_EVENTS, &num, &overflow);
                BDBG_ASSERT(!rc);
                BDBG_ASSERT(num <= MAX_EVENTS);
            }
            break;
        case 4:
            if (irInput[index]) {
                NEXUS_IrInput_FlushEvents(irInput[index]);
            }
            break;
        }
        if (rand() % 10 == 0) {
            BKNI_Sleep(rand() % 100);
        }
    }

    for (i=0;i<NEXUS_NUM_IR_INPUTS;i++) {
        if (irInput[i]) {
            NEXUS_IrInput_Close(irInput[i]);
        }
    }

    NEXUS_Platform_Uninit();
    printf("success\n");
    return 0;
}
