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
 * $brcm_Workfile: nexus_video_decoder_init.h $
 * $brcm_Revision: 23 $
 * $brcm_Date: 10/5/12 11:50a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/include/nexus_video_decoder_init.h $
 * 
 * 23   10/5/12 11:50a erickson
 * SW7435-400: clarify how app can set module settings
 * 
 * 22   3/15/12 5:11p erickson
 * SW7425-2607: increase for 7435
 * 
 * 21   3/15/12 9:21a erickson
 * SW7425-2607: use NEXUS_MAX_XVD_DEVICES
 * 
 * 20   3/14/12 11:52a erickson
 * SWDEPRECATED-2425: update comments on picture buffer heap
 * 
 * 19   3/13/12 5:33p erickson
 * SW7425-2607: remove #ifndef, add temp BCHP_CHIP list
 * 
 * 18   3/13/12 4:42p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 17   1/24/12 2:44p vsilyaev
 * SW7425-2258: Added Nexus video decoder extension to decode MJPEG
 * 
 * 16   9/9/11 12:36p erickson
 * SWDEPRECATED-2425: fix comments
 * 
 * 15   6/20/11 5:41p jgarrett
 * SW7425-409: Merging VP6 support to main branch
 * 
 * SW7425-409/1   6/6/11 7:16p jgarrett
 * SW7425-409: Adding DSP video decoder extensions for VDE
 * 
 * 14   5/12/11 3:10p erickson
 * SW7425-466: add avdMapping[], rework hardcoded array sizes
 * 
 * SW7425-466/1   5/11/11 12:59p hongtaoz
 * SW7425-466: rework avd/mfd/decoder channel mapping;
 * 
 * 13   2/22/11 10:22a erickson
 * SW7422-274: add NEXUS_VideoDecoderOpenSettings.supportedCodecs[]
 *
 * 12   10/19/10 12:28p erickson
 * SW7422-79: add NEXUS_VideoDecoderModuleSettings.mfdMapping[]
 *
 * 11   9/29/10 3:30p vsilyaev
 * SW7422-65: User per AVD core heap sizes
 *
 * 10   9/8/10 12:25p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * 9   7/14/10 10:33a erickson
 * SW7405-4621: replace nexus_base.h with nexus_types.h
 *
 * 8   3/10/10 3:00p erickson
 * SW7405-3965: deprecrate NEXUS_VideoDecoderModuleSettings.numDecodes,
 *  etc. clean up avd/xvd channel/mfd index assignment in
 *  NEXUS_VideoDecoder_P_Open.
 *
 * 7   4/20/09 9:22a erickson
 * PR53868: added NEXUS_VideoDecoderModuleSettings.hostAccessibleHeapIndex
 *
 * 6   4/6/09 9:07a erickson
 * PR53868: add NEXUS_VideoDecoderModuleSettings.avdHeapIndex[]
 *
 * 5   3/30/09 10:36a erickson
 * PR50748: support the secure heap using module settings handle
 *
 * 4   10/28/08 10:45a erickson
 * PR35457: update comments
 *
 * 3   9/2/08 4:58p erickson
 * PR46457: added runtime params for max resources
 *
 * 2   2/7/08 11:02a jgarrett
 * PR 39270: Adding selectable heap sizes
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/8   11/28/07 2:11p erickson
 * PR35457: doc update
 *
 * Nexus_Devel/7   11/28/07 10:37a erickson
 * PR35457: doc update
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_DECODER_INIT_H__
#define NEXUS_VIDEO_DECODER_INIT_H__

/*=========================================
The VideoDecoder module provides Interfaces for decode of compressed digital video.

It provides a single interface of the same name. See NEXUS_VideoDecoder_Open and related functions.
===========================================*/

#include "nexus_types.h"

#define NEXUS_MAX_XVD_DEVICES 2
#define NEXUS_MAX_VIDEO_DECODERS 4

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Settings used to configure the VideoDecoder module.

Description:

See Also:
NEXUS_VideoDecoderModule_GetDefaultSettings
NEXUS_VideoDecoderModule_Init
**/
typedef struct NEXUS_VideoDecoderModuleSettings
{
    NEXUS_ModuleHandle transport; /* Handle to Transport module. See NEXUS_Transport_Init. */

    unsigned avdHeapIndex[NEXUS_MAX_XVD_DEVICES]; /* Sets the heap[] index to be used per AVD core for picture data. */
    unsigned hostAccessibleHeapIndex; /* Sets the heap[] index to be used for loading AVD firmware and for userdata, not for picture data. */
    unsigned mfdMapping[NEXUS_MAX_VIDEO_DECODERS]; /* Maps video decoders to MFD block. Default is [0]=0, [1]=1. Note one AVD device could support two decoders/MFDs */
    unsigned avdMapping[NEXUS_MAX_VIDEO_DECODERS]; /* Maps video decoders to AVD device. Default is [0]=0, [1]=1. Note one AVD device could support two decoders/MFDs */

    NEXUS_VideoCodec supportedCodecs[NEXUS_VideoCodec_eMax]; /* Deprecated. Use NEXUS_VideoDecoderSettings.supportedCodecs[] instead. */
    NEXUS_VideoFormat maxDecodeFormat; /* Max resolution that will be decoded. */
    NEXUS_VideoFormat maxStillDecodeFormat; /* Max resolution that will be used for still decoder. Use eUnknown if still decode is not needed. */

    int numDecodes; /* unused */
    int numStillDecodes; /* unused */
    int numMosaicDecodes; /* unused */
    struct
    {
        /* This structure defines heap sizes in bytes per hardware decoder.
           These values should be calculated based upon the maximum number of concurrent decodes
           you wish to perform, and must be appropriately sized based upon stream codec and resolution.
           Please see the chip's memory worksheet, provided in the release or upon request, for recommended numbers.
           Please see magnum/portinginterface/xvd/<CHIP>/bxvd.h for more details on how to calculate these values correctly.
           If 0 is specified for all values will use defaults that will support one HD video and one HD still decode per HW decoder. */
        unsigned general;   /* General decoder heap.
                               This is used for picture buffer allocation. The memory worksheet reports this as "General Heap" and "Still Heap".
                               If you are using shared analog/digital memory, the only allocation that should be specified here is the still picture decode heap size.
                               Firmware is not allocated from this size. RAVE CDB/ITB buffers are not allocated from this size. */
        unsigned secure;    /* Secure decoder heap. This is only used for SVP systems where the host is prevented from reading clear, compressed data. For
                               non-SVP systems, set to zero.*/
        unsigned picture;   /* Picture buffer heap. This heap typically has no CPU memory mapping (NEXUS_MemoryType_eDeviceOnly). 
                               It is required for non-UMA systems and for large memory systems with limited virtual address space. */
    } heapSize[NEXUS_MAX_XVD_DEVICES];
    bool watchdogEnabled; /* Enable and process watchdog interrupts. Watchdog interrupts occur when the decoder stalls and must be reset. */

    NEXUS_HeapHandle secureHeap;    /* optional. if set, transport will allocate all RAVE contexts from this heap. */
    NEXUS_ModuleHandle audio; /* Handle to Audio module. See NEXUS_AudioModule_Init. needed for ZSP and DSP video decode. */
    NEXUS_ModuleHandle pictureDecoder; /* Handle to PictureDecoder module. See NEXUS_PictureDecoderModule_Init. */
} NEXUS_VideoDecoderModuleSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.

See Also:
NEXUS_VideoDecoderModule_Init
**/
void NEXUS_VideoDecoderModule_GetDefaultSettings(
    NEXUS_VideoDecoderModuleSettings *pSettings /* [out] */
    );

/**
Summary:
Initialize the VideoDecoder module.

Description:
This function is called by NEXUS_Platform_Init, not by applications.
If you want to modify these settings from your application, you can do this 
through NEXUS_PlatformSettings as follows:

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.videoDecoderModuleSettings.xxx = xxx;
    NEXUS_Platform_Init(&platformSettings);

**/
NEXUS_ModuleHandle NEXUS_VideoDecoderModule_Init(
    const NEXUS_VideoDecoderModuleSettings *pSettings
    );

/**
Summary:
Uninitialize the VideoDecoder module.

Description:
Called by NEXUS_Platform_Uninit
**/
void NEXUS_VideoDecoderModule_Uninit(void);

#ifdef __cplusplus
}
#endif

#endif
