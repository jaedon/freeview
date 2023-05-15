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
* $brcm_Workfile: nexus_ac3_encode.c $
* $brcm_Revision: 23 $
* $brcm_Date: 6/28/12 11:22a $
*
* API Description:
*   API name: Ac3Encode
*    Specific APIs related to SRS-XT audio processing
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7125/src/nexus_ac3_encode.c $
* 
* 23   6/28/12 11:22a mward
* SW7420-2085: Free the destroyed object.
* 
* 22   6/19/12 5:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 21   3/3/11 5:05p jrubio
* SW7346-99: remove legacy code to turn on AC3_ENCODE
* 
* 20   2/28/11 2:01p randyjew
* SW7344-9:Add AC3 encode support for 7344
* 
* 19   2/24/11 10:46a nickh
* SW7422-289: Add 7422/25 AC3 encode support
* 
* 18   2/15/11 8:54a akam
* SW35125-83: Included AC3 encode support for 35125.
* 
* 17   10/27/10 4:33p erickson
* SW7420-1215: add BDBG_OBJECT checking to NEXUS_AudioInput and
*  NEXUS_AudioOutput
* 
* 16   8/19/10 11:46a akam
* SW35230-211: Update for 35230 RAP base4 code changes.
* 
* 15   5/3/10 5:22p jgarrett
* SW7405-4212: Removing DolbyPulse as an explicit codec type
* 
* SW7405-4212/1   4/26/10 5:23p jgarrett
* SW7405-4212: Adding MS10 changes for pulse/transcode
* 
* 14   4/8/10 4:30p mward
* SW7125-226: Enabling AC3 Encode for 7125
* 
* 13   3/23/10 4:02p jgarrett
* SW7405-4030: Supporting Dolby Pulse Transcode
* 
* 12   3/15/10 7:14p randyjew
* SW7468-6: Enabling AC3 Encode for 7468
* 
* 11   1/16/10 2:18p rjain
* SW7550-190: Enabling AC3 Encode for 7550
* 
* 10   12/10/09 3:27p jgarrett
* SW7420-500: Making ProcessingStageSettings static
* 
* 9   5/29/09 2:38p jrubio
* PR55232: add 7342/7340
* 
* 8   5/29/09 2:21p nickh
* PR53564: Add support for 7420
* 
* 7   3/2/09 5:37p jgarrett
* PR 51468: Renaming globals to match naming conventions
* 
* 6   2/12/09 4:37p jrubio
* PR51629: add 7336
* 
* 5   1/9/09 9:48a katrep
* PR50063: Enable Ac3 encode for 7405,7325 and 7335
* 
* 4   10/7/08 9:01a jgarrett
* PR 47509: Fixing flags for 7405
* 
* 3   10/4/08 9:57a jgarrett
* PR 47509: Stubbing AC3 encode for 7405
* 
* 2   10/4/08 6:22a jgarrett
* PR 45709: Adding AC3 encode
* 
***************************************************************************/

#include "nexus_audio_module.h"

BDBG_MODULE(nexus_ac3_encode);

static BLST_S_HEAD(StageList, NEXUS_Ac3Encode) g_stageList;

typedef struct NEXUS_Ac3Encode
{
    NEXUS_OBJECT(NEXUS_Ac3Encode);
    BLST_S_ENTRY(NEXUS_Ac3Encode) node;
    NEXUS_AudioInputObject connector;
    NEXUS_Ac3EncodeSettings settings;
    NEXUS_AudioInput input;
    BRAP_ProcessingStageHandle stage;
} NEXUS_Ac3Encode;

#if BCHP_CHIP!=7400
#define NEXUS_HAS_AC3_ENCODE 1
#endif

void NEXUS_Ac3Encode_GetDefaultSettings(
    NEXUS_Ac3EncodeSettings *pSettings   /* [out] default settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    /* Taken from PI */
    pSettings->spdifHeaderEnabled = true;
}

NEXUS_Ac3EncodeHandle NEXUS_Ac3Encode_Open(
    const NEXUS_Ac3EncodeSettings *pSettings     /* Pass NULL for default settings */
    )
{
#if NEXUS_HAS_AC3_ENCODE
    NEXUS_Error errCode;
    NEXUS_Ac3Encode *pStage;    

    pStage = BKNI_Malloc(sizeof(*pStage));
    if ( NULL == pStage )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_Ac3Encode, pStage);
    NEXUS_AUDIO_INPUT_INIT(&pStage->connector, NEXUS_AudioInputType_eAc3Encode, pStage);
    pStage->connector.format = NEXUS_AudioInputFormat_eCompressed;

    if ( pSettings )
    {
        pStage->settings = *pSettings;
    }
    else
    {
        NEXUS_Ac3Encode_GetDefaultSettings(&pStage->settings);
    }

    BRAP_GetDefaultProcessingStageSettings(BRAP_ProcessingType_eEncodeAc3,
                                           &g_NEXUS_StageSettings);
    errCode = BRAP_CreateProcessingStage(g_NEXUS_audioModuleData.hRap,
                                         &g_NEXUS_StageSettings,
                                         &pStage->stage);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        NEXUS_Ac3Encode_Close(pStage);
        return NULL;
    }

    BLST_S_INSERT_HEAD(&g_stageList, pStage, node);

    /* Apply settings */
    (void)NEXUS_Ac3Encode_SetSettings(pStage, &pStage->settings);

    return pStage;
#else
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
#endif
}

static void NEXUS_Ac3Encode_P_Finalizer(
    NEXUS_Ac3EncodeHandle handle
    )
{
#if NEXUS_HAS_AC3_ENCODE
    NEXUS_Error errCode;

    NEXUS_OBJECT_ASSERT(NEXUS_Ac3Encode, handle);

    if ( handle->connector.pMixerData )
    {
        BDBG_ERR(("This object is still connected.  Please call NEXUS_AudioInput_Shutdown first."));
        errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
        return;
    }

    if ( NULL != handle->stage )
    {
        BRAP_DestroyProcessingStage(handle->stage);
        BLST_S_REMOVE(&g_stageList, handle, NEXUS_Ac3Encode, node);
    }

    NEXUS_OBJECT_DESTROY(NEXUS_Ac3Encode, handle);
    BKNI_Free(handle);
#else
    BSTD_UNUSED(handle);
#endif
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Ac3Encode, NEXUS_Ac3Encode_Close);

void NEXUS_Ac3Encode_GetSettings(
    NEXUS_Ac3EncodeHandle handle,
    NEXUS_Ac3EncodeSettings *pSettings    /* [out] Settings */
    )
{
#if NEXUS_HAS_AC3_ENCODE
    BDBG_OBJECT_ASSERT(handle, NEXUS_Ac3Encode);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
#else
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
#endif
}

NEXUS_Error NEXUS_Ac3Encode_SetSettings(
    NEXUS_Ac3EncodeHandle handle,
    const NEXUS_Ac3EncodeSettings *pSettings
    )
{
#if NEXUS_HAS_AC3_ENCODE
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Ac3Encode);
    BDBG_ASSERT(NULL != pSettings);

    handle->settings = *pSettings;

    BRAP_GetCurrentProcessingStageSettings(handle->stage, &g_NEXUS_StageSettings);
    BRAP_GetDefaultProcessingStageSettings(BRAP_ProcessingType_eEncodeAc3, &g_NEXUS_StageSettings);
    g_NEXUS_StageSettings.uConfigParams.sAc3ENCParams.bSpdifHeaderEnable = handle->settings.spdifHeaderEnabled;
    errCode = BRAP_SetProcessingStageSettings(handle->stage, &g_NEXUS_StageSettings);
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

NEXUS_AudioInput NEXUS_Ac3Encode_GetConnector(
    NEXUS_Ac3EncodeHandle handle
    )
{
#if NEXUS_HAS_AC3_ENCODE
    BDBG_OBJECT_ASSERT(handle, NEXUS_Ac3Encode);
    return &handle->connector;
#else
    BSTD_UNUSED(handle);
    return NULL;
#endif
}

NEXUS_Error NEXUS_Ac3Encode_AddInput(
    NEXUS_Ac3EncodeHandle handle,
    NEXUS_AudioInput input
    )
{
#if NEXUS_HAS_AC3_ENCODE
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Ac3Encode);
    BDBG_ASSERT(NULL != input);

    if ( handle->input )
    {
        BDBG_ERR(("An input is already connected to this processing stage"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Check if new input is already running */
    if ( NEXUS_AudioInput_P_IsRunning(input) )
    {
        BDBG_ERR(("Input %p is running.  Please stop first.", input));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Connect at input node */
    errCode = NEXUS_AudioInput_P_AddInput(&handle->connector, input);

    if ( BERR_SUCCESS == errCode )
    {
        handle->input = input;
    }

    return errCode;
#else
    BSTD_UNUSED(handle);
    BSTD_UNUSED(input);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

NEXUS_Error NEXUS_Ac3Encode_RemoveInput(
    NEXUS_Ac3EncodeHandle handle,
    NEXUS_AudioInput input
    )
{
#if NEXUS_HAS_AC3_ENCODE
    BDBG_OBJECT_ASSERT(handle, NEXUS_Ac3Encode);
    BDBG_ASSERT(NULL != input);

    if ( handle->input != input )
    {
        BDBG_ERR(("Input %p is not connected to this processing stage", input));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Check if new input is already running */
    if ( NEXUS_AudioInput_P_IsRunning(input) )
    {
        BDBG_ERR(("Input %p is running.  Please stop first.", input));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Disconnect at input node */
    handle->input = NULL;
    return NEXUS_AudioInput_P_RemoveInput(&handle->connector, input);
#else
    BSTD_UNUSED(handle);
    BSTD_UNUSED(input);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

NEXUS_Error NEXUS_Ac3Encode_RemoveAllInputs(
    NEXUS_Ac3EncodeHandle handle
    )
{
#if NEXUS_HAS_AC3_ENCODE
    BDBG_OBJECT_ASSERT(handle, NEXUS_Ac3Encode);

    if ( handle->input )
    {
        return NEXUS_Ac3Encode_RemoveInput(handle, handle->input);
    }

    return BERR_SUCCESS;
#else
    BSTD_UNUSED(handle);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

NEXUS_Error NEXUS_Ac3Encode_P_Init(void)
{
    BLST_S_INIT(&g_stageList);
    return BERR_SUCCESS;
}

void NEXUS_Ac3Encode_P_Uninit(void)
{
    NEXUS_Ac3Encode *pNode;
    while ( NULL != (pNode = BLST_S_FIRST(&g_stageList)) )
    {
        NEXUS_Ac3Encode_Close(pNode);
    }
}

BRAP_ProcessingStageHandle NEXUS_Ac3Encode_P_GetStageHandle(NEXUS_Ac3EncodeHandle handle, NEXUS_AudioCodec codec)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_Ac3Encode);
    BSTD_UNUSED(codec);

    return handle->stage;
}

