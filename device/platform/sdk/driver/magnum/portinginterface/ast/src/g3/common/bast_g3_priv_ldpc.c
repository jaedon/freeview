/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv_ldpc.c $
 * $brcm_Revision: Hydra_Software_Devel/88 $
 * $brcm_Date: 2/6/13 1:34p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv_ldpc.c $
 * 
 * Hydra_Software_Devel/88   2/6/13 1:34p ckpark
 * SWSATFE-174: added FROF2 workaround for 4538
 * 
 * Hydra_Software_Devel/87   2/6/13 10:15a enavarro
 * SWSATFE-174: removed BAST_g3_P_HpEnableFineFreqEstimate() since it is
 * no longer used
 * 
 * Hydra_Software_Devel/86   1/4/13 9:39a ckpark
 * SWSATFE-174: updated BAST_g3_P_LdpcOnHpLock()
 * 
 * Hydra_Software_Devel/85   12/18/12 3:06p ckpark
 * SWSATFE-174: disable front carrier loop always; remove tracking
 * settting for cldafectl when fine freq estimation enabled; dont re-
 * enable fine freq estimation after lock
 * 
 * Hydra_Software_Devel/84   12/18/12 1:38p enavarro
 * SWSATFE-174: updated HP input scaling for 4538
 * 
 * Hydra_Software_Devel/83   12/18/12 10:43a enavarro
 * SWSATFE-174: add getLockStatusFunct in channel handle struct
 * 
 * Hydra_Software_Devel/82   12/18/12 10:17a ckpark
 * SWSATFE-174: updated CLDAFECTL for fine freq estimation
 * 
 * Hydra_Software_Devel/81   12/12/12 3:14p enavarro
 * SWSATFE-234: added dynamic power down of SDS_OPLL
 * 
 * Hydra_Software_Devel/80   11/6/12 11:12a ronchan
 * SWSATFE-174: bypass dynamic power shutdown if ldpc is off
 * 
 * Hydra_Software_Devel/79   11/5/12 3:16p ronchan
 * SWSATFE-174: implement dynamic afec/tfec power management
 * 
 * Hydra_Software_Devel/78   10/29/12 1:37p ckpark
 * SWSATFE-225: allow some time for mpeg to regain lock if lost lock
 * 
 * Hydra_Software_Devel/77   10/18/12 12:04p enavarro
 * SWSATFE-174: changes for fine freq estimation
 * 
 * Hydra_Software_Devel/76   10/18/12 11:20a enavarro
 * SWSATFE-174: enable fine frequency estimation on 4538
 * 
 * Hydra_Software_Devel/75   10/17/12 10:37a enavarro
 * SWSATFE-174: enable fine frequency estimation only for ldpc pilot modes
 * 
 * Hydra_Software_Devel/74   10/11/12 10:07a ckpark
 * SWSATFE-219: fixed ldpc pilot scan bug
 * 
 * Hydra_Software_Devel/73   9/26/12 3:48p enavarro
 * SWSATFE-174: fixed compiler warning
 * 
 * Hydra_Software_Devel/72   9/26/12 9:37a enavarro
 * SWSATFE-174: monitor for ldpc mpeg lock
 * 
 * Hydra_Software_Devel/71   9/11/12 11:09a enavarro
 * SWSATFE-174: fixed snore programming for sds revid 0x64 and later
 * 
 * Hydra_Software_Devel/70   7/5/12 4:09p enavarro
 * SWSATFE-174: changes for 4538
 * 
 * Hydra_Software_Devel/69   6/21/12 11:40a ckpark
 * SWSATFE-203: use computed afec clock for computing cycle count
 * 
 * Hydra_Software_Devel/68   5/25/12 11:15a ckpark
 * SWSATFE-197: fixed compiler error
 * 
 * Hydra_Software_Devel/67   5/23/12 4:52p ckpark
 * SWSATFE-197: ensure mpeg is locked before declaring stable lock
 * 
 * Hydra_Software_Devel/66   5/4/12 11:04a ckpark
 * SWSATFE-193: revert back to previos cldafectl
 * 
 * Hydra_Software_Devel/65   5/4/12 11:00a ckpark
 * SW7346-807: fixed coverity error
 * 
 * Hydra_Software_Devel/64   3/16/12 4:21p enavarro
 * SWSATFE-140: added reacqCause to channel handle struct
 * 
 * Hydra_Software_Devel/63   3/6/12 3:06p ckpark
 * SWSATFE-179 : Acquisition DAFE loop BW is narrowed for LDPC 8PSK high
 * rate modes
 * 
 * Hydra_Software_Devel/62   3/5/12 11:02a ckpark
 * SWSATFE-178: set non-scan plc value in scan mode as soon as modcod is
 * decoded
 * 
 * Hydra_Software_Devel/61   2/28/12 3:55p enavarro
 * SWSATFE-177: added ACM mode support
 * 
 * Hydra_Software_Devel/60   1/27/12 2:30p enavarro
 * SWSATFE-140: move tracking plc to onStableLock funct
 * 
 * Hydra_Software_Devel/59   1/11/12 4:25p ckpark
 * SWSATFE-171: dont use soft PD table for 8psk no pilot below 8MBaud
 * 
 * Hydra_Software_Devel/58   12/15/11 11:11a ckpark
 * SWSATFE-165: changed afec clock
 * 
 * Hydra_Software_Devel/57   11/15/11 9:59a ckpark
 * SWSATFE-160: Pilot setting is fixed for Pilot Scan mode acquisition
 * 
 * Hydra_Software_Devel/56   10/10/11 3:11p enavarro
 * SWSATFE-151: accumulate bad/corr/total block counters
 * 
 * Hydra_Software_Devel/55   9/29/11 10:42a ckpark
 * SWSATFE-137: updated SNORE alpha value for B0
 * 
 * Hydra_Software_Devel/54   8/24/11 2:52p enavarro
 * SWSATFE-140: moved/renamed BAST_g3_P_LdpcSetMixctl() and
 * BAST_g3_P_LdpcAcquireInit() to bast_g3_priv_acq.c since those
 * functions are also used by turbo acquisition
 * 
 * Hydra_Software_Devel/53   8/12/11 10:43a enavarro
 * SWSATFE-140: add build option to exclude ldpc code from ast pi
 * 
 * Hydra_Software_Devel/52   8/10/11 10:57a enavarro
 * SWSATFE-86: reworked snr filtering
 * 
 * Hydra_Software_Devel/51   8/10/11 9:35a enavarro
 * SWSATFE-75: minimize the time between reading bch counters and
 * resetting them
 * 
 * Hydra_Software_Devel/50   7/21/11 2:24p guangcai
 * SWSATFE-127: LDCP lock criterion is relaxed.
 * 
 * Hydra_Software_Devel/49   7/19/11 11:51a enavarro
 * SW7346-331: fixed unchecked return values
 * 
 * Hydra_Software_Devel/48   7/13/11 9:17a enavarro
 * SWSATFE-133: added dynamic power shut down for AFEC memory
 * 
 * Hydra_Software_Devel/47   7/8/11 11:06a enavarro
 * SWSATFE-127: changes for fine freq estimation
 * 
 * Hydra_Software_Devel/46   7/7/11 4:15p ckpark
 * SWSATFE-127: moved BAST_ENABLE_FINE_FREQ_ESTIMATION definition to
 * bast_g3_priv.h
 * 
 * Hydra_Software_Devel/45   7/7/11 10:38a enavarro
 * SWSATFE-127: reworked fine freq estimation
 * 
 * Hydra_Software_Devel/44   6/27/11 6:10p ronchan
 * SWSATFE-75: fixed compile warning
 * 
 * Hydra_Software_Devel/43   6/23/11 9:28a enavarro
 * SWSATFE-86: disable fine freq estimation for now until it is more
 * thoroughly tested
 * 
 * Hydra_Software_Devel/42   6/21/11 3:44p enavarro
 * SWSATFE-130: set MPLCK same as 4506
 * 
 * Hydra_Software_Devel/41   6/20/11 2:17p enavarro
 * SWSATFE-127: check for FROF3 < 7KHz before enabling fine frequency
 * estimate
 * 
 * Hydra_Software_Devel/40   6/16/11 9:11a enavarro
 * SWSATFE-127: added fine freq offset estimation for DVB-S2
 * 
 * Hydra_Software_Devel/39   5/11/11 2:01p ronchan
 * SWSATFE-75: optimized pilot scan
 * 
 * Hydra_Software_Devel/38   5/11/11 1:52p ronchan
 * SWSATFE-75: added pilot scan
 * 
 * Hydra_Software_Devel/37   5/10/11 3:52p ckpark
 * SWSATFE-118: baud loop tracking BW is widened
 * 
 * Hydra_Software_Devel/36   4/29/11 2:59p enavarro
 * SWSATFE-86: added support for undersample mode
 * 
 * Hydra_Software_Devel/35   4/29/11 11:53a ckpark
 * SWSATFE-75: override modcod for non-ACM
 * 
 * Hydra_Software_Devel/34   4/14/11 3:02p enavarro
 * SWSATFE-75: dont set SEL_BYPASS_CLK in AFEC_GLOBAL_CLK_CNTRL
 * 
 * Hydra_Software_Devel/33   3/31/11 9:36a enavarro
 * SWSATFE-110: reacquire if actualMode is invalid from modcod decode
 * 
 * Hydra_Software_Devel/32   3/7/11 11:37a ckpark
 * SWSATFE-75: set EQ_HDQPSK correctly for LDPC QPSK when LDPC SCAN mode
 * acquisition
 * 
 * Hydra_Software_Devel/31   3/4/11 3:45p enavarro
 * SWSATFE-86: fix compiler error on BCM4528
 * 
 * Hydra_Software_Devel/30   2/25/11 3:52p ckpark
 * SWSATFE-75: make afec clock calculation chip specific
 * 
 * Hydra_Software_Devel/29   1/25/11 8:46a enavarro
 * SWSATFE-75: clean up code for setting afec clock
 * 
 * Hydra_Software_Devel/28   1/24/11 4:46p ckpark
 * SWSATFE-75: removed default afec freq checking
 * 
 * Hydra_Software_Devel/27   12/22/10 4:26p enavarro
 * SWSATFE-75: made changes to get ldpc no pilot to lock
 * 
 * Hydra_Software_Devel/26   12/22/10 11:22a enavarro
 * SWSATFE-75: narrow DAFE loop bw in tracking
 * 
 * Hydra_Software_Devel/25   12/22/10 11:11a enavarro
 * SWSATFE-75: fixed uninitialized variable bug
 * 
 * Hydra_Software_Devel/24   12/20/10 3:56p enavarro
 * SWSATFE-75: updated BAST_g3_P_LdpcConfigCycleCnt() per Neel's
 * spreadsheet
 * 
 * Hydra_Software_Devel/23   12/17/10 10:39a enavarro
 * SWSATFE-75: changed afec clock
 * 
 * Hydra_Software_Devel/22   12/14/10 3:44p enavarro
 * SWSATFE-75: fixed compiler warning
 * 
 * Hydra_Software_Devel/21   12/14/10 3:31p enavarro
 * SWSATFE-75: fixed BAST_g3_P_LdpcUpdateBlockCounters()
 * 
 * Hydra_Software_Devel/20   12/10/10 8:08a enavarro
 * SWSATFE-75: updated AFEC_ACM_MISC_0 due to RDB change from TT40G
 * 
 * Hydra_Software_Devel/19   11/30/10 3:00p enavarro
 * SWSATFE-75: add reacqCtl config parameter
 * 
 * Hydra_Software_Devel/18   11/24/10 4:48p enavarro
 * SWSATFE-75: added BAST_g3_P_LdpcAcquireInit()
 * 
 * Hydra_Software_Devel/17   10/20/10 4:31p enavarro
 * SWSATFE-10: renamed "BAST_g3_P_LdpcAcquire1" to
 * "BAST_g3_P_LdpcOnHpLock"; propagated changes from TT40G
 * 
 * Hydra_Software_Devel/16   10/14/10 5:36p ronchan
 * SWSATFE-75: renamed xport config struct to xportSettings
 * 
 * Hydra_Software_Devel/15   9/30/10 2:03p enavarro
 * SWSATFE-75: store transport settings in BAST_OutputTransportSettings
 * struct in the channel handle; removed xportCtl channel handle member
 * 
 * Hydra_Software_Devel/14   9/29/10 3:51p enavarro
 * SWSATFE-80: removed xport_ctl config parameter
 * 
 * Hydra_Software_Devel/13   9/27/10 5:30p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/12   9/24/10 3:33p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/11   9/22/10 5:18p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/10   9/21/10 3:58p enavarro
 * SWSATFE-75: implemented checkModeFunct for ldpc
 * 
 * Hydra_Software_Devel/9   9/21/10 11:47a enavarro
 * SWSATFE-75: clean up function declaration
 * 
 * Hydra_Software_Devel/8   9/21/10 10:30a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/7   9/20/10 5:26p ronchan
 * SWSATFE-75: fixed compile errors
 * 
 * Hydra_Software_Devel/6   9/20/10 5:09p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/5   9/17/10 5:25p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/4   9/15/10 10:30a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/3   9/14/10 2:29p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/2   9/8/10 10:36a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/1   9/7/10 4:33p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"

#ifndef BAST_EXCLUDE_LDPC

#define BAST_g3_PD_DATA_QPSK_EXTENSION 16

BDBG_MODULE(bast_g3_priv_ldpc);


/******************************************************************************
 BAST_g3_P_IsLdpcPilotPll()
******************************************************************************/
bool BAST_g3_P_IsLdpcPilotPll(BAST_ChannelHandle h)
{
   BSTD_UNUSED(h);
   return false;
}


/******************************************************************************
 BAST_g3_P_IsLdpc8psk()
******************************************************************************/
bool BAST_g3_P_IsLdpc8psk(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_ENABLED) == BAST_LDPC_SCAN_STATE_ENABLED)
   {
      /* ldpc scan in progress */
      if ((hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_QPSK) == 0)
         return true;
   }  
   else if (BAST_MODE_IS_LDPC(hChn->actualMode))
   {
      if (BAST_MODE_IS_LDPC_8PSK(hChn->actualMode))
         return true;
   }
   else if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      return true;
   return false;
}


/******************************************************************************
 BAST_g3_P_LdpcConfigEq()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcConfigEq(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, eqcfad, f0b, ffe_main_tap, i;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_EQ_EQFFECTL, &val);
   val &= 0x0000FF06;
   val |= 0x66060720;
   if ((hChn->acqParams.mode == BAST_Mode_eLdpc_ACM) ||
       (BAST_g3_P_IsLdpcPilotOn(h)))
      val |= 0x18;
   else
      val |= 0x10;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_EQFFECTL, &val);

   BAST_g3_P_ReadRegister(h, BCHP_SDS_EQ_EQMISCCTL, &val);
   val &= ~0x0038401F;
   val |= 0x00140000;
   if ((hChn->acqParams.mode != BAST_Mode_eLdpc_ACM) && BAST_g3_P_IsLdpc8psk(h))
      val |= 0x1A; /* err_mode, sym_mode, dvbs2_8psk_mapping */
   if ((hChn->acqParams.mode == BAST_Mode_eLdpc_ACM) || BAST_g3_P_IsLdpc8psk(h))
      val |= 0x4000;  /* ext_en */
   if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
   {
     /* this is the setting from Michael's script */
     val &= ~0x1F;
     val |= 0x1A;
   }
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_EQMISCCTL, &val);

   BAST_g3_P_ReadRegister(h, BCHP_SDS_EQ_EQFFECTL, &val);
   ffe_main_tap = (val >> 16) & 0x1F; 
   for (i = 0; i < 24; i++)
   {
      val = 0x00;
      if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
      {
         if (i == ffe_main_tap)
            val = 0x20;
      }
      else if (BAST_g3_P_IsLdpc8psk(h))
      {
         if (i == ffe_main_tap)
            val = 0x20;
      }
      else
      {
         if (i == ffe_main_tap)
            val = 0x18;
      }

      eqcfad = 0x40 | i;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_EQCFAD, &eqcfad);

      f0b = (val & 0xFF) << 24;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_F0B, &f0b);
   } 
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcSetVlctl()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcSetVlctl(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   val = 0x00040914;
   if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
      val |= 0x00020008; /* set bit 17 (vlc_soft_insel), set bit 3 (vgain_sel) */
   else if (BAST_g3_P_IsLdpc8psk(h))
      val |= 0x020000; /* set bit 17 (vlc_soft_insel) */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_VLCTL, &val);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcSetHardDecisionLevels()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcSetHardDecisionLevels(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   val = 0x01D901D9;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_HD8PSK1, &val);
   val = 0x00C400C4;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_HD8PSK2, &val);

   if (hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_ENABLED)
   {
      if (hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_FOUND)
      {
         if (hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_QPSK)
            goto ldpc_set_hd_1;
         else
            goto ldpc_set_hd_0;
      }
      else
      {
         /* assume 8PSK in scan mode */
         goto ldpc_set_hd_0;
      }
   }
   else if ((hChn->acqParams.mode == BAST_Mode_eLdpc_ACM) || BAST_g3_P_IsLdpc8psk(h))
   {
      ldpc_set_hd_0:
      val = 0x016A0000;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_HDQPSK, &val);
   }
   else
   {
      /* LDPC QPSK */
      ldpc_set_hd_1:
      val = 0x01000000;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_HDQPSK, &val);
   }  

   return BERR_SUCCESS;   
}


/******************************************************************************
 BAST_g3_P_LdpcSetScramblingSeq()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcSetScramblingSeq(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   val = (hChn->ldpcScramblingSeq[0] << 24) | (hChn->ldpcScramblingSeq[1] << 16) | 
         (hChn->ldpcScramblingSeq[2] << 8) | hChn->ldpcScramblingSeq[3];
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_XSEED, &val);
   val = (hChn->ldpcScramblingSeq[4] << 24) | (hChn->ldpcScramblingSeq[5] << 16) | 
         (hChn->ldpcScramblingSeq[6] << 8) | hChn->ldpcScramblingSeq[7];
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_PLHDRSCR1, &val);
   val = (hChn->ldpcScramblingSeq[8] << 24) | (hChn->ldpcScramblingSeq[9] << 16) | 
         (hChn->ldpcScramblingSeq[10] << 8) | hChn->ldpcScramblingSeq[11];
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_PLHDRSCR2, &val);
   val = (hChn->ldpcScramblingSeq[12] << 24) | (hChn->ldpcScramblingSeq[13] << 16) | 
         (hChn->ldpcScramblingSeq[14] << 8) | hChn->ldpcScramblingSeq[15];
   BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_PLHDRSCR3, &val);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcSetPilotctl()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcSetPilotctl(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   val = 0;
   if ((hChn->acqParams.mode != BAST_Mode_eLdpc_ACM) && BAST_g3_P_IsLdpcPilotOn(h))
      val |= 0x01; /* pilot_mode */
   if (BAST_g3_P_IsLdpcPilotOn(h) || (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM))
      val |= 0x02; /* pilot_update_mode */
   if ((hChn->acqParams.mode != BAST_Mode_eLdpc_ACM) && (BAST_g3_P_IsLdpcPilotOn(h) == false))
      val |= 0x04; /* phase_adj_en */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_PILOTCTL, &val); 
   return BERR_SUCCESS;   
}



/******************************************************************************
 BAST_g3_P_LdpcGetPdTableIdx()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcGetPdTableIdx(uint32_t i, uint32_t j, uint32_t *pIdx)
{
   uint32_t sval1;
   
   /* idx = 30*i - i*(i-1)/2 + j - 1; */
   sval1 = (i * (i - 1)) >> 1;
   *pIdx = 30 * i - sval1 + j - 1;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcGeneratePdTable()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcGeneratePdTable(BAST_ChannelHandle h)
{
   static const uint8_t pd_data_qpsk[] =
   {
      0x00,
      0x2E,
      0x54,
      0x70,
      0x81,
      0x8C,
      0x91,
      0x95,
      0x97,
      0x98,
      0x98,
      0x98,
      0x99,
      0x99,
      0x99,
      0x99,
      0x99,
   };

   static const uint8_t pd_data_8psk[] = 
   {
      0x00, 0x01, 0x06,
      0x00, 0x01, 0x6F,
      0x00, 0x01, 0x93,
      0x00, 0x01, 0xA1,
      0x00, 0x01, 0xA8,
      0x00, 0x01, 0xAB,
      0x00, 0x01, 0xAD,
      0x00, 0x01, 0xAD,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x00, 0x01, 0xAE,
      0x01, 0x55, 0x54,
      0x01, 0x05, 0x83,
      0x00, 0xD9, 0x99,
      0x00, 0xC3, 0xA4,
      0x00, 0xB7, 0xA9,
      0x00, 0xB3, 0xAC,
      0x00, 0xAF, 0xAD,
      0x00, 0xAF, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x00, 0xAD, 0xAE,
      0x01, 0xC7, 0x5C,
      0x01, 0x79, 0x82,
      0x01, 0x4B, 0x98,
      0x01, 0x31, 0xA3,
      0x01, 0x23, 0xA9,
      0x01, 0x1D, 0xAB,
      0x01, 0x1B, 0xAD,
      0x01, 0x19, 0xAD,
      0x01, 0x19, 0xAE,
      0x01, 0x19, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0x17, 0xAE,
      0x01, 0xF1, 0x5D,
      0x01, 0xA7, 0x81,
      0x01, 0x79, 0x96,
      0x01, 0x61, 0xA2,
      0x01, 0x55, 0xA8,
      0x01, 0x4D, 0xAB,
      0x01, 0x4B, 0xAD,
      0x01, 0x49, 0xAD,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0x49, 0xAE,
      0x01, 0xFF, 0x5D,
      0x01, 0xB9, 0x80,
      0x01, 0x8B, 0x96,
      0x01, 0x73, 0xA2,
      0x01, 0x67, 0xA8,
      0x01, 0x61, 0xAB,
      0x01, 0x5D, 0xAD,
      0x01, 0x5D, 0xAD,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x01, 0x5B, 0xAE,
      0x02, 0x05, 0x5D,
      0x01, 0xBF, 0x80,
      0x01, 0x93, 0x96,
      0x01, 0x7B, 0xA2,
      0x01, 0x6D, 0xA8,
      0x01, 0x67, 0xAB,
      0x01, 0x65, 0xAD,
      0x01, 0x63, 0xAD,
      0x01, 0x63, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x01, 0x61, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC1, 0x80,
      0x01, 0x95, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x6F, 0xA8,
      0x01, 0x69, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x01, 0x63, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC1, 0x80,
      0x01, 0x95, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC1, 0x80,
      0x01, 0x95, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC1, 0x80,
      0x01, 0x95, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x01, 0x65, 0xAE,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x01, 0x67, 0xAD,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x01, 0x67, 0xAD,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x01, 0x6B, 0xAB,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x01, 0x71, 0xA8,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x01, 0x7D, 0xA2,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x01, 0x97, 0x96,
      0x02, 0x07, 0x5D,
      0x01, 0xC3, 0x80,
      0x02, 0x07, 0x5D,
   };

   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val, i, j, idx;
   uint8_t isb_byte1, isb_byte2, isb_byte3;
   
#if 0   
   if ((BAST_g3_P_IsLdpcPilotOn(h) == true) && (BAST_g3_P_IsLdpc8psk(h)) && (hChn->acqParams.symbolRate < 8000000))
   {
      /* don't use soft PD table for 8psk no pilot below 8MBaud */
      return BERR_SUCCESS;
   } 
#endif
   
   /* enable PD LUT memory micro access */
   val = 0x80000000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_LUPA, &val);

   for (i = 0; i < 32; i++)
   {
      for (j = 0; j < 32; j++)
      {
         if (BAST_g3_P_IsLdpc8psk(h))
         {
            /* 8psk case */
            if (i < j)
            {
               /* i < j case */
               BAST_CHK_RETCODE(BAST_g3_P_LdpcGetPdTableIdx(i, j, &idx));
               idx *= 3;
               isb_byte3 = pd_data_8psk[idx];
               isb_byte2 = pd_data_8psk[idx + 1];
               isb_byte1 = pd_data_8psk[idx + 2];
            }
            else if (i > j)
            {
               /* i > j case, must transpose value also */
               BAST_CHK_RETCODE(BAST_g3_P_LdpcGetPdTableIdx(j, i, &idx));
               idx *= 3;
               isb_byte3 = ((pd_data_8psk[idx + 1] << 1) | (pd_data_8psk[idx + 2] >> 7)) & 0x3;
               isb_byte2 = (pd_data_8psk[idx + 2] << 1) | (pd_data_8psk[idx] >> 1);
               isb_byte1 = (pd_data_8psk[idx] << 7) | (pd_data_8psk[idx + 1] >> 1);
            }
            else
            {
               isb_byte1 = isb_byte2 = isb_byte3 = 0;
            }
         }
         else
         {
            /* qpsk case */
            isb_byte1 = pd_data_qpsk[(j < BAST_g3_PD_DATA_QPSK_EXTENSION) ? j : BAST_g3_PD_DATA_QPSK_EXTENSION];
            val = pd_data_qpsk[(i < BAST_g3_PD_DATA_QPSK_EXTENSION) ? i : BAST_g3_PD_DATA_QPSK_EXTENSION];
            isb_byte2 = (val << 1);
            isb_byte3 = (val >> 7);
         }
         val = (isb_byte3 << 24) | (isb_byte2 << 16) | (isb_byte1 << 8);
         BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_LUPD, &val);
      }
   }

   /* disable PD LUT memory micro access */
   val = 0x00000000; 
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_LUPA, &val);

   /* deselect CCI phase detect, select soft slicer for carrier loop */ 
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_EQ_EQMISCCTL, ~0x0000040, 0x00000020);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_LdpcConfigSnr()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcConfigSnr(BAST_ChannelHandle h)
{
   static uint32_t LDPC_SNRHT[] =
   {
      0x03c03235, /* LDPC QPSK 1/2 */
      0x02e95977, /* LDPC QPSK 3/5 */
      0x026bf466, /* LDPC QPSK 2/3 */
      0x01ec7290, /* LDPC QPSK 3/4 */
      0x01a3240c, /* LDPC QPSK 4/5 */
      0x01758f45, /* LDPC QPSK 5/6 */
      0x0128ba9e, /* LDPC QPSK 8/9 */
      0x011b5fbc, /* LDPC QPSK 9/10 */
      0x0299deea, /* LDPC 8PSK 3/5 */
      0x0210eb81, /* LDPC 8PSK 2/3 */
      0x01881801, /* LDPC 8PSK 3/4 */
      0x011594c5, /* LDPC 8PSK 5/6 */
      0x00c916fa, /* LDPC 8PSK 8/9 */
      0x00bbaafa  /* LDPC 8PSK 9/10 */
   };

   static uint32_t LDPC_SNRLT[] =
   {
      0x2581f613, /* LDPC QPSK 1/2 */
      0x2581f613, /* LDPC QPSK 3/5 */
      0x18378c00, /* LDPC QPSK 2/3 */
      0x133c79a2, /* LDPC QPSK 3/4 */
      0x105f6879, /* LDPC QPSK 4/5 */
      0x0e9798ae, /* LDPC QPSK 5/6 */
      0x0b974a29, /* LDPC QPSK 8/9 */
      0x0b11bd5a, /* LDPC QPSK 9/10 */
      0x1a02b525, /* LDPC 8PSK 3/5 */
      0x14a9330f, /* LDPC 8PSK 2/3 */
      0x0f50f00e, /* LDPC 8PSK 3/4 */
      0x0ad7cfb3, /* LDPC 8PSK 5/6 */
      0x07dae5c5, /* LDPC 8PSK 8/9 */
      0x0754adc5  /* LDPC 8PSK 9/10 */
   };

   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;         
   BERR_Code retCode;   
   uint32_t val, i;
   
   if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
   {
      /* TBD */
   }
   else
   {
      i = hChn->actualMode - BAST_Mode_eLdpc_Qpsk_1_2;
      val = LDPC_SNRHT[i];
      BAST_g3_P_WriteRegister(h, BCHP_SDS_SNR_SNRHT, &val);
      val = LDPC_SNRLT[i];
      BAST_g3_P_WriteRegister(h, BCHP_SDS_SNR_SNRLT, &val);
   }

   if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
      val = 0x01546732;
   else if (BAST_g3_P_IsLdpc8psk(h))
   {
      if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_PN)
         val = 0x04623751;
      else
         val = 0x01326754;
   }
   else
      val = 0x04576231;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_BERT_BEM1, &val);

   val = 0x01546732;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_BERT_BEM2, &val);

   BAST_CHK_RETCODE(BAST_g3_P_InitBert(h));

   val = 0x0FFF05FF;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_BERT_BEIT, &val);

   val = 0x88;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_SNR_SNRCTL, &val);
   val = 0x0B;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_SNR_SNRCTL, &val);

   if (hDev->sdsRevId >= 0x64)
      val = 0xA3; /* alpha=2^-8 */
   else
      val = 0xA4; /* alpha=2^-5 in A0 */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_SNR_SNORECTL, &val);   
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_LdpcConfigCycleCnt()
  Algorithm:
  S = frame size = 64800 (or 16200 for short frames)
  R = LDPC slow clock rate = 216 MHz
  L = cycle count adjustment = 50
  Fb = symbol rate
  ACM_CYCLE_CNT_0 = ((R*S/2) / Fb) - L
  ACM_CYCLE_CNT_1 = ((R*S/3) / Fb) - L
  ACM_CYCLE_CNT_2 = ((R*S/4) / Fb) - L
  ACM_CYCLE_CNT_3 = ((R*S/5) / Fb) - L
******************************************************************************/
BERR_Code BAST_g3_P_LdpcConfigCycleCnt(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;  
   uint32_t val, P_hi, P_lo, Q_hi, Q_lo;
   
   /* Q_lo = R*S/Fb */
   if (hChn->ldpcCtl & BAST_G3_CONFIG_LDPC_CTL_SHORT_FRAME_DETECTED)
      val = 16200 * 256;
   else
      val = 64800 * 256;
   BAST_MultU32U32(hChn->fecFreq, val, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &Q_lo);

   val = ((Q_lo / 2) - 50*256) >> 8;
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_ACM_CYCLE_CNT_0, &val);
 
   val *= 3; 
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_ACM_MISC_1, &val);

   val = ((Q_lo / 3) - 50*256) >> 8;
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_ACM_CYCLE_CNT_1, &val);

   val = ((Q_lo / 4) - 50*256) >> 8;
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_ACM_CYCLE_CNT_2, &val);

   val = ((Q_lo / 5) - 50*256) >> 8;
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_ACM_CYCLE_CNT_3, &val);   
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcSetMpcfg()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcSetMpcfg(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;  
   uint32_t val;
   
   BAST_g3_P_ReadRegister(h, BCHP_AFEC_BCH_MPCFG, &val);
   val &= 0xFFFFFFCF;
   if ((hChn->xportSettings.bchMpegErrorMode == BAST_BchMpegErrorMode_eBch) ||
       (hChn->xportSettings.bchMpegErrorMode == BAST_BchMpegErrorMode_eBchAndCrc8))
      val |= 0x20;
   if ((hChn->xportSettings.bchMpegErrorMode == BAST_BchMpegErrorMode_eCrc8) ||
       (hChn->xportSettings.bchMpegErrorMode == BAST_BchMpegErrorMode_eBchAndCrc8))
      val |= 0x10;
   if (hChn->xportSettings.bTei)
      val |= 0x08;
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_BCH_MPCFG, &val);

   val = 0x030F0E0F;
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_BCH_MPLCK, &val);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcSetModcod()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcSetModcod(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;  
   uint32_t val, i;
   
#if 0   
   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_ENABLED) == 0)
   {
      val = 0;
      if (hChn->acqParams.mode != BAST_Mode_eLdpc_ACM)
      {
         val |= 0x20;
         i = hChn->actualMode - BAST_Mode_eLdpc_Qpsk_1_2;
         val |= (i + 4);
      }
      BAST_g3_P_WriteRegister(h, BCHP_AFEC_ACM_MODCOD_OVERIDE, &val);
   }
#else
   val = 0;
   if (hChn->acqParams.mode != BAST_Mode_eLdpc_ACM)
   {
      val |= 0x20;
      i = hChn->actualMode - BAST_Mode_eLdpc_Qpsk_1_2;
      val |= (i + 4);
   }
   
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_ACM_MODCOD_OVERIDE, &val);
#endif   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcConfig()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcConfig(BAST_ChannelHandle h)
{
   static const uint32_t script_ldpc_config_0[] =
   {
      BAST_SCRIPT_WRITE(BCHP_AFEC_LDPC_CONFIG_0, 0x05000b02), /* to make lock LDPC easy, was 05000b05 */
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MAX_ITER_OVERIDE, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_1, 0x04c05574),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_2, 0x04c06f84),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_3, 0x04c07cf4),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_4, 0x04c068e5),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_5, 0x04c05f07),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_6, 0x04c05a88),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_7, 0x04c06348),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_8, 0x04c06cd8),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_9, 0x04c071b8),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_10, 0x04c08748),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_11, 0x04c08378),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_12, 0x04c079d8),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_13, 0x04c08748),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_14, 0x04c09b58),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_15, 0x04c0ba18),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_16, 0x04c0be99),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_17, 0x04c0c549),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_18, 0x04c09fd9),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_19, 0x04c0b859),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_20, 0x04c0ca99),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_21, 0x04c0d899),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_22, 0x04c0faf9),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_23, 0x04c101f9),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_24, 0x04c0f659),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_25, 0x04c11199),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_26, 0x04c12679),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_27, 0x04c15a69),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_28, 0x04c16909),
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_ldpc_config_1[] = /* normal frame */
   {
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_SYM_CNT_0, 0x54607E90),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_SYM_CNT_1, 0x3F4832A0),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MISC_0, 0x00010001), /* orig: 0x00110001 */
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_ldpc_config_2[] = /* short frame */
   {
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_SYM_CNT_0, 0x15181FA4),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_SYM_CNT_1, 0x0FD20CA8),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MISC_0, 0x00010009), /* orig: 0x00110009 */
      BAST_SCRIPT_EXIT   
   };
   
   static const uint32_t script_ldpc_config_7[] =
   {
      BAST_SCRIPT_WRITE(BCHP_AFEC_BCH_CTRL, 0x00000001),
      BAST_SCRIPT_WRITE(BCHP_AFEC_BCH_BBHDR4, 0x000005E0),
      BAST_SCRIPT_WRITE(BCHP_AFEC_BCH_BBHDR3, 0x0043000C),
      BAST_SCRIPT_WRITE(BCHP_AFEC_BCH_SMCFG, 0xE000005E),
      BAST_SCRIPT_WRITE(BCHP_AFEC_ACM_MODCOD_STATS_CONFIG, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_AFEC_CNTR_CTRL, 0x00000003), /* clear counters */
      BAST_SCRIPT_EXIT   
   };
   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;   
   uint32_t val;
   
   retCode = BAST_g3_P_LdpcUpdateBlockCounters(h); /* need to do this because counters are about to be reset */
   if (retCode != BERR_SUCCESS)
   {
      BDBG_WRN(("BAST_g3_P_LdpcConfig(): BAST_g3_P_LdpcUpdateBlockCounters() error 0x%X", retCode));
   }
   BAST_g3_P_ToggleBit(h, BCHP_AFEC_RST, 0x00000001);

   /* ldpc_set_dafe_acq_bw() */
   if (hChn->acqParams.mode != BAST_Mode_eLdpc_ACM)
   {
      if (hChn->actualMode <= BAST_Mode_eLdpc_Qpsk_2_3)
         val = 0x0000406A; 
      else if (hChn->actualMode <= BAST_Mode_eLdpc_Qpsk_9_10)
         val = 0x0000506A;
      else if (hChn->actualMode <= BAST_Mode_eLdpc_8psk_3_4)
         val = 0x0000606A;
      else
         val = 0x0000606A; /* was 0x0000708C;  too wide, fell out of lock */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_CL_CLDAFECTL, 0x60000, val);         
   }
   else
   {
      /* TBD: adjust CLDAFECTL for ACM */
   }
   

   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_ldpc_config_0));
   if (hChn->ldpcCtl & BAST_G3_CONFIG_LDPC_CTL_SHORT_FRAME_DETECTED)
   {
      BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_ldpc_config_2));
   }
   else
   {
      BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_ldpc_config_1));
   }
   
   BAST_CHK_RETCODE(BAST_g3_P_LdpcConfigCycleCnt(h));
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_ldpc_config_7));
   BAST_CHK_RETCODE(BAST_g3_P_LdpcSetMpcfg(h));
   BAST_CHK_RETCODE(BAST_g3_P_LdpcSetModcod(h));

   /* clear afec interrupt bits */
   val = 0xFFFFFFFF;
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_INTR_CTRL2_CPU_CLEAR, &val);

   BAST_g3_P_ToggleBit(h, BCHP_AFEC_CNTR_CTRL, 0x00000003); /* clear counters */
   BAST_g3_P_ToggleBit(h, BCHP_AFEC_RST, 0x00000100); /* reset data path */   
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_LdpcSetPsl() 
******************************************************************************/
BERR_Code BAST_g3_P_LdpcSetPsl(BAST_ChannelHandle h)
{
   static const uint32_t AFEC_MODCOD_REGISTER[] =
   {
      BCHP_AFEC_ACM_MODCOD_4, /* LDPC_QPSK_1_2 */
      BCHP_AFEC_ACM_MODCOD_5, /* LDPC_QPSK_3_5 */
      BCHP_AFEC_ACM_MODCOD_6, /* LDPC_QPSK_2_3 */
      BCHP_AFEC_ACM_MODCOD_7, /* LDPC_QPSK_3_4 */
      BCHP_AFEC_ACM_MODCOD_8, /* LDPC_QPSK_4_5 */
      BCHP_AFEC_ACM_MODCOD_9, /* LDPC_QPSK_5_6 */
      BCHP_AFEC_ACM_MODCOD_10, /* LDPC_QPSK_8_9 */
      BCHP_AFEC_ACM_MODCOD_11, /* LDPC_QPSK_9_10 */
      BCHP_AFEC_ACM_MODCOD_12, /* LDPC_8PSK_3_5 */
      BCHP_AFEC_ACM_MODCOD_13, /* LDPC_8PSK_2_3 */
      BCHP_AFEC_ACM_MODCOD_14, /* LDPC_8PSK_3_4 */
      BCHP_AFEC_ACM_MODCOD_15, /* LDPC_8PSK_5_6 */
      BCHP_AFEC_ACM_MODCOD_16, /* LDPC_8PSK_8_9 */
      BCHP_AFEC_ACM_MODCOD_17  /* LDPC_8PSK_9_10 */
   };
   
   static const uint16_t AFEC_CYCLES_ONE_TIME[] =
   {
      1375, /* LDPC_QPSK_1_2 */
      1725, /* LDPC_QPSK_3_5 */
      1353, /* LDPC_QPSK_2_3 */
      1431, /* LDPC_QPSK_3_4 */
      1479, /* LDPC_QPSK_4_5 */
      1515, /* LDPC_QPSK_5_6 */
      1281, /* LDPC_QPSK_8_9 */
      1286, /* LDPC_QPSK_9_10 */
      1725, /* LDPC_8PSK_3_5 */
      1353, /* LDPC_8PSK_2_3 */
      1431, /* LDPC_8PSK_3_4 */
      1515, /* LDPC_8PSK_5_6 */
      1281, /* LDPC_8PSK_8_9 */
      1286, /* LDPC_8PSK_9_10 */
   };
   
   static const uint16_t AFEC_CYCLES_PER_ITER[] =
   {
      1287, /* LDPC_QPSK_1_2 */
      1619, /* LDPC_QPSK_3_5 */
      1235, /* LDPC_QPSK_2_3 */
      1298, /* LDPC_QPSK_3_4 */
      1337, /* LDPC_QPSK_4_5 */
      1367, /* LDPC_QPSK_5_6 */
      1123, /* LDPC_QPSK_8_9 */
      1126, /* LDPC_QPSK_9_10 */
      1619, /* LDPC_8PSK_3_5 */
      1235, /* LDPC_8PSK_2_3 */
      1298, /* LDPC_8PSK_3_4 */
      1367, /* LDPC_8PSK_5_6 */
      1123, /* LDPC_8PSK_8_9 */
      1126, /* LDPC_8PSK_9_10 */
   };
   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, psl_ctl, i, max_iter, thresh, pct, gain, beta;
   
   if ((hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_ENABLED) || 
       (hChn->ldpcCtl & BAST_G3_CONFIG_LDPC_CTL_DISABLE_PSL))
      psl_ctl = 0x50;
   else
   {
      i = hChn->actualMode - BAST_Mode_eLdpc_Qpsk_1_2;

      /* calculate max iterations */
      if (BAST_MODE_IS_LDPC_8PSK(hChn->actualMode))
         BAST_g3_P_ReadRegister(h, BCHP_AFEC_ACM_CYCLE_CNT_1, &val);
      else
         BAST_g3_P_ReadRegister(h, BCHP_AFEC_ACM_CYCLE_CNT_0, &val);
      max_iter = ((val - AFEC_CYCLES_ONE_TIME[i]) / AFEC_CYCLES_PER_ITER[i]);
      if (max_iter > 150)
         max_iter = 150;
        
      BAST_g3_P_ReadRegister(h, AFEC_MODCOD_REGISTER[i], &val);
      val &= 0x000FFFFF;
      val |= (max_iter << 20);
      BAST_g3_P_WriteRegister(h, AFEC_MODCOD_REGISTER[i], &val);

      /* set PSL threshold to 68% max iterations for 8psk 3/5
         set PSL threshold to 56% max iterations for all other code rates */
      if (hChn->actualMode == BAST_Mode_eLdpc_8psk_3_5)
         pct = 68;
      else if (hChn->actualMode == BAST_Mode_eLdpc_8psk_3_4)
         pct = 75;
      else
         pct = 56;
      thresh = (((max_iter * pct * 2) / 100) + 50) >> 1;
      thresh = thresh << 2;

      BAST_g3_P_ReadRegister(h, BCHP_AFEC_LDPC_PSL_FILTER, &val);
      val &= 0xFFFF001F;
      val |= (thresh << 5);
      BAST_g3_P_WriteRegister(h, BCHP_AFEC_LDPC_PSL_FILTER, &val);

      /* set gain and beta */
      if (hChn->acqParams.symbolRate < 5000000)
      {
         gain = 0x0B;
         beta = 0x0A; 
      }
      else
      {
         gain = 0x09; 
         beta = 0x07; 
      }

      psl_ctl = ((beta << 4) & 0xF0) | 0x05;
      psl_ctl |= ((gain & 0x0F) | (((thresh & 0x0003) << 6) & 0xC0)) << 8;
      psl_ctl |= ((thresh >> 2) & 0xFF) << 16;
      psl_ctl |= ((thresh >> 10) & 0x07) << 24;
   }
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_LDPC_PSL_CTL, &psl_ctl);   
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcSetOpll()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcSetOpll(BAST_ChannelHandle h)
{
   static const uint16_t LDPC_NUMBER_OF_BITS[] =
   {
      32128, /* 1/2 */
      38608, /* 3/5 */
      42960, /* 2/3 */
      48328, /* 3/4 */
      51568, /* 4/5 */
      53760, /* 5/6 */
      57392, /* 8/9 */
      58112, /* 9/10 */
   };

   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;  
   uint32_t data0, i, lval1, lval2, number_of_bits, number_of_symbols;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
   BAST_Mode mode;
   
   if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
      mode = hChn->acmMaxMode;
   else
      mode = hChn->actualMode;
   
   if (BAST_MODE_IS_LDPC_8PSK(mode))
   {
      /* 8PSK */
      lval1 = 21600;
      if (hChn->actualMode <= BAST_Mode_eLdpc_8psk_3_4)
         data0 = 1;
      else
         data0 = 2;
      lval2 = 504; /* 14*36 */
      i = mode - BAST_Mode_eLdpc_8psk_3_5 + data0;
   }
   else if (BAST_MODE_IS_LDPC_QPSK(mode))
   {
      /* QPSK */
      lval1 = 32400;
      lval2 = 792; /* 22*36 */
      i = mode - BAST_Mode_eLdpc_Qpsk_1_2;
   }
   else
   {
      BDBG_WRN(("Invalid mode: %08X\n", mode));
      return BERR_INVALID_PARAMETER;  
   }

   number_of_bits = (uint32_t)LDPC_NUMBER_OF_BITS[i];

   if (BAST_g3_P_IsLdpcPilotOn(h) || (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM))
      lval1 += lval2;
   number_of_symbols = lval1 + 90;
 
   /* opll_N = final N */
   hChn->opll_N = (number_of_bits >> 1);

   /* opll_D = final D */
   if (hChn->bUndersample)
      hChn->opll_D = number_of_symbols;
   else
      hChn->opll_D = (number_of_symbols << 1);

   BAST_MultU32U32(hChn->acqParams.symbolRate, number_of_bits * 2, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, number_of_symbols, &Q_hi, &Q_lo);
   hChn->outputBitRate = (Q_lo + 1) >> 1; 

   return BERR_SUCCESS;
}


#if (BCHP_CHIP==4528) || (BCHP_CHIP==4538)
/******************************************************************************
 BAST_g3_P_LdpcEnableDynamicPowerShutDown()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcEnableDynamicPowerShutDown(BAST_ChannelHandle h, bool bEnable)
{   
   uint32_t val;
   
   if (BAST_g3_P_IsLdpcOn(h) == false)
      return BERR_SUCCESS;
   
   if (bEnable)
   {
      BAST_g3_P_OrRegister(h, BCHP_AFEC_ACM_MISC_0, 1<<20);
      BAST_g3_P_OrRegister(h, BCHP_AFEC_LDPC_MEM_POWER, 1);
   }
   else
   {
      BAST_g3_P_AndRegister(h, BCHP_AFEC_LDPC_MEM_POWER, ~1);
      BAST_g3_P_AndRegister(h, BCHP_AFEC_ACM_MISC_0, ~(1<<20));

      BAST_g3_P_ReadRegister(h, BCHP_AFEC_LDPC_STATUS, &val);
      if ((val & (1<<28)) == 0)
      {
         /* power up the memory using correct sequence */
         BAST_g3_P_OrRegister(h, BCHP_AFEC_LDPC_MEM_POWER, 1<<31);
      }      
   }

   return BERR_SUCCESS;
}
#endif


/******************************************************************************
 BAST_g3_P_LdpcOnHpLock() - called when HP locks
******************************************************************************/
BERR_Code BAST_g3_P_LdpcOnHpLock(BAST_ChannelHandle h)
{
   static const uint32_t script_ldpc_5[] =
   {
      BAST_SCRIPT_AND(BCHP_SDS_EQ_EQMISCCTL, 0xFFFFFBFF),           /* disable CMA */
      BAST_SCRIPT_AND_OR(BCHP_SDS_EQ_EQFFECTL, 0xFFFF00FF, 0x0200), /* unfreze other taps */
#ifndef BAST_ENABLE_FINE_FREQ_ESTIMATION         
      BAST_SCRIPT_OR(BCHP_SDS_HP_HPOVERRIDE, 0x0000000F),  /* override front carrier loop */
      BAST_SCRIPT_AND(BCHP_SDS_CL_CLCTL1, 0xFFFFFFEF),         /* disable front carrier loop */
      BAST_SCRIPT_OR(BCHP_SDS_CL_CLCTL2, 0x00000004),          /* freeze front carrier loop */               
#endif            
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_ldpc_6[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_MGAINA, 0x00000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_MGAIND, 0x5a383838),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_MGAIND, 0x384e5a5a),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_MGAIND, 0x5a5a5a5a),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_MGAIND, 0x40404040),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_MGAIND, 0x48480000),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_MGAINA, 0x00000000),
      BAST_SCRIPT_EXIT
   };
   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;   
   uint32_t val;

   BAST_CHK_RETCODE(BAST_g3_P_LdpcGeneratePdTable(h));

   if (hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_ENABLED)
   {
      BAST_CHK_RETCODE(BAST_g3_P_ConfigPlc(h, true)); /* set acquisition plc to the non-scan plc value */ 
      BAST_CHK_RETCODE(BAST_g3_P_LdpcSetVlctl(h));
      BAST_CHK_RETCODE(BAST_g3_P_LdpcConfigEq(h)); 
      
      if (BAST_MODE_IS_LDPC_QPSK(hChn->actualMode))
      {
         /* we need to reprogram HDQPSK because we assumed 8PSK when acquiring in scan mode */
         val = 0x01000000;
         BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_HDQPSK, &val);
      }
   }
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
   else if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_LDPC_PILOT_SCAN)
   {
      BAST_CHK_RETCODE(BAST_g3_P_LdpcConfigEq(h));    
   }
#endif
   BAST_CHK_RETCODE(BAST_g3_P_LdpcSetPilotctl(h)); 
 
   /* this causes constellation to be screwed up when fine freq is enabled */
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_ldpc_5));

   if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
   {
      BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_ldpc_6));
   }
   else
   {
      if (hChn->actualMode >= BAST_Mode_eLdpc_8psk_8_9)
         val = 0x24000000;
      else if (BAST_MODE_IS_LDPC_8PSK(hChn->actualMode))
         val = 0x28000000; 
      else if (hChn->actualMode == BAST_Mode_eLdpc_Qpsk_1_2)
         val = 0x33000000;  
      else if (hChn->actualMode == BAST_Mode_eLdpc_Qpsk_3_5)
         val = 0x3E000000;  
      else
         val = 0x43000000;    /* rest of qpsk */
      BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_VLCI, &val);
      BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_VLCQ, &val);
   }
   
   BAST_CHK_RETCODE(BAST_g3_P_SetAgcTrackingBw(h));

   val = 0x34;
   if ((hChn->acqParams.mode == BAST_Mode_eLdpc_ACM) || BAST_g3_P_IsLdpcPilotOn(h))
      val |= 0x08;
   if ((hChn->acqParams.mode == BAST_Mode_eLdpc_ACM) || BAST_g3_P_IsLdpc8psk(h))
      val |= 0xC0;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLFBCTL, &val);   

   BAST_CHK_RETCODE(BAST_g3_P_LdpcConfigSnr(h));

   BAST_CHK_RETCODE(BAST_g3_P_LdpcConfig(h));  
#if (BCHP_CHIP==4528) || (BCHP_CHIP==4538)
   BAST_CHK_RETCODE(BAST_g3_P_LdpcEnableDynamicPowerShutDown(h, true));
#endif 
   BAST_CHK_RETCODE(BAST_g3_P_LdpcSetPsl(h));

   BAST_g3_P_SdsPowerUpOpll(h);
   BAST_CHK_RETCODE(BAST_g3_P_LdpcSetOpll(h));
   BAST_CHK_RETCODE(BAST_g3_P_ConfigOif(h));

   retCode = BAST_g3_P_StartTracking(h);
  
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_LdpcAcquire()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcAcquire(BAST_ChannelHandle h)
{   
   static const uint32_t script_ldpc_4[] =
   {   
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_XTAP1, 0x00000100),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_XTAP2, 0x00805000),
      BAST_SCRIPT_EXIT
   };
   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;   
   uint32_t val;
   
   if (hChn->bHasAfec == false)
   {
      if (hChn->bBlindScan)
      {
         hChn->reacqCause = BAST_ReacqCause_eNoAfec;
         return BAST_g3_P_Reacquire(h);
      }
      else
         return BAST_g3_P_AbortAcq(h);
   }

   if (hChn->acqParams.mode == BAST_Mode_eLdpc_scan)
      hChn->ldpcScanState = BAST_LDPC_SCAN_STATE_ENABLED;
   else
      hChn->ldpcScanState = 0;
   
   /* software pilot scan */
   if ((hChn->acqCount > 0) && (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_LDPC_PILOT_SCAN))
      hChn->acqParams.acq_ctl ^= BAST_ACQSETTINGS_LDPC_PILOT;
      
#if (BCHP_CHIP==4528) || (BCHP_CHIP==4538)
   BAST_CHK_RETCODE(BAST_g3_P_LdpcEnableDynamicPowerShutDown(h, false));
#endif
      
   BAST_CHK_RETCODE(BAST_g3_P_LdpcPowerUp(h));
   BAST_CHK_RETCODE(BAST_g3_P_NonLegacyModeAcquireInit(h));
   
   BAST_CHK_RETCODE(BAST_g3_P_LdpcConfigEq(h));
   BAST_g3_P_ToggleBit(h, BCHP_SDS_EQ_EQFFECTL, 1); /* reset the eq */   
   BAST_CHK_RETCODE(BAST_g3_P_LdpcSetVlctl(h));
   BAST_CHK_RETCODE(BAST_g3_P_LdpcSetHardDecisionLevels(h));
   BAST_CHK_RETCODE(BAST_g3_P_LdpcSetScramblingSeq(h));
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_ldpc_4));
   
   val = 0x14; 
   if ((BAST_g3_P_IsLdpc8psk(h) == 0) && (hChn->acqParams.mode != BAST_Mode_eLdpc_ACM))
   {
      /* DVB-S2 QPSK */
#ifdef BAST_HAS_WFE
      val |= 0x7E00;
#else
      val |= 0x8000;
#endif
   }
   else
   {
      /* DVB-S2 8PSK */
#ifdef BAST_HAS_WFE
      val |= 0x4F00;
#else
      val |= 0x4000;
#endif
   }
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_PLDCTL, &val);
   
   BAST_CHK_RETCODE(BAST_g3_P_ConfigPlc(h, true)); /* set acquisition plc */ 
   BAST_CHK_RETCODE(BAST_g3_P_LdpcSetPilotctl(h));
 
   /* disable soft pd tables until after HP locks*/
   BAST_g3_P_AndRegister(h, BCHP_SDS_EQ_EQMISCCTL, ~0x00000060);
      
   /* configure and run the HP */
   retCode = BAST_g3_P_HpAcquire(h, BAST_g3_P_LdpcOnHpLock);

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_IsLdpcPilotOn()
******************************************************************************/
bool BAST_g3_P_IsLdpcPilotOn(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   if (hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_ENABLED)
   {
      if (hChn->ldpcScanState & BAST_LDPC_SCAN_STATE_PILOT)
         return true;
   }
   else if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_LDPC_PILOT)
      return true;
   return false;
}


/******************************************************************************
 BAST_g3_P_IsLdpcPilotLoopless()
******************************************************************************/
bool BAST_g3_P_IsLdpcPilotLoopless(BAST_ChannelHandle h)
{
   return BAST_g3_P_IsLdpcPilotOn(h);
}


/******************************************************************************
 BAST_g3_P_LdpcUpdateBlockCounters()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcUpdateBlockCounters(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t corr, bad, total;
   
   if (BAST_g3_P_IsLdpcOn(h))
   {
      BAST_g3_P_ReadRegister(h, BCHP_AFEC_BCH_DECCBLK, &corr);
      BAST_g3_P_ReadRegister(h, BCHP_AFEC_BCH_DECBBLK, &bad);
      BAST_g3_P_ReadRegister(h, BCHP_AFEC_BCH_DECNBLK, &total);   
      BAST_g3_P_ToggleBit(h, BCHP_AFEC_CNTR_CTRL, 0x04); /* clear BCH counters */
      hChn->ldpcBadBlocks += bad;
      hChn->ldpcCorrBlocks += corr;
      hChn->ldpcTotalBlocks += total;      
   }
   return BAST_g3_P_UpdateErrorCounters(h);   
}


/******************************************************************************
 BAST_g3_P_LdpcCheckMode() - ISR context; checks if the current LDPC mode is 
 one of the modes which we are allowed to lock
******************************************************************************/
BERR_Code BAST_g3_P_LdpcCheckMode(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t i; 
   uint16_t mask;

   if (hChn->acqParams.mode == BAST_Mode_eLdpc_ACM)
      goto done;
   
   if (BAST_MODE_IS_LDPC(hChn->actualMode))
   {
      if (hChn->actualMode == BAST_Mode_eLdpc_scan)
         goto invalid_condition;
         
      i = hChn->actualMode - BAST_Mode_eLdpc_Qpsk_1_2;
      mask = (uint16_t)(1 << i);
      if ((mask & hChn->ldpcScanModes) == 0)
      {
         hChn->reacqCause = BAST_ReacqCause_eInvalidMode;
         hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE;
      }
   }
   else
   {
      invalid_condition:
      BDBG_ERR(("BAST_g3_P_LdpcCheckMode() - invalid condition"));
      hChn->reacqCause = BAST_ReacqCause_eInvalidCondition1;
      return BAST_g3_P_Reacquire(h);
   }

   done:
   return retCode;
}


#ifndef BAST_HAS_WFE
/******************************************************************************
 BAST_g3_P_IsLdpcOn()
******************************************************************************/
bool BAST_g3_P_IsLdpcOn(BAST_ChannelHandle h)
{
   uint32_t val;
   
   BAST_g3_P_ReadRegister(h, BCHP_AFEC_GLOBAL_CLK_CNTRL, &val);
   if (val & 0x00800000)
      return false;
   return true;
}


/******************************************************************************
 BAST_g3_P_LdpcPowerUp() - power up the afec core
******************************************************************************/ 
BERR_Code BAST_g3_P_LdpcPowerUp(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   uint32_t val;
   
   BAST_g3_P_AndRegister(h, BCHP_AFEC_GLOBAL_CLK_CNTRL, ~0x00800000);
   
#if (BCHP_CHIP==7346)
   val = 0x00080000 | 4; /* afec mdiv=4 */
#else
   val = 0x00080000 | 6; /* afec mdiv=5 */
#endif   
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_GLOBAL_CLK_CNTRL, &val);
   val |= 0x00008000;  /* load a new mdiv value */
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_GLOBAL_CLK_CNTRL, &val);   
   val &= 0xFF00FFFF; /* enable output and disable new mdiv load */
   BAST_g3_P_WriteRegister(h, BCHP_AFEC_GLOBAL_CLK_CNTRL, &val); 
   
   if ((retCode = BAST_g3_P_GetAfecClock(h, &(hChn->fecFreq))) != BERR_SUCCESS)
   {
      BERR_TRACE(retCode = BAST_ERR_HAB_CMD_FAIL);
      BDBG_ERR(("BAST_g3_P_LdpcPowerUp() failed"));
   } 
   return retCode;
}


/******************************************************************************
 BAST_g3_P_LdpcPowerDown() - power down the afec core
******************************************************************************/ 
BERR_Code BAST_g3_P_LdpcPowerDown(BAST_ChannelHandle h)
{
   BERR_Code retCode;   

#if (BCHP_CHIP==4528) || (BCHP_CHIP==4538)
   BAST_g3_P_LdpcEnableDynamicPowerShutDown(h, false);
#endif   

   BAST_g3_P_OrRegister(h, BCHP_AFEC_GLOBAL_CLK_CNTRL, 0x00800000);
   if (BAST_g3_P_IsLdpcOn(h) == false)
      retCode = BERR_SUCCESS;
   else
   {
      BERR_TRACE(retCode = BAST_ERR_HAB_CMD_FAIL);
      BDBG_ERR(("BAST_g3_P_LdpcPowerDown() failed"));
   }
   return retCode;
}
#endif


/******************************************************************************
 BAST_g3_P_LdpcEnableLockInterrupts()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcEnableLockInterrupts(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BINT_EnableCallback_isr(hChn->hLdpcLockCb);
   BINT_EnableCallback_isr(hChn->hLdpcNotLockCb);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcDisableLockInterrupts()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcDisableLockInterrupts(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BINT_DisableCallback_isr(hChn->hLdpcLockCb);
   BINT_DisableCallback_isr(hChn->hLdpcNotLockCb);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcOnLock() - This function is called when lock status transitions 
                          from not_locked to locked.
******************************************************************************/
BERR_Code BAST_g3_P_LdpcOnLock(BAST_ChannelHandle h)
{
   BSTD_UNUSED(h);
      
   /* do nothing */
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcOnLostLock() - This function is called when lock status 
                              transitions from locked to not_locked.
******************************************************************************/
BERR_Code BAST_g3_P_LdpcOnLostLock(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   
   BAST_CHK_RETCODE(BAST_g3_P_LdpcUpdateBlockCounters(h));
      
   if (BAST_g3_P_IsHpLocked(h) == false)
   {
      hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE;
      hChn->reacqCause = BAST_ReacqCause_eHpLostLock3;
      goto done;
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_LdpcIsMpegLocked()
******************************************************************************/
BERR_Code BAST_g3_P_LdpcIsMpegLocked(BAST_ChannelHandle h, bool *bLocked)
{
   uint32_t val;
   
   BAST_g3_P_ReadRegister(h, BCHP_AFEC_LDPC_STATUS, &val);
   if ((val & 0xC0000000) != 0xC0000000)
      *bLocked = false;
   else
      *bLocked = true;
   return BERR_SUCCESS;   
}


/******************************************************************************
 BAST_g3_P_LdpcOnStableLock() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_LdpcOnStableLock(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
#ifndef BAST_ENABLE_FINE_FREQ_ESTIMATION   
   uint32_t val;
#endif   
   bool bMpLocked;

   BAST_g3_P_LdpcIsMpegLocked(h, &bMpLocked);
   if (!bMpLocked)
   {
#if 0
      /* MP is not locked, so force reacquire */
      BDBG_MSG(("BAST_g3_P_LdpcOnStableLock: mpeg not locked, reacquiring..."));
      hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE;
      return BERR_SUCCESS;
#else
      hChn->count1 = 1;
#endif      
   }
   
   if (hChn->bPlcTracking == false)
   {
      BAST_g3_P_ConfigPlc(h, false); /* set tracking PLC */   

#ifdef BAST_FROF2_WORKAROUND
      BAST_g3_P_OrRegister(h, BCHP_SDS_CL_CLCTL2, 0x00000004);   /* freeze front carrier loop */    
#endif      
   }
#ifndef BAST_ENABLE_FINE_FREQ_ESTIMATION
      /* narrow DAFE loop bw */
      if (BAST_g3_P_IsLdpcPilotOn(h)) /* all pilot on modes, except LDPC 8PSK 3/4 */
      {
         if (hChn->actualMode != BAST_Mode_eLdpc_8psk_3_4)
         {
            BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_HPCONTROL, &val);
            val |= 0x04; /* Use Pilot for FROF3 estimation to avoid error at the edge of freq. drift */
            BAST_g3_P_WriteRegister(h, BCHP_SDS_HP_HPCONTROL, &val);
            val = 0x00005044;
         }
         else
            val = 0x00005056;  /* 8psk 3/4 pilot  on, do not use pilot for FROF3 est. */
      }
      else
         val = 0x00005056;
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_CL_CLDAFECTL, 0x60000, val);   
#endif
   
   /* set tracking baud loop bw */
   BAST_CHK_RETCODE(BAST_g3_P_SetBaudBw(h, hChn->acqParams.symbolRate / 400, 4));
   
   if (hChn->bEverStableLock == false)
   {
      BAST_CHK_RETCODE(BAST_g3_P_LdpcUpdateBlockCounters(h));
      hChn->ldpcBadBlocks = 0;
      hChn->ldpcCorrBlocks = 0;
      hChn->ldpcTotalBlocks = 0;
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_LdpcMonitorLock()
******************************************************************************/ 
BERR_Code BAST_g3_P_LdpcMonitorLock(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;   
   bool bMpLocked;

   BAST_g3_P_LdpcIsMpegLocked(h, &bMpLocked);
   if (!bMpLocked)
   {
      hChn->count1++;
      if (hChn->count1 > 2)
      {
      /* MP is not locked, so force reacquire */
         BDBG_MSG(("BAST_g3_P_LdpcMonitorLock: mpeg not locked, reacquiring..."));
      hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE;
   }
   }
   else
      hChn->count1 = 0;
    return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_LdpcSetFunctTable()
******************************************************************************/ 
BERR_Code BAST_g3_P_LdpcSetFunctTable(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   hChn->acqFunct = BAST_g3_P_LdpcAcquire;
   hChn->checkModeFunct = BAST_g3_P_LdpcCheckMode;
   hChn->onLockFunct = BAST_g3_P_LdpcOnLock;
   hChn->onLostLockFunct = BAST_g3_P_LdpcOnLostLock;
   hChn->onStableLockFunct = BAST_g3_P_LdpcOnStableLock;
   hChn->onMonitorLockFunct = BAST_g3_P_LdpcMonitorLock;
   hChn->enableLockInterrupts = BAST_g3_P_LdpcEnableLockInterrupts;
   hChn->disableLockInterrupts = BAST_g3_P_LdpcDisableLockInterrupts;
   hChn->getLockStatusFunct = NULL;
   return BERR_SUCCESS;
}

#endif

