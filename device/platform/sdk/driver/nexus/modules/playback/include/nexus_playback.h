/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_playback.h $
 * $brcm_Revision: 52 $
 * $brcm_Date: 9/4/12 4:47p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/playback/include/nexus_playback.h $
 * 
 * 52   9/4/12 4:47p erickson
 * SW7425-3594: add NEXUS_PlaybackTrickModeSettings.maxFrameRepeat for use
 *  with simulated tsm
 * 
 * 51   6/15/12 11:51a erickson
 * SW7425-3081: add IsTransportTypeSupported
 * 
 * 50   6/14/12 2:35p erickson
 * SW7425-2741: add AudioDecoderPrimer
 * 
 * SW7425-2741/1   6/7/12 7:18p ahulse
 * SW7425-2741: add audio primer
 * 
 * 49   2/21/12 12:20p erickson
 * SW7208-183: convert NEXUS_PlaybackPidChannelSettings union to struct
 *  for consistent default state
 * 
 * 48   12/20/11 1:43p vsilyaev
 * SW7425-2034: Used 32-bit type for program id (pid) types
 * 
 * 47   11/11/11 11:28a erickson
 * SWDEPRECATED-2425: fix NEXUS_PlaybackHostTrickMode comments
 * 
 * 46   10/12/11 9:32a erickson
 * SW7425-1264: clarify automatic and custom trick modes
 *
 * 45   9/22/11 4:43p erickson
 * SW7420-1965: add simple decoder support to nexus playback module
 *
 * 44   8/31/11 2:44p vsilyaev
 * SW7420-1978: Add selection of index type for playback of MPEG-2 TS
 *  files
 *
 * 43   8/29/11 10:32a erickson
 * SW7425-1205: clarify NEXUS_PlaybackHostTrickMode_ePlayGop
 *
 * 42   8/12/11 9:40a erickson
 * SW7358-83: add NEXUS_PlaybackTrickModeSettings.brcmTrickMode
 *
 * 41   6/17/11 2:33p erickson
 * SWDEPRECATED-2425: update comments
 *
 * 40   5/26/11 7:32p vsilyaev
 * SW7425-646: Added configurable 'gap' for timeshifting
 *
 * 39   12/14/10 3:25p erickson
 * SW7420-1285: add null_allowed attribute
 *
 * 38   9/2/10 4:27p erickson
 * SWDEPRECATED-1003: added NEXUS_PlaybackHostTrickMode_eTimeSkip
 *
 * 37   8/31/10 2:44p erickson
 * SWGIGGSVIZIO-57: clarify comments for NEXUS_PlaybackMode_eFixedBitrate
 *
 * 36   8/9/10 8:15a erickson
 * SW3548-3045: clarify comments
 *
 * 35   7/14/10 12:00p erickson
 * SW7405-4621: change to nexus_base_os.h
 *
 * 34   7/14/10 10:32a erickson
 * SW7405-4621: NEXUS_ThreadSettings requires nexus_base.h, for now.
 *
 * 33   6/29/10 3:20p erickson
 * SW7405-4367: playback should flush when transitioning decoder skip
 *  modes. also added NEXUS_PlaybackTrickModeSettings.avoidFlush option to
 *  disable this behavior.
 *
 * 32   6/11/10 1:03p erickson
 * SWDEPRECATED-2425: update comments
 *
 * 31   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 *
 * 30   1/13/10 5:41p vsilyaev
 * SW3556-913: Added option control error handling for Play/Seek/SetTrick
 *  operations
 *
 * 29   12/28/09 2:53p erickson
 * SW3556-957: added comment to NEXUS_PlaybackTrickModeSettings about
 *  trick mode limitations
 *
 * 28   12/17/09 6:16p vsilyaev
 * SW3556-913: Added option to specify timeout on asynchronous I/O
 *
 * 27   12/17/09 5:52p vsilyaev
 * SW3548-2677: Added configuration that allows user to choose what
 *  context should be used for synchronous I/O
 *
 * 26   11/4/09 11:31a erickson
 * SWDEPRECATED-2425: update comments for NEXUS_PlaybackTrickModeSettings
 *  and related enums
 *
 * 25   7/21/09 5:57p vsilyaev
 * PR 56977: Nexus playback always control audio and video decoder.
 *  Provided way for the user to control the decoder slowmotion factor
 *  using the playback trick API.
 *
 * 24   7/1/09 12:09p erickson
 * PR56169: clarify comment
 *
 * 23   6/19/09 4:11p vsilyaev
 * PR 56169: PR 53824: Allow user to sepcify max decodder rate. This
 *  superceeds overrideStcTrick.
 *
 * 22   4/20/09 2:47p erickson
 * PR54166: update comment for NEXUS_Playback_Seek
 *
 * 21   4/3/09 12:56p erickson
 * PR53824: reenable stc trick modes. add
 *  NEXUS_PlaybackTrickModeSettings.overrideStcTrick for additional app
 *  control.
 *
 * 20   3/27/09 12:51p vsilyaev
 * PR 50311: Return readPosition in the Nexus playback status. In
 *  combination with the position it could be used to measure depth of
 *  decoder and playback buffers in units of time.
 *
 * 19   2/2/09 2:26p erickson
 * PR47815: added NEXUS_PlaybackHostTrickMode and
 *  NEXUS_PlaybackTrickModeSettings.mode
 *
 * 18   1/26/09 11:04a vsilyaev
 * PR 51579: Adding originalTransportType to the PlaypumpSettings
 *
 * 17   1/12/09 5:45p vsilyaev
 * PR 50763: Improved seek to position 0 after rewind reached begining of
 *  the file
 *
 * 16   12/19/08 5:59p vsilyaev
 * PR 50214: Added callbacks and counters for parsing index files
 *
 * 15   12/10/08 2:57p vsilyaev
 * PR 48760: Removed duplicated information
 *
 * 13   12/10/08 2:06a vsilyaev
 * PR 48760: Using ES player for accurate progress monitor of MP3 streams
 *
 * 12   12/8/08 11:19a erickson
 * PR49930: add NEXUS_PlaybackSettings.accurateSeek
 *
 * 11   11/19/08 1:29p erickson
 * PR35457: update comments
 *
 * 10   10/16/08 3:37p erickson
 * PR47673: add comment on NEXUS_PlaybackSettings.stcTrick
 *
 * 9   10/9/08 2:23p erickson
 * PR47608: NEXUS_PlaybackSettings.stcChannel is now required for TSM
 *  playback. NEXUS_PlaybackSettings.stcTrick was added for stc trick
 *  modes.
 *
 * 8   6/24/08 4:07p erickson
 * PR44107: rename to TrickMode
 *
 * 7   6/12/08 3:02p erickson
 * PR43606: rename NEXUS_PlaybackStreamSettings to
 *  NEXUS_PlaybackStartSettings
 *
 * 6   5/30/08 10:53a erickson
 * PR34925: add comment
 *
 * 5   5/13/08 4:41p erickson
 * PR42119: clarify comments, especially requirement that pidchannels be
 *  opened before Start for some transport types
 *
 * 4   4/25/08 1:24p erickson
 * PR41951: NEXUS_Playback_ClosePidChannel should return void
 *
 * 3   1/23/08 8:36p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 5:16p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/11   12/20/07 2:08p vsilyaev
 * PR 37574: Use explicitly defined PLAY_SPEED
 *
 * Nexus_Devel/10   12/20/07 10:29a vsilyaev
 * PR 38073: Updated to work with new bmedia_player
 *
 * Nexus_Devel/PR38073/1   12/18/07 4:57p vsilyaev
 * PR 38073: Updated playback module to work exclusively with media player
 *
 * Nexus_Devel/9   11/29/07 2:28p erickson
 * PR35457: doc update
 *
 * Nexus_Devel/8   11/16/07 12:38p vsilyaev
 * PR 35824: Added STC trickmodes
 *
 * Nexus_Devel/7   11/12/07 2:35p erickson
 * PR35824: added NEXUS_Playback_GetStatus
 *
 * Nexus_Devel/6   10/17/07 5:37p vsilyaev
 * PR 35824: Added trickmode test
 *
 * Nexus_Devel/5   10/15/07 2:55p vsilyaev
 * PR 35824: Added synchronization thunk layer
 *
 * Nexus_Devel/4   10/12/07 5:44p vsilyaev
 * PR 35824: Added more implementation for playback module
 *
 * Nexus_Devel/2   10/10/07 5:33p vsilyaev
 * PR 35824: Added association with the audio/video decoders
 *
 * Nexus_Devel/1   10/10/07 4:54p vsilyaev
 * PR 35824: Playback module
 *
 **************************************************************************/
#ifndef NEXUS_PLAYBACK_H__
#define NEXUS_PLAYBACK_H__

#include "nexus_base_os.h"
#include "nexus_file.h"
#include "nexus_playpump.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_primer.h"
#if NEXUS_HAS_SIMPLE_DECODER
#include "nexus_simple_video_decoder.h"
#include "nexus_simple_audio_decoder.h"
#else
typedef void *NEXUS_SimpleVideoDecoderHandle;
typedef void *NEXUS_SimpleAudioDecoderHandle;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Handle for the Playback Interface.

Description:
The Playback Interface provides high-level PVR playback functionality.
It integrates the data feeding (NEXUS_PlaypumpHandle) and decoder control (NEXUS_VideoDecoder and NEXUS_AudioDecoder).

It provides a set of trick mode options using host, decoder and transport based control.

It integrates file I/O using NEXUS_FileHandle.

It integrates with NEXUS_RecordHandle for timeshifting and continuous record.
**/
typedef struct NEXUS_Playback *NEXUS_PlaybackHandle;

/**
Summary:
NEXUS_PlaybackLoopMode is a value you tell the playback engine what action to take
when end of file and begining of file is reached.

Description:
We implemented loopmode as a callback because the app may take different actions
at different times and because the app might need to be notified when
loops take place. This accomplishes both.
**/
typedef enum NEXUS_PlaybackLoopMode
{
    NEXUS_PlaybackLoopMode_eLoop,   /* Loop to the other end of the stream */
    NEXUS_PlaybackLoopMode_ePause,  /* Pause the decode at the current spot */
    NEXUS_PlaybackLoopMode_ePlay,   /* Resume normal play at the beginning or pause at the end. */
    NEXUS_PlaybackLoopMode_eMax
} NEXUS_PlaybackLoopMode;

/**
Summary:
NEXUS_PlaybackErrorHandlingMode is a value you tell the playback engine what action to take
when error detected when reading or processing playback data.

**/
typedef enum NEXUS_PlaybackErrorHandlingMode
{
    NEXUS_PlaybackErrorHandlingMode_eIgnore,        /* Ignore error and proceed further */
    NEXUS_PlaybackErrorHandlingMode_eAbort,         /* When error detected pause decoders and abort playback, Application should call NEXUS_Playback_Stop,
                                                       application may be able to restart depending on the nature of the error. */
    NEXUS_PlaybackErrorHandlingMode_eEndOfStream,   /* When error treat it as was end of stream, it then would trigger endOfStreamAction/beginningOfStreamAction callbacks */
    NEXUS_PlaybackErrorHandlingMode_eMax
} NEXUS_PlaybackErrorHandlingMode;

/*
Summary:
General settings for Playback
*/
typedef struct NEXUS_PlaybackSettings
{
    NEXUS_PlaypumpHandle playpump;              /* playpump that is used by playback engine to feed data to the decoders */
    NEXUS_StcChannelHandle stcChannel;          /* stcChannel is required for playback with TSM. */
    NEXUS_PlaypumpSettings playpumpSettings;    /* configuration for the playpump. Note that this will overwrite any call to NEXUS_Playpump_SetSettings that you make. */
    NEXUS_PlaybackLoopMode endOfStreamAction;   /* action that shall be taken at the end of stream */
    NEXUS_PlaybackLoopMode beginningOfStreamAction; /* action that shall be taken at the begining of stream */
    NEXUS_PlaybackErrorHandlingMode playErrorHandling;  /* action should be taken when error encountered while playing stream, this mode is used in either normal or trick play. */
    NEXUS_PlaybackErrorHandlingMode seekErrorHandling;  /* action should be taken when error encountered while executing NEXUS_Playback_Seek function */
    NEXUS_PlaybackErrorHandlingMode trickErrorHandling;  /* action should be taken when error encountered while executing NEXUS_Playback_TrickMode or NEXUS_Playback_Play functions */
    NEXUS_CallbackDesc endOfStreamCallback;
    NEXUS_CallbackDesc beginningOfStreamCallback;
    NEXUS_CallbackDesc errorCallback;            /* Called when an I/O error happens. Playback could be paused internally, depending on the value of playErrorHandling */
    NEXUS_CallbackDesc parsingErrorCallback;     /* Callback called when error was detected in processing of index or data payloads */
    bool startPaused;   /* Start playback in a paused state. NEXUS_Playback_Play should be called after NEXUS_Playback_Start. */
    bool timeshifting;  /* Enable timeshifting, e.g. when playback is reached end of file, it shall wait for record to pump more into the file */

    bool stcTrick; /* If true, STC trick modes will be used instead of decoder trick modes for forward trick modes.
                      See NEXUS_StcChannel_SetRate for details on STC trick modes.
                      Must be set before NEXUS_Playback_Start.
                      See NEXUS_PlaybackTrickModeSettings.maxDecoderRate for an override based on decoder capabilities.
                      This is only used of mode == NEXUS_PlaybackHostTrickMode_eNone. For other values of mode, 'rateControl' is used. */

    bool accurateSeek; /* When to a new location, or when resuming normal play from a trick mode, an algorithm can be run which
                          attempts to resume play at the exact frame. This involves decoding but not displaying pictures from the random access point (e.g. I frame)
                          until a certain timestamp is reached.
                          The downside to this algorithm is that for high bitrate streams, this seek time may be long.
                          Also, for streams with bad PTS's, the desired resume location may never occur.
                          This feature defaults off. */
    bool enableStreamProcessing; /* enable use of extra stream processing, type of processing is depend on the stream type. For example some ES streams are converted to the PES streams for better navigation */
    unsigned ioTimeout; /* timeout for asynchronous I/O, ms */
    struct {
        unsigned endOfStreamGap; /* milliseconds to gap between end of playback and record which will not cause decode stuttering */
        unsigned beginningOfStreamGap; /* milliseconds to gap between beginning of playback and truncating record which will not cause excess beginningOfStream events */
    } timeshiftingSettings;
} NEXUS_PlaybackSettings;


/**
Summary:
Setting used for custom control of host and decoder trick modes

Description:
Please note that NEXUS_PlaybackHostTrickMode is misnamed. It can apply to both host and decoder trick modes.

See NEXUS_PlaybackTrickModeSettings for a full discussion.
**/
typedef enum NEXUS_PlaybackHostTrickMode
{
    NEXUS_PlaybackHostTrickMode_eNone,      /* Automatic control. See NEXUS_PlaybackTrickModeSettings for full discussion. */

    /* the following enums provide variations of custom control */
    NEXUS_PlaybackHostTrickMode_eNormal,    /* Normal host playback, but NEXUS_PlaybackTrickModeSettings.rate will be used as decoder rate control. */
    NEXUS_PlaybackHostTrickMode_ePlayI,     /* I frame host or decoder trick mode (as determined by NEXUS_PlaybackSkipControl).
                                               If skipControl == NEXUS_PlaybackSkipControl_eHost, the mode_modifier specifies the # of I frames to skip. */
    NEXUS_PlaybackHostTrickMode_ePlaySkipB, /* Plays all I's, all P's and some B's. mode_modifier specifies the # of adjacent B's to play before a skip.
                                               Only supported for NEXUS_PlaybackSkipControl_eHost. */
    NEXUS_PlaybackHostTrickMode_ePlayIP,    /* Play all I's and P's (i.e. skips all B's) with with eHost or eDecoder control (as determined by NEXUS_PlaybackSkipControl).
                                               mode_modifier is ignored. */
    NEXUS_PlaybackHostTrickMode_ePlaySkipP, /* Plays all I's and some P's. mode_modifier specifies the # of P's per GOP to decode.
                                               Only supported for NEXUS_PlaybackSkipControl_eHost. */
    NEXUS_PlaybackHostTrickMode_ePlayBrcm,  /* BRCM trick mode. mode_modifier is the rate in integer units of frames (e.g. -1 is 1x rewind, 2x is 2x fast-forward). */
    NEXUS_PlaybackHostTrickMode_ePlayGop,   /* DQT/GOP trick mode. mode_modifier has three parts:
                                                 mode_modifier % 100 is the number of frames per GOP to send to the decoder.
                                                 mode_modifier / 100 is the number of GOPs to skip.
                                                 the sign is the direction: negative is reverse, positive is forward. */
    NEXUS_PlaybackHostTrickMode_eTimeSkip,  /* Host trick mode that attempts to give a constant trick mode rate by adapting to GOP size changes.
                                               mode_modifier is the number of milliseconds between pictures that bcmplayer should try to output. */
    NEXUS_PlaybackHostTrickMode_eMax
} NEXUS_PlaybackHostTrickMode;

/*
Summary:
Determines how NEXUS_PlaybackTrickModeSettings.rate is applied if mode != eNone
*/
typedef enum NEXUS_PlaybackRateControl
{
    NEXUS_PlaybackRateControl_eStream,  /* NEXUS_PlaybackTrickModeSettings.rate sets the STC trick rate. TSM is required.
                                           As of now, this must be positive because HW STC only runs forward. */
    NEXUS_PlaybackRateControl_eDecoder, /* NEXUS_PlaybackTrickModeSettings.rate sets the decoder rate. TSM is not used.
                                           In this mode, 'rate' has two parts: the sign and the absolute value.
                                           The sign determines forward/reverse direction and can be positive or negative.
                                           The absolute value determines slow motion based on abs(rate)/NEXUS_NORMAL_PLAY_SPEED.
                                           abs(rate) > NEXUS_NORMAL_PLAY_SPEED is not currently supported. */
    NEXUS_PlaybackRateControl_eMax
} NEXUS_PlaybackRateControl;

/*
Summary:
Determines how frames are skipped if mode != eNone
*/
typedef enum NEXUS_PlaybackSkipControl
{
    NEXUS_PlaybackSkipControl_eHost,    /* host skips pictures. all values of NEXUS_PlaybackHostTrickMode supported. */
    NEXUS_PlaybackSkipControl_eDecoder, /* decoder skips pictures. only eI and eIP modes supported. mode_modifer is unused. */
    NEXUS_PlaybackSkipControl_eMax
} NEXUS_PlaybackSkipControl;

/*
Summary:
All parameters needed to specify a trick mode.

Description:
Playback will select from a variety of trick mode capabilities in order to achieve the rate. This could include
host trick modes, PTS remapping, decode skip modes, STC trick modes, etc.

Nexus exposes trick play in two modes: automatic and custom.
1) In automatic mode, you simply set 'rate'. 'mode' must be left as the default value of eNone.
    mode_modifier, rateControl and skipControl are unused.
2) In custom mode, you set 'mode' to something other than eNone, then rate, mode_modifier, rateControl and skipControl are used.
    NEXUS_PlaybackSettings.stcTrick is unused.

In both automatic and custom mode, direction is determined by the sign of 'rate'.

In custom mode, the sign of 'rate' must match the sign of 'mode_modifier'. For instance, if mode = eI and mode_modifier = -1, then the host
is doing a reverse I-frame trick; therefore, if you want to add a 2x decoder slow motion, you must set rateControl = eDecoder
and rate = -2*NEXUS_NORMAL_PLAY_SPEED.
This is required because rate controls the display rate, which is independent of the host skip control.
For instance, if rateControl = eStream, then an STC trick can be applied on top of a host or decoder skip mode.

Some types of trick modes are very limited because of either hardware or codec limitations.
For instance, using STC, decoder and host trick modes, a wide range of forward speeds and types are possible.
However, only a small set of actual reverse speeds are possible and only with host trick modes.
*/
typedef struct NEXUS_PlaybackTrickModeSettings
{
    int rate; /* trick mode rate defined in NEXUS_NORMAL_PLAY_SPEED units.
                   For NEXUS_PlaybackHostTrickMode_eNone, rate specifies the overall rate of the trick mode. Some examples are:

                       NEXUS_NORMAL_PLAY_SPEED    = normal playback
                       0                          = pause
                       NEXUS_NORMAL_PLAY_SPEED/2  = 0.5x slow motion forward
                       NEXUS_NORMAL_PLAY_SPEED*2  = 2x fast forward
                       NEXUS_NORMAL_PLAY_SPEED*-1 = 1x rewind
                       NEXUS_NORMAL_PLAY_SPEED*-7 = 7x rewind

                   For other values of 'mode', the meaning of rate depends on rateControl and its value is factored in
                   as only one part of the trick mode. */

    /* custom trick mode control (only applies if 'mode' is other than eNone) */
    NEXUS_PlaybackHostTrickMode mode; /* allows direct control of trick mode */
    int mode_modifier; /* modifier of mode if skipControl = eHost. see NEXUS_PlaybackHostTrickMode for specific meanings.
                          if skipControl = eDecoder, it must be left to default. */
    NEXUS_PlaybackRateControl rateControl; /* sets the meaning of rate */
    NEXUS_PlaybackSkipControl skipControl; /* selects whether specific NEXUS_PlaybackHostTrickMode modes will apply to decoder or host. */

    /* general capabilities. these apply for all values of 'mode' */
    unsigned maxDecoderRate; /* Maximum fast-forward playback rate supported by the decoder. Defined in NEXUS_NORMAL_PLAY_SPEED units.
                                For forward rates up to and including this rate, Nexus will feed the entire stream to the decoders and then use either STC trick modes
                                or host paced trickmode to achieve target rate.
                                However, decoder bandwidth may prevent the system from achieving the desired rate.
                                The maximum rate varies based on hardware capabilities, stream encoding and MEMC RTS settings.
                                Because of this, the application can tell Playback exactly what rate it knows is achievable. For any trick mode about the maxDecoderRate,
                                Nexus will use host-based trick modes to reduce the decoder bandwidth requirements.
                                Also, if you want to use STC pause and not STC fast-forward, you can set maxDecoderRate = NEXUS_NORMAL_PLAY_SPEED. */

    bool avoidFlush;     /* When making decoder trick mode transitions, it is not required that the pipeline be flushed.
                            If there is a flush, the current position will be reset to the nearest random access point and the trick mode will be seen immediately.
                            If there is no flush, the current position will not change, but the new trick mode will be delayed as the already-decoded pictures are fed out.
                            Set avoidFlush = true to get the latter behavior.
                            Any trick mode transition that changes the byte stream and could macroblock will always have a flush. */
    bool brcmTrickMode; /* use broadcom trick modes for certain rates. set to false to avoid using brcm trick modes. */
    
    unsigned maxFrameRepeat; /* if zero, there is no max. if non-zero, only repeat pictures this number of times for
                            a minimum smoothness. only used in NEXUS_TsmMode_eSimulated. */
} NEXUS_PlaybackTrickModeSettings;

/**
Summary:
State of Playback returned by NEXUS_Playback_GetStatus.
**/
typedef enum NEXUS_PlaybackState
{
    NEXUS_PlaybackState_eStopped,       /* Either never started or has been stopped. */
    NEXUS_PlaybackState_ePlaying,       /* Normal playback */
    NEXUS_PlaybackState_ePaused,        /* Decode is paused but can be resumed. */
    NEXUS_PlaybackState_eTrickMode,     /* Some variety of trick mode in progress. */
    NEXUS_PlaybackState_eAborted,       /* Aborted due to an internal error. App must stop and restart to continue playback. */
    NEXUS_PlaybackState_eMax
} NEXUS_PlaybackState;

/**
Summary:
playback position in milliseconds
**/
typedef unsigned long NEXUS_PlaybackPosition;

/**
Summary:
Mode used in NEXUS_PlaybackStartSettings
**/
typedef enum NEXUS_PlaybackMode
{
    NEXUS_PlaybackMode_eIndexed, /* use index to decode a stream */
    NEXUS_PlaybackMode_eFixedBitrate, /* don't use index. for navigation use fixed bitrate provided with NEXUS_PlaybackStartSettings.bitrate */
    NEXUS_PlaybackMode_eAutoBitrate, /* don't use index. for navigation use auto-detected bitrate in the stream. NEXUS_PlaybackStartSettings.bitrate is used as a hint */
    NEXUS_PlaybackMode_eMax
} NEXUS_PlaybackMode;

/**
Summary:
Index file I/O mode used in NEXUS_PlaybackStartSettings

Description:
The nexus playback modules has some amount of synchronous file I/O originated from its internal execution
environment, usually it completes reasonably fast, however there are cases where even small amount of I/O could cause large latencies.

This enum gives options on internal execution context.
**/
typedef enum NEXUS_PlaybackIndexFileIo
{
    NEXUS_PlaybackIndexFileIo_eCallback, /* handle it directly in the callback */
    NEXUS_PlaybackIndexFileIo_eModule,  /* switch from the callback to this module's context */
    NEXUS_PlaybackIndexFileIo_eThread,  /* switch from the callback to a dedicated thread */
    NEXUS_PlaybackIndexFileIo_eMax
} NEXUS_PlaybackIndexFileIo;

/**
Summary:
Selects type of the index when playing MPEG-2 TS file
**/
typedef enum NEXUS_PlaybackMpeg2TsIndexType
{
    NEXUS_PlaybackMpeg2TsIndexType_eAutoDetect, /* detect index file type based on the index content*/
    NEXUS_PlaybackMpeg2TsIndexType_eNav,
    NEXUS_PlaybackMpeg2TsIndexType_eSelf,       /* MPEG-2 TS file used as an index */
    NEXUS_PlaybackMpeg2TsIndexType_eMax
} NEXUS_PlaybackMpeg2TsIndexType;

/**
Summary:
Structure used in NEXUS_Playback_Start
**/
typedef struct NEXUS_PlaybackStartSettings
{
    NEXUS_PlaybackMode mode; /* playback mode */
    unsigned bitrate; /* stream bitrate, in bits per second. see NEXUS_PlaybackMode for options. */
    NEXUS_PlaybackMpeg2TsIndexType mpeg2TsIndexType;
    struct {
        NEXUS_PlaybackIndexFileIo mode; /* control what context is used for index file I/O */
        NEXUS_ThreadSettings threadSettings; /* if indexFileIo.mode == NEXUS_PlaybackIndexFileIo_eThread, this settings are used to spawn thread */
    } indexFileIo;
#if defined(HUMAX_PLATFORM_BASE) /*Added for delay-recording but required also to non-delay-recording model.*/	
    unsigned long delayRecordedOffsetAlign; /* for delay record file playback */
    unsigned long delayedRecordingStatus; /*current delayed recording status: bit0: 1 (current mode is delayed recording)*/
#endif	
} NEXUS_PlaybackStartSettings;


/*
Summary:
Create a new playback instance.
*/
NEXUS_PlaybackHandle NEXUS_Playback_Create(void);

/*
Summary:
Destroy a playback instance.
*/
void NEXUS_Playback_Destroy(
    NEXUS_PlaybackHandle playback
    );

/*
Summary:
Get default settings
*/
void NEXUS_Playback_GetDefaultStartSettings(
    NEXUS_PlaybackStartSettings *pSettings /* [out] */
    );

/*
Summary:
Start playback.

Description:
If NEXUS_PlaybackSettings.startPaused is true, data flow through the media framework and into transport hardware does not begin
until NEXUS_Playback_Play is called.

For NEXUS_TransportTypes which use media framework, it's important that all PidChannels are opened before NEXUS_Playback_Start is called.
*/
NEXUS_Error NEXUS_Playback_Start(
    NEXUS_PlaybackHandle playback,
    NEXUS_FilePlayHandle file,
    const NEXUS_PlaybackStartSettings *pSettings
    );

/*
Summary:
Stop playback.
*/
void NEXUS_Playback_Stop(
    NEXUS_PlaybackHandle playback
    );

/*
Summary:
Status returned by NEXUS_Playback_GetStatus
*/
typedef struct NEXUS_PlaybackStatus
{
    NEXUS_PlaybackPosition position;          /* current timestamp of stream played, time in mSec */
    NEXUS_PlaybackState state;          /* Current state of playback */
    NEXUS_PlaybackTrickModeSettings trickModeSettings; /* Current trick mode settings */
    unsigned fifoDepth;                 /* Depth in bytes of the playback buffer */
    unsigned fifoSize;                  /* Size in bytes of the playback buffer */
    uint64_t bytesPlayed;               /* Total number of bytes played since starting */
    NEXUS_PlaybackPosition first;            /* first timestamp of stream played, time in mSec */
    NEXUS_PlaybackPosition last;             /* last timestamp of stream played time in mSec */
    unsigned indexErrors;               /* count of error discovered when parsing index */
    unsigned dataErrors;                /* count of errors discovered when processing data payloads*/
    NEXUS_PlaybackPosition readPosition;  /* current read position of the stream played, time in mSec */
} NEXUS_PlaybackStatus;

/*
Summary:
Get current status
*/
NEXUS_Error NEXUS_Playback_GetStatus(
    NEXUS_PlaybackHandle playback,
    NEXUS_PlaybackStatus *pStatus /* [out] */
    );

/*
Summary:
Pause playback after it has started.

Description:
It can only be called after playback has started.
*/
NEXUS_Error NEXUS_Playback_Pause(
    NEXUS_PlaybackHandle playback
    );

/*
Summary:
Resume normal playback from pause or trick mode.

Description:
It can only be called after playback has been started.
*/
NEXUS_Error NEXUS_Playback_Play(
    NEXUS_PlaybackHandle playback
    );

/*
Summary:
If you are paused, this will advance or reverse one frame.

Description:
It can only be called after playback has been started.
*/
NEXUS_Error NEXUS_Playback_FrameAdvance(
    NEXUS_PlaybackHandle playback,
    bool forward                        /* is it forward? otherwise, reverse. */
    );

/*
Summary:
Set a trick mode, either fast or slow, forward or reverse.

Description:
It can only be called after playback has started.
*/
NEXUS_Error NEXUS_Playback_TrickMode(
    NEXUS_PlaybackHandle playback,
    const NEXUS_PlaybackTrickModeSettings *pTrickModeSettings
    );

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new structure members in the future.
**/
void NEXUS_Playback_GetDefaultTrickModeSettings(
    NEXUS_PlaybackTrickModeSettings *pTrickModeSettings
    );

/**
The following names are deprecated.
**/
#define NEXUS_Playback_GetDefaultTrickmodeSettings NEXUS_Playback_GetDefaultTrickModeSettings
#define NEXUS_Playback_Trickmode NEXUS_Playback_TrickMode
#define NEXUS_PlaybackState_eTrickmode NEXUS_PlaybackState_eTrickMode

/**
Summary:
For use in the NEXUS_PlaybackTrickModeSettings.rate parameter

Description:
The value of this macro may change in the future and should not affect any application if relative values are used.
**/
#define NEXUS_NORMAL_PLAY_SPEED 1000

/**
Summary:
Settings used in NEXUS_Playback_OpenPidChannel

Description:
When giving the audio or video decoder to playback, the app must not call API's the Playback is calling.
If the app does, the collision will result in undefined behavior.

Playback consumes all functions in nexus_video_decoder_trick.h and nexus_audio_decoder_trick.h.
Playback also consumes NEXUS_VideoDecoder_SetStartPts if accurateSeek is enabled.

The application should still call functions in nexus_video_decoder.h and nexus_audio_decoder.h.
This includes functions like Start, Stop. Playback will not call them.
**/
typedef struct NEXUS_PlaybackPidChannelSettings
{
    NEXUS_PlaypumpOpenPidChannelSettings pidSettings;
    struct
    {
        struct
        {
            NEXUS_VideoDecoderHandle decoder; /* see above for API implications */
            NEXUS_SimpleVideoDecoderHandle simpleDecoder; /* alternative to 'decoder' */
            NEXUS_VideoCodec codec;
            bool index;                  /* use index for this video pid. */
        } video; /* settings for pidSettings.pidType == NEXUS_PidType_eVideo */
        struct
        {
            NEXUS_AudioDecoderHandle primary;  /* see above for API implications */
            NEXUS_AudioDecoderHandle secondary;  /* see above for API implications */
            NEXUS_SimpleAudioDecoderHandle simpleDecoder; /* alternative to 'primary' and 'secondary' */
            NEXUS_AudioDecoderPrimerHandle primer;
        } audio; /* settings for pidSettings.pidType == NEXUS_PidType_eAudio */
    } pidTypeSettings;
} NEXUS_PlaybackPidChannelSettings;

/**
Summary:
Seek to a new location in the stream

Description:
You cannot call NEXUS_Playback_Seek until after NEXUS_Playback_Start has been called.
Playback does not have any file information until started, therefore no seek is possible.
If you want to start from a location other than the beginning, do the following steps:
1) call NEXUS_Playback_SetSettings with NEXUS_PlaybackSettings.startPaused = true
2) call NEXUS_Playback_Start
3) call NEXUS_Playback_Seek
4) call NEXUS_Playback_Play
**/
NEXUS_Error NEXUS_Playback_Seek(
    NEXUS_PlaybackHandle playback,
    NEXUS_PlaybackPosition position /* new position */
    );

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new structure members in the future.
**/
void NEXUS_Playback_GetDefaultPidChannelSettings(
    NEXUS_PlaybackPidChannelSettings *pSettings /* [out] */
    );

/**
Summary:
NEXUS_Playback_OpenPidChannel can be called before or after Start and Stop.
However, some transport types which use the media framework require that all pids be opened before Start.
**/
NEXUS_PidChannelHandle NEXUS_Playback_OpenPidChannel(
    NEXUS_PlaybackHandle playback,
    unsigned pid,
    const NEXUS_PlaybackPidChannelSettings *pSettings /* attr{null_allowed=y} */
    );

/**
Summary:
NEXUS_Playback_GetPidChannelSettings is used to get current configuration for the pidChannel opened with NEXUS_Playback_OpenPidChannel

Description:
This function can fail if the pidChannel does not belong to this Playback.
**/
NEXUS_Error NEXUS_Playback_GetPidChannelSettings(
    NEXUS_PlaybackHandle playback,
    NEXUS_PidChannelHandle pidChannel,
    NEXUS_PlaybackPidChannelSettings *pSettings /* [out] */
    );

/**
Summary:
NEXUS_Playback_SetPidChannelSettings is used to change current configuration for the pidChannel opened with NEXUS_Playback_OpenPidChannel

Description:
Main purpose of the NEXUS_Playback_SetPidChannelSettings is to change decoder assigment.
**/
NEXUS_Error NEXUS_Playback_SetPidChannelSettings(
    NEXUS_PlaybackHandle playback,
    NEXUS_PidChannelHandle pidChannel,
    const NEXUS_PlaybackPidChannelSettings *pSettings
    );

/**
Summary:
Closes a pid channel opened by NEXUS_Playback_OpenPidChannel
**/
void NEXUS_Playback_ClosePidChannel(
    NEXUS_PlaybackHandle playback,
    NEXUS_PidChannelHandle pidChannel
    );

/**
Summary:
Close all pid channels opened by NEXUS_Playback_OpenPidChannel
**/
void NEXUS_Playback_CloseAllPidChannels(
    NEXUS_PlaybackHandle playback
    );

/**
Summary:
Returns current configuration of playback
**/
void NEXUS_Playback_GetSettings(
    NEXUS_PlaybackHandle playback,
    NEXUS_PlaybackSettings *pSettings /* [out] */
    );

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new structure members in the future.
**/
void NEXUS_Playback_GetDefaultSettings(
    NEXUS_PlaybackSettings *pSettings /* [out] */
    );

/**
Summary:
Changes configuration of playback.

Description:
Some configuration (namely, playpump and playpumpSettings) can not be changed
unless playback is stopped and all pids are closed.
**/

NEXUS_Error NEXUS_Playback_SetSettings(
    NEXUS_PlaybackHandle playback,
    const NEXUS_PlaybackSettings *pSettings
    );

/**
Summary:
Learn if playback supports the transport type
**/
void NEXUS_Playback_IsTransportTypeSupported(
    NEXUS_PlaybackHandle playback,
    NEXUS_TransportType transportType,
    bool *pIsSupported
    );
    
#ifdef __cplusplus
}
#endif

#endif /* NEXUS_PLAYBACK_H__ */

