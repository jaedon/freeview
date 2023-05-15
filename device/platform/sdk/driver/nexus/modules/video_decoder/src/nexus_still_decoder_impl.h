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
 * $brcm_Workfile: nexus_still_decoder_impl.h $
 * $brcm_Revision: 8 $
 * $brcm_Date: 6/19/12 12:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/7400/src/nexus_still_decoder_impl.h $
 * 
 * 8   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 7   11/3/11 10:59a erickson
 * SW7346-486: add open/start time codec and buffer control, add
 *  NEXUS_StillDecoder_RequestMemorySize
 * 
 * 6   9/8/10 12:25p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * 5   6/23/10 12:11p erickson
 * SW7550-453: refactor NEXUS_StripedSurface to use
 *  nexus_striped_surface.h API
 * 
 * 4   3/8/08 7:08a erickson
 * PR40103: convert to TaskCallback and IsrCallback
 * 
 * 3   2/26/08 2:42p erickson
 * PR39786: added Stop
 * 
 * 2   2/25/08 3:01p erickson
 * PR39786: update
 * 
 * 1   2/20/08 4:57p erickson
 * PR39786: initial impl
 *
 **************************************************************************/
#ifndef NEXUS_STILL_DECODER_IMPL_H__
#define NEXUS_STILL_DECODER_IMPL_H__

#include "nexus_video_decoder_module.h"
#include "priv/nexus_core_video.h"

typedef struct NEXUS_StillDecoder_P_Interface {
    void (*Close)( NEXUS_StillDecoderHandle handle);
    NEXUS_Error (*Start)( NEXUS_StillDecoderHandle handle, const NEXUS_StillDecoderStartSettings *pSettings);
    void (*Stop)( NEXUS_StillDecoderHandle handle);
    NEXUS_Error (*GetStatus)( NEXUS_StillDecoderHandle handle, NEXUS_StillDecoderStatus *pStatus );
    NEXUS_Error (*GetStripedSurface)( NEXUS_StillDecoderHandle handle, NEXUS_StripedSurfaceHandle *pStripedSurface);
} NEXUS_StillDecoder_P_Interface;
extern const NEXUS_StillDecoder_P_Interface NEXUS_StillDecoder_P_Interface_Avd;

struct NEXUS_StillDecoder {
    NEXUS_OBJECT(NEXUS_StillDecoder);
    const NEXUS_StillDecoder_P_Interface *interface;
    NEXUS_VideoDecoderHandle parentDecoder;
    unsigned index;
    NEXUS_StillDecoderOpenSettings openSettings;
    NEXUS_StillDecoderStartSettings settings;
    bool started;
    NEXUS_Time startTime;
    NEXUS_StillDecoderStatus status;
    NEXUS_RaveHandle rave;
    NEXUS_TimerHandle timer;
    
    NEXUS_TaskCallbackHandle endCodeFoundCallback;
    NEXUS_IsrCallbackHandle stillPictureReadyCallback;
    
    BXVD_ChannelHandle xvdChannel;

    struct {
        bool recreate;
        BMEM_Heap_Handle mem;
        NEXUS_StripedSurfaceCreateSettings createSettings;
        NEXUS_StripedSurfaceHandle handle;
    } stripedSurface;
    
    /* user allocated output buffer */
    struct {
        void *mem;
        unsigned size;
        unsigned maxHeight; 
        BMEM_Heap_Handle heap;
    } buffer;
};

void NEXUS_StillDecoder_P_CheckForEndCode( void *context );
void NEXUS_VideoDecoder_P_StillReady_isr(void *context, int param, void *data);
void NEXUS_VideoDecoder_P_StillReady(void *context);

NEXUS_StillDecoderHandle NEXUS_StillDecoder_P_Open_Avd( NEXUS_VideoDecoderHandle parentDecoder, unsigned index, const NEXUS_StillDecoderOpenSettings *pSettings); 
void NEXUS_StillDecoder_P_Close_Avd( NEXUS_StillDecoderHandle handle);
NEXUS_Error NEXUS_StillDecoder_P_Start_Avd( NEXUS_StillDecoderHandle handle, const NEXUS_StillDecoderStartSettings *pSettings);
void NEXUS_StillDecoder_P_Stop_Avd( NEXUS_StillDecoderHandle handle);
NEXUS_Error NEXUS_StillDecoder_P_GetStatus_Avd( NEXUS_StillDecoderHandle handle, NEXUS_StillDecoderStatus *pStatus );
NEXUS_Error NEXUS_StillDecoder_P_GetStripedSurface_Avd( NEXUS_StillDecoderHandle handle, NEXUS_StripedSurfaceHandle *pStripedSurface);

#endif
