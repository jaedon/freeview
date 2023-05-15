/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_output_capture.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 8/22/12 2:13p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_output_capture.c $
 * 
 * Hydra_Software_Devel/21   8/22/12 2:13p mpeteri
 * SW7429-249: merge to Hydra
 * 
 * Hydra_Software_Devel/SW7429-249/1   8/22/12 2:02p mpeteri
 * SW7429-249: remove misplaced assert
 * 
 * Hydra_Software_Devel/20   5/30/12 12:33p jgarrett
 * SW7435-207: Resetting ISR on flush call
 * 
 * Hydra_Software_Devel/19   5/16/12 11:45a jgarrett
 * SW7425-3071: Adding programmable heaps
 * 
 * Hydra_Software_Devel/18   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/17   3/6/12 3:01p gskerl
 * SW7425-2570: Renamed OutputPortObject substruct from connector to
 * outputPort.
 * 
 * Hydra_Software_Devel/16   1/27/12 4:53p jgarrett
 * SW7429-55: Allowing setMclk_isr to be called prior to enable on 7429
 * 
 * Hydra_Software_Devel/15   11/14/11 3:36p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/14   11/1/11 1:54p gskerl
 * SW7425-1645: Set interrupte handlers using new passed in values instead
 * of existing values.
 * 
 * Hydra_Software_Devel/13   8/22/11 4:16p gskerl
 * SW7425-1179: Corrected BAPE_OutputCapture_Open() to use reasonable
 * default settings when called with a NULL settings argument. Also
 * modified BAPE_OutputCapture_GetDefaultOpenSettings() to provide
 * defaults for numBuffers and watermarkThreshold settings.
 * 
 * Hydra_Software_Devel/12   5/23/11 6:49p jgarrett
 * SW7425-402: Adding multichannel capture support
 * 
 * Hydra_Software_Devel/11   5/20/11 5:16p jgarrett
 * SW7425-402: Adding error code to handle output enable callback failing.
 * 
 * Hydra_Software_Devel/10   5/3/11 10:47a gskerl
 * SW7422-354: Added index and type args to APE_P_InitOutputPort macro
 * 
 * Hydra_Software_Devel/9   4/16/11 12:59p jgarrett
 * SW7425-372: Fixing loopback output resolution
 * 
 * Hydra_Software_Devel/8   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/7   4/6/11 1:23a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:49p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/6   3/22/11 3:00p gskerl
 * SW7422-146: Changed audio output connector callbacks to take the output
 * connector as an argument
 * 
 * Hydra_Software_Devel/5   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/4   3/10/11 7:03p jgarrett
 * SW7422-146: Refactored DFIFO code, added support for input capture from
 * compressed/multichannel
 * 
 * Hydra_Software_Devel/3   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 * Hydra_Software_Devel/2   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/1   12/16/10 4:05p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"

BDBG_MODULE(bape_output_capture);

BDBG_OBJECT_ID(BAPE_OutputCapture);

typedef struct BAPE_OutputCapture
{
    BDBG_OBJECT(BAPE_OutputCapture)
    BAPE_Handle hApe;
    BMEM_Heap_Handle hHeap;
    unsigned bufferSize, watermark;
    BAPE_OutputCaptureSettings settings;
    unsigned index;
    BAPE_OutputPortObject outputPort;
    unsigned sampleRate;
    unsigned numBuffers;
    void *pBuffers[BAPE_Channel_eMax];
    uint32_t bufferOffset[BAPE_Channel_eMax];
    BAPE_DfifoGroupHandle dfifoGroup;
    BAPE_LoopbackGroupHandle loopbackGroup;
#if BAPE_CHIP_MAX_FS > 0
    unsigned fs;
#else
    BAPE_MclkSource mclkSource;
    unsigned pllChannel;
    unsigned mclkFreqToFsRatio;
#endif
    bool enabled;
    BAPE_OutputCaptureInterruptHandlers interrupts;
    char name[7];   /* CAP %d */
} BAPE_OutputCapture;

static void BAPE_OutputCapture_P_ClearInterrupts_isr(BAPE_OutputCaptureHandle hOutputCapture);

/* Connector callbacks */
static void BAPE_OutputCapture_P_SetTimingParams_isr(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase);
static BERR_Code BAPE_OutputCapture_P_Enable(BAPE_OutputPort output);
static void BAPE_OutputCapture_P_Disable(BAPE_OutputPort output);
#if BAPE_CHIP_MAX_FS > 0
static void BAPE_OutputCapture_P_SetFs(BAPE_OutputPort output, unsigned fsNum);
#else
static void BAPE_OutputCapture_P_SetMclk_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio);
#endif

void BAPE_OutputCapture_GetDefaultOpenSettings(
    BAPE_OutputCaptureOpenSettings *pSettings       /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));    
    pSettings->bufferSize = BDSP_AF_P_NON_DELAY_RBUF_SIZE*2;
    pSettings->numBuffers = 1;
    pSettings->watermarkThreshold = pSettings->bufferSize / 2;
}

BERR_Code BAPE_OutputCapture_Open(
    BAPE_Handle hApe,
    unsigned index,
    const BAPE_OutputCaptureOpenSettings *pSettings,
    BAPE_OutputCaptureHandle *pHandle             /* [out] */
    )
{
    BERR_Code errCode;
    BAPE_OutputCaptureHandle hOutputCapture;
    BAPE_OutputCaptureOpenSettings defaultSettings;
    void *pMem, *pCachedMem;
    unsigned bufferSize=0, watermark=0;
    unsigned maxChannelPairs, numBuffers=1;
    unsigned i;

    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);

    if ( index >= BAPE_CHIP_MAX_OUTPUT_CAPTURES )
    {
        BDBG_ERR(("Unable to open output capture %u.  Max output captures is %u on this platform.", index, BAPE_CHIP_MAX_OUTPUT_CAPTURES));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    
    if ( NULL == pSettings )
    {
        BAPE_OutputCapture_GetDefaultOpenSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* Determine buffer size */
    if ( NULL != pSettings )
    {
        bufferSize = pSettings->bufferSize;
        watermark = pSettings->watermarkThreshold;
        if ( pSettings->numBuffers > BAPE_Channel_eMax )
        {
            BDBG_WRN(("Requested %u buffers but only %u channels are possible.  Allocating %u buffers.",
                      pSettings->numBuffers, BAPE_Channel_eMax, BAPE_Channel_eMax));
            numBuffers = BAPE_Channel_eMax;
        }
        else
        {
            numBuffers = pSettings->numBuffers;
        }
    }
    if ( 0 == bufferSize )
    {
        bufferSize = hApe->buffers[0].bufferSize;
    }
    if ( 0 == watermark )
    {
        watermark = bufferSize/4;
    }

    /* Allocate handle */
    hOutputCapture = BKNI_Malloc(sizeof(BAPE_OutputCapture));
    if ( NULL == hOutputCapture )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(hOutputCapture, 0, sizeof(BAPE_OutputCapture));

    BDBG_OBJECT_SET(hOutputCapture, BAPE_OutputCapture);
    hOutputCapture->hApe = hApe;
    hOutputCapture->hHeap = pSettings->heap ? pSettings->heap : hApe->memHandle;
    hOutputCapture->bufferSize = bufferSize;
    hOutputCapture->watermark = watermark;
    hOutputCapture->settings.bitsPerSample = 32;
    hOutputCapture->settings.interleaveData = true;
    hOutputCapture->index = index;
    BAPE_P_InitOutputPort(&hOutputCapture->outputPort, BAPE_OutputPortType_eOutputCapture, index, hOutputCapture);
    maxChannelPairs = BAPE_CHIP_MAX_LOOPBACKS;
    if ( maxChannelPairs > BAPE_CHIP_MAX_DFIFOS )
    {
        maxChannelPairs = BAPE_CHIP_MAX_DFIFOS;
    }
    if ( maxChannelPairs > pSettings->numBuffers )
    {
        maxChannelPairs = pSettings->numBuffers;
    }
    if ( maxChannelPairs > BAPE_ChannelPair_eMax )
    {
        maxChannelPairs = BAPE_ChannelPair_eMax;
    }
    BAPE_FMT_P_EnableSource(&hOutputCapture->outputPort.capabilities, BAPE_DataSource_eFci);
    BAPE_FMT_P_EnableType(&hOutputCapture->outputPort.capabilities, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableType(&hOutputCapture->outputPort.capabilities, BAPE_DataType_eIec61937);
    BAPE_FMT_P_EnableType(&hOutputCapture->outputPort.capabilities, BAPE_DataType_eIec61937x4);
    switch ( maxChannelPairs )
    {
    case 4:
        BAPE_FMT_P_EnableType(&hOutputCapture->outputPort.capabilities, BAPE_DataType_ePcm7_1);
        /* Fall through */
    case 3:
        BAPE_FMT_P_EnableType(&hOutputCapture->outputPort.capabilities, BAPE_DataType_ePcm5_1);
        break;
    default:
        break;
    }
    hOutputCapture->outputPort.enable = BAPE_OutputCapture_P_Enable;
    hOutputCapture->outputPort.disable = BAPE_OutputCapture_P_Disable;
    hOutputCapture->outputPort.setTimingParams_isr = BAPE_OutputCapture_P_SetTimingParams_isr;
    hOutputCapture->sampleRate = 0;
#if BAPE_CHIP_MAX_FS > 0
    hOutputCapture->outputPort.fsTiming = true;
    hOutputCapture->outputPort.setFs = BAPE_OutputCapture_P_SetFs;
    hOutputCapture->fs = (unsigned)-1;
#else
    hOutputCapture->outputPort.setMclk_isr = BAPE_OutputCapture_P_SetMclk_isr;
    hOutputCapture->mclkSource = BAPE_MclkSource_eNone;
    hOutputCapture->pllChannel = 0;
    hOutputCapture->mclkFreqToFsRatio = BAPE_BASE_PLL_TO_FS_RATIO;
#endif
    BKNI_Snprintf(hOutputCapture->name, sizeof(hOutputCapture->name), "CAP %u", index); 
    hOutputCapture->outputPort.pName = hOutputCapture->name;

    hOutputCapture->numBuffers = numBuffers;
    for ( i = 0; i < numBuffers; i++ )
    {
        /* Allocate buffer */
        hOutputCapture->pBuffers[i] = pMem = BMEM_Heap_AllocAligned(hOutputCapture->hHeap, bufferSize, 8, 0);
        if ( NULL == pMem )
        {
            errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto err_buffer;
        }
    
        errCode = BMEM_Heap_ConvertAddressToOffset(hOutputCapture->hHeap, pMem, &hOutputCapture->bufferOffset[i]);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_offset;
        }
    
        errCode = BMEM_Heap_ConvertAddressToCached(hOutputCapture->hHeap, pMem, &pCachedMem);
        if ( BERR_SUCCESS == errCode )
        {
            /* Flush once at open to make sure the buffer has been invalidated from the cache. */
            BMEM_Heap_FlushCache(hOutputCapture->hHeap, pCachedMem, bufferSize);
        }
        /* Not fatal if it fails, assume no cache support */
    }

    hOutputCapture->hApe->outputCaptures[index] = hOutputCapture;

    *pHandle = hOutputCapture;
    return BERR_SUCCESS;

err_offset:
err_buffer:
    for ( i = 0; i < BAPE_Channel_eMax; i++ )
    {
        if ( hOutputCapture->pBuffers[i] )
        {
            BMEM_Heap_Free(hOutputCapture->hHeap, hOutputCapture->pBuffers[i]);
        }
    }
    BDBG_OBJECT_DESTROY(hOutputCapture, BAPE_OutputCapture);
    BKNI_Free(hOutputCapture);
    return errCode;
}

void BAPE_OutputCapture_Close(
    BAPE_OutputCaptureHandle hOutputCapture
    )
{
    unsigned i;
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);
    BDBG_ASSERT(hOutputCapture->enabled == false);
    BDBG_ASSERT(hOutputCapture->outputPort.mixer == NULL);
    BDBG_ASSERT(NULL == hOutputCapture->dfifoGroup);
    BDBG_ASSERT(NULL == hOutputCapture->loopbackGroup);
    for ( i = 0; i < BAPE_Channel_eMax; i++ )
    {
        if ( hOutputCapture->pBuffers[i] )
        {
            BMEM_Heap_Free(hOutputCapture->hHeap, hOutputCapture->pBuffers[i]);
        }
    }
    hOutputCapture->hApe->outputCaptures[hOutputCapture->index] = NULL;
    BDBG_OBJECT_DESTROY(hOutputCapture, BAPE_OutputCapture);
    BKNI_Free(hOutputCapture);
}

void BAPE_OutputCapture_GetSettings(
    BAPE_OutputCaptureHandle hOutputCapture,
    BAPE_OutputCaptureSettings *pSettings       /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = hOutputCapture->settings;
}

BERR_Code BAPE_OutputCapture_SetSettings(
    BAPE_OutputCaptureHandle hOutputCapture,
    const BAPE_OutputCaptureSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);
    BDBG_ASSERT(NULL != pSettings);
#if BAPE_CHIP_DFIFO_SUPPORTS_16BIT_CAPTURE
    if ( pSettings->bitsPerSample != 32 && pSettings->bitsPerSample != 16)
    {
        BDBG_ERR(("Only 16-bit and 32-bit capture samples are supported on this chipset."));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
#else
    if ( pSettings->bitsPerSample != 32 )
    {
        BDBG_ERR(("Only 32-bit capture samples are supported on this chipset."));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
#endif
    hOutputCapture->settings = *pSettings;
    return BERR_SUCCESS;
}

void BAPE_OutputCapture_Flush(
    BAPE_OutputCaptureHandle hOutputCapture
    )
{
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);
    BKNI_EnterCriticalSection();
    BAPE_OutputCapture_Flush_isr(hOutputCapture);
    BKNI_LeaveCriticalSection();
}

void BAPE_OutputCapture_Flush_isr(
    BAPE_OutputCaptureHandle hOutputCapture
    )
{
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);
    
    if ( hOutputCapture->dfifoGroup )
    {
        BAPE_DfifoGroup_P_Flush_isr(hOutputCapture->dfifoGroup);
        BAPE_OutputCapture_P_ClearInterrupts_isr(hOutputCapture);
    }
}

BERR_Code BAPE_OutputCapture_GetBuffer(
    BAPE_OutputCaptureHandle hOutputCapture,
    BAPE_BufferDescriptor *pBuffers         /* [out] */
    )
{   
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);
    BDBG_ASSERT(NULL != pBuffers);

    BKNI_Memset(pBuffers, 0, sizeof(*pBuffers));
    pBuffers->interleaved = hOutputCapture->settings.interleaveData;
    if ( !hOutputCapture->enabled )
    {
        return BERR_SUCCESS;
    }

    BDBG_ASSERT(NULL != hOutputCapture->dfifoGroup);
    BKNI_EnterCriticalSection();
    errCode = BAPE_DfifoGroup_P_GetBuffer_isr(hOutputCapture->dfifoGroup, pBuffers);
    BKNI_LeaveCriticalSection();
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

BERR_Code BAPE_OutputCapture_ConsumeData(
    BAPE_OutputCaptureHandle hOutputCapture,
    unsigned numBytes                   /* Number of bytes read from the buffer */
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);

    if ( !hOutputCapture->enabled )
    {
        return BERR_SUCCESS;
    }

    BDBG_ASSERT(NULL != hOutputCapture->dfifoGroup);
    BKNI_EnterCriticalSection();
    errCode = BAPE_DfifoGroup_P_CommitData_isr(hOutputCapture->dfifoGroup, numBytes);
    BAPE_OutputCapture_P_ClearInterrupts_isr(hOutputCapture);
    BKNI_LeaveCriticalSection();

    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

void BAPE_OutputCapture_GetOutputPort(
    BAPE_OutputCaptureHandle hOutputCapture,
    BAPE_OutputPort *pOutputPort
    )
{
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);
    BDBG_ASSERT(NULL != pOutputPort);
    *pOutputPort = &hOutputCapture->outputPort;
}

void BAPE_OutputCapture_GetInterruptHandlers(
    BAPE_OutputCaptureHandle hOutputCapture,
    BAPE_OutputCaptureInterruptHandlers *pInterrupts    /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);
    BDBG_ASSERT(NULL != pInterrupts);
    *pInterrupts = hOutputCapture->interrupts;
}

BERR_Code BAPE_OutputCapture_SetInterruptHandlers(
    BAPE_OutputCaptureHandle hOutputCapture,
    const BAPE_OutputCaptureInterruptHandlers *pInterrupts
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);
    BDBG_ASSERT(NULL != pInterrupts);

    if ( hOutputCapture->enabled )
    {
        BDBG_ASSERT(NULL != hOutputCapture->dfifoGroup);
        /* Install interrupt handlers */
        errCode = BAPE_DfifoGroup_P_SetFullmarkInterrupt(hOutputCapture->dfifoGroup,
                                                         pInterrupts->watermark.pCallback_isr,
                                                         pInterrupts->watermark.pParam1,
                                                         pInterrupts->watermark.param2);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        errCode = BAPE_DfifoGroup_P_SetOverflowInterrupt(hOutputCapture->dfifoGroup,
                                                         pInterrupts->overflow.pCallback_isr,
                                                         pInterrupts->overflow.pParam1,
                                                         pInterrupts->overflow.param2);
        if ( errCode )
        {
            BAPE_DfifoGroup_P_SetFullmarkInterrupt(hOutputCapture->dfifoGroup, NULL, NULL, 0);
            return BERR_TRACE(errCode);
        }
    }

    hOutputCapture->interrupts = *pInterrupts;
    return BERR_SUCCESS;
}

static BERR_Code BAPE_OutputCapture_P_Enable(BAPE_OutputPort output)
{
    BERR_Code errCode;
    BAPE_DfifoGroupSettings dfifoSettings;
    BAPE_LoopbackGroupSettings loopbackSettings;
    BAPE_OutputCaptureHandle hOutputCapture;
    BAPE_LoopbackGroupCreateSettings loopbackCreateSettings;
    BAPE_DfifoGroupCreateSettings dfifoCreateSettings;
    unsigned numBuffersRequired, i, step, numChannelPairs;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    hOutputCapture = output->pHandle;
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);

    BDBG_ASSERT(false == hOutputCapture->enabled);

    /* Make sure we have enough buffers to satisfy this request */
    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(BAPE_Mixer_P_GetOutputFormat(output->mixer));
    numBuffersRequired = numChannelPairs;
    if ( hOutputCapture->settings.interleaveData )
    {
        step = 2;
    }
    else
    {
        numBuffersRequired *= 2;
        step = 1;
    }
    if ( numBuffersRequired > hOutputCapture->numBuffers )
    {
        BDBG_ERR(("To support %u channels %s requires %u buffers but only %u are allocated for OutputCapture %u", 
                  numChannelPairs, (hOutputCapture->settings.interleaveData)?"interleaved":"non-interleaved",
                  numBuffersRequired, hOutputCapture->numBuffers, hOutputCapture->index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Allocate needed loopback resources */
    BAPE_LoopbackGroup_P_GetDefaultCreateSettings(&loopbackCreateSettings);
    loopbackCreateSettings.numChannelPairs = numChannelPairs;
    errCode = BAPE_LoopbackGroup_P_Create(hOutputCapture->hApe, &loopbackCreateSettings, &hOutputCapture->loopbackGroup);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_loopback_alloc;
    }

    /* Allocate needed DFIFO resources */
    BAPE_DfifoGroup_P_GetDefaultCreateSettings(&dfifoCreateSettings);
    dfifoCreateSettings.numChannelPairs = numChannelPairs;
    errCode = BAPE_DfifoGroup_P_Create(hOutputCapture->hApe, &dfifoCreateSettings, &hOutputCapture->dfifoGroup);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_dfifo_alloc;
    }

    /* Setup and enable the DFIFO first, then Loopback */
    BAPE_DfifoGroup_P_GetSettings(hOutputCapture->dfifoGroup, &dfifoSettings);
    dfifoSettings.highPriority = (BAPE_Mixer_P_GetOutputSampleRate(output->mixer)) >= 96000 ? true : false;
    BAPE_LoopbackGroup_P_GetCaptureFciIds(hOutputCapture->loopbackGroup, &dfifoSettings.input);
    dfifoSettings.interleaveData = hOutputCapture->settings.interleaveData;
    dfifoSettings.dataWidth = hOutputCapture->settings.bitsPerSample;
    for ( i = 0; i < numBuffersRequired; i++ )
    {
        unsigned dfifoBufIndex = i*step;
        dfifoSettings.bufferInfo[dfifoBufIndex].base = hOutputCapture->bufferOffset[i];
        dfifoSettings.bufferInfo[dfifoBufIndex].length = hOutputCapture->bufferSize;
        dfifoSettings.bufferInfo[dfifoBufIndex].watermark = hOutputCapture->watermark;
    }
    errCode = BAPE_DfifoGroup_P_SetSettings(hOutputCapture->dfifoGroup, &dfifoSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_dfifo_settings;
    }

    /* Configure Loopback */
    BAPE_LoopbackGroup_P_GetSettings(hOutputCapture->loopbackGroup, &loopbackSettings);
#if BAPE_CHIP_MAX_FS > 0
    loopbackSettings.fs = hOutputCapture->fs;
#else
    loopbackSettings.mclkSource = hOutputCapture->mclkSource;    
    loopbackSettings.pllChannel = hOutputCapture->pllChannel;
    loopbackSettings.mclkFreqToFsRatio = hOutputCapture->mclkFreqToFsRatio;
#endif
    loopbackSettings.input = hOutputCapture->outputPort.sourceMixerFci;
    loopbackSettings.resolution = hOutputCapture->settings.bitsPerSample > 24 ? 24 : hOutputCapture->settings.bitsPerSample;
    errCode = BAPE_LoopbackGroup_P_SetSettings(hOutputCapture->loopbackGroup, &loopbackSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_loopback_settings;
    }

    /* Install interrupt handlers */
    if ( hOutputCapture->interrupts.watermark.pCallback_isr )
    {
        errCode = BAPE_DfifoGroup_P_SetFullmarkInterrupt(hOutputCapture->dfifoGroup,
                                                         hOutputCapture->interrupts.watermark.pCallback_isr,
                                                         hOutputCapture->interrupts.watermark.pParam1,
                                                         hOutputCapture->interrupts.watermark.param2);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_fullmark;
        }
    }
    if ( hOutputCapture->interrupts.overflow.pCallback_isr )
    {
        errCode = BAPE_DfifoGroup_P_SetOverflowInterrupt(hOutputCapture->dfifoGroup,
                                                         hOutputCapture->interrupts.overflow.pCallback_isr,
                                                         hOutputCapture->interrupts.overflow.pParam1,
                                                         hOutputCapture->interrupts.overflow.param2);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_overflow;
        }
    }

    /* Enable DFIFO */
    errCode = BAPE_DfifoGroup_P_Start(hOutputCapture->dfifoGroup, false);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_dfifo_start;
    }

    /* Enable Loopback */
    errCode = BAPE_LoopbackGroup_P_Start(hOutputCapture->loopbackGroup);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_loopback_start;
    }

    /* Done */
    hOutputCapture->enabled = true;
    return BERR_SUCCESS;

err_loopback_start:
    BAPE_DfifoGroup_P_Stop(hOutputCapture->dfifoGroup);
err_dfifo_start:
    BAPE_DfifoGroup_P_SetOverflowInterrupt(hOutputCapture->dfifoGroup, NULL, NULL, 0);
err_overflow:
    BAPE_DfifoGroup_P_SetFullmarkInterrupt(hOutputCapture->dfifoGroup, NULL, NULL, 0);
err_fullmark:
err_loopback_settings:
err_dfifo_settings:
    BAPE_DfifoGroup_P_Destroy(hOutputCapture->dfifoGroup);
    hOutputCapture->dfifoGroup = NULL;
err_dfifo_alloc:
    BAPE_LoopbackGroup_P_Destroy(hOutputCapture->loopbackGroup);
    hOutputCapture->loopbackGroup = NULL;
err_loopback_alloc:
    return errCode;
}

static void BAPE_OutputCapture_P_Disable(BAPE_OutputPort output)
{
    BAPE_OutputCaptureHandle hOutputCapture;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    hOutputCapture = output->pHandle;
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);

    if ( hOutputCapture->loopbackGroup )
    {
        /* Disable Loopback */
        BAPE_LoopbackGroup_P_Stop(hOutputCapture->loopbackGroup);

        if ( hOutputCapture->dfifoGroup )
        {
            /* Disable DFIFO */
            BAPE_DfifoGroup_P_Stop(hOutputCapture->dfifoGroup);

            /* Clear interrupts */
            BAPE_DfifoGroup_P_SetFullmarkInterrupt(hOutputCapture->dfifoGroup, NULL, NULL, 0);
            BAPE_DfifoGroup_P_SetOverflowInterrupt(hOutputCapture->dfifoGroup, NULL, NULL, 0);

            BAPE_DfifoGroup_P_Destroy(hOutputCapture->dfifoGroup);
            hOutputCapture->dfifoGroup = NULL;
        }

        BAPE_LoopbackGroup_P_Destroy(hOutputCapture->loopbackGroup);
        hOutputCapture->loopbackGroup = NULL;
    }

    hOutputCapture->enabled = false;
}

#if BAPE_CHIP_MAX_FS > 0
static void BAPE_OutputCapture_P_SetFs(BAPE_OutputPort output, unsigned fsNum)
{
    BAPE_OutputCaptureHandle hOutputCapture;
    BAPE_LoopbackGroupSettings loopbackSettings;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    hOutputCapture = output->pHandle;
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);

    hOutputCapture->fs = fsNum;    

    /* Update timing source in loopback */
    if ( hOutputCapture->loopbackGroup )
    {
        BAPE_LoopbackGroup_P_GetSettings(hOutputCapture->loopbackGroup, &loopbackSettings);
        loopbackSettings.fs = fsNum;
        errCode = BAPE_LoopbackGroup_P_SetSettings(hOutputCapture->loopbackGroup, &loopbackSettings);
        BDBG_ASSERT(errCode == BERR_SUCCESS);
    }
}
#else
static void BAPE_OutputCapture_P_SetMclk_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio)
{
    BAPE_OutputCaptureHandle hOutputCapture;
    BAPE_LoopbackGroupSettings loopbackSettings;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    hOutputCapture = output->pHandle;
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);

    if ( hOutputCapture->loopbackGroup )
    {
        /* Set Input Parameters */
        BAPE_LoopbackGroup_P_GetSettings(hOutputCapture->loopbackGroup, &loopbackSettings);
        loopbackSettings.mclkSource = mclkSource;
        loopbackSettings.pllChannel = pllChannel;
        loopbackSettings.mclkFreqToFsRatio = mclkFreqToFsRatio;
        errCode = BAPE_LoopbackGroup_P_SetSettings(hOutputCapture->loopbackGroup, &loopbackSettings);
        BDBG_ASSERT(BERR_SUCCESS == errCode);
    }

    hOutputCapture->mclkSource = mclkSource;
    hOutputCapture->pllChannel = pllChannel;
    hOutputCapture->mclkFreqToFsRatio = mclkFreqToFsRatio;
}
#endif

static void BAPE_OutputCapture_P_SetTimingParams_isr(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase)
{
    BAPE_OutputCaptureHandle hOutputCapture;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    hOutputCapture = output->pHandle;
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);
    BSTD_UNUSED(timebase);

    if ( hOutputCapture->sampleRate != sampleRate )
    {
        hOutputCapture->sampleRate = sampleRate;
        if ( hOutputCapture->interrupts.sampleRate.pCallback_isr )
        {
            hOutputCapture->interrupts.sampleRate.pCallback_isr(hOutputCapture->interrupts.sampleRate.pParam1,
                                                        hOutputCapture->interrupts.sampleRate.param2,
                                                        sampleRate);
        }
    }
}

static void BAPE_OutputCapture_P_ClearInterrupts_isr(BAPE_OutputCaptureHandle hOutputCapture)
{
    BDBG_OBJECT_ASSERT(hOutputCapture, BAPE_OutputCapture);

    if ( hOutputCapture->enabled )
    {
        BDBG_ASSERT(NULL != hOutputCapture->dfifoGroup);
        BAPE_DfifoGroup_P_RearmFullmarkInterrupt_isr(hOutputCapture->dfifoGroup);
        BAPE_DfifoGroup_P_RearmOverflowInterrupt_isr(hOutputCapture->dfifoGroup);
    }
}


