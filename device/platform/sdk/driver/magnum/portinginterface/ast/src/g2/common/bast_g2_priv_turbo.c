/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g2_priv_turbo.c $
 * $brcm_Revision: Hydra_Software_Devel/36 $
 * $brcm_Date: 9/6/12 6:18p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/common/bast_g2_priv_turbo.c $
 * 
 * Hydra_Software_Devel/36   9/6/12 6:18p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/35   10/5/11 11:14a enavarro
 * SWSATFE-150: spur canceller not supported in 73xx AST PI
 * 
 * Hydra_Software_Devel/34   1/11/11 2:32p ckpark
 * SWSATFE-96: fixed FLI-PLI leak for turbo spectral inversion case
 * 
 * Hydra_Software_Devel/33   11/3/09 2:05p enavarro
 * SWSATFE-1: removed BAST_USE_OLD_PLC option
 * 
 * Hydra_Software_Devel/32   10/15/09 3:40p enavarro
 * SW7342-51: deassert the TFEC block counter reset in
 * BAST_73xx_P_TurboUpdateErrorCounters()
 * 
 * Hydra_Software_Devel/31   9/22/09 9:08a enavarro
 * HW4506-79: set tracking PLC value in turbo; set final baud loop bw and
 * EQMU value after locking
 * 
 * Hydra_Software_Devel/30   9/10/09 9:28a enavarro
 * SW7325-594: moved functions used for fixed point arithmetic to common
 * location
 * 
 * Hydra_Software_Devel/29   9/8/09 4:00p enavarro
 * HW4506-67: use new plc programming for turbo modes
 * 
 * Hydra_Software_Devel/28   8/27/09 5:13p enavarro
 * HW4506-71: changes to turbo scan
 * 
 * Hydra_Software_Devel/27   8/10/09 4:40p ronchan
 * PR 55236: use new turbo error counters
 * 
 * Hydra_Software_Devel/26   7/7/09 10:39a enavarro
 * PR 56135: changes for turbo scan
 * 
 * Hydra_Software_Devel/25   6/22/09 9:32a enavarro
 * PR 56135: make common function for setting ONFN register
 * 
 * Hydra_Software_Devel/24   6/18/09 2:46p enavarro
 * PR 56135: improved turbo scan acquisition time
 * 
 * Hydra_Software_Devel/23   4/20/09 2:04p enavarro
 * PR 37769: added counters for all interrupts
 * 
 * Hydra_Software_Devel/22   1/15/09 10:12a ronchan
 * PR 37881: fixed compile warning in TurboCheckHP()
 * 
 * Hydra_Software_Devel/21   1/13/09 11:09a enavarro
 * PR 50993: update turbo block counters in
 * BAST_73xx_P_LdpcUpdateBlockCounters()
 * 
 * Hydra_Software_Devel/20   1/8/09 4:03p enavarro
 * PR 50925: set acquisition time based on when the lock event is first
 * set; PR 50818: added turbo lock filtering
 * 
 * Hydra_Software_Devel/19   1/5/09 4:44p ronchan
 * PR 50834: fixed turbo scan
 * 
 * Hydra_Software_Devel/18   10/1/08 2:22p enavarro
 * PR 37769: added BAST_73xx_P_TurboUpdateErrorCounters()
 * 
 * Hydra_Software_Devel/17   8/5/08 5:00p ronchan
 * PR 37881: removed '//' style comments
 * 
 * Hydra_Software_Devel/16   7/16/08 4:27p enavarro
 * PR 37769: tweaked PLC register setting for 8psk
 * 
 * Hydra_Software_Devel/15   6/24/08 11:45a ronchan
 * PR 43982: added array bounds checking for turbo arrays
 * 
 * Hydra_Software_Devel/14   6/16/08 2:45p enavarro
 * PR 37769: added BAST_73xx_P_TurboSetPlc()
 * 
 * Hydra_Software_Devel/13   6/9/08 2:53p enavarro
 * PR 37769: limit TTUR to 19
 * 
 * Hydra_Software_Devel/12   5/23/08 10:55a ronchan
 * PR 37769: added option to compile out turbo
 * 
 * Hydra_Software_Devel/10   5/6/08 4:01p enavarro
 * PR 37769: added some code for turbo scan
 * 
 * Hydra_Software_Devel/9   4/16/08 11:33a enavarro
 * PR 37769: fixed locked/not_lock interrupt handlers
 * 
 * Hydra_Software_Devel/8   3/3/08 3:16p enavarro
 * PR 37769: set pilot_pll for turbo acquisition
 * 
 * Hydra_Software_Devel/7   2/19/08 6:39p jrubio
 * PR38507: fix compile false warnings
 * 
 * Hydra_Software_Devel/6   2/8/08 9:23a enavarro
 * PR 37767: use pilot pll settings for turbo acquisition
 * 
 * Hydra_Software_Devel/5   2/6/08 4:40p enavarro
 * PR 37769: fixed BAST_73xx_P_TurboSetTtur()
 * 
 * Hydra_Software_Devel/4   2/5/08 2:57p ckpark
 * PR 37769: set funct_state to 0 before calling LdpcAcquire1()
 * 
 * Hydra_Software_Devel/3   12/17/07 11:06a enavarro
 * PR 37769: fixed compiler warning
 * 
 * Hydra_Software_Devel/2   12/4/07 4:39p ronchan
 * PR 37769: turbo implementation
 * 
 * Hydra_Software_Devel/1   11/21/07 11:34a ronchan
 * PR 37769: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2_priv.h"

BDBG_MODULE(bast_g2_priv_turbo);

#define BAST_TURBO_INIT_LOCK_FILTER_TIME 1000


#ifndef BAST_EXCLUDE_TURBO
/* local functions */
bool BAST_g2_P_TurboScanDetermineNextMode(BAST_ChannelHandle h);
BERR_Code BAST_g2_P_TurboCheckHP(BAST_ChannelHandle h);

/* external functions */
extern BERR_Code BAST_g2_P_LdpcAcquire1(BAST_ChannelHandle h);


static const uint32_t BAST_turbo_opll[10] =
{
   0x000924d2, /* turbo qpsk 1/2 */
   0x00011c92, /* turbo qpsk 2/3 */
   0x000dbbd2, /* turbo qpsk 3/4 */
   0x000f468c, /* turbo qpsk 5/6 */
   0x00100752, /* turbo qpsk 7/8 */
   0x0003ac00, /* turbo 8psk 2/3 */
   0x0003ee18, /* turbo 8psk 3/4 */
   0x000409a2, /* turbo 8psk 4/5 */
   0x00046744, /* turbo 8psk 5/6 */
   0x00049700, /* turbo 8psk 8/9 */
};


static const uint32_t BAST_turbo_opll2[10] =
{
   0x001d688b, /* turbo qpsk 1/2 */
   0x000264a5, /* turbo qpsk 2/3 */
   0x0018d18b, /* turbo qpsk 3/4 */
   0x001746d1, /* turbo qpsk 5/6 */
   0x0016860b, /* turbo qpsk 7/8 */
   0x0003fdb5, /* turbo 8psk 2/3 */
   0x0003bb9d, /* turbo 8psk 3/4 */
   0x0003a013, /* turbo 8psk 4/5 */
   0x00034271, /* turbo 8psk 5/6 */
   0x000312b5, /* turbo 8psk 8/9 */
};


static const uint32_t BAST_turbo_tssq_qpsk_1_2[2] = 
{
   1, 0xDD
};

static const uint32_t BAST_turbo_tssq_qpsk_2_3[4] =
{
   3, 0xDD, 0xDD, 0xEE
};

static const uint32_t BAST_turbo_tssq_qpsk_3_4[5] =
{
   4, 0xED, 0xED, 0xDE, 0xDE
};

static const uint32_t BAST_turbo_tssq_qpsk_5_6[7] =
{
   6, 0xED, 0xDE, 0xDE, 0xEE, 0xEE, 0xED
};

static const uint32_t BAST_turbo_tssq_qpsk_7_8[9] =
{
   8, 0xED, 0xED, 0xDE, 0xDE, 0xEE, 0xEE, 0xEE, 0xEE
};

static const uint32_t BAST_turbo_tssq_8psk_2_3[6] =
{
   5, 0x00, 0x00, 0x00, 0x00, 0x22
};

static const uint32_t BAST_turbo_tssq_8psk_3_4[8] =
{
   7, 0x00, 0x00, 0x20, 0x10, 0x00, 0x01, 0x02
};

static const uint32_t BAST_turbo_tssq_8psk_4_5[6] =
{
   5, 0x10, 0x00, 0x00, 0x01, 0x22
};

static const uint32_t BAST_turbo_tssq_8psk_5_6[6] =
{
   5, 0x10, 0x01, 0x10, 0x01, 0x22
};

static const uint32_t BAST_turbo_tssq_8psk_8_9[7] =
{
   6, 0x10, 0x01, 0x21, 0x10, 0x01, 0x12
};

static const uint32_t* BAST_turbo_tssq_table[10] =
{
   BAST_turbo_tssq_qpsk_1_2,
   BAST_turbo_tssq_qpsk_2_3,
   BAST_turbo_tssq_qpsk_3_4,
   BAST_turbo_tssq_qpsk_5_6,
   BAST_turbo_tssq_qpsk_7_8,
   BAST_turbo_tssq_8psk_2_3,
   BAST_turbo_tssq_8psk_3_4,
   BAST_turbo_tssq_8psk_4_5,
   BAST_turbo_tssq_8psk_5_6,
   BAST_turbo_tssq_8psk_8_9
};

static const uint32_t BAST_turbo_onfn[] =
{
   0x0000001F,  /* turbo qpsk 1/2 */
   0x0000001D,  /* turbo qpsk 2/3 */
   0x0000001C,  /* turbo qpsk 3/4 */
   0x0000001A,  /* turbo qpsk 5/6 */
   0x00000018,  /* turbo qpsk 7/8 */
   0x0000001B,  /* turbo 8psk 2/3 */
   0x00000019,  /* turbo 8psk 3/4 */
   0x0000001B,  /* turbo 8psk 4/5 */
   0x0000001B,  /* turbo 8psk 5/6 */
   0x0000001A,  /* turbo 8psk 8/9 */
};
#endif


/******************************************************************************
 BAST_g2_P_TurboSetOnfn()
******************************************************************************/
void BAST_g2_P_TurboSetOnfn(BAST_ChannelHandle h)
{
#ifndef BAST_EXCLUDE_TURBO
   BAST_g2_P_LdpcConfig(h, BCHP_SDS_ONFN, BAST_turbo_onfn);  
#endif   
}


/******************************************************************************
 BAST_g2_P_TurboAcquire()
******************************************************************************/
BERR_Code BAST_g2_P_TurboAcquire(BAST_ChannelHandle h)
{
#ifndef BAST_EXCLUDE_TURBO
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   BERR_Code BAST_g2_P_LdpcAcquire3(BAST_ChannelHandle h);
   uint32_t val;
   
   hChn->ldpcScanState = 0;
   if (hChn->acqParams.mode == BAST_Mode_eTurbo_scan)
   {
      if (BAST_g2_P_TurboScanDetermineNextMode(h) == false)
      {
         if (hChn->blindScanStatus == BAST_BlindScanStatus_eAcquire)
            return BAST_g2_P_BlindReacquire(h);
         else
            return false;
      }
      
      /* check if the next mode is same modulation as previous mode */
      if (hChn->turboScanState & BAST_TURBO_SCAN_HP_LOCKED)
      {
         BAST_g2_P_ReadRegister(h, BCHP_SDS_STAT, &val);       
         if (((val >> 24) & 0x1F) == 17)
         {         
            /* HP is still locked */                     
            val = 0x00;
            BAST_g2_P_WriteRegister(h, BCHP_TFEC_TTUR, &val);                        
            return BAST_g2_P_LdpcAcquire3(h);                
         }       
      }
   }
   else
      hChn->actualMode = hChn->acqParams.mode;

   /* use pilot pll settings for turbo acquisition */
   hChn->acqParams.acq_ctl |= (BAST_ACQSETTINGS_LDPC_PILOT_PLL | BAST_ACQSETTINGS_LDPC_PILOT);
   
   if (hChn->firstTimeLock == 0)
      hChn->lockFilterTime = BAST_TURBO_INIT_LOCK_FILTER_TIME;   
      
   hChn->funct_state = 0;
   retCode = BAST_g2_P_LdpcAcquire1(h);
   
   return retCode;
#else
   return BERR_NOT_SUPPORTED;
#endif
}


#ifndef BAST_EXCLUDE_TURBO
/******************************************************************************
 BAST_g2_P_TurboMonitor()
******************************************************************************/
BERR_Code BAST_g2_P_TurboMonitor(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;

   if (hChn->bExternalTuner == false)
      BAST_g2_P_3StageAgc_isr(h, 0);         
   
   BAST_g2_P_TransferFreqOffset(h);
   if (hChn->bForceReacq)
      return BAST_g2_P_LdpcReacquire(h);
        
#ifndef BAST_EXCLUDE_STATUS_EVENTS
   BAST_g2_P_CheckStatusIndicators(h);
#endif

   return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100000, BAST_g2_P_TurboMonitor);
}


/******************************************************************************
 BAST_g2_P_TurboLock_isr()
******************************************************************************/
void BAST_g2_P_TurboLock_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;   
   uint32_t val;
   BSTD_UNUSED(param);

   BAST_g2_P_IncrementInterruptCounter(h, BAST_Sds_IntID_eTurboLock);
   BDBG_MSG(("Turbo locked ISR"));

   if (hChn->lockIsrFlag == 1)
   {
      BDBG_MSG(("ignoring lock isr"));
      return;
   }
   hChn->lockIsrFlag = 1;
   
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eBaud);   
   
   if (hChn->acqState == BAST_AcqState_eWaitForLock)
   {      
      /* ldpc locked for the first time */        
      hChn->acqState = BAST_AcqState_eMonitor;
            
      BAST_g2_P_LdpcSetPlc(h, false);

      /* set final baud bw */
      BAST_g2_P_LdpcSetFinalBaudLoopBw(h);   

      val = 0x69;
      BAST_g2_P_WriteRegister(h, BCHP_SDS_EQMU, &val); 
      
      BAST_g2_P_ResyncBert(h);
   }
   
   BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eGen1, hChn->lockFilterTime, BAST_g2_P_LdpcLockStable);   
   BAST_g2_P_TurboMonitor(h);   
}


/******************************************************************************
 BAST_g2_P_TurboNotLock_isr()
******************************************************************************/
void BAST_g2_P_TurboNotLock_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g2_P_ChannelHandle *hChn = h->pImpl;
   uint32_t val;
   BSTD_UNUSED(param);

   BAST_g2_P_IncrementInterruptCounter(h, BAST_Sds_IntID_eTurboNotLock);   
   BAST_g2_P_LdpcUpdateBlockCounters(h);
      
   if (hChn->lockIsrFlag == 0)
   {
      BDBG_MSG(("ignoring not lock isr"));
      return;
   }
   hChn->lockIsrFlag = 0;
   
   BDBG_MSG(("Turbo not locked ISR"));
   
   BAST_g2_P_DisableTimer(h, BAST_TimerSelect_eGen1);
   BAST_g2_P_IndicateNotLocked(h);   
   
   BAST_g2_P_ReadRegister(h, BCHP_SDS_STAT, &val);
   if (((val >> 24) & 0x1F) != 17)
   {
      BDBG_MSG(("reacquiring because HP fell out of lock"));
      goto reacquire;
   } 
   
   if (hChn->acqState == BAST_AcqState_eWaitForLock)
      BAST_g2_P_LdpcAcquire6(h);
   else if ((hChn->turboCtl & BAST_G2_TURBO_CTL_DISABLE_FEC_REACQ) == 0)
   {
      BDBG_MSG(("reacquiring because Turbo fell out of lock"));
      
      reacquire:
      BAST_g2_P_LdpcReacquire(h);
   }   
   else
      BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 2000000, BAST_g2_P_TurboCheckHP); /* 100msec */   
}


/******************************************************************************
 BAST_g2_P_TurboCheckHP()
******************************************************************************/
BERR_Code BAST_g2_P_TurboCheckHP(BAST_ChannelHandle h)
{
   uint32_t val;
   
   BAST_g2_P_ReadRegister(h, BCHP_SDS_STAT, &val);
   if (((val >> 24) & 0x1F) != 17)
   {
      BDBG_MSG(("reacquiring because HP fell out of lock"));
      return BAST_g2_P_LdpcReacquire(h);      
   }
   else
      return BAST_g2_P_EnableTimer(h, BAST_TimerSelect_eBaud, 2000000, BAST_g2_P_TurboCheckHP); /* 100msec */      
}


/******************************************************************************
 BAST_g2_P_TurboSetOpll()
******************************************************************************/
BERR_Code BAST_g2_P_TurboSetOpll(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, i;
   
   if ((hChn->actualMode >= BAST_Mode_eTurbo_Qpsk_1_2) && (hChn->actualMode <= BAST_Mode_eTurbo_8psk_8_9))
      i = hChn->actualMode - BAST_Mode_eTurbo_Qpsk_1_2;
   else
      return BERR_INVALID_PARAMETER;
   
   val = BAST_turbo_opll[i];
   BAST_g2_P_WriteRegister(h, BCHP_SDS_OPLL, &val);
   val = BAST_turbo_opll2[i];
   BAST_g2_P_WriteRegister(h, BCHP_SDS_OPLL2, &val);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_TurboSetTssq()
******************************************************************************/
BERR_Code BAST_g2_P_TurboSetTssq(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val, i, len;
   uint32_t *ptr;
   
   if ((hChn->actualMode >= BAST_Mode_eTurbo_Qpsk_1_2) && (hChn->actualMode <= BAST_Mode_eTurbo_8psk_8_9))
      i = hChn->actualMode - BAST_Mode_eTurbo_Qpsk_1_2;
   else
      return BERR_INVALID_PARAMETER;
   
   ptr = (uint32_t*) BAST_turbo_tssq_table[i];
   
   len = *ptr++;
   for (i = len; i; i--)
   {
      val = *ptr++;
      BAST_g2_P_WriteRegister(h, BCHP_TFEC_TSSQ, &val);
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_TurboSetTtur()
******************************************************************************/
BERR_Code BAST_g2_P_TurboSetTtur(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo, val;
   
   BAST_g2_P_GetFecFreq(h);
         
   BAST_MultU32U32(hChn->fecFreq, 10000, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->acqParams.symbolRate, &Q_hi, &Q_lo);
   BAST_DivU64U32(Q_hi, Q_lo, 5625, &Q_hi, &Q_lo);
   Q_lo--;

   if (Q_lo > 19)
      Q_lo = 19;
   val = Q_lo << 16;
   val |= 0x0A03;
/* BDBG_MSG(("BAST_g2_P_TurboSetTtur(): FEC freq=%d, TTUR=0x%X", hChn->fecFreq, val)); */
   
   BAST_g2_P_WriteRegister(h, BCHP_TFEC_TTUR, &val);

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_TurboScanDetermineNextMode()
******************************************************************************/
bool BAST_g2_P_TurboScanDetermineNextMode(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;   
   
   hChn->actualMode = BAST_Mode_eUnknown;
   if (hChn->turboScanModes == 0)
      return false;
   
   next_mode:
   if (hChn->turboScanCurrMode == 0)
   {
      hChn->turboScanCurrMode = 1;
      hChn->turboScanState = 0;
   }
   else
   {
      hChn->turboScanCurrMode = (hChn->turboScanCurrMode << 1) & BAST_G2_TURBO_SCAN_MASK;
      
      if ((hChn->blindScanStatus == BAST_BlindScanStatus_eAcquire) && (hChn->turboScanCurrMode == 0))
         return false;
   }
   
   if (hChn->turboScanCurrMode & hChn->turboScanModes)
   {
      if (hChn->turboScanCurrMode & BAST_G2_TURBO_QPSK_SCAN_1_2)
         hChn->actualMode = BAST_Mode_eTurbo_Qpsk_1_2;
      else if (hChn->turboScanCurrMode & BAST_G2_TURBO_QPSK_SCAN_2_3)    
         hChn->actualMode = BAST_Mode_eTurbo_Qpsk_2_3;
      else if (hChn->turboScanCurrMode & BAST_G2_TURBO_QPSK_SCAN_3_4)    
         hChn->actualMode = BAST_Mode_eTurbo_Qpsk_3_4;
      else if (hChn->turboScanCurrMode & BAST_G2_TURBO_QPSK_SCAN_5_6)     
         hChn->actualMode = BAST_Mode_eTurbo_Qpsk_5_6;
      else if (hChn->turboScanCurrMode & BAST_G2_TURBO_QPSK_SCAN_7_8)     
         hChn->actualMode = BAST_Mode_eTurbo_Qpsk_7_8;
      else if (hChn->turboScanCurrMode & BAST_G2_TURBO_8PSK_SCAN_2_3)     
         hChn->actualMode = BAST_Mode_eTurbo_8psk_2_3;
      else if (hChn->turboScanCurrMode & BAST_G2_TURBO_8PSK_SCAN_3_4)    
         hChn->actualMode = BAST_Mode_eTurbo_8psk_3_4;
      else if (hChn->turboScanCurrMode & BAST_G2_TURBO_8PSK_SCAN_4_5)     
         hChn->actualMode = BAST_Mode_eTurbo_8psk_4_5;
      else if (hChn->turboScanCurrMode & BAST_G2_TURBO_8PSK_SCAN_5_6)    
         hChn->actualMode = BAST_Mode_eTurbo_8psk_5_6;
      else /* (hChn->turboScanCurrMode & BAST_G2_TURBO_8PSK_SCAN_8_9) */   
         hChn->actualMode = BAST_Mode_eTurbo_8psk_8_9;
         
      if (hChn->turboScanState & BAST_TURBO_SCAN_HP_INIT)
      {
         if (hChn->turboScanState & BAST_TURBO_SCAN_HP_LOCKED)
         {
            if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
            {         
               if ((hChn->turboScanState & BAST_TURBO_SCAN_8PSK_HP_LOCKED) == 0)
                  goto next_mode;
            }
            else if (hChn->turboScanState & BAST_TURBO_SCAN_8PSK_HP_LOCKED)
               goto next_mode;
         }
         else
         {
            if (hChn->turboScanState & BAST_TURBO_SCAN_8PSK_FAILED)
            {
               /* only consider qpsk */
               if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
                  goto next_mode;
            }
            else
            {
               /* only consider 8psk */
               if (!(BAST_MODE_IS_TURBO_8PSK(hChn->actualMode)))
                  goto next_mode;               
            }
         }
      }
   }
   else
      goto next_mode;
        
   switch (hChn->actualMode)
   {
      case BAST_Mode_eTurbo_Qpsk_1_2:
         BDBG_MSG(("trying BAST_Mode_eTurbo_Qpsk_1_2"));
         break;
      case BAST_Mode_eTurbo_Qpsk_2_3:
         BDBG_MSG(("trying BAST_Mode_eTurbo_Qpsk_2_3"));
         break;
      case BAST_Mode_eTurbo_Qpsk_3_4:
         BDBG_MSG(("trying BAST_Mode_eTurbo_Qpsk_3_4"));
         break;
      case BAST_Mode_eTurbo_Qpsk_5_6:
         BDBG_MSG(("trying BAST_Mode_eTurbo_Qpsk_5_6"));
         break;         
      case BAST_Mode_eTurbo_Qpsk_7_8:
         BDBG_MSG(("trying BAST_Mode_eTurbo_Qpsk_7_8"));
         break;
      case BAST_Mode_eTurbo_8psk_2_3:
         BDBG_MSG(("trying BAST_Mode_eTurbo_8psk_2_3"));
         break;
      case BAST_Mode_eTurbo_8psk_3_4:
         BDBG_MSG(("trying BAST_Mode_eTurbo_8psk_3_4"));
         break;
      case BAST_Mode_eTurbo_8psk_4_5:
         BDBG_MSG(("trying BAST_Mode_eTurbo_8psk_4_5"));
         break;
      case BAST_Mode_eTurbo_8psk_5_6:
         BDBG_MSG(("trying BAST_Mode_eTurbo_8psk_5_6"));
         break;
      case BAST_Mode_eTurbo_8psk_8_9:
         BDBG_MSG(("trying BAST_Mode_eTurbo_8psk_8_9"));
         break;
      default:
         BDBG_ERR(("BAST_g2_P_TurboScanDetermineNextMode(): should not get here!"));
         return false;         
   }

   return true;
}


/******************************************************************************
 BAST_g2_P_TurboUpdateErrorCounters()
******************************************************************************/
void BAST_g2_P_TurboUpdateErrorCounters(BAST_ChannelHandle h)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
#if (BCHP_CHIP==7340) || (BCHP_CHIP==7342)
   BAST_g2_P_ReadRegister(h, BCHP_TFEC_TNBLK, &val);
   hChn->turboTotalBlocks += val;
   
   BAST_g2_P_ReadRegister(h, BCHP_TFEC_TCBLK, &val);
   hChn->turboCorrBlocks += val;
   
   BAST_g2_P_ReadRegister(h, BCHP_TFEC_TBBLK, &val);
   hChn->turboBadBlocks += val;
   
   /* reset the FEC error counters */
   BAST_g2_P_ReadRegister(h, BCHP_TFEC_TFECTL, &val);
   val |= 0x06;   
   BAST_g2_P_WriteRegister(h, BCHP_TFEC_TFECTL, &val);
   val &= ~0x06;
   BAST_g2_P_WriteRegister(h, BCHP_TFEC_TFECTL, &val);   
#else
   BAST_g2_P_ReadRegister(h, BCHP_TFEC_TCOR, &val);
   hChn->turboTotalBlocks += ((val >> 16) & 0xFFFF);
   hChn->turboCorrBlocks += (val & 0xFFFF);

   BAST_g2_P_ReadRegister(h, BCHP_TFEC_TERR, &val);
   hChn->turboBadBlocks += (val & 0xFFFF);
#endif
}

#endif
