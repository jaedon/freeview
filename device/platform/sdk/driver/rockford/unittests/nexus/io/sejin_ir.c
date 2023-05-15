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
 * $brcm_Workfile: sejin_ir.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/6/08 2:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/io/sejin_ir.c $
 * 
 * Hydra_Software_Devel/1   11/6/08 2:15p erickson
 * PR42679: added sejin_ir test
 *
 *****************************************************************************/
#include <stdio.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_ir_input.h"
#include "bstd.h"
#include "bkni.h"

void dataready(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(void)
{
    NEXUS_Error rc;
#define TOTAL_IR_INPUTS 2
    NEXUS_IrInputHandle irInput[TOTAL_IR_INPUTS];
    NEXUS_IrInputSettings irInputSettings;
    BKNI_EventHandle event;
    NEXUS_PlatformSettings platformSettings;
    unsigned loops = 100;
    unsigned j;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    BKNI_CreateEvent(&event);

    NEXUS_IrInput_GetDefaultSettings(&irInputSettings);
    irInputSettings.mode = NEXUS_IrInputMode_eSejin56KhzKbd;
    irInputSettings.dataReady.callback = dataready;
    irInputSettings.dataReady.context = event;
    irInput[0] = NEXUS_IrInput_Open(0, &irInputSettings);

    NEXUS_IrInput_GetDefaultSettings(&irInputSettings);
    irInputSettings.mode = NEXUS_IrInputMode_eRemoteA;
    irInputSettings.dataReady.callback = dataready;
    irInputSettings.dataReady.context = event;
    irInput[1] = NEXUS_IrInput_Open(1, &irInputSettings);

    printf("waiting for IR remote or Sejin keyboard input...\n");
    while (loops) {
        bool got_data = false;
        for (j=0;j<TOTAL_IR_INPUTS;j++) {
            bool overflow;
            NEXUS_IrInputEvent irEvent;
            unsigned num;
            rc = NEXUS_IrInput_GetEvents(irInput[j], &irEvent, 1, &num, &overflow);
            BDBG_ASSERT(!rc);
            if (num) {
                printf("ir%d event: %d %s\n", j, irEvent.code, irEvent.repeat?"repeat":"");
                got_data = true;
            }
        }

        if (!got_data) {
            BKNI_WaitForEvent(event, 600); /* 600 * 100 = 1 minute */
            loops--;
        }
    }

    for (j=0;j<TOTAL_IR_INPUTS;j++) {
        NEXUS_IrInput_Close(irInput[j]);
    }
    NEXUS_Platform_Uninit();
    return 0;
}
