/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "PlaybackDevice.h"
#include "DeviceConstants.h"
#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>

using namespace netflix;
using namespace netflix::device;
using namespace netflix::device::esplayer;


PlaybackDevice::ESPlayerCallback::~ESPlayerCallback()
{
}

PlaybackDevice::ESPlayerCallback::ESPlayerCallback(PlaybackDevice& playbackDevice,
                                                           esplayer::MediaType mediaType) :
  mMediaType(mediaType),
  mPlaybackDevice(playbackDevice),
  mDisabled(true),
  mUnderflowReported(false),
  mPlaybackComplete(false),
  mMutex(ESP_CALLBACK_MUTEX, "ESPlayer Callback Mutex")
{

}

esplayer::Status
PlaybackDevice::ESPlayerCallback::getNextMediaSample(esplayer::ISampleWriter& writer)
{
    // Since mDisabled is set by the pumping thread and this is a device thread,
    // get the mutex.

    ScopedMutex cs(mMutex);

    if(mDisabled) {
        return esplayer::NO_AVAILABLE_SAMPLES;
    }


    // Get an NFErr back from the demux. If it's bad, send an error stack up to
    // the playback device. translate the NFErr to an esplayer status code as
    // specified in IElementaryStreamPlayer.
    esplayer::Status status  = mPlaybackDevice.demuxer_->getNextSample(mMediaType, writer);
    // report error up to the playback device
    if(status == STREAM_ERROR){
        reportError(esplayer::STREAM_ERROR,
                    mMediaType + 1,
                    "Mp4Demultiplexer::getNextMediaSample() encountered an error.");
    }

    if(status == ERROR){
        reportError(esplayer::SAMPLE_WRITER_ERROR,
                    mMediaType + 1,
                    "Mp4Demultiplexer::getNextMediaSample() encountered an sample writer error.");
    }


    return status;
}

esplayer::Status
PlaybackDevice::ESPlayerCallback::getNextMediaSampleSize(uint32_t& sampleSize, uint32_t viewNum)
{
    // Since mDisabled is set by the pumping thread and this is a device thread,
    // get the mutex.

    ScopedMutex cs(mMutex);

    if(mDisabled) {
        return esplayer::NO_AVAILABLE_SAMPLES;
    }


    // Get an NFErr back from the demux. If it's bad, send an error stack up to
    // the playback device. translate the NFErr to an esplayer status code as
    // specified in IElementaryStreamPlayer.
    esplayer::Status status  = mPlaybackDevice.demuxer_->getNextSampleSize(mMediaType, sampleSize, viewNum);
    // report error up to the playback device
    if(status == STREAM_ERROR){
        reportError(esplayer::STREAM_ERROR,
                    mMediaType + 1,
                    "Mp4Demultiplexer::getNextSampleSize() encountered an error.");
    }

    if(status == ERROR){
        reportError(esplayer::INTERNAL_ERROR,
                    mMediaType + 1,
                    "Mp4Demultiplexer::getNextSampleSize() encountered an internal demuxer error.");
    }


    return status;
}

esplayer::Status
PlaybackDevice::ESPlayerCallback::getNextMediaSampleTimestamps(llong& dts,
                                                               llong& pts)
{
    // Since mDisabled is set by the pumping thread and this is a device thread,
    // get the mutex.

    ScopedMutex cs(mMutex);

    if(mDisabled) {
        return esplayer::NO_AVAILABLE_SAMPLES;
    }


    // Get an NFErr back from the demux. If it's bad, send an error stack up to
    // the playback device. translate the NFErr to an esplayer status code as
    // specified in IElementaryStreamPlayer.
    esplayer::Status status  = mPlaybackDevice.demuxer_->getNextSampleTimestamps(mMediaType, dts, pts);
    // report error up to the playback device
    if(status == STREAM_ERROR){
        reportError(esplayer::STREAM_ERROR,
                    mMediaType + 1,
                    "Mp4Demultiplexer::getNextMediaSampleTimestamps encountered an error.");
    }
    if(status == ERROR){
        reportError(esplayer::INTERNAL_ERROR,
                    mMediaType + 1,
                    "Mp4Demultiplexer::getNextMediaSampleTimestamps encountered an internal demuxer error.");
    }

    return status;
}

void PlaybackDevice::ESPlayerCallback::updatePlaybackPosition(const llong& timestamp)
{
    ScopedMutex cs(mMutex);

    if(!mDisabled)
    {
        // Report video pts updates only to prevent us from needing a mutex in
        // PlaybackDevice::positionUpdatedReported().
        if(mMediaType == esplayer::VIDEO)
        {
            // The PlaybackDevice gets the postEventMutex inside this call. The
            // PlaybackDevice never holds that mutex when it calls into
            // ESPlayerCallback so it shouldn't cause deadlock.
            mPlaybackDevice.positionUpdateReported(timestamp, Time::mono().ms());
        }
    }
}


void PlaybackDevice::ESPlayerCallback::reportUnderflow()
{
    ScopedMutex cs(mMutex);

    if(!mDisabled)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "ESPlayerCallback:reportUnderflow. MediaType: %s",
               mMediaType == esplayer::AUDIO ?  "AUDIO" : "VIDEO");

        if(!mUnderflowReported)
        {
            if(mPlaybackDevice.reportUnderflow() == true)
            {
                mUnderflowReported = true;
            }
        }
    }
}

void PlaybackDevice::ESPlayerCallback::resetUnderflowReported()
{
    ScopedMutex cs(mMutex);
    mUnderflowReported = false;
}

bool PlaybackDevice::ESPlayerCallback::reportUnderflowReceived()
{
    ScopedMutex cs(mMutex);
    return mUnderflowReported;
}

void PlaybackDevice::ESPlayerCallback::playbackCompleted()
{
    ScopedMutex cs(mMutex);
    if(!mDisabled)
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackDevice::ESPlayerCallback::playbackCompleted() "
               " MediaType: %s", mMediaType == esplayer::AUDIO ?  "AUDIO" : "VIDEO");

        if(!mPlaybackComplete)
        {
            mPlaybackDevice.playbackCompleteReported(mMediaType);
            mPlaybackComplete = true;
        }
    }
}

bool PlaybackDevice::ESPlayerCallback::playbackCompletedReceived()
{
    ScopedMutex cs(mMutex);
    return mPlaybackComplete;
}

void PlaybackDevice::ESPlayerCallback::resetPlaybackCompleted()
{
    ScopedMutex cs(mMutex);
    mPlaybackComplete = false;
}

void PlaybackDevice::ESPlayerCallback::reportError(esplayer::Status errorCategory,
                                                   ullong errorCode,
                                                   const std::string& errorMessage)
{
    Log::error(TRACE_MEDIAPLAYBACK, "PlaybackDevice::ESPlayerCallback: Stream player reports "
               "error. MediaType: %s, category: %d, code: %llx, message: %s",
               mMediaType == esplayer::AUDIO ? "audio" : "video", static_cast<int>(errorCategory),
               errorCode,
               errorMessage.c_str());
    mPlaybackDevice.errorReported(errorCategory, errorCode, errorMessage);
}

void PlaybackDevice::ESPlayerCallback::reportError(IDeviceError err)
{
    reportError(err.getCategoryCode(), err.getDeviceSpecificErrorCode(), err.getDeviceSpecificErrorInfoStr());
}

void PlaybackDevice::ESPlayerCallback::reportError(NFErr err)
{
    shared_ptr<const NFError> error = err.find(NFErr_PlaybackDeviceError);
    if( error.get()){
        IDeviceError *derr = reinterpret_cast<IDeviceError*> (const_cast<NFError*> (error.get()));
        if( derr ){
            esplayer::Status status = derr->getCategoryCode();
            ullong dserr = derr->getDeviceSpecificErrorCode();
            std::string errMsg = derr->getDeviceSpecificErrorInfoStr();
            reportError(status, dserr, errMsg);
        }
    }
}

void PlaybackDevice::ESPlayerCallback::beginFlush()
{
    ScopedMutex cs(mMutex);
    mDisabled = true;
}

void PlaybackDevice::ESPlayerCallback::endFlush()
{
    ScopedMutex cs(mMutex);
    mDisabled = true;
    assert(mDisabled == true);
    mUnderflowReported = false;
    mPlaybackComplete = false;
    mDisabled = false;
}

esplayer::MediaType PlaybackDevice::ESPlayerCallback::getMediaType()
{
    return mMediaType;
}
void PlaybackDevice::ESPlayerCallback::disable()
{
    ScopedMutex cs(mMutex);
    mDisabled = true;
}

void PlaybackDevice::ESPlayerCallback::enable()
{
    ScopedMutex cs(mMutex);
    mDisabled = false;
}

void PlaybackDevice::ESPlayerCallback::updateDecodingStats(int numDecoded,
                                                           int numSkipped,
                                                           int numReducedQualityDecodes,
                                                           int numErrors)
{
    ScopedMutex cs(mMutex);

    if(numDecoded != STAT_NOT_SET) {
        if(mStatTotals.mNumDecoded == STAT_NOT_SET)
        {
            mStatTotals.mNumDecoded = numDecoded;
        } else {
            mStatTotals.mNumDecoded += numDecoded;
        }
    }
    if(numSkipped != STAT_NOT_SET) {
        if(mStatTotals.mNumSkipped == STAT_NOT_SET)
        {
            mStatTotals.mNumSkipped = numSkipped;
        } else {
            mStatTotals.mNumSkipped += numSkipped;
        }
    }
    if(numReducedQualityDecodes != STAT_NOT_SET){
        if(mStatTotals.mNumReducedQualityDecodes == STAT_NOT_SET){
            mStatTotals.mNumReducedQualityDecodes = numReducedQualityDecodes;
        } else {
            mStatTotals.mNumReducedQualityDecodes +=
                numReducedQualityDecodes;
        }
    }
    if(numErrors != STAT_NOT_SET){
        if(mStatTotals.mNumDecoderErrors == STAT_NOT_SET) {
            mStatTotals.mNumDecoderErrors = numErrors;
        } else {
            mStatTotals.mNumDecoderErrors += numErrors;
        }
    }
}

void PlaybackDevice::ESPlayerCallback::updateRenderingStats(int numRendered,
                                                            int numDropped,
                                                            int numErrors)
{
    ScopedMutex cs(mMutex);

    if(numRendered != STAT_NOT_SET) {
        if(mStatTotals.mNumRendered == STAT_NOT_SET)
        {
            mStatTotals.mNumRendered = numRendered;
        } else {
            mStatTotals.mNumRendered += numRendered;
        }
    }
    if(numDropped != STAT_NOT_SET) {
        if(mStatTotals.mNumDropped == STAT_NOT_SET)
        {
            mStatTotals.mNumDropped = numDropped;
        } else {
            mStatTotals.mNumDropped += numDropped;
        }
    }
    if(numErrors != STAT_NOT_SET){
        if(mStatTotals.mNumRendererErrors == STAT_NOT_SET) {
            mStatTotals.mNumRendererErrors = numErrors;
        } else {
            mStatTotals.mNumRendererErrors += numErrors;
        }
    }
}

void PlaybackDevice::ESPlayerCallback::getPlaybackQualityStats(IPlaybackDevice::PlaybackQualityStats& stats)
{
    ScopedMutex cs(mMutex);

    stats = mStatTotals;
}
