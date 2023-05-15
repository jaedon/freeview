/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpwr.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/21/12 6:56p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/pwr/src/bpwr.c $
 * 
 * Hydra_Software_Devel/1   9/21/12 6:56p nickh
 * SW7425-3971: Add common source files
 * 
 * Hydra_Software_Devel/9   1/26/11 11:43a jrubio
 * SW7325-836: Take out Critical Sections, PWR PI modules are not
 * reentrant, please modify application code to encapsulate critical
 * calls
 * 
 * Hydra_Software_Devel/8   1/26/11 10:15a jrubio
 * SW7325-836: [Yong] encapsulate critical sections
 * 
 * Hydra_Software_Devel/7   5/28/10 12:06p spothana
 * SW7125-237: Fixing compilation error.
 * 
 * Hydra_Software_Devel/6   5/26/10 2:22p gskerl
 * SW7125-237: Improved debug messages
 * 
 * Hydra_Software_Devel/5   5/15/09 11:59a erickson
 * PR55154: rework
 *
 * Hydra_Software_Devel/4   1/30/09 4:02p jrubio
 * PR51629: fix broken if statements
 *
 * Hydra_Software_Devel/3   1/29/09 4:39p garylin
 * PR51626: added support for 7336 (just add ID checking during compile
 * time.)
 *
 * Hydra_Software_Devel/2   8/19/08 10:25a garylin
 * PR44622: add power down/up clockgen for frontend AST0, AST1, QPSK, FTM
 *
 * Hydra_Software_Devel/1   7/7/08 4:25p garylin
 * PR44622: initial version, ported from 7325
 *
 ***************************************************************************/
#include "bstd.h"                /* standard types */
#include "berr.h"
#include "bstd_ids.h"
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"                /* malloc */

#include "bpwr.h"
#include "bpwr_mem.h"
#include "bpwr_priv.h"


BDBG_MODULE(BPWR);

BDBG_OBJECT_ID(BPWR_P_Context);

BERR_Code BPWR_GetDefaultSettings(BPWR_Settings *pDefSettings )
{
    BKNI_Memset(pDefSettings, 0, sizeof(*pDefSettings));
    return BERR_SUCCESS;
}

/***************************************************************************
 * BPWR_Open()
 *
 * Create Power Management Object
 *
 */
BERR_Code BPWR_Open
    ( BPWR_Handle                     *phPwr,
      BCHP_Handle                      hChip,
      BREG_Handle                      hRegister,
      const BPWR_Settings             *pDefSettings )
{
    BPWR_P_Context *pPwr;
    BERR_Code eStatus = BERR_SUCCESS;

    BDBG_ENTER(BPWR_Open);
    BDBG_ASSERT(hRegister);

#if 0
BDBG_SetLevel(BDBG_eMsg);
BDBG_SetModuleLevel("BPWR", BDBG_eMsg);
#endif

    /* The handle will be NULL if create fails. */
    *phPwr = NULL;

    /* check PWR settings */

    /* Alloc the main PWR context. */
    pPwr = (BPWR_P_Context*)(BKNI_Malloc(sizeof(BPWR_P_Context)));
    if(!pPwr)
    {
        BERR_TRACE(eStatus = BERR_OUT_OF_SYSTEM_MEMORY);
        goto done;
    }

    /* Clear out the context and set defaults. */
    BKNI_Memset((void*)pPwr, 0x0, sizeof(BPWR_P_Context));
    BDBG_OBJECT_SET(pPwr, BPWR_P_Context);

    /* Store the hRegister and hChip for later use. */
    pPwr->hChip      = hChip;
    pPwr->hRegister  = hRegister;
    pPwr->changeMode = BPWR_Mode_ePowerUndefined;

    /* Take in default settings. */
    if (pDefSettings) {
        pPwr->stSettings = *pDefSettings;
    }
    else {
        BPWR_GetDefaultSettings(&pPwr->stSettings);
        pDefSettings = &pPwr->stSettings;
    }

    /* Allocate and initialize module*/
    eStatus = BPWR_P_Init(pPwr);
    if (eStatus != BERR_SUCCESS)
    {
        BKNI_Free((void *)pPwr);
        goto done;
    }

    /* All done. now return the new fresh context to user. */
    *phPwr = (BPWR_Handle)pPwr;

done:
    BDBG_LEAVE(BPWR_Open);
    return eStatus;
}


/***************************************************************************
 *
 */
BERR_Code BPWR_Close
    ( BPWR_Handle                      hPwr )
{
    BDBG_ENTER(BPWR_Close);
    BDBG_OBJECT_ASSERT(hPwr, BPWR_P_Context);

    BPWR_P_Destroy(hPwr);
    BKNI_Free(hPwr);

    BDBG_LEAVE(BPWR_Close);
    return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BPWR_SetMode
    ( BPWR_Handle                      hPwr,
      BSTD_Module                      module,
      BPWR_ePowerMode                  eMode )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(hPwr, BPWR_P_Context);
    BDBG_ASSERT(eMode != BPWR_Mode_ePowerUndefined);

    if (eMode == BPWR_Mode_ePowerUndefined) {
        /* can't set to undefined */
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    else if (hPwr->changeMode == BPWR_Mode_ePowerUndefined) {
        /* this is the first Set */
        hPwr->changeMode = eMode;
    }
    else if (hPwr->changeMode != eMode) {
        /* this is an invalid mixed set */
        BDBG_ERR(("Cannot both power up and down in one apply changes set"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    for (i=0;i<BPWR_MAX_MODULES;i++) {
        if (hPwr->modules[i].moduleId == module) {
            hPwr->modules[i].change = true;
            BDBG_MSG(("BPWR_SetMode: hPwr: 0x%p %s module:%s", 
                      hPwr,
                      eMode == BPWR_Mode_ePowerUp   ?  "Power Up" :
                      eMode == BPWR_Mode_ePowerDown ?  "Power Down" :
                                                       "<undefined>"));
            return 0;
        }
    }

    BDBG_WRN(("Module %d is not supported on this chip", module));
    return -1; /* no BERR_TRACE */
}


/***************************************************************************
 *
 */
BERR_Code BPWR_GetMode
    ( const BPWR_Handle                hPwr,
      BSTD_Module                      module,
      BPWR_ePowerMode                 *peMode )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(hPwr, BPWR_P_Context);
    for(i=0; i<BPWR_MAX_MODULES; i++) {
        if (hPwr->modules[i].moduleId == module) {
            *peMode = hPwr->modules[i].mode;
            return 0;
        }
    }
    return BERR_TRACE(BERR_INVALID_PARAMETER);
}


/***************************************************************************
 *
 */
BERR_Code BPWR_GetModuleCount
    ( BPWR_Handle  hPwr,
      uint32_t    *pulModuleCount )
{
    unsigned i;

    BDBG_ENTER(BPWR_GetModuleCount);
    BDBG_OBJECT_ASSERT(hPwr, BPWR_P_Context);

    *pulModuleCount = 0;
    for(i=0; i<BPWR_MAX_MODULES; i++) {
        if (hPwr->modules[i].moduleId) {
            (*pulModuleCount)++;
        }
    }
    BDBG_WRN(("Number of modules: %d", *pulModuleCount));

    BDBG_LEAVE(BPWR_GetModuleCount);
    return 0;
}

/***************************************************************************
 *
 */
BERR_Code BPWR_GetModuleList
    ( BPWR_Handle  hPwr,
      BSTD_Module *pModuleList,
      unsigned ulModuleCount )
{
    unsigned i, total = 0;

    BDBG_ENTER(BPWR_GetModuleList);
    BDBG_OBJECT_ASSERT(hPwr, BPWR_P_Context);

    for(i=0; i<BPWR_MAX_MODULES; i++) {
        if (total == ulModuleCount) {
            return BERR_TRACE(BERR_UNKNOWN);
        }
        if (hPwr->modules[i].moduleId) {
            pModuleList[total++] = hPwr->modules[i].moduleId;
        }
    }

    BDBG_LEAVE(BPWR_GetModuleList);
    return 0;

}

/***************************************************************************
 *
 */
BERR_Code BPWR_SetModes
    ( BPWR_Handle     hPwr,
      BSTD_Module    *pModuleList,
      uint32_t        ulModuleCount,
      BPWR_ePowerMode eMode )
{
    unsigned i;

    BDBG_ENTER(BPWR_SetModes);
    BDBG_OBJECT_ASSERT(hPwr, BPWR_P_Context);

    for (i=0;i<ulModuleCount;i++)
    {
        BERR_Code rc;
        rc = BPWR_SetMode(hPwr, pModuleList[i], eMode);
        if (rc) return BERR_TRACE(rc);
    }

    BDBG_LEAVE(BPWR_SetModes);
    return 0;

}

/***************************************************************************
 * BPWR_ApplyChanges
 *
 * Validate/Apply User's new changes.
 */
BERR_Code BPWR_ApplyChanges
    ( BPWR_Handle                      hPwr )
{
    BERR_Code rc;
    unsigned i;

    BDBG_OBJECT_ASSERT(hPwr, BPWR_P_Context);

    BDBG_MSG(("BPWR_ApplyChanges: hPwr: 0x%p", hPwr));

    /* +------------------+
     * | VALIDATE CHANGES |
     * +------------------+
     * Check if modules will result in correct power up
     * or power down mode. See checklist for reference.
     * If requisite modules are not specified, return an
     * error.
     */
    rc = BPWR_P_ValidateChanges(hPwr);
    if(BERR_SUCCESS != rc)
    {
        return BERR_TRACE(rc);
    }

    /* +-------------- +
     * | APPLY CHANGES |
     * +---------------+
     * Apply power up or power down to user-specified modules.
     * Critical sections aren't necessary as all software modules are
     * are closed at this point.
     */
    rc = BPWR_P_SetPowerMode(hPwr);
    if(BERR_SUCCESS != rc)
    {
        return BERR_TRACE(rc);
    }

    /* verify that all changes were made */
    for (i=0;i<BPWR_MAX_MODULES;i++) {
        if (hPwr->modules[i].change) {
#if 1
            hPwr->modules[i].change = false;
#else
            /* this is a way of verifying that every module id listed has some PWR code associated */
            BDBG_ERR(("PWR unable to make requested change to module %d", hPwr->modules[i].moduleId));
            rc = BERR_TRACE(BERR_UNKNOWN);
#endif
        }
    }

    hPwr->changeMode = BPWR_Mode_ePowerUndefined;

    return rc;
}

