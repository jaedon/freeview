/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ESPLAYERCONSTANTS_H
#define ESPLAYERCONSTANTS_H

/* @file ESPlayerConstants.h - Contains constants specific to the reference
 * implementation of the IElementaryStreamPlayer interface.  A device
 * implementation may modify this header and accompanying .cpp files as
 * needed.
 */

#include <nrdbase/Thread.h>
#include <nrdbase/Time.h>
#include <nrddpi/Common.h>
#include <nrdbase/Exportable.h>

namespace netflix  {
namespace device   {
namespace esplayer {

class NRDP_EXPORTABLE ESPlayerConstants
{
public:
    // ES Player Settings
    static const Time WAIT_FOR_FLUSH_OFF;
    static const Time WAIT_FOR_FLUSH_END;

    // Audio Player Settings
    static const uint32_t AUDIO_REN_OUTPUT_BUF_SAMPLES;
    static const uint32_t AUDIO_REN_MAX_FRAME_SAMPLES;
    static const uint32_t AUDIO_REN_MAX_NUM_MAX_FRAMES;
    static const uint32_t AUDIO_REN_PTS_QUEUE_SIZE;
    static const int64_t  AUDIO_PLAYBACK_START_THRESHOLD_IN_MS;
    static const uint32_t AUDIO_OUTPUT_BITS; // 16 bits audio

    // Video Player Settings
    static uint32_t VIDEO_REN_SCREEN_WIDTH;
    static uint32_t VIDEO_REN_SCREEN_HEIGHT;
    static uint32_t VIDEO_REN_SCREEN_WIDTH_4K;
    static uint32_t VIDEO_REN_SCREEN_HEIGHT_4K;
    static uint32_t VIDEO_REN_SCREEN_PAR_X; // Screen Pixel Aspect Ratio
    static uint32_t VIDEO_REN_SCREEN_PAR_Y; // Screen Pixel Aspect Ratio

    static const uint32_t VIDEO_REN_FRAME_BUFFERS;
    static const uint32_t VIDEO_REN_MIN_FRAME_BUFFERS;
    static const int32_t  VIDEO_REN_PTS_TWEAK;
    static const int64_t  VIDEO_PLAYBACK_START_THRESHOLD_IN_MS;
    static const int64_t  VIDEO_DROP_THRESHOLD_DEC;
    static const int64_t  VIDEO_DROP_THRESHOLD_WHOLE_GOP;
    static const int64_t  VIDEO_DROP_THRESHOLD_REN;
    static const uint32_t MAX_CONSECUTIVE_VIDEO_DROP_DEC;
    static const uint32_t MAX_CONSECUTIVE_VIDEO_DROP_REN;
    static const uint32_t PIXEL_FORMAT_NONE;

    static const uint32_t DECODER_INPUT_EXHAUSTION_THRESHOLD;

    // Misc Settings
    static int32_t  CODEC_LOG_LEVEL;

    // Device Thread Priorities
    static const Thread::Priority AUDIO_DEC_PRIO;
    static const Thread::Priority AUDIO_DEV_PRIO;
    static const Thread::Priority VIDEO_DEC_PRIO;
    static const Thread::Priority VIDEO_DSP_PRIO;
};

/*
 * Partner devices should use device specific code that is meaningful for target
 * device.
 */
enum RefAppDeviceSpecificError
{
    refAppDeviceSpecific_NoError = 0,

    // related to drm
    refAppDeviceSpecific_UnknownPrectionSystemId = 1000,
    refAppDeviceSpecific_NoDecryptionContext,
    refAppDeviceSpecific_WrongSubsampleMapping,
    refAppDeviceSpecific_NoDrmSystemFound,
    refAppDeviceSpecific_NoDrmSessionFound,
    refAppDeviceSpecific_InvalidDrmSecureStopId,

    // related to decoding
    refAppDeviceSpecific_WrongBitDepth = 2000,
    refAppDeviceSpecific_MissingSampleAttributes,
    refAppDeviceSpecific_UpdateAttributesFailed,
    refAppDeviceSpecific_HevcDecoderError,
    refAppDeviceSpecific_FfmpegDecoderError,

    // related to player
    refAppDeviceSpecific_NoInitialStreamAttributes = 3000,
    refAppDeviceSpecific_NoInitialAudioStreamAttributes,
    refAppDeviceSpecific_NoInitialVideoStreamAttributes,
    refAppDeviceSpecific_VideoPlayerCreationFailed,
    refAppDeviceSpecific_VideoPlayerInitializationFailed,
    refAppDeviceSpecific_AudioPlayerCreationFailed,
    refAppDeviceSpecific_AudioPlayerInitializationFailed,
    refAppDeviceSpecific_Wrong3DMode,
    refAppDeviceSpecific_Wrong3DViewNumber,
    refAppDeviceSpecific_createPlaybackGroupFailed

};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif
