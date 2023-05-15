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
 * $brcm_Workfile: pmc_pandora.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/21/12 3:42p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/lib/security/common_drm/include/pmc_pandora.h $
 * 
 * 2   11/21/12 3:42p cdisc
 * SWSECURITY-200: memory heap management issue
 * 
 *****************************************************************************/
#ifndef PMC_PANDORA_H_
#define PMC_PANDORA_H_

#include "drm_metadata.h"

#ifdef __cplusplus
	extern "C" {
#endif

/* Pandora Parameter Settings structure */
typedef struct PmcPandoraParamSettings_t
{
	DrmCommonInit_t drmCommonInit;
}PmcPandoraParamSettings_t;

/******************************************************************************
** FUNCTION:
**  DRM_Pmc_PandoraGetDefaultParamSettings
**
** DESCRIPTION:
**   Retrieve the default settings
**
** PARAMETERS:
** pPmcPandoraParamSettings - pointer to settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Pmc_PandoraGetDefaultParamSettings(
		PmcPandoraParamSettings_t *pPmcPandoraParamSettings);


/******************************************************************************
** FUNCTION:
**  DRM_Pmc_PandoraSetParamSettings
**
** DESCRIPTION:
**   Retrieve the default settings
**
** PARAMETERS:
** pPmcPandoraParamSettings - pointer to settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Pmc_PandoraSetParamSettings(
		PmcPandoraParamSettings_t pmcPandoraParamSettings);

/******************************************************************************
** FUNCTION
**   DRM_Pmc_PandoraInit
**
** DESCRIPTION:
**   Initializes the PMC Pandora module
**
** PARAMETERS:
**   pmc_bin_filepath[in] - Pointer to the pmc.bin file path in the root filesystem
**   drm_bin_filepath[in] - Pointer to the DRM bin file path in the root filesystem
**
** RETURNS:
**   Drm_Success when the operation is successful or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_PandoraInit(char *pmc_bin_filepath, char *drm_bin_filepath);


void DRM_Pmc_PandoraUnInit(void);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_PandoraGetUsername
**
** DESCRIPTION:
**   This function returns Username to the caller.
**
** PARAMETERS:
**   pBuf - Pointer to the buffer where Username will be copied into.
**          BUFFER PASSED TO FUNCTION SHOULD BE 128-bytes 
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_PandoraGetUsername(uint8_t *pUsername);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_PandoraGetPassword
**
** DESCRIPTION:
**   This function returns Password to the caller.
**
** PARAMETERS:
**   pBuf - Pointer to the buffer where Password will be copied into.
**          BUFFER PASSED TO FUNCTION SHOULD BE 128-bytes
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_PandoraGetPassword(uint8_t *pPassword);



/******************************************************************************
** FUNCTION
**   DRM_Pmc_PandoraGetDevModel
**
** DESCRIPTION:
**   This function returns 'Device Model' to the called.
**
** PARAMETERS:
**   pBuf - Pointer to the buffer where esn will be copied into.
**          BUFFER PASSED TO FUNCTION SHOULD BE 64-bytes
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_PandoraGetDevModel(uint8_t *pDevModel);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_PandoraGetSyncKey
**
** DESCRIPTION:
**   This function returns Sync key to the caller.
**
** PARAMETERS:
**   pSyncKey - Pointer to the buffer where Sync Key will be copied into. 
**              BUFFER PASSED TO FUNCTION SHOULD BE 64-bytes
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_PandoraGetSyncKey(uint8_t *pSyncKey);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_PandoraGetReqKey
**
** DESCRIPTION:
**   This function returns Req key to the caller.
**
** PARAMETERS:
**   pReqKey - Pointer to the buffer where Req Key will be copied into. 
**              BUFFER PASSED TO FUNCTION SHOULD BE 64-bytes
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_PandoraGetReqKey(uint8_t *pReqKey);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_PandoraGetHttpUrl
**
** DESCRIPTION:
**   This function returns HTTP url to the caller
**
** PARAMETERS:
**   pReqKey - Pointer to the buffer where HTTP string will be copied into. 
**              BUFFER PASSED TO FUNCTION SHOULD BE 192-bytes
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_PandoraGetHttpUrl(uint8_t *pHttp_url);


/******************************************************************************
** FUNCTION
**   DRM_Pmc_PandoraGetHttpsUrl
**
** DESCRIPTION:
**   This function returns HTTPS url to the caller
**
** PARAMETERS:
**   pReqKey - Pointer to the buffer where HTTPS string will be copied into. 
**              BUFFER PASSED TO FUNCTION SHOULD BE 192-bytes
**
** RETURNS:
**   Drm_Success when the operation is successfull or an error.
**
******************************************************************************/
DrmRC DRM_Pmc_PandoraGetHttpsUrl(uint8_t *pHttpS_url);

#ifdef __cplusplus
	}
#endif

#endif /*PMC_PANDORA_H_*/
