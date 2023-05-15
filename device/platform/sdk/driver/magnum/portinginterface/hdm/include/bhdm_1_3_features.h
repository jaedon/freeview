/***************************************************************************
 *	   (c)2007-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").	Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.	   This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *	2.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bhdm_1_3_features.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/18/12 1:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/include/bhdm_1_3_features.h $
 * 
 * Hydra_Software_Devel/1   6/18/12 1:06p vle
 * SW7425-2951: Update HDM PI directory structure
 * 
 ***************************************************************************/


#ifndef BHDM_1_3_FEATURES_H__
#define BHDM_1_3_FEATURES_H__

#include "bhdm.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
HDMI settings used for setting HDMI color mode (standard and deep color modes)

See Also
	o BHDM_SetBitsPerPixel
****************************************************************************/
typedef struct _BHDM_ColorDepth_Settings
{
	BAVC_HDMI_BitsPerPixel eBitsPerPixel;
} BHDM_ColorDepth_Settings;


/******************************************************************************
Summary:
	Set output color mode (standard or deep color)

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	stColorDepthSettings - color mode configuration to set
	
	
Returns:
	BERR_SUCCESS - color depth mode successfully set
	BERR_INVALID_PARAMETER - Invalid function parameter.
	BERR_NOT_SUPPORTED - requested color depth is not supported
	
*******************************************************************************/
BERR_Code BHDM_SetColorDepth( 
	BHDM_Handle hHDMI,		   /* [in] HDMI handle */
	BHDM_ColorDepth_Settings *stColorDepthSettings
) ;


 
/******************************************************************************
Summary:
	Get current output color mode (standard or deep color)

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	stColorDepthSettings - the current color mode configuration 
	
	
Returns:
	BERR_SUCCESS - color depth mode successfully set
*******************************************************************************/
BERR_Code BHDM_GetColorDepth(
   BHDM_Handle hHDMI,		   /* [in] HDMI handle */
   BHDM_ColorDepth_Settings *stColorDepthSettings /* [out] color depth setting returns */
);


/******************************************************************************
Summary:
	Set pixel data override

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.

	
Returns:
	BERR_SUCCESS - override was successful
	BERR_INVALID_PARAMETER - Invalid function parameter.
	
*******************************************************************************/
BERR_Code BHDM_SetPixelDataOverride(
	BHDM_Handle hHDMI,		   /* [in] HDMI handle */
	uint8_t red,					/* R (Cr) */
	uint8_t green,					/* G (Y)  */
	uint8_t blue					/* B (Cb) */
) ;


/******************************************************************************
Summary:
	Clear pixel data override

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	
Returns:
	BERR_SUCCESS - clear of override was successful
	
*******************************************************************************/
BERR_Code BHDM_ClearPixelDataOverride(
	BHDM_Handle hHDMI		   /* [in] HDMI handle */
) ;


/******************************************************************************
Summary:
	Waits for stable video [to arrive from the VEC]

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.
	
Returns:
	BERR_SUCCESS - 
	
*******************************************************************************/
BERR_Code BHDM_WaitForStableVideo(
	BHDM_Handle hHDMI,		/* [in] HDMI handle */
	uint32_t stablePeriod,		 /* [in] Period of time video should be stable */
	uint32_t maxWait			 /* [in] Max amount of time to wait */
) ;
 
  
#ifdef __cplusplus
  }
#endif
  
#endif /* BHDM_1_3_FEATURES_H__ */
  
  
