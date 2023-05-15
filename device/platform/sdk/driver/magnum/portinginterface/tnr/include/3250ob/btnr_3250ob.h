/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3250ob.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/7/03 8:53a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3250ob/btnr_3250ob.h $
 * 
 * Hydra_Software_Devel/5   10/7/03 8:53a dlwin
 * Removed the need to call a create generic tuner  handle.
 * 
 * Hydra_Software_Devel/4   9/29/03 5:09p dlwin
 * Fixed a 'sgen' error/warning.
 * 
 * Hydra_Software_Devel/3   9/9/03 8:44a dlwin
 * Updated API function parameter comment for DocJet.
 * 
 * Hydra_Software_Devel/2   9/3/03 9:56a dlwin
 * Added default settings using '#define'.
 * 
 * Hydra_Software_Devel/1   8/26/03 2:21p dlwin
 * Initial version
 * 
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview
This module represents the implementation of Bcm3250Ob (OutOfBand) tuner.
It is designed as a subdevice of BTNR device.  It contains the 'Custom' part
of a tuner module.  The 'General' part is implemented in BTNR.  Please see
BTNR documentation for a detail discussion on a tuner and BTNR.


Design
The design for BTNR_3250Ob PI API is broken into two parts.

o Part 1 (open/close):

    These APIs are used for opening and closing a specific BTNR_3250Ob device.

o Part 2 (create):

	This section is an API to create a general BTNR device handle from
	Bcm3250Ob specific BTNR_3250Ob device handle.


Usage
The usage of BTNR_3250Ob involves the following:

   * Configure/Open of BTNR_3250Ob

      * Configure BTNR_3250Ob device for the target system
      * Open BTNR_3250Ob device
	  * Using Get/Set/Close functions



Interrupt Requirements:
None


Sample Code
Please see BTNR documentation for sample code.

</verbatim>
***************************************************************************/

#ifndef BTNR_3250OB_H__
#define BTNR_3250OB_H__

#include "bchp.h"
#include "bqob.h"

#ifdef __cplusplus
extern "C" {
#endif



/***************************************************************************
Summary:
	Required default settings structure for OutOfBand Bcm3250 Tuner module.

Description:
	The default setting structure defines the default configuration of
	OutOfBand Bcm3250 tuner when the device is opened.

See Also:
	BTNR_3250Ob_Open(), BTNR_3250Ob_GetDefaultSettings()

****************************************************************************/
#define	BTNR_3250OB_SETTINGS_IFFREQ			(44000000)		/* 44.00 MHz */
typedef struct
{
	unsigned long ifFreq;				/* in Hertz */
} BTNR_3250Ob_Settings;


/***************************************************************************
Summary:
	This function opens OutOfBand Bcm3250 Tuner module.

Description:
	This function is responsible for opening OutOfBand Bcm3250 BTNR module.
	When BTNR is opened, it will create a module handle and configure the
	module based on the default settings. Once the device is opened, it
	must be closed before it can be opened again.

	Note: This function requires BQOB_Handle, which is used to
	communicate to Bcm3250 OutOfBand core.  Access to BQOB core will
	be serialized by BTNR_3250Ob.

Returns:
	TODO:

See Also:
	BTNR_Close(), BTNR_3250Ob_GetDefaultSettings()

****************************************************************************/
BERR_Code BTNR_3250Ob_Open(
	BTNR_Handle *phDev,					/* [out] Returns handle */
	BCHP_Handle hChip,					/* [in] Chip handle */
	BQOB_Handle hQob,					/* [in] QOB handle for Bcm3250 */
	const BTNR_3250Ob_Settings *pDefSettings /* [in] Default settings */
	);

/***************************************************************************
Summary:
	This function returns the default settings for OutOfBand Bcm3250 Tuner module.

Description:
	This function is responsible for returns the default setting for 
	BTNR module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BTNR_3250Ob_Open()

****************************************************************************/
BERR_Code BTNR_3250Ob_GetDefaultSettings(
	BTNR_3250Ob_Settings *pDefSettings,	/* [out] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	);



#ifdef __cplusplus
}
#endif
 
#endif



