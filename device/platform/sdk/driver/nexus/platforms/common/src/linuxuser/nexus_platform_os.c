/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
*
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_platform_os.c $
* $brcm_Revision: 78 $
* $brcm_Date: 10/12/12 3:41p $
*
* API Description:
*   API name: Platform linuxuser
*    linuxuser OS routines
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/linuxuser/nexus_platform_os.c $
* 
* 78   10/12/12 3:41p erickson
* SW7435-419: use LOG for proc
* 
* 77   10/9/12 5:27p erickson
* SW7435-364: use BDBG_WRN for runaway L1 in usermode
* 
* 76   10/9/12 5:21p erickson
* SWDEPRECATED-3811: support "all" for proc
* 
* 75   9/26/12 11:01a erickson
* SW7435-364: improve printout for runaway L1 interrupts
* 
* 74   9/25/12 9:47a erickson
* SW7435-364: monitor for runaway L1 interrupts
*
* 73   9/24/12 3:59p erickson
* SW7435-362: moved lock/unlock to NEXUS_Platform_Uninit
* 
* 72   9/19/12 10:27a erickson
* SW7435-362: lock/unlock module around NEXUS_CancelTimer
*
* 70   8/7/12 4:29p vsilyaev
* SW7435-287: Added stack depth statistics gathering for the ISR
*  processing
*
* 69   7/20/12 10:56a ahulse
* SWDEPRECATED-3811: allow token separated input to proc debug
*
* 68   7/13/12 11:57a erickson
* SW7552-303: use OS-reported max_dcache_line_size instead of nexus macro
*
* 67   7/9/12 11:29a erickson
* SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
*
* 66   2/28/12 11:33a mward
* SW7435-7:  Fix assert for NEXUS_NUM_L1_REGISTERS > 3
*
* SW7435-7/3   1/4/12 5:48p mward
* SW7125-7:  Missed some handling of w3 registers.  Built and ran on
*  7125, which has NEXUS_NUM_L1_REGISTERS 3.
*
* SW7435-7/2   1/4/12 5:31p mward
* SW7435-7:  Merge from main.
*
* SW7435-7/1   11/3/11 5:45p mward
* SW7435-7:  Support NEXUS_NUM_L1_REGISTERS 4.
*
* 64   12/22/11 5:42p vsilyaev
* SW7425-2039: Collect statistics for the ISR handler
*
* 63   12/22/11 3:54p vsilyaev
* SW7425-2039: Added for 7425 (BMIPS5000/Zephyr) performance counter
*  acquisition
*
* 62   9/7/11 10:27a erickson
* SW7231-360: change definition of bmem_override to be directory
*
* 61   8/29/11 12:09p erickson
* SW7231-358: replace fscanf with fgets and sscanf to prevent buffer
*  overrun
*
* 60   8/3/11 5:33p vsilyaev
* SW7420-1465, SW7405-5221: Route debug output to the special FIFO
*  instead of syslog buffer
*
* 59   7/11/11 11:43a erickson
* SWDTV-7208: DTV's NEXUS_Platform_P_GetHostMemory should return standard
*  NEXUS_PlatformMemory. must be hardcoded for current version of DTV
*  linux.
*
* 58   6/6/11 3:27p vsilyaev
* SW7405-4477: Routed all debug output through buffer and use external
*  application to extract and print debug output
*
* 57   5/31/11 2:07p erickson
* SW7420-1907: clean up on bcm_driver ioctl failure
*
* 56   5/16/11 8:24p jhaberf
* SWDTV-6928: resolved build error for DTV chips
*
* 55   5/16/11 4:35p erickson
* SW7125-949: remove linux kernel test. perform runtime test.
*
* 54   5/16/11 7:02p jhaberf
* SWDTV-6928: Merge of 35233 platform code to the main
*
* 53   5/13/11 4:12p erickson
* SWDTV-6386: call prctl to set thread name
*
* 52   4/22/11 5:52p vsilyaev
* SW7405-5221: By default disable debug logging
*
* 51   4/13/11 7:35p vsilyaev
* SW7405-5221: Allow user to control size of the debug fifo
*
* 50   4/4/11 6:14p vsilyaev
* SW7405-5221: Added option to forward debug output to FIFO
*
* 49   3/16/11 10:27a erickson
* SW7420-1642: use NEXUS_Base_Settings.driverModuleInit instead of
*  NEXUS_Platform_P_InitDone
*
* 48   1/24/11 4:29p erickson
* SW7422-2: change bmem_override to be printf format string
*
* 47   1/3/11 10:34a erickson
* SW7422-2: add bmem_override option
*
* 46   12/27/10 10:40a erickson
* SW7422-2: fix ERR
*
* 45   12/23/10 4:07p erickson
* SW7422-2: add error messages and errno
*
* 44   12/22/10 5:08p mward
* SW7422-2:  Missing declaration of pFile.
*
* 43   12/22/10 2:33p erickson
* SW7422-2: add support for multiple physical address regions within a
*  single MEMC
*
* 41   12/20/10 4:26p erickson
* SW7422-2: rework nexus_platform_core, add board-specific file and API,
*  mmap after determining heaps
*
* 40   11/9/10 7:27p jhaberf
* SW35230-2163: Fixed build warning for the 35230 DTV chip
*
* 39   11/3/10 11:42a katrep
* SW7422-85: Fixed compiler warning for non 2630 kernels
*
* 38   11/2/10 5:47p spothana
* SW7422-85: 7420 does not have memc1 proc file.
*
* 37   10/29/10 5:23p nickh
* SW7422-85: Add support for sharing MEMC1 with Linux
*
* 36   9/20/10 4:45p jhaberf
* SW35230-1190: Read BMEM location/size from /proc/bmem for 2.6.28 kernel
*  if BMEM_RELOCATABLE is defined
*
* 35   9/1/10 3:07p gmohile
* SWDEPRECATED-3811 : Add debug support using procfs for usermode
*
* 34   8/4/10 2:04p erickson
* SW7400-2865: improve DBG
*
* 33   7/14/10 10:29a erickson
* SW7405-4621: rename to NEXUS_LARGEST_CPU_CACHE_LINE without MEMC param
*
* 32   2/4/10 5:37p jgarrett
* SW7408-57: Refactoring soft audio startup of TP1
*
* 31   1/15/10 6:02p jgarrett
* SW7408-17: Adding backtrace support for TP1
*
* 30   12/14/09 3:37p erickson
* SW7420-70: detect failures in NEXUS_Platform_P_GetMainMemory and return
*  an error
*
* 29   11/13/09 5:00p erickson
* SW7325-250: add nexus_platform_standby support
*
* 28   11/11/09 9:46a erickson
* SW7420-248: remove BCHP_CHIP == 7420 code that was no different from
*  general code
*
* 27   8/27/09 8:13p mward
* SW7125-4: Try 2.6.31 method for main memory first.
*
* 26   8/25/09 1:36p erickson
* SW7420-228: don't require every MEMC to be mmapped
*
* 25   8/25/09 12:24p erickson
* SW7420-228: rework platform memory mapping to allow for multiple MEMC's
*  on 7420
*
* 24   8/17/09 3:59p jrubio
* PR55232: add support for 2630 kernels
*
* 23   8/17/09 2:44p nickh
* PR56017: Add support for L2 disabled in B0
*
* 22   7/29/09 4:47p nickh
* PR56017: Add 7420B0 support
*
* 21   6/24/09 12:00a mphillip
* PR54926: Add a hook to allow customization at the end of
*  NEXUS_Platform_Init
*
* 20   2/5/09 10:47a nickh
* PR51382: Allow 7420A1 builds to work on A0 and A1 chip samples
*
* 18   1/28/09 7:20p vsilyaev
* PR 51392: Fixed typo
*
* 17   1/28/09 6:27p vsilyaev
* PR 51392: Moved magnum initialization from Nexus base to the platform.
*  Initialize only magnum layer inside of
*  NEXUS_Platform_GetDefaultSettings
*
* 16   1/26/09 12:05p erickson
* PR51468: global variable naming convention
*
* 15   11/3/08 5:35p vsilyaev
* PR 48443: Reapply thread priority after pthread_create
*
* 14   9/23/08 4:16p erickson
* PR46950: add NEXUS_Platform_P_AtomicUpdateCallback_isr
*
* 13   9/22/08 5:20p erickson
* PR45432: add check to ensure MEM alignment matches HW dcache line size
*
* 12   9/22/08 5:17p erickson
* PR47192: remove support for old usermode driver. 93549/93556 now uses
*  the new driver.
*
* 11   7/28/08 3:22p erickson
* PR45197: catch likely mem misconfiguration
*
* 10   7/24/08 11:24a erickson
* PR44816: add not_realtime_isr option for debug
*
* 9   7/23/08 2:15p erickson
* PR40198: fix warning
*
* 8   7/18/08 1:46p erickson
* PR44947: fix comment
*
* 7   5/28/08 10:18a erickson
* PR34925: added b_get_umdrv_fd for test
*
* 6   5/14/08 11:18a erickson
* PR42679: add BRCM_IOCTL_CHANGE_REQUEST to that NEXUS_Platform_Uninit
*  returns right away
*
* 5   4/8/08 10:40a vsilyaev
* PR 41083: Added bprofile support
*
* 4   4/2/08 10:14a erickson
* PR41212: call pthread_attr_getschedparam to init schedParam
*
* 3   3/25/08 1:24p jgarrett
* PR 40849: Fixing priority of ISR task
*
* 2   2/19/08 5:28p erickson
* PR36068: do safe kernel memory probe
*
* 1   1/18/08 2:17p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/9   11/30/07 11:56a jgarrett
* PR 37632: Changing header file name
*
* Nexus_Devel/8   11/27/07 1:15p jgarrett
* PR 37632: Adding support for L1 interrupts above 64
*
* Nexus_Devel/7   9/28/07 4:24p jgarrett
* PR 35002: Removing debug
*
* Nexus_Devel/6   9/27/07 10:11p jgarrett
* PR 35002: Adding OS interrupt reset
*
* Nexus_Devel/5   9/25/07 1:26p jgarrett
* PR 35002: Removing warnings
*
* Nexus_Devel/4   9/25/07 10:48a jgarrett
* PR 35002: Fixing circular call
*
* Nexus_Devel/3   9/24/07 1:00p jgarrett
* PR 35002: Changing structure of main memory allocation
*
* Nexus_Devel/2   9/21/07 6:26p jgarrett
* PR 35002: Successful 97400 build
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "nexus_core_utils.h"
#include "bkni.h"
#include "bcm_driver.h"
#include "bdbg_log.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <signal.h>
#include "nexus_platform_debug_log.h"
#include "nexus_base_statistics.h"

BDBG_MODULE(nexus_platform_os);
BDBG_FILE_MODULE(nexus_statistics_isr);

#define BDBG_MSG_TRACE(X) /* BDBG_WRN(X) */
#if defined(HUMAX_PLATFORM_BASE)
#define NEXUS_RUNAWAY_L1_THRESHOLD 20000
#else
#define NEXUS_RUNAWAY_L1_THRESHOLD 10000
#endif

int g_NEXUS_driverFd = -1;

typedef struct IsrTableEntry
{
    const char *name;
    NEXUS_Core_InterruptFunction pFunction;
    void *pFuncParam;
    int iFuncParam;

    unsigned count;
    bool print;
} IsrTableEntry;

typedef struct NEXUS_Platform_Os_State {
    bool NEXUS_magnum_init;
    bool interruptDone;
    uint32_t mask[NEXUS_NUM_L1_REGISTERS];
#define NUM_IRQS (32*NEXUS_NUM_L1_REGISTERS)
    IsrTableEntry table[NUM_IRQS];
    NEXUS_TimerHandle debugTimer;
    pthread_t interruptThread;
    NEXUS_Platform_P_DebugLog debugLog;
    int memFd;
    int memFdCached;
} NEXUS_Platform_Os_State;

static NEXUS_Platform_Os_State g_NEXUS_Platform_Os_State;

static NEXUS_Error NEXUS_Platform_P_ConnectInterrupt_isr(
    unsigned irqNum,
    NEXUS_Core_InterruptFunction pIsrFunc,
    void *pFuncParam,
    int iFuncParam
    );

static void NEXUS_Platform_P_DisconnectInterrupt_isr(
    unsigned irqNum
    );

NEXUS_P_STACKDEPTH_AGGREGATE();

BTRC_MODULE(L1_ISR, ENABLE);
static void *NEXUS_Platform_P_IsrTask(void *pParam)
{
    tbcm_linux_dd_interrupt isrData;
    uint32_t status[NEXUS_NUM_L1_REGISTERS];
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;
    NEXUS_P_STACKDEPTH_STATE();
    BSTD_UNUSED(pParam);

    prctl(PR_SET_NAME,"nx_platform_isr",0,0,0);
    NEXUS_Profile_MarkThread("isr");

    BKNI_Memset(&isrData, 0, sizeof(isrData));
    isrData.timeout = 10000;

    while(!state->interruptDone) {
        unsigned i;
        uint32_t mask[NEXUS_NUM_L1_REGISTERS];
        uint32_t test_one = 0;

        for (i=0;i<NEXUS_NUM_L1_REGISTERS;i++) {
            mask[i] = state->mask[i];
            test_one |= mask[i];
        }

        if (!test_one)
        {
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 1000*1000*1;
            nanosleep(&ts, NULL);
            continue;
        }
        
        for (i=0;i<NEXUS_NUM_L1_REGISTERS;i++) {
            isrData.interruptmask[i] = mask[i];
            isrData.interruptstatus[i] = 0;
        }

        ioctl(g_NEXUS_driverFd, BRCM_IOCTL_WAIT_FOR_INTERRUPTS, &isrData);
        /* If there's a timeout, then the system is not getting
        any interrupts. This isn't common, but isn't necessarily bad.
        Adding debug output here can cause a problem during system takedown. */

        /* If there is or isn't an error (which is most likely a timeout), the
        W0/W1 status returned is still good. */
        test_one = 0;
        for (i=0;i<NEXUS_NUM_L1_REGISTERS;i++) {
            status[i] = isrData.interruptstatus[i];
            test_one |= status[i];
            BDBG_MSG(("isr status[i]=0x%08x", i, status[i]));
        }

        /* Check for any pending interrupts */
        if( test_one )
        {
            NEXUS_P_STACKDEPTH_START();
            BKNI_EnterCriticalSection();
            BTRC_TRACE(L1_ISR, START);

            /* Reload mask to avoid stray interrupts */
            for (i=0;i<NEXUS_NUM_L1_REGISTERS;i++) {
                unsigned bit;
                mask[i] = state->mask[i];

                /* Scan all bits of each register */
                for ( bit = 0; bit < 32; bit++ )
                {
                    uint32_t statusMask = (1<<bit);
                    if ( status[i] & statusMask & mask[i] )
                    {
                        unsigned irq = i*32+bit;
                        IsrTableEntry *pEntry;

                        /* print on runaway L1 */
                        if (++state->table[irq].count % NEXUS_RUNAWAY_L1_THRESHOLD == 0) {
                            if (!state->table[irq].print) {
                                BDBG_WRN(("### %s (W%d, bit %d) fired %d times", state->table[irq].name, i, bit, state->table[irq].count));
                                state->table[irq].print = true; /* only print once to maximize chance that system keeps running */
                            }
                        }

                        pEntry = &state->table[irq];
                        if ( pEntry->pFunction )
                        {
                            pEntry->pFunction(pEntry->pFuncParam, pEntry->iFuncParam);
                        }
                        else
                        {
                            BDBG_ERR(("Stray Interrupt %d", irq));
                        }
                    }
                }
            }
            BTRC_TRACE(L1_ISR, STOP);
            BKNI_LeaveCriticalSection();
            NEXUS_P_STACKDEPTH_STOP();
        }
    }
    NEXUS_BTRC_REPORT(L1_ISR);
    return NULL;
}

void NEXUS_Platform_P_MonitorOS(void)
{
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;
    unsigned irq;
    for (irq=0;irq<NUM_IRQS;irq++) {
        if (state->table[irq].print) {
            unsigned i = irq / 32;
            unsigned bit = irq % 32;
            BDBG_WRN(("%s (W%d, bit %d) fired %d times", state->table[irq].name, i, bit, state->table[irq].count));
        }
    }
    BKNI_EnterCriticalSection();
    for (irq=0;irq<NUM_IRQS;irq++) {
        state->table[irq].count = 0;
        state->table[irq].print = false;
    }
    BKNI_LeaveCriticalSection();
}

static void NEXUS_Platform_P_DebugCallback(void *context, NEXUS_ModuleHandle handle, const char *name, const NEXUS_ModuleSettings *settings)
{
    if ( !strcmp( (const char *)context , "ls") && settings->dbgPrint ) {
        /* Print out nexus modules that have a "Print" module defined */
        BDBG_LOG(("%s", name ));
    }
    else {
        if (settings->dbgModules && settings->dbgPrint){
            if(!strcmp(context, settings->dbgModules) || !strcmp(context, name) || !strcmp("all", context)){
                NEXUS_Module_Lock(handle);
                settings->dbgPrint();
                NEXUS_Module_Unlock(handle);
            }
        }
    }
}

char findDelimiter( char *dbgInfo ) {

    const char delimiters[] = " .,;:";

    int dbgInfoLen = b_strlen( dbgInfo );
    int nrDelimiters=b_strlen( delimiters );
    int i,j;
    char rc = 0;

    for ( i=0; i < dbgInfoLen ; i++ ) {
        for ( j=0; j < nrDelimiters ; j++ ) {
            if ( delimiters[j] == *(dbgInfo+i) )  {
                break;
            }
        }
        if ( j < nrDelimiters ) {
            rc = (int)delimiters[j];
            break;
        }
    }
    return rc;
}

static void NEXUS_Platform_P_DebugTimer(void *context)
{
#define DEBUG_INFO_LEN 256
    char debug_info[DEBUG_INFO_LEN];
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;
    int rc;
    BSTD_UNUSED(context);

    state->debugTimer = NULL;

    rc = ioctl(g_NEXUS_driverFd, BRCM_IOCTL_GET_DEBUG, debug_info);

    if(rc==0 && debug_info[0]){
        char delim = findDelimiter( debug_info );
        const char *debug_list = debug_info;
        for(;;) {
            char buf[DEBUG_INFO_LEN];
            char *end = NULL;
            size_t name_len;

            if ( delim ) {
                end = strchr(debug_list, delim );
            }

            if (!end) {
                if(*debug_list) {
                    NEXUS_Module_EnumerateAll(NEXUS_Platform_P_DebugCallback, (char *)debug_list);
                }
                break;
            }
            name_len = end-debug_list;
            if(name_len>0 && name_len<sizeof(buf)) {
                strncpy(buf, debug_list, name_len);
                buf[name_len] = '\0';
                NEXUS_Module_EnumerateAll(NEXUS_Platform_P_DebugCallback, buf);
            }
            debug_list = end+1;
        }
    }
    state->debugTimer = NEXUS_ScheduleTimer(500, NEXUS_Platform_P_DebugTimer, NULL);
#if NEXUS_P_STACKDEPTH_STATS
    {
        static int timer_tick = 0;
        if(timer_tick++>10) {
            unsigned count, max, average;
            timer_tick = 0;
            BKNI_EnterCriticalSection();
            count = g_stackAggregate.count;
            max = g_stackAggregate.max;
            average = count ? g_stackAggregate.total/count : 0;
            g_stackAggregate.count = 0;
            g_stackAggregate.max = 0;
            g_stackAggregate.total = 0;
            BKNI_LeaveCriticalSection();
            BDBG_MODULE_MSG(nexus_statistics_isr,("isr stack usage max:%u average:%u count:%u", max, average, count));
        }
    }
#endif /* NEXUS_P_STACKDEPTH_STATS */
    return;
}

NEXUS_Error NEXUS_Platform_P_InitOSMem(void)
{
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;

    /* Open /dev/mem for memory mapping */
    state->memFd = open("/dev/mem", O_RDWR|O_SYNC);
    if ( state->memFd < 0 )
    {
        BDBG_ERR(("Unable to open /dev/mem for uncached memory: %d", errno));
        return BERR_TRACE(BERR_OS_ERROR);
    }
    fcntl(state->memFd, FD_CLOEXEC, 1);
    state->memFdCached = open("/dev/mem", O_RDWR);
    if ( state->memFdCached < 0 )
    {
        BDBG_ERR(("Unable to open /dev/mem for cached memory: %d", errno));
        close(state->memFd);
        return BERR_TRACE(BERR_OS_ERROR);
    }
    fcntl(state->memFdCached, FD_CLOEXEC, 1);

    return 0;
}

void NEXUS_Platform_P_UninitOSMem(void)
{
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;

    close(state->memFdCached);
    close(state->memFd);
    return;
}

NEXUS_Error NEXUS_Platform_P_InitOS(void)
{
    int rc;
    pthread_attr_t threadAttr;
    struct sched_param schedParam;
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;

    NEXUS_Platform_P_DebugLog_Init(&state->debugLog, NULL);

    /* Open user-mode driver */
    g_NEXUS_driverFd = open("/dev/brcm0", O_RDWR);
    if ( g_NEXUS_driverFd < 0 )
    {
        BDBG_ERR(("Unable to open user-mode driver"));
        /* give message which points to solution */
        switch (errno)
        {
        case ENXIO:
            BDBG_ERR(("bcmdriver has not been installed. Are you running the nexus script?"));
            break;
        case ENOENT:
            BDBG_ERR(("/dev/brcm0 does not exist. Are you running the nexus script?"));
            break;
        default:
            BDBG_ERR(("/dev/brcm0 error: %d", errno));
            break;
        }
        rc = BERR_TRACE(BERR_OS_ERROR);
        goto err_bcmdriver_open;
    }
    fcntl(g_NEXUS_driverFd, FD_CLOEXEC, 1);
    if ( ioctl(g_NEXUS_driverFd, BRCM_IOCTL_INT_RESET, 0) )
    {
        BDBG_ERR(("cannot reset interrupts. another instance of nexus is probably running."));
        rc = BERR_TRACE(BERR_OS_ERROR);
        goto err_bcmdriver_setup;
    }

    /* If MEM's alignment is not set to the MIPS L1 and (if present) L2 cache line size,
    we will have cache coherency problems (which lead to major system failures).
    This code verifies that Nexus's MEM configuration is compatible with the MIPS cache line size.
    If this code fails, please check to make sure the Linux kernel is configured right, then modify nexus_core_features.h to match.
    use g_platformMemory to pass OS value to NEXUS_Platform_P_SetCoreModuleSettings */
    rc = ioctl(g_NEXUS_driverFd, BRCM_IOCTL_GET_DCACHE_LINE_SIZE, &g_platformMemory.max_dcache_line_size);
    if (rc) {
        BDBG_ERR(("Nexus requires a new bcmdriver.ko ioctl API BRCM_IOCTL_GET_DCACHE_LINE_SIZE. Are you running with an old bcmdriver.ko?"));
        rc = BERR_TRACE(BERR_OS_ERROR);
        goto err_bcmdriver_setup;
    }

    rc = NEXUS_Platform_P_InitOSMem();
    if (rc) {
        rc = BERR_TRACE(rc);
        goto err_mem;
    }

    /* Launch interrupt thread */
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
    if (!NEXUS_GetEnv("not_realtime_isr")) {
        pthread_attr_setschedpolicy(&threadAttr, SCHED_FIFO);
        pthread_attr_getschedparam(&threadAttr, &schedParam);
        schedParam.sched_priority = sched_get_priority_max(SCHED_FIFO);
        pthread_attr_setschedparam(&threadAttr, &schedParam);
    }
    pthread_attr_setstacksize(&threadAttr, 8*1024);
    BDBG_MSG(("Launching ISR task"));
    rc = pthread_create(&state->interruptThread,
                        &threadAttr,
                        NEXUS_Platform_P_IsrTask,
                        NULL);
    if ( rc )
    {
        BDBG_ERR(("Unable to create ISR task"));
        BERR_TRACE(BERR_OS_ERROR);
        goto err_thread;
    }
    if (!NEXUS_GetEnv("not_realtime_isr")) {
        pthread_setschedparam(state->interruptThread, SCHED_FIFO, &schedParam);
    }

    state->debugTimer = NEXUS_ScheduleTimer(500, NEXUS_Platform_P_DebugTimer, NULL);

    /* Success */
    return BERR_SUCCESS;

err_thread:
    NEXUS_Platform_P_UninitOSMem();
err_mem:
err_bcmdriver_setup:
    close(g_NEXUS_driverFd);
    g_NEXUS_driverFd = -1;
err_bcmdriver_open:

    /* Failed */
    return BERR_OS_ERROR;
}

NEXUS_Error NEXUS_Platform_P_UninitOS(void)
{
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;

    if(state->debugTimer) {
        NEXUS_CancelTimer(state->debugTimer);
        state->debugTimer = NULL;
    }

    state->interruptDone = true;
    /* BRCM_IOCTL_CHANGE_REQUEST is needed to force BRCM_IOCTL_WAIT_FOR_INTERRUPTS to return immediately */
    ioctl(g_NEXUS_driverFd, BRCM_IOCTL_CHANGE_REQUEST, NULL);
    pthread_join(state->interruptThread, NULL);

    NEXUS_Platform_P_UninitOSMem();
    close(g_NEXUS_driverFd);
    NEXUS_Platform_P_DebugLog_Uninit(&state->debugLog);
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Platform_P_GetHostMemory(NEXUS_PlatformMemory *pMemory)
{
    unsigned total = 0;

#if NEXUS_DTV_PLATFORM
    /* use hardcoded values because DTV linux bmem does not return nexus-usable space. */
    pMemory->osRegion[0].base = 0x20000000;
    pMemory->osRegion[0].length = 256 * 1024 * 1024;
    total = pMemory->osRegion[0].length;
#else
    /* try 2.6.31 filesystem first. we cannot do a compile-time test for linux kernel version here
    because it's linux user mode. */
    {
        unsigned i;
        BDBG_CASSERT(NEXUS_MAX_HEAPS);
        for (i=0;i<NEXUS_MAX_HEAPS;i++) {
            char buf[256];
            const char *bmem_dir = NEXUS_GetEnv("bmem_override");
            FILE *f;
            unsigned n, base, length;

            if (!bmem_dir) {
                /* normal sys fs directory */
                bmem_dir = "/sys/devices/platform/brcmstb";
            }

            BKNI_Snprintf(buf, 256, "%s/bmem.%d", bmem_dir, i);
            f = fopen(buf, "r");
            if (!f) break;
            if (!fgets(buf, 256, f)) {
                n = 0;
            }
            else {
                buf[255] = 0; /* ensure no memory overrun */
                n = sscanf(buf, "0x%x 0x%x", &base, &length);
            }
            fclose(f);
            /* even on MEMC0, base should never be 0 because it is used by the OS */
            if (n == 2 && base && length) {
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_RAM_ROOTFS)
		if (i == CONFIG_RAM_ROOTFS_BMEM_INDEX)
		{
			/* bmem.x */
#if (CONFIG_RAM_LXC_BMEM_SIZE)
			unsigned long reservedSize = ((CONFIG_RAM_ROOTFS_BMEM_SIZE + CONFIG_RAM_LXC_BMEM_SIZE )* 1024 * 1024);
#else
			unsigned long reservedSize = (CONFIG_RAM_ROOTFS_BMEM_SIZE * 1024 * 1024);
#endif
			/* we got it */
			pMemory->osRegion[total].base = base + reservedSize;
			pMemory->osRegion[total].length = length - reservedSize;
			total++;
			}
		else
		{
                /* we got it */
                pMemory->osRegion[total].base = base;
                pMemory->osRegion[total].length = length;
                total++;
            }
#else
                /* we got it */
                pMemory->osRegion[total].base = base;
                pMemory->osRegion[total].length = length;
                total++;
#endif
            }
            else {
                break;
            }
        }
    }

    /* if there are no hits, try 2.6.18 file system */
    if (!total) {
        const char bmeminfo2618[] =  "/proc/bcmdriver/meminfo";
        FILE *pFile;
        pFile = fopen(bmeminfo2618, "r");
        if (pFile) {
            char buf[512];
            char *pStart, *pEnd;

            if ( fgets(buf, sizeof(buf), pFile) )
            {
                pStart = &buf[13];      /* "13" should point us to the "space" after "Kernel Memory " see umdrv.c */
                pStart += strspn(pStart, " ");
                pEnd = strchr(pStart, ' ');
                if ( pEnd )
                {
                    *pEnd = 0;
                    pMemory->osRegion[total].base = atoi(pStart);
                    pMemory->osRegion[total].length = pMemory->memc[0].region[0].length - pMemory->osRegion[total].base;
                    total++;
                }
            }
            fclose(pFile);
        }
    }
#endif

    if (!total) {
        BDBG_ERR(("unable to learn OS memory"));
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    return 0;
}

/***************************************************************************
Summary:
    Hook to connect a L1 interrupt to the OS
See Also:
    NEXUS_Platform_P_DisconnectInterrupt
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_ConnectInterrupt(
    unsigned irqNum,
    NEXUS_Core_InterruptFunction pIsrFunc,
    void *pFuncParam,
    int iFuncParam
    )
{
    NEXUS_Error errCode;

    BKNI_EnterCriticalSection();
    errCode = NEXUS_Platform_P_ConnectInterrupt_isr(irqNum, pIsrFunc, pFuncParam, iFuncParam);
    BKNI_LeaveCriticalSection();

    return errCode;
}

static NEXUS_Error NEXUS_Platform_P_ConnectInterrupt_isr(
    unsigned irqNum,
    NEXUS_Core_InterruptFunction pIsrFunc,
    void *pFuncParam,
    int iFuncParam
    )
{
    IsrTableEntry *pEntry;
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;

    /* Connect does not automatically enable the interrupt */
    if (irqNum >= NUM_IRQS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    pEntry = &state->table[irqNum];
    pEntry->pFunction = pIsrFunc;
    pEntry->pFuncParam = pFuncParam;
    pEntry->iFuncParam = iFuncParam;

    {    
        const BINT_P_IntMap *intMap;
        unsigned i;
        /* use BINT's record of managed L2's (and their corresponding L1's) to validate the L1 connect */
        intMap = g_pCoreHandles->bint_map;
        BDBG_ASSERT(intMap);
        
        /* find the first L2 that has this L1 */
        for (i=0;intMap[i].L1Shift!=-1;i++) {
            if (intMap[i].L1Shift == (int)irqNum) {
                pEntry->name = intMap[i].L2Name; /* use BINT's L2 name for the L1 name. in most cases it is a meaningful name. */
                break;
            }
        }
        if (intMap[i].L1Shift == -1) {
            pEntry->name = "unknown";
        }
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Hook to disconnect a L1 interrupt from the OS
See Also:
    NEXUS_Platform_P_ConnectInterrupt
 ***************************************************************************/
void NEXUS_Platform_P_DisconnectInterrupt(
    unsigned irqNum
    )
{
    BKNI_EnterCriticalSection();
    NEXUS_Platform_P_DisconnectInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();
}

static void NEXUS_Platform_P_DisconnectInterrupt_isr(
    unsigned irqNum
    )
{
    NEXUS_Platform_P_DisableInterrupt_isr(irqNum);
    (void)NEXUS_Platform_P_ConnectInterrupt_isr(irqNum, NULL, NULL, 0);
}

/***************************************************************************
Summary:
    Hook to enable an L1 interrupt
See Also:
    NEXUS_Platform_P_DisableInterrupt_isr
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_EnableInterrupt(
    unsigned irqNum
    )
{
    NEXUS_Error errCode;

    BKNI_EnterCriticalSection();
    errCode = NEXUS_Platform_P_EnableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();

    return errCode;
}

/***************************************************************************
Summary:
    Hook to enable an L1 interrupt
See Also:
    NEXUS_Platform_P_DisableInterrupt_isr
 ***************************************************************************/
NEXUS_Error NEXUS_Platform_P_EnableInterrupt_isr(
    unsigned irqNum
    )
{
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;

    if (irqNum >= NUM_IRQS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    state->mask[irqNum/32] |= 1<<(irqNum%32);

    (void)ioctl(g_NEXUS_driverFd, BRCM_IOCTL_CHANGE_REQUEST, 0);

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Hook to disable an L1 interrupt
See Also:
    NEXUS_Platform_P_EnableInterrupt_isr
 ***************************************************************************/
void NEXUS_Platform_P_DisableInterrupt(
    unsigned irqNum
    )
{
    BKNI_EnterCriticalSection();
    NEXUS_Platform_P_DisableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();
}

/***************************************************************************
Summary:
    Hook to disable an L1 interrupt
See Also:
    NEXUS_Platform_P_EnableInterrupt_isr
 ***************************************************************************/
void NEXUS_Platform_P_DisableInterrupt_isr(
    unsigned irqNum
    )
{
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;

    if (irqNum >= NUM_IRQS) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    state->mask[irqNum/32] &= ~(1<<(irqNum%32));

    (void)ioctl(g_NEXUS_driverFd, BRCM_IOCTL_CHANGE_REQUEST, 0);
}

/***************************************************************************
Summary:
    Map physical memory into virtual space
Returns:
    Valid address on success, NULL for failure.
See Also:
    NEXUS_Platform_P_UnmapMemory
 ***************************************************************************/
void *NEXUS_Platform_P_MapMemory(
    unsigned long offset,
    unsigned long length,
    bool cached)
{
    void *pMem;
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;
    int memFd = (cached) ? state->memFdCached : state->memFd;

    BDBG_ASSERT(length > 0);

    pMem = mmap(0, length, PROT_READ|PROT_WRITE, MAP_SHARED, memFd, offset);
    BDBG_MSG(("mmap offset=%x=>%x size=%d fd=%d", offset, pMem, length, memFd));

    if ( MAP_FAILED == pMem )
    {
        BDBG_ERR(("mmap failed: offset=%x size=%d (%s)", offset, length, cached?"cached":"uncached"));
        BERR_TRACE(BERR_OS_ERROR);
        pMem = NULL;
    }

    return pMem;
}

/***************************************************************************
Summary:
    Unmap a virtual address
See Also:
    NEXUS_Platform_P_MapMemory
 ***************************************************************************/
void NEXUS_Platform_P_UnmapMemory(
    void *pMem,
    unsigned long length
    )
{
    BDBG_ASSERT(NULL != pMem);
    (void)munmap(pMem, length);
}

/***************************************************************************
Summary:
    Reset any pending L1 interrupts
 ***************************************************************************/
void NEXUS_Platform_P_ResetInterrupts(void)
{
    if ( ioctl(g_NEXUS_driverFd, BRCM_IOCTL_INT_RESET, 0) )
    {
        BERR_TRACE(BERR_OS_ERROR);
    }
}

/* This function is dangerous and not meant for general use. It is needed for
test code that needs a special ioctl to the usermode driver. It must be extern'd. */
int b_get_umdrv_fd(void)
{
    return g_NEXUS_driverFd;
}

extern void *g_pRegAddress;

void NEXUS_Platform_P_AtomicUpdateCallback_isr(void *callbackContext, uint32_t reg, uint32_t mask, uint32_t value)
{
#if 1
    /* serialize magnum and the OS using an OS-specific lock */
    t_bcm_linux_dd_atomic_update data;
    int rc;

    BSTD_UNUSED(callbackContext);

    data.reg = reg;
    data.mask = mask;
    data.value = value;

    BDBG_MSG_TRACE(("atomic update: reg=%08x %08x %08x", reg, mask, value));

    rc = ioctl(g_NEXUS_driverFd, BRCM_IOCTL_ATOMIC_UPDATE, &data);
    if (rc) {rc = BERR_TRACE(rc);} /* REG basemodule can't do anything with the error, so fallthrough */
#else
    /* serialize within magnum only using isr context */
#define REGOFFSET_TO_ADDR(offset) ((uint8_t*)g_pRegAddress + offset)
    uint32_t val;
    /* read/modify/write */
    val = *(volatile uint32_t*)REGOFFSET_TO_ADDR(reg);
    val &= ~mask;
    val |= value;
    *(volatile uint32_t*)REGOFFSET_TO_ADDR(reg) = val;
#endif
}

NEXUS_Error
NEXUS_Platform_P_Magnum_Init(void)
{
    BERR_Code rc;
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;

    if(!state->NEXUS_magnum_init) {
        BKNI_Memset(state, 0, sizeof(*state));
        rc = BKNI_Init();
        if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}
        rc = BDBG_Init();
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); BKNI_Uninit();return rc;}
        rc = NEXUS_Base_Core_Init();
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc); BDBG_Uninit();BKNI_Uninit();return rc;}
        state->NEXUS_magnum_init = true;
    }
    return BERR_SUCCESS;
}

void
NEXUS_Platform_P_Magnum_Uninit(void)
{
    NEXUS_Platform_Os_State *state = &g_NEXUS_Platform_Os_State;

    if(state->NEXUS_magnum_init) {
        NEXUS_Base_Core_Uninit();
        BDBG_Uninit();
        BKNI_Uninit();
        BKNI_Memset(state, 0, sizeof(*state));
    }
    return;
}

/***************************************************************************
Summary:
Read reserved memory
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadReserved(
    uint32_t physicalAddress
    )
{
    t_bcm_linux_rw_reserved res;

    res.address = physicalAddress;
    res.value = 0;
    ioctl(g_NEXUS_driverFd, BRCM_IOCTL_READ_RESERVED, &res);
    return res.value;
}

/***************************************************************************
Summary:
Write reserved memory
***************************************************************************/
void NEXUS_Platform_P_WriteReserved(
    uint32_t physicalAddress,
    uint32_t value
    )
{
    t_bcm_linux_rw_reserved res;

    res.address = physicalAddress;
    res.value = value;
    ioctl(g_NEXUS_driverFd, BRCM_IOCTL_WRITE_RESERVED, &res);
}

/***************************************************************************
Summary:
Read core register
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadCoreReg(
    uint32_t offset
    )
{
    t_bcm_linux_rw_core_register reg;

    reg.offset = offset;
    reg.value = 0;
    ioctl(g_NEXUS_driverFd, BRCM_IOCTL_READ_CORE_REGISTER, &reg);
    return reg.value;
}

/***************************************************************************
Summary:
Write core register
***************************************************************************/
void NEXUS_Platform_P_WriteCoreReg(
    uint32_t offset,
    uint32_t value
    )
{
    t_bcm_linux_rw_core_register reg;

    reg.offset = offset;
    reg.value = value;
    ioctl(g_NEXUS_driverFd, BRCM_IOCTL_WRITE_CORE_REGISTER, &reg);
}

/***************************************************************************
Summary:
Read CMT Control Register
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadCmtControl(void)
{
    uint32_t value;
    ioctl(g_NEXUS_driverFd, BRCM_IOCTL_READ_CMT_CONTROL, &value);
    return value;
}

/***************************************************************************
Summary:
Write CMT Control Register
***************************************************************************/
void NEXUS_Platform_P_WriteCmtControl(
    uint32_t value
    )
{
    ioctl(g_NEXUS_driverFd, BRCM_IOCTL_WRITE_CMT_CONTROL, value);
}

#if B_HAS_BPROFILE || B_HAS_TRC
#include "bperf_counter.h"
#ifdef b_perf_write_cfg
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "bcm_driver.h"
void b_perf_write_cfg(unsigned select, unsigned data)
{
    int driver_fd = g_NEXUS_driverFd;
    int rc;
    t_bcm_linux_write_cp0_25 arg;

    arg.select = select;
    arg.value = data;
    rc = ioctl(driver_fd, BRCM_IOCTL_WRITE_CP0_25, &arg);
    if(rc<0) {(void)BERR_TRACE(BERR_OS_ERROR);goto err_ioctl;}
err_ioctl:
    return;
}
#endif /*b_perf_write_cfg*/
#endif /* B_HAS_BPROFILE || B_HAS_TRC */

void NEXUS_Platform_P_StopCallbacks(void *interfaceHandle)
{
    NEXUS_Base_P_StopCallbacks(interfaceHandle);
    return;
}

void NEXUS_Platform_P_StartCallbacks(void *interfaceHandle)
{
    NEXUS_Base_P_StartCallbacks(interfaceHandle);
    return;
}
