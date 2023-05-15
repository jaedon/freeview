/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmem_test.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/26/04 6:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/mem/test/bmem_test.h $
 * 
 * Hydra_Software_Devel/2   5/26/04 6:59p hongtaoz
 * PR10059: sorted the freed heap in size ascending order; fixed compile
 * error for bmem test; dump heap map before destroying the heap for bmem
 * test;
 * 
 * Hydra_Software_Devel/1   9/17/03 9:36a jasonh
 * Initial version.
 *
 ***************************************************************************/

#ifndef BMEM_TEST_H__
#define BMEM_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

void BMEM_Test_DisplayConfig(
	void );

BERR_Code BMEM_Test_DoTest1(
	void );

#ifdef __cplusplus
}
#endif

#endif /* BMEM_TEST_H__ */

/* End of file */
