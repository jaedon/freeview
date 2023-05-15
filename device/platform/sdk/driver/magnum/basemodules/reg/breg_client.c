/***************************************************************************
 *	   Copyright (c) 2003-2010, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_client.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 9/2/10 5:49p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_client.c $
 * 
 * Hydra_Software_Devel/5   9/2/10 5:49p hongtaoz
 * SW7425-10: SUN_TOP_CTRL_SW_RESET register is renamed;
 * 
 * Hydra_Software_Devel/4   6/5/09 2:49p jessem
 * PR 54881: Modified BREG_SetAtomicUpdate32Callback to handle null
 * contexts.
 *
 * Hydra_Software_Devel/3   4/28/09 3:05p jessem
 * PR 54380: Changed all IKOS references to EMU instead.
 *
 * Hydra_Software_Devel/2   11/26/08 11:47a jessem
 * PR 49655: Added register atomic update support.
 *
 * Hydra_Software_Devel/1   10/22/07 4:38p jessem
 * PR 36129: Supercedes breg_sims.c for use with IKOS emulation platform.
 *
 ***************************************************************************/
#include "bstd.h"

#include "bkni.h"

#include "breg_mem.h"
#include "client.h"

BDBG_MODULE(BREG_CLIENT);


void BREG_Open( BREG_Handle *pRegHandle, void *Address, size_t MaxRegOffset )
{
	*pRegHandle = (BREG_Handle)BKNI_Malloc( sizeof(BREG_Impl) );
	BDBG_ASSERT(*pRegHandle != NULL );

#if BDBG_DEBUG_BUILD
	(*pRegHandle)->MaxRegOffset = MaxRegOffset;
#else
	BSTD_UNUSED(MaxRegOffset);
#endif
	(*pRegHandle)->BaseAddr = (uintptr_t)Address;

    /* set default callback */
    BREG_SetAtomicUpdate32Callback( *pRegHandle, NULL, NULL );

}

void BREG_Close( BREG_Handle RegHandle )
{
	BDBG_ASSERT(RegHandle != NULL );
	BKNI_Free(RegHandle);
}

/* compile the register access functions even for the release build */
#undef	BREG_Write32
#undef	BREG_Write16
#undef	BREG_Write8

#undef	BREG_Read32
#undef	BREG_Read16
#undef	BREG_Read8


uint32_t BREG_Read32(BREG_Handle RegHandle, uint32_t reg)
{
	uint32_t data ;

	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	BEMU_Client_ReadRegister(reg, &data);
	return data ;
}

uint16_t BREG_Read16(BREG_Handle RegHandle, uint32_t reg)
{
	uint32_t data ;

	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	BEMU_Client_ReadRegister(reg, &data);
	return (data & 0xffff) ;
}

uint8_t BREG_Read8(BREG_Handle RegHandle, uint32_t reg)
{
	uint32_t data ;

	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	BEMU_Client_ReadRegister(reg, &data);
	return (data & 0xff) ;
}

void BREG_Write32(BREG_Handle RegHandle, uint32_t reg, uint32_t data)
{
	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	BEMU_Client_WriteRegister(reg, data) ;
}

void BREG_Write16(BREG_Handle RegHandle, uint32_t reg, uint16_t data)
{
	BSTD_UNUSED(data);
	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	BDBG_ASSERT(0);
}

void BREG_Write8(BREG_Handle RegHandle, uint32_t reg, uint8_t data)
{
	BSTD_UNUSED(data);
	BDBG_ASSERT(reg < RegHandle->MaxRegOffset);
	BDBG_ASSERT(0);
}


static void BREG_P_AtomicUpdate32_Impl_isr(void *context, uint32_t addr, uint32_t mask, uint32_t value)
{
	uint32_t temp;

#if 0
    addr = ((BREG_Handle)context)->BaseAddr + addr;
    temp = *(volatile uint32_t *)addr;
    temp = (temp&~mask)|value;
    *(volatile uint32_t *)addr = temp;
#else
/*    addr = ((BREG_Handle)context)->BaseAddr + addr;*/
	temp = BREG_Read32((BREG_Handle)context, addr);
    temp = (temp&~mask)|value;
	BREG_Write32((BREG_Handle)context, addr, temp);
#endif
    return;
}

#if BDBG_DEBUG_BUILD
#include "bchp_sun_top_ctrl.h"

#define BREG_P_ATOMIC_REG(reg) case reg: name=#reg;regAtomic=true;break
static void BREG_P_CheckAtomicRegister(uint32_t reg, const char *function, bool atomic )
{
    const char *name;
    bool regAtomic;
    switch(reg) {
#ifdef BCHP_SUN_TOP_CTRL_SW_RESET		
    BREG_P_ATOMIC_REG(BCHP_SUN_TOP_CTRL_SW_RESET);
#else
    BREG_P_ATOMIC_REG(BCHP_SUN_TOP_CTRL_SW_INIT_0_SET);
#endif
#if (BCHP_CHIP==7405)
#include "bchp_decode_sd_0.h"
#include "bchp_decode_ip_shim_0.h"
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_IP_SHIM_0_PFRI_REG);
#if BCHP_VER >= BCHP_VER_B0
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH);
#endif
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
#elif (BCHP_CHIP==7325 || BCHP_CHIP==7335)
#include "bchp_decode_sd_0.h"
#include "bchp_decode_ip_shim_0.h"
    BREG_P_ATOMIC_REG(BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH);
    BREG_P_ATOMIC_REG(BCHP_DECODE_IP_SHIM_0_PFRI_REG);
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


void BREG_SetAtomicUpdate32Callback( BREG_Handle RegHandle, BREG_AtomicUpdate32Callback callback_isr, void *callbackContext )
{
    BKNI_EnterCriticalSection();
    if (callback_isr)
	{
        RegHandle->atomicUpdate32.callback_isr = callback_isr;
		if (callbackContext)
		{
			RegHandle->atomicUpdate32.callbackContext = callbackContext;
		}
		else
		{
        	RegHandle->atomicUpdate32.callbackContext = RegHandle;
		}
    }
    else
	{
        /* restore default */
        RegHandle->atomicUpdate32.callback_isr = BREG_P_AtomicUpdate32_Impl_isr;
        RegHandle->atomicUpdate32.callbackContext = RegHandle;
    }

    BKNI_LeaveCriticalSection();
}

/* End of File */

