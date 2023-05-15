/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: boot_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 3/5/12 4:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/boot_stress.c $
 * 
 * Hydra_Software_Devel/3   3/5/12 4:55p jtna
 * SW7425-2362: add option to randomly mix in some NEXUS_Platform_Uninit's
 * 
 * Hydra_Software_Devel/2   3/2/12 4:42p jtna
 * SW7425-2362: update comment
 * 
 * Hydra_Software_Devel/1   3/2/12 4:37p jtna
 * SW7425-2362: add boot_stress.c
 * 
 *****************************************************************************/

/* stress test of NEXUS_Platform_Init(). the idea is to test whether we can init 
   repeatedly without a proper shutdown (NEXUS_Platform_Uninit()).
   run this in a shell script loop, e.g. while true; do nexus boot_stress; done; */
 
#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "nexus_types.h"
#include "nexus_platform.h"

BDBG_MODULE(boot_stress);

int main(void)
{
    NEXUS_Error rc;
    NEXUS_PlatformSettings platformSettings;
    unsigned sleep;
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;

    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);

    srand(time(NULL));
    sleep = rand() % 2000;
    BKNI_Printf("Sleep: %u\n", sleep);
    BKNI_Sleep(sleep);
    
#if 1
    /* mix in some uninits */
    if (sleep%4==0) { /* 1-in-4 chance */
        BKNI_Printf("Uninit\n");
        NEXUS_Platform_Uninit();
    }
    else {
        BKNI_Printf("Skip\n");
    }
#endif

    return 0;
}

