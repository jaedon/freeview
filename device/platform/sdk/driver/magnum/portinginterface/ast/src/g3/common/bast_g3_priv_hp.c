/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv_hp.c $
 * $brcm_Revision: Hydra_Software_Devel/77 $
 * $brcm_Date: 2/6/13 10:15a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv_hp.c $
 * 
 * Hydra_Software_Devel/77   2/6/13 10:15a enavarro
 * SWSATFE-174: removed BAST_g3_P_HpEnableFineFreqEstimate() since it is
 * no longer used
 * 
 * Hydra_Software_Devel/76   2/3/13 12:06p enavarro
 * SWSATFE-174: dont use this file for 4538
 * 
 * Hydra_Software_Devel/75   2/1/13 3:39p enavarro
 * SWSATFE-140: fixed compile error when BAST_EXCLUDE_LDPC is defined
 * 
 * Hydra_Software_Devel/74   1/24/13 5:06p ckpark
 * SWSATFE-174: changed HPOVERRIDE for turbo mode
 * 
 * Hydra_Software_Devel/73   1/21/13 3:06p enavarro
 * SWSATFE-174: enable fine freq est in modcod scan mode when pilot is
 * detected
 * 
 * Hydra_Software_Devel/72   1/21/13 10:42a ckpark
 * SWSATFE-174: disable front carrier loop when fine freq est is on;
 * update carrier error calculation
 * 
 * Hydra_Software_Devel/71   1/4/13 9:43a ckpark
 * SWSATFE-174: set dafe_average to 0x1F if fine freq estimation is
 * enabled; dont override front carrier loop control by state machine
 * 
 * Hydra_Software_Devel/70   1/3/13 11:16a ckpark
 * SWSATFE-174: use CLDAFECTL to bypass DAFE loop inside the filter; set
 * HPCONTROL.pilot_dafe_start_enable according to Michael's sim C file
 * 
 * Hydra_Software_Devel/69   12/20/12 3:12p ckpark
 * SWSATFE-174: use_sw_modcod_type for CCM
 * 
 * Hydra_Software_Devel/68   12/18/12 3:04p ckpark
 * SWSATFE-174: let state machine reset the dafe loop filter
 * 
 * Hydra_Software_Devel/67   12/18/12 10:16a ckpark
 * SWSATFE-174: fine frequency estimator tweaks
 * 
 * Hydra_Software_Devel/66   11/21/12 8:27a enavarro
 * SWSATFE-174: bypass DAFE loop when fine freq estimation is enabled
 * 
 * Hydra_Software_Devel/65   10/24/12 4:25p enavarro
 * SWSATFE-174: ensure n_check > dafe_average in new HP
 * 
 * Hydra_Software_Devel/64   10/18/12 11:28a enavarro
 * SWSATFE-174: add support for new h/w changes for fine frequency
 * estimation
 * 
 * Hydra_Software_Devel/63   9/10/12 3:58p enavarro
 * SWSATFE-174: set no_frequency_estimates_on_state_4=1 always
 * 
 * Hydra_Software_Devel/62   8/2/12 5:42p ckpark
 * SWSATFE-212: LDPC modes, SPINV signal acquisition is fixed
 * 
 * Hydra_Software_Devel/61   7/24/12 2:25p enavarro
 * SWSATFE-86: fixed compiler errors when BAST_EXCLUDE_TURBO is defined
 * 
 * Hydra_Software_Devel/60   7/17/12 4:09p enavarro
 * SWSATFE-140: use HP state 6 if possible; dont do sw spinv scan if h/w
 * spinv scan exists; updated BAST_g3_P_HpIsSpinv() for h/w spinv scan
 * 
 * Hydra_Software_Devel/59   7/13/12 11:02a enavarro
 * SWSATFE-140: changes to support new HP design
 * 
 * Hydra_Software_Devel/58   3/28/12 12:51p enavarro
 * SWSATFE-140: fixed compiler error in 4528
 * 
 * Hydra_Software_Devel/57   3/16/12 4:21p enavarro
 * SWSATFE-140: added reacqCause to channel handle struct
 * 
 * Hydra_Software_Devel/56   3/15/12 9:44a enavarro
 * SWSATFE-177: updated HPCONTROL.ignore_phi_from_dafe for ACM per Michael
 * Hui
 * 
 * Hydra_Software_Devel/55   3/5/12 9:38a enavarro
 * SWSATFE-177: ACM tweaks
 * 
 * Hydra_Software_Devel/54   2/22/12 10:17a enavarro
 * SWSATFE-86: added support for BAST_LOG_REG_ACCESS build option
 * 
 * Hydra_Software_Devel/53   2/21/12 4:14p ckpark
 * SWSATFE-140: disable frof3 and enable front carrier loop for turbo
 * 
 * Hydra_Software_Devel/52   1/17/12 11:01a enavarro
 * SWSATFE-86: fixed compiler warnings for turbo-disabled chips
 * 
 * Hydra_Software_Devel/51   1/11/12 11:06a ckpark
 * SWSATFE-162: added code for enabling/disabling frof2
 * 
 * Hydra_Software_Devel/50   1/6/12 8:36a enavarro
 * SWSATFE-140: fixed compiler warning in 4517
 * 
 * Hydra_Software_Devel/49   1/4/12 3:53p enavarro
 * SWSATFE-162: added compile options to exclude ldpc and/or turbo
 * 
 * Hydra_Software_Devel/48   1/4/12 10:17a ckpark
 * SWSATFE-162: switch turbo spinv only when spinv scan is enabled
 * 
 * Hydra_Software_Devel/47   1/4/12 9:35a ckpark
 * SWSATFE-162: disable FROF2 for turbo; set dafe_average to 1 for turbo
 * 
 * Hydra_Software_Devel/46   1/2/12 10:37a ckpark
 * SWSATFE-166: changed turbo m/n parameters
 * 
 * Hydra_Software_Devel/45   12/27/11 11:11a enavarro
 * SWSATFE-166: reacquire timing loop when switching turbo spinv
 * 
 * Hydra_Software_Devel/44   12/22/11 1:01p ckpark
 * SWSATFE-166: clear spinv setting when disabling the hp
 * 
 * Hydra_Software_Devel/43   12/22/11 12:35p enavarro
 * SWSATFE-166: added function to calculate dafe freq offset
 * 
 * Hydra_Software_Devel/42   12/21/11 3:01p ckpark
 * SWSATFE-166: reprogram HP when setting turbo spinv; kill reacquisition
 * timer when hp locks
 * 
 * Hydra_Software_Devel/41   9/29/11 9:33a ckpark
 * SWSATFE-137: override SNORE reset
 * 
 * Hydra_Software_Devel/40   9/20/11 4:49p enavarro
 * SWSATFE-86: changed debug message
 * 
 * Hydra_Software_Devel/39   8/24/11 2:51p enavarro
 * SWSATFE-140: added support for BAST_EXCLUDE_LDPC build option
 * 
 * Hydra_Software_Devel/38   7/29/11 9:58a guangcai
 * SWSATFE-127: enable fine freq 2000 frames after stable ldpc lock
 * 
 * Hydra_Software_Devel/37   7/27/11 2:12p guangcai
 * SWSATFE-127: fixed compiler warning
 * 
 * Hydra_Software_Devel/36   7/21/11 2:22p guangcai
 * SWSATFE-127: Fine freq estimation is integrated in after Lock for 1000
 * PL frames
 * 
 * Hydra_Software_Devel/35   7/19/11 11:50a enavarro
 * SW7346-331: fixed unchecked return values
 * 
 * Hydra_Software_Devel/34   7/8/11 4:31p guangcai
 * SWSATFE-127: changes for fine freq estimation
 * 
 * Hydra_Software_Devel/33   7/8/11 11:07a enavarro
 * SWSATFE-127: changes for fine freq estimation
 * 
 * Hydra_Software_Devel/32   7/7/11 4:16p ckpark
 * SWSATFE-127: include fine frequency estimation only if
 * BAST_ENABLE_FINE_FREQ_ESTIMATION is defined
 * 
 * Hydra_Software_Devel/31   7/7/11 11:28a enavarro
 * SWSATFE-127: override frof3=0 during acquisition
 * 
 * Hydra_Software_Devel/30   7/7/11 10:37a enavarro
 * SWSATFE-127: reworked fine freq estimation
 * 
 * Hydra_Software_Devel/29   6/23/11 3:21p enavarro
 * SWSATFE-127: disable HP frame boundary callbacks in
 * BAST_g3_P_HpDisableInterrupts()
 * 
 * Hydra_Software_Devel/28   6/21/11 3:01p enavarro
 * SWSATFE-86: removed debug message
 * 
 * Hydra_Software_Devel/27   6/20/11 2:16p enavarro
 * SWSATFE-127: check for FROF3 < 7KHz before enabling fine frequency
 * estimate
 * 
 * Hydra_Software_Devel/26   6/16/11 11:34a enavarro
 * SWSATFE-127: disable fine freq estimate at start of HpAcquire() only
 * for 4528/7358
 * 
 * Hydra_Software_Devel/25   6/16/11 9:10a enavarro
 * SWSATFE-127: added fine freq offset estimation for DVB-S2
 * 
 * Hydra_Software_Devel/24   4/21/11 5:06p ckpark
 * SWSATFE-75: read modcod status to see if stable
 * 
 * Hydra_Software_Devel/23   4/7/11 2:29p enavarro
 * SWSATFE-110: removed debug message
 * 
 * Hydra_Software_Devel/22   3/31/11 10:08a enavarro
 * SWSATFE-110: fixed compiler error for 7346
 * 
 * Hydra_Software_Devel/21   3/31/11 9:42a enavarro
 * SWSATFE-110: verify HP receiver lock state and modcod are stable before
 * declaring HP locked
 * 
 * Hydra_Software_Devel/20   3/30/11 11:48a enavarro
 * SWSATFE-110: make sure modcod is valid when HP locks
 * 
 * Hydra_Software_Devel/19   3/7/11 8:31a enavarro
 * SWSATFE-86: print BDBG_MSG when HP is locked
 * 
 * Hydra_Software_Devel/18   3/3/11 11:46a enavarro
 * SWSATFE-75: fixed compiler errors when BAST_EXCLUDE_TURBO is defined
 * 
 * Hydra_Software_Devel/17   12/27/10 3:39p enavarro
 * SWSATFE-75: added support for turbo scan
 * 
 * Hydra_Software_Devel/16   12/22/10 3:34p enavarro
 * SWSATFE-75: fixed bug when setting BAST_LDPC_SCAN_STATE_PILOT bit in
 * modcod search
 * 
 * Hydra_Software_Devel/15   12/22/10 11:12a enavarro
 * SWSATFE-75: turn off debugging messages by default
 * 
 * Hydra_Software_Devel/14   12/16/10 4:56p enavarro
 * SWSATFE-75: removed debug code
 * 
 * Hydra_Software_Devel/13   12/14/10 3:56p ronchan
 * SWSATFE-75: fix compile warning
 * 
 * Hydra_Software_Devel/12   12/14/10 3:32p enavarro
 * SWSATFE-75: increased HP timeout
 * 
 * Hydra_Software_Devel/11   12/14/10 9:43a enavarro
 * SWSATFE-75: enable HP interrupts before starting the HP
 * 
 * Hydra_Software_Devel/10   12/10/10 4:48p enavarro
 * SWSATFE-75: added debug messages
 * 
 * Hydra_Software_Devel/9   11/30/10 3:00p enavarro
 * SWSATFE-75: added comments
 * 
 * Hydra_Software_Devel/8   11/22/10 3:17p enavarro
 * SWSATFE-75: renamed hHpCb to hHpLockCb; renamed BAST_g3_P_Hp_isr() to
 * BAST_g3_HPLock_isr(); changes for turbo mode; disable HP interrupt on
 * receiver lock
 * 
 * Hydra_Software_Devel/7   10/5/10 3:05p enavarro
 * SWSATFE-75: fixed compiler warning
 * 
 * Hydra_Software_Devel/6   10/4/10 2:39p enavarro
 * SWSATFE-75: added BAST_g3_P_HpDisableInterrupts() and
 * BAST_g3_P_HpEnableInterrupts()
 * 
 * Hydra_Software_Devel/5   9/22/10 5:18p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/4   9/22/10 9:49a enavarro
 * SWSATFE-75: added BAST_g3_P_IsHpLocked()
 * 
 * Hydra_Software_Devel/3   9/21/10 10:27a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/2   9/17/10 5:25p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/1   9/16/10 2:06p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"

#if (BCHP_CHIP == 4538)
#error "This file should not be compiled into the BCM4538 firmware."
#endif

BDBG_MODULE(bast_g3_priv_hp);

#define BAST_DEBUG_HP(x) /* x */

#define BAST_HP_STATE_FIRST_PEAK       0
#define BAST_HP_STATE_PEAK_VERIFY_WAIT 1
#define BAST_HP_STATE_PEAK_VERIFY      2
#define BAST_HP_STATE_RCVR_LOCK_WAIT   3
#define BAST_HP_STATE_RCVR_VERIFY      4
#define BAST_HP_STATE_RCVR_LOCK        5

#define BAST_HP_TIMEOUT 2000000 /* time in baud clocks to wait for rcvr lock */
#define BAST_MODCOD_STABLE_TIMEOUT 150000 /* time in baud clocks to ensure modcod is stable */
#define hp_state_change_MASK 0x01000000 /* BCHP_SDS_INTR2_0_CPU_CLEAR_hp_state_change_MASK */

/* local functions */
BERR_Code BAST_g3_P_HpAcquire1(BAST_ChannelHandle h);
void BAST_g3_P_HpSetFineFrequencyEstimate(BAST_ChannelHandle h);


/******************************************************************************
 BAST_g3_P_HpDisableInterrupts()
******************************************************************************/
BERR_Code BAST_g3_P_HpDisableInterrupts(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BINT_DisableCallback_isr(hChn->hHpLockCb);
   BINT_ClearCallback_isr(hChn->hHpLockCb);

   if (hChn->hHpFrameBoundaryCb)
   {
      BINT_DisableCallback_isr(hChn->hHpFrameBoundaryCb);
      BINT_ClearCallback_isr(hChn->hHpFrameBoundaryCb);
   }

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_HpEnableInterrupts()
******************************************************************************/
BERR_Code BAST_g3_P_HpEnableInterrupts(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BINT_ClearCallback_isr(hChn->hHpLockCb);
   BINT_EnableCallback_isr(hChn->hHpLockCb);
   return BERR_SUCCESS;   
}


/******************************************************************************
 BAST_g3_P_HpEnable()
******************************************************************************/
BERR_Code BAST_g3_P_HpEnable(BAST_ChannelHandle h, bool bEnable)
{
   BERR_Code retCode;
   uint32_t val;
   
   BAST_CHK_RETCODE(BAST_g3_P_HpDisableInterrupts(h));
   
   if (bEnable)
   {
      /* enable */
      BAST_CHK_RETCODE(BAST_g3_P_HpEnableInterrupts(h));      
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_HP_HPCONTROL, ~1, 8);
   }
   else
   {
      /* disable FROF1, FROF2, FROF3 */
      val = 0;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FROF1_SW, &val);
      BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FROF2_SW, &val);
      BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FROF3_SW, &val);

      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_HP_HPCONTROL, ~0x1808, 1); /* acm_mode_delayed = 0, caren=0 */
      BAST_g3_P_AndRegister(h, BCHP_SDS_HP_HPCONFIG, ~0xC000);

      /* assert micro override of HP controls to receiver per Steve */
      val = 0x003FFFFF;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_HPOVERRIDE, &val);
   }
   
   done:
   return retCode;
}


#if defined(BCHP_SDS_HP_HP_DAFE)
/******************************************************************************
 BAST_g3_P_HpSetDafeAverage()
******************************************************************************/
void BAST_g3_P_HpSetDafeAverage(BAST_ChannelHandle h, uint8_t dafe_average)
{
   uint32_t val;
   
   val = 0x20230020 | (dafe_average << 8);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_HP_DAFE, &val); 
}
#endif

      
/******************************************************************************
 BAST_g3_P_HpConfig()
******************************************************************************/
BERR_Code BAST_g3_P_HpConfig(BAST_ChannelHandle h)
{
#ifndef BAST_EXCLUDE_LDPC   
   static const uint32_t script_hp_config_0[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_8PSK_NORMAL, (22194 << 16) | 21690),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_8PSK_SHORT, (5598 << 16) | 5490),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_QPSK_NORMAL, (33282 << 16) | 32490),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_QPSK_SHORT, (8370 << 16) | 8190),  
      BAST_SCRIPT_EXIT
   }; 
#endif
   
   static const uint32_t script_hp_config_1[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_DUMMY_NORMAL, (3330 << 16) | 3330),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_16APSK_NORMAL, (16686 << 16) | 16290),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_32APSK_NORMAL, (13338 << 16) | 13050),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_RESERVED_29_NORMAL, (3330 << 16) | 3330),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_RESERVED_30_NORMAL, (3330 << 16) | 3330),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_RESERVED_31_NORMAL, (3330 << 16) | 3330),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_DUMMY_SHORT, (3330 << 16) | 3330),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_16APSK_SHORT, (4212 << 16) | 4140),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_32APSK_SHORT, (3402 << 16) | 3330),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_RESERVED_29_SHORT, (3330 << 16) | 3330),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_RESERVED_30_SHORT, (3330 << 16) | 3330),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_RESERVED_31_SHORT, (3330 << 16) | 3330),
      BAST_SCRIPT_WRITE(BCHP_SDS_HP_FRAME_LENGTH_SAMPLE, 3 * 33282),
#ifndef BAST_ENABLE_FINE_FREQ_ESTIMATION      
      BAST_SCRIPT_AND(BCHP_SDS_HP_HPCONFIG, ~0x00080400), /* frof3_accum=0, dafe_loop_bypass=0 */
#endif      
      BAST_SCRIPT_EXIT
   }; 
   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;      
   BERR_Code retCode;
   uint32_t hpconfig, val, P_hi, P_lo, Q_hi, Q_lo, dafe_average;
#ifndef BAST_EXCLUDE_TURBO   
   uint16_t turbo_frame_length = 0;
#endif
   uint8_t peak_verify_n_check, peak_verify_m_peak, rcvr_verify_n_check, rcvr_verify_m_peak, rcvr_lock_n_check, rcvr_lock_m_peak;
#ifdef BAST_ENABLE_FINE_FREQ_ESTIMATION   
   uint8_t mask;
#endif
      
   /* set default values */ /* to make maximum DAFE freq average up to 16 PL Frames */
   peak_verify_n_check = 20;
   peak_verify_m_peak = 4;
   rcvr_verify_n_check = 9;
   rcvr_verify_m_peak = 4;
   rcvr_lock_n_check = 9;
   rcvr_lock_m_peak = 4;

   hpconfig = 0x00040000; /* orig: 0x000C0400 */

   if (hDev->sdsRevId < 0x68)
   {
      if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
      {   
         dafe_average = ((uint32_t)15 << 20); /* dafe_average must be < peak_verify_n_check */      
      }
      else
      {   
         /* turbo: set to 1 because frof2 is disabled */
         dafe_average = ((uint32_t)1 << 20); /* dafe_average must be < peak_verify_n_check */
       
         val = 1;
         BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FROF2_SW, &val); /* disable FROF2 */      
      }

      hpconfig |= dafe_average;
   }
   else
   {
#if defined(BCHP_SDS_HP_HP_DAFE)
      dafe_average = 0x0F; 
      if (dafe_average > peak_verify_n_check)
         peak_verify_n_check = dafe_average + 1; /* make sure n_check > dafe_average */
      BAST_g3_P_HpSetDafeAverage(h, dafe_average);     

      val = 0x000A0401;
#ifndef BAST_EXCLUDE_LDPC
      if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
      {
         if ((hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_ENABLED) || 
             (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_LDPC_PILOT) ||
             (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM))
         {
            /* We typically set no_frequency_estimates_on_state_4 to 1 for dvb-s2 pilot modes.  
               However for ACM or non-pilot modes, it is OK to leave this bit set since we'll only
               miss updating FROF3 for one single frame (during state 4) and that's not too bad. */
            val |= 0x10; /* no_frequency_estimates_on_state_4=1 */
         }
      }
#endif /* BAST_EXCLUDE_LDPC */ 
      BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_NEW_STATE, &val);       
#endif /* defined(BCHP_SDS_HP_HP_DAFE) */
   }

#ifndef BAST_EXCLUDE_LDPC      
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      hpconfig |= 0x5A001000; /* trnlen = 90, dvbs2_mode */
      if ((hChn->acqParams.mode == BAST_Mode_eLdpc_ACM) || 
          (hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_ENABLED))
      {
         /* ACM */
         hpconfig |= 0x00012300; /* set acm_mode, dcorr_modcod_search, hdr_mode=3 */
      }
      else        
      {
         /* CCM, non-modcod search */
         hpconfig |= 0x00000180; /* hdr_mode=1 */
         if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_LDPC_PILOT)
            hpconfig |= 0x00000020; /* type=1 */

         /* set modcod */
         hpconfig |= ((hChn->actualMode - BAST_Mode_eLdpc_Qpsk_1_2) + 4);
      }
   }
#endif  

#ifndef BAST_EXCLUDE_TURBO
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      /* turbo */    
      peak_verify_n_check = 20;
      peak_verify_m_peak = 12;
      rcvr_verify_n_check = 20;
      rcvr_verify_m_peak = 14;
      rcvr_lock_n_check = 20;
      rcvr_lock_m_peak = 17; 

      hpconfig |= 0x00020081; /* use_sw_modcod_type, set modcod=1, one_of_n_pn_mode */
      if ((hChn->bTurboSpinv) && (hDev->sdsRevId < 0x68))
      {
         hpconfig |= 0x0000C000; /* spinv */
         BAST_DEBUG_HP(BDBG_MSG(("setting hp spinv")));         
      }

      if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
         hpconfig |= 0x40000000; /* trnlen = 64 */
      else /* turbo qpsk */
         hpconfig |= 0x80000800; /* trnlen = 128, turbo_qpsk_mode */
   }
#endif

   if (hDev->sdsRevId < 0x68)
   {
      if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
         val = 2<<8; /* ignore_phi_from_dafe=2 per Michael */
      else
         val = 4<<8; /* ignore_phi_from_dafe=4 */
   }
   else
   {
#if defined(BCHP_SDS_HP_IGNORE_PHI_FROM_DAFE)
      if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
         val = 0x00111100; /* states peak_verify and up */
      else
         val = 0x00110000; /* states receiver_verify and up */
      BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_IGNORE_PHI_FROM_DAFE, &val);
#endif      
      
      val = 0; /* hpcontrol.ignore_phi_from_dafe was deleted in SDS versions 0x68 and later */
   }
   val |= 0x5812E853;
#ifndef BAST_EXCLUDE_TURBO
   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
   {
      /* set period of 1/N */
      val &= 0xFFFFFF0F;
      if (hChn->actualMode == BAST_Mode_eTurbo_8psk_3_4)
         val |= 0x70;
      else if (hChn->actualMode == BAST_Mode_eTurbo_8psk_8_9)
         val |= 0x60;
      else
         val |= 0x50;
   }
#endif   
   
#ifndef BAST_ENABLE_FINE_FREQ_ESTIMATION
#ifndef BAST_EXCLUDE_LDPC
   if (hDev->sdsRevId >= 0x68)
   {
      if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
      {
         if (BAST_g3_P_IsLdpcPilotOn(h))
            val |= 0x02;
      }
   }
#endif   
#endif

   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_HPCONTROL, &val);

   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_HPCONFIG, &hpconfig);

#ifndef BAST_EXCLUDE_TURBO   
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
      val = 1; /* disable FROF3 for turbo */
   else
#endif  
   {   
      /* LDPC mode */
      val = 0; 
#ifdef BAST_ENABLE_FINE_FREQ_ESTIMATION      
      if (hDev->sdsRevId >= 0x68)
      {
         if ((hChn->acqParams.mode != BAST_Mode_eLdpc_ACM) &&
             ((hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_ENABLED) == 0) &&
             (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_LDPC_PILOT))
         {
            /* in newer hp design, fine freq estimation can be enabled from the start for ldpc pilot modes */
            enable_fine_freq_est:
            BAST_g3_P_HpSetFineFrequencyEstimate(h);
#if defined(BCHP_SDS_HP_HP_DAFE)
            dafe_average = 0x1F; 
            if (dafe_average > peak_verify_n_check)
               peak_verify_n_check = dafe_average + 1; /* make sure n_check > dafe_average */
            BAST_g3_P_HpSetDafeAverage(h, dafe_average);     
#endif      
            goto after_fine_freq_est_enable;
         }
         
         mask = BAST_LDPC_SCAN_STATE_ENABLED | BAST_LDPC_SCAN_STATE_FOUND | BAST_LDPC_SCAN_STATE_PILOT;
         if ((hChn->ldpcScanState & mask) == mask)
            goto enable_fine_freq_est; /* modcod scan mode: from previous hp acquisition, we know there is pilot, so enable fine freq est */
      }
#endif      
   }
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FROF3_SW, &val);

#ifdef BAST_ENABLE_FINE_FREQ_ESTIMATION
   after_fine_freq_est_enable:
#endif   
   BAST_MultU32U32(1073741824, hChn->acqParams.symbolRate, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, &Q_lo);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FNORM, &Q_lo);

   /* TBD: determine threshold from 100000 symbols and set to about 1/2 peak */
   if (hDev->sdsRevId < 0x68)
      val = 1; /* do this for now... */
   else
      val = 0x100;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_DCORR_THRESHOLD, &val);

   val = peak_verify_m_peak | (peak_verify_n_check << 8);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_M_N_PEAK_VERIFY, &val);

   val = rcvr_verify_m_peak | (rcvr_verify_n_check << 8);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_M_N_RECEIVER_VERIFY, &val);

   val = rcvr_lock_m_peak | (rcvr_lock_n_check << 8);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_M_N_RECEIVER_LOCK, &val);

#ifndef BAST_EXCLUDE_TURBO
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      val = (2564 * 4) + ((hpconfig >> 24) & 0xFF); /* val=turbo_frame_length */
      turbo_frame_length = (uint16_t)val;
   }
#endif   
#ifndef BAST_EXCLUDE_LDPC
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      val = 33282;  /* use longest frame lengths for initial search (qpsk pilot) */
   }
#endif
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FRAME_LENGTH_INITIAL, &val);

#ifndef BAST_EXCLUDE_TURBO
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      val = (uint32_t)turbo_frame_length | (turbo_frame_length << 16);
      BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FRAME_LENGTH_8PSK_NORMAL, &val);
      BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FRAME_LENGTH_8PSK_SHORT, &val);
      BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FRAME_LENGTH_QPSK_NORMAL, &val);
      BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FRAME_LENGTH_QPSK_SHORT, &val);
   }
#endif   
#ifndef BAST_EXCLUDE_LDPC
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_hp_config_0)); 
   }    
#endif   

   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_hp_config_1)); 

#ifndef BAST_ENABLE_FINE_FREQ_ESTIMATION   
   val = 0x0022700F; /* orig: 0x0002700F; */
#ifndef BAST_EXCLUDE_TURBO   
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      /* set clfbintrstov(11), clfbrstov(10), clfbfrzov(9), clfbenov(8) */
      val |= 0x00000F00;
      val &= ~0x0002000F; /* enable front carrier loop */
   }
#endif
#else
   BAST_g3_P_AndRegister(h, BCHP_SDS_CL_CLCTL1, 0xFFFFFFEF);  /* disable front carrier loop */
   BAST_g3_P_OrRegister(h, BCHP_SDS_CL_CLCTL2, 0x00000004);   /* freeze front carrier loop */      
      
   val = 0x0000600F; /* dafe loop filter should be reset by state machine */ /* orig: 0x0000E000; */
#ifndef BAST_EXCLUDE_TURBO  
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      /* val |= 0x0F00; */
      val = 0x207F00;      
   }
#endif      
#endif
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_HPOVERRIDE, &val);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_HpAcquire()
******************************************************************************/
BERR_Code BAST_g3_P_HpAcquire(BAST_ChannelHandle h, BAST_g3_FUNCT funct)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   BAST_DEBUG_HP(BDBG_MSG(("in BAST_g3_P_HpAcquire")));
   
   hChn->passFunct = funct; /* passFunct = function to call after HP locks */
   hChn->count1 = 0;
   
   BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eStartHp));   
   BAST_CHK_RETCODE(BAST_g3_P_HpAcquire1(h));
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_HpAcquire1()
******************************************************************************/
BERR_Code BAST_g3_P_HpAcquire1(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   /* configure the HP */
   BAST_CHK_RETCODE(BAST_g3_P_HpEnable(h, false));  
   BAST_CHK_RETCODE(BAST_g3_P_HpConfig(h));

   /* interrupt on RECEIVER_LOCK state */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_HP_HPCONTROL, ~0xF0000000, 0x50000000);

   /* let the HP run */
   BAST_CHK_RETCODE(BAST_g3_P_HpEnable(h, true));

   /* start reacquisition timer */
   retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, BAST_HP_TIMEOUT, BAST_g3_P_OnHpTimeOut);  

#ifndef BAST_EXCLUDE_TURBO
   if (hChn->acqParams.mode == BAST_Mode_eTurbo_scan)
      hChn->turboScanState |= BAST_TURBO_SCAN_STATE_HP_INIT;
#endif      
      
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_HpIsSpinv()
******************************************************************************/
BERR_Code BAST_g3_P_HpIsSpinv(BAST_ChannelHandle h, bool *pSpinv)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl; 
#ifndef BAST_EXCLUDE_TURBO
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;            
#endif
   uint32_t val;

   *pSpinv = false;
#ifndef BAST_EXCLUDE_LDPC   
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_ACM_CHECK, &val);
      if (val & 0x80)
         *pSpinv = true;
   }
#endif
#ifndef BAST_EXCLUDE_TURBO
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      if (hDev->sdsRevId < 0x68)
      {
         /* software spinv scan */
         BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_HPCONFIG, &val);
         if ((val & 0xC000) == 0xC000)
            *pSpinv = true;
      }
      else
      {
         /* hardware spinv scan */
         BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_ACM_CHECK, &val);
         if (val & 0x80)
            *pSpinv = true;
      }
   }
#endif   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_HpGetFreqOffsetEstimate()
******************************************************************************/
BERR_Code BAST_g3_P_HpGetFreqOffsetEstimate(BAST_ChannelHandle h, int32_t *pFreq)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;   
   BERR_Code retCode;
   int32_t frof1, frof2, frof3, dafeint, carrier_error;
   uint32_t val, P_hi, P_lo, Q_hi, Q_lo;
   bool bSpinv;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_FROF1, (uint32_t*)&frof1);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_FROF2, (uint32_t*)&frof2);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_FROF3, (uint32_t*)&frof3);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_DAFEINT, (uint32_t*)&dafeint); 
   /* BDBG_P_PrintWithNewLine("frof1=%d, frof2=%d, dafeint=%d, frof3=%d", frof1, frof2, dafeint, frof3); */

   carrier_error = frof1 + frof2;

   /* check DAFE loop bypass and FROF3 accummulation */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_FROF3_SW, &val);
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
   BAST_MultU32U32(val, hChn->sampleFreq / 2, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 2147483648UL, &Q_hi, &Q_lo);
   if (carrier_error >= 0)
      carrier_error = (int32_t)Q_lo;
   else
      carrier_error = (int32_t)-Q_lo;
      
   BAST_CHK_RETCODE(BAST_g3_P_HpIsSpinv(h, &bSpinv));
   if (bSpinv)
      carrier_error = -carrier_error;  /* invert if HP is enabled and spinv detect */

   *pFreq = carrier_error;

   done:
   return BERR_SUCCESS;
}



/******************************************************************************
 BAST_g3_P_OnHpStableLock()
******************************************************************************/
BERR_Code BAST_g3_P_OnHpStableLock(BAST_ChannelHandle h)
{   
   BERR_Code retCode;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
#ifndef BAST_EXCLUDE_LDPC   
   uint32_t acm_check, modcod, val;
   BAST_Mode actualMode;
   uint8_t oldScanState;
#endif   

   /* HP is in RECEIVER_LOCK state */
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eBaud); /* kill the reacquisition timer */
   BAST_g3_P_HpDisableInterrupts(h);
   retCode = BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eRcvrLocked);
   if (retCode != BERR_SUCCESS)
   {
      BDBG_WRN(("BAST_g3_P_OnHpStableLock(): BAST_g3_P_LogTraceBuffer() error 0x%X", retCode));
   }
   BDBG_MSG(("HP(chan %d) locked", h->channel));  
   
#ifndef BAST_EXCLUDE_TURBO
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {
      BAST_g3_P_HpIsSpinv(h, &(hChn->bTurboSpinv));
      BAST_DEBUG_HP(BDBG_MSG(("Turbo SPINV = %d", hChn->bTurboSpinv)));
   }
#endif

#ifndef BAST_EXCLUDE_LDPC   
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_ACM_CHECK, &acm_check);
      if (acm_check & 0x02)
         hChn->ldpcCtl |= BAST_G3_CONFIG_LDPC_CTL_SHORT_FRAME_DETECTED;
      else
         hChn->ldpcCtl &= ~BAST_G3_CONFIG_LDPC_CTL_SHORT_FRAME_DETECTED;
         
      oldScanState = hChn->ldpcScanState;
      
      if (hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_ENABLED)
      {
         modcod = ((acm_check >> 2) & 0x1F);
         actualMode = BAST_Mode_eLdpc_Qpsk_1_2 + modcod - 4;   
         if ((BAST_MODE_IS_LDPC(actualMode) == 0) || (actualMode == BAST_Mode_eLdpc_scan) || (actualMode == BAST_Mode_eLdpc_ACM))
         {
            BDBG_ERR(("invalid modcod=0x%X, actualMode=0x%X", modcod, actualMode));
            hChn->reacqCause = BAST_ReacqCause_eInvalidModcod;
            return BAST_g3_P_Reacquire(h);           
         }
         hChn->actualMode = actualMode;

         hChn->ldpcScanState |= BAST_LDPC_SCAN_STATE_FOUND;
         if (acm_check & 0x01)
            hChn->ldpcScanState |= BAST_LDPC_SCAN_STATE_PILOT;
         else
            hChn->ldpcScanState &= ~BAST_LDPC_SCAN_STATE_PILOT;
         if (BAST_MODE_IS_LDPC_QPSK(hChn->actualMode))
            hChn->ldpcScanState |= BAST_LDPC_SCAN_STATE_QPSK;
         else
            hChn->ldpcScanState &= ~BAST_LDPC_SCAN_STATE_QPSK;

#ifdef BAST_ENABLE_FINE_FREQ_ESTIMATION
         if ((oldScanState & BAST_LDPC_SCAN_STATE_FOUND) == 0)
         {
            if (hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_PILOT)
            {
               /* fine freq est was off when we initially locked the HP in modcod scan mode */
               /* reacquire the HP again with fine freq enabled */
               return BAST_g3_P_HpAcquire1(h);
            }
         }
#endif


         BAST_DEBUG_HP(BDBG_MSG(("Read MODCOD: actualMode=0x%X, pilot=%d", hChn->actualMode, acm_check & 0x01)));  

         /* program new modcod and type */
         val = 0x80 | modcod | ((acm_check & 0x03) << 5);
         BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_HP_HPCONFIG, ~0x7F, val);

         BAST_g3_P_OrRegister(h, BCHP_SDS_HP_HPCONTROL, 0x1000); /* set acq_mode_delayed */
         BAST_g3_P_AndRegister(h, BCHP_SDS_HP_HPCONFIG, ~0x2000); /* acm_mode=0 */
      }
   }
#endif   
   
   return hChn->passFunct(h);
}


#ifndef BAST_EXCLUDE_LDPC
/******************************************************************************
 BAST_g3_P_HpCheckStableLock()
******************************************************************************/
BERR_Code BAST_g3_P_HpCheckStableLock(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val;
   uint8_t modcod;

   BAST_g3_P_ReadRegister(h, BCHP_SDS_INTR2_0_CPU_STATUS, &val);
   if (val & hp_state_change_MASK)
   {
      check_again:
      if (hChn->count1 < 1)
      {
         hChn->count1++;
         BAST_DEBUG_HP(BDBG_MSG(("modcod not stable after %d bclks", BAST_MODCOD_STABLE_TIMEOUT)));
         if ((retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, BAST_MODCOD_STABLE_TIMEOUT, BAST_g3_P_HpCheckStableLock)) != BERR_SUCCESS)
         {
            hChn->reacqCause = BAST_ReacqCause_eTimerError1;
            goto reacquire;
         }
      }
      else
      {
         hChn->reacqCause = BAST_ReacqCause_eModcodNotStable;
         
         reacquire:         
         return BAST_g3_P_Reacquire(h);
      }
   }
   else
   {
      BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_ACM_CHECK, &val);
      modcod = (uint8_t)(val & 0xFF);
      if (modcod != hChn->modcod)
      {
         hChn->modcod = modcod;
         BAST_DEBUG_HP(BDBG_MSG(("modcod changed")));
         goto check_again;
      }
      retCode = BAST_g3_P_OnHpStableLock(h);      
   }
      
   return retCode;
}
#endif


/******************************************************************************
 BAST_g3_P_HpSetFineFrequencyEstimate()
******************************************************************************/
void BAST_g3_P_HpSetFineFrequencyEstimate(BAST_ChannelHandle h)
{
   uint32_t val;
   
   val = 0x2;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_FROF3_SW, &val); /* use fine freq */
   val = 0x6509A;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLDAFECTL, &val); 
   BAST_DEBUG_HP(BDBG_MSG(("enable fine frequency estimate")));   
}


/******************************************************************************
 BAST_g3_P_HpFrameBoundary_isr()
******************************************************************************/
void BAST_g3_P_HpFrameBoundary_isr(void *p, int param)
{
   BSTD_UNUSED(p);
   BSTD_UNUSED(param);   
}


/******************************************************************************
 BAST_g3_P_HpLock_isr()
******************************************************************************/
void BAST_g3_P_HpLock_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;       
   BAST_g3_P_ChannelHandle *hChn = h->pImpl;
#ifndef BAST_EXCLUDE_LDPC        
   uint32_t val;
#endif
   
   BSTD_UNUSED(param);

#ifdef BAST_LOG_REG_ACCESS
   BKNI_Printf("hp lock isr\n");
#endif

   /* verify that we're in RECEIVER_LOCK state */
   if (BAST_g3_P_IsHpLocked(h) == false)
   {
#ifndef BAST_EXCLUDE_LDPC      
      hp_not_locked:
#endif      
      if (BAST_g3_P_IsTimerExpired(h, BAST_TimerSelect_eBaud) == false)
         BAST_g3_P_HpEnableInterrupts(h);
      else
      {
         hChn->reacqCause = BAST_ReacqCause_eHpCouldNotLock;
         BAST_g3_P_Reacquire(h); /* give up */
      }
      return;
   }

#ifndef BAST_EXCLUDE_LDPC
   /* make sure modcod is stable for a few frames (~150000 baud clocks) */
   if (BAST_MODE_IS_LDPC(hChn->acqParams.mode))
   {
      val = hp_state_change_MASK;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_INTR2_0_CPU_CLEAR, &val);
      if (BAST_g3_P_IsHpLocked(h) == false)
         goto hp_not_locked;

      BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eBaud); /* kill the reacquisition timer */   
      BAST_g3_P_HpDisableInterrupts(h);

      if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
         BAST_g3_P_OnHpStableLock(h);
      else
      {     
         /* save the modcod */
         BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_ACM_CHECK, &val);
         hChn->modcod = (uint8_t)(val & 0xFF);

         hChn->count1 = 0;
         if (BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, BAST_MODCOD_STABLE_TIMEOUT, BAST_g3_P_HpCheckStableLock) != BERR_SUCCESS)
         {
            hChn->reacqCause = BAST_ReacqCause_eTimerError2;
            BAST_g3_P_Reacquire(h);
         }
      }
      return;
   } 
#endif   

#ifndef BAST_EXCLUDE_TURBO
   /* this is assumed to be turbo */
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eBaud); /* kill the reacquisition timer */   
   BAST_g3_P_HpDisableInterrupts(h); 
   BAST_g3_P_OnHpStableLock(h);
#endif
}


/******************************************************************************
 BAST_g3_P_OnHpTimeOut()
******************************************************************************/
BERR_Code BAST_g3_P_OnHpTimeOut(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
#ifndef BAST_EXCLUDE_TURBO
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;         
#endif
   uint32_t val, hpstate;
   BERR_Code retCode = BERR_SUCCESS;

   BAST_DEBUG_HP(BDBG_WRN(("in BAST_g3_P_OnHpTimeOut")));

   /* check if HP is locked */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_ACM_CHECK, &val);
   hpstate = (val >> 8) & 0x07;   
   if (hpstate == BAST_HP_STATE_RCVR_LOCK)
   {
      BAST_DEBUG_HP(BDBG_WRN(("in BAST_g3_P_OnHpTimeOut(): locked (HP_ACM_CHECK=0x%X)", val)));      
      BAST_g3_P_HpLock_isr((void*)h, 0);
      return BERR_SUCCESS;
   }

#ifndef BAST_EXCLUDE_TURBO
   if (BAST_MODE_IS_TURBO(hChn->acqParams.mode))
   {     
      if (hDev->sdsRevId < 0x68)
      {
         /* versions older than 6.8 dont have turbo spinv scan h/w */
         if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_SPINV_IQ_SCAN) == BAST_ACQSETTINGS_SPINV_IQ_SCAN) 
         {         
            if (hChn->bTurboSpinv)
               hChn->bTurboSpinv = false;
            else
            {
               hChn->bTurboSpinv = true;               
               BAST_CHK_RETCODE(BAST_g3_P_HpEnable(h, false));
               return BAST_g3_P_Acquire1(h);            
            }
         }   
      }
   }

   if (hChn->acqParams.mode == BAST_Mode_eTurbo_scan)
   {
      if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
         hChn->turboScanState |= BAST_TURBO_SCAN_STATE_8PSK_FAILED;
   }
#endif   

   hChn->reacqCause = BAST_ReacqCause_eHpCouldNotLock;
   BAST_CHK_RETCODE(BAST_g3_P_Reacquire(h));
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_IsHpLocked()
******************************************************************************/
bool BAST_g3_P_IsHpLocked(BAST_ChannelHandle h)
{
   uint32_t acm_check, hpstate;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_ACM_CHECK, &acm_check);
   hpstate = (acm_check >> 8) & 0x07;   
   return (hpstate == BAST_HP_STATE_RCVR_LOCK) ? true : false;
}

