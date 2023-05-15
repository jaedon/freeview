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
* $brcm_Workfile: nexus_dts_encode.c $
* $brcm_Revision: 6 $
* $brcm_Date: 6/19/12 5:42p $
*
* API Description:
*   API name: DtsEncode
*    Specific APIs related to SRS-XT audio processing
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7405/src/nexus_dts_encode.c $
* 
* 6   6/19/12 5:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 5   5/2/12 12:03p jgarrett
* SW7420-2299: Porting client shutdown code from 7425
* 
* 4   10/27/10 4:34p erickson
* SW7420-1215: add BDBG_OBJECT checking to NEXUS_AudioInput and
*  NEXUS_AudioOutput
* 
* 3   12/10/09 3:27p jgarrett
* SW7420-500: Making ProcessingStageSettings static
* 
* 2   3/2/09 5:37p jgarrett
* PR 51468: Renaming globals to match naming conventions
* 
* 1   9/10/08 1:15p jgarrett
* PR 46131: Adding DtsEncode
* 
* 8   9/9/08 5:01p jgarrett
* PR 46637: Adding TruSurroundHd
* 
* 7   8/22/08 4:49p jgarrett
* PR 46029: Setting output mode properly
* 
* 6   8/21/08 2:25p jgarrett
* PR45874: Fixing GetSettings
* 
* PR45874/1   8/20/08 8:19p bingz
* PR45874: The problem of postprocessing get settings functions
* 
* 5   8/6/08 6:32p jgarrett
* PR 44922: Fixing initial settings
* 
* 4   7/22/08 1:00p jgarrett
* PR 44922: Adding config params for SRS-XT
* 
* 3   7/17/08 6:32p jgarrett
* PR 44922: Adding SRS-XT
* 
***************************************************************************/

#include "nexus_audio_module.h"

BDBG_MODULE(nexus_dts_encode);

static BLST_S_HEAD(StageList, NEXUS_DtsEncode) g_stageList;

typedef struct NEXUS_DtsEncode
{
    NEXUS_OBJECT(NEXUS_DtsEncode);
    BLST_S_ENTRY(NEXUS_DtsEncode) node;
    NEXUS_AudioInputObject connector;
    NEXUS_DtsEncodeSettings settings;
    NEXUS_AudioInput input;
    BRAP_ProcessingStageHandle stage;
} NEXUS_DtsEncode;

/***************************************************************************
Summary:
    Get default settings for an SRS-XT processing stage
See Also:
    NEXUS_DtsEncode_Open
 ***************************************************************************/
void NEXUS_DtsEncode_GetDefaultSettings(
    NEXUS_DtsEncodeSettings *pSettings   /* [out] default settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    /* Taken from PI */
    pSettings->spdifHeaderEnabled = true;
}

/***************************************************************************
Summary:
    Open a SRS-XT processing stage
See Also:
    NEXUS_DtsEncode_Close
 ***************************************************************************/
NEXUS_DtsEncodeHandle NEXUS_DtsEncode_Open(
    const NEXUS_DtsEncodeSettings *pSettings     /* Pass NULL for default settings */
    )
{
    NEXUS_Error errCode;
    NEXUS_DtsEncode *pStage;    

    pStage = BKNI_Malloc(sizeof(*pStage));
    if ( NULL == pStage )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_DtsEncode, pStage);
    NEXUS_AUDIO_INPUT_INIT(&pStage->connector, NEXUS_AudioInputType_eDtsEncode, pStage);
    pStage->connector.format = NEXUS_AudioInputFormat_eCompressed;

    if ( pSettings )
    {
        pStage->settings = *pSettings;
    }
    else
    {
        NEXUS_DtsEncode_GetDefaultSettings(&pStage->settings);
    }

    BRAP_GetDefaultProcessingStageSettings(BRAP_ProcessingType_eEncodeDts,
                                           &g_NEXUS_StageSettings);
    errCode = BRAP_CreateProcessingStage(g_NEXUS_audioModuleData.hRap,
                                         &g_NEXUS_StageSettings,
                                         &pStage->stage);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        NEXUS_DtsEncode_Close(pStage);
        return NULL;
    }

    BLST_S_INSERT_HEAD(&g_stageList, pStage, node);

    /* Apply settings */
    (void)NEXUS_DtsEncode_SetSettings(pStage, &pStage->settings);

    return pStage;
}

/***************************************************************************
Summary:
    Close a SRS-XT processing stage
Description:
    Input to the stage must be removed prior to closing.
See Also:
    NEXUS_DtsEncode_Open
    NEXUS_AudioOutput_RemoveInput
    NEXUS_AudioOutput_RemoveAllInputs
 ***************************************************************************/
static void NEXUS_DtsEncode_P_Finalizer(
    NEXUS_DtsEncodeHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_DtsEncode, handle);

    if ( handle->connector.pMixerData )
    {
        NEXUS_AudioInput_Shutdown(&handle->connector);
    }

    if ( NULL != handle->stage )
    {
        BRAP_DestroyProcessingStage(handle->stage);
    }

    BLST_S_REMOVE(&g_stageList, handle, NEXUS_DtsEncode, node);
    NEXUS_OBJECT_DESTROY(NEXUS_DtsEncode, handle);
    BKNI_Free(handle);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_DtsEncode, NEXUS_DtsEncode_Close);


/***************************************************************************
Summary:
    Get Settings for a SRS-XT processing stage
See Also:
    NEXUS_DtsEncode_SetSettings
 ***************************************************************************/
void NEXUS_DtsEncode_GetSettings(
    NEXUS_DtsEncodeHandle handle,
    NEXUS_DtsEncodeSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_DtsEncode);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}


/***************************************************************************
Summary:
    Set Settings for a SRS-XT processing stage
See Also:
    NEXUS_DtsEncode_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_DtsEncode_SetSettings(
    NEXUS_DtsEncodeHandle handle,
    const NEXUS_DtsEncodeSettings *pSettings
    )
{
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_DtsEncode);
    BDBG_ASSERT(NULL != pSettings);

    handle->settings = *pSettings;

    BRAP_GetCurrentProcessingStageSettings(handle->stage, &g_NEXUS_StageSettings);

    g_NEXUS_StageSettings.uConfigParams.sDTSENCParams.bSpdifHeaderEnable = handle->settings.spdifHeaderEnabled;

    errCode = BRAP_SetProcessingStageSettings(handle->stage, &g_NEXUS_StageSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Get the audio connector for a SRS-XT processing stage
See Also:

 ***************************************************************************/
NEXUS_AudioInput NEXUS_DtsEncode_GetConnector(
    NEXUS_DtsEncodeHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_DtsEncode);
    return &handle->connector;
}

/***************************************************************************
Summary:
    Add an input to this processing stage
See Also:
    NEXUS_DtsEncode_RemoveInput
    NEXUS_DtsEncode_RemoveAllInputs
 ***************************************************************************/
NEXUS_Error NEXUS_DtsEncode_AddInput(
    NEXUS_DtsEncodeHandle handle,
    NEXUS_AudioInput input
    )
{
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_DtsEncode);
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
}

/***************************************************************************
Summary:
    Add an input to this processing stage
See Also:
    NEXUS_DtsEncode_AddInput
    NEXUS_DtsEncode_RemoveAllInputs
 ***************************************************************************/
NEXUS_Error NEXUS_DtsEncode_RemoveInput(
    NEXUS_DtsEncodeHandle handle,
    NEXUS_AudioInput input
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_DtsEncode);
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
}

/***************************************************************************
Summary:
    Add an input to this processing stage
See Also:
    NEXUS_DtsEncode_AddInput
    NEXUS_DtsEncode_RemoveInput
 ***************************************************************************/
NEXUS_Error NEXUS_DtsEncode_RemoveAllInputs(
    NEXUS_DtsEncodeHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_DtsEncode);

    if ( handle->input )
    {
        return NEXUS_DtsEncode_RemoveInput(handle, handle->input);
    }

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_DtsEncode_P_Init(void)
{
    BLST_S_INIT(&g_stageList);
    return BERR_SUCCESS;
}

void NEXUS_DtsEncode_P_Uninit(void)
{
    NEXUS_DtsEncode *pNode;
    while ( NULL != (pNode = BLST_S_FIRST(&g_stageList)) )
    {
        NEXUS_DtsEncode_Close(pNode);
    }
}

BRAP_ProcessingStageHandle NEXUS_DtsEncode_P_GetStageHandle(NEXUS_DtsEncodeHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_DtsEncode);
    return handle->stage;
}

