/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_dummy_output.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 5/3/12 5:06p $
 *
 * Module Description: Dummysink Output Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_dummy_output.c $
 * 
 * Hydra_Software_Devel/10   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:52p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/9   3/6/12 3:07p gskerl
 * SW7425-2570: Renamed OutputPortObject substruct from connector to
 * outputPort.
 * 
 * Hydra_Software_Devel/8   11/14/11 3:37p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/7   9/30/11 11:42a gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume.
 * 
 * Hydra_Software_Devel/6   5/20/11 5:16p jgarrett
 * SW7425-402: Adding error code to handle output enable callback failing.
 * 
 * Hydra_Software_Devel/5   5/3/11 10:50a gskerl
 * SW7422-354: Added index and type args to APE_P_InitOutputPort macro
 * 
 * Hydra_Software_Devel/4   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/3   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/2   3/22/11 3:10p gskerl
 * SW7422-146: Changed audio output connector callbacks to take the output
 * connector as an argument
 * 
 * Hydra_Software_Devel/1   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"

BDBG_MODULE(bape_dummy_output);

BDBG_OBJECT_ID(BAPE_DummyOutput);

typedef struct BAPE_DummyOutput
{
    BDBG_OBJECT(BAPE_DummyOutput)
    BAPE_Handle deviceHandle;
    unsigned index;
    BAPE_OutputPortObject outputPort;
    BAPE_DummysinkGroupHandle hDummysinkGroup;
#if BAPE_CHIP_MAX_FS > 0
    unsigned fsNum;
#else
    BAPE_MclkSource mclkSource;
    unsigned pllChannel;
    unsigned mclkFreqToFsRatio;
#endif
    bool enabled;
    char name[14];   /* DummyOutput %d */
} BAPE_DummyOutput;

/* Static function prototypes */
static BERR_Code BAPE_DummyOutput_P_OpenHw(BAPE_DummyOutputHandle handle);

/* Output port callbacks */
static BERR_Code BAPE_DummyOutput_P_Enable(BAPE_OutputPort output);
static void BAPE_DummyOutput_P_Disable(BAPE_OutputPort output);

/* Timing callbacks */
#if BAPE_CHIP_MAX_FS > 0
static void BAPE_DummyOutput_P_SetFs(BAPE_OutputPort output, unsigned fsNum);
#else
static void BAPE_DummyOutput_P_SetMclk_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio);
#endif

/***************************************************************************
        Public APIs: From bape_output.h
***************************************************************************/

void BAPE_DummyOutput_GetDefaultOpenSettings(
    BAPE_DummyOutputOpenSettings *pSettings       /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));    

    pSettings->maxMultichannelFormat = BAPE_MultichannelFormat_e2_0;
}

/**************************************************************************/

BERR_Code BAPE_DummyOutput_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_DummyOutputOpenSettings *pSettings,
    BAPE_DummyOutputHandle *pHandle             /* [out] */
    )
{
    BERR_Code errCode;
    BAPE_DummyOutputHandle handle;
    unsigned numChannelPairs=1;
    BAPE_DummysinkGroupCreateSettings dummysinkCreateSettings;
    BAPE_DummyOutputOpenSettings defaultSettings;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);

    BDBG_MSG(("%s: Opening DummyOutput: %u", __FUNCTION__, index));

    if ( index >= BAPE_CHIP_MAX_DUMMYSINKS )
    {
        BDBG_ERR(("Unable to open dummysink %u.  Max dummysinks is %u on this platform.", index, BAPE_CHIP_MAX_DUMMYSINKS));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( deviceHandle->dummyOutputs[index] )
    {
        BDBG_ERR(("DummyOutput %d already open", index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Init to specified settings */
    if ( NULL == pSettings )
    {
        BAPE_DummyOutput_GetDefaultOpenSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* Allocate the device structure, then fill in all the fields. */
    handle = BKNI_Malloc(sizeof(BAPE_DummyOutput));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BAPE_DummyOutput));
    BDBG_OBJECT_SET(handle, BAPE_DummyOutput);
    handle->deviceHandle = deviceHandle;
    handle->index = index;
    BAPE_P_InitOutputPort(&handle->outputPort, BAPE_OutputPortType_eDummyOutput, index, handle);
    BAPE_FMT_P_EnableSource(&handle->outputPort.capabilities, BAPE_DataSource_eFci);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcmRf);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_eIec61937);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_eIec61937x4);
    switch ( pSettings->maxMultichannelFormat )
    {
    case BAPE_MultichannelFormat_e2_0:
        numChannelPairs = 1;
        break;
    case BAPE_MultichannelFormat_e5_1:
        numChannelPairs = 3;
        BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcm5_1);
        break;
    case BAPE_MultichannelFormat_e7_1:
        numChannelPairs = 4;
        BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcm5_1);
        BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcm7_1);
        break;
    default:
        BDBG_ERR(("Unsupported multichannel format %u", pSettings->maxMultichannelFormat));
        BAPE_DummyOutput_Close(handle);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    handle->outputPort.enable = BAPE_DummyOutput_P_Enable;
    handle->outputPort.disable = BAPE_DummyOutput_P_Disable;
#if BAPE_CHIP_MAX_FS > 0
    handle->outputPort.fsTiming = true;
    handle->outputPort.setFs = BAPE_DummyOutput_P_SetFs;
    handle->fsNum = (unsigned)-1;
#else
    handle->outputPort.setMclk_isr = BAPE_DummyOutput_P_SetMclk_isr;
    handle->mclkSource = BAPE_MclkSource_eNone;
    handle->pllChannel = 0;
    handle->mclkFreqToFsRatio = BAPE_BASE_PLL_TO_FS_RATIO;
#endif
    BKNI_Snprintf(handle->name, sizeof(handle->name), "Dummysink %u", index); 
    handle->outputPort.pName = handle->name;

    BAPE_DummysinkGroup_P_GetDefaultCreateSettings(&dummysinkCreateSettings);
    dummysinkCreateSettings.numChannelPairs = numChannelPairs;
    errCode = BAPE_DummysinkGroup_P_Create(deviceHandle, &dummysinkCreateSettings, &handle->hDummysinkGroup);
    if ( errCode )
    {
        BAPE_DummyOutput_Close(handle);
        return BERR_TRACE(errCode);
    }

    errCode = BAPE_DummyOutput_P_OpenHw(handle);
    if ( errCode )
    {
        BAPE_DummyOutput_Close(handle);
        return BERR_TRACE(errCode);
    }

    *pHandle = handle;
    handle->deviceHandle->dummyOutputs[index] = handle;
    return BERR_SUCCESS;
}

/**************************************************************************/

void BAPE_DummyOutput_Close(
    BAPE_DummyOutputHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_DummyOutput);

    if ( handle->hDummysinkGroup )
    {
        BAPE_DummysinkGroup_P_Destroy(handle->hDummysinkGroup);
    }

    handle->deviceHandle->dummyOutputs[handle->index] = NULL;
    BDBG_OBJECT_DESTROY(handle, BAPE_DummyOutput);
    BKNI_Free(handle);
}

/**************************************************************************/

void BAPE_DummyOutput_GetOutputPort(
    BAPE_DummyOutputHandle handle,
    BAPE_OutputPort *pOutputPort
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_DummyOutput);
    BDBG_ASSERT(NULL != pOutputPort);
    *pOutputPort = &handle->outputPort;
}

/***************************************************************************
        BAPE Internal APIs: From bape_fmm_priv.h
***************************************************************************/

BERR_Code BAPE_DummyOutput_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    dummyOutputIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    /* For each opened DummyOutput, call the functions necessary to restore the
     * hardware to it's appropriate state.
     */
    for ( dummyOutputIndex=0 ; dummyOutputIndex<BAPE_CHIP_MAX_DUMMYSINKS ; dummyOutputIndex++ )
    {
        if ( bapeHandle->dummyOutputs[dummyOutputIndex] )       /* If this DummyOutput is open... */
        {
            BAPE_DummyOutputHandle hDummyOutput = bapeHandle->dummyOutputs[dummyOutputIndex];

            /* Put the HW into the generic open state. */
            errCode = BAPE_DummyOutput_P_OpenHw(hDummyOutput);
            if ( errCode ) return BERR_TRACE(errCode);
            
            /* Now apply changes for the settings struct. */
                /* Nothing to do for that because there's no SetSettings function. */

#if BAPE_CHIP_MAX_FS > 0
            /* Now restore the timebase and sampleRate from the values saved in the device struct. */
            if ( hDummyOutput->fsNum !=  (unsigned)-1 )
            {
                BAPE_DummyOutput_P_SetFs(&hDummyOutput->outputPort, hDummyOutput->fsNum);
            }
#else
            if ( hDummyOutput->mclkSource != BAPE_MclkSource_eNone )
            {
                BKNI_EnterCriticalSection();
                BAPE_DummyOutput_P_SetMclk_isr(&hDummyOutput->outputPort, hDummyOutput->mclkSource, hDummyOutput->pllChannel, hDummyOutput->mclkFreqToFsRatio);
                BKNI_LeaveCriticalSection();                
            }
#endif
        } /* end if this DummyOutput was open */
    } /* end for each DummyOutput */
    return errCode;
}


/***************************************************************************
        Private callbacks: Protyped above
***************************************************************************/
static BERR_Code BAPE_DummyOutput_P_Enable(BAPE_OutputPort output)
{
    BAPE_DummyOutputHandle handle;
    BAPE_DummysinkGroupSettings dummysinkSettings;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_DummyOutput);

    BDBG_ASSERT(false == handle->enabled);

    /* Set Input Parameters */
    BAPE_DummysinkGroup_P_GetSettings(handle->hDummysinkGroup, &dummysinkSettings);
    dummysinkSettings.input = handle->outputPort.sourceMixerFci;
    errCode = BAPE_DummysinkGroup_P_SetSettings(handle->hDummysinkGroup, &dummysinkSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_dummysink_settings;
    }

    /* Enable each dummysink */
    errCode = BAPE_DummysinkGroup_P_Start(handle->hDummysinkGroup);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_dummysink_start;
    }

    /* Done */
    handle->enabled = true;

err_dummysink_start:    
err_dummysink_settings:
    return errCode;
}

/**************************************************************************/

static void BAPE_DummyOutput_P_Disable(BAPE_OutputPort output)
{
    BAPE_DummyOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_DummyOutput);

    /* disable each dummysink */
    BAPE_DummysinkGroup_P_Stop(handle->hDummysinkGroup);

    /* Done */
    handle->enabled = false;
}

/**************************************************************************/

#if BAPE_CHIP_MAX_FS > 0
static void BAPE_DummyOutput_P_SetFs(BAPE_OutputPort output, unsigned fsNum)
{
    BAPE_DummyOutputHandle handle;
    BAPE_DummysinkGroupSettings dummysinkSettings;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_DummyOutput);

    BDBG_ASSERT(false == handle->enabled);

    /* Set Input Parameters */
    BAPE_DummysinkGroup_P_GetSettings(handle->hDummysinkGroup, &dummysinkSettings);
    dummysinkSettings.fs = fsNum;
    errCode = BAPE_DummysinkGroup_P_SetSettings(handle->hDummysinkGroup, &dummysinkSettings);

    handle->fsNum = fsNum;

    BDBG_ASSERT(BERR_SUCCESS == errCode);
}
#else
static void BAPE_DummyOutput_P_SetMclk_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio)
{
    BAPE_DummyOutputHandle handle;
    BAPE_DummysinkGroupSettings dummysinkSettings;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_DummyOutput);

    BDBG_ASSERT(false == handle->enabled);

    /* Set Input Parameters */
    BAPE_DummysinkGroup_P_GetSettings(handle->hDummysinkGroup, &dummysinkSettings);
    dummysinkSettings.mclkSource = mclkSource;
    dummysinkSettings.pllChannel = pllChannel;
    dummysinkSettings.mclkFreqToFsRatio = mclkFreqToFsRatio;
    errCode = BAPE_DummysinkGroup_P_SetSettings(handle->hDummysinkGroup, &dummysinkSettings);

    handle->mclkSource = mclkSource;
    handle->pllChannel = pllChannel;
    handle->mclkFreqToFsRatio = mclkFreqToFsRatio;

    BDBG_ASSERT(BERR_SUCCESS == errCode);
}
#endif

/***************************************************************************
        Private functions: Protyped above
***************************************************************************/

static BERR_Code BAPE_DummyOutput_P_OpenHw(BAPE_DummyOutputHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_DummyOutput);

    BSTD_UNUSED(handle);
    /* Nothing to do for now. */

    return BERR_SUCCESS;
}


