/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_7358_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/24 $
 * $brcm_Date: 9/7/12 3:44p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/7358/bast_7358_priv.c $
 * 
 * Hydra_Software_Devel/24   9/7/12 3:44p ronchan
 * SWSATFE-75: no register mapping for single channel tuner
 * 
 * Hydra_Software_Devel/23   9/7/12 10:15a ronchan
 * SWSATFE-214: renamed chip header file
 * 
 * Hydra_Software_Devel/22   9/6/12 10:47a ronchan
 * SWSATFE-75: fix isr context violation in write register for < 8Mb
 * 
 * Hydra_Software_Devel/21   5/4/12 9:32a ckpark
 * SWSATFE-157: dont verify the register write for EQ_LUPD in
 * BAST_g3_P_WriteRegister()
 * 
 * Hydra_Software_Devel/20   4/20/12 2:55p ronchan
 * SWSATFE-75: add diseqc almost empty interrupt
 * 
 * Hydra_Software_Devel/19   11/30/11 3:38p enavarro
 * SWSATFE-157: fixed compiler warning
 * 
 * Hydra_Software_Devel/18   11/30/11 2:48p enavarro
 * SWSATFE-157: use common function, BAST_g3_P_GetRegisterWriteWaitTime(),
 * to determine if register is in baud clock domain
 * 
 * Hydra_Software_Devel/17   11/30/11 11:19a enavarro
 * SWSATFE-157: fixed cl register access for low baud rates
 * 
 * Hydra_Software_Devel/16   11/10/11 9:41a enavarro
 * SWSATFE-157: fixed eq register access for low baud rates
 * 
 * Hydra_Software_Devel/15   11/7/11 3:12p ronchan
 * SWSATFE-140: identify controlling channel of ref pll
 * 
 * Hydra_Software_Devel/14   7/27/11 4:47p guangcai
 * SWSATFE-136: added workaround for low baud rate register access
 * 
 * Hydra_Software_Devel/13   6/16/11 9:09a enavarro
 * SWSATFE-127: added fine freq offset estimation for DVB-S2
 * 
 * Hydra_Software_Devel/12   4/29/11 11:33a enavarro
 * SWSATFE-75: bypass opll
 * 
 * Hydra_Software_Devel/11   4/25/11 10:45a enavarro
 * SWSATFE-75: fixed overflow in afec clock calculation
 * 
 * Hydra_Software_Devel/10   4/23/11 11:04a ronchan
 * SWSATFE-75: AFEC clock from SYS0_PLL_DIV
 * 
 * Hydra_Software_Devel/9   4/22/11 4:43p ronchan
 * SWSATFE-75: calculate AFEC clock from SYS1_PLL_DIV
 * 
 * Hydra_Software_Devel/8   12/8/10 2:14p enavarro
 * SWSATFE-75: added turbo sync detect interrupt
 * 
 * Hydra_Software_Devel/7   10/14/10 5:37p ronchan
 * SWSATFE-75: renamed xport config struct to xportSettings
 * 
 * Hydra_Software_Devel/6   9/30/10 2:02p enavarro
 * SWSATFE-75: store transport settings in BAST_OutputTransportSettings
 * struct in the channel handle; removed xportCtl channel handle member
 * 
 * Hydra_Software_Devel/5   9/29/10 3:52p enavarro
 * SWSATFE-80: renamed bit definitions for xportCtl
 * 
 * Hydra_Software_Devel/4   9/22/10 5:18p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/3   9/17/10 5:20p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/2   9/14/10 2:22p enavarro
 * SWSATFE-75: configure BCM7358 BAST_g3_P_InitConfig() for 1 channel only
 * 
 * Hydra_Software_Devel/1   8/31/10 2:15p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3.h"
#include "bast_g3_priv.h"
#include "bast_7358_priv.h"


BDBG_MODULE(bast_7358_priv);


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
      /* for 7358: 
         AFEC_0 = xtal * CLKGEN_PLL_SYS0_PLL_DIV [9:0] / (2 * CLKGEN_PLL_SYS0_PLL_DIV [12:10] * AFEC_GLOBAL_CLK_CNTRL[7:0]) 
      */
      pll_div = BREG_Read32(hDev->hRegister, BCHP_CLKGEN_PLL_SYS0_PLL_DIV );
      ctrl = BREG_Read32(hDev->hRegister, BCHP_AFEC_GLOBAL_CLK_CNTRL);     
      ndiv = pll_div & 0x3FF;
      pdiv = (pll_div >> 10) & 0x7;
      mdiv = ctrl & 0xFF;
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
