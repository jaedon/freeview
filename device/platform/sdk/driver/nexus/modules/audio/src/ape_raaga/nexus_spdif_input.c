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
* $brcm_Workfile: nexus_spdif_input.c $
* $brcm_Revision: 4 $
* $brcm_Date: 6/19/12 7:08p $
*
* API Description:
*   API name: SpdifInput
*    Specific APIs related to SPDIF Input
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7422/src/nexus_spdif_input.c $
* 
* 4   6/19/12 7:08p vsilyaev
* SW7420-2085: Properly build stub API
* 
* 3   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 2   3/8/12 5:11p gskerl
* SW7429-18: Moved codec from preambleC substruct of
*  BAPE_SpdifInputFormatDetectionStatus.
* 
* 1   8/18/11 5:51p jgarrett
* SWDTV-6306: Merge DTV APE changes to main branch
* 
* Nexus_APE_Integration/2   8/11/11 6:25p jgarrett
* SWDTV-6306: Adding SPDIF Input
* 
* Nexus_APE_Integration/1   7/1/11 5:50p jgarrett
* SWDTV-6306: Integrated to latest 7422 baseline and nexus audio
*  directory structure
* 
* 1   5/5/11 11:44a jgarrett
* SW7425-354: Adding 35230 stubs
* 
***************************************************************************/
#include "nexus_audio_module.h"

BDBG_MODULE(nexus_spdif_input);

#if NEXUS_NUM_SPDIF_INPUTS

typedef struct NEXUS_SpdifInput
{
    NEXUS_OBJECT(NEXUS_SpdifInput);
    NEXUS_AudioInputObject connector;
    BAPE_SpdifInputHandle apeHandle;
    NEXUS_SpdifInputSettings settings;
    bool opened;
} NEXUS_SpdifInput;

static NEXUS_SpdifInput g_spdifInputs[NEXUS_NUM_SPDIF_INPUTS];

void NEXUS_SpdifInput_GetDefaultSettings(
    NEXUS_SpdifInputSettings *pSettings   /* [out] default settings */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_SpdifInputHandle NEXUS_SpdifInput_Open(  /* attr{destructor=NEXUS_SpdifInput_Close}  */
    unsigned index,
    const NEXUS_SpdifInputSettings *pSettings
    )
{
    NEXUS_SpdifInputSettings defaults;
    BAPE_SpdifInputSettings apeSettings;
    NEXUS_SpdifInputHandle handle;
    BAPE_InputPort inputPort;
    BERR_Code errCode;
    
    if ( index >= NEXUS_NUM_SPDIF_INPUTS )
    {
        BDBG_ERR(("SPDIF Input %u not supported", index));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }

    if ( g_spdifInputs[index].opened )
    {
        BDBG_ERR(("SPDIF Input %u already opened.", index));
        (void)BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }
    handle = &g_spdifInputs[index];    
    
    if ( NULL == pSettings )
    {
        NEXUS_SpdifInput_GetDefaultSettings(&defaults);
        pSettings = &defaults;
    }
            
    NEXUS_OBJECT_INIT(NEXUS_SpdifInput, handle);
    NEXUS_AUDIO_INPUT_INIT(&handle->connector, NEXUS_AudioInputType_eSpdif, handle);
    handle->opened = true;
    
    BAPE_SpdifInput_GetDefaultSettings(&apeSettings);
    errCode = BAPE_SpdifInput_Open(NEXUS_AUDIO_DEVICE_HANDLE, index, &apeSettings, &handle->apeHandle);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_ape_open;
    }
    
    BAPE_SpdifInput_GetInputPort(handle->apeHandle, &inputPort);
    handle->connector.inputPort = (uint32_t)inputPort;

    errCode = NEXUS_SpdifInput_SetSettings(handle, pSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_settings;
    }
        
    return handle;
    
err_settings:
    BAPE_SpdifInput_Close(handle->apeHandle);    
err_ape_open:
    BKNI_Memset(handle, 0, sizeof(NEXUS_SpdifInput));
    return NULL;
}

static void NEXUS_SpdifInput_P_Finalizer(
    NEXUS_SpdifInputHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SpdifInput, handle);
    BAPE_SpdifInput_Close(handle->apeHandle);
    BKNI_Memset(handle, 0, sizeof(NEXUS_SpdifInput));
}


void NEXUS_SpdifInput_GetSettings(
    NEXUS_SpdifInputHandle handle,
    NEXUS_SpdifInputSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifInput);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

NEXUS_Error NEXUS_SpdifInput_SetSettings(
    NEXUS_SpdifInputHandle handle,
    const NEXUS_SpdifInputSettings *pSettings
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifInput);
    BDBG_ASSERT(NULL != pSettings);
    handle->settings = *pSettings;
    /* Currently, these settings do not map to APE */
    return BERR_SUCCESS;
}

NEXUS_AudioInput NEXUS_SpdifInput_GetConnector( /* attr{shutdown=NEXUS_AudioInput_Shutdown} */
    NEXUS_SpdifInputHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifInput);
    return &handle->connector;
}

NEXUS_Error NEXUS_SpdifInput_P_SetFormatChangeInterrupt(
    NEXUS_SpdifInputHandle handle,
    void (*pCallback_isr)(void *, int),
    void *pParam1,
    int param2
    )
{
    BERR_Code errCode;
    BAPE_SpdifInputFormatDetectionSettings detectionSettings;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifInput);
    
    BAPE_SpdifInput_GetFormatDetectionSettings(handle->apeHandle, &detectionSettings);
    detectionSettings.enabled = (pCallback_isr != NULL) ? true : false;
    detectionSettings.formatChangeInterrupt.pCallback_isr = pCallback_isr;
    detectionSettings.formatChangeInterrupt.pParam1 = pParam1;
    detectionSettings.formatChangeInterrupt.param2 = param2;
    errCode = BAPE_SpdifInput_SetFormatDetectionSettings(handle->apeHandle, &detectionSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    return BERR_SUCCESS;
}        

NEXUS_Error NEXUS_SpdifInput_P_GetInputPortStatus(
    NEXUS_SpdifInputHandle handle,
    NEXUS_AudioInputPortStatus *pStatus     /* [out] */
    )
{
    BERR_Code errCode;
    BAPE_SpdifInputFormatDetectionStatus detectionStatus;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_SpdifInput);
    BDBG_ASSERT(NULL != pStatus);
    
    BKNI_Memset(pStatus, 0, sizeof(NEXUS_AudioInputPortStatus));
    errCode = BAPE_SpdifInput_GetFormatDetectionStatus(handle->apeHandle, &detectionStatus);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pStatus->signalPresent = detectionStatus.signalPresent;
    pStatus->compressed = detectionStatus.compressed;
    pStatus->codec = NEXUS_Audio_P_MagnumToCodec(detectionStatus.codec);
    pStatus->sampleRate = detectionStatus.sampleRate;
    pStatus->numPcmChannels = 2;
    return BERR_SUCCESS;
}

#else /* #if NEXUS_NUM_SPDIF_INPUTS */

typedef struct NEXUS_SpdifInput
{
    NEXUS_OBJECT(NEXUS_SpdifInput);
} NEXUS_SpdifInput;

void NEXUS_SpdifInput_GetDefaultSettings(
    NEXUS_SpdifInputSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

NEXUS_SpdifInputHandle NEXUS_SpdifInput_Open(  /* attr{destructor=NEXUS_SpdifInput_Close}  */
    unsigned index,
    const NEXUS_SpdifInputSettings *pSettings
    )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

static void NEXUS_SpdifInput_P_Finalizer(
    NEXUS_SpdifInputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

void NEXUS_SpdifInput_GetSettings(
    NEXUS_SpdifInputHandle handle,
    NEXUS_SpdifInputSettings *pSettings    /* [out] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_SpdifInput_SetSettings(
    NEXUS_SpdifInputHandle handle,
    const NEXUS_SpdifInputSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_AudioInput NEXUS_SpdifInput_GetConnector( /* attr{shutdown=NEXUS_AudioInput_Shutdown} */
    NEXUS_SpdifInputHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}

#endif /* #if NEXUS_NUM_SPDIF_INPUTS */

NEXUS_OBJECT_CLASS_MAKE(NEXUS_SpdifInput, NEXUS_SpdifInput_Close);

