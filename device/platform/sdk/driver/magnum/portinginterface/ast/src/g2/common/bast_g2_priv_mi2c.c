/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g2_priv_mi2c.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 9/6/12 6:16p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/common/bast_g2_priv_mi2c.c $
 * 
 * Hydra_Software_Devel/9   9/6/12 6:16p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/8   8/6/09 4:16p ronchan
 * PR 57474: re-add critical section to protect mi2c mutex
 * 
 * Hydra_Software_Devel/7   4/20/09 2:04p enavarro
 * PR 37769: added counters for all interrupts
 * 
 * Hydra_Software_Devel/6   9/9/08 10:31a ronchan
 * PR 37769: removed no-interrupt option
 * 
 * Hydra_Software_Devel/5   8/5/08 4:58p ronchan
 * PR 37881: removed '//' style comments
 * 
 * Hydra_Software_Devel/4   2/19/08 6:29p jrubio
 * PR38507: fix compile false warnings
 * 
 * Hydra_Software_Devel/3   12/18/07 2:33p enavarro
 * PR 37769: use _isr form of BINT_* callback functions
 * 
 * Hydra_Software_Devel/2   12/5/07 4:16p agin
 * PR37869: Integrate SDS into 97325 reference software.
 * 
 * Hydra_Software_Devel/1   11/21/07 11:31a ronchan
 * PR 32395: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2_priv.h"

BDBG_MODULE(bast_g2_priv_mi2c);

/* local functions */
BERR_Code BAST_g2_P_InitiateWriteMi2c(BAST_ChannelHandle h, uint8_t slave_addr, uint8_t *i2c_buf, uint8_t n);
BERR_Code BAST_g2_P_InitiateReadMi2c(BAST_ChannelHandle h, uint8_t slave_addr, uint8_t *out_buf, uint8_t out_n, uint8_t in_n);


/******************************************************************************
 BAST_g2_P_Mi2cInit()
******************************************************************************/
void BAST_g2_P_Mi2cInit(BAST_ChannelHandle h, uint8_t chip_address)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   uint32_t val;

   /* reset the mi2c */
   val = 0x80;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IICTL2, &val);
   val = 0x00;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IICTL2, &val);

   /* program chip address */
   val = chip_address;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IICHPA, &val);
   
   BINT_EnableCallback(hChn->hMi2cCb);   
}


/******************************************************************************
 BAST_g2_P_WriteMi2c() - called in task (not isr)
******************************************************************************/ 
BERR_Code BAST_g2_P_WriteMi2c(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint8_t slave_addr,   /* [in] address of the i2c slave device */
   uint8_t *i2c_buf,     /* [in] specifies the data to transmit */
   uint8_t n             /* [in] number of bytes to transmit after the i2c slave address */
)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   bool bBusy;

   if ((n > 8) || (i2c_buf == NULL))
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      return retCode;
   }

   BKNI_EnterCriticalSection();
   bBusy = hChn->bMi2cInProgress;
   if (!bBusy)
      hChn->bMi2cInProgress = true;
   BKNI_LeaveCriticalSection();
     
   if (bBusy)
   {
      BERR_TRACE(retCode = BAST_ERR_MI2C_BUSY);
      return retCode;
   }

   BKNI_WaitForEvent(hChn->hMi2cEvent, 0);
   retCode = BAST_g2_P_InitiateWriteMi2c(h, slave_addr, i2c_buf, n);   
   if (retCode == BERR_SUCCESS)
      retCode = BKNI_WaitForEvent(hChn->hMi2cEvent, 100);

   hChn->bMi2cInProgress = false;
   return retCode;
}


/******************************************************************************
 BAST_g2_P_InitiateWriteMi2c()
******************************************************************************/ 
BERR_Code BAST_g2_P_InitiateWriteMi2c(BAST_ChannelHandle h, uint8_t slave_addr, uint8_t *i2c_buf, uint8_t n)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val, cnt, i;

   BAST_g2_P_Mi2cInit(h, slave_addr);

   /* program number of bytes to transfer */
   cnt = n & 0x0F;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IICCNT, &cnt);

   /* set to normal mode */
   val = 0;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IICTL1, &val);

   /* load the transmit buffer */
   val = 0;
   for (i = 0; i < 8; i++)
   {
      if (i < 4)
      {
         if (i < cnt)
            val |= ((i2c_buf[i] & 0xFF) << ((3-i)*8));
         if (i == 3)
         {
            BAST_g2_P_WriteRegister(h, BCHP_SDS_MIICTX1, &val);
            val = 0;
         }
      }
      else
      {
         if (i < cnt)
            val |= ((i2c_buf[i] & 0xFF) << ((7-i)*8));
         if (i == 7)
            BAST_g2_P_WriteRegister(h, BCHP_SDS_MIICTX2, &val);
      }
   }

   /* write-only mode, start transfer */
   val = 0x01;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IICTL2, &val);

   return retCode;
}


/******************************************************************************
 BAST_g2_P_ReadMi2c()
******************************************************************************/
BERR_Code BAST_g2_P_ReadMi2c(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint8_t slave_addr,   /* [in] address of the i2c slave device */
   uint8_t *out_buf,     /* [in] specifies the data to transmit before the i2c restart condition */
   uint8_t out_n,        /* [in] number of bytes to transmit before the i2c restart condition not including the i2c slave address */
   uint8_t *in_buf,      /* [out] holds the data read */
   uint8_t in_n          /* [in] number of bytes to read after the i2c restart condition not including the i2c slave address */
)
{
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   BERR_Code retCode;
   uint32_t val;
   bool bBusy;
   
   if ((in_n == 0) || (in_n > 8) || (out_n > 8))
   {
      BERR_TRACE(retCode = BERR_INVALID_PARAMETER);
      return retCode;
   }
   
   BKNI_EnterCriticalSection();
   bBusy = hChn->bMi2cInProgress;
   if (!bBusy)
      hChn->bMi2cInProgress = true;
   BKNI_LeaveCriticalSection();
   
   if (bBusy)
   {
      BERR_TRACE(retCode = BAST_ERR_MI2C_BUSY);
      return retCode;
   }
   
   BKNI_WaitForEvent(hChn->hMi2cEvent, 0);
   retCode = BAST_g2_P_InitiateReadMi2c(h, slave_addr, out_buf, out_n, in_n);
   if (retCode == BERR_SUCCESS)
      retCode = BKNI_WaitForEvent(hChn->hMi2cEvent, 100);
   
   if (retCode == BERR_SUCCESS)
   {
      BAST_g2_P_ReadRegister(h, BCHP_SDS_MI2CSA, &val);
      if (val & 0x02000000)
      {
         BERR_TRACE(retCode = BAST_ERR_MI2C_NO_ACK);
         BDBG_MSG(("i2c no ack"));
      }
      else
      {
         /* read read_n bytes into mi2c_buffer */
         BAST_g2_P_ReadRegister(h, BCHP_SDS_MIICRX1, &val);
         in_buf[0] = (uint8_t)((val >> 24) & 0xFF);
         if (in_n > 1)
            in_buf[1] = (uint8_t)((val >> 16) & 0xFF);
         if (in_n > 2)
            in_buf[2] = (uint8_t)((val >> 8) & 0xFF);
         if (in_n > 3)
            in_buf[3] = (uint8_t)(val & 0xFF);
         if (in_n > 4)
         {
            BAST_g2_P_ReadRegister(h, BCHP_SDS_MIICRX2, &val);
            in_buf[4] = (uint8_t)((val >> 24) & 0xFF);
         }
         if (in_n > 5)
            in_buf[5] = (uint8_t)((val >> 16) & 0xFF);
         if (in_n > 2)
            in_buf[6] = (uint8_t)((val >> 8) & 0xFF);
         if (in_n > 3)
            in_buf[7] = (uint8_t)(val & 0xFF);
      }
   }
   
   hChn->bMi2cInProgress = false;
   
   return retCode;
}


/******************************************************************************
 BAST_g2_P_InitiateReadMi2c()
******************************************************************************/
BERR_Code BAST_g2_P_InitiateReadMi2c(BAST_ChannelHandle h, uint8_t slave_addr, uint8_t *out_buf, uint8_t out_n, uint8_t in_n)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t val, i;

   BAST_g2_P_Mi2cInit(h, slave_addr);

   /* program number of bytes to transfer */
   if (out_n == 0)
      val = in_n;  /* read only mode */
   else
      val = (out_n | (in_n << 4));
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IICCNT, &val);

   /* set to normal mode */
   val = 0;
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IICTL1, &val);

   /* load the transmit buffer */
   if (out_n)
   {
      /* load the transmit buffer */
      val = 0;
      for (i = 0; i < 8; i++)
      {
         if (i < 4)
         {
            if (i < out_n)
               val |= ((out_buf[i] & 0xFF) << ((3-i)*8));
            if (i == 3)
            {
               BAST_g2_P_WriteRegister(h, BCHP_SDS_MIICTX1, &val);
               val = 0;
            }
         }
         else
         {
            if (i < out_n)
               val |= ((out_buf[i] & 0xFF) << ((7-i)*8));
            if (i == 7)
               BAST_g2_P_WriteRegister(h, BCHP_SDS_MIICTX2, &val);
         }
      }
   }

   /* write-only mode, start transfer */
   if (out_n)
      val = 0x07; /* write-then-read mode */
   else
      val = 0x03; /* read only mode */
   BAST_g2_P_WriteRegister(h, BCHP_SDS_IICTL2, &val);

   return retCode;
}


/******************************************************************************
 BAST_g2_P_Mi2c_isr()
******************************************************************************/
void BAST_g2_P_Mi2c_isr(void *p, int param)
{
   BAST_ChannelHandle h = (BAST_ChannelHandle)p;
   BAST_g2_P_ChannelHandle *hChn = (BAST_g2_P_ChannelHandle *)h->pImpl;
   
   BSTD_UNUSED(param);
   
   BAST_g2_P_IncrementInterruptCounter(h, BAST_Sds_IntID_eMi2c);
   BINT_DisableCallback_isr(hChn->hMi2cCb);
   BKNI_SetEvent(hChn->hMi2cEvent);
}

