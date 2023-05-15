/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: vdec_input.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 7/9/12 5:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/vdec_input.h $
 * 
 * Hydra_Software_Devel/6   7/9/12 5:29p vanessah
 * SW7425-2501: 3548/3556 support removal
 * 
 * Hydra_Software_Devel/5   6/17/08 7:36p pntruong
 * PR40922: Fixed build errors.
 *
 * Hydra_Software_Devel/4   6/17/08 7:20p pntruong
 * PR40922: Some more tuning for SCART input.
 *
 * Hydra_Software_Devel/3   5/16/08 3:08p pntruong
 * PR34711: Updated to include new inputs.
 *
 * Hydra_Software_Devel/2   10/16/07 2:47p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 * Hydra_Software_Devel/1   9/27/07 4:13p rpan
 * PR34857: Copied over as is from mvd_vdc_test.
 *
 ***************************************************************************/
#ifndef VDEC_INPUT_H__
#define VDEC_INPUT_H__

#include "bvdc.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	BTST_VdecInput_eIfd_0 = 0,  /* Internal IFD */
	BTST_VdecInput_eIfd_1,      /* External IFD connect to CVBS5 */
	BTST_VdecInput_eCvbs_0,
	BTST_VdecInput_eCvbs_1,
	BTST_VdecInput_eCvbs_2,
	BTST_VdecInput_eCvbs_3,
	BTST_VdecInput_eSvid_0,
	BTST_VdecInput_eSvid_1,
	BTST_VdecInput_eSvid_2,
	BTST_VdecInput_eYPrPb_0,
	BTST_VdecInput_eYPrPb_1,
	BTST_VdecInput_eYPrPb_2,
	BTST_VdecInput_eRgb_0,
	BTST_VdecInput_eRgb_1,
	BTST_VdecInput_eRgb_2,
	BTST_VdecInput_ePcIn_0,

	/* Max: Must be last */
	BTST_VdecInput_eMax

} BTST_VdecInput;


/***************************************************************************
 * Print the available Vdec inputs
 *
 */
void BTST_PrintVdecInputMenu
	( void );

/***************************************************************************
 * Select an VDEC input!  User still need to call BVDC_ApplyChanges to
 * commit the changes.
 *
 */
void BTST_SelectVdecInput
	( BVDC_Source_Handle               hVidSrc,
	  BTST_VdecInput                   eVdecInput );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VDEC_INPUT_H__ */

/* end of file */
