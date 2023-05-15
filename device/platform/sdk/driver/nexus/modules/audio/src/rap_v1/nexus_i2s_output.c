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
* $brcm_Revision: 6 $
* $brcm_Date: 6/20/12 1:15p $
*
* API Description:
*   API name: I2sOutput
*    Specific APIs related to I2S audio outputs.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/src/nexus_i2s_output.c $
* 
* 6   6/20/12 1:15p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 5   8/18/11 6:13p jgarrett
* SWDTV-6306: Adding I2S Multichannel Stubs
* 
* 4   6/19/09 6:51p mward
* PR 47739: Coverity checked return.
* 
* 3   6/12/09 4:18p mward
* PR 49702: Initialize bAlignedToLRClk for I2S outputs.
* 
* 2   3/13/09 12:21p mward
* PR 51468: Renaming globals to match naming conventions
* 
* 1   1/18/08 2:18p jgarrett
* PR 38808: Merging to main branch
* 
* Nexus_Devel/1   12/6/07 2:17p jgarrett
* PR 37971: Adding I2sOutput
* 
***************************************************************************/

#include "nexus_audio_module.h"
#include "priv/nexus_core_audio.h"

BDBG_MODULE(nexus_i2s_output);

typedef struct NEXUS_I2sOutput
{
    NEXUS_OBJECT(NEXUS_I2sOutput);
    bool opened;
    NEXUS_I2sOutputSettings settings;
    NEXUS_AudioOutputObject connector;
} NEXUS_I2sOutput;

static NEXUS_I2sOutput g_i2sOutputs[NEXUS_NUM_I2S_OUTPUTS];

#define NEXUS_I2S_OUTPUT_PORT(i) ((i==0)?BRAP_OutputPort_eI2s0:(i==1)?BRAP_OutputPort_eI2s1:BRAP_OutputPort_eI2s2)

/***************************************************************************
Summary:
	Get default settings for an I2S output
See Also:

 ***************************************************************************/
void NEXUS_I2sOutput_GetDefaultSettings(
    NEXUS_I2sOutputSettings *pSettings   /* [out] default settings */
    )
{
    BRAP_OutputSettings outputSettings;

    BDBG_ASSERT(NULL != pSettings);

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    (void)BRAP_GetOutputDefaultConfig(g_NEXUS_audioModuleData.hRap, BRAP_OutputPort_eI2s0, &outputSettings);

    pSettings->lsbAtLRClock = outputSettings.uOutputPortSettings.sI2sSettings.bLsbAtLRClk;
    pSettings->alignedToLRClock = outputSettings.uOutputPortSettings.sI2sSettings.bAlignedToLRClk;
    pSettings->lrClkPolarity = outputSettings.uOutputPortSettings.sI2sSettings.bLRClkPolarity;
    pSettings->sclkRate = (outputSettings.uOutputPortSettings.sI2sSettings.eSClkRate == BRAP_OP_SClkRate_e64Fs)?NEXUS_I2sOutputSclkRate_e64Fs:NEXUS_I2sOutputSclkRate_e128Fs;
    switch ( outputSettings.uOutputPortSettings.sI2sSettings.eMClkRate )
    {
    case BRAP_OP_MClkRate_e128Fs:
        pSettings->mclkRate = NEXUS_I2sOutputMclkRate_e128Fs;
        break;
    default:
    case BRAP_OP_MClkRate_e256Fs:
        pSettings->mclkRate = NEXUS_I2sOutputMclkRate_e256Fs;
        break;
    case BRAP_OP_MClkRate_e384Fs:
        pSettings->mclkRate = NEXUS_I2sOutputMclkRate_e384Fs;
        break;
    case BRAP_OP_MClkRate_e512Fs:
        pSettings->mclkRate = NEXUS_I2sOutputMclkRate_e512Fs;
        break;
    }
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
    NEXUS_I2sOutput *pI2s;
    BRAP_OutputSettings outputSettings;

    if ( index >= NEXUS_NUM_I2S_OUTPUTS )
    {
        BDBG_ERR(("I2sOutput %u not supported on this chipset", index));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    pI2s = &g_i2sOutputs[index];
    if ( pI2s->opened )
    {
        BDBG_ERR(("I2sOutput %u is already open", index));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_I2sOutput, pI2s);
    pI2s->connector.port = NEXUS_I2S_OUTPUT_PORT(index);
    pI2s->connector.objectType = NEXUS_AudioOutputType_eI2s;
    pI2s->connector.pObjectHandle = pI2s;

    /* Set the port to defaults - this "opens" it from raptor */
    (void)BRAP_GetOutputDefaultConfig(g_NEXUS_audioModuleData.hRap, pI2s->connector.port, &outputSettings);
    outputSettings.uOutputPortSettings.sI2sSettings.bAlignedToLRClk = false;
    errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &outputSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        return NULL;
    }

    pI2s->opened = true;
    errCode = NEXUS_I2sOutput_SetSettings(pI2s, pSettings);   /* Handles NULL case */
    if ( errCode )
    {
        pI2s->opened = false;
        errCode = BERR_TRACE(errCode);
        return NULL;
    }

    /* success */
    return pI2s;
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
    NEXUS_Error errCode;

    NEXUS_OBJECT_ASSERT(NEXUS_I2sOutput, handle);
    BDBG_ASSERT(handle->opened);

    if ( handle->connector.pMixerData )
    {
        BDBG_ERR(("Output is still active.  Please call NEXUS_AudioOutput_Shutdown prior to closing."));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        return;
    }
    NEXUS_OBJECT_CLEAR(NEXUS_I2sOutput, handle);
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
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->opened);
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
    const NEXUS_I2sOutputSettings *pSettings
    )
{
    NEXUS_Error errCode;
    BRAP_OutputSettings outputSettings;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->opened);

    errCode = BRAP_GetOutputConfig(g_NEXUS_audioModuleData.hRap, handle->connector.port, &outputSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( NULL == pSettings )
    {
        /* Use defaults if NULL was passed */
        NEXUS_I2sOutput_GetDefaultSettings(&handle->settings);
    }
    else
    {
        /* Sanity check values if provided */
        if ( pSettings->sclkRate >= NEXUS_I2sOutputSclkRate_eMax ||
             pSettings->mclkRate >= NEXUS_I2sOutputMclkRate_eMax )
        {
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        handle->settings = *pSettings;
    }

    /* Convert to raptor values */
    outputSettings.uOutputPortSettings.sI2sSettings.bLsbAtLRClk = handle->settings.lsbAtLRClock;
    outputSettings.uOutputPortSettings.sI2sSettings.bAlignedToLRClk = handle->settings.alignedToLRClock;
    outputSettings.uOutputPortSettings.sI2sSettings.bLRClkPolarity = handle->settings.lrClkPolarity;
    outputSettings.uOutputPortSettings.sI2sSettings.eSClkRate = (handle->settings.sclkRate == NEXUS_I2sOutputSclkRate_e64Fs)?BRAP_OP_SClkRate_e64Fs:BRAP_OP_SClkRate_e128Fs;
    switch ( handle->settings.mclkRate )
    {
    case NEXUS_I2sOutputMclkRate_e128Fs:
        outputSettings.uOutputPortSettings.sI2sSettings.eMClkRate = BRAP_OP_MClkRate_e128Fs;
        break;
    default:
    case NEXUS_I2sOutputMclkRate_e256Fs:
        outputSettings.uOutputPortSettings.sI2sSettings.eMClkRate = BRAP_OP_MClkRate_e256Fs;
        break;
    case NEXUS_I2sOutputMclkRate_e384Fs:
        outputSettings.uOutputPortSettings.sI2sSettings.eMClkRate = BRAP_OP_MClkRate_e384Fs;
        break;
    case NEXUS_I2sOutputMclkRate_e512Fs:
        outputSettings.uOutputPortSettings.sI2sSettings.eMClkRate = BRAP_OP_MClkRate_e512Fs;
        break;
    }

    /* Set in raptor */
    errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap, &outputSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

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
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->opened);

    return &handle->connector;
}

/***************************************************************************
Summary:
	Apply output-specific settings to the raptor structure
Description:
    Mixer will setup the raptor output port settings initially and then
    hand this structure off to the individual outputs to make their
    adjustments.  It requires knowledge of raptor structures, but no
    actual raptor function calls.  This is required for things like LR swap
    because some part of the logic is in a downmix mode and some is in the
    DAC settings.
 ***************************************************************************/
NEXUS_Error NEXUS_I2sOutput_P_AdjustRapSettings(
    NEXUS_I2sOutputHandle handle,
    BRAP_OutputSettings *pSettings    /* [in/out] */
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(NULL != pSettings);

    /* Everything is done at SetSettings time for I2S */

    return BERR_SUCCESS;
}

typedef struct NEXUS_I2sMultiOutput {
    NEXUS_OBJECT(NEXUS_I2sMultiOutput);
} NEXUS_I2sMultiOutput;

/***************************************************************************
Summary:
Get default settings for a Multichannel I2S output
***************************************************************************/
void NEXUS_I2sMultiOutput_GetDefaultSettings(
    NEXUS_I2sMultiOutputSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
Open a Multichannel I2S Output device
***************************************************************************/
NEXUS_I2sMultiOutputHandle NEXUS_I2sMultiOutput_Open( /* attr{destructor=NEXUS_I2sMultiOutput_Close}  */
    unsigned index,
    const NEXUS_I2sMultiOutputSettings *pSettings   /* attr{null_allowed=y} */
    )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}
    

/***************************************************************************
Summary:
Close a Multichannel I2S Output device
***************************************************************************/
static void NEXUS_I2sMultiOutput_P_Finalizer(
    NEXUS_I2sMultiOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_I2sMultiOutput, NEXUS_I2sMultiOutput_Close);

/***************************************************************************
Summary:
Get settings for a Multichannel I2S output
***************************************************************************/
void NEXUS_I2sMultiOutput_GetSettings(
    NEXUS_I2sMultiOutputHandle handle,
    NEXUS_I2sMultiOutputSettings *pSettings  /* [out] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
Set settings for a Multichannel I2S output
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
Get the audio connector for a multichannel I2S or the first stereo pair 
if set to stereo mode. 
***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sMultiOutput_GetConnector(
    NEXUS_I2sMultiOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}


/***************************************************************************
Summary:
Get the audio connector for an individual stereo pair on Multichannel I2S. 
Only supported if NEXUS_I2sMultiOutputSettings.mode is set to 
NEXUS_I2sMultiMode_eStereo. 
***************************************************************************/
NEXUS_AudioOutput NEXUS_I2sMultiOutput_GetStereoConnector(
    NEXUS_I2sMultiOutputHandle handle,
    NEXUS_AudioChannelPair channelPair
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(channelPair);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}
