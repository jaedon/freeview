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
* $brcm_Workfile: nexus_spdif_output.c $
* $brcm_Revision: 7 $
* $brcm_Date: 6/19/12 12:42p $
*
* API Description:
*   API name: SpdifOutput
*    Specific APIs related to SPDIF audio outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7422/src/nexus_spdif_output.c $
* 
* 7   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 6   7/6/11 6:41p jgarrett
* SW7552-55: Mapping channelMode to stereoMode
* 
* 5   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 4   4/26/11 11:35a jgarrett
* SW7425-437: Resolving kernel mode shutdown issues
* 
* 3   4/20/11 6:16p jtna
* SW7425-365: fix build warnings for 2.6.37 kernel
* 
* 2   2/22/11 5:44p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
*  comments
* 
* 1   12/17/10 3:57p jgarrett
* SW7422-146: Adding initial nexus on APE for 7422
* 
***************************************************************************/
#include "nexus_audio_module.h"

BDBG_MODULE(nexus_spdif_output);

typedef struct NEXUS_SpdifOutput
{
    NEXUS_OBJECT(NEXUS_SpdifOutput);
    bool opened;
    bool rawStatus;
    BAPE_SpdifOutputHandle handle;
    NEXUS_SpdifOutputSettings settings;
    NEXUS_AudioOutputObject connector;
    NEXUS_AudioRawChannelStatus rawChannelStatus;
} NEXUS_SpdifOutput;

static NEXUS_SpdifOutput g_spdifOutputs[NEXUS_NUM_SPDIF_OUTPUTS];

static NEXUS_Error NEXUS_SpdifOutput_P_SetChannelMode(void *pHandle, NEXUS_AudioChannelMode channelMode);

/***************************************************************************
Summary:
    Get default settings for a SPDIF Output
See Also:
    NEXUS_SpdifOutput_Get
 ***************************************************************************/
void NEXUS_SpdifOutput_GetDefaultSettings(
    NEXUS_SpdifOutputSettings *pSettings   /* [out] default settings */
    )
{
    BAPE_SpdifOutputSettings defaults;
    BSTD_UNUSED(pSettings);
    BAPE_SpdifOutput_GetDefaultSettings(&defaults);
    pSettings->limitTo16Bits = defaults.limitTo16Bits;
    pSettings->dither = defaults.ditherEnabled;
    BDBG_CASSERT(NEXUS_SpdifOutputBurstType_eMax == (NEXUS_SpdifOutputBurstType)BAPE_SpdifBurstType_eMax);
    pSettings->burstType = defaults.underflowBurst;
    pSettings->channelStatusInfo.professionalMode = defaults.channelStatus.professional;
    pSettings->channelStatusInfo.swCopyRight = defaults.channelStatus.copyright;
    pSettings->channelStatusInfo.categoryCode = defaults.channelStatus.categoryCode;
    pSettings->channelStatusInfo.clockAccuracy = defaults.channelStatus.clockAccuracy;
}

/***************************************************************************
Summary:
    Open a SPDIF Output
See Also:
    NEXUS_SpdifOutput_Close
 ***************************************************************************/
NEXUS_SpdifOutputHandle NEXUS_SpdifOutput_Open(
    unsigned index,
    const NEXUS_SpdifOutputSettings *pSettings     /* Pass NULL for default settings */
    )
{
    BAPE_SpdifOutputHandle spdifHandle;
    BERR_Code errCode;
    BAPE_OutputPort connector;

    /* Sanity check */
    if ( index >= NEXUS_NUM_SPDIF_OUTPUTS )
    {
        BDBG_ERR(("SpdifOutput Index %u out of range", index));
        return NULL;
    }

    if ( g_spdifOutputs[index].opened )
    {
        BDBG_ERR(("SpdifOutput %d already open", index));
        return NULL;
    }

    errCode = BAPE_SpdifOutput_Open(NEXUS_AUDIO_DEVICE_HANDLE, index, NULL, &spdifHandle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        return NULL;
    }

    g_spdifOutputs[index].opened = true;
    /* Initialize connector */
    g_spdifOutputs[index].handle = spdifHandle;
    NEXUS_AUDIO_OUTPUT_INIT(&g_spdifOutputs[index].connector, NEXUS_AudioOutputType_eSpdif, &g_spdifOutputs[index]);
    g_spdifOutputs[index].connector.setChannelMode = NEXUS_SpdifOutput_P_SetChannelMode;
    NEXUS_OBJECT_SET(NEXUS_SpdifOutput, &g_spdifOutputs[index]);
    NEXUS_SpdifOutput_SetSettings(&g_spdifOutputs[index], pSettings);

    BAPE_SpdifOutput_GetOutputPort(spdifHandle, &connector);
    g_spdifOutputs[index].connector.port = (uint32_t)connector;

    /* Success */
    return &g_spdifOutputs[index];
}

/***************************************************************************
Summary:
    Close a SPDIF Output
Description:
    Input to the SPDIF output must be removed prior to closing.
See Also:
    NEXUS_SpdifOutput_Open
    NEXUS_AudioOutput_RemoveInput
    NEXUS_AudioOutput_RemoveAllInputs
 ***************************************************************************/
static void NEXUS_SpdifOutput_P_Finalizer(
    NEXUS_SpdifOutputHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SpdifOutput, handle);

    NEXUS_AudioOutput_Shutdown(&handle->connector);
    BAPE_SpdifOutput_Close(handle->handle);
    BKNI_Memset(handle, 0, sizeof(NEXUS_SpdifOutput));
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_SpdifOutput, NEXUS_SpdifOutput_Close);

/***************************************************************************
Summary:
    Get Settings for a SPDIF Output
See Also:
    NEXUS_SpdifOutput_SetSettings
 ***************************************************************************/
void NEXUS_SpdifOutput_GetSettings(
    NEXUS_SpdifOutputHandle handle,
    NEXUS_SpdifOutputSettings *pSettings    /* [out] Settings */
    )
{
    /* Sanity Check */
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifOutput);
    BDBG_ASSERT(NULL != pSettings);

    /* Copy settings */
    *pSettings = handle->settings;
}

/***************************************************************************
Summary:
    Set Settings for a SPDIF Output
See Also:
    NEXUS_SpdifOutput_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_SpdifOutput_SetSettings(
    NEXUS_SpdifOutputHandle handle,
    const NEXUS_SpdifOutputSettings *pSettings
    )
{
    int i;
    BERR_Code errCode;
    NEXUS_SpdifOutputSettings defaults;
    BAPE_SpdifOutputSettings settings;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifOutput);
    if ( NULL == pSettings )
    {
        NEXUS_SpdifOutput_GetDefaultSettings(&defaults);
        pSettings = &defaults;
    }

    BAPE_SpdifOutput_GetSettings(handle->handle, &settings);
    
    settings.limitTo16Bits = pSettings->limitTo16Bits;
    settings.ditherEnabled = pSettings->dither;
    settings.useRawChannelStatus = handle->rawStatus;
    if ( handle->rawStatus )
    {
        uint32_t raw;
        raw = handle->rawChannelStatus.leftChannelLow;
        for ( i = 0; i < 4; i++ )
        {
            settings.rawChannelStatus[i] = raw&0xff;
            raw >>= 8;
        }
        raw = handle->rawChannelStatus.leftChannelHigh;
        for ( i = 4; i < 8; i++ )
        {
            settings.rawChannelStatus[i] = raw&0xff;
            raw >>= 8;
        }
    }
    else
    {
        settings.channelStatus.professional = pSettings->channelStatusInfo.professionalMode;
        settings.channelStatus.copyright = pSettings->channelStatusInfo.swCopyRight;
        settings.channelStatus.categoryCode = pSettings->channelStatusInfo.categoryCode;
        settings.channelStatus.clockAccuracy = pSettings->channelStatusInfo.clockAccuracy;
    }

    errCode = BAPE_SpdifOutput_SetSettings(handle->handle, &settings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    handle->settings = *pSettings;

    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    Get the audio connector for a SPDIF output
See Also:

 ***************************************************************************/
NEXUS_AudioOutput NEXUS_SpdifOutput_GetConnector(
    NEXUS_SpdifOutputHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifOutput);
    return &handle->connector;
}

/***************************************************************************
Summary:
    Program the transmitted channel status in raw format.
Description:
    This API will set the SPDIF channel status bits using raw 64-bit values
    per channel.  Not available on all platforms.  Once set, the values in
    NEXUS_SpdifOutputSettings will be ignored.  To clear these settings, pass
    NULL for pChannelStatus.
See Also:
    NEXUS_AudioDecoder_GetRawChannelStatus
 ***************************************************************************/
NEXUS_Error NEXUS_SpdifOutput_SetRawChannelStatus(
    NEXUS_SpdifOutputHandle handle,
    NEXUS_AudioRawChannelStatus *pChannelStatus
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifOutput);
    if ( NULL == pChannelStatus )
    {
        handle->rawStatus = false;
    }
    else
    {
        handle->rawStatus = true;
        handle->rawChannelStatus = *pChannelStatus;
        NEXUS_SpdifOutput_SetSettings(handle, &handle->settings);
    }
    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_SpdifOutput_P_SetChannelMode(void *pHandle, NEXUS_AudioChannelMode channelMode)
{
    BERR_Code errCode;
    BAPE_SpdifOutputSettings spdifOutputSettings;

    NEXUS_SpdifOutputHandle handle = (NEXUS_SpdifOutputHandle) pHandle;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifOutput);
    
    BAPE_SpdifOutput_GetSettings(handle->handle, &spdifOutputSettings);
    switch ( channelMode )
    {
    default:
    case NEXUS_AudioChannelMode_eStereo:
        spdifOutputSettings.stereoMode = BAPE_StereoMode_eLeftRight;
        break;
    case NEXUS_AudioChannelMode_eLeft:
        spdifOutputSettings.stereoMode = BAPE_StereoMode_eLeftLeft;
        break;
    case NEXUS_AudioChannelMode_eRight:
        spdifOutputSettings.stereoMode = BAPE_StereoMode_eRightRight;
        break;
    case NEXUS_AudioChannelMode_eSwapped:
        spdifOutputSettings.stereoMode = BAPE_StereoMode_eRightLeft;
        break;
    }

    errCode = BAPE_SpdifOutput_SetSettings(handle->handle, &spdifOutputSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

