/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv_cwc.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 8/28/12 4:25p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/g3/bast_g3_priv_cwc.c $
 * 
 * Hydra_Software_Devel/12   8/28/12 4:25p enavarro
 * SWSATFE-174: implemented dco notch filter support for 4538
 * 
 * Hydra_Software_Devel/11   8/28/12 2:29p enavarro
 * SWSATFE-140: removed BAST_g3_P_UpdateCWC()
 * 
 * Hydra_Software_Devel/10   8/28/12 2:16p enavarro
 * SWSATFE-140: fixed cwc programming
 * 
 * Hydra_Software_Devel/9   8/28/12 9:59a enavarro
 * SWSATFE-174: started adding spur canceller code for 4538
 * 
 * Hydra_Software_Devel/8   5/10/12 9:54a ronchan
 * SW7360-13: updated cwc shift definitions
 * 
 * Hydra_Software_Devel/7   10/17/11 9:05a enavarro
 * SWSATFE-150: enable spur canceller on stable lock
 * 
 * Hydra_Software_Devel/6   10/13/11 1:59p enavarro
 * SWSATFE-150: added code to narrow bw
 * 
 * Hydra_Software_Devel/5   10/11/11 10:22a enavarro
 * SWSATFE-150: added BAST_g3_P_ResetCWC()
 * 
 * Hydra_Software_Devel/4   10/10/11 11:01a enavarro
 * SWSATFE-150: minor changes to cwc programming
 * 
 * Hydra_Software_Devel/3   10/7/11 4:42p enavarro
 * SWSATFE-150: implemented BAST_g3_P_InitCWC() and BAST_g3_P_UpdateCWC()
 * 
 * Hydra_Software_Devel/2   10/7/11 10:35a enavarro
 * SWSATFE-150: added BAST_g3_P_DisableSpurCanceller()
 * 
 * Hydra_Software_Devel/1   10/6/11 9:09a enavarro
 * SWSATFE-150: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"


BDBG_MODULE(bast_g3_priv_cwc);

#define BAST_DEBUG_CWC(x) /* x */


#ifndef BAST_EXCLUDE_SPUR_CANCELLER

#ifdef BAST_HAS_WFE
#define BAST_Notch_pdgain   0
#define BAST_Notch_delta    0
#define BAST_Notch_acqAlpha 0
#define BAST_Notch_acqBeta  20
#define BAST_Notch_trkAlpha 20
#define BAST_Notch_trkBeta  20
#else
#define BAST_CWC_STEP_SIZE 3
#define BAST_CWC_INIT_CINT 0x40         
#define BAST_CWC_INIT_CLIN 0x100
#define BAST_CWC_TRK_CINT  0x40
#define BAST_CWC_TRK_CLIN  0x10
#endif


/******************************************************************************
 BAST_g3_P_ClearSpurCancellerConfig() - this function clears the spur canceller
                                        configuration
******************************************************************************/
BERR_Code BAST_g3_P_ClearSpurCancellerConfig(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   int i;

   for (i = 0; i < 6; i++)
   {
      hChn->spurConfig[i].freq = 0;
      hChn->bCwcActive[i] = false;
   }
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_DisableSpurCanceller() - this function disables all 6 spur cancellers
******************************************************************************/
BERR_Code BAST_g3_P_DisableSpurCanceller(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;   
   uint32_t val32, i;
   
#ifdef BAST_HAS_WFE
   val32 = 0x30000000;   
   BAST_g3_P_WriteRegister(h, BCHP_STB_CHAN_CHx_NOTCH_0_CTRL, &val32);
   BAST_g3_P_WriteRegister(h, BCHP_STB_CHAN_CHx_NOTCH_1_CTRL, &val32);
#else
   val32 = 0x8;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CWC_CTRL1, &val32);
   
   val32 = 0;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CWC_CTRL1, &val32);   

   val32 = 0x00FC0FC0;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CWC_CTRL2, &val32);
#endif

   for (i = 0; i < 6; i++)
      hChn->bCwcActive[i] = false;
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_ResetCWC()
******************************************************************************/
BERR_Code BAST_g3_P_ResetCWC(BAST_ChannelHandle h)
{
#ifdef BAST_HAS_WFE
   BSTD_UNUSED(h);
#else   
   BAST_g3_P_OrRegister(h, BCHP_SDS_CWC_CTRL1, 0x08);  
   BAST_g3_P_AndRegister(h, BCHP_SDS_CWC_CTRL1, ~0x08);
#endif   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_NarrowCwcBw()
******************************************************************************/
BERR_Code BAST_g3_P_NarrowCwcBw(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t i;
   
#ifdef BAST_HAS_WFE
   uint32_t val32, reg;

   for (i = 0; i < 2; i++)
   {
      reg = i == 0 ? BCHP_STB_CHAN_CHx_NOTCH_0_CTRL : BCHP_STB_CHAN_CHx_NOTCH_1_CTRL;
      BAST_g3_P_ReadRegister(h, reg, &val32);
      if ((val32 & 0x10000000) == 0)
      {
         /* this notch is enabled */
         val32 &= ~0x00001F1F;
         val32 |= (BAST_Notch_trkAlpha & 0x1F);
         val32 |= ((BAST_Notch_trkBeta & 0x1F) << 8);
         BAST_g3_P_WriteRegister(h, reg, &val32);
      }
   }
#else   
   uint32_t cint, clin, lfc;
   
   hChn->funct_state++;
   cint = BAST_CWC_INIT_CINT - (hChn->funct_state * (BAST_CWC_INIT_CINT-BAST_CWC_TRK_CINT)>>2);
   clin = BAST_CWC_INIT_CLIN - (hChn->funct_state * (BAST_CWC_INIT_CLIN-BAST_CWC_TRK_CLIN)>>2);
   lfc = cint << BCHP_SDS_CWC_LFC1_LF_CINT_SHIFT;
   lfc |= (clin << BCHP_SDS_CWC_LFC1_LF_CLIN_SHIFT);

   for (i = 0; i < 6; i++)
   {
      if (hChn->bCwcActive[i])
      {
         BAST_g3_P_WriteRegister(h, BCHP_SDS_CWC_LFC1 + (i<<2), &lfc);                       
      }
   }

   if (hChn->funct_state < 4)      
      return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1000, BAST_g3_P_NarrowCwcBw);      
#endif

   return hChn->passFunct(h);
}


/******************************************************************************
 BAST_g3_P_InitCWC()
******************************************************************************/
BERR_Code BAST_g3_P_InitCWC(BAST_ChannelHandle h, BAST_g3_FUNCT funct)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t range, Fc, Fspur, i, Q_hi, Q_lo, P_hi, P_lo, val32;
   int32_t diff;
   bool bIsCwcActive = false;   
#ifdef BAST_HAS_WFE
   uint32_t min_freq, max_freq, n;
#else
   uint32_t i_cw, stepSize, ctrl1, ctrl2, fcw, lfc;
#endif

   hChn->passFunct = funct;
   BAST_g3_P_DisableSpurCanceller(h);

#ifdef BAST_HAS_WFE
   /* calculate the range for enabling DCO notch filters, i.e. (Fc-range) to (Fc+range)
      where range = Fb*(1+NYQUIST_ALPHA)/2
      for now, assume NYQUIST_ALPHA=20%, therefore range=0.6*Fb */
   Fc = hChn->actualTunerFreq;
   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_NYQUIST_20)     
      range = (hChn->acqParams.symbolRate * 3) / 5; /* range=0.6*Fb */
   else
      range = (hChn->acqParams.symbolRate * 27) / 40; /* range=0.675*Fb */
   min_freq = Fc - range;
   max_freq = Fc + range;

   for (i = n = 0; (n < 2) && (i < 6); i++)
   {
      Fspur = hChn->spurConfig[i].freq;
      if (Fspur == 0)
         break;
      
      if ((Fspur >= min_freq) && (Fspur <= max_freq))
      {
         bIsCwcActive = true;
         diff = Fspur - Fc;
         if (diff < 0)
            val32 = (uint32_t)(-diff);
         else
            val32 = (uint32_t)diff;
         BAST_DEBUG_CWC(BDBG_MSG(("enabling notch for spur at %u Hz, Fc=%u, notch_fcw_freq=%d", Fspur, Fc, diff)));         

         /* calculate FCW = (2^31)*Fc/(Fadc/24) */
         BAST_MultU32U32(val32 * 12, 2147483648UL, &P_hi, &P_lo);  /* 12*Fc*(2^31) */
         BAST_DivU64U32(P_hi, P_lo, (hChn->sampleFreq >> 1), &Q_hi, &Q_lo);  /* div by (Fs/2) */
         if (diff < 0)
            Q_lo = -Q_lo;
         BAST_g3_P_WriteRegister(h, n == 0 ? BCHP_STB_CHAN_CHx_NOTCH_0_FCW : BCHP_STB_CHAN_CHx_NOTCH_1_FCW, (uint32_t*)&Q_lo);      

         BAST_g3_P_ReadModifyWriteRegister(h, (n == 0) ? BCHP_STB_CHAN_CHx_NOTCH_0_CTRL : BCHP_STB_CHAN_CHx_NOTCH_1_CTRL, 
                                           ~0x307F1F1F, /* unfreeze, unbypass dco and notch, clear parameters */
                                           ((BAST_Notch_pdgain & 0x7) << 20) | ((BAST_Notch_delta & 0xF) << 16) | ((BAST_Notch_acqBeta & 0x1F) << 8) | (BAST_Notch_acqAlpha & 0x1F));

         n++;
      }      
   }
#else   
   range = hChn->acqParams.symbolRate << 1;
   BAST_g3_P_TunerGetActualLOFreq(h, &Fc);
   
   for (i = 0, i_cw = 0; (i_cw < 6) && (i < 6); i++)
   {
      Fspur = hChn->spurConfig[i].freq;      
      if (Fspur == 0)
         continue;
      diff = Fspur - Fc;
      if ((diff >= (int32_t)range) || (diff <= (int32_t)-range))
         continue;

      BAST_DEBUG_CWC(BDBG_MSG(("enabling CWC for spur at %u Hz, Fc=%u, initFreqOffset=%d, diff=%d", Fspur, Fc, hChn->initFreqOffset, diff)));
      bIsCwcActive = true;
      
      /* set ctrl1 */
      BAST_g3_P_ReadRegister(h, BCHP_SDS_CWC_CTRL1, &ctrl1);  
      ctrl1 &= ~0x7;
      ctrl1 |= (i_cw+1);
      stepSize = BAST_CWC_STEP_SIZE;
      ctrl1 &= ~(0x0000000F << ((i_cw+1)<<2));
      ctrl1 |= (stepSize << ((i_cw+1)<<2));   

      /* set ctrl2 */
      BAST_g3_P_ReadRegister(h, BCHP_SDS_CWC_CTRL2, &ctrl2);  
      ctrl2 &= ~(1<<(i_cw+6));   /* unfreeze loop filter integrator */
      ctrl2 &= ~(1<<(i_cw+18));  /* unfreeze tap */

      /* set freq control word */
      if (diff >= 0)
         val32 = (uint32_t)diff;
      else
         val32 = (uint32_t)-diff;
      BAST_MultU32U32(val32, 268435456, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &Q_lo);
      if (diff >= 0)
         fcw = Q_lo;
      else
         fcw = ~Q_lo + 1;
      fcw &= 0x1FFFFFFF;

      /* set the phase/frequency detector and loop filter coefficients */
      lfc = BAST_CWC_INIT_CINT << BCHP_SDS_CWC_LFC1_LF_CINT_SHIFT;
      lfc |= (BAST_CWC_INIT_CLIN << BCHP_SDS_CWC_LFC1_LF_CLIN_SHIFT);

      BAST_g3_P_WriteRegister(h, BCHP_SDS_CWC_CTRL1, &ctrl1); 
      BAST_g3_P_WriteRegister(h, BCHP_SDS_CWC_CTRL2, &ctrl2);   
      BAST_g3_P_WriteRegister(h, BCHP_SDS_CWC_SPUR_FCW1 + (i_cw<<2), &fcw);  
      BAST_g3_P_WriteRegister(h, BCHP_SDS_CWC_LFC1 + (i_cw<<2), &lfc);   

      val32 = 0;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_CWC_FOFS1 + (i_cw<<2), &val32);      

      BAST_g3_P_ToggleBit(h, BCHP_SDS_CWC_CTRL1, 0x08); /* toggle reset */
      
      hChn->bCwcActive[i] = true;
      BAST_DEBUG_CWC(BDBG_MSG(("CWC(%d): Fc=%u, ctrl1=0x%08X, ctrl2=0x%08X, fcw=0x%08X, lfc=0x%08X", i_cw, Fc, ctrl1, ctrl2, fcw, lfc)));
      i_cw++;
   }
#endif

   if (bIsCwcActive)
   {
      hChn->funct_state = 0;
      return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100, BAST_g3_P_NarrowCwcBw);
   }
   
   return hChn->passFunct(h);
}

#endif

