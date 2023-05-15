/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "ESPlayerConstants.h"

using namespace netflix;
using namespace netflix::device::esplayer;

// ES Player Settings
const Time ESPlayerConstants::WAIT_FOR_FLUSH_OFF(10);
const Time ESPlayerConstants::WAIT_FOR_FLUSH_END(10);

// Audio Player Settings
#if 0
const uint32_t ESPlayerConstants::AUDIO_REN_OUTPUT_BUF_SAMPLES = 1024;
const uint32_t ESPlayerConstants::AUDIO_REN_MAX_FRAME_SAMPLES  = 2400;
const uint32_t ESPlayerConstants::AUDIO_REN_MAX_NUM_MAX_FRAMES = 3;
#else
const uint32_t ESPlayerConstants::AUDIO_REN_OUTPUT_BUF_SAMPLES = 2048;
const uint32_t ESPlayerConstants::AUDIO_REN_MAX_FRAME_SAMPLES  = 96000; // 48000 x 2 (workaround for OGG Vorbis decoding)
const uint32_t ESPlayerConstants::AUDIO_REN_MAX_NUM_MAX_FRAMES = 4;
#endif
const uint32_t ESPlayerConstants::AUDIO_REN_PTS_QUEUE_SIZE     = 256;
const int64_t  ESPlayerConstants::AUDIO_PLAYBACK_START_THRESHOLD_IN_MS = 40; // 2 decoded "frames" where a
                                                                             // frame is a block of 1024 pcm samples
const uint32_t ESPlayerConstants::AUDIO_OUTPUT_BITS =         16; // 16 bits audio

// Video Player Settings
uint32_t ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH             = 720;
uint32_t ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT            = 480;
uint32_t ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH_4K          = 1920;
uint32_t ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT_4K         = 1080;
uint32_t ESPlayerConstants::VIDEO_REN_SCREEN_PAR_X             = 1; // Screen Pixel Aspect Ratio
uint32_t ESPlayerConstants::VIDEO_REN_SCREEN_PAR_Y             = 1; // Screen Pixel Aspect Ratio
const uint32_t ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS      = 8;
const uint32_t ESPlayerConstants::VIDEO_REN_MIN_FRAME_BUFFERS  = 2;
const int32_t  ESPlayerConstants::VIDEO_REN_PTS_TWEAK          = 0;
const int64_t  ESPlayerConstants::VIDEO_PLAYBACK_START_THRESHOLD_IN_MS =    47; // 2 decoded frames.
const int64_t  ESPlayerConstants::VIDEO_DROP_THRESHOLD_DEC       =   -33;
const int64_t  ESPlayerConstants::VIDEO_DROP_THRESHOLD_WHOLE_GOP =    1000;
const int64_t  ESPlayerConstants::VIDEO_DROP_THRESHOLD_REN       =    100;
const uint32_t ESPlayerConstants::MAX_CONSECUTIVE_VIDEO_DROP_DEC =    2;
const uint32_t ESPlayerConstants::MAX_CONSECUTIVE_VIDEO_DROP_REN =    1;
const uint32_t ESPlayerConstants::PIXEL_FORMAT_NONE =                 0;

const uint32_t ESPlayerConstants::DECODER_INPUT_EXHAUSTION_THRESHOLD = 5;

// Misc Settings
int32_t  ESPlayerConstants::CODEC_LOG_LEVEL              = 0;

// Device Thread Priorities
const Thread::Priority ESPlayerConstants::AUDIO_DEC_PRIO = 80;
const Thread::Priority ESPlayerConstants::AUDIO_DEV_PRIO = 80;
const Thread::Priority ESPlayerConstants::VIDEO_DEC_PRIO = 80;
const Thread::Priority ESPlayerConstants::VIDEO_DSP_PRIO = 80;

