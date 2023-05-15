/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEODECODER_H
#define DEVICE_VIDEODECODER_H

/* @file VideoDecoder.h - Video decoder base class used in the reference
 * application's video elementary stream player implementation. The file does
 * not implement a DPI interface. It provides reference-implementation-specific
 * audio decoding functionality.
 */
#include "ESPlayerConstants.h"
#include "VideoESPlayerNative.h"
#include "VideoRenderer.h"
#include <list>
#include <nrdbase/Mutex.h>
#include "DecoderStatsCounter.h"

namespace netflix {
namespace device {
namespace esplayer {

struct AccessUnitAttributes;



/**
 * @class VideoDecoder VideoDecoder.h
 * @brief Base implementation for video decoder engines.
 */
class VideoDecoder
{
public:
    /**
     * Constructor.
     *
     * @param[in] rendererInfo information of the video renderer.
     */
    VideoDecoder(const VideoRenderer::VideoRendererInfo& rendererInfo, uint32_t pipelineId);

    /**
     * Destructor.
     */
    virtual ~VideoDecoder();

    /**
     * Decode the input access-unit and output the decoded picture into the frame buffer.
     * (the decoder may or may not output a picture)
     *
     * @param[in] auData pointer to the access-unit data.
     * @param[in] auDataSize number of bytes of the access-unit data.
     * @param[in] accessUnitAttributes the attributes associated with the access-unit.
     * @param[out] frameBuffer the output frame buffer.
     * @param[out] gotPicture true if a picture is decoded into the frame buffer, false if
     *                        none decoded (which may be normal at the beginning of decoding)
     * @param[out] resend true if the input data should be resent again for decoding.
     *
     * @return true if successful, false if there is some decoding problem.
     */

    virtual RefAppDeviceSpecificError decode(const ISampleAttributes *pSampleAttr,
                                             const unsigned char* auData, uint32_t auDataSize,
                                             VideoRenderer::VideoFrameBuffer& frameBuffer,
                                             bool& gotPicture,
                                             bool& resend,
                                             Format3D& format3D) = 0;

    /** Reset the decoder. */
    virtual void reset() = 0;

    /**
     * Set the state of flush operation (inactive/has-begun/has-ended).
     *
     * @param[in] flushState the new flush operation state to be set.
     */
    void setFlushState(ESPlayerNative::FlushState flushState);

    /**
     * Get the state of flush operation (inactive/has-begun/has-ended).
     *
     * @return the current flush operation state.
     */
    ESPlayerNative::FlushState getFlushState();

    /**
     * Set or reset the state of input data exhaustion.
     *
     * @param[in] inputExhausted true to set the exhaustion state; false to reset.
     */
    void setInputExhausted(bool inputExhausted);

    /**
     * Check whether the input data is exhausted.
     *
     * @return true if the input data is exhausted; false otherwise.
     */
    bool isInputExhausted();

    DecoderStatsCounter& getStatsCounter()
    {
        return statsCounter_;
    }

protected:
    /** Information of the video renderer */
    const VideoRenderer::VideoRendererInfo rendererInfo_;

    DecoderStatsCounter statsCounter_;

    uint32_t mPipelineId;

    /**
     * Enqueue a new timestamp (from access-unit) into the reordering queue.
     * If the reordering queue is full, the oldest item in queue will be overwritten.
     *
     * @param[in] timestamp the timestamp to be enqueued.
     */
    void enqueueTimestamp(int64_t timestamp);

    /**
     * Dequeue the oldest timestamp from the reordering queue, and return its value.
     * If the reordering queue is empty, INVALID_TIMESTAMP will be returned.
     *
     * @return the timestamp which is just removed from queue, or INVALID_TIMESTAMP.
     */
    int64_t dequeueTimestamp();

    /** Empty the timestamp reordering queue */
    void resetTimestampQueue();

    /**
     * Converts the H.264/H.265 elementary stream format from 14496-15 style to
     * 14496-10 Annex B style.
     */
    void convertToAnnexBAndCheckProperties(const unsigned char* auData, uint32_t auDataSize);

private:

// for timestamp reordering queue handling
    std::list<int64_t> timestampReorderQueue_;

    // flushState_ is accessed by an SDK thread and by the decoderTask thread.
    // Access to the variable must therefore be synchronized with the mutex.
    ESPlayerNative::FlushState flushState_;

    // inputExhausted is accessed by the decoderTask and rendererTask threads.
    // Access to the variable must therefore be synchronized with the mutex.
    bool         inputExhausted_;

    Mutex  mutex_;
};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_VIDEODECODER_H
