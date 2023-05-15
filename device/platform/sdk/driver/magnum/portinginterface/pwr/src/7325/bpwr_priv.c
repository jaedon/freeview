/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpwr_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/21/12 7:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwr/src/7325/bpwr_priv.c $
 * 
 * Hydra_Software_Devel/1   9/21/12 7:06p nickh
 * SW7425-3971: Add chip-specific private files
 * 
 * Hydra_Software_Devel/19   11/3/10 10:57a jrubio
 * SW7325-805: add uhfr header
 * 
 * Hydra_Software_Devel/18   11/2/10 5:12p jrubio
 * SW7325-805: add UHF LDO power down
 * 
 * Hydra_Software_Devel/17   12/28/09 3:43p jrubio
 * SW7335-644: remove FMISC resets
 * 
 * Hydra_Software_Devel/16   9/17/09 3:50p jrubio
 * SW7335-548: fix audio issue. AC0 and VC0 were not powered up
 * 
 * Hydra_Software_Devel/15   7/31/09 1:59p jrubio
 * PR57226: Fix Power Up and Power Down Sequence
 * 
 * Hydra_Software_Devel/14   7/30/09 5:50p jrubio
 * PR57226: refactor file with AtomicUpdate and register changes
 * 
 * Hydra_Software_Devel/13   7/8/09 6:29p jtna
 * PR56665: refactor RFM power management (port from 7405)
 * 
 * Hydra_Software_Devel/12   6/26/09 4:42p jrubio
 * PR55075: rework PWR PI for 7325
 * 
 * Hydra_Software_Devel/11   5/13/09 6:10p jrubio
 * PR55075: fix PWR PI
 * 
 * Hydra_Software_Devel/10   3/31/09 4:25p garylin
 * PR43788: remove PWR_P_ValidateChange(), just return ok
 * 
 * Hydra_Software_Devel/9   2/13/09 11:05a garylin
 * PR43788: fix compile warning
 * 
 * Hydra_Software_Devel/8   1/16/09 6:11p garylin
 * PR51228: add VCXO_CTL_MISC_EREF_CTRL register bit 3 PLL power down/up
 * in PWR module for 73x5 platforms. EREF PLL provides clocks to ENET and
 * 73x5 frontend QPSK.
 * 
 * Hydra_Software_Devel/7   9/11/08 2:39p garylin
 * PR43788: added a new bit in clockGen power control 3 register to power
 * down the QPSK AFE and demod
 * 
 * Hydra_Software_Devel/6   8/26/08 9:40a garylin
 * PR43788: don't set PWRDN_CEC and PWRDN_REFAMP to 1 during power down
 * 
 * Hydra_Software_Devel/5   8/19/08 10:20a garylin
 * PR43788: add new frontend module ids into the module list
 * 
 * Hydra_Software_Devel/3   8/18/08 5:33p garylin
 * PR43788: added power down/up frontend clockgen for AST, QPSK, and FTM
 * 
 * Hydra_Software_Devel/2   6/30/08 11:26a garylin
 * PR43788: fixed s/w bug in TDAC power down/up
 * 
 * Hydra_Software_Devel/1   6/20/08 1:29p garylin
 * PR43788: initial version, ported from 7405.
 * 
 ***************************************************************************/
#include "bstd.h"                /* standard types */
#include "berr.h"
#include "bstd_ids.h"
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"                /* malloc */


#include "bchp_memc_1.h"
#include "bchp_memc_1_1.h"
#include "bchp_memc_1_ddr.h"
#include "bchp_clkgen.h"
#include "bchp_hdmi_tx_phy.h"
#include "bchp_rfm_sysclk.h"
#include "bchp_aio_misc.h"
#include "bchp_vcxo_ctl_misc.h"
#include "bchp_misc.h"
#include "bchp_bmisc.h"
#include "bchp_mmisc.h"
#include "bchp_fmisc.h"
#include "bchp_uhfr_glbl.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_sds.h"
#include "bchp_uhfr_1.h"

#include "bpwr.h"
#include "bpwr_priv.h"
#include "bpwr_mem.h"

#if ((BCHP_CHIP==7400 && BCHP_VER>=BCHP_VER_D0) || BCHP_CHIP==7420)
#define RFM_DUAL_DAC 1    /* RFM cores with dual DAC output */
#endif

BDBG_MODULE(BPWR);

static BSTD_Module g_ModuleList[] =
{
    BSTD_HDM_ID,
    BSTD_RFM_ID,
    BSTD_SCD_ID,
    BSTD_VBI_ID,
    BSTD_VDC_ID,
    BSTD_XPT_ID,
    BSTD_SDS_ID,
    BSTD_VSB_ID,
    BSTD_DMA_ID,
    BSTD_LNA_ID,
    BSTD_GRC_ID,
    BSTD_XVD_ID,
    BSTD_RAP_ID,
    BSTD_AST_ID,
    BSTD_UHF_ID,
    BSTD_MDM_ID,
    BSTD_NET_ID,
    BSTD_FTM_ID,
    BSTD_QPSK_ID,
    0
};

BERR_Code BPWR_P_Init(BPWR_P_Context    *pPwr )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(pPwr, BPWR_P_Context);

    for (i=0;i<BPWR_MAX_MODULES && g_ModuleList[i];i++) {
        pPwr->modules[i].moduleId = g_ModuleList[i];
        pPwr->modules[i].mode = BPWR_Mode_ePowerUndefined;
    }
    /* if we assert, then BPWR_MAX_MODULES must be increased. it's an internal failure, so assert. */
    BDBG_ASSERT(i < BPWR_MAX_MODULES);

    if (pPwr->hChip == NULL) {
        BERR_Code rc;

        /* BPWR_Open is being called before BCHP_Open. This code must power up the minimum number of modules to allow
        Magnum to initialize safely. This usually means ensuring that every L2 interrupt register can be accessed safely. */
        BSTD_Module initModuleList[10] = {BSTD_RAP_ID,BSTD_HDM_ID, BSTD_SCD_ID, BSTD_VDC_ID, BSTD_VBI_ID, BSTD_XVD_ID, BSTD_AST_ID, BSTD_RFM_ID,BSTD_GRC_ID, BSTD_XPT_ID};
        rc = BPWR_SetModes(pPwr, initModuleList,10, BPWR_Mode_ePowerUp);
        if (rc) return BERR_TRACE(rc);
        rc = BPWR_ApplyChanges(pPwr);
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

BERR_Code BPWR_P_Destroy
    ( BPWR_P_Context    *pPwr )
{
    BSTD_UNUSED(pPwr);
    return 0;
}

static bool BPWR_P_IsModuleChanging(const BPWR_P_Context *pPwr, BSTD_Module moduleId)
{
    unsigned i;
    for (i=0;i<BPWR_MAX_MODULES;i++) {
        if (pPwr->modules[i].moduleId == moduleId) {
            return pPwr->modules[i].change;
        }
    }
    return false;
}

BERR_Code BPWR_P_ValidateChanges
    ( BPWR_P_Context    *pPwr )
{
    BERR_Code rc = BERR_SUCCESS;
    unsigned count;

    /* verify change sets */

    count = (BPWR_P_IsModuleChanging(pPwr, BSTD_HDM_ID)?1:0) +
            (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID)?1:0) +
            (BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID)?1:0);
    if (count != 0 && count != 3) {
        BDBG_ERR(("VDC, VBI and HDM modes all need to be set together (%d).", count));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    return rc;
}

BERR_Code BPWR_P_SetPowerMode ( BPWR_P_Context    *pPwr)
{
    BERR_Code                     eStatus = BERR_SUCCESS;
    uint32_t                      ulRegData;

    BDBG_ENTER(BPWR_P_SetPowerMode);

    /* handle power down or power up in the order PWR requires */
    if (pPwr->changeMode == BPWR_Mode_ePowerDown)
    {
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_QPSK_ID))
        {
            BDBG_MSG(("Power Down: QPSK"));
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_CG_QPSK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_FTM_ID))
        {
            BDBG_MSG(("Power Down: FTM"));
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_CG_FTM_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData, ulRegData);
        }


        if (BPWR_P_IsModuleChanging(pPwr, BSTD_AST_ID))
        {
            BDBG_MSG(("Power Down: AST"));
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_CG_SDS0_MASK |
            BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_CG_AFEC_MASK | 
            BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_216_CG_TFEC_MASK | 
            BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_216_CG_AFEC_MASK ;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            /* turn on RFM clock before attempting reg access */
            ulRegData = (BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_RFM_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData, 0);

            /* This sequence is copied from RFM PI's power down sequence. 
            Required to support power down via either PWR or RFM. */
            BDBG_MSG(("Power Down: RFM"));
            ulRegData = BREG_Read32(pPwr->hRegister,BCHP_RFM_SYSCLK_DACCTL);
            ulRegData |= BCHP_RFM_SYSCLK_DACCTL_DAC_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL, ulRegData);

#if RFM_DUAL_DAC
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL);
            ulRegData |= BCHP_RFM_SYSCLK_DACCTL_DAC_PWRDN_2_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL, ulRegData);
#endif

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL);
            ulRegData |= BCHP_RFM_SYSCLK_CLKCTL_CLK_OFF_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_RFM_PHASEITP_CTL);
            ulRegData |= BCHP_RFM_SYSCLK_RFM_PHASEITP_CTL_PHASEITP_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_RFM_PHASEITP_CTL, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_PLL_MISC1);
            ulRegData |= BCHP_RFM_SYSCLK_PLL_MISC1_NDIV_PWRDN_MASK |
                     BCHP_RFM_SYSCLK_PLL_MISC1_REFCOMP_PWRDN_MASK |
                     BCHP_RFM_SYSCLK_PLL_MISC1_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_PLL_MISC1, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL);
            ulRegData |= BCHP_RFM_SYSCLK_DACCTL_BIAS50U_OFF_MASK |
                     BCHP_RFM_SYSCLK_DACCTL_BIASLOG_OFF_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL);
            ulRegData |= BCHP_RFM_SYSCLK_CLKCTL_RFMCLK_OFF_MASK |
                     BCHP_RFM_SYSCLK_CLKCTL_VREG_OFF_MASK |
                     BCHP_RFM_SYSCLK_CLKCTL_BGCORE_OFF_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL, ulRegData);

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            /* step 69, HifiDriver */
            BDBG_MSG(("Power Down: RAP"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN);
            ulRegData |= BCHP_AIO_MISC_PWRDOWN_DAC0_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: VDC"));
            /* TDAC_0: step 70 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC0_BG_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_REFAMP_MASK |
                         BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC0_BG_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC01_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC01_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC01_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC02_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC02_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC02_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC03_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC03_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC03_CTRL_REG, ulRegData);

            /* TDAC0 Top Voltage*/
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_TDAC0_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData, ulRegData);

            /* TDAC_1: step 75 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_REFAMP_MASK |
                         BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC11_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC11_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC11_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC12_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC12_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC12_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC13_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC13_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC13_CTRL_REG, ulRegData);

             /* TDAC1 Top Voltage*/
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_TDAC1_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_HDM_ID))
        {
            BDBG_MSG(("Power Down: HDM"));
            /* step 80, do we need to keep CEC Rx channel up? */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL);
            ulRegData |= BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_0_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_1_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_2_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_CK_PWRDN_MASK |
             /*            BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_CEC_MASK |  4 */
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PLL_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_REF_COMP_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_BG_MASK |
             /*            BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_REFAMP_MASK |  8 */
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_0_RESET_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_1_RESET_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_2_RESET_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_CK_RESET_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_A_RESET_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_D_RESET_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0);
            ulRegData |= BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0_RND_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1);
            ulRegData |= BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1_I_PWR_DN_CH1_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ulRegData);

            /* step 83 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2);
            ulRegData |= BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2_I_PWRDN_CH2_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, ulRegData);

            /* step 83.5 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_HDMI_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            BDBG_MSG(("Power Down: RFM CLK"));
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_RFM_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Down: RAP CLK"));
            /* step 84 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_RPTD_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_AIO_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,ulRegData);

            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_23P04_27_CG_PLLAUD0_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_RPTD_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_AIO_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,ulRegData);

            /* step 85, 87 */
            ulRegData =  BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_23P04_27_CG_PLLAUD1_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData,ulRegData);

            /* step 86, 88 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_3_PWRDN_CLOCK_RPT_AUD_DSP_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_3, ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: VDC CLK"));
            /* step 89: VEC */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_VEC_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_DAC_CG_VEC_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_27_656_VCXO_CG_VEC_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,ulRegData);

            /* step 90 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_VEC_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,ulRegData);

#if 0 /* Do not reset the registers. */
            /* step 91: BVND */
            /* ulRegData = BREG_Read32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET); */
            ulRegData = 0; /* all reserved bit need to be 0 */
            ulRegData |= BCHP_FMISC_SOFT_RESET_RDC_MASK |
                         BCHP_FMISC_SOFT_RESET_VFD_1_MASK |
                         BCHP_FMISC_SOFT_RESET_VFD_0_MASK |
                         BCHP_FMISC_SOFT_RESET_MFD_0_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET, ulRegData);
#endif
            /* step 92 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_BVND_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_BVNM_MASK | 
                BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_BVNE_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,ulRegData);

            /* step 93 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_BVND_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_BVNM_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_BVNE_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,ulRegData);

            if( BPWR_P_IsModuleChanging(pPwr, BSTD_GRC_ID) )
            {
                BDBG_MSG(("Power Down: GRC CLK"));
                /* step 99: GFX */
                ulRegData = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_GFX_MASK;
                BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,ulRegData);
    
                /* step 100 */
                ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_GFX_MASK;
                BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,ulRegData);
            }

        }

      

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XVD_ID))
        {
            BDBG_MSG(("Power Down: XVD"));
            /* step 101: AVD*/
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_AVD_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,ulRegData);

            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_AVD_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,ulRegData);

            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_3_PWRDN_CLOCK_AVD0_PROG_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_3, ulRegData,ulRegData);

            ulRegData = BCHP_CLKGEN_AVD_CTRL_POWERDOWN_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_AVD_CTRL, ulRegData,ulRegData);
     }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XPT_ID))
        {
            BDBG_MSG(("Power Down: XPT"));
            /* step 104: XPT */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_XPT_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,ulRegData);

            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_20P25_CG_XPT_MASK |
                  BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_27_CG_XPT_MASK |
                  BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_40P5_CG_XPT_MASK |
                  BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_54_CG_XPT_MASK |
                  BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_81_CG_XPT_MASK |
                  BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_XPT_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,ulRegData);

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_MDM_ID))
        {
            BDBG_MSG(("Power Down: Soft Modem"));
            /* step 108: MDM */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_27_SM_CG_SUN_MASK |
                         BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_32P4_DAA_CG_SUN_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_UHF_ID))
        {
            BDBG_MSG(("Power Down: UHF"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1);
            ulRegData &= ~BCHP_UHFR_GLBL_AUTOPD1_AUTO_PD_EN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1, ulRegData);

            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_81_27_UHFR_CG_SUN_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_27_UHFR_CG_SUN_MASK |
               BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_81_27_CG_UHF_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData,ulRegData);

            ulRegData = BCHP_UHFR_1_ANADIR_LDO_PWD_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_UHFR_1_ANADIR, ulRegData, 1);

        }

        /*
          *    VCXO, ClockGen, step 90, 94, 100?
          */
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: bVDC VCXO"));

            /* step 60, 61 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
            ulRegData |= BCHP_VCXO_CTL_MISC_VC0_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ulRegData);

#if 0 
            ulRegData = BCHP_CLKGEN_AVD_CTRL_POWERDOWN_MASK;
           BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLKGEN_AVD_CTRL,ulRegData,ulRegData);
#endif
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Down: bRAP VXCO"));
            /* step 62, 63, 64 */
            /* Cannot touch this clock it is powered down by another clock */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL);
            ulRegData |= BCHP_VCXO_CTL_MISC_AC0_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL, ulRegData);

            if (BPWR_P_IsModuleChanging(pPwr, BSTD_SCD_ID)) /* SmartCard clock is from AC1 */
            {
                ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL);
                ulRegData |= BCHP_VCXO_CTL_MISC_AC1_CTRL_POWERDOWN_MASK;
                BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL, ulRegData);

            }

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AUDDSP_CTRL);
            ulRegData |= BCHP_VCXO_CTL_MISC_AUDDSP_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AUDDSP_CTRL, ulRegData);

        }

        /* Power off all VCXO */
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_SCD_ID) &&(BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID)) && BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Down: VXCO all"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLKGEN_MISC);
            ulRegData |= BCHP_CLKGEN_MISC_DIS_VEC_VCXO_656_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLKGEN_MISC, ulRegData);

            ulRegData= BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_VCXOCTL_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1,ulRegData,ulRegData);
        }
    }
    /*
      *============================================================
      *                Power Up
      *============================================================
      */
    else
    {

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_SCD_ID) ||
            (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID)) ||
            BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Up: VCXO"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLKGEN_MISC);
            ulRegData &= ~BCHP_CLKGEN_MISC_DIS_VEC_VCXO_656_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLKGEN_MISC, ulRegData);

            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1,BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_VCXOCTL_MASK,0);

        }


        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            /* Power on all VCXO */
            BDBG_MSG(("Power Up: RAP VCXO"));

            if (BPWR_P_IsModuleChanging(pPwr, BSTD_NET_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_QPSK_ID))
            {
                ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_EREF_CTRL);
                ulRegData &= ~BCHP_VCXO_CTL_MISC_EREF_CTRL_POWERDOWN_MASK;
                BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_EREF_CTRL, ulRegData);
            }

      /* step 125 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AUDDSP_CTRL);
            ulRegData &= ~BCHP_VCXO_CTL_MISC_AUDDSP_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AUDDSP_CTRL, ulRegData);

            if  (BPWR_P_IsModuleChanging(pPwr, BSTD_SCD_ID)) /* SmartCard clock is from AC1 */
            {
                BDBG_MSG(("Power Up: SCD"));
                ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL);
                ulRegData &= ~BCHP_VCXO_CTL_MISC_AC1_CTRL_POWERDOWN_MASK;
                BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL, ulRegData);

            }

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL);
            ulRegData &= ~BCHP_VCXO_CTL_MISC_AC0_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL, ulRegData);

            
            ulRegData = BCHP_CLKGEN_AVD_CTRL_POWERDOWN_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_AVD_CTRL, ulRegData,0);
        }

        


        if  (BPWR_P_IsModuleChanging(pPwr, BSTD_UHF_ID))
        {
            BDBG_MSG(("Power Up: UHF"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1);
            ulRegData &= ~BCHP_UHFR_GLBL_AUTOPD1_AUTO_PD_EN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1, ulRegData);

            ulRegData = ( BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_81_27_UHFR_CG_SUN_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_27_UHFR_CG_SUN_MASK |
               BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_81_27_CG_UHF_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData,0);

            ulRegData = BCHP_UHFR_1_ANADIR_LDO_PWD_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_UHFR_1_ANADIR, ulRegData, 1);

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_MDM_ID))
        {
            BDBG_MSG(("Power Up: Soft Modem"));
            /* step 121: MDM */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2);
            ulRegData =  (BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_27_SM_CG_SUN_MASK |
                         BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_32P4_DAA_CG_SUN_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData,0);

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XPT_ID))
        {
            BDBG_MSG(("Power Up: XPT"));

            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_XPT_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,0);

            ulRegData = (BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_20P25_CG_XPT_MASK |
                  BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_27_CG_XPT_MASK |
                  BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_40P5_CG_XPT_MASK |
                  BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_54_CG_XPT_MASK |
                  BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_81_CG_XPT_MASK |
                  BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_XPT_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XVD_ID))
        {
            BDBG_MSG(("Power Up: XVD CLK"));

            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_AVD_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,0);

            ulRegData = (BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_AVD_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,0);

            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_3_PWRDN_CLOCK_AVD0_PROG_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_3, ulRegData,0);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
            ulRegData &= ~BCHP_VCXO_CTL_MISC_VC0_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ulRegData);

            ulRegData = BCHP_CLKGEN_AVD_CTRL_RESET_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister,BCHP_CLKGEN_AVD_CTRL,ulRegData,0);

            }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Up: VDC CLK"));
          /* step 89: VEC */
            ulRegData = (BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_VEC_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_DAC_CG_VEC_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_27_656_VCXO_CG_VEC_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,0);

            /* step 90 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_VEC_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,0);

            /* step 91*/
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLKGEN_MISC);
            ulRegData &= ~BCHP_CLKGEN_MISC_DIS_VEC_VCXO_656_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLKGEN_MISC, ulRegData);

            /* step 91: BVND */
            /* ulRegData = BREG_Read32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET); */
            ulRegData = 0; /* all reserved bit need to be 0 */
           /* BREG_Write32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET, ulRegData); */

            /* step 92 */
            ulRegData = (BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_BVND_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_BVNM_MASK | 
                BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_BVNE_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,0);

            /* step 93 */
            ulRegData = (BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_BVND_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_BVNM_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_BVNE_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,0);


            if( BPWR_P_IsModuleChanging(pPwr, BSTD_GRC_ID) )
            {
                BDBG_MSG(("Power Up: GRC CLK"));
                /* step 99: GFX */
                ulRegData = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_GFX_MASK;
                BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,0);
    
                /* step 100 */
                ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_GFX_MASK;
                BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,0);
            }



        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Up: RAP CLK"));
            ulRegData = (BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_RPTD_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_216_CG_AIO_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_0, ulRegData,0);

            ulRegData = (BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_23P04_27_CG_PLLAUD0_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_RPTD_MASK |
                BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_AIO_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,0);

            /* step 85, 87 */
            ulRegData =  BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_23P04_27_CG_PLLAUD1_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData,0);

            /* step 86, 88 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_3_PWRDN_CLOCK_RPT_AUD_DSP_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_3, ulRegData,0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            BDBG_MSG(("Power Up: RFM CLK"));
            ulRegData = (BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_RFM_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,0);
            
            /* for power up of RFM core via PWR PI, we only turn on the top-level clock to ensure register access. 
               RFM functionality is not guaranteed. RFM users should use the power down/up functions in RFM PI 
               for power management with RFM functionality */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_HDM_ID))
        {
            BDBG_MSG(("Power Up: HDM"));

            /* step 80.5 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_HDMI_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,0);
        
            /* step 83 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2);
            ulRegData &= ~BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2_I_PWRDN_CH2_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1);
            ulRegData &= ~BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1_I_PWR_DN_CH1_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0);
            ulRegData &= ~BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0_RND_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, ulRegData);

            /* step 80, do we need to keep CEC Rx channel up? */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL);
            ulRegData &= ~(BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_0_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_1_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_2_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_CK_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_CEC_MASK | /* 4 */
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PLL_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_REF_COMP_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_BG_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_REFAMP_MASK | /* 8 */
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_0_RESET_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_1_RESET_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_2_RESET_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_CK_RESET_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_A_RESET_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_D_RESET_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Up: VDC"));
            /* TDAC_1 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_TDAC1_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,0);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC13_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC13_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC13_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC12_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC12_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC12_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC11_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC11_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC11_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG);
            ulRegData &= ~(BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_REFAMP_MASK |
                         BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG, ulRegData);

            /* TDAC_0 */
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_108_CG_TDAC0_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_1, ulRegData,0);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC03_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC03_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC03_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC02_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC02_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC02_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC01_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC01_CTRL_REG_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC01_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC0_BG_CTRL_REG);
            ulRegData &= ~(BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_REFAMP_MASK |
                         BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC0_BG_CTRL_REG, ulRegData);
        }


        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            /* step 69, HifiDriver */
            BDBG_MSG(("Power Up: RAP"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN);
            ulRegData &= ~BCHP_AIO_MISC_PWRDOWN_DAC0_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_AST_ID))
        {
            BDBG_MSG(("Power Up: AST"));
            ulRegData = (BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_CG_SDS0_MASK |
            BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_CG_AFEC_MASK | 
            BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_216_CG_TFEC_MASK | 
            BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_216_CG_AFEC_MASK) ;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData,0);
 #if 1
            /* required for L2 access */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_SDS_SPLL_PWRDN);
            ulRegData = 0x20;
            BREG_Write32(pPwr->hRegister, BCHP_SDS_SPLL_PWRDN, ulRegData);
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_SDS_SPLL_MODE);
            ulRegData &= ~0x04;
            BREG_Write32(pPwr->hRegister, BCHP_SDS_SPLL_MODE, ulRegData);
#endif
        
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_FTM_ID))
        {
            BDBG_MSG(("Power Up: FTM"));
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_CG_FTM_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData,0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_QPSK_ID))
        {
            BDBG_MSG(("Power Down: QPSK"));
            ulRegData = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_CG_QPSK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLKGEN_PWRDN_CTRL_2, ulRegData,0);
        }

       
    }

    BDBG_LEAVE(BPWR_P_SetPowerMode);
    return eStatus;
}

/* End of file */

