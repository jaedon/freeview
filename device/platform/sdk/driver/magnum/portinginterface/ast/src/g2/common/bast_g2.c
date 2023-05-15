/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g2.c $
 * $brcm_Revision: Hydra_Software_Devel/25 $
 * $brcm_Date: 10/15/12 4:07p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/common/bast_g2.c $
 * 
 * Hydra_Software_Devel/25   10/15/12 4:07p enavarro
 * SWSATFE-220: add implementation for BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/24   10/15/12 11:03a enavarro
 * SWSATFE-220: added BAST_GetVersionInfo() to API table
 * 
 * Hydra_Software_Devel/23   9/6/12 6:11p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/22   8/1/12 10:33a enavarro
 * SWSATFE-174: updated API function table for BAST_SetAdcSelect() and
 * BAST_GetAdcSelect()
 * 
 * Hydra_Software_Devel/21   10/5/11 9:46a enavarro
 * SWSATFE-150: spur canceller not supported in 73xx AST
 * 
 * Hydra_Software_Devel/20   1/31/11 5:20p ronchan
 * SWSATFE-99: added implementation for BAST_SetFskConfig() and
 * BAST_GetFskConfig()
 * 
 * Hydra_Software_Devel/19   1/27/11 4:17p enavarro
 * SW7346-21: added implementation for BAST_SetPeakScanSymbolRateRange()
 * and BAST_GetPeakScanSymbolRateRange()
 * 
 * Hydra_Software_Devel/18   1/19/11 3:34p ronchan
 * SWSATFE-99: set api entries to null for fsk channel config
 * 
 * Hydra_Software_Devel/17   12/9/10 5:36p ronchan
 * SWSATFE-95: added placeholders for tuner LNA functions
 * 
 * Hydra_Software_Devel/16   11/30/10 9:36a ckpark
 * SWSATFE-80: implemented BAST_SetNetworkSpec() and BAST_GetNetworkSpec()
 * 
 * Hydra_Software_Devel/15   9/30/10 3:16p ronchan
 * SWSATFE-80: added API functions to set/get diseqc settings
 * 
 * Hydra_Software_Devel/14   9/30/10 9:07a enavarro
 * SWSATFE-80: added implementation functions for
 * SetOutputTransportSettings() and GetOutputTransportSettings()
 * 
 * Hydra_Software_Devel/13   9/29/10 10:25a ronchan
 * SWSATFE-75: added null ptr to api table for GetSignalDetectStatus
 * 
 * Hydra_Software_Devel/12   9/17/10 1:51p enavarro
 * SWSATFE-80: added BAST_SetSearchRange(), BAST_GetSearchRange(),
 * BAST_SetTunerFilter(), and BAST_SetAmcScramblingSeq()
 * 
 * Hydra_Software_Devel/11   7/1/09 7:12p ronchan
 * PR 56509: added new function BAST_EnableDiseqcLnb()
 * 
 * Hydra_Software_Devel/10   10/20/08 3:32p ronchan
 * PR 37769: renamed channel reset
 * 
 * Hydra_Software_Devel/9   10/17/08 2:56p ronchan
 * PR 37769: added channel reset function
 * 
 * Hydra_Software_Devel/8   10/3/08 10:09a enavarro
 * PR 37769: implemented EnableSpurCanceller API function
 * 
 * Hydra_Software_Devel/7   9/17/08 5:15p enavarro
 * PR 37769: updated API function table
 * 
 * Hydra_Software_Devel/6   9/17/08 9:12a enavarro
 * PR 37769: added pointers to API function table for BAST_ConfigBcm3445,
 * BAST_MapBcm3445ToTuner, and BAST_GetBcm3445Status
 * 
 * Hydra_Software_Devel/5   9/15/08 9:33a enavarro
 * PR 37769: updated API function table
 * 
 * Hydra_Software_Devel/4   5/30/08 10:19a enavarro
 * PR 37769: updated API function table
 * 
 * Hydra_Software_Devel/3   4/28/08 2:21p ronchan
 * PR 37881: added vsense functions
 * 
 * Hydra_Software_Devel/2   11/30/07 10:12a ronchan
 * PR 37769: enabled api function GetLockChangeEventHandle
 * 
 * Hydra_Software_Devel/1   11/21/07 11:18a ronchan
 * PR 37769: initial version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2.h"
#include "bast_g2_priv.h"


BDBG_MODULE(bast_g2);


static const BAST_Settings defDevSettings =
{
   {  /* host i2c settings */
      0x00, /* chipAddr */
      NULL, /* interruptEnableFunc */
      NULL  /* interruptEnableFuncParam */
   },
   { /* API function table */
      BAST_g2_P_Open,
      BAST_g2_P_Close,
      BAST_g2_P_GetTotalChannels,
      BAST_g2_GetChannelDefaultSettings,
      BAST_g2_P_OpenChannel,
      BAST_g2_P_CloseChannel,
      BAST_g2_P_GetDevice,
      BAST_g2_P_InitAp,
      BAST_g2_P_SoftReset,
      BAST_g2_P_GetApStatus,
      BAST_g2_P_GetApVersion,
      NULL, /* BAST_SetTmConfig, */
      NULL, /* BAST_GetTmConfig, */
      NULL, /* BAST_ConfigGpio */
      NULL, /* BAST_SetGpio */
      NULL, /* BAST_GetGpio */
      BAST_g2_P_TuneAcquire,
      BAST_g2_P_GetChannelStatus,
      BAST_g2_P_GetLockStatus,
      BAST_g2_P_ResetStatus,
      BAST_g2_P_SetDiseqcTone,
      BAST_g2_P_GetDiseqcTone,
      BAST_g2_P_SetDiseqcVoltage,
      BAST_g2_P_SendDiseqcCommand,
      BAST_g2_P_GetDiseqcStatus,
      BAST_g2_P_ResetDiseqc,
      BAST_g2_P_ResetFtm,
      BAST_g2_P_ReadFtm,
      BAST_g2_P_WriteFtm,
      BAST_g2_P_PowerDownFtm,
      BAST_g2_P_PowerUpFtm,
      BAST_g2_P_WriteMi2c,
      BAST_g2_P_ReadMi2c,
      BAST_g2_P_GetSoftDecisionBuf,
      BAST_g2_P_ReadAgc,
      BAST_g2_P_WriteAgc,
      BAST_g2_P_FreezeAgc,
      BAST_g2_P_FreezeEq,
      BAST_g2_P_PowerDown,
      BAST_g2_P_PowerUp,
      BAST_g2_P_ReadRegister,
      BAST_g2_P_WriteRegister,
      BAST_g2_P_ReadConfig,
      BAST_g2_P_WriteConfig,
      NULL, /* BAST_GetInterruptEventHandle */
      BAST_g2_P_GetLockChangeEventHandle,
      BAST_g2_P_GetFtmEventHandle,
      BAST_g2_P_GetDiseqcEventHandle,
      NULL, /*  BAST_HandleInterrupt_isr */
      NULL, /* BAST_ProcessInterruptEvent */
      BAST_g2_P_AbortAcq,
      BAST_g2_P_ConfigLna,
      BAST_g2_P_GetLnaStatus,
      NULL, /* BAST_ConfigAgc */
      BAST_g2_P_SendACW,
      BAST_g2_P_GetDiseqcVoltage,
      BAST_g2_P_GetDiseqcVsenseEventHandles,
      BAST_g2_P_EnableVsenseInterrupts,
      BAST_g2_P_PeakScan,
      BAST_g2_P_GetPeakScanStatus,
      BAST_g2_P_GetPeakScanEventHandle,
      BAST_g2_P_EnableStatusInterrupts,
      BAST_g2_P_GetStatusEventHandle,
      BAST_g2_P_ConfigBcm3445, 
      BAST_g2_P_MapBcm3445ToTuner,
      BAST_g2_P_GetBcm3445Status,
      NULL, /* BAST_EnableSpurCanceller */
      BAST_g2_P_ResetChannel,
      BAST_g2_P_EnableDiseqcLnb,
      BAST_g2_P_SetSearchRange,
      BAST_g2_P_GetSearchRange,
      BAST_g2_P_SetAmcScramblingSeq,
      BAST_g2_P_SetTunerFilter,
      NULL,  /* BAST_GetSignalDetectStatus */
      BAST_g2_P_SetOutputTransportSettings,
      BAST_g2_P_GetOutputTransportSettings,
      BAST_g2_P_SetDiseqcSettings,
      BAST_g2_P_GetDiseqcSettings,
      BAST_g2_P_SetNetworkSpec,
      BAST_g2_P_GetNetworkSpec,
      NULL, /* BAST_ConfigTunerLna */
      NULL, /* BAST_GetTunerLnaStatus */
      BAST_g2_P_SetFskChannel,
      BAST_g2_P_GetFskChannel,
      BAST_g2_P_SetPeakScanSymbolRateRange,
      BAST_g2_P_GetPeakScanSymbolRateRange, 
      NULL, /* BAST_SetAdcSelect */
      NULL, /* BAST_GetAdcSelect */
      BAST_g2_P_GetVersionInfo
   },
   BAST_NetworkSpec_eDefault
};


static const BAST_ChannelSettings defChnSettings =
{
   (uint8_t)0
};


/******************************************************************************
 BAST_g2_GetDefaultSettings()
******************************************************************************/
BERR_Code BAST_g2_GetDefaultSettings(
   BAST_Settings *pDefSettings /* [out] default settings */
)
{
   *pDefSettings = defDevSettings;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_P_GetChannelDefaultSettings()
******************************************************************************/
BERR_Code BAST_g2_GetChannelDefaultSettings(
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

