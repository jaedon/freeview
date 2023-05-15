/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape.c $
 * $brcm_Revision: Hydra_Software_Devel/42 $
 * $brcm_Date: 10/9/12 11:05p $
 *
 * Module Description: Audio PI Device Level Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape.c $
 * 
 * Hydra_Software_Devel/42   10/9/12 11:05p jgarrett
 * SW7425-3831: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-3831/2   10/9/12 5:41a jgarrett
 * SW7425-3831: Merge latest updates
 * 
 * Hydra_Software_Devel/SW7425-3831/1   9/14/12 6:12p jgarrett
 * SW7425-3831: Adding programmable mixer output rate
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape.c $
 * 
 * Hydra_Software_Devel/42   10/9/12 11:05p jgarrett
 * SW7425-3831: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-3831/2   10/9/12 5:41a jgarrett
 * SW7425-3831: Merge latest updates
 * 
 * Hydra_Software_Devel/41   10/8/12 6:49a jgarrett
 * SW7425-4033: Adding standby handler for MAI output
 * 
 * Hydra_Software_Devel/40   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/5   5/3/12 3:45p jgarrett
 * SW7425-2075: Merging latest mainline changes
 * 
 * Hydra_Software_Devel/39   4/30/12 7:55a jgarrett
 * SW7425-2690: Fixing capabilities for EQ-enabled chipsets
 * 
 * Hydra_Software_Devel/38   4/27/12 2:18p jgarrett
 * SW7425-2690: Adding numVcxos to capabilities
 * 
 * Hydra_Software_Devel/37   4/27/12 1:37p jgarrett
 * SW7425-2802: Adding runtime capabilities routine
 * 
 * Hydra_Software_Devel/36   4/20/12 5:25p jgarrett
 * SW7231-774: Adding DSP mixer watchdog handling
 * 
 * Hydra_Software_Devel/SW7425-2075/4   4/13/12 5:46p gskerl
 * SW7425-2075: Added BAPE_DataSource_eFci to the list of valid sources
 * for the buffer pool capabilities struct.
 * 
 * Hydra_Software_Devel/SW7425-2075/3   4/9/12 2:04p jgarrett
 * SW7425-2075: Merging latest changes from main branch and enabling
 * 4x/16x OP Types
 * 
 * Hydra_Software_Devel/35   3/28/12 2:25p gskerl
 * SW7429-107: Corrected channel status bits for 97K and Not Indicated
 * cases.
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/6/12 3:51p jgarrett
 * SW7425-2075: Adding debug, fixing buffer logic
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/34   3/7/12 4:35p jgarrett
 * SW7435-24: Adding DSP index for decode and dsp mixer
 * 
 * Hydra_Software_Devel/33   11/14/11 3:42p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/3   10/26/11 12:44p jgarrett
 * SW7429-18: Merging latest changes from main branch
 * 
 * Hydra_Software_Devel/SW7429-18/2   10/25/11 5:34p jgarrett
 * SW7429-18: Adding 7429 reset logic
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/32   10/26/11 5:54p gskerl
 * SW7231-129: Added BAPE_P_StandbyFmmHw() to get APE hardware ready to go
 * to Standby mode.
 * 
 * Hydra_Software_Devel/31   10/25/11 1:32p gskerl
 * SW7231-129: Added power standby/resume support for NCOs.
 * 
 * Hydra_Software_Devel/30   10/24/11 2:46p gskerl
 * SW7231-129: Added power standby/resume support for PLLs, SPDIF inputs,
 * and MAI inputs.
 * 
 * Hydra_Software_Devel/29   10/6/11 2:39p gskerl
 * SW7231-129: Added calls to BAPE_Xxxxx_ResumeFromStandby().
 * 
 * Hydra_Software_Devel/28   10/2/11 2:43p gskerl
 * SW7231-129: Added power standby/resume support for AudioReturnChannel
 * outputs
 * 
 * Hydra_Software_Devel/27   9/30/11 11:53a gskerl
 * SW7231-129: Added power standby/resume support for LoopbackGroup,
 * DummysinkGroups, and DummyOutputs
 * 
 * Hydra_Software_Devel/26   9/28/11 5:39p gskerl
 * SW7231-129: In BAPE_P_ResumeFmmHw(), added call to
 * BAPE_I2sOutput_P_ResumeFromStandby()
 * 
 * Hydra_Software_Devel/25   9/26/11 2:43p gskerl
 * SW7231-129: For APE's global interrupts... destroy at Suspend, then
 * recreate at Resume.
 * 
 * Hydra_Software_Devel/24   9/26/11 10:38a gskerl
 * SW7231-129: Added AIO reset via SUN_TOP_CTRL_SW_RESET reg (for 35230)
 * 
 * Hydra_Software_Devel/23   9/23/11 5:23p gskerl
 * SW7231-129: Added AIO reset via SUN_TOP_CTRL_SW_INIT reg
 * 
 * Hydra_Software_Devel/22   9/20/11 11:10a gskerl
 * SW7231-129: Resolved compiler warnings.
 * 
 * Hydra_Software_Devel/21   9/19/11 5:30p gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume. Only works for DACs so far.
 * 
 * Hydra_Software_Devel/20   9/16/11 6:48p gskerl
 * SW7231-129: Refactored to put hardware and software initialization into
 * separate functions.
 * 
 * Hydra_Software_Devel/19   7/8/11 6:38p jgarrett
 * SWDTV-6760: Adding I2sMultiOutput
 * 
 * Hydra_Software_Devel/18   6/24/11 4:40p gskerl
 * SW7231-128: Added BCHP_PWR_RESOURCE_AUD_PLL2 power resource
 * 
 * Hydra_Software_Devel/17   5/23/11 12:48p gskerl
 * SW7231-128: Changed tabs to spaces
 * 
 * Hydra_Software_Devel/16   5/20/11 7:24p gskerl
 * SW7231-128: Added DAC power control
 * 
 * Hydra_Software_Devel/15   5/18/11 2:31p gskerl
 * SW7231-128: Added BCHP_PWR support to APE
 * 
 * Hydra_Software_Devel/SW7231-128/3   5/16/11 2:29p gmohile
 * SW7231-128 : Add Power Management Support
 * 
 * Hydra_Software_Devel/SW7231-128/2   5/13/11 5:14p gmohile
 * SW7231-128 : Add Standby support
 * 
 * Hydra_Software_Devel/SW7231-128/1   5/12/11 1:38p gmohile
 * SW7231-128 : Add power management support
 * 
 * Hydra_Software_Devel/14   5/3/11 6:59p gskerl
 * SW7422-354: First attempt at adding support for the audio return
 * channel
 * 
 * Hydra_Software_Devel/13   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/12   4/11/11 5:54p jgarrett
 * SWDTV-6305: Adding ADC/RF Inputs for DTV
 * 
 * Hydra_Software_Devel/11   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/10   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/9   3/11/11 6:02p jgarrett
 * SW7422-146: Decoder supports external inputs
 * 
 * Hydra_Software_Devel/8   3/10/11 7:03p jgarrett
 * SW7422-146: Refactored DFIFO code, added support for input capture from
 * compressed/multichannel
 * 
 * Hydra_Software_Devel/7   2/28/11 5:02p jgarrett
 * SW7422-146: Adding watchdog handling
 * 
 * Hydra_Software_Devel/6   1/28/11 11:45a jgarrett
 * SW7422-146: Fixing warning
 * 
 * Hydra_Software_Devel/5   1/19/11 2:58p jgarrett
 * SW7422-146: Initial decode/passthrough of ac3
 * 
 * Hydra_Software_Devel/4   1/13/11 2:26p jgarrett
 * SW7422-146: Adding 7346 APE support
 * 
 * Hydra_Software_Devel/3   1/6/11 2:33p jgarrett
 * SW7422-146: Adding initial input capture API
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
#include "bchp_aud_fmm_bf_ctrl.h"
#include "bchp_sun_top_ctrl.h"
#include "bdsp_raaga.h"

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

BDBG_MODULE(bape);

BDBG_OBJECT_ID(BAPE_Device);
BDBG_OBJECT_ID(BAPE_BufferNode);

static BERR_Code BAPE_P_InitFmmSw(BAPE_Handle handle);
static BERR_Code BAPE_P_InitFmmHw(BAPE_Handle handle);

void BAPE_GetDefaultSettings(
    BAPE_Settings *pSettings    /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->maxDspTasks = BAPE_CHIP_MAX_DSP_TASKS;
    pSettings->rampPcmSamples = true;
    pSettings->maxIndependentDelay = 500;
    pSettings->maxPcmSampleRate = 48000;
    pSettings->numPcmBuffers = BAPE_CHIP_DEFAULT_NUM_PCM_BUFFERS;
    pSettings->numCompressedBuffers = BAPE_CHIP_DEFAULT_NUM_COMPRESSED_BUFFERS;
    pSettings->numCompressed4xBuffers = BAPE_CHIP_DEFAULT_NUM_COMPRESSED_4X_BUFFERS;
    pSettings->numCompressed16xBuffers = BAPE_CHIP_DEFAULT_NUM_COMPRESSED_16X_BUFFERS;
    pSettings->numRfEncodedPcmBuffers = 0;
}

BERR_Code BAPE_Open(
    BAPE_Handle *pHandle,   /* [out] returned handle */
    BCHP_Handle chpHandle,
    BREG_Handle regHandle,
    BMEM_Handle memHandle,
    BINT_Handle intHandle,
    BTMR_Handle tmrHandle,
    BDSP_Handle dspHandle,
    const BAPE_Settings *pSettings  /* NULL will use default settings */
    )
{
    BAPE_Settings defaultSettings;
    BAPE_Handle handle;
    BERR_Code errCode;
    BAPE_BufferNode *pNode;
    unsigned bufferSize, numBuffers;
    unsigned i, buffer;

    BDBG_ASSERT(NULL != pHandle);
    BDBG_ASSERT(NULL != chpHandle);
    BDBG_ASSERT(NULL != regHandle);
    BDBG_ASSERT(NULL != memHandle);
    BDBG_ASSERT(NULL != intHandle);
    BDBG_ASSERT(NULL != tmrHandle);
    
    if ( NULL == pSettings )
    {
        BAPE_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* Allocate device structure */
    handle = BKNI_Malloc(sizeof(BAPE_Device));
    if ( NULL == handle )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_handle;
    }
    /* Initialize structure */    
    BKNI_Memset(handle, 0, sizeof(BAPE_Device));
    BDBG_OBJECT_SET(handle, BAPE_Device);
    handle->chpHandle = chpHandle;
    handle->regHandle = regHandle;
    handle->memHandle = memHandle;
    handle->intHandle = intHandle;
    handle->tmrHandle = tmrHandle;
    handle->dspHandle = dspHandle;
    handle->settings = *pSettings;
    BLST_S_INIT(&handle->mixerList);

    if ( false == pSettings->rampPcmSamples )
    {
        /* Should only be used for test purposes.  */
        BDBG_WRN(("PCM Sample Ramping is disabled in SRC.  This should only be done for test purposes."));
    }

#ifdef BCHP_PWR_RESOURCE_AUD_AIO
    BCHP_PWR_AcquireResource(chpHandle, BCHP_PWR_RESOURCE_AUD_AIO);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL0
    BCHP_PWR_AcquireResource(chpHandle, BCHP_PWR_RESOURCE_AUD_PLL0);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL1
    BCHP_PWR_AcquireResource(chpHandle, BCHP_PWR_RESOURCE_AUD_PLL1);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL2
    BCHP_PWR_AcquireResource(chpHandle, BCHP_PWR_RESOURCE_AUD_PLL2);
#endif

    errCode = BAPE_P_InitFmmSw(handle);
    if ( errCode )
    {
        goto err_fmm;
    }

    errCode = BAPE_P_InitFmmHw(handle);
    if ( errCode )
    {
        goto err_fmm;
    }

    errCode = BAPE_P_InitInterrupts(handle);
    if ( errCode )
    {
        goto err_interrupt;
    }

    /* Buffer Allocations */
    for ( i = 0; i < BAPE_MAX_BUFFER_POOLS; i++ )
    {
        static const BAPE_DataType validTypes[BAPE_MAX_BUFFER_POOLS][BAPE_DataType_eMax] = 
        {
            {BAPE_DataType_ePcmStereo, BAPE_DataType_ePcm5_1, BAPE_DataType_ePcm7_1, BAPE_DataType_eMax},
            {BAPE_DataType_eIec61937, BAPE_DataType_eMax},
            {BAPE_DataType_eIec61937, BAPE_DataType_eIec61937x4, BAPE_DataType_ePcmRf, BAPE_DataType_eMax},
            {BAPE_DataType_eIec61937, BAPE_DataType_eIec61937x4, BAPE_DataType_ePcmRf, BAPE_DataType_eIec61937x16, BAPE_DataType_eMax}
        };
        static const BAPE_DataSource validSources[] = 
        {
            BAPE_DataSource_eDspBuffer, BAPE_DataSource_eHostBuffer, BAPE_DataSource_eDfifo, BAPE_DataSource_eFci, BAPE_DataSource_eMax
        };
        BLST_S_INIT(&handle->buffers[i].freeList);
        BLST_S_INIT(&handle->buffers[i].allocatedList);
        BAPE_FMT_P_InitCapabilities(&handle->buffers[i].capabilities, validSources, &validTypes[i][0]);
        bufferSize = pSettings->maxIndependentDelay > 0 ? BDSP_AF_P_DELAY_RBUF_SIZE : BDSP_AF_P_NON_DELAY_RBUF_SIZE;/*((BAPE_CHIP_MAX_PATH_DELAY + pSettings->maxIndependentDelay) * (pSettings->maxPcmSampleRate/1000)) * BAPE_CHIP_BYTES_PER_PCM_SAMPLE_PAIR;*/
        /* There are four classes of buffer pools.  PCM buffers, compressed 1x, compressed 4x/RF, compressed 16x. */
        switch ( i )
        {
        default:
        case 0:
            bufferSize = 2*bufferSize;      /* One buffer for 32-bit L samples, one for 32-bit R samples */
            if ( pSettings->maxPcmSampleRate > 48000 && pSettings->maxPcmSampleRate <= 96000 )
            {
                bufferSize *= 2;
            }
            else if ( pSettings->maxPcmSampleRate > 96000 )
            {
                bufferSize *= 4;
            }
            numBuffers = pSettings->numPcmBuffers;
            BDBG_MSG(("Allocating %u PCM buffers (size %u bytes)", numBuffers, bufferSize));
            break;
        case 1:
            bufferSize = bufferSize;        /* One buffer for 16-bit interleaved L/R sampeles */
            numBuffers = pSettings->numCompressedBuffers;
            BDBG_MSG(("Allocating %u compressed buffers (size %u bytes)", numBuffers, bufferSize));
            break;
        case 2:
            bufferSize = 4*bufferSize;      /* One buffer for 16-bit interleaved L/R sampeles (4x samplerate) */
            numBuffers = pSettings->numCompressed4xBuffers + pSettings->numRfEncodedPcmBuffers;
            BDBG_MSG(("Allocating %u compressed 4x / PCM RF buffers (size %u bytes)", numBuffers, bufferSize));
            break;
        case 3:
             bufferSize = 16*bufferSize;    /* One buffer for 16-bit interleaved L/R sampeles (16x samplerate) */
             numBuffers = pSettings->numCompressed16xBuffers;
             BDBG_MSG(("Allocating %u compressed 16x / HBR buffers (size %u bytes)", numBuffers, bufferSize));
             break;
        }
        handle->buffers[i].bufferSize = bufferSize;
        handle->buffers[i].numFreeBuffers = numBuffers;

        for ( buffer = 0; buffer < numBuffers; buffer++ )
    {
        pNode = BKNI_Malloc(sizeof(BAPE_BufferNode));
        if ( NULL == pNode )
        {
            errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto err_buffer;
        }
        pNode->pMemory = BMEM_AllocAligned(memHandle, bufferSize, 8, 0);
        if ( NULL == pNode->pMemory )
        {
            errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            BKNI_Free(pNode);
            goto err_buffer;
        }
        BMEM_ConvertAddressToOffset(memHandle, pNode->pMemory, &pNode->offset);
            BDBG_OBJECT_SET(pNode, BAPE_BufferNode);
            BDBG_MSG(("Created Buffer Node %#x in pool %u", pNode, i));
            BLST_S_INSERT_HEAD(&handle->buffers[i].freeList, pNode, node);
            BDBG_ASSERT(pNode == BLST_S_FIRST(&handle->buffers[i].freeList));
            pNode->bufferSize = bufferSize;
            pNode->allocated = false;
            pNode->poolIndex = i;
        }
    }

    if ( handle->dspHandle )
    {
        BDSP_ContextCreateSettings dspContextSettings;
        BDSP_Status dspStatus;

        /* Determine num dsps */
        BDSP_GetStatus(handle->dspHandle, &dspStatus);
        handle->numDsps = dspStatus.numDsp;

        /* Create DSP Context */
        BDSP_Context_GetDefaultCreateSettings(handle->dspHandle, BDSP_ContextType_eAudio, &dspContextSettings);
        dspContextSettings.maxTasks = pSettings->maxDspTasks;
        errCode = BDSP_Context_Create(handle->dspHandle, &dspContextSettings, &handle->dspContext);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_context;
        }
    }
    
    handle->bStandby = false;

    /* Success */
    *pHandle = handle;

    return BERR_SUCCESS;

err_context:
err_buffer:
    /* Remove and free all buffers and nodes */
    for ( i = 0; i < BAPE_MAX_BUFFER_POOLS; i++ )
    {
        while ( (pNode = BLST_S_FIRST(&handle->buffers[i].freeList)) )
        {
            BLST_S_REMOVE_HEAD(&handle->buffers[i].freeList, node);
            BDBG_OBJECT_ASSERT(pNode, BAPE_BufferNode);
            BMEM_Free(memHandle, pNode->pMemory);
            BDBG_OBJECT_DESTROY(pNode, BAPE_BufferNode);
            BKNI_Free(pNode);
        }
    }
    BAPE_P_UninitInterrupts(handle);
err_interrupt:    
err_fmm:
    BDBG_OBJECT_DESTROY(handle, BAPE_Device);
    BKNI_Free(handle);
err_handle:
    *pHandle = NULL;

#ifdef BCHP_PWR_RESOURCE_AUD_PLL0
    BCHP_PWR_ReleaseResource(chpHandle, BCHP_PWR_RESOURCE_AUD_PLL0);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL1
    BCHP_PWR_ReleaseResource(chpHandle, BCHP_PWR_RESOURCE_AUD_PLL1);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL2
    BCHP_PWR_ReleaseResource(chpHandle, BCHP_PWR_RESOURCE_AUD_PLL2);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_AIO
    BCHP_PWR_ReleaseResource(chpHandle, BCHP_PWR_RESOURCE_AUD_AIO);
#endif
    return errCode;
}

void BAPE_Close(
    BAPE_Handle handle
    )
{
    unsigned i=0;
    BAPE_BufferNode *pNode;
    BAPE_MixerHandle mixer;
    BAPE_MuxOutputHandle muxOutput;

    /* Stop all potential mixer inputs first */
#if BAPE_CHIP_MAX_DECODERS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_DECODERS; i++ )
    {
        if ( handle->decoders[i] )
        {
            BDBG_MSG(("Stopping decoder %p (%d)", handle->decoders[i], i));
            BAPE_Decoder_Stop(handle->decoders[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_INPUT_CAPTURES > 0
    for ( i = 0; i < BAPE_CHIP_MAX_INPUT_CAPTURES; i++ )
    {
        if ( handle->inputCaptures[i] )
        {
            BDBG_MSG(("Stopping input capture %p (%d)", handle->inputCaptures[i], i));
            BAPE_InputCapture_Stop(handle->inputCaptures[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_PLAYBACKS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_PLAYBACKS; i++ )
    {
        if ( handle->playbacks[i] )
        {
            BDBG_MSG(("Stopping playback %p (%d)", handle->playbacks[i], i));
            BAPE_Playback_Stop(handle->playbacks[i]);
        }
    }
#endif

    /* Close all mixers next */
    while ( (mixer=BLST_S_FIRST(&handle->mixerList)) )
    {
        BDBG_MSG(("Destroying mixer %p)", mixer));
        BAPE_Mixer_Destroy(mixer);
    }

    /* Close all MuxOutputs */
    while ( (muxOutput=BLST_S_FIRST(&handle->muxOutputList)) )
    {
        BDBG_MSG(("Destroying muxOutput %p", muxOutput));
        BAPE_MuxOutput_Destroy(muxOutput);
    }

    /* Close all inputs */
#if BAPE_CHIP_MAX_DECODERS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_DECODERS; i++ )
    {
        if ( handle->decoders[i] )
        {
            BDBG_MSG(("Closing decoder %p (%d)", handle->decoders[i], i));
            BAPE_Decoder_Close(handle->decoders[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_INPUT_CAPTURES > 0
    for ( i = 0; i < BAPE_CHIP_MAX_INPUT_CAPTURES; i++ )
    {
        if ( handle->inputCaptures[i] )
        {
            BDBG_MSG(("Closing input capture %p (%d)", handle->inputCaptures[i], i));
            BAPE_InputCapture_Close(handle->inputCaptures[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_PLAYBACKS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_PLAYBACKS; i++ )
    {
        if ( handle->playbacks[i] )
        {
            BDBG_MSG(("Closing playback %p (%d)", handle->playbacks[i], i));
            BAPE_Playback_Close(handle->playbacks[i]);
        }
    }
#endif

    /* Close all input and output ports */
#if BAPE_CHIP_MAX_DACS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_DACS; i++ )
    {
        if ( handle->dacs[i] )
        {
            BDBG_MSG(("Closing DAC %p (%d)", handle->dacs[i], i));
            BAPE_Dac_Close(handle->dacs[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_I2S_OUTPUTS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_I2S_OUTPUTS; i++ )
    {
        if ( handle->i2sOutputs[i] )
        {
            BDBG_MSG(("Closing I2S Output %p (%d)", handle->i2sOutputs[i], i));
            BAPE_I2sOutput_Close(handle->i2sOutputs[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_I2S_MULTI_OUTPUTS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_I2S_MULTI_OUTPUTS; i++ )
    {
        if ( handle->i2sMultiOutputs[i] )
        {
            BDBG_MSG(("Closing I2S Multi Output %p (%d)", handle->i2sMultiOutputs[i], i));
            BAPE_I2sMultiOutput_Close(handle->i2sMultiOutputs[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_SPDIF_OUTPUTS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_SPDIF_OUTPUTS; i++ )
    {
        if ( handle->spdifOutputs[i] )
        {
            BDBG_MSG(("Closing SPDIF Output %p (%d)", handle->spdifOutputs[i], i));
            BAPE_SpdifOutput_Close(handle->spdifOutputs[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_MAI_OUTPUTS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_MAI_OUTPUTS; i++ )
    {
        if ( handle->maiOutputs[i] )
        {
            BDBG_MSG(("Closing MAI Output %p (%d)", handle->maiOutputs[i], i));
            BAPE_MaiOutput_Close(handle->maiOutputs[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_OUTPUT_CAPTURES > 0
    for ( i = 0; i < BAPE_CHIP_MAX_OUTPUT_CAPTURES; i++ )
    {
        if ( handle->outputCaptures[i] )
        {
            BDBG_MSG(("Closing Output Capture %p (%d)", handle->outputCaptures[i], i));
            BAPE_OutputCapture_Close(handle->outputCaptures[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_DUMMYSINKS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_DUMMYSINKS; i++ )
    {
        if ( handle->dummyOutputs[i] )
        {
            BDBG_MSG(("Closing Dummy Output %p (%d)", handle->dummyOutputs[i], i));
            BAPE_DummyOutput_Close(handle->dummyOutputs[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_I2S_INPUTS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_I2S_INPUTS; i++ )
    {
        if ( handle->i2sInputs[i] )
        {
            BDBG_MSG(("Closing I2S Input %p (%d)", handle->i2sInputs[i], i));
            BAPE_I2sInput_Close(handle->i2sInputs[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_SPDIF_INPUTS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_SPDIF_INPUTS; i++ )
    {
        if ( handle->spdifInputs[i] )
        {
            BDBG_MSG(("Closing SPDIF Input %p (%d)", handle->spdifInputs[i], i));
            BAPE_SpdifInput_Close(handle->spdifInputs[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_MAI_INPUTS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_MAI_INPUTS; i++ )
    {
        if ( handle->maiInputs[i] )
        {
            BDBG_MSG(("Closing MAI Input %p (%d)", handle->maiInputs[i], i));
            BAPE_MaiInput_Close(handle->maiInputs[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_AUDIO_RETURN_CHANNELS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_AUDIO_RETURN_CHANNELS; i++ )
    {
        if ( handle->audioReturnChannels[i] )
        {
            BDBG_MSG(("Closing Audio Return Channel %p (%d)", handle->audioReturnChannels[i], i));
            BAPE_AudioReturnChannel_Close(handle->audioReturnChannels[i]);
        }
    }
#endif
#if BAPE_CHIP_MAX_RFMODS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_RFMODS; i++ )
    {
        if ( handle->rfmods[i] )
        {
            BDBG_MSG(("Closing RFMOD %p (%d)", handle->rfmods[i], i));
            BAPE_RfMod_Close(handle->rfmods[i]);
        }
    }
#endif

    if ( handle->dspContext )
    {
        BDSP_Context_Destroy(handle->dspContext);
        handle->dspContext = NULL;
    }

    for ( i = 0; i < BAPE_MAX_BUFFER_POOLS; i++ )
    {
        while ( (pNode = BLST_S_FIRST(&handle->buffers[i].freeList)) )
        {
            BDBG_MSG(("Destroy Free Buffer node %#x",pNode));
            BLST_S_REMOVE_HEAD(&handle->buffers[i].freeList, node);
            BDBG_OBJECT_ASSERT(pNode, BAPE_BufferNode);
            BMEM_Free(handle->memHandle, pNode->pMemory);
            BDBG_OBJECT_DESTROY(pNode, BAPE_BufferNode);
            BKNI_Free(pNode);
        }
        while ( (pNode = BLST_S_FIRST(&handle->buffers[i].allocatedList)) )
        {
            BDBG_MSG(("Destroy Allocated Buffer node %#x",pNode));
            BLST_S_REMOVE_HEAD(&handle->buffers[i].allocatedList, node);
            BDBG_OBJECT_ASSERT(pNode, BAPE_BufferNode);
            BMEM_Free(handle->memHandle, pNode->pMemory);
            BDBG_OBJECT_DESTROY(pNode, BAPE_BufferNode);
            BKNI_Free(pNode);
        }
    }

    BAPE_P_UninitIopSw(handle);
    BAPE_P_UninitSrcSw(handle);
    BAPE_P_UninitDpSw(handle);
    BAPE_P_UninitBfSw(handle);
    BAPE_P_UninitInterrupts(handle);

#ifdef BCHP_PWR_RESOURCE_AUD_PLL0
    BCHP_PWR_ReleaseResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_PLL0);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL1
    BCHP_PWR_ReleaseResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_PLL1);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL2
    BCHP_PWR_ReleaseResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_PLL2);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_AIO
    BCHP_PWR_ReleaseResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_AIO);
#endif

    BDBG_OBJECT_DESTROY(handle, BAPE_Device);
    BKNI_Free(handle);
}

unsigned BAPE_P_GetSampleRateCstatCode(unsigned sampleRate)
{
    switch ( sampleRate )
    {
    case 32000:      /* 32K Sample rate */
        return 0x3;
    case 44100:    /* 44.1K Sample rate */
        return 0x0;
    case 48000:      /* 48K Sample rate */
        return 0x2;
    case 96000:      /* 96K Sample rate */
        return 0xa;
    case 22050:   /* 22.05K Sample rate */
        return 0x4;
    case 24000:      /* 24K Sample rate */
        return 0x6;
    case 88200:    /* 88.2K Sample rate */
        return 0x8;
    case 176400:   /* 176.4K Sample rate */
        return 0xc;
    case 192000:     /* 192K Sample rate */
        return 0xe;
    case 768000:     /* 768K Sample rate */
        return 0x9;
    default:
        return 0x1; /* not indicated */
    }
}

/**************************************************************************/

static BERR_Code BAPE_P_InitFmmSw(BAPE_Handle handle)
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

    errCode = BAPE_P_InitBfSw(handle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_init_bf;
    }

    errCode = BAPE_P_InitDpSw(handle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_init_dp;
    }

    BDBG_MSG(("Initializing SRC registers"));
    errCode = BAPE_P_InitSrcSw(handle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_init_src;
    }

    errCode = BAPE_P_InitIopSw(handle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_init_iop;
    }

    return BERR_SUCCESS;

err_init_iop:
    BAPE_P_UninitSrcSw(handle);
err_init_src:
    BAPE_P_UninitDpSw(handle);
err_init_dp:
    BAPE_P_UninitBfSw(handle);
err_init_bf:
    return BERR_TRACE(errCode);
}


static BERR_Code BAPE_P_InitFmmHw(BAPE_Handle handle)
{
    BREG_Handle regHandle;
    uint32_t regVal;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

    regHandle = handle->regHandle;

    #ifdef BCHP_SUN_TOP_CTRL_SW_RESET
        /* Note, older SW_RESET registers DO need read-modify-write (atomic update) */
        BREG_AtomicUpdate32_isr(regHandle, BCHP_SUN_TOP_CTRL_SW_RESET, 
                0,                                                  /* Clear these bits */
                BCHP_MASK( SUN_TOP_CTRL_SW_RESET, aio_sw_reset));   /* Set these bits   */

        BREG_AtomicUpdate32_isr(regHandle, BCHP_SUN_TOP_CTRL_SW_RESET,
                BCHP_MASK( SUN_TOP_CTRL_SW_RESET, aio_sw_reset),    /* Clear these bits */
                0);                                                 /* Set these bits   */
    #else
        /* Note, newer SW_INIT set/clear registers DON'T need read-modify-write. */
        BREG_Write32(regHandle, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,
              BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, aio_sw_init, 1 ));

        /* Now clear the reset. */
        BREG_Write32(regHandle, BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,
              BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, aio_sw_init, 1 ));
    #endif


    BDBG_MSG(("Resetting FMM"));
#ifdef BCHP_AUD_FMM_MISC_RESET
    /* Assert toplevel reset */
    BREG_Write32(regHandle, BCHP_AUD_FMM_MISC_RESET, 0);
    regVal = BREG_Read32(regHandle, BCHP_AUD_FMM_MISC_RESET);
    regVal |= 
            (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_TOP_LOGIC_B, Inactive))
    #ifdef BCHP_AUD_FMM_MISC_RESET_RESET_SPDIFRX_LOGIC_B_Inactive
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_HDMIRX_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_HDMIRX_REGS_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SPDIFRX_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SPDIFRX_REGS_B, Inactive))      
    #else 
        #ifdef BCHP_AUD_FMM_MISC_RESET_RESET_SPDIFRX_0_LOGIC_B_Inactive
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SPDIFRX_0_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SPDIFRX_0_REGS_B, Inactive)) 
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SPDIFRX_1_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SPDIFRX_1_REGS_B, Inactive))
        #endif 
    #endif

          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_OP_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_OP_REGS_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_MS_PROC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_MS_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_MS_REGS_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SRC_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SRC_REGS_B, Inactive))
    #ifdef BCHP_AUD_FMM_MISC_RESET_RESET_ADC_CIC_REGS_B_Inactive
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_ADC_CIC_REGS_B, Inactive))
    #endif      
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_DP_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_DP_REGS_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_BF_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_BF_REGS_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_TOP_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_OP_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_OP_REGS_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_MS_PROC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_MS_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_MS_REGS_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SRC_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_SRC_REGS_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_DP_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_DP_REGS_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_BF_LOGIC_B, Inactive))
          | (BCHP_FIELD_ENUM (AUD_FMM_MISC_RESET, RESET_BF_REGS_B, Inactive));
    BREG_Write32(regHandle, BCHP_AUD_FMM_MISC_RESET, regVal);

    /* Powerup the FMM Modules */        
    regVal = BREG_Read32(regHandle, BCHP_AIO_MISC_PWRDOWN);

#if BCHP_AIO_MISC_PWRDOWN_ADC_RESET_Reset
    /* Don't bother with DAC-related fields... they're handled by BAPE_P_InitDacs */
    regVal &= ~((BCHP_MASK (AIO_MISC_PWRDOWN, ADC_IDDQ_PWRUP)) |
                (BCHP_MASK (AIO_MISC_PWRDOWN, ADC_REF_PWRUP)) |
                (BCHP_MASK (AIO_MISC_PWRDOWN, ADC_R_PWRUP)) |
                (BCHP_MASK (AIO_MISC_PWRDOWN, ADC_L_PWRUP)) |
                (BCHP_MASK (AIO_MISC_PWRDOWN, ADC_RESET)) |
                (BCHP_MASK (AIO_MISC_PWRDOWN, ADC_PWRUP_CORE)) |
                (BCHP_MASK (AIO_MISC_PWRDOWN, SPDIF_RX0)));

    regVal |= ((BCHP_FIELD_DATA (AIO_MISC_PWRDOWN, ADC_IDDQ_PWRUP, 1)) |
               (BCHP_FIELD_DATA (AIO_MISC_PWRDOWN, ADC_REF_PWRUP, 1)) |
               (BCHP_FIELD_DATA (AIO_MISC_PWRDOWN, ADC_R_PWRUP, 1)) |
               (BCHP_FIELD_DATA (AIO_MISC_PWRDOWN, ADC_L_PWRUP, 1)) |
               (BCHP_FIELD_ENUM (AIO_MISC_PWRDOWN, ADC_RESET, Normal)) |
               (BCHP_FIELD_ENUM (AIO_MISC_PWRDOWN, ADC_PWRUP_CORE, Normal)) |
               (BCHP_FIELD_ENUM (AIO_MISC_PWRDOWN, SPDIF_RX0, Normal)));    
#endif

#if BCHP_AIO_MISC_PWRDOWN_SPDIF_RX_Powerdown       
    regVal &= ~(BCHP_MASK (AIO_MISC_PWRDOWN, SPDIF_RX));
    regVal |= (BCHP_FIELD_ENUM (AIO_MISC_PWRDOWN, SPDIF_RX, Normal));  
#endif

    BREG_Write32(regHandle, BCHP_AIO_MISC_PWRDOWN, regVal);
#else
    /* Newer 7429-style chips have a single-bit reset for the entire audio block */
    regVal = BCHP_FIELD_ENUM(AUD_MISC_INIT, AUDIO_INIT, Init);
    BREG_Write32(regHandle, BCHP_AUD_MISC_INIT, regVal);
    (void)BREG_Read32(regHandle, BCHP_AUD_MISC_INIT);
    regVal = BCHP_FIELD_ENUM(AUD_MISC_INIT, AUDIO_INIT, Inactive);
    BREG_Write32(regHandle, BCHP_AUD_MISC_INIT, regVal);

    /* Powerup the FMM Modules */        
    regVal = BREG_Read32(regHandle, BCHP_AUD_MISC_PWRDOWN);

    #if BCHP_AUD_MISC_PWRDOWN_ADC_RESET_Reset
    /* Don't bother with DAC-related fields... they're handled by BAPE_P_InitDacs */
    regVal &= ~((BCHP_MASK (AUD_MISC_PWRDOWN, ADC_IDDQ_PWRUP)) |
                (BCHP_MASK (AUD_MISC_PWRDOWN, ADC_REF_PWRUP)) |
                (BCHP_MASK (AUD_MISC_PWRDOWN, ADC_R_PWRUP)) |
                (BCHP_MASK (AUD_MISC_PWRDOWN, ADC_L_PWRUP)) |
                (BCHP_MASK (AUD_MISC_PWRDOWN, ADC_RESET)) |
                (BCHP_MASK (AUD_MISC_PWRDOWN, ADC_PWRUP_CORE)) |
                (BCHP_MASK (AUD_MISC_PWRDOWN, SPDIF_RX0)));

    regVal |= ((BCHP_FIELD_DATA (AUD_MISC_PWRDOWN, ADC_IDDQ_PWRUP, 1)) |
               (BCHP_FIELD_DATA (AUD_MISC_PWRDOWN, ADC_REF_PWRUP, 1)) |
               (BCHP_FIELD_DATA (AUD_MISC_PWRDOWN, ADC_R_PWRUP, 1)) |
               (BCHP_FIELD_DATA (AUD_MISC_PWRDOWN, ADC_L_PWRUP, 1)) |
               (BCHP_FIELD_ENUM (AUD_MISC_PWRDOWN, ADC_RESET, Normal)) |
               (BCHP_FIELD_ENUM (AUD_MISC_PWRDOWN, ADC_PWRUP_CORE, Normal)) |
               (BCHP_FIELD_ENUM (AUD_MISC_PWRDOWN, SPDIF_RX0, Normal)));    
    #endif

    #if BCHP_AUD_MISC_PWRDOWN_SPDIF_RX_Powerdown       
    regVal &= ~(BCHP_MASK (AUD_MISC_PWRDOWN, SPDIF_RX));
    regVal |= (BCHP_FIELD_ENUM (AUD_MISC_PWRDOWN, SPDIF_RX, Normal));  
    #endif

    BREG_Write32(regHandle, BCHP_AUD_MISC_PWRDOWN, regVal);
#endif

    errCode = BAPE_P_InitDacHw(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_P_InitBfHw(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_P_InitDpHw(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_P_InitSrcHw(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_P_InitIopHw(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    return BERR_SUCCESS;
}


void BAPE_GetInterruptHandlers(
    BAPE_Handle handle,
    BAPE_InterruptHandlers *pInterrupts     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(NULL != pInterrupts);
    *pInterrupts = handle->interrupts;
}

BERR_Code BAPE_SetInterruptHandlers(
    BAPE_Handle handle,
    const BAPE_InterruptHandlers *pInterrupts
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(NULL != pInterrupts);

    if ( NULL != handle->dspContext )
    {
        BERR_Code errCode;
        BDSP_ContextInterruptHandlers contextInterrupts;

        BDSP_Context_GetInterruptHandlers(handle->dspContext, &contextInterrupts);
        contextInterrupts.watchdog.pCallback_isr = pInterrupts->watchdog.pCallback_isr;
        contextInterrupts.watchdog.pParam1 = pInterrupts->watchdog.pParam1;
        contextInterrupts.watchdog.param2 = pInterrupts->watchdog.param2;
        errCode = BDSP_Context_SetInterruptHandlers(handle->dspContext, &contextInterrupts);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    handle->interrupts = *pInterrupts;

    return BERR_SUCCESS;
}

BERR_Code BAPE_ProcessWatchdogInterrupt(
    BAPE_Handle handle
    )
{
    BERR_Code errCode;
    unsigned i, numFound;
    BAPE_PathNode *pNode;
    BAPE_MixerHandle hMixer;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

    if ( handle->dspContext )
    {
        /* Stop all running decoders */
        for ( i = 0; i < BAPE_CHIP_MAX_DECODERS; i++ )
        {
            if ( handle->decoders[i] )
            {
                handle->decoderWatchdogInfo[i].state = handle->decoders[i]->state;
                if ( handle->decoderWatchdogInfo[i].state != BAPE_DecoderState_eStopped )
                {
                    handle->decoderWatchdogInfo[i].startSettings = handle->decoders[i]->startSettings;
                    BAPE_Decoder_Stop(handle->decoders[i]);
                }
            }
            else
            {
                handle->decoderWatchdogInfo[i].state = BAPE_DecoderState_eMax;
            }
        }
        /* If we have playback/inputcapture feeding DSP mixer, we need to stop that as well */
        for ( i = 0; i < BAPE_CHIP_MAX_PLAYBACKS; i++)
        {
            if ( handle->playbacks[i] && handle->playbacks[i]->running )
            {
                BAPE_PathNode_P_FindConsumersBySubtype(&handle->playbacks[i]->node, BAPE_PathNodeType_eMixer, BAPE_MixerType_eDsp, 1, &numFound, &pNode);
                if ( numFound > 0 )
                {
                    handle->playbackWatchdogInfo[i].restartRequired = true;
                    handle->playbackWatchdogInfo[i].startSettings = handle->playbacks[i]->startSettings;
                    BAPE_Playback_Stop(handle->playbacks[i]);
                }
            }
        }
        for ( i = 0; i < BAPE_CHIP_MAX_INPUT_CAPTURES; i++)
        {
            if ( handle->inputCaptures[i] && handle->inputCaptures[i]->running )
            {
                BAPE_PathNode_P_FindConsumersBySubtype(&handle->inputCaptures[i]->node, BAPE_PathNodeType_eMixer, BAPE_MixerType_eDsp, 1, &numFound, &pNode);
                if ( numFound > 0 )
                {
                    handle->inputCaptureWatchdogInfo[i].restartRequired = true;
                    handle->inputCaptureWatchdogInfo[i].startSettings = handle->inputCaptures[i]->startSettings;
                    BAPE_InputCapture_Stop(handle->inputCaptures[i]);
                }
            }
        }
        /* If we have any explicitly started DSP mixers, stop them now */
        for ( hMixer = BLST_S_FIRST(&handle->mixerList);
              hMixer != NULL;
              hMixer = BLST_S_NEXT(hMixer, node) )
        {
            if ( hMixer->settings.type == BAPE_MixerType_eDsp && hMixer->startedExplicitly )
            {
                hMixer->restartPending = true;
                BAPE_Mixer_Stop(hMixer);
            }
        }

        /* Reboot the DSP */
        errCode = BDSP_Context_ProcessWatchdogInterrupt(handle->dspContext);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        /* If we have any explicitly started DSP mixers, stop them now */
        for ( hMixer = BLST_S_FIRST(&handle->mixerList);
              hMixer != NULL;
              hMixer = BLST_S_NEXT(hMixer, node) )
        {
            if ( hMixer->restartPending )
            {
                hMixer->restartPending = false;
                errCode = BAPE_Mixer_Start(hMixer);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                }
            }
        }
        /* If we have playback/inputcapture feeding DSP mixer, we need to stop that as well */
        for ( i = 0; i < BAPE_CHIP_MAX_PLAYBACKS; i++)
        {
            if ( handle->playbackWatchdogInfo[i].restartRequired  )
            {
                handle->playbackWatchdogInfo[i].restartRequired = false;
                errCode = BAPE_Playback_Start(handle->playbacks[i], &handle->playbackWatchdogInfo[i].startSettings);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                }
            }
        }
        for ( i = 0; i < BAPE_CHIP_MAX_INPUT_CAPTURES; i++)
        {
            if ( handle->inputCaptureWatchdogInfo[i].restartRequired )
            {
                handle->inputCaptureWatchdogInfo[i].restartRequired = false;
                errCode = BAPE_InputCapture_Start(handle->inputCaptures[i], &handle->inputCaptureWatchdogInfo[i].startSettings);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                }
            }
        }
        /* Reset all decoder state */
        for ( i = 0; i < BAPE_CHIP_MAX_DECODERS; i++ )
        {
            if ( handle->decoders[i] )
            {
                if ( handle->decoderWatchdogInfo[i].state != BAPE_DecoderState_eStopped )
                {
                    /* Restart Decoder */
                    errCode = BAPE_Decoder_Start(handle->decoders[i], &handle->decoderWatchdogInfo[i].startSettings);
                    if ( errCode )
                    {
                        BDBG_ERR(("Error restarting decoder %d", i));
                        errCode = BERR_TRACE(errCode);
                    }

                    if ( handle->decoderWatchdogInfo[i].state == BAPE_DecoderState_ePaused )
                    {
                        errCode = BAPE_Decoder_Pause(handle->decoders[i]);
                        if ( errCode )
                        {
                            BDBG_ERR(("Error re-pausing decoder %d", i));
                            errCode = BERR_TRACE(errCode);
                        }
                    }
                }
            }
        }
    }

    return BERR_SUCCESS;
}

void BAPE_GetDecoderPathDelay(
    BAPE_Handle handle,
    unsigned *pDelay    /* [out] in ms */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(NULL != pDelay);
    *pDelay = BAPE_CHIP_MAX_PATH_DELAY;
}


#ifdef BCHP_PWR_SUPPORT
static BERR_Code BAPE_P_StandbyFmmHw(BAPE_Handle handle)
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

    /* Since all channels are stopped, we can relese the unused "path resources"
     * and that should release everything... That way, when we resume from standby,
     * then all of the resources will be reallocated (causing the corresponding 
     * hardware to be reconfigured to its proper state).
     */
    BAPE_P_ReleaseUnusedPathResources(handle);

    /* Disable and destroy the global APE interrupts... We'll recreate them at resume time. */
    BAPE_P_UninitInterrupts(handle);

    /* Now tell a few more things to get ready to power down into standby mode. */
    errCode = BAPE_SpdifInput_P_PrepareForStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_MaiInput_P_PrepareForStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_MaiOutput_P_PrepareForStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    return BERR_SUCCESS;
}
#endif /* BCHP_PWR_SUPPORT*/


#ifdef BCHP_PWR_SUPPORT
static BERR_Code BAPE_P_ResumeFmmHw(BAPE_Handle handle)
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

    errCode = BAPE_Nco_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_Pll_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_LoopbackGroup_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_DummysinkGroup_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_AudioReturnChannel_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_Dac_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_DummyOutput_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_I2sMultiOutput_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_I2sOutput_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_MaiOutput_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_RfMod_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_SpdifOutput_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_SpdifInput_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    errCode = BAPE_MaiInput_P_ResumeFromStandby(handle);
    if ( errCode ) return BERR_TRACE(errCode);

    return BERR_SUCCESS;
}
#endif /* BCHP_PWR_SUPPORT*/


BERR_Code BAPE_Standby(
    BAPE_Handle handle,                 /* [in] AP device handle */
    BAPE_StandbySettings *pSettings     /* [in] standby settings */
)
{
#ifdef BCHP_PWR_SUPPORT
    unsigned i;
#endif

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    
    BSTD_UNUSED(pSettings);

#ifdef BCHP_PWR_SUPPORT
    /* check that all channels have been stopped */
#if BAPE_CHIP_MAX_DECODERS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_DECODERS; i++ )
    {
        if ( handle->decoders[i] && handle->decoders[i]->state != BAPE_DecoderState_eStopped )
        {
            BDBG_ERR(("Decoder %p (%d) is not stopped", handle->decoders[i], i));
            return BERR_UNKNOWN;
        }
    }
#endif
#if BAPE_CHIP_MAX_INPUT_CAPTURES > 0
    for ( i = 0; i < BAPE_CHIP_MAX_INPUT_CAPTURES; i++ )
    {
        if ( handle->inputCaptures[i] && handle->inputCaptures[i]->running == true )
        {
            BDBG_ERR(("Input capture %p (%d) is not stopped", handle->inputCaptures[i], i));
            return BERR_UNKNOWN;
        }
    }
#endif
#if BAPE_CHIP_MAX_PLAYBACKS > 0
    for ( i = 0; i < BAPE_CHIP_MAX_PLAYBACKS; i++ )
    {
        if ( handle->playbacks[i] && handle->playbacks[i]->running == true)
        {
            BDBG_ERR(("Playback %p (%d) is not stopped", handle->playbacks[i], i)); 
            return BERR_UNKNOWN;
        }
    }
#endif        

    /* if we reach here, then no channels are active. we can power down */
    if (!handle->bStandby)
    {
        BERR_Code   errCode;

        handle->bStandby = true;

        /* Prepare to go into standby. */
        errCode = BAPE_P_StandbyFmmHw(handle);
        if ( errCode ) return BERR_TRACE(errCode);

#ifdef BCHP_PWR_RESOURCE_AUD_PLL0
        BCHP_PWR_ReleaseResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_PLL0);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL1
        BCHP_PWR_ReleaseResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_PLL1);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL2
        BCHP_PWR_ReleaseResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_PLL2);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_AIO
        BCHP_PWR_ReleaseResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_AIO);
#endif
    }
#endif

    return BERR_SUCCESS;    
}

BERR_Code BAPE_Resume(
    BAPE_Handle handle  /* [in] APE device handle */
)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

#ifdef BCHP_PWR_SUPPORT
    if (handle->bStandby) {
        handle->bStandby = false;
#ifdef BCHP_PWR_RESOURCE_AUD_AIO
        BCHP_PWR_AcquireResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_AIO);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL0
        BCHP_PWR_AcquireResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_PLL0);
#endif
#ifdef BCHP_PWR_RESOURCE_AUD_PLL1
        BCHP_PWR_AcquireResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_PLL1);
#endif   
#ifdef BCHP_PWR_RESOURCE_AUD_PLL2
        BCHP_PWR_AcquireResource(handle->chpHandle, BCHP_PWR_RESOURCE_AUD_PLL2);
#endif      
        {
            BERR_Code   errCode;

            /* Put the FMM hardware into the "initial" state... the state that 
             * BAPE_Open() leaves things in.
             */
            errCode = BAPE_P_InitFmmHw(handle);
            if ( errCode ) return BERR_TRACE(errCode);

            /* Recreate and enable the APE global interrupts. */
            errCode = BAPE_P_InitInterrupts(handle);
            if ( errCode ) return BERR_TRACE(errCode);
        
            /* Now bring inputs and outputs into their "open" (but "unstarted") state. */
            errCode = BAPE_P_ResumeFmmHw(handle);
            if ( errCode ) return BERR_TRACE(errCode);
        }
    }
#endif

    return BERR_SUCCESS;
}

void BAPE_GetCapabilities(
    BAPE_Handle hApe,
    BAPE_Capabilities *pCaps        /* [out] */
    )
{
    BDSP_Handle hDsp;
    unsigned i;

    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);
    BDBG_ASSERT(NULL != pCaps);

    BKNI_Memset(pCaps, 0, sizeof(BAPE_Capabilities));

    #ifdef BAPE_CHIP_MAX_I2S_INPUTS
    pCaps->numInputs.i2s = BAPE_CHIP_MAX_I2S_INPUTS;
    #endif
    #ifdef BAPE_CHIP_MAX_MAI_INPUTS
    pCaps->numInputs.mai = BAPE_CHIP_MAX_MAI_INPUTS;
    #endif
    #ifdef BAPE_CHIP_MAX_SPDIF_INPUTS
    pCaps->numInputs.spdif = BAPE_CHIP_MAX_SPDIF_INPUTS;
    #endif

    #ifdef BAPE_CHIP_MAX_AUDIO_RETURN_CHANNELS
    pCaps->numOutputs.audioReturnChannel = BAPE_CHIP_MAX_AUDIO_RETURN_CHANNELS;
    #endif
    #ifdef BAPE_CHIP_MAX_OUTPUT_CAPTURES
    pCaps->numOutputs.capture = BAPE_CHIP_MAX_OUTPUT_CAPTURES;
    #endif
    #ifdef BAPE_CHIP_MAX_DACS
    pCaps->numOutputs.dac = BAPE_CHIP_MAX_DACS;
    #endif
    #ifdef BAPE_CHIP_MAX_DUMMYSINKS
    pCaps->numOutputs.dummy = BAPE_CHIP_MAX_DUMMYSINKS;
    #endif
    #ifdef BAPE_CHIP_MAX_I2S_OUTPUTS
    pCaps->numOutputs.i2s = BAPE_CHIP_MAX_I2S_OUTPUTS;
    #endif
    #ifdef BAPE_CHIP_MAX_LOOPBACKS
    pCaps->numOutputs.loopback = BAPE_CHIP_MAX_LOOPBACKS;
    #endif
    #ifdef BAPE_CHIP_MAX_MAI_OUTPUTS
    pCaps->numOutputs.mai = BAPE_CHIP_MAX_MAI_OUTPUTS;
    #endif
    #ifdef BAPE_CHIP_MAX_RFMODS
    pCaps->numOutputs.rfmod = BAPE_CHIP_MAX_RFMODS;
    #endif
    #ifdef BAPE_CHIP_MAX_SPDIF_OUTPUTS
    pCaps->numOutputs.spdif = BAPE_CHIP_MAX_SPDIF_OUTPUTS;
    #endif

    #ifdef BAPE_CHIP_MAX_DECODERS
    pCaps->numDecoders = BAPE_CHIP_MAX_DECODERS;
    #endif
    #ifdef BAPE_CHIP_MAX_PLAYBACKS
    pCaps->numPlaybacks = BAPE_CHIP_MAX_PLAYBACKS;
    #endif
    #ifdef BAPE_CHIP_MAX_INPUT_CAPTURES
    pCaps->numInputCaptures = BAPE_CHIP_MAX_INPUT_CAPTURES;
    #endif
    #ifdef BAPE_CHIP_MAX_VCXOS
    pCaps->numVcxos = BAPE_CHIP_MAX_VCXOS;
    #endif
    #ifdef BAPE_CHIP_MAX_PLLS
    pCaps->numPlls = BAPE_CHIP_MAX_PLLS;
    #endif
    #ifdef BAPE_CHIP_MAX_NCOS
    pCaps->numNcos = BAPE_CHIP_MAX_NCOS;
    #endif

    hDsp = hApe->dspHandle;
    if ( hDsp )
    {
        BERR_Code errCode;
        BDSP_Status dspStatus;
        const BAPE_CodecAttributes *pAttributes;
        const BDSP_AudioTypeInfo *pDspTypeInfo;
        const BDSP_AudioProcessingInfo *pDspProcessingInfo;
        const BDSP_AudioEncodeInfo *pDspEncodeInfo;

        BDSP_GetStatus(hDsp, &dspStatus);
        pCaps->numDsps = dspStatus.numDsp;
        
        for ( i = 0; i < BAVC_AudioCompressionStd_eMax; i++ )        
        {
            pAttributes = BAPE_P_GetCodecAttributes(i);
            BDBG_ASSERT(NULL != pAttributes);
            errCode = BDSP_GetAudioTypeInfo(hDsp, pAttributes->dspAudioType, &pDspTypeInfo);
            if ( BERR_SUCCESS == errCode )
            {
                if ( pDspTypeInfo->supported )
                {
                    pCaps->dsp.codecs[i].decode = true;
                }
            }
            errCode = BDSP_GetAudioEncodeInfo(hDsp, pAttributes->dspAudioEncode, &pDspEncodeInfo);
            if ( BERR_SUCCESS == errCode )
            {
                if ( pDspEncodeInfo->supported )
                {
                    pCaps->dsp.codecs[i].encode = true;
                    pCaps->dsp.encoder = true;
                }
            }
        }

        for ( i = 0; i < BDSP_AudioProcessing_eMax; i++ )
        {
            errCode = BDSP_GetAudioProcessingInfo(hDsp, i, &pDspProcessingInfo);
            if ( BERR_SUCCESS == errCode && true == pDspProcessingInfo->supported )
            {
                switch ( i )
                {
                case BDSP_AudioProcessing_eAVL:
                    pCaps->dsp.autoVolumeLevel = true;
                    break;
                case BDSP_AudioProcessing_eDsola:
                    pCaps->dsp.decodeRateControl = true;
                    break;
                case BDSP_AudioProcessing_eSrsHd:
                    pCaps->dsp.truSurroundHd = true;
                    break;
                case BDSP_AudioProcessing_eSrsTruVolume:
                    pCaps->dsp.truVolume = true;
                    break;
                case BDSP_AudioProcessing_eAudysseyVolume:
                    pCaps->dsp.audysseyAdv = true;
                    break;
                case BDSP_AudioProcessing_eBrcm3DSurround:
                    pCaps->dsp._3dSurround = true;
                    break;
                case BDSP_AudioProcessing_eFWMixer:
                    pCaps->dsp.mixer = true;
                    break;
                case BDSP_AudioProcessing_eAudysseyABX:
                    pCaps->dsp.audysseyAbx = true;
                    break;
                case BDSP_AudioProcessing_eDdre:
                    pCaps->dsp.dolbyDigitalReencode = true;
                    break;
                case BDSP_AudioProcessing_eDv258:
                    pCaps->dsp.dolbyVolume = true;
                    break;
                case BDSP_AudioProcessing_eSrsCsdTd:
                case BDSP_AudioProcessing_eSrsEqHl:
                    pCaps->dsp.studioSound = true;
                    break;
                case BDSP_AudioProcessing_eGenCdbItb:
                    pCaps->dsp.muxOutput = true;
                    break;
                case BDSP_AudioProcessing_eBtscEncoder:
                    pCaps->dsp.rfEncoder.supported = true;
                    pCaps->dsp.rfEncoder.encodings[BAPE_RfAudioEncoding_eBtsc] = true;
                    break;
                case BDSP_AudioProcessing_eSpeexAec:
                    pCaps->dsp.echoCanceller.supported = true;
                    pCaps->dsp.echoCanceller.algorithms[BAPE_EchoCancellerAlgorithm_eSpeex] = true;
                    break;
                default:
                    break;
                }
            }
        }
    }

    #if BAPE_CHIP_SRC_TYPE_IS_IIR
    pCaps->equalizer.supported = true;
    for ( i = 0; i < BAPE_EqualizerStageType_eMax; i++ )
    {
        pCaps->equalizer.types[i] = true;
    }
    #endif
}

