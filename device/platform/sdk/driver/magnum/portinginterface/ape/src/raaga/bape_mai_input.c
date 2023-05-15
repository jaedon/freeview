/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_mai_input.c $
 * $brcm_Revision: Hydra_Software_Devel/31 $
 * $brcm_Date: 5/10/12 6:06p $
 *
 * Module Description: MAI Audio Input Interface (used for HDMI)
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/7422/bape_mai_input.c $
 * 
 * Hydra_Software_Devel/31   5/10/12 6:06p gskerl
 * SW7425-2075: Moved updating of inputPort->halted into
 * bape_input_port_priv.c.
 * 
 * Hydra_Software_Devel/30   5/4/12 5:59p gskerl
 * SW7425-2075: Consolidated redundant halted flags from BAPE_MaiInput
 * struct and BAPE_Decoder struct into the BAPE_InputPortObject struct.
 * 
 * Hydra_Software_Devel/29   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/5   5/3/12 3:55p gskerl
 * SW7425-2075: Added support for input halting, so that we can handle
 * input format changes during consumer start.
 * 
 * Hydra_Software_Devel/SW7425-2075/4   4/12/12 1:55p jgarrett
 * SW7425-2075: Revising InputPort format handling, adding HBR
 * 
 * Hydra_Software_Devel/SW7425-2075/3   4/11/12 11:13a gskerl
 * SW7425-2075: Add comments around critical section entry/exit.
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/10/12 6:47p gskerl
 * SW7425-2075: Adding new FMT interfaces for 7429
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/10/12 6:43p gskerl
 * SW7425-2075: Adding new FMT interfaces for 7429
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:51p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/28   3/22/12 2:59p gskerl
 * SW7429-18: Moved preamble C parsing logic into
 * BAPE_InputPort_P_BurstPreambleToCodec().
 * 
 * Hydra_Software_Devel/27   3/16/12 6:15p gskerl
 * SW7429-18: Corrected and enhanced codec detection based on preamble C
 * burst info fields..
 * 
 * Hydra_Software_Devel/26   3/12/12 12:13p gskerl
 * SW7429-18: Lots of cleanup, reorganization, and refactoring.
 * 
 * Hydra_Software_Devel/25   2/23/12 4:38p gskerl
 * SW7429-18: Added some critical sections. Created
 * BAPE_HdmiRx_P_SetReceiverOutputFormat_isr() to separate out receiver
 * output format setting logic.
 * 
 * Hydra_Software_Devel/24   2/9/12 11:21a jgarrett
 * SW7425-2074: Enabling input halt and revising MAI shutown for
 * multichannel PCM
 * 
 * Hydra_Software_Devel/23   1/17/12 3:07p gskerl
 * SW7429-18: Removed references to obsolete fields in the
 * BAPE_MaiInputFormatDetectionStatus struct.
 * 
 * Hydra_Software_Devel/22   1/16/12 2:50p gskerl
 * SW7429-18: Merged changes from branch SW7429-18_20111212 to
 * main/Hydra_Software_Devel/.
 * 
 * Hydra_Software_Devel/SW7429-18_20111212/5   1/13/12 5:48p gskerl
 * SW7429-18: Added MAI/HDMI audio input support for 7429.
 * 
 * Hydra_Software_Devel/SW7429-18_20111212/4   1/10/12 10:52a gskerl
 * SW7429-18: Merged from main to branch SW7429-18_20111212.
 * 
 * Hydra_Software_Devel/21   1/9/12 6:40p gskerl
 * SW7429-18: Merged refactored code from branch SW7429-18_20111212 to
 * main.
 * 
 * Hydra_Software_Devel/SW7429-18_20111212/3   1/9/12 6:07p gskerl
 * SW7429-18: Added hbr flag to BAPE_MaiInputFormatDetectionStatus struct.
 * Fixed 7429 compile issues. Generalized calculation of numChannelPairs.
 * 
 * Hydra_Software_Devel/SW7429-18_20111212/2   1/6/12 5:38p gskerl
 * SW7429-18: Corrected misspelling: BAPE_NUM_ELEMS -> BAPE_P_NUM_ELEMS
 * 
 * Hydra_Software_Devel/SW7429-18_20111212/1   1/6/12 5:16p gskerl
 * SW7429-18: Major refactoring to eliminate dependence on
 * bape_input_port_priv.c
 * 
 * Hydra_Software_Devel/20   1/6/12 2:38p jgarrett
 * SW7435-24: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7435-24/3   12/15/11 4:33p mward
 * SW7435-24:  Merge from main.
 * 
 * Hydra_Software_Devel/SW7435-24/2   11/30/11 11:54a mward
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
 * Hydra_Software_Devel/SW7429-18/2   11/4/11 5:36p gskerl
 * SW7429-18: Added conditional includes for bchp_int_id_aio_inth.h to fix
 * compile errors on 7425
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/26/11 12:44p jgarrett
 * SW7429-18: Merging latest changes from main branch
 * 
 * Hydra_Software_Devel/17   10/26/11 5:59p gskerl
 * SW7231-129: Added stub version of BAPE_MaiInput_P_PrepareForStandby().
 * 
 * Hydra_Software_Devel/16   10/26/11 5:32p gskerl
 * SW7231-129: Added BAPE_MaiInput_P_PrepareForStandby() to disable
 * interrupts before entering standby.
 * 
 * Hydra_Software_Devel/15   10/25/11 5:33p gskerl
 * SW7231-129: Added code to invalidate the saved SPDIF receiver state
 * before reinitializing the SPDIF receiver hardware.
 * 
 * Hydra_Software_Devel/14   10/25/11 1:23p gskerl
 * SW7231-129: Added stub version of BAPE_MaiInput_P_ResumeFromStandby()
 * 
 * Hydra_Software_Devel/13   10/24/11 2:38p gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume.
 * 
 * Hydra_Software_Devel/12   8/24/11 1:44p jgarrett
 * SWDTV-8271: Reversing SPDIF/HDMI input ISRs on 352xx
 * 
 * Hydra_Software_Devel/11   8/16/11 4:11p jgarrett
 * SWDTV-8271: Correcting IOP Capture ID for DTV
 * 
 * Hydra_Software_Devel/10   5/12/11 12:07p gskerl
 * SW7422-354: Fixed to support APE compilation on 35230
 * 
 * Hydra_Software_Devel/9   5/2/11 5:28p gskerl
 * SW7422-354: Removed explicit setting of handle->connector.type because
 * it was already set by the BAPE_P_InitInputPort macro
 * 
 * Hydra_Software_Devel/8   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/7   3/27/11 12:32p piyushg
 * SW7422-364: Rearrange Callback enable/disable calls and other
 * bug fixes for HDMI input.
 * 
 * Hydra_Software_Devel/6   3/24/11 12:11p piyushg
 * SW7422-364: HDMI input support - fixing some initial bugs and
 * compilation errors.
 * 
 * Hydra_Software_Devel/5   3/24/11 10:28a jgarrett
 * SW7422-364: Fixing compilation errors on systems without HDMI or SPDIF
 * input
 * 
 * Hydra_Software_Devel/4   3/23/11 11:07a piyushg
 * SW7422-364: Initial checking for HDMI input support.
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

BDBG_MODULE(bape_mai_input);

BDBG_OBJECT_ID(BAPE_MaiInput);

#if BAPE_CHIP_MAX_MAI_INPUTS > 0

typedef struct BAPE_MaiInput
{
    BDBG_OBJECT(BAPE_MaiInput)
    BAPE_Handle deviceHandle;
    BAPE_MaiInputSettings settings;
    unsigned index;                 
    BAPE_InputPortObject inputPort;
    uint32_t offset;  
    BAPE_MaiInputFormatDetectionSettings clientFormatDetectionSettings;
    bool localFormatDetectionEnabled;
    bool formatDetectionEnabled;
    uint32_t outFormatEna;      /* last value written to OUT_FORMAT_ENA field. */
    bool enable;
    char name[12];   /* MAI IN %d */
    
    BINT_CallbackHandle maiRxCallback;

    BAPE_MaiInputFormatDetectionStatus  lastFormatDetectionStatus;
} BAPE_MaiInput;


#if BCHP_CHIP == 7425 || BCHP_CHIP == 7422 || BCHP_CHIP == 7435
#define BAPE_MAI_INPUT_CAPTURE_ID(chPair) (5+(chPair))
#elif BCHP_CHIP == 35230 || BCHP_CHIP == 35233 || BCHP_CHIP == 35125 || BCHP_CHIP == 35126
#define BAPE_MAI_INPUT_CAPTURE_ID(chPair) (6+(chPair))
#elif defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN
/* 7429-style chips do not need this hardcoded */
#else
#error MAI Input Capture ID not defined
#endif

/* Build some abstract register names and map them to the chip's real names. */
#if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN
    #define BAPE_P_HDMI_RX_MAI_FORMAT_REGADDR    BCHP_AUD_FMM_IOP_IN_HDMI_0_MAI_FORMAT
    #define BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME         AUD_FMM_IOP_IN_HDMI_0_MAI_FORMAT
    #define BAPE_P_HDMI_RX_STATUS_REGADDR        BCHP_AUD_FMM_IOP_IN_HDMI_0_STATUS
    #define BAPE_P_HDMI_RX_STATUS_REGNAME             AUD_FMM_IOP_IN_HDMI_0_STATUS
    #define BAPE_P_HDMI_RX_CONFIG_REGADDR        BCHP_AUD_FMM_IOP_IN_HDMI_0_CONFIG
    #define BAPE_P_HDMI_RX_CONFIG_REGNAME             AUD_FMM_IOP_IN_HDMI_0_CONFIG
#endif
#if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY
    #define BAPE_P_HDMI_RX_MAI_FORMAT_REGADDR   BCHP_HDMI_RCVR_CTRL_MAI_FORMAT /* Use HDMI register */
    #define BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME       SPDIF_RCVR_CTRL_MAI_FORMAT /* but use SPDIF field and enum definitions */
    #define BAPE_P_HDMI_RX_STATUS_REGADDR       BCHP_HDMI_RCVR_CTRL_STATUS
    #define BAPE_P_HDMI_RX_STATUS_REGNAME           SPDIF_RCVR_CTRL_STATUS
    #define BAPE_P_HDMI_RX_CONFIG_REGADDR       BCHP_HDMI_RCVR_CTRL_CONFIG
    #define BAPE_P_HDMI_RX_CONFIG_REGNAME           SPDIF_RCVR_CTRL_CONFIG
#endif

/* Static function prototypes */
static BERR_Code    BAPE_MaiInput_P_OpenHw(BAPE_MaiInputHandle hMaiInput);
static BERR_Code    BAPE_MaiInput_P_ApplySettings(BAPE_MaiInputHandle hMaiInput);
static void         BAPE_MaiInput_P_DetectInputChange_isr (BAPE_MaiInputHandle hMaiInput);
static BERR_Code    BAPE_MaiInput_P_GetFormatDetectionStatus_isr(BAPE_MaiInputHandle hMaiInput, BAPE_MaiInputFormatDetectionStatus *pStatus );
static void         BAPE_P_MAI_RX_isr (void * pParm1, int iParm2); 
static void         BAPE_MaiInput_P_SetReceiverOutputFormat_isr (BAPE_MaiInputHandle hMaiInput, BAPE_MaiInputFormatDetectionStatus *pFormatDetectionStatus);
static BERR_Code    BAPE_MaiInput_P_SetFormatDetection_isr(BAPE_MaiInputHandle hMaiInput);
static void         BAPE_MaiInput_P_UpdateFormat_isr (BAPE_MaiInputHandle hMaiInput, BAPE_MaiInputFormatDetectionStatus *pFormatDetectionStatus, BAPE_FMT_Descriptor *pFormat);

/* Input port callbacks */
static void         BAPE_MaiInput_P_Enable(BAPE_InputPort inputPort);
static void         BAPE_MaiInput_P_Disable(BAPE_InputPort inputPort);
static BERR_Code    BAPE_MaiInput_P_ConsumerAttached_isr(BAPE_InputPort inputPort, BAPE_PathNode *pConsumer, BAPE_FMT_Descriptor *pFormat);
static void         BAPE_MaiInput_P_ConsumerDetached_isr(BAPE_InputPort inputPort, BAPE_PathNode *pConsumer);

/***************************************************************************
        Public APIs: From bape_input.h
***************************************************************************/
void BAPE_MaiInput_GetDefaultSettings(
    BAPE_MaiInputSettings *pSettings        /* [out] */
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

BERR_Code BAPE_MaiInput_Open(
    BAPE_Handle hApe,
    unsigned index,
    const BAPE_MaiInputSettings *pSettings,
    BAPE_MaiInputHandle *phMaiInput
    )
{
    BERR_Code errCode;
    BAPE_FMT_Descriptor format;
    BAPE_MaiInputHandle hMaiInput;
    BAPE_MaiInputSettings defaultSettings;

    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);
    BDBG_ASSERT(NULL != phMaiInput);
    
    BDBG_MSG(("%s: Opening MAI Input: %u", __FUNCTION__, index));

    *phMaiInput = NULL;

    if ( index >= BAPE_CHIP_MAX_MAI_INPUTS )
    {
        BDBG_ERR(("Request to open MAI %d but chip only has %u MAI inputs", index, BAPE_CHIP_MAX_MAI_INPUTS ));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( hApe->maiInputs[index] )
    {
        BDBG_ERR(("MAI input %d already open", index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Allocate the device structure, then fill in all the fields. */
    hMaiInput = BKNI_Malloc(sizeof(BAPE_MaiInput));
    if ( NULL == hMaiInput )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    BKNI_Memset(hMaiInput, 0, sizeof(BAPE_MaiInput));
    BDBG_OBJECT_SET(hMaiInput, BAPE_MaiInput);
    BAPE_P_InitInputPort(&hMaiInput->inputPort, BAPE_InputPortType_eMai, index, hMaiInput);
    hMaiInput->maiRxCallback = NULL;
    hMaiInput->deviceHandle = hApe;
    hMaiInput->index = index;
    #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN
    {
        uint32_t regVal = BAPE_Reg_P_Read(hApe, BCHP_AUD_FMM_IOP_IN_HDMI_0_CAPTURE_FCI_ID_TABLE);
        regVal = BCHP_GET_FIELD_DATA(regVal, AUD_FMM_IOP_IN_HDMI_0_CAPTURE_FCI_ID_TABLE, START_FCI_ID);
        hMaiInput->inputPort.streamId[BAPE_ChannelPair_eLeftRight] = regVal;
        hMaiInput->inputPort.streamId[BAPE_ChannelPair_eLeftRightSurround] = regVal+1;
        hMaiInput->inputPort.streamId[BAPE_ChannelPair_eCenterLfe] = regVal+2;
        hMaiInput->inputPort.streamId[BAPE_ChannelPair_eLeftRightRear] = regVal+3;
    }
    #endif
    #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY

        hMaiInput->inputPort.streamId[BAPE_ChannelPair_eLeftRight] = BAPE_MAI_INPUT_CAPTURE_ID(BAPE_ChannelPair_eLeftRight);
        hMaiInput->inputPort.streamId[BAPE_ChannelPair_eLeftRightSurround] = BAPE_MAI_INPUT_CAPTURE_ID(BAPE_ChannelPair_eLeftRightSurround);
        hMaiInput->inputPort.streamId[BAPE_ChannelPair_eCenterLfe] = BAPE_MAI_INPUT_CAPTURE_ID(BAPE_ChannelPair_eCenterLfe);
        hMaiInput->inputPort.streamId[BAPE_ChannelPair_eLeftRightRear] = BAPE_MAI_INPUT_CAPTURE_ID(BAPE_ChannelPair_eLeftRightRear);
    #endif
    hMaiInput->inputPort.enable = BAPE_MaiInput_P_Enable;
    hMaiInput->inputPort.disable = BAPE_MaiInput_P_Disable;
    hMaiInput->inputPort.consumerAttached_isr = BAPE_MaiInput_P_ConsumerAttached_isr;
    hMaiInput->inputPort.consumerDetached_isr = BAPE_MaiInput_P_ConsumerDetached_isr;

    {   /* Start Critical Section */
        BKNI_EnterCriticalSection();
        BAPE_InputPort_P_GetFormat_isr(&hMaiInput->inputPort, &format);
        format.source = BAPE_DataSource_eFci;
        format.type = BAPE_DataType_ePcmStereo;
        format.sampleRate = 48000;
        (void)BAPE_InputPort_P_SetFormat_isr(&hMaiInput->inputPort, &format);
        BKNI_LeaveCriticalSection();
    }   /* End Critical Section */

    BKNI_Snprintf(hMaiInput->name, sizeof(hMaiInput->name), "MAI IN %u", index);
    hMaiInput->inputPort.pName = hMaiInput->name;

    /* Currently we only support one MAI input, this can be expanded later if we support more */
    #if BAPE_CHIP_MAX_MAI_INPUTS  > 1
        #error "Need to support more MAI inputs"
    #endif
    hMaiInput->offset = 0;

    BDBG_ASSERT(hMaiInput->offset == 0);

    /* Init to specified settings */
    if ( NULL == pSettings )
    {
        BAPE_MaiInput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    errCode = BAPE_MaiInput_P_OpenHw(hMaiInput);
    if ( errCode )
    {
        BAPE_MaiInput_Close(hMaiInput);
        return BERR_TRACE(errCode);
    }

    errCode = BAPE_MaiInput_SetSettings(hMaiInput, pSettings);
    if ( errCode )
    {
        BAPE_MaiInput_Close(hMaiInput);
        return BERR_TRACE(errCode);
    }

    *phMaiInput = hMaiInput;
    hMaiInput->deviceHandle->maiInputs[index] = hMaiInput;
    return BERR_SUCCESS;
}

/**************************************************************************/

void BAPE_MaiInput_Close(
    BAPE_MaiInputHandle hMaiInput
    )
{
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);

    /* Make sure we're not still connected to anything */
    if ( hMaiInput->inputPort.pConsumer )
    {
        BDBG_ERR(("Cannot close MAI input %p (%d), still connected to %s", hMaiInput, hMaiInput->index, hMaiInput->inputPort.pConsumer->pName));
        BDBG_ASSERT(NULL == hMaiInput->inputPort.pConsumer);
        return;
    }

    if ( hMaiInput->maiRxCallback ) {
        BINT_DestroyCallback(hMaiInput->maiRxCallback);
    }

    hMaiInput->deviceHandle->maiInputs[hMaiInput->index] = NULL;
    BDBG_OBJECT_DESTROY(hMaiInput, BAPE_MaiInput);
    BKNI_Free(hMaiInput);    
}

/**************************************************************************/

void BAPE_MaiInput_GetSettings(
    BAPE_MaiInputHandle hMaiInput,
    BAPE_MaiInputSettings *pSettings        /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = hMaiInput->settings;
}

/**************************************************************************/

BERR_Code BAPE_MaiInput_SetSettings(
    BAPE_MaiInputHandle hMaiInput,
    const BAPE_MaiInputSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);
    BDBG_ASSERT(NULL != pSettings);
    hMaiInput->settings = *pSettings;

    return BAPE_MaiInput_P_ApplySettings(hMaiInput);
}

/**************************************************************************/

void BAPE_MaiInput_GetInputPort(
    BAPE_MaiInputHandle hMaiInput,
    BAPE_InputPort *pPort
    )
{
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);
    BDBG_ASSERT(NULL != pPort);
    *pPort = &hMaiInput->inputPort;
}

/**************************************************************************/

void BAPE_MaiInput_GetFormatDetectionSettings(
    BAPE_MaiInputHandle hMaiInput,
    BAPE_MaiInputFormatDetectionSettings *pSettings   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);
    BDBG_ASSERT(NULL != pSettings);

    {  /* Start Critical Section */
        BKNI_EnterCriticalSection();
        *pSettings = hMaiInput->clientFormatDetectionSettings;
        BKNI_LeaveCriticalSection();
    }  /* End Critical Section */
}

/**************************************************************************/

BERR_Code BAPE_MaiInput_SetFormatDetectionSettings(
    BAPE_MaiInputHandle hMaiInput,
    const BAPE_MaiInputFormatDetectionSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);
    BDBG_ASSERT(NULL != pSettings);

    {  /* Start Critical Section */
        BKNI_EnterCriticalSection();
        hMaiInput->clientFormatDetectionSettings = *pSettings;
        BAPE_MaiInput_P_SetFormatDetection_isr(hMaiInput);
        BKNI_LeaveCriticalSection();
    }  /* End Critical Section */


    return BERR_SUCCESS;
}

/**************************************************************************/

BERR_Code BAPE_MaiInput_GetFormatDetectionStatus(
    BAPE_MaiInputHandle hMaiInput,
    BAPE_MaiInputFormatDetectionStatus *pStatus
    )
{
    BERR_Code ret = BERR_SUCCESS;
    
    BDBG_ENTER(BAPE_MaiInput_GetFormatDetectionStatus);
    
    BDBG_ASSERT(hMaiInput);
    BDBG_ASSERT(pStatus);
    
    if ( hMaiInput->formatDetectionEnabled )
    {
        /* If Format Detection is enabled, we can just return a copy of whats
         * in the handle's lastFormatDectionStatus structure (since it's being
         * kept up to date). */
        BDBG_CASSERT(sizeof (*pStatus) == sizeof (hMaiInput->lastFormatDetectionStatus));
        BKNI_Memcpy(pStatus, &hMaiInput->lastFormatDetectionStatus, sizeof (*pStatus));
    }
    else
    {
        /* Format Detection is not enabled, so build a fresh format detection status
         * structure by reading the hardware.  */
        {  /* Start Critical Section */
            BKNI_EnterCriticalSection();
            BAPE_MaiInput_P_GetFormatDetectionStatus_isr(hMaiInput, pStatus);
            BKNI_LeaveCriticalSection();
        }  /* End Critical Section */
    }

    BDBG_LEAVE(BAPE_MaiInput_GetFormatDetectionStatus);
    return ret;    
}

/***************************************************************************
        BAPE Internal APIs: From bape_fmm_priv.h
***************************************************************************/

BERR_Code BAPE_MaiInput_P_PrepareForStandby(BAPE_Handle hApe)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    maiInputIndex;

    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);

    /* For each opened MaiInput... */
    for ( maiInputIndex=0 ; maiInputIndex<BAPE_CHIP_MAX_MAI_INPUTS ; maiInputIndex++ )
    {
        if ( hApe->maiInputs[maiInputIndex] )       /* If this MaiInput is open... */
        {
            BAPE_MaiInputHandle hMaiInput = hApe->maiInputs[maiInputIndex];

            /* If we already have a callback (interrupt), make sure that it's disabled, then destroy it. */
            if( hMaiInput->maiRxCallback )
            {
                errCode = BINT_DisableCallback(hMaiInput->maiRxCallback);
                if (errCode != BERR_SUCCESS)                             
                {                                                    
                    BDBG_ERR(("Unable to Disable MAI RX callback")); 
                }

                BINT_DestroyCallback(hMaiInput->maiRxCallback);
                hMaiInput->maiRxCallback = NULL;
            }
        }
    }
    return errCode;
}

/**************************************************************************/

BERR_Code BAPE_MaiInput_P_ResumeFromStandby(BAPE_Handle hApe)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    maiInputIndex;

    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);

    /* For each opened MaiInput, call the functions necessary to restore the hardware to it's appropriate state. */
    for ( maiInputIndex=0 ; maiInputIndex<BAPE_CHIP_MAX_MAI_INPUTS ; maiInputIndex++ )
    {
        if ( hApe->maiInputs[maiInputIndex] )       /* If this MaiInput is open... */
        {
            BAPE_MaiInputHandle hMaiInput = hApe->maiInputs[maiInputIndex];

            /* Put the HW into the generic open state. */
            errCode = BAPE_MaiInput_P_OpenHw(hMaiInput);
            if ( errCode ) return BERR_TRACE(errCode);

            /* Now apply changes for the settings struct. */
            errCode = BAPE_MaiInput_SetSettings(hMaiInput, &hMaiInput->settings);
            if ( errCode ) return BERR_TRACE(errCode);

            /* Now restore the dynamic stuff from the values saved in the device struct. */
            errCode = BAPE_MaiInput_SetFormatDetectionSettings(hMaiInput, &hMaiInput->clientFormatDetectionSettings);
            if ( errCode ) return BERR_TRACE(errCode);
        }
    }
    return errCode;
}

/***************************************************************************
        Private callbacks: Protyped above
***************************************************************************/
static void BAPE_MaiInput_P_Enable(BAPE_InputPort inputPort)
{
    BAPE_MaiInputHandle hMaiInput;

    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    hMaiInput = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);
    BDBG_ASSERT(false == hMaiInput->enable);

    BDBG_MSG(("Enabling %s", hMaiInput->name));

    /* Group the input channel pairs, then enable capture */

    #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN  /* Local (HDMI-specific) STREAM_CFG register */
    {
        unsigned i;
        unsigned numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&inputPort->format);

        uint32_t regAddr;

        /* Set Capture Group ID's for the correct number of channel pairs */
        for ( i = 0; i < BAPE_FMT_P_GetNumChannelPairs(&inputPort->format); i++ )
        {
            regAddr = BAPE_Reg_P_GetArrayAddress(                    AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i, i);
            BAPE_Reg_P_UpdateField(hMaiInput->deviceHandle, regAddr, AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i, CAP_GROUP_ID, 0);
        }
        for ( ; i <= BCHP_AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i_ARRAY_END; i++ )
        {
            regAddr = BAPE_Reg_P_GetArrayAddress(                    AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i, i);
            BAPE_Reg_P_UpdateField(hMaiInput->deviceHandle, regAddr, AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i, CAP_GROUP_ID, i);
        }

        /* Enable capture */
        for ( i = 0; i < numChannelPairs; i++ )
        {
            regAddr = BAPE_Reg_P_GetArrayAddress(                    AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i, i);
            BAPE_Reg_P_UpdateField(hMaiInput->deviceHandle, regAddr, AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i, CAP_ENA, 1);
        }
    }
    #elif defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY  /* Shared pool of STREAM_CFG registers in IOP */
        BAPE_Iop_P_EnableCapture( hMaiInput->deviceHandle, inputPort->streamId[0], BAPE_FMT_P_GetNumChannelPairs(&inputPort->format));
    #else
        #error "Unknown MAI register format"
    #endif /* if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN... BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY */

    /* Enable output FIFO */
    {  /* Start Critical Section */
        BKNI_EnterCriticalSection();

        if ( inputPort->halted )
        {
            BDBG_MSG(("Input %s is halted. can't enable, disabling instead", hMaiInput->name ));
            /* This handles the unlikely case of input being halted before the consumer start was complete.
             * Since the input is halted, do a disable instead of enabling... which is the same thing
             * that happens when a halt occurs after enabling.  */
            BAPE_Reg_P_UpdateEnum_isr(hMaiInput->deviceHandle, BAPE_P_HDMI_RX_CONFIG_REGADDR, 
                                                               BAPE_P_HDMI_RX_CONFIG_REGNAME, OUTFIFO_ENA, Disable);
            hMaiInput->enable = false;
        }
        else
        {
            /* Normal case.  Input is not halted, so enable the input. */
            BAPE_Reg_P_UpdateEnum_isr(hMaiInput->deviceHandle, BAPE_P_HDMI_RX_CONFIG_REGADDR, 
                                                               BAPE_P_HDMI_RX_CONFIG_REGNAME, OUTFIFO_ENA, Enable);
            hMaiInput->enable = true;
        }

        BKNI_LeaveCriticalSection();
    }  /* End Critical Section */
}

/**************************************************************************/

static void BAPE_MaiInput_P_Disable(BAPE_InputPort inputPort)
{
    BAPE_MaiInputHandle hMaiInput;

    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    hMaiInput = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);

    BDBG_MSG(("Disabling %s", hMaiInput->name));

    /* Disable the output FIFO. */
    {  /* Start Critical Section */
        BKNI_EnterCriticalSection();
        BAPE_Reg_P_UpdateEnum_isr(hMaiInput->deviceHandle, BAPE_P_HDMI_RX_CONFIG_REGADDR, 
                                                           BAPE_P_HDMI_RX_CONFIG_REGNAME, OUTFIFO_ENA, Disable);
        hMaiInput->enable = false;
        BKNI_LeaveCriticalSection();
    }  /* End Critical Section */

    /* Disable capture and ungroup the channel pairs */
    #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN  /* Local (HDMI-specific) STREAM_CFG register */
    {
        uint32_t regAddr;
        unsigned i;

        /* Disable capture */
        for ( i = 0; i < BAPE_ChannelPair_eMax; i++ )
        {
            regAddr = BAPE_Reg_P_GetArrayAddress(                    AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i, i);
            BAPE_Reg_P_UpdateField(hMaiInput->deviceHandle, regAddr, AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i, CAP_ENA, 0);
        }

        /* Ungroup */
        for ( i = 0; i < BAPE_ChannelPair_eMax; i++ )
        {
            regAddr = BAPE_Reg_P_GetArrayAddress(                    AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i, i);
            BAPE_Reg_P_UpdateField(hMaiInput->deviceHandle, regAddr, AUD_FMM_IOP_IN_HDMI_0_CAP_STREAM_CFG_i, CAP_GROUP_ID, i);
        }
    }
    #elif defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY  /* Shared pool of STREAM_CFG registers in IOP */
        BAPE_Iop_P_DisableCapture( hMaiInput->deviceHandle, inputPort->streamId[0], BAPE_ChannelPair_eMax );
    #else
        #error "Unknown MAI register format"
    #endif /* if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN, BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY */
}

/**************************************************************************/

static void BAPE_MaiInput_P_Halt_isr(BAPE_InputPort inputPort)
{
    BAPE_MaiInputHandle hMaiInput;

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    hMaiInput = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);

    BDBG_MSG(("Halting %s", hMaiInput->name));

    /* Remember the halted state. */
    BAPE_InputPort_P_Halt_isr(inputPort);

    /* Disable the output FIFO. */
    if ( hMaiInput->enable )
    {
        BAPE_Reg_P_UpdateEnum_isr(hMaiInput->deviceHandle, BAPE_P_HDMI_RX_CONFIG_REGADDR, 
                                                           BAPE_P_HDMI_RX_CONFIG_REGNAME, OUTFIFO_ENA, Disable);
    }
}

/**************************************************************************/

static BERR_Code BAPE_MaiInput_P_ConsumerAttached_isr(BAPE_InputPort inputPort, BAPE_PathNode *pConsumer, BAPE_FMT_Descriptor *pFormat)
{
    BAPE_MaiInputHandle hMaiInput;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    hMaiInput = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);
    
    BDBG_MSG(("Attaching consumer %s", pConsumer->pName));

    switch ( pConsumer->type )
    {
    case BAPE_PathNodeType_eDecoder:
    case BAPE_PathNodeType_eInputCapture:
        break;
    default:
        BDBG_ERR(("Node %u is not a valid consumer for MAI Input", pConsumer->pName));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_ASSERT(inputPort->pConsumer == pConsumer);

    /* Enable format detection interupts.  After this, the current input format
     * will be maintained in the handle's lastFormatDetectionStatus struct.  */
    hMaiInput->localFormatDetectionEnabled = true;
    BAPE_MaiInput_P_SetFormatDetection_isr(hMaiInput);

    /* Update the InputPort's fields to match the current input format. */
    BAPE_InputPort_P_GetFormat_isr(inputPort, pFormat);
    BAPE_MaiInput_P_UpdateFormat_isr (hMaiInput, &hMaiInput->lastFormatDetectionStatus, pFormat);
    BAPE_MaiInput_P_SetReceiverOutputFormat_isr (hMaiInput, &hMaiInput->lastFormatDetectionStatus);
    errCode = BAPE_InputPort_P_SetFormat_isr(inputPort, pFormat);

    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_MaiInput_P_ConsumerDetached_isr(BAPE_InputPort inputPort, BAPE_PathNode *pConsumer)
{
    BAPE_MaiInputHandle hMaiInput;

    BSTD_UNUSED(pConsumer);

    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    hMaiInput = inputPort->pHandle;
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);

    /* Disable local format detection. */
    hMaiInput->localFormatDetectionEnabled = false;
    BAPE_MaiInput_P_SetFormatDetection_isr(hMaiInput);

    return;
}

/***************************************************************************
        Private functions: Protyped above
***************************************************************************/

static BERR_Code BAPE_MaiInput_P_OpenHw(BAPE_MaiInputHandle hMaiInput)
{
    uint32_t regAddr;
    BAPE_Handle hApe;
    BAPE_Reg_P_FieldList regFieldList;
    BERR_Code errCode = BERR_SUCCESS;

    /* Currently we only support one MAI input, this can be expanded later if we support more */
    #if BAPE_CHIP_MAX_MAI_INPUTS > 1
        #error "Need to support more MAI inputs"
    #endif

    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);

    hApe = hMaiInput->deviceHandle;
    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);

    /* Taken from RAP PI (regarding ALLOW_NZ_STUFFING=Nonzero_Ok) -->                                                                       
    PR 35668: Some Blu Ray DVD Players send Non Zero values between compressed
    frames. This was getting treated as PCM data and causing confusion in 3563
    and a workaround was put. In 3548 this has been fixed in hardware.        
    Enabling the bit here.                                                    
    */                                                                        

    BAPE_Reg_P_InitFieldList(hApe, &regFieldList);

    regAddr = BAPE_P_HDMI_RX_CONFIG_REGADDR;

    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PCM);
    hMaiInput->outFormatEna = BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PCM);  /* Remember OUT_FORMAT_ENA setting. */

    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, ALLOW_NZ_STUFFING, Nonzero_OK);
    #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, SOURCE_SEL, MAI);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, HDMI_SEL, HDMI0);
    #endif /* if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY */

    BAPE_Reg_P_ApplyFieldList(&regFieldList, regAddr);

    /* Clear the interrupts before enabling any callbacks */
    #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN
        /* Clear and unmask L3 interrupts. */
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_IN_HDMI_0_ESR_MASK_SET,     0xffffffff);
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_IN_HDMI_0_ESR_STATUS_CLEAR, 0xffffffff);
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_IN_HDMI_0_ESR_MASK_CLEAR,  BCHP_MASK( AUD_FMM_IOP_IN_HDMI_0_ESR_MASK_CLEAR, HDMIRX_CHAN_STAT_MASK) |
                                                                           BCHP_MASK( AUD_FMM_IOP_IN_HDMI_0_ESR_MASK_CLEAR, HDMIRX_PC_CHANGE_MASK) |
                                                                           BCHP_MASK( AUD_FMM_IOP_IN_HDMI_0_ESR_MASK_CLEAR, HDMIRX_COMP_CHANGE_MASK) |
                                                                           BCHP_MASK( AUD_FMM_IOP_IN_HDMI_0_ESR_MASK_CLEAR, HDMIRX_MAI_FMT_CHANGE_MASK) );

        /* Clear and unmask L2 interrupt. */
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_MISC_ESR_MASK_SET,     BCHP_MASK( AUD_FMM_IOP_MISC_ESR_MASK_SET,     IOP_INTERRUPT_IN_HDMI_0));
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_MISC_ESR_STATUS_CLEAR, BCHP_MASK( AUD_FMM_IOP_MISC_ESR_STATUS_CLEAR, IOP_INTERRUPT_IN_HDMI_0));
        BAPE_Reg_P_Write(hApe, BCHP_AUD_FMM_IOP_MISC_ESR_MASK_CLEAR,   BCHP_MASK( AUD_FMM_IOP_MISC_ESR_MASK_CLEAR,   IOP_INTERRUPT_IN_HDMI_0));

    #endif /* if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN */
    #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY
        /* Clear and unmask L2 interrupts. */
        BAPE_Reg_P_Write(hApe, BCHP_HDMI_RCVR_ESR_MASK_SET,     0xffffffff);
        BAPE_Reg_P_Write(hApe, BCHP_HDMI_RCVR_ESR_STATUS_CLEAR, 0xffffffff);
        BAPE_Reg_P_Write(hApe, BCHP_HDMI_RCVR_ESR_MASK_CLEAR, BCHP_MASK( SPDIF_RCVR_ESR_MASK_CLEAR, SPDIFRX_CHAN_STAT_MASK )   |
                                                              BCHP_MASK( SPDIF_RCVR_ESR_MASK_CLEAR, SPDIFRX_PC_CHANGE_MASK )   |
                                                              BCHP_MASK( SPDIF_RCVR_ESR_MASK_CLEAR, SPDIFRX_COMP_CHANGE_MASK ) |
                                                              BCHP_MASK( SPDIF_RCVR_ESR_MASK_CLEAR, SPDIFRX_MAI_FMT_CHANGE_MASK ));
    #endif /* if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY */

    {
        #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN
            BINT_Id             intId = BCHP_INT_ID_AUD_IOP;  
        #endif /* if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN */
        #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY
            BINT_Id             intId = BCHP_INT_ID_HDMIRX;  
        #endif /* if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY */

        errCode = BINT_CreateCallback( &hMaiInput->maiRxCallback,
                                       hApe->intHandle, 
                                       intId,
                                       BAPE_P_MAI_RX_isr, 
                                       (void*)hMaiInput,          
                                       0 /* Not used*/       
                                      );
    }
    if (errCode != BERR_SUCCESS) 
    {
        BDBG_ERR(("Unable to create MAI RX callback"));
        return errCode;
    }

    return errCode;
}

/**************************************************************************/

static BERR_Code BAPE_MaiInput_P_ApplySettings(BAPE_MaiInputHandle hMaiInput)
{
    BAPE_Reg_P_FieldList regFieldList;
    const BAPE_MaiInputSettings *pSettings = &hMaiInput->settings;

    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);
    BDBG_ASSERT(NULL != pSettings);

    BAPE_Reg_P_InitFieldList(hMaiInput->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, IGNORE_VALID_PCM, pSettings->ignoreValidity);
    BAPE_Reg_P_AddToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, IGNORE_PERR_PCM,  pSettings->ignorePcmParity);
    BAPE_Reg_P_AddToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, IGNORE_PERR_COMP, pSettings->ignoreCompressedParity);
    BAPE_Reg_P_AddToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, TIMEBASE_SEL,     pSettings->timebase);

    switch ( pSettings->errorInsertion ) 
    {
    case BAPE_SpdifInputErrorInsertion_eNone:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, INSERT_MODE, No_insert);
        break;
    case BAPE_SpdifInputErrorInsertion_eZero:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, INSERT_MODE, Insert_zero);
        break;
    case BAPE_SpdifInputErrorInsertion_eRepeat:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGNAME, INSERT_MODE, Insert_repeat);
        break;
    default:
        BDBG_ERR(("Invalid Error Insertion Mode %d", pSettings->errorInsertion));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    
    {  /* Start Critical Section */
        BKNI_EnterCriticalSection();
        BAPE_Reg_P_ApplyFieldList(&regFieldList, BAPE_P_HDMI_RX_CONFIG_REGADDR);
        BKNI_LeaveCriticalSection();
    }  /* End Critical Section */

    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_P_MAI_RX_isr (
        void * pParm1, /* [in] channel hMaiInput */
        int    iParm2  /* [in] Not used */        
)
{
    BAPE_MaiInputHandle hMaiInput = NULL;
    uint32_t                        ui32IntStatus=0;
    uint32_t                        ui32MaskStatus=0;
    
    BDBG_ENTER (BAPE_P_MAI_RX_isr);

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_ASSERT (pParm1);
    BSTD_UNUSED(iParm2);
        
    hMaiInput = (BAPE_MaiInputHandle) pParm1;

    #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN
        ui32IntStatus = BAPE_Reg_P_Read_isr(hMaiInput->deviceHandle,  BCHP_AUD_FMM_IOP_IN_HDMI_0_ESR_STATUS);
        ui32MaskStatus = BAPE_Reg_P_Read_isr(hMaiInput->deviceHandle, BCHP_AUD_FMM_IOP_IN_HDMI_0_ESR_MASK);
    #endif
    #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY
        ui32IntStatus = BAPE_Reg_P_Read_isr(hMaiInput->deviceHandle,  BCHP_HDMI_RCVR_ESR_STATUS);
        ui32MaskStatus = BAPE_Reg_P_Read_isr(hMaiInput->deviceHandle, BCHP_HDMI_RCVR_ESR_MASK);
    #endif

    ui32IntStatus &= ~ui32MaskStatus;
            
    BDBG_MSG(("MAI_RX_ISR: ESR_STATUS (unmasked): 0x%x", ui32IntStatus));

    if (ui32IntStatus)
    {
        #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN
            BAPE_Reg_P_Write_isr(hMaiInput->deviceHandle, BCHP_AUD_FMM_IOP_MISC_ESR_STATUS_CLEAR, BCHP_MASK( AUD_FMM_IOP_MISC_ESR_STATUS_CLEAR, IOP_INTERRUPT_IN_HDMI_0));
            BAPE_Reg_P_Write_isr(hMaiInput->deviceHandle, BCHP_AUD_FMM_IOP_IN_HDMI_0_ESR_STATUS_CLEAR, ui32IntStatus);
        #endif
        #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY
            BAPE_Reg_P_Write_isr(hMaiInput->deviceHandle, BCHP_HDMI_RCVR_ESR_STATUS_CLEAR, ui32IntStatus);
        #endif

        BAPE_MaiInput_P_DetectInputChange_isr(hMaiInput);
    }
    BDBG_LEAVE (BAPE_P_MAI_RX_isr);
    return;    
}

/**************************************************************************/

static BERR_Code BAPE_MaiInput_P_SetFormatDetection_isr(
    BAPE_MaiInputHandle hMaiInput
    )
{
    BERR_Code ret = BERR_SUCCESS;
    bool formatDetectionRequired = false;

    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);


    /* There are two reasons for enabling format detection.  First, when the input is enabled, we need
     * to use format detection for setting up the receiver's output format, and to halt the outfifo
     * in the case of a format change that can't be handled on-the-fly.  Secondly, the client (Nexus)
     * can enable format detection to keep track of the current input format.
     * So start by checking to see if anybody needs format detection on. */

    /* Check to see if our client needs format detection. */
    if ( hMaiInput->clientFormatDetectionSettings.enabled == true && 
         hMaiInput->clientFormatDetectionSettings.formatChangeInterrupt.pCallback_isr) {

        formatDetectionRequired = true;
    }

    /* Now see if we need it internally.  */
    if ( hMaiInput->localFormatDetectionEnabled )
    {
        formatDetectionRequired = true;
    }

    /* If it's not enabled and somebody needs it, then turn it on. */
    if (! hMaiInput->formatDetectionEnabled && formatDetectionRequired )
    {
        /* Make a call to BAPE_MaiInput_P_GetFormatDetectionStatus_isr() to get a
         * fresh snapshot of the current state of things from the hardware 
         * (so that we can detect a change when interrupts occur later.   */ 
        BAPE_MaiInput_P_GetFormatDetectionStatus_isr(hMaiInput, &hMaiInput->lastFormatDetectionStatus);

        BINT_EnableCallback_isr(hMaiInput->maiRxCallback);
        hMaiInput->formatDetectionEnabled = true;
    }

    /* If format detection is enabled, but nobody needs it, then turn if off.  */
    if ( hMaiInput->formatDetectionEnabled && ! formatDetectionRequired )
    {
        if( hMaiInput->maiRxCallback )
        {
            hMaiInput->formatDetectionEnabled = false;
            ret = BINT_DisableCallback_isr(hMaiInput->maiRxCallback);
            if (ret != BERR_SUCCESS)                             
            {                                                    
                BDBG_ERR(("Unable to Disable MAI RX callback")); 
            }                                                    
        }
    }

    return BERR_SUCCESS;
}

/**************************************************************************/

static BERR_Code BAPE_MaiInput_P_GetFormatDetectionStatus_isr(BAPE_MaiInputHandle hMaiInput, BAPE_MaiInputFormatDetectionStatus *pStatus)
{
    uint32_t        receiverStatusRegVal = 0;
    uint32_t        maiFormatRegVal = 0;
    uint32_t        maiFormatRegAudioFormat = 0;
    uint32_t        maiFormatRegSampleRate = 0;
    uint32_t        burstPreamble = 0;    
    unsigned int    i;

    BDBG_ENTER(BAPE_MaiInput_P_GetFormatDetectionStatus_isr);

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);
    BDBG_ASSERT(pStatus);   

    /* Clear out the destination buffer. */
    BKNI_Memset(pStatus, 0, sizeof (*pStatus));

    /* Set some default values in case we don't find a reason to change them. */
    pStatus->codec            = BAVC_AudioCompressionStd_eMax;
    pStatus->signalPresent    = true;

    /* Let them know if the format change interrupts are enabled.  */
    pStatus->detectionEnabled   = false;
    if (hMaiInput->clientFormatDetectionSettings.enabled == true && hMaiInput->clientFormatDetectionSettings.formatChangeInterrupt.pCallback_isr) {
        pStatus->detectionEnabled   = true;
    }

    /* Read the HDMI Receiver's MAI_FORMAT and STATUS registers.  */
    maiFormatRegVal         = BAPE_Reg_P_Read(hMaiInput->deviceHandle, BAPE_P_HDMI_RX_MAI_FORMAT_REGADDR);
    receiverStatusRegVal    = BAPE_Reg_P_Read(hMaiInput->deviceHandle, BAPE_P_HDMI_RX_STATUS_REGADDR);

    /* Gather some of the fields from the MAI Format register. */
    pStatus->sampleWidth = BCHP_GET_FIELD_DATA(maiFormatRegVal , BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_WIDTH);

    /* Get the audio format from the MAI Format register, then convert to the BAPE_MaiInputFormat enum. */
    maiFormatRegAudioFormat = BCHP_GET_FIELD_DATA(maiFormatRegVal, BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT);
    {
        struct
        {
            uint32_t                hwAudioFormat;
            BAPE_MaiInputFormat     maiInputFormat;
            bool                    compressed;     /* true => stream is compressed */
            bool                    hbr;            /* true => HBR stream           */
            unsigned                numPcmChannels;   /* Total number of PCM audio channels, 0 for comnpressed streams */  
        } maiInputFormatInfo[] =
        {    /* hwAudioFormat                                                                                      maiInputFormat                              compressed   hbr    numPcmChannels */


            {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, Idle),                               BAPE_MaiInputFormat_eIdle,                      false,    false,   0 },
            #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN                                                         
                {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, Linear_PCM_audio_stereo_mode),   BAPE_MaiInputFormat_eSpdifLinearPcm,            false,    false,   2 },
            #endif
            #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY                                                        
                {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, HDMI_linearPCM_stereo),          BAPE_MaiInputFormat_eHdmiPcmStereo,             false,    false,   2 },
                {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, SPDIF_linearPCM),                BAPE_MaiInputFormat_eSpdifLinearPcm,            false,    false,   2 },
            #endif                                                                                                
            {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, SPDIF_linearPCM_stereo),             BAPE_MaiInputFormat_eSpdifPcmStereo,            false,    false,   2 },
            {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, SPDIF_linearPCM_6_channel),          BAPE_MaiInputFormat_eSpdifPcm6Channel,          false,    false,   6 },
            {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, SPDIF_linearPCM_8_channel),          BAPE_MaiInputFormat_eSpdifPcm8Channel,          false,    false,   8 },
                                                                                                                                 
            {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, HBR_compressed_8_channel),           BAPE_MaiInputFormat_eHbrCompressed,              true,     true,   0 },
                                                                                                                  
            #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY                                                        
                {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, HDMI_nonlinearPCM),              BAPE_MaiInputFormat_eHdmiNonLinearPcm,           true,    false,   0 },
                {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, SPDIF_nonlinearPCM),             BAPE_MaiInputFormat_eSpdifNonLinearPcm,          true,    false,   0 },
            #endif                                                                                                
            #if defined BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN                                                         
                {BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, AUDIO_FORMAT, Compressed_audio_2_channel),     BAPE_MaiInputFormat_eSpdifNonLinearPcm,          true,    false,   0 },
            #endif
        };

        /* Loop through the table, looking for a match to the MAI Format audio format bits. */
        for ( i=0 ; i<BAPE_P_NUM_ELEMS(maiInputFormatInfo) ; i++ )
        {
            if ( maiInputFormatInfo[i].hwAudioFormat == maiFormatRegAudioFormat )  break;
        }

        if ( i < BAPE_P_NUM_ELEMS(maiInputFormatInfo) )   /* Found a match, use table entry. */
        {
            pStatus->compressed         = maiInputFormatInfo[i].compressed;
            pStatus->hbr                = maiInputFormatInfo[i].hbr;
            pStatus->numPcmChannels     = maiInputFormatInfo[i].numPcmChannels;
        }
        else  /* MAI Format's audio format not listed in table.  */
        {
            BDBG_ERR (("MAI Format: 0x%08x not valid => BAVC_AudioCompressionStd_eMax!!!", maiFormatRegAudioFormat));
            pStatus->compressed      = false;
            pStatus->hbr             = false;
            pStatus->numPcmChannels     = 0;
        }
    }

    /* Get the sample rate from the MAI Format register and convert to an integer. */
    maiFormatRegSampleRate = BCHP_GET_FIELD_DATA(maiFormatRegVal, BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE);

    /* HBR is always 192k, ignore the value in MAI_FORMAT */
    if ( pStatus->hbr )
    {
        pStatus->sampleRate = 192000;
    }
    else
    {
        switch (maiFormatRegSampleRate)
        {
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_8000Hz):      pStatus->sampleRate =  8000;      break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_11025Hz):     pStatus->sampleRate =  11025;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_12000Hz):     pStatus->sampleRate =  12000;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_16000Hz):     pStatus->sampleRate =  16000;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_22050Hz):     pStatus->sampleRate =  22050;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_24000Hz):     pStatus->sampleRate =  24000;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_32000Hz):     pStatus->sampleRate =  32000;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_44100Hz):     pStatus->sampleRate =  44100;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_48000Hz):     pStatus->sampleRate =  48000;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_64000Hz):     pStatus->sampleRate =  64000;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_88200Hz):     pStatus->sampleRate =  88200;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_96000Hz):     pStatus->sampleRate =  96000;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_12800Hz):     pStatus->sampleRate = 128000;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_17640Hz):     pStatus->sampleRate = 176400;     break;
            case BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_MAI_FORMAT_REGNAME, SAMPLE_RATE, Rate_19200Hz):     pStatus->sampleRate = 192000;     break;
            default:                                                                                 pStatus->sampleRate =      0;     break;
        }            
    }

    if ( pStatus->sampleRate == 0 )
    {
        BDBG_MSG (("MAI FORMAT : Sample Rate is Unknown"));
    }
    else
    {
        BDBG_MSG (("MAI FORMAT : Sample Rate is %u", pStatus->sampleRate ));
    }

    if ( ! pStatus->compressed )
    {
        if ( pStatus->numPcmChannels > 0 )
        {
            pStatus->codec = BAVC_AudioCompressionStd_ePcm;
            BDBG_MSG (("MAI_Format: Uncompressed => BAVC_AudioCompressionStd_ePcm"));
        }
        else
        {
            pStatus->signalPresent    = false;
            pStatus->codec = BAVC_AudioCompressionStd_eMax;
        }
    }
    else  /* must be compressed */
    {
            BDBG_MSG (("MAI_Format: Compressed... Need to check Preamble C bits"));
            pStatus->pcValid        = BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_HDMI_RX_STATUS_REGNAME, BPC_VALID);

            if (!pStatus->pcValid)   /* If Preamble C is not valid */
            {
                pStatus->codec = BAVC_AudioCompressionStd_eMax;
                BDBG_MSG (("Preamble C: Not yet valid: BAVC_AudioCompressionStd_eMax"));
            }        
            else /* if preamble C is valid... */
            {
                burstPreamble = BCHP_GET_FIELD_DATA(receiverStatusRegVal, BAPE_P_HDMI_RX_STATUS_REGNAME, BURST_PREAM_C);

                BAPE_InputPort_P_BurstPreambleToCodec(burstPreamble, &pStatus->codec );

            } /* End if preambleC is valid */
    } /* End if not linear PCM */

    BDBG_LEAVE(BAPE_MaiInput_P_GetFormatDetectionStatus_isr);    

    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_MaiInput_P_DetectInputChange_isr (BAPE_MaiInputHandle    hMaiInput)
{
    BAPE_MaiInputFormatDetectionStatus  oldFormatDetectionStatus, *pOldFormatDetectionStatus, *pNewFormatDetectionStatus;
    BERR_Code errCode;

    BDBG_ENTER(BAPE_MaiInput_P_DetectInputChange_isr);

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);

    pOldFormatDetectionStatus = &oldFormatDetectionStatus;
    pNewFormatDetectionStatus = &hMaiInput->lastFormatDetectionStatus;

    /* Make a copy of the current "old" format detection snapshot. */
    BKNI_Memcpy_isr(pOldFormatDetectionStatus, pNewFormatDetectionStatus, sizeof hMaiInput->lastFormatDetectionStatus);

    /* Now build a "new" format detection snapshot by reading the hardware. */
    BAPE_MaiInput_P_GetFormatDetectionStatus_isr(hMaiInput, pNewFormatDetectionStatus);

    BAPE_MaiInput_P_SetReceiverOutputFormat_isr (hMaiInput, pNewFormatDetectionStatus);

    if (BKNI_Memcmp_isr(pNewFormatDetectionStatus, pOldFormatDetectionStatus, sizeof (*pNewFormatDetectionStatus)) != 0)
    {
        BAPE_FMT_Descriptor format;

        BAPE_InputPort_P_GetFormat_isr(&hMaiInput->inputPort, &format);
        BAPE_MaiInput_P_UpdateFormat_isr (hMaiInput, pNewFormatDetectionStatus, &format);
        /* Let the consumer know about the format change. */
        errCode = BAPE_InputPort_P_SetFormat_isr(&hMaiInput->inputPort, &format);
        if ( errCode )
        {
            /* The consumer can't handle the format change on-the-fly, so halt the capture.
             * Nexus will need to stop and restart the decoder. */
            BAPE_MaiInput_P_Halt_isr(&hMaiInput->inputPort);
        }

        /* Notify our client (probably Nexus) that there has been a format change.  */
        if (hMaiInput->clientFormatDetectionSettings.enabled == true && hMaiInput->clientFormatDetectionSettings.formatChangeInterrupt.pCallback_isr) 
        {
            BDBG_MSG(("Calling format change callback ", __FUNCTION__, __LINE__   ));
            hMaiInput->clientFormatDetectionSettings.formatChangeInterrupt.pCallback_isr(
                                      hMaiInput->clientFormatDetectionSettings.formatChangeInterrupt.pParam1,
                                      hMaiInput->clientFormatDetectionSettings.formatChangeInterrupt.param2);
        }

        /* Done with the important stuff,  now print out each of the fields and indicate whether they've changed. */
        #define BAPE_PRINT_CHANGE(name, pfmt, old, new)                                  \
        	(  (old) != (new)   	 											         \
        		? 	   BDBG_MSG(("%s: " pfmt " -> " pfmt , name,	 (old),	 (new )))    \
        		: 	   BDBG_MSG(("%s: " pfmt, name, (new) ))			        	     \
        	)

        BDBG_MSG(("--------MAI Input Format Change Detection ---- begin ----"));

        BAPE_PRINT_CHANGE("  Codec from Preamble C", "%s",     BAPE_P_GetCodecName(pOldFormatDetectionStatus->codec),                   BAPE_P_GetCodecName(pNewFormatDetectionStatus->codec)  );  
        BAPE_PRINT_CHANGE("  Num Chans (PCM only) ", "%u",     pOldFormatDetectionStatus->numPcmChannels,   pNewFormatDetectionStatus->numPcmChannels );
        BAPE_PRINT_CHANGE("  Sample Rate          ", "%u",     pOldFormatDetectionStatus->sampleRate,       pNewFormatDetectionStatus->sampleRate );
        BAPE_PRINT_CHANGE("  Sample Width         ", "%u",     pOldFormatDetectionStatus->sampleWidth,      pNewFormatDetectionStatus->sampleWidth);
        BAPE_PRINT_CHANGE("  Signal Present       ", "%s",     pOldFormatDetectionStatus->signalPresent    ? "TRUE" : "false", pNewFormatDetectionStatus->signalPresent    ? "TRUE" : "false" );
        BAPE_PRINT_CHANGE("  Compressed Flag      ", "%s",     pOldFormatDetectionStatus->compressed       ? "TRUE" : "false",       pNewFormatDetectionStatus->compressed ? "TRUE" : "false" );
        BAPE_PRINT_CHANGE("  Hi-bitrate (HBR) Flag", "%s",     pOldFormatDetectionStatus->hbr              ? "TRUE" : "false",       pNewFormatDetectionStatus->hbr        ? "TRUE" : "false" );
        BAPE_PRINT_CHANGE("  Preamble C Valid     ", "%s",     pOldFormatDetectionStatus->pcValid          ? "TRUE" : "false", pNewFormatDetectionStatus->pcValid          ? "TRUE" : "false" );
        BAPE_PRINT_CHANGE("  Detection Enabled    ", "%s",     pOldFormatDetectionStatus->detectionEnabled ? "TRUE" : "false", pNewFormatDetectionStatus->detectionEnabled ? "TRUE" : "false" );

        BDBG_MSG(("--------MAI Input Format Change Detection ---- end ----"));
    }

    BDBG_LEAVE (BAPE_MaiInput_P_DetectInputChange_isr);   
    return;
}


/**************************************************************************/

static void BAPE_MaiInput_P_SetReceiverOutputFormat_isr (BAPE_MaiInputHandle hMaiInput, BAPE_MaiInputFormatDetectionStatus *pFormatDetectionStatus)
{
    uint32_t  outFormatEna;

    BDBG_ENTER(BAPE_MaiInput_P_SetReceiverOutputFormat_isr);

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);

    if ( pFormatDetectionStatus->compressed)
    {
        if ( hMaiInput->inputPort.pConsumer && hMaiInput->inputPort.pConsumer->type == BAPE_PathNodeType_eInputCapture )
        {
            outFormatEna =  BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, COMP);
        }
        else
        {
            outFormatEna =  BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PES);
        }
    }
    else
    {
        outFormatEna =  BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PCM);
    }

    if ( outFormatEna != hMaiInput->outFormatEna )
    {
        hMaiInput->outFormatEna = outFormatEna; /* Update the saved state in the handle. */

        BDBG_MSG (("Switching HDMI Receiver output format to %s", 
                   (outFormatEna ==  BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, COMP) ) ? "COMP" :
                   (outFormatEna ==  BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PES) )  ? "PES"  :
                   (outFormatEna ==  BAPE_P_BCHP_ENUM(BAPE_P_HDMI_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, PCM) )  ? "PCM"  :
                                                                                                            "Unknown" ));

        BAPE_Reg_P_UpdateField_isr(hMaiInput->deviceHandle, 
                                  BAPE_P_HDMI_RX_CONFIG_REGADDR, 
                                  BAPE_P_HDMI_RX_CONFIG_REGNAME, OUT_FORMAT_ENA, outFormatEna);

    }
    #if BAPE_CHIP_MAI_INPUT_HBR_SUPPORT
    BDBG_MSG(("Setting HBR_MODE to %u", pFormatDetectionStatus->hbr ? 1 : 0));
    BAPE_Reg_P_UpdateField_isr(hMaiInput->deviceHandle, BAPE_P_HDMI_RX_CONFIG_REGADDR, 
                               BAPE_P_HDMI_RX_CONFIG_REGNAME, HBR_MODE, pFormatDetectionStatus->hbr?1:0);
    
    #endif

    BDBG_LEAVE (BAPE_MaiInput_P_SetReceiverOutputFormat_isr);   
    return;
}

/**************************************************************************/

static void BAPE_MaiInput_P_UpdateFormat_isr (BAPE_MaiInputHandle hMaiInput, BAPE_MaiInputFormatDetectionStatus *pFormatDetectionStatus, BAPE_FMT_Descriptor *pFormat)
{
    BAPE_InputPort  inputPort;
    BDBG_ENTER(BAPE_MaiInput_P_UpdateFormat_isr);

    BKNI_ASSERT_ISR_CONTEXT();
    BDBG_OBJECT_ASSERT(hMaiInput, BAPE_MaiInput);

    inputPort = &hMaiInput->inputPort;
    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);

    pFormat->sampleRate = pFormatDetectionStatus->sampleRate;
    if ( pFormatDetectionStatus->hbr )
    {
        pFormat->type = BAPE_DataType_eIec61937x16;
        BAPE_FMT_P_SetAudioCompressionStd(pFormat, pFormatDetectionStatus->codec);
    }
    else if ( pFormatDetectionStatus->compressed )
    {
        if ( pFormat->sampleRate == 176400 || pFormat->sampleRate == 192000 )
        {
            pFormat->type = BAPE_DataType_eIec61937x4;
            BAPE_FMT_P_SetAudioCompressionStd(pFormat, pFormatDetectionStatus->codec);
        }
        else
        {
            pFormat->type = BAPE_DataType_eIec61937;
            BAPE_FMT_P_SetAudioCompressionStd(pFormat, pFormatDetectionStatus->codec);
        }
    }
    else
    {
        if ( pFormatDetectionStatus->numPcmChannels == 8 )
        {
            pFormat->type = BAPE_DataType_ePcm7_1;
        }
        else if ( pFormatDetectionStatus->numPcmChannels == 6 )
        {
            pFormat->type = BAPE_DataType_ePcm5_1;
        }
        else if ( pFormatDetectionStatus->numPcmChannels == 0 )
        {
            pFormat->type = BAPE_DataType_eUnknown;     /* can't determine format type. */
        }
        else
        {
            pFormat->type = BAPE_DataType_ePcmStereo;
        }
    }

    BDBG_MSG(( "Updated format fields: "  BAPE_FMT_P_TO_PRINTF_ARGS(pFormat)));

    BDBG_LEAVE (BAPE_MaiInput_P_UpdateFormat_isr);   
    return;
}

/***************************************************************************
    Define stub functions for when there are no MAI inputs. 
***************************************************************************/
#else /* BAPE_CHIP_MAX_MAI_INPUTS > 0 */
    /* No MAI inputs, just use stubbed out functions. */

/**************************************************************************/
void BAPE_MaiInput_GetDefaultSettings(
    BAPE_MaiInputSettings *pSettings        /* [out] */
    )
{
    BSTD_UNUSED(pSettings);
}

/**************************************************************************/

BERR_Code BAPE_MaiInput_Open(
    BAPE_Handle hApe,
    unsigned index,
    const BAPE_MaiInputSettings *pSettings,
    BAPE_MaiInputHandle *phMaiInput              /* [out] */
    )
{
    BSTD_UNUSED(hApe);
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(phMaiInput);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

void BAPE_MaiInput_Close(
    BAPE_MaiInputHandle hMaiInput
    )
{
    BSTD_UNUSED(hMaiInput);
}

/**************************************************************************/

void BAPE_MaiInput_GetSettings(
    BAPE_MaiInputHandle hMaiInput,
    BAPE_MaiInputSettings *pSettings        /* [out] */
    )
{
    BSTD_UNUSED(hMaiInput);
    BSTD_UNUSED(pSettings);
}

/**************************************************************************/

BERR_Code BAPE_MaiInput_SetSettings(
    BAPE_MaiInputHandle hMaiInput,
    const BAPE_MaiInputSettings *pSettings
    )
{
    BSTD_UNUSED(hMaiInput);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

void BAPE_MaiInput_GetInputPort(
    BAPE_MaiInputHandle hMaiInput,
    BAPE_InputPort *pPort
    )
{
    BSTD_UNUSED(hMaiInput);
    BSTD_UNUSED(pPort);
}

/**************************************************************************/

void BAPE_MaiInput_GetFormatDetectionSettings(
    BAPE_MaiInputHandle hMaiInput,
    BAPE_MaiInputFormatDetectionSettings *pSettings   /* [out] */
    )
{
    BSTD_UNUSED(hMaiInput);
    BSTD_UNUSED(pSettings);
}

/**************************************************************************/

BERR_Code BAPE_MaiInput_SetFormatDetectionSettings(
    BAPE_MaiInputHandle hMaiInput,
    const BAPE_MaiInputFormatDetectionSettings *pSettings
    )
{
    BSTD_UNUSED(hMaiInput);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

BERR_Code BAPE_MaiInput_GetFormatDetectionStatus(
    BAPE_MaiInputHandle hMaiInput,
    BAPE_MaiInputFormatDetectionStatus *pStatus
    )
{
    BSTD_UNUSED(hMaiInput);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

BERR_Code BAPE_MaiInput_P_PrepareForStandby(BAPE_Handle hApe)
{
    BSTD_UNUSED(hApe);
    return BERR_SUCCESS;
}

/**************************************************************************/

BERR_Code BAPE_MaiInput_P_ResumeFromStandby(BAPE_Handle hApe)
{
    BSTD_UNUSED(hApe);
    return BERR_SUCCESS;
}

#endif
