/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3461ib_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 10/23/12 5:32p $
 *
 * Module Description:
 *
 * Revision History:  $
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/leapnarrowband/3461ib/btnr_3461ib_priv.h $
 * 
 * Hydra_Software_Devel/11   10/23/12 5:32p atanugul
 * SW3461-252: Clean Up
 * 
 * Hydra_Software_Devel/10   10/18/12 5:34p atanugul
 * SW3461-169: Add BTNR_GetVersionInfo() support for 3461
 * 
 * Hydra_Software_Devel/9   9/7/12 10:34a atanugul
 * SW7425-3788: HAB directory restructure
 * 
 * Hydra_Software_Devel/8   7/30/12 3:14p atanugul
 * SW3461-252: Add PI support for Tuen Gain API
 * 
 * Hydra_Software_Devel/7   6/3/11 5:21p atanugul
 * SW3461-3: Implemented hab command for rfinputmode, updated tc2 acquire
 * parameters, TNR_GetPowerSaver()
 * 
 * Hydra_Software_Devel/6   5/27/11 2:18p atanugul
 * SW3461-3: Converted BTNR_LpfBandwidth enum into unsigned integer
 * 
 * Hydra_Software_Devel/5   5/24/11 5:37p atanugul
 * SW3461-3: Modify BTNR_LpfBandwidth, BTNR_Standard according to the
 * naming convention
 * 
 * Hydra_Software_Devel/4   5/24/11 3:29p atanugul
 * SW3461-3: Add BTNR_RfInputMode and agcVal to BTNR_Settings structure
 * and remore BTNR_PowerMode enum
 * 
 * Hydra_Software_Devel/3   4/19/11 12:33p atanugul
 * SW3461-3: Implement BTNR_SetSettings(), Powermode for 3461 TNR and
 * update BHAB_3461_P_DecodeInterrupt()
 * 
 * Hydra_Software_Devel/2   3/31/11 11:24a atanugul
 * SW3461-3: Update GetStatus, DecodeInterrupt and code cleanup
 * 
 * Hydra_Software_Devel/1   3/16/11 3:25p atanugul
 * SW3461-3: Add support for BCM3461
 * 
 ***************************************************************************/
#ifndef BTNR_3461_PRIV_H__
#define BTNR_3461_PRIV_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "berr_ids.h"
#include "btnr.h"
#include "bhab.h"
#include "btnr_3461ib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAB_MSG_HDR(OPCODE,N,CORE_TYPE,CORE_ID) \
    { ((uint8_t)(((uint16_t)(OPCODE)) >> 2)), \
    (((uint8_t)(0x03 & (OPCODE)) << 6) | ((uint8_t)((N)>>4))), \
    ((((uint8_t)(((N)& 0x0F) << 4))) | ((uint8_t)(0x0F & (CORE_TYPE)))), \
    ((uint8_t)(CORE_ID)) }

#define BTNR_CORE_TYPE		        0xF
#define BTNR_CORE_ID		        0x0
#define BTNR_ACQUIRE		        0x10
#define BTNR_ACQUIRE_PARAMS_WRITE 	0x11
#define BTNR_ACQUIRE_PARAMS_READ 	0x91
#define BTNR_POWER_CTRL_ON      	0x19
#define BTNR_POWER_CTRL_OFF      	0x18
#define BTNR_POWER_CTRL_READ      	0x98
#define BTNR_SYS_VERSION   	        0XBA

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BTNR_P_3461_Handle     *BTNR_3461_Handle;

typedef struct BTNR_P_3461_Settings
{
    unsigned long rfFreq;
    int iRevLetter, iRevNumber, iType;      /* Saved chip information */
    BTNR_TunerMode tunerMode;
    bool powerSaver;
    BTNR_Standard std;
    BTNR_TunerApplication tnrApplication;
    uint32_t bandwidth;
    BTNR_RfInputMode rfInputMode; /* TNR RF input mode */
    BTNR_InternalGainSettings internalGainSettings; /* internal gain settings */
    BTNR_ExternalGainSettings externalGainSettings; /* external gain settings */    
} BTNR_P_3461_Settings;

typedef struct BTNR_P_3461_Handle
{
    uint32_t magicId;                   /* Used to check if structure is corrupt */
    BHAB_Handle hHab;
    BTNR_P_3461_Settings settings;
    unsigned int channelNo;                 /* Channel number to tune to */
} BTNR_P_3461_Handle;

/*******************************************************************************
*
*   Private Module Data
*
*******************************************************************************/
BERR_Code BTNR_3461_Close(
    BTNR_Handle hDev                        /* [in] Device handle */
    );

BERR_Code BTNR_3461_SetRfFreq(
    BTNR_3461_Handle hDev,                /* [in] Device handle */
    uint32_t rfFreq,                        /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode                /* [in] Requested tuner mode */
    );

BERR_Code BTNR_3461_GetRfFreq(
    BTNR_3461_Handle hDev,                /* [in] Device handle */
    uint32_t *rfFreq,                       /* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode               /* [output] Returns tuner mode */
    );

BERR_Code BTNR_P_3461_GetAgcRegVal(
    BTNR_3461_Handle hDev,                /* [in] Device handle */
    uint32_t regOffset,                     /* [in] AGC register offset */
    uint32_t *agcVal                        /* [out] output value */
    );

BERR_Code BTNR_3461_SetAgcRegVal(
    BTNR_3461_Handle hDev,                /* [in] Device handle */
    uint32_t regOffset,                     /* [in] AGC register offset */
    uint32_t *agcVal                        /* [in] input value */
    );

BERR_Code BTNR_3461_GetInfo(
    BTNR_3461_Handle hDev,                /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo                 /* [out] Tuner information */
    );

BERR_Code BTNR_3461_GetPowerSaver(
    BTNR_3461_Handle hDev,                /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings    /* [in] Power saver settings. */
    );

BERR_Code BTNR_3461_SetPowerSaver(
    BTNR_3461_Handle hDev,                /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings    /* [in] Power saver settings. */
    );

BERR_Code BTNR_3461_GetSettings(
    BTNR_3461_Handle hDev,                /* [in] Device handle */
    BTNR_Settings *settings                 /* [out] TNR settings. */
    );

BERR_Code BTNR_3461_SetSettings(
    BTNR_3461_Handle hDev,                /* [in] Device handle */
    BTNR_Settings *settings                 /* [in] TNR settings. */
    );
    
BERR_Code BTNR_3461_GetVersionInfo(
    BTNR_3461_Handle hDev,          /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo  /* [out] Returns version Info */
    );
    
#ifdef __cplusplus
    }
#endif
    
#endif

