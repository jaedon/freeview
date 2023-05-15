/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_decoder_trick.h $
* $brcm_Revision: 9 $
* $brcm_Date: 2/15/11 2:22p $
*
* API Description:
*   API name: AudioDecoder Trick
*    API for audio decoder trick-play.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/include/nexus_audio_decoder_trick.h $
* 
* 9   2/15/11 2:22p erickson
* SW7420-1135: refactor into nexus_audio_decoder_types.h
*
* 8   2/12/09 5:36p jgarrett
* PR 46135: Adding TruVolume
*
* 7   5/22/08 8:28p jgarrett
* PR 42974: Adding mute flag
*
* 6   3/14/08 10:38a jgarrett
* PR 40120: Adding TSM option
*
* 5   3/6/08 10:46a erickson
* PR40351: moved NEXUS_NORMAL_DECODE_RATE from audio/video to base
*
* 4   1/23/08 8:36p vobadm
* PR35457: update docs
*
* 3   1/23/08 5:16p erickson
* PR35457: update docs
*
* 2   1/23/08 2:39p erickson
* PR35457: update docs
*
* 1   1/18/08 2:22p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/3   12/4/07 2:12p vsilyaev
* PR 36404: Use structure to conrol audio trickmodes
*
* Nexus_Devel/2   10/24/07 5:40p vsilyaev
* PR 36404: Added audio decode trick mode
*
* Nexus_Devel/1   10/24/07 12:36p vsilyaev
* PR 36404: Trick mode extension
*
*
***************************************************************************/
#ifndef NEXUS_AUDIO_DECODER_TRICK_H__
#define NEXUS_AUDIO_DECODER_TRICK_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_audio_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Get the current trick mode state.
**/
void NEXUS_AudioDecoder_GetTrickState(
    NEXUS_AudioDecoderHandle decoder,
    NEXUS_AudioDecoderTrickState *pState /* [out] current trick mode state */
    );


/**
Summary:
Set a new trick mode state.

Description:
You should call NEXUS_AudioDecoder_GetTrickState, modify any settings you want, then call NEXUS_AutioDecoder_SetTrickState.
**/
NEXUS_Error NEXUS_AudioDecoder_SetTrickState(
    NEXUS_AudioDecoderHandle decoder,
    const NEXUS_AudioDecoderTrickState *pTrickState
    );


/***************************************************************************
Summary:
    Advances audio decoder to the target PTS

Description:
    NEXUS_AudioDecoder_Advance causes audio decoder to drop compressed data,
    untill it finds data with timestamp greater then target PTS.

See Also:
    NEXUS_AudioDecoder_Pause
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_Advance(
    NEXUS_AudioDecoderHandle decoder,
    uint32_t targetPts /* PTS in 45Khz (MPEG-2) or 27MHz domain */
    );

#ifdef __cplusplus
}
#endif

#endif  /* NEXUS_AUDIO_DECODER_TRICK_H__ */

