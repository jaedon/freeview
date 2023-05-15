/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef NRDP_MEDIASOURCE_H
#define NRDP_MEDIASOURCE_H

#include <nrdase/IAseConfigParameter.h>
#include <nrdase/AseTimeStamp.h>
#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/SampleAttributes.h>

#include <vector>
#include <map>

#include "NrdpMediaSourceBuffer.h"

namespace netflix {
namespace mediacontrol {

class NrdpMediaSource;
class MediaRequestManager;


/** @class NrdpMediaSourceManager
 *  @brief
 */
class NrdpMediaSourceManager
{
public:
    NrdpMediaSourceManager();
    ~NrdpMediaSourceManager();

public:
    // returns a sourceId that can be used to get a NrdpMediaSource
    uint32_t createMediaSource(ase::IAseConfigParameterPtr streamingConfig);

    // destroys the mediaSource identified by sourceId
    void destroyMediaSource(uint32_t sourceId);

    shared_ptr<NrdpMediaSource> getMediaSource(uint32_t sourceId);

    uint32_t numberOfMediaSources();

private:
    typedef std::map<uint32_t, shared_ptr<NrdpMediaSource> > SourceMap;
    SourceMap m_sources;

    uint32_t m_nextSourceId;
};


/** @class NrdpMediaSource
 *  @brief
 */
class NrdpMediaSource : public enable_shared_from_this<NrdpMediaSource>
{
public:
    NrdpMediaSource(ase::IAseConfigParameterPtr pConfigParameter);

    ~NrdpMediaSource();

    /**
     * @return source buffer id
     */
    uint32_t createSourceBuffer(ase::MediaType mediaType);

    /**
     * flush the media source buffer
     */
    void flushSourceBuffer(ase::MediaType mediaType);

    /**
     * flush all the source buffers
     */
    void flushSourceBuffers();

    /**
     * flush the media source buffer
     */
    void completeFlushSourceBuffer(ase::MediaType mediaType);

    /**
     * flush all the source buffers
     */
    void completeFlushSourceBuffers();

    /**
     * provide the available buffer range the player can skip without buffering
     */
    void querySourceBufferTimeRange(ase::AseTimeStamp& startPts, ase::AseTimeStamp& endPts);

    /**
     * notify end of stream
     */
    void endOfStream(uint32_t srcBufferId);

    /**
     * skip to the provided pts
     */
    NFErr skip(ase::AseTimeStamp const& pts);

    /**
     * append the media buffer block to the corresponding media buffer
     */
    NFErr append(MediaSourceAppendablePtr pAppendable, uint32_t srcBufferId);

    /**
     *
     */
    NFErr setTimestampOffset( uint32_t srcBufferId, int32_t timeOffset, uint32_t timescale );


    /**
     * set the media request manager tat should be used to perform flush completions
     */
    void setMediaRequestManager( shared_ptr<MediaRequestManager> pMediaReqiestManager );

    /**
     * perform media source tasks in HttpRequestManager thread
     */
    void mediaSourceTasks();

    /**
     * update the current playback time
     */
    void updatePts(uint32_t currentPts);

    /*
     * @return
     *
     * side effect: when return OK, Sample is written to sampleWriter
     */
    device::esplayer::Status getNextSample(device::esplayer::MediaType mediaType,
                                            device::esplayer::ISampleWriter& sampleWriter,
                                            device::esplayer::SampleAttributes& sampleAttributes);

    /*
     * This does not proceed sample position for next getNextSample nor
     * peekNextSampleAttributes
     */
    device::esplayer::Status peekNextSampleAttributes(device::esplayer::MediaType mediaType,
                                                      device::esplayer::SampleAttributes& sampleAttributes);

    /*
     * These enable/disable getNextSample. If enableSampleFeeding, getNextSample
     * will return NO_AVAILABLE_SAMPLE.
     */
    void enableSampleFeeding();
    void disableSampleFeeding();

protected:
    void loadConfigParas();
    bool enableSkipBack();

protected:
    mutable Mutex mMutex;

    ase::IAseConfigParameterPtr  mConfigParameterPtr;

    weak_ptr<MediaRequestManager>   mMediaRequestManager;

    // map source buffer id to source buffer
    std::map<uint32_t, shared_ptr<NrdpMediaSourceBuffer> > mMediaSourceBufferMap;

    shared_ptr<NrdpMediaSourceBuffer> mActiveAudioSourceBuffer;
    shared_ptr<NrdpMediaSourceBuffer> mActiveVideoSourceBuffer;

    ase::AseTimeStamp mCurrentPlayoutTime;
    ase::AseTimeStamp mLastFlushPlayoutTime;

    bool     mDidSetAudioSyncTimestamp;

    uint32_t mLatestSrcBufferId;

    // total A/V playback buffer size
    uint32_t mTotalVideoBufferSize;
    uint32_t mTotalAudioBufferSize;

    // maximum trailing buffer length in ms
    uint32_t mMaxTrailingBufferLen;
    // maximum A/V trailing buffer size in bytes
    uint32_t mMaxVideoTrailingBuffer;
    uint32_t mMaxAudioTrailingBuffer;
    // the safe guard for streaming without rebuffering
    ase::AseTimeStamp mMinPrebufLength;
    // minimum audioPtsGap to be honored if ptsGap is necessary
    ase::AseTimeInterval mMinAudioPtsGap;
};

}}


#endif
