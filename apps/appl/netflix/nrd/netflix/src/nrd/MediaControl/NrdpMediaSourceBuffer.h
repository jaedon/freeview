/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NRDP_MEDIA_SOURCE_BUFFER_H
#define NRDP_MEDIA_SOURCE_BUFFER_H

#include <nrdase/AseCommonDataTypes.h>

#include "IAdaptiveStreamingPlayer.h"
#include "MediaSourceAppendable.h"
#include "../StreamParser/MediaSourceStreamParser.h"
#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/SampleAttributes.h>
#include <deque>
#include <queue>

namespace netflix {
namespace mediacontrol {
class NrdpMediaSourceBuffer : public enable_shared_from_this<NrdpMediaSourceBuffer>
{
public:
    NrdpMediaSourceBuffer(uint32_t srcBufferId,
                          ase::MediaType mediaType,
                          uint32_t maxTrailingBufferLen,
                          uint32_t maxTrailingBufferSize,
                          ase::AseTimeInterval minAudioPtsGap);
    ~NrdpMediaSourceBuffer();

    /* ================= THREAD-SAFE FUNCTIONS ================ */
    void flush();

    bool queryTimeRange(ase::AseTimeStamp& startPts, ase::AseTimeStamp& endPts);

    ase::AseTimeStamp skip(ase::AseTimeStamp const& pts);

    bool empty();

    void setAudioSyncTimestamp( ase::AseTimeStamp const& pts );

    /* ======= Call only in HttpRequestManager thread  ======== */
    void completeFlush();

    void flush( ase::AseTimeStamp const& pts );

    void endOfStream();

    NFErr append( MediaSourceAppendablePtr pAppendable );

    /** Set the timestamp offset to apply from the next fragment to be appended */
    NFErr setTimestampOffset( int32_t timeOffset, uint32_t timescale );

    /* === API for MediaSouceAppendable === */

    void indicateBlocksAvailable();

    void provideBlock( ase::AseBufferPtr pData );

    /* ======= Call only in device thread  ======== */
    device::esplayer::Status getNextSample(device::esplayer::ISampleWriter& sampleWriter,
                                           device::esplayer::SampleAttributes& sampleAttributes);

    device::esplayer::Status peekNextSampleAttributes(device::esplayer::SampleAttributes& sampleAttributes);

    void enableSampleFeeding();

    void disableSampleFeeding();

protected:

    bool findPlaybackPosition( ase::AseTimeStamp const& pts, uint32_t& index );

    // this is called from device thread
    NFErr constructSampleAttributes(streamparser::MediaFragmentPtr& fragmentPtr,
                                    streamparser::Sample& sample,
                                    device::esplayer::SampleAttributes& sampleAttributes);

    NFErr constructVideoSampleAttributes(streamparser::MediaFragmentPtr& fragmentPtr,
                                         streamparser::Sample& sample,
                                         device::esplayer::SampleAttributes& sampleAttributes);

    NFErr constructAudioSampleAttributes(streamparser::MediaFragmentPtr& fragmentPtr,
                                         streamparser::Sample& sample,
                                         device::esplayer::SampleAttributes& sampleAttributes);

    device::esplayer::Status writeSample(device::esplayer::ISampleWriter& sampleWriter,
                                         streamparser::MediaFragmentPtr& fragmentPtr,
                                         streamparser::Sample& sample);

    NFErr createNalSizeMap(streamparser::MediaFragmentPtr& fragmentPtr,
                           streamparser::Sample& sample);

    NFErr rewriteSubSampleMapAndSampleSizeForMVCSplit(
        streamparser::MediaFragmentPtr& fragmentPtr,
        streamparser::Sample& sample,
        device::esplayer::SampleAttributes& sampleAttributes);

    NFErr rewriteSubSampleMapAndSize(streamparser::MediaFragmentPtr& fragmentPtr,
                                     streamparser::Sample& sample,
                                     device::esplayer::SampleAttributes& sampleAttributes);

    void calculateFrameRate(uint32_t timescale,
                            uint32_t sampleDuration,
                            uint32_t& framerateValue,
                            uint32_t& framerateRate);

    bool isSampleFeedingDisabled();

    void processAppendedData();

    struct TimestampOffset
    {
        int32_t     mOffset;
        uint32_t    mTimescale;

        TimestampOffset() : mOffset( 0 ), mTimescale( 100000000 ) {}

        TimestampOffset( int32_t offset, uint32_t timescale )
            : mOffset( offset )
            , mTimescale( timescale )
        {}
    };

    struct AppendableRecord
    {
        bool                        mHaveTimestampOffset;
        TimestampOffset             mTimestampOffset;
        MediaSourceAppendablePtr    mAppendablePtr;

        AppendableRecord( MediaSourceAppendablePtr pAppendable )
            : mHaveTimestampOffset( false )
            , mAppendablePtr( pAppendable )
        {}

        AppendableRecord( TimestampOffset const& offset, MediaSourceAppendablePtr pAppendable )
            : mHaveTimestampOffset( true )
            , mTimestampOffset( offset )
            , mAppendablePtr( pAppendable )
        {}
    };

protected:

    uint32_t                    mSrcBufferId;

    ase::MediaType              mMediaType;

    mutable Mutex               mMutex;

    std::list<AppendableRecord> mAppendableList;

    /** Timestamp offset to be applied to the next fragment */
    TimestampOffset             mCurrentTimestampOffset;

    /** Timestamp offset to be applied to next fragment to be appended */
    bool                        mHavePendingTimestampOffset;
    TimestampOffset             mPendingTimestampOffset;

    uint32_t                    mMaxTrailingBufferDuration;
    uint32_t                    mMaxTrailingBufferSize;

    /** Timestamp to synchronize audio */
    bool                        mHaveAudioSyncronizationPts;

    ase::AseTimeStamp           mAudioSynchronizationPts;

    // true: if the last media block provider for the content attached to the source buffer
    bool                        mEndOfTrack;

    bool                        mFlushPending;

    streamparser::MediaSourceStreamParser mMediaSourceStreamParser;

    std::map<uint32_t, std::deque<streamparser::MediaFragmentPtr> > mMediaTrackFragments;
    std::map<uint32_t, shared_ptr<streamparser::TrackHeader> > mTrackHeaders;

    std::vector<uint32_t> mNalSizeMap;
    std::vector<uint32_t> mMvcNalSizeMap[2];

    // index for fragment. Whenever new fragment needs to be read,
    // increment. Whenever old fragment get deleted, decrement
    uint32_t mOutputTrackId;
    uint32_t mOutputTrackFragmentIndex;
    uint32_t mOutputTrackSampleIndex;

    // multi track specific
    uint32_t mPrimaryTrackId;
    uint32_t mSecondaryTrackId;
    bool mIsSecondaryTrackDolbyVisionTrack;

    // to make sure only available data are delivered to device
    uint32_t mPrimarySamplesDeliveredSize;
    uint32_t mSecondarySamplesDeliveredSize;

    bool mAudioDiscontinuity;
    bool mEndOfStreamLogged;

    // audio gap handling for segmented playlist
    ase::AseTimeInterval mMinAudioPtsGap;

    bool mSampleFeedingDisabled;
    bool mParseErrorHappened;
};

}}
#endif
