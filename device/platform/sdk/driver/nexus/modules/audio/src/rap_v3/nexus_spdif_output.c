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
* $brcm_Revision: 19 $
* $brcm_Date: 6/19/12 5:42p $
*
* API Description:
*   API name: SpdifOutput
*    Specific APIs related to SPDIF audio outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7405/src/nexus_spdif_output.c $
* 
* 19   6/19/12 5:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 18   4/14/11 3:23p jtna
* SWDTV-6321: fix build warnings for 2.6.37 kernel
* 
* 17   1/25/11 4:59p jgarrett
* SW7420-1413: Removing assert if not shutdown
* 
* 16   12/3/10 5:50p jgarrett
* SW35230-2472: Adding CGMS-A settings for SPDIF
* 
* 15   11/5/10 5:40p jgarrett
* SW7125-698: Applying HDMI channel status information when it updates
* 
* 14   10/27/10 4:34p erickson
* SW7420-1215: add BDBG_OBJECT checking to NEXUS_AudioInput and
*  NEXUS_AudioOutput
* 
* 13   5/14/10 2:41p jgarrett
* SW7335-709: Adding option to set PLL before audio module initializes
* 
* 12   1/22/10 4:18p jgarrett
* SW7550-193: Merge to main branch
* 
* SW7550-193/1   1/19/10 3:55p chengli
* SW7550-193 : Add stubs
* 
* 11   4/2/09 2:24p jgarrett
* PR 52729: Adding HBR option for SPDIF and allowing the platform to
*  configure whether HBR is enabled
* 
* 10   3/2/09 5:37p jgarrett
* PR 51468: Renaming globals to match naming conventions
* 
* 9   2/19/09 11:19a jgarrett
* PR 52181: Connecting dither option in SetSettings
* 
* 8   2/19/09 11:13a jgarrett
* PR 52181: Adding dither option
* 
* 7   8/18/08 10:28a katrep
* PR45674: Compiler warnings in DEBUG=n builds
* 
* 6   7/17/08 2:06p jgarrett
* PR 42576: Adding BDBG_OBJECT
* 
* 5   4/2/08 11:31a erickson
* PR40198: fix DEBUG=n warning
*
* 4   4/2/08 10:24a erickson
* PR41226: check rc
*
* 3   2/19/08 4:05p erickson
* PR39744: fix bounds check
*
* 2   2/5/08 1:49p jgarrett
* PR 39017: Setting proper PLL
*
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/3   1/9/08 5:56p jgarrett
* PR 38535: Adding SetRawChannelStatus
*
* Nexus_Devel/2   11/12/07 6:49p jgarrett
* PR 36953: Removing BRAP_VER since it's missing in 3563
*
* Nexus_Devel/1   11/9/07 9:59a jgarrett
* PR 36774: Adding 7405 support
*
* Nexus_Devel/4   11/6/07 4:22p jgarrett
* PR 34954: Migrating to audio connector model
*
* Nexus_Devel/3   10/4/07 2:26p erickson
* PR34594: removed audio descriptors
*
* Nexus_Devel/2   9/28/07 5:14p jgarrett
* PR 34594: Adding sync thunks
*
* Nexus_Devel/1   9/27/07 9:19p jgarrett
* PR 34954: Initial version
*
* Nexus_Devel/2   9/27/07 7:52p jgarrett
* PR 34954: Changing some prototypes
*
* Nexus_Devel/1   9/17/07 1:33p jgarrett
* PR 34954: Splitting I2S, SPDIF, and PCM inputs and outputs
*
***************************************************************************/
#include "nexus_audio_module.h"

#if NEXUS_NUM_SPDIF_OUTPUTS

BDBG_MODULE(nexus_spdif_output);

#if NEXUS_NUM_SPDIF_OUTPUTS != 1
#error Unsupported number of SPDIF Outputs ... Code changes required.
#endif

typedef struct NEXUS_SpdifOutput
{
    NEXUS_OBJECT(NEXUS_SpdifOutput);
    bool opened;
    bool rawStatus;
    BRAP_OutputPort port;
    NEXUS_SpdifOutputSettings settings;
    NEXUS_AudioOutputObject connector;
    NEXUS_AudioRawChannelStatus rawChannelStatus;
} NEXUS_SpdifOutput;

static NEXUS_SpdifOutput g_spdifOutputs[NEXUS_NUM_SPDIF_OUTPUTS];

/***************************************************************************
Summary:
    Init the SPDIF module -- No uninit is required.
***************************************************************************/
NEXUS_Error NEXUS_SpdifOutput_P_Init(void)
{
    int i;
    NEXUS_Error errCode;

    for ( i = 0; i < NEXUS_NUM_SPDIF_OUTPUTS; i++ )
    {
        BRAP_OutputPortConfig outputSettings;

        BDBG_MSG(("Initializing SPDIF %d (RAP port %d)", i, BRAP_OutputPort_eSpdif));

        g_spdifOutputs[i].port = BRAP_OutputPort_eSpdif;

        errCode = BRAP_GetDefaultOutputConfig(
            g_NEXUS_audioModuleData.hRap,
            BRAP_OutputPort_eSpdif,
            &outputSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        /*
            This "opens" the output port in raptor.
            It must be done for each output you wish to use
        */

        outputSettings.uOutputPortSettings.sSpdifSettings.ePll = g_NEXUS_audioModuleData.moduleSettings.defaultPll;

        errCode = BRAP_SetOutputConfig(
            g_NEXUS_audioModuleData.hRap,
            &outputSettings);

        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        /* Populate settings with defaults */
        NEXUS_SpdifOutput_GetDefaultSettings(&g_spdifOutputs[i].settings);
    }

    BDBG_MSG(("Successfully initialized SPDIF Outputs"));
    return BERR_SUCCESS;
}

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
    BERR_Code errCode;
    BRAP_OutputPortConfig outputSettings;

    BDBG_ASSERT(NULL != pSettings);

    errCode = BRAP_GetDefaultOutputConfig(
        g_NEXUS_audioModuleData.hRap,
        BRAP_OutputPort_eSpdif,
        &outputSettings);
    if ( errCode )
    {
        errCode=BERR_TRACE(errCode);
    }

    pSettings->limitTo16Bits = outputSettings.uOutputPortSettings.sSpdifSettings.bLimitTo16Bits;
    pSettings->channelStatusInfo.professionalMode = outputSettings.sSpdifChanStatusParams.bProfessionalMode;
    pSettings->channelStatusInfo.swCopyRight = outputSettings.sSpdifChanStatusParams.bSwCopyRight;
    pSettings->channelStatusInfo.categoryCode = outputSettings.sSpdifChanStatusParams.ui16CategoryCode;
    pSettings->channelStatusInfo.clockAccuracy = outputSettings.sSpdifChanStatusParams.ui16ClockAccuracy;
    pSettings->channelStatusInfo.separateLRChanNum = outputSettings.sSpdifChanStatusParams.bSeparateLRChanNum;
    pSettings->channelStatusInfo.cgmsA = outputSettings.sSpdifChanStatusParams.uiCGMS_A;
    pSettings->dither = outputSettings.sSpdiffmSettings.bEnableDither;
    pSettings->burstType = outputSettings.sSpdiffmSettings.eBurstType;
    BDBG_CASSERT(NEXUS_SpdifOutputBurstType_eNone == (NEXUS_SpdifOutputBurstType)BRAP_SPDIFFM_BurstType_eNone);
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
    NEXUS_SpdifOutputHandle handle;
    BERR_Code errCode=0;
    
    if ( index >= NEXUS_NUM_SPDIF_OUTPUTS )
    {
        errCode=BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    
    handle = &g_spdifOutputs[index];
    if ( handle->opened )
    {
        BDBG_ERR(("SPDIF Port %d is already open\n", index));
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_SpdifOutput, handle);
    handle->opened = true;
    NEXUS_AUDIO_OUTPUT_INIT(&handle->connector, NEXUS_AudioOutputType_eSpdif, handle);
    handle->connector.port = BRAP_OutputPort_eSpdif;
    NEXUS_SpdifOutput_SetSettings(handle, pSettings);

    return handle;
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

    NEXUS_OBJECT_DESTROY(NEXUS_SpdifOutput, handle);
    handle->opened = false;
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
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifOutput);
    BDBG_ASSERT(NULL != pSettings);

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
    BRAP_OutputPortConfig outputSettings;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifOutput);

    if ( NULL == pSettings )
    {
        NEXUS_SpdifOutput_GetDefaultSettings(&handle->settings);
    }
    else
    {
        handle->settings = *pSettings;
    }

    BRAP_GetCurrentOutputConfig(g_NEXUS_audioModuleData.hRap,
                                BRAP_OutputPort_eSpdif,
                                &outputSettings);


    outputSettings.uOutputPortSettings.sSpdifSettings.bLimitTo16Bits = handle->settings.limitTo16Bits;
    outputSettings.sSpdifChanStatusParams.bProfessionalMode = handle->settings.channelStatusInfo.professionalMode;
    outputSettings.sSpdifChanStatusParams.bSwCopyRight = handle->settings.channelStatusInfo.swCopyRight;
    outputSettings.sSpdifChanStatusParams.ui16CategoryCode = handle->settings.channelStatusInfo.categoryCode;
    outputSettings.sSpdifChanStatusParams.ui16ClockAccuracy = handle->settings.channelStatusInfo.clockAccuracy;
    outputSettings.sSpdifChanStatusParams.bSeparateLRChanNum = handle->settings.channelStatusInfo.separateLRChanNum;
    outputSettings.sSpdifChanStatusParams.uiCGMS_A = handle->settings.channelStatusInfo.cgmsA;

    #if NEXUS_HAS_RAW_CHANNEL_STATUS
    outputSettings.bUseSpdifPackedChanStatusBits = handle->rawStatus;
    if ( outputSettings.bUseSpdifPackedChanStatusBits )
    {
        outputSettings.sSpdifPackedChanStatusBits.ui32ChStatusBits[0] = handle->rawChannelStatus.leftChannelLow;
        outputSettings.sSpdifPackedChanStatusBits.ui32ChStatusBits[1] = handle->rawChannelStatus.leftChannelHigh;
    }
    #endif

    outputSettings.sSpdiffmSettings.bEnableDither = handle->settings.dither;
    outputSettings.sSpdiffmSettings.eBurstType = handle->settings.burstType;

    errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap,
                         &outputSettings);
    if (errCode) return BERR_TRACE(errCode);

    return BERR_SUCCESS;
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
    #if NEXUS_HAS_RAW_CHANNEL_STATUS
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifOutput);
    if ( NULL == pChannelStatus )
    {
        handle->rawStatus = false;
    }
    else
    {
        handle->rawStatus = true;
        handle->rawChannelStatus = *pChannelStatus;
    }
    /* Update settings to use values from previous settings */
    return NEXUS_SpdifOutput_SetSettings(handle, &handle->settings);
    #else
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pChannelStatus);
    /* Not available on STB chips */
    return BERR_TRACE(BERR_NOT_SUPPORTED);
    #endif
}

#else

void NEXUS_SpdifOutput_GetDefaultSettings(
    NEXUS_SpdifOutputSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

NEXUS_SpdifOutputHandle NEXUS_SpdifOutput_Open(
    unsigned index,
    const NEXUS_SpdifOutputSettings *pSettings     /* Pass NULL for default settings */
    )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);    
    return NULL;
}

void NEXUS_SpdifOutput_Close(
    NEXUS_SpdifOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

void NEXUS_SpdifOutput_GetSettings(
    NEXUS_SpdifOutputHandle handle,
    NEXUS_SpdifOutputSettings *pSettings    /* [out] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_SpdifOutput_SetSettings(
    NEXUS_SpdifOutputHandle handle,
    const NEXUS_SpdifOutputSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_AudioOutput NEXUS_SpdifOutput_GetConnector(
    NEXUS_SpdifOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}

NEXUS_Error NEXUS_SpdifOutput_SetRawChannelStatus(
    NEXUS_SpdifOutputHandle handle,
    NEXUS_AudioRawChannelStatus *pChannelStatus
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pChannelStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

#endif /* NEXUS_NUM_SPDIF_OUTPUTS */

