/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: bape_tru_volume.c $
* $brcm_Revision: Hydra_Software_Devel/16 $
* $brcm_Date: 7/31/12 3:10p $
*
* API Description:
*   API name: TruVolume
*    Specific APIs related to SRS TruVolume (formerly Volume IQ) Audio Processing
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_tru_volume.c $
* 
* Hydra_Software_Devel/16   7/31/12 3:10p jgarrett
* SW7425-3504: Normalizing TruVolume units to 1/100 scale and correcting
* Int to FP conversions
* 
* Hydra_Software_Devel/15   5/3/12 5:06p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/2   4/9/12 2:04p jgarrett
* SW7425-2075: Merging latest changes from main branch and enabling
* 4x/16x OP Types
* 
* Hydra_Software_Devel/14   3/30/12 2:48p jgarrett
* SW7435-71: Revising settings for TVOL 1.2.3.2
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:51p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
* Hydra_Software_Devel/13   3/2/12 10:25a jgarrett
* SWDTV-6761: Fixing reference level mapping
* 
* Hydra_Software_Devel/12   7/15/11 5:51p jgarrett
* SWDTV-6761: Adding StudioSound API
* 
* Hydra_Software_Devel/11   4/29/11 11:00a jgarrett
* SW7422-368: Switching to new GetDefaultSettings routines
* 
* Hydra_Software_Devel/10   4/18/11 10:09p jgarrett
* SW7425-361, SW7422-411: Refactoring DSP branch decisions.  Fixed user
* params for input/output/reference gains
* 
* Hydra_Software_Devel/9   4/16/11 12:32p jgarrett
* SW7425-371: Removing tabs
* 
* Hydra_Software_Devel/8   4/16/11 12:23p jgarrett
* SW7425-371: Removing tab characters
*                                                                                     
* Hydra_Software_Devel/7   4/15/11 4:29p piyushg
* SW7422-411: Disable SRS StudioSound processing for TruVolume.
* 
* Hydra_Software_Devel/6   4/14/11 7:18p piyushg
* SW7422-411: Map DSP settings between APE and the base module
* 
* Hydra_Software_Devel/5   3/11/11 12:20p jgarrett
* SW7422-146: Enabling PPM Correction
* 
* Hydra_Software_Devel/4   3/1/11 5:13p jgarrett
* SW7422-146: Fixing post-processing output modes
* 
* Hydra_Software_Devel/3   2/28/11 4:50p jgarrett
* SW7422-146: Adding TruVolume
* 
* Hydra_Software_Devel/2   2/22/11 5:44p jgarrett
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

BDBG_MODULE(bape_tru_volume);

BDBG_OBJECT_ID(BAPE_TruVolume);

typedef struct BAPE_TruVolume
{
    BDBG_OBJECT(BAPE_TruVolume)
    BAPE_PathNode node;
    BAPE_TruVolumeSettings settings;
    BAPE_Connector input;
} BAPE_TruVolume;

static BERR_Code BAPE_TruVolume_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_TruVolume_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_TruVolume_P_ApplyTruVolumeSettings(BAPE_TruVolumeHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId);
static void BAPE_TruVolume_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_TruVolume_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

/***************************************************************************
Summary:
    Get default settings for an SRS TruVolume stage
***************************************************************************/
void BAPE_TruVolume_GetDefaultSettings(
    BAPE_TruVolumeSettings *pSettings   /* [out] default settings */
    )
{
    BDSP_Raaga_Audio_TruVolumeUserConfig dspSettings;
    unsigned i, j;

    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BDSP_Raaga_GetDefaultAudioProcessingSettings(BDSP_AudioProcessing_eSrsTruVolume, 
                                                 (void *)&dspSettings, 
                                                 sizeof(BDSP_Raaga_Audio_TruVolumeUserConfig)
                                                 );

    pSettings->enabled = dspSettings.i32TruVolume_enable?true:false;

    switch (dspSettings.i32blockSize)
    {
    case 256:
        pSettings->blockSize = BAPE_TruVolumeBlockSize_e256;
        break;
    case 512:
        pSettings->blockSize = BAPE_TruVolumeBlockSize_e512;
        break;
    case 768:
        pSettings->blockSize = BAPE_TruVolumeBlockSize_e768;
        break;
    case 1024:
        pSettings->blockSize = BAPE_TruVolumeBlockSize_e1024;
        break;
    default:
        pSettings->blockSize = BAPE_TruVolumeBlockSize_eMax;
        break;
    }

    pSettings->enableNormalGain = dspSettings.i32mEnable?true:false;
    pSettings->inputGain = 100;
    BDBG_ASSERT(dspSettings.i32mInputGain == 0x00200000);
    BDBG_ASSERT(dspSettings.i32mInputGain == (int)BAPE_P_FloatToQ521(pSettings->inputGain, 3200));
    pSettings->outputGain = 100;
    BDBG_ASSERT(dspSettings.i32mOutputGain == 0x00200000);
    BDBG_ASSERT(dspSettings.i32mOutputGain == (int)BAPE_P_FloatToQ521(pSettings->outputGain, 3200));
    pSettings->bypassGain = 100;
    BDBG_ASSERT(0x007fffff == dspSettings.i32mBypassGain);
    pSettings->referenceLevel = 50;
    BDBG_ASSERT(0x00400000 == dspSettings.i32mReferenceLevel);


    switch (dspSettings.i32mMode)
    {
    case 0:
        pSettings->mode = BAPE_TruVolumeMode_eNormal;
        break;
    case 1:
        pSettings->mode = BAPE_TruVolumeMode_eLight;
        break;
    default:
        pSettings->mode = BAPE_TruVolumeMode_eMax;
        break;
    }

    switch (dspSettings.i32mSize)
    {
    case 0:
        pSettings->speakerResolution = BAPE_TruVolumeSpeakerResolution_e20Hz;
        break;
    case 1:
        pSettings->speakerResolution = BAPE_TruVolumeSpeakerResolution_e40Hz;
        break;
    case 2:
        pSettings->speakerResolution = BAPE_TruVolumeSpeakerResolution_e110Hz;
        break;
    case 3:
        pSettings->speakerResolution = BAPE_TruVolumeSpeakerResolution_e200Hz;
        break;
    case 4:
        pSettings->speakerResolution = BAPE_TruVolumeSpeakerResolution_e315Hz;
        break;
    case 5:
        pSettings->speakerResolution = BAPE_TruVolumeSpeakerResolution_e410Hz;
        break;
    default:
        pSettings->speakerResolution = BAPE_TruVolumeSpeakerResolution_eMax;
        break;
    }

    pSettings->maxGain = 1600;
    BDBG_ASSERT(0x00080000 == dspSettings.i32mMaxGain);
    pSettings->enableDcNotchFilter = dspSettings.i32EnableDCNotchFilter?true:false;
    pSettings->enableNoiseManager = dspSettings.i32mNoiseManager?true:false;
    pSettings->noiseManagerThreshold = 10;
    BDBG_ASSERT(0x000ccccd == dspSettings.i32mNoiseManagerThreshold);
    pSettings->enableNormalizer = dspSettings.i32mNormalizerEnable?true:false;
    pSettings->calibrate = 100;
    BDBG_ASSERT(0x8000 == dspSettings.i32mCalibrate);

    /* Top-level HPF settings */
    pSettings->highPassFilter.enabled = (dspSettings.sHighPassFilterConfig.ui32mEnable)?true:false;
    switch ( dspSettings.sHighPassFilterConfig.ui32CoefGenMode )
    {
    default:
        BDBG_WRN(("Unrecognized filter coefficient gen mode in default truvolume settings."));
        /* fall through */
    case 0:
        pSettings->highPassFilter.coefficientMode = BAPE_SrsFilterCoefficientMode_eUser;        
        break;
    case 1:
        pSettings->highPassFilter.coefficientMode = BAPE_SrsFilterCoefficientMode_eSpecification;
        break;
    }
    
    /* User Coefs */
    BDBG_CASSERT((sizeof(dspSettings.sHighPassFilterConfig.sFilterCoefHpf)/sizeof(BDSP_Raaga_FilterCoefHpf)) == 3);
    for ( i = 0; i < 3; i++ )
    {
        pSettings->highPassFilter.coefficientSettings.user[i].filterOrder = dspSettings.sHighPassFilterConfig.sFilterCoefHpf[i].ui32Order;
        BDBG_CASSERT((sizeof(dspSettings.sHighPassFilterConfig.sFilterCoefHpf[0].sFilterCoefHpfTdf2)/sizeof(BDSP_Raaga_FilterCoefHpfTdf2)) == 3);
        for ( j = 0; j < 3; j++ )
        {
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].scale = dspSettings.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32Scale;
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB0 = dspSettings.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB0;
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB1 = dspSettings.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB1;
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB2 = dspSettings.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB2;
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientA1 = dspSettings.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB1;
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientA2 = dspSettings.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientA2;
        }
    }

    /* Specification */
    pSettings->highPassFilter.coefficientSettings.specification.cutoffFrequency = dspSettings.sHighPassFilterConfig.sFilterSpecHpf.ui32CutoffFrequency;
    pSettings->highPassFilter.coefficientSettings.specification.filterOrder = dspSettings.sHighPassFilterConfig.sFilterSpecHpf.ui32Order;
}

/***************************************************************************
Summary:
    Open an SRS TruVolume stage
***************************************************************************/
BERR_Code BAPE_TruVolume_Create(
                               BAPE_Handle deviceHandle,
                               const BAPE_TruVolumeSettings *pSettings,
                               BAPE_TruVolumeHandle *pHandle
                               )
{
    BAPE_TruVolumeHandle handle;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);


    handle = BKNI_Malloc(sizeof(BAPE_TruVolume));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BAPE_TruVolume));
    BDBG_OBJECT_SET(handle, BAPE_TruVolume);
    handle->settings = *pSettings;
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_ePostProcessor, BAPE_PostProcessorType_eTruVolume, 1, deviceHandle, handle);
    handle->node.pName = "TruVolume";
    handle->node.connectors[0].useBufferPool = true;    

    BAPE_Connector_P_GetFormat(&handle->node.connectors[0], &format);
    format.source = BAPE_DataSource_eDspBuffer;
    format.type = BAPE_DataType_ePcmStereo;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[0], &format);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_format; }

    BAPE_PathNode_P_GetInputCapabilities(&handle->node, &caps);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDspBuffer);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmStereo);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->node, &caps);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_caps; }

    /* Generic Routines */
    handle->node.allocatePathToOutput = BAPE_DSP_P_AllocatePathToOutput;
    handle->node.configPathToOutput = BAPE_DSP_P_ConfigPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;
    handle->node.startPathToOutput = BAPE_DSP_P_StartPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;

    /* TruVolume Specifics */
    handle->node.allocatePathFromInput = BAPE_TruVolume_P_AllocatePathFromInput;
    handle->node.configPathFromInput = BAPE_TruVolume_P_ConfigPathFromInput;
    handle->node.stopPathFromInput = BAPE_TruVolume_P_StopPathFromInput;
    handle->node.removeInput = BAPE_TruVolume_P_RemoveInputCallback;

    *pHandle = handle;
    return BERR_SUCCESS;

err_caps:
err_format:
    BAPE_TruVolume_Destroy(handle);
    return errCode;
}


/***************************************************************************
Summary:
    Close an SRS TruVolume stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
void BAPE_TruVolume_Destroy(
                           BAPE_TruVolumeHandle handle
                           )
{
    bool running;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruVolume);
    running = (handle->node.connectors[0].task != NULL)?true:false;
    BDBG_ASSERT(false == running);
    BDBG_ASSERT(NULL == handle->input);
    BDBG_OBJECT_DESTROY(handle, BAPE_TruVolume);
    BKNI_Free(handle);
}

/***************************************************************************
Summary:
    Get Settings for an SRS TruVolume stage
***************************************************************************/
void BAPE_TruVolume_GetSettings(
                               BAPE_TruVolumeHandle handle,
                               BAPE_TruVolumeSettings *pSettings    /* [out] Settings */
                               )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_TruVolume);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}


/***************************************************************************
Summary:
    Set Settings for an SRS TruVolume stage
***************************************************************************/
BERR_Code BAPE_TruVolume_SetSettings(
                                    BAPE_TruVolumeHandle handle,
                                    const BAPE_TruVolumeSettings *pSettings
                                    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruVolume);
    BDBG_ASSERT(NULL != pSettings);

    /* Start time will always call this routine, don't copy the settings if we don't need to */
    if ( pSettings != &handle->settings )
    {
        handle->settings = *pSettings;
    }

    if ( handle->node.connectors[0].task != NULL )
    {
        errCode = BAPE_TruVolume_P_ApplyTruVolumeSettings(handle, 
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
    Get the audio connector for an SRS TruVolume stage
***************************************************************************/
void BAPE_TruVolume_GetConnector(
                                BAPE_TruVolumeHandle handle,
                                BAPE_Connector *pConnector
                                )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_TruVolume);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->node.connectors[0];
}


/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
BERR_Code BAPE_TruVolume_AddInput(
                                 BAPE_TruVolumeHandle handle,
                                 BAPE_Connector input
                                 )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruVolume);
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
BERR_Code BAPE_TruVolume_RemoveInput(
                                    BAPE_TruVolumeHandle handle,
                                    BAPE_Connector input
                                    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruVolume);
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
BERR_Code BAPE_TruVolume_RemoveAllInputs(
                                        BAPE_TruVolumeHandle handle
                                        )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_TruVolume);
    if ( handle->input )
    {
        return BAPE_TruVolume_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_TruVolume_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_TruVolumeHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->pTaskCreateSettings);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruVolume);
    /* Add stage to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input), 
                                            BDSP_AudioProcessing_eSrsTruVolume,
                                            false,
                                            &pNode->connectors[0].branchId, &pNode->connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pNode->connectors[0].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    return BERR_SUCCESS;
}

static BERR_Code BAPE_TruVolume_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_TruVolumeHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->task);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_TruVolume);
    pNode->connectors[0].task = pConnection->pSource->task;
    errCode = BAPE_TruVolume_P_ApplyTruVolumeSettings(handle, 
                                                      handle->node.connectors[0].task, 
                                                      handle->node.connectors[0].branchId, 
                                                      handle->node.connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;    
}

BERR_Code BAPE_TruVolume_P_ConvertSettingsToDsp(
    const BAPE_TruVolumeSettings *pSettings, 
    BDSP_Raaga_Audio_TruVolumeUserConfig *pUserConfig
    )
{
    unsigned i, j;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pUserConfig);

    BDBG_MSG(("BlockSize was %u", pUserConfig->i32blockSize));
    switch (pSettings->blockSize)
    {
    case BAPE_TruVolumeBlockSize_e256:
        pUserConfig->i32blockSize = 256;
        break;
    case BAPE_TruVolumeBlockSize_e512:
        pUserConfig->i32blockSize = 512;
        break;
    case BAPE_TruVolumeBlockSize_e768:
        pUserConfig->i32blockSize = 768;
        break;
    case BAPE_TruVolumeBlockSize_e1024:
        pUserConfig->i32blockSize = 1024;
        break;
    default:
        BDBG_MSG(("Incorrect value for TruVolume Blocksize. Possible values are 256, 512, 768, 1024"));
        break;
    }
    BDBG_MSG(("BlockSize now %u", pUserConfig->i32blockSize));
    BDBG_MSG(("i32EnableDCNotchFilter was %u", pUserConfig->i32EnableDCNotchFilter));
    pUserConfig->i32EnableDCNotchFilter = pSettings->enableDcNotchFilter?1:0;
    BDBG_MSG(("i32EnableDCNotchFilter now %u", pUserConfig->i32EnableDCNotchFilter));
    BDBG_MSG(("i32mBypassGain was %u", pUserConfig->i32mBypassGain));
    pUserConfig->i32mBypassGain = BAPE_P_FloatToQ923(pSettings->bypassGain, 100);
    BDBG_MSG(("i32mBypassGain now %u", pUserConfig->i32mBypassGain));
    BDBG_MSG(("i32mCalibrate was %u", pUserConfig->i32mCalibrate));
    pUserConfig->i32mCalibrate = BAPE_P_FloatToQ815(pSettings->calibrate, 25600);
    BDBG_MSG(("i32mCalibrate now %u", pUserConfig->i32mCalibrate));
    BDBG_MSG(("i32mEnable was %u", pUserConfig->i32mEnable));
    pUserConfig->i32mEnable = pSettings->enableNormalGain?1:0;
    BDBG_MSG(("i32mEnable now %u", pUserConfig->i32mEnable));
    BDBG_MSG(("i32mInputGain was %u", pUserConfig->i32mInputGain));
    pUserConfig->i32mInputGain = BAPE_P_FloatToQ521(pSettings->inputGain, 3200);
    BDBG_MSG(("i32mInputGain now %u", pUserConfig->i32mInputGain));
    BDBG_MSG(("i32mMaxGain was %u", pUserConfig->i32mMaxGain));
    pUserConfig->i32mMaxGain = BAPE_P_FloatToQ923(pSettings->maxGain, 25600);
    if ( pUserConfig->i32mMaxGain < 0x2000  )
    {
        pUserConfig->i32mMaxGain = 0x2000;
    }
    BDBG_MSG(("i32mMaxGain now %u", pUserConfig->i32mMaxGain));
    BDBG_MSG(("i32mMode was %u", pUserConfig->i32mMode));
    switch (pSettings->mode)
    {
    case BAPE_TruVolumeMode_eLight:
        pUserConfig->i32mMode = 1;
        break;
    case BAPE_TruVolumeMode_eNormal:
        pUserConfig->i32mMode = 0;
        break;
    case BAPE_TruVolumeMode_eHeavy:
        BDBG_ERR(("Heavy TruVolume mode is not supported in this SRS version"));
        pUserConfig->i32mMode = 2;
        break;
    default:
        BDBG_WRN(("Incorrect value for TruVolume Mode. Possible values are 0, 1"));
        break;
    }
    BDBG_MSG(("i32mMode now %u", pUserConfig->i32mMode));
    BDBG_MSG(("i32mNoiseManager was %u", pUserConfig->i32mNoiseManager));
    pUserConfig->i32mNoiseManager = pSettings->enableNoiseManager?1:0;
    BDBG_MSG(("i32mNoiseManager now %u", pUserConfig->i32mNoiseManager));
    BDBG_MSG(("i32mNoiseManagerThreshold was %u", pUserConfig->i32mNoiseManagerThreshold));
    pUserConfig->i32mNoiseManagerThreshold = BAPE_P_FloatToQ923(pSettings->noiseManagerThreshold, 100);
    BDBG_MSG(("i32mNoiseManagerThreshold now %u", pUserConfig->i32mNoiseManagerThreshold));
    BDBG_MSG(("i32mNormalizerEnable was %u", pUserConfig->i32mNormalizerEnable));
    pUserConfig->i32mNormalizerEnable = pSettings->enableNormalizer?1:0;
    BDBG_MSG(("i32mNormalizerEnable now %u", pUserConfig->i32mNormalizerEnable));
    BDBG_MSG(("i32mOutputGain was %u", pUserConfig->i32mOutputGain));
    pUserConfig->i32mOutputGain = BAPE_P_FloatToQ521(pSettings->outputGain, 3200);
    BDBG_MSG(("i32mOutputGain now %u", pUserConfig->i32mOutputGain));
    BDBG_MSG(("i32mReferenceLevel was %u", pUserConfig->i32mReferenceLevel));
    pUserConfig->i32mReferenceLevel = BAPE_P_FloatToQ923(pSettings->referenceLevel, 100);
    if ( pUserConfig->i32mReferenceLevel < 0x109 )
    {
        pUserConfig->i32mReferenceLevel = 0x109;
    }
    BDBG_MSG(("i32mReferenceLevel now %u", pUserConfig->i32mReferenceLevel));
    BDBG_MSG(("i32mSize was %u", pUserConfig->i32mSize));
    switch (pSettings->speakerResolution)
    {
    case BAPE_TruVolumeSpeakerResolution_e20Hz:
        pUserConfig->i32mSize = 0;
        break;
    case BAPE_TruVolumeSpeakerResolution_e40Hz:
        pUserConfig->i32mSize = 1;
        break;
    case BAPE_TruVolumeSpeakerResolution_e110Hz:
        pUserConfig->i32mSize = 2;
        break;
    case BAPE_TruVolumeSpeakerResolution_e200Hz:
        pUserConfig->i32mSize = 3;
        break;
    case BAPE_TruVolumeSpeakerResolution_e315Hz:
        pUserConfig->i32mSize = 4;
        break;
    case BAPE_TruVolumeSpeakerResolution_e410Hz:
        pUserConfig->i32mSize = 5;
        break;
    default:
        BDBG_WRN(("Incorrect value for TruVolume Resolution. Possible values are 0, 1, 2, 3, 4, 5"));
        break;
    }
    BDBG_MSG(("i32mSize now %u", pUserConfig->i32mSize));
    BDBG_MSG(("i32nchans was %u", pUserConfig->i32nchans));
    pUserConfig->i32nchans = 2;
    BDBG_MSG(("i32nchans now %u", pUserConfig->i32nchans));
    BDBG_MSG(("i32TruVolume_enable was %u", pUserConfig->i32TruVolume_enable));
    pUserConfig->i32TruVolume_enable = pSettings->enabled?1:0;
    BDBG_MSG(("i32TruVolume_enable now %u", pUserConfig->i32TruVolume_enable));

    /* Top-level HPF settings */
    pUserConfig->sHighPassFilterConfig.ui32mEnable = (pSettings->highPassFilter.enabled)?true:false;
    switch ( pSettings->highPassFilter.coefficientMode )
    {
    default:
        BDBG_WRN(("Unrecognized filter coefficient gen mode in truvolume settings."));
        /* fall through */
    case BAPE_SrsFilterCoefficientMode_eUser:
        pUserConfig->sHighPassFilterConfig.ui32CoefGenMode = 0;
        break;
    case BAPE_SrsFilterCoefficientMode_eSpecification:
        pUserConfig->sHighPassFilterConfig.ui32CoefGenMode = 1;
        break;
    }

    /* User Coefs */
    BDBG_CASSERT((sizeof(pUserConfig->sHighPassFilterConfig.sFilterCoefHpf)/sizeof(BDSP_Raaga_FilterCoefHpf)) == 3);
    for ( i = 0; i < 3; i++ )
    {
        pUserConfig->sHighPassFilterConfig.sFilterCoefHpf[i].ui32Order = pSettings->highPassFilter.coefficientSettings.user[i].filterOrder;
        BDBG_CASSERT((sizeof(pUserConfig->sHighPassFilterConfig.sFilterCoefHpf[0].sFilterCoefHpfTdf2)/sizeof(BDSP_Raaga_FilterCoefHpfTdf2)) == 3);
        for ( j = 0; j < 3; j++ )
        {
            pUserConfig->sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32Scale = pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].scale;
            pUserConfig->sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB0 = pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB0;
            pUserConfig->sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB1 = pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB1;
            pUserConfig->sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB2 = pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB2;
            pUserConfig->sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB1 = pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientA1;
            pUserConfig->sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientA2 = pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientA2;
        }
    }

    /* Specification */
    pUserConfig->sHighPassFilterConfig.sFilterSpecHpf.ui32CutoffFrequency = pSettings->highPassFilter.coefficientSettings.specification.cutoffFrequency;
    pUserConfig->sHighPassFilterConfig.sFilterSpecHpf.ui32Order = pSettings->highPassFilter.coefficientSettings.specification.filterOrder;
    
    return BERR_SUCCESS;    
}

static BERR_Code BAPE_TruVolume_P_ApplyTruVolumeSettings(BAPE_TruVolumeHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_TruVolumeUserConfig userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    userConfig.sTopLevelConfig.i32mEnable = false;  /* Disable studio sound */
    errCode = BAPE_TruVolume_P_ConvertSettingsToDsp(&handle->settings, &userConfig);
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

static void BAPE_TruVolume_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    /* Invalidate task handle */
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BSTD_UNUSED(pConnection);
    pNode->connectors[0].task = NULL;
}

static void BAPE_TruVolume_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_TruVolume_RemoveInput(pNode->pHandle, pConnector);
}


