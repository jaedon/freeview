/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast.c $
 * $brcm_Revision: Hydra_Software_Devel/49 $
 * $brcm_Date: 11/28/12 2:20p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/common/bast.c $
 * 
 * Hydra_Software_Devel/49   11/28/12 2:20p ronchan
 * SWSATFE-174: add missing assert for tune acquire
 * 
 * Hydra_Software_Devel/48   11/28/12 1:47p enavarro
 * SWSATFE-228: added BAST_EXCLUDE_API_TABLE build option
 * 
 * Hydra_Software_Devel/47   11/8/12 3:35p enavarro
 * SWSATFE-174: changed BAST_SetAdcSelect()/BAST_GetAdcSelect() signature
 * 
 * Hydra_Software_Devel/46   10/19/12 11:00a ronchan
 * SW3461-274: changes to bfec structure
 * 
 * Hydra_Software_Devel/45   10/15/12 9:59a enavarro
 * SWSATFE-220: add BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/44   8/1/12 10:30a enavarro
 * SWSATFE-174: added BAST_SetAdcSelect() and BAST_GetAdcSelect() to the
 * API for chips with multiple wideband tuners
 * 
 * Hydra_Software_Devel/43   10/6/11 9:16a enavarro
 * SWSATFE-150: reworked spur canceller to use CWC instead of notch
 * filters
 * 
 * Hydra_Software_Devel/42   4/9/11 1:25p enavarro
 * SWSATFE-86: removed BDBG_ASSERT in BAST_Open()
 * 
 * Hydra_Software_Devel/41   1/27/11 3:45p enavarro
 * SW7346-21: added BAST_SetPeakScanSymbolRateRange(); added
 * BAST_GetPeakScanSymbolRateRange()
 * 
 * Hydra_Software_Devel/40   1/19/11 3:25p ronchan
 * SWSATFE-99: added chip-independent fsk channel config api
 * 
 * Hydra_Software_Devel/39   12/10/10 9:26a ronchan
 * SWSATFE-95: rename tuner LNA api per magnum guidelines
 * 
 * Hydra_Software_Devel/38   12/9/10 5:29p ronchan
 * SWSATFE-95: added functions to control tuner LNA
 * 
 * Hydra_Software_Devel/37   11/29/10 3:21p enavarro
 * SWSATFE-80: added BAST_GetNetworkSpec() and BAST_SetNetworkSpec() to
 * the API
 * 
 * Hydra_Software_Devel/36   9/30/10 3:11p ronchan
 * SWSATFE-80: added API functions to set/get diseqc settings
 * 
 * Hydra_Software_Devel/35   9/29/10 3:47p enavarro
 * SWSATFE-80: added API functions to set/get output transport settings
 * 
 * Hydra_Software_Devel/34   9/29/10 10:09a enavarro
 * SWSATFE-75: added BAST_GetSignalDetectStatus() to BAST API
 * 
 * Hydra_Software_Devel/33   9/17/10 11:23a enavarro
 * SWSATFE-80: added BAST_SetSearchRange(), BAST_GetSearchRange(),
 * BAST_SetTunerFilter(), and BAST_SetAmcScramblingSeq()
 * 
 * Hydra_Software_Devel/32   9/10/09 9:26a enavarro
 * SW7325-594: moved functions used for fixed point arithmetic to common
 * location
 * 
 * Hydra_Software_Devel/31   7/1/09 7:13p ronchan
 * PR 56509: added new entry in api function table for
 * BAST_EnableDiseqcLnb
 * 
 * Hydra_Software_Devel/30   5/12/09 3:17p ronchan
 * PR 54946: changed voltage type for BAST_GetDiseqcVoltage()
 * 
 * Hydra_Software_Devel/29   10/20/08 3:48p ronchan
 * PR 37769: renamed channel reset
 * 
 * Hydra_Software_Devel/28   10/17/08 3:01p ronchan
 * PR 37769: fixed call to channel reset
 * 
 * Hydra_Software_Devel/27   10/17/08 2:55p ronchan
 * PR 37769: added channel reset function
 * 
 * Hydra_Software_Devel/26   9/17/08 5:06p enavarro
 * PR 32395: added BAST_EnableSpurCanceller() to the API table
 * 
 * Hydra_Software_Devel/25   9/15/08 9:19a enavarro
 * PR 32395: add API functions to better manage external LNA
 * 
 * Hydra_Software_Devel/24   5/30/08 10:09a enavarro
 * PR 32395: added BAST_GetStatusEventHandle() to API function table
 * 
 * Hydra_Software_Devel/23   5/29/08 4:42p enavarro
 * PR 32395: added BAST_PeakScan(), BAST_GetPeakScanStatus(),
 * BAST_GetPeakScanEventHandle(), and BAST_EnableStatusInterrupts() to
 * API function table
 * 
 * Hydra_Software_Devel/22   5/5/08 10:50a ronchan
 * PR 37881: check for null api pointer for GetDiseqcVoltage()
 * 
 * Hydra_Software_Devel/21   4/28/08 2:17p ronchan
 * PR 37881: added vsense functions
 * 
 * Hydra_Software_Devel/20   3/25/08 4:58p ronchan
 * PR 37769: fixed comment for WriteConfig function
 * 
 * Hydra_Software_Devel/19   11/5/07 10:34a enavarro
 * PR 24793: changed BAST_GetLnaStatus() to return BAST_LnaStatus struct
 * 
 * Hydra_Software_Devel/18   7/27/07 4:08p enavarro
 * PR 33579: added BAST_GetLnaStatus()
 * 
 * Hydra_Software_Devel/15   5/1/07 10:55a enavarro
 * PR 30347: changed BAST_ConfigLna() to support various board
 * configurations
 * 
 * Hydra_Software_Devel/14   2/7/07 8:33a enavarro
 * PR 25473: changed BAST_ConfigLna() to pass in an enum instead of
 * uint32_t
 * 
 * Hydra_Software_Devel/13   11/7/06 2:33p enavarro
 * PR 24793: added BAST_ConfigAgc()
 * 
 * Hydra_Software_Devel/12   11/2/06 4:57p enavarro
 * PR 25473: add LNA support
 * 
 * Hydra_Software_Devel/11   8/10/06 9:09a enavarro
 * PR 18135: make BAST_SendDiseqcCommand() non-blocking; added
 * BAST_GetDiseqcEventHandle() and BAST_GetDiseqcStatus()
 * 
 * Hydra_Software_Devel/10   8/2/06 2:34p enavarro
 * PR 18135: added BAST_AbortAcq()
 * 
 * Hydra_Software_Devel/9   5/16/06 11:46a enavarro
 * PR 18135: added BAST_SoftReset()
 * 
 * Hydra_Software_Devel/8   4/11/06 4:50p enavarro
 * PR 18135: added power up/down functions
 * 
 * Hydra_Software_Devel/7   3/16/06 8:42a enavarro
 * PR 18135: added bond option to ApGetVersion(); added tunerFreq to
 * ChannelStatus
 * 
 * Hydra_Software_Devel/6   2/27/06 9:11a enavarro
 * PR 18135: added BAST_GetLockStatus()
 * 
 * Hydra_Software_Devel/5   12/22/05 11:10a enavarro
 * PR 18135: replaced BAST_Tune() and BAST_Acquire() with
 * BAST_TuneAcquire()
 * 
 * Hydra_Software_Devel/4   12/21/05 10:41a enavarro
 * PR 18135: updated BAST_ReadConfig()/BAST_WriteConfig()
 * 
 * Hydra_Software_Devel/3   11/30/05 5:16p enavarro
 * PR 18135: renamed BAST_TransponderSettings to BAST_AcqSettings
 * 
 * Hydra_Software_Devel/2   11/23/05 11:16a enavarro
 * PR 18135: added BAST_ConfigGpio() and BAST_GetGpio()
 * 
 * Hydra_Software_Devel/1   11/14/05 10:27a enavarro
 * PR 18135: initial version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"

BDBG_MODULE(bast);

#ifndef BAST_EXCLUDE_API_TABLE

/******************************************************************************
 BAST_Open()
******************************************************************************/
BERR_Code BAST_Open(
   BAST_Handle *h,         /* [out] BAST handle */
   BCHP_Handle hChip,      /* [in] chip handle */
   void        *pReg,      /* [in] pointer to register or i2c handle */
   BINT_Handle hInterrupt, /* [in] interrupt handle */
   const BAST_Settings *pDefSettings /* [in] default settings */
)
{
   BDBG_ASSERT(h);
   BDBG_ASSERT(pDefSettings);

   return (pDefSettings->api.Open(h, hChip, pReg, hInterrupt, pDefSettings));
}


/******************************************************************************
 BAST_Close()
******************************************************************************/
BERR_Code BAST_Close(
   BAST_Handle h   /* [in] BAST handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.Close(h));
}

/******************************************************************************
 BAST_GetTotalChannels()
******************************************************************************/
BERR_Code BAST_GetTotalChannels(
   BAST_Handle  h,             /* [in] BAST handle */
   uint32_t         *totalChannels /* [out] number of channels supported */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetTotalChannels(h, totalChannels));
}


/******************************************************************************
 BAST_GetChannelDefaultSettings()
******************************************************************************/
BERR_Code BAST_GetChannelDefaultSettings(
   BAST_Handle   h,                      /* [in] BAST handle */
   uint32_t      chnNo,                  /* [in] channel number */
   BAST_ChannelSettings *pChnDefSettings /* [out] default channel settings */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetChannelDefaultSettings(h, chnNo, pChnDefSettings));
}


/******************************************************************************
 BAST_OpenChannel()
******************************************************************************/
BERR_Code BAST_OpenChannel(
   BAST_Handle                h,               /* [in] BAST handle */
   BAST_ChannelHandle         *pChannelHandle, /* [out] BAST channel handle */
   uint32_t                   chnNo,           /* [in] channel number */
   const BAST_ChannelSettings *pSettings       /* [in] channel settings */
)
{
   BDBG_ASSERT(h);
   BDBG_ASSERT(pChannelHandle);
   return (h->settings.api.OpenChannel(h, pChannelHandle, chnNo, pSettings));
}


/******************************************************************************
 BAST_CloseChannel()
******************************************************************************/
BERR_Code BAST_CloseChannel(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.CloseChannel(h));
}


/******************************************************************************
 BAST_GetDevice()
******************************************************************************/
BERR_Code BAST_GetDevice(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   BAST_Handle *pDev      /* [out] BAST handle */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.GetDevice(h, pDev));
}

/******************************************************************************
 BAST_SoftReset()
******************************************************************************/
BERR_Code BAST_SoftReset(
   BAST_Handle h           /* [in] BAST device handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.SoftReset(h));
}


/******************************************************************************
 BAST_ResetChannel()
******************************************************************************/
BERR_Code BAST_ResetChannel(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool bForceReset       /* [in] true=force reset, false=abort when busy */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.ResetChannel)
      return (h->pDevice->settings.api.ResetChannel(h, bForceReset));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_InitAp()
******************************************************************************/
BERR_Code BAST_InitAp(
   BAST_Handle   h,   /* [in] BAST handle */
   const uint8_t *pImage  /* [in] pointer to AP microcode image */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.InitAp(h, pImage));
}


/******************************************************************************
 BAST_GetApStatus()
******************************************************************************/
BERR_Code BAST_GetApStatus(
   BAST_Handle   h,         /* [in] BAST device handle */
   BAST_ApStatus *pStatus   /* [out] AP status */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetApStatus(h, pStatus));
}


/******************************************************************************
 BAST_GetApVersion()
******************************************************************************/
BERR_Code BAST_GetApVersion(
   BAST_Handle h,          /* [in] BAST handle */
   uint16_t    *pChipId,   /* [out] BAST chip id */
   uint8_t     *pChipVer,  /* [out] chip revision number */
   uint32_t    *pBondId,   /* [out] chip bonding option */
   uint8_t     *pApVer,    /* [out] AP microcode version */
   uint8_t     *pCfgVer    /* [out] host configuration version */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.GetApVersion(h, pChipId, pChipVer, pBondId, pApVer, pCfgVer));
}


/******************************************************************************
 BAST_SetTmConfig()
******************************************************************************/
BERR_Code BAST_SetTmConfig(
   BAST_Handle h,     /* [in] BAST handle */
   void        *pCfg  /* [in] TM configuration parameters */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.SetTmConfig)
      return (h->settings.api.SetTmConfig(h, pCfg));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetTmConfig()
******************************************************************************/
BERR_Code BAST_GetTmConfig(
   BAST_Handle h,     /* [in] BAST handle */
   void        *pCfg  /* [out] TM configuration parameters */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.GetTmConfig)
      return (h->settings.api.GetTmConfig(h, pCfg));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_ConfigGpio()
******************************************************************************/
BERR_Code BAST_ConfigGpio(
   BAST_Handle h,           /* [in] BAST handle */
   uint32_t    write_mask,  /* [in] selects which GPIO pins are output */
   uint32_t    read_mask    /* [in] selects which GPIO pins are input */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.ConfigGpio)
      return (h->settings.api.ConfigGpio(h, write_mask, read_mask));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SetGpio()
******************************************************************************/
BERR_Code BAST_SetGpio(
   BAST_Handle h,     /* [in] BAST handle */
   uint32_t    mask,  /* [in] selects which GPIO pins to set */
   uint32_t    state  /* [in] state of GPIO pins */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.SetGpio)
      return (h->settings.api.SetGpio(h, mask, state));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetGpio()
******************************************************************************/
BERR_Code BAST_GetGpio(
   BAST_Handle h,      /* [in] BAST handle */
   uint32_t    mask,   /* [in] selects which GPIO pins to read */
   uint32_t    *state  /* [out] state of GPIO pins */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.GetGpio)
      return (h->settings.api.GetGpio(h, mask, state));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_TuneAcquire()
******************************************************************************/
BERR_Code BAST_TuneAcquire(
   BAST_ChannelHandle h,            /* [in] BAST channel handle */
   const uint32_t     freq,         /* [in] RF tuner frequency in Hz */
   const BAST_AcqSettings *pParams  /* [in] acquisition parameters */
)
{
   BDBG_ASSERT(h);
   BDBG_ASSERT(pParams);
   if (pParams == NULL)
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   else
      return (h->pDevice->settings.api.TuneAcquire(h, freq, pParams));
}


/******************************************************************************
 BAST_GetChannelStatus()
******************************************************************************/
BERR_Code BAST_GetChannelStatus(
   BAST_ChannelHandle h,        /* [in] BAST handle */
   BAST_ChannelStatus *pStatus  /* [out] pointer to status structure */
)
{
   BDBG_ASSERT(h);
   BDBG_ASSERT(pStatus);
   return (h->pDevice->settings.api.GetChannelStatus(h, pStatus));
}


/******************************************************************************
 BAST_GetLockStatus()
******************************************************************************/
BERR_Code BAST_GetLockStatus(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   bool *bLocked          /* [out] true if demod is locked */
)
{
   BDBG_ASSERT(h);
   BDBG_ASSERT(bLocked);
   return (h->pDevice->settings.api.GetLockStatus(h, bLocked));
}


/******************************************************************************
 BAST_ResetStatus()
******************************************************************************/
BERR_Code BAST_ResetStatus(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.ResetStatus(h));
}


/******************************************************************************
 BAST_SetDiseqcTone()
******************************************************************************/
BERR_Code BAST_SetDiseqcTone(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool bTone             /* [in] true = tone on, false = tone off */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.SetDiseqcTone(h, bTone));
}


/******************************************************************************
 BAST_GetDiseqcTone()
******************************************************************************/
BERR_Code BAST_GetDiseqcTone(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool *bTone            /* [out] true = tone present, false = tone absent */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.GetDiseqcTone(h, bTone));
}


/******************************************************************************
 BAST_SetDiseqcVoltage()
******************************************************************************/
BERR_Code BAST_SetDiseqcVoltage(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   bool bVtop            /* [in] true = VTOP, false = VBOT */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.SetDiseqcVoltage(h, bVtop));
}


/******************************************************************************
 BAST_GetDiseqcVoltage()
******************************************************************************/
BERR_Code BAST_GetDiseqcVoltage(
   BAST_ChannelHandle h,   /* [in] BAST channel handle */
   uint8_t *pVoltage       /* [out] voltage estimation */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.GetDiseqcVoltage)
      return (h->pDevice->settings.api.GetDiseqcVoltage(h, pVoltage));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_EnableDiseqcLnb()
******************************************************************************/
BERR_Code BAST_EnableDiseqcLnb(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   bool bEnable           /* [in] true = LNB on, false = LNB off */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.EnableDiseqcLnb)
      return (h->pDevice->settings.api.EnableDiseqcLnb(h, bEnable));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_EnableVsenseInterrupts()
******************************************************************************/
BERR_Code BAST_EnableVsenseInterrupts(
   BAST_ChannelHandle h,   /* [in] BAST channel handle */
   bool bEnable            /* [in] true = enable, false = disable */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.EnableVsenseInterrupts)
      return (h->pDevice->settings.api.EnableVsenseInterrupts(h, bEnable));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SendDiseqcCommand()
******************************************************************************/
BERR_Code BAST_SendDiseqcCommand(
   BAST_ChannelHandle h,       /* [in] BAST channel handle */
   const uint8_t *pSendBuf,    /* [in] contains data to send */
   uint8_t sendBufLen          /* [in] number of bytes to send */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.SendDiseqcCommand(h, pSendBuf, sendBufLen));
}


/******************************************************************************
 BAST_GetDiseqcStatus()
******************************************************************************/
BERR_Code BAST_GetDiseqcStatus(
   BAST_ChannelHandle h,       /* [in] BAST channel handle */
   BAST_DiseqcStatus *pStatus  /* [out] status of most recent transaction */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.GetDiseqcStatus(h, pStatus));
}


/******************************************************************************
 BAST_ResetDiseqc()
******************************************************************************/
BERR_Code BAST_ResetDiseqc(
   BAST_ChannelHandle h,    /* [in] BAST channel handle */
   uint8_t options          /* [in] reset options */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.ResetDiseqc(h, options));
}


/******************************************************************************
 BERR_Code BAST_ResetFtm()
******************************************************************************/
BERR_Code BAST_ResetFtm(
   BAST_Handle h  /* [in] BAST handle */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.ResetFtm)
      return (h->settings.api.ResetFtm(h));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BERR_Code BAST_ReadFtm()
******************************************************************************/
BERR_Code BAST_ReadFtm(
   BAST_Handle h,  /* [in] BAST handle */
   uint8_t *pBuf,  /* [out] data read */
   uint8_t *n      /* [out] length of data read */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.ReadFtm)
      return (h->settings.api.ReadFtm(h, pBuf, n));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BERR_Code BAST_WriteFtm()
******************************************************************************/
BERR_Code BAST_WriteFtm(
   BAST_Handle h,  /* [in] BAST handle */
   uint8_t *pBuf,  /* [in] data to write */
   uint8_t n       /* [in] length of data to write */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.WriteFtm)
      return (h->settings.api.WriteFtm(h, pBuf, n));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_PowerDownFtm()
******************************************************************************/
BERR_Code BAST_PowerDownFtm(
   BAST_Handle h  /* [in] BAST handle */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.PowerDownFtm)
      return (h->settings.api.PowerDownFtm(h));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_PowerUpFtm()
******************************************************************************/
BERR_Code BAST_PowerUpFtm(
   BAST_Handle h  /* [in] BAST handle */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.PowerUpFtm)
      return (h->settings.api.PowerUpFtm(h));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_WriteMi2c()
******************************************************************************/
BERR_Code BAST_WriteMi2c(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint8_t slave_addr,   /* [in] address of the i2c slave device */
   uint8_t *buf,         /* [in] specifies the data to transmit */
   uint8_t n             /* [in] number of bytes to transmit after the i2c slave address */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.WriteMi2c)
      return (h->pDevice->settings.api.WriteMi2c(h, slave_addr, buf, n));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_ReadMi2c()
******************************************************************************/
BERR_Code BAST_ReadMi2c(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint8_t slave_addr,   /* [in] address of the i2c slave device */
   uint8_t *out_buf,     /* [in] specifies the data to transmit before the i2c restart condition */
   uint8_t out_n,        /* [in] number of bytes to transmit before the i2c restart condition not including the i2c slave address */
   uint8_t *in_buf,      /* [out] holds the data read */
   uint8_t in_n          /* [in] number of bytes to read after the i2c restart condition not including the i2c slave address */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.ReadMi2c)
      return (h->pDevice->settings.api.ReadMi2c(h, slave_addr, out_buf, out_n, in_buf, in_n));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetSoftDecisionBuf()
******************************************************************************/
BERR_Code BAST_GetSoftDecisionBuf(
   BAST_ChannelHandle h,  /* [in] BAST channel handle */
   int16_t *pI,           /* [out] 30 I-values */
   int16_t *pQ            /* [out] 30 Q-values */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.GetSoftDecisionBuf(h, pI, pQ));
}

/******************************************************************************
 BERR_Code BAST_ReadAgc()
******************************************************************************/
BERR_Code BAST_ReadAgc(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   BAST_Agc which_agc,    /* [in] AGC select */
   uint32_t *pAgc         /* [out] integrator value */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.ReadAgc)
      return (h->pDevice->settings.api.ReadAgc(h, which_agc, pAgc));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BERR_Code BAST_WriteAgc()
******************************************************************************/
BERR_Code BAST_WriteAgc(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   BAST_Agc which_agc,    /* [in] AGC select */
   uint32_t *pAgc         /* [in] integrator value */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.WriteAgc)
      return (h->pDevice->settings.api.WriteAgc(h, which_agc, pAgc));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BERR_Code BAST_FreezeAgc()
******************************************************************************/
BERR_Code BAST_FreezeAgc(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   BAST_Agc which_agc,    /* [in] AGC select */
   bool bFreeze           /* [in] true = freeze, false = unfreeze */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.FreezeAgc)
      return (h->pDevice->settings.api.FreezeAgc(h, which_agc, bFreeze));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BERR_Code BAST_FreezeEq()
******************************************************************************/
BERR_Code BAST_FreezeEq(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   bool bFreeze           /* [in] true = freeze, false = unfreeze */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.FreezeEq)
      return (h->pDevice->settings.api.FreezeEq(h, bFreeze));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_PowerDown()
******************************************************************************/
BERR_Code BAST_PowerDown(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   uint32_t options       /* [in] see BAST_CORE_* macros */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.PowerDown)
      return (h->pDevice->settings.api.PowerDown(h, options));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_PowerUp()
******************************************************************************/
BERR_Code BAST_PowerUp(
   BAST_ChannelHandle h,  /* [in] BAST handle */
   uint32_t options       /* [in] see BAST_CORE_* macros */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.PowerUp)
      return (h->pDevice->settings.api.PowerUp(h, options));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_ReadRegister()
******************************************************************************/
BERR_Code BAST_ReadRegister(
   BAST_ChannelHandle h,     /* [in] BAST channel handle */
   uint32_t           reg,   /* [in] address of register to read */
   uint32_t           *val   /* [in] contains data that was read */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.ReadRegister(h, reg, val));
}


/******************************************************************************
 BAST_WriteRegister()
******************************************************************************/
BERR_Code BAST_WriteRegister(
   BAST_ChannelHandle h,     /* [in] BAST channel handle */
   uint32_t           reg,   /* [in] address of register to write */
   uint32_t           *val   /* [in] contains data to write */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.WriteRegister(h, reg, val));
}


/******************************************************************************
 BAST_ReadConfig()
******************************************************************************/
BERR_Code BAST_ReadConfig(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint16_t id,          /* [in] configuration parameter ID */
   uint8_t *buf,         /* [out] bytes read */
   uint8_t n             /* [in] length of configuration parameter */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.ReadConfig(h, id, buf, n));
}


/******************************************************************************
 BAST_WriteConfig()
******************************************************************************/
BERR_Code BAST_WriteConfig(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint16_t id,          /* [in] configuration parameter ID */
   uint8_t *buf,         /* [in] bytes to write */
   uint8_t n             /* [in] length of configuration parameter */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.WriteConfig(h, id, buf, n));
}


/******************************************************************************
 BAST_GetInterruptEventHandle()
******************************************************************************/
BERR_Code BAST_GetInterruptEventHandle(
   BAST_Handle h,            /* [in] BAST handle */
   BKNI_EventHandle *phEvent /* [out] event handle */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.GetInterruptEventHandle)
      return (h->settings.api.GetInterruptEventHandle(h, phEvent));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetLockStateChangeEventHandle()
******************************************************************************/
BERR_Code BAST_GetLockStateChangeEventHandle(
   BAST_ChannelHandle h,     /* [in] BAST channel handle */
   BKNI_EventHandle *hEvent  /* [out] lock event handle */
)
{
   BDBG_ASSERT(h);
   return (h->pDevice->settings.api.GetLockStateChangeEventHandle(h, hEvent));
}


/******************************************************************************
 BAST_GetFtmEventHandle()
******************************************************************************/
BERR_Code BAST_GetFtmEventHandle(
   BAST_Handle      h,       /* [in] BAST handle */
   BKNI_EventHandle *hEvent  /* [out] FTM event handle */
)
{
   BDBG_ASSERT(h);
   if (h->settings.api.GetFtmEventHandle)
      return (h->settings.api.GetFtmEventHandle(h, hEvent));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetDiseqcEventHandle()
******************************************************************************/
BERR_Code BAST_GetDiseqcEventHandle(
   BAST_ChannelHandle h,       /* [in] BAST channel handle */
   BKNI_EventHandle   *hEvent  /* [out] Diseqc event handle */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.GetDiseqcEventHandle)
      return (h->pDevice->settings.api.GetDiseqcEventHandle(h, hEvent));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetDiseqcVsenseEventHandles()
******************************************************************************/
BERR_Code BAST_GetDiseqcVsenseEventHandles(
   BAST_ChannelHandle h,                  /* [in] BAST channel handle */
   BKNI_EventHandle   *hOverVoltageEvent, /* [out] Diseqc event handle */
   BKNI_EventHandle   *hUnderVoltageEvent /* [out] Diseqc event handle */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.GetDiseqcVsenseEventHandles)
      return (h->pDevice->settings.api.GetDiseqcVsenseEventHandles(h, hOverVoltageEvent, hUnderVoltageEvent));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_HandleInterrupt_isr()
******************************************************************************/
BERR_Code BAST_HandleInterrupt_isr(
   BAST_Handle h   /* [in] BAST handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.HandleInterrupt_isr(h));
}


/******************************************************************************
 BAST_ProcessInterruptEvent()
******************************************************************************/
BERR_Code BAST_ProcessInterruptEvent(
   BAST_Handle h  /* [in] BAST handle */
)
{
   BDBG_ASSERT(h);
   return (h->settings.api.ProcessInterruptEvent(h));
}


/******************************************************************************
 BAST_AbortAcq()
******************************************************************************/
BERR_Code BAST_AbortAcq(
   BAST_ChannelHandle h  /* [in] BAST channel handle */
)
{
   BDBG_ASSERT(h);
   if (h->pDevice->settings.api.AbortAcq)
      return (h->pDevice->settings.api.AbortAcq(h));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_ConfigLna()
******************************************************************************/
BERR_Code BAST_ConfigLna(
   BAST_Handle    h,              /* [in] BAST handle */
   BAST_LnaIntConfig int_config,  /* [in] internal switch configuration */
   BAST_LnaExtConfig ext_config   /* [in] external configuration */
)
{
   if (h->settings.api.ConfigLna)
      return (h->settings.api.ConfigLna(h, int_config, ext_config));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetLnaStatus()
******************************************************************************/
BERR_Code BAST_GetLnaStatus(
   BAST_Handle    h,        /* [in] BAST handle */
   BAST_LnaStatus *pStatus  /* [out] lna status struct */
)
{
   if (h->settings.api.GetLnaStatus)
      return (h->settings.api.GetLnaStatus(h, pStatus));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_ConfigAgc()
******************************************************************************/
BERR_Code BAST_ConfigAgc(
   BAST_Handle h,    /* [in] BAST handle */
   uint32_t    ctl   /* [in] control word */
)
{
   if (h->settings.api.ConfigAgc)
      return (h->settings.api.ConfigAgc(h, ctl));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SendACW()
******************************************************************************/
BERR_Code BAST_SendACW(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint8_t acw           /* [in] auto control word to send */
)
{
   if (h->pDevice->settings.api.SendACW)
      return (h->pDevice->settings.api.SendACW(h, acw));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_PeakScan()
******************************************************************************/
BERR_Code BAST_PeakScan(
   BAST_ChannelHandle h,           /* [in] BAST channel handle */
   uint32_t           tunerFreq    /* [in] frequency in Hz */
)
{
   if (h->pDevice->settings.api.PeakScan)
      return (h->pDevice->settings.api.PeakScan(h, tunerFreq));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetPeakScanStatus()
******************************************************************************/
BERR_Code BAST_GetPeakScanStatus(
   BAST_ChannelHandle  h,          /* [in] BAST channel handle */
   BAST_PeakScanStatus *pStatus    /* [out] Diseqc event handle */
)
{
   if (h->pDevice->settings.api.GetPeakScanStatus)
      return (h->pDevice->settings.api.GetPeakScanStatus(h, pStatus));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetPeakScanEventHandle()
******************************************************************************/
BERR_Code BAST_GetPeakScanEventHandle(
   BAST_ChannelHandle h,                /* [in] BAST channel handle */
   BKNI_EventHandle  *hPeakScanEvent    /* [out] Peak scan done event handle */
)
{
   if (h->pDevice->settings.api.GetPeakScanEventHandle)
      return (h->pDevice->settings.api.GetPeakScanEventHandle(h, hPeakScanEvent));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_EnableStatusInterrupts()
******************************************************************************/
BERR_Code BAST_EnableStatusInterrupts(
   BAST_ChannelHandle h,   /* [in] BAST channel handle */
   bool bEnable            /* [in] true = enable, false = disable */
)
{
   if (h->pDevice->settings.api.EnableStatusInterrupts)
      return (h->pDevice->settings.api.EnableStatusInterrupts(h, bEnable));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetStatusEventHandle()
******************************************************************************/
BERR_Code BAST_GetStatusEventHandle(
   BAST_ChannelHandle h,        /* [in] BAST channel handle */
   BKNI_EventHandle  *hEvent    /* [out] status event handle */
)
{
   if (h->pDevice->settings.api.GetStatusEventHandle)
      return (h->pDevice->settings.api.GetStatusEventHandle(h, hEvent));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_ConfigBcm3445()
******************************************************************************/
BERR_Code BAST_ConfigBcm3445(
   BAST_Handle h,                  /* [in] BAST device handle */
   BAST_Bcm3445Settings *pSettings /* [in] BCM3445 configuration settings */
)
{
   if (h->settings.api.ConfigBcm3445)
      return (h->settings.api.ConfigBcm3445(h, pSettings));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_MapBcm3445ToTuner()
******************************************************************************/
BERR_Code BAST_MapBcm3445ToTuner(
   BAST_ChannelHandle        h,    /* [in] BAST channel handle */
   BAST_Mi2cChannel          mi2c, /* [in] specifies which BCM3445 */
   BAST_Bcm3445OutputChannel out   /* [in] BCM3445 output channel */
)
{
   if (h->pDevice->settings.api.MapBcm3445ToTuner)
      return (h->pDevice->settings.api.MapBcm3445ToTuner(h, mi2c, out));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetBcm3445Status()
******************************************************************************/
BERR_Code BAST_GetBcm3445Status(
   BAST_ChannelHandle h,       /* [in] BAST channel handle */
   BAST_Bcm3445Status *pStatus /* [out] BCM3445 status struct */
)
{
   if (h->pDevice->settings.api.GetBcm3445Status)
      return (h->pDevice->settings.api.GetBcm3445Status(h, pStatus));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_ConfigTunerLna()
******************************************************************************/
BERR_Code BAST_ConfigTunerLna(
   BAST_Handle h,                   /* [in] BAST device handle */
   BAST_TunerLnaSettings *pSettings /* [in] Tuner LNA configuration settings */
)
{
   if (h->settings.api.ConfigTunerLna)
      return (h->settings.api.ConfigTunerLna(h, pSettings));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetTunerLnaStatus()
******************************************************************************/
BERR_Code BAST_GetTunerLnaStatus(
   BAST_ChannelHandle h,         /* [in] BAST channel handle */
   BAST_TunerLnaStatus *pStatus  /* [out] Tuner LNA status struct */
)
{
   if (h->pDevice->settings.api.GetTunerLnaStatus)
      return (h->pDevice->settings.api.GetTunerLnaStatus(h, pStatus));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_EnableSpurCanceller()
******************************************************************************/
BERR_Code BAST_EnableSpurCanceller(
   BAST_ChannelHandle h,       /* [in] BAST channel handle */
   uint8_t            n,       /* [in] number of spurs to cancel (i.e. size of pConfig array) */
   BAST_SpurCancellerConfig *pConfig /* [in] array of notch filter settings */
)
{
   if (h->pDevice->settings.api.EnableSpurCanceller)
      return (h->pDevice->settings.api.EnableSpurCanceller(h, n, pConfig));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SetSearchRange()
******************************************************************************/
BERR_Code BAST_SetSearchRange(
   BAST_Handle h,           /* [in] BAST device handle */
   uint32_t searchRange   /* [in] search range in Hz */
)
{
   if (h->settings.api.SetSearchRange)
      return (h->settings.api.SetSearchRange(h, searchRange));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetSearchRange()
******************************************************************************/
BERR_Code BAST_GetSearchRange(
   BAST_Handle h,           /* [in] BAST device handle */
   uint32_t *pSearchRange   /* [in] search range in Hz */
)
{
   if (h->settings.api.GetSearchRange)
      return (h->settings.api.GetSearchRange(h, pSearchRange));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SetAmcScramblingSeq()
******************************************************************************/
BERR_Code BAST_SetAmcScramblingSeq(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint32_t xseed,       /* [in] physical layer scrambler seed */
   uint32_t plhdrscr1,   /* [in] PL Header Scrambling Sequence Bit[31:0] */
   uint32_t plhdrscr2,   /* [in] PL Header Scrambling Sequence Bit[63:32] */
   uint32_t plhdrscr3    /* [in] PL Header Scrambling Sequence Bit[89:64] */
)
{
   if (h->pDevice->settings.api.SetAmcScramblingSeq)
      return (h->pDevice->settings.api.SetAmcScramblingSeq(h, xseed, plhdrscr1, plhdrscr2, plhdrscr3));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetSignalDetectStatus()
******************************************************************************/
BERR_Code BAST_GetSignalDetectStatus(
   BAST_ChannelHandle      h,       /* [in] BAST channel handle */
   BAST_SignalDetectStatus *pStatus /* [out] returned status */
)
{
   if (h->pDevice->settings.api.GetSignalDetectStatus)
      return (h->pDevice->settings.api.GetSignalDetectStatus(h, pStatus));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SetTunerFilter()
******************************************************************************/
BERR_Code BAST_SetTunerFilter(
   BAST_ChannelHandle h,       /* [in] BAST channel handle */
   uint32_t           cutoffHz /* [in] filter cutoff in Hz */
)
{
   if (h->pDevice->settings.api.SetTunerFilter)
      return (h->pDevice->settings.api.SetTunerFilter(h, cutoffHz));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SetOutputTransportSettings()
******************************************************************************/
BERR_Code BAST_SetOutputTransportSettings(
   BAST_ChannelHandle h,                   /* [in] BAST channel handle */
   BAST_OutputTransportSettings *pSettings /* [in] transport settings */
)
{
   if (h->pDevice->settings.api.SetOutputTransportSettings)
      return (h->pDevice->settings.api.SetOutputTransportSettings(h, pSettings));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetOutputTransportSettings()
******************************************************************************/
BERR_Code BAST_GetOutputTransportSettings(
   BAST_ChannelHandle h,                   /* [in] BAST channel handle */
   BAST_OutputTransportSettings *pSettings /* [out] transport settings */
)
{
   if (h->pDevice->settings.api.GetOutputTransportSettings)
      return (h->pDevice->settings.api.GetOutputTransportSettings(h, pSettings));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SetDiseqcSettings()
******************************************************************************/
BERR_Code BAST_SetDiseqcSettings(
   BAST_ChannelHandle h,            /* [in] BAST channel handle */
   BAST_DiseqcSettings *pSettings   /* [in] diseqc settings */
)
{
   if (h->pDevice->settings.api.SetDiseqcSettings)
      return (h->pDevice->settings.api.SetDiseqcSettings(h, pSettings));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetDiseqcSettings()
******************************************************************************/
BERR_Code BAST_GetDiseqcSettings(
   BAST_ChannelHandle h,            /* [in] BAST channel handle */
   BAST_DiseqcSettings *pSettings   /* [out] diseqc settings */
)
{
   if (h->pDevice->settings.api.GetDiseqcSettings)
      return (h->pDevice->settings.api.GetDiseqcSettings(h, pSettings));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SetNetworkSpec()
******************************************************************************/
BERR_Code BAST_SetNetworkSpec(
   BAST_Handle      h,            /* [in] BAST channel handle */
   BAST_NetworkSpec networkSpec   /* [in] network spec */
)
{
   if (h->settings.api.SetNetworkSpec)
      return (h->settings.api.SetNetworkSpec(h, networkSpec));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetNetworkSpec()
******************************************************************************/
BERR_Code BAST_GetNetworkSpec(
   BAST_Handle      h,            /* [in] BAST channel handle */
   BAST_NetworkSpec *pNetworkSpec /* [out] network spec */
)
{
   if (h->settings.api.GetNetworkSpec)
      return (h->settings.api.GetNetworkSpec(h, pNetworkSpec));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SetNetworkSpec()
******************************************************************************/
BERR_Code BAST_SetFskChannel(
   BAST_Handle     h,               /* [in] BAST device handle */
   BAST_FskChannel fskTxChannel,    /* [in] channel for fsk transmit */
   BAST_FskChannel fskRxChannel     /* [in] channel for fsk receive */
)
{
   if (h->settings.api.SetFskChannel)
      return (h->settings.api.SetFskChannel(h, fskTxChannel, fskRxChannel));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetNetworkSpec()
******************************************************************************/
BERR_Code BAST_GetFskChannel(
   BAST_Handle     h,               /* [in] BAST device handle */
   BAST_FskChannel *fskTxChannel,   /* [out] channel for fsk transmit */
   BAST_FskChannel *fskRxChannel    /* [out] channel for fsk receive */
)
{
   if (h->settings.api.GetFskChannel)
      return (h->settings.api.GetFskChannel(h, fskTxChannel, fskRxChannel));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SetPeakScanSymbolRateRange()
******************************************************************************/  
BERR_Code BAST_SetPeakScanSymbolRateRange(
   BAST_ChannelHandle h,             /* [in] BAST channel handle */
   uint32_t           minSymbolRate, /* [in] minimum symbol rate in sym/sec */
   uint32_t           maxSymbolRate  /* [in] maximum symbol rate in sym/sec */
)
{
   if (h->pDevice->settings.api.SetPeakScanSymbolRateRange)
      return (h->pDevice->settings.api.SetPeakScanSymbolRateRange(h, minSymbolRate, maxSymbolRate));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_SetAdcSelect()
******************************************************************************/  
BERR_Code BAST_SetAdcSelect(BAST_ChannelHandle h, uint8_t adcSelect)
{
   if (h->pDevice->settings.api.SetAdcSelect)
      return (h->pDevice->settings.api.SetAdcSelect(h, adcSelect));
   else
      return BERR_NOT_SUPPORTED;   
}


/******************************************************************************
 BAST_GetAdcSelect()
******************************************************************************/  
BERR_Code BAST_GetAdcSelect(BAST_ChannelHandle h, uint8_t *pAdcSelect, uint8_t *pNumAdc)
{
   if (h->pDevice->settings.api.GetAdcSelect)
      return (h->pDevice->settings.api.GetAdcSelect(h, pAdcSelect, pNumAdc));
   else
      return BERR_NOT_SUPPORTED;   
}


/******************************************************************************
 BAST_GetPeakScanSymbolRateRange()
******************************************************************************/  
BERR_Code BAST_GetPeakScanSymbolRateRange(
   BAST_ChannelHandle h,               /* [in] BAST channel handle */
   uint32_t           *pMinSymbolRate, /* [out] minimum symbol rate in sym/sec */
   uint32_t           *pMaxSymbolRate  /* [out] maximum symbol rate in sym/sec */
)
{
   if (h->pDevice->settings.api.GetPeakScanSymbolRateRange)
      return (h->pDevice->settings.api.GetPeakScanSymbolRateRange(h, pMinSymbolRate, pMaxSymbolRate));
   else
      return BERR_NOT_SUPPORTED;
}


/******************************************************************************
 BAST_GetVersionInfo()
******************************************************************************/  
BERR_Code BAST_GetVersionInfo(
   BAST_Handle h,                   /* [in] BAST device handle */
   BFEC_VersionInfo *pVersion /* [out] version information */
)
{
   if (h->settings.api.GetVersionInfo)
      return (h->settings.api.GetVersionInfo(h, pVersion));
   else
      return BERR_NOT_SUPPORTED;   
}
#endif /* BAST_EXCLUDE_API_TABLE */


/******************************************************************************
 BAST_MultU32U32()
******************************************************************************/
void BAST_MultU32U32(uint32_t A, uint32_t B, uint32_t *P_hi, uint32_t *P_lo)
{
   uint32_t A_lo = A & 0xFFFF;
   uint32_t A_hi = (A >> 16) & 0xFFFF;
   uint32_t B_lo = B & 0xFFFF;
   uint32_t B_hi = (B >> 16) & 0xFFFF;
   uint32_t P, P0, P1, P2, P3, c;

   P = B_lo * A_lo;
   P0 = P & 0xFFFF;
   P1 = (P >> 16) & 0xFFFF;

   P = B_hi * A_hi;
   P2 = P & 0xFFFF;
   P3 = (P >> 16) & 0xFFFF;

   P = B_lo * A_hi;
   P1 += (P & 0xFFFF);
   P2 += ((P >> 16) & 0xFFFF);

   P = B_hi * A_lo;
   P1 += (P & 0xFFFF);
   P2 += ((P >> 16) & 0xFFFF);

   c = (P1 >> 16) & 0xFFFF;
   if (c)
   {
      P1 &= 0xFFFF;
      P2 += c;
   }

   c = (P2 >> 16) & 0xFFFF;
   if (c)
   {
      P2 &= 0xFFFF;
      P3 += c;
   }

   P3 &= 0xFFFF;
   *P_hi = P2 | (P3 << 16);
   *P_lo = P0 | (P1 << 16);
}


/******************************************************************************
 BAST_DivU64U32()
******************************************************************************/
void BAST_DivU64U32(uint32_t A_hi, uint32_t A_lo, uint32_t B, uint32_t *Q_hi, uint32_t *Q_lo)
{
   uint32_t X;
   int i;

   X = *Q_hi = *Q_lo = 0;
   for (i = 63; i >= 0; i--)
   {
      X = (X << 1);
      if (i >= 32)
      {
         *Q_hi = *Q_hi << 1;
         X |= ((A_hi & (1 << (i - 32))) ? 1 : 0);
      }
      else
      {
         *Q_lo = *Q_lo << 1;
         X |= ((A_lo & (1 << i)) ? 1 : 0);
      }

      if (X >= B)
      {
         if (i >= 32)
            *Q_hi |= 1;
         else
            *Q_lo |= 1;
         X -= B;
      }
   }   
}


