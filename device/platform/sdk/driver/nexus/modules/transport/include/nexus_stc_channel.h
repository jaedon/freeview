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
* $brcm_Workfile: nexus_stc_channel.h $
* $brcm_Revision: 20 $
* $brcm_Date: 12/7/11 9:47a $
*
* API Description:
*   Management of STC Channels that deliver timebases to decoders.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/transport/7400/include/nexus_stc_channel.h $
* 
* 20   12/7/11 9:47a erickson
* SW7550-772: add attr{null_allowed=y}
* 
* 19   11/15/11 7:58p bandrews
* SW7405-5538: back up explosion to keep 2-level nested structs, instead
*  of all the way to 1-level
* 
* 18   11/7/11 3:41p bandrews
* SW7405-5538: merge to main
* 
* SW7405-5538/1   11/3/11 6:22p bandrews
* SW7405-5538: converted
* 
* 17   6/7/11 3:46p vsilyaev
* SW7425-654: Added API to support non-realtime transcode
* 
* 16   3/17/11 4:42p erickson
* SW7346-120: add NEXUS_StcChannel_VerifyPidChannel
* 
* 15   12/30/10 11:35a vsilyaev
* SW7425-39: Added control over PCR bits
* 
* 14   3/26/10 1:00p vsilyaev
* SW7550-349: Use offsetThreshold to guarantee STC update
* 
* 13   12/28/09 2:50p erickson
* SW3556-957: add comment to NEXUS_StcChannel_SetRate regarding negative
*  rates
*
* 12   10/28/09 4:56p erickson
* SW7405-3315: update comments on NEXUS_StcChannelMode
*
* 11   5/18/09 4:24p erickson
* PR54880: allow StcChannels with different PCR_OFFSET contexts and STC's
*
* 10   4/14/09 9:49a erickson
* PR42679: add header file
*
* 9   11/19/08 9:47a erickson
* PR49212: merge new status functions
*
* PR49212/1   11/18/08 7:45p bandrews
* PR49212: ASTM overrides user settings, but it should not destroy them
*  when it does so
*
* 8   10/16/08 2:57p erickson
* PR47673: clarify stc trick mode behavior
*
* 7   10/6/08 11:26p erickson
* PR46648: merge
*
* PR46648/1   9/30/08 9:19p bandrews
* PR46648: Added support for specifying STC seed behavior in Auto mode
*
* 6   7/14/08 12:04p erickson
* PR42739: added options for IPTV
*
* 5   3/11/08 2:11p erickson
* PR40222: move ptsOffset into the decoders' settings structures
*
* 4   2/7/08 11:42p vsilyaev
* PR 38682: Added attributes for the linux kernel/user proxy mode
*
* 3   1/23/08 8:37p vobadm
* PR35457: update docs
*
* 2   1/23/08 5:16p erickson
* PR35457: update docs
*
* 1   1/18/08 2:16p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/13   11/29/07 2:24p erickson
* PR35457: doc update for 0.5 release
*
***************************************************************************/
#ifndef NEXUS_STCCHANNEL_H__
#define NEXUS_STCCHANNEL_H__

#include "nexus_types.h"
#include "nexus_pid_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for an STC Channel

Description:
An STC (system time clock) channel provides basic synchronization between audio and video decoders.
This synchronization is called TSM (time stamp management).
In TSM mode, decoders will only present pictures or audio frames when the data's PTS matches the current STC, within
a certain threshold.

See Nexus_Usage.doc for more usage information.
See NEXUS_SyncChannel for the high-precision lipsync Interface.

An StcChannel manages a PCR_OFFSET block and related software.
**/
typedef struct NEXUS_StcChannel *NEXUS_StcChannelHandle;

/*
Summary:
TSM modes used in NEXUS_StcChannelSettings
*/
typedef enum NEXUS_StcChannelMode
{
    NEXUS_StcChannelMode_ePcr,  /* Live TSM.
                                    STC values are derived from the stream's PCR using the DPCR block.
                                    The PCR_OFFSET block pipelines the offsets in the ITB to handle PCR discontinuities.
                                    CDB/ITB buffer levels are determined by relative PCR/PTS muxing. */
    NEXUS_StcChannelMode_eAuto, /* Playback TSM.
                                    Decoders report PTS values and the StcChannel sets the STC.
                                    The PCR_OFFSET block is not used and offsets are not pipelined.
                                    CDB/ITB buffer levels are determined by the first decoder to fill its buffer
                                    and by relative audio/video muxing. */
    NEXUS_StcChannelMode_eHost, /* Playback TSM where the application is responsible for setting the STC.
                                    This has all the same characteristics as eAuto, except the PTS interrupts
                                    from the decoder do not set the STC. */
    NEXUS_StcChannelMode_eMax
} NEXUS_StcChannelMode;

/*
Summary:
This describes the behavior for STC seeding during PVR when StcChannelMode is set to eAuto
*/
typedef enum NEXUS_StcChannelAutoModeBehavior
{
    NEXUS_StcChannelAutoModeBehavior_eFirstAvailable, /* The STC will be driven by either the video or audio PTS, depending on stream muxing and error conditions. */
    NEXUS_StcChannelAutoModeBehavior_eVideoMaster,    /* The video PTS will always drive the STC.  Audio errors will be ignored. */
    NEXUS_StcChannelAutoModeBehavior_eAudioMaster,    /* The audio PTS will always drive the STC.  Video errors will be ignored. */
    NEXUS_StcChannelAutoModeBehavior_eMax
} NEXUS_StcChannelAutoModeBehavior;

/*
Summary:
This controls number of PCR bits send to the downstream modules
*/
typedef enum NEXUS_StcChannel_PcrBits {
    NEXUS_StcChannel_PcrBits_eLegacy,
    NEXUS_StcChannel_PcrBits_eLsb32, 
    NEXUS_StcChannel_PcrBits_eMsb32,
    NEXUS_StcChannel_PcrBits_eFull42,
    NEXUS_StcChannel_PcrBits_eMax
} NEXUS_StcChannel_PcrBits;

/*
Summary:
Pcr-mode-specific stc channel settings
*/
typedef struct NEXUS_StcChannelPcrModeSettings
{
    NEXUS_PidChannelHandle pidChannel; /* The pid channel for the PCR. This determines the transport type as well. */
    unsigned offsetThreshold;          /* Threshold for filtering PCR change notifications to decoders, in PTS units. */
    unsigned maxPcrError;              /* Maximum absolute PCR error, in PTS units. */
    bool disableTimestampCorrection;   /* Auto timestamp correction for PCR packet jitter */
    bool disableJitterAdjustment;      /* PCR offset jitter adjustment */
} NEXUS_StcChannelPcrModeSettings;

/*
Summary:
Auto-mode-specific stc channel settings
*/
typedef struct NEXUS_StcChannelAutoModeSettings
{
    NEXUS_TransportType transportType;         /* There is no pcr for auto mode, but transport type is needed for correct STC management. */
    NEXUS_StcChannelAutoModeBehavior behavior; /* controls how the STC is seeded in Auto mode */
    unsigned offsetThreshold;          /* Threshold for filtering STC change notifications to decoders, in PTS units. */
} NEXUS_StcChannelAutoModeSettings;

/*
Summary:
Host-mode-specific stc channel settings
*/
typedef struct NEXUS_StcChannelHostModeSettings
{
    NEXUS_TransportType transportType; /* There is no pcr for host mode, but transport type is needed for correct STC management. */
    unsigned offsetThreshold;          /* Threshold for filtering STC change notifications to decoders, in PTS units. */
} NEXUS_StcChannelHostModeSettings;

/*
Summary:
Settings passed into NEXUS_StcChannel_Open
*/
typedef struct NEXUS_StcChannelSettings
{
    NEXUS_Timebase timebase;       /* The timebase that drives the rate of this StcChannel */
    NEXUS_StcChannelMode mode;     /* Mode of this channel, either PCR-based or host-loaded */
    bool autoConfigTimebase;       /* If true, the STC Channel will automatically program the timebase. This works for typical scenarios like live broadcast or PVR.
                                      You may want to manually program the timebase for modes like IP settop or Mosaic mode. */
    int stcIndex;                  /* Index of the HW Serial STC. -1 means use the index passed to NEXUS_StcChannel_Open to select both the HW PCR_OFFSET context and the HW Serial STC.
                                      This is typical when the number of PCR_OFFSET's and STC's are the same. Default is -1. */

    NEXUS_StcChannel_PcrBits pcrBits; /* select PCR bits delivered to the downstream modules */

    struct
    {
        NEXUS_StcChannelPcrModeSettings pcr;
        NEXUS_StcChannelAutoModeSettings Auto;
        NEXUS_StcChannelHostModeSettings host;
    } modeSettings;
} NEXUS_StcChannelSettings;

/*
Summary:
STC Channel status
*/
typedef struct NEXUS_StcChannelStatus
{
    NEXUS_StcChannelMode mode; /* The mode that the STC channel is currently in. This can vary from NEXUS_StcChannelSettings.mode
                                  because of ASTM. */
} NEXUS_StcChannelStatus;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_StcChannel_GetDefaultSettings(
    unsigned index,                        /* index of the StcChannel you want to open */
    NEXUS_StcChannelSettings *pSettings    /* [out] Default Settings */
    );

/**
Summary:
Open a new STC channel
**/
NEXUS_StcChannelHandle NEXUS_StcChannel_Open(  /* attr{destructor=NEXUS_StcChannel_Close}  */
    unsigned index,                            /* index of the StcChannel you want to open. This corresponds to the HW PCR_OFFSET context number.
                                                  See NEXUS_StcChannelSettings.stcIndex if you want to control the selection of your HW STC mapping. */
    const NEXUS_StcChannelSettings *pSettings  /* attr{null_allowed=y} Initial settings */
    );

/**
Summary:
Close an STC channel
**/
void NEXUS_StcChannel_Close(
    NEXUS_StcChannelHandle handle
    );

/**
Summary:
Get current settings
**/
void NEXUS_StcChannel_GetSettings(
    NEXUS_StcChannelHandle handle,
    NEXUS_StcChannelSettings *pSettings          /* [out] Current Settings */
    );

/**
Summary:
Update current settings
**/
NEXUS_Error NEXUS_StcChannel_SetSettings(
    NEXUS_StcChannelHandle handle,
    const NEXUS_StcChannelSettings *pSettings    /* Settings for this StcChannel */
    );

/*
Summary:
Returns status info for this STC channel
*/
NEXUS_Error NEXUS_StcChannel_GetStatus(
    NEXUS_StcChannelHandle handle,
    NEXUS_StcChannelStatus *pStatus /* [out] */
    );

/**
Summary:
Mark the current STC as invalid, preventing any TSM decision from being made until new input is received.

Description:
This is useful when the application transitions the decoders and it knows that no TSM decision should
be made until the StcChannel receives new feedback from the decoders.
**/
NEXUS_Error NEXUS_StcChannel_Invalidate(
    NEXUS_StcChannelHandle handle
    );

/**
Summary:
Get the current STC value.

Description:
For RAVE-based systems, this will return the Serial STC + offset value.
**/
void NEXUS_StcChannel_GetStc(
    NEXUS_StcChannelHandle handle,
    uint32_t *pStc                                /* [out] Current Stc */
    );

/*
Summary:
Force a value to be loaded for the STC.

Description:
This is only effective in NEXUS_StcChannelMode_eHost mode.
*/
NEXUS_Error NEXUS_StcChannel_SetStc(
    NEXUS_StcChannelHandle handle,
    uint32_t stc                                  /* New STC */
    );

/*
Summary:
Freeze will stop the STC from changing value. All other settings are preserved.
*/
NEXUS_Error NEXUS_StcChannel_Freeze(
    NEXUS_StcChannelHandle stcChannel,
    bool frozen /* if true, the STC stops. if false, the STC starts. */
    );

/*
Summary:
Change the rate of the STC. Rate is calculated as: rate = (increment) / (prescale + 1).

Description:
The following are some examples of how the increment and prescale parameters
map to some typical rates:

    1, 0 = 1x (normal speed)
    0, X = 0.0x (paused)
    1, 1 = 0.5x (slow motion)
    2, 0 = 2x (fast forward)
    8, 9 = 0.8x (slow motion, audio trick mode may be possible)
    12, 9 = 1.2x (fast forward, audio trick mode may be possible)

Be aware that the audio and video decoders may not be able to decoder at faster than 1.0x speeds.
They are limited by overall memory bandwidth and RTS (real time scheduler) programming.
Also, speeds that may work for low bitrate (SD) streams or low-complexity streams may not work for
high bitrate (HD) streams or high-complexity streams.

If a decoder cannot keep up with the STC rate, it will drop data. This will result in
video drops or audio pops. Video drops may be acceptable because it will be similar to a host trick mode.
Audio pops are usually not acceptable.

Some versions of the audio decoder support 0.8x and 1.2x audio trick modes.
For any other STC trick mode, the audio decoder should be muted (for fast resumption to normal play) or stopped.
If you are using the Nexus Playback module, this will be done automatically if NEXUS_PlaybackSettings.stcTrick is true.

There is no reverse stc rate.
*/
NEXUS_Error NEXUS_StcChannel_SetRate(
    NEXUS_StcChannelHandle stcChannel,
    unsigned increment,
    unsigned prescale
    );

/**
Summary:
Verify that an STC channel and PID channel have compatible settings

Description:
NEXUS_StcChannel and a live or playback NEXUS_PidChannel are configured separately in an application.
If you misconfigure them (for example, set one to MPEG2TS and another to DSS), you will
get obscure TSM failures. This function allows the VideoDecoder or AudioDecoder, which joins the two
together, to provide more direct feedback on such a misconfiguration.

This function returns a non-zero value if there is a problem.
**/
NEXUS_Error NEXUS_StcChannel_VerifyPidChannel(
    NEXUS_StcChannelHandle stcChannel,
    NEXUS_PidChannelHandle pidChannel
    );

/*
Summary:
Settings passed into NEXUS_StcChannel_SetPairConfiguration
*/
typedef struct NEXUS_StcChannelPairSettings {
    bool connected; /* true if pair of STC's are cconnected */
    unsigned window; /* Window, expressed in mSec. The STC increment is stalled when the comparison exceedes this window. */
} NEXUS_StcChannelPairSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_StcChannel_GetDefaultPairSettings(
            NEXUS_StcChannelPairSettings *pSettings
        );

/*
Summary:
Connects or disconnects pair of STC's

Description:
This is used when transcoding audio and video streams in non-realtime configuration
*/
NEXUS_Error NEXUS_StcChannel_SetPairSettings(
    NEXUS_StcChannelHandle stcChannel1,
    NEXUS_StcChannelHandle stcChannel2,
    const NEXUS_StcChannelPairSettings *pSettings
    );


#ifdef __cplusplus
}
#endif

#endif
