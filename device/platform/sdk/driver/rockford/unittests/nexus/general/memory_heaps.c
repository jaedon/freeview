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
 * $brcm_Workfile: memory_heaps.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 6/1/09 1:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/memory_heaps.c $
 * 
 * Hydra_Software_Devel/4   6/1/09 1:23p erickson
 * PR48035: make test work with UMA and non-UMA systems
 *
 * Hydra_Software_Devel/3   5/5/09 3:04p erickson
 * PR42679: fix DTV
 *
 * Hydra_Software_Devel/2   3/30/09 2:54p erickson
 * PR53426: added more cases
 *
 * Hydra_Software_Devel/1   3/9/09 2:39p erickson
 * PR50748: added test
 *
 * Hydra_Software_Devel/1   1/12/09 3:45p erickson
 * PR48035: add memory test
 *
 *****************************************************************************/
#include <stdio.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_memory.h"

BDBG_MODULE(memory_heaps);

void print_heaps(void)
{
    NEXUS_PlatformConfiguration platformConfiguration;
    NEXUS_Error rc;
    unsigned i;

    printf("heap list\n");
    NEXUS_Platform_GetConfiguration(&platformConfiguration);
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_MemoryStatus status;
#if NEXUS_DTV_PLATFORM
        NEXUS_HeapHandle heap = platformConfiguration.memc[0].heap[i];
#else
        NEXUS_HeapHandle heap = platformConfiguration.heap[i];
#endif

        if (!heap) break;

        rc = NEXUS_Heap_GetStatus(heap, &status);
        BDBG_ASSERT(!rc);

        printf("heap %d %p: memc %d, size %d\n",
            i, (void*)heap, status.memcIndex, status.size);
    }
}

bool is_non_uma(void)
{
    NEXUS_PlatformConfiguration platformConfiguration;
    NEXUS_Error rc;
    unsigned i;

    NEXUS_Platform_GetConfiguration(&platformConfiguration);
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_MemoryStatus status;
#if NEXUS_DTV_PLATFORM
        NEXUS_HeapHandle heap = platformConfiguration.memc[0].heap[i];
#else
        NEXUS_HeapHandle heap = platformConfiguration.heap[i];
#endif

        if (!heap) break;

        rc = NEXUS_Heap_GetStatus(heap, &status);
        BDBG_ASSERT(!rc);

        if (status.memcIndex != 0) return true;
    }
    return false;
}

int main(void)
{
    NEXUS_Error rc;
    NEXUS_PlatformSettings platformSettings;
    bool nonUma;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    print_heaps();

    nonUma = is_non_uma();

    NEXUS_Platform_Uninit();

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
#if NEXUS_DTV_PLATFORM
    platformSettings.memc[0].heapSize[0] = 128 * 1024 * 1024;
    platformSettings.memc[0].heapSize[1] = 10 * 1024 * 1024;
    platformSettings.memc[0].heapSize[2] = -1;
#else
    platformSettings.heap[0].memcIndex = 0;
    platformSettings.heap[0].size = 128 * 1024 * 1024;
    platformSettings.heap[1].memcIndex = 0;
    platformSettings.heap[1].size = 10 * 1024 * 1024;
    platformSettings.heap[2].memcIndex = 0;
    platformSettings.heap[2].size = -1;
    if (nonUma) {
        platformSettings.videoDecoderModuleSettings.avdHeapIndex[0] = 3;
    }
#endif
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    print_heaps();
    NEXUS_Platform_Uninit();

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
#if NEXUS_DTV_PLATFORM
    platformSettings.memc[0].heapSize[0] = -1;
    platformSettings.memc[0].heapSize[1] = 15 * 1024 * 1024;
#else
    platformSettings.heap[0].memcIndex = 0;
    platformSettings.heap[0].size = -1;
    platformSettings.heap[1].memcIndex = 0;
    platformSettings.heap[1].size = 15 * 1024 * 1024;
    if (nonUma) {
        platformSettings.videoDecoderModuleSettings.avdHeapIndex[0] = 2;
    }
#endif
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    print_heaps();
    NEXUS_Platform_Uninit();

    return 0;
}

