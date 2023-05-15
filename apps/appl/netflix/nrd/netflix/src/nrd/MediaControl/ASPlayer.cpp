/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable NFErrellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "ASPlayer.h"

#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>

#include "AdaptiveStreamingPlayer.h"

using namespace std;
using namespace netflix;
using namespace netflix::ase;
using namespace netflix::device;
using namespace netflix::mediacontrol;

namespace {

class Tracer
{
private:
    const char* const functionName_;

public:
    Tracer(const char* functionName) : functionName_(functionName)
    {
        NTRACE(TRACE_MEDIACONTROL, "ASPlayer %s() begin", functionName_);
        NRDP_UNUSED(functionName_);
    }

    ~Tracer()
    {
        NTRACE(TRACE_MEDIACONTROL, "ASPlayer %s() end", functionName_);
        NRDP_UNUSED(functionName_);
    }
};

}

ASPlayer::ASPlayer() :
    asPlayerImpl_(new AdaptiveStreamingPlayer),
    controlMutex_(ASPCONTROL_MUTEX, "ASPControl")
{
}

ASPlayer::~ASPlayer()
{
    close();
}

void ASPlayer::shutdown()
{
    asPlayerImpl_->shutdown();
}

shared_ptr<ase::StreamingStatReport> ASPlayer::obtainStreamingStat()
{
    return asPlayerImpl_->obtainStreamingStat();
}

void ASPlayer::getPlaybackQualityStats(device::IPlaybackDevice::PlaybackQualityStats& audioStats,
                                       device::IPlaybackDevice::PlaybackQualityStats& videoStats)
{
    return asPlayerImpl_->getPlaybackQualityStats(audioStats, videoStats);
}

NFErr ASPlayer::open(shared_ptr<ase::Manifest> pManifest,
                     shared_ptr<IPlaybackReporter> playbackReporter,
                     shared_ptr<LicenseHandler> licenseHandler, ullong sessionId,
                     netflix::Video3DType video3DType,
                     const std::string &defaultAudioTrack, netflix::StreamingType streamingType,
                     ContainerType containerType,
                     const IPlaybackDevice::CommonVideoAttributesFromManifest &videoAttributes,
                     const IPlaybackDevice::CommonAudioAttributesFromManifest &audioAttributes,
                     Ticks const& pts)
{
    Tracer tracer("open");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->open(pManifest, playbackReporter, licenseHandler, sessionId,
                               video3DType, defaultAudioTrack, streamingType,
                               containerType,
                               videoAttributes, audioAttributes, pts);
}


void ASPlayer::completeBuffering(uint32_t actualStartPts)
{
    NRDP_UNUSED(actualStartPts);
}

NFErr ASPlayer::feedDrmHeader(std::vector<unsigned char> const& drmHeader)
{
    NRDP_UNUSED(drmHeader);
    return NFErr_OK;
}

void ASPlayer::abort()
{
    Tracer tracer("abort");
    asPlayerImpl_->abort();
}

NFErr ASPlayer::addManifest(shared_ptr<Manifest> manifest)
{
    Tracer tracer("addManifest");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->addManifest(manifest);
}

void ASPlayer::close()
{
    Tracer tracer("close");
    ScopedMutex scopedMutexControl(controlMutex_);
    asPlayerImpl_->close();
}

NFErr ASPlayer::play(const Ticks& pts, bool playAndPause)
{
    Tracer tracer("play");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->play(pts, playAndPause);
}

NFErr ASPlayer::skip(const Ticks& pts)
{
    Tracer tracer("skip");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->skip(pts);
}

void ASPlayer::getBufferRange(Ticks& startPts, Ticks& endPts)
{
    Tracer tracer("getBufferRange");
//    ScopedMutex scopedMutexControl(controlMutex_);
    asPlayerImpl_->getBufferRange(startPts, endPts);
}

uint32_t ASPlayer::getPredictedFutureRebuffers() const
{
    Tracer tracer("getPredictedFutureRebuffers");
    return asPlayerImpl_->getPredictedFutureRebuffers();
}

NFErr ASPlayer::provideLicense(const std::vector<unsigned char>& license,
                               std::vector<unsigned char>& secureStopId,
                               uint32_t& drmSessionId)
{
    Tracer tracer("provideLicense");

    // Don't gate provideLicense with the controlMutex_
    // This is normally called by the pumpingThread while it is executing in
    // MediaControl::provideChallenge... so it should be safe not to grab
    // In the JS world, this would be called back to us after an nccp license is
    // retrieved, but that should be safe to call here as well without gating

    // if we try to grab the control mutex here, we can deadlock against a UI thread
    // calling stop() as that grabs the controlMutex and then blocks waiting for the
    // pumping thread to stop. The pumping thread would be stopped here waiting for
    // the control mutex so that would never happen.

    return asPlayerImpl_->provideLicense(license, secureStopId, drmSessionId);
}

NFErr ASPlayer::stop()
{
    Tracer tracer("stop");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->stop();
}

NFErr ASPlayer::pause()
{
    Tracer tracer("pause");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->pause();
}

NFErr ASPlayer::unpause()
{
    Tracer tracer("unpause");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->unpause();
}

NFErr ASPlayer::addListener(shared_ptr<Listener> l)
{
    Tracer tracer("addListener");
    return asPlayerImpl_->addListener(l);
}

NFErr ASPlayer::removeListener(shared_ptr<Listener> l)
{
    Tracer tracer("removeListener");
    return asPlayerImpl_->removeListener(l);
}

NFErr ASPlayer::selectAudioTrack(const std::string& trackId)
{
    Tracer tracer("selectTracks");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->selectAudioTrack(trackId);
}

void ASPlayer::getCurrentAudioTrack(std::string& trackId) const
{
    return asPlayerImpl_->getCurrentAudioTrack(trackId);
}

Ticks ASPlayer::getCurrentPts() const
{
    return asPlayerImpl_->getCurrentPts();
}

Ticks ASPlayer::getPlayDuration() const
{
    return asPlayerImpl_->getPlayDuration();
}

uint32_t ASPlayer::getGopSize() const
{
    return asPlayerImpl_->getGopSize();
}

IAdaptiveStreamingPlayer::State ASPlayer::getState() const
{
    return asPlayerImpl_->getState();
}

bool ASPlayer::endOfPresentation() const
{
    return asPlayerImpl_->endOfPresentation();
}

void ASPlayer::getDisplayAspectRatio(uint32_t& aspectRatioX,
                                     uint32_t& aspectRatioY) const
{
    asPlayerImpl_->getDisplayAspectRatio(aspectRatioX, aspectRatioY);
}

void ASPlayer::setVideoWindow(Rect targetRect, uint32_t transitionDuration)
{
    Tracer tracer("setVideoWindow");
    ScopedMutex scopedMutexControl(controlMutex_);
    asPlayerImpl_->setVideoWindow(targetRect, transitionDuration);
}

NFErr ASPlayer::bringVideoToFront()
{
    Tracer tracer("bringVideoToFront");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->bringVideoToFront();
}

NFErr ASPlayer::sendVideoToBack()
{
    Tracer tracer("sendVideoToBack");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->sendVideoToBack();
}

IAdaptiveStreamingPlayer::NetworkProfile ASPlayer::getNetworkProfile() const
{
    Tracer tracer("getNetworkProfile");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->getNetworkProfile();
}

void ASPlayer::setNetworkProfile(NetworkProfile nwProfile)
{
    Tracer tracer("setNetworkProfile");
    ScopedMutex scopedMutexControl(controlMutex_);
    asPlayerImpl_->setNetworkProfile(nwProfile);
}

NFErr ASPlayer::setVideoBitrateRanges(vector< shared_ptr<BitrateRange> > const& videoBitrateRanges)
{
    Tracer tracer("setVideoBitrateRanges");
    ScopedMutex scopedMutexControl(controlMutex_);
    if ( !videoBitrateRanges.empty() )
        return asPlayerImpl_->setVideoBitrateRanges(videoBitrateRanges);
    else
        return NFErr_NotAllowed;
}

uint32_t ASPlayer::getCurrentBandwidth() const
{
    Tracer tracer("getCurrentBandwidth");
    return asPlayerImpl_->getCurrentBandwidth();
}

uint32_t ASPlayer::getMaxStreamingBuffer() const
{
    Tracer tracer("getMaxStreamingBuffer");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->getMaxStreamingBuffer();
}

void ASPlayer::setStreamingBuffer(bool powerSaving,
                                    uint32_t maxBufferLen,
                                    uint32_t minBufferLen)
{
    Tracer tracer("setStreamingBuffer");
    ScopedMutex scopedMutexControl(controlMutex_);
    asPlayerImpl_->setStreamingBuffer(powerSaving, maxBufferLen, minBufferLen);
}


void ASPlayer::getMediaBufferPoolSize(uint32_t& videoBufferPoolSize, uint32_t& audioBufferPoolSize)
{
    Tracer tracer("getMediaBufferPoolSize");
    asPlayerImpl_->getMediaBufferPoolSize(videoBufferPoolSize, audioBufferPoolSize);
}

void ASPlayer::networkChange( NetworkInterfaceInfo const& netinfo )
{
    Tracer tracer("networkChange");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->networkChange( netinfo );
}

void ASPlayer::externalIpAddressChange( std::string const& ipaddr )
{
    Tracer tracer("externalIpAddressChange");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->externalIpAddressChange( ipaddr );
}

Variant ASPlayer::getNetworkMonitorState()
{
    Tracer tracer("getNetworkMonitorState");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->getNetworkMonitorState();
}

void ASPlayer::setNetworkMonitorState( Variant const& state )
{
    Tracer tracer("setNetworkMonitorState");
    ScopedMutex scopedMutexControl(controlMutex_);
    asPlayerImpl_->setNetworkMonitorState( state );
}

void ASPlayer::setNetworkMonitorListener( shared_ptr<INetworkMonitorListener> pListener )
{
    Tracer tracer("setNetworkMonitorListener");
    ScopedMutex scopedMutexControl(controlMutex_);
    asPlayerImpl_->setNetworkMonitorListener( pListener );
}

NFErr ASPlayer::cachePrepare(shared_ptr<ase::Manifest> pManifest,
                             netflix::Video3DType video3DType,
                             const std::string &defaultAudioTrack,
                             shared_ptr<IPlaybackReporter> playbackReporter)
{
    Tracer tracer("cachePrepare");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->cachePrepare(pManifest, video3DType, defaultAudioTrack,
                                       playbackReporter);
}

void ASPlayer::cacheFlush()
{
    Tracer tracer("cacheFlush");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->cacheFlush();
}

std::vector<std::string> ASPlayer::cacheList()
{
    Tracer tracer("cacheList");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->cacheList();
}

void ASPlayer::cacheSetSize(int maxItems)
{
    Tracer tracer("cacheList");
    ScopedMutex scopedMutexControl(controlMutex_);
    asPlayerImpl_->cacheSetSize(maxItems);
}

double ASPlayer::getVolume()
{
    Tracer Tracer("getVolume");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->getVolume();
}

void ASPlayer::setVolume(double targetVolume, uint32_t transitionDuration, IAudioMixer::Ease ease)
{
    Tracer Tracer("setVolume");
    ScopedMutex scopedMutexControl(controlMutex_);
    return asPlayerImpl_->setVolume(targetVolume, transitionDuration, ease);
}
