/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3_priv_turbo.c $
 * $brcm_Revision: Hydra_Software_Devel/59 $
 * $brcm_Date: 12/18/12 10:43a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3_priv_turbo.c $
 * 
 * Hydra_Software_Devel/59   12/18/12 10:43a enavarro
 * SWSATFE-174: add getLockStatusFunct in channel handle struct
 * 
 * Hydra_Software_Devel/58   12/12/12 3:14p enavarro
 * SWSATFE-234: added dynamic power down of SDS_OPLL
 * 
 * Hydra_Software_Devel/57   11/5/12 3:22p ronchan
 * SWSATFE-174: implement dynamic afec/tfec power management
 * 
 * Hydra_Software_Devel/56   9/11/12 11:09a enavarro
 * SWSATFE-174: fixed snore programming for sds revid 0x64 and later
 * 
 * Hydra_Software_Devel/55   7/17/12 4:15p enavarro
 * SWSATFE-140: changes to support turbo spinv scan h/w
 * 
 * Hydra_Software_Devel/54   6/28/12 3:16p enavarro
 * SWSATFE-174: add 4538 support
 * 
 * Hydra_Software_Devel/53   5/1/12 9:20a ronchan
 * SW7346-804: fixed potential array overrun
 * 
 * Hydra_Software_Devel/52   4/30/12 5:04p ronchan
 * SW7346-804: fixed coverity defect 36895 potential array overrun
 * 
 * Hydra_Software_Devel/51   3/16/12 4:21p enavarro
 * SWSATFE-140: added reacqCause to channel handle struct
 * 
 * Hydra_Software_Devel/50   3/15/12 11:29a enavarro
 * SWSATFE-140: fixed acqCount for turbo scan
 * 
 * Hydra_Software_Devel/49   2/21/12 4:18p ckpark
 * SWSATFE-140: set HPOVERRIDE[3:0]=0; disable delay in applying dafe
 * tracking bw
 * 
 * Hydra_Software_Devel/48   2/20/12 10:06a ckpark
 * SWSATFE-140: updated cldafectl programming delay times
 * 
 * Hydra_Software_Devel/47   2/13/12 4:02p ckpark
 * SWSATFE-140: added code to step CLDAFEINT (not enabled currently)
 * 
 * Hydra_Software_Devel/46   2/13/12 3:21p ckpark
 * SWSATFE-140: increased delay setting final dafe loop bw for  turbo 8psk
 * for better aci performance
 * 
 * Hydra_Software_Devel/45   2/13/12 11:36a ckpark
 * SWSATFE-140: delay setting final dafe loop bw and plc tracking bw for
 * turbo 8psk for better aci performance
 * 
 * Hydra_Software_Devel/44   1/31/12 2:35p ckpark
 * SWSATFE-140: increased DAFE loop bw after HP locks; delay setting
 * tracking DAFE bw and plc for 8psk
 * 
 * Hydra_Software_Devel/43   1/27/12 2:26p enavarro
 * SWSATFE-140: delay setting the tracking plc
 * 
 * Hydra_Software_Devel/42   1/17/12 4:37p enavarro
 * SWSATFE-162: fixed turbo_scan with spinv
 * 
 * Hydra_Software_Devel/41   1/6/12 9:47a ckpark
 * SWSATFE-162: changed TZSY
 * 
 * Hydra_Software_Devel/40   1/4/12 2:13p enavarro
 * SWSATFE-162: fixed bug in turbo code rate scan
 * 
 * Hydra_Software_Devel/39   1/4/12 9:36a ckpark
 * SWSATFE-162: increase turbo sync timeout to 900K baud clocks; tweak HP
 * input scaling for 8psk
 * 
 * Hydra_Software_Devel/38   1/2/12 10:46a ckpark
 * SWSATFE-166: changed tfec retry count
 * 
 * Hydra_Software_Devel/37   1/2/12 10:08a ckpark
 * SWSATFE-166: set CLDAFECTL before/after HP lock, and after TFEC stable
 * lock
 * 
 * Hydra_Software_Devel/36   12/28/11 9:38a enavarro
 * SWSATFE-166: dynamically turn on/off TFEC clock as needed
 * 
 * Hydra_Software_Devel/35   12/27/11 4:48p ckpark
 * SWSATFE-166: optimized eq main tap initial value; calibrated pldctl
 * 
 * Hydra_Software_Devel/34   12/22/11 12:36p enavarro
 * SWSATFE-166: increase TFEC retries to 100
 * 
 * Hydra_Software_Devel/33   12/21/11 2:57p ckpark
 * SWSATFE-166: set TFEC max retries to 50
 * 
 * Hydra_Software_Devel/32   12/20/11 2:06p enavarro
 * SWSATFE-166: set EQ_VCOS
 * 
 * Hydra_Software_Devel/31   12/16/11 10:51a enavarro
 * SWSATFE-162: keep TFEC clock always on for now
 * 
 * Hydra_Software_Devel/30   12/15/11 11:27a ckpark
 * SWSATFE-162: rollback change to EQ_VLCTL
 * 
 * Hydra_Software_Devel/29   12/14/11 4:05p enavarro
 * SWSATFE-162: changes from Hiroshi
 * 
 * Hydra_Software_Devel/28   11/30/11 10:38a enavarro
 * SWSATFE-162: updated TFEC programming per Hiroshi
 * 
 * Hydra_Software_Devel/27   11/22/11 11:02a enavarro
 * SWSATFE-162: changed threshold for reacquisition due to no sync
 * 
 * Hydra_Software_Devel/26   11/18/11 3:28p ckpark
 * SWSATFE-162: update turbo acquisition
 * 
 * Hydra_Software_Devel/25   10/4/11 1:52p ckpark
 * SWSATFE-137: Re-check in, corrected comment line
 * 
 * Hydra_Software_Devel/24   10/4/11 9:59a ckpark
 * SWSATFE-137: SNORE alpha is optimized for Turbo modes
 * 
 * Hydra_Software_Devel/23   8/24/11 2:54p enavarro
 * SWSATFE-140: renamed BAST_g3_P_LdpcAcquireInit() to
 * BAST_g3_P_NonLegacyModeAcquireInit()
 * 
 * Hydra_Software_Devel/22   7/29/11 3:30p guangcai
 * SWSATFE-137: update SNORECTL for turbo
 * 
 * Hydra_Software_Devel/21   5/10/11 3:52p ckpark
 * SWSATFE-118: baud loop tracking BW is widened
 * 
 * Hydra_Software_Devel/20   3/10/11 9:47a ronchan
 * SWSATFE-75: fix compile warning when debug is off
 * 
 * Hydra_Software_Devel/19   1/11/11 10:52a enavarro
 * SWSATFE-75: added workaround for missing tfec out-of-lock irq
 * 
 * Hydra_Software_Devel/18   12/28/10 5:22p enavarro
 * SWSATFE-75: set ldpcScanState to 0
 * 
 * Hydra_Software_Devel/17   12/27/10 3:40p enavarro
 * SWSATFE-75: added support for turbo scan
 * 
 * Hydra_Software_Devel/16   12/23/10 1:48p enavarro
 * SWSATFE-75: initial lab checkout
 * 
 * Hydra_Software_Devel/15   12/8/10 2:17p enavarro
 * SWSATFE-75: added turbo sync detect interrupt
 * 
 * Hydra_Software_Devel/14   12/7/10 6:05p jrubio
 * SW7344-9: fix comments to Standar C
 * 
 * Hydra_Software_Devel/13   11/30/10 3:00p enavarro
 * SWSATFE-75: add reacqCtl config parameter
 * 
 * Hydra_Software_Devel/12   11/24/10 4:49p enavarro
 * SWSATFE-75: copied turbo updates from TT40G
 * 
 * Hydra_Software_Devel/11   10/20/10 4:29p enavarro
 * SWSATFE-10: initial turbo implementation
 * 
 * Hydra_Software_Devel/10   9/22/10 5:19p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/9   9/21/10 3:57p enavarro
 * SWSATFE-75: implemented checkModeFunct for turbo
 * 
 * Hydra_Software_Devel/8   9/21/10 10:30a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/7   9/20/10 5:25p ronchan
 * SWSATFE-75: fixed compile errors
 * 
 * Hydra_Software_Devel/6   9/20/10 5:11p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/5   9/17/10 5:28p ronchan
 * SWSATFE-75: initial compile
 * 
 * Hydra_Software_Devel/4   9/15/10 10:30a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/3   9/14/10 2:29p enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/2   9/8/10 10:34a enavarro
 * SWSATFE-75: ongoing development
 * 
 * Hydra_Software_Devel/1   9/7/10 4:36p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3_priv.h"

BDBG_MODULE(bast_g3_priv_turbo);

#define BAST_DEBUG_TURBO(x) /* x */

#define BAST_TURBO_SYNC_TIMEOUT 900000

#ifndef BAST_EXCLUDE_TURBO 

/* local functions */
BERR_Code BAST_g3_P_TurboOnHpLock(BAST_ChannelHandle h);
BERR_Code BAST_g3_P_TurboOnSyncTimeout(BAST_ChannelHandle h);


/******************************************************************************
 BAST_g3_P_TurboScanTryNextMode()
******************************************************************************/
bool BAST_g3_P_TurboScanTryNextMode(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   bool bIsTurbo8psk;

   hChn->actualMode = BAST_Mode_eUnknown;
   
   next_mode:
   if (hChn->turboScanModes == 0)
      return false;
      
   if (hChn->turboScanCurrMode == 0)
   {
      hChn->turboScanCurrMode = 1;
      hChn->turboScanState &= BAST_TURBO_SCAN_STATE_FIRST_TIME;  
      if (!hChn->bBlindScan && ((hChn->turboScanState & BAST_TURBO_SCAN_STATE_FIRST_TIME) == 0))
         hChn->acqCount++;
   }
   else
   {
      hChn->turboScanCurrMode = (hChn->turboScanCurrMode << 1) & BAST_G3_CONFIG_TURBO_SCAN_MODES_MASK;
      if (hChn->turboScanCurrMode == 0)
      {
         hChn->turboScanState &= ~BAST_TURBO_SCAN_STATE_FIRST_TIME;
         if (hChn->bBlindScan)
            return false; 
      }        
   }
   if (hChn->turboScanCurrMode & hChn->turboScanModes)
   {
      if (hChn->turboScanCurrMode & BAST_G3_CONFIG_TURBO_SCAN_MODES_QPSK_1_2)
         hChn->actualMode = BAST_Mode_eTurbo_Qpsk_1_2;
      else if (hChn->turboScanCurrMode & BAST_G3_CONFIG_TURBO_SCAN_MODES_QPSK_2_3)
         hChn->actualMode = BAST_Mode_eTurbo_Qpsk_2_3;
      else if (hChn->turboScanCurrMode & BAST_G3_CONFIG_TURBO_SCAN_MODES_QPSK_3_4)
         hChn->actualMode = BAST_Mode_eTurbo_Qpsk_3_4;
      else if (hChn->turboScanCurrMode & BAST_G3_CONFIG_TURBO_SCAN_MODES_QPSK_5_6)
         hChn->actualMode = BAST_Mode_eTurbo_Qpsk_5_6;
      else if (hChn->turboScanCurrMode & BAST_G3_CONFIG_TURBO_SCAN_MODES_QPSK_7_8)
         hChn->actualMode = BAST_Mode_eTurbo_Qpsk_7_8;
      else if (hChn->turboScanCurrMode & BAST_G3_CONFIG_TURBO_SCAN_MODES_8PSK_2_3)
         hChn->actualMode = BAST_Mode_eTurbo_8psk_2_3;
      else if (hChn->turboScanCurrMode & BAST_G3_CONFIG_TURBO_SCAN_MODES_8PSK_3_4)
         hChn->actualMode = BAST_Mode_eTurbo_8psk_3_4;
      else if (hChn->turboScanCurrMode & BAST_G3_CONFIG_TURBO_SCAN_MODES_8PSK_4_5)
         hChn->actualMode = BAST_Mode_eTurbo_8psk_4_5;
      else if (hChn->turboScanCurrMode & BAST_G3_CONFIG_TURBO_SCAN_MODES_8PSK_5_6)
         hChn->actualMode = BAST_Mode_eTurbo_8psk_5_6;
      else /* (hChn->turboScanCurrMode & BAST_G3_CONFIG_TURBO_SCAN_MODES_8PSK_8_9) */
         hChn->actualMode = BAST_Mode_eTurbo_8psk_8_9;     

      if (hChn->turboScanState & BAST_TURBO_SCAN_STATE_HP_INIT)
      {
         bIsTurbo8psk = BAST_MODE_IS_TURBO_8PSK(hChn->actualMode) ? true : false;
         if (hChn->turboScanState & BAST_TURBO_SCAN_STATE_HP_LOCKED)
         {
            if (bIsTurbo8psk)
            {
               if ((hChn->turboScanState & BAST_TURBO_SCAN_STATE_8PSK_HP_LOCKED) == 0)
                  goto next_mode;               
            }
            else if (hChn->turboScanState & BAST_TURBO_SCAN_STATE_8PSK_HP_LOCKED)
               goto next_mode;
         }
         else if (hChn->turboScanState & BAST_TURBO_SCAN_STATE_8PSK_FAILED)
         {
            /* only consider qpsk */
            if (bIsTurbo8psk)
               goto next_mode;
         }
         else
         {
            /* only consider 8psk */
            if (!bIsTurbo8psk)
               goto next_mode;
         }
      }
      
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
            BDBG_ERR(("BAST_g3_P_TurboScanTryNextMode(): should not get here!"));
            return false;        
      }
   }
   else
      goto next_mode;

   return true;
}


/******************************************************************************
 BAST_g3_P_TurboConfigCl()
******************************************************************************/
BERR_Code BAST_g3_P_TurboConfigCl(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_CLCTL1, &val);
#if 0
   if (hChn->tunerCtl & BAST_G3_CONFIG_TUNER_CTL_PRESERVE_COMMANDED_FREQ)
   {
      val &= ~0x0000FF00;
      if (BAST_MODE_IS_TURBO_QPSK(hChn->actualMode))
         val |= 0x1E00;
      else
         val |= 0x1600;
   }
   else
   {
      if (BAST_MODE_IS_TURBO_QPSK(hChn->actualMode))
         val |= 0x1A00;
      else
         val |= 0x1200;
   }
#else
   val &= ~0x00000A00;
#endif
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLCTL1, &val);
   
   BAST_g3_P_ReadRegister(h, BCHP_SDS_CL_CLCTL2, &val);
#if 0
   val &= 0x10FFFFFF;
   val |= 0x00006007;
   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
   {
      /* clf_lin_ext = 0 */
      /* set clf_coeff1_ext, clf_pd_ext, clf_int_ext, clb_coeff1_ext, clb_pd_ext, clb_int_ext */
      val |= 0xEE000000;
   }
#else
   val &= ~0xFF000000;
   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      val |= 0xEE000000;
#endif
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLCTL2, &val);
   
   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      BAST_g3_P_AndRegister(h, BCHP_SDS_CL_CLFFCTL, ~0x00000002);
   else
      BAST_g3_P_OrRegister(h, BCHP_SDS_CL_CLFFCTL, 0x00000002);

   BAST_g3_P_AndRegister(h, BCHP_SDS_CL_CLFBCTL, ~0x000000F8);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboConfigEq()
******************************************************************************/
BERR_Code BAST_g3_P_TurboConfigEq(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, i;

   BAST_g3_P_ReadRegister(h, BCHP_SDS_EQ_EQMISCCTL, &val);
   /* new */
   val &= ~0x0038401F;
   val |= 0x00140000;

   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      val |= 0x0000400A; /* err_mode=2, sym_mode=2, set ext_en */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_EQMISCCTL, &val);

   BAST_g3_P_ReadRegister(h, BCHP_SDS_EQ_EQFFECTL, &val);
   val &= 0x0000FF06;
   val |= 0x660C0720;
   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      val |= 0x08;
   else
      val |= 0x10;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_EQFFECTL, &val);

   for (i = 0; i < 24; i++)
   {
      BAST_g3_P_ReadRegister(h, BCHP_SDS_EQ_EQCFAD, &val);
      val &= ~0x1F;
      val |= (0x40 | i);
      BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_EQCFAD, &val);

      val = 0;
      if (i == 0x0C)
      {
         if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
            val = 0x39000000; /* orig: 0x45180000 */
         else
            val = 0x28600000; /* qpsk setting */
      }
      BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_F0B, &val);
   }

   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))   
      val = 0x670A; /* 0x67 = backoff 15% from 0x79 */
   else
      val = 0x750A;    
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_PLDCTL, &val);     
   
   return BERR_SUCCESS; 
}   


/******************************************************************************
 BAST_g3_P_TurboSetOpll()
******************************************************************************/
BERR_Code BAST_g3_P_TurboSetOpll(BAST_ChannelHandle h)
{
   static const uint32_t turbo_opll_N[10] =
   {
      38352000, /* turbo qpsk 1/2 */
      51286400, /* turbo qpsk 2/3 */
      57603200, /* turbo qpsk 3/4 */
      64070400, /* turbo qpsk 5/6 */
      67228800, /* turbo qpsk 7/8 */
      77004800, /* turbo 8psk 2/3 */
      82419200, /* turbo 8psk 3/4 */
      84675200, /* turbo 8psk 4/5 */
      92345600, /* turbo 8psk 5/6 */
      96256000  /* turbo 8psk 8/9 */
   };
   
   
   static const uint32_t turbo_opll_D[10] =
   {
      161699648, /* turbo qpsk 1/2 */
      161699648, /* turbo qpsk 2/3 */ 
      161699648, /* turbo qpsk 3/4 */
      161699648, /* turbo qpsk 5/6 */
      161699648, /* turbo qpsk 7/8 */
      160703040, /* turbo 8psk 2/3 */
      160703040, /* turbo 8psk 3/4 */
      160703040, /* turbo 8psk 4/5 */
      160703040, /* turbo 8psk 5/6 */
      160703040  /* turbo 8psk 8/9 */
   };

   static const uint16_t turbo_number_of_bits_in_block[10] =
   {
      2550, /* turbo qpsk 1/2 */
      3410, /* turbo qpsk 2/3 */
      3830, /* turbo qpsk 3/4 */
      4260, /* turbo qpsk 5/6 */
      4470, /* turbo qpsk 7/8 */
      5120, /* turbo 8psk 2/3 */
      5480, /* turbo 8psk 3/4 */
      5630, /* turbo 8psk 4/5 */
      6140, /* turbo 8psk 5/6 */
      6400  /* turbo 8psk 8/9 */
   };

   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, i, P_hi, P_lo, Q_hi, Q_lo;
   
   i = hChn->actualMode - BAST_Mode_eTurbo_Qpsk_1_2;
   hChn->opll_N = turbo_opll_N[i];
   hChn->opll_D = turbo_opll_D[i];
   
   /* val = HP header symbol */
   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      val = 64;
   else
      val = 128;

   BAST_MultU32U32(hChn->acqParams.symbolRate, turbo_number_of_bits_in_block[i] * 3760 * 4 * 2, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, (val + 10256) * 3893, &Q_hi, &Q_lo);
   hChn->outputBitRate = (Q_lo + 1) >> 1;

   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboSetTitr()
******************************************************************************/
BERR_Code BAST_g3_P_TurboSetTitr(BAST_ChannelHandle h)
{
   static const uint32_t turbo_titr[] =
   {
      0x007F2824,  /* turbo qpsk 1/2 */
      0x00D4C828,  /* turbo qpsk 2/3 */
      0x00BF4830,  /* turbo qpsk 3/4 */
      0x00D4C836,  /* turbo qpsk 5/6 */
      0x00DF483A,  /* turbo qpsk 7/8 */
      0x00FF8024,  /* turbo 8psk 2/3 */
      0x01118028,  /* turbo 8psk 3/4 */
      0x01190030,  /* turbo 8psk 4/5 */
      0x01328036,  /* turbo 8psk 5/6 */
      0x013F803A   /* turbo 8psk 8/9 */  
   };

   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, i;

   i = hChn->actualMode - BAST_Mode_eTurbo_Qpsk_1_2;
   val = turbo_titr[i];
   BAST_g3_P_WriteRegister(h, BCHP_TFEC_TITR, &val);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboSetTtur()
******************************************************************************/
BERR_Code BAST_g3_P_TurboSetTtur(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, iter, P_hi, P_lo, Q_hi, Q_lo;
   
   /* 0.5 + 16/256 = 0.5625 = 9/16 */
   /* iter = (uint32_t)((float)fec_freq/(0.5625 * (float)acq_symbol_rate) - 1.0); */
   BAST_MultU32U32(16, hChn->fecFreq, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 9 * hChn->acqParams.symbolRate, &Q_hi, &Q_lo);
   iter = Q_lo - 1;
   if (iter > 19)
      iter = 19;
   val = 0x00000A03 | (iter << 16);  
   BAST_g3_P_WriteRegister(h, BCHP_TFEC_TTUR, &val);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboSetTssq()
******************************************************************************/
BERR_Code BAST_g3_P_TurboSetTssq(BAST_ChannelHandle h)
{
   static const uint8_t turbo_tssq_qpsk_1_2[2] =
   {
      0x01,
      0xDD
   };

   static const uint8_t turbo_tssq_qpsk_2_3[4] =
   {
      0x03,
      0xDD,
      0xDD,
      0xEE
   };

   static const uint8_t turbo_tssq_qpsk_3_4[5] =
   {
      0x04,
      0xED,
      0xED,
      0xDE,
      0xDE
   };

   static const uint8_t turbo_tssq_qpsk_5_6[7] =
   {
      0x06,
      0xED,
      0xDE,
      0xDE,
      0xEE,
      0xEE,
      0xED
   };

   static const uint8_t turbo_tssq_qpsk_7_8[9] =
   {
      0x08,
      0xED,
      0xED,
      0xDE,
      0xDE,
      0xEE,
      0xEE,
      0xEE,
      0xEE
   };

   static const uint8_t turbo_tssq_8psk_2_3[6] =
   {
      0x05,
      0x00,
      0x00,
      0x00,
      0x00,
      0x22
   };
   
   static const uint8_t turbo_tssq_8psk_3_4[8] =
   {
      0x07,
      0x00,
      0x00,
      0x20,
      0x10,
      0x00,
      0x01,
      0x02
   };

   static const uint8_t turbo_tssq_8psk_4_5[6] =
   {
      0x05,
      0x10,
      0x00,
      0x00,
      0x01,
      0x22
   };

   static const uint8_t turbo_tssq_8psk_5_6[6] =
   {
      0x05,
      0x10,
      0x01,
      0x10,
      0x01,
      0x22
   };

   static const uint8_t turbo_tssq_8psk_8_9[7] =
   {
      0x06,
      0x10,
      0x01,
      0x21,
      0x10,
      0x01,
      0x12
   };

   static const uint8_t* turbo_tssq_table[10] =
   {
      (const uint8_t *)turbo_tssq_qpsk_1_2,
      (const uint8_t *)turbo_tssq_qpsk_2_3,
      (const uint8_t *)turbo_tssq_qpsk_3_4,
      (const uint8_t *)turbo_tssq_qpsk_5_6,
      (const uint8_t *)turbo_tssq_qpsk_7_8,
      (const uint8_t *)turbo_tssq_8psk_2_3,
      (const uint8_t *)turbo_tssq_8psk_3_4,
      (const uint8_t *)turbo_tssq_8psk_4_5,
      (const uint8_t *)turbo_tssq_8psk_5_6,
      (const uint8_t *)turbo_tssq_8psk_8_9
   };

   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, i;
   const uint8_t *pTable;
   uint8_t len;

   i = hChn->actualMode - BAST_Mode_eTurbo_Qpsk_1_2;
   pTable = turbo_tssq_table[i];

   len = *pTable++;
   for (i = (uint32_t)len; i; i--)
   {
      val = (uint32_t)(*pTable++);
      BAST_g3_P_WriteRegister(h, BCHP_TFEC_TSSQ, &val);
   }
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboConfigTfec()
******************************************************************************/
BERR_Code BAST_g3_P_TurboConfigTfec(BAST_ChannelHandle h)
{
   static const uint32_t script_turbo_2[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TFEC_GR_BRIDGE_SW_INIT_0, 1),
      BAST_SCRIPT_WRITE(BCHP_TFEC_GR_BRIDGE_SW_INIT_0, 0),  
      BAST_SCRIPT_WRITE(BCHP_TFEC_TFECTL, 0x80),
      BAST_SCRIPT_EXIT
   };

   static const uint32_t script_turbo_3[] =
   {   
      BAST_SCRIPT_WRITE(BCHP_TFEC_TCIL, 0x00009FCC),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TRSD, 0x00004FCC),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TZPK, 0x03B58F34),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TFMT, 0x00028008),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TSYN, 0x0103FEFE),
      BAST_SCRIPT_WRITE(BCHP_TFEC_TPAK, 0x0009BB47),
      BAST_SCRIPT_EXIT
   }; 

   static const uint32_t script_turbo_4[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TFEC_TZSY, 0x00020E0F), /* 0x0002070F */
      /* BAST_SCRIPT_WRITE(BCHP_TFEC_TFECTL, 0xC0),  */
      BAST_SCRIPT_WRITE(BCHP_TFEC_TFECTL, 0x00),        
      BAST_SCRIPT_WRITE(BCHP_TFEC_TFECTL, 0x40),  
      BAST_SCRIPT_EXIT      
   };

   BERR_Code retCode;

   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_turbo_2));  
   BAST_CHK_RETCODE(BAST_g3_P_TurboSetTitr(h));
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_turbo_3));   
   BAST_CHK_RETCODE(BAST_g3_P_TurboSetTtur(h));
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_turbo_4));
   BAST_CHK_RETCODE(BAST_g3_P_TurboSetTssq(h));  
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TurboRun()
******************************************************************************/
BERR_Code BAST_g3_P_TurboRun(BAST_ChannelHandle h)
{
   BERR_Code retCode;
   
   static const uint32_t script_turbo_6[] =
   {
      BAST_SCRIPT_WRITE(BCHP_TFEC_INTR2_CPU_CLEAR, 0x000000FF),       
      BAST_SCRIPT_WRITE(BCHP_TFEC_TFECTL, 0x63), /* reset error counters */
      BAST_SCRIPT_WRITE(BCHP_TFEC_TFECTL, 0x61), /* clear error counter reset */      
      BAST_SCRIPT_EXIT      
   };
   retCode = BAST_g3_P_ProcessScript(h, script_turbo_6);   
   if (retCode == BERR_SUCCESS)
      retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, BAST_TURBO_SYNC_TIMEOUT, BAST_g3_P_TurboOnSyncTimeout);   
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TurboEnableSyncInterrupt()
******************************************************************************/
BERR_Code BAST_g3_P_TurboEnableSyncInterrupt(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BINT_EnableCallback_isr(hChn->hTurboSyncCb);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboDisableSyncInterrupt()
******************************************************************************/
BERR_Code BAST_g3_P_TurboDisableSyncInterrupt(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BINT_DisableCallback_isr(hChn->hTurboSyncCb);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboSync_isr() - callback routine for turbo sync detected
******************************************************************************/
void BAST_g3_P_TurboSync_isr(void *p, int int_id)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;

   BAST_g3_P_DisableTimer(h, BAST_TimerSelect_eBaud); /* disable sync timeout */
   BAST_g3_P_TurboDisableSyncInterrupt(h);
   
   BAST_g3_P_IncrementInterruptCounter(h, int_id);   
     
   if (hChn->count1 > 0)
   {
      BAST_DEBUG_TURBO(BDBG_WRN(("BAST_g3_P_TurboSync_isr(): resync count=%d", hChn->count1)));
   }
   
   BAST_g3_P_StartTracking(h);
}

#endif


/******************************************************************************
 BAST_g3_P_TurboAcquire()
******************************************************************************/
BERR_Code BAST_g3_P_TurboAcquire(BAST_ChannelHandle h)
{
#ifndef BAST_EXCLUDE_TURBO 
   static const uint32_t script_turbo_1[] =
   {
#if 1      
      /* orig */
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_HD8PSK1, 0x01D901D9),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_HD8PSK2, 0x00C400C4),
#else
      /* 65 nm settings */
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_HD8PSK1, 0x014e01d9),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_HD8PSK2, 0x008b00c4),
#endif
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_HDQPSK, 0x01000000),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_XTAP1, 0x00000100),
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_XTAP2, 0x00805000),   
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_PILOTCTL, 0x00000004),   
      BAST_SCRIPT_AND(BCHP_SDS_EQ_EQMISCCTL, ~0x60), /* for now, disable soft pd tables */
      BAST_SCRIPT_WRITE(BCHP_SDS_EQ_VCOS, 0x76303000),         
      BAST_SCRIPT_EXIT
   };

#if 0
   static const uint32_t script_turbo_2[] = /* this script changes the TFEC clock */
   {
      BAST_SCRIPT_WRITE(BCHP_TFEC_GR_BRIDGE_SW_INIT_0, 1),
      BAST_SCRIPT_WRITE(BCHP_TFEC_GR_BRIDGE_SW_INIT_0, 0),  
#if 1    
      /* 246 MHz */
      BAST_SCRIPT_WRITE(BCHP_TFEC_MISC_POST_DIV_CTL, 0x0904), /* clock disable, mdiv=9 (246MHz), default is 0xa (221.4MHz) */
      BAST_SCRIPT_WRITE(BCHP_TFEC_MISC_POST_DIV_CTL, 0x0905), /* mdiv load enable */
      BAST_SCRIPT_WRITE(BCHP_TFEC_MISC_POST_DIV_CTL, 0x0904), 
      BAST_SCRIPT_WRITE(BCHP_TFEC_MISC_POST_DIV_CTL, 0x0900), /* clock enable */    
#else
      /* 201.27 MHz */
      BAST_SCRIPT_WRITE(BCHP_TFEC_MISC_POST_DIV_CTL, 0x0b04), /* clock disable, mdiv=0xb (201.2MHz), default is 0xa (221.4MHz) */
      BAST_SCRIPT_WRITE(BCHP_TFEC_MISC_POST_DIV_CTL, 0x0b05), /* mdiv load enable */
      BAST_SCRIPT_WRITE(BCHP_TFEC_MISC_POST_DIV_CTL, 0x0b04), 
      BAST_SCRIPT_WRITE(BCHP_TFEC_MISC_POST_DIV_CTL, 0x0b00), /* clock enable */    
#endif
      BAST_SCRIPT_EXIT
   };
#endif
   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;   
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val;

   if (hChn->bHasTfec == false)
   {
      turbo_acquire_failed:
      if (hChn->bBlindScan)
      {
         hChn->reacqCause = BAST_ReacqCause_eNoTfec;
         return BAST_g3_P_Reacquire(h);
      }
      else
         return BAST_g3_P_AbortAcq(h);
   }

   if (hChn->acqParams.mode == BAST_Mode_eTurbo_scan)
   {         
      if ((hDev->sdsRevId >= 0x68) || ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_SPINV_IQ_SCAN) != BAST_ACQSETTINGS_SPINV_IQ_SCAN) ||
          (((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_SPINV_IQ_SCAN) == BAST_ACQSETTINGS_SPINV_IQ_SCAN) &&
           (hChn->bTurboSpinv == false)))
      {
         if (BAST_g3_P_TurboScanTryNextMode(h) == false)
            goto turbo_acquire_failed;
      }
   }
   else
      hChn->turboScanState = 0;

#ifndef BAST_EXCLUDE_LDPC
   hChn->ldpcScanState = 0;
#endif

#if 1 /* dynamically turn on/off TFEC clock as needed */
   BAST_CHK_RETCODE(BAST_g3_P_TurboPowerUp(h));
#endif
#if 0 /* enable this to override default tfec clock */
   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_turbo_2));   
#endif
   BAST_CHK_RETCODE(BAST_g3_P_GetTfecClock(h, &(hChn->fecFreq)));
   BAST_CHK_RETCODE(BAST_g3_P_NonLegacyModeAcquireInit(h));
   BAST_CHK_RETCODE(BAST_g3_P_TurboConfigCl(h));
   BAST_CHK_RETCODE(BAST_g3_P_TurboConfigEq(h));   
   BAST_g3_P_ToggleBit(h, BCHP_SDS_EQ_EQFFECTL, 1); /* reset the eq */   

#if 0
   /* 65 nm setting */
   val = 0x00040904;
#else   
   val = 0x00000904;
#endif
   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      val |= 0x00010000;
   BAST_g3_P_WriteRegister(h, BCHP_SDS_EQ_VLCTL, &val);

   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_turbo_1));
   BAST_CHK_RETCODE(BAST_g3_P_ConfigPlc(h, true)); /* set acquisition plc */ 
    
   /* Chan: before HP lock */    
   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      val = 0x00005088; /*0x00005066*/ /*5088, 396/211 */
   else
      val = 0x0000406A; /*406a;15, 2048;11*/ /* 6048:18, 606a:14 , 806A:21, 206a;10, 208c; 12*/
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLDAFECTL, &val);
   
   /* configure and run the HP */
   retCode = BAST_g3_P_HpAcquire(h, BAST_g3_P_TurboOnHpLock);
   
   done:
   return retCode;
#else
   return BERR_NOT_SUPPORTED;
#endif
}


#ifndef BAST_EXCLUDE_TURBO
/******************************************************************************
 BAST_g3_P_TurboOnSyncTimeout()
******************************************************************************/
BERR_Code BAST_g3_P_TurboOnSyncTimeout(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

   BAST_CHK_RETCODE(BAST_g3_P_TurboDisableSyncInterrupt(h));

   hChn->count1++;
   if (BAST_g3_P_IsHpLocked(h))
   {
      if (hChn->count1 < 5)
      {     
         /* reprogram the TFEC */       
         BAST_CHK_RETCODE(BAST_g3_P_TurboConfigTfec(h));   
         BAST_CHK_RETCODE(BAST_g3_P_TurboEnableSyncInterrupt(h));
         retCode = BAST_g3_P_TurboRun(h);
      }
      else
      {
         hChn->count2++;
         if ((hChn->acqParams.mode == BAST_Mode_eTurbo_scan) && (hChn->count2 > 1))
         {
            hChn->reacqCause = BAST_ReacqCause_eTurboSyncTimeout;
            goto reacquire;
         }
         BAST_DEBUG_TURBO(BDBG_WRN(("reacquiring HP since TFEC can't lock")));
         BAST_CHK_RETCODE(BAST_g3_P_HpEnable(h, false));
         retCode = BAST_g3_P_TurboAcquire(h); /* reacquire HP */
      } 
   }
   else
   {
      hChn->reacqCause = BAST_ReacqCause_eHpLostLock2;
      
      reacquire:
      if ((hChn->acqParams.acq_ctl & BAST_ACQSETTINGS_SPINV_IQ_SCAN) == BAST_ACQSETTINGS_SPINV_IQ_SCAN)
         hChn->bTurboSpinv = false;
      retCode = BAST_g3_P_Reacquire(h);
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TurboOnHpLock() - called when HP locks
******************************************************************************/
BERR_Code BAST_g3_P_TurboOnHpLock(BAST_ChannelHandle h)
{
   static const uint32_t script_turbo_5[] =
   {
      BAST_SCRIPT_AND(BCHP_SDS_EQ_EQMISCCTL, 0xFFFFFBFF),           /* disable CMA */
      BAST_SCRIPT_AND_OR(BCHP_SDS_EQ_EQFFECTL, 0xFFFF00FF, 0x0200), /* unfreze other taps */
#if 0      
      BAST_SCRIPT_OR(BCHP_SDS_HP_HPOVERRIDE, 0x0000000F),           /* override front carrier loop */ 
#endif      
      BAST_SCRIPT_AND(BCHP_SDS_CL_CLCTL1, 0xFFFFFFEF),              /* disable front carrier loop */
      BAST_SCRIPT_OR(BCHP_SDS_CL_CLCTL2, 0x00000004),               /* freeze front carrier loop */
      BAST_SCRIPT_EXIT
   };
   
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BAST_g3_P_Handle *hDev = h->pDevice->pImpl;   
   uint32_t val;
   BERR_Code retCode;

   if (hChn->acqParams.mode == BAST_Mode_eTurbo_scan)
   {
      hChn->turboScanState |= BAST_TURBO_SCAN_STATE_HP_LOCKED;
      if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
         hChn->turboScanState |= BAST_TURBO_SCAN_STATE_8PSK_HP_LOCKED;
   }

   /* Chan: after HP locks */         
   if (hChn->actualMode <= BAST_Mode_eTurbo_Qpsk_2_3)
      val = 0x0000406A; /*406A; 17, 206A;15, 606A;12,20, 4048 no good, 408C:BAD */
   else if (hChn->actualMode <= BAST_Mode_eTurbo_Qpsk_7_8)
      val = 0x0000406A;
   else if (hChn->actualMode <= BAST_Mode_eTurbo_8psk_3_4)
      val = 0x0000408A; /*, was 0x406A, need to make DAFE converge fast *//* 4066, 50AA, 5066, 8PSK never lock, 406A is OK,  */
   else
      val = 0x0000408A;  /* was 506A */    
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLDAFECTL, &val); 

   BAST_CHK_RETCODE(BAST_g3_P_ProcessScript(h, script_turbo_5));   
   if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      val = 0x91;
   else
      val = 0x81;

   if (hDev->sdsRevId >= 0x64)
      val |= 0x08;
       
   BAST_g3_P_WriteRegister(h, BCHP_SDS_SNR_SNORECTL, &val);   

   BAST_g3_P_SdsPowerUpOpll(h);
   BAST_CHK_RETCODE(BAST_g3_P_TurboSetOpll(h));   
   BAST_CHK_RETCODE(BAST_g3_P_ConfigOif(h));   
   BAST_CHK_RETCODE(BAST_g3_P_SetAgcTrackingBw(h));
      
   /* set up the TFEC */
   BAST_CHK_RETCODE(BAST_g3_P_TurboConfigTfec(h));

   /* wait for sync with timeout */
   hChn->count1 = 0; /* count1=number of times we failed to get sync */
   BAST_CHK_RETCODE(BAST_g3_P_TurboEnableSyncInterrupt(h)); 

   /* need a delay here for some reason... */
   retCode = BAST_g3_P_EnableTimer(h, BAST_TimerSelect_eBaud, 20000, BAST_g3_P_TurboRun);   

   done:
   return retCode;   
}


/******************************************************************************
 BAST_g3_P_TurboUpdateErrorCounters()
******************************************************************************/
BERR_Code BAST_g3_P_TurboUpdateErrorCounters(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   if (BAST_g3_P_IsTurboOn(h))
   {
      BAST_g3_P_ReadRegister(h, BCHP_TFEC_TNBLK, &val);
      hChn->turboTotalBlocks += val;
      
      BAST_g3_P_ReadRegister(h, BCHP_TFEC_TCBLK, &val);
      hChn->turboCorrBlocks += val;
      
      BAST_g3_P_ReadRegister(h, BCHP_TFEC_TBBLK, &val);
      hChn->turboBadBlocks += val;
      
      /* reset the FEC error counters */
      BAST_g3_P_ToggleBit(h, BCHP_TFEC_TFECTL, 0x06); 
   }
   return BAST_g3_P_UpdateErrorCounters(h);      
}


/******************************************************************************
 BAST_g3_P_TurboCheckMode() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TurboCheckMode(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t i; 
   uint16_t mask;
   
   if (BAST_MODE_IS_TURBO(hChn->actualMode))
   {
      if (hChn->actualMode == BAST_Mode_eTurbo_scan)
         goto invalid_condition;
         
      i = hChn->actualMode - BAST_Mode_eTurbo_Qpsk_1_2;
      mask = (uint16_t)(1 << i);
      if ((mask & hChn->turboScanModes) == 0)
      {
         hChn->reacqCause = BAST_ReacqCause_eInvalidMode;
         hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE;
      }
   }
   else
   {
      invalid_condition:
      BDBG_ERR(("BAST_g3_P_TurboCheckMode() - invalid condition"));
      BERR_TRACE(retCode = BAST_ERR_AP_UNKNOWN);
   }

   return retCode;
}


/******************************************************************************
 BAST_g3_P_TurboOnLock() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TurboOnLock(BAST_ChannelHandle h)
{
   BSTD_UNUSED(h);

   BAST_DEBUG_TURBO(BDBG_MSG(("TFEC locked")));   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboOnLostLock() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TurboOnLostLock(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
      
   BAST_CHK_RETCODE(BAST_g3_P_TurboUpdateErrorCounters(h));
   
   if (BAST_g3_P_IsHpLocked(h) == false)
   {
      hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE;
      hChn->reacqCause = BAST_ReacqCause_eHpLostLock4;
      BAST_DEBUG_TURBO(BDBG_MSG(("HP lost lock")));
   }
   else
   {
      BAST_DEBUG_TURBO(BDBG_MSG(("TFEC lost lock")));      
   }
      
   done:
   return retCode;
}


/******************************************************************************
 BAST_g3_P_TurboOnStableLock() - ISR context
******************************************************************************/
BERR_Code BAST_g3_P_TurboOnStableLock(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;

#if 0
   uint32_t val;
   
   /* narrow DAFE loop bw */
BDBG_ERR(("set dafectl"));   
  if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      val = 0x00004066; /* 3066; bad for 8PSK, never lock*/
   else
      val = 0x00003063; /* 3066; *//*3038;16, 5034;17, 5056 */ /* 406A fell out of lock QPSK 1/2, 6A fell out of lock */
   BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLDAFECTL, &val);
#endif

   /* set tracking baud loop bw */
   BAST_CHK_RETCODE(BAST_g3_P_SetBaudBw(h, hChn->acqParams.symbolRate / 400, 4));
  
   if (hChn->bEverStableLock == false)
   {
      BAST_CHK_RETCODE(BAST_g3_P_TurboUpdateErrorCounters(h));
      hChn->turboTotalBlocks = 0;
      hChn->turboCorrBlocks = 0;
      hChn->turboBadBlocks = 0;   
   }
   
   done:
   return retCode;
}


#ifndef BAST_HAS_WFE
/******************************************************************************
 BAST_g3_P_GetTfecClock() - updates fecFreq
******************************************************************************/
BERR_Code BAST_g3_P_GetTfecClock(BAST_ChannelHandle h, uint32_t *pFreq)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val;
   BERR_Code retCode = BERR_SUCCESS;
   
   if (BAST_g3_P_IsTurboOn(h))
   {
      BAST_g3_P_ReadRegister(h, BCHP_TFEC_MISC_POST_DIV_CTL, &val);
      *pFreq = hChn->vcoRefClock / ((val >> 8) & 0x000000FF);
   }
   else
   {
      *pFreq = 0;
      BDBG_ERR(("BAST_g3_P_GetTfecClock(): TFEC not powered"));
      BERR_TRACE(retCode = BAST_ERR_POWERED_DOWN);
   }
   return retCode;
}


/******************************************************************************
 BAST_g3_P_IsTurboOn()
******************************************************************************/
bool BAST_g3_P_IsTurboOn(BAST_ChannelHandle h)
{
   uint32_t val;
   
   BAST_g3_P_ReadRegister(h, BCHP_TFEC_MISC_POST_DIV_CTL, &val);
   if (val & 0x04)
      return false;
   return true;
}


/******************************************************************************
 BAST_g3_P_TurboPowerUp() - power up the tfec core
******************************************************************************/ 
BERR_Code BAST_g3_P_TurboPowerUp(BAST_ChannelHandle h)
{
   BERR_Code retCode;
   
   BAST_g3_P_AndRegister(h, BCHP_TFEC_MISC_POST_DIV_CTL, ~0x4);
   if (BAST_g3_P_IsTurboOn(h))
      retCode = BERR_SUCCESS;
   else
   {
      BERR_TRACE(retCode = BAST_ERR_HAB_CMD_FAIL);
      BDBG_ERR(("BAST_g3_P_TurboPowerUp() failed"));
   }
   return retCode;     
}


/******************************************************************************
 BAST_g3_P_TurboPowerDown() - power down the tfec core
******************************************************************************/ 
BERR_Code BAST_g3_P_TurboPowerDown(BAST_ChannelHandle h)
{
   BERR_Code retCode;
   
   BAST_g3_P_OrRegister(h, BCHP_TFEC_MISC_POST_DIV_CTL, 0x4);
   if (BAST_g3_P_IsTurboOn(h) == false)
      retCode = BERR_SUCCESS;
   else
   {
      BERR_TRACE(retCode = BAST_ERR_HAB_CMD_FAIL);
      BDBG_ERR(("BAST_g3_P_TurboPowerDown() failed"));
   }
   return retCode;   
}
#endif


/******************************************************************************
 BAST_g3_P_TurboEnableLockInterrupts()
******************************************************************************/
BERR_Code BAST_g3_P_TurboEnableLockInterrupts(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BINT_EnableCallback_isr(hChn->hTurboLockCb);
   BINT_EnableCallback_isr(hChn->hTurboNotLockCb);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboDisableLockInterrupts()
******************************************************************************/
BERR_Code BAST_g3_P_TurboDisableLockInterrupts(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   BINT_DisableCallback_isr(hChn->hTurboLockCb);
   BINT_DisableCallback_isr(hChn->hTurboNotLockCb);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboMonitorLock()
******************************************************************************/
/* #define BAST_DELAY_DAFE_TRK_BW */
BERR_Code BAST_g3_P_TurboMonitorLock(BAST_ChannelHandle h)
{
#ifdef BAST_DELAY_DAFE_TRK_BW
   static uint32_t t0_Fb_table[8] = {5500000, 5000000, 4500000, 4000000, 3000000, 2500000, 2100000, 0};
   static uint32_t t0_table[8] =    {0,       1,       2,       4,       5,       8      , 10,      10};
   static uint32_t t1_Fb_table[6] = {5000000, 4500000, 4200000, 2500000, 2100000, 0};
   static uint32_t t1_table[6] =    {0,       1,       2,       4,       10,      10};
#endif
    
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   uint32_t val, t0, t1; 
#ifdef BAST_DELAY_DAFE_TRK_BW   
   uint32_t i;
#endif
   
   if (BAST_g3_P_IsHpLocked(h) == false)
   {
      BDBG_MSG(("HP fell out of lock"));
      hChn->reacqCause = BAST_ReacqCause_eHpLostLock4;
      hChn->reacqCtl |= BAST_G3_CONFIG_REACQ_CTL_FORCE;
   }   
   else
   {
      /* determine the time for setting cldafectl */     
#ifdef BAST_DELAY_DAFE_TRK_BW    
      if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
      {
         /* turbo 8psk */
#if 0
         t0 = hChn->debug1;
         t1 = hChn->debug2;
#else         
         for (i = 0; i < 7; i++)
         {
            if (hChn->acqParams.symbolRate > t0_Fb_table[i])
               break;
         }
         if ((i <= 1) || (i >= 7))
            t0 = t0_table[i];
         else
         {
            /* interpolate */
            t0 = ((t0_table[i] - t0_table[i-1]) * (hChn->acqParams.symbolRate - t0_Fb_table[i]) * 2) / (t0_Fb_table[i-1] - t0_Fb_table[i]);
            t0 = (t0 + 1) >> 1;
            t0 = t0_table[i] - t0;
         }  
           
         /* determine the time after t0 to set tracking plc */
         for (i = 0; i < 5; i++)
         {
            if (hChn->acqParams.symbolRate > t1_Fb_table[i])
               break;
         }
         if ((i <= 1) || (i >= 5))
            t1 = t1_table[i];
         else
         {
            /* interpolate */
            t1 = ((t1_table[i] - t1_table[i-1]) * (hChn->acqParams.symbolRate - t1_Fb_table[i]) * 2) / (t1_Fb_table[i-1] - t1_Fb_table[i]);
            t1 = (t1 + 1) >> 1;
            t1 = t1_table[i] - t1;         
         }   
#endif   
      }
      else
#endif      
         t0 = t1 = 0;
    
      if (hChn->timeSinceStableLock == t0) /* 5Mbaud=2, */
      {
/* BDBG_MSG(("setting cldafectl, t0=%d, t1=%d", t0, t1));  */
         if (BAST_MODE_IS_TURBO_8PSK(hChn->actualMode))
            val = 0x00004066; /*5066; *//* 3066; bad for 8PSK, never lock*/ /* 406A */
         else
            val = 0x00003063; /* 3066; *//*3038;16, 5034;17, 5056 */ /* 406A fell out of lock QPSK 1/2, 6A fell out of lock */
         BAST_g3_P_WriteRegister(h, BCHP_SDS_CL_CLDAFECTL, &val);
      }
         
      /* set tracking PLC lock */
      if (hChn->bPlcTracking == false)
      {         
         if (hChn->timeSinceStableLock > (t0 + t1))
         {
/* BDBG_MSG(("set tracking plc, t=%d", t0+t1)); */         
            BAST_g3_P_ConfigPlc(h, false); /* set tracking PLC */  
         }      
      }
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_TurboSetFunctTable()
******************************************************************************/ 
BERR_Code BAST_g3_P_TurboSetFunctTable(BAST_ChannelHandle h)
{
   BAST_g3_P_ChannelHandle *hChn = (BAST_g3_P_ChannelHandle *)h->pImpl;
   
   hChn->acqFunct = BAST_g3_P_TurboAcquire;
   hChn->checkModeFunct = BAST_g3_P_TurboCheckMode;
   hChn->onLockFunct = BAST_g3_P_TurboOnLock;
   hChn->onLostLockFunct = BAST_g3_P_TurboOnLostLock;
   hChn->onStableLockFunct = BAST_g3_P_TurboOnStableLock;
   hChn->onMonitorLockFunct = BAST_g3_P_TurboMonitorLock;
   hChn->enableLockInterrupts = BAST_g3_P_TurboEnableLockInterrupts;
   hChn->disableLockInterrupts = BAST_g3_P_TurboDisableLockInterrupts;
   hChn->getLockStatusFunct = NULL;
   
   return BERR_SUCCESS;
}

#endif
