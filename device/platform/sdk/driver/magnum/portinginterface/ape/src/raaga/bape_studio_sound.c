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
* $brcm_Workfile: bape_studio_sound.c $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 5/3/12 5:06p $
*
* API Description:
*   API name: StudioSound
*    Specific APIs related to SRS StudioSound Audio Processing
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7422/bape_studio_sound.c $
* 
* Hydra_Software_Devel/4   5/3/12 5:06p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:53p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
* Hydra_Software_Devel/3   8/9/11 10:00a jgarrett
* SWDTV-6761: Coverity CID 587,588
* 
* Hydra_Software_Devel/2   8/8/11 5:38p jgarrett
* SWDTV-6761: Adding StudioSound
* 
***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_studio_sound);

BDBG_OBJECT_ID(BAPE_StudioSound);

typedef struct BAPE_StudioSound
{
    BDBG_OBJECT(BAPE_StudioSound)
    BAPE_PathNode node;
    BAPE_StudioSoundSettings settings;
    BAPE_Connector input;
    unsigned branchId;
    struct
    {
        unsigned tvol;  /* TruVolume Stage */
        unsigned cstd;  /* CircleSurround/TruDialog Stage */
        unsigned tshd;  /* TruSurroundHd Stage */
        unsigned eqhl;  /* Equalizer/Hard Limiter Stage */
    } stageId;    
} BAPE_StudioSound;

static BERR_Code BAPE_StudioSound_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_StudioSound_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_StudioSound_P_ApplyDspSettings(BAPE_StudioSoundHandle handle, BDSP_TaskHandle task);
static void BAPE_StudioSound_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_StudioSound_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

static void BAPE_StudioSound_P_GetDefaultTopLevelSettings(
    BAPE_StudioSoundSettings *pSettings
    )
{
    BDSP_Raaga_SrsTruVolumeSettings userConfig;
    
    BDBG_ASSERT(NULL != pSettings);
    
    (void)BDSP_Raaga_GetDefaultAudioProcessingSettings(BDSP_AudioProcessing_eSrsTruVolume, &userConfig, sizeof(userConfig));
    pSettings->enabled = userConfig.sTopLevelConfig.i32mEnable?true:false;
    pSettings->mode = userConfig.sTopLevelConfig.i32StudioSoundMode == 2?BAPE_StudioSoundMode_eMode2:BAPE_StudioSoundMode_eMode1;
    pSettings->inputGain = 100;
    /*BDBG_ASSERT(userConfig.sTopLevelConfig.i32mInputGain == 0x7fffffff);*/ /* Default is incorrect */
    pSettings->outputGain = 100;
    /*BDBG_ASSERT(userConfig.sTopLevelConfig.i32mOutputGain == 0x7fffffff);*/
    pSettings->bypassGain = 100;
    /*BDBG_ASSERT(userConfig.sTopLevelConfig.i32mBypassGain == 0x7fffffff);*/
    pSettings->headroomGain = 100;
    /*BDBG_ASSERT(userConfig.sTopLevelConfig.i32mHeadroomGain == 0x7fffffff);*/
    switch ( userConfig.sTopLevelConfig.i32mInputMode )
    {
    case 0:
        pSettings->inputMode = BAPE_StudioSoundInputMode_eMono;
        break;
    case 1:
        pSettings->inputMode = BAPE_StudioSoundInputMode_eStereo;
        break;
    case 2:
        pSettings->inputMode = BAPE_StudioSoundInputMode_eMultichannel;
        break;
    default:
    case 3:
        pSettings->inputMode = BAPE_StudioSoundInputMode_eLtRt;
        break;
    }
}

static void BAPE_StudioSound_P_GetDefaultCsTdSettings(
    BAPE_StudioSoundSettings *pSettings
    )
{
    BDSP_Raaga_SrsCsdTdSettings userConfig;
    
    BDBG_ASSERT(NULL != pSettings);
    
    (void)BDSP_Raaga_GetDefaultAudioProcessingSettings(BDSP_AudioProcessing_eSrsCsdTd, &userConfig, sizeof(userConfig));
    pSettings->circleSurround.enabled = userConfig.sCSDecoderConfig.i32mEnable?true:false;
    pSettings->circleSurround.inputGain = 1000;
    BDBG_ASSERT(userConfig.sCSDecoderConfig.i32mInputGain == 0x7fffffff);
    pSettings->circleSurround.mode = (userConfig.sCSDecoderConfig.i32mMode == 1)?BAPE_CircleSurroundMode_eCinema:BAPE_CircleSurroundMode_eMusic;
    pSettings->circleSurround.outputMode = userConfig.sCSDecoderConfig.i32mOutputMode;
    pSettings->circleSurround.outputGainFront = 1000;
    BDBG_ASSERT(userConfig.sCSDecoderConfig.i32mCSDecOutputGainLR == 0x10000000);
    pSettings->circleSurround.outputGainRear = 1000;
    BDBG_ASSERT(userConfig.sCSDecoderConfig.i32mCSDecOutputGainLsRs == 0x10000000);
    pSettings->circleSurround.outputGainCenter = 1000;
    BDBG_ASSERT(userConfig.sCSDecoderConfig.i32mCSDecOutputGainC == 0x10000000);
    pSettings->circleSurround.outputGainSubwoofer = 1000;
    BDBG_ASSERT(userConfig.sCSDecoderConfig.i32mCSDecOutputGainSub == 0x10000000);
    
    pSettings->truDialog.enabled = userConfig.sTruDialogConfig.i32mEnable?true:false;
    pSettings->truDialog.inputGain = 100;
    BDBG_ASSERT(userConfig.sTruDialogConfig.i32mTruDialogInputGain == 0x7fffffff);
    pSettings->truDialog.outputGain = 100;
    BDBG_ASSERT(userConfig.sTruDialogConfig.i32mTruDialogOutputGain == 0x7fffffff);
    pSettings->truDialog.processGain = 150;
    BDBG_ASSERT(userConfig.sTruDialogConfig.i32mTruDialogProcessGain == 0x0c000000);
    pSettings->truDialog.bypassGain = 100;
    BDBG_ASSERT(userConfig.sTruDialogConfig.i32mTruDialogBypassGain == 0x7fffffff);
    pSettings->truDialog.enhancementGain = 100;    
    BDBG_ASSERT(userConfig.sTruDialogConfig.i32mTruDialogDialogClarityGain == 0x7fffffff);
}        

static void BAPE_StudioSound_P_GetDefaultEqHlSettings(
    BAPE_StudioSoundSettings *pSettings
    )
{
    unsigned i, j;
    static BDSP_Raaga_SrsEqHlSettings userConfig;   /* This structure is > 1k.  Magnum is not thread safe so this is acceptable */
    
    BDBG_ASSERT(NULL != pSettings);
    
    (void)BDSP_Raaga_GetDefaultAudioProcessingSettings(BDSP_AudioProcessing_eSrsEqHl, &userConfig, sizeof(userConfig));
    
    /* GEQ */
    pSettings->graphicEq.leftEnabled = userConfig.sGraphicEqConfig.i32mEnable[0]?true:false;
    pSettings->graphicEq.rightEnabled = userConfig.sGraphicEqConfig.i32mEnable[1]?true:false;
    switch ( userConfig.sGraphicEqConfig.i32mFilterMode )
    {
    default:
    case 0:
        pSettings->graphicEq.bandMode = BAPE_SrsGraphicEqBandMode_eFiveBand;    
        break;
    case 1:
        pSettings->graphicEq.bandMode = BAPE_SrsGraphicEqBandMode_eTenBand;    
        break;
    case 2:
        pSettings->graphicEq.bandMode = BAPE_SrsGraphicEqBandMode_eSpeakerCompensation;    
        break;
    }
    BDBG_CASSERT((sizeof(userConfig.sGraphicEqConfig.i32mBandGain)/(2*sizeof(int32_t))) == BAPE_STUDIO_SOUND_MAX_GEQ_BANDS);
    for ( i = 0; i < BAPE_STUDIO_SOUND_MAX_GEQ_BANDS; i++ )
    {
        pSettings->graphicEq.leftBandGain[i] = 1000;
        BDBG_ASSERT(userConfig.sGraphicEqConfig.i32mBandGain[0][i] == 0x20000000);
        pSettings->graphicEq.rightBandGain[i] = 1000;
        BDBG_ASSERT(userConfig.sGraphicEqConfig.i32mBandGain[1][i] == 0x20000000);
    }    
    pSettings->graphicEq.inputGain = 316;
    /*BDBG_ASSERT(userConfig.sGraphicEqConfig.i32mInputGain == 0x2872b021);   This does not appear to be correct in the comments vs. structure init */
    pSettings->graphicEq.outputGain = 100;
    BDBG_ASSERT(userConfig.sGraphicEqConfig.i32mOutputGain == 0x7fffffff);
    pSettings->graphicEq.bypassGain = 100;
    BDBG_ASSERT(userConfig.sGraphicEqConfig.i32mBypassGain == 0x7fffffff);
    
    /* TruEq */
    pSettings->truEq.leftEnabled = userConfig.sParametricEqConfig.i32mEnable[0];
    pSettings->truEq.rightEnabled = userConfig.sParametricEqConfig.i32mEnable[1];
    BDBG_CASSERT((sizeof(userConfig.sParametricEqConfig.i32mBandEnable)/(2*sizeof(int32_t))) == BAPE_STUDIO_SOUND_MAX_PEQ_BANDS);
    for ( i = 0; i < BAPE_STUDIO_SOUND_MAX_PEQ_BANDS; i++ )
    {
        pSettings->truEq.leftBandEnabled[i] = userConfig.sParametricEqConfig.i32mBandEnable[0][i]?true:false;
        pSettings->truEq.rightBandEnabled[i] = userConfig.sParametricEqConfig.i32mBandEnable[1][i]?true:false;
    }
    pSettings->truEq.inputGain = 100;
    BDBG_ASSERT(userConfig.sParametricEqConfig.i32mInputGain == 0x08000000);
    pSettings->truEq.outputGain = 100;
    BDBG_ASSERT(userConfig.sParametricEqConfig.i32mOutputGain == 0x08000000);
    pSettings->truEq.bypassGain = 100;
    BDBG_ASSERT(userConfig.sParametricEqConfig.i32mBypassGain == 0x08000000);
    pSettings->truEq.coefficientMode = userConfig.sParametricEqConfig.i32CoefGenMode == 1 ? BAPE_SrsFilterCoefficientMode_eSpecification : BAPE_SrsFilterCoefficientMode_eUser;
    for ( i = 0; i < 3; i++ )
    {
        for ( j = 0; j < BAPE_STUDIO_SOUND_MAX_PEQ_BANDS; j++ )
        {
            pSettings->truEq.coefficientSettings.user[i][j].filterOrder = userConfig.sParametricEqConfig.sFilterCoefPeq[i][j].i32Order; 
            pSettings->truEq.coefficientSettings.user[i][j].scale = userConfig.sParametricEqConfig.sFilterCoefPeq[i][j].i32Scale;
            pSettings->truEq.coefficientSettings.user[i][j].coefficientB0 = userConfig.sParametricEqConfig.sFilterCoefPeq[i][j].i32FilterCoefficientB0;
            pSettings->truEq.coefficientSettings.user[i][j].coefficientA1 = userConfig.sParametricEqConfig.sFilterCoefPeq[i][j].i32FilterCoefficientA1;
            pSettings->truEq.coefficientSettings.user[i][j].coefficientB1 = userConfig.sParametricEqConfig.sFilterCoefPeq[i][j].i32FilterCoefficientB1;
            pSettings->truEq.coefficientSettings.user[i][j].coefficientA2 = userConfig.sParametricEqConfig.sFilterCoefPeq[i][j].i32FilterCoefficientA2;
            pSettings->truEq.coefficientSettings.user[i][j].coefficientB2 = userConfig.sParametricEqConfig.sFilterCoefPeq[i][j].i32FilterCoefficientB2;
        }        
    }
    for ( i = 0; i < BAPE_STUDIO_SOUND_MAX_PEQ_BANDS; i++ )
    {
        pSettings->truEq.coefficientSettings.specification[i].bandGain = userConfig.sParametricEqConfig.sFilterSpecPeq[i].i32BandGain;
        pSettings->truEq.coefficientSettings.specification[i].bandFrequency = userConfig.sParametricEqConfig.sFilterSpecPeq[i].ui32BandFrequency;
        pSettings->truEq.coefficientSettings.specification[i].qFactor = userConfig.sParametricEqConfig.sFilterSpecPeq[i].i32QFactor;
    }        

    /* HPF */
    pSettings->highPassFilter.enabled = userConfig.sHighPassFilterConfig.ui32mEnable?true:false;
    pSettings->highPassFilter.coefficientMode = userConfig.sHighPassFilterConfig.ui32CoefGenMode == 1 ? BAPE_SrsFilterCoefficientMode_eSpecification : BAPE_SrsFilterCoefficientMode_eUser;
    for ( i = 0; i < 3; i++ )
    {
        pSettings->highPassFilter.coefficientSettings.user[i].filterOrder = userConfig.sHighPassFilterConfig.sFilterCoefHpf[i].ui32Order;
        for ( j = 0; j < 3; j++ )
        {
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].scale = userConfig.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32Scale;
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB0 = userConfig.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB0;
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB1 = userConfig.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB1;
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB2 = userConfig.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB2;
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientA1 = userConfig.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientA1;
            pSettings->highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientA2 = userConfig.sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientA2;
        }
    }
    pSettings->highPassFilter.coefficientSettings.specification.cutoffFrequency = userConfig.sHighPassFilterConfig.sFilterSpecHpf.ui32CutoffFrequency; 
    pSettings->highPassFilter.coefficientSettings.specification.filterOrder = userConfig.sHighPassFilterConfig.sFilterSpecHpf.ui32Order; 
    
    /* Hard Limiter */
    pSettings->hardLimiter.enabled = userConfig.sHardLimiterConfig.i32LimiterEnable?true:false;
    switch ( userConfig.sHardLimiterConfig.i32blockSize )
    {
    default:
    case 256:
        pSettings->hardLimiter.blockSize = BAPE_SrsHardLimiterBlockSize_e256;    
        break;
    case 512:
        pSettings->hardLimiter.blockSize = BAPE_SrsHardLimiterBlockSize_e512;    
        break;
    case 768:
        pSettings->hardLimiter.blockSize = BAPE_SrsHardLimiterBlockSize_e768;    
        break;
    case 1024:
        pSettings->hardLimiter.blockSize = BAPE_SrsHardLimiterBlockSize_e1024;    
        break;
    }
    pSettings->hardLimiter.inputGain = 100;
    /*BDBG_ASSERT(userConfig.sHardLimiterConfig.i32InputGain == 0x20000000);*/
    pSettings->hardLimiter.outputGain = 100;
    /*BDBG_ASSERT(userConfig.sHardLimiterConfig.i32OutputGain == 0x20000000);*/
    pSettings->hardLimiter.bypassGain = 100;
    BDBG_ASSERT(userConfig.sHardLimiterConfig.i32BypassGain == 0x7fffffff);
    pSettings->hardLimiter.boost = 2820;
    BDBG_ASSERT(userConfig.sHardLimiterConfig.i32LimiterBoost == 0x0168f5c3);
    pSettings->hardLimiter.level = 100000;
    BDBG_ASSERT(userConfig.sHardLimiterConfig.i32HardLimit == 0x7fffffff);
    pSettings->hardLimiter.delay = userConfig.sHardLimiterConfig.i32DelayLength;    
}

void BAPE_StudioSound_GetDefaultSettings(
    BAPE_StudioSoundSettings *pSettings   /* [out] default settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(BAPE_StudioSoundSettings));
    
    BAPE_StudioSound_P_GetDefaultTopLevelSettings(pSettings);
    BAPE_TruVolume_GetDefaultSettings(&pSettings->truVolume);
    BAPE_TruSurroundHd_GetDefaultSettings(&pSettings->truSurroundHd);
    BAPE_StudioSound_P_GetDefaultCsTdSettings(pSettings);
    BAPE_StudioSound_P_GetDefaultEqHlSettings(pSettings);    
}

BERR_Code BAPE_StudioSound_Create(
    BAPE_Handle deviceHandle,
    const BAPE_StudioSoundSettings *pSettings,
    BAPE_StudioSoundHandle *pHandle
    )
{
    BAPE_StudioSoundHandle handle;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);


    handle = BKNI_Malloc(sizeof(BAPE_StudioSound));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BAPE_StudioSound));
    BDBG_OBJECT_SET(handle, BAPE_StudioSound);
    handle->settings = *pSettings;
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_ePostProcessor, BAPE_PostProcessorType_eStudioSound, 1, deviceHandle, handle);
    handle->node.pName = "StudioSound";

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

    /* StudioSound Specifics */
    handle->node.allocatePathFromInput = BAPE_StudioSound_P_AllocatePathFromInput;
    handle->node.configPathFromInput = BAPE_StudioSound_P_ConfigPathFromInput;
    handle->node.stopPathFromInput = BAPE_StudioSound_P_StopPathFromInput;
    handle->node.removeInput = BAPE_StudioSound_P_RemoveInputCallback;

    *pHandle = handle;
    return BERR_SUCCESS;

err_caps:
err_format:
    BAPE_StudioSound_Destroy(handle);
    return errCode;
}

void BAPE_StudioSound_Destroy(
    BAPE_StudioSoundHandle handle
    )
{
    bool running;
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    running = (handle->node.connectors[0].task != NULL)?true:false;
    BDBG_ASSERT(false == running);
    BDBG_ASSERT(NULL == handle->input);
    BDBG_OBJECT_DESTROY(handle, BAPE_StudioSound);
    BKNI_Free(handle);
}

void BAPE_StudioSound_GetSettings(
    BAPE_StudioSoundHandle handle,
    BAPE_StudioSoundSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

BERR_Code BAPE_StudioSound_SetSettings(
    BAPE_StudioSoundHandle handle,
    const BAPE_StudioSoundSettings *pSettings
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    BDBG_ASSERT(NULL != pSettings);

    if ( pSettings != &handle->settings )
    {
        handle->settings = *pSettings;
    }

    if ( handle->node.connectors[0].task != NULL )
    {
        errCode = BAPE_StudioSound_P_ApplyDspSettings(handle, handle->node.connectors[0].task);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}


void BAPE_StudioSound_GetConnector(
    BAPE_StudioSoundHandle handle,
    BAPE_Connector *pConnector
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->node.connectors[0];
}


BERR_Code BAPE_StudioSound_AddInput(
    BAPE_StudioSoundHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
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


BERR_Code BAPE_StudioSound_RemoveInput(
    BAPE_StudioSoundHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
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

BERR_Code BAPE_StudioSound_RemoveAllInputs(
    BAPE_StudioSoundHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    if ( handle->input )
    {
        return BAPE_StudioSound_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_StudioSound_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_StudioSoundHandle handle;
    unsigned branchId;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->pTaskCreateSettings);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    /* Add stages to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input), 
                                            BDSP_AudioProcessing_eSrsTruVolume,
                                            false,
                                            &branchId, &handle->stageId.tvol);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->branchId = branchId;
    
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, handle->branchId, handle->stageId.tvol, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input), 
                                            BDSP_AudioProcessing_eSrsCsdTd,
                                            false,
                                            &branchId, &handle->stageId.cstd);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    if ( branchId != handle->branchId )
    {
        BDBG_ERR(("Unable to add StudioSound to the current processing branch."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, handle->branchId, handle->stageId.cstd, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input), 
                                            BDSP_AudioProcessing_eSrsHd,
                                            false,
                                            &branchId, &handle->stageId.tshd);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    if ( branchId != handle->branchId )
    {
        BDBG_ERR(("Unable to add StudioSound to the current processing branch."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, handle->branchId, handle->stageId.tshd, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input), 
                                            BDSP_AudioProcessing_eSrsEqHl,
                                            false,
                                            &branchId, &handle->stageId.eqhl);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    if ( branchId != handle->branchId )
    {
        BDBG_ERR(("Unable to add StudioSound to the current processing branch."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }    

    pNode->connectors[0].branchId = branchId;
    pNode->connectors[0].stageId = handle->stageId.eqhl;        
    pNode->connectors[0].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    return BERR_SUCCESS;
}

static BERR_Code BAPE_StudioSound_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_StudioSoundHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->task);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    pNode->connectors[0].task = pConnection->pSource->task;
    errCode = BAPE_StudioSound_P_ApplyDspSettings(handle, handle->node.connectors[0].task);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;    
}

static BERR_Code BAPE_StudioSound_P_ConvertTopLevelSettings(const BAPE_StudioSoundSettings *pSettings, BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig *pUserConfig)
{
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pUserConfig);
    
    pUserConfig->i32IsStudioSound = true;
    BDBG_MSG(("Top Level: i32IsStudioSound = true"));
    pUserConfig->i32StudioSoundMode = (pSettings->mode == BAPE_StudioSoundMode_eMode2) ? 2 : 1;
    BDBG_MSG(("Top Level: i32StudioSoundMode = %u", pUserConfig->i32StudioSoundMode));
    pUserConfig->i32mEnable = pSettings->enabled ? 1 : 0;
    BDBG_MSG(("Top Level: i32mEnable = %u", pUserConfig->i32mEnable));
    pUserConfig->i32mInputGain = BAPE_P_FloatToQ131(pSettings->inputGain, 100);
    BDBG_MSG(("Top Level: i32mInputGain = %#x", pUserConfig->i32mInputGain));
    pUserConfig->i32mHeadroomGain = BAPE_P_FloatToQ131(pSettings->headroomGain, 100);
    BDBG_MSG(("Top Level: i32mHeadroomGain = %#x", pUserConfig->i32mHeadroomGain));
    switch ( pSettings->inputMode )
    {
    case BAPE_StudioSoundInputMode_eMono:
        pUserConfig->i32mInputMode = 0;
        break;
    case BAPE_StudioSoundInputMode_eStereo:
        pUserConfig->i32mInputMode = 1;
        break;
    case BAPE_StudioSoundInputMode_eMultichannel:
        pUserConfig->i32mInputMode = 2;
        break;
    default:
    case BAPE_StudioSoundInputMode_eLtRt:
        pUserConfig->i32mInputMode = 3;
        break;
    }
    BDBG_MSG(("Top Level: i32mInputMode = %u", pUserConfig->i32mInputMode));
    pUserConfig->i32mOutputGain = BAPE_P_FloatToQ131(pSettings->outputGain, 100);
    BDBG_MSG(("Top Level: i32mOutputGain = %#x", pUserConfig->i32mOutputGain));
    pUserConfig->i32mBypassGain = BAPE_P_FloatToQ131(pSettings->bypassGain, 100);
    BDBG_MSG(("Top Level: i32mBypassGain = %#x", pUserConfig->i32mBypassGain));        
    
    return BERR_SUCCESS;
}

static BERR_Code BDSP_StudioSound_P_ApplyCsTdSettings(
    BAPE_StudioSoundHandle handle,
    BDSP_TaskHandle task,
    const BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig *pTopLevelConfig
    )
{
    BERR_Code errCode;
    BDSP_Raaga_SrsCsdTdSettings userConfig;
    
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    BDBG_ASSERT(NULL != task);
    BDBG_ASSERT(NULL != pTopLevelConfig);
    
    errCode = BDSP_Task_GetStageSettings(task, handle->branchId, handle->stageId.cstd, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    BKNI_Memcpy(&userConfig.sTopLevelConfig, pTopLevelConfig, sizeof(*pTopLevelConfig));
    
    BAPE_DSP_P_SET_VARIABLE(userConfig.sCSDecoderConfig, i32mEnable, handle->settings.circleSurround.enabled ? 1 : 0);
    BAPE_DSP_P_SET_VARIABLE(userConfig.sCSDecoderConfig, i32mInputGain, BAPE_P_FloatToQ131(handle->settings.circleSurround.inputGain, 1000));
    BAPE_DSP_P_SET_VARIABLE(userConfig.sCSDecoderConfig, i32mMode, (handle->settings.circleSurround.mode == BAPE_CircleSurroundMode_eCinema) ? 1 : 0);
    BAPE_DSP_P_SET_VARIABLE(userConfig.sCSDecoderConfig, i32mCSDecOutputGainLR, BAPE_P_FloatToQ428(handle->settings.circleSurround.outputGainFront, 1000));
    BAPE_DSP_P_SET_VARIABLE(userConfig.sCSDecoderConfig, i32mCSDecOutputGainLsRs, BAPE_P_FloatToQ428(handle->settings.circleSurround.outputGainRear, 1000));
    BAPE_DSP_P_SET_VARIABLE(userConfig.sCSDecoderConfig, i32mCSDecOutputGainC, BAPE_P_FloatToQ428(handle->settings.circleSurround.outputGainCenter, 1000));
    BAPE_DSP_P_SET_VARIABLE(userConfig.sCSDecoderConfig, i32mCSDecOutputGainSub, BAPE_P_FloatToQ428(handle->settings.circleSurround.outputGainSubwoofer, 1000));
    
    BAPE_DSP_P_SET_VARIABLE(userConfig.sTruDialogConfig, i32mEnable, handle->settings.truDialog.enabled ? 1 : 0);
    BAPE_DSP_P_SET_VARIABLE(userConfig.sTruDialogConfig, i32mTruDialogInputGain, BAPE_P_FloatToQ131(handle->settings.truDialog.inputGain, 100));
    BAPE_DSP_P_SET_VARIABLE(userConfig.sTruDialogConfig, i32mTruDialogOutputGain, BAPE_P_FloatToQ131(handle->settings.truDialog.outputGain, 100));
    BAPE_DSP_P_SET_VARIABLE(userConfig.sTruDialogConfig, i32mTruDialogProcessGain, BAPE_P_FloatToQ527(handle->settings.truDialog.processGain, 300));
    BAPE_DSP_P_SET_VARIABLE(userConfig.sTruDialogConfig, i32mTruDialogBypassGain, BAPE_P_FloatToQ131(handle->settings.truDialog.bypassGain, 100));
    BAPE_DSP_P_SET_VARIABLE(userConfig.sTruDialogConfig, i32mTruDialogDialogClarityGain, BAPE_P_FloatToQ131(handle->settings.truDialog.enhancementGain, 100));
        
    errCode = BDSP_Task_SetStageSettings(task, handle->branchId, handle->stageId.cstd, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}        

static BERR_Code BDSP_StudioSound_P_ApplyTruSurroundSettings(
    BAPE_StudioSoundHandle handle,
    BDSP_TaskHandle task,
    const BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig *pTopLevelConfig
    )
{
    BERR_Code errCode;
    BDSP_Raaga_SrsHdSettings userConfig;
    
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    BDBG_ASSERT(NULL != task);
    BDBG_ASSERT(NULL != pTopLevelConfig);
    
    errCode = BDSP_Task_GetStageSettings(task, handle->branchId, handle->stageId.tshd, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    BKNI_Memcpy(&userConfig.sTopLevelConfig, pTopLevelConfig, sizeof(*pTopLevelConfig));
    BAPE_TruSurroundHd_P_ConvertSettingsToDsp(&handle->settings.truSurroundHd, 1, &userConfig);
    
    errCode = BDSP_Task_SetStageSettings(task, handle->branchId, handle->stageId.tshd, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}        

static BERR_Code BDSP_StudioSound_P_ApplyEqHlSettings(
    BAPE_StudioSoundHandle handle,
    BDSP_TaskHandle task,
    const BDSP_Raaga_Audio_StudioSoundTopLevelUserConfig *pTopLevelConfig
    )
{
    BERR_Code errCode;
    static BDSP_Raaga_SrsEqHlSettings userConfig;   /* This structure is > 1k.  Magnum is not thread safe so this is acceptable */
    unsigned i, j;
    
    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    BDBG_ASSERT(NULL != task);
    BDBG_ASSERT(NULL != pTopLevelConfig);
    
    errCode = BDSP_Task_GetStageSettings(task, handle->branchId, handle->stageId.eqhl, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    BKNI_Memcpy(&userConfig.sTopLevelConfig, pTopLevelConfig, sizeof(*pTopLevelConfig));

    /* GEQ */
    BAPE_DSP_P_SET_VARIABLE(userConfig.sGraphicEqConfig, i32mEnable[0], handle->settings.graphicEq.leftEnabled ? 1 : 0);
    BAPE_DSP_P_SET_VARIABLE(userConfig.sGraphicEqConfig, i32mEnable[1], handle->settings.graphicEq.rightEnabled ? 1 : 0);
    switch ( handle->settings.graphicEq.bandMode )
    {
    default:
    case BAPE_SrsGraphicEqBandMode_eFiveBand:            
        i = 0; 
        BAPE_DSP_P_SET_VARIABLE(userConfig.sGraphicEqConfig, i32mInputGain, BAPE_P_FloatToQ131(handle->settings.graphicEq.inputGain, 4000));
        break;
    case BAPE_SrsGraphicEqBandMode_eTenBand:             
        i = 1; 
        BAPE_DSP_P_SET_VARIABLE(userConfig.sGraphicEqConfig, i32mInputGain, BAPE_P_FloatToQ131(handle->settings.graphicEq.inputGain, 1000));
        break;
    case BAPE_SrsGraphicEqBandMode_eSpeakerCompensation: 
        i = 2; 
        /* TODO: Input Gain Units? */
        BAPE_DSP_P_SET_VARIABLE(userConfig.sGraphicEqConfig, i32mInputGain, BAPE_P_FloatToQ131(handle->settings.graphicEq.inputGain, 4000));
        break;
    }
    BAPE_DSP_P_SET_VARIABLE(userConfig.sGraphicEqConfig, i32mFilterMode, i);
    for ( i = 0; i < BAPE_STUDIO_SOUND_MAX_GEQ_BANDS; i++ )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig.sGraphicEqConfig, i32mBandGain[0][i], BAPE_P_FloatToQ329(handle->settings.graphicEq.leftBandGain[i], 1000));
        BAPE_DSP_P_SET_VARIABLE(userConfig.sGraphicEqConfig, i32mBandGain[1][i], BAPE_P_FloatToQ329(handle->settings.graphicEq.rightBandGain[i], 1000));
    }    
                
    BAPE_DSP_P_SET_VARIABLE(userConfig.sGraphicEqConfig, i32mOutputGain, BAPE_P_FloatToQ131(handle->settings.graphicEq.outputGain, 100));
    BAPE_DSP_P_SET_VARIABLE(userConfig.sGraphicEqConfig, i32mBypassGain, BAPE_P_FloatToQ131(handle->settings.graphicEq.bypassGain, 100));
    
    /* TruEq */
    BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.i32mEnable[0], handle->settings.truEq.leftEnabled ? 1 : 0);
    BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.i32mEnable[1], handle->settings.truEq.rightEnabled ? 1 : 0);
    
    for ( i = 0; i < BAPE_STUDIO_SOUND_MAX_PEQ_BANDS; i++ )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.i32mBandEnable[0][i], handle->settings.truEq.leftBandEnabled[i] ? 1 : 0);
        BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.i32mBandEnable[1][i], handle->settings.truEq.rightBandEnabled[i] ? 1 : 0);
    }
    
    BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.i32mInputGain, BAPE_P_FloatToQ527(handle->settings.truEq.inputGain, 100));
    BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.i32mOutputGain, BAPE_P_FloatToQ527(handle->settings.truEq.outputGain, 100));
    BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.i32mBypassGain, BAPE_P_FloatToQ527(handle->settings.truEq.bypassGain, 100));

    BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.i32CoefGenMode, (handle->settings.truEq.coefficientMode == BAPE_SrsFilterCoefficientMode_eSpecification) ? 1 : 0);

    for ( i = 0; i < 3; i++ )
    {
        for ( j = 0; j < BAPE_STUDIO_SOUND_MAX_PEQ_BANDS; j++ )
        {
            BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.sFilterCoefPeq[i][j].i32Order, handle->settings.truEq.coefficientSettings.user[i][j].filterOrder);
            BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.sFilterCoefPeq[i][j].i32Scale, handle->settings.truEq.coefficientSettings.user[i][j].scale);            
            BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.sFilterCoefPeq[i][j].i32FilterCoefficientB0, handle->settings.truEq.coefficientSettings.user[i][j].coefficientB0);
            BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.sFilterCoefPeq[i][j].i32FilterCoefficientA1, handle->settings.truEq.coefficientSettings.user[i][j].coefficientA1);
            BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.sFilterCoefPeq[i][j].i32FilterCoefficientB1, handle->settings.truEq.coefficientSettings.user[i][j].coefficientB1);
            BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.sFilterCoefPeq[i][j].i32FilterCoefficientA2, handle->settings.truEq.coefficientSettings.user[i][j].coefficientA2);
            BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.sFilterCoefPeq[i][j].i32FilterCoefficientB2, handle->settings.truEq.coefficientSettings.user[i][j].coefficientB2);
        }        
    }
    for ( i = 0; i < BAPE_STUDIO_SOUND_MAX_PEQ_BANDS; i++ )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.sFilterSpecPeq[i].i32BandGain, handle->settings.truEq.coefficientSettings.specification[i].bandGain);
        BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.sFilterSpecPeq[i].ui32BandFrequency, handle->settings.truEq.coefficientSettings.specification[i].bandFrequency);
        BAPE_DSP_P_SET_VARIABLE(userConfig, sParametricEqConfig.sFilterSpecPeq[i].i32QFactor, handle->settings.truEq.coefficientSettings.specification[i].qFactor);
    }        

    /* HPF */
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.ui32mEnable, handle->settings.highPassFilter.enabled?1:0);
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.ui32CoefGenMode, handle->settings.highPassFilter.coefficientMode == BAPE_SrsFilterCoefficientMode_eSpecification?1:0);
    for ( i = 0; i < 3; i++ )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.sFilterCoefHpf[i].ui32Order, handle->settings.highPassFilter.coefficientSettings.user[i].filterOrder);
        for ( j = 0; j < 3; j++ )
        {
            BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32Scale, handle->settings.highPassFilter.coefficientSettings.user[i].coefficients[j].scale);            
            BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB0, handle->settings.highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB0);            
            BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientA1, handle->settings.highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientA1);            
            BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB1, handle->settings.highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB1);            
            BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientA2, handle->settings.highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientA2);            
            BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.sFilterCoefHpf[i].sFilterCoefHpfTdf2[j].i32FilterCoefficientB2, handle->settings.highPassFilter.coefficientSettings.user[i].coefficients[j].coefficientB2);            
        }
    }
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.sFilterSpecHpf.ui32CutoffFrequency, handle->settings.highPassFilter.coefficientSettings.specification.cutoffFrequency);
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHighPassFilterConfig.sFilterSpecHpf.ui32Order, handle->settings.highPassFilter.coefficientSettings.specification.filterOrder);
    
    /* Hard Limiter */
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHardLimiterConfig.i32LimiterEnable, handle->settings.hardLimiter.enabled?1:0);
    switch ( handle->settings.hardLimiter.blockSize )
    {
    default:
    case BAPE_SrsHardLimiterBlockSize_e256: i = 256; break;
    case BAPE_SrsHardLimiterBlockSize_e512: i = 512; break;
    case BAPE_SrsHardLimiterBlockSize_e768: i = 768; break;
    case BAPE_SrsHardLimiterBlockSize_e1024: i = 1024; break;
    }
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHardLimiterConfig.i32blockSize, i);
    
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHardLimiterConfig.i32InputGain, BAPE_P_FloatToQ131(handle->settings.hardLimiter.inputGain, 400));
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHardLimiterConfig.i32OutputGain, BAPE_P_FloatToQ131(handle->settings.hardLimiter.outputGain, 400));
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHardLimiterConfig.i32BypassGain, BAPE_P_FloatToQ131(handle->settings.hardLimiter.bypassGain, 100));
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHardLimiterConfig.i32LimiterBoost, BAPE_P_FloatToQ131(handle->settings.hardLimiter.boost, 256000));
    BAPE_DSP_P_SET_VARIABLE(userConfig, sHardLimiterConfig.i32HardLimit, BAPE_P_FloatToQ131(handle->settings.hardLimiter.level, 100000));

    BAPE_DSP_P_SET_VARIABLE(userConfig, sHardLimiterConfig.i32DelayLength, handle->settings.hardLimiter.delay);
    
    errCode = BDSP_Task_SetStageSettings(task, handle->branchId, handle->stageId.eqhl, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}        

static BERR_Code BAPE_StudioSound_P_ApplyDspSettings(BAPE_StudioSoundHandle handle, BDSP_TaskHandle task)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_TruVolumeUserConfig truVolumeConfig;

    BDBG_OBJECT_ASSERT(handle, BAPE_StudioSound);
    BDBG_ASSERT(NULL != task);
        
    errCode = BDSP_Task_GetStageSettings(task, handle->branchId, handle->stageId.tvol, &truVolumeConfig, sizeof(truVolumeConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    errCode = BAPE_StudioSound_P_ConvertTopLevelSettings(&handle->settings, &truVolumeConfig.sTopLevelConfig);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    errCode = BAPE_TruVolume_P_ConvertSettingsToDsp(&handle->settings.truVolume, &truVolumeConfig);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    errCode = BDSP_Task_SetStageSettings(task, handle->branchId, handle->stageId.tvol, &truVolumeConfig, sizeof(truVolumeConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    errCode = BDSP_StudioSound_P_ApplyCsTdSettings(handle, task, &truVolumeConfig.sTopLevelConfig); 
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
        
    errCode = BDSP_StudioSound_P_ApplyTruSurroundSettings(handle, task, &truVolumeConfig.sTopLevelConfig); 
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    errCode = BDSP_StudioSound_P_ApplyEqHlSettings(handle, task, &truVolumeConfig.sTopLevelConfig); 
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_StudioSound_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    /* Invalidate task handle */
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BSTD_UNUSED(pConnection);
    pNode->connectors[0].task = NULL;
}

static void BAPE_StudioSound_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_StudioSound_RemoveInput(pNode->pHandle, pConnector);
}
