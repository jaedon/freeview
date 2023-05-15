/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btc2_3461.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/2/12 6:18p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tc2/3461/btc2_3461.h $
 * 
 * Hydra_Software_Devel/2   2/2/12 6:18p atanugul
 * SW3461-141: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams
 * 
 * Hydra_Software_Devel/1   3/16/11 4:14p atanugul
 * SW3461-3: Add support for BCM3461
 * 
 *
 ***************************************************************************/


#ifndef BTC2_3461_H__
#define BTC2_3461_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"
#include "btc2.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
	This function returns the default settings for Qam In-Band Downstream module.

Description:
	This function is responsible for returns the default setting for 
	BTC2 module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BTC2_3461_Open()

****************************************************************************/
BERR_Code BTC2_3461_GetDefaultSettings(
	BTC2_Settings *pDefSettings,		/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);

/***************************************************************************
Summary:
	This function returns the default settings for Qam In-Band Downstream module.

Description:
	This function is responsible for returns the default setting for 
	BTC2 module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BTC2_3461_Open()

****************************************************************************/
BERR_Code BTC2_3461_GetDefaultAcquireParams(
    BTC2_InbandParam *ibParams          /* [out] default Inband Parameters */
    );
    

#ifdef __cplusplus
}
#endif
 
#endif



