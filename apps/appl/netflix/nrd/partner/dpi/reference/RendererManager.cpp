
/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <nrd/AppLog.h>
#include <nrdbase/UnionCast.h>
#include <nrdbase/ScopedMutex.h>

#include "RendererManager.h"
#include "VideoRenderer.h"
#include "AudioRenderer.h"
#include "DeviceLibNative.h"
#include "MediaUtils.h"

#if defined(BUILD_SINK_AUDIO_RENDERER)
#  include "sink/AudioRendererSink.h"
#endif
#if defined(BUILD_SINK_VIDEO_RENDERER)  || defined(BUILD_OPENMAXIL_VIDEO_RENDERER)
#  include "sink/VideoRendererSink.h"
#endif
#if defined(BUILD_OPENMAXIL_VIDEO_RENDERER)
#  include "openmax-il/AudioRendererOMXIL.h"
#endif
#if defined(BUILD_PORTAUDIO_AUDIO_RENDERER)
#  include "x86/AudioRendererPA.h"
#endif
#if defined(BUILD_DIRECTFB_VIDEO_RENDERER)
#  include "x86/VideoRendererDFB.h"
#endif
#if defined(BUILD_SDL_VIDEO_RENDERER)
# include "x86/VideoRendererSDL.h"
#endif
#if defined(BUILD_OPENGL_VIDEO_RENDERER)
#  include "x86/VideoRendererOpenGL.h"
#endif

using namespace netflix::device::esplayer;
using namespace netflix;

RendererManager::~RendererManager()
{

}

shared_ptr<AudioRenderer>
RendererManager::createAudioRenderer(ReferenceClockNative& clock,
                                     AudioESPlayerNative& ESPlayer,
                                     uint32_t samplemPerSecond,
                                     uint32_t outputBufferSamples,
                                     uint32_t maxFrameSamples,
                                     uint32_t maxNumMaxFrames,
                                     uint32_t timestampQueueSize,
                                     uint32_t preferredOutChnls)
{
    shared_ptr<AudioRenderer> audioRenderer;

#if defined(BUILD_SINK_AUDIO_RENDERER)
    audioRenderer.reset(new AudioRendererSink(clock,
                                               ESPlayer,
                                               samplemPerSecond,
                                               outputBufferSamples,
                                               maxFrameSamples,
                                               maxNumMaxFrames,
                                               timestampQueueSize,
                                               preferredOutChnls));
#elif defined(BUILD_OPENMAXIL_AUDIO_RENDERER)
    audioRenderer.reset(new AudioRendererOMXIL(clock,
                                               ESPlayer,
                                               samplemPerSecond,
                                               outputBufferSamples,
                                               maxFrameSamples,
                                               maxNumMaxFrames,
                                               timestampQueueSize,
                                               preferredOutChnls));
#elif defined(BUILD_PORTAUDIO_AUDIO_RENDERER)
    audioRenderer.reset(new AudioRendererPA(clock,
                                             ESPlayer,
                                             samplemPerSecond,
                                             outputBufferSamples,
                                             maxFrameSamples,
                                             maxNumMaxFrames,
                                             timestampQueueSize,
                                             preferredOutChnls));
#else
#   error "Unspecified audio renderer!"
#endif
    return audioRenderer;
}

shared_ptr<VideoRenderer> RendererManager::createVideoRenderer(const VideoAttributes& attr,
                                                               uint32_t maxImageWidth,
                                                               uint32_t maxImageHeight,
                                                               uint32_t maxFrameRateValue,
                                                               uint32_t maxFrameRateScale,
                                                               uint32_t pipelineId)
{
    NRDP_UNUSED(pipelineId);
    NRDP_UNUSED(maxFrameRateValue);
    NRDP_UNUSED(maxFrameRateScale);
    NRDP_UNUSED(maxImageWidth);
    NRDP_UNUSED(maxImageHeight);

    shared_ptr<VideoRenderer> videoRenderer;
    NTRACE(TRACE_DPI, "Max frameRate of movie = %d / %d\n", maxFrameRateValue, maxFrameRateScale);
#if defined(BUILD_SINK_VIDEO_RENDERER) || defined(BUILD_OPENMAXIL_VIDEO_RENDERER)
    videoRenderer.reset(new VideoRendererSink(attr, maxImageWidth, maxImageHeight));
#elif defined(BUILD_OPENGL_VIDEO_RENDERER)
    videoRenderer.reset(new VideoRendererOpenGL(attr, maxImageWidth, maxImageHeight));
#elif defined(BUILD_DIRECTFB_VIDEO_RENDERER)
    videoRenderer.reset(new VideoRendererDFB(attr,
                                             ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH,
                                             ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT,
                                             pipelineId));
#else
#   error "Unspecified video renderer!"
#endif
    if(!videoRenderer->init())
        videoRenderer.reset();
    return videoRenderer;
}
