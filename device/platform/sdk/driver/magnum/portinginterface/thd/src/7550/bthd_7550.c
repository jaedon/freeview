/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthd_7550.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/11/12 11:46a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/thd/src/7550/bthd_7550.c $
 * 
 * Hydra_Software_Devel/1   9/11/12 11:46a farshidf
 * SW7425-3819: Remove symbolic links and flatten out THD directory
 * structure
 * 
 * Hydra_Software_Devel/23   2/2/12 6:33p atanugul
 * SW3461-141: Add initializers for BTHD_SetAcquireParams
 * BTHD_GetAcquireParams and BTHD_PowerUp
 * 
 * Hydra_Software_Devel/22   6/10/11 2:56p farshidf
 * SWDTV-7504: update the interface for other chip
 * 
 * Hydra_Software_Devel/21   6/2/11 10:16a atanugul
 * SW3461-5: fix compilation error
 * 
 * Hydra_Software_Devel/20   6/1/11 3:58p atanugul
 * SW3461-5: Add initializer for BTHD_3461_P_RequestThdAsyncStatus,
 * BTHD_3461_P_GetThdAsyncStatus, BTHD_3461_P_InstallCallback
 * 
 * Hydra_Software_Devel/19   5/31/11 3:20p farshidf
 * SW7552-36: make it compatible
 * 
 * Hydra_Software_Devel/18   10/21/10 2:41p farshidf
 * SW7550-604: merge
 * 
 * Hydra_Software_Devel/SW7550-604/1   10/21/10 1:00p pinglong
 * SW7550-604: add interface for resetting OFDM status
 * 
 * Hydra_Software_Devel/16   5/18/10 3:40p farshidf
 * SW7550-440: merge to main branch
 * 
 * Hydra_Software_Devel/SW7550-399/1   5/6/10 6:15p farshidf
 * SW7550-399: add the mini tune while we are lock
 * 
 * Hydra_Software_Devel/15   3/2/10 4:28p farshidf
 * SW7550-38: merge the auto branch
 * 
 * Hydra_Software_Devel/THD_7550_Auto/2   2/3/10 5:38p farshidf
 * SW7550-38: add the scan mode
 * 
 * Hydra_Software_Devel/THD_7550_Auto/1   2/1/10 6:05p farshidf
 * SW7550-38: initinal ISDB-T channel scan
 * 
 * Hydra_Software_Devel/14   1/25/10 9:57a dliu
 * SW7405-3823: Add default for bIfAgcZero
 * 
 * Hydra_Software_Devel/13   12/17/09 6:33p farshidf
 * SW7550-38: add the BTHD_7550_P_GetSoftDecisionBuf
 * 
 * Hydra_Software_Devel/12   11/20/09 1:08p farshidf
 * SW7550-38: compile fix
 * 
 * Hydra_Software_Devel/11   11/20/09 11:37a farshidf
 * SW7550-38: update the default
 * 
 * Hydra_Software_Devel/10   11/20/09 7:31a nitinb
 * SW7550-38:Farshid merge the latest
 * 
 * Hydra_Software_Devel/THD_7550_40_6/1   11/18/09 5:05p rperlow
 * Changed default application parameters for ISDB-T
 * 
 * Hydra_Software_Devel/9   11/6/09 5:56p farshidf
 * SW7550-38: changes to map Nexus
 * 
 * Hydra_Software_Devel/8   11/6/09 10:55a farshidf
 * SW7550-38: compile fix
 * 
 * Hydra_Software_Devel/7   11/6/09 10:44a farshidf
 * SW7550-38: update the BTHD_InbandParams to match 7550
 * 
 * Hydra_Software_Devel/6   11/5/09 3:48p farshidf
 * SW7550-38: compile fix
 * 
 * Hydra_Software_Devel/5   11/5/09 3:33p farshidf
 * SW7550-38: compile issue
 * 
 * Hydra_Software_Devel/4   11/5/09 2:46p farshidf
 * SW7550-38: 7550 PI
 * 
 * Hydra_Software_Devel/THD_7550_40_2/1   11/5/09 2:41p farshidf
 * SW7550-38: 7550 API mapping
 * 
 * Hydra_Software_Devel/3   11/1/09 2:52p farshidf
 * SW7550-38:compile issue
 * 
 * Hydra_Software_Devel/2   11/1/09 2:18p farshidf
 * SW7550-38: merge the latest
 * 
 * Hydra_Software_Devel/THD_7550_40/3   10/9/09 1:02p farshidf
 * PR41450: dynamic chnages
 * 
 * Hydra_Software_Devel/THD_7550_40/2   10/1/09 4:03p farshidf
 * SW7550-38: remove ifd code and add the Dynamic Aquire Param
 * 
 * Hydra_Software_Devel/THD_7550_40/1   9/21/09 4:07p jputnam
 * SW7550-40:  Fixed VELOCE_EMULATION ifdef in P_TuneAcquire
 * - Cleaned-up FI coefficient tables
 * - Reworked WriteFICoef() function
 * - Cosmetic cleanup of indentation and comments
 * 
 * Hydra_Software_Devel/1   7/31/09 4:25p farshidf
 * PR41450: add the 7550 thd code
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bthd.h"
#include "bthd.h"
#include "bthd_7550.h"
#include "bthd_7550_priv.h"
#include "bthd_7550_platform.h"

BDBG_MODULE(bthd_7550);

static const BTHD_Settings defDevSettings =
{
  {  /* i2c settings */
    0x00, /* chipAddr */
    NULL, /* interruptEnableFunc */
    NULL  /* interruptEnableFuncParam */
  },
  { /* API function table */
    BTHD_7550_P_Open,
    BTHD_7550_P_Close,
    BTHD_7550_P_Init,
    NULL, /*BTHD_7550_P_ReadRegister,*/
    NULL, /* BTHD_7550_P_WriteRegister, */
    NULL, /* BTHD_7550_P_Mi2cWrite, */
    NULL, /* BTHD_7550_P_Mi2cRead, */
    NULL, /* BTHD_SetAcquireParams */
    NULL, /* BTHD_GetAcquireParams */    
    BTHD_7550_P_TuneAcquire,
    BTHD_7550_P_GetThdStatus,
    BTHD_7550_P_ResetStatus, /*BTHD_7550_P_ResetInbandStatus,*/
    NULL, /*BTHD_7550_P_GetChipRevision,*/
    NULL, /*BTHD_7550_P_GetVersion,*/
    BTHD_7550_P_GetInterruptEventHandle,
    NULL, /* BTHD_7550_P_HandleInterrupt_isr */
    BTHD_7550_P_ProcessInterruptEvent,
    BTHD_7550_P_GetLockStateChangeEvent,
    NULL, /* BTHD_7550_P_AcquireIfd,*/
    NULL, /* BTHD_7550_P_GetIfdStatus,*/
    BTHD_7550_P_GetSoftDecisionBuf,
    BTHD_7550_GetDefaultInbandParams,
    NULL, /* BTHD_7550_GetDefaultIfdParams, */
    NULL, /* BTHD_7550_P_ResetIfdStatus,*/
    NULL, /* BTHD_7550_P_SetIfdAudioSettings,*/
    NULL, /* BTHD_7550_P_GetIfdAudioSettings,*/
    NULL, /* BTHD_PowerUp */    
	BTHD_7550_P_PowerDown,
	BTHD_7550_P_GetEWSEventHandle,
	BTHD_7550_P_GetThdLockStatus,
	NULL, /* GetBBSInterruptEventHandle */
	NULL, /* ProcessBBSInterruptEvent */
    NULL, /* BTHD_RequestThdAsyncStatus */
    NULL, /* BTHD_GetThdAsyncStatus */
    NULL /* BTHD_InstallCallback */
    },
    NULL, /* BTMR_Handle */
    false, /* RF Delta Sigma Invert */
    false,  /* IF Delta Sigma Invert */
    0,
	NULL,  /* BMEM_Heap_Handle */
	false, /* IF AGC zero */
    NULL,
	false /* isdbt support */    
};


static const BTHD_InbandParams defInbandParams = 
{
  BTHD_InbandMode_eDvbt,         /* Inband mode */
  BTHD_Bandwidth_8Mhz,            /* Bandwidth */
  666000000,                      /* Tuner Frequency */
  36000000,                       /* IF Frequency */
  BTHD_CCI_Auto,                  /* Co-channel filter options */
  BTHD_PullInRange_eWide,         /* Pull-in range */
  true,                           /* Mode/Guard auto acquire */
  BTHD_TransmissionMode_e2K,      /* Manual Transmission mode (FFT Size) */
  BTHD_GuardInterval_e1_4,        /* Manual guard interval selection */
  BTHD_Decode_Hp,                 /* Decode mode */
  false,                          /* TPS Acquire */
  BTHD_Modulation_e64Qam,         /* Modulation type */
  BTHD_CodeRate_e1_2,             /* Manual coderate LP */
  BTHD_CodeRate_e1_2,             /* Manual coderate HP */
  BTHD_Hierarchy_0,               /* Hierarchy selection */
  true,							  /* ISDB-T TMCC acquire */
  false,						  /* ISDB-T partial reception */
  BTHD_Modulation_e16Qam,		  /* ISDB-T Layer A modulation type */
  BTHD_CodeRate_e1_2,			  /* ISDB-T Layer A code rate */
  BTHD_IsdbtTimeInterleaving_0X,  /* ISDB-T Layer A time interleaving */
  13,                             /*ISDB-T Layer A number of segments*/
  BTHD_Modulation_e16Qam,         /* ISDB-T Layer B modulation type */
  BTHD_CodeRate_e1_2,             /* ISDB-T Layer B code rate */
  BTHD_IsdbtTimeInterleaving_0X,  /* ISDB-T Layer B time interleaving */
  0,                              /*ISDB-T Layer A number of segments*/
  BTHD_Modulation_e16Qam,         /* ISDB-T Layer B modulation type */
  BTHD_CodeRate_e1_2,             /* ISDB-T Layer C code rate */
  BTHD_IsdbtTimeInterleaving_0X,  /* ISDB-T Layer C time interleaving*/
  0,                               /* ISDB-T Layer C number of segments*/
  BTHD_ThdAcquisitionMode_eAuto    /* Auto Acquire */
};




/***************************************************************************
 * BTHD_7550_P_GetDefaultInbandParams
 ***************************************************************************/
BERR_Code BTHD_7550_GetDefaultInbandParams(BTHD_InbandParams *pDefSettings)
{
  *pDefSettings = defInbandParams; 
  return BERR_SUCCESS;
}

/***************************************************************************
 * BTHD_7550_GetDefaultSettings()
 ***************************************************************************/
BERR_Code BTHD_7550_GetDefaultSettings(BTHD_Settings *pDefSettings)
{
  *pDefSettings = defDevSettings;
  return BERR_SUCCESS;
}


