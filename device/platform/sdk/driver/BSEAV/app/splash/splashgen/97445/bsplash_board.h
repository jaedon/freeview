/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsplash_board.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/24/12 2:34p $
 *
 * Module Description: Application to generate RULs for splash screen.
 *                     This is a slightly modified copy of vdc_dump.c
 *                     ported to Nucleus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/97445/bsplash_board.h $
 * 
 * 2   9/24/12 2:34p katrep
 * SW7445-1: updated for 7445
 * 
 * 1   8/31/12 2:50p katrep
 * SW7445-1:add spalsh
 * 
 * 1   4/19/12 1:35p mward
 * SW7435-114:  Add support for 97435 platform(s).
 * 
 * 6   4/18/12 2:55p jessem
 * SW7425-2828: Removed display formats.
 *
 * 4   3/27/12 12:15p jessem
 * SW7425-2653: Updated RDC scratch regsiters.
 *
 * 3   11/11/11 10:43a jessem
 * SW7425-1527: Excluded BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15 and
 * BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0. Added register block names in
 * register dump to aid in debugging.
 *
 * 2   8/22/11 5:11p jessem
 * SW7425-878: Updated RDC registers used by splash.
 *
 * 1   8/8/11 11:07p nickh
 * SW7425-878: Add 7425
 *
 * 1   6/27/11 11:34a katrep
 * SW7231-220:add support for 7231 &newer 40nm chips
 *
 *
 *
 ***************************************************************************/

/* #include "bchp_sec_it.h"
##include "bchp_prim_it.h" */

/* For register exclusions */
#include "bchp_common.h"
/* #include "bchp_dtg.h" */
#include "bchp_irq0.h"
#include "bchp_timer.h"
#include "bchp_scirq0.h"
#include "bchp_ebi.h"
#include "bchp_bsca.h"
#include "bchp_bscd.h"
#include "bchp_gio.h"
#include "bchp_memc_gen_2_0.h"
#include "bchp_memc_gen_2_1.h"
#include "bchp_memc_gen_2_2.h"
#include "bchp_rdc.h"

/* These are the registers that need to be excluded from the register dump either
   because they interrupt the CPU or disturb settings done elsewhere like the CFE
   If you do not want to program certain registers - add them to this macro.
*/
#define BSPLASH_ADDRESS_IN_RANGE(addr, rangeLow, rangeHigh) ((addr>=rangeLow) && (addr <=rangeHigh))

#define BSPLASH_REGDUMP_EXCLUDE(addr)   ( \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNF_INTR2_0_REG_START, BCHP_BVNF_INTR2_5_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HIF_INTR2_REG_START, BCHP_HIF_CPU_INTR1_REG_END)   || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_XPT_BUS_IF_REG_START, BCHP_XPT_XPU_REG_END)   || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SHVD_INTR2_0_REG_START, BCHP_SHVD_INTR2_0_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SHVD_INTR2_1_REG_START, BCHP_SHVD_INTR2_1_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SHVD_INTR2_2_REG_START, BCHP_SHVD_INTR2_2_REG_END)   || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RAAGA_DSP_INTH_REG_START, BCHP_RAAGA_DSP_INTH_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_M2MC_L2_REG_START, BCHP_M2MC_L2_REG_END)  || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_M2MC1_L2_REG_START, BCHP_M2MC1_L2_REG_END)  || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HDMI_TX_INTR2_REG_START, BCHP_HDMI_TX_INTR2_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_VIDEO_ENC_INTR2_REG_START, BCHP_VIDEO_ENC_INTR2_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_GEN_2_0_ARC_0_CNTRL, BCHP_MEMC_GEN_2_0_ALIAS_VIOLATION_INFO_CMD) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_GEN_2_1_ARC_0_CNTRL, BCHP_MEMC_GEN_2_1_ALIAS_VIOLATION_INFO_CMD) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_GEN_2_2_ARC_0_CNTRL, BCHP_MEMC_GEN_2_2_ALIAS_VIOLATION_INFO_CMD) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_EBI_CS_BASE_0, BCHP_EBI_ECR) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_TIMER_TIMER_IS, BCHP_TIMER_WDCTRL) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_GIO_ODEN_LO, BCHP_GIO_STAT_EXT) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_IRQ0_IRQEN, BCHP_IRQ0_IRQEN) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SCIRQ0_SCIRQEN, BCHP_SCIRQ0_SCIRQEN) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_TIMER_TIMER_IE0, BCHP_TIMER_TIMER_IE0) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RAAGA_DSP_SEC0_REG_START, BCHP_RAAGA_DSP_MEM_SUBSYSTEM_1_REG_END)   || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSP_CMDBUF_REG_START, BCHP_XPT_SECURITY_NS_REG_END)   || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RDC_REG_START, BCHP_RDC_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_L2_1_0_REG_START, BCHP_MEMC_L2_1_0_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AVS_CPU_PROG_MEM_REG_START, BCHP_AVS_PMB_REGISTERS_REG_END)\
        )

/* Removed  BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSCA_CHIP_ADDRESS, BCHP_BSCD_SCL_PARAM) || \ */
#define SPLASH_NUM_SVIDEO_OUTPUTS   0
#define SPLASH_NUM_COMPOSITE_OUTPUTS    1
#define SPLASH_NUM_COMPONENT_OUTPUTS    1

#define BRCM_DAC_SVIDEO_LUMA      BVDC_Dac_5
#define BRCM_DAC_SVIDEO_CHROMA    BVDC_Dac_6
#define BRCM_DAC_COMPOSITE_0      BVDC_Dac_3

#define BRCM_DAC_Y                BVDC_Dac_0
#define BRCM_DAC_PR               BVDC_Dac_2
#define BRCM_DAC_PB               BVDC_Dac_1

#define SPLASH_SURFACE_PXL_TYPE BPXL_eR5_G6_B5 /* BPXL_eA8_R8_G8_B8 */

#define B_I2C_CHANNEL_HDMI    3

/* End of File */



