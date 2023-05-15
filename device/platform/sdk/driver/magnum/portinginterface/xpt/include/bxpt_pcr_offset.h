/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_pcr_offset.h $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 6/18/12 10:53a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/include/bxpt_pcr_offset.h $
 * 
 * Hydra_Software_Devel/10   6/18/12 10:53a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 10:59a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * Hydra_Software_Devel/9   5/22/12 3:17p gmullen
 * SW7401-2177: Added bounds checks for BXPT_PcrOffset_ChangeStcRate()
 * 
 * Hydra_Software_Devel/8   3/30/12 11:06a gmullen
 * SW7435-81: Bug in TRIG_MODE selection
 * 
 * Hydra_Software_Devel/7   8/3/11 5:54p gmullen
 * SW7231-325: Merged new API to Hydra
 * 
 * Hydra_Software_Devel/SW7231-325/1   8/3/11 4:16p gmullen
 * SW7231-325: Added BXPT_PcrOffset_Acquire()
 * 
 * Hydra_Software_Devel/6   6/8/11 2:16p gmullen
 * SW7425-653: Fixed prototype
 * 
 * Hydra_Software_Devel/5   6/7/11 2:27p gmullen
 * SW7425-653: Return STC INC HI and LO registers
 * 
 * Hydra_Software_Devel/4   6/3/11 4:34p gmullen
 * SW7425-653: Merged changes to Hydra branch
 * 
 * Hydra_Software_Devel/SW7425-653/4   6/3/11 4:05p gmullen
 * SW7425-653: Fixed typos in NRT example code
 * 
 * Hydra_Software_Devel/SW7425-653/3   6/3/11 3:16p gmullen
 * SW7425-653: Updated sample code with change to
 * BXPT_PcrOffset_GetSoftIncrementRegOffset()
 * 
 * Hydra_Software_Devel/SW7425-653/2   6/2/11 10:35a gmullen
 * SW7425-653: Added non-realtime transcoding support
 * 
 * Hydra_Software_Devel/SW7425-653/1   5/28/11 12:53p gmullen
 * SW7425-653: Revised API proposal.
 * 
 * Hydra_Software_Devel/3   3/22/11 1:42p gmullen
 * SW7425-223: Added code to correctly map STC broadcasts
 * 
 * Hydra_Software_Devel/2   12/21/10 9:24a gmullen
 * SW7425-15: Added STC broadcast mode
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/1   10/8/10 2:57p gmullen
 * SW7425-15: Added header files
 * 
 * Hydra_Software_Devel/31   9/28/10 11:20a gmullen
 * SW35230-1354: Default error acquire threshold to 2
 * 
 * Hydra_Software_Devel/30   9/14/10 10:07a gmullen
 * SW35230-1354: Added ONE_ERR_ACQ or TWO_ERR_ACQ support to the PI
 * 
 * Hydra_Software_Devel/29   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/28   6/8/09 4:40p gmullen
 * PR55611: Added support for selecting which DPCR block drives the STC to
 * _GetSettings() and _SetSettings()
 * 
 * Hydra_Software_Devel/27   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/26   1/28/09 9:53a gmullen
 * PR51198: Fixed bug in IntId mapping
 * 
 * Hydra_Software_Devel/25   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/24   1/20/09 6:01p gmullen
 * PR50689: Added GetIntId API.
 * 
 * Hydra_Software_Devel/23   11/26/08 4:15p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/22   9/22/08 2:53p gmullen
 * PR47074: Do not disable PID channel if its used by the PCR Offset
 * block.
 * 
 * Hydra_Software_Devel/21   7/9/08 9:18a gmullen
 * PR44665: Added BXPT_PcrOffset_CaptureStcOnce() and
 * BXPT_PcrOffset_CaptureStcOnce_isr()
 * 
 * Hydra_Software_Devel/20   5/15/08 9:25a gmullen
 * PR42537: Added BXPT_PcrOffset_ReloadPcrPidChannel_isr()
 * 
 * Hydra_Software_Devel/19   3/26/08 11:24a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/18   2/27/08 3:29p piyushg
 * PR39387: Added a new API BXPT_PcrOffset_ForceInvalid to
 * clear the PCR_OFFSET_VALID bit.
 * 
 * Hydra_Software_Devel/17   2/19/08 2:31p gmullen
 * PR38918: Added BXPT_PcrOffset_IsOffsetValid() and
 * BXPT_PcrOffset_IsOffsetValid_isr()
 * 
 * Hydra_Software_Devel/16   12/3/07 10:54a katrep
 * PR36900: Added support for 7335.
 * 
 * Hydra_Software_Devel/15   11/28/07 11:26a gmullen
 * PR36900: Added 7335 support
 * 
 * Hydra_Software_Devel/14   10/30/07 3:11p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/13   6/5/07 8:53p gmullen
 * PR31834: PCR jitter correction can now be disabled.
 * 
 * Hydra_Software_Devel/12   4/17/07 5:53p gmullen
 * PR26217: Added StcSelect to BXPT_PcrOffset_Settings struct.
 * 
 * Hydra_Software_Devel/11   3/1/07 11:31a gmullen
 * PR26217: Changed new API names to conform to sw arch spec.
 * 
 * Hydra_Software_Devel/10   2/28/07 1:33p gmullen
 * PR26217: Enable offset generation for PCR PID.
 * 
 * Hydra_Software_Devel/9   2/14/07 1:10p gmullen
 * PR27642: Added support for 7405.
 * 
 * Hydra_Software_Devel/8   2/8/07 4:33p gmullen
 * PR26217: Changed API names for consistency
 * 
 * Hydra_Software_Devel/7   2/6/07 7:05p gmullen
 * PR26217: Added B0 support.
 * 
 * Hydra_Software_Devel/6   11/21/06 3:02p gmullen
 * PR26073: Added BXPT_PcrOffset_RegenOffset_isr()
 * 
 * Hydra_Software_Devel/5   8/18/06 10:13a gmullen
 * PR23345: Added MaxPcrError to Get/SetSettings calls.
 * 
 * Hydra_Software_Devel/4   8/8/06 10:58a gmullen
 * PR23345: Added MaxPcrError to BXPT_PcrOffset_Defaults.
 * 
 * Hydra_Software_Devel/3   3/31/06 11:30a gmullen
 * PR20565: Added UseHostPcrs to settings struct.
 * 
 * Hydra_Software_Devel/2   2/10/06 12:06p katrep
 * PR19583: added isr flavours of various pcr offset routines, similar to
 * 7401, pcrlib should now compile okay
 * 
 * Hydra_Software_Devel/9   11/14/05 9:11a gmullen
 * PR15309: OffsetThreshold may now be set through Defaults or Get/Set
 * methods.
 * 
 * Hydra_Software_Devel/8   10/28/05 4:39p gmullen
 * PR17859: Added BXPT_PcrOffset_GetOffset().
 * 
 * Hydra_Software_Devel/7   10/28/05 11:59a gmullen
 * PR15309: Added BXPT_PCROFFSET_USE_HOST_PCRS define.
 * 
 * Hydra_Software_Devel/6   9/26/05 3:51p gmullen
 * PR15309: Added BXPT_PcrOffset_SetStc() .
 * 
 * Hydra_Software_Devel/5   9/26/05 1:37p gmullen
 * PR15309: Moved CountMode from Defaults struct to Settings struct, to
 * reduce overhead during channel change,
 * 
 * Hydra_Software_Devel/4   9/21/05 2:17p gmullen
 * PR15309: Added support for AAC HE and AC3+ audio, fixed bug in playback
 * PI, modified RAVE PI to support channel change and reset.
 * 
 * Hydra_Software_Devel/3   8/31/05 10:58a gmullen
 * PR15309: Fixed bugs in PSI extraction and MPEG audio support.
 * 
 * Hydra_Software_Devel/2   8/4/05 8:14a gmullen
 * PR15309: Added PCR support.
 * 
 * Hydra_Software_Devel/1   5/13/05 1:27p gmullen
 * PR15309: First draft of XPT PI.
 * 
 ***************************************************************************/

#ifndef BXPT_PCR_OFFSET_H__
#define BXPT_PCR_OFFSET_H__

#include "bxpt.h"

#ifdef __cplusplus
extern "C"{
#endif

/*=************************ Module Overview ********************************
<verbatim>
Overview:
</verbatim>
***************************************************************************/

/***************************************************************************
Summary:
This define is being deprecated. Use BXPT_PcrOffset_Settings.UseHostPcrs 
instead.
****************************************************************************/
#define BXPT_PCROFFSET_USE_HOST_PCRS        ( 0xFF )

/***************************************************************************
Summary:
Module private defines. User code should not use these defines directly.
****************************************************************************/
#define BXPT_P_SPLICING_QUEUE_SIZE              8

/***************************************************************************
Summary:
Defines the number of usable slots in the PCR splicing stack. 
****************************************************************************/
#define MAX_PCR_SPLICING_ENTRIES    ( BXPT_P_SPLICING_QUEUE_SIZE - 1 )

/***************************************************************************
Summary:
The opaque handle for a PCR offset module. 

Description:
This structure represents the PCR handle. It is created at 
BXPT_PcrOffset_Open 
***************************************************************************/
typedef struct BXPT_P_PcrOffset_Impl *BXPT_PcrOffset_Handle;

/***************************************************************************
Summary:
Enumeration of all the data sources a parser band can use. 
****************************************************************************/
typedef enum BXPT_PcrOffset_StcCountMode
{
    BXPT_PcrOffset_StcCountMode_eMod300 = 0, 
    BXPT_PcrOffset_StcCountMode_eBinary = 1
}
BXPT_PcrOffset_StcCountMode;

#if BXPT_HAS_TSMUX
/***************************************************************************
Summary:
Enumeration of all the data sources a parser band can use. 
****************************************************************************/
typedef enum BXPT_PcrOffset_BroadcastMode
{
    BXPT_PcrOffset_BroadcastMode_eLegacy = 0, 
    BXPT_PcrOffset_BroadcastMode_eLsb32 = 1, 
    BXPT_PcrOffset_BroadcastMode_eMsb32 = 2,
    BXPT_PcrOffset_BroadcastMode_eFull42 = 3
}
BXPT_PcrOffset_BroadcastMode;
#endif

/***************************************************************************
Summary:
Default settings for a PCR Offset channel.

Description:
The settings read from, or will be written to, a PCR offset channel.
***************************************************************************/
typedef struct BXPT_PcrOffset_Defaults
{
    /* 
    ** If UsePcrTimebase is false, the STC count is driven from a free-running
    ** 27 MHz source. Otherwise, WhichPcr selects the timebase that clocks the
    ** STC.
    */
    bool UsePcrTimeBase;
    unsigned WhichPcr;

    /* 
    ** If the new PCR Offset differs from the current PCR Offset by less
    ** than OffsetThreshold, no Offset will be broadcast (non will show up
    ** in the ITB data). Setting OffsetThreshold to 0 will force a broadcast
    ** of each new PCR Offset as it is computed. 
    */
    unsigned OffsetThreshold;

    /* 
    ** The maximum absolute PCR error. If the base portion of the PCR just 
    ** received is greater than the current STC minus the PCR Offset, a
    ** PCR out-of-range error is generated. 
    */
    unsigned MaxPcrError;

#if BXPT_HAS_MOSAIC_SUPPORT
    /* 
    ** Which STC will drive the given PCR Offset channel.
    ** The default mapping mimics the behavior of the previous PCR Offset hardware,  
    ** for the first BXPT_P_MAX_STCS offset channels. Beyond that, the caller is
    ** expected to override this with whatever value is needed for mosaic support.
    */
    unsigned WhichStc;

    /* 
    ** The hardware can update the PCR offset after a number of consecutive PCRs are greater than the 
    ** OffsetThreshold (above) are seen. That number is programmable, below.
    */
    unsigned ConsecutiveErrorThreshold;
#endif
} 
BXPT_PcrOffset_Defaults;

/***************************************************************************
Summary:
Settings for a PCR Offset channel.

Description:
The settings read from, or will be written to, a PCR offset channel.
***************************************************************************/
typedef struct BXPT_PcrOffset_Settings
{
    /* 
    ** If UsePcrTimebase is false, the STC count is driven from a free-running
    ** 27 MHz source. Otherwise, WhichPcr selects the timebase that clocks the
    ** STC.
    */
    bool UsePcrTimeBase;
    unsigned WhichPcr;
	
    /* 
    ** Counting mode used by the STC counter. 
    ** For MPEG streams, CountMode should be BXPT_PcrOffset_StcCountMode_eMod300.
    ** For DirecTV streams, CountMode should be BXPT_PcrOffset_StcCountMode_eBinary
    */
    BXPT_PcrOffset_StcCountMode CountMode;

    uint32_t PidChannelNum;         /* PID channel carrying PCRs */

    /* 
    ** If the new PCR Offset differs from the current PCR Offset by less
    ** than OffsetThreshold, no Offset will be broadcast (non will show up
    ** in the ITB data). Setting OffsetThreshold to 0 will force a broadcast
    ** of each new PCR Offset as it is computed. 
    */
    unsigned OffsetThreshold;

    /*
    ** When the host processer wants to load its own PCRs instead of using those
    ** in the PID stream, UseHostPcrs should be set to true. This prevents PCRs 
    ** in the stream from being used. 
    */
    bool UseHostPcrs;

    /* 
    ** The maximum absolute PCR error. If the base portion of the PCR just                                                                     
    ** received is greater than the current STC minus the PCR Offset, a
    ** PCR out-of-range error is generated. 
    */
    unsigned MaxPcrError;

#if BXPT_HAS_MOSAIC_SUPPORT		  
    /* Selects which STC is used for PCR offset generation. */
    unsigned StcSelect;

    /* 
    ** The hardware can update the PCR offset after a number of consecutive PCRs are greater than the 
    ** OffsetThreshold (above) are seen. That number is programmable, below.
    */
    unsigned ConsecutiveErrorThreshold;
#endif

    /* 
    ** Disable timestamp correction for PCR packet jitter. For normal usage modes
    ** this setting should be false (ie, enable timestamp correction). IP SetTop
    ** user might need to set this to true.
    */
    bool TimestampDisable;

#if BXPT_HAS_TSMUX
    BXPT_PcrOffset_BroadcastMode BroadcastMode;
#endif
}
BXPT_PcrOffset_Settings;

#if BXPT_HAS_TSMUX

/***************************************************************************
Summary: 
Status bits for the TsMux channel. 
****************************************************************************/
typedef enum BXPT_PcrOffset_StcTriggerMode
{      
    BXPT_PcrOffset_StcTriggerMode_eTimebase,            /* Increment STC by timebase */
    BXPT_PcrOffset_StcTriggerMode_eExternalTrig,        /* Increment by external trigger source. */
    BXPT_PcrOffset_StcTriggerMode_eSoftIncrement,       /* Increment when STC_INC_TRIG register is written */
    BXPT_PcrOffset_StcTriggerMode_eMax
}
BXPT_PcrOffset_StcTriggerMode;

/***************************************************************************
Summary: 
PCR Offset configuration for Non-Realtime Transcoding.  
****************************************************************************/
typedef struct BXPT_PcrOffset_NRTConfig
{
    unsigned PairedStc;                      /* Which STC is paired with this PcrOffset's for AV Window comparisons */
    bool EnableAvWindowComparison;              /* Enable AV Window comparisons with AvCompareStc */
    unsigned AvWindow;                          /* Window, expressed in mSec. The STC increment is stalled when the 
                                                  comparison exceedes this window. */
    BXPT_PcrOffset_StcTriggerMode TriggerMode;      /* Event that will increment the STC */
    unsigned ExternalTriggerNum;                /* Identifies which external trigger is used when TriggerMode == _eExternalTrig */

    /* 
    ** Amount to increment the STC by when external or soft trigger is used.
    ** This should be set to one video or audio frame-time, depending on which 
    ** decoder is being driven. The value is expressed in 42-bit MPEG or  
    ** 32-bit binary STC format, as dictated by the transport format being used.
    */ 
    uint64_t StcIncrement;
}
BXPT_PcrOffset_NRTConfig;

/***************************************************************************
Summary: Example code for configurating an audio and a video decoder PCR 
Offset channels in Non-Realtime Transcoding. 
 
    // Pcr Offset 0 will be used for video, 1 for audio.
 
    BXPT_PcrOffset_Defaults ChannelDefaults;
    BXPT_PcrOffset_Handle hVideoPcrOffset, hAudioPcrOffset;
    BXPT_PcrOffset_Settings VideoPcrOffsetSettings, AudioPcrOffsetSettings;
    BXPT_PcrOffset_NRTConfig AudioNrtConfig, VideoNrtConfig;
    unsigned SoftIncRegisterOffset;
  
    BXPT_PcrOffset_GetChannelDefSettings( hXpt, 0, &ChannelDefaults );
    BXPT_PcrOffset_Open( hXpt, 0, &ChannelDefaults, &hVideoPcrOffset );
    BXPT_PcrOffset_GetChannelDefSettings( hXpt, 1, &ChannelDefaults );
    BXPT_PcrOffset_Open( hXpt, 1, &ChannelDefaults, &hAudioPcrOffset );
 
    BXPT_PcrOffset_GetNRTConfig( hVideoPcrOffset, &VideoNrtConfig );
    BXPT_PcrOffset_GetNRTConfig( hAudioPcrOffset, &AudioNrtConfig );
    BXPT_PcrOffset_GetSettings( hVideoPcrOffset, &VideoPcrOffsetSettings );
    BXPT_PcrOffset_GetSettings( hAudioPcrOffset, &AudioPcrOffsetSettings );
 
    VideoNrtConfig.EnableAvWindowComparison = AudioNrtConfig.EnableAvWindowComparison = true;
    VideoNrtConfig.AvWindow = AudioNrtConfig.AvWindow = 100; // 100mS
 
    VideoNrtConfig.TriggerMode = BXPT_PcrOffset_StcTriggerMode_eExternalTrig;
    VideoNrtConfig.ExternalTriggerNum = 0;  // This may change in actual usage.
    VideoNrtConfig.StcIncrement = 0x00;     // Placeholders. This should be set to one
                                            // video frame-time, expressed in MPEG or binary
                                            // STC format as appropriate.
 
    // For audio, the soft increment register is used, rather than the external
    // trigger. So, ExternalTriggerNum is a "don't care". 
    AudioNrtConfig.TriggerMode = BXPT_PcrOffset_StcTriggerMode_eSoftIncrement;
    AudioNrtConfig.StcIncrement = 0x00;     // Placeholders. This should be set to one
                                            // audio frame-time, expressed in MPEG or binary
                                            // STC format as appropriate.
 
    // This sets up the linking of the audio and video decoder STCs 
    VideoNrtConfig.PairedStc = AudioPcrOffsetSettings.StcSelect;
    AudioNrtConfig.PairedStc = VideoPcrOffsetSettings.StcSelect;
  
    BXPT_PcrOffset_SetNRTConfig( hVideoPcrOffset, &VideoNrtConfig );
    BXPT_PcrOffset_SetNRTConfig( hAudioPcrOffset, &AudioNrtConfig );
 
    // The register offsets in RegMap should be passed over to the audio DSP. The
    // struct is defined in magnum/commonutils/avc/$(CHIP)/bavc_xpt.h  . 
    BXPT_PcrOffset_GetSoftIncrementRegisterOffsets( hPcrOffset, &RegMap );
****************************************************************************/

/***************************************************************************
Summary: 
Get the PCR Offset configuration for Non-Realtime Transcoding.  
****************************************************************************/
void BXPT_PcrOffset_GetNRTConfig(
    BXPT_PcrOffset_Handle hPcrOffset,   /* [in] The channel handle */
    BXPT_PcrOffset_NRTConfig *Config
    );

/***************************************************************************
Summary: 
Set the PCR Offset configuration for Non-Realtime Transcoding.  
****************************************************************************/
BERR_Code BXPT_PcrOffset_SetNRTConfig(
    BXPT_PcrOffset_Handle hPcrOffset,   /* [in] The channel handle */
    const BXPT_PcrOffset_NRTConfig *Config
    );

/***************************************************************************
Summary: 
Get address of the soft increment registers. 
****************************************************************************/
void BXPT_PcrOffset_GetSoftIncrementRegisterOffsets(
    BXPT_PcrOffset_Handle hPcrOffset,   /* [in] The channel handle */
    BAVC_Xpt_StcSoftIncRegisters *RegMap
    );

#endif

/***************************************************************************
Summary:
Return the total number of PCR Offset channels available.

Description:
The number of PCR Offset instantiations can vary from one chip to another.
This call will return the number available on the current chip. 

Returns:
    BERR_SUCCESS                - Returned the total channel count.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_PcrOffset_GetTotalChannels(
    BXPT_Handle hXpt,               /* [in] The transport handle */
    unsigned int *TotalChannels     /* [out] The number of Pcr offset channels. */
    );

/***************************************************************************
Summary:
Return defaults for a PCR Offset channel.

Description:
This call returns the default settings for the given PCR Offset channel. The
settings returned can be changed and used during channel open.

Returns:
    BERR_SUCCESS                - Returned the defaults for the channel.
    BERR_INVALID_PARAMETER      - Bad input parameter

See also:
BXPT_PcrOffset_Open
****************************************************************************/
BERR_Code BXPT_PcrOffset_GetChannelDefSettings(
    BXPT_Handle hXpt,                       /* [in] The transport handle - need chip info */
    unsigned int ChannelNo,                 /* [in] Which pcr offset module */
    BXPT_PcrOffset_Defaults *Defaults   /* [out] The default settings of a pcr module */
    );

/***************************************************************************
Summary:
Open a PCR Offset channel.

Description:
The PCR Offset channel give by WhichOne is initialized, using the setting 
provided in the Defaults structure. A handle for the opened channel is 
returned.

Returns:
    BERR_SUCCESS                - Channel opened successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PcrOffset_Open(
    BXPT_Handle hXpt,                  /* [in] The transport handle*/
    unsigned int ChannelNo,            /* [in] Which pcr offset module */
    BXPT_PcrOffset_Defaults *Defaults, /* [in] The default setting */
    BXPT_PcrOffset_Handle *hChannel    /* [out] The channel handle */
    );

/***************************************************************************
Summary:
Close a PCR Offset channel.

Description:
The given channel is disabled. The associated splicing stack is also cleared.

Returns:
    BERR_SUCCESS                - Channel closed succesfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PcrOffset_Close(
    BXPT_PcrOffset_Handle hChannel   /* [in] The channel handle */
    );

/***************************************************************************
Summary:
Get the current PID settings.

Description:
The PID channel number and parser band being used for PCR offset calculation
are returned.

Returns:
    BERR_SUCCESS                - Settings read succesfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PcrOffset_GetSettings(
    BXPT_PcrOffset_Handle hPcrOff,
    BXPT_PcrOffset_Settings *Settings
    );

/***************************************************************************
Summary: 
Set the current PID settings.

Description:
Load mew values for the PID channel number into hardware, for
use by the PCR offset calculation. Note that the PID channel must be 
configured and enabled before calling this function.

Returns:
    BERR_SUCCESS                - Settings written succesfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PcrOffset_SetSettings(
    BXPT_PcrOffset_Handle hPcrOff,
    const BXPT_PcrOffset_Settings *Settings
    );

/***************************************************************************
Summary:
Change the STC clock rate.

Description:
The STC can be run at a faster or slower rate during playback, to support
STC-based trick modes. This call will change the STC rate according to the
following formula:

new_rate = increment_rate * ( Increment / ( Prescale + 1 ) )
where increment_rate is 45kHz for MPEG, or 27 MHz for other standards.

Returns:
    BERR_SUCCESS                - STC rate changed.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/

/* Max values for Increment and Prescale */
#define BXPT_MAX_STC_INCREMENT	(255)
#define BXPT_MAX_STC_PRESCALE	(255)

BERR_Code BXPT_PcrOffset_ChangeStcRate(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    unsigned Increment,                 /* [in] STC increment value */
    unsigned Prescale                   /* [in] STC prescale value */
    );

/***************************************************************************
Summary:
Freeze the STC clock.

Description:
The STC value is frozen at it's current value. That value will be continually
broadcast. 

Returns:
    BERR_SUCCESS                - STC frozen (or unfrozen as requested).
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PcrOffset_FreezeStc(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    bool Freeze                         /* [in] STC is frozen if true, unfrozen if false */
    );

/***************************************************************************
Summary:
Set the current STC.

Description:
Load a user-defined STC. This value will be the new STC broadcast by the
PCR Offset block. Note that this value must be added to the PCR Offset
value (obtained by calling BXPT_PcrOffset_GetOffset()) to obtain the full
STC value used by the decoders for timestamp management.

For MPEG streams, the PCR Offset is the most significant 32 bits of the 33 
bit base value. For DirecTV, it is the full 32 bit RTS.

Returns:
    BERR_SUCCESS                - New STC loaded succesfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PcrOffset_SetStc(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    uint32_t NewStc
    );
BERR_Code BXPT_PcrOffset_SetStc_isr(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    uint32_t NewStc
    );

/***************************************************************************
Summary:
Retrieve the STC clock value.

Description:
The upper 32 bits of the current STC are returned. The STC used here is NOT 
the actual PCR value recovered from the stream. The STC is initialized to 0 in
hardware, and is incremented by the DPCR block. When PCRs are seen in the 
transport stream, the value of that PCR is subtracted from the STC and 
becomes the PCR Offset that is given to the decoders. As such, the STC is 
usable for timestamp management, even though it's value (probably) doesn't
match the actuall PCRs in the stream.

Note that this value is added to the PCR Offset (which can be obtained by calling 
BXPT_PcrOffset_GetOffset()) to obtain the full STC value used by the decoders
for timestamp management.

For MPEG streams, the PCR Offset is the most significant 32 bits of the 33 
bit base value. For DirecTV, it is the full 32 bit RTS.

Returns:
    uint32_t - The 32 most significant bits of the STC.
***************************************************************************/
uint32_t BXPT_PcrOffset_GetStc(
    BXPT_PcrOffset_Handle hChannel      /* [in] The channel handle */
    );

uint32_t BXPT_PcrOffset_GetStc_isr(
    BXPT_PcrOffset_Handle hChannel      /* [in] The channel handle */
    );

/***************************************************************************
Summary:
Load a new PCR Offset.

Description:
A user-supplied PCR Offset is loaded into hardware. Note that this is added 
to the current STC (which can be obtained by calling BXPT_PcrOffset_GetStc()) 
to yield the value used by the decoders for timestamp management.

For MPEG streams, the PCR Offset is the most significant 32 bits of the 33 
bit base value. For DirecTV, it is the full 32 bit RTS.

Returns:
    BERR_SUCCESS                - PVR offset loaded.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PcrOffset_SetOffset(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    uint32_t UserPcrOffset                  /* [in] User-supplied PCR Offset */
    );

BERR_Code BXPT_PcrOffset_SetOffset_isr(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    uint32_t UserPcrOffset                  /* [in] User-supplied PCR Offset */
    );

/***************************************************************************
Summary:
Get the current PCR Offset. 

Description:
Return the current PCR Offset. This must be added to the value returned by
BXPT_PcrOffset_GetStc() to yield the complete STC that the audio and video
decoders will use for timestamp management. 

For MPEG streams, the PCR Offset is the most significant 32 bits of the 33 
bit base value. For DirecTV, it is the full 32 bit RTS.

Returns:
    uint32_t - The current PCR Offset
***************************************************************************/
uint32_t BXPT_PcrOffset_GetOffset(
    BXPT_PcrOffset_Handle hChannel          /* [in] The channel handle */
    );

uint32_t BXPT_PcrOffset_GetOffset_isr(
    BXPT_PcrOffset_Handle hChannel          /* [in] The channel handle */
    );

/***************************************************************************
Summary:
Put the STC counter into free running mode.

Description:
When free running is enabled, an internal 27 MHz clock will drive the STC
counter, rather than the timebase selected during the channel open. When 
free running is disabled, the timebase given during the open will be 
restored.

Returns:
    BERR_SUCCESS                - Mode changed
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PcrOffset_FreeRun(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    bool FreeRun                        /* [in] Free run if true. */
    );

/***************************************************************************
Summary:
Push a PID channel onto the splicing stack.

Description:
The given PID channel is pushed onto the PCR PID splicing stack. 

Returns:
    BERR_SUCCESS                - PID pushed onto the stack.
    BERR_INVALID_PARAMETER      - Bad input parameter
    BXPT_ERR_QUEUE_FULL         - No room left in the stack.
***************************************************************************/
BERR_Code BXPT_PcrOffset_PushPidChannel(
    BXPT_PcrOffset_Handle hChannel,     /* [in] The channel handle */
    unsigned int PidChannel             /* [in] Channel carrying the PID to splice. */
    );

/***************************************************************************
Summary:
Clear the splicing stack.

Description:
The splicing stack is emptied. All PIDs in the stack are removed. 

Returns:
    BERR_SUCCESS                - Queue cleared.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PcrOffset_ClearQueue(
    BXPT_PcrOffset_Handle hChannel      /* [in] The channel handle */
    );

/***************************************************************************
Summary:
Get splicing countdown.

Description:
Return the value of the splicing countdown seen in the last PCR-bearing 
packet. 0 will be returned if no PCR-bearing packet has been seen.

Returns:
    unsigned - The splicing countdown
***************************************************************************/
unsigned BXPT_PcrOffset_GetCountdown(
    BXPT_PcrOffset_Handle hChannel      /* [in] The channel handle */
    );

/***************************************************************************
Summary:
Get splicing stack depth.

Description:
Return depth of the splicing stack, i.e. the number of PIDs already pushed
but not yet spliced.

Returns:
    unsigned - The splicing countdown
***************************************************************************/
unsigned BXPT_PcrOffset_GetQueueDepth(
    BXPT_PcrOffset_Handle hChannel      /* [in] The channel handle */
    );

/***************************************************************************
Summary:
Regenerate the PCR Offset.

Description:
Regenerate a PCR Offset to the RAVE when the next PCR is seen in the current
stream. This function should be called only from within an ISR context.	

This API is being replaced by BXPT_PcrOffset_ReloadPcrPidChannel_isr(). 

Returns:
    void
***************************************************************************/
void BXPT_PcrOffset_RegenOffset_isr(
    BXPT_PcrOffset_Handle hPcrOff
    );

/***************************************************************************
Summary:
Reload the PCR PID channel.

Description:
Another way to regenerate the PCR Offset is to reload/rewrite the PCR PID
channel. This function should be called only from within an ISR context.

Returns:
    void
***************************************************************************/
void BXPT_PcrOffset_ReloadPcrPidChannel_isr(
	BXPT_PcrOffset_Handle hPcrOff
	);

/***************************************************************************
Summary:
Determine if the current offset is valid.

Description:
In some cases, it's handy to know if the current Offset is valid. Return a
boolean indicating the valid status.

Returns:
    void
***************************************************************************/
bool BXPT_PcrOffset_IsOffsetValid(
    BXPT_PcrOffset_Handle hPcrOff
    );

/***************************************************************************
Summary:
Determine if the current offset is valid, ISR version.

Description:
In some cases, it's handy to know if the current Offset is valid. Return a
boolean indicating the valid status. This API can be called from within an
ISR context.

Returns:
    void
***************************************************************************/
bool BXPT_PcrOffset_IsOffsetValid_isr(
    BXPT_PcrOffset_Handle hPcrOff
    );


/***************************************************************************
Summary:
Force reset the PCR_OFFSET_VALID bit.

Description:
 Application should call this API before starting the Playback to clear 
 PCR_OFFSET_VALID.

Returns:
    void
***************************************************************************/

void BXPT_PcrOffset_ForceInvalid( 
	BXPT_PcrOffset_Handle hPcrOff 
	);

/***************************************************************************
Summary:
Force reset the PCR_OFFSET_VALID bit. ISR version.

Description:
 Application should call this API before starting the Playback to clear 
 PCR_OFFSET_VALID.

Returns:
    void
***************************************************************************/

void BXPT_PcrOffset_ForceInvalid_isr( 
	BXPT_PcrOffset_Handle hPcrOff 
	);

#if BXPT_HAS_MOSAIC_SUPPORT
/***************************************************************************
Summary:
Generate PCR offsets for an additional PID channel.

Description:
Mosaic mode requires that PCR offsets be generated for PID channels other 
than the one specified in BXPT_PcrOffset_Settings.PidChannelNum . This call
is used to enable offset generation for those other channels. The offsets are
based on the PCR Offset / STC channel given by the BXPT_PcrOffset_Handle .

Returns:
    BERR_SUCCESS                - Offset generation is enabled.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_PcrOffset_EnableOffset(
    BXPT_PcrOffset_Handle hPcrOff,      /* [in] Handle for the PCR Offset channel to use */
    uint32_t PidChannelNum,             /* [in] Which PID channel to generate offsets for */
    bool FixedOffsetEn,                 /* [in] Add a fixed offset during jitter adjustment */
    bool JitterAdjustEn                 /* [in] Enable jitter adjustment */
    );

/***************************************************************************
Summary:
Disable PCR offset generation for an additional channel.

Description:
Mosaic mode requires that PCR offsets be generated for PID channels other 
than the one specified in BXPT_PcrOffset_Settings.PidChannelNum . This call
disables generation of PCR offsets for one of these other PID channels. 

Returns:
    void
***************************************************************************/
void BXPT_PcrOffset_DisableOffset(
    BXPT_PcrOffset_Handle hPcrOff,      /* [in] Handle for the PCR Offset channel to use */
    uint32_t PidChannelNum              /* [in] Which PID channel to disable offsets for */
    );
#endif

/***************************************************************************
Summary:
Arm single capture of the STC from the stream.

Description:
Enable capturing the STC from the next PCR in the stream. The capture is done
only once; to repeat the capture, call this API again after each capture.

Returns:
    void
***************************************************************************/
void BXPT_PcrOffset_CaptureStcOnce(
    BXPT_PcrOffset_Handle hPcrOff,      /* [in] Handle for the PCR Offset channel to use */
	bool Enable							/* [in] true to capture once, false to disable a pending capture */
    );

/***************************************************************************
Summary:
Arm single capture of the STC from the stream, ISR version.

Description:
Use this call instead of BXPT_PcrOffset_CaptureStcOnce() when operating 
from within an ISR context. 

Returns:
    void
***************************************************************************/
void BXPT_PcrOffset_CaptureStcOnce_isr(
    BXPT_PcrOffset_Handle hPcrOff,      /* [in] Handle for the PCR Offset channel to use */
	bool Enable							/* [in] true to capture once, false to disable a pending capture */
    );

typedef enum BXPT_PcrOffsetIntName
{
    BXPT_PcrOffsetIntName_ePcrNew = 0,               
    BXPT_PcrOffsetIntName_ePcrDiscont = 1,
    BXPT_PcrOffsetIntName_ePcrOneErr = 2,
    BXPT_PcrOffsetIntName_ePcrTwoErr = 3,
    BXPT_PcrOffsetIntName_ePcrSpliceDone = 4,
    BXPT_PcrOffsetIntName_ePcrSpliceError = 5,
    BXPT_PcrOffsetIntName_eStcCaptured = 6
}
BXPT_PcrOffsetIntName;

/***************************************************************************
Summary: 
Return the INT ID for a given interrupt

Description:
Given the PCR Offset channel and a generic interrupt name, return the INT ID
needed to create a callback for the channel's specific interrupt. 

Returns:
    BINT_id     A BINT compatable interrupt ID
***************************************************************************/
BERR_Code BXPT_PcrOffset_GetIntId(
    BXPT_PcrOffset_Handle hChannel,
    BXPT_PcrOffsetIntName Name,
    BINT_Id *IntId
    );

/***************************************************************************
Summary: 
Put the PCR Offset channel back into acquire mode. 
 
Description:
Should be called as part of channel change processing. 
 
Returns: 
        void 
***************************************************************************/
void BXPT_PcrOffset_Acquire(
    BXPT_PcrOffset_Handle hChannel
    );

/*
** These functions are called internally. 
** Users should NOT uses these functions directly.
*/

bool BXPT_P_PcrOffset_IsPidChannelInUse(
    BXPT_Handle hXpt,               /* [in] The transport handle */
    uint32_t PidChannelNum              /* [in] Which PID channel to disable offsets for */
    );

void BXPT_P_PcrOffset_ModuleInit( 
    BXPT_Handle hXpt 
    );

#ifdef __cplusplus
}
#endif

#endif      /* BXPT_PCR_OFFSET_H__ */
