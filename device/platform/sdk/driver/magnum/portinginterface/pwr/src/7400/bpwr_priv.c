/***************************************************************************
 *	   Copyright (c) 2003-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpwr_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/21/12 7:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwr/src/7400/bpwr_priv.c $
 * 
 * Hydra_Software_Devel/1   9/21/12 7:11p nickh
 * SW7425-3971: Add chip-specific private files
 * 
 * Hydra_Software_Devel/15   3/22/10 5:27p mward
 * SW7400-2727:  Corrected bits/registers for power-down XVD clks.
 * 
 * Hydra_Software_Devel/14   3/22/10 5:09p mward
 * SW7400-2727: BPWR_P_SetPowerMode() needs to use the same set of clock
 * enable/disable bits as is used by
 * XVD for dynamic power management.
 * 
 * Hydra_Software_Devel/13   3/9/10 3:48p mward
 * SW7400-2715: Use compile-time count of g_ModuleList[] elements.
 * 
 * Hydra_Software_Devel/12   7/29/09 6:33p mward
 * PR57145:  Eliminated BREG_AtomicUpdate32 of registers not intended for
 * it.
 * 
 * Hydra_Software_Devel/11   7/28/09 5:08p mward
 * PR57145:  pwr updated as done for 7335.
 * 
 * Hydra_Software_Devel/10   2/27/09 5:24p mward
 * PR52632: Compiler warnings in bpwr_priv.c when built with MODE=proxy
 * 
 * Hydra_Software_Devel/9   8/26/08 9:44a garylin
 * PR41981: don't set PWRDN_CEC and PWRDN_REFAMP to 1 during power down
 * 
 * Hydra_Software_Devel/8   7/3/08 9:10a garylin
 * PR41981: Power down VCXO when smartcard is powered down
 * 
 * Hydra_Software_Devel/7   4/28/08 5:59p mward
 * PR41981: Order of RFM power-down power down optimized as recommended.
 * 
 * Hydra_Software_Devel/6   4/25/08 6:20p mward
 * PR41981: Power on RFM in reverse-order of power-down.
 * 
 * Hydra_Software_Devel/5   2/21/08 2:24p jessem
 * PR 38367: Corrected USB power down/up.
 * 
 * Hydra_Software_Devel/4   1/25/08 3:04p jessem
 * PR 38367: Removed unnecessary code and comments.
 * 
 * Hydra_Software_Devel/3   1/15/08 12:02p jessem
 * PR 38367: Added 7400 D0 support.
 * 
 * Hydra_Software_Devel/2   1/4/08 9:37a jessem
 * PR 38367: Changed order of powering up/down MEMC1 and MEMC2. Also
 * corrected handling of return from call to BPWR_P_GetPowerMode().
 * 
 * Hydra_Software_Devel/1   12/20/07 10:41a jessem
 * PR 38367: Initial version.
 * 
 *
 ***************************************************************************/
#include "bstd.h"				 /* standard types */
#include "berr.h"
#include "bstd_ids.h"
#include "bdbg.h"				 /* Dbglib */
#include "bkni.h"				 /* malloc */

#include "bchp_memc_0.h"
#include "bchp_memc_1.h"
#include "bchp_memc_2.h"
#include "bchp_memc_0_ddr.h"
#include "bchp_memc_1_ddr.h"
#include "bchp_memc_2_ddr.h"
#include "bchp_memc16_gfx_misc.h"
#include "bchp_clk.h"
#include "bchp_hdmi_tx_phy.h"
#include "bchp_rfm_sysclk.h"
#include "bchp_aio_misc.h"
#include "bchp_vcxo_ctl_misc.h"
#include "bchp_misc.h"
#include "bchp_bmisc.h"
#include "bchp_mmisc.h"
#include "bchp_fmisc.h"
#include "bchp_usb_ctrl.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_uhfr_glbl.h"

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
	BSTD_AUD_ID,
	BSTD_HDM_ID,
	BSTD_IFD_ID,
#if (BCHP_CHIP==7038) || (BCHP_CHIP==3563)
	BSTD_MVD_ID,
#endif	
	BSTD_QDS_ID,
	BSTD_QOB_ID,
	BSTD_QUS_ID,
	BSTD_RFM_ID,
	BSTD_SCD_ID,
	BSTD_TNR_ID,
	BSTD_VBI_ID,
	BSTD_VDC_ID,
	BSTD_XPT_ID,
	BSTD_I2C_ID,
	BSTD_SPI_ID,
	BSTD_ICP_ID,
	BSTD_IRB_ID,
	BSTD_KIR_ID,
	BSTD_KPD_ID,
	BSTD_LED_ID,
	BSTD_PWM_ID,
	BSTD_URT_ID,
	BSTD_SDS_ID,
	BSTD_VSB_ID,
	BSTD_ENC_ID,
	BSTD_DMA_ID,
	BSTD_GIO_ID,
	BSTD_LNA_ID,
	BSTD_GRC_ID,
	BSTD_P3D_ID,
#if (BCHP_CHIP==7400) || (BCHP_CHIP==7403)		
	BSTD_XVD_ID,
#endif	
	BSTD_ARC_ID,
	BSTD_RAP_ID,
	BSTD_AST_ID,
    BSTD_UHF_ID,
    BSTD_MDM_ID,
	BSTD_ADS_ID,
	BSTD_AOB_ID,
	BSTD_AUS_ID,
	BSTD_XCD_ID,
	BSTD_XCU_ID,
	BSTD_NET_ID,
	BSTD_USB_ID,
	BSTD_ATA_ID,
    0
};

BERR_Code BPWR_P_Init(BPWR_P_Context    *pPwr )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(pPwr, BPWR_P_Context);

    for (i=0;i<sizeof(g_ModuleList)/sizeof(BSTD_Module) && g_ModuleList[i];i++) {
        pPwr->modules[i].moduleId = g_ModuleList[i];
        pPwr->modules[i].mode = BPWR_Mode_ePowerUndefined;
    }
    /* if we assert, then BPWR_MAX_MODULES must be increased. it's an internal failure, so assert. */
    BDBG_ASSERT(i < BPWR_MAX_MODULES);

    if (pPwr->hChip == NULL) {
        BERR_Code rc;

        /* BPWR_Open is being called before BCHP_Open. This code must power up the minimum number of modules to allow
        Magnum to initialize safely. This usually means ensuring that every L2 interrupt register can be accessed safely. */
        BSTD_Module initModuleList[] = {BSTD_HDM_ID, BSTD_VDC_ID, BSTD_VBI_ID, BSTD_XVD_ID, BSTD_RFM_ID};
        rc = BPWR_SetModes(pPwr, initModuleList, sizeof(initModuleList)/sizeof(BSTD_Module), BPWR_Mode_ePowerUp);
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
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            /* From 7405 */
            /* turn on RFM clock before attempting reg access */
            ulRegData = BCHP_CLK_PM_CTRL_DIS_RFM_108M_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

            /* This sequence is copied from RFM PI's power down sequence. 
            Required to support power down via either PWR or RFM. */
            BDBG_MSG(("Power Down: RFM"));
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
			ulRegData |= BCHP_RFM_SYSCLK_DACCTL_DAC_PWRDN_MASK;
#if (RFM_DUAL_DAC)
			ulRegData |= BCHP_RFM_SYSCLK_DACCTL_BIAS50U_OFF_MASK |	
						 BCHP_RFM_SYSCLK_DACCTL_BIASLOG_OFF_MASK |
						 BCHP_RFM_SYSCLK_DACCTL_DAC_PWRDN_2_MASK;
#endif
			BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL, ulRegData);
			
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL);
			ulRegData |= BCHP_RFM_SYSCLK_CLKCTL_VREG_OFF_MASK;
#if (RFM_DUAL_DAC)
			ulRegData |= BCHP_RFM_SYSCLK_CLKCTL_RFMCLK_OFF_MASK |	
						 BCHP_RFM_SYSCLK_CLKCTL_BGCORE_OFF_MASK;
#endif
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL, ulRegData);

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            /* HifiDriver */
            BDBG_MSG(("Power Down: RAP"));
			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN);
			ulRegData |= BCHP_AIO_MISC_PWRDOWN_DAC1_MASK |
						 BCHP_AIO_MISC_PWRDOWN_DAC0_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: VDC"));
			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC_BG_CTRL_REG);
			ulRegData |= BCHP_MISC_TDAC_BG_CTRL_REG_PWRDN_MASK |
						 BCHP_MISC_TDAC_BG_CTRL_REG_PWRDN_REFAMP_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC_BG_CTRL_REG, ulRegData);

				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC1_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
				ulRegData &= ~BCHP_MISC_TDAC1_CTRL_REG_PWRUP_BAIS_MASK;
				ulRegData |= BCHP_MISC_TDAC1_CTRL_REG_PWRDN_MASK;
#else
				ulRegData |= BCHP_MISC_TDAC1_CTRL_REG_PWRDN_MASK |
							 BCHP_MISC_TDAC1_CTRL_REG_PWRUP_BAIS_MASK;
#endif
				BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC1_CTRL_REG, ulRegData);

				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC2_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
				ulRegData &= ~BCHP_MISC_TDAC2_CTRL_REG_PWRUP_BAIS_MASK;
				ulRegData |= BCHP_MISC_TDAC2_CTRL_REG_PWRDN_MASK;
#else
				ulRegData |= BCHP_MISC_TDAC2_CTRL_REG_PWRDN_MASK |
							 BCHP_MISC_TDAC2_CTRL_REG_PWRUP_BAIS_MASK;
#endif
				BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC2_CTRL_REG, ulRegData);
					
				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC3_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
				ulRegData &= ~BCHP_MISC_TDAC3_CTRL_REG_PWRUP_BAIS_MASK;
				ulRegData |= BCHP_MISC_TDAC3_CTRL_REG_PWRDN_MASK;
#else				
				ulRegData |= BCHP_MISC_TDAC3_CTRL_REG_PWRDN_MASK |
							 BCHP_MISC_TDAC3_CTRL_REG_PWRUP_BAIS_MASK;
#endif
				BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC3_CTRL_REG, ulRegData);

				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_QDAC_BG_CTRL_REG);
				ulRegData |= BCHP_MISC_QDAC_BG_CTRL_REG_PWRDN_MASK |
							 BCHP_MISC_QDAC_BG_CTRL_REG_PWRDN_REFAMP_MASK;
				BREG_Write32(pPwr->hRegister, BCHP_MISC_QDAC_BG_CTRL_REG, ulRegData);
				
				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_QDAC1_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
				ulRegData &= ~BCHP_MISC_QDAC1_CTRL_REG_PWRUP_BAIS_MASK;
				ulRegData |= BCHP_MISC_QDAC1_CTRL_REG_PWRDN_MASK;
#else
				ulRegData |= BCHP_MISC_QDAC1_CTRL_REG_PWRDN_MASK |
							 BCHP_MISC_QDAC1_CTRL_REG_PWRUP_BAIS_MASK;
#endif
				BREG_Write32(pPwr->hRegister, BCHP_MISC_QDAC1_CTRL_REG, ulRegData);
				
				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_QDAC2_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
				ulRegData &= ~BCHP_MISC_QDAC2_CTRL_REG_PWRUP_BAIS_MASK;
				ulRegData |= BCHP_MISC_QDAC2_CTRL_REG_PWRDN_MASK;
#else
				ulRegData |= BCHP_MISC_QDAC2_CTRL_REG_PWRDN_MASK |
							 BCHP_MISC_QDAC2_CTRL_REG_PWRUP_BAIS_MASK;
#endif
				BREG_Write32(pPwr->hRegister, BCHP_MISC_QDAC2_CTRL_REG, ulRegData);
					
				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_QDAC3_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
				ulRegData &= ~BCHP_MISC_QDAC3_CTRL_REG_PWRUP_BAIS_MASK;
				ulRegData |= BCHP_MISC_QDAC3_CTRL_REG_PWRDN_MASK;
#else
				ulRegData |= BCHP_MISC_QDAC3_CTRL_REG_PWRDN_MASK |
							 BCHP_MISC_QDAC3_CTRL_REG_PWRUP_BAIS_MASK;
#endif
				BREG_Write32(pPwr->hRegister, BCHP_MISC_QDAC3_CTRL_REG, ulRegData);

				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_QDAC4_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
				ulRegData &= ~BCHP_MISC_QDAC4_CTRL_REG_PWRUP_BAIS_MASK;
				ulRegData |= BCHP_MISC_QDAC4_CTRL_REG_PWRDN_MASK;
#else
				ulRegData |= BCHP_MISC_QDAC4_CTRL_REG_PWRDN_MASK |
							 BCHP_MISC_QDAC4_CTRL_REG_PWRUP_BAIS_MASK;
#endif
				BREG_Write32(pPwr->hRegister, BCHP_MISC_QDAC4_CTRL_REG, ulRegData);

				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET);
				ulRegData |= BCHP_FMISC_SOFT_RESET_HD_DVI0_MASK |
							 BCHP_FMISC_SOFT_RESET_ITU_R656_DEC1_MASK |
							 BCHP_FMISC_SOFT_RESET_ITU_R656_DEC0_MASK |
							 BCHP_FMISC_SOFT_RESET_RDC_MASK |
							 BCHP_FMISC_SOFT_RESET_VFD_3_MASK |
							 BCHP_FMISC_SOFT_RESET_VFD_2_MASK |
							 BCHP_FMISC_SOFT_RESET_VFD_1_MASK |
							 BCHP_FMISC_SOFT_RESET_VFD_0_MASK |
							 BCHP_FMISC_SOFT_RESET_MFD_1_MASK |
							 BCHP_FMISC_SOFT_RESET_MFD_0_MASK;
				BREG_Write32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET, ulRegData);

				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
				ulRegData |= BCHP_VCXO_CTL_MISC_VC0_CTRL_POWERDOWN_MASK;
				BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ulRegData);

				ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL);
				ulRegData |= BCHP_VCXO_CTL_MISC_VC1_CTRL_POWERDOWN_MASK;
				BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_HDM_ID))
        {
            BDBG_MSG(("Power Down: HDM"));
            /* Do we need to keep CEC Rx channel up? */
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

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2);
            ulRegData |= BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2_I_PWRDN_CH2_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, ulRegData);

            ulRegData = BCHP_CLK_PM_CTRL_DIS_HDMI_108M_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            BDBG_MSG(("Power Down: RFM CLK"));
            
            ulRegData = BCHP_CLK_PM_CTRL_DIS_RFM_108M_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Down: RAP CLK"));

			ulRegData = BCHP_CLK_PM_CTRL_DIS_AIO_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_RPTD_108M_CLK_MASK;
			BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);

			ulRegData = BCHP_CLK_PM_CTRL_2_DIS_AUD_DSP_PROG_CLK_MASK;
			BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_2,ulRegData,ulRegData);
		}

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: VDC CLK"));

			ulRegData = BCHP_CLK_PM_CTRL_DIS_BVNM_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_BVNB_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_VEC_DAC_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_VEC_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_TDAC_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_QDAC_108M_CLK_MASK |
						 /*BCHP_CLK_PM_CTRL_DIS_BVNF_108M_CLK_MASK |*/
						 BCHP_CLK_PM_CTRL_DIS_BVNF_PIR_108M_CLK_MASK;
			BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,ulRegData);

			ulRegData = BCHP_CLK_PM_CTRL_DIS_GFX_108M_CLK_MASK;
			BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XVD_ID))
        {
            BDBG_MSG(("Power Down: XVD CLK"));
			ulRegData =  BCHP_CLK_PM_CTRL_DIS_AVD1_108M_CLK_MASK |
						  BCHP_CLK_PM_CTRL_DIS_AVD0_108M_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL,ulRegData,ulRegData);

			ulRegData = BCHP_CLK_PM_CTRL_1_DIS_AVD1_200_CLK_MASK |
						BCHP_CLK_PM_CTRL_1_DIS_AVD0_200_CLK_MASK |
						BCHP_CLK_PM_CTRL_1_DIS_MEMC_16_0_200_CLK_MASK | 
						BCHP_CLK_PM_CTRL_1_DIS_MEMC_16_1_200_CLK_MASK;
            BREG_AtomicUpdate32( pPwr->hRegister, BCHP_CLK_PM_CTRL_1,ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XPT_ID))
        {
            BDBG_MSG(("Power Down: XPT"));

			ulRegData = BCHP_CLK_PM_CTRL_DIS_XPT_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,ulRegData);

			ulRegData = BCHP_CLK_PM_CTRL_1_DIS_XPT_40P5M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_1_DIS_XPT_20P25M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_1_DIS_XPT_81M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_1_DIS_XPT_54M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_1_DIS_XPT_27M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData,ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_MDM_ID))
        {
            BDBG_MSG(("Power Down: Soft Modem"));
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
            BDBG_MSG(("Power Down: VDC VCXO"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
            ulRegData |= BCHP_VCXO_CTL_MISC_VC0_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL);
			ulRegData |= BCHP_VCXO_CTL_MISC_VC1_CTRL_POWERDOWN_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_MISC);
			ulRegData |= BCHP_CLK_MISC_DIS_VEC_VCXO_656_CLK_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_CLK_MISC, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Down: RAP VXCO"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL);
            ulRegData |= BCHP_VCXO_CTL_MISC_AC0_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL, ulRegData);

            if (BPWR_P_IsModuleChanging(pPwr, BSTD_SCD_ID)) /* SmartCard clock is from AC1 */
            {
                ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL);
                ulRegData |= BCHP_VCXO_CTL_MISC_AC1_CTRL_POWERDOWN_MASK;
                BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL, ulRegData);

            }
			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC2_CTRL);
			ulRegData |= BCHP_VCXO_CTL_MISC_AC2_CTRL_POWERDOWN_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC2_CTRL, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AUDDSP_CTRL);
            ulRegData |= BCHP_VCXO_CTL_MISC_AUDDSP_CTRL_POWERDOWN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AUDDSP_CTRL, ulRegData);
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

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL);
			ulRegData |= BCHP_VCXO_CTL_MISC_VC1_CTRL_POWERDOWN_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
			ulRegData |= BCHP_VCXO_CTL_MISC_VC0_CTRL_POWERDOWN_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ulRegData);
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
            /*ulRegData = BREG_Read32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1); */
            ulRegData = (BCHP_CLK_PM_CTRL_1_DIS_SUN_SM_27M_CLK_MASK |
                         BCHP_CLK_PM_CTRL_1_DIS_SUN_DAA_32P4M_CLK_MASK);
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData,0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XPT_ID))
        {
            BDBG_MSG(("Power Up: XPT"));

			ulRegData = BCHP_CLK_PM_CTRL_1_DIS_XPT_40P5M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_1_DIS_XPT_20P25M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_1_DIS_XPT_81M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_1_DIS_XPT_54M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_1_DIS_XPT_27M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData,0);

            ulRegData = BCHP_CLK_PM_CTRL_DIS_XPT_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XVD_ID))
        {
            BDBG_MSG(("Power Up: XVD CLK"));
			ulRegData = BCHP_CLK_PM_CTRL_1_DIS_AVD1_200_CLK_MASK |
						BCHP_CLK_PM_CTRL_1_DIS_AVD0_200_CLK_MASK |
						BCHP_CLK_PM_CTRL_1_DIS_MEMC_16_0_200_CLK_MASK | 
						BCHP_CLK_PM_CTRL_1_DIS_MEMC_16_1_200_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_1, ulRegData, 0);

			ulRegData =  BCHP_CLK_PM_CTRL_DIS_AVD1_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_AVD0_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Up: VDC CLK"));

            ulRegData = BCHP_CLK_PM_CTRL_DIS_GFX_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData,0);

			ulRegData = BCHP_CLK_PM_CTRL_DIS_BVNM_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_BVNB_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_VEC_DAC_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_VEC_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_TDAC_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_QDAC_108M_CLK_MASK |
						 /*BCHP_CLK_PM_CTRL_DIS_BVNF_108M_CLK_MASK |*/
						 BCHP_CLK_PM_CTRL_DIS_BVNF_PIR_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Up: RAP CLK"));

			ulRegData = BCHP_CLK_PM_CTRL_2_DIS_AUD_DSP_PROG_CLK_MASK;
			BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL_2, ulRegData, 0);

			ulRegData = BCHP_CLK_PM_CTRL_DIS_AIO_108M_CLK_MASK |
						 BCHP_CLK_PM_CTRL_DIS_RPTD_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_HDM_ID))
        {
            BDBG_MSG(("Power Up: HDM"));

            ulRegData = BCHP_CLK_PM_CTRL_DIS_HDMI_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2);
            ulRegData &= ~BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2_I_PWRDN_CH2_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1);
            ulRegData &= ~BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1_I_PWR_DN_CH1_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0);
            ulRegData &= ~BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0_RND_PWRDN_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0, ulRegData);

            /* Do we need to keep CEC Rx channel up? */
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
			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC_BG_CTRL_REG);
			ulRegData &= ~(BCHP_MISC_TDAC_BG_CTRL_REG_PWRDN_MASK |
						   BCHP_MISC_TDAC_BG_CTRL_REG_PWRDN_REFAMP_MASK); 
			BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC_BG_CTRL_REG, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC1_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
			ulRegData |= BCHP_MISC_TDAC1_CTRL_REG_PWRUP_BAIS_MASK;
			ulRegData &= ~BCHP_MISC_TDAC1_CTRL_REG_PWRDN_MASK;
#else
			ulRegData &= ~(BCHP_MISC_TDAC1_CTRL_REG_PWRDN_MASK |
						   BCHP_MISC_TDAC1_CTRL_REG_PWRUP_BAIS_MASK);
#endif
			BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC1_CTRL_REG, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC2_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
			ulRegData |= BCHP_MISC_TDAC2_CTRL_REG_PWRUP_BAIS_MASK;
			ulRegData &= ~BCHP_MISC_TDAC2_CTRL_REG_PWRDN_MASK;
#else
			ulRegData &= ~(BCHP_MISC_TDAC2_CTRL_REG_PWRDN_MASK |
						   BCHP_MISC_TDAC2_CTRL_REG_PWRUP_BAIS_MASK);
#endif
			BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC2_CTRL_REG, ulRegData);
				
			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_TDAC3_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
			ulRegData |= BCHP_MISC_TDAC3_CTRL_REG_PWRUP_BAIS_MASK;
			ulRegData &= ~BCHP_MISC_TDAC3_CTRL_REG_PWRDN_MASK;
#else				
			ulRegData &= ~(BCHP_MISC_TDAC3_CTRL_REG_PWRDN_MASK |
						   BCHP_MISC_TDAC3_CTRL_REG_PWRUP_BAIS_MASK);
#endif
			BREG_Write32(pPwr->hRegister, BCHP_MISC_TDAC3_CTRL_REG, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_QDAC_BG_CTRL_REG);
			ulRegData &= ~(BCHP_MISC_QDAC_BG_CTRL_REG_PWRDN_MASK |
						   BCHP_MISC_QDAC_BG_CTRL_REG_PWRDN_REFAMP_MASK);
			BREG_Write32(pPwr->hRegister, BCHP_MISC_QDAC_BG_CTRL_REG, ulRegData);
			
			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_QDAC1_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
			ulRegData |= BCHP_MISC_QDAC1_CTRL_REG_PWRUP_BAIS_MASK;
			ulRegData &= ~BCHP_MISC_QDAC1_CTRL_REG_PWRDN_MASK;
#else
			ulRegData &= ~(BCHP_MISC_QDAC1_CTRL_REG_PWRDN_MASK |
						   BCHP_MISC_QDAC1_CTRL_REG_PWRUP_BAIS_MASK);
#endif
			BREG_Write32(pPwr->hRegister, BCHP_MISC_QDAC1_CTRL_REG, ulRegData);
			
			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_QDAC2_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
			ulRegData |= BCHP_MISC_QDAC2_CTRL_REG_PWRUP_BAIS_MASK;
			ulRegData &= ~BCHP_MISC_QDAC2_CTRL_REG_PWRDN_MASK;
#else
			ulRegData &= ~(BCHP_MISC_QDAC2_CTRL_REG_PWRDN_MASK |
						   BCHP_MISC_QDAC2_CTRL_REG_PWRUP_BAIS_MASK);
#endif
			BREG_Write32(pPwr->hRegister, BCHP_MISC_QDAC2_CTRL_REG, ulRegData);
				
			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_QDAC3_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
			ulRegData |= BCHP_MISC_QDAC3_CTRL_REG_PWRUP_BAIS_MASK;
			ulRegData &= ~BCHP_MISC_QDAC3_CTRL_REG_PWRDN_MASK;
#else
			ulRegData &= ~(BCHP_MISC_QDAC3_CTRL_REG_PWRDN_MASK |
						  BCHP_MISC_QDAC3_CTRL_REG_PWRUP_BAIS_MASK);
#endif
			BREG_Write32(pPwr->hRegister, BCHP_MISC_QDAC3_CTRL_REG, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_MISC_QDAC4_CTRL_REG);
#if (BCHP_VER > BCHP_VER_C0)
			ulRegData |= BCHP_MISC_QDAC4_CTRL_REG_PWRUP_BAIS_MASK;
			ulRegData &= ~BCHP_MISC_QDAC4_CTRL_REG_PWRDN_MASK;
#else
			ulRegData &= ~(BCHP_MISC_QDAC4_CTRL_REG_PWRDN_MASK |
						   BCHP_MISC_QDAC4_CTRL_REG_PWRUP_BAIS_MASK);
#endif
			BREG_Write32(pPwr->hRegister, BCHP_MISC_QDAC4_CTRL_REG, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET);
			ulRegData |= BCHP_FMISC_SOFT_RESET_HD_DVI0_MASK |
						 BCHP_FMISC_SOFT_RESET_ITU_R656_DEC1_MASK |
						 BCHP_FMISC_SOFT_RESET_ITU_R656_DEC0_MASK |
						 BCHP_FMISC_SOFT_RESET_RDC_MASK |
						 BCHP_FMISC_SOFT_RESET_VFD_3_MASK |
						 BCHP_FMISC_SOFT_RESET_VFD_2_MASK |
						 BCHP_FMISC_SOFT_RESET_VFD_1_MASK |
						 BCHP_FMISC_SOFT_RESET_VFD_0_MASK |
						 BCHP_FMISC_SOFT_RESET_MFD_1_MASK |
						 BCHP_FMISC_SOFT_RESET_MFD_0_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_FMISC_SOFT_RESET, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL);
			ulRegData &= ~BCHP_VCXO_CTL_MISC_VC0_CTRL_POWERDOWN_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC0_CTRL, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL);
			ulRegData &= ~BCHP_VCXO_CTL_MISC_VC1_CTRL_POWERDOWN_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_VC1_CTRL, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            /* HifiDriver */
            BDBG_MSG(("Power Up: RAP"));
            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN);
			ulRegData &= ~(BCHP_AIO_MISC_PWRDOWN_DAC1_MASK |
						   BCHP_AIO_MISC_PWRDOWN_DAC0_MASK);
            BREG_Write32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN, ulRegData);
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RFM_ID)) /* No other modules are tied to RFM clock */
        {
            BDBG_MSG(("Power Up: RFM"));

            ulRegData = BCHP_CLK_PM_CTRL_DIS_RFM_108M_CLK_MASK;
            BREG_AtomicUpdate32(pPwr->hRegister, BCHP_CLK_PM_CTRL, ulRegData, 0);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL);
			ulRegData &= ~BCHP_RFM_SYSCLK_CLKCTL_VREG_OFF_MASK;
#if (BCHP_VER > BCHP_VER_C0)
			ulRegData &=   ~(BCHP_RFM_SYSCLK_CLKCTL_RFMCLK_OFF_MASK |	
						     BCHP_RFM_SYSCLK_CLKCTL_BGCORE_OFF_MASK);
#endif
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL);
			ulRegData &= ~BCHP_RFM_SYSCLK_DACCTL_DAC_PWRDN_MASK;
#if (BCHP_VER > BCHP_VER_C0)
			ulRegData &= ~(BCHP_RFM_SYSCLK_DACCTL_BIAS50U_OFF_MASK |	
						   BCHP_RFM_SYSCLK_DACCTL_BIASLOG_OFF_MASK |
						   BCHP_RFM_SYSCLK_DACCTL_DAC_PWRDN_2_MASK);
#endif
			BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_DACCTL, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_PLL_MISC1);
			ulRegData &= ~(BCHP_RFM_SYSCLK_PLL_MISC1_NDIV_PWRDN_MASK |
						   BCHP_RFM_SYSCLK_PLL_MISC1_REFCOMP_PWRDN_MASK |
						   BCHP_RFM_SYSCLK_PLL_MISC1_PWRDN_MASK);
			BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_PLL_MISC1, ulRegData);

			ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_RFM_PHASEITP_CTL);
			ulRegData &= ~BCHP_RFM_SYSCLK_RFM_PHASEITP_CTL_PHASEITP_PWRDN_MASK;
			BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_RFM_PHASEITP_CTL, ulRegData);

            ulRegData = BREG_Read32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL);
            ulRegData &= ~BCHP_RFM_SYSCLK_CLKCTL_CLK_OFF_MASK;
            BREG_Write32(pPwr->hRegister, BCHP_RFM_SYSCLK_CLKCTL, ulRegData);
        }
    }

    BDBG_LEAVE(BPWR_P_SetPowerMode);
    return eStatus;
}

/* End of file */

