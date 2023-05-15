/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_7342_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 9/6/12 6:26p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/7342/bast_7342_priv.c $
 * 
 * Hydra_Software_Devel/7   9/6/12 6:26p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/6   10/26/09 11:07a enavarro
 * SW7342-54: initialize chip-specific settings only in
 * BAST_73xx_P_InitHandle(); common device handle initialization has been
 * moved to BAST_73xx_P_InitHandleDefault()
 * 
 * Hydra_Software_Devel/5   8/14/09 11:36a ronchan
 * PR 55236: bypass SPLL for SDS1
 * 
 * Hydra_Software_Devel/4   8/14/09 9:46a ronchan
 * PR 55236: deselect splitter mode for tuner options
 * 
 * Hydra_Software_Devel/3   8/13/09 2:48p ronchan
 * PR 55236: fixed address translation logic
 * 
 * Hydra_Software_Devel/2   8/10/09 4:03p ronchan
 * PR 55236: update address translation
 * 
 * Hydra_Software_Devel/1   6/16/09 2:12p ronchan
 * PR 55236: add support for 7342
 *
 ***************************************************************************/

#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2.h"
#include "bast_g2_priv.h"
#include "bast_7342_priv.h"

BDBG_MODULE(bast_7342);


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
   hDev->ndiv = 55;
   hDev->m1div = 11;
   hDev->m2div = 7;
   hDev->m3div = 6;
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
      hChn->bHasDedicatedPll = false;
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
   hChn->tunerCtl = 0;
}


/******************************************************************************
 BAST_g2_P_GetRegisterAddress()
******************************************************************************/
void BAST_g2_P_GetRegisterAddress(BAST_ChannelHandle h, uint32_t reg, uint32_t *pAddr)
{
   *pAddr = reg;
   
   if (h->channel > 0)
   {
      if ((reg >= 0x00700000) && (reg <= 0x00700FFF))
      {
         /* SDS, TFEC, TUNER, SDS_INTR2, or GRB register access */
         *pAddr &= 0x00000FFF;
         if (h->channel == 1)
            *pAddr |= 0x00701000;
         else
            *pAddr |= 0x00706000;
      }
      else if ((reg >= 0x00702000) && (reg <= 0x00702FFF))
      {
         /* TFEC register access */
         *pAddr &= 0x00000FFF;
         *pAddr |= 0x00703000;
      }
      else if ((reg >= 0x00704000) && (reg <= 0x00704FFF))
      {
         /* AFEC register access */
         *pAddr &= 0x00000FFF;
         *pAddr |= 0x00705000;
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


