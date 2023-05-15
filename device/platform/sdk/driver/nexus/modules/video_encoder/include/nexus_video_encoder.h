/***************************************************************************
 *     (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_encoder.h $
 * $brcm_Revision: 20 $
 * $brcm_Date: 8/13/12 1:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_encoder/include/nexus_video_encoder.h $
 * 
 * 20   8/13/12 1:20p vsilyaev
 * SW7425-3707: Removed unused enum
 * 
 * 19   6/8/12 12:17p vsilyaev
 * SW7425-3156: Added ClearStatus API
 * 
 * 18   6/5/12 12:23p vsilyaev
 * SW7425-3167: Merged to mainline
 * 
 * SW7425-3167/1   6/5/12 11:47a nilesh
 * SW7425-3167: Removed trackInput and restartGopOnSceneChange from
 *  NEXUS_VideoEncoderStreamStructure
 * 
 * 17   2/9/12 11:10a vsilyaev
 * SW7425-2349: Added adaptiveLowDelayMode configuration
 * 
 * 16   1/26/12 12:14p vsilyaev
 * SW7425-2266: Added [optional] parameter to the NEXUS_VideoEncoder_Stop
 *  function
 * 
 * 15   12/16/11 5:29p erickson
 * SW7425-1878: merge XUDlib support for video encoder userdata
 * 
 * SW7425-1878/1   12/9/11 1:33p ajagmag
 * SW7425-1878: Integrate XUDlib into nexus
 * 
 * 14   12/9/11 3:53p vsilyaev
 * SW7425-1566: Added NEXUS_VideoEncoderType
 * 
 * 13   12/8/11 3:16p vsilyaev
 * SW7425-1746: Added NEXUS_VideoEncoder_InsertRandomAccessPoint
 * 
 * 12   8/17/11 1:07p vsilyaev
 * SW7425-651: Removed deprecated control over the encoder delay
 * 
 * 11   8/16/11 6:25p vsilyaev
 * SW7425-651: Added video encoder bounds to facilitate lower encoding
 *  delay
 * 
 * 10   7/7/11 12:49p erickson
 * SWDEPRECATED-2425: add comment
 * 
 * 9   4/18/11 1:04p vsilyaev
 * SW7425-171: Added NEXUS_VideoEncoderSettingsOnInputChange
 * 
 * 8   3/3/11 11:23a vsilyaev
 * SW7425-137: Removed target and min bitrates
 * 
 * SW7425-137/1   3/2/11 11:43p nilesh
 * SW7425-137: Removed uiMin and uiTarget bitrate settings
 * 
 * 7   2/23/11 10:40a vsilyaev
 * SW7425-102: Improved comments
 * 
 * 6   2/22/11 4:41p vsilyaev
 * SW7425-102: Added API to control the video encoder delay
 * 
 * 5   2/8/11 5:41p vsilyaev
 * SW7425-40: Updated profile/level configuration
 * 
 * 4   2/7/11 12:15p vsilyaev
 * SW7425-40: Added 'null_allowed' attributes
 * 
 * 3   2/7/11 9:41a vsilyaev
 * SW7425-40: Updated to match latest BVCE
 * 
 * 2   1/17/11 4:46p vsilyaev
 * SW7425-65: Added StcChannel to the videoEncoder
 * 
 * 1   12/9/10 8:11p vsilyaev
 * SW7425-40: Video Encoder module
 * 
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_ENCODER_H__
#define NEXUS_VIDEO_ENCODER_H__

#include "nexus_video_encoder_types.h"
#include "nexus_video_encoder_init.h"
#include "nexus_display.h"
#include "nexus_stc_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Options for video encoder configuration
**/
typedef enum NEXUS_VideoEncoderType
{
   NEXUS_VideoEncoderType_eMulti, /* Multiple instances allowed at a time
                                   * Combined Resolution of active encodes CANNOT exceed Max Resolution
                                   * No Low Delay Support: NEXUS_VideoEncoderStartSettings.lowDelayPipeline MUST BE false
                                   * Increased A2PDelay for ALL streams */
   NEXUS_VideoEncoderType_eSingle, /* Only single instance allowed at a time
                                    * Low Delay Supported: NEXUS_VideoEncoderStartSettings.lowDelayPipeline CAN be true
                                    * Max Resolution is supported */
   NEXUS_VideoEncoderType_eMultiNonRealTime,  /* Multiple non-realtime instances allowed at a time
                                               * NRT mode ONLY: NEXUS_VideoEncoderStartSettings.nonRealTime MUST BE true
                                               * Max Resolution is supported on EACH stream */

   NEXUS_VideoEncoderType_eMax
} NEXUS_VideoEncoderType;


/**
Summary:
Settings for opening a new VideoEncoder.
**/
typedef struct NEXUS_VideoEncoderOpenSettings
{
    struct {
        unsigned fifoSize;
    } data, index;
    NEXUS_HeapHandle system;
    NEXUS_HeapHandle secure;
    NEXUS_HeapHandle picture;
    NEXUS_VideoEncoder_MemoryConfig memoryConfig;
    NEXUS_CallbackDesc errorCallback;
    NEXUS_VideoEncoderType type;
} NEXUS_VideoEncoderOpenSettings;

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_VideoEncoder_GetDefaultOpenSettings(
    NEXUS_VideoEncoderOpenSettings *pSettings   /* [out] */
    );

/**
Summary:
Open a new VideoEncoder.

Description:
Each VideoEncoder instance is able to encode a single video stream.
**/
NEXUS_VideoEncoderHandle NEXUS_VideoEncoder_Open( /* attr{destructor=NEXUS_VideoEncoder_Close}  */
    unsigned index,
    const NEXUS_VideoEncoderOpenSettings *pSettings     /* attr{null_allowed=y} Pass NULL for default settings */
    );

/**
Summary:
Close a VideoEncoder.
**/
void NEXUS_VideoEncoder_Close(
    NEXUS_VideoEncoderHandle handle
    );

/**
Summary:
Expected properties of the video source and output


Description:
    If properites of the video source and output are known and specified, encoder latency can be improved.
   
See Also:
NEXUS_VideoEncoderStartSettings
NEXUS_VideoEncoder_GetDefaultStartSettings
NEXUS_VideoEncoder_Start
**/
typedef struct NEXUS_VideoEncoderBounds
{
     struct
     {
        NEXUS_VideoFrameRate    min;
        NEXUS_VideoFrameRate    max;
     } outputFrameRate; /* Output Frame Rate, NEXUS_VideoEncoderSettings.frameRate must be within specified range */
     /* Input Frame Rate -
      *    The input frame rate (from bvn) cannot go lower than eMin during the encode */
     struct
     {
        NEXUS_VideoFrameRate    min;
     } inputFrameRate; /* Output Frame Rate, input frame rate to the encoder cannot go lower */

     /* 
     *    The input picture dimensions (from bvn) cannot go above stMax */
     struct
     {
        struct
        {
           unsigned width; 
           unsigned height;
        } max;
     } inputDimension; /* Picture Dimension - input picture dimensions to the encoder cannot go above */
} NEXUS_VideoEncoderBounds;

/**
Summary:
Start-time settings.

Description:
See Also:
NEXUS_VideoEncoder_GetDefaultStartSettings
NEXUS_VideoEncoder_Start
**/
typedef struct NEXUS_VideoEncoderStartSettings
{
    NEXUS_DisplayHandle input;
    bool interlaced;
    bool nonRealTime;
    bool lowDelayPipeline;
    bool encodeUserData;
    bool adaptiveLowDelayMode;    /* If set  encoder will drop all incoming frames until the first decoded frame is seen.  
                                     The first frame will be encoded with a low delay.  Then delay
                                     will automatically ramp to the value specified in NEXUS_VideoEncoderSettings.encoderDelay
                                  */
    NEXUS_VideoCodec codec;
    NEXUS_VideoCodecProfile profile;
    NEXUS_VideoCodecLevel level;
    NEXUS_VideoEncoderBounds bounds; /* Encoder Bounds - If the bounds are known and specified, encoder latency can be improved.
                                        These bounds are for a single encode session.  E.g. if the output frame rate is known to to be
                                        fixed at 30fps, then the output frame rate min/max can be set to 30. */
    unsigned rateBufferDelay;       /* in ms.  Higher values indicate better quality but more latency.  0 indicates use encoder's defaults */

    NEXUS_StcChannelHandle stcChannel;  /* The StcChannel connects audio and video decode to provide lipsync (also called Time Stamp Managed mode, or TSM mode).
                                            In TSM mode, pictures are sent to Display only when the PTS matches the STC within a certain threshold.
                                            If NULL, the decoder performs no TSM and is in "vsync mode". A new pictures is sent to the display on every vsync interrupt.
                                            See NEXUS_StcChannel_Open. */
} NEXUS_VideoEncoderStartSettings;

/**
Summary:
Range for delay in the video encoder path
**/
typedef struct NEXUS_VideoEncoderDelayRange {
    unsigned min; /* min delay in 27MHz ticks */
    unsigned max; /* max delay in 27MHz ticks */
} NEXUS_VideoEncoderDelayRange;

/**
Summary:
Settings that could be changed synchronously with changes of the source
**/
typedef struct NEXUS_VideoEncoderSettingsOnInputChange
{
    unsigned bitrateMax; /* units of bits per second */
} NEXUS_VideoEncoderSettingsOnInputChange;

/**
Summary:
Options for video encoder stop
**/
typedef enum NEXUS_VideoEncoderStopMode
{
    NEXUS_VideoEncoderStopMode_eNormal, 
    NEXUS_VideoEncoderStopMode_eImmediate,
    NEXUS_VideoEncoderStopMode_eMax
} NEXUS_VideoEncoderStopMode;

/**
Summary:
Settings for stopping VideoEncoder.
**/
typedef struct NEXUS_VideoEncoderStopSettings
{
    NEXUS_VideoEncoderStopMode mode;
} NEXUS_VideoEncoderStopSettings;



/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new structure members in the future.
**/
void NEXUS_VideoEncoder_GetDefaultStartSettings(
    NEXUS_VideoEncoderStartSettings *pSettings /* [out] */
    );

/**
Summary:
Start encoding a stream
**/
NEXUS_Error NEXUS_VideoEncoder_Start(
    NEXUS_VideoEncoderHandle handle,
    const NEXUS_VideoEncoderStartSettings *pSettings
    );

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void NEXUS_VideoEncoder_GetDefaultStopSettings(
    NEXUS_VideoEncoderStopSettings *pSettings   /* [out] */
);



/**
Summary:
    Stop encoding a stream
**/
void NEXUS_VideoEncoder_Stop(
    NEXUS_VideoEncoderHandle handle,
    const NEXUS_VideoEncoderStopSettings *pSettings   /* attr{null_allowed=y} */
);


/**
Summary:
GOP Parameters 
**/
typedef struct NEXUS_VideoEncoderStreamStructure
{
     /* The following are only relevant if frameP != 0 */
      bool openGop;

      /* The following are only relevant if trackInput = false */
      unsigned framesP; /* number of P frames between I frames. 0xFFFFFFFF indicated IP infinite mode */

      /* The following are only relevant if framesB != 0xFFFFFFFF */
      unsigned framesB; /* number of B frames between I or P frames */
} NEXUS_VideoEncoderStreamStructure;

/**
Summary:
Run-time settings

Description:
Some encoding settings are made with other nexus API's in the video pipeline. For example,
you can make run-time resolution changes by calling NEXUS_Display_SetCustomFormatSettings on the 
encoder's display input.
**/
typedef struct NEXUS_VideoEncoderSettings
{
    unsigned bitrateMax; /* units of bits per second */
    bool variableFrameRate;
    bool enableFieldPairing;

    NEXUS_VideoFrameRate    frameRate;
    NEXUS_VideoEncoderStreamStructure streamStructure;
    unsigned encoderDelay; /* encoder delay, should be within NEXUS_VideoEncoderDelayRange.min .. NEXUS_VideoEncoderDelayRange.max range */
} NEXUS_VideoEncoderSettings;


/**
Summary:
Get the current NEXUS_VideoEncoderSettings from the decoder.
**/
void NEXUS_VideoEncoder_GetSettings(
    NEXUS_VideoEncoderHandle handle,
    NEXUS_VideoEncoderSettings *pSettings /* [out] */
    );
    
/**
Summary:
Set new NEXUS_VideoEncoderSettings to the decoder.
**/
NEXUS_Error NEXUS_VideoEncoder_SetSettings(
    NEXUS_VideoEncoderHandle handle,
    const NEXUS_VideoEncoderSettings *pSettings
    );
    
/**
Summary:
Returnns range of delays supported by the decoder
**/
NEXUS_Error NEXUS_VideoEncoder_GetDelayRange (
    NEXUS_VideoEncoderHandle handle,
    const NEXUS_VideoEncoderSettings *pSettings,
    const NEXUS_VideoEncoderStartSettings *pStartSettings,
    NEXUS_VideoEncoderDelayRange *pDelayRange
    );

/**
Summary:
Get default settings for the structure.

Description:
This is required in order to make application code resilient to the addition of new strucutre members in the future.
**/
void
NEXUS_VideoEncoder_GetSettingsOnInputChange(
    NEXUS_VideoEncoderHandle handle,
    NEXUS_VideoEncoderSettingsOnInputChange *pSettings
    );

/**
Summary:
Set new NEXUS_VideoEncoderSettingsOnInputChange to the decoder.

Description:
Sets the encode parameters to be used in sync with change to
the input to the encoder (e.g. resolution change)
**/
NEXUS_Error NEXUS_VideoEncoder_SetSettingsOnInputChange(
    NEXUS_VideoEncoderHandle handle,
    const NEXUS_VideoEncoderSettingsOnInputChange *pSettings
    );

/**
Summary:
Instructs video encoder to insert random acccess point

Description:q
Instructes the video encoder firmwarew to insert random access point (RAP) at earliest possibility
**/
NEXUS_Error NEXUS_VideoEncoder_InsertRandomAccessPoint(
    NEXUS_VideoEncoderHandle handle
    );

/**
Summary:
**/
typedef struct NEXUS_VideoEncoderClearStatus {
    uint32_t errorFlags;
    uint32_t eventFlags;
} NEXUS_VideoEncoderClearStatus;

/**
Summary: Clears error and events
**/
void NEXUS_VideoEncoder_ClearStatus(
    NEXUS_VideoEncoderHandle handle,
    const NEXUS_VideoEncoderClearStatus *pClearStatus /* attr{null_allowed=y} error and events bit to clear, if NULL, all bits would get cleared */
    );

/**
Summary: Clears error and events
**/
void NEXUS_VideoEncoder_GetDefaultClearStatus(
        NEXUS_VideoEncoderClearStatus *pClearStatus /* default to clear all bits */
     );




#ifdef __cplusplus
}
#endif


#endif /* NEXUS_VIDEO_ENCODER_H__ */


