/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_output.c $
* $brcm_Revision: 27 $
* $brcm_Date: 2/12/10 5:53p $
*
* API Description:
*   Private routines internal to the audio module
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/src/nexus_audio_output.c $
* 
* 27   2/12/10 5:53p mward
* SW7405-3774: add mute control support to sync channel
* 
* 26   9/24/09 3:34p gmohile
* SW7403-890 : FIx 7401/03 build
* 
* 25   9/8/09 6:28p mward
* SW7400-2385: Independent delay fixes from PR56621 branch.
* 
* PR56621/5   8/25/09 11:35a mward
* SW7400-2385: Independent delay not allowed with multichannel.
* 
* PR56621/4   8/11/09 4:57p mward
* PR 56621: Downgrade debug messages.
* 
* PR56621/3   8/7/09 5:03p mward
* PR 57329 : Fix spdif output
* 
* 24   8/4/09 8:19p gmohile
* PR 57329 : Fix spdif output
* 
* PR56621/2   8/7/09 3:56p mward
* PR57280: Set output's current sample rate when it is known, in the
*  sample rate interrupt, set it unknown initially.  Set HDMI from the
*  current sample rate if it is known.
* 
* PR56621/1   8/5/09 5:20p mward
* PR56621: Independent delay must be set before output port is added.
* 
* 23   8/4/09 2:51p gmohile
* PR 56512 : Add 7401/03 support
* 
* 22   6/26/09 6:43p mward
* PR 56384:  Added NEXUS_AudioOutput_P_SetVcxo().
* 
* 21   6/12/09 4:13p mward
* PR 49702: Apply additional settings for multi-channel HDMI.
* 
* 20   6/10/09 6:41p mward
* PR 53152: Add debug messages for channel order.
* 
* 19   6/8/09 6:13p mward
* PR55830:  Initial implementation of PCM audio capture for 7400.
* 
* 18   5/28/09 7:16p mward
* PR 49702: Add NEXUS_AudioOutput_P_SetHDMI(), debug.
* 
* 17   3/13/09 12:20p mward
* PR 51468: Renaming globals to match naming conventions
* 
* 16   2/26/09 9:02p mward
* PR52440,PR52441: Add trick mute.
* 
* 15   2/25/09 7:28p mward
* PR52437: outputSettings.sSpdifChanStatusParams.bLinear_PCM does not
*  accurately indicate compressed / PCM.
* 
* 14   2/25/09 6:21p mward
* PR47739:  Sync status and settings merged from 7405.
* 
* 13   11/20/08 2:06p mward
* PR47739: Also enable audio on RFM when setting DAC slave.
* 
* 12   11/20/08 10:08a mward
* PR47739: Compiler warning for unused with DEBUG=n.
* 
* 11   11/17/08 11:40a mward
* PR47739: Add/remove port wrappers not needed.
* 
* 10   11/14/08 5:51p mward
* PR47739:  Functions to add/remove BRAP output ports only if not already
*  done.
* 
* 9   11/11/08 5:05p mward
* PR47739: Volume must be applied to only PCM outputs. Initialize Flex
*  port settings.
* 
* 8   11/10/08 11:29a mward
* PR47739: Defer application of settings until the output is started
*  (opened).  Other updates from 7405 implementation.
* 
* 7   10/31/08 10:21a mward
* PR47739: Merge BDBG_OBJECTs and other updates from 7405 implementation.
* 
* 6   10/27/08 7:19p mward
* PR46055: Revising equalizer API
* 
* 5   3/11/08 3:08p erickson
* PR40222: added independent audio output delay for both the user and
*  SyncChannel
*
* 4   2/21/08 9:12a erickson
* PR39405: stub RFM
*
* 3   2/7/08 11:57a vsilyaev
* PR 38682: Changed return type of XXX_Shutdown routines to void
*
* 2   2/5/08 5:17p jgarrett
* PR 39017: Adding SyncChannel stubs
*
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/5   1/8/08 7:37p jgarrett
* PR 38535: Updating to new 3563 APIs
*
* Nexus_Devel/4   11/6/07 4:22p jgarrett
* PR 34954: Migrating to audio connector model
*
* Nexus_Devel/3   10/4/07 2:25p erickson
* PR34594: removed audio descriptors
*
* Nexus_Devel/2   9/28/07 5:14p jgarrett
* PR 34594: Adding sync thunks
*
* Nexus_Devel/1   9/27/07 7:52p jgarrett
* PR 34954: Adding output functions
*
***************************************************************************/
#include "nexus_audio_module.h"
#include "priv/nexus_audio_output_priv.h"
#if NEXUS_NUM_HDMI_OUTPUTS
#include "nexus_hdmi_output.h"
#include "priv/nexus_hdmi_output_priv.h"
#endif
#if NEXUS_NUM_RFM_OUTPUTS
#include "priv/nexus_rfm_priv.h"
#endif
#include "bchp_vcxo_0_rm.h"
#if BCHP_CHIP == 7400
#include "bchp_vcxo_1_rm.h"
#include "bchp_vcxo_ctl_misc.h"
#endif

BDBG_MODULE(nexus_audio_output);

BDBG_OBJECT_ID(NEXUS_AudioOutputData);

typedef struct NEXUS_AudioOutputData
{
    BDBG_OBJECT(NEXUS_AudioOutputData)
    NEXUS_AudioOutputSettings settings;
    NEXUS_AudioInput input;
    NEXUS_AudioOutputSyncSettings syncSettings;
    NEXUS_AudioOutputStatus status;
    BAVC_AudioSamplingRate sampleRate;
    bool settingsChanged;
    bool addedToRapCh;
    bool trickMute;
    bool syncMute; /* could be moved into syncSettings if sync were refactored to know to mute sinks in addition to sources */
} NEXUS_AudioOutputData;

static NEXUS_AudioOutputData *NEXUS_AudioOutput_P_CreateData(NEXUS_AudioOutput output);

#ifdef BDBG_DEBUG_BUILD
static const char *g_pRapOutputPortNames[BRAP_OutputPort_eMax] =
{
    "Spdif",
	"Dac0",
	"I2s0",
	"I2s1",
	"I2s2",
	"Dac1",
	"Mai",
	"Flex",
	"RfMod"
};
#endif
/***************************************************************************
Summary:
	Get default settings for an audio output
See Also:
    NEXUS_AudioOutput_Open
 ***************************************************************************/
void NEXUS_AudioOutput_GetDefaultSettings(
    NEXUS_AudioOutputSettings *pSettings      /* [out] Settings */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(NEXUS_AudioOutputSettings));
    pSettings->defaultSampleRate = 48000;
    pSettings->leftVolume = NEXUS_AUDIO_VOLUME_DB_NORMAL;
    pSettings->rightVolume = NEXUS_AUDIO_VOLUME_DB_NORMAL;
}

/***************************************************************************
Summary:
	Get settings of an audio output
See Also:
    NEXUS_AudioOutput_SetSettings
 ***************************************************************************/
void NEXUS_AudioOutput_GetSettings(
    NEXUS_AudioOutput output,
    NEXUS_AudioOutputSettings *pSettings    /* [out] Current Settings */
    )
{
    NEXUS_AudioOutputData *pData;

    BDBG_ASSERT(NULL != output);
    BDBG_ASSERT(NULL != pSettings);

    if ( NULL == output->pMixerData )
    {
        NEXUS_AudioOutput_GetDefaultSettings(pSettings);
        return;
    }

    pData = output->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
    *pSettings = pData->settings;

    /* Success */
}

/***************************************************************************
Summary:
    Set settings of an audio output
See Also:
    NEXUS_AudioOutput_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_SetSettings(
    NEXUS_AudioOutput output,
    const NEXUS_AudioOutputSettings *pSettings
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioOutputSettings defaultSettings;
    NEXUS_AudioOutputData *pData;
    BDBG_ASSERT(NULL != output);

    if ( NULL == pSettings )
    {
        NEXUS_AudioOutput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    BDBG_MSG(("%s(%s):",__FUNCTION__,g_pRapOutputPortNames[output->port]));
    if ( NULL == output->pMixerData )
    {
        pData = NEXUS_AudioOutput_P_CreateData(output);
        if ( NULL == pData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }

    pData = output->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
    /* Store settings */
    pData->settings = *pSettings;
    pData->settingsChanged = true;

    /* Apply volume settings only if we have a destination */
    if ( pData->input && NEXUS_AudioInput_P_HasOutputDestination(pData->input, output) )
    {
        errCode = NEXUS_AudioOutput_P_ApplySettings(output);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    else if (!pData->input)
    {
        BDBG_MSG(("%s() not applying settings.  NULL pData->input",__FUNCTION__));
    }
    else if (!NEXUS_AudioInput_P_HasOutputDestination(pData->input, output))
    {
        BDBG_MSG(("%s() not applying settings.  No output destination",__FUNCTION__));
    }
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	Add an input to this output
See Also:
    NEXUS_AudioOutput_RemoveInput
    NEXUS_AudioOutput_RemoveAllInputs
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_AddInput(
    NEXUS_AudioOutput output,
    NEXUS_AudioInput input
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioOutputData *pData;

    BDBG_ASSERT(NULL != output);
    BDBG_ASSERT(NULL != input);

    if ( NULL == output->pMixerData )
    {
        pData = NEXUS_AudioOutput_P_CreateData(output);
        if ( NULL == pData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }

    pData = output->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    if ( pData->input != NULL )
    {
        BDBG_WRN(("Input %p is already connected to this output", output->pMixerData));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( NEXUS_AudioInput_P_IsRunning(input) )
    {
        BDBG_WRN(("Input %p is running.  Please stop first.", input));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    errCode = NEXUS_AudioInput_P_ConnectOutput(input, output);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    pData->input = input;

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	Add an input to this output
See Also:
    NEXUS_AudioOutput_AddInput
    NEXUS_AudioOutput_RemoveAllInputs
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_RemoveInput(
    NEXUS_AudioOutput output,
    NEXUS_AudioInput input
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioOutputData *pData;

    BDBG_ASSERT(NULL != output);
    BDBG_ASSERT(NULL != input);

    pData = output->pMixerData;

    if ( NULL == pData || input != pData->input )
    {
        BDBG_ERR(("Input %p is not connected to this output.", input));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    if ( NEXUS_AudioInput_P_IsRunning(input) )
    {
        BDBG_WRN(("Input %p is running.  Please stop first.", input));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    errCode = NEXUS_AudioInput_P_DisconnectOutput(input, output);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    pData->input = NULL;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	Add an input to this output
See Also:
    NEXUS_AudioOutput_AddInput
    NEXUS_AudioOutput_RemoveInput
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_RemoveAllInputs(
    NEXUS_AudioOutput output
    )
{
    NEXUS_AudioOutputData *pData;

    BDBG_MSG(("%s(%s):",__FUNCTION__,g_pRapOutputPortNames[output->port]));

    BDBG_ASSERT(NULL != output);

    pData = output->pMixerData;
    if ( pData && NULL != pData->input )
    {
        NEXUS_Error errCode;

        BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
        errCode = NEXUS_AudioOutput_RemoveInput(output, pData->input);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;;
}

/***************************************************************************
Summary:
	Shutdown this output handle
Description:
    This routine should be called before the specific output object
    (e.g. AudioDac) is closed.
See Also:
    NEXUS_AudioOutput_AddInput
    NEXUS_AudioOutput_RemoveInput
 ***************************************************************************/
void NEXUS_AudioOutput_Shutdown(
    NEXUS_AudioOutput output
    )
{
    NEXUS_AudioOutputData *pData;

    BDBG_ASSERT(NULL != output);

    pData = output->pMixerData;
    if ( pData )
    {
        NEXUS_Error rc;

        BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

        rc = NEXUS_AudioOutput_RemoveAllInputs(output);
        if (rc) {
            /* we cannot complete the shutdown */
            return;
        }

        BDBG_OBJECT_DESTROY(pData, NEXUS_AudioOutputData);
        BKNI_Free(pData);
    }
    output->pMixerData = NULL;

    return ;
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
NEXUS_Error NEXUS_AudioOutput_P_AdjustRapSettings(
    NEXUS_AudioOutput output,
    BRAP_OutputSettings *pSettings    /* [in/out] */
    )
{
    BDBG_ASSERT(NULL != output);
    BDBG_ASSERT(NULL != pSettings);

    switch ( output->objectType )
    {
    case NEXUS_AudioOutputType_eDac:
        return NEXUS_AudioDac_P_AdjustRapSettings(output->pObjectHandle, pSettings);
#if NEXUS_NUM_I2S_OUTPUTS
    case NEXUS_AudioOutputType_eI2s:
        return NEXUS_I2sOutput_P_AdjustRapSettings(output->pObjectHandle, pSettings);
#endif
#if NEXUS_NUM_RFM_OUTPUTS
    case NEXUS_AudioOutputType_eRfm:
        /* This is a slave output, nothing to do */
        return BERR_SUCCESS;
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    case NEXUS_AudioOutputType_eHdmi:
        {
            NEXUS_HdmiOutputSettings hdmiSettings;
            NEXUS_HdmiOutput_GetSettings(output->pObjectHandle, &hdmiSettings);
            pSettings->sSpdifChanStatusParams.bProfessionalMode = hdmiSettings.audioChannelStatusInfo.professionalMode;
            pSettings->sSpdifChanStatusParams.bSwCopyRight = hdmiSettings.audioChannelStatusInfo.swCopyRight;
            pSettings->sSpdifChanStatusParams.ui16CategoryCode = hdmiSettings.audioChannelStatusInfo.categoryCode;
            pSettings->sSpdifChanStatusParams.ui16ClockAccuracy = hdmiSettings.audioChannelStatusInfo.clockAccuracy;
            pSettings->sSpdifChanStatusParams.bSeparateLRChanNum = hdmiSettings.audioChannelStatusInfo.separateLRChanNum;
            return BERR_SUCCESS;
        }
#endif
#if NEXUS_NUM_SPDIF_OUTPUTS
    case NEXUS_AudioOutputType_eSpdif:
        return NEXUS_SpdifOutput_P_AdjustRapSettings(output->pObjectHandle, pSettings);
#endif
    default:
        BDBG_ERR(("Unsupported audio output type %d", output->objectType));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    return BERR_SUCCESS;    /* Never gets here */
}

/***************************************************************************
Summary:
	Determine if an output must be tied to a DAC
Description:
    Some outputs (i.e. RFM) are slaves to a DAC.  Mixer will find the proper
    DAC to bind the output to and provide it here.
 ***************************************************************************/
bool NEXUS_AudioOutput_P_IsDacSlave(
    NEXUS_AudioOutput output
    )
{
    BDBG_ASSERT(NULL != output);
    return (output->objectType == NEXUS_AudioOutputType_eRfm) ? true : false;
}

/***************************************************************************
Summary:
    For slave outputs, set the DAC source
Description:
    Some outputs (i.e. RFM) are slaves to a DAC.  Mixer will find the proper
    DAC to bind the output to and provide it here.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SetSlaveSource(
    NEXUS_AudioOutput slaveHandle,
    NEXUS_AudioOutput sourceHandle)
{
    #if NEXUS_NUM_RFM_OUTPUTS
    NEXUS_RfmConnectionSettings rfmConnectionSettings;
    bool enabled=false;
    NEXUS_Error errCode;

    BDBG_ASSERT(NULL != slaveHandle);
    BDBG_ASSERT(true == NEXUS_AudioOutput_P_IsDacSlave(slaveHandle));

    if ( NULL == g_NEXUS_audioModuleData.moduleSettings.modules.rfm )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( NULL != sourceHandle )
    {
        BDBG_ASSERT(sourceHandle->objectType == NEXUS_AudioOutputType_eDac);
        enabled = true;

        #if NEXUS_NUM_AUDIO_DACS > 1
        /* If we have more than one DAC, make sure RAP selects the proper source to the mux */
        errCode = BRAP_OP_SetRfmodSource(g_NEXUS_audioModuleData.hRap, sourceHandle->port);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        #endif
    }

    /* Tell RFM to mute or unmute appropriately */
    NEXUS_Module_Lock(g_NEXUS_audioModuleData.moduleSettings.modules.rfm);
    NEXUS_Rfm_GetConnectionSettings_priv(slaveHandle->pObjectHandle, &rfmConnectionSettings);
    rfmConnectionSettings.audioEnabled = enabled;
    errCode = NEXUS_Rfm_SetConnectionSettings_priv(slaveHandle->pObjectHandle, &rfmConnectionSettings);
    NEXUS_Module_Unlock(g_NEXUS_audioModuleData.moduleSettings.modules.rfm);

    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    #else

    /* No RFM */
    BSTD_UNUSED(slaveHandle);
    BSTD_UNUSED(sourceHandle);

    #endif

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Set audio parameters for an HDMI output
Description:
    Set number of audio channels and compressed / PCM.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SetHDMI(
    NEXUS_AudioOutput output,
    NEXUS_AudioInputFormat format
    )
{
    int numberOfChannels=0;
    BAVC_AudioFormat HDMIformat;

    BDBG_ASSERT(NULL != output);

    if (output->objectType == NEXUS_AudioOutputType_eHdmi)
    {
        switch(format)
        {
        case NEXUS_AudioInputFormat_eNone:
            BDBG_MSG(("HDMI audio format not set"));
            return BERR_SUCCESS;
        case NEXUS_AudioInputFormat_eCompressed:
            numberOfChannels=2; /* number of channels */
            HDMIformat = BAVC_AudioFormat_eAC3;
            break;
        case NEXUS_AudioInputFormat_ePcmStereo:
            numberOfChannels=2; /* number of channels */
            HDMIformat = BAVC_AudioFormat_ePCM;
            break;
        case NEXUS_AudioInputFormat_ePcm5_1:
            HDMIformat = BAVC_AudioFormat_ePCM;
            numberOfChannels=6; /* number if channels */
            break;
        case NEXUS_AudioInputFormat_ePcm7_1: /* Not supported in 7400 */
            /* fall through */
        default:
            BDBG_ERR(("Unknown format %d",format));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        BDBG_MSG(("Setting HDMI format %s number of channnels %d",
                  (HDMIformat == BAVC_AudioFormat_eAC3)? "compressed":"PCM",numberOfChannels));

        NEXUS_Module_Lock(g_NEXUS_audioModuleData.moduleSettings.modules.hdmiOutput);
        {
            BAVC_AudioSamplingRate sampleRate = BAVC_AudioSamplingRate_e48k;
            NEXUS_AudioOutputData *pData = output->pMixerData;
            if ( pData )
            {
				if (pData->sampleRate == BAVC_AudioSamplingRate_eUnknown)
				{
					sampleRate = NEXUS_AudioModule_P_SampleRate2Avc(pData->settings.defaultSampleRate);
				}
				else
				{
					sampleRate = pData->sampleRate;
				}
            }
			BDBG_MSG(("Setting HDMI sample rate %d",sampleRate));
            /* Eventually, audio decoder will need to set the proper compressed format */
            NEXUS_HdmiOutput_SetAudioParams_priv(output->pObjectHandle, BAVC_AudioBits_e16, sampleRate,
                                                 HDMIformat,numberOfChannels);
        }
        NEXUS_Module_Unlock(g_NEXUS_audioModuleData.moduleSettings.modules.hdmiOutput);
    }

    return BERR_SUCCESS;
}

static NEXUS_AudioOutputData *NEXUS_AudioOutput_P_CreateData(NEXUS_AudioOutput output)
{
    NEXUS_AudioOutputData *pData;
    if ( NULL == output->pMixerData )
    {
        output->pMixerData = pData = BKNI_Malloc(sizeof(NEXUS_AudioOutputData));
        if ( pData )
        {
            BKNI_Memset(pData, 0, sizeof(NEXUS_AudioOutputData));
            BDBG_OBJECT_SET(pData, NEXUS_AudioOutputData);
            NEXUS_AudioOutput_GetDefaultSettings(&pData->settings);

			pData->sampleRate = BAVC_AudioSamplingRate_eUnknown;

            switch ( output->objectType )
            {
            #if NEXUS_NUM_HDMI_OUTPUTS
            case NEXUS_AudioOutputType_eHdmi:
                {
                    BERR_Code errCode;
                    BRAP_OutputSettings outputSettings;

                    BDBG_MSG(("%s() initializing HDMI output",__FUNCTION__));
                    /* Must manually "open" hdmi inside the audio module.  This is as good a place as any to do that, it will be done only once. */
                    errCode = BRAP_GetOutputDefaultConfig(g_NEXUS_audioModuleData.hRap, output->port, &outputSettings);
                    /* This should never fail */
                    BDBG_ASSERT(BERR_SUCCESS == errCode);
                    /* Fill up output settings -- default to PCM */
                    outputSettings.eOutput = output->port;
                    outputSettings.uOutputPortSettings.sMaiSettings.eMaiMuxSelector = BRAP_OutputPort_eFlex;
#if BCHP_CHIP != 7401 && BCHP_CHIP != 7403
                    outputSettings.uOutputPortSettings.sMaiSettings.eMaiAudioFormat = BRAP_OP_MaiAudioFormat_eSpdif2Channel;
#else
		    outputSettings.uOutputPortSettings.sMaiSettings.eMaiAudioFormat = BRAP_OP_MaiAudioFormat_ePcmStereo;
#endif
                    errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &outputSettings);
                    /* This should never fail */
                    BDBG_ASSERT(BERR_SUCCESS == errCode);

                    errCode = BRAP_GetOutputDefaultConfig(g_NEXUS_audioModuleData.hRap, BRAP_OutputPort_eFlex, &outputSettings);
                    /* This should never fail */
                    BDBG_ASSERT(BERR_SUCCESS == errCode);
                    /* Fill up output settings -- default to PCM */
                    outputSettings.eOutput = BRAP_OutputPort_eFlex;
                    outputSettings.uOutputPortSettings.sFlexSettings.ePll = BRAP_OP_Pll_ePll0;
                    errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &outputSettings);
                    /* This should never fail */
                    BDBG_ASSERT(BERR_SUCCESS == errCode);

                    /* I2S ports are used for HDMI multichannel PCM*/
                    errCode = BRAP_GetOutputDefaultConfig  (g_NEXUS_audioModuleData.hRap,BRAP_OutputPort_eI2s0,&outputSettings);
                    BDBG_ASSERT(BERR_SUCCESS == errCode);

                    /* Fill up output settings */
                    outputSettings.eOutput = BRAP_OutputPort_eI2s0;
                    outputSettings.uOutputPortSettings.sI2sSettings.bAlignedToLRClk = false;
                    outputSettings.uOutputPortSettings.sI2sSettings.ePll = BRAP_OP_Pll_ePll0;  
                    errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &outputSettings);
                    /* This should never fail */
                    BDBG_ASSERT(BERR_SUCCESS == errCode);

#if NEXUS_NUM_I2S_OUTPUTS > 1
                    errCode = BRAP_GetOutputDefaultConfig  (g_NEXUS_audioModuleData.hRap,BRAP_OutputPort_eI2s1,&outputSettings);
                    BDBG_ASSERT(BERR_SUCCESS == errCode);

                    /* Fill up output settings */
                    outputSettings.eOutput = BRAP_OutputPort_eI2s1;
                    outputSettings.uOutputPortSettings.sI2sSettings.bAlignedToLRClk = false;
                    outputSettings.uOutputPortSettings.sI2sSettings.ePll = BRAP_OP_Pll_ePll0;  
                    errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &outputSettings);
                    /* This should never fail */
                    BDBG_ASSERT(BERR_SUCCESS == errCode);
#endif

#if NEXUS_NUM_I2S_OUTPUTS > 2
                    errCode = BRAP_GetOutputDefaultConfig  (g_NEXUS_audioModuleData.hRap,BRAP_OutputPort_eI2s2,&outputSettings);
                    BDBG_ASSERT(BERR_SUCCESS == errCode);

                    /* Fill up output settings */
                    outputSettings.eOutput = BRAP_OutputPort_eI2s2;
                    outputSettings.uOutputPortSettings.sI2sSettings.bAlignedToLRClk = false;
                    outputSettings.uOutputPortSettings.sI2sSettings.ePll = BRAP_OP_Pll_ePll0;  
                    errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &outputSettings);
                    /* This should never fail */
                    BDBG_ASSERT(BERR_SUCCESS == errCode);
#endif
                }
                break;
            #endif
            #if NEXUS_NUM_SPDIF_OUTPUTS
            case NEXUS_AudioOutputType_eSpdif:
                {
                    BERR_Code errCode;
                    BRAP_OutputSettings outputSettings;

                    BDBG_MSG(("%s() initializing SPDIF output PLL",__FUNCTION__));
                    errCode = BRAP_GetOutputDefaultConfig(g_NEXUS_audioModuleData.hRap, output->port, &outputSettings);
                    /* This should never fail */
                    BDBG_ASSERT(BERR_SUCCESS == errCode);
                    /* Fill up output settings -- default to PCM */
                    outputSettings.eOutput = output->port;
                    outputSettings.uOutputPortSettings.sSpdifSettings.ePll = BRAP_OP_Pll_ePll0;  
                    errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &outputSettings);
                    /* This should never fail */
                    BDBG_ASSERT(BERR_SUCCESS == errCode);
                }
                break;
            #endif
            default:
                break;
            }
        }
        else
        {
            NEXUS_Error errCode;
            errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            return NULL;
        }
    }

    return output->pMixerData;
}

void NEXUS_AudioOutput_GetSyncSettings_priv( NEXUS_AudioOutput audioOutput, NEXUS_AudioOutputSyncSettings *pSyncSettings )
{
    NEXUS_AudioOutputData *pData = audioOutput->pMixerData;
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
    *pSyncSettings = pData->syncSettings;
}

NEXUS_Error NEXUS_AudioOutput_SetSyncSettings_priv( NEXUS_AudioOutput audioOutput, const NEXUS_AudioOutputSyncSettings *pSyncSettings )
{
    NEXUS_AudioOutputData *pData = audioOutput->pMixerData;
    BRAP_OutputSettings outputSettings;
    NEXUS_Error errCode;

    NEXUS_ASSERT_MODULE();
    if (!pData) return 0; /* TODO */

    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
    pData->syncSettings = *pSyncSettings;

    BKNI_EnterCriticalSection();
    /* fire right away. always "on". */
    if (pSyncSettings->startCallback_isr) {
        (*pSyncSettings->startCallback_isr)(pSyncSettings->callbackContext, 0);
    }
    BKNI_LeaveCriticalSection();

    /* set delay + additionalDelay */
    if ( g_NEXUS_audioModuleData.moduleSettings.independentDelay )
    {
        /* set independent delay */
        errCode = BRAP_GetOutputConfig(g_NEXUS_audioModuleData.hRap, audioOutput->port, &outputSettings);
        if ( errCode ) return BERR_TRACE(errCode);
        outputSettings.iDelay = pData->status.delay = pData->settings.additionalDelay + pData->syncSettings.delay;
        errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &outputSettings);
        if ( errCode ) return BERR_TRACE(errCode);
    }
    else if ( pData->settings.additionalDelay || pData->syncSettings.delay )
    {
        BDBG_ERR(("Independent output delay is not enabled, ignoring delay.  Please check NEXUS_AudioModuleSettings.independentDelay"));
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_AudioOutput_GetSyncStatus_isr( NEXUS_AudioOutput audioOutput, NEXUS_AudioOutputSyncStatus *pSyncStatus )
{

    BDBG_ASSERT(NULL != audioOutput);
    BDBG_ASSERT(NULL != pSyncStatus);

    /* Default to PCM */
    pSyncStatus->compressed = false;

    /* Look for connected input.  If found, determine format from AudioInput. */
    if ( NULL != audioOutput->pMixerData )
    {
        NEXUS_AudioOutputData *pData;

        pData = audioOutput->pMixerData;
        BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
        if ( pData->input != NULL )
        {
            NEXUS_AudioInputFormat format = NEXUS_AudioInput_P_GetFormat(pData->input);

            if ( format == NEXUS_AudioInputFormat_eCompressed )
            {
                pSyncStatus->compressed = true;
            }
        }
    }

    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    Inform an audio output of a sample rate change
Description:
    Certain outputs such as HDMI require notification of sample rate changes
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SampleRateChange_isr(
    NEXUS_AudioOutput output,
    BAVC_AudioSamplingRate sampleRate
    )
{
    BDBG_ASSERT(NULL != output);

    #if NEXUS_NUM_HDMI_OUTPUTS
    if ( output->objectType == NEXUS_AudioOutputType_eHdmi )
    {
		NEXUS_AudioOutputData *pData = output->pMixerData;
        pData->sampleRate = sampleRate;
        if ( sampleRate == BAVC_AudioSamplingRate_eUnknown )
        {
            /* We're reverting to default */
            sampleRate = NEXUS_AudioModule_P_SampleRate2Avc(pData->settings.defaultSampleRate);
        }
		BDBG_MSG(("%s(%d)",__FUNCTION__,sampleRate));
        NEXUS_HdmiOutput_AudioSampleRateChange_isr(output->pObjectHandle, sampleRate);
    }
    #else
    BSTD_UNUSED(output);
    BSTD_UNUSED(sampleRate);
    #endif

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_AudioOutput_GetStatus( NEXUS_AudioOutput audioOutput, NEXUS_AudioOutputStatus *pStatus )
{
    BSTD_UNUSED(audioOutput);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

void NEXUS_AudioOutput_P_SetVcxo(
    unsigned outputTimebase,
    NEXUS_AudioOutputPll pll
    )
{
    uint32_t reg;
    uint32_t addr;

    BREG_Handle hRegister = g_pCoreHandles->reg;

    /* Hack since we lack a PI for setting Vcxo */
    /* set the VCXO Timebase for Rate Manager 0 or 1, selected by pll */
    /* this is the way it was done in native Settop API implementation. */
#if BCHP_CHIP == 7400
	addr = (pll == BRAP_OP_Pll_ePll0)?BCHP_VCXO_0_RM_CONTROL:BCHP_VCXO_1_RM_CONTROL;
#else
    addr = BCHP_VCXO_0_RM_CONTROL;
#endif

    BKNI_EnterCriticalSection();
    reg = BREG_Read32(hRegister, addr);
    reg &= ~(BCHP_MASK(VCXO_0_RM_CONTROL,TIMEBASE));
    reg |= BCHP_FIELD_DATA(VCXO_0_RM_CONTROL,TIMEBASE, outputTimebase);
    BREG_Write32(hRegister, addr, reg);
    BKNI_LeaveCriticalSection();
    BDBG_MSG(("%s: Set Rate Manager %d's VCXO to sync with Timebase %d", __FUNCTION__, 
              (pll == BRAP_OP_Pll_ePll0)? 0:1, outputTimebase));
}

NEXUS_Error NEXUS_AudioOutput_P_ApplySettings(
    NEXUS_AudioOutput output
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioOutputData *pData;
    NEXUS_AudioOutputSettings *pSettings;
	NEXUS_AudioOutputSyncStatus syncStatus;
    bool canSetVolume=true;

    BDBG_ASSERT(NULL != output);
    pData = output->pMixerData;
    if ( NULL == pData )
    {
        /* Nothing to do */
        return BERR_SUCCESS;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    BDBG_MSG(("Applying audio output settings"));

    pSettings = &pData->settings;
    if ( !pData->settingsChanged )
    {
        return BERR_SUCCESS;    /* Nothing to do */
    }
    pData->settingsChanged = false;

    if ( NEXUS_AudioOutput_P_IsDacSlave(output) )
    {
        /* TODO: Apply RFM mute here? */
        return BERR_SUCCESS;
    }

    /* We can mute compressed and PCM ports */
    BDBG_MSG(("Setting output mute to %d", pSettings->muted || pData->trickMute));

    errCode = BRAP_OP_SetMute(g_NEXUS_audioModuleData.hRap,
                                 output->port,
                                 pSettings->muted || pData->trickMute);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

	BKNI_EnterCriticalSection();
	NEXUS_AudioOutput_GetSyncStatus_isr( output, &syncStatus );
	BKNI_LeaveCriticalSection();

	canSetVolume = ( (( output->port != BRAP_OutputPort_eSpdif ) && ( output->port != BRAP_OutputPort_eMai ) &&
	  ( output->port != BRAP_OutputPort_eFlex )) || !syncStatus.compressed );

    /* Volume must be applied to only PCM outputs */
    if ( canSetVolume )
    {
#ifndef NEXUS_CAS
        BDBG_MSG(("Setting destination volume (port %d) (type %d) to 0x%x 0x%x", 
                  output->port,
                  pSettings->volumeType,
                  NEXUS_AudioModule_P_Vol2Magnum(pSettings->volumeType, pSettings->leftVolume),
                  NEXUS_AudioModule_P_Vol2Magnum(pSettings->volumeType, pSettings->rightVolume)));
        errCode = BRAP_MIXER_SetOutputVolume(g_NEXUS_audioModuleData.hRap,
                                             output->port,
                                             NEXUS_AudioModule_P_Vol2Magnum(pSettings->volumeType, pSettings->leftVolume),
                                             NEXUS_AudioModule_P_Vol2Magnum(pSettings->volumeType, pSettings->rightVolume));
#endif /* NEXUS_CAS */

        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    else
    {
        BDBG_MSG(("Can't set volume on port %d", output->port));
    }

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_AudioOutput_P_ApplyConfig(
    NEXUS_AudioOutput output
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioOutputData *pData;
    NEXUS_AudioOutputSettings *pSettings;
    BRAP_OutputSettings outputSettings, oldSettings;

	BDBG_ASSERT(NULL != output);
    pData = output->pMixerData;
    if ( NULL == pData )
    {
        /* Nothing to do */
        return BERR_SUCCESS;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    BDBG_MSG(("Applying audio output configuration"));

    pSettings = &pData->settings;
    if ( !pData->settingsChanged )
    {
        return BERR_SUCCESS;    /* Nothing to do */
    }
    pData->settingsChanged = false;

    if ( NEXUS_AudioOutput_P_IsDacSlave(output) )
    {
        /* TODO: Apply RFM mute here? */
        return BERR_SUCCESS;
    }

    if ( output->objectType != NEXUS_AudioOutputType_eCapture )
    {
        BRAP_OP_ChannelOrder channelOrder;

        /* Apply remaining settings to raptor based on compressed vs. pcm */
        errCode = BRAP_GetOutputConfig(g_NEXUS_audioModuleData.hRap, output->port, &outputSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        oldSettings = outputSettings;

        switch ( pSettings->channelMode )
        {
            case NEXUS_AudioChannelMode_eStereo:
            channelOrder = BRAP_OP_ChannelOrder_eNormal;
                break;
            case NEXUS_AudioChannelMode_eLeft:
            channelOrder = BRAP_OP_ChannelOrder_eBothLeft;
                break;
            case NEXUS_AudioChannelMode_eRight:
            channelOrder = BRAP_OP_ChannelOrder_eBothRight;
                break;
            case NEXUS_AudioChannelMode_eSwapped:
            channelOrder = BRAP_OP_ChannelOrder_eSwap;
                break;
            default:
                BDBG_ERR(("Invalid Channel Order"));
                return BERR_TRACE(BERR_INVALID_PARAMETER);
        }

        switch ( output->objectType )
        {
        case NEXUS_AudioOutputType_eDac:
            outputSettings.uOutputPortSettings.sDacSettings.eChannelOrder = channelOrder;
            BDBG_MSG(("%s() DAC %d channelOrder = %d",__FUNCTION__,output->port,channelOrder));
            break;
        case NEXUS_AudioOutputType_eI2s:
            outputSettings.uOutputPortSettings.sI2sSettings.eChannelOrder = channelOrder;
            BDBG_MSG(("%s() I2S %d channelOrder = %d",__FUNCTION__,output->port,channelOrder));
            outputSettings.uOutputPortSettings.sI2sSettings.ePll = pSettings->pll;
            BDBG_MSG(("%s() I2S %d pll = %d",__FUNCTION__,output->port,pSettings->pll));
            break;
        case NEXUS_AudioOutputType_eHdmi:
            outputSettings.uOutputPortSettings.sMaiSettings.eChannelOrder = channelOrder;
            BDBG_MSG(("sMaiSettings.eMaiAudioFormat = %x",outputSettings.uOutputPortSettings.sMaiSettings.eMaiAudioFormat));
            BDBG_MSG(("sMaiSettings.eMaiMuxSelector = %d",outputSettings.uOutputPortSettings.sMaiSettings.eMaiMuxSelector));
            BKNI_EnterCriticalSection();
            /* Update infoframe with new sample rate if it changed */
            if (( pData->sampleRate == BAVC_AudioSamplingRate_eUnknown ) && 0)
            {
                /* This will force the new default sample rate to HDMI */
                NEXUS_AudioOutput_P_SampleRateChange_isr(output, BAVC_AudioSamplingRate_eUnknown);
            }
            BKNI_LeaveCriticalSection();
            /* HDMI needs to also handle the multichannel I2S output PLL and sample rate */
            {
                BRAP_OutputSettings i2sSettings;
                BRAP_GetOutputConfig(g_NEXUS_audioModuleData.hRap, BRAP_OutputPort_eFlex, &i2sSettings);
                i2sSettings.uOutputPortSettings.sFlexSettings.ePll = pSettings->pll;
                BDBG_MSG(("%s() flex port %d pll = %d",__FUNCTION__,output->port,pSettings->pll));
                i2sSettings.eOutputSR = NEXUS_AudioModule_P_SampleRate2Avc(pSettings->defaultSampleRate);
                errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &i2sSettings);
                if (errCode) {
                    return BERR_TRACE(errCode);
                }
                BRAP_GetOutputConfig(g_NEXUS_audioModuleData.hRap, BRAP_OutputPort_eI2s0, &i2sSettings);
                i2sSettings.uOutputPortSettings.sI2sSettings.ePll = pSettings->pll;
                BDBG_MSG(("%s() I2S0 pll = %d",__FUNCTION__,pSettings->pll));
                i2sSettings.eOutputSR = NEXUS_AudioModule_P_SampleRate2Avc(pSettings->defaultSampleRate);
                errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &i2sSettings);
                if (errCode) {
                    return BERR_TRACE(errCode);
                }
#if NEXUS_NUM_I2S_OUTPUTS > 1
                BRAP_GetOutputConfig(g_NEXUS_audioModuleData.hRap, BRAP_OutputPort_eI2s1, &i2sSettings);
                i2sSettings.uOutputPortSettings.sI2sSettings.ePll = pSettings->pll;
                BDBG_MSG(("%s() I2S1 pll = %d",__FUNCTION__,pSettings->pll));
                i2sSettings.eOutputSR = NEXUS_AudioModule_P_SampleRate2Avc(pSettings->defaultSampleRate);
                errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &i2sSettings);
                if (errCode) {
                    return BERR_TRACE(errCode);
                }
#endif

#if NEXUS_NUM_I2S_OUTPUTS > 2
                BRAP_GetOutputConfig(g_NEXUS_audioModuleData.hRap, BRAP_OutputPort_eI2s2, &i2sSettings);
                i2sSettings.uOutputPortSettings.sI2sSettings.ePll = pSettings->pll;
                BDBG_MSG(("%s() I2S2 pll = %d",__FUNCTION__,pSettings->pll));
                i2sSettings.eOutputSR = NEXUS_AudioModule_P_SampleRate2Avc(pSettings->defaultSampleRate);
                errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &i2sSettings);
                if (errCode) {
                    return BERR_TRACE(errCode);
                }
#endif
            }
            break;
        case NEXUS_AudioOutputType_eSpdif:
            outputSettings.uOutputPortSettings.sSpdifSettings.eChannelOrder = channelOrder;
            outputSettings.uOutputPortSettings.sSpdifSettings.ePll = pSettings->pll;
            BDBG_MSG(("%s() SPDIF %d pll = %d",__FUNCTION__,output->port,pSettings->pll));

            BDBG_MSG(("%s() SPDIF %d channelOrder = %d",__FUNCTION__,output->port,channelOrder));
            break;
        default:
            break;
        }

		BDBG_MSG(("%s() output->objectType = %d outputSettings.uOutputPortSettings.sMaiSettings.eMaiAudioFormat = %02x",
				  __FUNCTION__,output->objectType,outputSettings.uOutputPortSettings.sMaiSettings.eMaiAudioFormat));
#if BCHP_CHIP != 7401 && BCHP_CHIP != 7403
		if ((output->objectType==NEXUS_AudioOutputType_eHdmi) && 
			(outputSettings.uOutputPortSettings.sMaiSettings.eMaiAudioFormat == BRAP_OP_MaiAudioFormat_eSpdif6Channel))
		{
			/* no independent delay, for multichannel HDMI */
			outputSettings.iDelay = 0;
			outputSettings.bIndpDlyRqd = false;
		}
		else
#endif
		{
			if ( g_NEXUS_audioModuleData.moduleSettings.independentDelay )
			{
				/* set independent delay, but not for multichannel HDMI */
				BDBG_MSG(("Setting independent delay to %d", pSettings->additionalDelay + pData->syncSettings.delay));
				outputSettings.iDelay = pData->status.delay = pSettings->additionalDelay + pData->syncSettings.delay;
				outputSettings.bIndpDlyRqd = (outputSettings.iDelay != 0);
			}
			else if ( pSettings->additionalDelay )
			{
				BDBG_ERR(("Independent output delay is not enabled, ignoring delay.  Please check NEXUS_AudioModuleSettings.independentDelay"));
			}
		}

        /* update the timebase and sample rate */
        outputSettings.eOutputTimebase = pSettings->timebase;
        BDBG_MSG(("outputSettings.eOutputTimebase = %d",outputSettings.eOutputTimebase));
        outputSettings.eOutputSR = NEXUS_AudioModule_P_SampleRate2Avc(pSettings->defaultSampleRate);

        /* Only apply the settings if something has actually changed.  Otherwise, the port may mute. */
        if ( BKNI_Memcmp(&oldSettings, &outputSettings, sizeof(outputSettings)) )
        {
            errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap,
                                           &outputSettings);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }

    /* update the timebase for VCXO as well */
    NEXUS_AudioOutput_P_SetVcxo(pSettings->timebase,pSettings->pll);


    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_AudioOutput_P_ZeroDelay(
    NEXUS_AudioOutput output
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioOutputData *pData;
    BRAP_OutputSettings outputSettings;

	BDBG_ASSERT(NULL != output);
    pData = output->pMixerData;
    if ( NULL == pData )
    {
        /* Nothing to do */
        return BERR_SUCCESS;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    if ( NEXUS_AudioOutput_P_IsDacSlave(output) )
    {
        /* TODO: Apply RFM mute here? */
        return BERR_SUCCESS;
    }

    if ( output->objectType != NEXUS_AudioOutputType_eCapture )
    {
        /* Apply remaining settings to raptor based on compressed vs. pcm */
        errCode = BRAP_GetOutputConfig(g_NEXUS_audioModuleData.hRap, output->port, &outputSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
		if ( g_NEXUS_audioModuleData.moduleSettings.independentDelay )
		{
			/* set independent delay */
			BDBG_MSG(("Setting independent delay to 0"));
			outputSettings.iDelay = 0;
			outputSettings.bIndpDlyRqd = false;
		}
		errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap,
									   &outputSettings);
		if ( errCode )
		{
			return BERR_TRACE(errCode);
		}
    }

    return BERR_SUCCESS;
}

static NEXUS_Error NEXUS_AudioOutput_P_SetMute(
	NEXUS_AudioOutput output,
    NEXUS_AudioOutputData *pData
)
{
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    BDBG_MSG(("Setting output mute to %d", pData->settings.muted || pData->trickMute || pData->syncMute));

    errCode = BRAP_OP_SetMute(g_NEXUS_audioModuleData.hRap,
                                 output->port,
                                 pData->settings.muted || pData->trickMute || pData->syncMute);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return NEXUS_SUCCESS;
}


NEXUS_Error NEXUS_AudioOutput_P_SetSyncMute(
    NEXUS_AudioOutput output,
    bool mute
    )
{
    NEXUS_AudioOutputData *pData;
    NEXUS_Error errCode;

    if ( NEXUS_AudioOutput_P_IsDacSlave(output) )
    {
        return NEXUS_SUCCESS;
    }

    if ( NULL == output->pMixerData )
    {
        pData = NEXUS_AudioOutput_P_CreateData(output);
        if ( NULL == pData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }

    pData = output->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    pData->syncMute = mute;

    errCode = NEXUS_AudioOutput_P_SetMute(output,pData);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_AudioOutput_P_SetTrickMute(
    NEXUS_AudioOutput output,
    bool mute
    )
{
    NEXUS_AudioOutputData *pData;
    NEXUS_Error errCode;

    if ( NEXUS_AudioOutput_P_IsDacSlave(output) )
    {
        return NEXUS_SUCCESS;
    }

    if ( NULL == output->pMixerData )
    {
        pData = NEXUS_AudioOutput_P_CreateData(output);
        if ( NULL == pData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }

    pData = output->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    pData->trickMute = mute;

    errCode = NEXUS_AudioOutput_P_SetMute(output,pData);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    Determine if an output is connected to any inputs
 ***************************************************************************/
bool NEXUS_AudioOutput_P_IsConnected(
    NEXUS_AudioOutput output
    )
{
    NEXUS_AudioOutputData *pData;

    BDBG_ASSERT(NULL != output);

    pData = output->pMixerData;
    if ( NULL != pData && NULL != pData->input )
    {
        BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
        return true;
    }

    return false;
}

