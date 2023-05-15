/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_7344_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 9/7/12 3:44p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/7344/bast_7344_priv.c $
 * 
 * Hydra_Software_Devel/20   9/7/12 3:44p ronchan
 * SWSATFE-75: no register mapping for single channel tuner
 * 
 * Hydra_Software_Devel/19   9/7/12 10:02a ronchan
 * SWSATFE-214: renamed chip header file
 * 
 * Hydra_Software_Devel/18   9/6/12 10:46a ronchan
 * SWSATFE-75: fix isr context violation in write register for < 8Mb
 * 
 * Hydra_Software_Devel/17   6/21/12 11:40a ckpark
 * SWSATFE-203: add an extra divide by 2 in the afec_clock calculation
 * 
 * Hydra_Software_Devel/16   6/21/12 10:57a ckpark
 * SWSATFE-203: updated fec clock calculation
 * 
 * Hydra_Software_Devel/15   5/4/12 9:44a enavarro
 * SWSATFE-157: dont verify the register write for EQ_LUPD in
 * BAST_g3_P_WriteRegister()
 * 
 * Hydra_Software_Devel/14   4/20/12 2:58p ronchan
 * SWSATFE-75: add diseqc almost empty interrupt
 * 
 * Hydra_Software_Devel/13   11/30/11 3:44p enavarro
 * SWSATFE-157: fix compiler warning
 * 
 * Hydra_Software_Devel/12   11/30/11 2:47p enavarro
 * SWSATFE-157: use common function, BAST_g3_P_GetRegisterWriteWaitTime(),
 * to determine if register is in baud clock domain
 * 
 * Hydra_Software_Devel/11   11/10/11 9:41a enavarro
 * SWSATFE-157: fixed eq register access for low baud rates
 * 
 * Hydra_Software_Devel/10   11/7/11 4:19p ronchan
 * SWSATFE-140: identify controlling channel of ref pll
 * 
 * Hydra_Software_Devel/9   7/27/11 4:38p enavarro
 * SWSATFE-136: updated debug msg
 * 
 * Hydra_Software_Devel/8   7/27/11 3:15p guangcai
 * SWSATFE-136: implement workaround for register accesses to VIT/RS cores
 * in low baud rate
 * 
 * Hydra_Software_Devel/7   6/16/11 9:09a enavarro
 * SWSATFE-127: added fine freq offset estimation for DVB-S2
 * 
 * Hydra_Software_Devel/6   4/29/11 11:33a enavarro
 * SWSATFE-75: bypass opll
 * 
 * Hydra_Software_Devel/5   3/1/11 10:42a ckpark
 * SWSATFE-75: deleted unused variable declaration
 * 
 * Hydra_Software_Devel/4   2/28/11 10:37a ckpark
 * SWSATFE-75: commented out print routine
 * 
 * Hydra_Software_Devel/3   2/25/11 3:52p ckpark
 * SWSATFE-75: change afec clock calculation
 * 
 * Hydra_Software_Devel/2   12/8/10 2:14p enavarro
 * SWSATFE-75: added turbo sync detect interrupt
 * 
 * Hydra_Software_Devel/1   11/15/10 5:21p ronchan
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3.h"
#include "bast_g3_priv.h"
#include "bast_7344_priv.h"


BDBG_MODULE(bast_7344_priv);


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
      0
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
      /* for 7344: AFEC_0 = (xtal/CLKGEN_PLL_RAAGA_PLL_DIV[12:10]) * CLKGEN_PLL_RAAGA_PLL_DIV[9:0] / CLKGEN_PLL_RAAGA_PLL_CHANNEL_CTRL_CH_1[8:1]  */
      pll_div = BREG_Read32(hDev->hRegister, BCHP_CLKGEN_PLL_RAAGA_PLL_DIV);
      ctrl = BREG_Read32(hDev->hRegister, BCHP_CLKGEN_PLL_RAAGA_PLL_CHANNEL_CTRL_CH_1);     
      ndiv = pll_div & 0x3FF;
      pdiv = (pll_div >> 10) & 0x7;
      mdiv = (ctrl >> 1) & 0xFF;
      *pFreq = (((hDev->xtalFreq / pdiv) * ndiv) / mdiv) / 2; /* do extra div by 2 */
      /* BDBG_ERR(("afec_freq=%u, ndiv=%d, pdiv=%d, mdiv=%d", *pFreq, ndiv, pdiv, mdiv));  */
      return BERR_SUCCESS;
   }
   else
   {
      *pFreq = 0;
      return BERR_NOT_INITIALIZED;
   }   
}
