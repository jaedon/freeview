/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "PlayerBridge.h"

#include <nrdbase/Base64.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/tr1.h>
#include <nrdbase/LexicalCast.h>
#include <nrdbase/Application.h>

#include <nrd/NrdApplication.h>
#include <nrd/IPlaybackDevice.h>

#include <nrdase/AseTimeStamp.h>

#include "NrdConfiguration.h"
#include "NrdpBridge.h"
#include "MediaRequestBridge.h"
#include "TransportReporter.h"

#include <nrdase/ILocationSetMonitor.h>
#include "ManifestUtilities.h"
#include "ASPlayer.h"

using namespace netflix;
using namespace netflix::ase;
using namespace netflix::device;
using namespace netflix::mediacontrol;
using namespace netflix::util;

#define DevicePropertyList(Property)                            \
    Property(allowNrdpVideoSwitch, NfObject::ReadOnly)          \
    Property(bufferPoolSize, NfObject::ReadOnly)                \
    Property(currentAudioTrack, NfObject::ReadWrite)            \
    Property(displayAspectRatio, NfObject::ReadOnly)            \
    Property(duration, NfObject::ReadOnly)                      \
    Property(gopSize, NfObject::ReadOnly)                       \
    Property(maxStreamingBuffer, NfObject::ReadOnly)            \
    Property(networkProfile, NfObject::ReadWrite)               \
    Property(state, NfObject::ReadOnly)

PropertyTable(PlayerBridge, DevicePropertyList, properties)

#define PROPUPS "_propups"

#define MethodList(Method)                      \
    Method(addManifest)                         \
    Method(bringVideoToFront)                   \
    Method(cacheFlush)                          \
    Method(cacheList)                           \
    Method(cachePrepare)                        \
    Method(cacheSetSize)                        \
    Method(close)                               \
    Method(completeAudioTrackSwitch)            \
    Method(externalIpAddressChange)             \
    Method(getBufferRange)                      \
    Method(getVolume)                           \
    Method(notifyBufferingComplete)             \
    Method(obtainPlaybackStat)                  \
    Method(obtainStreamingStat)                 \
    Method(open)                                \
    Method(pause)                               \
    Method(play)                                \
    Method(provideLicense)                      \
    Method(sendVideoToBack)                     \
    Method(setStreamingBuffer)                  \
    Method(setVideoBitrateRanges)               \
    Method(setVideoWindow)                      \
    Method(setVolume)                           \
    Method(skip)                                \
    Method(startAudioTrackSwitch)               \
    Method(stop)                                \
    Method(swim)                                \
    Method(unpause)

MethodTable(PlayerBridge, MethodList)

static inline Variant variantFromTicks(const Ticks &ticks)
{
    return ticks.valid() ? Variant(static_cast<int>(ticks.value(Ticks::RES_1KHZ))) : Variant();
}

#if 0
static std::string mediaTypeToString( MediaType mediaType )
{
    switch(mediaType)
    {
        case MEDIA_AUDIO:
            return "audio";
        case MEDIA_VIDEO:
            return "video";
        case MEDIA_HEADERS:
            return "headers";
        case MEDIA_TEXT:
            return "timedtext";
        default:
            break;
    }

    return "unknown";
}
#endif

#if !defined(BUILD_DEBUG)
# define CHECK_EVENT_NAME(eventName) eventName
#else
    static inline const char * checkEventName(const char * eventName, const char * functionName)
    {

#if defined(NRDP_PLATFORM_X360) || defined(NRDP_PLATFORM_XB1)
            const char * baseFunctionName = strrchr(functionName, ':');
            assert(baseFunctionName && !strcmp(eventName, baseFunctionName + 1));
#else
            assert(!strcmp(eventName, functionName));
#endif

        return eventName;
    }

#define CHECK_EVENT_NAME(eventName) checkEventName(eventName, __FUNCTION__)
#endif

//////////
//
// PlayerBridge::ASPListener
//
//////////
class PlayerBridge::ASPListener : public mediacontrol::IAdaptiveStreamingPlayer::Listener
{
public:
    ASPListener(PlayerBridge *bridge) :
        mMutex(PLAYERBRIDGE_MUTEX, "PlayerBridge::MCListener"),
        mBridge(bridge)
    {
    }
    virtual ~ASPListener() {}

    virtual void stateChanged(ullong sessionId,
                              mediacontrol::IAdaptiveStreamingPlayer::State state)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->stateChanged(sessionId, state);
    }
    virtual void openComplete(ullong sessionId)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->openComplete(sessionId);
    }
    virtual void streamSelected(ullong /*sessionId*/, int manifest, int track, int stream,
                                const Ticks &pts, uint32_t rebuffer)
    {
        ScopedMutex lock(mMutex);
        if (mBridge) {
            Variant map;
            map["type"] = "streamselected";
            map["manifestIndex"] = manifest;
            map["trackIndex"] = track;
            map["streamIndex"] = stream;
            map["startPts"] = variantFromTicks(pts);
            map["rebuffer"] = rebuffer;
            mBridge->sendPlayerEvent(map);
        }
    }
    virtual void buffering(ullong /*sessionId*/, uint32_t percentage)
    {
        ScopedMutex lock(mMutex);
        if (mBridge) {
            Variant map;
            map["percentage"] = percentage;
            map["type"] = "buffering";
            mBridge->sendPlayerEvent(map);
        }
    }
    virtual void bufferingComplete(ullong sessionId, bool faststart)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->bufferingComplete(sessionId, faststart);
    }
    virtual void streamPresenting(ullong /*sessionId*/, int manifest, int track, int stream,
                                  const Ticks &pts)
    {
        ScopedMutex lock(mMutex);
        if (mBridge) {
            Variant map;
            map["type"] = "streampresenting";
            map["manifestIndex"] = manifest;
            map["trackIndex"] = track;
            map["streamIndex"] = stream;
            map["startPts"] = variantFromTicks(pts);
            mBridge->sendPlayerEvent(map);
        }
    }
    virtual void endOfStream(ullong /*sessionId*/)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->sendPlayerEvent(Variant("type", "endofstream"));
    }
    virtual void underflow(ullong /*sessionId*/)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->sendPlayerEvent(Variant("type", "underflow"));
    }
    virtual void skipped(ullong /*sessionId*/)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->sendPlayerEvent(Variant("type", "skipped"));
    }
    virtual void updatePts(ullong sessionId, const Ticks &pts, const ullong& monotimeInMS)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->updatePts(sessionId, pts, monotimeInMS);
    }
    virtual void updateContentDuration(ullong sessionId, uint32_t duration)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->updateContentDuration(sessionId, duration);
    }
    virtual void updateVideoBitrate(ullong /*sessionId*/, uint32_t bps)
    {
        ScopedMutex lock(mMutex);
        if (mBridge) {
            Variant map;
            map["bitsPerSecond"] = bps;
            map["type"] = "updatevideobitrate";
            mBridge->sendPlayerEvent(map);
        }
    }
    virtual void audioTrackChanged(ullong sessionId, uint32_t trackIndex, const std::string &trackId)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->audioTrackChanged(sessionId, trackIndex, trackId);
    }

    virtual void audioTrackChangeStarted(ullong sessionId)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->audioTrackChangeStarted(sessionId);
    }

    virtual void audioTrackChangeRejected(ullong sessionId)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->audioTrackChangeRejected(sessionId);
    }

    virtual void audioTrackChangeComplete(ullong sessionId)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->audioTrackChangeComplete(sessionId);
    }



    virtual void videoBitrateSelectionCompleted(ullong /*sessionId*/)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->sendPlayerEvent(Variant("type", "videobitratechanged"));
    }
    virtual void videoWindowChanged(ullong /*sessionId*/, const Rect& rect)
    {
        ScopedMutex lock(mMutex);
        if (mBridge) {
            Variant map;
            map["x"] = rect.x;
            map["y"] = rect.y;
            map["width"] = rect.width;
            map["height"] = rect.height;
            map["type"] = "videowindowchanged";
            mBridge->sendPlayerEvent(map);
        }
    }
    virtual void error(ullong /*sessionId*/, const NFErr &err)
    {
        ScopedMutex lock(mMutex);
        if (mBridge) {
            Variant map;
            map["type"] = "error";
            map["error"] = static_cast<int>(err.peekCode());
            map["stack"] = err.toVariant();
            mBridge->sendPlayerEvent(map);
        }
    }
    virtual void restartPlayback(ullong sessionId, const NFErr &err, const Ticks &pts)
    {
        ScopedMutex lock(mMutex);
        if (mBridge) {
            Variant vm;
            vm["type"] = "restart";
            vm["sessionId"] = lexical_cast<std::string>(sessionId);
            vm["error"] = static_cast<int>(err.peekCode());
            vm["stack"] = err.toVariant();
            vm["pts"] = variantFromTicks(pts);
            mBridge->sendPlayerEvent(vm);
        }
    }
    virtual void manifestSelected(ullong sessionId, int manifestIndex)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->manifestSelected(sessionId, manifestIndex);
    }

    virtual void hevcInfo(ullong sessionId, const std::string &info)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->hevcInfo(sessionId, info);
    }

    void reset()
    {
        ScopedMutex lock(mMutex);
        mBridge = NULL;
    }

private:
    Mutex mMutex;
    PlayerBridge *mBridge;
};

//////////
//
// PlayerBridge::ASPLicenseHandler
//
//////////
class PlayerBridge::ASPLicenseHandler :
    public mediacontrol::IAdaptiveStreamingPlayer::LicenseHandler
{
public:
    ASPLicenseHandler(PlayerBridge *bridge) :
        mMutex(PLAYERBRIDGE_ASPLICENSE_MUTEX, "PlayerBridgeASPLicenseHandler"),
        mBridge(bridge)
    {
    }
    virtual ~ASPLicenseHandler() {}

    virtual NFErr provideChallenge(std::string const& drmType,
                                   std::string const& streamID,
                                   const std::vector<unsigned char>& challenge,
                                   const std::vector<unsigned char>& drmHeader)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            return mBridge->provideChallenge(drmType, streamID, challenge, drmHeader);
        else
            return NFErr_Bad;
    }

    virtual NFErr provideChallenge(const std::vector<unsigned char>& challenge)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            return mBridge->provideChallenge(challenge);
        else
            return NFErr_Bad;
    }


    virtual void provideRelease(std::string const& drmType, std::string const& release)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->provideRelease(drmType, release);
    }
    virtual void notifyBufferingComplete()
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->notifyBufferingComplete();
    }
    virtual NFErr checkStatus()
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            return mBridge->checkStatus();
        else
            return NFErr_Bad;
    }

    void reset()
    {
        ScopedMutex lock(mMutex);
        mBridge = NULL;
    }

private:
    Mutex mMutex;
    PlayerBridge *mBridge;
};


//////////
//
// PlayerBridge::PlaybackReporter
//
//////////
class PlayerBridge::PlaybackReporter : public mediacontrol::IPlaybackReporter, public TransportReporter
{
public:
    PlaybackReporter(const shared_ptr<PlayerBridge> &bridge) :
        mMutex(PLAYBACKREPORTERBRIDGE_MUTEX, "PlayerBridgePlaybackReporter"),
        mBridge(bridge)
    {
    }
    virtual ~PlaybackReporter() {}

    // The following are called by the player but have an empty implementation in PlaybackReporter
    virtual void openCompleted(uint32_t /*duration*/, uint32_t /*gopSize*/)
    {
       // Do nothing
    }
    virtual void updateContentDuration(uint32_t /*duration*/)
    {
       // Do nothing
    }
    virtual void notifyManifestSelected(uint32_t manifestIndex)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["manifestIndex"] = manifestIndex;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("notifyManifestSelected"), args);
        }
    }
    virtual void updateStreamingStat( shared_ptr<StreamingStatReport> /*pStreamingStat*/)
    {
        // Do nothing
    }
    virtual void flushComplete(MediaType /*mediaType*/)
    {
        // Do nothing
    }
    virtual void videoBitrateSelectionCompleted()
    {
        // Do nothing
    }

    virtual void setLicenseTime(ullong licenseTimeMS)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["licenseTimeMS"] = licenseTimeMS;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("setLicenseTime"), args);
        }
    }
    virtual void playbackStateChange(const AseTimeStamp &movieTime,
                                     IMediaPipelineReporter::PlaybackState newState)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["movieTime"] = (int)AseTimeStamp(movieTime, 1000).getValueInMs();
            args["state"] =  newState;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("playbackStateChange"), args);
        }
    }
    virtual void playbackStarted(std::string const& additionalInfo,
                                 Time const& eventTime)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["additionalInfo"] = additionalInfo;
            args["eventTime"] =  eventTime.ms();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("playbackStarted"), args);
        }
    }
    virtual void playbackFailure(NFErr nferr)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["nferr"] = nferr.toVariant();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("playbackFailure"), args);
        }
    }
    virtual void playbackStreamChange(int32_t manifestIndex,
                                      int32_t trackIndex, int32_t streamIndex,
                                      const AseTimeStamp &movieTime)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["nativetime"] = Time::mono().ms();
            args["manifestIndex"] =  manifestIndex;
            args["trackIndex"] =  trackIndex;
            args["streamIndex"] =  streamIndex;
            args["movieTime"] = (int)movieTime.getValueInMs();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("playbackStreamChange"), args);
        }
    }
    virtual void playbackReposition(const AseTimeStamp &movieTime)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["movieTime"] = (int)AseTimeStamp(movieTime, 1000).getValueInMs();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("playbackReposition"), args);
        }
    }
    virtual void updatePlaybackPts(const AseTimeStamp &movieTime,
                                   uint32_t vcdnid,
                                   uint32_t acdnid)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["movieTime"] = (int)AseTimeStamp(movieTime, 1000).getValueInMs();
            args["vcdnid"] =  vcdnid;
            args["acdnid"] =  acdnid;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("updatePlaybackPts"), args);
        }
    }
    virtual void playbackStopped(NFErr reason, const AseTimeStamp &movieTime,
                                 Time const& eventTime )
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["movieTime"] = (int)AseTimeStamp(movieTime, 1000).getValueInMs();
            args["reason"] =  reason.toVariant();
            args["eventTime"] =  eventTime.ms();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("playbackStopped"), args);
        }
    }
    virtual void playbackClosed(NFErr reason, const AseTimeStamp &movieTime)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["movieTime"] = (int)AseTimeStamp(movieTime, 1000).getValueInMs();
            args["reason"] =  reason.toJsonString();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("playbackClosed"), args);
        }
    }
    virtual void bufferingStarted()
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("bufferingStarted"));
        }
    }
    virtual void skipStarted()
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("skipStarted"));
        }
    }
    virtual void bufferingComplete(uint32_t actualStartPts,
                                   ullong predictedBufferingTime,
                                   uint32_t initialBW, uint32_t actualBW)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["actualStartPts"] = actualStartPts;
            args["predictedBufferingTime"] = predictedBufferingTime;
            args["initialBW"] = initialBW;
            args["actualBW"] = actualBW;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("bufferingComplete"), args);
        }
    }
    virtual void bufferingStatus(uint32_t totalBufferingTime,
                                 uint32_t currentBufferingTime)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["totalBufferingTime"] = totalBufferingTime;
            args["currentBufferingTime"] = currentBufferingTime;
            bridge->PlaybackReporterEvent("bufferingStatus", args);
        }
    }
    virtual void streamingFailureReasons(AseErrorCode networkErrorCode,
                                         int32_t httpError)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["networkErrorCode"] = networkErrorCode;
            args["httpError"] = httpError;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("streamingFailureReasons"), args);
        }
    }
    virtual void streamingFailure(int32_t manifestIndex,
                                  int32_t trackIndex, int32_t streamIndex,
                                  AseErrorCode aserr, int32_t httpError,
                                  const std::string &errorMsg)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["manifestIndex"] = manifestIndex;
            args["trackIndex"] = trackIndex;
            args["streamIndex"] = streamIndex;
            args["aserr"] = aserr;
            args["httpError"] = httpError;
            args["errorMsg"] = errorMsg;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("streamingFailure"), args);
        }
    }
    virtual void reportStreamingServerFailure(uint32_t cdnId)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["cdnId"] = cdnId;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("reportStreamingServerFailure"), args);
        }
    }
    virtual void streamSelected(int32_t manifestIndex,
                                int32_t trackIndex, int32_t streamIndex,
                                const AseTimeStamp &movieTime,
                                uint32_t bandwidth, uint32_t longtermBw,
                                uint32_t rebuffer)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["nativetime"] = Time::mono().ms();
            args["manifestIndex"] = manifestIndex;
            args["trackIndex"] = trackIndex;
            args["streamIndex"] = streamIndex;
            args["movieTime"] = movieTime.getValueInMs();
            args["bandwidth"] = bandwidth;
            args["longtermBw"] = longtermBw;
            args["rebuffer"] = rebuffer;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("streamSelected"), args);
        }
    }
    virtual void updateStreamingPts(int32_t manifestIndex, int32_t trackIndex,
                                    const AseTimeStamp &movieTime)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["manifestIndex"] = manifestIndex;
            args["trackIndex"] = trackIndex;
            args["movieTime"] = movieTime.getValueInMs();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("updateStreamingPts"), args);
        }
    }
    virtual void updateBufferLevel(uint32_t abuflbytes,
                                   uint32_t vbuflbytes,
                                   uint32_t predictedFutureRebuffers,
                                   uint32_t currentBandwidth)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["abuflbytes"] = abuflbytes;
            args["vbuflbytes"] = vbuflbytes;
            args["predictedFutureRebuffers"] = predictedFutureRebuffers;
            args["currentBandwidth"] = currentBandwidth;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("updateBufferLevel"), args);
        }
    }
    virtual void serverSelected(uint32_t selcdnid,
                                std::string const& selcdnname,
                                uint32_t selcdnrtt,
                                uint32_t selcdnbw,
                                std::string const& domainBwData,
                                std::string const& testreason,
                                std::string const& selreason,
                                uint32_t fastselthreshold)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["selcdnid"] = selcdnid;
            args["selcdnname"] = selcdnname;
            args["selcdnrtt"] = selcdnrtt;
            args["selcdnbw"] = selcdnbw;
            args["domainBwData"] = domainBwData;
            args["testreason"] = testreason;
            args["selreason"] = selreason;
            args["fastselthreshold"] = fastselthreshold;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("serverSelected"), args);
        }
    }
    virtual void audioTrackSwitchStarted(
        std::string const& oldLangCode, uint32_t oldNumChannels,
        std::string const& newLangCode, uint32_t newNumChannels)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["oldLangCode"] = oldLangCode;
            args["oldNumChannels"] = oldNumChannels;
            args["newLangCode"] = newLangCode;
            args["newNumChannels"] = newNumChannels;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("audioTrackSwitchStarted"), args);
        }
    }
    virtual void audioTrackSwitchComplete()
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("audioTrackSwitchComplete"));
        }
    }
    virtual void connectionSelected1(uint32_t origmaxnum, uint32_t actualmaxnum,
                                     uint32_t rtt)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["origmaxnum"] = origmaxnum;
            args["actualmaxnum"] = actualmaxnum;
            args["rtt"] = rtt;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("connectionSelected1"), args);
        }
    }
    virtual void connectionSelected2(uint32_t defaultnumconn, uint32_t selnumconn,
                                     uint32_t maxnumconn)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["defaultnumconn"] = defaultnumconn;
            args["selnumconn"] = selnumconn;
            args["maxnumconn"] = maxnumconn;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("connectionSelected2"), args);
        }
    }
    virtual void reportKeyStartplayEvent(std::string const& eventName)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["eventName"] = eventName;
            args["nativetime"] = Time::mono().ms();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("reportKeyStartplayEvent"), args);
        }
    }
    virtual void pipeliningDisabled()
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("pipeliningDisabled"));
        }
    }
    virtual void serverSwitch(
        uint32_t                        cdnid,
        shared_ptr<ILocationSetMonitor> pLocationSetMonitor,
        IStreamingReporter::ServerSelectionReason           reason,
        uint32_t                        bitrate)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            Variant locationSetMonitor;
            locationSetMonitor["key"] = pLocationSetMonitor->getLocationSetKey();
            locationSetMonitor["throughput"] = pLocationSetMonitor->getShortTermThroughput(AseTimeVal::now());
            locationSetMonitor["confidence"] = pLocationSetMonitor->getConfidence();
            args["cdnid"] = cdnid;
            args["pLocationSetMonitor"] = locationSetMonitor;
            args["reason"] = reason;
            args["bitrate"] = bitrate;
            args["nativetime"] = Time::mono().ms();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("serverSwitch"), args);
        }
    }
    virtual void reportFileToken( std::vector<unsigned char> const& filetoken )
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["filetoken"] = DataBuffer( filetoken ).toBase64().toString();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("reportFileToken"), args);
        }
    }

    virtual void setPlaybackQualityStats(const IPlaybackDevice::PlaybackQualityStats& audioStats,
                                                   const IPlaybackDevice::PlaybackQualityStats& videoStats)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            Variant aStats;
            Variant vStats;
            //TODO covert args to JSON
            aStats["mNumDecoded"] = audioStats.mNumDecoded;
            aStats["mNumDecoderErrors"] = audioStats.mNumDecoderErrors;
            aStats["mNumDropped"] = audioStats.mNumDropped;
            aStats["mNumReducedQualityDecodes"] = audioStats.mNumReducedQualityDecodes;
            aStats["mNumRendered"] = audioStats.mNumRendered;
            aStats["mNumRendererErrors"] = audioStats.mNumRendererErrors;
            aStats["mNumSkipped"] = audioStats.mNumSkipped;

            vStats["mNumDecoded"] = videoStats.mNumDecoded;
            vStats["mNumDecoderErrors"] = videoStats.mNumDecoderErrors;
            vStats["mNumDropped"] = videoStats.mNumDropped;
            vStats["mNumReducedQualityDecodes"] = videoStats.mNumReducedQualityDecodes;
            vStats["mNumRendered"] = videoStats.mNumRendered;
            vStats["mNumRendererErrors"] = videoStats.mNumRendererErrors;
            vStats["mNumSkipped"] = videoStats.mNumSkipped;
            args["audioStats"] = aStats;
            args["videoStats"] = vStats;
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("setPlaybackQualityStats"), args);
        }
    }

    void addNetworkTime(uint32_t ms) {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["ms"] = ms;

            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("addNetworkTime"), args);
        }
    }

    void sendEvent( const char *fn, const Variant &args )
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<PlayerBridge> bridge = mBridge.lock()) {
            bridge->PlaybackReporterEvent( fn, args );
        }
    }

    void shutdown()
    {
        ScopedMutex lock(mMutex);
        mBridge.reset();
    }

private:
    Mutex mMutex;
    weak_ptr<PlayerBridge> mBridge;
};

//////////
//
// PlayerBridge
//
//////////
PlayerBridge::PlayerBridge()
    : NfObject("player",
               Properties::PlayerBridge::properties, Properties::PlayerBridge::PropertyCount,
               Methods::PlayerBridge::methods, Methods::PlayerBridge::MethodCount),
      mPlayerMutex(PLAYERBRIDGE_PLAYER_MUTEX, "PlayerBridge::mPlayer"),
      mTime0(0)
{
    mListener.reset(new ASPListener(this));
    mLicenseHandler.reset(new ASPLicenseHandler(this));
}

PlayerBridge::~PlayerBridge()
{
    mListener->reset();
    mLicenseHandler->reset();
    if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player())
        p->removeListener(mListener);
    if (mPlaybackReporter)
        mPlaybackReporter.reset();
}

void PlayerBridge::shutdown()
{
    close();
    if (mPlaybackReporter)
        mPlaybackReporter->shutdown();
    if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player())
        p->shutdown();
}

void PlayerBridge::close()
{
    if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player())
        p->close();
}

unsigned PlayerBridge::requestFlags(const Request &request) const
{
    return NfObject::requestFlags(request);
}

Variant PlayerBridge::invoke(int method, const Variant &data)
{
    // create playback reporter if necessary
    if (!mPlaybackReporter.get()) {
        mPlaybackReporter.reset(
            new PlaybackReporter(static_pointer_cast<PlayerBridge>(shared_from_this())));
    }

    shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> player = createPlayer();

    bool ok;
    switch (method) {
    case Methods::PlayerBridge::obtainPlaybackStat: {
        Variant retdata;
        retdata["type"] = "playbackstat";
        IPlaybackDevice::PlaybackQualityStats audioStats, videoStats;
        player->getPlaybackQualityStats(audioStats, videoStats);
        retdata["audio"]["numDecoded"] = audioStats.mNumDecoded;
        retdata["audio"]["numSkipped"] = audioStats.mNumSkipped;
        retdata["audio"]["numReducedQualityDecodes"] = audioStats.mNumReducedQualityDecodes;
        retdata["audio"]["numDecoderErrors"] = audioStats.mNumDecoderErrors;
        retdata["audio"]["numRendered"] = audioStats.mNumRendered;
        retdata["audio"]["numDropped"] = audioStats.mNumDropped;
        retdata["audio"]["numRendererErrors"] = audioStats.mNumRendererErrors;
        retdata["video"]["numDecoded"] = videoStats.mNumDecoded;
        retdata["video"]["numSkipped"] = videoStats.mNumSkipped;
        retdata["video"]["numReducedQualityDecodes"] = videoStats.mNumReducedQualityDecodes;
        retdata["video"]["numDecoderErrors"] = videoStats.mNumDecoderErrors;
        retdata["video"]["numRendered"] = videoStats.mNumRendered;
        retdata["video"]["numDropped"] = videoStats.mNumDropped;
        retdata["video"]["numRendererErrors"] = videoStats.mNumRendererErrors;
        sendPlayerEvent(retdata);
        break; }
    case Methods::PlayerBridge::obtainStreamingStat: {
        Variant retdata;
        retdata["type"] = "streamingstat";
        shared_ptr<ase::StreamingStatReport> report = player->obtainStreamingStat();
        if (report) {
            retdata["playbackTime"] = report->mPlaybackTimeInMs;
            std::vector<shared_ptr<StreamingMediaStat> > &statList = report->mStreamingMediaStatList;
            for (size_t s = 0; s < statList.size(); s++) {
                retdata["stat"][s]["type"] = statList[s]->mMediaType;
                retdata["stat"][s]["playbackBitrate"] = statList[s]->mPlaybackBitrate;
                retdata["stat"][s]["streamingBitrate"] = statList[s]->mStreamingBitrate;
                retdata["stat"][s]["streamingTime"] = statList[s]->mStreamingTimeInMs;
                retdata["stat"][s]["completeBuffer"] = statList[s]->mCompleteBufferInMs;
                retdata["stat"][s]["incompleteBuffer"] = statList[s]->mIncompleteBufferInMs;
                retdata["stat"][s]["usedMediaBuffer"] = statList[s]->mUsedMediaBuffer;
                retdata["stat"][s]["availableMediaBuffer"] = statList[s]->mAvailableMediaBuffer;
            }
            shared_ptr<LocationStat> location = report->mCurrentLocationStatPtr;
            retdata["location"]["bandwidth"] = location->mBandwidth;
            retdata["location"]["confidece"] = location->mConfidence;
            retdata["location"]["avgConnectTime"] = location->mAvgConnectTime;
            retdata["location"]["name"] = location->mLocationName;
        } else {
            retdata["error"] = "no report";
        }
        sendPlayerEvent(retdata);
        break; }

    case Methods::PlayerBridge::cachePrepare: {
        // check to see how we build manifest
        bool drmOnly = data.mapValue<bool>("drmOnly", 0, false);

        // get manifest
        shared_ptr<Manifest> manifest;
        bool fastplayPossible = false;
        Variant rawManifest = data.value("manifest", &ok);
        if (!ok)
        {
            // missing manifest
            invalidArgumentError(method, "manifest");
            return false;
        }

        NFErr err = ManifestUtilities::createManifestFromVariant(
            /*out*/manifest, /*out*/fastplayPossible, rawManifest, drmOnly);
        if (!err.ok())
        {
            // There was a problem building the manifest from passed in data
            Log::error(TRACE_NBP, "Unable to build manifest from passed in data: %s",
                       err.toString().c_str());
            Variant retdata;
            retdata["type"] = "error";
            retdata["error"] = static_cast<int>(err.peekCode());
            retdata["stack"] = err.toVariant();
            sendPlayerEvent(retdata);
            return false;
        }

        // get other params
        const std::string videoType =
            data.mapValue<std::string>("videoType", 0, "default");
        std::string defaultAudioTrack =
            data.mapValue<std::string>("audioTrack");

        player->cachePrepare(
            manifest, video3DTypeFromString(videoType), defaultAudioTrack,
            mPlaybackReporter);

        break; }

    case Methods::PlayerBridge::cacheFlush: {
        player->cacheFlush();
        break; }

    case Methods::PlayerBridge::cacheList: {
        std::vector<std::string> contentList = player->cacheList();

        Variant retList(Variant::Type_Array);
        std::vector<std::string>::iterator it;
        for (it = contentList.begin(); it != contentList.end(); ++it) {
            std::string id = *it;
            retList.push_back(id);
        }

        Variant map;
        map["movieids"] = retList;
        sendPlayerEvent(map);

        break; }

    case Methods::PlayerBridge::cacheSetSize: {
        const int maxItems = data.mapValue<int>("maxItems", &ok);
        if (ok)
            player->cacheSetSize(maxItems);

        break; }

    case Methods::PlayerBridge::open: {
        shared_ptr<Manifest> manifest;
        bool fastplayPossible = false;

        // check to make sure fastplay is allowed
        bool allowFastplay = data.mapValue<bool>("allowFastplay", 0, false);

        Variant rawManifest = data.value("manifest", &ok);
        if (!ok)
        {
            // missing manifest
            invalidArgumentError(method, "manifest");
            return false;
        }

        NFErr err = ManifestUtilities::createManifestFromVariant(
            /*out*/manifest, /*out*/fastplayPossible, rawManifest,
            !allowFastplay/*drmOnly*/);
        if (!err.ok())
        {
            // There was a problem building the manifest from passed in data
            Log::error(TRACE_NBP, "Unable to build manifest from passed in data: %s",
                       err.toString().c_str());
            Variant retdata;
            retdata["type"] = "error";
            retdata["error"] = static_cast<int>(err.peekCode());
            retdata["stack"] = err.toVariant();
            sendPlayerEvent(retdata);
            return false;
        }

        std::string sessionId = data.mapValue<std::string>("sessionId", &ok);
        if (!ok) {
            invalidArgumentError(method, "sessionId");
            return false;
        }

        // 3D?
        const std::string videoTypeStr =
            data.mapValue<std::string>("videoType", 0, "default");
        Video3DType videoType = video3DTypeFromString(videoTypeStr);

        mOpenAndPause  = data.mapValue<bool>("openAndPause", 0, false);
        std::string defaultAudioTrack =
            data.mapValue<std::string>("audioTrack");
        bool ptsSet;
        const int ms = data.mapValue<int>("pts", &ptsSet, 0);

        // streaming type
        StreamingType streamingType = (StreamingType)
            data.mapValue<int>("streamingType", &ok);
        if (!ok)
            streamingType = fastplayPossible ? FAST_PLAY : NORMAL_PLAY;

        // watch for player events
        player->addListener(mListener);

        // video attributes
        IPlaybackDevice::CommonVideoAttributesFromManifest videoAttributes;
        Variant attrs = data.value("videoAttributes", &ok);
        if (ok)
        {
            ContentProfile profile =
                stringToProfile(attrs.mapValue<std::string>("profile"));
            videoAttributes.codecType_ = profileVideoCodecType(profile);
            videoAttributes.video3DType_ = videoType;
            videoAttributes.maxVideoWidth_ = attrs.mapValue<int>("maxWidth", NULL, 0);
            videoAttributes.maxVideoHeight_ = attrs.mapValue<int>("maxHeight", NULL, 0);
            videoAttributes.videoFrameRateValue_ =
                attrs.mapValue<int>("frameRateValue", NULL, 0);
            videoAttributes.videoFrameRateScale_ =
                attrs.mapValue<int>("frameRateScale", NULL, 0);
        }

        // audio attributes
        IPlaybackDevice::CommonAudioAttributesFromManifest audioAttributes;
        attrs = data.value("audioAttributes", &ok);
        if (ok)
        {
            ContentProfile profile =
                stringToProfile(attrs.mapValue<std::string>("profile"));
            audioAttributes.codecType_ = profileAudioCodecType(profile);
            audioAttributes.mNumberOfChannels_ = attrs.mapValue<int>("channels", NULL, 0);
            audioAttributes.mSamplesPerSecond_ =
                attrs.mapValue<int>("sampleRate", NULL, 0);
        }

        player->open(manifest, mPlaybackReporter, mLicenseHandler,
                      lexical_cast<ullong>(sessionId), videoType, defaultAudioTrack,
                      streamingType, PIFF_CONTAINER, videoAttributes, audioAttributes,
                      ptsSet ? Ticks(ms, Ticks::RES_1KHZ) : Ticks() );

        if (ptsSet)
            mPlayPts = Ticks(ms, Ticks::RES_1KHZ);
        else
            mPlayPts.valid(false);

        break; }

    case Methods::PlayerBridge::close:
        close();
        break;
    case Methods::PlayerBridge::externalIpAddressChange: {
        const std::string ipaddr = data.mapValue<std::string>("ipaddr", &ok);
        if (!ok) {
            invalidArgumentError(method, "ipaddr");
            return false;
        }
        player->externalIpAddressChange(ipaddr);
        break; }
    case Methods::PlayerBridge::play: {
        Ticks ticks;
        const int ms = data.mapValue<int>("pts", &ok, 0);
        if (ok)
            ticks = Ticks(ms, Ticks::RES_1KHZ);
        NTRACE(TRACE_NBP, "play mOpenAndPause=%d pts=%d", mOpenAndPause?1:0, ms);
        const NFErr err = player->play(ticks, mOpenAndPause);
        NTRACEIF(!err.ok(), TRACE_NBP, "IAdaptiveStreamingPlayer::play returned %s", err.toString().c_str());
        break; }
    case Methods::PlayerBridge::skip: {
        const int ms = data.mapValue<int>("pts", &ok, 0);
        if (!ok) {
            invalidArgumentError(method, "pts");
            return false;
        }

        Ticks ticks = Ticks(ms, Ticks::RES_1KHZ);
        const NFErr err = player->skip(ticks);
        mTime0 = 0;
        Variant data;
        data["type"] = "skip";
        data["pts"] = variantFromTicks(ticks);
        if (!err.ok())
        {
            NTRACE(TRACE_NBP, "IAdaptiveStreamingPlayer::skip returned %s",
                   err.toString().c_str());
            data["error"] = true;
        }
        sendPlayerEvent(data);
        break; }
    case Methods::PlayerBridge::swim: {
        int ms = data.mapValue<int>("pts", &ok, 0);
        if (!ok) {
            invalidArgumentError(method, "pts");
            return false;
        }
        Ticks jump = Ticks(ms, Ticks::RES_1KHZ);

        bool absolute = data.mapValue<bool>("absolute", &ok, false);
        const int f = data.mapValue<int>("fuzz", &ok, 0);
        Ticks fuzz = Ticks(f, Ticks::RES_1KHZ);
        bool allowRebuffer = data.mapValue<bool>("allowRebuffer", &ok, false);
        //   INSTRUMENTATION_INTERVAL_SWITCHED_SCOPED(a,"media.swim",Variant::StringMap("pts",ms)("absolute",absolute)("fuzz",f));

        NFErr err;
        Variant retdata;
        retdata["type"] = "swim";
        retdata["pts"] = variantFromTicks(jump);

        IAdaptiveStreamingPlayer::State state = player->getState();
        if (state == IAdaptiveStreamingPlayer::CLOSED ||
            state == IAdaptiveStreamingPlayer::OPENING)
        {
            NTRACE(TRACE_NBP, "swim failed, wrong state %d", state);
            retdata["error"] = true;
            sendPlayerEvent(retdata);
            return Variant();
        }

        Ticks currentPts = player->getCurrentPts();

        if (!absolute) {
            jump = currentPts;
            // wtf happens if you add a ullong and a negative signed int?
            if (ms < 0) {
                ms = -ms;
                Ticks t(ms, Ticks::RES_1KHZ);
                if (jump < t)
                    jump = Ticks(0, Ticks::RES_1KHZ);
                else
                    jump -= t;
            } else {
                jump += Ticks(ms, Ticks::RES_1KHZ);
            }
        }

        Ticks startPts, endPts;
        player->getBufferRange(/*out*/startPts, /*out*/endPts);
        if (state != IAdaptiveStreamingPlayer::STOPPED &&
            startPts <= jump + fuzz &&
            jump <= endPts + fuzz &&
            startPts < endPts)
        {
            // there's an overlap between the buffered range and what's
            // acceptable, and we're in the right state to skip to it
            player->pause(); // will silently succeed if already paused

            if (startPts <= jump && endPts >= jump) {
                err = player->skip(jump);
            } else if (jump < startPts) {
                err = player->skip(startPts);
            } else {
                // endPts must be less than jump
                assert(endPts < jump);
                err = player->skip(endPts);
            }

            if (err.ok()) {
                err = player->unpause();
            }
        }
        else if (state == IAdaptiveStreamingPlayer::STOPPED || allowRebuffer)
        {
            // stop and play at the right position
            Ticks currentPts1 = player->getCurrentPts();
            if (mPlaybackReporter.get())
                mPlaybackReporter->playbackStopped(NFErr_Seek, AseTimeStamp(currentPts1.value(Ticks::RES_1KHZ), 1000), Time::mono());
            player->stop(); // will silently succeed if already stopped
            err = player->play(jump, false);
        } else {
            NTRACE(TRACE_NBP, "swim failed, not enough buffer and no rebuffer");
            retdata["error"] = true;
            retdata["currentPts"] = variantFromTicks(currentPts);
            retdata["startPts"] = variantFromTicks(startPts);
            retdata["endPts"] = variantFromTicks(endPts);
        }
        mTime0 = 0;

        if (!err.ok()) {
            Log::error(TRACE_NBP, "IASPlayer::swim returned %s",
                       err.toString().c_str());
            retdata["error"] = true;
            retdata["currentPts"] = variantFromTicks(currentPts);
            retdata["startPts"] = variantFromTicks(startPts);
            retdata["endPts"] = variantFromTicks(endPts);
            //   INSTRUMENTATION_INTERVAL_SWITCHED_TAG("media.swim",Variant::StringMap("result",err.toVariant()));
        }
        sendPlayerEvent(retdata);
        break; }
    case Methods::PlayerBridge::getBufferRange: {
        // get the buffer range from mediacontrol
        Ticks startPts, endPts, currentPts;
        currentPts = player->getCurrentPts();
        player->getBufferRange(/*out*/startPts, /*out*/endPts);
        uint32_t rebuffer = player->getPredictedFutureRebuffers();
        uint32_t bandwidth = player->getCurrentBandwidth();

        Variant map;
        map["type"] = "bufferrange";
        map["currentPts"] = variantFromTicks(currentPts);
        map["startPts"] = variantFromTicks(startPts);
        map["endPts"] = variantFromTicks(endPts);
        map["rebuffer"] = rebuffer;
        map["bandwidth"] = bandwidth;
        sendPlayerEvent(map);
        break; }
    case Methods::PlayerBridge::startAudioTrackSwitch: {
        player->startAudioTrackSwitch();
        break; }
    case Methods::PlayerBridge::completeAudioTrackSwitch: {
        player->completeAudioTrackSwitch();
        break; }
    case Methods::PlayerBridge::setStreamingBuffer: {
#define READ_VAR(type, name)                                            \
        data.mapValue<type>(name, &ok);                                 \
        do { if (!ok) { invalidArgumentError(method, name); return false; } } while(0)

        const bool powerSaving = READ_VAR(bool, "powerSaving");
        const uint32_t maxBufferLen = READ_VAR(uint32_t, "maxBufferLen");
        const uint32_t minBufferLen = READ_VAR(uint32_t, "minBufferLen");
        player->setStreamingBuffer(powerSaving, maxBufferLen, minBufferLen);
        break; }
    case Methods::PlayerBridge::setVideoBitrateRanges: {
        std::vector<shared_ptr<BitrateRange> > bitrateRanges;

        Variant ranges = data.value("ranges");
        if (ranges.isArray()) {
            for (int i = 0; i < ranges.size(); i++) {
                Variant m = ranges[i];
                if (m.isStringMap()) {
                    shared_ptr<BitrateRange> range(new BitrateRange);
                    range->mContentProfile = stringToProfile(m["profile"].string());
                    range->mMinVideoBitrate = (uint32_t)m["min"].dbl();
                    range->mMaxVideoBitrate = (uint32_t)m["max"].dbl();

                    bitrateRanges.push_back(range);
                }
            }
        }

        NFErr err = player->setVideoBitrateRanges(bitrateRanges);
        if (!err.ok()) {
            NTRACE(TRACE_NBP,
                   "IAdaptiveStreamingPlayer::setVideoBitrateRange returned %s",
                   err.toString().c_str());
            Variant vm;
            vm["error"] = true;
            sendPlayerEvent(vm);
        }
        break; }

    case Methods::PlayerBridge::addManifest: {
        shared_ptr<Manifest> manifest;
        Variant rawManifest = data.value("manifest", &ok);
        if (ok)
        {
            bool faststart = false;
            NFErr err = ManifestUtilities::createManifestFromVariant(
                /*out*/manifest, /*out*/faststart, rawManifest, true/*drmOnly*/);
            if (!err.ok())
            {
                // There was a problem building the manifest from passed in data
                Log::error(TRACE_NBP, "Unable to build manifest from passed in data");
                Variant retdata;
                retdata["type"] = "error";
                retdata["error"] = static_cast<int>(err.peekCode());
                retdata["stack"] = err.toVariant();
                sendPlayerEvent(retdata);
                return Variant();
            }
        }

        // give manifest to player
        player->addManifest(manifest);

        break; }

    case Methods::PlayerBridge::provideLicense: {
        const std::string e = data.mapValue<std::string>("license", &ok);
        if (!ok) {
            invalidArgumentError(method, "license");
            return false;
        }
        std::string l = Base64::decode(e);

        std::vector<unsigned char> license(l.begin(), l.end());
        std::vector<unsigned char> secureStopId;
        uint32_t drmSessionId;

        NFErr err =player->provideLicense(license, secureStopId, drmSessionId);
        if(err.ok()){
            Variant vm;
            vm["type"] = "licensesession";
            vm["session"] = Base64::encode(std::string(secureStopId.begin(), secureStopId.end()));
            vm["drmSession"] = drmSessionId;
            sendPlayerEvent(vm);
        }
        break; }
    case Methods::PlayerBridge::stop:
        mPlayPts.valid(false);
        player->stop();
        break;
    case Methods::PlayerBridge::pause:
        player->pause();
        break;
    case Methods::PlayerBridge::unpause:
        player->unpause();
        break;
    case Methods::PlayerBridge::setVideoWindow: {
        uint32_t ints[5] = { 0 };
        static const char *names[] = { "x", "y", "width", "height", "transitionDuration" };
        for (int i=0; i<5; ++i) {
            ints[i] = data.mapValue<uint32_t>(names[i], &ok);
            if (!ok) {
                invalidArgumentError(method, names[i]);
                return false;
            }
        }
        /*
        if ((ints[0] % 2) || (ints[2] % 2)) {
            Variant map;
            map["type"] = "videowindowchanged";
            map["error"] = true;
            sendPlayerEvent(map);
            break;
        }
        */
        const Rect rect = { ints[0], ints[1], ints[2], ints[3] };
        player->setVideoWindow(rect, ints[4]);
        break; }
    case Methods::PlayerBridge::bringVideoToFront:
        player->bringVideoToFront();
        break;
    case Methods::PlayerBridge::sendVideoToBack:
        player->sendVideoToBack();
        break;
    case Methods::PlayerBridge::getVolume:
    {
        double volume = player->getVolume();
        Variant map;
        map["type"] = "volume";
        map["volume"] = volume;
        sendPlayerEvent(map);
        break;
    }
    case Methods::PlayerBridge::setVolume:
    {
        double targetVolume = data.mapValue<double>("targetVolume");

        // check targetVolume validity
        if (targetVolume < 0) {
            NTRACE(TRACE_NBP, "PlayerBridge::invoke(): warning, setVolume: targetVolume < 0, set to 0");
            targetVolume = 0;
        } else if (targetVolume > 1) {
            NTRACE(TRACE_NBP, "PlayerBridge::invoke(): warning, setVolume: targetVolume > 1.0, set to 1.0");
            targetVolume = 1.0;
        }

        int transitionDuration = data.mapValue<int>("transitionDuration", NULL, 0);
        // check transitionDuration validity
        if (transitionDuration < 0) {
            NTRACE(TRACE_NBP, "PlayerBridge::invoke(): warning, setVolume: transitionDuration < 0, set to 0");
            transitionDuration = 0;
        }

        IAudioMixer::Ease ease = (IAudioMixer::Ease)data.mapValue<int>("ease", NULL, 0);
        if (ease < 0 || ease >= IAudioMixer::EaseCount) {
            NTRACE(TRACE_NBP, "PlayerBridge::invoke(): warning, setVolume: invalid ease specified, set to EaseLinear");
            ease = IAudioMixer::EaseLinear;
        }

        player->setVolume(targetVolume, transitionDuration, ease);
        break;
    }
    case Methods::PlayerBridge::notifyBufferingComplete:
    {
        // JS is notifying the player that buffering has completed and actual playback
        // can commence
        const uint32_t pts = data.mapValue<int>("pts", NULL, 0);
        player->completeBuffering(pts);
        break;
    }
    default:
        return NfObject::invoke(method, data);
    }
    return Variant();
}

bool PlayerBridge::getProperty(int index, Variant *result) const
{
    switch (index) {
    case Properties::PlayerBridge::bufferPoolSize:
        if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player()) {
            Variant map;
            uint32_t video, audio;
            p->getMediaBufferPoolSize(video, audio);
            map["video"] = video;
            map["audio"] = audio;
            *result = map;
        }
        break;
    case Properties::PlayerBridge::currentAudioTrack:
        if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player()) {
            std::string trackId;
            p->getCurrentAudioTrack(trackId);
            *result = trackId;
        }
        break;
    case Properties::PlayerBridge::displayAspectRatio:
        if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player()) {
            Variant map;
            uint32_t x, y;
            p->getDisplayAspectRatio(x, y);
            map["x"] = x;
            map["y"] = y;
            *result = map;
        }
        break;
    case Properties::PlayerBridge::duration:
        if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player())
            *result = variantFromTicks(p->getPlayDuration());
        break;
    case Properties::PlayerBridge::gopSize:
        if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player())
            *result = p->getGopSize();
        break;
    case Properties::PlayerBridge::maxStreamingBuffer:
        if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player())
            *result = p->getMaxStreamingBuffer();
        break;
    case Properties::PlayerBridge::networkProfile:
        if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player())
            *result = p->getNetworkProfile();
        break;
    case Properties::PlayerBridge::state:
        if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player())
            *result = p->getState();
        else
            *result = IAdaptiveStreamingPlayer::CLOSED;
        break;
    case Properties::PlayerBridge::allowNrdpVideoSwitch:
        *result = false;
        break;
    default:
        return false;
    }
    return true;
}

bool PlayerBridge::setProperty(int index, const Variant &value)
{
    shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> player = createPlayer();

    bool ok = false;
    switch (index)
    {
        case Properties::PlayerBridge::currentAudioTrack:
        {
            std::string str = value.value<std::string>(&ok);
            if (ok) {
                player->selectAudioTrack(str);
            }
            break;
        }
        case Properties::PlayerBridge::networkProfile:
        {
            IAdaptiveStreamingPlayer::NetworkProfile profile =
                static_cast<IAdaptiveStreamingPlayer::NetworkProfile>(
                    value.value<int>(&ok, 0));
            if (ok) {
                player->setNetworkProfile(profile);
                propertyUpdated(Properties::PlayerBridge::networkProfile);
            }
            break;
        }
    }
    return true;
}

void PlayerBridge::stateChanged(ullong /*sessionId*/,
                                mediacontrol::IAdaptiveStreamingPlayer::State state)
{
    int proplist[3], count = 0;
    if (state == IAdaptiveStreamingPlayer::CLOSED) {
        nrdApp()->sendEvent(ApplicationEvent::Event_PlaybackExited);
        if (shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> p = player())
            p->removeListener(mListener);

        // update duration property
        proplist[count++] = Properties::PlayerBridge::duration;
    } else if (state == IAdaptiveStreamingPlayer::OPENING) {
        nrdApp()->sendEvent(ApplicationEvent::Event_PlaybackEntered);
    }

    proplist[count++] = Properties::PlayerBridge::bufferPoolSize;
    proplist[count++] = Properties::PlayerBridge::state;

    mTime0 = 0;

    Variant map;
    map[PROPUPS] = properties(proplist, count);
    map["type"] = "statechanged";
    map["state"] = state;
    sendPlayerEvent(map);
}

void PlayerBridge::openComplete(ullong sessionId)
{
    const int openProperties[] = {
        Properties::PlayerBridge::bufferPoolSize,
        Properties::PlayerBridge::currentAudioTrack,
        Properties::PlayerBridge::displayAspectRatio,
        Properties::PlayerBridge::duration,
        Properties::PlayerBridge::gopSize,
    };
    Variant map;
    map[PROPUPS] = properties(openProperties, sizeof(openProperties) / sizeof(int));
    map["type"] = "opencomplete";
    map["sessionid"] = lexical_cast<std::string>(sessionId);
    // if openAndPlay() was called, the UI may have passed in audio/subtitle
    // tracks. if we successfully selected those tracks, then we can go ahead
    // and call play(). otherwise, notify them that we're not sure we got what
    // they wanted and they need to double-check.
    if (mPlayPts.valid()) {
        map["playing"] = mPlayPts.valid();
    }
    // ok, now let the UI know that open is complete
    sendPlayerEvent(map);

    // there are three cases we want to call play() here:
    // 1. openAndPlay()
    // 2. fastStart()
    // 3. restartPlayback event

    if (mPlayPts.valid()) {
        NTRACE(TRACE_NBP, "PlayerBridge mPlayPts %d, calling play automatically",
               mPlayPts.valid());
        Variant vm;
        vm["object"] = path();
        vm["method"] = "play";
        if (mPlayPts.valid()) {
            vm["pts"] = variantFromTicks(mPlayPts);
        } else {
            vm["pts"] = 0;
        }
        mPlayPts.valid(false);
        Request req(vm);
        nrdApp()->post(req);
    }
}

void PlayerBridge::bufferingComplete(ullong /*sessionId*/, bool faststart)
{
    mTime0 = 0;
    Variant map;
    map["type"] = "bufferingcomplete";
    map["faststart"] = faststart;
    sendPlayerEvent(map);
}

void PlayerBridge::updatePts(ullong /*sessionId*/, const Ticks& pts, const ullong& monotimeInMS)
{
    bool driftDetected = true;
    Time time(monotimeInMS);
    const ullong nowMS = Time::mono().ms();
    const ullong tmMS = pts.value(Ticks::RES_1KHZ);
    if (mTime0) {
        // if the wall clock time significantly differs from what media
        // control's view is, it implies a drift that should be investigated.
        llong diff = (mTime0 + tmMS) - nowMS;
        if (diff > 750 || diff < -750) {
            Log::info(TRACE_NBP, "unexpected drift of %lld in player time: %llu + %llu = %llu != %llu",
                      diff, mTime0, tmMS, mTime0 + tmMS, nowMS);
        } else {
            driftDetected = false;
        }
    }

    if (!driftDetected)
        return;

    mTime0 = nowMS - tmMS;

    Variant map;
    map["type"] = "updatepts";
    map["pts"] = variantFromTicks(pts);
    sendEvent("IASPlayer", map, 0, time);
}

void PlayerBridge::audioTrackChanged(ullong /*sessionId*/, uint32_t trackIndex, std::string const& trackId)
{
    const int proplist[] = { Properties::PlayerBridge::currentAudioTrack };
    Variant map;
    map[PROPUPS] = properties(proplist, sizeof(proplist) / sizeof(int));
    map["type"] = "audiotrackchanged";
    map["trackIndex"] = trackIndex;
    map["trackId"] = trackId;
    sendPlayerEvent(map);
}

void PlayerBridge::audioTrackChangeStarted(ullong /*sessionId*/)
{
    Variant map;
    map["type"] = "audiotrackchangestarted";
    sendPlayerEvent(map);
}

void PlayerBridge::audioTrackChangeRejected(ullong /*sessionId*/)
{
    Variant map;
    map["type"] = "audiotrackswitchrejected";
    sendPlayerEvent(map);
}

void PlayerBridge::audioTrackChangeComplete(ullong /*sessionId*/)
{
    Variant map;
    map["type"] = "audiotrackswitchcomplete";
    sendPlayerEvent(map);
}

NFErr PlayerBridge::provideChallenge(const std::vector<unsigned char>& challenge)
{
    Variant map;
    map["type"] = "providechallenge";
    map["challenge"] = Base64::encode(std::string(challenge.begin(), challenge.end()));
    sendPlayerEvent(map);

    return NFErr_OK;
}

NFErr PlayerBridge::provideChallenge(std::string const& drmType,
                                     std::string const& streamID,
                                     const std::vector<unsigned char>& challenge,
                                     const std::vector<unsigned char>& drmHeader)
{
    Variant map;
    map["type"] = "providechallenge";
    map["drmType"] = drmType;
    map["streamID"] = streamID;
    map["challenge"] = Base64::encode(std::string(challenge.begin(), challenge.end()));
    map["drmHeader"] = Base64::encode(std::string(drmHeader.begin(), drmHeader.end()));
    sendPlayerEvent(map);

    return NFErr_OK;
}

void PlayerBridge::provideRelease(std::string const& /*drmType*/, std::string const& /*release*/)
{
}

void PlayerBridge::notifyBufferingComplete()
{
}

NFErr PlayerBridge::checkStatus()
{
    return NFErr_OK; // XXX check status of authorization/license
}

void PlayerBridge::updateContentDuration(ullong /*sessionId*/, uint32_t /*duration*/)
{
    propertyUpdated(Properties::PlayerBridge::duration);
}

void PlayerBridge::manifestSelected(ullong sessionId, int manifestIndex)
{
    const int openProperties[] = {
        Properties::PlayerBridge::bufferPoolSize,
        Properties::PlayerBridge::currentAudioTrack,
        Properties::PlayerBridge::displayAspectRatio,
        Properties::PlayerBridge::duration,
        Properties::PlayerBridge::gopSize,
    };
    Variant vm;
    vm[PROPUPS] = properties(openProperties, sizeof(openProperties) / sizeof(int));
    vm["type"] = "manifestselected";
    vm["sessionid"] = lexical_cast<std::string>(sessionId);
    vm["manifestIndex"] = manifestIndex;
    sendPlayerEvent(vm);
}

void PlayerBridge::hevcInfo(ullong sessionId, const std::string &info)
{
    Variant event;
    event["type"] = "hevcstreaminfo";
    event["info"] = info;
    event["sessionId"] = sessionId;
    sendPlayerEvent(event);
}

class PlayerBridgeEvent;
static weak_ptr<PlayerBridgeEvent> sPlayerBridgeEvent;
static Mutex sPlayerBridgeEventLock(PLAYERBRIDGE_EVENT_MUTEX, "PlayerBridgeEventLock");
class PlayerBridgeEvent : public Application::Event
{
public:
    PlayerBridgeEvent(const shared_ptr<PlayerBridge> &bridge) : Event(HighPriority), mBridge(bridge) { }
    virtual void eventFired() {
        {
            ScopedMutex _lock(sPlayerBridgeEventLock);
            sPlayerBridgeEvent.reset();
        }
        for(std::list<Variant>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it) {
            const Variant &data = (*it);
            mBridge->sendEvent("IASPlayer", data, Response::Flag_Synchronous);
        }
    }
    virtual std::string describe() const { return "PlayerBridgeEvent"; }
    void append(const Variant &event) {
        if(!mEvents.empty()) {
            const std::string type = event["type"].string();
            if(type == "buffering") {
                Variant &last = mEvents.back();
                if(last["type"] == "buffering") {
                    last["percentage"] = event["percentage"];
                    return;
                }
            } else if(type == "updatepts") {
                std::string lastType;
                for(std::list<Variant>::iterator it = mEvents.begin(); it != mEvents.end(); ) {
                    const Variant &data = (*it);
                    lastType = data["type"].string();
                    if(lastType == "buffering") {
                        lastType.clear();
                        it = mEvents.erase(it);
                    } else {
                        ++it;
                    }
                }
                if(!lastType.empty() && lastType == "updatepts") {
                    Variant &last = mEvents.back();
                    last["pts"] = event["pts"];
                    return;
                }
            }
        }
        mEvents.push_back(event);
    }
protected:
    std::list<Variant> mEvents;
    shared_ptr<PlayerBridge> mBridge;
};

void PlayerBridge::sendPlayerEvent(const Variant &data)
{
#if 1
    sendEvent("IASPlayer", data);
#else
    shared_ptr<PlayerBridgeEvent> post;
    {
        ScopedMutex _lock(sPlayerBridgeEventLock);
        shared_ptr<PlayerBridgeEvent> event = sPlayerBridgeEvent.lock();
        if(!event) {
            event.reset(new PlayerBridgeEvent(static_pointer_cast<PlayerBridge>(shared_from_this())));
            sPlayerBridgeEvent = event;
            post = event;
        }
        event->append(data);
    }
    if(post)
        app()->postEvent(post);
#endif
}

void PlayerBridge::PlaybackReporterEvent(const char *function, const Variant &args)
{
    Variant vmap;
    vmap["type"] = "PlaybackReporter";
    vmap["funcname"] = function;
    vmap["args"] = args;
    sendEvent("PlaybackReporter", vmap);
}

shared_ptr<IAdaptiveStreamingPlayer> PlayerBridge::createPlayer()
{
    ScopedMutex lock(mPlayerMutex);
    if (!mPlayer) {
        mPlayer.reset(new ASPlayer);
        nrdApp()->setPlayer(mPlayer);
        nrdApp()->setPlayerType(PLAYER_ASPLAYER);
        const std::vector<shared_ptr<BitrateRange> > ranges = NrdConfiguration::videoBitrateRanges();
        mPlayer->setVideoBitrateRanges(ranges);
    }
    return mPlayer;
}

