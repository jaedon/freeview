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
 * $brcm_Workfile: nexus_simple_video_decoder.h $
 * $brcm_Revision: 12 $
 * $brcm_Date: 8/6/12 4:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/simple_decoder/include/nexus_simple_video_decoder.h $
 * 
 * 12   8/6/12 4:36p erickson
 * SW7425-3572: add userdata capture
 * 
 * 11   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 10   2/9/12 12:38p erickson
 * SW7420-2237: remove NEXUS_SimpleVideoDecoder_SetWindowSettings
 * 
 * 9   11/2/11 4:21p erickson
 * SW7420-1992: add Settings, ExtendedSettings, SetStartPts
 *
 * 8   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 *
 * 7   10/3/11 9:27a erickson
 * SW7420-1148: deprecate NEXUS_SimpleWindowSettings
 *
 * 6   9/22/11 4:43p erickson
 * SW7420-1965: add simple decoder support to nexus playback module
 *
 * 5   6/1/11 2:49p erickson
 * SW7425-614: add
 *  NEXUS_SimpleVideoDecoderStartSettings.enhancementPidChannel
 *
 * 4   11/1/10 2:29p erickson
 * SW7420-1135: add TSM support
 *
 * 3   10/29/10 4:33p erickson
 * SW7420-1135: update
 *
 * 2   10/22/10 11:17a erickson
 * SW7420-1135: allow runtime changes of maxWidth/maxHeight for mosaic
 *  decodes
 *
 * 1   10/13/10 10:32a erickson
 * SW7420-1135: add
 *
 **************************************************************************/
#ifndef NEXUS_SIMPLE_VIDEO_DECODER_H__
#define NEXUS_SIMPLE_VIDEO_DECODER_H__

#include "nexus_types.h"
#include "nexus_video_decoder_types.h"
#include "nexus_display_types.h"
#include "nexus_simple_decoder_types.h"
#include "nexus_pid_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
SimpleVideoDecoder provides a self-configuring video decoder for untrusted clients.
It is used in multi-process systems where garbage collection is required.
It is able to dynamically allocate memory for start-time max width/height.

HD/SD simul and main/pip configurations are supported. Dual output configurations (i.e. TV1 and TV2) are currently not supported.
**/

/**
Summary:
**/
typedef struct NEXUS_SimpleVideoDecoder *NEXUS_SimpleVideoDecoderHandle;

/**
Summary:
**/
NEXUS_SimpleVideoDecoderHandle NEXUS_SimpleVideoDecoder_Acquire( /* attr{release=NEXUS_SimpleVideoDecoder_Release}  */
    unsigned index
    );

/**
Summary:
Stops decode and releases the handle. The handle can no longer be used.
**/
void NEXUS_SimpleVideoDecoder_Release(
    NEXUS_SimpleVideoDecoderHandle handle
    );

/**
Summary:
**/
typedef struct NEXUS_SimpleVideoDecoderStartSettings
{
	NEXUS_VideoDecoderStartSettings settings;
    unsigned maxWidth, maxHeight; /* max source size dimensions that will be decoded in this session.
                                     set to 0,0 if you want to use defaults. */
} NEXUS_SimpleVideoDecoderStartSettings;

/**
Summary:
**/
void NEXUS_SimpleVideoDecoder_GetDefaultStartSettings(
    NEXUS_SimpleVideoDecoderStartSettings *pSettings /* [out] */
    );

/**
Summary:
Start decode

Description:
Internally, this will set up the connection to the display, allocate memory and start decode.
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_Start(
    NEXUS_SimpleVideoDecoderHandle handle,
    const NEXUS_SimpleVideoDecoderStartSettings *pSettings
    );

/**
Summary:
Stop decode, but don't free memory.

Description:
This allows the last picture to remain visible. Useful for channel change.
**/
void NEXUS_SimpleVideoDecoder_Stop(
    NEXUS_SimpleVideoDecoderHandle handle
    );

/**
Summary:
Stop decode and free all memory.

Description:
The last picture is no longer available.
**/
void NEXUS_SimpleVideoDecoder_StopAndFree(
    NEXUS_SimpleVideoDecoderHandle handle
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_GetStatus(
    NEXUS_SimpleVideoDecoderHandle handle,
    NEXUS_VideoDecoderStatus *pStatus   /* [out] Note that the regular NEXUS_VideoDecoder structure is used */
    );

/**
Summary:
**/
void NEXUS_SimpleVideoDecoder_Flush(
    NEXUS_SimpleVideoDecoderHandle handle
    );

/**
Summary:
**/
void NEXUS_SimpleVideoDecoder_GetTrickState(
    NEXUS_SimpleVideoDecoderHandle handle,
    NEXUS_VideoDecoderTrickState *pSettings   /* [out] Note that the regular NEXUS_VideoDecoder structure is used */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_SetTrickState(
    NEXUS_SimpleVideoDecoderHandle handle,
    const NEXUS_VideoDecoderTrickState *pSettings   /* Note that the regular NEXUS_VideoDecoder structure is used */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_FrameAdvance(
    NEXUS_SimpleVideoDecoderHandle handle
    );

/**
Summary:
**/
void NEXUS_SimpleVideoDecoder_GetPlaybackSettings(
    NEXUS_SimpleVideoDecoderHandle handle,
    NEXUS_VideoDecoderPlaybackSettings *pSettings /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_SetPlaybackSettings(
    NEXUS_SimpleVideoDecoderHandle handle,
    const NEXUS_VideoDecoderPlaybackSettings *pSettings
    );

typedef struct NEXUS_SimpleVideoDecoderClientStatus
{
    bool enabled; /* set true if the underlying video decoder is available */
} NEXUS_SimpleVideoDecoderClientStatus;

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_GetClientStatus(
    NEXUS_SimpleVideoDecoderHandle handle,
    NEXUS_SimpleVideoDecoderClientStatus *pStatus
    );

/**
Summary:
**/
void NEXUS_SimpleVideoDecoder_GetSettings(
    NEXUS_SimpleVideoDecoderHandle handle,
    NEXUS_VideoDecoderSettings *pSettings
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_SetSettings(
    NEXUS_SimpleVideoDecoderHandle handle,
    const NEXUS_VideoDecoderSettings *pSettings
    );

/**
Summary:
**/
void NEXUS_SimpleVideoDecoder_GetExtendedSettings(
    NEXUS_SimpleVideoDecoderHandle handle,
    NEXUS_VideoDecoderExtendedSettings *pSettings
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_SetExtendedSettings(
    NEXUS_SimpleVideoDecoderHandle handle,
    const NEXUS_VideoDecoderExtendedSettings *pSettings
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_SetStartPts(
    NEXUS_SimpleVideoDecoderHandle handle,
    uint32_t pts
    );

/**
Summary:
Capture raw user data

Description:
Data is guaranteed to be returned in whole blocks. That is, NEXUS_UserDataHeader plus payload.
Zero, one or more blocks may be return in each call.
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_ReadUserDataBuffer(
    NEXUS_SimpleVideoDecoderHandle handle,
    void *pBuffer,      /* [out] attr{nelem=bufferSize;nelem_out=pBytesCopied;reserved=1024} pointer for userdata buffer */
    size_t bufferSize,  /* number of bytes of userdata that can be copied */
    size_t *pBytesCopied /* [out] number of bytes of userdata that was copied */
    );
    
/**
Summary:
Flush user data
**/
void NEXUS_SimpleVideoDecoder_FlushUserData(
    NEXUS_SimpleVideoDecoderHandle videoDecoder
    );
    
#ifdef __cplusplus
}
#endif

#endif
