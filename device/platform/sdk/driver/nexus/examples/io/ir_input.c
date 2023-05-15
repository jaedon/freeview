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
 * $brcm_Workfile: ir_input.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 2/4/09 2:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/io/ir_input.c $
 * 
 * 3   2/4/09 2:42p erickson
 * PR51841: reorganize examples
 *
 * 2   6/7/08 2:42p jgarrett
 * PR 43318: Removing dependency on frontend
 *
 * 1   3/24/08 7:01p mphillip
 * PR39982: Add UHF and IR input examples
 *
 *
 *****************************************************************************/
/* Example to get IR remote input using nexus */

#include "nexus_platform.h"
#include "nexus_ir_input.h"
#include <assert.h>
#include <stdio.h>

void irCallback(void *pParam, int iParam)
{
    size_t numEvents = 1;
    NEXUS_Error rc = 0;
    bool overflow;
    NEXUS_IrInputHandle irHandle = *(NEXUS_IrInputHandle *)pParam;
    BSTD_UNUSED(iParam);
    while (numEvents && !rc) {
        NEXUS_IrInputEvent irEvent;
        rc = NEXUS_IrInput_GetEvents(irHandle,&irEvent,1,&numEvents,&overflow);
        if (numEvents)
            printf("irCallback: rc: %d, code: %08x, repeat: %s\n", rc, irEvent.code, irEvent.repeat ? "true" : "false");
    }
}

int main(void)
{
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_IrInputHandle irHandle;
    NEXUS_IrInputSettings irSettings;

    NEXUS_Platform_Init(NULL);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_IrInput_GetDefaultSettings(&irSettings);
    irSettings.mode = NEXUS_IrInputMode_eRemoteA;
    irSettings.dataReady.callback = irCallback;
    irSettings.dataReady.context = &irHandle;
    irHandle = NEXUS_IrInput_Open(0, &irSettings);

    printf("Press buttons on the remote.  Pressing <ENTER> on the keyboard exits.");

    getchar();

    NEXUS_IrInput_Close(irHandle);
    NEXUS_Platform_Uninit();

    return 0;
}

