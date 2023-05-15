/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NRDP_MEDIA_EVENT_THREAD_H__
#define __NRDP_MEDIA_EVENT_THREAD_H__

#include <nrd/IAdaptiveStreamingPlayer.h> // only for IAdativeStreamingPlayer::State
#include <nrd/IMediaSourcePlayer.h>
#include "ListenerEventQueue.h"
#include "McThread.h"

namespace netflix {

namespace mediacontrol {

class NrdpMediaSourcePlayer;

class NrdpMediaEventThread : public McThread
{
public:
    typedef IMediaSourcePlayer::Listener Listener;

public:
    NrdpMediaEventThread(NrdpMediaSourcePlayer &player);
    virtual ~NrdpMediaEventThread() {}

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

    // sekwon : do we still need this?
    /** Post a stream presenting event into the listener event queue. */
    /*
    inline void postStreamPresentingEvent(uint32_t manifest, uint32_t track,
                                          uint32_t index, uint32_t startPTS)
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new PlayerStreamPresentingEvent<Listener>(
                          m_sessionId, manifest, track, index, startPTS)));
    }
    */

    /** Post a skipped event into the listener event queue. */
    inline void postSkippedEvent()
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new SkippedEvent<Listener>(m_sessionId)));
    }

    /** Post an updatePTS event to the listener event queue. */
    void postUpdatePTSEvent(uint32_t pts, ullong monotimeInMS);

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

    /** Post a tracks changed event into the listener event queue. */
    inline void postAudioTrackChangeStartedEvent()
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                    new AudioTrackChangeStartedEvent<Listener>(m_sessionId)));
    }

    inline void postAudioTrackChangeCompleteEvent()
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                    new AudioTrackChangeCompleteEvent<Listener>(m_sessionId)));
    }

    inline void postAudioTrackChangeRejectedEvent()
    {
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                    new AudioTrackChangeRejectedEvent<Listener>(m_sessionId)));
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
    NrdpMediaSourcePlayer& m_player;
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
