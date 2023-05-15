/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bods_3462.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 5/25/12 2:41p $
 *
 * Module Description:
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ods/include/bods_3462.h $
 * 
 * Hydra_Software_Devel/1   5/25/12 2:41p atanugul
 * SW3462-7: Initial Version
 * 
 ***************************************************************************/
#ifndef BODS_3462_H__
#define BODS_3462_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"
#include "bods.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	This function returns the default settings for Qam In-Band Downstream module.

Description:
	This function is responsible for returns the default setting for 
	BODS module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BODS_34xx_Open()

****************************************************************************/
BERR_Code BODS_3462_GetDefaultSettings(
	BODS_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);

#ifdef __cplusplus
}
#endif
 
#endif
