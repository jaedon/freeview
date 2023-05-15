/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhab_3109.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/10/09 1:58p $
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/hab/3109/bhab_3109.h $
 * 
 * Hydra_Software_Devel/1   3/10/09 1:58p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * 
 ***************************************************************************/

#ifndef BHAB_3109_H
#define BHAB_3109_H

#ifdef __cplusplus
extern "C" {
#endif


#include "bhab.h"

/***************************************************************************
Summary:
	This function returns the default settings for 3109 module.

Description:
	This function is responsible for returns the default setting for 
	3109 module. The returning default setting should be used when
	opening the device.

Returns:
	TODO:

See Also:
	BSPI_Open()

****************************************************************************/
BERR_Code BHAB_3109_GetDefaultSettings(
	BHAB_Settings * pDefSetting     /* [in] Default settings */
);


#ifdef __cplusplus
}
#endif

#endif
