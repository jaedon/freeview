/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DecoderManager.h"
#include "DecoderLock.h"
#include "MediaUtils.h"

#include <stdio.h>

#include <nrdbase/Log.h>

#if defined(BUILD_SINK_AUDIO_DECODER)
# include "sink/AudioDecoderSink.h"
#endif
#if defined(BUILD_SINK_VIDEO_DECODER)
# include "sink/VideoDecoderSink.h"
#endif
#if defined(BUILD_FFMPEG_AUDIO_DECODER)
# include "x86/AudioDecoderNative.h"
#endif
#if defined(BUILD_FFMPEG_VIDEO_DECODER)
extern "C"
{
#  include <libavutil/log.h>
#ifndef attribute_deprecated
#    define attribute_deprecated // prevent warning on deprecated stuff
#  endif
#  include <libavcodec/avcodec.h>
#  include <libswscale/swscale.h>
}
# include "x86/VideoDecoderNative.h"
#endif
#if defined(BUILD_VANGUARDHEVC_VIDEO_DECODER)
# include "x86/VideoDecoderHEVC.h"
# include "x86/VideoDecoderDolbyVision.h"
#endif
#if defined(BUILD_OPENMAXIL_VIDEO_DECODER)
#  include "openmax-il/VideoDecoderOMXIL.h"
#endif
#if defined(BUILD_OPENMAXIL_AUDIO_DECODER)
#  include "openmax-il/AudioDecoderOMXIL.h"
#endif

using namespace netflix::device;
using namespace netflix::device::esplayer;
using namespace netflix;

void DecoderManager::initFramework()
{
    DecoderLock::init();
}

void DecoderManager::quitFramework()
{
    DecoderLock::shutdown();
}

shared_ptr<AudioDecoder> DecoderManager::createAudioDecoder(uint32_t numOutputChannels)
{
    shared_ptr<AudioDecoder> audioDecoder;
#if defined(BUILD_SINK_AUDIO_DECODER)
    audioDecoder.reset(new AudioDecoderSink(numOutputChannels));
#elif defined(BUILD_OPENMAXIL_AUDIO_DECODER)
    audioDecoder.reset(new AudioDecoderOMXIL(numOutputChannels));
#elif defined(BUILD_FFMPEG_AUDIO_DECODER)
    audioDecoder.reset(new AudioDecoderNative(numOutputChannels));
#else
#   error "Unspecified audio decoder!"
#endif
    return audioDecoder;
}

shared_ptr<VideoDecoder> DecoderManager::createVideoDecoder(const VideoAttributes& attributes,
                                                            ReferenceClockNative& referenceClock,
                                                            const VideoRenderer::VideoRendererInfo& rendererInfo,
                                                            uint32_t pipelineId)
{
    NRDP_UNUSED(pipelineId);

    shared_ptr<VideoDecoder> videoDecoder;
#if defined(BUILD_SINK_VIDEO_DECODER)
    videoDecoder = VideoDecoderSink::create(attributes, referenceClock, rendererInfo);
#elif defined(BUILD_OPENMAXIL_VIDEO_DECODER)
    videoDecoder = VideoDecoderOMXIL::create(attributes, referenceClock, rendererInfo);
#elif defined(BUILD_VANGUARDHEVC_VIDEO_DECODER)
    const uint32_t codec_id = MediaUtils::getFourCCFromCodecParam(attributes.mCodecParam);
    if(codec_id == _FOURCC_LE_('h','v','c','1') ||
       codec_id == _FOURCC_LE_('h','e','v','1') ||
       codec_id == _FOURCC_LE_('d','v','h','e') )
#if 0
    {
        videoDecoder = VideoDecoderHEVC::create(attributes, referenceClock, rendererInfo, pipelineId);
    } else if(codec_id == _FOURCC_LE_('d','v','h','e') )
#endif
    {
        // This is actually VanguardDecoder. We don't have DolyVision decoder yet.
        // It seems that, VanguardHEVC decoder can not be used multiple places.
        // So, just use below for both hevc and dvhe for now
        videoDecoder = VideoDecoderDolbyVision::create(attributes, referenceClock, rendererInfo, pipelineId);
    } else {
        videoDecoder = VideoDecoderNative::create(attributes, referenceClock, rendererInfo, pipelineId);
    }
#elif defined(BUILD_FFMPEG_VIDEO_DECODER)
    videoDecoder = VideoDecoderNative::create(attributes, referenceClock, rendererInfo, pipelineId);
#else
#   error "Unspecified video decoder!"
#endif
    return videoDecoder;
}
