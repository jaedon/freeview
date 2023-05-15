/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef PlayerBridge_h
#define PlayerBridge_h

#include <nrd/NfObject.h>
#include <nrd/Response.h>
#include "IAdaptiveStreamingPlayer.h"
namespace netflix {


class PlayerBridge : public NfObject
{
public:
    PlayerBridge();
    ~PlayerBridge();

    void shutdown();
    Variant invoke(int method, const Variant &data);

    virtual unsigned requestFlags(const Request &) const;
    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);

    // IAdaptiveStreamingPlayer::Listener interface
    void stateChanged(ullong, mediacontrol::IAdaptiveStreamingPlayer::State);
    void openComplete(ullong);
    void bufferingComplete(ullong, bool faststart);
    void updatePts(ullong, const Ticks&, const ullong&);
    void updateContentDuration(ullong, uint32_t);
    void audioTrackChanged(ullong, uint32_t, std::string const&);
    void audioTrackChangeStarted(ullong);
    void audioTrackChangeComplete(ullong);
    void audioTrackChangeRejected(ullong);
    void manifestSelected(ullong, int);
    void hevcInfo(ullong, const std::string &);

    // IAdaptiveStreamingPlayer::LicenseHandler interface
    NFErr provideChallenge(std::string const& drmType,
                           std::string const& streamID,
                           const std::vector<unsigned char>& challenge,
                           const std::vector<unsigned char>& drmHeader);
    NFErr provideChallenge(const std::vector<unsigned char>& challenge);
    void provideRelease(std::string const& drmType, std::string const& release);
    void notifyBufferingComplete();
    NFErr checkStatus();

private:
    void close();

private:
    // helper classes for IAdaptiveStreamingPlayer interfaces
    class ASPListener;
    shared_ptr<ASPListener> mListener;
    friend class ASPListener;

    class ASPLicenseHandler;
    shared_ptr<ASPLicenseHandler> mLicenseHandler;

    mutable Mutex mPlayerMutex;
    shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> mPlayer;
    void sendPlayerEvent(const Variant &data);
    inline shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> player() const {
        ScopedMutex _lock(mPlayerMutex);
        return mPlayer;
    }
    shared_ptr<mediacontrol::IAdaptiveStreamingPlayer> createPlayer();

    class PlaybackReporter;
    shared_ptr<PlaybackReporter> mPlaybackReporter;
    friend class PlaybackReporter;
    void PlaybackReporterEvent(const char *fn, const Variant &args = Variant());

    ullong mTime0;

    // information necessary for openAndPlay() tracking
    Ticks mPlayPts;
    bool mOpenAndPause;
};
}

#endif
