/******************************************************************************
*    (c)2011-2013 Broadcom Corporation
*
* This program is the proprietary software of Broadcom Corporation and/or its licensors,
* and may only be used, duplicated, modified or distributed pursuant to the terms and
* conditions of a separate, written license agreement executed between you and Broadcom
* (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
* no license (express or implied), right to use, or waiver of any kind with respect to the
* Software, and Broadcom expressly reserves all rights in and to the Software and all
* intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
* HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
* NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
*
* Except as expressly set forth in the Authorized License,
*
* 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
* secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
* and to use this information only in connection with your use of Broadcom integrated circuit products.
*
* 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
* THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
* LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
* OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
* USE OR PERFORMANCE OF THE SOFTWARE.
*
* 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
* LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
* EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
* USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
* ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
* LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
* ANY LIMITED REMEDY.
*
* $brcm_Workfile: $
* $brcm_Revision: $
* $brcm_Date: $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: $
*
*****************************************************************************/
#include "bstd.h"
#include "bmth.h"
#include "bsat.h"
#include "bsat_priv.h"
#include "bsat_g1_priv.h"


BDBG_MODULE(bsat_g1_priv_hp);

#define BSAT_DEBUG_HP(x) /* x */

#define BSAT_HP_TIMEOUT 2000000 /* time in baud clocks to wait for rcvr lock */

/* local functions */
BERR_Code BSAT_g1_P_HpAcquire1_isr(BSAT_ChannelHandle h);
bool BSAT_g1_P_HpOkToEnableFineFreqEst_isr(BSAT_ChannelHandle h);

extern BSAT_g1_ModeFunct BSAT_ModeFunct[];


static const uint8_t BSAT_P_dvbs2x_plscode[] =
{
   132, /* DVB-S2X QPSK 13/45 */
   134, /* DVB-S2X QPSK 9/20 */
   136, /* DVB-S2X QPSK 11/20 */
   138, /* DVB-S2X 8APSK 5/9 */
   140, /* DVB-S2X 8APSK 26/45-L */
   142, /* DVB-S2X 8APSK 23/36 */
   144, /* DVB-S2X 8APSK 25/36 */
   146, /* DVB-S2X 8APSK 13/18 */
   148, /* DVB-S2X 16APSK 1/2-L */
   150, /* DVB-S2X 16APSK 8/15-L */
   152, /* DVB-S2X 16APSK 5/9-L */
   154, /* DVB-S2X 16APSK 26/45 */
   156, /* DVB-S2X 16APSK 3/5 */
   158, /* DVB-S2X 16APSK 3/5-L */
   160, /* DVB-S2X 16APSK 28/45 */
   162, /* DVB-S2X 16APSK 23/36 */
   164, /* DVB-S2X 16APSK 2/3-L */
   166, /* DVB-S2X 16APSK 25/36 */
   168, /* DVB-S2X 16APSK 13/18 */
   170, /* DVB-S2X 16APSK 7/9 */
   172, /* DVB-S2X 16APSK 77/90 */
   174, /* DVB-S2X 32APSK 2/3-L */
   178, /* DVB-S2X 32APSK 32/45 */
   180, /* DVB-S2X 32APSK 11/15 */
   182  /* DVB-S2X 32APSK 7/9 */
};


/******************************************************************************
 BSAT_g1_P_HpDisableInterrupts_isr()
******************************************************************************/
BERR_Code BSAT_g1_P_HpDisableInterrupts_isr(BSAT_ChannelHandle h)
{
   BERR_Code retCode;
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;

   BSAT_CHK_RETCODE(BINT_DisableCallback_isr(hChn->hHpStateMatchCb));
   BSAT_CHK_RETCODE(BINT_ClearCallback_isr(hChn->hHpStateMatchCb));
   BSAT_CHK_RETCODE(BINT_DisableCallback_isr(hChn->hHpStateChangeCb));
   BSAT_CHK_RETCODE(BINT_ClearCallback_isr(hChn->hHpStateChangeCb));
   BSAT_CHK_RETCODE(BINT_DisableCallback_isr(hChn->hHpFrameBoundaryCb));
   BSAT_CHK_RETCODE(BINT_ClearCallback_isr(hChn->hHpFrameBoundaryCb));

   done:
   return retCode;
}


/******************************************************************************
 BSAT_g1_P_HpFrameBoundary_isr() - callback routine for HP frame boundary
                                   interrupt
******************************************************************************/
void BSAT_g1_P_HpFrameBoundary_isr(void *p, int int_id)
{
   BSAT_ChannelHandle h = (BSAT_ChannelHandle)p;
   BSAT_g1_P_IncrementInterruptCounter_isr(h, int_id);
}


/******************************************************************************
 BSAT_g1_P_IsHpLocked_isr()
******************************************************************************/
bool BSAT_g1_P_IsHpLocked_isr(BSAT_ChannelHandle h)
{
   uint32_t acm_check, hpstate;

   acm_check = BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_SDS_HP_ACM_CHECK);
   hpstate = (acm_check >> 8) & 0x07;
   return (hpstate == 5) ? true : false;
}


/******************************************************************************
 BSAT_g1_P_HpStateMatch_isr() - callback routine for HP state match interrupt
******************************************************************************/
void BSAT_g1_P_HpStateMatch_isr(void *p, int int_id)
{
   BSAT_ChannelHandle h = (BSAT_ChannelHandle)p;
   BSAT_g1_P_ChannelHandle *hChn = h->pImpl;
#ifndef BSAT_EXCLUDE_AFEC
   uint32_t acm_check, modcod, val;
   uint8_t oldScanState;
#endif

   BSAT_g1_P_IncrementInterruptCounter_isr(h, int_id);

   /* verify that we're in RECEIVER_LOCK state */
   if (BSAT_g1_P_IsHpLocked_isr(h) == false)
   {
      if (BSAT_g1_P_IsTimerExpired_isr(h, BSAT_TimerSelect_eBaud) == false)
      {
         /* re-enable the HP state match irq */
         BINT_ClearCallback_isr(hChn->hHpStateMatchCb);
         BINT_EnableCallback_isr(hChn->hHpStateMatchCb);
      }
      else
      {
         hChn->reacqCause = BSAT_ReacqCause_eHpCouldNotLock;
         BSAT_ModeFunct[hChn->acqType].Reacquire(h); /* give up */
      }
      return;
   }

   BSAT_g1_P_DisableTimer_isr(h, BSAT_TimerSelect_eBaud); /* kill the reacquisition timer */
   BSAT_g1_P_HpDisableInterrupts_isr(h);

   /* HP is locked */
   BSAT_g1_P_LogTraceBuffer_isr(h, BSAT_TraceEvent_eRcvrLocked);
   BDBG_MSG(("HP%d locked", h->channel));

#ifndef BSAT_EXCLUDE_AFEC
   if (BSAT_MODE_IS_DVBS2(hChn->acqSettings.mode))
   {
      acm_check = BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_SDS_HP_ACM_CHECK);
      if (acm_check & 0x02)
         hChn->bShortFrame = true;
      else
         hChn->bShortFrame = false;

      oldScanState = hChn->dvbs2ScanState;

      if (hChn->dvbs2ScanState & BSAT_DVBS2_SCAN_STATE_ENABLED)
      {
         modcod = ((acm_check >> 2) & 0x1F);
         if (modcod == 0)
         {
            /* invalid modcod */
            hChn->bEnableFineFreqEst = false;
            goto reacquire_hp;
         }
         hChn->actualMode = BSAT_Mode_eDvbs2_Qpsk_1_4 + modcod - 1;
         hChn->dvbs2ScanState |= BSAT_DVBS2_SCAN_STATE_FOUND;
         if (acm_check & 0x01)
            hChn->dvbs2ScanState |= BSAT_DVBS2_SCAN_STATE_PILOT;
         else
            hChn->dvbs2ScanState &= ~BSAT_DVBS2_SCAN_STATE_PILOT;
         if (BSAT_MODE_IS_DVBS2_QPSK(hChn->actualMode))
            hChn->dvbs2ScanState |= BSAT_DVBS2_SCAN_STATE_QPSK;
         else
            hChn->dvbs2ScanState &= ~BSAT_DVBS2_SCAN_STATE_QPSK;

         BSAT_DEBUG_HP(BDBG_MSG(("Read MODCOD: actualMode=0x%X, pilot=%d", hChn->actualMode, acm_check & 0x01)));

         if ((oldScanState & BSAT_DVBS2_SCAN_STATE_FOUND) == 0)
         {
            if (hChn->dvbs2ScanState & BSAT_DVBS2_SCAN_STATE_PILOT)
            {
               /* fine freq est was off when we initially locked the HP in modcod scan mode */
               /* reacquire the HP again with fine freq enabled */
               if (BSAT_g1_P_HpOkToEnableFineFreqEst_isr(h))
               {
                  reacquire_hp:
                  BSAT_g1_P_HpAcquire1_isr(h);
                  return;
               }
            }
         }

         /* program new modcod and type */
         val = 0x80 | modcod | ((acm_check & 0x03) << 5);
         BSAT_g1_P_ReadModifyWriteRegister_isrsafe(h, BCHP_SDS_HP_HPCONFIG, ~0x7F, val);

         BSAT_g1_P_OrRegister_isrsafe(h, BCHP_SDS_HP_HPCONTROL, 0x1000); /* set acq_mode_delayed */
         BSAT_g1_P_AndRegister_isrsafe(h, BCHP_SDS_HP_HPCONFIG, ~0x2000); /* acm_mode=0 */
      }
   }
#endif

   hChn->nextFunct(h);
}


/******************************************************************************
 BSAT_g1_P_HpStateChange_isr() - callback routine for HP state change irq
******************************************************************************/
void BSAT_g1_P_HpStateChange_isr(void *p, int int_id)
{
   BSAT_ChannelHandle h = (BSAT_ChannelHandle)p;
   BSAT_g1_P_IncrementInterruptCounter_isr(h, int_id);
}


/******************************************************************************
 BSAT_g1_P_HpEnable()
******************************************************************************/
BERR_Code BSAT_g1_P_HpEnable(BSAT_ChannelHandle h, bool bEnable)
{
   BERR_Code retCode;

   BKNI_EnterCriticalSection();
   retCode = BSAT_g1_P_HpEnable_isr(h, bEnable);
   BKNI_LeaveCriticalSection();

   return retCode;
}


/******************************************************************************
 BSAT_g1_P_HpEnable_isr()
******************************************************************************/
BERR_Code BSAT_g1_P_HpEnable_isr(BSAT_ChannelHandle h, bool bEnable)
{
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   BSAT_CHK_RETCODE(BSAT_g1_P_HpDisableInterrupts_isr(h));

   if (bEnable)
   {
      /* enable */
      BINT_ClearCallback_isr(hChn->hHpStateMatchCb);
      BINT_EnableCallback_isr(hChn->hHpStateMatchCb);
      BSAT_g1_P_ReadModifyWriteRegister_isrsafe(h, BCHP_SDS_HP_HPCONTROL, ~1, 8);
   }
   else
   {
      /* disable FROF1, FROF2, FROF3 */
      BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_FROF1_SW, 0);
      BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_FROF2_SW, 0);
      BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_FROF3_SW, 0);

      BSAT_g1_P_ReadModifyWriteRegister_isrsafe(h, BCHP_SDS_HP_HPCONTROL, ~0x1808, 1); /* acm_mode_delayed = 0, caren=0 */
      BSAT_g1_P_AndRegister_isrsafe(h, BCHP_SDS_HP_HPCONFIG, ~0xC000);

      /* assert micro override of HP controls to receiver per Steve */
      BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_HPOVERRIDE, 0x003FFFFF);
   }

   done:
   return retCode;
}


/******************************************************************************
 BSAT_g1_P_HpConfig_isr()
******************************************************************************/
BERR_Code BSAT_g1_P_HpConfig_isr(BSAT_ChannelHandle h)
{
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t hpconfig, hpcontrol, fnorm, dcorr_threshold, hpoverride, m_n_peak_verify, m_n_receiver_verify, m_n_receiver_lock;
   uint32_t frame_length_sample, frof3_sw, hp_dafe, new_state, ignore_phi_from_dafe;
   uint32_t P_hi, P_lo, Q_hi;
   uint16_t turbo_frame_length = 0;
   uint8_t tfec_symbol_length, n_check, m_peak, plscode, modcod, s2_type, mask, n_check_limit, dafe_average;
   uint8_t peak_verify_n_check, peak_verify_m_peak, rcvr_verify_n_check, rcvr_verify_m_peak, rcvr_lock_n_check, rcvr_lock_m_peak;
   bool bDvbs2Pilot = false, bDvbs2Mode = false;

   if (BSAT_MODE_IS_DVBS2(hChn->acqSettings.mode) || BSAT_MODE_IS_DVBS2X(hChn->acqSettings.mode))
      bDvbs2Mode = true;

   /* determine if fine freq estimation should be enabled */
   hChn->bEnableFineFreqEst = false;
   if (bDvbs2Mode)
   {
      /* DVB-S2* mode */
      if ((hChn->acqSettings.mode != BSAT_Mode_eDvbs2_ACM) &&
          ((hChn->dvbs2ScanState & BSAT_DVBS2_SCAN_STATE_ENABLED) == 0) &&
          (hChn->acqSettings.options & BSAT_ACQ_PILOT))
      {
         dvbs2_pilot_mode:
         bDvbs2Pilot = true;
         hChn->bEnableFineFreqEst = BSAT_g1_P_HpOkToEnableFineFreqEst_isr(h);
      }
      else
      {
         mask = BSAT_DVBS2_SCAN_STATE_ENABLED | BSAT_DVBS2_SCAN_STATE_FOUND | BSAT_DVBS2_SCAN_STATE_PILOT;
         if ((hChn->dvbs2ScanState & mask) == mask)
            goto dvbs2_pilot_mode; /* modcod scan mode: from previous hp acquisition, we know there is pilot, so enable fine freq est */
      }
   }

   /* calculate HP_FROF3_SW */
   if (BSAT_MODE_IS_TURBO(hChn->acqSettings.mode))
      frof3_sw = 1; /* disable FROF3 for turbo */
   else if (hChn->bEnableFineFreqEst)
      frof3_sw = BCHP_SDS_HP_0_FROF3_SW_FROF3_USE_FINE_FREQ_MASK;
   else
      frof3_sw = 0;

   /* calculate HPOVERRIDE */
   hpoverride = 0x7000;
   if (BSAT_MODE_IS_TURBO(hChn->acqSettings.mode))
      hpoverride |= 0x0F00;
   else
      hpoverride |= 0x000F;

   /* calculate HPCONFIG */
   hpconfig = 0x40000; /* frof2_accum=1 */
   s2_type = 0;
   if (bDvbs2Mode)
   {
      hpconfig |= 0x1000; /* dvbs2_mode=1 */
      hpconfig |= (90 << 24); /* trnlen=90 */

      mask = BSAT_DVBS2_SCAN_STATE_ENABLED | BSAT_DVBS2_SCAN_STATE_FOUND | BSAT_DVBS2_SCAN_STATE_PILOT;
      if ((((hChn->dvbs2ScanState & BSAT_DVBS2_SCAN_STATE_ENABLED) == 0) && (hChn->acqSettings.options & BSAT_ACQ_PILOT)) ||
          ((hChn->dvbs2ScanState & mask) == mask))
      {
         s2_type |= 1;
      }
      if (BSAT_MODE_IS_DVBS2X(hChn->acqSettings.mode))
      {
         hpconfig |= (1<<BCHP_SDS_HP_0_HPCONFIG_modcod_msb_for_dvbs2x_SHIFT); /* modcod_msb_for_dvbs2x=1 */
         plscode = BSAT_P_dvbs2x_plscode[hChn->acqSettings.mode - BSAT_Mode_eDvbs2x_Qpsk_13_45];
         modcod = (plscode >> 2) & 0x1F;
      }
      else
      {
         if (hChn->acqSettings.options & BSAT_ACQ_DVBS2_SHORT_FRAMES)
            s2_type |= 2;
         modcod = hChn->acqSettings.mode - BSAT_Mode_eDvbs2_Qpsk_1_4 + 1;
      }

      if ((hChn->acqSettings.mode == BSAT_Mode_eDvbs2_ACM) || (hChn->dvbs2ScanState & BSAT_DVBS2_SCAN_STATE_ENABLED))
      {
         /* modcod scan or ACM */
         hpconfig |= 0x10000; /* dcorr_modcod_search=1 */
         hpconfig |= (3 << 8); /* hdr_mode=3 */

         if (hChn->acqSettings.mode == BSAT_Mode_eDvbs2_ACM)
            hpconfig |= 0x2000; /* acm_mode=1 */
      }
      else
      {
         hpconfig |= 0x80; /* use_sw_modcod_type=1 */
         hpconfig |= (1 << 8); /* hdr_mode=1 */
      }
   }
   else
   {
      /* turbo mode */
      hpconfig |= 0x20000; /* one_of_n_pn_mode=1 */
      hpconfig |= 0x80;    /* use_sw_modcod_type=1 */
      if (BSAT_MODE_IS_TURBO_QPSK(hChn->acqSettings.mode))
      {
         hpconfig |= (128 << 24); /* trnlen=128 */
         hpconfig |= 0x800; /* turbo_qpsk_mode=1 */
         modcod = 29;
      }
      else
      {
         hpconfig |= (64 << 24); /* trnlen=64 */
         modcod = 30;
      }
   }
   hpconfig |= (modcod << BCHP_SDS_HP_0_HPCONFIG_modcod_SHIFT);
   hpconfig |= (s2_type << BCHP_SDS_HP_0_HPCONFIG_type_SHIFT);

   /* calculate HP_DAFE */
   if (BSAT_MODE_IS_TURBO(hChn->acqSettings.mode))
   {
      dafe_average = 8;
      hp_dafe = 0;
   }
   else
   {
      dafe_average = 4;
      hp_dafe = 2; /* use_hns_coefficients=1 */
   }
   hp_dafe |= (dafe_average << 8);
   hp_dafe |= 0x00120008; /* read_internal_FROF_values=1, dafe_n_frof3=2, dafe_n_frof2=1, dafe_n_frof1=0, dafe_n_pilot=0 */

   /* calculate NEW_STATE */
   n_check = 9;
   m_peak = 3;
   new_state = 0x01000001; /* use_state_6=1, update_frof3_every_frame=1 */
   new_state |= (n_check << BCHP_SDS_HP_0_NEW_STATE_N_Check_SHIFT);
   new_state |= (m_peak << BCHP_SDS_HP_0_NEW_STATE_M_Peak_SHIFT);
   if (bDvbs2Pilot)
      new_state |= 0x10; /* no_frequency_estimates_on_state_4=1 */
   if (hChn->acqSettings.mode == BSAT_Mode_eDvbs2_ACM)
      new_state |= 0x10000000; /* use_rm_decoder_to_decode_pilot_state=1 */

   /* calculate IGNORE_PHI_FROM_DAFE */
   ignore_phi_from_dafe = 0x00111100;

   /* calculate HPCONTROL */
   hpcontrol = 0x0012E802; /* update_shadow_register=1, caren=1, pfc_rst=1, one_of_n_direction=1, trn_valid_dir=1, equalizer_pipeline_delay=9 */
   if (bDvbs2Pilot && !hChn->bEnableFineFreqEst)
      hpcontrol |= 0x4; /* pilot_dafe_start_enable=1 */
   if ((hChn->acqSettings.mode == BSAT_Mode_eTurbo_8psk_8_9) || (hChn->acqSettings.mode == BSAT_Mode_eTurbo_Qpsk_5_6))
      tfec_symbol_length = 6;
   else if (hChn->acqSettings.mode == BSAT_Mode_eTurbo_Qpsk_1_2)
      tfec_symbol_length = 1;
   else if (hChn->acqSettings.mode == BSAT_Mode_eTurbo_Qpsk_2_3)
      tfec_symbol_length = 3;
   else if (hChn->acqSettings.mode == BSAT_Mode_eTurbo_Qpsk_3_4)
      tfec_symbol_length = 4;
   else
      tfec_symbol_length = 5;
   hpcontrol |= (tfec_symbol_length << BCHP_SDS_HP_0_HPCONTROL_one_of_n_period_SHIFT);
   hpcontrol |= (32<<BCHP_SDS_HP_0_HPCONTROL_hopeless_modcod_SHIFT); /* hopeless_modcod=32 */

   /* calculate FNORM */
   BMTH_HILO_32TO64_Mul(1073741824, hChn->acqSettings.symbolRate, &P_hi, &P_lo);
   BMTH_HILO_64TO64_Div32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, &fnorm);

   /* calculate M_N_PEAK_VERIFY, M_N_RECEIVER_VERIFY, M_N_RECEIVER_LOCK, DCORR_THRESHOLD, FRAME_LENGTH_SAMPLE */
   n_check_limit = 9;
   peak_verify_n_check = (n_check_limit >= dafe_average) ? n_check_limit : (dafe_average+1);
   peak_verify_m_peak = bDvbs2Mode ? 2 : 3;
   rcvr_verify_n_check = 9;
   rcvr_verify_m_peak = 4;
   rcvr_lock_n_check = 9;
   rcvr_lock_m_peak = 4;
   m_n_peak_verify = (peak_verify_n_check << 8) | peak_verify_m_peak;
   m_n_receiver_verify = (rcvr_verify_n_check << 8) | rcvr_verify_m_peak;
   m_n_receiver_lock = (rcvr_lock_n_check << 8) | rcvr_lock_m_peak;
   dcorr_threshold = 1; /* N_Check=1 */
   frame_length_sample = 100000; /* frame_length=100000 */

   /* write the registers */
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_HPCONTROL, hpcontrol);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_HPCONFIG, hpconfig);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_FNORM, fnorm);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_DCORR_THRESHOLD, dcorr_threshold);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_HPOVERRIDE, hpoverride);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_M_N_PEAK_VERIFY, m_n_peak_verify);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_M_N_RECEIVER_VERIFY, m_n_receiver_verify);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_M_N_RECEIVER_LOCK, m_n_receiver_lock);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_FRAME_LENGTH_SAMPLE, frame_length_sample);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_FROF3_SW, frof3_sw);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_HP_DAFE, hp_dafe);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_NEW_STATE, new_state);
   BSAT_g1_P_WriteRegister_isrsafe(h, BCHP_SDS_HP_IGNORE_PHI_FROM_DAFE, ignore_phi_from_dafe);

//EFREN: START HERE....

   BSAT_g1_P_AndRegister_isrsafe(h, BCHP_SDS_CL_CLCTL1, 0xFFFFFFEF);  /* disable front carrier loop */
   BSAT_g1_P_OrRegister_isrsafe(h, BCHP_SDS_CL_CLCTL2, 0x00000004);   /* freeze front carrier loop */

   return retCode;
}


/******************************************************************************
 BSAT_g1_P_OnHpTimeOut_isr()
******************************************************************************/
BERR_Code BSAT_g1_P_OnHpTimeOut_isr(BSAT_ChannelHandle h)
{
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;
   uint32_t val, hpstate;
   BERR_Code retCode = BERR_SUCCESS;

   BSAT_DEBUG_HP(BDBG_WRN(("in BSAT_g1_P_OnHpTimeOut_isr")));

   /* check if HP is locked (state 5) */
   val = BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_SDS_HP_ACM_CHECK);
   hpstate = (val >> 8) & 0x07;
   if (hpstate == 5)
   {
      /* for some reason, we didn't get the HP state match irq */
      BSAT_DEBUG_HP(BDBG_WRN(("in BSAT_g1_P_OnHpTimeOut_isr(): locked (HP_ACM_CHECK=0x%X)", val)));
      BSAT_g1_P_HpStateMatch_isr((void*)h, 0);
      return BERR_SUCCESS;
   }

#ifndef BSAT_EXCLUDE_TFEC
   if (hChn->acqSettings.mode == BSAT_Mode_eTurbo_scan)
   {
      if (BSAT_MODE_IS_TURBO_8PSK(hChn->actualMode))
         hChn->turboScanState |= BSAT_TURBO_SCAN_STATE_8PSK_FAILED;
   }
#endif

   hChn->reacqCause = BSAT_ReacqCause_eHpCouldNotLock;
   BSAT_CHK_RETCODE(BSAT_ModeFunct[hChn->acqType].Reacquire(h));

   done:
   return retCode;
}


/******************************************************************************
 BSAT_g1_P_HpAcquire_isr()
******************************************************************************/
BERR_Code BSAT_g1_P_HpAcquire_isr(BSAT_ChannelHandle h, BSAT_g1_FUNCT funct)
{
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   BSAT_DEBUG_HP(BDBG_MSG(("in BSAT_g1_P_HpAcquire")));

   hChn->nextFunct = funct; /* passFunct = function to call after HP locks */
   hChn->bEnableFineFreqEst = false;

   BSAT_CHK_RETCODE(BSAT_g1_P_LogTraceBuffer_isr(h, BSAT_TraceEvent_eStartHp));
   BSAT_CHK_RETCODE(BSAT_g1_P_HpAcquire1_isr(h));

   done:
   return retCode;
}


/******************************************************************************
 BSAT_g1_P_HpAcquire1_isr()
******************************************************************************/
BERR_Code BSAT_g1_P_HpAcquire1_isr(BSAT_ChannelHandle h)
{
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   /* configure the HP */
   BSAT_CHK_RETCODE(BSAT_g1_P_HpEnable_isr(h, false));
   BSAT_CHK_RETCODE(BSAT_g1_P_HpConfig_isr(h));
   BSAT_CHK_RETCODE(BSAT_g1_P_AfecSetScramblingSeq_isr(h));

   /* interrupt on HP state match 5 (RECEIVER_LOCK) */
   BSAT_g1_P_ReadModifyWriteRegister_isrsafe(h, BCHP_SDS_HP_HPCONTROL, ~0xF0000000, 0x50000000);

   /* let the HP run */
   BSAT_CHK_RETCODE(BSAT_g1_P_HpEnable_isr(h, true));

   /* start reacquisition timer */
   retCode = BSAT_g1_P_EnableTimer_isr(h, BSAT_TimerSelect_eBaud, BSAT_HP_TIMEOUT, BSAT_g1_P_OnHpTimeOut_isr);

#ifndef BSAT_EXCLUDE_TFEC
   if (hChn->acqSettings.mode == BSAT_Mode_eTurbo_scan)
      hChn->turboScanState |= BSAT_TURBO_SCAN_STATE_HP_INIT;
#endif

   done:
   return retCode;
}


/******************************************************************************
 BSAT_g1_P_HpIsSpinv_isrsafe()
******************************************************************************/
bool BSAT_g1_P_HpIsSpinv_isrsafe(BSAT_ChannelHandle h)
{
   uint32_t val;

   val = BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_SDS_HP_ACM_CHECK);
   if (val & 0x80)
      return true;
   else
      return false;
}


/******************************************************************************
 BSAT_g1_P_HpGetFreqOffsetEstimate()
******************************************************************************/
BERR_Code BSAT_g1_P_HpGetFreqOffsetEstimate(BSAT_ChannelHandle h, int32_t *pFreq)
{
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;
   int32_t frof1, frof2, frof3, dafeint, carrier_error;
   uint32_t val, P_hi, P_lo, Q_hi, Q_lo;

   frof1 = (int32_t)(BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_SDS_HP_FROF1));
   frof2 = (int32_t)(BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_SDS_HP_FROF2));
   frof3 = (int32_t)(BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_SDS_HP_FROF3));
   dafeint = (int32_t)(BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_SDS_CL_DAFEINT));
   /* BDBG_MSG(("frof1=%d, frof2=%d, dafeint=%d, frof3=%d", frof1, frof2, dafeint, frof3)); */

   carrier_error = frof1 + frof2;

   /* check DAFE loop bypass and FROF3 accummulation */
   val = BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_SDS_HP_FROF3_SW);
   if (val & 0x02) /* use frof3 if fine freq est is enabled */
   {
      carrier_error += frof3;
   }

   carrier_error += dafeint;

   /* multiply by (Fs / 2^32) */
   if (carrier_error >= 0)
      val = (uint32_t)carrier_error;
   else
      val = (uint32_t)-carrier_error;
   BMTH_HILO_32TO64_Mul(val, hChn->sampleFreq / 2, &P_hi, &P_lo);
   BMTH_HILO_64TO64_Div32(P_hi, P_lo, 2147483648UL, &Q_hi, &Q_lo);
   if (carrier_error >= 0)
      carrier_error = (int32_t)Q_lo;
   else
      carrier_error = (int32_t)-Q_lo;

   if (BSAT_g1_P_HpIsSpinv_isrsafe(h))
      carrier_error = -carrier_error;  /* invert if HP is enabled and spinv detect */

   *pFreq = carrier_error;

   return BERR_SUCCESS;
}


/******************************************************************************
 BSAT_g1_P_HpOkToEnableFineFreqEst_isr()
******************************************************************************/
bool BSAT_g1_P_HpOkToEnableFineFreqEst_isr(BSAT_ChannelHandle h)
{
   BSAT_g1_P_ChannelHandle *hChn = (BSAT_g1_P_ChannelHandle *)h->pImpl;

   if ((hChn->dvbs2Settings.ctl & BSAT_DVBS2_CTL_DISABLE_FINE_FREQ_EST) == 0)
      return true;
   else
      return false;
}


/******************************************************************************
 BSAT_g1_P_HpGetAcmStatus()
******************************************************************************/
BERR_Code BSAT_g1_P_HpGetAcmStatus(BSAT_ChannelHandle h, BSAT_g1_P_AcmStatus *pStatus)
{
   uint32_t val;

   val = BSAT_g1_P_ReadRegister_isrsafe(h, BCHP_SDS_HP_ACM_CHECK);
   pStatus->modcod = (uint8_t)((val >> 2) & 0x1F);
   pStatus->type = (uint8_t)(val & 0x3);
   pStatus->bSpinv = val & 0x80 ? true : false;
   return BERR_SUCCESS;
}
