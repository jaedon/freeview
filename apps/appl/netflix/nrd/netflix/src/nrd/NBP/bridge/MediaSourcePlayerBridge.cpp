/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MediaSourcePlayerBridge.h"

#include <nrdbase/Base64.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/tr1.h>
#include <nrdbase/LexicalCast.h>
#include <nrdbase/Application.h>

#include <nrd/NrdApplication.h>
#include "NrdpMedia.h"
#include <nrd/IPlaybackDevice.h>
#include <nrdase/AseTimeStamp.h>
#include <nrd/IAdaptiveStreamingPlayer.h>

#include "NrdConfiguration.h"
#include "NrdpBridge.h"
#include "MediaRequestBridge.h"

using namespace netflix;
using namespace netflix::ase;
using namespace netflix::device;
using namespace netflix::mediacontrol;
using namespace netflix::util;

#define DevicePropertyList(Property)                            \
    Property(bufferPoolSize, NfObject::ReadOnly)                \
    Property(state, NfObject::ReadOnly)

PropertyTable(MediaSourcePlayerBridge, DevicePropertyList, properties)

#define PROPUPS "_propups"

#define MethodList(Method)                      \
    Method(bringVideoToFront)                   \
    Method(close)                               \
    Method(disableAudio)                        \
    Method(enableAudio)                         \
    Method(getBufferRange)                      \
    Method(getVolume)                           \
    Method(obtainPlaybackStat)                  \
    Method(open)                                \
    Method(pause)                               \
    Method(play)                                \
    Method(sendVideoToBack)                     \
    Method(setVideoWindow)                      \
    Method(setVolume)                           \
    Method(skip)                                \
    Method(stop)                                \
    Method(swim)                                \
    Method(unpause)

MethodTable(MediaSourcePlayerBridge, MethodList)

static inline Variant variantFromTicks(const Ticks &ticks)
{
    return ticks.valid() ? Variant(static_cast<int>(ticks.value(Ticks::RES_1KHZ))) : Variant();
}


#if !defined(BUILD_DEBUG)
    #define CHECK_EVENT_NAME(eventName) eventName
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
// MediaSourcePlayerBridge::Listener
//
//////////
class MediaSourcePlayerBridge::Listener : public mediacontrol::IMediaSourcePlayer::Listener
{
public:
    Listener(MediaSourcePlayerBridge *bridge) :
      mMutex(PLAYERBRIDGE_MUTEX, "MediaSourcePlayerBridge::Listener"),
      mBridge(bridge)
    {
    }
    virtual ~Listener() {}

    virtual void stateChanged(ullong sessionId, mediacontrol::IAdaptiveStreamingPlayer::State state)
    {
        ScopedMutex lock(mMutex);
        if (mBridge){
            mBridge->stateChanged(sessionId, state);
        }
    }
    virtual void openComplete(ullong sessionId)
    {
        ScopedMutex lock(mMutex);
        if (mBridge)
            mBridge->openComplete(sessionId);
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
        if (mBridge){
            mBridge->sendPlayerEvent(Variant("type", "underflow"));
        }
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
    virtual void audioTrackChangeStarted(ullong)
    {
        ScopedMutex lock(mMutex);
        if (mBridge){
            Variant map;
            map["type"] = "audiotrackchangestarted";
            mBridge->sendPlayerEvent(map);
        }
    }
    virtual void audioTrackChangeRejected(ullong)
    {
        ScopedMutex lock(mMutex);
        if (mBridge){
            Variant map;
            map["type"] = "audiotrackswitchrejected";
            mBridge->sendPlayerEvent(map);
        }

    }
    virtual void audioTrackChangeComplete(ullong)
    {
        ScopedMutex lock(mMutex);
        if (mBridge){
            Variant map;
            map["type"] = "audiotrackswitchcomplete";
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
    void reset()
    {
        ScopedMutex lock(mMutex);
        mBridge = NULL;
    }

private:
    Mutex mMutex;
    MediaSourcePlayerBridge *mBridge;
};

//////////
//
// MediaSourcePlayerBridge::PlaybackReporter
//
//////////
class MediaSourcePlayerBridge::PlaybackReporter : public mediacontrol::IMediaSourcePlaybackReporter
{
public:
    PlaybackReporter(const shared_ptr<MediaSourcePlayerBridge> &bridge) :
      mMutex(PLAYBACKREPORTERBRIDGE_MUTEX, "MediaSourcePlayerBridgePlaybackReporter"),
      mBridge(bridge)
    {
    }
    virtual ~PlaybackReporter() {}

    /*
     * IMediaPipelineReporter interfaces
     */
    virtual void playbackStateChange(const AseTimeStamp &movieTime,
                                     IMediaPipelineReporter::PlaybackState newState)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
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
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["additionalInfo"] = additionalInfo;
            args["eventTime"] =  eventTime.ms();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("playbackStarted"), args);
        }
    }
    virtual void playbackFailure(NFErr nferr)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
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
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
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
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
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
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
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
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
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
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["movieTime"] = (int)AseTimeStamp(movieTime, 1000).getValueInMs();
            args["reason"] =  reason.toJsonString();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("playbackClosed"), args);
        }
    }

    virtual void setPlaybackQualityStats(const IPlaybackDevice::PlaybackQualityStats& audioStats,
                                                   const IPlaybackDevice::PlaybackQualityStats& videoStats)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
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

    // IMediaSourcePlaybackReporter interface
    virtual void skipStarted()
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("skipStarted"));
        }
    }

    virtual void reportKeyStartplayEvent(std::string const& eventName)
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
            Variant args;
            args["eventName"] = eventName;
            args["nativetime"] = Time::mono().ms();
            bridge->PlaybackReporterEvent(CHECK_EVENT_NAME("reportKeyStartplayEvent"), args);
        }
    }

    // PlaybackReporter method
    void sendEvent( const char *fn, const Variant &args )
    {
        ScopedMutex lock(mMutex);
        if (shared_ptr<MediaSourcePlayerBridge> bridge = mBridge.lock()) {
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
    weak_ptr<MediaSourcePlayerBridge> mBridge;
};

//////////
//
// MediaSourcePlayerBridge
//
//////////
MediaSourcePlayerBridge::MediaSourcePlayerBridge(const std::string &name)
    : NfObject(name,
               Properties::MediaSourcePlayerBridge::properties,
               Properties::MediaSourcePlayerBridge::PropertyCount,
               Methods::MediaSourcePlayerBridge::methods,
               Methods::MediaSourcePlayerBridge::MethodCount),
      mPlayerMutex(PLAYERBRIDGE_PLAYER_MUTEX, "MediaSourcePlayerBridge::mPlayer"),
      mTime0(0)
{
    mListener.reset(new Listener(this));
    createPlayer();
}

MediaSourcePlayerBridge::~MediaSourcePlayerBridge()
{
    mListener->reset();
    {
        /*
         * protect with mPlayerMutex here. Previously, while other thread was in
         * the middle of getting player sinde of player() function, this
         * destructor reset the mPlayer, and player() was getting null
         * pointer. To prevent that happen, protect with mutex.
         */
        ScopedMutex _lock(mPlayerMutex);
        if(mPlayer){
            mPlayer->removeListener(mListener);
            mPlayer.reset();
        }
    }
    if (mPlaybackReporter)
        mPlaybackReporter.reset();
}

void MediaSourcePlayerBridge::shutdown()
{
    close();
    if (mPlaybackReporter)
        mPlaybackReporter->shutdown();
    if (shared_ptr<mediacontrol::IMediaSourcePlayer> p = player()) {
        p->shutdown();
        nrdApp()->setPlayer(name(), shared_ptr<mediacontrol::IMediaSourcePlayer>());
    }
}

void MediaSourcePlayerBridge::pause()
{
    if (shared_ptr<mediacontrol::IMediaSourcePlayer> p = player())
        p->pause();
}

void MediaSourcePlayerBridge::close()
{
    if (shared_ptr<mediacontrol::IMediaSourcePlayer> p = player())
        p->close();
}

Variant MediaSourcePlayerBridge::invoke(int method, const Variant &data)
{
    // create playback reporter if necessary
    if (!mPlaybackReporter.get()) {
        mPlaybackReporter.reset(
            new PlaybackReporter(static_pointer_cast<MediaSourcePlayerBridge>(shared_from_this())));
    }

    shared_ptr<mediacontrol::IMediaSourcePlayer> player = createPlayer();
    nrdApp()->setPlayerType(PLAYER_MEDIASOURCEPLAYER);

    bool ok;
    switch (method) {
    case Methods::MediaSourcePlayerBridge::obtainPlaybackStat: {
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

    case Methods::MediaSourcePlayerBridge::open: {
        NTRACE(TRACE_MSPBRIDGE, "open()");
        // sessionId
        std::string sessionId = data.mapValue<std::string>("sessionId", &ok);
        if (!ok) {
            invalidArgumentError(method, "sessionId");
            return false;
        }

        // 3D type
        const std::string videoTypeStr = data.mapValue<std::string>("videoType", 0, "default");
        Video3DType videoType = video3DTypeFromString(videoTypeStr);

        // openAndPause
        mOpenAndPause  = data.mapValue<bool>("openAndPause", 0, false);

        // pts
        bool ptsSet;
        const int ms = data.mapValue<int>("pts", &ptsSet, 0);

        // watch for player events
        player->addListener(mListener);

        // video attributes
        IPlaybackDevice::CommonVideoAttributesFromManifest videoAttributes;
        Variant attrs = data.value("videoAttributes", &ok);
        if (ok)
        {
            ContentProfile profile = stringToProfile(attrs.mapValue<std::string>("profile"));
            videoAttributes.codecType_ = profileVideoCodecType(profile);
            videoAttributes.video3DType_ = videoType;
            videoAttributes.maxVideoWidth_ = attrs.mapValue<int>("maxWidth", NULL, 0);
            videoAttributes.maxVideoHeight_ = attrs.mapValue<int>("maxHeight", NULL, 0);
            videoAttributes.pixelAspectRatioX_ = attrs.mapValue<int>("pixelAspectRatioX", NULL, 0);
            videoAttributes.pixelAspectRatioY_ = attrs.mapValue<int>("pixelAspectRatioY", NULL, 0);
            videoAttributes.videoFrameRateValue_ = attrs.mapValue<int>("frameRateValue", NULL, 0);
            videoAttributes.videoFrameRateScale_ = attrs.mapValue<int>("frameRateScale", NULL, 0);
        }

        // audio attributes
        IPlaybackDevice::CommonAudioAttributesFromManifest audioAttributes;
        attrs = data.value("audioAttributes", &ok);
        if (ok)
        {
            ContentProfile profile = stringToProfile(attrs.mapValue<std::string>("profile"));
            audioAttributes.codecType_ = profileAudioCodecType(profile);
            audioAttributes.mNumberOfChannels_ = attrs.mapValue<int>("channels", NULL, 0);
            audioAttributes.mSamplesPerSecond_ = attrs.mapValue<int>("sampleRate", NULL, 0);
        }

        uint32_t pipelineId = data.mapValue<uint32_t>("pipelineId", &ok);

        const std::string movieId = data.mapValue<std::string>("movieId", 0, "0");

        player->open(videoAttributes,
                     audioAttributes,
                     mPlaybackReporter,
                     lexical_cast<ullong>(sessionId),
                     pipelineId,
                     movieId);

        if (ptsSet)
            mPlayPts = Ticks(ms, Ticks::RES_1KHZ);
        else
            mPlayPts.valid(false);

        break; }

    case Methods::MediaSourcePlayerBridge::close:
        NTRACE(TRACE_MSPBRIDGE, "close()");
        close();
        break;

    case Methods::MediaSourcePlayerBridge::play: {
        Ticks ticks;
        const int ms = data.mapValue<int>("pts", &ok, 0);
        if (ok)
            ticks = Ticks(ms, Ticks::RES_1KHZ);

        NTRACE(TRACE_MSPBRIDGE, "play() mOpenAndPause=%d pts=%d", mOpenAndPause?1:0, ms);
        const NFErr err = player->play(ticks, mOpenAndPause);
        NTRACEIF(!err.ok(), TRACE_MSPBRIDGE, "play returned %s from player",
                 err.toString().c_str());
        break; }

    case Methods::MediaSourcePlayerBridge::skip: {
        const int ms = data.mapValue<int>("pts", &ok, 0);
        if (!ok) {
            invalidArgumentError(method, "pts");
            return false;
        }

        NTRACE(TRACE_MSPBRIDGE, "skip() pts=%d", ms);
        Ticks ticks = Ticks(ms, Ticks::RES_1KHZ);
        const NFErr err = player->skip(ticks);
        mTime0 = 0;
        Variant data;
        data["type"] = "skip";
        data["pts"] = variantFromTicks(ticks);
        if (!err.ok())
        {
            NTRACE(TRACE_MSPBRIDGE, "skip returned %s from player",
                   err.toString().c_str());
            data["error"] = true;
        }
        sendPlayerEvent(data);
        break; }

    case Methods::MediaSourcePlayerBridge::swim: {
        int ms = data.mapValue<int>("pts", &ok, 0);
        if (!ok) {
            invalidArgumentError(method, "pts");
            return false;
        }

        NTRACE(TRACE_MSPBRIDGE, "swim() pts=%d", ms);
        Ticks jump = Ticks(ms, Ticks::RES_1KHZ);
        bool absolute = data.mapValue<bool>("absolute", &ok, false);
        const int f = data.mapValue<int>("fuzz", &ok, 0);
        Ticks fuzz = Ticks(f, Ticks::RES_1KHZ);
        bool allowRebuffer = data.mapValue<bool>("allowRebuffer", &ok, false);

        NFErr err;
        Variant retdata;
        retdata["type"] = "swim";
        retdata["pts"] = variantFromTicks(jump);

        IAdaptiveStreamingPlayer::State state = player->getState();
        if (state == IAdaptiveStreamingPlayer::CLOSED ||
            state == IAdaptiveStreamingPlayer::OPENING)
        {
            NTRACE(TRACE_MSPBRIDGE, "swim failed, wrong state %d", state);
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
            NTRACE(TRACE_MSPBRIDGE, "swim(), skipping in buffer");
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
            NTRACE(TRACE_MSPBRIDGE, "swim() stopping player");
            if (mPlaybackReporter.get())
            {
                Ticks playerPts = player->getCurrentPts();
                AseTimeStamp stopPts(playerPts.value(Ticks::RES_1KHZ), 1000);
                mPlaybackReporter->playbackStopped(
                    NFErr_Seek, stopPts, Time::mono());
            }
            player->stop(); // will silently succeed if already stopped
        } else {
            NTRACE(TRACE_MSPBRIDGE, "swim failed, not enough buffer and no rebuffer");
            retdata["error"] = true;
            retdata["currentPts"] = variantFromTicks(currentPts);
            retdata["startPts"] = variantFromTicks(startPts);
            retdata["endPts"] = variantFromTicks(endPts);
        }
        mTime0 = 0;

        if (!err.ok()) {
            Log::error(TRACE_MSPBRIDGE, "swim() returned %s from player",
                       err.toString().c_str());
            retdata["error"] = true;
            retdata["currentPts"] = variantFromTicks(currentPts);
            retdata["startPts"] = variantFromTicks(startPts);
            retdata["endPts"] = variantFromTicks(endPts);
            //   INSTRUMENTATION_INTERVAL_SWITCHED_TAG("media.swim",Variant::StringMap("result",err.toVariant()));
        }
        sendPlayerEvent(retdata);
        break; }

    case Methods::MediaSourcePlayerBridge::disableAudio: {
        NTRACE(TRACE_MSPBRIDGE, "disableAudio()");
        player->disableAudio();
        break; }

    case Methods::MediaSourcePlayerBridge::enableAudio: {
        NTRACE(TRACE_MSPBRIDGE, "enableAudio()");
        player->enableAudio();
        break; }

    case Methods::MediaSourcePlayerBridge::stop:
        NTRACE(TRACE_MSPBRIDGE, "stop()");
        mPlayPts.valid(false);
        player->stop();
        break;

    case Methods::MediaSourcePlayerBridge::pause:
        NTRACE(TRACE_MSPBRIDGE, "pause()");
        player->pause();
        break;

    case Methods::MediaSourcePlayerBridge::unpause:
        NTRACE(TRACE_MSPBRIDGE, "unpause()");
        player->unpause();
        break;

    case Methods::MediaSourcePlayerBridge::setVideoWindow: {
        uint32_t ints[6] = { 0 };
        static const char *names[] = { "x", "y", "width", "height", "transitionDuration", "zOrder" };
        for (int i=0; i<5; ++i) {
            ints[i] = data.mapValue<uint32_t>(names[i], &ok);
            if (!ok) {
                invalidArgumentError(method, names[i]);
                return false;
            }
        }
        ints[5] = data.mapValue<uint32_t>(names[5], &ok);
        const Rect rect = { ints[0], ints[1], ints[2], ints[3] };
        player->setVideoWindow(rect, ints[4], ints[5]);
        break; }

    case Methods::MediaSourcePlayerBridge::getBufferRange:
    {
        Ticks startPts, endPts, currentPts;
        currentPts = player->getCurrentPts();
        player->getBufferRange(/*out*/startPts, /*out*/endPts);

        Variant map;
        map["type"] = "bufferrange";
        map["currentPts"] = variantFromTicks(currentPts);
        map["startPts"] = variantFromTicks(startPts);
        map["endPts"] = variantFromTicks(endPts);
        sendPlayerEvent(map);
        break;
    }

    case Methods::MediaSourcePlayerBridge::getVolume:
    {
        double volume = player->getVolume();
        Variant map;
        map["type"] = "volume";
        map["volume"] = volume;
        sendPlayerEvent(map);
        break;
    }

    case Methods::MediaSourcePlayerBridge::setVolume:
    {
        double targetVolume = data.mapValue<double>("targetVolume");

        // check targetVolume validity
        if (targetVolume < 0) {
            NTRACE(TRACE_MSPBRIDGE, "MediaSourcePlayerBridge::invoke(): warning, setVolume: targetVolume < 0, set to 0");
            targetVolume = 0;
        } else if (targetVolume > 1) {
            NTRACE(TRACE_MSPBRIDGE, "MediaSourcePlayerBridge::invoke(): warning, setVolume: targetVolume > 1.0, set to 1.0");
            targetVolume = 1.0;
        }

        int transitionDuration = data.mapValue<int>("transitionDuration", NULL, 0);
        // check transitionDuration validity
        if (transitionDuration < 0) {
            NTRACE(TRACE_MSPBRIDGE, "MediaSourcePlayerBridge::invoke(): warning, setVolume: transitionDuration < 0, set to 0");
            transitionDuration = 0;
        }

        IAudioMixer::Ease ease = (IAudioMixer::Ease)data.mapValue<int>("ease", NULL, 0);
        if (ease < 0 || ease >= IAudioMixer::EaseCount) {
            NTRACE(TRACE_MSPBRIDGE, "MediaSourcePlayerBridge::invoke(): warning, setVolume: invalid ease specified, set to EaseLinear");
            ease = IAudioMixer::EaseLinear;
        }

        player->setVolume(targetVolume, transitionDuration, ease);
        break;
    }

    default:
        return NfObject::invoke(method, data);
    }
    return Variant();
}

bool MediaSourcePlayerBridge::getProperty(int index, Variant *result) const
{
    switch (index) {
    case Properties::MediaSourcePlayerBridge::bufferPoolSize:
        if (shared_ptr<mediacontrol::IMediaSourcePlayer> p = player()) {
            Variant map;
            uint32_t video, audio;
            p->getMediaBufferPoolSize(video, audio);
            map["video"] = video;
            map["audio"] = audio;
            *result = map;
        }
        break;
    case Properties::MediaSourcePlayerBridge::state:
        if (shared_ptr<mediacontrol::IMediaSourcePlayer> p = player())
            *result = p->getState();
        else
            *result = IAdaptiveStreamingPlayer::CLOSED;
        break;
    default:
        return false;
    }
    return true;
}

void MediaSourcePlayerBridge::stateChanged(ullong /*sessionId*/,
                                           mediacontrol::IAdaptiveStreamingPlayer::State state)
{
    int proplist[2], count = 0;
    if (state == IAdaptiveStreamingPlayer::CLOSED) {
        nrdApp()->sendEvent(ApplicationEvent::Event_PlaybackExited);
        if (shared_ptr<mediacontrol::IMediaSourcePlayer> p = player())
            p->removeListener(mListener);

    } else if (state == IAdaptiveStreamingPlayer::OPENING) {
        nrdApp()->sendEvent(ApplicationEvent::Event_PlaybackEntered);
    }

    proplist[count++] = Properties::MediaSourcePlayerBridge::bufferPoolSize;
    proplist[count++] = Properties::MediaSourcePlayerBridge::state;

    mTime0 = 0;

    Variant map;
    map[PROPUPS] = properties(proplist, count);
    map["type"] = "statechanged";
    map["state"] = state;
    NTRACE(TRACE_MSPBRIDGE, "sending event stateChanged to %d at %lld ms", static_cast<int> (state), Time::mono().val());
    sendPlayerEvent(map);
}

void MediaSourcePlayerBridge::openComplete(ullong sessionId)
{
    const int openProperties[] = {
        Properties::MediaSourcePlayerBridge::bufferPoolSize,
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
}

void MediaSourcePlayerBridge::updatePts(ullong /*sessionId*/, const Ticks& pts, const ullong& monotimeInMS)
{
    Time time(monotimeInMS);
    const ullong nowMS = Time::mono().ms();
    const ullong tmMS = pts.value(Ticks::RES_1KHZ);
    if (mTime0) {
        // if the wall clock time significantly differs from what media
        // control's view is, it implies a drift that should be investigated.
        llong diff = (mTime0 + tmMS) - nowMS;
        if (diff > 750 || diff < -750) {
            NTRACE(TRACE_MSPBRIDGE, "unexpected drift of %lld in player time: %llu + %llu = %llu != %llu",
                   diff, mTime0, tmMS, mTime0 + tmMS, nowMS);
        }
    }

    mTime0 = nowMS - tmMS;

    NTRACE(TRACE_MSPBRIDGE, "MediaSourcePlayerBridge::%s sending pts event for %lld", __func__, tmMS);

    Variant map;
    map["type"] = "updatepts";
    map["pts"] = variantFromTicks(pts);
    sendEvent("MediaSourcePlayer", map, 0, time);
}

class MediaSourcePlayerBridgeEvent;
static weak_ptr<MediaSourcePlayerBridgeEvent> sMediaSourcePlayerBridgeEvent;
static Mutex sMediaSourcePlayerBridgeEventLock(PLAYERBRIDGE_EVENT_MUTEX, "MediaSourcePlayerBridgeEventLock");
class MediaSourcePlayerBridgeEvent : public Application::Event
{
public:
    MediaSourcePlayerBridgeEvent(const shared_ptr<MediaSourcePlayerBridge> &bridge)
      : Event(HighPriority)
      , mBridge(bridge)
    {}

    virtual void eventFired() {
        {
            ScopedMutex _lock(sMediaSourcePlayerBridgeEventLock);
            sMediaSourcePlayerBridgeEvent.reset();
        }
        for(std::list<Variant>::const_iterator it = mEvents.begin(); it != mEvents.end(); ++it) {
            const Variant &data = (*it);
            mBridge->sendEvent("MediaSourcePlayer", data, Response::Flag_Synchronous);
        }
    }

    virtual std::string describe() const { return "MediaSourcePlayerBridgeEvent"; }
    void append(const Variant &event) {
        if(!mEvents.empty()) {
            const std::string type = event["type"].string();
            if(type == "updatepts") {
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
    shared_ptr<MediaSourcePlayerBridge> mBridge;
};

void MediaSourcePlayerBridge::sendPlayerEvent(const Variant &data)
{
    sendEvent("MediaSourcePlayer", data);
}

void MediaSourcePlayerBridge::PlaybackReporterEvent(const char *function, const Variant &args)
{
    Variant vmap;
    vmap["type"] = "PlaybackReporter";
    vmap["funcname"] = function;
    vmap["args"] = args;
    sendEvent("PlaybackReporter", vmap);
}

shared_ptr<IMediaSourcePlayer> MediaSourcePlayerBridge::createPlayer()
{
    ScopedMutex lock(mPlayerMutex);

    if (!mPlayer) {
        NTRACE(TRACE_MSPBRIDGE, "MediaSourcePlayerBridge::createPlayer");
        mPlayer.reset(new NrdpMediaSourcePlayer);
        nrdApp()->setPlayer(name(), mPlayer);
        nrdApp()->setPlayerType(PLAYER_MEDIASOURCEPLAYER);
    }
    return mPlayer;
}

shared_ptr<mediacontrol::IMediaSourcePlayer> MediaSourcePlayerBridge::player() const
{
    ScopedMutex _lock(mPlayerMutex);
    return mPlayer;
}
