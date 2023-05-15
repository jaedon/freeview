/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
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
* $brcm_Workfile: bape_tru_surround.c $
* $brcm_Revision: Hydra_Software_Devel/12 $
* $brcm_Date: 5/3/12 5:06p $
*
* API Description:
*   API name: TruSurround
*    Specific APIs related to SRS TruSurround Audio Processing
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7422/bape_tru_surround.c $
* 
* Hydra_Software_Devel/12   5/3/12 5:06p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:51p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
* Hydra_Software_Devel/11   8/26/11 4:16p jgarrett
* SWDTV-8402: Adding debug for user configuration
* 
* Hydra_Software_Devel/10   7/15/11 5:51p jgarrett
* SWDTV-6761: Adding StudioSound API
* 
* Hydra_Software_Devel/9   5/27/11 11:20a jgarrett
* SW7425-408: Revising format propagation logic
* 
* Hydra_Software_Devel/8   5/25/11 5:14p jgarrett
* SW7425-408: Implemted fixed output format
* 
* Hydra_Software_Devel/7   4/29/11 3:42p jgarrett
* SW7422-368: Fixing settings structure size in defaults
* 
* Hydra_Software_Devel/6   4/29/11 11:00a jgarrett
* SW7422-368: Switching to new GetDefaultSettings routines
* 
* Hydra_Software_Devel/5   4/19/11 11:10a piyushg
* SW7422-353: Settings the DSP Input Acmod and LFE present flag based on
* certification is enabled or not.
* 
* Hydra_Software_Devel/4   4/19/11 11:08a piyushg
* SW7422-353: Settings the DSP Input Acmod and LFE present flag based on
* certification is enabled or not.
* 
* Hydra_Software_Devel/3   4/19/11 10:15a piyushg
* SW7422-353: Added APE/Nexus support for SRS TruSurroundHD.
* 
* Hydra_Software_Devel/2   2/22/11 5:43p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
* comments
* 
* Hydra_Software_Devel/1   1/12/11 4:24p jgarrett
* SW7422-146: Adding additional APIs
* 
***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_tru_surround);

BDBG_OBJECT_ID(BAPE_TruSurroundHd);

typedef struct BAPE_TruSurroundHd
{
    BDBG_OBJECT(BAPE_TruSurroundHd)
    BAPE_PathNode node;
    BAPE_TruSurroundHdSettings settings;
    BAPE_Connector input;
} BAPE_TruSurroundHd;

static BERR_Code BAPE_TruSurroundHd_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat);
static BERR_Code BAPE_TruSurroundHd_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_TruSurroundHd_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_TruSurroundHd_P_ApplyTruSurroundHdSettings(BAPE_TruSurroundHdHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId);
static void BAPE_TruSurroundHd_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_TruSurroundHd_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

/***************************************************************************
Summary:
    Get default settings for an SRS TruSurround HD stage
***************************************************************************/
void BAPE_TruSurroundHd_GetDefaultSettings(
    BAPE_TruSurroundHdSettings *pSettings   /* [out] default settings */
    )
{
    BDSP_Raaga_Audio_TruSurrndHDConfigParams dspSettings;

    BDBG_ASSERT(NULL != pSettings);
	BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BDSP_Raaga_GetDefaultAudioProcessingSettings(BDSP_AudioProcessing_eSrsHd, 
                                                 (void *)&dspSettings, 
                                                 sizeof(dspSettings)
                                                 );

    pSettings->enabled = dspSettings.i32TruSurrndHDEnableFlag?true:false;
    pSettings->headphoneEnabled = dspSettings.i32HeadPhoneEnableFlag?true:false;
    pSettings->monoToStereoEnabled = dspSettings.i32Mono2StereoEnableFlag?true:false;
    pSettings->dialogClarityEnabled = dspSettings.i32DialogClarityEnableFlag?true:false;
    pSettings->dialogClarityLevel = 50;
    BDBG_ASSERT(0x40000000 == dspSettings.i32DialogClarityCtrl); /*Assert when the default value changes in FW so that we can adjust values in PI*/
    pSettings->truBassEnabled = dspSettings.i32TruBassFrontEnableFlag?true:false;
    pSettings->truBassLevel = 30;
    BDBG_ASSERT(0x26666666 == dspSettings.i32TruBassFrontCtrl);

    switch (dspSettings.eTruBassFrontSpeakerSize) {
    case BDSP_Raaga_Audio_SpeakerSize_eLFResponse40Hz:
        pSettings->speakerSize = BAPE_TruBassSpeakerSize_e40Hz;
        break;
    case BDSP_Raaga_Audio_SpeakerSize_eLFResponse60Hz:
        pSettings->speakerSize = BAPE_TruBassSpeakerSize_e60Hz;
        break;
    case BDSP_Raaga_Audio_SpeakerSize_eLFResponse100Hz:
        pSettings->speakerSize = BAPE_TruBassSpeakerSize_e100Hz;
        break;
    case BDSP_Raaga_Audio_SpeakerSize_eLFResponse150Hz:
        pSettings->speakerSize = BAPE_TruBassSpeakerSize_e150Hz;
        break;
    case BDSP_Raaga_Audio_SpeakerSize_eLFResponse200Hz:
        pSettings->speakerSize = BAPE_TruBassSpeakerSize_e200Hz;
        break;
    case BDSP_Raaga_Audio_SpeakerSize_eLFResponse250Hz:
        pSettings->speakerSize = BAPE_TruBassSpeakerSize_e250Hz;
        break;
    case BDSP_Raaga_Audio_SpeakerSize_eLFResponse300Hz:
        pSettings->speakerSize = BAPE_TruBassSpeakerSize_e300Hz;
        break;
    case BDSP_Raaga_Audio_SpeakerSize_eLFResponse400Hz:
        pSettings->speakerSize = BAPE_TruBassSpeakerSize_e400Hz;
        break;
    default:
        pSettings->speakerSize = BAPE_TruBassSpeakerSize_eMax;
        break;
    }

    switch (dspSettings.eSRSTruBassProcessMode) {
    case BDSP_Raaga_Audio_SRSTruBassProcessMode_eMono:
        pSettings->truBassMode = BAPE_TruBassMode_eMono;
        break;
    case BDSP_Raaga_Audio_SRSTruBassProcessMode_eStereo:
        pSettings->truBassMode = BAPE_TruBassMode_eStereo;
        break;
    default:
        pSettings->truBassMode = BAPE_TruBassMode_eMax;
        break;
    }
    pSettings->inputGain = 50;
    pSettings->outputGain = 100;
    pSettings->bypassGain = 100;
    BDBG_ASSERT(0x40000000 == dspSettings.i32TruSurrndHDInputGain);
    BDBG_ASSERT(0x7fffffff == dspSettings.i32TruSurrndHDOutputGain);
    BDBG_ASSERT(0x7fffffff == dspSettings.i32TruSurrndHDByPassGain);

    pSettings->definitionEnabled = dspSettings.i32DefinitionFrontEnableFlag?true:false;
    pSettings->definitionLevel = 30;
    BDBG_ASSERT(0x26666666 == dspSettings.i32DefinitionFrontCtrl);

    pSettings->surroundLevel = 60;
    BDBG_ASSERT(0x4CCCCCCD == dspSettings.i32SurroundLevel);

    pSettings->wowHdEnabled = dspSettings.i32WowHDSRS3DEnableFlag?true:false;
    pSettings->wowHdHighBitRateEnabled = dspSettings.i32SRS3DHighBitRateFlag?true:false;
    pSettings->wowHdFocusEnabled = dspSettings.i32WowHDFocusEnableFlag?true:false;

    switch (dspSettings.eWowHDSRS3DMode) {
    case BDSP_Raaga_Audio_SRS3DMode_eMono:
        pSettings->wowHdMode = BAPE_WowHdMode_eMono;
        break;
    case BDSP_Raaga_Audio_SRS3DMode_eSingleSpeaker:
        pSettings->wowHdMode = BAPE_WowHdMode_eSingleSpeaker;
        break;
    case BDSP_Raaga_Audio_SRS3DMode_eStereo:
        pSettings->wowHdMode = BAPE_WowHdMode_eStereo;
        break;
    case BDSP_Raaga_Audio_SRS3DMode_eExtreme:
        pSettings->wowHdMode = BAPE_WowHdMode_eExtreme;
        break;
    default:
        pSettings->wowHdMode = BAPE_WowHdMode_eMax;
        break;
    }

    pSettings->wowHdFocusLevel = 40;
    pSettings->wowHdCenterLevel = 50;
    pSettings->wowHdSpaceLevel = 80;
    BDBG_ASSERT(0x33333333 == dspSettings.i32WowHDFocusCtrl);
    BDBG_ASSERT(0x40000000 == dspSettings.i32WowHDSRS3DCenterCtrl);
    BDBG_ASSERT(0x66666666 == dspSettings.i32WowHDSRS3DSpaceCtrl);

    pSettings->certificationApp = dspSettings.i32CertificationEnableFlag?true:false;
    pSettings->inputLfeEnabled = dspSettings.i32InputLFEPresentFlag?true:false;

    switch (dspSettings.eInputAcMode) {
    case BDSP_Raaga_Audio_AcMode_eMode10:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_e1_0_C;        
        break;
    case BDSP_Raaga_Audio_AcMode_eMode20:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_e2_0_LR;       
        break;
    case BDSP_Raaga_Audio_AcMode_eMode30:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_e3_0_LCR;      
        break;
    case BDSP_Raaga_Audio_AcMode_eMode21:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_e2_1_LRS;      
        break;
    case BDSP_Raaga_Audio_AcMode_eMode31:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_e3_1_LCRS;     
        break;
    case BDSP_Raaga_Audio_AcMode_eMode22:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_e2_2_LRLsRs;   
        break;
    case BDSP_Raaga_Audio_AcMode_eMode32:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_e3_2_LCRLsRs;  
        break;
    case BDSP_Raaga_Audio_AcMode_eMode33:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_e3_3_LCRLsRsCs;
        break;
    case BDSP_Raaga_Audio_AcMode_eMode32_BSDigital:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_e3_2_BSDigital;
        break;
    case BDSP_Raaga_Audio_AcMode_eModeLtRt:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_eLtRt;         
        break;
    case BDSP_Raaga_Audio_AcMode_eModePLII_Movie:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_ePLII_Movie;   
        break;
    case BDSP_Raaga_Audio_AcMode_eModePLII_Music:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_ePLII_Music;   
        break;
    case BDSP_Raaga_Audio_AcMode_eModePassiveMatrix:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_ePassiveMatrix;
        break;
    case BDSP_Raaga_Audio_AcMode_eModeCSII:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_eCSII;         
        break;
    default:
        pSettings->inputAcmod = BAPE_TruSurroundHdAcmod_eMax;          
        break;
    }
}

/***************************************************************************
Summary:
    Open an SRS TruSurround HD stage
***************************************************************************/
BERR_Code BAPE_TruSurroundHd_Create(
    BAPE_Handle deviceHandle,
    const BAPE_TruSurroundHdSettings *pSettings,
    BAPE_TruSurroundHdHandle *pHandle               /* [out] */
    )
{
    BAPE_TruSurroundHdHandle handle;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);


    handle = BKNI_Malloc(sizeof(BAPE_TruSurroundHd));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    BKNI_Memset(handle, 0, sizeof(BAPE_TruSurroundHd));
    BDBG_OBJECT_SET(handle, BAPE_TruSurroundHd);
    handle->settings = *pSettings;
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_ePostProcessor, BAPE_PostProcessorType_eTruSurround, 1, deviceHandle, handle);
    handle->node.pName = "TruSurroundHd";
    handle->node.connectors[0].useBufferPool = true;    

    BAPE_Connector_P_GetFormat(&handle->node.connectors[0], &format);
    format.source = BAPE_DataSource_eDspBuffer;
    format.type = BAPE_DataType_ePcmStereo;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[0], &format);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_format; }

    BAPE_PathNode_P_GetInputCapabilities(&handle->node, &caps);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDspBuffer);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcm5_1);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->node, &caps);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_caps; }

    /* Generic Routines */
    handle->node.allocatePathToOutput = BAPE_DSP_P_AllocatePathToOutput;
    handle->node.configPathToOutput = BAPE_DSP_P_ConfigPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;
    handle->node.startPathToOutput = BAPE_DSP_P_StartPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;

    /* TruSurroundHd Specifics */
    handle->node.inputFormatChange = BAPE_TruSurroundHd_P_InputFormatChange;
    handle->node.allocatePathFromInput = BAPE_TruSurroundHd_P_AllocatePathFromInput;
    handle->node.configPathFromInput = BAPE_TruSurroundHd_P_ConfigPathFromInput;
    handle->node.stopPathFromInput = BAPE_TruSurroundHd_P_StopPathFromInput;
    handle->node.removeInput = BAPE_TruSurroundHd_P_RemoveInputCallback;

    *pHandle = handle;
    return BERR_SUCCESS;

err_caps:
err_format:
    BAPE_TruSurroundHd_Destroy(handle);
    return errCode;
}

/***************************************************************************
Summary:
    Close an SRS TruSurround HD stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
void BAPE_TruSurroundHd_Destroy(
    BAPE_TruSurroundHdHandle handle
    )
{
	bool running;
	BDBG_OBJECT_ASSERT(handle, BAPE_TruSurroundHd);
	running = (handle->node.connectors[0].task != NULL)?true:false;
	BDBG_ASSERT(false == running);
	BDBG_ASSERT(NULL == handle->input);
	BDBG_OBJECT_DESTROY(handle, BAPE_TruSurroundHd);
	BKNI_Free(handle);
}

/***************************************************************************
Summary:
    Get Settings for an SRS TruSurround HD stage
***************************************************************************/
void BAPE_TruSurroundHd_GetSettings(
    BAPE_TruSurroundHdHandle handle,
    BAPE_TruSurroundHdSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_TruSurroundHd);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

/***************************************************************************
Summary:
    Set Settings for an SRS TruSurround HD stage
***************************************************************************/
BERR_Code BAPE_TruSurroundHd_SetSettings(
    BAPE_TruSurroundHdHandle handle,
    const BAPE_TruSurroundHdSettings *pSettings
    )
{
	BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruSurroundHd);
    BDBG_ASSERT(NULL != pSettings);

    /* Start time will always call this routine, don't copy the settings if we don't need to */
    if ( pSettings != &handle->settings )
    {
        handle->settings = *pSettings;
    }

    if ( handle->node.connectors[0].task != NULL )
    {
		errCode = BAPE_TruSurroundHd_P_ApplyTruSurroundHdSettings(handle, 
														          handle->node.connectors[0].task, 
														          handle->node.connectors[0].branchId, 
														          handle->node.connectors[0].stageId);
		if ( errCode )
		{
			return BERR_TRACE(errCode);
		}
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Get the audio connector for an SRS TruSurround HD stage
***************************************************************************/
void BAPE_TruSurroundHd_GetConnector(
    BAPE_TruSurroundHdHandle handle,
    BAPE_Connector *pConnector
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_TruSurroundHd);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->node.connectors[0];
}

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
BERR_Code BAPE_TruSurroundHd_AddInput(
    BAPE_TruSurroundHdHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruSurroundHd);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    if ( NULL != handle->input )
    {
        BDBG_ERR(("Can not have more than one input"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    errCode = BAPE_PathNode_P_AddInput(&handle->node, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->input = input;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
BERR_Code BAPE_TruSurroundHd_RemoveInput(
    BAPE_TruSurroundHdHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruSurroundHd);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    if ( input != handle->input )
    {
        BDBG_ERR(("Input %s %s (%#x) is not connected", input->pParent->pName, input->pName, input));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    errCode = BAPE_PathNode_P_RemoveInput(&handle->node, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->input = NULL;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
BERR_Code BAPE_TruSurroundHd_RemoveAllInputs(
    BAPE_TruSurroundHdHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_TruSurroundHd);
    if ( handle->input )
    {
        return BAPE_TruSurroundHd_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_TruSurroundHd_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat)
{
    BAPE_FMT_Descriptor outputFormat;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pNewFormat);

    /* This module will convert the input to stereo.  All other fields are passthrough. */
    outputFormat = *pNewFormat;
    outputFormat.type = BAPE_DataType_ePcmStereo;
    errCode = BAPE_Connector_P_SetFormat(&pNode->connectors[0], &outputFormat);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_TruSurroundHd_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_TruSurroundHdHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->pTaskCreateSettings);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruSurroundHd);
    /* Add stage to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input), 
                                            BDSP_AudioProcessing_eSrsHd,
                                            false,
                                            &pNode->connectors[0].branchId, &pNode->connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pNode->connectors[0].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    return BERR_SUCCESS;
}

static BERR_Code BAPE_TruSurroundHd_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_TruSurroundHdHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->task);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruSurroundHd);
    pNode->connectors[0].task = pConnection->pSource->task;
    errCode = BAPE_TruSurroundHd_P_ApplyTruSurroundHdSettings(handle, 
                                                              handle->node.connectors[0].task, 
                                                              handle->node.connectors[0].branchId, 
                                                              handle->node.connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;    
}

BERR_Code BAPE_TruSurroundHd_P_ConvertSettingsToDsp(
    const BAPE_TruSurroundHdSettings *pSettings, 
    unsigned numChannelPairs,
    BDSP_Raaga_Audio_TruSurrndHDConfigParams *pUserConfig
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pUserConfig);

    pUserConfig->i32TruSurrndHDEnableFlag = pSettings->enabled?1:0;
    BDBG_MSG(("i32TruSurrndHDEnableFlag: %u", pUserConfig->i32TruSurrndHDEnableFlag));
    pUserConfig->i32HeadPhoneEnableFlag = pSettings->headphoneEnabled?1:0;
    BDBG_MSG(("i32HeadPhoneEnableFlag: %u", pUserConfig->i32HeadPhoneEnableFlag));
    pUserConfig->i32TruSurrndHDInputGain = BAPE_P_FloatToQ131(pSettings->inputGain, 100);
    BDBG_MSG(("i32TruSurrndHDInputGain: %u", pUserConfig->i32TruSurrndHDInputGain));
    pUserConfig->i32TruSurrndHDOutputGain = BAPE_P_FloatToQ131(pSettings->outputGain, 100);
    BDBG_MSG(("i32TruSurrndHDOutputGain: %u", pUserConfig->i32TruSurrndHDOutputGain));
    pUserConfig->i32TruSurrndHDByPassGain = BAPE_P_FloatToQ131(pSettings->bypassGain, 100);
    BDBG_MSG(("i32TruSurrndHDByPassGain: %u", pUserConfig->i32TruSurrndHDByPassGain));
    pUserConfig->i32TruBassFrontEnableFlag = pSettings->truBassEnabled?1:0;
    BDBG_MSG(("i32TruBassFrontEnableFlag: %u", pUserConfig->i32TruBassFrontEnableFlag));
    pUserConfig->i32TruBassFrontCtrl = BAPE_P_FloatToQ131(pSettings->truBassLevel, 100);
    BDBG_MSG(("i32TruBassFrontCtrl: %u", pUserConfig->i32TruBassFrontCtrl));
    switch (pSettings->speakerSize) {
    case BAPE_TruBassSpeakerSize_e40Hz:
        pUserConfig->eTruBassFrontSpeakerSize = BDSP_Raaga_Audio_SpeakerSize_eLFResponse40Hz;
        break;
    case BAPE_TruBassSpeakerSize_e60Hz:
        pUserConfig->eTruBassFrontSpeakerSize = BDSP_Raaga_Audio_SpeakerSize_eLFResponse60Hz;
        break;
    case BAPE_TruBassSpeakerSize_e100Hz:
        pUserConfig->eTruBassFrontSpeakerSize = BDSP_Raaga_Audio_SpeakerSize_eLFResponse100Hz;
        break;
    case BAPE_TruBassSpeakerSize_e150Hz:
        pUserConfig->eTruBassFrontSpeakerSize = BDSP_Raaga_Audio_SpeakerSize_eLFResponse150Hz;
        break;
    case BAPE_TruBassSpeakerSize_e200Hz:
        pUserConfig->eTruBassFrontSpeakerSize = BDSP_Raaga_Audio_SpeakerSize_eLFResponse200Hz;
        break;
    case BAPE_TruBassSpeakerSize_e250Hz:
        pUserConfig->eTruBassFrontSpeakerSize = BDSP_Raaga_Audio_SpeakerSize_eLFResponse250Hz;
        break;
    case BAPE_TruBassSpeakerSize_e300Hz:
        pUserConfig->eTruBassFrontSpeakerSize = BDSP_Raaga_Audio_SpeakerSize_eLFResponse300Hz;
        break;
    case BAPE_TruBassSpeakerSize_e400Hz:
        pUserConfig->eTruBassFrontSpeakerSize = BDSP_Raaga_Audio_SpeakerSize_eLFResponse400Hz;
        break;
    default:
        BDBG_WRN(("TruSurroundHd Speaker Size INVALID"));
        pUserConfig->eTruBassFrontSpeakerSize = BDSP_Raaga_Audio_SpeakerSize_eINVALID;
        break;
    }
    BDBG_MSG(("eTruBassFrontSpeakerSize: %u", pUserConfig->eTruBassFrontSpeakerSize));

    switch (pSettings->truBassMode) {
    case BAPE_TruBassMode_eMono:
        pUserConfig->eSRSTruBassProcessMode = BDSP_Raaga_Audio_SRSTruBassProcessMode_eMono;
        break;
    case BAPE_TruBassMode_eStereo:
        pUserConfig->eSRSTruBassProcessMode = BDSP_Raaga_Audio_SRSTruBassProcessMode_eStereo;
        break;
    default:
        BDBG_WRN(("TruSurround Bass Processing Mode INVALID"));
        pUserConfig->eSRSTruBassProcessMode = BDSP_Raaga_Audio_SRSTruBassProcessMode_eInvalid;
        break;
    }
    BDBG_MSG(("eSRSTruBassProcessMode: %u", pUserConfig->eSRSTruBassProcessMode));

    pUserConfig->i32DefinitionFrontEnableFlag = pSettings->definitionEnabled?1:0;
    BDBG_MSG(("i32DefinitionFrontEnableFlag: %u", pUserConfig->i32DefinitionFrontEnableFlag));
    pUserConfig->i32DefinitionFrontCtrl = BAPE_P_FloatToQ131(pSettings->definitionLevel, 100);
    BDBG_MSG(("i32DefinitionFrontCtrl: %u", pUserConfig->i32DefinitionFrontCtrl));
    pUserConfig->i32DialogClarityEnableFlag = pSettings->dialogClarityEnabled?1:0;
    BDBG_MSG(("i32DialogClarityEnableFlag: %u", pUserConfig->i32DialogClarityEnableFlag));
    pUserConfig->i32DialogClarityCtrl = BAPE_P_FloatToQ131(pSettings->dialogClarityLevel, 100);
    BDBG_MSG(("i32DialogClarityCtrl: %u", pUserConfig->i32DialogClarityCtrl));
    pUserConfig->i32SurroundLevel = BAPE_P_FloatToQ131(pSettings->surroundLevel, 100);
    BDBG_MSG(("i32SurroundLevel: %u", pUserConfig->i32SurroundLevel));
    pUserConfig->i32WowHDSRS3DEnableFlag = pSettings->wowHdEnabled?1:0;
    BDBG_MSG(("i32WowHDSRS3DEnableFlag: %u", pUserConfig->i32WowHDSRS3DEnableFlag));
    pUserConfig->i32SRS3DHighBitRateFlag = pSettings->wowHdHighBitRateEnabled?1:0;
    BDBG_MSG(("i32SRS3DHighBitRateFlag: %u", pUserConfig->i32SRS3DHighBitRateFlag));
    switch (pSettings->wowHdMode) {
    case BAPE_WowHdMode_eMono:
        pUserConfig->eWowHDSRS3DMode = BDSP_Raaga_Audio_SRS3DMode_eMono;
        break;
    case BAPE_WowHdMode_eSingleSpeaker:
        pUserConfig->eWowHDSRS3DMode = BDSP_Raaga_Audio_SRS3DMode_eSingleSpeaker;
        break;
    case BAPE_WowHdMode_eStereo:
        pUserConfig->eWowHDSRS3DMode = BDSP_Raaga_Audio_SRS3DMode_eStereo;
        break;
    case BAPE_WowHdMode_eExtreme:
        pUserConfig->eWowHDSRS3DMode = BDSP_Raaga_Audio_SRS3DMode_eExtreme;
        break;
    default:
        BDBG_WRN(("TruSurround SRS3DMode INVALID"));
        pUserConfig->eWowHDSRS3DMode = BDSP_Raaga_Audio_SRS3DMode_eInvalid;
        break;
    }
    BDBG_MSG(("eWowHDSRS3DMode: %u", pUserConfig->eWowHDSRS3DMode));
    pUserConfig->i32WowHDSRS3DSpaceCtrl = BAPE_P_FloatToQ131(pSettings->wowHdSpaceLevel, 100);
    BDBG_MSG(("i32WowHDSRS3DSpaceCtrl: %u", pUserConfig->i32WowHDSRS3DSpaceCtrl));
    pUserConfig->i32WowHDSRS3DCenterCtrl = BAPE_P_FloatToQ131(pSettings->wowHdCenterLevel, 100);
    BDBG_MSG(("i32WowHDSRS3DCenterCtrl: %u", pUserConfig->i32WowHDSRS3DCenterCtrl));
    pUserConfig->i32WowHDFocusEnableFlag = pSettings->wowHdFocusEnabled?1:0;
    BDBG_MSG(("i32WowHDFocusEnableFlag: %u", pUserConfig->i32WowHDFocusEnableFlag));
    pUserConfig->i32WowHDFocusCtrl = BAPE_P_FloatToQ131(pSettings->wowHdFocusLevel, 100);
    BDBG_MSG(("i32WowHDFocusCtrl: %u", pUserConfig->i32WowHDFocusCtrl));
    pUserConfig->i32Mono2StereoEnableFlag = pSettings->monoToStereoEnabled?1:0;
    BDBG_MSG(("i32Mono2StereoEnableFlag: %u", pUserConfig->i32Mono2StereoEnableFlag));

    pUserConfig->i32CertificationEnableFlag = pSettings->certificationApp?1:0;
    BDBG_MSG(("i32CertificationEnableFlag: %u", pUserConfig->i32CertificationEnableFlag));
    if (pSettings->certificationApp) {
        switch (pSettings->inputAcmod) {
        case BAPE_TruSurroundHdAcmod_e1_0_C:
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode10;
            break;
        case BAPE_TruSurroundHdAcmod_e2_0_LR:       
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode20;
            break;
        case BAPE_TruSurroundHdAcmod_e3_0_LCR:      
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode30;
            break;
        case BAPE_TruSurroundHdAcmod_e2_1_LRS:      
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode21;
            break;
        case BAPE_TruSurroundHdAcmod_e3_1_LCRS:     
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode31;
            break;
        case BAPE_TruSurroundHdAcmod_e2_2_LRLsRs:   
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode22;
            break;
        case BAPE_TruSurroundHdAcmod_e3_2_LCRLsRs:  
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode32;
            break;
        case BAPE_TruSurroundHdAcmod_e3_3_LCRLsRsCs:
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode33;
            break;
        case BAPE_TruSurroundHdAcmod_e3_2_BSDigital:
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode32_BSDigital;
            break;
        case BAPE_TruSurroundHdAcmod_eLtRt:         
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eModeLtRt;
            break;
        case BAPE_TruSurroundHdAcmod_ePLII_Movie:   
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eModePLII_Movie;
            break;
        case BAPE_TruSurroundHdAcmod_ePLII_Music:   
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eModePLII_Music;
            break;
        case BAPE_TruSurroundHdAcmod_ePassiveMatrix:
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eModePassiveMatrix;
            break;
        case BAPE_TruSurroundHdAcmod_eCSII:         
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eModeCSII;
            break;
        default:
            BDBG_WRN(("TruSurround Input ACMode Invalid"));
            pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eINVALID;
            break;
        }

        pUserConfig->i32InputLFEPresentFlag = pSettings->inputLfeEnabled?1:0;
    }
    else if (numChannelPairs == 1) {
        pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode20;
        pUserConfig->i32InputLFEPresentFlag = 0;
    }
    else if (numChannelPairs == 3) {
        pUserConfig->eInputAcMode = BDSP_Raaga_Audio_AcMode_eMode32;
        pUserConfig->i32InputLFEPresentFlag = 1;
    }
    BDBG_MSG(("eInputAcMode: %u", pUserConfig->eInputAcMode));
    BDBG_MSG(("i32InputLFEPresentFlag: %u", pUserConfig->i32InputLFEPresentFlag));

    return BERR_SUCCESS;
}

static BERR_Code BAPE_TruSurroundHd_P_ApplyTruSurroundHdSettings(BAPE_TruSurroundHdHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
	BDSP_Raaga_Audio_TruSurrndHDConfigParams userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    userConfig.sTopLevelConfig.i32mEnable = false;  /* Disable studio sound */
    errCode = BAPE_TruSurroundHd_P_ConvertSettingsToDsp(&handle->settings, BAPE_FMT_P_GetNumChannelPairs(&handle->input->format), &userConfig);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_TruSurroundHd_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    /* Invalidate task handle */
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BSTD_UNUSED(pConnection);
    pNode->connectors[0].task = NULL;
}

static void BAPE_TruSurroundHd_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
	(void)BAPE_TruSurroundHd_RemoveInput(pNode->pHandle, pConnector);
}

