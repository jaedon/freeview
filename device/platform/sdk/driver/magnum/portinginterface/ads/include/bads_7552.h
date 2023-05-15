/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_7552.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/22/12 12:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/include/bads_7552.h $
 * 
 * Hydra_Software_Devel/1   6/22/12 12:37p farshidf
 * SW7425-2983: 7550 and 7552 ADS update
 * 
 * Hydra_Software_Devel/1   10/10/11 2:05p farshidf
 * SW7552-134: sync up to V2_0 of 3461
 * 
 * Hydra_Software_Devel/1   4/20/11 12:39p farshidf
 * SWDTV-6190: add intial DS files
 *
 ***************************************************************************/


#ifndef BADS_7552_H__
#define BADS_7552_H__

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
	BADS_7517_Open()

****************************************************************************/
BERR_Code BADS_7552_GetDefaultSettings(
	BADS_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);



#ifdef __cplusplus
}
#endif
 
#endif

