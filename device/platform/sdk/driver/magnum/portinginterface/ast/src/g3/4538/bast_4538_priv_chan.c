/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4538_priv_chan.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 9/26/12 1:51p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/4538/bast_4538_priv_chan.c $
 * 
 * Hydra_Software_Devel/9   9/26/12 1:51p enavarro
 * SWSATFE-174: turn on all adc fifos in BAST_g3_P_TunerInit()
 * 
 * Hydra_Software_Devel/8   9/13/12 5:58p enavarro
 * SWSATFE-174: enable rx chan fifo
 * 
 * Hydra_Software_Devel/7   9/7/12 10:10a ronchan
 * SWSATFE-214: renamed debug module identifier
 * 
 * Hydra_Software_Devel/6   8/30/12 9:07a enavarro
 * SWSATFE-174: moved BAST_g3_P_GetChanAciCoeff() and
 * BAST_g3_P_SetChanAciCoeff() to bast_4538_aci.c
 * 
 * Hydra_Software_Devel/5   8/1/12 10:30a enavarro
 * SWSATFE-174: added BAST_SetAdcSelect() and BAST_GetAdcSelect() to the
 * API for chips with multiple wideband tuners
 * 
 * Hydra_Software_Devel/4   7/10/12 2:03p enavarro
 * SWSATFE-174: moved BAST_g3_P_SelectAdc to bast_4538.c
 * 
 * Hydra_Software_Devel/3   6/28/12 3:17p enavarro
 * SWSATFE-174: added BAST_g3_P_SelectAdc(); fix compiler warnings
 * 
 * Hydra_Software_Devel/2   4/24/12 3:46p enavarro
 * SWSATFE-174: RDB changes
 * 
 * Hydra_Software_Devel/1   4/24/12 3:23p enavarro
 * SWSATFE-174: initial check-in
 *
 ***************************************************************************/

#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"

#if (BCHP_CHIP != 4538)
#error "This file is for BCM4538 firmware only (not for host software)"
#endif


/* this file is used only for chips that have wideband tuner */
#ifdef BAST_HAS_WFE

#include "bchp_stb_chan_ctrl.h"
#include "bchp_stb_chan_ch0.h"


BDBG_MODULE(bast_4538_priv_chan);


/******************************************************************************
 BAST_g3_P_SetChanDecFcw() - sets CHAN_DEC_FCW_CHx based on Fs and Fc
******************************************************************************/
BERR_Code BAST_g3_P_SetChanDecFcw(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;  
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
  
   /* fcw = (2^32)*Fc/Fadc */
   BAST_MultU32U32(hChn->actualTunerFreq/1000, 2147483648UL, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, BAST_ADC_FREQ_KHZ>>1, &Q_hi, &Q_lo); /* Fadc/2==2484MHz */
   BAST_g3_P_WriteRegister(h, BCHP_STB_CHAN_CHx_DEC_FCW, &Q_lo);
   /* BDBG_MSG(("BCHP_CHAN_DEC_FCW_CHx: Fc=%u, val=0x%X", hChn->actualTunerFreq, Q_lo)); */
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_ConfigChanAgc()
******************************************************************************/
BERR_Code BAST_g3_P_ConfigChanAgc(BAST_ChannelHandle h, bool bTracking)
{
   uint32_t val, agc_thresh;
   
   /* agc_chx_byp = 0
      agc_chx_la_byp = 0
      agc_chx_la_frz = 0
      agc_chx_lf_frz = 0
      agc_chx_lf_k1 = 3    
    */     
   val = (3 << BCHP_STB_CHAN_CH0_AGC_CTRL_AGC_LF_K1_SHIFT) & BCHP_STB_CHAN_CH0_AGC_CTRL_AGC_LF_K1_MASK;
   if (bTracking)
      agc_thresh = 7366246;
   else 
      agc_thresh = 1074790;
   val |= agc_thresh;
   BAST_g3_P_WriteRegister(h, BCHP_STB_CHAN_CHx_AGC_CTRL, &val);    
   
   val = 0;
   BAST_g3_P_WriteRegister(h, BCHP_STB_CHAN_CHx_AGC_LF_INT, &val);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerConfigLna()
******************************************************************************/
BERR_Code BAST_g3_P_TunerConfigLna(BAST_Handle h, BAST_TunerLnaSettings *pSettings)
{
   BSTD_UNUSED(h);
   BSTD_UNUSED(pSettings);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_g3_P_GetTunerLnaStatus()
******************************************************************************/
BERR_Code BAST_g3_P_TunerGetLnaStatus(BAST_ChannelHandle h, BAST_TunerLnaStatus *pStatus)
{
   BSTD_UNUSED(h);
   BSTD_UNUSED(pStatus);
   return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_g3_P_TunerPowerUp()
******************************************************************************/
BERR_Code BAST_g3_P_TunerPowerUp(BAST_ChannelHandle h)
{
   uint32_t val;
   uint32_t mask = (1 << h->channel);

   BAST_g3_P_ReadRegister(h, BCHP_STB_CHAN_CTRL_PWRDN, &val);
   val &= ~mask;
   BAST_g3_P_WriteRegister(h, BCHP_STB_CHAN_CTRL_PWRDN, &val);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerPowerDown()
******************************************************************************/
BERR_Code BAST_g3_P_TunerPowerDown(BAST_ChannelHandle h)
{
   uint32_t val;
   uint32_t mask = (1 << h->channel);

   BAST_g3_P_ReadRegister(h, BCHP_STB_CHAN_CTRL_PWRDN, &val);
   val |= mask;
   BAST_g3_P_WriteRegister(h, BCHP_STB_CHAN_CTRL_PWRDN, &val);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerGetLockStatus()
******************************************************************************/
BERR_Code BAST_g3_P_TunerGetLockStatus(BAST_ChannelHandle h, bool *bRefPllLocked, bool *bMixPllLocked)
{
   BSTD_UNUSED(h);

   /* TBD */
   *bRefPllLocked = true;
   *bMixPllLocked = true;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerSetFilter()
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetFilter(BAST_ChannelHandle h, bool bTracking)
{
   BSTD_UNUSED(h);
   BSTD_UNUSED(bTracking);
   /* TBD */
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerUpdateActualTunerFreq()
******************************************************************************/
BERR_Code BAST_g3_P_TunerUpdateActualTunerFreq(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   hChn->actualTunerFreq = hChn->tunerFreq + hChn->tunerIfStep;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerSetFreq()
******************************************************************************/
BERR_Code BAST_g3_P_TunerSetFreq(BAST_ChannelHandle h, BAST_g3_FUNCT nextFunct)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   int16_t *pCoeff;

   hChn->postTuneFunct = nextFunct;
   
   /* set acq state to tuning */
   if (hChn->acqState == BAST_AcqState_eIdle)
      hChn->acqState = BAST_AcqState_eTuning;

   BAST_g3_P_TunerUpdateActualTunerFreq(h);  
   BAST_g3_P_SetChanDecFcw(h);   
   BAST_g3_P_GetChanAciCoeff(h, &pCoeff);   
   BAST_g3_P_SetChanAciCoeff(h, pCoeff);   
   BAST_g3_P_ConfigChanAgc(h, false); /* config agc for acquisition */ 
   return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1000, hChn->postTuneFunct);
}


/******************************************************************************
 BAST_g3_P_TunerInit()
******************************************************************************/
BERR_Code BAST_g3_P_TunerInit(BAST_ChannelHandle h, BAST_g3_FUNCT nextFunct)
{
   uint32_t mask = (1 << h->channel);

   BAST_g3_P_ToggleBit(h, BCHP_STB_CHAN_CTRL_LOCAL_SW_RESET, mask);     
   BAST_g3_P_OrRegister(h, BCHP_STB_CHAN_CTRL_XBAR_CTRL, 0x0F); /* turn on all adc fifos */
   
   if (nextFunct)
      return nextFunct(h);
   else
      return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TunerQuickTune()
******************************************************************************/
BERR_Code BAST_g3_P_TunerQuickTune(BAST_ChannelHandle h, BAST_g3_FUNCT nextFunct)
{
   BAST_g3_P_TunerUpdateActualTunerFreq(h);
   BAST_g3_P_SetChanDecFcw(h);
   return nextFunct(h);
}


/******************************************************************************
 BAST_g3_P_SetAdcSelect() - configures the crossbar to select the ADC for the channel
******************************************************************************/
BERR_Code BAST_g3_P_SetAdcSelect(BAST_ChannelHandle h, BAST_AdcSelect adcSelect)
{
   BERR_Code retCode;
   uint32_t xbar, mask;
   
   if (adcSelect > BAST_AdcSelect_e3)
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      return retCode;
   }

   BAST_g3_P_ReadRegister(h, BCHP_STB_CHAN_CTRL_XBAR_SEL, &xbar);
   mask = ~(0x3 << (h->channel * 2));
   xbar &= mask;
   xbar |= (((uint32_t)adcSelect & 0x03) << (h->channel * 2));
   BAST_g3_P_WriteRegister(h, BCHP_STB_CHAN_CTRL_XBAR_SEL, &xbar);   

   /* we dont need to turn on the adc fifo because it has already been turned on in TunerInit
   BAST_g3_P_OrRegister(h, BCHP_STB_CHAN_CTRL_XBAR_CTRL, 1 << adcSelect); */
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetAdcSelect() - returns the crossbar configuration for the channel
******************************************************************************/
BERR_Code BAST_g3_P_GetAdcSelect(BAST_ChannelHandle h, BAST_AdcSelect *pAdcSelect)
{
   uint32_t xbar;

   BAST_g3_P_ReadRegister(h, BCHP_STB_CHAN_CTRL_XBAR_SEL, &xbar);
   *pAdcSelect = (BAST_AdcSelect)(xbar >> (h->channel * 2)) & 0x03;
   return BERR_SUCCESS;   
}

#endif /* BAST_HAS_WFE */
