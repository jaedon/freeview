/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7043.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/4/09 7:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7043/bint_7043.c $
 * 
 * Hydra_Software_Devel/2   2/4/09 7:26p shyam
 * PR48138 : Add interrupt support for BSP interrupts
 * 
 * Hydra_Software_Devel/1   9/8/08 7:56p shyam
 * PR40732 : Add PCI interrupt support for 7043
 *
 ***************************************************************************/
#include "bstd.h"
#include "bint_7043.h"

/* Include interrupt definitions from RDB */
#include "bchp_hif_cpu_intr1.h"
#include "bchp_enc_hif_intr1.h"

/* Standard L2 stuff */
#include "bchp_enc_mnm_intr2.h"
#include "bchp_enc_bsp_control_intr2.h"

BDBG_MODULE(interruptinterface_7043);

#define BINT_P_STD_STATUS       0x00
#define BINT_P_STD_SET          0x04
#define BINT_P_STD_CLEAR        0x08
#define BINT_P_STD_MASK_STATUS  0x0C
#define BINT_P_STD_MASK_SET     0x10
#define BINT_P_STD_MASK_CLEAR   0x14


#define BINT_P_STANDARD_L2_CASES \
    case BCHP_ENC_MNM_INTR2_PCI_STATUS:  \
    case BCHP_ENC_BSP_CONTROL_INTR2_PCI_STATUS: 


#define BINT_P_STAT_TIMER_TICKS_PER_USEC 27

static void BINT_P_7043_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7043_ClearInt_isr( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7043_SetMask_isr( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7043_ClearMask_isr( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_7043_ReadMask_isr( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_7043_ReadStatus_isr( BREG_Handle regHandle, uint32_t baseAddr );

static const BINT_P_IntMap bint_7043[] =
{
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_0_CPU_INTR_SHIFT + 32,         BCHP_ENC_MNM_INTR2_PCI_STATUS,                    0,          "MNM"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_0_CPU_INTR_SHIFT + 32,         BCHP_ENC_BSP_CONTROL_INTR2_PCI_STATUS,     0,          "BSP"},
    { -1, 0, 0, NULL}
};

static const BINT_Settings bint_7043Settings =
{
    BINT_P_7043_SetInt,
    BINT_P_7043_ClearInt_isr,
    BINT_P_7043_SetMask_isr,
    BINT_P_7043_ClearMask_isr,
    BINT_P_7043_ReadMask_isr,
    BINT_P_7043_ReadStatus_isr,
    bint_7043,
    "7043"
};

static void BINT_P_7043_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    switch( baseAddr )
    {
    BINT_P_STANDARD_L2_CASES
        BREG_Write32( regHandle, baseAddr + BINT_P_STD_SET, 1ul<<shift);
        break;
    default:
        /* Only standard L2 interrupts support setting of interrupts */
        break;
    }
}

static void BINT_P_7043_ClearInt_isr( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BDBG_MSG(("ClearInt %#x:%d", baseAddr, shift));
    switch( baseAddr )
    {
        BINT_P_STANDARD_L2_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_STD_CLEAR, 1ul<<shift);
            break;
        default:
            /* Other types of interrupts do not support clearing of interrupts (condition must be cleared) */
            break;
    }
}

static void BINT_P_7043_SetMask_isr( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BDBG_MSG(("SetMask %#x:%d", baseAddr, shift));

    switch( baseAddr )
    {
    BINT_P_STANDARD_L2_CASES
        BREG_Write32( regHandle, baseAddr + BINT_P_STD_MASK_SET, 1ul<<shift);
        break;
    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        break;
    }
}

static void BINT_P_7043_ClearMask_isr( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BDBG_MSG(("ClearMask %#x:%d", baseAddr, shift));
    BREG_Write32( regHandle, BCHP_ENC_HIF_INTR1_INTR_W0_MASK_CLEAR, 
		BCHP_FIELD_DATA(ENC_HIF_INTR1_INTR_W0_MASK_CLEAR, MNM_INTR, 1) ) ;
    BREG_Write32( regHandle, BCHP_ENC_HIF_INTR1_INTR_W0_MASK_CLEAR, 
		BCHP_FIELD_DATA(ENC_HIF_INTR1_INTR_W0_MASK_CLEAR, BSP_INTR, 1) ) ;
    switch( baseAddr )
    {
    BINT_P_STANDARD_L2_CASES
        BDBG_MSG(("Wrote reg addr %#x:%d", baseAddr + BINT_P_STD_MASK_CLEAR, 1ul<<shift));
        BREG_Write32( regHandle, baseAddr + BINT_P_STD_MASK_CLEAR, 1ul<<shift);
        BDBG_MSG(("Mask Status = %#08x", BREG_Read32(regHandle, baseAddr + BINT_P_STD_MASK_STATUS)  ));
        break;
    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        break;
    }
}

static uint32_t BINT_P_7043_ReadMask_isr( BREG_Handle regHandle, uint32_t baseAddr )
{
    BDBG_MSG(("ReadMask %#x", baseAddr));
    switch( baseAddr )
    {
    BINT_P_STANDARD_L2_CASES
        return BREG_Read32(regHandle, baseAddr + BINT_P_STD_MASK_STATUS);
    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        return 0;
    }
}

static uint32_t BINT_P_7043_ReadStatus_isr( BREG_Handle regHandle, uint32_t baseAddr )
{
    BDBG_MSG(("ReadStatus %#x", baseAddr));
    switch( baseAddr )
    {
    BINT_P_STANDARD_L2_CASES
        return BREG_Read32(regHandle, baseAddr + BINT_P_STD_STATUS);
    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        return 0;
    }
}

const BINT_Settings *BINT_7043_GetSettings( void )
{
    return &bint_7043Settings;
}

/* End of file */

