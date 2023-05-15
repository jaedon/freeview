/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_i2s_input.c $
 * $brcm_Revision: Hydra_Software_Devel/21 $
 * $brcm_Date: 5/3/12 5:05p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_i2s_input.c $
 * 
 * Hydra_Software_Devel/21   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/20   3/6/12 2:53p gskerl
 * SW7425-2570: Renamed InputPortObject substruct from connector to
 * inputPort.
 * 
 * Hydra_Software_Devel/19   3/5/12 9:22a jgarrett
 * SW7425-2455: Removing ADC and RF Inputs
 * 
 * Hydra_Software_Devel/18   11/14/11 3:38p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/2   10/26/11 4:09p jgarrett
 * SW7429-18: Adding I2S Input for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/17   10/6/11 6:39p gskerl
 * SW7231-129: Added stub functions for when BAPE_CHIP_MAX_RF_INPUTS is
 * zero.
 * 
 * Hydra_Software_Devel/16   10/6/11 6:15p gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume.
 * 
 * Hydra_Software_Devel/15   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/14   4/11/11 5:54p jgarrett
 * SWDTV-6305: Adding ADC/RF Inputs for DTV
 * 
 * Hydra_Software_Devel/13   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 7:13p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/12   3/22/11 3:07p gskerl
 * SW7422-146: Reverted last checkin
 * 
 * Hydra_Software_Devel/11   3/22/11 3:02p gskerl
 * SW7422-146: Changed audio output connector callbacks to take the output
 * connector as an argument
 * 
 * Hydra_Software_Devel/10   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/9   3/18/11 6:06p gskerl
 * SW7422-146: Changed audio input connector callbacks to take the connect
 * handle as an argument
 * 
 * Hydra_Software_Devel/8   3/11/11 3:55p gskerl
 * SW7422-146: Added test code that will change the 7422/7425 pinmux
 * settings in order to route
 * pins to the I2S input signals.  The test code must be enabled by
 * defining
 * SETUP_PINMUX_FOR_I2S_IN_ON_7422.
 * 
 * Hydra_Software_Devel/7   3/3/11 6:31p jgarrett
 * SW7422-146: Adding SRC and DSOLA and path review feedback
 * 
 * Hydra_Software_Devel/6   3/1/11 3:26p jgarrett
 * SW7422-146: Moving samlpe rate to input port from input capture
 * 
 * Hydra_Software_Devel/5   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/4   1/26/11 5:38p piyushg
 * SW7422-146: Add capture configuration settings in Input
 * enable/disable.
 * 
 * Hydra_Software_Devel/3   1/21/11 7:32p piyushg
 * SW7422-146: Initial checkin for I2S input.
 * 
 * Hydra_Software_Devel/2   1/18/11 11:04a jgarrett
 * SW7422-146: Adding handle to input open routines
 * 
 * Hydra_Software_Devel/1   1/12/11 4:24p jgarrett
 * SW7422-146: Adding additional APIs
 * 
 ***************************************************************************/

#include "bape.h"
#include "bape_priv.h"

BDBG_MODULE(bape_i2s_input);

BDBG_OBJECT_ID(BAPE_I2sInput);

typedef struct BAPE_I2sInput
{
    BDBG_OBJECT(BAPE_I2sInput)
    BAPE_Handle deviceHandle;
    BAPE_I2sInputSettings settings;
    unsigned index;                 
    BAPE_InputPortObject inputPort;
    uint32_t offset;  
    bool enable;
    char name[14];   /* I2S Input %d */
} BAPE_I2sInput;


#ifdef BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_REG_START
    #include "bchp_aud_fmm_iop_in_i2s_stereo_0.h"
    #ifdef BCHP_AUD_FMM_IOP_IN_I2S_STEREO_1_REG_START
        #include "bchp_aud_fmm_iop_in_i2s_stereo_0.h"
        #define GET_I2S_INPUT_OFFSET(idx) (idx*(BCHP_AUD_FMM_IOP_IN_I2S_STEREO_1_REG_START-BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_REG_START))
    #else
        #define GET_I2S_INPUT_OFFSET(idx) (0)
    #endif
#else
    #ifdef BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG1
        #define GET_I2S_INPUT_OFFSET(idx) (idx*(BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG1-BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG0))
    #else
        #define GET_I2S_INPUT_OFFSET(idx) (0)
    #endif
#endif

#if BAPE_CHIP_MAX_I2S_INPUTS > 0
/* Static function prototypes */
#if defined BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_REG_START
/* 7429 style of registers */
static void BAPE_I2sInput_P_Enable_IopOut(BAPE_InputPort inputPort);
static void BAPE_I2sInput_P_Disable_IopOut(BAPE_InputPort inputPort);
static BERR_Code BAPE_I2sInput_P_ApplySettings_IopOut(BAPE_I2sInputHandle handle, const BAPE_I2sInputSettings *pSettings);

#elif defined BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG0
/* Legacy style of registers */
static void BAPE_I2sInput_P_Enable_Legacy(BAPE_InputPort inputPort);
static void BAPE_I2sInput_P_Disable_Legacy(BAPE_InputPort inputPort);
static BERR_Code BAPE_I2sInput_P_ApplySettings_Legacy(BAPE_I2sInputHandle handle, const BAPE_I2sInputSettings *pSettings);

#endif

/* Input port callbacks */
static void BAPE_I2sInput_P_Enable(BAPE_InputPort inputPort);
static void BAPE_I2sInput_P_Disable(BAPE_InputPort inputPort);
#endif

/****  #define SETUP_PINMUX_FOR_I2S_IN_ON_7422   Only defined for testing... changes pinmux settings. */
#ifdef  SETUP_PINMUX_FOR_I2S_IN_ON_7422
    static void BAPE_I2sInput_P_SetupPinmuxForI2sTesting( BAPE_Handle deviceHandle );
    #warning "Compiling with special pinmux code to enable I2S input"
#endif /* SETUP_PINMUX_FOR_I2S_IN_ON_7422 */


/***************************************************************************
        Public APIs: From bape_input.h
***************************************************************************/
void BAPE_I2sInput_GetDefaultSettings(
    BAPE_I2sInputSettings *pSettings        /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->sampleRate = 48000;
    pSettings->bitsPerSample = 0;
    pSettings->lrPolarity = BAPE_I2sLRClockPolarity_eLeftLow;
    pSettings->sclkPolarity = BAPE_I2sSclkPolarity_eFalling;
    pSettings->dataAlignment = BAPE_I2sDataAlignment_eDelayed;
    pSettings->justification = BAPE_I2sJustification_eMsbFirst;
}

/**************************************************************************/

BERR_Code BAPE_I2sInput_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_I2sInputSettings *pSettings,
    BAPE_I2sInputHandle *pHandle             /* [out] */
    )
{
#if BAPE_CHIP_MAX_I2S_INPUTS > 0
    BERR_Code errCode;
    BAPE_I2sInputHandle handle;
    BAPE_I2sInputSettings defaultSettings;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);
    
    BDBG_MSG(("%s: Opening I2S Input: %u", __FUNCTION__, index));

    *pHandle = NULL;

    if ( index >= BAPE_CHIP_MAX_I2S_INPUTS )
    {
        BDBG_ERR(("Request to open I2S input %d but chip only has %u I2S inputs", index, BAPE_CHIP_MAX_I2S_INPUTS));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( deviceHandle->i2sInputs[index] )
    {
        BDBG_ERR(("I2S input %d already open", index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    #ifdef  SETUP_PINMUX_FOR_I2S_IN_ON_7422
        BAPE_I2sInput_P_SetupPinmuxForI2sTesting(deviceHandle);
    #endif /* SETUP_PINMUX_FOR_I2S_IN_ON_7422 */

    /* Allocate the device structure, then fill in all the fields. */
    handle = BKNI_Malloc(sizeof(BAPE_I2sInput));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    BKNI_Memset(handle, 0, sizeof(BAPE_I2sInput));
    BDBG_OBJECT_SET(handle, BAPE_I2sInput);
    handle->deviceHandle = deviceHandle;
    handle->index = index;
    handle->offset = GET_I2S_INPUT_OFFSET(index);
    BAPE_P_InitInputPort(&handle->inputPort, BAPE_InputPortType_eI2s, index, handle);
#if defined BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_REG_START
    {
        uint32_t regVal;
        regVal = BAPE_Reg_P_Read(deviceHandle, handle->offset + BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_CAPTURE_FCI_ID_TABLE);
        handle->inputPort.streamId[BAPE_ChannelPair_eLeftRight] = BCHP_GET_FIELD_DATA(regVal, AUD_FMM_IOP_IN_I2S_STEREO_0_CAPTURE_FCI_ID_TABLE, START_FCI_ID);
    }
#else
    handle->inputPort.streamId[BAPE_ChannelPair_eLeftRight] = 4+index;
#endif
    handle->inputPort.enable = BAPE_I2sInput_P_Enable;
    handle->inputPort.disable = BAPE_I2sInput_P_Disable;
    BKNI_Snprintf(handle->name, sizeof(handle->name), "I2S Input %u", index);
    handle->inputPort.pName = handle->name;

    /* Init to specified settings */
    if ( NULL == pSettings )
    {
        BAPE_I2sInput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    errCode = BAPE_I2sInput_SetSettings(handle, pSettings);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_settings;
    }

    *pHandle = handle;
    handle->deviceHandle->i2sInputs[index] = handle;
    return BERR_SUCCESS;

err_settings:
    BAPE_I2sInput_Close(handle);
    return errCode;
#else
    BSTD_UNUSED(deviceHandle);
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    BDBG_ASSERT(NULL != pHandle);
    *pHandle = NULL;
    return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif    
}

/**************************************************************************/

void BAPE_I2sInput_Close(
    BAPE_I2sInputHandle handle
    )
{
#if BAPE_CHIP_MAX_I2S_INPUTS > 0
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sInput);

    /* Make sure we're not still connected to anything */
    if ( handle->inputPort.pConsumer )
    {
        BDBG_ERR(("Cannot close I2S input %p (%d), still connected to %s", handle, handle->index, handle->inputPort.pConsumer->pName));
        BDBG_ASSERT(NULL == handle->inputPort.pConsumer);
        return;
    }

    handle->deviceHandle->i2sInputs[handle->index] = NULL;
    BDBG_OBJECT_DESTROY(handle, BAPE_I2sInput);
    BKNI_Free(handle);    
#else
    BSTD_UNUSED(handle);
#endif
}

/**************************************************************************/

void BAPE_I2sInput_GetSettings(
    BAPE_I2sInputHandle handle,
    BAPE_I2sInputSettings *pSettings        /* [out] */
    )
{
#if BAPE_CHIP_MAX_I2S_INPUTS > 0
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sInput);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
#else
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
#endif
}

/**************************************************************************/

BERR_Code BAPE_I2sInput_SetSettings(
    BAPE_I2sInputHandle handle,
    const BAPE_I2sInputSettings *pSettings
    )
{
#if BAPE_CHIP_MAX_I2S_INPUTS > 0   
    BERR_Code errCode;
    BAPE_FMT_Descriptor format;

    #if BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_REG_START
    errCode = BAPE_I2sInput_P_ApplySettings_IopOut(handle, pSettings);
    #else
    errCode = BAPE_I2sInput_P_ApplySettings_Legacy(handle, pSettings);
    #endif
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    handle->settings = *pSettings;

    /* Update Format */
    BKNI_EnterCriticalSection();
    BAPE_InputPort_P_GetFormat_isr(&handle->inputPort, &format);
    format.source = BAPE_DataSource_eFci;
    format.type = BAPE_DataType_ePcmStereo;
    format.sampleRate = handle->settings.sampleRate;
    errCode = BAPE_InputPort_P_SetFormat_isr(&handle->inputPort, &format);
    BKNI_LeaveCriticalSection();
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
#else
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/**************************************************************************/

void BAPE_I2sInput_GetInputPort(
    BAPE_I2sInputHandle handle,
    BAPE_InputPort *pInputPort
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sInput);
    BDBG_ASSERT(NULL != pInputPort);
    *pInputPort = &handle->inputPort;
}

/***************************************************************************
        BAPE Internal APIs: From bape_fmm_priv.h
***************************************************************************/

BERR_Code BAPE_I2sInput_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BERR_Code   errCode = BERR_SUCCESS;
#if BAPE_CHIP_MAX_I2S_INPUTS > 0
    unsigned    i2sInputIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    /* For each opened I2sInput, call the functions necessary to restore the hardware to it's appropriate state. */
    for ( i2sInputIndex=0 ; i2sInputIndex<BAPE_CHIP_MAX_I2S_INPUTS ; i2sInputIndex++ )
    {
        if ( bapeHandle->i2sInputs[i2sInputIndex] )       /* If this I2sInput is open... */
        {
            BAPE_I2sInputHandle hI2sInput = bapeHandle->i2sInputs[i2sInputIndex];

            /* Put the HW into the generic open state. */
                /* Nothing to do here for I2sInput. */
            
            /* Now apply changes for the settings struct. */
            errCode = BAPE_I2sInput_SetSettings(hI2sInput, &hI2sInput->settings);
            if ( errCode ) return BERR_TRACE(errCode);

            /* Now restore the dynamic stuff from the values saved in the device struct. */
                /* And nothing to do here either. */
        }
    }
#else
    BSTD_UNUSED(bapeHandle);
#endif
    return errCode;
}

/***************************************************************************
        Private callbacks: Protyped above
***************************************************************************/
#if BAPE_CHIP_MAX_I2S_INPUTS > 0
static void BAPE_I2sInput_P_Enable(BAPE_InputPort inputPort)
{
#if defined BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_REG_START
    BAPE_I2sInput_P_Enable_IopOut(inputPort);
#else
    BAPE_I2sInput_P_Enable_Legacy(inputPort);
#endif
}

/**************************************************************************/

static void BAPE_I2sInput_P_Disable(BAPE_InputPort inputPort)
{
#if defined BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_REG_START
    BAPE_I2sInput_P_Disable_IopOut(inputPort);
#else
    BAPE_I2sInput_P_Disable_Legacy(inputPort);
#endif
}

#if defined BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_REG_START
/* 7429 style of registers */
static void BAPE_I2sInput_P_Enable_IopOut(BAPE_InputPort inputPort)
{
    BAPE_I2sInputHandle handle;

    handle = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sInput);
    BDBG_ASSERT(false == handle->enable);

    BDBG_MSG(("Enabling %s", handle->name));

    BAPE_Reg_P_UpdateField(handle->deviceHandle, 
                           BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_CAP_STREAM_CFG_0 + handle->offset,
                           AUD_FMM_IOP_IN_I2S_STEREO_0_CAP_STREAM_CFG_0,
                           CAP_ENA,
                           1);

    handle->enable = true;
}

static void BAPE_I2sInput_P_Disable_IopOut(BAPE_InputPort inputPort)
{
    BAPE_I2sInputHandle handle;

    handle = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sInput);
    BDBG_ASSERT(true == handle->enable);

    BDBG_MSG(("Disabling %s", handle->name));

    BAPE_Reg_P_UpdateField(handle->deviceHandle, 
                           BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_CAP_STREAM_CFG_0 + handle->offset,
                           AUD_FMM_IOP_IN_I2S_STEREO_0_CAP_STREAM_CFG_0,
                           CAP_ENA,
                           0);

    handle->enable = false;
}

static BERR_Code BAPE_I2sInput_P_ApplySettings_IopOut(BAPE_I2sInputHandle handle, const BAPE_I2sInputSettings *pSettings)
{
    BAPE_Reg_P_FieldList regFieldList;
    uint32_t regAddr;

    BDBG_OBJECT_ASSERT(handle, BAPE_I2sInput);
    BDBG_ASSERT(NULL != pSettings);

    regAddr = BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_I2S_IN_CFG + handle->offset;
    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_IN_I2S_STEREO_0_I2S_IN_CFG, BITS_PER_SAMPLE, pSettings->bitsPerSample);

    switch ( pSettings->dataAlignment )
    {
    case BAPE_I2sDataAlignment_eAligned:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_IN_I2S_STEREO_0_I2S_IN_CFG, DATA_ALIGNMENT, Aligned);
        break;
    case BAPE_I2sDataAlignment_eDelayed:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_IN_I2S_STEREO_0_I2S_IN_CFG, DATA_ALIGNMENT, Delayed);
        break;
    default:
        BDBG_ERR(("Invalid Data Alignment type %d", pSettings->dataAlignment));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch ( pSettings->justification )
    {
    case BAPE_I2sJustification_eLsbFirst:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_IN_I2S_STEREO_0_I2S_IN_CFG, DATA_JUSTIFICATION, LSB);
        break;
    case BAPE_I2sJustification_eMsbFirst:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_IN_I2S_STEREO_0_I2S_IN_CFG, DATA_JUSTIFICATION, MSB);
        break;
    default:
        BDBG_ERR(("Invalid Data Justification type %d", pSettings->justification));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch ( pSettings->lrPolarity )
    {
    case BAPE_I2sLRClockPolarity_eLeftHigh:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_IN_I2S_STEREO_0_I2S_IN_CFG, LRCK_POLARITY, High_for_left);
        break;
    case BAPE_I2sLRClockPolarity_eLeftLow:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_IN_I2S_STEREO_0_I2S_IN_CFG, LRCK_POLARITY, Low_for_left);
        break;
    default:
        BDBG_ERR(("Invalid polarity of the left/right clock %d", pSettings->lrPolarity));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch ( pSettings->sclkPolarity )
    {
    case BAPE_I2sSclkPolarity_eRising:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_IN_I2S_STEREO_0_I2S_IN_CFG, SCLK_POLARITY, Rising_aligned_with_sdata);
        break;
    case BAPE_I2sSclkPolarity_eFalling:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_IN_I2S_STEREO_0_I2S_IN_CFG, SCLK_POLARITY, Falling_aligned_with_sdata);
        break;
    default:
        BDBG_ERR(("Invalid polarity of serial bit clock %d", pSettings->lrPolarity));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BAPE_Reg_P_ApplyFieldList(&regFieldList, regAddr);

    return BERR_SUCCESS;
}

#elif defined BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG0
/* Legacy style of registers */
static void BAPE_I2sInput_P_Enable_Legacy(BAPE_InputPort inputPort)
{
    BAPE_I2sInputHandle handle;
    BERR_Code errCode;

    handle = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sInput);
    BDBG_ASSERT(false == handle->enable);

    BDBG_MSG(("Enabling %s", handle->name));

    errCode = BAPE_Iop_P_EnableCapture(handle->deviceHandle, inputPort->streamId[0], 1);
    BDBG_ASSERT(BERR_SUCCESS == errCode);

    handle->enable = true;
}

static void BAPE_I2sInput_P_Disable_Legacy(BAPE_InputPort inputPort)
{
    BAPE_I2sInputHandle handle;

    handle = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_I2sInput);
    BDBG_ASSERT(true == handle->enable);

    BDBG_MSG(("Disabling %s", handle->name));

    BAPE_Iop_P_DisableCapture(handle->deviceHandle, inputPort->streamId[0], 1);

    handle->enable = false;
}

static BERR_Code BAPE_I2sInput_P_ApplySettings_Legacy(BAPE_I2sInputHandle handle, const BAPE_I2sInputSettings *pSettings)
{
    uint32_t regVal, regAddr;

    BDBG_OBJECT_ASSERT(handle, BAPE_I2sInput);
    BDBG_ASSERT(NULL != pSettings);

    regAddr = BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG0 + handle->offset;
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~(BCHP_MASK(AUD_FMM_IOP_CTRL_I2SIN_CFG0, DATA_JUSTIFICATION) |
                BCHP_MASK(AUD_FMM_IOP_CTRL_I2SIN_CFG0, DATA_ALIGNMENT) |
                BCHP_MASK(AUD_FMM_IOP_CTRL_I2SIN_CFG0, SCLK_POLARITY) |
                BCHP_MASK(AUD_FMM_IOP_CTRL_I2SIN_CFG0, LRCK_POLARITY) |
                BCHP_MASK(AUD_FMM_IOP_CTRL_I2SIN_CFG0, BITS_PER_SAMPLE));

    regVal |= BCHP_FIELD_DATA(AUD_FMM_IOP_CTRL_I2SIN_CFG0, BITS_PER_SAMPLE, pSettings->bitsPerSample);

    switch ( pSettings->dataAlignment )
    {
    case BAPE_I2sDataAlignment_eAligned:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, DATA_ALIGNMENT, Aligned);
        break;
    case BAPE_I2sDataAlignment_eDelayed:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, DATA_ALIGNMENT, Delayed);
        break;
    default:
        BDBG_ERR(("Invalid Data Alignment type %d", pSettings->dataAlignment));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch ( pSettings->justification )
    {
    case BAPE_I2sJustification_eLsbFirst:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, DATA_JUSTIFICATION, LSB);
        break;
    case BAPE_I2sJustification_eMsbFirst:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, DATA_JUSTIFICATION, MSB);
        break;
    default:
        BDBG_ERR(("Invalid Data Justification type %d", pSettings->justification));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch ( pSettings->lrPolarity )
    {
    case BAPE_I2sLRClockPolarity_eLeftHigh:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, LRCK_POLARITY, High_for_left);
        break;
    case BAPE_I2sLRClockPolarity_eLeftLow:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, LRCK_POLARITY, Low_for_left);
        break;
    default:
        BDBG_ERR(("Invalid polarity of the left/right clock %d", pSettings->lrPolarity));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch ( pSettings->sclkPolarity )
    {
    case BAPE_I2sSclkPolarity_eRising:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, SCLK_POLARITY, Rising_aligned_with_sdata);
        break;
    case BAPE_I2sSclkPolarity_eFalling:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_IOP_CTRL_I2SIN_CFG0, SCLK_POLARITY, Falling_aligned_with_sdata);
        break;
    default:
        BDBG_ERR(("Invalid polarity of serial bit clock %d", pSettings->lrPolarity));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);

    return BERR_SUCCESS;
}

#endif

#endif

/***************************************************************************
        Test code to support I2S testing
***************************************************************************/
#ifdef  SETUP_PINMUX_FOR_I2S_IN_ON_7422

#include "bchp_aon_pin_ctrl.h"
#include "bchp_sun_top_ctrl.h"

static void BAPE_I2sInput_P_SetupPinmuxForI2sTesting( BAPE_Handle deviceHandle )
{
    uint32_t    reg;

    /* First, route the I2S0 inputs from somewhere that we can attach to them. It seems that we should be able to get
     * them from either aon_gpio pins 01,02,03, or aon_gpio pins 10,11,12. 
     */
    #if 1
        reg = BREG_Read32(deviceHandle->regHandle,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1);
    
        reg &=~(    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_10 ) |
                    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11 ) |
                    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12 ) );
    
        reg |=(    BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_10, 4 ) |  /* I2S_CLK0_IN  on J2303/14 (Front panel i/f connector) */
                   BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_11, 4 ) |  /* I2S_DATA0_IN on J2303/15 (Front panel i/f connector) */
                   BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_1, aon_gpio_12, 4 ) ); /* I2S_LR0_IN   on J2303/16 (Front panel i/f connector) */
    
        BREG_Write32 (deviceHandle->regHandle, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1, reg);
    #else
        reg = BREG_Read32(deviceHandle->regHandle,BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0);
    
        reg &=~(    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_01 ) |
                    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_02 ) |
                    BCHP_MASK(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_03 ) );
    
        reg |=(    BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_01, 2 ) |  /* I2S_CLK0_IN  on J2303/10 (Front panel i/f connector) */
                   BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_02, 2 ) |  /* I2S_DATA0_IN on J2303/11 (Front panel i/f connector) */
                   BCHP_FIELD_DATA(AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_03, 2 ) ); /* I2S_LR0_IN   on J2303/12 (Front panel i/f connector) */
    
        BREG_Write32 (deviceHandle->regHandle, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, reg);
    #endif
}
#endif /* SETUP_PINMUX_FOR_I2S_IN_ON_7422 */

