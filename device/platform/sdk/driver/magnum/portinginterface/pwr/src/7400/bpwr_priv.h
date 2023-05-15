/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpwr_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/21/12 7:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwr/src/7400/bpwr_priv.h $
 * 
 * Hydra_Software_Devel/1   9/21/12 7:12p nickh
 * SW7425-3971: Add chip-specific private files
 * 
 * Hydra_Software_Devel/4   7/28/09 5:09p mward
 * PR57145:  pwr updated as done for 7335.
 * 
 * Hydra_Software_Devel/3   4/1/08 10:57a mward
 * PR41138: Fix build with DEBUG=n.
 * 
 * Hydra_Software_Devel/2   1/4/08 9:38a jessem
 * PR 38367: Added eMemCtrlPwrMode field to BPWR_P_MemCtrl_Info struct.
 * 
 * Hydra_Software_Devel/1   12/20/07 10:38a jessem
 * PR 38367: Initial version.
 * 
 ***************************************************************************/
#ifndef BPWR_PRIV_H__
#define BPWR_PRIV_H__

#include "bpwr.h"
#include "bdbg.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * PWR Internal data structures
 ***************************************************************************/

BDBG_OBJECT_ID_DECLARE(BPWR_P_Context);

/* static array makes for a simple implementation */
#define BPWR_MAX_MODULES 128

typedef struct BPWR_P_Context
{
    BDBG_OBJECT(BPWR_P_Context)

    /* public fields */
    BPWR_Settings       stSettings;
    BREG_Handle         hRegister;
    BCHP_Handle         hChip;

    struct {
        BSTD_Module moduleId; /* supported module */
        BPWR_ePowerMode mode; /* current state */
        bool change;          /* in current change set */
    } modules[BPWR_MAX_MODULES];

    BPWR_ePowerMode changeMode; /* new mode */
} BPWR_P_Context;

/***************************************************************************
 * PWR private functions
 * These functions are chip-specific.
 ***************************************************************************/
BERR_Code BPWR_P_Init(BPWR_P_Context *pPwr);
BERR_Code BPWR_P_Destroy(BPWR_P_Context *pPwr);
BERR_Code BPWR_P_ValidateChanges(BPWR_P_Context *pPwr);
BERR_Code BPWR_P_SetPowerMode(BPWR_P_Context *pPwr);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BPWR_PRIV_H__ */
/* End of file. */
