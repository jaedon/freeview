/*********************************************************************************
*     Copyright (c) 2010-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*   This program is the proprietary software of Broadcom Corporation and/or its licensors, 
*   and may only be used, duplicated, modified or distributed pursuant to the terms and 
*   conditions of a separate, written license agreement executed between you and Broadcom 
*   (an "Authorized License").  Except as set forth in an Authorized License, 
*   Broadcom grants no license (express or implied), right to use, or waiver of any kind 
*   with respect to the Software, and Broadcom expressly reserves all rights in and to the 
*   Software and all intellectual property rights therein.  
* 
*   IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, 
*   AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
* 
*   Except as expressly set forth in the Authorized License,
*   1.     This program, including its structure, sequence and organization, constitutes the 
*       valuable trade secrets of Broadcom, and you shall use all reasonable efforts to protect 
*       the confidentiality thereof, and to use this information only in connection with your use 
*       of Broadcom integrated circuit products.
*   2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH ALL 
*       FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, 
*       STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND 
*       ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*       LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO 
*       DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
*   3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS BE LIABLE 
*       FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT 
*       OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN 
*       ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID 
*       FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING 
*       ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
*
* $brcm_Workfile: drm_wmdrm.h $
* $brcm_Revision: 2 $
* $brcm_Date: 11/21/12 3:42p $
*
* Module Description: Customer specific module used to fetch WMDRM private keys
*
* Revision History:
*
* Created: 07/30/10 by Claudio Discepola
*
* $brcm_Log: /BSEAV/lib/security/common_drm/include/drm_wmdrm.h $
* 
* 2   11/21/12 3:42p cdisc
* SWSECURITY-200: memory heap management issue
* 
*
*********************************************************************************/
#ifndef DRM_WMDRM_H_
#define DRM_WMDRM_H_

#include "drm_metadata.h"
#include "drm_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Wmdrm Parameter Settings structure */
typedef struct DrmWmdrmParamSettings_t
{
	DrmCommonInit_t drmCommonInit;
}DrmWmdrmParamSettings_t;

/******************************************************************************
** FUNCTION:
**  DRM_Wmdrm_GetDefaultParamSettings
**
** DESCRIPTION:
**   Retrieve the default settings
**
** PARAMETERS:
** pWmdrmParamSettings - pointer to settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Wmdrm_GetDefaultParamSettings(
		DrmWmdrmParamSettings_t *pWmdrmParamSettings);

/******************************************************************************
** FUNCTION:
**  DRM_Wmdrm_SetParamSettings
**
** DESCRIPTION:
**   Set param settings
**
** PARAMETERS:
** wmdrmParamSettings - settings structure
**
** RETURNS:
**   void.
**
******************************************************************************/
void DRM_Wmdrm_SetParamSettings(
		DrmWmdrmParamSettings_t wmdrmParamSettings);

/*********************************************************************************************
// FUNCTION:
//  DRM_Wmdrm_Finalize
//
// DESCRIPTION:
//   This function read encrypted widevine key image from file and
//   initialize the key slots. MUST BE CALLED FIRST
//   key_file is the name of the DRM Utility generated key image file
*********************************************************************************************/
void DRM_Wmdrm_Finalize(void);

/**********************************************************************************************
DRM_Wmdrm_GetPrivateKey

Retrieve the WMDRM Private key from a user specified file.

Input Parameters:
    wmdrm_file_path
        Type: const char *
        Purpose: a NULL terminated string representing the file where the WMDRM key will be retrieved
        
Output parameter:
    pBuf
        Type: uint8_t*
        Purpose: pointer to a preallocated memory buffer that will store the retrieved key
        
Returns:
    SUCCESS: Drm_Success
    FAILURE: other
**********************************************************************************************/
DrmRC DRM_Wmdrm_GetPrivateKey(
    const char* wmdrm_file_path,
    uint8_t* pBuf
    );


/**********************************************************************************************
DRM_Wmdrm_GetFallbackKey

Retrieve the WMDRM Private Fallback key.

Input Parameters:
	N/A

Output parameter:
    pBuf
        Type: uint8_t*
        Purpose: pointer to a preallocated memory buffer that will store the retrieved key

Returns:
    SUCCESS: Drm_Success
    FAILURE: other
**********************************************************************************************/
DrmRC DRM_Wmdrm_GetFallbackKey(
    uint8_t* pBuf);


/**********************************************************************************************
DRM_Wmdrm_SetKeyFilePath

Set the path of the WMDRM key file from a user specified file.

Input Parameters:
    wmdrm_file_path
        Type: const char *
        Purpose: a NULL terminated string representing the file where the WMDRM key will be retrieved

Output parameter:
        N/A

Returns:
    SUCCESS: Drm_Success
    FAILURE: other
**********************************************************************************************/
DrmRC DRM_Wmdrm_SetKeyFilePath(
    const char* wmdrm_file_path
    );

/*********************************************************************************************
// FUNCTION:
//  DRM_Wmdrm_Initialize
//
// DESCRIPTION:
//   This function read encrypted widevine key image from file and
//   initialize the key slots. MUST BE CALLED FIRST
//   key_file is the name of the DRM Utility generated key image file
*********************************************************************************************/
DrmRC DRM_Wmdrm_Initialize(char *key_file);


/**********************************************************************************************
DRM_Wmdrm_GetUtvKeyPath

Retrieve the path of the WMDRM key file (UTV)

Input Parameters:
    pBuf
        Type: char *
        Purpose: buffer that will contain the wmdrmpd_keys.dat file path

Output parameter:
        N/A

Returns:
		N/A
**********************************************************************************************/
void DRM_Wmdrm_GetUtvKeyPath(char *pBuf);

#ifdef __cplusplus
}
#endif

#endif /*DRM_WMDRM_H_*/
