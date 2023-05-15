/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g3.c $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 10/22/12 5:08p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g3/common/bast_g3.c $
 * 
 * Hydra_Software_Devel/19   10/22/12 5:08p enavarro
 * SWSATFE-174: Read/Write AGC not supported when BAST_HAS_WFE is defined
 * 
 * Hydra_Software_Devel/18   10/22/12 4:31p enavarro
 * SWSATFE-174: removed unused functions/variables when BAST_HAS_WFE is
 * defined
 * 
 * Hydra_Software_Devel/17   10/15/12 10:02a enavarro
 * SWSATFE-220: add implementation for BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/16   8/1/12 10:30a enavarro
 * SWSATFE-174: added BAST_SetAdcSelect() and BAST_GetAdcSelect() to the
 * API for chips with multiple wideband tuners
 * 
 * Hydra_Software_Devel/15   8/24/11 10:54a enavarro
 * SWSATFE-140: fixed compiler errors when BAST_EXCLUDE_LDPC is defined
 * 
 * Hydra_Software_Devel/14   8/18/11 5:13p enavarro
 * SWSATFE-140: add build option to exclude ftm
 * 
 * Hydra_Software_Devel/13   8/11/11 1:52p enavarro
 * SWSATFE-86: add build options to exclude bcm3445 and mi2c support
 * 
 * Hydra_Software_Devel/12   3/7/11 8:34a enavarro
 * SWSATFE-86: fixed compiler warning
 * 
 * Hydra_Software_Devel/11   1/27/11 3:44p enavarro
 * SW7346-21: added BAST_g3_P_SetPeakScanSymbolRateRange(); added
 * BAST_g3_P_GetPeakScanSymbolRateRange()
 * 
 * Hydra_Software_Devel/10   1/19/11 3:21p ronchan
 * SWSATFE-99: added chip-independent fsk channel config api
 * 
 * Hydra_Software_Devel/9   1/11/11 4:55p ronchan
 * SWSATFE-75: added calls to 3445 functions
 * 
 * Hydra_Software_Devel/8   12/9/10 5:25p ronchan
 * SWSATFE-95: added functions to control tuner LNA
 * 
 * Hydra_Software_Devel/7   11/29/10 3:22p enavarro
 * SWSATFE-80: added BAST_GetNetworkSpec() and BAST_SetNetworkSpec() to
 * the API
 * 
 * Hydra_Software_Devel/6   9/30/10 3:14p ronchan
 * SWSATFE-80: added API functions to set/get diseqc settings
 * 
 * Hydra_Software_Devel/5   9/29/10 3:48p enavarro
 * SWSATFE-80: added API functions to set/get output transport settings
 * 
 * Hydra_Software_Devel/4   9/29/10 10:10a enavarro
 * SWSATFE-75: added BAST_GetSignalDetectStatus() to BAST API
 * 
 * Hydra_Software_Devel/3   9/21/10 10:27a enavarro
 * SWSATFE-75: fixed compiler warnings
 * 
 * Hydra_Software_Devel/2   9/17/10 2:52p enavarro
 * SWSATFE-80: added BAST_SetSearchRange(), BAST_GetSearchRange(),
 * BAST_SetTunerFilter(), and BAST_SetAmcScramblingSeq()
 * 
 * Hydra_Software_Devel/1   8/31/10 1:54p enavarro
 * SWSATFE-75: initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g3.h"
#include "bast_g3_priv.h"


static const BAST_Settings defDevSettings =
{
   {  /* host i2c settings */
      0x00, /* chipAddr */
      NULL, /* interruptEnableFunc */
      NULL  /* interruptEnableFuncParam */
   },
   { /* API function table */
      BAST_g3_P_Open,
      BAST_g3_P_Close,
      BAST_g3_P_GetTotalChannels,
      BAST_g3_GetChannelDefaultSettings,
      BAST_g3_P_OpenChannel,
      BAST_g3_P_CloseChannel,
      BAST_g3_P_GetDevice,
      BAST_g3_P_InitAp,
      BAST_g3_P_SoftReset,
      NULL, /* BAST_g3_P_GetApStatus, */
#ifdef BAST_HAS_LEAP
      NULL,
#else      
      BAST_g3_P_GetApVersion,
#endif      
      NULL, /* SetTmConfig, */
      NULL, /* GetTmConfig, */
      NULL, /* ConfigGpio */
      NULL, /* SetGpio */
      NULL, /* GetGpio */
      BAST_g3_P_TuneAcquire,
      BAST_g3_P_GetChannelStatus,
      BAST_g3_P_GetLockStatus,
      BAST_g3_P_ResetStatus,
      BAST_g3_P_SetDiseqcTone,
      BAST_g3_P_GetDiseqcTone,
      BAST_g3_P_SetDiseqcVoltage,
      BAST_g3_P_SendDiseqcCommand,
      BAST_g3_P_GetDiseqcStatus,
      BAST_g3_P_ResetDiseqc,
#ifndef BAST_EXCLUDE_FTM            
      BAST_g3_P_ResetFtm,
      BAST_g3_P_ReadFtm,
      BAST_g3_P_WriteFtm,
      BAST_g3_P_PowerDownFtm,
      BAST_g3_P_PowerUpFtm,
#else
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
#endif
#ifndef BAST_EXCLUDE_MI2C      
      BAST_g3_P_WriteMi2c,
      BAST_g3_P_ReadMi2c,
#else
      NULL,
      NULL,
#endif
      BAST_g3_P_GetSoftDecisionBuf,
#ifndef BAST_HAS_WFE      
      BAST_g3_P_ReadAgc,
      BAST_g3_P_WriteAgc,
      BAST_g3_P_FreezeAgc,
#else
      NULL,
      NULL,
      NULL,
#endif      
      BAST_g3_P_FreezeEq,
      BAST_g3_P_PowerDown,
      BAST_g3_P_PowerUp,
      BAST_g3_P_ReadRegister,
      BAST_g3_P_WriteRegister,
      BAST_g3_P_ReadConfig,
      BAST_g3_P_WriteConfig,
      NULL, /* GetInterruptEventHandle */
      BAST_g3_P_GetLockChangeEventHandle,
#ifndef BAST_EXCLUDE_FTM      
      BAST_g3_P_GetFtmEventHandle,
#else
      NULL,
#endif
      BAST_g3_P_GetDiseqcEventHandle,
      NULL, /*  HandleInterrupt_isr */
      NULL, /* ProcessInterruptEvent */
      BAST_g3_P_AbortAcq,
      NULL, /* BAST_g3_P_ConfigLna, */
      NULL, /* BAST_g3_P_GetLnaStatus, */
      NULL, /* ConfigAgc */
#ifndef BAST_EXCLUDE_ACW      
      BAST_g3_P_SendACW,
#else
      NULL,
#endif      
      BAST_g3_P_GetDiseqcVoltage,
      BAST_g3_P_GetDiseqcVsenseEventHandles,
      BAST_g3_P_EnableVsenseInterrupts,
      BAST_g3_P_PeakScan,
      BAST_g3_P_GetPeakScanStatus,
      BAST_g3_P_GetPeakScanEventHandle,
#ifndef BAST_EXCLUDE_STATUS_EVENTS            
      BAST_g3_P_EnableStatusInterrupts,
      BAST_g3_P_GetStatusEventHandle,
#else
      NULL,
      NULL,
#endif      
#ifndef BAST_EXCLUDE_BCM3445      
      BAST_g3_P_ConfigBcm3445,
      BAST_g3_P_MapBcm3445ToTuner,
      BAST_g3_P_GetBcm3445Status,
#else
      NULL,
      NULL,
      NULL,
#endif
      BAST_g3_P_EnableSpurCanceller,
      BAST_g3_P_ResetChannel,
      BAST_g3_P_EnableDiseqcLnb,
      BAST_g3_P_SetSearchRange,
      BAST_g3_P_GetSearchRange,
#ifndef BAST_EXCLUDE_LDPC      
      BAST_g3_P_SetAmcScramblingSeq,
#else
      NULL,
#endif
      BAST_g3_P_SetTunerFilter,
      BAST_g3_P_GetSignalDetectStatus,
      BAST_g3_P_SetOutputTransportSettings,
      BAST_g3_P_GetOutputTransportSettings,
      BAST_g3_P_SetDiseqcSettings,
      BAST_g3_P_GetDiseqcSettings,
      BAST_g3_P_SetNetworkSpec,
      BAST_g3_P_GetNetworkSpec,
#ifndef BAST_HAS_WFE
      BAST_g3_P_TunerConfigLna,
      BAST_g3_P_TunerGetLnaStatus,
#else
      NULL,
      NULL,
#endif      
      BAST_g3_P_SetFskChannel,
      BAST_g3_P_GetFskChannel,
      BAST_g3_P_SetPeakScanSymbolRateRange,
      BAST_g3_P_GetPeakScanSymbolRateRange,
#ifdef BAST_HAS_WFE
      BAST_g3_P_SetAdcSelect,
      BAST_g3_P_GetAdcSelect,
#else
      NULL,
      NULL,
#endif
#ifdef BAST_HAS_LEAP
      NULL,
#else      
      BAST_g3_P_GetVersionInfo,
#endif      
   },
   BAST_NetworkSpec_eDefault
};


static const BAST_ChannelSettings defChnSettings =
{
   (uint8_t)0
};


/******************************************************************************
 BAST_g3_GetDefaultSettings()
******************************************************************************/
BERR_Code BAST_g3_GetDefaultSettings(
   BAST_Settings *pDefSettings /* [out] default settings */
)
{
   *pDefSettings = defDevSettings;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g3_P_GetChannelDefaultSettings()
******************************************************************************/
BERR_Code BAST_g3_GetChannelDefaultSettings(
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

