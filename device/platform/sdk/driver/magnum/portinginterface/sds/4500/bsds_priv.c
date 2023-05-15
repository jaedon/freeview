/***************************************************************************
 *     Copyright (c) 2003-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsds_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 6/27/06 10:41a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/sds/4500/bsds_priv.c $
 * 
 * Hydra_Software_Devel/21   6/27/06 10:41a jkim
 * PR14344: Handle unexpected interrupt
 * 
 * Hydra_Software_Devel/20   2/13/04 11:14a enavarro
 * PR 9466: reworked BSDS_P_WaitForApiEvent()
 * 
 * Hydra_Software_Devel/19   2/9/04 10:40a enavarro
 * PR 9675: fix compiler warnings
 * 
 * Hydra_Software_Devel/18   2/7/04 11:29a dlwin
 * PR 9663: Replaced C++ style of comments with C style.
 * 
 * Hydra_Software_Devel/17   2/2/04 2:04p enavarro
 * PR 9466: removed BSDS_P_AccessRegister(); reset HABSTAT and APSTAT2
 * registers when resetting the AP; added code to decode interrupts at
 * API context; added BSDS_P_WaitForApiEvent(), BSDS_P_DecodeInterrupt(),
 * and BSDS_P_EnableHostInterrupt()
 * 
 * Hydra_Software_Devel/16   1/29/04 9:13a enavarro
 * PR 9466: interrupt on HABR transitioning from 1 to 0
 * 
 * Hydra_Software_Devel/15   11/11/03 3:35p enavarro
 * print out apStatus (via BDBG_MSG) if 4500 not ready to accept HAB
 * commands in BSDS_P_SendHabCommand()
 * 
 * Hydra_Software_Devel/14   11/4/03 4:06p enavarro
 * added BSDS_P_AccessRegister()
 * 
 * Hydra_Software_Devel/13   10/31/03 4:02p enavarro
 * fixed compiler error in linux
 * 
 * Hydra_Software_Devel/11   10/31/03 3:56p enavarro
 * check for HABSTAT.0=0 before reading HAB in BSDS_P_SendHabCommand()
 * 
 * Hydra_Software_Devel/10   10/31/03 3:48p enavarro
 * poll for HABSTAT.0 = 0 in BSDS_P_SendHabCommand()
 * 
 * Hydra_Software_Devel/9   10/30/03 11:34a enavarro
 * initial checkout with board
 * 
 * Hydra_Software_Devel/8   10/20/03 9:33a enavarro
 * took out BDBG_MODULE because it was causing compiler warning
 * 
 * Hydra_Software_Devel/7   10/17/03 3:44p enavarro
 * removed BDBG_ENTER(__FUNCTION__) and BDBG_LEAVE(__FUNCTION__)
 * 
 * Hydra_Software_Devel/6   9/30/03 6:32p enavarro
 * added BDBG_MODULE macro
 * 
 * Hydra_Software_Devel/5   9/18/03 7:02p enavarro
 * fixed compiler warnings
 * 
 * Hydra_Software_Devel/4   9/11/03 4:26p enavarro
 * fixed compiler errors/warnings in linux
 * 
 * Hydra_Software_Devel/3   9/9/03 3:28p enavarro
 * added support for 8psk rate 3/4 (2.20 bps)
 * 
 * Hydra_Software_Devel/2   9/5/03 10:03a enavarro
 * rename Enable/Disable Interrupt functions
 * 
 * Hydra_Software_Devel/1   8/28/03 10:43a enavarro
 * initial version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bchp_4500.h"
#include "bsds.h"
#include "bsds_priv.h"

BDBG_MODULE(bsds_priv);


const uint8_t BSDS_CodeRateData_turbo_qpsk_7_8[80] =  /* Turbo QPSK rate 7/8 */
{
   0xD1, 0x00, 0x0A, 0x03, 0x2B, 0x70, 0x9F, 0xCC,
   0x00, 0x00, 0x4F, 0xCC, 0x07, 0x9E, 0x13, 0xA6,
   0x03, 0xB5, 0x8F, 0x34, 0x76, 0x30, 0x30, 0x00,
   0x00, 0x1F, 0x00, 0x1F, 0x04, 0x00, 0x00, 0x00,
   0xEE, 0xEE, 0xEE, 0xDD, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x0A, 0x03, 0x18, 0x00, 0x55, 0x04,
   0x08, 0x32, 0x08, 0x7F, 0x00, 0x28, 0x8F, 0x00,
   0x00, 0x1E, 0x8B, 0x00, 0x01, 0xBE, 0xBB, 0x47,
   0x00, 0x00, 0x00, 0x00, 0x11, 0x04, 0x3B, 0x95
};


const uint8_t BSDS_CodeRateData_turbo_8psk_3_4_2_20[80] = /* Turbo 8PSK rate 3/4 (2.20) */
{
   0xB5, 0x00, 0x0A, 0x03, 0x2B, 0x70, 0x9F, 0xCC,
   0x00, 0x00, 0x4F, 0xCC, 0x07, 0x9E, 0x13, 0xA6,
   0x03, 0xB5, 0x8F, 0x34, 0x76, 0x30, 0x60, 0x00,
   0x00, 0x1F, 0x00, 0x1F, 0x04, 0x00, 0x00, 0x00,
   0x21, 0x01, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x0A, 0x03, 0x16, 0x0a, 0x15, 0x04,
   0x0F, 0x20, 0x0A, 0x3F, 0x00, 0x28, 0x4F, 0x00,
   0x00, 0x1E, 0x4B, 0x00, 0x02, 0x4C, 0xBB, 0x47,
   0x84, 0x00, 0x00, 0x00, 0x00, 0x06, 0x2D, 0xC4
};


#if 0
/******************************************************************************
 BSDS_P_AccessRegister() 
******************************************************************************/
BERR_Code BSDS_P_AccessRegister(BSDS_Handle hSDS, BSDS_AccessType accessType,
                                uint8_t offset, uint8_t *val)
{
   BERR_Code retCode = BERR_SUCCESS;
   
   if (accessType == BSDS_AccessType_eWrite)
      retCode = BREG_I2C_Write(hSDS->hRegister, hSDS->settings.chipAddr, offset, val, 1);
   else
      retCode = BREG_I2C_Read(hSDS->hRegister, hSDS->settings.chipAddr, offset, val, 1);
   
   return retCode;
}
#endif
   

/******************************************************************************
 BSDS_P_ResetAp() 
******************************************************************************/
BERR_Code BSDS_P_ResetAp(BSDS_Handle hSDS)
{
   BERR_Code     retCode = BERR_SUCCESS;
   uint8_t       hctl1, apstat1, sb;

   BDBG_ASSERT(hSDS);

   /* reset the AP */
   hctl1 = BCM4500_HCTL1_RESET;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_HCTL1, &hctl1));

   /* verify that the AP is reset */
   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APSTAT1, &apstat1));
   if ((apstat1 & BCM4500_APSTAT1_RESET) == 0)
   {
      BERR_TRACE(retCode = BSDS_ERR_AP_NOT_STOPPED);
   }
   
   sb = BCM4500_HABSTAT_LDMSK | BCM4500_HABSTAT_MCMSK | BCM4500_HABSTAT_LDHABR;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_HABSTAT, &sb));
   sb = 0;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APSTAT2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_IdleAp() 
******************************************************************************/
BERR_Code BSDS_P_IdleAp(BSDS_Handle hSDS)
{
   BERR_Code     retCode = BERR_SUCCESS;
   uint8_t       hctl1, apstat1;

   BDBG_ASSERT(hSDS);

   /* reset the AP */
   hctl1 = BCM4500_HCTL1_IDLE;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_HCTL1, &hctl1));

   /* verify that the AP is idle */
   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APSTAT1, &apstat1));
   if ((apstat1 & BCM4500_APSTAT1_IDLE) == 0)
   {
      BERR_TRACE(retCode = BSDS_ERR_AP_NOT_STOPPED);
   }

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_RunAp() 
******************************************************************************/
BERR_Code BSDS_P_RunAp(BSDS_Handle hSDS)
{
   BERR_Code     retCode = BERR_SUCCESS;
   uint8_t       hctl1, apstat1;

   BDBG_ASSERT(hSDS);

   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APSTAT1, &apstat1));
   if (apstat1 & BCM4500_APSTAT1_RUN)
      goto done;
   else if (apstat1 & BCM4500_APSTAT1_IDLE)
      hctl1 = BCM4500_HCTL1_CLEAR_IDLE;
   else
      hctl1 = BCM4500_HCTL1_CLEAR_RESET;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_HCTL1, &hctl1));

   /* verify that the AP is running */
   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APSTAT1, &apstat1));
   if ((apstat1 & BCM4500_APSTAT1_RUN) == 0)
   {
      BERR_TRACE(retCode = BSDS_ERR_AP_NOT_RUNNING);
   }

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_SendHabCommand()
******************************************************************************/
BERR_Code BSDS_P_SendHabCommand(BSDS_Handle hSDS, uint8_t write_len, uint8_t *write_buf, uint8_t read_len, uint8_t *read_buf)
{
   BERR_Code     retCode = BERR_SUCCESS;
   BSDS_ApStatus apStatus;
   uint8_t       habadr, i, habstat, byte0, apmsk2, sb;
  
   BDBG_ASSERT(hSDS);
   BDBG_ASSERT(write_buf);

   if ((write_len > 128) || (read_len > 128) || (write_len < 3))
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      goto done;
   }

   /* ok to send an hab command? */
   BSDS_CHK_RETCODE(BSDS_GetApStatus(hSDS, &apStatus));
   if ((apStatus & BSDS_APSTATUS_SEND_MASK) != BSDS_APSTATUS_OK_TO_SEND)
   {
      BERR_TRACE(retCode = BSDS_ERR_HAB_NOT_AVAIL);
      BDBG_ERR(("HAB not available: apStatus = 0x%08X\n", apStatus));
      goto done;
   }

   /* set the HAB address to 0x00 */
   habadr = 0;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_HABADR, &habadr)); 

   byte0 = write_buf[0];
   for (i = 0; i < write_len; i++)
   {
      BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_HABDATA, &write_buf[i]));
      BDBG_MSG(("write HAB[%d] = %02X\n", i, write_buf[i]));
   }

   /* disable other bcm4500 interrupts while habr=1 */
   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APMSK2, &apmsk2));
   if (apmsk2)
   {
      sb = 0;
      if ((retCode = BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &sb)) != BERR_SUCCESS)
         goto restore_apmsk2;
   }
   
   /* clear any pending HAB events */
   BKNI_WaitForEvent(hSDS->hHabEvent, 0);
   /* BKNI_WaitForEvent(hSDS->hApiEvent, 0); */
      
   /* interrupt on HABR = 0 */
   /* set HABR and enable the HABR interrupt */
   BDBG_MSG(("BSDS_P_SendHabCommand(): setting HABR and HMSK\n"));      
   habstat = BCM4500_HABSTAT_HABR | BCM4500_HABSTAT_LDHABR | BCM4500_HABSTAT_LDMSK | BCM4500_HABSTAT_MCMSK | BCM4500_HABSTAT_HMSK;
   if ((retCode = BSDS_WriteRegister(hSDS, BCM4500_HABSTAT, &habstat)) != BERR_SUCCESS)
      goto restore_apmsk2;
   
   /* wait for AP to release the HAB */
   BDBG_MSG(("BSDS_P_SendHabCommand(): wait for habr interrrupt\n"));   
   /* retCode = BKNI_WaitForEvent(hSDS->hHabEvent, 1000); */
   retCode = BSDS_P_WaitForApiEvent(hSDS, hSDS->hHabEvent, 1000);
   
   /* disable HABR interrupt */
   BDBG_MSG(("BSDS_P_SendHabCommand(): disabling habr interrrupt\n"));   
   habstat = BCM4500_HABSTAT_LDMSK | BCM4500_HABSTAT_MCMSK;
   BSDS_WriteRegister(hSDS, BCM4500_HABSTAT, &habstat);
   
   if (retCode != BERR_SUCCESS)
      goto restore_apmsk2;   

   /* set the HAB address to 0x00 and read the contents */
   habadr = 0;
   if ((retCode = BSDS_WriteRegister(hSDS, BCM4500_HABADR, &habadr)) != BERR_SUCCESS)
      goto restore_apmsk2;

   for (i = 0; i < read_len; i++)
   {
      if ((retCode = BSDS_ReadRegister(hSDS, BCM4500_HABDATA, &read_buf[i])) != BERR_SUCCESS)
         goto restore_apmsk2;
      BDBG_MSG(("read HAB[%d] = %02X\n", i, read_buf[i]));         
   }

   if (read_buf[0] != (byte0 | 0x80))
   {
      BERR_TRACE(retCode = BSDS_ERR_CMD_NOT_SERVICED);
   }

   /* put back other interrupts */
   restore_apmsk2:
   if (apmsk2)
   {
      BERR_TRACE(BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &apmsk2));
   }

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_WaitForApiEvent()
******************************************************************************/
BERR_Code BSDS_P_WaitForApiEvent(BSDS_Handle hSDS, BKNI_EventHandle hEvent, int timeoutMsec)
{
   BERR_Code retCode = BERR_SUCCESS;
   
   BSDS_P_EnableHostInterrupt(hSDS, true);
   
   while ((retCode = BKNI_WaitForEvent(hSDS->hApiEvent, timeoutMsec)) != BERR_TIMEOUT)
   {
      BSDS_CHK_RETCODE(BSDS_P_DecodeInterrupt(hSDS));
      if (BKNI_WaitForEvent(hEvent, 0) == BERR_SUCCESS)
         break;
      BSDS_P_EnableHostInterrupt(hSDS, true);
   }    

   done:
   BSDS_P_EnableHostInterrupt(hSDS, true);
   return retCode;
}


/******************************************************************************
 BSDS_P_DecodeInterrupt()
******************************************************************************/
BERR_Code BSDS_P_DecodeInterrupt(BSDS_Handle hSDS)
{
   BERR_Code     retCode = BERR_SUCCESS;
   uint8_t       andmask, habstat, apstat2, apmsk2;

   BDBG_ASSERT(hSDS);
   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_HABSTAT, &habstat));
   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APSTAT2, &apstat2));
   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APMSK2, &apmsk2));
   andmask = apstat2 & apmsk2;
   
   if ((habstat & BCM4500_HABSTAT_HMSK) && 
       ((habstat & BCM4500_HABSTAT_HABR) == 0))
   {
      BSDS_CHK_RETCODE(BSDS_P_DisableHabInterrupt(hSDS)); /* disable habr irq */
      BKNI_SetEvent(hSDS->hHabEvent);
   }

   if (andmask)
   {
      apmsk2 &= ~(BCM4500_APSTAT2_DISEQC | BCM4500_APSTAT2_INIT_DONE);
      if (andmask & BCM4500_APSTAT2_FECNL)
      {
         apmsk2 &= ~BCM4500_APSTAT2_FECNL;      
         apmsk2 |= BCM4500_APSTAT2_FECL;
         if (hSDS->bLocked)
            hSDS->bLocked = false;         
      }
      else if (andmask & BCM4500_APSTAT2_FECL)
      {
         apmsk2 &= ~BCM4500_APSTAT2_FECL;      
         apmsk2 |= BCM4500_APSTAT2_FECNL;
         if (hSDS->bLocked == false)
            hSDS->bLocked = true;         
      }
            
      BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &apmsk2);

      if (andmask & (BCM4500_APSTAT2_FECNL | BCM4500_APSTAT2_FECL))
         BKNI_SetEvent(hSDS->hLockStateChangeEvent);
      if (andmask & BCM4500_APSTAT2_DISEQC)
         BKNI_SetEvent(hSDS->hDiseqcEvent);
      if (andmask & BCM4500_APSTAT2_INIT_DONE)
         BKNI_SetEvent(hSDS->hInitEvent);
   }

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_DisableAllInterrupts()
******************************************************************************/
BERR_Code BSDS_P_DisableAllInterrupts(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   mask;

   BDBG_ASSERT(hSDS);

   mask = 0;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APMSK1, &mask));
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &mask));

   mask = BCM4500_HABSTAT_LDMSK | BCM4500_HABSTAT_MCMSK;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_HABSTAT, &mask));

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_EnableInitDoneInterrupt() - enable the init_done interrupt
******************************************************************************/
BERR_Code BSDS_P_EnableInitDoneInterrupt(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hSDS);

   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APMSK2, &sb));
   sb |= BCM4500_APSTAT2_INIT_DONE;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_DisableInitDoneInterrupt() - disable the init_done interrupt
******************************************************************************/
BERR_Code BSDS_P_DisableInitDoneInterrupt(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hSDS);

   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APMSK2, &sb));
   sb &= ~BCM4500_APSTAT2_INIT_DONE;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_EnableHabInterrupt() - enable the HAB interrupt
******************************************************************************/
BERR_Code BSDS_P_EnableHabInterrupt(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hSDS);

   sb = BCM4500_HABSTAT_LDMSK | BCM4500_HABSTAT_MCMSK | BCM4500_HABSTAT_HMSK;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_HABSTAT, &sb));
   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_DisableHabInterrupt() - disable the HAB interrupt
******************************************************************************/
BERR_Code BSDS_P_DisableHabInterrupt(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hSDS);

   sb = BCM4500_HABSTAT_LDMSK | BCM4500_HABSTAT_MCMSK;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_HABSTAT, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_EnableLockInterrupt() - enable the lock interrupt
******************************************************************************/
BERR_Code BSDS_P_EnableLockInterrupt(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hSDS);

   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APMSK2, &sb));
   sb |= BCM4500_APSTAT2_FECL;
   sb &= ~BCM4500_APSTAT2_FECNL;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_DisableLockInterrupt() - disable the lock interrupt
******************************************************************************/
BERR_Code BSDS_P_DisableLockInterrupt(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hSDS);

   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APMSK2, &sb));
   sb &= ~BCM4500_APSTAT2_FECL;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_EnableLossLockInterrupt() - enable the lost lock interrupt
******************************************************************************/
BERR_Code BSDS_P_EnableLossLockInterrupt(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hSDS);

   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APMSK2, &sb));
   sb |= BCM4500_APSTAT2_FECNL;
   sb &= ~BCM4500_APSTAT2_FECL;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_DisableLossLockInterrupt() - disable the lost lock interrupt
******************************************************************************/
BERR_Code BSDS_P_DisableLossLockInterrupt(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hSDS);

   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APMSK2, &sb));
   sb &= ~BCM4500_APSTAT2_FECNL;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_EnableDiseqcInterrupt() - enable the diseqc interrupt
******************************************************************************/
BERR_Code BSDS_P_EnableDiseqcInterrupt(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hSDS);

   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APSTAT2, &sb));
   sb &= ~BCM4500_APSTAT2_DISEQC;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APSTAT2, &sb));
   
   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APMSK2, &sb));
   sb |= BCM4500_APSTAT2_DISEQC;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &sb));
   
   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_DisableDiseqcInterrupt() - disable the diseqc interrupt
******************************************************************************/
BERR_Code BSDS_P_DisableDiseqcInterrupt(BSDS_Handle hSDS)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hSDS);

   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APMSK2, &sb));
   sb &= ~BCM4500_APSTAT2_DISEQC;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APMSK2, &sb));
   
   BSDS_CHK_RETCODE(BSDS_ReadRegister(hSDS, BCM4500_APSTAT2, &sb));
   sb &= ~BCM4500_APSTAT2_DISEQC;
   BSDS_CHK_RETCODE(BSDS_WriteRegister(hSDS, BCM4500_APSTAT2, &sb));
   
   done:
   return retCode;
}


/******************************************************************************
 BSDS_P_EnableHostInterrupt() - enables/disable the host interrupt through
                                a callback function in settings struct
******************************************************************************/
BERR_Code BSDS_P_EnableHostInterrupt(BSDS_Handle hSDS, bool bEnable)
{
   BKNI_EnterCriticalSection();
   hSDS->settings.interruptEnableFunc(bEnable, hSDS->settings.interruptEnableFuncParam);
   BKNI_LeaveCriticalSection();   

   return BERR_SUCCESS;
}


/******************************************************************************
 BSDS_P_LoadTurboCodeRate9() - loads code rate data into slot index 9
******************************************************************************/
BERR_Code BSDS_P_LoadTurboCodeRate9(BSDS_Handle hSDS, BSDS_ModulationType m)
{
   BERR_Code retCode = BERR_SUCCESS;
   const uint8_t *pData;
   uint8_t i, hab[128];

   BDBG_ASSERT(hSDS);

   switch (m)
   {
      case BSDS_ModulationType_eTurboQpsk7_8:
         pData = BSDS_CodeRateData_turbo_qpsk_7_8;
         break;

      case BSDS_ModulationType_eTurbo8psk3_4_2_20:
         pData = BSDS_CodeRateData_turbo_8psk_3_4_2_20;
         break;

      default:
         BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
         goto done;
   }
      
   /* load turbo code rate index 9 */
   hab[0] = 0x53;
   hab[1] = 0x09;
   hab[2] = 0x3C;
   hab[3] = 0x5B;
   for (i = 0; i < 80; i++)
      hab[4+i] = pData[i];
   hab[84] = 0x00;            
   BSDS_CHK_RETCODE(BSDS_P_SendHabCommand(hSDS, 85, hab, 1, hab));
   
   hSDS->turboIdx9 = m;

   done:
   return retCode;
}
