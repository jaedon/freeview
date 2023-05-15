/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdgp.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/21/07 2:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/dgp/7405/bdgp.h $
 * 
 * Hydra_Software_Devel/1   6/21/07 2:08p nissen
 * PR 29766: Initial version of DGP module.
 *
 ***************************************************************************/
#ifndef BDGP_H__
#define BDGP_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bmem.h"
#include "bint.h"
#include "bpxl.h"
#include "bsur.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************ Module Overview ********************************
The purpose of Dedicated Graphics Port (DGP) is to feed internal graphics 
buffers to an external ASIC. This external chip will do some additional 
processing of video and will then composite the provided graphics surface 
on top. 
***************************************************************************/

/***************************************************************************
Summary:
	Main Dedicated Graphics Port (DGP) context handle.

Description:
	This is an opaque handle that the application creates with BDGP_Open.
	BDGP_Handle holds the context of the DGP and there should be only one
	BDGP_Handle per chip.

See Also:
	BDGP_Open, BDGP_Close.
***************************************************************************/
typedef struct BDGP_P_Context *BDGP_Handle;

/***************************************************************************
Summary:
	This enumeration represents device operations that can trigger an
	interrupt.

Description:
	BDGP_Trigger is an enumeration that is used when calling 
	BDGP_InstallCallback to determine which device operation will
	trigger an interrupt.

See Also:
	BDGP_InstallCallback
***************************************************************************/
typedef enum BDGP_Trigger
{
	BDGP_Trigger_Disable = 0,
	BDGP_Trigger_VSync,
	BDGP_Trigger_LineCompare,
	BDGP_Trigger_EndOfPicture
}
BDGP_Trigger;

/***************************************************************************
Summary:
	Prototype for external modules to install callback with DGP.

Description:
	The user can install a callback for a specific trigger event from
	from DGP by calling BDGP_InstallCallback.

Inpute:
	pvParam1 - User defined data structure casted to void.
	iParam2 - Additional user defined value.

Output:
	None

See Also:
	BDGP_InstallCallback
**************************************************************************/
typedef void (*BDGP_CallbackFunc_isr)(
	void *pvParam1,
	int iParam2 );

/***************************************************************************
Summary:
	This function opens and initializes the dedicated graphics port.

Description:
	The first thing the user needs to do is open DGP which will create 
	a main context handle, BDGP_Handle, which will hold the state of the
	dedicated graphics port.

Input:
	hChip - The chip handle that application created ealier during chip
	initialization sequence.  This handle is use for get chip
	information, chip revision, and miscellaneous chip configurations.

	hRegister - The register handle that application created ealier during
	chip initialization sequence.  This handle is use to access chip
	registers (DGP registers).

	hInterrupt - The level2 interrupt handle that application created
	earlier chip initialization sequence.  This handle is use to install
	level 2 interrupt callback.

Output:
	phDgp - a reference to a DGP handle.  Upon successful open this will
	reference to a fresh new DGP handle (context).  If error occur during
	BDGP_Open *phDgp be NULL.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully opened DGP.

See Also:
	BDGP_Close
**************************************************************************/
BERR_Code BDGP_Open(
	BDGP_Handle *phDgp,
	BCHP_Handle hChip,
	BREG_Handle hRegister,
	BINT_Handle hInterrupt );

/***************************************************************************
Summary:
	This function closes the dedicated graphics port.

Description:
	The user closes the DGP to free the resources allocated when it was
	opened.

Input:
	hDgp - DGP handle that was previously opened by	BDGP_Open.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully closed DGP.

See Also:
	BDGP_Open.
**************************************************************************/
BERR_Code BDGP_Close(
	BDGP_Handle hDgp );

/***************************************************************************
Summary:
	This function applies the state changes to the dedicated graphics port.

Description:
	After the user has made all required state changes to the DGP, calling 
	this function to apply the changes causes the registers to be written, 
	to update the state of the hardware.

Input:
	hDgp - DGP handle that was previously opened by BDGP_Open, 
	and setup with BDGP calls.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully updated hardware.

See Also:
	BDGP_SetSurface, BDGP_SetAlphaPremultiply, BDGP_SetPixelErrorSelect,
	BDGP_InstallCallback, BDGP_SetLineCompare
**************************************************************************/
BERR_Code BDGP_ApplyChanges(
	BDGP_Handle hDgp );

/***************************************************************************
Summary:
	This function changes the suface state for the dedicated graphics port.

Description:
	The user can change the surface used by DGP as long as the surface is 
	an ARGB8888, AP88, or P8 format, and the width is aligned to 32 bits.

Input:
	hDgp - DGP handle that was previously opened by BDGP_Open.
	hSurface - A surface previously created by the SUR module.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully updated state.

See Also:
	BDGP_ApplyChanges
**************************************************************************/
BERR_Code BDGP_SetSurface(
	BDGP_Handle hDgp, 
	BSUR_Surface_Handle hSurface );

/***************************************************************************
Summary:
	This function changes the alpha premultiply state for the 
	dedicated graphics port.

Description:
	The user can change the alpha premultiply state used by DGP, 
	determining whether pixel's color channels should be multiplied by
	the alpha channel.

Input:
	hDgp - DGP handle that was previously opened by BDGP_Open.
	bPremultiply - Enables premultied alpha when true.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully updated state.

See Also:
	BDGP_ApplyChanges
**************************************************************************/
BERR_Code BDGP_SetAlphaPremultiply(
	BDGP_Handle hDgp,
	bool bPremultiply );

/***************************************************************************
Summary:
	This function changes the pixel error select state for the 
	dedicated graphics port.

Description:
	The user can change the pixel error select state used by DGP, 
	determining whether a pixel should be repeated or a constant value
	should be used when an underflow condition occurs when reading
	pixels.

Input:
	hDgp - DGP handle that was previously opened by BDGP_Open.
	bSelectPixelValue - Enables repeating pixels when false, and 
	enables using constant value when true.
	uiPixelValue - Constant pixel value when bSelectPixelValue is true.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully updated state.

See Also:
	BDGP_ApplyChanges
**************************************************************************/
BERR_Code BDGP_SetPixelErrorSelect(
	BDGP_Handle hDgp,
	bool bSelectPixelValue,
	uint32_t uiPixelValue );

/***************************************************************************
Summary:
	This function changes the trigger callback function for the
	dedicated graphics port.

Description:
	The user can change the callback function and it's trigger for DGP. 
	The user can also included parameters that will be used when calling 
	their callback.

Input:
	hDgp - DGP handle that was previously opened by BDGP_Open.
	eTrigger - Trigger used to determine when a callback is called.
	pfCallback - User defined callback function.
	pvParm1 - User defined pointer used as parameter when calling callback.
	iParm2 - User defined integer used as parameter when calling callback.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully updated callback and trigger.

See Also:
	BDGP_ApplyChanges, BDGP_SetLineCompare
**************************************************************************/
BERR_Code BDGP_InstallCallback(
	BDGP_Handle hDgp,
	BDGP_Trigger eTrigger,
	const BDGP_CallbackFunc_isr pfCallback,
	void *pvParm1,
	int iParm2 );

/***************************************************************************
Summary:
	This function changes the line compare used for a trigger by the
	dedicated graphics port.

Description:
	The user can change the line compare mask and position when the 
	trigger is set to BDGP_Trigger_LineCompare by BDGP_InstallCallback.
	The position is AND'ed with the mask and compared with the current
	display scan out to determine when to trigger an interrupt.

Input:
	hDgp - DGP handle that was previously opened by BDGP_Open.
	uiMask - Mask that is AND'ed with position.
	uiPosition - Position that is compared with display scan out.

Returns:
	BERR_INVALID_PARAMETER - Invalid function parameters.
	BERR_SUCCESS - Successfully updated state.

See Also:
	BDGP_ApplyChanges, BDGP_InstallCallback
**************************************************************************/
BERR_Code BDGP_SetLineCompare(
	BDGP_Handle hDgp,
	uint32_t uiMask,
	uint32_t uiPosition );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_H__ */

/* End of File */
