/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  .  Except as set forth in an Authorized License, Broadcom grants
 *  no license , right to use, or waiver of any kind with respect to the
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
 *  LICENSORS BE LIABLE FOR  CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR  ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_video_decoder_module_zsp.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 11:55a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/zspdecode_ext/src/nexus_video_decoder_module_zsp.h $
 * 
 * 2   9/7/12 11:55a erickson
 * SW7435-349: add standard header
 * 
 * 1   9/8/10 11:47a vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * SW7468-129/6   3/9/10 8:01p vsilyaev
 * SW7468-129: Used static functions where aprorpiate
 * 
 * SW7468-129/5   3/9/10 7:14p vsilyaev
 * SW7468-129: Added functional software video decoder
 * 
 * SW7468-129/4   3/8/10 6:33p vsilyaev
 * SW7468-129: Added basic implementation for video audio decoder
 * 
 * SW7468-129/3   3/8/10 12:49p vsilyaev
 * SW7468-129: Added private interface between audio and video decoders
 * 
 * SW7468-129/2   3/1/10 7:09p vsilyaev
 * SW7468-129: Added private interfaces for ZSP video decoder
 * 
 * SW7468-129/1   2/26/10 7:09p vsilyaev
 * SW7468-129: Added ZSP video decoder stub
 * 
 **************************************************************************/
#ifndef NEXUS_VIDEO_DECODER_MODULE_ZSP_H__
#define NEXUS_VIDEO_DECODER_MODULE_ZSP_H__

#include "priv/nexus_audio_video_decoder.h"



NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_P_Open_Zsp( unsigned index, const NEXUS_VideoDecoderOpenSettings *pOpenSettings);
NEXUS_Error NEXUS_VideoDecoderModule_P_Init_Zsp(const NEXUS_VideoDecoderModuleSettings *pSettings);
void NEXUS_VideoDecoderModule_P_Uninit_Zsp(void);

BDBG_OBJECT_ID_DECLARE(NEXUS_VideoDecoder_Zsp);

typedef struct NEXUS_VideoDecoder_Zsp {
    BDBG_OBJECT(NEXUS_VideoDecoder_Zsp)
    NEXUS_ZspVideoDecoderHandle decoder;
} NEXUS_VideoDecoder_Zsp;


#define LOCK_AUDIO()    NEXUS_Module_Lock(g_NEXUS_videoDecoderModuleSettings.audio)
#define UNLOCK_AUDIO()  NEXUS_Module_Unlock(g_NEXUS_videoDecoderModuleSettings.audio)


extern const NEXUS_VideoDecoder_P_Interface NEXUS_VideoDecoder_P_Interface_Zsp;



#endif /* NEXUS_VIDEO_DECODER_MODULE_ZSP_H__ */

