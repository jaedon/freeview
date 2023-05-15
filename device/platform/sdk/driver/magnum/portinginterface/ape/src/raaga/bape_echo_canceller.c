/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_echo_canceller.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 5/23/12 3:13p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_echo_canceller.c $
 * 
 * Hydra_Software_Devel/5   5/23/12 3:13p jgarrett
 * SW7425-2075: Fixing initialization of local node structure
 * 
 * Hydra_Software_Devel/4   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:53p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/3   2/3/12 4:30p jgarrett
 * SW7425-2268: Initial bringup of voice conferencing support
 * 
 * Hydra_Software_Devel/2   2/2/12 4:49p jgarrett
 * SW7425-2268: Adding EchoCanceller
 * 
 * Hydra_Software_Devel/1   1/31/12 6:17p jgarrett
 * SW7425-2268: Adding initial voice conferencing support
 * 
 ***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_echo_canceller);
BDBG_OBJECT_ID(BAPE_EchoCanceller);

static BDSP_AudioProcessing BAPE_EchoCanceller_P_GetType(BAPE_EchoCancellerAlgorithm algorithm);
static BERR_Code BAPE_EchoCanceller_P_ApplyDspSettings(BAPE_EchoCancellerHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId);
static void BAPE_EchoCanceller_P_GetDefaultAlgorithmSettings(BAPE_EchoCancellerHandle handle);
static bool BAPE_EchoCanceller_P_IsRunning(BAPE_EchoCancellerHandle handle);
static void BAPE_EchoCanceller_P_InitInterTaskDescriptors(BAPE_EchoCancellerHandle handle);

static BERR_Code BAPE_EchoCanceller_P_Local_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_EchoCanceller_P_Local_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_EchoCanceller_P_Local_StartPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_EchoCanceller_P_Local_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_EchoCanceller_P_Local_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

static BERR_Code BAPE_EchoCanceller_P_Remote_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_EchoCanceller_P_Remote_StartPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_EchoCanceller_P_Remote_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_EchoCanceller_P_Remote_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

void BAPE_EchoCanceller_GetDefaultSettings(
    BAPE_EchoCancellerSettings *pSettings   /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->algorithm = BAPE_EchoCancellerAlgorithm_eSpeex;
}

BERR_Code BAPE_EchoCanceller_Create(
    BAPE_Handle deviceHandle,
    const BAPE_EchoCancellerSettings *pSettings,
    BAPE_EchoCancellerHandle *pHandle               /* [out] */
    )
{
    BERR_Code errCode = BERR_SUCCESS;
    BAPE_EchoCancellerHandle handle;
    BAPE_EchoCancellerSettings defaults;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);

    if ( NULL == pSettings )
    {
        BAPE_EchoCanceller_GetDefaultSettings(&defaults);
        pSettings = &defaults;
    }

    handle = BKNI_Malloc(sizeof(BAPE_EchoCanceller));
    if ( NULL == handle )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_handle;
    }

    BKNI_Memset(handle, 0, sizeof(BAPE_EchoCanceller));
    BDBG_OBJECT_SET(handle, BAPE_EchoCanceller);
    handle->deviceHandle = deviceHandle;

    /* Initialize node for local connections.  This is the primary node that can attach to outputs. */
    BAPE_P_InitPathNode(&handle->localNode, BAPE_PathNodeType_eEchoCanceller, pSettings->algorithm, 1, deviceHandle, handle);   
    handle->localNode.pName = "Echo Canceller (local)";

    handle->localNode.connectors[0].useBufferPool = true;
    BAPE_Connector_P_GetFormat(&handle->localNode.connectors[0], &format);
    format.source = BAPE_DataSource_eDspBuffer;
    format.type = BAPE_DataType_ePcmMono;    
    errCode = BAPE_Connector_P_SetFormat(&handle->localNode.connectors[0], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    /* Generic Routines */
    handle->localNode.allocatePathToOutput = BAPE_DSP_P_AllocatePathToOutput;
    handle->localNode.configPathToOutput = BAPE_DSP_P_ConfigPathToOutput;
    handle->localNode.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;
    handle->localNode.startPathToOutput = BAPE_DSP_P_StartPathToOutput;
    handle->localNode.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;

    /* Echo Canceller Specifics */
    handle->localNode.allocatePathFromInput = BAPE_EchoCanceller_P_Local_AllocatePathFromInput;
    handle->localNode.configPathFromInput = BAPE_EchoCanceller_P_Local_ConfigPathFromInput;
    handle->localNode.startPathFromInput = BAPE_EchoCanceller_P_Local_StartPathFromInput;
    handle->localNode.stopPathFromInput = BAPE_EchoCanceller_P_Local_StopPathFromInput;
    handle->localNode.removeInput = BAPE_EchoCanceller_P_Local_RemoveInputCallback;

    /* Initialize node for remote inputs.  This node can not connect downstream, and is an inter-task link between a decoder and the ecno-canceller. */
    BAPE_P_InitPathNode(&handle->remoteNode, BAPE_PathNodeType_eEchoCanceller, pSettings->algorithm, 1, deviceHandle, handle);
    handle->remoteNode.pName = "Echo Canceller (remote)";
    handle->remoteNode.connectors[0].useBufferPool = true;
    BAPE_Connector_P_GetFormat(&handle->remoteNode.connectors[0], &format);
    format.source = BAPE_DataSource_eDspBuffer;
    format.type = BAPE_DataType_ePcmMono;    
    errCode = BAPE_Connector_P_SetFormat(&handle->remoteNode.connectors[0], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    BAPE_PathNode_P_GetInputCapabilities(&handle->localNode, &caps);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmMono);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDspBuffer);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->localNode, &caps);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_set_caps;
    }

    BAPE_PathNode_P_GetInputCapabilities(&handle->remoteNode, &caps);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmMono);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDspBuffer);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->remoteNode, &caps);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_set_caps;
    }

    /* No generic routines, we can not connect to outputs */

    /* Echo Canceller Specifics */
    handle->remoteNode.allocatePathFromInput = BAPE_EchoCanceller_P_Remote_AllocatePathFromInput;
    handle->remoteNode.startPathFromInput = BAPE_EchoCanceller_P_Remote_StartPathFromInput;
    handle->remoteNode.stopPathFromInput = BAPE_EchoCanceller_P_Remote_StopPathFromInput;
    handle->remoteNode.removeInput = BAPE_EchoCanceller_P_Remote_RemoveInputCallback;

    /* Init algorithm settings */
    BKNI_Memcpy(&handle->settings, pSettings, sizeof(BAPE_EchoCancellerSettings));
    BAPE_EchoCanceller_P_GetDefaultAlgorithmSettings(handle);

    /* Allocate required buffers */
    handle->pInterTaskGenericBuffer = BMEM_Heap_Alloc(deviceHandle->memHandle, BDSP_AF_P_INTERTASK_IOGENBUFFER_SIZE);
    if ( NULL == handle->pInterTaskGenericBuffer )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_generic_buffer;
    }

    handle->pInterTaskIoBuffer = BMEM_Heap_Alloc(deviceHandle->memHandle, BAPE_P_INTER_TASK_BUFFER_SIZE);
    if ( NULL == handle->pInterTaskIoBuffer )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_io_buffer;
    }

    handle->pInterTaskIoDescriptor = BMEM_Heap_Alloc(deviceHandle->memHandle, sizeof(BDSP_AF_P_sIO_BUFFER));
    if ( NULL == handle->pInterTaskIoDescriptor )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_io_descriptor;
    }

    handle->pInterTaskGenericDescriptor = BMEM_Heap_Alloc(deviceHandle->memHandle, sizeof(BDSP_AF_P_sIO_GENERIC_BUFFER));
    if ( NULL == handle->pInterTaskGenericDescriptor )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_generic_descriptor;
    }

    BAPE_EchoCanceller_P_InitInterTaskDescriptors(handle);

    *pHandle = handle;
    return BERR_SUCCESS;

err_generic_descriptor:
    BMEM_Heap_Free(deviceHandle->memHandle, handle->pInterTaskIoDescriptor);
err_io_descriptor:
    BMEM_Heap_Free(deviceHandle->memHandle, handle->pInterTaskIoBuffer);
err_io_buffer:
    BMEM_Heap_Free(deviceHandle->memHandle, handle->pInterTaskGenericBuffer);
err_generic_buffer:
err_set_caps:
err_connector_format:
    BDBG_OBJECT_DESTROY(handle, BAPE_EchoCanceller);
    BKNI_Free(handle);
err_handle:
    return errCode;
}

void BAPE_EchoCanceller_Destroy(
    BAPE_EchoCancellerHandle handle
    )
{
    bool running;
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);

    running = handle->localStarted || handle->remoteStarted;
    BDBG_ASSERT(false == running);
    BDBG_ASSERT(NULL == handle->localInput);
    BDBG_ASSERT(NULL == handle->remoteInput);

    BMEM_Heap_Free(handle->deviceHandle->memHandle, handle->pInterTaskGenericDescriptor);
    BMEM_Heap_Free(handle->deviceHandle->memHandle, handle->pInterTaskIoDescriptor);
    BMEM_Heap_Free(handle->deviceHandle->memHandle, handle->pInterTaskIoBuffer);
    BMEM_Heap_Free(handle->deviceHandle->memHandle, handle->pInterTaskGenericBuffer);
    BDBG_OBJECT_DESTROY(handle, BAPE_EchoCanceller);
    BKNI_Free(handle);
}

void BAPE_EchoCanceller_GetSettings(
    BAPE_EchoCancellerHandle handle,
    BAPE_EchoCancellerSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

BERR_Code BAPE_EchoCanceller_SetSettings(
    BAPE_EchoCancellerHandle handle,
    const BAPE_EchoCancellerSettings *pSettings
    )
{
    bool running;

    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    BDBG_ASSERT(NULL != pSettings);

    running = BAPE_EchoCanceller_P_IsRunning(handle);
    if ( running && pSettings->algorithm != handle->settings.algorithm )
    {
        BDBG_ERR(("Can not change algorithm while running."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    handle->settings = *pSettings;

    return BERR_SUCCESS;
}

void BAPE_EchoCanceller_GetAlgorithmSettings(
    BAPE_EchoCancellerHandle handle,
    BAPE_EchoCancellerAlgorithm algorithm,
    BAPE_EchoCancellerAlgorithmSettings *pSettings  /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    BDBG_ASSERT(NULL != pSettings);

    pSettings->algorithm = algorithm;
    switch ( algorithm )
    {
    case BAPE_EchoCancellerAlgorithm_eSpeex:
        pSettings->algorithmSettings.speex = handle->speexSettings;
        break;
    default:
        break;
    }
}

BERR_Code BAPE_EchoCanceller_SetAlgorithmSettings(
    BAPE_EchoCancellerHandle handle,
    const BAPE_EchoCancellerAlgorithmSettings *pSettings
    )
{
    bool running, updateDsp=false;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    BDBG_ASSERT(NULL != pSettings);

    running = BAPE_EchoCanceller_P_IsRunning(handle);
    if ( handle->settings.algorithm == pSettings->algorithm )
    {
        updateDsp = true;
    }

    switch ( pSettings->algorithm )
    {
    case BAPE_EchoCancellerAlgorithm_eSpeex:
        handle->speexSettings = pSettings->algorithmSettings.speex;
        break;
    default:
        updateDsp = false;
        break;
    }

    if ( running && updateDsp )
    {
        errCode = BAPE_EchoCanceller_P_ApplyDspSettings(handle,
                                                        handle->localNode.connectors[0].task, 
                                                        handle->localNode.connectors[0].branchId, 
                                                        handle->localNode.connectors[0].stageId);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

void BAPE_EchoCanceller_GetConnector(
    BAPE_EchoCancellerHandle handle,
    BAPE_Connector *pConnector   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->localNode.connectors[0];
}

BERR_Code BAPE_EchoCanceller_AddLocalInput(
    BAPE_EchoCancellerHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    if ( NULL != handle->localInput )
    {
        BDBG_ERR(("Can not have more than one local input"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    errCode = BAPE_PathNode_P_AddInput(&handle->localNode, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->localInput = input;
    return BERR_SUCCESS;
}

BERR_Code BAPE_EchoCanceller_RemoveLocalInput(
    BAPE_EchoCancellerHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    if ( input != handle->localInput )
    {
        BDBG_ERR(("Input %s %s (%#x) is not connected as the local input", input->pParent->pName, input->pName, input));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    errCode = BAPE_PathNode_P_RemoveInput(&handle->localNode, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->localInput = NULL;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Add a remote (far-end) input to the echo canceller
***************************************************************************/
BERR_Code BAPE_EchoCanceller_AddRemoteInput(
    BAPE_EchoCancellerHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    if ( NULL != handle->remoteInput )
    {
        BDBG_ERR(("Can not have more than one remote input"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    errCode = BAPE_PathNode_P_AddInput(&handle->remoteNode, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->remoteInput = input;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Remove a remote (far-end) input from this processing stage
***************************************************************************/
BERR_Code BAPE_EchoCanceller_RemoveRemoteInput(
    BAPE_EchoCancellerHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    if ( input != handle->remoteInput )
    {
        BDBG_ERR(("Input %s %s (%#x) is not connected as the remote input", input->pParent->pName, input->pName, input));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    errCode = BAPE_PathNode_P_RemoveInput(&handle->remoteNode, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->remoteInput = NULL;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
BERR_Code BAPE_EchoCanceller_RemoveAllInputs(
    BAPE_EchoCancellerHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    if ( handle->localInput )
    {
        (void)BAPE_EchoCanceller_RemoveLocalInput(handle, handle->localInput);
    }
    if ( handle->remoteInput )
    {
        (void)BAPE_EchoCanceller_RemoveRemoteInput(handle, handle->remoteInput);
    }
    return BERR_SUCCESS;
}

static BDSP_AudioProcessing BAPE_EchoCanceller_P_GetType(BAPE_EchoCancellerAlgorithm algorithm)
{
    switch ( algorithm )
    {
    case BAPE_EchoCancellerAlgorithm_eSpeex:
        return BDSP_AudioProcessing_eSpeexAec;
    default:
        return BDSP_AudioProcessing_eMax;
    }    
}

static BERR_Code BAPE_EchoCanceller_P_ApplySpeexSettings(BAPE_EchoCancellerHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_SpeexAECConfigParams userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    userConfig.ui32GainResolution = handle->speexSettings.gainMode == BAPE_SpeexEchoCancellerGainMode_eBark?0:1;

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_EchoCanceller_P_ApplyDspSettings(BAPE_EchoCancellerHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);

    BDBG_ASSERT(NULL != task);
    
    switch ( handle->settings.algorithm )
    {
    case BAPE_EchoCancellerAlgorithm_eSpeex:
        return BAPE_EchoCanceller_P_ApplySpeexSettings(handle, task, branchId, stageId);
    default:
        return BERR_SUCCESS;
    }
}

static BERR_Code BAPE_EchoCanceller_P_Local_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_EchoCancellerHandle handle;
    BERR_Code errCode;
    unsigned sourceIndex;
    BDSP_CIT_P_FwStageInfo *pStageInfo;
    BDSP_CIT_P_FwStgSrcDstDetails *pDetails; 

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = pNode->pHandle;

    /* More Sanity */
    BDBG_ASSERT(pNode == &handle->localNode);
    BDBG_ASSERT(pConnection->pSource == handle->localInput);

    /* First, make sure we have both inputs known before attempting to start. */
    if ( NULL == handle->remoteInput )
    {
        BDBG_ERR(("Both the local and remote inputs must be connected to the DSP in order to start an echo canceller."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Local (microphone) inputs connect in a standard fashion as a post-processing stage.  The local task drives the process. */
    /* Add stage to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                            BDSP_AF_P_DistinctOpType_eMono_PCM,
                                            BAPE_EchoCanceller_P_GetType(handle->settings.algorithm),
                                            false,
                                            &pNode->connectors[0].branchId, &pNode->connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pNode->connectors[0].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;        

    /* Also add the inter-task buffer as input for this stage */
    pStageInfo = &pConnection->pSource->pTaskCreateSettings->pBranchInfo[pNode->connectors[0].branchId]->sFwStgInfo[pNode->connectors[0].stageId];
    sourceIndex = pStageInfo->sStgConnectivity.ui32NumSrc;
    BDBG_ASSERT(sourceIndex < BDSP_P_MAX_FW_STG_INPUTS);
    pDetails = &pStageInfo->sStgConnectivity.sSrcDetails[sourceIndex];
    pDetails->eType = BDSP_CIT_P_FwStgSrcDstType_eInterTaskDRAMBuf;
    BMEM_Heap_ConvertAddressToOffset(handle->deviceHandle->memHandle, 
                                     handle->pInterTaskIoDescriptor,
                                     &pDetails->uDetails.sInterTaskDramBuffCfg.ui32IoBuffCfgAddr);
    BMEM_Heap_ConvertAddressToOffset(handle->deviceHandle->memHandle, 
                                     handle->pInterTaskGenericDescriptor,
                                     &pDetails->uDetails.sInterTaskDramBuffCfg.ui32IoGenericBuffCfgAddr);
    pStageInfo->sStgConnectivity.ui32NumSrc = sourceIndex+1;

    return BERR_SUCCESS;
}

static BERR_Code BAPE_EchoCanceller_P_Remote_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_EchoCancellerHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = pNode->pHandle;

    /* More Sanity */
    BDBG_ASSERT(pNode == &handle->remoteNode);
    BDBG_ASSERT(pConnection->pSource == handle->remoteInput);

    /* First, make sure we have both inputs known before attempting to start. */
    if ( NULL == handle->localInput )
    {
        BDBG_ERR(("Both the local and remote inputs must be connected to the DSP in order to start an echo canceller."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Remote inputs connect as an inter-task connection.  The local task will drive the processing. */
    pConnection->pInterTaskIoDescriptor = handle->pInterTaskIoDescriptor;
    pConnection->pInterTaskIoBuffer = handle->pInterTaskIoBuffer;
    pConnection->pInterTaskGenericDescriptor = handle->pInterTaskGenericDescriptor;
    pConnection->pInterTaskGenericBuffer = handle->pInterTaskGenericBuffer;

    return BERR_SUCCESS;
}

static BERR_Code BAPE_EchoCanceller_P_Local_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_EchoCancellerHandle handle;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);
    
    BDBG_ASSERT(pNode == &handle->localNode);
    BDBG_ASSERT(pConnection->pSource == handle->localInput);
        
    BDBG_ASSERT(NULL != pConnection->pSource->task);
    pNode->connectors[0].task = pConnection->pSource->task;
    errCode = BAPE_EchoCanceller_P_ApplyDspSettings(handle, 
                                                    handle->localNode.connectors[0].task, 
                                                    handle->localNode.connectors[0].branchId, 
                                                    handle->localNode.connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_EchoCanceller_P_Local_StartPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_EchoCancellerHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);

    BDBG_ASSERT(pNode == &handle->localNode);
    BDBG_ASSERT(pConnection->pSource == handle->localInput);

    BDBG_ASSERT(false == handle->localStarted);

    /* When the first input starts, initialize the IO Generic Buffer RD/WR pointers to defaults. */
    if ( false == handle->remoteStarted )
    {
        BAPE_EchoCanceller_P_InitInterTaskDescriptors(handle);
    }

    handle->localStarted = true;

    return BERR_SUCCESS;
}

static BERR_Code BAPE_EchoCanceller_P_Remote_StartPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_EchoCancellerHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);

    BDBG_ASSERT(pNode == &handle->remoteNode);
    BDBG_ASSERT(pConnection->pSource == handle->remoteInput);

    /* When the first input starts, initialize the IO Generic Buffer RD/WR pointers to defaults. */
    if ( false == handle->localStarted )
    {
        BAPE_EchoCanceller_P_InitInterTaskDescriptors(handle);
    }

    BDBG_ASSERT(false == handle->remoteStarted);
    handle->remoteStarted = true;

    return BERR_SUCCESS;
}

static void BAPE_EchoCanceller_P_Local_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_EchoCancellerHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);

    BDBG_ASSERT(pNode == &handle->localNode);
    BDBG_ASSERT(pConnection->pSource == handle->localInput);

    BDBG_ASSERT(handle->localStarted);
    handle->localStarted = false;
}

static void BAPE_EchoCanceller_P_Remote_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BAPE_EchoCancellerHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_EchoCanceller);

    BDBG_ASSERT(pNode == &handle->remoteNode);
    BDBG_ASSERT(pConnection->pSource == handle->remoteInput);

    BDBG_ASSERT(handle->remoteStarted);
    handle->remoteStarted = false;
}

static void BAPE_EchoCanceller_P_GetDefaultSpeexSettings(BAPE_EchoCancellerHandle handle)
{
    BDSP_Raaga_Audio_SpeexAECConfigParams userConfig;

    BERR_TRACE(BDSP_Raaga_GetDefaultAudioProcessingSettings(BDSP_AudioProcessing_eSpeexAec, &userConfig, sizeof(userConfig)));

    handle->speexSettings.gainMode = (userConfig.ui32GainResolution == 0)?BAPE_SpeexEchoCancellerGainMode_eBark:BAPE_SpeexEchoCancellerGainMode_eLinear;
}

static void BAPE_EchoCanceller_P_GetDefaultAlgorithmSettings(BAPE_EchoCancellerHandle handle)
{
    BAPE_EchoCanceller_P_GetDefaultSpeexSettings(handle);
}

static void BAPE_EchoCanceller_P_Local_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_EchoCanceller_RemoveLocalInput(pNode->pHandle, pConnector);
}

static void BAPE_EchoCanceller_P_Remote_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_EchoCanceller_RemoveRemoteInput(pNode->pHandle, pConnector);
}

static bool BAPE_EchoCanceller_P_IsRunning(BAPE_EchoCancellerHandle handle)
{
    return handle->localStarted;
}

static void BAPE_EchoCanceller_P_InitInterTaskDescriptors(BAPE_EchoCancellerHandle handle)
{
    BDSP_AF_P_sIO_GENERIC_BUFFER *pGenericDescriptor;
    BDSP_AF_P_sIO_BUFFER *pIoDescriptor;
    void *pCached;
    uint32_t base, end;
    unsigned i;

    /* Initialize IO Buffer Descriptor */
    (void)BMEM_Heap_ConvertAddressToCached(handle->deviceHandle->memHandle, handle->pInterTaskIoDescriptor, &pCached);
    pIoDescriptor = pCached;
    pIoDescriptor->eBufferType = BDSP_AF_P_BufferType_eDRAM;
    pIoDescriptor->ui32NumBuffers = 1;  /* Mono data */
    BMEM_ConvertAddressToOffset(handle->deviceHandle->memHandle, handle->pInterTaskIoBuffer, &base);
    end = base+(BAPE_P_INTER_TASK_BUFFER_SIZE);
    pIoDescriptor->sCircBuffer[0].ui32BaseAddr = base;
    pIoDescriptor->sCircBuffer[0].ui32ReadAddr = base;
    pIoDescriptor->sCircBuffer[0].ui32WriteAddr = base;
    pIoDescriptor->sCircBuffer[0].ui32EndAddr = end;
    pIoDescriptor->sCircBuffer[0].ui32WrapAddr = end;

    for ( i = 1; i < BDSP_AF_P_MAX_CHANNELS; i++ )
    {
        BKNI_Memset(&pIoDescriptor->sCircBuffer[i], 0, sizeof(pIoDescriptor->sCircBuffer[i]));
    }
    BMEM_Heap_FlushCache(handle->deviceHandle->memHandle, pIoDescriptor, sizeof(BDSP_AF_P_sIO_BUFFER));

    /* Initialize IO Generic Buffer Descriptor */
    (void)BMEM_Heap_ConvertAddressToCached(handle->deviceHandle->memHandle, handle->pInterTaskGenericDescriptor, &pCached);
    pGenericDescriptor = pCached;
    BKNI_Memset(pGenericDescriptor, 0, sizeof(BDSP_AF_P_sIO_GENERIC_BUFFER));
    pGenericDescriptor->eBufferType = BDSP_AF_P_BufferType_eDRAM;
    pGenericDescriptor->ui32NumBuffers = 1;
    BMEM_ConvertAddressToOffset(handle->deviceHandle->memHandle, handle->pInterTaskGenericBuffer, &base);
    end = base+BDSP_AF_P_INTERTASK_IOGENBUFFER_SIZE;
    pGenericDescriptor->sCircBuffer.ui32BaseAddr = base;
    pGenericDescriptor->sCircBuffer.ui32ReadAddr = base;
    pGenericDescriptor->sCircBuffer.ui32WriteAddr = base;
    pGenericDescriptor->sCircBuffer.ui32EndAddr = end;
    pGenericDescriptor->sCircBuffer.ui32WrapAddr = end;
    BMEM_Heap_FlushCache(handle->deviceHandle->memHandle, pGenericDescriptor, sizeof(BDSP_AF_P_sIO_GENERIC_BUFFER));
}

