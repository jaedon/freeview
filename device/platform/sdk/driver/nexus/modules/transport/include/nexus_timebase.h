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
* $brcm_Workfile: nexus_timebase.h $
* $brcm_Revision: 32 $
* $brcm_Date: 11/15/11 8:00p $
*
* API Description:
*   Management of timebase (clock rate) blocks.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/transport/7400/include/nexus_timebase.h $
* 
* 32   11/15/11 8:00p bandrews
* SW7405-5538: back up explosion to keep 2-level nested structs, instead
*  of all the way to 1-level
* 
* 31   11/14/11 2:28p erickson
* SW7425-1448: add NEXUS_Timebase_SetVdecFrameRate and
*  NEXUS_Timebase_SetHdDviFrameRate
* 
* 30   11/7/11 3:42p bandrews
* SW7405-5538: merge to main
* 
* SW7405-5538/1   11/3/11 6:22p bandrews
* SW7405-5538: converted
* 
* 29   10/31/11 7:05p bandrews
* SW7231-391: merge to main
* 
* SW7420-2078/1   10/11/11 8:26p bandrews
* SW7231-391: add support for parser band and timebase protected client
*  usage
* 
* 28   9/14/11 2:00p erickson
* SW7408-284: add
*  NEXUS_TimebaseSettings.sourceSettings.pcr.jitterCorrection
* 
* 27   3/21/11 9:32a erickson
* SWDTV-5829: add NEXUS_TimebaseSettings.sourceSettings.hdDvi.format
*
* 26   3/10/11 12:40p erickson
* SWDTV-5829: add
*  NEXUS_TimebaseSettings.sourceSettings.hdDvi.vertSyncClock
*
* 25   7/14/10 5:37p erickson
* SW3556-1145: add NEXUS_TimebaseTrackRange to all source type settings
*
* 24   5/3/10 11:15a erickson
* SW7335-706: add NEXUS_TimebaseStatus.lastValueLo
*
* PR52472/1   4/1/09 8:44p bandrews
* PR52472: Use latched phase error instead of reading STC.  This allows
*  us to have accurate results regardless of background processing eating
*  cpu
*
* 21   12/19/08 10:16a jgarrett
* PR 49587: Moving internal members to freeRun instead
*
* 20   12/2/08 3:03p jgarrett
* PR 49587: Merging to mainline
*
* Nexus_Devel/wince_7400/9   12/1/08 10:40a gmullen
* PR 49587: Merged with mainline to get Pcr Error callbacks.
*
* Nexus_Devel/wince_7400/8   12/1/08 9:40a gmullen
* PR 49587: Merged with mainline version 17 to get updated
*  NEXUS_TimebaseStatus struct.
*
* Nexus_Devel/wince_7400/7   9/17/08 4:31p ptimariu
* PR47063: merge from nexus 3.2 mainline
*
* Nexus_Devel/wince_7400/6   4/24/08 11:32a rrlee
* PR42137:  Update to latest Nexus code
*
* 19   11/24/08 11:45a erickson
* PR48846: move pcrErrorCallback outside substruct for kernel mode proxy
*  parsing
*
* 18   11/24/08 8:10a erickson
* PR48846: added pcrError callback and pcrErrors status counter
*
* 17   11/19/08 9:47a erickson
* PR49212: merge new status functions
*
* PR49212/1   11/18/08 7:45p bandrews
* PR49212: ASTM overrides user settings, but it should not destroy them
*  when it does so
*
* 16   11/17/08 11:46a erickson
* PR35457: fix units in comment, better example
*
* 15   11/10/08 7:18p erickson
* PR 35457: fix missing ;
*
* 14   11/10/08 5:45p erickson
* PR 35457: update comments
*
* 13   10/24/08 4:41p erickson
* PR47232: add trackRange for freeRun
*
* 12   10/7/08 10:40p erickson
* PR47232: move centerFrequency to freeRun.centerFrequency'
*
* 11   10/6/08 2:35a erickson
* PR47232: added
*  NEXUS_TimebaseSettings.sourceSettings.pcr.centerFrequency
*
* 10   9/2/08 11:19a erickson
* PR46213: added NEXUS_TimebaseSettings.freeze
*
* 9   7/17/08 10:45a erickson
* PR44492: add NEXUS_TimebaseSettings.sourceSettings.i2s.sampleRate
*
* 8   7/14/08 12:04p erickson
* PR42739: added options for IPTV
*
* 7   7/3/08 10:40a erickson
* PR44492: add i2s and spdif timebase selection
*
* 6   3/21/08 11:40a erickson
* PR40716: merge
*
* PR40716/1   3/21/08 3:58p dyzhang
* PR40716: timebase settings for analog inputs are not correct. rename
*  NEXUS_TimebaseSourceType_eVdec to NEXUS_TimebaseSourceType_eAnalog.
*
* 5   3/20/08 10:22a erickson
* PR40716: configure DPCR for analog video sources using frameRate, just
*  like HdDvi.
*
* 4   3/11/08 4:23p erickson
* PR40363: clarified comment on pcrCallback
*
* Nexus_Devel/wince_7400/5   3/5/08 5:00p gmullen
* Added DPCR loops params, again!
*
* Nexus_Devel/wince_7400/4   3/5/08 4:08p gmullen
* Previous check-in lost members in NEXUS_TimebaseSettings.
*
* Nexus_Devel/wince_7400/3   3/5/08 2:54p gmullen
* Added access to the DPCR loop params
*
* Nexus_Devel/wince_7400/2   2/7/08 1:43p gmullen
* Merged with main.
*
* Nexus_Devel/15   1/14/08 5:47p jgarrett
* PR 38708: Adding PCR callback to application
*
* Nexus_Devel/14   11/29/07 2:24p erickson
* PR35457: doc update for 0.5 release
*
***************************************************************************/
#ifndef NEXUS_TIMEBASE_H__
#define NEXUS_TIMEBASE_H__

#include "nexus_types.h"
#include "nexus_video_types.h"
#include "nexus_pid_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=*
A NEXUS_Timebase performs clock recovery using a DPCR hardware block.
It provides interfaces to connect and sync with various inputs.

See nexus/examples/decode.c for an example application.

See NEXUS_StcChannelHandle to see how NEXUS_Timebase is used to provide lipsync.
See NEXUS_DisplayHandle and NEXUS_AudioMixerHandle to see how NEXUS_Timebase is used to regulate output timing.
**/

/**
Summary:
The type of source that NEXUS_Timebase is connected to.
**/
typedef enum NEXUS_TimebaseSourceType
{
    NEXUS_TimebaseSourceType_ePcr,       /* Lock to a PCR in a transport stream. Used for live decode. Can also be used for PVR with pacing. */
    NEXUS_TimebaseSourceType_eFreeRun,   /* Lock to the 27MHz system clock. Used for PVR. */
    NEXUS_TimebaseSourceType_eAnalog,    /* Lock to AnalogVideoDecoder. */
    NEXUS_TimebaseSourceType_eHdDviIn,   /* Lock to the HdmiInput. */
    NEXUS_TimebaseSourceType_eCcir656In, /* Lock to the CCIR 656 video input. */
    NEXUS_TimebaseSourceType_eI2sIn,     /* Lock to the I2S audio input. */
    NEXUS_TimebaseSourceType_eSpdifIn,   /* Lock to the SPDIF audio input. */
    NEXUS_TimebaseSourceType_eMax
} NEXUS_TimebaseSourceType;

/**
Summary:
Tracking range for DPCR block in units of parts per million
**/
typedef enum NEXUS_TimebaseTrackRange
{
    NEXUS_TimebaseTrackRange_e8ppm,
    NEXUS_TimebaseTrackRange_e15ppm,
    NEXUS_TimebaseTrackRange_e30ppm,
    NEXUS_TimebaseTrackRange_e61ppm,
    NEXUS_TimebaseTrackRange_e122ppm,
    NEXUS_TimebaseTrackRange_e244ppm,
    NEXUS_TimebaseTrackRange_eMax
} NEXUS_TimebaseTrackRange;

/**
Summary:
Settings for the free-run source type
**/
typedef struct NEXUS_TimebaseFreeRunSourceSettings
{
    unsigned centerFrequency; /* Controls the absolute center frequency of the timebase relative to the crystal.
                                 A 27MHz timebase is generated with 0x400000 (default). centerFrequency may be adjusted to compensate for crystal variations.
                                 Adjusting centerFrequency by 1 LSB has an effect of roughly 3.8 ppm. */
    NEXUS_TimebaseTrackRange trackRange;
    /*
    ** For the internal (free-running) source, the 27 MHz clock may be scaled by the prescale and inc values.
    ** The scaled clock signal is used to generate an imitation PCR, which the hardware uses as if it where
    ** the real thing.
    */
    uint32_t prescale;   /* A prescale of N divides the clock by N+1. For example, setting prescale
                        to zero will not affect the clock. Or, setting prescale to 57 will swallow
                        57 of every 58 clock ticks */
    uint32_t inc;        /* Together with prescale, this value converts clock ticks to PCRs for lock.
                        The clock ticks are divided by the prescaler. At every divided event,
                        the synthesized PCR is incremented by the amount in this inc register */
    uint8_t loopDirectPathGain;
    uint8_t loopGain;
    uint8_t loopIntegratorLeak;
} NEXUS_TimebaseFreeRunSourceSettings;

/**
Summary:
Settings for the PCR source type
**/
typedef struct NEXUS_TimebasePcrSourceSettings
{
    NEXUS_PidChannelHandle pidChannel; /* Pid channel for the PCR */
    unsigned maxPcrError;      /* Maximum amount of PCR jitter which DPCR hardware will absorb. Any PCR change outside of this
                                 threshold will result in the STC being updated. Measured in 33 bit PTS units (e.g. 90KHz MPEG TS).
                                 Default is 255 (which, for MPEG TS, is 255/90000 = 0.0028 = 2.8 milliseconds). */
    NEXUS_TimebaseTrackRange trackRange;
    NEXUS_TristateEnable jitterCorrection;
} NEXUS_TimebasePcrSourceSettings;

/**
Summary:
Settings for the VDEC source type
**/
typedef struct NEXUS_TimebaseVdecSourceSettings
{
    unsigned index; /* index of the AnalogVideoDecoder */
    NEXUS_VideoFormat format;
    NEXUS_VideoFrameRate frameRate; /* deprecated. use NEXUS_Timebase_SetVdecFrameRate instead. */
    NEXUS_TimebaseTrackRange trackRange;
} NEXUS_TimebaseVdecSourceSettings;

/**
Summary:
Settings for the HDDVI source type
**/
typedef struct NEXUS_TimebaseHddviSourceSettings
{
    unsigned index; /* index of the HD-DVI input */
    unsigned vertSyncClock; /* Vertical sync clock measured from HD-DVI. This is the number of clock cycles between vsync's.
                               Set to 0 to ignore/bypass this value. */
    NEXUS_VideoFormat format;
    NEXUS_VideoFrameRate frameRate; /* deprecated. use NEXUS_Timebase_SetHdDviFrameRate instead. */
    NEXUS_TimebaseTrackRange trackRange;
} NEXUS_TimebaseHddviSourceSettings;

/**
Summary:
Settings for the 656 source type
**/
typedef struct NEXUS_TimebaseCcir656SourceSettings
{
    unsigned index; /* index of the CCIR 656 input */
    NEXUS_VideoFormat format;
    NEXUS_TimebaseTrackRange trackRange;
} NEXUS_TimebaseCcir656SourceSettings;

/**
Summary:
Settings for the I2S source type
**/
typedef struct NEXUS_TimebaseI2sSourceSettings
{
    unsigned index; /* index of the I2S input */
    unsigned sampleRate; /* in Hz. This must be programmed by the application when it sets NEXUS_I2sSettings.sampleRate. */
    NEXUS_TimebaseTrackRange trackRange;
} NEXUS_TimebaseI2sSourceSettings;

/**
Summary:
Settings to configure a NEXUS_Timebase
**/
typedef struct NEXUS_TimebaseSettings
{
    NEXUS_TimebaseSourceType sourceType;    /* Type of timebase source */

    struct {
        NEXUS_TimebaseFreeRunSourceSettings freeRun; /* valid for sourceType NEXUS_TimebaseSourceType_eFreeRun */
        NEXUS_TimebasePcrSourceSettings pcr;     /* valid for sourceType NEXUS_TimebaseSourceType_ePcr */
        NEXUS_TimebaseVdecSourceSettings vdec;    /* valid for sourceType NEXUS_TimebaseSourceType_eAnalog */
        NEXUS_TimebaseHddviSourceSettings hdDvi;    /* valid for sourceType NEXUS_TimebaseSourceType_eHdDviIn */
        NEXUS_TimebaseCcir656SourceSettings ccir656; /* valid for sourceType NEXUS_TimebaseSource_eCcir656In */
        NEXUS_TimebaseI2sSourceSettings i2s; /* valid for sourceType NEXUS_TimebaseSourceType_eI2sIn */
    } sourceSettings; /* source-type-specific settings */

    /* NOTE: Callbacks are outside substruct because of kernel mode proxy parser */
    NEXUS_CallbackDesc pcrCallback;      /* Called when a new PCR is received. */
    NEXUS_CallbackDesc pcrErrorCallback; /* Called whenever there are two consecutive PCR out-of-range errors (as defined by maxPcrError.
                                            If the callback is set, it will be fired and NEXUS_TimebaseStatus.pcrErrors will accumulate. */
    bool freeze;                         /* Stop loop tracking by freezing the integrator. */
} NEXUS_TimebaseSettings;

/**
Summary:
Update timebase settings
**/
NEXUS_Error NEXUS_Timebase_SetSettings(
    NEXUS_Timebase timebase,
    const NEXUS_TimebaseSettings *pSettings   /* Settings */
    );

/**
Summary:
Get current timebase settings
**/
void NEXUS_Timebase_GetSettings(
    NEXUS_Timebase timebase,
    NEXUS_TimebaseSettings *pSettings    /* [out] Settings */
    );

/**
Summary:
Open a timebase for exclusive use

Description:
NEXUS_Timebase_Open is required for protected clients to exclusively use a timebase.
**/
NEXUS_Timebase NEXUS_Timebase_Open( /* attr{destructor=NEXUS_Timebase_Close}  */
    unsigned index /* Use a specific index or NEXUS_ANY_ID */
    );

/**
Summary:
Close a timebase opened with NEXUS_Timebase_Open
**/
void NEXUS_Timebase_Close(
    NEXUS_Timebase timebase
    );

/**
Summary:
Current status of the timebase.

Description:
This is chiefly used for diagnostics.
Call NEXUS_Timebase_GetStatus to get the status.
Call NEXUS_Timebase_ResetStatus to reset any acculumated values.
**/
typedef struct NEXUS_TimebaseStatus
{
    uint32_t lastValue;   /* Last PCR value received.
                             For DSS, this is the full 32 bits of PCR (27 MHz units).
                             For MPEG2 TS, this is the most significant 32 bits of the 42 bit PCR (that is, 45 KHz units).
                             Throughout the system, whenever the "PCR" is referred to, it is this 32 bit value. */
    uint32_t lastValueLo; /* Least significant 10 bits of the last 42 bit PCR for MPEG2 TS. Unused for DSS. */
    int32_t lastError;  /* error between last PCR and STC at time of receipt of last PCR */
    unsigned pcrCount;   /* number of PCR's received since call to NEXUS_Timebase_ResetStatus */
    unsigned pcrErrors;  /* number of PCR errors since call to NEXUS_Timebase_ResetStatus. This only accumulates if NEXUS_TimebaseSettings.sourceSettings.pcr.pcrError is set. */
    bool pcrValid;       /* valid PCR was captured from the stream since call to NEXUS_Timebase_ResetStatus */
    NEXUS_TimebaseSourceType sourceType; /* current source type this timebase is tracking */
} NEXUS_TimebaseStatus;

/**
Summary:
Get current NEXUS_TimebaseStatus
**/
NEXUS_Error NEXUS_Timebase_GetStatus(
    NEXUS_Timebase timebase,
    NEXUS_TimebaseStatus *pStatus        /* [out] current status */
    );

/**
Summary:
Reset any accumulated values in NEXUS_TimebaseStatus
**/
NEXUS_Error NEXUS_Timebase_ResetStatus(
    NEXUS_Timebase timebase
    );

/* 
Atomically set NEXUS_TimebaseSettings.sourceSettings.hdDvi
This function is automatically called by the HdmiInput Interface. 
A non-zero value will override anything set by NEXUS_TimebaseSettings.
*/
NEXUS_Error NEXUS_Timebase_SetHdDviFrameRate(
    NEXUS_Timebase timebase,
    NEXUS_VideoFrameRate frameRate
    );

/* 
Atomically set NEXUS_TimebaseSettings.sourceSettings.vdec
This function is automatically called by the AnalogVideoDecoder interface.
A non-zero value will override anything set by NEXUS_TimebaseSettings.
*/
NEXUS_Error NEXUS_Timebase_SetVdecFrameRate(
    NEXUS_Timebase timebase,
    NEXUS_VideoFrameRate frameRate
    );
    
#ifdef __cplusplus
}
#endif

#endif
