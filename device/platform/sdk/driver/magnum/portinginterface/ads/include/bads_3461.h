/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_3461.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 6/21/12 3:21p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/include/bads_3461.h $
 * 
 * Hydra_Software_Devel/3   6/21/12 3:21p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure; cleanup
 * 
 * Hydra_Software_Devel/2   6/18/12 6:11p atanugul
 * SW7425-2983: Update bads.inc based on the new ADS directory structure
 * 
 * Hydra_Software_Devel/1   3/16/11 3:37p atanugul
 * SW3461-3: Add support for BCM3461
 * 
 *
 ***************************************************************************/
#ifndef BADS_3461_H__
#define BADS_3461_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"
#include "bads.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
	This function returns the default settings for Qam In-Band Downstream module.

Description:
	This function is responsible for returns the default setting for 
	BADS module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BADS_Leap_Open()

****************************************************************************/
BERR_Code BADS_3461_GetDefaultSettings(
	BADS_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);

#ifdef __cplusplus
}
#endif
 
#endif



