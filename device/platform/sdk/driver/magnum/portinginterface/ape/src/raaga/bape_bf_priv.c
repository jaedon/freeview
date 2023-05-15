/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_bf_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 1/23/13 1:39p $
 *
 * Module Description: Destination FIFO Interfaces
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_bf_priv.c $
 * 
 * Hydra_Software_Devel/20   1/23/13 1:39p jgarrett
 * SW7425-3699: SW7425-4440: Adjusting adaptive rate controller workaround
 * to only use dfifo 0 and ignore sfifo's linked to a dfifo
 * 
 * Hydra_Software_Devel/19   11/16/12 4:55p jgarrett
 * SW7425-3699: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-3699/2   11/16/12 4:21p jgarrett
 * SW7425-3699: Cleaning up debug output
 * 
 * Hydra_Software_Devel/SW7425-3699/1   11/12/12 5:05p jgarrett
 * SW7425-3699: Programming DFIFOs to use first running SFIFO always for
 * test purposes
 * 
 * Hydra_Software_Devel/18   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/12/12 1:56p jgarrett
 * SW7425-2075: Fixing priority programming for SFIFO
 * 
 * Hydra_Software_Devel/17   3/2/12 3:03p jgarrett
 * SW7429-18: Clearing all rbuf registers on startup
 * 
 * Hydra_Software_Devel/16   11/30/11 6:49p jgarrett
 * SW7429-18: Fixing freemark re-arm for 7429
 * 
 * Hydra_Software_Devel/15   11/14/11 3:42p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/3   11/3/11 4:58p gskerl
 * SW7429-18: Fixed typo... #if -> #ifdef
 * 
 * Hydra_Software_Devel/SW7429-18/2   11/3/11 4:45p gskerl
 * SW7429-18: Added conditional includes for bchp_aud_fmm_bf_esr1_h.h and
 * bchp_aud_fmm_bf_esr2_h.h to fix compile errors on 7425
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/14   11/10/11 3:48p jgarrett
 * SW7346-537: Fixing wrap buffer size calculation when buffer is empty
 * 
 * Hydra_Software_Devel/13   11/9/11 5:50p jgarrett
 * SW7346-537: If buffers are empty, wrap buffer size was not populated
 * 
 * Hydra_Software_Devel/12   10/11/11 6:53p gskerl
 * SW7425-1479: Set a DFIFO's SOURCE_FIFO_ID to an invalid value (0x1F)
 * when it is not used, in order to prevent conflict with other DFIFOs
 * 
 * Hydra_Software_Devel/11   9/16/11 6:50p gskerl
 * SW7231-129: Refactored to put hardware and software initialization into
 * separate functions.
 * 
 * Hydra_Software_Devel/10   8/31/11 8:53a jgarrett
 * SW7425-1119: Correcting build error
 * 
 * Hydra_Software_Devel/9   8/30/11 7:04p gskerl
 * SW7425-1119: Corrected Playback buffer handling issue that happened
 * when BAPE_Playback_CommitData() is called while Playback is not
 * running.
 * 
 * Hydra_Software_Devel/8   6/7/11 10:47a jgarrett
 * SW7425-681: Setting WRCNT to 0 prior to Adaptive Rate Controller
 * shutdown
 * 
 * Hydra_Software_Devel/7   5/23/11 6:49p jgarrett
 * SW7425-402: Adding multichannel capture support
 * 
 * Hydra_Software_Devel/6   5/12/11 4:39p piyushg
 * SW7425-401: Added PCM playback support for non-interleaved and
 * multichannel data
 * 
 * Hydra_Software_Devel/5   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/4   4/12/11 10:04a jgarrett
 * SWDTV-6305: Fixing SFIFO/DFIFO indexes
 * 
 * Hydra_Software_Devel/3   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/2   4/5/11 7:14p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/2   3/18/11 12:00p jgarrett
 * SW7422-146: Fixing asserts and playfromcapture logic
 * 
 * Hydra_Software_Devel/1   3/10/11 7:03p jgarrett
 * SW7422-146: Refactored DFIFO code, added support for input capture from
 * compressed/multichannel
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"
#include "bchp_aud_fmm_bf_ctrl.h"
#ifdef BCHP_AUD_FMM_BF_ESR_REG_START
#include "bchp_aud_fmm_bf_esr.h"
#endif

#ifdef BCHP_AUD_FMM_BF_ESR1_H_REG_START
#include "bchp_aud_fmm_bf_esr1_h.h"
#endif

#ifdef BCHP_AUD_FMM_BF_ESR2_H_REG_START
#include "bchp_aud_fmm_bf_esr2_h.h"
#endif

BDBG_MODULE(bape_bf_priv);

typedef struct BAPE_SfifoGroup
{
    bool allocated;
    bool started;
    bool ppmCorrection;
    unsigned numChannelPairs;
    unsigned sampleRate;
    BAPE_Handle deviceHandle;
    uint32_t sfifoIds[BAPE_ChannelPair_eMax];
    uint32_t adaptRateIds[BAPE_ChannelPair_eMax];
    BAPE_SfifoGroupSettings settings;
} BAPE_SfifoGroup;

typedef struct BAPE_DfifoGroup
{
    bool allocated;
    bool started;
    unsigned numChannelPairs;
    BAPE_Handle deviceHandle;
    uint32_t dfifoIds[BAPE_ChannelPair_eMax];
    BAPE_DfifoGroupSettings settings;
} BAPE_DfifoGroup;

static BERR_Code BAPE_Sfifo_P_GetBuffer(BAPE_SfifoGroupHandle handle, BAPE_BufferDescriptor *pBuffers, unsigned chPair, unsigned bufferNum);
static BERR_Code BAPE_Sfifo_P_CommitData (BAPE_SfifoGroupHandle handle, unsigned numBytes, unsigned chPair, unsigned bufferNum );
static BERR_Code BAPE_Sfifo_P_GetQueuedBytes(BAPE_SfifoGroupHandle handle, unsigned *pQueuedBytes, unsigned chPair, unsigned bufferNum );
static void BAPE_Bf_P_SfifoStarting(BAPE_SfifoGroupHandle handle);
static void BAPE_Bf_P_SfifoStopping(BAPE_SfifoGroupHandle handle);
static bool BAPE_Sfifo_P_IsEnabled(BAPE_Handle hApe, unsigned sfifoId);
static unsigned BAPE_Bf_P_GetFirstRunningSfifo(BAPE_Handle hApe, const uint32_t *pSfifoIds);

void BAPE_SfifoGroup_P_GetDefaultCreateSettings(
    BAPE_SfifoGroupCreateSettings *pSettings    /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    pSettings->numChannelPairs = 1;
    pSettings->ppmCorrection = false;
}

static void BAPE_Sfifo_P_SetGroup(BAPE_Handle handle, uint32_t sfifoId, uint32_t groupId)
{
    uint32_t regAddr, regVal;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);
    BDBG_ASSERT(sfifoId < BAPE_CHIP_MAX_SFIFOS);
    BDBG_ASSERT(groupId < BAPE_CHIP_MAX_SFIFOS);

    regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_GRPi_ARRAY_BASE + (4*sfifoId);
    regVal = BREG_Read32(handle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_GRPi, GROUP_ID);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_GRPi, GROUP_ID, groupId);
    BREG_Write32(handle->regHandle, regAddr, regVal);
}

BERR_Code BAPE_SfifoGroup_P_Create(
    BAPE_Handle deviceHandle,           
    const BAPE_SfifoGroupCreateSettings *pSettings,
    BAPE_SfifoGroupHandle *pHandle  /* [out] */
    )
{
    unsigned i, sfifo, adaptrate=(unsigned)-1;
    BERR_Code errCode;
    BAPE_SfifoGroupHandle handle=NULL;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);
    BDBG_ASSERT(pSettings->numChannelPairs <= BAPE_ChannelPair_eMax);

    /* Sanity Check */
    if ( pSettings->numChannelPairs > 1 && pSettings->ppmCorrection )
    {
        BDBG_ERR(("PPM Correction is not supported for multichannel data"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Find an available group handle */
    for ( i = 0; i < BAPE_CHIP_MAX_SFIFO_GROUPS; i++ )
    {
        BDBG_ASSERT(NULL != deviceHandle->sfifoGroups[i]);
        if ( !deviceHandle->sfifoGroups[i]->allocated )
        {
            handle = deviceHandle->sfifoGroups[i];
            break;
        }
    }

    /* If none found, return error */
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Now search for the correct number of resources */
    errCode = BAPE_P_AllocateFmmResource(deviceHandle, BAPE_FmmResourceType_eSfifo, pSettings->numChannelPairs, &sfifo);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_alloc_sfifo;
    }
    if ( pSettings->ppmCorrection )
    {
        errCode = BAPE_P_AllocateFmmResource(deviceHandle, BAPE_FmmResourceType_eAdaptiveRate, pSettings->numChannelPairs, &adaptrate);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_alloc_adaptrate;
        }
    }

    /* Successfully allocated resources.  Initialize Group */
    handle->allocated = true;
    handle->started = false;
    handle->numChannelPairs = pSettings->numChannelPairs;
    handle->ppmCorrection = pSettings->ppmCorrection;
    handle->deviceHandle = deviceHandle;
    handle->sampleRate = 0;
    BKNI_Memset(&handle->settings, 0, sizeof(handle->settings));
    handle->settings.stereoData = true;
    handle->settings.signedData = true;
    handle->settings.sampleRepeatEnabled = deviceHandle->settings.rampPcmSamples;
    handle->settings.dataWidth = 32;
    handle->settings.defaultSampleRate = 48000;
    BKNI_Memset(handle->sfifoIds, 0xff, sizeof(handle->sfifoIds));
    BKNI_Memset(handle->adaptRateIds, 0xff, sizeof(handle->adaptRateIds));
    for ( i = 0; i < pSettings->numChannelPairs; i++ )
    {
        handle->sfifoIds[i] = sfifo + i;
        BAPE_Sfifo_P_SetGroup(handle->deviceHandle, sfifo+i, sfifo);    /* Set HW Grouping */
    }
    if ( handle->ppmCorrection )
    {
        for ( i = 0; i < pSettings->numChannelPairs; i++ )
        {
            handle->adaptRateIds[i] = adaptrate + i;
            /* Adapt Rate -> SFIFO linkage is done at start time */
        }
    }
    *pHandle = handle;
    return BERR_SUCCESS;

    err_alloc_adaptrate:
    BAPE_P_FreeFmmResource(deviceHandle, BAPE_FmmResourceType_eSfifo, pSettings->numChannelPairs, sfifo);
    err_alloc_sfifo:
    return errCode;
}

void BAPE_SfifoGroup_P_Destroy(
    BAPE_SfifoGroupHandle handle
    )
{
    unsigned i;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(!handle->started);
    BDBG_OBJECT_ASSERT(handle->deviceHandle, BAPE_Device);

    /* Release Resources */
    if ( handle->ppmCorrection )
    {
        BAPE_P_FreeFmmResource(handle->deviceHandle, BAPE_FmmResourceType_eAdaptiveRate, handle->numChannelPairs, handle->adaptRateIds[0]);
    }
    /* Clear SFIFO Grouping */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        BAPE_Sfifo_P_SetGroup(handle->deviceHandle, handle->sfifoIds[i], handle->sfifoIds[i]);
    }
    BAPE_P_FreeFmmResource(handle->deviceHandle, BAPE_FmmResourceType_eSfifo, handle->numChannelPairs, handle->sfifoIds[0]);
    BKNI_Memset(handle->sfifoIds, 0xff, sizeof(handle->sfifoIds));
    BKNI_Memset(handle->adaptRateIds, 0xff, sizeof(handle->adaptRateIds));
    handle->allocated = false;
}

void BAPE_SfifoGroup_P_GetSettings(
    BAPE_SfifoGroupHandle handle,
    BAPE_SfifoGroupSettings *pSettings  /* [out] */
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    *pSettings = handle->settings;
}

BERR_Code BAPE_SfifoGroup_P_SetSettings(
    BAPE_SfifoGroupHandle handle,
    const BAPE_SfifoGroupSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    if ( handle->started )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    handle->settings = *pSettings;

    return BERR_SUCCESS;
}

BERR_Code BAPE_SfifoGroup_P_Start(
    BAPE_SfifoGroupHandle handle,
    bool enableOnly                 /* If true, a separate call to BAPE_SfifoGroup_P_Run_isr is required to 
                                    start data flow.  If false, data flow will start immediately. */
    )
{
    uint32_t regAddr, regVal;
    unsigned i;
    BAPE_Handle deviceHandle;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    if ( handle->started )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    /* Program each SFIFO */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        uint32_t base, end, watermark, wrpoint, writeOffset;

        regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE + (4*handle->sfifoIds[i]);
        regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
        regVal &= ~(
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, PROCESS_SEQ_ID_VALID)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, PROCESS_ID_HIGH)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, REVERSE_ENDIAN)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, BIT_RESOLUTION)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SHARED_SBUF_ID)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SHARE_SBUF)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SFIFO_START_HALFFULL)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, DMA_READ_DISABLE)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SAMPLE_REPEAT_ENABLE)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, NOT_PAUSE_WHEN_EMPTY)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, START_SELECTION)|
#ifdef BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_STCSYNC_ENABLE_MASK
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, STCSYNC_ENABLE)|
#endif
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SAMPLE_CH_MODE)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SOURCEFIFO_SIZE_DOUBLE)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, BUFFER_PAIR_ENABLE)|
                   BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SOURCEFIFO_ENABLE)
                   );
        regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, PROCESS_SEQ_ID_VALID, 1);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, PROCESS_ID_HIGH, handle->settings.highPriority?1:0);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, BIT_RESOLUTION, handle->settings.dataWidth==32?0:handle->settings.dataWidth);
        if ( handle->settings.master )
        {
            BDBG_ASSERT(handle->settings.master->allocated);
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SHARED_SBUF_ID, handle->settings.master->sfifoIds[i]);
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SHARE_SBUF, 1);
        }
        regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SFIFO_START_HALFFULL, 1);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, DMA_READ_DISABLE, handle->settings.bypassMemory?1:0);
        if ( handle->settings.sampleRepeatEnabled && deviceHandle->settings.rampPcmSamples )
        {
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SAMPLE_REPEAT_ENABLE, 1);
        }
        regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, NOT_PAUSE_WHEN_EMPTY, handle->settings.loopAround?1:0);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, START_SELECTION, handle->settings.wrpointEnabled?1:0);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SAMPLE_CH_MODE, handle->settings.stereoData?0:1);
        if ( handle->settings.interleaveData )
        {
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SOURCEFIFO_SIZE_DOUBLE, 1);
        }
        else
        {
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_CFGi, BUFFER_PAIR_ENABLE, 1);            
        }
        BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

        /* Sign handling is in the GRP register */
        regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_GRPi_ARRAY_BASE + (4*handle->sfifoIds[i]);
        regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
        regVal &= ~BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_GRPi, INVERT_MSB);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_GRPi, INVERT_MSB, handle->settings.signedData?0:1);
        BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

        /* Setup Ringbuffer Registers */
        regAddr = BAPE_P_SFIFO_TO_RDADDR_REG(handle->sfifoIds[i]);
        base = handle->settings.bufferInfo[2*i].base;
        end = base + handle->settings.bufferInfo[2*i].length - 1;
        watermark = handle->settings.bufferInfo[2*i].watermark;
        wrpoint = handle->settings.bufferInfo[2*i].wrpoint;
        writeOffset = handle->settings.bufferInfo[2*i].writeOffset;
        BREG_Write32(deviceHandle->regHandle, regAddr, base);
        regAddr += 4;   /* Write is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, base);  /* leave buf empty for now */
        regAddr += 4;   /* Base is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, base);
        regAddr += 4;   /* End is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, end);
        regAddr += 4;   /* Freefull is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, watermark);
        regAddr += 4;   /* WRPOINT is last */
        BREG_Write32(deviceHandle->regHandle, regAddr, wrpoint);
        BAPE_Sfifo_P_CommitData (handle,  writeOffset, i  , 0 ); /* now adjust write pointer for existing data */

        if ( handle->settings.interleaveData )
        {
            base=end=watermark=wrpoint=writeOffset=0;
        }
        else
        {
            base = handle->settings.bufferInfo[(2*i)+1].base;
            end = base + handle->settings.bufferInfo[(2*i)+1].length - 1;
            watermark = handle->settings.bufferInfo[(2*i)+1].watermark;
            wrpoint = handle->settings.bufferInfo[(2*i)+1].wrpoint;
            writeOffset = handle->settings.bufferInfo[(2*i)+1].writeOffset;
        }
        regAddr += 4;   /* Next RDADDR */
        BREG_Write32(deviceHandle->regHandle, regAddr, base);
        regAddr += 4;   /* Write is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, base);  /* leave buf empty for now */
        regAddr += 4;   /* Base is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, base);
        regAddr += 4;   /* End is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, end);
        regAddr += 4;   /* Freefull is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, watermark);
        regAddr += 4;   /* WRPOINT is last */
        BREG_Write32(deviceHandle->regHandle, regAddr, wrpoint);
        BAPE_Sfifo_P_CommitData (handle,  writeOffset, i  , 1 ); /* now adjust write pointer for existing data */
    }

    /* Enable all SFIFO's first. */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        BDBG_MSG(("Enabling SFIFO %u", handle->sfifoIds[i]));
        regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE + (4*handle->sfifoIds[i]);
        regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
        regVal |= BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SOURCEFIFO_ENABLE);
        BREG_Write32(deviceHandle->regHandle, regAddr, regVal);
    }

    if ( !enableOnly )
    {
        /* Run */
        for ( i = 0; i < handle->numChannelPairs; i++ )
        {
            BDBG_MSG(("Enabling PLAY_RUN for SFIFO %u", handle->sfifoIds[i]));
            regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_CTRLi_ARRAY_BASE + (4*handle->sfifoIds[i]);
            BREG_Write32(deviceHandle->regHandle, regAddr, 1);
        }
    }

    BAPE_Bf_P_SfifoStarting(handle);

    /* Enable Adaptive Rate Controllers if Required */
    if ( handle->ppmCorrection )
    {
        for ( i = 0; i < handle->numChannelPairs; i++ )
        {
            BKNI_EnterCriticalSection();
#ifdef BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG
            regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + ((BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_1_CFG - BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG)*handle->adaptRateIds[i]);
            /* This register must be manipulated in critical section, it is programmed at sample rate changes as well */
            regVal = BREG_Read32_isr(deviceHandle->regHandle, regAddr);

            regVal &= ~(BCHP_MASK(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_SFIFO_SEL)|
                        BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, TRIWINDOW_WIDTH_SEL)|
                        BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_ENABLE)|
                        BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_MASTER_ENABLE));

            regVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_SFIFO_SEL, handle->sfifoIds[i]));         
            /* TODO: hardcoding window width field to 8 ie actual window width of 256. This has to be changed later
               to be taken from application */
            regVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, TRIWINDOW_WIDTH_SEL, 8));
            /* Earlier, for a decode channel FW was setting the enable flag. 
            Now we're doing AdaptRate control for PCM channels also. So let the 
            PI set this flag always */                
            regVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_ENABLE, 1));
            regVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_MASTER_ENABLE, 1));                
#ifndef BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_THRESHOLD_0
            regVal &= ~BCHP_MASK(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_THRESHOLD);
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_THRESHOLD, 0xffff);
#endif
            BREG_Write32_isr(deviceHandle->regHandle, regAddr, regVal);

#ifdef BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_THRESHOLD_0
            regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_THRESHOLD_0 + 4*handle->adaptRateIds[i];
            regVal = BREG_Read32_isr(deviceHandle->regHandle, regAddr);
            regVal &= ~BCHP_MASK(AUD_FMM_BF_CTRL_ADAPTRATE_THRESHOLD_0, ADAPTIVE_RATE_THRESHOLD);
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_THRESHOLD_0, ADAPTIVE_RATE_THRESHOLD, 0xffff);
            BREG_Write32_isr(deviceHandle->regHandle, regAddr, regVal);
#endif
            regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_REPEATDROP_WRCNT_0 + 4*handle->adaptRateIds[i];
            BREG_Write32_isr(deviceHandle->regHandle, regAddr, 0);
#else
            regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_CFGi_ARRAY_BASE + ((BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_CFGi_ARRAY_ELEMENT_SIZE/8)*handle->adaptRateIds[i]);
            /* This register must be manipulated in critical section, it is programmed at sample rate changes as well */
            regVal = BREG_Read32_isr(deviceHandle->regHandle, regAddr);

            regVal &= ~(BCHP_MASK(AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPTIVE_SFIFO_SEL)|
                        BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, TRIWINDOW_WIDTH_SEL)|
                        BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, AUTOMATIC_RATE_ENABLE)|
                        BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPTIVE_RATE_MASTER_ENABLE));

            regVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPTIVE_SFIFO_SEL, handle->sfifoIds[i]));         
            /* TODO: hardcoding window width field to 8 ie actual window width of 256. This has to be changed later
               to be taken from application */
            regVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, TRIWINDOW_WIDTH_SEL, 8));
            /* Earlier, for a decode channel FW was setting the enable flag. 
            Now we're doing AdaptRate control for PCM channels also. So let the 
            PI set this flag always */                
            regVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, AUTOMATIC_RATE_ENABLE, 1));
            regVal |= (BCHP_FIELD_DATA (AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPTIVE_RATE_MASTER_ENABLE, 1));                
            BREG_Write32_isr(deviceHandle->regHandle, regAddr, regVal);

#ifdef BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_THRESHOLDi_ARRAY_BASE
            regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_THRESHOLDi_ARRAY_BASE + ((BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_THRESHOLDi_ARRAY_ELEMENT_SIZE/8)*handle->adaptRateIds[i]);
            regVal = BREG_Read32_isr(deviceHandle->regHandle, regAddr);
            regVal &= ~BCHP_MASK(AUD_FMM_BF_CTRL_ADAPTRATE_THRESHOLDi, ADAPTIVE_RATE_THRESHOLD);
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_THRESHOLDi, ADAPTIVE_RATE_THRESHOLD, 0xffff);
            BREG_Write32_isr(deviceHandle->regHandle, regAddr, regVal);
#endif
            regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_REPEATDROP_WRCNTi_ARRAY_BASE + ((BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_REPEATDROP_WRCNTi_ARRAY_ELEMENT_SIZE/8)*handle->adaptRateIds[i]);
            BREG_Write32_isr(deviceHandle->regHandle, regAddr, 0);            
#endif

            BAPE_SfifoGroup_P_SetSampleRate_isr(handle, handle->settings.defaultSampleRate);
            BKNI_LeaveCriticalSection();
        }
    }

    handle->started = true;
    return BERR_SUCCESS;
}

void BAPE_SfifoGroup_P_Stop(
    BAPE_SfifoGroupHandle handle
    )
{
    uint32_t regAddr, regVal;
    unsigned i;
    BAPE_Handle deviceHandle;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    if ( !handle->started )
    {
        return;
    }
    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    BAPE_Bf_P_SfifoStopping(handle);

    if ( handle->ppmCorrection )
    {
#ifdef BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG
        /* Workaround for HWAIO-20 (SW7425-681) - Program WRCNT to 0 prior to clearing master enable bit. */
        for ( i = 0; i < handle->numChannelPairs; i++ )
        {            
            regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_REPEATDROP_WRCNT_0 + ((BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_REPEATDROP_WRCNT_1 - BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_REPEATDROP_WRCNT_0)*handle->adaptRateIds[i]);
            BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        }
        for ( i = 0; i < handle->numChannelPairs; i++ )
        {
            regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + ((BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_1_CFG - BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG)*handle->adaptRateIds[i]);
            /* This register must be manipulated in critical section, it is programmed at sample rate changes as well */
            BKNI_EnterCriticalSection();
            regVal = BREG_Read32_isr(deviceHandle->regHandle, regAddr);
            regVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_ENABLE)|
                        BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPTIVE_RATE_MASTER_ENABLE));
            BREG_Write32_isr(deviceHandle->regHandle, regAddr, regVal);
            BKNI_LeaveCriticalSection();
        }
#else
        /* Workaround for HWAIO-20 (SW7425-681) - Program WRCNT to 0 prior to clearing master enable bit. */
        for ( i = 0; i < handle->numChannelPairs; i++ )
        {            
            regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_REPEATDROP_WRCNTi_ARRAY_BASE + ((BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_REPEATDROP_WRCNTi_ARRAY_ELEMENT_SIZE/8)*handle->adaptRateIds[i]);
            BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        }
        for ( i = 0; i < handle->numChannelPairs; i++ )
        {
            regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_CFGi_ARRAY_BASE + ((BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_CFGi_ARRAY_ELEMENT_SIZE/8)*handle->adaptRateIds[i]);
            /* This register must be manipulated in critical section, it is programmed at sample rate changes as well */
            BKNI_EnterCriticalSection();
            regVal = BREG_Read32_isr(deviceHandle->regHandle, regAddr);
            regVal &= ~(BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, AUTOMATIC_RATE_ENABLE)|
                        BCHP_MASK (AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPTIVE_RATE_MASTER_ENABLE));
            BREG_Write32_isr(deviceHandle->regHandle, regAddr, regVal);
            BKNI_LeaveCriticalSection();
        }        
#endif
    }
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        uint32_t sfifo;
        unsigned timeout;

        sfifo = handle->sfifoIds[i];

        BDBG_MSG(("Disabling PLAY_RUN for SFIFO %u", sfifo));
        regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_CTRLi_ARRAY_BASE + (4*sfifo);
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);

        /* Wait for group flow to stop */
        for ( timeout = 1000; timeout > 0; timeout-- )
        {
            regVal = BREG_Read32(deviceHandle->regHandle, BCHP_AUD_FMM_BF_CTRL_SOURCECH_GROUP_FLOWON);
            if ( 0 == (regVal & (1<<sfifo)) )
            {
                break;
            }
            BKNI_Sleep(1);
        }
        if ( 0 == timeout )
        {
            regVal = BREG_Read32(deviceHandle->regHandle, BCHP_AUD_FMM_BF_CTRL_SOURCECH_GROUP_FLOWON);
            BDBG_WRN(("Timeout waiting for SFIFO %u flow to stop [0x%08x]", sfifo, regVal));
        }

        /* Disable Source Channels */
        BDBG_MSG(("Disabling SFIFO %u", sfifo));
        regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_BASE + (4*sfifo);
        regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
        regVal &= ~BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_CFGi, SOURCEFIFO_ENABLE);
        BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

        /* Wait for the source channel to stop */            
        for ( timeout = 1000; timeout > 0; timeout-- )
        {
            regVal = BREG_Read32(deviceHandle->regHandle, BCHP_AUD_FMM_BF_CTRL_SOURCECH_GROUP_ENABLE);
            if ( 0 == (regVal & (1<<sfifo)) )
            {
                break;
            }
            BKNI_Sleep(1);
        }
        if ( 0 == timeout )
        {
            regVal = BREG_Read32(deviceHandle->regHandle, BCHP_AUD_FMM_BF_CTRL_SOURCECH_GROUP_ENABLE);
            BDBG_WRN(("Timeout waiting for SFIFO %u enable status to clear [0x%08x]", sfifo, regVal));
        }
    }

    /* Reset Ringbuffers */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        regAddr = BAPE_P_SFIFO_TO_RDADDR_REG(handle->sfifoIds[i]);
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* Write is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* Base is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* End is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* Freefull is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* WRPOINT is last */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* Next RDADDR */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* Write is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* Base is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* End is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* Freefull is next */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
        regAddr += 4;   /* WRPOINT is last */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
    }

    handle->started = false;
}

void BAPE_SfifoGroup_P_Run_isr(
    BAPE_SfifoGroupHandle handle
    )
{
    uint32_t regAddr;
    unsigned i;
    BAPE_Handle deviceHandle;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    if ( !handle->started )
    {
        BDBG_ERR(("SFIFO Group %#x is not started, can not run.", handle));
        return;
    }
    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        BDBG_MSG(("Enabling PLAY_RUN for SFIFO %u", handle->sfifoIds[i]));
        regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_CTRLi_ARRAY_BASE + (4*handle->sfifoIds[i]);
        BREG_Write32(deviceHandle->regHandle, regAddr, 1);
    }
}

void BAPE_SfifoGroup_P_Halt_isr(
    BAPE_SfifoGroupHandle handle
    )
{
    uint32_t regAddr;
    unsigned i;
    BAPE_Handle deviceHandle;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    if ( !handle->started )
    {
        BDBG_ERR(("SFIFO Group %#x is not started, can not halt.", handle));
        return;
    }
    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        BDBG_MSG(("Disabling PLAY_RUN for SFIFO %u", handle->sfifoIds[i]));
        regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_CTRLi_ARRAY_BASE + (4*handle->sfifoIds[i]);
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
    }
}

void BAPE_SfifoGroup_P_SetSampleRate_isr(
    BAPE_SfifoGroupHandle handle,
    unsigned sampleRate
    )
{
    uint32_t regAddr, regVal;
    unsigned i;
    BAPE_Handle deviceHandle;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BKNI_ASSERT_ISR_CONTEXT();

    if ( !handle->ppmCorrection )
    {
        return;
    }

    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
#ifdef BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG
        regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG + 4*handle->adaptRateIds[i];

        regVal = BREG_Read32_isr(deviceHandle->regHandle, regAddr);
        regVal &= ~(BCHP_MASK(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPT_SAMPLINGRATE));
        switch ( sampleRate )
        {
        case 32000:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPT_SAMPLINGRATE, 0);
            break;
        default:
        case 48000:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPT_SAMPLINGRATE, 1);
            break;
        case 96000:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPT_SAMPLINGRATE, 2);
            break;
        case 192000:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPT_SAMPLINGRATE, 3);
            break;
        case 44100:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPT_SAMPLINGRATE, 4);
            break;
        case 88200:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPT_SAMPLINGRATE, 5);
            break;
        case 176400:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_0_CFG, ADAPT_SAMPLINGRATE, 6);
            break;
        }
#else
        regAddr = BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_CFGi_ARRAY_BASE + ((BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_CFGi_ARRAY_ELEMENT_SIZE/8)*handle->adaptRateIds[i]);

        regVal = BREG_Read32_isr(deviceHandle->regHandle, regAddr);
        regVal &= ~(BCHP_MASK(AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPT_SAMPLINGRATE));
        switch ( sampleRate )
        {
        case 32000:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPT_SAMPLINGRATE, 0);
            break;
        default:
        case 48000:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPT_SAMPLINGRATE, 1);
            break;
        case 96000:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPT_SAMPLINGRATE, 2);
            break;
        case 192000:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPT_SAMPLINGRATE, 3);
            break;
        case 44100:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPT_SAMPLINGRATE, 4);
            break;
        case 88200:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPT_SAMPLINGRATE, 5);
            break;
        case 176400:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_ADAPTRATE_CFGi, ADAPT_SAMPLINGRATE, 6);
            break;
        }
#endif
        BREG_Write32_isr(deviceHandle->regHandle, regAddr, regVal);
    }
}

void BAPE_SfifoGroup_P_GetOutputFciIds(
    BAPE_SfifoGroupHandle handle,
    BAPE_FciIdGroup *pFciGroup      /* [out] */
    )
{
    unsigned i;
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(NULL != pFciGroup);
    BAPE_FciIdGroup_Init(pFciGroup);
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        pFciGroup->ids[i] = handle->sfifoIds[i];    /* SFIFO FCI ID Base is 0x000*/
    }
}

BERR_Code BAPE_SfifoGroup_P_GetBuffer(
    BAPE_SfifoGroupHandle handle,
    BAPE_BufferDescriptor *pBuffers      /* [out] */
    )
{
    BERR_Code errCode;
    unsigned chPair;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(NULL != pBuffers);

    BKNI_Memset(pBuffers, 0, sizeof(BAPE_BufferDescriptor));

    /* TODO: Handle non-interleaved and multichannel */
    pBuffers->interleaved = handle->settings.interleaveData;
    if (pBuffers->interleaved) 
    {
        pBuffers->numBuffers = handle->numChannelPairs;
    }
    else
    {
        pBuffers->numBuffers = handle->numChannelPairs * 2;
    }

    for (chPair = 0; chPair < handle->numChannelPairs; chPair++) 
    {
        if (pBuffers->interleaved)
        {
            errCode = BAPE_Sfifo_P_GetBuffer(handle, pBuffers, chPair, 0);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
        }
        else
        {
            errCode = BAPE_Sfifo_P_GetBuffer(handle, pBuffers, chPair, 0);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }

            errCode = BAPE_Sfifo_P_GetBuffer(handle, pBuffers, chPair, 1);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
        }

    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Sfifo_P_GetBuffer(
    BAPE_SfifoGroupHandle handle,   
    BAPE_BufferDescriptor *pBuffers,
    unsigned chPair,                /*0,1,2,3*/
    unsigned bufferNum              /*0,1*/
    )
{
    BERR_Code errCode;

    uint32_t rd,wr,base,sfifoId,rdaddr,wraddr;
    unsigned bufferSize, padding=1024;

    sfifoId = handle->sfifoIds[chPair];
    bufferSize = handle->settings.bufferInfo[chPair*2 + bufferNum].length;
    rd = BREG_Read32(handle->deviceHandle->regHandle, BAPE_P_SFIFO_TO_RDADDR_REG(sfifoId) + (bufferNum * BAPE_P_RINGBUFFER_STRIDE));
    wr = BREG_Read32(handle->deviceHandle->regHandle, BAPE_P_SFIFO_TO_WRADDR_REG(sfifoId) + (bufferNum * BAPE_P_RINGBUFFER_STRIDE));
    base = BREG_Read32(handle->deviceHandle->regHandle, BAPE_P_SFIFO_TO_BASEADDR_REG(sfifoId) + (bufferNum * BAPE_P_RINGBUFFER_STRIDE));

    BDBG_MSG(("PB GetBuffer: RDADDR 0x%x WRADDR 0x%x BASEADDR 0x%x Size %d ", rd, wr, base, bufferSize));

#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
    rdaddr = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
    wraddr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#else
    rdaddr = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
    wraddr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#endif

    if (bufferNum == 0) 
    {
        errCode = BMEM_ConvertOffsetToAddress(handle->deviceHandle->memHandle, wraddr, &pBuffers->buffers[BAPE_Channel_eLeft].pBuffer);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
        }
        errCode = BMEM_ConvertOffsetToAddress(handle->deviceHandle->memHandle, base, &pBuffers->buffers[BAPE_Channel_eLeft].pWrapBuffer);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
        }
    }
    else if (bufferNum == 1) 
    {
        errCode = BMEM_ConvertOffsetToAddress(handle->deviceHandle->memHandle, wraddr, &pBuffers->buffers[BAPE_Channel_eRight].pBuffer);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
        }
        errCode = BMEM_ConvertOffsetToAddress(handle->deviceHandle->memHandle, base, &pBuffers->buffers[BAPE_Channel_eRight].pWrapBuffer);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
        }
    }

    if ( wraddr > rdaddr )
    {
        pBuffers->bufferSize = bufferSize - (wraddr-base);
        pBuffers->wrapBufferSize = rdaddr-base;
    }
    else if ( wraddr < rdaddr )
    {
        pBuffers->bufferSize = rdaddr-wraddr;
    }
    else    /* equal */
    {
        if ( 
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
             BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ==
             BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP) 
#else
             BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ==
             BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP) 
#endif
           )
        {
            /* Toggle bits are equal, buffer is empty. */
            pBuffers->bufferSize = bufferSize - (wraddr-base);
            pBuffers->wrapBufferSize = bufferSize - pBuffers->bufferSize;
        }
        else
        {
            /* Toggle bit mismatch, buffer is full. */
            pBuffers->bufferSize = 0;
        }
    }

    /* Don't allow entire buffer to be filled, need gap between rd/wr for master/slave setups */
    if ( pBuffers->wrapBufferSize > padding )
    {
        pBuffers->wrapBufferSize -= padding;
        padding = 0;
    }
    else
    {
        padding -= pBuffers->wrapBufferSize;
        pBuffers->wrapBufferSize = 0;
    }
    if ( pBuffers->bufferSize > padding )
    {
        pBuffers->bufferSize -= padding;
        padding = 0;
    }
    else
    {
        pBuffers->bufferSize = 0;
    }

    /* Make sure wrap pointers are NULL if we have no data */
    if (pBuffers->interleaved)
    {
        if ( pBuffers->wrapBufferSize == 0 )
        {
            pBuffers->buffers[bufferNum*2].pWrapBuffer = NULL;
        }
        if ( pBuffers->bufferSize == 0 )
        {
            pBuffers->buffers[bufferNum*2].pBuffer = NULL;
        }
    }
    else
    {
        if ( pBuffers->wrapBufferSize == 0 )
        {
            pBuffers->buffers[bufferNum].pWrapBuffer = NULL;
        }
        if ( pBuffers->bufferSize == 0 )
        {
            pBuffers->buffers[bufferNum].pBuffer = NULL;
        }
    }
    return BERR_SUCCESS;
}



BERR_Code BAPE_SfifoGroup_P_CommitData(
    BAPE_SfifoGroupHandle handle,
    unsigned numBytes                   /* Number of bytes written into the buffer */
    )
{
    BERR_Code errCode;
    unsigned chPair;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    /* TODO: Handle multichannel and non-interleaved */
    for (chPair = 0; chPair < handle->numChannelPairs; chPair++) 
    {
        if (handle->settings.interleaveData)
        {
            errCode = BAPE_Sfifo_P_CommitData(handle, numBytes, chPair, 0);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
        }
        else
        {
            errCode = BAPE_Sfifo_P_CommitData(handle, numBytes, chPair, 0);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
            errCode = BAPE_Sfifo_P_CommitData(handle, numBytes, chPair, 1);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
        }
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Sfifo_P_CommitData (BAPE_SfifoGroupHandle handle,                                                    
    unsigned numBytes,                   /* Number of bytes written into the buffer */
    unsigned chPair,                     /*0,1,2,3*/
    unsigned bufferNum                   /*0,1*/
    )                                                                                
{

    uint32_t rd,wr,base,sfifoId,rdaddr,wraddr;
    unsigned bufferSize;

    sfifoId = handle->sfifoIds[chPair];
    bufferSize = handle->settings.bufferInfo[chPair*2 + bufferNum].length;
    rd = BREG_Read32(handle->deviceHandle->regHandle, BAPE_P_SFIFO_TO_RDADDR_REG(sfifoId) + (bufferNum * BAPE_P_RINGBUFFER_STRIDE));
    wr = BREG_Read32(handle->deviceHandle->regHandle, BAPE_P_SFIFO_TO_WRADDR_REG(sfifoId) + (bufferNum * BAPE_P_RINGBUFFER_STRIDE));
    base = BREG_Read32(handle->deviceHandle->regHandle, BAPE_P_SFIFO_TO_BASEADDR_REG(sfifoId) + (bufferNum * BAPE_P_RINGBUFFER_STRIDE));

    BDBG_MSG(("PB Commit: RDADDR 0x%x WRADDR 0x%x BASEADDR 0x%x Size %d ", rd, wr, base, bufferSize));

#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
    rdaddr = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
    wraddr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#else
    rdaddr = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
    wraddr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#endif

    /* Make sure the write pointer doesn't try and pass read */
    if ( rdaddr > wraddr )
    {
        if ( wraddr + numBytes > rdaddr )
        {
            BDBG_ERR(("Playback: Attempt to write more data than available in the buffer."));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        else if ( wraddr + numBytes == rdaddr )
        {
            if ( 
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
                 BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ==
                 BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP) 
#else
                 BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ==
                 BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP) 
#endif
                )
            {
                /* If the toggle bit is the same we will overflow. */
                BDBG_ERR(("Playback: Attempt to write more data than available in the buffer."));
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
        }
    }
    else    /* rdaddr <= wraddr */
    {
        if ( wraddr + numBytes > (base + bufferSize) )
        {
            BDBG_ERR(("Playback: Attempt to write more data than available in the buffer."));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
    }

    /* The request is legal.  Update the write pointer. */
    wraddr += numBytes;
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
    wr &= ~BCHP_MASK(AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
    if ( wraddr == bufferSize + base )
    {
        BDBG_MSG(("Inverting toggle bit - was 0x%x now 0x%x", wr, wr ^ BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP, 1)));
        wr ^= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP, 1);   /* flip the toggle bit */
        wraddr = base;
    }
    wr |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR, wraddr);
#else
    wr &= ~BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
    if ( wraddr == bufferSize + base )
    {
        BDBG_MSG(("Inverting toggle bit - was 0x%x now 0x%x", wr, wr ^ BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP, 1)));
        wr ^= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP, 1);   /* flip the toggle bit */
        wraddr = base;
    }
    wr |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR, wraddr);
#endif
    BREG_Write32(handle->deviceHandle->regHandle, BAPE_P_SFIFO_TO_WRADDR_REG(sfifoId) + (bufferNum * BAPE_P_RINGBUFFER_STRIDE), wr);

    return BERR_SUCCESS;
}



BERR_Code BAPE_SfifoGroup_P_Flush(
    BAPE_SfifoGroupHandle handle
    )
{   
    unsigned bufferNum;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    /* TODO: Handle multichannel */

    /* Flush by making write pointer equal to read pointer */
    if (handle->settings.interleaveData) 
    {
        for (bufferNum = 0; bufferNum < handle->numChannelPairs; bufferNum++) 
        {
            BREG_Write32(handle->deviceHandle->regHandle,
                         BAPE_P_SFIFO_TO_WRADDR_REG(handle->sfifoIds[bufferNum]),
                         BREG_Read32(handle->deviceHandle->regHandle,
                                     BAPE_P_SFIFO_TO_RDADDR_REG(handle->sfifoIds[bufferNum])));
        }
    }
    else
    {
        for (bufferNum = 0; bufferNum < handle->numChannelPairs; bufferNum++) 
        {
            BREG_Write32(handle->deviceHandle->regHandle,
                         BAPE_P_SFIFO_TO_WRADDR_REG(handle->sfifoIds[bufferNum]),
                         BREG_Read32(handle->deviceHandle->regHandle,
                                     BAPE_P_SFIFO_TO_RDADDR_REG(handle->sfifoIds[bufferNum])));

            BREG_Write32(handle->deviceHandle->regHandle,
                         BAPE_P_SFIFO_TO_WRADDR_REG(handle->sfifoIds[bufferNum]) + BAPE_P_RINGBUFFER_STRIDE,
                         BREG_Read32(handle->deviceHandle->regHandle,
                                     BAPE_P_SFIFO_TO_RDADDR_REG(handle->sfifoIds[bufferNum] + BAPE_P_RINGBUFFER_STRIDE)));
        }
    }

    return BERR_SUCCESS;
}

BERR_Code BAPE_SfifoGroup_P_GetQueuedBytes(
    BAPE_SfifoGroupHandle handle,
    unsigned *pQueuedBytes
    )
{
    BERR_Code errCode;
    uint32_t chPair;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(NULL != pQueuedBytes);

    /* TODO: Handle non-interleaved and multichannel */
    for (chPair = 0; chPair < handle->numChannelPairs; chPair++) 
    {
        if (handle->settings.interleaveData)
        {
            errCode = BAPE_Sfifo_P_GetQueuedBytes(handle, pQueuedBytes, chPair, 0 );
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
        }
        else
        {
            errCode = BAPE_Sfifo_P_GetQueuedBytes(handle, pQueuedBytes, chPair, 0);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
            errCode = BAPE_Sfifo_P_GetQueuedBytes(handle, pQueuedBytes, chPair, 1);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
            }
        }

    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Sfifo_P_GetQueuedBytes(
    BAPE_SfifoGroupHandle handle, 
    unsigned *pQueuedBytes,
    unsigned chPair,       /*0,1,2,3*/
    unsigned bufferNum     /*0,1*/
    )
{
    uint32_t rd,wr,base,sfifoId,rdaddr,wraddr;
    unsigned bufferSize, queuedBytes;

    sfifoId = handle->sfifoIds[chPair];
    bufferSize = handle->settings.bufferInfo[chPair*2 + bufferNum].length;
    rd = BREG_Read32(handle->deviceHandle->regHandle, BAPE_P_SFIFO_TO_RDADDR_REG(sfifoId) + (bufferNum * BAPE_P_RINGBUFFER_STRIDE));
    wr = BREG_Read32(handle->deviceHandle->regHandle, BAPE_P_SFIFO_TO_WRADDR_REG(sfifoId) + (bufferNum * BAPE_P_RINGBUFFER_STRIDE));
    base = BREG_Read32(handle->deviceHandle->regHandle, BAPE_P_SFIFO_TO_BASEADDR_REG(sfifoId) + (bufferNum * BAPE_P_RINGBUFFER_STRIDE));

#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
    rdaddr = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
    wraddr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#else
    rdaddr = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
    wraddr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#endif

    if ( wraddr > rdaddr )
    {
        queuedBytes = wraddr-rdaddr;
    }
    else if ( wraddr < rdaddr )
    {
        queuedBytes = (wraddr-base)+(bufferSize-(rdaddr-base));
    }
    else    /* equal */
    {
        if ( 
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
             BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ==
             BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP) 
#else
             BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ==
             BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP) 
#endif
            )
        {
            /* Toggle bits are equal, buffer is empty. */
            queuedBytes = 0;
        }
        else
        {
            /* Toggle bit mismatch, buffer is full. */
            queuedBytes = bufferSize;
        }
    }
    *pQueuedBytes = queuedBytes;

    return BERR_SUCCESS;
}


BERR_Code BAPE_SfifoGroup_P_SetFreemarkInterrupt(
    BAPE_SfifoGroupHandle handle,
    BINT_CallbackFunc callback_isr,
    void *pParam1,
    int param2
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    return BAPE_P_SetSourceChannelFreemarkInterrupt(handle->deviceHandle, handle->sfifoIds[0], callback_isr, pParam1, param2);
}

void BAPE_SfifoGroup_P_RearmFreemarkInterrupt(
    BAPE_SfifoGroupHandle handle
    )
{
    uint32_t regVal;
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    regVal = 1<<handle->sfifoIds[0];
#ifdef BCHP_AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK
    BREG_Write32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK, regVal);
#else
    BREG_Write32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_BF_CTRL_REARM_FREE_MARK, regVal);
#endif
}

uint32_t BAPE_SfifoGroup_P_GetHwIndex(
    BAPE_SfifoGroupHandle handle,
    BAPE_ChannelPair channelPair
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    if ( channelPair >= handle->numChannelPairs )
    {
        return 0xFFFFFFFF;
    }
    else
    {
        return handle->sfifoIds[channelPair];
    }
}

uint32_t BAPE_SfifoGroup_P_GetAdaptRateHwIndex(
    BAPE_SfifoGroupHandle handle,
    BAPE_ChannelPair channelPair
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    if ( channelPair >= handle->numChannelPairs )
    {
        return 0xFFFFFFFF;
    }
    else
    {
        return handle->adaptRateIds[channelPair];
    }
}

void BAPE_DfifoGroup_P_GetDefaultCreateSettings(
    BAPE_DfifoGroupCreateSettings *pSettings    /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    pSettings->numChannelPairs = 1;
}

BERR_Code BAPE_DfifoGroup_P_Create(
    BAPE_Handle deviceHandle,
    const BAPE_DfifoGroupCreateSettings *pSettings,
    BAPE_DfifoGroupHandle *pHandle  /* [out] */
    )
{
    unsigned i, dfifo;
    BERR_Code errCode;
    BAPE_DfifoGroupHandle handle=NULL;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);
    BDBG_ASSERT(pSettings->numChannelPairs <= BAPE_ChannelPair_eMax);

    /* Find an available group handle */
    for ( i = 0; i < BAPE_CHIP_MAX_DFIFO_GROUPS; i++ )
    {
        BDBG_ASSERT(NULL != deviceHandle->dfifoGroups[i]);
        if ( !deviceHandle->dfifoGroups[i]->allocated )
        {
            handle = deviceHandle->dfifoGroups[i];
            break;
        }
    }

    /* If none found, return error */
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Now search for the correct number of resources */
    /* TODO: DFIFO's technically don't group so they could be allocated individually
       rather than as a group if that helps optimize resource usage */
    errCode = BAPE_P_AllocateFmmResource(deviceHandle, BAPE_FmmResourceType_eDfifo, pSettings->numChannelPairs, &dfifo);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_alloc_dfifo;
    }

    /* Successfully allocated resources.  Initialize Group */
    handle->allocated = true;
    handle->started = false;
    handle->numChannelPairs = pSettings->numChannelPairs;
    handle->deviceHandle = deviceHandle;
    BKNI_Memset(&handle->settings, 0, sizeof(handle->settings));
    BAPE_FciIdGroup_Init(&handle->settings.input);
    handle->settings.dataWidth = 32;
    BKNI_Memset(handle->dfifoIds, 0xff, sizeof(handle->dfifoIds));
    for ( i = 0; i < pSettings->numChannelPairs; i++ )
    {
        handle->dfifoIds[i] = dfifo + i;
        /* DFIFO's don't support grouping, rather the inputs are grouped */
    }
    *pHandle = handle;
    return BERR_SUCCESS;

    err_alloc_dfifo:
    return errCode;
}

void BAPE_DfifoGroup_P_Destroy(
    BAPE_DfifoGroupHandle handle
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(!handle->started);
    BDBG_OBJECT_ASSERT(handle->deviceHandle, BAPE_Device);

    /* Release Resources */
    BAPE_P_FreeFmmResource(handle->deviceHandle, BAPE_FmmResourceType_eDfifo, handle->numChannelPairs, handle->dfifoIds[0]);
    BKNI_Memset(handle->dfifoIds, 0xff, sizeof(handle->dfifoIds));
    handle->allocated = false;
}

void BAPE_DfifoGroup_P_GetSettings(
    BAPE_DfifoGroupHandle handle,
    BAPE_DfifoGroupSettings *pSettings  /* [out] */
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    *pSettings = handle->settings;
}

BERR_Code BAPE_DfifoGroup_P_SetSettings(
    BAPE_DfifoGroupHandle handle,
    const BAPE_DfifoGroupSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    if ( handle->started )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    switch ( pSettings->dataWidth )
    {
    case 0:
    case 32:
#if BAPE_CHIP_DFIFO_SUPPORTS_16BIT_CAPTURE
    case 16:
#endif
        /* Supported */
        break;
    default:
        BDBG_ERR(("This chip does not support %u-bit DFIFO Capture", pSettings->dataWidth));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    handle->settings = *pSettings;

    return BERR_SUCCESS;
}

static void BAPE_DfifoGroup_P_ApplySettings(
    BAPE_DfifoGroupHandle handle
    )
{
    uint32_t regAddr, regVal, regMask, regData;
    unsigned i;
    bool playFromCapture, captureToSfifo;
    BREG_Handle regHandle;

    regHandle = handle->deviceHandle->regHandle;

    if ( NULL == handle->settings.linkedSfifo )
    {
        /* No SFIFO was provided.  This should capture to memory only. */
        BDBG_ASSERT(handle->settings.bypassMemory == false);
        playFromCapture = false;
        captureToSfifo = false;
    }
    else
    {
        /* SFIFO provided.  Determine if we are bypassing memory (CAPTURE_TO_SOURCEFIFO) or capturing to the SFIFO ringbuffers (PLAY_FROM_CAPTURE). */
        captureToSfifo = handle->settings.bypassMemory;
        playFromCapture = !captureToSfifo;
    }

    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        uint32_t sfifoId = (handle->settings.linkedSfifo) ? handle->settings.linkedSfifo->sfifoIds[i] : BAPE_Bf_P_GetFirstRunningSfifo(handle->deviceHandle, NULL);
        uint32_t fciId = handle->settings.input.ids[i];
        uint32_t base, end, watermark;
#ifdef BCHP_AUD_FMM_BF_CTRL_DESTCH_CFG0
        regAddr = BCHP_AUD_FMM_BF_CTRL_DESTCH_CFG0;
        regMask = 
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, CAPTURE_ENABLE)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, BUFFER_PAIR_ENABLE)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, DESTFIFO_SIZE_DOUBLE)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, PLAY_FROM_CAPTURE)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, CAPTURE_TO_SOURCEFIFO)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, PLAY_FROM_CAPTURE)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, SOURCE_FIFO_ID)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, NOT_PAUSE_WHEN_FULL)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, FCI_CAP_ID)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, PROCESS_ID_HIGH)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, PROCESS_SEQ_ID_VALID);
        regData = 
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFG0, BUFFER_PAIR_ENABLE, (handle->settings.interleaveData)?0:1) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFG0, DESTFIFO_SIZE_DOUBLE, (handle->settings.interleaveData)?1:0) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFG0, CAPTURE_TO_SOURCEFIFO, (captureToSfifo)?1:0) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFG0, PLAY_FROM_CAPTURE, (playFromCapture)?1:0) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFG0, SOURCE_FIFO_ID, sfifoId) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFG0, FCI_CAP_ID, fciId) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFG0, PROCESS_ID_HIGH, (handle->settings.highPriority)?1:0) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFG0, PROCESS_SEQ_ID_VALID, 1);
#if BAPE_CHIP_DFIFO_SUPPORTS_16BIT_CAPTURE
        regMask |= BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_MODE);
        if ( handle->settings.dataWidth == 16 )
        {
            regData |= BCHP_FIELD_ENUM(AUD_FMM_BF_CTRL_DESTCH_CFG0, CAPTURE_MODE, Compressed);
        }
        else
        {
            regData |= BCHP_FIELD_ENUM(AUD_FMM_BF_CTRL_DESTCH_CFG0, CAPTURE_MODE, PCM);
        }
#endif
#else
        BDBG_MSG(("DFIFO %u setup for SFIFO ID %u", handle->dfifoIds[i], sfifoId));
        regAddr = BCHP_AUD_FMM_BF_CTRL_DESTCH_CFGi_ARRAY_BASE;
        regAddr += handle->dfifoIds[i]*(BCHP_AUD_FMM_BF_CTRL_DESTCH_CFGi_ARRAY_ELEMENT_SIZE/8);
        regMask = 
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_ENABLE)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, BUFFER_PAIR_ENABLE)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, DESTFIFO_SIZE_DOUBLE)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, PLAY_FROM_CAPTURE)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_TO_SOURCEFIFO)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, PLAY_FROM_CAPTURE)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, SOURCE_FIFO_ID)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, NOT_PAUSE_WHEN_FULL)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, FCI_CAP_ID)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, PROCESS_ID_HIGH)|
        BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, PROCESS_SEQ_ID_VALID);
        regData = 
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFGi, BUFFER_PAIR_ENABLE, (handle->settings.interleaveData)?0:1) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFGi, DESTFIFO_SIZE_DOUBLE, (handle->settings.interleaveData)?1:0) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_TO_SOURCEFIFO, (captureToSfifo)?1:0) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFGi, PLAY_FROM_CAPTURE, (playFromCapture)?1:0) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFGi, SOURCE_FIFO_ID, sfifoId) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFGi, FCI_CAP_ID, fciId) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFGi, PROCESS_ID_HIGH, (handle->settings.highPriority)?1:0) |
        BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFGi, PROCESS_SEQ_ID_VALID, 1);    
#if BAPE_CHIP_DFIFO_SUPPORTS_16BIT_CAPTURE
        regMask |= BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_MODE);
        if ( handle->settings.dataWidth == 16 )
        {
            regData |= BCHP_FIELD_ENUM(AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_MODE, Compressed);
        }
        else
        {
            regData |= BCHP_FIELD_ENUM(AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_MODE, PCM);
        }
#endif
#endif

        /* configure channel based on above settings */    
        regVal = BREG_Read32(regHandle, regAddr);
        regVal = (regVal & ~regMask) | regData;
        BREG_Write32(regHandle, regAddr, regVal);

        /* program ringbuffer addresses */
        /* Setup Ringbuffer Registers */
        regAddr = BAPE_P_DFIFO_TO_RDADDR_REG(handle->dfifoIds[i]);
        base = handle->settings.bufferInfo[2*i].base;
        end = base + handle->settings.bufferInfo[2*i].length - 1;
        watermark = handle->settings.bufferInfo[2*i].watermark;
        BREG_Write32(regHandle, regAddr, base);
        regAddr += 4;   /* Write is next */
        BREG_Write32(regHandle, regAddr, base);
        regAddr += 4;   /* Base is next */
        BREG_Write32(regHandle, regAddr, base);
        regAddr += 4;   /* End is next */
        BREG_Write32(regHandle, regAddr, end);
        regAddr += 4;   /* Freefull is next */
        BREG_Write32(regHandle, regAddr, watermark);
        regAddr += 4;   /* WRPOINT is last */
        BREG_Write32(regHandle, regAddr, 0);
        if ( handle->settings.interleaveData )
        {
            base=end=watermark=0;
        }
        else
        {
            base = handle->settings.bufferInfo[(2*i)+1].base;
            end = base + handle->settings.bufferInfo[(2*i)+1].length - 1;
            watermark = handle->settings.bufferInfo[(2*i)+1].watermark;
        }
        regAddr += 4;   /* Next RDADDR */
        BREG_Write32(regHandle, regAddr, base);
        regAddr += 4;   /* Write is next */
        BREG_Write32(regHandle, regAddr, base);
        regAddr += 4;   /* Base is next */
        BREG_Write32(regHandle, regAddr, base);
        regAddr += 4;   /* End is next */
        BREG_Write32(regHandle, regAddr, end);
        regAddr += 4;   /* Freefull is next */
        BREG_Write32(regHandle, regAddr, watermark);
        regAddr += 4;   /* WRPOINT is last */
        BREG_Write32(regHandle, regAddr, 0);
    }
}

static void BAPE_DfifoGroup_P_SetCaptureEnable(
    BAPE_DfifoGroupHandle handle,
    uint32_t value
    )
{
    uint32_t baseAddr, stride, mask, data, regVal, regAddr;
    unsigned i;
    BAPE_Handle deviceHandle;

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

#ifdef BCHP_AUD_FMM_BF_CTRL_DESTCH_CFG0
    baseAddr = BCHP_AUD_FMM_BF_CTRL_DESTCH_CFG0;
    stride = 0;
    mask = 
    BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, CAPTURE_ENABLE)|
    BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFG0, PROCESS_SEQ_ID_VALID);
    data = 
    BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFG0, CAPTURE_ENABLE, value)|
    BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFG0, PROCESS_SEQ_ID_VALID, value);
#else
    baseAddr = BCHP_AUD_FMM_BF_CTRL_DESTCH_CFGi_ARRAY_BASE;
    stride = BCHP_AUD_FMM_BF_CTRL_DESTCH_CFGi_ARRAY_ELEMENT_SIZE/8;
    mask = 
    BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_ENABLE)|
    BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, PROCESS_SEQ_ID_VALID);
    data = 
    BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_ENABLE, value)|
    BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_DESTCH_CFGi, PROCESS_SEQ_ID_VALID, value);
#endif    

    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        regAddr = baseAddr + (handle->dfifoIds[i]*stride);
        regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
        regVal = (regVal & ~mask) | data;
        BREG_Write32(deviceHandle->regHandle, regAddr, regVal);
    }
}

#define BAPE_DfifoGroup_P_SetCaptureRun_isr(h, val) do { BKNI_ASSERT_ISR_CONTEXT(); BAPE_DfifoGroup_P_SetCaptureRun((h), (val)); } while (0)
static void BAPE_DfifoGroup_P_SetCaptureRun(
    BAPE_DfifoGroupHandle handle,
    uint32_t value
    )
{
    uint32_t baseAddr, stride;
    unsigned i;

    /* These registers are not read/modify/write and thus are safe from either task or interrupt context */

    BDBG_OBJECT_ASSERT(handle->deviceHandle, BAPE_Device);

#ifdef BCHP_AUD_FMM_BF_CTRL_DESTCH_CTRL0
    baseAddr = BCHP_AUD_FMM_BF_CTRL_DESTCH_CTRL0;
    stride = 0;
#else
    baseAddr = BCHP_AUD_FMM_BF_CTRL_DESTCH_CTRLi_ARRAY_BASE;
    stride = BCHP_AUD_FMM_BF_CTRL_DESTCH_CTRLi_ARRAY_ELEMENT_SIZE/8;
#endif    

    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        BREG_Write32(handle->deviceHandle->regHandle, baseAddr + (handle->dfifoIds[i])*stride, value);
    }
}

BERR_Code BAPE_DfifoGroup_P_Start(
    BAPE_DfifoGroupHandle handle,
    bool enableOnly                 /* If true, a separate call to BAPE_DfifoGroup_P_Run_isr is required to 
                                    start data flow.  If false, data flow will start immediately. */
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    /* Refresh Settings */
    BAPE_DfifoGroup_P_ApplySettings(handle);

    /* Enable */
    BAPE_DfifoGroup_P_SetCaptureEnable(handle, 1);

    handle->started = true;

    if ( !enableOnly )
    {
        /* Run */
        BAPE_DfifoGroup_P_SetCaptureRun(handle, 1);
    }

    return BERR_SUCCESS;
}

void BAPE_DfifoGroup_P_Stop(
    BAPE_DfifoGroupHandle handle
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    /* Halt */
    BAPE_DfifoGroup_P_SetCaptureRun(handle, 0);

    /* Enable */
    BAPE_DfifoGroup_P_SetCaptureEnable(handle, 0);

    /* Reset Ringbuffer Registers to 0 */

    handle->started = false;
}

void BAPE_DfifoGroup_P_Run_isr(
    BAPE_DfifoGroupHandle handle
    )
{
    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    if ( !handle->started )
    {
        BDBG_ERR(("DFIFO Group %#x is not started, can not run.", handle));
        return;
    }

    BAPE_DfifoGroup_P_SetCaptureRun_isr(handle, 1);
}

void BAPE_DfifoGroup_P_Halt_isr(
    BAPE_DfifoGroupHandle handle
    )
{
    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    if ( !handle->started )
    {
        BDBG_ERR(("DFIFO Group %#x is not started, can not halt.", handle));
        return;
    }
    BAPE_DfifoGroup_P_SetCaptureRun_isr(handle, 0);
}

BERR_Code BAPE_DfifoGroup_P_GetBuffer_isr(
    BAPE_DfifoGroupHandle handle,
    BAPE_BufferDescriptor *pBuffers      /* [out] */
    )
{
    uint32_t rd, wr, base, wrap;
    unsigned bufferSize, fifoSize, i, minBufferSize=0x7fffffff;
    BERR_Code errCode;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(NULL != pBuffers);
    BKNI_ASSERT_ISR_CONTEXT();

    BKNI_Memset(pBuffers, 0, sizeof(*pBuffers));

    if ( handle->settings.interleaveData )
    {
        pBuffers->numBuffers = handle->numChannelPairs;
        pBuffers->interleaved = true;
    }
    else
    {
        pBuffers->numBuffers = 2*handle->numChannelPairs;
        pBuffers->interleaved = false;
    }
    fifoSize = handle->settings.bufferInfo[0].length;
    if ( !handle->started )
    {
        return BERR_SUCCESS;
    }

    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        /* Read registers */
        rd = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_RDADDR_REG(handle->dfifoIds[i]));
        wr = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_WRADDR_REG(handle->dfifoIds[i]));
        base = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_BASEADDR_REG(handle->dfifoIds[i]));
    
        /* Same toggle bit means no wrap.  Opposite toggle bits means wrap. */    
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
        wrap = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ^ 
               BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP);

        /* Mask off toggle bits */
        rd = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
        wr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#else
        wrap = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ^ 
               BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP);

        /* Mask off toggle bits */
        rd = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
        wr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#endif
    
        /* Get base address to read from */
        errCode = BMEM_Heap_ConvertOffsetToAddress(handle->deviceHandle->memHandle, rd, &pBuffers->buffers[2*i].pBuffer);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    
        /* Compute size of contiguous space */
        if ( wrap )
        {
            bufferSize = fifoSize - (rd-base);
        }
        else
        {
            bufferSize = (wr - rd);
        }
        
        if ( bufferSize < minBufferSize )
        {
            minBufferSize = bufferSize;
        }

        if ( handle->settings.interleaveData == false )
        {
            /* Read registers */
            rd = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_RDADDR_REG(handle->dfifoIds[i])+BAPE_P_RINGBUFFER_STRIDE);
            wr = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_WRADDR_REG(handle->dfifoIds[i])+BAPE_P_RINGBUFFER_STRIDE);
            base = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_BASEADDR_REG(handle->dfifoIds[i])+BAPE_P_RINGBUFFER_STRIDE);
    
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
            /* Same toggle bit means no wrap.  Opposite toggle bits means wrap. */    
            wrap = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ^ 
                   BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP);
    
            /* Mask off toggle bits */
            rd = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
            wr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#else
            /* Same toggle bit means no wrap.  Opposite toggle bits means wrap. */    
            wrap = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ^ 
                   BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP);

            /* Mask off toggle bits */
            rd = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
            wr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#endif
    
            /* Get base address to read from */
            errCode = BMEM_Heap_ConvertOffsetToAddress(handle->deviceHandle->memHandle, rd, &pBuffers->buffers[(2*i)+1].pBuffer);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
    
            /* Compute size of contiguous space */
            if ( wrap )
            {
                bufferSize = fifoSize - (rd-base);
            }
            else
            {
                bufferSize = (wr - rd);
            }
    
            if ( bufferSize < minBufferSize )
            {
                minBufferSize = bufferSize;
            }
        }
    }
    
    pBuffers->bufferSize = minBufferSize;

    return BERR_SUCCESS;
}

BERR_Code BAPE_DfifoGroup_P_CommitData_isr(
    BAPE_DfifoGroupHandle handle,
    unsigned numBytes                   /* Number of bytes written into the buffer */
    )
{
    uint32_t rd, wr, base, wrap, rdaddr, wraddr, wrapBit;
    unsigned fifoSize, i;
    uint32_t rdaddrRegs[BAPE_Channel_eMax];

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BKNI_ASSERT_ISR_CONTEXT();

    if ( !handle->started )
    {
        return BERR_SUCCESS;
    }

    fifoSize = handle->settings.bufferInfo[0].length;
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        /* Read registers */
        rd = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_RDADDR_REG(handle->dfifoIds[i]));
        wr = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_WRADDR_REG(handle->dfifoIds[i]));
        base = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_BASEADDR_REG(handle->dfifoIds[i]));
    
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
        /* Same toggle bit means no wrap.  Opposite toggle bits means wrap. */    
        wrap = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ^ 
               BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP);
    
        /* Mask off toggle bits */
        rdaddr = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
        wraddr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#else
        /* Same toggle bit means no wrap.  Opposite toggle bits means wrap. */    
        wrap = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ^ 
               BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP);

        /* Mask off toggle bits */
        rdaddr = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
        wraddr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#endif
    
        /* Check for potential overflow */
        if ( wrap )
        {
            if ( ((rdaddr-base)+numBytes) > fifoSize )
            {
                BKNI_LeaveCriticalSection();
                BDBG_ERR(("Invalid number of bytes provided to BAPE_OutputCapture_ConsumeData [wrap]"));
                BDBG_ERR(("rd %#x wr %#x base %#x size %#x numBytes %#x", rd, wr, base, fifoSize, numBytes));
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
        }
        else
        {
            if ( (rdaddr+numBytes) > wraddr )
            {
                BKNI_LeaveCriticalSection();
                BDBG_ERR(("Invalid number of bytes provided to BAPE_OutputCapture_ConsumeData [no wrap]"));
                BDBG_ERR(("rd %#x wr %#x base %#x size %#x numBytes %#x", rd, wr, base, fifoSize, numBytes));
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
        }
    
        /* Update read pointer */
        rdaddr += numBytes;
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
        wrapBit = rd & BCHP_MASK(AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP);
        if ( (rdaddr-base) >= fifoSize )
        {
            wrapBit ^= BCHP_MASK(AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP);
            rdaddr = base;
        }
#else
        wrapBit = rd & BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP);
        if ( (rdaddr-base) >= fifoSize )
        {
            wrapBit ^= BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP);
            rdaddr = base;
        }
#endif
        rdaddr |= wrapBit;
        rdaddrRegs[2*i] = rdaddr;
        
        if ( handle->settings.interleaveData == false )
        {
            /* Read registers */
            rd = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_RDADDR_REG(handle->dfifoIds[i])+BAPE_P_RINGBUFFER_STRIDE);
            wr = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_WRADDR_REG(handle->dfifoIds[i])+BAPE_P_RINGBUFFER_STRIDE);
            base = BREG_Read32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_BASEADDR_REG(handle->dfifoIds[i])+BAPE_P_RINGBUFFER_STRIDE);

#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
            /* Same toggle bit means no wrap.  Opposite toggle bits means wrap. */    
            wrap = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ^ 
                   BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP);

            /* Mask off toggle bits */
            rdaddr = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
            wraddr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR, RINGBUF_WRADDR);
#else
            /* Same toggle bit means no wrap.  Opposite toggle bits means wrap. */    
            wrap = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP) ^ 
                   BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR_WRAP);

            /* Mask off toggle bits */
            rdaddr = BCHP_GET_FIELD_DATA(rd, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR);
            wraddr = BCHP_GET_FIELD_DATA(wr, AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR, RINGBUF_WRADDR);            
#endif

            /* Check for potential overflow */
            if ( wrap )
            {
                if ( ((rdaddr-base)+numBytes) > fifoSize )
                {
                    BKNI_LeaveCriticalSection();
                    BDBG_ERR(("Invalid number of bytes provided to BAPE_OutputCapture_ConsumeData [wrap]"));
                    BDBG_ERR(("rd %#x wr %#x base %#x size %#x numBytes %#x", rd, wr, base, fifoSize, numBytes));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }
            }
            else
            {
                if ( (rdaddr+numBytes) > wraddr )
                {
                    BKNI_LeaveCriticalSection();
                    BDBG_ERR(("Invalid number of bytes provided to BAPE_OutputCapture_ConsumeData [no wrap]"));
                    BDBG_ERR(("rd %#x wr %#x base %#x size %#x numBytes %#x", rd, wr, base, fifoSize, numBytes));
                    return BERR_TRACE(BERR_INVALID_PARAMETER);
                }
            }

            /* Update read pointer */
            rdaddr += numBytes;
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
            wrapBit = rd & BCHP_MASK(AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP);
            if ( (rdaddr-base) >= fifoSize )
            {
                wrapBit ^= BCHP_MASK(AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP);
                rdaddr = base;
            }
#else
            wrapBit = rd & BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP);
            if ( (rdaddr-base) >= fifoSize )
            {
                wrapBit ^= BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP);
                rdaddr = base;
            }
#endif
            rdaddr |= wrapBit;
            rdaddrRegs[(2*i)+1] = rdaddr;
        }
    }

    /* Move the actual read pointer after validation */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        BREG_Write32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_RDADDR_REG(handle->dfifoIds[i]), rdaddrRegs[2*i]);
        if ( false == handle->settings.interleaveData )
        {
            BREG_Write32_isr(handle->deviceHandle->regHandle, BAPE_P_DFIFO_TO_RDADDR_REG(handle->dfifoIds[i])+BAPE_P_RINGBUFFER_STRIDE, rdaddrRegs[(2*i)+1]);
        }
    }

    return BERR_SUCCESS;
}

BERR_Code BAPE_DfifoGroup_P_Flush_isr(
    BAPE_DfifoGroupHandle handle
    )
{
    uint32_t regAddr, regVal, offset, wrap;
    unsigned i;

    /* To flush, first compute where the read address should be in terms
       of offset from base.  Then apply to all buffers. */
    regAddr = BAPE_P_DFIFO_TO_WRADDR_REG(handle->dfifoIds[0]);
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
    wrap = regVal & BCHP_MASK(AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP);
    offset = (regVal & BCHP_MASK(AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, RINGBUF_RDADDR)) - handle->settings.bufferInfo[0].base;    
#else
    wrap = regVal & BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR_WRAP);
    offset = (regVal & BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR, RINGBUF_RDADDR)) - handle->settings.bufferInfo[0].base;    
#endif
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        regAddr = BAPE_P_DFIFO_TO_RDADDR_REG(handle->dfifoIds[i]);
        regVal = wrap | (offset+handle->settings.bufferInfo[2*i].base);
        BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);
        if ( handle->settings.interleaveData )
        {
            regAddr += BAPE_P_RINGBUFFER_STRIDE;
            regVal = wrap | (offset+handle->settings.bufferInfo[(2*i)+1].base);
            BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);
        }
    }

    return BERR_SUCCESS;
}

BERR_Code BAPE_DfifoGroup_P_SetFullmarkInterrupt(
    BAPE_DfifoGroupHandle handle,
    BINT_CallbackFunc callback_isr,
    void *pParam1,
    int param2
    )
{
    return BAPE_P_SetDfifoFullmarkInterrupt(handle->deviceHandle,
                                            handle->dfifoIds[0],
                                            callback_isr,
                                            pParam1,
                                            param2);
}

void BAPE_DfifoGroup_P_RearmFullmarkInterrupt_isr(
    BAPE_DfifoGroupHandle handle
    )
{
    uint32_t regVal;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BKNI_ASSERT_ISR_CONTEXT();

#ifdef BCHP_AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK
    /* Clear the ESR status */
    regVal = BCHP_FIELD_DATA(AUD_FMM_BF_ESR2_H_STATUS_CLEAR, DEST_RINGBUF_0_EXCEED_FULLMARK, 1) << handle->dfifoIds[0];
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_BF_ESR2_H_STATUS_CLEAR, regVal);        
    /* Rearm */
    regVal = (1<<BCHP_SHIFT(AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK, REARM_FULLMARK))<<handle->dfifoIds[0];
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_BF_CTRL_REARM_FREEFULL_MARK, regVal);
#else
    /* Clear the ESR status */
    regVal = BCHP_FIELD_DATA(AUD_FMM_BF_ESR_ESR4_STATUS_CLEAR, DEST_RINGBUF_0_EXCEED_FULLMARK, 1) << handle->dfifoIds[0];
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_BF_ESR_ESR4_STATUS_CLEAR, regVal);        
    /* Rearm */
    regVal = (1<<BCHP_SHIFT(AUD_FMM_BF_CTRL_REARM_FULL_MARK, REARM_FULLMARK))<<handle->dfifoIds[0];
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_BF_CTRL_REARM_FULL_MARK, regVal);
#endif
}

BERR_Code BAPE_DfifoGroup_P_SetOverflowInterrupt(
    BAPE_DfifoGroupHandle handle,
    BINT_CallbackFunc callback_isr,
    void *pParam1,
    int param2
    )
{
    return BAPE_P_SetDfifoOverflowInterrupt(handle->deviceHandle,
                                            handle->dfifoIds[0],
                                            callback_isr,
                                            pParam1,
                                            param2);
}

void BAPE_DfifoGroup_P_RearmOverflowInterrupt_isr(
    BAPE_DfifoGroupHandle handle
    )
{
    uint32_t regVal;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BKNI_ASSERT_ISR_CONTEXT();

#ifdef BCHP_AUD_FMM_BF_ESR1_H_STATUS_CLEAR
    /* Clear the ESR status */
    regVal = BCHP_FIELD_DATA(AUD_FMM_BF_ESR1_H_STATUS_CLEAR, DEST_RINGBUF_0_OVERFLOW, 1) << handle->dfifoIds[0];
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_BF_ESR1_H_STATUS_CLEAR, regVal);
#else
    /* Clear the ESR status */
    regVal = BCHP_FIELD_DATA(AUD_FMM_BF_ESR_ESR2_STATUS_CLEAR, DEST_RINGBUF_0_OVERFLOW, 1) << handle->dfifoIds[0];
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_BF_ESR_ESR2_STATUS_CLEAR, regVal);
#endif
}

uint32_t BAPE_DfifoGroup_P_GetHwIndex(
    BAPE_DfifoGroupHandle handle,
    BAPE_ChannelPair channelPair
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    if ( channelPair >= handle->numChannelPairs )
    {
        return 0xFFFFFFFF;
    }
    else
    {
        return handle->dfifoIds[channelPair];
    }
}


BERR_Code BAPE_P_InitBfSw(
    BAPE_Handle handle
    )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

    BDBG_MSG(("Allocating %u SFIFO Groups", BAPE_CHIP_MAX_SFIFO_GROUPS));
    handle->sfifoGroups[0] = BKNI_Malloc(BAPE_CHIP_MAX_SFIFO_GROUPS*sizeof(BAPE_SfifoGroup));
    if ( NULL == handle->sfifoGroups[0] )
    {
        BAPE_P_UninitBfSw(handle);
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle->sfifoGroups[0], 0, BAPE_CHIP_MAX_SFIFO_GROUPS*sizeof(BAPE_SfifoGroup));
    for ( i = 1; i < BAPE_CHIP_MAX_SFIFO_GROUPS; i++ )
    {
        handle->sfifoGroups[i] = handle->sfifoGroups[0] + i;
    }
    BDBG_MSG(("Allocating %u DFIFO Groups", BAPE_CHIP_MAX_DFIFO_GROUPS));
    handle->dfifoGroups[0] = BKNI_Malloc(BAPE_CHIP_MAX_DFIFO_GROUPS*sizeof(BAPE_DfifoGroup));
    BKNI_Memset(handle->dfifoGroups[0], 0, BAPE_CHIP_MAX_DFIFO_GROUPS*sizeof(BAPE_DfifoGroup));
    if ( NULL == handle->dfifoGroups[0] )
    {
        BAPE_P_UninitBfSw(handle);
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    for ( i = 1; i < BAPE_CHIP_MAX_DFIFO_GROUPS; i++ )
    {
        handle->dfifoGroups[i] = handle->dfifoGroups[0] + i;
    }

    return BERR_SUCCESS;
}

BERR_Code BAPE_P_InitBfHw(
    BAPE_Handle handle
    )
{
    uint32_t regAddr, endAddr, regVal;
    BREG_Handle regHandle;
    unsigned i;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

    regHandle = handle->regHandle;

    BDBG_MSG(("Initializing BF registers"));

#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
    /* Clear Ringbuffer registers */
    regAddr = BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR;
    endAddr = regAddr + ((BCHP_AUD_FMM_BF_CTRL_RINGBUF_2_RDADDR - BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR) * (BAPE_CHIP_MAX_SFIFOS + BAPE_CHIP_MAX_DFIFOS));
#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_FRMSTADDR
    endAddr += ((BCHP_AUD_FMM_BF_CTRL_RINGBUF_2_FRMSTADDR-BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_FRMSTADDR) * (BAPE_CHIP_MAX_SFIFOS + BAPE_CHIP_MAX_DFIFOS));
#endif
    BDBG_MSG(("Clearing ringbuffer registers from 0x%x to 0x%x", regAddr, endAddr-4));
    while ( regAddr < endAddr )
    {
        BREG_Write32(regHandle, regAddr, 0);
        regAddr += 4;
    }
#else
    /* Clear Ringbuffer registers */
    regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR;
    endAddr = regAddr + (BAPE_P_RINGBUFFER_STRIDE * 2 * (BAPE_CHIP_MAX_SFIFOS + BAPE_CHIP_MAX_DFIFOS));
    BDBG_MSG(("Clearing ringbuffer registers from 0x%x to 0x%x", regAddr, endAddr-4));
    while ( regAddr < endAddr )
    {
        BREG_Write32(regHandle, regAddr, 0);
        regAddr += 4;
    }    
#ifdef BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_FRMSTADDR
    regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_FRMSTADDR;
    endAddr = regAddr + (8*2*(BAPE_CHIP_MAX_SFIFOS + BAPE_CHIP_MAX_DFIFOS));
    while ( regAddr < endAddr )
    {
        BREG_Write32(regHandle, regAddr, 0);
        regAddr += 4;
    }    
#endif
#endif
    BDBG_MSG(("Resetting source channel group IDs to default"));
    regAddr = BCHP_AUD_FMM_BF_CTRL_SOURCECH_GRPi_ARRAY_BASE;
    for ( i = BCHP_AUD_FMM_BF_CTRL_SOURCECH_GRPi_ARRAY_START; i <= BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_END; i++ )
    {
        regVal = BREG_Read32(regHandle, regAddr);
        regVal &= ~BCHP_MASK(AUD_FMM_BF_CTRL_SOURCECH_GRPi, GROUP_ID);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_BF_CTRL_SOURCECH_GRPi, GROUP_ID, i);
        BREG_Write32(regHandle, regAddr, regVal);
        regAddr += (BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_ELEMENT_SIZE/8);
    }

    return BERR_SUCCESS;
}


void BAPE_P_UninitBfSw(
    BAPE_Handle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

    if ( NULL != handle->sfifoGroups[0] )
    {
        BKNI_Free(handle->sfifoGroups[0]);
    }
    BKNI_Memset(handle->sfifoGroups, 0, sizeof(handle->sfifoGroups));
    if ( NULL != handle->dfifoGroups[0] )
    {
        BKNI_Free(handle->dfifoGroups[0]);
    }
    BKNI_Memset(handle->dfifoGroups, 0, sizeof(handle->dfifoGroups));
}

static bool BAPE_Sfifo_P_IsEnabled(BAPE_Handle hApe, unsigned sfifoId)
{
    uint32_t regAddr;
    bool enabled;

    regAddr = BAPE_Reg_P_GetArrayAddress(AUD_FMM_BF_CTRL_SOURCECH_CFGi, sfifoId);
    enabled = BAPE_Reg_P_ReadField(hApe, regAddr, AUD_FMM_BF_CTRL_SOURCECH_CFGi, SOURCEFIFO_ENABLE) ? true : false;
    if ( enabled )
    {
        regAddr = BAPE_Reg_P_GetArrayAddress(AUD_FMM_BF_CTRL_SOURCECH_CTRLi, sfifoId);
        enabled = BAPE_Reg_P_ReadField(hApe, regAddr, AUD_FMM_BF_CTRL_SOURCECH_CTRLi, PLAY_RUN) ? true : false;
    }

    return enabled;
}

static void BAPE_Dfifo_P_GetSfifoStatus(BAPE_Handle hApe, unsigned dfifoId, unsigned *pSfifoId, bool *pBound, bool *pEnabled)
{
    uint32_t regVal, regAddr;

    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);
    BDBG_ASSERT(dfifoId < BAPE_CHIP_MAX_DFIFOS);
    BDBG_ASSERT(NULL != pSfifoId);
    BDBG_ASSERT(NULL != pEnabled);
    BDBG_ASSERT(NULL != pBound);

    regAddr = BAPE_Reg_P_GetArrayAddress(AUD_FMM_BF_CTRL_DESTCH_CFGi, dfifoId);
    regVal = BAPE_Reg_P_Read(hApe, regAddr);

    *pSfifoId = BCHP_GET_FIELD_DATA(regVal, AUD_FMM_BF_CTRL_DESTCH_CFGi, SOURCE_FIFO_ID);
    *pBound = ((BCHP_GET_FIELD_DATA(regVal, AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_TO_SOURCEFIFO))||
               (BCHP_GET_FIELD_DATA(regVal, AUD_FMM_BF_CTRL_DESTCH_CFGi, PLAY_FROM_CAPTURE))) ? true : false;
    *pEnabled = BCHP_GET_FIELD_DATA(regVal, AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_ENABLE) ? true : false;
}

static void BAPE_Dfifo_P_SetSfifo(BAPE_Handle hApe, unsigned dfifoId, unsigned sfifoId)
{
    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);
    BDBG_ASSERT(dfifoId < BAPE_CHIP_MAX_DFIFOS);
    /* sfifoId = 0x1f is a valid setting, don't validate it */
    BAPE_Reg_P_UpdateField(hApe, BAPE_Reg_P_GetArrayAddress(AUD_FMM_BF_CTRL_DESTCH_CFGi, dfifoId), AUD_FMM_BF_CTRL_DESTCH_CFGi, SOURCE_FIFO_ID, sfifoId);
}

static void BAPE_Bf_P_SfifoStarting(BAPE_SfifoGroupHandle handle)
{
    unsigned dfifoId, sfifoId;
    bool dfifoEnabled, dfifoLinked;

    BDBG_ASSERT(NULL != handle);

    BDBG_MSG(("SFIFO Starting (Group Base %u, %u pairs)", handle->sfifoIds[0], handle->numChannelPairs));

    /* SW7425-3699: DFIFO[0] must always be programmed to a running SFIFO to
       avoid a data fetch error in the adaptive rate controllers */
    dfifoId = 0;
    BAPE_Dfifo_P_GetSfifoStatus(handle->deviceHandle, dfifoId, &sfifoId, &dfifoLinked, &dfifoEnabled);
    if ( sfifoId == 0x1f && !(dfifoLinked && dfifoEnabled) )
    {
        /* If a DFIFO is linked to a SFIFO group intentionally and enabled, don't mess with it */
        BDBG_MSG(("Reprogramming DFIFO %u for SFIFO %u", dfifoId, sfifoId));
        BAPE_Dfifo_P_SetSfifo(handle->deviceHandle, dfifoId, handle->sfifoIds[0]);
    }
    else if ( sfifoId != 0x1f )
    {
        BDBG_MSG(("DFIFO %u is already using running SFIFO %u", dfifoId, sfifoId));
    }
    else
    {
        BDBG_MSG(("DFIFO %u is linked to SFIFO %u for CAPTURE_TO_SOURCEFIFO", dfifoId, sfifoId));
    }
}

static void BAPE_Bf_P_SfifoStopping(BAPE_SfifoGroupHandle handle)
{
    unsigned sfifoId, newSfifoId, dfifoId;
    bool dfifoLinked, dfifoEnabled;

    BDBG_MSG(("SFIFO Stopping (Group Base %u, %u pairs)", handle->sfifoIds[0], handle->numChannelPairs));

    newSfifoId = BAPE_Bf_P_GetFirstRunningSfifo(handle->deviceHandle, handle->sfifoIds);

    dfifoId = 0;
    BAPE_Dfifo_P_GetSfifoStatus(handle->deviceHandle, dfifoId, &sfifoId, &dfifoLinked, &dfifoEnabled);
    if ( !(dfifoLinked && dfifoEnabled) )
    {
        /* If a DFIFO is linked to a SFIFO group intentionally and enabled, don't mess with it */
        BDBG_MSG(("Reprogramming DFIFO %u for SFIFO %u", dfifoId, newSfifoId));
        BAPE_Dfifo_P_SetSfifo(handle->deviceHandle, dfifoId, newSfifoId);
    }
    else
    {
        BDBG_MSG(("DFIFO %u is linked to SFIFO %u for CAPTURE_TO_SOURCEFIFO", dfifoId, sfifoId));
    }
}

static unsigned BAPE_Bf_P_GetFirstRunningSfifo(BAPE_Handle hApe, const uint32_t *pSfifoIds)
{
    unsigned sfifoId, i;
    const unsigned invalidSfifoIds[BAPE_ChannelPair_eMax] = {0x1f, 0x1f, 0x1f, 0x1f};

    if ( pSfifoIds == NULL )
    {
        pSfifoIds = &invalidSfifoIds[0];
    }

    for ( sfifoId = 0; sfifoId < BAPE_CHIP_MAX_SFIFOS; sfifoId++ )
    {
        if ( BAPE_Sfifo_P_IsEnabled(hApe, sfifoId) )
        {
            bool ignore = false;
            for ( i = 0; i < BAPE_ChannelPair_eMax; i++ )
            {
                if ( sfifoId == pSfifoIds[i] )
                {
                    ignore = true;
                    break;
                }
            }
            for ( i = 0; i < BAPE_CHIP_MAX_DFIFOS; i++ )
            {
                uint32_t regAddr, regVal;

                regAddr = BAPE_Reg_P_GetArrayAddress(AUD_FMM_BF_CTRL_DESTCH_CFGi, i);
                regVal = BAPE_Reg_P_Read(hApe, regAddr);
                if ( (regVal & BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, PLAY_FROM_CAPTURE)) ||
                     (regVal & BCHP_MASK(AUD_FMM_BF_CTRL_DESTCH_CFGi, CAPTURE_TO_SOURCEFIFO)) )
                {
                    /* This SFIFO is being used as part of a capture to sfifo pr play from capture operation.  Don't use it.  */
                    ignore = true;
                    break;
                }
            }
            if ( !ignore )
            {
                break;
            }
        }
    }

    if ( sfifoId >= BAPE_CHIP_MAX_SFIFOS )
    {
        BDBG_MSG(("No Running Sfifo, DFIFOs will use 0x1f"));
        return 0x1f;
    }
    else
    {
        BDBG_MSG(("First Running Sfifo is %u, DFIFOs will use 0x%x", sfifoId, sfifoId));
        return sfifoId;
    }
}

