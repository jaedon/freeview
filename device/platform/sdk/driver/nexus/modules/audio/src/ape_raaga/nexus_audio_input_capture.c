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
* 
* $brcm_Workfile: nexus_audio_input_capture.c $
* $brcm_Revision: 8 $
* $brcm_Date: 9/6/12 2:27p $
*
* API Description:
*   API name: Audio Module
*    Module includes
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_audio_input_capture.c $
* 
* 8   9/6/12 2:27p jgarrett
* SW7425-3667: Changing signalPresent logic
* 
* 7   6/28/12 6:13p jgarrett
* SW7231-877: Setting input volume in SetSettings
* 
* 6   5/14/12 3:42p gskerl
* SW7425-2075: Removed 'halted' flag from NEXUS_AudioInputCapture struct.
*  Added BDBG_MSG prints when inputCapture fails to start after a format
*  change (because of an unsupported format).
* 
* 5   5/10/12 6:33p gskerl
* SW7425-2075: Revised NEXUS_AudioInputCapture_P_InputFormatChange() to
*  use the APE InputPort's running and halted flags to decide when to
*  restart InputCapture.
* 
* 4   4/12/12 2:40p jgarrett
* SW7425-2075: Restarting whenever inputcapture halts
* 
* 3   2/13/12 11:53a jgarrett
* SW7425-2382: Coverity CID 36820
* 
* 2   2/9/12 5:39p jgarrett
* SW7425-2074: Fixing issues when NEXUS_NUM_AUDIO_INPUT_CAPTURES is not
*  defined
* 
* 1   2/9/12 2:24p jgarrett
* SW7425-2074: Merge to main branch
* 
* SW7425-2074/4   2/9/12 1:25p jgarrett
* SW7425-2074: Implementing TODOs, adding code review feedback
* 
* SW7425-2074/3   2/7/12 7:16p jgarrett
* SW7425-2074: Enabling format change handling
* 
* SW7425-2074/2   1/18/12 6:06p jgarrett
* SW7425-2074: Initial version of InputCapture API
* 
* SW7425-2074/1   1/18/12 10:21a jgarrett
* SW7425-2156: Adding power management support for DSP video decoder
* 
***************************************************************************/

#include "nexus_audio_module.h"

BDBG_MODULE(nexus_audio_input_capture);


#if NEXUS_NUM_AUDIO_INPUT_CAPTURES > 0
BDBG_OBJECT_ID(NEXUS_AudioInputCapture);
typedef struct NEXUS_AudioInputCapture
{
    BDBG_OBJECT(NEXUS_AudioInputCapture)
    NEXUS_AudioInputObject connector;
    BAPE_InputCaptureHandle apeHandle;
    NEXUS_AudioInputCaptureStartSettings startSettings;
    NEXUS_AudioInputCaptureSettings settings;
    BAPE_InputPort inputPort;
    unsigned index;
    bool opened;
    bool started;
    BKNI_EventHandle inputFormatChangeEvent;
    NEXUS_EventCallbackHandle inputFormatChangeEventHandler;
    NEXUS_TaskCallbackHandle sourceChangedCallback;
    bool compressed;
    bool hbr;
    unsigned numPcmChannels;
} NEXUS_AudioInputCapture;

static NEXUS_AudioInputCapture g_inputCaptures[NEXUS_NUM_AUDIO_INPUT_CAPTURES];

static void NEXUS_AudioInputCapture_P_InputFormatChange_isr(void *pParam1, int param2);
static void NEXUS_AudioInputCapture_P_InputCaptureHalted_isr(void *pParam1, int param2);
static void NEXUS_AudioInputCapture_P_InputFormatChange(void *pContext);
static void NEXUS_AudioInputCapture_P_SetConnectorFormat(NEXUS_AudioInputCaptureHandle handle, bool compressed, bool hbr, unsigned numPcmChannels);

/***************************************************************************
Summary:
Get default Open-time settings for AudioInputCapture
***************************************************************************/
void NEXUS_AudioInputCapture_GetDefaultOpenSettings(
    NEXUS_AudioInputCaptureOpenSettings *pSettings  /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

/***************************************************************************
Summary:
Open an AudioInputCapture handle
***************************************************************************/
NEXUS_AudioInputCaptureHandle NEXUS_AudioInputCapture_Open(
    unsigned index,
    const NEXUS_AudioInputCaptureOpenSettings *pSettings
    )
{    
    NEXUS_AudioInputCaptureHandle handle;
    BAPE_InputCaptureOpenSettings apeOpenSettings;
    BAPE_Connector connector;
    BERR_Code errCode;
    unsigned i;

    if ( index >= NEXUS_NUM_AUDIO_INPUT_CAPTURES )
    {
        BDBG_ERR(("Invalid input capture index %u.  This platform only supports %u input captures.", index, NEXUS_NUM_AUDIO_INPUT_CAPTURES));
        (void)BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    handle = &g_inputCaptures[index];
    if ( handle->opened )
    {
        BDBG_ERR(("Input capture %u is already open."));
        (void)BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    BSTD_UNUSED(pSettings);

    BDBG_OBJECT_SET(handle, NEXUS_AudioInputCapture);
    handle->index = index;
    handle->opened = true;
    NEXUS_AUDIO_INPUT_INIT(&handle->connector, NEXUS_AudioInputType_eInputCapture, handle);
    handle->connector.format = NEXUS_AudioInputFormat_ePcmStereo;
    for ( i = 0; i < NEXUS_AudioChannel_eMax; i++ )
    {
        handle->settings.volumeMatrix[i][i] = NEXUS_AUDIO_VOLUME_LINEAR_NORMAL;
    }
    BAPE_InputCapture_GetDefaultOpenSettings(&apeOpenSettings);
    errCode = BAPE_InputCapture_Open(NEXUS_AUDIO_DEVICE_HANDLE, 
                                     NEXUS_AUDIO_CAPTURE_INDEX(NEXUS_AudioInputType_eInputCapture, index), 
                                     &apeOpenSettings, 
                                     &handle->apeHandle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_ape_open;
    }
    BAPE_InputCapture_GetConnector(handle->apeHandle, &connector);
    handle->connector.port = (uint32_t)connector;

    errCode = BKNI_CreateEvent(&handle->inputFormatChangeEvent);
    if ( errCode )
    {
        errCode=BERR_TRACE(errCode);
        goto err_input_format_change_event;
    }

    handle->inputFormatChangeEventHandler = NEXUS_RegisterEvent(handle->inputFormatChangeEvent, NEXUS_AudioInputCapture_P_InputFormatChange, handle);
    if ( NULL == handle->inputFormatChangeEventHandler )
    {
        errCode=BERR_TRACE(BERR_OS_ERROR);
        goto err_input_format_change_event_handler;
    }

    handle->sourceChangedCallback = NEXUS_TaskCallback_Create(handle, NULL);
    if ( NULL == handle->sourceChangedCallback )
    {
       errCode = BERR_TRACE(BERR_OS_ERROR);
       goto err_source_change_callback;
    }
    
    return handle;

err_source_change_callback:
    NEXUS_UnregisterEvent(handle->inputFormatChangeEventHandler);
err_input_format_change_event_handler:
    BKNI_DestroyEvent(handle->inputFormatChangeEvent);
err_input_format_change_event:
    BAPE_InputCapture_Close(handle->apeHandle);
err_ape_open:
    BKNI_Memset(handle, 0, sizeof(NEXUS_AudioInputCapture));
    return NULL;
}

/***************************************************************************
Summary:
Close an AudioInputCapture handle
***************************************************************************/
void NEXUS_AudioInputCapture_Close(
    NEXUS_AudioInputCaptureHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioInputCapture);
    if ( handle->started )
    {
        NEXUS_AudioInputCapture_Stop(handle);
    }
    NEXUS_AudioInput_Shutdown(&handle->connector);

    NEXUS_TaskCallback_Destroy(handle->sourceChangedCallback);
    NEXUS_UnregisterEvent(handle->inputFormatChangeEventHandler);
    BKNI_DestroyEvent(handle->inputFormatChangeEvent);

    BAPE_InputCapture_Close(handle->apeHandle);
    BKNI_Memset(handle, 0, sizeof(NEXUS_AudioInputCapture));    /* Destroys magic numbers and marks as free */
}

/***************************************************************************
Summary:
Get Default Start-time settings for AudioInputCapture
***************************************************************************/
void NEXUS_AudioInputCapture_GetDefaultStartSettings(
    NEXUS_AudioInputCaptureStartSettings *pSettings /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

/***************************************************************************
Summary:
Start capturing input data
***************************************************************************/
NEXUS_Error NEXUS_AudioInputCapture_Start(
    NEXUS_AudioInputCaptureHandle handle,
    const NEXUS_AudioInputCaptureStartSettings *pSettings
    )
{
    NEXUS_Error errCode;
    BAPE_InputCaptureStartSettings apeStartSettings;
    BAPE_InputCaptureInterruptHandlers inputCaptureInterrupts;
    BAPE_InputPort inputPort;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioInputCapture);

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->input);

    inputPort = NEXUS_AudioInput_P_GetInputPort(pSettings->input);
    if ( inputPort == 0 )
    {
        BDBG_ERR(("Invalid input specified to NEXUS_AudioInputCapture"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( handle->started )
    {
        BDBG_ERR(("Input Capture %u already started", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    handle->startSettings = *pSettings;
    handle->inputPort = inputPort;

    if ( NEXUS_AudioInput_P_SupportsFormatChanges(pSettings->input) )
    {
        NEXUS_AudioInputPortStatus inputPortStatus;

        errCode = NEXUS_AudioInput_P_SetFormatChangeInterrupt(pSettings->input, NEXUS_AudioInputCapture_P_InputFormatChange_isr, handle, 0);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_format_interrupt;
        }

        errCode = NEXUS_AudioInput_P_GetInputPortStatus(pSettings->input, &inputPortStatus);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_status;
        }

        handle->compressed = inputPortStatus.compressed;
        handle->numPcmChannels = inputPortStatus.numPcmChannels;
        handle->hbr = inputPortStatus.hbr;

        /* Set output data type correctly */
        NEXUS_AudioInputCapture_P_SetConnectorFormat(handle, inputPortStatus.compressed, inputPortStatus.hbr, inputPortStatus.numPcmChannels);
    }

    errCode = NEXUS_AudioInput_P_PrepareToStart(&handle->connector);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_prepare_to_start;
    }

    BAPE_InputCapture_GetInterruptHandlers(handle->apeHandle, &inputCaptureInterrupts);
    inputCaptureInterrupts.inputHalted.pCallback_isr = NEXUS_AudioInputCapture_P_InputCaptureHalted_isr;
    inputCaptureInterrupts.inputHalted.pParam1 = handle;
    errCode = BAPE_InputCapture_SetInterruptHandlers(handle->apeHandle, &inputCaptureInterrupts);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_set_interrupts;
    }

    BAPE_InputCapture_GetDefaultStartSettings(&apeStartSettings);
    apeStartSettings.input = inputPort;
    errCode = BAPE_InputCapture_Start(handle->apeHandle, &apeStartSettings);
    if ( errCode == BERR_NOT_SUPPORTED )
    {
        BDBG_MSG(("InputCapture start is delayed because current input format is unsupported"));
        BDBG_MSG(("InputCapture will restart at next format change"));
    }
    else if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_start;
    }
    handle->started = true;

    return BERR_SUCCESS;

err_start:
    BAPE_InputCapture_GetInterruptHandlers(handle->apeHandle, &inputCaptureInterrupts);
    inputCaptureInterrupts.inputHalted.pCallback_isr = NULL;
    (void)BAPE_InputCapture_SetInterruptHandlers(handle->apeHandle, &inputCaptureInterrupts);
err_set_interrupts:
err_prepare_to_start:
err_status:
    if ( NEXUS_AudioInput_P_SupportsFormatChanges(pSettings->input) )
    {
        NEXUS_AudioInput_P_SetFormatChangeInterrupt(pSettings->input, NULL, NULL, 0);
    }
err_format_interrupt:
    return BERR_TRACE(errCode);
}


/***************************************************************************
Summary:
Stop capturing input data
***************************************************************************/
void NEXUS_AudioInputCapture_Stop(
    NEXUS_AudioInputCaptureHandle handle
    )
{
    BAPE_InputCaptureInterruptHandlers inputCaptureInterrupts;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioInputCapture);
    if ( !handle->started )
    {
        return;
    }

    if ( NEXUS_AudioInput_P_SupportsFormatChanges(handle->startSettings.input) )
    {
        NEXUS_AudioInput_P_SetFormatChangeInterrupt(handle->startSettings.input, NULL, NULL, 0);
    }
    BAPE_InputCapture_Stop(handle->apeHandle);
    BAPE_InputCapture_GetInterruptHandlers(handle->apeHandle, &inputCaptureInterrupts);
    inputCaptureInterrupts.inputHalted.pCallback_isr = NULL;
    (void)BAPE_InputCapture_SetInterruptHandlers(handle->apeHandle, &inputCaptureInterrupts);

    handle->started = false;
}

/***************************************************************************
Summary:
Get current settings for AudioInputCapture
***************************************************************************/
void NEXUS_AudioInputCapture_GetSettings(
    NEXUS_AudioInputCaptureHandle handle,
    NEXUS_AudioInputCaptureSettings *pSettings  /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioInputCapture);
    *pSettings = handle->settings;
}

/***************************************************************************
Summary:
Set settings for AudioInputCapture
***************************************************************************/
NEXUS_Error NEXUS_AudioInputCapture_SetSettings(
    NEXUS_AudioInputCaptureHandle handle,
    const NEXUS_AudioInputCaptureSettings *pSettings
    )
{
    BERR_Code errCode;
    unsigned i, j;
    BAPE_MixerInputVolume volume;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioInputCapture);
    BDBG_ASSERT(NULL != pSettings);

    NEXUS_AudioInput_P_GetVolume(&handle->connector, &volume);
    for ( i = 0; i < NEXUS_AudioChannel_eMax; i++ )
    {
        for ( j = 0; j < NEXUS_AudioChannel_eMax; j++ )
        {
            volume.coefficients[i][j] = pSettings->volumeMatrix[j][i];
        }
    }       
    volume.muted = pSettings->muted;
    BDBG_WRN(("Setting volume... first is %#x", volume.coefficients[0][0]));
    errCode = NEXUS_AudioInput_P_SetVolume(&handle->connector, &volume);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    NEXUS_TaskCallback_Set(handle->sourceChangedCallback, &pSettings->sourceChanged);

    handle->settings = *pSettings;   
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Get current status for AudioInputCapture
***************************************************************************/
NEXUS_Error NEXUS_AudioInputCapture_GetStatus(
    NEXUS_AudioInputCaptureHandle handle,
    NEXUS_AudioInputCaptureStatus *pStatus  /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioInputCapture);
    BDBG_ASSERT(NULL != pStatus);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->started = handle->started;
    pStatus->codec = NEXUS_AudioCodec_eUnknown;
    pStatus->numPcmChannels = 2;
    if ( handle->started )
    {
        if ( NEXUS_AudioInput_P_SupportsFormatChanges(handle->startSettings.input) )
        {
            NEXUS_AudioInputPortStatus inputPortStatus;
            NEXUS_Error errCode;

            errCode = NEXUS_AudioInput_P_GetInputPortStatus(handle->startSettings.input, &inputPortStatus);
            if ( errCode )
            {                
                return BERR_TRACE(errCode);
            }
            
            pStatus->sampleRate = inputPortStatus.sampleRate;
            if ( inputPortStatus.signalPresent )
            {
                pStatus->inputSignalPresent = true;
                pStatus->inputSignalValid = true;
                if ( inputPortStatus.compressed )
                {
                    pStatus->codec = inputPortStatus.codec;
                }
                else
                {
                    pStatus->codec = NEXUS_AudioCodec_ePcm;
                    pStatus->numPcmChannels = inputPortStatus.numPcmChannels;
                }
            }
        }
        else
        {
            pStatus->inputSignalPresent = true;
            pStatus->inputSignalValid = true;
        }
    }

    return BERR_SUCCESS;
}

NEXUS_AudioInput NEXUS_AudioInputCapture_GetConnector(
    NEXUS_AudioInputCaptureHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioInputCapture);
    return &handle->connector;
}

static void NEXUS_AudioInputCapture_P_InputFormatChange_isr(void *pParam1, int param2)
{
    NEXUS_AudioInputCaptureHandle handle = (NEXUS_AudioInputCaptureHandle)pParam1;
    BSTD_UNUSED(param2);
    /* convert to task time */
    BDBG_MSG(("Input Format change interrupt"));
    BKNI_SetEvent_isr(handle->inputFormatChangeEvent);
}

static void NEXUS_AudioInputCapture_P_InputCaptureHalted_isr(void *pParam1, int param2)
{
    NEXUS_AudioInputCaptureHandle handle = (NEXUS_AudioInputCaptureHandle)pParam1;
    BSTD_UNUSED(param2);
    BSTD_UNUSED(handle);
    BDBG_MSG(("Ignoring Input Capture Halt interrupt"));
}

static void NEXUS_AudioInputCapture_P_InputFormatChange(void *pParam)
{
    NEXUS_AudioInputCaptureHandle handle = (NEXUS_AudioInputCaptureHandle)pParam;
    NEXUS_AudioInputPortStatus inputPortStatus;
    BAPE_InputCaptureStatus apeInputCaptureStatus;
    NEXUS_Error errCode;
    bool restart = false;

    BDBG_MSG(("Processing Input Format change"));

    if ( !handle->started ){
        BDBG_MSG(("Not Started.  Aborting."));
        return;
    }

    NEXUS_TaskCallback_Fire(handle->sourceChangedCallback);

    errCode = NEXUS_AudioInput_P_GetInputPortStatus(handle->startSettings.input, &inputPortStatus);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        return;
    }

    BAPE_InputCapture_GetStatus(handle->apeHandle, &apeInputCaptureStatus );

    /* We only need to restart if APE's decoder is halted, or didn't start (not running). */
    if ( apeInputCaptureStatus.halted )
    {
        BDBG_MSG(("InputCapture has been halted, restart required"));
        restart = true;
    }
    else if ( ! apeInputCaptureStatus.running )
    {
        BDBG_MSG(("InputCapture is not running, restart required"));
        restart = true;
    }

    if ( restart == true )
    {
        BERR_Code errCode;
        BAPE_InputCaptureStartSettings apeStartSettings;

        BAPE_InputCapture_Stop(handle->apeHandle);

        BAPE_InputCapture_GetDefaultStartSettings(&apeStartSettings);
        apeStartSettings.input = handle->inputPort;
        BDBG_MSG(("Restarting InputCapture"));

        /* Set output data type correctly */
        NEXUS_AudioInputCapture_P_SetConnectorFormat(handle, inputPortStatus.compressed, inputPortStatus.hbr, inputPortStatus.numPcmChannels);

        /* Update downstream mixers */
        handle->started = false;
        (void)NEXUS_AudioInput_P_PrepareToStart(&handle->connector);
        handle->started = true;

        errCode = BAPE_InputCapture_Start(handle->apeHandle, &apeStartSettings);
        if ( errCode == BERR_NOT_SUPPORTED )
        {
            BDBG_MSG(("InputCapture start is delayed because current input format is unsupported"));
            BDBG_MSG(("InputCapture will restart at next format change"));
        }
        else if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
        }

        handle->compressed = inputPortStatus.compressed;
        handle->numPcmChannels = inputPortStatus.numPcmChannels;
        handle->hbr = inputPortStatus.hbr;
    }
}

static void NEXUS_AudioInputCapture_P_SetConnectorFormat(NEXUS_AudioInputCaptureHandle handle, bool compressed, bool hbr, unsigned numPcmChannels)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioInputCapture);
    if ( compressed )
    {
        BDBG_MSG(("Receiving Compressed.  Updating connector format."));
        BSTD_UNUSED(hbr);   /* TODO: Do we need to distinguish this for HDM setup? */
        handle->connector.format = NEXUS_AudioInputFormat_eCompressed;
    }
    else
    {
        BDBG_MSG(("Receiving PCM with %u channels.  Updating connector format.", numPcmChannels));
        switch ( numPcmChannels )
        {
        case 6:
            handle->connector.format = NEXUS_AudioInputFormat_ePcm5_1;
            break;
        case 8:
            handle->connector.format = NEXUS_AudioInputFormat_ePcm7_1;
            break;
        default:
            handle->connector.format = NEXUS_AudioInputFormat_ePcmStereo;
            break;
        }
    }
}

bool NEXUS_AudioInputCapture_P_IsRunning(NEXUS_AudioInputCaptureHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioInputCapture);
    return handle->started;
}

#else
/***************************************************************************
Summary:
Get default Open-time settings for AudioInputCapture
***************************************************************************/
void NEXUS_AudioInputCapture_GetDefaultOpenSettings(
    NEXUS_AudioInputCaptureOpenSettings *pSettings  /* [out] */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
Open an AudioInputCapture handle
***************************************************************************/
NEXUS_AudioInputCaptureHandle NEXUS_AudioInputCapture_Open(
    unsigned index,
    const NEXUS_AudioInputCaptureOpenSettings *pSettings
    )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

/***************************************************************************
Summary:
Close an AudioInputCapture handle
***************************************************************************/
void NEXUS_AudioInputCapture_Close(
    NEXUS_AudioInputCaptureHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/***************************************************************************
Summary:
Get Default Start-time settings for AudioInputCapture
***************************************************************************/
void NEXUS_AudioInputCapture_GetDefaultStartSettings(
    NEXUS_AudioInputCaptureStartSettings *pSettings /* [out] */
    )
{
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
Start capturing input data
***************************************************************************/
NEXUS_Error NEXUS_AudioInputCapture_Start(
    NEXUS_AudioInputCaptureHandle handle,
    const NEXUS_AudioInputCaptureStartSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}


/***************************************************************************
Summary:
Stop capturing input data
***************************************************************************/
void NEXUS_AudioInputCapture_Stop(
    NEXUS_AudioInputCaptureHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/***************************************************************************
Summary:
Get current settings for AudioInputCapture
***************************************************************************/
void NEXUS_AudioInputCapture_GetSettings(
    NEXUS_AudioInputCaptureHandle handle,
    NEXUS_AudioInputCaptureSettings *pSettings  /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

/***************************************************************************
Summary:
Set settings for AudioInputCapture
***************************************************************************/
NEXUS_Error NEXUS_AudioInputCapture_SetSettings(
    NEXUS_AudioInputCaptureHandle handle,
    const NEXUS_AudioInputCaptureSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
Get current status for AudioInputCapture
***************************************************************************/
NEXUS_Error NEXUS_AudioInputCapture_GetStatus(
    NEXUS_AudioInputCaptureHandle handle,
    NEXUS_AudioInputCaptureStatus *pStatus  /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pStatus);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/***************************************************************************
Summary:
Get audio connector to connect InputCapture to other objects
***************************************************************************/
NEXUS_AudioInput NEXUS_AudioInputCapture_GetConnector(
    NEXUS_AudioInputCaptureHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}

#endif
