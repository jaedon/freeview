/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bthd_3461.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/1/11 3:23p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/thd/3461/bthd_3461.h $
 * 
 * Hydra_Software_Devel/1   6/1/11 3:23p atanugul
 * SW3461-5: Add THD support to 3461
 *
 ***************************************************************************/

#ifndef _BTHD_3461_H__
#define _BTHD_3461_H__				   

#include "bthd.h"
#include "breg_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif



/***************************************************************************
Summary:
	This function returns the default settings for 3461 THD module.

Description:
	This function is responsible for returns the default setting for 
	3461 THD module. The returning default setting should be used when
	opening the device.

Returns:
	TODO:

See Also:
	None.

****************************************************************************/
BERR_Code BTHD_3461_GetDefaultSettings(
	BTHD_Settings *		/* [in] Default settings */
);


/***************************************************************************
Summary:
	This function returns the default inbandParams for 3461 THD module.

Description:
	This function is responsible for returns the default setting for 
	3461 THD module. The returning default setting should be used when
	opening the device.

Returns:
	TODO:

See Also:
	None.

****************************************************************************/
BERR_Code BTHD_3461_GetDefaultInbandParams(
	BTHD_InbandParams *		/* [in] Default InbandParams */
);

#ifdef __cplusplus
}
#endif

#endif /* BTHD_3461_H__ */

