/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Revision: 2 $
* $brcm_Date: 6/6/11 3:43p $
*
* API Description:
*   API name: Platform linuxuser
*    linuxkernel OS routines
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97420/src/bare/nexus_platform_os.c $
* 
* 2   6/6/11 3:43p ttrammel
* SW7420-1819: Update NEXUS/PI to NFE 2.0.
* 
* 1   5/3/11 5:10p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
***************************************************************************/

#include "nexus_platform_priv.h"
#include "bkni.h"
#include "b_bare_os.h"
#include "nexus_interrupt_map.h"
#include "nexus_generic_driver_impl.h"

BDBG_MODULE(nexus_platform_os);

NEXUS_Error
NEXUS_Platform_P_InitOS(void)
{
    int rc;
    rc = nexus_driver_scheduler_init();
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error
NEXUS_Platform_P_UninitOS(void)
{
    nexus_driver_scheduler_uninit();
    return 0;
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

void *
NEXUS_Platform_P_MapMemory(unsigned long offset, unsigned long length, bool cached)
{
    return pb_bare_os->mmap(cached, offset, length);
}

void
NEXUS_Platform_P_UnmapMemory(void *pMem, unsigned long length)
{
    pb_bare_os->unmmap(pMem, length);
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

#define NEXUS_NUM_IRQS (32*NEXUS_NUM_L1_REGISTERS)

NEXUS_Error
NEXUS_Platform_P_ConnectInterrupt(unsigned irqNum, NEXUS_Core_InterruptFunction pIsrFunc, void *context_ptr, int context_int)
{
    struct NEXUS_Platform_P_IrqMap *map = NEXUS_Platform_P_IrqMap;
    if (irqNum >= NEXUS_NUM_IRQS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    return pb_bare_os->connect_interrupt(map[irqNum].name, irqNum, 
        pIsrFunc, context_ptr, context_int, 
        map[irqNum].shared, map[irqNum].special_handler);
    return 0;
}

void
NEXUS_Platform_P_DisconnectInterrupt( unsigned irqNum)
{
    if (irqNum >= NEXUS_NUM_IRQS) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    pb_bare_os->disconnect_interrupt(irqNum);
}

NEXUS_Error
NEXUS_Platform_P_EnableInterrupt_isr( unsigned irqNum)
{
    BKNI_ASSERT_ISR_CONTEXT();
    return pb_bare_os->enable_interrupt(irqNum);
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
NEXUS_Platform_P_DisableInterrupt_isr( unsigned irqNum)
{
    BKNI_ASSERT_ISR_CONTEXT();
    if (irqNum >= NEXUS_NUM_IRQS) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    pb_bare_os->disable_interrupt(irqNum);
}

void
NEXUS_Platform_P_DisableInterrupt( unsigned irqNum)
{
    BKNI_EnterCriticalSection();
    NEXUS_Platform_P_DisableInterrupt_isr(irqNum);
    BKNI_LeaveCriticalSection();
}

void NEXUS_Platform_P_AtomicUpdateCallback_isr(void *callbackContext, uint32_t reg, uint32_t mask, uint32_t value)
{
    BKNI_ASSERT_ISR_CONTEXT();
    pb_bare_os->atomic_update(reg, mask, value);
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
    return pb_bare_os->copy_to_process(to, from, n);
}

unsigned long
copy_from_user_small(void * to, const void * from, unsigned long n)
{
    return pb_bare_os->copy_from_process(to, from, n);
}

void NEXUS_Platform_P_TerminateProcess(unsigned pid)
{
    pb_bare_os->terminate_process(pid);
}
