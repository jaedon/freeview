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
 * $brcm_Workfile: nexus_picture_decoder_module.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 6/19/12 12:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/picture_decoder/3548/src/nexus_picture_decoder_module.h $
 * 
 * 7   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 6   2/28/12 1:25p gmohile
 * SW7425-2401 : Picture decoder standby support
 * 
 * 5   2/27/12 12:01p ahulse
 * SW7425-2112: fix structure types, immediate stop
 * 
 * 4   2/21/12 11:15a ahulse
 * SW7425-2112: merge SID mjpeg changes to main
 * 
 * SW7425-2112/1   2/16/12 4:52p fbasso
 * SW7425-2112: updated nexus picture decoder to reflect changes to
 *  bsid.h.
 *
 * 3   1/26/12 2:27p vsilyaev
 * SW7425-2258: Moved NEXUS_PictureDecoder_P_HWState into the external
 *  symbol
 * 
 * 2   1/24/12 2:44p vsilyaev
 * SW7425-2258: Added Nexus video decoder extension to decode MJPEG
 * 
 * 1   4/11/08 5:51p vsilyaev
 * PR 40870: Hardware JPEG decoder
 * 
 **************************************************************************/
#ifndef NEXUS_PICTURE_DECODER_MODULE_H__
#define NEXUS_PICTURE_DECODER_MODULE_H__

#include "nexus_picture_decoder_thunks.h"
#include "nexus_base.h"
#include "nexus_picture_decoder.h"
#include "nexus_picture_decoder_init.h"
#if NEXUS_NUM_SID_VIDEO_DECODERS
#include "nexus_picture_decoder_sid_video_decoder_module.h"
#endif
#include "bsid.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SID_DECODE_CHANNELS BSID_MAX_CHANNELS  /* number of simultaneous decode channels in SID */

typedef struct NEXUS_PictureDecoder_P_HWState {
    BSID_Handle                sid;
    int                        nrSidChOpen;
    BSID_ChannelHandle         sidCh; 
    BSID_OpenChannelSettings       sidChOpenSettings;
    NEXUS_PictureDecoderHandle opened[SID_DECODE_CHANNELS];
    NEXUS_PictureDecoderHandle acquired[SID_DECODE_CHANNELS];
} NEXUS_PictureDecoder_P_HWState;


typedef struct NEXUS_PictureDecoder_P_ModuleState {
    NEXUS_ModuleHandle module;
    NEXUS_PictureDecoderModuleSettings settings;
    NEXUS_PictureDecoder_P_HWState     hwState;
} NEXUS_PictureDecoder_P_ModuleState;

extern NEXUS_PictureDecoder_P_ModuleState g_NEXUS_PictureDecoder_P_ModuleState;

#define NEXUS_MODULE_SELF g_NEXUS_PictureDecoder_P_ModuleState.module

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_PictureDecoder);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_PICTURE_DECODER_MODULE_H__ */

