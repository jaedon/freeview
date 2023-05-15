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
 * $brcm_Workfile: memory.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 6/27/12 2:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/general/memory.c $
 * 
 * Hydra_Software_Devel/11   6/27/12 2:39p mward
 * SW7435-238:  SW7420-2315: 7400 is marginal for alloc time too.
 * Increase by 1 sec.
 * 
 * Hydra_Software_Devel/10   6/26/12 4:32p mward
 * SW7435-238:  SW7420-2315:  Increase time limit to allower for slower
 * random-order frees.
 * 
 * Hydra_Software_Devel/9   6/21/12 4:54p vsilyaev
 * SW7420-2315: Resolved warnings
 * 
 * Hydra_Software_Devel/8   6/21/12 3:58p vsilyaev
 * SW7420-2315: Added client version of the memory allocator benchmark
 * 
 * Hydra_Software_Devel/7   6/21/12 11:58a vsilyaev
 * SW7420-2315: Free memory in random order to prevent 'test specific'
 * optimizations
 * 
 * Hydra_Software_Devel/6   6/20/12 11:22a erickson
 * SW7405-5350: skip openFrontend, fail if NEXUS_Memory_Allocate fails
 * 
 * Hydra_Software_Devel/5   9/9/11 5:19p erickson
 * SW7405-5350: now that BMEM_AllocAligned/Free are fast, I've added the
 * time assertions for unittest
 * 
 * Hydra_Software_Devel/4   6/3/11 2:17p erickson
 * SW7405-5349: update memory test, add optional profiling
 * 
 * Hydra_Software_Devel/3   1/12/11 2:42p mward
 * SW7422-2:  NEXUS_Memory_GetStatus() eliminated.  Use
 * NEXUS_Heap_GetStatus().
 * 
 * Hydra_Software_Devel/2   4/13/09 12:52p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/1   1/12/09 3:45p erickson
 * PR48035: add memory test
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_memory.h"

BDBG_MODULE(memory);

void print_status(void)
{
    NEXUS_Error rc;
    NEXUS_MemoryStatus status;
    NEXUS_PlatformConfiguration platformConfig;

    NEXUS_Platform_GetConfiguration(&platformConfig);

    rc = NEXUS_Heap_GetStatus(platformConfig.heap[0],&status);
    BDBG_ASSERT(!rc);

    printf("memory: size=%d, free=%d\n",
        status.size,
        status.free);
}

unsigned b_get_time(void)
{
#if defined(LINUX)
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec/1000;
#elif defined(VXWORKS)
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);
    return tv.tv_sec*1000 + tv.tv_nsec/1000/1000;
#elif defined(NEXUS_BASE_OS_ucos)
    const unsigned msPerTick = 10;
    long osTicks = OSTimeGet();
    return (osTicks * msPerTick);
#else
#error Please implement b_get_time for your OS
#endif
}

/* build with export BPROFILE_SUPPORT=y to enable profiling of memory allocation */
#if !BPROFILE_SUPPORT
/* if BPROFILE_SUPPORT not enabled, stub out functions to prevent error */
#define NEXUS_Profile_Start()
#define NEXUS_Profile_Stop(X)
#endif

#if NEXUS_MODE_client
/* client mode compiled as rockford/unittests/nexus/multiprocess/memory_client.c */
#include <string.h>
int main(int argc, char **argv)
#else
int main(void)
#endif
{
    NEXUS_Error rc;
#define MAX_ALLOCS 30000
    void *ptr[MAX_ALLOCS];
    unsigned i;
    unsigned starttime, totaltime;
    bool random=true;

#if NEXUS_MODE_client
    NEXUS_ClientAuthenticationSettings authSettings;
    unsigned client_index = 0;

    if (argc > 1) {
        client_index = atoi(argv[1]);
    }

    NEXUS_Platform_GetDefaultClientAuthenticationSettings(&authSettings);
    sprintf((char*)authSettings.certificate.data, "testclient%d", client_index);
    authSettings.certificate.length = strlen((const char *)authSettings.certificate.data)+1;
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) {
        fprintf(stderr, "client %d(%d) NEXUS_Platform_Join failed: %d\n", client_index, getpid(), rc);
        return rc;
    }
    BDBG_WRN(("client %d(%d) joined", client_index, getpid()));
#else /* #if NEXUS_MODE_client */
    NEXUS_PlatformSettings platformSettings;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
#endif /* #if NEXUS_MODE_client */

    BKNI_Memset(ptr, 0, sizeof(ptr));

    BDBG_WRN(("start alloc"));
    NEXUS_Profile_Start();
    starttime = b_get_time();
    for (i=0;i<MAX_ALLOCS;) {
        rc = NEXUS_Memory_Allocate(10, NULL, &ptr[i]);
        BDBG_ASSERT(!rc);
        if (++i % 1000 == 0) BDBG_WRN(("%d allocations...", i));
    }
    totaltime = b_get_time() - starttime;
    NEXUS_Profile_Stop("alloc_memory");
    BDBG_WRN(("%d allocations took %d.%03d seconds", MAX_ALLOCS, totaltime/1000, totaltime%1000));
#if NEXUS_MODE_client
    BDBG_ASSERT(totaltime <= 25000);
#else
    /* 97413 B0 took 10 seconds after BMEM rework */
    BDBG_ASSERT(totaltime <= 16000);
    print_status();
#endif

    BDBG_WRN(("start free"));
    NEXUS_Profile_Start();
    starttime = b_get_time();
    for (i=0;i<MAX_ALLOCS;) {
        unsigned n,j;
        n = random ? (unsigned)rand() : i;
        for(j=0;j<MAX_ALLOCS;j++) {
            unsigned k = (n+j)%MAX_ALLOCS;
            if (ptr[k]) {
                NEXUS_Memory_Free(ptr[k]);
                ptr[k] = NULL;
                break;
            }
        }
        if (++i % 1000 == 0) BDBG_WRN(("%d frees...", i));
    }
    totaltime = b_get_time() - starttime;
    NEXUS_Profile_Stop("free_memory");
    BDBG_WRN(("%d frees took %d.%03d seconds", MAX_ALLOCS, totaltime/1000, totaltime%1000));
#if NEXUS_MODE_client
    BDBG_ASSERT(totaltime <= 100000);
#else
    /* 97413 B0 took 7.6 seconds after BMEM rework */
    BDBG_ASSERT(totaltime <= 125000);
    print_status();
#endif

    NEXUS_Platform_Uninit();

    printf("success\n");
    return 0;
}

