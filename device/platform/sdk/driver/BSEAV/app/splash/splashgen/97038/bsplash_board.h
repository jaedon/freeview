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
 * $brcm_Revision: 4 $
 * $brcm_Date: 5/7/10 2:26p $
 *
 * Module Description: Application to generate RULs for splash screen. 
 *                     This is a slightly modified copy of vdc_dump.c
 *                     ported to Nucleus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/97038/bsplash_board.h $
 * 
 * 4   5/7/10 2:26p randyjew
 * SW7468-227:Add SPLASH_NUM_XXX_OUTPUTS defines to be compatible with
 * splash_setup_vdc.c
 * 
 * 3   10/19/07 4:16p shyam
 * PR 30741 : Update for PAL config and 576p support
 * 
 * 2   8/1/07 6:04p shyam
 * PR 33658 : Add legacy define for legacy VDC platforms
 * 
 * 1   5/14/07 6:58p shyam
 * PR 30741 : Add reference support for generic portable splash
 *
 ***************************************************************************/

#include "bchp_sec_it.h"
#include "bchp_prim_it.h"

/* For register exclusions */
#include "bchp_hif_intr2.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_gfx_l2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_5.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_video_enc_intr2.h"
#include "bchp_hdmi_intr2.h"
#include "bchp_vdec_l2_0.h"
#include "bchp_xpt_int.h"
#include "bchp_rfm_l2.h"
#include "bchp_mtp_l2_1.h"
#include "bchp_mtp_l2_2.h"
#include "bchp_ifd_l2.h"
#include "bchp_audio_inth.h"
#include "bchp_irq0.h"
#include "bchp_timer.h"
#include "bchp_scirq0.h"
#include "bchp_ebi.h"
#include "bchp_dtg.h"
#include "bchp_bsca.h"
#include "bchp_bscd.h"
#include "bchp_gio.h"


#if (BCHP_VER<=BCHP_VER_B2)
#include "bchp_memc.h"
#define BCHP_MEMC_0_ARC_0_CNTRL BCHP_MEMC_ARC_0_CNTRL
#define BCHP_MEMC_0_ARC_3_VIOLATION_INFO_HIGH BCHP_MEMC_ARC_3_VIOLATION_INFO_HIGH

#else /* C0 or later */
#include "bchp_memc_0.h"
#endif

/* These are the registers that need to be excluded from the register dump either
   because they interrupt the CPU or disturb settings done elsewhere like the CFE 
   If you do not want to program certain registers - add them to this macro. 
*/
#define BSPLASH_ADDRESS_IN_RANGE(addr, rangeLow, rangeHigh) ((addr>=rangeLow) && (addr <=rangeHigh))

#define BSPLASH_REGDUMP_EXCLUDE(addr)	( \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNF_INTR2_0_REG_START, BCHP_BVNF_INTR2_5_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RFM_L2_REG_START, BCHP_RFM_L2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HIF_INTR2_REG_START, BCHP_HIF_CPU_INTR1_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_XPT_INT_REG_START, BCHP_XPT_INT_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AUDIO_INTH_REG_START, BCHP_AUDIO_INTH_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_IFD_L2_REG_START, BCHP_IFD_L2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_GFX_L2_REG_START, BCHP_GFX_L2_REG_END)  || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HDMI_INTR2_REG_START, BCHP_HDMI_INTR2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_VDEC_L2_0_REG_START, BCHP_VDEC_L2_0_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_VIDEO_ENC_INTR2_REG_START, BCHP_VIDEO_ENC_INTR2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MTP_L2_2_REG_START, BCHP_MTP_L2_2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MTP_L2_1_REG_START, BCHP_MTP_L2_1_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC_0_ARC_0_CNTRL, BCHP_MEMC_0_ARC_3_VIOLATION_INFO_HIGH) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_EBI_CS_BASE_0, BCHP_EBI_BURST_CFG) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSCA_CHIP_ADDRESS, BCHP_BSCD_SCL_PARAM) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_TIMER_TIMER_IS, BCHP_TIMER_WDCTRL) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_GIO_ODEN_LO, BCHP_GIO_STAT_EXT) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_IRQ0_IRQEN, BCHP_IRQ0_IRQEN) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SCIRQ0_SCIRQEN, BCHP_SCIRQ0_SCIRQEN) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_TIMER_TIMER_IE0, BCHP_TIMER_TIMER_IE0) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RDC_REG_START, BCHP_RDC_REG_END) \
		)

#define SPLASH_NUM_SVIDEO_OUTPUTS	1
#define SPLASH_NUM_COMPOSITE_OUTPUTS	1
#define SPLASH_NUM_COMPONENT_OUTPUTS	1
#define BRCM_DAC_SVIDEO_LUMA      BVDC_Dac_2
#define BRCM_DAC_SVIDEO_CHROMA    BVDC_Dac_1
#define BRCM_DAC_COMPOSITE_0      BVDC_Dac_0

#define BRCM_DAC_Y                BVDC_Dac_4
#define BRCM_DAC_PR               BVDC_Dac_3
#define BRCM_DAC_PB               BVDC_Dac_5

#define BRCM_DAC_COMPOSITE_1      BVDC_Dac_Unused

/*************** 7038 has Legacy VDC ********************/
#define LEGACY_VDC

/* The default display for this board */
#ifdef CFG_SPLASH_PAL
#define SPLASH_CMP_0_PATH_DISPLAY	BFMT_VideoFmt_e576p_50Hz
#define SPLASH_CMP_1_PATH_DISPLAY	BFMT_VideoFmt_ePAL_I
#else
#define SPLASH_CMP_0_PATH_DISPLAY	BFMT_VideoFmt_e480p
#define SPLASH_CMP_1_PATH_DISPLAY	BFMT_VideoFmt_eNTSC
#endif

/* End of File */
