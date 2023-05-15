/***************************************************************************
 *     Copyright (c) 2002-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mvd_test.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/16/07 2:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/mvd_test.h $
 * 
 * Hydra_Software_Devel/2   10/16/07 2:46p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 * 
 * Hydra_Software_Devel/1   9/27/07 4:13p rpan
 * PR34857: Copied over as is from mvd_vdc_test.
 * 
 * 
 ***************************************************************************/

#ifndef MVD_TEST_H__
#define MVD_TEST_H__

#include "bvdc.h"
#include "bmvd.h"

#define TestError(e, str)	{\
	err = e;\
	if (err != BERR_SUCCESS)\
	{\
	BDBG_ERR(( str". %s: %d", __FILE__, __LINE__ ));\
		goto Done;\
	}\
}

BERR_Code mvd_test
(	BREG_Handle 				hReg, 
	BMEM_Handle 				hMem,
	BINT_Handle 				hInt, 
	BMVD_Handle 			hMvd,
	BMVD_ChannelHandle		hMpgChn,
	BVDC_Handle 			hVdc,
	BVDC_Display_Handle 		hDisplay0, 
	BVDC_Window_Handle 		hMpegWindow0, 
	BVDC_Source_Handle 		hMpegSource0,
	BCHP_Handle 				hChip);

#endif /* MVD_TEST_H__ */

/* end of file */
