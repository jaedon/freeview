/***************************************************************************
 *     (c)2005-2009 Broadcom Corporation
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
 * $brcm_Workfile: bsplash_board.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 5/7/10 2:29p $
 *
 * Module Description: Application to generate RULs for splash screen. 
 *                     This is a slightly modified copy of vdc_dump.c
 *                     ported to Nucleus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/97405/bsplash_board.h $
 * 
 * 6   5/7/10 2:29p randyjew
 * SW7468-227:Add SPLASH_NUM_XXX_OUTPUTS defines to be compatible with
 * splash_setup_vdc.c
 * 
 * 5   5/14/09 5:41p shyam
 * PR52592 : Add support for ARGB8888 surfaces
 * 
 * 4   4/8/09 4:02p shyam
 * PR52386 : Merge from nexus based splash
 * 
 * 2   4/8/09 12:46p shyam
 * PR52386 : Port splash to nexus Base Build system
 * 
 * 3   9/3/08 3:40p mananp
 * PR46483:PACEBOX
 * 
 * 2   9/2/08 6:02p mananp
 * PR46483:PACEBOX
 * 
 * 1   9/02/08 5:05p mananp
 *          Add flag for Pace Box control register exclude list
 *
 * 1   3/25/08 7:14p katrep
 * PR38270: Add splash support for 7405
 * 
 * 4   11/2/07 12:57p shyam
 * PR 30741 : Correct composite 1
 * 
 * 3   10/19/07 4:11p shyam
 * PR 30741 : Add support for PAL + 576p
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

#if 0
    BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_GFX_L2_REG_START, BCHP_GFX_L2_REG_END)  || \            /* TODO: revisit GFD_0 replacement */
    BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSP_CMDBUF_REG_START, BCHP_BSP_PATCHRAM_REG_END)   || \ /* TODO: revisit CMDBUF_REG_END */
#endif
 
/* These are the registers that need to be excluded from the register dump either
   because they interrupt the CPU or disturb settings done elsewhere like the CFE 
   If you do not want to program certain registers - add them to this macro. 
*/
#define BSPLASH_ADDRESS_IN_RANGE(addr, rangeLow, rangeHigh) ((addr>=rangeLow) && (addr <=rangeHigh))

#ifdef SPLASH_MSIPTV_PACE
#define BSPLASH_REGDUMP_EXCLUDE(addr)	( \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNF_INTR2_0_REG_START, BCHP_BVNF_INTR2_5_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RFM_L2_REG_START, BCHP_RFM_L2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HIF_INTR2_REG_START, BCHP_HIF_CPU_INTR1_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_XPT_BUS_IF_REG_START, BCHP_XPT_GR_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AVD_INTR2_0_REG_START, BCHP_AVD_INTR2_0_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AUD_DSP_INTH0_REG_START, BCHP_AUD_DSP_INTH0_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_GFD_0_REG_START, BCHP_GFD_0_REG_END)  || \
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
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_UHFR_1_REG_START, BCHP_UHFR_GLBL_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AIO_MISC_REG_START, BCHP_AUD_DSP_SEC0_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSP_CMDBUF_REG_START, BCHP_BSP_CMDBUF_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SUN_TOP_CTRL_SW_RESET, BCHP_SUN_TOP_CTRL_SW_RESET)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13)   || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RDC_REG_START, BCHP_RDC_REG_END) \
		)
#else
#define BSPLASH_REGDUMP_EXCLUDE(addr)	( \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNF_INTR2_0_REG_START, BCHP_BVNF_INTR2_5_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BVNB_INTR2_REG_START, BCHP_BVNB_INTR2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RFM_L2_REG_START, BCHP_RFM_L2_REG_END) || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_HIF_INTR2_REG_START, BCHP_HIF_CPU_INTR1_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_XPT_BUS_IF_REG_START, BCHP_XPT_GR_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AVD_INTR2_0_REG_START, BCHP_AVD_INTR2_0_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AUD_DSP_INTH0_REG_START, BCHP_AUD_DSP_INTH0_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_GFD_0_REG_START, BCHP_GFD_0_REG_END)  || \
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
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_UHFR_1_REG_START, BCHP_UHFR_GLBL_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_AIO_MISC_REG_START, BCHP_AUD_DSP_SEC0_REG_END)   || \
		BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSP_CMDBUF_REG_START, BCHP_BSP_CMDBUF_REG_END)   || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_RDC_REG_START, BCHP_RDC_REG_END) || \
        BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_MEMC64_GFX_L2_REG_START, BCHP_MEMC64_GFX_L2_REG_END) \
		)
#endif 

/* Removed	BSPLASH_ADDRESS_IN_RANGE(addr, BCHP_BSCA_CHIP_ADDRESS, BCHP_BSCD_SCL_PARAM) || \ */

#define SPLASH_NUM_SVIDEO_OUTPUTS	1
#define SPLASH_NUM_COMPOSITE_OUTPUTS	1
#define SPLASH_NUM_COMPONENT_OUTPUTS	1

#define BRCM_DAC_SVIDEO_LUMA      BVDC_Dac_0
#define BRCM_DAC_SVIDEO_CHROMA    BVDC_Dac_1
#define BRCM_DAC_COMPOSITE_0      BVDC_Dac_2

#define BRCM_DAC_Y                BVDC_Dac_4
#define BRCM_DAC_PR               BVDC_Dac_5
#define BRCM_DAC_PB               BVDC_Dac_3

#define BRCM_DAC_COMPOSITE_1      0

/* The default display for this board */
#ifdef CFG_SPLASH_PAL
#define SPLASH_CMP_0_PATH_DISPLAY	BFMT_VideoFmt_e576p_50Hz
#define SPLASH_CMP_1_PATH_DISPLAY	BFMT_VideoFmt_ePAL_I
#else
#define SPLASH_CMP_0_PATH_DISPLAY	BFMT_VideoFmt_e480p
#define SPLASH_CMP_1_PATH_DISPLAY	BFMT_VideoFmt_eNTSC
#endif

#define SPLASH_SURFACE_PXL_TYPE	BPXL_eA8_R8_G8_B8	 /* BPXL_eR5_G6_B5  */

#define B_I2C_CHANNEL_HDMI    1

/* End of File */
