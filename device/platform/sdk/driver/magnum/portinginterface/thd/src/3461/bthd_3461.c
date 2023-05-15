/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthd_3461.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 10/18/12 5:16p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/thd/src/3461/bthd_3461.c $
 * 
 * Hydra_Software_Devel/12   10/18/12 5:16p atanugul
 * SW3461-269: Merge to main
 * 
 * Hydra_Software_Devel/SW3461-252/1   9/28/12 5:03p atanugul
 * SW3461-269: Add support for BTHD_GetVersionInfo()
 * 
 * Hydra_Software_Devel/11   8/3/12 3:07p atanugul
 * SW3461-229: Send receiver configuration HAB command for FW version 5.0
 * or newer
 * 
 * Hydra_Software_Devel/10   5/23/12 6:04p atanugul
 * SW3461-212: Add TS out to THD and TC2 acquireparameters
 * 
 * Hydra_Software_Devel/9   2/6/12 12:39p atanugul
 * SW3461-141: set default acquisitionMode to Auto
 * 
 * Hydra_Software_Devel/7   2/2/12 6:23p atanugul
 * SW3461-141: Implement new apis for SetAcquireParams, GetAcquireParams
 * and PowerUp
 * 
 * Hydra_Software_Devel/6   1/19/12 5:00p atanugul
 * SW3461-135: Add auto acquire on tune support for 3461
 * 
 * Hydra_Software_Devel/5   8/18/11 4:36p atanugul
 * SW3461-3: Update BTHD_3461_GetDefaultInbandParams()
 * 
 * Hydra_Software_Devel/4   8/17/11 3:29p atanugul
 * SW3461-36: Implement BTHD_GetLockStatus()
 * 
 * Hydra_Software_Devel/3   6/10/11 2:55p farshidf
 * SWDTV-7504: update the interface for other chip
 * 
 * Hydra_Software_Devel/2   6/1/11 4:01p atanugul
 * SW3461-5: Add THD support for 3461
 * 
 * Hydra_Software_Devel/1   6/1/11 3:23p atanugul
 * SW3461-5: Add THD support to 3461
 *
 ***************************************************************************/
#include "bstd.h"
#include "bthd.h"
#include "bthd.h"
#include "bhab.h"
#include "bthd_3461.h"
#include "bthd_3461_priv.h"

BDBG_MODULE(bthd_3461);

static const BTHD_Settings defDevSettings =
{
    {  /* i2c settings */
        0x00, /* chipAddr */
        NULL, /* interruptEnableFunc */
        NULL  /* interruptEnableFuncParam */
    },
    { /* API function table */
        BTHD_3461_P_Open,
        BTHD_3461_P_Close,
        BTHD_3461_P_Init,
        NULL, /* BTHD_3461_P_ReadRegister*/
        NULL, /* BTHD_3461_P_WriteRegister */
        NULL, /* BTHD_3461_P_Mi2cWrite */
        NULL, /* BTHD_3461_P_Mi2cRead */
        BTHD_3461_P_SetAcquireParams,        
        BTHD_3461_P_GetAcquireParams,       
        BTHD_3461_P_TuneAcquire,
        BTHD_3461_P_GetThdStatus,
        BTHD_3461_P_ResetInbandStatus,
        BTHD_3461_P_GetChipRevision,
        BTHD_3461_P_GetVersion,
        BTHD_3461_P_GetVersionInfo,
        BTHD_3461_P_GetInterruptEventHandle,
        NULL, /* BTHD_3461_P_HandleInterrupt_isr */
        BTHD_3461_P_ProcessInterruptEvent,
        BTHD_3461_P_GetLockStateChangeEvent,
        NULL, /* BTHD_3461_P_AcquireIfd */
        NULL, /* BTHD_3461_P_GetIfdStatus */
        BTHD_3461_P_GetSoftDecisionBuf,
        BTHD_3461_GetDefaultInbandParams,
        NULL, /* BTHD_3461_GetDefaultIfdParams */
        NULL, /* BTHD_3461_P_ResetIfdStatus */
        NULL, /* BTHD_3461_P_SetIfdAudioSettings */
        NULL, /* BTHD_3461_P_GetIfdAudioSettings */
        BTHD_3461_P_PowerUp,
        BTHD_3461_P_PowerDown,
        NULL, /* BTHD_7550_P_GetEWSEventHandle */
        BTHD_3461_P_GetThdLockStatus,
        NULL, /* BTHD_3x7x_GetBBSInterruptEventHandle */
        NULL, /* BTHD_3x7x_ProcessBBSInterruptEvent */
        BTHD_3461_P_RequestThdAsyncStatus,
        BTHD_3461_P_GetThdAsyncStatus,
        BTHD_3461_P_InstallCallback        
    },
    NULL, /* BTMR_Handle */
    false, /* RF Delta Sigma Invert */
    false,  /* IF Delta Sigma Invert */
    BTHD_AgcPinMode_e25V,
    NULL,  /* BMEM_Heap_Handle */
    false, /* IF AGC zero */
    NULL,
	false /* isdbt support */
};

static const BTHD_InbandParams defInbandParams = 
{
    BTHD_InbandMode_eDvbt,          /* Inband mode */
    BTHD_Bandwidth_8Mhz,            /* Bandwidth */
    666000000,                      /* Tuner Frequency */
    36000000,                       /* IF Frequency */
    BTHD_CCI_Auto,                  /* Co-channel filter options */
    BTHD_PullInRange_eWide,         /* Pull-in range */
    BTHD_ModeGuard_eAutoDvbt,       /* Mode/Guard auto acquire */
    BTHD_TransmissionMode_e2K,      /* Manual Transmission mode (FFT Size) */
    BTHD_GuardInterval_e1_4,        /* Manual guard interval selection */
    false,                          /* Auto tuneAcquire */
    BTHD_Decode_Hp,                 /* Decode mode */
    true,                           /* TPS Acquire */
    BTHD_Modulation_e64Qam,         /* Modulation type */
    BTHD_CodeRate_e1_2,             /* Manual coderate LP */
    BTHD_CodeRate_e1_2,             /* Manual coderate HP */
    BTHD_Hierarchy_0,               /* Hierarchy selection */
    false,							/* ISDB-T */
    false,							/* ISDB-T partial reception */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    0,								/* ISDB-T */
    BTHD_ThdAcquisitionMode_eAuto   /* eAcquisitionMode */  
};


/******************************************************************************
  BTHD_3461_P_GetDefaultInbandParams
 ******************************************************************************/
BERR_Code BTHD_3461_GetDefaultInbandParams(
        BTHD_InbandParams *pDefSettings /* [out] default InbandParams */
        )
{
    *pDefSettings = defInbandParams; 
    return BERR_SUCCESS;
}


/******************************************************************************
  BTHD_3461_GetDefaultSettings()
 ******************************************************************************/
BERR_Code BTHD_3461_GetDefaultSettings(
        BTHD_Settings *pDefSettings /* [out] default settings */
        )
{
    *pDefSettings = defDevSettings;
    return BERR_SUCCESS;
}

