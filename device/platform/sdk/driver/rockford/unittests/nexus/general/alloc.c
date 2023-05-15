/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: alloc.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/3/11 2:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/alloc.c $
 * 
 * Hydra_Software_Devel/2   6/3/11 2:17p erickson
 * SW7405-5349: add alignment check
 * 
 * Hydra_Software_Devel/1   2/26/10 2:13p erickson
 * SW7325-655: added NEXUS_Memory_Allocate/Free test
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

BDBG_MODULE(alloc);

/**
This app queries the default heaps per platform, typically one heap per memory controller.
**/

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_Error rc;
    unsigned i;
    void *buffer;
    
    srand(time(NULL));

    /* by default, nexus will create 1 heap per MEMC */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);

    /* TODO: expand this test to include
    - multiple heaps
    - multiple allocations (avoiding fragmentation failures)
    - NEXUS_Memory_GetStatus check
    */
    for (i=0;i<100000;i++) {
        unsigned size;
        NEXUS_MemoryAllocationSettings settings;
        
        if (i%10000==0) printf("test %d\n", i);
        if (rand() % 100 == 0) {
            /* test boundary case extra */
            size = rand() % 100;
        }
        else {
            size = rand() % 1000000;
        }
        if (!size) continue;

        NEXUS_Memory_GetDefaultAllocationSettings(&settings);
        settings.alignment = 1 << (rand() % 8);
        rc = NEXUS_Memory_Allocate(size, &settings, &buffer);
        BDBG_ASSERT(!rc);
        BDBG_ASSERT(buffer);
        if (((unsigned)buffer & (settings.alignment-1)) != 0) {
            BDBG_ERR(("buffer %p, alignment %d", buffer, settings.alignment));
        }
        BDBG_ASSERT(((unsigned)buffer & (settings.alignment-1)) == 0);

        NEXUS_Memory_Free(buffer);
    }

    NEXUS_Platform_Uninit();

    return 0;
}
