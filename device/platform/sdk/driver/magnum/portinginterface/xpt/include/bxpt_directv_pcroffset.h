/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_directv_pcroffset.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/25/10 2:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_directv_pcroffset.h $
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/1   10/8/10 2:57p gmullen
 * SW7425-15: Added header files
 * 
 * Hydra_Software_Devel/1   9/6/05 5:12p gmullen
 * PR15309: Added support for DirecTV in PCR offset block.
 * 
 * 
 ***************************************************************************/

#ifndef BXPT_DIRECTV_PCR_OFFSET_H__
#define BXPT_DIRECTV_PCR_OFFSET_H__

#include "bxpt_directv_pcr.h"
#include "bxpt_pcr_offset.h"

#ifdef __cplusplus
extern "C"{
#endif

/*=************************ Module Overview ********************************
The API module provides the DirecTV only APIs for the PCR Offset module. 
***************************************************************************/

/***************************************************************************
Summary:
Set the stream type used by a PCR Offset module.
	 
Description: 
This function sets mode for a given PCR module to DirecTV, or back to the
default MPEG.

Returns:
    BERR_SUCCESS                - Directv mode set 
    BERR_INVALID_PARAMETER      - Bad input parameter  
***************************************************************************/
BERR_Code BXPT_DirecTv_PcrOffset_SetPcrMode( 
	BXPT_PcrOffset_Handle hChannel,		 /* [in] The channel handle */
	BXPT_PcrMode Mode
	);

#ifdef __cplusplus
}
#endif

#endif

