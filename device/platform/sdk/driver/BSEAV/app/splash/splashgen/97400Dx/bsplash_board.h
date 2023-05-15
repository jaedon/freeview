/***************************************************************************
 *     Copyright (c) 2005-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsplash_board.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 5/7/10 2:28p $
 *
 * Module Description: Application to generate RULs for splash screen. 
 *                     This is a slightly modified copy of vdc_dump.c
 *                     ported to Nucleus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/97400Dx/bsplash_board.h $
 * 
 * 2   5/7/10 2:28p randyjew
 * SW7468-227:Add SPLASH_NUM_XXX_OUTPUTS defines to be compatible with
 * splash_setup_vdc.c
 * 
 * 1   11/2/07 1:04p mward
 * PR34874: Add 7400Dx
 * 
 * 2   9/7/07 4:45p shyam
 * PR 30741 : Added board level configuration of Default display
 * 
 * 2   6/29/07 1:42p shyam
 * PR 30741 : Fixed HDMI problem
 * 
 * 1   5/14/07 7:00p shyam
 * PR 30741 : Add reference support for generic portable splash
 *
 ***************************************************************************/

#include "bchp_sec_it.h"
#include "bchp_prim_it.h"

/* For register exclusions */
#include "bchp_common.h"
#include "bchp_dtg.h"
#include "bchp_irq0.h"
#include "bchp_timer.h"
#include "bchp_scirq0.h"
#include "bchp_ebi.h"
#include "bchp_bsca.h"
#include "bchp_bscd.h"
#include "bchp_gio.h"
#include "bchp_memc_0.h"

/* These are the registers that need to be excluded from the register dump either
   because they interrupt the CPU or disturb settings done elsewhere like the CFE 
   If you do not want to program certain registers - add them to this macro. 
*/
#define BSPLASH_ADDRESS_IN_RANGE(addr, rangeLow, rangeHigh) ((addr>=rangeLow) && (addr <=rangeHigh))

#define BSPLASH_REGDUMP_EXCLUDE(addr)	( \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNF_INTR2_0_REG_START, BCHP_BVNF_INTR2_6_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RFM_L2_REG_START, BCHP_RFM_L2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HIF_INTR2_REG_START, BCHP_HIF_CPU_INTR1_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_XPT_BUS_IF_REG_START, BCHP_XPT_GR_REG_END)   || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AVD_INTR2_0_REG_START, BCHP_AVD_INTR2_0_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AVD_INTR2_1_REG_START, BCHP_AVD_INTR2_1_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AUD_DSP_INTH0_REG_START, BCHP_AUD_DSP_INTH0_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC16_GFX_L2_REG_START, BCHP_MEMC16_GFX_L2_REG_END)  || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HDMI_INTR2_REG_START, BCHP_HDMI_INTR2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_VIDEO_ENC_INTR2_REG_START, BCHP_VIDEO_ENC_INTR2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_0_ARC_0_CNTRL, BCHP_MEMC_0_ARC_3_VIOLATION_INFO_CMD) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_EBI_CS_BASE_0, BCHP_EBI_BURST_CFG) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_TIMER_TIMER_IS, BCHP_TIMER_WDCTRL) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_GIO_ODEN_LO, BCHP_GIO_STAT_EXT) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_IRQ0_IRQEN, BCHP_IRQ0_IRQEN) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SCIRQ0_SCIRQEN, BCHP_SCIRQ0_SCIRQEN) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_TIMER_TIMER_IE0, BCHP_TIMER_TIMER_IE0) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_UHFR_1_REG_START, BCHP_UHFR_2_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AIO_MISC_REG_START, BCHP_AUD_DSP_SEC0_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSP_CMDBUF_REG_START, BCHP_BSP_INST_PATCHRAM_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RDC_REG_START, BCHP_RDC_REG_END) \
		)

/* Removed	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSCA_CHIP_ADDRESS, BCHP_BSCD_SCL_PARAM) || \ */

#define SPLASH_NUM_SVIDEO_OUTPUTS	1
#define SPLASH_NUM_COMPOSITE_OUTPUTS	2
#define SPLASH_NUM_COMPONENT_OUTPUTS	1

#define BRCM_DAC_SVIDEO_LUMA      BVDC_Dac_6
#define BRCM_DAC_SVIDEO_CHROMA    BVDC_Dac_5
#define BRCM_DAC_COMPOSITE_0      BVDC_Dac_4

#define BRCM_DAC_Y                BVDC_Dac_0
#define BRCM_DAC_PR               BVDC_Dac_2
#define BRCM_DAC_PB               BVDC_Dac_1

#define BRCM_DAC_COMPOSITE_1      BVDC_Dac_3

/* The default display for this board */
#ifdef CFG_SPLASH_PAL
#define SPLASH_CMP_0_PATH_DISPLAY	BFMT_VideoFmt_e480p
#define SPLASH_CMP_1_PATH_DISPLAY	BFMT_VideoFmt_ePAL_I
#else
#define SPLASH_CMP_0_PATH_DISPLAY	BFMT_VideoFmt_e480p
#define SPLASH_CMP_1_PATH_DISPLAY	BFMT_VideoFmt_eNTSC
#endif

/* End of File */
