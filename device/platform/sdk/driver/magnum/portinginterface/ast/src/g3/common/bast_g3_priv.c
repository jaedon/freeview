/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/122 $
 * $brcm_Date: 12/13/12 4:31p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv.c $
 * 
 * Hydra_Software_Devel/122   12/13/12 4:31p enavarro
 * SWSATFE-174: added postInitApFunct
 * 
 * Hydra_Software_Devel/121   12/13/12 3:03p enavarro
 * SWSATFE-174: In BAST_g3_P_GetLockStatus(), added requirement to be in
 * monitor state for declaring lock
 * 
 * Hydra_Software_Devel/120   12/10/12 4:27p enavarro
 * SWSATFE-233: set xbert only if bert is enabled and not dtv mode
 * 
 * Hydra_Software_Devel/119   11/28/12 2:03p ronchan
 * SWSATFE-174: consolidate asserts to api mapping function
 * 
 * Hydra_Software_Devel/118   11/12/12 11:12a ronchan
 * SWSATFE-174: flag cores powered down after aborting acquisition
 * 
 * Hydra_Software_Devel/117   11/6/12 11:19a ronchan
 * SWSATFE-174: bypass abort acquire if powered down
 * 
 * Hydra_Software_Devel/116   11/2/12 10:58a enavarro
 * SWSATFE-174: added check for NULL handle
 * 
 * Hydra_Software_Devel/115   10/25/12 2:25p enavarro
 * SWSATFE-174: implement BAST_PowerDown/BAST_PowerUp for BAST_HAS_WFE
 * 
 * Hydra_Software_Devel/114   10/22/12 5:09p enavarro
 * SWSATFE-174: Read/Write AGC not supported when BAST_HAS_WFE is defined
 * 
 * Hydra_Software_Devel/113   10/22/12 4:31p enavarro
 * SWSATFE-174: removed unused functions/variables when BAST_HAS_WFE is
 * defined
 * 
 * Hydra_Software_Devel/111   10/19/12 11:03a ronchan
 * SW3461-274: changes to bfec structure
 * 
 * Hydra_Software_Devel/110   10/15/12 4:16p enavarro
 * SWSATFE-220: renamed BAST_Gx_BUILD_VERSION to BAST_G3_BUILD_VERSION
 * 
 * Hydra_Software_Devel/109   10/15/12 10:02a enavarro
 * SWSATFE-220: add implementation for BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/108   9/11/12 11:12a enavarro
 * SWSATFE-174: added BAST_EXCLUDE_EXT_TUNER build option
 * 
 * Hydra_Software_Devel/107   8/30/12 2:18p ronchan
 * SWSATFE-207: added config parameters for tuner agc window length,
 * amplitude threshold, and loop coefficient
 * 
 * Hydra_Software_Devel/106   8/29/12 4:07p ronchan
 * SW7346-1005: change Mi2cDisableInterrupts to isr context
 * 
 * Hydra_Software_Devel/104   8/14/12 10:08a ronchan
 * SWSATFE-213: exit critical section if error condition occurs
 * 
 * Hydra_Software_Devel/103   8/13/12 10:20a ronchan
 * SWSATFE-213: exit critical section if error condition occurs
 * 
 * Hydra_Software_Devel/102   7/19/12 2:17p ronchan
 * SWSATFE-207: defer daisy gain programming to acquisition
 * 
 * Hydra_Software_Devel/101   7/18/12 3:23p ronchan
 * SWSATFE-207: added daisy gain config param, renamed agc thresh config
 * param
 * 
 * Hydra_Software_Devel/100   7/13/12 10:14a enavarro
 * SWSATFE-140: added sds rev id to bast handle
 * 
 * Hydra_Software_Devel/99   7/5/12 11:41a ronchan
 * SWSATFE-205: enter InitChannels in isr context
 * 
 * Hydra_Software_Devel/98   6/27/12 3:09p ckpark
 * SWSATFE-204: call BAST_g3_P_LdpcUpdateBlockCounters() within a critical
 * section
 * 
 * Hydra_Software_Devel/97   6/26/12 4:32p enavarro
 * SWSATFE-201: put critical section around call to
 * BAST_g3_P_TurboUpdateErrorCounters()
 * 
 * Hydra_Software_Devel/96   5/14/12 6:17p ronchan
 * SWSATFE-194: reduce tuner initialization timeout to 200ms
 * 
 * Hydra_Software_Devel/95   5/10/12 7:04p ronchan
 * SWSATFE-194: wait for tuner init done after power up sequence
 * 
 * Hydra_Software_Devel/94   4/25/12 10:12a ronchan
 * SWSATFE-75: disable output interface during abort acquisition
 * 
 * Hydra_Software_Devel/93   4/20/12 2:50p ronchan
 * SWSATFE-75: create diseqc almost empty callback
 * 
 * Hydra_Software_Devel/92   4/16/12 4:08p ckpark
 * SWSATFE-140: automatically enable xbert in xportSettings when bert is
 * enabled
 * 
 * Hydra_Software_Devel/91   3/28/12 2:39p enavarro
 * SWSATFE-140: fixed compiler error for 4528
 * 
 * Hydra_Software_Devel/90   3/22/12 2:05p enavarro
 * SWSATFE-184: added tuner_rfagc_thresh config parameter
 * 
 * Hydra_Software_Devel/89   3/16/12 4:20p enavarro
 * SWSATFE-140: added reacqCause to channel handle struct
 * 
 * Hydra_Software_Devel/88   3/5/12 3:01p enavarro
 * SWSATFE-177: fixed compiler warning
 * 
 * Hydra_Software_Devel/87   2/28/12 3:51p enavarro
 * SWSATFE-177: added ACM_MAX_MODE config parameter
 * 
 * Hydra_Software_Devel/86   2/9/12 3:12p enavarro
 * SWSATFE-140: reworked acquisition timer
 * 
 * Hydra_Software_Devel/85   2/1/12 4:53p enavarro
 * SWSATFE-140: checkout BAST_g3_P_SoftReset() on 4517
 * 
 * Hydra_Software_Devel/84   2/1/12 3:27p enavarro
 * SWSATFE-140: added input parameter to BAST_g3_P_InitChannels()
 * 
 * Hydra_Software_Devel/83   1/17/12 11:00a enavarro
 * SWSATFE-86: fixed compiler warnings for turbo-disabled chips
 * 
 * Hydra_Software_Devel/82   1/3/12 10:06a ckpark
 * SWSATFE-162: added debug1 and debug2 config params
 * 
 * Hydra_Software_Devel/81   12/27/11 11:08a enavarro
 * SWSATFE-166: reworked BAST_g3_P_GetChannelStatus() to use
 * BAST_g3_P_HpIsSpinv()
 * 
 * Hydra_Software_Devel/80   12/15/11 4:35p ckpark
 * SWSATFE-166: fixed spinv status for turbo
 * 
 * Hydra_Software_Devel/79   10/13/11 1:57p enavarro
 * SWSATFE-150: changed BAST_SpurCancellerConfig struct
 * 
 * Hydra_Software_Devel/78   10/7/11 4:45p enavarro
 * SWSATFE-150: clear spur canceller data if no config data is given in
 * BAST_g3_P_EnableSpurCanceller()
 * 
 * Hydra_Software_Devel/77   10/6/11 9:17a enavarro
 * SWSATFE-150: reworked spur canceller to use CWC instead of notch
 * filters
 * 
 * Hydra_Software_Devel/76   10/3/11 9:16a enavarro
 * SWSATFE-75: fixed bug when setting PLC_CTL config parameter
 * 
 * Hydra_Software_Devel/75   9/16/11 9:56a ronchan
 * SWSATFE-139: add config parameter for ambient temperature
 * 
 * Hydra_Software_Devel/74   9/15/11 2:49p ronchan
 * SWSATFE-75: fix compile warning
 * 
 * Hydra_Software_Devel/73   9/15/11 2:46p ronchan
 * SWSATFE-75: don't check for bInit for power up and power down
 * 
 * Hydra_Software_Devel/72   9/14/11 10:41a ronchan
 * SWSATFE-139: fix compile warning
 * 
 * Hydra_Software_Devel/71   9/13/11 5:40p ronchan
 * SWSATFE-139: initialize tuner on power up
 * 
 * Hydra_Software_Devel/70   8/24/11 10:54a enavarro
 * SWSATFE-140: fixed compiler errors when BAST_EXCLUDE_LDPC is defined
 * 
 * Hydra_Software_Devel/69   8/24/11 10:32a ronchan
 * SWSATFE-139: initialize chip handle on open
 * 
 * Hydra_Software_Devel/68   8/18/11 5:13p enavarro
 * SWSATFE-140: add build option to exclude ftm
 * 
 * Hydra_Software_Devel/67   8/16/11 2:05p ronchan
 * SWSATFE-86: adjusted freq bounds for wide band tuner
 * 
 * Hydra_Software_Devel/66   8/12/11 10:43a enavarro
 * SWSATFE-140: add build option to exclude ldpc code from ast pi
 * 
 * Hydra_Software_Devel/65   8/11/11 2:00p enavarro
 * SWSATFE-86: add build options to exclude bcm3445 and mi2c support
 * 
 * Hydra_Software_Devel/64   8/10/11 10:57a enavarro
 * SWSATFE-86: reworked snr filtering
 * 
 * Hydra_Software_Devel/63   8/8/11 10:24a enavarro
 * SW7346-405: fixed coverity warning due to missing "break" in switch
 * statement
 * 
 * Hydra_Software_Devel/62   8/2/11 4:19p ronchan
 * SWSATFE-75: increased lower bound of tuning frequency
 * 
 * Hydra_Software_Devel/61   7/29/11 11:11a guangcai
 * SWSATFE-137: implemented SNORE filtering; enabled SNORE for all g3
 * chips
 * 
 * Hydra_Software_Devel/60   7/15/11 10:00a enavarro
 * SWSATFE-86: added bOpen in BAST_g3_P_Handle struct
 * 
 * Hydra_Software_Devel/59   6/16/11 11:32a enavarro
 * SWSATFE-127: destroy hHpFrameBoundaryCb
 * 
 * Hydra_Software_Devel/58   6/16/11 9:10a enavarro
 * SWSATFE-127: added fine freq offset estimation for DVB-S2
 * 
 * Hydra_Software_Devel/57   5/26/11 4:12p enavarro
 * SWSATFE-86: updated debug message
 * 
 * Hydra_Software_Devel/56   5/25/11 6:14p enavarro
 * SWSATFE-86: added debug message
 * 
 * Hydra_Software_Devel/55   4/13/11 10:46a guangcai
 * SWSATFE-113: input power estimation is added
 * 
 * Hydra_Software_Devel/55   4/12/11 10:11a guangcai
 * SWSATFE-75: changes for input power estimation
 * 
 * Hydra_Software_Devel/54   4/5/11 10:55a ckpark
 * SWSATFE-102: normalized tuner calibration threshold is introduced
 * 
 * Hydra_Software_Devel/53   3/9/11 10:51a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/52   3/7/11 8:33a enavarro
 * SWSATFE-86: fixed compiler errors for BAST_EXCLUDE_SPUR_CANCELLER
 * 
 * Hydra_Software_Devel/51   3/4/11 8:11a enavarro
 * SWSATFE-75: add support for BCM4528
 * 
 * Hydra_Software_Devel/50   3/3/11 11:46a enavarro
 * SWSATFE-75: fixed compiler errors when BAST_EXCLUDE_TURBO is defined
 * 
 * Hydra_Software_Devel/49   3/3/11 10:52a enavarro
 * SWSATFE-75: put all diseqc state variables in BAST_g3_P_DiseqcDevice
 * struct and allocate this only if the channel has diseqc capability
 * 
 * Hydra_Software_Devel/48   2/14/11 4:37p ronchan
 * SWSATFE-75: limit ftm tx power configuration parameter
 * 
 * Hydra_Software_Devel/47   2/9/11 3:18p enavarro
 * SWSATFE-75: fixed bug that incorrectly cleared reacqCtl
 * 
 * Hydra_Software_Devel/46   1/27/11 3:43p enavarro
 * SW7346-21: removed peak_scan_sym_rate_min, peak_scan_sym_rate_max, and
 * dft_size config parameters; added
 * BAST_g3_P_SetPeakScanSymbolRateRange(); added
 * BAST_g3_P_GetPeakScanSymbolRateRange()
 * 
 * Hydra_Software_Devel/45   1/24/11 11:49a enavarro
 * SWSATFE-75: added MISC_CTL config param
 * 
 * Hydra_Software_Devel/44   1/24/11 10:14a enavarro
 * SWSATFE-75: fixed acquisition timing bug
 * 
 * Hydra_Software_Devel/43   1/19/11 3:20p ronchan
 * SWSATFE-99: added chip-independent fsk channel config api
 * 
 * Hydra_Software_Devel/42   1/11/11 3:25p ronchan
 * SWSATFE-75: add bcm3445_ctl config parameter
 * 
 * Hydra_Software_Devel/41   1/5/11 5:21p ronchan
 * SWSATFE-75: increased upper bound of tuning frequency
 * 
 * Hydra_Software_Devel/40   12/29/10 12:10p enavarro
 * SWSATFE-75: removed peakScanPower from channel handle struct; return
 * peakScanMaxPower as peakPower in BAST_GetPeakScanStatus()
 * 
 * Hydra_Software_Devel/39   12/28/10 4:06p enavarro
 * SWSATFE-75: fixed BAST_g3_P_GetChannelStatus()
 * 
 * Hydra_Software_Devel/38   12/27/10 8:52a enavarro
 * SWSATFE-75: make rain fade threshold parameter 1 byte
 * 
 * Hydra_Software_Devel/37   12/22/10 11:15a enavarro
 * SWSATFE-75: disable turbo sync interrupt in
 * BAST_g3_P_DisableChannelInterrupts_isr()
 * 
 * Hydra_Software_Devel/36   12/9/10 4:57p enavarro
 * SWSATFE-75: removed test/debug function
 * 
 * Hydra_Software_Devel/35   12/9/10 9:01a enavarro
 * SWSATFE-75: fixed naming conflict
 * 
 * Hydra_Software_Devel/34   12/8/10 2:16p enavarro
 * SWSATFE-75: added turbo sync detect interrupt
 * 
 * Hydra_Software_Devel/33   11/30/10 2:57p enavarro
 * SWSATFE-75: added reacq_ctl config parameter
 * 
 * Hydra_Software_Devel/32   11/29/10 3:22p enavarro
 * SWSATFE-80: added BAST_GetNetworkSpec() and BAST_SetNetworkSpec() to
 * the API
 * 
 * Hydra_Software_Devel/31   11/22/10 3:15p enavarro
 * SWSATFE-75: renamed hHpCb to hHpLockCb; renamed BAST_g3_P_Hp_isr() to
 * BAST_g3_HPLock_isr()
 * 
 * Hydra_Software_Devel/30   10/19/10 11:11a enavarro
 * SWSATFE-75: implemented BAST_g3_P_GetPeakScanStatus()
 * 
 * Hydra_Software_Devel/29   10/18/10 2:53p enavarro
 * SWSATFE-75: added peak scan support
 * 
 * Hydra_Software_Devel/28   10/18/10 10:07a enavarro
 * SWSATFE-75: updated self test
 * 
 * Hydra_Software_Devel/27   10/14/10 5:42p ronchan
 * SWSATFE-75: moved diseqc config parameters to diseqcSettings
 * 
 * Hydra_Software_Devel/26   10/13/10 11:44a ronchan
 * SWSATFE-75: fixed device handles
 * 
 * Hydra_Software_Devel/25   10/13/10 11:29a enavarro
 * SWSATFE-75: updated self test
 * 
 * Hydra_Software_Devel/24   10/13/10 10:41a enavarro
 * SWSATFE-75: fixed bug in CloseChannel()
 * 
 * Hydra_Software_Devel/23   10/12/10 4:37p enavarro
 * SWSATFE-75: in BAST_g3_P_CloseChannel(), abort acq only if PI
 * previously initialized successfully; added temporary self test code
 * for emulation
 * 
 * Hydra_Software_Devel/22   10/12/10 10:43a enavarro
 * SWSATFE-75: fixed compiler warning
 * 
 * Hydra_Software_Devel/21   10/12/10 9:55a enavarro
 * SWSATFE-75: dont check for bInit in BAST_g3_P_AbortAcq()
 * 
 * Hydra_Software_Devel/20   10/11/10 4:35p enavarro
 * SWSATFE-75: added BAST_g3_GetTraceBuffer()
 * 
 * Hydra_Software_Devel/19   10/11/10 11:34a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/18   10/4/10 2:43p enavarro
 * SWSATFE-75: clean up BAST_g3_P_DisableChannelInterrupts_isr(); clear
 * init done event before calling BAST_g3_P_InitChannels(); added bInit
 * checking in public API functions; implemented BAST_g3_P_PowerDown()
 * 
 * Hydra_Software_Devel/17   9/30/10 3:17p ronchan
 * SWSATFE-80: added API functions to set/get diseqc settings
 * 
 * Hydra_Software_Devel/16   9/30/10 2:02p enavarro
 * SWSATFE-75: store transport settings in BAST_OutputTransportSettings
 * struct in the channel handle; removed xportCtl channel handle member
 * 
 * Hydra_Software_Devel/15   9/29/10 3:49p enavarro
 * SWSATFE-80: added API functions to set/get output transport settings;
 * removed xport_ctl config parameter
 * 
 * Hydra_Software_Devel/14   9/29/10 10:11a enavarro
 * SWSATFE-75: added BAST_GetSignalDetectStatus() to BAST API
 * 
 * Hydra_Software_Devel/13   9/23/10 6:19p ronchan
 * SWSATFE-75: added AGC_CTL config parameter
 * 
 * Hydra_Software_Devel/12   9/22/10 5:18p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/11   9/21/10 1:31p enavarro
 * SWSATFE-75: implemented BAST_g3_P_EnableSpurCanceller()
 * 
 * Hydra_Software_Devel/10   9/21/10 11:47a enavarro
 * SWSATFE-75: clean up function declaration
 * 
 * Hydra_Software_Devel/9   9/21/10 10:27a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/8   9/20/10 5:09p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/7   9/17/10 5:22p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/6   9/15/10 10:30a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/5   9/14/10 2:29p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/4   9/8/10 2:39p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/3   9/8/10 10:36a enavarro
 * SWSATFE-75: added plc_ctl config parameter
 * 
 * Hydra_Software_Devel/2   9/1/10 5:03p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/1   8/31/10 1:55p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"

BDBG_MODULE(bast_g3_priv);


/******************************************************************************
 BAST_g3_P_DisableChannelInterrupts_isr() - ISR context
******************************************************************************/ 
BERR_Code BAST_g3_P_DisableChannelInterrupts_isr(
   BAST_ChannelHandle h,   /* [in] BAST handle */
   bool bDisableDiseqc,    /* [in] true = disable diseqc interrupts also */
   bool bDisableMi2c       /* [in] true = disable mi2c interrupts also */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

#ifdef BAST_EXCLUDE_MI2C
   BSTD_UNUSED(bDisableMi2c);
#endif

   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eBaud);
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eBer);
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eGen1);
   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eGen2); 

   if (hChn->gen2TimerIsr != NULL)
   {
      BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eGen3);
   }
     
   BAST_CHK_RETCODE(BAST_g3_P_QpskDisableLockInterrupts(h));
   
   if (hChn->bHasAfec || hChn->bHasTfec)
   {
      BAST_CHK_RETCODE(BAST_g3_P_HpDisableInterrupts(h));
   }

#ifndef BAST_EXCLUDE_LDPC      
   if ((hChn->bHasAfec) && BAST_g3_P_IsLdpcOn(h))
   {
      BAST_CHK_RETCODE(BAST_g3_P_LdpcDisableLockInterrupts(h));
   }
#endif   
   
#ifndef BAST_EXCLUDE_TURBO
   if (hChn->bHasTfec && BAST_g3_P_IsTurboOn(h))
   {
      BAST_CHK_RETCODE(BAST_g3_P_TurboDisableLockInterrupts(h));
      BAST_CHK_RETCODE(BAST_g3_P_TurboDisableSyncInterrupt(h));
   }
#endif

   if (hChn->bHasDiseqc && bDisableDiseqc)  
      BAST_g3_P_DisableDiseqcInterrupts_isr(h);

#ifndef BAST_EXCLUDE_MI2C   
   if (bDisableMi2c)
   {
      BAST_CHK_RETCODE(BAST_g3_P_Mi2cDisableInterrupts_isr(h));
   }
#endif   
   
   done:
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_DisableChannelInterrupts() - Non-ISR context
******************************************************************************/ 
BERR_Code BAST_g3_P_DisableChannelInterrupts(
   BAST_ChannelHandle h,   /* [in] BAST handle */
   bool bDisableDiseqc,    /* [in] true = disable diseqc interrupts also */
   bool bDisableMi2c       /* [in] true = disable mi2c interrupts also */
)
{
   BERR_Code retCode;
   
   BKNI_EnterCriticalSection();
   retCode = BAST_g3_P_DisableChannelInterrupts_isr(h, bDisableDiseqc, bDisableMi2c);
   BKNI_LeaveCriticalSection();
   
   return retCode;
}


/******************************************************************************
 BAST_g3_P_Open()
******************************************************************************/
BERR_Code BAST_g3_P_Open(
   BAST_Handle *h,                   /* [out] BAST handle */
   BCHP_Handle hChip,                /* [in] chip handle */
   void        *pReg,                /* [in] pointer to register or i2c handle */
   BINT_Handle hInterrupt,           /* [in] Interrupt handle */
   const BAST_Settings *pDefSettings /* [in] default settings */
)
{
#ifndef BAST_EXCLUDE_FTM            
   extern BAST_g3_Ftm_P_InterruptCbTable BAST_Ftm_Interrupts[];
#endif

   BERR_Code retCode;
   BAST_Handle hDev;
   BAST_g3_P_Handle *hG3Dev;
   uint8_t i;
   
   BDBG_ENTER(BAST_g3_P_Open);

   /* allocate memory for the handle */
   hDev = (BAST_Handle)BKNI_Malloc(sizeof(BAST_P_Handle));
   BDBG_ASSERT(hDev);
   BKNI_Memset((void*)hDev, 0, sizeof(BAST_P_Handle));
   hG3Dev = (BAST_g3_P_Handle *)BKNI_Malloc(sizeof(BAST_g3_P_Handle));
   BDBG_ASSERT(hG3Dev);
   BKNI_Memset((void*)hG3Dev, 0, sizeof(BAST_g3_P_Handle));
   hDev->pImpl = (void*)hG3Dev;

   hDev->pChannels = (BAST_P_ChannelHandle **)BKNI_Malloc(BAST_G3_MAX_CHANNELS * sizeof(BAST_P_ChannelHandle *));
   BDBG_ASSERT(hDev->pChannels);
   BKNI_Memset((void*)hDev->pChannels, 0, sizeof(BAST_G3_MAX_CHANNELS * sizeof(BAST_P_ChannelHandle *)));

   /* initialize our handle */
   hG3Dev->hRegister = (BREG_Handle)pReg;
   hG3Dev->hInterrupt = hInterrupt;
   hG3Dev->hChip = hChip;
   hG3Dev->postInitApFunct = NULL;
   BAST_g3_P_InitHandleDefault(hDev); /* intialize default settings for device handle */
   BAST_g3_P_InitHandle(hDev); /* initialize chip-specific device handle settings */

   BKNI_Memcpy((void*)(&(hDev->settings)), (void*)pDefSettings, sizeof(BAST_Settings));
   hDev->totalChannels = BAST_G3_MAX_CHANNELS;
   for (i = 0; i < BAST_G3_MAX_CHANNELS; i++)
      hDev->pChannels[i] = NULL;

#ifndef BAST_EXCLUDE_FTM
   /* initialize ftm device  */
   retCode = BAST_g3_Ftm_P_InitDevice(hDev);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
   /* create ftm callbacks */
   for(i = 0; i < 32 ; i++)
   {
      retCode = BINT_CreateCallback(&(hG3Dev->hFtmDev.hCallback[i]), hInterrupt, BAST_Ftm_Interrupts[i].IntrId, BAST_g3_Ftm_P_HandleInterrupt_isr, (void*)hDev, i);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
#endif

   hG3Dev->bOpen = false;
   
   /* create events */
#ifndef BAST_EXCLUDE_FTM            
   retCode = BKNI_CreateEvent(&(hG3Dev->hFtmEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
#endif   
   retCode = BKNI_CreateEvent(&(hG3Dev->hInitDoneEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   *h = hDev;

   BDBG_LEAVE(BAST_g3_P_Open);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_Close() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_Close(BAST_Handle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g3_P_Handle *hG3Dev;
#ifndef BAST_EXCLUDE_FTM   
   uint8_t i;
#endif

   BDBG_ENTER(BAST_g3_P_Close);
   hG3Dev = (BAST_g3_P_Handle *)(h->pImpl);
   
#ifndef BAST_EXCLUDE_FTM   
   /* deregister tuners from FTM if not powered down */
   if (!hG3Dev->bFtmPoweredDown)
      retCode = BAST_g3_P_ResetFtm(h);
   
   /* destroy ftm callbacks */
   BAST_g3_Ftm_P_EnableFtmInterrupts(h, false);
   for(i = 0; i < 32 ; i++)
      BINT_DestroyCallback(hG3Dev->hFtmDev.hCallback[i]);
   
   BKNI_DestroyEvent(((BAST_g3_P_Handle *)(h->pImpl))->hFtmEvent);
#endif
   
   BKNI_DestroyEvent(((BAST_g3_P_Handle *)(h->pImpl))->hInitDoneEvent);
   BKNI_Free((void*)h->pChannels);
   BKNI_Free((void*)h->pImpl);
   BKNI_Free((void*)h);
   h = NULL;

   BDBG_LEAVE(BAST_g3_P_Close);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_GetTotalChannels()
******************************************************************************/
BERR_Code BAST_g3_P_GetTotalChannels(
   BAST_Handle  h,             /* [in] BAST handle */
   uint32_t     *totalChannels /* [out] number of channels supported */
)
{
   BSTD_UNUSED(h);
   BDBG_ENTER(BAST_g3_P_GetTotalChannels);
   
   *totalChannels = BAST_G3_MAX_CHANNELS;
   
   BDBG_LEAVE(BAST_g3_P_GetTotalChannels);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_OpenChannel()
******************************************************************************/
BERR_Code BAST_g3_P_OpenChannel(
   BAST_Handle                 h,              /* [in] BAST handle */
   BAST_ChannelHandle         *pChannelHandle, /* [out] BAST channel handle */
   uint32_t                   chnNo,           /* [in] channel number */
   const BAST_ChannelSettings *pSettings       /* [in] channel settings */
)
{
   extern const uint32_t BAST_g3_ChannelIntrID[BAST_G3_MAX_CHANNELS][BAST_g3_MaxIntID];
   
   BERR_Code retCode;
   BAST_ChannelSettings cs;
   BAST_P_ChannelHandle *ch;
   BAST_g3_P_ChannelHandle *chG3;
   BAST_g3_P_Handle *pDev = h->pImpl;

   BDBG_ASSERT(chnNo < BAST_G3_MAX_CHANNELS);
   BDBG_ENTER(BAST_g3_P_OpenChannel);

   if (pSettings == NULL)
      BAST_g3_GetChannelDefaultSettings(h, chnNo, &cs);
   else
      cs = *pSettings;

   /* allocate memory for the channel handle */
   ch = (BAST_P_ChannelHandle *)BKNI_Malloc(sizeof(BAST_P_ChannelHandle));
   BDBG_ASSERT(ch);
   BKNI_Memset((void*)ch, 0, sizeof(BAST_P_ChannelHandle));
   BKNI_Memcpy((void*)(&ch->settings), (void*)&cs, sizeof(BAST_ChannelSettings)); 
   ch->channel = (uint8_t)chnNo;
   ch->pDevice = h;
   h->pChannels[chnNo] = ch;

   chG3 = (BAST_g3_P_ChannelHandle *)BKNI_Malloc(sizeof(BAST_g3_P_ChannelHandle));
   BDBG_ASSERT(chG3);
   BKNI_Memset((void*)chG3, 0, sizeof(BAST_g3_P_ChannelHandle));

   /* create callbacks for legacy qpsk lock interrupts */
   retCode = BINT_CreateCallback(&(chG3->hQpskLockCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eLock], BAST_g3_P_Lock_isr, (void*)ch, BAST_g3_IntID_eLock);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_CreateCallback(&(chG3->hQpskNotLockCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eNotLock], BAST_g3_P_NotLock_isr, (void*)ch, BAST_g3_IntID_eNotLock);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   /* create callbacks for timer interrupts */
   retCode = BINT_CreateCallback(&(chG3->hBaudTimerCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eBaudTimer], BAST_g3_P_BaudTimer_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_CreateCallback(&(chG3->hBerTimerCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eBerTimer], BAST_g3_P_BerTimer_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_CreateCallback(&(chG3->hGen1TimerCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eGenTimer1], BAST_g3_P_Gen1Timer_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_CreateCallback(&(chG3->hGen2TimerCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eGenTimer2], BAST_g3_P_Gen2Timer_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_CreateCallback(&(chG3->hGen3TimerCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eGenTimer3], BAST_g3_P_Gen3Timer_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

#ifndef BAST_EXCLUDE_MI2C   
   /* create callback for MI2C interrupt */
   retCode = BINT_CreateCallback(&(chG3->hMi2cCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eMi2c], BAST_g3_P_Mi2c_isr, (void*)ch, 0);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BKNI_CreateEvent(&(chG3->hMi2cEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);   
#endif

   retCode = BKNI_CreateEvent(&(chG3->hLockChangeEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
#ifndef BAST_EXCLUDE_STATUS_EVENTS    
   retCode = BKNI_CreateEvent(&(chG3->hStatusEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
#endif   
   retCode = BKNI_CreateEvent(&(chG3->hPeakScanEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BKNI_CreateEvent(&(chG3->hTunerInitDoneEvent));
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
   ch->pImpl = (void*)chG3;
   BAST_g3_P_InitChannelHandle(ch);
     
   if (chG3->bHasDiseqc)
   {
      chG3->diseqc = (BAST_g3_P_DiseqcDevice*)BKNI_Malloc(sizeof(BAST_g3_P_DiseqcDevice));
      BDBG_ASSERT(chG3->diseqc);
      
      /* create diseqc timer1 callback */
      retCode = BINT_CreateCallback(&(chG3->diseqc->hDiseqc1TimerCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eDiseqcTimer1], BAST_g3_P_Diseqc1Timer_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create diseqc timer2 callback */
      retCode = BINT_CreateCallback(&(chG3->diseqc->hDiseqc2TimerCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eDiseqcTimer2], BAST_g3_P_Diseqc2Timer_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   
      /* create diseqc done callback */
      retCode = BINT_CreateCallback(&(chG3->diseqc->hDiseqcDoneCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eDiseqcDone], BAST_g3_P_DiseqcDone_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create diseqc tx fifo almost empty callback */
      retCode = BINT_CreateCallback(&(chG3->diseqc->hDiseqcTxFifoAlmostEmptyCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eDiseqcTxFifoAlmostEmpty], BAST_g3_P_DiseqcTxFifoAlmostEmpty_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create diseqc event */
      retCode = BKNI_CreateEvent(&(chG3->diseqc->hDiseqcEvent));
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create diseqc over voltage callback */
      retCode = BINT_CreateCallback(&(chG3->diseqc->hDiseqcOverVoltageCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eDiseqcVoltageGtHiThresh], BAST_g3_P_DiseqcOverVoltage_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create over-voltage diseqc event */
      retCode = BKNI_CreateEvent(&(chG3->diseqc->hDiseqcOverVoltageEvent));
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create diseqc under voltage callback */
      retCode = BINT_CreateCallback(&(chG3->diseqc->hDiseqcUnderVoltageCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eDiseqcVoltageLtLoThresh], BAST_g3_P_DiseqcUnderVoltage_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
      
      /* create under-voltage diseqc event */
      retCode = BKNI_CreateEvent(&(chG3->diseqc->hDiseqcUnderVoltageEvent));
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
      
   /* create HP callback */
   if (chG3->bHasAfec || chG3->bHasTfec)
   {
      /* create event for HP state match */
      retCode = BINT_CreateCallback(&(chG3->hHpLockCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eHp], BAST_g3_P_HpLock_isr, (void*)ch, 0);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }    
   else
      chG3->hHpLockCb = NULL;      
   chG3->hHpFrameBoundaryCb = NULL;      
   
#ifndef BAST_EXCLUDE_LDPC   
   if (chG3->bHasAfec)
   {
      retCode = BINT_CreateCallback(&(chG3->hLdpcLockCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eAfecLock], BAST_g3_P_Lock_isr, (void*)ch, BAST_g3_IntID_eAfecLock);
      BDBG_ASSERT(retCode == BERR_SUCCESS);   
      retCode = BINT_CreateCallback(&(chG3->hLdpcNotLockCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eAfecNotLock], BAST_g3_P_NotLock_isr, (void*)ch, BAST_g3_IntID_eAfecNotLock);
      BDBG_ASSERT(retCode == BERR_SUCCESS);  
      if (BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eHpFrameBoundary] != 0)
      {
         /* create HP frame boundary event */
         retCode = BINT_CreateCallback(&(chG3->hHpFrameBoundaryCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eHpFrameBoundary], BAST_g3_P_HpFrameBoundary_isr, (void*)ch, 0);
         BDBG_ASSERT(retCode == BERR_SUCCESS);
      }     
   }
   else
   {
      chG3->hLdpcLockCb = NULL;  
      chG3->hLdpcNotLockCb = NULL;
   }
#endif   
   
#ifndef BAST_EXCLUDE_TURBO
   if (chG3->bHasTfec)
   {
      retCode = BINT_CreateCallback(&(chG3->hTurboLockCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eTurboLock], BAST_g3_P_Lock_isr, (void*)ch, BAST_g3_IntID_eTurboLock);
      BDBG_ASSERT(retCode == BERR_SUCCESS);   
      retCode = BINT_CreateCallback(&(chG3->hTurboNotLockCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eTurboNotLock], BAST_g3_P_NotLock_isr, (void*)ch, BAST_g3_IntID_eTurboNotLock);
      BDBG_ASSERT(retCode == BERR_SUCCESS); 
      retCode = BINT_CreateCallback(&(chG3->hTurboSyncCb), pDev->hInterrupt, BAST_g3_ChannelIntrID[chnNo][BAST_g3_IntID_eTurboSync], BAST_g3_P_TurboSync_isr, (void*)ch, BAST_g3_IntID_eTurboSync);
      BDBG_ASSERT(retCode == BERR_SUCCESS);       
   }
   else
   {
      chG3->hTurboLockCb = NULL;
      chG3->hTurboNotLockCb = NULL;
   }
#endif
   
   *pChannelHandle = ch;

   BDBG_LEAVE(BAST_g3_P_OpenChannel);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_CloseChannel()
******************************************************************************/
BERR_Code BAST_g3_P_CloseChannel(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   BERR_Code retCode;
   BAST_g3_P_ChannelHandle *chG3;
   
   BDBG_ENTER(BAST_g3_P_CloseChannel);

   /* disable interrupts */
   BAST_CHK_RETCODE(BAST_g3_P_DisableChannelInterrupts(h, true, true));
    
   /* abort acquisition */
   if (hDev->bInit)
      BAST_g3_P_AbortAcq(h);    

   BAST_g3_P_IndicateNotLocked(h);

   chG3 = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   BKNI_DestroyEvent(chG3->hLockChangeEvent);
   BKNI_DestroyEvent(chG3->hPeakScanEvent);
#ifndef BAST_EXCLUDE_STATUS_EVENTS    
   BKNI_DestroyEvent(chG3->hStatusEvent);
#endif
   BKNI_DestroyEvent(chG3->hTunerInitDoneEvent);
#ifndef BAST_EXCLUDE_MI2C   
   BKNI_DestroyEvent(chG3->hMi2cEvent);
#endif

   BINT_DestroyCallback(chG3->hBaudTimerCb);
   BINT_DestroyCallback(chG3->hBerTimerCb);
   BINT_DestroyCallback(chG3->hGen1TimerCb);
   BINT_DestroyCallback(chG3->hGen2TimerCb);
   BINT_DestroyCallback(chG3->hGen3TimerCb);    
   BINT_DestroyCallback(chG3->hQpskLockCb);
   BINT_DestroyCallback(chG3->hQpskNotLockCb);
#ifndef BAST_EXCLUDE_MI2C
   BINT_DestroyCallback(chG3->hMi2cCb);
#endif

   if (chG3->hHpLockCb)
      BINT_DestroyCallback(chG3->hHpLockCb);   

   if (chG3->hHpFrameBoundaryCb)
      BINT_DestroyCallback(chG3->hHpFrameBoundaryCb);   
      
   if (chG3->bHasDiseqc)
   {
      BKNI_DestroyEvent(chG3->diseqc->hDiseqcEvent);   
      BINT_DestroyCallback(chG3->diseqc->hDiseqcDoneCb);    
      BKNI_DestroyEvent(chG3->diseqc->hDiseqcOverVoltageEvent);
      BINT_DestroyCallback(chG3->diseqc->hDiseqcOverVoltageCb);     
      BKNI_DestroyEvent(chG3->diseqc->hDiseqcUnderVoltageEvent);
      BINT_DestroyCallback(chG3->diseqc->hDiseqcUnderVoltageCb);      
      BINT_DestroyCallback(chG3->diseqc->hDiseqc1TimerCb);          
      BINT_DestroyCallback(chG3->diseqc->hDiseqc2TimerCb); 
      BKNI_Free((void*)chG3->diseqc);
      chG3->diseqc = NULL;
   }

#ifndef BAST_EXCLUDE_LDPC   
   if (chG3->bHasAfec)
   {
      BINT_DestroyCallback(chG3->hLdpcLockCb);
      BINT_DestroyCallback(chG3->hLdpcNotLockCb);
   }
#endif

#ifndef BAST_EXCLUDE_TURBO
   if (chG3->bHasTfec)
   {
      BINT_DestroyCallback(chG3->hTurboLockCb);
      BINT_DestroyCallback(chG3->hTurboNotLockCb);
   }
#endif
   BKNI_Free((void*)chG3); 
   BKNI_Free((void*)h);
   h = NULL;

   done:
   BDBG_LEAVE(BAST_g3_P_CloseChannel);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_GetDevice()
******************************************************************************/
BERR_Code BAST_g3_P_GetDevice(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   BAST_Handle *pDev      /* [out] BAST handle */
)
{
   BDBG_ENTER(BAST_g3_P_GetDevice);
   
   *pDev = h->pDevice;
   
   BDBG_LEAVE(BAST_g3_P_GetDevice);
   return BERR_SUCCESS; 
}


/******************************************************************************
 BAST_g3_P_DisableInterrupts() - Non-ISR context
******************************************************************************/ 
BERR_Code BAST_g3_P_DisableInterrupts(
   BAST_Handle h   /* [in] BAST handle */
)
{
   int i;
   BAST_ChannelHandle hChn;
   
#ifndef BAST_EXCLUDE_FTM   
   BAST_g3_Ftm_P_EnableFtmInterrupts(h, false);
#endif   
   for (i = 0; i < h->totalChannels; i++)
   {
      hChn = h->pChannels[i];
      if (hChn != NULL)
         BAST_g3_P_DisableChannelInterrupts(hChn, true, true);
   }
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_InitAp() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_InitAp(
   BAST_Handle h,            /* [in] BAST handle */
   const uint8_t *pHexImage  /* [in] pointer to firmware image */
)
{
   BAST_g3_P_Handle *hDev = (BAST_g3_P_Handle *)(h->pImpl);
   BAST_ChannelHandle hChn;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val;
   int i;

   BSTD_UNUSED(pHexImage);
   BDBG_ENTER(BAST_g3_P_InitAp);
   
   hDev->bInit = false;
   
   /* disable interrupts */
   BAST_CHK_RETCODE(BAST_g3_P_DisableInterrupts(h));
   
   /* reinitialize channel handle state */
   hDev->numInternalTuners = 0;
   for (i = 0; i < h->totalChannels; i++)
   {
      hChn = h->pChannels[i];
      if (hChn == NULL)
         continue;
      BAST_g3_P_InitChannelHandle(hChn);     
#ifndef BAST_EXCLUDE_EXT_TUNER            
      if (((BAST_g3_P_ChannelHandle *)(hChn->pImpl))->bExternalTuner == false)
#endif         
         hDev->numInternalTuners++;
         
      /* power up sds core */
      BAST_CHK_RETCODE(BAST_g3_P_SdsPowerUp(hChn));       

#ifndef BAST_EXCLUDE_EXT_TUNER            
      if (((BAST_g3_P_ChannelHandle *)(hChn->pImpl))->bExternalTuner == false)
#endif         
      {
         /* power up tuner */
         BAST_CHK_RETCODE(BAST_g3_P_TunerPowerUp(hChn));
      }
   }
   /* BDBG_MSG(("numInternalTuners=%d", hDev->numInternalTuners)); */

   /* start initialization process */
   BKNI_WaitForEvent(hDev->hInitDoneEvent, 0); /* clear init done event */
   BKNI_EnterCriticalSection();
   retCode = BAST_g3_P_InitChannels(h->pChannels[0], true);
   BKNI_LeaveCriticalSection();

   /* wait for init done */
   if (BKNI_WaitForEvent(hDev->hInitDoneEvent, 1000) != BERR_SUCCESS)
   {
      BDBG_ERR(("BAST initialization timeout\n")); 
      BERR_TRACE(retCode = BAST_ERR_AP_NOT_INIT);            
   }
   else
   {
      hDev->bInit = true;
      hDev->bOpen = true;
   }

   BAST_g3_P_ReadRegister(h->pChannels[0], BCHP_SDS_MISC_REVID, &val);
   hDev->sdsRevId = (uint8_t)(val & 0xFF);
   
   if (hDev->postInitApFunct)
      retCode = hDev->postInitApFunct(h);
   
   done:
   BDBG_LEAVE(BAST_g3_P_InitAp);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_SoftReset()
******************************************************************************/
BERR_Code BAST_g3_P_SoftReset(
   BAST_Handle h           /* [in] BAST device handle */
)
{
   BERR_Code retCode = BERR_SUCCESS;
   int i;
   
   BDBG_ENTER(BAST_g3_P_SoftReset);

   /* stop any acquisitions */
   for (i = 0; i < h->totalChannels; i++)
   {
      BAST_CHK_RETCODE(BAST_g3_P_ResetChannel(h->pChannels[i], true));
   }

   BAST_g3_P_InitHandleDefault(h); /* intialize default settings for device handle */
   BAST_g3_P_InitHandle(h); /* initialize chip-specific device handle settings */
   for (i = 0; i < h->totalChannels; i++)   
      BAST_g3_P_InitChannelHandle(h->pChannels[i]);

   /* re-initialize tuners */
   BAST_CHK_RETCODE(BAST_g3_P_InitAp(h, NULL));
   
   done:
   BDBG_LEAVE(BAST_g3_P_SoftReset);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_ResetChannel() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_ResetChannel(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool bForceReset       /* [in] true=force reset, false=abort when busy */
)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

#ifndef BAST_EXCLUDE_MI2C   
   /* check if i2c busy */
   if (hChn->bMi2cInProgress)
   {
      if (bForceReset)
      {
         /* reset the mi2c */
         BAST_CHK_RETCODE(BAST_g3_P_Mi2cReset(h));
      }
      else
      {
         BERR_TRACE(retCode = BAST_ERR_MI2C_BUSY);
         return retCode;
      }
   }
#endif   
   
   /* check if diseqc busy */
   if (hChn->bHasDiseqc)
   {
      if (hChn->diseqc->diseqcStatus.status == BAST_DiseqcSendStatus_eBusy)
      {
         if (bForceReset)
         {
            /* reset the diseqc core */
            BAST_CHK_RETCODE(BAST_g3_P_ResetDiseqc(h, 0));
         }
         else
         {
            BERR_TRACE(retCode = BAST_ERR_DISEQC_BUSY);
            return retCode;
         }
      }
   }
   
   /* abort acquisition */
   BAST_CHK_RETCODE(BAST_g3_P_AbortAcq(h));
   BAST_g3_P_IndicateNotLocked(h);
   
   /* disable channel interrupts */
   BAST_CHK_RETCODE(BAST_g3_P_DisableChannelInterrupts(h, true, true));

   /* re-initialize single channel */
   BAST_CHK_RETCODE(BAST_g3_P_InitChannelHandle(h));
#ifndef BAST_EXCLUDE_POWERDOWN
   BAST_CHK_RETCODE(BAST_g3_P_PowerUp(h, BAST_CORE_ALL)); /* power up all cores */
#endif
   BKNI_EnterCriticalSection();
   retCode = BAST_g3_P_InitChannels(h, false);
   BKNI_LeaveCriticalSection();
   
   done:
   return retCode;
}


#ifndef BAST_HAS_LEAP
/******************************************************************************
 BAST_g3_P_GetApVersion()
******************************************************************************/
BERR_Code BAST_g3_P_GetApVersion(
   BAST_Handle h,          /* [in] BAST handle */
   uint16_t    *pChipId,   /* [out] BAST chip ID */
   uint8_t     *pChipVer,  /* [out] chip revision number */
   uint32_t    *pBondId,   /* [out] chip bonding option */
   uint8_t     *pMajVer,   /* [out] AST PI major version */
   uint8_t     *pMinVer    /* [out] AST PI minor version */
)
{
   BSTD_UNUSED(h);
     
   BDBG_ENTER(BAST_g3_P_GetApVersion);
   
   *pChipId = BAST_G3_CHIP;
   *pChipVer = 0xA0 + ((BCHP_VER >> 12) & 0xF0) + (BCHP_VER & 0xF);
   *pBondId = 0;
   *pMajVer = BAST_API_VERSION;
   *pMinVer = BAST_G3_RELEASE_VERSION;

   BDBG_LEAVE(BAST_g3_P_GetApVersion);
   return BERR_SUCCESS; 
}
#endif


/******************************************************************************
 BAST_g3_P_TuneAcquire() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TuneAcquire(
   BAST_ChannelHandle h,            /* [in] BAST channel handle */
   const uint32_t     freq,         /* [in] RF tuner frequency in Hz */
   const BAST_AcqSettings *pParams  /* [in] acquisition parameters */
)
{
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)(h->pImpl);
   BERR_Code retCode = BERR_SUCCESS;
   bool bXbert;
   
   BDBG_ENTER(BAST_g3_P_TuneAcquire);

   if (hDev->bInit != true)
      return (BERR_TRACE(BAST_ERR_AP_NOT_INIT));
   
   BDBG_MSG(("BAST_g3_P_TuneAcquire(chan %d):", h->channel));
   BDBG_MSG(("   freq=%u", freq));
   BDBG_MSG(("   mode=0x%X", pParams->mode));
   BDBG_MSG(("   Fb=%u", pParams->symbolRate));
   BDBG_MSG(("   acq_ctl=0x%X", pParams->acq_ctl));
   BDBG_MSG(("   carrOffset=%d", pParams->carrierFreqOffset));

   if (hChn->coresPoweredDown & BAST_CORE_SDS)
      return BAST_ERR_POWERED_DOWN;
   
   if ((pParams->symbolRate < 1000000UL) || (pParams->symbolRate > 45000000UL))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));

#ifdef BAST_HAS_WFE
   if (freq > 2450000000UL)
      return (BERR_TRACE(BERR_INVALID_PARAMETER));
#else
   if ((freq < 250000000UL) || (freq > 3700000000UL))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));
#endif

   if ((pParams->mode == BAST_Mode_eUnknown) ||
       ((pParams->mode > BAST_Mode_eDcii_4_5) && (pParams->mode < BAST_Mode_eLdpc_Qpsk_1_2)) ||
       (pParams->mode > BAST_Mode_eBlindScan) ||
       ((pParams->mode > BAST_Mode_eTurbo_8psk_8_9) && (pParams->mode < BAST_Mode_eBlindScan)))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));
   
   if ((!hChn->bHasAfec) && (BAST_MODE_IS_LDPC(pParams->mode)))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));
   
   if ((!hChn->bHasTfec) && (BAST_MODE_IS_TURBO(pParams->mode)))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));

   BAST_CHK_RETCODE(BAST_g3_P_PrepareNewAcquisition(h));
           
   hChn->tunerFreq = freq;
   hChn->acqParams = *pParams;
   
   bXbert = false;
   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_BERT_ENABLE)
   {
      if (!(BAST_MODE_IS_DTV(pParams->mode)))
         bXbert = true;
   }
   hChn->xportSettings.bXbert = bXbert; 
   
   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_SIGNAL_DETECT_MODE)
      hChn->signalDetectStatus = (BAST_SIGNAL_DETECT_ENABLED | BAST_SIGNAL_DETECT_IN_PROGRESS);
   else
      hChn->signalDetectStatus = 0;
      
   /* complete tuning and acquisition in ISR context */
   BKNI_EnterCriticalSection();
   retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 30, BAST_g3_P_TuneAcquire_isr);
   BKNI_LeaveCriticalSection();

   done:
   BDBG_LEAVE(BAST_g3_P_TuneAcquire);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_GetChannelStatus()
******************************************************************************/
BERR_Code BAST_g3_P_GetChannelStatus(
   BAST_ChannelHandle h,        /* [in] BAST handle */
   BAST_ChannelStatus *pStatus  /* [out] pointer to status structure */
)
{
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;   
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val, aif, agf, filtctl;   
#ifndef BAST_HAS_WFE   
   bool bRefPllLocked, bMixPllLocked;
#endif
#ifndef BAST_EXCLUDE_TURBO
   bool bSpinv;
#endif
   
   BDBG_ENTER(BAST_g3_P_GetChannelStatus);

   if (hDev->bInit != true)
      return (BERR_TRACE(BAST_ERR_AP_NOT_INIT));
   
   if (hChn->coresPoweredDown & BAST_CORE_SDS)
      return BAST_ERR_POWERED_DOWN;

   BKNI_Memset((void*)pStatus, 0, sizeof(BAST_ChannelStatus));
   
   BAST_CHK_RETCODE(BAST_g3_P_GetActualMode(h, &(pStatus->mode)));

#ifndef BAST_EXCLUDE_EXT_TUNER      
   if (hChn->bExternalTuner)
      pStatus->bTunerLocked = true;
   else
#endif      
#ifndef BAST_HAS_WFE
   {
      BAST_CHK_RETCODE(BAST_g3_P_TunerGetLockStatus(h, &bRefPllLocked, &bMixPllLocked));
      pStatus->bTunerLocked = (bMixPllLocked && bRefPllLocked);
   }
#else
   pStatus->bTunerLocked = true;
#endif

   pStatus->bBertLocked = false;
   BAST_CHK_RETCODE(BAST_g3_P_GetLockStatus(h, &(pStatus->bDemodLocked)));
   if (pStatus->bDemodLocked)
   {
      BAST_g3_P_ReadRegister(h,(uint32_t) BCHP_SDS_BERT_BEST, &val);
      if (val & 0x02000000)
         pStatus->bBertLocked = true;
   }

   BAST_CHK_RETCODE(BAST_g3_P_GetSnr(h, &(pStatus->snrEstimate)));

#ifdef BAST_HAS_WFE
   pStatus->IFagc = 0; /* TBD */
   pStatus->RFagc = 0; /* TBD */
#else
   BAST_g3_P_ReadRegister(h, BCHP_SDS_AGC_AII, &(pStatus->IFagc));
   BAST_g3_P_ReadRegister(h, BCHP_SDS_AGC_AIT, &(pStatus->RFagc));
#endif

   BAST_g3_P_ReadRegister(h,(uint32_t) BCHP_SDS_FE_AIF, (uint32_t *)&aif);
   BAST_g3_P_ReadRegister(h,(uint32_t) BCHP_SDS_FE_AGF, (uint32_t *)&agf);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_FE_FILTCTL, &filtctl);
   pStatus->agf = (aif & 0xFFFFFF00) | ((filtctl >> 8) & 0xC0) | ((agf >> 16)& 0x0000000F); 

   BAST_CHK_RETCODE(BAST_g3_P_GetSampleFreq(h, &(pStatus->sample_clock)));
   pStatus->symbolRate = hChn->acqParams.symbolRate;
   pStatus->carrierOffset = hChn->acqParams.carrierFreqOffset;
   pStatus->reacqCount = hChn->acqCount;
   pStatus->tunerFreq = hChn->tunerFreq;
   pStatus->outputBitrate = hChn->outputBitRate;
   
   if (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_SET_FILTER_MANUAL)
      pStatus->tunerFilter = (uint32_t)hChn->tunerCutoff;
   else
      pStatus->tunerFilter = (uint32_t)hChn->tunerLpfToCalibrate;
   pStatus->tunerFilter *= 1000000;
      
   BAST_CHK_RETCODE(BAST_g3_P_GetSymbolRateError(h, &(pStatus->symbolRateError)));      
   BAST_CHK_RETCODE(BAST_g3_P_GetCarrierError(h, &(pStatus->carrierError)));

   if (BAST_MODE_IS_LEGACY_QPSK(pStatus->mode))
   {   
      BAST_g3_P_QpskUpdateErrorCounters(h);
      pStatus->modeStatus.legacy.rsCorrCount = hChn->rsCorr;
      pStatus->modeStatus.legacy.rsUncorrCount = hChn->rsUncorr;
      pStatus->modeStatus.legacy.preVitErrCount = hChn->preVitErrors;

      BAST_g3_P_ReadRegister(h, BCHP_SDS_VIT_VST, &val);
      pStatus->modeStatus.legacy.spinv = (BAST_Spinv)((val >> 28) & 0x03);
      pStatus->modeStatus.legacy.phase = (BAST_PhaseRotation)((val >> 17) & 0x03);
   }
#ifndef BAST_EXCLUDE_LDPC   
   else if (BAST_MODE_IS_LDPC(pStatus->mode))
   {  
      BKNI_EnterCriticalSection();
      retCode = BAST_g3_P_LdpcUpdateBlockCounters(h);
      BKNI_LeaveCriticalSection();
      if (retCode != BERR_SUCCESS)
         goto done;      
      pStatus->modeStatus.ldpc.corrBlocks = hChn->ldpcCorrBlocks;
      pStatus->modeStatus.ldpc.badBlocks = hChn->ldpcBadBlocks;
      pStatus->modeStatus.ldpc.totalBlocks = hChn->ldpcTotalBlocks;

      BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_ACM_CHECK, &val);
      pStatus->modeStatus.ldpc.hpstatus = val;
      if (BAST_g3_P_GetAfecClock(h, &(pStatus->modeStatus.ldpc.fec_clock)) != BERR_SUCCESS)
         pStatus->modeStatus.ldpc.fec_clock = 0;
   }
#endif   
#ifndef BAST_EXCLUDE_TURBO
   else if (BAST_MODE_IS_TURBO(pStatus->mode))
   {   
      BKNI_EnterCriticalSection();
      retCode = BAST_g3_P_TurboUpdateErrorCounters(h);
      BKNI_LeaveCriticalSection();
      if (retCode != BERR_SUCCESS)
         goto done;
      pStatus->modeStatus.turbo.totalBlocks = hChn->turboTotalBlocks;
      pStatus->modeStatus.turbo.corrBlocks = hChn->turboCorrBlocks;
      pStatus->modeStatus.turbo.badBlocks = hChn->turboBadBlocks;

      BAST_g3_P_ReadRegister(h, BCHP_SDS_HP_ACM_CHECK, &val);   
      BAST_CHK_RETCODE(BAST_g3_P_HpIsSpinv(h, &bSpinv));      
      if (bSpinv)      
         val |= 0x80;
      else
         val &= ~0x80;         
      
      pStatus->modeStatus.turbo.hpstatus = val;
      if (BAST_g3_P_GetTfecClock(h, &(pStatus->modeStatus.turbo.fec_clock)) != BERR_SUCCESS)
         pStatus->modeStatus.turbo.fec_clock = 0;
   }
#endif
   else
   {   
      BAST_CHK_RETCODE(BAST_g3_P_UpdateErrorCounters(h));
   }
   
   pStatus->mpegErrors = hChn->mpegErrorCount;
   pStatus->mpegCount = hChn->mpegFrameCount;
   pStatus->berErrors = hChn->berErrors;
   
   done:
   BDBG_LEAVE(BAST_g3_P_GetChannelStatus);   
   return retCode;
}


/******************************************************************************
 BAST_g3_P_GetLockStatus()
******************************************************************************/
BERR_Code BAST_g3_P_GetLockStatus(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   bool *bLocked          /* [out] lock indicator */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BDBG_ENTER(BAST_g3_P_GetLockStatus);

   *bLocked = (hChn->bLocked && (hChn->acqState == BAST_AcqState_eMonitorLock)) ? true : false;
   
   BDBG_LEAVE(BAST_g3_P_GetLockStatus);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_ResetStatus()
******************************************************************************/
BERR_Code BAST_g3_P_ResetStatus(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   
   BDBG_ENTER(BAST_g3_P_ResetStatus);

   retCode = BAST_g3_P_ResetErrorCounters(h);
   hChn->acqCount = 0;
   
   BDBG_LEAVE(BAST_g3_P_ResetStatus);
   return retCode;
}


/******************************************************************************
 BAST_g3_P_GetSoftDecisionBuf()
******************************************************************************/
BERR_Code BAST_g3_P_GetSoftDecisionBuf(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   int16_t *pI,           /* [out] 15 I-values */
   int16_t *pQ            /* [out] 15 Q-values */
)
{
   uint32_t val, i;
   
   BDBG_ENTER(BAST_g3_P_GetSoftDecisionBuf);

   for (i = 0; i < 15; i++)
   {
      BAST_g3_P_ReadRegister(h, BCHP_SDS_EQ_EQSFT, &val);
      pI[i] = (int16_t)((val >> 24) & 0xFF);
      if (pI[i] & 0x80)
         pI[i] -= 0x100;
      pQ[i] = (int16_t)((val >> 16) & 0xFF);
      if (pQ[i] & 0x80)
         pQ[i] -= 0x100;
   }
   
   BDBG_LEAVE(BAST_g3_P_GetSoftDecisionBuf);
   return BERR_SUCCESS;
}


#ifndef BAST_HAS_WFE
/******************************************************************************
 BERR_Code BAST_g3_P_ReadAgc()
******************************************************************************/
BERR_Code BAST_g3_P_ReadAgc(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   BAST_Agc which_agc,    /* [in] AGC select */
   uint32_t *pAgc         /* [out] integrator value */
)
{
   uint32_t reg = ((which_agc == BAST_Agc_eRF) ? BCHP_SDS_AGC_AIT : BCHP_SDS_AGC_AII);
   BDBG_ENTER(BAST_g3_P_ReadAgc);
   
   BAST_g3_P_ReadRegister(h, reg, pAgc);
   
   BDBG_LEAVE(BAST_g3_P_ReadAgc);
   return BERR_SUCCESS;
}
#endif   


#ifndef BAST_HAS_WFE
/******************************************************************************
 BERR_Code BAST_g3_P_WriteAgc()
******************************************************************************/
BERR_Code BAST_g3_P_WriteAgc(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   BAST_Agc which_agc,    /* [in] AGC select */
   uint32_t *pAgc         /* [in] integrator value */
)
{
   uint32_t reg = ((which_agc == BAST_Agc_eRF) ? BCHP_SDS_AGC_AIT : BCHP_SDS_AGC_AII);
   BDBG_ENTER(BAST_g3_P_WriteAgc);
   
   BAST_g3_P_WriteRegister(h, reg, pAgc);
   
   BDBG_LEAVE(BAST_g3_P_WriteAgc);
   return BERR_SUCCESS;
}
#endif


#ifndef BAST_HAS_WFE
/******************************************************************************
 BERR_Code BAST_g3_P_FreezeAgc()
******************************************************************************/
BERR_Code BAST_g3_P_FreezeAgc(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   BAST_Agc which_agc,    /* [in] AGC select */
   bool bFreeze           /* [in] true = freeze, false = unfreeze */
)
{
   uint32_t and_mask, or_mask;
   
   BDBG_ENTER(BAST_g3_P_FreezeAgc);
   
   or_mask = 0;
   if (which_agc == BAST_Agc_eRF)
   {
      and_mask = ~0x200;
      if (bFreeze)
         or_mask = 0x200;
   }
   else
   {
      and_mask = ~2;
      if (bFreeze)
         or_mask = 2;
   }
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_AGC_AGCCTL, and_mask, or_mask);
   
   BDBG_LEAVE(BAST_g3_P_FreezeAgc);
   return BERR_SUCCESS;
}
#endif


/******************************************************************************
 BERR_Code BAST_g3_P_FreezeEq()
******************************************************************************/
BERR_Code BAST_g3_P_FreezeEq(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   bool bFreeze           /* [in] true = freeze, false = unfreeze */
)
{
   uint32_t and_mask, or_mask;
   
   BDBG_ENTER(BAST_g3_P_FreezeEq);
   
   and_mask = ~0x700;
   or_mask = bFreeze ? 0x700 : 0x200;   
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_EQ_EQFFECTL, and_mask, or_mask);
   
   BDBG_LEAVE(BAST_g3_P_FreezeEq);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_PowerDown()
******************************************************************************/
BERR_Code BAST_g3_P_PowerDown(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   uint32_t options       /* [in] see BAST_CORE_* macros */
)
{
#ifndef BAST_EXCLUDE_POWERDOWN
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;  
   BERR_Code retCode = BERR_SUCCESS;
   bool bAbort = false;
   
   BDBG_ENTER(BAST_g3_P_PowerDown);  
   
#ifdef BAST_HAS_WFE
   if ((options & BAST_CORE_CHANNELIZER) && ((hChn->coresPoweredDown & BAST_CORE_CHANNELIZER) == 0))
   {
      /* abort acquisition before powering down */
      BAST_CHK_RETCODE(BAST_g3_P_AbortAcq(h));
      bAbort = true;
      
      hChn->coresPoweredDown |= BAST_CORE_CHANNELIZER;
      BAST_g3_P_TunerPowerDown(h);
   }
#endif

   if ((options & BAST_CORE_SDS) && ((hChn->coresPoweredDown & BAST_CORE_SDS) == 0))
   {
      /* abort acquisition before powering down */
      if (bAbort == false)
      {
      BAST_CHK_RETCODE(BAST_g3_P_AbortAcq(h));
         bAbort = true;
      }
      hChn->coresPoweredDown |= BAST_CORE_SDS;

#ifndef BAST_HAS_WFE
#ifndef BAST_EXCLUDE_EXT_TUNER            
      if (!(hChn->bExternalTuner))
#endif         
      {
         /* power down tuner */
         BAST_g3_P_TunerPowerDown(h);
      }
#endif
      
      BAST_CHK_RETCODE(BAST_g3_P_SdsPowerDown(h));
#ifndef BAST_EXCLUDE_LDPC      
      BAST_CHK_RETCODE(BAST_g3_P_LdpcPowerDown(h));
#endif
#ifndef BAST_EXCLUDE_TURBO      
      BAST_CHK_RETCODE(BAST_g3_P_TurboPowerDown(h));
#endif      
   }
   
   if (hChn->bHasDiseqc)
   {
      if ((options & BAST_CORE_DISEQC) && ((hChn->coresPoweredDown & BAST_CORE_DISEQC) == 0))
      {
         hChn->coresPoweredDown |= BAST_CORE_DISEQC;
         BAST_CHK_RETCODE(BAST_g3_P_DiseqcPowerDown(h));
      }
   }
   
   done:
   BDBG_LEAVE(BAST_g3_P_PowerDown);   
   return retCode;
#endif
}


/******************************************************************************
 BAST_g3_P_TunerInitDone()
******************************************************************************/
BERR_Code BAST_g3_P_TunerInitDone(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BKNI_SetEvent(hChn->hTunerInitDoneEvent);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_PowerUp()
******************************************************************************/
BERR_Code BAST_g3_P_PowerUp(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   uint32_t options       /* [in] see BAST_CORE_* macros */
)
{
#ifndef BAST_EXCLUDE_POWERDOWN
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   
   BDBG_ENTER(BAST_g3_P_PowerUp);  
   
#ifdef BAST_HAS_WFE
   if ((options & BAST_CORE_CHANNELIZER) && (hChn->coresPoweredDown & BAST_CORE_CHANNELIZER))
   {
      hChn->coresPoweredDown &= ~BAST_CORE_CHANNELIZER;   
      BAST_g3_P_TunerPowerUp(h);
   }
#endif

   if ((options & BAST_CORE_SDS) && (hChn->coresPoweredDown & BAST_CORE_SDS))
   {
      /* power up TUNER and SDS */
      hChn->coresPoweredDown &= ~BAST_CORE_SDS;
      
      /* power up sds core */
      BAST_CHK_RETCODE(BAST_g3_P_SdsPowerUp(h));

#ifndef BAST_HAS_WFE
#ifndef BAST_EXCLUDE_EXT_TUNER            
      if (hChn->bExternalTuner == false)
#endif         
      {
         /* power up and initialize tuner */
         BKNI_WaitForEvent(hChn->hTunerInitDoneEvent, 0); /* clear init done event */
         BAST_CHK_RETCODE(BAST_g3_P_TunerPowerUp(h));
         
         BKNI_EnterCriticalSection();
         retCode = BAST_g3_P_TunerInit(h, BAST_g3_P_TunerInitDone);
         BKNI_LeaveCriticalSection();
         if (retCode != BERR_SUCCESS)
            goto done;
         
         /* wait for tuner init done */
         if (BKNI_WaitForEvent(hChn->hTunerInitDoneEvent, 200) != BERR_SUCCESS)
         {
            BDBG_ERR(("Tuner initialization timeout\n")); 
            BERR_TRACE(retCode = BAST_ERR_AP_NOT_INIT);            
         }
      }
#endif
   }
   
   if (hChn->bHasDiseqc)
   {
      if ((options & BAST_CORE_DISEQC) && (hChn->coresPoweredDown & BAST_CORE_DISEQC))
      {
         /* power up DISEQC */
         hChn->coresPoweredDown &= ~BAST_CORE_DISEQC;
         BAST_CHK_RETCODE(BAST_g3_P_DiseqcPowerUp(h));
      }
   }
   
   done:
   BDBG_LEAVE(BAST_g3_P_PowerUp);   
   return retCode;
#endif
}


/******************************************************************************
 BAST_g3_P_ReadConfig()
******************************************************************************/
BERR_Code BAST_g3_P_ReadConfig(
   BAST_ChannelHandle h,   /* [in] BAST channel handle */
   uint16_t id,            /* [in] configuration parameter ID */
   uint8_t *p,                /* [in] buffer to hold the read data */
   uint8_t len             /* [in] number of bytes to read */
)
{
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;

   BDBG_ENTER(BAST_g3_P_ReadConfig); 
   
   if (hDev->bInit != true)
      return (BERR_TRACE(BAST_ERR_AP_NOT_INIT));
      
   if ((p == NULL) ||
       !((len == 1) || (len == 2) || (len == 4)))
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      goto done;
   }

   switch (id)
   {
#ifndef BAST_HAS_WFE
      case BAST_G3_CONFIG_TUNER_AGC_THRESH:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_AGC_THRESH);
         p[0] = (uint8_t)((uint32_t)hChn->tunerAgcThreshold >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->tunerAgcThreshold >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->tunerAgcThreshold >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->tunerAgcThreshold & 0xFF);
         break;
      
      case BAST_G3_CONFIG_TUNER_DAISY_GAIN:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_DAISY_GAIN);
         *p = hChn->tunerDaisyGain;
         break;
      
      case BAST_G3_CONFIG_TUNER_AGC_WIN_LEN:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_AGC_WIN_LEN);
         p[0] = (uint8_t)((uint32_t)hChn->tunerAgcWinLength >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->tunerAgcWinLength >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->tunerAgcWinLength >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->tunerAgcWinLength & 0xFF);
         break;
      
      case BAST_G3_CONFIG_TUNER_AGC_AMP_THRESH:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_AGC_AMP_THRESH);
         p[0] = (uint8_t)(hChn->tunerAgcAmpThresh >> 8);
         p[1] = (uint8_t)(hChn->tunerAgcAmpThresh & 0xFF);
         break;
      
      case BAST_G3_CONFIG_TUNER_AGC_LOOP_COEFF:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_AGC_LOOP_COEFF);
         p[0] = (uint8_t)(hChn->tunerAgcLoopCoeff >> 8);
         p[1] = (uint8_t)(hChn->tunerAgcLoopCoeff & 0xFF);
         break;
      
      case BAST_G3_CONFIG_AMBIENT_TEMP:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_AMBIENT_TEMP);
         *p = hChn->tempAmbient;
         break; 
      
      case BAST_G3_CONFIG_TUNER_FILCAL_UPPER:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_FILCAL_UPPER);
         *p = hChn->tunerFilCalUpperThresh;
         break;
      
      case BAST_G3_CONFIG_TUNER_FILCAL_LOWER:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_FILCAL_LOWER);
         *p = hChn->tunerFilCalLowerThresh;
         break;         
#endif
      
      case BAST_G3_CONFIG_REACQ_CAUSE:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_REACQ_CAUSE);
         p[0] = (uint8_t)((uint32_t)hChn->reacqCause >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->reacqCause >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->reacqCause >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->reacqCause & 0xFF);   
         break;
         
#ifndef BAST_EXCLUDE_LDPC
      case BAST_G3_CONFIG_ACM_MAX_MODE:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_ACM_MAX_MODE);
         *p = (uint8_t)(hChn->acmMaxMode);
         break;
      
      case BAST_G3_CONFIG_LDPC_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_LDPC_CTL);
         *p = hChn->ldpcCtl;
         break;
      
      case BAST_G3_CONFIG_LDPC_SCAN_MODES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_LDPC_SCAN_MODES);     
         p[0] = (uint8_t)(hChn->ldpcScanModes >> 8);
         p[1] = (uint8_t)(hChn->ldpcScanModes & 0xFF);
         break;           
#endif
      
      case BAST_G3_CONFIG_DEBUG1:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DEBUG1);
         p[0] = (uint8_t)((uint32_t)hChn->debug1 >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->debug1 >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->debug1 >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->debug1 & 0xFF);   
         break;
      
      case BAST_G3_CONFIG_DEBUG2:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DEBUG2);
         p[0] = (uint8_t)((uint32_t)hChn->debug2 >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->debug2 >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->debug2 >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->debug2 & 0xFF);   
         break;
      
      case BAST_G3_CONFIG_DEBUG3:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DEBUG3);
         p[0] = (uint8_t)((uint32_t)hChn->debug3 >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->debug3 >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->debug3 >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->debug3 & 0xFF);   
         break;
         
      case BAST_G3_CONFIG_MISC_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_MISC_CTL);
         *p = hChn->miscCtl;      
         break;
      
      case BAST_G3_CONFIG_REACQ_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_REACQ_CTL);
         *p = hChn->reacqCtl;
         break;
      
      case BAST_G3_CONFIG_PLC_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_PLC_CTL);
         *p = hChn->plcCtl;
         break;
      
      case BAST_G3_CONFIG_PLC_ALT_ACQ_BW:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_PLC_ALT_ACQ_BW);
         p[0] = (uint8_t)((uint32_t)hChn->altPlcAcqBw >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->altPlcAcqBw >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->altPlcAcqBw >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->altPlcAcqBw & 0xFF);           
         break;
      
      case BAST_G3_CONFIG_PLC_ALT_ACQ_DAMP:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_PLC_ALT_ACQ_DAMP);
         *p = hChn->altPlcAcqDamp;         
         break;
      
      case BAST_G3_CONFIG_PLC_ALT_TRK_BW:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_PLC_ALT_TRK_BW);
         p[0] = (uint8_t)((uint32_t)hChn->altPlcTrkBw >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->altPlcTrkBw >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->altPlcTrkBw >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->altPlcTrkBw & 0xFF);           
         break;
      
      case BAST_G3_CONFIG_PLC_ALT_TRK_DAMP:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_PLC_ALT_TRK_DAMP);
         *p = hChn->altPlcTrkDamp;         
         break;
      
      case BAST_G3_CONFIG_IF_STEP_SAVE:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_IF_STEP_SAVE);      
         p[0] = (uint8_t)((uint32_t)hChn->dftFreqEstimate >> 24);
         p[1] = (uint8_t)((uint32_t)hChn->dftFreqEstimate >> 16);
         p[2] = (uint8_t)((uint32_t)hChn->dftFreqEstimate >> 8);
         p[3] = (uint8_t)((uint32_t)hChn->dftFreqEstimate & 0xFF);         
         break;
      
      case BAST_G3_CONFIG_FREQ_ESTIMATE_STATUS:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_FREQ_ESTIMATE_STATUS); 
         *p = hChn->dftFreqEstimateStatus;
         break;
      
      case BAST_G3_CONFIG_STUFF_BYTES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_STUFF_BYTES);   
         *p = hChn->stuffBytes;   
         break;
      
#ifndef BAST_EXCLUDE_TURBO
      case BAST_G3_CONFIG_TURBO_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TURBO_CTL);
         *p = hChn->turboCtl;
         break;
      
      case BAST_G3_CONFIG_TURBO_SCAN_MODES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TURBO_SCAN_MODES);     
         p[0] = (uint8_t)(hChn->turboScanModes >> 8);
         p[1] = (uint8_t)(hChn->turboScanModes & 0xFF);
         break;
#endif
      
      case BAST_G3_CONFIG_DFT_RANGE_START:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DFT_RANGE_START);
         p[0] = (uint8_t)(hChn->dftRangeStart >> 8);
         p[1] = (uint8_t)(hChn->dftRangeStart & 0xFF);
         break;
         
      case BAST_G3_CONFIG_DFT_RANGE_END:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DFT_RANGE_END);
         p[0] = (uint8_t)(hChn->dftRangeEnd >> 8);
         p[1] = (uint8_t)(hChn->dftRangeEnd & 0xFF);
         break;         
         
      case BAST_G3_CONFIG_ACQ_TIME:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_ACQ_TIME);
         p[0] = (uint8_t)(hChn->acqTime >> 24);
         p[1] = (uint8_t)(hChn->acqTime >> 16);
         p[2] = (uint8_t)(hChn->acqTime >> 8);
         p[3] = (uint8_t)(hChn->acqTime & 0xFF);
         break;
         
      case BAST_G3_CONFIG_XTAL_FREQ:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_XTAL_FREQ);
         p[0] = (uint8_t)(hDev->xtalFreq >> 24);
         p[1] = (uint8_t)(hDev->xtalFreq >> 16);
         p[2] = (uint8_t)(hDev->xtalFreq >> 8);
         p[3] = (uint8_t)(hDev->xtalFreq & 0xFF);
         break;
      
      case BAST_G3_CONFIG_TUNER_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_CTL);
         *p = hChn->tunerCtl;
         break;
         
      case BAST_G3_CONFIG_TUNER_FILTER_OVERRIDE:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_FILTER_OVERRIDE);
         *p = hChn->tunerCutoff;
         break;
         
      case BAST_G3_CONFIG_AGC_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_AGC_CTL);
         *p = hChn->agcCtl;
         break;
      
      case BAST_G3_CONFIG_BLIND_SCAN_MODES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_BLIND_SCAN_MODES);
         *p = hChn->blindScanModes;
         break;
         
      case BAST_G3_CONFIG_DTV_SCAN_CODE_RATES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DTV_SCAN_CODE_RATES);     
         *p = hChn->dtvScanCodeRates;
         break;
         
      case BAST_G3_CONFIG_DVB_SCAN_CODE_RATES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DVB_SCAN_CODE_RATES);     
         *p = hChn->dvbScanCodeRates;
         break;
         
      case BAST_G3_CONFIG_DCII_SCAN_CODE_RATES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DCII_SCAN_CODE_RATES);     
         *p = hChn->dciiScanCodeRates;
         break;
         
#ifndef BAST_EXCLUDE_STATUS_EVENTS
      case BAST_G3_CONFIG_STATUS_INDICATOR:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_STATUS_INDICATOR);
         *p = hChn->statusEventIndicators;
         break;
         
      case BAST_G3_CONFIG_FREQ_DRIFT_THRESHOLD:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_FREQ_DRIFT_THRESHOLD);
         p[0] = (uint8_t)(hChn->freqDriftThreshold >> 24);
         p[1] = (uint8_t)(hChn->freqDriftThreshold >> 16);
         p[2] = (uint8_t)(hChn->freqDriftThreshold >> 8);
         p[3] = (uint8_t)(hChn->freqDriftThreshold & 0xFF);
         break;
         
      case BAST_G3_CONFIG_RAIN_FADE_THRESHOLD:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_RAIN_FADE_THRESHOLD); 
         *p = hChn->rainFadeThreshold;
         break;
         
      case BAST_G3_CONFIG_RAIN_FADE_WINDOW:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_RAIN_FADE_WINDOW);
         p[0] = (uint8_t)(hChn->rainFadeWindow >> 24);
         p[1] = (uint8_t)(hChn->rainFadeWindow >> 16);
         p[2] = (uint8_t)(hChn->rainFadeWindow >> 8);
         p[3] = (uint8_t)(hChn->rainFadeWindow & 0xFF);
         break;
#endif            
         
#ifndef BAST_EXCLUDE_FTM
      case BAST_G3_CONFIG_FTM_TX_POWER:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_FTM_TX_POWER);
         *p = hDev->ftmTxPower;
         break;
         
      case BAST_G3_CONFIG_FTM_CH_SELECT:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_FTM_CH_SELECT);
         if ((hDev->fskTxChannel == BAST_FskChannel_e0) && (hDev->fskRxChannel == BAST_FskChannel_e0))
         {
            *p = BAST_G3_FskChannelConfig_eCh0Tx_Ch0Rx;
         }
         else if ((hDev->fskTxChannel == BAST_FskChannel_e0) && (hDev->fskRxChannel == BAST_FskChannel_e1))
         {
            *p = BAST_G3_FskChannelConfig_eCh0Tx_Ch1Rx;
         }
         else if ((hDev->fskTxChannel == BAST_FskChannel_e1) && (hDev->fskRxChannel == BAST_FskChannel_e0))
         {
            *p = BAST_G3_FskChannelConfig_eCh1Tx_Ch0Rx;
         }
         else
         {
            *p = BAST_G3_FskChannelConfig_eCh1Tx_Ch1Rx;
         }
         break;
#endif
      
      case BAST_G3_CONFIG_BCM3445_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_BCM3445_CTL);
#ifndef BAST_EXCLUDE_BCM3445
         *p = hChn->bcm3445Ctl;
#else
         *p = 0;
#endif
         break;
      
      default:
         BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
         break;
   }
   
   done:
   BDBG_LEAVE(BAST_g3_P_ReadConfig);     
   return retCode;
}


/******************************************************************************
 BAST_g3_P_WriteConfig()
******************************************************************************/
BERR_Code BAST_g3_P_WriteConfig(
   BAST_ChannelHandle h,   /* [in] BAST channel handle */
   uint16_t id,            /* [in] configuration parameter ID */
   uint8_t *p,             /* [in] data to write */
   uint8_t len             /* [in] number of bytes to write */
)
{
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
#ifndef BAST_EXCLUDE_LDPC     
   BAST_Mode m;
#endif

   BDBG_ENTER(BAST_g3_P_WriteConfig); 
   
   if (hDev->bInit != true)
      return (BERR_TRACE(BAST_ERR_AP_NOT_INIT));
      
   if ((p == NULL) ||
       !((len == 1) || (len == 2) || (len == 4) ))
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      goto done;
   }

   switch (id)
   {
#ifndef BAST_EXCLUDE_LDPC
      case BAST_G3_CONFIG_ACM_MAX_MODE:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_ACM_MAX_MODE);
         m = (BAST_Mode)(*p);
         if ((m < BAST_Mode_eLdpc_Qpsk_1_2) || (m > BAST_Mode_eLdpc_8psk_9_10))
            retCode = BERR_INVALID_PARAMETER;
         else
            hChn->acmMaxMode = m;
         break;
         
      case BAST_G3_CONFIG_LDPC_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_LDPC_CTL);
         hChn->ldpcCtl = *p;
         break;
      
      case BAST_G3_CONFIG_LDPC_SCAN_MODES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_LDPC_SCAN_MODES);
         hChn->ldpcScanModes = (p[0] << 8) | p[1];
         break;         
#endif
      
#ifndef BAST_HAS_WFE
      case BAST_G3_CONFIG_TUNER_AGC_THRESH:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_AGC_THRESH);
         hChn->tunerAgcThreshold = (uint32_t)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
         BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eRfagc, 0x1, hChn->tunerAgcThreshold & 0x0000FFFF);
         BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eBbagc, 0x1, hChn->tunerAgcThreshold & 0xFFFF0000);
         break;
      
      case BAST_G3_CONFIG_TUNER_DAISY_GAIN:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_DAISY_GAIN);
         hChn->tunerDaisyGain = *p & 0x3;
         break;
      
      case BAST_G3_CONFIG_TUNER_AGC_WIN_LEN:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_AGC_WIN_LEN);
         hChn->tunerAgcWinLength = (uint32_t)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
         BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eRfagc, 0x2, hChn->tunerAgcWinLength & 0x0000FFFF);
         BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eBbagc, 0x2, hChn->tunerAgcWinLength >> 16);  
         break;
      
      case BAST_G3_CONFIG_TUNER_AGC_AMP_THRESH:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_AGC_AMP_THRESH);
         hChn->tunerAgcAmpThresh = (uint16_t)((p[0] << 8) | p[1]);
         hChn->tunerAgcAmpThresh &= 0x1F1F;
         BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_RFAGC_R01, ~0x0003E000, (hChn->tunerAgcAmpThresh & 0x001F) << 13);
         BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_TUNER_BBAGC_R01, ~0x0003E000, (hChn->tunerAgcAmpThresh & 0x1F00) << 5);
         break;
      
      case BAST_G3_CONFIG_TUNER_AGC_LOOP_COEFF:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_AGC_LOOP_COEFF);
         hChn->tunerAgcLoopCoeff = (uint16_t)((p[0] << 8) | p[1]);
         hChn->tunerAgcLoopCoeff &= 0x1F1F;
         BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eRfagc, 0x4, hChn->tunerAgcLoopCoeff & 0x001F);
         BAST_g3_P_TunerIndirectWrite(h, BAST_TunerIndirectRegGroup_eBbagc, 0x4, hChn->tunerAgcLoopCoeff >> 8);
         break;
      
      case BAST_G3_CONFIG_AMBIENT_TEMP:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_AMBIENT_TEMP);
         hChn->tempAmbient = *p;
         break;  
      
      case BAST_G3_CONFIG_TUNER_FILCAL_UPPER:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_FILCAL_UPPER);
         hChn->tunerFilCalUpperThresh = *p;
         break;
      
      case BAST_G3_CONFIG_TUNER_FILCAL_LOWER:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_FILCAL_LOWER);
         hChn->tunerFilCalLowerThresh = *p;
         break;         
#endif
      
      case BAST_G3_CONFIG_REACQ_CAUSE:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_REACQ_CAUSE);
         hChn->reacqCause = (BAST_ReacqCause)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
         break;
      
      case BAST_G3_CONFIG_DEBUG1:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DEBUG1);
         hChn->debug1 = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
      
      case BAST_G3_CONFIG_DEBUG2:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DEBUG2);
         hChn->debug2 = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
      
      case BAST_G3_CONFIG_DEBUG3:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DEBUG3);
         hChn->debug3 = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
         
      case BAST_G3_CONFIG_PLC_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_PLC_CTL);
         hChn->plcCtl = *p;
         break;
      
      case BAST_G3_CONFIG_MISC_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_MISC_CTL);
         hChn->miscCtl = *p;
         break;
      
      case BAST_G3_CONFIG_REACQ_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_REACQ_CTL);
         hChn->reacqCtl = *p;
         break;
      
      case BAST_G3_CONFIG_PLC_ALT_ACQ_BW:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_PLC_ALT_ACQ_BW);
         hChn->altPlcAcqBw = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
      
      case BAST_G3_CONFIG_PLC_ALT_ACQ_DAMP:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_PLC_ALT_ACQ_DAMP);
         hChn->altPlcAcqDamp = *p;
         break;
      
      case BAST_G3_CONFIG_PLC_ALT_TRK_BW:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_PLC_ALT_TRK_BW);
         hChn->altPlcTrkBw = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
      
      case BAST_G3_CONFIG_PLC_ALT_TRK_DAMP:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_PLC_ALT_TRK_DAMP);
         hChn->altPlcTrkDamp = *p;
         break;
      
      case BAST_G3_CONFIG_IF_STEP_SAVE:
         BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
         break;
      
      case BAST_G3_CONFIG_FREQ_ESTIMATE_STATUS:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_FREQ_ESTIMATE_STATUS); 
         hChn->dftFreqEstimateStatus = *p;
         break;
      
      case BAST_G3_CONFIG_STUFF_BYTES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_STUFF_BYTES);   
         hChn->stuffBytes = *p & 0x1F;
         break;
      
#ifndef BAST_EXCLUDE_TURBO
       case BAST_G3_CONFIG_TURBO_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TURBO_CTL);
         hChn->turboCtl = *p;
         break;
      
      case BAST_G3_CONFIG_TURBO_SCAN_MODES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TURBO_SCAN_MODES);
         hChn->turboScanModes = (p[0] << 8) | p[1];
         break;
#endif
      
      case BAST_G3_CONFIG_DFT_RANGE_START:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DFT_RANGE_START);
         hChn->dftRangeStart = (p[0] << 8) | p[1]; 
         break;
      
      case BAST_G3_CONFIG_DFT_RANGE_END:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DFT_RANGE_END);
         hChn->dftRangeEnd = (p[0] << 8) | p[1]; 
         break;         
      
      case BAST_G3_CONFIG_ACQ_TIME:
         BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
         break;
      
      case BAST_G3_CONFIG_XTAL_FREQ:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_XTAL_FREQ);
         hDev->xtalFreq = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
      
      case BAST_G3_CONFIG_TUNER_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_CTL);
         hChn->tunerCtl = *p;
         break;
      
      case BAST_G3_CONFIG_TUNER_FILTER_OVERRIDE:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_TUNER_FILTER_OVERRIDE);
         hChn->tunerCutoff = *p;
         break;
      
      case BAST_G3_CONFIG_AGC_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_AGC_CTL);
         hChn->agcCtl = *p;
         break;       
      
      case BAST_G3_CONFIG_BLIND_SCAN_MODES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_BLIND_SCAN_MODES);
         hChn->blindScanModes = *p;
         break;
      
      case BAST_G3_CONFIG_DTV_SCAN_CODE_RATES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DTV_SCAN_CODE_RATES);
         hChn->dtvScanCodeRates = *p;
         break;
      
      case BAST_G3_CONFIG_DVB_SCAN_CODE_RATES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DVB_SCAN_CODE_RATES);
         hChn->dvbScanCodeRates = *p;
         break;
      
      case BAST_G3_CONFIG_DCII_SCAN_CODE_RATES:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_DCII_SCAN_CODE_RATES);
         hChn->dciiScanCodeRates = *p;
         break;         
      
#ifndef BAST_EXCLUDE_STATUS_EVENTS
      case BAST_G3_CONFIG_STATUS_INDICATOR:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_STATUS_INDICATOR);
         hChn->statusEventIndicators = *p;
         break;
      
      case BAST_G3_CONFIG_FREQ_DRIFT_THRESHOLD:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_FREQ_DRIFT_THRESHOLD);
         hChn->freqDriftThreshold = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
      
      case BAST_G3_CONFIG_RAIN_FADE_THRESHOLD:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_RAIN_FADE_THRESHOLD);
         hChn->rainFadeThreshold = *p;
         break;
      
      case BAST_G3_CONFIG_RAIN_FADE_WINDOW:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_RAIN_FADE_WINDOW);   
         hChn->rainFadeWindow = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
         break;
#endif
      
#ifndef BAST_EXCLUDE_FTM
      case BAST_G3_CONFIG_FTM_TX_POWER:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_FTM_TX_POWER);
         hDev->ftmTxPower = *p & 0x1F;
         break;
      
      case BAST_G3_CONFIG_FTM_CH_SELECT:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_FTM_CH_SELECT);
         if (*p == BAST_G3_FskChannelConfig_eCh1Tx_Ch1Rx)
         {
            hDev->fskTxChannel = BAST_FskChannel_e1;
            hDev->fskRxChannel = BAST_FskChannel_e1;
         }
         else if (*p == BAST_G3_FskChannelConfig_eCh1Tx_Ch0Rx)
         {
            hDev->fskTxChannel = BAST_FskChannel_e1;
            hDev->fskRxChannel = BAST_FskChannel_e0;
         }
         else if (*p == BAST_G3_FskChannelConfig_eCh0Tx_Ch1Rx)
         {
            hDev->fskTxChannel = BAST_FskChannel_e0;
            hDev->fskRxChannel = BAST_FskChannel_e1;
         }
         else
         {
            hDev->fskTxChannel = BAST_FskChannel_e0;
            hDev->fskRxChannel = BAST_FskChannel_e0;
         }
         break;
#endif
      
      case BAST_G3_CONFIG_BCM3445_CTL:
         BDBG_ASSERT(len == BAST_G3_CONFIG_LEN_BCM3445_CTL);
#ifndef BAST_EXCLUDE_BCM3445
         hChn->bcm3445Ctl = *p & 0x7;
#endif
         break;
      
      default:
         BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
         break;
   }

   done:
   BDBG_LEAVE(BAST_g3_P_WriteConfig);    
   return retCode;
}


/******************************************************************************
 BAST_g3_P_AbortAcq() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_AbortAcq(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;   
   BERR_Code retCode;
   
   BDBG_ENTER(BAST_g3_P_AbortAcq);
        
   if (hChn->coresPoweredDown & BAST_CORE_SDS)
      return BAST_ERR_POWERED_DOWN;
   
   BAST_CHK_RETCODE(BAST_g3_P_DisableChannelInterrupts(h, false, false));
   BAST_CHK_RETCODE(BAST_g3_P_SdsDisableOif(h));

   BAST_g3_P_IndicateNotLocked(h);   
   hChn->acqTime = 0;
   hChn->acqState = BAST_AcqState_eIdle;
   hChn->reacqCtl &= (BAST_G3_CONFIG_REACQ_CTL_FREQ_DRIFT | BAST_G3_CONFIG_REACQ_CTL_RETUNE);

#if 0   
   if (hChn->bFsNotDefault)
      retCode = BAST_g3_P_SetDefaultSampleFreq(h);
#endif

   done:
   BDBG_LEAVE(BAST_g3_P_AbortAcq);
   return retCode;   
}


/******************************************************************************
 BAST_g3_P_GetPeakScanEventHandle()
******************************************************************************/
BERR_Code BAST_g3_P_GetPeakScanEventHandle(BAST_ChannelHandle h, BKNI_EventHandle *pEvent)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDBG_ENTER(BAST_g3_P_GetPeakScanEventHandle);
   
   *pEvent = ((BAST_g3_P_ChannelHandle *)h->pImpl)->hPeakScanEvent;
   
   BDBG_LEAVE(BAST_g3_P_GetPeakScanEventHandle);
   return retCode;
}


#ifndef BAST_EXCLUDE_STATUS_EVENTS   
/******************************************************************************
 BAST_g3_P_GetStatusEventHandle()
******************************************************************************/
BERR_Code BAST_g3_P_GetStatusEventHandle(BAST_ChannelHandle h, BKNI_EventHandle *pEvent)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDBG_ENTER(BAST_g3_P_GetStatusEventHandle);
   
   *pEvent = ((BAST_g3_P_ChannelHandle *)h->pImpl)->hStatusEvent;
   
   BDBG_LEAVE(BAST_g3_P_GetStatusEventHandle);
   return retCode;
}
#endif


/******************************************************************************
 BAST_g3_P_PeakScan() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g3_P_PeakScan(BAST_ChannelHandle h, uint32_t tunerFreq)
{
#ifndef BAST_EXCLUDE_PEAK_SCAN
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   
   BDBG_ENTER(BAST_g3_P_PeakScan);
   
   if (hDev->bInit != true)
      return (BERR_TRACE(BAST_ERR_AP_NOT_INIT));

#ifndef BAST_EXCLUDE_EXT_TUNER            
   if (hChn->bExternalTuner)
      return BERR_NOT_SUPPORTED;
#endif

   if ((tunerFreq < 200000000UL) || (tunerFreq > 2200000000UL))
      return (BERR_TRACE(BERR_INVALID_PARAMETER));
   
   if (hChn->coresPoweredDown & BAST_CORE_SDS)
      return BAST_ERR_POWERED_DOWN;
      
   BAST_CHK_RETCODE(BAST_g3_P_PrepareNewAcquisition(h));
   hChn->tunerFreq = tunerFreq;
   if ((hChn->peakScanSymRateMin == 0) || (hChn->peakScanSymRateMax == 0))
   {
      hChn->peakScanSymRateMin = hChn->peakScanSymRateMax = 0;
      hChn->acqParams.symbolRate = 20000000;
   }
   else
      hChn->acqParams.symbolRate = hChn->peakScanSymRateMax;
   hChn->acqParams.carrierFreqOffset = 0;
   hChn->acqParams.acq_ctl = BAST_ACQSETTINGS_DEFAULT;
   hChn->peakScanOutput = 0;
   hChn->signalDetectStatus = 0;   
   hChn->peakScanStatus = BAST_PEAK_SCAN_STATUS_ENABLED;
   
   /* complete tuning and acquisition in ISR context */
   BKNI_EnterCriticalSection();
   retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 30, BAST_g3_P_TuneAcquire_isr);
   BKNI_LeaveCriticalSection();
   
   done:
   BDBG_LEAVE(BAST_g3_P_PeakScan);   
   return retCode;   
#else
   BSTD_UNUSED(h);
   BSTD_UNUSED(tunerFreq);
   return BERR_NOT_SUPPORTED;
#endif
}


/******************************************************************************
 BAST_g3_P_GetPeakScanStatus()
******************************************************************************/
BERR_Code BAST_g3_P_GetPeakScanStatus(BAST_ChannelHandle h, BAST_PeakScanStatus *pStatus)
{
#ifndef BAST_EXCLUDE_PEAK_SCAN
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BDBG_ENTER(BAST_g3_P_GetPeakScanStatus);

   pStatus->status = 2; /* scan error */
   pStatus->tunerFreq = 0;
   pStatus->out = 0;
   pStatus->peakPower = 0;
   
   if (hChn->peakScanStatus & BAST_PEAK_SCAN_STATUS_ENABLED)
   {
      if (hChn->peakScanStatus & BAST_PEAK_SCAN_STATUS_DONE)
      {
         pStatus->status = 0; /* scan finished */
         pStatus->tunerFreq = hChn->tunerFreq;
         pStatus->out = hChn->peakScanOutput;
         pStatus->peakPower = hChn->peakScanMaxPower;         
      }
      else
         pStatus->status = 1; /* scan in progress */
   }
   
   BDBG_LEAVE(BAST_g3_P_GetPeakScanStatus);   
   return BERR_SUCCESS; 
#else
   return BERR_NOT_SUPPORTED;
#endif
}


#ifndef BAST_EXCLUDE_STATUS_EVENTS   
/******************************************************************************
 BAST_g3_P_EnableStatusInterrupts()
******************************************************************************/
BERR_Code BAST_g3_P_EnableStatusInterrupts(BAST_ChannelHandle h, bool bEnable)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BDBG_ENTER(BAST_g3_P_EnableStatusInterrupts);
   
   hChn->bStatusInterrupts = bEnable;
   
   BDBG_LEAVE(BAST_g3_P_EnableStatusInterrupts);
   return BERR_SUCCESS;
}
#endif


/******************************************************************************
 BAST_g3_ResetInterruptCounters()
******************************************************************************/
void BAST_g3_ResetInterruptCounters(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   int i;

   BKNI_EnterCriticalSection();   
   for (i = 0; i < BAST_g3_MaxIntID; i++)
      hChn->irqCount[i] = 0;
   BKNI_LeaveCriticalSection();
}


/******************************************************************************
 BAST_g3_GetInterruptCount()
******************************************************************************/
BERR_Code BAST_g3_GetInterruptCount(BAST_ChannelHandle h, BAST_g3_IntID idx, uint32_t *pCount)
{
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   if (idx >= BAST_g3_MaxIntID)
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);      
   }
   else
      *pCount = hChn->irqCount[idx];
   return retCode;
}


/******************************************************************************
 BAST_g3_P_EnableSpurCanceller()
******************************************************************************/
BERR_Code BAST_g3_P_EnableSpurCanceller(BAST_ChannelHandle h, uint8_t n, BAST_SpurCancellerConfig *pConfig)
{
#ifndef BAST_EXCLUDE_SPUR_CANCELLER
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t i;

   if ((n && (pConfig == NULL)) || (n > 6))
      return BERR_TRACE(BERR_INVALID_PARAMETER);

   BAST_g3_P_DisableSpurCanceller(h);

   if ((pConfig == NULL) || (n == 0))
   {
      BAST_g3_P_ClearSpurCancellerConfig(h);
      goto done;
   }

   for (i = 0; i < 6; i++)
   {
      if (n <= i)
         hChn->spurConfig[i].freq = 0;
      else
         hChn->spurConfig[i].freq = pConfig[i].freq;
   }

   done:
   return BERR_SUCCESS;
#else
   BSTD_UNUSED(h);
   BSTD_UNUSED(n);
   BSTD_UNUSED(pConfig);
   return BERR_NOT_SUPPORTED;
#endif  
}


/******************************************************************************
 BAST_g3_P_SetSearchRange()
******************************************************************************/
BERR_Code BAST_g3_P_SetSearchRange(BAST_Handle h, uint32_t searchRange)
{
   BAST_g3_P_Handle *hDev = (BAST_g3_P_Handle *)(h->pImpl);
   
   hDev->searchRange = searchRange;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetSearchRange()
******************************************************************************/
BERR_Code BAST_g3_P_GetSearchRange(BAST_Handle h, uint32_t *pSearchRange)
{
   BAST_g3_P_Handle *hDev = (BAST_g3_P_Handle *)(h->pImpl);
   
   *pSearchRange = hDev->searchRange;
   return BERR_SUCCESS;
}


#ifndef BAST_EXCLUDE_LDPC
/******************************************************************************
 BAST_g3_P_SetAmcScramblingSeq()
******************************************************************************/
BERR_Code BAST_g3_P_SetAmcScramblingSeq(BAST_ChannelHandle h, uint32_t xseed, uint32_t plhdrscr1, uint32_t plhdrscr2, uint32_t plhdrscr3)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   hChn->ldpcScramblingSeq[0] = (uint8_t)((xseed >> 24) & 0xFF);
   hChn->ldpcScramblingSeq[1] = (uint8_t)((xseed >> 16) & 0xFF);
   hChn->ldpcScramblingSeq[2] = (uint8_t)((xseed >> 8) & 0xFF);
   hChn->ldpcScramblingSeq[3] = (uint8_t)(xseed & 0xFF);
   hChn->ldpcScramblingSeq[4] = (uint8_t)((plhdrscr1 >> 24) & 0xFF);
   hChn->ldpcScramblingSeq[5] = (uint8_t)((plhdrscr1 >> 16) & 0xFF);
   hChn->ldpcScramblingSeq[6] = (uint8_t)((plhdrscr1 >> 8) & 0xFF);
   hChn->ldpcScramblingSeq[7] = (uint8_t)(plhdrscr1 & 0xFF);
   hChn->ldpcScramblingSeq[8] = (uint8_t)((plhdrscr2 >> 24) & 0xFF);
   hChn->ldpcScramblingSeq[9] = (uint8_t)((plhdrscr2 >> 16) & 0xFF);
   hChn->ldpcScramblingSeq[10] = (uint8_t)((plhdrscr2 >> 8) & 0xFF);
   hChn->ldpcScramblingSeq[11] = (uint8_t)(plhdrscr2 & 0xFF);
   hChn->ldpcScramblingSeq[12] = (uint8_t)((plhdrscr3 >> 24) & 0xFF);
   hChn->ldpcScramblingSeq[13] = (uint8_t)((plhdrscr3 >> 16) & 0xFF);
   hChn->ldpcScramblingSeq[14] = (uint8_t)((plhdrscr3 >> 8) & 0xFF);
   hChn->ldpcScramblingSeq[15] = (uint8_t)(plhdrscr3 & 0xFF);
   return BERR_SUCCESS;
}
#endif


/******************************************************************************
 BAST_g3_P_SetTunerFilter()
******************************************************************************/
BERR_Code BAST_g3_P_SetTunerFilter(BAST_ChannelHandle h, uint32_t cutoffHz)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
 
   if (cutoffHz == 0)
      hChn->tunerCtl &= ~BAST_G3_CONFIG_TUNER_CTL_SET_FILTER_MANUAL;      
   else
   {
      hChn->tunerCutoff = (uint8_t)((cutoffHz + 500000) / 1000000); /* round to nearest MHz */
      hChn->tunerCtl |= BAST_G3_CONFIG_TUNER_CTL_SET_FILTER_MANUAL;
   }
 
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetSignalDetectStatus()
******************************************************************************/
BERR_Code BAST_g3_P_GetSignalDetectStatus(BAST_ChannelHandle h, BAST_SignalDetectStatus *pStatus)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   pStatus->bEnabled = pStatus->bDone = pStatus->bTimingLoopLocked = false;
   if (hChn->signalDetectStatus & BAST_SIGNAL_DETECT_ENABLED)
   {
      pStatus->bEnabled = true;
      if ((hChn->signalDetectStatus & BAST_SIGNAL_DETECT_IN_PROGRESS) == 0)
      {
         pStatus->bDone = true;
         if (hChn->signalDetectStatus & BAST_SIGNAL_DETECT_TIMING_LOCK)
            pStatus->bTimingLoopLocked = true;
      }
   }
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_SetOutputTransportSettings()
******************************************************************************/
BERR_Code BAST_g3_P_SetOutputTransportSettings(BAST_ChannelHandle h, BAST_OutputTransportSettings *pSettings)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   hChn->xportSettings = *pSettings;  
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetOutputTransportSettings()
******************************************************************************/
BERR_Code BAST_g3_P_GetOutputTransportSettings(BAST_ChannelHandle h, BAST_OutputTransportSettings *pSettings)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   *pSettings = hChn->xportSettings;      
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_SetDiseqcSettings()
******************************************************************************/
BERR_Code BAST_g3_P_SetDiseqcSettings(BAST_ChannelHandle h, BAST_DiseqcSettings *pSettings)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   if (hChn->bHasDiseqc)
   {
      hChn->diseqc->dsecSettings = *pSettings;  
      return BERR_SUCCESS;
   }
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_g3_P_GetDiseqcSettings()
******************************************************************************/
BERR_Code BAST_g3_P_GetDiseqcSettings(BAST_ChannelHandle h, BAST_DiseqcSettings *pSettings)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   if (hChn->bHasDiseqc)
   {
      *pSettings = hChn->diseqc->dsecSettings;      
      return BERR_SUCCESS;
   }
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_g3_GetTraceBuffer()
******************************************************************************/
BERR_Code BAST_g3_GetTraceBuffer(BAST_ChannelHandle h, uint32_t *pBuffer)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   if (pBuffer == NULL)
      return (BERR_TRACE(BERR_INVALID_PARAMETER)); 

   BKNI_Memcpy((void*)pBuffer, hChn->trace, BAST_TraceEvent_eMax * sizeof(uint32_t));
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_SetNetworkSpec()
******************************************************************************/
BERR_Code BAST_g3_P_SetNetworkSpec(BAST_Handle h, BAST_NetworkSpec s)
{
   if (s > BAST_NetworkSpec_eEuro)
      return BERR_INVALID_PARAMETER;

   h->settings.networkSpec = s;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetNetworkSpec()
******************************************************************************/
BERR_Code BAST_g3_P_GetNetworkSpec(BAST_Handle h, BAST_NetworkSpec *p)
{
   *p = h->settings.networkSpec;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_SetFskChannel()
******************************************************************************/
BERR_Code BAST_g3_P_SetFskChannel(BAST_Handle h, BAST_FskChannel fskTxChannel, BAST_FskChannel fskRxChannel)
{
   BAST_g3_P_Handle *hDev = (BAST_g3_P_Handle *)(h->pImpl);
   
   if ((fskTxChannel > BAST_FskChannel_e1) || (fskRxChannel > BAST_FskChannel_e1))
      return BERR_INVALID_PARAMETER;
   
   hDev->fskTxChannel = fskTxChannel;
   hDev->fskRxChannel = fskRxChannel;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetFskChannel()
******************************************************************************/
BERR_Code BAST_g3_P_GetFskChannel(BAST_Handle h, BAST_FskChannel *fskTxChannel, BAST_FskChannel *fskRxChannel)
{
   BAST_g3_P_Handle *hDev = (BAST_g3_P_Handle *)(h->pImpl);
   
   *fskTxChannel = hDev->fskTxChannel;
   *fskRxChannel = hDev->fskRxChannel;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_SetPeakScanSymbolRateRange()
******************************************************************************/  
BERR_Code BAST_g3_P_SetPeakScanSymbolRateRange(
   BAST_ChannelHandle h,             /* [in] BAST channel handle */
   uint32_t           minSymbolRate, /* [in] minimum symbol rate in sym/sec */
   uint32_t           maxSymbolRate  /* [in] maximum symbol rate in sym/sec */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   if ((minSymbolRate == 0) || (maxSymbolRate == 0))
   {
      minSymbolRate = 0;
      maxSymbolRate = 0;
   }
   else if ((minSymbolRate < 1000000) || (maxSymbolRate < 1000000) || 
       (minSymbolRate > 45000000) || (maxSymbolRate > 45000000) ||
       (minSymbolRate > maxSymbolRate))
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   hChn->peakScanSymRateMin = minSymbolRate;
   hChn->peakScanSymRateMax = maxSymbolRate;
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetPeakScanSymbolRateRange()
******************************************************************************/  
BERR_Code BAST_g3_P_GetPeakScanSymbolRateRange(
   BAST_ChannelHandle h,               /* [in] BAST channel handle */
   uint32_t           *pMinSymbolRate, /* [out] minimum symbol rate in sym/sec */
   uint32_t           *pMaxSymbolRate  /* [out] maximum symbol rate in sym/sec */
)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   if ((pMinSymbolRate == NULL) || (pMaxSymbolRate == NULL))
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   *pMinSymbolRate = hChn->peakScanSymRateMin;
   *pMaxSymbolRate = hChn->peakScanSymRateMax;
   return BERR_SUCCESS;
}


#ifndef BAST_HAS_LEAP
/******************************************************************************
 BAST_g3_P_GetVersionInfo()
******************************************************************************/  
BERR_Code BAST_g3_P_GetVersionInfo(BAST_Handle h, BFEC_VersionInfo *pVersion)
{
   BSTD_UNUSED(h);

   pVersion->majorVersion = BAST_API_VERSION;
   pVersion->minorVersion = BAST_G3_RELEASE_VERSION;
   pVersion->buildId = BAST_G3_BUILD_VERSION;
   pVersion->buildType = 0;
   return BERR_SUCCESS;
}
#endif

