/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_spdif_output.c $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 10/19/12 5:15p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_spdif_output.c $
 * 
 * Hydra_Software_Devel/27   10/19/12 5:15p mpeteri
 * SW7429-295: merge to main
 * 
 * Hydra_Software_Devel/SW7429-295/1   10/19/12 11:43a mpeteri
 * SW7429-295: port DTS-CD changes to 7429
 * 
 * Hydra_Software_Devel/26   9/27/12 10:18a mpeteri
 * SW7445-19: merge 7445 APE changes to main
 * 
 * Hydra_Software_Devel/SW7445-19/2   9/26/12 3:44p mpeteri
 * SW7445-19: pick up latest from Hydra
 * 
 * Hydra_Software_Devel/25   9/25/12 1:39p mpeteri
 * SW7425-3778: merge to main
 * 
 * Hydra_Software_Devel/SW7425-3778/3   9/20/12 5:44p mpeteri
 * SW7425-3778: Fix ivalid 24 bit dither using reserved bits in FW MS.
 * 
 * Hydra_Software_Devel/24   9/13/12 3:50p jgarrett
 * SW7425-3778: Merge to main branch
 * 
 * Hydra_Software_Devel/23   9/12/12 5:10p jgarrett
 * FWRAA-486: Updating cbit control register
 * 
 * Hydra_Software_Devel/22   9/11/12 3:35p jgarrett
 * SW7425-3796: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-3796/1   8/23/12 6:05p mpeteri
 * SW7425-3796: PI Changes to transmit original ES DTS-CD signal to
 * SPDIF/HDMI
 * 
 * Hydra_Software_Devel/21   8/31/12 4:13p mpeteri
 * SW7425-3778: merge to Hydra
 * 
 * Hydra_Software_Devel/SW7425-3778/2   9/13/12 4:41p mpeteri
 * SW7425-3778: change enable sequence for FW cbit formatter
 * 
 * Hydra_Software_Devel/20   8/20/12 11:41a mpeteri
 * SW7425-3646: merge to Hydra
 * 
 * Hydra_Software_Devel/SW7425-3646/1   8/10/12 6:04p mpeteri
 * SW7425-3646: updates to align SPDIF enable with HW open/close. Enable
 * proper burst handling
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_spdif_output.c $
 * 
 * Hydra_Software_Devel/27   10/19/12 5:15p mpeteri
 * SW7429-295: merge to main
 * 
 * Hydra_Software_Devel/SW7429-295/1   10/19/12 11:43a mpeteri
 * SW7429-295: port DTS-CD changes to 7429
 * 
 * Hydra_Software_Devel/26   9/27/12 10:18a mpeteri
 * SW7445-19: merge 7445 APE changes to main
 * 
 * Hydra_Software_Devel/SW7445-19/2   9/26/12 3:44p mpeteri
 * SW7445-19: pick up latest from Hydra
 * 
 * Hydra_Software_Devel/SW7445-19/1   8/17/12 5:49p mpeteri
 * SW7445-19: Add additional PLL and NCO for 7445
 * 
 * Hydra_Software_Devel/19   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/10/12 6:52p gskerl
 * SW7425-2075: Adding new FMT interfaces for 7429
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/18   3/6/12 3:04p gskerl
 * SW7425-2570: Renamed OutputPortObject substruct from connector to
 * outputPort.
 * 
 * Hydra_Software_Devel/17   2/21/12 6:40p jgarrett
 * SW7231-363: Failing gracefully if AC3+ passthrough is requested on
 * SPDIF
 * 
 * Hydra_Software_Devel/16   11/14/11 3:36p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/2   11/1/11 4:32p jgarrett
 * SW7429-18: Adding SPDIF output support for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/15   10/5/11 6:10p gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume.
 * 
 * Hydra_Software_Devel/14   9/13/11 11:06a jgarrett
 * SW7346-436: Re-applying channel status bit state on each enable call
 * 
 * Hydra_Software_Devel/13   7/8/11 4:25p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/12   5/20/11 5:16p jgarrett
 * SW7425-402: Adding error code to handle output enable callback failing.
 * 
 * Hydra_Software_Devel/11   5/3/11 10:48a gskerl
 * SW7422-354: Added index and type args to APE_P_InitOutputPort macro
 * 
 * Hydra_Software_Devel/10   4/16/11 12:16p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/9   4/6/11 1:23a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/8   3/28/11 5:55p srajapur
 * SW7422-146: [7425] Fixing dither.
 * 
 * Hydra_Software_Devel/7   3/22/11 3:01p gskerl
 * SW7422-146: Changed audio output connector callbacks to take the output
 * connector as an argument
 * 
 * Hydra_Software_Devel/6   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/5   2/17/11 5:29p gskerl
 * SW7422-146: Added comment headers at the beginning of each function.
 * Some other minor cosmetic changes.
 * 
 * Hydra_Software_Devel/4   2/16/11 4:51p jgarrett
 * SW7422-146: Coverity CID 250
 * 
 * Hydra_Software_Devel/3   2/11/11 7:38p gskerl
 * SW7422-146: Fixed compiler warning about pllclksel being used
 * uninitialized.
 * 
 * Hydra_Software_Devel/2   2/10/11 5:45p gskerl
 * SW7422-146: Changed calling sequence for the audio output setMclk_isr
 * callbacks
 * 
 * Hydra_Software_Devel/1   12/16/10 4:05p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 ***************************************************************************/

#include "bape.h"
#include "bape_priv.h"

BDBG_MODULE(bape_spdif_output);

#if BAPE_CHIP_MAX_SPDIF_OUTPUTS > 0

BDBG_OBJECT_ID(BAPE_SpdifOutput);

typedef struct BAPE_SpdifOutput
{
    BDBG_OBJECT(BAPE_SpdifOutput)
    BAPE_Handle deviceHandle;
    BAPE_SpdifOutputSettings settings;
    unsigned index;
    BAPE_OutputPortObject outputPort;
    uint32_t offset;
    unsigned sampleRate;
    struct
    {
        BAPE_MclkSource mclkSource;
        unsigned pllChannel;    /* only applies if mclkSource refers to a PLL */
        unsigned mclkFreqToFsRatio;
    } mclkInfo;
    bool enabled;
    char name[9];   /* SPDIF %d */
} BAPE_SpdifOutput;

/* Static function prototypes */
static void      BAPE_SpdifOutput_P_SetCbits_isr(BAPE_SpdifOutputHandle handle);
static BERR_Code BAPE_SpdifOutput_P_OpenHw(BAPE_SpdifOutputHandle handle);
static BERR_Code BAPE_SpdifOutput_P_CloseHw(BAPE_SpdifOutputHandle handle);
static BERR_Code BAPE_SpdifOutput_P_ApplySettings(BAPE_SpdifOutputHandle handle, 
                                                  const BAPE_SpdifOutputSettings *pSettings, bool force );

/* Output port callbacks */
static void      BAPE_SpdifOutput_P_SetTimingParams_isr(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase);
static void      BAPE_SpdifOutput_P_SetMute(BAPE_OutputPort output, bool muted, bool sync);
static BERR_Code BAPE_SpdifOutput_P_Enable(BAPE_OutputPort output);
static void      BAPE_SpdifOutput_P_Disable(BAPE_OutputPort output);
static void      BAPE_SpdifOutput_P_SetMclk_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio);

#if BAPE_CHIP_MAX_SPDIF_OUTPUTS  > 1
    #define  GET_SPDIF_REG_ADDR2(prefix,idx)         (prefix##0         + (prefix##1         - prefix##0        ) * idx )
    #define  GET_SPDIF_REG_ADDR3(prefix,idx,suffix)  (prefix##0##suffix + (prefix##1##suffix - prefix##0##suffix) * idx )
    #define  GET_SPDIF_STREAM_ID(idx) ((idx == 1)?0:(BDBG_ASSERT(0),0))
#else
    #define  GET_SPDIF_REG_ADDR2(prefix,idx       )  (prefix##0         )
    #define  GET_SPDIF_REG_ADDR3(prefix,idx,suffix)  (prefix##0##suffix )
    #define  GET_SPDIF_STREAM_ID(idx) (0)
#endif

/* Implementations for Legacy vs. New [7429] Hardware */
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
#include "bchp_aud_fmm_iop_out_spdif_0.h"
static void      BAPE_SpdifOutput_P_SetCbits_IopOut_isr(BAPE_SpdifOutputHandle handle);
static void      BAPE_SpdifOutput_P_SetMute_IopOut(BAPE_OutputPort output, bool muted, bool sync);
static BERR_Code BAPE_SpdifOutput_P_Enable_IopOut(BAPE_OutputPort output);
static void      BAPE_SpdifOutput_P_Disable_IopOut(BAPE_OutputPort output);
static void      BAPE_SpdifOutput_P_SetMclk_IopOut_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio);
static BERR_Code BAPE_SpdifOutput_P_OpenHw_IopOut(BAPE_SpdifOutputHandle handle);
static BERR_Code BAPE_SpdifOutput_P_CloseHw_IopOut(BAPE_SpdifOutputHandle handle);
static BERR_Code BAPE_SpdifOutput_P_ApplySettings_IopOut(BAPE_SpdifOutputHandle handle, 
                                                         const BAPE_SpdifOutputSettings *pSettings, bool force);
#else
static void      BAPE_SpdifOutput_P_SetCbits_Legacy_isr(BAPE_SpdifOutputHandle handle);
static void      BAPE_SpdifOutput_P_SetMute_Legacy(BAPE_OutputPort output, bool muted, bool sync);
static BERR_Code BAPE_SpdifOutput_P_Enable_Legacy(BAPE_OutputPort output);
static void      BAPE_SpdifOutput_P_Disable_Legacy(BAPE_OutputPort output);
static void      BAPE_SpdifOutput_P_SetMclk_Legacy_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio);
static BERR_Code BAPE_SpdifOutput_P_OpenHw_Legacy(BAPE_SpdifOutputHandle handle);
static BERR_Code BAPE_SpdifOutput_P_CloseHw_Legacy(BAPE_SpdifOutputHandle handle);
static BERR_Code BAPE_SpdifOutput_P_ApplySettings_Legacy(BAPE_SpdifOutputHandle handle, 
                                                         const BAPE_SpdifOutputSettings *pSettings, bool force);

#define ALWAYS_COMP_FOR_FW_CBITS        0
/* Use the second output of the last mixer.  It will never be used.  */
#define BAPE_SPDIF_INVALID_FCI_SOURCE (0x100|((BAPE_CHIP_MAX_MIXERS*2)+1))
#endif

/***************************************************************************
        Public APIs: From bape_output.h
***************************************************************************/
void BAPE_SpdifOutput_GetDefaultSettings(
    BAPE_SpdifOutputSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->stereoMode = BAPE_StereoMode_eLeftRight;
    pSettings->ditherEnabled = true;
    pSettings->limitTo16Bits = false;
    pSettings->underflowBurst = BAPE_SpdifBurstType_ePause;
    pSettings->useRawChannelStatus = false;
    /* channel status is initialized to zero for all fields */
}

/**************************************************************************/

BERR_Code BAPE_SpdifOutput_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_SpdifOutputSettings *pSettings,
    BAPE_SpdifOutputHandle *pHandle             /* [out] */
    )
{
    BERR_Code errCode;
    BAPE_SpdifOutputHandle handle;
    BAPE_SpdifOutputSettings defaultSettings;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);
    
    BDBG_MSG(("%s: Opening SPDIF Output: %u", __FUNCTION__, index));

    *pHandle = NULL;

    if ( index >= BAPE_CHIP_MAX_SPDIF_OUTPUTS )
    {
        BDBG_ERR(("Request to open SPDIF output %d but chip only has %u SPDIF outputs", index, BAPE_CHIP_MAX_SPDIF_OUTPUTS));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( deviceHandle->spdifOutputs[index] )
    {
        BDBG_ERR(("SPDIF output %d already open", index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Allocate the device structure, then fill in all the fields. */
    handle = BKNI_Malloc(sizeof(BAPE_SpdifOutput));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    BKNI_Memset(handle, 0, sizeof(BAPE_SpdifOutput));
    BDBG_OBJECT_SET(handle, BAPE_SpdifOutput);
    handle->deviceHandle = deviceHandle;
    handle->index = index;
    BAPE_P_InitOutputPort(&handle->outputPort, BAPE_OutputPortType_eSpdifOutput, index, handle);
    BAPE_FMT_P_EnableSource(&handle->outputPort.capabilities, BAPE_DataSource_eFci);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_eIec61937);
    handle->outputPort.pllRequired = true;
    handle->outputPort.setTimingParams_isr = BAPE_SpdifOutput_P_SetTimingParams_isr;
    handle->outputPort.setMute = BAPE_SpdifOutput_P_SetMute;
    handle->outputPort.enable = BAPE_SpdifOutput_P_Enable;
    handle->outputPort.disable = BAPE_SpdifOutput_P_Disable;
    handle->outputPort.setMclk_isr = BAPE_SpdifOutput_P_SetMclk_isr;
    BKNI_Snprintf(handle->name, sizeof(handle->name), "SPDIF %u", index);
    handle->outputPort.pName = handle->name;

    /* Currently we only support one SPDIF, this can be expanded later if we support more */
    #if BAPE_CHIP_MAX_SPDIF_OUTPUTS > 1
        #error "Need to support more SPDIF outputs"
    #endif
    handle->offset = 0;

    /* Setup to 48k, muted by default */
    BKNI_EnterCriticalSection();
    BAPE_SpdifOutput_P_SetTimingParams_isr(&handle->outputPort, 48000, BAVC_Timebase_e0);
    BKNI_LeaveCriticalSection();

    /* Init to specified settings */
    if ( NULL == pSettings )
    {
        BAPE_SpdifOutput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    errCode = BAPE_SpdifOutput_P_OpenHw(handle);
    if ( errCode )
    {
        BAPE_SpdifOutput_Close(handle);
        return BERR_TRACE(errCode);
    }

    /* mute */
    BAPE_SpdifOutput_P_SetMute(&handle->outputPort, true, false);

    /* Initialize hardware before applying settings */
    BKNI_EnterCriticalSection();
    BAPE_SpdifOutput_P_SetMclk_isr(&handle->outputPort, BAPE_MclkSource_ePll0, 0, BAPE_BASE_PLL_TO_FS_RATIO);
    BKNI_LeaveCriticalSection();    

    errCode = BAPE_SpdifOutput_P_ApplySettings(handle, pSettings, true);   /* true => force update of HW */
    if ( errCode )
    {
        BAPE_SpdifOutput_Close(handle);
        return BERR_TRACE(errCode);
    }

    *pHandle = handle;
    handle->deviceHandle->spdifOutputs[index] = handle;
    return BERR_SUCCESS;
}

/**************************************************************************/

void BAPE_SpdifOutput_Close(
    BAPE_SpdifOutputHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    /* Make sure we're not still connected to anything */
    if ( handle->outputPort.mixer )
    {
        BDBG_ERR(("Cannot close SPDIF output %p (%d), still connected to mixer %p", handle, handle->index, handle->outputPort.mixer));
        BDBG_ASSERT(NULL == handle->outputPort.mixer);
        return;
    }

    BAPE_SpdifOutput_P_CloseHw(handle);

    BDBG_ASSERT(handle->index < BAPE_CHIP_MAX_SPDIF_OUTPUTS);
    handle->deviceHandle->spdifOutputs[handle->index] = NULL;
    BDBG_OBJECT_DESTROY(handle, BAPE_SpdifOutput);
    BKNI_Free(handle);    
}

/**************************************************************************/

void BAPE_SpdifOutput_GetSettings(
    BAPE_SpdifOutputHandle handle,
    BAPE_SpdifOutputSettings *pSettings     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

/**************************************************************************/

BERR_Code BAPE_SpdifOutput_SetSettings(
    BAPE_SpdifOutputHandle handle,
    const BAPE_SpdifOutputSettings *pSettings
    )
{
    BERR_Code   errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);
    BDBG_ASSERT(NULL != pSettings);

    errCode = BAPE_SpdifOutput_P_ApplySettings(handle, pSettings, false); /* false => don't force (only update HW for changes) */
    return errCode;
}

/**************************************************************************/

void BAPE_SpdifOutput_GetOutputPort(
    BAPE_SpdifOutputHandle handle,
    BAPE_OutputPort *pOutputPort        /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);
    BDBG_ASSERT(NULL != pOutputPort);
    *pOutputPort = &handle->outputPort;
}

/***************************************************************************
        BAPE Internal APIs: From bape_fmm_priv.h
***************************************************************************/

BERR_Code BAPE_SpdifOutput_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    spdifOutputIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    /* For each opened SpdifOutput, call the functions necessary to restore the hardware to it's appropriate state. */
    for ( spdifOutputIndex=0 ; spdifOutputIndex<BAPE_CHIP_MAX_SPDIF_OUTPUTS ; spdifOutputIndex++ )
    {
        if ( bapeHandle->spdifOutputs[spdifOutputIndex] )       /* If this SpdifOutput is open... */
        {
            BAPE_SpdifOutputHandle hSpdifOutput = bapeHandle->spdifOutputs[spdifOutputIndex];

            /* Put the HW into the generic open state. */
            errCode = BAPE_SpdifOutput_P_OpenHw(hSpdifOutput);
            if ( errCode ) return BERR_TRACE(errCode);
            
            /* Now apply changes for the settings struct. */
            errCode = BAPE_SpdifOutput_P_ApplySettings(hSpdifOutput, &hSpdifOutput->settings, true);   /* true => force update of HW */
            if ( errCode ) return BERR_TRACE(errCode);

            /* Now restore the dynamic stuff from the values saved in the device struct. */
            BKNI_EnterCriticalSection();

                BAPE_SpdifOutput_P_SetTimingParams_isr(&hSpdifOutput->outputPort, 
                                                     hSpdifOutput->sampleRate, 
                                                     0);    /* timebase is unused, 0 is dummy value */
                BAPE_SpdifOutput_P_SetMclk_isr(&hSpdifOutput->outputPort,
                                             hSpdifOutput->mclkInfo.mclkSource,
                                             hSpdifOutput->mclkInfo.pllChannel,
                                             hSpdifOutput->mclkInfo.mclkFreqToFsRatio );
            BKNI_LeaveCriticalSection();

        }
    }
    return errCode;
}

/***************************************************************************
        Private callbacks: Protyped above
***************************************************************************/

static void BAPE_SpdifOutput_P_SetTimingParams_isr(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase)
{
    BAPE_SpdifOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    BSTD_UNUSED(timebase);  /* SPDIF doesn't care */

    handle->sampleRate = sampleRate;

    BAPE_SpdifOutput_P_SetCbits_isr(handle);
}

/**************************************************************************/

static void BAPE_SpdifOutput_P_SetMute(BAPE_OutputPort output, bool muted, bool sync)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
    BAPE_SpdifOutput_P_SetMute_IopOut(output, muted, sync);
#else
    BAPE_SpdifOutput_P_SetMute_Legacy(output, muted, sync);
#endif
}

/**************************************************************************/

static BERR_Code BAPE_SpdifOutput_P_Enable(BAPE_OutputPort output)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
    return BAPE_SpdifOutput_P_Enable_IopOut(output);
#else
    return BAPE_SpdifOutput_P_Enable_Legacy(output);
#endif
}

/**************************************************************************/

static void BAPE_SpdifOutput_P_Disable(BAPE_OutputPort output)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
    BAPE_SpdifOutput_P_Disable_IopOut(output);
#else
    BAPE_SpdifOutput_P_Disable_Legacy(output);
#endif
}

/**************************************************************************/

static void BAPE_SpdifOutput_P_SetMclk_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
    BAPE_SpdifOutput_P_SetMclk_IopOut_isr(output, mclkSource, pllChannel, mclkFreqToFsRatio);
#else
    BAPE_SpdifOutput_P_SetMclk_Legacy_isr(output, mclkSource, pllChannel, mclkFreqToFsRatio);
#endif
}

/***************************************************************************
        Private functions: Protyped above
***************************************************************************/

static void BAPE_SpdifOutput_P_SetCbits_isr(BAPE_SpdifOutputHandle handle)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
    BAPE_SpdifOutput_P_SetCbits_IopOut_isr(handle);
#else
    BAPE_SpdifOutput_P_SetCbits_Legacy_isr(handle);
#endif
}

/**************************************************************************/

static BERR_Code BAPE_SpdifOutput_P_OpenHw(BAPE_SpdifOutputHandle handle)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
    return BAPE_SpdifOutput_P_OpenHw_IopOut(handle);
#else
    return BAPE_SpdifOutput_P_OpenHw_Legacy(handle);
#endif
}

/**************************************************************************/

static BERR_Code BAPE_SpdifOutput_P_CloseHw(BAPE_SpdifOutputHandle handle)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
    return BAPE_SpdifOutput_P_CloseHw_IopOut(handle);
#else
    return BAPE_SpdifOutput_P_CloseHw_Legacy(handle);
#endif
}

/**************************************************************************/

static BERR_Code BAPE_SpdifOutput_P_ApplySettings(
    BAPE_SpdifOutputHandle handle,
    const BAPE_SpdifOutputSettings *pSettings,
    bool force
    )
{
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
    return BAPE_SpdifOutput_P_ApplySettings_IopOut(handle, pSettings, force);
#else
    return BAPE_SpdifOutput_P_ApplySettings_Legacy(handle, pSettings, force);
#endif
}

#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
/**************************************************************************
7429-style RDB
**************************************************************************/
static void BAPE_SpdifOutput_P_SetMute_IopOut(BAPE_OutputPort output, bool muted, bool sync)
{
    BAPE_SpdifOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    /* Only support one now */
    BDBG_ASSERT(handle->offset == 0);

    /* Must manipulate CSTAT registers in critical section */
    BKNI_EnterCriticalSection();

    if ( muted )
    {
        BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, OVERWRITE_DATA, Enable);
    }
    else
    {
        BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, OVERWRITE_DATA, Disable);
    }

    BKNI_LeaveCriticalSection();

    BSTD_UNUSED(sync);
}

/**************************************************************************/

static BERR_Code BAPE_SpdifOutput_P_Enable_IopOut(BAPE_OutputPort output)
{
    BAPE_Reg_P_FieldList regFieldList;
    BAPE_SpdifOutputHandle handle;
    BAPE_MixerHandle mixer;
    unsigned numChannelPairs = 0;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);
    BDBG_ASSERT(false == handle->enabled);
    BDBG_ASSERT(NULL != output->mixer);

    mixer = output->mixer;

    BDBG_MSG(("Enabling %s", handle->name));

    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(BAPE_Mixer_P_GetOutputFormat(mixer));

    BDBG_ASSERT(numChannelPairs <= 1);

    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, GROUP_ID, 0);
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, CHANNEL_GROUPING, 1);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, STREAM_BIT_RESOLUTION, Res_24_Bit); 
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, FCI_ID, output->sourceMixerFci.ids[0]);
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0);        

    /* Update CBITS */
    BKNI_EnterCriticalSection();
    BAPE_SpdifOutput_P_SetCbits_isr(handle);
    BKNI_LeaveCriticalSection();

    /* Enable the interface. */
    BAPE_Reg_P_UpdateField(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, ENA, 1);

    handle->enabled = true;
    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_SpdifOutput_P_Disable_IopOut(BAPE_OutputPort output)
{
    BAPE_SpdifOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    BDBG_ASSERT(true == handle->enabled);

    /* Disable the interface */
    BAPE_Reg_P_UpdateField(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, ENA, 0);

    /* Reset source FCI to Invalid */
    BAPE_Reg_P_UpdateField(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, FCI_ID, BAPE_FCI_ID_INVALID);

    handle->enabled = false;
}

/**************************************************************************/

static void BAPE_SpdifOutput_P_SetMclk_IopOut_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio)
{
    BAPE_Reg_P_FieldList regFieldList;

    BAPE_SpdifOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);
    BDBG_ASSERT(handle->offset == 0);

    /* Save the settings in case we need to re-apply them later. */
    handle->mclkInfo.mclkSource         = mclkSource;
    handle->mclkInfo.pllChannel         = pllChannel;
    handle->mclkInfo.mclkFreqToFsRatio  = mclkFreqToFsRatio;

    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);

    switch ( mclkSource )
    {
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0_PLLCLKSEL_PLL0_ch1
    case BAPE_MclkSource_ePll0:
        switch ( pllChannel )
        {
        case 0: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, PLL0_ch1); break;
        case 1: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, PLL0_ch2); break;
        case 2: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, PLL0_ch3); break;                
        default: (void) BERR_TRACE(BERR_NOT_SUPPORTED); break;
        }
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0_PLLCLKSEL_PLL1_ch1
    case BAPE_MclkSource_ePll1:
        switch ( pllChannel )
        {
        case 0: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, PLL1_ch1); break;
        case 1: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, PLL1_ch2); break;
        case 2: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, PLL1_ch3); break;                
        default: (void) BERR_TRACE(BERR_NOT_SUPPORTED); break;
        }
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0_PLLCLKSEL_PLL2_ch1
    case BAPE_MclkSource_ePll2:
        switch ( pllChannel )
        {
        case 0: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, PLL2_ch1); break;
        case 1: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, PLL2_ch2); break;
        case 2: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, PLL2_ch3); break;                
        default: (void) BERR_TRACE(BERR_NOT_SUPPORTED); break;
        }
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen0
    case BAPE_MclkSource_eNco0:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen0); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen1
    case BAPE_MclkSource_eNco1:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen1); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen2
    case BAPE_MclkSource_eNco2:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen2); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen3
    case BAPE_MclkSource_eNco3:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen3); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen4
    case BAPE_MclkSource_eNco4:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen4); 
        break;
#endif
    default:
        BDBG_ERR(("Unsupported clock source %u for SPDIF %u", mclkSource, handle->index));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    switch ( mclkFreqToFsRatio )
    {
    case 128: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, MCLK_RATE, MCLK_128fs_SCLK_64fs); break;
    case 256: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, MCLK_RATE, MCLK_256fs_SCLK_64fs); break;
    case 384: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, MCLK_RATE, MCLK_384fs_SCLK_64fs); break;
    case 512: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, MCLK_RATE, MCLK_512fs_SCLK_64fs); break;
    default:
        BDBG_ERR(("Unsupported MCLK Rate of %uFs", mclkFreqToFsRatio));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        break;
    }
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0);
}

/***************************************************************************
        Private functions: Protyped above
***************************************************************************/

static void BAPE_SpdifOutput_P_SetCbits_IopOut_isr(BAPE_SpdifOutputHandle handle)
{
    BAPE_Reg_P_FieldList regFieldList;
    uint32_t regAddr, regVal;
    unsigned validity = 0;
    unsigned dither = 0;
    BAPE_DataType dataType = BAPE_DataType_ePcmStereo;
    bool compressed = false;
    bool compressedAsPcm = false;

    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    if ( handle->outputPort.mixer )
    {
        const BAPE_FMT_Descriptor     *pBfd = BAPE_Mixer_P_GetOutputFormat(handle->outputPort.mixer);

        dataType = pBfd->type;
        compressed = BAPE_FMT_P_IsCompressed(pBfd);
        compressedAsPcm = BAPE_FMT_P_IsDtsCdCompressed(pBfd);
    }

    BDBG_MSG(("Set SPDIF CBITS SR %u", handle->sampleRate));

    /* Only set validity if we're outputting compressed in a non-high-bitrate mode. */
    if ( compressed && !compressedAsPcm && handle->sampleRate != 192000 && handle->sampleRate != 176400 )
    {
        validity = 1;
    }
    else if ( !compressed && !compressedAsPcm )
    {
        if ( handle->settings.ditherEnabled )
        {
            dither = 1;
        }
    }

    /* Hold while we update the other bits */
    BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, HOLD_CSTAT, Hold);

    /* Tell the HW to hold the current CBITS and set dither correctly */
    regAddr = BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL;
    BAPE_Reg_P_InitFieldList_isr(handle->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, HOLD_CSTAT, Hold);
    BAPE_Reg_P_AddToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, DITHER_ENA, dither);
    BAPE_Reg_P_AddToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, VALIDITY, validity);
    BAPE_Reg_P_ApplyFieldList_isr(&regFieldList, regAddr);

    /* Program channel status */
    if ( handle->settings.useRawChannelStatus )
    {
        regVal = 
            (uint32_t)handle->settings.rawChannelStatus[0] |
            (((uint32_t)handle->settings.rawChannelStatus[1])<<8) |
            (((uint32_t)handle->settings.rawChannelStatus[2])<<16) |
            (((uint32_t)handle->settings.rawChannelStatus[3])<<24);
        BAPE_Reg_P_Write_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_0, regVal);
        regVal = (uint32_t)handle->settings.rawChannelStatus[4];
        BAPE_Reg_P_Write_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_1, regVal);
    }
    else 
    {
        BAPE_Spdif_P_ChannelStatusBits cbits;

        BAPE_P_MapSpdifChannelStatusToBits_isr(&handle->outputPort, &handle->settings.channelStatus, &cbits);
        BAPE_Reg_P_Write_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_0, cbits.bits[0]);
        BAPE_Reg_P_Write_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_1, cbits.bits[1]);
    }

    /* Reload the new channel status in HW */
    BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, HOLD_CSTAT, Update);

    return;
}

/**************************************************************************/

static BERR_Code BAPE_SpdifOutput_P_OpenHw_IopOut(BAPE_SpdifOutputHandle handle)
{
    BERR_Code            errCode = BERR_SUCCESS;
    BAPE_Handle          deviceHandle;
    BAPE_Reg_P_FieldList regFieldList;

    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    /* Enable the clock and data while opening the output. */

    BAPE_Reg_P_InitFieldList(deviceHandle, &regFieldList);

    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, CLOCK_ENABLE, Enable);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, DATA_ENABLE, Enable);

    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG);

    return errCode;
}

/**************************************************************************/

static BERR_Code BAPE_SpdifOutput_P_CloseHw_IopOut(BAPE_SpdifOutputHandle handle)
{
    BERR_Code            errCode = BERR_SUCCESS;
    BAPE_Handle          deviceHandle;
    BAPE_Reg_P_FieldList regFieldList;

    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    /* Enable the clock and data while opening the output. */

    BAPE_Reg_P_InitFieldList(deviceHandle, &regFieldList);

    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, CLOCK_ENABLE, Disable);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, DATA_ENABLE, Disable);

    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG);

    return errCode;
}

static BERR_Code BAPE_SpdifOutput_P_ApplySettings_IopOut(
    BAPE_SpdifOutputHandle handle,
    const BAPE_SpdifOutputSettings *pSettings,
    bool force
    )
{
    BAPE_Reg_P_FieldList regFieldList;
    BAPE_Handle          deviceHandle;

    BSTD_UNUSED(force); /* Not used because HW gets updated unconditionally */

    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);
    BDBG_ASSERT(NULL != pSettings);
    
    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    BAPE_Reg_P_InitFieldList(deviceHandle, &regFieldList);

    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, LIMIT_TO_16_BITS, (pSettings->limitTo16Bits) ? 1 : 0);

    switch ( pSettings->stereoMode )
    {
    default:
    case BAPE_StereoMode_eLeftRight:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, LR_SELECT, Normal);
        break;
    case BAPE_StereoMode_eLeftLeft:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, LR_SELECT, Both_Get_Left);
        break;
    case BAPE_StereoMode_eRightRight:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, LR_SELECT, Both_Get_Right);
        break;
    case BAPE_StereoMode_eRightLeft:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, LR_SELECT, Swap);
        break;        
    }

    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG);

    handle->settings = *pSettings;

    /* Remaining fields are handled by MS regs.  Must modify those in critical section. */    
    BKNI_EnterCriticalSection();
    BAPE_SpdifOutput_P_SetCbits_isr(handle);
    BKNI_LeaveCriticalSection();

    return BERR_SUCCESS;
}
#else
/**************************************************************************
Legacy HW routines
**************************************************************************/

static void BAPE_SpdifOutput_P_SetMute_Legacy(BAPE_OutputPort output, bool muted, bool sync)
{
    BAPE_SpdifOutputHandle handle;
    uint32_t regVal;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    /* Only support one now */
    BDBG_ASSERT(handle->offset == 0);

    /* Must manipulate MS registers in critical section */
    BKNI_EnterCriticalSection();
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0);

    /* We always use FW channel status, so set override only to mute */
    regVal &= ~BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, OVERWRITE_DATA);
    if ( muted )
    {
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, OVERWRITE_DATA, Enable);
    }
    else
    {
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, OVERWRITE_DATA, Disable);
    }
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, regVal);

    BKNI_LeaveCriticalSection();

    BSTD_UNUSED(sync);
}

/**************************************************************************/

static BERR_Code BAPE_SpdifOutput_P_Enable_Legacy(BAPE_OutputPort output)
{
    BAPE_SpdifOutputHandle handle;
    uint32_t regVal;
    BAPE_IopStreamSettings streamSettings;
    BERR_Code errCode;
    unsigned streamId;

    
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    streamId = GET_SPDIF_STREAM_ID(handle->index);
    BDBG_MSG(("Enabling %s [stream %u]", handle->name, streamId));
    
    /* Write source FCI to IOP */
    BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    streamSettings.resolution = handle->settings.limitTo16Bits?16:24;
    streamSettings.input = output->sourceMixerFci.ids[BAPE_ChannelPair_eLeftRight];         /* Take source FCI provided from mixer */
    errCode = BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    BAPE_SpdifOutput_P_SetMute(output, false, false);

    /* Enable this stream in the MS FW */
    BKNI_EnterCriticalSection();
    /* Make sure CBITS are correct for current format */
    BAPE_SpdifOutput_P_SetCbits_isr(handle);

    /* enable stream 0 */
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0);
    regVal &= ~BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, STREAM_ENA);
    regVal |= (BCHP_FIELD_ENUM(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, STREAM_ENA, Enable));
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, regVal);
    BKNI_LeaveCriticalSection();

    handle->enabled = true;
    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_SpdifOutput_P_Disable_Legacy(BAPE_OutputPort output)
{
    BAPE_SpdifOutputHandle handle;
    uint32_t regVal;
    BAPE_IopStreamSettings streamSettings;
    BERR_Code errCode;
    unsigned streamId;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    if ( !handle->enabled )
    {
        return;
    }

    streamId = GET_SPDIF_STREAM_ID(handle->index);
    BDBG_MSG(("Disabling %s [stream %u]", handle->name, streamId));

    BAPE_SpdifOutput_P_SetMute(output, true, true);

    /* Disable this stream in the MS FW */
    BKNI_EnterCriticalSection();
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0);
    regVal &= ~BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, STREAM_ENA);
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, regVal);
    BKNI_LeaveCriticalSection();

    /* Reset source FCI to Invalid */
    BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    streamSettings.input = BAPE_SPDIF_INVALID_FCI_SOURCE;
    errCode = BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    BDBG_ASSERT(BERR_SUCCESS == errCode);

    handle->enabled = false;
}

/**************************************************************************/

static void BAPE_SpdifOutput_P_SetMclk_Legacy_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio)
{
    uint32_t regAddr, regVal;
    uint32_t pllclksel, mclkRate;

    BAPE_SpdifOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);
    BDBG_ASSERT(handle->offset == 0);

    /* Save the settings in case we need to re-apply them later. */
    handle->mclkInfo.mclkSource         = mclkSource;
    handle->mclkInfo.pllChannel         = pllChannel;
    handle->mclkInfo.mclkFreqToFsRatio  = mclkFreqToFsRatio;

    switch ( mclkSource )
    {
    /* SPDIF Timing */
    #if BAPE_CHIP_MAX_PLLS > 0
    case BAPE_MclkSource_ePll0:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL0_ch1 + pllChannel;
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 1
    case BAPE_MclkSource_ePll1:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL1_ch1 + pllChannel;
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 2
    case BAPE_MclkSource_ePll2:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_PLL2_ch1 + pllChannel;
        break;
    #endif
    
    /* DAC Timing */
    #if BAPE_CHIP_MAX_DACS > 0
    case BAPE_MclkSource_eHifidac0:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Hifidac0;
        break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 1
    case BAPE_MclkSource_eHifidac1:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Hifidac1;
        break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 2
    case BAPE_MclkSource_eHifidac2:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Hifidac2;
        break;
    #endif
    
    /* NCO Timing */
    #if BAPE_CHIP_MAX_NCOS > 0
    case BAPE_MclkSource_eNco0:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Mclk_gen0;
        break;
    #endif
    #if BAPE_CHIP_MAX_NCOS > 1
    case BAPE_MclkSource_eNco1:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Mclk_gen1;
        break;
    #endif
    #if BAPE_CHIP_MAX_NCOS > 2
    case BAPE_MclkSource_eNco2:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0_PLLCLKSEL_Mclk_gen2;
        break;
    #endif
    
    default:
        BDBG_ERR(("mclkSource (%u) doesn't refer to a valid PLL or DAC", mclkSource));
        BDBG_ASSERT(false);     /* something went wrong somewhere! */
        return;
    }

    /*  PLLs are the recommended timing source for SPDIF, so print a warning if they're using a DAC rate manager. */
    if ( BAPE_MCLKSOURCE_IS_DAC(mclkSource) )
    {
        BDBG_WRN(("DAC timing source has been selected for SPDIF output %u.", handle->index));
        BDBG_WRN(("It is strongly recommended to place SPDIF and DAC outputs on separate mixers."));
    }
    else if ( BAPE_MCLKSOURCE_IS_NCO(mclkSource) )
    {
        BDBG_WRN(("NCO timing source has been selected for SPDIF output %u.", handle->index));
        BDBG_WRN(("It is strongly recommended to use PLL timing for SPDIF."));
    }


    /* Tell the output formatter how fast our mclk is. */
    mclkRate = mclkFreqToFsRatio / ( 128 );  /* mclkRate (for SPDIF) is in multiples of 128Fs */

    /* Choose the register for the appropriate output. */
    regAddr = GET_SPDIF_REG_ADDR2(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF, handle->index);

    /* Read the register and clear the fields that we're going to fill in. */
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, PLLCLKSEL)|
                BCHP_MASK(AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, MCLK_RATE));

    /* Fill in the MCLK_RATE and PLLCLKSEL fields. */
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, MCLK_RATE, mclkRate);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0, PLLCLKSEL, pllclksel);

    /* Then write it to the reg. */
    BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);
}

/***************************************************************************
        Private functions: Protyped above
***************************************************************************/

static void BAPE_SpdifOutput_P_SetCbits_Legacy_isr(BAPE_SpdifOutputHandle handle)
{
    uint32_t regAddr, regVal, cbitAddr, cbitVal, cbitBufAddr;
    unsigned i;
    BAPE_DataType dataType = BAPE_DataType_ePcmStereo;
    bool compressed = false;
    bool compressedAsPcm = false;
    uint16_t cbits[3];

    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    if ( handle->outputPort.mixer )
    {
        const BAPE_FMT_Descriptor     *pBfd = BAPE_Mixer_P_GetOutputFormat(handle->outputPort.mixer);

        dataType = pBfd->type;
        compressed = BAPE_FMT_P_IsCompressed(pBfd);
        compressedAsPcm = BAPE_FMT_P_IsDtsCdCompressed(pBfd);
    }

    BDBG_MSG(("Configuring CBITS for %s", compressed?"Compressed":"PCM"));

    regAddr = BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0;
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~( (BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, CHAN_OVERRIDE)) 
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, DITHER_ENA)) 
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, VALIDITY)) 
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, COMP_OR_LINEAR))
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, SPDIF_OR_PCM))
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, RESERVED)) /* clear reserved Dither bits */
                );

    regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, SPDIF_OR_PCM, 1));
    if ( handle->settings.channelStatus.separateLeftRight )
    {
        regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, CHAN_OVERRIDE, 1));
    }

    BDBG_MSG(("Compressed %u CompressedAsPcm %u", compressed, compressedAsPcm));

    if ( compressed && !compressedAsPcm )
    {
        regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, VALIDITY, 1));
        regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, COMP_OR_LINEAR, 1));
    } 
    else if ( handle->settings.ditherEnabled && !compressed )
    {
        regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, DITHER_ENA, 1));
        /*
        AUD_FMM_MS_CTRL_FW_STREAM_CTRL_x[15:12]	Dither width 	Dither sample 
                                              0	      RES_16         0x000100
                                              1	      RES_17         0x000080
                                              2	      RES_18         0x000040
                                              3	      RES_19         0x000020
                                              4	      RES_20         0x000010
                                              5	      RES_21         0x000008
                                              6	      RES_22         0x000004
                                              7	      RES_23         0x000002
                                              8	      RES_24         0x000001
        */    
        if ( !handle->settings.limitTo16Bits )
        {
            regVal |= 8 << 12;
        }
    }
    
#if ALWAYS_COMP_FOR_FW_CBITS
    regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, COMP_OR_LINEAR, 1));
#endif

    /* RAP programs channel status bits here. */
    if ( handle->settings.useRawChannelStatus )
    {
        cbits[0] = (uint16_t)handle->settings.rawChannelStatus[0] |
                (((uint16_t)handle->settings.rawChannelStatus[1])<<8);
        cbits[1] = ((uint32_t)handle->settings.rawChannelStatus[2]) |
            (((uint32_t)handle->settings.rawChannelStatus[3])<<8);
        cbits[2] = 0;
    }
    else if ( handle->settings.channelStatus.professional )
    {
        cbits[0] = 0xc000 | (BAPE_P_GetSampleRateCstatCode(handle->sampleRate)<<8);
        cbits[1] = cbits[2] = 0;
    }
    else
    {
        /* Refer to IEC 60958-3 (5.2.2) */
        cbits[0] = (((uint16_t)handle->settings.channelStatus.categoryCode)<<8) |
                   (((uint16_t)handle->settings.channelStatus.formatInformation)<<3);
        if ( !handle->settings.channelStatus.copyright )
        {
            cbits[0] |= 1<<2;   /* Copyright = 0, no copyright = 1 in bit 2 */
        }
        if ( compressed && !compressedAsPcm )
        {
            cbits[0] |= 1<<1;   /* Linear/PCM */
        }
        cbits[1] = (((uint16_t)handle->settings.channelStatus.clockAccuracy)<<12) |
                    (BAPE_P_GetSampleRateCstatCode(handle->sampleRate)<<8) |
                    ((uint16_t)handle->settings.channelStatus.sourceNumber) |
                    (((uint16_t)handle->settings.channelStatus.separateLeftRight)<<4);
        cbits[2] = (((uint16_t)handle->settings.channelStatus.cgmsA)<<8);
    }
    
    /* Load channel status bits into FW registers */
    cbitAddr = BCHP_AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0;
    cbitVal = BREG_Read32_isr(handle->deviceHandle->regHandle, cbitAddr);
    if ( 0x50 == BCHP_GET_FIELD_DATA(cbitVal, AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0, OFFSET) )
    {
        /* Using ping buffer - Load pong */
        cbitBufAddr = BCHP_AUD_FMM_MS_CTRL_FW_CBITSi_ARRAY_BASE + 48;
    }
    else
    {
        /* Using pong buffer - Load ping */
        cbitBufAddr = BCHP_AUD_FMM_MS_CTRL_FW_CBITSi_ARRAY_BASE;
    }
    BREG_Write32_isr(handle->deviceHandle->regHandle, cbitBufAddr, cbits[0]);
    BREG_Write32_isr(handle->deviceHandle->regHandle, cbitBufAddr+4, cbits[1]);
    BREG_Write32_isr(handle->deviceHandle->regHandle, cbitBufAddr+8, cbits[2]);
    for ( i = 0; i < 9; i++ )
    {
        BREG_Write32_isr(handle->deviceHandle->regHandle, cbitBufAddr+12+(4*i), 0);
    }
    cbitVal &= ~BCHP_MASK(AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0, OFFSET);
    if ( cbitBufAddr == BCHP_AUD_FMM_MS_CTRL_FW_CBITSi_ARRAY_BASE )
    {
        cbitVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0, OFFSET, 0x50);
    }
    else
    {
        cbitVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0, OFFSET, 0x5c);
    }
    BREG_Write32_isr(handle->deviceHandle->regHandle, cbitAddr, cbitVal);
                
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_FW_RAMP_AMOUNT_0, 0);
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, regVal);

    return;
}

/**************************************************************************/

static BERR_Code BAPE_SpdifOutput_P_OpenHw_Legacy(BAPE_SpdifOutputHandle handle)
{
    BERR_Code       errCode = BERR_SUCCESS;
    BAPE_Handle     deviceHandle;
    uint32_t        regAddr, regVal;
    unsigned        streamId;
    BAPE_IopStreamSettings streamSettings;

    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    /* Enable the clock and data while opening the output port. Never disable it */
    regAddr = BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + handle->offset;
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0, CLOCK_ENABLE)|
                BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0,DATA_ENABLE));
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_SPDIF_CFG_0, CLOCK_ENABLE, Enable);
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_SPDIF_CFG_0, DATA_ENABLE, Enable);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    streamId = GET_SPDIF_STREAM_ID(handle->index);

    /* Reset source FCI to Invalid */
    BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    streamSettings.input = BAPE_SPDIF_INVALID_FCI_SOURCE;
    errCode = BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    BDBG_ASSERT(BERR_SUCCESS == errCode);

    /* Write the enable bit in the OP (only stereo) */
    BREG_Write32(handle->deviceHandle->regHandle, 
                 BCHP_AUD_FMM_OP_CTRL_ENABLE_SET,
                 (BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_SET, STREAM0_ENA))<<(streamId));


    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0);
    regVal &= ~( (BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, CHAN_OVERRIDE)) 
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, DITHER_ENA)) 
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, VALIDITY)) 
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, COMP_OR_LINEAR))
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, SPDIF_OR_PCM))
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, FLUSH_ON_UFLOW))
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, INSERT_ON_UFLOW))
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, INSERT_WHEN_DISA))      
                |(BCHP_MASK(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, STREAM_ENA))
                );

    regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, SPDIF_OR_PCM, 1));
#if ALWAYS_COMP_FOR_FW_CBITS
    regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, COMP_OR_LINEAR, 1));
#endif
    regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, FLUSH_ON_UFLOW, 1));
    regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, INSERT_ON_UFLOW, 1));
    regVal |= (BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, INSERT_WHEN_DISA, 1));      
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_0, regVal);

    /* Disable bypass */
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_USEQ_BYPASS);
    regVal &= ~BCHP_MASK(AUD_FMM_MS_CTRL_USEQ_BYPASS, STREAM0);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_USEQ_BYPASS, STREAM0, 0);
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_USEQ_BYPASS, regVal);

    /* Enable microsequencer */
    regVal = BREG_Read32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_STRM_ENA);
    regVal &= ~BCHP_MASK(AUD_FMM_MS_CTRL_STRM_ENA, STREAM0_ENA);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_STRM_ENA, STREAM0_ENA, 1);
    BREG_Write32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_STRM_ENA, regVal);

    return errCode;
}

/**************************************************************************/

static BERR_Code BAPE_SpdifOutput_P_CloseHw_Legacy(BAPE_SpdifOutputHandle handle)
{
    BERR_Code       errCode = BERR_SUCCESS;
    BAPE_Handle     deviceHandle;
    uint32_t        regAddr, regVal;
    unsigned        streamId;

    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    /* Disable microsequencer */
    regVal = BREG_Read32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_STRM_ENA);
    regVal &= ~BCHP_MASK(AUD_FMM_MS_CTRL_STRM_ENA, STREAM0_ENA);
    BREG_Write32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_STRM_ENA, regVal);

    /* Clear the enable bit in the OP */
    streamId = GET_SPDIF_STREAM_ID(handle->index);
    BREG_Write32(handle->deviceHandle->regHandle, 
                 BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                 BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_CLEAR, STREAM0_ENA)<<streamId);

    /* Disable the Spdif clock and data. */
    regAddr = BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + handle->offset;
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0, CLOCK_ENABLE)|
                BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0,DATA_ENABLE));
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_SPDIF_CFG_0, CLOCK_ENABLE, Disable);
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_SPDIF_CFG_0, DATA_ENABLE, Disable);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    return errCode;
}

static BERR_Code BAPE_SpdifOutput_P_ApplySettings_Legacy(
    BAPE_SpdifOutputHandle handle,
    const BAPE_SpdifOutputSettings *pSettings,
    bool force
    )
{
    uint32_t regAddr, regVal;
    BAPE_Reg_P_FieldList regFieldList;

    BSTD_UNUSED(force); /* Not used because HW gets updated unconditionally */

    BDBG_OBJECT_ASSERT(handle, BAPE_SpdifOutput);
    BDBG_ASSERT(NULL != pSettings);
    
    regAddr = BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 + handle->offset;
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0, LR_SELECT)|
                BCHP_MASK(AUD_FMM_OP_CTRL_SPDIF_CFG_0, LIMIT_TO_16_BITS));
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_SPDIF_CFG_0, LR_SELECT, pSettings->stereoMode);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_SPDIF_CFG_0, LIMIT_TO_16_BITS, (pSettings->limitTo16Bits?1:0));
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);

    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
    if ( pSettings->underflowBurst == BAPE_SpdifBurstType_ePause )
    {
        BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_MS_CTRL_FW_BURST_0, TYPE, Pause);
        BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_MS_CTRL_FW_BURST_0, REP_PERIOD, PER_3);
    }
    else
    {
        BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_MS_CTRL_FW_BURST_0, TYPE, Null);
        BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_MS_CTRL_FW_BURST_0, REP_PERIOD, None);
    }
    BAPE_Reg_P_ApplyFieldList_isr(&regFieldList, BCHP_AUD_FMM_MS_CTRL_FW_BURST_0);

    handle->settings = *pSettings;
    /* Remaining fields are handled by MS regs.  Must modify those in critical section. */    
    BKNI_EnterCriticalSection();
    BAPE_SpdifOutput_P_SetCbits_isr(handle);

    BKNI_LeaveCriticalSection();

    return BERR_SUCCESS;
}
#endif

#else

void BAPE_SpdifOutput_GetDefaultSettings(
    BAPE_SpdifOutputSettings *pSettings
    )
{
    BSTD_UNUSED(pSettings);
}

BERR_Code BAPE_SpdifOutput_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_SpdifOutputSettings *pSettings,
    BAPE_SpdifOutputHandle *pHandle             /* [out] */
    )
{
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(deviceHandle);
    BSTD_UNUSED(index);
    BSTD_UNUSED(pHandle);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}


void BAPE_SpdifOutput_Close(
    BAPE_SpdifOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

void BAPE_SpdifOutput_GetSettings(
    BAPE_SpdifOutputHandle handle,
    BAPE_SpdifOutputSettings *pSettings     /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}


BERR_Code BAPE_SpdifOutput_SetSettings(
    BAPE_SpdifOutputHandle handle,
    const BAPE_SpdifOutputSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

void BAPE_SpdifOutput_GetOutputPort(
    BAPE_SpdifOutputHandle handle,
    BAPE_OutputPort *pOutputPort        /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pOutputPort);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
}

#endif

#if BAPE_CHIP_MAX_SPDIF_OUTPUTS > 0 || BAPE_CHIP_MAX_MAI_OUTPUTS > 0
/***************************************************************************
Summary:
Setup Channel Status Bits for SPDIF or HDMI outputs
***************************************************************************/
void BAPE_P_MapSpdifChannelStatusToBits_isr(
    BAPE_OutputPort output,
    const BAPE_SpdifChannelStatus *pChannelStatus,
    BAPE_Spdif_P_ChannelStatusBits *pBits           /* [out] */
    )
{
    bool compressed;
    bool compressedAsPcm = false;
    unsigned sampleRate;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);
    BDBG_ASSERT(NULL != pChannelStatus);
    BDBG_ASSERT(NULL != pBits);

    pBits->bits[0] = 0;
    pBits->bits[1] = 0;
    pBits->bits[2] = 0;

    if ( output->mixer )
    {
        const BAPE_FMT_Descriptor     *pBfd = BAPE_Mixer_P_GetOutputFormat(output->mixer);

        compressed = BAPE_FMT_P_IsCompressed(pBfd);
        compressedAsPcm = BAPE_FMT_P_IsDtsCdCompressed(pBfd);
        sampleRate = (pBfd->type == BAPE_DataType_eIec61937x16) ?  768000 :  pBfd->sampleRate;
    }
    else
    {
        compressed = false;
        sampleRate = 48000;
    }
    
    if ( pChannelStatus->professional )
    {
        pBits->bits[0] |= 0x01 <<  0;   /* PRO_CONS */
    }
    if ( compressed && !compressedAsPcm )
    {
        pBits->bits[0] |= 0x01 <<  1;   /* COMP_LIN */
    }
    if ( !pChannelStatus->copyright )
    {
        pBits->bits[0] |= 0x01 <<  2;   /* CP */
    }
    /* EMPH = 0 */
    /* CMODE = 0 */
    pBits->bits[0] |= ((uint32_t)pChannelStatus->categoryCode&0xff) <<  8;   /* CATEGORY */
    pBits->bits[0] |= ((uint32_t)pChannelStatus->sourceNumber&0x0f) << 16;   /* SOURCE */
    pBits->bits[0] |= ((uint32_t)BAPE_P_GetSampleRateCstatCode(sampleRate)) << 24;   /* FREQ */
    pBits->bits[0] |= ((uint32_t)pChannelStatus->clockAccuracy&0x03) << 28;   /* ACCURACY */
    /* FREQ_EXTN = 0 */

    if ( compressed && !compressedAsPcm )  
    {
        /* Compressed leaves word1 at 0 */
    }
    else
    {
        pBits->bits[1] |= 0x01 << 0;   /* MAX_LEN = 24 bits */
        /* LENGTH = 0 [not indicated] */
        /* ORIG_FREQ = 0 [not indicated] */
        pBits->bits[1] |= ((uint32_t)pChannelStatus->cgmsA&0x03) << 8;   /* CGMS_A */
    }

    /* Done */
}

#endif

