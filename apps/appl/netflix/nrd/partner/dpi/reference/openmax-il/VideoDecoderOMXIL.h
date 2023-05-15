/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEODECODEROMXIL_H
#define DEVICE_VIDEODECODEROMXIL_H

/* @file VideoDecoderOMXIL.h - Video decoder class used in the reference
 * application's video elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * video decoding functionality.
 */

#include "../VideoDecoder.h"
#include "ComponentOMXIL.h"
#include <vector>

namespace netflix {
namespace device {
namespace esplayer {

class ReferenceClockNative;
class VideoRenderer;

/**
 * @class VideoDecoderOMXIL VideoDecoderOMXIL.h
 * @brief Wrapper class of omxil video decoders
 */
class VideoDecoderOMXIL : public VideoDecoder, private ComponentOMXIL
{
public:
    static shared_ptr<VideoDecoderOMXIL> create(const VideoAttributes& attributes,
                                               ReferenceClockNative& referenceClock,
                                               const VideoRenderer::VideoRendererInfo& rendererInfo)
    {
        bool error;
        shared_ptr<VideoDecoderOMXIL> decoder;
        decoder.reset(new VideoDecoderOMXIL(attributes, referenceClock, rendererInfo, error));
        return decoder;
    }

    virtual ~VideoDecoderOMXIL();

    virtual RefAppDeviceSpecificError decode(const ISampleAttributes *pSampleAttr,
                                             const unsigned char* auData, uint32_t auDataSize,
                                             VideoRenderer::VideoFrameBuffer& frameBuffer,
                                             bool& gotPicture,
                                             bool& resend,
                                             Format3D& format3D);

    virtual void reset();

protected:
    VideoDecoderOMXIL(const VideoAttributes& attributes,
                      ReferenceClockNative& referenceClock,
                      const VideoRenderer::VideoRendererInfo& rendererInfo,
                      bool& error);

    virtual OMX_ERRORTYPE eventHandler(OMXEvent);

    bool checkIfShouldDropFrame(const int64_t &PTS, const int64_t &currPresentationTime) const;
    bool checkIfShouldDropUntilNextGOP(const ISampleAttributes* pSampleAttr, const int64_t& currPresentationTime) const;

    unsigned mFrames;
    uint32_t mDroppedFrames;
    mutable bool mDropFrames;
    unsigned int mFrameWidth, mFrameHeight;
    ReferenceClockNative &mReferenceClock;
#if defined(BUILD_OPENMAXIL_VIDEO_RENDERER)
    static OMX_ERRORTYPE omxil_glRendererEventHandler(OMX_HANDLETYPE nType, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
    ComponentOMXIL mRenderer, mScheduler;
#endif
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_VIDEODECODEROMXIL_H
