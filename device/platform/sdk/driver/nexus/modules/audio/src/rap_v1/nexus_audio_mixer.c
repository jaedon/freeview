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
* $brcm_Workfile: nexus_audio_mixer.c $
* $brcm_Revision: 9 $
* $brcm_Date: 7/30/12 4:35p $
*
* API Description:
*   API name: AudioMixer
*    APIs for an audio mixer.  Allows inputs to be connected to outputs
*    with the capability to mix inputs and control volume/equalization.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/rap_v1/nexus_audio_mixer.c $
* 
* 9   7/30/12 4:35p jgarrett
* SW7435-266: Switching to unified audio module structure
* 
* 8   6/20/12 1:15p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 7   5/26/10 11:48a mward
* SW7405-4212: Stub support for DSP mixing on 7400.
* 
* 6   11/10/08 11:29a mward
* PR47739: Defer application of settings until the output is started
*  (opened).  Other updates from 7405 implementation.
* 
* 5   10/31/08 10:21a mward
* PR47739: Merge BDBG_OBJECTs and other updates from 7405 implementation.
* 
* 4   6/20/08 2:48p jgarrett
* PR 42243: Moving input volume to specific inputs and out of mixer
* 
* 3   3/13/08 6:35p erickson
* PR39017: add NEXUS_AudioMixer_GetConnector
* 
* 2   2/5/08 4:43p jgarrett
* PR 39017: Refactoring audio output list
* 
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/11   1/8/08 7:37p jgarrett
* PR 38535: Updating to new 3563 APIs
* 
* Nexus_Devel/10   11/19/07 1:20p erickson
* PR34925: split demux.h into separate interface files
* 
* Nexus_Devel/9   11/12/07 12:17p jgarrett
* PR 36774: Renaming start to connect
* 
* Nexus_Devel/8   11/9/07 10:03a jgarrett
* PR 36774: backing out change
* 
* Nexus_Devel/6   11/6/07 4:22p jgarrett
* PR 34954: Migrating to audio connector model
* 
* Nexus_Devel/5   10/11/07 4:16p jgarrett
* PR 34954: Fixing assertion when removing inputs
* 
* Nexus_Devel/4   10/10/07 2:35p erickson
* PR34954: fix misspelling
* 
* Nexus_Devel/3   9/28/07 5:14p jgarrett
* PR 34594: Adding sync thunks
* 
* Nexus_Devel/2   9/27/07 9:19p jgarrett
* PR 34954: Adding debug
* 
* Nexus_Devel/1   9/27/07 7:52p jgarrett
* PR 34954: Adding baseline support
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
    NEXUS_AudioMixerSettings settings;
    NEXUS_AudioInputObject connector;
} NEXUS_AudioMixer;

#if NEXUS_NUM_AUDIO_MIXERS
static NEXUS_AudioMixer g_mixers[NEXUS_NUM_AUDIO_MIXERS];

static NEXUS_Error NEXUS_AudioMixer_P_InputStarted(void *pHandle, struct NEXUS_AudioInputObject *pInput);
static NEXUS_Error NEXUS_AudioMixer_P_ConnectionChange(void *pHandle, struct NEXUS_AudioInputObject *pInput);

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

    /* Setup connector */
    pMixer->connector.objectType = NEXUS_AudioInputType_eMixer;
    pMixer->connector.pObjectHandle = pMixer;
    pMixer->connector.format = NEXUS_AudioInputFormat_eNone; /* Determined by inputs */
    pMixer->connector.connectCb = NEXUS_AudioMixer_P_InputStarted;
    pMixer->connector.disconnectCb = NEXUS_AudioMixer_P_ConnectionChange;

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

    if ( NULL != handle->connector.pMixerData )
    {
        BDBG_WRN(("Connector has not been shutdown.  Please call NEXUS_AudioInput_Shutdown first."));
        BDBG_ASSERT(NULL == handle->connector.pMixerData);
        return;
    }


    /* Done */
    NEXUS_OBJECT_CLEAR(NEXUS_AudioMixer, handle);
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
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMixer);

    if ( NULL == pSettings )
    {
        NEXUS_AudioMixer_GetDefaultSettings(&handle->settings);
    }
    else
    {
        if ( pSettings->mixUsingDsp )
        {
            BDBG_ERR(("DSP mixing not supported on this platform."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        handle->settings = *pSettings;
    }

    /* Mixer settings are only applied at start-time.  Nothing more to do now, it will be handled at next start */

    /* Success */
    return BERR_SUCCESS;
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
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMixer);
    BDBG_ASSERT(NULL != input);

    /* Check if new input is already running */
    if ( NEXUS_AudioInput_P_IsRunning(input) )
    {
        BDBG_ERR(("Input %p is running.  Please stop first.", input));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Check if I'm running */
    if ( NEXUS_AudioInput_P_IsRunning(&handle->connector) )
    {
        BDBG_ERR(("Mixer %p is running.  Stop all inputs first.", handle));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Check format */
    if ( NEXUS_AudioInputFormat_eCompressed == NEXUS_AudioInput_P_GetFormat(input) )
    {
        BDBG_ERR(("Can not attach a compressed source to a mixer"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Connect at input node */
    return NEXUS_AudioInput_P_AddInput(&handle->connector, input);
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
    if ( NEXUS_AudioInput_P_IsRunning(&handle->connector) )
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
    if ( NEXUS_AudioInput_P_IsRunning(&handle->connector) )
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

    /* Remove Input */
    return NEXUS_AudioInput_P_RemoveInput(&handle->connector, input);
}

static NEXUS_Error NEXUS_AudioMixer_P_InputStarted(void *pHandle, struct NEXUS_AudioInputObject *pInput)
{
    NEXUS_Error errCode;
    NEXUS_AudioMixerHandle mixer = (NEXUS_AudioMixerHandle) pHandle;

    BDBG_OBJECT_ASSERT(mixer, NEXUS_AudioMixer);
    BDBG_ASSERT(NULL != pInput);

    BDBG_MSG(("Input %p Started", pInput));


    /* If this is the first input to start... */
    if ( !NEXUS_AudioInput_P_IsRunning(&mixer->connector) )
    {
        /* Must add outputs */
        errCode = NEXUS_AudioInput_P_PrepareToStart(&mixer->connector);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

static NEXUS_Error NEXUS_AudioMixer_P_ConnectionChange(void *pHandle, struct NEXUS_AudioInputObject *pInput)
{
    NEXUS_AudioMixerHandle mixer = (NEXUS_AudioMixerHandle) pHandle;

    BDBG_OBJECT_ASSERT(mixer, NEXUS_AudioMixer);
    BSTD_UNUSED(pInput);

    BDBG_MSG(("Mixer %p connection change", mixer));

    return BERR_SUCCESS;
}
NEXUS_AudioInput NEXUS_AudioMixer_GetConnector(
    NEXUS_AudioMixerHandle mixer
    )
{
    BDBG_OBJECT_ASSERT(mixer, NEXUS_AudioMixer);
    return &mixer->connector;
}

NEXUS_Error NEXUS_AudioMixer_Start(
    NEXUS_AudioMixerHandle handle
    )
{
    BSTD_UNUSED(handle);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

void NEXUS_AudioMixer_Stop(
    NEXUS_AudioMixerHandle handle
    )
{
    BSTD_UNUSED(handle);
}

#endif /* #if NEXUS_NUM_AUDIO_MIXERS */

