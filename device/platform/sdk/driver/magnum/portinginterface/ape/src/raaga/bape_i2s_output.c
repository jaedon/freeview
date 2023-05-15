/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_i2s_output.c $
 * $brcm_Revision: Hydra_Software_Devel/30 $
 * $brcm_Date: 9/27/12 10:18a $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_i2s_output.c $
 * 
 * Hydra_Software_Devel/30   9/27/12 10:18a mpeteri
 * SW7445-19: merge 7445 APE changes to main
 * 
 * Hydra_Software_Devel/SW7445-19/1   8/17/12 5:49p mpeteri
 * SW7445-19: Add additional PLL and NCO for 7445
 * 
 * Hydra_Software_Devel/29   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:51p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/28   3/6/12 3:05p gskerl
 * SW7425-2570: Renamed OutputPortObject substruct from connector to
 * outputPort.
 * 
 * Hydra_Software_Devel/27   11/14/11 3:37p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/27/11 3:21p jgarrett
 * SW7429-18: Adding I2S Output for 7429
 * 
 * Hydra_Software_Devel/26   10/3/11 12:41p gskerl
 * SW7231-129: Changed a call from BAPE_I2sOutput_SetSettings() to
 * BAPE_I2sOutput_P_ApplySettings() with force=true. Also added an assert
 * to BAPE_I2sOutput_Close().
 * 
 * Hydra_Software_Devel/25   10/2/11 2:57p gskerl
 * SW7231-129: Added stub function for
 * BAPE_I2sOutput_P_ResumeFromStandby() (for when there are no I2S
 * outputs)
 * 
 * Hydra_Software_Devel/24   9/30/11 11:33a gskerl
 * SW7231-129: Updated comments. Removed dead code.
 * 
 * Hydra_Software_Devel/23   9/29/11 10:24a gskerl
 * SW7231-129: Commented out #define of SETUP_PINMUX_FOR_I2S_OUT_ON_7425,
 * which was inadvertently left defined
 * 
 * Hydra_Software_Devel/22   9/28/11 5:42p gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume.
 * 
 * Hydra_Software_Devel/21   9/7/11 10:36a sgadara
 * SWDTV-8370: [35233] Correcting the IOP index retrieve for I2S Stereo 1
 * port
 * 
 * Hydra_Software_Devel/20   8/25/11 2:43p gskerl
 * SW7425-1178: Changed name of conditional from
 * SETUP_PINMUX_FOR_I2S_OUT_ON_7422 to SETUP_PINMUX_FOR_I2S_OUT_ON_7425.,
 * 
 * Hydra_Software_Devel/18   7/8/11 6:38p jgarrett
 * SWDTV-6760: Adding I2sMultiOutput
 * 
 * Hydra_Software_Devel/17   7/8/11 4:25p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/16   7/7/11 10:53a jgarrett
 * SW7552-69: Added stereoMode setting
 * 
 * Hydra_Software_Devel/15   5/20/11 5:16p jgarrett
 * SW7425-402: Adding error code to handle output enable callback failing.
 * 
 * Hydra_Software_Devel/14   5/3/11 10:49a gskerl
 * SW7422-354: Added index and type args to APE_P_InitOutputPort macro
 * 
 * Hydra_Software_Devel/13   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/12   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/2   4/5/11 7:13p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/11   3/22/11 3:10p gskerl
 * SW7422-146: Changed audio output connector callbacks to take the output
 * connector as an argument
 * 
 * Hydra_Software_Devel/10   3/11/11 3:57p gskerl
 * SW7422-146: Corrected and clarified some comments.
 * 
 * Hydra_Software_Devel/9   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/8   2/17/11 5:40p gskerl
 * SW7422-146: Minor cosmetic cleanup.
 * 
 * Hydra_Software_Devel/7   2/16/11 4:50p jgarrett
 * SW7422-146: Coverity CID 252
 * 
 * Hydra_Software_Devel/6   2/16/11 2:24p gskerl
 * SW7422-146: Cleaned up, refactored, added support for changing I2S SCLK
 * (bit clock) rate.
 * 
 * Hydra_Software_Devel/5   2/11/11 7:31p gskerl
 * SW7422-146: Corrected a few compiler warnings.  Corrected logic that
 * sets the LRCLK polarity (it was reversed).
 * 
 * Hydra_Software_Devel/4   2/10/11 5:45p gskerl
 * SW7422-146: Changed calling sequence for the audio output setMclk_isr
 * callbacks
 * 
 * Hydra_Software_Devel/3   2/4/11 12:51p gskerl
 * SW7422-146:Added stub functions to handle BAPE_CHIP_MAX_I2S_OUTPUTS ==
 * 0 (no I2S outputs).
 * 
 * Hydra_Software_Devel/2   2/3/11 5:01p gskerl
 * SW7422-146:First pass at I2S output support.  BAPE_SclkRate_e128Fs is
 * not yet supported.
 * 
 * Hydra_Software_Devel/1   1/12/11 4:24p jgarrett
 * SW7422-146: Adding additional APIs
 * 
 ***************************************************************************/

#include "bape.h"
#include "bape_priv.h"

BDBG_MODULE(bape_i2s_output);

#if BAPE_CHIP_MAX_I2S_OUTPUTS > 0   /* If no I2S outputs, then skip all of this and just put in stub funcs at bottom of file. */

BDBG_OBJECT_ID(BAPE_I2sOutput);

typedef struct BAPE_I2sOutput
{
    BDBG_OBJECT(BAPE_I2sOutput)
    BAPE_Handle deviceHandle;
    BAPE_I2sOutputSettings settings;
    unsigned index;
    BAPE_OutputPortObject outputPort;
    unsigned sampleRate;
    uint32_t offset;
    struct
    {
        BAPE_MclkSource mclkSource;
        unsigned pllChannel;    /* only applies if mclkSource refers to a PLL */
        unsigned mclkFreqToFsRatio;
    } mclkInfo;
    bool enabled;
    char name[14];   /* I2s Output %d */
} BAPE_I2sOutput;


/* Static function prototypes */
static void         BAPE_I2sOutput_P_UpdateMclkReg_isr(BAPE_I2sOutputHandle hI2sOutput );
static BERR_Code    BAPE_I2sOutput_P_OpenHw(BAPE_I2sOutputHandle handle);
static BERR_Code    BAPE_I2sOutput_P_CloseHw(BAPE_I2sOutputHandle handle);
static BERR_Code    BAPE_I2sOutput_P_ApplySettings(BAPE_I2sOutputHandle handle,const BAPE_I2sOutputSettings *pSettings,bool force);

/* Output port callbacks */
static void BAPE_I2sOutput_P_SetTimingParams_isr(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase);
static BERR_Code BAPE_I2sOutput_P_Enable(BAPE_OutputPort output);
static void BAPE_I2sOutput_P_Disable(BAPE_OutputPort output);
static void BAPE_I2sOutput_P_SetMclk_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio);

#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
#include "bchp_aud_fmm_iop_out_i2s_stereo_0.h"
/* 7249-style chips */
static void         BAPE_I2sOutput_P_UpdateMclkReg_IopOut_isr(BAPE_I2sOutputHandle hI2sOutput );
static BERR_Code    BAPE_I2sOutput_P_OpenHw_IopOut(BAPE_I2sOutputHandle handle);
static BERR_Code    BAPE_I2sOutput_P_CloseHw_IopOut(BAPE_I2sOutputHandle handle);
static BERR_Code    BAPE_I2sOutput_P_ApplySettings_IopOut(BAPE_I2sOutputHandle handle,const BAPE_I2sOutputSettings *pSettings,bool force);
static BERR_Code    BAPE_I2sOutput_P_Enable_IopOut(BAPE_OutputPort output);
static void         BAPE_I2sOutput_P_Disable_IopOut(BAPE_OutputPort output);

#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_1_REG_START
#include "bchp_aud_fmm_iop_out_i2s_stereo_1.h"
#define GET_I2S_OFFSET(idx) ((idx)*(BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_1_REG_START-BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START))
#endif

#else
/* Legacy STB chips */
static void         BAPE_I2sOutput_P_UpdateMclkReg_Legacy_isr(BAPE_I2sOutputHandle hI2sOutput );
static BERR_Code    BAPE_I2sOutput_P_OpenHw_Legacy(BAPE_I2sOutputHandle handle);
static BERR_Code    BAPE_I2sOutput_P_CloseHw_Legacy(BAPE_I2sOutputHandle handle);
static BERR_Code    BAPE_I2sOutput_P_ApplySettings_Legacy(BAPE_I2sOutputHandle handle,const BAPE_I2sOutputSettings *pSettings,bool force);
static BERR_Code    BAPE_I2sOutput_P_Enable_Legacy(BAPE_OutputPort output);
static void         BAPE_I2sOutput_P_Disable_Legacy(BAPE_OutputPort output);

#if BAPE_CHIP_MAX_I2S_OUTPUTS > 1
    #define  GET_I2S_REG_ADDR2(prefix,idx)         (prefix##0         + (prefix##1         - prefix##0        ) * idx )
    #define  GET_I2S_REG_ADDR3(prefix,idx,suffix)  (prefix##0##suffix + (prefix##1##suffix - prefix##0##suffix) * idx )

    /* STB Chips */
    #define  GET_I2S_OP_STREAM_ID(idx) ((idx) == 0 ? 7 : (idx) == 1 ? 8 : (BDBG_ASSERT(0), 0))
    #define  GET_I2S_IOP_STREAM_ID(idx) GET_I2S_OP_STREAM_ID(idx) /* STB chips have this privilege */

    #define  GET_I2S_CROSSBAR_ADDR(idx) (((idx) * (BCHP_AUD_FMM_OP_CTRL_I2SS1_CROSSBAR - BCHP_AUD_FMM_OP_CTRL_I2SS0_CROSSBAR)) + BCHP_AUD_FMM_OP_CTRL_I2SS0_CROSSBAR);
#else
    #define  GET_I2S_REG_ADDR2(prefix,idx       )  (prefix##0         )
    #define  GET_I2S_REG_ADDR3(prefix,idx,suffix)  (prefix##0##suffix )
    #define  GET_I2S_OP_STREAM_ID(idx) (7)     /* Holds true across all chips today */
    #define  GET_I2S_IOP_STREAM_ID(idx) GET_I2S_OP_STREAM_ID(idx)
    #define  GET_I2S_CROSSBAR_ADDR(idx) (BCHP_AUD_FMM_OP_CTRL_I2SS0_CROSSBAR);
#endif
#endif

#ifndef GET_I2S_OFFSET
#define GET_I2S_OFFSET(idx) (0)
#endif

/****  #define SETUP_PINMUX_FOR_I2S_OUT_ON_7425 ****/   /* Only defined for testing... changes pinmux settings. */
#ifdef  SETUP_PINMUX_FOR_I2S_OUT_ON_7425
    static void BAPE_I2sOutput_P_SetupPinmuxForI2sTesting( BAPE_Handle deviceHandle );
    #warning "Compiling with special pinmux code to enable I2S outputs"
#endif /* SETUP_PINMUX_FOR_I2S_OUT_ON_7425 */


/***************************************************************************
        Public APIs: From bape_output.h
***************************************************************************/

void BAPE_I2sOutput_GetDefaultSettings(
    BAPE_I2sOutputSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->stereoMode     = BAPE_StereoMode_eLeftRight;
    pSettings->justification  = BAPE_I2sJustification_eMsbFirst;
    pSettings->dataAlignment  = BAPE_I2sDataAlignment_eDelayed;
    pSettings->lrPolarity     = BAPE_I2sLRClockPolarity_eLeftLow;
    pSettings->sclkPolarity   = BAPE_I2sSclkPolarity_eFalling;
    pSettings->sclkRate       = BAPE_SclkRate_e64Fs;
}

/**************************************************************************/

BERR_Code BAPE_I2sOutput_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_I2sOutputSettings *pSettings,
    BAPE_I2sOutputHandle *pHandle             /* [out] */
    )
{
    BERR_Code errCode;
    BAPE_I2sOutputHandle handle;
    BAPE_I2sOutputSettings defaultSettings;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);

    BDBG_MSG(("%s: Opening I2S Output: %u", __FUNCTION__, index));

    *pHandle = NULL;    /* Set up to return null handle in case of error. */

    if ( index >= BAPE_CHIP_MAX_I2S_OUTPUTS )
    {
        BDBG_ERR(("Request to open I2S output %d but chip only has %u I2S outputs", index, BAPE_CHIP_MAX_I2S_OUTPUTS));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( deviceHandle->i2sOutputs[index] )
    {
        BDBG_ERR(("I2S output %d already open", index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    #ifdef  SETUP_PINMUX_FOR_I2S_OUT_ON_7425
        BAPE_I2sOutput_P_SetupPinmuxForI2sTesting(deviceHandle);
    #endif /* SETUP_PINMUX_FOR_I2S_OUT_ON_7425 */

    /* Allocate the device structure, then fill in all the fields. */
    handle = BKNI_Malloc(sizeof(BAPE_I2sOutput));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    BKNI_Memset(handle, 0, sizeof(BAPE_I2sOutput));
    BDBG_OBJECT_SET(handle, BAPE_I2sOutput);
    handle->deviceHandle = deviceHandle;
    handle->index = index;
    handle->offset = GET_I2S_OFFSET(index);
    BAPE_P_InitOutputPort(&handle->outputPort, BAPE_OutputPortType_eI2sOutput, index, handle);
    handle->outputPort.mclkOutput = BAPE_MclkSource_eNone;
    handle->outputPort.muteInMixer = true;
    handle->outputPort.fsTiming = false;
    handle->outputPort.pllRequired = true;

    /* Setup source capabilities */
    BAPE_FMT_P_EnableSource(&handle->outputPort.capabilities, BAPE_DataSource_eFci);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcmStereo);

/*  handle->connector.mixerId[]  leave at default */
    handle->outputPort.setTimingParams_isr = BAPE_I2sOutput_P_SetTimingParams_isr;
    handle->outputPort.setMclk_isr = BAPE_I2sOutput_P_SetMclk_isr;
    handle->outputPort.setFs = NULL;         /* not used unless fsTiming == true */
    handle->outputPort.setMute = NULL;
    handle->outputPort.enable = BAPE_I2sOutput_P_Enable;
    handle->outputPort.disable = BAPE_I2sOutput_P_Disable;
/*  handle->connector.volume - leave at default */
    BKNI_Snprintf(handle->name, sizeof(handle->name), "I2S Output %u", index);
    handle->outputPort.pName = handle->name;

    /* Setup to 48k, muted by default */
    BKNI_EnterCriticalSection();
    BAPE_I2sOutput_P_SetTimingParams_isr(&handle->outputPort, 48000, BAVC_Timebase_e0);
    BKNI_LeaveCriticalSection();

    /* Init to specified settings */
    if ( NULL == pSettings )
    {
        BAPE_I2sOutput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }
                    
    errCode = BAPE_I2sOutput_P_OpenHw(handle);
    if ( errCode )
    {
        BAPE_I2sOutput_Close(handle);
        return BERR_TRACE(errCode);
    }

    /* Initialize hardware before applying settings */
    BKNI_EnterCriticalSection();
    BAPE_I2sOutput_P_SetMclk_isr(&handle->outputPort, BAPE_MclkSource_ePll0, 0, BAPE_BASE_PLL_TO_FS_RATIO);
    BKNI_LeaveCriticalSection();    
                    
    errCode = BAPE_I2sOutput_P_ApplySettings(handle, pSettings, true);   /* true => force update of HW */
    if ( errCode )
    {
        BAPE_I2sOutput_Close(handle);
        return BERR_TRACE(errCode);
    }
                        
    *pHandle = handle;
    handle->deviceHandle->i2sOutputs[index] = handle;
    return BERR_SUCCESS;
}

/**************************************************************************/

void BAPE_I2sOutput_Close(
    BAPE_I2sOutputHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);

    /* Make sure we're not still connected to anything */
    if ( handle->outputPort.mixer )
    {
        BDBG_ERR(("Cannot close I2S output %p (%s), still connected to mixer %p", handle, handle->name, handle->outputPort.mixer));
        BDBG_ASSERT(NULL == handle->outputPort.mixer);
        return;
    }

    BAPE_I2sOutput_P_CloseHw(handle);

    BDBG_ASSERT(handle->index < BAPE_CHIP_MAX_I2S_OUTPUTS);
    handle->deviceHandle->i2sOutputs[handle->index] = NULL;
    BDBG_OBJECT_DESTROY(handle, BAPE_I2sOutput);
    BKNI_Free(handle);    
}

/**************************************************************************/

void BAPE_I2sOutput_GetSettings(
    BAPE_I2sOutputHandle handle,
    BAPE_I2sOutputSettings *pSettings     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

/**************************************************************************/

BERR_Code BAPE_I2sOutput_SetSettings(
    BAPE_I2sOutputHandle handle,
    const BAPE_I2sOutputSettings *pSettings
    )
{
    BERR_Code   errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);
    BDBG_ASSERT(NULL != pSettings);

    errCode = BAPE_I2sOutput_P_ApplySettings(handle, pSettings, false); /* false => don't force (only update HW for changes) */
    return errCode;
}

/**************************************************************************/

void BAPE_I2sOutput_GetOutputPort(
    BAPE_I2sOutputHandle handle,
    BAPE_OutputPort *pOutputPort        /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);
    BDBG_ASSERT(NULL != pOutputPort);
    *pOutputPort = &handle->outputPort;
}

/***************************************************************************
        BAPE Internal APIs: From bape_fmm_priv.h
***************************************************************************/

BERR_Code BAPE_I2sOutput_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    i2sOutputIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    /* For each opened I2sOutput, call the functions necessary to restore the hardware to it's appropriate state. */
    for ( i2sOutputIndex=0 ; i2sOutputIndex<BAPE_CHIP_MAX_I2S_OUTPUTS ; i2sOutputIndex++ )
    {
        if ( bapeHandle->i2sOutputs[i2sOutputIndex] )       /* If this I2sOutput is open... */
        {
            BAPE_I2sOutputHandle hI2sOutput = bapeHandle->i2sOutputs[i2sOutputIndex];

            /* Put the HW into the generic open state. */
            errCode = BAPE_I2sOutput_P_OpenHw(hI2sOutput);
            if ( errCode ) return BERR_TRACE(errCode);
            
            /* Now apply changes for the settings struct. */
            errCode = BAPE_I2sOutput_P_ApplySettings(hI2sOutput, &hI2sOutput->settings, true);   /* true => force update of HW */
            if ( errCode ) return BERR_TRACE(errCode);

            /* Now restore the dynamic stuff from the values saved in the device struct. */
            BKNI_EnterCriticalSection();

                BAPE_I2sOutput_P_SetTimingParams_isr(&hI2sOutput->outputPort, 
                                                     hI2sOutput->sampleRate, 
                                                     0);    /* timebase is unused, 0 is dummy value */
                BAPE_I2sOutput_P_UpdateMclkReg_isr(hI2sOutput);
            BKNI_LeaveCriticalSection();
        }
    }
    return errCode;
}


/***************************************************************************
        Private callbacks: Protyped above
***************************************************************************/

static void BAPE_I2sOutput_P_SetTimingParams_isr(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase)
{
    BAPE_I2sOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);
    BSTD_UNUSED(timebase);  /* I2s doesn't care */

    handle->sampleRate = sampleRate;
}

/**************************************************************************/

static BERR_Code BAPE_I2sOutput_P_Enable(BAPE_OutputPort output)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
    return BAPE_I2sOutput_P_Enable_IopOut(output);
#else
    return BAPE_I2sOutput_P_Enable_Legacy(output);
#endif
}

/**************************************************************************/

static void BAPE_I2sOutput_P_Disable(BAPE_OutputPort output)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
    BAPE_I2sOutput_P_Disable_IopOut(output);
#else
    BAPE_I2sOutput_P_Disable_Legacy(output);
#endif
}

/**************************************************************************/

static void BAPE_I2sOutput_P_SetMclk_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio)
{
    BAPE_I2sOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);

    /* Just do some validation, then set the new values in the handle's mclkInfo struct.
     * Then ...UpdateMclkReg_isr() will take it from  there.
     */
    if (BAPE_MCLKSOURCE_IS_PLL(mclkSource) )
    {
        handle->mclkInfo.mclkSource = mclkSource;
        handle->mclkInfo.pllChannel = pllChannel;
    }
    else  if ( BAPE_MCLKSOURCE_IS_DAC(mclkSource) )
    {
        handle->mclkInfo.mclkSource = mclkSource;
        handle->mclkInfo.pllChannel = 0;

        BDBG_WRN(("DAC timing source has been selected for I2S output %u.", handle->index));
        BDBG_WRN(("It is strongly recommended to place I2S and DAC outputs on separate mixers."));
    }
    else  if ( BAPE_MCLKSOURCE_IS_NCO(mclkSource) )
    {
        handle->mclkInfo.mclkSource = mclkSource;
        handle->mclkInfo.pllChannel = 0;

        BDBG_WRN(("NCO timing source has been selected for I2S output %u.", handle->index));
        BDBG_WRN(("It is strongly recommended to use PLL timing for I2S."));
    }
    /* mclkSource is not a valid PLL and not a valid DAC... give up. */
    else
    {
        BDBG_ERR(("mclkSource (%u) doesn't refer to a valid PLL or DAC", mclkSource));
        BDBG_ASSERT(false);     /* something went wrong somewhere! */
        return;
    }

    handle->mclkInfo.mclkFreqToFsRatio = mclkFreqToFsRatio;

    BAPE_I2sOutput_P_UpdateMclkReg_isr( handle );

    return;
}

/***************************************************************************
        Private functions: Protyped above
***************************************************************************/
static void         BAPE_I2sOutput_P_UpdateMclkReg_isr(BAPE_I2sOutputHandle hI2sOutput )
{
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
    BAPE_I2sOutput_P_UpdateMclkReg_IopOut_isr(hI2sOutput);
#else
    BAPE_I2sOutput_P_UpdateMclkReg_Legacy_isr(hI2sOutput);
#endif
}

static BERR_Code    BAPE_I2sOutput_P_OpenHw(BAPE_I2sOutputHandle handle)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
    return BAPE_I2sOutput_P_OpenHw_IopOut(handle);
#else
    return BAPE_I2sOutput_P_OpenHw_Legacy(handle);
#endif    
}

static BERR_Code    BAPE_I2sOutput_P_CloseHw(BAPE_I2sOutputHandle handle)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
    return BAPE_I2sOutput_P_CloseHw_IopOut(handle);
#else
    return BAPE_I2sOutput_P_CloseHw_Legacy(handle);
#endif    
}

static BERR_Code    BAPE_I2sOutput_P_ApplySettings(BAPE_I2sOutputHandle handle,const BAPE_I2sOutputSettings *pSettings,bool force)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
    return BAPE_I2sOutput_P_ApplySettings_IopOut(handle, pSettings, force);
#else
    return BAPE_I2sOutput_P_ApplySettings_Legacy(handle, pSettings, force);
#endif    
}

#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
/* 7429 style chips */
static void BAPE_I2sOutput_P_UpdateMclkReg_IopOut_isr(BAPE_I2sOutputHandle hI2sOutput )
{
    /* Use the values in the handle's mclkInfo struct to update the
     * MCLK_CFG register.  */

    BAPE_MclkSource      mclkSource          = hI2sOutput->mclkInfo.mclkSource;
    unsigned             pllChannel          = hI2sOutput->mclkInfo.pllChannel;
    unsigned             mclkFreqToFsRatio   = hI2sOutput->mclkInfo.mclkFreqToFsRatio;
    BAPE_Reg_P_FieldList regFieldList;

    BDBG_OBJECT_ASSERT(hI2sOutput, BAPE_I2sOutput);

    BAPE_Reg_P_InitFieldList(hI2sOutput->deviceHandle, &regFieldList);

    switch ( mclkSource )
    {
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL0_ch1
    case BAPE_MclkSource_ePll0:
        switch ( pllChannel )
        {
        case 0: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, PLL0_ch1); break;
        case 1: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, PLL0_ch2); break;
        case 2: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, PLL0_ch3); break;                
        default: (void) BERR_TRACE(BERR_NOT_SUPPORTED); break;
        }
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL1_ch1
    case BAPE_MclkSource_ePll1:
        switch ( pllChannel )
        {
        case 0: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, PLL1_ch1); break;
        case 1: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, PLL1_ch2); break;
        case 2: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, PLL1_ch3); break;                
        default: (void) BERR_TRACE(BERR_NOT_SUPPORTED); break;
        }
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_PLL2_ch1
    case BAPE_MclkSource_ePll2:
        switch ( pllChannel )
        {
        case 0: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, PLL2_ch1); break;
        case 1: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, PLL2_ch2); break;
        case 2: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, PLL2_ch3); break;                
        default: (void) BERR_TRACE(BERR_NOT_SUPPORTED); break;
        }
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen0
    case BAPE_MclkSource_eNco0:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen0); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen1
    case BAPE_MclkSource_eNco1:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen1); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen2
    case BAPE_MclkSource_eNco2:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen2); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen3
    case BAPE_MclkSource_eNco3:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen3); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen4
    case BAPE_MclkSource_eNco4:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen4); 
        break;
#endif
    default:
        BDBG_ERR(("Unsupported clock source %u for MAI %u", mclkSource, hI2sOutput->index));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    switch ( mclkFreqToFsRatio )
    {
    case 128: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, MCLK_RATE, MCLK_128fs_SCLK_64fs); break;
    case 256: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, MCLK_RATE, MCLK_256fs_SCLK_64fs); break;
    case 384: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, MCLK_RATE, MCLK_384fs_SCLK_64fs); break;
    case 512: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0, MCLK_RATE, MCLK_512fs_SCLK_64fs); break;
    default:
        BDBG_ERR(("Unsupported MCLK Rate of %uFs", mclkFreqToFsRatio));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        break;
    }
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_MCLK_CFG_0 + hI2sOutput->offset);
}

static BERR_Code BAPE_I2sOutput_P_OpenHw_IopOut(BAPE_I2sOutputHandle handle)
{
    BAPE_Reg_P_FieldList regFieldList;
    BERR_Code            errCode = BERR_SUCCESS;
    BAPE_Handle          deviceHandle;
    unsigned             index;

    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    index = handle->index;

    /* Enable the clock and data while opening the output port. Never disable it */
    BAPE_Reg_P_InitFieldList(deviceHandle, &regFieldList);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, CLOCK_ENABLE, Enable);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, DATA_ENABLE, Enable);
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG + handle->offset);

    /* Turn on the I2S outputs  */
    BAPE_Reg_P_InitFieldList(deviceHandle, &regFieldList);    

    if (index == 0)
    {
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, SDATS0_OE, Drive);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, SCLKS0_OE, Drive);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, LRCKS0_OE, Drive);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, MCLKS0_OE, Drive);
    }
    #if BAPE_CHIP_MAX_I2S_OUTPUTS > 1
    else if(index == 1)
    {
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, SDATS1_OE, Drive);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, SCLKS1_OE, Drive);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, LRCKS1_OE, Drive);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, MCLKS1_OE, Drive);
    }
    #endif 
    #if BAPE_CHIP_MAX_I2S_OUTPUTS > 2
        #error "Need to support more I2S outputs"
    #endif 

    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_MISC_SEROUT_OE);

    return errCode;
}

static BERR_Code BAPE_I2sOutput_P_CloseHw_IopOut(BAPE_I2sOutputHandle handle)
{
    BAPE_Reg_P_FieldList regFieldList;
    BERR_Code            errCode = BERR_SUCCESS;
    BAPE_Handle          deviceHandle;
    unsigned             index;

    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    index = handle->index;

    /* Turn off the I2S outputs  */
    BAPE_Reg_P_InitFieldList(deviceHandle, &regFieldList);    

    if (index == 0)
    {
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, SDATS0_OE, Tristate);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, SCLKS0_OE, Tristate);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, LRCKS0_OE, Tristate);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, MCLKS0_OE, Tristate);
    }
    #if BAPE_CHIP_MAX_I2S_OUTPUTS > 1
    else if(index == 1)
    {
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, SDATS1_OE, Tristate);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, SCLKS1_OE, Tristate);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, LRCKS1_OE, Tristate);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_MISC_SEROUT_OE, MCLKS1_OE, Tristate);
    }
    #endif 
    #if BAPE_CHIP_MAX_I2S_OUTPUTS > 2
        #error "Need to support more I2S outputs"
    #endif 

    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_MISC_SEROUT_OE);

    /* Enable the clock and data while opening the output port. Never disable it */
    BAPE_Reg_P_InitFieldList(deviceHandle, &regFieldList);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, CLOCK_ENABLE, Disable);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, DATA_ENABLE, Disable);
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG + handle->offset);

    return errCode;
}

static BERR_Code BAPE_I2sOutput_P_ApplySettings_IopOut(
    BAPE_I2sOutputHandle handle,
    const BAPE_I2sOutputSettings *pSettings,
    bool force
    )
{
    BAPE_Reg_P_FieldList regFieldList;

    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);
    BDBG_ASSERT(NULL != pSettings);

    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);

    switch ( pSettings->justification )
    {
    case BAPE_I2sJustification_eMsbFirst:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, DATA_JUSTIFICATION, MSB);
        break;
    case BAPE_I2sJustification_eLsbFirst:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, DATA_JUSTIFICATION, LSB);
        break;
    default:
        BDBG_ERR(("Invalid value for BAPE_I2sOutputSettings.justification: %u", pSettings->justification ));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch ( pSettings->dataAlignment )
    {
    case BAPE_I2sDataAlignment_eDelayed:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, DATA_ALIGNMENT, Delayed);
        break;
    case BAPE_I2sDataAlignment_eAligned:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, DATA_ALIGNMENT, Aligned);
        break;
    default:
        BDBG_ERR(("Invalid value for BAPE_I2sOutputSettings.dataAlignment: %u", pSettings->dataAlignment ));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch ( pSettings->lrPolarity )
    {
    case BAPE_I2sLRClockPolarity_eLeftLow:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, LRCK_POLARITY, Low_for_left);
        break;
    case BAPE_I2sLRClockPolarity_eLeftHigh:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, LRCK_POLARITY, High_for_left);
        break;
    default:
        BDBG_ERR(("Invalid value for BAPE_I2sOutputSettings.lrPolarity: %u", pSettings->lrPolarity ));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /*  Since setting the sclkRate involves updating two registers, only bother with this
     *  if somebody is actually changing it.
     */
    if (pSettings->sclkRate != handle->settings.sclkRate || force)
    {
        switch ( pSettings->sclkRate )
        {
        case BAPE_SclkRate_e64Fs:
            BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, SCLKS_PER_1FS_DIV32, 2);
            break;
        case BAPE_SclkRate_e128Fs:
            BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, SCLKS_PER_1FS_DIV32, 4);
            break;
        default:
            BDBG_ERR(("Invalid value for BAPE_I2sOutputSettings.sclkRate: %u", pSettings->sclkRate ));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }

        /* Set the new sclkRate into the settings struct so that ..._UpdateMclkReg_isr()
         * will be able to use it.
         */
        handle->settings.sclkRate = pSettings->sclkRate;

        /* Now update the MCLK_CONFIG register.  */
        BKNI_EnterCriticalSection();
        BAPE_I2sOutput_P_UpdateMclkReg_isr( handle );
        BKNI_LeaveCriticalSection();
    }

    /* We only support 24 bits per sample. */
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG, BITS_PER_SAMPLE, Bitwidth24);

    /* Apply register changes */
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG + handle->offset);

    if ( pSettings->stereoMode != handle->settings.stereoMode || force )
    {
        BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
        switch ( pSettings->stereoMode )
        {
        default:
        case BAPE_StereoMode_eLeftRight:
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR, OUT_L, In_l);
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR, OUT_R, In_r);
            break;
        case BAPE_StereoMode_eLeftLeft:
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR, OUT_L, In_l);
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR, OUT_R, In_l);
            break;
        case BAPE_StereoMode_eRightRight:
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR, OUT_L, In_r);
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR, OUT_R, In_r);
            break;
        case BAPE_StereoMode_eRightLeft:
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR, OUT_L, In_r);
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CROSSBAR, OUT_R, In_l);
            break;            
        }

        /* Apply register changes */
        BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_I2S_CFG + handle->offset);

        handle->settings.stereoMode = pSettings->stereoMode;
    }

    handle->settings = *pSettings;

    return BERR_SUCCESS;
}

static BERR_Code BAPE_I2sOutput_P_Enable_IopOut(BAPE_OutputPort output)
{
    BAPE_I2sOutputHandle handle;
    BAPE_Reg_P_FieldList regFieldList;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);

    BDBG_ASSERT(false == handle->enabled);

    /* Setup Interface */
    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0, CHANNEL_GROUPING, 1);
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0, GROUP_ID, 0);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0, STREAM_BIT_RESOLUTION, Res_24_Bit);
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0, FCI_ID, output->sourceMixerFci.ids[BAPE_ChannelPair_eLeftRight]);
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0 + handle->offset);

    /* Enable the interface */
    BAPE_Reg_P_UpdateField(handle->deviceHandle, 
                           BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0 + handle->offset,
                           AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0,
                           ENA,
                           1);

    handle->enabled = true;

    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_I2sOutput_P_Disable_IopOut(BAPE_OutputPort output)
{
    BAPE_I2sOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);

    /* Disable the interface */
    BAPE_Reg_P_UpdateField(handle->deviceHandle, 
                           BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0 + handle->offset,
                           AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0,
                           ENA,
                           0);

    /* Clear the input value */
    BAPE_Reg_P_UpdateField(handle->deviceHandle, 
                           BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0 + handle->offset,
                           AUD_FMM_IOP_OUT_I2S_STEREO_0_STREAM_CFG_0,
                           FCI_ID,
                           BAPE_FCI_ID_INVALID);


    handle->enabled = false;
}

#else
/* Legacy STB chips */
static void BAPE_I2sOutput_P_UpdateMclkReg_Legacy_isr(BAPE_I2sOutputHandle hI2sOutput )
{
    /* Use the values in the handle's mclkInfo struct to update the
     * MCLK_CFG register.  */

    BAPE_MclkSource     mclkSource          = hI2sOutput->mclkInfo.mclkSource;
    unsigned            pllChannel          = hI2sOutput->mclkInfo.pllChannel;
    unsigned            mclkFreqToFsRatio   = hI2sOutput->mclkInfo.mclkFreqToFsRatio;

    uint32_t regAddr;
    uint32_t regVal;
    uint32_t mclkRate;
    uint32_t pllclksel;

    BDBG_OBJECT_ASSERT(hI2sOutput, BAPE_I2sOutput);

    /* We need to determine new values for the MCLK_RATE and PLLCLKSEL fields of the
     * AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO<n> register.
     */ 
    switch ( mclkSource )
    {
    /* PLL Timing */
    #if BAPE_CHIP_MAX_PLLS > 0
    case BAPE_MclkSource_ePll0:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_PLL0_ch1 + pllChannel;
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 1
    case BAPE_MclkSource_ePll1:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_PLL1_ch1 + pllChannel;
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 2
    case BAPE_MclkSource_ePll2:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_PLL2_ch1 + pllChannel;
        break;
    #endif
    
    /* DAC Timing */
    #if BAPE_CHIP_MAX_DACS > 0
    case BAPE_MclkSource_eHifidac0:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_Hifidac0;
        break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 1
    case BAPE_MclkSource_eHifidac1:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_Hifidac1;
        break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 2
    case BAPE_MclkSource_eHifidac2:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_Hifidac2;
        break;
    #endif
    
    /* NCO Timing */
    #if BAPE_CHIP_MAX_NCOS > 0
    case BAPE_MclkSource_eNco0:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_Mclk_gen0;
        break;
    #endif
    #if BAPE_CHIP_MAX_NCOS > 1
    case BAPE_MclkSource_eNco1:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_Mclk_gen1;
        break;
    #endif
    #if BAPE_CHIP_MAX_NCOS > 2
    case BAPE_MclkSource_eNco2:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0_PLLCLKSEL_Mclk_gen2;
        break;
    #endif
    
    /* Should never get here */
    default:
        BDBG_ERR(("mclkSource (%u) doesn't refer to a valid PLL or DAC", mclkSource));
        BDBG_ASSERT(false);     /* something went wrong somewhere! */
        return;
    }

    /* Compute the value for the MCLK_RATE */
    switch ( hI2sOutput->settings.sclkRate )
    {
    case BAPE_SclkRate_e64Fs:
        mclkRate = mclkFreqToFsRatio / ( 2 * 64 );
        break;

        /* Add code for this after we increase the base PLL rates. */
    case BAPE_SclkRate_e128Fs:
        mclkRate = mclkFreqToFsRatio / ( 2 * 128 );
        break;

    default:
        BDBG_ASSERT(false); /* Should never get here */
        return;
    }

    BDBG_ASSERT(mclkRate > 0); /* If this happens, might need to increase BAPE_BASE_PLL_TO_FS_RATIO */

    /* We have the values in "pllclksel" and "mclkRate", now write to the registers. */

    /* Choose the register for the appropriate output. */
    regAddr = GET_I2S_REG_ADDR2(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO, hI2sOutput->index);

    /* Read the register and clear the fields that we're going to fill in. */
    regVal = BREG_Read32_isr(hI2sOutput->deviceHandle->regHandle, regAddr);
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, PLLCLKSEL)|
                BCHP_MASK(AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, MCLK_RATE));

    /* Fill in the MCLK_RATE and PLLCLKSEL fields. */
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, MCLK_RATE, mclkRate);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0, PLLCLKSEL, pllclksel);

    /* Then write it to the reg. */
    BREG_Write32_isr(hI2sOutput->deviceHandle->regHandle, regAddr, regVal);
}

/**************************************************************************/

static BERR_Code BAPE_I2sOutput_P_OpenHw_Legacy(BAPE_I2sOutputHandle handle)
{
    BERR_Code       errCode = BERR_SUCCESS;
    BAPE_Handle     deviceHandle;
    uint32_t        regAddr, regVal;
    unsigned        index;

    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    index = handle->index;

    /* Enable the clock and data while opening the output port. Never disable it */
    regAddr = GET_I2S_REG_ADDR3(BCHP_AUD_FMM_OP_CTRL_I2SS, index, _CFG);
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG, CLOCK_ENABLE)|
                BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ENABLE));
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, CLOCK_ENABLE, Enable);
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ENABLE, Enable);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    /* Turn on the I2S outputs  */
    regAddr = BCHP_AUD_FMM_MISC_SEROUT_OE;
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);

    if (index == 0)
    {
        regVal &= ~(
            #ifdef BCHP_AUD_FMM_MISC_SEROUT_OE_EXTI2SS0_MCLK_OE_MASK
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,EXTI2SS0_MCLK_OE) |
            #endif
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,LRCKS0_OE) |
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,SCLKS0_OE) |
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,SDATS0_OE));

        #ifdef BCHP_AUD_FMM_MISC_SEROUT_OE_EXTI2SS0_MCLK_OE_MASK
            regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,EXTI2SS0_MCLK_OE, Drive));
        #endif
        regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKS0_OE, Drive));            
        regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKS0_OE, Drive));            
        regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATS0_OE, Drive));
    }
    #if BAPE_CHIP_MAX_I2S_OUTPUTS > 1
    else if(index == 1)
    {
        regVal &= ~(
            #ifdef BCHP_AUD_FMM_MISC_SEROUT_OE_EXTI2SS1_MCLK_OE_MASK
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,EXTI2SS1_MCLK_OE) |
            #endif
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,LRCKS1_OE) |
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,SCLKS1_OE) |
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,SDATS1_OE));

        #ifdef BCHP_AUD_FMM_MISC_SEROUT_OE_EXTI2SS1_MCLK_OE_MASK
            regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,EXTI2SS1_MCLK_OE, Drive));
        #endif
        regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKS1_OE, Drive));            
        regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKS1_OE, Drive));            
        regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATS1_OE, Drive));
    }
    #endif 
    #if BAPE_CHIP_MAX_I2S_OUTPUTS > 2
        #error "Need to support more I2S outputs"
    #endif 
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    return errCode;
}

/**************************************************************************/

static BERR_Code BAPE_I2sOutput_P_CloseHw_Legacy(BAPE_I2sOutputHandle handle)
{
    BERR_Code       errCode = BERR_SUCCESS;
    BAPE_Handle     deviceHandle;
    uint32_t        regAddr, regVal;
    unsigned        index;

    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    index = handle->index;

    /* Put the I2S outputs into a high-Z state */
    regAddr = BCHP_AUD_FMM_MISC_SEROUT_OE;
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
    if (handle->index == 0)
    {
        regVal &= ~(
            #ifdef BCHP_AUD_FMM_MISC_SEROUT_OE_EXTI2SS0_MCLK_OE_MASK
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,EXTI2SS0_MCLK_OE) |
            #endif
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,LRCKS0_OE) |
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,SCLKS0_OE) |
                    BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,SDATS0_OE));

        #ifdef BCHP_AUD_FMM_MISC_SEROUT_OE_EXTI2SS0_MCLK_OE_MASK
            regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,EXTI2SS0_MCLK_OE, Tristate));
        #endif
        regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKS0_OE, Tristate));
        regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKS0_OE, Tristate));
        regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATS0_OE, Tristate));
    }
    #if BAPE_CHIP_MAX_I2S_OUTPUTS > 1
        else if(handle->index == 1)
        {
            regVal &= ~(
                #ifdef BCHP_AUD_FMM_MISC_SEROUT_OE_EXTI2SS1_MCLK_OE_MASK
                        BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,EXTI2SS1_MCLK_OE) |
                #endif
                        BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,LRCKS1_OE) |
                        BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,SCLKS1_OE) |
                        BCHP_MASK(AUD_FMM_MISC_SEROUT_OE,SDATS1_OE));

            #ifdef BCHP_AUD_FMM_MISC_SEROUT_OE_EXTI2SS1_MCLK_OE_MASK
                regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,EXTI2SS1_MCLK_OE, Tristate));
            #endif
            regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,LRCKS1_OE, Tristate));
            regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SCLKS1_OE, Tristate));
            regVal |= (BCHP_FIELD_ENUM (AUD_FMM_MISC_SEROUT_OE,SDATS1_OE, Tristate));
        }
    #endif 
    #if BAPE_CHIP_MAX_I2S_OUTPUTS > 2
        #error "Need to support more I2S outputs"
    #endif 
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);

    /* Disable the I2S clock and data. */
    regAddr = GET_I2S_REG_ADDR3(BCHP_AUD_FMM_OP_CTRL_I2SS, index, _CFG);
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG, CLOCK_ENABLE)|
                BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ENABLE));
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, CLOCK_ENABLE, Disable);
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ENABLE, Disable);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    return errCode;
}

/**************************************************************************/

static BERR_Code BAPE_I2sOutput_P_ApplySettings_Legacy(
    BAPE_I2sOutputHandle handle,
    const BAPE_I2sOutputSettings *pSettings,
    bool force
    )
{
    uint32_t regAddr, regVal;

    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);
    BDBG_ASSERT(NULL != pSettings);

    /* Start by reading the appropriate BCHP_AUD_FMM_OP_CTRL_I2SS<n>_CFG register  for this device. */
    regAddr = GET_I2S_REG_ADDR3(BCHP_AUD_FMM_OP_CTRL_I2SS, handle->index, _CFG);
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);

    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_JUSTIFICATION));
    switch ( pSettings->justification )
    {
    case BAPE_I2sJustification_eMsbFirst:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_JUSTIFICATION,MSB);
        break;
    case BAPE_I2sJustification_eLsbFirst:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_JUSTIFICATION,LSB);
        break;
    default:
        BDBG_ERR(("Invalid value for BAPE_I2sOutputSettings.justification: %u", pSettings->justification ));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ALIGNMENT));
    switch ( pSettings->dataAlignment )
    {
    case BAPE_I2sDataAlignment_eDelayed:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ALIGNMENT, Delayed);
        break;
    case BAPE_I2sDataAlignment_eAligned:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, DATA_ALIGNMENT, Aligned);
        break;
    default:
        BDBG_ERR(("Invalid value for BAPE_I2sOutputSettings.dataAlignment: %u", pSettings->dataAlignment ));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG, LRCK_POLARITY));
    switch ( pSettings->lrPolarity )
    {
    case BAPE_I2sLRClockPolarity_eLeftLow:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, LRCK_POLARITY, Low_for_left);
        break;
    case BAPE_I2sLRClockPolarity_eLeftHigh:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, LRCK_POLARITY, High_for_left);
        break;
    default:
        BDBG_ERR(("Invalid value for BAPE_I2sOutputSettings.lrPolarity: %u", pSettings->lrPolarity ));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /*  Since setting the sclkRate involves updating two registers, only bother with this
     *  if somebody is actually changing it.
     */
    if (pSettings->sclkRate != handle->settings.sclkRate || force)
    {
        regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG, SCLKS_PER_1FS_DIV32));
        switch ( pSettings->sclkRate )
        {
        case BAPE_SclkRate_e64Fs:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_I2SS0_CFG, SCLKS_PER_1FS_DIV32, 2 );
            break;

        case BAPE_SclkRate_e128Fs:
            regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_I2SS0_CFG, SCLKS_PER_1FS_DIV32, 4 );
            break;

        default:
            BDBG_ERR(("Invalid value for BAPE_I2sOutputSettings.sclkRate: %u", pSettings->sclkRate ));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }

        /* Set the new sclkRate into the settings struct so that ..._UpdateMclkReg_isr()
         * will be able to use it.
         */
        handle->settings.sclkRate = pSettings->sclkRate;

        /* Now update the MCLK_CONFIG register.  */
        BKNI_EnterCriticalSection();
        BAPE_I2sOutput_P_UpdateMclkReg_isr( handle );
        BKNI_LeaveCriticalSection();
    }

    /* We only support 24 bits per sample. */
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CFG, BITS_PER_SAMPLE ));
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CFG, BITS_PER_SAMPLE, Bitwidth24);

    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);

    if ( pSettings->stereoMode != handle->settings.stereoMode || force )
    {
        regAddr = GET_I2S_CROSSBAR_ADDR(handle->index);
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, OUT_L)|
                    BCHP_MASK(AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, OUT_R));
        switch ( pSettings->stereoMode )
        {
        default:
        case BAPE_StereoMode_eLeftRight:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, OUT_L, In_l);
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, OUT_R, In_r);
            break;
        case BAPE_StereoMode_eLeftLeft:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, OUT_L, In_l);
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, OUT_R, In_l);
            break;
        case BAPE_StereoMode_eRightRight:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, OUT_L, In_r);
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, OUT_R, In_r);
            break;
        case BAPE_StereoMode_eRightLeft:
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, OUT_L, In_r);
            regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_I2SS0_CROSSBAR, OUT_R, In_l);
            break;            
        }
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
        handle->settings.stereoMode = pSettings->stereoMode;
    }

    handle->settings = *pSettings;

    return BERR_SUCCESS;
}

static BERR_Code BAPE_I2sOutput_P_Enable_Legacy(BAPE_OutputPort output)
{
    BAPE_I2sOutputHandle handle;
    BAPE_IopStreamSettings streamSettings;
    BERR_Code errCode;
    unsigned streamId;
    
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);

    BDBG_ASSERT(false == handle->enabled);

    streamId = GET_I2S_IOP_STREAM_ID(handle->index);
    
    /* Write source FCI to IOP */
    BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    streamSettings.resolution = 24;
    streamSettings.input = output->sourceMixerFci.ids[BAPE_ChannelPair_eLeftRight];         /* Take source FCI provided from mixer */
    errCode = BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    streamId = GET_I2S_OP_STREAM_ID(handle->index);
    BDBG_MSG(("Enabling %s [stream %u]", handle->name, streamId));
    
    /* Write the enable bit in the OP (only stereo) */
    BDBG_MSG(("Writing %x to enable set", (BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_SET, STREAM0_ENA))<<(streamId)));
    BREG_Write32(handle->deviceHandle->regHandle, 
                 BCHP_AUD_FMM_OP_CTRL_ENABLE_SET,
                 (BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_SET, STREAM0_ENA))<<(streamId));

    handle->enabled = true;
    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_I2sOutput_P_Disable_Legacy(BAPE_OutputPort output)
{
    BAPE_I2sOutputHandle handle;
    BAPE_IopStreamSettings streamSettings;
    BERR_Code errCode;
    unsigned streamId;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sOutput);

    streamId = GET_I2S_OP_STREAM_ID(handle->index);
    BDBG_MSG(("Disabling %s [stream %u]", handle->name, streamId));

    /* Clear the enable bit in the OP */
    BDBG_MSG(("Writing %x to enable clear", (BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_SET, STREAM0_ENA))<<(streamId)));
    BREG_Write32(handle->deviceHandle->regHandle, 
                 BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                 BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_CLEAR, STREAM0_ENA)<<streamId);

    /* Reset source FCI to Invalid */
    streamId = GET_I2S_IOP_STREAM_ID(handle->index);    
    BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    streamSettings.input = BAPE_FCI_ID_INVALID;
    errCode = BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    BDBG_ASSERT(BERR_SUCCESS == errCode);

    handle->enabled = false;
}

#endif
/***************************************************************************
Summary:
Test code to support I2S testing
***************************************************************************/
#ifdef  SETUP_PINMUX_FOR_I2S_OUT_ON_7425

#include "bchp_aon_pin_ctrl.h"
#include "bchp_sun_top_ctrl.h"

static void BAPE_I2sOutput_P_SetupPinmuxForI2sTesting( BAPE_Handle deviceHandle )
{
    uint32_t    reg;

    /* First, route the I2S0 outputs to somewhere that we can attach to them.
     * It seems that we should be able to send them to either aon_gpio 
     * pins 01,02,03, or aon_gpio pins 10,11,12.  But it doesn't seem like
     * AON_GPIO_03 works.
     */
    #if 1
        reg = BREG_Read32(deviceHandle->regHandle,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1);
    
        reg &=~(    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_10 ) |
                    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11 ) |
                    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12 ) );
    
        reg |=(    BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_10, 3 ) |  /* I2S_CLK0_OUT  on J2303/14 (Front panel i/f connector) */
                   BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11, 3 ) |  /* I2S_DATA0_OUT on J2303/15 (Front panel i/f connector) */
                   BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12, 3 ) ); /* I2S_LR0_OUT   on J2303/16 (Front panel i/f connector) */
    
        BREG_Write32 (deviceHandle->regHandle, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, reg);
    #else
        reg = BREG_Read32(deviceHandle->regHandle,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0);
    
        reg &=~(    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_01 ) |
                    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_02 ) |
                    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_03 ) );
    
        reg |=(    BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_01, 3 ) |  /* I2S_CLK0_OUT  on J2303/10 (Front panel i/f connector) */
                   BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_02, 3 ) |  /* I2S_DATA0_OUT on J2303/11 (Front panel i/f connector) */
                   BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_03, 3 ) ); /* I2S_LR0_OUT   on J2303/12 (Front panel i/f connector) */
    
        BREG_Write32 (deviceHandle->regHandle, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, reg);
    #endif

    /* Now, route the I2S1 outputs to somewhere that we can attach to them. 
     * These can only come out on gpio pins 55,56,57.
     */
    reg = BREG_Read32(deviceHandle->regHandle,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

    reg &=~(    BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_057 ) |
                BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_055 ) |
                BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_056 ) );

    reg |=(    BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_057, 5 ) |  /* I2S_CLK1 on  J2902/3  (RMXP_OUT connector)  */
               BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_055, 5 ) |  /* I2S_DATA1 on J2902/21 (RMXP_OUT connector)  */
               BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_056, 5 ) ); /* I2S_LR1 on   J2902/23 (RMXP_OUT connector)  */

    BREG_Write32 (deviceHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, reg);

    /* Route each "External MCLK" (also called AUD_FS_CLKx)  */
    reg = BREG_Read32(deviceHandle->regHandle,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2);
    reg &=~ ( BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_099) |
              BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_100) );
    reg |=  ( BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_099, 2 ) |  /* External MCLK0 (AUD_FS_CLK0) on J3002/1 */
              BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_2, gpio_100, 2 ) ); /* External MCLK1 (AUD_FS_CLK1) on J3002/5 */
    BREG_Write32 (deviceHandle->regHandle, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, reg);
}
#endif /* SETUP_PINMUX_FOR_I2S_OUT_ON_7425 */


/***************************************************************************
    Define stub functions for when there are no I2S outputs. 
***************************************************************************/
#else /* BAPE_CHIP_MAX_I2S_OUTPUTS <= 0 */
    /* No I2S outputs, just use stubbed out functions. */

/**************************************************************************/

void BAPE_I2sOutput_GetDefaultSettings(
    BAPE_I2sOutputSettings *pSettings
    )
{
    BSTD_UNUSED(pSettings);
}

/**************************************************************************/

BERR_Code BAPE_I2sOutput_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_I2sOutputSettings *pSettings,
    BAPE_I2sOutputHandle *pHandle             /* [out] */
    )
{
    BSTD_UNUSED(deviceHandle);
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);

    *pHandle = NULL;

    return BERR_NOT_SUPPORTED;
}

/**************************************************************************/

void BAPE_I2sOutput_Close(
    BAPE_I2sOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/**************************************************************************/

void BAPE_I2sOutput_GetSettings(
    BAPE_I2sOutputHandle handle,
    BAPE_I2sOutputSettings *pSettings     /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

/**************************************************************************/

BERR_Code BAPE_I2sOutput_SetSettings(
    BAPE_I2sOutputHandle handle,
    const BAPE_I2sOutputSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);

    return BERR_NOT_SUPPORTED;
}

/**************************************************************************/

void BAPE_I2sOutput_GetOutputPort(
    BAPE_I2sOutputHandle handle,
    BAPE_OutputPort *pOutputPort        /* [out] */
    )
{
    BSTD_UNUSED(handle);

    *pOutputPort = NULL;
}

/**************************************************************************/

BERR_Code BAPE_I2sOutput_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BSTD_UNUSED(bapeHandle);
    return BERR_SUCCESS;
}

#endif /* BAPE_CHIP_MAX_I2S_OUTPUTS > */

