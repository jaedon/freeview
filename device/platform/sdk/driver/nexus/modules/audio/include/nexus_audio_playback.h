/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_playback.h $
* $brcm_Revision: 9 $
* $brcm_Date: 5/17/12 3:11p $
*
* API Description:
*   API name: AudioPlayback
*    Specific APIs related to PCM audio playback.  This supports playback
*    of data from memory.  It can be routed either to a mixer or directly
*    to output devices.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/include/nexus_audio_playback.h $
* 
* 9   5/17/12 3:11p jgarrett
* SW7429-178: Adding programmable heaps
* 
* 8   4/22/11 10:43a erickson
* SW7420-1148: remove attr{shutdown=NEXUS_AudioInput_Shutdown}
* 
* 7   12/14/10 3:18p erickson
* SW7420-1285: add null_allowed attribute
* 
* 6   10/4/10 4:09p jgarrett
* SW7400-2848: Merge to main branch
* 
* SW7400-2848/2   10/4/10 3:18p mward
* SW7400-2848: SW7420-934: rename NEXUS_AudioPlayback_ReadComplete to
*  NEXUS_AudioPlayback_WriteComplete
* 
* 5   8/10/10 3:10p erickson
* SW7420-934: rename NEXUS_AudioPlayback_ReadComplete to
*  NEXUS_AudioPlayback_WriteComplete
*
* SW7400-2848/1   7/22/10 4:18p jgarrett
* SW7400-2848: Adding option for runtime sample rate adjustment and loop
*  around
* 
* 4   8/31/09 3:23p jgarrett
* SW7405-2706: Merging playback flush to main branch
*
* PR56656/1   8/19/09 4:53p jgarrett
* PR 56656: Adding playback flush
*
* 3   5/14/09 9:46a jgarrett
* PR 53780: Adding startThreshold
*
* PR53780/1   4/16/09 4:46p jgarrett
* PR 53780: Adding startThreshold
*
* 2   6/19/08 3:17p jgarrett
* PR 42243: Moving input volume control to specific inputs and out of
*  mixer
*
* 1   3/11/08 4:38p jgarrett
* pr 39983: Adding audio playback support
*
***************************************************************************/
#ifndef NEXUS_AUDIO_PLAYBACK_H__
#define NEXUS_AUDIO_PLAYBACK_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"
#include "nexus_timebase.h"

/*=************************************
Interface: AudioPlayback

Header file: nexus_audio_playback.h

Module: Audio

Description: Playback audio PCM data

**************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Handle for audio playback
***************************************************************************/
typedef struct NEXUS_AudioPlayback *NEXUS_AudioPlaybackHandle;

/***************************************************************************
Summary:
Audio Playback Open Settings

Description:
Audio Playback Open Settings

See Also:
NEXUS_AudioPlaybackSettings
***************************************************************************/
typedef struct NEXUS_AudioPlaybackOpenSettings
{
    size_t fifoSize;        /* FIFO size in bytes */
    size_t threshold;       /* FIFO data callback threshold in bytes.  When the amount of data
                               remaining in the FIFO drops below this level, the dataCallback
                               routine provided in NEXUS_AudioPlaybackSettings will be called.
                               If this value is 0, a default threshold percentage will be used. */
    NEXUS_HeapHandle heap;  /* Optional heap for fifo allocation */                               
} NEXUS_AudioPlaybackOpenSettings;

/***************************************************************************
Summary:
Get default settings for an audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetDefaultOpenSettings(
    NEXUS_AudioPlaybackOpenSettings *pSettings      /* [out] default settings */
    );

/***************************************************************************
Summary:
Open an audio playback channel
***************************************************************************/
NEXUS_AudioPlaybackHandle NEXUS_AudioPlayback_Open(     /* attr{destructor=NEXUS_AudioPlayback_Close}  */
    unsigned index,
    const NEXUS_AudioPlaybackOpenSettings *pSettings    /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close an audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_Close(
    NEXUS_AudioPlaybackHandle handle
    );

/***************************************************************************
Summary:
Settings of an audio playback channel
***************************************************************************/
typedef struct NEXUS_AudioPlaybackStartSettings
{
    NEXUS_Timebase timebase;
    unsigned sampleRate;                /* In Hz.  Pass 0 to use the value in NEXUS_AudioPlaybackSettings.sampleRate instead. */
    unsigned bitsPerSample;             /* Currently supports 8 and 16. */
    size_t startThreshold;              /* Starting threshold in bytes.  If set, the hardware will wait until the number of bytes
                                           specified has been buffered before starting to consume the data. */
    bool stereo;                        /* If true, data will be treated as stereo data.  If false, data will be treated as mono. */
    bool signedData;                    /* If true, data will be treated as signed.  If false, data will be treated as unsigned. */
    bool loopAround;                    /* If true, the buffer will loop around when empty instead of outputting zero samples. */
    NEXUS_CallbackDesc dataCallback;    /* Callback when space becomes available. User should call NEXUS_AudioPlayback_GetBuffer.
                                           You will not receive another callback until NEXUS_AudioPlayback_GetBuffer is called. */
} NEXUS_AudioPlaybackStartSettings;

/***************************************************************************
Summary:
Get default settings for an audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetDefaultStartSettings(
    NEXUS_AudioPlaybackStartSettings *pSettings  /* [out] Default Settings */
    );

/***************************************************************************
Summary:
Start playing data data from an audio playback channel
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_Start(
    NEXUS_AudioPlaybackHandle handle,
    const NEXUS_AudioPlaybackStartSettings *pSettings
    );

/***************************************************************************
Summary:
Stop playing data from an audio playback channel.

Description:
This will stop the channel and flush all data from the FIFO.
***************************************************************************/
void NEXUS_AudioPlayback_Stop(
    NEXUS_AudioPlaybackHandle handle
    );

/***************************************************************************
Summary:
Flush all data from the playback FIFO.

Description:
This will flush all data from the FIFO without stopping the channel.
***************************************************************************/
void NEXUS_AudioPlayback_Flush(
    NEXUS_AudioPlaybackHandle handle
    );

/***************************************************************************
Summary:
Get a pointer and size for the next location in the buffer which can accept data

Description:
NEXUS_AudioPlayback_GetBuffer is non-destructive. You can safely call it multiple times.
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_GetBuffer(
    NEXUS_AudioPlaybackHandle handle,
    void **pBuffer, /* [out] attr{memory=cached} pointer to memory mapped region that is ready for playback data */
    size_t *pSize   /* [out] total number of writeable, contiguous bytes which buffer is pointing to */
    );

/***************************************************************************
Summary:
Notify AudioPlayback how much data was added into the buffer.

Description:
You can only call NEXUS_AudioPlayback_WriteComplete once after a
NEXUS_AudioPlayback_GetBuffer call.  After calling it, you must call
NEXUS_AudioPlayback_GetBuffer before adding more data.
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_WriteComplete(
    NEXUS_AudioPlaybackHandle handle,
    size_t amountWritten            /* The number of bytes written to the buffer */
    );

/* backward compatibility */
#define NEXUS_AudioPlayback_ReadComplete NEXUS_AudioPlayback_WriteComplete

/***************************************************************************
Summary:
Audio playback status
***************************************************************************/
typedef struct NEXUS_AudioPlaybackStatus
{
    NEXUS_AudioPlaybackStartSettings startSettings;
    bool started;
    size_t queuedBytes;
    size_t fifoSize;
    size_t playedBytes;
} NEXUS_AudioPlaybackStatus;

/***************************************************************************
Summary:
Get current status of the audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetStatus(
    NEXUS_AudioPlaybackHandle handle,
    NEXUS_AudioPlaybackStatus *pStatus      /* [out] Current Status */
    );

/***************************************************************************
Summary:
Audio playback settings
***************************************************************************/
typedef struct NEXUS_AudioPlaybackSettings
{
    int32_t leftVolume;         /* Linear volume level of left channel output */
    int32_t rightVolume;        /* Linear volume level of right channel output */
    bool muted;
    unsigned sampleRate;        /* In Hz.  This value is ignored unless
                                   NEXUS_AudioPlaybackStartSettings.sampleRate is
                                   set to 0. */
} NEXUS_AudioPlaybackSettings;

/***************************************************************************
Summary:
Get current setting of the audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetSettings(
    NEXUS_AudioPlaybackHandle handle,
    NEXUS_AudioPlaybackSettings *pSettings  /* [out] Current settings */
    );

/***************************************************************************
Summary:
Set current setting of the audio playback channel
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_SetSettings(
    NEXUS_AudioPlaybackHandle handle,
    const NEXUS_AudioPlaybackSettings *pSettings
    );

/***************************************************************************
Summary:
Get an audio connector for use with downstream components.
**************************************************************************/
NEXUS_AudioInput NEXUS_AudioPlayback_GetConnector(
    NEXUS_AudioPlaybackHandle handle
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_PLAYBACK_H__ */
