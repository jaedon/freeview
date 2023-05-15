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
 * $brcm_Date: 9/21/12 7:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwr/src/7340/bpwr_priv.h $
 * 
 * Hydra_Software_Devel/1   9/21/12 7:10p nickh
 * SW7425-3971: Add chip-specific private files
 * 
 * Hydra_Software_Devel/1   10/15/10 4:52p gskerl
 * SW7340-215: Add first draft of PWR PI for 7340/7342
 * 
 * Hydra_Software_Devel/1   3/1/10 4:37p randyjew
 * SW7468-6:Adding bpwr_priv for 7468/7208
 * 
 * Hydra_Software_Devel/1   2/4/10 6:37p nickh
 * SW7420-573: Future test port
 * 
 * Hydra_Software_Devel/5   5/15/09 11:59a erickson
 * PR55154: rework
 *
 * Hydra_Software_Devel/4   1/30/09 3:58p jrubio
 * PR51629: fix broken if statements
 *
 * Hydra_Software_Devel/3   1/29/09 4:39p garylin
 * PR51626: added support for 7336 (just add ID checking during compile
 * time.)
 *
 * Hydra_Software_Devel/2   8/19/08 10:23a garylin
 * PR44622: add power down/up clockgen for frontend AST0, AST1, QPSK, FTM
 *
 * Hydra_Software_Devel/1   7/7/08 4:25p garylin
 * PR44622: initial version, ported from 7325
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
bool BPWR_P_IsModuleChanging(const BPWR_P_Context *pPwr, BSTD_Module moduleId);
void BPWR_P_TestPort_Up(BPWR_P_Context *pPwr);
void BPWR_P_TestPort_Down(BPWR_P_Context *pPwr);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BPWR_PRIV_H__ */
/* End of file. */
