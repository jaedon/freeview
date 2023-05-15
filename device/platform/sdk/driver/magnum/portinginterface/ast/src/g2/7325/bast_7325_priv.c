/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_7325_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 9/6/12 6:22p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/7325/bast_7325_priv.c $
 * 
 * Hydra_Software_Devel/21   9/6/12 6:22p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/20   10/26/09 11:07a enavarro
 * SW7342-54: initialize chip-specific settings only in
 * BAST_73xx_P_InitHandle(); common device handle initialization has been
 * moved to BAST_73xx_P_InitHandleDefault()
 * 
 * Hydra_Software_Devel/19   8/7/09 2:43p ronchan
 * PR 57301: add option to disable AFEC for 7322 bondout
 * 
 * Hydra_Software_Devel/18   7/21/09 4:42p ronchan
 * PR 37769: no AFEC or TFEC for external demod
 * 
 * Hydra_Software_Devel/17   4/27/09 3:25p ronchan
 * PR 54581: initialize bFtmPoweredDown flag
 * 
 * Hydra_Software_Devel/16   10/15/08 4:30p ronchan
 * PR 37769: fixed comments
 * 
 * Hydra_Software_Devel/15   10/9/08 11:45a ronchan
 * PR 37769: set searchRange back to 5000000
 * 
 * Hydra_Software_Devel/14   9/22/08 2:45p ronchan
 * PR 37769: updated default fsk channel select to use enum
 * 
 * Hydra_Software_Devel/13   8/25/08 4:36p enavarro
 * PR 37769: initialized DFT_RANGE_START, DFT_RANGE_END, and DFT_SIZE
 * config parameters
 * 
 * Hydra_Software_Devel/12   7/8/08 4:32p ronchan
 * PR 37769: initialize tx power & channel select config parameters
 * 
 * Hydra_Software_Devel/11   7/7/08 3:39p ronchan
 * PR 37769: decreased vco separation to 2 MHz
 * 
 * Hydra_Software_Devel/10   5/19/08 10:18a enavarro
 * PR 37769: initialize tunerCtl in BAST_73xx_P_InitConfig()
 * 
 * Hydra_Software_Devel/9   4/28/08 2:47p ronchan
 * PR 37769: added vsense interrupts
 * 
 * Hydra_Software_Devel/8   4/10/08 11:56a ronchan
 * PR 41555: init xport_ctl to enable sync1 bit for external tuner dtv
 * serial
 * 
 * Hydra_Software_Devel/7   3/13/08 10:28a enavarro
 * PR 37769: set reacqDelay to 1000000
 * 
 * Hydra_Software_Devel/6   3/12/08 5:19p enavarro
 * PR 37769: initialized reacqDelay in BAST_73xx_P_InitHandle()
 * 
 * Hydra_Software_Devel/5   3/10/08 9:44a ronchan
 * PR 37769: initialize value of coresPoweredDown
 * 
 * Hydra_Software_Devel/4   1/22/08 11:23a enavarro
 * PR 37769: set searchRange to 5250000
 * 
 * Hydra_Software_Devel/3   12/13/07 5:52p ronchan
 * PR 37769: added channel interrupts array
 * 
 * Hydra_Software_Devel/2   12/4/07 11:49a ronchan
 * PR 37769: removed non-chip specific diseqc settings, enabled turbo on
 * channel 0
 * 
 * Hydra_Software_Devel/1   11/21/07 11:34a ronchan
 * PR 37769: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2.h"
#include "bast_g2_priv.h"
#include "bast_7325_priv.h"


BDBG_MODULE(bast_7325);


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
   #ifndef BAST_AFEC_CORE_DISABLED
      BCHP_INT_ID_AFEC_LOCK_IS_0,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_0
   #else
      0,
      0
   #endif
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
      0,
      0,
      BCHP_INT_ID_DSDN_IS_1,
      BCHP_INT_ID_SDS_HP_IS_1,
      BCHP_INT_ID_MI2C_IS_1,
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
      hChn->bHasTfec = true;
   #ifndef BAST_AFEC_CORE_DISABLED
      hChn->bHasAfec = true;
   #else
      hChn->bHasAfec = false;
   #endif
   }
   else
   {
      hChn->bHasDiseqc = false;
      hChn->bExternalTuner = true;
      hChn->bHasTunerDacPll = false;
      hChn->bHasDedicatedPll = false;
      hChn->xportCtl = 0x3A1A; /* serial out, clkinv, clksup, tei, sync1 for dtv serial */
      hChn->bHasTfec = false;
      hChn->bHasAfec = false;
   }
   hChn->tunerCtl = 0;   
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
   uint32_t addr = reg;

   if ((addr >= 0x01100000) && (addr <= 0x011007F4) && (h->channel == 1))
   {
      /* do address translation for QPSK core only */
      addr &= 0x000FFFFF;
      addr |= 0x01300000;
   }
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
   uint32_t addr = reg;

   if ((addr >= 0x01100000) && (addr <= 0x011007F4) && (h->channel == 1))
   {
      /* do address translation for QPSK core only */
      addr &= 0x000FFFFF;
      addr |= 0x01300000;
   }

   BREG_Write32(hDev->hRegister, addr, *val);
   return BERR_SUCCESS;
}


