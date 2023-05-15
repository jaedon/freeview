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
 * $brcm_Workfile: ir_input_open.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 5/7/10 6:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/io/ir_input_open.c $
 * 
 * Hydra_Software_Devel/3   5/7/10 6:36p mward
 * SW7125-253:  Allow compilation with C++, eliminate "invalid conversion"
 * errors.
 * 
 * Hydra_Software_Devel/2   5/11/09 2:42p erickson
 * PR42679: only assume 1 ir input on all platforms
 *
 * Hydra_Software_Devel/1   2/27/09 4:33p erickson
 * PR51468: added ir_input_open
 *
 * Hydra_Software_Devel/1   11/6/08 2:15p erickson
 * PR42679: added sejin_ir test
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_ir_input.h"
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(ir_input_open);

void dataready(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(void)
{
    NEXUS_Error rc;
#define MAX_IR_INPUTS 5
    NEXUS_IrInputHandle irInput[MAX_IR_INPUTS];
    NEXUS_IrInputSettings irInputSettings;
    NEXUS_PlatformSettings platformSettings;
    unsigned i;

    for (i=0;i<10;i++) {
        unsigned loops = 25;

        NEXUS_Platform_GetDefaultSettings(&platformSettings);
        platformSettings.openFrontend = false;
        rc = NEXUS_Platform_Init(&platformSettings);
        BDBG_ASSERT(!rc);

        BKNI_Memset(irInput, 0, sizeof(irInput));

        while (loops--) {
            unsigned index = rand() % MAX_IR_INPUTS;
            if (!irInput[index]) {
                NEXUS_IrInput_GetDefaultSettings(&irInputSettings);
                irInputSettings.mode = NEXUS_IrInputMode_eSejin56KhzKbd;
                irInputSettings.dataReady.callback = dataready;
                irInputSettings.dataReady.context = NULL;
                irInputSettings.channel_number = index;
                BDBG_MSG(("open %d", index));
                irInput[index] = NEXUS_IrInput_Open(index, &irInputSettings);
                /* every platform should have at least 1. */
                if (index == 0) {
                    BDBG_ASSERT(irInput[index]);
                }
            }
            else {
                BDBG_MSG(("close %d", index));
                NEXUS_IrInput_Close(irInput[index]);
                irInput[index] = NULL;
            }
        }

        printf("uninit\n");
        NEXUS_Platform_Uninit();
    }
    printf("success\n");
    return 0;
}
