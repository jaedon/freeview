/***************************************************************************
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
 * $brcm_Workfile: drm_ci_plus.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/21/12 3:41p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /BSEAV/lib/security/common_drm/include/drm_ci_plus.h $
 * 
 * 2   11/21/12 3:41p cdisc
 * SWSECURITY-200: memory heap management issue
 * 
 *
 ***************************************************************************/

#ifndef DRM_CI_PLUS_H_
#define DRM_CI_PLUS_H_

#include "bstd.h"
#include "drm_types.h"
#include "drm_metadata.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum drm_ci_plus_data_e
{
    DRM_CI_PLUS_DATA_CA = 0,
    DRM_CI_PLUS_DATA_ROOT,
    DRM_CI_PLUS_DATA_CERT,
    DRM_CI_PLUS_DATA_KEYS,
    DRM_CI_PLUS_DATA_FILENAME_KEY,
    DRM_CI_PLUS_DATA_FILENAME_CERT,
    DRM_CI_PLUS_DATA_NONE
}drm_ci_plus_data_e;

/* CiPlusStd Parameter Settings structure */
typedef struct DrmCiPlusParamSettings_t
{
	DrmCommonInit_t drmCommonInit;
}DrmCiPlusParamSettings_t;


/******************************************************************************
** FUNCTION
**   DRM_CiPlus_GetDefaultParamSettings
**
** DESCRIPTION:
**   Retrieve the default settings
**
** PARAMETERS:
** pCiPlusParamSettings - pointer to settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_CiPlus_GetDefaultParamSettings(
		DrmCiPlusParamSettings_t *pCiPlusParamSettings);

/******************************************************************************
** FUNCTION
**   DRM_CiPlus_SetParamSettings
**
** DESCRIPTION:
**   Set the default settings
**
** PARAMETERS:
** ciPlusParamSettings - settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_CiPlus_SetParamSettings(
		DrmCiPlusParamSettings_t ciPlusParamSettings);

/******************************************************************************
** FUNCTION
**   DRM_CiPlus_Initialize
**
** DESCRIPTION:
**   Initialize the standalone CI+ module
**
** PARAMETERS:
**   custom_bin_file_path[in] - pointer to the DRM bin file containing the
**   					standalone CI+ key 
**
** RETURNS:
**   Drm_Success or error
**
******************************************************************************/
DrmRC DRM_CiPlus_Initialize(char *custom_bin_file_path);
DrmRC DRM_CiPlus_Finalize(void);


/******************************************************************************
** FUNCTION
**   DRM_CiPlus_GetDataBlock
**
** DESCRIPTION:
**   Retrieve one of six possible data blocks:
**           CA, ROOT, CERT, KEYS, Filename of key or cert DER
**
**   The size of the 'data_block' buffer passed to this function should
**   correspond to the data being retrieved. Therefore, 2Kbytes for all data types
**   except for the filename parameters, that is:
**
**  DRM_CI_PLUS_DATA_CA   :  CI_PLUS_PADDED_ENC_CA_SIZE      (2K)
**  DRM_CI_PLUS_DATA_ROOT :  CI_PLUS_PADDED_ENC_ROOT_SIZE    (2K)
**  DRM_CI_PLUS_DATA_CERT :  CI_PLUS_PADDED_ENC_CERT_SIZE    (2K)
**  DRM_CI_PLUS_DATA_KEYS :  CI_PLUS_PADDED_ENC_KEYS_SIZE    (2K)
**  DRM_CI_PLUS_DATA_FILENAME_KEY  : 16
**  DRM_CI_PLUS_DATA_FILENAME_CERT : 16
**
**
**
** PARAMETERS:
**   data_block - buffer to store returned data
**   data_type - DRM_CI_PLUS_DATA_CA, DRM_CI_PLUS_DATA_ROOT,
**               DRM_CI_PLUS_DATA_CERT or DRM_CI_PLUS_DATA_KEYS,
**               DRM_CI_PLUS_DATA_FILENAME_KEY, DRM_CI_PLUS_DATA_FILENAME_CERT
**   *return_size - the actual size of the data block copied and returned
**                  to the caller.  Decrypted data will always be 16-byte aligned
**                  and be terminated with padded with zero (if necessary)
**
**  NOTE: The DRM_CI_PLUS_DATA_FILENAME_KEY, DRM_CI_PLUS_DATA_FILENAME_CERT
**          can be represented as strings although they will not be NULL
**          terminated
**
** RETURNS:
**   DRM_SUCCESS or error
**
******************************************************************************/
DrmRC DRM_CiPlus_GetDataBlock(uint8_t *data_block,
								drm_ci_plus_data_e data_type,
								uint32_t *return_size);

#ifdef __cplusplus
}
#endif

#endif /*DRM_CI_PLUS_H_*/
