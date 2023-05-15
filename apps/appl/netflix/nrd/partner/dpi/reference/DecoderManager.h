/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_DECODERMANAGER_H
#define DEVICE_DECODERMANAGER_H

/* @file DecoderManager.h - Class used in the reference application's
 * implementations of IElementaryStreamManager and IPlaybackGroup to handle
 * reference-implementation-specific initialization of the library that provides
 * the reference app with audio and video codec functionality and to handle
 * reference-implementation-specific instantiation of codec instances. The file does not
 * implement a DPI interface.
 */

//#include <nrdbase/tr1.h>
#include "VideoDecoder.h"
#include "AudioDecoder.h"
#include "DecoderLock.h"

namespace netflix {
namespace device {
namespace esplayer {


class ReferenceClockNative;


/**
 * @class DecoderManager DecoderManager.h
 * @brief Manager of audio/video decoders
 */
class DecoderManager
{
public:
    ~DecoderManager(){};

    /** decoder framework initialization. */
    void initFramework();

    /** decoder framework clean-up. */
    void quitFramework();

    /**
     * Create an audio decoder.
     *
     * @param[in] numOutputChannels the number of output channels of the audio renderer.
     *
     * @return pointer of the newly created audio decoder if succeeded; NULL otherwise.
     */
    shared_ptr<AudioDecoder> createAudioDecoder(uint32_t numOutputChannels);

    /**
     * Create a video decoder.
     *
     * @param[in] attributes the video attributes.
     * @param[in] rendererInfo information of the video renderer.
     *
     * @return pointer of the newly created video decoder if succeeded; NULL otherwise.
     */
    shared_ptr<VideoDecoder> createVideoDecoder(const VideoAttributes& attributes,
                                                ReferenceClockNative& referenceClock,
                                                const VideoRenderer::VideoRendererInfo& rendererInfo,
                                                uint32_t pipelineId);
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_DECODERMANAGER_H
