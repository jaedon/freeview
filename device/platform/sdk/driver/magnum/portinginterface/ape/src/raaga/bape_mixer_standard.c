/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_mixer_standard.c $
 * $brcm_Revision: Hydra_Software_Devel/28 $
 * $brcm_Date: 1/23/13 4:29p $
 *
 * Module Description: Mixer Interface for "Standard" Mixers
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_mixer_standard.c $
 * 
 * Hydra_Software_Devel/28   1/23/13 4:29p mpeteri
 * SW7358-430: merge to main
 * 
 * Hydra_Software_Devel/SW7358-430/1   1/18/13 4:54p mpeteri
 * SW7358-430: add checks to wait for ramping to complete during mixer
 * changes
 * 
 * Hydra_Software_Devel/27   1/23/13 9:04a jgarrett
 * SW7425-3404: Adding support for variable max independent delay
 * 
 * Hydra_Software_Devel/26   10/9/12 11:05p jgarrett
 * SW7425-3831: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-3831/2   10/9/12 5:41a jgarrett
 * SW7425-3831: Merge latest updates
 * 
 * Hydra_Software_Devel/SW7425-3831/1   9/14/12 6:12p jgarrett
 * SW7425-3831: Adding programmable mixer output rate
 * 
 * Hydra_Software_Devel/25   9/25/12 1:45p mpeteri
 * SW7358-278: merge to main
 * 
 * Hydra_Software_Devel/SW7358-278/2   9/13/12 12:12p mpeteri
 * SW7358-278: pick up RBUF pause burst changes
 * 
 * Hydra_Software_Devel/REFSW_APE_MAI_PAUSEBURST/2   9/13/12 11:59a mpeteri
 * SW7358-278: merge fixes back to MAI PB branch
 * 
 * Hydra_Software_Devel/REFSW_APE_MAI_PAUSEBURST/1   8/23/12 1:51p jgarrett
 * SW7358-78: Adding support for pauseburst insertion with HW cbits on MAI
 * 
 * Hydra_Software_Devel/24   8/20/12 11:41a mpeteri
 * SW7425-3646: merge to Hydra
 * 
 * Hydra_Software_Devel/SW7425-3646/2   8/10/12 6:09p mpeteri
 * SW7425-3646: update mute logic to handle ramping in PCM cases on
 * SPDIF/HDMI
 * 
 * Hydra_Software_Devel/SW7425-3646/1   8/10/12 5:07p mpeteri
 * SW7425-3646: pick up mute changes from sw7358-278
 * 
 * Hydra_Software_Devel/SW7358-278/1   7/18/12 5:36p mpeteri
 * SW7358-278: updates for muting MAI when using fw cbits
 * 
 * Hydra_Software_Devel/23   8/1/12 2:49p jgarrett
 * SW7418-110: Output mute is now independent of input mute for compressed
 * data
 * 
 * Hydra_Software_Devel/22   6/26/12 10:45a jgarrett
 * SW7425-3307: Refreshing NCO/PLL programming w/FS at mixer startup
 * 
 * Hydra_Software_Devel/21   6/25/12 3:17p jgarrett
 * SW7231-870: Supporting masters with LSF/QSF sample rates
 * 
 * Hydra_Software_Devel/20   6/11/12 5:27p jgarrett
 * SW7425-3220: Resolving Coverity 6 issues
 * 
 * Hydra_Software_Devel/19   5/9/12 3:30p jgarrett
 * SW7425-2075: Adding critical section around output sample rate changes
 * 
 * Hydra_Software_Devel/18   5/9/12 1:43p jgarrett
 * SW7425-2075: Checking for SFIFO before applying adaptive rate sample
 * rate
 * 
 * Hydra_Software_Devel/17   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/6   4/12/12 1:57p jgarrett
 * SW7425-2075: Adding HBR
 * 
 * Hydra_Software_Devel/SW7425-2075/5   4/9/12 2:04p jgarrett
 * SW7425-2075: Merging latest changes from main branch and enabling
 * 4x/16x OP Types
 * 
 * Hydra_Software_Devel/16   3/28/12 4:06p gskerl
 * SW7425-2644: When mixer is stopped, free mixer resources on any input
 * format change.
 * 
 * Hydra_Software_Devel/SW7425-2075/4   4/6/12 5:25p jgarrett
 * SW7425-2075: Re-evaluating mixer output format when new inputs are
 * added
 * 
 * Hydra_Software_Devel/SW7425-2075/3   4/6/12 4:15p jgarrett
 * SW7425-2075: Adding PCM-RF handling
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/6/12 3:51p jgarrett
 * SW7425-2075: Adding debug, fixing buffer logic
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:53p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/15   3/22/12 4:27p jgarrett
 * SW7425-2644: Ensuring output format is set properly with only a single
 * input
 * 
 * Hydra_Software_Devel/14   2/16/12 3:00p jgarrett
 * SW7552-188: Removing dummy mixer outputs.  MAI CBIT formatting no
 * longer requires this
 * 
 * Hydra_Software_Devel/13   2/13/12 10:41a jgarrett
 * SW7425-2377: Adding check for output delay out of range
 * 
 * Hydra_Software_Devel/12   1/27/12 5:12p jgarrett
 * SW7429-55: Revising MCLK selection to handle 7429 case with a DAC and
 * other outputs.  Also forcing default sample rate to be set prior to
 * enabling any outputs.
 * 
 * Hydra_Software_Devel/11   12/13/11 5:34p jgarrett
 * SW7425-1648: Adding support for mixing compressed sources
 * 
 * Hydra_Software_Devel/10   12/2/11 3:19p jgarrett
 * SW7346-578: Adding ability to change some mixer settings without
 * destroying and re-creating the mixer
 * 
 * Hydra_Software_Devel/9   8/23/11 1:23p gskerl
 * SW7425-1119: Added prototypes and stubs for BAPE_Mixer_Start() and
 * BAPE_Mixer_Stop(). For now, they just return BERR_NOT_SUPPORTED
 * 
 * Hydra_Software_Devel/8   8/2/11 3:27p jgarrett
 * SWDTV-8150: Adding mixer output connection support
 * 
 * Hydra_Software_Devel/7   7/11/11 4:56p gskerl
 * SW7425-884: Corrected arguments passed to BAPE_P_DetachMixerFromPll and
 * BAPE_P_DetachMixerFromNco
 * 
 * Hydra_Software_Devel/6   7/8/11 4:32p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/5   6/27/11 6:11p jgarrett
 * SW7344-84: Not applying volume coefficients to compressed inputs of
 * mixers
 * 
 * Hydra_Software_Devel/4   6/27/11 5:41p jgarrett
 * SW7231-97: Refactoring SPDIF/HDMI enable mechanisms to handle older
 * Onkyo receiver DTS->PCM switching requirements
 * 
 * Hydra_Software_Devel/3   6/6/11 1:13p gskerl
 * SW7425-321: Added comments
 * 
 * Hydra_Software_Devel/2   5/31/11 6:37p jgarrett
 * SW7425-406: Adding initial DSP mixer code
 * 
 * Hydra_Software_Devel/1   5/27/11 7:50p gskerl
 * SW7425-321: Split mixer code into multiple files. Added framework for
 * dispatching mixer API calls to multiple mixer types.
 * 
 * 
 * Hydra_Software_Devel/26   5/26/11 5:38p jgarrett
 * SW7346-224: Making sure buffers are validated even if mixer path is
 * cached.  Removing other connections may have removed buffers.
 *
 * Hydra_Software_Devel/SW7425-321/2   5/27/11 7:06p gskerl
 * SW7425-321: Changed names of BAPE_MixerInterface members. Changed
 * BAPE_Mixer.interface from struct to pointer.
 * 
 * Hydra_Software_Devel/SW7425-321/1   5/27/11 4:24p gskerl
 * SW7425-321: Split mixer code into multiple files. Added framework for
 * dispatching mixer API calls to multiple mixer types.
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
BDBG_FILE_MODULE(bape_mixer_get_mclk_source);

#define BAPE_MIXER_INPUT_INDEX_INVALID ((unsigned)-1)

/* Local function prototypes */
static void BAPE_StandardMixer_P_SetSampleRate_isr(BAPE_MixerHandle mixer, unsigned sampleRate);
static void BAPE_StandardMixer_P_SetInputSRC_isr(BAPE_MixerHandle mixer, BAPE_Connector input, unsigned inputRate, unsigned outputRate);
static BERR_Code BAPE_StandardMixer_P_RemoveAllInputs( BAPE_MixerHandle handle );
static BERR_Code BAPE_StandardMixer_P_RemoveAllOutputs( BAPE_MixerHandle handle );
static bool BAPE_StandardMixer_P_ValidateSettings(BAPE_Handle hApe, const BAPE_MixerSettings *pSettings);

/* Node callbacks */
static BERR_Code BAPE_StandardMixer_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void      BAPE_StandardMixer_P_FreePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_StandardMixer_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_StandardMixer_P_StartPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void      BAPE_StandardMixer_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_StandardMixer_P_OutputConnectionAdded(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void      BAPE_StandardMixer_P_OutputConnectionRemoved(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection); 
static void      BAPE_StandardMixer_P_InputSampleRateChange_isr(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, unsigned newSampleRate);
static BERR_Code BAPE_StandardMixer_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat);
static void      BAPE_StandardMixer_P_InputMute(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, bool enabled);
static void      BAPE_StandardMixer_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

/* Resource routines */
static BERR_Code BAPE_StandardMixer_P_AllocateResources(BAPE_MixerHandle mixer);
static BERR_Code BAPE_StandardMixer_P_AllocateConnectionResources(BAPE_MixerHandle handle, BAPE_PathConnection *pConnection);
static void BAPE_StandardMixer_P_FreeConnectionResources(BAPE_MixerHandle handle, BAPE_PathConnection *pConnection);
static void BAPE_StandardMixer_P_FreeResources(BAPE_MixerHandle handle);
static BERR_Code BAPE_StandardMixer_P_AllocateMixerGroups(BAPE_MixerHandle handle);

/* Define the interface struct with all the API callbacks.   It's declared and
 * populated at the bottom of this file.  */
static const BAPE_MixerInterface  standardMixerInterface;     


/*************************************************************************/
BERR_Code BAPE_StandardMixer_P_Create(
    BAPE_Handle deviceHandle,
    const BAPE_MixerSettings *pSettings,
    BAPE_MixerHandle *pHandle               /* [out] */
    )
{
    BAPE_MixerHandle handle;
    BAPE_MixerSettings defaultSettings;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);

    *pHandle = NULL;

    if ( NULL == pSettings )
    {
        BAPE_Mixer_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }
    
    /* Make sure settings are valid */
    if ( !BAPE_StandardMixer_P_ValidateSettings(deviceHandle, pSettings) )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    handle = BKNI_Malloc(sizeof(BAPE_Mixer));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    BKNI_Memset(handle, 0, sizeof(BAPE_Mixer));
    BDBG_OBJECT_SET(handle, BAPE_Mixer);
    handle->settings = *pSettings;
    handle->interface = &standardMixerInterface;
    handle->deviceHandle = deviceHandle;
    BLST_S_INSERT_HEAD(&deviceHandle->mixerList, handle, node);
    handle->fs = BAPE_FS_INVALID;
    BAPE_P_InitPathNode(&handle->pathNode, BAPE_PathNodeType_eMixer, handle->settings.type, 1, deviceHandle, handle);
    handle->pathNode.subtype = BAPE_MixerType_eStandard;
    handle->pathNode.pName = "Standard Mixer";
    handle->pathNode.connectors[0].useBufferPool = true;

    BAPE_Connector_P_GetFormat(&handle->pathNode.connectors[0], &format);
    format.sampleRate = 0;
    format.source = BAPE_DataSource_eFci;
    format.type = BAPE_DataType_ePcmStereo;
    errCode = BAPE_Connector_P_SetFormat(&handle->pathNode.connectors[0], &format);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_format; }

    BAPE_PathNode_P_GetInputCapabilities(&handle->pathNode, &caps);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDspBuffer);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eHostBuffer);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDfifo);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eFci);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcm5_1);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcm7_1);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmRf);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937x4);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937x16);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->pathNode, &caps);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_caps; }

    /* Fill in node connection routines */
    handle->pathNode.allocatePathFromInput = BAPE_StandardMixer_P_AllocatePathFromInput;
    handle->pathNode.freePathFromInput = BAPE_StandardMixer_P_FreePathFromInput;
    handle->pathNode.configPathFromInput = BAPE_StandardMixer_P_ConfigPathFromInput;
    handle->pathNode.startPathFromInput = BAPE_StandardMixer_P_StartPathFromInput;
    handle->pathNode.stopPathFromInput = BAPE_StandardMixer_P_StopPathFromInput;
    handle->pathNode.outputConnectionAdded = BAPE_StandardMixer_P_OutputConnectionAdded;
    handle->pathNode.outputConnectionRemoved = BAPE_StandardMixer_P_OutputConnectionRemoved;
    handle->pathNode.inputSampleRateChange_isr = BAPE_StandardMixer_P_InputSampleRateChange_isr;
    handle->pathNode.inputFormatChange = BAPE_StandardMixer_P_InputFormatChange;
    handle->pathNode.inputMute = BAPE_StandardMixer_P_InputMute;
    handle->pathNode.removeInput = BAPE_StandardMixer_P_RemoveInputCallback;

    *pHandle = handle;

    return BERR_SUCCESS;

err_caps:
err_format:
    BAPE_Mixer_Destroy(handle);

    return errCode;
}


/*************************************************************************/
static void BAPE_StandardMixer_P_Destroy(
    BAPE_MixerHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    
    /* Make sure no inputs are running. */
    if ( handle->running )
    {
        BDBG_ERR(("Mixer %p (%d) is running.  Cannot close.", handle, handle->index));
        BDBG_ASSERT(false == handle->running);
        return;
    }

    /* Remove all inputs */
    BAPE_StandardMixer_P_RemoveAllInputs(handle);

    /* Remove all outputs */
    BAPE_StandardMixer_P_RemoveAllOutputs(handle);

    /* Break any downstream connections */
    BAPE_Connector_P_RemoveAllConnections(&handle->pathNode.connectors[0]);
    
    /* Unlink from device */
    BLST_S_REMOVE(&handle->deviceHandle->mixerList, handle, BAPE_Mixer, node);
    BDBG_OBJECT_DESTROY(handle, BAPE_Mixer);
    BKNI_Free(handle);
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_AddInput(
    BAPE_MixerHandle handle,
    BAPE_Connector input,
    const BAPE_MixerAddInputSettings *pSettings     /* Optional, pass NULL for default settings */
    )
{
    unsigned i;
    BERR_Code errCode;
    BAPE_MixerAddInputSettings defaultSettings;
    bool makeMaster;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);

    if ( NULL == pSettings )
    {
        pSettings = &defaultSettings;
        BAPE_Mixer_GetDefaultAddInputSettings(&defaultSettings);
    }

    makeMaster = pSettings->sampleRateMaster;

    if ( handle->running )
    {
        BDBG_ERR(("Cannot change inputs while a mixer is active."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

#if BDBG_DEBUG_BUILD
    /* Make sure the same input isn't added multiple times */
    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
    {
        if ( handle->inputs[i] == input )
        {
            BDBG_ERR(("Cannot add the same input multiple times to a single mixer."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
#endif

    /* Find empty slot */
    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
    {
        if ( handle->inputs[i] == NULL )
        {
            handle->inputs[i] = input;
            BDBG_MSG(("Adding input %s to mixer %u as %s", input->pName, handle->index, makeMaster?"master":"slave"));
            if ( makeMaster )
            {
                if ( handle->master )
                {
                    BDBG_WRN(("Replacing current master input with new input"));
                }
                handle->master = input;
            }
            BAPE_Mixer_P_GetDefaultInputVolume(&handle->inputVolume[i]);
            errCode = BAPE_PathNode_P_AddInput(&handle->pathNode, input);
            if ( errCode )
            {
                handle->inputs[i] = NULL;
                return BERR_TRACE(errCode);
            }
            return BERR_SUCCESS;
        }
    }

    /* Mixer has no available slots. */
    BDBG_ERR(("Mixer can not accept any more inputs.  Maximum inputs per mixer is %u.", BAPE_CHIP_MAX_MIXER_INPUTS));
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_RemoveInput(
    BAPE_MixerHandle handle,
    BAPE_Connector input
    )
{
    unsigned i;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);

    if ( handle->running )
    {
        BDBG_ERR(("Cannot change inputs while a mixer is active."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Find slot containing this input */
    i = BAPE_Mixer_P_FindInputIndex(handle, input);
    if ( i < BAPE_CHIP_MAX_MIXER_INPUTS )
    {
        errCode = BAPE_PathNode_P_RemoveInput(&handle->pathNode, input);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        handle->inputs[i] = NULL;
        if ( handle->master == input )
        {
            BDBG_MSG(("Removing master input %p", input));
            handle->master = NULL;
        }
        return BERR_SUCCESS;
    }

    /* Input not found. */
    BDBG_ERR(("Input %p is not connected to mixer %p", input, handle));
    return BERR_TRACE(BERR_INVALID_PARAMETER);
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_RemoveAllInputs(
    BAPE_MixerHandle handle
    )
{
    unsigned i;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
    {
        if ( handle->inputs[i] )
        {
            errCode = BAPE_StandardMixer_P_RemoveInput(handle, handle->inputs[i]);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }

    return BERR_SUCCESS;
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_AddOutput(
    BAPE_MixerHandle handle,
    BAPE_OutputPort output
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    if ( output->mixer )
    {
        BDBG_ERR(("Output %p is already connected to mixer %p", output, output->mixer));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( handle->running )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
        /* Bind mixer and output.  Remainder is done at start time. */
        output->mixer = handle;
        BLST_S_INSERT_HEAD(&handle->outputList, output, node);
        handle->numOutputs++;
        output->sourceMixerGroup = NULL;
        output->sourceMixerOutputIndex = 0;
        BAPE_StandardMixer_P_FreeResources(handle);
    
        BDBG_MSG(("Added output %p '%s' to mixer %u [mixer rate %u]", output, output->pName, handle->index, BAPE_Mixer_P_GetOutputSampleRate(handle)));
        BKNI_EnterCriticalSection();
        if ( output->setTimingParams_isr && BAPE_Mixer_P_GetOutputSampleRate_isr(handle) != 0 )
        {
            output->setTimingParams_isr(output, BAPE_Mixer_P_GetOutputSampleRate_isr(handle), handle->settings.outputTimebase);
        }
        BKNI_LeaveCriticalSection();
    }

    return BERR_SUCCESS;
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_RemoveOutput(
    BAPE_MixerHandle handle,
    BAPE_OutputPort output
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    if ( output->mixer != handle )
    {
        BDBG_ERR(("Output %p is not connected to mixer %p"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( handle->running )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else
    {
        /* Release mixer-level resources and rebuild on the next start */
        BAPE_StandardMixer_P_FreeResources(handle);
        output->mixer = NULL;
        BLST_S_REMOVE(&handle->outputList, output, BAPE_OutputPortObject, node);
        BDBG_ASSERT(handle->numOutputs > 0);
        handle->numOutputs--;
        BDBG_MSG(("Removed output %p '%s' from mixer %u [mixer rate %u]", output, output->pName, handle->index, BAPE_Mixer_P_GetOutputSampleRate(handle)));
    }

    return BERR_SUCCESS;
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_RemoveAllOutputs(
    BAPE_MixerHandle handle
    )
{
    BAPE_OutputPort output;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    while ( (output=BLST_S_FIRST(&handle->outputList)) )
    {
        errCode = BAPE_StandardMixer_P_RemoveOutput(handle, output);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_GetInputVolume(
    BAPE_MixerHandle handle,
    BAPE_Connector input,
    BAPE_MixerInputVolume *pVolume      /* [out] */
    )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    BDBG_ASSERT(NULL != pVolume);

    i = BAPE_Mixer_P_FindInputIndex(handle, input);
    if ( i == BAPE_MIXER_INPUT_INDEX_INVALID )
    {
        BDBG_ERR(("Input %p is not connected to mixer %p", input, handle));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    *pVolume = handle->inputVolume[i];

    return BERR_SUCCESS;
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_ApplyInputVolume(BAPE_MixerHandle mixer, unsigned index)
{
    BAPE_MixerGroupInputSettings mixerInputSettings;
    BAPE_Connector source = mixer->inputs[index];
    unsigned mixerNum;
    unsigned i;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(source, BAPE_PathConnector);

    for ( mixerNum = 0; mixerNum < BAPE_CHIP_MAX_MIXERS; mixerNum++ )
    {
        BAPE_MixerGroupHandle mixerGroup = mixer->mixerGroups[mixerNum];
        if ( NULL != mixerGroup )
        {
            BAPE_MixerGroup_P_GetInputSettings(mixerGroup, index, &mixerInputSettings);
            for ( i = 0; i < BAPE_ChannelPair_eMax; i++ )
            {
                if ( !BAPE_FMT_P_IsLinearPcm(&source->format) && !mixer->inputMuted[index] )
                {
                    /* Non-PCM inputs must be either full-scale or 0 coefficients. */
                    int32_t coef = BAPE_FMT_P_GetNumChannelPairs(&source->format) > i ? BAPE_VOLUME_NORMAL : 0;
                    mixerInputSettings.coefficients[i][0][0] = coef;
                    mixerInputSettings.coefficients[i][1][0] = 0;
                    mixerInputSettings.coefficients[i][0][1] = 0;
                    mixerInputSettings.coefficients[i][1][1] = coef;
                }
                else if ( BAPE_FMT_P_GetNumChannelPairs(&source->format) > i && !mixer->inputMuted[index] && !mixer->inputVolume[index].muted )
                {
                    /* PCM inputs can have variable volume */
                    mixerInputSettings.coefficients[i][0][0] = mixer->inputVolume[index].coefficients[2*i][2*i];
                    mixerInputSettings.coefficients[i][1][0] = mixer->inputVolume[index].coefficients[(2*i)+1][2*i];
                    mixerInputSettings.coefficients[i][0][1] = mixer->inputVolume[index].coefficients[2*i][(2*i)+1];
                    mixerInputSettings.coefficients[i][1][1] = mixer->inputVolume[index].coefficients[(2*i)+1][(2*i)+1];
                }
                else
                {
                    /* Mute for PCM or compressed inputs is identical */
                    mixerInputSettings.coefficients[i][0][0] = 0;
                    mixerInputSettings.coefficients[i][1][0] = 0;
                    mixerInputSettings.coefficients[i][0][1] = 0;
                    mixerInputSettings.coefficients[i][1][1] = 0;
                }
            }
            errCode = BAPE_MixerGroup_P_SetInputSettings(mixerGroup, index, &mixerInputSettings);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }

    return BERR_SUCCESS;
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_SetInputVolume(
    BAPE_MixerHandle handle,
    BAPE_Connector input,
    const BAPE_MixerInputVolume *pVolume
    )
{
    unsigned i;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    BDBG_ASSERT(NULL != pVolume);

    i = BAPE_Mixer_P_FindInputIndex(handle, input);
    if ( i == BAPE_MIXER_INPUT_INDEX_INVALID )
    {
        BDBG_ERR(("Input %p is not connected to mixer %p", input, handle));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    handle->inputVolume[i] = *pVolume;
    /* Apply volume if input is running */
    if ( handle->running )
    {
        errCode = BAPE_StandardMixer_P_ApplyInputVolume(handle, i);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;    
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_ApplyOutputVolume(BAPE_MixerHandle mixer, BAPE_OutputPort output)
{
    bool pcm;
    bool handleOutputMuteFirst = true;
    unsigned i;
    BERR_Code errCode;
    BAPE_MixerGroupOutputSettings outputSettings;

    BDBG_ASSERT(mixer == output->mixer);
    BDBG_ASSERT(NULL != output->sourceMixerGroup);

    BAPE_MixerGroup_P_GetOutputSettings(output->sourceMixerGroup, output->sourceMixerOutputIndex, &outputSettings);

    pcm = BAPE_FMT_P_IsLinearPcm(BAPE_Mixer_P_GetOutputFormat(mixer));

    for ( i = 0; i < BAPE_Channel_eMax; i++ )
    {
        outputSettings.coefficients[i] = (pcm)?output->volume.volume[i]:BAPE_VOLUME_NORMAL;
    }

    if ( output->muteInMixer )
    {
        outputSettings.muted = output->volume.muted;
        outputSettings.volumeRampDisabled = !pcm;  /* Disable volume ramp if input is compressed for a pseudo-compressed-mute */
    }
    else
    {
        outputSettings.muted = false;
        outputSettings.volumeRampDisabled = false;
    }

    if ( (output->volume.muted && pcm) || (!output->volume.muted && !pcm) )
    {
        handleOutputMuteFirst = false;
    }

    /* mute output before mixer mute settings have been applied */
    if ( output->setMute && handleOutputMuteFirst )
    {
        /* Call the output's mute handler */
        BDBG_MSG(("Setting output mute to %u", output->volume.muted));
        output->setMute(output, output->volume.muted, false);
    }

    errCode = BAPE_MixerGroup_P_SetOutputSettings(output->sourceMixerGroup ,output->sourceMixerOutputIndex, &outputSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* unmute output after mixer mute settings have been applied */
    if ( output->setMute && !handleOutputMuteFirst )
    {
        /* Call the output's mute handler */
        BDBG_MSG(("Setting output mute to %u", output->volume.muted));
        output->setMute(output, output->volume.muted, false);
    }

    return BERR_SUCCESS;
}

static bool BAPE_StandardMixer_P_ValidateSettings(BAPE_Handle hApe, const BAPE_MixerSettings *pSettings)
{
    switch ( pSettings->defaultSampleRate )
    {
    case 32000:
    case 44100:
    case 48000:
        break;
    default:
        BDBG_ERR(("Mixers only support default sample rates of 32k, 44.1k or 48kHz."));
        return false;
    }

    if ( pSettings->outputDelay > hApe->settings.maxIndependentDelay )
    {
        BDBG_ERR(("Output delay value (%u ms) is larger than the maximum (%u ms).  Adjust BAPE_Settings.maxIndependentDelay if a larger value is required.", pSettings->outputDelay, hApe->settings.maxIndependentDelay));
        return false;
    }

    /* Settings are valid */
    return true;
}

static BERR_Code BAPE_StandardMixer_P_SetSettings(
    BAPE_MixerHandle hMixer,
    const BAPE_MixerSettings *pSettings
    )
{
    bool timebaseChanged;

    BDBG_OBJECT_ASSERT(hMixer, BAPE_Mixer);
    BDBG_ASSERT(NULL != pSettings);

    if ( hMixer->running )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( !BAPE_StandardMixer_P_ValidateSettings(hMixer->deviceHandle, pSettings) )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    
    /* Give up our resources if allocated */   
    BAPE_StandardMixer_P_FreeResources(hMixer);

    /* Determine if the timebase has changed */
    timebaseChanged = (hMixer->settings.outputTimebase != pSettings->outputTimebase) ? true : false;

    /* Store new settings, they will be applied next time the mixer tries to start */
    hMixer->settings = *pSettings;

    /* Refresh output timebase if needed */
    if ( timebaseChanged && BAPE_Mixer_P_GetOutputSampleRate(hMixer) != 0 )
    {
        BAPE_OutputPort output;

        BKNI_EnterCriticalSection();
        for ( output = BLST_S_FIRST(&hMixer->outputList); 
              output != NULL;
              output = BLST_S_NEXT(output, node) )
        {
            if ( output->setTimingParams_isr )
            {
                output->setTimingParams_isr(output, BAPE_Mixer_P_GetOutputSampleRate(hMixer), pSettings->outputTimebase);
            }
        }
        BKNI_LeaveCriticalSection();
    }

    return BERR_SUCCESS;
}

/*************************************************************************/
/* Allocate mixer resources */
static BERR_Code BAPE_StandardMixer_P_AllocatePathFromInput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    )
{
    BAPE_MixerHandle handle;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    /* Grab mixer-level resources */
    errCode = BAPE_StandardMixer_P_AllocateResources(handle);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Grab resources for this connection */
    errCode = BAPE_StandardMixer_P_AllocateConnectionResources(handle, pConnection);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}


/*************************************************************************/
/* Release mixer resources */
static void BAPE_StandardMixer_P_FreePathFromInput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    )
{
    BAPE_MixerHandle handle;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    BDBG_MSG(("Free Path From Input %s %s", pConnection->pSource->pParent->pName, pConnection->pSource->pName));

    /* Release resources for this connection */
    BAPE_StandardMixer_P_FreeConnectionResources(handle, pConnection);

    /* Free mixer-level resources if needed  */
    BAPE_StandardMixer_P_FreeResources(handle);
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_ConfigPathFromInput(
    struct BAPE_PathNode *pNode, 
    struct BAPE_PathConnection *pConnection
    )
{
    unsigned inputNum, numChannelPairs;
    unsigned mixerIndex;
    BERR_Code errCode;
    
    BAPE_MixerHandle handle;
    BAPE_MixerGroupInputSettings mixerInputSettings;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    
    /* Setup linkage from conection into mixer */
    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(BAPE_Mixer_P_GetOutputFormat(handle));
    inputNum = BAPE_Mixer_P_FindInputIndex(handle, pConnection->pSource);

    for ( mixerIndex = 0; mixerIndex < handle->numMixerGroups; mixerIndex++ )
    {
        BAPE_MixerGroupHandle mixerGroup = handle->mixerGroups[mixerIndex];
        /* Link Input -> Mixer */
        BAPE_MixerGroup_P_GetInputSettings(mixerGroup, inputNum, &mixerInputSettings);
        if ( pConnection->srcGroup )
        {
            BDBG_MSG(("Linking mixer to associated SRC"));
            BAPE_SrcGroup_P_GetOutputFciIds(pConnection->srcGroup, &mixerInputSettings.input);
        }
        else if ( pConnection->sfifoGroup )
        {
            BDBG_MSG(("Linking mixer to associated SFIFO"));
            BAPE_SfifoGroup_P_GetOutputFciIds(pConnection->sfifoGroup, &mixerInputSettings.input);
        }
        else
        {
            BDBG_MSG(("Linking mixer to associated FCI"));
            mixerInputSettings.input = pConnection->inputFciGroup;
        }
        errCode = BAPE_MixerGroup_P_SetInputSettings(mixerGroup, inputNum, &mixerInputSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    /* Done.  Refresh input scaling now */
    BAPE_StandardMixer_P_ApplyInputVolume(handle, inputNum);

    return BERR_SUCCESS;
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_StartPathFromInput(
    struct BAPE_PathNode *pNode, 
    struct BAPE_PathConnection *pConnection
    )
{
    BAPE_MixerHandle handle;
    BAPE_Connector input;
    BAPE_OutputPort output;
    unsigned mixerIndex, inputIndex;
    BERR_Code errCode;
    BAPE_PathConnection *pOutputConnection;
    bool highPriority;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    input = pConnection->pSource;
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);

    BDBG_MSG(("Input %s %s starting", input->pParent->pName, input->pName));

    /* First, setup the sample rate since it may affect output programming. */
    BKNI_EnterCriticalSection();
    BAPE_StandardMixer_P_InputSampleRateChange_isr(pNode, pConnection, pConnection->pSource->format.sampleRate);
    BKNI_LeaveCriticalSection();

    /* Start the global mixer components */
    if ( handle->running == 0 )
    {
        BAPE_MixerGroupSettings mixerGroupSettings;

        highPriority = (BAPE_Mixer_P_GetOutputDataType(handle) == BAPE_DataType_eIec61937x16)? true : false;

        mixerIndex = 0;
        /* Enable all the mixer outputs */
        for ( output = BLST_S_FIRST(&handle->outputList); 
              output != NULL;
              output = BLST_S_NEXT(output, node) )
        {
            BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);
            BDBG_ASSERT(NULL != output->enable);

            BDBG_MSG(("Enabling output %s (%p)", output->pName, output));                
            errCode = output->enable(output);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_output;
            }

            /* Apply output volume after enabling outputs (many mute in the output itself) */
            BAPE_StandardMixer_P_ApplyOutputVolume(handle, output);

            /* Setup mixer priority */
            BAPE_MixerGroup_P_GetSettings(output->sourceMixerGroup, &mixerGroupSettings);
            mixerGroupSettings.highPriority = highPriority;
            errCode = BAPE_MixerGroup_P_SetSettings(output->sourceMixerGroup, &mixerGroupSettings);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_output;
            }

            /* Start mixer output(s) */
            BDBG_ASSERT(NULL != output->sourceMixerGroup);
            errCode = BAPE_MixerGroup_P_StartOutput(output->sourceMixerGroup, output->sourceMixerOutputIndex);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_output;
            }
            /* This had best not fail */
            BDBG_ASSERT(errCode == BERR_SUCCESS);
            mixerIndex++;
        }
        for ( pOutputConnection = BLST_SQ_FIRST(&handle->pathNode.connectors[0].connectionList);
              pOutputConnection != NULL;
              pOutputConnection = BLST_SQ_NEXT(pOutputConnection, downstreamNode) )
        {
            BAPE_MixerGroup_P_GetSettings(handle->mixerGroups[mixerIndex], &mixerGroupSettings);
            mixerGroupSettings.highPriority = highPriority;
            errCode = BAPE_MixerGroup_P_SetSettings(handle->mixerGroups[mixerIndex], &mixerGroupSettings);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_output;
            }
            errCode = BAPE_MixerGroup_P_StartOutput(handle->mixerGroups[mixerIndex], 0);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_output;
            }
            mixerIndex++;
        }
    }

    /* Enable Mixer Inputs Next */
    inputIndex = BAPE_Mixer_P_FindInputIndex(handle, input);
    BDBG_ASSERT(inputIndex < BAPE_CHIP_MAX_MIXER_INPUTS);
    for ( mixerIndex = 0; mixerIndex < handle->numMixerGroups; mixerIndex++ )
    {
        errCode = BAPE_MixerGroup_P_StartInput(handle->mixerGroups[mixerIndex], inputIndex);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_input;
        }
        /* This had best not fail */
        BDBG_ASSERT(errCode == BERR_SUCCESS);
    }

    /* Enable SRC's next */
    if ( pConnection->srcGroup )
    {
        errCode = BAPE_SrcGroup_P_Start(pConnection->srcGroup);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_src;
        }
    }

    BDBG_MSG(("Mixer Input %s [%u] started", input->pName, inputIndex));
    handle->running++;
    handle->inputRunning[inputIndex] = true;

    return BERR_SUCCESS;

err_src:
    inputIndex = BAPE_Mixer_P_FindInputIndex(handle, input);
    BDBG_ASSERT(inputIndex < BAPE_CHIP_MAX_MIXER_INPUTS);
    for ( mixerIndex = 0; mixerIndex < handle->numMixerGroups; mixerIndex++ )
    {
        BAPE_MixerGroup_P_StopInput(handle->mixerGroups[mixerIndex], inputIndex);
    }    
err_input:
err_output:
    if ( handle->running == 0 )
    {
        mixerIndex = 0;
        /* Disable all the mixer outputs */
        for ( output = BLST_S_FIRST(&handle->outputList); 
            output != NULL;
            output = BLST_S_NEXT(output, node) )
        {
            BAPE_MixerGroup_P_StopOutput(output->sourceMixerGroup, output->sourceMixerOutputIndex);

            BDBG_ASSERT(NULL != output->disable);
            output->disable(output);
            mixerIndex++;
        }
        for ( pOutputConnection = BLST_SQ_FIRST(&handle->pathNode.connectors[0].connectionList);
              pOutputConnection != NULL;
              pOutputConnection = BLST_SQ_NEXT(pOutputConnection, downstreamNode) )
        {
            BAPE_MixerGroup_P_StopOutput(handle->mixerGroups[mixerIndex], 0);
            mixerIndex++;
        }
    }                
    return errCode;
}


/*************************************************************************/
static void BAPE_StandardMixer_P_StopPathFromInput(
    struct BAPE_PathNode *pNode, 
    struct BAPE_PathConnection *pConnection
    )
{
    BAPE_MixerHandle handle;
    BAPE_Connector input;
    BAPE_OutputPort output;
    unsigned mixerIndex, inputIndex;
    BAPE_PathConnection *pOutputConnection;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    input = pConnection->pSource;
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);

    /* Determine the input index for this connection */
    inputIndex = BAPE_Mixer_P_FindInputIndex(handle, input);
    BDBG_ASSERT(inputIndex < BAPE_CHIP_MAX_MIXER_INPUTS);

    /* If this input was not started (e.g. error in StartPath) return out */
    if ( !handle->inputRunning[inputIndex] )
    {
        BDBG_MSG(("Input %u was not started.  Nothing to stop.", inputIndex));
        return;
    }
    handle->inputRunning[inputIndex] = false;

    for ( mixerIndex = 0; mixerIndex < handle->numMixerGroups; mixerIndex++ )
    {
        /* wait for ramping to complete on this mixer group */
        if ( BAPE_MixerGroup_P_WaitForRamping(handle->mixerGroups[mixerIndex]) == BERR_TIMEOUT )
        {
            BDBG_ERR(("WARNING - %s - Vol Ramp timed out...", __FUNCTION__));
        }
    }

    /* Disable SRC */
    if ( pConnection->srcGroup )
    {
        BAPE_SrcGroup_P_Stop(pConnection->srcGroup);
    }

    /* Disable input to each mixer */
    for ( mixerIndex = 0; mixerIndex < handle->numMixerGroups; mixerIndex++ )
    {
        BAPE_MixerGroup_P_StopInput(handle->mixerGroups[mixerIndex], inputIndex);
    }

    BDBG_ASSERT(handle->running > 0);
    BDBG_MSG(("Mixer run count was %u now %u", handle->running, handle->running-1));
    handle->running--;
    /* If there are other running inputs, stop outputs here. */
    if ( handle->running == 0 )
    {
        /* We're the last running input.  Stop the mixers and outputs now. */
        BDBG_MSG(("Mixer %p (%d) has no more running inputs.  Stopping all mixer outputs.", handle, handle->index));

        for ( output = BLST_S_FIRST(&handle->outputList); 
            output != NULL;
            output = BLST_S_NEXT(output, node) )
        {
            BDBG_ASSERT(NULL != output->sourceMixerGroup);
            BAPE_MixerGroup_P_StopOutput(output->sourceMixerGroup, output->sourceMixerOutputIndex);
        }

        mixerIndex = 0;
        for ( output = BLST_S_FIRST(&handle->outputList); 
            output != NULL;
            output = BLST_S_NEXT(output, node) )
        {
            BDBG_MSG(("Disabling output %s (%p)", output->pName, output));

            BDBG_ASSERT(NULL != output->disable);
            output->disable(output);    
            mixerIndex++;
        }
        for ( pOutputConnection = BLST_SQ_FIRST(&handle->pathNode.connectors[0].connectionList);
              pOutputConnection != NULL;
              pOutputConnection = BLST_SQ_NEXT(pOutputConnection, downstreamNode) )
        {
            BAPE_MixerGroup_P_StopOutput(handle->mixerGroups[mixerIndex], 0);
            mixerIndex++;
        }
    }    
}

static BERR_Code BAPE_StandardMixer_P_OutputConnectionAdded(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_MixerHandle handle;

    BSTD_UNUSED(pConnection);        
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    
    if ( handle->running > 0 )
    {
        BDBG_ERR(("Cannot add connections while a mixer is running."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    
    /* Update number of output connections */
    handle->numOutputConnections++;
    /* Release any saved resources and re-allocate on the next start call */
    BAPE_StandardMixer_P_FreeResources(handle);
    
    return BERR_SUCCESS;
}

static void BAPE_StandardMixer_P_OutputConnectionRemoved(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_MixerHandle handle;
        
    BSTD_UNUSED(pConnection);        
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    /* Make sure we're not running.  Should never happen. */
    BDBG_ASSERT(handle->running == 0);
    
    /* Release any saved resources and re-allocate on the next start call.  This will also remove any output connection resources. */
    BAPE_StandardMixer_P_FreeResources(handle);

    /* Decrement number of output connections */
    BDBG_ASSERT(handle->numOutputConnections > 0);
    handle->numOutputConnections--;        
}

/*************************************************************************/
static void BAPE_StandardMixer_P_InputSampleRateChange_isr(
    struct BAPE_PathNode *pNode, 
    struct BAPE_PathConnection *pConnection,
    unsigned newSampleRate
    )
{
    BAPE_MixerHandle handle;
    unsigned sampleRate;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BKNI_ASSERT_ISR_CONTEXT();

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    sampleRate = newSampleRate;

    /* If we are setting the same rate, ignore it */
    if ( pConnection->format.sampleRate == sampleRate && sampleRate != 0 )
    {
        BDBG_MSG(("Input %s %s sample rate unchanged (%u)", pConnection->pSource->pParent->pName, pConnection->pSource->pName, pConnection->format.sampleRate));
        return;
    }

    BDBG_MSG(("Input %s %s sample rate changed from %u to %u (master=%d)", pConnection->pSource->pParent->pName, pConnection->pSource->pName, pConnection->format.sampleRate, sampleRate, pConnection->pSource == handle->master));

    /* Mark new sample rate in connection */
    pConnection->format.sampleRate = sampleRate;

    /* Handle fixed rate output */
    if ( handle->settings.mixerSampleRate )
    {
        /* Set mixer sample rate. */
        BAPE_StandardMixer_P_SetSampleRate_isr(handle, handle->settings.mixerSampleRate);
    }
    /* Handle the sample rate master */
    else if ( handle->master == pConnection->pSource )
    {
        if ( sampleRate != 0 )
        {
            unsigned mixerRate=sampleRate;
            if ( BAPE_FMT_P_IsLinearPcm(&pConnection->pSource->format) )
            {
                switch ( sampleRate )
                {
                case 8000:
                case 16000:
                    BDBG_MSG(("Using output rate of 32000 for mixer"));
                    mixerRate = 32000;
                    break;
                case 11025:
                case 22050:
                    BDBG_MSG(("Using output rate of 44100 for mixer"));
                    mixerRate = 44100;
                    break;
                case 12000:
                case 24000:
                    BDBG_MSG(("Using output rate of 48000 for mixer"));
                    mixerRate = 48000;
                    break;
                case 32000:
                case 44100:
                case 48000:
                case 64000:
                case 88200:
                case 96000:
                case 176400:
                case 192000:
                    BDBG_MSG(("Mixer input rate is supported.  Using it directly."));
                    mixerRate = sampleRate;
                    break;
                default:
                    BDBG_WRN(("Nonstandard PCM rate (%u) from input %s %s, defaulting to %u output rate", sampleRate, pConnection->pSource->pParent->pName, pConnection->pSource->pName, handle->settings.defaultSampleRate));
                    mixerRate = handle->settings.defaultSampleRate;
                    break;
                }
            }
            /* Set mixer sample rate. */
            BAPE_StandardMixer_P_SetSampleRate_isr(handle, mixerRate);
        }
        else if ( BAPE_Mixer_P_GetOutputSampleRate(handle) == 0 )
        {
            /* Make sure there is a valid sample rate */
            BAPE_StandardMixer_P_SetSampleRate_isr(handle, handle->settings.defaultSampleRate);
        }
    }
    else
    {
        /* Make sure there is a valid sample rate */
        if ( BAPE_Mixer_P_GetOutputSampleRate(handle) == 0 )
        {
            BAPE_StandardMixer_P_SetSampleRate_isr(handle, handle->settings.defaultSampleRate);
        }
    }

    /* Update SRCs accordingly. */
    BAPE_StandardMixer_P_SetInputSRC_isr(handle, pConnection->pSource, sampleRate, BAPE_Mixer_P_GetOutputSampleRate(handle));

    if ( pConnection->format.ppmCorrection && NULL != pConnection->sfifoGroup )
    {
        /* Update adaptive rate controllers */
        BAPE_SfifoGroup_P_SetSampleRate_isr(pConnection->sfifoGroup, sampleRate);
    }
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_InputFormatChange(
    struct BAPE_PathNode *pNode, 
    struct BAPE_PathConnection *pConnection,
    const BAPE_FMT_Descriptor *pNewFormat
    )
{
    BAPE_MixerHandle handle;
    BAPE_DataType dataType;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    /* Handle a format change by releasing all connection resources.
       They will get re-allocated on the next start sequence for the correct format.
    */
    BDBG_MSG(("Input Format Changed for input %s %s [%s->%s]", pConnection->pSource->pParent->pName, pConnection->pSource->pName,
              BAPE_FMT_P_GetTypeName(&pConnection->format), BAPE_FMT_P_GetTypeName(pNewFormat)));

    /* Determine if we need to release resources or not */
    if ( pConnection->format.type != pNewFormat->type || pConnection->format.ppmCorrection != pNewFormat->ppmCorrection )
    {
        BDBG_MSG(("Resource requirements changed.  Freeing connection resources."));
        BAPE_StandardMixer_P_FreeConnectionResources(handle, pConnection);

        /* Some changes are not supported while running.  Check these now. */    
        if ( handle->running > 0 )
        {
            if ( BAPE_FMT_P_IsLinearPcm(pNewFormat) != BAPE_FMT_P_IsLinearPcm(BAPE_Mixer_P_GetOutputFormat(handle)) )
            {
            BDBG_ERR(("Can not change from compressed to PCM or vice-versa while other inputs are running."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }

            /* TODO: Technically, this is possible if we permit downmixing in the mixer.  Currently not required for STB use cases. */
            if ( BAPE_FMT_P_GetNumChannelPairs(pNewFormat) > BAPE_FMT_P_GetNumChannelPairs(BAPE_Mixer_P_GetOutputFormat(handle)) )
        {
            BDBG_ERR(("Input is wider than the current mixer data format.  Cannot change this while other inputs are running."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        }
    }

    /* Re-evaluate mixer output format */
    errCode = BAPE_Mixer_P_DetermineOutputDataType(handle, &dataType);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    if ( dataType != BAPE_Mixer_P_GetOutputDataType(handle) )
    {
        BAPE_FMT_Descriptor format;

        /* If the data format has changed while stopped, re-evaluate the output data format. */
        BAPE_StandardMixer_P_FreeResources(handle);

        BAPE_Connector_P_GetFormat(&handle->pathNode.connectors[0], &format);
        format.type = dataType;
        /* Update output format and propagate it */
        errCode = BAPE_Connector_P_SetFormat(&handle->pathNode.connectors[0], &format);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    
    /* If this is the master input, propagate data type to output data type if required. */
    if ( pConnection->pSource == handle->master )
    {
        switch ( pNewFormat->type )
        {
        case BAPE_DataType_eIec61937:
        case BAPE_DataType_eIec61937x4:
        case BAPE_DataType_eIec61937x16:
            BAPE_FMT_P_SetAudioCompressionStd(&handle->pathNode.connectors[0].format, BAPE_FMT_P_GetAudioCompressionStd(pNewFormat));
            break;
        case BAPE_DataType_ePcmRf:
            BAPE_FMT_P_SetRfAudioEncoding(&handle->pathNode.connectors[0].format, BAPE_FMT_P_GetRfAudioEncoding(pNewFormat));
            break;
        default:
            break;
        }
    }
    BKNI_EnterCriticalSection();
    BAPE_StandardMixer_P_InputSampleRateChange_isr(pNode, pConnection, pNewFormat->sampleRate);
    BKNI_LeaveCriticalSection();

    return BERR_SUCCESS;
}


/*************************************************************************/
static void BAPE_StandardMixer_P_InputMute(
    struct BAPE_PathNode *pNode, 
    struct BAPE_PathConnection *pConnection,
    bool enabled
    )
{
    BAPE_MixerHandle handle;
    unsigned i;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    /* Determine mixer input index */
    i = BAPE_Mixer_P_FindInputIndex(handle, pConnection->pSource);

    /* Should never happen */
    BDBG_ASSERT(i < BAPE_CHIP_MAX_MIXER_INPUTS);

    /* See if mute state is changing */
    if ( handle->inputMuted[i] != enabled )
    {
        /* Update mute state and re-apply input volume */
        handle->inputMuted[i] = enabled;
        BAPE_StandardMixer_P_ApplyInputVolume(handle, i);
    }
}


/*************************************************************************/
static BAPE_MclkSource BAPE_StandardMixer_P_GetMclkSource(BAPE_MixerHandle mixer)
{
    BAPE_OutputPort output;
    bool needMclk = false;
    bool pllRequired = false;
    bool ncoRequired = false;
    BAPE_MixerHandle thisMixer;

    /* Do we even need an MCLK source? */
    BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Beginning MCLK selection", mixer ));

    for ( output = BLST_S_FIRST(&mixer->outputList);
          output != NULL;
          output = BLST_S_NEXT(output, node) )
    {
        BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);
        if ( output->setMclk_isr || output->fsTiming )
        {
            needMclk = true;
            break;
        }
    }

    if ( false == needMclk )
    {
        /* No outputs need MCLK (all are DACs) */
        BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Done (no outputs need MCLK) Returning %s",
                                                     mixer, BAPE_Mixer_P_MclkSourceToText(BAPE_MclkSource_eNone )));
        return BAPE_MclkSource_eNone;
    }

    /* Check local outputs first */
    for ( output = BLST_S_FIRST(&mixer->outputList);
          output != NULL;
          output = BLST_S_NEXT(output, node) )
    {
        BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);
        if ( output->mclkOutput != BAPE_MclkSource_eNone )
        {
            BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Done (output provides MCLK) Returning %s",
                                                         mixer, BAPE_Mixer_P_MclkSourceToText(output->mclkOutput) ));
            return output->mclkOutput;  /* the mixer has a DAC output, use the DAC's rate manager */
        }
        if ( output->pllRequired )
        {
            pllRequired = true;
        }
        if ( output->builtinNco )
        {
            ncoRequired = true;
        }
    }

    /* At this point, the mixer has no outputs generating a usable clock source,
     * now see if there are any outputs that request a PLL (like Spdif or I2S), if so,
     * then try use the PLL specified in settings.outputPll.  If we have an output that 
     * requires an NCO (e.g. DAC on 7429) then skip this. 
     */
    if ( ncoRequired )
    {
        BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Outputs require an NCO clock", mixer ));
    }
    else if ( pllRequired )
    {
        BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Outputs desire a PLL clock", mixer ));
        /* If the outputPll mixer setting is valid, use it. */
        if ( (signed)mixer->settings.outputPll >= BAPE_Pll_e0 && mixer->settings.outputPll < BAPE_Pll_eMax )
        {
            unsigned pllIndex = mixer->settings.outputPll - BAPE_Pll_e0;
            if ( pllIndex < BAPE_CHIP_MAX_PLLS )
            {
                BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Done (valid PLL specified) Returning %s", mixer, BAPE_Mixer_P_MclkSourceToText(pllIndex + BAPE_MclkSource_ePll0)));
                return pllIndex + BAPE_MclkSource_ePll0;
            }
        }
        BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Valid PLL was not specified, continuing...", mixer ));
    }

    /* Now use an NCO if one was specified. */
    #if BAPE_CHIP_MAX_NCOS > 0
        if ( (signed)mixer->settings.outputNco >= BAPE_Nco_e0 && mixer->settings.outputNco < BAPE_Nco_eMax )
        {
            unsigned ncoIndex = mixer->settings.outputNco - BAPE_Nco_e0;
            if ( ncoIndex < BAPE_CHIP_MAX_NCOS )
            {
                BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Done (valid NCO specified) Returning %s", mixer, BAPE_Mixer_P_MclkSourceToText(ncoIndex + BAPE_MclkSource_eNco0) ));
                return ncoIndex + BAPE_MclkSource_eNco0;
            }
        }
    #endif /* BAPE_CHIP_MAX_NCOS */

    if ( ncoRequired )
    {
        BDBG_ERR(("Mixer %p requires an NCO but one has not been specified.", mixer));
        return BAPE_MclkSource_eMax;
    }
    else
    {
        BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Valid NCO was not specified, continuing...", mixer ));
    }

    BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Peeking for DAC on another mixer", mixer ));
    /* Check all open mixers with matching inputs and settings  */
    for ( thisMixer = BLST_S_FIRST(&mixer->deviceHandle->mixerList);
          thisMixer != NULL;
          thisMixer = BLST_S_NEXT(thisMixer, node) )
    {
        int i, j;
        bool matchInput=true, matchSettings=false;

        if ( thisMixer == mixer )
        {
            continue;
        }

        /* Determine if we have a matching timing or not */
        if ( mixer->master )
        {
            /* Must have the same master channel, timebase, and default sample rate */
            if ( thisMixer->master && 
                 (thisMixer->master->pParent == mixer->master->pParent) &&
                 (thisMixer->settings.outputTimebase == mixer->settings.outputTimebase) &&
                 (thisMixer->settings.defaultSampleRate == mixer->settings.defaultSampleRate) &&
                 (thisMixer->settings.defaultSampleRate == mixer->settings.mixerSampleRate) )
            {
                matchSettings = true;
            }
        }
        else
        {
            /* No master, just default timing. */
            if ( NULL == thisMixer->master &&
                 (thisMixer->settings.outputTimebase == mixer->settings.outputTimebase) &&
                 (thisMixer->settings.defaultSampleRate == mixer->settings.defaultSampleRate) &&
                 (thisMixer->settings.defaultSampleRate == mixer->settings.mixerSampleRate) )
            {
                matchSettings = true;
            }
        }

        /* Must match all inputs */
        for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
        {
            if ( mixer->inputs[i] )
            {
                for ( j = 0; j < BAPE_CHIP_MAX_MIXER_INPUTS; j++ )
                {
                    if ( thisMixer->inputs[j] &&
                         (thisMixer->inputs[j]->pParent == mixer->inputs[i]->pParent) )
                    {
                        break;
                    }
                }
                if ( j >= BAPE_CHIP_MAX_MIXER_INPUTS )
                {
                    matchInput = false;
                    break;
                }
            }
            if ( thisMixer->inputs[i] )
            {
                for ( j = 0; j < BAPE_CHIP_MAX_MIXER_INPUTS; j++ )
                {
                    if ( mixer->inputs[j] &&
                         (mixer->inputs[j]->pParent == thisMixer->inputs[i]->pParent) )
                    {
                        break;
                    }
                }
                if ( j >= BAPE_CHIP_MAX_MIXER_INPUTS )
                {
                    matchInput = false;
                    break;
                }
            }
        }

        /* If we've matched settings and input(s), look for an mclk source */
        if ( matchSettings && matchInput )
        {
            for ( output = BLST_S_FIRST(&thisMixer->outputList);
                  output != NULL;
                  output = BLST_S_NEXT(output, node) )
            {
                BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);
                if ( output->mclkOutput != BAPE_MclkSource_eNone )
                {
                    BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p (Found a DAC RM on another mixer) Returning %s", 
                                                                 mixer, BAPE_Mixer_P_MclkSourceToText(output->mclkOutput)));
                    BDBG_MSG(("Found associated MCLK source %s", BAPE_Mixer_P_MclkSourceToText(output->mclkOutput)));
                    return output->mclkOutput;
                }
            }
        }
    }
    BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Couldn't find usable DAC on another mixer", mixer ));


    /* If we get here, we haven't found one.  Return the PLL if
     * one was specified, otherwise just fail.
     */
    if ( (signed)mixer->settings.outputPll >= BAPE_Pll_e0 && mixer->settings.outputPll < BAPE_Pll_eMax )
    {
        unsigned pllIndex = mixer->settings.outputPll - BAPE_Pll_e0;
        if ( pllIndex < BAPE_CHIP_MAX_PLLS )
        {
            BDBG_MODULE_MSG(bape_mixer_get_mclk_source, ("Mixer:%p Done (last resort, using specified PLL) Returning %s", mixer, BAPE_Mixer_P_MclkSourceToText(pllIndex + BAPE_MclkSource_ePll0)));
            return pllIndex + BAPE_MclkSource_ePll0;
        }
    }

    BDBG_ERR(("Can't find MCLK source for mixer:%p, giving up.", mixer ));
    
    return BERR_TRACE(BAPE_MclkSource_eMax);    /* this indicates failure */
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_AllocateResources(BAPE_MixerHandle mixer)
{
    unsigned i;
    BAPE_OutputPort output;
    BAPE_MclkSource mclkSource;
    unsigned numOutputs=0;
    bool fsRequired = false;
    BERR_Code errCode;
    BAPE_PathConnection *pConnection;
    BAPE_DataType outputDataType;
    BAPE_FMT_Descriptor newFormat;

    BDBG_OBJECT_ASSERT(mixer, BAPE_Mixer);

    if ( mixer->resourcesAllocated )
    {
        BDBG_MSG(("Mixer %p (%d) already has resources allocated", mixer, mixer->index));
        return BERR_SUCCESS;
    }

    output = BLST_S_FIRST(&mixer->outputList);
    if ( NULL == output && 0 == mixer->numOutputConnections )
    {
        BDBG_ERR(("No outputs or nodes connected to mixer %p (%d), cannot allocate resources.", mixer, mixer->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Determine output format based on widest input format */
    errCode = BAPE_Mixer_P_DetermineOutputDataType(mixer, &outputDataType);
    if ( errCode )
    {
        BDBG_ERR(("Unable to determine mixer ouptut data format."));
        return BERR_TRACE(errCode);
    }

    BAPE_Connector_P_GetFormat(&mixer->pathNode.connectors[0], &newFormat);
    newFormat.type = outputDataType;

    /* Make sure all outputs are capable of receiving this data format */
    for ( output = BLST_S_FIRST(&mixer->outputList);
          output != NULL;
          output = BLST_S_NEXT(output, node) )
    {
        BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);
        if ( !BAPE_FMT_P_FormatSupported(&output->capabilities, &newFormat) )
        {
            BDBG_ERR(("Output %s can not accept a data type of %s", output->pName, BAPE_FMT_P_GetTypeName(&newFormat)));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        numOutputs++;
    }

    /* Update output format and propagate it */
    errCode = BAPE_Connector_P_SetFormat(&mixer->pathNode.connectors[0], &newFormat);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BDBG_MSG(("Mixer %p (%d): Allocating mixers for %u outputs using data format %s.", 
              mixer, mixer->index, numOutputs, BAPE_FMT_P_GetTypeName(&newFormat)));

    /* This allocates the required number of mixers */
    errCode = BAPE_StandardMixer_P_AllocateMixerGroups(mixer);
    if ( errCode )
    {
        BDBG_ERR(("Unable to allocate sufficient hardware mixers.  Please reduce the number of outputs and/or the number of output channels."));
        errCode = BERR_TRACE(errCode);
        goto err_mixers;
    }
    
    /* Now, we have the resources we need.  Setup the linkage correctly. */   
    i=0;
    for ( output = BLST_S_FIRST(&mixer->outputList);
          output != NULL;
          output = BLST_S_NEXT(output, node) )
    {
        /* First output always to master */
        output->sourceMixerGroup = mixer->mixerGroups[i];
        output->sourceMixerOutputIndex = 0;
        BAPE_MixerGroup_P_GetOutputFciIds(mixer->mixerGroups[i], 0, &output->sourceMixerFci);

        i++;
    }
    
    /* Setup Downstream connections as well */
    for ( pConnection = BLST_SQ_FIRST(&mixer->pathNode.connectors[0].connectionList);
          pConnection != NULL;
          pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
    {
        BAPE_MixerGroup_P_GetOutputFciIds(mixer->mixerGroups[i], 0, &pConnection->inputFciGroup);
        i++;
    }
    
    /* Refresh output scaling */    
    for ( output = BLST_S_FIRST(&mixer->outputList);
          output != NULL;
          output = BLST_S_NEXT(output, node) )
    {
        BDBG_MSG(("Refreshing output scaling for output %p", output));
        BAPE_StandardMixer_P_ApplyOutputVolume(mixer, output);
    }
    
    mclkSource = BAPE_StandardMixer_P_GetMclkSource(mixer);
    if ( mclkSource >= BAPE_MclkSource_eMax )
    {
        BDBG_ERR(("Unable to find MclkSource for mixer."));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_mixer_mclksource;
    }

    mixer->mclkSource = mclkSource;
    if (BAPE_MCLKSOURCE_IS_PLL(mclkSource))
    {
        #if BAPE_CHIP_MAX_PLLS > 0
        BAPE_P_AttachMixerToPll(mixer, mixer->mclkSource - BAPE_MclkSource_ePll0);
        #endif
    }
    else if (BAPE_MCLKSOURCE_IS_NCO(mclkSource))
    {
        #if BAPE_CHIP_MAX_NCOS > 0
            BAPE_P_AttachMixerToNco(mixer, mixer->mclkSource - BAPE_MclkSource_eNco0);
        #endif
    }
    else  /* Must be a DAC or none. */
    {
        BKNI_EnterCriticalSection();
        /* Set outputs to DAC rate manager MCLK if required */
        for ( output = BLST_S_FIRST(&mixer->outputList);
              output != NULL;
              output = BLST_S_NEXT(output, node) )
        {
            if ( output->setMclk_isr )
            {
                output->setMclk_isr(output, mixer->mclkSource, 0, 256 /* DAC is 256*Fs */); /* Actually, at Fs > 48 KHz the DAC runs at 64*Fs */
            }
        }
        BKNI_LeaveCriticalSection();
    }

    /* Determine if we need an Fs or not */
    for ( output = BLST_S_FIRST(&mixer->outputList);
          output != NULL;
          output = BLST_S_NEXT(output, node) )
    {
        if ( output->fsTiming )
        {
            fsRequired = true;
            break;
        }
    }

    /* Try and grab an Fs */
    if ( fsRequired )
    {
        mixer->fs = BAPE_P_AllocateFs(mixer->deviceHandle);
        if ( mixer->fs == BAPE_FS_INVALID )
        {
            BDBG_ERR(("Unable to allocate fs timing source"));
            errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
            goto err_alloc_fs;
        }
        if ( BAPE_MCLKSOURCE_IS_PLL(mclkSource) )
        {
            BKNI_EnterCriticalSection();
            errCode = BAPE_P_UpdatePll_isr(mixer->deviceHandle, mixer->settings.outputPll);
            BKNI_LeaveCriticalSection();
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_alloc_fs;
            }
        }
        #if BAPE_CHIP_MAX_NCOS > 0
        else if ( BAPE_MCLKSOURCE_IS_NCO(mclkSource) )
        {
            /* Update the NCO */
            BKNI_EnterCriticalSection();
            errCode = BAPE_P_UpdateNco_isr(mixer->deviceHandle, mixer->settings.outputNco);
            BKNI_LeaveCriticalSection();
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_alloc_fs;
            }
        }
        #endif
        else
        {
            /* Set the FS to use DAC timing */
            BKNI_EnterCriticalSection();
            BAPE_P_SetFsTiming_isr(mixer->deviceHandle, 
                                   mixer->fs, 
                                   mixer->mclkSource, 
                                   0,       /* pllChan doesn't apply */
                                   256 );   /* DAC is 256Fs */
            BKNI_LeaveCriticalSection();
        }   
        /* Bind each output to the fs */     
        for ( output = BLST_S_FIRST(&mixer->outputList);
              output != NULL;
              output = BLST_S_NEXT(output, node) )
        {
            if ( output->fsTiming )
            {
                BDBG_ASSERT(NULL != output->setFs);
                output->setFs(output, mixer->fs);
            }
        }
    }

    mixer->resourcesAllocated = true;
    return BERR_SUCCESS;

err_alloc_fs:
    if ( mixer->fs != BAPE_FS_INVALID )
    {
        BAPE_P_FreeFs(mixer->deviceHandle, mixer->fs);
        mixer->fs = BAPE_FS_INVALID;
    }

err_mixer_mclksource:
    BAPE_StandardMixer_P_FreeResources(mixer);
    
err_mixers:
    return errCode;
}

/*************************************************************************/
static void BAPE_StandardMixer_P_SetSampleRate_isr(BAPE_MixerHandle mixer, unsigned sampleRate)
{
    BDBG_OBJECT_ASSERT(mixer, BAPE_Mixer);

    /* Only do this if something actually changed */
    BDBG_MSG(("Set Mixer Output Rate to %u (was %u)", sampleRate, BAPE_Mixer_P_GetOutputSampleRate(mixer)));
    if ( BAPE_Mixer_P_GetOutputSampleRate(mixer) != sampleRate )
    {
        unsigned i;
        BAPE_OutputPort output;

        if ( sampleRate == 0 )

        BDBG_MSG(("Changing mixer %p (%d) sample rate to %u [was %u]", mixer, mixer->index, sampleRate, BAPE_Mixer_P_GetOutputSampleRate(mixer)));

        (void)BAPE_Connector_P_SetSampleRate_isr(&mixer->pathNode.connectors[0], sampleRate);

        for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
        {
            if ( mixer->inputs[i] )
            {
                BAPE_StandardMixer_P_SetInputSRC_isr(mixer, mixer->inputs[i], mixer->inputs[i]->format.sampleRate, sampleRate);
            }
        }

        /* For each output... */
        for ( output = BLST_S_FIRST(&mixer->outputList);
              output != NULL;
              output = BLST_S_NEXT(output, node) )
        {
            if ( output->setTimingParams_isr )
            {
                output->setTimingParams_isr(output, sampleRate, mixer->settings.outputTimebase);
            }
        }

        #if BAPE_CHIP_MAX_PLLS > 0
        if ( BAPE_MCLKSOURCE_IS_PLL(mixer->mclkSource) && sampleRate != 0 )
        {
            BERR_Code errCode;
            /* Update the PLL */
            errCode = BAPE_P_UpdatePll_isr(mixer->deviceHandle, mixer->settings.outputPll);
            BDBG_ASSERT(errCode == BERR_SUCCESS);
        }
        #endif

        #if BAPE_CHIP_MAX_NCOS > 0
        if ( BAPE_MCLKSOURCE_IS_NCO(mixer->mclkSource) && sampleRate != 0 )
        {
            BERR_Code errCode;
            /* Update the NCO */
            errCode = BAPE_P_UpdateNco_isr(mixer->deviceHandle, mixer->settings.outputNco);
            BDBG_ASSERT(errCode == BERR_SUCCESS);
        }
        #endif
    }
    else
        {            
        BDBG_MSG(("NOT Changing mixer %p (%d) sample rate to %u [currently %u]", mixer, mixer->index, sampleRate, BAPE_Mixer_P_GetOutputSampleRate(mixer)));
    }
}

/*************************************************************************/
static void BAPE_StandardMixer_P_SetInputSRC_isr(BAPE_MixerHandle mixer, BAPE_Connector input, unsigned inputRate, unsigned outputRate)
{
    BAPE_PathConnection *pLink=NULL;

    BDBG_OBJECT_ASSERT(mixer, BAPE_Mixer);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);

    /* Find connection node for this object */
    for ( pLink = BLST_SQ_FIRST(&input->connectionList);
          NULL != pLink;
          pLink = BLST_SQ_NEXT(pLink, downstreamNode) )
    {
        if ( pLink->pSink == &mixer->pathNode )
            break;
    }
    BDBG_ASSERT(NULL != pLink);

    if ( pLink->srcGroup )
    {
        BDBG_MSG(("Setting input %s SRC to %u->%u", input->pName, inputRate, outputRate));
        BAPE_SrcGroup_P_SetSampleRate_isr(pLink->srcGroup, inputRate, outputRate);
    }
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_AllocateConnectionResources(BAPE_MixerHandle handle, BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    bool sfifoRequired=false, srcRequired=false, buffersOnly=false;

    BAPE_Connector input = pConnection->pSource;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(&handle->pathNode == pConnection->pSink);

    switch ( pConnection->pSource->format.source )
    {
    case BAPE_DataSource_eDfifo:
    case BAPE_DataSource_eDspBuffer:
    case BAPE_DataSource_eHostBuffer:
        /* SFIFO is required - also always allocate SRC since it handles startup ramp and input mute */
        sfifoRequired = true;
        if ( pConnection->pSource->format.type != BAPE_DataType_eIec61937x16 )
        {
            srcRequired = true;     /* Omit for HBR sources, only one sample rate is valid for HBR */
        }
        break;
    case BAPE_DataSource_eFci:          
        /* TODO: Many cases could require an SRC - omit now to save HW resources for EQ */
        return BERR_SUCCESS;
    default:
        BDBG_ERR(("Unrecognized source type %u", pConnection->pSource->format.source));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    
    BDBG_MSG(("Mixer %p Allocating resources for input %s %s", handle, input->pParent->pName, input->pName));

    /* Check if we've already allocated resources */
    if ( pConnection->resourcesAllocated )
    {
        if ( (pConnection->format.type == input->format.type) &&
             (pConnection->format.ppmCorrection == input->format.ppmCorrection) )
        {
            BDBG_MSG(("Connection %s -> %s %s already has the required resources", input->pParent->pName, pConnection->pSink->pName, input->pName));
            if ( sfifoRequired )
            {
                BDBG_ASSERT(NULL != pConnection->sfifoGroup);
            }
            if ( srcRequired )
            {
                BDBG_ASSERT(NULL != pConnection->srcGroup);
            }

            buffersOnly = true;
        }
        else
        {
            /* Connection format has changed */
            BDBG_MSG(("Connection %s -> %s %s format changed.  Releasing resources.", input->pParent->pName, pConnection->pSink->pName, input->pName));
            BAPE_StandardMixer_P_FreeConnectionResources(handle, pConnection);
        }
    }

    if ( input->useBufferPool )
    {
        /* This is safe to call multiple times, it only allocates if need be */
        errCode = BAPE_P_AllocateInputBuffers(handle->deviceHandle, input);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    if ( buffersOnly )
    {
        /* We don't need to re-add the remaining resources */
        return BERR_SUCCESS;
    }

    BDBG_ASSERT(NULL == pConnection->sfifoGroup);
    BDBG_ASSERT(NULL == pConnection->srcGroup);

    if ( sfifoRequired )
    {
        BAPE_SfifoGroupCreateSettings sfifoCreateSettings;
        BAPE_SfifoGroup_P_GetDefaultCreateSettings(&sfifoCreateSettings);
        sfifoCreateSettings.numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&input->format);
        sfifoCreateSettings.ppmCorrection = input->format.ppmCorrection;
        errCode = BAPE_SfifoGroup_P_Create(handle->deviceHandle, &sfifoCreateSettings, &pConnection->sfifoGroup);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_alloc_sfifo;
        }
    }

    if ( srcRequired )
    {
        BAPE_SrcGroupSettings srcSettings;
        BAPE_SrcGroupCreateSettings srcCreateSettings;
        BAPE_SrcGroup_P_GetDefaultCreateSettings(&srcCreateSettings);
        srcCreateSettings.numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&input->format);
        errCode = BAPE_SrcGroup_P_Create(handle->deviceHandle, &srcCreateSettings, &pConnection->srcGroup);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_alloc_src;
        }
        /* Set SRC Linkage */
        BAPE_SrcGroup_P_GetSettings(pConnection->srcGroup, &srcSettings);
        if ( pConnection->sfifoGroup )
        {
            /* Input From Sfifo */
            BAPE_SfifoGroup_P_GetOutputFciIds(pConnection->sfifoGroup, &srcSettings.input);
        }
        else
        {
            /* Input from other FCI source */
            srcSettings.input = pConnection->inputFciGroup;
        }
        srcSettings.rampEnabled = BAPE_FMT_P_RampingValid(&input->format);
        srcSettings.startupRampEnabled = srcSettings.rampEnabled;
        errCode = BAPE_SrcGroup_P_SetSettings(pConnection->srcGroup, &srcSettings);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_src_settings;
        }
        BKNI_EnterCriticalSection();
        BAPE_SrcGroup_P_SetSampleRate_isr(pConnection->srcGroup, pConnection->format.sampleRate, BAPE_Mixer_P_GetOutputSampleRate(handle));
        BKNI_LeaveCriticalSection();
    }

    pConnection->format = input->format;
    pConnection->resourcesAllocated = true;

    return BERR_SUCCESS;

    err_src_settings:
    if ( pConnection->srcGroup )
    {
        BAPE_SrcGroup_P_Destroy(pConnection->srcGroup);
        pConnection->srcGroup = NULL;
    }
    err_alloc_src:
    if ( pConnection->sfifoGroup )
    {
        BAPE_SfifoGroup_P_Destroy(pConnection->sfifoGroup);
        pConnection->sfifoGroup = NULL;
    }
    err_alloc_sfifo:
    BAPE_P_FreeInputBuffers(handle->deviceHandle, pConnection->pSource);
    return errCode;
}

/*************************************************************************/
static void BAPE_StandardMixer_P_FreeConnectionResources(BAPE_MixerHandle handle, BAPE_PathConnection *pConnection)
{
    BDBG_MSG(("Freeing connection resources for input %s %s", pConnection->pSource->pParent->pName, pConnection->pSource->pName));
    if ( pConnection->pSource->useBufferPool )
    {
        BAPE_P_FreeInputBuffers(handle->deviceHandle, pConnection->pSource);
    }
    if ( pConnection->srcGroup )
    {
        BAPE_SrcGroup_P_Destroy(pConnection->srcGroup);
        pConnection->srcGroup = NULL;
    }
    if ( pConnection->sfifoGroup )
    {
        BAPE_SfifoGroup_P_Destroy(pConnection->sfifoGroup);
        pConnection->sfifoGroup = NULL;
    }
    pConnection->resourcesAllocated = false;
}

/*************************************************************************/
static void BAPE_StandardMixer_P_FreeResources(BAPE_MixerHandle handle)
{
    unsigned i;
    BAPE_PathConnection *pConnection;

    BDBG_MSG(("Mixer %d (%#x) Free Resources", handle->index, handle));

    if ( handle->running )
    {
        BDBG_ERR(("Can't release resources while mixer %p (%d) is running", handle, handle->index));
        BDBG_ASSERT(!handle->running);
        return;
    }

    if ( !handle->resourcesAllocated )
    {
        /* Nothing to do */
        return;
    }

#if BAPE_CHIP_MAX_PLLS > 0
    if ( BAPE_MCLKSOURCE_IS_PLL(handle->mclkSource))
    {
        BAPE_P_DetachMixerFromPll(handle, handle->mclkSource - BAPE_MclkSource_ePll0);
    }
#endif

#if BAPE_CHIP_MAX_NCOS > 0
    if ( BAPE_MCLKSOURCE_IS_NCO(handle->mclkSource))
    {
        BAPE_P_DetachMixerFromNco(handle, handle->mclkSource - BAPE_MclkSource_eNco0);
    }
#endif
        handle->mclkSource = BAPE_MclkSource_eNone;

    if ( handle->fs != BAPE_FS_INVALID )
    {
        BAPE_P_FreeFs(handle->deviceHandle, handle->fs);
        handle->fs = BAPE_FS_INVALID;
    }

    /* Invalidate Downstream connections as well */
    for ( pConnection = BLST_SQ_FIRST(&handle->pathNode.connectors[0].connectionList);
          pConnection != NULL;
          pConnection = BLST_SQ_NEXT(pConnection, downstreamNode) )
    {
        BAPE_FciIdGroup_Init(&pConnection->inputFciGroup);
    }

    for ( i = 0; i < handle->numMixerGroups; i++ )
    {
        BDBG_ASSERT(NULL != handle->mixerGroups[i]);
        BAPE_MixerGroup_P_Destroy(handle->mixerGroups[i]);
        handle->mixerGroups[i] = NULL;
    }
    handle->numMixerGroups = 0;

    handle->resourcesAllocated = false;    
}

/*************************************************************************/
static void BAPE_StandardMixer_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_StandardMixer_P_RemoveInput(pNode->pHandle, pConnector);
}


/*************************************************************************/
static BERR_Code BAPE_StandardMixer_P_AllocateMixerGroups(BAPE_MixerHandle handle)
{
    BERR_Code errCode=BERR_SUCCESS;
    BAPE_MixerGroupCreateSettings createSettings;
    unsigned i, numOutputs;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

#if BDBG_DEBUG_BUILD
    /* Sanity check, make sure all resources are marked invalid in the mixer */
    for ( i = 0; i < BAPE_CHIP_MAX_MIXERS; i++ )
    {
        BDBG_ASSERT(NULL == handle->mixerGroups[i]);
    }
    BDBG_ASSERT(handle->numMixerGroups == 0);
#endif

    BAPE_MixerGroup_P_GetDefaultCreateSettings(&createSettings);
    createSettings.numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(BAPE_Mixer_P_GetOutputFormat(handle));
    numOutputs = handle->numOutputs + handle->numOutputConnections;
    /* TODO: Can compact this into fewer mixers or possibly use cascading again. */
    for ( i = 0; i < numOutputs; i++ )
    {
        errCode = BAPE_MixerGroup_P_Create(handle->deviceHandle, &createSettings, &handle->mixerGroups[i]);
        if ( errCode )
        {
            goto err_create_group;
        }
        handle->numMixerGroups++;
    }

    return BERR_SUCCESS;

    err_create_group:
    for ( i = 0; i < handle->numMixerGroups; i++ )
    {
        BAPE_MixerGroup_P_Destroy(handle->mixerGroups[i]);
        handle->mixerGroups[i] = NULL;
    }
    handle->numMixerGroups = 0;

    return errCode;        
}

/*************************************************************************/
/* Declare the interface struct containing the API callbacks */

#define NOT_SUPPORTED(x) NULL
/* If any of the APIs are not supported by a particular type of mixer,
 * just add a placeholder like this:
 *   NOT_SUPPORTED(BAPE_StandardMixer_P_RemoveAllInputs)
 */

static const BAPE_MixerInterface  standardMixerInterface  = {
    BAPE_StandardMixer_P_Destroy,                /*       (*destroy)           */
    NOT_SUPPORTED(BAPE_StandardMixer_P_Start),   /*       (*start)             */
    NOT_SUPPORTED(BAPE_StandardMixer_P_Stop),    /*       (*stop)              */
    BAPE_StandardMixer_P_AddInput,               /*       (*addInput)          */
    BAPE_StandardMixer_P_RemoveInput,            /*       (*removeInput)       */
    BAPE_StandardMixer_P_RemoveAllInputs,        /*       (*removeAllInputs)   */
    BAPE_StandardMixer_P_AddOutput,              /*       (*addOutput)         */
    BAPE_StandardMixer_P_RemoveOutput,           /*       (*removeOutput)      */
    BAPE_StandardMixer_P_RemoveAllOutputs,       /*       (*removeAllOutputs)  */
    BAPE_StandardMixer_P_GetInputVolume,         /*       (*getInputVolume)    */
    BAPE_StandardMixer_P_SetInputVolume,         /*       (*setInputVolume)    */
    BAPE_StandardMixer_P_ApplyOutputVolume,      /*       (*applyOutputVolume) */
    BAPE_StandardMixer_P_SetSettings             /*       (*setSettings) */
};

