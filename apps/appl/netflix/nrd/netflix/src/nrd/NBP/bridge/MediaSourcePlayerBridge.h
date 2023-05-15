/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MediaSourcePlayerBridge_h
#define MediaSourcePlayerBridge_h

#include <nrd/NfObject.h>
#include <nrd/Response.h>
#include "IMediaSourcePlayer.h"

namespace netflix {

class MediaSourcePlayerBridge : public NfObject
{
public:
    MediaSourcePlayerBridge(const std::string &name);
    ~MediaSourcePlayerBridge();

    void shutdown();
    Variant invoke(int method, const Variant &data);

    virtual bool getProperty(int index, Variant *result) const;
    //virtual bool setProperty(int index, const Variant &value);

    // IMediaSourcePlayer::Listener interface helper
    void stateChanged(ullong, mediacontrol::IAdaptiveStreamingPlayer::State);
    void openComplete(ullong);
    void updatePts(ullong, const Ticks&, const ullong&);

    NFErr checkStatus();

private:
    void pause();
    void close();

private:
    // listener
    class Listener;
    shared_ptr<Listener> mListener;
    friend class Listener;

    // reporter
    class PlaybackReporter;
    shared_ptr<PlaybackReporter> mPlaybackReporter;
    friend class PlaybackReporter;
    void PlaybackReporterEvent(const char *fn, const Variant &args = Variant());

    // player
    mutable Mutex mPlayerMutex;
    shared_ptr<mediacontrol::IMediaSourcePlayer> mPlayer;
    shared_ptr<mediacontrol::IMediaSourcePlayer> createPlayer();
    shared_ptr<mediacontrol::IMediaSourcePlayer> player() const;
    void sendPlayerEvent(const Variant &data);

    ullong mTime0;

    // information necessary for openAndPlay() tracking
    Ticks mPlayPts;
    bool mOpenAndPause;

    friend class MediaSourcePlayerFactoryBridge;
};
}

#endif
