/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef PLAYBACKGROUPNATIVE_H
#define PLAYBACKGROUPNATIVE_H

/** @file PlaybackGroupNative.h - Reference implementation of the IPlaybackGroup
 * interface.
 *
 * PlaybackGroupNative is the reference application's implementation of the
 * IPlaybackGroup interface (see IElementaryStreamPlayer.h). A device partner
 * may use or modify this header and accompanying .cpp as needed.
 *
 * A PlaybackGroupNative is created each time a movie is loaded for viewing and
 * persists until the movie is unloaded. The playback group is used to
 * instantiate video and audio elementary stream players,to set the playback
 * state of the movie presentation to play or pause, and to control the video
 * window as defined in IElementaryStreamPlayer.h.
 *
 */


#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/IDeviceError.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/NFErr.h>
#include <set>

namespace netflix {
namespace device {
namespace esplayer {


class ESManagerNative;
class ESPlayerNative;
class AudioESPlayerNative;
class VideoESPlayerNative;


class PlaybackGroupNative : public IPlaybackGroup
{

public:
    virtual ~PlaybackGroupNative();
    PlaybackGroupNative(ESManagerNative& ESManager, uint32_t pipelineId);

    virtual netflix::NFErr
    createStreamPlayer(const struct StreamPlayerInitData& initData,
                       shared_ptr<IESPlayerCallback> callback,
                       IElementaryStreamPlayer*& pStreamPlayer);
    virtual void destroyStreamPlayer(IElementaryStreamPlayer* streamPlayer);
    virtual bool setPlaybackState(PlaybackState state);
    virtual IPlaybackGroup::PlaybackState getPlaybackState();
    virtual void flush();
    virtual Rect setVideoWindow(const Rect& targetRect, uint32_t transitionDuration); // to be deprecated
    virtual Rect setVideoWindow(const Rect& targetRect, uint32_t transitionDuration, uint32_t zorder = 0);
    virtual void bringVideoToFront();
    virtual void sendVideoToBack();

    ESManagerNative* getESManager();

    // The audio player, if it is flushed independent of the playback group, calls
    // this method so that the reference clock's current audio presentation time
    // can be invalidated.
    void audioFlushed();
    uint32_t getPipelineId() const;

private:
    bool streamPlayersAreReadyForPlaybackStart();

    // The video audio rendering threads access mPlaybackState. The state is
    // set in and SDK thread. The mPlaybackStateMutex should be held when
    // mPlaybackState is accessed.
    IPlaybackGroup::PlaybackState  mPlaybackState;
    Mutex                    mPlaybackStateMutex;

    ESManagerNative&                    mESManager;
    std::set<ESPlayerNative*>           mStreamPlayers;
    uint32_t mPipelineId;
};

} // namespace esplayer
} // device
} // namespace netflix



#endif

