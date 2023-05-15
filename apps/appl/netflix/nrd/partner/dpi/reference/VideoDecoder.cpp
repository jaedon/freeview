/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "VideoDecoder.h"
#include "VideoRenderer.h"
#include <nrddpi/Common.h>
#include <algorithm>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Log.h>
#include <nrdbase/Time.h>

#define TIMESTAMP_DEBUG 1
//#define NALSIZETYPE_DEBUG 1

using namespace netflix::device::esplayer;
using namespace netflix;

VideoDecoder::VideoDecoder(const VideoRenderer::VideoRendererInfo& rendererInfo, uint32_t pipelineId)
  : rendererInfo_(rendererInfo),
    mPipelineId(pipelineId),
    flushState_(ESPlayerNative::FLUSH_OFF),
    inputExhausted_(true),
    mutex_(ESP_DECODER_MUTEX, "VideoDecoder Mutex")
{
}

VideoDecoder::~VideoDecoder()
{
}

void VideoDecoder::enqueueTimestamp(int64_t timestamp)
{
    if(timestamp == INVALID_TIMESTAMP)
        return;

#if defined (TIMESTAMP_DEBUG) && defined(NRDP_HAS_TRACING)
    static int64_t prevTimestamp = 0;
    static int64_t prevMonotime = 0;
    int64_t monoTime = Time::mono().val();

    NTRACE(TRACE_MEDIAPLAYBACK, "[%d] %s: Monotime %lld (diff %lld)\tPTS %lld (diff %lld)"
           , mPipelineId, __func__, monoTime, monoTime - prevMonotime, timestamp, timestamp - prevTimestamp);

    prevTimestamp = timestamp;
    prevMonotime = monoTime;
#endif

    std::list<int64_t>::iterator insertPos =
        std::lower_bound(timestampReorderQueue_.begin(), timestampReorderQueue_.end(), timestamp);
    timestampReorderQueue_.insert(insertPos, timestamp);
}

int64_t VideoDecoder::dequeueTimestamp()
{
    if(timestampReorderQueue_.empty())
        return 0;
    int64_t timestamp = timestampReorderQueue_.front();
    timestampReorderQueue_.pop_front();

#if defined (TIMESTAMP_DEBUG) && defined(NRDP_HAS_TRACING)
    static int64_t prevTimestamp = 0;
    static int64_t prevMonotime = 0;
    int64_t monoTime = Time::mono().val();

    NTRACE(TRACE_MEDIAPLAYBACK, "[%d] %s: Monotime %lld (diff %lld)\tPTS %lld (diff %lld)"
           , mPipelineId, __func__, monoTime, monoTime - prevMonotime, timestamp, timestamp - prevTimestamp);

    prevTimestamp = timestamp;
    prevMonotime = monoTime;
#endif

    return timestamp;
}

void VideoDecoder::resetTimestampQueue()
{
    timestampReorderQueue_.clear();
}


void VideoDecoder::setFlushState(ESPlayerNative::FlushState flushState)
{
    ScopedMutex cs(mutex_);
    flushState_ = flushState;
}

ESPlayerNative::FlushState VideoDecoder::getFlushState()
{
    ScopedMutex cs(mutex_);
    return flushState_;
}

void VideoDecoder::setInputExhausted(bool inputExhausted)
{
    ScopedMutex cs(mutex_);
    inputExhausted_ = inputExhausted;
}

bool VideoDecoder::isInputExhausted()
{
    ScopedMutex cs(mutex_);
    return inputExhausted_;
}

void VideoDecoder::convertToAnnexBAndCheckProperties(const unsigned char* auData, uint32_t auDataSize)
{
    if(auData != NULL)
    {
        unsigned int i = 0;
        unsigned char* data = const_cast<unsigned char*>(auData);

        while(i + 3 < auDataSize)
        {
            uint32_t naluSize =
                (data[i + 0] << 24) +
                (data[i + 1] << 16) +
                (data[i + 2] <<  8) +
                (data[i + 3]);

            data[i + 0] = 0;
            data[i + 1] = 0;
            data[i + 2] = 0;
            data[i + 3] = 1;

#if defined(NALSIZETYPE_DEBUG)
            NTRACE(TRACE_MEDIAPLAYBACK, "NAL size: %d, NAL type: 0x%x 0x%x\n", naluSize, data[i+4], data[i+5]);
#endif
            i += 4 + naluSize;
        }
    }
}
