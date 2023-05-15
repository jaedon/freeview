/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4538.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/15/12 2:12p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/4538/bast_4538.c $
 * 
 * Hydra_Software_Devel/5   10/15/12 2:12p enavarro
 * SWSATFE-220: implemented BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/4   10/12/12 9:19a enavarro
 * SWSATFE-174: added BAST_4538_WriteGpo()
 * 
 * Hydra_Software_Devel/3   8/1/12 2:33p enavarro
 * SWSATFE-174: added BAST_4538_P_SetAdcSelect() and
 * BAST_4538_P_GetAdcSelect()
 * 
 * Hydra_Software_Devel/2   7/23/12 1:52p enavarro
 * SWSATFE-174: ongoing development
 * 
 * Hydra_Software_Devel/1   7/11/12 4:36p enavarro
 * SWSATFE-174: initial check-in
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bchp_hif.h"
#include "bast_priv.h"
#include "bast_4538.h"
#include "bast_4538_priv.h"


BDBG_MODULE(bast_4538);


static const BAST_Settings defDevSettings =
{
   {  /* host i2c settings */
      0x6C, /* chipAddr */
      NULL, /* interruptEnableFunc */
      NULL  /* interruptEnableFuncParam */
   },
   { /* API function table */
      BAST_4538_P_Open,
      BAST_4538_P_Close,
      BAST_4538_P_GetTotalChannels,
      BAST_4538_GetChannelDefaultSettings,
      BAST_4538_P_OpenChannel,
      BAST_4538_P_CloseChannel,
      BAST_4538_P_GetDevice,
      BAST_4538_P_InitAp,
      BAST_4538_P_SoftReset,
      BAST_4538_P_GetApStatus,
      BAST_4538_P_GetApVersion,
      NULL, /* BAST_SetTmConfig, */
      NULL, /* BAST_GetTmConfig, */
      BAST_4538_P_ConfigGpio,
      BAST_4538_P_SetGpio,
      BAST_4538_P_GetGpio,
      BAST_4538_P_TuneAcquire,
      BAST_4538_P_GetChannelStatus,
      BAST_4538_P_GetLockStatus,
      BAST_4538_P_ResetStatus,
      BAST_4538_P_SetDiseqcTone,
      BAST_4538_P_GetDiseqcTone,
      BAST_4538_P_SetDiseqcVoltage,
      BAST_4538_P_SendDiseqcCommand,
      BAST_4538_P_GetDiseqcStatus,
      BAST_4538_P_ResetDiseqc,
      BAST_4538_P_ResetFsk, 
      BAST_4538_P_ReadFsk, 
      BAST_4538_P_WriteFsk, 
      BAST_4538_P_PowerDownFsk,
      BAST_4538_P_PowerUpFsk,
      NULL, /* BAST_4538_P_WriteMi2c, */
      NULL, /* BAST_4538_P_ReadMi2c, */
      BAST_4538_P_GetSoftDecisionBuf,
      BAST_4538_P_ReadAgc, 
      BAST_4538_P_WriteAgc,
      BAST_4538_P_FreezeAgc,
      BAST_4538_P_FreezeEq,
      BAST_4538_P_PowerDown,
      BAST_4538_P_PowerUp,     
      BAST_4538_P_ReadRegister,
      BAST_4538_P_WriteRegister,
      BAST_4538_P_ReadConfig,
      BAST_4538_P_WriteConfig,
      NULL, /* BAST_GetInterruptEventHandle */
      BAST_4538_P_GetLockStateChangeEventHandle,
      BAST_4538_P_GetFskEventHandle,
      BAST_4538_P_GetDiseqcEventHandle,
      NULL, /* BAST_HandleInterrupt_isr */
      NULL, /* BAST_ProcessInterruptEvent */
      BAST_4538_P_AbortAcq,
      NULL, /* BAST_ConfigLna */
      NULL, /* BAST_GetLnaStatus */
      NULL, /* BAST_ConfigAgc */
      BAST_4538_P_SendACW,
      BAST_4538_P_GetDiseqcVoltage,
      BAST_4538_P_GetDiseqcVsenseEventHandles,
      BAST_4538_P_EnableVsenseInterrupts,
      BAST_4538_P_PeakScan,
      BAST_4538_P_GetPeakScanStatus,
      BAST_4538_P_GetPeakScanEventHandle,
      BAST_4538_P_EnableStatusInterrupts,
      BAST_4538_P_GetStatusEventHandle,
      NULL, /* BAST_4538_P_ConfigBcm3445, */
      NULL, /* BAST_4538_P_MapBcm3445ToTuner, */
      NULL, /* BAST_4538_P_GetBcm3445Status, */
      BAST_4538_P_EnableSpurCanceller, 
      BAST_4538_P_ResetChannel, 
      BAST_4538_P_EnableDiseqcLnb, 
      BAST_4538_P_SetSearchRange,
      BAST_4538_P_GetSearchRange,
      BAST_4538_P_SetAmcScramblingSeq, 
      BAST_4538_P_SetTunerFilter,
      BAST_4538_P_GetSignalDetectStatus,
      BAST_4538_P_SetOutputTransportSettings,
      BAST_4538_P_GetOutputTransportSettings,
      BAST_4538_P_SetDiseqcSettings,
      BAST_4538_P_GetDiseqcSettings, 
      BAST_4538_P_SetNetworkSpec,
      BAST_4538_P_GetNetworkSpec,
      NULL, /* BAST_4538_P_TunerConfigLna, */
      NULL, /* BAST_4538_P_GetTunerLnaStatus, */
      NULL, /* BAST_SetFskChannel */
      NULL, /* BAST_GetFskChannel */
      BAST_4538_P_SetPeakScanSymbolRateRange,
      BAST_4538_P_GetPeakScanSymbolRateRange,
      BAST_4538_P_SetAdcSelect,
      BAST_4538_P_GetAdcSelect,
      BAST_4538_P_GetVersionInfo,
   },
   BAST_NetworkSpec_eDefault
};


static const BAST_ChannelSettings defChnSettings =
{
   (uint8_t)0
};


/******************************************************************************
 BAST_4538_GetDefaultSettings()
******************************************************************************/
BERR_Code BAST_4538_GetDefaultSettings(
   BAST_Settings *pDefSettings /* [out] default settings */
)
{
   *pDefSettings = defDevSettings;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4538_P_GetChannelDefaultSettings()
******************************************************************************/
BERR_Code BAST_4538_GetChannelDefaultSettings(
   BAST_Handle   h,                      /* [in] BAST handle */
   uint32_t      chnNo,                  /* [in] channel number */
   BAST_ChannelSettings *pChnDefSettings /* [out] default channel settings */
)
{
   BSTD_UNUSED(h);
   BSTD_UNUSED(chnNo);
   *pChnDefSettings = defChnSettings;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4538_PrintUart()
******************************************************************************/
BERR_Code BAST_4538_PrintUart(BAST_Handle h, char *pBuf)
{
   BAST_4538_P_Handle *pImpl = (BAST_4538_P_Handle *)(h->pImpl);
   uint8_t len, buf[128], n;

   buf[0] = 0x0C;
   for (len = 0; len < 124; len++)
   {
      buf[len+2] = pBuf[len];
      if (pBuf[len] == 0)
         break;
   }
   buf[1] = len; 

   n = (uint8_t)(3+len);
   return BHAB_SendHabCommand(pImpl->hHab, buf, n, buf, n, true, true, n);
}


/******************************************************************************
 BAST_4538_EnableAvs()
******************************************************************************/
BERR_Code BAST_4538_EnableAvs(BAST_Handle h, bool bEnable)
{
   BAST_4538_P_Handle *pImpl = (BAST_4538_P_Handle *)(h->pImpl);
   uint8_t buf[4];

   buf[0] = 0x36;
   buf[1] = bEnable ? 1 : 0;
   return BHAB_SendHabCommand(pImpl->hHab, buf, 3, buf, 3, true, true, 3);
}


/******************************************************************************
 BAST_4538_GetAvsStatus()
******************************************************************************/
BERR_Code BAST_4538_GetAvsStatus(BAST_Handle h, BAST_AvsStatus *pStatus)
{
   BAST_4538_P_Handle *pImpl = (BAST_4538_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[12];

   buf[0] = 0x29;
   BAST_4538_CHK_RETCODE(BHAB_SendHabCommand(pImpl->hHab, buf, 11, buf, 11, true, true, 11));

   pStatus->bEnabled = (buf[1] & 0x01) ? true : false;
   pStatus->temperature = (uint32_t)((buf[2] << 24) | (buf[3] << 16) | (buf[4] << 8) | buf[5]);
   pStatus->voltage_1p1_0 = (uint32_t)((buf[6] << 24) | (buf[7] << 16) | (buf[8] << 8) | buf[9]);

   done:
   return retCode;
}


/******************************************************************************
 BAST_4538_WriteBsc()
******************************************************************************/ 
BERR_Code BAST_4538_WriteBsc(
   BAST_Handle h,        /* [in] BAST device handle */
   uint8_t channel,      /* [in] BSC channel select */
   uint8_t slave_addr,   /* [in] address of the i2c slave device */
   uint8_t *i2c_buf,     /* [in] specifies the data to transmit */
   uint8_t n             /* [in] number of bytes to transmit after the i2c slave address */
)
{
   BAST_4538_P_Handle *pImpl = (BAST_4538_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[128], i, len;
   
   if (channel > 2)
      return BERR_INVALID_PARAMETER;

   if ((n == 0) || (n > 8))
      return BERR_INVALID_PARAMETER;

   buf[0] = 0x26;
   buf[1] = channel;
   buf[2] = 0;
   buf[3] = n;
   buf[4] = slave_addr & 0xFE;
   for (i = 0; i < n; i++)
      buf[5+i] = i2c_buf[i];
   len = (uint8_t)(6+n);
   BAST_4538_CHK_RETCODE(BHAB_SendHabCommand(pImpl->hHab, buf, len, buf, len, true, true, len));

   switch (buf[2])
   {
      case 0:
         retCode = BERR_SUCCESS;
         break;

      case 1:
         retCode = BAST_ERR_MI2C_NO_ACK;
         BDBG_WRN(("no i2c ack"));
         break;

      case 2:
         retCode = BAST_ERR_MI2C_BUSY;
         BDBG_WRN(("mi2c busy"));
         break;

      default:
         retCode = BAST_ERR_HAB_FAIL;
         BDBG_WRN(("mi2c error"));
         break;
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_4538_ReadBsc()
******************************************************************************/ 
BERR_Code BAST_4538_ReadBsc(
   BAST_Handle h,        /* [in] BAST device handle */
   uint8_t channel,      /* [in] bsc channel select */
   uint8_t slave_addr,   /* [in] address of the i2c slave device */
   uint8_t *out_buf,     /* [in] specifies the data to transmit before the i2c restart condition */
   uint8_t out_n,        /* [in] number of bytes to transmit before the i2c restart condition not including the i2c slave address */
   uint8_t *in_buf,      /* [out] holds the data read */
   uint8_t in_n          /* [in] number of bytes to read after the i2c restart condition not including the i2c slave address */
)
{
   BAST_4538_P_Handle *pImpl = (BAST_4538_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[128], i, len;
   
   if ((out_n == 0) || (out_n > 8) || (in_n == 0) || (in_n > 8) || (in_buf == NULL) || (channel > 2))
      return BERR_INVALID_PARAMETER;

   buf[0] = 0x27;
   buf[1] = channel;
   buf[2] = 0;
   buf[3] = out_n;
   buf[4] = in_n;
   buf[5] = slave_addr & 0xFE;
   for (i = 0; i < out_n; i++)
      buf[6+i] = out_buf[i];
   len = (uint8_t)(7+out_n+in_n);
   BAST_4538_CHK_RETCODE(BHAB_SendHabCommand(pImpl->hHab, buf, len, buf, len, true, true, len));

   switch (buf[2])
   {
      case 0:
         retCode = BERR_SUCCESS;
         for (i = 0; i < in_n; i++)
            in_buf[i] = buf[6+out_n+i];
         break;

      case 1:
         retCode = BAST_ERR_MI2C_NO_ACK;
         BDBG_WRN(("no i2c ack"));
         break;

      case 2:
         retCode = BAST_ERR_MI2C_BUSY;
         BDBG_WRN(("mi2c busy"));
         break;

      default:
         retCode = BAST_ERR_HAB_FAIL;
         BDBG_WRN(("mi2c error"));
         break;
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_4538_WriteGpo()
******************************************************************************/ 
BERR_Code BAST_4538_WriteGpo(
   BAST_Handle h,        /* [in] BAST device handle */
   uint32_t mask,        /* [in] specifies which GPO pins to write */
   uint32_t data         /* [in] specifies the state of the GPO pins to be written */
)
{
   BAST_4538_P_Handle *pImpl = (BAST_4538_P_Handle *)(h->pImpl);
   BERR_Code retCode;
   uint8_t buf[10];
   
   buf[0] = 0x28;
   buf[1] = (uint8_t)((mask >> 24) & 0xFF);
   buf[2] = (uint8_t)((mask >> 16) & 0xFF);
   buf[3] = (uint8_t)((mask >> 8) & 0xFF);
   buf[4] = (uint8_t)(mask & 0xFF);
   buf[5] = (uint8_t)((data >> 24) & 0xFF);
   buf[6] = (uint8_t)((data >> 16) & 0xFF);
   buf[7] = (uint8_t)((data >> 8) & 0xFF);
   buf[8] = (uint8_t)(data & 0xFF);
   BAST_4538_CHK_RETCODE(BHAB_SendHabCommand(pImpl->hHab, buf, 10, buf, 10, true, true, 10));

   done:
   return retCode;
}