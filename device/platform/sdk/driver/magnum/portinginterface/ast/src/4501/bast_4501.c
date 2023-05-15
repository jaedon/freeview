/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4501.c $
 * $brcm_Revision: Hydra_Software_Devel/35 $
 * $brcm_Date: 10/15/12 11:04a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/4501/bast_4501.c $
 * 
 * Hydra_Software_Devel/35   10/15/12 11:04a enavarro
 * SWSATFE-220: added BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/34   8/1/12 10:38a enavarro
 * SWSATFE-174: updated API function table for BAST_SetAdcSelect() and
 * BAST_GetAdcSelect()
 * 
 * Hydra_Software_Devel/33   1/27/11 3:56p enavarro
 * SW7346-21: updated API function table with NULL pointers for
 * SetPeakScanSymbolRateRange and GetPeakScanSymbolRateRange
 * 
 * Hydra_Software_Devel/32   1/19/11 3:34p ronchan
 * SWSATFE-99: set api entries to null for fsk channel config
 * 
 * Hydra_Software_Devel/31   12/9/10 5:36p ronchan
 * SWSATFE-95: added placeholders for tuner LNA functions
 * 
 * Hydra_Software_Devel/30   11/29/10 4:35p enavarro
 * SWSATFE-80: fixed compile warning
 * 
 * Hydra_Software_Devel/29   11/29/10 4:32p enavarro
 * SWSATFE-80: implemented BAST_SetNetworkSpec() and BAST_GetNetworkSpec()
 * API functions
 * 
 * Hydra_Software_Devel/28   9/30/10 6:17p ronchan
 * SWSATFE-80: added API functions to set/get diseqc settings
 * 
 * Hydra_Software_Devel/27   9/29/10 4:21p enavarro
 * SWSATFE-80: added implementation functions for set/get output transport
 * settings
 * 
 * Hydra_Software_Devel/26   9/29/10 10:21a enavarro
 * SWSATFE-80: added NULL pointer for BAST_GetSignalDetectStatus function
 * in API function table
 * 
 * Hydra_Software_Devel/25   9/17/10 11:50a enavarro
 * SWSATFE-80: added BAST_SetSearchRange(), BAST_GetSearchRange(),
 * BAST_SetTunerFilter(), and BAST_SetAmcScramblingSeq()
 * 
 * Hydra_Software_Devel/24   7/1/09 7:18p ronchan
 * PR 56509: add null entry for BAST_EnableDiseqcLnb
 * 
 * Hydra_Software_Devel/23   10/20/08 3:50p ronchan
 * PR 24793: renamed channel reset
 * 
 * Hydra_Software_Devel/22   10/17/08 3:07p ronchan
 * PR 24793: added channel reset entry to api table
 * 
 * Hydra_Software_Devel/21   9/17/08 5:12p enavarro
 * PR 24793: added entry for BAST_EnableSpurCanceller in the API table
 * 
 * Hydra_Software_Devel/20   9/15/08 9:26a enavarro
 * PR 24793: updated API function table for new BCM3445 functions
 * 
 * Hydra_Software_Devel/19   8/11/08 3:43p ronchan
 * PR 24793: fixed compiler warning
 * 
 * Hydra_Software_Devel/18   7/16/08 4:54p enavarro
 * PR 24793: added BAST_4501_WriteHostRegister() and
 * BAST_4501_ReadHostRegister()
 * 
 * Hydra_Software_Devel/17   5/30/08 10:25a enavarro
 * PR 24793: updated API function table
 * 
 * Hydra_Software_Devel/16   5/29/08 5:07p enavarro
 * PR 24793: updated API function table
 * 
 * Hydra_Software_Devel/15   4/28/08 3:11p ronchan
 * PR 18135: updated api function table
 * 
 * Hydra_Software_Devel/14   11/8/07 10:28a enavarro
 * PR 24793: added BAST_NetworkSpec to BAST_Settings struct
 * 
 * Hydra_Software_Devel/13   7/27/07 4:10p enavarro
 * PR 33579: added BAST_GetLnaStatus()
 * 
 * Hydra_Software_Devel/12   6/22/07 10:05a enavarro
 * PR 32395: added a NULL pointer for BAST_SendACW to the API function
 * table
 * 
 * Hydra_Software_Devel/11   11/7/06 2:35p enavarro
 * PR 24793: added BAST_ConfigAgc()
 * 
 * Hydra_Software_Devel/10   11/2/06 4:57p enavarro
 * PR 25473: add LNA support
 * 
 * Hydra_Software_Devel/9   8/10/06 9:09a enavarro
 * PR 18135: make BAST_SendDiseqcCommand() non-blocking; added
 * BAST_GetDiseqcEventHandle() and BAST_GetDiseqcStatus()
 * 
 * Hydra_Software_Devel/8   8/2/06 2:34p enavarro
 * PR 18135: added BAST_AbortAcq()
 * 
 * Hydra_Software_Devel/7   6/5/06 5:36p enavarro
 * PR 18135: fixed compiler warnings
 * 
 * Hydra_Software_Devel/6   4/11/06 4:50p enavarro
 * PR 18135: added power up/down functions
 * 
 * Hydra_Software_Devel/5   2/27/06 9:12a enavarro
 * PR 18135: added BAST_4501_P_GetLockStatus()
 * 
 * Hydra_Software_Devel/4   12/22/05 11:11a enavarro
 * PR 18135: replaced BAST_Tune() and BAST_Acquire() with
 * BAST_TuneAcquire()
 * 
 * Hydra_Software_Devel/3   12/21/05 10:32a enavarro
 * PR 18135: set default i2c address to 0x69
 * 
 * Hydra_Software_Devel/2   11/23/05 11:21a enavarro
 * PR 18135: added BAST_4501_P_ConfigGpio() and BAST_4501_P_GetGpio()
 * 
 * Hydra_Software_Devel/1   11/14/05 10:30a enavarro
 * PR 18135: initial version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bchp_4501.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_4501.h"
#include "bast_4501_priv.h"


BDBG_MODULE(bast_4501);


static const BAST_Settings defDevSettings =
{
   {  /* host i2c settings */
      0x69, /* chipAddr */
      NULL, /* interruptEnableFunc */
      NULL  /* interruptEnableFuncParam */
   },
   { /* API function table */
      BAST_4501_P_Open,
      BAST_4501_P_Close,
      BAST_4501_P_GetTotalChannels,
      BAST_4501_GetChannelDefaultSettings,
      BAST_4501_P_OpenChannel,
      BAST_4501_P_CloseChannel,
      BAST_4501_P_GetDevice,
      BAST_4501_P_InitAp,
      BAST_4501_P_SoftReset,
      BAST_4501_P_GetApStatus,
      BAST_4501_P_GetApVersion,
      BAST_4501_P_SetTmConfig,
      BAST_4501_P_GetTmConfig,
      BAST_4501_P_ConfigGpio,
      BAST_4501_P_SetGpio,
      BAST_4501_P_GetGpio,
      BAST_4501_P_TuneAcquire,
      BAST_4501_P_GetChannelStatus,
      BAST_4501_P_GetLockStatus,
      BAST_4501_P_ResetStatus,
      BAST_4501_P_SetDiseqcTone,
      BAST_4501_P_GetDiseqcTone,
      BAST_4501_P_SetDiseqcVoltage,
      BAST_4501_P_SendDiseqcCommand,
      BAST_4501_P_GetDiseqcStatus,
      BAST_4501_P_ResetDiseqc,
      BAST_4501_P_ResetFtm,
      BAST_4501_P_ReadFtm,
      BAST_4501_P_WriteFtm,
      BAST_4501_P_PowerDownFtm,
      BAST_4501_P_PowerUpFtm,
      BAST_4501_P_WriteMi2c,
      BAST_4501_P_ReadMi2c,
      BAST_4501_P_GetSoftDecisionBuf,
      BAST_4501_P_ReadAgc,
      BAST_4501_P_WriteAgc,
      BAST_4501_P_FreezeAgc,
      BAST_4501_P_FreezeEq,
      BAST_4501_P_PowerDown,
      BAST_4501_P_PowerUp,
      BAST_4501_P_ReadRegister,
      BAST_4501_P_WriteRegister,
      BAST_4501_P_ReadConfig,
      BAST_4501_P_WriteConfig,
      BAST_4501_P_GetInterruptEventHandle,
      BAST_4501_P_GetLockStateChangeEventHandle,
      BAST_4501_P_GetFtmEventHandle,
      BAST_4501_P_GetDiseqcEventHandle,
      BAST_4501_P_HandleInterrupt_isr,
      BAST_4501_P_ProcessInterruptEvent,
      BAST_4501_P_AbortAcq,
      BAST_4501_P_ConfigLna,
      BAST_4501_P_GetLnaStatus,
      BAST_4501_P_ConfigAgc,
      NULL, /* BAST_SendACW not available on BCM4501 */
      NULL, /* BAST_GetDiseqcVoltage not implemented */
      NULL, /* BAST_GetDiseqcVsenseEventHandles not implemented */
      NULL, /* BAST_EnableVsenseInterrupts not implemented */
      NULL, /* BAST_PeakScan not supported */
      NULL, /* BAST_GetPeakScanStatus not supported */
      NULL, /* BAST_GetPeakScanEventHandle not supported */
      NULL, /* BAST_EnableStatusInterrupts not supported */
      NULL, /* BAST_GetStatusEventHandle not supported */
      NULL, /* BAST_ConfigBcm3445 not supported */
      NULL, /* BAST_MapBcm3445ToTuner not supported */
      NULL, /* BAST_GetBcm3445Status not supported */
      NULL, /* BAST_EnableSpurCanceller not supported */
      NULL, /* BAST_ResetChannel not supported */
      NULL, /* BAST_EnableDiseqcLnb not supported */
      BAST_4501_P_SetSearchRange,
      BAST_4501_P_GetSearchRange,
      BAST_4501_P_SetAmcScramblingSeq,
      BAST_4501_P_SetTunerFilter,
      NULL,  /* BAST_GetSignalDetectStatus not supported */
      BAST_4501_P_SetOutputTransportSettings,
      BAST_4501_P_GetOutputTransportSettings,
      BAST_4501_P_SetDiseqcSettings,
      BAST_4501_P_GetDiseqcSettings,
      BAST_4501_P_SetNetworkSpec,
      BAST_4501_P_GetNetworkSpec,
      NULL, /* BAST_ConfigTunerLna */
      NULL, /* BAST_GetTunerLnaStatus */
      NULL, /* BAST_SetFskChannel */
      NULL, /* BAST_GetFskChannel */
      NULL, /* BAST_SetPeakScanSymbolRateRange */
      NULL, /* BAST_GetPeakScanSymbolRateRange */
      NULL, /* BAST_SetAdcSelect */
      NULL, /* BAST_GetAdcSelect */
      NULL, /* BAST_GetVersionInfo */
   },
   BAST_NetworkSpec_eDefault /* enable internal tuner */
};


static const BAST_ChannelSettings defChnSettings =
{
   (uint8_t)0 /* use internal tuner */
};


/******************************************************************************
 BAST_4501_GetDefaultSettings()
******************************************************************************/
BERR_Code BAST_4501_GetDefaultSettings(
   BAST_Settings *pDefSettings /* [out] default settings */
)
{
   *pDefSettings = defDevSettings;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4501_P_GetChannelDefaultSettings()
******************************************************************************/
BERR_Code BAST_4501_GetChannelDefaultSettings(
   BAST_Handle   h,                      /* [in] BAST handle */
   uint32_t      chnNo,                      /* [in] channel number */
   BAST_ChannelSettings *pChnDefSettings /* [out] default channel settings */
)
{
   BSTD_UNUSED(h);
   BSTD_UNUSED(chnNo);
   *pChnDefSettings = defChnSettings;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4501_WriteHostRegister()
******************************************************************************/
BERR_Code BAST_4501_WriteHostRegister(BAST_Handle h, uint8_t address, uint8_t *data)
{
   return (BAST_WriteHostRegister(h, address, data));
}


/******************************************************************************
 BAST_4501_ReadHostRegister()
******************************************************************************/
BERR_Code BAST_4501_ReadHostRegister(BAST_Handle h, uint8_t address, uint8_t *data)
{
   return (BAST_ReadHostRegister(h, address, data));
}

