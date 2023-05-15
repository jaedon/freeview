/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MEDIA_FRAGMENT_H
#define MEDIA_FRAGMENT_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/IMediaSink.h>
#include "MediaStream2.h"
#include "MediaTrack2.h"
#include <nrdase/AseTimeStamp.h>
#include <nrdase/AseBuffer.h>
#include <nrdase/DebugLogging.h>

#include <nrdase/IMediaFragment.h>

#include <list>

namespace netflix {
namespace ase {


struct BlockMetaInfo
{
    BlockMetaInfo( uint32_t blockSize, bool startOfFragment, bool finalBlock ) :
                mBlockSize(blockSize),
                mStartOfFragment(startOfFragment),
                mFinalBlock(finalBlock)
    {
    }

    uint32_t mBlockSize;
    bool     mStartOfFragment;
    bool     mFinalBlock;
};

DECLARE_LOGGER( MediaFragmentLog );

/** Class representing a single media fragment
 *
 * Objects of this class receive the media fragment data and store it
 * until the right time to forward it to the MediaSink. MediaFragments
 * are linked so that each fragment can indicate to the next one that
 * it should start forwarding media data
 *
 * Under the following scenarios, data forwarding is triggered
 * 1. startForwarding is called from outside,
 * 2. new data is coming and mForward flag is set
 */
class MediaFragment : public IMediaFragment
{
public:
    typedef shared_ptr<MediaFragment> MediaFragmentPtr;

    /** Construct a new media fragment
     * @param startPts [in] The start PTS of the fragment
     * @param fragmentInfo [in] The duration and size of the fragment
     * @param pStream [in] The MediaStream of which the fragment is part
     * @param pMediaSink [in] The MediaSink to which the fragment data
     *                        should be sent
     * @param lastFragment [in] true if this is the last fragment of the stream
     * @param pFragment [out] The constructed media fragment
     * @return 0 on success or negative error code to be defines
     */
    static AseErrorCode construct(
                          uint32_t fragmentId,
                          int32_t manifestId,
                          int32_t trackId,
                          MediaType mediaType,
                          uint32_t cdnId,
                          AseTimeStamp const&      startPts,
                          MediaFragmentInfo const&     fragmentInfo,
                          shared_ptr<MediaStream> pMediaStream,
                          shared_ptr<MediaPresentation>    pMediaPresentation,
                          IMediaSinkPtr const&   pMediaSink,
                          uint32_t bufferUnitSize,
                          IMediaBufferPoolPtr pMediaBufferPool,
                          bool lastFragment,
                          ByteRange const& byteRange,
                          uint32_t numOfFragments,
                          std::deque<ullong> const& startOfFragmentsByteOffsets,
                          int32_t predictedDlTimeForBackupStream,
                          uint32_t maxDataDryPeriod,
                          bool forwardDataBeforeComplete,
                          uint32_t minBlockCompleteTime,
                          uint32_t blockReceptionBufferBudget,
                          uint32_t abandonBlockControlTime,
                          uint32_t abandonBlockControlPct,
                          uint32_t alternativeFragmentSize,
                          MediaFragmentPtr&     pFragment);

    ~MediaFragment();

    /**
     * IMediaFragment Interface
     */
    /** Get the time at which this request should be completed */
    virtual AseTimeVal getCompletionDeadline() const;

    /** get notified that the playback buffer is temporarilly full */
    virtual void bufferFull();
    virtual bool isBufferAvailable()
    {
        return mMediaPresentationPtr.lock()->isBufferAvailable(mMediaType);
    }

    /** Handle media data
     * @param pBuffer [in] Buffer containing some media data
     * @param sequence [in] the sequence number of the incoming buffer
     * @return: negative error codes; 1 if the request is completed
     */
    virtual int provideResponseBody( AseBufferPtr const& pBuffer, int sequence, AseTimeVal& abandontime );

    /** Handle a failure of an MEDIA request
     * @param code [in] The MEDIA failure code
     */
    void provideFailureCode( AseErrorCode errCode );


    /** Get the number of bytes received */
    size_t getBytesReceived() const;

    uint32_t remainingBytes() const;

    /** Get the start timestamp */
    AseTimeStamp getStartPts() const;

    /** Get the fragment id */
    uint32_t getFragmentId() const;

    /** Get the fragment's duration */
    AseTimeStamp getDuration() const;

    /** get the nominal bitrate of the fragment */
    int getNominalBitrate() const;

    /** Get the media fragment info */
    BufferedFragmentInfo getFragmentInfo() const;

    /** set the corresponding media request object */
    void setMediaRequest( shared_ptr<MediaRequest> pMediaRequest);

    /** Set the next/prev fragment
     *  @param pFragment [in] The next/prev fragment in playout sequence
     */
    void setNextFragment( shared_ptr<MediaFragment> pFragment );
    void setPrevFragment( shared_ptr<MediaFragment> pFragment );

    /** Return the next/prev fragment in playout sequence */
    shared_ptr<MediaFragment> getNextFragment();
    shared_ptr<MediaFragment> getPrevFragment();

    /** get byte range for the next request */
    AseErrorCode getNextRequestRange(ByteRange& byteRange);

    /** Return whether the fragment has been completed downloaded and forwarded */
    bool downloadedAndForwarded();

    /** Trigger the Media Fragment to start forwarding media data
     */
    void startForwarding();
    void completeForwarding();

    shared_ptr<MediaStream> getMediaStream() const;


    // immediately abandon
    void abandonImmediately();

    // predict when the fragment will be completed
    uint32_t predictCompleteTime(uint32_t throughput);

    bool hasStartedForwardingData();

    bool isAbandoned();

protected:
    MediaFragment( uint32_t fragmentId,
                   int32_t manifestId,
                   int32_t trackId,
                   MediaType mediaType,
                   uint32_t cdnId,
                   AseTimeStamp const&      startPts,
                   MediaFragmentInfo const&     fragmentInfo,
                   shared_ptr<MediaStream> pMediaStream,
                   shared_ptr<MediaPresentation>    pMediaPresentation,
                   IMediaSinkPtr const&   pMediaSink,
                   uint32_t bufferUnitSize,
                   IMediaBufferPoolPtr pMediaBufferPool,
                   bool lastFragment,
                   ByteRange const& byteRange,
                   uint32_t numOfFragments,
                   std::deque<ullong> const& startOfFragmentsByteOffsets,
                   int32_t predictedDlTimeForBackupStream,
                   uint32_t maxDataDryPeriod,
                   bool forwardDataBeforeComplete,
                   uint32_t minBlockCompleteTime,
                   uint32_t blockReceptionBufferBudget,
                   uint32_t abandonBlockControlTime,
                   uint32_t abandonBlockControlPct,
                   uint32_t alternativeFragmentSize);

    // deliver as much data as possilble to the media sink
    void deliverDataToSink();

    AseErrorCode deliverOneBlock(AseBufferPtr pDataBlock, bool finalBlock);

    /**
     * @param[in] blockSize
     * @param[in] finalBlock, whether it's the last block of the entire fragment
     */
    AseErrorCode constructAndDeliverLastBlockOfAGop(uint32_t blockSize, bool startOfFragment, bool finalBlock);



    size_t computeCurrentBlockSize(bool finalBlock);
    size_t computeCurrentBlockSize(AseBufferPtr pDataBlock);
    size_t updateBlockSizeForFragmentBoundary(size_t currentBlockSize);

    void debugOutput(AseBufferPtr pDataBlock, uint32_t startIndex);

    uint32_t computeNumberOfBlocks(uint32_t fragmentSize, uint32_t bufferUnitSize);
    uint32_t computeCurrentGopSize();
    bool     isStartOfFragment();
    void updateGopStartPts();

    AseErrorCode deliverRemainingData(bool finalBlock);

    weak_ptr<MediaStream>        mMediaStreamPtr;
    weak_ptr<MediaPresentation>  mMediaPresentationPtr;

    IMediaSinkPtr       mMediaSinkPtr;

    weak_ptr<MediaFragment>      mNextMediaFragmentPtr;
    weak_ptr<MediaFragment>      mPrevMediaFragmentPtr;

    uint32_t            mBufferUnitSize;
    IMediaBufferPoolPtr     mMediaBufferPoolPtr;

    /** Flag to indicate if media should be forwarded */
    bool                mForward;

    uint32_t            mFragmentId;
    int32_t             mManifestId;
    int32_t             mTrackId;
    MediaType           mMediaType;
    uint32_t            mCdnId;
    AseTimeStamp        mFragmentStartPts;
    BufferedFragmentInfo mFragmentInfo;

    std::list<BufferObj>    mBufferedData;

    // the expected sequence number of the next buffer in sequence
    uint32_t            mExpectedSequenceNum;
    // the sequence number of the last buffer
    uint32_t            mFinalSequenceNum;

    bool                mFinalBlock;

    bool                mLastFragment;
    bool                mDownloadedAndForwarded;

    ByteRange           mByteRange;
    bool                mAllowToForward;
    int32_t             mPredictDlTimeForBackup;

    // Variables for multiple blocks feature
    uint32_t            mNumOfFragments;
    std::deque<ullong>  mStartOfFragmentsByteOffsets;
    unsigned char*      mDataFromPreviousBlock;
    uint32_t            mDataSizeFromPreviousBlock;
    // the current start byte offset of undelievered data
    ullong              mCurrentByteOffset;
    AseTimeStamp        mCurrentTimestamp;
    AseTimeStamp        mCurrentGopStartPts;
    AseTimeStamp        mTimeSlicePerBlock;
    AseTimeStamp        mGopDuration;

    uint32_t            mMaxDataDryPeriod;
    bool                mHasStartedForwarding;

    /** meta info for the last block of a gop which has been unsuccessfully delivered last time */
    shared_ptr<BlockMetaInfo> mLastBlockMetaInfo;

    weak_ptr<MediaRequest> mMediaRequestPtr;

    uint32_t                mNumOfBlocksReceived;
    uint32_t                mNumOfBlocksDelivered;

    AseTimeVal         mCompletionDeadline;

    AseTimeVal         mFragmentCreateTime;
    AseTimeVal         mAbandonBlockControlTime;
    AseTimeVal         mFirstDataArrivalTime;

    uint32_t                mAbandonBlockControlPct;
    uint32_t                mAlternativeFragmentSize;
};

typedef MediaFragment::MediaFragmentPtr MediaFragmentPtr;

}}
#endif /* MEDIA_FRAGMENT_H */
