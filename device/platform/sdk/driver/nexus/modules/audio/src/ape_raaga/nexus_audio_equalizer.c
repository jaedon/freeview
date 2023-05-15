/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_audio_equalizer.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 6/19/12 12:41p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/modules/audio/7422/src/nexus_audio_equalizer.c $
 * 
 * 7   6/19/12 12:41p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 6   5/31/12 10:34a jgarrett
 * SW7425-3133: Adding audio_equalizer_disabled option
 * 
 * 5   2/22/12 3:31p jgarrett
 * SW7405-5003: Removing BKNI_Memcmp
 * 
 * 4   9/19/11 5:34p jgarrett
 * SW7231-372: Coverity CID 35386
 * 
 * 3   9/15/11 3:26p jgarrett
 * SWDTV-8673: Adding checks for changing subwoofer/subsonic filters on
 *  the fly
 * 
 * 2   9/8/11 8:45a jgarrett
 * SWDTV-6627: Adding nexus EQ support
 * 
 * 1   8/18/11 5:51p jgarrett
 * SWDTV-6306: Merge DTV APE changes to main branch
 * 
 * Nexus_APE_Integration/1   7/1/11 5:50p jgarrett
 * SWDTV-6306: Integrated to latest 7422 baseline and nexus audio
 *  directory structure
 * 
 * 1   4/11/11 5:52p jgarrett
 * SWDTV-6306: Adding preliminary Nexus APE support for DTV
 * 
 *****************************************************************************/
#include "nexus_audio_module.h"

BDBG_MODULE(nexus_audio_equalizer);

typedef struct NEXUS_AudioEqualizerOutputNode
{
    BLST_S_ENTRY(NEXUS_AudioEqualizerOutputNode) node;
    NEXUS_AudioOutput output;           /* Output attached to this equalizer */
} NEXUS_AudioEqualizerOutputNode;

/* You can have at most one of each type in this interface.  All types except cascaded PEQ are 1 stage.  Cascated PEQ is up to NEXUS_MAX_CASCADED_PARAMETRIC_EQ stages.  Subwoofer/Subsonic are two additional. */
#define NEXUS_AUDIO_EQUALIZER_MAX_STAGES ((2+NEXUS_AUDIO_EQUALIZER_MAX_ADDITIONAL_MODE) + NEXUS_MAX_CASCADED_PARAMETRIC_EQ + 2)
#define EQ_STAGE_ID_INVALID (0xffffffff)

typedef struct NEXUS_AudioEqualizer
{
    NEXUS_OBJECT(NEXUS_AudioEqualizer);
    /* Is this opened? */
    bool opened;
    /* Current Settings */
    NEXUS_AudioEqualizerSettings settings;
    /* Stage List.  Stages are allocated per nexus equalizer, while APE Equalizers are allocated per-output. */
    unsigned numStages;    
    BAPE_EqualizerStageHandle stages[NEXUS_AUDIO_EQUALIZER_MAX_STAGES];
    unsigned subwooferStageId;
    unsigned subsonicStageId;
    unsigned stageTypeIds[NEXUS_AudioEqualizerMode_eMax];
    /* List of outputs connected to this equalizer */
    BLST_S_HEAD(EqualizerOutputList, NEXUS_AudioEqualizerOutputNode) outputList;
} NEXUS_AudioEqualizer;

#define NEXUS_MAX_AUDIO_EQUALIZERS (12) /* Max number of SRC's on most chips, will likely not use more than 2 or 3 on any chip */
static NEXUS_AudioEqualizer g_equalizers[NEXUS_MAX_AUDIO_EQUALIZERS];

static bool NEXUS_AudioEqualizer_P_IsRunning(NEXUS_AudioEqualizerHandle handle);
static NEXUS_Error NEXUS_AudioEqualizer_P_CreateConnection(NEXUS_AudioEqualizerHandle handle, NEXUS_AudioOutput output);
static void NEXUS_AudioEqualizer_P_DestroyConnection(NEXUS_AudioEqualizerHandle handle, NEXUS_AudioOutput output);
static NEXUS_Error NEXUS_AudioEqualizer_P_AllocateStages(NEXUS_AudioEqualizerHandle handle, const NEXUS_AudioEqualizerSettings *pSettings);
static void NEXUS_AudioEqualizer_P_FreeStages(NEXUS_AudioEqualizerHandle handle);        
static NEXUS_Error NEXUS_AudioEqualizer_P_ApplySettings(NEXUS_AudioEqualizerHandle handle, const NEXUS_AudioEqualizerSettings *pSettings);
static void NEXUS_AudioEqualizer_P_OutputSettingsChanged(NEXUS_AudioEqualizerHandle handle);

static NEXUS_AudioEqualizerWindowStep NEXUS_AudioEqualizer_P_WindowStepToNexus(BAPE_EqualizerWindowStep magnum)
{
    switch ( magnum )
    {
    default:
    case BAPE_EqualizerWindowStep_eNone:
        return NEXUS_AudioEqualizerWindowStep_eNone;
    case BAPE_EqualizerWindowStep_e170_6:
        return NEXUS_AudioEqualizerWindowStep_e170_6;
    case BAPE_EqualizerWindowStep_e85_3:
        return NEXUS_AudioEqualizerWindowStep_e85_3;
    case BAPE_EqualizerWindowStep_e42_6:
        return NEXUS_AudioEqualizerWindowStep_e42_6;
    case BAPE_EqualizerWindowStep_e21_3:
        return NEXUS_AudioEqualizerWindowStep_e21_3;
    case BAPE_EqualizerWindowStep_e10_6:
        return NEXUS_AudioEqualizerWindowStep_e10_6;
    case BAPE_EqualizerWindowStep_e5_3:
        return NEXUS_AudioEqualizerWindowStep_e5_3;
    case BAPE_EqualizerWindowStep_e2_6:
        return NEXUS_AudioEqualizerWindowStep_e2_6;
    }
}

void NEXUS_AudioEqualizer_GetDefaultSettings(
    NEXUS_AudioEqualizerSettings *pSettings    /* [out] Default Settings */
    )
{
    BAPE_EqualizerStageSettings stageSettings;
    unsigned i, j;
    
    BDBG_ASSERT(NULL != pSettings);
    
    BKNI_Memset(pSettings, 0, sizeof(NEXUS_AudioEqualizerSettings));
        
    /* Propagate nexus settings to stages in the PI */
    BAPE_EqualizerStage_GetDefaultSettings(BAPE_EqualizerStageType_eToneControl, &stageSettings);
    pSettings->modeSettings.tone.bass = stageSettings.modeSettings.toneControl.bassGain;
    pSettings->modeSettings.tone.treble = stageSettings.modeSettings.toneControl.trebleGain;
    pSettings->modeSettings.tone.bassFrequency = stageSettings.modeSettings.toneControl.bassFreq;
    pSettings->modeSettings.tone.trebleFrequency = stageSettings.modeSettings.toneControl.trebleFreq;
    pSettings->modeSettings.tone.bassBandWidthFrequency = stageSettings.modeSettings.toneControl.bassBandwidthFreq;
    pSettings->modeSettings.tone.trebleBandWidthFrequency = stageSettings.modeSettings.toneControl.trebleBandwidthFreq;

    BAPE_EqualizerStage_GetDefaultSettings(BAPE_EqualizerStageType_eFiveBand, &stageSettings);
    pSettings->modeSettings.fiveBand.gain100Hz = stageSettings.modeSettings.fiveBand.gain100Hz;
    pSettings->modeSettings.fiveBand.gain300Hz = stageSettings.modeSettings.fiveBand.gain300Hz;
    pSettings->modeSettings.fiveBand.gain1000Hz = stageSettings.modeSettings.fiveBand.gain1000Hz;
    pSettings->modeSettings.fiveBand.gain3000Hz = stageSettings.modeSettings.fiveBand.gain3000Hz;
    pSettings->modeSettings.fiveBand.gain10000Hz = stageSettings.modeSettings.fiveBand.gain10000Hz;

    BAPE_EqualizerStage_GetDefaultSettings(BAPE_EqualizerStageType_eSevenBand, &stageSettings);
    pSettings->modeSettings.parametric.windowStep = NEXUS_AudioEqualizer_P_WindowStepToNexus(stageSettings.modeSettings.sevenBand.windowStep);
    for ( i = 0; i < 7; i++ )
    {
        pSettings->modeSettings.parametric.bandSettings[i].peak = stageSettings.modeSettings.sevenBand.bandSettings[i].peak;
        pSettings->modeSettings.parametric.bandSettings[i].gain = stageSettings.modeSettings.sevenBand.bandSettings[i].gain;
        pSettings->modeSettings.parametric.bandSettings[i].q = stageSettings.modeSettings.sevenBand.bandSettings[i].q;
    }
    for ( j = 0; j < NEXUS_MAX_CASCADED_PARAMETRIC_EQ; j++ )
    {
        pSettings->modeSettings.cascadedParametric[j].windowStep = NEXUS_AudioEqualizer_P_WindowStepToNexus(stageSettings.modeSettings.sevenBand.windowStep);
        for ( i = 0; i < 7; i++ )
        {
            pSettings->modeSettings.cascadedParametric[j].bandSettings[i].peak = stageSettings.modeSettings.sevenBand.bandSettings[i].peak;
            pSettings->modeSettings.cascadedParametric[j].bandSettings[i].gain = stageSettings.modeSettings.sevenBand.bandSettings[i].gain;
            pSettings->modeSettings.cascadedParametric[j].bandSettings[i].q = stageSettings.modeSettings.sevenBand.bandSettings[i].q;
        }
    }

    BAPE_EqualizerStage_GetDefaultSettings(BAPE_EqualizerStageType_eCustom, &stageSettings);
    stageSettings.modeSettings.custom.numFilters = pSettings->modeSettings.custom.numFilters;
    for ( i = 0; i < BAPE_EQUALIZER_MAX_CUSTOM_FILTERS; i++ )
    {
        pSettings->modeSettings.custom.coefficient0[i] = stageSettings.modeSettings.custom.filterCoefficients[i].b0;
        pSettings->modeSettings.custom.coefficient1[i] = stageSettings.modeSettings.custom.filterCoefficients[i].b1;
        pSettings->modeSettings.custom.coefficient2[i] = stageSettings.modeSettings.custom.filterCoefficients[i].b2;
        pSettings->modeSettings.custom.coefficient3[i] = stageSettings.modeSettings.custom.filterCoefficients[i].a1;
        pSettings->modeSettings.custom.coefficient4[i] = stageSettings.modeSettings.custom.filterCoefficients[i].a2;
    }

    BAPE_EqualizerStage_GetDefaultSettings(BAPE_EqualizerStageType_eSubwoofer, &stageSettings);
    pSettings->subwooferFrequency = stageSettings.modeSettings.subwoofer.frequency;

    BAPE_EqualizerStage_GetDefaultSettings(BAPE_EqualizerStageType_eSubsonic, &stageSettings);
    pSettings->subsonicFrequency = stageSettings.modeSettings.subsonic.frequency;
}

NEXUS_AudioEqualizerHandle NEXUS_AudioEqualizer_Open(
    unsigned index,
    const NEXUS_AudioEqualizerSettings *pSettings   /* Pass NULL for defaults */
    )
{
    NEXUS_AudioEqualizerSettings defaults;
    NEXUS_AudioEqualizerHandle handle;
    NEXUS_Error errCode;
    unsigned i;
        
    if ( NULL == pSettings )
    {
        NEXUS_AudioEqualizer_GetDefaultSettings(&defaults);
        pSettings = &defaults;
    }
    
    if ( index >= NEXUS_MAX_AUDIO_EQUALIZERS )
    {
        BDBG_ERR(("Equalizer %u not supported.  This chipset supports up to %u equalizers.", index, NEXUS_MAX_AUDIO_EQUALIZERS));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }
    
    handle = &g_equalizers[index];
    if ( handle->opened )
    {
        BDBG_ERR(("Equalizer %u is already open.", index));
        (void)BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    
    NEXUS_OBJECT_INIT(NEXUS_AudioEqualizer, handle);
    handle->opened = true;
    handle->subwooferStageId = EQ_STAGE_ID_INVALID;
    handle->subsonicStageId = EQ_STAGE_ID_INVALID;
    for ( i = 0; i < (unsigned)NEXUS_AudioEqualizerMode_eMax; i++ )
    {
        handle->stageTypeIds[i] = EQ_STAGE_ID_INVALID;
    }
    
    errCode = NEXUS_AudioEqualizer_SetSettings(handle, pSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        NEXUS_AudioEqualizer_Close(handle);
        return NULL;
    }
    
    return handle;
}


static void NEXUS_AudioEqualizer_P_Finalizer(
    NEXUS_AudioEqualizerHandle handle
    )
{
    NEXUS_AudioEqualizerOutputNode *pNode;
    
    NEXUS_OBJECT_ASSERT(NEXUS_AudioEqualizer, handle);

    /* Go through all connected outputs and break the connections */
    while ( NULL != (pNode = BLST_S_FIRST(&handle->outputList)) )
    {
        (void)NEXUS_AudioOutput_SetEqualizer(pNode->output, NULL);
    }
    
    NEXUS_AudioEqualizer_P_FreeStages(handle);
    
    BKNI_Memset(handle, 0, sizeof(NEXUS_AudioEqualizer));
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioEqualizer, NEXUS_AudioEqualizer_Close);

void NEXUS_AudioEqualizer_GetSettings(
    NEXUS_AudioEqualizerHandle handle,
    NEXUS_AudioEqualizerSettings *pSettings    /* [out] Current Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEqualizer);
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memcpy(pSettings, &handle->settings, sizeof(NEXUS_AudioEqualizerSettings));
}

NEXUS_Error NEXUS_AudioEqualizer_SetSettings(
    NEXUS_AudioEqualizerHandle handle,
    const NEXUS_AudioEqualizerSettings *pSettings
    )
{
    NEXUS_Error errCode;
    bool modeChanged = false;
        
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEqualizer);
    BDBG_ASSERT(NULL != pSettings);
       
    BDBG_CASSERT(2==NEXUS_AUDIO_EQUALIZER_MAX_ADDITIONAL_MODE);

    /* Determine if a mode has changed */
    if ( (pSettings->mode != handle->settings.mode) ||
         (pSettings->secondaryMode != handle->settings.secondaryMode) ||
         pSettings->additionalModes[0] != handle->settings.additionalModes[0] ||
         pSettings->additionalModes[1] != handle->settings.additionalModes[1] ||
         (pSettings->subwooferFilterEnabled != handle->settings.subwooferFilterEnabled) ||
         (pSettings->subsonicFilterEnabled != handle->settings.subsonicFilterEnabled) )
    {
        /* Can not change modes while running */
        if ( NEXUS_AudioEqualizer_P_IsRunning(handle) )
        {
            BDBG_ERR(("Can not change equalizer modes while any connected outputs are running."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);            
        }
        
        modeChanged = true;    
    }
      
    if ( modeChanged )
    {
        /* Free existing stages */
        NEXUS_AudioEqualizer_P_FreeStages(handle);        
        
        errCode = NEXUS_AudioEqualizer_P_AllocateStages(handle, pSettings);
        if ( errCode )
        {
            NEXUS_AudioEqualizer_P_FreeStages(handle);        
            /* Settings are now invalid.  Revert to defaults. */
            NEXUS_AudioEqualizer_GetDefaultSettings(&handle->settings);
            return BERR_TRACE(errCode);         
        }    
    }
    
    /* Update settings per stage */
    errCode = NEXUS_AudioEqualizer_P_ApplySettings(handle, pSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);                 
    }

    /* Store new settings */
    BKNI_Memcpy(&handle->settings, pSettings, sizeof(NEXUS_AudioEqualizerSettings));
    
    /* Force equalizers to be re-allocated on next start call if required */
    if ( modeChanged )
    {
        NEXUS_AudioEqualizer_P_OutputSettingsChanged(handle);
    }    
    
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_AudioOutput_SetEqualizer(
    NEXUS_AudioOutput output,
    NEXUS_AudioEqualizerHandle equalizer    /* Pass NULL to remove any equalizer connected to this output */
    )
{
    NEXUS_AudioOutputData *pData;
    NEXUS_Error errCode;
    
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    if ( NULL != equalizer )
    {
        /* Passing NULL to remove the equalizer is acceptable */
        BDBG_OBJECT_ASSERT(equalizer, NEXUS_AudioEqualizer);
    }
    
    if ( NEXUS_GetEnv("audio_equalizer_disabled") )
    {
        return BERR_SUCCESS;
    }

    pData = output->pMixerData;
    if ( NULL == pData )
    {
        pData = NEXUS_AudioOutput_P_CreateData(output);
        if ( NULL == pData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }
    
    if ( equalizer != pData->equalizer )
    {
        if ( NEXUS_AudioOutput_P_IsRunning(output) )
        {
            BDBG_ERR(("Can not change equalizers while an output is running."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        if ( pData->equalizer )
        {
            /* If connected to an EQ already, break the connection */
            NEXUS_AudioEqualizer_P_DestroyConnection(pData->equalizer, output);
            pData->equalizer = NULL;
        }
        if ( equalizer )
        {
            errCode = NEXUS_AudioEqualizer_P_CreateConnection(equalizer, output);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
            pData->equalizer = equalizer;
        }
        if ( pData->input )
        {
            /* Mixer reconfiguration will be required on the next start/stop */
            (void)NEXUS_AudioInput_P_OutputSettingsChanged(pData->input, output);
        }
    }
    
    return BERR_SUCCESS;
}

static bool NEXUS_AudioEqualizer_P_IsRunning(NEXUS_AudioEqualizerHandle handle)
{
    NEXUS_AudioEqualizerOutputNode *pNode;
    
    for ( pNode = BLST_S_FIRST(&handle->outputList); 
          NULL != pNode; 
          pNode = BLST_S_NEXT(pNode, node) )
    {
        if ( NEXUS_AudioOutput_P_IsRunning(pNode->output) )
        {
            return true;
        }
    }
    
    return false;
}    

static void NEXUS_AudioEqualizer_P_OutputSettingsChanged(NEXUS_AudioEqualizerHandle handle)
{
    NEXUS_AudioEqualizerOutputNode *pNode;

    for ( pNode = BLST_S_FIRST(&handle->outputList); 
          NULL != pNode; 
          pNode = BLST_S_NEXT(pNode, node) )
    {
        NEXUS_AudioInput input;
        NEXUS_AudioOutput output;
        NEXUS_AudioOutputData *pData;

        output = pNode->output;
        pData = output->pMixerData;
        if ( pData )
        {
            input = pData->input;
            if ( input )
            {
                (void)NEXUS_AudioInput_P_OutputSettingsChanged(input, output);
            }
        }
    }
}

static NEXUS_Error NEXUS_AudioEqualizer_P_CreateConnection(NEXUS_AudioEqualizerHandle handle, NEXUS_AudioOutput output)
{
    NEXUS_AudioOutputData *pData;
    NEXUS_AudioEqualizerOutputNode *pNode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEqualizer);
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);

    pData = output->pMixerData;
    if ( NULL == pData )
    {
        pData = NEXUS_AudioOutput_P_CreateData(output);
        if ( NULL == pData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }

    /* Make sure no other connection exists */
    BDBG_ASSERT(pData->equalizer == NULL);

    /* Create node for output */
    pNode = BKNI_Malloc(sizeof(NEXUS_AudioEqualizerOutputNode));
    if ( NULL == pNode )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(pNode, 0, sizeof(NEXUS_AudioEqualizerOutputNode));

    pNode->output = output;
    BLST_S_INSERT_HEAD(&handle->outputList, pNode, node);
    pData->equalizer = handle;

    return NEXUS_SUCCESS;
}

static void NEXUS_AudioEqualizer_P_DestroyConnection(NEXUS_AudioEqualizerHandle handle, NEXUS_AudioOutput output)
{
    NEXUS_AudioOutputData *pData;
    NEXUS_AudioEqualizerOutputNode *pPrev=NULL, *pNode;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEqualizer);
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    
    pData = output->pMixerData;
    if ( pData )
    {
        NEXUS_AudioInput input = pData->input;
        
        /* Make sure these are actually connected */
        BDBG_ASSERT(pData->equalizer == handle);    

        if ( input )
        {
            NEXUS_Error errCode;
            
            if ( NEXUS_AudioInput_P_IsRunning(input) )
            {
                BDBG_WRN(("Forcing input %p to stop on equalizer %p shutdown", input, handle));
                NEXUS_AudioInput_P_ForceStop(input);
            }
            
            /* Make sure the actual equalizer is destroyed by removing and re-adding the output now that we're sure it's stopped */
            NEXUS_AudioOutput_RemoveAllInputs(output);
            errCode = NEXUS_AudioOutput_AddInput(output, input);
            /* This had best not fail */
            BDBG_ASSERT(errCode == BERR_SUCCESS);
        }                                           
    }
    for ( pNode = BLST_S_FIRST(&handle->outputList);
          NULL != pNode;
          pNode = BLST_S_NEXT(pNode, node) )
    {
        if ( pNode->output == output )
        {
            /* Unlink the output from the equalizer */
            if ( pPrev )
            {
                BLST_S_REMOVE_NEXT(&handle->outputList, pPrev, node);
            }
            else
            {
                BLST_S_REMOVE_HEAD(&handle->outputList, node);
            }
            BKNI_Memset(pNode, 0, sizeof(NEXUS_AudioEqualizerOutputNode));
            BKNI_Free(pNode);
            break;
        }
        pPrev = pNode;
    }
}

static unsigned NEXUS_AudioEqualizer_P_GetNumStages(NEXUS_AudioEqualizerMode mode)
{
    switch ( mode )
    {
    default:
    case NEXUS_AudioEqualizerMode_eDisabled:
        return 0;
    case NEXUS_AudioEqualizerMode_eTone:
    case NEXUS_AudioEqualizerMode_eFiveBand:
    case NEXUS_AudioEqualizerMode_eParametric:
    case NEXUS_AudioEqualizerMode_eCustom:
        return 1;
    case NEXUS_AudioEqualizerMode_eCascadedParametric:
        return NEXUS_MAX_CASCADED_PARAMETRIC_EQ;    /* TODO: This may be smaller */
    }
}

static NEXUS_Error NEXUS_AudioEqualizer_P_AllocateStages(NEXUS_AudioEqualizerHandle handle, const NEXUS_AudioEqualizerSettings *pSettings)
{
    BAPE_EqualizerStageSettings stageSettings;
    unsigned numStages=0, i, stage;
    BERR_Code errCode;
    bool modeUsed[NEXUS_AudioEqualizerMode_eMax];
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEqualizer);
    BDBG_ASSERT(NULL != pSettings);
    
    /* Determine number of stages required */
    if ( pSettings->bassManagementEnabled )
    {
        BDBG_ERR(("Bass management is not supported"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    
    /* Reset array of visited types */
    BKNI_Memset(modeUsed, 0, sizeof(modeUsed));
    
    if ( modeUsed[pSettings->mode] == false || pSettings->mode == NEXUS_AudioEqualizerMode_eDisabled )
    {
        numStages += NEXUS_AudioEqualizer_P_GetNumStages(pSettings->mode);
        modeUsed[pSettings->mode] = true;
    }
    else
    {
        BDBG_ERR(("The same mode can not be used more than once with the same equalizer"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    if ( modeUsed[pSettings->secondaryMode] == false || pSettings->secondaryMode == NEXUS_AudioEqualizerMode_eDisabled )
    {
        numStages += NEXUS_AudioEqualizer_P_GetNumStages(pSettings->secondaryMode);
        modeUsed[pSettings->secondaryMode] = true;
    }
    else
    {
        BDBG_ERR(("The same mode can not be used more than once with the same equalizer"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    for ( i = 0; i < NEXUS_AUDIO_EQUALIZER_MAX_ADDITIONAL_MODE; i++ )
    {
        if ( modeUsed[pSettings->additionalModes[i]] == false || pSettings->additionalModes[i] == NEXUS_AudioEqualizerMode_eDisabled )
        {
            numStages += NEXUS_AudioEqualizer_P_GetNumStages(pSettings->additionalModes[i]);
            modeUsed[pSettings->additionalModes[i]] = true;
        }
        else
        {
            BDBG_ERR(("The same mode can not be used more than once with the same equalizer"));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    if ( pSettings->subsonicFilterEnabled )
    {
        numStages++;
    }
    if ( pSettings->subwooferFilterEnabled )
    {
        numStages++;
    }
    if ( numStages > NEXUS_AUDIO_EQUALIZER_MAX_STAGES )
    {
        BDBG_ERR(("Requested equalizer mode would require %u stages but only %u are available.", NEXUS_AUDIO_EQUALIZER_MAX_STAGES));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    
    stage = 0;
    /* Go through and actually allocate the stages */    
    for ( i = (unsigned)NEXUS_AudioEqualizerMode_eTone;
          i < (unsigned)NEXUS_AudioEqualizerMode_eMax;
          i++ )
    {
        unsigned j, num=1;
        BAPE_EqualizerStageType stageType;

        /* Add only modes we are actually using */
        if ( modeUsed[i] )
        {
            switch ( i )
            {
            default:
            case NEXUS_AudioEqualizerMode_eTone:
                stageType = BAPE_EqualizerStageType_eToneControl;
                break;
            case NEXUS_AudioEqualizerMode_eFiveBand:
                stageType = BAPE_EqualizerStageType_eFiveBand;
                break;
            case NEXUS_AudioEqualizerMode_eParametric:
                stageType = BAPE_EqualizerStageType_eSevenBand;
                break;
            case NEXUS_AudioEqualizerMode_eCustom:
                stageType = BAPE_EqualizerStageType_eCustom;
                break;
            case NEXUS_AudioEqualizerMode_eCascadedParametric:
                stageType = BAPE_EqualizerStageType_eSevenBand;
                num = NEXUS_MAX_CASCADED_PARAMETRIC_EQ;
                break;
            }        
            
            handle->stageTypeIds[i] = stage;
            BAPE_EqualizerStage_GetDefaultSettings(stageType, &stageSettings);
            for ( j = 0; j < num; j++ )
            {
                errCode = BAPE_EqualizerStage_Create(NEXUS_AUDIO_DEVICE_HANDLE, &stageSettings, &handle->stages[stage]);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                    goto err_stage_create;                                
                }
                stage++;
            }
        }
    }

    /* Add subwoofer/subsonic stages also */
    if ( pSettings->subwooferFilterEnabled )
    {
        handle->subwooferStageId = stage;
        BAPE_EqualizerStage_GetDefaultSettings(BAPE_EqualizerStageType_eSubwoofer, &stageSettings);        
        errCode = BAPE_EqualizerStage_Create(NEXUS_AUDIO_DEVICE_HANDLE, &stageSettings, &handle->stages[stage]);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_stage_create;                                
        }
        stage++;
    }
    if ( pSettings->subsonicFilterEnabled )
    {
        handle->subsonicStageId = stage;
        BAPE_EqualizerStage_GetDefaultSettings(BAPE_EqualizerStageType_eSubsonic, &stageSettings);        
        errCode = BAPE_EqualizerStage_Create(NEXUS_AUDIO_DEVICE_HANDLE, &stageSettings, &handle->stages[stage]);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_stage_create;                                
        }
        stage++;
    }
    handle->numStages = numStages;
        
    return BERR_SUCCESS;

err_stage_create:
    NEXUS_AudioEqualizer_P_FreeStages(handle);
    return errCode;            
}

static void NEXUS_AudioEqualizer_P_FreeStages(NEXUS_AudioEqualizerHandle handle)
{
    unsigned i;
    NEXUS_Error errCode;
    NEXUS_AudioInput input;
    NEXUS_AudioOutputData *pData;
    NEXUS_AudioEqualizerOutputNode *pNode;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEqualizer);
    
    /* We need to make sure that no outputs are actively using these stages first */
    for ( pNode = BLST_S_FIRST(&handle->outputList);
          NULL != pNode;
          pNode = BLST_S_NEXT(pNode, node) )
    {
        /* We need to be sure and release all connections between equalizers and the stages */
        /* Break the connection between the output and input to force release all the PI resources */
        pData = pNode->output->pMixerData;
        if ( pData )
        {
            input = pData->input;
            if ( input )
            {
                (void)NEXUS_AudioOutput_RemoveAllInputs(pNode->output);
                errCode = NEXUS_AudioOutput_AddInput(pNode->output, input);
                BDBG_ASSERT(errCode == NEXUS_SUCCESS);
            }
        }
    }

    for ( i = 0; i < NEXUS_AUDIO_EQUALIZER_MAX_STAGES; i++ )
    {
        if ( handle->stages[i] )
        {
            BAPE_EqualizerStage_Destroy(handle->stages[i]);
            handle->stages[i] = NULL;
        }
    }
    
    handle->subwooferStageId = EQ_STAGE_ID_INVALID;
    handle->subsonicStageId = EQ_STAGE_ID_INVALID;
    for ( i = 0; i < (unsigned)NEXUS_AudioEqualizerMode_eMax; i++ )
    {
        handle->stageTypeIds[i] = EQ_STAGE_ID_INVALID;
    }
}

static BAPE_EqualizerWindowStep NEXUS_AudioEqualizer_P_WindowStepToMagnum(NEXUS_AudioEqualizerWindowStep nexus)
{
    switch ( nexus )
    {
    default:
    case NEXUS_AudioEqualizerWindowStep_eNone:
        return BAPE_EqualizerWindowStep_eNone;
    case NEXUS_AudioEqualizerWindowStep_e170_6:
        return BAPE_EqualizerWindowStep_e170_6;
    case NEXUS_AudioEqualizerWindowStep_e85_3:
        return BAPE_EqualizerWindowStep_e85_3;
    case NEXUS_AudioEqualizerWindowStep_e42_6:
        return BAPE_EqualizerWindowStep_e42_6;
    case NEXUS_AudioEqualizerWindowStep_e21_3:
        return BAPE_EqualizerWindowStep_e21_3;
    case NEXUS_AudioEqualizerWindowStep_e10_6:
        return BAPE_EqualizerWindowStep_e10_6;
    case NEXUS_AudioEqualizerWindowStep_e5_3:
        return BAPE_EqualizerWindowStep_e5_3;
    case NEXUS_AudioEqualizerWindowStep_e2_6:
        return BAPE_EqualizerWindowStep_e2_6;
    }
}

static NEXUS_Error NEXUS_AudioEqualizer_P_ApplySettings(NEXUS_AudioEqualizerHandle handle, const NEXUS_AudioEqualizerSettings *pSettings)
{
    BAPE_EqualizerStageHandle stage;
    BAPE_EqualizerStageSettings stageSettings;
    unsigned i,j;
    BERR_Code errCode;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEqualizer);
    BDBG_ASSERT(NULL != pSettings);

    /* Propagate nexus settings to stages in the PI */
    if ( handle->stageTypeIds[NEXUS_AudioEqualizerMode_eTone] != EQ_STAGE_ID_INVALID )
    {
        stage = handle->stages[handle->stageTypeIds[NEXUS_AudioEqualizerMode_eTone]];
        BAPE_EqualizerStage_GetSettings(stage, &stageSettings);
        stageSettings.modeSettings.toneControl.bassGain = pSettings->modeSettings.tone.bass;
        stageSettings.modeSettings.toneControl.trebleGain = pSettings->modeSettings.tone.treble;
        stageSettings.modeSettings.toneControl.bassFreq = pSettings->modeSettings.tone.bassFrequency;
        stageSettings.modeSettings.toneControl.trebleFreq = pSettings->modeSettings.tone.trebleFrequency;
        stageSettings.modeSettings.toneControl.bassBandwidthFreq = pSettings->modeSettings.tone.bassBandWidthFrequency;
        stageSettings.modeSettings.toneControl.trebleBandwidthFreq = pSettings->modeSettings.tone.trebleBandWidthFrequency;
        errCode = BAPE_EqualizerStage_SetSettings(stage, &stageSettings);
        if ( errCode ) { return BERR_TRACE(errCode); }
    }
    if ( handle->stageTypeIds[NEXUS_AudioEqualizerMode_eFiveBand] != EQ_STAGE_ID_INVALID )
    {
        stage = handle->stages[handle->stageTypeIds[NEXUS_AudioEqualizerMode_eFiveBand]];
        BAPE_EqualizerStage_GetSettings(stage, &stageSettings);
        stageSettings.modeSettings.fiveBand.gain100Hz = pSettings->modeSettings.fiveBand.gain100Hz;
        stageSettings.modeSettings.fiveBand.gain300Hz = pSettings->modeSettings.fiveBand.gain300Hz;
        stageSettings.modeSettings.fiveBand.gain1000Hz = pSettings->modeSettings.fiveBand.gain1000Hz;
        stageSettings.modeSettings.fiveBand.gain3000Hz = pSettings->modeSettings.fiveBand.gain3000Hz;
        stageSettings.modeSettings.fiveBand.gain10000Hz = pSettings->modeSettings.fiveBand.gain10000Hz;
        errCode = BAPE_EqualizerStage_SetSettings(stage, &stageSettings);
        if ( errCode ) { return BERR_TRACE(errCode); }
    }
    if ( handle->stageTypeIds[NEXUS_AudioEqualizerMode_eParametric] != EQ_STAGE_ID_INVALID )
    {
        stage = handle->stages[handle->stageTypeIds[NEXUS_AudioEqualizerMode_eParametric]];
        BAPE_EqualizerStage_GetSettings(stage, &stageSettings);
        stageSettings.modeSettings.sevenBand.windowStep = NEXUS_AudioEqualizer_P_WindowStepToMagnum(pSettings->modeSettings.parametric.windowStep);
        for ( i = 0; i < 7; i++ )
        {
            stageSettings.modeSettings.sevenBand.bandSettings[i].peak = pSettings->modeSettings.parametric.bandSettings[i].peak;
            stageSettings.modeSettings.sevenBand.bandSettings[i].gain = pSettings->modeSettings.parametric.bandSettings[i].gain;
            stageSettings.modeSettings.sevenBand.bandSettings[i].q = pSettings->modeSettings.parametric.bandSettings[i].q;
        }
        errCode = BAPE_EqualizerStage_SetSettings(stage, &stageSettings);
        if ( errCode ) { return BERR_TRACE(errCode); }
    }
    if ( handle->stageTypeIds[NEXUS_AudioEqualizerMode_eCascadedParametric] != EQ_STAGE_ID_INVALID )
    {
        for ( j = 0; j < NEXUS_MAX_CASCADED_PARAMETRIC_EQ; j++ )
        {
            stage = handle->stages[handle->stageTypeIds[NEXUS_AudioEqualizerMode_eCascadedParametric]+j];
            BAPE_EqualizerStage_GetSettings(stage, &stageSettings);
            stageSettings.modeSettings.sevenBand.windowStep = NEXUS_AudioEqualizer_P_WindowStepToMagnum(pSettings->modeSettings.cascadedParametric[j].windowStep);
            for ( i = 0; i < 7; i++ )
            {
                stageSettings.modeSettings.sevenBand.bandSettings[i].peak = pSettings->modeSettings.cascadedParametric[j].bandSettings[i].peak;
                stageSettings.modeSettings.sevenBand.bandSettings[i].gain = pSettings->modeSettings.cascadedParametric[j].bandSettings[i].gain;
                stageSettings.modeSettings.sevenBand.bandSettings[i].q = pSettings->modeSettings.cascadedParametric[j].bandSettings[i].q;
            }
            errCode = BAPE_EqualizerStage_SetSettings(stage, &stageSettings);
            if ( errCode ) { return BERR_TRACE(errCode); }
        }
    }
    if ( handle->stageTypeIds[NEXUS_AudioEqualizerMode_eCustom] != EQ_STAGE_ID_INVALID )
    {
        stage = handle->stages[handle->stageTypeIds[NEXUS_AudioEqualizerMode_eCustom]];
        BAPE_EqualizerStage_GetSettings(stage, &stageSettings);
        stageSettings.modeSettings.custom.numFilters = pSettings->modeSettings.custom.numFilters;
        BDBG_CASSERT(NEXUS_AUDIO_EQUALIZER_MAX_CUSTOM_COEFFICIENTS==BAPE_EQUALIZER_MAX_CUSTOM_FILTERS);
        for ( i = 0; i < BAPE_EQUALIZER_MAX_CUSTOM_FILTERS; i++ )
        {
            stageSettings.modeSettings.custom.filterCoefficients[i].b0 = pSettings->modeSettings.custom.coefficient0[i];
            stageSettings.modeSettings.custom.filterCoefficients[i].b1 = pSettings->modeSettings.custom.coefficient1[i];
            stageSettings.modeSettings.custom.filterCoefficients[i].b2 = pSettings->modeSettings.custom.coefficient2[i];
            stageSettings.modeSettings.custom.filterCoefficients[i].a1 = pSettings->modeSettings.custom.coefficient3[i];
            stageSettings.modeSettings.custom.filterCoefficients[i].a2 = pSettings->modeSettings.custom.coefficient4[i];
        }
        errCode = BAPE_EqualizerStage_SetSettings(stage, &stageSettings);
        if ( errCode ) { return BERR_TRACE(errCode); }
    }
    if ( handle->subwooferStageId != EQ_STAGE_ID_INVALID )
    {
        stage = handle->stages[handle->subwooferStageId];
        BAPE_EqualizerStage_GetSettings(stage, &stageSettings);
        stageSettings.modeSettings.subwoofer.frequency = pSettings->subwooferFrequency;
        errCode = BAPE_EqualizerStage_SetSettings(stage, &stageSettings);
        if ( errCode ) { return BERR_TRACE(errCode); }
    }
    if ( handle->subsonicStageId != EQ_STAGE_ID_INVALID )
    {
        stage = handle->stages[handle->subsonicStageId];
        BAPE_EqualizerStage_GetSettings(stage, &stageSettings);
        stageSettings.modeSettings.subsonic.frequency = pSettings->subsonicFrequency;
        errCode = BAPE_EqualizerStage_SetSettings(stage, &stageSettings);
        if ( errCode ) { return BERR_TRACE(errCode); }
    }

    return NEXUS_SUCCESS;
}

NEXUS_AudioEqualizerHandle NEXUS_AudioOutput_P_GetEqualizer(
    NEXUS_AudioOutput output
    )
{
    NEXUS_AudioOutputData *pData;

    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);

    pData = output->pMixerData;
    if ( pData )
    {
        return pData->equalizer;
    }
    return NULL;
}

void NEXUS_AudioEqualizer_P_GetStages(
    NEXUS_AudioEqualizerHandle handle,
    BAPE_EqualizerStageHandle **pStages,
    unsigned *pNumStages
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEqualizer);
    BDBG_ASSERT(NULL != pStages);
    BDBG_ASSERT(NULL != pNumStages);

    *pStages = handle->stages;
    *pNumStages = handle->numStages;
}

