/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __EVENT_THREAD_H__
#define __EVENT_THREAD_H__

#include "IAdaptiveStreamingPlayer.h"
#include "ListenerEventQueue.h"
#include "McThread.h"

namespace netflix {

namespace mediacontrol {

class AdaptiveStreamingPlayer;
class PlayerState;

class EventThread : public McThread
{
public:
    typedef IAdaptiveStreamingPlayer::Listener Listener;

public:
    EventThread(AdaptiveStreamingPlayer &player,
                shared_ptr<PlayerState> playerState);
    virtual ~EventThread() {}

    /**
     * Sets the sessionID sent with listener events so that listener can verify the
     * event matches its session
     */
    void setSessionId(ullong sessionId) { m_sessionId = sessionId; }

    /**
     * adds/removes listeners
     */
    void addListener(shared_ptr<Listener> l);
    void removeListener(shared_ptr<Listener> l);

    /**
     * Post a listener event into the listener event queue, so that the event thread
     * will call the notification functions when it gets this event from queue.
     *
     * If the listener event queue is full, inside this function it will keep on
     * sleep-and-retry until it succeeds.
     *
     * @param[in] event the listener event object to be posted into queue.
     */
    void postEvent(shared_ptr<ListenerEvent<Listener> > event);

    /**
     * schedule the enabling/disabling of device events handling
     * (actual work is done in thread at later time)
     */
    void enableDeviceEvents();
    void disableDeviceEvents();

public:  // listener event post helpers

    /** Post a state-change event into the listener event queue. */
    inline void postStateChangedEvent(IAdaptiveStreamingPlayer::State newState)
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new StateChangedEvent<Listener>(m_sessionId, newState)));
    }

    /** Post an open-complete event into the listener event queue. */
    inline void postOpenCompleteEvent()
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new OpenCompleteEvent<Listener>(m_sessionId)));
    }

    /** Post a stream-selected event into the listener event queue. */
    inline void postStreamSelectedEvent(int manifest, int track, int stream,
                                        uint32_t startPts, uint32_t rebuffer)
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new PlayerStreamSelectedEvent<Listener>(
                          m_sessionId, manifest, track, stream, startPts, rebuffer)));
    }

    /** Post a stream presenting event into the listener event queue. */
    inline void postStreamPresentingEvent(uint32_t manifest, uint32_t track,
                                          uint32_t index, uint32_t startPTS)
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new PlayerStreamPresentingEvent<Listener>(
                          m_sessionId, manifest, track, index, startPTS)));
    }

    /** Post a buffering-progress event into the listener event queue. */
    inline void postBufferingEvent(uint32_t percentage)
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new BufferingEvent<Listener>(m_sessionId, percentage)));
    }

    /** Post a buffering-complete event into the listener event queue. */
    inline void postBufferingCompleteEvent(bool faststart)
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new BufferingCompleteEvent<Listener>(m_sessionId, faststart)));
    }

    /** Post an updatePTS event to the listener event queue. */
    void postUpdatePTSEvent(uint32_t pts, ullong monotimeInMS);

    /** Post an updateContentDuration event to listener event queue. */
    inline void postUpdateContentDurationEvent(uint32_t duration)
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new UpdateContentDurationEvent<Listener>(m_sessionId, duration)));
    }

    /** Post a video bitrate selection complete event into the listener event queue. */
    inline void postVideoBitrateSelectionCompleteEvent()
    {
        postEvent(
            shared_ptr<ListenerEvent<Listener> >(
                new VideoBitrateSelectionCompleteEvent<Listener>(m_sessionId)));
    }

    /** Post a video bitrate update event to the listener event queue. */
    inline void postUpdateVideoBitrateEvent(uint32_t bitrate)
    {
        postEvent(
            shared_ptr<ListenerEvent<Listener> >(
                new UpdateVideoBitrateEvent<Listener>(m_sessionId, bitrate)));
    }

    /** Post a tracks changed event into the listener event queue. */
    inline void postAudioTrackChangedEvent(uint32_t trackIndex, std::string const&trackId)
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new AudioTrackChangedEvent<Listener>(m_sessionId, trackIndex, trackId)));
    }

    /** Post an error event into the listener event queue. */
    inline void postErrorEvent(const NFErr &err)
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new ErrorEvent<Listener>(m_sessionId, err)));
    }

    /** Post an video-window-change event into the listener event queue. */
    inline void postVideoWindowChangedEvent(const Rect& actualRect)
    {
        postEvent(
            shared_ptr<ListenerEvent<Listener> >(
                new VideoWindowChangedEvent<Listener>(m_sessionId, actualRect)));
    }

    /** Post a restartPlayback event into the listener event queue. */
    inline void postRestartPlaybackEvent(const NFErr &err, const Ticks &pts)
    {
        postEvent(
            shared_ptr<ListenerEvent<Listener> >(
                new RestartPlaybackEvent<Listener>(m_sessionId, err, pts)));
    }

    /** Post a manifestSelected event into the listener event queue */
    inline void postManifestSelectedEvent(int manifestIndex)
    {
        postEvent(
            shared_ptr<ListenerEvent<Listener> >(
                new ManifestSelectedEvent<Listener>(m_sessionId, manifestIndex)));
    }

private:
    virtual void Run();

    /**
     * Call the listeners' notification functions for the specified listener event.
     * @param[in] event the listener event to be notified.
     */
    void callListener(const ListenerEvent<Listener> &event);

    /**
     * Try sending an event from the queue to all listeners
     * @return true if an event was handled
     */
    bool handleEventQueue();

    /**
     * handle the enabling/disabling of device events in the event thread
     */
    void handleDisableDeviceEvents();
    bool handleEnableDeviceEvents();

    /**
     * handle any device events we might have
     * @return true if a device event has handled
     */
    bool handleDeviceEvents();

    /**
     * handle specific device events
     */
    void handlePTSUpdate(ullong param1, ullong param2);
    void handleDataExhausted();
    void handleEndOfStream();
    void handleDeviceError(ullong param1, ullong param2, const std::string &param3);
    void handleHevcInfo(const std::string &info);

private:
    static const uint32_t LISTENER_EVENT_QUEUE_SIZE = 64;

    enum State
    {
        DISABLE_DEVICE_EVENTS,
        DEVICE_EVENTS_DISABLED,
        ENABLE_DEVICE_EVENTS,
        DEVICE_EVENTS_ENABLED
    };

private:
    State m_state;
    AdaptiveStreamingPlayer &m_player;
    shared_ptr<PlayerState> m_playerState;

    ullong m_sessionId;

    // registered listeneres
    mutable Mutex m_listenerMutex;
    std::vector<shared_ptr<Listener> > m_listeners;

    // queue of listener events
    shared_ptr<ListenerEventQueue<Listener> > m_eventQueue;
    uint32_t m_eventCount;
    ullong m_lastPTS;
    bool m_firstPTSUpdate;
};

}} // namespace netflix::mediacontrol

#endif // __EVENT_THREAD_H__
