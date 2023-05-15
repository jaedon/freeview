/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv_dft.c $
 * $brcm_Revision: Hydra_Software_Devel/42 $
 * $brcm_Date: 2/6/13 9:37a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv_dft.c $
 * 
 * Hydra_Software_Devel/42   2/6/13 9:37a ckpark
 * SWSATFE-174: added debug message
 * 
 * Hydra_Software_Devel/41   10/23/12 4:21p enavarro
 * SWSATFE-174: implement dft low baud rate workaround for 4538
 * 
 * Hydra_Software_Devel/40   10/22/12 11:18a enavarro
 * SWSATFE-221: fix dft for low baud rate
 * 
 * Hydra_Software_Devel/39   9/19/12 10:35a enavarro
 * SWSATFE-217: put minimum delay for dft_done
 * 
 * Hydra_Software_Devel/38   9/11/12 11:10a enavarro
 * SWSATFE-174: added BAST_EXCLUDE_EXT_TUNER build option
 * 
 * Hydra_Software_Devel/37   2/22/12 10:18a enavarro
 * SWSATFE-86: fixed bug in TUNER_CTL_BYPASS_DFT_FREQ_EST
 * 
 * Hydra_Software_Devel/36   12/12/11 10:39a enavarro
 * SWSATFE-159: fixed bug in BAST_g3_P_DftStartAndWaitForDone() when more
 * than 4 decimation filters are enabled
 * 
 * Hydra_Software_Devel/35   11/10/11 9:24a enavarro
 * SWSATFE-159: check for arithmetic overflow
 * 
 * Hydra_Software_Devel/34   10/3/11 2:05p enavarro
 * SWSATFE-97: extend dft timeout when dft range is widened
 * 
 * Hydra_Software_Devel/33   9/28/11 4:17p enavarro
 * SWSATFE-148: made changes for TUNER_CTL_PRESERVE_COMMANDED_FREQ
 * 
 * Hydra_Software_Devel/32   6/2/11 10:38a enavarro
 * SWSATFE-86: updated debug message to display the channel in dft freq
 * estimate
 * 
 * Hydra_Software_Devel/31   5/9/11 9:53a ckpark
 * SWSATFE-75: change input scaling based on number of decimation filters
 * 
 * Hydra_Software_Devel/30   5/2/11 1:55p ckpark
 * SWSATFE-75: updates to DFT
 * 
 * Hydra_Software_Devel/29   5/2/11 9:07a ckpark
 * SWSATFE-75: bypassing DFT freq. est. bug is fixed
 * 
 * Hydra_Software_Devel/28   4/25/11 5:12p ckpark
 * SWSATFE-75: use FLIF stepping for DFT freq. est. (LPF is narrowed)
 * 
 * Hydra_Software_Devel/27   4/22/11 3:07p ckpark
 * SWSATFE-75: use QuickTune for DFT freq. est stepping
 * 
 * Hydra_Software_Devel/26   1/27/11 2:57p enavarro
 * SWSATFE-75: fixed bug in symbol rate scan; removed dftSize config
 * parameter
 * 
 * Hydra_Software_Devel/25   1/27/11 10:17a enavarro
 * SWSATFE-75: made changes to get tone detection to work
 * 
 * Hydra_Software_Devel/24   1/11/11 10:47a enavarro
 * SWSATFE-75: add BYPASS_DFT_FREQ_EST bit in TUNER_CTL config param
 * 
 * Hydra_Software_Devel/23   1/4/11 4:37p enavarro
 * SWSATFE-75: put timeout on dft_done; changes to tone search
 * 
 * Hydra_Software_Devel/22   12/29/10 12:09p enavarro
 * SWSATFE-75: check out symbol rate scan
 * 
 * Hydra_Software_Devel/21   12/21/10 4:46p enavarro
 * SWSATFE-75: got dft to work
 * 
 * Hydra_Software_Devel/20   12/15/10 3:24p enavarro
 * SWSATFE-75: changed BAST_DCO_WORKAROUND
 * 
 * Hydra_Software_Devel/19   12/15/10 11:13a enavarro
 * SWSATFE-75: added BAST_DCO_WORKAROUND option
 * 
 * Hydra_Software_Devel/18   12/14/10 9:08a enavarro
 * SWSATFE-75: added test option to bypass dft; changed dft done timeout
 * due to change in Fs divider
 * 
 * Hydra_Software_Devel/17   12/13/10 6:23p enavarro
 * SWSATFE-75: changed the timeout for DFT done
 * 
 * Hydra_Software_Devel/16   12/12/10 11:41p jrubio
 * SW7344-9: fix comments to fit Standard C
 * 
 * Hydra_Software_Devel/15   12/11/10 7:45p enavarro
 * SWSATFE-75: initial lab checkout
 * 
 * Hydra_Software_Devel/14   10/20/10 4:31p enavarro
 * SWSATFE-10: fixed compile error
 * 
 * Hydra_Software_Devel/13   10/19/10 2:21p enavarro
 * SWSATFE-75: applied latest TT40 changes
 * 
 * Hydra_Software_Devel/12   10/19/10 11:10a enavarro
 * SWSATFE-75: finished implementing peak scan
 * 
 * Hydra_Software_Devel/11   10/18/10 2:54p enavarro
 * SWSATFE-75: started coding BAST_g3_P_DftPeakScan()
 * 
 * Hydra_Software_Devel/10   10/14/10 3:38p enavarro
 * SWSATFE-75: added BAST_DEBUG_DFT() macro
 * 
 * Hydra_Software_Devel/9   10/11/10 11:34a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/8   9/22/10 6:35p enavarro
 * SWSATFE-75: code clean up
 * 
 * Hydra_Software_Devel/7   9/22/10 5:18p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/6   9/21/10 10:27a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/5   9/17/10 5:22p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/4   9/13/10 10:56a enavarro
 * SWSATFE-75: added oqpsk dft carrier search
 * 
 * Hydra_Software_Devel/3   9/9/10 5:07p enavarro
 * SWSATFE-75: reworked dft freq estimate per TT40G
 * 
 * Hydra_Software_Devel/2   9/1/10 5:04p enavarro
 * SWSATFE-75: change function name to BAST_g3_P_DftSetDdfsFcw()
 * 
 * Hydra_Software_Devel/1   8/31/10 2:52p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"

BDBG_MODULE(bast_g3_priv_dft);


#define BAST_DEBUG_DFT(x) /* x */
#define BAST_DEBUG_PEAK_SCAN(x) /* x */


/* local functions */
BERR_Code BAST_g3_P_DftPeakScanStateMachine(BAST_ChannelHandle h);


/******************************************************************************
 BAST_g3_P_SetDftDdfsFcw()
******************************************************************************/
BERR_Code BAST_g3_P_DftSetDdfsFcw(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t P_hi, P_lo, Q_hi, Q_lo;
   
   BAST_MultU32U32(hChn->acqParams.symbolRate, 32768 << 1, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, &Q_lo);
   Q_lo = (Q_lo + 1) >> 1;
   /* BDBG_MSG(("DFT_DDFS_FCW=0x%X", Q_lo)); */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_DDFS_FCW, &Q_lo); 
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_DftWaitForDone()
******************************************************************************/
BERR_Code BAST_g3_P_DftWaitForDone(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_STATUS, &val);   
   if (((val & 0x06) == 0x06) || (hChn->dft_done_timeout > 3))
   {
      if (hChn->dft_done_timeout > 1)
      {
         BDBG_ERR(("dft_done_timeout=%d (DFT_STATUS=0x%X)", hChn->dft_done_timeout, val));
      }
      return hChn->failFunct(h);
   }

   hChn->dft_done_timeout++;
   BDBG_ERR(("DFT not yet ready (DFT_STATUS=0x%X)...", val));
   return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 100, BAST_g3_P_DftWaitForDone); 
}


/******************************************************************************
 BAST_g3_P_DftStartAndWaitForDone()
******************************************************************************/
BERR_Code BAST_g3_P_DftStartAndWaitForDone(BAST_ChannelHandle h, BAST_g3_FUNCT funct)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t range_start, range_end, val, P_hi, P_lo, Q_hi, Q_lo, i, n, retry_count = 0;
   
   hChn->failFunct = funct;

   /* start the DFT engine */
   retry:
   val = 1;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_CTRL0, &val);
   
   /* time it takes for DFT to run is 33.4K Fs cycles * 2^N with N decimation filters enabled */
   n = BAST_g3_P_GetNumDecimatingFilters(h);
   
   BAST_MultU32U32(33400 * (1 << n), 2000000, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, hChn->sampleFreq, &Q_hi, &Q_lo);
   Q_lo = (Q_lo + 1) >> 1;
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_RANGE_START, &range_start);
   BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_RANGE_END, &range_end);
   val = (range_end - range_start) >> 4;   
   if (val > 0)
      Q_lo *= (val+1);  
   if (Q_lo == 0)
      Q_lo = 400;
   /* BAST_DEBUG_DFT(BDBG_MSG(("%d decimation filters, waiting %u usecs", n, Q_lo))); */
      
   /* wait at least 70 Fs clocks from DFT_CTRL0.0=0 to DFT_CTRL.0=1 */
   val = 0;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_CTRL0, &val); 
     
   for (i = 0; i < 50; i++)
   {
      BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_STATUS, &val);  
      if (val & 0x07)
         break;      
   }
   if ((val & 0x07) == 0)
   {
      BDBG_WRN(("dft_status should not be 0, trying again..."));
      if (retry_count++ < 10)
         goto retry;
      BDBG_ERR(("ERROR: dft_status=0!"));
      return BERR_TRACE(BAST_ERR_HAB_CMD_FAIL);
   }  

   hChn->dft_done_timeout = 0;
   return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, Q_lo, BAST_g3_P_DftWaitForDone);       
}


/******************************************************************************
 BAST_g3_P_DftSearchCarrierStateMachine()
******************************************************************************/
BERR_Code BAST_g3_P_DftSearchCarrierStateMachine(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t peak_pow, total_pow, peak_bin, val, start, stop, hb;
   
   while (hChn->dft_funct_state != 0xFFFFFFFF)
   {
      switch (hChn->dft_funct_state)
      {
         case 0:   
            /* coarse freq step is 10% of Fb */
            hChn->dftFreqEstimateStatus |= BAST_G3_CONFIG_FREQ_ESTIMATE_STATUS_COARSE;    
            hChn->tunerIfStepSize = hChn->acqParams.symbolRate / 10;
            hChn->tunerIfStep = (int32_t)(-(hDev->searchRange) - hChn->tunerIfStepSize);
            hChn->tunerIfStepMax = (int32_t)(hDev->searchRange);
            hChn->dft_current_max_pow = 0;
      
            if (hChn->tunerIfStepSize > hDev->searchRange)
               hChn->dft_funct_state = 6; /* do fine search */
            else
            {
               hChn->dft_funct_state = 1;         
               BAST_DEBUG_DFT(BDBG_MSG(("\nDFT(coarse): StepSize=%d", hChn->tunerIfStepSize)));
            }
            break;
            
         case 1:
            if (hChn->tunerIfStep <= hChn->tunerIfStepMax)
            {
               hChn->tunerIfStep += (int32_t)hChn->tunerIfStepSize;     
               hChn->dft_funct_state = 2;    
#if 0          /* if LPF is narrowed, FLIF stepping is OK */
               BAST_CHK_RETCODE(BAST_g3_P_SetFlifOffset(h, hChn->tunerIfStep));
#else          
               return BAST_g3_P_TunerQuickTune(h, BAST_g3_P_DftSearchCarrierStateMachine);  
#endif 
            }
            else
               hChn->dft_funct_state = 6;        
            break;
            
         case 2:
            hChn->dft_count1 = 0; /* dft_count1 = current number of passes */
            hChn->count2 = 0; /* count2 = sum of total_pow */
            hChn->dft_funct_state = 3;
            break;
            
         case 3:    
            val = hChn->dft_count1 ? 0x7F0 : 0;
            BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_RANGE_START, &val);  
            hChn->dft_funct_state = 31;
#if 0            
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1, BAST_g3_P_DftSearchCarrierStateMachine);    
#else
            break;
#endif

         case 31:
            BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_RANGE_START, &val);  
            val |= 0x0F; 
            BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_RANGE_END, &val); 

            val = 0x02;
            BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_CTRL0, &val); 
            hChn->dft_funct_state = 32;
#if 0            
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 1, BAST_g3_P_DftSearchCarrierStateMachine);    
#else
            break;
#endif

         case 32:
            val = 0x00;
            BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_CTRL0, &val); 

            hb = BAST_g3_P_GetNumDecimatingFilters(h);
            if (hb <= 2)
               val = 0x2F7;
            else if (hb <= 3)
               val = 0x217;  
            else
               val = 0x317;         
            BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_CTRL1, &val); 

            BAST_CHK_RETCODE(BAST_g3_P_DftSetDdfsFcw(h));
            hChn->dft_funct_state = 4;
            return BAST_g3_P_DftStartAndWaitForDone(h, BAST_g3_P_DftSearchCarrierStateMachine);
                        
         case 4:   
            BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_PEAK_POW, &peak_pow);    
            BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_TOTAL_POW, &total_pow); 
            BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_PEAK_BIN, &peak_bin);
            BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_RANGE_START, &start);
            BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_RANGE_END, &stop);            
            BAST_DEBUG_DFT(BDBG_MSG(("%d Hz: [%x,%x] %08X %08X %08X", hChn->tunerIfStep, start, stop, peak_pow, total_pow, peak_bin))); 
            if (hChn->dft_count1)
            {
               if (peak_pow > hChn->maxCount1)
                  goto save_peak;
            }
            else
            {
               save_peak:
               hChn->maxCount1 = peak_pow;
            }

            hChn->count2 += total_pow;
            hChn->dft_count1++;
            if (hChn->dft_count1 == 1)          
               hChn->dft_funct_state = 3;
            else
               hChn->dft_funct_state = 5;
            break;
  
         case 5: 
               val = hChn->maxCount1 - ((hChn->count2 - hChn->maxCount1) >> 5);
               if (val & 0x80000000) /* check for overflow */
                  val = 0;
               BAST_DEBUG_DFT(BDBG_MSG(("%d Hz: max_peak=0x%08X, sum_total=0x%08X, pow=%u", hChn->tunerIfStep, hChn->maxCount1, hChn->count2, val)));               
               if (val > hChn->dft_current_max_pow)
               {
                  hChn->dft_current_max_pow = val;
                  hChn->dftFreqEstimate = hChn->tunerIfStep;
               }
               hChn->dft_funct_state = 1;
            break;
     
         case 6:
            if (hChn->dftFreqEstimateStatus & BAST_G3_CONFIG_FREQ_ESTIMATE_STATUS_FINE)
               hChn->dft_funct_state = 7;
            else
            {
               /* reset baud loop integrator */
               val = 0;
               BAST_g3_P_WriteRegister(h, BCHP_SDS_BL_BRI, &val);
               
               hChn->dftFreqEstimateStatus |= BAST_G3_CONFIG_FREQ_ESTIMATE_STATUS_FINE;                  
               hChn->tunerIfStep = hChn->dftFreqEstimate - (int32_t)hChn->tunerIfStepSize;
               if (hChn->tunerIfStep < (int32_t)-(hDev->searchRange))
                  hChn->tunerIfStep = -(hDev->searchRange);
               hChn->tunerIfStepMax = hChn->dftFreqEstimate + (int32_t)hChn->tunerIfStepSize;
               if (hChn->tunerIfStepMax > (int32_t)hDev->searchRange)
                  hChn->tunerIfStepMax = (int32_t)(hDev->searchRange);
               hChn->tunerIfStepSize = hChn->acqParams.symbolRate / 20; 
               if (hChn->tunerIfStepSize > 1000000)
                  hChn->tunerIfStepSize = 1000000;
               hChn->tunerIfStep -= hChn->tunerIfStepSize;
               
               hChn->dft_current_max_pow = 0;
               hChn->dft_count1 = 0; /* dft_count1 = current max */  
               hChn->dft_funct_state = 1;
               BAST_DEBUG_DFT(BDBG_MSG(("DFT(fine): coarse_est=%d, StepSize=%d", hChn->dftFreqEstimate, hChn->tunerIfStepSize)));               
            }         
            break;

         case 7:    
            hChn->dftFreqEstimateStatus |= BAST_G3_CONFIG_FREQ_ESTIMATE_STATUS_DONE;
            BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eFreqEstDone));
            BDBG_MSG(("DFT freq estimate (chan %d) = %d", h->channel, hChn->dftFreqEstimate)); 
            hChn->tunerIfStep = 0;            
            if (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_BYPASS_DFT_FREQ_EST)
            {
               BDBG_WRN(("DFT: did not transfer freq offset to the LO"));
            }
            else if (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_PRESERVE_COMMANDED_FREQ)
            {
               BDBG_MSG(("DFT: transferring freq offset to FLIF"));
               BAST_CHK_RETCODE(BAST_g3_P_SetFlifOffset(h, hChn->dftFreqEstimate));               
            }
            else
               hChn->tunerIfStep = hChn->dftFreqEstimate;

#ifdef BAST_HAS_WFE
            if (hChn->bOverrideFiltctl)
            {
               hChn->bOverrideFiltctl = true;
               BAST_g3_P_ConfigBl(h);  /* restore FILTCTL and BFOS */
            }
#endif

            hChn->dft_funct_state = 8;                         
            return BAST_g3_P_TunerQuickTune(h, BAST_g3_P_DftSearchCarrierStateMachine);               

         case 8:
            hChn->dft_funct_state = 9;       
            /* tuner settling delay */
            return BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaudUsec, 20000, BAST_g3_P_DftSearchCarrierStateMachine);    
           
         case 9:
            BAST_CHK_RETCODE(BAST_g3_P_LogTraceBuffer(h, BAST_TraceEvent_eRetuneDone));
            
            val = 0;
            if ((hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_PRESERVE_COMMANDED_FREQ) == 0)     
               BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_FLIF, &val);                           
            BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_FLI, &val);    
            BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_PLI, &val);         
            hChn->dft_funct_state = 0xFFFFFFFF;      
            break;
            
         default:
            /* should never get here */
            BDBG_ERR(("BAST_g3_P_DftSearchCarrierStateMachine(): invalid funct_state (%d)", hChn->dft_funct_state));
            BDBG_ASSERT(0);
      }
   }
   
   BAST_CHK_RETCODE(hChn->passFunct(h));
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_DftOqpskSearchCarrierStateMachine()
******************************************************************************/
BERR_Code BAST_g3_P_DftOqpskSearchCarrierStateMachine(BAST_ChannelHandle h)
{
   static const uint32_t script_dft_oqpsk_2[] =
   {
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_RANGE_START, 0),
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_RANGE_END, 0x7FF),
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_CTRL0, 0x02),
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_CTRL0, 0),
      BAST_SCRIPT_WRITE(BCHP_SDS_DFT_CTRL1, 0xD37), /* input to DFT is ADC, offset QPSK */
      BAST_SCRIPT_EXIT
   };
   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;   
   BERR_Code retCode;
   uint32_t peak_pow, total_pow, peak_bin, P_hi, P_lo, Q_hi, Q_lo;  
   
   while (hChn->dft_funct_state <= 3)
   {
      switch (hChn->dft_funct_state)
      {
         case 0:
            hChn->dftFreqEstimateStatus |= BAST_G3_CONFIG_FREQ_ESTIMATE_STATUS_OQPSK;
                   
            if (hChn->acqParams.symbolRate > 11000000)
               hChn->tunerIfStepSize = 3000000; /* 1.5MHz filter * 2 */
            else if (hChn->acqParams.symbolRate > 4800000)
               hChn->tunerIfStepSize = 2600000; /* 1.3MHz filter * 2 */
            else
               hChn->tunerIfStepSize = 2000000; /* 1.0MHz filter * 2 */
               
            hChn->dft_count1 = 0; /* dft_count1 = if step num */   
            hChn->count2 = 0; /* count2 = max snr */   
            hChn->dft_funct_state = 1;
            break;
            
         case 1:
            if (hChn->dft_count1)
               hChn->tunerIfStep = (hChn->tunerIfStepSize >> 1) + (((hChn->dft_count1 - 1) >> 1) * hChn->tunerIfStepSize);
            else
               hChn->tunerIfStep = 0;

            /* check for done */
            if (hChn->tunerIfStep > (int32_t)(hDev->searchRange))
               hChn->dft_funct_state = 7;
            else
            {
               if ((hChn->dft_count1 & 1) == 0)
                  hChn->tunerIfStep = -(hChn->tunerIfStep);

               BAST_CHK_RETCODE(BAST_g3_P_SetFlifOffset(h, hChn->tunerIfStep));  
               hChn->dft_funct_state = 2;   
            }            
            break;
            
         case 2:
            BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_dft_oqpsk_2));      
            BAST_CHK_RETCODE(BAST_g3_P_DftSetDdfsFcw(h));
            hChn->dft_funct_state = 3;
            return BAST_g3_P_DftStartAndWaitForDone(h, BAST_g3_P_DftOqpskSearchCarrierStateMachine);  

         case 3:
            BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_PEAK_POW, &peak_pow);    
            BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_TOTAL_POW, &total_pow);
            BAST_DEBUG_DFT(BDBG_MSG(("DFT OQPSK: %d Hz: peak_pow=%u, total_pow=%u", hChn->tunerIfStep, peak_pow, total_pow)));
            BAST_MultU32U32(peak_pow, 2048, &P_hi, &P_lo);
            BAST_DivU64U32(P_hi, P_lo, total_pow, &Q_hi, &Q_lo);  

            if (Q_lo > hChn->count2)
            {
               hChn->count2 = Q_lo;   
               BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_PEAK_BIN, &peak_bin); 
               peak_bin &= 0x00000FFF;
               if (peak_bin < 0x400)
               {
                  BAST_MultU32U32(hChn->sampleFreq, peak_bin, &P_hi, &P_lo);
                  BAST_DivU64U32(P_hi, P_lo, 65536, &Q_hi, &Q_lo);
                  hChn->dftFreqEstimate = Q_lo;
               }
               else
               {
                  BAST_MultU32U32(hChn->sampleFreq, 2048 - peak_bin, &P_hi, &P_lo);
                  BAST_DivU64U32(P_hi, P_lo, 65536, &Q_hi, &Q_lo);  
                  hChn->dftFreqEstimate = (int32_t)-Q_lo;         
               }
               BAST_DEBUG_DFT(BDBG_MSG(("   -> count2=%u, bin=%u, Q_lo=%u, delta=%d", hChn->count2, peak_bin, Q_lo, hChn->dftFreqEstimate)));
               hChn->dftFreqEstimate += hChn->tunerIfStep;
            }
            hChn->dft_count1++;
            hChn->dft_funct_state = 1;      
            break;
            
         default:
            /* should never get here */
            BDBG_ERR(("BAST_g3_P_DftOqpskSearchCarrierStateMachine(): invalid funct_state (%d)", hChn->dft_funct_state));
            BDBG_ASSERT(0);            
      }
   }
   
   BDBG_ASSERT(hChn->dft_funct_state == 7);
   retCode = BAST_g3_P_DftSearchCarrierStateMachine(h);
   
   done:
   return retCode;
}

  
/******************************************************************************
 BAST_g3_P_DftSearchCarrier()
******************************************************************************/
BERR_Code BAST_g3_P_DftSearchCarrier(BAST_ChannelHandle h, BAST_g3_FUNCT funct)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t nDecFilters;
#ifndef BAST_HAS_WFE   
   uint32_t mdiv, ndiv;
#else
   uint32_t filtctl;
#endif

   hChn->passFunct = funct; /* passFunct = function to call after DFT freq estimate */

   hChn->dftFreqEstimate = 0;
   hChn->dftFreqEstimateStatus = BAST_G3_CONFIG_FREQ_ESTIMATE_STATUS_START;
#ifdef BAST_HAS_WFE   
   hChn->bOverrideFiltctl = false;
#endif

   if ((nDecFilters = BAST_g3_P_GetNumDecimatingFilters(h)) >= 4)
   {
      BDBG_MSG(("%d decimation filters enabled", nDecFilters));
      /* dft h/w problem with 4 or more decimation filters enabled */
#ifndef BAST_HAS_WFE      
      while (nDecFilters >= 4)
      {
         /* divide Fs by 2 */
         BAST_g3_P_GetSampleFreqMN(h, &ndiv, &mdiv);
         mdiv = mdiv << 1;
         BAST_g3_P_SetSampleFreq(h, ndiv, mdiv);

         BAST_g3_P_ConfigBl(h);
         nDecFilters = BAST_g3_P_GetNumDecimatingFilters(h);
         BDBG_MSG(("Change Fs to %u, nDecFilters=%d", hChn->sampleFreq, nDecFilters));
      }
#else
      /* we can't change Fs on WFE, so set filtctl */
      hChn->bOverrideFiltctl = true;
      filtctl = 0x00D8;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_FE_FILTCTL, &filtctl);
      BAST_g3_P_SetBfos(h, 8); /* 8=2^HB, where HB=3 */
#endif      
   }

#ifndef BAST_EXCLUDE_EXT_TUNER
   if (hChn->bExternalTuner)
      hChn->dft_funct_state = 6; 
   else
#endif      
   {
      hChn->dft_funct_state = 0;   
      if ((hChn->acqParams.carrierFreqOffset) || 
          (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_BYPASS_DFT_FREQ_EST))
      {
         if (hChn->acqCount == 0)
         {
            /* warm start */
            hChn->dftFreqEstimate = hChn->acqParams.carrierFreqOffset;
            hChn->dft_funct_state = 7;
         }
         else
            hChn->acqParams.carrierFreqOffset = 0; /* warm start failed previously, so do cold start */
      }
      
      if (hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_OQPSK)
         return BAST_g3_P_DftOqpskSearchCarrierStateMachine(h);
   }
   return BAST_g3_P_DftSearchCarrierStateMachine(h);
}


/******************************************************************************
 BAST_g3_P_DftPeakScanStateMachine()
******************************************************************************/
BERR_Code BAST_g3_P_DftPeakScanStateMachine(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, P_hi, P_lo, Q_hi, Q_lo, total_pow, peak_pow;
     
   next_state:
   /* BAST_DEBUG_PEAK_SCAN(BDBG_MSG(("BAST_g3_P_DftPeakScanStateMachine(): state=%d", hChn->dft_funct_state))); */
   switch (hChn->dft_funct_state)
   {
      case 0:
         hChn->dft_funct_state = 1;
         BAST_g3_P_DftSetDdfsFcw(h);
         return BAST_g3_P_DftStartAndWaitForDone(h, BAST_g3_P_DftPeakScanStateMachine);
         
      case 1:
         BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_PEAK_POW, &peak_pow);
         BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_TOTAL_POW, &total_pow);
         
         BAST_DEBUG_PEAK_SCAN(BDBG_MSG(("PeakScan: %d sym/s: peak_pow=0x%08X, total_pow=0x%08X, peak/total=%f", hChn->acqParams.symbolRate, peak_pow, total_pow, (float)peak_pow/(float)total_pow)));
         if (peak_pow > hChn->peakScanMaxPower)
         {
            hChn->peakScanMaxPower = peak_pow;
            BAST_g3_P_ReadRegister(h, BCHP_SDS_DFT_PEAK_BIN, &val);
            hChn->peakScanMaxPeakBin = val & 0x00000FFF;
            BAST_DEBUG_PEAK_SCAN(BDBG_MSG(("PeakScan: peak_bin=0x%X", hChn->peakScanMaxPeakBin)));
            hChn->peakScanSymRateEst = hChn->acqParams.symbolRate;
         }
         hChn->acqParams.symbolRate += hChn->peakScanIfStepSize;    
         if (hChn->acqParams.symbolRate <= hChn->peakScanSymRateMax)
         {
            hChn->dft_funct_state = 0;
            goto next_state;
         }
         
         if (hChn->peakScanSymRateMax)
         {
            if (hChn->peakScanMaxPeakBin > (hChn->peakScanDftSize >> 1))
            {
               BAST_MultU32U32((uint32_t)(hChn->peakScanDftSize - hChn->peakScanMaxPeakBin), hChn->sampleFreq, &P_hi, &P_lo);
               BAST_DivU64U32(P_hi, P_lo, (uint32_t)hChn->peakScanDftSize * 8, &Q_hi, &Q_lo);  
               Q_lo = (Q_lo + 1) >> 1;               
               hChn->peakScanOutput = hChn->peakScanSymRateEst - Q_lo;               
            }               
            else  /* positive frequency */
            {
               BAST_MultU32U32((uint32_t)hChn->peakScanMaxPeakBin, hChn->sampleFreq, &P_hi, &P_lo);
               BAST_DivU64U32(P_hi, P_lo, (uint32_t)hChn->peakScanDftSize * 8, &Q_hi, &Q_lo);   
               Q_lo = (Q_lo + 1) >> 1;                
               hChn->peakScanOutput = hChn->peakScanSymRateEst + Q_lo;                  
            }            
         }
         else
         {
            hChn->peakScanOutput = hChn->peakScanMaxPeakBin;
         }
         BAST_DEBUG_PEAK_SCAN(BDBG_MSG(("peak_bin=%d, peak_symrate=%d, output=%d", hChn->peakScanMaxPeakBin, hChn->peakScanSymRateEst, hChn->peakScanOutput)));

         BAST_g3_P_DisableChannelInterrupts_isr(h, false, false);   

         hChn->peakScanStatus &= ~BAST_PEAK_SCAN_STATUS_ERROR;
         hChn->peakScanStatus |= BAST_PEAK_SCAN_STATUS_DONE;
         hChn->acqState = BAST_AcqState_eIdle;
         BKNI_SetEvent(hChn->hPeakScanEvent);     
         break;

      default:
         break;      
   }
   return BERR_SUCCESS; 
}


/******************************************************************************
 BAST_g3_P_DftPeakScan()
******************************************************************************/
BERR_Code BAST_g3_P_DftPeakScan(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, ctrl1;
   
   /* reset the DFT */
   val = 0x02;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_CTRL0, &val);
   val = 0x00;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_CTRL0, &val);

   /* configure the DFT */
   if ((hChn->peakScanSymRateMin == 0) || (hChn->peakScanSymRateMax == 0))
   {
      hChn->peakScanSymRateMax = hChn->peakScanSymRateMin = 0;
      val = (uint32_t)(hChn->dftRangeStart & 0x7FF);
      BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_RANGE_START, &val);
      val = (uint32_t)(hChn->dftRangeEnd & 0x7FF);
      BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_RANGE_END, &val);

      /* set to 6 HB */
      BAST_g3_P_ReadModifyWriteRegister(h, BCHP_SDS_FE_FILTCTL, 0xFFFFFF00, 0x60);
      
      ctrl1 = 0x013D;
   }
   else
   {
      val = 0;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_RANGE_START, &val);
      val = 0xFF;
      BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_RANGE_END, &val);
      
      ctrl1 = 0x0197;
   }

   BAST_g3_P_WriteRegister(h, BCHP_SDS_DFT_CTRL1, &ctrl1);

   hChn->acqParams.symbolRate = hChn->peakScanSymRateMin;
   hChn->peakScanDftSize = 256 << (val & 0x03);   
   hChn->peakScanIfStepSize = (uint32_t)((hChn->sampleFreq + 64) >> 7); /* Fs/128 */
   hChn->peakScanMaxPower = 0;
   hChn->peakScanMaxPeakBin = 0;
   hChn->peakScanSymRateEst = hChn->peakScanSymRateMin;
   hChn->dft_funct_state = 0;
   return BAST_g3_P_DftPeakScanStateMachine(h);
}

