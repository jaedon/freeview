/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4528_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 9/7/12 9:01a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/4528/bast_4528_priv.c $
 * 
 * Hydra_Software_Devel/21   9/7/12 9:01a enavarro
 * SWSATFE-214: restructured magnum directories
 * 
 * Hydra_Software_Devel/20   4/20/12 3:17p ronchan
 * SWSATFE-75: add placeholder for almost empty interrupt on other
 * channels
 * 
 * Hydra_Software_Devel/19   4/20/12 3:13p ronchan
 * SWSATFE-75: add diseqc almost empty interrupt
 * 
 * Hydra_Software_Devel/18   2/22/12 10:17a enavarro
 * SWSATFE-86: added support for BAST_LOG_REG_ACCESS build option
 * 
 * Hydra_Software_Devel/17   1/17/12 11:01a enavarro
 * SWSATFE-86: disable Fs smart tune
 * 
 * Hydra_Software_Devel/16   11/7/11 4:19p ronchan
 * SWSATFE-140: identify controlling channel of ref pll, although
 * irrelevant for 4528
 * 
 * Hydra_Software_Devel/15   7/15/11 10:01a enavarro
 * SWSATFE-86: dont change sampleFreq when PI was already openned in
 * BAST_g3_P_InitConfig()
 * 
 * Hydra_Software_Devel/14   7/13/11 2:25p enavarro
 * SWSATFE-86: added BAST_4528_SetSampleFreq()
 * 
 * Hydra_Software_Devel/13   6/16/11 11:33a enavarro
 * SWSATFE-127: added HP frame boundary interrupts
 * 
 * Hydra_Software_Devel/12   5/26/11 8:40a enavarro
 * SWSATFE-86: removed debug messages
 * 
 * Hydra_Software_Devel/11   5/25/11 6:15p enavarro
 * SWSATFE-86: fixed afec clock calculation
 * 
 * Hydra_Software_Devel/10   5/13/11 4:22p ronchan
 * SWSATFE-86: added function to calculate adc clock
 * 
 * Hydra_Software_Devel/9   4/13/11 3:20p enavarro
 * SWSATFE-86: added test code for emulator
 * 
 * Hydra_Software_Devel/8   4/1/11 4:28p enavarro
 * SWSATFE-86: add a divide by 2 in afec_clk calculation
 * 
 * Hydra_Software_Devel/7   4/1/11 9:49a enavarro
 * SWSATFE-86: fixed compiler error
 * 
 * Hydra_Software_Devel/6   4/1/11 9:06a enavarro
 * SWSATFE-86: implemented BAST_g3_P_GetAfecClock() and
 * BAST_g3_P_GetChanAddress()
 * 
 * Hydra_Software_Devel/5   3/21/11 7:00a enavarro
 * SWSATFE-86: updated interrupt IDs
 * 
 * Hydra_Software_Devel/4   3/7/11 9:36a enavarro
 * SWSATFE-86: ongoing development
 * 
 * Hydra_Software_Devel/3   3/4/11 3:43p enavarro
 * SWSATFE-86: ongoing development
 * 
 * Hydra_Software_Devel/2   3/4/11 8:14a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/1   3/2/11 11:33a enavarro
 * SWSATFE-86: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3.h"
#include "bast_g3_priv.h"
#include "bast_4528_priv.h"
#include "bchp_tm.h"
#include "bchp_chan.h"


#if (BCHP_CHIP != 4528)
#error "This file is for BCM4528 firmware only (not for host software)"
#endif

BDBG_MODULE(bast_4528_priv);


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
      0, /* TURBO_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_NOT_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_SYNC: no TFEC in 4528 */
      BCHP_INT_ID_AFEC_LOCK_IS_0,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_0,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_0
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
      0, /* TURBO_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_NOT_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_SYNC: no TFEC in 4528 */
      BCHP_INT_ID_AFEC_LOCK_IS_1,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_1,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_1
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
      0, /* TURBO_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_NOT_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_SYNC: no TFEC in 4528 */
      BCHP_INT_ID_AFEC_LOCK_IS_2,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_2,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_2
   },
   /* channel 3 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_3,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_3,
      BCHP_INT_ID_SDS_BTM_IS_3,
      BCHP_INT_ID_SDS_BRTM_IS_3,
      BCHP_INT_ID_SDS_GENTM_IS1_3,
      BCHP_INT_ID_SDS_GENTM_IS2_3,
      BCHP_INT_ID_SDS_GENTM_IS3_3,
      0,
      0, 
      0,
      0,
      0,
      0,
      BCHP_INT_ID_SDS_HP_IS_3,
      BCHP_INT_ID_MI2C_IS_3,
      0, /* TURBO_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_NOT_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_SYNC: no TFEC in 4528 */
      BCHP_INT_ID_AFEC_LOCK_IS_3,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_3,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_3
   },
   /* channel 4 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_4,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_4,
      BCHP_INT_ID_SDS_BTM_IS_4,
      BCHP_INT_ID_SDS_BRTM_IS_4,
      BCHP_INT_ID_SDS_GENTM_IS1_4,
      BCHP_INT_ID_SDS_GENTM_IS2_4,
      BCHP_INT_ID_SDS_GENTM_IS3_4,
      0,
      0, 
      0,
      0,
      0,
      0,
      BCHP_INT_ID_SDS_HP_IS_4,
      BCHP_INT_ID_MI2C_IS_4,
      0, /* TURBO_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_NOT_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_SYNC: no TFEC in 4528 */
      BCHP_INT_ID_AFEC_LOCK_IS_4,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_4,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_4
   },
   /* channel 5 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_5,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_5,
      BCHP_INT_ID_SDS_BTM_IS_5,
      BCHP_INT_ID_SDS_BRTM_IS_5,
      BCHP_INT_ID_SDS_GENTM_IS1_5,
      BCHP_INT_ID_SDS_GENTM_IS2_5,
      BCHP_INT_ID_SDS_GENTM_IS3_5,
      0,
      0, 
      0,
      0,
      0,
      0,
      BCHP_INT_ID_SDS_HP_IS_5,
      BCHP_INT_ID_MI2C_IS_5,
      0, /* TURBO_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_NOT_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_SYNC: no TFEC in 4528 */
      BCHP_INT_ID_AFEC_LOCK_IS_5,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_5,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_5
   },
   /* channel 6 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_6,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_6,
      BCHP_INT_ID_SDS_BTM_IS_6,
      BCHP_INT_ID_SDS_BRTM_IS_6,
      BCHP_INT_ID_SDS_GENTM_IS1_6,
      BCHP_INT_ID_SDS_GENTM_IS2_6,
      BCHP_INT_ID_SDS_GENTM_IS3_6,
      0,
      0, 
      0,
      0,
      0,
      0,
      BCHP_INT_ID_SDS_HP_IS_6,
      BCHP_INT_ID_MI2C_IS_6,
      0, /* TURBO_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_NOT_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_SYNC: no TFEC in 4528 */
      BCHP_INT_ID_AFEC_LOCK_IS_6,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_6,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_6
   },
   /* channel 7 interrupts */
   {
      BCHP_INT_ID_SDS_LOCK_IS_7,
      BCHP_INT_ID_SDS_NOT_LOCK_IS_7,
      BCHP_INT_ID_SDS_BTM_IS_7,
      BCHP_INT_ID_SDS_BRTM_IS_7,
      BCHP_INT_ID_SDS_GENTM_IS1_7,
      BCHP_INT_ID_SDS_GENTM_IS2_7,
      BCHP_INT_ID_SDS_GENTM_IS3_7,
      0,
      0, 
      0,
      0,
      0,
      0,
      BCHP_INT_ID_SDS_HP_IS_7,
      BCHP_INT_ID_MI2C_IS_7,
      0, /* TURBO_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_NOT_LOCK_IS: no TFEC in 4528 */
      0, /* TURBO_SYNC: no TFEC in 4528 */
      BCHP_INT_ID_AFEC_LOCK_IS_7,
      BCHP_INT_ID_AFEC_NOT_LOCK_IS_7,
      BCHP_INT_ID_HP_FRAME_BOUNDARY_7
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
      hChn->bHasTunerRefPll = false;
      hChn->bHasAfec = true;
      hChn->bHasTfec = false;
      hChn->tunerRefPllChannel = 0;
      
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

      hChn->miscCtl = BAST_G3_CONFIG_MISC_CTL_DISABLE_SMART_TUNE;
      hChn->acqParams.mode = BAST_Mode_eDvb_scan;
      hChn->tunerFreq = 950000000UL;

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
      if (((reg >= 0x000B0000) && (reg < 0x000B0D00)) || ((reg >= 0x000B4000) && (reg < 0x000B5000)))
      {
         /* SDS or AFEC register access */
         *pAddr += (uint32_t)((uint32_t)h->channel * 0x8000);
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
#ifdef BAST_LOG_REG_ACCESS   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;   
#endif
   uint32_t addr;

   BAST_g3_P_GetRegisterAddress(h, reg, &addr);

   BREG_Write32(0, addr, *val);

#ifdef BAST_LOG_REG_ACCESS
   if ((hChn->acqState != BAST_AcqState_eMonitorLock) && (hChn->acqState != BAST_AcqState_eIdle))
      BKNI_Printf("w %08X %08X\n", addr, *val);
#endif   

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetAfecClock() - updates fecFreq
******************************************************************************/
BERR_Code BAST_g3_P_GetAfecClock(BAST_ChannelHandle h, uint32_t *pFreq)
{
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl; 
   uint32_t afec_div, P_hi, P_lo, Q_hi, Pdiv, Ndiv_int, Ndiv_frac, Fclk_sys_pll_vco;
   
   if (BAST_g3_P_IsLdpcOn(h))
   {
      /*
         Fclk_sys_pll_vco = Fclk_xtal * (1 / Pdiv) * (Ndiv_int + (Ndiv_frac / 2^20))
         where Fclk_xtal = 54MHz, Pdiv = 2 (default), 
               Ndiv_int = 80 (default), and 
               Ndiv_frac = 0 (default)
         So the default Fclk_sys_pll_vco freq is:
         Fclk_sys_pll_vco = 54 MHz * (1 / 2) * (80 + (0 / 2^20)) = 2160MHz
         AFEC clock is (Fclk_sys_pll_vco / afec_div) / 2
         Default AFEC clock is (432 MHz / 2) = 216 MHz
      */
      Pdiv = BREG_Read32(0, BCHP_TM_SYS_PLL_PDIV) & BCHP_TM_SYS_PLL_PDIV_DIV_MASK;
      Ndiv_int = BREG_Read32(0, BCHP_TM_SYS_PLL_NDIV_INT) & BCHP_TM_SYS_PLL_NDIV_INT_DIV_MASK;
      Ndiv_frac = BREG_Read32(0, BCHP_TM_SYS_PLL_NDIV_FRAC) & BCHP_TM_SYS_PLL_NDIV_FRAC_DIV_MASK;      
      afec_div = BREG_Read32(0, BCHP_TM_SYS_PLL_CLK_AFEC) & BCHP_TM_SYS_PLL_CLK_AFEC_DIV_MASK;

      /* deal with fractional Ndiv later since it complicates the calculation */
      if (Ndiv_frac != 0)
      {
         BDBG_ERR(("TM_SYS_PLL_NDIV_FRAC is non-zero!"));
         BDBG_ASSERT(0);
      }

      BAST_MultU32U32(hDev->xtalFreq, Ndiv_int, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, Pdiv, &Q_hi, &Fclk_sys_pll_vco);
      /* BDBG_MSG(("Fclk_sys_pll_vco=%u", Fclk_sys_pll_vco)); */
      
      *pFreq = (Fclk_sys_pll_vco / afec_div) >> 1;      
      /* BDBG_MSG(("AFEC clock=%u", *pFreq)); */
      return BERR_SUCCESS;
   }
   else
   {
      BDBG_ERR(("BAST_g3_P_GetAfecClock(): AFEC not powered"));
      *pFreq = 0;
      return BERR_NOT_INITIALIZED;
   }   
}


/******************************************************************************
 BAST_g3_P_GetAdcClock()
******************************************************************************/
BERR_Code BAST_g3_P_GetAdcClock(BAST_Handle h, uint32_t *clk)
{
   BAST_g3_P_Handle *hDev = h->pImpl;
   
   uint32_t pdiv;
   uint32_t ndiv_int;
   uint32_t ndiv_frac;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
   
   pdiv = BREG_Read32(0, BCHP_TM_ANA_PLL_PDIV) & 0xF;
   ndiv_int = BREG_Read32(0, BCHP_TM_ANA_PLL_NDIV_INT) & 0xFF;
   ndiv_frac = BREG_Read32(0, BCHP_TM_ANA_PLL_NDIV_FRAC) & 0xFFFFF;
   
   /* Fclk_rx_pll_vco = 54 MHz * (2 / Pdiv) * (Ndiv_int + (Ndiv_frac / 2^20)) */
   /* equivalent to (54 MHz * 2) * (2^20 * Ndiv_int + Ndiv_frac) / (2^20 * Pdiv) */
   
   BAST_MultU32U32(hDev->xtalFreq << 1, (ndiv_int << 20) + ndiv_frac, &P_hi, &P_lo);  /* (54 MHz * 2) * (2^20 * Ndiv_int + Ndiv_frac) */
   BAST_DivU64U32(P_hi, P_lo, pdiv << 20, &Q_hi, &Q_lo);  /* div by (2^20 * Pdiv) */
   
   BDBG_MSG(("Fs_adc=%d", Q_lo));
   *clk = Q_lo;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetChanAddress()
******************************************************************************/
BERR_Code BAST_g3_P_GetChanAddress(BAST_ChannelHandle h, uint32_t reg, uint32_t *pAddr)
{
   BERR_Code retCode = BERR_SUCCESS;
   
   switch (reg)
   {
      case BCHP_CHAN_DEC_FCW_CH0:
      case BCHP_CHAN_AGC_CTRL_CH0:   
      case BCHP_CHAN_AGC_CH0_LA_INT:   
      case BCHP_CHAN_AGC_CH0_LF_INT:      
         *pAddr = reg + (h->channel * 4);
         break;

      case BCHP_CHAN_ACI_CH0_H0:
         *pAddr = reg + (h->channel * 0x104);
         break;

      default:
         BDBG_ERR(("BAST_g3_P_GetChanAddress(): invalid register 0x%X", reg));
         *pAddr = reg;
         retCode = BERR_INVALID_PARAMETER;
   }
   return retCode;
}


/******************************************************************************
 BAST_4528_P_SetSampleFreq()
******************************************************************************/
BERR_Code BAST_4528_SetSampleFreq(BAST_Handle hAst, uint32_t Fs)
{   
   BAST_ChannelHandle h;
   BAST_g3_P_ChannelHandle *hChn;
   uint32_t i;

   for (i = 0; i < BAST_G3_MAX_CHANNELS; i++)
   {
      h = hAst->pChannels[i];
      BAST_AbortAcq(h);
      
      hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
      hChn->sampleFreq = Fs;  
   }

   return BERR_SUCCESS;
}

