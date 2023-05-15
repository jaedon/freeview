/***************************************************************************
*     (c)2004-2013 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_mixer.c $
* $brcm_Revision: SW7346-1228-1/1 $
* $brcm_Date: 3/29/13 12:38p $
*
* API Description:
*   API name: AudioMixer
*    APIs for an audio mixer.  Allows multiple inputs to be connected to outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_audio_mixer.c $
* 
* SW7346-1228-1/1   3/29/13 12:38p scho
* SW7346-1228-1: update NEXUS Audio Input to check if the mixer is
*  already started
* 
* 11   10/9/12 11:05p jgarrett
* SW7425-3831: Merge to main branch
* 
* SW7425-3831/1   9/14/12 6:12p jgarrett
* SW7425-3831: Adding programmable mixer output rate
* 
* 10   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 9   6/15/12 3:26p jgarrett
* SW7425-3252: Adding DSP mixer balance per dolby specification
* 
* 8   3/13/12 6:18p jgarrett
* SW7435-24: Adding programmable DSP index
* 
* 7   10/10/11 10:18a jgarrett
* SW7425-1119: Merge to main branch
* 
* SW7425-1119/3   9/26/11 4:14p jgarrett
* SW7425-1119: Allowing compressed inputs
* 
* SW7425-1119/2   8/29/11 6:20p jgarrett
* SW7425-1119: Adding PrepareToStart in start routine
* 
* SW7425-1119/1   8/26/11 12:19p jgarrett
* SW7425-1119: Adding mixer start/stop routines
* 
* 6   9/15/11 3:24p jgarrett
* SWDTV-8673: Downgrading debug message
* 
* 5   6/28/11 10:33a jgarrett
* SW7422-406: Adding input volume settings for DSP mixers
* 
* 4   6/5/11 6:57p jgarrett
* SW7425-406: Initial FW mixer implemenation on APE
* 
* 3   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 2   4/26/11 11:35a jgarrett
* SW7425-437: Resolving kernel mode shutdown issues
* 
* 1   12/17/10 3:56p jgarrett
* SW7422-146: Adding initial nexus on APE for 7422
* 
***************************************************************************/
#include "nexus_audio_module.h"
#include "nexus_pid_channel.h"
#include "nexus_timebase.h"    /* Timebase */

BDBG_MODULE(nexus_audio_mixer);

typedef struct NEXUS_AudioMixer
{
    NEXUS_OBJECT(NEXUS_AudioMixer);
    bool opened;
    bool started;
    NEXUS_AudioMixerSettings settings;
    NEXUS_AudioInputObject connector;
    BAPE_MixerHandle dspMixer;
} NEXUS_AudioMixer;

#if NEXUS_NUM_AUDIO_MIXERS
static NEXUS_AudioMixer g_mixers[NEXUS_NUM_AUDIO_MIXERS];

/***************************************************************************
Summary:
    Get default settings for an audio mixer
See Also:
    NEXUS_AudioMixer_Open
 ***************************************************************************/
void NEXUS_AudioMixer_GetDefaultSettings(
    NEXUS_AudioMixerSettings *pSettings    /* [out] Default Settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(NEXUS_AudioMixerSettings));
}

/***************************************************************************
Summary:
    Open an audio mixer
See Also:
    NEXUS_AudioMixer_Close
 ***************************************************************************/
NEXUS_AudioMixerHandle NEXUS_AudioMixer_Open(
    const NEXUS_AudioMixerSettings *pSettings
    )
{
    NEXUS_AudioMixer *pMixer=NULL;
    int i;
    NEXUS_Error rc=0;
    NEXUS_AudioMixerSettings defaults;

    if ( NULL == pSettings )
    {
        NEXUS_AudioMixer_GetDefaultSettings(&defaults);
        pSettings = &defaults;
    }

    for ( i = 0; i < NEXUS_NUM_AUDIO_MIXERS; i++ )
    {
        pMixer = &g_mixers[i];
        if ( !pMixer->opened )
        {
            /* Clear structure */
            NEXUS_OBJECT_INIT(NEXUS_AudioMixer, pMixer);
            pMixer->opened = true;
            break;
        }
    }

    if ( i >= NEXUS_NUM_AUDIO_MIXERS )
    {
        BDBG_ERR(("Out of mixers"));
        rc=BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }

    pMixer->settings.mixUsingDsp = pSettings->mixUsingDsp;
    BDBG_MSG(("mixUsingDsp = %u",pMixer->settings.mixUsingDsp));

    /* Setup connector */
    if ( pSettings->mixUsingDsp )
    {
        BAPE_MixerSettings mixerSettings;
        BAPE_Connector connector;

        NEXUS_AUDIO_INPUT_INIT(&pMixer->connector, NEXUS_AudioInputType_eDspMixer, pMixer);
        BAPE_Mixer_GetDefaultSettings(&mixerSettings);
        mixerSettings.type = BAPE_MixerType_eDsp;
        mixerSettings.dspIndex = pSettings->dspIndex;
        mixerSettings.multiStreamBalance = pSettings->multiStreamBalance;
        mixerSettings.mixerSampleRate = pSettings->outputSampleRate;
        rc = BAPE_Mixer_Create(NEXUS_AUDIO_DEVICE_HANDLE, &mixerSettings, &pMixer->dspMixer);
        if ( rc )
        {
            rc=BERR_TRACE(rc);
            return NULL;
        }
        BAPE_Mixer_GetConnector(pMixer->dspMixer, &connector);
        pMixer->connector.port = (uint32_t)connector;
    }
    else
    {
        NEXUS_AUDIO_INPUT_INIT(&pMixer->connector, NEXUS_AudioInputType_eMixer, pMixer);
    }
    pMixer->connector.format = NEXUS_AudioInputFormat_eNone; /* Determined by inputs */

    /* Set Settings */
    NEXUS_AudioMixer_SetSettings(pMixer, pSettings);        /* Handles NULL for defaults */

    /* Success */
    return pMixer;
}

/***************************************************************************
Summary:
    Close an audio mixer
See Also:
    NEXUS_AudioMixer_Open
 ***************************************************************************/
static void NEXUS_AudioMixer_P_Finalizer(
    NEXUS_AudioMixerHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_AudioMixer, handle);

    NEXUS_AudioInput_Shutdown(&handle->connector);

    if ( handle->dspMixer )
    {
        BAPE_Mixer_Destroy(handle->dspMixer);
    }

    /* Done */
    BKNI_Memset(handle, 0, sizeof(NEXUS_AudioMixer));
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioMixer, NEXUS_AudioMixer_Close);

/***************************************************************************
Summary:
    Get current settings for an audio mixer
See Also:
    NEXUS_AudioMixer_SetSettings
 ***************************************************************************/
void NEXUS_AudioMixer_GetSettings(
    NEXUS_AudioMixerHandle handle,
    NEXUS_AudioMixerSettings *pSettings      /* [out] Mixer Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMixer);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = handle->settings;
}

/***************************************************************************
Summary:
    Set settings of an audio mixer
See Also:
    NEXUS_AudioMixer_SetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_SetSettings(
    NEXUS_AudioMixerHandle handle,
    const NEXUS_AudioMixerSettings *pSettings    /* [out] Mixer Settings */
    )
{
    NEXUS_AudioInput oldMaster, newMaster;
    int oldBalance, newBalance;
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMixer);
    BDBG_ASSERT(NULL != pSettings);

    if ( handle->settings.mixUsingDsp != pSettings->mixUsingDsp )
    {
        BDBG_ERR(("mixUsingDsp can only be set when a mixer is opened.  It cannot be changed on-the-fly."));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    oldMaster = handle->settings.master;
    newMaster = pSettings->master;
    oldBalance = handle->settings.multiStreamBalance;
    newBalance = pSettings->multiStreamBalance;

    handle->settings = *pSettings;

    if ( oldMaster != newMaster )
    {
        if ( NULL != newMaster )
        {
            errCode = NEXUS_AudioMixer_RemoveInput(handle, newMaster);
            if ( errCode )
            {
                BDBG_ERR(("Master input %#x is not connected to mixer %#x", pSettings->master, handle));
                handle->settings.master = NULL;
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
            errCode = NEXUS_AudioMixer_AddInput(handle, newMaster);
            if ( errCode )
            {
                handle->settings.master = NULL;
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
        }
    }

    if ( oldBalance != newBalance && pSettings->mixUsingDsp )
    {
        BAPE_MixerSettings mixerSettings;
        BAPE_Mixer_GetSettings(handle->dspMixer, &mixerSettings);
        mixerSettings.multiStreamBalance = newBalance;
        errCode = BAPE_Mixer_SetSettings(handle->dspMixer, &mixerSettings);
        if ( errCode )
        {
            handle->settings.multiStreamBalance = oldBalance;
            return BERR_TRACE(errCode);
        }
    }

    /* Success */
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_AudioMixer_Start(
    NEXUS_AudioMixerHandle handle
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMixer);

    if ( handle->started )
    {
        BDBG_ERR(("Already started."));
    }
    
    if ( handle->settings.mixUsingDsp )
    {
        errCode = NEXUS_AudioInput_P_PrepareToStart(&handle->connector);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        errCode = BAPE_Mixer_Start(handle->dspMixer);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    
        handle->started = true;    
        return BERR_SUCCESS;
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }        
}

void NEXUS_AudioMixer_Stop(
    NEXUS_AudioMixerHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMixer);
    
    if ( false == handle->started )
    {
        return;
    }
    
    if ( handle->settings.mixUsingDsp )
    {
        BAPE_Mixer_Stop(handle->dspMixer);
        handle->started = false;
    }
    else
    {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }            
}
    
/***************************************************************************
Summary:
    Add an audio input to a mixer
See Also:
    NEXUS_AudioMixer_RemoveInput
 ***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_AddInput(
    NEXUS_AudioMixerHandle handle,
    NEXUS_AudioInput input
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMixer);
    BDBG_ASSERT(NULL != input);

    /* Check if new input is already running */
    if ( NEXUS_AudioInput_P_IsRunning(input) )
    {
        BDBG_ERR(("Input %p is running.  Please stop first.", input));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Check if I'm running */
    if ( NEXUS_AudioInput_P_IsRunning(&handle->connector) || handle->started )
    {
        BDBG_ERR(("Mixer %p is running.  Stop all inputs first.", handle));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( handle->settings.mixUsingDsp )
    {
        BAPE_MixerAddInputSettings addInputSettings;
        BAPE_MixerInputVolume inputVolume;
        bool master = (input == handle->settings.master)?true:false;

        BAPE_Mixer_GetDefaultAddInputSettings(&addInputSettings);
        addInputSettings.sampleRateMaster = master;
        errCode = BAPE_Mixer_AddInput(handle->dspMixer, (BAPE_Connector)input->port, &addInputSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        /* Match input volume in mixer */
        NEXUS_AudioInput_P_GetVolume(input, &inputVolume);
        errCode = BAPE_Mixer_SetInputVolume(handle->dspMixer, (BAPE_Connector)input->port, &inputVolume);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    /* Connect at input node */
    errCode = NEXUS_AudioInput_P_AddInput(&handle->connector, input);
    if ( errCode )
    {
        if ( handle->settings.mixUsingDsp )
        {
            BAPE_Mixer_RemoveInput(handle->dspMixer, (BAPE_Connector)input->port);
        }
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Remove all audio inputs from a mixer
See Also:
    NEXUS_AudioMixer_RemoveInput
 ***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_RemoveAllInputs(
    NEXUS_AudioMixerHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMixer);

    /* Check if I'm running */
    if ( NEXUS_AudioInput_P_IsRunning(&handle->connector) || handle->started )
    {
        BDBG_ERR(("Mixer %p is running.  Stop all inputs first.", handle));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Connect at input node */
    return NEXUS_AudioInput_P_RemoveAllInputs(&handle->connector);
}

/***************************************************************************
Summary:
    Remove an audio input from a mixer
See Also:
    NEXUS_AudioMixer_AddInput
 ***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_RemoveInput(
    NEXUS_AudioMixerHandle handle,
    NEXUS_AudioInput input
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMixer);
    BDBG_ASSERT(NULL != input);

    /* Check if I'm running */
    if ( NEXUS_AudioInput_P_IsRunning(&handle->connector) || handle->started )
    {
        BDBG_ERR(("Mixer %p is running.  Stop all inputs first.", handle));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Make sure this input is actually connected to this mixer */
    if ( !NEXUS_AudioInput_P_IsConnected(&handle->connector, input) )
    {
        BDBG_ERR(("Input %p is not connected to mixer %p", input, handle));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( handle->settings.mixUsingDsp )
    {
        (void)BAPE_Mixer_RemoveInput(handle->dspMixer, (BAPE_Connector)input->port);
    }

    /* Remove Input */
    return NEXUS_AudioInput_P_RemoveInput(&handle->connector, input);
}

/***************************************************************************
Summary:
Get the audio input connector for connection to outputs or post-processing
***************************************************************************/
NEXUS_AudioInput NEXUS_AudioMixer_GetConnector(
    NEXUS_AudioMixerHandle mixer
    )
{
    BDBG_OBJECT_ASSERT(mixer, NEXUS_AudioMixer);
    return &mixer->connector;
}

/***************************************************************************
Summary: 
    Propagate mixer input volume into a mixer object in nexus 
 ***************************************************************************/
NEXUS_Error NEXUS_AudioMixer_P_SetInputVolume(
    NEXUS_AudioMixerHandle handle, 
    NEXUS_AudioInput input,
    const BAPE_MixerInputVolume *pInputVolume
    )
{
    if ( handle->dspMixer )
    {
        BERR_Code errCode;

        errCode = BAPE_Mixer_SetInputVolume(handle->dspMixer, (BAPE_Connector)input->port, pInputVolume);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary: 
    Check mixer state - mixer can be started even if inputs are not started 
 ***************************************************************************/
bool NEXUS_AudioMixer_P_IsStarted(
    NEXUS_AudioMixerHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMixer);
    return handle->started;
}

#endif /* #if NEXUS_NUM_AUDIO_MIXERS */

