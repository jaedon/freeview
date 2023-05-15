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
 * $brcm_Date: 9/21/12 7:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwr/src/7340/bpwr_priv.c $
 * 
 * Hydra_Software_Devel/1   9/21/12 7:09p nickh
 * SW7425-3971: Add chip-specific private files
 * 
 * Hydra_Software_Devel/1   10/15/10 4:52p gskerl
 * SW7340-215: Add first draft of PWR PI for 7340/7342
 * 
 * Hydra_Software_Devel/3   9/24/10 1:20p gskerl
 * SW7468-102: Merged back to /main/Hydra_Software_Devel branch
 * 
 * Hydra_Software_Devel/BRUTUS_97468_IBC_PM_DEMO/1   8/26/10 2:45p gskerl
 * SW7468-102: Added powerdown of top level clocks when all modules are
 * being powered down.
 * 
 * Hydra_Software_Devel/2   6/9/10 3:56p gskerl
 * SW7468-102: Modified to support the 7468
 * 
 * Hydra_Software_Devel/1   3/1/10 4:36p randyjew
 * SW7468-6:Adding bpwr_priv for 7468/7208
 * 
 * Hydra_Software_Devel/7   2/4/10 6:25p nickh
 * SW7420-573: Future test port
 * 
 * Hydra_Software_Devel/6   12/28/09 11:45a nickh
 * SW7420-70: Fix register access to CLK.SYS_PLL_0_PLL_3
 * 
 * Hydra_Software_Devel/5   12/22/09 1:04p nickh
 * SW7420-70: Fixes and additions for other modules
 *
 * Hydra_Software_Devel/4   12/14/09 2:34p erickson
 * SW7420-332: rename BSTD_TMN_ID
 *
 * Hydra_Software_Devel/3   9/24/09 6:50p nickh
 * SW7420-70: Add 7420B0 support for changed register names
 *
 * Hydra_Software_Devel/2   8/16/09 6:00p nickh
 * PR47760: Add 7420 PWR PI support
 *
 * Hydra_Software_Devel/1   12/8/08 2:56p garylin
 * PR47760: init version (skeleton)
 *
 ***************************************************************************/
#include "bstd.h"                /* standard types */
#include "berr.h"
#include "bstd_ids.h"
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"                /* malloc */
#include "bchp_clkgen.h"
#include "bchp_hdmi_tx_phy.h"
#include "bchp_misc.h"
#include "bchp_vcxo_ctl_misc.h"
#include "bchp_aio_misc.h"
#include "bchp_fmisc.h"
#include "bchp_avd_rgr_0.h"
#include "bchp_uhfr_glbl.h"

#include "bpwr.h"
#include "bpwr_priv.h"
#include "bpwr_mem.h"

BDBG_MODULE(BPWR);
 

/* List of modules relevant to this chip.
Don't add unused ids here. */
static BSTD_Module g_ModuleList[] =
{
    BSTD_DMA_ID,
    BSTD_GRC_ID,

    BSTD_HDM_ID,
    BSTD_UHF_ID,

    BSTD_RAP_ID,
	BSTD_SCD_ID,
	BSTD_VBI_ID,
	BSTD_VDC_ID,
	BSTD_XPT_ID,
	BSTD_XVD_ID,
    0
};

BERR_Code BPWR_P_Init(BPWR_P_Context    *pPwr )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(pPwr, BPWR_P_Context);

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
        BSTD_Module initModuleList[] = {BSTD_HDM_ID, BSTD_VDC_ID, BSTD_VBI_ID, BSTD_XVD_ID };
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

bool BPWR_P_IsModuleChanging(const BPWR_P_Context *pPwr, BSTD_Module moduleId)
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


void BPWR_RegUpdate32(BREG_Handle RegHandle, uint32_t reg, uint32_t mask, uint32_t value )
{
    uint32_t temp;

    temp = BREG_Read32(RegHandle, reg);
    temp = (temp&~mask)|value;
    BREG_Write32(RegHandle, reg, temp);
    return;
}


BERR_Code BPWR_P_SetPowerMode ( BPWR_P_Context    *pPwr)
{
    BERR_Code                     eStatus = BERR_SUCCESS;

    BDBG_ENTER(BPWR_P_SetPowerMode);

    /* handle power down or power up in the order PWR requires */
    if (pPwr->changeMode == BPWR_Mode_ePowerDown)
    {
        /* Power Down HDMI */
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_HDM_ID))
        {
            BDBG_MSG(("Power Down: HDM"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)

                BPWR_RegUpdate32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL,
                                 /* clear these bits */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_reserved0_MASK           |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_reserved1_MASK           ,
                                 /* set these bits */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_0_PWRDN_MASK          |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_1_PWRDN_MASK          |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_2_PWRDN_MASK          |     
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_CK_PWRDN_MASK         |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_CEC_MASK           |  /* Clear this bit for CEC up */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PLL_PWRDN_MASK           |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_REF_COMP_PWRDN_MASK      |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_BG_MASK            |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_REFAMP_MASK        |  /* Clear this bit for CEC up */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_D_RESET_MASK             |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_A_RESET_MASK             |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_CK_RESET_MASK         |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_2_RESET_MASK          |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_1_RESET_MASK          |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_0_RESET_MASK );


                BPWR_RegUpdate32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,
                                 /* clear these bits */
                                 0  ,
                                 /* set these bits */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0_RND_PWRDN_MASK );

                BPWR_RegUpdate32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1,
                                 /* clear these bits */
                                 0  ,
                                 /* set these bits */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1_I_PWR_DN_CH1_MASK );

                BPWR_RegUpdate32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2,
                                 /* clear these bits */
                                 0  ,
                                 /* set these bits */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2_I_PWRDN_CH2_MASK );

                #if (BCHP_CHIP==7340)
    
                    /* power down all HDMI clocks, except for CEC related clock */
                    BPWR_RegUpdate32(pPwr->hRegister, BCHP_CLKGEN_DVPHT_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_reserved0_MASK           |
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_reserved_for_eco1_MASK   ,
                                     /* set these bits */
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_DIS_CLK_108_ALTERNATE_MASK   |
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_DIS_CLK_27X_IIC_MASK         |  /* Clear this bit for CEC up */
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_DIS_CLK_27X_PM_HDMI_MASK     |
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_DIS_CLK_216_MASK             |
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_DIS_CLK_108_MASK             );



                #endif /* (BCHP_CHIP==7340)  */

                #if (BCHP_CHIP==7342)
    
                    BPWR_RegUpdate32(pPwr->hRegister, BCHP_CLK_DVP_HT_CLK_PM_CTRL,
                                     /* clear these bits */
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_reserved0_MASK  ,
                                     /* set these bits */
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_2ND_108M_CLK_MASK        |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_VEC_216M_CLK_MASK        |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_HDMI_MAX_250M_CLK_MASK   |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_HDMI_27M_CLK_MASK        |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_HDMI_PM_27M_CLK_MASK     |  /* Clear this bit for CEC up */
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_216M_CLK_MASK            |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_108M_CLK_MASK            );

                #endif /* (BCHP_CHIP==7342)  */

            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_UHF_ID))
        {
            BDBG_MSG(("Power Down: UHF Receiver"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)

                /* These AUTOPD1 bits are controlled by the UHF PI              */
                /* Target.SUNDRY.UHFR_BLOCK.UHFR_GLBL.AUTOPD1.AUTO_PD_EN = 0    */
                /* Target.SUNDRY.UHFR_BLOCK.UHFR_GLBL.AUTOPD1.MANUAL_PD = 1     */

                #if (BCHP_CHIP==7340)

                    /* Power down UHF clocks. Will save some power in 1.2v Digital */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLKGEN_UHFR_CLK_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_UHFR_CLK_PM_CTRL_reserved0_MASK                ,
                                     /* set these bits */
                                     BCHP_CLKGEN_UHFR_CLK_PM_CTRL_DIS_CLK_54X_FILT_UHFR_MASK    |
                                     BCHP_CLKGEN_UHFR_CLK_PM_CTRL_DIS_CLK_54X_DIG_UHFR_MASK     |
                                     BCHP_CLKGEN_UHFR_CLK_PM_CTRL_DIS_CLK_54X_UHF_MASK          |
                                     BCHP_CLKGEN_UHFR_CLK_PM_CTRL_DIS_CLK_27X_UHFR_MASK         );

                #endif /* (BCHP_CHIP==7340)  */

                #if (BCHP_CHIP==7342)

                    /* Power down UHF clocks. will save some power in 1.2v Digital */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLK_UHFR_CLK_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLK_UHFR_CLK_PM_CTRL_reserved0_MASK           ,
                                     /* set these bits */
                                     BCHP_CLK_UHFR_CLK_PM_CTRL_DIS_ANA_UHFR_CLK_MASK    |
                                     BCHP_CLK_UHFR_CLK_PM_CTRL_DIS_DIGI_UHFR_CLK_MASK   );
#if 0
/* Dont mess with pinmux settings unless we need to */
                    /* Power down UHF external LNA */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21  ,
                                     /* clear these bits */
                                     BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_05_MASK        |
                                     BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_04_MASK        ,
                                     /* set these bits */
                                     (BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_05_UHFR_GLB_PWRDN  <<  
                                                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_05_SHIFT)   |
                                     (BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_04_UHFR_GLB_PWRDN  <<  
                                                        BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_04_SHIFT)   );
#endif
                #endif /* (BCHP_CHIP==7342)  */

            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: VDC DACs"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)

                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_BG_CTRL_0   ,
                                 /* clear these bits */
                                 BCHP_MISC_DAC_BG_CTRL_0_reserved0_MASK     ,
                                 /* set these bits */
                                  BCHP_MISC_DAC_BG_CTRL_0_PWRDN_REFAMP_MASK |
                                  BCHP_MISC_DAC_BG_CTRL_0_PWRDN_MASK        );
    
                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_0_CTRL  ,
                                 /* clear these bits */
                                  BCHP_MISC_DAC_0_CTRL_reserved0_MASK     |
                                  BCHP_MISC_DAC_0_CTRL_PWRUP_BAIS_MASK    ,
                                 /* set these bits */
                                  BCHP_MISC_DAC_0_CTRL_PWRDN_MASK         );
    
                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_1_CTRL  ,
                                 /* clear these bits */
                                  BCHP_MISC_DAC_1_CTRL_reserved0_MASK     |
                                  BCHP_MISC_DAC_1_CTRL_PWRUP_BAIS_MASK    ,
                                 /* set these bits */
                                  BCHP_MISC_DAC_1_CTRL_PWRDN_MASK         );
    
                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_2_CTRL  ,
                                 /* clear these bits */
                                  BCHP_MISC_DAC_2_CTRL_reserved0_MASK     |
                                  BCHP_MISC_DAC_2_CTRL_PWRUP_BAIS_MASK    ,
                                 /* set these bits */
                                  BCHP_MISC_DAC_2_CTRL_PWRDN_MASK         );
    
                #if (BCHP_CHIP==7340)
                
                    /* Power down TDAC_0 216MHz clock */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLKGEN_TDAC_CLK_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_TDAC_CLK_PM_CTRL_reserved0_MASK          ,
                                     /* set these bits */
                                     BCHP_CLKGEN_TDAC_CLK_PM_CTRL_DIS_CLK_216_TDAC0_MASK  );

                #endif /* (BCHP_CHIP==7340)  */

                #if (BCHP_CHIP==7342)
                
                    /* Power down TDAC_0 108MHz clock */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLK_TDAC_0_CLK_PM_CTRL     ,
                                     /* clear these bits */
                                     BCHP_CLK_TDAC_0_CLK_PM_CTRL_reserved0_MASK           ,
                                     /* set these bits */
                                     BCHP_CLK_TDAC_0_CLK_PM_CTRL_DIS_TDAC_0_108M_CLK_MASK );

                #endif /* (BCHP_CHIP==7342)  */

                /* Power down TDAC_1 */
                BPWR_RegUpdate32(pPwr->hRegister,    BCHP_MISC_DAC_BG_CTRL_1    ,
                                 /* clear these bits */
                                 BCHP_MISC_DAC_BG_CTRL_1_reserved0_MASK         ,
                                 /* set these bits */
                                 BCHP_MISC_DAC_BG_CTRL_1_PWRDN_REFAMP_MASK      |
                                 BCHP_MISC_DAC_BG_CTRL_1_PWRDN_MASK             );

                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_3_CTRL  ,
                                 /* clear these bits */
                                  BCHP_MISC_DAC_3_CTRL_reserved0_MASK     |
                                  BCHP_MISC_DAC_3_CTRL_PWRUP_BAIS_MASK    ,
                                 /* set these bits */
                                  BCHP_MISC_DAC_3_CTRL_PWRDN_MASK         );
    
                /* Power down TDAC_1_4 */
                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_4_CTRL  ,
                                 /* clear these bits */
                                  BCHP_MISC_DAC_4_CTRL_reserved0_MASK     |
                                  BCHP_MISC_DAC_4_CTRL_PWRUP_BAIS_MASK    ,
                                 /* set these bits */
                                  BCHP_MISC_DAC_4_CTRL_PWRDN_MASK         );
    
                /* Power down TDAC_1_5 */
                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_5_CTRL  ,
                                 /* clear these bits */
                                  BCHP_MISC_DAC_5_CTRL_reserved0_MASK     |
                                  BCHP_MISC_DAC_5_CTRL_PWRUP_BAIS_MASK    ,
                                 /* set these bits */
                                  BCHP_MISC_DAC_5_CTRL_PWRDN_MASK         );

                #if (BCHP_CHIP==7340)
                
                    /* Power down TDAC_1 216MHz clock */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLKGEN_TDAC_CLK_PM_CTRL   ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_TDAC_CLK_PM_CTRL_reserved0_MASK            ,
                                     /* set these bits */
                                     BCHP_CLKGEN_TDAC_CLK_PM_CTRL_DIS_CLK_216_TDAC1_MASK    );

                #endif /* (BCHP_CHIP==7340)  */
    
                #if (BCHP_CHIP==7342)
                
                    /* Power down TDAC_1 108 MHz clock */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_CLK_TDAC_1_CLK_PM_CTRL     ,
                                     /* clear these bits */
                                      BCHP_CLK_TDAC_1_CLK_PM_CTRL_reserved0_MASK              ,
                                     /* set these bits */
                                      BCHP_CLK_TDAC_1_CLK_PM_CTRL_DIS_TDAC_1_108M_CLK_MASK     );

                #endif /* (BCHP_CHIP==7342)  */

            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Down: RAP AIO"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)

                /* Power down HifiDriver0 */
                BPWR_RegUpdate32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN ,
                                 /* clear these bits */
                                 BCHP_AIO_MISC_PWRDOWN_reserved0_MASK   ,
                                 /* set these bits */
                                 BCHP_AIO_MISC_PWRDOWN_DAC0_MASK        );

                #if (BCHP_CHIP==7340)
                
                    /* Power down AC0. AC PLLs should be powered down before VC0/1 power down.  */
                    /* Disable AC0 output clock first                                           */
                    BPWR_RegUpdate32(pPwr->hRegister,  BCHP_VCXO_CTL_MISC_AC0_PM_CLOCK_ENA_1    ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_CLOCK_ENA_1_reserved0_MASK       |
                                     BCHP_VCXO_CTL_MISC_AC0_PM_CLOCK_ENA_1_CLOCK_ENA_MASK       ,
                                     /* set these bits */
                                     0 );
                    
                    /* Power down channel 1 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_1_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_1_DIS_CH_MASK    );
                    
                    /* Power down channel 2 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_2 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_2_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_2_DIS_CH_MASK    );

                    
                    /* Power down channel 3 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_3 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_3_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_3_DIS_CH_MASK    );
                
                #endif /* (BCHP_CHIP==7340)  */

                /* Power down AC0. Should AC PLLs be powered down before VC0/1 power down? */
                BPWR_RegUpdate32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL ,
                                 /* clear these bits */
                                 BCHP_VCXO_CTL_MISC_AC0_CTRL_reserved0_MASK   |
                                 BCHP_VCXO_CTL_MISC_AC0_CTRL_RESERVED_MASK    ,
                                 /* set these bits */
                                 BCHP_VCXO_CTL_MISC_AC0_CTRL_RESET_MASK      |
                                 BCHP_VCXO_CTL_MISC_AC0_CTRL_POWERDOWN_MASK  );


                #if (BCHP_CHIP==7340)
                
                    /* Power down AC1 */
                    /* Disable AC1 output clock first */
                    BPWR_RegUpdate32(pPwr->hRegister,  BCHP_VCXO_CTL_MISC_AC1_PM_CLOCK_ENA_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_PM_CLOCK_ENA_1_reserved0_MASK   |
                                     BCHP_VCXO_CTL_MISC_AC1_PM_CLOCK_ENA_1_CLOCK_ENA_MASK   ,
                                     /* set these bits */
                                     0 );
                    
                    /* Power down channel 1 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_1_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_1_DIS_CH_MASK        );
                    
                    /* Power down channel 2 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_2,
                                     /* clear these bits */
                                      BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_2_reserved0_MASK    ,
                                     /* set these bits */
                                      BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_2_DIS_CH_MASK       );
                    
                    /* Power down channel 3 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_3   ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_3_reserved0_MASK ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_3_DIS_CH_MASK    );
                    
                    /* Power down AC1 PLL. */
                    BPWR_RegUpdate32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_CTRL_reserved0_MASK   |
                                     BCHP_VCXO_CTL_MISC_AC1_CTRL_RESERVED_MASK    ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_CTRL_RESET_MASK      |
                                     BCHP_VCXO_CTL_MISC_AC1_CTRL_POWERDOWN_MASK  );
                    
                    /* Disable AC2 output clock first */
                    BPWR_RegUpdate32(pPwr->hRegister,  BCHP_VCXO_CTL_MISC_AC2_PM_CLOCK_ENA_1,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_PM_CLOCK_ENA_1_reserved0_MASK   |
                                     BCHP_VCXO_CTL_MISC_AC2_PM_CLOCK_ENA_1_CLOCK_ENA_MASK   ,
                                     /* set these bits */
                                     0 );
                    
                    /* Power down channel 1 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_1_reserved0_MASK ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_1_DIS_CH_MASK    );
                    
                    /* Power down channel 2 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_2,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_2_reserved0_MASK ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_2_DIS_CH_MASK    );
                    
                    /* Power down channel 3 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_3,
                                     /* clear these bits */
                                      BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_3_reserved0_MASK,
                                     /* set these bits */
                                      BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_3_DIS_CH_MASK   );
                    
                    /* Power down AC2 PLL. */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_VCXO_CTL_MISC_AC2_CTRL  ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_CTRL_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_AC2_CTRL_RESERVED_MASK  ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_CTRL_RESET_MASK     |
                                     BCHP_VCXO_CTL_MISC_AC2_CTRL_POWERDOWN_MASK );
                    
                    /* Power down RPTD AIO clocks               */
                    BPWR_RegUpdate32(pPwr->hRegister,  BCHP_CLKGEN_RPT_AIO_CLK_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_RPT_AIO_CLK_PM_CTRL_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_CLKGEN_RPT_AIO_CLK_PM_CTRL_DIS_CLK_250P71_RPTD_MASK   |
                                     BCHP_CLKGEN_RPT_AIO_CLK_PM_CTRL_DIS_CLK_216_AIO_MASK       |
                                     BCHP_CLKGEN_RPT_AIO_CLK_PM_CTRL_DIS_CLK_108_AIO_MASK       );
                
                #endif /* (BCHP_CHIP==7340)  */

                #if (BCHP_CHIP==7342)

                    /* Shutdown the 250 MHz clock by CLK_RPT_AIO_CLK_PM_CTRL [2] first,     */
                    /* then shutdown PLL in the VCXO_CTL_MISC_RAP_PM_CLOCK_ENA_1            */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_CLK_RPT_AIO_CLK_PM_CTRL    ,
                                     /* clear these bits */
                                     BCHP_CLK_RPT_AIO_CLK_PM_CTRL_reserved0_MASK        ,
                                     /* set these bits */
                                     BCHP_CLK_RPT_AIO_CLK_PM_CTRL_DIS_RPT_PROG_CLK_MASK |
                                     BCHP_CLK_RPT_AIO_CLK_PM_CTRL_DIS_216M_CLK_MASK     );
                    
                    /* =============================================    */
                    /*  power down sys RAPTOR PLL channel 1 for RAPTOR. */
                    /* =============================================    */
                    
                    /* Disable PLL channel 1 clock output */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_VCXO_CTL_MISC_RAP_PM_CLOCK_ENA_1  ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PM_CLOCK_ENA_1_reserved0_MASK       |
                                     BCHP_VCXO_CTL_MISC_RAP_PM_CLOCK_ENA_1_CLOCK_ENA_MASK       ,
                                     /* set these bits */
                                     0 );
                    
                    /* Shut down PLL channel 1 */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_VCXO_CTL_MISC_RAP_PM_DIS_CHL_1,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PM_DIS_CHL_1_reserved0_MASK ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PM_DIS_CHL_1_DIS_CH_MASK    );
                    
                    /* Power down RAP_PLL_CHL_1 CML buffer */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_VCXO_CTL_MISC_RAP_PM_EN_CML_BUF_1,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PM_EN_CML_BUF_1_reserved0_MASK  |
                                     BCHP_VCXO_CTL_MISC_RAP_PM_EN_CML_BUF_1_EN_CMLBUF_MASK  ,
                                     /* set these bits */
                                     0 );
                    
                    /* Power down RAP_PLL */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_RAP_PLL_CTRL,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PLL_CTRL_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_RAP_PLL_CTRL_RESERVED_MASK  ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PLL_CTRL_RESET_MASK     |
                                     BCHP_VCXO_CTL_MISC_RAP_PLL_CTRL_POWERDOWN_MASK );
                    
                    /* print "power down RPTD AIO 108 clocks  "*/
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_CLK_RPT_AIO_CLK_PM_CTRL    ,
                                     /* clear these bits */
                                     BCHP_CLK_RPT_AIO_CLK_PM_CTRL_reserved0_MASK        ,
                                     /* set these bits */
                                     BCHP_CLK_RPT_AIO_CLK_PM_CTRL_DIS_108M_CLK_MASK     );
                
                #endif /* (BCHP_CHIP==7342)  */

            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: VEC"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                #if (BCHP_CHIP==7340)
    
                    /* Power down VEC  216 */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_CLKGEN_VEC_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_VEC_CLK_PM_CTRL_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_CLKGEN_VEC_CLK_PM_CTRL_DIS_CLK_108_MASK   |
                                     BCHP_CLKGEN_VEC_CLK_PM_CTRL_DIS_CLK_216_MASK   );
                
                #endif /* (BCHP_CHIP==7340)  */
    
                #if (BCHP_CHIP==7342)
    
                    /* Power down VEC  656 */ 
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLK_VEC_656_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLK_VEC_656_CLK_PM_CTRL_reserved0_MASK      ,
                                     /* set these bits */
                                     BCHP_CLK_VEC_656_CLK_PM_CTRL_DIS_VEC_VCXO_656_CLK_MASK  );
                    
                    /* Power down VEC */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLK_VEC_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLK_VEC_CLK_PM_CTRL_reserved0_MASK       ,
                                     /* set these bits */
                                     BCHP_CLK_VEC_CLK_PM_CTRL_DIS_VEC_DAC_108M_CLK_MASK |
                                     BCHP_CLK_VEC_CLK_PM_CTRL_DIS_216M_CLK_MASK         |
                                     BCHP_CLK_VEC_CLK_PM_CTRL_DIS_108M_CLK_MASK         );
                
                #endif /* (BCHP_CHIP==7342)  */
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_GRC_ID))
        {
            BDBG_MSG(("Power Down: GFX 2D"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                #if (BCHP_CHIP==7340)
                
                    /* Power down GFX 2D 216M and 108M clocks */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_CLKGEN_GFX_CLK_PM_CTRL_2D ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_2D_reserved0_MASK      ,
                                     /* set these bits */
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_2D_DIS_CLK_216_MASK    |
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_2D_DIS_CLK_108_MASK    );
                
                    /* Power down GFX 3D 216M and 108M clocks */
                    BPWR_RegUpdate32(pPwr->hRegister,       BCHP_CLKGEN_GFX_CLK_PM_CTRL_3D  ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_3D_reserved0_MASK      ,
                                     /* set these bits */
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_3D_DIS_CLK_216_MASK    |
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_3D_DIS_CLK_108_MASK    );

                #endif /* (BCHP_CHIP==7340)  */
    
                #if (BCHP_CHIP==7342)
                
                    /* Power down GFX 2D */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLK_GFX_2D_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLK_GFX_2D_PM_CTRL_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_CLK_GFX_2D_PM_CTRL_DIS_108M_CLK_MASK  |
                                     BCHP_CLK_GFX_2D_PM_CTRL_DIS_216M_CLK_MASK  );

                    
                    /* Power Down GFX 3D */
                    /* GFX 3D 108 in pm ctrl reg is not used. It uses only 216 MHz. */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLK_GFX_3D_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLK_GFX_3D_PM_CTRL_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_CLK_GFX_3D_PM_CTRL_DIS_108M_CLK_MASK  |
                                     BCHP_CLK_GFX_3D_PM_CTRL_DIS_216M_CLK_MASK  );

                #endif /* (BCHP_CHIP==7342)  */
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Down: VDC"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                #if (BCHP_CHIP==7340)
                        
                    /* Power down BVN_TOP 216 MHz and 108 MHz clocks */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_CLKGEN_BVN_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_BVN_CLK_PM_CTRL_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_CLKGEN_BVN_CLK_PM_CTRL_DIS_CLK_216_MASK   |
                                     BCHP_CLKGEN_BVN_CLK_PM_CTRL_DIS_CLK_108_MASK   );
                
                #endif /* (BCHP_CHIP==7340)  */
                
                
                #if (BCHP_CHIP==7342)
                        
                    /* Power down BVN_TOP 216 MHz and 108 MHz clocks */
                    BPWR_RegUpdate32(pPwr->hRegister,       BCHP_CLK_BVN_TOP_PM_CTRL ,
                                     /* clear these bits */
                                      BCHP_CLK_BVN_TOP_PM_CTRL_reserved0_MASK       ,
                                     /* set these bits */
                                      BCHP_CLK_BVN_TOP_PM_CTRL_DIS_216M_CLK_MASK    |
                                      BCHP_CLK_BVN_TOP_PM_CTRL_DIS_108M_CLK_MASK    );
                
                #endif /* (BCHP_CHIP==7342)  */
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XVD_ID))
        {
            BDBG_MSG(("Power Down: AVD"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                #if (BCHP_CHIP==7340)
                        
                    /* Power down AVD 250, 216, and 108 clocks */ 
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLKGEN_AVD_CLK_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_AVD_CLK_PM_CTRL_reserved0_MASK       ,
                                     /* set these bits */
                                     BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_250P71_MASK  |
                                     BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_216_MASK     |
                                     BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_108_MASK     );

                #endif /* (BCHP_CHIP==7340)  */
                
                #if (BCHP_CHIP==7342)

                    /* Power down AVD clocks */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLK_AVD0_CLK_PM_CTRL   ,
                                     /* clear these bits */
                                     BCHP_CLK_AVD0_CLK_PM_CTRL_reserved0_MASK         ,
                                     /* set these bits */
                                     BCHP_CLK_AVD0_CLK_PM_CTRL_DIS_AVD0_PROG_CLK_MASK |
                                     BCHP_CLK_AVD0_CLK_PM_CTRL_DIS_216M_CLK_MASK      |
                                     BCHP_CLK_AVD0_CLK_PM_CTRL_DIS_108M_CLK_MASK      );

                    /* Power down AVD_PM_CLOCK_ENA_1 */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AVD_PM_CLOCK_ENA_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AVD_PM_CLOCK_ENA_1_reserved0_MASK  |
                                     BCHP_VCXO_CTL_MISC_AVD_PM_CLOCK_ENA_1_CLOCK_ENA_MASK  ,
                                     /* set these bits */
                                     0 );

                    /* Power down AVD_PM_DIS_CHL_1 */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_VCXO_CTL_MISC_AVD_PM_DIS_CHL_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AVD_PM_DIS_CHL_1_reserved0_MASK ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AVD_PM_DIS_CHL_1_DIS_CH_MASK    );

                    /* Power down AVD_PM_EN_CML_BUF_1 */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_VCXO_CTL_MISC_AVD_PM_EN_CML_BUF_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AVD_PM_EN_CML_BUF_1_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_AVD_PM_EN_CML_BUF_1_EN_CMLBUF_MASK ,
                                     /* set these bits */
                                     0 );

                    /* Power down AVD_PLL_CTRL */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AVD_PLL_CTRL ,
                                     /* clear these bits */
                                      BCHP_VCXO_CTL_MISC_AVD_PLL_CTRL_reserved0_MASK  ,
                                     /* set these bits */
                                      BCHP_VCXO_CTL_MISC_AVD_PLL_CTRL_RESET_MASK      |
                                      BCHP_VCXO_CTL_MISC_AVD_PLL_CTRL_POWERDOWN_MASK  );

                #endif /* (BCHP_CHIP==7342)  */
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XPT_ID))
        {
            BDBG_MSG(("Power Down: XPT"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                #if (BCHP_CHIP==7340)
            
                    /* Power down XPT clocks. */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLKGEN_XPT_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_20P25_MASK |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_27_MASK    |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_40P5_MASK  |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_54_MASK    |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_81_MASK    |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_216_MASK   |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_108_MASK   );
                    
                    /* Power down PLL_MAIN_CH3 81MHz core_xpt clock. */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLKGEN_PLLMAIN_CH3_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_PLLMAIN_CH3_PM_CTRL_reserved0_MASK         ,
                                     /* set these bits */
                                     BCHP_CLKGEN_PLLMAIN_CH3_PM_CTRL_PWRDN_CH3_PLLMAIN_MASK );
                    
                #endif /* (BCHP_CHIP==7340)  */
                
                #if (BCHP_CHIP==7342)
            
                    /* Power down XPT clocks. */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_CLK_XPT_CLK_PM_CTRL   ,
                                     /* clear these bits */
                                     BCHP_CLK_XPT_CLK_PM_CTRL_reserved0_MASK          ,
                                     /* set these bits */
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_XPT_20P25M_CLK_MASK |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_XPT_27M_CLK_MASK    |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_XPT_40P5M_CLK_MASK  |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_XPT_54M_CLK_MASK    |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_XPT_81M_CLK_MASK    |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_216M_CLK_MASK       |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_108M_CLK_MASK       );
                    
                    /* Power down SYS_PLL_0_PLL_2 81MHz core_xpt clock, PLL source post divider and disable CML buffer */
                    BPWR_RegUpdate32(pPwr->hRegister,  BCHP_CLK_SYS_PLL_0_PLL_2 ,
                                     /* clear these bits */
                                     BCHP_CLK_SYS_PLL_0_PLL_2_reserved0_MASK |   
                                     BCHP_CLK_SYS_PLL_0_PLL_2_EN_CMLBUF_MASK ,
                                     /* set these bits */
                                     BCHP_CLK_SYS_PLL_0_PLL_2_DIS_CH_MASK    );
            
                #endif /* (BCHP_CHIP==7342)  */
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_DMA_ID) )
        {
            BDBG_MSG(("Power Down: DMA"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                #if (BCHP_CHIP==7340)
            
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_CLKGEN_SECTOP_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_SECTOP_CLK_PM_CTRL_reserved0_MASK     ,
                                     /* set these bits */
                                     BCHP_CLKGEN_SECTOP_CLK_PM_CTRL_DIS_CLK_216_ALTERNATE_MASK );
                    
                #endif /* (BCHP_CHIP==7340)  */
                
                #if (BCHP_CHIP==7342)
                        
                    Target.CLK.SECTOP_DMA_PM_CTRL.DIS_216M_CLK = 1
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLK_SECTOP_DMA_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLK_SECTOP_DMA_PM_CTRL_reserved0_MASK      ,
                                     /* set these bits */
                                     BCHP_CLK_SECTOP_DMA_PM_CTRL_DIS_216M_CLK_MASK   );
            
                #endif /* (BCHP_CHIP==7342)  */
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        /* If all modules are being powered down, then power down the top-level clocks. */
        {
            int i=0;
            for (i=0;i<BPWR_MAX_MODULES && g_ModuleList[i];i++)
            {
                if ( ! BPWR_P_IsModuleChanging(pPwr, g_ModuleList[i]) ) {
                    break;
                }
            }
            if (g_ModuleList[i] == 0)   /* All modules are being powered down */
            {
                BDBG_MSG(("Power Down: Top Level Clocks"));
    
                #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                    #if (BCHP_CHIP==7340)
                
                                
                        /* TODO */
                
                                
                    #endif /* (BCHP_CHIP==7340)  */
                    
                    #if (BCHP_CHIP==7342)


                        /* TODO */


                    #endif /* (BCHP_CHIP==7342)  */
                #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
            }
        }


    /*
      *============================================================
      *                Power Up
      *============================================================
      */
    }
    else
    {
        /* Start by making sure that the top level clocks are on. */
        BDBG_MSG(("Power Up: Top Level Clocks"));

        #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
            #if (BCHP_CHIP==7342)


                /* TODO */


            #endif /* (BCHP_CHIP==7342)  */
            #if (BCHP_CHIP==7340)
        
                        
                /* TODO */
        
                        
            #endif /* (BCHP_CHIP==7340)  */
        #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_DMA_ID) )
        {
            BDBG_MSG(("Power Up: DMA"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                #if (BCHP_CHIP==7342)
                        
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLK_SECTOP_DMA_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLK_SECTOP_DMA_PM_CTRL_reserved0_MASK      |
                                     BCHP_CLK_SECTOP_DMA_PM_CTRL_DIS_216M_CLK_MASK   ,
                                     /* set these bits */
                                     0 );
            
                #endif /* (BCHP_CHIP==7342)  */
                #if (BCHP_CHIP==7340)
            
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_CLKGEN_SECTOP_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_SECTOP_CLK_PM_CTRL_reserved0_MASK              |
                                     BCHP_CLKGEN_SECTOP_CLK_PM_CTRL_DIS_CLK_216_ALTERNATE_MASK  ,
                                     /* set these bits */
                                     0 );
                    
                #endif /* (BCHP_CHIP==7340)  */
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XPT_ID))
        {
            BDBG_MSG(("Power Up: XPT"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                
                #if (BCHP_CHIP==7342)
                    /* Power up SYS_PLL_0_PLL_2 81MHz core_xpt clock, PLL source post divider and disable CML buffer */
                    BPWR_RegUpdate32(pPwr->hRegister,  BCHP_CLK_SYS_PLL_0_PLL_2 ,
                                     /* clear these bits */
                                     BCHP_CLK_SYS_PLL_0_PLL_2_reserved0_MASK |   
                                     BCHP_CLK_SYS_PLL_0_PLL_2_DIS_CH_MASK    ,
                                     /* set these bits */
                                     BCHP_CLK_SYS_PLL_0_PLL_2_EN_CMLBUF_MASK );

                    /* Power up XPT clocks. */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_CLK_XPT_CLK_PM_CTRL   ,
                                     /* clear these bits */
                                     BCHP_CLK_XPT_CLK_PM_CTRL_reserved0_MASK          |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_XPT_20P25M_CLK_MASK |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_XPT_27M_CLK_MASK    |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_XPT_40P5M_CLK_MASK  |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_XPT_54M_CLK_MASK    |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_XPT_81M_CLK_MASK    |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_216M_CLK_MASK       |
                                     BCHP_CLK_XPT_CLK_PM_CTRL_DIS_108M_CLK_MASK       ,
                                     /* set these bits */
                                     0 );
                #endif /* (BCHP_CHIP==7342)  */

                #if (BCHP_CHIP==7340)
            
                    /* Power up PLL_MAIN_CH3 81MHz core_xpt clock. */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLKGEN_PLLMAIN_CH3_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_PLLMAIN_CH3_PM_CTRL_reserved0_MASK         |
                                     BCHP_CLKGEN_PLLMAIN_CH3_PM_CTRL_PWRDN_CH3_PLLMAIN_MASK ,
                                     /* set these bits */
                                     0 );

                    /* Power up XPT clocks. */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLKGEN_XPT_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_reserved0_MASK     |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_20P25_MASK |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_27_MASK    |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_40P5_MASK  |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_54_MASK    |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_81_MASK    |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_216_MASK   |
                                     BCHP_CLKGEN_XPT_CLK_PM_CTRL_DIS_CLK_108_MASK   ,
                                     /* set these bits */
                                     0 );
                #endif /* (BCHP_CHIP==7340)  */
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_XVD_ID))
        {
            BDBG_MSG(("Power Up: AVD"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                #if (BCHP_CHIP==7342)

                    /* Power up AVD_PLL_CTRL */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AVD_PLL_CTRL ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AVD_PLL_CTRL_reserved0_MASK  |
                                     BCHP_VCXO_CTL_MISC_AVD_PLL_CTRL_RESET_MASK      |
                                     BCHP_VCXO_CTL_MISC_AVD_PLL_CTRL_POWERDOWN_MASK  .
                                     /* set these bits */
                                     0 );
        
                    /* Power up AVD_PM_EN_CML_BUF_1 */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_VCXO_CTL_MISC_AVD_PM_EN_CML_BUF_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AVD_PM_EN_CML_BUF_1_reserved0_MASK ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AVD_PM_EN_CML_BUF_1_EN_CMLBUF_MASK );

                    /* Power up AVD_PM_DIS_CHL_1 */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_VCXO_CTL_MISC_AVD_PM_DIS_CHL_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AVD_PM_DIS_CHL_1_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_AVD_PM_DIS_CHL_1_DIS_CH_MASK    ,
                                     /* set these bits */
                                     0 );

                    /* Power up AVD_PM_CLOCK_ENA_1 */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AVD_PM_CLOCK_ENA_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AVD_PM_CLOCK_ENA_1_reserved0_MASK  ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AVD_PM_CLOCK_ENA_1_CLOCK_ENA_MASK  );
                    
                    /* Power up AVD clocks */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLK_AVD0_CLK_PM_CTRL   ,
                                     /* clear these bits */
                                     BCHP_CLK_AVD0_CLK_PM_CTRL_reserved0_MASK         |
                                     BCHP_CLK_AVD0_CLK_PM_CTRL_DIS_AVD0_PROG_CLK_MASK |
                                     BCHP_CLK_AVD0_CLK_PM_CTRL_DIS_216M_CLK_MASK      |
                                     BCHP_CLK_AVD0_CLK_PM_CTRL_DIS_108M_CLK_MASK      ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7342)  */

                #if (BCHP_CHIP==7340)

                    /* Power up AVD 250, 216, and 108 clocks */ 
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLKGEN_AVD_CLK_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_AVD_CLK_PM_CTRL_reserved0_MASK       |
                                     BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_250P71_MASK  |
                                     BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_216_MASK     |
                                     BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_108_MASK     ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7340)  */
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Up: VDC"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                
                #if (BCHP_CHIP==7342)
                        
                    /* Power up BVN_TOP 216 MHz and 108 MHz clocks */
                    BPWR_RegUpdate32(pPwr->hRegister,       BCHP_CLK_BVN_TOP_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLK_BVN_TOP_PM_CTRL_reserved0_MASK       |
                                     BCHP_CLK_BVN_TOP_PM_CTRL_DIS_216M_CLK_MASK    |
                                     BCHP_CLK_BVN_TOP_PM_CTRL_DIS_108M_CLK_MASK    ,
                                     /* set these bits */
                                     0 );
                
                #endif /* (BCHP_CHIP==7342)  */

                #if (BCHP_CHIP==7340)
                        
                    /* Power up BVN_TOP 216 MHz and 108 MHz clocks */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_CLKGEN_BVN_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_BVN_CLK_PM_CTRL_reserved0_MASK     |
                                     BCHP_CLKGEN_BVN_CLK_PM_CTRL_DIS_CLK_216_MASK   |
                                     BCHP_CLKGEN_BVN_CLK_PM_CTRL_DIS_CLK_108_MASK   ,
                                     /* set these bits */
                                     0 );
                
                #endif /* (BCHP_CHIP==7340)  */
                
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_GRC_ID) )
        {
            BDBG_MSG(("Power Up: GFX 2D"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)
                #if (BCHP_CHIP==7342)

                    /* Power up GFX 3D */
                    /* GFX 3D 108 in pm ctrl reg is not used. It uses only 216 MHz. */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLK_GFX_3D_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLK_GFX_3D_PM_CTRL_reserved0_MASK     |
                                     BCHP_CLK_GFX_3D_PM_CTRL_DIS_108M_CLK_MASK  |
                                     BCHP_CLK_GFX_3D_PM_CTRL_DIS_216M_CLK_MASK  ,
                                     /* set these bits */
                                     0 );
        
                    /* Power up GFX 2D */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLK_GFX_2D_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLK_GFX_2D_PM_CTRL_reserved0_MASK     |
                                     BCHP_CLK_GFX_2D_PM_CTRL_DIS_108M_CLK_MASK  |
                                     BCHP_CLK_GFX_2D_PM_CTRL_DIS_216M_CLK_MASK  ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7342)  */

                #if (BCHP_CHIP==7340)

                    /* Power up GFX 3D 216M and 108M clocks */
                    BPWR_RegUpdate32(pPwr->hRegister,       BCHP_CLKGEN_GFX_CLK_PM_CTRL_3D  ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_3D_reserved0_MASK      |
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_3D_DIS_CLK_216_MASK    |
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_3D_DIS_CLK_108_MASK    ,
                                     /* set these bits */
                                     0 );

                    /* Power up GFX 2D 216M and 108M clocks */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_CLKGEN_GFX_CLK_PM_CTRL_2D ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_2D_reserved0_MASK      |
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_2D_DIS_CLK_216_MASK    |
                                     BCHP_CLKGEN_GFX_CLK_PM_CTRL_2D_DIS_CLK_108_MASK    ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7340)  */
            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Up: VEC"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)

                #if (BCHP_CHIP==7342)

                    /* Power up VEC */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLK_VEC_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLK_VEC_CLK_PM_CTRL_reserved0_MASK            |
                                     BCHP_CLK_VEC_CLK_PM_CTRL_DIS_VEC_DAC_108M_CLK_MASK |
                                     BCHP_CLK_VEC_CLK_PM_CTRL_DIS_216M_CLK_MASK         |
                                     BCHP_CLK_VEC_CLK_PM_CTRL_DIS_108M_CLK_MASK         ,
                                     /* set these bits */
                                     0 );

                    /* Power up VEC  656 */ 
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLK_VEC_656_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLK_VEC_656_CLK_PM_CTRL_reserved0_MASK            |
                                     BCHP_CLK_VEC_656_CLK_PM_CTRL_DIS_VEC_VCXO_656_CLK_MASK ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7342)  */

                #if (BCHP_CHIP==7340)

                    /* Power up VEC  216 */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_CLKGEN_VEC_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_VEC_CLK_PM_CTRL_reserved0_MASK     |
                                     BCHP_CLKGEN_VEC_CLK_PM_CTRL_DIS_CLK_108_MASK   |
                                     BCHP_CLKGEN_VEC_CLK_PM_CTRL_DIS_CLK_216_MASK   ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7340)  */

            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_RAP_ID))
        {
            BDBG_MSG(("Power Up: RAP AIO"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)

                #if (BCHP_CHIP==7342)

                    /* Power up RPTD AIO 108 clocks  "*/
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_CLK_RPT_AIO_CLK_PM_CTRL    ,
                                     /* clear these bits */
                                     BCHP_CLK_RPT_AIO_CLK_PM_CTRL_reserved0_MASK        |
                                     BCHP_CLK_RPT_AIO_CLK_PM_CTRL_DIS_108M_CLK_MASK     ,
                                     /* set these bits */
                                     0 );

                    /* Power up RAP_PLL */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_RAP_PLL_CTRL,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PLL_CTRL_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_RAP_PLL_CTRL_RESERVED_MASK  |
                                     BCHP_VCXO_CTL_MISC_RAP_PLL_CTRL_RESET_MASK     |
                                     BCHP_VCXO_CTL_MISC_RAP_PLL_CTRL_POWERDOWN_MASK ,
                                     /* set these bits */
                                     0 );

                    /* Power up RAP_PLL_CHL_1 CML buffer */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_VCXO_CTL_MISC_RAP_PM_EN_CML_BUF_1,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PM_EN_CML_BUF_1_reserved0_MASK  ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PM_EN_CML_BUF_1_EN_CMLBUF_MASK  );

                    /* Start up PLL channel 1 */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_VCXO_CTL_MISC_RAP_PM_DIS_CHL_1,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PM_DIS_CHL_1_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_RAP_PM_DIS_CHL_1_DIS_CH_MASK    ,
                                     /* set these bits */
                                     0 );

                    /* Enable PLL channel 1 clock output */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_VCXO_CTL_MISC_RAP_PM_CLOCK_ENA_1  ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PM_CLOCK_ENA_1_reserved0_MASK       ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_RAP_PM_CLOCK_ENA_1_CLOCK_ENA_MASK       );

                    /* Startup the 250 MHz clock by CLK_RPT_AIO_CLK_PM_CTRL [2] first,     */
                    /* then startup PLL in the VCXO_CTL_MISC_RAP_PM_CLOCK_ENA_1            */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_CLK_RPT_AIO_CLK_PM_CTRL    ,
                                     /* clear these bits */
                                     BCHP_CLK_RPT_AIO_CLK_PM_CTRL_reserved0_MASK        |
                                     BCHP_CLK_RPT_AIO_CLK_PM_CTRL_DIS_RPT_PROG_CLK_MASK |
                                     BCHP_CLK_RPT_AIO_CLK_PM_CTRL_DIS_216M_CLK_MASK     ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7342)  */

                #if (BCHP_CHIP==7340)

                    /* Power up RPTD AIO clocks               */
                    BPWR_RegUpdate32(pPwr->hRegister,  BCHP_CLKGEN_RPT_AIO_CLK_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_RPT_AIO_CLK_PM_CTRL_reserved0_MASK             |
                                     BCHP_CLKGEN_RPT_AIO_CLK_PM_CTRL_DIS_CLK_250P71_RPTD_MASK   |
                                     BCHP_CLKGEN_RPT_AIO_CLK_PM_CTRL_DIS_CLK_216_AIO_MASK       |
                                     BCHP_CLKGEN_RPT_AIO_CLK_PM_CTRL_DIS_CLK_108_AIO_MASK       ,
                                     /* set these bits */
                                     0 );

                    /* Power up AC2 PLL. */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_VCXO_CTL_MISC_AC2_CTRL  ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_CTRL_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_AC2_CTRL_RESERVED_MASK  |
                                     BCHP_VCXO_CTL_MISC_AC2_CTRL_RESET_MASK     |
                                     BCHP_VCXO_CTL_MISC_AC2_CTRL_POWERDOWN_MASK ,
                                     /* set these bits */
                                     0 );

                    /* Power up channel 3 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_3,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_3_reserved0_MASK    |
                                     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_3_DIS_CH_MASK        ,
                                     /* set these bits */
                                     0 );

                    /* Power up channel 2 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_2,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_2_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_2_DIS_CH_MASK    ,
                                     /* set these bits */
                                     0 );

                    /* Power up channel 1 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_1_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_AC2_PM_DIS_CHL_1_DIS_CH_MASK    ,
                                     /* set these bits */
                                     0 );

                    /* Enable AC2 output clock first */
                    BPWR_RegUpdate32(pPwr->hRegister,  BCHP_VCXO_CTL_MISC_AC2_PM_CLOCK_ENA_1,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_PM_CLOCK_ENA_1_reserved0_MASK   ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC2_PM_CLOCK_ENA_1_CLOCK_ENA_MASK   );

                    /* Power up AC1 PLL. */
                    BPWR_RegUpdate32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC1_CTRL ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_CTRL_reserved0_MASK   |
                                     BCHP_VCXO_CTL_MISC_AC1_CTRL_RESERVED_MASK    |
                                     BCHP_VCXO_CTL_MISC_AC1_CTRL_RESET_MASK       |
                                     BCHP_VCXO_CTL_MISC_AC1_CTRL_POWERDOWN_MASK   ,
                                     /* set these bits */
                                     0 );

                    /* Power up channel 3 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_3   ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_3_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_3_DIS_CH_MASK    ,
                                     /* set these bits */
                                     0 );

                    /* Power up channel 2 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_2,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_2_reserved0_MASK    |
                                     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_2_DIS_CH_MASK       ,
                                     /* set these bits */
                                     0 );

                    /* Power up channel 1 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_1_reserved0_MASK     |
                                     BCHP_VCXO_CTL_MISC_AC1_PM_DIS_CHL_1_DIS_CH_MASK        ,
                                     /* set these bits */
                                     0 );

                    /* Enable AC1 output clock */
                    BPWR_RegUpdate32(pPwr->hRegister,  BCHP_VCXO_CTL_MISC_AC1_PM_CLOCK_ENA_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_PM_CLOCK_ENA_1_reserved0_MASK   ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC1_PM_CLOCK_ENA_1_CLOCK_ENA_MASK   );

                #endif /* (BCHP_CHIP==7340)  */

                /* Power up AC0.  */
                BPWR_RegUpdate32(pPwr->hRegister, BCHP_VCXO_CTL_MISC_AC0_CTRL ,
                                 /* clear these bits */
                                 BCHP_VCXO_CTL_MISC_AC0_CTRL_reserved0_MASK   |
                                 BCHP_VCXO_CTL_MISC_AC0_CTRL_RESERVED_MASK    |
                                 BCHP_VCXO_CTL_MISC_AC0_CTRL_RESET_MASK       |
                                 BCHP_VCXO_CTL_MISC_AC0_CTRL_POWERDOWN_MASK   ,
                                 /* set these bits */
                                 0 );

                #if (BCHP_CHIP==7340)

                    /* Power up channel 3 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_3 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_3_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_3_DIS_CH_MASK    ,
                                     /* set these bits */
                                     0 );
    
                    /* Power up channel 2 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_2 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_2_reserved0_MASK |
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_2_DIS_CH_MASK    ,
                                     /* set these bits */
                                     0 );
    
                    /* Power up channel 1 post divider */
                    BPWR_RegUpdate32(pPwr->hRegister,   BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_1 ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_1_reserved0_MASK     |
                                     BCHP_VCXO_CTL_MISC_AC0_PM_DIS_CHL_1_DIS_CH_MASK        ,
                                     /* set these bits */
                                     0 );
    
                    /* Enable AC0 output clock last */
                    BPWR_RegUpdate32(pPwr->hRegister,  BCHP_VCXO_CTL_MISC_AC0_PM_CLOCK_ENA_1    ,
                                     /* clear these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_CLOCK_ENA_1_reserved0_MASK   ,
                                     /* set these bits */
                                     BCHP_VCXO_CTL_MISC_AC0_PM_CLOCK_ENA_1_CLOCK_ENA_MASK   );
    
                #endif /* (BCHP_CHIP==7340)  */

                /* Power up HifiDriver0 */
                BPWR_RegUpdate32(pPwr->hRegister, BCHP_AIO_MISC_PWRDOWN ,
                                 /* clear these bits */
                                 BCHP_AIO_MISC_PWRDOWN_reserved0_MASK   |
                                 BCHP_AIO_MISC_PWRDOWN_DAC0_MASK        ,
                                 /* set these bits */
                                 0 );

            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */

        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_VDC_ID) && BPWR_P_IsModuleChanging(pPwr, BSTD_VBI_ID))
        {
            BDBG_MSG(("Power Up: VDC DACs"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)

                #if (BCHP_CHIP==7342)

                    /* Power up TDAC_1 108 MHz clock */
                    BPWR_RegUpdate32(pPwr->hRegister,      BCHP_CLK_TDAC_1_CLK_PM_CTRL     ,
                                     /* clear these bits */
                                     BCHP_CLK_TDAC_1_CLK_PM_CTRL_reserved0_MASK              |
                                     BCHP_CLK_TDAC_1_CLK_PM_CTRL_DIS_TDAC_1_108M_CLK_MASK    ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7342)  */

                #if (BCHP_CHIP==7340)

                    /* Power up TDAC_1 216MHz clock */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLKGEN_TDAC_CLK_PM_CTRL   ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_TDAC_CLK_PM_CTRL_reserved0_MASK            |
                                     BCHP_CLKGEN_TDAC_CLK_PM_CTRL_DIS_CLK_216_TDAC1_MASK    ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7340)  */

                /* Power up TDAC_1_5 */
                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_5_CTRL  ,
                                 /* clear these bits */
                                 BCHP_MISC_DAC_5_CTRL_reserved0_MASK     |
                                 BCHP_MISC_DAC_5_CTRL_PWRDN_MASK         ,
                                 /* set these bits */
                                 BCHP_MISC_DAC_5_CTRL_PWRUP_BAIS_MASK    );

                /* Power up TDAC_1_4 */
                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_4_CTRL  ,
                                 /* clear these bits */
                                 BCHP_MISC_DAC_4_CTRL_reserved0_MASK    |
                                 BCHP_MISC_DAC_4_CTRL_PWRDN_MASK        ,
                                 /* set these bits */
                                 BCHP_MISC_DAC_4_CTRL_PWRUP_BAIS_MASK   );

                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_3_CTRL  ,
                                 /* clear these bits */
                                 BCHP_MISC_DAC_3_CTRL_reserved0_MASK     |
                                 BCHP_MISC_DAC_3_CTRL_PWRDN_MASK         ,
                                 /* set these bits */
                                 BCHP_MISC_DAC_3_CTRL_PWRUP_BAIS_MASK    );

                /* Power up TDAC_1 */
                BPWR_RegUpdate32(pPwr->hRegister,    BCHP_MISC_DAC_BG_CTRL_1    ,
                                 /* clear these bits */
                                 BCHP_MISC_DAC_BG_CTRL_1_reserved0_MASK         |
                                 BCHP_MISC_DAC_BG_CTRL_1_PWRDN_REFAMP_MASK      |
                                 BCHP_MISC_DAC_BG_CTRL_1_PWRDN_MASK             ,
                                 /* set these bits */
                                 0 );

                #if (BCHP_CHIP==7342)

                    /* Power up TDAC_0 108MHz clock */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLK_TDAC_0_CLK_PM_CTRL     ,
                                     /* clear these bits */
                                     BCHP_CLK_TDAC_0_CLK_PM_CTRL_reserved0_MASK           |
                                     BCHP_CLK_TDAC_0_CLK_PM_CTRL_DIS_TDAC_0_108M_CLK_MASK ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7342)  */

                #if (BCHP_CHIP==7340)

                    /* Power up TDAC_0 216MHz clock */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLKGEN_TDAC_CLK_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_TDAC_CLK_PM_CTRL_reserved0_MASK          |
                                     BCHP_CLKGEN_TDAC_CLK_PM_CTRL_DIS_CLK_216_TDAC0_MASK  ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7340)  */

                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_2_CTRL  ,
                                 /* clear these bits */
                                 BCHP_MISC_DAC_2_CTRL_reserved0_MASK     |
                                 BCHP_MISC_DAC_2_CTRL_PWRDN_MASK         ,
                                 /* set these bits */
                                 BCHP_MISC_DAC_2_CTRL_PWRUP_BAIS_MASK    );

                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_1_CTRL  ,
                                 /* clear these bits */
                                 BCHP_MISC_DAC_1_CTRL_reserved0_MASK     |
                                 BCHP_MISC_DAC_1_CTRL_PWRDN_MASK         ,
                                 /* set these bits */
                                 BCHP_MISC_DAC_1_CTRL_PWRUP_BAIS_MASK    );

                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_0_CTRL  ,
                                 /* clear these bits */
                                 BCHP_MISC_DAC_0_CTRL_reserved0_MASK     |
                                 BCHP_MISC_DAC_0_CTRL_PWRDN_MASK         ,
                                 /* set these bits */
                                 BCHP_MISC_DAC_0_CTRL_PWRUP_BAIS_MASK    );

                BPWR_RegUpdate32(pPwr->hRegister,   BCHP_MISC_DAC_BG_CTRL_0   ,
                                 /* clear these bits */
                                 BCHP_MISC_DAC_BG_CTRL_0_reserved0_MASK    |
                                 BCHP_MISC_DAC_BG_CTRL_0_PWRDN_REFAMP_MASK |
                                 BCHP_MISC_DAC_BG_CTRL_0_PWRDN_MASK        ,
                                 /* set these bits */
                                 0 );

            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        if (BPWR_P_IsModuleChanging(pPwr, BSTD_UHF_ID))
        {
            BDBG_MSG(("Power Up: UHF Receiver"));

            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)

                #if (BCHP_CHIP==7340)

                    /* Power up UHF clocks. Will save some power in 1.2v Digital */
                    BPWR_RegUpdate32(pPwr->hRegister,    BCHP_CLKGEN_UHFR_CLK_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_UHFR_CLK_PM_CTRL_reserved0_MASK                |
                                     BCHP_CLKGEN_UHFR_CLK_PM_CTRL_DIS_CLK_54X_FILT_UHFR_MASK    |
                                     BCHP_CLKGEN_UHFR_CLK_PM_CTRL_DIS_CLK_54X_DIG_UHFR_MASK     |
                                     BCHP_CLKGEN_UHFR_CLK_PM_CTRL_DIS_CLK_54X_UHF_MASK          |
                                     BCHP_CLKGEN_UHFR_CLK_PM_CTRL_DIS_CLK_27X_UHFR_MASK         ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7340)  */

                #if (BCHP_CHIP==7342)

                    #if 0
                            /* Dont mess with pinmux settings unless we need to */
                            /* Power up UHF external LNA */
                            BPWR_RegUpdate32(pPwr->hRegister,   BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21  ,
                                             /* clear these bits */
                                             BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_05_MASK        |
                                             BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21_sgpio_04_MASK        ,
                                             /* set these bits */
                                             0 );  /* not sure what to set these fields to.  */
                    #endif

                    /* Power up UHF clocks. will save some power in 1.2v Digital */
                    BPWR_RegUpdate32(pPwr->hRegister,     BCHP_CLK_UHFR_CLK_PM_CTRL  ,
                                     /* clear these bits */
                                     BCHP_CLK_UHFR_CLK_PM_CTRL_reserved0_MASK           |
                                     BCHP_CLK_UHFR_CLK_PM_CTRL_DIS_ANA_UHFR_CLK_MASK    |
                                     BCHP_CLK_UHFR_CLK_PM_CTRL_DIS_DIGI_UHFR_CLK_MASK   ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7342)  */

                /* These AUTOPD1 bits are controlled by the UHF PI              */
                /* Target.SUNDRY.UHFR_BLOCK.UHFR_GLBL.AUTOPD1.AUTO_PD_EN = 0    */
                /* Target.SUNDRY.UHFR_BLOCK.UHFR_GLBL.AUTOPD1.MANUAL_PD = 1     */

            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */
        }

        /* Power Up HDMI */
        if (BPWR_P_IsModuleChanging(pPwr, BSTD_HDM_ID))
        {
            BDBG_MSG(("Power Up: HDM"));


            #if (BCHP_CHIP==7340 || BCHP_CHIP==7342)

                #if (BCHP_CHIP==7340)

                    /* power up all HDMI clocks */
                    BPWR_RegUpdate32(pPwr->hRegister, BCHP_CLKGEN_DVPHT_CLK_PM_CTRL ,
                                     /* clear these bits */
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_reserved0_MASK               |
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_reserved_for_eco1_MASK       |
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_DIS_CLK_108_ALTERNATE_MASK   |
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_DIS_CLK_27X_IIC_MASK         |
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_DIS_CLK_27X_PM_HDMI_MASK     |
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_DIS_CLK_216_MASK             |
                                     BCHP_CLKGEN_DVPHT_CLK_PM_CTRL_DIS_CLK_108_MASK             ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7340)  */

                #if (BCHP_CHIP==7342)

                    BPWR_RegUpdate32(pPwr->hRegister, BCHP_CLK_DVP_HT_CLK_PM_CTRL,
                                     /* clear these bits */
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_reserved0_MASK             |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_2ND_108M_CLK_MASK      |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_VEC_216M_CLK_MASK      |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_HDMI_MAX_250M_CLK_MASK |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_HDMI_27M_CLK_MASK      |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_HDMI_PM_27M_CLK_MASK   |  /* Clear this bit for CEC up */
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_216M_CLK_MASK          |
                                     BCHP_CLK_DVP_HT_CLK_PM_CTRL_DIS_108M_CLK_MASK          ,
                                     /* set these bits */
                                     0 );

                #endif /* (BCHP_CHIP==7342)  */

                BPWR_RegUpdate32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2,
                                 /* clear these bits */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_2_I_PWRDN_CH2_MASK ,
                                 /* set these bits */
                                 0 );

                BPWR_RegUpdate32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1,
                                 /* clear these bits */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_1_I_PWR_DN_CH1_MASK ,
                                 /* set these bits */
                                 0 );

                BPWR_RegUpdate32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0,
                                 /* clear these bits */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_CTL_0_RND_PWRDN_MASK ,
                                 /* set these bits */
                                 0 );

                BPWR_RegUpdate32(pPwr->hRegister, BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL,
                                 /* clear these bits */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_reserved0_MASK          |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_reserved1_MASK          |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_0_PWRDN_MASK         |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_1_PWRDN_MASK         |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_2_PWRDN_MASK         |     
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_CK_PWRDN_MASK        |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_CEC_MASK          |  /* Clear this bit for CEC up */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PLL_PWRDN_MASK          |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_REF_COMP_PWRDN_MASK     |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_BG_MASK           |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_PWRDN_REFAMP_MASK       |  /* Clear this bit for CEC up */
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_D_RESET_MASK            |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_A_RESET_MASK            |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_CK_RESET_MASK        |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_2_RESET_MASK         |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_1_RESET_MASK         |
                                 BCHP_HDMI_TX_PHY_HDMI_TX_PHY_RESET_CTL_TX_0_RESET_MASK         ,
                                 /* set these bits */
                                 0 );

            #endif /* (BCHP_CHIP==7340 || BCHP_CHIP==7342)  */

        }
    }

    BDBG_LEAVE(BPWR_P_SetPowerMode);
    return eStatus;
}

/* End of file */

