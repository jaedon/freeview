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
#include "../ESPlayerConstants.h"
#include "../VideoRenderer.h"
#include "../ReferenceClockNative.h"

#include "VideoDecoderSink.h"

#include <nrdbase/Log.h>
#include <cstring>

#include <nrdbase/ScopedMutex.h>
#include <nrddpi/Common.h>
#include "../DecoderLock.h"

using namespace netflix::device::esplayer;
using namespace std;
using namespace netflix;


VideoDecoderSink::VideoDecoderSink(
    const VideoAttributes& attributes,
    ReferenceClockNative& referenceClock,
    const VideoRenderer::VideoRendererInfo& rendererInfo,
    bool& error)

  : VideoDecoder(rendererInfo, 0),
    referenceClock_(referenceClock),
    videoDroppedFrameCountDec_(0),
    consecutiveDroppedFrameCount_(0),
    dropUntilNextGOP_(false),
    enablePrintDTS_(false),
    firstDecodingAfterDecoderOpen_(true)
{
    NRDP_UNUSED(attributes);
    NRDP_UNUSED(error);

    // for QA test convienience
    char* printDTS = getenv("NF_VERBOSE_DTS");
    if(printDTS != NULL){
        enablePrintDTS_ = true;
    }
}

VideoDecoderSink::~VideoDecoderSink()
{

}

// *NOTE* Workaround for H.264 seamless video stream switch.
// Even though the FFMPEG codec seems to handle SPS/PPS update properly,
// there is no way to find out the actual dimension of the decoded picture
// from the output AVFrame data structure. Therefore, we still need to
// push out the last reference frame from the decoder, and restart the
// decoder, as well as image scaler, to prepare for the new stream.

// Returns whether you should reset the codec.
bool VideoDecoderSink::getUpdatedH264ExtraData(
    unsigned char*& new_extradata,
    int& new_extradata_size,
    const vector<const uint8_t*>& paramSets,
    const vector<uint32_t>& paramSetLens,
    int& new_width, int& new_height) const
{
    bool resetCodec = false;

    NRDP_UNUSED(new_extradata);
    NRDP_UNUSED(new_extradata_size);
    NRDP_UNUSED(paramSets);
    NRDP_UNUSED(paramSetLens);
    NRDP_UNUSED(new_width);
    NRDP_UNUSED(new_height);

    return resetCodec;
}


bool VideoDecoderSink::checkIfShouldDropFrame(const int64_t& PTS, const int64_t& currPresentationTime,
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
bool VideoDecoderSink::checkIfShouldDropUntilNextGOP(const ISampleAttributes *pSampleAttr,
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

void VideoDecoderSink::resetCodecWithUpdatedExtraData(unsigned char*& new_extradata,
                                                      const int& new_extradata_size,
                                                      const int& new_width,
                                                      const int& new_height)
{
    NRDP_UNUSED(new_extradata);
    NRDP_UNUSED(new_extradata_size);
    NRDP_UNUSED(new_width);
    NRDP_UNUSED(new_height);

    resetTimestampQueue();
    firstDecodingAfterDecoderOpen_ = true;
}

RefAppDeviceSpecificError VideoDecoderSink::decode(const ISampleAttributes *pSampleAttr,
                                                   const unsigned char* auData, uint32_t auDataSize,
                                                   VideoRenderer::VideoFrameBuffer& frameBuffer,
                                                   bool& gotPicture,
                                                   bool& resend,
                                                   Format3D& format3D)
{
    // flush processing
    if( (pSampleAttr == NULL) && (auData == NULL) && (auDataSize == 0) ){
        return refAppDeviceSpecific_NoError;
    }

    bool resetCodec = false, dropable = false, dropFrame = false;
    unsigned char* new_extradata = 0;
    int new_extradata_size = 0, new_width = 0, new_height = 0;

    int64_t currPresentationTime = referenceClock_.getCurrPresentationTime();

    NRDP_UNUSED(format3D);
    NRDP_UNUSED(auData);

    int64_t PTS = INVALID_TIMESTAMP;
    int64_t DTS = INVALID_TIMESTAMP;

    // Interpret a NULL pSampleAttr as an end of stream signal. A zero-length
    // auData will request the avcodec to push out the last picture.
    if(pSampleAttr != NULL)
    {
        PTS = pSampleAttr->getPTS();
        DTS = pSampleAttr->getDTS();

        if(pSampleAttr->isKeyFrame())
        {
            VideoAttributes* attr = NULL;

            if(pSampleAttr->getMediaAttributes() == NULL)
            {
                Log::error(TRACE_MEDIAPLAYBACK, "VideoDecoderSink: isKeyFrame but "
                           "getMediaAttributes() returns NULL");

                return refAppDeviceSpecific_MissingSampleAttributes;
            }

            if((attr = pSampleAttr->getMediaAttributes()->mVideoAttributes) == NULL)
            {
                Log::error(TRACE_MEDIAPLAYBACK, "VideoDecoderSink: isKeyFrame but "
                           "getMediaAttributes()->mVideoAttributes is NULL");
                return  refAppDeviceSpecific_MissingSampleAttributes;
            }


            resetCodec = getUpdatedH264ExtraData(new_extradata, new_extradata_size,
                                                 attr->mCodecSpecificData,
                                                 attr->mCodecSpecificDataLen,
                                                 new_width,
                                                 new_height);
            if(resetCodec)
            {
                auDataSize = 0;
            }


            dropable = !pSampleAttr->isDependedOn();
            dropFrame = checkIfShouldDropFrame(PTS, currPresentationTime, dropable);
        }
        dropFrame |= checkIfShouldDropUntilNextGOP(pSampleAttr, currPresentationTime);
    }

    if(enablePrintDTS_){
        NTRACE(TRACE_MEDIAPLAYBACK, "VideoDecoderSink::%s\t dataSize %d\t CurrentTime %lld\t PTS %lld\t DTS %lld\t %s", __func__ , auDataSize, currPresentationTime, PTS, DTS, dropFrame?"drop":"");
    }

    if(!dropFrame)
    {
        // here assume that decoding is successful
        gotPicture = true;
        enqueueTimestamp(PTS);
        if(gotPicture && !firstDecodingAfterDecoderOpen_){
            frameBuffer.setTimestamp(dequeueTimestamp());
        }
        firstDecodingAfterDecoderOpen_ = false;
        consecutiveDroppedFrameCount_ = 0;
    } else {
        // here, assume that decoding is skipped
        printf("Drop frame\n");

        gotPicture = false;
        consecutiveDroppedFrameCount_++;
        videoDroppedFrameCountDec_ ++;
        resend = false;
        statsCounter_.frameDropped();
        NTRACE(TRACE_MEDIAPLAYBACK,
               "Video Decoder drops frame at decode%s: dts=%lld currPresentationTime=%lld (diff: %lld)\n",
               dropUntilNextGOP_ ? " (dropping remainder of GOP)" : "",
               PTS, currPresentationTime,
               currPresentationTime - PTS);
    }

    if(resetCodec)
    {
        resetCodecWithUpdatedExtraData(new_extradata, new_extradata_size, new_width, new_height);
    }
    resend = resetCodec;

    return refAppDeviceSpecific_NoError;
}

void VideoDecoderSink::reset()
{
    //ScopedMutex scopedMutex(DecoderLock::mutexForOpenClose());

    resetTimestampQueue();
    consecutiveDroppedFrameCount_ = 0;
    dropUntilNextGOP_ = false;
}


