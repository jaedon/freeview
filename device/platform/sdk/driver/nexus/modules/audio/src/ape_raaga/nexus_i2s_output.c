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
* $brcm_Workfile: nexus_i2s_output.c $
* $brcm_Revision: 11 $
* $brcm_Date: 8/3/12 4:44p $
*
* API Description:
*   API name: I2sOutput
*    Specific APIs related to I2S audio outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_i2s_output.c $
* 
* 11   8/3/12 4:44p vsilyaev
* SW7420-2330: NEXUS_OBJECT_CALSS_MAKE should be used _after_
*  protype/implementaion for the finalizer function
* 
* 10   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 9   8/31/11 4:33p jgarrett
* SWDTV-8467: Removing connector re-initialization on GetConnector()
*  call, switched default to stereo mode for DTV systems
* 
* 8   8/18/11 5:51p jgarrett
* SWDTV-6306: Merge DTV APE changes to main branch
* 
* Nexus_APE_Integration/1   7/8/11 6:40p jgarrett
* SWDTV-6760: Adding I2sMultiOutput
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
* 4   2/22/11 5:44p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
*  comments
* 
* 3   1/31/11 6:46p gskerl
* SW7422-146:Removed references to BAPE_I2sOutputSettings.mclkRate.
* 
* 2   1/18/11 5:14p gskerl
* SW7422-146:Updated to support APE API.
* 
* 1   12/17/10 3:56p jgarrett
* SW7422-146: Adding initial nexus on APE for 7422
* 
***************************************************************************/

#include "nexus_audio_module.h"
#include "priv/nexus_core_audio.h"

BDBG_MODULE(nexus_i2s_output);

#if NEXUS_NUM_I2S_OUTPUTS || NEXUS_NUM_I2S_MULTI_OUTPUTS
/***************************************************************************
Summary:
    Move and convert fields from a NEXUS_I2sOutputSettings struct to a
    BAPE_I2sOutputSettings struct.
See Also:
    NEXUS_I2sOutput_P_ConvertSettingsFromBape
 ***************************************************************************/
static NEXUS_Error NEXUS_I2sOutput_P_ConvertSettingsToBape(
    BAPE_I2sOutputSettings  *pBapeSettings,
    const NEXUS_I2sOutputSettings *pSettings
    )
{
    NEXUS_Error errCode;

    switch ( pSettings->lsbAtLRClock )
    {
    case false:
        pBapeSettings->justification = BAPE_I2sJustification_eMsbFirst;
        break;
    default:
    case true:
        pBapeSettings->justification = BAPE_I2sJustification_eLsbFirst;
        break;
    }

    switch ( pSettings->alignedToLRClock )
    {
    case false:
        pBapeSettings->dataAlignment = BAPE_I2sDataAlignment_eDelayed;
        break;
    default:
    case true:
        pBapeSettings->dataAlignment = BAPE_I2sDataAlignment_eAligned;
        break;
    }

    switch ( pSettings->lrClkPolarity )
    {
    case false:
        pBapeSettings->lrPolarity = BAPE_I2sLRClockPolarity_eLeftLow;
        break;
    default:
    case true:
        pBapeSettings->lrPolarity = BAPE_I2sLRClockPolarity_eLeftHigh;
        break;
    }

    switch ( pSettings->sclkRate )
    {
    case NEXUS_I2sOutputSclkRate_e64Fs:
        pBapeSettings->sclkRate = BAPE_SclkRate_e64Fs;
        break;
    case NEXUS_I2sOutputSclkRate_e128Fs:
        pBapeSettings->sclkRate = BAPE_SclkRate_e128Fs;
        break;
    default:
        BDBG_ERR(("NEXUS_I2sOutputSettings.sclkRate:%d is invalid", pSettings->sclkRate));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return errCode;
    }

    if ( NEXUS_I2sOutputMclkRate_eAuto !=  pSettings->mclkRate )
    {
        BDBG_ERR(("NEXUS_I2sOutputSettings.mclkRate:%d is invalid. Must be set to NEXUS_I2sOutputMclkRate_eAuto ", pSettings->mclkRate));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return errCode;
    }

    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    Move and convert fields from a NEXUS_I2sOutputSettings struct to a
    BAPE_I2sOutputSettings struct.
See Also:
    NEXUS_I2sOutput_P_ConvertSettingsToBape
 ***************************************************************************/
static NEXUS_Error NEXUS_I2sOutput_P_ConvertSettingsFromBape(
    NEXUS_I2sOutputSettings *pSettings,
    BAPE_I2sOutputSettings  *pBapeSettings
    )
{
    switch ( pBapeSettings->justification )
    {
    default:
    case BAPE_I2sJustification_eMsbFirst:
        pSettings->lsbAtLRClock = false;
        break;
    case BAPE_I2sJustification_eLsbFirst:
        pSettings->lsbAtLRClock = true;
        break;
    }

    switch ( pBapeSettings->dataAlignment )
    {
    default:
    case BAPE_I2sDataAlignment_eDelayed:
        pSettings->alignedToLRClock = false;
        break;
    case BAPE_I2sDataAlignment_eAligned:
        pSettings->alignedToLRClock = true;
        break;
    }

    switch ( pBapeSettings->lrPolarity )
    {
    default:
    case BAPE_I2sLRClockPolarity_eLeftLow:
        pSettings->lrClkPolarity = false;
        break;
    case BAPE_I2sLRClockPolarity_eLeftHigh:
        pSettings->lrClkPolarity = true;
        break;
    }

    switch ( pBapeSettings->sclkRate )
    {
    default:
    case BAPE_SclkRate_e64Fs:
        pSettings->sclkRate = NEXUS_I2sOutputSclkRate_e64Fs;
        break;
    case BAPE_SclkRate_e128Fs:
        pSettings->sclkRate = NEXUS_I2sOutputSclkRate_e128Fs;
        break;
    }

    /* BAPE doesn't expose this, just set it to Auto.  */
    pSettings->mclkRate = NEXUS_I2sOutputMclkRate_eAuto;

    return NEXUS_SUCCESS;
}
#endif

#if NEXUS_NUM_I2S_OUTPUTS
typedef struct NEXUS_I2sOutput
{
    NEXUS_OBJECT(NEXUS_I2sOutput);
    bool opened;
    BAPE_I2sOutputHandle handle;
    NEXUS_I2sOutputSettings settings;
    NEXUS_AudioOutputObject connector;
} NEXUS_I2sOutput;

static NEXUS_I2sOutput g_i2sOutputs[NEXUS_NUM_I2S_OUTPUTS];

static NEXUS_Error NEXUS_I2sOutput_P_SetChannelMode(void *pHandle, NEXUS_AudioChannelMode channelMode);

/***************************************************************************
Summary:
	Get default settings for an I2S output
See Also:

 ***************************************************************************/
void NEXUS_I2sOutput_GetDefaultSettings(
    NEXUS_I2sOutputSettings *pSettings   /* [out] default settings */
    )
{
    BAPE_I2sOutputSettings bapeI2sOutputSettings;
    NEXUS_Error errCode;

    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    BAPE_I2sOutput_GetDefaultSettings(&bapeI2sOutputSettings);

    errCode = NEXUS_I2sOutput_P_ConvertSettingsFromBape(pSettings, &bapeI2sOutputSettings);
    BDBG_ASSERT(errCode == NEXUS_SUCCESS);  /* We should never get bad stuff from BAPE */
}

/***************************************************************************
Summary:
	Open an I2S Output device
See Also:
    NEXUS_I2sOutput_Close
 ***************************************************************************/
NEXUS_I2sOutputHandle NEXUS_I2sOutput_Open(
    unsigned index,
    const NEXUS_I2sOutputSettings *pSettings
    )
{
    BAPE_I2sOutputHandle i2sHandle;
    NEXUS_Error errCode;
    BAPE_OutputPort connector;

    if ( index >= NEXUS_NUM_I2S_OUTPUTS )
    {
        BDBG_ERR(("I2sOutput %u not supported on this chipset", index));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    if ( g_i2sOutputs[index].opened )
    {
        BDBG_ERR(("I2sOutput %u is already open", index));
        return NULL;
    }
    NEXUS_OBJECT_SET(NEXUS_I2sOutput, &g_i2sOutputs[index]);

    errCode = BAPE_I2sOutput_Open( NEXUS_AUDIO_DEVICE_HANDLE, index, NULL, &i2sHandle );
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        return NULL;
    }

    g_i2sOutputs[index].opened = true;
    /* Initialize connector */
    g_i2sOutputs[index].handle = i2sHandle;
    NEXUS_AUDIO_OUTPUT_INIT(&g_i2sOutputs[index].connector, NEXUS_AudioOutputType_eI2s, &g_i2sOutputs[index]);
    g_i2sOutputs[index].connector.setChannelMode = NEXUS_I2sOutput_P_SetChannelMode;
    BDBG_OBJECT_SET(&g_i2sOutputs[index], NEXUS_I2sOutput);
    NEXUS_I2sOutput_SetSettings(&g_i2sOutputs[index], pSettings);

    BAPE_I2sOutput_GetOutputPort(i2sHandle, &connector);
    g_i2sOutputs[index].connector.port = (uint32_t)connector;

    /* Success */
    return &g_i2sOutputs[index];
}

/***************************************************************************
Summary:
	Close an I2S Output device
See Also:
    NEXUS_I2sOutput_Open
 ***************************************************************************/
static void NEXUS_I2sOutput_P_Finalizer(
    NEXUS_I2sOutputHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_I2sOutput, handle);

    NEXUS_AudioOutput_Shutdown(&handle->connector);
    BAPE_I2sOutput_Close(handle->handle);
    /* This is equivalent to BDBG_OBJECT_UNSET and also clears other flags */
    BKNI_Memset(handle, 0, sizeof(NEXUS_I2sOutput));
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_I2sOutput, NEXUS_I2sOutput_Close);


/***************************************************************************
Summary:
	Get settings for an I2S output
See Also:
	NEXUS_I2sOutput_SetSettings
 ***************************************************************************/
void NEXUS_I2sOutput_GetSettings(
    NEXUS_I2sOutputHandle handle,
    NEXUS_I2sOutputSettings *pSettings  /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_I2sOutput);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = handle->settings;
}

/***************************************************************************
Summary:
	Set settings for an I2S output
See Also:
	NEXUS_I2sOutput_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_I2sOutput_SetSettings(
    NEXUS_I2sOutputHandle handle,
    const NEXUS_I2sOutputSettings *pSettings    /* [in] Settings */
    )
{
    NEXUS_I2sOutputSettings settings;
    BAPE_I2sOutputSettings bapeI2sSettings;
    NEXUS_Error errCode;

    /* Sanity Check */
    BDBG_OBJECT_ASSERT(handle, NEXUS_I2sOutput);

    if ( NULL == pSettings )
    {
        NEXUS_I2sOutput_GetDefaultSettings(&settings);
        pSettings = &settings;
    }
    BAPE_I2sOutput_GetSettings(handle->handle, &bapeI2sSettings);

    errCode = NEXUS_I2sOutput_P_ConvertSettingsToBape(&bapeI2sSettings, pSettings);
    if (errCode)
    {
        return BERR_TRACE(errCode);
    }

    errCode = BAPE_I2sOutput_SetSettings(handle->handle, &bapeI2sSettings);
    if (errCode)
    {
        return BERR_TRACE(errCode);
    }

    handle->settings = *pSettings;

    /* success */
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Get the audio connector for an I2S output
See Also:

 ***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sOutput_GetConnector(
    NEXUS_I2sOutputHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_I2sOutput);
    return &handle->connector;
}

static NEXUS_Error NEXUS_I2sOutput_P_SetChannelMode(void *pHandle, NEXUS_AudioChannelMode channelMode)
{
    BERR_Code errCode;
    BAPE_I2sOutputSettings i2sOutputSettings;

    NEXUS_I2sOutputHandle handle = (NEXUS_I2sOutputHandle) pHandle;
    BDBG_OBJECT_ASSERT(handle, NEXUS_I2sOutput);
    
    BAPE_I2sOutput_GetSettings(handle->handle, &i2sOutputSettings);
    switch ( channelMode )
    {
    default:
    case NEXUS_AudioChannelMode_eStereo:
        i2sOutputSettings.stereoMode = BAPE_StereoMode_eLeftRight;
        break;
    case NEXUS_AudioChannelMode_eLeft:
        i2sOutputSettings.stereoMode = BAPE_StereoMode_eLeftLeft;
        break;
    case NEXUS_AudioChannelMode_eRight:
        i2sOutputSettings.stereoMode = BAPE_StereoMode_eRightRight;
        break;
    case NEXUS_AudioChannelMode_eSwapped:
        i2sOutputSettings.stereoMode = BAPE_StereoMode_eRightLeft;
        break;
    }

    errCode = BAPE_I2sOutput_SetSettings(handle->handle, &i2sOutputSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}
#else
/***************************************************************************
Summary:
	Get default settings for an I2S output
See Also:

 ***************************************************************************/
void NEXUS_I2sOutput_GetDefaultSettings(
    NEXUS_I2sOutputSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
	Open an I2S Output device
See Also:
    NEXUS_I2sOutput_Close
 ***************************************************************************/
NEXUS_I2sOutputHandle NEXUS_I2sOutput_Open(
    unsigned index,
    const NEXUS_I2sOutputSettings *pSettings
    )
{
    NEXUS_Error errCode;
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

/***************************************************************************
Summary:
	Close an I2S Output device
See Also:
    NEXUS_I2sOutput_Open
 ***************************************************************************/
void NEXUS_I2sOutput_Close(
    NEXUS_I2sOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/***************************************************************************
Summary:
	Get settings for an I2S output
See Also:
	NEXUS_I2sOutput_SetSettings
 ***************************************************************************/
void NEXUS_I2sOutput_GetSettings(
    NEXUS_I2sOutputHandle handle,
    NEXUS_I2sOutputSettings *pSettings  /* [out] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
	Set settings for an I2S output
See Also:
	NEXUS_I2sOutput_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_I2sOutput_SetSettings(
    NEXUS_I2sOutputHandle handle,
    const NEXUS_I2sOutputSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
    Get the audio connector for an I2S output
See Also:

 ***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sOutput_GetConnector(
    NEXUS_I2sOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}
#endif

#if NEXUS_NUM_I2S_MULTI_OUTPUTS
typedef struct NEXUS_I2sMultiOutput
{
    NEXUS_OBJECT(NEXUS_I2sMultiOutput);
    bool opened;
    BAPE_I2sMultiOutputHandle handle;
    NEXUS_I2sMultiOutputSettings settings;
    NEXUS_AudioOutputObject connectors[BAPE_ChannelPair_eMax];
    unsigned numConnectors;
} NEXUS_I2sMultiOutput;

static NEXUS_I2sMultiOutput g_i2sMultiOutputs[NEXUS_NUM_I2S_MULTI_OUTPUTS];

static NEXUS_Error NEXUS_I2sMultiOutput_P_SetChannelMode(void *pHandle, NEXUS_AudioChannelMode channelMode);

/***************************************************************************
Summary:
	Get default settings for an I2S output
See Also:

 ***************************************************************************/
void NEXUS_I2sMultiOutput_GetDefaultSettings(
    NEXUS_I2sMultiOutputSettings *pSettings   /* [out] default settings */
    )
{
    BAPE_I2sMultiOutputSettings bapeI2sOutputSettings;
    NEXUS_Error errCode;

    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    BAPE_I2sMultiOutput_GetDefaultSettings(&bapeI2sOutputSettings);

    switch ( bapeI2sOutputSettings.mode )
    {
    case BAPE_I2sMultiMode_eMultichannel:
        pSettings->mode = NEXUS_I2sMultiMode_eMultichannel;
        break;
    case BAPE_I2sMultiMode_eStereo:
        pSettings->mode = NEXUS_I2sMultiMode_eStereo;
        break;
    default:
        BDBG_ERR(("Unsupported I2SMultiMode %u", bapeI2sOutputSettings.mode));
        break;
    }
    
    #if NEXUS_DTV_PLATFORM
    /* Default DTV chips to the stereo interface */
    pSettings->mode = NEXUS_I2sMultiMode_eStereo;
    #endif
    
    errCode = NEXUS_I2sOutput_P_ConvertSettingsFromBape(&pSettings->i2sSettings, &bapeI2sOutputSettings.i2sSettings);
    BDBG_ASSERT(errCode == NEXUS_SUCCESS);  /* We should never get bad stuff from BAPE */
}

/***************************************************************************
Summary:
	Open an I2S Output device
See Also:
    NEXUS_I2sMultiOutput_Close
 ***************************************************************************/
NEXUS_I2sMultiOutputHandle NEXUS_I2sMultiOutput_Open(
    unsigned index,
    const NEXUS_I2sMultiOutputSettings *pSettings
    )
{
    BAPE_I2sMultiOutputHandle i2sHandle;
    NEXUS_Error errCode;
    BAPE_I2sMultiOutputSettings apeSettings;
    NEXUS_I2sMultiOutputSettings settings;
    BAPE_OutputPort connector;
    unsigned i, numConnectors=1;

    if ( NULL == pSettings )
    {
        NEXUS_I2sMultiOutput_GetDefaultSettings(&settings);
        pSettings = &settings;
    }

    if ( index >= NEXUS_NUM_I2S_MULTI_OUTPUTS )
    {
        BDBG_ERR(("I2sOutput %u not supported on this chipset", index));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    if ( g_i2sMultiOutputs[index].opened )
    {
        BDBG_ERR(("I2sOutput %u is already open", index));
        return NULL;
    }

    BAPE_I2sMultiOutput_GetDefaultSettings(&apeSettings);
    switch ( pSettings->mode )
    {
    case NEXUS_I2sMultiMode_eMultichannel:
        apeSettings.mode = BAPE_I2sMultiMode_eMultichannel;
        break;
    case NEXUS_I2sMultiMode_eStereo:
        apeSettings.mode = BAPE_I2sMultiMode_eStereo;
#if NEXUS_DTV_PLATFORM
        numConnectors = 3;    /* DTV chips only have a 5.1 interface */
#else
        numConnectors = 4;    /* STB/BD chips have 7.1 */
#endif
        break;
    default:
        BDBG_ERR(("Unsupported I2SMultiMode %u", pSettings->mode));
        (void)BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    errCode = BAPE_I2sMultiOutput_Open( NEXUS_AUDIO_DEVICE_HANDLE, index, &apeSettings, &i2sHandle );
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        return NULL;
    }

    g_i2sMultiOutputs[index].opened = true;
    /* Defer connector init until GetConnector() */
    g_i2sMultiOutputs[index].handle = i2sHandle;
    NEXUS_OBJECT_SET(NEXUS_I2sMultiOutput, &g_i2sMultiOutputs[index]);
    errCode = NEXUS_I2sMultiOutput_SetSettings(&g_i2sMultiOutputs[index], pSettings);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        NEXUS_I2sMultiOutput_Close(&g_i2sMultiOutputs[index]);
        return NULL;
    }
    
    for ( i = 0; i < numConnectors; i++ )
    {
        BAPE_I2sMultiOutput_GetStereoOutputPort(i2sHandle, i, &connector);
        NEXUS_AUDIO_OUTPUT_INIT(&g_i2sMultiOutputs[index].connectors[i], NEXUS_AudioOutputType_eI2s, &g_i2sMultiOutputs[index]);
        g_i2sMultiOutputs[index].connectors[i].setChannelMode = NEXUS_I2sMultiOutput_P_SetChannelMode;
        g_i2sMultiOutputs[index].connectors[i].port = (uint32_t)connector;
    }

    g_i2sMultiOutputs[index].opened = true;
    g_i2sMultiOutputs[index].numConnectors = numConnectors;
    
    /* Success */
    return &g_i2sMultiOutputs[index];    
}

/***************************************************************************
Summary:
	Close an I2S Output device
See Also:
    NEXUS_I2sMultiOutput_Open
 ***************************************************************************/
void NEXUS_I2sMultiOutput_Close(
    NEXUS_I2sMultiOutputHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_I2sMultiOutput, handle);
    NEXUS_OBJECT_RELEASE(NEXUS_I2sMultiOutput, handle);
    return;
}

static void NEXUS_I2sMultiOutput_P_Close(
    NEXUS_I2sMultiOutputHandle handle
    )
{
    unsigned i;

    NEXUS_OBJECT_ASSERT(NEXUS_I2sMultiOutput, handle);

    for ( i = 0; i < handle->numConnectors; i++ )
    {
        NEXUS_AudioOutput_Shutdown(&handle->connectors[i]);
    }
    BAPE_I2sMultiOutput_Close(handle->handle);
    /* This is equivalent to BDBG_OBJECT_UNSET and also clears other flags */
    BKNI_Memset(handle, 0, sizeof(NEXUS_I2sMultiOutput));
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_I2sMultiOutput, NEXUS_I2sMultiOutput_P_Close);

/***************************************************************************
Summary:
	Get settings for an I2S output
See Also:
	NEXUS_I2sMultiOutput_SetSettings
 ***************************************************************************/
void NEXUS_I2sMultiOutput_GetSettings(
    NEXUS_I2sMultiOutputHandle handle,
    NEXUS_I2sMultiOutputSettings *pSettings  /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_I2sMultiOutput);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = handle->settings;
}

/***************************************************************************
Summary:
	Set settings for an I2S output
See Also:
	NEXUS_I2sMultiOutput_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_I2sMultiOutput_SetSettings(
    NEXUS_I2sMultiOutputHandle handle,
    const NEXUS_I2sMultiOutputSettings *pSettings    /* [in] Settings */
    )
{
    BAPE_I2sMultiOutputSettings bapeI2sSettings;
    NEXUS_Error errCode;

    /* Sanity Check */
    BDBG_OBJECT_ASSERT(handle, NEXUS_I2sMultiOutput);

    BAPE_I2sMultiOutput_GetSettings(handle->handle, &bapeI2sSettings);

    /* Ignore changes to mode.  They're not supported anyway. */

    errCode = NEXUS_I2sOutput_P_ConvertSettingsToBape(&bapeI2sSettings.i2sSettings, &pSettings->i2sSettings);
    if (errCode)
    {
        return BERR_TRACE(errCode);
    }

    errCode = BAPE_I2sMultiOutput_SetSettings(handle->handle, &bapeI2sSettings);
    if (errCode)
    {
        return BERR_TRACE(errCode);
    }

    handle->settings = *pSettings;

    /* success */
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Get the audio connector for an I2S output
See Also:

 ***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sMultiOutput_GetConnector(
    NEXUS_I2sMultiOutputHandle handle
    )
{
    return NEXUS_I2sMultiOutput_GetStereoConnector(handle, 0);
}

NEXUS_AudioOutput NEXUS_I2sMultiOutput_GetStereoConnector(
    NEXUS_I2sMultiOutputHandle handle,
    NEXUS_AudioChannelPair channelPair
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_I2sMultiOutput);

    if ( (unsigned)channelPair < handle->numConnectors )
    {
        return &handle->connectors[channelPair];
    }
    else
    {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }    
}

static NEXUS_Error NEXUS_I2sMultiOutput_P_SetChannelMode(void *pHandle, NEXUS_AudioChannelMode channelMode)
{
    BERR_Code errCode;
    BAPE_I2sMultiOutputSettings i2sOutputSettings;

    NEXUS_I2sMultiOutputHandle handle = (NEXUS_I2sMultiOutputHandle) pHandle;
    BDBG_OBJECT_ASSERT(handle, NEXUS_I2sMultiOutput);
    
    BAPE_I2sMultiOutput_GetSettings(handle->handle, &i2sOutputSettings);
    switch ( channelMode )
    {
    default:
    case NEXUS_AudioChannelMode_eStereo:
        i2sOutputSettings.i2sSettings.stereoMode = BAPE_StereoMode_eLeftRight;
        break;
    case NEXUS_AudioChannelMode_eLeft:
        i2sOutputSettings.i2sSettings.stereoMode = BAPE_StereoMode_eLeftLeft;
        break;
    case NEXUS_AudioChannelMode_eRight:
        i2sOutputSettings.i2sSettings.stereoMode = BAPE_StereoMode_eRightRight;
        break;
    case NEXUS_AudioChannelMode_eSwapped:
        i2sOutputSettings.i2sSettings.stereoMode = BAPE_StereoMode_eRightLeft;
        break;
    }

    errCode = BAPE_I2sMultiOutput_SetSettings(handle->handle, &i2sOutputSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}
#else /* NEXUS_NUM_I2S_OUTPUTS || NEXUS_NUM_I2S_MULTI_OUTPUTS */

typedef struct NEXUS_I2sMultiOutput
{
    NEXUS_OBJECT(NEXUS_I2sMultiOutput);
} NEXUS_I2sMultiOutput;


/***************************************************************************
Summary:
	Get default settings for an I2S output
See Also:

 ***************************************************************************/
void NEXUS_I2sMultiOutput_GetDefaultSettings(
    NEXUS_I2sMultiOutputSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
	Open an I2S Output device
See Also:
    NEXUS_I2sMultiOutput_Close
 ***************************************************************************/
NEXUS_I2sMultiOutputHandle NEXUS_I2sMultiOutput_Open(
    unsigned index,
    const NEXUS_I2sMultiOutputSettings *pSettings
    )
{
    NEXUS_Error errCode;
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

/***************************************************************************
Summary:
	Close an I2S Output device
See Also:
    NEXUS_I2sMultiOutput_Open
 ***************************************************************************/
static void NEXUS_I2sMultiOutput_P_Finalizer(
    NEXUS_I2sMultiOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}
NEXUS_OBJECT_CLASS_MAKE(NEXUS_I2sMultiOutput, NEXUS_I2sMultiOutput_Close); /* stub */

/***************************************************************************
Summary:
	Get settings for an I2S output
See Also:
	NEXUS_I2sMultiOutput_SetSettings
 ***************************************************************************/
void NEXUS_I2sMultiOutput_GetSettings(
    NEXUS_I2sMultiOutputHandle handle,
    NEXUS_I2sMultiOutputSettings *pSettings  /* [out] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
	Set settings for an I2S output
See Also:
	NEXUS_I2sMultiOutput_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_I2sMultiOutput_SetSettings(
    NEXUS_I2sMultiOutputHandle handle,
    const NEXUS_I2sMultiOutputSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
    Get the audio connector for an I2S output
See Also:

 ***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sMultiOutput_GetConnector(
    NEXUS_I2sMultiOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}

NEXUS_AudioOutput NEXUS_I2sMultiOutput_GetStereoConnector(
    NEXUS_I2sMultiOutputHandle handle,
    NEXUS_AudioChannelPair channelPair
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(channelPair);
    return NULL;
}

#endif /* NEXUS_NUM_I2S_OUTPUTS || NEXUS_NUM_I2S_MULTI_OUTPUTS */
