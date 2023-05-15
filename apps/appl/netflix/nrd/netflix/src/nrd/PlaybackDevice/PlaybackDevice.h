/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef PLAYBACKDEVICE_H
#define PLAYBACKDEVICE_H

#include <assert.h>
#include <nrdbase/tr1.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <map>
#include <nrdbase/tr1.h>
#include <nrd/IPlaybackDevice.h>
#include <nrd/IDeviceError.h>
#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/ISystem.h>
#include "Demultiplexers/Mp4Demultiplexer.h"
#include "StreamAttributes.h"
#include <nrdbase/Mutex.h>
#include "EventQueue.h"
#include "ProducerConsumerList.h"

namespace netflix {
namespace device {

class Demultiplexer;

/**
 * @class PlaybackDevice PlaybackDevice.h
 * @brief Implementation of device playback functionality in native
 *        linux environment.
 */
class PlaybackDevice : public IPlaybackDevice
{
public:

    PlaybackDevice(shared_ptr<esplayer::IElementaryStreamManager> ESManager,
                   shared_ptr<ISystem> NrdSystem);
    virtual ~PlaybackDevice();

    virtual NFErr setDisplayArea(uint32_t x, uint32_t y,
                                 uint32_t width, uint32_t height);
    // deprecated
    virtual NFErr open(const std::vector<StreamInfo>& streams,
                       ContainerType containerType, DrmType drmType,
                       uint32_t maxWidth=0, uint32_t maxHeight=0);
    virtual NFErr open(ContainerType ContainerType);
    virtual NFErr initializePlayers();
    virtual NFErr initializeVideoPlayer(const std::vector<unsigned char>& headerData,
                                        const CommonVideoAttributesFromManifest &videoAttributes);

    virtual NFErr initializeAudioPlayer(const std::vector<unsigned char>& headerData,
                                        const CommonAudioAttributesFromManifest &audioAttributes);
    virtual void  close();
    // deprecated
    virtual NFErr feedStreamHeader(const std::vector<unsigned char>& headerData,
                                   uint32_t streamIndex,
                                   uint32_t manifestIndex,
                                   StreamType streamType);
    virtual NFErr feedVideoStreamHeader(const std::vector<unsigned char>& headerData,
                                        const VideoAttributesFromManifest& videoStreamInfo,
                                        uint32_t downloadableId);
    virtual NFErr feedAudioStreamHeader(const std::vector<unsigned char>& headerData,
                                        const AudioAttributesFromManifest& audioStreamInfo,
                                        uint32_t downloadableId);
    virtual NFErr feedDrmHeader(const std::vector<unsigned char>& drmHeader);
    virtual bool  drmHeaderReceived();
    virtual NFErr setStreamInfo(const std::vector<StreamInfo>& streams,
                                uint32_t manifestIndex);

    // deprecated
    virtual NFErr feedData(IBufferManager::Buffer& dataBuffer,
                           uint32_t streamIndex,
                           uint32_t manifestIndex,
                           StreamType streamType);
    virtual NFErr feedData(IBufferManager::Buffer& dataBuffer,
                           StreamType streamType,
                           uint32_t downloadableId);
    virtual NFErr setSpeed(int32_t speed);
    virtual NFErr flush();
    virtual NFErr flushAudio();

    virtual NFErr endOfStream(StreamType type);
    virtual NFErr getEvent(EventID& eventID, ullong& eventParam1,
                           ullong& eventParam2, std::string& eventParam3);
    virtual NFErr getChallenge(std::vector<unsigned char>& challengeData,
                               std::vector<unsigned char>& nflxHeader);
    virtual NFErr getChallenge(std::vector<unsigned char>& challengeData)
    {
        NRDP_UNUSED(challengeData);
        return NFErr_OK;
    }

    virtual NFErr storeLicense(const std::vector<unsigned char>& licenseData,
                               std::vector<unsigned char>& secureStopID,
                               uint32_t& drmSessionId);
    virtual NFErr clearLicense();
    virtual NFErr execute(uint32_t id, uint32_t& param1, uint32_t& param2);
    virtual Rect setVideoWindow(Rect, uint32_t);
    virtual void bringVideoToFront();
    virtual void sendVideoToBack();
    virtual void getPlaybackQualityStats(IPlaybackDevice::PlaybackQualityStats& audioStats,
                                         IPlaybackDevice::PlaybackQualityStats& videoStats);
    virtual double getVolume();
    virtual void setVolume(double, uint32_t, IAudioMixer::Ease);

    void enableAudio();


private:

    class  DataBlockCleanup : public IListElementCleanup<Mp4Demultiplexer::DataBlock>
    {
    public:
        void cleanup(Mp4Demultiplexer::DataBlock& block)
        {
            *(block.getDataBuffer().inUse_) = false;
        }
    };

    class ESPlayerCallback : public esplayer::IESPlayerCallback
    {
    public:
        ESPlayerCallback(PlaybackDevice& playbackDevice,
                          esplayer::MediaType mediaType);
        virtual ~ESPlayerCallback();

        // IElementaryStreamPlayer::Callback methods
        virtual esplayer::Status getNextMediaSample(esplayer::ISampleWriter&
                                                    sampleWriter);
        virtual esplayer::Status getNextMediaSampleSize(uint32_t& size,
            uint32_t viewNum = 0);
        virtual esplayer::Status getNextMediaSampleTimestamps(llong& dts,
                                                              llong& pts);
        virtual void updatePlaybackPosition(const llong& timestamp);
        virtual void reportUnderflow();
        virtual bool reportUnderflowReceived();
        virtual void playbackCompleted();
        virtual bool playbackCompletedReceived();
        void reportError(esplayer::Status errorCategory,
                         ullong errorCode,
                         const std::string& errorMessage);
        virtual void reportError(IDeviceError err);
        virtual void reportError(NFErr err);

        virtual void updateDecodingStats( int numDecoded,
                                          int numSkipped,
                                          int numReducedQualityDecodes,
                                          int numErrors);
        virtual void updateRenderingStats(int numRendered,
                                          int numDropped,
                                          int numErrors);
        // Methods for PlaybackDevice
        void     resetUnderflowReported();
        void     resetPlaybackCompleted();
        void     endFlush();
        void     disable();
        void     enable();
        void     beginFlush();  // sets mDisabled flag so that
                                // getNextMediaSample will not touch anything
                                // until endFlush() is called.

        esplayer::MediaType getMediaType();

        void getPlaybackQualityStats(PlaybackQualityStats& stats);

    private:

        esplayer::MediaType mMediaType;
        PlaybackDevice&     mPlaybackDevice;

        // These variables are shared between threads. You should hold the mutex
        // when accessing them.
        bool                 mDisabled;
        bool                 mUnderflowReported;
        bool                 mPlaybackComplete;
        Mutex          mMutex;
        PlaybackQualityStats mStatTotals;
    };

    /**
     * The vector of StreamInfos argument contains the basic characteristics of
     * all the video and audio streams that the PlaybackDevice may receive
     * during playback. The analyzeStreamInfo method creates a place holder for
     * each potential stream in PlaybackDevice's streamsAttributes_ array
     * and parses header data for any streamInfo that has it.  It returns with
     * the attributes of the video stream that had header data attached as well
     * as the maximum frame width and height for video streams.
     *
     * @param[in] streams: a vector of stream info for all the potential streams
     * that the playback device might receive via adaptive streaming during the
     * presentation.
     *
     * @param[out] primaryVideoAttributes: parsed header data for the first
     * video stream whose streamInfo has header data attached (as of 3.2 there
     * is only one with header attached.
     *
     * @param[out] maxImageWidth: the maximum width of the video streams.
     *
     * @param[out] maxImageHeight: the maximum height of the video streams.
     *
     */
    NFErr analyzeStreamInfo(const std::vector<StreamInfo>& streams,
                            uint32_t manifestIndex,
                            VideoAttributes*& primaryVideoAttr,
                            uint32_t& maxImageWidth,
                            uint32_t& maxImageHeight,
                            uint32_t& maxFrameRateScale,
                            uint32_t& maxFrameRateValue);

    /**
     * Parse the stream header data to derive attributes of a stream.
     *
     * @param[in] containerType the stream container type.
     * @param[in] streamType the stream type (audio or video).
     * @param[in] headerData the stream header data to be parsed.
     * @param[out] streamsAttr the retrieved audio/video stream attributes.
     * @param[out] drmHeader the retrieved DRM header. NULL if the retrieval
     *             of DRM header is not requested.
     *
     * @return NFErr_OK if successful, NFErr_Bad if something went wrong.
     */
    NFErr parseStreamHeader(ContainerType containerType,
                            StreamType streamType,
                            const std::vector<unsigned char>& headerData,
                            StreamAttributes& streamsAttr,
                            std::vector<unsigned char>* drmHeader);

    // Helpers
    bool reportUnderflow(); // returns true if the data exhausted event is
                            // successfully queued.
    void playbackCompleteReported(esplayer::MediaType type);
    void positionUpdateReported(const llong& pts, const ullong& monotimeInMS);
    void errorReported(esplayer::Status errorCategory,
                       ullong errorCode,
                       const std::string& errorMessage);

    NFErr initAudioPlayer();
    NFErr initVideoPlayer(VideoAttributes* videoAttr,
                          uint32_t maxImageWidth,
                          uint32_t maxImageHeight,
                          uint32_t maxFrameRateValue,
                          uint32_t maxFrameRateScale);
    void updateTransition();

    // Resources
    // There's a separate vector of StreamAttributes for each manifest.
    std::map<int,std::vector<StreamAttributes> >     streamsAttributes_;
    shared_ptr<EventQueue>            eventQueue_;
    shared_ptr<Demultiplexer>                        demuxer_;
    std::vector<unsigned char>                       drmHeader_;
    std::vector<unsigned char>                       nflxHeader_;
    shared_ptr<esplayer::IElementaryStreamManager>   ESManager_;
    shared_ptr<ISystem>                              NrdSystem_;
    esplayer::IPlaybackGroup*                        playbackGroup_;
    std::vector<unsigned char>                       secureStopID_;

    DataBlockCleanup mDataBlockCleanup;

    shared_ptr<ProducerConsumerList< Mp4Demultiplexer::DataBlock> >   streamDataBlocks_[2];
    shared_ptr<ESPlayerCallback>                    playerCallbacks_[2];
    esplayer::IElementaryStreamPlayer*              streamPlayers_[2];

    // State Variables
    bool     volatile endOfStream_[2];
    bool     volatile videoFrameStepping_;
    llong  volatile lastPostedPresentationTimestamp_;
    bool     volatile drmHeaderReceived_;

    int32_t  requested_speed_;

    // 3D format state
    esplayer::Format3D format3D_;

    // Others
    bool                initialized_;
    ContainerType       containerType_;
    VideoAttributes*    primaryVideoAttr_;
    uint32_t            maxImageWidth_;
    uint32_t            maxImageHeight_;
    uint32_t            maxFrameRateValue_;
    uint32_t            maxFrameRateScale_;
    double              targetVolume_;

    // After exhausted, we want to log a few feedData calls for each media type.
    uint32_t numRemainingFeedDatasToReport[2];
    std::map<int, std::vector<StreamInfo> > streams_;

    unsigned int mSessionId;
};

}} // namespace netflix::device

#endif // PLAYBACKDEVICENATIVE_H
