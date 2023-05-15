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
* $brcm_Workfile: nexus_audio_mixer.h $
* $brcm_Revision: 14 $
* $brcm_Date: 10/9/12 11:05p $
*
* API Description:
*   API name: AudioMixer
*    APIs for an audio mixer.  Allows one or more inputs to be
*    connected, and provides input volume control for mixing.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/include/nexus_audio_mixer.h $
* 
* 14   10/9/12 11:05p jgarrett
* SW7425-3831: Merge to main branch
* 
* SW7425-3831/1   9/14/12 6:12p jgarrett
* SW7425-3831: Adding programmable mixer output rate
* 
* 13   7/30/12 4:35p jgarrett
* SW7435-266: Switching to unified audio module structure
* 
* 3   6/15/12 3:26p jgarrett
* SW7425-3252: Adding DSP mixer balance per dolby specification
* 
* 2   3/13/12 6:18p jgarrett
* SW7435-24: Adding programmable DSP index
* 
* 1   10/10/11 10:18a jgarrett
* SW7425-1119: Merge to main branch
* 
* SW7425-1119/1   8/26/11 12:19p jgarrett
* SW7425-1119: Adding mixer start/stop routines
* 
***************************************************************************/
#ifndef NEXUS_AUDIO_MIXER_H__
#define NEXUS_AUDIO_MIXER_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************
Interface: AudioMixer

Header file: nexus_audio_mixer.h

Module: Audio

Description: Mixer two or more PCM NEXUS_AudioInputs into a single NEXUS_AudioOutput.

**************************************/

/**
Summary:
Handle for an Audio Mixer
**/
typedef struct NEXUS_AudioMixer *NEXUS_AudioMixerHandle;

/***************************************************************************
Summary:
Audio Mixer Settings
***************************************************************************/
typedef struct NEXUS_AudioMixerSettings
{
    bool mixUsingDsp;           /* If true, mix contents in the DSP as opposed to the mixer hardware.  
                                   This requires the masterInput field to be set below prior to starting
                                   any input.  This field should be set prior to NEXUS_AudioMixer_Open
                                   and not changed on the fly. */

    unsigned dspIndex;          /* If mixUsingDsp is true, this selects the DSP to use.  When mixing decoder
                                   inputs from the DSP, all inputs must run on the same DSP as the mixer.  
                                   This is not changeable on the fly. */

    NEXUS_AudioInput master;    /* This field will determine what input is deemed to be the master for mixing 
                                   purposes.  The master will determine the output timing, and if mixing multi-stream
                                   content such as audio descriptors it will determine the primary trach as opposed to
                                   the description track.  */

    int multiStreamBalance;     /* If this is a DSP mixer and you are mixing multi-stream content, this
                                   value controls the relative volume between the main and associated
                                   audio content.  0 means no preference.  Positive values up to 32 
                                   favor associated audio in dB steps.  32 will mute main audio.  Negative
                                   values down to -32 favor main audio in dB steps.  -32 will mute the
                                   associated audio. */

    unsigned outputSampleRate;  /* Output sample rate in Hz.  If 0 (default) the output rate will
                                   be automatically selected based on the input sample rate.  This will
                                   override the sample rate of a master input if specified in
                                   NEXUS_AudioMixerSettings.master */
} NEXUS_AudioMixerSettings;

/***************************************************************************
Summary:
Get default settings for an audio mixer
***************************************************************************/
void NEXUS_AudioMixer_GetDefaultSettings(
    NEXUS_AudioMixerSettings *pSettings    /* [out] Default Settings */
    );

/***************************************************************************
Summary:
Open an audio mixer
***************************************************************************/
NEXUS_AudioMixerHandle NEXUS_AudioMixer_Open( /* attr{destructor=NEXUS_AudioMixer_Close}  */
    const NEXUS_AudioMixerSettings *pSettings /* attr{null_allowed=y} */
    );

/***************************************************************************
Summary:
Close an audio mixer
***************************************************************************/
void NEXUS_AudioMixer_Close(
    NEXUS_AudioMixerHandle handle
    );

/***************************************************************************
Summary:
Get current settings for an audio mixer
***************************************************************************/
void NEXUS_AudioMixer_GetSettings(
    NEXUS_AudioMixerHandle handle,
    NEXUS_AudioMixerSettings *pSettings      /* [out] Mixer Settings */
    );

/***************************************************************************
Summary:
Start a mixer 

Description:
This call is optional.  By default, mixers will automatically start when
The first input starts, but if you want to explicitly enable the mixer
earlier call this function prior to starting any inputs.   Currently, this 
is only supported if mixUsingDsp is set to true.
***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_Start(
    NEXUS_AudioMixerHandle handle
    );

/***************************************************************************
Summary:
Stop a mixer

Description:
This call is required only if you call NEXUS_AudioMixer_Start().  By default, 
mixers will automatically stop when the last input stops, but if you
have explicitly started the mixer via NEXUS_AudioMixer_Start() you must call
this routine to stop it after all inputs have stopped.
***************************************************************************/
void NEXUS_AudioMixer_Stop(
    NEXUS_AudioMixerHandle handle
    );

/***************************************************************************
Summary:
Set settings of an audio mixer
***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_SetSettings(
    NEXUS_AudioMixerHandle handle,
    const NEXUS_AudioMixerSettings *pSettings    /* Mixer Settings */
    );

/***************************************************************************
Summary:
Add an audio input to a mixer
***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_AddInput(
    NEXUS_AudioMixerHandle handle,
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Remove an audio input from a mixer
***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_RemoveInput(
    NEXUS_AudioMixerHandle handle,
    NEXUS_AudioInput input
    );

/***************************************************************************
Summary:
Remove all audio inputs from a mixer
***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_RemoveAllInputs(
    NEXUS_AudioMixerHandle handle
    );

/***************************************************************************
Summary:
Get the audio input connector for connection to outputs or post-processing
***************************************************************************/
NEXUS_AudioInput NEXUS_AudioMixer_GetConnector(
    NEXUS_AudioMixerHandle mixer
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_MIXER_H__ */

