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
* $brcm_Workfile: nexus_dolby_digital_reencode.c $
* $brcm_Revision: 10 $
* $brcm_Date: 10/9/12 11:03p $
*
* API Description:
*   API name: DolbyDigitalReencode
*    Specific APIs related to Dolby Digital Reencoding used in Dolby MS11
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_dolby_digital_reencode.c $
* 
* 10   10/9/12 11:03p jgarrett
* SW7425-4042: Mapping additional DDRE settings
* 
* 9   5/4/12 11:05a jgarrett
* SW7425-2075: Merge to main branch
* 
* SW7425-2075/1   4/16/12 3:46p jgarrett
* SW7425-2075: Adding Compressed 4x/16x connectors
* 
* 8   2/29/12 11:31a jgarrett
* SW7425-943: Fixing crash on close
* 
* 7   2/1/12 3:10p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* 6   1/23/12 10:19a jgarrett
* SW7425-604: Fixing data types for stereo and compressed outputs
* 
* 5   1/18/12 3:54p jgarrett
* SW7231-581: Setting connector types for DDRE
* 
* 4   12/20/11 4:18p jgarrett
* SW7425-1018: Propagating loudness equivalence setting to APE
* 
* 3   12/20/11 4:13p jgarrett
* SW7425-1018: Adding initial A/85 implementation
* 
* 2   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 1   1/10/11 3:26p jgarrett
* SW7422-146: Adding full nexus API set
* 
***************************************************************************/

#include "nexus_audio_module.h"

BDBG_MODULE(nexus_dolby_digital_reencode);

BDBG_OBJECT_ID(NEXUS_DolbyDigitalReencode);
typedef struct NEXUS_DolbyDigitalReencode
{
    BDBG_OBJECT(NEXUS_DolbyDigitalReencode)
    NEXUS_AudioInputObject connector;
    NEXUS_DolbyDigitalReencodeSettings settings;
    NEXUS_AudioInputObject connectors[NEXUS_AudioConnectorType_eMax];
    NEXUS_AudioInput input;
    BAPE_DolbyDigitalReencodeHandle apeHandle;
} NEXUS_DolbyDigitalReencode;


void NEXUS_DolbyDigitalReencode_GetDefaultSettings(
    NEXUS_DolbyDigitalReencodeSettings *pSettings   /* [out] */
    )
{
    BAPE_DolbyDigitalReencodeSettings piSettings;
    BDBG_ASSERT(NULL != pSettings);
    BAPE_DolbyDigitalReencode_GetDefaultSettings(&piSettings);
    pSettings->encodeSettings.spdifHeaderEnabled = piSettings.encodeSettings.spdifHeaderEnabled;
    pSettings->encodeSettings.certificationMode = piSettings.encodeSettings.certificationMode;
    pSettings->loudnessEquivalenceEnabled = piSettings.loudnessEquivalenceEnabled;
    pSettings->externalPcmMode = piSettings.externalPcmMode;
    BDBG_CASSERT((int)NEXUS_DolbyDigitalReencodeProfile_eMax == (int)BAPE_DolbyDigitalReencodeProfile_eMax);
    pSettings->profile = piSettings.profile;
    pSettings->centerMixLevel = piSettings.centerMixLevel;      /* These are defined by the AC3 spec and don't require CASSERT. */
    pSettings->surroundMixLevel = piSettings.surroundMixLevel;  /* These are defined by the AC3 spec and don't require CASSERT. */
    pSettings->dolbySurround = piSettings.dolbySurround;        /* These are defined by the AC3 spec and don't require CASSERT. */
    pSettings->dialogLevel = piSettings.dialogLevel;
    pSettings->drcMode = piSettings.drcMode == BAPE_DolbyDigitalReencodeDrcMode_eLine?NEXUS_AudioDecoderDolbyDrcMode_eLine:NEXUS_AudioDecoderDolbyDrcMode_eRf;
    pSettings->cut = piSettings.drcScaleHi;
    pSettings->boost = piSettings.drcScaleLow;
    pSettings->stereoDownmixMode = piSettings.stereoMode;
    BDBG_CASSERT((int)NEXUS_DolbyDigitalReencodeDownmixMode_eMax == (int)BAPE_DolbyDigitalReencodeStereoMode_eMax);
    pSettings->dualMonoMode = piSettings.dualMonoMode;
    BDBG_CASSERT((int)NEXUS_AudioDecoderDualMonoMode_eMax == (int)BAPE_DualMonoMode_eMax);
}

NEXUS_DolbyDigitalReencodeHandle NEXUS_DolbyDigitalReencode_Open(
    const NEXUS_DolbyDigitalReencodeSettings *pSettings
    )
{
    NEXUS_DolbyDigitalReencodeSettings defaults;
    NEXUS_DolbyDigitalReencodeHandle handle;
    BAPE_Connector connector;
    BERR_Code errCode;

    if ( NULL == pSettings )
    {
        NEXUS_DolbyDigitalReencode_GetDefaultSettings(&defaults);
        pSettings = &defaults;
    }

    handle = BKNI_Malloc(sizeof(NEXUS_DolbyDigitalReencode));
    if ( NULL == handle )
    {
        (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_malloc;
    }
    BKNI_Memset(handle, 0, sizeof(NEXUS_DolbyDigitalReencode));

    errCode = BAPE_DolbyDigitalReencode_Create(NEXUS_AUDIO_DEVICE_HANDLE, NULL, &handle->apeHandle);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_ape_handle;
    }

    BDBG_OBJECT_SET(handle, NEXUS_DolbyDigitalReencode);
    NEXUS_AUDIO_INPUT_INIT(&handle->connectors[NEXUS_AudioConnectorType_eStereo], NEXUS_AudioInputType_eDolbyDigitalReencode, handle);
    handle->connectors[NEXUS_AudioConnectorType_eStereo].format = NEXUS_AudioInputFormat_ePcmStereo;
    BAPE_DolbyDigitalReencode_GetConnector(handle->apeHandle, BAPE_ConnectorFormat_eStereo, &connector);
    handle->connectors[NEXUS_AudioConnectorType_eStereo].port = (uint32_t)connector;
    NEXUS_AUDIO_INPUT_INIT(&handle->connectors[NEXUS_AudioConnectorType_eMultichannel], NEXUS_AudioInputType_eDolbyDigitalReencode, handle);
    handle->connectors[NEXUS_AudioConnectorType_eMultichannel].format = NEXUS_AudioInputFormat_ePcm5_1;
    BAPE_DolbyDigitalReencode_GetConnector(handle->apeHandle, BAPE_ConnectorFormat_eMultichannel, &connector);
    handle->connectors[NEXUS_AudioConnectorType_eMultichannel].port = (uint32_t)connector;
    NEXUS_AUDIO_INPUT_INIT(&handle->connectors[NEXUS_AudioConnectorType_eCompressed], NEXUS_AudioInputType_eDolbyDigitalReencode, handle);
    handle->connectors[NEXUS_AudioConnectorType_eCompressed].format = NEXUS_AudioInputFormat_eCompressed;
    BAPE_DolbyDigitalReencode_GetConnector(handle->apeHandle, BAPE_ConnectorFormat_eCompressed, &connector);
    handle->connectors[NEXUS_AudioConnectorType_eCompressed].port = (uint32_t)connector;

    errCode = NEXUS_DolbyDigitalReencode_SetSettings(handle, pSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_settings;
    }

    return handle;
    
err_settings:
    BAPE_DolbyDigitalReencode_Destroy(handle->apeHandle);
    BDBG_OBJECT_DESTROY(handle, NEXUS_DolbyDigitalReencode);
err_ape_handle:    
    BKNI_Free(handle);
err_malloc:
    return NULL;
}

void NEXUS_DolbyDigitalReencode_Close(
    NEXUS_DolbyDigitalReencodeHandle handle
    )
{
    unsigned i;
    BDBG_OBJECT_ASSERT(handle, NEXUS_DolbyDigitalReencode);
    for ( i = 0; i < NEXUS_AudioConnectorType_eMax; i++ )
    {
        switch ( i )
        {
        case NEXUS_AudioConnectorType_eStereo:
        case NEXUS_AudioConnectorType_eMultichannel:
        case NEXUS_AudioConnectorType_eCompressed:
            NEXUS_AudioInput_Shutdown(&handle->connectors[i]);
            break;
        default:
            break;
        }
    }
    BAPE_DolbyDigitalReencode_Destroy(handle->apeHandle);
    BDBG_OBJECT_DESTROY(handle, NEXUS_DolbyDigitalReencode);
    BKNI_Free(handle);
}

void NEXUS_DolbyDigitalReencode_GetSettings(
    NEXUS_DolbyDigitalReencodeHandle handle,
    NEXUS_DolbyDigitalReencodeSettings *pSettings   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_DolbyDigitalReencode);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

NEXUS_Error NEXUS_DolbyDigitalReencode_SetSettings(
    NEXUS_DolbyDigitalReencodeHandle handle,
    const NEXUS_DolbyDigitalReencodeSettings *pSettings
    )
{
    BERR_Code errCode;
    BAPE_DolbyDigitalReencodeSettings piSettings;

    BDBG_OBJECT_ASSERT(handle, NEXUS_DolbyDigitalReencode);
    BDBG_ASSERT(NULL != pSettings);

    BAPE_DolbyDigitalReencode_GetSettings(handle->apeHandle, &piSettings);

    piSettings.encodeSettings.spdifHeaderEnabled = pSettings->encodeSettings.spdifHeaderEnabled;
    piSettings.encodeSettings.certificationMode = pSettings->encodeSettings.certificationMode;
    piSettings.loudnessEquivalenceEnabled = pSettings->loudnessEquivalenceEnabled;
    piSettings.externalPcmMode = pSettings->externalPcmMode;
    piSettings.profile = pSettings->profile;
    piSettings.centerMixLevel = pSettings->centerMixLevel;
    piSettings.surroundMixLevel = pSettings->surroundMixLevel;
    piSettings.dolbySurround = pSettings->dolbySurround;
    piSettings.dialogLevel = pSettings->dialogLevel;
    piSettings.drcMode = (pSettings->drcMode == NEXUS_AudioDecoderDolbyDrcMode_eRf) ? BAPE_DolbyDigitalReencodeDrcMode_eRf : BAPE_DolbyDigitalReencodeDrcMode_eLine; 
    piSettings.drcModeDownmix = piSettings.drcMode;
    piSettings.drcScaleHi = pSettings->cut;
    piSettings.drcScaleLow = pSettings->boost;
    piSettings.drcScaleHiDownmix = pSettings->cut;
    piSettings.drcScaleLowDownmix = pSettings->boost;
    piSettings.stereoMode = pSettings->stereoDownmixMode;
    piSettings.dualMonoMode = pSettings->dualMonoMode;

    errCode = BAPE_DolbyDigitalReencode_SetSettings(handle->apeHandle, &piSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    handle->settings = *pSettings;

    return BERR_SUCCESS;
}

NEXUS_AudioInput NEXUS_DolbyDigitalReencode_GetConnector( /* attr{shutdown=NEXUS_AudioInput_Shutdown} */
    NEXUS_DolbyDigitalReencodeHandle handle,
    NEXUS_AudioConnectorType connectorType
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_DolbyDigitalReencode);

    switch ( connectorType )
    {
    case NEXUS_AudioConnectorType_eStereo:
    case NEXUS_AudioConnectorType_eMultichannel:
    case NEXUS_AudioConnectorType_eCompressed:
        break;
    default:
        BDBG_ERR(("Unsuported connector type %u", connectorType));
        (void)BERR_TRACE(BERR_INVALID_PARAMETER);
        return NULL;
    }

    return &handle->connectors[connectorType];
}

NEXUS_Error NEXUS_DolbyDigitalReencode_AddInput(
    NEXUS_DolbyDigitalReencodeHandle handle,
    NEXUS_AudioInput input
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_DolbyDigitalReencode);
    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    if ( handle->input )
    {
        BDBG_ERR(("Already connected to another input source"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Add to the PI */
    errCode = BAPE_DolbyDigitalReencode_AddInput(handle->apeHandle, (BAPE_Connector)input->port);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_ape;
    }
    errCode = NEXUS_AudioInput_P_AddInput(&handle->connectors[NEXUS_AudioConnectorType_eStereo], input);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_stereo;
    }
    errCode = NEXUS_AudioInput_P_AddInput(&handle->connectors[NEXUS_AudioConnectorType_eMultichannel], input);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_multichannel;
    }
    errCode = NEXUS_AudioInput_P_AddInput(&handle->connectors[NEXUS_AudioConnectorType_eCompressed], input);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_compressed;
    }
    handle->input = input;

    return BERR_SUCCESS;

err_compressed:
    NEXUS_AudioInput_P_RemoveInput(&handle->connectors[NEXUS_AudioConnectorType_eMultichannel], input);
err_multichannel:
    NEXUS_AudioInput_P_RemoveInput(&handle->connectors[NEXUS_AudioConnectorType_eStereo], input);
err_stereo:
    BAPE_DolbyDigitalReencode_RemoveInput(handle->apeHandle, (BAPE_Connector)input->port);
err_ape:
    return errCode;
}

NEXUS_Error NEXUS_DolbyDigitalReencode_RemoveInput(
    NEXUS_DolbyDigitalReencodeHandle handle,
    NEXUS_AudioInput input
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_DolbyDigitalReencode);
    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    if ( input != handle->input )
    {
        BDBG_ERR(("Not connected to input %#x", input));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    NEXUS_AudioInput_P_RemoveInput(&handle->connectors[NEXUS_AudioConnectorType_eCompressed], input);
    NEXUS_AudioInput_P_RemoveInput(&handle->connectors[NEXUS_AudioConnectorType_eMultichannel], input);
    NEXUS_AudioInput_P_RemoveInput(&handle->connectors[NEXUS_AudioConnectorType_eStereo], input);
    BAPE_DolbyDigitalReencode_RemoveInput(handle->apeHandle, (BAPE_Connector)input->port);
    handle->input = NULL;
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_DolbyDigitalReencode_RemoveAllInputs(
    NEXUS_DolbyDigitalReencodeHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_DolbyDigitalReencode);
    if ( handle->input )
    {
        return NEXUS_DolbyDigitalReencode_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

