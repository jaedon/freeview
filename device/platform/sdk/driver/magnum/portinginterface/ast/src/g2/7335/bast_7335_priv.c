/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_7335_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 9/6/12 6:23p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/7335/bast_7335_priv.c $
 * 
 * Hydra_Software_Devel/18   9/6/12 6:23p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/17   12/21/11 3:46p ckpark
 * SWSATFE-169: added compile option (BAST_HIGH_AFEC_CLOCK) for 270MHz
 * AFEC clock
 * 
 * Hydra_Software_Devel/16   10/26/09 11:07a enavarro
 * SW7342-54: initialize chip-specific settings only in
 * BAST_73xx_P_InitHandle(); common device handle initialization has been
 * moved to BAST_73xx_P_InitHandleDefault()
 * 
 * Hydra_Software_Devel/15   4/27/09 3:26p ronchan
 * PR 54581: initialize bFtmPoweredDown flag
 * 
 * Hydra_Software_Devel/14   1/31/09 1:31a jrubio
 * PR51629: adjust 7336 support
 * 
 * Hydra_Software_Devel/13   10/15/08 4:30p ronchan
 * PR 37881: fixed comments
 * 
 * Hydra_Software_Devel/12   9/22/08 2:46p ronchan
 * PR 37881: updated default fsk channel select to use enum
 * 
 * Hydra_Software_Devel/11   8/26/08 8:56a enavarro
 * PR 37881: initialized DFT_RANGE_START, DFT_RANGE_END, and DFT_SIZE
 * config parameters
 * 
 * Hydra_Software_Devel/10   7/8/08 4:24p ronchan
 * PR 37881: initialize tx power & channel select config parameters
 * 
 * Hydra_Software_Devel/9   7/7/08 3:17p ckpark
 * PR 37881: decreased vco separation to 2 MHz
 * 
 * Hydra_Software_Devel/8   5/16/08 4:24p enavarro
 * PR 37881: set to splitter mode as default
 * 
 * Hydra_Software_Devel/7   5/15/08 4:28p enavarro
 * PR 37881: initialize VCO_SEPARATION config parameter for splitter mode
 * 
 * Hydra_Software_Devel/6   4/28/08 2:29p ronchan
 * PR 37881: added vsense interrupts
 * 
 * Hydra_Software_Devel/4   3/13/08 10:01a ronchan
 * PR 37881: initialized reacqDelay in BAST_73xx_P_InitHandle()
 * 
 * Hydra_Software_Devel/3   3/10/08 1:32p ronchan
 * PR 37881: initialize value of coresPoweredDown
 * 
 * Hydra_Software_Devel/2   12/13/07 5:50p ronchan
 * PR 37881: added channel interrupts array, disabled dac pll on channel 1
 * 
 * Hydra_Software_Devel/1   12/4/07 3:53p enavarro
 * PR 37881: initial version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2.h"
#include "bast_g2_priv.h"
#include "bast_7335_priv.h"

BDBG_MODULE(bast_7335);


/* local functions */
void BAST_g2_P_GetRegisterAddress(BAST_ChannelHandle h, uint32_t reg, uint32_t *pAddr);


uint32_t BAST_Sds_ChannelIntrID[BAST_G2_MAX_CHANNELS][BAST_Sds_MaxIntID] = {
   /* channel 0 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_0,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_0,
      BCHP_INT_ID_SDS_BTM_IS_0,
      BCHP_INT_ID_SDS_BRTM_IS_0,
      BCHP_INT_ID_SDS_GENTM_IS1_0,
      BCHP_INT_ID_SDS_GENTM_IS2_0,
      BCHP_INT_ID_SDS_XTLTM_IS_0,
      BCHP_INT_ID_SDS_if_agc_le_thresh_rise_0,
      BCHP_INT_ID_SDS_if_agc_le_thresh_fall_0,
      BCHP_INT_ID_SDS_rf_agc_le_thresh_rise_0,
      BCHP_INT_ID_SDS_rf_agc_le_thresh_fall_0,
      BCHP_INT_ID_SDS_rf_agc_gt_max_change_0,
      BCHP_INT_ID_SDS_sar_vol_gt_hi_thrsh_0,
      BCHP_INT_ID_SDS_sar_vol_lt_lo_thrsh_0,
      BCHP_INT_ID_DSDN_IS_0,
      BCHP_INT_ID_SDS_HP_IS_0,
      BCHP_INT_ID_MI2C_IS_0,
      BCHP_INT_ID_TURBO_LOCK_IS_0,
      BCHP_INT_ID_TURBO_NOT_LOCK_IS_0,
      BCHP_INT_ID_AFEC_LOCK_IS_0,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_0
   },
   /* channel 1 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_1,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_1,
      BCHP_INT_ID_SDS_BTM_IS_1,
      BCHP_INT_ID_SDS_BRTM_IS_1,
      BCHP_INT_ID_SDS_GENTM_IS1_1,
      BCHP_INT_ID_SDS_GENTM_IS2_1,
      BCHP_INT_ID_SDS_XTLTM_IS_1,
      BCHP_INT_ID_SDS_if_agc_le_thresh_rise_1,
      BCHP_INT_ID_SDS_if_agc_le_thresh_fall_1,
      BCHP_INT_ID_SDS_rf_agc_le_thresh_rise_1,
      BCHP_INT_ID_SDS_rf_agc_le_thresh_fall_1,
      BCHP_INT_ID_SDS_rf_agc_gt_max_change_1,
      BCHP_INT_ID_SDS_sar_vol_gt_hi_thrsh_1,
      BCHP_INT_ID_SDS_sar_vol_lt_lo_thrsh_1,
      BCHP_INT_ID_DSDN_IS_1,
      BCHP_INT_ID_SDS_HP_IS_1,
      BCHP_INT_ID_MI2C_IS_1,
      BCHP_INT_ID_TURBO_LOCK_IS_1,
      BCHP_INT_ID_TURBO_NOT_LOCK_IS_1,
      BCHP_INT_ID_AFEC_LOCK_IS_1,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_1
   },
   /* channel 2 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_2,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_2,
      BCHP_INT_ID_SDS_BTM_IS_2,
      BCHP_INT_ID_SDS_BRTM_IS_2,
      BCHP_INT_ID_SDS_GENTM_IS1_2,
      BCHP_INT_ID_SDS_GENTM_IS2_2,
      BCHP_INT_ID_SDS_XTLTM_IS_2,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      BCHP_INT_ID_MI2C_IS_2,
      0,
      0,
      0,
      0
   }
};


/******************************************************************************
 BAST_g2_P_InitHandle()
******************************************************************************/
void BAST_g2_P_InitHandle(BAST_Handle h)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);

   hDev->xtalFreq = BAST_G2_XTAL_FREQ;

#ifdef BAST_HIGH_AFEC_CLOCK
   hDev->m1div = 8; /* Fs-> 8: 135 MHz, 6: 180 MHz */
   hDev->m2div = 5;
   hDev->m3div = 4;  /*AFEC clk->  5: 216 MHz, 4: 270 MHz */
#endif     
}


/******************************************************************************
 BAST_g2_P_InitConfig()
******************************************************************************/
void BAST_g2_P_InitConfig(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   if (h->channel == 0)
   {
      hChn->bHasDiseqc = true;
      hChn->bExternalTuner = false;
      hChn->bHasDedicatedPll = true;
      hChn->bHasTunerDacPll = true;
      hChn->xportCtl = 0x1A18; /* parallel out, clkinv, clksup, tei */
      hChn->bHasAfec = true;
      hChn->bHasTfec = true;
   }
   else if (h->channel == 1)
   {
      hChn->bHasDiseqc = true;
      hChn->bExternalTuner = false;
      hChn->bHasDedicatedPll = true;
      hChn->bHasTunerDacPll = false;
      hChn->xportCtl = 0x1A18; /* parallel out, clkinv, clksup, tei */
      hChn->bHasAfec = true;
      hChn->bHasTfec = true;
   }
   else
   {
      hChn->bHasDiseqc = false;
      hChn->bExternalTuner = true;
      hChn->bHasTunerDacPll = false;
      hChn->bHasDedicatedPll = false;
      hChn->xportCtl = 0x1A18; /* parallel out, clkinv, clksup, tei */
      hChn->bHasAfec = false;
      hChn->bHasTfec = false;
   }
#if ((BCHP_REV_A1) || (BCHP_REV_A0))
   hChn->tunerCtl = BAST_G2_TUNER_CTL_ENABLE_SPLITTER_MODE;
#else
   hChn->tunerCtl = 0;
#endif    
}


/******************************************************************************
 BAST_g2_P_GetRegisterAddress()
******************************************************************************/
void BAST_g2_P_GetRegisterAddress(BAST_ChannelHandle h, uint32_t reg, uint32_t *pAddr)
{
   *pAddr = reg;
   
   if (h->channel > 0)
   {
      if ((reg >= 0x01100000) && (reg <= 0x01100FFF))
      {
         /* SDS, TFEC, TUNER, SDS_INTR2, or GRB register access */
         *pAddr &= 0x000FFFFF;
         if (h->channel == 1)
            *pAddr |= 0x01200000;
         else
            *pAddr |= 0x01500000;
      }
      else if ((reg >= 0x01300000) && (reg <= 0x01300FFF))
      {
         /* AFEC register access */
         *pAddr &= 0x000FFFFF;
         *pAddr |= 0x01400000;
      }
   }
}


/******************************************************************************
 BAST_g2_P_ReadRegister()
******************************************************************************/
BERR_Code BAST_g2_P_ReadRegister(
   BAST_ChannelHandle h,     /* [in] BAST channel handle */
   uint32_t           reg,   /* [in] address of register to read */
   uint32_t           *val   /* [out] contains data that was read */
)
{
   BAST_g2_P_Handle *hDev = h->pDevice->pImpl;
   uint32_t addr;

   BAST_g2_P_GetRegisterAddress(h, reg, &addr);
   *val = BREG_Read32(hDev->hRegister, addr); 
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_WriteRegister()
******************************************************************************/
BERR_Code BAST_g2_P_WriteRegister(
   BAST_ChannelHandle h,     /* [in] BAST channel handle */
   uint32_t           reg,   /* [in] address of register to write */
   uint32_t           *val   /* [in] contains data to be written */
)
{
   BAST_g2_P_Handle *hDev = h->pDevice->pImpl;
   uint32_t addr;

   BAST_g2_P_GetRegisterAddress(h, reg, &addr);
   BREG_Write32(hDev->hRegister, addr, *val);
   return BERR_SUCCESS;
}


