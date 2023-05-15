/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_3117.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 12/16/08 11:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/3117/bads_3117.h $
 * 
 * Hydra_Software_Devel/1   12/16/08 11:19p vishk
 * PR46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112
 * 
  * 
 ***************************************************************************/


#ifndef BADS_3117_H__
#define BADS_3117_H__

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
	BADS_3117_Open()

****************************************************************************/
BERR_Code BADS_3117_GetDefaultSettings(
	BADS_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);



#ifdef __cplusplus
}
#endif
 
#endif


