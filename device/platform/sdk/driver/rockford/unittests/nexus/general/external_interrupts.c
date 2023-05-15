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
 * $brcm_Workfile: external_interrupts.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/12/09 4:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/external_interrupts.c $
 * 
 * Hydra_Software_Devel/2   5/12/09 4:34p erickson
 * PR52109: modify app for settop support
 *
 * Hydra_Software_Devel/1   2/13/09 1:50p erickson
 * PR47116: added sample code to show how to connect a callback to
 * external interrupt in Nexus
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_platform_extint.h"

BDBG_MODULE(external_interrupts);

#if NEXUS_DTV_PLATFORM
void callback(void *context)
{
    BDBG_ASSERT(context == (void *)1);
    printf("callback\n");
}
#else
void callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BDBG_ASSERT(context == (void *)1);
    printf("callback\n");
}
#endif

int main(void)
{
    NEXUS_Error rc;
    unsigned i, j;

    rc = NEXUS_Platform_Init(NULL);
    BDBG_ASSERT(!rc);

#if NEXUS_DTV_PLATFORM
/**
See PR 52109 - rework Nexus Platform external interrupt interface.
The nexus platform external interrupt interface is subject to change.
**/
    rc = NEXUS_Platform_InitExtint();
    BDBG_ASSERT(!rc);

    for (i=0;i<20;i++) {
        NEXUS_EXTINT_Params params;

        printf("open\n");
        NEXUS_Platform_GetDefaultExtintParams(&params);
        params.pFunc = callback;
        params.context = (void *)1;
        rc = NEXUS_Platform_OpenExtint(NEXUS_ExtIntId_e0, &params);
        BDBG_ASSERT(!rc);

        for (j=0;j<10;j++) {
            rc = NEXUS_Platform_EnableExtint(NEXUS_ExtIntId_e0);
            BDBG_ASSERT(!rc);
            BKNI_Sleep(rand() % 100);
            (void)NEXUS_Platform_DisableExtint(NEXUS_ExtIntId_e0);
        }

        (void)NEXUS_Platform_CloseExtint(NEXUS_ExtIntId_e0);
        printf("close\n");
    }

    (void)NEXUS_Platform_UnInitExtint();
#else
    /* new interface only has enable/disable. it also uses NEXUS_CallbackDesc for kernel mode support. */
    for (i=0;i<20;i++) {
        NEXUS_ExternalInterruptSettings settings;

        NEXUS_Platform_GetDefaultExternalInterruptSettings(&settings);
        settings.callback.callback = callback;
        settings.callback.context = (void *)1;

        for (j=0;j<10;j++) {
            rc = NEXUS_Platform_EnableExternalInterrupt(0, &settings);
            BDBG_ASSERT(!rc);
            BKNI_Sleep(rand() % 100);
            NEXUS_Platform_DisableExternalInterrupt(0);
        }
    }
#endif

    NEXUS_Platform_Uninit();

    printf("success\n");
    return 0;
}

