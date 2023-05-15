/***************************************************************************
 *     (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_encoder_init.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 10/5/12 11:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_encoder/include/nexus_video_encoder_init.h $
 * 
 * 9   10/5/12 11:50a erickson
 * SW7435-400: clarify how app can set module settings
 * 
 * 8   5/18/12 10:22a erickson
 * SW7435-165: merge
 * 
 * SW7435-165/3   5/17/12 3:25p mward
 * SW7435-165: vceMapping explicitly map VCE device, channel.
 * 
 * 7   3/27/12 1:43p erickson
 * SW7425-2607: use NEXUS_MAX_VCE_DEVICES
 * 
 * 6   2/1/12 12:08p erickson
 * SW7435-7: merge
 * 
 * SW7435-7/3   1/27/12 10:59a mward
 * SW7435-7:  Merge from Main.
 * 
 * SW7435-7/2   11/3/11 5:44p mward
 * SW7435-1:  Allow NEXUS_NUM_VCE_DEVICES to be defined in
 *  nexus_platform_features.h.
 * 
 * SW7435-7/1   11/1/11 11:28a mward
 * SW7435-7:  Allow NEXUS_NUM_VCE_DEVICES to be defined elsewhere (in
 *  nexus_platform_features.h).
 * 
 * 5   1/16/12 3:29p erickson
 * SW7425-1473: merge
 * 
 * 5   1/16/12 3:28p erickson
 * SW7425-1473: merge
 * 
 * SW7425-1473/1   10/14/11 1:40p gmohile
 * SW7425-1473 : Initial implementation of raaga based video encoder
 * 
 * 4   2/16/11 2:34p vsilyaev
 * SW7425-40: Added system heap
 * 
 * 3   1/17/11 4:46p vsilyaev
 * SW7425-65: Added StcChannel to the videoEncoder
 * 
 * 2   12/13/10 7:02p vsilyaev
 * SW7425-40: Added system initialization for the StreamMux and
 *  VideoEncoder modules
 * 
 * 1   12/9/10 8:11p vsilyaev
 * SW7425-40: Video Encoder module
 * 
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_ENCODER_INIT_H__
#define NEXUS_VIDEO_ENCODER_INIT_H__

#include "nexus_video_encoder_types.h"
#include "nexus_display_types.h" /* defines NEXUS_MAX_VIDEO_ENCODERS */

#ifdef __cplusplus
extern "C" {
#endif

#define NEXUS_MAX_VCE_DEVICES   2

/**
Summary:
Settings used to configure the VideoEncoder module.

Description:

See Also:
NEXUS_VideoEncoderModule_GetDefaultSettings
NEXUS_VideoEncoderModule_Init
**/

typedef struct NEXUS_VideoEncoderModuleSettings
{
    NEXUS_VideoEncoder_MemoryConfig heapSize[NEXUS_MAX_VCE_DEVICES];
    struct {
        unsigned general; 
        unsigned firmware[2];
        unsigned secure;
        unsigned picture;
        unsigned system; /* should be accessible by CPU */
    } heapIndex[NEXUS_MAX_VCE_DEVICES];
    NEXUS_ModuleHandle display;
    NEXUS_ModuleHandle transport;
    NEXUS_ModuleHandle audio;
    struct {
        int device; /* VCE core to use. -1 if unused. */
        int channel; /* channel of VCE core to use. -1 if unused.  */
    } vceMapping[NEXUS_MAX_VIDEO_ENCODERS]; /* index of NEXUS_VideoEncoder */
} NEXUS_VideoEncoderModuleSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_VideoEncoderModule_Init
**/
void NEXUS_VideoEncoderModule_GetDefaultSettings(
    NEXUS_VideoEncoderModuleSettings *pSettings /* [out] */
    );

/**
Summary:
Initialize the VideoEncoder module.

Description:
This function is called by NEXUS_Platform_Init, not by applications.
If you want to modify these settings from your application, you can do this 
through NEXUS_PlatformSettings as follows:

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.videoEncoderSettings.xxx = xxx;
    NEXUS_Platform_Init(&platformSettings);

**/
NEXUS_ModuleHandle NEXUS_VideoEncoderModule_Init(
    const NEXUS_VideoEncoderModuleSettings *pSettings
    );

/**
Summary:
Uninitialize the VideoEncoder module.

Description:
Called by NEXUS_Platform_Uninit
**/
void NEXUS_VideoEncoderModule_Uninit(void);


#ifdef __cplusplus
}
#endif


#endif /* NEXUS_VIDEO_ENCODER_INIT_H__ */

