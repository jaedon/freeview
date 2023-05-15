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
 * $brcm_Workfile: nexus_video_decoder_zsp_avd.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/7/12 11:56a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/zspdecode_ext/src/nexus_video_decoder_zsp_avd.c $
 * 
 * 3   9/7/12 11:56a erickson
 * SW7435-349: add standard header
 * 
 * 2   9/17/10 2:42p jtna
 * SW7468-295: Replace all instances of deprecated variable
 *  g_NEXUS_videoDecoderModuleSettings.numDecodes with
 *  NEXUS_NUM_VIDEO_DECODERS
 * 
 * 1   9/8/10 11:47a vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * SW7468-129/2   3/5/10 7:31p vsilyaev
 * SW7468-129: Added hooks for soft video decoder
 * 
 * SW7468-129/1   2/26/10 7:12p vsilyaev
 * SW7468-129: Added ZSP video decoder stub
 * 
 **************************************************************************/
#include "nexus_video_decoder_module.h"
#include "nexus_still_decoder_impl.h"

BDBG_MODULE(nexus_video_decoder_zsp_avd);

NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_OpenMosaic( unsigned parentIndex, unsigned index, const NEXUS_VideoDecoderOpenMosaicSettings *pOpenSettings )
{
    return NEXUS_VideoDecoder_P_OpenMosaic_Avd( parentIndex, index, pOpenSettings );
}

NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_Open( unsigned index, const NEXUS_VideoDecoderOpenSettings *pOpenSettings)
{
    if(index<NEXUS_NUM_VIDEO_DECODERS) {
        return NEXUS_VideoDecoder_P_Open_Avd(index, pOpenSettings);
    } else if (index<(NEXUS_NUM_VIDEO_DECODERS+NEXUS_NUM_ZSP_VIDEO_DECODERS)) {
        return NEXUS_VideoDecoder_P_Open_Zsp(index - NEXUS_NUM_VIDEO_DECODERS, pOpenSettings);
#if NEXUS_NUM_SOFT_VIDEO_DECODERS
    } else if (index<NEXUS_NUM_VIDEO_DECODERS+NEXUS_NUM_ZSP_VIDEO_DECODERS+NEXUS_NUM_SOFT_VIDEO_DECODERS) {
        return NEXUS_VideoDecoder_P_Open_Soft(index - (NEXUS_NUM_VIDEO_DECODERS+NEXUS_NUM_ZSP_VIDEO_DECODERS), pOpenSettings);
#endif
    } else {
        (void)BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
}

NEXUS_ModuleHandle NEXUS_VideoDecoderModule_Init(const NEXUS_VideoDecoderModuleSettings *pSettings)
{
    NEXUS_Error rc;
    rc = NEXUS_VideoDecoderModule_P_Init_Zsp(pSettings);
    if(rc!=NEXUS_SUCCESS) {
        (void)BERR_TRACE(rc);
        return NULL;
    }
    return NEXUS_VideoDecoderModule_P_Init_Avd(pSettings);
}

void NEXUS_VideoDecoderModule_Uninit(void)
{
    NEXUS_VideoDecoderModule_P_Uninit_Avd();
    NEXUS_VideoDecoderModule_P_Uninit_Zsp();
}

