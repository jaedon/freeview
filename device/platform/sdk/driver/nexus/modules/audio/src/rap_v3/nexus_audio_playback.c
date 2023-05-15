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

* $brcm_Workfile: nexus_audio_playback.c $
* $brcm_Revision: 33 $
* $brcm_Date: 6/19/12 5:42p $
*
* API Description:
*   API name: AudioPlayback
*    Specific APIs related to PCM audio playback.  This supports playback
*    of data from memory.  It can be routed either to a mixer or directly
*    to output devices.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7405/src/nexus_audio_playback.c $
* 
* 33   6/19/12 5:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 32   3/1/12 4:03p jgarrett
* SW7425-2522: Populating playedBytes in status
* 
* 31   12/9/11 5:22p bandrews
* SW7550-772: merge to main
* 
* SW7550-772/1   12/8/11 8:58p bandrews
* SW7550-772: implement default timebase usage
* 
* 30   10/31/11 7:46p bandrews
* SW7231-391: merge to main
* 
* SW7420-2078/1   10/25/11 5:22p bandrews
* SW7231-391: update parser band and timebase implementations to use
*  handles everywhere, even for legacy enum usage
* 
* 29   5/31/11 2:31p carson
* SW7420-1885 : fixes for DTV
* 
* 28   5/26/11 2:26p jgarrett
* SW7420-1885: Adding loop around mode and sample rate adjustments
* 
* 27   1/24/11 3:57p erickson
* SW7420-1009: add NEXUS_ANY_ID support
*
* 26   10/28/10 8:23p akam
* SW35230-1832: With PCM mixing to multiple ports available in RAP,
*  enable independent delay for PCM playback channel.
*
* 25   10/27/10 4:34p erickson
* SW7420-1215: add BDBG_OBJECT checking to NEXUS_AudioInput and
*  NEXUS_AudioOutput
*
* 24   10/7/10 11:27a akam
* SW35230-1553: The independent delay for the playback channel should be
*  disabled per RAP base 4 requirement.
*
* 23   8/12/10 6:25p akam
* SW35230-211: Replaced BCHP_CHIP check with BRAP_VER.
*
* 22   8/10/10 3:10p erickson
* SW7420-934: rename NEXUS_AudioPlayback_ReadComplete to
*  NEXUS_AudioPlayback_WriteComplete
*
* 21   7/2/10 11:00a akam
* SW35230-211: Modified bHbrMode and audio description per 35230 RAP PI
*  changes.
*
* 20   6/24/10 5:45p jgarrett
* SW3548-2938: Applying playback volume settings prior to channel
*  starting
*
* 19   6/3/10 4:17p akam
* SW35230-211: Update Nexus audio module code per 35230 RAP PI changes.
*
* 18   5/11/10 4:18p akam
* SW35230-211: Update Nexus audio module code per 35230 RAP PI changes.
*
* 17   3/31/10 5:29p akam
* SW35230-48: Fix build for 35230.
*
* 16   9/14/09 3:50p jgarrett
* SW7405-3023: Removing outputs before closing
*
* 16   9/14/09 3:48p jgarrett
* SW7405-3023: Removing outputs before closing
*
* 15   9/14/09 12:02p jgarrett
* SW3548-2448: Restoring flush functionality for 3548
*
* 14   9/2/09 2:16p jgarrett
* SW3548-2448: Stubbing out flush call for 3548/3556
*
* 13   8/31/09 3:23p jgarrett
* SW7405-2706: Merging playback flush to main branch
*
* PR56656/1   8/19/09 4:53p jgarrett
* PR 56656: Adding playback flush
*
* 12   5/14/09 9:47a jgarrett
* PR 53780: Adding startThreshold
*
* PR53780/1   4/16/09 4:46p jgarrett
* PR 53780: Adding startThreshold
*
* 11   3/2/09 5:37p jgarrett
* PR 51468: Renaming globals to match naming conventions
*
* 10   12/15/08 4:17p jgarrett
* PR 50346: Fixing fifoSize computation in status
*
* 9   11/12/08 3:49p jgarrett
* PR 48920: Adding a global independent delay flag that affects all
*  channels
*
* 8   10/22/08 7:27p jgarrett
* PR 44768: 3548/3556 moved independent volume to global settings
*
* 7   10/14/08 3:50p jgarrett
* PR47800: PR 47800: Return all bytes queued in HW
*
* PR47800/1   10/10/08 6:44p vsilyaev
* PR 47800: Return all bytes queued in HW
*
* 6   7/14/08 3:58p jgarrett
* PR 44645: Disabling independent channel->output port volume control for
*  capture
*
* 5   6/19/08 3:17p jgarrett
* PR 42243: Moving input volume to specific inputs and out of mixer
*
* 4   4/18/08 4:21p jgarrett
* PR 41825: One channel to more than one association is not working.
*  Sharing associations to avoid the issue.
*
* 3   4/15/08 12:36p jgarrett
* PR 41621: Reworking association logic
*
* 2   4/7/08 11:22a jgarrett
* PR 40017: Fixing shutdown conditions
*
* 1   3/11/08 4:39p jgarrett
* PR 39983: Adding audio playback support
*
***************************************************************************/
#include "nexus_audio_module.h"

BDBG_MODULE(nexus_audio_playback);

#if NEXUS_NUM_AUDIO_PLAYBACKS
/***************************************************************************
Summary:
Handle for audio playback
***************************************************************************/
typedef struct NEXUS_AudioPlayback
{
    NEXUS_OBJECT(NEXUS_AudioPlayback);
    NEXUS_AudioInputObject connector;
    BRAP_ChannelParams audioParams;
    BRAP_ChannelHandle rapChannel;
    NEXUS_AudioAssociationHandle association;
    NEXUS_TaskCallbackHandle appCallback;
    NEXUS_AudioPlaybackOpenSettings openSettings;
    NEXUS_AudioPlaybackStartSettings startSettings;
    NEXUS_AudioPlaybackSettings settings;
    BKNI_EventHandle event;
    NEXUS_EventCallbackHandle eventCallback;
    void *pLastAddress;
    size_t bytesPlayed;
    bool started;
    bool opened;
} NEXUS_AudioPlayback;

static NEXUS_AudioPlayback g_playbacks[NEXUS_NUM_AUDIO_PLAYBACKS];
static void NEXUS_AudioPlayback_P_DataEvent(void *pParam);
static void NEXUS_AudioPlayback_P_BufferFree_isr(void *pParam1, int param2, void *pData);
static NEXUS_Error NEXUS_AudioPlayback_P_ConnectionChange(void *pParam, NEXUS_AudioInput input);

/***************************************************************************
Summary:
Get default settings for an audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetDefaultOpenSettings(
    NEXUS_AudioPlaybackOpenSettings *pSettings      /* [out] default settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

/***************************************************************************
Summary:
Open an audio playback channel
***************************************************************************/
NEXUS_AudioPlaybackHandle NEXUS_AudioPlayback_Open(     /* attr{destructor=NEXUS_AudioPlayback_Close}  */
    unsigned index,
    const NEXUS_AudioPlaybackOpenSettings *pSettings    /* Pass NULL for default settings */
    )
{
    BERR_Code errCode;
    NEXUS_AudioPlayback *pChannel;
    NEXUS_AudioPlaybackOpenSettings defaultSettings;
    BRAP_ChannelSettings *pChannelSettings;

    if (index == NEXUS_ANY_ID) {
        for (index=0;index<NEXUS_NUM_AUDIO_PLAYBACKS;index++) {
            if (!g_playbacks[index].opened) break;
        }
        if (index == NEXUS_NUM_AUDIO_PLAYBACKS) {
            BDBG_ERR(("No more playback channels available"));
            return NULL;
        }
    }
    if ( index >= NEXUS_NUM_AUDIO_PLAYBACKS )
    {
        BDBG_ERR(("Playback channel %u not available on this chipset", index));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    pChannel = &g_playbacks[index];
    if ( pChannel->opened )
    {
        BDBG_ERR(("Playback channel %u already open", index));
        errCode = BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_AudioPlayback, pChannel);
    pChannel->settings.leftVolume = pChannel->settings.rightVolume = NEXUS_AUDIO_VOLUME_LINEAR_NORMAL;
    pChannel->settings.sampleRate = 48000;

    if ( NULL == pSettings )
    {
        NEXUS_AudioPlayback_GetDefaultOpenSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* Setup Connector */
    NEXUS_AUDIO_INPUT_INIT(&pChannel->connector, NEXUS_AudioInputType_ePlayback, pChannel);
    pChannel->connector.format = NEXUS_AudioInputFormat_ePcmStereo;
    pChannel->connector.disconnectCb = NEXUS_AudioPlayback_P_ConnectionChange;

    /* Open Raptor Channel */
    pChannelSettings = BKNI_Malloc(sizeof(*pChannelSettings));   /* this structure is HUGE, don't use the stack */
    if ( NULL == pChannelSettings )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    (void)BRAP_GetDefaultChannelSettings(g_NEXUS_audioModuleData.hRap, pChannelSettings);
    pChannelSettings->eChType = BRAP_ChannelType_ePcmPlayback;

#if BRAP_VER < 4
    pChannelSettings->eChSubType = BRAP_ChannelSubType_eNone;
#endif

    pChannelSettings->bEnaIndDly = g_NEXUS_audioModuleData.moduleSettings.independentDelay;

    pChannelSettings->sChnRBufPool.uiMaxNumInChPairs[0]=0;
    pChannelSettings->sChnRBufPool.uiMaxNumOutChPairs[0]=0;
    pChannelSettings->sChnRBufPool.uiMaxNumRBuf=1;
    pChannelSettings->sChnRBufPool.sBufSettings.pLeftBufferStart=NULL;
    pChannelSettings->sChnRBufPool.sBufSettings.pRightBufferStart=NULL;
    /* Round up size to the nearest 256-byte boundary */
    pChannelSettings->sChnRBufPool.sBufSettings.uiSize=(pSettings->fifoSize + 255)&(~255);
    if ( pSettings->threshold && pSettings->fifoSize )
    {
        unsigned threshold;
        /* User specified a threshold.  Convert to % of fifo size */
        threshold = (100*pSettings->threshold)/pSettings->fifoSize;
        if ( threshold >= 100 )
        {
            BDBG_WRN(("Invalid FIFO threshold requested.  Using Defaults."));
            threshold=0;
        }
        pChannelSettings->sChnRBufPool.sBufSettings.uiWaterMark = threshold;
    }
    else
    {
        /* Use defaults from raptor */
        pChannelSettings->sChnRBufPool.sBufSettings.uiWaterMark=50;
    }

#if BRAP_VER < 4
    pChannelSettings->bOpenTimeWrToRbuf = true; /* Allow writes prior to start for loop around */
#endif

#if BRAP_VER >= 4
    pChannelSettings->sChnRBufPool.bHbrMode = g_NEXUS_audioModuleData.moduleSettings.hbrEnabled;
#endif

    errCode = BRAP_OpenChannel(g_NEXUS_audioModuleData.hRap, &pChannel->rapChannel, pChannelSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_channel;
    }

    errCode = BRAP_InstallAppInterruptCb(pChannel->rapChannel,
                                         BRAP_Interrupt_eFmmRbufFreeByte,
                                         NEXUS_AudioPlayback_P_BufferFree_isr, pChannel, 0);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_interrupt;
    }

    pChannel->appCallback = NEXUS_TaskCallback_Create(pChannel, NULL);
    if ( NULL == pChannel->appCallback )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        goto err_callback;
    }

    errCode = BKNI_CreateEvent(&pChannel->event);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_event;
    }

    pChannel->eventCallback = NEXUS_RegisterEvent(pChannel->event, NEXUS_AudioPlayback_P_DataEvent, pChannel);
    if ( NULL == pChannel->eventCallback )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        goto err_event_callback;
    }

    /* Success */
    BKNI_Free(pChannelSettings);
    pChannel->opened = true;
    return pChannel;

err_event_callback:
    BKNI_DestroyEvent(pChannel->event);
err_event:
    NEXUS_TaskCallback_Destroy(pChannel->appCallback);
err_callback:
    BRAP_RemoveAppInterruptCb(pChannel->rapChannel, BRAP_Interrupt_eFmmRbufFreeByte);
err_interrupt:
    BRAP_CloseChannel(pChannel->rapChannel);
err_channel:
    BKNI_Free(pChannelSettings);
    BKNI_Memset(pChannel, 0, sizeof(*pChannel));
    return NULL;
}

/***************************************************************************
Summary:
Close an audio playback channel
***************************************************************************/
static void NEXUS_AudioPlayback_P_Finalizer(
    NEXUS_AudioPlaybackHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_AudioPlayback, handle);
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);
    if ( handle->started )
    {
        BDBG_WRN(("Automatically stopping audio playback channel %p on close", handle));
        NEXUS_AudioPlayback_Stop(handle);
    }
    BRAP_RemoveAppInterruptCb(handle->rapChannel, BRAP_Interrupt_eFmmRbufFreeByte);
    NEXUS_AudioInput_Shutdown(&handle->connector);
    if ( handle->association )
    {
        NEXUS_AudioAssociation_P_Destroy(handle->association);
    }
    BRAP_CloseChannel(handle->rapChannel);
    NEXUS_TaskCallback_Destroy(handle->appCallback);
    NEXUS_UnregisterEvent(handle->eventCallback);
    BKNI_DestroyEvent(handle->event);
    NEXUS_OBJECT_CLEAR(NEXUS_AudioPlayback, handle);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioPlayback, NEXUS_AudioPlayback_Close);

void NEXUS_AudioPlayback_Flush(
    NEXUS_AudioPlaybackHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);
    if ( handle->started )
    {
#if BRAP_VER >= 4
        (void)BRAP_FlushChannel(handle->rapChannel);
#else
        (void)BRAP_FlushPbChannel(handle->rapChannel);
#endif
    }
}

/***************************************************************************
Summary:
Get default settings for an audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetDefaultStartSettings(
    NEXUS_AudioPlaybackStartSettings *pSettings  /* [out] Default Settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    /* Setup default parameters (the non-zero ones) */
    pSettings->timebase = NEXUS_Timebase_eInvalid;
    pSettings->sampleRate = 48000;
    pSettings->bitsPerSample = 16;
    pSettings->stereo = true;
    pSettings->signedData = true;
    pSettings->startThreshold = (size_t)-1;
}

/***************************************************************************
Summary:
Start playing data data from an audio playback channel
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_Start(
    NEXUS_AudioPlaybackHandle handle,
    const NEXUS_AudioPlaybackStartSettings *pSettings
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioOutputList outputList;
    NEXUS_TimebaseHandle timebase;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);
    BDBG_ASSERT(NULL != pSettings);

    if ( handle->started )
    {
        BDBG_ERR(("Audio playback channel %p already started.", handle));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Check that we have some outputs ready */
    NEXUS_AudioInput_P_GetOutputs(&handle->connector, &outputList, false);
    if ( NULL == outputList.outputs[0] )
    {
        BDBG_ERR(("No outputs are connected to this playback channel.  Please connect outputs before starting."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    (void)BRAP_GetDefaultChannelParams(handle->rapChannel, &handle->audioParams);
    if ( pSettings->sampleRate != 0 )
    {
        handle->audioParams.eInputSR = NEXUS_AudioModule_P_SampleRate2Avc(pSettings->sampleRate);
    }
    else
    {
#if BRAP_VER < 4
        handle->audioParams.eInputSR = BAVC_AudioSamplingRate_e48k;
#else
        BDBG_ERR(("This chipset does not support runtime sample rate adjustments"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
    }
    switch ( pSettings->bitsPerSample )
    {
    case 8:
        handle->audioParams.eInputBitsPerSample = BRAP_InputBitsPerSample_e8;
        break;
    case 16:
        handle->audioParams.eInputBitsPerSample = BRAP_InputBitsPerSample_e16;
        break;
    case 32:
        handle->audioParams.eInputBitsPerSample = BRAP_InputBitsPerSample_e32;
        break;
    default:
        BDBG_ERR(("Unsupported number of bits per channel %u", pSettings->bitsPerSample));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    handle->audioParams.eBufDataMode = (pSettings->stereo)?BRAP_BufDataMode_eStereoInterleaved:BRAP_BufDataMode_eMono;

    NEXUS_Module_Lock(g_NEXUS_audioModuleData.transport);
    timebase = NEXUS_Timebase_Resolve_priv(pSettings->timebase);
    if (timebase)
    {
        unsigned timebaseIndex = 0;

        errCode = NEXUS_Timebase_GetIndex_priv(timebase, &timebaseIndex);
        handle->audioParams.eTimebase = BAVC_Timebase_e0 + timebaseIndex;
    }
    else
    {
        errCode = NEXUS_INVALID_PARAMETER;
    }
    NEXUS_Module_Unlock(g_NEXUS_audioModuleData.transport);
    if (errCode) return BERR_TRACE(errCode);

    handle->audioParams.bIsPcmSigned = pSettings->signedData;
    handle->audioParams.bLoopAroundEn = pSettings->loopAround;

    if ( pSettings->startThreshold != (size_t)-1 )
    {
        handle->audioParams.uiStartWRPtrLocation = pSettings->startThreshold;
    }

#if !NEXUS_GLOBAL_INDEPENDENT_VOLUME
    /***************************************************************************/
    /* This is required to avoid glitches in captured audio due to massive     */
    /* timebase differences between the capture buffer and the output ports    */
    /***************************************************************************/
    handle->audioParams.bIndOpVolCtrl = false;
#endif

    NEXUS_TaskCallback_Set(handle->appCallback, &pSettings->dataCallback);

    NEXUS_AudioInput_P_GetOutputs(&handle->connector, &outputList, true);
    /* If there are no outputs directly connected to this object, we don't need an association */
    if ( outputList.outputs[0] )
    {
        if ( NULL == handle->association )
        {
            BRAP_AssociatedChanSettings associationSettings;

            BKNI_Memset(&associationSettings, 0, sizeof(associationSettings));
            associationSettings.sChDetail[0].hRapCh = handle->rapChannel;
            handle->association = NEXUS_AudioAssociation_P_Create(&associationSettings);
            if ( NULL == handle->association )
            {
                return BERR_TRACE(NEXUS_NOT_SUPPORTED);
            }
        }
    }
    /* Connect to outputs */
    NEXUS_AudioInput_P_PrepareToStart(&handle->connector, handle->association);

    /* Reset bytes played */
    handle->bytesPlayed = 0;
    /* Save Settings */
    handle->started = true;
    handle->startSettings = *pSettings;
    handle->pLastAddress = NULL;

    NEXUS_AudioPlayback_SetSettings(handle, &handle->settings);

    /* Start Playback */
    errCode = BRAP_StartChannel(handle->rapChannel, &handle->audioParams);
    if ( errCode )
    {
        handle->started = false;
        return BERR_TRACE(errCode);
    }

    /* If using a custom sample rate apply it now */
    if ( 0 == pSettings->sampleRate )
    {
#if BRAP_VER < 4
        (void)BRAP_PB_SetInputSampleRate(handle->rapChannel, handle->settings.sampleRate);
#endif
    }
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Stop playing data from an audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_Stop(
    NEXUS_AudioPlaybackHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);

    if ( handle->started )
    {
        BRAP_StopChannel(handle->rapChannel);
        handle->started = false;
    }
}

/***************************************************************************
Summary:
Get a pointer and size for the next location in the buffer which can accept data

Description:
NEXUS_AudioPlayback_GetBuffer is non-destructive. You can safely call it multiple times.
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_GetBuffer(
    NEXUS_AudioPlaybackHandle handle,
    void **pBuffer, /* [out] attr{memory=cached} pointer to memory mapped region that is ready for playback data */
    size_t *pSize   /* [out] total number of writeable, contiguous bytes which buffer is pointing to */
    )
{
    BERR_Code errCode;
    BRAP_PcmBufInfo bufInfo;
    void *pBase;
    size_t length;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);
    BDBG_ASSERT(NULL != pBuffer);
    BDBG_ASSERT(NULL != pSize);

    errCode = BRAP_PB_GetBufInfo(handle->rapChannel, &bufInfo);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( bufInfo.sRtBufInfo.uiContiguousSize )
    {
        BDBG_ERR(("Dual channel PCM playback not supported."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( handle->started )
    {
        pBase = bufInfo.sLtOrSnglBufInfo.pWritePtr;
        length = bufInfo.sLtOrSnglBufInfo.uiContiguousSize;
    }
    else
    {
        pBase = bufInfo.sLtOrSnglBufInfo.pBasePtr;
        length = bufInfo.sLtOrSnglBufInfo.uiTotalSize;
    }

    errCode = BMEM_Heap_ConvertAddressToCached(g_pCoreHandles->heap[0], pBase, pBuffer);

    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->pLastAddress = *pBuffer;  /* Save this for the cacheflush on write complete */

    *pSize = length;
    BDBG_MSG(("NEXUS_AudioPlayback_GetBuffer %p, %d", *pBuffer, *pSize));

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Notify AudioPlayback how much data was added into the buffer.

Description:
You can only call NEXUS_AudioPlayback_WriteComplete once after a
NEXUS_AudioPlayback_GetBuffer call.  After calling it, you must call
NEXUS_AudioPlayback_GetBuffer before adding more data.
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_WriteComplete(
    NEXUS_AudioPlaybackHandle handle,
    size_t amountWritten            /* The number of bytes written to the buffer */
    )
{
    BERR_Code errCode;
    BRAP_PcmBufSzUpdt update;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);

    /* this code assumes (and asserts) that there will be at least one get_buffer before each write_complete.
    this should be true regardless of cache flush, but it makes the cache flush algo easier too. */
    if ( NULL == handle->pLastAddress )
    {
        BDBG_ERR(("You must call NEXUS_AudioPlayback_GetBuffer before calling NEXUS_AudioPlayback_WriteComplete"));
        BDBG_ASSERT(NULL != handle->pLastAddress);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    if ( amountWritten > 0 )
    {
        BMEM_Heap_FlushCache(g_pCoreHandles->heap[0], handle->pLastAddress, amountWritten);
    }
    handle->pLastAddress = NULL;

    update.uiLtOrSnglBufSzUpdt = amountWritten;
    update.uiRtBufSzUpdt = 0;

    errCode = BRAP_PB_UpdateBufUsg(handle->rapChannel, &update);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    BDBG_MSG(("NEXUS_AudioPlayback_WriteComplete received %u bytes", amountWritten));

    errCode = BRAP_PB_BufferWriteDone(handle->rapChannel);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Increment bytes played for status purposes */
    handle->bytesPlayed += amountWritten;

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Get current status of the audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetStatus(
    NEXUS_AudioPlaybackHandle handle,
    NEXUS_AudioPlaybackStatus *pStatus      /* [out] Current Status */
    )
{
    BERR_Code errCode;
    BRAP_PcmBufInfo bufInfo;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);
    BDBG_ASSERT(NULL != pStatus);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    pStatus->started = handle->started;
    pStatus->startSettings = handle->startSettings;

    errCode = BRAP_PB_GetBufInfo(handle->rapChannel, &bufInfo);
    if ( errCode == BERR_SUCCESS )
    {
        pStatus->fifoSize = (((uint8_t*)bufInfo.sLtOrSnglBufInfo.pEndPtr)+1) - (uint8_t*)bufInfo.sLtOrSnglBufInfo.pBasePtr; /* number of bytes */
        pStatus->queuedBytes = pStatus->fifoSize - bufInfo.sLtOrSnglBufInfo.uiTotalSize; /* total - usable free == queued */
        pStatus->playedBytes = handle->bytesPlayed - pStatus->queuedBytes;
    }
}

/***************************************************************************
Summary:
Get an audio connector for use with downstream components.
**************************************************************************/
NEXUS_AudioInput NEXUS_AudioPlayback_GetConnector( /* attr{shutdown=NEXUS_AudioInput_Shutdown} */
    NEXUS_AudioPlaybackHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);
    return &handle->connector;
}

/* Handle the raptor buffer interrupt */
static void NEXUS_AudioPlayback_P_BufferFree_isr(void *pParam1, int param2, void *pData)
{
    NEXUS_AudioPlaybackHandle handle = pParam1;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);
    BSTD_UNUSED(param2);
    BSTD_UNUSED(pData);

    BKNI_SetEvent(handle->event);
}

/* Handle the raptor data ready callback */
static void NEXUS_AudioPlayback_P_DataEvent(void *pParam)
{
    NEXUS_Error errCode;
    NEXUS_AudioPlaybackHandle handle = pParam;
    void *pBuffer;
    size_t size=0;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);

    /* non-blocking get of buffer space */
    /* If it fails, or if there's no space available, don't send a callback */
    errCode = NEXUS_AudioPlayback_GetBuffer(handle, &pBuffer, &size);
    if ( errCode )
    {
        BDBG_MSG(("No space available"));
        return;
    }
    else if ( size == 0 )
    {
        NEXUS_AudioPlayback_WriteComplete(handle, 0);
        return;
    }

    /* call back to the user with the space available -- this will do nothing if no callback is present */
    NEXUS_TaskCallback_Fire(handle->appCallback);
}

/* Private routine to get channel state */
bool NEXUS_AudioPlayback_P_IsRunning(NEXUS_AudioPlaybackHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);
    return handle->started;
}

/* Private routine to get channel handle */
BRAP_ChannelHandle NEXUS_AudioPlayback_P_GetChannel(NEXUS_AudioPlaybackHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioPlayback);
    return handle->rapChannel;
}

static NEXUS_Error NEXUS_AudioPlayback_P_ConnectionChange(void *pParam, NEXUS_AudioInput input)
{
    NEXUS_AudioPlayback *pDecoder = pParam;
    BSTD_UNUSED(input);
    BDBG_ASSERT(NULL != pDecoder);

    BDBG_MSG(("Audio playback %p connection change", pParam));
    if ( pDecoder->association )
    {
        BDBG_MSG(("Destroying association on connection change"));
        NEXUS_AudioAssociation_P_Destroy(pDecoder->association);
        pDecoder->association = NULL;
    }
    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
Get current setting of the audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetSettings(
    NEXUS_AudioPlaybackHandle handle,
    NEXUS_AudioPlaybackSettings *pSettings  /* [out] Current settings */
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

/***************************************************************************
Summary:
Set current setting of the audio playback channel
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_SetSettings(
    NEXUS_AudioPlaybackHandle handle,
    const NEXUS_AudioPlaybackSettings *pSettings
    )
{
    BRAP_ChannelConfigParams *pConfig;
    NEXUS_Error errCode;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(NULL != pSettings);
    handle->settings = *pSettings;

    pConfig = BKNI_Malloc(sizeof(*pConfig));
    if ( NULL == pConfig )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    errCode = BRAP_GetCurrentChannelConfig(handle->rapChannel,
                                           BRAP_DSPCHN_AudioType_eInvalid,
                                           #if BCHP_CHIP == 3563
                                           BRAP_MAX_PP_BRANCH_SUPPORTED,
                                           BRAP_MAX_PP_PER_BRANCH_SUPPORTED,
                                           #endif
                                           pConfig);
    if ( BERR_SUCCESS == errCode )
    {
        if ( pSettings->muted )
        {
            pConfig->sMixingCoef.ui32Coef[BRAP_OutputChannel_eLeft][BRAP_OutputChannel_eLeft] = 0;
            pConfig->sMixingCoef.ui32Coef[BRAP_OutputChannel_eRight][BRAP_OutputChannel_eRight] = 0;
        }
        else
        {
            pConfig->sMixingCoef.ui32Coef[BRAP_OutputChannel_eLeft][BRAP_OutputChannel_eLeft] = pSettings->leftVolume;
            pConfig->sMixingCoef.ui32Coef[BRAP_OutputChannel_eRight][BRAP_OutputChannel_eRight] = pSettings->rightVolume;
        }

        handle->audioParams.sMixingCoeff = pConfig->sMixingCoef;

        errCode = BRAP_SetChannelConfig(handle->rapChannel, pConfig);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
        }
    }
    else
    {
        errCode = BERR_TRACE(errCode);
    }    

    BKNI_Free(pConfig);

    if ( handle->startSettings.sampleRate == 0 )
    {
#if BRAP_VER < 4
        (void)BRAP_PB_SetInputSampleRate(handle->rapChannel, pSettings->sampleRate);
#endif
    }

    return errCode;
}

#else
/***************************************************************************
Summary:
Get default settings for an audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetDefaultOpenSettings(
    NEXUS_AudioPlaybackOpenSettings *pSettings      /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
Open an audio playback channel
***************************************************************************/
NEXUS_AudioPlaybackHandle NEXUS_AudioPlayback_Open(     /* attr{destructor=NEXUS_AudioPlayback_Close}  */
    unsigned index,
    const NEXUS_AudioPlaybackOpenSettings *pSettings    /* Pass NULL for default settings */
    )
{
    NEXUS_Error errCode;
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

/***************************************************************************
Summary:
Close an audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_Close(
    NEXUS_AudioPlaybackHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/***************************************************************************
Summary:
Get default settings for an audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetDefaultStartSettings(
    NEXUS_AudioPlaybackStartSettings *pSettings  /* [out] Default Settings */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
Start playing data data from an audio playback channel
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_Start(
    NEXUS_AudioPlaybackHandle handle,
    const NEXUS_AudioPlaybackStartSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
Stop playing data from an audio playback channel.

Description:
This will stop the channel and flush all data from the FIFO.
***************************************************************************/
void NEXUS_AudioPlayback_Stop(
    NEXUS_AudioPlaybackHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/***************************************************************************
Summary:
Get a pointer and size for the next location in the buffer which can accept data

Description:
NEXUS_AudioPlayback_GetBuffer is non-destructive. You can safely call it multiple times.
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_GetBuffer(
    NEXUS_AudioPlaybackHandle handle,
    void **pBuffer, /* [out] attr{memory=cached} pointer to memory mapped region that is ready for playback data */
    size_t *pSize   /* [out] total number of writeable, contiguous bytes which buffer is pointing to */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pBuffer);
    BSTD_UNUSED(pSize);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
Notify AudioPlayback how much data was added into the buffer.

Description:
You can only call NEXUS_AudioPlayback_WriteComplete once after a
NEXUS_AudioPlayback_GetBuffer call.  After calling it, you must call
NEXUS_AudioPlayback_GetBuffer before adding more data.
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_WriteComplete(
    NEXUS_AudioPlaybackHandle handle,
    size_t amountWritten            /* The number of bytes written to the buffer */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(amountWritten);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
Get current status of the audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetStatus(
    NEXUS_AudioPlaybackHandle handle,
    NEXUS_AudioPlaybackStatus *pStatus      /* [out] Current Status */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pStatus);
}

/***************************************************************************
Summary:
Get an audio connector for use with downstream components.
**************************************************************************/
NEXUS_AudioInput NEXUS_AudioPlayback_GetConnector( /* attr{shutdown=NEXUS_AudioInput_Shutdown} */
    NEXUS_AudioPlaybackHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}

/***************************************************************************
Summary:
Get current setting of the audio playback channel
***************************************************************************/
void NEXUS_AudioPlayback_GetSettings(
    NEXUS_AudioPlaybackHandle handle,
    NEXUS_AudioPlaybackSettings *pSettings  /* [out] Current settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
Set current setting of the audio playback channel
***************************************************************************/
NEXUS_Error NEXUS_AudioPlayback_SetSettings(
    NEXUS_AudioPlaybackHandle handle,
    const NEXUS_AudioPlaybackSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

#endif /* #if NEXUS_NUM_AUDIO_PLAYBACKS */

