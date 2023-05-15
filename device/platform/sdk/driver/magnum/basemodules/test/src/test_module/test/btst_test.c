/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btst_test.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 4/8/03 2:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/test/test_module/test/btst_test.c $
 * 
 * Hydra_Software_Devel/4   4/8/03 2:18p vsilyaev
 * Remane btst to btst_module
 * 
 * Hydra_Software_Devel/3   4/1/03 9:42a vsilyaev
 * Added breg_mem.h file.
 * 
 * Hydra_Software_Devel/2   3/28/03 6:51p vsilyaev
 * Unititalize BDBG before BKNI.
 * 
 * Hydra_Software_Devel/1   3/10/03 6:32p vsilyaev
 * Test case for the test module.
 * 
 *
 ***************************************************************************/


#include "bstd.h"
#include "bkni.h"
#include "btst_module.h"
#include "breg_mem.h"
#include "btst.h"

BDBG_MODULE(test_module_test);

BERR_Code 
BTST_EntryPoint(const BTST_TestContext *context)
{
    BCHP_Handle hChip = context->hChip;
    BREG_Handle hRegister = context->hRegister;
    BTST_Handle hTst;
    BERR_Code rc;
    BTST_Settings settings;

    rc = BTST_Open(&hTst, hChip, hRegister, NULL);
    if (rc!=BERR_SUCCESS) {
        goto done;
    }

    rc = BTST_DoTest(hTst);
    if (rc!=BERR_SUCCESS) {
        BDBG_WRN(("First BTST_DoTest Failed rc=%d", (int)rc));
    }

    BKNI_Delay(1000*1000);

    BTST_GetDefaultSettings(&settings);
    settings.dummy = 100;
    rc = BTST_Reset(hTst, &settings);
    if (rc!=BERR_SUCCESS) {
        BDBG_WRN(("BTST_Reset Failed"));
    }

    rc = BTST_DoTest(hTst);
    if (rc!=BERR_SUCCESS) {
        BDBG_WRN(("Second BTST_DoTest Failed rc=%d", (int)rc));
    }

    BKNI_Sleep(1000);

    rc = BTST_Close(hTst);

done:
    return rc;
}
