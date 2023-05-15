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
 * $brcm_Date: 9/21/12 7:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwr/src/7325/bpwr_priv.h $
 * 
 * Hydra_Software_Devel/1   9/21/12 7:07p nickh
 * SW7425-3971: Add chip-specific private files
 * 
 * Hydra_Software_Devel/7   5/26/10 2:27p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/7   5/26/10 2:25p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/7   5/26/10 2:24p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/7   5/26/10 2:23p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/7   5/26/10 2:23p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/7   5/26/10 2:23p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/7   5/26/10 2:23p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/7   5/26/10 2:22p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/6   1/12/10 3:31p rjain
 * SW7550-163: Adding functions to calibrate the IRW timer
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
#include "btmr.h"
#include "bint.h"
#include "bkni.h"


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

    /* Following are required to calibrate the IRW timer tick */
    BINT_CallbackHandle hIrwCallback;
    BINT_CallbackFunc  appCb;       /* The app callback function */ 
    void               *context;    /* The app specified parameter 
                                      that needs to be passed unchanged to the 
                                      callback */
    int                 param;       /* The app specified parameter 
                                      that needs to be passed unchanged to the 
                                      callback */   
    BTMR_TimerHandle    hTimer;
    uint32_t            startTime;  /* value in microseconds */
    uint32_t            endTime;    /* value in microseconds */
    
    struct {
        BSTD_Module moduleId;   /* supported module ID*/
        const char *moduleText; /* printable module name */
        BPWR_ePowerMode mode;   /* current state */
        bool change;            /* in current change set */
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
