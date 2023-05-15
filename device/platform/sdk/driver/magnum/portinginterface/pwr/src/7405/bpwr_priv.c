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
 * $brcm_Date: 9/21/12 7:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwr/src/7405/bpwr_priv.c $
 * 
 * Hydra_Software_Devel/1   9/21/12 7:13p nickh
 * SW7425-3971: Add chip-specific private files
 * 
 * Hydra_Software_Devel/14   5/11/10 11:31a jtna
 * SW7405-4314: Coverity Defect ID:16460 OVERRUN_STATIC
 * 
 * Hydra_Software_Devel/13   7/15/09 4:17p katrep
 * PR56518: Changed the powerdown sequence for UHF,VDC clocks. During
 * powerdown core registers should be changed before clocgen registers to
 * avoid gisb errors.
 * 
 * Hydra_Software_Devel/12   7/14/09 7:40p katrep
 * PR56518: Changed power sequence for BVN and UHF modules
 * 
 * Hydra_Software_Devel/11   7/8/09 5:51p jtna
 * PR56665: refactor RFM power management
 * 
 * Hydra_Software_Devel/10   7/6/09 10:26a jtna
 * PR56435: add RFM to init list
 * 
 * Hydra_Software_Devel/9   7/2/09 11:11a katrep
 * PR56528: atomics access for SCXO_CTL_MISC_AVD
 * 
 * Hydra_Software_Devel/8   7/1/09 5:18p katrep
 * PR56528: atomic access should be used for CLK_PM_CTRL and
 * SCXO_CTL_MISC_AVD registers
 * 
 * Hydra_Software_Devel/7   6/25/09 2:28p erickson
 * PR50407: remove bad ID from initModuleList so that BPWR_Open succeeds
 *
 * Hydra_Software_Devel/6   6/4/09 4:26p katrep
 * PR55685: reworked power pi for 7405
 *
 * Hydra_Software_Devel/5   11/4/08 2:40p garylin
 * PR42849: remove compile warning for un-init variables
 *
 * Hydra_Software_Devel/4   8/26/08 9:43a garylin
 * PR42849: don't set PWRDN_CEC and PWRDN_REFAMP to 1 during power down
 *
 * Hydra_Software_Devel/3   6/19/08 3:34p garylin
 * PR42849: add device Id UPG for Sundry and MDM for soft modem
 *
 * Hydra_Software_Devel/2   6/10/08 2:16p katrep
 * PR42324:Update for 7405
 *
 * Hydra_Software_Devel/1   5/20/08 1:56p garylin
 * PR42849: init version, ported from 7400.
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
#include "bchp_memc_gfx_misc.h"
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

#include "bpwr.h"
#include "bpwr_priv.h"
#include "bpwr_mem.h"

#if ((BCHP_CHIP==7400 && BCHP_VER>=BCHP_VER_D0) || BCHP_CHIP==7420)
#define RFM_DUAL_DAC 1    /* RFM cores with dual DAC output */
#endif

BDBG_MODULE(BPWR);

/* List of modules relevant to this chip.
Don't add unused ids here. */
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
    BSTD_GRC_ID, /* no nothing because GFX is tied to MEMC */
#if (BCHP_CHIP==7405 && BCHP_VER >= BCHP_VER_C0 )
    BSTD_P3D_ID,
#endif
    BSTD_XVD_ID,
    BSTD_RAP_ID,
    BSTD_AST_ID,
    BSTD_UHF_ID,
    BSTD_MDM_ID,
    BSTD_NET_ID, /* handled by pmlib */
    0 /* last item must be 0. used for loop termination */
};

BERR_Code BPWR_P_Init(BPWR_P_Context    *pPwr )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(pPwr, BPWR_P_Context);

    /* coverity[overrun-local] */
    for (i=0;i<BPWR_MAX_MODULES && g_ModuleList[i];i++)
    {
        pPwr->modules[i].moduleId = g_ModuleList[i];
        pPwr->modules[i].mode = BPWR_Mode_ePowerUndefined;
    }
    /* if we assert, then BPWR_MAX_MODULES must be increased. it's an internal failure, so assert. */
    BDBG_ASSERT(i < BPWR_MAX_MODULES);

    if (pPwr->hChip == NULL)
    {
        BERR_Code rc;

        /* BPWR_Open is being called before BCHP_Open. This code must power up the minimum number of modules to allow
        Magnum to initialize safely. This usually means ensuring that every L2 interrupt register can be accessed safely. */
        BSTD_Module initModuleList[] = {BSTD_HDM_ID, BSTD_VDC_ID, BSTD_VBI_ID, BSTD_XVD_ID, BSTD_AST_ID, BSTD_RFM_ID};
        rc = BPWR_SetModes(pPwr, initModuleList, sizeof(initModuleList)/sizeof(BSTD_Module), BPWR_Mode_ePowerUp);
        if (rc) return BERR_TRACE(rc);
        rc = BPWR_ApplyChanges(pPwr);
        if (rc) return BERR_TRACE(rc);
    }

    return 0;
}

BERR_Code BPWR_P_Destroy(BPWR_P_Context *pPwr)
{
    BSTD_UNUSED(pPwr);
    return 0;
}

static bool BPWR_P_IsModuleChanging(const BPWR_P_Context *pPwr, BSTD_Module moduleId)
{
    unsigned i;
    for (i=0;i<BPWR_MAX_MODULES;i++)
    {
        if (pPwr->modules[i].moduleId == moduleId)
        {
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
    if (count != 0 && count != 3)
    {
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
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
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
            /* TDAC_0: step 10 */
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

            /* step 47 XXX */

            /* TDAC_1: step 14 */
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

            /* step 48 XXX */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_HDM_ID))
        {
            BDBG_MSG(("Power Down: HDM"));
            /* step 18, do we need to keep CEC Rx channel up? */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL);
            ulRegData |= BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_0_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_1_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_2_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_CK_PWRDN_MASK |
                         /*             BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_CEC_MASK |  4 */
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PLL_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_REF_COMP_PWRDN_MASK |
                         BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_BG_MASK |
                         /*              BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_REFAMP_MASK |  8 */
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

            /* step 21 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2);
            ulRegData |= BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2_I_PWRDN_CH2_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, ulRegData);

            /* may or may not, XXX */
            /* ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = BCHP_CLK_PM_CTRL_DIS_HDMI_108M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData); */
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            BDBG_MSG(("Power Down: RFM CLK"));
            /* step 24 */
            /* ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = BCHP_CLK_PM_CTRL_DIS_RFM_108M_CLK_MASK;
            /* BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData); */
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);
        }


        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Down: bRAP CLK"));
            /* step 26, 28 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = BCHP_CLK_PM_CTRL_DIS_AIO_108M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_DIS_RPTD_108M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);

            /* step 25, 27, 29 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2);*/
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_AUD_DSP_PROG_CLK_MASK |
                         BCHP_CLK_PM_CTRL_2_DIS_AIO_216M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_2_DIS_RPTD_216M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: VDC CLK"));
            /* step 36 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET);
            ulRegData |= /* BCHP_FMISC_SOFT_RESET_HD_DVI0_MASK |
                         BCHP_FMISC_SOFT_RESET_ITU_R656_DEC1_MASK | */
                         BCHP_FMISC_SOFT_RESET_ITU_R656_DEC0_MASK |
                         BCHP_FMISC_SOFT_RESET_RDC_MASK |
                         BCHP_FMISC_SOFT_RESET_VFD_3_MASK |
                         BCHP_FMISC_SOFT_RESET_VFD_2_MASK |
                         BCHP_FMISC_SOFT_RESET_VFD_1_MASK |
                         BCHP_FMISC_SOFT_RESET_VFD_0_MASK |
                         BCHP_FMISC_SOFT_RESET_MFD_1_MASK |
                         BCHP_FMISC_SOFT_RESET_MFD_0_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET, ulRegData);

            /* step 30, 32, 34, 39 */
            /* ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = BCHP_CLK_PM_CTRL_DIS_VEC_DAC_108M_CLK_MASK |   /* step 30 */
                        BCHP_CLK_PM_CTRL_DIS_VEC_108M_CLK_MASK |       /* step 30 */
                        BCHP_CLK_PM_CTRL_DIS_BVND_108M_CLK_MASK |  /* step 32 */
                        BCHP_CLK_PM_CTRL_DIS_BVNM_108M_CLK_MASK |  /* step 34 */
                        BCHP_CLK_PM_CTRL_DIS_BVNE_108M_CLK_MASK;       /* step 39 */
            /* BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);

            /* step 31, 33, 35, 38 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2);*/
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_VEC_216M_CLK_MASK | /* step 31 */
                        BCHP_CLK_PM_CTRL_2_DIS_BVND_216M_CLK_MASK |    /* step 33 */
                        BCHP_CLK_PM_CTRL_2_DIS_BVNM_216M_CLK_MASK |    /* step 35 */
                        BCHP_CLK_PM_CTRL_2_DIS_BVNE_216M_CLK_MASK; /* step 38 */
            /* BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,ulRegData);

            /* step 37 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData |= BCHP_CLK_MISC_DIS_BVNE_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XVD_ID))
        {
            BDBG_MSG(("Power Down: XVD"));
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData =  BCHP_CLK_PM_CTRL_DIS_AVD0_108M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);

            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1);*/
            ulRegData = BCHP_CLK_PM_CTRL_1_DIS_AVD0_PROG_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_1,ulRegData,ulRegData);


            /* ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2); */
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_AVD_216M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XPT_ID))
        {
            BDBG_MSG(("Power Down: bXPT"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData |=  BCHP_CLK_MISC_DIS_XPT_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);

            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData =  BCHP_CLK_PM_CTRL_DIS_XPT_108M_CLK_MASK;
            /* BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);

            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1);*/
            ulRegData = BCHP_CLK_PM_CTRL_1_DIS_XPT_40P5M_CLK_MASK |
                        BCHP_CLK_PM_CTRL_1_DIS_XPT_20P25M_CLK_MASK |
                        BCHP_CLK_PM_CTRL_1_DIS_XPT_81M_CLK_MASK |
                        BCHP_CLK_PM_CTRL_1_DIS_XPT_54M_CLK_MASK |
                        BCHP_CLK_PM_CTRL_1_DIS_XPT_27M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_1,ulRegData,ulRegData);

            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2);*/
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_XPT_216M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,ulRegData);
        }


        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: bVDC DAC CLK"));
            /* step 47, 48 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = BCHP_CLK_PM_CTRL_DIS_TDAC_0_108M_CLK_MASK |
                        BCHP_CLK_PM_CTRL_DIS_TDAC_1_108M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_MDM_ID))
        {
            BDBG_MSG(("Power Down: Modem"));
            /* step 49 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1);*/
            ulRegData = BCHP_CLK_PM_CTRL_1_DIS_SUN_SM_27M_CLK_MASK |
                        BCHP_CLK_PM_CTRL_1_DIS_SUN_DAA_32P4M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_1,ulRegData,ulRegData);

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_UHF_ID))
        {
            BDBG_MSG(("Power Down: UHF"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1);
            ulRegData &= BCHP_UHFR_GLBL_AUTOPD1_reserved_for_eco0_MASK;
            ulRegData |= BCHP_UHFR_GLBL_AUTOPD1_AUTO_PD_EN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1, ulRegData);
#if 0
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD2);
            ulRegData &= BCHP_UHFR_GLBL_AUTOPD2_reserved_for_eco0_MASK;
            ulRegData |= BCHP_UHFR_GLBL_AUTOPD2_AUTO_PD_EN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD2, ulRegData);
#endif
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData |= BCHP_CLK_MISC_DIS_ANA_UHFR_CLK_MASK |
                         BCHP_CLK_MISC_DIS_DIGI_UHFR_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);
        }

        /*
          *    VCXO
          */
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: bVDC VCXO"));
            /* step 60, 61 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
            ulRegData |= BCHP_VCXO_CTL_MISC_VC0_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ulRegData);

            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AVD_CTRL);**/
            ulRegData = BCHP_VCXO_CTL_MISC_AVD_CTRL_POWERDOWN_MASK;
            /* BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AVD_CTRL, ulRegData); */
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

        /*
          *    VCXO
          */
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Up: bVDC VCXO"));
            /* step 60, 61 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
            ulRegData &= ~BCHP_VCXO_CTL_MISC_VC0_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ulRegData);

            /* ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AVD_CTRL);*/
            ulRegData = BCHP_VCXO_CTL_MISC_AVD_CTRL_POWERDOWN_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AVD_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_VCXO_CTL_MISC_AVD_CTRL,ulRegData,0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Up: bRAP VCXO"));
            /* step 62, 63, 64 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL);
            ulRegData &= ~BCHP_VCXO_CTL_MISC_AC0_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL);
            ulRegData &= ~BCHP_VCXO_CTL_MISC_AC1_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AUDDSP_CTRL);
            ulRegData &= ~BCHP_VCXO_CTL_MISC_AUDDSP_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AUDDSP_CTRL, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_SCD_ID))
        {
            BDBG_MSG(("Power Up: VCXO bSCD"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL);
            ulRegData &= ~BCHP_VCXO_CTL_MISC_AC1_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL, ulRegData);
        }

        /* Power on all VCXO */
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_SCD_ID) ||
            (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID)) ||
            BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Up: VCXO"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData &=  ~BCHP_CLK_MISC_DIS_XPT_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_UHF_ID))
        {
            BDBG_MSG(("Power Up: UHF"));

             ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData &= ~(BCHP_CLK_MISC_DIS_ANA_UHFR_CLK_MASK |
                           BCHP_CLK_MISC_DIS_DIGI_UHFR_CLK_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1);
            ulRegData &= BCHP_UHFR_GLBL_AUTOPD1_reserved_for_eco0_MASK;
            ulRegData &= ~BCHP_UHFR_GLBL_AUTOPD1_AUTO_PD_EN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD1, ulRegData);
#if 0
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD2);
            ulRegData &= BCHP_UHFR_GLBL_AUTOPD2_reserved_for_eco0_MASK;
            ulRegData &= ~BCHP_UHFR_GLBL_AUTOPD2_AUTO_PD_EN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_UHFR_GLBL_AUTOPD2, ulRegData);
#endif
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_MDM_ID))
        {
            BDBG_MSG(("Power Up: Modem"));
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1);*/
            ulRegData = (BCHP_CLK_PM_CTRL_1_DIS_SUN_SM_27M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_SUN_DAA_32P4M_CLK_MASK);
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_1,ulRegData,0);
        }


        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Up: VDC DAC CLK"));
            /* step 47, 48 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = (BCHP_CLK_PM_CTRL_DIS_TDAC_0_108M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_DIS_TDAC_1_108M_CLK_MASK);
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,0);

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XPT_ID))
        {
            BDBG_MSG(("Power Up: XPT"));
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData =  BCHP_CLK_PM_CTRL_DIS_XPT_108M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,0);

            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1);*/
            ulRegData = (BCHP_CLK_PM_CTRL_1_DIS_XPT_40P5M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_20P25M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_81M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_54M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_XPT_27M_CLK_MASK);
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_1,ulRegData,0);

            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2);*/
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_XPT_216M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,0);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData &=  ~BCHP_CLK_MISC_DIS_XPT_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XVD_ID))
        {
            BDBG_MSG(("Power Up: bXVD CLK"));

            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2);*/
            ulRegData = BCHP_CLK_PM_CTRL_2_DIS_AVD_216M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,0);

            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1);*/
            ulRegData = BCHP_CLK_PM_CTRL_1_DIS_AVD0_PROG_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_1,ulRegData,0);

            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData =  BCHP_CLK_PM_CTRL_DIS_AVD0_108M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,0);
            
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Up: VDC CLK"));
            /* step 37 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
            ulRegData &= ~BCHP_CLK_MISC_DIS_BVNE_DGP_CLK_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);


            /* step 30, 32, 34, 39 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = (BCHP_CLK_PM_CTRL_DIS_VEC_DAC_108M_CLK_MASK | /* step 30 */
                         BCHP_CLK_PM_CTRL_DIS_VEC_108M_CLK_MASK | /* step 30 */
                         BCHP_CLK_PM_CTRL_DIS_BVND_108M_CLK_MASK |    /* step 32 */
                         BCHP_CLK_PM_CTRL_DIS_BVNM_108M_CLK_MASK |    /* step 34 */
                         BCHP_CLK_PM_CTRL_DIS_BVNE_108M_CLK_MASK);        /* step 39 */
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,0);

            /* step 31, 33, 35, 38 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2);*/
            ulRegData = (BCHP_CLK_PM_CTRL_2_DIS_VEC_216M_CLK_MASK |   /* step 31 */
                         BCHP_CLK_PM_CTRL_2_DIS_BVND_216M_CLK_MASK |  /* step 33 */
                         BCHP_CLK_PM_CTRL_2_DIS_BVNM_216M_CLK_MASK |  /* step 35 */
                         BCHP_CLK_PM_CTRL_2_DIS_BVNE_216M_CLK_MASK);  /* step 38 */
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,0);

            /* step 36 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET);
            ulRegData &= ~(/* BCHP_FMISC_SOFT_RESET_HD_DVI0_MASK |
                           BCHP_FMISC_SOFT_RESET_ITU_R656_DEC1_MASK | */
                           BCHP_FMISC_SOFT_RESET_ITU_R656_DEC0_MASK |
                           BCHP_FMISC_SOFT_RESET_RDC_MASK |
                           BCHP_FMISC_SOFT_RESET_VFD_3_MASK |
                           BCHP_FMISC_SOFT_RESET_VFD_2_MASK |
                           BCHP_FMISC_SOFT_RESET_VFD_1_MASK |
                           BCHP_FMISC_SOFT_RESET_VFD_0_MASK |
                           BCHP_FMISC_SOFT_RESET_MFD_1_MASK |
                           BCHP_FMISC_SOFT_RESET_MFD_0_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET, ulRegData);

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Up: RAP CLK"));
            /* step 26, 28 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = (BCHP_CLK_PM_CTRL_DIS_AIO_108M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_DIS_RPTD_108M_CLK_MASK);
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,0);

            /* step 25, 27, 29 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2);*/
            ulRegData = (BCHP_CLK_PM_CTRL_2_DIS_AUD_DSP_PROG_CLK_MASK |
                         BCHP_CLK_PM_CTRL_2_DIS_AIO_216M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_2_DIS_RPTD_216M_CLK_MASK);
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            BDBG_MSG(("Power Up: bRFM CLK"));
            /* step 24 */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = BCHP_CLK_PM_CTRL_DIS_RFM_108M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,0);
            /* for power up of RFM core via PWR PI, we only turn on the top-level clock to ensure register access. 
               RFM functionality is not guaranteed. RFM users should use the power down/up functions in RFM PI 
               for power management with RFM functionality */
        }
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_HDM_ID))
        {
            BDBG_MSG(("Power Up: HDM"));
            /* may or may not, XXX */
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL);*/
            ulRegData = BCHP_CLK_PM_CTRL_DIS_HDMI_108M_CLK_MASK;
            /*BREG_Write32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData);*/
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,0);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0);
            ulRegData &= ~BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0_RND_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1);
            ulRegData &= ~BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1_I_PWR_DN_CH1_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ulRegData);

            /* step 21 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2);
            ulRegData &= ~BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2_I_PWRDN_CH2_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, ulRegData);

            /* step 18, do we need to keep CEC Rx channel up? */
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
            /* TDAC_0: step 10 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC0_BG_CTRL_REG);
            ulRegData &= ~(BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_REFAMP_MASK |
                           BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_AUX_MASK |
                           BCHP_MISC_TDAC0_BG_CTRL_REG_PWRDN_CORE_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC0_BG_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC01_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC01_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC01_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC02_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC02_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC02_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC03_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC03_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC03_CTRL_REG, ulRegData);

            /* step 47 XXX */

            /* TDAC_1: step 14 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG);
            ulRegData &= ~(BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_REFAMP_MASK |
                           BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_AUX_MASK |
                           BCHP_MISC_TDAC1_BG_CTRL_REG_PWRDN_CORE_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC1_BG_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC11_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC11_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC11_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC12_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC12_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC12_CTRL_REG, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC13_CTRL_REG);
            ulRegData &= ~BCHP_MISC_TDAC13_CTRL_REG_PWRDN_CORE_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC13_CTRL_REG, ulRegData);


        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Up: bRAP"));
            /* step 9 */
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN);
            ulRegData &= ~BCHP_AIO_MISC_PWRDOWN_DAC0_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN, ulRegData);
        }

    }

    BDBG_LEAVE(BPWR_P_SetPowerMode);
    return eStatus;
}

/* End of file */

