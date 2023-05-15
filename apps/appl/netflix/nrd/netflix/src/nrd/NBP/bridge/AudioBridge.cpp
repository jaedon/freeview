/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioBridge.h"

#include <nrdbase/ScopedMutex.h>

#include <nrdnet/ResourceManager.h>

#include "NrdApplication.h"

using namespace netflix;
using namespace netflix::device;

#define AudioPropertyList(Property)             \
    Property(capability, NfObject::ReadOnly)    \
    Property(codecs, NfObject::ReadOnly)        \
    Property(urls, NfObject::ReadOnly)

PropertyTable(AudioBridge, AudioPropertyList, properties)

#define MethodList(Method)                      \
    Method(getVolume)                           \
    Method(load)                                \
    Method(play)                                \
    Method(setVolume)                           \
    Method(stop)                                \
    Method(unload)                              \
    Method(unloadAll)


MethodTable(AudioBridge, MethodList)

AudioBridge::AudioBridge()
    : NfObject("audio", Properties::AudioBridge::properties, Properties::AudioBridge::PropertyCount,
               Methods::AudioBridge::methods, Methods::AudioBridge::MethodCount),
      mSoundMapMutex(AUDIOBRIDGE_MUTEX, "AudioBridge"),
      mShuttingDown(false),
      mSetVolumeMutex(AUDIOBRIDGE_MUTEX, "SetVolume")
{
    mMixer = nrdApp()->getDeviceLib()->getAudioMixer();
}

AudioBridge::~AudioBridge()
{
    mShuttingDown = true;
    SoundMapType::iterator it;
    for (it = mSoundMap.begin(); it != mSoundMap.end(); it++) {
        if (mMixer) {
            if (!it->second.playQueue.empty())
                // we do not fade out for implicit stop
                mMixer->stopClip(it->second.clip, it->first, it->second.playId);
            mMixer->freeClip(it->second.clip, it->first);
        }
    }
}

bool AudioBridge::getProperty(int index, Variant *result) const
{
    if (mShuttingDown)
        return false;

    if (!mMixer) {
        *result = Variant();
        return true;
    }

    switch (index) {
    case Properties::AudioBridge::capability: {
        const IAudioMixer::Capability c = mMixer->getCapability();
        *result = Variant();
        (*result)["uiSoundBufferSize"] = c.uiSoundBufferSize;
        (*result)["uiSoundBufferTransferSize"] = c.uiSoundBufferTransferSize;
        (*result)["uiSoundBufferSizeEstimate"] = c.uiSoundBufferSizeEstimate;
        break;
    }
    case Properties::AudioBridge::codecs: {
        Variant arr(Variant::Type_Array);
        std::vector<AudioCodecType> codecs = mMixer->supportedCodecs();
        for (size_t i = 0; i < codecs.size(); i++) {
            arr.push_back(audioCodecTypeToString(codecs[i]));
        }
        *result = arr;
        break;
    }
    case Properties::AudioBridge::urls: {
        Variant arr(Variant::Type_Array);
        SoundMapType::const_iterator it;
        {
            ScopedMutex sm(mSoundMapMutex);
            for (it = mSoundMap.begin(); it != mSoundMap.end(); it++) {
                if (it->second.clip)
                    arr.push_back(it->first);
            }
        }
        *result = arr;
        break;
    }
    default:
        return false;
    }
    return true;
}

void AudioBridge::sendAudioEvent(int id, const std::string &URL, bool success, unsigned int size)
{
    if (id == -1)
        return;

    Variant map;
    map["id"] = id;
    if (!URL.empty()) {
        map["URL"] = URL;
    }
    map["success"] = success;
    map["size"] = size;
    sendEvent("Audio", map);
}

void AudioBridge::capabilityChanged()
{
    propertyUpdated(Properties::AudioBridge::capability);
}

void AudioBridge::load(const std::string &URL, int cbId)
{
    ScopedMutex sm(mSoundMapMutex);
    NTRACE(TRACE_NBP, "AudioBridge::load(): URL=%s, mSoundMap.size()=%zu", URL.c_str(), mSoundMap.size());

    SoundMapType::iterator it;
    it = mSoundMap.find(URL);

    if (!mMixer) {
        sendAudioEvent(cbId, URL, false);

    } else if (it == mSoundMap.end()) {
        // URL is new
        NTRACE(TRACE_NBP, "AudioBridge::load(): add new load request, cbId=%d, URL=%s", cbId, URL.c_str());
        struct Sound sound;
        sound.clip = NULL;
        sound.playId = 0;
        sound.unloadPending = false;
        sound.loadQueue.push(cbId);
        mSoundMap.insert(std::pair<std::string, struct Sound>(URL, sound));
        addRequest(URL);

    } else if (it->second.unloadPending) {
        // unload is pending
        NTRACE(TRACE_NBP, "AudioBridge::load(): reject load request when unload is pending, cbId=%d, URL=%s", cbId, URL.c_str());
        sendAudioEvent(cbId, URL, false);

    } else if (!it->second.clip) {
        // Load is in progress, queue this request
        NTRACE(TRACE_NBP, "AudioBridge::load(): load is in progress, queue load request, cbId=%d, URL=%s", cbId, URL.c_str());
        it->second.loadQueue.push(cbId);

    } else {
        // URL is already loaded
        NTRACE(TRACE_NBP, "AudioBridge::load(): reject redundant load request, cbId=%d, URL=%s", cbId, URL.c_str());
        sendAudioEvent(cbId, URL, true);
    }
}

void AudioBridge::addRequest(const std::string &URL)
{
    shared_ptr<AudioBridge> that = static_pointer_cast<AudioBridge>(shared_from_this());
    ResourceRequest::SharedPtr req(new AudioBridgeResourceRequest(URL, that));
    nrdApp()->resourceManager()->sendRequest(req);
}

void AudioBridge::AudioBridgeResourceRequest::onComplete(const Resource::SharedPtr &resource, Resource::State)
{
    if (shared_ptr<AudioBridge> bridge = mBridge.lock()) {
        bool result = false;
        Variant map;
        SoundMapType::iterator it;
        int loadCbID;
        unsigned int clipSize = 0;
        bool urlErased = false;
        const std::string URL = resource->url().str();

        {
            ScopedMutex sm(bridge->mSoundMapMutex);

            NTRACE(TRACE_NBP, "AudioBridge::onLoadDoneCallback(): mSoundMap.size()=%zu", bridge->mSoundMap.size());

            it = bridge->mSoundMap.find(URL);

            if (it == bridge->mSoundMap.end()) {
                NTRACE(TRACE_NBP, "AudioBridge::onLoadDoneCallback(): URL not found, URL=%s", resource->url().c_str());
                return;
            }

            struct Sound & snd = it->second;

            if (snd.unloadPending) {
                // unload is pending
                NTRACE(TRACE_NBP, "AudioBridge::onLoadDoneCallback(): unloadPending is set, loadQueue.size()=%zu, URL=%s", snd.loadQueue.size(), URL.c_str());
                while (!snd.loadQueue.empty()) {
                    loadCbID = snd.loadQueue.front();
                    NTRACE(TRACE_NBP, "unloading cbID=%d, URL=%s", loadCbID, URL.c_str());
                    bridge->sendAudioEvent(loadCbID, URL, false);
                    snd.loadQueue.pop();
                }
                bridge->mSoundMap.erase(it);
                return;
            }

            loadCbID = snd.loadQueue.front();
            snd.loadQueue.pop();
            const DataBuffer data = resource->responseData();

            if (!data.size() || resource->httpStatusCode() != 200) {
                // Load failed
                NTRACE(TRACE_NBP, "AudioBridge::onLoadDoneCallback(): load failed, loadCbID=%d, URL=%s", loadCbID, URL.c_str());
                if (snd.loadQueue.empty()) {
                    bridge->mSoundMap.erase(it);
                    urlErased = true;
                }

            } else {
                // Load succeeded, we'll never get here if mMixer is NULL
                IAudioMixer::AudioClip clip = 0;
                NFErr err = bridge->mMixer->prepareClip(URL, data, clip, clipSize);
                if (err == NFErr_OK) {
                    NTRACE(TRACE_NBP, "AudioBridge::onLoadDoneCallback(): load successful, loadCbID=%d size=%d clip=%p, URL=%s", loadCbID, data.size(), clip, URL.c_str());
                    result = true;
                    snd.clip = clip;
                    NTRACE(TRACE_NBP, "AudioBridge::onLoadDoneCallback(): outstanding sound count = %zu", bridge->mSoundMap.size());
                } else {
                    NTRACE(TRACE_NBP, "AudioBridge::onLoadDoneCallback(): prepareClip returned %s, loadCbID=%d size=%d clip=%p, URL=%s", err.toString().c_str(), loadCbID, data.size(), clip, URL.c_str() );
                    bridge->mSoundMap.erase(it);
                    urlErased = true;
                }
            }

            if (result)
                bridge->propertyUpdated(Properties::AudioBridge::urls);
            bridge->sendAudioEvent(loadCbID, URL, result, result ? clipSize : 0);

            // Service load queue
            if (result) {
                NTRACE(TRACE_NBP, "AudioBridge::onLoadDoneCallback(): invalidating remaining %zu load requests", snd.loadQueue.size());
                while (!snd.loadQueue.empty()) {
                    bridge->sendAudioEvent(snd.loadQueue.front(), URL, false);
                    snd.loadQueue.pop();
                }
            } else if (!urlErased) {
                NTRACE(TRACE_NBP, "AudioBridge::onLoadDoneCallback(): send a new http load request");
                bridge->addRequest(URL);
            }
        }
    }
}

void AudioBridge::unloadAll(int id)
{
    bool urlUpdated = false;
    {
        ScopedMutex sm(mSoundMapMutex);
        NTRACE(TRACE_NBP, "AudioBridge::unloadAll(): mSoundMap.size()=%zu", mSoundMap.size());

        for (SoundMapType::iterator it = mSoundMap.begin(); it != mSoundMap.end();) {
            SoundMapType::iterator toDelete = it;
            it++;
            struct Sound & snd = toDelete->second;
            if (!snd.unloadPending) {
                if (snd.clip && !snd.playQueue.size()) {
                    // if there's no mMixer, we wouldn't have loaded anything, so
                    // would never get here
                    NTRACE(TRACE_NBP, "AudioBridge::unloadAll(): erasing URL=%s", toDelete->first.c_str());
                    mMixer->freeClip(snd.clip, toDelete->first);
                    mSoundMap.erase(toDelete);
                    urlUpdated = true;
                    NTRACE(TRACE_NBP, "AudioBridge::unloadAll(): outstanding sound count = %zu", mSoundMap.size());
                    continue;
                } else {
                    snd.unloadPending = true;
                    if (snd.playQueue.size()) {
                        NTRACE(TRACE_NBP, "AudioBridge::unloadAll(): play is in flight, URL=%s", toDelete->first.c_str());
                        // we do not fade out for implicit stop
                        mMixer->stopClip(snd.clip, toDelete->first, snd.playId);
                    } else {
                        NTRACE(TRACE_NBP, "AudioBridge::unloadAll(): load is in flight, URL=%s", toDelete->first.c_str());
                    }
                }
            } else {
                NTRACE(TRACE_NBP, "AudioBridge::unloadAll(): unloadPending is already set, URL=%s", toDelete->first.c_str());
            }
        }
    }
    if (urlUpdated)
        propertyUpdated(Properties::AudioBridge::urls);
    sendAudioEvent(id, std::string(), true);
}

double AudioBridge::getVolume()
{
    NTRACE(TRACE_NBP, "AudioBridge::getVolume()");

    return mMixer->getVolume();
}

void AudioBridge::setVolume(double vol, int transitionMs, IAudioMixer::Ease ease)
{
    NTRACE(TRACE_NBP, "AudioBridge::setVolume(): vol=%f transitionMs=%d ease=%d",
        vol, transitionMs, (int)ease);

    mMixer->setVolume(vol, transitionMs, ease);
}

void AudioBridge::unload(const std::string &URL, int id)
{
    NTRACE(TRACE_NBP, "AudioBridge::unload(): URL=%s", URL.c_str());

    bool urlUpdated = false;
    bool result = true;
    {
        ScopedMutex sm(mSoundMapMutex);
        NTRACE(TRACE_NBP, "AudioBridge::unload(): mSoundMap.size()=%zu", mSoundMap.size());

        SoundMapType::iterator it;
        it = mSoundMap.find(URL);

        if (it != mSoundMap.end()) {
            struct Sound & snd = it->second;
            if (!snd.unloadPending) {
                if (snd.clip && !snd.playQueue.size()) {
                    // if there's no mMixer, we wouldn't have loaded anything, so
                    // would never get here
                    NTRACE(TRACE_NBP, "AudioBridge::unload(): erasing URL=%s", it->first.c_str());
                    mMixer->freeClip(snd.clip, it->first);
                    mSoundMap.erase(it);
                    urlUpdated = true;
                    NTRACE(TRACE_NBP, "AudioBridge::unload(): outstanding sound count = %zu", mSoundMap.size());
                } else {
                    snd.unloadPending = true;
                    if (snd.playQueue.size()) {
                        NTRACE(TRACE_NBP, "AudioBridge::unload(): play is in flight, URL=%s", it->first.c_str());
                        // we do not fade out for implicit stop
                        mMixer->stopClip(snd.clip, URL, snd.playId);
                    } else {
                        NTRACE(TRACE_NBP, "AudioBridge::unload(): load is in flight, URL=%s", it->first.c_str());
                    }
                }
            } else {
                NTRACE(TRACE_NBP, "AudioBridge::unload(): unloadPending is already set, URL=%s", it->first.c_str());
            }
        } else {
            NTRACE(TRACE_NBP, "AudioBridge::unload(): not found, URL=%s", URL.c_str());
            result = false;
        }
    }

    if (urlUpdated)
        propertyUpdated(Properties::AudioBridge::urls);
    sendAudioEvent(id, URL, result);
}

void AudioBridge::play(const std::string &URL, double volume, int fadeInMs, int fadeOutMs, IAudioMixer::Ease fadeInEase, IAudioMixer::Ease fadeOutEase, int id)
{
    NTRACE(TRACE_NBP, "AudioBridge::play(): URL=%s", URL.c_str());

    ScopedMutex sm(mSoundMapMutex);
    NTRACE(TRACE_NBP, "AudioBridge::play(): mSoundMap.size()=%zu", mSoundMap.size());

    SoundMapType::iterator it;
    it = mSoundMap.find(URL);
    if (it == mSoundMap.end()) {
        NTRACE(TRACE_NBP, "AudioBridge::play(): not found, URL=%s", URL.c_str());
        sendAudioEvent(id, URL, false);
        return;
    }

    struct Sound & snd = it->second;

    if (!snd.clip) {
        NTRACE(TRACE_NBP, "AudioBridge::play(): sound is downloading, URL=%s", URL.c_str());
        sendAudioEvent(id, URL, false);
        return;
    }
    if (snd.unloadPending) {
        NTRACE(TRACE_NBP, "AudioBridge::play(): unloadPending is set, URL=%s", URL.c_str());
        sendAudioEvent(id, URL, false);
        return;
    }

    if (!snd.playQueue.empty()) {
        // Stop current play, queue play request
        NTRACE(TRACE_NBP, "AudioBridge::play(): Stop current play, queue play request, URL=%s", URL.c_str());
        // if there's no mMixer, we wouldn't have loaded anything, so
        // would never get here

        // we do not fade out for implicit stop
        mMixer->stopClip(snd.clip, URL, snd.playId);
    }

    // Don't want to make assumptions about the DPI handling stop synchronously
    if (snd.playQueue.empty()) {
        IAudioMixer::PlayId playId;
        // We always want play end to callback internally
        NFErr err = mMixer->playClip(snd.clip, volume, this, URL, playId, fadeInMs, fadeOutMs, fadeInEase, fadeOutEase);
        if (err != NFErr_OK) {
            NTRACE(TRACE_NBP, "AudioBridge::play(): playClip returns err, URL=%s", URL.c_str());
            sendAudioEvent(id, URL, false);
            return;
        }
        NTRACE(TRACE_NBP, "AudioBridge::play(): playing, URL=%s", URL.c_str());

        snd.playId = playId;
    }

    struct PlayRequest pr;
    pr.playCbID = id;
    pr.volume = volume;
    pr.fadeInMs = fadeInMs;
    pr.fadeOutMs = fadeOutMs;
    pr.fadeInEase = fadeInEase;
    pr.fadeOutEase = fadeOutEase;
    snd.playQueue.push(pr);
}

void AudioBridge::playDone(const std::string &URL)
{
    NTRACE(TRACE_NBP, "AudioBridge::playDone(): URL=%s", URL.c_str());

    if (mShuttingDown)
        return;

    Variant map(Variant::Type_StringMap);
    SoundMapType::iterator it;
    bool urlUpdated = false;

    {
        ScopedMutex sm(mSoundMapMutex);
        NTRACE(TRACE_NBP, "AudioBridge::playDone(): mSoundMap.size()=%zu", mSoundMap.size());

        it = mSoundMap.find(URL);

        assert(it != mSoundMap.end());

        struct Sound & snd = it->second;

        if (snd.unloadPending) {
            NTRACE(TRACE_NBP, "AudioBridge::playDone(): unloadPending is set, playQueue.size=%zu, URL=%s", snd.playQueue.size(), URL.c_str());
            // unload is pending
            while (!snd.playQueue.empty()) {
                sendAudioEvent(snd.playQueue.front().playCbID, URL, false);
                snd.playQueue.pop();
            }
            mMixer->freeClip(snd.clip, it->first);
            mSoundMap.erase(it);
            urlUpdated = true;

        } else  {

            /* walk the play queue. the first one just finished, it's successful. all
             * but the last one are about to get skipped, so they're all unsuccessful.
             * then the last one has play() called on it. */
            bool success = true;
            do {
                struct PlayRequest pr = snd.playQueue.front();
                snd.playQueue.pop();

                // Make callback
                sendAudioEvent(pr.playCbID, URL, success);

                success = false;
            } while (snd.playQueue.size() > 1);

            snd.playId = 0; // indicate play is done

            if (!snd.playQueue.empty()) {
                // Play next request
                struct PlayRequest pr = snd.playQueue.front();
                IAudioMixer::PlayId playId;
                NFErr err = mMixer->playClip(snd.clip, pr.volume, this, URL, playId, pr.fadeInMs, pr.fadeOutMs, pr.fadeInEase, pr.fadeOutEase);
                if (err != NFErr_OK) {
                    NTRACE(TRACE_NBP, "AudioBridge::playDone(): playClip returns err, URL=%s", URL.c_str());
                    sendAudioEvent(pr.playCbID, URL, false);
                    snd.playQueue.pop();
                    return;
                }
                NTRACE(TRACE_NBP, "AudioBridge::playDone(): playing next request, URL=%s", URL.c_str());

                snd.playId = playId;
            }
        }
    }
    if (urlUpdated)
        propertyUpdated(Properties::AudioBridge::urls);
}

void AudioBridge::stop(const std::string &URL, int fadeOutMs, IAudioMixer::Ease fadeOutEase, int id)
{
    NTRACE(TRACE_NBP, "AudioBridge::stop(): URL=%s", URL.c_str());

    bool result = false;
    {
        ScopedMutex sm(mSoundMapMutex);
        NTRACE(TRACE_NBP, "AudioBridge::stop(): mSoundMap.size()=%zu", mSoundMap.size());

        SoundMapType::iterator it, itend;
        it = mSoundMap.find(URL);

        if (it == mSoundMap.end()) {
            NTRACE(TRACE_NBP, "AudioBridge::stop(): not found, URL=%s", URL.c_str());
        } else if (!it->second.playId) {
            NTRACE(TRACE_NBP, "AudioBridge::stop(): sound is not playing, URL=%s", URL.c_str());
        } else if (it->second.unloadPending) {
            NTRACE(TRACE_NBP, "AudioBridge::stop(): unloadPending is set, URL=%s", URL.c_str());
        } else {
            struct Sound & snd = it->second;

            NTRACE(TRACE_NBP, "AudioBridge::stop(): deleting all pending play requests, playQueue.size()=%zu, URL=%s", snd.playQueue.size(), URL.c_str());
            // Delete all pending play requests
            if (snd.playQueue.size() > 1) {
                struct PlayRequest pr = snd.playQueue.front();
                while (!snd.playQueue.empty()) {
                    snd.playQueue.pop();
                }
                snd.playQueue.push(pr);
            }

            // if there's no mMixer, we wouldn't have loaded anything, so
            // would never get here
            NFErr err = mMixer->stopClip(snd.clip, URL, snd.playId, fadeOutMs, fadeOutEase);

            if (err == NFErr_OK) {
                NTRACE(TRACE_NBP, "AudioBridge::stop(): called stopClip, URL=%s", URL.c_str());
                result = true;
            } else {
                NTRACE(TRACE_NBP, "AudioBridge::stop(): stopClip returns err, URL=%s", URL.c_str());
            }
        }
    }

    sendAudioEvent(id, URL, result);
}

Variant AudioBridge::invoke(int method, const Variant &data)
{
    if (mShuttingDown)
        return false;

    Variant returnValue;

    const int id = data.mapValue<int>("id", 0, -1);
    switch (method) {
    case Methods::AudioBridge::load: {
        const std::string URL = data.mapValue<std::string>("URL");
        load(URL, id);
        break; }
    case Methods::AudioBridge::unload: {
        const std::string URL = data.mapValue<std::string>("URL");
        unload(URL, id);
        break; }
    case Methods::AudioBridge::unloadAll:
        unloadAll(id);
        break;
    case Methods::AudioBridge::play:
        {
            const std::string URL = data.mapValue<std::string>("URL");

            // check volume validity
            bool ok;
            double volume = data.mapValue<double>("volume", &ok);
            if (ok) {
                if (volume < 0.0) {
                    NTRACE(TRACE_NBP, "AudioBridge::invoke(): warning, play: URL=%s, volume < 0, set to 0", URL.c_str());
                    volume = 0.0;
                } else if (volume > 1.0) {
                    NTRACE(TRACE_NBP, "AudioBridge::invoke(): warning, play: URL=%s, volume > 1, set to 1", URL.c_str());
                    volume = 1.0;
                }
            } else {
                NTRACE(TRACE_NBP, "AudioBridge::invoke(): play(): URL=%s, volume unspecified, set to 1", URL.c_str());
                volume = 1.0;
            }

            const int fadeInMs = data.mapValue<int>("fadeInMs");
            const int fadeOutMs = data.mapValue<int>("fadeOutMs");

            IAudioMixer::Ease fadeInEase = (IAudioMixer::Ease)data.mapValue<int>("fadeInEase");
            IAudioMixer::Ease fadeOutEase = (IAudioMixer::Ease)data.mapValue<int>("fadeOutEase");

            // check fadeInEase validity
            if (fadeInEase < 0 || fadeInEase >= IAudioMixer::EaseCount) {
                NTRACE(TRACE_NBP, "AudioBridge::invoke(): warning, play: URL=%s, invalid fadeInEase specified, set to EaseLinear", URL.c_str());
                fadeInEase = IAudioMixer::EaseLinear;
            }

            // check fadeOutEase validity
            if (fadeOutEase < 0 || fadeOutEase >= IAudioMixer::EaseCount) {
                NTRACE(TRACE_NBP, "AudioBridge::invoke(): warning, play: URL=%s, invalid fadeOutEase specified, set to EaseLinear", URL.c_str());
                fadeOutEase = IAudioMixer::EaseLinear;
            }

            play(URL, volume, fadeInMs, fadeOutMs, fadeInEase, fadeOutEase, id);
            break;
        }
    case Methods::AudioBridge::stop:
        {
            const std::string URL = data.mapValue<std::string>("URL");
            const int fadeOutMs = data.mapValue<int>("fadeOutMs");
            IAudioMixer::Ease fadeOutEase = (IAudioMixer::Ease)data.mapValue<int>("fadeOutEase");

            // check fadeOutEase validity
            if (fadeOutEase < 0 || fadeOutEase >= IAudioMixer::EaseCount) {
                NTRACE(TRACE_NBP, "AudioBridge::invoke(): warning, stop: URL=%s, invalid fadeOutEase specified, set to EaseLinear", URL.c_str());
                fadeOutEase = IAudioMixer::EaseLinear;
            }

            stop(URL, fadeOutMs, fadeOutEase, id);
            break;
        }
    case Methods::AudioBridge::getVolume: {
        Variant map;
        map["id"] = id;
        map["data"] = getVolume();
        sendEvent("getVolume", map);
        break; }
    case Methods::AudioBridge::setVolume:
        {
            ScopedMutex sm(mSetVolumeMutex);

            // check volume validity
            bool ok;
            double volume = data.mapValue<double>("volume", &ok);
            if (ok) {
                if (volume < 0.0) {
                    NTRACE(TRACE_NBP, "AudioBridge::invoke(): warning, setVolume: volume < 0, set to 0");
                    volume = 0.0;
                } else if (volume > 1.0) {
                    NTRACE(TRACE_NBP, "AudioBridge::invoke(): warning, setVolume: volume > 1, set to 1");
                    volume = 1.0;
                }
            } else {
                NTRACE(TRACE_NBP, "AudioBridge::invoke(): setVolume(): volume unspecified, set to 1");
                volume = 1.0;
            }

            const int transitionMs = data.mapValue<int>("transitionMs");
            IAudioMixer::Ease ease = (IAudioMixer::Ease)data.mapValue<int>("ease");

            // check ease validity
            if (ease < 0 || ease >= IAudioMixer::EaseCount) {
                NTRACE(TRACE_NBP, "AudioBridge::invoke(): warning, setVolume: invalid ease specified, set to EaseLinear");
                ease = IAudioMixer::EaseLinear;
            }

            setVolume(volume, transitionMs, ease);
            break;
        }
    default:
        return false;
    }
    return true;
}
