/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_input_capture.c $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 9/21/12 3:05p $
 *
 * Module Description: Audio Input Capture Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_input_capture.c $
 * 
 * Hydra_Software_Devel/27   9/21/12 3:05p jgarrett
 * SW7425-3965: Correcting buffer division logic for interleaved formats
 * 
 * Hydra_Software_Devel/26   5/21/12 4:04p gskerl
 * SW7425-2075: Removed redundant copying of *pSettings to handle-
 * >startSettings.
 * 
 * Hydra_Software_Devel/25   5/4/12 6:03p gskerl
 * SW7425-2075: Added definition of BAPE_InputCapture_GetStatus() function
 * 
 * Hydra_Software_Devel/24   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/3   4/12/12 1:55p jgarrett
 * SW7425-2075: Revising InputPort format handling, adding HBR
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/9/12 2:04p jgarrett
 * SW7425-2075: Merging latest changes from main branch and enabling
 * 4x/16x OP Types
 * 
 * Hydra_Software_Devel/23   4/2/12 1:08p jgarrett
 * SW7425-2804: Invalidaing master connection on stop
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/22   3/8/12 5:03p gskerl
 * SW7429-18: Rearranged code so that BAPE_InputPort_P_AttachConsumer() is
 * called before accessing the InputPort format fields.
 * 
 * Hydra_Software_Devel/21   2/23/12 4:29p gskerl
 * SW7429-18: Use saved audio format from handle instead of from
 * PathConnector.
 * 
 * Hydra_Software_Devel/20   2/9/12 11:21a jgarrett
 * SW7425-2074: Enabling input halt and revising MAI shutown for
 * multichannel PCM
 * 
 * Hydra_Software_Devel/19   2/7/12 4:23p jgarrett
 * SW7425-2074: Removing halt on format change as it confuses the HDMI Rx
 * core, adding variable format handling
 * 
 * Hydra_Software_Devel/18   1/17/12 1:39p gskerl
 * SW7425-1178: Added call to BAPE_Mixer_P_PrintMixers() when input
 * capture is started.
 * 
 * Hydra_Software_Devel/17   11/14/11 3:39p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/16   10/12/11 11:40a gskerl
 * SW7425-1482: Correct indexing into dfifoSettings.input.ids[] to use
 * channel pair instead of constant zero
 * 
 * Hydra_Software_Devel/15   7/8/11 4:53p gskerl
 * SW7425-321: Added printing of downstream nodes when an audio path is
 * started (if bape_startprint is enabled)
 * 
 * Hydra_Software_Devel/14   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/13   4/8/11 3:50p gskerl
 * SW35330-35: Fixed some issues with sfifo configuration
 * 
 * Hydra_Software_Devel/12   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/11   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/10   3/18/11 6:05p gskerl
 * SW7422-146: Changed audio input connector callbacks to take the connect
 * handle as an argument
 * 
 * Hydra_Software_Devel/9   3/17/11 3:31p gskerl
 * SW7422-146: Removing DMA_READ_DISABLE
 * 
 * Hydra_Software_Devel/8   3/10/11 7:03p jgarrett
 * SW7422-146: Refactored DFIFO code, added support for input capture from
 * compressed/multichannel
 * 
 * Hydra_Software_Devel/7   3/3/11 6:30p jgarrett
 * SW7422-146: Adding DMA_READ_DISABLE
 * 
 * Hydra_Software_Devel/6   3/1/11 3:25p jgarrett
 * SW7422-146: Moving sample rate to input port from input capture
 * 
 * Hydra_Software_Devel/5   2/28/11 3:04p jgarrett
 * SW7422-146: Programming CAPTURE_RUN bit on start/stop
 * 
 * Hydra_Software_Devel/4   2/28/11 2:59p jgarrett
 * SW7422-146: Initializing number of channel pairs
 * 
 * Hydra_Software_Devel/3   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 * Hydra_Software_Devel/2   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/1   1/7/11 11:37a jgarrett
 * SW7422-146: Adding input capture
 * 
 ***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bchp_aud_fmm_bf_ctrl.h"

BDBG_MODULE(bape_input_capture);

BDBG_OBJECT_ID(BAPE_InputCapture);

static BERR_Code BAPE_InputCapture_P_ConfigPathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    );

static BERR_Code BAPE_InputCapture_P_StartPathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    );

static void BAPE_InputCapture_P_StopPathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    );

void BAPE_InputCapture_GetDefaultOpenSettings(
    BAPE_InputCaptureOpenSettings *pSettings
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

static BERR_Code BAPE_InputCapture_P_InputFormatChange_isr(
    BAPE_PathNode *pNode,
    BAPE_InputPort inputPort
    );

BERR_Code BAPE_InputCapture_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_InputCaptureOpenSettings *pSettings,
    BAPE_InputCaptureHandle *pHandle                    /* [out] */
    )
{
    BAPE_InputCaptureOpenSettings openSettings;
    BAPE_InputCaptureHandle handle;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);

    if ( NULL == pSettings )
    {
        BAPE_InputCapture_GetDefaultOpenSettings(&openSettings);
        pSettings = &openSettings;
    }

    *pHandle = NULL;
    if ( index >= BAPE_CHIP_MAX_INPUT_CAPTURES )
    {
        BDBG_WRN(("This chip can not support more than %u input capture channels", BAPE_CHIP_MAX_DFIFOS));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_index;
    }

    handle = BKNI_Malloc(sizeof(BAPE_InputCapture));
    if ( NULL == handle )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_malloc;
    }

    BKNI_Memset(handle, 0, sizeof(BAPE_InputCapture));
    BDBG_OBJECT_SET(handle, BAPE_InputCapture);
    handle->deviceHandle = deviceHandle;
    handle->index = index;
    BKNI_Memcpy(&handle->openSettings, pSettings, sizeof(BAPE_InputCaptureOpenSettings));
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_eInputCapture, 0, 1, deviceHandle, handle);
    BKNI_Snprintf(handle->name, sizeof(handle->name), "InputCapture %u", index);
    handle->node.pName = handle->name;
    handle->node.connectors[0].useBufferPool = true;
    BAPE_Connector_P_GetFormat(&handle->node.connectors[0], &format);
    format.source = BAPE_DataSource_eDfifo;
    format.type = BAPE_DataType_ePcmStereo;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[0], &format);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_format;
    }
    BAPE_PathNode_P_GetInputCapabilities(&handle->node, &caps);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eFci);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcm5_1);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcm7_1);
#if BAPE_CHIP_DFIFO_SUPPORTS_16BIT_CAPTURE
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937x4);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937x16);
#endif
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->node, &caps);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_caps;
    }
    /* Init node callbacks */
    handle->node.configPathToOutput = BAPE_InputCapture_P_ConfigPathToOutput;
    handle->node.startPathToOutput = BAPE_InputCapture_P_StartPathToOutput;
    handle->node.stopPathToOutput = BAPE_InputCapture_P_StopPathToOutput;
    handle->node.inputPortFormatChange_isr = BAPE_InputCapture_P_InputFormatChange_isr;
    *pHandle = handle;
    deviceHandle->inputCaptures[index] = handle;
    return BERR_SUCCESS;

err_format:
err_caps:
    BAPE_InputCapture_Close(handle);
err_malloc:    
err_index:
    return errCode;
}

/***************************************************************************
Summary:
Close an input capture channel
***************************************************************************/
void BAPE_InputCapture_Close(
    BAPE_InputCaptureHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_InputCapture);
    if ( handle->running )
    {
        BDBG_WRN(("Stopping input capture %p (%d) on close", handle, handle->index));
        BAPE_InputCapture_Stop(handle);
    }

    /* Disconnect from all mixers, post-processors, and groups */
    BAPE_Connector_P_RemoveAllConnections(&handle->node.connectors[0]);
    
    handle->deviceHandle->inputCaptures[handle->index] = NULL;
    BDBG_OBJECT_DESTROY(handle, BAPE_InputCapture);
    BKNI_Free(handle);
}

/***************************************************************************
Summary:
Get Default InputCapture Start Settings
***************************************************************************/
void BAPE_InputCapture_GetDefaultStartSettings(
    BAPE_InputCaptureStartSettings *pSettings       /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

/***************************************************************************
Summary:
Start InputCapture
***************************************************************************/
BERR_Code BAPE_InputCapture_Start(
    BAPE_InputCaptureHandle handle,
    const BAPE_InputCaptureStartSettings *pSettings
    )
{
    BERR_Code errCode;
    unsigned i;
    BAPE_Handle deviceHandle;
    BAPE_BufferNode *pBuffer;
    BREG_Handle regHandle;
    unsigned numChannelPairs;
    BAPE_DfifoGroupCreateSettings dfifoCreateSettings;
    BAPE_DfifoGroupSettings dfifoSettings;
    BAPE_FMT_Descriptor format;

    BDBG_OBJECT_ASSERT(handle, BAPE_InputCapture);
    BDBG_ASSERT(NULL != pSettings);

    deviceHandle = handle->deviceHandle;
    regHandle = deviceHandle->regHandle;

    /* If we're already running, return error */
    if ( handle->running )
    {
        BDBG_ERR(("InputCapture %p (%d) is already running.  Can't start.", handle, handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Check for input */
    if ( NULL == pSettings->input )
    {
        BDBG_ERR(("No input provided"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }    

    BDBG_OBJECT_ASSERT(pSettings->input, BAPE_InputPort);

    /* Don't reference InputPort format fields until after this.  */
    errCode = BAPE_InputPort_P_AttachConsumer(pSettings->input, &handle->node, &handle->inputPortFormat);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_attach;
    }

    BDBG_MSG(("InputCapture %u attached to input %s using format %s [%u Hz]", handle->index, pSettings->input->pName, BAPE_FMT_P_GetTypeName(&handle->inputPortFormat), handle->inputPortFormat.sampleRate));

    /* Setup output format */
    BAPE_Connector_P_GetFormat(&handle->node.connectors[0], &format);
    format = handle->inputPortFormat;
    format.source = BAPE_DataSource_eDfifo;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[0], &format);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_format;
    }

    BKNI_Memcpy(&handle->startSettings, pSettings, sizeof(handle->startSettings));

    BDBG_MSG(("InputCapture %p (%d) Starting", handle, handle->index));

    BAPE_DfifoGroup_P_GetDefaultCreateSettings(&dfifoCreateSettings);
    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&handle->inputPortFormat);
    BDBG_MSG(("Input requires %u channel pairs", numChannelPairs));
    dfifoCreateSettings.numChannelPairs = numChannelPairs;
    errCode = BAPE_DfifoGroup_P_Create(handle->deviceHandle, &dfifoCreateSettings, &handle->dfifoGroup);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_dfifo_alloc;
    }
    
   /* Build downstream network */
    errCode = BAPE_PathNode_P_AcquirePathResources(&handle->node);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_acquire_resources;
    }
    
    /* Prepare network to start */
    errCode = BAPE_PathNode_P_ConfigurePathResources(&handle->node);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_configure_resources;
    }

    if ( NULL == handle->pMasterConnection )
    {
        BDBG_ERR(("No outputs have been connected to this input capture."));
        goto err_no_master;
    }

    /* Configure DFIFOs after all outputs are configured so we know which is the master */
    BAPE_DfifoGroup_P_GetSettings(handle->dfifoGroup, &dfifoSettings);
    dfifoSettings.highPriority = (handle->inputPortFormat.sampleRate >= 96000)?true:false;
    dfifoSettings.linkedSfifo = handle->pMasterConnection->sfifoGroup;
    if ( BAPE_FMT_P_IsLinearPcm(&handle->inputPortFormat) )
    {
        dfifoSettings.dataWidth = 32;
        dfifoSettings.interleaveData = false;
        for ( i = 0; i < numChannelPairs; i++ )
        {
            uint32_t length;
            unsigned bufferNum = i*2;
            pBuffer = handle->node.connectors[0].pBuffers[i];
            length = pBuffer->bufferSize/2;
            dfifoSettings.bufferInfo[bufferNum].base = pBuffer->offset;
            dfifoSettings.bufferInfo[bufferNum+1].base = pBuffer->offset + length;
            dfifoSettings.bufferInfo[bufferNum].length = length;
            dfifoSettings.bufferInfo[bufferNum+1].length = length;
        }
    }
    else
    {
        dfifoSettings.dataWidth = 16;
        dfifoSettings.interleaveData = true;
        pBuffer = handle->node.connectors[0].pBuffers[0];
        dfifoSettings.bufferInfo[0].base = pBuffer->offset;
        dfifoSettings.bufferInfo[0].length = pBuffer->bufferSize;
    }
    BAPE_InputPort_P_GetFciIds(pSettings->input, &dfifoSettings.input);
    errCode = BAPE_DfifoGroup_P_SetSettings(handle->dfifoGroup, &dfifoSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_apply_dfifo_settings;
    }

    #if BDBG_DEBUG_BUILD
        BAPE_Mixer_P_PrintMixers(handle->deviceHandle);
    #endif

    /* Start */
    errCode = BAPE_PathNode_P_StartPaths(&handle->node);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_start_paths;
    }

    #if BDBG_DEBUG_BUILD
        BAPE_Mixer_P_PrintDownstreamNodes(&handle->node);
    #endif

    /* Start DFIFO */
    errCode = BAPE_DfifoGroup_P_Start(handle->dfifoGroup, false);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_start_dfifo;
    }

    /* Start Input via callback */
    BDBG_ASSERT(NULL != pSettings->input->enable);
    pSettings->input->enable(pSettings->input);

    handle->running = true;
    return BERR_SUCCESS;

err_start_dfifo:
    BAPE_PathNode_P_StopPaths(&handle->node);
err_start_paths:
err_apply_dfifo_settings:
err_no_master:
    BAPE_PathNode_P_ReleasePathResources(&handle->node);
err_configure_resources:
err_acquire_resources:
err_format:
    /* Release resources */
    BAPE_DfifoGroup_P_Destroy(handle->dfifoGroup);
    handle->dfifoGroup = NULL;
err_dfifo_alloc:
    (void)BAPE_InputPort_P_DetachConsumer(pSettings->input, &handle->node);
err_attach:
    return errCode;
}

/***************************************************************************
Summary:
Stop InputCapture
***************************************************************************/
void BAPE_InputCapture_Stop(
    BAPE_InputCaptureHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_InputCapture);
    
    if ( !handle->running )
    {
        BDBG_WRN(("Input Capture %u already stopped", handle->index));
        return;
    }

    /* Stop input */
    BDBG_ASSERT(NULL != handle->startSettings.input);
    BDBG_ASSERT(NULL != handle->startSettings.input->disable);
    handle->startSettings.input->disable(handle->startSettings.input);
    
    /* Stop DFIFOs */
    BAPE_DfifoGroup_P_Stop(handle->dfifoGroup);
    
    /* Stop Paths Downstream */
    BAPE_PathNode_P_StopPaths(&handle->node);
        
    (void)BAPE_InputPort_P_DetachConsumer(handle->startSettings.input, &handle->node);

    /* Release resources */
    BAPE_DfifoGroup_P_Destroy(handle->dfifoGroup);
    handle->dfifoGroup = NULL;
    handle->pMasterConnection = NULL;

    handle->running = false;
}

/***************************************************************************
Summary:
Get Audio Source Connector for output data
***************************************************************************/
void BAPE_InputCapture_GetConnector(
    BAPE_InputCaptureHandle handle,
    BAPE_Connector *pConnector /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_InputCapture);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->node.connectors[0];
}

/***************************************************************************
Summary:
Get InputCapture status
***************************************************************************/
void BAPE_InputCapture_GetStatus(
    BAPE_InputCaptureHandle handle,
    BAPE_InputCaptureStatus *pStatus     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_InputCapture);
    BDBG_ASSERT(NULL != pStatus);

    BKNI_Memset(pStatus, 0, sizeof(BAPE_InputCaptureStatus));

    if (  handle->running )
    {
        pStatus->running = true;
        pStatus->halted  = handle->startSettings.input->halted;
    }
}


static BERR_Code BAPE_InputCapture_P_ConfigPathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    )
{
    BAPE_PathConnector *pSource;
    BAPE_InputCaptureHandle handle;
    unsigned numChannelPairs;
    BAPE_PathNode *pSink;
    unsigned i;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_OBJECT_ASSERT(pConnection->pSource, BAPE_PathConnector);
    BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_InputCapture);

    pSource = pConnection->pSource;
    pSink = pConnection->pSink;

    if ( pSink->type == BAPE_PathNodeType_eMixer || pSink->type == BAPE_PathNodeType_eEqualizer )
    {
        BAPE_SfifoGroupSettings sfifoSettings;

        if ( NULL == handle->pMasterConnection )
        {
            handle->pMasterConnection = pConnection;
        }

        BAPE_SfifoGroup_P_GetSettings(pConnection->sfifoGroup, &sfifoSettings);
        sfifoSettings.highPriority = (handle->inputPortFormat.sampleRate >= 96000)?true:false;
        sfifoSettings.signedData = true;
        sfifoSettings.stereoData = true;

        if ( pConnection != handle->pMasterConnection )
        {
            BDBG_MSG(("Linking playback source channel group %#x to master %#x", pConnection->sfifoGroup, handle->pMasterConnection->sfifoGroup));
            sfifoSettings.master = handle->pMasterConnection->sfifoGroup;
        }
        else
        {
            sfifoSettings.master = NULL;
        }

        numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&handle->inputPortFormat);

        if ( BAPE_FMT_P_IsLinearPcm(&handle->inputPortFormat) )
        {
            sfifoSettings.interleaveData = false;
            sfifoSettings.dataWidth = 32;
            sfifoSettings.sampleRepeatEnabled = true;
            /* Setup buffers from pool */
            for ( i = 0; i < numChannelPairs; i++ )
            {
                unsigned bufferId = 2*i;
                BAPE_BufferNode *pBuffer = pSource->pBuffers[i];
                BDBG_ASSERT(NULL != pBuffer);
                sfifoSettings.bufferInfo[bufferId].base = pBuffer->offset;
                sfifoSettings.bufferInfo[bufferId].length = pBuffer->bufferSize/2;
                sfifoSettings.bufferInfo[bufferId].wrpoint = sfifoSettings.bufferInfo[bufferId].base+(pBuffer->bufferSize/2)-1;
                bufferId++;
                sfifoSettings.bufferInfo[bufferId].base = pBuffer->offset+(pBuffer->bufferSize/2);
                sfifoSettings.bufferInfo[bufferId].length = pBuffer->bufferSize/2;
                sfifoSettings.bufferInfo[bufferId].wrpoint = sfifoSettings.bufferInfo[bufferId].base+(pBuffer->bufferSize/2)-1;
            }
        }
        else
        {
            sfifoSettings.interleaveData = true;
            sfifoSettings.dataWidth = 16;
            sfifoSettings.sampleRepeatEnabled = false;
            /* Setup buffers from pool */
            for ( i = 0; i < numChannelPairs; i++ )
            {
                unsigned bufferId = 2*i;
                BAPE_BufferNode *pBuffer = pSource->pBuffers[i];
                BDBG_ASSERT(NULL != pBuffer);
                sfifoSettings.bufferInfo[bufferId].base = pBuffer->offset;
                sfifoSettings.bufferInfo[bufferId].length = pBuffer->bufferSize;
                sfifoSettings.bufferInfo[bufferId].wrpoint = pBuffer->offset+pBuffer->bufferSize-1;
                bufferId++;
                sfifoSettings.bufferInfo[bufferId].base = 0;
                sfifoSettings.bufferInfo[bufferId].length = 0;
                sfifoSettings.bufferInfo[bufferId].wrpoint = 0;
            }
        }
        BAPE_SfifoGroup_P_SetSettings(pConnection->sfifoGroup, &sfifoSettings);
    }
    /* Other nodes don't require anything done here */

    return BERR_SUCCESS;    
}

static BERR_Code BAPE_InputCapture_P_StartPathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    )
{
    /* This currently works the same as the DSP.  Should really be separated into a SFIFO routine not DSP... */
    return BAPE_DSP_P_StartPathToOutput(pNode, pConnection);
}

static void BAPE_InputCapture_P_StopPathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    )
{   
    /* This currently works the same as the DSP.  Should really be separated into a SFIFO routine not DSP... */
    BAPE_DSP_P_StopPathToOutput(pNode, pConnection);
}

static BERR_Code BAPE_InputCapture_P_InputFormatChange_isr(
    BAPE_PathNode *pNode,
    BAPE_InputPort inputPort
    )
{
    BAPE_InputCaptureHandle handle;    
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    BKNI_ASSERT_ISR_CONTEXT();
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_InputCapture);
    /* On the fly format changes are not possible */
    if ( inputPort->format.type != handle->inputPortFormat.type )
    {
        BDBG_MSG(("Input data format has changed (%s->%s).  Halting capture.", BAPE_FMT_P_GetTypeName(&handle->inputPortFormat), BAPE_FMT_P_GetTypeName(&inputPort->format)));
        if ( handle->interrupts.inputHalted.pCallback_isr )
        {
            handle->interrupts.inputHalted.pCallback_isr(handle->interrupts.inputHalted.pParam1, handle->interrupts.inputHalted.param2);
        }
        /* Intentionally omitted BERR_TRACE */
        return BERR_NOT_SUPPORTED;
    }

    if ( inputPort->format.sampleRate != handle->inputPortFormat.sampleRate )
    {
    /* Sample rate has changed.  Propagate this. -- TODO if HQ SRC is required this can't be done - also may introduce buffering issues. */
        handle->inputPortFormat.sampleRate = inputPort->format.sampleRate;
        BAPE_Connector_P_SetSampleRate_isr(&handle->node.connectors[0], handle->inputPortFormat.sampleRate);
    }
    return BERR_SUCCESS;
}

void BAPE_InputCapture_GetInterruptHandlers(
    BAPE_InputCaptureHandle handle,
    BAPE_InputCaptureInterruptHandlers *pInterrupts     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_InputCapture);
    BDBG_ASSERT(NULL != handle);
    *pInterrupts = handle->interrupts;
}

BERR_Code BAPE_InputCapture_SetInterruptHandlers(
    BAPE_InputCaptureHandle handle,
    const BAPE_InputCaptureInterruptHandlers *pInterrupts
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_InputCapture);
    BDBG_ASSERT(NULL != handle);
    BKNI_EnterCriticalSection();
    handle->interrupts = *pInterrupts;
    BKNI_LeaveCriticalSection();    
    return BERR_SUCCESS;
}

