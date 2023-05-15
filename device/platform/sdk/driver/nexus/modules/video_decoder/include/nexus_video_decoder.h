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
 * $brcm_Workfile: nexus_video_decoder.h $
 * $brcm_Revision: 94 $
 * $brcm_Date: 10/12/12 3:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/include/nexus_video_decoder.h $
 * 
 * 94   10/12/12 3:15p vsilyaev
 * SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle
 * 
 * 93   9/13/12 2:37p erickson
 * SW7435-351: add NEXUS_VideoDecoderStreamInformation.valid
 * 
 * 92   9/6/12 2:10p erickson
 * SW7405-3276: clarify NEXUS_VideoDecoder_GetConnector with regard to
 *  AddInput/RemoveInput and manualPowerState
 * 
 * 91   7/20/12 2:51p erickson
 * SW7425-3316: add NEXUS_GetVideoDecoderCapabilities
 * 
 * 90   11/2/11 4:20p erickson
 * SW7420-1992: move types to nexus_video_decoder_types.h
 * 
 * 89   10/24/11 1:38p erickson
 * SWDEPRECATED-2425: improve comments on picking fifoSize
 * 
 * 88   10/11/11 8:24a erickson
 * SW7420-1992: move types to nexus_video_decoder_types.h
 * 
 * 87   10/3/11 10:15a erickson
 * SW7425-1367: move pictureCoding to NEXUS_VideoDecoderStatus so we get
 *  fewer callbacks
 * 
 * 86   9/29/11 10:18a erickson
 * SW7425-1367: added NEXUS_VideoDecoderStreamInformation.pictureCoding
 * 
 * 85   9/9/11 1:11p erickson
 * SWDEPRECATED-2425: fix comments
 * 
 * 84   9/9/11 11:44a erickson
 * SWDTV-4865: clarify meaning of NEXUS_VideoDecoderScanMode
 * 
 * 83   8/18/11 6:18p vsilyaev
 * SW7425-1086, SW7425-966: added support for LeftRightEnhancedResolution
 * 
 * 82   8/8/11 11:29a erickson
 * SW7425-1066: NEXUS_VideoDecoderOpenSettings.heap only applies to per-
 *  channel picture buffer heap
 * 
 * 81   7/12/11 1:26p erickson
 * SW7420-1979: add shutdown attr to NEXUS_VideoDecoder_GetConnector
 * 
 * 80   6/10/11 11:10a vsilyaev
 * SW7425-654: Added support for non realtime video decode
 * 
 * 79   5/4/11 11:38a vsilyaev
 * SW7346-179, SW7425-416: Use different types for the video decoder and
 *  display orientation
 * 
 * 78   4/19/11 2:14p erickson
 * SW7335-1168: rework NEXUS_PidChannelScramblingSettings.raveContext to
 *  require it be already started by decoder
 * 
 * 77   3/17/11 1:07p erickson
 * SW7422-333: move supportedCodecs[] to NEXUS_VideoDecoderSettings. add
 *  NEXUS_VideoDecoderOpenSettings.enhancementPidChannelSupported.
 *
 * 76   3/7/11 11:03a jtna
 * SW7422-304: add 3D SVC decode support
 *
 * 75   3/1/11 10:21a erickson
 * SW7335-1168: add NEXUS_VideoDecoder_DetachRaveContext
 *
 * 74   2/22/11 10:22a erickson
 * SW7422-274: add NEXUS_VideoDecoderOpenSettings.supportedCodecs[]
 *
 * 73   12/27/10 4:23p erickson
 * SW7405-5051: add NEXUS_VideoDecoderSettings.horizontalOverscanMode
 *
 * 72   12/17/10 4:59p vsilyaev
 * SW7422-154: Added custom sourced 3D configuration
 *
 * 71   12/14/10 3:25p erickson
 * SW7420-1285: add null_allowed attribute
 *
 * 70   12/10/10 5:06p erickson
 * SW35230-2209: add NEXUS_VideoDecoderSettings.scanMode
 *
 * 69   12/6/10 3:39p erickson
 * SW3556-1213: added enums for still/moving content interpolation modes
 *
 * 68   11/1/10 5:23p vsilyaev
 * SW7422-65: Added pidChannel for enhancement video data
 *
 * 67   10/13/10 12:09p erickson
 * SWDEPRECATED-2425: update
 *
 * 66   10/13/10 10:33a erickson
 * SW7420-1135: move types to nexus_video_decoder_types.h
 *
 * 65   9/29/10 12:42p erickson
 * SW7550-586: add default values for NEXUS_MatrixCoefficients,
 *  NEXUS_ColorPrimaries, and NEXUS_TransferCharacteristics
 *
 * 64   6/16/10 10:22a erickson
 * SWDEPRECATED-2425: improved comments on
 *  NEXUS_VideoDecoderStatus.avdStatusBlock
 *
 * 63   6/16/10 9:49a erickson
 * SW7405-4378: correct comments on NEXUS_VideoDecoderStreamInformation.
 *  it only returns data from the head of the queue on the first picture
 *  decoded. after first TSM passed, it returns information about the same
 *  picture as NEXUS_VideoDecoderStatus
 *
 * 62   6/3/10 4:14p erickson
 * SW7405-4378: added
 *  NEXUS_VideoDecoderStreamInformation.codedSourceHorizontalSize and
 *  .codedSourceVerticalSize
 *
 * 61   4/26/10 3:10p erickson
 * SW3556-1090: added
 *  NEXUS_VideoDecoderExtendedSettings.zeroDelayOutputMode
 *
 * 60   4/19/10 11:23a erickson
 * SW35230-178: add codec profile and level information to
 *  NEXUS_VideoDecoderStatus
 *
 * 59   2/11/10 7:36p vsilyaev
 * SW3556-980: Updated names and description for
 *  NEXUS_VideoDecoderStartSettings.timestampMode
 *
 * 58   2/3/10 4:19p erickson
 * SWDEPRECATED-2425: add comment regarding AddInput, Start order
 *  requirement
 *
 * 57   2/2/10 3:41p erickson
 * SWDEPRECATED-2425: update comments on NEXUS_VideoDecoderErrorHandling
 *
 * 56   1/12/10 2:38p erickson
 * SW3556-980: added NEXUS_VideoDecoderStartSettings.timestampMode
 *
 * 55   12/30/09 5:17p erickson
 * SW7405-3276: add NEXUS_VideoDecoderSettings.manualPowerState and
 *  NEXUS_VideoDecoder_SetPowerState
 *
 * 54   12/7/09 5:30p erickson
 * SW3548-2645: add
 *  NEXUS_VideoDecoderStartSettings.progressiveOverrideMode
 *
 * 53   11/12/09 11:11a erickson
 * SW3556-836: added
 *  NEXUS_VideoDecoderStreamInformation.fixedFrameRateFlag
 *
 * 52   11/9/09 4:57p erickson
 * SW3556-836: add NEXUS_VideoDecoderStreamInformation.lowDelayFlag
 *
 * 51   11/2/09 11:15a erickson
 * SW7405-3315: remove playbackOverride. use NEXUS_StcChannelMode instead.
 *
 * 50   10/15/09 12:57p erickson
 * SW7405-3005: add NEXUS_VideoDecoderStatus.numDecodeOverflows
 *
 * 49   10/6/09 11:06a erickson
 * SW7405-3127: added NEXUS_VideoDecoderOpenSettings.itbFifoSize
 *
 * 48   8/14/09 4:45p erickson
 * PR57700: add NEXUS_VideoDecoderSettings.discardThreshold
 *
 * 47   7/30/09 5:13p erickson
 * PR57211: added NEXUS_VideoDecoderOpenSettings.excessDirModeEnabled
 *
 * 46   7/23/09 12:22p erickson
 * PR56967: added NEXUS_VideoDecoderSettings.decodeError callback
 *
 * 45   6/8/09 7:07a erickson
 * PR55617: add NEXUS_VideoDecoderStartSettings.aspectRatio
 *
 * 44   6/2/09 12:15p erickson
 * PR55639: improve accuracy of VideoDecoder status and stream information
 *  by adjusting timing and field selection
 *
 * 43   5/28/09 10:54a erickson
 * PR55483: clarify usage of NEXUS_VideoDecoderOpenSettings.heap
 *
 * 42   5/22/09 5:21p vsilyaev
 * PR 55376 PR 52344: Added option to enable processing of AVC(H.264)
 *  Level 5.1 video
 *
 * 41   5/20/09 3:30p erickson
 * PR55042: refactor active format description support. add callback and 8
 *  bit status value.
 *
 * 40   5/15/09 2:06p erickson
 * PR55094: add NEXUS_VideoDecoderStatus.numWatchdogs
 *
 * 39   3/16/09 3:32p erickson
 * PR53229: added NEXUS_VideoDecoderStatus.cabacBinDepth
 *
 * 38   2/23/09 2:56p erickson
 * PR52404: added NEXUS_VideoDecoder_IsCodecSupported
 *
 * 37   2/13/09 12:42a erickson
 * PR51134: added NEXUS_VideoDecoderSettings.dropFieldMode
 *
 * 36   1/29/09 5:27p erickson
 * PR51694: added NEXUS_VideoDecoderStatus.ptsStcDifference
 *
 * 35   1/8/09 9:37p erickson
 * PR50757: added NEXUS_VideoDecoderStartSettings.frameRate for default
 *  setting
 *
 * 34   12/12/08 10:57a erickson
 * PR49924: add NEXUS_VideoDecoderSettings.preferredUserDataFormat
 *
 * 33   12/9/08 6:22p erickson
 * PR49923: added firstPtsPassed callback
 *
 * 32   12/8/08 12:21p erickson
 * PR49500: remove interal AVD status bits
 *
 * 31   12/8/08 11:15a erickson
 * PR49930: add NEXUS_VideoDecoder_SetStartPts
 *
 * 30   12/3/08 12:26p erickson
 * PR35457: improve comments
 *
 * 29   11/26/08 5:22p erickson
 * PR49500: add NEXUS_VideoDecoderStatus.avdStatusBlock
 *
 * 28   11/26/08 12:39p erickson
 * PR49361: added NEXUS_VideoDecoderStatus.numPicturesReceived
 *
 * 27   11/24/08 12:09p erickson
 * PR35457: update NEXUS_AspectRatio_eSar comments
 *
 * 26   11/4/08 4:23p erickson
 * PR47030: add NEXUS_VideoDecoder_GetOpenSettings
 *
 * 25   10/30/08 8:53p erickson
 * PR47030: use NEXUS_HeapHandle for shared analog/digital heap
 *
 * 24   10/8/08 12:51a erickson
 * PR47666: add firstPts callback
 *
 * 23   9/23/08 9:36a erickson
 * PR47231: added NEXUS_VideoDecoderStartSettings.prerollRate
 *
 * 22   9/2/08 1:51p erickson
 * PR45940: added NEXUS_VideoDecoderSettings.fifoEmpty and
 *  NEXUS_VideoDecoderStatus.tsm
 *
 * 21   8/28/08 11:09a erickson
 * PR45875: add NEXUS_VideoDecoderErrorHandling
 *
 * 20   7/21/08 1:59p erickson
 * PR40222: change ptsOffset to signed int
 *
 * 19   7/17/08 5:24p jgarrett
 * PR 42642: Adding fifoThreshold
 *
 * 18   7/14/08 12:06p erickson
 * PR42739: added playbackOverride option for IPTV. fixed
 *  additionalPtsOffset logic.
 *
 * 17   5/9/08 3:04p erickson
 * PR42128: provide extended status information
 *
 * 16   4/28/08 4:10p erickson
 * PR41533: unify EIA-608 and EIA-708 closed caption data capture in
 *  NEXUS_VideoInput_ReadClosedCaption
 *
 * 15   4/18/08 1:53p erickson
 * PR41533: impl EIA-708 parsing and capture
 *
 * 14   4/17/08 4:38p erickson
 * PR41533: rework NEXUS_VideoDecoder_GetUserDataBuffer to use
 *  NEXUS_UserDataHeader before payload
 *
 * 13   3/25/08 12:54p erickson
 * PR40850: add more status
 *
 * 12   3/24/08 11:28a erickson
 * PR39000: added NEXUS_VideoDecoder_FlushUserdata. added
 *  NEXUS_VideoDecoderSettings.userDataEnabled.
 *
 * 11   3/11/08 2:11p erickson
 * PR40222: add ptsOffset
 *
 * 10   2/27/08 3:33p erickson
 * PR38888: complete VideoDecoder status
 *
 * 9   2/26/08 11:40p erickson
 * PR38888: complete NEXUS_VideoDecoderStreamInformation
 *
 * 8   2/7/08 4:41p erickson
 * PR34926: added userDataBufferSize open settings, convert to BMEM_Alloc
 *  for kernel mode support
 *
 * 7   2/6/08 12:42p vsilyaev
 * PR 38682: Added attributes for the linux/kernel proxy mode
 *
 * 6   1/25/08 3:32p erickson
 * PR38882: use NEXUS_VideoDecoderSettings.fifoSize if non-zero
 *
 * 5   1/23/08 9:22p vobadm
 * PR35457: update docs
 *
 * 4   1/23/08 2:39p erickson
 * PR35457: update docs
 *
 * 3   1/22/08 5:07p erickson
 * PR38888: added NEXUS_VideoDecoder_GetStreamInformation
 *
 * 2   1/22/08 3:45p erickson
 * PR38888: added streamCallback for sequence header notification
 *
 * 1   1/18/08 2:16p jgarrett
 * PR 38808: Merging to main branch
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_DECODER_H__
#define NEXUS_VIDEO_DECODER_H__

#include "nexus_types.h"
#include "nexus_video_types.h"
#include "nexus_video_decoder_types.h"
#include "nexus_video_decoder_userdata.h"
#include "nexus_rave.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=*******************************************
VideoDecoder is able to decode compressed digital streams from transport.
It outputs the decoded pictures to one or more Display VideoWindows.

VideoDecoder is also able to extract "userdata" from digital streams. If the decoder
is connected to a VideoWindow, this userdata will be sent directly to that window for
VBI encoding. VBI encoding control is done at the window.
Userdata can also be captured for processing by the application using VideoDecoder api's.
**********************************************/

/*
Summary:
Settings for opening a new VideoDecoder.
*/
typedef struct NEXUS_VideoDecoderOpenSettings
{
    unsigned fifoSize;                /* Size of compressed data buffer in bytes. If 0, it will pick a value based on codec-capabilities. 
                                         The minimum size depends on the maximum bitrate of your streams. 
                                         For live, it also depends on the PCR/PTS difference. The CDB must hold all the data until the PCR arrives for a given PTS.
                                         For playback, it also depends on the video/audio muxing difference. The CDB must hold all the data until the audio's PTS matures. 
                                         If the size chosen is too small, you will have TSM errors and stuttering on the screen. */
    unsigned userDataBufferSize;      /* Size of userdata buffer in bytes. Increase this for high bitrate userdata applications. */
    NEXUS_HeapHandle heap;            /* Optional picture buffer heap. This overrides the NEXUS_VideoDecoderModuleSettings.avdHeapIndex setting.
                                         Used by AVD and Display. This is used for systems with a shared analog/digital heap.
                                         If AVD and Display use different heaps (e.g. non-UMA mode), then this must be NULL. */
    bool avc51Enabled;                /* Enable AVC 5.1 mode */
    bool svc3dSupported;              /* Enable support for 3D SVC mode */
    bool excessDirModeEnabled;        /* Enable excess direct memory mode for this decoder instance */

    unsigned itbFifoSize;             /* Size of ITB in bytes. If 0, it will pick a value in proportion to fifoSize based on typical codec requirements. */

    bool enhancementPidChannelSupported;
} NEXUS_VideoDecoderOpenSettings;

/*
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
*/
void NEXUS_VideoDecoder_GetDefaultOpenSettings(
    NEXUS_VideoDecoderOpenSettings *pOpenSettings   /* [out] default settings */
    );

/*
Summary:
Open a new VideoDecoder.

Description:
Each VideoDecoder instance is able to decode a single program.

See Also:
NEXUS_VideoDecoder_GetDefaultOpenSettings
NEXUS_VideoDecoder_Close
NEXUS_VideoDecoderModule_Init - must init VideoDecoder module before calling this function
NEXUS_VideoDecoder_OpenMosaic
*/
NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_Open( /* attr{destructor=NEXUS_VideoDecoder_Close}  */
    unsigned index,
    const NEXUS_VideoDecoderOpenSettings *pOpenSettings /* attr{null_allowed=y} */
    );

/*
Summary:
Close a VideoDecoder.
*/
void NEXUS_VideoDecoder_Close(
    NEXUS_VideoDecoderHandle handle
    );

/*
Summary:
Get open settings
*/
void NEXUS_VideoDecoder_GetOpenSettings(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoderOpenSettings *pOpenSettings   /* [out] */
    );

/*
Summary:
Get the current NEXUS_VideoDecoderSettings from the decoder.
*/
void NEXUS_VideoDecoder_GetSettings(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoderSettings *pSettings   /* [out] */
    );

/*
Summary:
Set new NEXUS_VideoDecoderSettings to the decoder.
*/
NEXUS_Error NEXUS_VideoDecoder_SetSettings(
    NEXUS_VideoDecoderHandle handle,
    const NEXUS_VideoDecoderSettings *pSettings
    );

/*
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
The pidChannel must always be specified by the user.
*/
void NEXUS_VideoDecoder_GetDefaultStartSettings(
    NEXUS_VideoDecoderStartSettings *pSettings /* [out] */
    );

/**
Summary:
Start decoding a program

Description:

See Also:
NEXUS_VideoDecoder_GetDefaultStartSettings
**/
NEXUS_Error NEXUS_VideoDecoder_Start(
    NEXUS_VideoDecoderHandle handle,
    const NEXUS_VideoDecoderStartSettings *pSettings /* the video program to be decoded */
    );

/**
Summary:
Stop decoding a program
**/
void NEXUS_VideoDecoder_Stop(
    NEXUS_VideoDecoderHandle handle
    );

/**
Summary:
Immediately discard all pictures in the decoder's fifo, including pictures which have been
decoded and those waiting to be decoded.

Description:
The currently displayed picture will continue being displayed.
**/
void NEXUS_VideoDecoder_Flush(
    NEXUS_VideoDecoderHandle handle
    );

/*
Summary:
Get current status information from the video decoder.
*/
NEXUS_Error NEXUS_VideoDecoder_GetStatus(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoderStatus *pStatus   /* [out] */
    );

/*
Summary:
Returns the abstract NEXUS_VideoInput connector for the VideoDecoder.
This connector is passed to a VideoWindow to display the decoded video.

Description:
The video connector is typically passed into NEXUS_VideoWindow_AddInput to show video.

Nexus requires that NEXUS_VideoWindow_AddInput be done before NEXUS_VideoDecoder_Start. 
AddInput gives the VEC vsync interrupt heartbeat to the AVD HW and also initializes the underlying state machine.
Mirroring this order, NEXUS_VideoDecoder_Stop should be called before NEXUS_VideoWindow_RemoveInput. 
However, if NEXUS_VideoWindow_RemoveInput is called first, Stop will be called automatically.

Setting NEXUS_VideoDecoderSettings.manualPowerState = true allows you to disconnect from the display without stopping and restarting the decoder.
You must still AddInput before Start, but RemoveInput will not be automatically called on Stop.
The main use case is doing RemoveInput/AddInput swaps for Main and PIP without stopping and restarting the decoder. 
See nexus/examples/video/digital_pip_swap.c for an example.
*/
NEXUS_VideoInputHandle NEXUS_VideoDecoder_GetConnector( /* attr{shutdown=NEXUS_VideoInput_Shutdown} */
    NEXUS_VideoDecoderHandle handle
    );

/**
Summary:
Picture information for the currently displayed picture or the first decoded picture for a new decode.

Description:
NEXUS_VideoDecoderStreamInformation picture information differs from NEXUS_VideoDecoderStatus information in that
NEXUS_VideoDecoderStatus returns information on the currently displayed picture. When starting a new decode,
this could be the last picture from the previous decode.
NEXUS_VideoDecoderStreamInformation always gives picture information from this decode.
When starting decode, after the first picture is decoded, it will be held until its PTS matures.
NEXUS_VideoDecoderStreamInformation will have information about that first picture, before it is displayed.
After the first picture is displayed, NEXUS_VideoDecoderStreamInformation will return the same
picture information as NEXUS_VideoDecoderStatus.

Because this information is available before display, it is similar to the old MPEG sequence header interrupt.
However, after that first picture is displayed, it does not provided any advance warning like a sequence header interrupt.

Any changes in this information will generate a NEXUS_VideoDecoderSettings.streamChanged callback.

Used in NEXUS_VideoDecoder_GetStreamInformation
**/
typedef struct NEXUS_VideoDecoderStreamInformation
{
    bool valid; /* if false, there is no picture so ignore all data */
    
    uint32_t sourceHorizontalSize; /* width of active area. possibly clipped from coded size. */
    uint32_t sourceVerticalSize; /* height of active area. possibly clipped from coded size. */

    uint32_t codedSourceHorizontalSize; /* coded width */
    uint32_t codedSourceVerticalSize; /* coded height */

    uint32_t displayHorizontalSize; /* intended display width of active area needed to preserve correct aspect ratio. this is not the actual display width used. */
    uint32_t displayVerticalSize; /* intended display height of active area needed to preserve correct aspect ratio. this is not the actual display height used. */

    NEXUS_AspectRatio aspectRatio;
    uint16_t sampleAspectRatioX; /* Only valid if aspectRatio is NEXUS_AspectRatio_eSar. NEXUS_AspectRatio_eSar is only used for some codecs.
                                    If used, the source's aspect ratio is calculated as the ratio of sampleAspectRatioX/sampleAspectRatioY.
                                    sampleAspectRatioX and sampleAspectRatioY should not be used except as a ratio. */
    uint16_t sampleAspectRatioY; /* See comments for sampleAspectRatioX */
    NEXUS_VideoFrameRate frameRate;
    NEXUS_ColorPrimaries colorPrimaries;
    NEXUS_TransferCharacteristics transferCharacteristics;
    NEXUS_MatrixCoefficients matrixCoefficients;
    bool frameProgressive;
    bool streamProgressive;
    int32_t horizontalPanScan;
    int32_t verticalPanScan;

    uint32_t lowDelayFlag;       /* delay flag from the sequence extension */
    uint32_t fixedFrameRateFlag; /* fixed_frame_rate_flag from AVC VUI (video usability info) */
} NEXUS_VideoDecoderStreamInformation;

/**
Summary:
Get information about the last picture decoded.
**/
NEXUS_Error NEXUS_VideoDecoder_GetStreamInformation(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoderStreamInformation *pStreamInformation /* [out] */
    );

/**
Summary:
Set the PTS where decode should start. All pictures before that PTS will be discarded.

Description:
This is used to implement NEXUS_PlaybackSettings.accurateSeek.
**/
NEXUS_Error NEXUS_VideoDecoder_SetStartPts(
    NEXUS_VideoDecoderHandle handle,
    uint32_t pts
    );

/**
Summary:
Discover if a codec is supported.

Description:
Codecs may not be supported because of lack of hardware support, compile-time options, or
run-time options like module memory settings.
**/
void NEXUS_VideoDecoder_IsCodecSupported(
    NEXUS_VideoDecoderHandle videoDecoder,
    NEXUS_VideoCodec codec,
    bool *pSupported
    );

/*
Summary:
Set manual power state

Description:
If NEXUS_VideoDecoderSettings.manualPowerState is false (default), you do not need to call this function.
The default behavior is that NEXUS_VideoDecoder_SetPowerState(false) is automatically called on NEXUS_VideoWindow_RemoveInput and
NEXUS_VideoDecoder_SetPowerState(true) is called on NEXUS_VideoWindow_AddInput.
This results in lower power consumption.
However, powering down requires that the video decoder must be stopped and the current picture is lost.

One reason to set manualPowerState to false is to allow a video decoder to be swapped between two video windows
without having to stop decode or without having to lose the current picture.
See nexus/examples/video/digital_pip_swap.c for an example.

The following rules apply:
You cannot power down if currently connected to a video window.
You cannot power up unless you have connected to a video window at least once.

Because NEXUS_VideoDecoder_SetPowerState exposes low level control of internal state, the required order of operations is mutable.
It is possible that calling requirements may be tightened, loosened, or rearranged.
*/
NEXUS_Error NEXUS_VideoDecoder_SetPowerState(
    NEXUS_VideoDecoderHandle handle,
    bool powerUp
    );

/**
Summary:
Temporarily detach the video decoder's RAVE context for external use.

Description:
One use of the detached RAVE context is NEXUS_PidChannelScramblingSettings.raveContext.
**/
NEXUS_RaveHandle NEXUS_VideoDecoder_DetachRaveContext(
    NEXUS_VideoDecoderHandle handle
    );

/**
Summary:
Temporarily detach the video decoder's enhancement RAVE context for external use.

Description:
See NEXUS_VideoDecoder_DetachRaveContext for usage details.
**/
NEXUS_RaveHandle NEXUS_VideoDecoder_DetachEnhancementRaveContext(
    NEXUS_VideoDecoderHandle handle
    );

/**
Summary:
Re-attach a RAVE context that was detached with NEXUS_VideoDecoder_DetachRaveContext.

Description:
This function will fail if the RAVE context is not the one that was detached.
**/
NEXUS_Error NEXUS_VideoDecoder_AttachRaveContext(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_RaveHandle rave
    );

/**
Summary:
video decoder module capabilities
**/
typedef struct NEXUS_VideoDecoderCapabilities
{
    unsigned numVideoDecoders; /* total number of AVD-based decoders. base index is always 0. */
    struct {
        unsigned total;
        unsigned baseIndex;
        bool useForVp6;
    } dspVideoDecoder; /* DSP-based decoders */
} NEXUS_VideoDecoderCapabilities;

/**
Summary:
video decoder module capabilities
**/
void NEXUS_GetVideoDecoderCapabilities(
    NEXUS_VideoDecoderCapabilities *pCapabilities
    );

#ifdef __cplusplus
}
#endif

#endif
