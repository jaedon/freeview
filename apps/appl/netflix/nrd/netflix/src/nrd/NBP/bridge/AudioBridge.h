/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef AudioBridge_h__
#define AudioBridge_h__

#include <queue>

#include <nrdbase/Mutex.h>

#include <nrdnet/ResourceRequest.h>
#include <nrdnet/Resource.h>

#include <nrd/IAudioMixer.h>
#include <nrd/NfObject.h>

namespace netflix {

class AudioBridge : public NfObject,
                    public device::IAudioMixer::AudioListener
{
public:
    AudioBridge();
    ~AudioBridge();

    virtual bool getProperty(int index, Variant *result) const;
    virtual Variant invoke(int method, const Variant &data);

    virtual void playDone(const std::string &URL);
    virtual void capabilityChanged();

private:

    class AudioBridgeResourceRequest : public ResourceRequest
    {
    public:
        AudioBridgeResourceRequest(const AseUrl &url, const shared_ptr<AudioBridge> &bridge)
            : ResourceRequest(0, url, Resource::Method_Get), mBridge(bridge)
        {}

        virtual void onComplete(const Resource::SharedPtr &resource, Resource::State state);
    private:
        weak_ptr<AudioBridge> mBridge;
    };

    struct PlayRequest {
        int playCbID;
        double volume;
        int fadeInMs;
        int fadeOutMs;
        device::IAudioMixer::Ease fadeInEase;
        device::IAudioMixer::Ease fadeOutEase;
    };

    struct Sound {
        device::IAudioMixer::AudioClip clip;

        std::queue<int> loadQueue;
        std::queue<struct PlayRequest> playQueue;

        // Used by 'stop'
        device::IAudioMixer::PlayId playId;

        bool unloadPending;
    };

    void sendAudioEvent(int id, const std::string &URL, bool success, unsigned int size=0);

    void load(const std::string &URL, int cbId);
    void addRequest(const std::string &URL);
    void unload(const std::string &URL, int id);
    void unloadAll(int id);

    void play(const std::string &URL, double volume, int fadeInMs, int fadeOutMs, device::IAudioMixer::Ease fadeInEase, device::IAudioMixer::Ease fadeOutEase, int id);
    void stop(const std::string &URL, int fadeOutMs, device::IAudioMixer::Ease fadeOutEase, int id);
    double getVolume();
    void setVolume(double vol, int transitionMs, device::IAudioMixer::Ease ease);

    typedef std::map<std::string, struct Sound> SoundMapType;
    SoundMapType mSoundMap;
    mutable Mutex mSoundMapMutex;
    shared_ptr<netflix::device::IAudioMixer> mMixer;
    bool mShuttingDown;
    Mutex mSetVolumeMutex;

    friend class AudioBridgeResourceRequest;
};

}

#endif // AudioBridge_h__
