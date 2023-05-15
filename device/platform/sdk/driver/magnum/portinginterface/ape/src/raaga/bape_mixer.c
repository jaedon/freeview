/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_mixer.c $
 * $brcm_Revision: Hydra_Software_Devel/43 $
 * $brcm_Date: 9/27/12 10:18a $
 *
 * Module Description: Mixer Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_mixer.c $
 * 
 * Hydra_Software_Devel/43   9/27/12 10:18a mpeteri
 * SW7445-19: merge 7445 APE changes to main
 * 
 * Hydra_Software_Devel/SW7445-19/2   9/25/12 4:12p mpeteri
 * SW7445-19: bump up BAPE_MclkSource_eMax to 12
 * 
 * Hydra_Software_Devel/SW7445-19/1   8/17/12 5:49p mpeteri
 * SW7445-19: Add additional PLL and NCO for 7445
 * 
 * Hydra_Software_Devel/42   6/15/12 4:25p jgarrett
 * SW7425-3252: Allowing mixer balance to be set on the fly for
 * certification
 * 
 * Hydra_Software_Devel/41   6/11/12 5:27p jgarrett
 * SW7425-3220: Resolving Coverity 6 issues
 * 
 * Hydra_Software_Devel/40   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/39   3/16/12 6:07p gskerl
 * SW7425-1178: Added code so that bape_startprint will print info about
 * InputPorts and RAVE contexts.
 * 
 * Hydra_Software_Devel/38   1/27/12 4:52p jgarrett
 * SW7429-55: Setting default NCO to 0 on chips that have an NCO
 * 
 * Hydra_Software_Devel/37   12/2/11 3:19p jgarrett
 * SW7346-578: Adding ability to change some mixer settings without
 * destroying and re-creating the mixer
 * 
 * Hydra_Software_Devel/36   8/23/11 1:23p gskerl
 * SW7425-1119: Added prototypes and stubs for BAPE_Mixer_Start() and
 * BAPE_Mixer_Stop(). For now, they just return BERR_NOT_SUPPORTED
 * 
 * Hydra_Software_Devel/35   7/8/11 4:26p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/34   6/29/11 4:07p gskerl
 * SW7425-321: Resolved the bape_mixerprint in kernel mode issue
 * 
 * Hydra_Software_Devel/33   6/29/11 11:27a gskerl
 * SW7425-321: Disabled bape_mixerprint when in kernel mode (because BDBG
 * level detection doesn't work in kernel mode)
 * 
 * Hydra_Software_Devel/32   6/20/11 7:02p jgarrett
 * SW7425-406: Adding input scaling coefficients to mixer
 * 
 * Hydra_Software_Devel/31   6/16/11 4:56p gskerl
 * SW7425-321: Changed diagnostic prints to use new BDBG_LOG macro
 * 
 * Hydra_Software_Devel/30   6/14/11 6:53p gskerl
 * SW7425-321: Added BAPE_Mixer_P_PrintMixers() function to print a mixer-
 * centric representation of the filter graph
 * 
 * Hydra_Software_Devel/29   6/3/11 6:41p jgarrett
 * SW7425-406: Adding BAPE_Mixer_GetConnector routines
 * 
 * Hydra_Software_Devel/28   5/31/11 6:37p jgarrett
 * SW7425-406: Adding initial DSP mixer code
 * 
 * Hydra_Software_Devel/27   5/27/11 7:48p gskerl
 * SW7425-321: Split mixer code into multiple files. Added framework for
 * dispatching mixer API calls to multiple mixer types.
 * 
 * Hydra_Software_Devel/26   5/26/11 5:38p jgarrett
 * SW7346-224: Making sure buffers are validated even if mixer path is
 * cached.  Removing other connections may have removed buffers.
 * 
 * Hydra_Software_Devel/25   5/20/11 5:16p jgarrett
 * SW7425-402: Adding error code to handle output enable callback failing.
 * 
 * Hydra_Software_Devel/24   5/13/11 3:45p jgarrett
 * SW7425-571: Resetting input sample rate when resources are released.
 * Fixes issues when enabling/disabling PPM correction at non-bypass
 * sample rates
 * 
 * Hydra_Software_Devel/23   4/27/11 3:11p jgarrett
 * SW7346-160: Making input format changes less restrictive while running
 * 
 * Hydra_Software_Devel/22   4/20/11 7:00p gskerl
 * SW7425-384: Refactored BAPE_P_SetFsTiming_isr() to improve PLLCLKSEL
 * logic and to add support for multiple DACS
 * 
 * Hydra_Software_Devel/21   4/18/11 10:09p jgarrett
 * SW7425-361: Refactoring DSP branch decisions
 * 
 * Hydra_Software_Devel/20   4/17/11 12:28p jgarrett
 * SW7344-46: Resetting connection state after resource release
 * 
 * Hydra_Software_Devel/19   4/16/11 12:32p jgarrett
 * SW7425-371: Removing tabs
 * 
 * Hydra_Software_Devel/18   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/17   4/14/11 6:18p jgarrett
 * SW7231-125: Fixing buffer allocation/free order on input format change
 * 
 * Hydra_Software_Devel/16   4/12/11 5:34p jgarrett
 * SW7425-292: Fixing input mute and preserving output mute across
 * start/stop
 * 
 * Hydra_Software_Devel/15   4/6/11 1:23a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:49p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/14   3/22/11 2:57p gskerl
 * SW7422-146: Changed audio output connector callbacks to take the output
 * connector as an argument
 * 
 * Hydra_Software_Devel/13   3/11/11 12:20p jgarrett
 * SW7422-146: Enabling PPM Correction
 * 
 * Hydra_Software_Devel/12   3/11/11 11:17a jgarrett
 * SW7422-146: Coverity CID 289
 * 
 * Hydra_Software_Devel/11   3/9/11 2:54p jgarrett
 * SW7422-146: Fixing SRC lockup with decode stop/start
 * 
 * Hydra_Software_Devel/10   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 * Hydra_Software_Devel/9   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/8   2/10/11 5:45p gskerl
 * SW7422-146: Changed calling sequence for the audio output setMclk_isr
 * callbacks
 * 
 * Hydra_Software_Devel/7   1/21/11 10:58a jgarrett
 * SW7422-146: Fixing second mixer port offset on 7422/7425
 * 
 * Hydra_Software_Devel/6   1/19/11 7:20p jgarrett
 * SW7422-146: Fixing volume register offset on 734x
 * 
 * Hydra_Software_Devel/5   1/19/11 2:58p jgarrett
 * SW7422-146: Initial decode/passthrough of ac3
 * 
 * Hydra_Software_Devel/4   1/13/11 2:26p jgarrett
 * SW7422-146: Adding 7346 APE support
 * 
 * Hydra_Software_Devel/3   1/10/11 5:19p jgarrett
 * SW7422-146: Adding ramp step APIs
 * 
 * Hydra_Software_Devel/2   12/17/10 3:58p jgarrett
 * SW7422-146: Nexus APE integration on 7422
 * 
 * Hydra_Software_Devel/1   12/16/10 4:05p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"

BDBG_MODULE(bape_mixer);

BDBG_OBJECT_ID(BAPE_Mixer);
BDBG_OBJECT_ID(BAPE_OutputPort);

/*************************************************************************/
/* Generic mixer functions (independent of mixer type)                   */
/*************************************************************************/
void BAPE_Mixer_GetDefaultSettings(
    BAPE_MixerSettings *pSettings   /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->defaultSampleRate = 48000;
    #if BAPE_CHIP_MAX_NCOS > 0
    pSettings->outputNco = BAPE_Nco_e0;
    #else
    pSettings->outputNco = BAPE_Nco_eMax;       /* eMax => Don't use an NCO  */
    #endif
}

/*************************************************************************/
void BAPE_Mixer_GetDefaultAddInputSettings(
    BAPE_MixerAddInputSettings *pSettings   /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

/*************************************************************************/
unsigned BAPE_Mixer_P_FindInputIndex(BAPE_MixerHandle handle, BAPE_Connector input)
{
    unsigned i;

    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
    {
        if ( handle->inputs[i] == input )
        {
            return i;
        }
    }

    return BAPE_MIXER_INPUT_INDEX_INVALID;
}

/*************************************************************************/
void BAPE_Mixer_P_GetDefaultInputVolume(BAPE_MixerInputVolume *pVolume)
{
    unsigned i;
    BKNI_Memset(pVolume, 0, sizeof(*pVolume));
    for ( i = 0; i < BAPE_Channel_eMax; i++ )
    {
        pVolume->coefficients[i][i] = BAPE_VOLUME_NORMAL;
    }
}

/*************************************************************************/
/* Some functions more related to outputs than mixers.                   */
/*************************************************************************/
BERR_Code BAPE_GetOutputVolume(
    BAPE_OutputPort output,
    BAPE_OutputVolume *pVolume      /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);
    BDBG_ASSERT(NULL != pVolume);
    
    *pVolume = output->volume;
    return BERR_SUCCESS;
}

/*************************************************************************/
BERR_Code BAPE_SetOutputVolume(
    BAPE_OutputPort output,
    const BAPE_OutputVolume *pVolume
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);
    BDBG_ASSERT(NULL != pVolume);

    output->volume = *pVolume;
    if ( output->mixer && output->mixer->running )
    {
        errCode = BAPE_Mixer_P_ApplyOutputVolume(output->mixer, output);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

/*************************************************************************/
void BAPE_GetOutputDelayStatus(
    BAPE_OutputPort output,
    BAPE_OutputDelayStatus *pStatus     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);
    BDBG_ASSERT(NULL != pStatus);

    pStatus->pathDelay = BAPE_CHIP_MAX_PATH_DELAY;
    pStatus->additionalDelay = (output->mixer)?output->mixer->settings.outputDelay:0;    
}   

/*************************************************************************/
/* Dispatchers for type-specific mixer functions                         */
/*************************************************************************/
BERR_Code BAPE_Mixer_Create(
    BAPE_Handle deviceHandle,
    const BAPE_MixerSettings *pSettings,
    BAPE_MixerHandle *pHandle               /* [out] */
    )
{
    BERR_Code errCode;
    BAPE_MixerSettings defaultSettings;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);

    *pHandle = NULL;

    if ( NULL == pSettings )
    {
        BAPE_Mixer_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    switch ( pSettings->type )
    {
    #if BAPE_CHIP_MAX_MIXERS
    case BAPE_MixerType_eStandard:
        errCode = BAPE_StandardMixer_P_Create( deviceHandle, pSettings, pHandle );
        break;
    #endif
        
    #if BAPE_CHIP_MAX_BYPASS_MIXERS
    case BAPE_MixerType_eBypass:
        errCode = BAPE_BypassMixer_P_Create( deviceHandle, pSettings, pHandle );
        break;
    #endif
        
    #if BAPE_CHIP_MAX_DSP_MIXERS
    case BAPE_MixerType_eDsp:
        errCode = BAPE_DspMixer_P_Create( deviceHandle, pSettings, pHandle );
        break;
    #endif
    
    default:
        BDBG_ERR(("MixerType %u is invalid or not supported", pSettings->type ));
        errCode = BERR_INVALID_PARAMETER;
        break;
    }

    return BERR_TRACE(errCode);
}

/*************************************************************************/
void BAPE_Mixer_Destroy(
    BAPE_MixerHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->destroy) {
        handle->interface->destroy(handle);
    } else {
        (void) BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/**************************************************************************/
void BAPE_Mixer_GetSettings(
    BAPE_MixerHandle hMixer,
    BAPE_MixerSettings *pSettings   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(hMixer, BAPE_Mixer);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = hMixer->settings;
}

/**************************************************************************/
BERR_Code BAPE_Mixer_SetSettings(
    BAPE_MixerHandle hMixer,
    const BAPE_MixerSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(hMixer, BAPE_Mixer);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != hMixer->interface);

    if ( hMixer->running && hMixer->settings.type != BAPE_MixerType_eDsp )
    {
        BDBG_ERR(("Cannot change mixer settings while running.  Please stop all inputs."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    if ( hMixer->settings.type != pSettings->type )
    {
        BDBG_ERR(("Mixer type cannot be changed on the fly."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( hMixer->interface->setSettings )
    {
        return hMixer->interface->setSettings(hMixer, pSettings);
    }
    else
    {   
        BDBG_ERR(("This mixer type does not support changing settings on the fly."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }   
}

/*************************************************************************/
BERR_Code BAPE_Mixer_Start(
    BAPE_MixerHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);
    
    if(handle->interface->start) {
        return handle->interface->start(handle);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/*************************************************************************/
void BAPE_Mixer_Stop(
    BAPE_MixerHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->stop) {
        handle->interface->stop(handle);
    } else {
        (void) BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/*************************************************************************/
BERR_Code BAPE_Mixer_AddInput(
    BAPE_MixerHandle handle,
    BAPE_Connector input,
    const BAPE_MixerAddInputSettings *pSettings     /* Optional, pass NULL for default settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->addInput) {
        return handle->interface->addInput(handle, input, pSettings);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/*************************************************************************/
BERR_Code BAPE_Mixer_RemoveInput(
    BAPE_MixerHandle handle,
    BAPE_Connector input
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->removeInput) {
        return handle->interface->removeInput(handle, input);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/*************************************************************************/
BERR_Code BAPE_Mixer_RemoveAllInputs(
    BAPE_MixerHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->removeAllInputs) {
        return handle->interface->removeAllInputs(handle);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/*************************************************************************/
BERR_Code BAPE_Mixer_AddOutput(
    BAPE_MixerHandle handle,
    BAPE_OutputPort output
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->addOutput) {
        return handle->interface->addOutput(handle, output);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/*************************************************************************/
BERR_Code BAPE_Mixer_RemoveOutput(
    BAPE_MixerHandle handle,
    BAPE_OutputPort output
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->removeOutput) {
        return handle->interface->removeOutput(handle, output);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/*************************************************************************/
BERR_Code BAPE_Mixer_RemoveAllOutputs(
    BAPE_MixerHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->removeAllOutputs) {
        return handle->interface->removeAllOutputs(handle);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/*************************************************************************/
BERR_Code BAPE_Mixer_GetInputVolume(
    BAPE_MixerHandle handle,
    BAPE_Connector input,
    BAPE_MixerInputVolume *pVolume      /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->getInputVolume) {
        return handle->interface->getInputVolume(handle, input, pVolume);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/*************************************************************************/
BERR_Code BAPE_Mixer_SetInputVolume(
    BAPE_MixerHandle handle,
    BAPE_Connector input,
    const BAPE_MixerInputVolume *pVolume
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->setInputVolume) {
        return handle->interface->setInputVolume(handle, input, pVolume);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/*************************************************************************/
BERR_Code BAPE_Mixer_P_ApplyOutputVolume(BAPE_MixerHandle handle, BAPE_OutputPort output)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != handle->interface);

    if(handle->interface->applyOutputVolume) {
        return handle->interface->applyOutputVolume(handle, output);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void BAPE_Mixer_GetConnector(
    BAPE_MixerHandle handle,
    BAPE_Connector *pConnector /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->pathNode.connectors[0];
}

/*************************************************************************/
const char *BAPE_Mixer_P_MclkSourceToText( BAPE_MclkSource mclkSource )
{
    BDBG_CASSERT((int)BAPE_MclkSource_eMax == (int)12);  /* This should cause compile error if somebody adds to the BAPE_MclkSource enum */

    return ( mclkSource == BAPE_MclkSource_eNone       ?  "None"      : 
             mclkSource == BAPE_MclkSource_ePll0       ?  "Pll0"      : 
             mclkSource == BAPE_MclkSource_ePll1       ?  "Pll1"      : 
             mclkSource == BAPE_MclkSource_ePll2       ?  "Pll2"      : 
             mclkSource == BAPE_MclkSource_eHifidac0   ?  "Hifidac0"  : 
             mclkSource == BAPE_MclkSource_eHifidac1   ?  "Hifidac1"  : 
             mclkSource == BAPE_MclkSource_eHifidac2   ?  "Hifidac2"  : 
             mclkSource == BAPE_MclkSource_eNco0       ?  "Nco0"      : 
             mclkSource == BAPE_MclkSource_eNco1       ?  "Nco1"      : 
             mclkSource == BAPE_MclkSource_eNco2       ?  "Nco2"      : 
             mclkSource == BAPE_MclkSource_eNco3       ?  "Nco3"      : 
             mclkSource == BAPE_MclkSource_eNco4       ?  "Nco4"      : 
             mclkSource == BAPE_MclkSource_eMax        ?  "<eMax>"    : 
                                                          "<Undefined>" );
}
/*************************************************************************/

BERR_Code BAPE_Mixer_P_DetermineOutputDataType(BAPE_MixerHandle handle, BAPE_DataType *pDataType)
{
    unsigned i;
    BAPE_DataType dataType = BAPE_DataType_eMax;
    bool pcm=true;
    unsigned numChannels=0;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != pDataType);

    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
    {
        if ( NULL != handle->inputs[i] )
        {
            BAPE_Connector input = handle->inputs[i];

            BDBG_MSG(("Input %s %s [%u] data format %s", input->pParent->pName, input->pName, i, BAPE_FMT_P_GetTypeName(&input->format)));
            if ( dataType == BAPE_DataType_eMax )
            {
                /* First valid input */
                dataType = input->format.type;
                pcm = BAPE_FMT_P_IsLinearPcm(&input->format);
                numChannels = BAPE_FMT_P_GetNumChannels(&input->format);
            }
            else
            {
                if ( BAPE_FMT_P_IsLinearPcm(&input->format) != pcm )
                {
                    BDBG_ERR(("PCM and non-PCM inputs can not be mixed."));
                    return BERR_TRACE(BERR_NOT_SUPPORTED);
                }
                if ( pcm )
                {
                    if ( BAPE_FMT_P_GetNumChannels(&input->format) > numChannels )
                    {
                        dataType = input->format.type;
                        numChannels = BAPE_FMT_P_GetNumChannels(&input->format);
                    }
                }
                else
                {
                    /* Don't allow mixing of data types for non-PCM sources. */
                    if ( dataType != input->format.type )
                    {
                        BDBG_ERR(("Can not mix non-PCM data of different formats"));
                        return BERR_TRACE(BERR_NOT_SUPPORTED);
                    }
                }
            }
        }
    }

    if ( dataType == BAPE_DataType_eMax )
    {
        /* If no inputs are detected, default to stereo */
        dataType = BAPE_DataType_ePcmStereo;
    }

    *pDataType = dataType;
    return BERR_SUCCESS;
}

#if BDBG_DEBUG_BUILD 

BDBG_FILE_MODULE(bape_diag);

static BERR_Code BAPE_Mixer_P_PrintOutputPortObjectInfo( BAPE_OutputPortObject  *pOutputPortObject, int level)
{
    BERR_Code errCode=BERR_SUCCESS;

    #if 0
        
            /* When we find a way to get info about the specific output device, then
             * we can print info specific to the output type like this:
             */
        
            if ( BAPE_OutputPortType_eI2sOutput == pOutputPortObject->type  )
            {
                BAPE_I2sOutputHandle  i2sOutputHandle = pOutputPortObject->pHandle;
        
                BDBG_MODULE_LOG(bape_diag,("%*sOutputPort: %s (%p) muted:%u Fs:%u enab:%u", level*4, "", 
                                            pOutputPortObject->pName, (void *) pOutputPortObject,
                                            pOutputPortObject->volume.muted  ));
            }
            else
    #endif
    {
        BDBG_LOG(("%*sOutputPort: %s (%p)", level*4, "", 
        pOutputPortObject->pName, (void *) pOutputPortObject  ));
    }

    return( errCode );
}


BERR_Code BAPE_Mixer_P_PrintInputPortInfo( BAPE_PathNode *pPathNode, int level, int index)
{
    BERR_Code errCode=BERR_SUCCESS;
    BAPE_InputPort  inputPort = NULL;
    BAVC_XptContextMap *pContextMap = NULL;
    BSTD_UNUSED(index);

    /* This function only works for nodes that can have an input port connected to them.
     * That means either a decoder, or input capture nodes.   */
    if ( BAPE_PathNodeType_eDecoder == pPathNode->type  )
    {
        BAPE_DecoderHandle      decoderHandle;

        /* The node is a decoder, so try to get either the input port or the RAVE context map.  */
        decoderHandle = pPathNode->pHandle;
        if (decoderHandle)
        {
            if (decoderHandle->startSettings.inputPort)
            {
                inputPort = decoderHandle->startSettings.inputPort;
            }
            else if (decoderHandle->startSettings.pContextMap)
            {
                pContextMap = &decoderHandle->contextMap;   
            }
        }
    }
    else if ( BAPE_PathNodeType_eInputCapture == pPathNode->type    )
    {
        BAPE_InputCaptureHandle      inputCaptureHandle;

        /* The node is an InputCapture, so get the connected InputPort.  */
        inputCaptureHandle = pPathNode->pHandle;
        if (inputCaptureHandle)
        {
            if (inputCaptureHandle->startSettings.input)
            {
                inputPort = inputCaptureHandle->startSettings.input;
            }
        }
    }

    if (inputPort)
    {
        /* Print out a line to describe the input port.  */
        BDBG_LOG(("%*sInputPort: %s (%p) Format:%s Fs:%u", level*4, "", 
                                    inputPort->pName,  (void *) inputPort,
                                    BAPE_FMT_P_GetTypeName(&inputPort->format), 
                                    inputPort->format.sampleRate
                                    ));
    }
    else if (pContextMap)
    {
        /* For a RAVE context input, just print the context index.  */
        BDBG_LOG(("%*sRAVE Context Index:%u", level*4, "", 
                                    pContextMap->ContextIdx ));
    }
    return( errCode );
}


BERR_Code BAPE_Mixer_P_PrintNodeInfo( BAPE_PathNode *pPathNode, int level, int index)
{
    BERR_Code errCode=BERR_SUCCESS;
    BSTD_UNUSED(index);

    /*  BDBG_ERR(("%s : %d : pPathNode:%p  level:%d  index:%u", __FUNCTION__, __LINE__ , pPathNode, level, index )); */
    if ( BAPE_PathNodeType_eDecoder == pPathNode->type  )
    {
        BAPE_DecoderHandle      decoderHandle;

        decoderHandle = pPathNode->pHandle;
        if (decoderHandle)
        {
            const BAPE_CodecAttributes  *pCodecAttributes = BAPE_P_GetCodecAttributes(decoderHandle->startSettings.codec);

            BDBG_LOG(("%*sPathNode: %s (%p) Type:%s Codec:%s", level*4, "", 
                                        pPathNode->pName,  (void *) pPathNode,   
                                        BAPE_PathNode_P_PathNodeTypeToText(pPathNode->type), 
                                        pCodecAttributes->pName ) );
        }
    }
    else if ( BAPE_PathNodeType_eMixer == pPathNode->type  )
    {
        BAPE_MixerHandle mixerHandle = pPathNode->pHandle;
        BAPE_OutputPortObject *pOutputPortObject;

        BDBG_LOG(("%*sPathNode: %s (%p) Type:%s %s(%u) Format:%s Fs:%u MclkSource:%s", level*4, "", 
                                    pPathNode->pName,  (void *) pPathNode,   
                                    BAPE_PathNode_P_PathNodeTypeToText(pPathNode->type),
                                    mixerHandle->running ? "RUNNING" : "stopped",
                                    mixerHandle->running , 
                                    BAPE_FMT_P_GetTypeName(&pPathNode->connectors[0].format),
                                    pPathNode->connectors[0].format.sampleRate,
                                    BAPE_Mixer_P_MclkSourceToText(mixerHandle->mclkSource)  ));
        
        for ( pOutputPortObject = BLST_S_FIRST(&mixerHandle->outputList);
              pOutputPortObject != NULL;
              pOutputPortObject = BLST_S_NEXT(pOutputPortObject, node) )
        {
            level++;
                BAPE_Mixer_P_PrintOutputPortObjectInfo( pOutputPortObject, level );
            level--;
        }

    }
    else
    {
        BDBG_LOG(("%*sPathNode: %s (%p) Type:%s ", level*4, "", 
        pPathNode->pName,  (void *) pPathNode,   BAPE_PathNode_P_PathNodeTypeToText(pPathNode->type) ) );
    
    }
    return( errCode );
}


static const char  *BAPE_Mixer_P_CheckBdbgEnabled(bool *enabled) {
    *enabled = true;
     return "";
}


BDBG_FILE_MODULE(bape_mixerprint);
BERR_Code BAPE_Mixer_P_PrintMixers(BAPE_Handle deviceHandle)
{
    BERR_Code errCode=BERR_SUCCESS;
    BAPE_MixerHandle mixer;
    BAPE_PathNode   *pPathNode;
    unsigned mixerIndex;
    bool  dbug_enabled = false;

    /* To determine if our debuglevel is enabled, we'll need to make two calls to BDBG_MODULE_MSG.  The first call
     * is to make sure that the module gets registered, then the second will only evaluate the argument list
     * if the BDBG level is enabled. 
     */ 
    BDBG_MODULE_MSG(bape_mixerprint,("bape_mixerprint is enabled"));                                                      /* Make sure that bape_mixerprint is registered with BDBG   */
    BDBG_MODULE_MSG(bape_mixerprint,("Printing Audio Filter Graph...%s", BAPE_Mixer_P_CheckBdbgEnabled(&dbug_enabled)));  /* Set dbug_enabled if bape_mixerprint is enabled           */
    if (!dbug_enabled)
    {
        return BERR_SUCCESS;    /* if debug level not enabled, just return without printing */
    }

    for ( mixer = BLST_S_FIRST(&deviceHandle->mixerList), mixerIndex=0;
          mixer != NULL;
          mixer = BLST_S_NEXT(mixer, node), mixerIndex++ )
    {
        BDBG_MODULE_LOG(bape_diag,("==================  Mixer:%u (%p) Begin  =====================================",
                                    mixerIndex, (void *) mixer ));

        pPathNode = &mixer->pathNode;
        BAPE_PathNode_P_EnumerateUpstreamPathNodes(pPathNode, 0, BAPE_Mixer_P_PrintNodeInfo );
        BAPE_Mixer_P_PrintNodeInfo( pPathNode, 0, 0);
        BAPE_PathNode_P_EnumerateDownstreamPathNodes(pPathNode, 0, BAPE_Mixer_P_PrintNodeInfo );

        BDBG_MODULE_LOG(bape_diag,("===================  Mixer:%u (%p) End  ======================================",
                                    mixerIndex, (void *) mixer ));

        BDBG_MODULE_LOG(bape_diag,(""));
    }

    return( errCode );
}


BDBG_FILE_MODULE(bape_startprint);
BERR_Code BAPE_Mixer_P_PrintDownstreamNodes(BAPE_PathNode *pPathNode)
{
    BERR_Code errCode=BERR_SUCCESS;
    bool  dbug_enabled = false;

    /* To determine if our debuglevel is enabled, we'll need to make two calls to BDBG_MODULE_MSG.  The first call
     * is to make sure that the module gets registered, then the second will only evaluate the argument list
     * if the BDBG level is enabled. 
     */ 
    BDBG_MODULE_MSG(bape_startprint,("bape_startprint is enabled"));                                                 /* Make sure that bape_startprint is registered with BDBG   */
    BDBG_MODULE_MSG(bape_startprint,("Printing Started Nodes...%s", BAPE_Mixer_P_CheckBdbgEnabled(&dbug_enabled)));  /* Set dbug_enabled if bape_startprint is enabled           */
    if (!dbug_enabled)
    {
        return BERR_SUCCESS;    /* if debug level not enabled, just return without printing */
    }

        BDBG_MODULE_LOG(bape_diag,("==================  Started node:%s (%p) Begin  =====================================",
                                    pPathNode->pName, pPathNode));

        /* First, try to print the InputPort or RAVE context that's attached to our starting node. */
        BAPE_Mixer_P_PrintInputPortInfo(pPathNode, 0, 0);

        /* Then print the starting node... */
        BAPE_Mixer_P_PrintNodeInfo( pPathNode, 1, 0);
        /* Then recurse down through the remaining connected nodes.  */
        BAPE_PathNode_P_EnumerateDownstreamPathNodes(pPathNode, 1, BAPE_Mixer_P_PrintNodeInfo );

        BDBG_MODULE_LOG(bape_diag,("==================  Started node:%s (%p) End  =====================================",
                                    pPathNode->pName, pPathNode));

        BDBG_MODULE_LOG(bape_diag,(""));

    return( errCode );
}


#else  /* BDBG_DEBUG_BUILD is false */
BERR_Code BAPE_Mixer_P_PrintMixers(BAPE_Handle deviceHandle)
{
    BERR_Code errCode=BERR_SUCCESS;
    BSTD_UNUSED(deviceHandle);
    return errCode;
}

#endif /* BDBG_DEBUG_BUILD */


