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
 * $brcm_Date: 9/21/12 7:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwr/src/7335/bpwr_priv.c $
 * 
 * Hydra_Software_Devel/1   9/21/12 7:08p nickh
 * SW7425-3971: Add chip-specific private files
 * 
 * Hydra_Software_Devel/17   5/26/10 2:27p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/17   5/26/10 2:25p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/17   5/26/10 2:24p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/17   5/26/10 2:23p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/17   5/26/10 2:23p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/17   5/26/10 2:23p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/17   5/26/10 2:23p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/17   5/26/10 2:23p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/17   5/26/10 2:22p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/16   9/17/09 5:07p jrubio
 * SW7335-548: make sure VC0 and AC0 are powered up
 * 
 * Hydra_Software_Devel/15   7/29/09 2:37p jrubio
 * PR56659: cleanup further 7335 Power Down Sequence, Let VDC power down
 * TDACs
 * 
 * Hydra_Software_Devel/14   7/9/09 5:50p jrubio
 * PR56659: Update with AtomicUpdate32, do not shutdown TDAC's high Level
 * Clock
 * 
 * Hydra_Software_Devel/13   7/8/09 6:07p jtna
 * PR56665: refactor RFM power management (port from 7405)
 * 
 * Hydra_Software_Devel/12   6/25/09 3:54p erickson
 * PR45902: add RFM to initial state, fix VEC, fix HDM
 *
 * Hydra_Software_Devel/11   5/15/09 11:59a erickson
 * PR55154: rework
 *
 * Hydra_Software_Devel/10   5/13/09 10:05a jrubio
 * PR55075: fix power pi
 *
 * Hydra_Software_Devel/8   2/13/09 10:59a garylin
 * PR51629: fix compile warning
 *
 * Hydra_Software_Devel/7   1/30/09 4:01p jrubio
 * PR51629: fix broken if statements
 *
 * Hydra_Software_Devel/6   1/30/09 3:58p jrubio
 * PR51629: fix broken if statements
 *
 * Hydra_Software_Devel/5   1/29/09 4:39p garylin
 * PR51626: added support for 7336 (just add ID checking during compile
 * time.)
 *
 * Hydra_Software_Devel/4   1/16/09 6:41p garylin
 * PR51229: add VCXO_CTL_MISC_EREF_CTRL register bit 3 PLL power down/up
 * in PWR module for 73x5 platforms. EREF PLL provides clocks to ENET and
 * 73x5 frontend QPSK.
 *
 * Hydra_Software_Devel/3   8/26/08 9:41a garylin
 * PR44622: don't set PWRDN_CEC and PWRDN_REFAMP to 1 during power down
 *
 * Hydra_Software_Devel/2   8/19/08 10:27a garylin
 * PR44622: add power down/up clockgen for frontend AST0, AST1, QPSK, FTM
 *
 * Hydra_Software_Devel/1   7/7/08 4:25p garylin
 * PR44622: initial version, ported from 7325
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
#include "bchp_clk.h"
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
#include "bchp_sds_0.h"
#include "bchp_sds_1.h"

#include "bpwr.h"
#include "bpwr_priv.h"
#include "bpwr_mem.h"

#if ((BCHP_CHIP==7400 && BCHP_VER>=BCHP_VER_D0) || BCHP_CHIP==7420)
#define RFM_DUAL_DAC 1    /* RFM cores with dual DAC output */
#endif

BDBG_MODULE(BPWR);

/* List of modules relevant to this chip.
Don't add unused ids here. */
static struct 
{
    BSTD_Module moduleId;
    char        *moduleText;
} g_ModuleList[] =
{
    {BSTD_HDM_ID,"HDM"},
    {BSTD_RFM_ID,"RFM"},
    {BSTD_SCD_ID,"SCD"},
    {BSTD_VBI_ID,"VBI"},
    {BSTD_VDC_ID,"VDC"},
    {BSTD_XPT_ID,"XPT"},
    {BSTD_SDS_ID,"SDS"},
    {BSTD_VSB_ID,"VSB"},
    {BSTD_DMA_ID,"DMA"},
    {BSTD_LNA_ID,"LNA"},
    {BSTD_GRC_ID,"GRC"},
    {BSTD_XVD_ID,"XVD"},
    {BSTD_RAP_ID,"RAP"},
    {BSTD_AST_ID,"AST"},
    {BSTD_UHF_ID,"UHF"},
    {BSTD_MDM_ID,"MDM"},
    {BSTD_NET_ID,"NET"},
    {BSTD_FTM_ID,"FTM"},
    {BSTD_AST1_ID,"AST1"},
    {BSTD_QPSK_ID,"QPSK"},
    {0          ,"<null>"}
};

BERR_Code BPWR_P_Init(BPWR_P_Context    *pPwr )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(pPwr, BPWR_P_Context);

    for (i=0;i<BPWR_MAX_MODULES && g_ModuleList[i].moduleId;i++) {
        pPwr->modules[i].moduleId = g_ModuleList[i].moduleId;
        pPwr->modules[i].moduleText = g_ModuleList[i].moduleText;
        pPwr->modules[i].mode = BPWR_Mode_ePowerUndefined;
    }
    /* if we assert, then BPWR_MAX_MODULES must be increased. it's an internal failure, so assert. */
    BDBG_ASSERT(i < BPWR_MAX_MODULES);

    if (pPwr->hChip == NULL) {
        BERR_Code rc;

        /* BPWR_Open is being called before BCHP_Open. This code must power up the minimum number of modules to allow
        Magnum to initialize safely. This usually means ensuring that every L2 interrupt register can be accessed safely. */
        BSTD_Module initModuleList[7] = {BSTD_HDM_ID, BSTD_VDC_ID, BSTD_VBI_ID, BSTD_XVD_ID, BSTD_AST_ID, BSTD_AST1_ID, BSTD_RFM_ID};
        rc = BPWR_SetModes(pPwr, initModuleList, 7, BPWR_Mode_ePowerUp);
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
            ulRegData = BCHP_CLK_PM_CTRL_DIS_QPSK_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_FTM_ID))
        {
            BDBG_MSG(("Power Down: FTM"));
            ulRegData = BCHP_CLK_PM_CTRL_DIS_FTM_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, ulRegData);
        }


        if (BPWR_P_IsModuleChanging(pPwr, BSTD_AST1_ID))
        {
            BDBG_MSG(("Power Down: AST1"));
            ulRegData = BCHP_CLK_PM_CTRL_DIS_SDS1_108M_CLK_MASK |
            BCHP_CLK_PM_CTRL_DIS_AFEC1_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,ulRegData);

            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_TFEC1_216M_CLK_MASK |
            BCHP_CLK_PM_CTRL_2_DIS_AFEC1_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_AST_ID))
        {
            BDBG_MSG(("Power Down: AST"));
            ulRegData = BCHP_CLK_PM_CTRL_DIS_SDS0_108M_CLK_MASK |
            BCHP_CLK_PM_CTRL_DIS_AFEC0_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,ulRegData);

            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_TFEC0_216M_CLK_MASK |
            BCHP_CLK_PM_CTRL_2_DIS_AFEC0_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            /* From 7405 */
            /* turn on RFM clock before attempting reg access */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_RFM_108M_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

            /* This sequence is copied from RFM PI's power down sequence. 
            Required to support power down via either PWR or RFM. */
            BDBG_MSG(("Power Down: RFM"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL);
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
                         BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_AUX_MASK |
                         BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC0_BG_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC01_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC01_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC01_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC02_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC02_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC02_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC03_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC03_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC03_CTRL_REG, ulRegData);

#if 0
            ulRegData = BCHP_CLK_PM_CTRL_DIS_TDAC_0_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,ulRegData);
#endif
            /* TDAC_1: step 75 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_REFAMP_MASK |
                         BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_AUX_MASK |
                         BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC11_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC11_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC11_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC12_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC12_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC12_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC13_CTRL_REG);
            ulRegData |= BCHP_MISC_TDAC13_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC13_CTRL_REG, ulRegData);

            ulRegData = BCHP_CLK_PM_CTRL_DIS_TDAC_1_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,ulRegData);
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
            ulRegData = BCHP_CLK_PM_CTRL_DIS_HDMI_108M_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            BDBG_MSG(("Power Down: RFM CLK"));
            /* step 24 */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_RFM_108M_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Down: RAP CLK"));
            /* step 84 */
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_AUD_DSP_PROG_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,ulRegData);

            /* step 85, 87 */
            ulRegData = (BCHP_CLK_PM_CTRL_DIS_AIO_108M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_DIS_RPTD_108M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, ulRegData);

            /* step 86, 88 */
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_AIO_216M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_2_DIS_RPTD_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,ulRegData);
        }

#if 0 /* JRUBIO TEST */
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: VDC CLK"));
            /* step 30, 32, 34, 39 */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_VEC_DAC_108M_CLK_MASK |   /* step 30 */
                        BCHP_CLK_PM_CTRL_DIS_VEC_108M_CLK_MASK |       /* step 30 */
                        BCHP_CLK_PM_CTRL_DIS_BVND_108M_CLK_MASK |  /* step 32 */
                        BCHP_CLK_PM_CTRL_DIS_BVNM_108M_CLK_MASK |  /* step 34 */
                        BCHP_CLK_PM_CTRL_DIS_BVNE_108M_CLK_MASK;       /* step 39 */
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);

            /* step 31, 33, 35, 38 */
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_VEC_216M_CLK_MASK | /* step 31 */
                        BCHP_CLK_PM_CTRL_2_DIS_BVND_216M_CLK_MASK |    /* step 33 */
                        BCHP_CLK_PM_CTRL_2_DIS_BVNM_216M_CLK_MASK |    /* step 35 */
                        BCHP_CLK_PM_CTRL_2_DIS_BVNE_216M_CLK_MASK; /* step 38 */
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,ulRegData);

            /* step 36 */
            ulRegData = 0; /* all reserved bit need to be 0 */
            ulRegData |= BCHP_FMISC_SOFT_RESET_RDC_MASK |
                         BCHP_FMISC_SOFT_RESET_VFD_1_MASK |
                         BCHP_FMISC_SOFT_RESET_VFD_0_MASK |
                         BCHP_FMISC_SOFT_RESET_MFD_0_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET, ulRegData);

            /* step 37 */
           ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData |= BCHP_CLK_MISC_DIS_BVNE_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);
        }
#endif

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XVD_ID))
        {
            BDBG_MSG(("Power Down: XVD"));
            ulRegData =  BCHP_CLK_PM_CTRL_DIS_AVD0_108M_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);

            ulRegData = BCHP_CLK_PM_CTRL_1_DIS_AVD0_PROG_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_1,ulRegData,ulRegData);


            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_AVD_216M_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XPT_ID))
        {
            BDBG_MSG(("Power Down: XPT"));
            /* step 104: XPT */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC); 
            ulRegData |= BCHP_CLK_MISC_DIS_XPT_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);

            /* ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = BCHP_CLK_PM_CTRL_DIS_XPT_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,ulRegData);

            /* ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1); */
            ulRegData = BCHP_CLK_PM_CTRL_1_DIS_XPT_27M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_54M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_81M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_20P25M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_40P5M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData,ulRegData);

            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_XPT_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_MDM_ID))
        {
            BDBG_MSG(("Power Down: Soft Modem"));
            /* step 108: MDM */
            ulRegData = BCHP_CLK_PM_CTRL_1_DIS_SUN_SM_27M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_SUN_DAA_32P4M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_UHF_ID))
        {
            BDBG_MSG(("Power Down: UHF, Automic Power for UHF Analog Circuit"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData |=  (BCHP_CLK_MISC_DIS_ANA_UHFR_CLK_MASK |
                         BCHP_CLK_MISC_DIS_DIGI_UHFR_CLK_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);

            /* Automatic Power for Analog Circuit*/
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1);
            ulRegData &= ~(BCHP_UHFR_GLBL_AUTOPD1_AUTO_PD_EN_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1, ulRegData);
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

            ulRegData = BCHP_VCXO_CTL_MISC_AVD_CTRL_POWERDOWN_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_VCXO_CTL_MISC_AVD_CTRL,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Down: bRAP VXCO"));
            /* step 62, 63, 64 */
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
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData |=  BCHP_CLK_MISC_DIS_XPT_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);
        }
    }
    /*
      *============================================================
      *                Power Up
      *============================================================
      */
    else
    {
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
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
                ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL);
                ulRegData &= ~BCHP_VCXO_CTL_MISC_AC1_CTRL_POWERDOWN_MASK;
                BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL, ulRegData);

            }


                ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL);
                ulRegData &= ~BCHP_VCXO_CTL_MISC_AC0_CTRL_POWERDOWN_MASK;
                BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL, ulRegData);
        }

        /* Power on all VCXO */
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_SCD_ID) ||
            (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID)) ||
            BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Up: VCXO"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData &= ~BCHP_CLK_MISC_DIS_VEC_VCXO_656_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData &=  ~BCHP_CLK_MISC_DIS_XPT_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);
        }

        if  (BPWR_P_IsModuleChanging(pPwr, BSTD_UHF_ID))
        {
            BDBG_MSG(("Bring  UHF To Automatic Power Up and keep it there"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1);
            ulRegData &= ~BCHP_UHFR_GLBL_AUTOPD1_AUTO_PD_EN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData &= ~(BCHP_CLK_MISC_DIS_DIGI_UHFR_CLK_MASK |
                         BCHP_CLK_MISC_DIS_ANA_UHFR_CLK_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_MDM_ID))
        {
            BDBG_MSG(("Power Up: Soft Modem"));
            /* step 108: MDM */
            ulRegData = (BCHP_CLK_PM_CTRL_1_DIS_SUN_SM_27M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_SUN_DAA_32P4M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData,0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XPT_ID))
        {
            BDBG_MSG(("Power Up: XPT"));
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_XPT_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,0);


            ulRegData = (BCHP_CLK_PM_CTRL_1_DIS_XPT_27M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_54M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_81M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_20P25M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_40P5M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData,0);

            ulRegData = BCHP_CLK_PM_CTRL_DIS_XPT_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,0);

            /* step 104: XPT */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData &= ~BCHP_CLK_MISC_DIS_XPT_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XVD_ID))
        {
            BDBG_MSG(("Power Up: XVD CLK"));
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_AVD_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,0);

            ulRegData = BCHP_CLK_PM_CTRL_1_DIS_AVD0_PROG_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData, 0);

            /* step 101: AVD*/
            ulRegData = BCHP_CLK_PM_CTRL_DIS_AVD0_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
            ulRegData &= ~BCHP_VCXO_CTL_MISC_VC0_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ulRegData);

            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AVD_CTRL, BCHP_VCXO_CTL_MISC_AVD_CTRL_POWERDOWN_MASK, 0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Up: VDC CLK"));

            /* step 100 */
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_GFX_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,0);

            /* step 99: GFX */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_GFX_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,0);

            /* step 98 */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_BVNE_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,0);

            /* step 97 */
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_BVNE_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,0);

            /* step 96: BVNE */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData &= ~BCHP_CLK_MISC_DIS_BVNE_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);

            /* step 95 */
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_BVNM_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,0);

            /* step 94: BVNM */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_BVNM_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,0);

            /* step 93 */
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_BVND_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData,0);

            /* step 92 */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_BVND_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,0);

            /* step 91: BVND */
      /*
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET);
            ulRegData &= ~(BCHP_FMISC_SOFT_RESET_RDC_MASK |
                         BCHP_FMISC_SOFT_RESET_VFD_1_MASK |
                         BCHP_FMISC_SOFT_RESET_VFD_0_MASK |
                         BCHP_FMISC_SOFT_RESET_MFD_0_MASK); */
            ulRegData = 0;
            /* BREG_Write32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET, ulRegData); */

            /* step 90 */
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_VEC_216M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData, 0);

            /* step 89: VEC */
            ulRegData = (BCHP_CLK_PM_CTRL_DIS_VEC_108M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_DIS_VEC_DAC_108M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Up: RAP CLK"));
            /* step 86, 88 */
            ulRegData = (BCHP_CLK_PM_CTRL_2_DIS_AIO_216M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_2_DIS_RPTD_216M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData, 0);

            /* step 85, 87 */
            ulRegData = (BCHP_CLK_PM_CTRL_DIS_AIO_108M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_DIS_RPTD_108M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

            /* step 84 */
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_AUD_DSP_PROG_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData, 0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_HDM_ID))
        {
            BDBG_MSG(("Power Up: HDM"));

            /* step 80.5 */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_HDMI_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

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
            ulRegData = BCHP_CLK_PM_CTRL_DIS_TDAC_1_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC13_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC13_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC13_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC12_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC12_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC12_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC11_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC11_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC11_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG);
            ulRegData &= ~(BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_REFAMP_MASK |
                         BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_AUX_MASK |
                         BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_CORE_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG, ulRegData);

            /* TDAC_0 */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_TDAC_0_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC03_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC03_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC03_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC02_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC02_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC02_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC01_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC01_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC01_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC0_BG_CTRL_REG);
            ulRegData &= ~(BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_REFAMP_MASK |
                         BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_AUX_MASK |
                         BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_CORE_MASK);
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

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            BDBG_MSG(("Power Up: RFM"));

            /* step 68 */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_RFM_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL);
            ulRegData &= ~(BCHP_RFM_SYSCLK_CLKCTL_VREG_OFF_MASK |
                         BCHP_RFM_SYSCLK_CLKCTL_RFMCLK_OFF_MASK |
                         BCHP_RFM_SYSCLK_CLKCTL_BGCORE_OFF_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL);
            ulRegData &= ~(BCHP_RFM_SYSCLK_DACCTL_DAC_PWRDN_MASK |
                         BCHP_RFM_SYSCLK_DACCTL_BIAS50U_OFF_MASK |
                         BCHP_RFM_SYSCLK_DACCTL_BIASLOG_OFF_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_PLL_MISC1);
            ulRegData &= ~(BCHP_RFM_SYSCLK_PLL_MISC1_NDIV_PWRDN_MASK |
                         BCHP_RFM_SYSCLK_PLL_MISC1_REFCOMP_PWRDN_MASK |
                         BCHP_RFM_SYSCLK_PLL_MISC1_PWRDN_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_PLL_MISC1, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_RFM_PHASEITP_CTL);
            ulRegData &= ~BCHP_RFM_SYSCLK_RFM_PHASEITP_CTL_PHASEITP_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_RFM_PHASEITP_CTL, ulRegData);

      /* step 63 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL);
            ulRegData &= ~BCHP_RFM_SYSCLK_CLKCTL_CLK_OFF_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_AST_ID))
        {
            BDBG_MSG(("Power Up: AST"));

            ulRegData = (BCHP_CLK_PM_CTRL_DIS_SDS0_108M_CLK_MASK |
            BCHP_CLK_PM_CTRL_DIS_AFEC0_108M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

            ulRegData = (BCHP_CLK_PM_CTRL_2_DIS_TFEC0_216M_CLK_MASK |
            BCHP_CLK_PM_CTRL_2_DIS_AFEC0_216M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData, 0);

#if 1
            /* required for L2 access */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_SDS_0_SPLL_PWRDN);
            ulRegData = 0x20;
            BREG_Write32(pPwr->hRegister, BCHP_SDS_0_SPLL_PWRDN, ulRegData);
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_SDS_0_SPLL_MODE);
            ulRegData &= ~0x04;
            BREG_Write32(pPwr->hRegister, BCHP_SDS_0_SPLL_MODE, ulRegData);
#endif
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_AST1_ID))
        {
            BDBG_MSG(("Power Up: AST1"));
            ulRegData = (BCHP_CLK_PM_CTRL_DIS_SDS1_108M_CLK_MASK |
            BCHP_CLK_PM_CTRL_DIS_AFEC1_108M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

            ulRegData = (BCHP_CLK_PM_CTRL_2_DIS_TFEC1_216M_CLK_MASK |
            BCHP_CLK_PM_CTRL_2_DIS_AFEC1_216M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData, 0);

#if 1
            /* required for L2 access */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_SDS_1_SPLL_PWRDN);
            ulRegData = 0x20;
            BREG_Write32(pPwr->hRegister, BCHP_SDS_1_SPLL_PWRDN, ulRegData);
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_SDS_1_SPLL_MODE);
            ulRegData &= ~0x04;
            BREG_Write32(pPwr->hRegister, BCHP_SDS_1_SPLL_MODE, ulRegData);
#endif
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_FTM_ID))
        {
            BDBG_MSG(("Power Up: FTM"));
            ulRegData = BCHP_CLK_PM_CTRL_DIS_FTM_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_QPSK_ID))
        {
            BDBG_MSG(("Power Up: QPSK"));
            ulRegData = BCHP_CLK_PM_CTRL_DIS_QPSK_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);
        }
    }

    BDBG_LEAVE(BPWR_P_SetPowerMode);
    return eStatus;
}

/* End of file */

