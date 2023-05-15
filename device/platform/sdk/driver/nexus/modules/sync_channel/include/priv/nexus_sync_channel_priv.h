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
 * $brcm_Workfile: nexus_sync_channel_priv.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 7/30/12 5:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/sync_channel/include/priv/nexus_sync_channel_priv.h $
 * 
 * 2   7/30/12 5:16p bandrews
 * SW7425-3422: fix newline at end of file
 * 
 * 1   7/3/12 4:45p bandrews
 * SW7425-3422: added status to sync channel
 **************************************************************************/
#ifndef NEXUS_SYNC_CHANNEL_PRIV_H__
#define NEXUS_SYNC_CHANNEL_PRIV_H__

#include "nexus_sync_channel.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
Summary:
Supported number of video outputs for sync channel.  This can differ from num inputs
defined in nexus_platform_features.h.
**/
#define NEXUS_SYNC_CHANNEL_NUM_AUDIO_OUTPUTS 4

/**
Summary:
Supported number of video outputs for sync channel.  This can differ from num inputs
defined in nexus_platform_features.h.
**/
#define NEXUS_SYNC_CHANNEL_NUM_VIDEO_OUTPUTS 2

/**
Summary:
Sync channel state
**/
typedef enum
{
    NEXUS_SyncChannelState_eAcquiring,
    NEXUS_SyncChannelState_eLocked,
    NEXUS_SyncChannelState_eMax
} NEXUS_SyncChannelState;

/**
Summary:
Sync channel status
**/
typedef struct NEXUS_SyncChannelStatus
{
    NEXUS_SyncChannelState displaySync;
    NEXUS_SyncChannelState audioOutputSync;
    NEXUS_SyncChannelState avSync;
    
    bool stcTrickMode;
    bool nonRealTime;

    struct
    {
        struct
        {
            bool muted;          /* Mute video until SyncChannel is ready. */
        	bool started; /* has this delay path element been started? */
        	bool digital; /* is the video source digital or analog */
        	bool synchronized; /* should this element be synchronized? */
        	struct
        	{
        		int applied;
                int phase;
        		int measured; /* measured delay, defaults to 45 Khz ticks */
        		int custom; /* delay supplied by user that isn't in the normal path delay, defaults to ms */
                int notificationThreshold; /* threshold (in 45 KHz units) to limit frequency of callback */
        	} delay;
        	struct
        	{
        		unsigned height; /* height of format, required to predict VDC MAD state changes */
        		bool interlaced; /* whether the format is interlaced */
        		unsigned frameRate;
        	} format;
        	bool lastPictureHeld; /* is the decoder holding the last picture or blanking? */
        } source;
        struct
        {
        	bool synchronized; /* should this element be synchronized? */
        	struct
        	{
                int applied;
        		int measured; /* measured delay, defaults to vsyncs */
        		int phase; /* sub-vsync phase */
        		int custom; /* delay supplied by user that isn't in the normal path delay in ms */
                int notificationThreshold;
        	} delay;
        	struct
        	{
        		unsigned height; /* height of format, required to predict VDC MAD state changes */
        		bool interlaced; /* whether the format is interlaced */
        		unsigned refreshRate;
        	} format;
        	bool forcedCapture; /* is forced capture enabled on this window */
        	bool masterFrameRate; /* is master frame rate enabled on the main window for this display */
        	bool fullScreen; /* does window rect match display rect? */
        	bool visible; /* is this window visible? */
        	bool aligned;
        } sinks[NEXUS_SYNC_CHANNEL_NUM_VIDEO_OUTPUTS];
        int syncLockedWindow;
    } video;
    struct
    {
        struct
        {
            bool muted;          /* Mute audio until SyncChannel is ready. */
        	bool started; /* has this delay path element been started? */
        	bool digital; /* is the audio source digital or analog */
        	bool synchronized; /* should this element be synchronized? */
        	struct
        	{
        	    int applied;
        		int measured; /* measured delay, defaults to ms */
        		int custom; /* delay supplied by user that isn't in the normal path delay in ms */
        	} delay;
        	bool samplingRateReceived; /* has the audio sampling rate callback been received since audio start? (self-clearing) */
        } sources[NEXUS_SYNC_CHANNEL_NUM_AUDIO_INPUTS];
        struct
        {
        	bool synchronized; /* should this element be synchronized? */
        	bool compressed; /* is the audio sink compressed */
        	struct
        	{
        		int applied;
        		int measured; /* measured delay, defaults to ms */
        		int custom; /* delay supplied by user that isn't in the normal path delay in ms */
        	} delay;
        	unsigned samplingRate; /* the audio sampling rate in Hz (for analog) */
        } sinks[NEXUS_SYNC_CHANNEL_NUM_AUDIO_OUTPUTS];
    } audio;
} NEXUS_SyncChannelStatus;

/**
Summary:
Get current settings.
**/
NEXUS_Error NEXUS_SyncChannel_GetStatus(
    NEXUS_SyncChannelHandle    handle,
    NEXUS_SyncChannelStatus *pStatus /* [out] */
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_SYNC_CHANNEL_PRIV_H__ */

