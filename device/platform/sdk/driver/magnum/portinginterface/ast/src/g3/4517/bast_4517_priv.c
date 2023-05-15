/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4517_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 9/7/12 8:20a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/4517/bast_4517_priv.c $
 * 
 * Hydra_Software_Devel/10   9/7/12 8:20a enavarro
 * SWSATFE-214: restructure AST directory
 * 
 * Hydra_Software_Devel/9   8/6/12 4:42p enavarro
 * SWSATFE-140: set tuner_ctl=0x1 for channel 2
 * 
 * Hydra_Software_Devel/8   4/20/12 3:15p ronchan
 * SWSATFE-75: add diseqc almost empty interrupt
 * 
 * Hydra_Software_Devel/7   1/27/12 2:25p enavarro
 * SWSATFE-140: dont do retries in BAST_g3_P_WriteRegister()
 * 
 * Hydra_Software_Devel/6   1/13/12 3:31p enavarro
 * SWSATFE-140: disable smart tune by default
 * 
 * Hydra_Software_Devel/5   11/30/11 3:19p enavarro
 * SWSATFE-140: fixed compiler warning
 * 
 * Hydra_Software_Devel/4   11/30/11 2:47p enavarro
 * SWSATFE-157: use common function, BAST_g3_P_GetRegisterWriteWaitTime(),
 * to determine if register is in baud clock domain
 * 
 * Hydra_Software_Devel/3   11/10/11 9:43a enavarro
 * SWSATFE-157: fixed eq register access for low baud rates
 * 
 * Hydra_Software_Devel/2   11/7/11 3:09p ronchan
 * SWSATFE-140: identify controller of shared ref pll
 * 
 * Hydra_Software_Devel/1   10/17/11 3:17p enavarro
 * SWSATFE-140: initial check-in
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3.h"
#include "bast_g3_priv.h"
#include "bast_4517_priv.h"


BDBG_MODULE(bast_4517_priv);


/* local functions */
void BAST_g3_P_GetRegisterAddress(BAST_ChannelHandle h, uint32_t reg, uint32_t *pAddr);

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
      BCHP_INT_ID_DISEQC_TIMER1,
      BCHP_INT_ID_DISEQC_TIMER2, 
      BCHP_INT_ID_SDS_sar_vol_gt_hi_thrsh,
      BCHP_INT_ID_SDS_sar_vol_lt_lo_thrsh,
      BCHP_INT_ID_DSDN_IS,
      BCHP_INT_ID_DISEQC_tx_fifo_a_empty_0,
      BCHP_INT_ID_SDS_HP_IS_0,
      BCHP_INT_ID_MI2C_IS_0,
      BCHP_INT_ID_TURBO_LOCK_IS_0,
      BCHP_INT_ID_TURBO_NOT_LOCK_IS_0,
      BCHP_INT_ID_TURBO_SYNC_0,       
      0,
      0,
      0
   },
   /* channel 1 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_1,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_1,
      BCHP_INT_ID_SDS_BTM_IS_1,
      BCHP_INT_ID_SDS_BRTM_IS_1,
      BCHP_INT_ID_SDS_GENTM_IS1_1,
      BCHP_INT_ID_SDS_GENTM_IS2_1,
      BCHP_INT_ID_SDS_GENTM_IS3_1,
      0,
      0, 
      0,
      0,
      0,
      0,
      BCHP_INT_ID_SDS_HP_IS_1,
      BCHP_INT_ID_MI2C_IS_1,
      BCHP_INT_ID_TURBO_LOCK_IS_1,
      BCHP_INT_ID_TURBO_NOT_LOCK_IS_1,
      BCHP_INT_ID_TURBO_SYNC_1,       
      0,
      0,
      0
   },
   /* channel 2 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_2,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_2,
      BCHP_INT_ID_SDS_BTM_IS_2,
      BCHP_INT_ID_SDS_BRTM_IS_2,
      BCHP_INT_ID_SDS_GENTM_IS1_2,
      BCHP_INT_ID_SDS_GENTM_IS2_2,
      BCHP_INT_ID_SDS_GENTM_IS3_2,
      0,
      0, 
      0,
      0,
      0,
      0,
      BCHP_INT_ID_SDS_HP_IS_2,
      BCHP_INT_ID_MI2C_IS_2,
      BCHP_INT_ID_TURBO_LOCK_IS_2,
      BCHP_INT_ID_TURBO_NOT_LOCK_IS_2,
      BCHP_INT_ID_TURBO_SYNC_2,       
      0,
      0,
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
   BAST_g3_P_Handle *hDev = (BAST_g3_P_Handle *)h->pDevice->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
  
   if (h->channel >= BAST_G3_MAX_CHANNELS)
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      BDBG_ASSERT(0);
   }
   else
   {
      hChn->bHasDiseqc = (h->channel == 0) ? true : false;
      hChn->bExternalTuner = false;
      hChn->bHasTunerRefPll = (h->channel % 2) ? true : false;
      if (h->channel == 1)
         hChn->tunerRefPllChannel = 0;
      else
         hChn->tunerRefPllChannel = h->channel;
      hChn->bHasAfec = false;
      hChn->bHasTfec = true;

      if (h->channel == 2)
         hChn->tunerCtl = BAST_G3_CONFIG_TUNER_CTL_LNA_PGA_MODE;
      
      hChn->xportSettings.bSerial = true;
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
      hChn->xportSettings.bOpllBypass = false;
      hChn->xportSettings.bchMpegErrorMode = BAST_BchMpegErrorMode_eBchAndCrc8;

      hChn->acqParams.mode = BAST_Mode_eDvb_scan;
      hChn->tunerFreq = 950000000UL;
      hChn->miscCtl = BAST_G3_CONFIG_MISC_CTL_DISABLE_SMART_TUNE;
      
      if (hDev->bOpen == false)
         hChn->sampleFreq = BAST_DEFAULT_SAMPLE_FREQ;      
      hChn->bFsNotDefault = false;
   }
   return retCode;   
}


/******************************************************************************
 BAST_g3_P_GetRegisterAddress()
******************************************************************************/
void BAST_g3_P_GetRegisterAddress(BAST_ChannelHandle h, uint32_t reg, uint32_t *pAddr)
{
   *pAddr = reg;
   
   if (h->channel > 0)
   {
      if ((reg >= 0xA8000) && (reg <= 0xA8CFF))
      {
         /* SDS register access */
         *pAddr += (h->channel * 0x8000);
      }
      else if ((reg >= 0xAC000) && (reg <= 0xAC3FF))
      {
         /* TFEC register access */
         *pAddr += (h->channel * 0x8000);

      }
   }
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
   uint32_t addr;
   
   BAST_g3_P_GetRegisterAddress(h, reg, &addr);
   *val = BREG_Read32(0, addr); 
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
   uint32_t addr, wait_time;   
      
   BAST_g3_P_GetRegisterWriteWaitTime(h, reg, &wait_time);
   BAST_g3_P_GetRegisterAddress(h, reg, &addr);

   BREG_Write32(0, addr, *val);    
   if (wait_time > 0)
      BKNI_Delay(wait_time);

   return BERR_SUCCESS;
}



