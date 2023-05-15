/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab_3462.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/10/12 3:00p $
 *
 * Module Description:
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/3462/bhab_3462.h $
 * 
 * Hydra_Software_Devel/1   4/10/12 3:00p atanugul
 * SW3462-4: Add HAB support for 3462
 * 
 ***************************************************************************/
#ifndef BHAB_3462_H
#define BHAB_3462_H

#ifdef __cplusplus
extern "C" {
#endif


#include "bhab.h"

/***************************************************************************
Summary:
	This function returns the default settings for 3462 module.

Description:
	This function is responsible for returns the default setting for 
	3462 module. The returning default setting should be used when
	opening the device.

Returns:
	TODO:

See Also:
	BSPI_Open()

****************************************************************************/
BERR_Code BHAB_3462_GetDefaultSettings(
	BHAB_Settings * pDefSetting     /* [in] Default settings */
);


#ifdef __cplusplus
}
#endif

#endif
