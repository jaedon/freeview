/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "../VideoESPlayerNative.h"
#include "../MediaUtils.h"
#include "../VideoRenderer.h"
#include "../ESPlayerConstants.h"
#include "../ReferenceClockNative.h"

#include "VideoDecoderHEVCSink.h"

#include <nrdbase/Log.h>
#include <cstring>

#include <nrdbase/ScopedMutex.h>
#include <nrddpi/Common.h>
#include "../DecoderLock.h"

using namespace netflix::device::esplayer;
using namespace std;
using namespace netflix;

VideoDecoderHEVCSink::VideoDecoderHEVCSink(
    const VideoAttributes& attributes,
    ReferenceClockNative& referenceClock,
    const VideoRenderer::VideoRendererInfo& rendererInfo,
    bool& error)
    : VideoDecoder(rendererInfo),
      referenceClock_(referenceClock),
      videoDroppedFrameCountDec_(0),
      consecutiveDroppedFrameCount_(0),
      dropUntilNextGOP_(false),
      mAttributes(attributes),
      timestamp_(0)
{
    NRDP_UNUSED(error);
}

VideoDecoderHEVCSink::~VideoDecoderHEVCSink()
{
}

bool VideoDecoderHEVCSink::checkIfShouldDropFrame(const int64_t& PTS,
                                              const int64_t& currPresentationTime,
                                              const bool dropable) const
{
    bool dropFrame = false;

    if(dropable && PTS != INVALID_TIMESTAMP)
    {
        if(currPresentationTime != INVALID_TIMESTAMP &&
           currPresentationTime > PTS + ESPlayerConstants::VIDEO_DROP_THRESHOLD_DEC)
        {
            if(consecutiveDroppedFrameCount_ < ESPlayerConstants::MAX_CONSECUTIVE_VIDEO_DROP_DEC)
            {
                dropFrame = true;
            }
        }
    }
    return dropFrame;
}

 // Drop the rest of the GOP if the decoder is really far behind.
bool VideoDecoderHEVCSink::checkIfShouldDropUntilNextGOP(const ISampleAttributes *pSampleAttr,
                                                     const int64_t& currPresentationTime)
{
    if(pSampleAttr->isKeyFrame())
    {
        dropUntilNextGOP_ = false;
    }

    if(currPresentationTime != INVALID_TIMESTAMP &&
       currPresentationTime > pSampleAttr->getPTS() + ESPlayerConstants::VIDEO_DROP_THRESHOLD_WHOLE_GOP)
    {
        dropUntilNextGOP_ = true;
    }
    return dropUntilNextGOP_;
}

void VideoDecoderHEVCSink::resetCodecWithUpdatedExtraData(unsigned char*& new_extradata,
                                                      const int& new_extradata_size,
                                                      const int& new_width,
                                                      const int& new_height)
{
    NRDP_UNUSED(new_extradata);
    NRDP_UNUSED(new_extradata_size);
    NRDP_UNUSED(new_width);
    NRDP_UNUSED(new_height);
    resetTimestampQueue();
}

ESNativeError VideoDecoderHEVCSink::decode(const ISampleAttributes *pSampleAttr,
                                       const unsigned char* auData,
                                       uint32_t auDataSize,
                                       VideoRenderer::VideoFrameBuffer& frameBuffer,
                                       bool& gotPicture,
                                       bool& resend,
                                       Format3D& format3D)
{
    NRDP_UNUSED(auData);
    NRDP_UNUSED(auDataSize);
    NRDP_UNUSED(resend);
    NRDP_UNUSED(format3D);

    bool dropFrame = false;
    bool dropable = false;
    int64_t currPresentationTime = referenceClock_.getCurrPresentationTime();
    int64_t PTS = INVALID_TIMESTAMP;

    if(pSampleAttr != NULL)
    {
        PTS = pSampleAttr->getPTS();
    }

    dropable = !pSampleAttr->isDependedOn();
    dropFrame = checkIfShouldDropFrame(PTS, currPresentationTime, dropable);
    dropFrame |= checkIfShouldDropUntilNextGOP(pSampleAttr, currPresentationTime);

    if(!dropFrame){
        gotPicture = true;
        enqueueTimestamp(PTS);
        frameBuffer.setTimestamp(dequeueTimestamp());
        consecutiveDroppedFrameCount_ = 0;
    } else {
        gotPicture = false;
        consecutiveDroppedFrameCount_++;
        videoDroppedFrameCountDec_++;
        resend = false;
        statsCounter_.frameDropped();
        NTRACE(TRACE_MEDIAPLAYBACK,
               "Video Decoder Sink drops frame at decode%s: pts=%lld currPresentationTime=%lld (diff: %lld)\n",
               dropUntilNextGOP_ ? " (dropping remainder of GOP)" : "",
               PTS, currPresentationTime,
               currPresentationTime - PTS);
    }
    return NoError;
}

void VideoDecoderHEVCSink::reset()
{
    resetTimestampQueue();
    consecutiveDroppedFrameCount_ = 0;
    dropUntilNextGOP_ = false;
}
