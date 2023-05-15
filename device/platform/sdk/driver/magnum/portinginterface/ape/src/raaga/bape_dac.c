/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_dac.c $
 * $brcm_Revision: Hydra_Software_Devel/SW7425-5116-12.4/1 $
 * $brcm_Date: 12/6/13 5:15p $
 *
 * Module Description: Audio DAC Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_dac.c $
 * 
 * Hydra_Software_Devel/SW7425-5116-12.4/1   12/6/13 5:15p mpeteri
 * SW7425-5116: back port to 12.4
 * 
 * Hydra_Software_Devel/SW7425-5116/1   12/6/13 5:10p mpeteri
 * SW7425-5116: Don't share the Hifidac RM if we might encounter
 * samplrates > 96k
 * 
 * Hydra_Software_Devel/33   10/17/12 3:32p mpeteri
 * SW7445-19: merge to main
 * 
 * Hydra_Software_Devel/SW7445-19/2   10/17/12 3:25p mpeteri
 * SW7445-19: fix compiler macro
 * 
 * Hydra_Software_Devel/32   10/15/12 3:00p mpeteri
 * SW7445-19: merge to main
 * 
 * Hydra_Software_Devel/SW7445-19/1   10/3/12 3:35p mpeteri
 * SW7445-19: RDB changes for 7445
 * 
 * Hydra_Software_Devel/31   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:49p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/30   3/6/12 2:57p gskerl
 * SW7425-2570: Renamed OutputPortObject substruct from connector to
 * outputPort.
 * 
 * Hydra_Software_Devel/29   2/23/12 10:53a jgarrett
 * SW7405-5003: Removing BKNI_Memcmp where structures aren't copied with
 * BKNI_Memcpy
 * 
 * Hydra_Software_Devel/28   2/8/12 1:48p gskerl
 * SW7425-2339: Moved DAC dynamic power management from enable/disable to
 * open/close (because it causes a small pop that's noticeable when the
 * receiver is at 100% volume)
 * 
 * Hydra_Software_Devel/27   1/27/12 5:11p jgarrett
 * SW7429-55: Changing ncoRequired to builtinNco
 * 
 * Hydra_Software_Devel/26   1/27/12 4:52p jgarrett
 * SW7429-55: Setting ncoRequired field for 7429 to indicate only NCO
 * outputs may share a mixer with DACs
 * 
 * Hydra_Software_Devel/25   11/30/11 6:36p jgarrett
 * SW7429-18: Fixing DAC powerup and init on 7429
 * 
 * Hydra_Software_Devel/24   11/16/11 3:11p gskerl
 * SW7429-18: Corrected accessing of TESTTONE_SAMPLE register array.
 * 
 * Hydra_Software_Devel/23   11/14/11 3:31p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/3   10/25/11 5:41p jgarrett
 * SW7429-18: Removing DAC MCLK output for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/2   10/25/11 10:42a jgarrett
 * SW7429-18: Removing RM warning for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/22   9/30/11 11:30a gskerl
 * SW7231-129: Updated comments.
 * 
 * Hydra_Software_Devel/21   9/28/11 5:36p gskerl
 * SW7231-129: Renamed BAPE_Dac_ApplySettings() to
 * BAPE_Dac_P_ApplySettings()
 * 
 * Hydra_Software_Devel/20   9/19/11 5:28p gskerl
 * SW7231-129: Added support for recovering hardware state after power
 * standby/resume.
 * 
 * Hydra_Software_Devel/19   9/16/11 6:49p gskerl
 * SW7231-129: Refactored to put hardware and software initialization into
 * separate functions.
 * 
 * Hydra_Software_Devel/18   8/24/11 11:53a jgarrett
 * SW7425-724: Adding RF Audio Encoder
 * 
 * Hydra_Software_Devel/17   8/18/11 3:19p jgarrett
 * SWDTV-8296: Handling IOP/OP mismatches in stream IDs for DTV chips
 * 
 * Hydra_Software_Devel/16   8/16/11 2:44p gskerl
 * SW7231-246: Re-enabled and fixed DAC dynamic power management
 * 
 * Hydra_Software_Devel/15   8/12/11 3:57p gskerl
 * SW7231-246: Temporarily disabling audio DAC powerdown because when it
 * is powered down for more than a minute, it takes a really long time
 * (like 20 seconds) to power back up
 * 
 * Hydra_Software_Devel/14   6/8/11 6:55p gskerl
 * SW7425-321: Changed BAPE_Dac_Open() to mute RF Mod output.
 * 
 * Hydra_Software_Devel/13   5/20/11 7:24p gskerl
 * SW7231-128: Added DAC power control
 * 
 * Hydra_Software_Devel/12   5/20/11 5:16p jgarrett
 * SW7425-402: Adding error code to handle output enable callback failing.
 * 
 * Hydra_Software_Devel/11   5/3/11 10:45a gskerl
 * SW7422-354: Added index and type args to APE_P_InitOutputPort macro
 * 
 * Hydra_Software_Devel/10   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/9   4/6/11 1:23a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/2   4/5/11 7:13p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:49p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/8   3/28/11 2:54p gskerl
 * SW7422-375: Fixed PEAK_GAIN handling
 * 
 * Hydra_Software_Devel/7   3/25/11 12:43p gskerl
 * SW7422-375: Removed peakA1 and peakA2 from DAC settings
 * 
 * Hydra_Software_Devel/6   3/24/11 1:40p gskerl
 * SW7422-375: Added handling for DAC peaking filter settings
 * 
 * Hydra_Software_Devel/5   3/22/11 2:56p gskerl
 * SW7422-146: Changed audio output connector callbacks to take the output
 * connector as an argument
 * 
 * Hydra_Software_Devel/4   2/28/11 11:29a gskerl
 * SW7422-146: Added support for multiple DACs.
 * 
 * Hydra_Software_Devel/3   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/2   1/6/11 5:15p jgarrett
 * SW7422-146: Adding DAC init register setup
 * 
 * Hydra_Software_Devel/1   12/16/10 4:04p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"
#ifdef BCHP_HIFIDAC_CTRL0_REG_START
/* Old-style RDB */
#include "bchp_hifidac_ctrl0.h"
#include "bchp_hifidac_rm0.h"
#include "bchp_aud_fmm_op_ctrl.h"

#if BAPE_CHIP_MAX_DACS > 1
#include "bchp_hifidac_ctrl1.h"
#include "bchp_hifidac_rm1.h"
#if BAPE_CHIP_MAX_DACS > 2
#include "bchp_hifidac_ctrl2.h"
#include "bchp_hifidac_rm2.h"
#endif
#endif

#else
/* 7429 and later */
#include "bchp_hifidac_ctrl_0.h"
#include "bchp_hifidac_rm_0.h"
#include "bchp_aud_fmm_iop_out_dac_ctrl_0.h"

#if BAPE_CHIP_MAX_DACS > 1
#include "bchp_hifidac_ctrl_1.h"
#include "bchp_hifidac_rm_1.h"
#include "bchp_aud_fmm_iop_out_dac_ctrl_1.h"
#if BAPE_CHIP_MAX_DACS > 2
#include "bchp_hifidac_ctrl_2.h"
#include "bchp_hifidac_rm_2.h"
#include "bchp_aud_fmm_iop_out_dac_ctrl_2.h"
#endif
#endif

#endif


BDBG_MODULE(bape_dac);

BDBG_OBJECT_ID(BAPE_Dac);

typedef struct BAPE_Dac
{
    BDBG_OBJECT(BAPE_Dac)
    BAPE_Handle deviceHandle;
    BAPE_DacSettings settings;
    unsigned index;
    BAPE_OutputPortObject outputPort;
    unsigned sampleRate;
    bool rfMode;
    bool enabled;
    BAVC_Timebase timebase;
    char name[7];   /* DAC %d */
} BAPE_Dac;

/* Define some default register values (in case they get used in more than one place) */
#define  BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_PEAK_GAIN_PEAK_GAIN     0xAA90
#define  BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_PEAK_A1_PEAK_A1         0x1C402
#define  BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_PEAK_A2_PEAK_A2         0x32CB4
#define  BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_RANGE_ASRCOUT           0x02
#define  BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_DAC_VOLUMECFG_STEPSIZE  0x04

/* The peaking filter must be reprogrammed if the source is RF data */
#define  BAPE_DAC_RF_HIFIDAC_CTRL0_PEAK_GAIN_PEAK_GAIN        0xe100    /* Taken from brap_hifidac.c */
#define  BAPE_DAC_RF_HIFIDAC_CTRL0_PEAK_A1_PEAK_A1            0
#define  BAPE_DAC_RF_HIFIDAC_CTRL0_PEAK_A2_PEAK_A2            0
#define  BAPE_DAC_RF_HIFIDAC_CTRL0_RANGE_ASRCOUT              0x06
#define  BAPE_DAC_RF_HIFIDAC_CTRL0_DAC_VOLUMECFG_STEPSIZE     0xfff

/* Static function prototypes */
static void         BAPE_Dac_P_SetRateManager_isr(BAPE_DacHandle handle, unsigned sampleRate);
static void         BAPE_Dac_P_SyncVolume(BAPE_DacHandle handle);
static void         BAPE_Dac_P_PowerDown(BAPE_Handle bapeHandle, unsigned dacIndex);
static void         BAPE_Dac_P_PowerUp(BAPE_Handle bapeHandle, unsigned dacIndex);
static BERR_Code    BAPE_Dac_P_OpenHw(BAPE_DacHandle hDac);
static BERR_Code    BAPE_Dac_P_ApplySettings(BAPE_DacHandle handle, const BAPE_DacSettings *pSettings, bool force);

/* Output port callbacks */
static void         BAPE_Dac_P_SetTimingParams_isr(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase);
static void         BAPE_Dac_P_SetMute(BAPE_OutputPort output, bool muted, bool sync);
static BERR_Code    BAPE_Dac_P_Enable(BAPE_OutputPort output);
static void         BAPE_Dac_P_Disable(BAPE_OutputPort output);


#ifdef BCHP_HIFIDAC_CTRL0_REG_START
#define DAC_MASK(prefix, suffix, field) (BCHP_MASK(prefix##0##suffix, field))
#define DAC_FIELD_DATA(prefix, suffix, field, val) (BCHP_FIELD_DATA(prefix##0##suffix, field, val))
#define DAC_FIELD_ENUM(prefix, suffix, field, val) (BCHP_FIELD_ENUM(prefix##0##suffix, field, val))
#define DAC_GET_FIELD_DATA(val, prefix, suffix, field) (BCHP_GET_FIELD_DATA(val, prefix##0##suffix, field))

#if BAPE_CHIP_MAX_DACS > 1
    #define  GET_DAC_REG_ADDR2(prefix,idx)         (prefix##0         + (prefix##1         - prefix##0        ) * idx )
    #define  GET_DAC_REG_ADDR3(prefix,idx,suffix)  (prefix##0##suffix + (prefix##1##suffix - prefix##0##suffix) * idx )
#ifdef BCHP_AUD_FMM_IOP_CTRL_MIX_IOP_IDMAP98
    /* 35230 style*/
    #define  GET_DAC_OP_STREAM_ID(idx) ((idx) == 0 ? 2 :(idx) == 1 ? 8 : (idx) == 2 ? 9 : (BDBG_ASSERT(0), 0))
    #define  GET_DAC_IOP_STREAM_ID(idx) ((idx) == 0 ? 2 :(idx) == 1 ? 8 : (idx) == 2 ? 13 : (BDBG_ASSERT(0), 0))    /* DTV chips have loopback stuck in the middle of the IOP indexes */
#else
    /* 7422/7425 style */
    #define  GET_DAC_OP_STREAM_ID(idx) ((idx) == 0 ? 2 :(idx) == 1 ? 9 : (BDBG_ASSERT(0), 0))
    #define  GET_DAC_IOP_STREAM_ID(idx) GET_DAC_OP_STREAM_ID(idx) /* STB chips have this right */
#endif
#else
    #define  GET_DAC_REG_ADDR2(prefix,idx       )  (prefix##0         )
    #define  GET_DAC_REG_ADDR3(prefix,idx,suffix)  (prefix##0##suffix )
    #define  GET_DAC_OP_STREAM_ID(idx) (2)         /* DAC 0 seems to always be stream 2. */
    #define  GET_DAC_IOP_STREAM_ID(idx) GET_DAC_OP_STREAM_ID(idx)
#endif

#else

#define DAC_MASK(prefix, suffix, field) (BCHP_MASK(prefix##_0##suffix, field))
#define DAC_FIELD_DATA(prefix, suffix, field, val) (BCHP_FIELD_DATA(prefix##_0##suffix, field, val))
#define DAC_FIELD_ENUM(prefix, suffix, field, val) (BCHP_FIELD_ENUM(prefix##_0##suffix, field, val))
#define DAC_GET_FIELD_DATA(val, prefix, suffix, field) (BCHP_GET_FIELD_DATA(val, prefix##_0##suffix, field))

#define GET_DAC_OP_STREAM_ID(idx)  (0xffffffff)
#define GET_DAC_IOP_STREAM_ID(idx) (0xffffffff)

#if BAPE_CHIP_MAX_DACS > 1
    #define  GET_DAC_REG_ADDR2(prefix,idx)         (prefix##_0         + (prefix##_1         - prefix##_0        ) * idx )
    #define  GET_DAC_REG_ADDR3(prefix,idx,suffix)  (prefix##_0##suffix + (prefix##_1##suffix - prefix##_0##suffix) * idx )
#else
    #define  GET_DAC_REG_ADDR2(prefix,idx       )  (prefix##_0         )
    #define  GET_DAC_REG_ADDR3(prefix,idx,suffix)  (prefix##_0##suffix )
#endif
#endif


/***************************************************************************
        Public APIs: From bape_output.h
***************************************************************************/

void BAPE_Dac_GetDefaultSettings(
    BAPE_DacSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->stereoMode = BAPE_StereoMode_eLeftRight;
    pSettings->muteType = BAPE_DacMuteType_eDrive0;
    pSettings->customLeftValue = 0x3333;
    pSettings->customRightValue = 0xcccc;
    pSettings->testTone.sharedSamples = true;
    pSettings->testTone.sampleRate = 48000;

    pSettings->peakGain = BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_PEAK_GAIN_PEAK_GAIN;
}

/**************************************************************************/

BERR_Code BAPE_Dac_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_DacSettings *pSettings,
    BAPE_DacHandle *pHandle             /* [out] */
    )
{
    BERR_Code errCode;
    BAPE_DacHandle handle;
    BAPE_DacSettings defaultSettings;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);
    
    BDBG_MSG(("%s: Opening DAC Output: %u", __FUNCTION__, index));

    *pHandle = NULL;    /* Set up to return null handle in case of error. */

    if ( index >= BAPE_CHIP_MAX_DACS )
    {
        BDBG_ERR(("Request to open DAC %d but chip only has %u DACs", index, BAPE_CHIP_MAX_DACS));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( deviceHandle->dacs[index] )
    {
        BDBG_ERR(("DAC %d already open", index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Allocate the device structure, then fill in all the fields. */
    handle = BKNI_Malloc(sizeof(BAPE_Dac));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    BKNI_Memset(handle, 0, sizeof(BAPE_Dac));
    BDBG_OBJECT_SET(handle, BAPE_Dac);
    handle->deviceHandle = deviceHandle;
    handle->index = index;
    BAPE_P_InitOutputPort(&handle->outputPort, BAPE_OutputPortType_eDac, index, handle);
    BAPE_FMT_P_EnableSource(&handle->outputPort.capabilities, BAPE_DataSource_eFci);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableType(&handle->outputPort.capabilities, BAPE_DataType_ePcmRf);
    handle->outputPort.builtinNco = true;
#ifdef BCHP_HIFIDAC_CTRL0_REG_START
    /* Starting with 7429, DACn is not a valid MCLK source - it is private to the DAC itself */
    if ( handle->deviceHandle->settings.maxPcmSampleRate > 48000 )
    {
        handle->outputPort.mclkOutput = BAPE_MclkSource_eNone;
    }
    else
    {
        handle->outputPort.mclkOutput = BAPE_MclkSource_eHifidac0 + index;
    }
#endif
    handle->outputPort.setTimingParams_isr = BAPE_Dac_P_SetTimingParams_isr;
    handle->outputPort.setMute = BAPE_Dac_P_SetMute;
    handle->outputPort.enable = BAPE_Dac_P_Enable;
    handle->outputPort.disable = BAPE_Dac_P_Disable;
    BKNI_Snprintf(handle->name, sizeof(handle->name), "DAC %u", index);
    handle->outputPort.pName = handle->name;

    /* Setup to 48k, muted by default */
    BKNI_EnterCriticalSection();
    BAPE_Dac_P_SetTimingParams_isr(&handle->outputPort, 48000, BAVC_Timebase_e0);
    BKNI_LeaveCriticalSection();
    BAPE_Dac_P_SetMute(&handle->outputPort, false, false);

    /* Init to specified settings */
    if ( NULL == pSettings )
    {
        BAPE_Dac_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    errCode = BAPE_Dac_P_OpenHw(handle);
    if ( errCode )
    {
        BAPE_Dac_Close(handle);
        return BERR_TRACE(errCode);
    }

    errCode = BAPE_Dac_SetSettings(handle, pSettings);
    if ( errCode )
    {
        BAPE_Dac_Close(handle);
        return BERR_TRACE(errCode);
    }

    *pHandle = handle;
    handle->deviceHandle->dacs[index] = handle;
    return BERR_SUCCESS;
}

/**************************************************************************/

void BAPE_Dac_Close(
    BAPE_DacHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);

    /* Make sure we're not still connected to anything */
    if ( handle->outputPort.mixer )
    {
        BDBG_ERR(("Cannot close dac %p (%d), still connected to mixer %p", handle, handle->index, handle->outputPort.mixer));
        BDBG_ASSERT(NULL == handle->outputPort.mixer);
        return;
    }

    /* Turn off power to the DAC. */
    BAPE_Dac_P_PowerDown(handle->deviceHandle, handle->index);

    handle->deviceHandle->dacs[handle->index] = NULL;
    BDBG_OBJECT_DESTROY(handle, BAPE_Dac);
    BKNI_Free(handle);    
}

/**************************************************************************/

void BAPE_Dac_GetSettings(
    BAPE_DacHandle handle,
    BAPE_DacSettings *pSettings     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

/**************************************************************************/

BERR_Code BAPE_Dac_SetSettings(
    BAPE_DacHandle handle,
    const BAPE_DacSettings *pSettings
    )
{
    BERR_Code   errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);
    BDBG_ASSERT(NULL != pSettings);

    errCode = BAPE_Dac_P_ApplySettings(handle, pSettings, false); /* false => don't force (only update HW for changes) */
    return errCode;
}

/**************************************************************************/

void BAPE_Dac_GetOutputPort(
    BAPE_DacHandle handle,
    BAPE_OutputPort *pOutputPort        /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);
    BDBG_ASSERT(NULL != pOutputPort);
    *pOutputPort = &handle->outputPort;
}


/***************************************************************************
        BAPE Internal APIs: From bape_fmm_priv.h
***************************************************************************/

BERR_Code BAPE_Dac_P_ResumeFromStandby(BAPE_Handle bapeHandle)
{
    BERR_Code   errCode = BERR_SUCCESS;
    unsigned    dacIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    /* For each opened DAC, call the functions necessary to restore the hardware to it's appropriate state.
     * The DACs have already been initialized by BAPE_P_InitDacHw() before this function gets called.
     */
    for ( dacIndex=0 ; dacIndex<BAPE_CHIP_MAX_DACS ; dacIndex++ )
    {
        if ( bapeHandle->dacs[dacIndex] )       /* If this DAC is open... */
        {
            BAPE_DacHandle hDac = bapeHandle->dacs[dacIndex];

            /* Put the HW into the generic open state. */
            errCode = BAPE_Dac_P_OpenHw(hDac);
            if ( errCode ) return BERR_TRACE(errCode);
            
            /* Now apply changes for the settings struct. */
            errCode = BAPE_Dac_P_ApplySettings(hDac, &hDac->settings, true);   /* true => force update of HW */
            if ( errCode ) return BERR_TRACE(errCode);

            /* Now restore the timebase and sampleRate from the values saved in the device struct. */
            BKNI_EnterCriticalSection();
                BAPE_Dac_P_SetTimingParams_isr(&hDac->outputPort, hDac->sampleRate, hDac->timebase);
                BAPE_Dac_P_SetRateManager_isr(hDac, hDac->sampleRate);
            BKNI_LeaveCriticalSection();
        }
    }
    return errCode;
}

/**************************************************************************/

BERR_Code BAPE_P_InitDacHw( BAPE_Handle bapeHandle )
{
    unsigned   dacIndex;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);

    BDBG_MSG(("Initializing DAC registers"));

    for ( dacIndex=0 ; dacIndex<BAPE_CHIP_MAX_DACS ; dacIndex++ )
    {
        /* Power down the DAC, it will get powered up at enable time. */
        BAPE_Dac_P_PowerDown(bapeHandle, dacIndex);
    }

    return BERR_SUCCESS;
}

/***************************************************************************
        Private callbacks: Protyped above
***************************************************************************/

static void BAPE_Dac_P_SetTimingParams_isr(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase)
{
    uint32_t regAddr, regVal;
    BAPE_DacHandle handle;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);

    /* Update timebase */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_RM, handle->index, _CONTROL );
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_RM,_CONTROL,TIMEBASE);
    switch ( timebase )
    {
    case BAVC_Timebase_e0:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_RM,_CONTROL, TIMEBASE, TIMEBASE_0);
        break;
    case BAVC_Timebase_e1:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_RM,_CONTROL, TIMEBASE, TIMEBASE_1);
        break;
    case BAVC_Timebase_e2:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_RM,_CONTROL, TIMEBASE, TIMEBASE_2);
        break;
    case BAVC_Timebase_e3:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_RM,_CONTROL, TIMEBASE, TIMEBASE_3);
        break;
    }
    BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);

    /* Update Sample Rate */
    handle->sampleRate = sampleRate;
    handle->timebase   = timebase;
    BAPE_Dac_P_SetRateManager_isr(handle, sampleRate);
}

/**************************************************************************/

static void BAPE_Dac_P_SetMute(BAPE_OutputPort output, bool muted, bool sync)
{
    BAPE_DacHandle handle;
    uint32_t regAddr, regVal;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _MUTECTRL_DACONLY );
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_MUTECTRL_DACONLY, MUTEDAC);
    if ( muted )
    {
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_MUTECTRL_DACONLY, MUTEDAC, Ramp_mute);
    }
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
    if ( sync )
    {
        BAPE_Dac_P_SyncVolume(handle);
    }
}

/**************************************************************************/

static BERR_Code BAPE_Dac_P_Enable(BAPE_OutputPort output)
{
    BAPE_DacHandle handle;
    uint32_t regAddr, regVal;
    uint32_t peakGain, peakA1, peakA2, asrcout, stepSize;
    unsigned streamId;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);
    BDBG_ASSERT(false == handle->enabled);

    streamId = GET_DAC_IOP_STREAM_ID(handle->index);
    BDBG_MSG(("Enabling DAC %u [stream %u]", handle->index, streamId));

#ifdef BCHP_AUD_FMM_IOP_CTRL_REG_START
    {
        BAPE_IopStreamSettings streamSettings;
        BERR_Code errCode;
        /* Write source FCI to IOP */
        BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
        streamSettings.resolution = 24;
        streamSettings.input = output->sourceMixerFci.ids[BAPE_ChannelPair_eLeftRight];         /* Take source FCI provided from mixer */
        errCode = BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
#else
    regAddr = GET_DAC_REG_ADDR3(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL, handle->index, _STREAM_CFG_0);
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(AUD_FMM_IOP_OUT_DAC_CTRL,_STREAM_CFG_0,FCI_ID);
    regVal |= DAC_FIELD_DATA(AUD_FMM_IOP_OUT_DAC_CTRL,_STREAM_CFG_0, FCI_ID, output->sourceMixerFci.ids[BAPE_ChannelPair_eLeftRight]);
    regVal &= ~DAC_MASK(AUD_FMM_IOP_OUT_DAC_CTRL,_STREAM_CFG_0,STREAM_BIT_RESOLUTION);
    regVal |= DAC_FIELD_ENUM(AUD_FMM_IOP_OUT_DAC_CTRL,_STREAM_CFG_0, STREAM_BIT_RESOLUTION, Res_24_Bit);
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
#endif

    handle->rfMode = BAPE_Mixer_P_GetOutputDataType(output->mixer) == BAPE_DataType_ePcmRf ? true : false;
    if ( handle->rfMode )
    {
        /* Setup peaking filter for RF audio */
        peakGain = BAPE_DAC_RF_HIFIDAC_CTRL0_PEAK_GAIN_PEAK_GAIN;
        peakA1 = BAPE_DAC_RF_HIFIDAC_CTRL0_PEAK_A1_PEAK_A1;
        peakA2 = BAPE_DAC_RF_HIFIDAC_CTRL0_PEAK_A2_PEAK_A2;
        asrcout = BAPE_DAC_RF_HIFIDAC_CTRL0_RANGE_ASRCOUT;
        stepSize = BAPE_DAC_RF_HIFIDAC_CTRL0_DAC_VOLUMECFG_STEPSIZE;
    }
    else
    {
        /* Setup peaking filter for PCM audio */
        peakGain = handle->settings.peakGain;
        peakA1 = BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_PEAK_A1_PEAK_A1;
        peakA2 = BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_PEAK_A2_PEAK_A2;
        asrcout = BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_RANGE_ASRCOUT;        
        stepSize = BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_DAC_VOLUMECFG_STEPSIZE;
    }
    
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _PEAK_GAIN );
    regVal = DAC_FIELD_DATA(HIFIDAC_CTRL,_PEAK_GAIN, PEAK_GAIN, peakGain);
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _PEAK_A1 );
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr );
    regVal &= ~(DAC_MASK(HIFIDAC_CTRL,_PEAK_A1, PEAK_A1));
    regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_PEAK_A1, PEAK_A1, peakA1);
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _PEAK_A2 );
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr );
    regVal &= ~(DAC_MASK(HIFIDAC_CTRL,_PEAK_A2, PEAK_A2));
    regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_PEAK_A2, PEAK_A2, peakA2);
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
                
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _RANGE );
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_RANGE, ASRCOUT);
    regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_RANGE, ASRCOUT, asrcout);
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
    
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _DAC_VOLUMECFG );
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr );
    regVal &= ~(DAC_MASK(HIFIDAC_CTRL,_DAC_VOLUMECFG, STEPSIZE));
    regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_DAC_VOLUMECFG, STEPSIZE, stepSize);
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);

    /* Write the enable bit in the OP (only stereo) */
#ifdef BCHP_AUD_FMM_IOP_CTRL_REG_START
    streamId = GET_DAC_OP_STREAM_ID(handle->index);
    BDBG_MSG(("Writing %x to enable set", (BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_SET, STREAM0_ENA))<<(streamId)));
    BREG_Write32(handle->deviceHandle->regHandle, 
                 BCHP_AUD_FMM_OP_CTRL_ENABLE_SET,
                 (BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_SET, STREAM0_ENA))<<(streamId));
#else
    regAddr = GET_DAC_REG_ADDR3(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL, handle->index, _STREAM_CFG_0);
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(AUD_FMM_IOP_OUT_DAC_CTRL,_STREAM_CFG_0,ENA);
    regVal |= DAC_FIELD_DATA(AUD_FMM_IOP_OUT_DAC_CTRL,_STREAM_CFG_0, ENA, 1);
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
#endif

    BAPE_Dac_P_SetMute(output, false, false);
      
    handle->enabled = true;

    return BERR_SUCCESS;
}

/**************************************************************************/

static void BAPE_Dac_P_Disable(BAPE_OutputPort output)
{
    BAPE_DacHandle handle;
    unsigned streamId;

    BDBG_OBJECT_ASSERT(output, BAPE_OutputPort);

    handle = output->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);

    streamId = GET_DAC_OP_STREAM_ID(handle->index);
    BDBG_MSG(("Disabling DAC %u [stream %u]", handle->index, streamId));

    if ( !handle->settings.testTone.enabled )
    {
        BAPE_Dac_P_SetMute(output, true, true);
    }

#ifdef BCHP_AUD_FMM_IOP_CTRL_REG_START
    {
        BAPE_IopStreamSettings streamSettings;
        BERR_Code errCode;
        /* Clear the enable bit in the OP */
        BDBG_MSG(("Writing %x to enable clear", (BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_SET, STREAM0_ENA))<<(streamId)));
        BREG_Write32(handle->deviceHandle->regHandle, 
                     BCHP_AUD_FMM_OP_CTRL_ENABLE_CLEAR,
                     BCHP_MASK(AUD_FMM_OP_CTRL_ENABLE_CLEAR, STREAM0_ENA)<<streamId);

        /* Reset source FCI to Invalid */
        streamId = GET_DAC_IOP_STREAM_ID(handle->index);
        BAPE_Iop_P_GetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
        streamSettings.input = BAPE_FCI_ID_INVALID;
        errCode = BAPE_Iop_P_SetStreamSettings(handle->deviceHandle, streamId, &streamSettings);
        BDBG_ASSERT(BERR_SUCCESS == errCode);
    }
#else
    {
        uint32_t regAddr, regVal;

        regAddr = GET_DAC_REG_ADDR3(BCHP_AUD_FMM_IOP_OUT_DAC_CTRL, handle->index, _STREAM_CFG_0);
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        regVal &= ~DAC_MASK(AUD_FMM_IOP_OUT_DAC_CTRL,_STREAM_CFG_0,ENA);
        regVal &= ~DAC_MASK(AUD_FMM_IOP_OUT_DAC_CTRL,_STREAM_CFG_0,FCI_ID);
        regVal |= DAC_FIELD_DATA(AUD_FMM_IOP_OUT_DAC_CTRL,_STREAM_CFG_0, FCI_ID, BAPE_FCI_ID_INVALID);
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
    }
#endif

    handle->enabled = false;
    handle->rfMode = false;
}

/***************************************************************************
        Private functions: Protyped above
***************************************************************************/

static void BAPE_Dac_P_SetRateManager_isr(BAPE_DacHandle handle, unsigned sampleRate)
{
    uint32_t regAddr, regVal, numerator, denominator, sampleInc, phaseInc;
    unsigned rateNum;

    rateNum = sampleRate;

    BDBG_MSG(("Setting DAC %u to %u Hz", handle->index, sampleRate));

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _CONFIG );
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_CONFIG, FIR_MODE);
    if ( rateNum > 96000 )
    {
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, FIR_MODE, Audio_over100kHz);
    }
    else
    {
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, FIR_MODE, Audio_under100kHz);
    }
    BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);

    /* Program the Oversampling frequency */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _RATEMGRCFG );
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_RATEMGRCFG, OVERSAMPLE);
    if ( rateNum <= 48000 )
    {
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_RATEMGRCFG, OVERSAMPLE, Fs256);
    }
    else
    {
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_RATEMGRCFG, OVERSAMPLE, Fs64);
    }
    BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);

    /* These chips run at 108 MHz */
    switch ( sampleRate )
    {
    case 128000:
    case 32000:
        numerator = 303;
        denominator = 1024;
        sampleInc = 3;
        phaseInc = 0x0009b583;
        break;
    case 64000:
        numerator = 303;
        denominator = 512;
        sampleInc = 6;
        phaseInc = 0x0004dac2;
        break;
    case 176400:
    case 44100:
        numerator = 307;
        denominator = 784;
        sampleInc = 2;
        phaseInc = 0x000d6159;
        break;
    case 88200:
        numerator = 307;
        denominator = 392;
        sampleInc = 4;
        phaseInc = 0x0006b0ad;
        break;
    default:
        BDBG_WRN(("Unsupported DAC sampling rate (%d).  Defaulting to 48k.", sampleRate));
        /* Fall through */
    case 192000:
    case 48000:
        numerator = 101;
        denominator = 512;
        sampleInc = 2;
        phaseInc = 0x000e9045;
        break;
    case 96000:
        numerator = 101;
        denominator = 256;
        sampleInc = 4;
        phaseInc = 0x00074823;
        break;
    }

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_RM, handle->index, _RATE_RATIO );
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_RM,_RATE_RATIO, DENOMINATOR);
    regVal |= DAC_FIELD_DATA(HIFIDAC_RM,_RATE_RATIO, DENOMINATOR, denominator);
    BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_RM, handle->index, _SAMPLE_INC );
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~(DAC_MASK(HIFIDAC_RM,_SAMPLE_INC, SAMPLE_INC) | DAC_MASK(HIFIDAC_RM,_SAMPLE_INC, NUMERATOR));
    regVal |= DAC_FIELD_DATA(HIFIDAC_RM,_SAMPLE_INC, NUMERATOR, numerator);
    regVal |= DAC_FIELD_DATA(HIFIDAC_RM,_SAMPLE_INC, SAMPLE_INC, sampleInc);
    BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_RM, handle->index, _PHASE_INC );
    regVal = BREG_Read32_isr(handle->deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_RM,_PHASE_INC, PHASE_INC);
    regVal |= DAC_FIELD_DATA(HIFIDAC_RM,_PHASE_INC, PHASE_INC, phaseInc);
    BREG_Write32_isr(handle->deviceHandle->regHandle, regAddr, regVal);
}

/**************************************************************************/

static void BAPE_Dac_P_SyncVolume(BAPE_DacHandle handle)
{
    uint32_t regAddr;
    int i;

    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _STATUS );
    for ( i = 0; i < 1000; i++ )
    {
        uint32_t status = BREG_Read32(handle->deviceHandle->regHandle, regAddr);
        if ( DAC_GET_FIELD_DATA(status, HIFIDAC_CTRL,_STATUS, VOLUME_AT_TARGET) )
        {
            break;
        }
        BKNI_Sleep(1);
    }
}

/**************************************************************************/

static void BAPE_Dac_P_PowerDown(BAPE_Handle bapeHandle, unsigned dacIndex)
{
    uint32_t regVal, regAddr, regMask;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);
    BDBG_ASSERT(dacIndex < BAPE_CHIP_MAX_DACS);

/* While the DAC is powered off, keep its SYNC_INIT_DAC (or SYNC_RESET_DAC) bit set.
     * And don't clear it until after the DAC is powered back up.
     */
    #ifdef BCHP_HIFIDAC_CTRL0_RESET   /* 35230 */
        regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, dacIndex, _RESET );
        regVal = BREG_Read32(bapeHandle->regHandle, regAddr);
        regVal |= BCHP_MASK(HIFIDAC_CTRL0_RESET, SYNC_RESET_DAC );      /* Set the reset bit*/
        BREG_Write32(bapeHandle->regHandle, regAddr, regVal);
    #elif defined BCHP_HIFIDAC_CTRL0_INIT || defined BCHP_HIFIDAC_CTRL_0_INIT
        regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, dacIndex, _INIT );
        regVal = BREG_Read32(bapeHandle->regHandle, regAddr);
        regVal |= DAC_MASK(HIFIDAC_CTRL,_INIT, SYNC_INIT_DAC );      /* Set the reset bit*/
        BREG_Write32(bapeHandle->regHandle, regAddr, regVal);
    #endif

#ifdef BCHP_AIO_MISC_PWRDOWN
    /* Figure out which powerdown bits to set. */
    switch ( dacIndex )
    {
    #if BAPE_CHIP_MAX_DACS > 0
       case 0:
            regMask = BCHP_MASK(AIO_MISC_PWRDOWN, DAC0_POWERDN_REF) | 
                      BCHP_MASK(AIO_MISC_PWRDOWN, DAC0_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 1
        case 1:
            regMask = BCHP_MASK(AIO_MISC_PWRDOWN, DAC1_POWERDN_REF) | 
                      BCHP_MASK(AIO_MISC_PWRDOWN, DAC1_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 2
        case 2:
            regMask = BCHP_MASK(AIO_MISC_PWRDOWN, DAC2_POWERDN_REF) | 
                      BCHP_MASK(AIO_MISC_PWRDOWN, DAC2_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 3
        case 3:
            regMask = BCHP_MASK(AIO_MISC_PWRDOWN, DAC3_POWERDN_REF) | 
                      BCHP_MASK(AIO_MISC_PWRDOWN, DAC3_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 4
            #error "Need to add support for more DACS"
    #endif
    /* Should never get here */
    default:
        BDBG_ERR(("DAC index (%u) doesn't refer to a valid DAC", dacIndex));
        BDBG_ASSERT(false);     /* something went wrong somewhere! */
        return;
    }

    /* Now just set those bits in the DAC's powerdown register */
    regAddr = BCHP_AIO_MISC_PWRDOWN;
    regVal = BREG_Read32(bapeHandle->regHandle, regAddr);
    regVal |= regMask;
    BREG_Write32(bapeHandle->regHandle, regAddr, regVal);
#else
    /* Figure out which powerdown bits to set. */
    switch ( dacIndex )
    {
    #if BAPE_CHIP_MAX_DACS > 0 && defined(BCHP_AUD_MISC_PWRDOWN_DAC0_POWERDN_REF_MASK)
       case 0:
            regMask = BCHP_MASK(AUD_MISC_PWRDOWN, DAC0_POWERDN_REF) | 
                      BCHP_MASK(AUD_MISC_PWRDOWN, DAC0_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 1 && defined(BCHP_AUD_MISC_PWRDOWN_DAC0_POWERDN_REF_MASK)
        case 1:
            regMask = BCHP_MASK(AUD_MISC_PWRDOWN, DAC1_POWERDN_REF) | 
                      BCHP_MASK(AUD_MISC_PWRDOWN, DAC1_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 2 && defined(BCHP_AUD_MISC_PWRDOWN_DAC0_POWERDN_REF_MASK)
        case 2:
            regMask = BCHP_MASK(AUD_MISC_PWRDOWN, DAC2_POWERDN_REF) | 
                      BCHP_MASK(AUD_MISC_PWRDOWN, DAC2_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 3 && defined(BCHP_AUD_MISC_PWRDOWN_DAC0_POWERDN_REF_MASK)
        case 3:
            regMask = BCHP_MASK(AUD_MISC_PWRDOWN, DAC3_POWERDN_REF) | 
                      BCHP_MASK(AUD_MISC_PWRDOWN, DAC3_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 4
            #error "Need to add support for more DACS"
    #endif
    /* Should never get here */
    default:
        BDBG_ERR(("DAC index (%u) doesn't refer to a valid DAC", dacIndex));
        BDBG_ASSERT(false);     /* something went wrong somewhere! */
        return;
    }

    /* Now just set those bits in the DAC's powerdown register */
    regAddr = BCHP_AUD_MISC_PWRDOWN;
    regVal = BREG_Read32(bapeHandle->regHandle, regAddr);
    regVal |= regMask;
    BREG_Write32(bapeHandle->regHandle, regAddr, regVal);
#endif

    return;
}

/**************************************************************************/

static void BAPE_Dac_P_PowerUp(BAPE_Handle bapeHandle, unsigned dacIndex)
{
    uint32_t regVal, regAddr, regMask;

    BDBG_OBJECT_ASSERT(bapeHandle, BAPE_Device);
    BDBG_ASSERT(dacIndex < BAPE_CHIP_MAX_DACS);

    /* Clear bit 9 from the CURRDAC_CTRL.CTRL field during DAC power up.
     * Otherwise, the DAC can take really long to power up... like up to
     * 40 seconds or more.  But only clear this bit for a millisecond, 
     * then set it again. 
     */ 
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, dacIndex, _CURRDAC_CTRL );
    regVal = BREG_Read32(bapeHandle->regHandle, regAddr);
    regVal &= ~(DAC_FIELD_DATA(HIFIDAC_CTRL,_CURRDAC_CTRL, CTRL, 0x200));
    BREG_Write32(bapeHandle->regHandle, regAddr, regVal);

#ifdef BCHP_AIO_MISC_PWRDOWN
    /* Figure out which powerdown bits to clear. */
    switch ( dacIndex )
    {
    #if BAPE_CHIP_MAX_DACS > 0
       case 0:
            regMask = BCHP_MASK(AIO_MISC_PWRDOWN, DAC0_POWERDN_REF) | 
                      BCHP_MASK(AIO_MISC_PWRDOWN, DAC0_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 1
        case 1:
            regMask = BCHP_MASK(AIO_MISC_PWRDOWN, DAC1_POWERDN_REF) | 
                      BCHP_MASK(AIO_MISC_PWRDOWN, DAC1_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 2
        case 2:
            regMask = BCHP_MASK(AIO_MISC_PWRDOWN, DAC2_POWERDN_REF) | 
                      BCHP_MASK(AIO_MISC_PWRDOWN, DAC2_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 3
        case 3:
            regMask = BCHP_MASK(AIO_MISC_PWRDOWN, DAC3_POWERDN_REF) | 
                      BCHP_MASK(AIO_MISC_PWRDOWN, DAC3_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 4
            #error "Need to add support for more DACS"
    #endif
    /* Should never get here */
    default:
        BDBG_ERR(("DAC index (%u) doesn't refer to a valid DAC", dacIndex));
        BDBG_ASSERT(false);     /* something went wrong somewhere! */
        return;
    }

    /* Now just clear those bits in the DAC's powerdown register */
    regAddr = BCHP_AIO_MISC_PWRDOWN;
    regVal = BREG_Read32(bapeHandle->regHandle, regAddr);
    regVal &= ~regMask;
    BREG_Write32(bapeHandle->regHandle, regAddr, regVal);
#else
    /* Figure out which powerdown bits to clear. */
    switch ( dacIndex )
    {
    #if BAPE_CHIP_MAX_DACS > 0 && defined(BCHP_AUD_MISC_PWRDOWN_DAC0_POWERDN_REF_MASK)
       case 0:
            regMask = BCHP_MASK(AUD_MISC_PWRDOWN, DAC0_POWERDN_REF) | 
                      BCHP_MASK(AUD_MISC_PWRDOWN, DAC0_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 1 && defined(BCHP_AUD_MISC_PWRDOWN_DAC0_POWERDN_REF_MASK)
        case 1:
            regMask = BCHP_MASK(AUD_MISC_PWRDOWN, DAC1_POWERDN_REF) | 
                      BCHP_MASK(AUD_MISC_PWRDOWN, DAC1_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 2 && defined(BCHP_AUD_MISC_PWRDOWN_DAC0_POWERDN_REF_MASK)
        case 2:
            regMask = BCHP_MASK(AUD_MISC_PWRDOWN, DAC2_POWERDN_REF) | 
                      BCHP_MASK(AUD_MISC_PWRDOWN, DAC2_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 3 && defined(BCHP_AUD_MISC_PWRDOWN_DAC0_POWERDN_REF_MASK)
        case 3:
            regMask = BCHP_MASK(AUD_MISC_PWRDOWN, DAC3_POWERDN_REF) | 
                      BCHP_MASK(AUD_MISC_PWRDOWN, DAC3_POWERDN_LR);
            break;
    #endif
    #if BAPE_CHIP_MAX_DACS > 4
            #error "Need to add support for more DACS"
    #endif
    /* Should never get here */
    default:
        BDBG_ERR(("DAC index (%u) doesn't refer to a valid DAC", dacIndex));
        BDBG_ASSERT(false);     /* something went wrong somewhere! */
        return;
    }

    /* Now just clear those bits in the DAC's powerdown register */
    regAddr = BCHP_AUD_MISC_PWRDOWN;
    regVal = BREG_Read32(bapeHandle->regHandle, regAddr);
    regVal &= ~regMask;
    BREG_Write32(bapeHandle->regHandle, regAddr, regVal);
#endif

    /* Now that the DAC is powered up, we can take it out of reset by
     * clearing the SYNC_INIT_DAC (or SYNC_RESET_DAC) bit.
     */
    #ifdef BCHP_HIFIDAC_CTRL0_RESET   /* 35230 */
        regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, dacIndex, _RESET );
        regVal = BREG_Read32(bapeHandle->regHandle, regAddr);
        regVal &= ~DAC_MASK(HIFIDAC_CTRL,_RESET, SYNC_RESET_DAC );      /* Clear the reset bit*/
        BREG_Write32(bapeHandle->regHandle, regAddr, regVal);
    #elif defined BCHP_HIFIDAC_CTRL0_INIT || defined BCHP_HIFIDAC_CTRL_0_INIT
        regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, dacIndex, _INIT );
        regVal = BREG_Read32(bapeHandle->regHandle, regAddr);
        regVal &= ~DAC_MASK(HIFIDAC_CTRL,_INIT, SYNC_INIT_DAC );      /* Clear the reset bit*/
        BREG_Write32(bapeHandle->regHandle, regAddr, regVal);
    #endif

    /* Finally, set bit 9 of the CURRDAC_CTRL.CTRL back to its normal state. */
    BKNI_Sleep(1);    /* Min Gyu said that bit 9 should be held clear for at least a millisecond. */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, dacIndex, _CURRDAC_CTRL );
    regVal = BREG_Read32(bapeHandle->regHandle, regAddr);
    regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_CURRDAC_CTRL, CTRL, 0x200);
    BREG_Write32(bapeHandle->regHandle, regAddr, regVal);

    return;
}

/**************************************************************************/

static BERR_Code BAPE_Dac_P_OpenHw(BAPE_DacHandle handle)
{
    BERR_Code       errCode = BERR_SUCCESS;
    BAPE_Handle     deviceHandle;
    unsigned        i;
    uint32_t        regAddr, regVal;

    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);

    deviceHandle = handle->deviceHandle;
    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    #ifdef BCHP_HIFIDAC_CTRL0_RESET
        /* Initialize hardware before applying settings */
        regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _RESET );
        BREG_Write32(deviceHandle->regHandle, regAddr, 1);
        (void)BREG_Read32(deviceHandle->regHandle, regAddr);    /* Sync the register write */
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
    #elif defined BCHP_HIFIDAC_CTRL0_INIT || defined BCHP_HIFIDAC_CTRL_0_INIT
        regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _INIT );
        BREG_Write32(deviceHandle->regHandle, regAddr, 0);
    #endif

    /* Init test RAM... map the testtone register array to the Samples buffer. */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TEST );
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_TEST, RAM_BUF_SEL);
    regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, RAM_BUF_SEL, Samples);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    /* Enable the test tone, otherwise we can't write to the testtone register array. */
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_TEST, TESTTONE_ENABLE);
    regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, TESTTONE_ENABLE, Enable);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    /* Set samples buffer to zero */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_BASE );
    for (   i =  GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_START) ; 
            i <= GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_END)   ;
            i++ )
    {
        BREG_Write32(deviceHandle->regHandle, regAddr + (4 * i), 0);
    }

    /* Now map the testtone register array to the Pingpong buffer. */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TEST );
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_TEST, RAM_BUF_SEL);
    regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, RAM_BUF_SEL, Pingpong);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    /* Set pingpong buffer to zero */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_BASE );
    for (   i =  GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_START) ; 
            i <= GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_END)   ;
            i++ )
    {
        BREG_Write32(deviceHandle->regHandle, regAddr + (4 * i), 0);
    }

    /* We're done with the testtone registers, we can turn off testone_enable. */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TEST );
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_TEST, TESTTONE_ENABLE);
    regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, TESTTONE_ENABLE, Normal_operation);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    /* Mute RFmod by default.  It will be unmuted (if necessary) by BAPE_RfMod_SetSettings(). */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _CONFIG );
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_CONFIG, RFMOD_MUTE);
    regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, RFMOD_MUTE, Mute);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    /* Program MAPPER_SOFTMUTE to normal */
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_CONFIG, MAPPER_SOFTMUTE);
    regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, MAPPER_SOFTMUTE, Normal_operation);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    /* Write full volume by default */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _DAC_VOLUME );
    regVal = DAC_FIELD_DATA(HIFIDAC_CTRL,_DAC_VOLUME, DAC_VOL, 0x1ffff);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    /* Write set the DAC's RANGE.ASRCOUT to the appropriate default */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _RANGE );
    regVal = BREG_Read32(deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_RANGE, ASRCOUT);
    regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_RANGE, ASRCOUT, BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_RANGE_ASRCOUT);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _PEAK_A1 );
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr );
    regVal &= ~(DAC_MASK(HIFIDAC_CTRL,_PEAK_A1, PEAK_A1));
    regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_PEAK_A1, PEAK_A1, BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_PEAK_A1_PEAK_A1);
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _PEAK_A2 );
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr );
    regVal &= ~(DAC_MASK(HIFIDAC_CTRL,_PEAK_A2, PEAK_A2));
    regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_PEAK_A2, PEAK_A2, BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_PEAK_A2_PEAK_A2);
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _DAC_VOLUMECFG );
    regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr );
    regVal &= ~(DAC_MASK(HIFIDAC_CTRL,_DAC_VOLUMECFG, STEPSIZE));
    regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_DAC_VOLUMECFG, STEPSIZE, BAPE_DAC_DEFAULT_HIFIDAC_CTRL0_DAC_VOLUMECFG_STEPSIZE);
    BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);

    /* mute */
    BAPE_Dac_P_SetMute(&handle->outputPort, true, false);

    /* Power up the DAC */
    BAPE_Dac_P_PowerUp(handle->deviceHandle, handle->index);

    /* unmute the DACALL Control port */
    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _MUTECTRL );
    regVal = BREG_Read32 (deviceHandle->regHandle, regAddr);
    regVal &= ~DAC_MASK(HIFIDAC_CTRL,_MUTECTRL, MUTEALL);
    regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_MUTECTRL, MUTEALL, Ramp_unmute);
    BREG_Write32(deviceHandle->regHandle, regAddr, regVal);

    return errCode;
}

/**************************************************************************/

static bool BAPE_Dac_P_ToneSettingsChanged(const BAPE_DacSettings *pSettings1, const BAPE_DacSettings *pSettings2)
{
    unsigned i;

    BDBG_ASSERT(NULL != pSettings1);
    BDBG_ASSERT(NULL != pSettings2);

    if ( pSettings1->testTone.enabled != pSettings2->testTone.enabled )
    {
        return true;
    }
    if ( pSettings1->testTone.zeroOnLeft != pSettings2->testTone.zeroOnLeft )
    {
        return true;
    }
    if ( pSettings1->testTone.zeroOnRight != pSettings2->testTone.zeroOnRight )
    {
        return true;
    }
    if ( pSettings1->testTone.sharedSamples != pSettings2->testTone.sharedSamples )
    {
        return true;
    }
    if ( pSettings1->testTone.numSamplesLeft != pSettings2->testTone.numSamplesLeft )
    {
        return true;
    }
    if ( pSettings1->testTone.numSamplesRight != pSettings2->testTone.numSamplesRight )
    {
        return true;
    }
    if ( pSettings1->testTone.sampleRate != pSettings2->testTone.sampleRate )
    {
        return true;
    }
    if ( pSettings1->testTone.sampleRate != pSettings2->testTone.sampleRate )
    {
        return true;
    }
    for ( i = 0; i < sizeof(pSettings1->testTone.samples)/sizeof(int32_t); i++ )
    {
        if ( pSettings1->testTone.samples[i] != pSettings2->testTone.samples[i] )
        {
            return true;
        }
    }
    return false;
}

/**************************************************************************/

static BERR_Code BAPE_Dac_P_ApplySettings(
    BAPE_DacHandle handle,
    const BAPE_DacSettings *pSettings,
    bool force
    )
{
    uint32_t regAddr, regVal, origRegVal;
    unsigned i;
    BDBG_OBJECT_ASSERT(handle, BAPE_Dac);
    BDBG_ASSERT(NULL != pSettings);
    
    /* Sanity checks before changing any settings */
    if ( pSettings->testTone.enabled && 
         (pSettings->testTone.numSamplesLeft < 1 || pSettings->testTone.numSamplesLeft > 64 ||
          pSettings->testTone.numSamplesRight < 1 || pSettings->testTone.numSamplesRight > 64) )
    {
        BDBG_ERR(("Test tone samples must be between 1 and 64 for both left and right."));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Mute before changing this setting */

    regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _CONFIG );
    origRegVal = regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr );
    regVal &= ~(DAC_MASK(HIFIDAC_CTRL,_CONFIG, MAPPER_MUTETYPE) |
                DAC_MASK(HIFIDAC_CTRL,_CONFIG, I2S_SELECT_RIGHT) |
                DAC_MASK(HIFIDAC_CTRL,_CONFIG, I2S_SELECT_LEFT));
    switch ( pSettings->muteType )
    {
    case BAPE_DacMuteType_eDrive0:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, MAPPER_MUTETYPE, Drive_0);
        break;
    case BAPE_DacMuteType_eDriveNegative1:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, MAPPER_MUTETYPE, Drive_neg1);
        break;
    case BAPE_DacMuteType_eCustom:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, MAPPER_MUTETYPE, Use_reg_val);
        {
            uint32_t  myRegAddr, myRegVal;

            myRegAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _DAC_CONST_VAL );
            myRegVal = BREG_Read32(handle->deviceHandle->regHandle, myRegAddr);
            myRegVal &= ~(DAC_MASK(HIFIDAC_CTRL,_DAC_CONST_VAL, LEFT)|DAC_MASK(HIFIDAC_CTRL,_DAC_CONST_VAL, RIGHT));
            myRegVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_DAC_CONST_VAL, LEFT, pSettings->customLeftValue);
            myRegVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_DAC_CONST_VAL, RIGHT, pSettings->customRightValue);
            BREG_Write32(handle->deviceHandle->regHandle, myRegAddr, myRegVal);
        }
        break;
    default:
        BDBG_ERR(("Invalid DAC mute type %d", pSettings->muteType));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    switch ( pSettings->stereoMode )
    {
    case BAPE_StereoMode_eLeftRight:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, I2S_SELECT_LEFT, Left) | 
                  DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, I2S_SELECT_RIGHT, Right);
        break;
    case BAPE_StereoMode_eLeftLeft:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, I2S_SELECT_LEFT, Left) | 
                  DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, I2S_SELECT_RIGHT, Left);
        break;
    case BAPE_StereoMode_eRightLeft:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, I2S_SELECT_LEFT, Right) | 
                  DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, I2S_SELECT_RIGHT, Left);
        break;
    case BAPE_StereoMode_eRightRight:
        regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, I2S_SELECT_LEFT, Right) | 
                  DAC_FIELD_ENUM(HIFIDAC_CTRL,_CONFIG, I2S_SELECT_RIGHT, Right);
        break;
    }

    if ( (regVal != origRegVal) || force )
    {
        uint32_t  myRegAddr, myRegVal;

        /* If the register is going to change, we need to mute first to avoid a pop */
        myRegAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _DAC_VOLUME );
        myRegVal  = BREG_Read32(handle->deviceHandle->regHandle, myRegAddr);
        BREG_Write32(handle->deviceHandle->regHandle, myRegAddr, 0);
        BAPE_Dac_P_SyncVolume(handle);

        /* write the new register and unmute */
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
        BREG_Write32(handle->deviceHandle->regHandle, myRegAddr, myRegVal);
    }

    /* Update PEAK_GAIN if it has changed */
    if ( (handle->settings.peakGain != pSettings->peakGain && !handle->rfMode) || force )
    {
        regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _PEAK_GAIN );
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr );
        regVal &= ~(DAC_MASK(HIFIDAC_CTRL,_PEAK_GAIN, PEAK_GAIN));
        regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_PEAK_GAIN, PEAK_GAIN, pSettings->peakGain);
        BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
    }

    /* Only do this if test tone settings have changed */
    if ( BAPE_Dac_P_ToneSettingsChanged(&handle->settings, pSettings) || force )
    {
        /* Setup the test tone if enabled */
        regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TEST );
        regVal = BREG_Read32(handle->deviceHandle->regHandle, regAddr );
        if ( pSettings->testTone.enabled )
        {
            regVal &= ~(DAC_MASK(HIFIDAC_CTRL,_TEST, RAM_BUF_SEL)|
                        DAC_MASK(HIFIDAC_CTRL,_TEST, TESTTONE_LEFT)|
                        DAC_MASK(HIFIDAC_CTRL,_TEST, TESTTONE_RIGHT)|
                        DAC_MASK(HIFIDAC_CTRL,_TEST, TESTTONE_SHARE)|
                        DAC_MASK(HIFIDAC_CTRL,_TEST, TESTTONE_REPEAT_LEFT)|
                        DAC_MASK(HIFIDAC_CTRL,_TEST, TESTTONE_REPEAT_RIGHT)|
                        DAC_MASK(HIFIDAC_CTRL,_TEST, TESTTONE_ENABLE));
    
            /* Disable the test tone first if it's enabled */
            BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
    
            regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, RAM_BUF_SEL, Samples);
            regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, TESTTONE_ENABLE, Enable);
    
            if ( pSettings->testTone.zeroOnLeft )
                regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, TESTTONE_LEFT, Output_zero);
            else
                regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, TESTTONE_LEFT, Output_testtone);
    
            if ( pSettings->testTone.zeroOnRight )
                regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, TESTTONE_RIGHT, Output_zero);
            else
                regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, TESTTONE_RIGHT, Output_testtone);
    
            if ( pSettings->testTone.sharedSamples )
                regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, TESTTONE_SHARE, Share);
            else
                regVal |= DAC_FIELD_ENUM(HIFIDAC_CTRL,_TEST, TESTTONE_SHARE, Dont_share);
    
            regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_TEST, TESTTONE_REPEAT_LEFT, pSettings->testTone.numSamplesLeft-1);
            regVal |= DAC_FIELD_DATA(HIFIDAC_CTRL,_TEST, TESTTONE_REPEAT_RIGHT, pSettings->testTone.numSamplesRight-1);
    
            /* Enable the tone */
            BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
    
            /* Load samples afterward */
            regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_BASE );
            for (   i =  GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_START) ; 
                    i <= GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_END)   ;
                    i++ )
            {
                BREG_Write32(handle->deviceHandle->regHandle, regAddr + (i*4),
                             pSettings->testTone.samples[i] & DAC_MASK(HIFIDAC_CTRL,_TESTTONE_SAMPLE_i, TESTTONE_SAMPLE));
            }

            /* Un-Mute */
            BAPE_Dac_P_SetMute(&handle->outputPort, false, false);
        }
        else
        {
            if ( DAC_GET_FIELD_DATA(regVal, HIFIDAC_CTRL,_TEST, TESTTONE_ENABLE) )
            {
                if ( !handle->enabled )
                {
                    /* Mute */
                    BAPE_Dac_P_SetMute(&handle->outputPort, true, true);
                }

                /* Clear sample buffer */
                regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_BASE );
                for (   i =  GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_START) ; 
                        i <= GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TESTTONE_SAMPLE_i_ARRAY_END)   ;
                        i++ )
                {
                    BREG_Write32(handle->deviceHandle->regHandle, regAddr + (i*4), 0);
                }

                regAddr = GET_DAC_REG_ADDR3(BCHP_HIFIDAC_CTRL, handle->index, _TEST );
                regVal &= ~DAC_MASK(HIFIDAC_CTRL,_TEST, TESTTONE_ENABLE);
                BREG_Write32(handle->deviceHandle->regHandle, regAddr, regVal);
                /* Set the rate manager back to the desired rate */
                BKNI_EnterCriticalSection();
                BAPE_Dac_P_SetRateManager_isr(handle, handle->sampleRate);
                BKNI_LeaveCriticalSection();
            }
        }
    }

    handle->settings = *pSettings;

    return BERR_SUCCESS;
}

/**************************************************************************/


