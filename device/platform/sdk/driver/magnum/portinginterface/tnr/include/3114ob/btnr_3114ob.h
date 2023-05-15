/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3114ob.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/29/09 1:28p $
 *
 * Module Description: $
 *
 * Revision History:  $
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3114ob/btnr_3114ob.h $
 * 
 * Hydra_Software_Devel/2   6/29/09 1:28p vishk
 * PR 56235: Add support to set IfFreq in AOB PI for 3117
 * 
 * Hydra_Software_Devel/1   3/11/09 4:21p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112

 ***************************************************************************/
#ifndef BTNR_3114OB_H__
#define BTNR_3114OB_H__

#include "bchp.h"
#include "breg_i2c.h"
#include "bhab.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	unsigned short i2cAddr;				/* 7bit I2C address of Bcm3114 */
	unsigned ifFreq;
} BTNR_3114Ob_Settings;

/***************************************************************************
Summary:
	This function returns the default settings for Bcm3114 Tuner module.

Description:
	This function is responsible for returns the default setting for 
	BTNR module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BTNR_3114_Open()

****************************************************************************/
BERR_Code BTNR_3114Ob_GetDefaultSettings(
	BTNR_3114Ob_Settings *pDefSettings	/* [out] Returns default setting */
	);


/***************************************************************************
Summary:
	This function opens Bcm3114 Tuner module.

Description:
	This function is responsible for opening Bcm3114 BTNR module. When BTNR is
	opened, it will create a module handle and configure the module based
	on the default settings. Once the device is opened, it must be closed
	before it can be opened again.

Returns:
	TODO:

See Also:
	BTNR_Close(), BTNR_3114_GetDefaultSettings()

****************************************************************************/
BERR_Code BTNR_3114Ob_Open(
	BTNR_Handle *phDev,					/* [out] Returns handle */
	BTNR_3114Ob_Settings *pSettings,
	BHAB_Handle hHab
	);

#ifdef __cplusplus
}
#endif
 
#endif



