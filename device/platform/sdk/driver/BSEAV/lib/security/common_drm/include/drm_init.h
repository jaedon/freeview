/******************************************************************************
 *    (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: drm_init.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/22/11 9:35a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 *****************************************************************************/
#ifndef DRM_INIT_H_
#define DRM_INIT_H_

#include "drm_types.h"
#include "nexus_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************
DRM_Init_Module

Initialize the DRM module

Input Parameters:
	N/A

Returns:
    SUCCESS: Drm_Success
    FAILURE: other
**********************************************************************************************/
DrmRC DRM_Init_Module(void);


/**********************************************************************************************
DRM_UnInit_Module

UnInitialize the DRM module

Input Parameters:
	N/A

Returns:
    SUCCESS: Drm_Success
    FAILURE: other
**********************************************************************************************/
DrmRC DRM_UnInit_Module(void);

/**********************************************************************************************
DRM_Init_SetDmaHandle

Set an external DMA handle if one has already been opened by the upper application.
A call to this API should only be done once for the lifecycle of the DMA handle.

Input Parameters:
    dma
        Type: NEXUS_DmaHandle
        Purpose: a valid DMA handle that has already been opened by the upper layers

Returns:
    SUCCESS: Drm_Success
    FAILURE: other
**********************************************************************************************/
DrmRC DRM_Init_SetDmaHandle(NEXUS_DmaHandle dma);


/**********************************************************************************************
DRM_Init_SetKeyProvisionType

Wrapper function for KeyRegion funciton.
Sets the provisioning type for way in which the keys will be fetched.

Input Parameters:
    provisioningType
        Type: DrmKeyProvisioningType
        Purpose: Set to either (DrmKeyProvisioningType_eUtv or DrmKeyProvisioningType_eBrcm)

Returns:
    SUCCESS: Drm_Success
    FAILURE: other
**********************************************************************************************/
DrmRC DRM_Init_SetKeyProvisionType(DrmKeyProvisioningType provisioningType);

#ifdef __cplusplus
}
#endif

#endif  /*DRM_INIT_H_*/
