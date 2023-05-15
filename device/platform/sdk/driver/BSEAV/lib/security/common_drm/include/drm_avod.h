/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: drm_avod.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/21/12 3:41p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 *****************************************************************************/
#ifndef PMC_AVOD_H_
#define PMC_AVOD_H_

#include "drm_metadata.h"
#include "drm_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* AVOD Parameter Settings structure */
typedef struct DrmAvodParamSettings_t
{
	DrmCommonInit_t drmCommonInit;
}DrmAvodParamSettings_t;

/******************************************************************************
** FUNCTION
**   DRM_Avod_GetDefaultParamSettings
**
** DESCRIPTION:
**   Retrieve the default settings
**
** PARAMETERS:
** pAvodParamSettings - pointer to settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Avod_GetDefaultParamSettings(
		DrmAvodParamSettings_t *pAvodParamSettings);


/******************************************************************************
** FUNCTION
**   DRM_Avod_SetParamSettings
**
** DESCRIPTION:
**   Set custom settings
**
** PARAMETERS:
** avodParamSettings - structure settings
**
** RETURNS:
**   void
**
******************************************************************************/
void DRM_Avod_SetParamSettings(
		DrmAvodParamSettings_t avodParamSettings);

/******************************************************************************
** FUNCTION
**   DRM_Avod_Init
**
** DESCRIPTION:
**   Initializes the DRM module with the pmc.bin file and if specified the
**   DRM bin file as well.  If the 'drm_bin_filepath' is NULL the key region
**   will not be initialized.  This assumes another module already initialized it.
**
** PARAMETERS:
**   pmc_bin_filepath - Pointer to the pmc.bin file path in the root filesystem
**   						Can be NULL only in UTV provisioning case or CARD
**   drm_bin_filepath - Pointer to the DRM bin file path in the root filesystem
**   						Can be NULL only in UTV provisioning case or CARD
**
** RETURNS:
**   Drm_Success when the operation is successful or an error.
**
******************************************************************************/
DrmRC DRM_Avod_Init(char *drm_bin_filepath);


/******************************************************************************
** FUNCTION
**   DRM_Avod_UnInit
**
** DESCRIPTION:
**   Cleans up the AVOD DRM module.  Must be called to avoid a memory leak
**
** PARAMETERS:
**   N/A
**
** RETURNS:
**   N/A
**
******************************************************************************/
void DRM_Avod_UnInit(void);


/******************************************************************************
** FUNCTION
**   DRM_Avod_GetCertificate
**
** DESCRIPTION:
**   Return the AVOD certificate to the caller.
**   This function must be called twice.
**   The first time the caller should pass NULL for pCertificate while still
**   passing the address of a uint32_t for the 2nd argument.
**   This will return the size in bytes that the caller should allocate to
**   the buffer that will contain the certificate.
**   The second call should pass a non-NULL pointer that was allocated with the
**   proper size.
**
**   Example:
**
**   DRM_Avod_GetCertificate(NULL, &size);
**   certificate = (uint8_t*)malloc(size);
**   DRM_Avod_GetCertificate(certificate, size);
**
** PARAMETERS:
**   pCertificate - Pointer to where the certificate will be copied into
**   pSize        - Pointer to variable to contain the size
**
** RETURNS:
**   Drm_Success when the operation is successful or an error.
**
******************************************************************************/
DrmRC DRM_Avod_GetCertificate(uint8_t *pCertificate, uint32_t *pCertificateSize);


/******************************************************************************
** FUNCTION
**   DRM_Avod_GetKey
**
** DESCRIPTION:
**   Return the AVOD key to the caller.
**   This function must be called twice.
**   The first time the caller should pass NULL for pKey while still
**   passing the address of a uint32_t for the 2nd argument.
**   This will return the size in bytes that the caller should allocate to
**   the buffer that will contain the certificate.
**   The second call should pass a non-NULL pointer that was allocated with the
**   proper size.
**
**   Example:
**
**   DRM_Avod_GetKey(NULL, &size);
**   certificate = (uint8_t*)malloc(size);
**   DRM_Avod_GetKey(certificate, size);
**
** PARAMETERS:
**   pKey - Pointer to where the key will be copied into
**   pKeySize     - Pointer to variable to contain the size
**
** RETURNS:
**   Drm_Success when the operation is successful or an error.
**
******************************************************************************/
DrmRC DRM_Avod_GetKey(uint8_t *pKey, uint32_t *pKeySize);

#ifdef __cplusplus
}
#endif



#endif /*PMC_AVOD_H_*/
