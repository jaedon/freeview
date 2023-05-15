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
 * $brcm_Workfile: ir_input_client.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 12/7/11 10:46a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/multiprocess/ir_input_client.c $
 * 
 * Hydra_Software_Devel/3   12/7/11 10:46a erickson
 * SW7420-1148: add assertion
 * 
 * Hydra_Software_Devel/2   10/12/11 12:24p erickson
 * SW7420-1991: fix warn
 * 
 * Hydra_Software_Devel/1   7/29/11 12:06p erickson
 * SW7420-1991: add multiprocess tests for nelem and nelem_out attributes
 * 
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_ir_input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void irCallback(void *pParam, int iParam)
{
    NEXUS_IrInputHandle irHandle = *(NEXUS_IrInputHandle *)pParam;
    BSTD_UNUSED(iParam);
    while (1) {
        size_t numEvents;
        NEXUS_Error rc;
        bool overflow;
#define MAX_EVENTS 16
        NEXUS_IrInputEvent irEvent[MAX_EVENTS];
        unsigned i;
        rc = NEXUS_IrInput_GetEvents(irHandle, irEvent, MAX_EVENTS, &numEvents, &overflow);
        BDBG_ASSERT(!rc);
        printf("irCallback: rc: %d, num %d\n", rc, numEvents);
        if (!numEvents) break;
        for (i=0;i<numEvents;i++) {
            printf("   %d: code=%08x repeat=%s\n", i, irEvent[i].code, irEvent[i].repeat ? "true" : "false");
        }
    }
    
    BKNI_Sleep(rand() % 500); /* allow queue to grow to variable sizes */
}

int main(void)
{
    NEXUS_IrInputHandle irHandle;
    NEXUS_IrInputSettings irSettings;
    NEXUS_Error rc;
    
    srand(time(NULL));

    rc = NEXUS_Platform_Join();
    if (rc) return -1;

    NEXUS_IrInput_GetDefaultSettings(&irSettings);
    irSettings.mode = NEXUS_IrInputMode_eRemoteA;
    irSettings.dataReady.callback = irCallback;
    irSettings.dataReady.context = &irHandle;
    irHandle = NEXUS_IrInput_Open(0, &irSettings);
    BDBG_ASSERT(irHandle);

    BKNI_Sleep(30*1000);
        
    NEXUS_IrInput_Close(irHandle);
    NEXUS_Platform_Uninit();

    return 0;
}

