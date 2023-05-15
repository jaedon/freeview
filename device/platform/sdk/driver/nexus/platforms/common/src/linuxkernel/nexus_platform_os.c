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
* $brcm_Revision: 112 $
* $brcm_Date: 10/5/12 11:13a $
*
* API Description:
*   API name: Platform linuxuser
*    linuxkernel OS routines
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/linuxkernel/nexus_platform_os.c $
* 
* 112   10/5/12 11:13a mphillip
* SW7425-3963: Merge interrupt changes to main
* 
* SW7425-3963/1   10/3/12 4:30p mphillip
* SW7425-3963: Fix kernel mode interrupt lookups
* 
* 111   10/3/12 1:43p erickson
* SW7435-364: fix printout
* 
* 110   9/26/12 11:01a erickson
* SW7435-364: improve printout for runaway L1 interrupts
* 
* 109   9/25/12 9:47a erickson
* SW7435-364: monitor for runaway L1 interrupts
* 
* 108   7/24/12 4:09p vsilyaev
* SW7425-3488: Removed unused code
* 
* 107   7/13/12 11:57a erickson
* SW7552-303: use OS-reported max_dcache_line_size instead of nexus macro
* 
* 106   7/9/12 11:29a erickson
* SW7425-3418: rework NEXUS_StopCallbacks to avoid locking core module
* 
* 105   2/21/12 6:08p mward
* SW7435-37: Support Linux 3.3 for NEXUS_MODE=proxy. (merge)
* 
* SW7435-37/1   2/21/12 11:30a mward
* SW7435-37:  Support Linux 3.3 for NEXUS_MODE=proxy.
* 
* 104   2/6/12 11:45a vsilyaev
* SW7420-2214: Don't use gating of tasklet_hi_schedule, instead relly on
*  properties of tasklet_hi_schedule to queue only single instance of
*  tasklet
* 
* 103   1/6/12 9:36a erickson
* SW7420-1318: NEXUS_Platform_P_IrqMap still required to manage L1's with
*  no L2's (for instance, external irqs)
* 
* 102   1/4/12 3:43p erickson
* SW7420-1318: revert to previous type and use terminator for test
* 
* 101   1/4/12 3:22p erickson
* SW7420-1318: if NEXUS_Platform_P_IrqMap is NULL, use BINT_P_IntMap for
*  L1 validation
* 
* 100   11/18/11 1:03p erickson
* SW7420-2144: if linux_irq is 0, do not connect. it is unused by nexus.
* 
* 99   11/15/11 3:19p erickson
* SW7425-1747: remove old code
* 
* 98   11/9/11 2:48p ahulse
* SW3461-53: Separate state of irq state in kernel and irq state of
*  pi/nexus
* 
* 97   8/3/11 5:33p vsilyaev
* SW7420-1465, SW7405-5221: Route debug output to the special FIFO
*  instead of syslog buffer
* 
* 96   7/27/11 12:00p jtna
* SW7346-360: Coverity defect 33743: OVERRUN_STATIC
* 
* 95   7/25/11 1:10p erickson
* SWDTV-8071: move free_irq outside spin_lock_irqsave
* 
* 94   7/11/11 11:43a erickson
* SWDTV-7208: add DTV kernel mode support
* 
* 93   5/16/11 4:19p erickson
* SW7420-1819: add alternative impl of NEXUS_Platform_P_TerminateProcess
* 
* 92   5/10/11 12:15p erickson
* SW7420-1819: fix linux 2.6.18 builds
* 
* 91   5/4/11 1:51p erickson
* SW7346-187: require 40NM uses 2.6.31-3.2 or greater
* 
* 90   5/3/11 5:08p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
***************************************************************************/

#include "nexus_platform_priv.h"
#include "bkni.h"
#include "bdbg_log.h"
#include "nexus_interrupt_map.h"
#include "nexus_generic_driver_impl.h"
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/kmod.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)
    #include <linux/kconfig.h>
    #include <linux/brcmstb/brcmapi.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
/* DTV */
#if  BMIPS_ZEPHYR_40NM
    #include <brcmapi.h>
    #ifndef BRCM_MAGNUM_SPINLOCK
        static spinlock_t g_magnum_spinlock = SPIN_LOCK_UNLOCKED;
    #else
        extern spinlock_t g_magnum_spinlock;
    #endif
    #define brcm_magnum_spinlock g_magnum_spinlock
    #define UMH_WAIT_PROC 1
#else
    #include <asm/brcmstb/brcmapi.h>
#endif
#else
    #include <asm/uaccess.h>
    #include <asm/brcmstb/common/brcmstb.h>
    #ifndef BRCM_MAGNUM_SPINLOCK
    static spinlock_t g_magnum_spinlock = SPIN_LOCK_UNLOCKED;
    #else
    extern spinlock_t g_magnum_spinlock;
    #endif
    #define brcm_magnum_spinlock g_magnum_spinlock
    #define UMH_WAIT_PROC 1
#endif

BDBG_MODULE(nexus_platform_os);


/* TODO: need a Linux macro to detect 2631-3.2 or greater */
#if BMIPS4380_40NM || BMIPS5000_40NM || BMIPS_ZEPHYR_40NM
#define LINUX_2631_3_2_OR_GREATER 1
#endif

/*
The interrupt code is meant to stay in sync with nexus/build/nfe_driver/b_bare_os.c's 
interrupt code.
*/

typedef void (*b_bare_os_special_interrupt_handler)(int linux_irq);
#define BDBG_MSG_IRQ(X) /* BDBG_MSG(X) */

#define NUM_IRQS (32*NEXUS_NUM_L1_REGISTERS)
/* b_bare_os L1 interrupts are 0-based. linux is 1-based. */
#define LINUX_IRQ(i) (i+1)
#define NEXUS_IRQ(i) (i-1)

struct b_bare_interrupt_entry {
    const char *name;
    void (*handler)(void *,int);
    void *context_ptr;
    int context_int;
    b_bare_os_special_interrupt_handler special_handler;
    bool requested;      /* request_irq called. must be defered until first enable. */
    bool enabled;        /* external "state" of irq, enabled by caller. Nexus needs to balance disable/enable_irq calls and */
                         /* keeps this state in "taskletEnabled" */
    bool taskletEnabled; /* true if irq is enabled in the kernel (enable_irq), false if irq is off at kernel (disable_irq) */
    bool shared;
    
    unsigned count;
    bool print;
};

static struct b_bare_interrupt_state {
    spinlock_t lock;
    bool started;
    struct {
        uint32_t IntrStatus;
        uint32_t IntrMaskStatus;
    } processing[NEXUS_NUM_L1_REGISTERS], pending[NEXUS_NUM_L1_REGISTERS];
    struct b_bare_interrupt_entry table[NUM_IRQS];
    struct work_struct task;
} b_bare_interrupt_state = {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,0)
    __SPIN_LOCK_UNLOCKED(b_bare_interrupt_state.lock),
#else
    SPIN_LOCK_UNLOCKED,
#endif
};

static void NEXUS_Platform_P_Isr(unsigned long data);
static DECLARE_TASKLET(NEXUS_Platform_P_IsrTasklet, NEXUS_Platform_P_Isr, 0);

NEXUS_Error
NEXUS_Platform_P_InitOS(void)
{
    NEXUS_Error rc;
    struct b_bare_interrupt_state *state = &b_bare_interrupt_state;
    unsigned i;

    memset(&state->table, 0, sizeof(state->table));
    for(i=0;i<NEXUS_NUM_L1_REGISTERS;i++) {
        state->processing[i].IntrMaskStatus = ~0;
        state->processing[i].IntrStatus = 0;
        state->pending[i] = state->processing[i];
    }
    state->started = true;
    
    /* use g_platformMemory to pass OS value to NEXUS_Platform_P_SetCoreModuleSettings */
    g_platformMemory.max_dcache_line_size = nexus_driver_state.settings.max_dcache_line_size;

    rc = BKNI_LinuxKernel_SetIsrTasklet(&NEXUS_Platform_P_IsrTasklet);
    if ( rc!=BERR_SUCCESS ) { rc = BERR_TRACE(rc); goto err_tasklet;}

    rc = nexus_driver_scheduler_init();
    if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc);goto err_driver; }
    
    return NEXUS_SUCCESS;
    
err_driver:
err_tasklet:
    return rc;
}

NEXUS_Error
NEXUS_Platform_P_UninitOS(void)
{
    struct b_bare_interrupt_state *state = &b_bare_interrupt_state;
    unsigned i;

    nexus_driver_scheduler_uninit();

    state->started = false;
    for(i=0;i<NUM_IRQS;i++) {
        if (state->table[i].handler) {
            NEXUS_Platform_P_DisconnectInterrupt(i);
        }
    }
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_Platform_P_InitOSMem()
{
    /* nothing required */
    return 0;
}

void NEXUS_Platform_P_UninitOSMem()
{
    /* nothing required */
}

/* IMPORTANT: the logic in NEXUS_Platform_P_MapMemory must be carefully paired with NEXUS_Platform_P_UnmapMemory */
void *
NEXUS_Platform_P_MapMemory(unsigned long offset, unsigned long length, bool cached)
{
    void *addr = NULL;

#if LINUX_2631_3_2_OR_GREATER
    if (cached)
        addr = ioremap_cachable(offset, length);
    else
        addr = ioremap_nocache(offset, length);

    if (!addr) {
        BDBG_ERR(("ioremap(%#x, %#x, %s) failed", offset, length, cached?"cached":"uncached"));
    }

#else
#if 1 /* TODO: remove this code if/when __ioremap provides "zone normal" fixed mapping */
#if BMIPS4380_40NM || BMIPS5000_40NM
#error
#elif BMIPS5000_65NM
    /* for the lower 256MB of MEMC0 + 32 MB of register space, we use KSEG0/1 */
    if (offset < 0x12000000) {
        /* fixed map */
        if (cached) {
            addr = (void *)(offset | 0x80000000); /* KSEG0 */
        }
        else {
            addr = (void *)(offset | 0xA0000000); /* KSEG1 */
        }
    }
    /* for the upper 256MB of MEMC0 */
    else if (offset >= 0x20000000 && offset < 0x30000000) {
        /* fixed map */
        if (cached) {
            addr = (void *)((offset-0x20000000) | 0xc0000000);
        }
        else {
            addr = (void *)((offset-0x20000000) | 0xd0000000);
        }
    }
    /* else, fall into ioremap() case */
#else
    /* for the lower 256MB of MEMC0 + 32 MB of register space, we use KSEG0/1 */
    if (offset < 0x12000000) {
        /* fixed map */
        if (cached) {
            addr = (void *)(offset | 0x80000000); /* KSEG0 */
        } else {
            addr = (void *)(offset | 0xA0000000); /* KSEG1 */
        }
    }
    /* else, fall into ioremap() case */
#endif
    else
#endif
    {
        addr = __ioremap(offset, length, cached?_CACHE_CACHABLE_NONCOHERENT:_CACHE_UNCACHED);
        if (!addr) {
            BDBG_ERR(("ioremap(%#x, %#x, %s) failed", offset, length, cached?"cached":"uncached"));
        }
    }
#endif
    return addr;
}

void
NEXUS_Platform_P_UnmapMemory(void *pMem, unsigned long length)
{
    unsigned addr = (unsigned)pMem;

    BSTD_UNUSED(length);

#if LINUX_2631_3_2_OR_GREATER
     iounmap(pMem);
#else
#if BMIPS4380_40NM || BMIPS5000_40NM
#error
#elif BMIPS5000_65NM
    if ((addr >= 0x80000000 && addr < 0x92000000) ||
        (addr >= 0xa0000000 && addr < 0xb2000000) ||
        (addr >= 0xc0000000 && addr < 0xe0000000))
    {
        /* fixed map has no unmap */
    }
#else
    if ((addr >= 0x80000000 && addr < 0x92000000) ||
        (addr >= 0xa0000000 && addr < 0xb2000000))
    {
        /* fixed map has no unmap */
    }
#endif
    else {
        iounmap(pMem);
    }
#endif
    return;
}

NEXUS_Error NEXUS_Platform_P_GetHostMemory(NEXUS_PlatformMemory *pMemory)
{
    unsigned i;
    for (i=0;i<NEXUS_MAX_HEAPS && i<sizeof(nexus_driver_state.settings.region)/sizeof(nexus_driver_state.settings.region[0]);i++) {
        pMemory->osRegion[i].base = nexus_driver_state.settings.region[i].offset;
        pMemory->osRegion[i].length = nexus_driver_state.settings.region[i].size;
    }
    return 0;
}

void
NEXUS_Platform_P_ResetInterrupts(void)
{
    /* not needed */
}

/* ISR handler, calls L1 interrupt handler */
static void __attribute__((no_instrument_function))
NEXUS_Platform_P_Isr(unsigned long data)
{
    struct b_bare_interrupt_state *state = &b_bare_interrupt_state;
    unsigned long flags;

    BSTD_UNUSED(data);

    if(!state->started) {
        goto done;
    }

    /* Loop until all is cleared */
    for(;;) {
        uint32_t re_enable[NEXUS_NUM_L1_REGISTERS];
        uint32_t status;
        unsigned bit;
        unsigned i;

        /* Mask interrupts only to read current status */
        spin_lock_irqsave(&state->lock, flags);

        for(status=0,i=0;i<NEXUS_NUM_L1_REGISTERS;i++) {
            /* swap pending and current interrupt, then clear pending  and reenable interrupts */
            status |= state->pending[i].IntrStatus;
            /* coverity[use] */
            state->processing[i].IntrStatus = state->pending[i].IntrStatus;
            /* mask interrupts */
            state->processing[i].IntrStatus &= ~state->processing[i].IntrMaskStatus;

            /* Delay reenabling interrupts until after they have been serviced */
            /* disable_irq nests, so if the interrupt handler disables the interrupt */
            /* again, this is still safe. */
            re_enable[i] = state->processing[i].IntrStatus & state->pending[i].IntrStatus;

            /* clear list of delayed interrupts */
            state->pending[i].IntrStatus = 0;
        }

        /* Restore interrupts */
        spin_unlock_irqrestore(&state->lock, flags);

        if(status==0) {
            goto done;
        }


        /* then call L1 handlers inside critical section (KNI serializes with tasklet_disable so we do nothing here) */
        for(bit=0; bit<NUM_IRQS ; bit+=32) {

            status = state->processing[bit/32].IntrStatus;

            if(!status) {
                continue;
            }
            for(i=0;i<32;i++) {
                if(status & (1<<i)) {
                    unsigned irq = i+bit;
                    /* print on runaway L1 */
                    if (++state->table[irq].count % 10000 == 0) {
                        if (!state->table[irq].print && !nexus_driver_state.uninit_pending) {
                            printk("<0>### %s (W%d, bit %d) fired %d times\n", state->table[irq].name, irq/32, irq%32, state->table[irq].count);
                            state->table[irq].print = true; /* only print once to maximize chance that system keeps running */
                        }
                    }
                    
                    state->table[irq].handler(state->table[irq].context_ptr, state->table[irq].context_int);
                }
            }
        }

        /* Now, restore any disabled interrupts (masking not required) */
        for(bit=0; bit<NUM_IRQS ; bit+=32) {
            status = re_enable[bit/32];
            if (!status)  {
                continue;
            }
            for(i=0;i<32;i++)
            {
                /* only enable interrupts which are not masked by the software */
                if (status & (1<<i))
                {
                    BDBG_MSG_IRQ(("BH enable[irq] %d", LINUX_IRQ(i+bit)));
                    if (!state->table[i+bit].special_handler)
                    {
                        if ( state->table[i+bit].enabled ) {
                            BDBG_ASSERT(!state->table[i+bit].taskletEnabled); 
                            state->table[i+bit].taskletEnabled = true;
                            enable_irq(LINUX_IRQ(i+bit));
                        }
                    }
                }
            }
        }
    }


done:
    return;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
static irqreturn_t __attribute__((no_instrument_function))
NEXUS_Platform_P_LinuxIsr(int linux_irq, void *dev_id)
#else
static irqreturn_t __attribute__((no_instrument_function))
NEXUS_Platform_P_LinuxIsr(int linux_irq, void *dev_id, struct pt_regs *regs)
#endif
{
    struct b_bare_interrupt_entry *entry = dev_id;
    unsigned irq = NEXUS_IRQ(linux_irq);
    struct b_bare_interrupt_state *state = &b_bare_interrupt_state;
    unsigned i;
    unsigned long flags;

    if (irq >= NUM_IRQS) {
        goto error;
    }

    /* Make sure we're serialized with the tasklet across multiple CPUs */
    spin_lock_irqsave(&state->lock, flags);

    /* disable irq */
    BDBG_MSG_IRQ(("TH disable[irq] %d", linux_irq));
    if ( !entry->special_handler ) {
        if ( entry->taskletEnabled ) { 
            entry->taskletEnabled = false;
            disable_irq_nosync(linux_irq);
        }
    } else {
        entry->special_handler(irq);
    }

    for(i=0;i<NEXUS_NUM_L1_REGISTERS;i++,irq-=32) {
        if(irq<32) {
            state->pending[i].IntrStatus |= 1<<irq;
            break;
        }
    }

    /* This needs to run as a high-priority tasklet, which will immediately follow */
    tasklet_hi_schedule(&NEXUS_Platform_P_IsrTasklet);

    spin_unlock_irqrestore(&state->lock, flags);

    return IRQ_HANDLED;
    
error:
    BDBG_ASSERT(0); /* fail hard on unknown irq */
#if 0
    BDBG_WRN(("unknown irq %d", linux_irq));
    disable_irq_nosync(linux_irq);
#endif
    return IRQ_HANDLED;
}

NEXUS_Error NEXUS_Platform_P_ConnectInterrupt(unsigned irqNum, 
    NEXUS_Core_InterruptFunction handler, void *context_ptr, int context_int)
{
    struct NEXUS_Platform_P_IrqMap *nexusMap = NEXUS_Platform_P_IrqMap;
    const char *name=NULL;
    bool shared;
    b_bare_os_special_interrupt_handler special_handler;
    struct b_bare_interrupt_state *state = &b_bare_interrupt_state;
    struct b_bare_interrupt_entry *entry;
    unsigned long flags;
    unsigned i;

    if (irqNum>=NUM_IRQS || handler==NULL || !state->started) {
        return BERR_TRACE(-1);
    }
    
    /* search the nexus map first */    
    for (i=0;nexusMap[i].name;i++) {
        if (nexusMap[i].linux_irq == irqNum+1) {
            name = nexusMap[i].name;
            shared = nexusMap[i].shared;
            special_handler = nexusMap[i].special_handler;
            break;
        }
    }
    if (name==NULL) {
        const BINT_P_IntMap *intMap;
        /* use BINT's record of managed L2's (and their corresponding L1's) to validate the L1 connect */
        intMap = g_pCoreHandles->bint_map;
        BDBG_ASSERT(intMap);
        
        /* find the first L2 that has this L1 */
        for (i=0;intMap[i].L1Shift!=-1;i++) {
            if (BINT_MAP_GET_L1SHIFT(&intMap[i]) == irqNum) {
                name = intMap[i].L2Name; /* use BINT's L2 name for the L1 name. in most cases it is a meaningful name. */
                shared = true;
                special_handler = NULL;
                break;
            }
        }
        if (intMap[i].L1Shift == -1) {
            return BERR_TRACE(NEXUS_NOT_AVAILABLE);
        }
    }
    
    entry = &state->table[irqNum];
    if (entry->handler) {
        /* can't overwrite old handler, b_bare_disconnect_interrupt shall be called first */
        return BERR_TRACE(-1);
    }
    
    spin_lock_irqsave(&state->lock, flags);
    entry->name = name;
    entry->handler = handler;
    entry->context_ptr = context_ptr;
    entry->context_int = context_int;
    entry->special_handler = special_handler;
    entry->shared = shared;
    BDBG_ASSERT(!entry->enabled);
    BDBG_ASSERT(!entry->taskletEnabled);
    BDBG_ASSERT(!entry->requested);
    /* request_irq deferred to first enable. */
    spin_unlock_irqrestore(&state->lock, flags);
    
    return NEXUS_SUCCESS;
}

void NEXUS_Platform_P_MonitorOS(void)
{
    struct b_bare_interrupt_state *state = &b_bare_interrupt_state;
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
    
NEXUS_Error NEXUS_Platform_P_EnableInterrupt_isr( unsigned irqNum)
{
    struct b_bare_interrupt_state *state = &b_bare_interrupt_state;
    struct b_bare_interrupt_entry *entry;
    unsigned reg = irqNum/32;
    unsigned flags;

    if (irqNum>=NUM_IRQS || !state->started) {
        return BERR_TRACE(-1);
    }
    entry = &state->table[irqNum];
    if (!entry->handler) {
        return BERR_TRACE(-1);
    }
    
    spin_lock_irqsave(&state->lock, flags);
    state->processing[reg].IntrMaskStatus &= ~(1 << (irqNum%32));
    if (!entry->requested) {
        int irqflags;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
        irqflags = 0; /* later versions of linux always enable interrupts on request_irq */
        if (entry->shared) {
            irqflags |= IRQF_SHARED;
        }
#else
        irqflags = SA_INTERRUPT;
        if (entry->shared) {
            irqflags |= SA_SHIRQ;
        }
#endif    
    
        spin_unlock_irqrestore(&state->lock, flags);
        BDBG_MSG(("connect interrupt %s %d (%d, %p)", entry->name, LINUX_IRQ(irqNum), entry->shared, entry->special_handler));
        entry->taskletEnabled = true; /* set before call request_irq as irq can immediately fire */
        if (request_irq(LINUX_IRQ(irqNum), NEXUS_Platform_P_LinuxIsr, irqflags, entry->name, entry)) {
            /* disable */
            spin_lock_irqsave(&state->lock, flags);
            entry->handler = NULL;
            state->processing[reg].IntrMaskStatus |= (1 << (irqNum%32));
            spin_unlock_irqrestore(&state->lock, flags);
            return BERR_TRACE(-1);
        }
        entry->requested = true;
        entry->enabled = true;
        return 0;
    }    
    else if (!entry->enabled) {
        BDBG_MSG(("enable interrupt %d", LINUX_IRQ(irqNum)));
        if (!entry->special_handler) {
            BDBG_ASSERT(!entry->taskletEnabled);

            if ( !entry->taskletEnabled ) {
                entry->taskletEnabled = true;
                enable_irq(LINUX_IRQ(irqNum));
            }
        }
        entry->enabled = true;
    }
    spin_unlock_irqrestore(&state->lock, flags);
    return 0;
}

void NEXUS_Platform_P_DisableInterrupt_isr( unsigned irqNum)
{
    struct b_bare_interrupt_state *state = &b_bare_interrupt_state;
    struct b_bare_interrupt_entry *entry;
    unsigned reg = irqNum/32;
    unsigned flags;

    if (irqNum>=NUM_IRQS) {
        return BERR_TRACE(-1);
    }
    entry = &state->table[irqNum];
    if (!entry->handler) {
        BERR_TRACE(-1);
        return;
    }
    
    if (entry->enabled) {
        BDBG_ASSERT(entry->requested);
        BDBG_MSG(("disable interrupt %d", LINUX_IRQ(irqNum)));
        spin_lock_irqsave(&state->lock, flags);
        state->processing[reg].IntrMaskStatus |= (1 << (irqNum%32));
        if (!entry->special_handler) {
            /* If the TH has received the interrupt but it has not been processed by the tasklet, don't nest the disable call. */
            if ( entry->taskletEnabled ) 
            {
                disable_irq_nosync(LINUX_IRQ(irqNum));
                entry->taskletEnabled = false;
            }
        }
        entry->enabled = false;
        spin_unlock_irqrestore(&state->lock, flags);
    }
}

void NEXUS_Platform_P_DisconnectInterrupt( unsigned irqNum)
{
    int rc;
    struct b_bare_interrupt_state *state = &b_bare_interrupt_state;
    struct b_bare_interrupt_entry *entry;
    unsigned reg = irqNum/32;
    unsigned long flags;

    if(irqNum>=NUM_IRQS) {
        rc = BERR_TRACE(-1);
        return;
    }
    entry = &state->table[irqNum];
    if (!entry->handler) {
        BERR_TRACE(-1);
        return;
    }
    
    spin_lock_irqsave(&state->lock, flags);
    BDBG_MSG(("disconnect interrupt %d", LINUX_IRQ(irqNum)));
    entry->handler = NULL;
    if (entry->enabled) {
        state->processing[reg].IntrMaskStatus |= (1 << (irqNum%32));
        entry->enabled = false;
    }
    if (entry->requested) {
        entry->requested = false;
        entry->taskletEnabled = false;
        spin_unlock_irqrestore(&state->lock, flags);
        /* kernel can sleep in free_irq, so must release the spinlock first */
        free_irq(LINUX_IRQ(irqNum), entry);
    }
    else {
        spin_unlock_irqrestore(&state->lock, flags);
    }
}

NEXUS_Error
NEXUS_Platform_P_EnableInterrupt( unsigned irqNum)
{
    NEXUS_Error rc;

    BKNI_EnterCriticalSection();
    rc = NEXUS_Platform_P_EnableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();

    return rc;
}

void
NEXUS_Platform_P_DisableInterrupt( unsigned irqNum)
{
    BKNI_EnterCriticalSection();
    NEXUS_Platform_P_DisableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();
    return;
}

void NEXUS_Platform_P_AtomicUpdateCallback_isr(void *callbackContext, uint32_t reg, uint32_t mask, uint32_t value)
{
    uint32_t temp;
    unsigned long flags;

    BSTD_UNUSED(callbackContext);

    /* this spinlock synchronizes with any kernel use of a set of shared registers.
    see BREG_P_CheckAtomicRegister in magnum/basemodules/reg/breg_mem.c for the list of registers. */
    spin_lock_irqsave(&brcm_magnum_spinlock, flags);

    /* read/modify/write */
    temp = BREG_Read32(g_pCoreHandles->reg, reg);
    temp &= ~mask;
    temp |= value;
    BREG_Write32(g_pCoreHandles->reg, reg, temp);

    spin_unlock_irqrestore(&brcm_magnum_spinlock, flags);
}


NEXUS_Error
NEXUS_Platform_P_Magnum_Init(void)
{
    /* nothing to do here, magnum already initialized when driver loaded */
    return BERR_SUCCESS;
}

void
NEXUS_Platform_P_Magnum_Uninit(void)
{
    return;
}

unsigned long
copy_to_user_small(void * to, const void * from, unsigned long n)
{
    return copy_to_user(to, from, n);
}

unsigned long
copy_from_user_small(void * to, const void * from, unsigned long n)
{
    return copy_from_user(to, from, n);
}

void NEXUS_Platform_P_TerminateProcess(unsigned id)
{
#if 1
    int rc;
    char pidstr[16];
    char *argv[] = {"/bin/kill", "-9", pidstr, NULL};
    snprintf(pidstr, 16, "%u", id);
    rc = call_usermodehelper("/bin/kill", argv, NULL, UMH_WAIT_PROC);
    if (rc) BERR_TRACE(rc);
#else
    force_sig(SIGKILL, (struct task_struct *)id);
#endif
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30)
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,3,0)
	#include "asm/brcmstb/brcmstb.h"
#endif
/***************************************************************************
Summary:
Read reserved memory
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadReserved(
    uint32_t physicalAddress
    )
{
    volatile uint32_t *pMem;
    BDBG_ASSERT(0 == (physicalAddress&0x3));

    pMem = (volatile uint32_t *)phys_to_virt(physicalAddress);
    return *pMem;
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
    volatile uint32_t *pMem;
    BDBG_ASSERT(0 == (physicalAddress&0x3));

    pMem = (volatile uint32_t *)phys_to_virt(physicalAddress);
    *pMem = value;
    dma_cache_wback_inv((uint32_t)pMem, 4);
}

#ifdef CONFIG_BMIPS4380
/***************************************************************************
Summary:
Read core register
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadCoreReg(
    uint32_t offset
    )
{
    volatile uint32_t *pMem;
    BDBG_ASSERT(0 == (offset&0x3));

    pMem = (volatile uint32_t *)(BMIPS_GET_CBR() + offset);
    return *pMem;
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
    volatile uint32_t *pMem;
    BDBG_ASSERT(0 == (offset&0x3));

    pMem = (volatile uint32_t *)(BMIPS_GET_CBR() + offset);
    *pMem = value;
}

/***************************************************************************
Summary:
Read CMT Control Register
***************************************************************************/
uint32_t NEXUS_Platform_P_ReadCmtControl(void)
{
    uint32_t value = read_c0_brcm_cmt_ctrl();
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
    write_c0_brcm_cmt_ctrl(value);
}
#endif  /* CONFIG_BMIPS4380 */
#endif  /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30) */


void NEXUS_Platform_P_StopCallbacks(void *interfaceHandle)
{
    NEXUS_Base_P_StopCallbacks(interfaceHandle);
    NEXUS_P_Proxy_StopCallbacks(interfaceHandle);
    return;
}

void NEXUS_Platform_P_StartCallbacks(void *interfaceHandle)
{
    NEXUS_Base_P_StartCallbacks(interfaceHandle);
    NEXUS_P_Proxy_StartCallbacks(interfaceHandle);
    return;
}
