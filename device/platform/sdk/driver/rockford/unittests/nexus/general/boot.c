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
 * $brcm_Workfile: boot.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 11/17/09 5:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/boot.c $
 * 
 * Hydra_Software_Devel/5   11/17/09 5:26p mphillip
 * SW7405-3034: Disable incorrect test on platform initialization with
 * frontend failure
 * 
 * Hydra_Software_Devel/4   5/22/09 11:36a vishk
 * PR 54842: rockford/unittests/nexus/general/boot should test
 * NEXUS_PlatformSettings options
 * 
 * Hydra_Software_Devel/3   5/13/09 4:55p jtna
 * PR54809: test unpaired init and uninits
 * 
 * Hydra_Software_Devel/2   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   10/23/08 4:00p erickson
 * PR42679: update
 *
 * 2   7/18/08 11:31a jgarrett
 * PR 44953: Removing -Wstrict-prototypes warnings
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "nexus_types.h"
#include "nexus_platform.h"

int main(void)
{
    unsigned loops = 3;
    NEXUS_Error errCode;
    NEXUS_PlatformSettings platformSettings;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);

    while (--loops) {
        fprintf(stderr, "Ready to initialize\n");

        errCode = NEXUS_Platform_Init(NULL);
        BDBG_ASSERT(!errCode);

        /* check unpaired init */
        errCode = NEXUS_Platform_Init(NULL);
        BDBG_ASSERT(errCode);

        fprintf(stderr, "Initialization complete\n");
        BKNI_Sleep(rand() % 2000);

        NEXUS_Platform_Uninit();
        /* check unpaired uninit */
        NEXUS_Platform_Uninit();

        fprintf(stderr, "Uninit complete\n");
        BKNI_Sleep(rand() % 2000);
    }

    printf("Test NEXUS_PlatformSettings options\n");
    printf("\nTest: openFrontend = false\n");
    platformSettings.openFrontend = false;  
    errCode = NEXUS_Platform_Init(&platformSettings);   
    BDBG_ASSERT(!errCode);
    NEXUS_Platform_Uninit();

    printf("\nTest: openFpga = false\n");
    platformSettings.openFrontend = true;   
    platformSettings.openFpga = false;  
    errCode = NEXUS_Platform_Init(&platformSettings);   
    BDBG_ASSERT(!errCode);
    NEXUS_Platform_Uninit();

#if 0
    /* Temporarily disabling this until it can be re-thought --
     * There are cases where failing to initialize the front-end is 
     * acceptable.  */
    printf("\nTest: openI2c = false\n");
    platformSettings.openFpga = true;
    platformSettings.openI2c = false;   
    errCode = NEXUS_Platform_Init(&platformSettings);   
    BDBG_ASSERT(errCode);
    if(!errCode) NEXUS_Platform_Uninit();
#endif

    printf("\nTest: cachedMemory = false\n");
    platformSettings.openI2c = true;
    platformSettings.cachedMemory = false;  
    errCode = NEXUS_Platform_Init(&platformSettings);   
    BDBG_ASSERT(!errCode);
    NEXUS_Platform_Uninit();
    
    printf("\nTest: openOutputs = false\n");
    platformSettings.cachedMemory = true;   
    platformSettings.openOutputs = false;   
    errCode = NEXUS_Platform_Init(&platformSettings);   
    BDBG_ASSERT(!errCode);
    NEXUS_Platform_Uninit();
    
    printf("\nTest: Everything set to false\n");
    platformSettings.openFrontend = false;
    platformSettings.openFpga = false;
    platformSettings.openI2c = false;
    platformSettings.cachedMemory = false;  
    platformSettings.openOutputs = false;   
    errCode = NEXUS_Platform_Init(&platformSettings);   
    BDBG_ASSERT(!errCode);
    NEXUS_Platform_Uninit();

    return 0;
}

