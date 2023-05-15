/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MEDIACONTROL_MEDIASINK_H
#define MEDIACONTROL_MEDIASINK_H

#include <deque>

#include <nrdbase/Time.h>

#include <nrdase/IMediaSink.h>
#include <nrdase/IAseConfigParameter.h>

#include "AdaptiveStreamingPlayer.h"

namespace netflix {
namespace mediacontrol {

/* @class McMediaSink McMediaSink.h
 * @brief implementation of the IMediaSink interface for streaming engine.
 */
class AdaptiveStreamingPlayer::McMediaSink : public ase::IMediaSink
{
private:
    struct StreamHeaderWrapper
    {
        shared_ptr<std::vector<unsigned char> > streamHeader;
        uint32_t trackIndex;
        uint32_t streamIndex;
    };

    mutable Mutex mMutex;

    std::map<uint32_t, shared_ptr<const ase::Manifest> > mManifestMap;

    ase::IAseConfigParameterPtr  mConfigParameterPtr;
    ase::IAseConfigParameterPtr  mLocalConfigParameterPtr;
    bool                         mIsStereoVideo;

    std::deque<MediaEntity> mVideoMediaQueue;
    std::deque<MediaEntity> mAudioMediaQueue;

    // total A/V playback buffer size
    uint32_t mTotalVideoBufferSize;
    uint32_t mTotalAudioBufferSize;

    // The postion for the current A/V block being played
    int32_t mCurrentVideoPlaybackPos;
    int32_t mCurrentAudioPlaybackPos;
    // The position for the current A/V block to be delivered
    int32_t mNextVideoDeliveryPos;
    int32_t mNextAudioDeliveryPos;
    // the A/V trailing buffer size.
    // Trailing buffer size/length is buffer size/length before the current playback pts
    uint32_t mVideoTrailingBufferSize;
    uint32_t mAudioTrailingBufferSize;

    std::map<uint32_t, StreamHeaderWrapper> mFirstSelStreamHeaderMap;

    ase::AseTimeStamp mCurrentPlayoutTime;
    ase::AseTimeStamp mLastBufferUpdateTime;

    // maximum trailing buffer length in ms
    uint32_t mMaxTrailingBufferLen;
    // maximum A/V trailing buffer size in bytes
    uint32_t mMaxVideoTrailingBuffer;
    uint32_t mMaxAudioTrailingBuffer;
    // the safe guard for streaming without rebuffering
    uint32_t mMinPrebufLength;

    // map from stream index to stream header
    std::string mCurrentVideoStreamKey;
    std::string mCurrentAudioStreamKey;
    uint32_t mCurrentVideoGopStartPts;
    uint32_t mCurrentAudioGopStartPts;
    // a map between streamKey and video/audio header
    std::map<std::string, MediaEntity> mVideoHeaderMap;
    std::map<std::string, MediaEntity> mAudioHeaderMap;

    // New stream headers are moved here after they have been retrieved via
    // getNewStreamHeader(). We can't let them be deleted because the playback
    // device gets a raw pointer to the header and may use it until the
    // streaming player is closed. We only need to deliver them once and it's
    // easier to tell headers that have been delivered from ones that haven't by
    // moving them out of the map when they are delivered.
    std::vector<MediaEntity>       mDeliveredStreamHeaders;

    /* Counters for triggering logging */
    uint32_t mNumDeliverCallsSinceLastLog;
    uint32_t mNumReadBlockCallsSinceLastLog;
    const static uint32_t mLogTriggerThreshold = 300; // About 2 seconds worth
                                                      // at 4 Mbps
    void LogQueueState();

    void loadConfigParas();

    bool enableSkipBack();

    /** update the playback postion based on the updated playback pts
     * @param[in] mediaQueue: the provided media queue
     * @param[in/out] currentPlayPos: the current playback position in the queue
     * @param[in] currentDeliveryPos: the current delivery postion in the queue
     * @param[in/out] trailingBufferSize: the updated trailingBufferSize
     * @param[in] currentPts: the current playback pts
     */
    void updatePlaybackPosition( std::deque<MediaEntity> const& mediaQueue,
                                int32_t& currentPlayPos,
                                int32_t currentDelieveryPos,
                                uint32_t& trailingBufferSize,
                                uint32_t currentPts );

    /** pop one gop of data from the provided queue
     * @param[in/out] mediaQueue: the provided media queue
     * @param[in/out] currentPlayPos: the current playback position in the queue
     * @param[in/out] currentDeliveryPos: the current delivery postion in the queue
     * @param[out] trailingBufferSize: updated trailing buffer size
     */
    bool popOneGopData( std::deque<MediaEntity>& mediaQueue,
                        int32_t& currentPlayPos,
                        int32_t& currentDeliveryPos,
                        uint32_t& trailingBufferSize );

    /**
     * updated the training buffer size and playback position based on the given pts
     * @param[in] pts: the update playback pts
     * @param[in] mediaQueue: the media queue holding all the media blocks
     * @param[out] trailingBufferSize: updated trailing buffer size
     * @param[out] updatedPlaybackPos: updated playback position
     */
    void skipMedia( ase::AseTimeStamp const& pts,
                    std::deque<MediaEntity> const& mediaQueue,
                    uint32_t& trailingBufferSize,
                    int32_t& updatedPlaybackPos);

    /**
     * check whether the index in the given queue is invalid
     */
    bool invalidIndex(std::deque<MediaEntity> const& mediaQueue,
                      int32_t index) const;

    /**
     * construct the stream key
     */
    std::string computeStreamKey(MediaEntity const& mediaEntity);
    void parseStreamKey(std::string const& StreamKey, uint32_t& manifestIndex,
                        uint32_t& trackIndex, uint32_t& streamIndex);

    /**
     *  Moves a header from the audio or video header to the delivered stream
     *  headers vector.
     */
    void moveDeliveredHeaderFromMap(std::map<std::string, MediaEntity>& map,
                                    std::map<std::string, MediaEntity>::iterator it);

public:
    McMediaSink(const shared_ptr<const ase::Manifest> pManifest,
            ase::IAseConfigParameterPtr pConfigParameter,
            ase::IAseConfigParameterPtr pLocalConfigParameter,
            bool stereoVideo);
    ~McMediaSink();

    bool getFirstSelStream(
        uint32_t manifestIndex,
        /*out*/uint32_t &firstSelTrackIndex,
        /*out*/uint32_t &firstSelStreamIndex,
        /*out*/shared_ptr<std::vector<unsigned char> > &firstSelStreamHeader);

    void flush();

    /**
     * @return NFErr_OK if a block is successfully read;
     *         NFErr_Pending if there is no block available at the moment;
     *         NFErr_EndOfStream if it has reach the end
     */
    NFErr readBlock(ase::MediaType mediaType, MediaEntity &block);

    void updateVideoPlayPosition(
        /*out*/uint32_t& videoManifestIndex,
        /*out*/uint32_t& videoStreamIndex,
        /*out*/uint32_t& videoGopStartPTS,
        /*out*/uint32_t& videoGopBitrate,
        uint32_t currentPts);

    void updateAudioPlayPosition(
        /*out*/uint32_t& audioManifestIndex,
        /*out*/uint32_t& audioStreamIndex,
        /*out*/uint32_t& audioGopStartPTS,
        uint32_t currentPts);

    /**
     * skip to the provided pts
     */
    NFErr skip(ase::AseTimeStamp const& pts);

    /**
     * provide the available playback buffer range the player can skip without the risk of rebuffering
     */
    void getBufferRange(ase::AseTimeStamp& startPts, ase::AseTimeStamp& endPts);

    /**
     * Add a new period to the playback session
     * @param[in] pManifest the newly added manifest period to the playback
     * @return
     */
    NFErr addManifest(shared_ptr<const ase::Manifest> pManifest);

    // Removes the stream header from the McMediaSinks internal mapping.
    // Args:
    //          type[out]
    //          streamIndex[out]
    // Returns:
    //
    shared_ptr<std::vector<unsigned char> > getNewStreamHeader(ase::MediaType& type,
                                                               int32_t& manifestIndex,
                                                               int32_t& trackIndex,
                                                               int32_t& streamIndex);

    /**
     * get the cdn id for the current block being played
     */
    void getCurrentCdnId(uint32_t& vcdnid, uint32_t& acdnid) const;


    // IMediaSink
    //
    virtual bool canPlayType(ase::MimeType& mime) const;
    virtual void deliver(ase::AseBufferPtr const& dataBlock,
                         int32_t manifestIndex,
                         int32_t trackIndex,
                         int32_t streamIndex,
                         bool startOfFragment,
                         ase::AseTimeStamp const& pts);
    virtual void newStream(unsigned char const* mediaHeader,
                           size_t mediaHeaderSize,
                           int32_t manifestIndex,
                           int32_t trackIndex,
                           int32_t streamIndex);
    virtual void endOfTrack(int32_t manifestIndex, int32_t trackIndex);
    virtual void currentPlaybackInfo(double& currentPlayoutRate,
                                     ase::AseTimeStamp& currentPlayoutTime);

    virtual void flushMedia(ase::MediaType mediaType);
    virtual void updateStreamingBuffer();
};

}} // namespace netflix::mediacontrol

#endif // MEDIACONTROL_MEDIASINK_H
