/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_3128.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 6/21/12 3:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/include/bads_3128.h $
 * 
 * Hydra_Software_Devel/4   6/21/12 3:21p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure; cleanup
 * 
 * Hydra_Software_Devel/3   6/18/12 5:15p atanugul
 * SW7425-2983: Remove symbolic links and flatten out ADS directory
 * structure
 * 
 * Hydra_Software_Devel/2   2/8/12 5:30p atanugul
 * SW3128-119: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams; dump 3128 registers in case of HAB
 * timeout
 * 
 * Hydra_Software_Devel/1   2/1/11 7:45a atanugul
 * SW3128-2: Add Support for BCM3128
 *
 ***************************************************************************/


#ifndef BADS_3128_H__
#define BADS_3128_H__

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
	BADS_3128_Open()

****************************************************************************/
BERR_Code BADS_3128_GetDefaultSettings(
	BADS_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
    );

#ifdef __cplusplus
}
#endif
 
#endif



