/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ASPLAYER_H
#define ASPLAYER_H

#include "IAdaptiveStreamingPlayer.h"
#include "IPlaybackReporter.h"
#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>

namespace netflix {

namespace mediacontrol {

class ASPlayer : public IAdaptiveStreamingPlayer
{
public:
    ASPlayer();
    virtual ~ASPlayer();

    virtual void shutdown();

    virtual shared_ptr<ase::StreamingStatReport> obtainStreamingStat( );
    virtual void getPlaybackQualityStats(device::IPlaybackDevice::PlaybackQualityStats& audioStats,
                                         device::IPlaybackDevice::PlaybackQualityStats& videoStats);
    virtual NFErr open(shared_ptr<ase::Manifest> pManifest,
                       shared_ptr<IPlaybackReporter> playbackReporter,
                       shared_ptr<LicenseHandler> licensehandler, ullong sessionId,
                       netflix::Video3DType video3DType,
                       const std::string &defaultAudioTrack, netflix::StreamingType streamingType,
                       ContainerType containerType,
                       const device::IPlaybackDevice::CommonVideoAttributesFromManifest &videoAttributes,
                       const device::IPlaybackDevice::CommonAudioAttributesFromManifest &audioAttributes,
                       Ticks const& pts);
    virtual void completeBuffering(uint32_t actualStartPts);
    virtual NFErr feedDrmHeader(std::vector<unsigned char> const& drmHeader);

    virtual void abort();
    virtual NFErr addManifest(shared_ptr<ase::Manifest> pManifest);
    virtual void close();
    virtual NFErr play(const Ticks& pts, bool playAndPause );
    virtual NFErr skip(const Ticks& pts);
    virtual void getBufferRange(Ticks& startPts, Ticks& endPts);
    virtual uint32_t getPredictedFutureRebuffers() const;
    virtual NFErr provideLicense(const std::vector<unsigned char>& license,
                                 std::vector<unsigned char>& secureStopId,
                                 uint32_t& drmSessionId);
    virtual NFErr stop();
    virtual NFErr pause();
    virtual NFErr unpause();
    virtual NFErr addListener(shared_ptr<Listener> l);
    virtual NFErr removeListener(shared_ptr<Listener> l);
    virtual NFErr selectAudioTrack(const std::string& trackId );
    virtual void getCurrentAudioTrack(std::string &) const;

    // Two NrdpVideo APIs
    virtual void startAudioTrackSwitch() {}
    virtual void completeAudioTrackSwitch()  {}

    virtual Ticks getCurrentPts() const;
    virtual Ticks getPlayDuration() const;
    virtual uint32_t getGopSize() const;
    virtual State getState() const;
    virtual bool endOfPresentation() const;
    virtual void getDisplayAspectRatio(uint32_t& aspectRatioX,
                                       uint32_t& aspectRatioY) const;
    virtual void setVideoWindow(Rect targetRect, uint32_t transitionDuration);
    virtual NFErr bringVideoToFront();
    virtual NFErr sendVideoToBack();

    virtual NetworkProfile getNetworkProfile() const;
    virtual void setNetworkProfile(NetworkProfile nwProfile);
    virtual NFErr setVideoBitrateRanges(std::vector< shared_ptr<netflix::ase::BitrateRange> > const& videoBitrateRanges);
    virtual uint32_t getCurrentBandwidth() const;
    virtual uint32_t getMaxStreamingBuffer() const;
    virtual void setStreamingBuffer(bool powerSaving,
                                    uint32_t maxBufferLen,
                                    uint32_t minBufferLen);
    virtual void getMediaBufferPoolSize(uint32_t& videoBufferPoolSize, uint32_t& audioBufferPoolSize);
    virtual void networkChange( NetworkInterfaceInfo const& netinfo );
    virtual void externalIpAddressChange( std::string const& ipaddr );
    virtual Variant getNetworkMonitorState();
    virtual void setNetworkMonitorState( Variant const& state );
    virtual void setNetworkMonitorListener( shared_ptr<ase::INetworkMonitorListener> pListener );
    virtual NFErr cachePrepare(shared_ptr<ase::Manifest> manifest,
                               netflix::Video3DType video3DType,
                               const std::string &defaultAudioTrack,
                               shared_ptr<IPlaybackReporter> playbackReporter);
    virtual void cacheFlush();
    virtual std::vector<std::string> cacheList();
    virtual void cacheSetSize(int maxItems);
    virtual double getVolume();
    virtual void setVolume(double targetVolume, uint32_t transitionDuration, device::IAudioMixer::Ease ease);

private:
    shared_ptr<IAdaptiveStreamingPlayer> asPlayerImpl_;
    mutable Mutex controlMutex_;
};

}} // namespace netflix::mediacontrol

#endif // ASPLAYER_H
