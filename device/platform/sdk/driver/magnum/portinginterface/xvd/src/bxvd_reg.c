/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_reg.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 7/20/11 3:04p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_reg.c $
 * 
 * Hydra_Software_Devel/12   7/20/11 3:04p davidp
 * SW7420-2001: Reorder header file includes.
 * 
 * Hydra_Software_Devel/11   6/27/11 1:30p davidp
 * SW7425-628: Fix GISB workaround non SVD capable write_isr case.
 * 
 * Hydra_Software_Devel/10   6/25/11 9:51a davidp
 * SW7425-628: Change BKNI_Printfs to BDBG_MSGs.
 * 
 * Hydra_Software_Devel/9   6/24/11 6:55p davidp
 * SW7425-628: Add delay and retry to BXVD_Reg_Write32_isr GISB timeout
 * workaround.
 * 
 * Hydra_Software_Devel/8   6/24/11 8:07a davidp
 * SW7425-628: Only perform GISB timeout workaround on SVD.
 * 
 * Hydra_Software_Devel/7   6/23/11 6:03p davidp
 * SW7425-615: Increase write error delay to 50 usec.
 * 
 * Hydra_Software_Devel/6   6/21/11 3:43p btosi
 * SW7425-615: added GISB error check to the register write routines
 * 
 * Hydra_Software_Devel/5   6/17/11 11:40a btosi
 * SW7425-615: fixed converity issue
 * 
 * Hydra_Software_Devel/4   6/16/11 4:30p davidp
 * SW7425-615: Add and use BXVD_P_GISB_ERR_WORKAROUND symbolic constant.
 * 
 * Hydra_Software_Devel/3   6/16/11 11:21a btosi
 * SW7425-615: added retry mechanism for register reads that fail
 * 
 * Hydra_Software_Devel/2   7/15/05 1:08p pblanco
 * PR16052: Clean build with new code and data.
 * 
 * Hydra_Software_Devel/1   7/13/05 12:30p pblanco
 * PR16052: Initial checkin.
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd.h"
#include "bxvd_reg.h"

BDBG_MODULE(BXVD_REG);

#define BXVD_REG_READ_MAX_RETRIES            10
#define BXVD_REG_WRITE_POST_FAILURE_DELAY    5 /* wait this many usecs for the write to complete */

uint32_t BXVD_Reg_Read32_isr(BXVD_Handle hXvd, uint32_t offset)
{
   uint32_t uiValue;

#if BXVD_P_SVD_GISB_ERR_WORKAROUND

   if (hXvd->bSVCCapable)
   {
      uint32_t uiLoop;
      bool bSuccess=true;

      for ( uiLoop=0; uiLoop < BXVD_REG_READ_MAX_RETRIES; uiLoop++ )
      {

         /* SW7425-628: work around for the bus error caused by register reads. */
         uint32_t uiStatus;

         uiValue = BREG_Read32_isr(hXvd->hReg, offset);

         /* Check if the AVD_RGR_BRIDGE_INTR bit is set in the CPU status register. */
         uiStatus = BREG_Read32_isr(hXvd->hReg, BCHP_SVD_INTR2_0_CPU_STATUS);

         if ( uiStatus & BCHP_SVD_INTR2_0_CPU_STATUS_AVD_RGR_BRIDGE_INTR_MASK )
         {
            /* If the bit is set, the read failed.  Clear the AVD_RGR_BRIDGE_INTR
             * bit and read the register again.
             */
            bSuccess = false;

            /* Clear the RBUS-GISB-RBUS Bridge interrupt.  */
            BREG_Write32_isr(
               hXvd->hReg, 
               BCHP_SVD_INTR2_0_CPU_CLEAR, 
               BCHP_SVD_INTR2_0_CPU_CLEAR_AVD_RGR_BRIDGE_INTR_MASK
               );

            BXVD_DBG_MSG(hXvd, ("BXVD_Reg_Read32_isr read failed: offset: %08x uiStatus: %08x", offset, uiStatus ));
         }
         else
         {
            bSuccess = true;
         }

         if ( true == bSuccess )
         {
            break;
         }

      }    /* end of for ( uiLoop < BXVD_REG_READ_MAX_RETRIES ) */
 
      if ( uiLoop == BXVD_REG_READ_MAX_RETRIES )
      {
         BXVD_DBG_MSG(hXvd, ("BXVD_Reg_Read32_isr: didn't get a clean read of %08x", offset ));
      }
   }
   else
   {
      uiValue = BREG_Read32_isr(hXvd->hReg, offset);
   }
#else

   uiValue = BREG_Read32_isr(hXvd->hReg, offset);

#endif

   return uiValue;
}

#define MAX_LOOP_CNT 10

void BXVD_Reg_Write32_isr(BXVD_Handle hXvd, uint32_t offset, uint32_t data)
{

#if BXVD_P_SVD_GISB_ERR_WORKAROUND

   bool bDone= false;
   uint32_t loopCnt=0;
#if 0
   volatile uint32_t uiVal = data;
   uint32_t delayLoopCnt = 20000;
#endif

   while (!bDone)
   {
      BREG_Write32_isr(hXvd->hReg, offset, data);

      if (hXvd->bSVCCapable)
      {
         uint32_t uiStatus;
      
#if 1
         BKNI_Delay( 1 );  /* not a long one */
#else
         while(delayLoopCnt--)
         {
            uiVal = data;
         }
#endif

         /* Check if the AVD_RGR_BRIDGE_INTR bit is set in the CPU status register. */
         uiStatus = BREG_Read32_isr(hXvd->hReg, BCHP_SVD_INTR2_0_CPU_STATUS);

         if ( uiStatus & BCHP_SVD_INTR2_0_CPU_STATUS_AVD_RGR_BRIDGE_INTR_MASK )
         {
            /* If the write "fails", clear the interrupt bit and then wait
             * for the write to complete.
             */
            BREG_Write32_isr(
               hXvd->hReg, 
               BCHP_SVD_INTR2_0_CPU_CLEAR, 
               BCHP_SVD_INTR2_0_CPU_CLEAR_AVD_RGR_BRIDGE_INTR_MASK
               );

            BXVD_DBG_MSG(hXvd, ("BXVD_Reg_Write32_isr write failed: LoopCnt:%d  offset: %08x data: %08x uiStatus: %08x",
                                loopCnt, offset, data, uiStatus ));

            BKNI_Delay( BXVD_REG_WRITE_POST_FAILURE_DELAY );
#if 0
            uiVal = BREG_Read32_isr(hXvd->hReg, offset);

            BKNI_Printf("\t**** BXVD_Write_ISR bus error:cnt: %d addr:%08x  data:%08x val:%08x ****\n", 
                        loopCnt, offset, data, uiVal); 

            if ((data == uiVal) || (loopCnt==MAX_LOOP_CNT))
            {
               bDone=true;
            }
#else
            if (loopCnt==MAX_LOOP_CNT)
            {
               bDone=true;
            }
#endif
         }
         else
         {
            bDone=true;
         }
      }
      else /* Not SVD Capable */
      {
         bDone=true;
      }
      
      loopCnt++;
   }
#else

   BREG_Write32_isr(hXvd->hReg, offset, data);

#endif
}

uint32_t BXVD_Reg_Read32(BXVD_Handle hXvd, uint32_t offset)
{
   uint32_t uiValue;

#if BXVD_P_SVD_GISB_ERR_WORKAROUND

   if (hXvd->bSVCCapable)
   {
      uint32_t uiLoop;
      bool bSuccess=true;

      for ( uiLoop=0; uiLoop < BXVD_REG_READ_MAX_RETRIES; uiLoop++ )
      {

         /* SW7425-628: work around for the bus error caused by register reads. */
         uint32_t uiStatus;

         uiValue = BREG_Read32(hXvd->hReg, offset);

         /* Check if the AVD_RGR_BRIDGE_INTR bit is set in the CPU status register. */
         uiStatus = BREG_Read32(hXvd->hReg, BCHP_SVD_INTR2_0_CPU_STATUS);

         if ( uiStatus & BCHP_SVD_INTR2_0_CPU_STATUS_AVD_RGR_BRIDGE_INTR_MASK )
         {
            /* If the bit is set, the read failed.  Clear the AVD_RGR_BRIDGE_INTR
             * bit and read the register again.
             */
            bSuccess = false;

            /* Clear the RBUS-GISB-RBUS Bridge interrupt.  */
            BREG_Write32(
               hXvd->hReg, 
               BCHP_SVD_INTR2_0_CPU_CLEAR, 
               BCHP_SVD_INTR2_0_CPU_CLEAR_AVD_RGR_BRIDGE_INTR_MASK
               );

            BXVD_DBG_MSG(hXvd, ("BXVD_Reg_Read32 read failed: offset: %08x uiStatus: %08x", offset, uiStatus ));
         }
         else
         {
            bSuccess = true;
         }

         if ( true == bSuccess )
         {
            break;
         }

      }     /* end of for ( uiLoop < BXVD_REG_READ_MAX_RETRIES ) */

      if ( uiLoop == BXVD_REG_READ_MAX_RETRIES )
      {
         BXVD_DBG_MSG(hXvd, ("BXVD_Reg_Read32: didn't get a clean read of %08x", offset ));
      }
   }
   else
   {
      uiValue = BREG_Read32(hXvd->hReg, offset);
   }
#else

   uiValue = BREG_Read32(hXvd->hReg, offset);

#endif

   return uiValue;
}

void BXVD_Reg_Write32(BXVD_Handle hXvd, uint32_t offset, uint32_t data)
{
   BREG_Write32(hXvd->hReg, offset, data);

#if BXVD_P_SVD_GISB_ERR_WORKAROUND

   if (hXvd->bSVCCapable)
   {
      uint32_t uiStatus;

      /* Check if the AVD_RGR_BRIDGE_INTR bit is set in the CPU status register. */
      uiStatus = BREG_Read32(hXvd->hReg, BCHP_SVD_INTR2_0_CPU_STATUS);

      if ( uiStatus & BCHP_SVD_INTR2_0_CPU_STATUS_AVD_RGR_BRIDGE_INTR_MASK )
      {
         /* If the write "fails", clear the interrupt bit and then wait
          * for the write to complete.
          */
         BREG_Write32(
            hXvd->hReg, 
            BCHP_SVD_INTR2_0_CPU_CLEAR, 
            BCHP_SVD_INTR2_0_CPU_CLEAR_AVD_RGR_BRIDGE_INTR_MASK
            );

         BXVD_DBG_MSG(hXvd, ("BXVD_Reg_Write32 write failed: offset: %08x uiStatus: %08x", offset, uiStatus ));

         BKNI_Delay( BXVD_REG_WRITE_POST_FAILURE_DELAY );
      }
   }

#endif   
}

