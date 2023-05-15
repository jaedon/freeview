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
* $brcm_Workfile: nexus_audio_dac.c $
* $brcm_Revision: 8 $
* $brcm_Date: 6/19/12 12:41p $
*
* API Description:
*   API name: AudioDac
*    Specific APIs related to audio DAC outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7422/src/nexus_audio_dac.c $
* 
* 8   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 7   7/6/11 6:41p jgarrett
* SW7552-55: Mapping channelMode to stereoMode
* 
* 6   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 5   4/26/11 11:35a jgarrett
* SW7425-437: Resolving kernel mode shutdown issues
* 
* 4   3/25/11 11:55a jgarrett
* SW7422-375: Refactoring peak gain api
* 
* 3   3/23/11 7:12p jgarrett
* SW7422-375: Adding peakingFilter settings
* 
* 2   2/22/11 5:44p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
*  comments
* 
* 1   12/17/10 3:56p jgarrett
* SW7422-146: Adding initial nexus on APE for 7422
* 
***************************************************************************/
#include "nexus_audio_module.h"

BDBG_MODULE(nexus_audio_dac);

typedef struct NEXUS_AudioDac
{
    NEXUS_OBJECT(NEXUS_AudioDac);
    bool opened;
    BAPE_DacHandle handle;
    NEXUS_AudioDacSettings settings;
    NEXUS_AudioOutputObject connector;
} NEXUS_AudioDac;

static NEXUS_AudioDac g_dacs[NEXUS_NUM_AUDIO_DACS];

static NEXUS_Error NEXUS_AudioDac_P_SetChannelMode(void *pHandle, NEXUS_AudioChannelMode channelMode);

/***************************************************************************
Summary:
    Get default settings for an audio DAC
See Also:

 ***************************************************************************/
void NEXUS_AudioDac_GetDefaultSettings(
    NEXUS_AudioDacSettings *pSettings   /* [out] default settings */
    )
{
    BAPE_DacSettings dacSettings;

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    BAPE_Dac_GetDefaultSettings(&dacSettings);

    switch ( dacSettings.muteType )
    {
    default:
    case BAPE_DacMuteType_eDrive0:
        pSettings->muteType = NEXUS_AudioDacMuteType_eDrive0;
        break;
    case BAPE_DacMuteType_eDriveNegative1:
        pSettings->muteType = NEXUS_AudioDacMuteType_eDriveNegative1;
        break;
    case BAPE_DacMuteType_eCustom:
        pSettings->muteType = NEXUS_AudioDacMuteType_eCustomValue;
        break;
    }

    pSettings->muteValueLeft = dacSettings.customLeftValue;
    pSettings->muteValueRight = dacSettings.customRightValue;
    pSettings->testTone.sharedSamples = true;
    pSettings->testTone.numSamplesLeft = 64;
    pSettings->testTone.numSamplesRight = 64;
    pSettings->testTone.sampleRate = 48000;
    pSettings->peakGain = dacSettings.peakGain;
}

/***************************************************************************
Summary:
    Open an audio DAC
See Also:
    NEXUS_AudioDac_Close
 ***************************************************************************/
NEXUS_AudioDacHandle NEXUS_AudioDac_Open(
    unsigned index,
    const NEXUS_AudioDacSettings *pSettings     /* Pass NULL for default settings */
    )
{
    BAPE_DacHandle dacHandle;
    BERR_Code errCode;
    BAPE_OutputPort connector;

    /* Sanity check */
    if ( index >= NEXUS_NUM_AUDIO_DACS )
    {
        BDBG_ERR(("Dac Index %u out of range", index));
        return NULL;
    }

    if ( g_dacs[index].opened )
    {
        BDBG_ERR(("Dac %d already open", index));
        return NULL;
    }

    errCode = BAPE_Dac_Open(NEXUS_AUDIO_DEVICE_HANDLE, index, NULL, &dacHandle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        return NULL;
    }

    g_dacs[index].opened = true;
    /* Initialize connector */
    g_dacs[index].handle = dacHandle;
    NEXUS_AUDIO_OUTPUT_INIT(&g_dacs[index].connector, NEXUS_AudioOutputType_eDac, &g_dacs[index]);
    g_dacs[index].connector.setChannelMode = NEXUS_AudioDac_P_SetChannelMode;
    NEXUS_OBJECT_SET(NEXUS_AudioDac, &g_dacs[index]);
    NEXUS_AudioDac_SetSettings(&g_dacs[index], pSettings);

    BAPE_Dac_GetOutputPort(dacHandle, &connector);
    g_dacs[index].connector.port = (uint32_t)connector;

    /* Success */
    return &g_dacs[index];
}


/***************************************************************************
Summary:
    Close an audio DAC
Description:
    Input to the DAC must be removed prior to closing.
See Also:
    NEXUS_AudioDac_Open
    NEXUS_AudioOutput_RemoveInput
    NEXUS_AudioOutput_RemoveAllInputs
 ***************************************************************************/
static void NEXUS_AudioDac_P_Finalizer(
    NEXUS_AudioDacHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_AudioDac, handle);

    NEXUS_AudioOutput_Shutdown(&handle->connector);
    BAPE_Dac_Close(handle->handle);
    BKNI_Memset(handle, 0, sizeof(NEXUS_AudioDac));
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioDac, NEXUS_AudioDac_Close);

/***************************************************************************
Summary:
    Get Settings for an audio DAC
See Also:
    NEXUS_AudioDac_SetSettings
 ***************************************************************************/
void NEXUS_AudioDac_GetSettings(
    NEXUS_AudioDacHandle handle,
    NEXUS_AudioDacSettings *pSettings    /* [out] Settings */
    )
{
    /* Sanity Check */
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDac);
    BDBG_ASSERT(NULL != pSettings);

    /* Copy settings */
    *pSettings = handle->settings;
}

/***************************************************************************
Summary:
    Set Settings for an audio DAC
See Also:
    NEXUS_AudioDac_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDac_SetSettings(
    NEXUS_AudioDacHandle handle,
    const NEXUS_AudioDacSettings *pSettings    /* [in] Settings */
    )
{
    NEXUS_AudioDacSettings settings;
    BAPE_DacSettings dacSettings;
    NEXUS_Error errCode;

    /* Sanity Check */
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDac);

    if ( NULL == pSettings )
    {
        NEXUS_AudioDac_GetDefaultSettings(&settings);
        pSettings = &settings;
    }

    BAPE_Dac_GetSettings(handle->handle, &dacSettings);

    /* Adjust RAP parameters to match mute type */
    switch ( pSettings->muteType )
    {
    default:
        BDBG_ERR(("Mute mode %d not supported on this chipset", pSettings->muteType));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case NEXUS_AudioDacMuteType_eDrive0:
        dacSettings.muteType = BAPE_DacMuteType_eDrive0;
        break;
    case NEXUS_AudioDacMuteType_eDriveNegative1:
        dacSettings.muteType = BAPE_DacMuteType_eDriveNegative1;
        break;
    case NEXUS_AudioDacMuteType_eCustomValue:
        dacSettings.muteType = BAPE_DacMuteType_eCustom;
        dacSettings.customLeftValue = pSettings->muteValueLeft;
        dacSettings.customRightValue = pSettings->muteValueRight;
        break;
    }

    dacSettings.testTone.enabled = pSettings->testTone.enabled;
    BDBG_CASSERT(sizeof(dacSettings.testTone.samples) == sizeof(pSettings->testTone.samples));
    BKNI_Memcpy(dacSettings.testTone.samples, pSettings->testTone.samples, sizeof(pSettings->testTone.samples));
    dacSettings.testTone.zeroOnLeft = pSettings->testTone.zeroOnLeft;
    dacSettings.testTone.zeroOnRight = pSettings->testTone.zeroOnRight;
    dacSettings.testTone.sharedSamples = pSettings->testTone.sharedSamples;
    dacSettings.testTone.numSamplesLeft = pSettings->testTone.numSamplesLeft;
    dacSettings.testTone.numSamplesRight = pSettings->testTone.numSamplesRight;
    dacSettings.testTone.sampleRate = pSettings->testTone.sampleRate;
    dacSettings.peakGain = pSettings->peakGain;

    errCode = BAPE_Dac_SetSettings(handle->handle, &dacSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    handle->settings = *pSettings;

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Get the audio connector for an audio DAC
See Also:

 ***************************************************************************/
NEXUS_AudioOutput NEXUS_AudioDac_GetConnector(
    NEXUS_AudioDacHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDac);
    return &handle->connector;
}

static NEXUS_Error NEXUS_AudioDac_P_SetChannelMode(void *pHandle, NEXUS_AudioChannelMode channelMode)
{
    BERR_Code errCode;
    BAPE_DacSettings dacSettings;

    NEXUS_AudioDacHandle handle = (NEXUS_AudioDacHandle) pHandle;
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioDac);
    
    BAPE_Dac_GetSettings(handle->handle, &dacSettings);
    switch ( channelMode )
    {
    default:
    case NEXUS_AudioChannelMode_eStereo:
        dacSettings.stereoMode = BAPE_StereoMode_eLeftRight;
        break;
    case NEXUS_AudioChannelMode_eLeft:
        dacSettings.stereoMode = BAPE_StereoMode_eLeftLeft;
        break;
    case NEXUS_AudioChannelMode_eRight:
        dacSettings.stereoMode = BAPE_StereoMode_eRightRight;
        break;
    case NEXUS_AudioChannelMode_eSwapped:
        dacSettings.stereoMode = BAPE_StereoMode_eRightLeft;
        break;
    }

    errCode = BAPE_Dac_SetSettings(handle->handle, &dacSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

