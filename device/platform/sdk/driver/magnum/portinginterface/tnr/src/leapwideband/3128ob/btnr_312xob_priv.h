/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_312xob_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 10/18/12 3:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/leapwideband/3128ob/btnr_312xob_priv.h $
 * 
 * Hydra_Software_Devel/8   10/18/12 3:43p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/7   9/28/12 3:07p atanugul
 * SW3128-228: Add support for BTNR_GetVersionInfo()
 * 
 * Hydra_Software_Devel/6   12/22/11 3:38p atanugul
 * SW3128-100: Check for power status before sending HAB commands
 * 
 * Hydra_Software_Devel/5   12/12/11 5:32p atanugul
 * SW3128-92: Add support to auto detect 3123/4/8 and restrict features
 * based on the chipId
 * 
 * Hydra_Software_Devel/4   8/30/11 1:03p atanugul
 * SW3128-47: Add Support for BCM3123
 * 
 * Hydra_Software_Devel/3   6/9/11 3:51p atanugul
 * SW3128-2: Add channel specific power up/down support for 3128 TNR
 * 
 * Hydra_Software_Devel/2   5/5/11 4:14p atanugul
 * SW3128-2: Remove BTNR_AciFilterCoeff
 * 
 * Hydra_Software_Devel/1   4/11/11 5:45p atanugul
 * SW3128-2: Add AOB TNR support for 3128
 * 
 ****************************************************************************/
#ifndef BTNR_312XOB_PRIV_H__
#define BTNR_312XOB_PRIV_H__

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
#define BTNR_CORE_TYPE_TO_FEED      0x2
#define BTNR_CORE_ID		        0x0
#define BTNR_CHANNEL_ID		        0x8
#define BTNR_ACQUIRE		        0x10
#define BTNR_ACQUIRE_PARAMS_WRITE 	0x11
#define BTNR_POWER_OFF            	0x18
#define BTNR_POWER_ON            	0x19
#define BTNR_POWER_STATUS_READ     	0x98
#define BTNR_ACQUIRE_PARAMS_READ 	0x91
#define BTNR_SYS_VERSION   	        0XBA

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BTNR_P_312xOb_Handle     *BTNR_312xOb_Handle;

typedef struct BTNR_P_312xOb_Settings
{
    unsigned long rfFreq;
    unsigned long ifFreq;
    int iRevLetter, iRevNumber, iType;      /* Saved chip information */
    BTNR_TunerMode tunerMode;   
} BTNR_P_312xOb_Settings;

typedef struct BTNR_P_312xOb_Handle
{
    uint32_t magicId;                   /* Used to check if structure is corrupt */
    BHAB_Handle hHab;
    BTNR_P_312xOb_Settings settings;
    bool bPowerdown;    
} BTNR_P_312xOb_Handle;

/*******************************************************************************
*
*   Private Module Data
*
*******************************************************************************/
BERR_Code BTNR_312xOb_Close(
    BTNR_Handle hDev                        /* [in] Device handle */
    );

BERR_Code BTNR_312xOb_SetRfFreq(
    BTNR_312xOb_Handle hDev,                /* [in] Device handle */
    uint32_t rfFreq,                        /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode                /* [in] Requested tuner mode */
    );

BERR_Code BTNR_312xOb_GetRfFreq(
    BTNR_312xOb_Handle hDev,                /* [in] Device handle */
    uint32_t *rfFreq,                       /* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode               /* [output] Returns tuner mode */
    );

BERR_Code BTNR_P_312xOb_GetAgcRegVal(
    BTNR_312xOb_Handle hDev,                /* [in] Device handle */
    uint32_t regOffset,                     /* [in] AGC register offset */
    uint32_t *agcVal                        /* [out] output value */
    );

BERR_Code BTNR_312xOb_SetAgcRegVal(
    BTNR_312xOb_Handle hDev,                /* [in] Device handle */
    uint32_t regOffset,                     /* [in] AGC register offset */
    uint32_t *agcVal                        /* [in] input value */
    );

BERR_Code BTNR_312xOb_GetInfo(
    BTNR_312xOb_Handle hDev,                /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo                 /* [out] Tuner information */
    );

BERR_Code BTNR_312xOb_GetPowerSaver(
    BTNR_312xOb_Handle hDev,                /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings    /* [in] Power saver settings. */
    );

BERR_Code BTNR_312xOb_SetPowerSaver(
    BTNR_312xOb_Handle hDev,                /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings    /* [in] Power saver settings. */
    );

BERR_Code BTNR_312xOb_GetSettings(
    BTNR_312xOb_Handle hDev,                /* [in] Device handle */
    BTNR_Settings *settings                 /* [out] TNR settings. */
    );

BERR_Code BTNR_312xOb_SetSettings(
    BTNR_312xOb_Handle hDev,                /* [in] Device handle */
    BTNR_Settings *settings                 /* [in] TNR settings. */
    );
    
BERR_Code BTNR_312xOb_GetVersionInfo(
    BTNR_312xOb_Handle hDev,        /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo  /* [out] Returns version Info */
    );
    
#ifdef __cplusplus
}
#endif
 
#endif



