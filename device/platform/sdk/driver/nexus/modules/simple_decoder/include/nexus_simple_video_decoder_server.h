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
 * $brcm_Workfile: nexus_simple_video_decoder_server.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 3/13/12 4:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/simple_decoder/include/nexus_simple_video_decoder_server.h $
 * 
 * 5   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 4   2/9/12 12:38p erickson
 * SW7420-2237: remove NEXUS_SimpleVideoDecoder_SetWindowSettings
 * 
 * 3   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 * 
 * 2   12/2/10 3:56p erickson
 * SW7420-1227: allow virtual (offscreen) window coordinates
 * 
 * 1   10/13/10 10:32a erickson
 * SW7420-1135: add
 *
 **************************************************************************/
#ifndef NEXUS_SIMPLE_VIDEO_DECODER_SERVER_H__
#define NEXUS_SIMPLE_VIDEO_DECODER_SERVER_H__

#include "nexus_types.h"
#include "nexus_simple_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:

Simple decoder assumes videoDecoder is already connected to the windows
**/
typedef struct NEXUS_SimpleVideoDecoderServerSettings
{
    bool enabled;
    NEXUS_SimpleDecoderDisableMode disableMode;
    NEXUS_VideoDecoderHandle videoDecoder; /* by giving the VideoDecoder handle, the caller gives up all ability to any VideoDecoder API directly */
    NEXUS_VideoWindowHandle window[NEXUS_MAX_DISPLAYS]; /* by giving the VideoWindow handle, the caller gives up the ability to call 
        NEXUS_VideoWindow_AddInput, RemoveInput and RemoveAllInputs. the caller can still use other VideoWindow API's. for instance, 
        it can call NEXUS_VideoWindow_SetSettings to move the video window. */
} NEXUS_SimpleVideoDecoderServerSettings;

/**
Summary:
**/
void NEXUS_SimpleVideoDecoder_GetDefaultServerSettings(
    NEXUS_SimpleVideoDecoderServerSettings *pSettings /* [out] */
    );

/**
Summary:
Server could set up multiple SimpleVideoDecoders for one VideoDecoder.
This would allow it to create multiple ways of using the same decoder.
**/
NEXUS_SimpleVideoDecoderHandle NEXUS_SimpleVideoDecoder_Create( /* attr{destructor=NEXUS_SimpleVideoDecoder_Destroy}  */
    unsigned index,
    const NEXUS_SimpleVideoDecoderServerSettings *pSettings
    );

/**
Summary:
Server is responsible for ensuring that all clients have called NEXUS_SimpleVideoDecoder_Release.
If any handle is not released beforehand, there will likely be system failure.
If a client will not release its handle, the client process should be forcibly terminated before calling Destroy.
**/
void NEXUS_SimpleVideoDecoder_Destroy(
    NEXUS_SimpleVideoDecoderHandle handle
    );

/**
Summary:
**/
void NEXUS_SimpleVideoDecoder_GetServerSettings(
    NEXUS_SimpleVideoDecoderHandle handle,
    NEXUS_SimpleVideoDecoderServerSettings *pSettings /* [out] */
    );

/**
Summary:
This might cause a stop/start, unbeknownst to the client
Even the VideoDecoder could be taken away while Simple is acquired and even started
**/
NEXUS_Error NEXUS_SimpleVideoDecoder_SetServerSettings(
    NEXUS_SimpleVideoDecoderHandle handle,
    const NEXUS_SimpleVideoDecoderServerSettings *pSettings
    );

#ifdef __cplusplus
}
#endif

#endif
