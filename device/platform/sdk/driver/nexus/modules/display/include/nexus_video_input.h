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
 * $brcm_Workfile: nexus_video_input.h $
 * $brcm_Revision: 18 $
 * $brcm_Date: 10/11/12 12:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/include/nexus_video_input.h $
 * 
 * 18   10/11/12 12:25p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
 *  added object verification
 * 
 * 17   4/22/11 3:13p vsilyaev
 * SW7425-416: Updated 3D display API
 * 
 * 16   8/19/10 2:03p jhaberf
 * SW35230-1059: Added HDMI 3D support to nexus header files
 * 
 * 15   9/21/09 2:55p erickson
 * SW3548-2475: update comments. if NEXUS_VideoInputStatus.videoPresent is
 *  false, the other fields have meaning.
 *
 * 14   9/9/09 1:18p erickson
 * SW7405-2977: remove unneeded #include
 *
 * 13   9/1/09 4:23p erickson
 * SWDEPRECATED-2425: update comments
 *
 * 12   4/2/09 9:38a jgarrett
 * PR 53654: Adding option to suspend dynamic RTS for a source
 *
 * 11   3/20/09 1:59p jgarrett
 * PR 53465: Adding numeric refresh rate
 *
 * 10   12/16/08 11:50a jgarrett
 * PR 50002: Adding NEXUS_VideoInput_ForcePending
 *
 * 9   5/9/08 12:44p erickson
 * PR39453: add sourcePending callback, set status.sourcePending based on
 *  VDC dirty bits.
 *
 * 8   4/30/08 3:08p erickson
 * PR39453: add 3548/3556 features
 *
 * 7   4/4/08 10:08a erickson
 * PR40513: add repeat
 *
 * 6   3/12/08 4:31p erickson
 * PR40412: NEXUS_VideoInput_GetStatus requires NEXUS_Error
 *
 * 5   2/29/08 5:39p erickson
 * PR40156: implement ColorMatrix function
 *
 * 4   1/31/08 9:52a erickson
 * PR36808: added include
 *
 * 3   1/23/08 9:22p vobadm
 * PR35457: update docs
 *
 * 2   1/23/08 2:39p erickson
 * PR35457: update docs
 *
 * 1   1/18/08 2:16p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/9   12/18/07 1:34p erickson
 * PR38213: analog video input rework
 *
 * Nexus_Devel/8   11/30/07 10:53a erickson
 * PR35457: api update
 *
 * Nexus_Devel/7   11/2/07 4:42p vsilyaev
 * PR 36696: Used connector model for VideoInput's and VideoOutput's
 *
 * Nexus_Devel/6   10/4/07 3:34p vsilyaev
 * PR 34662: Removed VideoInput descriptor
 *
 * Nexus_Devel/5   9/28/07 12:25p vsilyaev
 * PR 34662: Added sync thunk layer
 *
 * Nexus_Devel/4   9/21/07 4:11p vsilyaev
 * PR 34662: Added VideoInput API
 *
 * Nexus_Devel/3   9/12/07 1:52p vsilyaev
 * PR 34419: Updated display API
 *
 * Nexus_Devel/2   9/10/07 4:35p vsilyaev
 * PR 34662: Added nexus_display_types.h
 *
 * Nexus_Devel/1   9/7/07 2:30p vsilyaev
 * PR 34419: New video modules
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_INPUT_H__
#define NEXUS_VIDEO_INPUT_H__

#include "nexus_types.h"
#include "nexus_display_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Settings which are generic to all video inputs
**/
typedef struct NEXUS_VideoInputSettings
{
    bool mute;                        /* Send the muteColor to the VideoWindow. Actual source data is discarded.
                                         This only applies to analog/hdmi sources. See NEXUS_VideoDecoderSettings.mute for digital. */
    NEXUS_Pixel muteColor;            /* Color of muted video in NEXUS_PixelFormat_eA8_R8_G8_B8 colorspace */
    bool repeat;                      /* Repeat last picture to the VideoWindow. This can be used for a continuous picture during channel change.
                                         This only applies to analog/hdmi sources. See NEXUS_VideoDecoder_ChannelChangeMode for digital.
                                         If mute==true, it will override repeat==true. */
    NEXUS_CallbackDesc sourceChanged; /* Called whenever anything in NEXUS_VideoInputStatus changes. */
    NEXUS_CallbackDesc sourcePending; /* Called whenever NEXUS_VideoInputStatus.sourcePending goes true and a NEXUS_VideoInput_SetResumeMode(videoInput, NEXUS_VideoInputResumeMode_eNow) is required.
                                         See NEXUS_VideoInput_SetResumeMode for more information about source pending and dynamic RTS. */
    struct {
        bool                    enable;
        NEXUS_VideoFormat       videoFormat;
        NEXUS_Video3DStructure  structure;
        NEXUS_Video3DSubSample  subSample;
        bool                    overrideOrientation; /*  force to override source orientation */
        NEXUS_VideoOrientation  orientation; /* orientation of the 3D source */
    } video3DSettings;                /* Settings for 3D sources. */
} NEXUS_VideoInputSettings;

/**
Summary:
Get current settings
**/
void NEXUS_VideoInput_GetSettings(
    NEXUS_VideoInputHandle input,
    NEXUS_VideoInputSettings *pSettings    /* [out] */
    );

/**
Summary:
Apply new settings
**/
NEXUS_Error NEXUS_VideoInput_SetSettings(
    NEXUS_VideoInputHandle input,
    const NEXUS_VideoInputSettings *pSettings
    );

/**
Summary:
Status which is generic to all video inputs
**/
typedef struct NEXUS_VideoInputStatus
{
    bool videoPresent; /* If true, then active video is detected at the source.
                          If false, the other members of NEXUS_VideoInputStatus will be based on the previous active format. */
    unsigned width, height;
    NEXUS_AspectRatio aspectRatio;
    NEXUS_VideoFormat format;           /* You can determine if a source is progressive using NEXUS_VideoFormat_GetInfo(format). */
    NEXUS_VideoFrameRate frameRate;     /* This is an enumerated frame rate code */
    unsigned refreshRate;               /* This is a numeric refresh rate (aka frame rate) value in 1/1000 Hz. e.g. 29.97 = 29970. */
    bool sourcePending; /* Set to true if Nexus is in NEXUS_VideoInputResumeMode_eManual mode and is currently waiting for an eNow trigger.
                           When this is changed to true, you will receive a NEXUS_VideoInputSettings.sourcePending callback.
                           This will not be set to false until NEXUS_VideoInput_SetResumeMode(videoInput, NEXUS_VideoInputResumeMode_eNow) is called. */
} NEXUS_VideoInputStatus;

/**
Summary:
Get current status
**/
NEXUS_Error NEXUS_VideoInput_GetStatus(
    NEXUS_VideoInputHandle input,
    NEXUS_VideoInputStatus *pStatus    /* [out] */
    );

/**
Summary:
The application must call NEXUS_VideoInput_Shutdown after the NEXUS_VideoInput
has been disconnected from all video windows.
This is needed to free internally cached data.
**/
void NEXUS_VideoInput_Shutdown(
    NEXUS_VideoInputHandle input
    );

/**
Summary:
Get current color space convertor matrix
**/
void NEXUS_VideoInput_GetColorMatrix(
    NEXUS_VideoInputHandle input,
    NEXUS_ColorMatrix *pColorMatrix /* [out] */
    );

/**
Summary:
Set new color space convertor matrix

Description:
This only works for analog and HDMI inputs.
It does not work for VideoDecoder inputs.
**/
NEXUS_Error NEXUS_VideoInput_SetColorMatrix(
    NEXUS_VideoInputHandle input,
    const NEXUS_ColorMatrix *pColorMatrix
    );

/*********************
Summary:
Modes used in NEXUS_VideoInput_SetResumeMode

Description:
This controls a feature often referred to as "source pending".
**********************/
typedef enum
{
    NEXUS_VideoInputResumeMode_eAuto,        /* Resume displaying video automatically after source changes. No action is needed by the application. */
    NEXUS_VideoInputResumeMode_eManual,      /* Do not resume displaying video after a source change. Instead, wait for the user to make any related
                                                changes, then resume when they call NEXUS_VideoInput_SetResumeMode(videoInput, NEXUS_VideoInputResumeMode_eNow). */
    NEXUS_VideoInputResumeMode_eNow,         /* Use this enum as a trigger when in NEXUS_VideoInputResumeMode_eManual mode; eNow is not a mode.
                                                After calling NEXUS_VideoInput_SetResumeMode(videoInput, NEXUS_VideoInputResumeMode_eNow), the videoInput will
                                                be in eManual mode. */
    NEXUS_VideoInputResumeMode_eFreezeRts    /* Only applicable to systems that implement dynamic RTS.  If this option is selected, the current RTS
                                                bandwidth will be maintained across all format changes.  This will allow seamless source resolution
                                                changes without an intermediate black screen.  However, this mode has very strict usage guidelines.  Please
                                                make sure all possible source resolutions while this option is selected will have the same constraints and
                                                RTS set ID in your usage mode spreadsheet to avoid display bandwidth overflows.  To enter this mode, your
                                                application should set the resume mode to manual and wait for a source pending callback indicating the
                                                correct resolution.  Once that callback has been received, enter this mode to freeze the bandwidth for that
                                                mode.  To exit this mode, you must select either eAuto or eManual.  Please note, some changes such as window
                                                size may still cause video to mute while bandwidth is adjusted. */
} NEXUS_VideoInputResumeMode;

/**
Summary:
Set the resume mode for a video source. Used for picture quality and dynamic RTS.

Description:
When a video source changes, the application may need to make multiple adjustments to the picture quality or memory RTS (real-time scheduling) settings
before displaying the next frame of video. The ResumeMode allows the application to do this.
Resume mode is also known as "source pending."

There are two main resume modes: eAuto and eManual. eAuto requires no further action by the application.
Video will be displayed immediately after a source change, as if the "source pending" feature was not there.
Dynamic RTS changes will be applied automatically. The only picture quality changes that will be made is to limit features based on RTS requirements.
The default resume mode is eAuto.

In eManual mode, the application can make PQ changes and (if handleDynamicRts is false) can make RTS changes in the source pending state.
In eManual mode the application must either wait for a NEXUS_VideoInputSettings.sourceChanged event
or poll the NEXUS_VideoInputStatus.sourcePending boolean.
Once in source pending state, the application should call NEXUS_VideoInput_GetStatus, make changes based on the new source type,
then call NEXUS_VideoInput_SetResumeMode(videoInput, NEXUS_VideoInputResumeMode_eNow).
Video display will resume and Nexus will remain in eManual mode.

If you switch sources to a VideoWindow and you are in eManual mode, Nexus will trigger a sourceChanged callback and the
system will be in a sourcePending state, as if there was a source format change.

If you use eManual mode, Nexus will still default to handling dynamic RTS. If you would like to manually control dynamic RTS,
you should set NEXUS_VideoInputResumeMode_eNow and also set NEXUS_DisplayModuleSettings.handleDynamicRts = false at system initialization time.

See nexus/examples/source_pending.c for a working example.
**/
NEXUS_Error NEXUS_VideoInput_SetResumeMode(
    NEXUS_VideoInputHandle videoInput,
    NEXUS_VideoInputResumeMode resumeMode
    );

/***************************************************************************
Summary:
Force a video source to the pending state

Description:
If the video input is in eManual resume mode, this can force
the video input to the pending state.  This will allow the
video pipeline to be stopped while RTS changes are made.
**************************************************************************/
NEXUS_Error NEXUS_VideoInput_ForcePending(
    NEXUS_VideoInputHandle videoInput
    );

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_VIDEO_INPUT_H__ */
