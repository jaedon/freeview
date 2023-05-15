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
 * $brcm_Workfile: nexus_hdmi_output_priv.h $
 * $brcm_Revision: 12 $
 * $brcm_Date: 4/13/12 4:58p $
 *
 * Module Description:
 *                      HdmiOutput: Specific interfaces for an HDMI/DVI output.
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_output/7400/include/priv/nexus_hdmi_output_priv.h $
 * 
 * 12   4/13/12 4:58p erickson
 * SW7346-737: add NEXUS_HdmiOutput_GetCrcData
 * 
 * SW7346-319/1   7/28/11 2:14p vle
 * SW7346-319: Make sure to set HDMI data transfer mode even when
 *  disconnected.
 * 
 * 10   2/7/11 2:03p erickson
 * SW7422-234: merge
 * 
 * SW7422-234/1   2/1/11 7:36p vle
 * SW7422-234: Fix deep color support for 7422
 * 
 * 9   12/3/10 9:27a erickson
 * SW7550-577: call BHDM_SetHdmiDataTransferMode based on
 *  NEXUS_DisplayTimingGenerator_eHdmiDvo
 * 
 * 8   11/5/10 5:40p jgarrett
 * SW7125-698: Applying HDMI channel status information when it updates
 * 
 * 7   10/8/10 4:30p erickson
 * SW7405-4782: merge
 * 
 * SW7405-4782/1   10/7/10 12:04p vle
 * SW7405-4782: Changing aspect ratio does not require HDCP re-
 *  authentication - TAKE 2. First implementation attempt has a potential
 *  race condition.
 * 
 * 6   9/4/09 5:02p erickson
 * SW7405-2962: add NEXUS_HdmiOutput_SetNotifyDisplay_priv
 *
 * 5   5/4/09 3:40p erickson
 * PR53373: merge HDMI 1.3a support
 *
 * 4   12/17/08 11:38p erickson
 * PR50501: refactor HdmiOutput to remove double BVDC_ApplyChanges and 400
 *  msec wait on format change
 *
 * 3   4/3/08 12:11p erickson
 * PR41302: add MPAA decimation
 *
 * 2   3/28/08 4:24p katrep
 * PR40844: Fixed NEXUS_HdmiOutput_SetAudioParams_priv declaration
 *
 * 1   2/1/08 5:25p jgarrett
 * PR 39017: Adding hdmi_output
 *
 **************************************************************************/
#ifndef NEXUS_HDMI_OUTPUT_PRIV_H__
#define NEXUS_HDMI_OUTPUT_PRIV_H__

#include "nexus_hdmi_output.h"
#include "bavc.h"
#include "bfmt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* VDC -> HDM callback info for rate changes */
void NEXUS_HdmiOutput_VideoRateChange_isr(
    NEXUS_HdmiOutputHandle handle,
    BAVC_VdcDisplay_Info *pDisplayInfo
    );

/* RAP -> HDM callback for sample rate changes */
void NEXUS_HdmiOutput_AudioSampleRateChange_isr(
    NEXUS_HdmiOutputHandle handle,
    BAVC_AudioSamplingRate sampleRate
    );

/* Returns false if the format is not supported, true if it is */
bool NEXUS_HdmiOutput_GetColorimetry_priv(
    NEXUS_HdmiOutputHandle handle,
    BFMT_VideoFmt format,
    BAVC_MatrixCoefficients *pColorimetry
    );

/**
Summary:
Set the DisconnectParams callback
**/
void NEXUS_HdmiOutput_SetDisconnectParams_priv(
    NEXUS_HdmiOutputHandle handle,
    bool masterMode,
    const NEXUS_CallbackDesc *pNotifyDisplay
    );

/* Connect video and set format parameters */
NEXUS_Error NEXUS_HdmiOutput_SetDisplayParams_priv(
    NEXUS_HdmiOutputHandle handle,
    BFMT_VideoFmt format,
    BAVC_MatrixCoefficients colorimetry,
    BFMT_AspectRatio aspectRatio,
    bool masterMode,
    const NEXUS_CallbackDesc *notifyDisplay
    );

/* Disconnect video */
NEXUS_Error NEXUS_HdmiOutput_Disconnect_priv(
    NEXUS_HdmiOutputHandle handle
    );

/* Audio can only start if video is connected */
bool NEXUS_HdmiOutput_VideoIsConnected_priv(
    NEXUS_HdmiOutputHandle handle
    );

/* Sanity checks for the audio module */
bool NEXUS_HdmiOutput_AudioFormatSupported_priv(
    NEXUS_HdmiOutputHandle handle,
    BAVC_AudioFormat format,
    unsigned numChannels    /* PCM only */
    );

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
bool NEXUS_HdmiOutput_VideoFormatSupported_priv(
    NEXUS_HdmiOutputHandle handle,
    unsigned VideoFormat
    );
#endif

/* Set audio format information */
NEXUS_Error NEXUS_HdmiOutput_SetAudioParams_priv(
    NEXUS_HdmiOutputHandle handle,
    BAVC_AudioBits bitsPerSample,
    BAVC_AudioSamplingRate sampleRate,
    BAVC_AudioFormat format,
    unsigned numChannels    /* PCM only */
    );

/**
Summary:
Set the notifyAudio event
**/
void NEXUS_HdmiOutput_SetNotifyAudioEvent_priv(
    NEXUS_HdmiOutputHandle handle,
    BKNI_EventHandle notifyAudioEvent
    );

    /* called before Display NEXUS_VideoFormat change */
NEXUS_Error NEXUS_HdmiOutput_P_PreFormatChange_priv(
    NEXUS_HdmiOutputHandle hdmiOutput, 
    bool aspectRatioChangeOnly
    );

/* called after Display NEXUS_VideoFormat change */
NEXUS_Error NEXUS_HdmiOutput_P_PostFormatChange_priv(
    NEXUS_HdmiOutputHandle hdmiOutput
    );


#if BAVC_HDMI_1_3_SUPPORT

#define NEXUS_HAS_HDMI_1_3 1

/**
Summary:
Set the color depth for deep color feature (10 or 12 bits).

Description:
Uses to set specific deep color mode. This feature only available on platforms
with HDMI 1.3 features.
**/
NEXUS_Error NEXUS_HdmiOutput_SetColorDepth_priv(
    NEXUS_HdmiOutputHandle handle,
    NEXUS_HdmiColorDepth colorDepth
    );
#endif

void NEXUS_HdmiOutput_P_Vsync_isr(
    void *handle /* NEXUS_HdmiOutputHandle */
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_HDMI_OUTPUT_PRIV_H__ */

