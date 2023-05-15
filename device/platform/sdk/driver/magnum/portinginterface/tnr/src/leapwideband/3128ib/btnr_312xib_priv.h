/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_312xib_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 10/18/12 3:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/leapwideband/3128ib/btnr_312xib_priv.h $
 * 
 * Hydra_Software_Devel/12   10/18/12 3:43p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/11   9/28/12 3:06p atanugul
 * SW3128-228: Add support for BTNR_GetVersionInfo()
 * 
 * Hydra_Software_Devel/10   3/30/12 6:09p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/9   3/30/12 12:08p atanugul
 * SW3128-140: Add Spectrum Analyzer feature to the TNR PI
 * 
 * Hydra_Software_Devel/8   12/22/11 3:37p atanugul
 * SW3128-100: Check for power status before sending HAB commands
 * 
 * Hydra_Software_Devel/7   12/12/11 5:31p atanugul
 * SW3128-92: Add support to auto detect 3123/4/8 and restrict features
 * based on the chipId
 * 
 * Hydra_Software_Devel/6   8/30/11 4:17p atanugul
 * SW3128-47: Add Support for BCM3123
 * 
 * Hydra_Software_Devel/5   8/30/11 12:50p atanugul
 * SW3128-47: Add Support for BCM3123
 * 
 * Hydra_Software_Devel/4   5/5/11 4:13p atanugul
 * SW3128-2: Remove BTNR_AciFilterCoeff
 * 
 * Hydra_Software_Devel/3   4/11/11 5:15p atanugul
 * SW3128-2: Add Get_Settings and SetSettings support to 3128 TNR PI
 * 
 * Hydra_Software_Devel/2   3/28/11 6:15p atanugul
 * SW3128-2: code cleanup
 * 
 * Hydra_Software_Devel/1   3/4/11 3:18p vishk
 * SW7422-171: Nexus support for the 3128 (8 TNR core + tuner on daughter
 * board) plug on 7422 board
 * 
 * Hydra_Software_Devel/1   2/1/11 7:36a atanugul
 * SW3128-2: Add Support for BCM3128
 *  
 ***************************************************************************/
#ifndef BTNR_312XIB_PRIV_H__
#define BTNR_312XIB_PRIV_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "berr_ids.h"
#include "btnr.h"
#include "bhab.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CHK_RETCODE( rc, func )         \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

#define HAB_MSG_HDR(OPCODE,N,CORE_TYPE,CORE_ID) \
    { ((uint8_t)(((uint16_t)(OPCODE)) >> 2)), \
    (((uint8_t)(0x03 & (OPCODE)) << 6) | ((uint8_t)((N)>>4))), \
    ((((uint8_t)(((N)& 0x0F) << 4))) | ((uint8_t)(0x0F & (CORE_TYPE)))), \
    ((uint8_t)(CORE_ID)) }

#define BTNR_CORE_TYPE		        0xE
#define BTNR_CORE_TYPE_TO_FEED      0x1
#define BTNR_CORE_ID		        0x0
#define BTNR_ACQUIRE		        0x10
#define BTNR_ACQUIRE_PARAMS_WRITE 	0x11
#define BTNR_POWER_OFF            	0x18
#define BTNR_POWER_ON            	0x19
#define BTNR_POWER_STATUS_READ     	0x98
#define BTNR_ACQUIRE_PARAMS_READ 	0x91
#define BTNR_REQUEST_SPECTRUM_DATA 	0x122
#define BTNR_GET_SPECTRUM_DATA   	0x322
#define BTNR_SYS_VERSION   	        0XBA

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BTNR_P_312xIb_Handle     *BTNR_312xIb_Handle;

typedef struct BTNR_P_312xIb_Settings
{
    unsigned long rfFreq;
    int iRevLetter, iRevNumber, iType;      /* Saved chip information */
    BTNR_TunerMode tunerMode;
} BTNR_P_312xIb_Settings;

typedef struct BTNR_P_312xIb_Handle
{
    uint32_t magicId;                   /* Used to check if structure is corrupt */
    BHAB_Handle hHab;
    BHAB_DevId devId;    
    BTNR_P_312xIb_Settings settings;
    unsigned int channelNo;                 /* Channel number to tune to */
    unsigned int mxChnNo;
    bool bPowerdown;
    BTNR_CallbackFunc pCallback[BTNR_Callback_eLast];
    void *pCallbackParam[BTNR_Callback_eLast];    
} BTNR_P_312xIb_Handle;

/*******************************************************************************
*
*   Private Module Data
*
*******************************************************************************/
BERR_Code BTNR_312xIb_Close(
    BTNR_Handle hDev                        /* [in] Device handle */
    );

BERR_Code BTNR_312xIb_SetRfFreq(
    BTNR_312xIb_Handle hDev,                /* [in] Device handle */
    uint32_t rfFreq,                        /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode                /* [in] Requested tuner mode */
    );

BERR_Code BTNR_312xIb_GetRfFreq(
    BTNR_312xIb_Handle hDev,                /* [in] Device handle */
    uint32_t *rfFreq,                       /* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode               /* [output] Returns tuner mode */
    );

BERR_Code BTNR_P_312xIb_GetAgcRegVal(
    BTNR_312xIb_Handle hDev,                /* [in] Device handle */
    uint32_t regOffset,                     /* [in] AGC register offset */
    uint32_t *agcVal                        /* [out] output value */
    );

BERR_Code BTNR_312xIb_SetAgcRegVal(
    BTNR_312xIb_Handle hDev,                /* [in] Device handle */
    uint32_t regOffset,                     /* [in] AGC register offset */
    uint32_t *agcVal                        /* [in] input value */
    );

BERR_Code BTNR_312xIb_GetInfo(
    BTNR_312xIb_Handle hDev,                /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo                 /* [out] Tuner information */
    );

BERR_Code BTNR_312xIb_GetPowerSaver(
    BTNR_312xIb_Handle hDev,                /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings    /* [in] Power saver settings. */
    );

BERR_Code BTNR_312xIb_SetPowerSaver(
    BTNR_312xIb_Handle hDev,                /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings    /* [in] Power saver settings. */
    );

BERR_Code BTNR_312xIb_GetSettings(
    BTNR_312xIb_Handle hDev,                /* [in] Device handle */
    BTNR_Settings *settings                 /* [out] TNR settings. */
    );

BERR_Code BTNR_312xIb_SetSettings(
    BTNR_312xIb_Handle hDev,                /* [in] Device handle */
    BTNR_Settings *settings                 /* [in] TNR settings. */
    );

BERR_Code BTNR_312xIb_RequestSpectrumAnalyzerData(
    BTNR_312xIb_Handle hDev,     /* [in] Device handle */ 
    BTNR_SpectrumSettings *pSettings /* [in] spectrum settings */
    );

BERR_Code BTNR_312xIb_GetSpectrumAnalyzerData(
    BTNR_312xIb_Handle hDev,     /* [in] Device handle */ 
    BTNR_SpectrumData  *pSpectrumData /* [out] spectrum Data*/
    );

BERR_Code BTNR_312xIb_InstallCallback(
    BTNR_312xIb_Handle hDev,     /* [in] Device handle */
    BTNR_Callback callbackType, /* [in] Type of callback */
    BTNR_CallbackFunc pCallback, /* [in] Function Ptr to callback */
    void *pParam                 /* [in] Generic parameter send on callback */
    );

BERR_Code BTNR_312xIb_GetVersionInfo(
    BTNR_312xIb_Handle hDev,        /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo  /* [out] Returns version Info */
    );
    
#ifdef __cplusplus
    }
#endif
    
#endif

