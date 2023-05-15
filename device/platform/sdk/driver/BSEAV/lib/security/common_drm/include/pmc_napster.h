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
 * $brcm_Workfile: pmc_napster.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/21/12 3:42p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/lib/security/common_drm/include/pmc_napster.h $
 * 
 * 2   11/21/12 3:42p cdisc
 * SWSECURITY-200: memory heap management issue
 * 
 *****************************************************************************/
#ifndef PMC_NAPSTER_H_
#define PMC_NAPSTER_H_

#include "drm_metadata.h"

#ifdef __cplusplus
	extern "C" {
#endif


/* Napster Parameter Settings structure */
typedef struct PmcNapsterParamSettings_t
{
	DrmCommonInit_t drmCommonInit;
}PmcNapsterParamSettings_t;

/******************************************************************************
** FUNCTION:
**  DRM_Pmc_NapsterGetDefaultParamSettings
**
** DESCRIPTION:
**   Retrieve the default settings
**
** PARAMETERS:
** pPmcNapsterParamSettings - pointer to settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Pmc_NapsterGetDefaultParamSettings(
		PmcNapsterParamSettings_t *pPmcNapsterParamSettings);

/******************************************************************************
** FUNCTION:
**  DRM_Pmc_NapsterSetParamSettings
**
** DESCRIPTION:
**   Set param settings
**
** PARAMETERS:
** pmcNapsterParamSettings - settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Pmc_NapsterSetParamSettings(
		PmcNapsterParamSettings_t pmcNapsterParamSettings);

/******************************************************************************
** FUNCTION
**   PMC_Napster_Init
**
** DESCRIPTION:
**   Initializes the PMC napster module
**
** PARAMETERS:
**   pmc_bin_filepath - Pointer to the pmc.bin file path in the root filesystem
**   drm_bin_filepath - Pointer to the DRM bin file path in the root filesystem
**
** RETURNS:
**   Drm_Success when the operation is successful or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_NapsterInit(char *pmc_bin_filepath, char *drm_bin_filepath);

void DRM_Pmc_NapsterUnInit(void);



/******************************************************************************
** FUNCTION
**   PMC_Napster_GetApiKey
**
** DESCRIPTION:
**   Return the API key string to the caller
**
** PARAMETERS:
**   pBuf - Pointer to the buffer where esn will be copied into.
**          BUFFER PASSED TO FUNCTION SHOULD BE 128-bytes even though 56 are used
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_NapsterGetApiKey(uint8_t *pApiKey);


/******************************************************************************
** FUNCTION
**   PMC_Napster_GetStreamFormat
**
** DESCRIPTION:
**   Return the Stream Format string to the caller.
**
** PARAMETERS:
**   pStreamFormat - Pointer to the buffer where the stream format string will be copied into.
**              BUFFER PASSED TO FUNCTION SHOULD BE 64-bytes
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_NapsterGetStreamFormat(uint8_t *pStreamFormat);



/******************************************************************************
** FUNCTION
**   PMC_Napster_GetHttp
**
** DESCRIPTION:
**   Return the Username string to the caller.
**
** PARAMETERS:
**   pHttp - Pointer to the buffer where Http URL will be copied into.
**          BUFFER PASSED TO FUNCTION SHOULD BE 64-bytes 
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_NapsterGetHttp(uint8_t *pHttp);


/******************************************************************************
** FUNCTION
**   PMC_Napster_GetHttps
**
** DESCRIPTION:
**   Return the HTTPS URL string to the caller.
**
** PARAMETERS:
**   pBuf - Pointer to the buffer where HTTPS URL will be copied into.
**          BUFFER PASSED TO FUNCTION SHOULD BE 64-bytes 
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_NapsterGetHttps(uint8_t *pHttps);


  
#ifdef __cplusplus
	}
#endif

#endif /*PMC_NAPSTER_H_*/
