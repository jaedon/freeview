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
 * $brcm_Workfile: drm_sfu.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/21/12 3:42p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 *****************************************************************************/
#ifndef _DRM_SFU_H_
#define _DRM_SFU_H_

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"

#include "drm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Sfu Parameter Settings structure */
typedef struct DrmSfuParamSettings_t
{
	DrmCommonInit_t drmCommonInit;
}DrmSfuParamSettings_t;

/******************************************************************************
** FUNCTION:
**  DRM_Sfu_GetDefaultParamSettings
**
** DESCRIPTION:
**   Retrieve the default settings
**
** PARAMETERS:
** pSfuParamSettings - pointer to settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Sfu_GetDefaultParamSettings(
		DrmSfuParamSettings_t *pSfuParamSettings);


/******************************************************************************
** FUNCTION:
**  DRM_Sfu_SetParamSettings
**
** DESCRIPTION:
**   Set param settings
**
** PARAMETERS:
** sfuParamSettings - settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Sfu_SetParamSettings(
		DrmSfuParamSettings_t sfuParamSettings);


/******************************************************************************
** FUNCTION:
**  DRM_Sfu_Initialize
**
** DESCRIPTION:
**   Reads the bin file specified and pre-loads the confidential info
******************************************************************************/
DrmRC DRM_Sfu_Initialize(char *key_file);


/******************************************************************************
** FUNCTION:
**  DRM_Sfu_Finalize
**
** DESCRIPTION:
**   Cleans up the SFU DRM module
******************************************************************************/
DrmRC DRM_Sfu_Finalize(void);



DrmRC DRM_Sfu_GetKpublicKey(uint8_t *pKpublicKey);


/******************************************************************************
** FUNCTION
**   DRM_Sfu_DecryptAndValidate
**
** DESCRIPTION:
**   Decrypts and verifies the hash of the data pointer passed in.
**
** PARAMETERS:
**   pData - Pointer to an encrypted alloc-aligned buffer.
**	 size  - Size of the data to decrypt
**   packet_type - Indicates the START, MID or END packet
**   pHash - applicable only to a START pack.  Can be NULL for MID or END
**
** RETURNS:
**   Drm_Success or an error.
**
******************************************************************************/
DrmRC DRM_Sfu_DecryptAndValidate(
		uint8_t *pData,
		uint32_t size,
		DrmM2mPacketType packet_type,
		uint8_t *pHash);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _DRM_SFU_H_ */
