/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btst.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 7/8/03 3:06p $
 *
 * Module Description:
 *
 * O/S Specific entry point to the unit tests
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/test/generic/btst.c $
 * 
 * Hydra_Software_Devel/3   7/8/03 3:06p vsilyaev
 * Replace BREG_CreateRegHanlde with BREG_Open
 * 
 * Hydra_Software_Devel/2   4/8/03 7:21p vsilyaev
 * Use function to create handle.
 * 
 * Hydra_Software_Devel/1   4/8/03 2:13p vsilyaev
 * Helper functions for the unit tests.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "btst.h"

static BERR_Code 
do_test(const BTST_TestContext *context)
{
    BTST_TEST_API(BTST_EntryPoint(context));
    return BERR_SUCCESS;
}


int 
main(void)
{
    BTST_TestContext context;
    BERR_Code rc;

    context.hChip = NULL;
	BREG_Open(&context.hRegister, 0, 0);
    context.hIntr = NULL;

    BTST_TEST_CRITICAL(BKNI_Init()==BERR_SUCCESS);
    BTST_TEST_CRITICAL(BDBG_Init()==BERR_SUCCESS);

    rc = do_test(&context);

    BDBG_Uninit();
    BKNI_Uninit();
    if (rc==BERR_SUCCESS) {
        BKNI_Printf("test passed\n");  
        return 0;
    } else {
        BKNI_Printf("test failed\n");  
        return 1;
    }

}
