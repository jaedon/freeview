/***************************************************************************
 *     (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_encoder_types.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 2/8/11 5:41p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_encoder/7425/include/nexus_video_encoder_types.h $
 * 
 * 3   2/8/11 5:41p vsilyaev
 * SW7425-40: Updated profile/level configuration
 * 
 * 2   2/7/11 9:41a vsilyaev
 * SW7425-40: Updated to match latest BVCE
 * 
 * 1   12/9/10 8:11p vsilyaev
 * SW7425-40: Video Encoder module
 * 
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_ENCODER_TYPES_H__
#define NEXUS_VIDEO_ENCODER_TYPES_H__

#include "nexus_types.h"
#include "nexus_video_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
**/
typedef struct NEXUS_VideoEncoder *NEXUS_VideoEncoderHandle;


/**
Summary:
**/
typedef enum NEXUS_VideoCodecLevel {
   NEXUS_VideoCodecLevel_eUnknown = 0,
   NEXUS_VideoCodecLevel_e00,
   NEXUS_VideoCodecLevel_e10,
   NEXUS_VideoCodecLevel_e1B,
   NEXUS_VideoCodecLevel_e11,
   NEXUS_VideoCodecLevel_e12,
   NEXUS_VideoCodecLevel_e13,
   NEXUS_VideoCodecLevel_e20,
   NEXUS_VideoCodecLevel_e21,
   NEXUS_VideoCodecLevel_e22,
   NEXUS_VideoCodecLevel_e30,
   NEXUS_VideoCodecLevel_e31,
   NEXUS_VideoCodecLevel_e32,
   NEXUS_VideoCodecLevel_e40,
   NEXUS_VideoCodecLevel_e41,
   NEXUS_VideoCodecLevel_e42,
   NEXUS_VideoCodecLevel_e50,
   NEXUS_VideoCodecLevel_e51,
   NEXUS_VideoCodecLevel_e60,
   NEXUS_VideoCodecLevel_e62,
   NEXUS_VideoCodecLevel_eLow,
   NEXUS_VideoCodecLevel_eMain,
   NEXUS_VideoCodecLevel_eHigh,
   NEXUS_VideoCodecLevel_eHigh1440,
   NEXUS_VideoCodecLevel_eMax
} NEXUS_VideoCodecLevel;

/**
Summary:
**/
typedef enum NEXUS_VideoCodecProfile
{
   NEXUS_VideoCodecProfile_eUnknown = 0,
   NEXUS_VideoCodecProfile_eSimple,
   NEXUS_VideoCodecProfile_eMain,
   NEXUS_VideoCodecProfile_eHigh,
   NEXUS_VideoCodecProfile_eAdvanced,
   NEXUS_VideoCodecProfile_eJizhun,
   NEXUS_VideoCodecProfile_eSnrScalable,
   NEXUS_VideoCodecProfile_eSpatiallyScalable,
   NEXUS_VideoCodecProfile_eAdvancedSimple,
   NEXUS_VideoCodecProfile_eBaseline,

   NEXUS_VideoCodecProfile_eMax
} NEXUS_VideoCodecProfile;


typedef struct NEXUS_VideoEncoder_MemoryConfig 
{
    /* This structure defines heap sizes in bytes per hardware encoder
       These values should be calculated based upon the maximum number of concurrent encodes
       you wish to perform, and must be appropriately sized based upon stream codec and resolution.
       */
       
    unsigned general;   /* General encoder heap.
                           This is used for picture buffer allocation. The memory worksheet reports this as "General Heap" and "Still Heap".
                           If you are using shared analog/digital memory, the only allocation that should be specified here is the still picture decode heap size.
                           Firmware is not allocated from this size. RAVE CDB/ITB buffers are not allocated from this size. */
    unsigned secure;    /* Secure encoder heap. This is only used for SVP systems where the host is prevented from reading clear, compressed data. For
                           non-SVP systems, set to zero.*/
    unsigned picture;   /* Picture buffer heap. This is only used on non-UMA systems. For UMA systems, set to zero. */
} NEXUS_VideoEncoder_MemoryConfig;


#ifdef __cplusplus
}
#endif


#endif /* NEXUS_VIDEO_ENCODER_TYPES_H__ */

