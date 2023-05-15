/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvsb_3548.c $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 4/6/10 10:15a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vsb/3548/bvsb_3548.c $
 * 
 * Hydra_Software_Devel/15   4/6/10 10:15a dliu
 * SW3548-2803: Allow app to set agc pins to delta sigma mode after close
 * 
 * Hydra_Software_Devel/14   3/3/10 12:15p dliu
 * SW3548-2803: Add option to change delta sigma to open drain mode
 * 
 * Hydra_Software_Devel/13   5/19/09 11:05a dliu
 * PR55183: Add OOB lock interrupt
 * 
 * Hydra_Software_Devel/12   3/24/09 3:45p dliu
 * PR53219: Add OOB control for 3520
 * 
 * Hydra_Software_Devel/11   11/20/08 11:01p dliu
 * PR47770: Change default PGA gain
 * 
 * Hydra_Software_Devel/10   11/12/08 6:06p dliu
 * PR47770: prepend e in front of the enum
 * 
 * Hydra_Software_Devel/9   11/12/08 5:25p dliu
 * PR47770: Add interface for PGA gain
 * 
 * Hydra_Software_Devel/8   10/23/08 3:52p dliu
 * PR48016: Move IFD code to MIPS
 * 
 * Hydra_Software_Devel/7   10/20/08 11:49a dliu
 * PR47770:fix compile issue.
 * PR47770:Add PGA interface
 * 
 * Hydra_Software_Devel/PR47770/2   10/16/08 6:37p jerryy
 * PR47770:fix compile issue.
 * 
 * Hydra_Software_Devel/PR47770/1   10/10/08 7:37p jerryy
 * PR47770:Add PGA interface
 * 
 * Hydra_Software_Devel/6   9/29/08 4:51p dliu
 * PR46516: Add dynamic power management
 * 
 * Hydra_Software_Devel/5   5/8/08 10:33a dliu
 * PR38953: Change interrupt scheme to use BINT
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bvsb.h"
#include "bvsb_priv.h"
#include "bvsb_3548.h"
#include "bvsb_3548_priv.h"

BDBG_MODULE(bvsb_3548);

static const BVSB_Settings defDevSettings =
{
   {  /* i2c settings */
      0x00, /* chipAddr */
      NULL, /* interruptEnableFunc */
      NULL  /* interruptEnableFuncParam */
   },
   {  /* VSB acquisition settings */
      BVSB_PhaseLoopBw_eMedium,  /* BW */
      true,  /* bAutoAcq */
      true,  /* bFastAcq */
      true,  /* bTei */
      true,  /* bTerr */
      false, /* bNtscSweep */
      false, /* bRfiSweep */
      BVSB_PgaGain_e6dB
   },
   {  /* QAM acquisition settings */
      BVSB_PhaseLoopBw_eMedium, 
      BVSB_IDepth_e12_17,  /* idepth */
      BVSB_NyquistFilter_e12, /* nyquist filter rolloff */
      true,  /* bAutoAcq */
      true,  /* bFastAcq */
      true,  /* bTerr */
      true,  /* bEq */
      false, /* use 6MHz channelization */
      true,  /* bTei */
      true,  /* bSpinv */
      false,  /* select DVB unmapper */
      BVSB_PgaGain_e6dB
   },
   {  /* NTSC acquisition settings */
      BVSB_PhaseLoopBw_eMedium, 
      true,  /* bFastAcq */
      true,  /* bTerr */
      true,  /* bAutoAcq */
	  BVSB_PullInRange_eWide,
	  45750000, /* will need this when we integrate 3563 changes */
	  BVSB_AnalogMode_eNTSC,
      BVSB_BtscCountry_eUS,
      BVSB_PgaGain_e6dB
#ifdef NOT_USED
      100, /* delay */
      0xA, /* RF Bandwidth */
      0x2, /* IF Bandwidth */
      0x5A80, /* TOP Bandwidth */
      false /* bSpectrumInvert */
#endif
   },
   {  /* BTSC settings - not used in 3548 BVSB module */
      BVSB_BtscDecodeMode_eStereo,  /* decode mode */
      BVSB_BtscSampleRate_e48KHz,   /* sample rate */
      true                          /* primary i2s output */
   },
   {  /* OOB acquisition settings */
      BVSB_PhaseLoopBw_eMedium, 
      true,  /* bAutoAcq */
      true,  /* bSpinv */
      true  /* bBypassFEC */ 
   },
   {  /* Inband Transport output interface settings */
      false, /* bHead4 */
      false,  /* bSync1 */
      false, /* bXBERT */
      false, /* bErrinv */
      false, /* bSyncinv */
      false, /* bVldinv */
      false, /* bClksup */
      true,  /* bClkinv */
      false   /* bSerial */
   },
   {  /* set threshold for signal detection */
      BVSB_P_UNCHANGED_THRESHOLD,
      BVSB_P_UNCHANGED_THRESHOLD,
      BVSB_P_UNCHANGED_THRESHOLD
   },
   { /* API function table */
      BVSB_3548_P_Open,
      BVSB_3548_P_Close,
      BVSB_3548_P_InitAp,
      BVSB_3548_P_GetApStatus,
      BVSB_3548_P_GetApVersion,
      BVSB_3548_P_ReadRegister,
      BVSB_3548_P_WriteRegister,
      NULL,
      NULL,
      BVSB_3548_P_AcquireInband,
      BVSB_3548_P_GetVsbStatus,
      BVSB_3548_P_GetQamStatus,
      BVSB_3548_P_GetNtscStatus,
      BVSB_3548_P_ResetInbandStatus,
      NULL, /*BVSB_3548_P_AcquireOob*/
      NULL, /*BVSB_3548_P_GetOobStatus, */
      NULL, /*BVSB_3548_P_ResetOobStatus,*/
      NULL,
      NULL,
      BVSB_3548_P_GetBtscStatus,
      BVSB_3548_P_SetInbandOi,
      BVSB_3548_P_GetSoftDecisionBuf,
      NULL, /*BVSB_3548_P_GetOobSoftDecisionBuf,*/
      NULL, /*BVSB_3548_P_SetTmConfig */
      NULL, /*BVSB_3548_P_GetTmConfig, */
      BVSB_3548_P_WriteConfig,
      BVSB_3548_P_ReadConfig,
      BVSB_3548_P_GetLockStateChangeEventHandle,
      NULL, /* GetOobLockStateChangeEventHandle */
      NULL,
      NULL,
      NULL, /* BVSB_3548_P_HandleInterrupt_isr,*/
      NULL,
      NULL,
      BVSB_3548_P_DetectChannelSignal,
      BVSB_3548_P_GetUnlockstatusEventHandle,
      BVSB_3548_P_GetLockstatusEventHandle,
      BVSB_3548_P_SetInbandIfFreq,
      BVSB_3548_P_SetIfdPullInRange,
      BVSB_3548_P_PowerDown,
      BVSB_3548_P_SetPgaGain,
      NULL /* BVSB_SetOobInterfaceControl */
#ifdef NEW_BVSB_SETTINGS
      ,
      BVSB_3548_P_SetQamSettings,
	  BVSB_3548_P_SetIfdPixCarrier
#endif
   },
   NULL, /* BTMR_Handle */
   false, /* bRfDeltaSigmaInvert */
   false, /* bIfDeltaSigmaInvert */
   BVSB_AgcPinMode_e25V,
   false, /* bIfAgcZero */
   false, /* bRfAgcZero */
   false, /* bRfDeltaSigmaOpenDrain */
   false, /* bIfDeltaSigmaOpenDrain */
   false /* bOpenDrainPinsOnClose */
};


/******************************************************************************
 BVSB_3548_GetDefaultSettings()
******************************************************************************/
BERR_Code BVSB_3548_GetDefaultSettings(
   BVSB_Settings *pDefSettings /* [out] default settings */
)
{
   *pDefSettings = defDevSettings;
   return BERR_SUCCESS;
}


