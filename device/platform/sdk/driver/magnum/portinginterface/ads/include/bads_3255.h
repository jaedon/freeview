/***************************************************************************
 *     Copyright (c) 2005-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_3255.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 5/12/06 5:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/3255/bads_3255.h $
 * 
 * Hydra_Software_Devel/3   5/12/06 5:31p haisongw
 * PR 20984: move private APIs bads_3255.h into bads_3255_priv.h
 * 
 * Hydra_Software_Devel/2   4/11/06 4:53p dlwin
 * PR 19978: Merge 3255 support.
 * 
 * Hydra_Software_Devel/PROD_DSG_REFSW_Devel/1   4/3/06 4:19p haisongw
 * PR 19978:add  tuner Settop API support on BCM97455 platform
 * 
 * Hydra_Software_Devel/1   12/13/05 2:58p dlwin
 * PR 18598: Add new ADS for 3255
 * 
 ***************************************************************************/


#ifndef BADS_3255_H__
#define BADS_3255_H__

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
	BADS_3255_Open()

****************************************************************************/
BERR_Code BADS_3255_GetDefaultSettings(
	BADS_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);


#ifdef __cplusplus
}
#endif
 
#endif



