/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_mixer_dsp.c $
 * $brcm_Revision: Hydra_Software_Devel/28 $
 * $brcm_Date: 10/11/12 3:06a $
 *
 * Module Description: Mixer Interface for DSP Mixers
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_mixer_dsp.c $
 * 
 * Hydra_Software_Devel/28   10/11/12 3:06a jgarrett
 * SW7425-3831: Not allowing openGateAtStart for NRT or compressed cases
 * 
 * Hydra_Software_Devel/27   10/9/12 11:05p jgarrett
 * SW7425-3831: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-3831/3   10/9/12 10:48p jgarrett
 * SW7425-3831: Force sample rate updates on playback inputs
 * 
 * Hydra_Software_Devel/SW7425-3831/2   10/9/12 5:41a jgarrett
 * SW7425-3831: Merge latest updates
 * 
 * Hydra_Software_Devel/SW7425-3831/1   9/14/12 6:12p jgarrett
 * SW7425-3831: Adding programmable mixer output rate
 * 
 * Hydra_Software_Devel/26   6/15/12 1:07p jgarrett
 * SW7425-3252: Adding multiStreamBalance setting per dolby specification
 * 
 * Hydra_Software_Devel/25   5/23/12 2:42p jgarrett
 * SW7425-2075: Forcing PPM correction off when mixing with the DSP
 * 
 * Hydra_Software_Devel/24   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/2   5/3/12 3:46p jgarrett
 * SW7425-2075: Merging latest mainline changes
 * 
 * Hydra_Software_Devel/23   4/20/12 5:24p jgarrett
 * SW7231-774: Unwinding correctly if input start fails in bdsp
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:53p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/22   3/22/12 4:22p jgarrett
 * SW7425-2644: Ensuring output format is set properly with only a single
 * input
 * 
 * Hydra_Software_Devel/21   3/7/12 4:35p jgarrett
 * SW7435-24: Adding DSP index for decode and dsp mixer
 * 
 * Hydra_Software_Devel/20   12/2/11 3:19p jgarrett
 * SW7346-578: Adding ability to change some mixer settings without
 * destroying and re-creating the mixer
 * 
 * Hydra_Software_Devel/19   11/14/11 3:40p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/18   10/14/11 12:11p jgarrett
 * SW7425-468: Adding hooks for NRT DSP mixer support
 * 
 * Hydra_Software_Devel/17   9/28/11 2:48p jgarrett
 * SW7425-1119: Allowing compressed inputs to DSP mixer
 * 
 * Hydra_Software_Devel/16   8/25/11 6:27p gskerl
 * SW7425-1119: Finished adding BAPE_Mixer_Start() and BAPE_Mixer_Stop()
 * support for dspMixers.
 * 
 * Hydra_Software_Devel/15   8/23/11 1:43p gskerl
 * SW7425-406: Corrected code that determines whether an FS is needed when
 * there are multiple mixer inputs. Added code to propagate the sample
 * rate to downstream nodes.
 * 
 * Hydra_Software_Devel/14   8/23/11 1:24p gskerl
 * SW7425-1119: Added prototypes and stubs for BAPE_Mixer_Start() and
 * BAPE_Mixer_Stop(). For now, they just return BERR_NOT_SUPPORTED
 * 
 * Hydra_Software_Devel/13   8/22/11 10:34a jgarrett
 * SWDTV-8271: Changing timebase field name
 * 
 * Hydra_Software_Devel/12   7/27/11 4:17p gskerl
 * SW7425-884: Corrected arguments being passed to
 * BAPE_P_DetachMixerFromPll() and BAPE_P_DetachMixerFromNco
 * 
 * Hydra_Software_Devel/11   7/8/11 4:53p gskerl
 * SW7425-321: Added printing of downstream nodes when an audio path is
 * started (if bape_startprint is enabled)
 * 
 * Hydra_Software_Devel/10   7/8/11 4:31p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/9   6/28/11 10:33a jgarrett
 * SW7422-406: Setting input volume for DSP mixers
 * 
 * Hydra_Software_Devel/8   6/21/11 9:22a jgarrett
 * SW7425-406: Coverity CID 443
 * 
 * Hydra_Software_Devel/7   6/20/11 7:02p jgarrett
 * SW7425-406: Adding input scaling coefficients to mixer
 * 
 * Hydra_Software_Devel/6   6/16/11 11:03a gskerl
 * SW7425-321: Added call to BAPE_Mixer_P_PrintMixers() just before
 * starting paths
 * 
 * Hydra_Software_Devel/5   6/15/11 6:27p jgarrett
 * SW7425-406: Coverity CID 437
 * 
 * Hydra_Software_Devel/4   6/14/11 3:46p jgarrett
 * SW7425-406: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-406/4   6/13/11 7:27p jgarrett
 * SW7425-406: Updates for new BDSP_Task_AddStageInput APIs
 * 
 * Hydra_Software_Devel/SW7425-406/3   6/8/11 6:55p jgarrett
 * SW7425-406: Adding support for multichannel and playback inputs to DSP
 * mixer
 * 
 * Hydra_Software_Devel/SW7425-406/2   6/6/11 8:53a jgarrett
 * SW7425-406: Fixing inter-task buffer logic
 * 
 * Hydra_Software_Devel/SW7425-406/1   6/3/11 6:42p jgarrett
 * SW7425-406: Initial working version
 * 
 * Hydra_Software_Devel/3   6/2/11 2:12p jgarrett
 * SW7425-406: Removing feedback buffer
 * 
 * Hydra_Software_Devel/2   6/1/11 6:31p jgarrett
 * SW7425-406: Adding additional DSP inter-task resource managment
 * 
 * Hydra_Software_Devel/1   5/31/11 6:37p jgarrett
 * SW7425-406: Adding initial DSP mixer code
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_mixer_dsp);

/* Mixer Interface */
static const BAPE_MixerInterface  g_dspMixerInterface;

/* Node callbacks */
static BERR_Code BAPE_DspMixer_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void      BAPE_DspMixer_P_FreePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_DspMixer_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_DspMixer_P_StartPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void      BAPE_DspMixer_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void      BAPE_DspMixer_P_InputSampleRateChange_isr(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, unsigned sampleRate);
static BERR_Code BAPE_DspMixer_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat);
static void      BAPE_DspMixer_P_InputMute(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, bool enabled);
static void      BAPE_DspMixer_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

/* Local Routines */
static BERR_Code BAPE_DspMixer_P_RemoveAllInputs(BAPE_MixerHandle handle);
static BERR_Code BAPE_DspMixer_P_StartTask(BAPE_MixerHandle handle);
static void BAPE_DspMixer_P_StopTask(BAPE_MixerHandle handle);
static void BAPE_DspMixer_P_CleanupTaskCreateSettings(BAPE_MixerHandle handle);
static void BAPE_DspMixer_P_FreeConnectionResources(BAPE_MixerHandle handle, BAPE_PathConnection *pConnection);
static BERR_Code BAPE_DspMixer_P_AllocateLoopbackInput(BAPE_MixerHandle handle, BAPE_PathConnection *pConnection);
static void BAPE_DspMixer_P_SetSampleRate_isr(BAPE_MixerHandle mixer, unsigned sampleRate);
static void BAPE_DspMixer_P_SetInputSRC_isr(BAPE_MixerHandle mixer, BAPE_Connector input, unsigned inputRate, unsigned outputRate);
static BERR_Code BAPE_DspMixer_P_ApplyInputVolume(BAPE_MixerHandle mixer, unsigned index);

BERR_Code BAPE_DspMixer_P_Create(
    BAPE_Handle deviceHandle,
    const BAPE_MixerSettings *pSettings,
    BAPE_MixerHandle *pHandle               /* [out] */
    )
{
    BERR_Code errCode;
    BAPE_MixerHandle handle;
    BAPE_MixerSettings defaultSettings;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);

    *pHandle = NULL;

    if ( NULL == pSettings )
    {
        BAPE_Mixer_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    if ( NULL == deviceHandle->dspContext )
    {
        BDBG_ERR(("DSP support is not available.  Can't create a DSP mixer."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( pSettings->dspIndex >= deviceHandle->numDsps )
    {
        BDBG_ERR(("DSP %u is not available.  This system has %u DSPs.", pSettings->dspIndex, deviceHandle->numDsps));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( pSettings->outputDelay )
    {
        BDBG_WRN(("Output delay is not supported for DSP mixers.  Ignoring outputDelay value."));
    }

    handle = BKNI_Malloc(sizeof(BAPE_Mixer));
    if ( NULL == handle )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_handle;
    }

    BKNI_Memset(handle, 0, sizeof(BAPE_Mixer));
    BDBG_OBJECT_SET(handle, BAPE_Mixer);
    handle->settings = *pSettings;
    handle->interface = &g_dspMixerInterface;
    handle->deviceHandle = deviceHandle;
    BLST_S_INSERT_HEAD(&deviceHandle->mixerList, handle, node);
    handle->fs = BAPE_FS_INVALID;
    BAPE_P_InitPathNode(&handle->pathNode, BAPE_PathNodeType_eMixer, handle->settings.type, 1, deviceHandle, handle);
    handle->pathNode.subtype = BAPE_MixerType_eDsp;
    handle->pathNode.pName = "DSP Mixer";

    handle->pathNode.connectors[0].useBufferPool = true;
    BAPE_Connector_P_GetFormat(&handle->pathNode.connectors[0], &format);
    format.sampleRate = 0;
    format.source = BAPE_DataSource_eDspBuffer;
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
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937x4);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->pathNode, &caps);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_caps; }

    /* Fill in node connection routines */
    handle->pathNode.allocatePathFromInput = BAPE_DspMixer_P_AllocatePathFromInput;
    handle->pathNode.freePathFromInput = BAPE_DspMixer_P_FreePathFromInput;
    handle->pathNode.configPathFromInput = BAPE_DspMixer_P_ConfigPathFromInput;
    handle->pathNode.startPathFromInput = BAPE_DspMixer_P_StartPathFromInput;
    handle->pathNode.stopPathFromInput = BAPE_DspMixer_P_StopPathFromInput;

    /* Generic Routines for DSP nodes */
    handle->pathNode.allocatePathToOutput = BAPE_DSP_P_AllocatePathToOutput;
    handle->pathNode.configPathToOutput = BAPE_DSP_P_ConfigPathToOutput;
    handle->pathNode.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;
    handle->pathNode.startPathToOutput = BAPE_DSP_P_StartPathToOutput;

    /* Other misc. node routines for mixers */
    handle->pathNode.inputSampleRateChange_isr = BAPE_DspMixer_P_InputSampleRateChange_isr;
    handle->pathNode.inputFormatChange = BAPE_DspMixer_P_InputFormatChange;
    handle->pathNode.inputMute = BAPE_DspMixer_P_InputMute;
    handle->pathNode.removeInput = BAPE_DspMixer_P_RemoveInputCallback;

    handle->pPrimaryBranch = BKNI_Malloc(sizeof(BDSP_CIT_P_FwBranchInfo));
    if ( NULL == handle->pPrimaryBranch )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_branch;
    }

    *pHandle = handle;

    return BERR_SUCCESS;

    BKNI_Free(handle->pPrimaryBranch);
err_branch:
err_caps:
err_format:
    BLST_S_REMOVE(&handle->deviceHandle->mixerList, handle, BAPE_Mixer, node);
    BDBG_OBJECT_DESTROY(handle, BAPE_Mixer);
    BKNI_Free(handle);
err_handle:
    return errCode;
}

static void BAPE_DspMixer_P_Destroy(
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
    BAPE_DspMixer_P_RemoveAllInputs(handle);

    /* Unlink from device */
    BLST_S_REMOVE(&handle->deviceHandle->mixerList, handle, BAPE_Mixer, node);

    /* Free Allocations */
    BKNI_Free(handle->pPrimaryBranch);

    /* Destroy Handle */
    BDBG_OBJECT_DESTROY(handle, BAPE_Mixer);
    BKNI_Free(handle);
}

static BERR_Code BAPE_DspMixer_P_AddInput(
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

    if ( makeMaster && input->format.source != BAPE_DataSource_eDspBuffer )
    {
        BDBG_ERR(("Only DSP inputs can be treated as master for DSP mixers"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Find empty slot */
    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
    {
        if ( handle->inputs[i] == NULL )
        {
            handle->inputs[i] = input;
            BDBG_MSG(("Adding input %s %s to DSP mixer %u as %s", input->pParent->pName, input->pName, handle->index, makeMaster?"master":"slave"));
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

static BERR_Code BAPE_DspMixer_P_RemoveInput(
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
    BDBG_ERR(("Input %p is not connected to DSP mixer %p", input, handle));
    return BERR_TRACE(BERR_INVALID_PARAMETER);
}

static BERR_Code BAPE_DspMixer_P_RemoveAllInputs(
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
            errCode = BAPE_DspMixer_P_RemoveInput(handle, handle->inputs[i]);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_DspMixer_P_Start(BAPE_MixerHandle handle)
{
    BERR_Code errCode;
    BAPE_PathNode *pNode;
    unsigned numFound;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    if ( handle->startedExplicitly )
    {
        BDBG_ERR(("Mixer %p (%d) has already been explicity started.  Can't start.", handle, handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Look for MuxOutput */
    BAPE_PathNode_P_FindConsumersByType(&handle->pathNode, BAPE_PathNodeType_eMuxOutput, 1, &numFound, &pNode);
    if ( numFound > 0 )
    {
        BAPE_MuxOutputHandle hMuxOutput = (BAPE_MuxOutputHandle)pNode->pHandle;
        if ( !BAPE_MuxOutput_P_IsRunning(hMuxOutput) )
        {
            BDBG_ERR(("When explicitly starting a mixer for transcode operations, the MuxOutput object must be started first."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }

    handle->startedExplicitly = true;

    if ( NULL == handle->task )
    {
        BDBG_MSG(("BAPE_DspMixer_P_Start: Task not present.  Starting mixer task."));
        errCode = BAPE_DspMixer_P_StartTask(handle);
        if ( errCode )
        {
            handle->startedExplicitly = false;
            return BERR_TRACE(errCode);
        }
    }
    
    handle->running++;
        
    return BERR_SUCCESS;
}


static BERR_Code BAPE_DspMixer_P_StartTask(BAPE_MixerHandle handle)
{
    BERR_Code errCode;
    bool fsRequired=false;
    BAPE_PathConnection *pInputConnection;
    BAPE_PathNode *pNode;
    unsigned numFound, branchId, stageId, i;

    /* Scan inputs and determine if we need a FS and PLL linkage for loopback */
    for ( pInputConnection = BLST_S_FIRST(&handle->pathNode.upstreamList);
          pInputConnection != NULL;
          pInputConnection = BLST_S_NEXT(pInputConnection, upstreamNode) )
    {
        if ( pInputConnection->pSource->format.source != BAPE_DataSource_eDspBuffer )
        {
            fsRequired = true;
            break; 
        }
    }

    if ( fsRequired )
    {
        /* Loopback found, allocate an FS timing object */
#if BAPE_CHIP_MAX_FS > 0
        BDBG_ASSERT(handle->fs == BAPE_FS_INVALID);
        handle->fs = BAPE_P_AllocateFs(handle->deviceHandle);
        if ( handle->fs == BAPE_FS_INVALID )
        {
            BDBG_ERR(("Unable to allocate FS resources for input loopback."));
            errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
            goto err_fs;
        }
#endif
        /* Use PLL for now, other timing sources are not available here. */
        /* TODO: Allow for selection of NCO timing source... maybe even call
         * BAPE_Mixer_P_GetMclkSource(), after moving it from "standard"
         * mixer code to common mixer code. */ 
        handle->mclkSource = BAPE_MclkSource_ePll0 + handle->settings.outputPll;

        /* Connect to the appropriate PLL */
        #if BAPE_CHIP_MAX_PLLS > 0
        BAPE_P_AttachMixerToPll(handle, handle->settings.outputPll);
        #else
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_acquire_resources;
        #endif

        /* Set Loopback-FS linkage */
        for ( pInputConnection = BLST_S_FIRST(&handle->pathNode.upstreamList);
              pInputConnection != NULL;
              pInputConnection = BLST_S_NEXT(pInputConnection, upstreamNode) )
        {
            if ( pInputConnection->loopbackGroup )
            {
                BAPE_LoopbackGroupSettings loopbackSettings;
                BAPE_LoopbackGroup_P_GetSettings(pInputConnection->loopbackGroup, &loopbackSettings);
#if BAPE_CHIP_MAX_FS > 0
                loopbackSettings.fs = handle->fs;
#else
                loopbackSettings.mclkSource = handle->mclkSource;
                loopbackSettings.pllChannel = 0;
                loopbackSettings.mclkFreqToFsRatio = BAPE_BASE_PLL_TO_FS_RATIO;
#endif
                errCode = BAPE_LoopbackGroup_P_SetSettings(pInputConnection->loopbackGroup, &loopbackSettings);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                    goto err_acquire_resources;
                }
            }
            break;
        }

    }
    else
    {
        handle->mclkSource = BAPE_MclkSource_eNone;
    }


    BDSP_Task_GetDefaultCreateSettings(handle->deviceHandle->dspContext, &handle->taskCreateSettings);

    /* Setup default branch */
    handle->taskCreateSettings.numBranches = 1;
    handle->taskCreateSettings.pBranchInfo[0] = handle->pPrimaryBranch;

    handle->taskCreateSettings.openGateAtStart = handle->startedExplicitly;
    handle->taskCreateSettings.timeBaseType = BDSP_AF_P_TimeBaseType_e45Khz;
    handle->taskCreateSettings.schedulingMode = BDSP_TaskSchedulingMode_eMaster;
    BAPE_DSP_P_InitBranch(handle->pPrimaryBranch);
    handle->pPrimaryBranch->ui32NumStages = 1;
    handle->pPrimaryBranch->sFwStgInfo[0].ui32BranchId = 0;
    handle->pPrimaryBranch->sFwStgInfo[0].ui32StageId = 0;
    handle->pPrimaryBranch->sFwStgInfo[0].eStageType = BDSP_CIT_P_StageType_ePostProc;
    handle->pPrimaryBranch->sFwStgInfo[0].uAlgorithm.eProcAudioAlgo = BDSP_AudioProcessing_eFWMixer;
    handle->pPrimaryBranch->sFwStgInfo[0].uAudioMode.eProcessingAudioMode = BDSP_DSPCHN_ProcessingMode_ePP;
    handle->pPrimaryBranch->sFwStgInfo[0].sStgConnectivity.ui32NumSrc = 0;    /* Inputs will be added later. */        
    BKNI_Memset(&handle->pPrimaryBranch->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0], 0, 
                sizeof(handle->pPrimaryBranch->sFwStgInfo[0].sStgConnectivity.sSrcDetails[0]));

    branchId = stageId = 0;

    /* Add sample rate converter if required */
    if ( handle->settings.mixerSampleRate )
    {
        /* Add SRC stage after mixer if required */
        errCode = BAPE_DSP_P_AddProcessingStage(&handle->taskCreateSettings, branchId, stageId, BAPE_DSP_P_GetDataTypeFromConnector(&handle->pathNode.connectors[0]), BDSP_AudioProcessing_eSrc, false, &branchId, &stageId);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_acquire_resources;
        }

        /* Program sample rate map table so all output rates match the requested rate */
        for ( i = 0; i < BDSP_AF_P_SampFreq_eMax; i++ )
        {
            handle->sampleRateMap.ui32OpSamplingFrequency[i] = handle->settings.mixerSampleRate;
        }
        handle->taskCreateSettings.pSampleRateMap = &handle->sampleRateMap;  /* If not set, this will be set to the default rate table in bdsp */
    }

    /* Prepare Connector */
    handle->pathNode.connectors[0].pTaskCreateSettings = &handle->taskCreateSettings;
    handle->pathNode.connectors[0].branchId = branchId;
    handle->pathNode.connectors[0].stageId = stageId;

    /* Prepare subnodes */
    errCode = BAPE_PathNode_P_AcquirePathResources(&handle->pathNode);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_acquire_resources;
    }

    BKNI_EnterCriticalSection();
    BAPE_Connector_P_SetSampleRate_isr(&handle->pathNode.connectors[0], (handle->master) ? (handle->master->format.sampleRate) : 0);
    BKNI_LeaveCriticalSection();

    /* Validate we either have real-time outputs or don't depending on system requirements */
    BAPE_PathNode_P_FindConsumersBySubtype(&handle->pathNode, BAPE_PathNodeType_eMixer, BAPE_MixerType_eStandard, 1, &numFound, &pNode);
    if ( handle->taskCreateSettings.realtimeMode == BDSP_TaskRealtimeMode_eNonRealTime )
    {
        if ( numFound != 0 )
        {
            BDBG_ERR(("No outputs should be connected to a DSP mixer in non-realtime mode."));
            errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
            goto err_consumers;
        }
    }
    else
    {
        if ( numFound == 0 )
        {
            BDBG_ERR(("No outputs are connected.  Cannot start."));
            errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
            goto err_consumers;
        }
    }

    /* Don't set openGateAtStart in NRT or for compressed input */
    if ( !BAPE_FMT_P_IsLinearPcm(&handle->pathNode.connectors[0].format) || handle->taskCreateSettings.realtimeMode != BDSP_TaskRealtimeMode_eRealTime )
    {
        handle->taskCreateSettings.openGateAtStart = false;
    }
    handle->taskCreateSettings.dspIndex = handle->settings.dspIndex;
    errCode = BDSP_Task_Create(handle->deviceHandle->dspContext, &handle->taskCreateSettings, &handle->task);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_task_create;
    }

    /* TODO: Apply TSM/Framesync settings */

    handle->pathNode.connectors[0].task = handle->task;

    /* Link the path resources */
    errCode = BAPE_PathNode_P_ConfigurePathResources(&handle->pathNode);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_config_path;
    }

    #if BDBG_DEBUG_BUILD
        BAPE_Mixer_P_PrintMixers(handle->deviceHandle);
    #endif

    /* Start the consumers */
    errCode = BAPE_PathNode_P_StartPaths(&handle->pathNode);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_start_path;
    }

    #if BDBG_DEBUG_BUILD
        BAPE_Mixer_P_PrintDownstreamNodes(&handle->pathNode);
    #endif

    /* Start the DSP Task */
    errCode = BDSP_Task_Start(handle->task);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_start_task;
    }
    
    return BERR_SUCCESS;

err_start_task:
err_start_path:
err_config_path:
    BDSP_Task_Destroy(handle->task);
    handle->task = NULL;
err_task_create:
err_consumers:
err_acquire_resources:
    BAPE_PathNode_P_ReleasePathResources(&handle->pathNode);
    BAPE_DspMixer_P_CleanupTaskCreateSettings(handle);    
    if ( fsRequired )
    {
        BAPE_P_FreeFs(handle->deviceHandle, handle->fs);
        handle->fs = BAPE_FS_INVALID;
    }
#if BAPE_CHIP_MAX_FS > 0
err_fs:
#endif
    return errCode;
}

static BERR_Code BAPE_DspMixer_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_MixerHandle handle;
    BERR_Code errCode;
    unsigned i;
    void *pCached;
    uint32_t base, end;
    BDSP_AF_P_sIO_BUFFER *pIoDescriptor;
    BDSP_AF_P_sIO_GENERIC_BUFFER *pGenericDescriptor;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = (BAPE_MixerHandle)pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    /* Sanity check if the dsp indexes match */
    if ( pConnection->pSource->format.source == BAPE_DataSource_eDspBuffer )
    {
        if ( pConnection->pSource->pTaskCreateSettings->dspIndex != handle->settings.dspIndex )
        {
            BDBG_ERR(("All inputs to a DSP mixer must run on the same task as the DSP mixer."));
            BDBG_ERR(("This mixer is configured for DSP %u but the input %s is configured for DSP %u",
                      handle->settings.dspIndex, pConnection->pSource->pParent->pName,
                      pConnection->pSource->pTaskCreateSettings->dspIndex));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }

    /* First see if we need to create the mixer task itself */
    if ( NULL == handle->task )
    {
        BDBG_MSG(("AllocatePathFromInput: Task not present.  Starting mixer task."));
        errCode = BAPE_DspMixer_P_StartTask(handle);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    /* Task Handle is valid if we reach here. */
    
    switch ( pConnection->pSource->format.source )
    {
    case BAPE_DataSource_eDspBuffer:
        /* Set mixer task as the master for the input task */
        pConnection->pSource->pTaskCreateSettings->schedulingMode = BDSP_TaskSchedulingMode_eSlave;
        pConnection->pSource->pTaskCreateSettings->masterTask = handle->task;
        pConnection->pSource->pTaskCreateSettings->realtimeMode = handle->taskCreateSettings.realtimeMode;

        /* Allocate required buffers */
        if ( NULL == pConnection->pInterTaskGenericBuffer )
        {
            pConnection->pInterTaskGenericBuffer = BMEM_Heap_Alloc(handle->deviceHandle->memHandle, BDSP_AF_P_INTERTASK_IOGENBUFFER_SIZE);
            if ( NULL == pConnection->pInterTaskGenericBuffer )
            {
                BAPE_DspMixer_P_FreeConnectionResources(handle, pConnection);
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
        }
        if ( NULL == pConnection->pInterTaskIoBuffer )
        {
            pConnection->pInterTaskIoBuffer = BMEM_Heap_Alloc(handle->deviceHandle->memHandle, BAPE_FMT_P_GetNumChannels(&pConnection->pSource->format) * (BAPE_P_INTER_TASK_BUFFER_SIZE));
            if ( NULL == pConnection->pInterTaskIoBuffer )
            {
                BAPE_DspMixer_P_FreeConnectionResources(handle, pConnection);
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
        }
        if ( NULL == pConnection->pInterTaskIoDescriptor )
        {
            pConnection->pInterTaskIoDescriptor = BMEM_Heap_Alloc(handle->deviceHandle->memHandle, sizeof(BDSP_AF_P_sIO_BUFFER));
            if ( NULL == pConnection->pInterTaskIoDescriptor )
            {
                BAPE_DspMixer_P_FreeConnectionResources(handle, pConnection);
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
        }
        /* Initialize IO Buffer Descriptor */
        (void)BMEM_Heap_ConvertAddressToCached(handle->deviceHandle->memHandle, pConnection->pInterTaskIoDescriptor, &pCached);
        pIoDescriptor = pCached;
        pIoDescriptor->eBufferType = BDSP_AF_P_BufferType_eDRAM;
        pIoDescriptor->ui32NumBuffers = BAPE_FMT_P_GetNumChannels(&pConnection->pSource->format);
        for ( i = 0; i < pIoDescriptor->ui32NumBuffers; i++ )
        {
            BMEM_ConvertAddressToOffset(handle->deviceHandle->memHandle, pConnection->pInterTaskIoBuffer, &base);
            base += i*BAPE_P_INTER_TASK_BUFFER_SIZE;
            end = base+(BAPE_P_INTER_TASK_BUFFER_SIZE);
            pIoDescriptor->sCircBuffer[i].ui32BaseAddr = base;
            pIoDescriptor->sCircBuffer[i].ui32ReadAddr = base;
            pIoDescriptor->sCircBuffer[i].ui32WriteAddr = base;
            pIoDescriptor->sCircBuffer[i].ui32EndAddr = end;
            pIoDescriptor->sCircBuffer[i].ui32WrapAddr = end;
        }
        for ( i = pIoDescriptor->ui32NumBuffers; i < BDSP_AF_P_MAX_CHANNELS; i++ )
        {
            BKNI_Memset(&pIoDescriptor->sCircBuffer[i], 0, sizeof(pIoDescriptor->sCircBuffer[i]));
        }
        BMEM_Heap_FlushCache(handle->deviceHandle->memHandle, pIoDescriptor, sizeof(BDSP_AF_P_sIO_BUFFER));
        if ( NULL == pConnection->pInterTaskGenericDescriptor )
        {
            pConnection->pInterTaskGenericDescriptor = BMEM_Heap_Alloc(handle->deviceHandle->memHandle, sizeof(BDSP_AF_P_sIO_GENERIC_BUFFER));
            if ( NULL == pConnection->pInterTaskGenericDescriptor )
            {
                BAPE_DspMixer_P_FreeConnectionResources(handle, pConnection);
                return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            }
        }
        /* Initialize IO Generic Buffer Descriptor */
        (void)BMEM_Heap_ConvertAddressToCached(handle->deviceHandle->memHandle, pConnection->pInterTaskGenericDescriptor, &pCached);
        pGenericDescriptor = pCached;
        BKNI_Memset(pGenericDescriptor, 0, sizeof(BDSP_AF_P_sIO_GENERIC_BUFFER));
        pGenericDescriptor->eBufferType = BDSP_AF_P_BufferType_eDRAM;
        pGenericDescriptor->ui32NumBuffers = 1;
        BMEM_ConvertAddressToOffset(handle->deviceHandle->memHandle, pConnection->pInterTaskGenericBuffer, &base);
        end = base+BDSP_AF_P_INTERTASK_IOGENBUFFER_SIZE;
        pGenericDescriptor->sCircBuffer.ui32BaseAddr = base;
        pGenericDescriptor->sCircBuffer.ui32ReadAddr = base;
        pGenericDescriptor->sCircBuffer.ui32WriteAddr = base;
        pGenericDescriptor->sCircBuffer.ui32EndAddr = end;
        pGenericDescriptor->sCircBuffer.ui32WrapAddr = end;
        BMEM_Heap_FlushCache(handle->deviceHandle->memHandle, pGenericDescriptor, sizeof(BDSP_AF_P_sIO_GENERIC_BUFFER));
        break;
    case BAPE_DataSource_eHostBuffer:
    case BAPE_DataSource_eDfifo:
    case BAPE_DataSource_eFci:
        errCode = BAPE_DspMixer_P_AllocateLoopbackInput(handle, pConnection);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        break;
    default:
        BDBG_ERR(("Unsupported input type"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    return BERR_SUCCESS;
}

static void BAPE_DspMixer_P_FreePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_MixerHandle handle;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = (BAPE_MixerHandle)pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    BAPE_DspMixer_P_FreeConnectionResources(handle, pConnection);

    /* Stop the mixer task if nobody is running. */
    if ( 0 == handle->running && handle->task )
    {
        BDBG_MSG(("FreePathFromInput: No running inputs - stop mixer task"));
        BAPE_DspMixer_P_StopTask(handle);
    }
}

static BERR_Code BAPE_DspMixer_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_MixerHandle handle;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = (BAPE_MixerHandle)pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    return BERR_SUCCESS;
}

static BERR_Code BAPE_DspMixer_P_GetFwSrcType(BAPE_MixerHandle handle, BAPE_PathConnection *pConnection, BDSP_CIT_P_FwStgSrcDstType *pType)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_ASSERT(NULL != pType);

    switch ( pConnection->pSource->format.source )    
    {
    case BAPE_DataSource_eDspBuffer:
        *pType = BDSP_CIT_P_FwStgSrcDstType_eInterTaskDRAMBuf;
        break;
    case BAPE_DataSource_eHostBuffer:
    case BAPE_DataSource_eDfifo:
    case BAPE_DataSource_eFci:
        /* DFIFO Input */
        *pType = BDSP_CIT_P_FwStgSrcDstType_eFMMBuf;
        break;
    default:
        BDBG_ERR(("Unsupported input type"));
        *pType = BDSP_CIT_P_FwStgSrcDstType_eInvalid;
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_DspMixer_P_GetFwSrcDetails(BAPE_MixerHandle handle, BAPE_PathConnection *pConnection, BDSP_CIT_P_FwStgSrcDstDetails *pDetails)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    switch ( pConnection->pSource->format.source )    
    {
    case BAPE_DataSource_eDspBuffer:
        pDetails->eType = BDSP_CIT_P_FwStgSrcDstType_eInterTaskDRAMBuf;
        BMEM_Heap_ConvertAddressToOffset(pConnection->pSink->deviceHandle->memHandle, 
                                         pConnection->pInterTaskIoDescriptor,
                                         &pDetails->uDetails.sInterTaskDramBuffCfg.ui32IoBuffCfgAddr);

        BMEM_Heap_ConvertAddressToOffset(pConnection->pSink->deviceHandle->memHandle, 
                                         pConnection->pInterTaskGenericDescriptor,
                                         &pDetails->uDetails.sInterTaskDramBuffCfg.ui32IoGenericBuffCfgAddr);
        break;
    case BAPE_DataSource_eHostBuffer:
    case BAPE_DataSource_eDfifo:
    case BAPE_DataSource_eFci:
        /* DFIFO Input */
        BAPE_DSP_P_GetDfifoSrcDetails(handle->deviceHandle, pConnection->dfifoGroup, BAPE_FMT_P_GetNumChannels(&pConnection->pSource->format), false, pDetails);
        break;
    default:
        BDBG_ERR(("Unsupported input type"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_DspMixer_P_StartPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_MixerHandle handle;
    BDSP_CIT_P_FwStgSrcDstType srcType;
    BDSP_TaskAddStageInputSettings addInputSettings;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = (BAPE_MixerHandle)pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    
    BDBG_MSG(("Input %s %s starting", pConnection->pSource->pParent->pName, pConnection->pSource->pName));

    /* First, setup the sample rate since it may affect output programming. */
    BKNI_EnterCriticalSection();
    BAPE_DspMixer_P_InputSampleRateChange_isr(pNode, pConnection, pConnection->pSource->format.sampleRate);
    BKNI_LeaveCriticalSection();

    /* For FMM sources, start the FMM data path */
    if ( pConnection->dfifoGroup )
    {
        errCode = BAPE_DfifoGroup_P_Start(pConnection->dfifoGroup, false);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_dfifo_start;
        }
    }
    if ( pConnection->loopbackGroup )
    {
        errCode = BAPE_LoopbackGroup_P_Start(pConnection->loopbackGroup);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_loopback_start;
        }
    }
    if ( pConnection->mixerGroup )
    {
        errCode = BAPE_MixerGroup_P_StartOutput(pConnection->mixerGroup, 0);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_mixer_output;
        }
        errCode = BAPE_MixerGroup_P_StartInput(pConnection->mixerGroup, 0);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_mixer_input;
        }
    }
    if ( pConnection->srcGroup )
    {
        errCode = BAPE_SrcGroup_P_Start(pConnection->srcGroup);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_src_start;
        }
    }

    errCode = BAPE_DspMixer_P_GetFwSrcType(handle, pConnection, &srcType);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_get_details;
    }

    BDSP_Task_GetDefaultAddStageInputSettings(srcType, &addInputSettings);
    addInputSettings.branchId = 0;
    addInputSettings.stageId = 0;
    errCode = BAPE_DspMixer_P_GetFwSrcDetails(handle, pConnection, &addInputSettings.sourceDetails);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_get_details;
    }

    /* Start input */
    BDBG_ASSERT(NULL == pConnection->taskInput);
    BDBG_MSG(("Adding input to FW Mixer for %s %s", pConnection->pSource->pParent->pName, pConnection->pSource->pName));
    errCode = BDSP_Task_AddStageInput(handle->task, &addInputSettings, &pConnection->taskInput);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_add_input;
    }

    /* Refresh input scaling prior now that the input is valid to the mixer */    
    errCode = BAPE_DspMixer_P_ApplyInputVolume(handle, BAPE_Mixer_P_FindInputIndex(handle, pConnection->pSource));
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_input_scaling;
    }

    handle->running++;
    return BERR_SUCCESS;

err_input_scaling:
    BDSP_Task_RemoveStageInput(pConnection->taskInput);
    pConnection->taskInput = NULL;
err_add_input:
err_get_details:
    if ( pConnection->srcGroup )
    {
        BAPE_SrcGroup_P_Stop(pConnection->srcGroup);
    }
err_src_start:
    if ( pConnection->mixerGroup )
    {
        BAPE_MixerGroup_P_StopInput(pConnection->mixerGroup, 0);
    }
err_mixer_input:
    if( pConnection->mixerGroup )
    {
        BAPE_MixerGroup_P_StopOutput(pConnection->mixerGroup, 0);
    }
err_mixer_output:
    if ( pConnection->loopbackGroup )
    {
        BAPE_LoopbackGroup_P_Stop(pConnection->loopbackGroup);
    }
err_loopback_start:
    if ( pConnection->dfifoGroup )
    {
        BAPE_DfifoGroup_P_Stop(pConnection->dfifoGroup);
    }
err_dfifo_start:
    return errCode;
}

static void BAPE_DspMixer_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_MixerHandle handle;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = (BAPE_MixerHandle)pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    
    BDBG_ASSERT(handle->running > 0);

    if ( NULL == pConnection->taskInput )
    {
        BDBG_MSG(("Input already stopped"));
        return;
    }

    BDBG_MSG(("Removing input from FW Mixer for %s %s", pConnection->pSource->pParent->pName, pConnection->pSource->pName));
    errCode = BDSP_Task_RemoveStageInput(pConnection->taskInput);
    BDBG_ASSERT(errCode == BERR_SUCCESS);   /* If this fails something has gone horribly wrong */
    pConnection->taskInput = NULL;

    if ( pConnection->srcGroup )
    {
        BAPE_SrcGroup_P_Stop(pConnection->srcGroup);
    }
    if ( pConnection->mixerGroup )
    {
        BAPE_MixerGroup_P_StopInput(pConnection->mixerGroup, 0);
        BAPE_MixerGroup_P_StopOutput(pConnection->mixerGroup, 0);
    }
    if ( pConnection->loopbackGroup )
    {
        BAPE_LoopbackGroup_P_Stop(pConnection->loopbackGroup);
    }
    if ( pConnection->dfifoGroup )
    {
        BAPE_DfifoGroup_P_Stop(pConnection->dfifoGroup);
    }

    handle->running--;
    /* Stop the mixer task if nobody is running. */
    if ( 0 == handle->running && handle->task )
    {
        BDBG_MSG(("FreePathFromInput: Last running input has stopped - stop mixer task"));
        BAPE_DspMixer_P_StopTask(handle);
    }
}

/* TODO: Share with other mixers */
static void BAPE_DspMixer_P_InputSampleRateChange_isr(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, unsigned newSampleRate)
{
    BAPE_MixerHandle handle;
    unsigned sampleRate;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BKNI_ASSERT_ISR_CONTEXT();

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    sampleRate = newSampleRate;

    BDBG_MSG(("Input %s %s sample rate changed from %u to %u", pConnection->pSource->pParent->pName, pConnection->pSource->pName, pConnection->format.sampleRate, sampleRate));

    /* Mark new sample rate in connection */
    pConnection->format.sampleRate = sampleRate;

    /* Handle fixed rate output */
    if ( handle->settings.mixerSampleRate )
    {
        /* Set mixer sample rate. */
        BAPE_DspMixer_P_SetSampleRate_isr(handle, handle->settings.mixerSampleRate);
    }
    /* Handle the sample rate master or fixed rate output */
    else if ( handle->master == pConnection->pSource )
    {
        if ( sampleRate != 0 )
        {
            /* Set mixer sample rate. */
            BAPE_DspMixer_P_SetSampleRate_isr(handle, sampleRate);
        }
        else if ( BAPE_Mixer_P_GetOutputSampleRate_isr(handle) == 0 )
        {
            /* Make sure there is a valid sample rate */
            BAPE_DspMixer_P_SetSampleRate_isr(handle, handle->settings.defaultSampleRate);
        }
    }
    else
    {
        /* Make sure there is a valid sample rate */
        if ( BAPE_Mixer_P_GetOutputSampleRate_isr(handle) == 0 )
        {
            BAPE_DspMixer_P_SetSampleRate_isr(handle, handle->settings.defaultSampleRate);
        }
    }

    /* Update SRCs accordingly. */
    BAPE_DspMixer_P_SetInputSRC_isr(handle, pConnection->pSource, sampleRate, BAPE_Mixer_P_GetOutputSampleRate_isr(handle));
}

static BERR_Code BAPE_DspMixer_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat)
{
    BERR_Code errCode;
    BAPE_MixerHandle handle;
    BAPE_DataType outputDataType;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = (BAPE_MixerHandle)pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    
    /* See if input format has changed.  If not, do nothing. */
    if ( pNewFormat->type != pConnection->format.type )
    {
        BDBG_MSG(("Input connection format change reported, but does not affect resource requirements."));
    }
    else
    {
        BDBG_MSG(("Input connection format change reported - freeing connection resources."));
        BAPE_DspMixer_P_FreeConnectionResources(handle, pConnection);
    }

    /* Determine output format based on widest input format */
    errCode = BAPE_Mixer_P_DetermineOutputDataType(handle, &outputDataType);
    if ( errCode )
    {
        BDBG_ERR(("Unable to determine mixer ouptut data format."));
        return BERR_TRACE(errCode);
    }

    /* If the output format has changed, propagate that downstream */
    if ( outputDataType != BAPE_Mixer_P_GetOutputDataType(handle) )
    {
        BAPE_FMT_Descriptor format;
        if ( handle->running )
        {
            BDBG_ERR(("Can not change mixer output format while running."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        BAPE_DspMixer_P_FreeConnectionResources(handle, pConnection);   /* Resources will be allocated next time required. */
        BAPE_Connector_P_GetFormat(&handle->pathNode.connectors[0], &format);
        format.type = outputDataType;
        BDBG_MSG(("Mixer output format has changed.  Was %s now %s.", BAPE_FMT_P_GetTypeName(BAPE_Mixer_P_GetOutputFormat(handle)), BAPE_FMT_P_GetTypeName(&format)));
        errCode = BAPE_Connector_P_SetFormat(&handle->pathNode.connectors[0], &format);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

static void BAPE_DspMixer_P_InputMute(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, bool enabled)
{
    BSTD_UNUSED(pNode);
    BSTD_UNUSED(pConnection);
    BSTD_UNUSED(enabled);
    /* TODO */
}

static void BAPE_DspMixer_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    BAPE_DspMixer_P_RemoveInput(pNode->pHandle, pConnector);
}

static void BAPE_DspMixer_P_Stop(BAPE_MixerHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    if ( !handle->running )
    {
        BDBG_MSG(("Mixer %p (%d) is already stopped.", handle, handle->index));
        return;
    }

    if ( !handle->startedExplicitly )
    {
        BDBG_MSG(("Mixer %p (%d) was not started with BAPE_Mixer_Start(). Can't stop.", handle, handle->index));
        return;
    }

    handle->startedExplicitly = false;
    handle->running--;
    
    /* Stop the mixer task if nobody is running. */
    if ( 0 == handle->running && handle->task )
    {
        BDBG_MSG(("BAPE_DspMixer_P_Stop: Last running input has stopped - stop mixer task"));
        BAPE_DspMixer_P_StopTask(handle);
    }
            
    return;
}


static void BAPE_DspMixer_P_StopTask(BAPE_MixerHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    
    if ( NULL == handle->task )
    {
        return;
    }

    BDBG_ASSERT(0 == handle->running);

    BDSP_Task_Stop(handle->task);
    BAPE_PathNode_P_StopPaths(&handle->pathNode);
    BDSP_Task_Destroy(handle->task);
    handle->task = NULL;
    BAPE_DspMixer_P_CleanupTaskCreateSettings(handle);

    /* Unlink from PLL and give up FS if allocated */
    if ( handle->mclkSource != BAPE_MclkSource_eNone )
    {
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
#if BAPE_CHIP_MAX_FS > 0
        BAPE_P_FreeFs(handle->deviceHandle, handle->fs);
        handle->fs = BAPE_FS_INVALID;
#endif
    }
}

static void BAPE_DspMixer_P_CleanupTaskCreateSettings(BAPE_MixerHandle handle)
{
    unsigned i;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);

    /* The first branch is always static.  Others need to be BKNI_Free()'d. */
    /* This is not an ideal programming model, but workable */
    for ( i = 1; i < handle->taskCreateSettings.numBranches; i++ )
    {
        BDBG_ASSERT(NULL != handle->taskCreateSettings.pBranchInfo[i]);
        BKNI_Free(handle->taskCreateSettings.pBranchInfo[i]);
    }
    /* Reset structure to defaults */
    BDSP_Task_GetDefaultCreateSettings(handle->deviceHandle->dspContext, &handle->taskCreateSettings);
}

static BERR_Code BAPE_DspMixer_P_AllocateLoopbackInput(BAPE_MixerHandle handle, BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    bool sfifo=false, buffers=false, buffersOnly=false;
    unsigned numChannelPairs, i;

    BAPE_Connector input = pConnection->pSource;

    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&pConnection->pSource->format);

    BDBG_MSG(("DSP Mixer %p Allocating resources for input %s %s", handle, input->pParent->pName, input->pName));

    switch ( pConnection->pSource->format.source )
    {
    case BAPE_DataSource_eDfifo:        /* Technically this could route directly to the DSP but it may require sample rate conversion */
    case BAPE_DataSource_eHostBuffer:
        sfifo = true;                   /* Playback via a Host Buffer requires a SFIFO allocation */
        buffers = pConnection->pSource->useBufferPool;
        break;
    case BAPE_DataSource_eFci:
        break;
    default:
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Check if we've already allocated resources */
    if ( pConnection->resourcesAllocated )
    {
        if ( pConnection->format.type == pConnection->pSource->format.type && pConnection->format.ppmCorrection == pConnection->pSource->format.ppmCorrection )
        {
            BDBG_MSG(("Connection %s -> %s %s already has the required resources", input->pParent->pName, pConnection->pSink->pName, input->pName));
            if ( sfifo )
            {
                BDBG_ASSERT(NULL != pConnection->sfifoGroup);
            }
            BDBG_ASSERT(NULL != pConnection->srcGroup);
            BDBG_ASSERT(NULL != pConnection->mixerGroup);
            BDBG_ASSERT(NULL != pConnection->loopbackGroup);
            BDBG_ASSERT(NULL != pConnection->dfifoGroup);

            buffersOnly = true;
        }
        else
        {
            /* Connection format has changed */
            BDBG_MSG(("Connection %s -> %s %s format changed.  Releasing resources.", input->pParent->pName, pConnection->pSink->pName, input->pName));
            BAPE_DspMixer_P_FreeConnectionResources(handle, pConnection);
        }
    }

    if ( buffers )
    {
        /* This is safe to call multiple times, it only allocates if need be */
        errCode = BAPE_P_AllocateInputBuffers(handle->deviceHandle, input);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    pConnection->format = pConnection->pSource->format;

    if ( buffersOnly )
    {
        /* We don't need to re-add the remaining resources */
        return BERR_SUCCESS;
    }

    BDBG_ASSERT(NULL == pConnection->sfifoGroup);
    BDBG_ASSERT(NULL == pConnection->srcGroup);

    /* Allocate SFIFO if required. */
    if ( sfifo )
    {
        if ( NULL == pConnection->sfifoGroup )
        {
            BAPE_SfifoGroupCreateSettings sfifoCreateSettings;
            BAPE_SfifoGroup_P_GetDefaultCreateSettings(&sfifoCreateSettings);
            sfifoCreateSettings.numChannelPairs = numChannelPairs;
            sfifoCreateSettings.ppmCorrection = false;
            errCode = BAPE_SfifoGroup_P_Create(handle->deviceHandle, &sfifoCreateSettings, &pConnection->sfifoGroup);
            if ( errCode )
            {
                (void)BERR_TRACE(errCode);
                goto err_alloc_sfifo;
            }
        }
    }

    if ( NULL == pConnection->srcGroup )
    {
        BAPE_SrcGroupSettings srcSettings;
        BAPE_SrcGroupCreateSettings srcCreateSettings;
        BAPE_SrcGroup_P_GetDefaultCreateSettings(&srcCreateSettings);
        srcCreateSettings.numChannelPairs = numChannelPairs;
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
        srcSettings.rampEnabled = false;            /* No need to ramp, output from the mixer will again be ramped. */
        srcSettings.startupRampEnabled = false;        
        errCode = BAPE_SrcGroup_P_SetSettings(pConnection->srcGroup, &srcSettings);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_src_settings;
        }
    }
        
    if ( NULL == pConnection->mixerGroup )
    {
        BAPE_MixerGroupInputSettings mixerInputSettings;
        BAPE_MixerGroupCreateSettings mixerCreateSettings;
        BAPE_MixerGroup_P_GetDefaultCreateSettings(&mixerCreateSettings);
        mixerCreateSettings.numChannelPairs = numChannelPairs;
        errCode = BAPE_MixerGroup_P_Create(handle->deviceHandle, &mixerCreateSettings, &pConnection->mixerGroup);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_alloc_mixer;
        }

        BAPE_MixerGroup_P_GetInputSettings(pConnection->mixerGroup, 0, &mixerInputSettings);
        BAPE_SrcGroup_P_GetOutputFciIds(pConnection->srcGroup, &mixerInputSettings.input);
        errCode = BAPE_MixerGroup_P_SetInputSettings(pConnection->mixerGroup, 0, &mixerInputSettings);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_mixer_input;
        }
    }

    if ( NULL == pConnection->loopbackGroup )
    {
        BAPE_LoopbackGroupSettings loopbackSettings;
        BAPE_LoopbackGroupCreateSettings loopbackCreateSettings;
        BAPE_LoopbackGroup_P_GetDefaultCreateSettings(&loopbackCreateSettings);
        loopbackCreateSettings.numChannelPairs = numChannelPairs;
        errCode = BAPE_LoopbackGroup_P_Create(handle->deviceHandle, &loopbackCreateSettings, &pConnection->loopbackGroup);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_alloc_loopback;
        }
        BAPE_LoopbackGroup_P_GetSettings(pConnection->loopbackGroup, &loopbackSettings);
        /* FS will be set when task starts */
        BAPE_MixerGroup_P_GetOutputFciIds(pConnection->mixerGroup, 0, &loopbackSettings.input);
        loopbackSettings.resolution = 24;
        errCode = BAPE_LoopbackGroup_P_SetSettings(pConnection->loopbackGroup, &loopbackSettings);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_loopback_settings;
        }
    }

    errCode = BAPE_P_AllocateBuffers(handle->deviceHandle, &pConnection->pSource->format, pConnection->pLoopbackBuffers);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
                goto err_buffer;
            }

    if ( NULL == pConnection->dfifoGroup )
    {
        BAPE_DfifoGroupSettings dfifoSettings;
        BAPE_DfifoGroupCreateSettings dfifoCreateSettings;
        BAPE_DfifoGroup_P_GetDefaultCreateSettings(&dfifoCreateSettings);
        dfifoCreateSettings.numChannelPairs = numChannelPairs;
        errCode = BAPE_DfifoGroup_P_Create(handle->deviceHandle, &dfifoCreateSettings, &pConnection->dfifoGroup);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_alloc_dfifo;
        }
        BAPE_DfifoGroup_P_GetSettings(pConnection->dfifoGroup, &dfifoSettings);
        dfifoSettings.interleaveData = false;
        dfifoSettings.dataWidth = 32;
        BAPE_LoopbackGroup_P_GetCaptureFciIds(pConnection->loopbackGroup, &dfifoSettings.input);
        for ( i = 0; i < numChannelPairs; i++ )
        {
            uint32_t length;
            unsigned bufferNum = i*2;
            BAPE_BufferNode *pBuffer = pConnection->pLoopbackBuffers[i];
            length = pBuffer->bufferSize/2;
            dfifoSettings.bufferInfo[bufferNum].base = pBuffer->offset;
            dfifoSettings.bufferInfo[bufferNum+1].base = pBuffer->offset + length;
            dfifoSettings.bufferInfo[bufferNum].length = length;
            dfifoSettings.bufferInfo[bufferNum+1].length = length;
        }
        errCode = BAPE_DfifoGroup_P_SetSettings(pConnection->dfifoGroup, &dfifoSettings);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_dfifo_settings;
        }
    }

    pConnection->resourcesAllocated = true;

    return BERR_SUCCESS;

err_dfifo_settings:
err_alloc_dfifo:
err_buffer:
err_loopback_settings:
err_alloc_loopback:
err_mixer_input:
err_alloc_mixer:
err_src_settings:
err_alloc_src:
err_alloc_sfifo:
    BAPE_DspMixer_P_FreeConnectionResources(handle, pConnection);
    return errCode;
}

static void BAPE_DspMixer_P_FreeConnectionResources(BAPE_MixerHandle handle, BAPE_PathConnection *pConnection)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Mixer);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    if ( NULL != pConnection->pInterTaskIoBuffer )
    {
        BMEM_Heap_Free(handle->deviceHandle->memHandle, pConnection->pInterTaskIoBuffer);
        pConnection->pInterTaskIoBuffer = NULL;
    }
    if ( NULL != pConnection->pInterTaskGenericBuffer )
    {
        BMEM_Heap_Free(handle->deviceHandle->memHandle, pConnection->pInterTaskGenericBuffer);
        pConnection->pInterTaskGenericBuffer = NULL;
    }
    if ( NULL != pConnection->pInterTaskIoDescriptor )
    {
        BMEM_Heap_Free(handle->deviceHandle->memHandle, pConnection->pInterTaskIoDescriptor);
        pConnection->pInterTaskIoDescriptor = NULL;
    }
    if ( NULL != pConnection->pInterTaskGenericDescriptor )
    {
        BMEM_Heap_Free(handle->deviceHandle->memHandle, pConnection->pInterTaskGenericDescriptor);
        pConnection->pInterTaskGenericDescriptor = NULL;
    }
    if ( pConnection->pSource->useBufferPool )
    {
        BAPE_P_FreeInputBuffers(handle->deviceHandle, pConnection->pSource);
    }
    if ( NULL != pConnection->sfifoGroup )
    {
        BAPE_SfifoGroup_P_Destroy(pConnection->sfifoGroup);
        pConnection->sfifoGroup = NULL;
    }
    if ( NULL != pConnection->srcGroup )
    {
        BAPE_SrcGroup_P_Destroy(pConnection->srcGroup);
        pConnection->srcGroup = NULL;
    }
    if ( NULL != pConnection->mixerGroup )
    {
        BAPE_MixerGroup_P_Destroy(pConnection->mixerGroup);
        pConnection->mixerGroup = NULL;
    }
    if ( NULL != pConnection->dfifoGroup )
    {
        BAPE_DfifoGroup_P_Destroy(pConnection->dfifoGroup);
        pConnection->dfifoGroup = NULL;
    }
    if ( NULL != pConnection->loopbackGroup )
    {
        BAPE_LoopbackGroup_P_Destroy(pConnection->loopbackGroup);
        pConnection->loopbackGroup = NULL;
    }

    BAPE_P_FreeBuffers(handle->deviceHandle, pConnection->pLoopbackBuffers);

    pConnection->resourcesAllocated = false;
}

/* TODO: Share with other mixers */
static void BAPE_DspMixer_P_SetSampleRate_isr(BAPE_MixerHandle mixer, unsigned sampleRate)
{
    BDBG_OBJECT_ASSERT(mixer, BAPE_Mixer);
    /* Only do this if something actually changed */
    if ( BAPE_Mixer_P_GetOutputSampleRate(mixer) != sampleRate )
    {
        unsigned i;

        BDBG_MSG(("Changing DSP mixer %p (%d) sample rate to %u [was %u]", mixer, mixer->index, sampleRate, BAPE_Mixer_P_GetOutputSampleRate(mixer)));

        /* Propagate sample rate downstream */
        (void)BAPE_Connector_P_SetSampleRate_isr(&mixer->pathNode.connectors[0], sampleRate);

        for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
        {
            if ( mixer->inputs[i] )
            {
                BAPE_DspMixer_P_SetInputSRC_isr(mixer, mixer->inputs[i], mixer->inputs[i]->format.sampleRate, sampleRate);
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
        BDBG_MSG(("NOT Changing DSP mixer %p (%d) sample rate to %u [currently %u]", mixer, mixer->index, sampleRate, BAPE_Mixer_P_GetOutputSampleRate(mixer)));
    }
}

/* TODO: Share with other mixers */
static void BAPE_DspMixer_P_SetInputSRC_isr(BAPE_MixerHandle mixer, BAPE_Connector input, unsigned inputRate, unsigned outputRate)
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
static BERR_Code BAPE_DspMixer_P_GetInputVolume(
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
static BERR_Code BAPE_DspMixer_P_ApplyInputVolume(BAPE_MixerHandle mixer, unsigned index)
{
    BDSP_Raaga_Audio_MixerConfigParams userConfig;
    BAPE_Connector source = mixer->inputs[index];
    BERR_Code errCode;
    unsigned i, taskInputIndex;
    BAPE_PathConnection *pConnection;

    BDBG_OBJECT_ASSERT(source, BAPE_PathConnector);

    /* Make sure we have a valid input index */
    if ( index == BAPE_MIXER_INPUT_INDEX_INVALID )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Find Connection */
    pConnection = BAPE_Connector_P_GetConnectionToSink(mixer->inputs[index], &mixer->pathNode);
    if ( NULL == pConnection )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Input may not be running yet, check that. */
    if ( NULL == pConnection->taskInput )
    {
        return BERR_SUCCESS;
    }

    /* Get task input index */
    errCode = BDSP_Task_GetStageInputIndex(pConnection->taskInput, &taskInputIndex);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BDBG_ASSERT(taskInputIndex < BDSP_AF_P_MAX_IP_FORKS);

    /* Apply settings to FW Mixer */
    errCode = BDSP_Task_GetStageSettings(mixer->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    for ( i = 0; i < BAPE_Channel_eMax; i++ )
    {
        uint32_t scaleValue;
        
        if ( mixer->inputVolume[index].muted || (i >= (2*BAPE_FMT_P_GetNumChannelPairs(&mixer->inputs[index]->format))) )
        {
            scaleValue = 0;
        }
        else
        {
            scaleValue = mixer->inputVolume[index].coefficients[i][i];
            if ( scaleValue > 0x7FFFFFF )
            {
                BDBG_WRN(("Input coefficients out of range for input %s %s - saturating at 0x7FFFFFF", mixer->inputs[index]->pParent->pName, mixer->inputs[index]->pName));
                scaleValue = 0xFFFFFFFF;
            }
            else
            {
                scaleValue <<= 5;   /* Convert from 5.23 (HW) to 4.28 (DSP) */
            }
        }
        BDBG_MSG(("Setting FW Mixing Coefs[%u][%u] to 0x%08x (mute=%u value=%#x)", taskInputIndex, i, scaleValue, mixer->inputVolume[index].muted, mixer->inputVolume[index].coefficients[i][i]));
        userConfig.MixingCoeffs[taskInputIndex][i] = scaleValue;
    }

    BAPE_DSP_P_SET_VARIABLE(userConfig, i32UserMixBalance, mixer->settings.multiStreamBalance);

    errCode = BDSP_Task_SetStageSettings(mixer->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}


/*************************************************************************/
static BERR_Code BAPE_DspMixer_P_SetInputVolume(
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
        errCode = BAPE_DspMixer_P_ApplyInputVolume(handle, i);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;    
}

static BERR_Code BAPE_DspMixer_P_SetSettings(
    BAPE_MixerHandle hMixer,
    const BAPE_MixerSettings *pSettings
    )
{
    BDSP_Raaga_Audio_MixerConfigParams userConfig;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(hMixer, BAPE_Mixer);
    BDBG_ASSERT(NULL != pSettings);

    hMixer->settings = *pSettings;

    if ( NULL == hMixer->task )
    {
        return BERR_SUCCESS;
    }

    /* Apply settings to FW Mixer */
    errCode = BDSP_Task_GetStageSettings(hMixer->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_DSP_P_SET_VARIABLE(userConfig, i32UserMixBalance, pSettings->multiStreamBalance);

    errCode = BDSP_Task_SetStageSettings(hMixer->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

#define NOT_SUPPORTED(x) NULL
/* If any of the APIs are not supported by a particular type of mixer,
 * just add a placeholder like this:
 *   NOT_SUPPORTED(BAPE_DspMixer_P_RemoveAllInputs)
 */

static const BAPE_MixerInterface g_dspMixerInterface = {
    BAPE_DspMixer_P_Destroy,
    BAPE_DspMixer_P_Start,
    BAPE_DspMixer_P_Stop,
    BAPE_DspMixer_P_AddInput,
    BAPE_DspMixer_P_RemoveInput,
    BAPE_DspMixer_P_RemoveAllInputs,
    NOT_SUPPORTED(BAPE_DspMixer_P_AddOutput),
    NOT_SUPPORTED(BAPE_DspMixer_P_RemoveOutput),
    NOT_SUPPORTED(BAPE_DspMixer_P_RemoveAllOutputs),
    BAPE_DspMixer_P_GetInputVolume,
    BAPE_DspMixer_P_SetInputVolume,
    NOT_SUPPORTED(BAPE_DspMixer_P_ApplyOutputVolume),
    BAPE_DspMixer_P_SetSettings
};

