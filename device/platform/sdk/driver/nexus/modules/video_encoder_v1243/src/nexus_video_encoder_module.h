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
 * $brcm_Workfile: nexus_video_encoder_module.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 6/19/12 12:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_encoder/7425/src/nexus_video_encoder_module.h $
 * 
 * 7   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 6   9/19/11 5:46p hongtaoz
 * SW7425-1322: added nexus platform support for dual transcode;
 * 
 * 5   9/7/11 4:04p gmohile
 * SW7425-533 : Add VCE standby support
 * 
 * 4   2/11/11 8:39p vsilyaev
 * SW7425-81: Added image interface plumbing to the video encoder module
 * 
 * 3   1/17/11 4:46p vsilyaev
 * SW7425-65: Added StcChannel to the videoEncoder
 * 
 * 2   12/16/10 4:05p vsilyaev
 * SW7425-40: Propagate more configuration to the video encoder
 * 
 * 1   12/9/10 8:11p vsilyaev
 * SW7425-40: Video Encoder module
 * 
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_ENCODER_MODULE_H__
#define NEXUS_VIDEO_ENCODER_MODULE_H__

#include "nexus_base.h"
#include "nexus_video_encoder_thunks.h"
#include "nexus_video_encoder_types.h"
#include "nexus_video_encoder.h"
#include "nexus_video_encoder_output.h"
#include "nexus_video_encoder_init.h"
#include "nexus_platform_features.h"

#include "priv/nexus_core_img.h"
#include "priv/nexus_rave_priv.h"
#include "priv/nexus_video_encoder_priv.h"
#include "priv/nexus_video_encoder_standby_priv.h"

#include "bvce.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NEXUS_VideoEncoder_P_Device;

struct NEXUS_VideoEncoder {
    NEXUS_OBJECT(NEXUS_VideoEncoder);
    BVCE_Channel_Handle enc;
    BVCE_Output_Handle output;
    BVCE_OutputBuffers_Handle buffers;
    NEXUS_VideoEncoderSettings settings;
    NEXUS_VideoEncoderOpenSettings openSettings;
    NEXUS_VideoEncoderStartSettings startSettings;
    struct NEXUS_VideoEncoder_P_Device *device;
};

typedef struct NEXUS_VideoEncoder_P_Device {
    BVCE_Handle vce;
    BMEM_Heap_Handle mem;
    struct NEXUS_VideoEncoder channels[NEXUS_NUM_VCE_CHANNELS];
    /* image interface */
    void * img_context;
    BIMG_Interface img_interface;
} NEXUS_VideoEncoder_P_Device;

typedef struct NEXUS_VideoEncoder_P_State {
    NEXUS_ModuleHandle module;
    NEXUS_VideoEncoder_P_Device vceDevices[NEXUS_NUM_VCE_DEVICES];
    NEXUS_VideoEncoderModuleSettings config;
    NEXUS_TimerHandle logTimer;
} NEXUS_VideoEncoder_P_State;

extern NEXUS_VideoEncoder_P_State g_NEXUS_VideoEncoder_P_State;
#define NEXUS_MODULE_NAME video_encoder
#define NEXUS_MODULE_SELF g_NEXUS_VideoEncoder_P_State.module
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_VideoEncoder);


#ifdef __cplusplus
}
#endif


#endif /* NEXUS_VIDEO_ENCODER_MODULE_H__ */

