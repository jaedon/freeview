/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 * 
 * $brcm_Workfile: nexus_picture_decoder_init.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 1/24/12 2:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/picture_decoder/3548/include/nexus_picture_decoder_init.h $
 * 
 * 3   1/24/12 2:44p vsilyaev
 * SW7425-2258: Added Nexus video decoder extension to decode MJPEG
 * 
 * 2   7/14/10 3:20p erickson
 * SW7405-4621: change to nexus_types.h
 * 
 * 1   4/11/08 5:51p vsilyaev
 * PR 40870: Hardware JPEG decoder
 * 
 **************************************************************************/
#ifndef NEXUS_PICTURE_DECODER_INIT_H__
#define NEXUS_PICTURE_DECODER_INIT_H__

#include "nexus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Settings used to configure the PictureDecoder module.

Description:

See Also:
NEXUS_PictureDecoderModule_GetDefaultSettings
NEXUS_PictureDecoderModule_Init
**/
typedef struct NEXUS_PictureDecoderModuleSettings
{
    NEXUS_ModuleHandle surface; /* Handle to Surface module. See NEXUS_SurfaceModule_Init. */
    NEXUS_ModuleHandle transport; /* Handle to Transport module. See NEXUS_TransportModule_Init. */
} NEXUS_PictureDecoderModuleSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_PictureDecoderModule_Init
**/
void NEXUS_PictureDecoderModule_GetDefaultSettings(
    NEXUS_PictureDecoderModuleSettings *pSettings /* [out] */
    );

/**
Summary:
Initialize the PictureDecoder module.

Description:
This is called by the NEXUS Platform when the system is initialized.

See Also:
NEXUS_PictureDecoderModule_Uninit
NEXUS_PictureDecoder_Open - open Interface for PictureDecoder
**/    
NEXUS_ModuleHandle NEXUS_PictureDecoderModule_Init(
    const NEXUS_PictureDecoderModuleSettings *pSettings
    );
   
/**
Summary:
Uninitialize the PictureDecoder module.

Description:
This is called by the NEXUS Platform when the system is uninitialized.

See Also:
NEXUS_PictureDecoderModule_Init
**/     
void NEXUS_PictureDecoderModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
