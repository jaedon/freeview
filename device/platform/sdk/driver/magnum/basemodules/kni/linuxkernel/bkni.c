/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni.c $
 * $brcm_Revision: Hydra_Software_Devel/77 $
 * $brcm_Date: 8/20/12 12:02p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kni/linuxkernel/bkni.c $
 * 
 * Hydra_Software_Devel/77   8/20/12 12:02p erickson
 * SW7425-3260: add tagged version of AcquireMutex/TryAcquireMutex for
 * deadlock debug
 * 
 * Hydra_Software_Devel/76   8/14/12 3:25p vsilyaev
 * SW7420-1817: Added option to enable mutex tracking/deadlock detection
 * 
 * Hydra_Software_Devel/75   4/23/12 4:16p erickson
 * SW7346-774: prevent bad call to BKNI_ReleaseMutex from corrupting state
 * machine
 * 
 * Hydra_Software_Devel/74   2/21/12 8:44a erickson
 * SW7346-703: add linux 3.3 support
 * 
 * Hydra_Software_Devel/73   8/3/11 5:33p vsilyaev
 * SW7420-1465, SW7405-5221: Route debug output to the special FIFO
 * instead of syslog buffer
 * 
 * Hydra_Software_Devel/72   5/3/11 1:41p erickson
 * SW7420-1819: remove copy to/from user from linuxkernel KNI
 * 
 * Hydra_Software_Devel/71   4/22/11 1:59p erickson
 * SW3548-2837: fix BKNI_DEBUG_CS_TIMING test
 * 
 * Hydra_Software_Devel/70   4/19/11 10:53p nickh
 * SW7425-365: Add 2.6.37 kernel support
 * 
 * Hydra_Software_Devel/69   10/11/10 5:23p jgarrett
 * SW7125-630: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7125-630/2   10/11/10 1:54p mward
 * SW7125-630:  Merge from main.
 * 
 * Hydra_Software_Devel/68   9/22/10 4:36p gskerl
 * SW7125-636: Merged back to /main/Hydra_Software_Devel branch
 * 
 * Hydra_Software_Devel/SW7125-636/2   9/22/10 4:08p gskerl
 * SW7125-636: Added freeze handling (for suspend/hibernate) to
 * BKNI_P_signalPending(). Refactored BKNI_WaitForGroup() to handle
 * spurious wakeups without returning.
 * 
 * Hydra_Software_Devel/67   8/20/10 2:59p mward
 * SW7420-943: Need <asm/uaccess.h> for 2.6.18.
 * 
 * Hydra_Software_Devel/66   8/12/10 4:35p ttrammel
 * SW7420-943: Merge NFE to main branch.
 * 
 * Hydra_Software_Devel/SW7125-630/1   10/1/10 5:48p jgarrett
 * SW7125-630: Switching critical section to use semaphore instead of
 * spinlock if tasklets are being utilized
 * 
 * Hydra_Software_Devel/65   8/5/10 7:13p mward
 * SWLINUX-1567: spin_lock's flags unsigned long, not unsigned int,
 * eliminate compiler warning.
 * 
 * Hydra_Software_Devel/64   7/2/10 5:43p vsilyaev
 * SW7405-4601: BKNI_WaitForGroup should not report timeout if there are
 * some events are set
 * 
 * Hydra_Software_Devel/63   7/2/10 4:45p vsilyaev
 * SW7405-4601: Use down (not down_interruptible) in BKNI_AcquireMutex
 * 
 * Hydra_Software_Devel/61   7/2/10 2:00p vsilyaev
 * SW7405-4601: Protect access to the EventGroup when walking through
 * group list
 * 
 * Hydra_Software_Devel/60   7/2/10 12:59p vsilyaev
 * SW7405-4601: Use atomic operations to access primitive mutable state
 * 
 * Hydra_Software_Devel/59   7/2/10 11:13a vsilyaev
 * SW7400-290: PR23016 use simple down, instead of down_interruptible ->
 * down for 2.4 kernel
 * 
 * Hydra_Software_Devel/58   6/30/10 6:05p vsilyaev
 * SW7405-2178: Use semaphore(mutex) to serialize malloc tracking code
 * 
 * Hydra_Software_Devel/57   6/30/10 5:44p vsilyaev
 * SW7405-4574: Block signals while inside BKNI_WaitForGroup
 * 
 * Hydra_Software_Devel/56   6/30/10 12:07p vsilyaev
 * SWLINUX-1567: Use spin_lock_irqsave/spin_lock_irqrestore for siglock to
 * seriliaze access to the process signal state
 * 
 * Hydra_Software_Devel/55   5/28/10 2:55p vsilyaev
 * SW7420-789: Short-circuit BKNI_ResetEvent instead of calling
 * BKNI_WaitForEvent
 * 
 * Hydra_Software_Devel/54   3/10/10 2:59p mward
 * SW7400-2712:  Add annotation to indicate to Coverity that BKNI_Fail()
 * is a "kill path".
 * 
 * Hydra_Software_Devel/53   9/11/09 12:23p erickson
 * SW7420-228: add 2.6.30 support
 *
 * Hydra_Software_Devel/52   4/30/09 3:45p erickson
 * PR53778: added #undef instead of #if. allows interoperability between
 * debug and non debug code.
 *
 * Hydra_Software_Devel/51   4/20/09 12:40p jtna
 * PR53778: use bkni_track_mallocs.inc
 *
 * Hydra_Software_Devel/50   4/16/09 3:26p jtna
 * PR51960: Keep BKNI_Malloc in sync with linuxuser
 *
 * Hydra_Software_Devel/49   4/16/09 11:54a jtna
 * PR51960: BKNI_GARBLE_MALLOC now enabled by default
 *
 * Hydra_Software_Devel/48   4/13/09 3:50p jtna
 * PR53480: merge BKNI_TRACK_MALLOCS logic for linuxkernel
 *
 * Hydra_Software_Devel/47   4/8/09 12:35p erickson
 * PR53862: use BDBG_OBJECT to protect mutexes and events
 *
 * Hydra_Software_Devel/46   4/2/09 11:29a erickson
 * PR53778: extend BKNI_TRACK_MALLOCS to events and mutexes
 *
 * Hydra_Software_Devel/45   1/30/09 9:17p vsilyaev
 * PR 42495: Using matching types for the line number
 *
 * Hydra_Software_Devel/44   1/29/09 8:12p vsilyaev
 * PR 42495: Improved tracking of memory allocation
 *
 * Hydra_Software_Devel/43   12/29/08 10:40a erickson
 * PR50742: fix spelling of BKNI_AssertIsrContext
 *
 * Hydra_Software_Devel/42   9/24/08 2:17p ahulse
 * PR45850: Use linux/module.h for 2.6 and above kernels
 *
 * Hydra_Software_Devel/41   8/21/08 4:43p vsilyaev
 * PR 32280: Added BKNI_ASSERT_ISR_CONTEXT
 *
 * Hydra_Software_Devel/40   6/19/07 11:40a jgarrett
 * PR 31935: Updating for 2.6.18
 *
 * Hydra_Software_Devel/39   5/2/07 2:16p jgarrett
 * PR 25902: Converting signal logic to block unwanted signals rather than
 * discarding them.
 *
 * Hydra_Software_Devel/38   4/2/07 9:52a erickson
 * PR25902: convert include bstd.h to individual includes to avoid bchp.h
 * and bdbg.h. they aren't needed.
 *
 * Hydra_Software_Devel/37   2/26/07 4:26p erickson
 * PR27885: fixed warning
 *
 * Hydra_Software_Devel/36   1/31/07 3:41p jgarrett
 * PR 27468: Resolving race condition when event is set nearly coincident
 * with the timeout
 *
 * Hydra_Software_Devel/35   1/16/07 11:56a erickson
 * PR25902: readded dequeue_signal_lock, fixed printk call
 *
 * Hydra_Software_Devel/34   1/10/07 4:01p erickson
 * PR25902: remove dequeue_signal_lock again because of checkin problem
 *
 * Hydra_Software_Devel/33   1/10/07 3:44p erickson
 * PR25902: move linux kernel header files into .c
 *
 * Hydra_Software_Devel/32   12/5/06 7:01p jgarrett
 * PR 24364: Modifying ASSERT_NOT_CRITICAL/ASSERT_CRITICAL based upon
 * BKNI_MASK_INTERRUPTS
 *
 * Hydra_Software_Devel/31   11/17/06 1:40p jgarrett
 * PR 26021: Removing NESTED_CS_ENABLED option, it is ill-advised.
 *
 * Hydra_Software_Devel/30   11/8/06 3:58p jgarrett
 * PR 21708: Adding support for nested critical sections via
 * BKNI_NESTED_CS_ENABLED
 *
 * Hydra_Software_Devel/29   10/18/06 2:31p jgarrett
 * PR 21708: Removing tasklet disable option for SMP and CONFIG_PREEMPT
 *
 * Hydra_Software_Devel/28   10/11/06 3:01p jgarrett
 * PR 24364: Merging to main branch
 *
 * Hydra_Software_Devel/PR24364/3   10/10/06 4:27p jgarrett
 * PR 24364: Adding BKNI_LinuxKernel_SetIsrTasklet()
 *
 * Hydra_Software_Devel/PR24364/2   10/6/06 10:48a jgarrett
 * PR 24364: Adding missing include for 2.6
 *
 * Hydra_Software_Devel/PR24364/1   10/5/06 3:08p jgarrett
 * PR 24364: Revamping kni critical sections to use spin_lock_bh by
 * default and optionally tasklet_disable or spin_lock_irqsave.
 *
 * Hydra_Software_Devel/25   7/12/06 3:24p vsilyaev
 * PR 22357: Fixed signal interraction in BKNI_Sleep
 *
 * Hydra_Software_Devel/24   3/15/06 2:27p vsilyaev
 * PR20221: fix for the 2.6 kernel and signal handling
 *
 * Hydra_Software_Devel/23   3/14/06 7:58p dlwin
 * PR 9014: Problem with BKNI wait for event under kernel mode operation,
 * updated for a bug.
 *
 * Hydra_Software_Devel/22   1/11/06 1:12p erickson
 * PR19014: added loop in BKNI_WaitForEvent so that non-terminating
 * signals don't cause false events
 *
 * Hydra_Software_Devel/21   12/9/05 3:25p vsilyaev
 * PR 18019: Added support for 2.6 kernel
 *
 * Hydra_Software_Devel/20   2/24/05 4:38p vsilyaev
 * PR 14237: Add code to cause oops in the linux kernel.
 *
 * Hydra_Software_Devel/19   1/29/05 1:23p dlwin
 * PR 13970: Changed OS signal check for compatible with Docsis code.
 * Previous code would terminate with error from WaitForEvent when on any
 * OS signal.  This does not work when other driver/app uses signals. New
 * code only terminates with error for a given set of signals, not all
 * signals.
 *
 * Hydra_Software_Devel/18   10/28/04 5:11p erickson
 * PR13046: fixed race condition in BKNI_WaitForGroup by using
 * set_current_state(TASK_INTERRUPTIBLE)
 *
 * Hydra_Software_Devel/17   10/20/04 8:42a dlwin
 * PR 13046: Fixed problem with BKNI_WaitForGroup() to work as "level
 * event", not as a "trigger event".
 *
 * Hydra_Software_Devel/16   8/23/04 4:29p erickson
 * PR11135: added CHECK_SLEEP_IN_INTERRUPT for debug
 *
 * Hydra_Software_Devel/15   5/26/04 4:18p erickson
 * PR11135: added BKNI_EventGroup impl for kernel mode
 *
 * Hydra_Software_Devel/14   4/8/04 9:49p vsilyaev
 * PR 10508: Fixed compile  issue in the debug build
 *
 * Hydra_Software_Devel/13   4/8/03 5:15p erickson
 * implemented TryAcquireMutex for linux
 *
 * Hydra_Software_Devel/12   4/8/03 4:47p erickson
 * BKNI_ResetEvent now returns void
 * Split BKNI_AcquireMutex into Acquire and TryAcquire
 *
 * Hydra_Software_Devel/11   4/3/03 6:13p erickson
 * some pre 0.9 api rework
 *
 * Hydra_Software_Devel/10   4/3/03 3:57p erickson
 * linuxkernel work
 *
 * Hydra_Software_Devel/9   3/31/03 3:40p erickson
 * added BKNI_ResetEvent
 *
 * Hydra_Software_Devel/8   3/12/03 3:50p erickson
 * updated tests and got linuxkernel working with them
 *
 * Hydra_Software_Devel/7   3/12/03 12:04p erickson
 * updated return codes, comments
 *
 * Hydra_Software_Devel/6   3/11/03 6:59p erickson
 * changed kernelinterface from using object ptrs to handles
 *
 * Hydra_Software_Devel/5   3/10/03 4:55p erickson
 * using standard bkni.h
 * no longer includes STD_INC include path
 *
 * Hydra_Software_Devel/4   3/10/03 12:36p erickson
 * linuxkernel tasks initial implementationd
 *
 * Hydra_Software_Devel/3   3/7/03 5:20p erickson
 * linux kernel interface work
 *
 * Hydra_Software_Devel/2   3/6/03 6:27p erickson
 * rework KNI api
 *
 * Hydra_Software_Devel/1   3/5/03 5:16p erickson
 * Initial kernelinterface work
 *
 ***************************************************************************/
/**
The following is copied from bstd.h
Removed are: bchp.h, bdbg.h
**/
/* standard types */
#include "bstd_defs.h"

/* standard defines */
#define BSTD_ENDIAN_BIG 4321
#define BSTD_ENDIAN_LITTLE 1234

/* platform specific include file. This must
come before other basemodules which may depend upon #defines.
Setting compile options in bstd_cfg.h or on the command line
should be equivalent. */
#include "bstd_cfg.h"
#include "berr.h"

#include "bkni.h"
#include "bkni_multi.h"
#include "bkni_event_group.h"
#include "bdbg.h"

/* This is the only place in Magnum where linux kernel header files are allowed. */
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/kernel.h> /* printk */
#include <linux/slab.h> /* memset and friends */

#define BKNI_EVENTGROUP_SUPPORT

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
#define DECLARE_MUTEX DEFINE_SEMAPHORE
#define init_MUTEX(sem)     sema_init(sem, 1)
#define init_MUTEX_LOCKED(sem)  sema_init(sem, 0)
#endif

#ifdef BKNI_EVENTGROUP_SUPPORT
#include "blst_list.h"
#endif

#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/version.h>
#if defined(CONFIG_MODVERSIONS) && ! defined(MODVERSIONS)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <linux/module.h>
#else
#include <linux/modversions.h>
#endif
#endif
#include <linux/signal.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
#include <linux/semaphore.h>
#include <linux/freezer.h>
#else
#include <asm/semaphore.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <linux/hardirq.h>
#include <linux/interrupt.h>
#else
#include <linux/tqueue.h>
#include <asm/smplock.h>
#endif
#include <asm/atomic.h>
#include <asm/uaccess.h>

BDBG_MODULE(kernelinterface);



#ifdef BKNI_METRICS_ENABLED
/* METRICS are not SMP or CONFIG_PREEMPT safe */
#include "bkni_metrics.h"
static BKNI_Metrics g_metrics;
static BKNI_Metrics_LoggingState g_metricsLoggingState;
#endif

static bool g_init = false;
static int BKNI_P_signalPending(void);
static void BKNI_P_BlockSignals(sigset_t *pPrevMask);
static void BKNI_P_RestoreSignals(sigset_t *pPrevMask);
#ifdef BKNI_METRICS_ENABLED
static long g_mutexSectionStart, g_criticalSectionStart;
#endif

/***********************************************/
/* Options that may be enabled via gcc -DXXX=1 */
/***********************************************/
#if 0
#undef BKNI_MASK_INTERRUPTS
#undef BKNI_DEBUG_CS_TIMING
#endif

static struct task_struct *g_csOwner;

#define SET_CRITICAL() do { g_csOwner = current; } while (0)
#define CLEAR_CRITICAL() do { g_csOwner = NULL; } while (0)

#ifdef  BKNI_MASK_INTERRUPTS
#define CHECK_CRITICAL() ( g_csOwner == current || in_irq() )
#else
#define CHECK_CRITICAL() ( g_csOwner == current || in_interrupt() )
#endif

#if BDBG_DEBUG_BUILD

/* BDBG_P_AssertFailed("Error, must be in critical section to call " __FUNCTION__ "\n", __FILE__, __LINE__);\ */

#define ASSERT_CRITICAL() do \
{\
    if ( !CHECK_CRITICAL() )\
    {\
        BDBG_P_PrintString("Error, must be in critical section to call %s\n", __FUNCTION__);\
        BDBG_P_AssertFailed("Error, must be in critical section", __FILE__, __LINE__);\
    }\
} while (0)

#define ASSERT_NOT_CRITICAL() do \
{\
    if ( CHECK_CRITICAL() )\
    {\
        BDBG_P_PrintString("Error, must not be in critical section to call %s\n", __FUNCTION__);\
        BDBG_P_AssertFailed("Error, must not be in critical section", __FILE__, __LINE__);\
    }\
} while (0)

#else /* #if BDBG_DEBUG_BUILD */

#define ASSERT_CRITICAL() (void)0
#define ASSERT_NOT_CRITICAL() (void)0

#endif

/* #define BKNI_DEBUG_MUTEX_TRACKING   1 */



#if BKNI_DEBUG_MUTEX_TRACKING
static void BKNI_P_ForceReleaseMutex(void *);
#define BKNI_P_MUTEXTRACKING_HEARTBEAT_TYPE spinlock_t
#define BKNI_P_MUTEXTRACKING_HEARTBEAT_INIT(x) init_timer(&BKNI_P_MutexTracking_Timer)
#define BKNI_P_MUTEXTRACKING_HEARTBEAT_UNINIT(x) del_timer(&BKNI_P_MutexTracking_Timer)
#define BKNI_P_MUTEXTRACKING_HEARTBEAT_LOCK(x) spin_lock_bh(x)
#define BKNI_P_MUTEXTRACKING_HEARTBEAT_UNLOCK(x) spin_unlock_bh(x)
#define BKNI_P_MUTEXTRACKING_HEARTBEAT_DECLARE(x,heartBeat,delayms) static void BKNI_P_MutexTracking_HeartBeat(unsigned long); static struct timer_list BKNI_P_MutexTracking_Timer = TIMER_INITIALIZER(BKNI_P_MutexTracking_HeartBeat, delayms, 0); static void BKNI_P_MutexTracking_HeartBeat(unsigned long unused) {BSTD_UNUSED(unused);if(g_init){x();mod_timer(&BKNI_P_MutexTracking_Timer, jiffies+msecs_to_jiffies(delayms));}return;}
#define BKNI_P_MUTEXTRACKING_HEARTBEAT_START(x) mod_timer(&BKNI_P_MutexTracking_Timer, jiffies)
#define BKNI_P_MUTEXTRACKING_TICK_TYPE  unsigned long
#define BKNI_P_MUTEXTRACKING_TICK_GET(x) *(x) = jiffies
#define BKNI_P_MUTEXTRACKING_TICK_DIFF_MS(x1,x2) jiffies_to_msecs(*(x1)-*(x2))
#define BKNI_P_MUTEXTRACKING_FORCE_RELEASE(x) BKNI_P_ForceReleaseMutex(x)
#endif /* if BKNI_DEBUG_MUTEX_TRACKING */

#include "bkni_mutex_tracking.inc"



BDBG_OBJECT_ID(BKNI_Event);
BDBG_OBJECT_ID(BKNI_Mutex);

void* BKNI_Malloc_tagged(size_t size, const char *file, unsigned line);
void BKNI_Free_tagged(void *mem, const char *file, unsigned line);

static struct tasklet_struct *g_pIsrTasklet;
static struct tasklet_struct *g_pPendingTasklet;
static sigset_t g_blockedSignals;

#ifdef BKNI_EVENTGROUP_SUPPORT
struct BKNI_GroupObj {
    BLST_D_HEAD(group, BKNI_EventObj) members;
    BKNI_MutexHandle lock;           /* mutex for protecting signal and conditional variables */
    wait_queue_head_t wq;
};
#endif

struct BKNI_EventObj {
    BDBG_OBJECT(BKNI_Event)
#ifdef BKNI_EVENTGROUP_SUPPORT
    BLST_D_ENTRY(BKNI_EventObj) list;
    struct BKNI_GroupObj *group;
#endif
    wait_queue_head_t wq;
    atomic_t eventset;
};

struct BKNI_MutexObj {
    BKNI_P_MutexTracking tracking; /* must be first */
    BDBG_OBJECT(BKNI_Mutex)
    struct semaphore sem;
    unsigned count;
};

#if BKNI_TRACK_MALLOCS
static DECLARE_MUTEX(g_alloc_mutex);
#endif

#define B_TRACK_ALLOC_LOCK()  down(&g_alloc_mutex);
#define B_TRACK_ALLOC_UNLOCK() up(&g_alloc_mutex);
#define B_TRACK_ALLOC_ALLOC(size) kmalloc(size, GFP_KERNEL)
#define B_TRACK_ALLOC_FREE(ptr) kfree(ptr)
#define B_TRACK_ALLOC_OS "linuxkernel"

#include "bkni_track_mallocs.inc"


BERR_Code BKNI_Init(void)
{
    if (!g_init) {
        int i;

        /* A list of terminal signals.  Any of these signals will trigger a
           BERR_OS_ERROR if received while waiting or sleeping */
        static const int terminal_signals[ ] ={
            SIGCONT, /* Continue if stopped */
            SIGSTOP, /* Stop Stop process */
            SIGTSTP, /* Stop typed at tty */
            SIGKILL,
            SIGTERM,
            SIGINT,
            SIGHUP,
            SIGQUIT,
            SIGABRT,
            SIGILL,
            SIGSEGV,
            SIGBUS
        };

        g_init = true;
        BKNI_P_MutexTrackingState_Init();

        /* By default, block all signals except for the ones listed above */
        sigfillset(&g_blockedSignals);
        for(i=0;i<sizeof(terminal_signals)/sizeof(*terminal_signals);i++) {
            sigdelset(&g_blockedSignals, terminal_signals[i]);
        }

#ifdef BKNI_METRICS_ENABLED
        BKNI_Metrics_Reset();
#endif

        BKNI_P_TrackAlloc_Init();

        return BERR_SUCCESS;
    }
    else
        return BERR_OS_ERROR;
}

void BKNI_Uninit(void)
{
    g_init = false;
    BKNI_P_MutexTrackingState_Uninit();
    BKNI_P_TrackAlloc_Uninit();
}

/* coverity[+kill]  */
void BKNI_Fail(void)
{
    volatile int i=0;
    volatile int j=0;
    g_init = false;
    printk("BKNI_Fail: forcing oops\n");
    i = *(int *)i;
    i /= j;
    panic("BKNI_Fail: panic...");
}

void *BKNI_Memchr(const void *s, int c, size_t n)
{
    const unsigned char *p = s;
    while (n-- != 0) {
        if ((unsigned char)c == *p++) {
            return (void *)(p-1);
        }
    }
    return NULL;
}

/**
* WARNING: This implementation is unfortunate, but it is safe, simple,
* and memory is only used when it is called. For a release build
* it is no doubt unacceptable.
**/
int BKNI_Vprintf(const char *fmt, va_list ap)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    /* 2.6 has a proper vprintk */
    return vprintk(fmt,ap);
#else
/**
 * WARNING: This implementation is unfortunate, but it is safe, simple,
 * and memory is only used when it is called.  You must be careful about
 * Linux's limited kernel stack size, so it is better to use static data
 * w/semaphore instead of putting such a large buffer on the stack.
**/
#define VPRINTF_BUFSIZE 1024
    static DECLARE_MUTEX(vprintf_mutex);
    static char buf[VPRINTF_BUFSIZE];
    static char isrbuf[VPRINTF_BUFSIZE];
    int rc;

    if ( CHECK_CRITICAL() )
    {
        /* ISR's need a different buffer to avoid a semaphore lock */
        vsnprintf(isrbuf, VPRINTF_BUFSIZE, fmt, ap);
        rc = printk("isr: %s", isrbuf);
    }
    else
    {
        down(&vprintf_mutex);

        vsnprintf(buf, VPRINTF_BUFSIZE, fmt, ap);
        rc = printk(buf);

        up(&vprintf_mutex);
    }

    return rc;
#endif
}

void BKNI_Delay_tagged(int microsec, const char *file, unsigned line)
{
#ifdef BKNI_METRICS_ENABLED
    /* metrics */
    g_metrics.totalDelays++;
    g_metrics.totalDelayTime += microsec;
    if (microsec > g_metrics.maxDelayTime)
        g_metrics.maxDelayTime = microsec;
    if (g_metricsLoggingState.printDelays)
        BDBG_P_PrintString("BKNI_Delay(%d) at %s, line %d\n", microsec, file, line);
#endif
    udelay(microsec);
}

BERR_Code BKNI_Sleep_tagged(int millisec, const char *file, unsigned line)
{
    unsigned long ticks;
    long rc;
    sigset_t mask;
    BERR_Code retval;

    ASSERT_NOT_CRITICAL();

#ifdef BKNI_METRICS_ENABLED
    /* metrics */
    g_metrics.totalSleeps++;
    g_metrics.totalSleepTime += millisec;
    if (millisec > g_metrics.maxSleepTime)
        g_metrics.maxSleepTime = millisec;
    if (g_metricsLoggingState.printSleeps)
        BDBG_P_PrintString("BKNI_Sleep(%d) at %s, line %d\n", millisec, file, line);
#endif
    ticks = (millisec * HZ) / 1000;

    /* Each tick is 1 or 10ms, so we must wait at least that long */
    if (ticks == 0) {
        ticks = 1;
    }

    /* Block all non-terminal signals while sleeping */
    BKNI_P_BlockSignals(&mask);

    for(;;) {
        set_current_state(TASK_INTERRUPTIBLE);
        rc = schedule_timeout(ticks);
        if (rc==0) {
            retval = BERR_SUCCESS;
            break;
        }
        if (BKNI_P_signalPending()) {
            retval = BERR_TRACE(BERR_OS_ERROR);
            break;
        }
        ticks = rc; /* keep sleeping */
    }

    /* Restore original signal mask */
    BKNI_P_RestoreSignals(&mask);

    return retval;
}

BERR_Code BKNI_CreateEvent_tagged(BKNI_EventHandle *p_event, const char *file, unsigned line)
{
    BKNI_EventHandle event;

    ASSERT_NOT_CRITICAL();

    event = BKNI_Malloc_tagged(sizeof(*event), file, line);
    if (!event)
        return BERR_OS_ERROR;
    BDBG_OBJECT_SET(event, BKNI_Event);
    atomic_set(&event->eventset, 0);
#ifdef BKNI_EVENTGROUP_SUPPORT
    event->group = NULL;
#endif
    init_waitqueue_head(&event->wq);
    *p_event = event;
    return BERR_SUCCESS;
}

void BKNI_DestroyEvent_tagged(BKNI_EventHandle event, const char *file, unsigned line)
{
    ASSERT_NOT_CRITICAL();
    BDBG_OBJECT_ASSERT(event, BKNI_Event);

#ifdef BKNI_EVENTGROUP_SUPPORT
    {
    struct BKNI_GroupObj *group = event->group;

        if (group) {
            BKNI_AcquireMutex(group->lock);
            BLST_D_REMOVE(&group->members, event, list);
            BKNI_ReleaseMutex(group->lock);
        }
    }
#endif
    BDBG_OBJECT_DESTROY(event, BKNI_Event);
    BKNI_Free_tagged(event, file, line);
}

#undef BKNI_CreateEvent
BERR_Code BKNI_CreateEvent(BKNI_EventHandle *p_event)
{
    return BKNI_CreateEvent_tagged(p_event, NULL, 0);
}

#undef BKNI_DestroyEvent
void BKNI_DestroyEvent(BKNI_EventHandle event)
{
    BKNI_DestroyEvent_tagged(event, NULL, 0);
}

void BKNI_SetEvent_tagged(BKNI_EventHandle event, const char *file, unsigned line)
{
    BDBG_OBJECT_ASSERT(event, BKNI_Event);
    atomic_set(&event->eventset, 1);
#ifdef BKNI_METRICS_ENABLED
    if (g_metricsLoggingState.printEvents)
        BDBG_P_PrintString("BKNI_SetEvent(%p) at %s, line %d\n", event, file, line);
#endif
    wake_up_interruptible(&event->wq);

#ifdef BKNI_EVENTGROUP_SUPPORT
    {
        struct BKNI_GroupObj *group = event->group;
        if (group) {
            wake_up_interruptible(&group->wq);
        }
    }
#endif
}

void BKNI_ResetEvent_tagged(BKNI_EventHandle event, const char *file, unsigned line)
{
    BDBG_OBJECT_ASSERT(event, BKNI_Event);
    /* just clear event state */
    atomic_set(&event->eventset, 0);
    return;
}

/**
* This will modify the caller's signal mask to block all signals
* except for the terminal signals listed in BKNI_Init().  This prevents
* user signals from interrupting magnum code, but they will be safely
* dispatched at the next opportunity.
**/
static void BKNI_P_BlockSignals(sigset_t *pPrevMask)
{
    unsigned long flags;
    spinlock_t *pSignalLock;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    /* 2.6 signal lock */
    pSignalLock = &current->sighand->siglock;
#else
    /* 2.4 signal lock */
    pSignalLock = &current->sigmask_lock;
#endif

    /* Lock the signal structs, should use spin_lock_irq since the same
     * spinlock coule be acqured by the kernel code from within IRQ handler */
    spin_lock_irqsave(pSignalLock, flags);

    /* Save current signals */
    memcpy(pPrevMask, &current->blocked, sizeof(sigset_t));
    /* Set to block all but the terminal signals */
    memcpy(&current->blocked, &g_blockedSignals, sizeof(sigset_t));
    /* Must be called after manipulating blocked signals */
    recalc_sigpending();

    /* Release the lock */
    spin_unlock_irqrestore(pSignalLock, flags);
    return;
}

/**
* This will restore the original signal mask saved by BKNI_P_BlockSignals()
**/
static void BKNI_P_RestoreSignals(sigset_t *pPrevMask)
{
    unsigned long flags;
    spinlock_t *pSignalLock;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    /* 2.6 signal lock */
    pSignalLock = &current->sighand->siglock;
#else
    /* 2.4 signal lock */
    pSignalLock = &current->sigmask_lock;
#endif

    /* Lock the signal structs, should use spin_lock_irq since the same
     * spinlock coule be acqured by the kernel code from within IRQ handler */
    spin_lock_irqsave(pSignalLock, flags);

    /* Restore signals */
    memcpy(&current->blocked, pPrevMask, sizeof(sigset_t));
    /* Must be called after manipulating blocked signals */
    recalc_sigpending();

    /* Release the lock */
    spin_unlock_irqrestore(pSignalLock, flags);
}

/**
* This will return 1 if a signal is pending.  Previous implementations
* of this function would dequeue all non-terminal signals to allow the
* caller to continue waiting.  That approach had the drawback of consuming
* any non-terminal signals and preventing the application from using them.
* The new implementation assumes that the caller has called BKNI_P_BlockSignals()
* prior to waiting on an event or timeout and therefore can only be interrupted by
* a terminal signal.  When the original signal mask is restored via BKNI_P_RestoreSignals,
* the user signals will be dispatched again.
**/
static int BKNI_P_signalPending(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
    while (signal_pending(current)) {
        if ( ! try_to_freeze()) {       /* try to freeze returns 0 when no freeze request */
            return 1;                   /* signal pending (but no freeze request)         */
        }
    }
    return 0;                           /* no signal (or freeze request) pending          */
#else
    if (signal_pending(current)) {
        return 1;
    }
    return 0;
#endif
}


/**
* This WaitForEvent consumes all signals. This is allowed by KNI impl's, but not allowed
* for Magnum code to us.
**/
BERR_Code BKNI_WaitForEvent_tagged(BKNI_EventHandle event, int timeoutMsec, const char *file, unsigned line)
{
    BERR_Code result = BERR_TIMEOUT;
    sigset_t mask;
    int prev_eventset;

    BDBG_OBJECT_ASSERT(event, BKNI_Event);
    if ( timeoutMsec )
    {
        ASSERT_NOT_CRITICAL();
    }
    else if ( CHECK_CRITICAL() )
    {
        /* Don't mess with current or wait queues from an ISR */
        prev_eventset = atomic_xchg(&event->eventset, 0); /* this atomically clears 'eventset' and returns current (old) value */
        if ( prev_eventset)
        {
            return BERR_SUCCESS;
        }
        else
        {
            return BERR_TIMEOUT;
        }
    }

    /* This is used to achieve consistency between different OS's. */
    if (timeoutMsec>0 && timeoutMsec<30)
    {
        /* wait at least 30 msec */
        timeoutMsec = 30;
    }

    BKNI_P_BlockSignals(&mask);

#ifdef BKNI_METRICS_ENABLED
    if (g_metricsLoggingState.printEvents)
        BDBG_P_PrintString("BKNI_WaitForEvent(%p,%d) at %s, line %d\n", event,timeoutMsec, file, line);
#endif
    if (BKNI_P_signalPending())
        result = BERR_OS_ERROR;
    else
    {
        wait_queue_t wqe;
        unsigned long ticks;

        init_waitqueue_entry(&wqe, current);
        add_wait_queue(&event->wq, &wqe);

        if (timeoutMsec == BKNI_INFINITE)
            ticks = MAX_SCHEDULE_TIMEOUT;
        else if (timeoutMsec)
            ticks = (timeoutMsec * HZ) / 1000;
        else
            ticks = 0;

        /* Need to repeat the sleep until the entire timeout
        is consumed, or event occurs, or a true fatal signal is detected.
        It's possible to be signalled and yet keep going. */
        for ( ;; )
        {
            /* Be sure to go half asleep before checking condition. */
            /* Otherwise we have a race condition between when we   */
            /* check the condition and when we call schedule().     */
            set_current_state(TASK_INTERRUPTIBLE);

            prev_eventset = atomic_xchg(&event->eventset, 0); /* this atomically clears 'eventset' and returns current (old) value */
            if(prev_eventset) 
            {
                result = BERR_SUCCESS;
                break;
            }
            else if (!ticks)
            {
                result = BERR_TIMEOUT;
                break;
            }
            else
            {
                /* When SetEvent is called, event process on event->wq is woken. */
                ticks = schedule_timeout(ticks);
                if (BKNI_P_signalPending())
                {
                    result = BERR_OS_ERROR;
                    break;
                }
            }
        }

        set_current_state(TASK_RUNNING);
        remove_wait_queue(&event->wq, &wqe);
    }

    BKNI_P_RestoreSignals(&mask);

    return result;
}

static spinlock_t g_criticalSection = __SPIN_LOCK_UNLOCKED(bkni.lock);
static DECLARE_MUTEX(g_csMutex);
#if BKNI_DEBUG_CS_TIMING
static unsigned long g_csTimeStart;
static const char *g_csFile;
static int g_csLine;
#endif

#ifdef BKNI_MASK_INTERRUPTS
static unsigned long g_flags;
#endif

void BKNI_EnterCriticalSection_tagged(const char *file, unsigned line)
{
    ASSERT_NOT_CRITICAL();

    if ( g_pIsrTasklet )
    {
        down(&g_csMutex);
        tasklet_disable(g_pIsrTasklet);
    }
    else
    {
        #ifdef BKNI_MASK_INTERRUPTS
        unsigned long flags;
        spin_lock_irqsave(&g_criticalSection, flags);
        g_flags = flags;
        #else
        spin_lock_bh(&g_criticalSection);
        #endif
    }

    SET_CRITICAL();

#if BKNI_DEBUG_CS_TIMING
    g_csTimeStart = read_c0_count();
    g_csFile = file;
    g_csLine = line;
#endif

#ifdef BKNI_METRICS_ENABLED
    /* metrics and logging */
    g_metrics.totalCriticalSections++;
    g_criticalSectionStart = jiffies;
    if (g_metricsLoggingState.printCriticalSections)
        BDBG_P_PrintString("BKNI_EnterCriticalSection() at %s, line %d\n", file, line);
#endif
}

void BKNI_LeaveCriticalSection_tagged(const char *file, unsigned line)
{
    struct tasklet_struct *pIsrTasklet;
#if BKNI_DEBUG_CS_TIMING
    uint32_t currentCount, elapsedCount;
#endif

#if BKNI_DEBUG_CS_TIMING
    /* Snapshot time */
    currentCount = read_c0_count();

    if ( currentCount <= g_csTimeStart )
    {
        elapsedCount = currentCount - g_csTimeStart;
    }
    else
    {
        elapsedCount = currentCount + (0xFFFFFFFFUL-g_csTimeStart);
    }

    if ( elapsedCount > 148500 )    /* 1ms on a 97400A, 97401, or 97038 */
    {
        printk("<0>Long CS detected (%u.%u ms).\nEntered: %s:%d\nLeaving %s:%d\n",
               elapsedCount/148500, (elapsedCount%148500)/1485, g_csFile, g_csLine, file, line);
    }
#endif

    ASSERT_CRITICAL();

#ifdef BKNI_METRICS_ENABLED
    {
        long totalTime = jiffies - g_criticalSectionStart;

        /* metrics and logging */
        g_metrics.totalCriticalSectionTime += totalTime;
        if (totalTime > g_metrics.maxCriticalSectionTime)
            g_metrics.maxCriticalSectionTime = totalTime;
        if (g_metricsLoggingState.printCriticalSections)
            BDBG_P_PrintString("BKNI_LeaveCriticalSection() at %s, line %d\n", file, line);
    }
#endif

    CLEAR_CRITICAL();

    /* Store tasklet and replace with any possible changes */
    pIsrTasklet = g_pIsrTasklet;
    g_pIsrTasklet = g_pPendingTasklet;

    /* Re-enable interrupts in the same way they were disabled */
    if ( pIsrTasklet )
    {
        tasklet_enable(pIsrTasklet);
        up(&g_csMutex);
    }
    else
    {
        #ifdef BKNI_MASK_INTERRUPTS
        unsigned long flags = g_flags;
        spin_lock_irqrestore(&g_criticalSection, flags);
        #else
        spin_unlock_bh(&g_criticalSection);
        #endif
    }
}

void BKNI_AssertIsrContext(const char *filename, unsigned lineno)
{
    if ( !CHECK_CRITICAL() ) {
        BDBG_P_AssertFailed("Not in critical section", filename, lineno);
    }
}


/***********************************************************

BKNI_Multi.h

************************************************************/

BERR_Code BKNI_CreateMutex_tagged(BKNI_MutexHandle *mutex, const char *file, int line)
{
    ASSERT_NOT_CRITICAL();

    *mutex = BKNI_Malloc_tagged(sizeof(**mutex), file, line);
    if (!*mutex)
        return BERR_OS_ERROR;
    BDBG_OBJECT_SET(*mutex, BKNI_Mutex);
    BKNI_P_MutexTracking_Init(&(*mutex)->tracking, file, line);
    init_MUTEX(&(*mutex)->sem);
    (*mutex)->count = 0;
    return BERR_SUCCESS;
}

void
BKNI_DestroyMutex_tagged(BKNI_MutexHandle mutex, const char *file, int line)
{
    ASSERT_NOT_CRITICAL();
    BDBG_OBJECT_ASSERT(mutex, BKNI_Mutex);
    BKNI_P_MutexTracking_Uninit(&mutex->tracking);
    BDBG_OBJECT_DESTROY(mutex, BKNI_Mutex);
    BKNI_Free_tagged(mutex, file, line);
}

#undef BKNI_CreateMutex
BERR_Code BKNI_CreateMutex(BKNI_MutexHandle *mutex)
{
    return BKNI_CreateMutex_tagged(mutex, NULL, 0);
}

#undef BKNI_DestroyMutex
void BKNI_DestroyMutex(BKNI_MutexHandle mutex)
{
    BKNI_DestroyMutex_tagged(mutex, NULL, 0);
}

BERR_Code BKNI_AcquireMutex_tagged(BKNI_MutexHandle mutex, const char *file, unsigned line)
{
    ASSERT_NOT_CRITICAL();
    BDBG_OBJECT_ASSERT(mutex, BKNI_Mutex);

#ifdef BKNI_METRICS_ENABLED
    if (g_metricsLoggingState.printMutexSections)
        BDBG_P_PrintString("BKNI_AcquireMutex(%p) at %s, line %d\n", mutex, file, line);
#endif
    down(&mutex->sem);
    BKNI_P_MutexTracking_AfterAcquire(&mutex->tracking, file, line);

    BDBG_ASSERT(mutex->count == 0);
    mutex->count++;

#ifdef BKNI_METRICS_ENABLED
    g_metrics.totalMutexSections++;
    g_mutexSectionStart = jiffies;
#endif

    return BERR_SUCCESS;
}

/**
* No possibility of BERR_OS_ERROR in TryAcquireMutex.
**/
BERR_Code BKNI_TryAcquireMutex_tagged(BKNI_MutexHandle mutex, const char *file, unsigned line)
{
    ASSERT_NOT_CRITICAL();
    BDBG_OBJECT_ASSERT(mutex, BKNI_Mutex);

#ifdef BKNI_METRICS_ENABLED
    if (g_metricsLoggingState.printMutexSections)
        BDBG_P_PrintString("BKNI_TryAcquireMutex(%p) at %s, line %d\n", mutex, file, line);
#endif
    if (down_trylock(&mutex->sem)) {
#ifdef BKNI_METRICS_ENABLED
        if (g_metricsLoggingState.printMutexSections)
            BDBG_P_PrintString("BKNI_TryAcquireMutex failed\n");
#endif
        return BERR_TIMEOUT;
    }
    else {
        BKNI_P_MutexTracking_AfterAcquire(&mutex->tracking, file, line);
        BDBG_ASSERT(mutex->count == 0);
        mutex->count++;
#ifdef BKNI_METRICS_ENABLED
        g_metrics.totalMutexSections++;
        g_mutexSectionStart = jiffies;
#endif
        return BERR_SUCCESS;
    }
}

void BKNI_ReleaseMutex_tagged(BKNI_MutexHandle mutex, const char *file, unsigned line)
{
#ifdef BKNI_METRICS_ENABLED
    long totalTime = jiffies - g_mutexSectionStart;
    if (g_metricsLoggingState.printMutexSections)
        BDBG_P_PrintString("BKNI_ReleaseMutex(%p) at %s, line %d\n", mutex, file, line);
    g_metrics.totalMutexSectionTime += totalTime;
    if (totalTime > g_metrics.maxMutexSectionTime)
        g_metrics.maxMutexSectionTime += totalTime;
#endif
    ASSERT_NOT_CRITICAL();
    BDBG_OBJECT_ASSERT(mutex, BKNI_Mutex);

    /* if release is called when not acquired, we must not call up(). magnum cannot survive this bug, so we need to fail */
    if (mutex->count != 1) {
        BDBG_P_PrintString("unacquired mutex cannot be released at %s:%d (count %d)\n", file, line, mutex->count);
        BKNI_Fail();
    }
    mutex->count--;
    BKNI_P_MutexTracking_BeforeRelease(&mutex->tracking);

    up(&mutex->sem);
}

#if BKNI_DEBUG_MUTEX_TRACKING
static void BKNI_P_ForceReleaseMutex(void *x)
{
   up(&((struct BKNI_MutexObj *)x)->sem);
   return;
}
#endif

#ifdef BKNI_METRICS_ENABLED
void BKNI_Metrics_Get(BKNI_Metrics *metrics)
{
    memcpy(metrics, &g_metrics, sizeof(g_metrics));
}

void BKNI_Metrics_Reset(void)
{
    memset(&g_metrics, 0, sizeof(g_metrics));
}

void BKNI_Metrics_GetLoggingState(BKNI_Metrics_LoggingState *logging)
{
    memcpy(logging, &g_metricsLoggingState, sizeof(g_metricsLoggingState));
}

void BKNI_Metrics_SetLoggingState(const BKNI_Metrics_LoggingState *logging)
{
    memcpy(&g_metricsLoggingState, logging, sizeof(g_metricsLoggingState));
}
#endif

/**********************************
*
* ISR Tasklet
*
*/
BERR_Code
BKNI_LinuxKernel_SetIsrTasklet(void *pTasklet)
{
    ASSERT_NOT_CRITICAL();

    /* For safety, enter the critical section */
    BKNI_EnterCriticalSection();

    /* Set tasklet pointer -- this is done to avoid having 2 copies of LeaveCriticalSection, which is complex*/
    g_pPendingTasklet = (struct tasklet_struct *)pTasklet;

    /* Leaving Critical section will swap this to active safely */
    BKNI_LeaveCriticalSection();

    return BERR_SUCCESS;
}

/**********************************
*
* Event Groups
*
*/

#ifdef BKNI_EVENTGROUP_SUPPORT
BERR_Code
BKNI_CreateEventGroup(BKNI_EventGroupHandle *pGroup)
{
    BKNI_EventGroupHandle group;
    BERR_Code result;

    ASSERT_NOT_CRITICAL();

    group = BKNI_Malloc(sizeof(*group));
    if (!group) {
        result = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto error;
    }
    BLST_D_INIT(&group->members);
    if (BKNI_CreateMutex(&group->lock)) {
        result = BERR_TRACE(BERR_OS_ERROR);
        goto error;
    }
    init_waitqueue_head(&group->wq);
    *pGroup = group;

    return BERR_SUCCESS;

error:
    if (group)
        BKNI_Free(group);
    return result;
}

void
BKNI_DestroyEventGroup(BKNI_EventGroupHandle group)
{
    BKNI_EventHandle event;

    ASSERT_NOT_CRITICAL();

    BKNI_AcquireMutex(group->lock);
    while(NULL != (event=BLST_D_FIRST(&group->members)) ) {
        event->group = NULL;
        BLST_D_REMOVE_HEAD(&group->members, list);
    }
    BKNI_ReleaseMutex(group->lock);
    BKNI_DestroyMutex(group->lock);
    BKNI_Free(group);
    return;
}


BERR_Code
BKNI_AddEventGroup(BKNI_EventGroupHandle group, BKNI_EventHandle event)
{
    BERR_Code result = BERR_SUCCESS;

    ASSERT_NOT_CRITICAL();

    BKNI_AcquireMutex(group->lock);
    if (event->group != NULL) {
        printk("### Event %#x already connected to the group %#x\n", (unsigned)event, (unsigned)group);
        result = BERR_TRACE(BERR_OS_ERROR);
    } else {
        BLST_D_INSERT_HEAD(&group->members, event, list);
        event->group = group;
        if (atomic_read(&event->eventset)) { /* we can't use clear and return, since 'eventset' evaluated and cleared later in WaitForGroup */
            /* signal condition if signal already set */
            wake_up_interruptible(&group->wq);
       }
    }
    BKNI_ReleaseMutex(group->lock);
    return result;
}

BERR_Code
BKNI_RemoveEventGroup(BKNI_EventGroupHandle group, BKNI_EventHandle event)
{
    BERR_Code result = BERR_SUCCESS;

    ASSERT_NOT_CRITICAL();

    BKNI_AcquireMutex(group->lock);
    if (event->group != group) {
        printk("### Event %#x doesn't belong to the group %#x\n", (unsigned)event, (unsigned)group);
        result = BERR_TRACE(BERR_OS_ERROR);
    } else {
        BLST_D_REMOVE(&group->members, event, list);
        event->group = NULL;
    }
    BKNI_ReleaseMutex(group->lock);
    return result;
}

static unsigned
group_get_events(BKNI_EventGroupHandle group, BKNI_EventHandle *events, unsigned max_events)
{
    BKNI_EventHandle ev;
    unsigned event;

    BKNI_AcquireMutex(group->lock);
    for(event=0, ev=BLST_D_FIRST(&group->members); ev && event<max_events ; ev=BLST_D_NEXT(ev, list)) {
        int prev_eventset;
        prev_eventset = atomic_xchg(&ev->eventset, 0); /* this atomically clears 'eventset' and returns icurrent (old) value */
        if (prev_eventset) {
            events[event] = ev;
            event++;
        }
    }
    BKNI_ReleaseMutex(group->lock);
    return event;
}

BERR_Code
BKNI_WaitForGroup(BKNI_EventGroupHandle group, int timeoutMsec, BKNI_EventHandle *events, unsigned max_events, unsigned *nevents)
{
    BERR_Code result = BERR_SUCCESS;
    wait_queue_t wqe;
    unsigned long ticks;
    sigset_t mask;

    ASSERT_NOT_CRITICAL();

    if (max_events<1) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Block all non-terminal signals while sleeping */
    BKNI_P_BlockSignals(&mask);

    init_waitqueue_entry(&wqe, current);
    add_wait_queue(&group->wq, &wqe);

    if (timeoutMsec == BKNI_INFINITE)
        ticks = MAX_SCHEDULE_TIMEOUT;
    else if (timeoutMsec)
        ticks = ((timeoutMsec * HZ + (1000-1))) / 1000; /* if timeoutMsec > 0, then we should sleep at least one jiffy, so we calculate ((timeoutMsec/1000) * HZ) + 0.999 */
    else
        ticks = 0;

    /* Need to repeat the sleep until the entire timeout
    is consumed, or event occurs, or a true fatal signal is detected.
    It's possible to be signalled and yet keep going. */
    for ( ;; )
    {
        /* Be sure to go half asleep before checking condition. */
        /* Otherwise we have a race condition between when we   */
        /* check the condition and when we call schedule().     */
        set_current_state(TASK_INTERRUPTIBLE);
    
            *nevents = group_get_events(group, events, max_events);
            if(*nevents) {
                result = BERR_SUCCESS;
            break;
        }
        else if (!ticks) {
            result = BERR_TIMEOUT; /* return timeout if no events set */
            break;
        }
        else
        {
            /* When SetEvent is called, event process on event->wq is woken. */
            ticks = schedule_timeout(ticks);
            if (BKNI_P_signalPending())
            {
                result = BERR_OS_ERROR;
                break;
            }  
        }
    }

    current->state = TASK_RUNNING;
    remove_wait_queue(&group->wq, &wqe);

    /* Restore original signal mask */
    BKNI_P_RestoreSignals(&mask);

    return result;
}
#else
BERR_Code
BKNI_CreateEventGroup(BKNI_EventGroupHandle *pGroup)
{
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}
void
BKNI_DestroyEventGroup(BKNI_EventGroupHandle group)
{
}
BERR_Code
BKNI_AddEventGroup(BKNI_EventGroupHandle group, BKNI_EventHandle event)
{
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}
BERR_Code
BKNI_RemoveEventGroup(BKNI_EventGroupHandle group, BKNI_EventHandle event)
{
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}
BERR_Code
BKNI_WaitForGroup(BKNI_EventGroupHandle group, int timeoutMsec, BKNI_EventHandle *events, unsigned max_events, unsigned *nevents)
{
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}
#endif

void *BKNI_Memset(void *mem, int ch, size_t n)
{
    return memset(mem, ch, n);
}

void *BKNI_Memcpy(void *dest, const void *src, size_t n)
{
    return memcpy(dest, src, n);
}

int BKNI_Memcmp(const void *s1, const void *s2, size_t n)
{
    return memcmp(s1, s2, n);
}

void *BKNI_Memmove(void *dest, const void *src, size_t n)
{
    return memmove(dest, src, n);
}

int BKNI_Printf(const char *fmt, ...)
{
    va_list ap;
    int rc;
    va_start(ap, fmt);
    rc = BKNI_Vprintf(fmt, ap);
    va_end(ap);
    return rc;
}

int BKNI_Snprintf(char *s, size_t n, const char *fmt, ...)
{
    va_list ap;
    int rc;
    va_start(ap, fmt);
    rc = vsnprintf(s, n, fmt, ap);
    va_end(ap);
    return rc;
}

int BKNI_Vsnprintf(char *s, size_t n, const char *fmt, va_list ap)
{
    return vsnprintf(s, n, fmt, ap);
}


