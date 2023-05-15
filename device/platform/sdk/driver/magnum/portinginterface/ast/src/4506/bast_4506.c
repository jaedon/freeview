/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4506.c $
 * $brcm_Revision: Hydra_Software_Devel/30 $
 * $brcm_Date: 10/15/12 11:05a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/4506/bast_4506.c $
 * 
 * Hydra_Software_Devel/30   10/15/12 11:05a enavarro
 * SWSATFE-220: added BAST_GetVersionInfo()
 * 
 * Hydra_Software_Devel/29   8/1/12 10:35a enavarro
 * SWSATFE-174: updated API function table for BAST_SetAdcSelect() and
 * BAST_GetAdcSelect()
 * 
 * Hydra_Software_Devel/28   10/5/11 9:51a enavarro
 * SWSATFE-150: spur canceller not enabled in 4506 AST
 * 
 * Hydra_Software_Devel/27   9/12/11 2:41p enavarro
 * SWSATFE-146: added BAST_4506_GetNumTuners()
 * 
 * Hydra_Software_Devel/26   9/9/11 2:36p enavarro
 * SWSATFE-146: adjust LO divider in BAST_4506_CalculateTunerVco() per
 * TUNER_ANACTL10
 * 
 * Hydra_Software_Devel/25   9/8/11 3:06p enavarro
 * SWSATFE-146: moved functions related to VCO avoidance to bast_4506.c
 * and declare them in bast_4506.h so they can be used by the app
 * 
 * Hydra_Software_Devel/24   1/27/11 3:47p enavarro
 * SW7346-21: added BAST_SetPeakScanSymbolRateRange(); added
 * BAST_GetPeakScanSymbolRateRange()
 * 
 * Hydra_Software_Devel/23   1/20/11 5:16p ronchan
 * SWSATFE-99: calls to chip-independent fsk channel config
 * 
 * Hydra_Software_Devel/22   1/19/11 3:34p ronchan
 * SWSATFE-99: set api entries to null for fsk channel config
 * 
 * Hydra_Software_Devel/21   12/9/10 5:35p ronchan
 * SWSATFE-95: added placeholders for tuner LNA functions
 * 
 * Hydra_Software_Devel/20   11/29/10 4:12p enavarro
 * SWSATFE-80: implemented BAST_SetNetworkSpec() and BAST_GetNetworkSpec()
 * API functions
 * 
 * Hydra_Software_Devel/19   9/30/10 5:55p ronchan
 * SWSATFE-80: added API functions to set/get diseqc settings
 * 
 * Hydra_Software_Devel/18   9/29/10 4:10p enavarro
 * SWSATFE-80: added implementation functions for set/get output transport
 * settings
 * 
 * Hydra_Software_Devel/17   9/29/10 10:22a enavarro
 * SWSATFE-80: added implementation function for
 * BAST_GetSignalDetectStatus
 * 
 * Hydra_Software_Devel/16   9/17/10 11:42a enavarro
 * SWSATFE-80: added BAST_SetSearchRange(), BAST_GetSearchRange(),
 * BAST_SetTunerFilter(), and BAST_SetAmcScramblingSeq()
 * 
 * Hydra_Software_Devel/15   7/6/09 10:21a enavarro
 * PR 56603: added BAST_4506_P_ConfigAgc()
 * 
 * Hydra_Software_Devel/14   7/1/09 7:16p ronchan
 * PR 56509: add null entry for BAST_EnableDiseqcLnb()
 * 
 * Hydra_Software_Devel/13   5/26/09 9:23a enavarro
 * PR 55460: added newline at end of file to fix compiler error
 * 
 * Hydra_Software_Devel/12   5/5/09 3:41p ronchan
 * PR 54845: Added BAST_4506_WriteHostRegister() and
 * BAST_4506_ReadHostRegister()
 * 
 * Hydra_Software_Devel/11   10/20/08 3:49p ronchan
 * PR 32395: renamed channel reset
 * 
 * Hydra_Software_Devel/10   10/17/08 3:03p ronchan
 * PR 32395: added channel reset function
 * 
 * Hydra_Software_Devel/9   9/17/08 5:06p enavarro
 * PR 32395: added BAST_EnableSpurCanceller() to the API table
 * 
 * Hydra_Software_Devel/8   9/15/08 9:19a enavarro
 * PR 32395: add API functions to better manage external LNA
 * 
 * Hydra_Software_Devel/7   5/30/08 10:09a enavarro
 * PR 32395: added BAST_GetStatusEventHandle() to API function table
 * 
 * Hydra_Software_Devel/6   5/29/08 4:40p enavarro
 * PR 32395: added BAST_PeakScan(), BAST_GetPeakScanStatus(),
 * BAST_GetPeakScanEventHandle(), and BAST_EnableStatusInterrupts() to
 * API function table
 * 
 * Hydra_Software_Devel/5   4/28/08 3:30p ronchan
 * PR 32395: added vsense functions to api table
 * 
 * Hydra_Software_Devel/4   11/8/07 10:23a enavarro
 * PR 32395: changed BAST_NetworkSpec_eDirecTV to
 * BAST_NetworkSpec_eDefault
 * 
 * Hydra_Software_Devel/3   11/5/07 10:36a enavarro
 * PR 24793: removed bEnableInternalTuner in BAST_Settings struct; added
 * networkSpec to BAST_Settings struct
 * 
 * Hydra_Software_Devel/2   7/30/07 3:05p enavarro
 * PR 33579: Added BAST_GetLnaStatus()
 * 
 * Hydra_Software_Devel/1   6/21/07 3:39p enavarro
 * PR 32395: initial version
 *
 ***************************************************************************/
 
#include "bstd.h"
#include "bchp_4506.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_4506.h"
#include "bast_4506_priv.h"


BDBG_MODULE(bast_4506);


static const BAST_Settings defDevSettings =
{
   {  /* host i2c settings */
      0x69, /* chipAddr */
      NULL, /* interruptEnableFunc */
      NULL  /* interruptEnableFuncParam */
   },
   { /* API function table */
      BAST_4506_P_Open,
      BAST_4506_P_Close,
      BAST_4506_P_GetTotalChannels,
      BAST_4506_GetChannelDefaultSettings,
      BAST_4506_P_OpenChannel,
      BAST_4506_P_CloseChannel,
      BAST_4506_P_GetDevice,
      BAST_4506_P_InitAp,
      BAST_4506_P_SoftReset,
      BAST_4506_P_GetApStatus,
      BAST_4506_P_GetApVersion,
      NULL, /* BAST_SetTmConfig, */
      NULL, /* BAST_GetTmConfig, */
      BAST_4506_P_ConfigGpio,
      BAST_4506_P_SetGpio,
      BAST_4506_P_GetGpio,
      BAST_4506_P_TuneAcquire,
      BAST_4506_P_GetChannelStatus,
      BAST_4506_P_GetLockStatus,
      BAST_4506_P_ResetStatus,
      BAST_4506_P_SetDiseqcTone,
      BAST_4506_P_GetDiseqcTone,
      BAST_4506_P_SetDiseqcVoltage,
      BAST_4506_P_SendDiseqcCommand,
      BAST_4506_P_GetDiseqcStatus,
      BAST_4506_P_ResetDiseqc,
      BAST_4506_P_ResetFtm,
      BAST_4506_P_ReadFtm,
      BAST_4506_P_WriteFtm,
      BAST_4506_P_PowerDownFtm,
      BAST_4506_P_PowerUpFtm,
      BAST_4506_P_WriteMi2c,
      BAST_4506_P_ReadMi2c,
      BAST_4506_P_GetSoftDecisionBuf,
      BAST_4506_P_ReadAgc,
      BAST_4506_P_WriteAgc,
      BAST_4506_P_FreezeAgc,
      BAST_4506_P_FreezeEq,
      BAST_4506_P_PowerDown,
      BAST_4506_P_PowerUp,
      BAST_4506_P_ReadRegister,
      BAST_4506_P_WriteRegister,
      BAST_4506_P_ReadConfig,
      BAST_4506_P_WriteConfig,
      BAST_4506_P_GetInterruptEventHandle,
      BAST_4506_P_GetLockStateChangeEventHandle,
      BAST_4506_P_GetFtmEventHandle,
      BAST_4506_P_GetDiseqcEventHandle,
      BAST_4506_P_HandleInterrupt_isr,
      BAST_4506_P_ProcessInterruptEvent,
      BAST_4506_P_AbortAcq,
      BAST_4506_P_ConfigLna,
      BAST_4506_P_GetLnaStatus,
      BAST_4506_P_ConfigAgc,
      BAST_4506_P_SendACW,
      BAST_4506_P_GetDiseqcVoltage,
      BAST_4506_P_GetDiseqcVsenseEventHandles,
      BAST_4506_P_EnableVsenseInterrupts,
      BAST_4506_P_PeakScan,
      BAST_4506_P_GetPeakScanStatus,
      BAST_4506_P_GetPeakScanEventHandle,
      BAST_4506_P_EnableStatusInterrupts,
      BAST_4506_P_GetStatusEventHandle,
      BAST_4506_P_ConfigBcm3445,
      BAST_4506_P_MapBcm3445ToTuner,
      BAST_4506_P_GetBcm3445Status,
      NULL, /* BAST_EnableSpurCanceller */
      NULL, /* BAST_ResetChannel */
      NULL, /* BAST_EnableDiseqcLnb */
      BAST_4506_P_SetSearchRange,
      BAST_4506_P_GetSearchRange,
      BAST_4506_P_SetAmcScramblingSeq,
      BAST_4506_P_SetTunerFilter,
      BAST_4506_P_GetSignalDetectStatus,
      BAST_4506_P_SetOutputTransportSettings,
      BAST_4506_P_GetOutputTransportSettings,
      BAST_4506_P_SetDiseqcSettings,
      BAST_4506_P_GetDiseqcSettings,
      BAST_4506_P_SetNetworkSpec,
      BAST_4506_P_GetNetworkSpec,
      NULL, /* BAST_ConfigTunerLna */
      NULL, /* BAST_GetTunerLnaStatus */
      BAST_4506_P_SetFskChannel,
      BAST_4506_P_GetFskChannel,
      BAST_4506_P_SetPeakScanSymbolRateRange,
      BAST_4506_P_GetPeakScanSymbolRateRange, 
      NULL, /* BAST_SetAdcSelect */
      NULL, /* BAST_GetAdcSelect */
      NULL  /* BAST_GetVersionInfo */
   },
   BAST_NetworkSpec_eDefault
};


static const BAST_ChannelSettings defChnSettings =
{
   (uint8_t)0
};


/******************************************************************************
 BAST_4506_GetDefaultSettings()
******************************************************************************/
BERR_Code BAST_4506_GetDefaultSettings(
   BAST_Settings *pDefSettings /* [out] default settings */
)
{
   *pDefSettings = defDevSettings;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4506_P_GetChannelDefaultSettings()
******************************************************************************/
BERR_Code BAST_4506_GetChannelDefaultSettings(
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
 BAST_4506_WriteHostRegister()
******************************************************************************/
BERR_Code BAST_4506_WriteHostRegister(BAST_Handle h, uint8_t address, uint8_t *data)
{
   return (BAST_WriteHostRegister(h, address, data));
}


/******************************************************************************
 BAST_4506_ReadHostRegister()
******************************************************************************/
BERR_Code BAST_4506_ReadHostRegister(BAST_Handle h, uint8_t address, uint8_t *data)
{
   return (BAST_ReadHostRegister(h, address, data));
}


#ifdef BAST_4506_INCLUDE_VCO_AVOIDANCE
/******************************************************************************
 BAST_4506_GetNumTuners()
******************************************************************************/
BERR_Code BAST_4506_GetNumTuners(BAST_Handle h, uint8_t *nTuners)
{
   BAST_4506_P_Handle *p = (BAST_4506_P_Handle *)(h->pImpl);
   *nTuners = p->numTuners;
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_4506_GetTunerFddfs()
******************************************************************************/
BERR_Code BAST_4506_GetTunerFddfs(BAST_ChannelHandle h, uint32_t *pFddfs)
{
   BERR_Code retCode;
   BAST_ChannelHandle h0;
   uint32_t anactl23;

   h0 = (BAST_ChannelHandle)(h->pDevice->pChannels[0]);
   BAST_CHK_RETCODE(BAST_ReadRegister(h0, BCM4506_TUNER_ANACTL23, &anactl23));
   *pFddfs = ((BAST_4506_P_Handle*)h->pDevice->pImpl)->xtal * ((anactl23 & 0x7F) + 2);

   done:
   return retCode;
}


/******************************************************************************
 BAST_4506_CalculateTunerVco()
******************************************************************************/
BERR_Code BAST_4506_CalculateTunerVco(BAST_ChannelHandle h, uint32_t fcw, uint32_t *pVco)
{
   BERR_Code retCode;
   uint32_t anactl14, anactl10, Fddfs, div, P_hi, P_lo, Q_hi;

   BAST_CHK_RETCODE(BAST_ReadRegister(h, BCM4506_TUNER_ANACTL14, &anactl14));
   BAST_CHK_RETCODE(BAST_4506_GetTunerFddfs(h, &Fddfs));

   if (anactl14 & 1)
      div = 16;
   else
      div = 12;

   BAST_CHK_RETCODE(BAST_ReadRegister(h, BCM4506_TUNER_ANACTL10, &anactl10));
   if (anactl10 == 5)
      div = div >> 1; 
   else if (anactl10 == 4)
      div = div >> 2;
   else if (anactl10 == 6)
      div = div >> 3;

   /* vco = (Fddfs * fcw / 16777216.0) * div; */ 
   BAST_MultU32U32(Fddfs, fcw * div, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, 16777216, &Q_hi, pVco);

   done:
   return retCode;
}


/******************************************************************************
 BAST_4506_GetTunerActualVco()
******************************************************************************/
BERR_Code BAST_4506_GetTunerActualVco(BAST_ChannelHandle h, uint32_t *pVco)
{
   BERR_Code retCode;
   uint32_t mb2, fcw;

   BAST_CHK_RETCODE(BAST_ReadRegister(h, BCM4506_TUNER_MB2, &mb2));

   fcw = mb2 >> 8;
   retCode = BAST_4506_CalculateTunerVco(h, fcw, pVco);

   done:
   return retCode;
}


/******************************************************************************
 BAST_4506_GetTunerExpectedVco()
******************************************************************************/
BERR_Code BAST_4506_GetTunerExpectedVco(BAST_ChannelHandle h, uint32_t freq, uint32_t *pVco, uint32_t *pDiv)
{
   static uint32_t tuner_range[7] = {1700000000, 1130000000, 850000000, 565000000, 425000000, 283000000, 0};
   static uint8_t sel_mux[7] = {2, 2, 5, 5, 4, 4, 6};
   BERR_Code retCode;
   uint32_t fcw, P_hi, P_lo, Q_hi, Q_lo, anactl10, anactl14, Fddfs, lo_divider, i;

   *pDiv = 1;

   /* determine the anactl10[2:0]=sel_mux for the new freq */
   for (i = 0; i < 7; i++)
   {
      if (freq > tuner_range[i])
         break;
   }
   anactl10 = sel_mux[i];

   BAST_CHK_RETCODE(BAST_4506_GetTunerFddfs(h, &Fddfs));
   BAST_CHK_RETCODE(BAST_ReadRegister(h, BCM4506_TUNER_ANACTL14, &anactl14));

   if (anactl14 & 0x01)
      lo_divider = 16;
   else
      lo_divider = 12;
   if (anactl10 == 5)
   {
      *pDiv = 2;
      lo_divider = lo_divider >> 1; 
   }
   else if (anactl10 == 4)
   {
      *pDiv = 4;
      lo_divider = lo_divider >> 2;
   }
   else if (anactl10 == 6)
   {
      *pDiv = 8;
      lo_divider = lo_divider >> 3;
   }
   BAST_MultU32U32(freq, 16777216, &P_hi, &P_lo);
   BAST_DivU64U32(P_hi, P_lo, Fddfs, &Q_hi, &Q_lo);
   BAST_DivU64U32(Q_hi, Q_lo, lo_divider, &Q_hi, &fcw);

   retCode = BAST_4506_CalculateTunerVco(h, fcw, pVco);

   done:
   return retCode;
}


/******************************************************************************
 BAST_4506_GetClosestXtalHarmonic()
******************************************************************************/
uint32_t BAST_4506_GetClosestXtalHarmonic(uint32_t vco)
{
   uint32_t i, h1, h2;

   i = vco / 13500000;
   h1 = 13500000 * i;
   h2 = 13500000 * (i+1);
   if ((vco - h1) > (h2 - vco))
      i++;
   return (uint32_t)(13500000 * i);
}
#endif /* BAST_4506_INCLUDE_VCO_AVOIDANCE */
