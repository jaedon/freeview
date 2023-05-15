/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MEDIASOURCE_PLAYBACKDEVICE_H
#define MEDIASOURCE_PLAYBACKDEVICE_H

#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <map>

#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/NrdpMediaSource.h>
#include <nrd/ISystem.h>
#include "EventQueue.h"

namespace netflix {
namespace device {

/**
 * @class MediaSourcePlaybackDevice MediaSourcePlaybackDevice.h
 * @brief Implementation of device playback functionality in native
 *        linux environment.
 */
class MediaSourcePlaybackDevice
{
public:

    MediaSourcePlaybackDevice(shared_ptr<esplayer::IElementaryStreamManager> ESManager,
                              shared_ptr<ISystem> NrdSystem);

    virtual ~MediaSourcePlaybackDevice();

    /*
     * open playback device with information received from NCCP. These
     * information is not available from stream, but manifest from NCCP only
     * knows reliably.
     *
     * when this is called, playback group will be created. when creating
     * playback group, we can pass basic codec information so that playback
     * group can take adavantage of open decoders ahead of time without waiting
     * first stream header(track header) arrives
     */
    NFErr open(const device::IPlaybackDevice::CommonVideoAttributesFromManifest &videoAttributes,
               const device::IPlaybackDevice::CommonAudioAttributesFromManifest &audioAttributes,
               uint32_t videoPipelineId);

    void  close();

    /*
     * IESPlayerCallback can access Sample retrieval method once NrdpMediaSource is set
     */
    void setMediaSource(shared_ptr<mediacontrol::NrdpMediaSource> pMediaSource);

    shared_ptr<mediacontrol::NrdpMediaSource> getMediaSource();

    /*
     * playback control
     */
    NFErr setSpeed(int32_t speed);

    NFErr flush();

    NFErr flushAudio();

    void enableAudio();

    NFErr endOfStream(IPlaybackDevice::StreamType type);

    NFErr getEvent(IPlaybackDevice::EventID& eventID,
                   ullong& eventParam1,
                   ullong& eventParam2,
                   std::string& eventParam3);

    /**
     * presentation control
     */
    Rect setVideoWindow(Rect, uint32_t, uint32_t);

    double getVolume();

    void setVolume(double, uint32_t, IAudioMixer::Ease);

    /**
     * Informational
     */
    void getPlaybackQualityStats(IPlaybackDevice::PlaybackQualityStats& audioStats,
                                 IPlaybackDevice::PlaybackQualityStats& videoStats);

    /**
     * event notification
     */
    bool reportUnderflow(); // returns true if the data exhausted event is
                            // successfully queued.

    void playbackCompleteReported(esplayer::MediaType type);

    void positionUpdateReported(const llong& pts, const ullong& monotimeInMS);

    void errorReported(esplayer::Status errorCategory,
                       ullong errorCode,
                       const std::string& errorMessage);
private:

    class ESPlayerCallback : public esplayer::IESPlayerCallback
    {
    public:
        ESPlayerCallback(MediaSourcePlaybackDevice& playbackDevice,
                         esplayer::MediaType mediaType);
        virtual ~ESPlayerCallback();

        esplayer::MediaType getMediaType();

        // IElementaryStreamPlayer::Callback methods
        virtual esplayer::Status getNextMediaSample(esplayer::ISampleWriter& sampleWriter);
        virtual esplayer::Status getNextMediaSampleSize(uint32_t& size, uint32_t viewNum = 0);
        virtual esplayer::Status getNextMediaSampleTimestamps(llong& dts, llong& pts);
        virtual void updatePlaybackPosition(const llong& timestamp);
        virtual void reportUnderflow();
        virtual bool reportUnderflowReceived();
        virtual void playbackCompleted();
        virtual bool playbackCompletedReceived();

        void reportError(esplayer::Status errorCategory, ullong errorCode, const std::string& errorMessage);
        virtual void reportError(IDeviceError err);
        virtual void reportError(NFErr err);

        // Methods for PlaybackDevice
        void resetUnderflowReported();
        void resetPlaybackCompleted();
        void endFlush();
        void disable();
        void enable();
        void beginFlush(); // sets mDisabled flag so that
                           // getNextMediaSample will not touch anything
                           // until endFlush() is called.

        void getPlaybackQualityStats(IPlaybackDevice::PlaybackQualityStats& stats);
        virtual void updateDecodingStats( int numDecoded,
                                          int numSkipped,
                                          int numReducedQualityDecodes,
                                          int numErrors);
        virtual void updateRenderingStats(int numRendered,
                                          int numDropped,
                                          int numErrors);
    private:
        esplayer::MediaType mMediaType;
        MediaSourcePlaybackDevice& mMediaSourcePlaybackDevice;
        esplayer::SampleAttributes mSampleAttributes;

        // These variables are shared between threads. You should hold the mutex
        // when accessing them.
        bool mDisabled;
        bool mUnderflowReported;
        bool mPlaybackComplete;
        Mutex mMutex;
        IPlaybackDevice::PlaybackQualityStats mStatTotals;
    };

private:
    // nrdlib object
    shared_ptr<EventQueue>                           mEventQueue;
    shared_ptr<mediacontrol::NrdpMediaSource>        mMediaSource;
    // dpi object
    shared_ptr<ISystem>                              mNrdSystem;
    shared_ptr<esplayer::IElementaryStreamManager>   mESManager;
    shared_ptr<ESPlayerCallback>                     mPlayerCallback[2];
    esplayer::IPlaybackGroup*                        mPlaybackGroup;
    esplayer::IElementaryStreamPlayer*               mStreamPlayer[2];

    // internal state
    esplayer::Format3D format3D_;
    bool mPlaybackDeviceInitialized;
    bool mEndOfStream[2];
    llong mLastPostedPresentationTimestamp;
    double mTargetVolume;
};

}} // namespace netflix::device

#endif // PLAYBACKDEVICENATIVE_H
