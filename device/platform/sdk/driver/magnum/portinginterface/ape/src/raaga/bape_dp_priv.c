/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_dp_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 1/24/13 11:41a $
 *
 * Module Description: Audio PI Device Level Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_dp_priv.c $
 * 
 * Hydra_Software_Devel/22   1/24/13 11:41a mpeteri
 * SW7358-430: merge to main
 * 
 * Hydra_Software_Devel/SW7358-430/3   1/24/13 11:37a mpeteri
 * SW7358-430: Fix compile warnings
 * 
 * Hydra_Software_Devel/21   1/23/13 4:28p mpeteri
 * SW7358-430: merge to main
 * 
 * Hydra_Software_Devel/SW7358-430/2   1/21/13 3:22p mpeteri
 * SW7358-430: remove warnings
 * 
 * Hydra_Software_Devel/SW7358-430/1   1/18/13 4:54p mpeteri
 * SW7358-430: add checks to wait for ramping to complete during mixer
 * changes
 * 
 * Hydra_Software_Devel/20   10/16/12 3:36p jgarrett
 * SW7346-643: Correcting default ramp step for all mixer inputs, not just
 * the first one
 * 
 * Hydra_Software_Devel/19   10/9/12 10:48p jgarrett
 * SW7418-116: Remove compiler warning
 * 
 * Hydra_Software_Devel/18   10/9/12 5:16p mpeteri
 * SW7418-116: merge to main
 * 
 * Hydra_Software_Devel/SW7418-116/3   10/9/12 5:00p mpeteri
 * SW7418-116: correct masking logic
 * 
 * Hydra_Software_Devel/SW7418-116/2   10/9/12 2:41p mpeteri
 * SW7418-116: Correct loop end point
 * 
 * Hydra_Software_Devel/17   9/25/12 1:45p mpeteri
 * SW7358-278: merge to main
 * 
 * Hydra_Software_Devel/SW7358-278/1   9/13/12 12:12p mpeteri
 * SW7358-278: pick up RBUF pause burst changes
 * 
 * Hydra_Software_Devel/REFSW_APE_MAI_PAUSEBURST/1   8/23/12 1:51p jgarrett
 * SW7358-78: Adding support for pauseburst insertion with HW cbits on MAI
 * 
 * Hydra_Software_Devel/16   6/12/12 9:43a jgarrett
 * SW7425-3220: Correcting coefficient change detection logic
 * 
 * Hydra_Software_Devel/15   6/11/12 5:27p jgarrett
 * SW7425-3220: Resolving Coverity 6 issues
 * 
 * Hydra_Software_Devel/14   4/30/12 11:52a jgarrett
 * SW7358-278: Revising compressed mute logic on chips without per-output
 * ramp disabling
 * 
 * Hydra_Software_Devel/13   2/23/12 10:53a jgarrett
 * SW7405-5003: Removing BKNI_Memcmp where structures aren't copied with
 * BKNI_Memcpy
 * 
 * Hydra_Software_Devel/12   2/22/12 10:17a gskerl
 * SW7425-2094: Added initialization code to disable volume ramp at zero
 * crossing. This will insure that volume ramping happens at each sample.
 * 
 * Hydra_Software_Devel/11   1/9/12 5:39p jgarrett
 * SW7425-2097: Correcting mixer input address on 7429
 * 
 * Hydra_Software_Devel/10   11/15/11 6:38p gskerl
 * SW7429-18: Removed a couple of #warnings that were leftover from a
 * previous merge.
 * 
 * Hydra_Software_Devel/9   11/14/11 3:42p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/2   11/8/11 11:22a gskerl
 * SW7429-18: Fixed sanity check for ping coefficients regOffset.
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/8   11/1/11 2:43p gskerl
 * SW7425-1647: Changed default OutputVolumeRampStep from 0x10000 to
 * 0xA00.
 * 
 * Hydra_Software_Devel/7   11/1/11 2:34p gskerl
 * SW7231-129: Added power standby/resume support for
 * OutputVolumeRampStep.
 * 
 * Hydra_Software_Devel/6   9/16/11 6:52p gskerl
 * SW7231-129: Refactored to put hardware and software initialization into
 * separate functions.
 * 
 * Hydra_Software_Devel/5   5/13/11 4:14p jgarrett
 * SW7425-212: Fixing multichannel DP output mute handling
 * 
 * Hydra_Software_Devel/4   5/4/11 4:07p jgarrett
 * SW7425-469: Enabling only connected output channel pairs.  Resolves bug
 * where 5.1 PCM is unstable for HDMI since the channel status formatter
 * only takes one channel pair as input instead of 3
 * 
 * Hydra_Software_Devel/3   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/2   4/12/11 5:34p jgarrett
 * SW7425-292: Fixing input mute and preserving output mute across
 * start/stop
 * 
 * Hydra_Software_Devel/1   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/2   4/5/11 7:14p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"
#include "bchp_aud_fmm_dp_ctrl0.h"

BDBG_MODULE(bape_dp_priv);

#define BAPE_PLAYBACK_ID_INVALID (0xffffffff)

#define BAPE_MIXER_DEFAULT_RAMP_STEP (0xa00)
#define BAPE_DP_MAX_RAMP_STEP (0x0800000)

/***************************************************************************
Summary:
Mixer Group
***************************************************************************/
typedef struct BAPE_MixerGroup
{
    bool allocated;
    uint8_t blockId;
    unsigned numChannelPairs;
    BAPE_Handle deviceHandle;
    uint32_t mixerIds[BAPE_ChannelPair_eMax];
    unsigned numRunningInputs;
    struct
    {
        BAPE_MixerGroupInputSettings settings;
        unsigned numChannelPairs;
        uint32_t playbackIds[BAPE_ChannelPair_eMax];
        bool started;
        bool linked;
    } inputs[BAPE_CHIP_MAX_MIXER_INPUTS];
    unsigned numRunningOutputs;
    struct
    {
        BAPE_MixerGroupOutputSettings settings;     
        bool started;
    } outputs[BAPE_CHIP_MAX_MIXER_OUTPUTS];
    BAPE_MixerGroupSettings settings;
} BAPE_MixerGroup;


static void BAPE_DpMixer_P_SetGroup(BAPE_Handle deviceHandle, uint32_t mixerId, uint32_t groupId);
static void BAPE_DpMixer_P_LoadInputCoefs(BAPE_Handle deviceHandle, uint32_t mixerId, unsigned inputIndex, 
                                          uint32_t leftToLeft, uint32_t rightToLeft, uint32_t leftToRight, uint32_t rightToRight, uint32_t rampStep);
static void BAPE_DpMixer_P_LoadOutputCoefs(BAPE_Handle deviceHandle, uint32_t mixerId, unsigned outputIndex, uint32_t left, uint32_t right);

static BERR_Code BAPE_MixerGroup_P_LinkInput(BAPE_MixerGroupHandle handle, unsigned inputIndex);
static void BAPE_MixerGroup_P_UnlinkInput(BAPE_MixerGroupHandle handle, unsigned inputIndex);
static void BAPE_MixerGroup_P_ApplyInputCoefficients(BAPE_MixerGroupHandle handle, unsigned inputIndex);
static void BAPE_MixerGroup_P_ApplyOutputCoefficients(BAPE_MixerGroupHandle handle, unsigned outputIndex);
static uint32_t BAPE_DpMixer_P_GetConfigAddress(unsigned mixerId);
static uint32_t BAPE_DpMixer_P_GetInputConfigAddress(unsigned mixerId, unsigned inputId);
#if !(defined BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG_VOLUME_RAMP_DISABLE_OUTPUT0_MASK || defined BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_VOLUME_RAMP_ENA_OUTPUT0_MASK)
static bool BAPE_DpMixer_P_MixerOutputIsActive(BAPE_MixerGroupHandle handle, unsigned mixerIndex);
static uint32_t BAPE_DpMixer_P_GetActiveMixerInputs(BAPE_MixerGroupHandle handle, unsigned mixerIndex);
static bool BAPE_DpMixer_P_MixerRampingActive(BAPE_MixerGroupHandle handle);
#endif



/***************************************************************************
Summary:
Initialize the DP block data structures
***************************************************************************/
BERR_Code BAPE_P_InitDpSw(
    BAPE_Handle handle
    )
{
    unsigned i;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

    /* TODO: Register for ESR's to catch HW Failures?? */

    /* Setup Group Handles */
    BDBG_MSG(("Allocating %u MIXER Groups", BAPE_CHIP_MAX_MIXER_GROUPS));
    handle->mixerGroups[0] = BKNI_Malloc(BAPE_CHIP_MAX_MIXER_GROUPS*sizeof(BAPE_MixerGroup));
    if ( NULL == handle->mixerGroups[0] )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle->mixerGroups[0], 0, BAPE_CHIP_MAX_MIXER_GROUPS*sizeof(BAPE_MixerGroup));
    for ( i = 1; i < BAPE_CHIP_MAX_MIXER_GROUPS; i++ )
    {
        handle->mixerGroups[i] = handle->mixerGroups[0] + i;
    }

    /* Set the default ramp step value. */
    handle->outputVolumeRampStep = BAPE_MIXER_DEFAULT_RAMP_STEP;

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Initialize the DP block
***************************************************************************/
BERR_Code BAPE_P_InitDpHw(
    BAPE_Handle handle
    )
{
    uint32_t regAddr;
    BREG_Handle regHandle;
    unsigned i, j;

    BDBG_OBJECT_ASSERT(handle, BAPE_Device);

    regHandle = handle->regHandle;

    BDBG_MSG(("Clearing all DP registers"));

    for ( i = 0; i < BAPE_CHIP_MAX_MIXERS; i++ )
    {
        BAPE_DpMixer_P_SetGroup(handle, i, i);
        for ( j = 0; j < BAPE_CHIP_MAX_MIXER_INPUTS; j++ )
        {
            BAPE_DpMixer_P_LoadInputCoefs(handle, i, j, 0, 0, 0, 0, BAPE_MIXER_DEFAULT_RAMP_STEP);
        }
        for ( j = 0; j < BAPE_CHIP_MAX_MIXER_OUTPUTS; j++ )
        {
            BAPE_DpMixer_P_LoadOutputCoefs(handle, i, j, 0, 0);
        }
        /* Make sure to disable the "volume ramp at zero cross" setting (if it exists). */
        regAddr = BAPE_DpMixer_P_GetConfigAddress(i);
        #if defined BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG_DISABLE_VOL_RAMP_AT_ZERO_CROSS_MASK
                BAPE_Reg_P_UpdateEnum(handle, regAddr, AUD_FMM_DP_CTRL0_MIXER0_CONFIG, DISABLE_VOL_RAMP_AT_ZERO_CROSS, Disable);
        #elif defined BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi
                BAPE_Reg_P_UpdateEnum(handle, regAddr, AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOL_RAMP_AT_ZERO_CROSS_ENA, Disable);
        #endif
    }

#ifdef BCHP_AUD_FMM_DP_CTRL0_PING_COEFF_RAMP_STEPi_ARRAY_BASE
    regAddr = BCHP_AUD_FMM_DP_CTRL0_PING_COEFF_RAMP_STEPi_ARRAY_BASE;
    for ( i=BCHP_AUD_FMM_DP_CTRL0_PING_COEFF_RAMP_STEPi_ARRAY_START; i<=BCHP_AUD_FMM_DP_CTRL0_PING_COEFF_RAMP_STEPi_ARRAY_END; i++ )
    {
        BREG_Write32(regHandle, regAddr, BAPE_MIXER_DEFAULT_RAMP_STEP);
        regAddr += 4;
    }
#else
    BREG_Write32(regHandle, BCHP_AUD_FMM_DP_CTRL0_PING_COEFF_RAMP_STEP, BAPE_MIXER_DEFAULT_RAMP_STEP);
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE
#ifdef BCHP_AUD_FMM_DP_CTRL0_PONG_COEFF_RAMP_STEPi_ARRAY_BASE
    regAddr = BCHP_AUD_FMM_DP_CTRL0_PONG_COEFF_RAMP_STEPi_ARRAY_BASE;
    for ( i=BCHP_AUD_FMM_DP_CTRL0_PONG_COEFF_RAMP_STEPi_ARRAY_START; i<=BCHP_AUD_FMM_DP_CTRL0_PONG_COEFF_RAMP_STEPi_ARRAY_END; i++ )
    {
        BREG_Write32(regHandle, regAddr, BAPE_MIXER_DEFAULT_RAMP_STEP);
        regAddr += 4;
    }
#else
    BREG_Write32(regHandle, BCHP_AUD_FMM_DP_CTRL0_PONG_COEFF_RAMP_STEP, BAPE_MIXER_DEFAULT_RAMP_STEP);
#endif
#endif

    BDBG_MSG(("Setting RAMP STEP to %lu", handle->outputVolumeRampStep));
    (void)BAPE_SetOutputVolumeRampStep(handle, handle->outputVolumeRampStep);

   /* Init Playback FCI ID's to Invalid */
    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_PLAYBACKS; i++ )
    {
        regAddr = BCHP_AUD_FMM_DP_CTRL0_PB_FCI_IDi_ARRAY_BASE + 4*(i);  
        BREG_Write32(regHandle, regAddr, BAPE_FCI_ID_INVALID);
    }

    /* TODO: Register for ESR's to catch HW Failures?? */

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Un-Initialize the DP block
***************************************************************************/
void BAPE_P_UninitDpSw(
    BAPE_Handle handle
    )
{
    if ( handle->mixerGroups[0] )
    {
        BKNI_Free(handle->mixerGroups[0]);
        BKNI_Memset(handle->mixerGroups, 0, sizeof(handle->mixerGroups));
    }
}

static void BAPE_DpMixer_P_SetGroup(
    BAPE_Handle deviceHandle, 
    uint32_t mixerId, 
    uint32_t groupId
    )
{
    uint32_t regAddr, regVal;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    regAddr = BAPE_DpMixer_P_GetConfigAddress(mixerId);
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG
    regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, MIXER_GROUP_BEGIN);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, MIXER_GROUP_BEGIN, groupId);
#else
    regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, MIXER_GROUP_BEGIN);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, MIXER_GROUP_BEGIN, groupId);    
#endif
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);
}

void BAPE_MixerGroup_P_GetDefaultCreateSettings(
    BAPE_MixerGroupCreateSettings *pSettings    /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    pSettings->numChannelPairs = 1;
    pSettings->blockId = 0;
}

BERR_Code BAPE_MixerGroup_P_Create(
    BAPE_Handle deviceHandle,
    const BAPE_MixerGroupCreateSettings *pSettings,
    BAPE_MixerGroupHandle *pHandle  /* [out] */
    )
{
    BERR_Code errCode;
    unsigned i, j, mixer;
    BAPE_MixerGroupHandle handle=NULL;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);
    BDBG_ASSERT(pSettings->numChannelPairs <= BAPE_ChannelPair_eMax);
    BDBG_ASSERT(pSettings->blockId == 0);   /* TODO: Handle more than one */

    /* Find an available group handle */
    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_GROUPS; i++ )
    {
        BDBG_ASSERT(NULL != deviceHandle->mixerGroups[i]);
        if ( !deviceHandle->mixerGroups[i]->allocated )
        {
            handle = deviceHandle->mixerGroups[i];
            break;
        }
    }

    /* If none found, return error */
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Now search for the correct number of resources */
    errCode = BAPE_P_AllocateFmmResource(deviceHandle, BAPE_FmmResourceType_eMixer, pSettings->numChannelPairs, &mixer);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_alloc_mixer;
    }

    /* Successfully allocated resources.  Initialize Group */
    BKNI_Memset(handle, 0, sizeof(BAPE_MixerGroup));
    handle->allocated = true;
    handle->blockId = pSettings->blockId;
    handle->numChannelPairs = pSettings->numChannelPairs;
    handle->deviceHandle = deviceHandle;
    handle->settings.volumeControlEnabled = true;
    BKNI_Memset(handle->mixerIds, 0xff, sizeof(handle->mixerIds));
    for ( i = 0; i < pSettings->numChannelPairs; i++ )
    {
        handle->mixerIds[i] = mixer + i;
        /* Setup Grouping */
        BAPE_DpMixer_P_SetGroup(deviceHandle, mixer + i, mixer);
    }
    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
    {
        BAPE_FciIdGroup_Init(&handle->inputs[i].settings.input);
        handle->inputs[i].settings.rampStep = BAPE_MIXER_DEFAULT_RAMP_STEP;
        for ( j = 0; j < pSettings->numChannelPairs; j++ )
        {
            handle->inputs[i].settings.coefficients[j][0][0] = BAPE_VOLUME_NORMAL;
            handle->inputs[i].settings.coefficients[j][1][1] = BAPE_VOLUME_NORMAL;
        }
        BKNI_Memset(handle->inputs[i].playbackIds, 0xff, sizeof(handle->inputs[i].playbackIds));
    }
    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_OUTPUTS; i++ )
    {
        for ( j = 0; j < 2*pSettings->numChannelPairs; j++ )
        {
            handle->outputs[i].settings.coefficients[j] = BAPE_VOLUME_NORMAL;
        }
    }

    *pHandle = handle;
    return BERR_SUCCESS;

    err_alloc_mixer:
    return errCode; 
}


void BAPE_MixerGroup_P_Destroy(
    BAPE_MixerGroupHandle handle
    )
{
    unsigned i;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    BDBG_ASSERT(handle->numRunningInputs == 0);
    BDBG_ASSERT(handle->numRunningOutputs == 0);

    /* Make sure we release all playbacks to the pool */
    for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
    {
        if ( handle->inputs[i].linked )
        {
            BAPE_MixerGroup_P_UnlinkInput(handle, i);
        }
    }

    /* Make sure Mixer Group ID's are reset to un-grouped */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        BAPE_DpMixer_P_SetGroup(handle->deviceHandle, handle->mixerIds[i], handle->mixerIds[i]);
    }

    /* Release Resources */
    BAPE_P_FreeFmmResource(handle->deviceHandle, BAPE_FmmResourceType_eMixer, handle->numChannelPairs, handle->mixerIds[0]);
    BKNI_Memset(handle->mixerIds, 0xff, sizeof(handle->mixerIds));

    /* Done */
    handle->allocated = false;
}

void BAPE_MixerGroup_P_GetSettings(
    BAPE_MixerGroupHandle handle,
    BAPE_MixerGroupSettings *pSettings  /* [out] */
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    *pSettings = handle->settings;
}

BERR_Code BAPE_MixerGroup_P_SetSettings(
    BAPE_MixerGroupHandle handle,
    const BAPE_MixerGroupSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);

    /* Soft Limiting is not currently supported */
    if ( pSettings->softLimitEnabled )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( handle->numRunningOutputs > 0 )
    {
        uint32_t regVal, regAddr;
        unsigned i;
        /* Support changing VOLUME_ENA on the fly.  Others do not change. */
        for ( i = 0; i < handle->numChannelPairs; i++ )
        {
            regAddr = BAPE_DpMixer_P_GetConfigAddress(handle->mixerIds[i]);
            regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG
            regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_ENA);
            regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_ENA, pSettings->volumeControlEnabled?1:0);
#else
            regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOLUME_ENA);
            regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOLUME_ENA, pSettings->volumeControlEnabled?1:0);
#endif
            BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
        }       
    }
    handle->settings = *pSettings;

    return BERR_SUCCESS;
}

void BAPE_MixerGroup_P_GetInputSettings(
    BAPE_MixerGroupHandle handle,
    unsigned inputIndex,
    BAPE_MixerGroupInputSettings *pSettings  /* [out] */
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(inputIndex < BAPE_CHIP_MAX_MIXER_INPUTS);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = handle->inputs[inputIndex].settings;
}

static bool BAPE_MixerGroup_P_CoefficientsChanged(const BAPE_MixerGroupInputSettings *pSettings1, const BAPE_MixerGroupInputSettings *pSettings2)
{
    unsigned i,j,k;

    BDBG_ASSERT(NULL != pSettings1);
    BDBG_ASSERT(NULL != pSettings2);

    for ( i = 0; i < BAPE_ChannelPair_eMax; i++ )
    {
        for ( j = 0; j < 2; j++ )
        {
            for ( k = 0; k < 2; k++ )
            {
                if ( pSettings1->coefficients[i][j][k] != pSettings2->coefficients[i][j][k] )
                {
                    return true;
                }
            }
        }
    }

    return false;
}

BERR_Code BAPE_MixerGroup_P_SetInputSettings(
    BAPE_MixerGroupHandle handle,
    unsigned inputIndex,
    const BAPE_MixerGroupInputSettings *pSettings
    )
{
    BERR_Code errCode;
    unsigned numInputPairs=0;
    bool inputChanged = false;
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(inputIndex < BAPE_CHIP_MAX_MIXER_INPUTS);
    BDBG_ASSERT(NULL != pSettings);

    if ( handle->inputs[inputIndex].started )
    {
        /* We can't change FCI ID while running - only check volume */
        if ( BAPE_MixerGroup_P_CoefficientsChanged(&handle->inputs[inputIndex].settings, pSettings) )
        {
            /* Coefficients have changed.  Copy and update. */
            BKNI_Memcpy(handle->inputs[inputIndex].settings.coefficients, pSettings->coefficients, sizeof(pSettings->coefficients));
            BAPE_MixerGroup_P_ApplyInputCoefficients(handle, inputIndex);
        }
        return BERR_SUCCESS;
    }

    /* If not started, we need to check for an input change */
    if ( !BAPE_FciIdGroup_IsEqual(handle->inputs[inputIndex].settings.input, pSettings->input) )
    {
        /* Make sure new FCI ID Group is legal */
        numInputPairs = BAPE_FciIdGroup_GetNumChannelPairs(pSettings->input);
        if ( numInputPairs > handle->numChannelPairs )
        {
            BDBG_ERR(("Input %u has more channel pairs (%u) than mixers in the group (%u).", inputIndex, numInputPairs, handle->numChannelPairs));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        if ( handle->inputs[inputIndex].linked )
        {
            BAPE_MixerGroup_P_UnlinkInput(handle, inputIndex);
        }
        inputChanged = true;
    }

    /* Update stored settings */
    handle->inputs[inputIndex].settings = *pSettings;

    /* Setup Playback Binding if Required */
    if ( inputChanged )
    {
        handle->inputs[inputIndex].numChannelPairs = numInputPairs;
        if ( numInputPairs > 0 )
        {
            errCode = BAPE_MixerGroup_P_LinkInput(handle, inputIndex);
            if ( errCode )
            {
                /* Likely unable to retrieve resources.  Reset Stored FCI ID to default. */
                BAPE_FciIdGroup_Init(&handle->inputs[inputIndex].settings.input);
                handle->inputs[inputIndex].numChannelPairs = 0;
                return BERR_TRACE(errCode);
            }
        }
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_MixerGroup_P_LinkInput(
    BAPE_MixerGroupHandle handle, 
    unsigned inputIndex
    )
{
    unsigned i, j, playback=(unsigned)-1;
    BAPE_FciId firstFci;
    BAPE_Handle deviceHandle;
    BERR_Code errCode;
    uint32_t regAddr;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(inputIndex < BAPE_CHIP_MAX_MIXER_INPUTS);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    /* Make sure Free was previously called */
    BDBG_ASSERT(handle->inputs[inputIndex].linked == false);

    /* Check for no input */
    if ( handle->inputs[inputIndex].numChannelPairs == 0 )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Determine if FCI ID's are already used by another playback */
    firstFci = handle->inputs[inputIndex].settings.input.ids[0];
    for ( i = 0; i <= (BAPE_CHIP_MAX_MIXER_PLAYBACKS-(handle->inputs[inputIndex].numChannelPairs)); i++ )
    {
        if ( deviceHandle->playbackReferenceCount[i] > 0 &&
             deviceHandle->playbackFci[i] == firstFci )
        {
            /* We have found a match.  Sanity check that other group members are also present or we have an
               unrecoverable failure.  */
            for ( j = 1; j < handle->inputs[inputIndex].numChannelPairs; j++ )
            {
                if ( deviceHandle->playbackReferenceCount[i+j] == 0 ||
                     deviceHandle->playbackFci[i+j] != handle->inputs[inputIndex].settings.input.ids[j] )
                {
                    BDBG_ERR(("FCI ID %#x is already in use by DP Playback %u but does not follow proper grouping.", firstFci));
                    return BERR_TRACE(BERR_NOT_SUPPORTED);
                }
            }

            playback = i;
            goto update_reference_counts;
        }
    }

    /* If we reach here, we didn't find a playback already in use.  Allocate a new set. */
    errCode = BAPE_P_AllocateFmmResource(deviceHandle, BAPE_FmmResourceType_ePlayback, handle->inputs[inputIndex].numChannelPairs, &playback);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    update_reference_counts:
    /* Update Reference Counts */
    for ( j = 0; j < handle->inputs[inputIndex].numChannelPairs; j++ )
    {
        handle->inputs[inputIndex].playbackIds[j] = playback+j;
        if ( 0 == deviceHandle->playbackReferenceCount[playback+j] )
        {
            regAddr = BCHP_AUD_FMM_DP_CTRL0_PB_FCI_IDi_ARRAY_BASE + 4*(playback+j);
            BREG_Write32(deviceHandle->regHandle, regAddr, handle->inputs[inputIndex].settings.input.ids[j]);
            deviceHandle->playbackFci[playback+j] = handle->inputs[inputIndex].settings.input.ids[j];

        }
        deviceHandle->playbackReferenceCount[playback+j]++;
    }
    handle->inputs[inputIndex].linked = true;

    return BERR_SUCCESS;
}

static void BAPE_MixerGroup_P_UnlinkInput(
    BAPE_MixerGroupHandle handle, 
    unsigned inputIndex
    )
{
    unsigned i;
    uint32_t regAddr;
    BAPE_Handle deviceHandle;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(inputIndex < BAPE_CHIP_MAX_MIXER_INPUTS);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    /* Make sure Free was not previously called and we're not releasing a running resource */
    BDBG_ASSERT(handle->inputs[inputIndex].linked == true);
    BDBG_ASSERT(handle->inputs[inputIndex].started == false);

    for ( i = 0; i < handle->inputs[inputIndex].numChannelPairs; i++ )
    {
        unsigned playback = handle->inputs[inputIndex].playbackIds[i];
        /* If this is the last user of the playback, we will need to free it. */
        if ( deviceHandle->playbackReferenceCount[playback] == 1 )
        {
            /* Free this playback */
            BAPE_P_FreeFmmResource(handle->deviceHandle, BAPE_FmmResourceType_ePlayback, 1, playback);
            regAddr = BCHP_AUD_FMM_DP_CTRL0_PB_FCI_IDi_ARRAY_BASE + 4*(playback+i);
            deviceHandle->playbackFci[playback] = BAPE_FCI_ID_INVALID;
            BREG_Write32(deviceHandle->regHandle, regAddr, BAPE_FCI_ID_INVALID);
        }
        deviceHandle->playbackReferenceCount[playback]--;
    }
    handle->inputs[inputIndex].linked = false;
}

BERR_Code BAPE_MixerGroup_P_StartInput(
    BAPE_MixerGroupHandle handle,
    unsigned inputIndex
    )
{
    unsigned i;
    BAPE_Handle deviceHandle;
    uint32_t regVal, regAddr;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(inputIndex < BAPE_CHIP_MAX_MIXER_INPUTS);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    /* Make sure Free was not previously called and we're not starting twice */
    BDBG_ASSERT(handle->inputs[inputIndex].linked == true);
    BDBG_ASSERT(handle->inputs[inputIndex].started == false);

    /* Setup Input Linkage */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        uint32_t pbId;

        /* Handle "Type1 vs Type2 - see MIXER_GROUP_BEGIN field in RDB docs */
        pbId = (i < handle->inputs[inputIndex].numChannelPairs)?handle->inputs[inputIndex].playbackIds[i]:handle->inputs[inputIndex].playbackIds[handle->inputs[inputIndex].numChannelPairs-1];

        /* Correlate mixer ID to register base */
        regAddr = BAPE_DpMixer_P_GetInputConfigAddress(handle->mixerIds[i], inputIndex);
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);

#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
        if ( (inputIndex % 2) == 0 )
        {
            regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT0_PB_NUMBER);
            regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT0_PB_NUMBER, pbId);
        }
        else
        {
            regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT1_PB_NUMBER);
            regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT1_PB_NUMBER, pbId);
        }
#else
        regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi, MIXER_INPUT_PB_NUMBER);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi, MIXER_INPUT_PB_NUMBER, pbId);
#endif

        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);     
    }

    /* Apply Group-Level Settings */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
        /* Volume -- Add soft limit here also if later required. */
        regAddr = BAPE_DpMixer_P_GetConfigAddress(handle->mixerIds[i]);
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_ENA);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_ENA, handle->settings.volumeControlEnabled?1:0);
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
        /* Priority */
    #ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER_HIGH_PRIORITY
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER_HIGH_PRIORITY;
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_HIGH_PRIORITY, MIXER0_HIGH_PRIORITY)<<handle->mixerIds[i]);
        regVal |= (BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER_HIGH_PRIORITY, MIXER0_HIGH_PRIORITY, handle->settings.highPriority?1:0)<<handle->mixerIds[i]);
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
    #else
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA;
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal &= ~(BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_HIGH_PRIORITY)<<handle->mixerIds[i]);
        regVal |= (BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_HIGH_PRIORITY, handle->settings.highPriority?1:0)<<handle->mixerIds[i]);
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
    #endif
#else
        /* New 7429-style mixer combines these bits in the config register */
        regAddr = BAPE_DpMixer_P_GetConfigAddress(handle->mixerIds[i]);
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOLUME_ENA);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOLUME_ENA, handle->settings.volumeControlEnabled?1:0);
        regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, MIXER_HIGH_PRIORITY);
        regVal |= (BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, MIXER_HIGH_PRIORITY, handle->settings.highPriority?1:0)<<handle->mixerIds[i]);
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);        
#endif
    }           

    /* Apply Input Scaling Coefs */
    BAPE_MixerGroup_P_ApplyInputCoefficients(handle, inputIndex);

    /* Enable the mixer input */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        uint32_t bitmask;

        /* Find input config register (they are paired) */
        regAddr = BAPE_DpMixer_P_GetInputConfigAddress(handle->mixerIds[i], inputIndex);

#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
        if ( 0 == (inputIndex%2) )
        {
            /* Even.  Write the first enable. */
            bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT0_ENA);
        }
        else
        {
            /* Odd.  Write the second enable. */
            bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT1_ENA);
        }
#else
        bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi, MIXER_INPUT_ENA);
#endif

        BDBG_MSG(("Enabling mixer %u input port %u (PB %u FCI %u)", handle->mixerIds[i], inputIndex,
                  handle->inputs[inputIndex].playbackIds[i], 
                  handle->deviceHandle->playbackFci[handle->inputs[inputIndex].playbackIds[i]]));
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal |= bitmask;
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);     
    }

    handle->inputs[inputIndex].started = true;
    handle->numRunningInputs++;
    BDBG_MSG(("Mixer Group %#x now has %u running inputs", handle, handle->numRunningInputs));

    return BERR_SUCCESS;
}

void BAPE_MixerGroup_P_StopInput(
    BAPE_MixerGroupHandle handle,
    unsigned inputIndex
    )
{
    unsigned i;
    BAPE_Handle deviceHandle;
    uint32_t regVal, regAddr;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(inputIndex < BAPE_CHIP_MAX_MIXER_INPUTS);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    /* Make sure Free was not previously called and we're not stopping twice */
    BDBG_ASSERT(handle->inputs[inputIndex].linked == true);
    if ( false == handle->inputs[inputIndex].started )
    {
        return;
    }

    /* Disable the mixer input */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        uint32_t bitmask;

        /* Find input config register */
        regAddr = BAPE_DpMixer_P_GetInputConfigAddress(handle->mixerIds[i], inputIndex);
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
        if ( 0 == (inputIndex%2) )
        {
            /* Even.  Write the first enable. */
            bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT0_ENA);
        }
        else
        {
            /* Odd.  Write the second enable. */
            bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT1_ENA);
        }
#else
        bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi, MIXER_INPUT_ENA);
#endif

        /* wait for ramping to complete on this mixer group */
        if ( BAPE_MixerGroup_P_WaitForRamping(handle) == BERR_TIMEOUT )
        {
            BDBG_ERR(("WARNING - %s - Vol Ramp timed out...", __FUNCTION__));
        }

        BDBG_MSG(("Disabling mixer %u input port %u (PB %u FCI %u)", handle->mixerIds[i], inputIndex,
                  handle->inputs[inputIndex].playbackIds[i], 
                  handle->deviceHandle->playbackFci[handle->inputs[inputIndex].playbackIds[i]]));
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal &= ~bitmask;
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);     
    }

    handle->inputs[inputIndex].started = false;
    BDBG_ASSERT(handle->numRunningInputs > 0);
    handle->numRunningInputs--; 
}

void BAPE_MixerGroup_P_GetOutputSettings(
    BAPE_MixerGroupHandle handle,
    unsigned outputIndex,
    BAPE_MixerGroupOutputSettings *pSettings  /* [out] */
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(outputIndex < BAPE_CHIP_MAX_MIXER_OUTPUTS);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->outputs[outputIndex].settings;
}

BERR_Code BAPE_MixerGroup_P_SetOutputSettings(
    BAPE_MixerGroupHandle handle,
    unsigned outputIndex,
    const BAPE_MixerGroupOutputSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(outputIndex < BAPE_CHIP_MAX_MIXER_OUTPUTS);
    BDBG_ASSERT(NULL != pSettings);

    handle->outputs[outputIndex].settings = *pSettings;

    if ( handle->outputs[outputIndex].started )
    {
        BAPE_MixerGroup_P_ApplyOutputCoefficients(handle, outputIndex);
    }

    return BERR_SUCCESS;
}

void BAPE_MixerGroup_P_GetOutputStatus(
    BAPE_MixerGroupHandle handle,
    unsigned outputIndex,
    BAPE_MixerGroupOutputStatus *pStatus    /* [out] */
    )
{
    unsigned i;
    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(outputIndex < BAPE_CHIP_MAX_MIXER_OUTPUTS);
    BDBG_ASSERT(NULL != pStatus);

    BKNI_Memset(pStatus, 0, sizeof(BAPE_MixerGroupOutputStatus));
    if ( handle->outputs[outputIndex].started )
    {
        uint32_t rampStatus, regAddr;

#ifdef BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUS0
        /* Read back ramp status from mixer */
        regAddr = (outputIndex == 0) ? BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUS0 : BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUS1;
        rampStatus = BREG_Read32(handle->deviceHandle->regHandle, regAddr);

        for ( i = 0; i < handle->numChannelPairs; i++ )
        {
            uint32_t bitmask;
            bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUS0, MIXER0_LEFT_VOLUME_RAMP) << handle->mixerIds[i];
            pStatus->rampActive[2*i] = (rampStatus & bitmask) ? true : false;
            bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUS0, MIXER0_RIGHT_VOLUME_RAMP) << handle->mixerIds[i];
            pStatus->rampActive[(2*i)+1] = (rampStatus & bitmask) ? true : false;
        }
#else        
        regAddr = BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUSi_ARRAY_BASE + ((BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUSi_ARRAY_ELEMENT_SIZE/8)*outputIndex);
        rampStatus = BREG_Read32(handle->deviceHandle->regHandle, regAddr);

        for ( i = 0; i < handle->numChannelPairs; i++ )
        {
            uint32_t bitmask;
            bitmask = (outputIndex == 0) ? 
                BCHP_MASK(AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUSi, MIXER_OUTPUT0_LEFT_VOLUME_RAMP) :
                BCHP_MASK(AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUSi, MIXER_OUTPUT1_LEFT_VOLUME_RAMP);
            pStatus->rampActive[2*i] = (rampStatus & bitmask) ? true : false;
            bitmask = (outputIndex == 0) ? 
                BCHP_MASK(AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUSi, MIXER_OUTPUT0_RIGHT_VOLUME_RAMP) :
                BCHP_MASK(AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUSi, MIXER_OUTPUT1_RIGHT_VOLUME_RAMP);
            pStatus->rampActive[(2*i)+1] = (rampStatus & bitmask) ? true : false;
        }
#endif
    }
}

BERR_Code BAPE_MixerGroup_P_StartOutput(
    BAPE_MixerGroupHandle handle,
    unsigned outputIndex
    )
{
    unsigned i, outputChannelPairs;
    uint32_t regAddr, regVal, bitmask;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(outputIndex < BAPE_CHIP_MAX_MIXER_OUTPUTS);
    BDBG_ASSERT(handle->outputs[outputIndex].started == false);

    /* Refresh output scaling coefficients first */
    BAPE_MixerGroup_P_ApplyOutputCoefficients(handle, outputIndex);

    /* Start */
    if ( handle->outputs[outputIndex].settings.numChannelPairs == 0 )
    {
        outputChannelPairs = handle->numChannelPairs;
    }
    else
    {
        outputChannelPairs = handle->outputs[outputIndex].settings.numChannelPairs;
    }
    for ( i = 0; i < outputChannelPairs; i++ )
    {
        /* Start mixer output */
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA;
        bitmask = (0 == outputIndex) ?
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_OUTPUT0_ENA) :
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_OUTPUT1_ENA);

        BDBG_MSG(("Enabling mixer %u output port %u", handle->mixerIds[i], outputIndex));
        bitmask <<= handle->mixerIds[i];
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal |= bitmask;
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
#else
        regAddr = BAPE_DpMixer_P_GetConfigAddress(handle->mixerIds[i]);
        bitmask = (0 == outputIndex) ?
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, MIXER_OUTPUT0_ENA) :
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, MIXER_OUTPUT1_ENA);

        BDBG_MSG(("Enabling mixer %u output port %u", handle->mixerIds[i], outputIndex));
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal |= bitmask;
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
#endif
    }

    handle->outputs[outputIndex].started = true;
    handle->numRunningOutputs++;

    return BERR_SUCCESS;
}

void BAPE_MixerGroup_P_StopOutput(
    BAPE_MixerGroupHandle handle,
    unsigned outputIndex
    )
{
    unsigned i;
    uint32_t regAddr, regVal, bitmask;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(outputIndex < BAPE_CHIP_MAX_MIXER_OUTPUTS);
    if ( handle->outputs[outputIndex].started == false )
    {
        return;
    }

    /* Stop */
    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        /* Stop mixer output */
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA;
        bitmask = (0 == outputIndex) ?
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_OUTPUT0_ENA) :
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA, MIXER0_OUTPUT1_ENA);

        BDBG_MSG(("Disabling mixer %u output port %u", handle->mixerIds[i], outputIndex));
        bitmask <<= handle->mixerIds[i];
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal &= ~bitmask;
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
#else
        regAddr = BAPE_DpMixer_P_GetConfigAddress(handle->mixerIds[i]);
        bitmask = (0 == outputIndex) ?
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, MIXER_OUTPUT0_ENA) :
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, MIXER_OUTPUT1_ENA);

        BDBG_MSG(("Disabling mixer %u output port %u", handle->mixerIds[i], outputIndex));
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal &= ~bitmask;
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
#endif
    }

    handle->outputs[outputIndex].started = false;
    BDBG_ASSERT(handle->numRunningOutputs > 0);
    handle->numRunningOutputs--;
}

void BAPE_MixerGroup_P_GetOutputFciIds(
    BAPE_MixerGroupHandle handle,
    unsigned outputIndex,
    BAPE_FciIdGroup *pFciGroup      /* [out] */
    )
{
    unsigned i;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(handle->allocated);
    BDBG_ASSERT(outputIndex < BAPE_CHIP_MAX_MIXER_OUTPUTS);
    BDBG_ASSERT(NULL != pFciGroup);

    BAPE_FciIdGroup_Init(pFciGroup);

    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        pFciGroup->ids[i] = ((0x4<<6)|(handle->mixerIds[i]*BAPE_CHIP_MAX_MIXER_OUTPUTS)|outputIndex);
    }
}

static void BAPE_DpMixer_P_LoadInputCoefs(
    BAPE_Handle deviceHandle, 
    uint32_t mixerId, 
    unsigned inputIndex, 
    uint32_t left0, 
    uint32_t right0, 
    uint32_t left1, 
    uint32_t right1,
    uint32_t rampStep
    )
{
    uint32_t regVal, regAddr, regOffset;
    uint32_t mixerNum, portNum;
    unsigned i;

    mixerNum = mixerId;
    portNum = inputIndex;

#ifdef BCHP_AUD_FMM_DP_CTRL0_USE_NEW_SCALING_COEFF
    /* On chips without a ping/pong setup - program the mixer to freeze the coefficients while updating */
    regVal = BREG_Read32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_USE_NEW_SCALING_COEFF);
    regVal &= ~(1<<mixerNum);
    BREG_Write32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_USE_NEW_SCALING_COEFF, regVal);
#endif

    /* -- From the RDB for 7408: (Same for other chips except for number of mixers, inputs, and coefficients)
    Lout = Left_Coef_0 * Lin + Right_Coef_0 * Rin;;;;  Rout = Left_Coef_1 * Lin + Right_Coef_1 * Rin.
    For input M of mixer N (where M ranges from 0 to 3 and N are ranges from 0 to 5), the index to the scaling coefficient array is at follow:
    Left_Coef_0 = coef[N*16 + M*4 + 0], Right_Coef_0 = coef[N*16 + M*4 + 1];;;;  Left_Coef_1 = coef[N*16 + M*4 + 2], Right_Coef_1 = coef[N*16 + M*4 + 3],
    For example, the scaling coefficients for input 0 of mixer 0 are
    Left_Coef_0 = coef[0], Right_Coef_0 = coef[1];;;;  Left_Coef_1 = coef[2], Right_Coef_1 = coef[3].
    The scaling coefficients for input 3 of mixer 1 are
    Left_Coef_0 = coef[28], Right_Coef_0 = coef[29];;;;  Left_Coef_1 = coef[30], Right_Coef_1 = coef[31].
    */

    /* Compute left coefficient 0 offset */
    regOffset = (mixerNum * 4 * BAPE_CHIP_MAX_MIXER_INPUTS) + (portNum * 4);
    BDBG_ASSERT(regOffset <= (BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_END-3));   /* Sanity check */

    /* Setup Ramp Steps prior to setting coefficients */   
    for ( i = 0; i < 4; i++ )
    {
        regAddr = (regOffset + i) * 4;
        BREG_Write32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_PING_COEFF_RAMP_STEPi_ARRAY_BASE+regAddr, rampStep);
        BREG_Write32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_PONG_COEFF_RAMP_STEPi_ARRAY_BASE+regAddr, rampStep);
    }

    /* Setup Volume Coefs */    
    regAddr = (regOffset * 4) + BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE;
    regVal = left0 & BCHP_MASK(AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi, COEFFICIENTS);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);
#ifdef BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE
    /* Write pong coef also */
    BDBG_CASSERT(BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE > BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE);
    BREG_Write32(deviceHandle->regHandle, regAddr + (BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE-BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE), regVal);
#endif

    regAddr += 4;   /* Jump to Right_Coef_0 */
    regVal = right0 & BCHP_MASK(AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi, COEFFICIENTS);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);
#ifdef BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE
    /* Write pong coef also */
    BREG_Write32(deviceHandle->regHandle, regAddr + (BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE-BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE), regVal);
#endif

    regAddr += 4;   /* Jump to Left_Coef_1 */
    regVal = left1 & BCHP_MASK(AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi, COEFFICIENTS);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);
#ifdef BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE
    /* Write pong coef also */
    BREG_Write32(deviceHandle->regHandle, regAddr + (BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE-BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE), regVal);
#endif

    regAddr += 4;   /* Jump to Right_Coef_1 */
    regVal = right1 & BCHP_MASK(AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi, COEFFICIENTS);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);
#ifdef BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE
    /* Write pong coef also */
    BREG_Write32(deviceHandle->regHandle, regAddr + (BCHP_AUD_FMM_DP_CTRL0_PONG_COEFFICIENTSi_ARRAY_BASE-BCHP_AUD_FMM_DP_CTRL0_PING_COEFFICIENTSi_ARRAY_BASE), regVal);
#endif

#ifdef BCHP_AUD_FMM_DP_CTRL0_USE_NEW_SCALING_COEFF
    /* On chips without a ping/pong setup - program the mixer to load the coefficients after updating */
    regVal = BREG_Read32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_USE_NEW_SCALING_COEFF);
    regVal |= (1<<mixerNum);
    BREG_Write32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_USE_NEW_SCALING_COEFF, regVal);
#endif
}

static void BAPE_MixerGroup_P_ApplyInputCoefficients(
    BAPE_MixerGroupHandle handle, 
    unsigned inputIndex
    )
{
    unsigned i;
    int32_t leftToLeft, rightToLeft, leftToRight, rightToRight;

    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        /* For now, cloned input channel pairs when input res < output res to be zero.
           If need be, these could be passed from the caller instead */
        if ( handle->inputs[inputIndex].numChannelPairs < i )
        {
            leftToLeft = 0;
            rightToLeft = 0;
            leftToRight = 0;
            rightToRight = 0;
        }
        else
        {
            /* Index by [Channel Pair][input channel][output channel] */
            leftToLeft = handle->inputs[inputIndex].settings.coefficients[i][0][0];
            rightToLeft = handle->inputs[inputIndex].settings.coefficients[i][1][0];
            leftToRight = handle->inputs[inputIndex].settings.coefficients[i][0][1];
            rightToRight = handle->inputs[inputIndex].settings.coefficients[i][1][1];
        }
        BAPE_DpMixer_P_LoadInputCoefs(handle->deviceHandle, handle->mixerIds[i], inputIndex, 
                                      leftToLeft, rightToLeft, leftToRight, rightToRight, handle->inputs[inputIndex].settings.rampStep);
    }
}

static void BAPE_DpMixer_P_LoadOutputCoefs(
    BAPE_Handle deviceHandle, 
    uint32_t mixerId, 
    unsigned outputIndex, 
    uint32_t left, 
    uint32_t right
    )
{
    uint32_t regAddr, regVal;

#if defined BCHP_AUD_FMM_DP_CTRL0_MIXER00_0_RT_VOL_LEVEL
    const uint32_t rightRegOffset = BCHP_AUD_FMM_DP_CTRL0_MIXER00_0_RT_VOL_LEVEL-BCHP_AUD_FMM_DP_CTRL0_MIXER00_0_LT_VOL_LEVEL;
    const uint32_t mixerRegOffset = BCHP_AUD_FMM_DP_CTRL0_MIXER01_0_LT_VOL_LEVEL-BCHP_AUD_FMM_DP_CTRL0_MIXER00_0_LT_VOL_LEVEL;

    if ( 1 == outputIndex )
    {
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER00_1_LT_VOL_LEVEL;
    }
    else
    {
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER00_0_LT_VOL_LEVEL;
    }

    left = BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER00_0_LT_VOL_LEVEL, MIXER_OUTPUT_LEFT_VOLUME_LEVEL, left) &
           BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER00_0_LT_VOL_LEVEL, MIXER_OUTPUT_LEFT_VOLUME_LEVEL);
    right = BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER00_0_RT_VOL_LEVEL, MIXER_OUTPUT_RIGHT_VOLUME_LEVEL, right) &
            BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER00_0_RT_VOL_LEVEL, MIXER_OUTPUT_RIGHT_VOLUME_LEVEL);

    regAddr += mixerRegOffset * ((uint32_t) mixerId);
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER00_0_LT_VOL_LEVEL, MIXER_OUTPUT_LEFT_VOLUME_LEVEL);            
    regVal |= left;
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    regAddr += rightRegOffset;
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER00_0_RT_VOL_LEVEL, MIXER_OUTPUT_RIGHT_VOLUME_LEVEL);
    regVal |= right;
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);
#elif defined BCHP_AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL
    const uint32_t rightRegOffset = BCHP_AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL-BCHP_AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL;
    const uint32_t mixerRegOffset = BCHP_AUD_FMM_DP_CTRL0_MIXER10_LT_VOL_LEVEL-BCHP_AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL;

    if ( 1 == outputIndex )
    {
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER01_LT_VOL_LEVEL;
    }
    else
    {
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL;
    }

    left = BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL, MIXER_OUTPUT_LEFT_VOLUME_LEVEL, left) &
           BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL, MIXER_OUTPUT_LEFT_VOLUME_LEVEL);
    right = BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL, MIXER_OUTPUT_RIGHT_VOLUME_LEVEL, right) &
            BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL, MIXER_OUTPUT_RIGHT_VOLUME_LEVEL);

    regAddr += mixerRegOffset * ((uint32_t) mixerId);
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER00_LT_VOL_LEVEL, MIXER_OUTPUT_LEFT_VOLUME_LEVEL);            
    regVal |= left;
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    regAddr += rightRegOffset;
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER00_RT_VOL_LEVEL, MIXER_OUTPUT_RIGHT_VOLUME_LEVEL);
    regVal |= right;
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);
#elif defined BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_LT_VOL_LEVELi_ARRAY_BASE
    const uint32_t rightRegOffset = BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_RT_VOL_LEVELi_ARRAY_BASE - BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_LT_VOL_LEVELi_ARRAY_BASE;
    const uint32_t mixerRegOffset = BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_RT_VOL_LEVELi_ARRAY_ELEMENT_SIZE/8;

    if ( 1 == outputIndex )
    {
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT1_LT_VOL_LEVELi_ARRAY_BASE;
    }
    else
    {
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_LT_VOL_LEVELi_ARRAY_BASE;
    }

    left = BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_LT_VOL_LEVELi, MIXER_OUTPUT_LEFT_VOLUME_LEVEL, left) &
           BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_LT_VOL_LEVELi, MIXER_OUTPUT_LEFT_VOLUME_LEVEL);
    right = BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_RT_VOL_LEVELi, MIXER_OUTPUT_RIGHT_VOLUME_LEVEL, right) &
            BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_RT_VOL_LEVELi, MIXER_OUTPUT_RIGHT_VOLUME_LEVEL);

    regAddr += mixerRegOffset * ((uint32_t) mixerId);
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_LT_VOL_LEVELi, MIXER_OUTPUT_LEFT_VOLUME_LEVEL);            
    regVal |= left;
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    regAddr += rightRegOffset;
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_OUTPUT0_RT_VOL_LEVELi, MIXER_OUTPUT_RIGHT_VOLUME_LEVEL);
    regVal |= right;
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);    
#else
#error Mixer output volume registers undefined
#endif
}

#if !(defined BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG_VOLUME_RAMP_DISABLE_OUTPUT0_MASK || defined BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_VOLUME_RAMP_ENA_OUTPUT0_MASK)
static bool BAPE_DpMixer_P_MixerOutputIsActive( 
    BAPE_MixerGroupHandle handle, 
    unsigned mixerIndex
    )
{
    uint32_t enabledMixers;

    if ( mixerIndex < BAPE_CHIP_MAX_MIXERS )
    {
        enabledMixers = BREG_Read32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_MIXER_OUTPUT_ENA);
        enabledMixers = (enabledMixers % BAPE_CHIP_MAX_MIXERS) | (enabledMixers >> BAPE_CHIP_MAX_MIXERS); /* combine outputs 0/1 */

        return ( (enabledMixers>>mixerIndex) & 1 );
    }

    return false;
}

static uint32_t BAPE_DpMixer_P_GetActiveMixerInputs( 
    BAPE_MixerGroupHandle handle, 
    unsigned mixerIndex
    )
{
    uint32_t enabledInputs = 0;
    int i;

    if ( mixerIndex < BAPE_CHIP_MAX_MIXERS )
    {
        for ( i = 0; i < BAPE_CHIP_MAX_MIXER_INPUTS; i++ )
        {
            uint32_t bitmask;
            uint32_t regVal, regAddr;

            /* Find input config register (they are paired) */
            regAddr = BAPE_DpMixer_P_GetInputConfigAddress(mixerIndex, i);
            regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);

#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
            if ( 0 == (i%2) )
            {
                /* Even.  Read the first enable. */
                bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT0_ENA);
            }
            else
            {
                /* Odd.  Read the second enable. */
                bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG, MIXER_INPUT1_ENA);
            }
#else
            bitmask = BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi, MIXER_INPUT_ENA);
#endif

            if ( (regVal & bitmask) != 0 )
            {
                enabledInputs |= (1<<i);
            }
        }
        return enabledInputs;
    }

    return 0;
}

static bool BAPE_DpMixer_P_MixerRampingActive( 
    BAPE_MixerGroupHandle handle
    )
{
    int i;
    uint32_t rampMask;

    rampMask = ( BREG_Read32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUS0) |
                 BREG_Read32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STATUS1) );

    for ( i = 0; i < BAPE_CHIP_MAX_MIXERS; i++ )
    {
        if ( (BAPE_DpMixer_P_GetActiveMixerInputs(handle, i) > 0) &&
              BAPE_DpMixer_P_MixerOutputIsActive(handle, i) &&
              (rampMask & (1<<i | 1<<(i+BAPE_CHIP_MAX_MIXERS))) )
        {
            return true;
        }                
    }

    return false;
}
#endif

BERR_Code BAPE_MixerGroup_P_WaitForRamping(
    BAPE_MixerGroupHandle handle
    )
{
#if !(defined BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG_VOLUME_RAMP_DISABLE_OUTPUT0_MASK || defined BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_VOLUME_RAMP_ENA_OUTPUT0_MASK)
    unsigned timeout=1000;

    /* Spin for all pending ramps to finish */
    while ( BAPE_DpMixer_P_MixerRampingActive(handle) &&
            timeout > 0 )
    {
        BDBG_MSG(("Waiting for ramp to complete..."));
        BKNI_Sleep(1);
        timeout--;
    }

    if ( timeout == 0 )
    {
        return BERR_TIMEOUT;
    }
#else
    BSTD_UNUSED(handle);
#endif

    return BERR_SUCCESS;
}

static void BAPE_MixerGroup_P_ApplyOutputCoefficients(
    BAPE_MixerGroupHandle handle, 
    unsigned outputIndex
    )
{
    bool wasMuted;
    unsigned i;
    uint32_t configRegVal, configRegAddr, bitmask;
#if !(defined BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG_VOLUME_RAMP_DISABLE_OUTPUT0_MASK || defined BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_VOLUME_RAMP_ENA_OUTPUT0_MASK)
    uint32_t oldRampStep=0;
#endif

    /* Cases to be handled.  */
    /* Mute -> Unmute - Load Coefs first, then unmute so mixer ramps properly. */
    /* Unmute -> Mute - Set Mute First, starts ramping immediately.  Then set coefs. */
    /* Unmute -> Unmute (change in coefs only) */

#if !(defined BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG_VOLUME_RAMP_DISABLE_OUTPUT0_MASK || defined BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_VOLUME_RAMP_ENA_OUTPUT0_MASK)
    /* Ugh.  No direct support for disabling output ramp.  Need to spin while any pending ramp is going on, jam the ramp step to max, change vol, restore */
    if ( handle->outputs[outputIndex].settings.volumeRampDisabled )
    {
        if ( BAPE_MixerGroup_P_WaitForRamping(handle) == BERR_TIMEOUT )
        {
            BDBG_ERR(("WARNING - %s - Vol Ramp timed out...(DISABLE)", __FUNCTION__));
        }

        /* Set max ramp step value */
        BAPE_GetOutputVolumeRampStep(handle->deviceHandle, &oldRampStep);
        BAPE_SetOutputVolumeRampStep(handle->deviceHandle, BAPE_DP_MAX_RAMP_STEP);
    }
#endif

    for ( i = 0; i < handle->numChannelPairs; i++ )
    {
        /* Get Current Mute Status */
        configRegAddr = BAPE_DpMixer_P_GetConfigAddress(handle->mixerIds[i]);
        configRegVal = BREG_Read32(handle->deviceHandle->regHandle, configRegAddr);
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG
        bitmask = (outputIndex == 0) ?
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_MUTE_ENA_OUTPUT0):
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_MUTE_ENA_OUTPUT1);
#else
        bitmask = (outputIndex == 0) ?
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOLUME_MUTE_ENA_OUTPUT0):
                  BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOLUME_MUTE_ENA_OUTPUT1);
#endif
        wasMuted = (configRegVal & bitmask) ? true : false;

        /* Handle Ramp Disable First */
#if defined BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG_VOLUME_RAMP_DISABLE_OUTPUT0_MASK
        /* Sweet, the mixer supports it on a per-output basis. */
        if ( outputIndex == 0 )
        {
            configRegVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_RAMP_DISABLE_OUTPUT0);
            configRegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_RAMP_DISABLE_OUTPUT0, handle->outputs[outputIndex].settings.volumeRampDisabled?1:0);
        }
        else
        {
            configRegVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_RAMP_DISABLE_OUTPUT1);
            configRegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER0_CONFIG, VOLUME_RAMP_DISABLE_OUTPUT1, handle->outputs[outputIndex].settings.volumeRampDisabled?1:0);
        }
#elif defined BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_VOLUME_RAMP_ENA_OUTPUT0_MASK
        /* Sweet, the mixer supports it on a per-output basis. */
        if ( outputIndex == 0 )
        {
            configRegVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOLUME_RAMP_ENA_OUTPUT0);
            configRegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOLUME_RAMP_ENA_OUTPUT0, handle->outputs[outputIndex].settings.volumeRampDisabled?0:1);
        }
        else
        {
            configRegVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOLUME_RAMP_ENA_OUTPUT0);
            configRegVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_MIXER_CONFIGi, VOLUME_RAMP_ENA_OUTPUT0, handle->outputs[outputIndex].settings.volumeRampDisabled?0:1);
        }        
#endif

        /* Determine if we're going to mute and if so do that first to start the ramp. */
        if ( !wasMuted && handle->outputs[outputIndex].settings.muted )
        {
            configRegVal |= bitmask;
        }

        /* Update config register with any changes */
        BREG_Write32(handle->deviceHandle->regHandle, configRegAddr, configRegVal);

        /* Now set coefficients */
        if ( !handle->outputs[outputIndex].settings.muted )
        {
            BAPE_DpMixer_P_LoadOutputCoefs(handle->deviceHandle, handle->mixerIds[i], outputIndex, 
                                           handle->outputs[outputIndex].settings.coefficients[2*i],
                                           handle->outputs[outputIndex].settings.coefficients[(2*i)+1]);

            if ( wasMuted )
            {
                /* Done with coefficients.  Now Unmute if we need to. */
                configRegVal &= ~bitmask;
                BREG_Write32(handle->deviceHandle->regHandle, configRegAddr, configRegVal);
            }
        }
    }

    /* Restore ramp step if required. */
#if !(defined BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG_VOLUME_RAMP_DISABLE_OUTPUT0_MASK || defined BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_VOLUME_RAMP_ENA_OUTPUT0_MASK)
    if ( handle->outputs[outputIndex].settings.volumeRampDisabled )
    {
        if ( BAPE_MixerGroup_P_WaitForRamping(handle) == BERR_TIMEOUT )
        {
            BDBG_ERR(("WARNING - %s - Vol Ramp timed out...(RESTORE)", __FUNCTION__));
        }

        /* Restore original value */
        BAPE_SetOutputVolumeRampStep(handle->deviceHandle, oldRampStep);
    }
#endif
}

void BAPE_GetOutputVolumeRampStep(
    BAPE_Handle deviceHandle,
    uint32_t *pRampStep                 /* All mixers output output volume is changed by this amount
                                                                            every Fs while ramping.  Specified in 4.23 format. 
                                                                            Ignored for compressed data. */
    )
{
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pRampStep);

    *pRampStep = deviceHandle->outputVolumeRampStep;
    return;
}

BERR_Code BAPE_SetOutputVolumeRampStep(
    BAPE_Handle deviceHandle,
    uint32_t rampStep                   /* All mixers output output volume is changed by this amount
                                                                                 every Fs while ramping.  Specified in 4.23 format. 
                                                                                 Ignored for compressed data. */
    )
{
    uint32_t regVal;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

#if defined BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_UP_STEP
    regVal = BREG_Read32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_UP_STEP);
    regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_VOLUME_RAMP_UP_STEP, VOLUME_RAMP_UP_STEP);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_VOLUME_RAMP_UP_STEP, VOLUME_RAMP_UP_STEP, rampStep);
    BREG_Write32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_UP_STEP, regVal);
    BREG_Write32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_DOWN_STEP, regVal);

    /* Save the setting in the BAPE_Device struct.  Use the value from the register field in case the user's value was truncated. */
    deviceHandle->outputVolumeRampStep = BCHP_GET_FIELD_DATA(regVal, AUD_FMM_DP_CTRL0_VOLUME_RAMP_UP_STEP, VOLUME_RAMP_UP_STEP);

#elif defined BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP
    regVal = BREG_Read32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP);
    regVal &= ~BCHP_MASK(AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP, VOLUME_RAMP_STEP);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP, VOLUME_RAMP_STEP, rampStep);
    BREG_Write32(deviceHandle->regHandle, BCHP_AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP, regVal);

    /* Save the setting in the BAPE_Device struct.  Use the value from the register field in case the user's value was truncated. */
    deviceHandle->outputVolumeRampStep = BCHP_GET_FIELD_DATA(regVal, AUD_FMM_DP_CTRL0_VOLUME_RAMP_STEP, VOLUME_RAMP_STEP);

#else
#error Mixer output ramp step not defined
#endif
    return BERR_SUCCESS;
}

static uint32_t BAPE_DpMixer_P_GetConfigAddress(
    unsigned mixerId
    )
{
    switch ( mixerId )
    {
    #if BCHP_CHIP == 7408
    case 5:
        /* 7408 made this discontiguous for some reason... */
        return BCHP_AUD_FMM_DP_CTRL0_MIXER5_CONFIG;
    #endif
    default:
        BDBG_ASSERT(mixerId < BAPE_CHIP_MAX_MIXERS);
    #ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG
        return BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG + ((BCHP_AUD_FMM_DP_CTRL0_MIXER1_CONFIG-BCHP_AUD_FMM_DP_CTRL0_MIXER0_CONFIG)*mixerId);
    #else
        return BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_ARRAY_BASE + ((BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_ARRAY_ELEMENT_SIZE/8)*mixerId);
    #endif
    }
}

static uint32_t BAPE_DpMixer_P_GetInputConfigAddress(
    unsigned mixerId,
    unsigned inputId
    )
{
    uint32_t regAddr;
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG
    regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG + (mixerId*(BCHP_AUD_FMM_DP_CTRL0_MIXER1_INPUT10_CONFIG - BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT10_CONFIG));
    regAddr += 4*(inputId/2);
#else
    switch ( mixerId )
    {
    case 0:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER1_INPUT_CONFIGi_ARRAY_BASE
    case 1:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER1_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER2_INPUT_CONFIGi_ARRAY_BASE
    case 2:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER2_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER3_INPUT_CONFIGi_ARRAY_BASE
    case 3:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER3_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER4_INPUT_CONFIGi_ARRAY_BASE
    case 4:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER4_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER5_INPUT_CONFIGi_ARRAY_BASE
    case 5:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER5_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER6_INPUT_CONFIGi_ARRAY_BASE
    case 6:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER6_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER7_INPUT_CONFIGi_ARRAY_BASE
    case 7:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER7_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER8_INPUT_CONFIGi_ARRAY_BASE
    case 8:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER8_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER9_INPUT_CONFIGi_ARRAY_BASE
    case 9:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER9_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER10_INPUT_CONFIGi_ARRAY_BASE
    case 10:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER10_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER11_INPUT_CONFIGi_ARRAY_BASE
    case 11:
        regAddr = BCHP_AUD_FMM_DP_CTRL0_MIXER11_INPUT_CONFIGi_ARRAY_BASE; 
        break;
#endif
#ifdef BCHP_AUD_FMM_DP_CTRL0_MIXER12_INPUT_CONFIGi_ARRAY_BASE
        #error need to add support for additional mixers
#endif
    default:
        BDBG_ASSERT(mixerId < BAPE_CHIP_MAX_MIXERS);
        return 0xffffffff;
    }
    regAddr += (BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi_ARRAY_ELEMENT_SIZE/8)*inputId;
#endif
    return regAddr;
}

