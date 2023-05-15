/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_7360_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 9/7/12 3:44p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/7360/bast_7360_priv.c $
 * 
 * Hydra_Software_Devel/6   9/7/12 3:44p ronchan
 * SWSATFE-75: no register mapping for single channel tuner
 * 
 * Hydra_Software_Devel/5   9/7/12 10:15a ronchan
 * SWSATFE-214: renamed chip header file
 * 
 * Hydra_Software_Devel/4   9/6/12 10:48a ronchan
 * SWSATFE-75: fix isr context violation in write register for < 8Mb
 * 
 * Hydra_Software_Devel/3   6/22/12 11:29a enavarro
 * SWSATFE-190: updated fec_clock calculation for 7360
 * 
 * Hydra_Software_Devel/2   5/4/12 9:46a enavarro
 * SWSATFE-157: dont verify the register write for EQ_LUPD in
 * BAST_g3_P_WriteRegister()
 * 
 * Hydra_Software_Devel/1   4/26/12 10:39a ronchan
 * SW7360-13: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3.h"
#include "bast_g3_priv.h"
#include "bast_7360_priv.h"


BDBG_MODULE(bast_7360_priv);


uint32_t BAST_g3_ChannelIntrID[BAST_G3_MAX_CHANNELS][BAST_g3_MaxIntID] = {
   /* channel 0 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_0,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_0,
      BCHP_INT_ID_SDS_BTM_IS_0,
      BCHP_INT_ID_SDS_BRTM_IS_0,
      BCHP_INT_ID_SDS_GENTM_IS1_0,
      BCHP_INT_ID_SDS_GENTM_IS2_0,
      BCHP_INT_ID_SDS_GENTM_IS3_0,
      BCHP_INT_ID_DISEQC_TIMER1_0,
      BCHP_INT_ID_DISEQC_TIMER2_0, 
      BCHP_INT_ID_SDS_sar_vol_gt_hi_thrsh_0,
      BCHP_INT_ID_SDS_sar_vol_lt_lo_thrsh_0,
      BCHP_INT_ID_DSDN_IS_0,
      BCHP_INT_ID_DISEQC_tx_fifo_a_empty_0,
      BCHP_INT_ID_SDS_HP_IS_0,
      BCHP_INT_ID_MI2C_IS_0,
      BCHP_INT_ID_TURBO_LOCK_IS_0,
      BCHP_INT_ID_TURBO_NOT_LOCK_IS_0,
      BCHP_INT_ID_TURBO_SYNC_0,      
      BCHP_INT_ID_AFEC_LOCK_IS_0,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_0,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_0
   }
};


/******************************************************************************
 BAST_g3_P_InitHandle()
******************************************************************************/
BERR_Code BAST_g3_P_InitHandle(BAST_Handle h)
{
   BAST_g3_P_Handle *hDev = (BAST_g3_P_Handle *)(h->pImpl);

   hDev->xtalFreq = BAST_G3_XTAL_FREQ;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_InitConfig()
******************************************************************************/
BERR_Code BAST_g3_P_InitConfig(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

   if (h->channel == 0)
   {
      hChn->bHasDiseqc = true;
      hChn->bExternalTuner = false;
      hChn->bHasTunerRefPll = true;
      hChn->bHasAfec = true;
      hChn->bHasTfec = true;
      hChn->tunerRefPllChannel = 0;
      
      hChn->xportSettings.bSerial = false;
      hChn->xportSettings.bClkInv = true;
      hChn->xportSettings.bClkSup = true;
      hChn->xportSettings.bVldInv = false;
      hChn->xportSettings.bSyncInv = false;
      hChn->xportSettings.bErrInv = false;
      hChn->xportSettings.bXbert = false;
      hChn->xportSettings.bTei = true;
      hChn->xportSettings.bDelay = false;
      hChn->xportSettings.bSync1 = false;
      hChn->xportSettings.bHead4 = false;
      hChn->xportSettings.bDelHeader = false;
      hChn->xportSettings.bOpllBypass = true;
      hChn->xportSettings.bchMpegErrorMode = BAST_BchMpegErrorMode_eBchAndCrc8;
   }
   else
   {
      BDBG_ERR(("BAST_g3_P_InitConfig(): invalid channnel number"));
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
   } 
   return retCode;
}


/******************************************************************************
 BAST_g3_P_ReadRegister()
******************************************************************************/
BERR_Code BAST_g3_P_ReadRegister(
   BAST_ChannelHandle h,     /* [in] BAST channel handle */
   uint32_t           reg,   /* [in] address of register to read */
   uint32_t           *val   /* [out] contains data that was read */
)
{
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;

   *val = BREG_Read32(hDev->hRegister, reg); 
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_WriteRegister()
******************************************************************************/
BERR_Code BAST_g3_P_WriteRegister(
   BAST_ChannelHandle h,     /* [in] BAST channel handle */
   uint32_t           reg,   /* [in] address of register to write */
   uint32_t           *val   /* [in] contains data to be written */
)
{
   BERR_Code retCode = BERR_TIMEOUT;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   uint32_t retry, max_retry = 10, val2, wait_time;
   
   /* determine if wait time is necessary depending on baud rate */
   BAST_g3_P_GetRegisterWriteWaitTime(h, reg, &wait_time);

   for (retry = 0; retry < max_retry; retry++)
   {
      BREG_Write32(hDev->hRegister, reg, *val);    
      if (wait_time > 0)
      {
         BKNI_Delay(wait_time);
         
         if (reg == BCHP_SDS_EQ_LUPD)
         {
            /* skip readback for this indirect register */
            retCode = BERR_SUCCESS;
            break;
         }
         
         val2 =  BREG_Read32(hDev->hRegister, reg); 
         if (val2 == *val)
         {
            /* readback successful */
            retCode = BERR_SUCCESS;
            break;
         }
      }
      else
      {
         /* case if no delay required */
         retCode = BERR_SUCCESS;
         break;
      }
   }
   
   if (retry > 0)
   {
      BDBG_MSG(("BAST_g3_P_WriteRegister(0x%X): %d retries", reg, retry));   
   }
   
   return retCode;
}


#include "bchp_clkgen.h"
/******************************************************************************
 BAST_g3_P_GetAfecClock() - updates fecFreq
******************************************************************************/
BERR_Code BAST_g3_P_GetAfecClock(BAST_ChannelHandle h, uint32_t *pFreq)
{
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   uint32_t ndiv, pdiv, mdiv, pll_div, ctrl;
   
   if (BAST_g3_P_IsLdpcOn(h))
   {   
      /* for 7360:
         afec_clk_freq = (xtal/CLKGEN_PLL_SYS0_PLL_DIV[12:10]) * CLKGEN_PLL_SYS0_PLL_DIV[9:0] * (1/CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_0[8:1])
         afec_clk_freq = (xtal/pdiv) * (ndiv/mdiv)
      */
      pll_div = BREG_Read32(hDev->hRegister, BCHP_CLKGEN_PLL_SYS0_PLL_DIV );
      ctrl = BREG_Read32(hDev->hRegister, BCHP_CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_0);     
      ndiv = pll_div & 0x3FF;
      pdiv = (pll_div >> 10) & 0x7;
      mdiv = (ctrl >> 1) & 0xFF;
      *pFreq = ((hDev->xtalFreq >> 1) * ndiv) / (pdiv * mdiv);     
      /* BDBG_MSG(("afec_freq=%u, xtal=%u, ndiv=%d, pdiv=%d, mdiv=%d", *pFreq, hDev->xtalFreq, ndiv, pdiv, mdiv)); */
      return BERR_SUCCESS;
   }
   else
   {
      *pFreq = 0;
      return BERR_NOT_INITIALIZED;
   }   
}
