/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MEDIA_STREAM_H
#define MEDIA_STREAM_H

#include <nrdase/IMediaConsumer.h>
#include <nrdase/ITrackInfo.h>
#include <nrdase/Manifest.h>
#include "MediaTrack2.h"
#include <nrdase/IMediaFile.h>

#include <nrdase/DebugLogging.h>

namespace netflix {
namespace ase {
DECLARE_LOGGER( MediaStreamLog );

class MediaFragment;
typedef shared_ptr<MediaFragment> MediaFragmentPtr;

class MediaStreamRequest;
typedef shared_ptr<MediaStreamRequest> MediaStreamRequestPtr;

class IStreamingLocationSet;

/**
 * @class MediaStream
 * @brief Represents a single stream of a track
 *
 * This class represents a stream of an active track which could
 * potentially be streamed. This object is created using information
 * from the manifest, after stripping out any unsupported or unsuitable
 * streams.
 *
 * When a stream is selected (by the stream selection
 * algorithm) the parent MediaTrack requests the next fragment required
 * from the stream and repeatedly calls getNextRequest to enable the stream
 * to issue the requests needed to obtain the media requested.
 *
 * There are no assumptions about what requests are needed - this is up to
 * the stream. Requests for headers and indexes might be needed and we
 * don't assume anything about mapping between requests and blocks (one
 * request could be issued for multiple blocks or multiple requests for
 * one block).
 *
 * This class must implement the IStreamInfo interface which is used by
 * the stream selection object to obtain information about the future
 * bitrate profile of the stream.
 *
 *       1) The MediaTrack needs to maintain a representation of the current
 *          requested and buffered data, which is queried by the StreamSelector
 *          So, a method is needed for the MediaStream to report the requested
 *          blocks and the current reception status. This will be done with a new
 *          MediaFragment class which represents a single fragment and owns the
 *          MEDIA request objects for that fragment.
 *
 *       2) The actual media data needs to be fed into the MediaSink, preferably
 *          as it arrives. MediaFragment objects will be linked in time order
 *          so that each fragment can indicate to the next when it has fed
 *          all of its data into the MediaSink, triggering the next one to start
 *          feeding whatever is ready.
 *
 *          For each request, there is only one media fragment or one super media fragment,
 *          which is a combination of multiple fragments
 */
class MediaStream : public IStreamInfo
{
public:
    typedef shared_ptr<MediaStream> MediaStreamPtr;

    /** construct the media stream object
     * @param pMediaTrack [in] The parent media track object.
     * @param pManifestStream [in] The stream manifest.
     * @param streamId [in] the stream identifier
     * @param pConfigParameters [in] Configuration parameters
     * @param pMediaStream [out] The new media stream object
     * @return 0 success or negative error code to be defined
     */
    static int construct( shared_ptr<MediaTrack> pMediaTrack,
                          shared_ptr<MediaPresentation>    pMediaPresentation,
                          ManifestStreamPtr pManifestStream,
                          IMediaSinkPtr const&           pMediaSink,
                          int32_t     manifestId,
                          int32_t     streamId,
                          IMediaBufferPoolPtr const&   pMediaBufferPool,
                          ILocationSelectorPtr const& pLocationSelector,
                          IAseConfigParameterPtr pConfigParameters,
                          AseTimeStamp const& timestampOffset,
                          IMediaFilePtr pPreExistingMediaFile,
                          MediaStreamPtr&               pMediaStream );

    void configure( IAseConfigParameterPtr pConfigParameters );

    /** Set the current download time from outside
     * @param [in/out] currentTime The current download time
     * The information returned by the IStreamInfo API will start at this
     * timestamp and the next fragment returned by getNextFragment will
     * be the first fragment which starts at or later than this time
     */
    AseTimeStamp updateStreamingTime( AseTimeStamp const& currentTime );

    /** Ask if the object has MEDIA requests it wishes to issue
     *  @return True if the object has requests to issue. The caller
     *               should call getNextRequest to get the next request
     */
    bool hasPendingRequest(uint32_t currentBufferLen);

    /** Get a new Request object
     *  @param numOfBlocks [in] number of blocks to be requested
     *  @param newStreamingTime [out] the updated streaming time after schedule downloading a few fragments
     *  @param pRequest [out] The new MediaRequest object
     *  @return 0   for success, negative error code, or positive number
     *              indicating minimum time before next call in ms
     */
    AseErrorCode getNextRequest( int32_t numOfBlocks,
                            AseTimeStamp& newStreamingTime,
                            int32_t predictedDlTimeForBackupStream,
                            uint32_t alternativeFragmentSize,
                            MediaPresentationRequestPtr& pRequest);

    /** Get a new request object when MediaStream has a pending request */
    AseErrorCode getNextRequest( AseTimeStamp& newStreamingTime,
                                 MediaPresentationRequestPtr& pRequest);

    int32_t computeNumOfBlocksToRequest(uint32_t maxRequestSize, uint32_t minRequestSize,
                                        int32_t maxRequestLen) const;

    /** Notify the download failure from a media fragment */
    void reportFailure(AseErrorCode errorCode, uint32_t fragmentId);

    bool deliverHeader(uint32_t& duration, uint32_t& gopSize);

    bool indexComplete();

    uint32_t getContentDuration();

    /** Get the byte range of the header+index */
    ByteRange getHeaderRange() const;

    /** Provide a byte range hint for the header+ndex */
    void provideHeaderRangeHint( ByteRange const& range );

    std::vector<unsigned char> const& getFileToken() const;

    void completeForwarding(AseTimeStamp const& fragmentStartPts,
                            AseTimeStamp const& duration,
                           uint32_t numOfBlocks, bool lastFragment);

    // IStreamInfo - public interface
    //               provides information about the future fragments
    //               of the stream (i.e. starting at the seek time or
    //               after the

    virtual int32_t streamId() const;
    virtual std::string const& streamKey() const;
    virtual bool empty() const;
    virtual bool complete() const;
    virtual AseTimeStamp firstFragmentTime() const;
    virtual AseTimeStamp currentFragmentTime() const;
    virtual std::vector<StartplayPoint>  getStartplayCurve() const;
    virtual AseErrorCode getPreferredLocationSet(shared_ptr<IStreamingLocationSet>& pStreamingLocationSet);
    virtual uint32_t getCurrentFragmentSize() const;

    // get the average bitrate in kbps
    virtual int getAverageBitrate() const;
    virtual bool isAvailable() const;
    virtual const_iterator begin();
    virtual const_iterator end();

    void reset();

    bool isCompatible(MediaStreamPtr pMediaStream);

    std::pair<int,int> getVideoSize() const;

    ManifestStreamPtr getManifestStream() const;

    shared_ptr<ILocationSetMonitor> getPreferredLocationSetMonitor();

    void enable();
    void disable();

    MediaType mediaType() const;

public:
    virtual ~MediaStream() {}

protected:
    MediaStream( shared_ptr<MediaTrack> pMediaTrack,
                 shared_ptr<MediaPresentation>    pMediaPresentation,
                 ManifestStreamPtr pManifestStream,
                 IAseConfigParameterPtr pConfigParameters,
                 IMediaFilePtr pMediaFile,
                 IMediaSinkPtr const&           pMediaSink,
                 IMediaBufferPoolPtr const&   pMediaBufferPool,
                 ILocationSelectorPtr const& pLocationSelector,
                 AseTimeStamp const& timestampOffset,
                 int32_t manifestId,
                 int32_t streamId);

    // IStreamInfo protected methods
    virtual const_iterator& increment( const_iterator& it );
    virtual MediaFragmentInfo dereference( const_iterator& it );
    virtual bool equal( const_iterator const& lhs, const_iterator const& rhs );

    /** return whether the media header has been downloaded */
    bool getHeader();

    /** return whether the media index has been downloaded */
    bool getIndex();

    /** create the media stream request for the provided media fragment */
    AseErrorCode createMediaStreamRequest( MediaFragmentPtr& pMediaFragment,
                                      AseTimeStamp& newStreamingTime,
                                      int32_t predictedDlTimeForBackupStream,
                                      uint32_t alternativeFragmentSize,
                                      MediaStreamRequestPtr& pRequest);

    AseErrorCode createMediaFragment(   MediaFragmentPtr& pMediaFragment,
                                        uint32_t cdnid,
                                        int32_t predictedDlTimeForBackupStream,
                                        uint32_t alternativeFragmentSize);

    void outputMediaIndex();

    void getConfiguration();

protected:

    /** The manifest information for this stream */
    ManifestStreamPtr  mManifestStreamPtr;

    /** parent media track object */
    weak_ptr<MediaTrack>           mMediaTrackPtr;
    weak_ptr<MediaPresentation>    mMediaPresentationPtr;

    IAseConfigParameterPtr  mConfigParameterPtr;

    /** interface to the media file itself */
    IMediaFilePtr           mMediaFilePtr;
    IMediaSinkPtr           mMediaSinkPtr;

    IMediaBufferPoolPtr     mMediaBufferPoolPtr;
    ILocationSelectorPtr    mLocationSelectorPtr;

    /** media fragment info list */
    MediaFileIndexConstPtr  mMediaIndexPtr;

    // true when we have retrieved the complete index
    bool                    mIndexComplete;

    AseBufferPtr            mMediaHeaderPtr;
    bool                    mHeaderDelivered;
    uint32_t                mGopSize;
    AseTimeStamp            mStreamingTime;

    uint32_t mNextFragmentId;
    int32_t mManifestId;
    int32_t mStreamId;

    uint32_t mRemainingBlocks;

    AseTimeStamp mCurrentFragmentStartPts;
    ullong mCurrentFragmentStartByteOffset;
    int32_t mCurrentFragmentIndex;
    AseTimeStamp mPrevFragmentStartPts;
    ullong mPrevFragmentStartByteOffset;
    int32_t mPrevFragmentIndex;
    bool    mFirstTimeAdjustStreamingTime;

    AseTimeStamp mInitFragmentStartPts;
    AseTimeStamp mIndexEndPts;

    ullong mInitFragmentStartByteOffset;
    int32_t mInitFragmentIndex;

    uint32_t mDuration;

    // whether this stream is available for streaming
    bool mAvailable;
    int32_t mPredictDlTimeForBackup;

    AseTimeStamp mTimeStampOffset;

    uint32_t mMaxDataDryPeriod;
    bool mForwardDataBeforeComplete;
    uint32_t mMinBlockCompleteTime;
    uint32_t mBlockReceptionBufferBudget;

    uint32_t mAbandonBlockControlTime;
    uint32_t mAbandonBlockControlPct;
};

typedef MediaStream::MediaStreamPtr MediaStreamPtr;

inline ByteRange MediaStream::getHeaderRange() const
{
    return mMediaFilePtr->getHeaderRange();
}

inline void MediaStream::provideHeaderRangeHint( ByteRange const& range )
{
    mMediaFilePtr->provideHeaderRangeHint( range );
}

}}
#endif
