/******************************************************************************
 *    (c)2010-2012 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: pmc_generic.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/21/12 3:42p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/lib/security/common_drm/include/pmc_generic.h $
 * 
 * 2   11/21/12 3:42p cdisc
 * SWSECURITY-200: memory heap management issue
 * 
 *****************************************************************************/
#ifndef PMC_GENERIC_H_
#define PMC_GENERIC_H_

#include "drm_metadata.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Generic Parameter Settings structure */
typedef struct PmcGenericParamSettings_t
{
	DrmCommonInit_t drmCommonInit;
}PmcGenericParamSettings_t;

/******************************************************************************
** FUNCTION:
**  DRM_Pmc_GenericGetDefaultParamSettings
**
** DESCRIPTION:
**   Retrieve the default settings
**
** PARAMETERS:
** pPmcGenericParamSettings - pointer to settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Pmc_GenericGetDefaultParamSettings(
		PmcGenericParamSettings_t *pPmcGenericParamSettings);


/******************************************************************************
** FUNCTION:
**  DRM_Pmc_GenericSetParamSettings
**
** DESCRIPTION:
**   Set param settings
**
** PARAMETERS:
** pmcGenericParamSettings - Settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Pmc_GenericSetParamSettings(
		PmcGenericParamSettings_t pmcGenericParamSettings);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_GenericInit
**
** DESCRIPTION:
**   Initializes the Generic PMC module
**
** PARAMETERS:
**   pmc_bin_filepath - Pointer to the pmc.bin file path in the root filesystem
**   drm_bin_filepath - Pointer to the DRM bin file path in the root filesystem
**
** RETURNS:
**   Drm_Success when the operation is successful or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_GenericInit(char *pmc_bin_filepath, char *drm_bin_filepath);


void DRM_Pmc_GenericUnInit(void);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_GenericGetCategory
**
** DESCRIPTION:
**   Return the player Category to the caller
**
** PARAMETERS:
**   pBuf - Pointer to the buffer where the player category will be copied into.
**          BUFFER PASSED TO FUNCTION SHOULD BE MAX 64-bytes
**
** RETURNS:
**   Drm_Success when the operation is successful or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_GenericGetCategory(uint8_t *pCategory);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_GenericGetManufacturerId
**
** DESCRIPTION:
**   Return the manufacturer ID to the caller
**
** PARAMETERS:
**   pBuf - Pointer to the buffer where the manufacturer ID will be copied into.
**          BUFFER PASSED TO FUNCTION SHOULD BE MAX 64-bytes
**
** RETURNS:
**   Drm_Success when the operation is successful or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_GenericGetManufacturerId(uint8_t *pManuID);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_GenericGetModelId
**
** DESCRIPTION:
**   Return the model ID to the caller
**
** PARAMETERS:
**   pBuf - Pointer to the buffer where model ID will be copied into.
**          BUFFER PASSED TO FUNCTION SHOULD BE MAX 64-bytes
**
** RETURNS:
**   Drm_Success when the operation is successful or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_GenericGetModelId(uint8_t *pModelID);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_GenericGetModelGroup
**
** DESCRIPTION:
**   Return the model group to the caller
**
** PARAMETERS:
**   pBuf - Pointer to the buffer where model group will be copied into.
**          BUFFER PASSED TO FUNCTION SHOULD BE MAX 64-bytes
**
** RETURNS:
**   Drm_Success when the operation is successful or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_GenericGetModelGroup(uint8_t *pModelGroup);

 
#ifdef __cplusplus
}
#endif
  


#endif /*PMC_GENERIC_H_*/
