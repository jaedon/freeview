/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvsb_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 9/12/08 4:50p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vsb/3510/bvsb_priv.c $
 * 
 * Hydra_Software_Devel/15   9/12/08 4:50p dliu
 * PR40148: Fix case where an interrupt is triggered during
 * WaitForApiEvent()
 * Also reworked some debug message
 * 
 * Hydra_Software_Devel/14   2/13/04 11:15a enavarro
 * PR 9466: reworked BVSB_P_WaitForApiEvent()
 * 
 * Hydra_Software_Devel/13   2/9/04 10:41a enavarro
 * PR 9675: fix compiler warnings
 * 
 * Hydra_Software_Devel/12   2/7/04 11:28a dlwin
 * PR 9663: Replaced C++ style of comments with C style.
 * 
 * Hydra_Software_Devel/11   2/2/04 2:06p enavarro
 * PR 9466: removed BVSB_P_AccessRegister(); reset HABSTAT and APSTAT2
 * registers when resetting AP; added code to decode interrupts in API
 * context; added BVSB_P_WaitForApiEvent() and
 * BVSB_P_EnableHostInterrupt()
 * 
 * Hydra_Software_Devel/10   1/29/04 9:39a enavarro
 * PR 9466: interrupt on HABR transitioning from 1 to 0
 * 
 * Hydra_Software_Devel/9   11/4/03 4:47p enavarro
 * added BVSB_P_AccessRegister()
 * 
 * Hydra_Software_Devel/8   11/4/03 1:52p enavarro
 * wait for HABR bit to clear in BVSB_P_SendHabCommand()
 * 
 * Hydra_Software_Devel/7   10/30/03 11:34a enavarro
 * initial checkout with board
 * 
 * Hydra_Software_Devel/6   10/20/03 9:33a enavarro
 * took out BDBG_MODULE because it was causing compiler warning
 * 
 * Hydra_Software_Devel/5   10/17/03 3:44p enavarro
 * removed BDBG_ENTER(__FUNCTION__) and BDBG_LEAVE(__FUNCTION__)
 * 
 * Hydra_Software_Devel/4   9/30/03 6:32p enavarro
 * added BDBG_MODULE macro
 * 
 * Hydra_Software_Devel/3   9/18/03 7:02p enavarro
 * fixed compiler warnings
 * 
 * Hydra_Software_Devel/2   9/5/03 10:10a enavarro
 * got it to compile
 * 
 * Hydra_Software_Devel/1   8/28/03 4:36p enavarro
 * initial version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bchp_3510.h"
#include "bvsb.h"
#include "bvsb_priv.h"

BDBG_MODULE(bvsb_priv); 


#if 0
/******************************************************************************
 BVSB_P_AccessRegister() 
******************************************************************************/
BERR_Code BVSB_P_AccessRegister(BVSB_Handle hVSB, BVSB_AccessType accessType,
                                uint8_t offset, uint8_t *val)
{
   BERR_Code retCode = BERR_SUCCESS;
   
   if (accessType == BVSB_AccessType_eWrite)
      retCode = BREG_I2C_Write(hVSB->hRegister, hVSB->settings.chipAddr, offset, val, 1);
   else
      retCode = BREG_I2C_Read(hVSB->hRegister, hVSB->settings.chipAddr, offset, val, 1);
   
   return retCode;
}
#endif
   

/******************************************************************************
 BVSB_P_ResetAp() 
******************************************************************************/
BERR_Code BVSB_P_ResetAp(BVSB_Handle hVSB)
{
   BERR_Code     retCode = BERR_SUCCESS;
   uint8_t       hctl1, apstat1, sb;

   BDBG_ASSERT(hVSB);

   /* reset the AP */
   hctl1 = BCM3510_HCTL1_RESET;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_HCTL1, &hctl1));

   /* verify that the AP is reset */
   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APSTAT1, &apstat1));
   if ((apstat1 & BCM3510_APSTAT1_RESET) == 0)
   {
      BERR_TRACE(retCode = BVSB_ERR_AP_NOT_STOPPED);
   }

   sb = BCM3510_HABSTAT_LDMSK | BCM3510_HABSTAT_MCMSK | BCM3510_HABSTAT_LDHABR;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_HABSTAT, &sb));
   sb = 0;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_APSTAT2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_IdleAp() 
******************************************************************************/
BERR_Code BVSB_P_IdleAp(BVSB_Handle hVSB)
{
   BERR_Code     retCode = BERR_SUCCESS;
   uint8_t       hctl1, apstat1;

   BDBG_ASSERT(hVSB);

   /* reset the AP */
   hctl1 = BCM3510_HCTL1_IDLE;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_HCTL1, &hctl1));

   /* verify that the AP is idle */
   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APSTAT1, &apstat1));
   if ((apstat1 & BCM3510_APSTAT1_IDLE) == 0)
   {
      BERR_TRACE(retCode = BVSB_ERR_AP_NOT_STOPPED);
   }

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_RunAp() 
******************************************************************************/
BERR_Code BVSB_P_RunAp(BVSB_Handle hVSB)
{
   BERR_Code     retCode = BERR_SUCCESS;
   uint8_t       hctl1, apstat1;

   BDBG_ASSERT(hVSB);

   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APSTAT1, &apstat1));
   if (apstat1 & BCM3510_APSTAT1_RUN)
      goto done;
   else if (apstat1 & BCM3510_APSTAT1_IDLE)
      hctl1 = BCM3510_HCTL1_CLEAR_IDLE;
   else
      hctl1 = BCM3510_HCTL1_CLEAR_RESET;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_HCTL1, &hctl1));

   /* verify that the AP is running */
   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APSTAT1, &apstat1));
   if ((apstat1 & BCM3510_APSTAT1_RUN) == 0)
   {
      BERR_TRACE(retCode = BVSB_ERR_AP_NOT_RUNNING);
   }

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_SendHabCommand()
******************************************************************************/
BERR_Code BVSB_P_SendHabCommand(BVSB_Handle hVSB, uint8_t write_len, uint8_t *write_buf, uint8_t read_len, uint8_t *read_buf)
{
   BERR_Code     retCode = BERR_SUCCESS;
   BVSB_ApStatus apStatus;
   uint8_t       habadr, i, habstat, byte0, apmsk2, sb;
  
   BDBG_ASSERT(hVSB);
   BDBG_ASSERT(write_buf);

   if ((write_len > 128) || (read_len > 128) || (write_len < 3))
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      goto done;
   }

   /* ok to send an hab command? */
   BVSB_CHK_RETCODE(BVSB_GetApStatus(hVSB, &apStatus));
   if ((apStatus & BVSB_APSTATUS_SEND_MASK) != BVSB_APSTATUS_OK_TO_SEND)
   {
      BERR_TRACE(retCode = BVSB_ERR_HAB_NOT_AVAIL);
      BDBG_ERR(("HAB not available: apStatus = 0x%08X\n", apStatus));
      goto done;
   }

   /* set the HAB address to 0x00 */
   habadr = 0;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_HABADR, &habadr)); 

   byte0 = write_buf[0];
   BDBG_MSG(("write HAB %x, %x, %x\n", write_buf[0], write_buf[1], write_buf[2]));
   for (i = 0; i < write_len; i++)
   {
      BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_HABDATA, &write_buf[i]));
   }

   /* disable other bcm3510 interrupts while habr=1 */
   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APMSK2, &apmsk2));
   if (apmsk2)
   {
      sb = 0; 
      if ((retCode = BVSB_WriteRegister(hVSB, BCM3510_APMSK2, &sb)) != BERR_SUCCESS)
         goto restore_apmsk2;
   }
   
   /* clear any pending HAB events */
   BKNI_WaitForEvent(hVSB->hHabEvent, 0);
   /* BKNI_WaitForEvent(hVSB->hApiEvent, 0); */
   
   /* interrupt on HABR = 0 */
   /* set HABR and enable the HABR interrupt */
   BDBG_MSG(("BVSB_P_SendHabCommand(): setting HABR and HMSK\n"));      
   habstat = BCM3510_HABSTAT_HABR | BCM3510_HABSTAT_LDHABR | BCM3510_HABSTAT_LDMSK | BCM3510_HABSTAT_MCMSK | BCM3510_HABSTAT_HMSK;
   if ((retCode = BVSB_WriteRegister(hVSB, BCM3510_HABSTAT, &habstat)) != BERR_SUCCESS)
      goto restore_apmsk2;
   
   /* wait for AP to release the HAB */
   BDBG_MSG(("BVSB_P_SendHabCommand(): wait for habr interrrupt\n"));   
   /*retCode = BKNI_WaitForEvent(hVSB->hHabEvent, 1000);*/
   retCode = BVSB_P_WaitForApiEvent(hVSB, hVSB->hHabEvent, 1000);
    
   /* disable HABR interrupt */
   BDBG_MSG(("BVSB_P_SendHabCommand(): disabling habr interrrupt\n"));   
   habstat = BCM3510_HABSTAT_LDMSK | BCM3510_HABSTAT_MCMSK;
   BVSB_WriteRegister(hVSB, BCM3510_HABSTAT, &habstat);
   
   if (retCode != BERR_SUCCESS)
      goto restore_apmsk2;   

   /* set the HAB address to 0x00 and read the contents */
   habadr = 0;
   if ((retCode = BVSB_WriteRegister(hVSB, BCM3510_HABADR, &habadr)) != BERR_SUCCESS)
      goto restore_apmsk2;

   for (i = 0; i < read_len; i++)
   {
      if ((retCode = BVSB_ReadRegister(hVSB, BCM3510_HABDATA, &read_buf[i])) != BERR_SUCCESS)
         goto restore_apmsk2;
   }

   if (read_buf[0] != (byte0 | 0x80))
   {
      BERR_TRACE(retCode = BVSB_ERR_CMD_NOT_SERVICED);
   }

   /* put back other interrupts */
   restore_apmsk2:
   if (apmsk2)
   {
      BERR_TRACE(BVSB_WriteRegister(hVSB, BCM3510_APMSK2, &apmsk2));
   }

   done:
   BDBG_MSG(("HAB SendCommand Done\n"));
   return retCode;
}


/******************************************************************************
 BVSB_P_WaitForApiEvent()
******************************************************************************/
BERR_Code BVSB_P_WaitForApiEvent(BVSB_Handle hVSB, BKNI_EventHandle hEvent, int timeoutMsec)
{
   BERR_Code retCode = BERR_SUCCESS;
   
   BVSB_P_EnableHostInterrupt(hVSB, true);
   
   while ((retCode = BKNI_WaitForEvent(hVSB->hApiEvent, timeoutMsec)) != BERR_TIMEOUT)
   {
      BVSB_CHK_RETCODE(BVSB_P_DecodeInterrupt(hVSB));
      BVSB_P_EnableHostInterrupt(hVSB, true);
      if (BKNI_WaitForEvent(hEvent, 0) == BERR_SUCCESS)
         break;
   }
   
   done:
   BVSB_P_EnableHostInterrupt(hVSB, true);
   return retCode;
}


/******************************************************************************
 BVSB_P_DecodeInterrupt()
******************************************************************************/
BERR_Code BVSB_P_DecodeInterrupt(BVSB_Handle hVSB)
{
   BERR_Code     retCode = BERR_SUCCESS;
   uint8_t       andmask, habstat, apstat2, apmsk2;

   BDBG_ASSERT(hVSB);
   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_HABSTAT, &habstat));
   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APSTAT2, &apstat2));
   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APMSK2, &apmsk2));
   BDBG_MSG(("BVSB_P_DecodeInterrupt Enter %x, %x, %x", habstat, apstat2, apmsk2));
   andmask = apstat2 & apmsk2;
   
   if ((habstat & BCM3510_HABSTAT_HMSK) && 
       ((habstat & BCM3510_HABSTAT_HABR) == 0))
   {
      BVSB_CHK_RETCODE(BVSB_P_DisableHabInterrupt(hVSB)); /* disable habr irq */
      BKNI_SetEvent(hVSB->hHabEvent);
   }

   if (andmask)
   {
      apmsk2 &= ~BCM3510_APSTAT2_INIT_DONE;
      if (andmask & BCM3510_APSTAT2_NOTLOCK)
      {
         apmsk2 &= ~BCM3510_APSTAT2_NOTLOCK;      
         apmsk2 |= BCM3510_APSTAT2_LOCK;
         if (hVSB->bLocked)
            hVSB->bLocked = false;         
      }
      else if (andmask & BCM3510_APSTAT2_LOCK)
      {
         apmsk2 &= ~BCM3510_APSTAT2_LOCK;      
         apmsk2 |= BCM3510_APSTAT2_NOTLOCK;
         if (hVSB->bLocked == false)
            hVSB->bLocked = true;         
      }
            
      BVSB_WriteRegister(hVSB, BCM3510_APMSK2, &apmsk2);
      
      if (andmask & (BCM3510_APSTAT2_NOTLOCK | BCM3510_APSTAT2_LOCK))
         BKNI_SetEvent(hVSB->hLockStateChangeEvent);
      if (andmask & BCM3510_APSTAT2_INIT_DONE)
         BKNI_SetEvent(hVSB->hInitEvent);
   }

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_DisableAllInterrupts()
******************************************************************************/
BERR_Code BVSB_P_DisableAllInterrupts(BVSB_Handle hVSB)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   mask;

   BDBG_ASSERT(hVSB);

   mask = 0;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_APMSK1, &mask));
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_APMSK2, &mask));

   mask = BCM3510_HABSTAT_LDMSK | BCM3510_HABSTAT_MCMSK;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_HABSTAT, &mask));

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_EnableInitDoneInterrupt() - enable the init_done interrupt
******************************************************************************/
BERR_Code BVSB_P_EnableInitDoneInterrupt(BVSB_Handle hVSB)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hVSB);

   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APMSK2, &sb));
   sb |= BCM3510_APSTAT2_INIT_DONE;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_DisableInitDoneInterrupt() - disable the init_done interrupt
******************************************************************************/
BERR_Code BVSB_P_DisableInitDoneInterrupt(BVSB_Handle hVSB)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hVSB);

   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APMSK2, &sb));
   sb &= ~BCM3510_APSTAT2_INIT_DONE;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_EnableHabInterrupt() - enable the HAB interrupt
******************************************************************************/
BERR_Code BVSB_P_EnableHabInterrupt(BVSB_Handle hVSB)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hVSB);

   sb = BCM3510_HABSTAT_LDMSK | BCM3510_HABSTAT_MCMSK | BCM3510_HABSTAT_HMSK;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_HABSTAT, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_DisableHabInterrupt() - disable the HAB interrupt
******************************************************************************/
BERR_Code BVSB_P_DisableHabInterrupt(BVSB_Handle hVSB)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hVSB);

   sb = BCM3510_HABSTAT_LDMSK | BCM3510_HABSTAT_MCMSK;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_HABSTAT, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_EnableLockInterrupt() - enable the lock interrupt
******************************************************************************/
BERR_Code BVSB_P_EnableLockInterrupt(BVSB_Handle hVSB)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hVSB);

   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APMSK2, &sb));
   sb |= BCM3510_APSTAT2_LOCK;
   sb &= ~BCM3510_APSTAT2_NOTLOCK;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_DisableLockInterrupt() - disable the lock interrupt
******************************************************************************/
BERR_Code BVSB_P_DisableLockInterrupt(BVSB_Handle hVSB)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hVSB);

   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APMSK2, &sb));
   sb &= ~BCM3510_APSTAT2_LOCK;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_EnableLossLockInterrupt() - enable the lost lock interrupt
******************************************************************************/
BERR_Code BVSB_P_EnableLossLockInterrupt(BVSB_Handle hVSB)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hVSB);

   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APMSK2, &sb));
   sb |= BCM3510_APSTAT2_NOTLOCK;
   sb &= ~BCM3510_APSTAT2_LOCK;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_DisableLossLockInterrupt() - disable the lost lock interrupt
******************************************************************************/
BERR_Code BVSB_P_DisableLossLockInterrupt(BVSB_Handle hVSB)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t   sb;

   BDBG_ASSERT(hVSB);

   BVSB_CHK_RETCODE(BVSB_ReadRegister(hVSB, BCM3510_APMSK2, &sb));
   sb &= ~BCM3510_APSTAT2_NOTLOCK;
   BVSB_CHK_RETCODE(BVSB_WriteRegister(hVSB, BCM3510_APMSK2, &sb));

   done:
   return retCode;
}


/******************************************************************************
 BVSB_P_EnableHostInterrupt() - enables/disable the host interrupt through
                                a callback function in settings struct
******************************************************************************/
BERR_Code BVSB_P_EnableHostInterrupt(BVSB_Handle hVSB, bool bEnable)
{
   BKNI_EnterCriticalSection();
   hVSB->settings.interruptEnableFunc(bEnable, hVSB->settings.interruptEnableFuncParam);
   BKNI_LeaveCriticalSection();   

   return BERR_SUCCESS;
}



