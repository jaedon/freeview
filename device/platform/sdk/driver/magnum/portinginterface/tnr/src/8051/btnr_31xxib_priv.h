/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_31xxib_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 6/22/10 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3117ib/btnr_31xxib_priv.h $
 * 
 * Hydra_Software_Devel/5   6/22/10 6:06p vishk
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 * Hydra_Software_Devel/4   8/21/09 6:16p vishk
 * PR 57484: Bcm31xx: Add Power Management support for TNR
 * 
 * Hydra_Software_Devel/3   3/11/09 4:33p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/2   3/11/09 3:42p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/1   3/11/09 2:41p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 *  
 ***************************************************************************/
#ifndef BTNR_31XXIB_PRIV_H__
#define BTNR_31XXIB_PRIV_H__

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

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BTNR_P_31xxIb_Handle     *BTNR_31xxIb_Handle;

typedef struct BTNR_P_31xxIb_Settings
{
    unsigned long rfFreq;
    int iRevLetter, iRevNumber, iType;      /* Saved chip information */
    BTNR_TunerMode tunerMode;
    bool powerSaver;
} BTNR_P_31xxIb_Settings;

typedef struct BTNR_P_31xxIb_Handle
{
    uint32_t magicId;                   /* Used to check if structure is corrupt */
    BHAB_Handle hHab;
    BTNR_P_31xxIb_Settings settings;
} BTNR_P_31xxIb_Handle;

/*******************************************************************************
*
*   Private Module Data
*
*******************************************************************************/
BERR_Code BTNR_31xxIb_Close(
    BTNR_Handle hDev                        /* [in] Device handle */
    );

BERR_Code BTNR_31xxIb_SetRfFreq(
    BTNR_31xxIb_Handle hDev,                /* [in] Device handle */
    uint32_t rfFreq,                        /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode                /* [in] Requested tuner mode */
    );

BERR_Code BTNR_31xxIb_GetRfFreq(
    BTNR_31xxIb_Handle hDev,                /* [in] Device handle */
    uint32_t *rfFreq,                       /* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode               /* [output] Returns tuner mode */
    );

BERR_Code BTNR_P_31xxIb_GetAgcRegVal(
    BTNR_31xxIb_Handle hDev,                /* [in] Device handle */
    uint32_t regOffset,                     /* [in] AGC register offset */
    uint32_t *agcVal                        /* [out] output value */
    );

BERR_Code BTNR_31xxIb_SetAgcRegVal(
    BTNR_31xxIb_Handle hDev,                /* [in] Device handle */
    uint32_t regOffset,                     /* [in] AGC register offset */
    uint32_t *agcVal                        /* [in] input value */
    );

BERR_Code BTNR_31xxIb_GetInfo(
    BTNR_31xxIb_Handle hDev,                /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo                 /* [out] Tuner information */
    );

BERR_Code BTNR_31xxIb_GetPowerSaver(
    BTNR_31xxIb_Handle hDev,                /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings    /* [in] Power saver settings. */
    );

BERR_Code BTNR_31xxIb_SetPowerSaver(
    BTNR_31xxIb_Handle hDev,                /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings    /* [in] Power saver settings. */
    );

#ifdef __cplusplus
    }
#endif
    
#endif

