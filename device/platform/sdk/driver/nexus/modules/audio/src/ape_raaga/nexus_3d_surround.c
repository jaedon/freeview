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
* $brcm_Workfile: nexus_3d_surround.c $
* $brcm_Revision: 6 $
* $brcm_Date: 6/19/12 12:41p $
*
* API Description:
*   API name: AutoVolumeLevel
*    Specific APIs related to Broadcom AutoVolumeLevel (AVL) Audio Processing
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7422/src/nexus_3d_surround.c $
* 
* 6   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 5   4/25/12 5:48p jgarrett
* SW7425-2736: Correcting JIRA number
* 
* 3   8/8/11 5:38p jgarrett
* SWDTV-6761: Adding StudioSound
* 
* 2   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 1   5/16/11 4:57p jgarrett
* SWDTV-6763: Adding 3D Surround
* 
***************************************************************************/

#include "nexus_audio_module.h"

BDBG_MODULE(nexus_3d_surround);

typedef struct NEXUS_3dSurround
{
    NEXUS_OBJECT(NEXUS_3dSurround);
    NEXUS_AudioInputObject connector;
    NEXUS_3dSurroundSettings settings;
    NEXUS_AudioInput input;
    BAPE_3dSurroundHandle apeHandle;
} NEXUS_3dSurround;

void NEXUS_3dSurround_GetDefaultSettings(
    NEXUS_3dSurroundSettings *pSettings   /* [out] default settings */
    )
{
    BAPE_3dSurroundSettings piSettings;
    BDBG_ASSERT(NULL != pSettings);
    BAPE_3dSurround_GetDefaultSettings(&piSettings);
    pSettings->enabled = piSettings.enabled;
    pSettings->softLimiterEnabled = piSettings.softLimiterEnabled;
    switch ( piSettings.speakerPosition )
    {
    default:
    case BAPE_3dSurroundSpeakerPosition_eCenter:
        pSettings->speakerPosition = NEXUS_3dSurroundSpeakerPosition_eCenter;
        break;
    case BAPE_3dSurroundSpeakerPosition_eWide:
        pSettings->speakerPosition = NEXUS_3dSurroundSpeakerPosition_eWide;
        break;
    case BAPE_3dSurroundSpeakerPosition_eExtraWide:
        pSettings->speakerPosition = NEXUS_3dSurroundSpeakerPosition_eExtraWide;
        break;
    }
}

NEXUS_3dSurroundHandle NEXUS_3dSurround_Open(
    const NEXUS_3dSurroundSettings *pSettings     /* Pass NULL for default settings */
    )
{
    NEXUS_3dSurroundHandle handle;
    BAPE_3dSurroundSettings defaults;
    BAPE_Connector connector;
    BERR_Code errCode;
    handle = BKNI_Malloc(sizeof(NEXUS_3dSurround));
    if ( NULL == handle )
    {
        (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_3dSurround, handle);
    NEXUS_AUDIO_INPUT_INIT(&handle->connector, NEXUS_AudioInputType_e3dSurround, handle);
    handle->connector.format = NEXUS_AudioInputFormat_eNone; /* Determined by inputs */
    BAPE_3dSurround_GetDefaultSettings(&defaults);
    errCode = BAPE_3dSurround_Create(NEXUS_AUDIO_DEVICE_HANDLE, &defaults, &handle->apeHandle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        BDBG_OBJECT_DESTROY(handle, NEXUS_3dSurround);
        BKNI_Free(handle);
        return NULL;
    }
    handle->connector.format = NEXUS_AudioInputFormat_ePcmStereo;
    BAPE_3dSurround_GetConnector(handle->apeHandle, &connector);
    handle->connector.port = (uint32_t)connector;
    if ( NULL == pSettings )
    {
        NEXUS_3dSurround_GetDefaultSettings(&handle->settings);
    }
    else
    {
        (void)NEXUS_3dSurround_SetSettings(handle, pSettings);
    }

    return handle;
}

static void NEXUS_3dSurround_P_Finalizer(
    NEXUS_3dSurroundHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_3dSurround, handle);
    NEXUS_AudioInput_Shutdown(&handle->connector);
    BAPE_3dSurround_Destroy(handle->apeHandle);
    NEXUS_OBJECT_DESTROY(NEXUS_3dSurround, handle);
    BKNI_Free(handle);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_3dSurround, NEXUS_3dSurround_Close);

void NEXUS_3dSurround_GetSettings(
    NEXUS_3dSurroundHandle handle,
    NEXUS_3dSurroundSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_3dSurround);
    BDBG_ASSERT(NULL != pSettings);
    
    BKNI_Memcpy(pSettings, &handle->settings, sizeof(NEXUS_3dSurroundSettings));
}

NEXUS_Error NEXUS_3dSurround_SetSettings(
    NEXUS_3dSurroundHandle handle,
    const NEXUS_3dSurroundSettings *pSettings
    )
{
    BERR_Code errCode;
    BAPE_3dSurroundSettings piSettings;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_3dSurround);
    BDBG_ASSERT(NULL != pSettings);

    BAPE_3dSurround_GetSettings(handle->apeHandle, &piSettings);
    
    piSettings.enabled = pSettings->enabled;
    piSettings.softLimiterEnabled = pSettings->softLimiterEnabled;

    switch ( pSettings->speakerPosition )
    {
    default:
    case NEXUS_3dSurroundSpeakerPosition_eCenter:
        piSettings.speakerPosition = BAPE_3dSurroundSpeakerPosition_eCenter;
        break;
    case NEXUS_3dSurroundSpeakerPosition_eWide:
        piSettings.speakerPosition = BAPE_3dSurroundSpeakerPosition_eWide;
        break;
    case NEXUS_3dSurroundSpeakerPosition_eExtraWide:
        piSettings.speakerPosition = BAPE_3dSurroundSpeakerPosition_eExtraWide;
        break;
    }

    errCode = BAPE_3dSurround_SetSettings(handle->apeHandle, &piSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    BKNI_Memcpy(&handle->settings, pSettings, sizeof(NEXUS_3dSurroundSettings));

    return BERR_SUCCESS;
}

NEXUS_AudioInput NEXUS_3dSurround_GetConnector(
    NEXUS_3dSurroundHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_3dSurround);
    return &handle->connector;
}

NEXUS_Error NEXUS_3dSurround_AddInput(
    NEXUS_3dSurroundHandle handle,
    NEXUS_AudioInput input
    )
{
    NEXUS_Error errCode;
    BDBG_OBJECT_ASSERT(handle, NEXUS_3dSurround);
    BDBG_ASSERT(NULL != input);
    if ( NULL != handle->input )
    {
        BDBG_ERR(("Only one input can be added"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    errCode = BAPE_3dSurround_AddInput(handle->apeHandle, (BAPE_Connector)input->port);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    errCode = NEXUS_AudioInput_P_AddInput(&handle->connector, input);
    if ( errCode )
    {
        (void)BAPE_3dSurround_RemoveInput(handle->apeHandle, (BAPE_Connector)input->port);
        return BERR_TRACE(errCode);
    }
    handle->input = input;
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_3dSurround_RemoveInput(
    NEXUS_3dSurroundHandle handle,
    NEXUS_AudioInput input
    )
{
    NEXUS_Error errCode;
    BDBG_OBJECT_ASSERT(handle, NEXUS_3dSurround);
    BDBG_ASSERT(NULL != handle->input);
    if ( input != handle->input )
    {
        BDBG_ERR(("Input not connected"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    errCode = BAPE_3dSurround_RemoveInput(handle->apeHandle, (BAPE_Connector)input->port);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    errCode = NEXUS_AudioInput_P_RemoveInput(&handle->connector, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->input = NULL;
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_3dSurround_RemoveAllInputs(
    NEXUS_3dSurroundHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_3dSurround);
    if ( handle->input )
    {
        return NEXUS_3dSurround_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

