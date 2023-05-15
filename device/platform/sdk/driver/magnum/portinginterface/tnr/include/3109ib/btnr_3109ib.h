/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3109ib.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/11/09 3:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: //fs-irva-74/stbdev/vishk/win_views/nexus_latest/magnum/portinginterface/tnr/3109ib/btnr_3109ib.h $
 * 
 * Hydra_Software_Devel/1   3/11/09 3:19p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/1   3/3/09 12:15p vishk
 * PR46431: TNR PI, Add support for BCM3117, BCM3114, and BCM3112
 * 
 * Hydra_Software_Devel/1   12/16/08 11:33p vishk
 * PR46431: TNR PI, Add support for BCM3117, BCM3114, and BCM3112
 * 
  * 
 ***************************************************************************/
#ifndef BTNR_3109IB_H__
#define BTNR_3109IB_H__

#include "bchp.h"
#include "breg_i2c.h"
#include "bhab.h"
#include "btnr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	unsigned short i2cAddr;				/* 7bit I2C address of Bcm3109 */
} BTNR_3109Ib_Settings;

/***************************************************************************
Summary:
	This function returns the default settings for Bcm3109 Tuner module.

Description:
	This function is responsible for returns the default setting for 
	BTNR module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BTNR_3109_Open()

****************************************************************************/
BERR_Code BTNR_3109Ib_GetDefaultSettings(
	BTNR_3109Ib_Settings *pDefSettings	/* [out] Returns default setting */
	);


/***************************************************************************
Summary:
	This function opens Bcm3109 Tuner module.

Description:
	This function is responsible for opening Bcm3109 BTNR module. When BTNR is
	opened, it will create a module handle and configure the module based
	on the default settings. Once the device is opened, it must be closed
	before it can be opened again.

Returns:
	TODO:

See Also:
	BTNR_Close(), BTNR_3109_GetDefaultSettings()

****************************************************************************/
BERR_Code BTNR_3109Ib_Open(
	BTNR_Handle *phDev,					/* [out] Returns handle */
	BTNR_3109Ib_Settings *pSettings,
	BHAB_Handle hHab
	);

#ifdef __cplusplus
}
#endif
 
#endif



