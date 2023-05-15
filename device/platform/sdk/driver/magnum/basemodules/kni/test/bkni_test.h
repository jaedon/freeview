/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_test.h $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 4/3/03 6:13p $
 *
 * Module Description: Cross-platform kernel inteface conformance test
 *
 * Revision History:
 *
 * $brcm_Log: /vobs/magnum/basemodules/kernelinterface/test/bkni_test.h $
 * 
 * Hydra_Software_Devel/10   4/3/03 6:13p erickson
 * some pre 0.9 api rework
 * 
 * Hydra_Software_Devel/9   4/3/03 3:57p erickson
 * linuxkernel work
 *
 * Hydra_Software_Devel/8   3/31/03 4:56p dlwin
 * Remove task test.
 * 
 * Hydra_Software_Devel/7   3/27/03 8:58a dlwin
 * Added more printout statements
 * 
 * Hydra_Software_Devel/6   3/25/03 10:16a dlwin
 * Added more testing functions.
 * 
 * Hydra_Software_Devel/5   3/17/03 7:34p vsilyaev
 * Fixed code to match standards.
 * 
 * Hydra_Software_Devel/4   3/12/03 3:50p erickson
 * updated tests and got linuxkernel working with them
 * 
 * Hydra_Software_Devel/3   3/10/03 12:36p erickson
 * linuxkernel tasks initial implementationd
 *
 * Hydra_Software_Devel/2   3/7/03 5:20p erickson
 * linux kernel interface work
 *
 * Hydra_Software_Devel/1   3/5/03 5:16p erickson
 * Initial kernelinterface work
 *
 ***************************************************************************/
#ifndef BKNI_TEST_H__
#define BKNI_TEST_H__

BERR_Code BKNI_TestAll(void);

BERR_Code BKNI_TestInit(void);
BERR_Code BKNI_TestMemoryFunctions(void);
BERR_Code BKNI_TestMemoryFunctions2(void);
BERR_Code BKNI_TestPrintFunctions(void);
BERR_Code BKNI_TestMallocAndFree(void);
BERR_Code BKNI_TestSleepAndDelay(void);
BERR_Code BKNI_TestEvents(void);
BERR_Code BKNI_TestEventWithTask(void);
BERR_Code BKNI_TestMutexes(void);
BERR_Code BKNI_TestMutexWithTask(void);

#endif /* BKNI_TEST_H__ */
