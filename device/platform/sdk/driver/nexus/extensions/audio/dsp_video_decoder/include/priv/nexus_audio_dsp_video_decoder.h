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
 * $brcm_Workfile: nexus_audio_dsp_video_decoder.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 11:55a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/audio/dsp_video_decoder/include/priv/nexus_audio_dsp_video_decoder.h $
 * 
 * 2   9/7/12 11:55a erickson
 * SW7435-349: add standard header
 * 
 * 1   6/20/11 5:42p jgarrett
 * SW7425-409: Merging VP6 support to main branch
 * 
 * SW7425-409/1   6/6/11 7:17p jgarrett
 * SW7425-409: Adding DSP video decoder extensions for VDE
 * 
 **************************************************************************/
#ifndef NEXUS_AUDIO_DSP_VIDEO_DECODER_H__
#define NEXUS_AUDIO_DSP_VIDEO_DECODER_H__

#include "bxdm_decoder.h"

typedef struct NEXUS_DspVideoDecoder *NEXUS_DspVideoDecoderHandle;

typedef struct NEXUS_DspVideoDecoderOpenSettings
{
    BMEM_Heap_Handle pictureHeap;
} NEXUS_DspVideoDecoderOpenSettings;

typedef struct NEXUS_DspVideoDecoderStartSettings
{
    NEXUS_RaveHandle raveContext;
    NEXUS_VideoCodec codec;             /* The type of video being decoded. */
} NEXUS_DspVideoDecoderStartSettings;

void NEXUS_DspVideoDecoder_GetDefaultStartSettings_priv( NEXUS_DspVideoDecoderStartSettings *pSettings );
void NEXUS_DspVideoDecoder_GetDefaultOpenSettings_priv( NEXUS_DspVideoDecoderOpenSettings *pSettings );
NEXUS_DspVideoDecoderHandle NEXUS_DspVideoDecoder_Open_priv(unsigned index, const NEXUS_DspVideoDecoderOpenSettings *openSettings);
void NEXUS_DspVideoDecoder_Close_priv(NEXUS_DspVideoDecoderHandle decoder);
NEXUS_Error NEXUS_DspVideoDecoder_Start_priv(NEXUS_DspVideoDecoderHandle decoder, const NEXUS_DspVideoDecoderStartSettings *startSettings);
void NEXUS_DspVideoDecoder_Stop_priv(NEXUS_DspVideoDecoderHandle decoder);
NEXUS_Error NEXUS_DspVideoDecoder_Flush_priv(NEXUS_DspVideoDecoderHandle decoder);
void NEXUS_DspVideoDecoder_GetDecoderInterface_priv(NEXUS_DspVideoDecoderHandle decoder, const BXDM_Decoder_Interface **decoderInterface, void **decoderContext);
void NEXUS_DspVideoDecoder_GetRaveSettings_priv(NEXUS_RaveOpenSettings *raveSettings);

#endif /* NEXUS_AUDIO_DSP_VIDEO_DECODER_H__ */

