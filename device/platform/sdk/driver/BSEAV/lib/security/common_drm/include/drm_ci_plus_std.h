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
 * $brcm_Workfile: drm_ci_plus_std.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/21/12 3:41p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/lib/security/common_drm/include/drm_ci_plus_std.h $
 * 
 * 2   11/21/12 3:41p cdisc
 * SWSECURITY-200: memory heap management issue
 * 
 *****************************************************************************/
#ifndef DRM_CI_PLUS_STD_H_
#define DRM_CI_PLUS_STD_H_

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "drm_types.h"
#include "drm_metadata.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CiPlusStd Parameter Settings structure */
typedef struct DrmCiPlusStdParamSettings_t
{
	DrmCommonInit_t drmCommonInit;
}DrmCiPlusStdParamSettings_t;


/******************************************************************************
** FUNCTION
**   DRM_CiPlusStd_GetDefaultParamSettings
**
** DESCRIPTION:
**   Retrieve the default settings
**
** PARAMETERS:
** pCiPlusStdParamSettings - pointer to settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_CiPlusStd_GetDefaultParamSettings(
		DrmCiPlusStdParamSettings_t *pCiPlusStdParamSettings);

/******************************************************************************
** FUNCTION
**   DRM_CiPlusStd_SetParamSettings
**
** DESCRIPTION:
**   Set the default settings
**
** PARAMETERS:
** pCiPlusStdParamSettings - settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_CiPlusStd_SetParamSettings(
		DrmCiPlusStdParamSettings_t ciPlusStdParamSettings);

/******************************************************************************
** FUNCTION
**   DRM_CiPlusStd_Initialize
**
** DESCRIPTION:
**   Initialize the standalone CI+ module
**
** PARAMETERS:
**   cip_bin_file_path[in] - pointer to the DRM bin file containing the
**   					standalone CI+ key
**
** RETURNS:
**   Drm_Success or error
**
******************************************************************************/
DrmRC DRM_CiPlusStd_Initialize(char *cip_bin_file_path);

DrmRC DRM_CiPlusStd_Finalize(void);


/******************************************************************************
** FUNCTION
**   DRM_CiPlusStd_GetCiPlusStdKey
**
** DESCRIPTION:
**   Retrieve the standalone CI+ key
**
** PARAMETERS:
**   ci_plus_std_key[out] - buffer to store the standalone CI+ key
**
** RETURNS:
**   Drm_Success or error
**
******************************************************************************/
DrmRC DRM_CiPlusStd_GetCiPlusStdKey(uint8_t *ci_plus_std_key);

#ifdef __cplusplus
}
#endif

#endif /*DRM_CI_PLUS_STD_H_*/
