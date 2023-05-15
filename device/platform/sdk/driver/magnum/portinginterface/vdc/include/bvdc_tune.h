/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_tune.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/24/09 5:36p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/3548/bvdc_tune.h $
 * 
 * Hydra_Software_Devel/1   11/24/09 5:36p yuxiaz
 * SW7420-462: Added new tuning functions for Tuning Database.
 * 
 ***************************************************************************/
#ifndef BVDC_TUNE_H__
#define BVDC_TUNE_H__

#include "bvdc.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Public defines
 ***************************************************************************/
/* macro to create unique field ID: 0xII00SSFF
 *  I = INTERFACE (REQUIRED)
 *  S = STRUCTURE 
 *  F = FIELD */
#define BVDC_TUNE_FIELD_ID(I, S, F) (((I)<<24) | ((S)<<8) | (F))

/* Interfaces (values must be unique across all magnum modules) */
#define BVDC_TUNE_INTERFACE_SOURCE                (0x00)
#define BVDC_TUNE_INTERFACE_WINDOW                (0x01)
#define BVDC_TUNE_INTERFACE_COMPOSITOR            (0x02)
#define BVDC_TUNE_INTERFACE_DISPLAY               (0x03)

/* Structure */
#define BVDC_TUNE_STRUCTURE_SHARPNESS_SETTINGS    (0x00)

/***************************************************************************
Summary:
	Used to set tuning parameters.

Description:

Input:
	hWindow - A valid window handle created earlier.
	aFields - Array of fields to set
	aValues - Array of values to use with the fields
	num -Number of fields and values
	
Output:
	
See Also:
	BVDC_Window_GetTune, BVDC_Window_ResetTune
***************************************************************************/
BERR_Code BVDC_Window_SetTuneParam
	( BVDC_Window_Handle               hWindow,
	  const uint32_t                   aFields[], 
	  const uint32_t                   aValues[], 
	  int                              num );

/***************************************************************************
Summary:
	Used to get tuning parameters.

Description:

Input:
	hWindow - A valid window handle created earlier.
	aFields - Array of fields to set
	num -Number of fields and values
	
Output:
	aValues - Returned array of values for each field
	
See Also:
	BVDC_Window_SetTune, BVDC_Window_ResetTune
***************************************************************************/
BERR_Code BVDC_Window_GetTuneParam
	( const BVDC_Window_Handle         hWindow,
	  const uint32_t                   aFields[], 
	  uint32_t                         aValues[], 
	  int                              num );

/***************************************************************************
Summary:
	Used to reset all tuning parameters for the window.

Description:

Input:
	hWindow - A valid window handle created earlier.
	
Output:
	
See Also:
	BVDC_Window_SetTune, BVDC_Window_ResetTune
***************************************************************************/
BERR_Code BVDC_Window_ResetTuneParam
	( const BVDC_Window_Handle         hWindow );



#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_TUNE_H__ */

/* End of File */

