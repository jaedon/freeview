/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_dotest.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 4/4/03 6:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kernelinterface/test/bkni_dotest.c $
 * 
 * Hydra_Software_Devel/4   4/4/03 6:35p vsilyaev
 * Fixed test case.
 * 
 * Hydra_Software_Devel/3   3/31/03 4:56p dlwin
 * Remove task test.
 * 
 * Hydra_Software_Devel/2   3/25/03 10:15a dlwin
 * Added more testing functions.
 * 
 * Hydra_Software_Devel/1   3/17/03 7:35p vsilyaev
 * File to run KernelInterface tests.
 * 
***************************************************************************/

#include "bstd.h"
#include "bkni.h"

#include "bkni_test.h"


#define DO_TEST(test) do { rc = test ; if (rc!=BERR_SUCCESS) {BKNI_Printf("Test %s failed rc=%d\n", #test, rc); goto failure;}}while(0)

int main(void)
{
    BERR_Code rc;

    DO_TEST(BKNI_Init());
    DO_TEST(BDBG_Init());

    DO_TEST(BKNI_TestAll());

    BDBG_Uninit();
    BKNI_Uninit();
    BKNI_Printf("Kernel test passed\n");  

    return 0;

failure:
    return 1;
}


