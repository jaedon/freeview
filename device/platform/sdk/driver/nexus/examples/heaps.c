/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
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
 * $brcm_Workfile: heaps.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 6/12/12 10:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/heaps.c $
 * 
 * 11   6/12/12 10:50a erickson
 * SW7435-65: add NEXUS_MemoryType to print_heaps()
 *
 * 10   5/11/12 10:56a erickson
 * SW7435-170: add status
 *
 * 9   1/20/11 2:59p erickson
 * SW7408-193: add NEXUS_MemoryStatus.highWatermark and
 *  NEXUS_Heap_ResetHighWatermark
 *
 * 8   1/19/11 5:08p erickson
 * SW7422-220: allow heap[] array to be sparse
 *
 * 7   12/22/10 2:28p erickson
 * SW7422-2: add support for multiple physical address regions within a
 *  single MEMC
 *
 * 6   11/16/10 1:39p erickson
 * SWDEPRECATED-2425: add default heap print out, fix custom heaps for
 *  non-UMA 7405
 *
 * 5   10/28/10 1:00p erickson
 * SWDEPRECATED-2425: improve comments
 *
 * 4   2/25/10 9:41a erickson
 * SW7420-549: add NEXUS_MemoryStatus.alignment
 *
 * 3   2/24/10 4:15p erickson
 * SW3548-2721: add NEXUS_Heap_Dump
 *
 * 2   2/10/10 4:46p erickson
 * SW7405-3850: improve example
 *
 * 1   10/2/09 11:17a erickson
 * SWDEPRECATED-2425: add heap sample app
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_surface.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>

/**
This app queries the default heaps per platform, typically one heap per memory controller.
**/

static const char *b_memory_type_str(NEXUS_MemoryType memoryType) {
    static char g_str[32];
    switch (memoryType) {
    case NEXUS_MemoryType_eDriver: return "eDriver";
    case NEXUS_MemoryType_eFull: return "eFull";
    case NEXUS_MemoryType_eApplication: return "eApp";
    case NEXUS_MemoryType_eDeviceOnly: return "eDevice";
    default: BKNI_Snprintf(g_str, 32, "0x%x", memoryType); return g_str; /* NOTE: not reentrant */
    }
}

void print_heaps(void)
{
    NEXUS_PlatformConfiguration platformConfig;
    unsigned i;
    NEXUS_Error rc;

    /* call NEXUS_Platform_GetConfiguration to get the heap handles */
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* example of getting status about each heap */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_MemoryStatus status;
        NEXUS_HeapHandle heap;

        heap = platformConfig.heap[i];
        if (!heap) continue;
        rc = NEXUS_Heap_GetStatus(heap, &status);
        BDBG_ASSERT(!rc);
        printf("Heap[%d]: MEMC%d, physical addr 0x%08x, size %9d (0x%08x), alignment %2d, base ptr %p, high water %9d, mapping %s, guardbanding? %c\n",
            i, status.memcIndex, status.offset, status.size, status.size, status.alignment, status.addr,
            status.highWatermark,
            b_memory_type_str(status.memoryType),
            status.guardBanding?'y':'n');
    }
}

int main(void)
{
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Error rc;
    unsigned i;
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;
    NEXUS_SurfaceHandle surface;
    void *buffer;

    /* first, print default heaps. this is useful for system debug. */

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    print_heaps();

    printf("Press ENTER for custom heaps\n");
    getchar();
    NEXUS_Platform_Uninit();

    /* by default, nexus will create 1 heap per MEMC */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;

    /* This is sample code for user-specified heaps (>1 heap per MEMC).
    More combinations are possible. This set of heaps may not work on all systems. */
    BDBG_CASSERT(NEXUS_MAX_HEAPS > 2);
    platformSettings.heap[0].memcIndex = 0; /* MEMC 0 */
    platformSettings.heap[0].size = -1; /* remainder after allocating heaps 1 & 2 */
    platformSettings.heap[0].alignment = 1;
    /* skip [1] so that default heap[0] is MEMC0 and default heap[1] is MEMC1 (where present) */
    /* now start our custom heaps */
    platformSettings.heap[2].memcIndex = 0; /* MEMC 0 */
    platformSettings.heap[2].size = 10 * 1024 * 1024; /* 10MB heap */
    platformSettings.heap[3].memcIndex = 0; /* MEMC 0 */
    platformSettings.heap[3].size = 20 * 1024 * 1024; /* 20MB heap */
    platformSettings.heap[3].memoryType = NEXUS_MemoryType_eSecure; /* user can specify only one secure heap. Nexus will detect it and
                                                                       pass the appropriate heap handles to Magnum. */

    /* NEXUS_Platform_Init creates the heaps */
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    print_heaps();

    printf("Press ENTER to test allocations\n");
    getchar();

    /* app allocates from heap[0] */
    rc = NEXUS_Memory_Allocate(1000, NULL, &buffer);
    BDBG_ASSERT(!rc);
    BDBG_ASSERT(buffer);

    /* example of dumping information about each allocation and free block */
    NEXUS_Platform_GetConfiguration(&platformConfig);
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (platformConfig.heap[i]) {
            /* Nexus requires export debug_mem=y to track allocations. */
            printf("Heap[%d] dump:\n", i);
            NEXUS_Heap_Dump(platformConfig.heap[i]);
        }
    }

    NEXUS_Memory_Free(buffer);

    /* sample code for using heap handle for non-default heap usage */
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
    surfaceCreateSettings.width = 100;
    surfaceCreateSettings.height = 100;
    surfaceCreateSettings.heap = platformConfig.heap[2];
    surface = NEXUS_Surface_Create(&surfaceCreateSettings);
    /* app can use it */
    NEXUS_Surface_Destroy(surface);

    NEXUS_Platform_Uninit();

    return 0;
}
