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
* $brcm_Revision: 6 $
* $brcm_Date: 6/20/12 1:15p $
*
* API Description:
*   API name: SpdifOutput
*    Specific APIs related to SPDIF audio outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/src/nexus_spdif_output.c $
* 
* 6   6/20/12 1:15p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 5   9/8/09 6:29p mward
* SW7400-2385: Independent delay fixes from PR56621 branch.
* 
* PR56621/2   8/24/09 12:05p mward
* SW7400-2385: Not necessary to force bIndpDlyRqd to 0 when setting
*  settings.
* 
* PR56621/1   8/5/09 5:21p mward
* PR56621: Independent delay must be set before output port is added.
* 
* 4   6/10/09 6:47p mward
* PR 53152: NEXUS_SpdifOutput_Open() was clearing connector that were,
*  and should be, initialized.
* 
* 3   3/13/09 12:21p mward
* PR 51468: Renaming globals to match naming conventions
* 
* 2   10/31/08 10:22a mward
* PR47739: Merge BDBG_OBJECTs and other updates from 7405 implementation.
* 
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/5   1/9/08 5:56p jgarrett
* PR 38535: Adding SetRawChannelStatus stub
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
    BRAP_OutputPort port;
    NEXUS_SpdifOutputSettings settings;
    NEXUS_AudioOutputObject connector;
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
        BRAP_OutputSettings settings;

        BDBG_MSG(("Initializing SPDIF %d (RAP port %d)", i, BRAP_OutputPort_eSpdif));

        g_spdifOutputs[i].port = BRAP_OutputPort_eSpdif;

        errCode = BRAP_GetOutputDefaultConfig(
            g_NEXUS_audioModuleData.hRap,
            BRAP_OutputPort_eSpdif,
            &settings);

        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        /* 
            This "opens" the output port in raptor.  
            It must be done for each output you wish to use 
        */

        errCode = BRAP_SetOutputConfig(
            g_NEXUS_audioModuleData.hRap,
            &settings);

        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

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
    BRAP_OutputSettings outputSettings;

    BDBG_ASSERT(NULL != pSettings);

    errCode = BRAP_GetOutputDefaultConfig(
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
    BERR_Code errCode=0;
    if ( index >= NEXUS_NUM_SPDIF_OUTPUTS )
    {
        errCode=BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    if ( g_spdifOutputs[index].opened )
    {
        BDBG_ERR(("SPDIF Port %d is already open\n", index));
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_SpdifOutput, &g_spdifOutputs[index]);
    g_spdifOutputs[index].opened = true;
    g_spdifOutputs[index].connector.objectType = NEXUS_AudioOutputType_eSpdif;
    g_spdifOutputs[index].connector.pObjectHandle = &g_spdifOutputs[index];
    g_spdifOutputs[index].connector.port = BRAP_OutputPort_eSpdif;
    if (NULL == pSettings)
    {
        /* Populate settings with defaults */
        NEXUS_SpdifOutput_GetDefaultSettings(&g_spdifOutputs[index].settings);
    }
    else
    {
        NEXUS_SpdifOutput_SetSettings(&g_spdifOutputs[index], pSettings);
    }

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

    if ( handle->connector.pMixerData )
    {
        BDBG_ERR(("Output is still active.  Please call NEXUS_AudioOutput_Shutdown first."));
        BDBG_ASSERT(NULL == handle->connector.pMixerData);
    }

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
    BRAP_OutputSettings outputSettings;
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

    BRAP_GetOutputConfig(g_NEXUS_audioModuleData.hRap,
                         BRAP_OutputPort_eSpdif,
                         &outputSettings);

    outputSettings.uOutputPortSettings.sSpdifSettings.bLimitTo16Bits = handle->settings.limitTo16Bits;
    outputSettings.sSpdifChanStatusParams.bProfessionalMode = handle->settings.channelStatusInfo.professionalMode;
    outputSettings.sSpdifChanStatusParams.bSwCopyRight = handle->settings.channelStatusInfo.swCopyRight;
    outputSettings.sSpdifChanStatusParams.ui16CategoryCode = handle->settings.channelStatusInfo.categoryCode;
    outputSettings.sSpdifChanStatusParams.ui16ClockAccuracy = handle->settings.channelStatusInfo.clockAccuracy;
    outputSettings.sSpdifChanStatusParams.bSeparateLRChanNum = handle->settings.channelStatusInfo.separateLRChanNum;

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
	Apply output-specific start-time settings to the raptor structure
Description:
    This routine will be called when the channels up-stream are started.
    It requires knowledge of raptor structures, but no actual raptor function 
    calls.  This is required for things like LR swap because some part of the 
    logic is in a downmix mode and some is in the DAC settings.  It's also
    required for outputs that are not part of the audio module (e.g. HDMI)
 ***************************************************************************/
NEXUS_Error NEXUS_SpdifOutput_P_AdjustRapSettings(
    NEXUS_SpdifOutputHandle handle,
    BRAP_OutputSettings *pSettings    /* [in/out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifOutput);
    BDBG_ASSERT(NULL != pSettings);
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);

    /* Everything is done at SetSettings time for SPDIF */

    return BERR_SUCCESS;
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
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pChannelStatus);
    /* Not available on STB chips */
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

#else

/* TODO: Add stubs if required */

#endif /* NEXUS_NUM_SPDIF_OUTPUTS */
