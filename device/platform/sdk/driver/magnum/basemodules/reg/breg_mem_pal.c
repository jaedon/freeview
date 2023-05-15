/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_mem_pal.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 1/20/10 11:14a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_mem_pal.c $
 * 
 * Hydra_Software_Devel/1   1/20/10 11:14a kaiz
 * SW35230-56: adding linuxpal support for Palladium emulation environment
 * 
 * Hydra_Software_Devel/SW35230-56/1   1/18/10 6:29p kaiz
 * SW35230-56: adding linuxpal support for Palladium emulation environment
 * 
 * Hydra_Software_Devel/33   12/22/09 1:22p nickh
 * SW7420-70: Add more registers for atomic access
 * 
 * Hydra_Software_Devel/33   12/22/09 1:21p nickh
 * SW7420-70: Add more registers for atomic access
 * 
 * Hydra_Software_Devel/32   10/27/09 9:35a jrubio
 * SW7342-11: add support for 7342/7340 Atomic Registers
 * 
 * Hydra_Software_Devel/31   8/27/09 8:09p mward
 * SW7125-4: Specify atomic access registers for 7125.
 * 
 * Hydra_Software_Devel/30   8/16/09 6:30p nickh
 * PR47760: Use atomic access for 7420 Power management registers
 * 
 * Hydra_Software_Devel/29   7/30/09 5:49p jrubio
 * PR57226: add AVD into the register list for 7325
 * 
 * Hydra_Software_Devel/28   7/30/09 2:48p jrubio
 * PR56659: remove VCXO from 7325
 * 
 * Hydra_Software_Devel/27   7/30/09 12:17a pntruong
 * PR55856: Fixed 7325 build errors.
 *
 * Hydra_Software_Devel/26   7/29/09 2:38p jrubio
 * PR56659: CLCK_MISC is not protected
 *
 * Hydra_Software_Devel/25   7/9/09 5:51p jrubio
 * PR56659: include the necessary Registers for 7335 for AtomicUpdate
 *
 * Hydra_Software_Devel/24   7/8/09 4:50p jrubio
 * PR55856: fix 7325 includes
 *
 * Hydra_Software_Devel/23   7/6/09 11:42a jrubio
 * PR55856: add PM Register for 7335/7325
 *
 * Hydra_Software_Devel/22   7/1/09 5:18p katrep
 * PR56528: atomic access should be used for CLK_PM_CTRL and
 * SCXO_CTL_MISC_AVD registers
 *
 * Hydra_Software_Devel/21   6/9/09 1:53p mward
 * PR55856 :  Add BCHP_CLK_PM_CTRL registers to atomic reg access list.
 *
 * Hydra_Software_Devel/20   2/9/09 4:54p erickson
 * PR51629: add 7336 register
 *
 * Hydra_Software_Devel/19   2/4/09 5:22p tdo
 * PR51627: Add support for 7336 and 7420
 *
 * Hydra_Software_Devel/18   11/3/08 10:21a erickson
 * PR48615: add BCHP_VCXO_CTL_MISC_AVD_CTRL to AtomicUpdate list, remove
 * USB
 *
 * Hydra_Software_Devel/17   9/26/08 11:56a erickson
 * PR45703: fix warning
 *
 * Hydra_Software_Devel/16   9/24/08 7:32p vsilyaev
 * PR 46950: Use unified declaration of BREG handle for debug and release
 * builds
 *
 * Hydra_Software_Devel/15   9/23/08 3:43p erickson
 * PR46950: added BREG_SetAtomicUpdate32Callback
 *
 * Hydra_Software_Devel/14   7/30/08 4:05p vishk
 * PR45177: uintptr_t is now defined in linux 2.6.18-5.1 header files
 *
 * Hydra_Software_Devel/13   6/30/08 3:28p vsilyaev
 * PR 43119: Using atomic update functions for the 7325 platform
 *
 * Hydra_Software_Devel/12   6/12/08 1:00p vsilyaev
 * PR 43119: Added AVD registers for the 3548/3556 build
 *
 * Hydra_Software_Devel/11   6/7/08 8:22a pntruong
 * PR43119: Fixed build errors.
 *
 * Hydra_Software_Devel/10   6/5/08 5:28p vsilyaev
 * PR43119: PR 43119: Added API for atomic register access
 *
 * Hydra_Software_Devel/PR43119/1   5/29/08 12:14p vsilyaev
 * PR 43119: Added API for atomic register access
 *
 * Hydra_Software_Devel/9   9/14/06 2:44p jgarrett
 * PR 23982: Removing warning in release mode
 *
 * Hydra_Software_Devel/8   7/21/06 11:27a vsilyaev
 * PR 22695: Changes to make BREG_MEM compatible between debug and release
 * builds
 *
 * Hydra_Software_Devel/7   9/19/03 8:55a marcusk
 * Added volatile to typecasts to ensure compiler does not optimize
 * register reads and writes.
 *
 * Hydra_Software_Devel/6   9/15/03 6:08p marcusk
 * Updated to use void * as register base address.
 *
 * Hydra_Software_Devel/5   5/30/03 12:03p marcusk
 * Changed CreateRegHandle/DestroyRegHandle to Open/Close to be more
 * consistant.
 *
 * Hydra_Software_Devel/4   3/10/03 6:37p vsilyaev
 * Integrated with bstd.h .
 *
 * Hydra_Software_Devel/3   3/7/03 9:24a marcusk
 * Minor cleanup.
 *
 * Hydra_Software_Devel/2   3/5/03 4:19p marcusk
 * Fixed minor issues (got it to compile).
 *
 * Hydra_Software_Devel/1   3/5/03 3:33p marcusk
 * Initial version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "breg_mem.h"

BDBG_MODULE(breg_mem);


void BREG_Open( BREG_Handle *pRegHandle, void *Address, size_t MaxRegOffset )
{
    *pRegHandle = (BREG_Handle)BKNI_Malloc( sizeof(BREG_Impl) );
    BDBG_ASSERT(*pRegHandle != NULL );

#if BDBG_DEBUG_BUILD
    (*pRegHandle)->MaxRegOffset = MaxRegOffset;
#else
    BSTD_UNUSED(MaxRegOffset);
#endif
    (*pRegHandle)->BaseAddr = (uint32_t)((uintptr_t)Address);

    /* set default callback */
    BREG_SetAtomicUpdate32Callback( *pRegHandle, NULL, NULL );

}

void BREG_Close( BREG_Handle RegHandle )
{
    BDBG_ASSERT(RegHandle != NULL );
    BKNI_Free(RegHandle);
}

/* compile the register access functions even for the release build */
#undef  BREG_Write32
#undef  BREG_Write16
#undef  BREG_Write8

#undef  BREG_Read32
#undef  BREG_Read16
#undef  BREG_Read8

#if BDBG_DEBUG_BUILD
#include "bchp_sun_top_ctrl.h"

#define BREG_P_ATOMIC_REG(reg) case reg: name=#reg;regAtomic=true;break
static void BREG_P_CheckAtomicRegister(uint32_t reg, const char *function, bool atomic )
{
    const char *name;
    bool regAtomic;
    switch(reg) {
    BREG_P_ATOMIC_REG(BCHP_SUN_TOP_CTRL_SW_RESET);
#if (BCHP_CHIP==7405)
#include "bchp_decode_sd_0.h"
#include "bchp_decode_ip_shim_0.h"
#include "bchp_clk.h"
#include "bchp_vcxo_ctl_misc.h"
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_IP_SHIM_0_PFRI_REG);
    BREG_P_ATOMIC_REG(BCHP_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_PM_CTRL_1);
    BREG_P_ATOMIC_REG(BCHP_CLK_PM_CTRL_2);
    BREG_P_ATOMIC_REG(BCHP_VCXO_CTL_MISC_AVD_CTRL);
#elif (BCHP_CHIP==7400)
#include "bchp_clk.h"
    BREG_P_ATOMIC_REG(BCHP_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_PM_CTRL_1);
    BREG_P_ATOMIC_REG(BCHP_CLK_PM_CTRL_2);
#elif (BCHP_CHIP==3556 || BCHP_CHIP==3548)
#include "bchp_decode_sd_0.h"
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH);
#include "bchp_clkgen.h"
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_0);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_1);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_2);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_3);
#include "bchp_vcxo_ctl_misc.h"
    BREG_P_ATOMIC_REG(BCHP_VCXO_CTL_MISC_AVD_CTRL);
#elif ( BCHP_CHIP==7335 )
#include "bchp_clk.h"
#include "bchp_decode_sd_0.h"
#include "bchp_decode_ip_shim_0.h"
#include "bchp_vcxo_ctl_misc.h"
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_IP_SHIM_0_PFRI_REG);
    BREG_P_ATOMIC_REG(BCHP_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_PM_CTRL_1);
    BREG_P_ATOMIC_REG(BCHP_CLK_PM_CTRL_2);
    BREG_P_ATOMIC_REG(BCHP_VCXO_CTL_MISC_AVD_CTRL);
#elif    (BCHP_CHIP==7325 )
#include "bchp_clkgen.h"
#include "bchp_decode_sd_0.h"
#include "bchp_decode_ip_shim_0.h"
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_IP_SHIM_0_PFRI_REG);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_0);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_1);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_2);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_3);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_AVD_CTRL);
#elif    (BCHP_CHIP==7340 )
#include "bchp_clkgen.h"
#include "bchp_decode_sd_0.h"
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_0);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_216_108_0);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_216_108_1);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_MISC_CLOCKS);
#elif    (BCHP_CHIP==7342 )
#include "bchp_clk.h"
#include "bchp_decode_sd_0.h"
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH);
#elif    (BCHP_CHIP==7125 )
#include "bchp_clkgen.h"
#include "bchp_decode_sd_0.h"
#include "bchp_decode_ip_shim_0.h"
	BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_IP_SHIM_0_PFRI_REG);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_0);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_1);
    BREG_P_ATOMIC_REG(BCHP_CLKGEN_PWRDN_CTRL_2);
#elif (BCHP_CHIP==7420)
#include "bchp_decode_sd_0.h"
#include "bchp_decode_sd_1.h"
#include "bchp_clk.h"
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_1_REG_SD_PFRI_DATA_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_1_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_CLK_DVP_HT_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_UHFR_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_SUN_DAA_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_SUN_SM_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_RFM_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_TDAC_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_QDAC_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_RPT_AIO_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_VEC_656_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_VEC_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_GFX_2D_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_GFX_3D_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_BVN_EDGE_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_BVN_MCVP_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_BVN_MIDDLE_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_AVD0_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_AVD1_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_XPT_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_SECTOP_DMA_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_TOP1394_CLK_PM_CTRL);
    BREG_P_ATOMIC_REG(BCHP_CLK_MISC);
#endif
    default:
        name = "";
        regAtomic = false;
        break;
    }
    if(regAtomic!=atomic) {
        if(!atomic) {
            BDBG_ERR(("%s: register %s(%#lx) should only be used with atomic access", function, name, (unsigned long)reg));
        } else {
        BDBG_ERR(("%s: register %#lx shouldn't be used for atomic access", function, (unsigned long)reg));
        }
    }
}
#else
#define BREG_P_CheckAtomicRegister(reg, function, atomic)
#endif

uint32_t BREG_Read32(BREG_Handle RegHandle, uint32_t reg)
{
    BDBG_ASSERT(reg < RegHandle->MaxRegOffset);

    return *((volatile uint32_t *)((uintptr_t)(RegHandle->BaseAddr)+reg));
}

uint16_t BREG_Read16(BREG_Handle RegHandle, uint32_t reg)
{
    BDBG_ASSERT(reg < RegHandle->MaxRegOffset);

    return *((volatile uint16_t *)((uintptr_t)(RegHandle->BaseAddr)+reg));
}

uint8_t BREG_Read8(BREG_Handle RegHandle, uint32_t reg)
{
    BDBG_ASSERT(reg < RegHandle->MaxRegOffset);

    return *((volatile uint8_t *)((uintptr_t)(RegHandle->BaseAddr)+reg));
}

void BREG_Write32(BREG_Handle RegHandle, uint32_t reg, uint32_t data)
{
    BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
    BREG_P_CheckAtomicRegister(reg, "BREG_Write32", false);
    *((volatile uint32_t *)((uintptr_t)(RegHandle->BaseAddr)+reg)) = data;
}

void BREG_Write16(BREG_Handle RegHandle, uint32_t reg, uint16_t data)
{
    BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
    BREG_P_CheckAtomicRegister(reg, "BREG_Write16", false);

    *((volatile uint16_t *)((uintptr_t)(RegHandle->BaseAddr)+reg)) = data;
}

void BREG_Write8(BREG_Handle RegHandle, uint32_t reg, uint8_t data)
{
    BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
    BREG_P_CheckAtomicRegister(reg, "BREG_Write8", false);

    *((volatile uint8_t *)((uintptr_t)(RegHandle->BaseAddr)+reg)) = data;
}


static void BREG_P_AtomicUpdate32_Impl_isr(void *context, uint32_t addr, uint32_t mask, uint32_t value)
{
    uint32_t temp;
    addr = ((BREG_Handle)context)->BaseAddr + addr;
    temp = *(volatile uint32_t *)((uintptr_t)addr);
    temp = (temp&~mask)|value;
    *(volatile uint32_t *)((uintptr_t)addr) = temp;
    return;
}

void BREG_AtomicUpdate32_isr(BREG_Handle RegHandle, uint32_t reg, uint32_t mask, uint32_t value)
{
    BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
    BREG_P_CheckAtomicRegister(reg, "BREG_AtomicUpdate32_isr", true);
    (*RegHandle->atomicUpdate32.callback_isr)(RegHandle->atomicUpdate32.callbackContext, reg, mask, value);
}


void BREG_AtomicUpdate32(BREG_Handle RegHandle, uint32_t reg, uint32_t mask, uint32_t value )
{
    BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
    BREG_P_CheckAtomicRegister(reg, "BREG_AtomicUpdate32", true);
    BKNI_EnterCriticalSection();
    (*RegHandle->atomicUpdate32.callback_isr)(RegHandle->atomicUpdate32.callbackContext, reg, mask, value);
    BKNI_LeaveCriticalSection();
}

static uint32_t BREG_P_CompareAndSwap32_Impl_isr(uint32_t addr, uint32_t oldValue, uint32_t newValue)
{
    uint32_t result = *(volatile uint32_t *)((uintptr_t)addr);
    if(result == oldValue) {
        *(volatile uint32_t *)((uintptr_t)addr) = newValue;
    }
    return result;
}

uint32_t BREG_CompareAndSwap32(BREG_Handle RegHandle, uint32_t reg, uint32_t oldValue, uint32_t newValue )
{
    uint32_t result;
    BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
    BREG_P_CheckAtomicRegister(reg, "BREG_CompareAndSwap32", true);
    BKNI_EnterCriticalSection();
    result = BREG_P_CompareAndSwap32_Impl_isr(RegHandle->BaseAddr+reg, oldValue, newValue);
    BKNI_LeaveCriticalSection();
    return result;
}

uint32_t BREG_CompareAndSwap32_isr(BREG_Handle RegHandle, uint32_t reg, uint32_t oldValue, uint32_t newValue )
{
    uint32_t result;
    BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
    BREG_P_CheckAtomicRegister(reg, "BREG_CompareAndSwap32_isr", true);
    result = BREG_P_CompareAndSwap32_Impl_isr(RegHandle->BaseAddr+reg, oldValue, newValue);
    return result;
}

void BREG_SetAtomicUpdate32Callback( BREG_Handle RegHandle, BREG_AtomicUpdate32Callback callback_isr, void *callbackContext )
{
    BKNI_EnterCriticalSection();
    if (callback_isr) {
        RegHandle->atomicUpdate32.callback_isr = callback_isr;
        RegHandle->atomicUpdate32.callbackContext = callbackContext;
    }
    else {
        /* restore default */
        RegHandle->atomicUpdate32.callback_isr = BREG_P_AtomicUpdate32_Impl_isr;
        RegHandle->atomicUpdate32.callbackContext = RegHandle;
    }
    BKNI_LeaveCriticalSection();
}

/* End of File */



