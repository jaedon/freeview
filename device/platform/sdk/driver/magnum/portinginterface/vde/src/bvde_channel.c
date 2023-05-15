/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvde_channel.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 7/14/11 1:29p $
 *
 * Module Description: Video Channel Interface for DSP
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vde/7425/bvde_channel.c $
 * 
 * Hydra_Software_Devel/7   7/14/11 1:29p speter
 * SW7358-63: [7425] Storing a local copy of the XptContextMap structure
 * 
 * Hydra_Software_Devel/6   6/16/11 10:35a speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 * Hydra_Software_Devel/5   6/15/11 10:50a speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 * Hydra_Software_Devel/4   6/15/11 10:40a speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 * Hydra_Software_Devel/3   6/10/11 7:50p speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 * Hydra_Software_Devel/2   6/2/11 3:56p speter
 * SW7425-409: [7425] Modifying the frame buffer allocation part
 * 
 * Hydra_Software_Devel/1   5/31/11 4:44p speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bvde.h"
#include "bvde_priv.h"
#include "bdsp_raaga.h"
#include "bvde_dsp_utils_priv.h"
#include "bxdm_picture.h"

BDBG_MODULE(bvde_channel);

BDBG_OBJECT_ID(BVDE_Channel);

#define BVDE_DISABLE_DSP 0  /* Enable this to check for CIT errors and avoid starting the DSP */

static void BVDE_Channel_P_SetupDefaults(BVDE_ChannelHandle handle);
static BERR_Code BVDE_Channel_P_Start(BVDE_ChannelHandle handle);
static void BVDE_Channel_P_CleanupTaskCreateSettings(BVDE_ChannelHandle handle);
static void BVDE_Channel_P_Stop(BVDE_ChannelHandle handle);

static const  BAVC_VideoCompressionStd  g_codeclist [] =
{
   BAVC_VideoCompressionStd_eVP6
};

void BVDE_Channel_GetDefaultOpenSettings(
    BVDE_ChannelOpenSettings *pSettings     /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->codecCount = sizeof(g_codeclist)/sizeof(BAVC_VideoCompressionStd); 
    pSettings->pCodecList = (BAVC_VideoCompressionStd *) g_codeclist;
    pSettings->resolution = BVDE_Resolution_ePAL;
    pSettings->memPicHandle = NULL;
 }

BERR_Code BVDE_Channel_Open(
    BVDE_Handle deviceHandle,
    unsigned index,
    const BVDE_ChannelOpenSettings *pSettings, 
    BVDE_ChannelHandle *pHandle                 /* [out] */
    )
{
    BERR_Code errCode;
    BVDE_ChannelOpenSettings defaults;
    BVDE_ChannelHandle handle;

    BDBG_OBJECT_ASSERT(deviceHandle, BVDE_Device);
    
    if ( NULL == pSettings )
    {
        BDBG_WRN (("pSettings is NULL. Using Defaults with memPicHandle as NULL"));
        BVDE_Channel_GetDefaultOpenSettings(&defaults);
        pSettings = &defaults;
    }

    if ( index >= BVDE_MAX_CHANNELS )
    {
        BDBG_ERR(("This chip only supports %u channels. Cannot open channel %u", BVDE_MAX_CHANNELS, index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( deviceHandle->channels[index] )
    {
        BDBG_ERR(("Channel %d already open", index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    handle = BKNI_Malloc(sizeof(BVDE_Channel));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BVDE_Channel));

    handle->deviceHandle = deviceHandle;
    handle->index = index;
    BKNI_Snprintf(handle->name, sizeof(handle->name), "Channel %u", index);
    handle->state = BVDE_ChannelState_eStopped;
    BKNI_Memcpy(&handle->settings, pSettings, sizeof(handle->settings));
    BVDE_Channel_GetDefaultStartSettings(&handle->startSettings);
    BDBG_OBJECT_SET(handle, BVDE_Channel);
    BVDE_Channel_P_SetupDefaults(handle);


    /* Allocate Memory for Display Buffers and UPBs*/
    errCode = BVDE_Channel_P_AllocateFrameBuffer (handle);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_alloc_framebuf;
    }
    
    /* Success */
    *pHandle = handle;
    deviceHandle->channels[index] = handle;
    return BERR_SUCCESS;

err_alloc_framebuf:
    BKNI_Free (handle);

    *pHandle = NULL;

    return errCode;
}

void BVDE_Channel_Close(
    BVDE_ChannelHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);

    if ( handle->state != BVDE_ChannelState_eStopped )
    {
        BDBG_WRN(("Implicitly stopping channel %u on shutdown.", handle->index));
        BVDE_Channel_Stop(handle);
    }

    /* Cleanup */
    handle->deviceHandle->channels[handle->index] = NULL;

    BVDE_Channel_P_DeAllocateFrameBuffer (handle);
    
    BDBG_OBJECT_DESTROY(handle, BVDE_Channel);
    BKNI_Free(handle);
}

void BVDE_Channel_GetDefaultStartSettings(
    BVDE_ChannelStartSettings *pSettings    /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->codec = BAVC_VideoCompressionStd_eVP6;
    pSettings->pContextMap = NULL;
}

static BERR_Code BVDE_Channel_P_ValidateDecodeSettings(
    BVDE_ChannelHandle handle,
    const BVDE_ChannelStartSettings *pSettings,
    BVDE_ChannelStartSettings *pOutputSettings
    )
{
    BDSP_VideoType videoType;
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pOutputSettings);
    
    /* Start by copying the existing settings */
    BKNI_Memcpy(pOutputSettings, pSettings, sizeof(BVDE_ChannelStartSettings));

    /* Check for valid input */
    if ( NULL == pSettings->pContextMap )
    {
        BDBG_ERR(("Must specify an input to decode"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Store local copy of the RAVE context map in case it goes out of scope after start. */
    if ( pSettings->pContextMap )
    {
        BKNI_Memcpy(&handle->contextMap, pSettings->pContextMap, sizeof(BAVC_XptContextMap));
        pOutputSettings->pContextMap = &handle->contextMap;
    }
    

    videoType = BVDE_P_GetCodecVideoType(pSettings->codec);
        
    if ( videoType == BDSP_VideoType_eMax )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    return BERR_SUCCESS;
}

static BERR_Code BVDE_Channel_P_Start(
    BVDE_ChannelHandle handle
    )
{
    BERR_Code errCode;
    BDSP_TaskHandle task;
    const BVDE_ChannelStartSettings *pSettings;
    BXDM_Picture *hXDMPicture = NULL;
	unsigned int            	i=0,  ui32BaseAddr=0,ui32UsedSize=0;
 
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    pSettings = &handle->startSettings;

    BDBG_MSG(("BVDE_Channel_P_Start(%#x) [index %u]", handle, handle->index));

    /* Setup DSP Task First */
    BDSP_Task_GetDefaultCreateSettings(handle->deviceHandle->dspContext, &handle->taskCreateSettings);

    /* Setup default branch */
    handle->taskCreateSettings.numBranches = 1;
    handle->taskCreateSettings.pBranchInfo[0] = &handle->primaryBranch;

    BVDE_DSP_P_InitBranch(&handle->primaryBranch);
    handle->primaryBranch.ui32NumStages = 1;
    
    /* Setup Decode Stage */
    handle->primaryBranch.sFwStgInfo[0].ui32BranchId = 0;
    handle->primaryBranch.sFwStgInfo[0].ui32StageId = 0;
    handle->primaryBranch.sFwStgInfo[0].eStageType = BDSP_CIT_P_StageType_eVideoDecode;
    handle->primaryBranch.sFwStgInfo[0].uAlgorithm.eVideoAlgo = BVDE_P_GetCodecVideoType(handle->startSettings.codec);

    handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.ui32NumSrc = 1; 
    BKNI_Memset(&handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0], 0, 
                sizeof(handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0]));
    if ( pSettings->pContextMap )
    {
        /* Input Hardcoded to RAVE for now (checked above) */
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 2;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BDSP_AF_P_BufferType_eRAVE;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Base;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_End;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Read;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Valid;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Wrap;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32BaseAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Base;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32EndAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_End;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32ReadAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Read;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WriteAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Valid;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WrapAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Wrap;  
    }

	handle->taskCreateSettings.psVDecodeBuffCfg =(BDSP_VF_P_sVDecodeBuffCfg *)BKNI_Malloc(sizeof(BDSP_VF_P_sVDecodeBuffCfg));
    if (NULL == handle->taskCreateSettings.psVDecodeBuffCfg)
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
	BKNI_Memset(handle->taskCreateSettings.psVDecodeBuffCfg, 0 , sizeof(BDSP_VF_P_sVDecodeBuffCfg));

    ui32BaseAddr = (uint32_t) handle->videoMemory.pFrameBufferMemory;
    ui32UsedSize = 0;
    
    /*sLumaFrameBuffParams and sChromaFrameBuffParams*/
    handle->taskCreateSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.ui32NumBuffAvl = handle->videoMemory.totalFrames;
    for(i=0; i<handle->videoMemory.totalFrames ; i++)
    {
        hXDMPicture = &handle->videoMemory.pUpbMemory[i];

        /* Reset the entire structure */
        BKNI_Memset(hXDMPicture, 0, sizeof(BXDM_Picture));

        hXDMPicture->stBufferInfo.hHeap = handle->settings.memPicHandle?handle->settings.memPicHandle:handle->deviceHandle->memHandle;
        hXDMPicture->stBufferInfo.pLumaBufferVirtualAddress = (void *)(ui32BaseAddr + ui32UsedSize);
        handle->taskCreateSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress
            = (uint32_t)(ui32BaseAddr + ui32UsedSize);

        handle->taskCreateSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes = handle->videoMemory.lumasize;
        ui32UsedSize += handle->videoMemory.lumasize;

        hXDMPicture->stBufferInfo.pChromaBufferVirtualAddress = (void *)(ui32BaseAddr + ui32UsedSize);
        handle->taskCreateSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress
            = (uint32_t)(ui32BaseAddr + ui32UsedSize);
        handle->taskCreateSettings.psVDecodeBuffCfg->sDisplayFrameBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes = handle->videoMemory.chromasize;
        ui32UsedSize += handle->videoMemory.chromasize;

        handle->taskCreateSettings.psVDecodeBuffCfg->sUPBs[i].ui32DramBufferAddress = (uint32_t)hXDMPicture;
        handle->taskCreateSettings.psVDecodeBuffCfg->sUPBs[i].ui32BufferSizeInBytes= sizeof(BXDM_Picture);                   
    }

    handle->taskCreateSettings.psVDecodeBuffCfg->sReferenceBuffParams.ui32NumBuffAvl = handle->videoMemory.totalRefFrames;
    for(i=0; i<handle->videoMemory.totalRefFrames ; i++)
    {
        handle->taskCreateSettings.psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32DramBufferAddress = (uint32_t)(ui32BaseAddr + ui32UsedSize);
        handle->taskCreateSettings.psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffLuma.ui32BufferSizeInBytes = handle->videoMemory.lumarefsize;                           
        ui32UsedSize += handle->videoMemory.lumarefsize;

        handle->taskCreateSettings.psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32DramBufferAddress = (uint32_t)(ui32BaseAddr + ui32UsedSize);
        handle->taskCreateSettings.psVDecodeBuffCfg->sReferenceBuffParams.sBuffParams[i].sFrameBuffChroma.ui32BufferSizeInBytes = handle->videoMemory.chromarefsize;                           
        ui32UsedSize += handle->videoMemory.chromarefsize;
    }      

    /* Destination Details for this Task is not provided. If required we need to add to BDSP_CIT_P_FwStgSrcDstType a new type for video */

    /* Ready */
    errCode = BDSP_Task_Create(handle->deviceHandle->dspContext, &handle->taskCreateSettings, &task);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_task_create;
    }
    BKNI_EnterCriticalSection();    /* Serialize with display interrupt */
    handle->task = task;
    BKNI_LeaveCriticalSection();

    /* Apply codec settings */
    errCode = BVDE_Channel_P_ApplyCodecSettings(handle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_codec_settings;
    }

    
#if BVDE_DISABLE_DSP
    #warning Task Start is Disabled!
    BDBG_ERR(("NOT STARTING"));
#else
    errCode = BDSP_Task_Start(handle->task);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_start_task;
    }
#endif

    handle->state = BVDE_ChannelState_eStarted;
    return BERR_SUCCESS;

err_start_task:
err_codec_settings:    
    task = handle->task;
    BKNI_EnterCriticalSection();    /* Serialize with display interrupt */
    handle->task = NULL;
    BKNI_LeaveCriticalSection();
    BDSP_Task_Destroy(task);
err_task_create:
    BVDE_Channel_P_CleanupTaskCreateSettings(handle);    

    return errCode;
}

BERR_Code BVDE_Channel_Start(
    BVDE_ChannelHandle handle,
    const BVDE_ChannelStartSettings *pSettings
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BDBG_ASSERT(NULL != pSettings);

    BDBG_MSG(("BVDE_Channel_Start(%#x) [index %u]", handle, handle->index));

    if ( NULL == handle->deviceHandle->dspContext )
    {
        BDBG_ERR(("DSP Not avaliable"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( handle->state != BVDE_ChannelState_eStopped )
    {
        BDBG_ERR(("Already running, cannot start"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Sanity check settings */
    errCode = BVDE_Channel_P_ValidateDecodeSettings(handle, pSettings, &handle->startSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    errCode = BVDE_Channel_P_Start(handle);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Success */
    return BERR_SUCCESS;
}

static void BVDE_Channel_P_Stop(
    BVDE_ChannelHandle handle
    )
{
    BDSP_TaskHandle task;

    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);

    if ( NULL == handle->task )
    {
        BDBG_MSG(("BVDE_Channel_P_Stop: Channel %u already stopped.", handle->index));
        return;
    }

#if BVDE_DISABLE_DSP
    #warning Task Start is Disabled!
    BDBG_ERR(("NOT STOPPING DSP"));
#else
    BDSP_Task_Stop(handle->task);
#endif

    task = handle->task;
    BKNI_EnterCriticalSection();    /* Required to sync with display interrupts so they have finished prior to calling BDSP_Task_Destroy */
    handle->task = NULL;
    BKNI_LeaveCriticalSection();

    BDSP_Task_Destroy(task);
    BVDE_Channel_P_CleanupTaskCreateSettings(handle);

}

void BVDE_Channel_Stop(
    BVDE_ChannelHandle handle
    )
{
    bool unmute = false;

    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);

    BDBG_MSG(("BVDE_Channel_Stop(%#x) [index %u]", handle, handle->index));

    switch ( handle->state )
    {
    case BVDE_ChannelState_eStopped:
        BDBG_WRN(("Channel %u Already Stopped.", handle->index));
        return;
    case BVDE_ChannelState_ePaused:
    case BVDE_ChannelState_eDisabledPaused:
        unmute = true;
        break;
    default:
        break;
    }

    /* Stop the task first */
    handle->state = BVDE_ChannelState_eStopped;
    /* Serialize with critical section prior to stopping the task, guarantees isrs are not updating while we stop (they check the state first) */
    BKNI_EnterCriticalSection();
    BKNI_LeaveCriticalSection();

    BVDE_Channel_P_Stop(handle);
}


BERR_Code BVDE_Channel_DisableForFlush(
    BVDE_ChannelHandle handle
    )
{
    BVDE_ChannelState newState = BVDE_ChannelState_eMax;

    BDBG_MSG(("BVDE_Channel_DisableForFlush(%#x) [index %u]", handle, handle->index));

    /* Make sure we're performing a valid state transition */
    switch ( handle->state )
    {
    case BVDE_ChannelState_eStopped:
        BDBG_ERR(("Channel %u is not started, cannot disable for flush.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BVDE_ChannelState_eStarted:
        newState = BVDE_ChannelState_eDisabled;
        break;
    case BVDE_ChannelState_ePaused:
        newState = BVDE_ChannelState_eDisabledPaused;
        break;
    case BVDE_ChannelState_eDisabled:
    case BVDE_ChannelState_eDisabledPaused:
        /* No change */
        return BERR_SUCCESS;
    default:
        BDBG_ERR(("Unexpected Channel state %u", handle->state));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Transition State */
    handle->state = newState;
    BKNI_EnterCriticalSection();
    BKNI_LeaveCriticalSection();
    BVDE_Channel_P_Stop(handle);

    return BERR_SUCCESS;
}

BERR_Code BVDE_Channel_Flush(
    BVDE_ChannelHandle handle
    )
{
    BERR_Code errCode;
    bool paused = false;

    BDBG_MSG(("BVDE_Channel_Flush(%#x) [index %u]", handle, handle->index));

    /* Make sure we're performing a valid state transition */
    switch ( handle->state )
    {
    case BVDE_ChannelState_eStopped:
        BDBG_ERR(("Channel %u is not started, cannot flush.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BVDE_ChannelState_eStarted:
    case BVDE_ChannelState_ePaused:
        BDBG_ERR(("Channel %u is not disabled, cannot flush.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BVDE_ChannelState_eDisabled:
        break;
    case BVDE_ChannelState_eDisabledPaused:
        paused = true;
        break;
    default:
        BDBG_ERR(("Unexpected Channel state %u", handle->state));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    errCode = BVDE_Channel_P_Start(handle);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}


void BVDE_Channel_GetSettings(
    BVDE_ChannelHandle handle,
    BVDE_ChannelOpenSettings *pSettings     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = handle->settings;
}

BERR_Code BVDE_Channel_SetSettings(
    BVDE_ChannelHandle handle,
    const BVDE_ChannelOpenSettings *pSettings
    )
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BDBG_ASSERT(NULL != pSettings);


    handle->settings = *pSettings;

    if ( handle->task )
    {
        errCode = BVDE_Channel_P_ApplyCodecSettings(handle);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

void BVDE_Channel_GetDefaultCdbItbConfig(
    BVDE_ChannelHandle handle,
    BAVC_CdbItbConfig *pConfig  /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BSTD_UNUSED(handle);
    BKNI_Memset(pConfig, 0, sizeof(BAVC_CdbItbConfig));
    pConfig->Cdb.Length = 256*1024;
    pConfig->Cdb.Alignment = 6; /* cache line on 4380 */
    pConfig->Itb.Length = 128*1024;
    pConfig->Itb.Alignment = 6; /* cache line on 4380 */
#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE
    pConfig->Cdb.LittleEndian = true;
#else
    pConfig->Cdb.LittleEndian = false;
#endif
}

static void BVDE_Channel_P_SetupDefaults(BVDE_ChannelHandle handle)
{
    /* BVDE_Channel_P_GetDefaultCodecSettings(handle); */
    BSTD_UNUSED(handle);
}

static void BVDE_Channel_P_CleanupTaskCreateSettings(BVDE_ChannelHandle handle)
{
    unsigned i;

    /* The first branch is always static.  Others need to be BKNI_Free()'d. */
    /* This is not an ideal programming model, but workable */
    for ( i = 1; i < handle->taskCreateSettings.numBranches; i++ )
    {
        BDBG_ASSERT(NULL != handle->taskCreateSettings.pBranchInfo[i]);
        BKNI_Free(handle->taskCreateSettings.pBranchInfo[i]);
    }

    if (handle->taskCreateSettings.psVDecodeBuffCfg)
        BKNI_Free(handle->taskCreateSettings.psVDecodeBuffCfg);
    
    /* Reset structure to defaults */
    BDSP_Task_GetDefaultCreateSettings(handle->deviceHandle->dspContext, &handle->taskCreateSettings);
}

BERR_Code BVDE_Channel_P_ApplyCodecSettings(BVDE_ChannelHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);

    if ( handle->task )
    {
        switch ( handle->startSettings.codec )
        {
        case BAVC_VideoCompressionStd_eVP6:
            /* return BVDE_Decoder_P_ApplyVP6Settings(handle); */
        default:
            break;
        }
    }

    return BERR_SUCCESS;
}

int ceiling (unsigned num, unsigned den)
{
    return (num%den?num/den + 1:num/den);
}


BERR_Code BVDE_Channel_P_AllocateFrameBuffer(BVDE_ChannelHandle handle)
{
    unsigned size, numframes, numrefframes;
    unsigned height, width, luma, chroma, lumaRef, chromaRef, stripWidth;
    unsigned horzpad = BVDE_TOTAL_HORZIZONTAL_PADDING, vertluma = BVDE_TOTAL_VERTICAL_LUMA_PADDING, vertchroma = BVDE_TOTAL_VERTICAL_CHROMA_PADDING;
    BMEM_Handle memHandle = NULL;

    if (handle->settings.memPicHandle != NULL)
    {
        BDBG_WRN(("Frame buffer from %#x (default %#x)", memHandle, handle->deviceHandle->memHandle));
        memHandle = handle->settings.memPicHandle;
    }
    else
    {
        BDBG_WRN(("Frame buffer from default"));
        memHandle = handle->deviceHandle->memHandle;
    }

    /* Get the height and width for this resolution */
    width = BVDE_Channel_P_GetResolutionWidth (handle->settings.resolution);
    height = BVDE_Channel_P_GetResolutionHeight (handle->settings.resolution);
    stripWidth = handle->deviceHandle->settings.stripeWidth;

    luma = (ceiling(width,stripWidth)*stripWidth) * (ceiling(height,16)*16);
    chroma = (ceiling(width,stripWidth)*stripWidth) * (ceiling(height,32)*16);

    lumaRef = (ceiling(((width+horzpad)*2),stripWidth)*stripWidth) * (ceiling ((height+vertluma),16)*16);
    chromaRef = (ceiling(((width+horzpad)*2),stripWidth)*stripWidth) * (ceiling ((height+vertchroma),16)*16);

    numframes = BDSP_FWMAX_VIDEO_BUFF_AVAIL;
    numrefframes = BDSP_FWMAX_VIDEO_REF_BUFF_AVAIL;

    /* Store the information in handle */
    handle->videoMemory.lumasize = luma;
    handle->videoMemory.chromasize = chroma;
    handle->videoMemory.lumarefsize = lumaRef;
    handle->videoMemory.chromarefsize = chromaRef;
    handle->videoMemory.totalFrames = numframes;
    handle->videoMemory.totalRefFrames = numrefframes;

    /* Video Memory allocation*/
    size = (luma + chroma) * numframes + (lumaRef + chromaRef) * numrefframes;

    /* Video FW requires the buffers to be 256 byte alligned for DMA */
    handle->videoMemory.pFrameBufferMemory = BMEM_AllocAligned(memHandle,size, 8, 0);
    if ( NULL == handle->videoMemory.pFrameBufferMemory )
    {
        BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory !"));
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    /* UPB Allocation */
    size = sizeof(BXDM_Picture) * numframes;
    handle->videoMemory.pUpbMemory = BMEM_AllocAligned(handle->deviceHandle->memHandle,size, 8, 0);
    if(NULL == handle->videoMemory.pUpbMemory)
    {
        BDBG_ERR(("BRAP_DSP_AllocMem: Unable to Allocate memory !"));
        BMEM_Free (memHandle, handle->videoMemory.pFrameBufferMemory);
        handle->videoMemory.pFrameBufferMemory = NULL;
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    return BERR_SUCCESS;
}

void BVDE_Channel_P_DeAllocateFrameBuffer(BVDE_ChannelHandle handle)
{
    BMEM_Handle memHandle = NULL;

    if (handle->settings.memPicHandle != NULL)
        memHandle = handle->settings.memPicHandle;
    else
        memHandle = handle->deviceHandle->memHandle;

    if (handle->videoMemory.pFrameBufferMemory)
        BMEM_Free (memHandle, handle->videoMemory.pFrameBufferMemory);

    if (handle->videoMemory.pUpbMemory)
        BMEM_Free (handle->deviceHandle->memHandle, handle->videoMemory.pUpbMemory);    
    

}
unsigned BVDE_Channel_P_GetResolutionWidth (BVDE_Resolution resolution)
{
    switch (resolution)
    {
        case BVDE_Resolution_eFullHD: return 1920;        
        case BVDE_Resolution_eHD: return 1280;    
        case BVDE_Resolution_ePAL: return 720;
        case BVDE_Resolution_eSD: return 720;
        case BVDE_Resolution_eCIF: return 352;
        case BVDE_Resolution_eQCIF: return 176;
        case BVDE_Resolution_eMaxModes: return 720;
    }

    return 0;
}

unsigned BVDE_Channel_P_GetResolutionHeight (BVDE_Resolution resolution)
{
    switch (resolution)
    {
        case BVDE_Resolution_eFullHD: return 1080;        
        case BVDE_Resolution_eHD: return 720;    
        case BVDE_Resolution_ePAL: return 576;
        case BVDE_Resolution_eSD: return 480;
        case BVDE_Resolution_eCIF: return 288;
        case BVDE_Resolution_eQCIF: return 144;
        case BVDE_Resolution_eMaxModes: return 576;
    }

    return 0;    
}


BERR_Code BVDE_Channel_GetPictureCount_isr(
    BVDE_ChannelHandle handle,
    unsigned *pPictureCount
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BDBG_ASSERT(NULL != pPictureCount);
    
    if ( handle->task )
    {    
    return BDSP_Video_GetPictureCount_isr (handle->task, pPictureCount);
}
    else
    {
        *pPictureCount = 0;
        return BERR_SUCCESS;
    }
}

BERR_Code BVDE_Channel_PeekAtPicture_isr(
    BVDE_ChannelHandle handle,
    unsigned index,
    BXDM_Picture **pUnifiedPicture    
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BSTD_UNUSED(index);
    BDBG_ASSERT(NULL != pUnifiedPicture);
    
    if ( handle->task )
    {    
    return BDSP_Video_PeekAtPicture_isr (handle->task, index, pUnifiedPicture);
}
    else
    {
        *pUnifiedPicture = NULL;    /* ?? */
        return BERR_SUCCESS;
    }
}

BERR_Code BVDE_Channel_GetNextPicture_isr(
    BVDE_ChannelHandle handle,
    BXDM_Picture **pUnifiedPicture    
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BDBG_ASSERT(NULL != pUnifiedPicture);
    
    if ( handle->task )
    {    
    return BDSP_Video_GetNextPicture_isr (handle->task, pUnifiedPicture);
}
    else
    {
        *pUnifiedPicture = NULL;    /* ?? */
        return BERR_SUCCESS;
    }
}

BERR_Code BVDE_Channel_ReleasePicture_isr(
    BVDE_ChannelHandle handle,
    BXDM_Picture *pUnifiedPicture    
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BDBG_ASSERT(NULL != pUnifiedPicture);
    
    if ( handle->task )
    {    
    return BDSP_Video_ReleasePicture_isr (handle->task, pUnifiedPicture);
}
    else
    {
        return BERR_SUCCESS;
    }
}

BERR_Code BVDE_Channel_GetPictureDropPendingCount_isr(
    BVDE_ChannelHandle handle,
    unsigned *pPictureDropPendingCount
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BDBG_ASSERT(NULL != pPictureDropPendingCount);
    
    if ( handle->task )
    {    
    return BDSP_Video_GetPictureDropPendingCount_isr (handle->task, pPictureDropPendingCount);
}
    else
    {
        *pPictureDropPendingCount = 0;
        return BERR_SUCCESS;
    }
}

BERR_Code BVDE_Channel_RequestPictureDrop_isr(
    BVDE_ChannelHandle handle,
    unsigned *pPictureDropRequestCount
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BDBG_ASSERT(NULL != pPictureDropRequestCount);
    
    if ( handle->task )
    {    
    return BDSP_Video_RequestPictureDrop_isr (handle->task, pPictureDropRequestCount);
}
    else
    {
        *pPictureDropRequestCount = 0;
        return BERR_SUCCESS;
    }
}

BERR_Code BVDE_Channel_DisplayInterruptEvent_isr(
    BVDE_ChannelHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    
    if ( handle->task )    
    {
    return BDSP_Video_DisplayInterruptEvent_isr (handle->task);
    }
    else
    {
        return BERR_SUCCESS;
    }
}

BERR_Code BVDE_Channel_GetDMInterface(
    BVDE_ChannelHandle handle,
    BXDM_Decoder_Interface *pDMInterface,
    void **pContext
    )
{
    BDBG_OBJECT_ASSERT(handle, BVDE_Channel);
    BDBG_ASSERT(NULL != pDMInterface);
    BDBG_ASSERT(NULL != pContext);

    pDMInterface->getPictureCount_isr = (BXDM_Decoder_GetPictureCount_isr) BVDE_Channel_GetPictureCount_isr;
    pDMInterface->peekAtPicture_isr = (BXDM_Decoder_PeekAtPicture_isr) BVDE_Channel_PeekAtPicture_isr;
    pDMInterface->getNextPicture_isr = (BXDM_Decoder_GetNextPicture_isr) BVDE_Channel_GetNextPicture_isr;
    pDMInterface->releasePicture_isr = (BXDM_Decoder_ReleasePicture_isr) BVDE_Channel_ReleasePicture_isr;
    pDMInterface->getPictureDropPendingCount_isr = (BXDM_Decoder_GetPictureDropPendingCount_isr) BVDE_Channel_GetPictureDropPendingCount_isr;
    pDMInterface->requestPictureDrop_isr = (BXDM_Decoder_RequestPictureDrop_isr) BVDE_Channel_RequestPictureDrop_isr;
    pDMInterface->displayInterruptEvent_isr = (BXDM_Decoder_DisplayInterruptEvent_isr) BVDE_Channel_DisplayInterruptEvent_isr;

   *pContext = handle;

   return BERR_SUCCESS;
}
