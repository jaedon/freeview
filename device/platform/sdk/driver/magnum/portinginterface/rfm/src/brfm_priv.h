/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brfm_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 4/27/12 2:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/rfm/7038/brfm_priv.h $
 * 
 * Hydra_Software_Devel/21   4/27/12 2:10p jtna
 * SW7584-17: add 7584 support
 * 
 * Hydra_Software_Devel/20   3/21/12 11:32a jtna
 * SW7360-10: use BRFM_PLL_MISC=0x4 for 7360
 * 
 * Hydra_Software_Devel/19   1/23/12 5:12p jtna
 * SW7360-10: add 7360 rfm
 * 
 * Hydra_Software_Devel/18   9/27/11 11:54a jtna
 * SW7429-22: add 7429 support
 * 
 * Hydra_Software_Devel/17   7/6/11 11:06a jtna
 * SW7552-26: fix macro typo
 * 
 * Hydra_Software_Devel/16   5/19/11 3:35p jtna
 * SW7552-26: add 7552 compile support
 * 
 * Hydra_Software_Devel/15   1/18/11 6:19p jtna
 * SW7550-660: make brfm_scripts.c more stand-alone
 * 
 * Hydra_Software_Devel/14   1/17/11 3:09p jtna
 * SW7550-660: refactored RFM conditional compile definitions
 * 
 * Hydra_Software_Devel/13   11/2/10 2:41p jtna
 * SW7552-6: add 7552 RFM support
 * 
 * Hydra_Software_Devel/12   5/6/10 3:49p jtna
 * SW7420-730: added SW workaround for noise spur
 * 
 * Hydra_Software_Devel/11   7/10/09 3:23p jtna
 * PR56665: handle platform-dependent PM registers better
 * 
 * Hydra_Software_Devel/10   7/10/09 10:21a jtna
 * PR56665: defines for 7420 PM registers
 * 
 * Hydra_Software_Devel/9   7/8/09 5:50p jtna
 * PR56665: refactor RFM power management
 * 
 * Hydra_Software_Devel/8   2/6/09 10:14a jtna
 * PR45633: 7420 support
 * 
 ***************************************************************************/

#ifndef BRFM_PRIV_H__
#define BRFM_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Platform definitions */

/* RFM core major revision. Taken from RFM_SYSCLK_REVID.MAJOR */
#if (BCHP_CHIP==7552 || BCHP_CHIP==7429 || BCHP_CHIP==7360 || BCHP_CHIP==7584)
#define BRFM_REVID 51
#elif (BCHP_CHIP==7420)
#define BRFM_REVID 50
#elif ((BCHP_CHIP==7400 && BCHP_VER>=BCHP_VER_B0) || BCHP_CHIP==7405 || BCHP_CHIP==7325 || BCHP_CHIP==7335 || \
        BCHP_CHIP==7336)
#define BRFM_REVID 40
#else
#define BRFM_REVID 30
#endif

#if (BCHP_CHIP==7420)
#define BRFM_SPUR_WORKAROUND 1 /* SW workaround for noise spur (SW7420-730) */
#endif

#if ((BCHP_CHIP==7400 && BCHP_VER>=BCHP_VER_D0) || BCHP_CHIP==7420)
#define BRFM_DUAL_DAC 1    /* dual DAC output */
#endif

#if (BRFM_REVID==51 && (BCHP_CHIP==7429)) 
/* the only difference between 7552 and 7429 is that 7429 hands off HiFiDAC and VEC data on the negative edge of 108M clock
   RFM_SYSCLK_MISC.VIRF_EDGE and RFM_SYSCLK_MISC.HIRF_EDGE must be 1. i.e. PLL_MISC=0x5 */
#define BRFM_PLL_MULT  (0x002E)
#define BRFM_PLL_MISC  (0x00000005)
#elif (BRFM_REVID>=50 || (BCHP_CHIP==7400 && BCHP_VER>=BCHP_VER_B0) /* 7400B0 and up is an exception*/) /* includes 7552, 7360 and 7584 */
#define BRFM_PLL_MULT  (0x002E)
#define BRFM_PLL_MISC  (0x00000004)
#elif (BRFM_REVID==40)
#define BRFM_PLL_MULT  (0x002E)
#define BRFM_PLL_MISC  (0x00000005)
#else
#define BRFM_PLL_MULT  (0x502E)
#define BRFM_PLL_MISC  (0x0000000C)
#endif
#define BRFM_PLL_FS    (378.0)

/* Max/Min volumes in dB */
#if (BRFM_REVID>=50)
#define BRFM_MAX_VOLUME (12)
#define BRFM_MIN_VOLUME (-52) 
#else
#define BRFM_MAX_VOLUME (30)
#define BRFM_MIN_VOLUME (-34)
#endif
#define BRFM_MAX_VOLUME_NICAM (12)
#define BRFM_MIN_VOLUME_NICAM (-52)

/* this is an outside register that is platform-dependent */
#if (BCHP_CHIP==7325)
#define BRFM_P_CLK_PM_CTRL_BCHP    BCHP_CLKGEN_PWRDN_CTRL_1
#define BRFM_P_CLK_PM_CTRL_MASK    BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_RFM_MASK
#define BRFM_P_CLK_PM_CTRL_REG     CLKGEN_PWRDN_CTRL_1
#define BRFM_P_CLK_PM_CTRL_FIELD   PWRDN_CLOCK_108_CG_RFM
#define BRFM_P_CLK_PM_CTRL_ENABLED 0
#elif (BCHP_CHIP==7420)
#define BRFM_P_CLK_PM_CTRL_BCHP    BCHP_CLK_RFM_CLK_PM_CTRL
#define BRFM_P_CLK_PM_CTRL_MASK    BCHP_CLK_RFM_CLK_PM_CTRL_DIS_RFM_108M_CLK_MASK
#define BRFM_P_CLK_PM_CTRL_REG     CLK_RFM_CLK_PM_CTRL
#define BRFM_P_CLK_PM_CTRL_FIELD   DIS_RFM_108M_CLK
#define BRFM_P_CLK_PM_CTRL_ENABLED 0
#elif (BRFM_REVID==51)
    #if (BCHP_CHIP==7552 || BCHP_CHIP==7360 || BCHP_CHIP==7584)
#define BRFM_P_CLK_PM_CTRL_BCHP    BCHP_CLKGEN_RFM_TOP_CLOCK_ENABLE
#define BRFM_P_CLK_PM_CTRL_MASK    BCHP_CLKGEN_RFM_TOP_CLOCK_ENABLE_RFM_108_CLOCK_ENABLE_MASK
#define BRFM_P_CLK_PM_CTRL_REG     CLKGEN_RFM_TOP_CLOCK_ENABLE
#define BRFM_P_CLK_PM_CTRL_FIELD   RFM_108_CLOCK_ENABLE
#define BRFM_P_CLK_PM_CTRL_ENABLED 1 /* if enabled, then 1. other platforms have the opposite meaning */
    #else /* 7429 */
#define BRFM_P_CLK_PM_CTRL_BCHP    BCHP_CLKGEN_RFM_TOP_INST_CLOCK_ENABLE
#define BRFM_P_CLK_PM_CTRL_MASK    BCHP_CLKGEN_RFM_TOP_INST_CLOCK_ENABLE_RFM_108_CLOCK_ENABLE_MASK
#define BRFM_P_CLK_PM_CTRL_REG     CLKGEN_RFM_TOP_INST_CLOCK_ENABLE
#define BRFM_P_CLK_PM_CTRL_FIELD   RFM_108_CLOCK_ENABLE
#define BRFM_P_CLK_PM_CTRL_ENABLED 1 /* if enabled, then 1. other platforms have the opposite meaning */
    #endif
#else /* all others */
#define BRFM_P_CLK_PM_CTRL_BCHP    BCHP_CLK_PM_CTRL
#define BRFM_P_CLK_PM_CTRL_MASK    BCHP_CLK_PM_CTRL_DIS_RFM_108M_CLK_MASK
#define BRFM_P_CLK_PM_CTRL_REG     CLK_PM_CTRL
#define BRFM_P_CLK_PM_CTRL_FIELD   DIS_RFM_108M_CLK
#define BRFM_P_CLK_PM_CTRL_ENABLED 0
#endif

/*******************************************************************************
*
* Private Module Handles
*
*******************************************************************************/
typedef struct BRFM_P_ModulationInfo
{
    BRFM_ModulationType modType;
    const uint32_t *setupScrCh[BRFM_OutputChannel_eLast];
    const uint32_t *setupScrAudioEncoding[BRFM_AudioEncoding_eLast];
    const uint32_t *setupScrConfig;
} BRFM_P_ModulationInfo;

#ifdef __cplusplus
}
#endif

#endif

