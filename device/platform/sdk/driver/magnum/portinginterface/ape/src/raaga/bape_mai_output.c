/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_mai_output.c $
 * $brcm_Revision: Hydra_Software_Devel/34 $
 * $brcm_Date: 1/23/13 4:28p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_mai_output.c $
 * 
 * Hydra_Software_Devel/34   1/23/13 4:28p mpeteri
 * SW7358-430: merge to main
 * 
 * Hydra_Software_Devel/SW7358-430/1   1/22/13 10:48a mpeteri
 * SW7358-430: Fix MAI enable sequence to correct PB mixing startup issue
 * 
 * Hydra_Software_Devel/33   10/22/12 12:24p jgarrett
 * SW7358-417: Only filling mute buffer on legacy aio systems
 * 
 * Hydra_Software_Devel/32   10/19/12 5:15p mpeteri
 * SW7429-295: merge to main
 * 
 * Hydra_Software_Devel/31   10/19/12 4:07p mpeteri
 * SW7358-417: merge to main
 * 
 * Hydra_Software_Devel/SW7358-417/2   10/10/12 12:00p mpeteri
 * SW7358-417: remove settings copy from SetSettings
 * 
 * Hydra_Software_Devel/SW7358-417/1   10/3/12 12:21p mpeteri
 * SW7358-417: Changes to allow NEXUS to configure HDMI audio burst
 * settings.
 * 
 * Hydra_Software_Devel/SW7429-295/1   10/19/12 11:43a mpeteri
 * SW7429-295: port DTS-CD changes to 7429
 * 
 * Hydra_Software_Devel/30   10/8/12 6:49a jgarrett
 * SW7425-4033: Adding standby handler for MAI output
 * 
 * Hydra_Software_Devel/29   10/8/12 5:19a jgarrett
 * SW7346-643: Disabling input ramp for data input to pauseburst mixer
 * 
 * Hydra_Software_Devel/28   9/28/12 11:34a jgarrett
 * SW7358-278: Resolve coverity CHECKED_RETURN
 * 
 * Hydra_Software_Devel/27   9/27/12 10:18a mpeteri
 * SW7445-19: merge 7445 APE changes to main
 * 
 * Hydra_Software_Devel/SW7445-19/2   9/26/12 3:44p mpeteri
 * SW7445-19: pick up latest from Hydra
 * 
 * Hydra_Software_Devel/26   9/25/12 1:45p mpeteri
 * SW7358-278: merge to main
 * 
 * Hydra_Software_Devel/SW7358-278/9   9/20/12 12:45p mpeteri
 * SW7358-278: changes to keep mixer and source fifo active at all times
 * when feeding MAI with stereo path
 * 
 * Hydra_Software_Devel/SW7358-278/8   9/13/12 3:25p mpeteri
 * SW7358-278: clean up HW cbit formatter enable sequence
 * 
 * Hydra_Software_Devel/SW7358-278/7   9/13/12 3:15p mpeteri
 * SW7358-278: fix merge error
 * 
 * Hydra_Software_Devel/SW7358-278/6   9/13/12 3:04p mpeteri
 * SW7358-278: pick up DTS CD changes from Hydra
 * 
 * Hydra_Software_Devel/25   9/11/12 3:35p jgarrett
 * SW7425-3796: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-3796/1   8/23/12 6:05p mpeteri
 * SW7425-3796: PI Changes to transmit original ES DTS-CD signal to
 * SPDIF/HDMI
 * 
 * Hydra_Software_Devel/SW7358-278/5   9/13/12 12:12p mpeteri
 * SW7358-278: pick up RBUF pause burst changes
 * 
 * Hydra_Software_Devel/REFSW_APE_MAI_PAUSEBURST/2   9/13/12 11:59a mpeteri
 * SW7358-278: merge fixes back to MAI PB branch
 * 
 * Hydra_Software_Devel/REFSW_APE_MAI_PAUSEBURST/1   8/23/12 1:51p jgarrett
 * SW7358-78: Adding support for pauseburst insertion with HW cbits on MAI
 * 
 * Hydra_Software_Devel/SW7445-19/1   8/17/12 5:49p mpeteri
 * SW7445-19: Add additional PLL and NCO for 7445
 * 
 * Hydra_Software_Devel/24   7/31/12 2:56p mpeteri
 * SW7429-209: merge to main
 * 
 * Hydra_Software_Devel/SW7429-209/1   7/31/12 2:50p mpeteri
 * SW7429-209: change clock source for 7429 to remove warning
 * 
 * Hydra_Software_Devel/23   7/13/12 5:56p jgarrett
 * SW7346-893: Fixing compile warning on 7429
 * 
 * Hydra_Software_Devel/22   7/13/12 3:48p jgarrett
 * SW7346-893, SW7231-900: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7346-893/6   7/13/12 3:29p jgarrett
 * SW7346-893, SW7231-900: Revised handling of MS_CTRL_STRM_ENA logic to
 * avoid lockups during cbit updates
 * 
 * Hydra_Software_Devel/SW7346-893/5   7/13/12 5:41p mpeteri
 * SW7346-893: fix up HBR configuration
 * 
 * Hydra_Software_Devel/SW7346-893/4   7/13/12 5:11p mpeteri
 * SW7346-893: fix up HBR configuration
 * 
 * Hydra_Software_Devel/21   7/6/12 11:45a jgarrett
 * SW7346-893: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7346-893/3   7/5/12 5:37p mpeteri
 * SW7346-893: Change to stereo source for stereo PCM content to enable
 * Dither
 * 
 * Hydra_Software_Devel/SW7346-893/2   7/3/12 3:42p jgarrett
 * SW7346-893: Removing extra debug messages
 * 
 * Hydra_Software_Devel/SW7346-893/1   7/3/12 12:01p jgarrett
 * SW7346-893: Setting legacy routing to enable stereo path for dithering
 * 
 * Hydra_Software_Devel/20   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/6   4/25/12 3:04p gskerl
 * SW7425-2075: Removed some extra code from
 * BAPE_MaiOutput_P_Disable_IopOut().
 * 
 * Hydra_Software_Devel/SW7425-2075/6   4/25/12 3:02p gskerl
 * SW7425-2075: Removed some extra code from
 * BAPE_MaiOutput_P_Disable_IopOut().
 * 
 * Hydra_Software_Devel/SW7425-2075/5   4/18/12 3:27p mpeteri
 * SW7425-2075: added changes for HBR test tone app support on 7429
 * 
 * Hydra_Software_Devel/SW7425-2075/4   4/12/12 3:46p jgarrett
 * SW7425-2075: Merging HBR changes
 * 
 * Hydra_Software_Devel/SW7425-2075/3   4/10/12 6:51p gskerl
 * SW7425-2075: Adding new FMT interfaces for 7429
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/6/12 6:29p jgarrett
 * SW7425-2075: Correcting CBITS and MAI_FORMAT programming
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/19   3/6/12 2:59p gskerl
 * SW7425-2570: Renamed OutputPortObject substruct from connector to
 * outputPort.
 * 
 * Hydra_Software_Devel/18   2/16/12 2:59p jgarrett
 * SW7552-188: Refactoring MAI channel status handling based on
 * recommendations from BD platforms
 * 
 * Hydra_Software_Devel/17   12/1/11 5:20p jgarrett
 * SW7429-18: Disabling multichannel output grouping for now on 7429
 * 
 * Hydra_Software_Devel/16   11/14/11 3:35p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/3   11/4/11 4:42p jgarrett
 * SW7429-18: Adding SPDIF Output for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/2   10/27/11 3:21p jgarrett
 * SW7429-18: Adding I2S Output for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/25/11 5:34p jgarrett
 * SW7429-18: Adding HDMI output support for 7429
 * 
 * Hydra_Software_Devel/15   10/5/11 12:38p gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume.
 * 
 * Hydra_Software_Devel/14   8/19/11 1:41p jgarrett
 * SW7344-170: Handling MS FREQ_EXT changes
 * 
 * Hydra_Software_Devel/13   7/8/11 4:25p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/12   7/7/11 10:53a jgarrett
 * SW7552-69: Added stereoMode setting
 * 
 * Hydra_Software_Devel/11   6/27/11 5:41p jgarrett
 * SW7231-97: Refactoring SPDIF/HDMI enable mechanisms to handle older
 * Onkyo receiver DTS->PCM switching requirements
 * 
 * Hydra_Software_Devel/10   5/20/11 5:16p jgarrett
 * SW7425-402: Adding error code to handle output enable callback failing.
 * 
 * Hydra_Software_Devel/9   5/3/11 10:46a gskerl
 * SW7422-354: Added index and type args to APE_P_InitOutputPort macro
 * 
 * Hydra_Software_Devel/8   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/7   4/12/11 5:34p jgarrett
 * SW7425-292: Fixing input mute and preserving output mute across
 * start/stop
 * 
 * Hydra_Software_Devel/6   4/6/11 1:23a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/2   4/5/11 7:13p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:49p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/5   3/22/11 2:56p gskerl
 * SW7422-146: Changed audio output connector callbacks to take the output
 * connector as an argument
 * 
 * Hydra_Software_Devel/4   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/3   2/16/11 4:51p jgarrett
 * SW7422-146: Coverity CID 251
 * 
 * Hydra_Software_Devel/2   2/10/11 5:45p gskerl
 * SW7422-146: Changed calling sequence for the audio output setMclk_isr
 * callbacks
 * 
 * Hydra_Software_Devel/1   12/16/10 4:05p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 ***************************************************************************/

#include "bape.h"
#include "bape_priv.h"

BDBG_MODULE(bape_mai_output);


#if BAPE_CHIP_MAX_MAI_OUTPUTS > 0   /* If no MAI outputs, then skip all of this and just put in stub funcs at bottom of file. */

BDBG_OBJECT_ID(BAPE_MaiOutput);

typedef enum BAPE_MaiOutputDataPath
{
    BAPE_MaiOutputDataPath_eNone,
    BAPE_MaiOutputDataPath_eStereo,
    BAPE_MaiOutputDataPath_eMaiMulti,
    BAPE_MaiOutputDataPath_eHbr,
    BAPE_MaiOutputDataPath_eMax
} BAPE_MaiOutputDataPath;

typedef struct BAPE_MaiOutput
{
    BDBG_OBJECT(BAPE_MaiOutput)
    BAPE_Handle deviceHandle;
    BAPE_MaiOutputSettings settings;
    unsigned index;
    BAPE_OutputPortObject outputPort;
    uint32_t offset;
    unsigned sampleRate;
    struct
    {
        BAPE_MclkSource mclkSource;
        unsigned pllChannel;    /* only applies if mclkSource refers to a PLL */
        unsigned mclkFreqToFsRatio;
    } mclkInfo;
    bool enabled;
    char name[7];   /* MAI %d */
    bool muted;
    BAPE_MaiOutputInterruptHandlers interrupts;

    /* The following are used to generate a pauseburst compressed mute on legacy chips */
    uint32_t *pMuteBuffer;
    BAPE_SfifoGroupHandle hSfifo;
    BAPE_MixerGroupHandle hMixer;
    BAPE_MaiOutputDataPath dataPath;
} BAPE_MaiOutput;

/* Currently all chips support only one -- TODO: HBR? */
#define GET_MAI_MULTI_STREAM_ID(idx, chPair) ((chPair)+3)
#define GET_MAI_STEREO_STREAM_ID(idx) (1)
#define GET_MAI_HBR_STREAM_ID(idx) (10)
#define GET_MAI_CBIT_STREAM_ID(idx) (1)

/* Static function prototypes */
static void      BAPE_MaiOutput_P_SetCbits_isr(BAPE_MaiOutputHandle handle);
static uint32_t  BAPE_MaiOutput_P_SampleRateToMaiFormat(unsigned sampleRate);
static BERR_Code BAPE_MaiOutput_P_OpenHw(BAPE_MaiOutputHandle handle);
static void      BAPE_MaiOutput_P_CloseHw(BAPE_MaiOutputHandle handle);
static BERR_Code BAPE_MaiOutput_P_ApplySettings(BAPE_MaiOutputHandle handle, const BAPE_MaiOutputSettings *pSettings, bool force);
static uint32_t  BAPE_MaiOutput_P_SampleRateToMaiFormat(unsigned sampleRate);

/* Output port callbacks */
static void      BAPE_MaiOutput_P_SetTimingParams_isr(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase);
static BERR_Code BAPE_MaiOutput_P_Enable(BAPE_OutputPort output);
static void      BAPE_MaiOutput_P_Disable(BAPE_OutputPort output);
static void      BAPE_MaiOutput_P_SetMclk_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio);
static void      BAPE_MaiOutput_P_SetMute(BAPE_OutputPort output, bool muted, bool sync);

/* Implementations for Legacy vs. New [7429] Hardware */
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
#include "bchp_aud_fmm_iop_out_mai_0.h"
static BERR_Code BAPE_MaiOutput_P_Open_IopOut(BAPE_MaiOutputHandle handle);
static void      BAPE_MaiOutput_P_Close_IopOut(BAPE_MaiOutputHandle handle);
static void      BAPE_MaiOutput_P_SetCbits_IopOut_isr(BAPE_MaiOutputHandle handle);
static BERR_Code BAPE_MaiOutput_P_Enable_IopOut(BAPE_OutputPort output);
static void      BAPE_MaiOutput_P_Disable_IopOut(BAPE_OutputPort output);
static void      BAPE_MaiOutput_P_SetMclk_IopOut_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio);
static BERR_Code BAPE_MaiOutput_P_OpenHw_IopOut(BAPE_MaiOutputHandle handle);
static void      BAPE_MaiOutput_P_CloseHw_IopOut(BAPE_MaiOutputHandle handle);
static void      BAPE_MaiOutput_P_SetCrossbar_IopOut(BAPE_MaiOutputHandle handle, BAPE_StereoMode stereoMode);
static void      BAPE_MaiOutput_P_SetMute_IopOut(BAPE_OutputPort output, bool muted);
#else
static const uint16_t g_pauseburst[6] = {0xf872, 0x4e1f, 0x0003, 0x0020, 0x0000, 0x0000};
static const uint16_t g_nullburst[4] = {0xf872, 0x4e1f, 0xe000, 0x0000};

#define BAPE_P_MUTE_BUFFER_SIZE         sizeof(g_pauseburst)*64

static BAPE_MaiOutputDataPath BAPE_MaiOutput_P_FormatToDataPath(const BAPE_FMT_Descriptor *pFormat);
static BERR_Code BAPE_MaiOutput_P_FillMuteBuffer(BAPE_MaiOutputHandle handle);

static BERR_Code BAPE_MaiOutput_P_Open_Legacy(BAPE_MaiOutputHandle handle);
static void      BAPE_MaiOutput_P_Close_Legacy(BAPE_MaiOutputHandle handle);
static void      BAPE_MaiOutput_P_SetCbits_Legacy_isr(BAPE_MaiOutputHandle handle);
static BERR_Code BAPE_MaiOutput_P_Enable_Legacy(BAPE_OutputPort output);
static void      BAPE_MaiOutput_P_Disable_Legacy(BAPE_OutputPort output);
static void      BAPE_MaiOutput_P_SetMclk_Legacy_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio);
static BERR_Code BAPE_MaiOutput_P_OpenHw_Legacy(BAPE_MaiOutputHandle handle);
static void      BAPE_MaiOutput_P_CloseHw_Legacy(BAPE_MaiOutputHandle handle);
static void      BAPE_MaiOutput_P_SetCrossbar_Legacy(BAPE_MaiOutputHandle handle, BAPE_StereoMode stereoMode);
static void      BAPE_MaiOutput_P_SetMute_Legacy(BAPE_OutputPort output, bool muted);
#endif

/***************************************************************************
        Public APIs: From bape_output.h
***************************************************************************/

void BAPE_MaiOutput_GetDefaultSettings(
    BAPE_MaiOutputSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->stereoMode = BAPE_StereoMode_eLeftRight;
    pSettings->ditherEnabled = true;
    pSettings->underflowBurst = BAPE_SpdifBurstType_ePause;
    pSettings->useRawChannelStatus = false;
    /* channel status is initialized to zero for all fields */
}

/**************************************************************************/

BERR_Code BAPE_MaiOutput_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_MaiOutputSettings *pSettings,
    BAPE_MaiOutputHandle *pHandle             /* [out] */
    )
{
    BERR_Code errCode;
    BAPE_MaiOutputHandle handle;
    BAPE_MaiOutputSettings defaultSettings;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);
    
    BDBG_MSG(("%s: Opening MAI Output: %u", __FUNCTION__, index));

    *pHandle = NULL;    /* Set up to return null handle in case of error. */

    if ( index >= BAPE_CHIP_MAX_MAI_OUTPUTS )
    {
        BDBG_ERR(("Request to open MAI %d but chip only has %u MAI outputs", index, BAPE_CHIP_MAX_MAI_OUTPUTS));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( deviceHandle->maiOutputs[index] )
    {
        BDBG_ERR(("MAI output %d already open", index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Allocate the device structure, then fill in all the fields. */
    handle = BKNI_Malloc(sizeof(BAPE_MaiOutput));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    BKNI_Memset(handle, 0, sizeof(BAPE_MaiOutput));
    BDBG_OBJECT_SET(handle, BAPE_MaiOutput);
    handle->deviceHandle = deviceHandle;
    handle->index = index;
    BAPE_P_InitOutputPort(&handle->outputPort, BAPE_OutputPortType_eMaiOutput, index, handle);
    BAPE_FMT_P_EnableSource(&handle->outputPort.capabilities, BAPE_DataSource_eFci);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcm5_1);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcm7_1);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_eIec61937);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_eIec61937x4);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_eIec61937x16);
    handle->outputPort.muteInMixer = true;
    handle->outputPort.setTimingParams_isr = BAPE_MaiOutput_P_SetTimingParams_isr;
    handle->outputPort.enable = BAPE_MaiOutput_P_Enable;
    handle->outputPort.disable = BAPE_MaiOutput_P_Disable;
    handle->outputPort.setMclk_isr = BAPE_MaiOutput_P_SetMclk_isr;
    handle->outputPort.setMute = BAPE_MaiOutput_P_SetMute;
    BKNI_Snprintf(handle->name, sizeof(handle->name), "MAI %u", index);
    handle->outputPort.pName = handle->name;
    /* Currently we only support one HDMI, this can be expanded later if we support more */
    #if BAPE_CHIP_MAX_MAI_OUTPUTS > 1
        #error "Need to support more MAI outputs"
    #endif
    handle->offset = 0;
    handle->sampleRate = 48000;

    BDBG_ASSERT(handle->offset == 0);

    BKNI_EnterCriticalSection();
    BAPE_MaiOutput_P_SetTimingParams_isr(&handle->outputPort, 48000, BAVC_Timebase_e0);
    BKNI_LeaveCriticalSection();

    /* Init to specified settings */
    if ( NULL == pSettings )
    {
        BAPE_MaiOutput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    /* store open settings before open calls. */
    handle->settings = *pSettings;

#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
    errCode = BAPE_MaiOutput_P_Open_IopOut(handle);
#else
    errCode = BAPE_MaiOutput_P_Open_Legacy(handle);
#endif
    if ( errCode )
    {
        BAPE_MaiOutput_Close(handle);
        return BERR_TRACE(errCode);
    }

    errCode = BAPE_MaiOutput_P_OpenHw(handle);
    if ( errCode )
    {
        BAPE_MaiOutput_Close(handle);
        return BERR_TRACE(errCode);
    }

    errCode = BAPE_MaiOutput_P_ApplySettings(handle, pSettings, true);   /* true => force update of HW */
    if ( errCode )
    {
        BAPE_MaiOutput_Close(handle);
        return BERR_TRACE(errCode);
    }

    *pHandle = handle;
    handle->deviceHandle->maiOutputs[index] = handle;
    return BERR_SUCCESS;
}

/**************************************************************************/

void BAPE_MaiOutput_Close(
    BAPE_MaiOutputHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);

    /* Make sure we're not still connected to anything */
    if ( handle->outputPort.mixer )
    {
        BDBG_ERR(("Cannot close MAI output %p (%d), still connected to mixer %p", handle, handle->index, handle->outputPort.mixer));
        BDBG_ASSERT(NULL == handle->outputPort.mixer);
        return;
    }

#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
    BAPE_MaiOutput_P_Close_IopOut(handle);
#else
    BAPE_MaiOutput_P_Close_Legacy(handle);
#endif

    BAPE_MaiOutput_P_CloseHw(handle);

    BDBG_ASSERT(handle->index < BAPE_CHIP_MAX_MAI_OUTPUTS);
    handle->deviceHandle->maiOutputs[handle->index] = NULL;
    BDBG_OBJECT_DESTROY(handle, BAPE_MaiOutput);
    BKNI_Free(handle);    
}

/**************************************************************************/

void BAPE_MaiOutput_GetSettings(
    BAPE_MaiOutputHandle handle,
    BAPE_MaiOutputSettings *pSettings     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

/**************************************************************************/

BERR_Code BAPE_MaiOutput_SetSettings(
    BAPE_MaiOutputHandle handle,
    const BAPE_MaiOutputSettings *pSettings
    )
{
    BERR_Code   errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(NULL != pSettings);
    
    errCode = BAPE_MaiOutput_P_ApplySettings(handle, pSettings, false); /* false => don't force (only update HW for changes) */

    return errCode;
}

/**************************************************************************/

void BAPE_MaiOutput_GetOutputPort(
    BAPE_MaiOutputHandle handle,
    BAPE_OutputPort *pOutputPort        /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(NULL != pOutputPort);
    *pOutputPort = &handle->outputPort;
}

/**************************************************************************/

void BAPE_MaiOutput_GetInterruptHandlers(
    BAPE_MaiOutputHandle handle,
    BAPE_MaiOutputInterruptHandlers *pInterrupts    /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(NULL != pInterrupts);
    BKNI_EnterCriticalSection();
    *pInterrupts = handle->interrupts;
    BKNI_LeaveCriticalSection();
}

/**************************************************************************/

BERR_Code BAPE_MaiOutput_SetInterruptHandlers(
    BAPE_MaiOutputHandle handle,
    const BAPE_MaiOutputInterruptHandlers *pInterrupts
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(NULL != pInterrupts);
    BKNI_EnterCriticalSection();
    handle->interrupts = *pInterrupts;
    BKNI_LeaveCriticalSection();
    return BERR_SUCCESS;
}

/***************************************************************************
        BAPE Internal APIs: From bape_fmm_priv.h
***************************************************************************/

BERR_Code BAPE_MaiOutput_P_PrepareForStandby(
    BAPE_Handle bapeHandle
    )
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    maiOutputIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    /* For each opened MaiOutput, call the functions necessary to restore the hardware to it's appropriate state. */
    for ( maiOutputIndex=0 ; maiOutputIndex<BAPE_CHIP_MAX_MAI_OUTPUTS ; maiOutputIndex++ )
    {
        if ( bapeHandle->maiOutputs[maiOutputIndex] )       /* If this MaiOutput is open... */
        {
            BAPE_MaiOutputHandle hMaiOutput = bapeHandle->maiOutputs[maiOutputIndex];

            /* Put the HW into the generic closed state. */
            BAPE_MaiOutput_P_CloseHw(hMaiOutput);
        }
    }
    return errCode;
}

BERR_Code BAPE_MaiOutput_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    maiOutputIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    /* For each opened MaiOutput, call the functions necessary to restore the hardware to it's appropriate state. */
    for ( maiOutputIndex=0 ; maiOutputIndex<BAPE_CHIP_MAX_MAI_OUTPUTS ; maiOutputIndex++ )
    {
        if ( bapeHandle->maiOutputs[maiOutputIndex] )       /* If this MaiOutput is open... */
        {
            BAPE_MaiOutputHandle hMaiOutput = bapeHandle->maiOutputs[maiOutputIndex];

            /* Put the HW into the generic open state. */
            errCode = BAPE_MaiOutput_P_OpenHw(hMaiOutput);
            if ( errCode ) return BERR_TRACE(errCode);
            
            /* Now apply changes for the settings struct. */
            errCode = BAPE_MaiOutput_P_ApplySettings(hMaiOutput, &hMaiOutput->settings, true);   /* true => force update of HW */
            if ( errCode ) return BERR_TRACE(errCode);

            /* Now restore the dynamic stuff from the values saved in the device struct. */
            BKNI_EnterCriticalSection();

            BAPE_MaiOutput_P_SetTimingParams_isr(&hMaiOutput->outputPort, 
                                                 hMaiOutput->sampleRate, 
                                                 0);    /* timebase is unused, 0 is dummy value */

            BAPE_MaiOutput_P_SetMclk_isr(&hMaiOutput->outputPort,
                                         hMaiOutput->mclkInfo.mclkSource,
                                         hMaiOutput->mclkInfo.pllChannel,
                                         hMaiOutput->mclkInfo.mclkFreqToFsRatio );

            BKNI_LeaveCriticalSection();
        }
    }
    return errCode;
}

/***************************************************************************
        Private callbacks: Protyped above
***************************************************************************/

static void BAPE_MaiOutput_P_SetTimingParams_isr(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase)
{
    BAPE_MaiOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BSTD_UNUSED(timebase);  /* MAI doesn't care */

    handle->sampleRate = sampleRate;

    BAPE_MaiOutput_P_SetCbits_isr(handle);

    if ( handle->interrupts.sampleRate.pCallback_isr )
    {
        BDBG_MSG(("Sending sample rate callback - new rate %u", sampleRate));
        handle->interrupts.sampleRate.pCallback_isr(handle->interrupts.sampleRate.pParam1, handle->interrupts.sampleRate.param2, sampleRate);
    }
}

/**************************************************************************/
static BERR_Code BAPE_MaiOutput_P_Enable(BAPE_OutputPort output)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
    return BAPE_MaiOutput_P_Enable_IopOut(output);
#else
    return BAPE_MaiOutput_P_Enable_Legacy(output);
#endif
}

/**************************************************************************/

static void BAPE_MaiOutput_P_Disable(BAPE_OutputPort output)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
    BAPE_MaiOutput_P_Disable_IopOut(output);
#else
    BAPE_MaiOutput_P_Disable_Legacy(output);
#endif
}

/**************************************************************************/

static void BAPE_MaiOutput_P_SetMclk_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
    BAPE_MaiOutput_P_SetMclk_IopOut_isr(output, mclkSource, pllChannel, mclkFreqToFsRatio);
#else
    BAPE_MaiOutput_P_SetMclk_Legacy_isr(output, mclkSource, pllChannel, mclkFreqToFsRatio);
#endif
}

/**************************************************************************/

static void      BAPE_MaiOutput_P_SetMute(BAPE_OutputPort output, bool muted, bool sync)
{
    BSTD_UNUSED(sync);
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
    BAPE_MaiOutput_P_SetMute_IopOut(output, muted);
#else
    BAPE_MaiOutput_P_SetMute_Legacy(output, muted);
#endif
}

/***************************************************************************
        Private functions: Protyped above
***************************************************************************/

static void BAPE_MaiOutput_P_SetCbits_isr(BAPE_MaiOutputHandle handle)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
    BAPE_MaiOutput_P_SetCbits_IopOut_isr(handle);
#else
    BAPE_MaiOutput_P_SetCbits_Legacy_isr(handle);
#endif
}

/**************************************************************************/

static BERR_Code BAPE_MaiOutput_P_OpenHw(BAPE_MaiOutputHandle handle)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
    return BAPE_MaiOutput_P_OpenHw_IopOut(handle);
#else
    return BAPE_MaiOutput_P_OpenHw_Legacy(handle);
#endif
}

static void BAPE_MaiOutput_P_CloseHw(BAPE_MaiOutputHandle handle)
{
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
    BAPE_MaiOutput_P_CloseHw_IopOut(handle);
#else
    BAPE_MaiOutput_P_CloseHw_Legacy(handle);
#endif
}

/**************************************************************************/

static BERR_Code BAPE_MaiOutput_P_ApplySettings(
    BAPE_MaiOutputHandle handle,
    const BAPE_MaiOutputSettings *pSettings,
    bool force
    )
{
    bool burstTypeChanged;
    BSTD_UNUSED(force); /* We don't need this because all settings are always written to HW. */

    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(NULL != pSettings);

    burstTypeChanged = handle->settings.underflowBurst != pSettings->underflowBurst;

    handle->settings = *pSettings;

    /* Remaining fields are handled by MS regs.  Must modify those in critical section. */    
    BKNI_EnterCriticalSection();
    BAPE_MaiOutput_P_SetCbits_isr(handle);
    BKNI_LeaveCriticalSection();

#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
    BAPE_MaiOutput_P_SetCrossbar_IopOut(handle, handle->settings.stereoMode);

    BSTD_UNUSED(burstTypeChanged);
#else
    BAPE_MaiOutput_P_SetCrossbar_Legacy(handle, handle->settings.stereoMode);

    /* check/modify burst rbuf */
    if ( handle->pMuteBuffer && burstTypeChanged )
    {
        BAPE_MaiOutput_P_FillMuteBuffer(handle);
    }
#endif

    return BERR_SUCCESS;
}

static uint32_t BAPE_MaiOutput_P_SampleRateToMaiFormat(unsigned sampleRate)
{
    switch ( sampleRate )
    {
    case 32000:    /* 32K Sample rate */
        return 7;
    case 44100:    /* 44.1K Sample rate */
        return 8;
    case 48000:      /* 48K Sample rate */
        return 9;
    case 96000:      /* 96K Sample rate */
        return 12;
    case 128000:     /* 128K Sample rate */
        return 13;
    case 176400:   /* 176.4K Sample rate */
        return 14;
    case 192000:     /* 192K Sample rate */
        return 15;
    default:
        return 0;   /* Not Indicated */
    }
}

#ifndef BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
static BAPE_MaiOutputDataPath BAPE_MaiOutput_P_FormatToDataPath(const BAPE_FMT_Descriptor *pFormat)
{
    if ( pFormat )
    {
        switch ( pFormat->type )
        {
        case BAPE_DataType_ePcmStereo:
        case BAPE_DataType_eIec61937:
        case BAPE_DataType_eIec61937x4:
            return BAPE_MaiOutputDataPath_eStereo;
        case BAPE_DataType_ePcm5_1:
        case BAPE_DataType_ePcm7_1:
            return BAPE_MaiOutputDataPath_eMaiMulti;
        case BAPE_DataType_eIec61937x16:
            return BAPE_MaiOutputDataPath_eHbr;
        default:
            break;
        }
    }
    return BAPE_MaiOutputDataPath_eNone;
}

static void BAPE_MaiOutput_P_SetCbits_Legacy_isr(BAPE_MaiOutputHandle handle)
{
    uint32_t regAddr, regVal;
    unsigned validity = 0;
    BAPE_DataType dataType = BAPE_DataType_ePcmStereo;
    unsigned compressed = 0;
    unsigned hbr = 0;
    unsigned dither = 0;
    bool compressedAsPcm = false;
    const BAPE_FMT_Descriptor     *pFormat;

    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);

    if ( handle->outputPort.mixer )
    {
        pFormat = BAPE_Mixer_P_GetOutputFormat(handle->outputPort.mixer);

        dataType = pFormat->type;
        compressed = (unsigned)BAPE_FMT_P_IsCompressed(pFormat);
        hbr = (unsigned)BAPE_FMT_P_IsHBR(pFormat);
        compressedAsPcm = BAPE_FMT_P_IsDtsCdCompressed(pFormat);
    }
    else
    {
        /* No update required, maintain previous settings until connected.  Update will be applied in Enable(). */
        return;
    }

    BDBG_MSG(("Set MAI CBITS SR %u compressed %u, compressedAsPcm %u", handle->sampleRate, compressed, compressedAsPcm));

    /* Only set validity if we're outputting compressed */
    if ( compressed && !compressedAsPcm )
    {
        validity = 1;
    }
    else if ( !compressedAsPcm )
    {
        if ( handle->settings.ditherEnabled )
        {
            dither = 1;
        }
    }

    /* Program MAI format correctly */
    if ( handle->outputPort.mixer )
    {
        regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_OP_CTRL_MAI_FORMAT);
        regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_FORMAT, AUDIO_FORMAT)|
                    BCHP_MASK(AUD_FMM_OP_CTRL_MAI_FORMAT, SAMPLE_WIDTH)|
                    BCHP_MASK(AUD_FMM_OP_CTRL_MAI_FORMAT, SAMPLE_RATE));
        if ( compressed && !compressedAsPcm )
        {
            if ( hbr )
            {
                regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_FORMAT, AUDIO_FORMAT, SPDIF_Comp_8_Channel);
            }
            else
            {
                regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_FORMAT, AUDIO_FORMAT, SPDIF_Comp_2_Channel);
            }
        }
        else
        {
            switch ( dataType )
            {
            case BAPE_DataType_ePcm5_1:
                regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_FORMAT, AUDIO_FORMAT, SPDIF_PCM_6_Channel);
                break;
            case BAPE_DataType_ePcm7_1:
                regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_FORMAT, AUDIO_FORMAT, SPDIF_PCM_8_Channel);
                break;
            default:
            case BAPE_DataType_ePcmStereo:
                regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_FORMAT, AUDIO_FORMAT, SPDIF_PCM_2_Channel);
                break;
            }
            regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MAI_FORMAT, SAMPLE_WIDTH, 0); /* 32 bits per sample */
        }

        if ( compressed || compressedAsPcm )
        {
            regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MAI_FORMAT, SAMPLE_WIDTH, 16);    /* TODO: */
        }

        if ( hbr )
        {
            regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MAI_FORMAT, SAMPLE_RATE, 
                                      BAPE_MaiOutput_P_SampleRateToMaiFormat(192000));
        }
        else
        {
            regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MAI_FORMAT, SAMPLE_RATE, 
                                      BAPE_MaiOutput_P_SampleRateToMaiFormat(handle->sampleRate));
        }
        BDBG_MSG(("Set MAI format to 0x%x", regVal));
        BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_OP_CTRL_MAI_FORMAT, regVal);
    }

    BDBG_MSG(("Set MAI HW CBITS SR %u compressed %u hbr %u validity %u dither %u", handle->sampleRate, compressed, hbr, validity, dither));
    /* Tell the HW to hold the current CBITS and set dither correctly */
    regAddr = BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1;
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1, HOLD_CSTAT, Hold);
    BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);

    regVal &= ~(BCHP_MASK(AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1, DITHER_ENA)|
                BCHP_MASK(AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1, VALIDITY));
    regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1, DITHER_ENA, dither);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1, VALIDITY, validity);
    BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);

    /* JDG - Do not change the value of AUD_FMM_MS_CTRL_STRM_ENA.STREAM1_ENA here.
             This causes upstream requests to stop if you are using the Stereo datapath.  That can cause 
             mixer rate errors and FMM lockups.  So, we need to leave this enabled always.  For MAI_Multi and HBR
             paths, this is disabled already so this has no effect. */

    /* Program channel status */
    if ( handle->settings.useRawChannelStatus )
    {
        regVal = 
            (uint32_t)handle->settings.rawChannelStatus[0] |
            (((uint32_t)handle->settings.rawChannelStatus[1])<<8) |
            (((uint32_t)handle->settings.rawChannelStatus[2])<<16) |
            (((uint32_t)handle->settings.rawChannelStatus[3])<<24);
        BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, regVal);
        regVal = (uint32_t)handle->settings.rawChannelStatus[4];
        BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1, regVal);
    }
    else if ( compressed && !compressedAsPcm )
    {
        regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1);
        regVal &= ~(BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_PRO_CONS)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_COMP_LIN)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_CP)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_EMPH)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_CMODE)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_CATEGORY)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_SOURCE)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_FREQ)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_ACCURACY)|
#ifdef BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1_Compressed_BITS_31_to_30_MASK          
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_BITS_31_to_30)
#else
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_FREQ_EXTN)
#endif                    
                    );
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_PRO_CONS, handle->settings.channelStatus.professional);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_COMP_LIN, 1);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_CP, (handle->settings.channelStatus.copyright)?0:1);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_CATEGORY, handle->settings.channelStatus.categoryCode);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_SOURCE, handle->settings.channelStatus.sourceNumber);
        if ( dataType == BAPE_DataType_eIec61937x16 )
        {
            regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_FREQ, BAPE_P_GetSampleRateCstatCode(768000));
        }
        else
        {
            regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_FREQ, BAPE_P_GetSampleRateCstatCode(handle->sampleRate));
        }
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, Compressed_ACCURACY, handle->settings.channelStatus.clockAccuracy);
        BREG_Write32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, regVal);
        BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1, 0);
    }
    else
    {   
        regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1);
        regVal &= ~(BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_PRO_CONS)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_COMP_LIN)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_CP)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_EMPH)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_CMODE)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_CATEGORY)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_SOURCE)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_FREQ)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_ACCURACY)|
#ifdef BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1_Compressed_BITS_31_to_30_MASK          
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_BITS_31_to_30)
#else
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_FREQ_EXTN)
#endif                    
                    );
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_PRO_CONS, handle->settings.channelStatus.professional);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_CP, (handle->settings.channelStatus.copyright)?0:1);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_CATEGORY, handle->settings.channelStatus.categoryCode);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_SOURCE, handle->settings.channelStatus.sourceNumber);
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_FREQ, BAPE_P_GetSampleRateCstatCode(handle->sampleRate));
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, PCM_ACCURACY, handle->settings.channelStatus.clockAccuracy);
        BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1, regVal);        
        regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1);
        regVal &= ~(BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1, PCM_MAX_LEN)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1, PCM_LENGTH)|
                    BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1, PCM_ORIG_FREQ));
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1, PCM_MAX_LEN, 1);    /* 24-bits */
        #ifdef BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1_PCM_CGMS_A_MASK
        regVal &= ~(BCHP_MASK(AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1, PCM_CGMS_A));
        regVal |= BCHP_FIELD_DATA(AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1, PCM_CGMS_A, handle->settings.channelStatus.cgmsA);
        #endif
        BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_HI_1, regVal);
    }

    /* Begin using new bits */
    regAddr = BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1;
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~BCHP_MASK(AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1, HOLD_CSTAT);
    regVal |= BCHP_FIELD_ENUM(AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1, HOLD_CSTAT, Update);
    BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);

    return;
}

static void BAPE_MaiOutput_P_SetMclk_Legacy_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio)
{
    uint32_t regVal, pllclksel=0;
    uint32_t mclkRate;

    BAPE_MaiOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(handle->offset == 0);

    /* Save the settings in case we need to re-apply them later. */
    handle->mclkInfo.mclkSource         = mclkSource;
    handle->mclkInfo.pllChannel         = pllChannel;
    handle->mclkInfo.mclkFreqToFsRatio  = mclkFreqToFsRatio;

    switch ( mclkSource )
    {
    /* PLL Timing */
    #if BAPE_CHIP_MAX_PLLS > 0
    case BAPE_MclkSource_ePll0:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI_PLLCLKSEL_PLL0_ch1 + pllChannel;
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 1
    case BAPE_MclkSource_ePll1:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI_PLLCLKSEL_PLL1_ch1 + pllChannel;
        break;
    #endif
    #if BAPE_CHIP_MAX_PLLS > 2
    case BAPE_MclkSource_ePll2:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI_PLLCLKSEL_PLL2_ch1 + pllChannel;
        break;
    #endif

    /* DAC Timing */
    #if BAPE_CHIP_MAX_DACS > 0
    case BAPE_MclkSource_eHifidac0:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI_PLLCLKSEL_Hifidac0;
        break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 1
    case BAPE_MclkSource_eHifidac1:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI_PLLCLKSEL_Hifidac1;
        break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 2
    case BAPE_MclkSource_eHifidac2:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI_PLLCLKSEL_Hifidac2;
        break;
    #endif

    /* NCO (Mclkgen) Timing */
    #if BAPE_CHIP_MAX_NCOS > 0
    case BAPE_MclkSource_eNco0:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI_PLLCLKSEL_Mclk_gen0;
        break;
    #endif
    #if BAPE_CHIP_MAX_NCOS > 1
    case BAPE_MclkSource_eNco1:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI_PLLCLKSEL_Mclk_gen1;
        break;
    #endif
    #if BAPE_CHIP_MAX_NCOS > 2
    case BAPE_MclkSource_eNco2:
        pllclksel = BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI_PLLCLKSEL_Mclk_gen2;
        break;
    #endif

    /* Should never get here */
    default:
        BDBG_ERR(("mclkSource (%u) doesn't refer to a valid PLL DAC, or NCO", mclkSource));
        BDBG_ASSERT(false);     /* something went wrong somewhere! */
        return;
    }

    /* Tell the output formatter how fast our mclk is. */
    mclkRate = mclkFreqToFsRatio / ( 2 * 64 );  /* MAI has sclk = 64Fs */

    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI);
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, PLLCLKSEL)|
                BCHP_MASK(AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, MCLK_RATE));
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, MCLK_RATE, mclkRate);
    regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, PLLCLKSEL, pllclksel);
    BREG_Write32_isr(handle->deviceHandle->regHandle, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI, regVal);
}

static void BAPE_MaiOutput_P_SetMute_Legacy(BAPE_OutputPort output, bool muted)
{
    BAPE_MaiOutputHandle handle;
    BAPE_MixerGroupInputSettings dataInputSettings, burstInputSettings;
    const BAPE_FMT_Descriptor *pFormat;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);

    pFormat = BAPE_Mixer_P_GetOutputFormat(output->mixer);
    BDBG_ASSERT(NULL != pFormat);

    if ( BAPE_FMT_P_IsCompressed(pFormat) )
    {
        BAPE_MixerGroup_P_GetInputSettings(handle->hMixer, 0, &dataInputSettings);
        BAPE_MixerGroup_P_GetInputSettings(handle->hMixer, 1, &burstInputSettings);
        dataInputSettings.rampStep = 0x800000;
        burstInputSettings.rampStep = 0x800000;

        if ( muted )
        {
            dataInputSettings.coefficients[0][0][0] = 0;
            dataInputSettings.coefficients[0][1][1] = 0;
            burstInputSettings.coefficients[0][0][0] = 0x800000;
            burstInputSettings.coefficients[0][1][1] = 0x800000;
            (void)BAPE_MixerGroup_P_SetInputSettings(handle->hMixer, 0, &dataInputSettings);
            (void)BAPE_MixerGroup_P_SetInputSettings(handle->hMixer, 1, &burstInputSettings);
        }
        else
        {
            dataInputSettings.coefficients[0][0][0] = 0x800000;
            dataInputSettings.coefficients[0][1][1] = 0x800000;
            burstInputSettings.coefficients[0][0][0] = 0;
            burstInputSettings.coefficients[0][1][1] = 0;
            (void)BAPE_MixerGroup_P_SetInputSettings(handle->hMixer, 1, &burstInputSettings);
            (void)BAPE_MixerGroup_P_SetInputSettings(handle->hMixer, 0, &dataInputSettings);
        }
    }
}

static BERR_Code BAPE_MaiOutput_P_Enable_Legacy(BAPE_OutputPort output)
{
    BAPE_IopStreamSettings streamSettings;
    unsigned streamId;
    
    BAPE_MaiOutputHandle handle;
    BAPE_MixerHandle mixer;
    BREG_Handle regHandle;
    BAPE_Reg_P_FieldList regFieldList;
    const BAPE_FMT_Descriptor *pFormat;
    uint32_t streamEnables, regAddr;
    unsigned i, numChannelPairs;
    bool enableNewPath=false;
    BAPE_MaiOutputDataPath newDataPath;
    BAPE_MixerGroupInputSettings dataInputSettings, burstInputSettings;
    
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(false == handle->enabled);
    BDBG_ASSERT(NULL != output->mixer);

    mixer = output->mixer;
    regHandle = handle->deviceHandle->regHandle;

    BDBG_MSG(("Enabling %s", handle->name));

    pFormat = BAPE_Mixer_P_GetOutputFormat(output->mixer);
    BDBG_ASSERT(NULL != pFormat);
    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(pFormat);
    BDBG_ASSERT(numChannelPairs <= 4);

    BDBG_MSG(("Starting MAI output %u using data type %s", handle->index, BAPE_FMT_P_GetTypeName(pFormat)));

    /* Set MAI/I2S Multi Grouping based on number of channel pairs */
    regAddr = BCHP_AUD_FMM_OP_CTRL_MAI_MULTI_GROUPING;
    #if 0 /* This grouping does not seem to be stable in HW.  If programmed, once in a handful of tries the HW doesn't start */
    /* We need to write 1's in the fields that are multichannel.  For all stereo, write 0 */
    if ( numChannelPairs == 1 )
    {
        BREG_Write32(regHandle, regAddr, 0);
    }
    else
    {
        BREG_Write32(regHandle, regAddr, 0xf>>(4-numChannelPairs));
    }
    #else
    BREG_Write32(regHandle, regAddr, 0);
    #endif

    newDataPath = BAPE_MaiOutput_P_FormatToDataPath(pFormat);

    /* Setup new data layout if required */
    if ( newDataPath != handle->dataPath )
    {
        enableNewPath = true;

        if ( handle->dataPath != BAPE_MaiOutputDataPath_eNone )
        {
            /* Stop stream 1 path */
            BAPE_SfifoGroup_P_Stop(handle->hSfifo);
            BAPE_MixerGroup_P_StopInput(handle->hMixer, 1);
            BAPE_MixerGroup_P_StopOutput(handle->hMixer, 0);
            BREG_Write32(regHandle, BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR, 1<<GET_MAI_STEREO_STREAM_ID(handle->index));
            /* If HBR, stop the HBR output path */
            if ( handle->dataPath == BAPE_MaiOutputDataPath_eHbr )
            {
                BAPE_MixerGroup_P_StopOutput(handle->hMixer, 1);
                BREG_Write32(regHandle, BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR, 1<<GET_MAI_HBR_STREAM_ID(handle->index));
            }
            handle->dataPath = BAPE_MaiOutputDataPath_eNone;
        }
    }

    for ( i = 0; i < numChannelPairs; i++ )
    {
        switch ( newDataPath )
        {
        case BAPE_MaiOutputDataPath_eStereo:
        case BAPE_MaiOutputDataPath_eHbr:
            streamId = GET_MAI_STEREO_STREAM_ID(handle->index);
            BDBG_MSG(("Stereo/HBR Payload source FCI 0x%x", output->sourceMixerFci.ids[i]));
            /* Setup actual data source as primary mixer input */
            BAPE_MixerGroup_P_GetInputSettings(handle->hMixer, 0, &dataInputSettings);
            BAPE_MixerGroup_P_GetInputSettings(handle->hMixer, 1, &burstInputSettings);
            dataInputSettings.input = handle->outputPort.sourceMixerFci;
            if ( handle->muted && BAPE_FMT_P_IsCompressed(pFormat) )
            {
                /* If switching between compressed<->pcm we need to ensure pausebursts are only sent in compressed mode */
                dataInputSettings.coefficients[0][0][0] = 0;   /* Mute data path */
                dataInputSettings.coefficients[0][1][1] = 0;
                burstInputSettings.coefficients[0][0][0] = 0x800000;    /* Enable pauseburst */
                burstInputSettings.coefficients[0][1][1] = 0x800000;
            }
            else
            {
                dataInputSettings.coefficients[0][0][0] = 0x800000;    /* Enable data path */
                dataInputSettings.coefficients[0][1][1] = 0x800000;
                burstInputSettings.coefficients[0][0][0] = 0;   /* Mute pauseburst */
                burstInputSettings.coefficients[0][1][1] = 0;
            }
            (void)BAPE_MixerGroup_P_SetInputSettings(handle->hMixer, 0, &dataInputSettings);
            (void)BAPE_MixerGroup_P_SetInputSettings(handle->hMixer, 1, &burstInputSettings);
            break;
        case BAPE_MaiOutputDataPath_eMaiMulti:
            streamId = GET_MAI_MULTI_STREAM_ID(handle->index, i);
            BDBG_MSG(("MAI Multi Payload [stream %u] source FCI 0x%x", streamId, output->sourceMixerFci.ids[i]));
            /* Write source FCI to IOP */
            BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
            streamSettings.resolution = 24;
            streamSettings.input = output->sourceMixerFci.ids[i];   /* Take source FCI provided from mixer */
            (void)BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
            break;
        default:
            break;
        }
    }

    /* Setup for HBR vs. non-HBR output */
    #ifdef BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE_HBR_ENABLE_MASK
    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
    if ( newDataPath == BAPE_MaiOutputDataPath_eHbr )
    {
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, MAI_PAYLOAD_SEL, HBR_channel);
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, HBR_ENABLE, 1);
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, HBR_HW_CH_STATUS_EN, 1);
    }
    else 
    {
        if ( newDataPath == BAPE_MaiOutputDataPath_eMaiMulti )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, MAI_PAYLOAD_SEL, MAI_Multi);
        }
        else
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, MAI_PAYLOAD_SEL, Stereo);
        }
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, HBR_ENABLE, 0);
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, HBR_HW_CH_STATUS_EN, 0);
    }
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE);
    #endif

    /* Enable the formatter using HW channel status */
    BKNI_EnterCriticalSection();
    BAPE_MaiOutput_P_SetCbits_isr(handle);
    if ( newDataPath == BAPE_MaiOutputDataPath_eStereo )
    {
        BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_MS_CTRL_STRM_ENA, AUD_FMM_MS_CTRL_STRM_ENA, STREAM1_ENA, Enable);
    }
    else /* HBR/Multichannel */
    {
        BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_MS_CTRL_STRM_ENA, AUD_FMM_MS_CTRL_STRM_ENA, STREAM1_ENA, Disable);
    }
    BKNI_LeaveCriticalSection();

    /* Enable output streams always */
    streamEnables = (BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_SET, STREAM0_ENA))<<(GET_MAI_STEREO_STREAM_ID(handle->index));
    switch ( newDataPath )
    {
    case BAPE_MaiOutputDataPath_eMaiMulti:
        for ( i = 0; i < numChannelPairs; i++ )
        {
            streamEnables |= (BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_SET, STREAM0_ENA))<<(GET_MAI_MULTI_STREAM_ID(handle->index, i));
        }
        break;
    case BAPE_MaiOutputDataPath_eHbr:
        streamEnables |= (BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_SET, STREAM0_ENA))<<(GET_MAI_HBR_STREAM_ID(handle->index));
        break;
    default:
        break;
    }
    BREG_Write32(regHandle, BCHP_AUD_FMM_OP_CTRL_ENABLE_SET, streamEnables);

    if ( enableNewPath )
    {
        /* Enable remaining path consumer..producer */
        if ( newDataPath == BAPE_MaiOutputDataPath_eHbr )
        {
            /* Enable the HBR output from mixer output 1 only in HBR mode */
            (void)BAPE_MixerGroup_P_StartOutput(handle->hMixer, 1);
        }
        else
        {
            (void)BAPE_MixerGroup_P_StartOutput(handle->hMixer, 0);
        }
        (void)BAPE_MixerGroup_P_StartInput(handle->hMixer, 1);  /* Input for pauseburst */
        (void)BAPE_SfifoGroup_P_Start(handle->hSfifo, false);   /* Sfifo for pauseburst */
        handle->dataPath = newDataPath;
    }

    /* Last, enable the actual data input for stereo/hbr mode */
    switch ( newDataPath )
    {
    case BAPE_MaiOutputDataPath_eStereo:
    case BAPE_MaiOutputDataPath_eHbr:
        (void)BAPE_MixerGroup_P_StartInput(handle->hMixer, 0);  /* Input for data */
        break;
    default:
        break;
    }

    handle->enabled = true;
    return BERR_SUCCESS;
}

static void BAPE_MaiOutput_P_Disable_Legacy(BAPE_OutputPort output)
{
    BAPE_MaiOutputHandle handle;
    BAPE_MixerHandle mixer;
    BREG_Handle regHandle;
    const BAPE_FMT_Descriptor *pFormat;
    unsigned i, numChannelPairs;

    BERR_Code errCode;
    uint32_t regVal;
    
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(NULL != output->mixer);

    mixer = output->mixer;
    regHandle = handle->deviceHandle->regHandle;

    BDBG_MSG(("Disabling %s", handle->name));
    pFormat = BAPE_Mixer_P_GetOutputFormat(output->mixer);
    BDBG_ASSERT(NULL != pFormat);
    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(pFormat);
    BDBG_ASSERT(numChannelPairs <= 4);

    /* We need to stop producer..consumer.  First stop consuming from the real data source */
    switch ( handle->dataPath )
    {
    case BAPE_MaiOutputDataPath_eStereo:
    case BAPE_MaiOutputDataPath_eHbr:
        BAPE_MixerGroup_P_StopInput(handle->hMixer, 0);
        break;
    case BAPE_MaiOutputDataPath_eMaiMulti:
        regVal = 0;
        for ( i = 0; i < numChannelPairs; i++ )
        {
            regVal |= 1<<GET_MAI_MULTI_STREAM_ID(handle->index, i);
        }
        BREG_Write32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR, regVal);
        break;
    default:
        break;
    }

    /* Now data consumption has stopped, clear out FCI ID's */
    if ( handle->dataPath == BAPE_MaiOutputDataPath_eMaiMulti )
    {
        unsigned streamId;
        for ( i = 0; i < numChannelPairs; i++ )
        {
            BAPE_IopStreamSettings streamSettings;
            streamId = GET_MAI_MULTI_STREAM_ID(handle->index, i);
            /* Write source FCI to IOP */
            BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
            streamSettings.input = BAPE_FCI_ID_INVALID;
            errCode = BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
            BDBG_ASSERT(BERR_SUCCESS == errCode);
        }
    }
    else
    {
        BAPE_MixerGroupInputSettings mixerInputSettings;
        BAPE_MixerGroup_P_GetInputSettings(handle->hMixer, 0, &mixerInputSettings);
        BAPE_FciIdGroup_Init(&mixerInputSettings.input);
        (void)BAPE_MixerGroup_P_SetInputSettings(handle->hMixer, 0, &mixerInputSettings);
    }

    handle->enabled = false;
}

static BERR_Code BAPE_MaiOutput_P_OpenHw_Legacy(BAPE_MaiOutputHandle handle)
{
    BERR_Code       errCode = BERR_SUCCESS;
    BAPE_Handle     deviceHandle;
    uint32_t        streamId;
    BAPE_SfifoGroupCreateSettings sfifoCreateSettings;
    BAPE_MixerGroupCreateSettings mixerCreateSettings;
    BAPE_SfifoGroupSettings sfifoSettings;
    BAPE_MixerGroupSettings mixerSettings;
    BAPE_MixerGroupInputSettings mixerInputSettings;
    BAPE_FciIdGroup fciGroup;
    BAPE_Reg_P_FieldList regFieldList;
    BAPE_IopStreamSettings streamSettings;

    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    BDBG_ASSERT(handle->index == 0);

    /* Setup Channel Status Formatter and enable always.  This solves a lot of receiver compatibility issues. */
    BKNI_EnterCriticalSection();
    {
        /* Enable HW cbit formatter */
        BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_MS_CTRL_USEQ_BYPASS, AUD_FMM_MS_CTRL_USEQ_BYPASS, STREAM1, Bypass);
        BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1, AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1, ENABLE, Enable);

        /* Disable FW cbit formatter */
        BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
        BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_MS_CTRL_FW_STREAM_CTRL_1, OVERWRITE_DATA, Disable);
        BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_MS_CTRL_FW_STREAM_CTRL_1, STREAM_ENA, Disable);
        BAPE_Reg_P_ApplyFieldList_isr(&regFieldList, BCHP_AUD_FMM_MS_CTRL_FW_STREAM_CTRL_1);

        BAPE_MaiOutput_P_SetCbits_isr(handle);
    }
    BKNI_LeaveCriticalSection();
    
    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, STRM1_TMG_SRC_SEL, MAI_Multi);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, MAI_PAYLOAD_SEL, Stereo);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, MAI_CSTAT_SEL, STRM1);
    #ifdef BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE_HBR_ENABLE_MASK
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, HBR_ENABLE, 0);
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, HBR_HW_CH_STATUS_EN, 0);
    #endif
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE);

    /* The data going to the MS doesn't matter.  Set it to an invalid FCI ID */
    streamId = GET_MAI_CBIT_STREAM_ID(handle->index);
    BDBG_MSG(("MAI Channel Status [stream %u] source FCI 0x%x", streamId, BAPE_FCI_ID_INVALID));

    /* Write dummy source FCI to IOP */
    BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    streamSettings.resolution = 24;
    streamSettings.input = BAPE_FCI_ID_INVALID;
    errCode = BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Enable MAI Bus */
    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_OP_CTRL_MAI_CFG, SPDIF_MODE, SPDIF_Format);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_OP_CTRL_MAI_CFG, ENABLE_MAI, Enable);
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_OP_CTRL_MAI_CFG);

    BAPE_SfifoGroup_P_GetDefaultCreateSettings(&sfifoCreateSettings);
    sfifoCreateSettings.numChannelPairs = 1;
    sfifoCreateSettings.ppmCorrection = false;
    errCode = BAPE_SfifoGroup_P_Create(handle->deviceHandle, &sfifoCreateSettings, &handle->hSfifo);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    BAPE_MixerGroup_P_GetDefaultCreateSettings(&mixerCreateSettings);
    mixerCreateSettings.numChannelPairs = 1;
    errCode = BAPE_MixerGroup_P_Create(handle->deviceHandle, &mixerCreateSettings, &handle->hMixer);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_SfifoGroup_P_GetSettings(handle->hSfifo, &sfifoSettings);
    sfifoSettings.dataWidth = 16;
    sfifoSettings.sampleRepeatEnabled = false;
    sfifoSettings.interleaveData = true;
    sfifoSettings.loopAround = true;
    BMEM_Heap_ConvertAddressToOffset(handle->deviceHandle->memHandle, handle->pMuteBuffer, &sfifoSettings.bufferInfo[0].base);
    sfifoSettings.bufferInfo[0].length = sizeof(g_pauseburst)*64;
    sfifoSettings.bufferInfo[0].wrpoint = sfifoSettings.bufferInfo[0].base;
    errCode = BAPE_SfifoGroup_P_SetSettings(handle->hSfifo, &sfifoSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_MixerGroup_P_GetSettings(handle->hMixer, &mixerSettings);
    mixerSettings.volumeControlEnabled = true;
    errCode = BAPE_MixerGroup_P_SetSettings(handle->hMixer, &mixerSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Link pauseburst SFIFO to the mixer's second input (actual data will be first input) */
    BAPE_MixerGroup_P_GetInputSettings(handle->hMixer, 1, &mixerInputSettings);
    BAPE_SfifoGroup_P_GetOutputFciIds(handle->hSfifo, &mixerInputSettings.input);
    mixerInputSettings.coefficients[0][0][0] = 0;
    mixerInputSettings.coefficients[0][0][1] = 0;
    mixerInputSettings.coefficients[0][1][0] = 0;
    mixerInputSettings.coefficients[0][1][1] = 0;
    mixerInputSettings.rampStep = 0x800000;
    errCode = BAPE_MixerGroup_P_SetInputSettings(handle->hMixer, 1, &mixerInputSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BKNI_EnterCriticalSection();
    BAPE_MaiOutput_P_SetMclk_isr(&handle->outputPort, BAPE_MclkSource_eHifidac0, 0, 256);
    BKNI_LeaveCriticalSection();

    /* Setup stereo data path as mixer output 0 */
    BAPE_MixerGroup_P_GetOutputFciIds(handle->hMixer, 0, &fciGroup);
    BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, GET_MAI_STEREO_STREAM_ID(0), &streamSettings);
    streamSettings.resolution = 24;
    streamSettings.input = fciGroup.ids[0];
    (void)BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, GET_MAI_STEREO_STREAM_ID(0), &streamSettings);
    /* Setup HBR data path as mixer output 1 */
    BAPE_MixerGroup_P_GetOutputFciIds(handle->hMixer, 1, &fciGroup);
    BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, GET_MAI_HBR_STREAM_ID(0), &streamSettings);
    streamSettings.resolution = 24;
    streamSettings.input = fciGroup.ids[0];
    (void)BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, GET_MAI_HBR_STREAM_ID(0), &streamSettings);
    /* Enable stereo data path consumer..producer */
    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, MAI_PAYLOAD_SEL, Stereo);
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, HBR_ENABLE, 0);
    BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_OP_CTRL_STREAM_ROUTE, HBR_HW_CH_STATUS_EN, 0);
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_OP_CTRL_STREAM_ROUTE);
    BKNI_EnterCriticalSection();
    BAPE_MaiOutput_P_SetCbits_Legacy_isr(handle);
    BKNI_LeaveCriticalSection();
    BAPE_Reg_P_Write(handle->deviceHandle, BCHP_AUD_FMM_OP_CTRL_ENABLE_SET, 1<<GET_MAI_STEREO_STREAM_ID(0));
    /* Enable data source and pauseburst ringbuffer */
    (void)BAPE_MixerGroup_P_StartOutput(handle->hMixer, 0);
    (void)BAPE_MixerGroup_P_StartInput(handle->hMixer, 1);  /* Input for pauseburst */
    (void)BAPE_SfifoGroup_P_Start(handle->hSfifo, false);   /* Sfifo for pauseburst */
    /* Save current state */
    handle->dataPath = BAPE_MaiOutputDataPath_eStereo;
    
    return errCode;
}

static void BAPE_MaiOutput_P_CloseHw_Legacy(BAPE_MaiOutputHandle handle)
{
    if ( handle->dataPath != BAPE_MaiOutputDataPath_eNone )
    {
        /* Stop stream 1 path */
        BAPE_SfifoGroup_P_Stop(handle->hSfifo);
        BAPE_MixerGroup_P_StopInput(handle->hMixer, 1);
        BAPE_MixerGroup_P_StopOutput(handle->hMixer, 0);
        BREG_Write32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR, 1<<GET_MAI_STEREO_STREAM_ID(handle->index));
        /* If HBR, stop the HBR output path */
        if ( handle->dataPath == BAPE_MaiOutputDataPath_eHbr )
        {
            BAPE_MixerGroup_P_StopOutput(handle->hMixer, 1);
            BREG_Write32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR, 1<<GET_MAI_HBR_STREAM_ID(handle->index));
        }
        handle->dataPath = BAPE_MaiOutputDataPath_eNone;
    }

    /* Disable microsequencer */
    BKNI_EnterCriticalSection();
    {
        BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_MS_CTRL_STRM_ENA, AUD_FMM_MS_CTRL_STRM_ENA, STREAM1_ENA, Disable);
    }
    BKNI_LeaveCriticalSection();

    if ( handle->hSfifo )
    {
        BAPE_SfifoGroup_P_Destroy(handle->hSfifo);
        handle->hSfifo = NULL;
    }
    if ( handle->hMixer )
    {
        BAPE_MixerGroup_P_Destroy(handle->hMixer);
        handle->hMixer = NULL;
    }
}

static void      BAPE_MaiOutput_P_SetCrossbar_Legacy(BAPE_MaiOutputHandle handle, BAPE_StereoMode stereoMode)
{
    uint32_t regVal;

    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);

    /* Setup the MAI output crossbar to properly assign the channel order for HDMI. */
    /* HDMI expects L R LFE C Ls Rs Lr Rr, we output L R Ls Rs C LFE Lr Rr */
    regVal = BREG_Read32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_OP_CTRL_MAI_CROSSBAR);
    regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT0)|
                BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT1)|
                BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT2)|
                BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT3)|
                BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT4)|
                BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT5)|
                BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT6)|
                BCHP_MASK(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT7));
    switch ( stereoMode )
    {
    default:
    case BAPE_StereoMode_eLeftRight:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT0, In0_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT1, In0_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT2, In1_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT3, In1_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT4, In2_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT5, In2_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT6, In3_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT7, In3_r);
        break;
    case BAPE_StereoMode_eLeftLeft:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT0, In0_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT1, In0_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT2, In1_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT3, In1_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT4, In2_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT5, In2_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT6, In3_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT7, In3_l);
        break;
    case BAPE_StereoMode_eRightRight:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT0, In0_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT1, In0_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT2, In1_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT3, In1_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT4, In2_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT5, In2_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT6, In3_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT7, In3_r);
        break;
    case BAPE_StereoMode_eRightLeft:
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT0, In0_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT1, In0_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT2, In1_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT3, In1_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT4, In2_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT5, In2_l);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT6, In3_r);
        regVal |= BCHP_FIELD_ENUM(AUD_FMM_OP_CTRL_MAI_CROSSBAR, OUT7, In3_l);
        break;
    }
    BREG_Write32(handle->deviceHandle->regHandle, BCHP_AUD_FMM_OP_CTRL_MAI_CROSSBAR, regVal);
}

static BERR_Code BAPE_MaiOutput_P_FillMuteBuffer(BAPE_MaiOutputHandle handle)
{
    unsigned i;
    uint16_t *pCached;

    (void)BMEM_Heap_ConvertAddressToCached(handle->deviceHandle->memHandle, handle->pMuteBuffer, (void **)&pCached);

    BDBG_MSG(("filling Burst RBUF addr %p/%p, size %u with underflowBurst=%d(%s)", 
        handle->pMuteBuffer, pCached, BAPE_P_MUTE_BUFFER_SIZE, handle->settings.underflowBurst,
        (handle->settings.underflowBurst==BAPE_SpdifBurstType_ePause)?"Pause Bursts" : (handle->settings.underflowBurst==BAPE_SpdifBurstType_eNull)?"NULL Bursts" : "Zeros"));

    if ( handle->settings.underflowBurst == BAPE_SpdifBurstType_ePause )
    {
        for ( i = 0; i < (BAPE_P_MUTE_BUFFER_SIZE/sizeof(g_pauseburst)); i++ )
        {
            pCached[6*i] = g_pauseburst[0];
            pCached[(6*i)+1] = g_pauseburst[1];
            pCached[(6*i)+2] = g_pauseburst[2];
            pCached[(6*i)+3] = g_pauseburst[3];
            pCached[(6*i)+4] = g_pauseburst[4];
            pCached[(6*i)+5] = g_pauseburst[5];
        }
    }
    else if ( handle->settings.underflowBurst == BAPE_SpdifBurstType_eNull )
    {
        for ( i = 0; i < (BAPE_P_MUTE_BUFFER_SIZE/sizeof(g_nullburst)); i++ )
        {
            pCached[4*i] = g_nullburst[0];
            pCached[(4*i)+1] = g_nullburst[1];
            pCached[(4*i)+2] = g_nullburst[2];
            pCached[(4*i)+3] = g_nullburst[3];
        }
    }
    else /* no bursts */
    {
        BKNI_Memset( pCached, 0, BAPE_P_MUTE_BUFFER_SIZE );
    }
    BMEM_Heap_FlushCache(handle->deviceHandle->memHandle, pCached, sizeof(g_pauseburst)*64);

    return BERR_SUCCESS;
}

static BERR_Code BAPE_MaiOutput_P_Open_Legacy(BAPE_MaiOutputHandle handle)
{
    handle->pMuteBuffer = BMEM_Heap_AllocAligned(handle->deviceHandle->memHandle, BAPE_P_MUTE_BUFFER_SIZE, 8, 0);
    if ( NULL == handle->pMuteBuffer )
    {
        return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
    }

    BAPE_MaiOutput_P_FillMuteBuffer(handle);
    
    return BERR_SUCCESS;
}

static void BAPE_MaiOutput_P_Close_Legacy(BAPE_MaiOutputHandle handle)
{
    if ( handle->pMuteBuffer )
    {
        BMEM_Heap_Free(handle->deviceHandle->memHandle, handle->pMuteBuffer);
    }
}

#else
/* 7429 style RDB */
static BERR_Code BAPE_MaiOutput_P_Open_IopOut(BAPE_MaiOutputHandle handle)
{
    BKNI_EnterCriticalSection();
    BAPE_MaiOutput_P_SetMclk_isr(&handle->outputPort, BAPE_MclkSource_ePll0, 0, 256);
    BKNI_LeaveCriticalSection();
    return BERR_SUCCESS;
}

static void BAPE_MaiOutput_P_Close_IopOut(BAPE_MaiOutputHandle handle)
{
    BSTD_UNUSED(handle);
    return;
}

static void BAPE_MaiOutput_P_SetCbits_IopOut_isr(BAPE_MaiOutputHandle handle)
{
    BAPE_Reg_P_FieldList regFieldList;
    uint32_t regAddr, regVal;
    unsigned validity = 0;
    unsigned dither = 0;
    unsigned hbr = 0;
    BAPE_DataType dataType = BAPE_DataType_ePcmStereo;
    bool compressed = false;
    bool compressedAsPcm = false;

    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);

    if ( handle->outputPort.mixer )
    {
        const BAPE_FMT_Descriptor     *pBfd = BAPE_Mixer_P_GetOutputFormat(handle->outputPort.mixer);
        
        dataType = pBfd->type;
        hbr = (unsigned)BAPE_FMT_P_IsHBR(pBfd);
        compressed = BAPE_FMT_P_IsCompressed(pBfd);
        compressedAsPcm = BAPE_FMT_P_IsDtsCdCompressed(pBfd);
    }

    BDBG_MSG(("Set MAI CBITS SR %u compressed %u, compressedAsPcm %u", handle->sampleRate, compressed, compressedAsPcm));

    /* Only set validity if we're outputting compressed */
    if ( compressed && !compressedAsPcm )
    {
        validity = 1;
    }
    else if ( !compressedAsPcm )
    {
        if ( handle->settings.ditherEnabled )
        {
            dither = 1;
        }
    }

    /* Program MAI format correctly */
    if ( handle->outputPort.mixer )
    {
        regAddr = BCHP_AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_WORD;
        BAPE_Reg_P_InitFieldList_isr(handle->deviceHandle, &regFieldList);

        if ( compressed && !compressedAsPcm )
        {
            if ( hbr )
            {
                BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_WORD, AUDIO_FORMAT, HBR_compressed_8_channel);
            }
            else
            {
                BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_WORD, AUDIO_FORMAT, Compressed_audio_2_channel);
            }
        }
        else
        {
            switch ( dataType )
            {
            case BAPE_DataType_ePcm5_1:
                BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_WORD, AUDIO_FORMAT, SPDIF_linearPCM_6_channel);
                break;
            case BAPE_DataType_ePcm7_1:
                BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_WORD, AUDIO_FORMAT, SPDIF_linearPCM_8_channel);
                break;
            default:
            case BAPE_DataType_ePcmStereo:
                BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_WORD, AUDIO_FORMAT, SPDIF_linearPCM_stereo);
                break;
            }
            BAPE_Reg_P_AddToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_WORD, SAMPLE_WIDTH, 0); /* 32 bits per sample */
        }

        if ( compressed || compressedAsPcm )
        {
            BAPE_Reg_P_AddToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_WORD, SAMPLE_WIDTH, 16);
        }

        if ( hbr )
        {
            BAPE_Reg_P_AddToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_WORD, 
                                          SAMPLE_RATE, BAPE_MaiOutput_P_SampleRateToMaiFormat(192000));
        }
        else
        {
            BAPE_Reg_P_AddToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_WORD, 
                                          SAMPLE_RATE, BAPE_MaiOutput_P_SampleRateToMaiFormat(handle->sampleRate));
        }

        BAPE_Reg_P_ApplyFieldList_isr(&regFieldList, regAddr);
    }

    /* hold cbits before changing dither or validity */
    BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL, AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL, HOLD_CSTAT, Hold);

    /* Tell the HW to hold the current CBITS and set dither correctly */
    regAddr = BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL;
    BAPE_Reg_P_InitFieldList_isr(handle->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddEnumToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL, HOLD_CSTAT, Hold);
    BAPE_Reg_P_AddToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL, DITHER_ENA, dither);
    BAPE_Reg_P_AddToFieldList_isr(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL, VALIDITY, validity);
    BAPE_Reg_P_ApplyFieldList_isr(&regFieldList, regAddr);

    /* Program channel status */
    if ( handle->settings.useRawChannelStatus )
    {
        regVal = 
            (uint32_t)handle->settings.rawChannelStatus[0] |
            (((uint32_t)handle->settings.rawChannelStatus[1])<<8) |
            (((uint32_t)handle->settings.rawChannelStatus[2])<<16) |
            (((uint32_t)handle->settings.rawChannelStatus[3])<<24);
        BAPE_Reg_P_Write_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CHANSTAT_0, regVal);
        regVal = (uint32_t)handle->settings.rawChannelStatus[4];
        BAPE_Reg_P_Write_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CHANSTAT_1, regVal);
    }
    else 
    {
        BAPE_Spdif_P_ChannelStatusBits cbits;

        BAPE_P_MapSpdifChannelStatusToBits_isr(&handle->outputPort, &handle->settings.channelStatus, &cbits);
        BAPE_Reg_P_Write_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CHANSTAT_0, cbits.bits[0]);
        BAPE_Reg_P_Write_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CHANSTAT_1, cbits.bits[1]);
    }

    /* Reload the new channel status in HW */
    BAPE_Reg_P_UpdateEnum_isr(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL, AUD_FMM_IOP_OUT_MAI_0_SPDIF_CTRL, HOLD_CSTAT, Update);

    return;
}

static void BAPE_MaiOutput_P_SetMclk_IopOut_isr(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio)
{
    BAPE_Reg_P_FieldList regFieldList;

    BAPE_MaiOutputHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(handle->offset == 0);

    /* Save the settings in case we need to re-apply them later. */
    handle->mclkInfo.mclkSource         = mclkSource;
    handle->mclkInfo.pllChannel         = pllChannel;
    handle->mclkInfo.mclkFreqToFsRatio  = mclkFreqToFsRatio;

    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);

    switch ( mclkSource )
    {
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0_PLLCLKSEL_PLL0_ch1
    case BAPE_MclkSource_ePll0:
        switch ( pllChannel )
        {
        case 0: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, PLL0_ch1); break;
        case 1: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, PLL0_ch2); break;
        case 2: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, PLL0_ch3); break;                
        default: (void) BERR_TRACE(BERR_NOT_SUPPORTED); break;
        }
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0_PLLCLKSEL_PLL1_ch1
    case BAPE_MclkSource_ePll1:
        switch ( pllChannel )
        {
        case 0: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, PLL1_ch1); break;
        case 1: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, PLL1_ch2); break;
        case 2: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, PLL1_ch3); break;                
        default: (void) BERR_TRACE(BERR_NOT_SUPPORTED); break;
        }
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0_PLLCLKSEL_PLL2_ch1
    case BAPE_MclkSource_ePll2:
        switch ( pllChannel )
        {
        case 0: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, PLL2_ch1); break;
        case 1: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, PLL2_ch2); break;
        case 2: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, PLL2_ch3); break;                
        default: (void) BERR_TRACE(BERR_NOT_SUPPORTED); break;
        }
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen0
    case BAPE_MclkSource_eNco0:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen0); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen1
    case BAPE_MclkSource_eNco1:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen1); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen2
    case BAPE_MclkSource_eNco2:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen2); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen3
    case BAPE_MclkSource_eNco3:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen3); 
        break;
#endif
#ifdef BCHP_AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0_PLLCLKSEL_Mclk_gen4
    case BAPE_MclkSource_eNco4:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, PLLCLKSEL, Mclk_gen4); 
        break;
#endif
    default:
        BDBG_ERR(("Unsupported clock source %u for MAI %u", mclkSource, handle->index));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    switch ( mclkFreqToFsRatio )
    {
    case 128: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, MCLK_RATE, MCLK_128fs_SCLK_64fs); break;
    case 256: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, MCLK_RATE, MCLK_256fs_SCLK_64fs); break;
    case 384: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, MCLK_RATE, MCLK_384fs_SCLK_64fs); break;
    case 512: BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0, MCLK_RATE, MCLK_512fs_SCLK_64fs); break;
    default:
        BDBG_ERR(("Unsupported MCLK Rate of %uFs", mclkFreqToFsRatio));
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        break;
    }
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_MAI_0_MCLK_CFG_0);
}

static void BAPE_MaiOutput_P_SetMute_IopOut(BAPE_OutputPort output, bool muted)
{
    BAPE_MaiOutputHandle handle;

    /* TODO: Set OVERWRITE_DATA as required or could use similar pauseburst as legacy */
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);

    BDBG_MSG(("MAI output (IopOut) mute %u - NOT IMPLEMENTED", muted));
}

static BERR_Code BAPE_MaiOutput_P_Enable_IopOut(BAPE_OutputPort output)
{
    BAPE_Reg_P_FieldList regFieldList;
    BAPE_MaiOutputHandle handle;
    BAPE_MixerHandle mixer;
    const BAPE_FMT_Descriptor *pFormat;
    uint32_t regAddr;
    unsigned numChannelPairs = 0;
    unsigned i;
    bool hbr;
    
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(false == handle->enabled);
    BDBG_ASSERT(NULL != output->mixer);

    mixer = output->mixer;

    BDBG_MSG(("Enabling %s", handle->name));

    pFormat = BAPE_Mixer_P_GetOutputFormat(output->mixer);
    BDBG_ASSERT(NULL != pFormat);
    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(pFormat);
    BDBG_ASSERT(numChannelPairs <= 4);
    hbr = (pFormat->type == BAPE_DataType_eIec61937x16) ? true : false;

    for ( i = 0; i < numChannelPairs; i++ )
    {
        regAddr = BAPE_Reg_P_GetArrayAddress(AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, i);
        BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
        #if 0   /* TODO: Same issue as older chips.  This does not appear to be stable. */
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, GROUP_ID, 0);
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, CHANNEL_GROUPING, 0xf>>(4-numChannelPairs));
        #else
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, GROUP_ID, i);
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, CHANNEL_GROUPING, 1);
        #endif
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, STREAM_BIT_RESOLUTION, Res_24_Bit); 
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, FCI_ID, output->sourceMixerFci.ids[i]);
        BAPE_Reg_P_ApplyFieldList(&regFieldList, regAddr);        
    }
    for ( ; i <= BCHP_AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i_ARRAY_END; i++ )
    {
        regAddr = BAPE_Reg_P_GetArrayAddress(AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, i);
        BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, GROUP_ID, i);
        BAPE_Reg_P_AddToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, CHANNEL_GROUPING, 1);
        BAPE_Reg_P_ApplyFieldList(&regFieldList, regAddr);        
    }

    /* Set MAI source */
    regAddr = BCHP_AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG;
    if ( hbr )
    {
        BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG, SPDIF_MODE, SPDIF_Format);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG, MAI_PAYLOAD_SEL, MAI_HBR);
    }
    else
    {
        BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG, SPDIF_MODE, SPDIF_Format);
        if ( numChannelPairs > 1 )
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG, MAI_PAYLOAD_SEL, MAI_Multi);
        }
        else
        {
            BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG, MAI_PAYLOAD_SEL, Stereo);
        }
    }
    BAPE_Reg_P_ApplyFieldList(&regFieldList, regAddr);

    /* Update CBITS */
    BKNI_EnterCriticalSection();
    BAPE_MaiOutput_P_SetCbits_isr(handle);
    BKNI_LeaveCriticalSection();

    /* Enable the interface. */
    for ( i = 0; i < numChannelPairs; i++ )
    {
        regAddr = BAPE_Reg_P_GetArrayAddress(AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, i);
        BAPE_Reg_P_UpdateField(handle->deviceHandle, regAddr, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, ENA, 1);
    }

    /* Enable the MAI bus - This must be done last to make older Onkyo receivers happy switching from DTS->PCM */
    /* TBD: How is this handled on 7429? */

    handle->enabled = true;
    return BERR_SUCCESS;
}

static void BAPE_MaiOutput_P_Disable_IopOut(BAPE_OutputPort output)
{
    BAPE_MaiOutputHandle handle;
    BAPE_MixerHandle mixer;
    /* BAPE_Reg_P_FieldList regFieldList; */
    uint32_t regAddr;
    const BAPE_FMT_Descriptor *pFormat;
    unsigned i, numChannelPairs = 0;
    
    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);
    BDBG_ASSERT(NULL != output->mixer);

    mixer = output->mixer;

    BDBG_MSG(("Disabling %s", handle->name));
    pFormat = BAPE_Mixer_P_GetOutputFormat(output->mixer);
    BDBG_ASSERT(NULL != pFormat);
    numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(pFormat);
    BDBG_ASSERT(numChannelPairs <= 4);

    /* Disable the MAI streams */
    for ( i = 0; i < numChannelPairs; i++ )
    {
        regAddr = BAPE_Reg_P_GetArrayAddress(AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, i);
        BAPE_Reg_P_UpdateField(handle->deviceHandle, regAddr, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, ENA, 0);
    }

    /* Clear out FCI ID's */
    for ( i = 0; i < numChannelPairs; i++ )
    {
        regAddr = BAPE_Reg_P_GetArrayAddress(AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, i);
        BAPE_Reg_P_UpdateField(handle->deviceHandle, regAddr, AUD_FMM_IOP_OUT_MAI_0_STREAM_CFG_i, FCI_ID, BAPE_FCI_ID_INVALID);
    }

#if 0
    /* wmp - not sure we want to do this?  what if we are transitioning to the same type again?  compressed->compressed, or legacy compressed -> HBR, etc */
    /* Revert to stereo mode */
    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG, SPDIF_MODE, SPDIF_Format);
    BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG, MAI_PAYLOAD_SEL, Stereo);
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG);
#endif

    handle->enabled = false;
}

static BERR_Code BAPE_MaiOutput_P_OpenHw_IopOut(BAPE_MaiOutputHandle handle)
{
    BERR_Code       errCode = BERR_SUCCESS;
    BAPE_Handle     deviceHandle;

    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    BDBG_ASSERT(handle->index == 0);

    /* Always source MAI timing from I2S Multi or MAI Multi.  */
    BAPE_Reg_P_UpdateEnum(handle->deviceHandle, BCHP_AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG, AUD_FMM_IOP_OUT_MAI_0_MAI_FORMAT_CFG, MAI_PAYLOAD_SEL, MAI_Multi);

    return errCode;
}

static void BAPE_MaiOutput_P_CloseHw_IopOut(BAPE_MaiOutputHandle handle)
{
    BSTD_UNUSED(handle);
    return;
}

static void      BAPE_MaiOutput_P_SetCrossbar_IopOut(BAPE_MaiOutputHandle handle, BAPE_StereoMode stereoMode)
{
    BAPE_Reg_P_FieldList regFieldList;

    BDBG_OBJECT_ASSERT(handle, BAPE_MaiOutput);

    /* Setup the MAI output crossbar to properly assign the channel order for HDMI. */
    /* HDMI expects L R LFE C Ls Rs Lr Rr, we output L R Ls Rs C LFE Lr Rr */
    BAPE_Reg_P_InitFieldList(handle->deviceHandle, &regFieldList);
    switch ( stereoMode )
    {
    default:
    case BAPE_StereoMode_eLeftRight:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT0_L, In0_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT0_R, In0_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT1_L, In1_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT1_R, In1_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT2_L, In2_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT2_R, In2_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT3_L, In3_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT3_R, In3_r);
        break;
    case BAPE_StereoMode_eLeftLeft:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT0_L, In0_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT0_R, In0_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT1_L, In1_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT1_R, In1_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT2_L, In2_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT2_R, In2_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT3_L, In3_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT3_R, In3_l);
        break;
    case BAPE_StereoMode_eRightRight:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT0_L, In0_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT0_R, In0_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT1_L, In1_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT1_R, In1_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT2_L, In2_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT2_R, In2_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT3_L, In3_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT3_R, In3_r);
        break;
    case BAPE_StereoMode_eRightLeft:
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT0_L, In0_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT0_R, In0_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT1_L, In1_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT1_R, In1_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT2_L, In2_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT2_R, In2_l);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT3_L, In3_r);
        BAPE_Reg_P_AddEnumToFieldList(&regFieldList, AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR, OUT3_R, In3_l);
        break;
    }
    BAPE_Reg_P_ApplyFieldList(&regFieldList, BCHP_AUD_FMM_IOP_OUT_MAI_0_MAI_CROSSBAR);
}
#endif

/***************************************************************************
    Define stub functions for when there are no I2S outputs. 
***************************************************************************/
#else
/* No MAI output interface.  Use Stubs. */

/**************************************************************************/

void BAPE_MaiOutput_GetDefaultSettings(
    BAPE_MaiOutputSettings *pSettings
    )
{
    BSTD_UNUSED(pSettings);
}

/**************************************************************************/

BERR_Code BAPE_MaiOutput_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_MaiOutputSettings *pSettings,
    BAPE_MaiOutputHandle *pHandle             /* [out] */
    )
{
    BSTD_UNUSED(deviceHandle);
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(pHandle);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

void BAPE_MaiOutput_Close(
    BAPE_MaiOutputHandle handle
    )
{
    BSTD_UNUSED(handle);
}

/**************************************************************************/

void BAPE_MaiOutput_GetSettings(
    BAPE_MaiOutputHandle handle,
    BAPE_MaiOutputSettings *pSettings     /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

/**************************************************************************/

BERR_Code BAPE_MaiOutput_SetSettings(
    BAPE_MaiOutputHandle handle,
    const BAPE_MaiOutputSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

void BAPE_MaiOutput_GetOutputPort(
    BAPE_MaiOutputHandle handle,
    BAPE_OutputPort *pOutputPort        /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pOutputPort);
}

/**************************************************************************/

void BAPE_MaiOutput_GetInterruptHandlers(
    BAPE_MaiOutputHandle handle,
    BAPE_MaiOutputInterruptHandlers *pInterrupts    /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pInterrupts);
}

/**************************************************************************/

BERR_Code BAPE_MaiOutput_SetInterruptHandlers(
    BAPE_MaiOutputHandle handle,
    const BAPE_MaiOutputInterruptHandlers *pInterrupts
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pInterrupts);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

/**************************************************************************/

BERR_Code BAPE_MaiOutput_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BSTD_UNUSED(bapeHandle);
    return BERR_SUCCESS;
}

#endif
