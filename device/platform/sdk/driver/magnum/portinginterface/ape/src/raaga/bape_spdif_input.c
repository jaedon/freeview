/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_spdif_input.c $
 * $brcm_Revision: Hydra_Software_Devel/28 $
 * $brcm_Date: 5/18/12 2:34p $
 *
 * Module Description: SPDIF Audio Input Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_spdif_input.c $
 * 
 * Hydra_Software_Devel/28   5/18/12 2:34p gskerl
 * SW7425-2075: Brought back in sync with bape_mai_input.c by merging
 * recent halt-related changes.
 * 
 * Hydra_Software_Devel/27   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/3   4/12/12 1:54p jgarrett
 * SW7425-2075: Revising InputPort format handling
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/11/12 11:12a gskerl
 * SW7425-2075: Enter critical section before calling _isr functions.
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:51p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/26   3/23/12 10:43a gskerl
 * SW7429-18: Fixed compile error on 7425 and 7435.
 * 
 * Hydra_Software_Devel/25   3/22/12 3:08p gskerl
 * SW7429-18: Moved preamble C parsing logic into
 * BAPE_InputPort_P_BurstPreambleToCodec().
 * 
 * Hydra_Software_Devel/24   3/8/12 5:41p gskerl
 * SW7429-18: Lots of cleanup, reorganization, and refactoring.
 * 
 * Hydra_Software_Devel/23   2/9/12 11:21a jgarrett
 * SW7425-2074: Enabling input halt and revising MAI shutown for
 * multichannel PCM
 * 
 * Hydra_Software_Devel/22   2/2/12 6:58p gskerl
 * SW7429-18: Merged to Hydra_Software_Devel branch.
 * 
 * Hydra_Software_Devel/SW7429-18_20120120/4   2/2/12 6:53p gskerl
 * SW7429-18: Added debug prints for goodBiPhase, leftValidity, and
 * rightValidity fields of the BAPE_SpdifInputFormatDetectionStatus
 * struct.
 * 
 * Hydra_Software_Devel/SW7429-18_20120120/3   2/1/12 4:46p gskerl
 * SW7429-18: Added some code to update our local state with whatever
 * we're reporting through the status change callback. Misc cleanup
 * 
 * Hydra_Software_Devel/SW7429-18_20120120/2   1/23/12 5:31p gskerl
 * SW7429-18: Added an abstract name for the SPDIF receiver control
 * register.
 * 
 * Hydra_Software_Devel/SW7429-18_20120120/1   1/20/12 12:36p gskerl
 * SW7429-18: Started refactoring and cleanup... Compiles on 7425
 * 
 * Hydra_Software_Devel/20   1/6/12 2:38p jgarrett
 * SW7435-24: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7435-24/3   12/15/11 4:34p mward
 * SW7435-24:  Merge from main.
 * 
 * Hydra_Software_Devel/SW7435-24/2   11/30/11 12:11p mward
 * SW7435-24:  Merge from main.
 * 
 * Hydra_Software_Devel/SW7435-24/1   11/3/11 4:34p mward
 * SW7435-24:  Add 7435.
 * 
 * Hydra_Software_Devel/19   11/30/11 4:26p jgarrett
 * SW7429-18: Adding stubs for SPDIF/MAI inputs on 7429
 * 
 * Hydra_Software_Devel/18   11/14/11 3:38p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/2   11/4/11 5:37p gskerl
 * SW7429-18: Added conditional include for bchp_int_id_aio_inth.h to fix
 * compile errors on 7425
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/26/11 12:44p jgarrett
 * SW7429-18: Merging latest changes from main branch
 * 
 * Hydra_Software_Devel/17   10/26/11 6:00p gskerl
 * SW7231-129: Added stub version of
 * BAPE_SpdifInput_P_PrepareForStandby().
 * 
 * Hydra_Software_Devel/16   10/26/11 5:34p gskerl
 * SW7231-129: Added BAPE_SpdifInput_P_PrepareForStandby() to disable
 * interrupts before entering standby.
 * 
 * Hydra_Software_Devel/15   10/26/11 3:43p gskerl
 * SW7231-129: Added code to invalidate the saved SPDIF receiver state
 * before reinitializing the SPDIF receiver hardware.
 * 
 * Hydra_Software_Devel/14   10/25/11 1:25p gskerl
 * SW7231-129: Added stub version of BAPE_SpdifInput_P_ResumeFromStandby()
 * 
 * Hydra_Software_Devel/13   10/12/11 4:49p gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume.
 * 
 * Hydra_Software_Devel/12   8/24/11 1:44p jgarrett
 * SWDTV-8271: Reversing SPDIF/HDMI input ISRs on 352xx
 * 
 * Hydra_Software_Devel/11   8/16/11 4:10p jgarrett
 * SWDTV-8271: Correcting IOP Capture ID for STB
 * 
 * Hydra_Software_Devel/10   8/10/11 9:30a venkatr
 * SWDTV-6584 : [35233] Add ADC Input for APE
 * 
 * Hydra_Software_Devel/9   5/12/11 11:42a gskerl
 * SW7422-354: Fixed to support compilation on 35230
 * 
 * Hydra_Software_Devel/8   5/2/11 5:28p gskerl
 * SW7422-354: Removed explicit setting of handle->connector.type because
 * it was already set by the BAPE_P_InitInputPort macro
 * 
 * Hydra_Software_Devel/7   4/16/11 12:16p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/6   4/12/11 9:22a jgarrett
 * SW7422-365: Fixing build issues
 * 
 * Hydra_Software_Devel/5   4/11/11 6:24p gskerl
 * SW7422-365: Fixed a compile error
 * 
 * Hydra_Software_Devel/4   4/8/11 3:39p piyushg
 * SW7422-365: Added support for the SPDIF input handling and format
 * detection in APE
 * 
 * Hydra_Software_Devel/3   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
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

#ifdef BCHP_AIO_INTH_REG_START
    #include "bchp_int_id_aio_inth.h"
#endif

#ifdef BCHP_AUD_INTH_REG_START
    #include "bchp_int_id_aud_inth.h"
#endif

BDBG_MODULE(bape_spdif_input);

BDBG_OBJECT_ID(BAPE_SpdifInput);

#if BAPE_CHIP_MAX_SPDIF_INPUTS > 0

typedef struct BAPE_SpdifInput
{
    BDBG_OBJECT(BAPE_SpdifInput)
    BAPE_Handle deviceHandle;
    BAPE_SpdifInputSettings settings;
    unsigned index;                 
    BAPE_InputPortObject inputPort;
    uint32_t offset;  
    BAPE_SpdifInputFormatDetectionSettings clientFormatDetectionSettings;
    bool                            localFormatDetectionEnabled;
    bool                            formatDetectionEnabled;
    uint32_t outFormatEna;      /* last value written to OUT_FORMAT_ENA field. */
    bool enable;
    char name[12];   /* SPDIF IN %d */

    BINT_CallbackHandle spdifRxCallback;

    BAPE_SpdifInputFormatDetectionStatus  lastFormatDetectionStatus;
} BAPE_SpdifInput;


#if BCHP_CHIP == 7422 || BCHP_CHIP == 7425 || BCHP_CHIP == 7435
#define BAPE_SPDIF_INPUT_CAPTURE_ID() (9)
#elif BCHP_CHIP == 35230 || BCHP_CHIP == 35125 || BCHP_CHIP == 35233 || BCHP_CHIP == 35126
#define BAPE_SPDIF_INPUT_CAPTURE_ID() (11)
#elif defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN
/* 7429-style chips do not need this hardcoded */
#else
#error SPDIF capture ID not defined
#endif

/* Build some abstract register names and map them to the chip's real names. */
#if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN
    #define BAPE_P_SPDIF_RX_STATUS_REGADDR        BCHP_AUD_FMM_IOP_IN_SPDIF_0_STATUS
    #define BAPE_P_SPDIF_RX_STATUS_REGNAME             AUD_FMM_IOP_IN_SPDIF_0_STATUS
    #define BAPE_P_SPDIF_RX_CONFIG_REGADDR        BCHP_AUD_FMM_IOP_IN_SPDIF_0_CONFIG
    #define BAPE_P_SPDIF_RX_CONFIG_REGNAME             AUD_FMM_IOP_IN_SPDIF_0_CONFIG
#endif
#if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY
    #define BAPE_P_SPDIF_RX_STATUS_REGADDR       BCHP_SPDIF_RCVR_CTRL_STATUS
    #define BAPE_P_SPDIF_RX_STATUS_REGNAME            SPDIF_RCVR_CTRL_STATUS
    #define BAPE_P_SPDIF_RX_CONFIG_REGADDR       BCHP_SPDIF_RCVR_CTRL_CONFIG
    #define BAPE_P_SPDIF_RX_CONFIG_REGNAME            SPDIF_RCVR_CTRL_CONFIG
#endif


/* Static function prototypes */
static BERR_Code    BAPE_SpdifInput_P_OpenHw(BAPE_SpdifInputHandle hSpdifInput);
static BERR_Code    BAPE_SpdifInput_P_ApplySettings(BAPE_SpdifInputHandle hSpdifInput);
static void         BAPE_SpdifInput_P_DetectInputChange_isr (BAPE_SpdifInputHandle    hSpdifInput);
static BERR_Code    BAPE_SpdifInput_P_GetFormatDetectionStatus_isr(BAPE_SpdifInputHandle hSpdifInput, BAPE_SpdifInputFormatDetectionStatus *pStatus );
static void         BAPE_P_SPDIF_RX_isr (void * pParm1, int iParm2); 
static void         BAPE_SpdifInput_P_SetReceiverOutputFormat_isr (BAPE_SpdifInputHandle hSpdifInput, BAPE_SpdifInputFormatDetectionStatus *pFormatDetectionStatus);
static BERR_Code    BAPE_SpdifInput_P_SetFormatDetection_isr (BAPE_SpdifInputHandle hSpdifInput);
static void         BAPE_SpdifInput_P_UpdateFormat_isr (BAPE_SpdifInputHandle hSpdifInput, BAPE_SpdifInputFormatDetectionStatus *pFormatDetectionStatus, BAPE_FMT_Descriptor *pFormat);

/* Input port callbacks */
static void         BAPE_SpdifInput_P_Enable(BAPE_InputPort inputPort);
static void         BAPE_SpdifInput_P_Disable(BAPE_InputPort inputPort);
static BERR_Code    BAPE_SpdifInput_P_ConsumerAttached_isr(BAPE_InputPort inputPort, BAPE_PathNode *pConsumer, BAPE_FMT_Descriptor *pFormat);
static void         BAPE_SpdifInput_P_ConsumerDetached_isr(BAPE_InputPort inputPort, BAPE_PathNode *pConsumer);

/***************************************************************************
        Public APIs: From bape_input.h
***************************************************************************/
void BAPE_SpdifInput_GetDefaultSettings(
    BAPE_SpdifInputSettings *pSettings        /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->errorInsertion = BAPE_SpdifInputErrorInsertion_eNone;
    pSettings->ignoreCompressedParity = true;
    pSettings->ignorePcmParity = true;
    pSettings->ignoreValidity = true;
    pSettings->timebase = BAVC_Timebase_e0; 
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_Open(
    BAPE_Handle hApe,
    unsigned index,
    const BAPE_SpdifInputSettings *pSettings,
    BAPE_SpdifInputHandle *phSpdifInput              /* [out] */
    )
{
    BERR_Code errCode;
    BAPE_FMT_Descriptor format;
    BAPE_SpdifInputHandle hSpdifInput;
    BAPE_SpdifInputSettings defaultSettings;

    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);
    BDBG_ASSERT(NULL != phSpdifInput);
    
    BDBG_MSG(("%s: Opening SPDIF Input: %u", __FUNCTION__, index));

    *phSpdifInput = NULL;

    if ( index >= BAPE_CHIP_MAX_SPDIF_INPUTS )
    {
        BDBG_ERR(("Request to open SPDIF input %d but chip only has %u SPDIF inputs", index, BAPE_CHIP_MAX_SPDIF_INPUTS ));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( hApe->spdifInputs[index] )
    {
        BDBG_ERR(("SPDIF input %d already open", index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Allocate the device structure, then fill in all the fields. */
    hSpdifInput = BKNI_Malloc(sizeof(BAPE_SpdifInput));
    if ( NULL == hSpdifInput )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* Init to specified settings */
    if ( NULL == pSettings )
    {
        BAPE_SpdifInput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    BKNI_Memset(hSpdifInput, 0, sizeof(BAPE_SpdifInput));
    BDBG_OBJECT_SET(hSpdifInput, BAPE_SpdifInput);
    BAPE_P_InitInputPort(&hSpdifInput->inputPort, BAPE_InputPortType_eSpdif, index, hSpdifInput);
    hSpdifInput->spdifRxCallback = NULL;
    hSpdifInput->deviceHandle = hApe;
    hSpdifInput->index = index;
    #ifdef BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN
    {
        uint32_t regVal = BAPE_Reg_P_Read(hApe, BCHP_AUD_FMM_IOP_IN_SPDIF_0_CAPTURE_FCI_ID_TABLE);
        regVal = BCHP_GET_FIELD_DATA(regVal, AUD_FMM_IOP_IN_SPDIF_0_CAPTURE_FCI_ID_TABLE, START_FCI_ID);
        hSpdifInput->inputPort.streamId[BAPE_ChannelPair_eLeftRight] = regVal;
    }
    #endif
    #ifdef BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY
        hSpdifInput->inputPort.streamId[BAPE_ChannelPair_eLeftRight] = BAPE_SPDIF_INPUT_CAPTURE_ID();
    #endif
    hSpdifInput->inputPort.enable = BAPE_SpdifInput_P_Enable;
    hSpdifInput->inputPort.disable = BAPE_SpdifInput_P_Disable;
    hSpdifInput->inputPort.consumerAttached_isr = BAPE_SpdifInput_P_ConsumerAttached_isr;
    hSpdifInput->inputPort.consumerDetached_isr = BAPE_SpdifInput_P_ConsumerDetached_isr;
    {   /* Start Critical Section */
        BKNI_EnterCriticalSection();
        BAPE_InputPort_P_GetFormat_isr(&hSpdifInput->inputPort, &format);
        format.source = BAPE_DataSource_eFci;
        format.type = BAPE_DataType_ePcmStereo;
        format.sampleRate = 48000;
        (void)BAPE_InputPort_P_SetFormat_isr(&hSpdifInput->inputPort, &format);
        BKNI_LeaveCriticalSection();
    }   /* End Critical Section */

    BKNI_Snprintf(hSpdifInput->name, sizeof(hSpdifInput->name), "SPDIF IN %u", index);
    hSpdifInput->inputPort.pName = hSpdifInput->name;

    /* Currently we only support one SPDIF input, this can be expanded later if we support more */
    #if BAPE_CHIP_MAX_SPDIF_INPUTS > 1
        #error "Need to support more SPDIF inputs"
    #endif
    hSpdifInput->offset = 0;

    BDBG_ASSERT(hSpdifInput->offset == 0);

    errCode = BAPE_SpdifInput_P_OpenHw(hSpdifInput);
    if ( errCode )
    {
        BAPE_SpdifInput_Close(hSpdifInput);
        return BERR_TRACE(errCode);
    }

    errCode = BAPE_SpdifInput_SetSettings(hSpdifInput, pSettings);
    if ( errCode )
    {
        BAPE_SpdifInput_Close(hSpdifInput);
        return BERR_TRACE(errCode);
    }

    *phSpdifInput = hSpdifInput;
    hSpdifInput->deviceHandle->spdifInputs[index] = hSpdifInput;
    return BERR_SUCCESS;
}

/**************************************************************************/

void BAPE_SpdifInput_Close(
    BAPE_SpdifInputHandle hSpdifInput
    )
{
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);

    /* Make sure we're not still connected to anything */
    if ( hSpdifInput->inputPort.pConsumer )
    {
        BDBG_ERR(("Cannot close SPDIF input %p (%d), still connected to %s", hSpdifInput, hSpdifInput->index, hSpdifInput->inputPort.pConsumer->pName));
        BDBG_ASSERT(NULL == hSpdifInput->inputPort.pConsumer);
        return;
    }

    if ( hSpdifInput->spdifRxCallback ) {
        BINT_DestroyCallback(hSpdifInput->spdifRxCallback);
    }

    hSpdifInput->deviceHandle->spdifInputs[hSpdifInput->index] = NULL;
    BDBG_OBJECT_DESTROY(hSpdifInput, BAPE_SpdifInput);
    BKNI_Free(hSpdifInput);    
}

/**************************************************************************/

void BAPE_SpdifInput_GetSettings(
    BAPE_SpdifInputHandle hSpdifInput,
    BAPE_SpdifInputSettings *pSettings        /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = hSpdifInput->settings;
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_SetSettings(
    BAPE_SpdifInputHandle hSpdifInput,
    const BAPE_SpdifInputSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);
    BDBG_ASSERT(NULL != pSettings);
    hSpdifInput->settings = *pSettings;

    return BAPE_SpdifInput_P_ApplySettings(hSpdifInput);
}

/**************************************************************************/

void BAPE_SpdifInput_GetInputPort(
    BAPE_SpdifInputHandle hSpdifInput,
    BAPE_InputPort *pPort
    )
{
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);
    BDBG_ASSERT(NULL != pPort);
    *pPort = &hSpdifInput->inputPort;
}

/**************************************************************************/

void BAPE_SpdifInput_GetFormatDetectionSettings(
    BAPE_SpdifInputHandle hSpdifInput,
    BAPE_SpdifInputFormatDetectionSettings *pSettings   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);
    BDBG_ASSERT(NULL != pSettings);

    {   /* Start Critical Section */
        BKNI_EnterCriticalSection();
        *pSettings = hSpdifInput->clientFormatDetectionSettings;
        BKNI_LeaveCriticalSection();
    }   /* End Critical Section */
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_SetFormatDetectionSettings(
    BAPE_SpdifInputHandle hSpdifInput,
    const BAPE_SpdifInputFormatDetectionSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);
    BDBG_ASSERT(NULL != pSettings);

    {  /* Start Critical Section */
        BKNI_EnterCriticalSection();
        hSpdifInput->clientFormatDetectionSettings = *pSettings;
        BAPE_SpdifInput_P_SetFormatDetection_isr(hSpdifInput);
        BKNI_LeaveCriticalSection();
    }  /* End Critical Section */


    return BERR_SUCCESS;
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_GetFormatDetectionStatus(
    BAPE_SpdifInputHandle hSpdifInput,
    BAPE_SpdifInputFormatDetectionStatus *pStatus
    )
{
    BERR_Code ret = BERR_SUCCESS;
    
    BDBG_ENTER(BAPE_SpdifInput_GetFormatDetectionStatus);
    
    BDBG_ASSERT(hSpdifInput);
    BDBG_ASSERT(pStatus);

    if ( hSpdifInput->formatDetectionEnabled )
    {
        /* If Format Detection is enabled, we can just return a copy of whats
         * in the handle's lastFormatDectionStatus structure (since it's being
         * kept up to date). */
        BDBG_CASSERT(sizeof (*pStatus) == sizeof (hSpdifInput->lastFormatDetectionStatus));
        BKNI_Memcpy(pStatus, &hSpdifInput->lastFormatDetectionStatus, sizeof (*pStatus));
    }
    else
    {
        /* Format Detection is not enabled, so build a fresh format detection status
           structure by reading the hardware.  */
        {  /* Start Critical Section */
            BKNI_EnterCriticalSection();
            BAPE_SpdifInput_P_GetFormatDetectionStatus_isr(hSpdifInput, pStatus);
            BKNI_LeaveCriticalSection();
        }  /* End Critical Section */
    }

    BDBG_LEAVE(BAPE_SpdifInput_GetFormatDetectionStatus);

    return ret;
}

/***************************************************************************
        BAPE Internal APIs: From bape_fmm_priv.h
***************************************************************************/

BERR_Code BAPE_SpdifInput_P_PrepareForStandby(BAPE_Handle hApe)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    spdifInputIndex;

    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);

    /* For each opened SpdifInput... */
    for ( spdifInputIndex=0 ; spdifInputIndex<BAPE_CHIP_MAX_SPDIF_INPUTS ; spdifInputIndex++ )
    {
        if ( hApe->spdifInputs[spdifInputIndex] )       /* If this SpdifInput is open... */
        {
            BAPE_SpdifInputHandle hSpdifInput = hApe->spdifInputs[spdifInputIndex];

            /* If we already have a callback (interrupt), make sure that it's disabled, then destroy it. */
            if( hSpdifInput->spdifRxCallback )
            {
                errCode = BINT_DisableCallback(hSpdifInput->spdifRxCallback);
                if (errCode != BERR_SUCCESS)                             
                {                                                    
                    BDBG_ERR(("Unable to Disable SPDIF RX callback")); 
                }

                BINT_DestroyCallback(hSpdifInput->spdifRxCallback);
                hSpdifInput->spdifRxCallback = NULL;
            }
        }
    }
    return errCode;
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    spdifInputIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    /* For each opened SpdifInput, call the functions necessary to restore the hardware to it's appropriate state. */
    for ( spdifInputIndex=0 ; spdifInputIndex<BAPE_CHIP_MAX_SPDIF_INPUTS ; spdifInputIndex++ )
    {
        if ( bapeHandle->spdifInputs[spdifInputIndex] )       /* If this SpdifInput is open... */
        {
            BAPE_SpdifInputHandle hSpdifInput = bapeHandle->spdifInputs[spdifInputIndex];

            /* Put the HW into the generic open state. */
            errCode = BAPE_SpdifInput_P_OpenHw(hSpdifInput);
            if ( errCode ) return BERR_TRACE(errCode);
            
            /* Now apply changes for the settings struct. */
            errCode = BAPE_SpdifInput_SetSettings(hSpdifInput, &hSpdifInput->settings);
            if ( errCode ) return BERR_TRACE(errCode);

            /* Now restore the dynamic stuff from the values saved in the device struct. */
            errCode = BAPE_SpdifInput_SetFormatDetectionSettings(hSpdifInput, &hSpdifInput->clientFormatDetectionSettings);
            if ( errCode ) return BERR_TRACE(errCode);
        }
    }
    return errCode;
}

/***************************************************************************
        Private callbacks: Protyped above
***************************************************************************/
static void BAPE_SpdifInput_P_Enable(BAPE_InputPort inputPort)
{
    BAPE_SpdifInputHandle hSpdifInput;

    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    hSpdifInput = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);
    BDBG_ASSERT(false == hSpdifInput->enable);

    BDBG_MSG(("Enabling %s", hSpdifInput->name));

    /* Enable capture */
    #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN  /* Local (SPDIF-specific) STREAM_CFG register */
        BAPE_Reg_P_UpdateField(hSpdifInput->deviceHandle, BCHP_AUD_FMM_IOP_IN_SPDIF_0_CAP_STREAM_CFG_0,
                                                               AUD_FMM_IOP_IN_SPDIF_0_CAP_STREAM_CFG_0, CAP_ENA, 1);
    #elif defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY  /* Shared pool of STREAM_CFG registers in IOP */
        BAPE_Iop_P_EnableCapture( hSpdifInput->deviceHandle, inputPort->streamId[0], BAPE_FMT_P_GetNumChannelPairs(&inputPort->format) );
    #else
        #error "Unknown SPDIF register format"
    #endif /* if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN... BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY */

    /* Enable output FIFO */
    {  /* Start Critical Section */
        BKNI_EnterCriticalSection();

        if ( inputPort->halted )
        {
            BDBG_MSG(("Input %s is halted. can't enable, disabling instead", hSpdifInput->name ));
            /* This handles the unlikely case of input being halted before the consumer start was complete.
             * Since the input is halted, do a disable instead of enabling... which is the same thing
             * that happens when a halt occurs after enabling.  */
            BAPE_Reg_P_UpdateEnum_isr(hSpdifInput->deviceHandle, BAPE_P_SPDIF_RX_CONFIG_REGADDR,
                                                                 BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUTFIFO_ENA, Disable);
            hSpdifInput->enable = false;
        }
        else
        {
            /* Normal case.  Input is not halted, so enable the input. */
            BAPE_Reg_P_UpdateEnum_isr(hSpdifInput->deviceHandle, BAPE_P_SPDIF_RX_CONFIG_REGADDR,
                                                                 BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUTFIFO_ENA, Enable);
            hSpdifInput->enable = true;
        }

        BKNI_LeaveCriticalSection();
    }  /* End Critical Section */
}

/**************************************************************************/

static void BAPE_SpdifInput_P_Disable(BAPE_InputPort inputPort)
{
    BAPE_SpdifInputHandle hSpdifInput;

    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    hSpdifInput = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);

    BDBG_MSG(("Disabling %s", hSpdifInput->name));

    /* Disable output FIFO */
    {  /* Start Critical Section */
        BKNI_EnterCriticalSection();
        BAPE_Reg_P_UpdateEnum_isr(hSpdifInput->deviceHandle, BAPE_P_SPDIF_RX_CONFIG_REGADDR,
                                                             BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUTFIFO_ENA, Disable);
        hSpdifInput->enable = false;
        BKNI_LeaveCriticalSection();
    }  /* End Critical Section */

    /* Disable capture */
    #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN  /* Local (SPDIF-specific) STREAM_CFG register */
        BAPE_Reg_P_UpdateField(hSpdifInput->deviceHandle, BCHP_AUD_FMM_IOP_IN_SPDIF_0_CAP_STREAM_CFG_0,
                                                               AUD_FMM_IOP_IN_SPDIF_0_CAP_STREAM_CFG_0, CAP_ENA, 0);

    #elif defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY  /* Shared pool of STREAM_CFG registers in IOP */
        BAPE_Iop_P_DisableCapture( hSpdifInput->deviceHandle, inputPort->streamId[0], BAPE_FMT_P_GetNumChannelPairs(&inputPort->format) );
    #else
        #error "Unknown SPDIF register format"
    #endif /* if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN... BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY */
}

/**************************************************************************/

static void BAPE_SpdifInput_P_Halt_isr(BAPE_InputPort inputPort)
{
    BAPE_SpdifInputHandle hSpdifInput;

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    hSpdifInput = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);

    BDBG_MSG(("Halting %s", hSpdifInput->name));

    /* Remember the halted state. */
    BAPE_InputPort_P_Halt_isr(inputPort);

    /* Disable the output FIFO. */
    if ( hSpdifInput->enable )
    {
        BAPE_Reg_P_UpdateEnum_isr(hSpdifInput->deviceHandle, BAPE_P_SPDIF_RX_CONFIG_REGADDR,
                                                             BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUTFIFO_ENA, Disable);
    }
}

/**************************************************************************/

static BERR_Code BAPE_SpdifInput_P_ConsumerAttached_isr(BAPE_InputPort inputPort, BAPE_PathNode *pConsumer, BAPE_FMT_Descriptor *pFormat)
{
    BAPE_SpdifInputHandle hSpdifInput;

    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    hSpdifInput = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);

    BDBG_MSG(("Attaching consumer %s", pConsumer->pName));

    switch ( pConsumer->type )
    {
    case BAPE_PathNodeType_eDecoder:
    case BAPE_PathNodeType_eInputCapture:
        break;
    default:
        BDBG_ERR(("Node %u is not a valid consumer for SPDIF Input", pConsumer->pName));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_ASSERT(inputPort->pConsumer == pConsumer);

    /* Enable format detection interupts.  After this, the current input format
     * will be maintained in the handle's lastFormatDetectionStatus struct.  */
    hSpdifInput->localFormatDetectionEnabled = true;
    BAPE_SpdifInput_P_SetFormatDetection_isr(hSpdifInput);

    /* Update the InputPort's fields to match the current input format. */
    BAPE_InputPort_P_GetFormat_isr(&hSpdifInput->inputPort, pFormat);
    BAPE_SpdifInput_P_UpdateFormat_isr (hSpdifInput, &hSpdifInput->lastFormatDetectionStatus, pFormat);
    BAPE_SpdifInput_P_SetReceiverOutputFormat_isr (hSpdifInput, &hSpdifInput->lastFormatDetectionStatus);
    (void)BAPE_InputPort_P_SetFormat_isr(&hSpdifInput->inputPort, pFormat);

    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_SpdifInput_P_ConsumerDetached_isr(BAPE_InputPort inputPort, BAPE_PathNode *pConsumer)
{
    BAPE_SpdifInputHandle hSpdifInput;

    BSTD_UNUSED(pConsumer);

    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    hSpdifInput = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);

    /* Disable local format detection. */
    hSpdifInput->localFormatDetectionEnabled = false;
    BAPE_SpdifInput_P_SetFormatDetection_isr(hSpdifInput);

    return;
}

/***************************************************************************
        Private functions: Protyped above
***************************************************************************/

static BERR_Code BAPE_SpdifInput_P_OpenHw(BAPE_SpdifInputHandle hSpdifInput)
{

    BAPE_Handle hApe;
    BAPE_Reg_P_FieldList regFieldList;
    BERR_Code errCode = BERR_SUCCESS;

    /* Currently we only support one SPDIF input, this can be expanded later if we support more */
    #if BAPE_CHIP_MAX_SPDIF_INPUTS > 1
        #error "Need to support more SPDIF inputs"
    #endif

    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);

    hApe = hSpdifInput->deviceHandle;
    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);

    /* Taken from RAP PI (regarding ALLOW_NZ_STUFFING=Nonzero_Ok) -->                                                                       
    PR 35668: Some Blu Ray DVD Players send Non Zero values between compressed
    frames. This was getting treated as PCM data and causing confusion in 3563
    and a workaround was put. In 3548 this has been fixed in hardware.        
    Enabling the bit here.                                                    
    */                                                                        

    BAPE_Reg_P_InitFieldList(hApe, &regFieldList);

    if( BAPE_FMT_P_IsLinearPcm(&hSpdifInput->inputPort.format) )
    {            
        /* Set the output format ena to PCM */
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PCM);
        hSpdifInput->outFormatEna = BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PCM);  /* Remember OUT_FORMAT_ENA setting. */
    }
    else
    {
        /* Set the output format ena to PES */
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PES);
        hSpdifInput->outFormatEna = BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PES);  /* Remember OUT_FORMAT_ENA setting. */
    }

    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, ALLOW_NZ_STUFFING, Nonzero_OK);

    #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, SOURCE_SEL, SPDIF);
    #endif /* if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY */

    BAPE_Reg_P_ApplyFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGADDR);

    /* Clear the interrupts before enabling any callbacks */
    #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN
        /* Clear and unmask L3 interrupts. */
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_IN_SPDIF_0_ESR_MASK_SET,     0xffffffff);
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_IN_SPDIF_0_ESR_STATUS_CLEAR, 0xffffffff);
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_IN_SPDIF_0_ESR_MASK_CLEAR,  BCHP_MASK( AUD_FMM_IOP_IN_SPDIF_0_ESR_MASK_CLEAR, SPDIFRX_CHAN_STAT_MASK) |
                                                                            BCHP_MASK( AUD_FMM_IOP_IN_SPDIF_0_ESR_MASK_CLEAR, SPDIFRX_V_CHANGE_MASK)  |
                                                                            BCHP_MASK( AUD_FMM_IOP_IN_SPDIF_0_ESR_MASK_CLEAR, SPDIFRX_PC_CHANGE_MASK) |
                                                                            BCHP_MASK( AUD_FMM_IOP_IN_SPDIF_0_ESR_MASK_CLEAR, SPDIFRX_GOOD_FMT_MASK)  |
                                                                            BCHP_MASK( AUD_FMM_IOP_IN_SPDIF_0_ESR_MASK_CLEAR, SPDIFRX_FMT_ERR_MASK)   |
                                                                            BCHP_MASK( AUD_FMM_IOP_IN_SPDIF_0_ESR_MASK_CLEAR, SPDIFRX_RATE_CHANGE_MASK) );

        /* Clear and unmask L2 interrupt. */
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_MISC_ESR_MASK_SET,     BCHP_MASK( AUD_FMM_IOP_MISC_ESR_MASK_SET,     IOP_INTERRUPT_IN_SPDIF_0));
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_MISC_ESR_STATUS_CLEAR, BCHP_MASK( AUD_FMM_IOP_MISC_ESR_STATUS_CLEAR, IOP_INTERRUPT_IN_SPDIF_0));
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_MISC_ESR_MASK_CLEAR,   BCHP_MASK( AUD_FMM_IOP_MISC_ESR_MASK_CLEAR,   IOP_INTERRUPT_IN_SPDIF_0));

    #endif /* if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN */
    #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY
    /* Clear and unmask L2 interrupts. */
    BAPE_Reg_P_Write(hApe, BCHP_SPDIF_RCVR_ESR_MASK_SET,   0xffffffff);
    BAPE_Reg_P_Write(hApe, BCHP_SPDIF_RCVR_ESR_STATUS_CLEAR, 0xffffffff);
    BAPE_Reg_P_Write(hApe, BCHP_SPDIF_RCVR_ESR_MASK_CLEAR, BCHP_MASK( SPDIF_RCVR_ESR_MASK_CLEAR, SPDIFRX_CHAN_STAT_MASK ) |
                                                           BCHP_MASK( SPDIF_RCVR_ESR_MASK_CLEAR, SPDIFRX_V_CHANGE_MASK )  |
                                                           BCHP_MASK( SPDIF_RCVR_ESR_MASK_CLEAR, SPDIFRX_PC_CHANGE_MASK ) |
                                                           BCHP_MASK( SPDIF_RCVR_ESR_MASK_CLEAR, SPDIFRX_GOOD_FMT_MASK )  |
                                                           BCHP_MASK( SPDIF_RCVR_ESR_MASK_CLEAR, SPDIFRX_FMT_ERR_MASK )   |
                                                           BCHP_MASK( SPDIF_RCVR_ESR_MASK_CLEAR, SPDIFRX_RATE_CHANGE_MASK ));
    #endif /* if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY */

    {
        #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN
            BINT_Id             intId = BCHP_INT_ID_AUD_IOP;  
        #endif /* if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN */
        #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY
            BINT_Id             intId = BCHP_INT_ID_SPDIFRX;  
        #endif /* if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY */

        errCode = BINT_CreateCallback( &hSpdifInput->spdifRxCallback,
                                       hApe->intHandle, 
                                       intId, 
                                       BAPE_P_SPDIF_RX_isr, 
                                       (void*)hSpdifInput,          
                                       0 /* Not used*/       
                                      );
    }
    if (errCode != BERR_SUCCESS) 
    {
        BDBG_ERR(("Unable to create SPDIF RX callback"));
        return errCode;
    }

    return errCode;
}

/**************************************************************************/

static BERR_Code BAPE_SpdifInput_P_ApplySettings(BAPE_SpdifInputHandle hSpdifInput)
{
    BAPE_Reg_P_FieldList regFieldList;
    const BAPE_SpdifInputSettings *pSettings = &hSpdifInput->settings;

    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);
    BDBG_ASSERT(NULL != pSettings);

    BAPE_Reg_P_InitFieldList(hSpdifInput->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, IGNORE_VALID_PCM, pSettings->ignoreValidity);
    BAPE_Reg_P_AddToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, IGNORE_PERR_PCM,  pSettings->ignorePcmParity);
    BAPE_Reg_P_AddToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, IGNORE_PERR_COMP, pSettings->ignoreCompressedParity);
    BAPE_Reg_P_AddToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, TIMEBASE_SEL,     pSettings->timebase);

    switch ( pSettings->errorInsertion ) 
    {
    case BAPE_SpdifInputErrorInsertion_eNone:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, INSERT_MODE, No_insert);
        break;
    case BAPE_SpdifInputErrorInsertion_eZero:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, INSERT_MODE, Insert_zero);
        break;
    case BAPE_SpdifInputErrorInsertion_eRepeat:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGNAME, INSERT_MODE, Insert_repeat);
        break;
    default:
        BDBG_ERR(("Invalid Error Insertion Mode %d", pSettings->errorInsertion));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    {  /* Start Critical Section */
        BKNI_EnterCriticalSection();
        BAPE_Reg_P_ApplyFieldList(&regFieldList, BAPE_P_SPDIF_RX_CONFIG_REGADDR);
        BKNI_LeaveCriticalSection();
    }  /* End Critical Section */

    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_P_SPDIF_RX_isr (
        void * pParm1, /* [in] channel handle */
        int    iParm2  /* [in] Not used */        
)
{
    BAPE_SpdifInputHandle   hSpdifInput = NULL;
    uint32_t                ui32IntStatus=0;
    uint32_t                ui32MaskStatus=0;

    BDBG_ENTER (BAPE_P_SPDIF_RX_isr);

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_ASSERT (pParm1);
    BSTD_UNUSED(iParm2);

    hSpdifInput = (BAPE_SpdifInputHandle) pParm1;

    #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN
        ui32IntStatus = BAPE_Reg_P_Read_isr(hSpdifInput->deviceHandle,  BCHP_AUD_FMM_IOP_IN_SPDIF_0_ESR_STATUS);
        ui32MaskStatus = BAPE_Reg_P_Read_isr(hSpdifInput->deviceHandle, BCHP_AUD_FMM_IOP_IN_SPDIF_0_ESR_MASK);
    #endif
    #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY
        ui32IntStatus = BAPE_Reg_P_Read_isr(hSpdifInput->deviceHandle,  BCHP_SPDIF_RCVR_ESR_STATUS);
        ui32MaskStatus = BAPE_Reg_P_Read_isr(hSpdifInput->deviceHandle, BCHP_SPDIF_RCVR_ESR_MASK);
    #endif

    ui32IntStatus &= ~ui32MaskStatus;

    BDBG_MSG(("SPDIF_RX_ISR: ESR_STATUS (unmasked): 0x%x", ui32IntStatus));

    if (ui32IntStatus)
    {
        #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN
            BAPE_Reg_P_Write_isr(hSpdifInput->deviceHandle, BCHP_AUD_FMM_IOP_MISC_ESR_STATUS_CLEAR, BCHP_MASK( AUD_FMM_IOP_MISC_ESR_STATUS_CLEAR, IOP_INTERRUPT_IN_SPDIF_0));
            BAPE_Reg_P_Write_isr(hSpdifInput->deviceHandle, BCHP_AUD_FMM_IOP_IN_SPDIF_0_ESR_STATUS_CLEAR, ui32IntStatus);
        #endif
        #if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY
            BAPE_Reg_P_Write_isr(hSpdifInput->deviceHandle, BCHP_SPDIF_RCVR_ESR_STATUS_CLEAR, ui32IntStatus);
        #endif

        BAPE_SpdifInput_P_DetectInputChange_isr(hSpdifInput);
    }
    BDBG_LEAVE (BAPE_P_SPDIF_RX_isr);
    return;    
}

/**************************************************************************/

static BERR_Code BAPE_SpdifInput_P_SetFormatDetection_isr(
    BAPE_SpdifInputHandle hSpdifInput
    )
{
    BERR_Code ret = BERR_SUCCESS;
    bool formatDetectionRequired = false;

    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);


    /* There are two reasons for enabling format detection.  First, when the input is enabled, we need
     * to use format detection for setting up the receiver's output format, and to halt the outfifo
     * in the case of a format change that can't be handled on-the-fly.  Secondly, the client (Nexus)
     * can enable format detection to keep track of the current input format.
     * So start by checking to see if anybody needs format detection on. */

    /* Check to see if our client needs format detection. */
    if ( hSpdifInput->clientFormatDetectionSettings.enabled == true && 
         hSpdifInput->clientFormatDetectionSettings.formatChangeInterrupt.pCallback_isr) {

        formatDetectionRequired = true;
    }

    /* Now see if we need it internally.  */
    if ( hSpdifInput->localFormatDetectionEnabled )
    {
        formatDetectionRequired = true;
    }

    /* If it's not enabled and somebody needs it, then turn it on. */
    if (! hSpdifInput->formatDetectionEnabled && formatDetectionRequired )
    {
        /* Make a call to BAPE_SpdifInput_P_GetFormatDetectionStatus_isr() to get a
         * fresh snapshot of the current state of things from the hardware 
         * (so that we can detect a change when interrupts occur later.   */ 
        BAPE_SpdifInput_P_GetFormatDetectionStatus_isr(hSpdifInput, &hSpdifInput->lastFormatDetectionStatus);

        BINT_EnableCallback_isr(hSpdifInput->spdifRxCallback);
        hSpdifInput->formatDetectionEnabled = true;
    }

    /* If format detection is enabled, but nobody needs it, then turn if off.  */
    if ( hSpdifInput->formatDetectionEnabled && ! formatDetectionRequired )
    {
        if( hSpdifInput->spdifRxCallback )
        {
            hSpdifInput->formatDetectionEnabled = false;
            ret = BINT_DisableCallback_isr(hSpdifInput->spdifRxCallback);
            if (ret != BERR_SUCCESS)                             
            {                                                    
                BDBG_ERR(("Unable to Disable SPDIF RX callback")); 
            }                                                    
        }
    }

    return BERR_SUCCESS;
}

/**************************************************************************/

static BERR_Code BAPE_SpdifInput_P_GetFormatDetectionStatus_isr(BAPE_SpdifInputHandle hSpdifInput, BAPE_SpdifInputFormatDetectionStatus *pStatus)
{
    uint32_t        receiverStatusRegVal = 0;
    uint32_t        receiverStatusStreamType = 0;
    uint32_t        receiverStatusSampleRate = 0;
    uint32_t        receiverStatusBurstPreamble = 0;    

    BDBG_ENTER(BAPE_SpdifInput_P_GetFormatDetectionStatus_isr);

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);
    BDBG_ASSERT(pStatus);   

    /* Clear out the destination buffer. */
    BKNI_Memset(pStatus, 0, sizeof (*pStatus));

    /* Set some default values in case we don't find a reason to change them. */
    pStatus->codec            = BAVC_AudioCompressionStd_eMax;
    pStatus->numPcmChannels   = 0;

    /* Let them know if the format change interrupts are enabled.  */
    pStatus->detectionEnabled   = false;
    if (hSpdifInput->clientFormatDetectionSettings.enabled == true && hSpdifInput->clientFormatDetectionSettings.formatChangeInterrupt.pCallback_isr) {
        pStatus->detectionEnabled   = true;
    }

    receiverStatusRegVal     = BAPE_Reg_P_Read(hSpdifInput->deviceHandle, BAPE_P_SPDIF_RX_STATUS_REGADDR );

#if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN
    BDBG_MSG(("RCVR_STATUS: BURST_PC:0x%u BPC_VLD:%u CSTAT_VLD:%u GOOD_BP:%u STREAM_TYPE:%u VALIDITY R/L:%u/%u SIG_PRES:%u SAMP_RATE:%u",
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, BURST_PREAM_C),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, BPC_VALID),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, CHAN_STAT_VALID),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, GOOD_BIPHASE),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, STREAM_TYPE),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, VALIDITY_R),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, VALIDITY_L),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, SIGNAL_PRESENT),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE)  ));
#endif
#if defined BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY
    BDBG_MSG(("RCVR_STATUS: BURST_PC:0x%u BPC_VLD:%u GOOD_BP:%u STREAM_TYPE:%u VALIDITY R/L:%u/%u SIG_PRES:%u SAMP_RATE:%u",
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, BURST_PREAM_C),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, BPC_VALID),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, GOOD_BIPHASE),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, STREAM_TYPE),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, VALIDITY_R),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, VALIDITY_L),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, SIGNAL_PRESENT),
               BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE)  ));
#endif

    pStatus->goodBiphase     = BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, GOOD_BIPHASE);
    pStatus->pcValid         = BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, BPC_VALID);
    pStatus->signalPresent   = BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, SIGNAL_PRESENT);
    pStatus->leftValidity    = BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, VALIDITY_L);
    pStatus->rightValidity   = BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, VALIDITY_R);
    receiverStatusStreamType = BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, STREAM_TYPE);
    pStatus->compressed = (receiverStatusStreamType == BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, STREAM_TYPE, Compressed)) ? true : false;

    /* Get the sample rate and convert to an integer. */
    receiverStatusSampleRate = BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE);

    switch (receiverStatusSampleRate)
    {
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_16000Hz ):      pStatus->sampleRate =   16000;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_22050Hz ):      pStatus->sampleRate =   22050;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_24000Hz ):      pStatus->sampleRate =   24000;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_32000Hz ):      pStatus->sampleRate =   32000;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_44100Hz ):      pStatus->sampleRate =   44100;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_48000Hz ):      pStatus->sampleRate =   48000;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_64000Hz ):      pStatus->sampleRate =   64000;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_88200Hz ):      pStatus->sampleRate =   88200;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_96000Hz ):      pStatus->sampleRate =   96000;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_128000Hz):      pStatus->sampleRate =  128000;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_176400Hz):      pStatus->sampleRate =  176400;     break;
        case BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_STATUS_REGNAME, SAMPLE_RATE, SR_192000Hz):      pStatus->sampleRate =  192000;     break;
        default:                                                                              pStatus->sampleRate =       0;     break;
    }            

    if ( pStatus->sampleRate == 0 )
    {
        BDBG_MSG (("SPDIF_RCVR_CTRL_STATUS : Sample Rate is Unknown"));
    }
    else
    {
        BDBG_MSG (("SPDIF_RCVR_CTRL_STATUS : Sample Rate is %u", pStatus->sampleRate ));
    }

    if ( ! pStatus->compressed )
    {
        pStatus->codec = BAVC_AudioCompressionStd_ePcm;
        BDBG_MSG (("SPDIF_RCVR_CTRL_STATUS : Uncompressed => BAVC_AudioCompressionStd_ePcm"));
        pStatus->numPcmChannels = 2;
    }
    else  /* must be compressed */
    {
            BDBG_MSG (("SPDIF_RCVR_CTRL_STATUS : Compressed... Need to check Preamble C bits"));
            if (!pStatus->pcValid)   /* If Preamble C is not valid */
            {
                pStatus->codec = BAVC_AudioCompressionStd_eMax;
                BDBG_MSG (("Preamble C: Not yet valid: BAVC_AudioCompressionStd_eMax"));
            }        
            else /* if preamble C is valid... */
            {
                receiverStatusBurstPreamble = BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_SPDIF_RX_STATUS_REGNAME, BURST_PREAM_C);

                BAPE_InputPort_P_BurstPreambleToCodec(receiverStatusBurstPreamble, &pStatus->codec );

            } /* End if preambleC is valid */
    } /* End if not linear PCM */

    BDBG_LEAVE(BAPE_SpdifInput_P_GetFormatDetectionStatus_isr);    

    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_SpdifInput_P_DetectInputChange_isr (BAPE_SpdifInputHandle    hSpdifInput)
{
    BAPE_SpdifInputFormatDetectionStatus  oldFormatDetectionStatus, *pOldFormatDetectionStatus, *pNewFormatDetectionStatus;
    BERR_Code errCode;

    BDBG_ENTER(BAPE_SpdifInput_P_DetectInputChange_isr);

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);

    pOldFormatDetectionStatus = &oldFormatDetectionStatus;
    pNewFormatDetectionStatus = &hSpdifInput->lastFormatDetectionStatus;

    /* Make a copy of the current "old" format detection snapshot. */
    BKNI_Memcpy_isr(pOldFormatDetectionStatus, pNewFormatDetectionStatus, sizeof hSpdifInput->lastFormatDetectionStatus);

    /* Now build a "new" format detection snapshot by reading the hardware. */
    BAPE_SpdifInput_P_GetFormatDetectionStatus_isr(hSpdifInput, pNewFormatDetectionStatus);


#if 0  /* ==================== Start: Legacy code left here for reference ==================== */
    {
        uint32_t    chanStatL0 = 0;

        chanStatL0 = BAPE_Reg_P_Read(hSpdifInput->deviceHandle, BCHP_SPDIF_RCVR_CTRL_CHAN_STAT_L0);

        bGoodBiphase = (bool) BCHP_GET_FIELD_DATA ( ui32SpdifRxCtrlStatus, SPDIF_RCVR_CTRL0_STATUS, GOOD_BIPHASE);

        bValidLeft = (bool) BCHP_GET_FIELD_DATA ( ui32SpdifRxCtrlStatus, SPDIF_RCVR_CTRL0_STATUS, VALIDITY_L);

        ui32SampleRate = (uint32_t) BCHP_GET_FIELD_DATA ( ui32SpdifRxCtrlStatus, SPDIF_RCVR_CTRL0_STATUS, SAMPLE_RATE);

        if ((bGoodBiphase) || (ui32SampleRate > 11))
        {
            /* No Good input or Rate Not Indicated */
            eInputState = BAPE_P_SpdifRx_InputState_eStreamNone;
        }
        else if ((bValidLeft) || (ui32ChanStatus & CBITS_STREAMTYPE))
        {
            /* One Validity bit set or CBIT 1 set indicates compressed */
            eInputState = BAPE_P_SpdifRx_InputState_eStreamPendingCompress;
        }
        else
        {
            eInputState = BAPE_P_SpdifRx_InputState_eStreamPCM;    
        }

    }
#endif   /* ==================== End: Legacy code left here for reference ==================== */

    BAPE_SpdifInput_P_SetReceiverOutputFormat_isr (hSpdifInput, pNewFormatDetectionStatus);

    if (BKNI_Memcmp_isr(pNewFormatDetectionStatus, pOldFormatDetectionStatus, sizeof (*pNewFormatDetectionStatus)) != 0)
    {
	    BAPE_FMT_Descriptor format;

        BAPE_InputPort_P_GetFormat_isr(&hSpdifInput->inputPort, &format);
        BAPE_SpdifInput_P_UpdateFormat_isr (hSpdifInput, pNewFormatDetectionStatus, &format);
        /* Let the consumer know about the format change. */
        errCode = BAPE_InputPort_P_SetFormat_isr(&hSpdifInput->inputPort, &format);
        if ( errCode )
        {
            /* The consumer can't handle the format change on-the-fly, so halt the capture.
             * Nexus will need to stop and restart the decoder. */
            BAPE_SpdifInput_P_Halt_isr(&hSpdifInput->inputPort);
        }

        /* Notify our client (probably Nexus) that there has been a format change.  */
        if (hSpdifInput->clientFormatDetectionSettings.enabled == true && hSpdifInput->clientFormatDetectionSettings.formatChangeInterrupt.pCallback_isr) 
        {
            BDBG_MSG(("Calling format change callback ", __FUNCTION__, __LINE__   ));
            hSpdifInput->clientFormatDetectionSettings.formatChangeInterrupt.pCallback_isr(
                                        hSpdifInput->clientFormatDetectionSettings.formatChangeInterrupt.pParam1,
                                        hSpdifInput->clientFormatDetectionSettings.formatChangeInterrupt.param2);
        }

        /* Done with the important stuff,  now print out each of the fields and indicate whether they've changed. */
        #define BAPE_PRINT_CHANGE(name, pfmt, old, new)                                  \
        	(  (old) != (new)   	 											         \
        		? 	   BDBG_MSG(("%s: " pfmt " -> " pfmt , name,	 (old),	 (new )))    \
        		: 	   BDBG_MSG(("%s: " pfmt, name, (new) ))			        	     \
        	)

        BDBG_MSG(("--------SPDIF Input Format Change Detection ---- begin ----"));

        BAPE_PRINT_CHANGE("  Codec from Preamble C", "%s",     BAPE_P_GetCodecName(pOldFormatDetectionStatus->codec),                   BAPE_P_GetCodecName(pNewFormatDetectionStatus->codec)  );  
        BAPE_PRINT_CHANGE("  Num Chans (PCM only) ", "%u",     pOldFormatDetectionStatus->numPcmChannels,   pNewFormatDetectionStatus->numPcmChannels );
        BAPE_PRINT_CHANGE("  Sample Rate          ", "%u",     pOldFormatDetectionStatus->sampleRate,       pNewFormatDetectionStatus->sampleRate );
        BAPE_PRINT_CHANGE("  Signal Present       ", "%s",     pOldFormatDetectionStatus->signalPresent    ? "TRUE" : "false", pNewFormatDetectionStatus->signalPresent    ? "TRUE" : "false" );
        BAPE_PRINT_CHANGE("  Compressed Flag      ", "%s",     pOldFormatDetectionStatus->compressed       ? "TRUE" : "false",       pNewFormatDetectionStatus->compressed ? "TRUE" : "false" );
        BAPE_PRINT_CHANGE("  Preamble C Valid     ", "%s",     pOldFormatDetectionStatus->pcValid          ? "TRUE" : "false", pNewFormatDetectionStatus->pcValid          ? "TRUE" : "false" );
        BAPE_PRINT_CHANGE("  Good BiPhase         ", "%s",     pOldFormatDetectionStatus->goodBiphase      ? "TRUE" : "false", pNewFormatDetectionStatus->goodBiphase      ? "TRUE" : "false" );
        BAPE_PRINT_CHANGE("  Left Validity        ", "%s",     pOldFormatDetectionStatus->leftValidity     ? "TRUE" : "false", pNewFormatDetectionStatus->leftValidity     ? "TRUE" : "false" );
        BAPE_PRINT_CHANGE("  Right Validity       ", "%s",     pOldFormatDetectionStatus->rightValidity    ? "TRUE" : "false", pNewFormatDetectionStatus->rightValidity    ? "TRUE" : "false" );
        BAPE_PRINT_CHANGE("  Detection Enabled    ", "%s",     pOldFormatDetectionStatus->detectionEnabled ? "TRUE" : "false", pNewFormatDetectionStatus->detectionEnabled ? "TRUE" : "false" );

        BDBG_MSG(("--------SPDIF Input Format Change Detection ---- end ----"));
    }

    BDBG_LEAVE (BAPE_SpdifInput_P_DetectInputChange_isr);   
    return;
}

/**************************************************************************/

static void BAPE_SpdifInput_P_SetReceiverOutputFormat_isr (BAPE_SpdifInputHandle hSpdifInput, BAPE_SpdifInputFormatDetectionStatus *pFormatDetectionStatus)
{
    uint32_t  outFormatEna;

    BDBG_ENTER(BAPE_SpdifInput_P_SetReceiverOutputFormat_isr);

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);

    if ( pFormatDetectionStatus->compressed)
    {
        if ( hSpdifInput->inputPort.pConsumer && hSpdifInput->inputPort.pConsumer->type == BAPE_PathNodeType_eInputCapture )
        {
            outFormatEna =  BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, COMP);
        }
        else
        {
            outFormatEna =  BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PES);
        }
    }
    else
    {
        outFormatEna =  BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PCM);
    }

    if ( outFormatEna != hSpdifInput->outFormatEna )
    {
        hSpdifInput->outFormatEna = outFormatEna; /* Update the saved state in the handle. */

        BDBG_MSG (("Switching SPDIF Receiver output format to %s", 
                   (outFormatEna ==  BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, COMP) ) ? "COMP" :
                   (outFormatEna ==  BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PES) )  ? "PES"  :
                   (outFormatEna ==  BAPE_P_BCHP_ENUM(BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PCM) )  ? "PCM"  :
                                                                                                            "Unknown" ));

        BAPE_Reg_P_UpdateField_isr(hSpdifInput->deviceHandle, 
                                  BAPE_P_SPDIF_RX_CONFIG_REGADDR, 
                                  BAPE_P_SPDIF_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, outFormatEna);
    }
    BDBG_LEAVE (BAPE_SpdifInput_P_SetReceiverOutputFormat_isr);   
    return;
}

/**************************************************************************/

static void BAPE_SpdifInput_P_UpdateFormat_isr (BAPE_SpdifInputHandle hSpdifInput, BAPE_SpdifInputFormatDetectionStatus *pFormatDetectionStatus, BAPE_FMT_Descriptor *pFormat)
{
    BAPE_InputPort  inputPort;
    BDBG_ENTER(BAPE_SpdifInput_P_UpdateFormat_isr);

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_OBJECT_ASSERT(hSpdifInput, BAPE_SpdifInput);

    inputPort = &hSpdifInput->inputPort;
    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);

    pFormat->sampleRate = pFormatDetectionStatus->sampleRate;

    if ( pFormatDetectionStatus->compressed )
    {
        pFormat->type = BAPE_DataType_eIec61937;
        BAPE_FMT_P_SetAudioCompressionStd(pFormat, pFormatDetectionStatus->codec);
    }
    else
    {
        if ( pFormatDetectionStatus->numPcmChannels == 0 )
        {
            pFormat->type = BAPE_DataType_eUnknown;     /* can't determine format type. */
        }
        else
        {
            pFormat->type = BAPE_DataType_ePcmStereo;
        }
    }

    BDBG_MSG(( "Updated format fields: "  BAPE_FMT_P_TO_PRINTF_ARGS(pFormat)));

    BDBG_LEAVE (BAPE_SpdifInput_P_UpdateFormat_isr);   
    return;
    }

/***************************************************************************
    Define stub functions for when there are no SPDIF inputs. 
***************************************************************************/
#else /* BAPE_CHIP_MAX_SPDIF_INPUTS > 0 */
    /* No SPDIF inputs, just use stubbed out functions. */

/**************************************************************************/
void BAPE_SpdifInput_GetDefaultSettings(
    BAPE_SpdifInputSettings *pSettings        /* [out] */
    )
{
    BSTD_UNUSED(pSettings);
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_SpdifInputSettings *pSettings,
    BAPE_SpdifInputHandle *pHandle              /* [out] */
    )
{
    BSTD_UNUSED(deviceHandle);
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(pHandle);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

void BAPE_SpdifInput_Close(
    BAPE_SpdifInputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/**************************************************************************/

void BAPE_SpdifInput_GetSettings(
    BAPE_SpdifInputHandle handle,
    BAPE_SpdifInputSettings *pSettings        /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_SetSettings(
    BAPE_SpdifInputHandle handle,
    const BAPE_SpdifInputSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

void BAPE_SpdifInput_GetInputPort(
    BAPE_SpdifInputHandle handle,
    BAPE_InputPort *pPort
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pPort);
}

/**************************************************************************/

void BAPE_SpdifInput_GetFormatDetectionSettings(
    BAPE_SpdifInputHandle handle,
    BAPE_SpdifInputFormatDetectionSettings *pSettings   /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_SetFormatDetectionSettings(
    BAPE_SpdifInputHandle handle,
    const BAPE_SpdifInputFormatDetectionSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_GetFormatDetectionStatus(
    BAPE_SpdifInputHandle handle,
    BAPE_SpdifInputFormatDetectionStatus *pStatus
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_P_PrepareForStandby(BAPE_Handle bapeHandle)
{
    BSTD_UNUSED(bapeHandle);
    return BERR_SUCCESS;
}

/**************************************************************************/

BERR_Code BAPE_SpdifInput_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BSTD_UNUSED(bapeHandle);
    return BERR_SUCCESS;
}

#endif
