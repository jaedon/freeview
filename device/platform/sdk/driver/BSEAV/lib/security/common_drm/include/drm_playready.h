/*********************************************************************************
*     Copyright (c) 2011, Broadcom Corporation
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
* $brcm_Workfile: drm_playready.h $
* $brcm_Revision: 1 $
* $brcm_Date: 8/22/11 9:35a $
*
* Module Description: Customer specific module used to fetch Playready information stored in the playready bin file.
*
* Revision History:
*
*********************************************************************************/
#ifndef DRM_PLAYREADY_H_
#define DRM_PLAYREADY_H_

#include "drm_metadata.h"
#include "drm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This enums list all the kind of data which can be read from the playready drm bin file */
typedef enum DRM_Msdrm_Data{
    WMDRM_GROUP_PRIV_KEY,        /* WMDRM Group Private Key */
    WMDRM_FALLBACK_PRIV_KEY,     /* WMDRM Fallback Private Key */
    PRDY_MODEL_CERT_PRIV_KEY,    /* Playready Model Certificate Private Key */
    WMDRM_DEV_TEMPLATE,          /* WMDRM device template */
    PRDY_BGROUP_CERT             /* Playready BGroup certificate */
} DRM_Msdrm_Data_e;


/*********************************************************************************************
* DRM_Prdy_Initialize
*
* This function reads the Playready drm bin file. It must be called before any other functions
* of this module.
* 
* Input Parameters: 
* key_file : Specifices the location of the playready drm bin file to read.
*
*
* Returns: 
*   On Success:   Drm_Success
*   On Failure:   any other error code. 
*********************************************************************************************/
DrmRC DRM_Prdy_Initialize(char *key_file);

/*********************************************************************************************
* DRM_Prdy_Uninitialize
*
* This function free up the ressources allocated by this module.
* 
* Returns: N/A
*********************************************************************************************/
void DRM_Prdy_Uninitialize(void);


/*********************************************************************************************
* DRM_Prdy_GetData
*
* This function retrieves data from thePlayready drm bin file loaded in memory and copy the it into
* the buffer pointed to by pBuf. The number of bytes copieds is returned into bufSize. An application can
* call this function and set pBuf to NULL to figure out how much memory it needs to allocate.
* 
* Input Parameters: 
* type : Id specifying the data to retrieve.
*
* Input/Output Parameter:
* pBuf 
*     Type:  uint8_t*
*     Purpose:  Buffer Where the data will be copied into. When the parameter is NULL,
*     the function only returns the size of the data saved in the file.
* 
* bufSize 
*     Type: uint32_t
*     Purpose: Specifies the length of the buffer in bytes.
*
* Returns: 
*   On Success:   Drm_Success
*   On Failure:   any other error code. 
*********************************************************************************************/
DrmRC DRM_Prdy_GetData(
    DRM_Msdrm_Data_e  type,
    uint8_t          *pBuf,
    uint32_t         *pSize);

#ifdef __cplusplus
}
#endif

#endif /*DRM_PLAYREADY_H_*/
