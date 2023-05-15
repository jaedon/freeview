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
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/5/12 6:07p $
 *
 * Module Description: Application to generate RULs for splash screen. 
 *                     This is a slightly modified copy of vdc_dump.c
 *                     ported to Nucleus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/97360/bsplash_board.h $
 * 
 * 1   10/5/12 6:07p bselva
 * SW7360-38: Add splash support for 7360 platform
 * 
 * 1   10/5/12 6:06p bselva
 * SW7360-38: Add splash support for 7360 platform
 * 
 * 1   10/5/12 6:04p bselva
 * SW7360-38: Add splash support for 7360 platform
 * 
 * 1   9/27/11 7:23p bselva
 * SW7358-147: Added the support for 7358 platform
 * 
 * 1   7/12/11 10:59a jianweiz
 * SW7552-48: Add 97552 splash support
 * 
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
#include "bchp_memc_gen_0.h"
#include "bchp_rdc.h"


/* new vdc pull the surface adddress from the RDC scratch registers. This information can be found from the VDC team.
   Will request VDC team for an API to determine the scratch adddress location until that happens hardcoding it here */
#define BSPLASH_RDC_SCRATCH_DISP0_REG0  (BCHP_RDC_scratch_i_ARRAY_BASE + 4*42)
#define BSPLASH_RDC_SCRATCH_DISP0_REG1  (BCHP_RDC_scratch_i_ARRAY_BASE + 4*43)
#define BSPLASH_RDC_SCRATCH_DISP1_REG0  (BCHP_RDC_scratch_i_ARRAY_BASE + 4*48)
#define BSPLASH_RDC_SCRATCH_DISP1_REG1  (BCHP_RDC_scratch_i_ARRAY_BASE + 4*49)

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
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RAAGA_DSP_INTH_REG_START, BCHP_RAAGA_DSP_INTH_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HDMI_TX_INTR2_REG_START, BCHP_HDMI_TX_INTR2_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_VIDEO_ENC_INTR2_REG_START, BCHP_VIDEO_ENC_INTR2_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_GEN_0_ARC_0_CNTRL, BCHP_MEMC_GEN_0_ARC_3_VIOLATION_INFO_CMD) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_EBI_CS_BASE_0, BCHP_EBI_ECR) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_TIMER_TIMER_IS, BCHP_TIMER_WDCTRL) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_GIO_ODEN_LO, BCHP_GIO_STAT_EXT) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_IRQ0_IRQEN, BCHP_IRQ0_IRQEN) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SCIRQ0_SCIRQEN, BCHP_SCIRQ0_SCIRQEN) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_TIMER_TIMER_IE0, BCHP_TIMER_TIMER_IE0) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AIO_MISC_REG_START, BCHP_RAAGA_DSP_SEC0_REG_END)   || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSP_CMDBUF_REG_START, BCHP_BSP_INST_PATCHRAM_REG_END)   || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RDC_REG_START, BCHP_RDC_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_L2_1_0_REG_START, BCHP_MEMC_L2_1_0_REG_END) \
        )

/* Removed  BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSCA_CHIP_ADDRESS, BCHP_BSCD_SCL_PARAM) || \ */
#define SPLASH_NUM_SVIDEO_OUTPUTS   0
#define SPLASH_NUM_COMPOSITE_OUTPUTS    1
#define SPLASH_NUM_COMPONENT_OUTPUTS    1

#define BRCM_DAC_SVIDEO_LUMA      BVDC_Dac_5
#define BRCM_DAC_SVIDEO_CHROMA    BVDC_Dac_6
#define BRCM_DAC_COMPOSITE_0      BVDC_Dac_0

#define BRCM_DAC_Y                BVDC_Dac_2
#define BRCM_DAC_PR               BVDC_Dac_3
#define BRCM_DAC_PB               BVDC_Dac_1

#define BRCM_DAC_COMPOSITE_1      BVDC_Dac_3

#define CFG_SPLASH_PAL

/* The default display for this board */
#ifdef CFG_SPLASH_PAL
#define SPLASH_CMP_0_PATH_DISPLAY   BFMT_VideoFmt_e720p_50Hz   
#define SPLASH_CMP_1_PATH_DISPLAY   BFMT_VideoFmt_ePAL_I
#else
#define SPLASH_CMP_0_PATH_DISPLAY   BFMT_VideoFmt_e720p 
#define SPLASH_CMP_1_PATH_DISPLAY   BFMT_VideoFmt_eNTSC
#endif

#define SPLASH_SURFACE_PXL_TYPE BPXL_eR5_G6_B5  /* BPXL_eA8_R8_G8_B8*/

#define B_I2C_CHANNEL_HDMI    0

/* End of File */



