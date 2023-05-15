/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_tune_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/24/09 5:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/3548/bvdc_tune_priv.h $
 * 
 * Hydra_Software_Devel/1   11/24/09 5:36p yuxiaz
 * SW7420-462: Added new tuning functions for Tuning Database.
 * 
 ***************************************************************************/
#ifndef BVDC_TUNE_PRIV_H__
#define BVDC_TUNE_PRIV_H__

#include "bvdc_tune.h"
#include "bvdc_window_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Private macros
 ***************************************************************************/
#define BVDC_P_TUNE_GET_INTERFACE(x)             (x>>24)
#define BVDC_P_TUNE_GET_STRUCTURE(x)             ((x>>8) & 0xff)
#define BVDC_P_TUNE_GET_FIELD(x)                 (x & 0xff)

#define BVDC_P_TUNE_INTERFACE_IS_SOURCE(ulInterface) \
	((ulInterface) == BVDC_TUNE_INTERFACE_SOURCE)

#define BVDC_P_TUNE_INTERFACE_IS_WINDOW(ulInterface) \
	((ulInterface) == BVDC_TUNE_INTERFACE_WINDOW)

#define BVDC_P_TUNE_INTERFACE_IS_COMPOSITOR(ulInterface) \
	((ulInterface) == BVDC_TUNE_INTERFACE_COMPOSITOR)

#define BVDC_P_TUNE_INTERFACE_IS_DISPLAY(ulInterface) \
	((ulInterface) == BVDC_TUNE_INTERFACE_DISPLAY)

#define BVDC_P_TUNE_VALID_STRUCTURE(ulStructure)  \
	((ulStructure) == BVDC_TUNE_STRUCTURE_SHARPNESS_SETTINGS)

/***************************************************************************
 * private functions
 ***************************************************************************/

BERR_Code BVDC_P_Tune_Sharpness_Settings_GetData
	( const BVDC_SharpnessSettings          *pSharpnessSettings,
	  uint32_t                               ulField, 
	  uint32_t                              *pulData );

BERR_Code BVDC_P_Tune_Sharpness_Settings_SetData
	( BVDC_SharpnessSettings                *pSharpnessSettings,
	  uint32_t                               ulField, 
	  uint32_t                               ulData );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_FGT_PRIV_H__ */
/* End of file. */
