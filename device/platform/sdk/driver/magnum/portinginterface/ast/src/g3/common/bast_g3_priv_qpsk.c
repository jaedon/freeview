/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv_qpsk.c $
 * $brcm_Revision: Hydra_Software_Devel/55 $
 * $brcm_Date: 12/18/12 10:43a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv_qpsk.c $
 * 
 * Hydra_Software_Devel/55   12/18/12 10:43a enavarro
 * SWSATFE-174: add getLockStatusFunct in channel handle struct
 * 
 * Hydra_Software_Devel/54   12/12/12 3:14p enavarro
 * SWSATFE-234: added dynamic power down of SDS_OPLL
 * 
 * Hydra_Software_Devel/53   11/15/12 10:11a ckpark
 * SWSATFE-230: SNORECTL.alpha is set, starts from REVID=63 for Legacy
 * modes.
 * 
 * Hydra_Software_Devel/52   9/20/12 9:09a enavarro
 * SWSATFE-174: close loop in the receiver (changed CLCTL1 for 4538)
 * 
 * Hydra_Software_Devel/51   9/11/12 11:09a enavarro
 * SWSATFE-174: fixed snore programming for sds revid 0x64 and later
 * 
 * Hydra_Software_Devel/50   9/10/12 4:08p enavarro
 * SWSATFE-174: implement BAST_ACQSETTINGS_RS_DISABLE; retain default
 * setting for bits 19:16 in SDS_CL_CLCTL1
 * 
 * Hydra_Software_Devel/49   7/5/12 4:09p enavarro
 * SWSATFE-174: changes for 4538
 * 
 * Hydra_Software_Devel/48   3/26/12 10:23a ckpark
 * SWSATFE-140: updated legacy QPSK SNORE settings for 4517
 * 
 * Hydra_Software_Devel/47   3/16/12 4:21p enavarro
 * SWSATFE-140: added reacqCause to channel handle struct
 * 
 * Hydra_Software_Devel/46   2/17/12 3:02p ronchan
 * SWSATFE-137: optimized snr averaging filter for A1 chips
 * 
 * Hydra_Software_Devel/45   1/27/12 2:30p enavarro
 * SWSATFE-140: move tracking plc to onStableLock funct
 * 
 * Hydra_Software_Devel/44   11/10/11 11:01a guangcai
 * SWSATFE-75: changed carrier loop BW and delays for low symbol rate
 * carrrier search algorithm
 * 
 * Hydra_Software_Devel/43   10/31/11 3:15p ckpark
 * SWSATFE-153: fixed spectral inversion scan
 * 
 * Hydra_Software_Devel/42   10/28/11 2:02p ronchan
 * SW7346-524: fixed coverity defect for unchecked return values
 * 
 * Hydra_Software_Devel/41   9/29/11 10:42a ckpark
 * SWSATFE-137: updated SNORE alpha value for B0
 * 
 * Hydra_Software_Devel/40   8/10/11 10:57a enavarro
 * SWSATFE-86: reworked snr filtering
 * 
 * Hydra_Software_Devel/39   8/2/11 5:40p ronchan
 * SW7346-389: fix coverity defect 32702
 * 
 * Hydra_Software_Devel/38   7/29/11 11:11a guangcai
 * SWSATFE-137: implemented SNORE filtering; enabled SNORE for all g3
 * chips
 * 
 * Hydra_Software_Devel/37   7/27/11 3:14p guangcai
 * SWSATFE-136: rollback previous changes
 * 
 * Hydra_Software_Devel/36   7/27/11 11:35a guangcai
 * SWSATFE-136: updates for low baud rate acquisition
 * 
 * Hydra_Software_Devel/35   7/19/11 11:51a enavarro
 * SW7346-331: fixed unchecked return values
 * 
 * Hydra_Software_Devel/34   5/27/11 3:00p enavarro
 * SWSATFE-86: put in timer workaround for 4528
 * 
 * Hydra_Software_Devel/33   4/29/11 2:58p enavarro
 * SWSATFE-86: added support for undersample mode
 * 
 * Hydra_Software_Devel/32   4/5/11 4:04p ckpark
 * SWSATFE-75: Legacy EQ mu setting is changed
 * 
 * Hydra_Software_Devel/31   3/31/11 3:43p ckpark
 * SWSATFE-75: Legacy acquisition BW is adjusted, baud loop BW is fixed
 * 
 * Hydra_Software_Devel/30   3/4/11 3:45p enavarro
 * SWSATFE-86: fix compiler error on BCM4528
 * 
 * Hydra_Software_Devel/29   2/23/11 4:01p ronchan
 * SWSATFE-75: use real time status for spinv scan, revert previous
 * bandwidth optimizations
 * 
 * Hydra_Software_Devel/28   2/10/11 2:10p ckpark
 * SWSATFE-75: Legacy acquisition BW is adjusted
 * 
 * Hydra_Software_Devel/27   1/26/11 4:47p enavarro
 * SWSATFE-75: removed debug message
 * 
 * Hydra_Software_Devel/26   1/6/11 9:45a enavarro
 * SWSATFE-75: updated acquisition carrier bw for 20MBaud; reworked
 * viterbi lock detection
 * 
 * Hydra_Software_Devel/25   12/22/10 11:24a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/24   12/21/10 4:20p enavarro
 * SWSATFE-75: don't double carrierDelay in BAST_g3_P_QpskLockViterbi()
 * 
 * Hydra_Software_Devel/23   12/21/10 4:04p enavarro
 * SWSATFE-75: made changes to get legacy qpsk to lock
 * 
 * Hydra_Software_Devel/22   12/17/10 4:57p ckpark
 * SWSATFE-75: fixed bugs in BAST_g3_P_QpskSetMode()
 * 
 * Hydra_Software_Devel/21   12/6/10 9:53a ronchan
 * SWSATFE-75: use same bit define for vit_in_sync mask
 * 
 * Hydra_Software_Devel/20   11/29/10 11:08a ronchan
 * SWSATFE-10: retain bit 4 of CLCTL2 for oqpsk
 * 
 * Hydra_Software_Devel/19   10/18/10 3:37p ronchan
 * SWSATFE-75: updated legacy bandwidths, check real time viterbi lock
 * status
 * 
 * Hydra_Software_Devel/18   10/14/10 5:31p ronchan
 * SWSATFE-75: fixed compile error
 * 
 * Hydra_Software_Devel/17   10/14/10 5:21p ronchan
 * SWSATFE-75: added trace events
 * 
 * Hydra_Software_Devel/16   9/30/10 2:03p enavarro
 * SWSATFE-75: store transport settings in BAST_OutputTransportSettings
 * struct in the channel handle; removed xportCtl channel handle member
 * 
 * Hydra_Software_Devel/15   9/29/10 3:51p enavarro
 * SWSATFE-80: renamed bit definitions for xportCtl
 * 
 * Hydra_Software_Devel/14   9/27/10 3:46p ronchan
 * SWSATFE-75: optimize QpskInitializeLoopParameters()
 * 
 * Hydra_Software_Devel/13   9/24/10 10:13a ronchan
 * SWSATFE-75: fix compile error
 * 
 * Hydra_Software_Devel/12   9/24/10 10:11a ronchan
 * SWSATFE-75: clean up code in QpskAcquire()
 * 
 * Hydra_Software_Devel/11   9/24/10 10:08a ronchan
 * SWSATFE-75: implement script_qpsk_1
 * 
 * Hydra_Software_Devel/10   9/23/10 6:21p ronchan
 * SWSATFE-75: implement narrow bw, final bw, set opll functions
 * 
 * Hydra_Software_Devel/9   9/22/10 5:54p ronchan
 * SWSATFE-75: implemented spinv scan
 * 
 * Hydra_Software_Devel/8   9/22/10 5:37p ronchan
 * SWSATFE-75: implemented lock viterbi algorithm
 * 
 * Hydra_Software_Devel/7   9/21/10 10:30a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/6   9/20/10 5:26p ronchan
 * SWSATFE-75: fixed compile errors
 * 
 * Hydra_Software_Devel/5   9/20/10 5:10p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/4   9/17/10 5:27p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/3   9/14/10 2:29p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/2   9/8/10 10:35a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/1   8/31/10 1:57p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"

BDBG_MODULE(bast_g3_priv_qpsk);

#define BAST_DEBUG_QPSK(x) /* x */

/* bit definitions for dtvScanState */
#define BAST_G3_DTV_SCAN_ON  0x80
#define BAST_G3_DTV_SCAN_1_2 0x01
#define BAST_G3_DTV_SCAN_OFF 0x00

/* local function prototypes */
BERR_Code BAST_g3_P_QpskInitializeLoopParameters(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskSetMode(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskLockViterbi(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskSetActualMode(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskSetVcos(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskSpInvScan(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskSpinvScan1(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskNarrowBw(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskNarrowBw1(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskAcquire1(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskSetFinalFlBw(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskSetFinalBlBw(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskSetOqpsk(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_QpskSetOpll(BAST_ChannelHandle h);


/* private functions */
/******************************************************************************
 BAST_g3_P_QpskAcquire() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_QpskAcquire(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   
   if (hChn->acqParams.mode == BAST_Mode_eDss_scan)
      hChn->dtvScanState = BAST_G3_DTV_SCAN_ON;
   else
      hChn->dtvScanState = BAST_G3_DTV_SCAN_OFF;
    
   /* set qpsk mode and init bert */
   BAST_g3_P_QpskSetMode(h);
   BAST_g3_P_InitBert(h);

   BAST_g3_P_QpskInitializeLoopParameters(h);
   BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eStartViterbi));

   hChn->funct_state = 0;
   return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, 20000, BAST_g3_P_QpskLockViterbi);
   
   done:
   return retCode;
}

/******************************************************************************
 BAST_g3_P_QpskUpdateErrorCounters()
******************************************************************************/
BERR_Code BAST_g3_P_QpskUpdateErrorCounters(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_FEC_FERR, &val);
   hChn->rsCorr += ((val >> 16) & 0xFFFF);
   hChn->rsUncorr += (val & 0xFFFF);

   /* get pre-Viterbi error count */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_VIT_VTCTL, 0x00008000); /* take snapshot of VRCV+VREC */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_VIT_VREC, &val);
   hChn->preVitErrors += val;
   
   BAST_g3_P_UpdateErrorCounters(h);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_QpskCheckMode() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_QpskCheckMode(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_Mode acq_mode = hChn->acqParams.mode;
   uint8_t base, idx, enable_mask, mode_mask;
   
   if ((acq_mode == BAST_Mode_eDss_scan) || (acq_mode == BAST_Mode_eDvb_scan) ||
       (acq_mode == BAST_Mode_eDcii_scan))
   {
      if (BAST_MODE_IS_DVB(hChn->actualMode))
      {
         base = BAST_Mode_eDvb_1_2;
         enable_mask = hChn->dvbScanCodeRates; 
      }
      else if (BAST_MODE_IS_DTV(hChn->actualMode))
      {
         base = BAST_Mode_eDss_1_2;
         enable_mask = hChn->dtvScanCodeRates;
      }
      else
      {
         base = BAST_Mode_eDcii_1_2;
         enable_mask = hChn->dciiScanCodeRates;
      }
      idx = hChn->actualMode - base;
      mode_mask = 1 << idx;
      if ((mode_mask & enable_mask) == 0)
      {
         hChn->reacqCause = BAST_ReacqCause_eInvalidMode;
         hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE; /* invalid mode */
      }
   }

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_DisableFreqDetector()
******************************************************************************/
BERR_Code BAST_g3_P_DisableFreqDetector(BAST_ChannelHandle h)
{
#if 0   
   uint32_t val;
#endif

   BAST_DEBUG_QPSK(BDBG_MSG(("disabling freq detector")));
#if 0   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_CLCTL1, &val);
   val &= 0xFF00FFFF;
   val |= 0x000A0000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLCTL1, &val);
#else  
   BAST_g3_P_AndRegister(h, BCHP_SDS_CL_CLCTL1, ~0x00400000);
#endif   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_QpskGetRTLockStatus
******************************************************************************/
void BAST_g3_P_QpskGetRTLockStatus(BAST_ChannelHandle h, bool *pbLocked)
{
   uint32_t val;

   BAST_g3_P_ReadRegister(h, BCHP_SDS_MISC_INTR_RAW_STS0, &val);
   if ((val & 0x1B) == 0x09)
      *pbLocked = true;
   else
      *pbLocked = false;      
}


/******************************************************************************
 BAST_g3_P_QpskOnLock() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_QpskOnLock(BAST_ChannelHandle h)
{
#if (BCHP_CHIP!=4528)
   /* disable freq detector 30 msecs after lock */   
   BAST_DEBUG_QPSK(BDBG_MSG(("starting 30 msec timer for freq detector disable")));
   return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 30000, BAST_g3_P_DisableFreqDetector);
#else
   return BERR_SUCCESS;
#endif
}


/******************************************************************************
 BAST_g3_P_QpskOnLostLock() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_QpskOnLostLock(BAST_ChannelHandle h)
{
#if 0   
   uint32_t val;
#endif

   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eBaudUsec);
   
   /* re-enable frequency detector */
#if 0   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_CLCTL1, &val);
   val &= 0xFF00FFFF;
   val |= 0x004A0000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLCTL1, &val);
#else
   BAST_g3_P_OrRegister(h, BCHP_SDS_CL_CLCTL1, 0x00400000);
#endif

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_QpskOnStableLock() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_QpskOnStableLock(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_Mode prev_mode = hChn->actualMode;
   BAST_Mode curr_mode;
   BERR_Code retCode;

   if (hChn->bPlcTracking == false)
      BAST_g3_P_ConfigPlc(h, false); /* set tracking PLC */   

#if (BCHP_CHIP==4528) || (BCHP_CHIP==4538)
   BAST_g3_P_DisableFreqDetector(h);
#endif

   BAST_CHK_RETCODE(BAST_g3_P_GetActualMode(h, &curr_mode));
   if (prev_mode != curr_mode)
   {
      /* different code rate now detected */
      BDBG_MSG(("diff code rate detected"));
      hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE;
      hChn->reacqCause = BAST_ReacqCause_eCodeRateChanged;
   }
   else if (hChn->bEverStableLock == false)
   {
      BAST_g3_P_QpskUpdateErrorCounters(h);
      hChn->rsCorr = hChn->rsUncorr = 0;
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_QpskEnableLockInterrupts()
******************************************************************************/
BERR_Code BAST_g3_P_QpskEnableLockInterrupts(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BINT_EnableCallback_isr(hChn->hQpskLockCb);
   BINT_EnableCallback_isr(hChn->hQpskNotLockCb);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_QpskDisableLockInterrupts()
******************************************************************************/
BERR_Code BAST_g3_P_QpskDisableLockInterrupts(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BINT_DisableCallback_isr(hChn->hQpskLockCb);
   BINT_DisableCallback_isr(hChn->hQpskNotLockCb);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_QpskSetFunctTable()
******************************************************************************/ 
BERR_Code BAST_g3_P_QpskSetFunctTable(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   hChn->acqFunct = BAST_g3_P_QpskAcquire;
   hChn->checkModeFunct = BAST_g3_P_QpskCheckMode;
   hChn->onLockFunct = BAST_g3_P_QpskOnLock;
   hChn->onLostLockFunct = BAST_g3_P_QpskOnLostLock;
   hChn->onStableLockFunct = BAST_g3_P_QpskOnStableLock;
   hChn->onMonitorLockFunct = NULL;
   hChn->enableLockInterrupts = BAST_g3_P_QpskEnableLockInterrupts;
   hChn->disableLockInterrupts = BAST_g3_P_QpskDisableLockInterrupts;
   hChn->getLockStatusFunct = BAST_g3_P_QpskGetRTLockStatus;
   
   return BERR_SUCCESS;
}


/* local functions */
/******************************************************************************
 BAST_g3_P_QpskInitializeLoopParameters()
******************************************************************************/
BERR_Code BAST_g3_P_QpskInitializeLoopParameters(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t carrier_acq1_bw, carrier_acq2_bw, carrier_acq3_bw, carrier_trk_bw, baud_acq_bw, baud_trk_bw;
   uint32_t Fb, P_hi, P_lo, Q_hi;
   uint8_t idx = 0;
   
   /* generated 10-18-2010 10:29:22 */
   /* BW scale = 2097152 (BW is 2 bytes), damp scale = 4 (damp is 1 byte) */
   static const uint16_t qpsk_bw[7][6] = 
   {
      /* carrierAcq1, carrierAcq2, carrierAcq3, carrierTrk, baudAcq, baudTrk */
      {0x51EC, 0x051F, 0x03B0, 0x03B0, 0x49BA, 0x0EBF},  /* 0 <= Fb < 2000000 */
      {0x71EC, 0x0C3D, 0x0819, 0x0619, 0x99BA, 0x1A31},  /* 2000000 <= Fb < 4000000 */
      {0x71EC, 0x083D, 0x0219, 0x0119, 0x49BA, 0x0831},  /* 4000000 <= Fb < 12000000 */
      {0x71EC, 0x083D, 0x080C, 0x0619, 0x49BA, 0x0831},  /* 12000000 <= Fb < 17000000 */
      {0x71EC, 0x0C3D, 0x0C3D, 0x0C3D, 0x49BA, 0x0831},  /* 17000000 <= Fb < 23000000 */
      {0x71EC, 0x0C3D, 0x0C3D, 0x0C3D, 0x49BA, 0x0831},  /* 23000000 <= Fb < 27000000 */
      {0x71EC, 0x0C3D, 0x0C3D, 0x0C3D, 0x49BA, 0x0831}   /* 27000000 <= Fb < 45000000 */
   };  

   static const uint8_t qpsk_damp[7][6] =
   {
      /* carrierAcq1, carrierAcq2, carrierAcq3, carrierTrk, baudAcq, baudTrk */
      {0x0C, 0x08, 0x18, 0x18, 0x0C, 0x0C},  /* 0 <= Fb < 2000000 */
      {0x0C, 0x08, 0x18, 0x18, 0x08, 0x08},  /* 2000000 <= Fb < 4000000 */
      {0x0C, 0x08, 0x18, 0x18, 0x04, 0x04},  /* 4000000 <= Fb < 12000000 */
      {0x0C, 0x08, 0x18, 0x18, 0x04, 0x04},  /* 12000000 <= Fb < 17000000 */
      {0x0C, 0x08, 0x08, 0x08, 0x0C, 0x0C},  /* 17000000 <= Fb < 23000000 */
      {0x0C, 0x08, 0x18, 0x18, 0x08, 0x04},  /* 23000000 <= Fb < 27000000 */
      {0x0C, 0x08, 0x18, 0x18, 0x04, 0x04}   /* 27000000 <= Fb < 45000000 */
   };

   /* determine symbol rate range */
   Fb = hChn->acqParams.symbolRate;
   if (Fb >= 27000000)
      idx = 6;
   else if (Fb >= 23000000)
      idx = 5;
   else if (Fb >= 17000000)
      idx = 4;
   else if (Fb >= 12000000)
      idx = 3;
   else if (Fb >= 4000000)
      idx = 2;
   else if (Fb >= 2000000)
      idx = 1;
   else
      idx = 0;
   
   /* initialize baud/carrier loop parameters based on symbol rate */
   carrier_acq1_bw = qpsk_bw[idx][0]; 
   carrier_acq2_bw = qpsk_bw[idx][1]; 
   carrier_acq3_bw = qpsk_bw[idx][2]; 
   carrier_trk_bw = qpsk_bw[idx][3]; 
   baud_acq_bw = qpsk_bw[idx][4]; 
   baud_trk_bw = qpsk_bw[idx][5]; 

   /* bw scaled 2^21 = 2097152 */
   BAST_MultU32U32(carrier_acq1_bw, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 2097152, &Q_hi, &carrier_acq1_bw);
   BAST_MultU32U32(carrier_acq2_bw, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 2097152, &Q_hi, &carrier_acq2_bw);
   BAST_MultU32U32(carrier_acq3_bw, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 2097152, &Q_hi, &carrier_acq3_bw);
   BAST_MultU32U32(carrier_trk_bw, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 2097152, &Q_hi, &carrier_trk_bw);
   BAST_MultU32U32(baud_acq_bw, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 2097152, &Q_hi, &baud_acq_bw);
   BAST_MultU32U32(baud_trk_bw, Fb, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 2097152, &Q_hi, &baud_trk_bw);
   
   hChn->carrierAcq1Bw = carrier_acq1_bw;
   hChn->carrierAcq2Bw = carrier_acq2_bw;
   hChn->carrierAcq3Bw = carrier_acq3_bw;
   hChn->carrierTrkBw = carrier_trk_bw;
   hChn->baudAcqBw = baud_acq_bw;
   hChn->baudTrkBw = baud_trk_bw;
   
   hChn->carrierAcq1Damp = qpsk_damp[idx][0];
   hChn->carrierAcq2Damp = qpsk_damp[idx][1];
   hChn->carrierAcq3Damp = qpsk_damp[idx][2];
   hChn->carrierTrkDamp = qpsk_damp[idx][3];
   hChn->baudAcqDamp = qpsk_damp[idx][4];
   hChn->baudTrkDamp = qpsk_damp[idx][5];

#if 0
   BDBG_MSG(("carrierAcq1Bw=%d, carrierAcq1Damp=%d", hChn->carrierAcq1Bw, hChn->carrierAcq1Damp));
   BDBG_MSG(("carrierAcq2Bw=%d, carrierAcq2Damp=%d", hChn->carrierAcq2Bw, hChn->carrierAcq2Damp));
   BDBG_MSG(("carrierAcq3Bw=%d, carrierAcq3Damp=%d", hChn->carrierAcq3Bw, hChn->carrierAcq3Damp));
   BDBG_MSG(("carrierTrkBw=%d, carrierTrkDamp=%d", hChn->carrierTrkBw, hChn->carrierTrkDamp));
   BDBG_MSG(("baudAcqBw=%d, baudAcqDamp=%d", hChn->baudAcqBw, hChn->baudAcqDamp));    
   BDBG_MSG(("baudTrkBw=%d, baudTrkDamp=%d", hChn->baudTrkBw, hChn->baudTrkDamp));    
#endif
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_QpskSetMode()
******************************************************************************/
BERR_Code BAST_g3_P_QpskSetMode(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t vtctl, fmod, val;
   BERR_Code retCode;
   
   static const uint32_t script_qpsk_dcii[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_VIT_V76, 0x0B000A00),   /* 7/8 and 5/6 */
      BAST_SCRIPT_WRITE(BCHP_SDS_VIT_V54, 0x08000700),   /* 4/5 and 3/4 */
      BAST_SCRIPT_WRITE(BCHP_SDS_VIT_V32, 0x04000500),   /* 2/3 and 3/5 */
      BAST_SCRIPT_WRITE(BCHP_SDS_VIT_V10, 0x04500400),   /* 1/2 and 5/11 */
      BAST_SCRIPT_WRITE(BCHP_SDS_VIT_VINT, 0x25800000),  /* integration period */
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FRS, 0x0a0BFFFE),
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FSYN, 0x02000805),
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_qpsk_dvb_dtv[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_VIT_V76, 0x00E600E6),
      BAST_SCRIPT_WRITE(BCHP_SDS_VIT_V54, 0x09000880),
      BAST_SCRIPT_WRITE(BCHP_SDS_VIT_V32, 0x08660780),
      BAST_SCRIPT_WRITE(BCHP_SDS_VIT_V10, 0x06000480),
      BAST_SCRIPT_WRITE(BCHP_SDS_VIT_VINT, 0x27E70000),
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FRS, 0x020B2523),
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FSYN, 0x02000805),
      BAST_SCRIPT_EXIT
   };
   
   static const uint8_t vtctl_init[0x13] =
   {
      0xF0, /* BAST_Mode_eDvb_scan */
      0xE0, /* BAST_Mode_eDvb_1_2 */
      0xE1, /* BAST_Mode_eDvb_2_3 */
      0xE2, /* BAST_Mode_eDvb_3_4 */
      0xE3, /* BAST_Mode_eDvb_5_6 */
      0xE5, /* BAST_Mode_eDvb_7_8 */
      0xF1, /* BAST_Mode_eDss_scan */
      0xE0, /* BAST_Mode_eDss_1_2 */
      0xE1, /* BAST_Mode_eDss_2_3 */
      0xE4, /* BAST_Mode_eDss_6_7 */
      0x12, /* BAST_Mode_eDcii_scan */
      0x09, /* BAST_Mode_eDcii_1_2 */
      0x0B, /* BAST_Mode_eDcii_2_3 */
      0x0C, /* BAST_Mode_eDcii_3_4 */
      0x0E, /* BAST_Mode_eDcii_5_6 */
      0x0F, /* BAST_Mode_eDcii_7_8 */
      0x08, /* BAST_Mode_eDcii_5_11 */
      0x0A, /* BAST_Mode_eDcii_3_5 */
      0x0D, /* BAST_Mode_eDcii_4_5 */
   };
    
   if (BAST_MODE_IS_DCII(hChn->acqParams.mode))
   {
      /* DCII */
      BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_qpsk_dcii));
   }
   else if (BAST_MODE_IS_LEGACY_QPSK(hChn->acqParams.mode))
   {
      /* DVB or DTV */  
      BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_qpsk_dvb_dtv));  
   }
   else
   {
      /* non-legacy-qpsk mode */
      return BERR_INVALID_PARAMETER;
   }
   
   vtctl = (uint32_t)vtctl_init[hChn->acqParams.mode];
   if (BAST_MODE_IS_DCII(hChn->acqParams.mode))
   {
      /* DCII */
      if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_DCII_SPLIT)
      {
         vtctl |= 0x0400;
         if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_DCII_SPLIT_Q)
         {
            vtctl |= 0xA0; /* split Q */
            vtctl &= ~0x40;
         }
         else
         {
            vtctl |= 0x80; /* split I */
            vtctl &= ~0x60;
         }
      }
      else
         vtctl |= 0xE0; /*combine */
   }

   if ((hChn->acqParams.mode < BAST_Mode_eDcii_scan) ||
       ((hChn->acqParams.mode >= BAST_Mode_eDcii_scan) && ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_DCII_SPLIT) == 0)))
   {
      vtctl &= ~0x60;
      vtctl |= ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_SPINV_IQ_SCAN) >> 8);
   }

   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK)
   {
      vtctl |= 0x0200;
      BAST_g3_P_OrRegister(h, BCHP_SDS_EQ_EQMISCCTL, 0x00020000);
      BAST_g3_P_OrRegister(h, BCHP_SDS_CL_CLCTL2, 0x00000010);
   }
   
   BAST_g3_P_WriteRegister(h, BCHP_SDS_VIT_VTCTL, &vtctl);

   /* reset viterbi decoder */
   BAST_g3_P_OrRegister(h, BCHP_SDS_VIT_VTCTL, 0x00004000);

   val = 0x19;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_INTR2_0_CPU_CLEAR, &val);
   
   BAST_g3_P_AndRegister(h, BCHP_SDS_VIT_VTCTL, ~0x00004000);   
   
   /* set FMOD */
   fmod = hChn->stuffBytes & 0x1F;
   if (BAST_MODE_IS_DVB(hChn->acqParams.mode))
   {
      /* DVB */
      fmod |= 0x5200;
   }
   else if (BAST_MODE_IS_DTV(hChn->acqParams.mode))
   {
      /* DTV */
      fmod |= 0x5600;
   }
   else
   {
      /* DCII */
      if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_DCII_SPLIT)
      {
         if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_DCII_SPLIT_Q)
            fmod |= 0x0A00;   /* split Q */
         else
            fmod |= 0x4A00;   /* split I */
      }
      else
         fmod |= 0x5A00; /* combine */
   }
   fmod |= ((hChn->xportSettings.bTei) ? 0x0200 : 0x0000);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_FEC_FMOD, &fmod);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_QpskLockViterbi() - this routine is formerly verify_lock
******************************************************************************/
BERR_Code BAST_g3_P_QpskLockViterbi(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, bw, irq, raw;
   BERR_Code retCode;
   
   while (1)
   {
      switch (hChn->funct_state)
      {
         case 0:
            /* reset the equalizer */
            BAST_g3_P_ToggleBit(h, BCHP_SDS_EQ_EQFFECTL, 1);
            
            hChn->carrierDelay = (hChn->acqParams.mode == BAST_Mode_eDcii_scan) ? 900000 : 700000;          
            hChn->checkLockInterval = 20000; 

            /* extend delay for small symbol rates, shorten delay for higher symbol rates */
            if (hChn->acqParams.symbolRate < 5000000)
            {
               hChn->carrierDelay = (hChn->carrierDelay * 3) >> 1;
            }

            /* set baud loop bw */
#if 0       /* this bw is way too high, --> do NOT turn on */
            BAST_g3_P_SetBaudBw(h, hChn->baudAcqBw, hChn->baudAcqDamp);
#endif
            
            hChn->funct_state = 1;         
            break;
            
         case 1:
            /* set carrier loop bw */
            BAST_g3_P_SetCarrierBw(h, hChn->carrierAcq1Bw, hChn->carrierAcq1Damp);
            
            /* initialize counters */
            hChn->count1 = 0;
            hChn->maxCount1 = hChn->carrierDelay / hChn->checkLockInterval;
            if (hChn->maxCount1 == 0)
               hChn->maxCount1 = 1;
            
            hChn->funct_state = 2;
            break;
            
         case 2:
            /* gradually narrow the carrier bw from carrierAcq1Bw to carrierAcq2Bw */
            bw = hChn->carrierAcq1Bw - ((hChn->count1 * (hChn->carrierAcq1Bw - hChn->carrierAcq2Bw)) / hChn->maxCount1);
            BAST_g3_P_SetCarrierBw(h, bw, hChn->carrierAcq1Damp);
            hChn->count1++;
            
            if (hChn->count1 < hChn->maxCount1)
               return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, hChn->checkLockInterval, BAST_g3_P_QpskLockViterbi);
            
            /* narrow to tracking bw */
            BAST_g3_P_SetCarrierBw(h, hChn->carrierAcq2Bw, hChn->carrierAcq2Damp);
            
            hChn->funct_state = 3;
            break;
         
         case 3:       
            /* calculate time to wait for viterbi */
            hChn->count1 = hChn->carrierDelay / hChn->checkLockInterval;
            if (hChn->count1 < 1)
               hChn->count1 = 1;
            
            BAST_g3_P_ReadRegister(h, BCHP_SDS_MISC_INTR_RAW_STS0, &val);   
            hChn->prev_state = (val & BCHP_SDS_INTR2_0_CPU_STATUS_vit_in_sync_MASK);    

            /* clear sticky vit lock status bit */
            val = 0x18;
            BAST_g3_P_WriteRegister(h, BCHP_SDS_INTR2_0_CPU_CLEAR, &val);  

            hChn->funct_state = 4;             
            break;
            
         case 4:
            /* wait for viterbi lock */
            if ((hChn->count1 > 0) || (hChn->prev_state == BCHP_SDS_INTR2_0_CPU_STATUS_vit_in_sync_MASK))
            {                                          
               hChn->funct_state = 5; /* vit NOT locked */               
               return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, hChn->checkLockInterval, BAST_g3_P_QpskLockViterbi);
            }
            hChn->funct_state = 6; /* vit NOT locked after carrierDelay */
            break;
            
         case 5:
            /* make sure real time status is locked before checking sticky bits */
            BAST_g3_P_ReadRegister(h, BCHP_SDS_MISC_INTR_RAW_STS0, &raw);
            BAST_g3_P_ReadRegister(h, BCHP_SDS_INTR2_0_CPU_STATUS, &irq);            
            /* BDBG_MSG(("raw=0x%X, irq=0x%X", raw, irq)); */
            raw &= 0x18;
            irq &= 0x18;
            if (raw == BCHP_SDS_INTR2_0_CPU_STATUS_vit_in_sync_MASK)
            {
               BAST_DEBUG_QPSK(BDBG_MSG(("vit raw sts is locked")));            

               /* check sticky bit status for lock stability */               
               if ((irq == 0) && (hChn->prev_state == BCHP_SDS_INTR2_0_CPU_STATUS_vit_in_sync_MASK))
               {                
                  hChn->bVitLocked = true;               
                  hChn->funct_state = 7;  /* vit locked */
                  break;
               }
               hChn->prev_state = BCHP_SDS_INTR2_0_CPU_STATUS_vit_in_sync_MASK;              
            }
            else 
               hChn->prev_state = 0;
            if (irq != 0)
               BAST_g3_P_WriteRegister(h, BCHP_SDS_INTR2_0_CPU_CLEAR, &irq);                                            
               
            if (hChn->count1 > 0)
               hChn->count1--;
            hChn->funct_state = 4;  /* wait again */ 
            break;
            
         case 6:
            /* viterbi NOT locked */
            hChn->bVitLocked = false;
            
            /* manually handle dtv scan */
            if (hChn->dtvScanState & BAST_G3_DTV_SCAN_ON)
            {
               hChn->dtvScanState ^= BAST_G3_DTV_SCAN_1_2;
               
               if (hChn->dtvScanState & BAST_G3_DTV_SCAN_1_2)
               {
                  /* trying dtv 1/2 */
                  BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_VIT_VTCTL, ~0x0000001F, 0x00004000);
                  BAST_g3_P_AndRegister(h, BCHP_SDS_VIT_VTCTL, ~0x00004000);  /* deassert vit soft rst */
                  
                  hChn->funct_state = 1;
                  break;
               }
            }
            hChn->funct_state = 7;
            break;
         
         case 7:
            /* reacquire if unable to lock viterbi */
            if (hChn->bVitLocked == false)
            {
               BDBG_MSG(("Vit could not lock"));
               hChn->reacqCause = BAST_ReacqCause_eVitNotLock;
               return BAST_g3_P_Reacquire(h);
            }
            
            BDBG_MSG(("vit locked"));          
            BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eViterbiLocked));
            
            /* determine stabilized code rate */
            if (BAST_g3_P_QpskSetActualMode(h) != BERR_SUCCESS)
            {
               BDBG_MSG(("unable to set actual mode"));
               hChn->reacqCause = BAST_ReacqCause_eUnableToDetermineActualMode;
               return BAST_g3_P_Reacquire(h);
            }
            BAST_CHK_RETCODE(BAST_g3_P_QpskSetVcos(h));
            
            return BAST_g3_P_QpskSpInvScan(h);
            
         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            break;
      }
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_QpskSetActualMode()
******************************************************************************/
BERR_Code BAST_g3_P_QpskSetActualMode(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t i, j, val, vst_save;
   BERR_Code retCode;
   
   /* check initial code rate */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_VIT_VST, &val);
   vst_save = (val >> 24) & 0x0F;
   
   if ((hChn->acqParams.mode == BAST_Mode_eDss_scan) ||
       (hChn->acqParams.mode == BAST_Mode_eDvb_scan) || 
       (hChn->acqParams.mode == BAST_Mode_eDcii_scan))
   {
      /* check for stable code rate in scan mode */
      for (i = 10; i > 0; i--)
      {
         for (j = 50; j > 0; j--)
         {
            BAST_g3_P_ReadRegister(h, BCHP_SDS_VIT_VST, &val);
            val = (val >> 24) & 0x0F;
            if (val != vst_save)
               break;
         }
         vst_save = val;
         if (j == 0)
         {
            /* VST is stable */
            break;
         }
      }
   }
   
   BAST_CHK_RETCODE(BAST_g3_P_GetActualMode(h, &(hChn->actualMode)));
   if (hChn->actualMode == BAST_Mode_eUnknown)
      BERR_TRACE(retCode = BERR_UNKNOWN);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_QpskSetVcos()
******************************************************************************/
BERR_Code BAST_g3_P_QpskSetVcos(BAST_ChannelHandle h)
{
   uint32_t vst, val = 0;
   
   static const uint8_t vcos_byte1[16] =
   {
      0x40, 0x48, 0x50, 0x60,
      0x68, 0x78, 0x40, 0x40,
      0x40, 0x40, 0x48, 0x48,
      0x50, 0x50, 0x60, 0x78
   };
   
   /* read code rate */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_VIT_VST, &vst);
   vst = (vst >> 24) & 0x0F;
   
   /* look up qpsk hard decision level for VLC slicer */
   val = (vcos_byte1[vst] << 8);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_VCOS, &val);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_QpskSpInvScan()
******************************************************************************/
BERR_Code BAST_g3_P_QpskSpInvScan(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   if ((hChn->actualMode != BAST_Mode_eDvb_5_6) && (hChn->actualMode != BAST_Mode_eDss_6_7))
      return BAST_g3_P_QpskNarrowBw(h);
   
   /* do spinv scan */
   BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_FEC_FSYN, ~0xFFFF, 0x0802);
   
   hChn->funct_state = 0;
   hChn->count1 = 0;
   return BAST_g3_P_QpskSpinvScan1(h);
}


/******************************************************************************
 BAST_g3_P_QpskSpinvScan1()
******************************************************************************/
BERR_Code BAST_g3_P_QpskSpinvScan1(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, vtctl, sts;
   BERR_Code retCode;

   while (1)
   {
      switch (hChn->funct_state)
      {
         case 0:
            BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_VIT_VTCTL, ~0x60, (hChn->count1 % 3) << 5);
            hChn->funct_state = 1;
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, 30000, BAST_g3_P_QpskSpinvScan1);
            
         case 1:
            sts = 0x7F;         
            BAST_g3_P_WriteRegister(h, BCHP_SDS_INTR2_0_CPU_CLEAR, &sts);
            hChn->funct_state = 2;
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, 30000, BAST_g3_P_QpskSpinvScan1);         
       
         case 2:
            BAST_g3_P_ReadRegister(h, BCHP_SDS_MISC_INTR_RAW_STS0, &val);
            BAST_g3_P_ReadRegister(h, BCHP_SDS_VIT_VTCTL, &vtctl);   
            BAST_g3_P_ReadRegister(h, BCHP_SDS_INTR2_0_CPU_STATUS, &sts);   
/* BDBG_MSG(("spinv scan: vtctl=0x%X, raw=0x%X, sticky=0x%X", vtctl, val, sts)); */
            sts &= 0x1F;
         val &= 0x1F;
            if (val == 0x09)
            {         
            if ((sts == 0) || (sts == 0x09))
            {
/* BDBG_MSG(("spinv scan success")); */      
                  goto done;  /* IQ scan successful */
            }
            else
               goto not_locked;
            }
            else
            {
            not_locked:
/* BDBG_MSG(("spinv scan: RS/Vit not locked")); */
               hChn->count1++;
               if (hChn->count1 < 9)
                  hChn->funct_state = 0;
               else
               {
                  BAST_g3_P_AndRegister(h, BCHP_SDS_VIT_VTCTL, ~0x60);
                  goto done;
               }
            }
            break;
            
         default:
            BDBG_ERR(("invalid state"));
            BERR_TRACE(retCode = BAST_ERR_AP_IRQ);
            break;
      }
   }

   done:
   return BAST_g3_P_QpskNarrowBw(h);
}


/******************************************************************************
 BAST_g3_P_QpskNarrowBw()
******************************************************************************/
BERR_Code BAST_g3_P_QpskNarrowBw(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   /* set thresholds for bad headers */
   val = 0x04003020;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_FEC_FSYN, &val);
   
   hChn->count1 = 1;
   return BAST_g3_P_QpskNarrowBw1(h);
}


/******************************************************************************
 BAST_g3_P_QpskNarrowBw1()
******************************************************************************/
BERR_Code BAST_g3_P_QpskNarrowBw1(BAST_ChannelHandle h)
{
   BERR_Code retCode;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t cdelta, bw, damp;
#if 0
   uint32_t bdelta;
#endif
   
   if (hChn->count1 <= 4)
   {
      cdelta = (hChn->carrierAcq2Bw - hChn->carrierAcq3Bw) >> 2;
      bw = hChn->carrierAcq2Bw - hChn->count1 * cdelta;
      cdelta = (hChn->carrierAcq3Damp - hChn->carrierAcq2Damp) >> 2;
      damp = hChn->carrierAcq2Damp + hChn->count1 * cdelta;
      BAST_g3_P_SetCarrierBw(h, bw, damp);

#if 0      
      bdelta = (hChn->baudAcqBw - hChn->baudTrkBw) >> 2;
      bw = hChn->baudAcqBw - hChn->count1 * bdelta;
      bdelta = (hChn->baudTrkDamp - hChn->baudAcqDamp) >> 2;
      damp = hChn->baudAcqDamp + hChn->count1 * bdelta;
      BAST_g3_P_SetBaudBw(h, bw, damp);
#endif

      hChn->count1++;
      return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, 1000, BAST_g3_P_QpskNarrowBw1);
   }
   
   BAST_g3_P_SetCarrierBw(h, hChn->carrierAcq3Bw, hChn->carrierAcq3Damp);
   retCode = BAST_g3_P_SetBaudBw(h, hChn->baudTrkBw, hChn->baudTrkDamp);
   if (retCode != BERR_SUCCESS)
   {
      BDBG_WRN(("BAST_g3_P_SetBaudBw() error 0x%X", retCode));
      return retCode;
   }
 
   return BAST_g3_P_QpskAcquire1(h);
}


/******************************************************************************
 BAST_g3_P_QpskAcquire1() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_QpskAcquire1(BAST_ChannelHandle h)
{
   BERR_Code retCode;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;         

   static const uint32_t script_qpsk_1[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_FEC_FSYN, 0x04003020),
#ifndef BAST_HAS_WFE
      BAST_SCRIPT_WRITE(BCHP_SDS_AGC_ABW, 0x03030000),      /* TBD set AGC and DCO tracking bandwidths per Steve */
#endif
      BAST_SCRIPT_AND(BCHP_SDS_EQ_EQMISCCTL, ~0x0000FF00),  /* EQBLND=0 */
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_FLTD, 0x16000000),
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_qpsk_2[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_VLCTL, 0x00040704),
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_PLTD, 0x28000000),
      BAST_SCRIPT_AND_OR(BCHP_SDS_CL_CLCTL2, ~0x000000EF, 0x000000A1),  /* CLMISC=0xA1 but retain bit 4 */
#if 0      
      BAST_SCRIPT_AND_OR(BCHP_SDS_CL_CLCTL1, ~0x00FFFF00, 0x004A1000),  /* CLPDCTL=0x10, CLQCFD=0x4A */
#else
      BAST_SCRIPT_AND_OR(BCHP_SDS_CL_CLCTL1, ~0x0CFFFF00, 0x0C481000),  /* CLPDCTL=0x10, CLQCFD=0x40 */
#endif
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_qpsk_3[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_CL_PLI, 0x00000000),
      BAST_SCRIPT_AND_OR(BCHP_SDS_CL_CLCTL2, ~0x0000FF00, 0x00007100),  /*CLMISC2=0x71 */
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_qpsk_4[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_EQFFECTL, 0x690C0271), /* same as B0 chip, was 0x440C0271 */
      BAST_SCRIPT_AND(BCHP_SDS_EQ_EQFFECTL, ~0x00000001),   /* clear eq reset */
      BAST_SCRIPT_EXIT
   };
   
   static const uint32_t script_qpsk_5_new[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_SNR_SNRCTL, 0x03), 
      BAST_SCRIPT_WRITE(BCHP_SDS_SNR_SNORECTL, 0x8A),      
      BAST_SCRIPT_WRITE(BCHP_SDS_SNR_SNORECTL, 0x0A),
      BAST_SCRIPT_WRITE(BCHP_SDS_SNR_SNORECTL, 0x8A),      
      BAST_SCRIPT_WRITE(BCHP_SDS_BERT_BEIT, 0x00FF005F),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_qpsk_5_old[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_SNR_SNRCTL, 0x03), 
      BAST_SCRIPT_WRITE(BCHP_SDS_SNR_SNORECTL, 0x80),
      BAST_SCRIPT_WRITE(BCHP_SDS_SNR_SNORECTL, 0x00),
      BAST_SCRIPT_WRITE(BCHP_SDS_SNR_SNORECTL, 0x80),    
      BAST_SCRIPT_WRITE(BCHP_SDS_BERT_BEIT, 0x00FF005F),
      BAST_SCRIPT_EXIT
   };
   
   /* set agc tracking bw */
   BAST_g3_P_SetAgcTrackingBw(h); 
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_qpsk_1));
   BAST_g3_P_QpskSetFinalBlBw(h);
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_qpsk_2));
   BAST_g3_P_QpskSetOqpsk(h);
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_qpsk_3));
   BAST_g3_P_QpskSetFinalFlBw(h);
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_qpsk_4));
   BAST_g3_P_SetFfeMainTap(h);

   if (hDev->sdsRevId >= 0x63)
   {
      BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_qpsk_5_new));      
   }
   else
   {
      BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_qpsk_5_old));            
   }
         
   /* configure output pll */
   return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, 1000, BAST_g3_P_QpskSetOpll);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_QpskSetFinalFlBw()
******************************************************************************/
BERR_Code BAST_g3_P_QpskSetFinalFlBw(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t vst, fllc, flic;

   BAST_g3_P_ReadRegister(h, BCHP_SDS_VIT_VST, &vst);
   vst = (vst >> 24) & 0x0F;

   if (BAST_MODE_IS_DVB(hChn->acqParams.mode))
   {
      /* DVB */
      if (vst <= 1)
      {
         /* code rate 1/2 or 2/3 */
         fllc = 0x01000100;
         flic = 0x01000000;
      }
      else
      {
         fllc = 0x01000100;
         flic = 0x01000000;
      }
   }
   else if (BAST_MODE_IS_DTV(hChn->acqParams.mode))
   {
      /* DTV */
      if (vst <= 1)
      {
         /* code rate 1/2 or 2/3 */
         fllc = 0x02880300;
         flic = 0x029B0200;
      }
      else
      {
         fllc = 0x04F20200;
         flic = 0x01B20200;
      }
   }
   else
   {
      /* DCII */
      fllc = 0x02000200;
      flic = 0x01000300;
   }
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_FLLC, &fllc);
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_FLIC, &flic);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_QpskSetFinalBlBw()
******************************************************************************/
BERR_Code BAST_g3_P_QpskSetFinalBlBw(BAST_ChannelHandle h)
{  
   /* sets tracking PLC */
   return BAST_g3_P_ConfigPlc(h, false);
}


/******************************************************************************
 BAST_g3_P_QpskSetOqpsk() - from qpsk_set_clctl_3() in 4506
******************************************************************************/
BERR_Code BAST_g3_P_QpskSetOqpsk(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_CLCTL1, &val);
   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK)
      val = (val & 0x70) | 0x0C;
   else
      val = (val & 0x30) | 0x0C;
   val |= 0x0C080000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLCTL1, &val);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_QpskSetOpll()
******************************************************************************/
BERR_Code BAST_g3_P_QpskSetOpll(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;         
   uint32_t val, fmod, stuff, rs_mode, vst, data0, data2, data3, data6, data7, data4;
   uint32_t lval1, lval2, gcf, P_hi, P_lo, Q_hi, Q_lo;
   
   static const uint8_t qpsk_opll_data6[16] =
   {
      0x01, 0x02, 0x03, 0x05,
      0x06, 0x07, 0x01, 0x01,
      0x05, 0x01, 0x03, 0x02,
      0x03, 0x04, 0x05, 0x07
   };


   static const uint8_t qpsk_opll_data7[16] =
   {
      0x02, 0x03, 0x04, 0x06,
      0x07, 0x08, 0x01, 0x01,
      0x0B, 0x02, 0x05, 0x03,
      0x04, 0x05, 0x06, 0x08
   };
   
   BAST_g3_P_SdsPowerUpOpll(h);   
   
   /* reset output interface */
   BAST_g3_P_ToggleBit(h, BCHP_SDS_OI_OIFCTL00, 0x00000001);

   BAST_g3_P_ReadRegister(h, BCHP_SDS_FEC_FMOD, &fmod);
   rs_mode = (fmod & 0x0C00);
   stuff = fmod & 0x1F;

   if ((rs_mode == 0x0800) || (rs_mode == 0x0000))
   {
      /* DVB and DCII */
      data2 = 188;
      data3 = 204;
   }
   else
   {
      /* DTV */
      data2 = 130;
      data3 = 147;
   }

   if (fmod & 0x00008000)
      data4 = 16; /* RS decoder disabled */
   else
      data4 = 0;

   lval1 = data2 * (data2 + data4 + stuff);
   lval2 = data3 * data2;

   if (data3 != 147)
      data3 = 188;
   
   /* read code rate */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_VIT_VST, &vst);
   vst = (vst >> 24) & 0x0F;
   data6 = qpsk_opll_data6[vst];
   data7 = qpsk_opll_data7[vst];
   
   lval1 *= (data6 << 1);  /* lval1 = m */
   lval2 *= (data7 * 4);   /* lval2 = n */

   /* are we in split mode? */
   BAST_g3_P_ReadRegister(h, BCHP_SDS_VIT_VTCTL, &val);
   if (val & 0x0400)
      lval2 = lval2 << 1;

   BAST_MultU32U32(hChn->acqParams.symbolRate, 4 * lval1, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, lval2, &Q_hi, &Q_lo);

   if (hChn->bUndersample)
      Q_lo = Q_lo >> 1;

   if (hChn->xportSettings.bDelHeader)
   {
      if (hChn->xportSettings.bHead4)
         data0 = 4;
      else
         data0 = 1;
      BAST_MultU32U32(Q_lo, data3 - data0, &P_hi, &P_lo);
      BAST_DivU64U32(P_hi, P_lo, data3, &Q_hi, &Q_lo);
   }
   hChn->outputBitRate = Q_lo;

   gcf = BAST_g3_P_GCF(lval1, lval2);
   lval1 /= gcf;  /* opll_N */
   lval2 /= gcf;  /* opll_D */
   
   /* config output interface */
   hChn->opll_N = lval1;
   hChn->opll_D = lval2;
   BAST_g3_P_ConfigOif(h);

   if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_RS_DISABLE)
   {
      if (hDev->sdsRevId < 0x69)
         BAST_g3_P_OrRegister(h, BCHP_SDS_FEC_FECTL, 0x00000008);         
      else
         BAST_g3_P_OrRegister(h, BCHP_SDS_FEC_FMOD, 0x00000100);         
   }
   else
   {
      if (hDev->sdsRevId < 0x69)
         BAST_g3_P_AndRegister(h, BCHP_SDS_FEC_FECTL, ~0x00000008);         
      else
         BAST_g3_P_AndRegister(h, BCHP_SDS_FEC_FMOD, ~0x00000100);         
   }
  
   BAST_g3_P_OrRegister(h, BCHP_SDS_FEC_FECTL, 0x00000001);
   val = 0x7F;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_INTR2_0_CPU_CLEAR, &val);
   BAST_g3_P_AndRegister(h, BCHP_SDS_FEC_FECTL, ~0x00000001);
   
   /* start tracking */
   return BAST_g3_P_StartTracking(h);
}

