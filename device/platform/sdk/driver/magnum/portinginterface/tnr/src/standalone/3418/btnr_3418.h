/***************************************************************************
 *     Copyright (c) 2003-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3418.h $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 6/7/05 8:36a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3418/btnr_3418.h $
 * 
 * Hydra_Software_Devel/9   6/7/05 8:36a dlwin
 * PR 14682: Added additional functions for Docsis use.  Also added
 * function to return additional information about the tuner.
 * 
 * Hydra_Software_Devel/8   12/30/03 3:11p dlwin
 * PR 9155: Added support for configuring the Xtal Clock Freq. for Bcm3418
 * tuner.
 * 
 * Hydra_Software_Devel/7   10/8/03 11:19a dlwin
 * Converted I2C address to use 7bit instead of 8bit.
 * 
 * Hydra_Software_Devel/6   10/7/03 8:53a dlwin
 * Removed the need to call a create generic tuner  handle.
 * 
 * Hydra_Software_Devel/5   9/29/03 4:54p dlwin
 * Fixed a 'sgen' error/warning.
 * 
 * Hydra_Software_Devel/4   9/14/03 2:22p dlwin
 * Replaced TRUE with true and FALSE with flase
 * 
 * Hydra_Software_Devel/3   9/9/03 8:44a dlwin
 * Updated API function parameter comment for DocJet.
 * 
 * Hydra_Software_Devel/2   9/3/03 9:57a dlwin
 * Added default settings using '#define'.
 * 
 * Hydra_Software_Devel/1   8/26/03 2:20p dlwin
 * Initial version
 * 
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview
This module represents the implementation of Bcm3418 tuner.  It is 
designed as a subdevice of BTNR device.  It contains the 'Custom' part
of a tuner module.  The 'General' part is implemented in BTNR.  Please see
BTNR documentation for a detail discussion on a tuner and BTNR.


Design
The design for BTNR_3418 PI API is broken into two parts.

o Part 1 (open/close):

    These APIs are used for opening and closing a specific BTNR_3418 device.

o Part 2 (create):

	This section is an API to create a general BTNR device handle from
	Bcm3418 specific BTNR_3418 device handle.


Usage
The usage of BTNR_3418 involves the following:

   * Configure/Open of BTNR_3418

      * Configure BTNR_3418 device for the target system
      * Open BTNR_3418 device
	  * Using Get/Set/Close functions


Interrupt Requirements:
None


Sample Code
Please see BTNR documentation for sample code.

</verbatim>
***************************************************************************/


#ifndef BTNR_3418_H__
#define BTNR_3418_H__

#include "bchp.h"
#include "breg_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
	Required default settings structure for Bcm3418 Tuner module.

Description:
	The default setting structure defines the default configuration of
	Bcm3418 tuner when the device is opened.

See Also:
	BTNR_3418_Open(), BTNR_3418_GetDefaultSettings()

****************************************************************************/
#define	BTNR_3418_SETTINGS_IFFREQ			(43750000)		/* 43.75 MHz */
#define	BTNR_3418_SETTINGS_I2CADDR			(0x65)			/* 7bit addr */
#define BTNR_3418_SETTINGS_XTALFREQ			(24000000)		/* 24.00 MHz */
#define	BTNR_3418_SETTINGS_ENABLEAGGAIN		(false)
typedef struct
{
	unsigned long ifFreq;				/* in Hertz */
	unsigned long xtalFreq;				/* in Hertz */
	unsigned short i2cAddr;				/* 7bit I2C address of Bcm3418 */
	bool enableAgcGain;					/* 1=Enable AGC Gain, otherwise disabled */
} BTNR_3418_Settings;


/***************************************************************************
Summary:
	This function opens Bcm3418 Tuner module.

Description:
	This function is responsible for opening Bcm3418 BTNR module. When BTNR is
	opened, it will create a module handle and configure the module based
	on the default settings. Once the device is opened, it must be closed
	before it can be opened again.

Returns:
	TODO:

See Also:
	BTNR_Close(), BTNR_3418_GetDefaultSettings()

****************************************************************************/
BERR_Code BTNR_3418_Open(
	BTNR_Handle *phDev,					/* [out] Returns handle */
	BCHP_Handle hChip,					/* [in] Chip handle */
	BREG_I2C_Handle hI2CReg,			/* [in] I2C Register handle */
	const BTNR_3418_Settings *pDefSettings /* [in] Default settings */
	);

/***************************************************************************
Summary:
	This function returns the default settings for Bcm3418 Tuner module.

Description:
	This function is responsible for returns the default setting for 
	BTNR module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BTNR_3418_Open()

****************************************************************************/
BERR_Code BTNR_3418_GetDefaultSettings(
	BTNR_3418_Settings *pDefSettings,	/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);

/* Private functions for Docsis use: Do not for general use, therefore not documented */
void BTNR_3418_SetIf1AgcForceMode(
	BTNR_Handle hDev					/* [in] Device handle */
	);

/* Private functions for Docsis use: Do not for general use, therefore not documented */
void BTNR_3418_SetIf1AgcAutoMode(
	BTNR_Handle hDev					/* [in] Device handle */
	);


#ifdef __cplusplus
}
#endif
 
#endif



