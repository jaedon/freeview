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
* $brcm_Workfile: nexus_audio_mux_output.c $
* $brcm_Revision: 12 $
* $brcm_Date: 8/31/12 3:33p $
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_audio_mux_output.c $
* 
* 12   8/31/12 3:33p jgarrett
* SW7425-3692: Merge to main branch
* 
* SW7425-3692/2   8/31/12 2:48p jgarrett
* SW7425-3692: Incorporating code review feedback
* 
* SW7425-3692/1   8/16/12 5:58p mpeteri
* SW7425-3692: add field for configuring maxItbDescriptors
* 
* 11   6/20/12 1:29p vsilyaev
* SW7420-2085: Really fixing build
* 
* 10   6/20/12 1:11p jgarrett
* SW7420-2085: Fixing build when NEXUS_HAS_AUDIO_MUX_OUTPUT is not
*  defined
* 
* 9   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 8   6/15/12 2:31p jgarrett
* SW7425-3258: Correcting comments regarding size parameters to GetBuffer
* 
* 7   4/20/12 5:23p jgarrett
* SW7231-774: Resetting MuxOutput on a watchdog interrupt
* 
* 6   10/14/11 2:54p jgarrett
* SW7425-1389: Coverity CID 33805
* 
* 5   6/21/11 5:04p jgarrett
* SW7425-654: Adding Audio NRT APIs
* 
* 4   4/19/11 5:01p jgarrett
* SW7425-288: Merge to main branch
* 
* SW7425-288/1   4/19/11 2:36a nilesh
* SW7425-288: Propagate audio metadata buffer address
* 
* 3   3/21/11 7:05p jgarrett
* SW7422-356: Adding MuxOutput support in APE/Nexus
* 
* 2   2/22/11 5:31p jgarrett
* SW7422-146: Fixing build errors
* 
* 1   1/4/11 2:39p jgarrett
* SW7422-146: Adding 7425 APE support
* 
* 9   1/4/11 11:19a jgarrett
* SW7425-41: Adding debug and removing cache conversion
* 
* 8   12/30/10 4:06p jgarrett
* SW7425-41: Fixing stub prototype
* 
* 7   12/30/10 12:16p jgarrett
* SW7425-41: Linked audio mux output and stream mux
* 
* 6   12/30/10 11:12a jgarrett
* SW7425-41: Fixing compilation error in stub apis
* 
* 5   12/30/10 10:37a jgarrett
* SW7425-41: Making mux output only functional on 7425
* 
* 4   12/29/10 9:48p jgarrett
* SW7425-41: Removing stubs on 7425
* 
* 3   12/29/10 6:33p jgarrett
* SW7425-41: Stub RAP mux files
* 
* 2   12/29/10 6:29p jgarrett
* SW7425-41: Adding audio encoder and mux outputs
* 
* 1   12/17/10 4:45p jgarrett
* SW7425-41: Adding initial encoder/mux APIs
*
***************************************************************************/

#include "nexus_audio_module.h"

BDBG_MODULE(nexus_audio_mux_output);


#if NEXUS_HAS_AUDIO_MUX_OUTPUT

typedef struct NEXUS_AudioMuxOutput
{
    NEXUS_OBJECT(NEXUS_AudioMuxOutput);
    BLST_S_ENTRY(NEXUS_AudioMuxOutput) node;
    NEXUS_AudioOutputObject connector;
    NEXUS_RaveHandle raveContext;
    BAVC_XptContextMap contextMap;
    BAPE_MuxOutputHandle muxOutput;
    BAVC_Xpt_StcSoftIncRegisters softIncRegisters;
    NEXUS_AudioMuxOutputStartSettings startSettings;
    NEXUS_AudioInput input;
    bool started;
} NEXUS_AudioMuxOutput;


static BLST_S_HEAD(MuxOutputList, NEXUS_AudioMuxOutput) g_muxOutputList;

/* Static array defining CDB and ITB sizes for various Buffer Configuration Modes */
static const BAVC_CdbItbConfig g_cdbItbCfg =
{
    { 256 * 1024, 4, true }, { 128 * 1024, 4, false }, false
};

#define LOCK_TRANSPORT()    NEXUS_Module_Lock(g_NEXUS_audioModuleData.settings.modules.transport)
#define UNLOCK_TRANSPORT()  NEXUS_Module_Unlock(g_NEXUS_audioModuleData.settings.modules.transport)

/***************************************************************************
Summary:
    Get default settings for an Audio Mux output
***************************************************************************/
void NEXUS_AudioMuxOutput_GetDefaultCreateSettings(
    NEXUS_AudioMuxOutputCreateSettings *pSettings       /* [out] default settings */
    )
{
    BAPE_MuxOutputCreateSettings apeDefaults;

    if ( NULL == pSettings )
    {
        return;
    }

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BAPE_MuxOutput_GetDefaultCreateSettings(&apeDefaults);
    pSettings->numDescriptors = apeDefaults.numDescriptors;
}

/***************************************************************************
Summary:
    Open an Audio Mux Ouptut Handle
***************************************************************************/
NEXUS_AudioMuxOutputHandle NEXUS_AudioMuxOutput_Create(     /* attr{destructor=NEXUS_AudioMuxOutput_Destroy}  */
    const NEXUS_AudioMuxOutputCreateSettings *pSettings     /* Pass NULL for default settings */
    )
{
    NEXUS_AudioMuxOutputCreateSettings defaults;
    NEXUS_RaveOpenSettings raveSettings;
    NEXUS_RaveStatus raveStatus;
    NEXUS_AudioMuxOutputHandle handle;
    BAPE_MuxOutputCreateSettings createSettings;
    BERR_Code errCode;

    if ( NULL == pSettings )
    {
        NEXUS_AudioMuxOutput_GetDefaultCreateSettings(&defaults);
        pSettings = &defaults;
    }
    
    /* Create Handle */
    handle = BKNI_Malloc(sizeof(NEXUS_AudioMuxOutput));
    if ( NULL == handle )
    {
        (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_malloc;
    }
    NEXUS_OBJECT_INIT(NEXUS_AudioMuxOutput, handle);
    NEXUS_AUDIO_OUTPUT_INIT(&handle->connector, NEXUS_AudioOutputType_eMux, handle);

    /* Setup rave buffer */
    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultOpenSettings_priv(&raveSettings);
    raveSettings.config = g_cdbItbCfg;
    if ( 0 != pSettings->data.fifoSize )
    {
        raveSettings.config.Cdb.Length = pSettings->data.fifoSize;
    }
    if ( 0 != pSettings->index.fifoSize )
    {
        raveSettings.config.Itb.Length = pSettings->index.fifoSize;
    }
    handle->raveContext = NEXUS_Rave_Open_priv(&raveSettings);    
    if ( NULL == handle->raveContext )
    {
        (void)BERR_TRACE(BERR_UNKNOWN);
        UNLOCK_TRANSPORT();
        goto err_rave_context;
    }
    errCode = NEXUS_Rave_GetStatus_priv(handle->raveContext, &raveStatus);
    UNLOCK_TRANSPORT();
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_rave_status;
    }
    BKNI_Memcpy(&handle->contextMap, &raveStatus.xptContextMap, sizeof(raveStatus.xptContextMap));

    BAPE_MuxOutput_GetDefaultCreateSettings(&createSettings);
    createSettings.pContextMap = &handle->contextMap;
    createSettings.numDescriptors = pSettings->numDescriptors;
    errCode = BAPE_MuxOutput_Create(NEXUS_AUDIO_DEVICE_HANDLE, &createSettings, &handle->muxOutput);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_muxoutput_create;
    }
    
    /* Success */
    BLST_S_INSERT_HEAD(&g_muxOutputList, handle, node);
    return handle;

err_muxoutput_create:
err_rave_status:
    LOCK_TRANSPORT();
    NEXUS_Rave_Close_priv(handle->raveContext);
    UNLOCK_TRANSPORT();
err_rave_context:
    NEXUS_OBJECT_DESTROY(NEXUS_AudioMuxOutput, handle);
    BKNI_Free(handle);
err_malloc:
    return NULL;
}


/***************************************************************************
Summary:
    Close an Audio Mux Ouptut Handle
***************************************************************************/
static void NEXUS_AudioMuxOutput_P_Finalizer(
    NEXUS_AudioMuxOutputHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_AudioMuxOutput, handle);
    BLST_S_REMOVE(&g_muxOutputList, handle, NEXUS_AudioMuxOutput, node);
    NEXUS_AudioOutput_Shutdown(&handle->connector);
    BAPE_MuxOutput_Destroy(handle->muxOutput);
    LOCK_TRANSPORT();
    NEXUS_Rave_Close_priv(handle->raveContext);
    UNLOCK_TRANSPORT();
    NEXUS_OBJECT_DESTROY(NEXUS_AudioMuxOutput, handle);
    BKNI_Free(handle);
}

/***************************************************************************
Summary:
Get the NEXUS_AudioOutput connector to make upstream connection
    
Description:
NEXUS_AudioMuxOutput can be connected to NEXUS_AudioEncoder (for transcode mode) as follows:

    NEXUS_AudioOutput_AddInput(NEXUS_AudioMuxOutput_GetConnector(muxOutput), NEXUS_AudioEncoder_GetConnector(audioEncoder));
        
Or it can be connected to NEXUS_AudioDecoder (for passthrough mode) as follows:

    NEXUS_AudioOutput_AddInput(NEXUS_AudioMuxOutput_GetConnector(muxOutput), NEXUS_AudioDecoder_GetConnector(audioDecoder));
        
***************************************************************************/
NEXUS_AudioOutput NEXUS_AudioMuxOutput_GetConnector( /* attr{shutdown=NEXUS_AudioOutput_Shutdown} */
    NEXUS_AudioMuxOutputHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMuxOutput);
    return &handle->connector;
}


/***************************************************************************
Summary:
Get default start-time settings for a mux output
***************************************************************************/
void NEXUS_AudioMuxOutput_GetDefaultStartSettings(
    NEXUS_AudioMuxOutputStartSettings *pSettings  /* [out] default settings */
    )
{
    if ( NULL == pSettings )
    {
        return;
    }

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

/***************************************************************************
Summary:
Start capturing data.  

Decription:
An input must be connected prior to starting.

See Also:
NEXUS_AudioMuxOutput_Stop
NEXUS_AudioMuxOutput_GetConnector
***************************************************************************/
NEXUS_Error NEXUS_AudioMuxOutput_Start(
    NEXUS_AudioMuxOutputHandle handle,
    const NEXUS_AudioMuxOutputStartSettings *pSettings
    )
{
    BAPE_MuxOutputStartSettings startSettings;
    NEXUS_AudioMuxOutputStartSettings defaults;
    NEXUS_StcChannelNonRealtimeSettings stcNrtSettings;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMuxOutput);
    if ( NULL == pSettings )
    {
        NEXUS_AudioMuxOutput_GetDefaultStartSettings(&defaults);
        pSettings = &defaults;
    }

    handle->startSettings = *pSettings;

    BAPE_MuxOutput_GetDefaultStartSettings(&startSettings);
    if ( NULL != pSettings->stcChannel )
    {
        unsigned stcChannelIndex;
        LOCK_TRANSPORT();
        NEXUS_StcChannel_GetIndex_priv(pSettings->stcChannel, &stcChannelIndex);
        if ( pSettings->nonRealTime )
        {

            NEXUS_StcChannel_GetSoftIncrementRegOffset_priv(pSettings->stcChannel, &handle->softIncRegisters);
            startSettings.pNonRealTimeIncrement = &handle->softIncRegisters;

            /* set STC to freerunning mode on stop */
            NEXUS_StcChannel_GetDefaultNonRealtimeSettings_priv(&stcNrtSettings);
            stcNrtSettings.triggerMode = NEXUS_StcChannelTriggerMode_eSoftIncrement;
            errCode = NEXUS_StcChannel_SetNonRealtimeConfig_priv(pSettings->stcChannel, &stcNrtSettings);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                UNLOCK_TRANSPORT();
                goto err_nrt_config;
            }
        }
        UNLOCK_TRANSPORT();
        startSettings.stcIndex = stcChannelIndex;
    }
    startSettings.presentationDelay = pSettings->presentationDelay;
    LOCK_TRANSPORT();
    /* Reset RAVE pointers prior to start */
    NEXUS_Rave_Flush_priv(handle->raveContext);
    UNLOCK_TRANSPORT();
    errCode = BAPE_MuxOutput_Start(handle->muxOutput, &startSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_start;
    }

    handle->started = true;

    return BERR_SUCCESS;

err_start:
    if ( NULL != pSettings->stcChannel )
    {
        LOCK_TRANSPORT();
        NEXUS_StcChannel_GetDefaultNonRealtimeSettings_priv(&stcNrtSettings);
        stcNrtSettings.triggerMode = NEXUS_StcChannelTriggerMode_eTimebase;
        (void)NEXUS_StcChannel_SetNonRealtimeConfig_priv(pSettings->stcChannel, &stcNrtSettings);
        UNLOCK_TRANSPORT();
    }
err_nrt_config:
    return errCode;
}

/***************************************************************************
Summary:
Stop capturing data.
***************************************************************************/
void NEXUS_AudioMuxOutput_Stop(
    NEXUS_AudioMuxOutputHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMuxOutput);

    if ( !handle->started )
    {
        return;
    }

    BAPE_MuxOutput_Stop(handle->muxOutput);
    handle->started = false;

    if ( NULL != handle->startSettings.stcChannel )
    {
        NEXUS_StcChannelNonRealtimeSettings stcNrtSettings;
        LOCK_TRANSPORT();
        NEXUS_StcChannel_GetDefaultNonRealtimeSettings_priv(&stcNrtSettings);
        stcNrtSettings.triggerMode = NEXUS_StcChannelTriggerMode_eTimebase;
        (void)NEXUS_StcChannel_SetNonRealtimeConfig_priv(handle->startSettings.stcChannel, &stcNrtSettings);
        UNLOCK_TRANSPORT();
    }
}

/**
Summary:
**/
NEXUS_Error NEXUS_AudioMuxOutput_GetStatus(
    NEXUS_AudioMuxOutputHandle handle,
    NEXUS_AudioMuxOutputStatus *pStatus /* [out] */
    )
{
    BERR_Code errCode;
    BAVC_AudioBufferStatus bufferStatus;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMuxOutput);

    if ( NULL == pStatus )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    errCode = BAPE_MuxOutput_GetBufferStatus(handle->muxOutput, &bufferStatus);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    /* TODO: This is not cached memory.  Video Encoder has the same issue, we need to
       determine whether to have the VCE/RAP PI's use cached memory internally or
       whether to have nexus walk the descriptors to understand how to flush the cache
       properly. */
    pStatus->pBufferBase = (void*)bufferStatus.stCommon.pFrameBufferBaseAddress;
    pStatus->pMetadataBufferBase = (void*)bufferStatus.stCommon.pMetadataBufferBaseAddress;

    return BERR_SUCCESS;
}

/**
Summary:
The mux manager (or other consumer) will call this API to get encoded frames from the NEXUS_AudioMuxOutput
**/
NEXUS_Error NEXUS_AudioMuxOutput_GetBuffer(
    NEXUS_AudioMuxOutputHandle handle,
    const NEXUS_AudioMuxOutputFrame **pBuffer, /* [out] pointer to NEXUS_AudioMuxOutputFrame structs */
    size_t *pSize,  /* [out] number of NEXUS_AudioMuxOutputFrame elements in pBuffer */
    const NEXUS_AudioMuxOutputFrame **pBuffer2, /* [out] pointer to NEXUS_AudioMuxOutputFrame structs after wrap around */
    size_t *pSize2  /* [out] number of NEXUS_AudioMuxOutputFrame elements in pBuffer2 */
    )
{
    BERR_Code errCode;
    const BAVC_AudioBufferDescriptor *pDesc1, *pDesc2;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMuxOutput);
    BDBG_CASSERT(sizeof(NEXUS_AudioMuxOutputFrame) == sizeof(BAVC_AudioBufferDescriptor));

    if ( NULL == pBuffer || NULL == pSize || NULL == pBuffer2 || NULL == pSize2 )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    errCode = BAPE_MuxOutput_GetBufferDescriptors(handle->muxOutput, &pDesc1, pSize, &pDesc2, pSize2);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    *pBuffer = (const NEXUS_AudioMuxOutputFrame *)pDesc1;
    *pBuffer2 = (const NEXUS_AudioMuxOutputFrame *)pDesc2;

    BDBG_MSG(("GetBuffer: Returning %u descriptors (%u+%u)", *pSize+*pSize2, *pSize, *pSize2));

    return BERR_SUCCESS;
}

/**
Summary:
Report how much data returned by last NEXUS_AudioMuxOutput_GetBuffer call was consumed
**/
NEXUS_Error NEXUS_AudioMuxOutput_ReadComplete(
    NEXUS_AudioMuxOutputHandle handle,
    unsigned framesCompleted /* must be <= pSize+pSize2 returned by last NEXUS_AudioMuxOutput_GetBuffer call. */
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMuxOutput);
    
    BDBG_MSG(("ReadComplete: Completed %u frames", framesCompleted));

    errCode = BAPE_MuxOutput_ConsumeBufferDescriptors(handle->muxOutput, framesCompleted);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_AudioMuxOutput_GetDelayStatus(
    NEXUS_AudioMuxOutputHandle handle,
    NEXUS_AudioCodec codec,
    NEXUS_AudioMuxOutputDelayStatus *pStatus    /* [out] */
    )
{
    BERR_Code errCode;
    BAVC_AudioCompressionStd avcCodec;
    BAPE_MuxOutputDelayStatus delayStatus;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMuxOutput);
    BDBG_ASSERT(NULL != pStatus);

    avcCodec = NEXUS_Audio_P_CodecToMagnum(codec);
    errCode = BAPE_MuxOutput_GetDelayStatus(handle->muxOutput, avcCodec, &delayStatus);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pStatus->endToEndDelay = delayStatus.endToEndDelay;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary: 
    Link audio mux output to a particular node
 ***************************************************************************/
NEXUS_Error NEXUS_AudioMuxOutput_P_AddInput(
    NEXUS_AudioMuxOutputHandle handle, 
    NEXUS_AudioInput input
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMuxOutput);
    BDBG_ASSERT(NULL != input);
    BDBG_ASSERT(NULL == handle->input);

    errCode = BAPE_MuxOutput_AddInput(handle->muxOutput, (BAPE_Connector)input->port);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->input = input;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary: 
    Unlink audio mux output from a particular node
 ***************************************************************************/
void NEXUS_AudioMuxOutput_P_RemoveInput(
    NEXUS_AudioMuxOutputHandle handle, 
    NEXUS_AudioInput input
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioMuxOutput);
    BDBG_ASSERT(handle->input == input);

    (void)BAPE_MuxOutput_RemoveAllInputs(handle->muxOutput);
    handle->input = NULL;
}

void NEXUS_AudioMuxOutput_P_WatchdogReset(void)
{
    NEXUS_AudioMuxOutputHandle hMuxOutput;

    for ( hMuxOutput = BLST_S_FIRST(&g_muxOutputList);
          hMuxOutput != NULL;
          hMuxOutput = BLST_S_NEXT(hMuxOutput, node) )
    {
        if ( hMuxOutput->started )
        {
            NEXUS_AudioMuxOutput_Stop(hMuxOutput);
            (void)NEXUS_AudioMuxOutput_Start(hMuxOutput, &hMuxOutput->startSettings);
        }
    }
}

#else /* #if NEXUS_HAS_AUDIO_MUX_OUTPUT */

typedef struct NEXUS_AudioMuxOutput
{
    NEXUS_OBJECT(NEXUS_AudioMuxOutput);
} NEXUS_AudioMuxOutput;


/* Stubs */

/***************************************************************************
Summary:
    Get default settings for an Audio Mux output
***************************************************************************/
void NEXUS_AudioMuxOutput_GetDefaultCreateSettings(
    NEXUS_AudioMuxOutputCreateSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
    Open an Audio Mux Ouptut Handle
***************************************************************************/
NEXUS_AudioMuxOutputHandle NEXUS_AudioMuxOutput_Create(   /* attr{destructor=NEXUS_AudioMuxOutput_Destroy}  */
    const NEXUS_AudioMuxOutputCreateSettings *pSettings /* Pass NULL for default settings */
    )
{
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}


/***************************************************************************
Summary:
    Close an Audio Mux Ouptut Handle
***************************************************************************/
static void NEXUS_AudioMuxOutput_P_Finalizer(
    NEXUS_AudioMuxOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/***************************************************************************
Summary:
Get the NEXUS_AudioOutput connector to make upstream connection
    
Description:
NEXUS_AudioMuxOutput can be connected to NEXUS_AudioEncoder (for transcode mode) as follows:

    NEXUS_AudioOutput_AddInput(NEXUS_AudioMuxOutput_GetConnector(muxOutput), NEXUS_AudioEncoder_GetConnector(audioEncoder));
        
Or it can be connected to NEXUS_AudioDecoder (for passthrough mode) as follows:

    NEXUS_AudioOutput_AddInput(NEXUS_AudioMuxOutput_GetConnector(muxOutput), NEXUS_AudioDecoder_GetConnector(audioDecoder));
        
***************************************************************************/
NEXUS_AudioOutput NEXUS_AudioMuxOutput_GetConnector( /* attr{shutdown=NEXUS_AudioOutput_Shutdown} */
    NEXUS_AudioMuxOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}


/***************************************************************************
Summary:
Get default start-time settings for a mux output
***************************************************************************/
void NEXUS_AudioMuxOutput_GetDefaultStartSettings(
    NEXUS_AudioMuxOutputStartSettings *pSettings  /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
Start capturing data.  

Decription:
An input must be connected prior to starting.

See Also:
NEXUS_AudioMuxOutput_Stop
NEXUS_AudioMuxOutput_GetConnector
***************************************************************************/
NEXUS_Error NEXUS_AudioMuxOutput_Start(
    NEXUS_AudioMuxOutputHandle handle,
    const NEXUS_AudioMuxOutputStartSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
Stop capturing data.
***************************************************************************/
void NEXUS_AudioMuxOutput_Stop(
    NEXUS_AudioMuxOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/**
Summary:
**/
NEXUS_Error NEXUS_AudioMuxOutput_GetStatus(
    NEXUS_AudioMuxOutputHandle handle,
    NEXUS_AudioMuxOutputStatus *pStatus /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**
Summary:
The mux manager (or other consumer) will call this API to get encoded frames from the NEXUS_AudioMuxOutput
**/
NEXUS_Error NEXUS_AudioMuxOutput_GetBuffer(
    NEXUS_AudioMuxOutputHandle handle,
    const NEXUS_AudioMuxOutputFrame **pBuffer, /* [out] pointer to NEXUS_AudioMuxOutputFrame structs */
    size_t *pSize, /* [out] size of pBuffer in bytes (not number of NEXUS_AudioMuxOutputFrame structs) */
    const NEXUS_AudioMuxOutputFrame **pBuffer2, /* [out] pointer to NEXUS_AudioMuxOutputFrame structs after wrap around */
    size_t *pSize2 /* [out] size of pBuffer2 in bytes (not number of NEXUS_AudioMuxOutputFrame structs) */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pBuffer);
    BSTD_UNUSED(pSize);
    BSTD_UNUSED(pBuffer2);
    BSTD_UNUSED(pSize2);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**
Summary:
Report how much data returned by last NEXUS_AudioMuxOutput_GetBuffer call was consumed
**/
NEXUS_Error NEXUS_AudioMuxOutput_ReadComplete(
    NEXUS_AudioMuxOutputHandle handle,
    unsigned framesCompleted /* must be <= pSize+pSize2 returned by last NEXUS_AudioMuxOutput_GetBuffer call. */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(framesCompleted);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_AudioMuxOutput_GetDelayStatus(
    NEXUS_AudioMuxOutputHandle handle,
    NEXUS_AudioCodec codec,
    NEXUS_AudioMuxOutputDelayStatus *pStatus    /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(codec);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

void NEXUS_AudioMuxOutput_P_WatchdogReset(void)
{
    return;
}

#endif /* #if NEXUS_HAS_AUDIO_MUX_OUTPUT */


NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioMuxOutput, NEXUS_AudioMuxOutput_Destroy);

