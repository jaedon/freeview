/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btst_module.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 4/8/03 2:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/test/test_module/btst_module.c $
 * 
 * Hydra_Software_Devel/3   4/8/03 2:18p vsilyaev
 * Rename btst to btst_module
 * 
 * Hydra_Software_Devel/2   3/25/03 8:24p vsilyaev
 * Renaming error code.
 * 
 * Hydra_Software_Devel/1   3/10/03 6:30p vsilyaev
 * Test case for the base modules.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "btst_module.h"


BDBG_MODULE(test);

struct BTST_HandleImpl
{
    BREG_Handle reg;
    uint32_t dummy;
};

const static BTST_Settings default_settings =
{
     0
};

BERR_Code 
BTST_GetDefaultSettings(BTST_Settings *pDefSettings)
{
    *pDefSettings = default_settings;
    return BERR_SUCCESS;
}

BERR_Code 
BTST_Open(BTST_Handle *phTst, BCHP_Handle hChip, BREG_Handle hRegister, const BTST_Settings *pDefSettings)
{
    BTST_Handle hTst;

    hTst = BKNI_Malloc(sizeof(*hTst));
    *phTst = hTst;
    if (!hTst) {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    if (pDefSettings==NULL) {
        pDefSettings = &default_settings;
    }
    hTst->reg = hRegister;
    hTst->dummy = pDefSettings->dummy;
    return BERR_SUCCESS;
}

BERR_Code 
BTST_Reset(BTST_Handle hTst, const BTST_Settings *pDefSettings)
{
    hTst->dummy = pDefSettings->dummy;
    return BERR_SUCCESS;
}

BERR_Code 
BTST_Close(BTST_Handle hTst)
{
    BKNI_Free(hTst);
    return BERR_SUCCESS;
}

BERR_Code 
BTST_DoTest(BTST_Handle hTst)
{
    hTst->dummy++;
    if (hTst->dummy>10) {
        return BERR_TRACE(BERR_UNKNOWN);
    }
    return BERR_SUCCESS;
}
