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
 * $brcm_Workfile: nexus_simple_audio_decoder.h $
 * $brcm_Revision: 12 $
 * $brcm_Date: 8/21/12 5:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/simple_decoder/include/nexus_simple_audio_decoder.h $
 * 
 * 12   8/21/12 5:23p erickson
 * SW7231-958: only #include audio headers with structs/enums
 * 
 * 11   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 10   6/6/12 11:41a erickson
 * SW7425-2981: rework simple audio decoder input[] configuration
 * 
 * 9   1/12/12 1:01p vsilyaev
 * SW7425-2071:  Added extended functionality
 * 
 * 8   12/19/11 11:52a erickson
 * SW7231-562: clarify unused secondary decoder
 * 
 * 7   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 * 
 * 6   9/22/11 4:43p erickson
 * SW7420-1965: add simple decoder support to nexus playback module
 * 
 * 5   2/15/11 2:23p erickson
 * SW7420-1135: use nexus_audio_decoder_types.h
 *
 * 4   1/24/11 3:58p erickson
 * SW7405-5105: add simple audio playback
 *
 * 3   11/17/10 3:48p erickson
 * SW7420-1135: add comment
 *
 * 2   11/1/10 2:29p erickson
 * SW7420-1135: add TSM support
 *
 * 1   10/13/10 10:32a erickson
 * SW7420-1135: add
 *
 **************************************************************************/
#ifndef NEXUS_SIMPLE_AUDIO_DECODER_H__
#define NEXUS_SIMPLE_AUDIO_DECODER_H__

#include "nexus_types.h"
#include "nexus_simple_video_decoder.h"
#include "nexus_audio_decoder_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
SimpleAudioDecoder is an already-opened, pre-connected audio decoder.
It is used in multi-process systems where garbage collection is required.

Dual output configurations (i.e. TV1 and TV2) are not supported.
**/

/**
Summary:
**/
typedef struct NEXUS_SimpleAudioDecoder *NEXUS_SimpleAudioDecoderHandle;

/**
Summary:
**/
NEXUS_SimpleAudioDecoderHandle NEXUS_SimpleAudioDecoder_Acquire( /* attr{release=NEXUS_SimpleAudioDecoder_Release}  */
    unsigned index
    );

/**
Summary:
**/
void NEXUS_SimpleAudioDecoder_Release(
    NEXUS_SimpleAudioDecoderHandle handle
    );

/**
Summary:
**/
typedef struct NEXUS_SimpleAudioDecoderStartSettings
{
    NEXUS_AudioDecoderStartSettings primary;
    NEXUS_AudioDecoderStartSettings secondary; /* optional second pid and codec for secondary decoder. if secondary.pidChannel == NULL, primary start settings will be used for secondary decoder */
} NEXUS_SimpleAudioDecoderStartSettings;

/**
Summary:
**/
void NEXUS_SimpleAudioDecoder_GetDefaultStartSettings(
    NEXUS_SimpleAudioDecoderStartSettings *pSettings /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleAudioDecoder_Start(
    NEXUS_SimpleAudioDecoderHandle handle,
    const NEXUS_SimpleAudioDecoderStartSettings *pSettings
    );

/**
Summary:
**/
void NEXUS_SimpleAudioDecoder_Stop(
    NEXUS_SimpleAudioDecoderHandle handle
    );

/**
Summary:
Run-time settings

Description:
Changing some of these settings after start may require a stop/start. Nexus will handle this automatically.

pcmOutput will result in an internal restart.
**/
typedef struct NEXUS_SimpleAudioDecoderSettings
{
    NEXUS_AudioDecoderSettings primary;
    NEXUS_AudioDecoderSettings secondary;
} NEXUS_SimpleAudioDecoderSettings;

/**
Summary:
**/
void NEXUS_SimpleAudioDecoder_GetSettings(
    NEXUS_SimpleAudioDecoderHandle handle,
    NEXUS_SimpleAudioDecoderSettings *pSettings /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleAudioDecoder_SetSettings(
    NEXUS_SimpleAudioDecoderHandle handle,
    const NEXUS_SimpleAudioDecoderSettings *pSettings
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleAudioDecoder_GetStatus(
    NEXUS_SimpleAudioDecoderHandle handle,
    NEXUS_AudioDecoderStatus *pStatus   /* [out] Note that the regular NEXUS_AudioDecoder structure is used */
    );

/**
Summary:
**/
void NEXUS_SimpleAudioDecoder_Flush(
    NEXUS_SimpleAudioDecoderHandle handle
    );

/**
Summary:
**/
void NEXUS_SimpleAudioDecoder_GetTrickState(
    NEXUS_SimpleAudioDecoderHandle handle,
    NEXUS_AudioDecoderTrickState *pStatus   /* [out] Note that the regular NEXUS_AudioDecoder structure is used */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleAudioDecoder_SetTrickState(
    NEXUS_SimpleAudioDecoderHandle handle,
    const NEXUS_AudioDecoderTrickState *pStatus   /* Note that the regular NEXUS_AudioDecoder structure is used */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleAudioDecoder_Advance(
    NEXUS_SimpleAudioDecoderHandle handle,
    uint32_t pts
    );


/**
Summary:
**/
typedef enum NEXUS_SimpleAudioDecoderSelector {
    NEXUS_SimpleAudioDecoderSelector_ePrimary,
    NEXUS_SimpleAudioDecoderSelector_eSecondary,
    NEXUS_SimpleAudioDecoderSelector_eMax
}NEXUS_SimpleAudioDecoderSelector;

/***************************************************************************
 * Summary:
 *     Get codec-specific decoder settings
 *     ***************************************************************************/
void NEXUS_SimpleAudioDecoder_GetCodecSettings(
    NEXUS_SimpleAudioDecoderHandle handle,
    NEXUS_SimpleAudioDecoderSelector selector,
    NEXUS_AudioCodec codec,
    NEXUS_AudioDecoderCodecSettings *pSettings  /* [out] settings for specified codec */
    );

/***************************************************************************
 * Summary:
 *     Set codec-specific decoder settings
 *     ***************************************************************************/
NEXUS_Error NEXUS_SimpleAudioDecoder_SetCodecSettings(
    NEXUS_SimpleAudioDecoderHandle handle,
    NEXUS_SimpleAudioDecoderSelector selector,
    const NEXUS_AudioDecoderCodecSettings *pSettings
);





    
#ifdef __cplusplus
}
#endif

#endif
