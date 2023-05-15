/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: $
 * 
 ***************************************************************************/
 
#include "bdsq.h"
#include "bdsq_priv.h"
#include "bdsq_g1_priv.h"

BDBG_MODULE(bdsq_g1_priv_txn);


/******************************************************************************
 BDSQ_g1_Txn_P_UpdateSettings()
******************************************************************************/
BERR_Code BDSQ_g1_Txn_P_UpdateSettings(BDSQ_ChannelHandle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   uint32_t mb;
   
   /* set envelope or tone mode */
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DDIO, &mb);
   mb &= ~0x00700000;
   if (h->settings.bEnvelope)
      mb |= 0x00100000;
   if (hChn->configParam[BDSQ_g1_CONFIG_ENABLE_LNBPU])
      mb |= 0x10000000;
   else
      mb &= ~0x10000000;
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DDIO, mb);
   
   /* tone align enable or disable */
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL01, &mb);
   if (h->settings.bToneAlign)
      mb |= 0x00000800;
   else
      mb &= ~0x00000800;
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL01, mb);
   
   /* receive reply timeout enable or disable */
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL01, &mb);
   if (h->settings.bDisableRRTO)
      mb |= 0x400000;
   else
      mb &= ~0x400000;
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL01, mb);
   
   /* program receive reply timeout duration */
   BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_RXRT, ~0x000FFFFF, hChn->configParam[BDSQ_g1_CONFIG_RRTO_US] & 0x000FFFFF);
   
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL00, &mb);
   /* burst enable or disable */
   if (h->settings.bEnableToneburst)
      mb |= 0x40;
   else
      mb &= ~0x40;
   /* tone A or tone B */
   if (h->settings.bToneburstB)
      mb |= 0x80;
   else
      mb &= ~0x80;
   /* expect reply enable or disable */
   if ((h->settings.bOverrideFraming) && (h->settings.bExpectReply == false))
      mb |= 0x04;
   else
      mb &= ~0x04;
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL00, mb);
   
   /* program pretx delay */
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_Q15T, &mb);
   mb |= (hChn->configParam[BDSQ_g1_CONFIG_PRETX_DELAY_MS] * 1000) << 16;
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_Q15T, mb);
   
   return retCode;
}


/******************************************************************************
 BDSQ_g1_Txn_P_Transmit() - ISR context
******************************************************************************/
BERR_Code BDSQ_g1_Txn_P_Transmit(BDSQ_ChannelHandle h)
{
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb, dsctl;
   uint8_t nBytesToTx, i;
   
   /* clear diseqc status */
   BDSQ_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x00000001);
   
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DSCTL00, &dsctl);
   
   if (hChn->txLen)
   {
      if (h->settings.bOverrideFraming)
      {
         if (h->settings.bExpectReply)
         {
            /* always expect reply byte */
            hChn->dsecStatus.bRxExpected = true;
            dsctl |= 0x0000000C;
         }
         else
         {
            /* don't expect reply byte */
            dsctl &= ~0x00000008;
            dsctl |= 0x00000004;
         }
      }
      else
      {
         /* reply expectation depends on framing byte */
         dsctl &= ~0x00000004;
         if (hChn->txBuf[0] & 0x02)
            hChn->dsecStatus.bRxExpected = true;
      }
      
      /* fifo limit is 16 bytes */
      if (hChn->txLen <= 16)
         nBytesToTx = hChn->txLen;
      else
         nBytesToTx = 16;
      
      for (i = 0; i < nBytesToTx; i++)
      {
         mb = hChn->txBuf[hChn->txCount++];
         BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCMD, mb);
      }
   }
   else
   {
      if (h->settings.bExpectReply)
      {
         /* receive only mode */
         hChn->dsecStatus.bRxExpected = true;
         dsctl |= 0x0000000C;
      }
      else
      {
         /* don't expect reply byte */
         dsctl &= ~0x00000008;
         dsctl |= 0x00000004;
      }
   }
   
   if (hChn->dsecStatus.bRxExpected)
   {
      /* power on diseqc rx if reply expected */
      BDSQ_P_AndRegister(h, BCHP_SDS_DSEC_DSCTL02, ~0x00800000);
   }
   
   /* check for extended send */
   if (hChn->txLen > 16)
   {
      /* set almost empty levels */
      BDSQ_P_ReadModifyWriteRegister(h, BCHP_SDS_DSEC_DSCTL01, ~0x00070000, 0x00020000);
      
      /* set up tx fifo almost empty interrupt */
      BINT_ClearCallback_isr(hChn->hTxAlmostEmptyCb);
      BINT_EnableCallback_isr(hChn->hTxAlmostEmptyCb);
   }
   
   /* clear and enable the diseqc transaction done interrupt */
   BINT_ClearCallback_isr(hChn->hDiseqcDoneCb);
   BINT_EnableCallback_isr(hChn->hDiseqcDoneCb);
   
   /* start the transaction */
   dsctl |= 0x00000002;
   BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCTL00, dsctl);
   BDSQ_P_AndRegister(h, BCHP_SDS_DSEC_DSCTL00, ~0x00000002);  /* clear start txn */
   
   return retCode;
}


/******************************************************************************
 BDSQ_g1_Txn_P_TxAlmostEmpty_isr()
******************************************************************************/
void BDSQ_g1_Txn_P_TxAlmostEmpty_isr(void *p, int param)
{
   BDSQ_ChannelHandle h = (BDSQ_ChannelHandle)p;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   uint32_t mb;
   uint8_t bytesAvail, nBytesToTx, i;
   
   BSTD_UNUSED(param);
   
   /* clear and disable interrupt */
   BINT_ClearCallback_isr(hChn->hTxAlmostEmptyCb);
   BINT_DisableCallback_isr(hChn->hTxAlmostEmptyCb);
   
   /* determine number of bytes available in tx fifo */
   BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DST1, &mb);
   bytesAvail = (mb >> 14) & 0x1F;
   
   /* cannot send more bytes than what's available */
   if (hChn->txLen - hChn->txCount <= bytesAvail)
      nBytesToTx = hChn->txLen - hChn->txCount;
   else
      nBytesToTx = bytesAvail;
   
   for (i = 0; i < nBytesToTx; i++)
   {
      mb = hChn->txBuf[hChn->txCount++];
      BDSQ_P_WriteRegister(h, BCHP_SDS_DSEC_DSCMD, mb);
   }
   
   if (hChn->txCount < hChn->txLen)
   {
      /* re-arm tx fifo almost empty interrupt if more to transmit */
      BINT_ClearCallback_isr(hChn->hTxAlmostEmptyCb);
      BINT_EnableCallback_isr(hChn->hTxAlmostEmptyCb);
   }
}


/******************************************************************************
 BDSQ_g1_Txn_P_DiseqcDone_isr() - ISR context
******************************************************************************/
void BDSQ_g1_Txn_P_DiseqcDone_isr(void *p, int param)
{
   BDSQ_ChannelHandle h = (BDSQ_ChannelHandle)p;
   BDSQ_g1_P_ChannelHandle *hChn = (BDSQ_g1_P_ChannelHandle *)h->pImpl;
   uint32_t dst1, dst2, val;
   uint8_t i;
   
   BSTD_UNUSED(param);
   
   /* clear and disable interrupt */
   BINT_ClearCallback_isr(hChn->hDiseqcDoneCb);
   BINT_DisableCallback_isr(hChn->hDiseqcDoneCb);
   hChn->dsecStatus.status = BDSQ_SendStatus_eSuccess;   /* clear diseqc busy flag */
   
   /* drop incoming packet if rx buffer not read yet */
   if ((hChn->bufStatus & BDSQ_BUF_RX_NOT_EMPTY) == 0)
   {
      /* check diseqc status */
      BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DST1, &dst1);
      
      if (dst1 & 0x01000000)
      {
         /* error detected */
         if (dst1 & 0x1000)
            hChn->dsecStatus.status = BDSQ_SendStatus_eRxOverflow;
         else if (dst1 & 0x0800)
            hChn->dsecStatus.status = BDSQ_SendStatus_eRxReplyTimeout;
         else if (dst1 & 0x0400)
         {
            hChn->dsecStatus.status = BDSQ_SendStatus_eRxParityError;
            BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DST2, &val);
            hChn->dsecStatus.posParityErr = val >> 16;
         }
      }

      /* check additional diseqc status */
      BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DST2, &dst2);
      if (dst2 & 0x8000)
         hChn->dsecStatus.status = BDSQ_SendStatus_eRxIncomplete;

      /* get size of diseqc reply */
      hChn->dsecStatus.nRxBytes = (dst1 >> 19) & 0x1F;
      
      /* limit size of reply to maximum buffer size */
      if (hChn->dsecStatus.nRxBytes > BDSQ_MAX_REPLY_SIZE)
      {
         hChn->dsecStatus.nRxBytes = BDSQ_MAX_REPLY_SIZE;
         hChn->dsecStatus.status = BDSQ_SendStatus_eRxOverflow;
      }
      
      if (hChn->dsecStatus.nRxBytes > 0)
      {
         hChn->rxIdx = 0;  /* start reading message from beginning */
      	 hChn->bufStatus |= BDSQ_BUF_RX_NOT_EMPTY;
         
         for (i = 0; i < hChn->dsecStatus.nRxBytes; i++)
         {
            BDSQ_P_ReadRegister(h, BCHP_SDS_DSEC_DSRPLY, &val);
            hChn->rxBuf[i] = (uint8_t)(val & 0xFF);
         }

         /* signal rx event if rx bytes received */
         BKNI_SetEvent(hChn->hRxEvent);
      }
   }
   
   /* reset the FIFO, memory, noise integrator, etc. */
   BDSQ_P_ToggleBit(h, BCHP_SDS_DSEC_DSCTL00, 0x7F002201);
   
   /* power off diseqc rx */
   BDSQ_P_OrRegister(h, BCHP_SDS_DSEC_DSCTL02, 0x00800000);
   
   /* signal tx done event if non-zero bytes transmitted */
   if (hChn->txLen)
      BKNI_SetEvent(hChn->hTxEvent);
}
