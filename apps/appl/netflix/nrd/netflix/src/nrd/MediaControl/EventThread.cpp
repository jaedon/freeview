/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "EventThread.h"

#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Thread.h>

#include <nrd/config.h>
#include <nrd/IElementaryStreamPlayer.h>

#include "AdaptiveStreamingPlayer.h"
#include "PlaybackDeviceError.h"
#include "PlayerState.h"
#include <limits>

using namespace netflix;
using namespace netflix::device;
using namespace netflix::mediacontrol;

//////////
//
// EventThread
//
//////////
EventThread::EventThread(AdaptiveStreamingPlayer &player,
                         shared_ptr<PlayerState> playerState)
    : McThread(MCTHREAD_MUTEX, &THREAD_MC_EVENT),
      m_state(DEVICE_EVENTS_DISABLED),
      m_player(player),
      m_playerState(playerState),
      m_sessionId(0),
      m_listenerMutex(ASPEVENTLISTENER_MUTEX, "ASPEventListener"),
      m_eventCount(0),
      m_lastPTS(std::numeric_limits<ullong>::max()),
      m_firstPTSUpdate(false)
{
    // Create the listener event queue before the event thread gets started.
    m_eventQueue.reset(
        new ListenerEventQueue<Listener>(LISTENER_EVENT_QUEUE_SIZE));
}

//////////
//
// addListener
//
//////////
void EventThread::addListener(shared_ptr<Listener> l)
{
    ScopedMutex lock(m_listenerMutex);

    std::vector<shared_ptr<Listener> >::iterator n =
        std::find(m_listeners.begin(), m_listeners.end(), l);

    if (n == m_listeners.end())
        m_listeners.push_back(l);
}

//////////
//
// removeListener
//
//////////
void EventThread::removeListener(shared_ptr<Listener> l)
{
    ScopedMutex lock(m_listenerMutex);

    std::vector<shared_ptr<Listener> >::iterator n =
        std::find(m_listeners.begin(), m_listeners.end(), l);

    if (n != m_listeners.end())
        m_listeners.erase(n);
}

//////////
//
// postEvent
//
//////////
void EventThread::postEvent(shared_ptr<ListenerEvent<Listener> > event)
{
    static const Time WAIT_ON_LISTENER_QUEUE_FULL(30);
    bool errorLogged = false;

    while (!m_eventQueue->send(event))
    {
        if (!errorLogged)
        {
            Log::warn(TRACE_MEDIACONTROL, "Listener Event Queue Full");
            errorLogged = true;
        }

        Thread::Sleep(WAIT_ON_LISTENER_QUEUE_FULL); // sleep and retry.
    }

    ++m_eventCount;
}

//////////
//
// enableDeviceEvents
//
//////////
void EventThread::enableDeviceEvents()
{
    static const Time WAIT_FOR_DEVICE_EVENTS_ENABLED(30);

    ASSERT(m_state == DEVICE_EVENTS_DISABLED, "state = %d", m_state);

    // Change the state into ENABLE_DEVICE_EVENTS to request device event
    // enabling, wait for the completion of enabling by periodically checking
    // whether the state has been changed into DEVICE_EVENTS_ENABLED.

    lock();

    m_state = ENABLE_DEVICE_EVENTS;
    pinch();

    while (m_state != DEVICE_EVENTS_ENABLED)
        wait(WAIT_FOR_DEVICE_EVENTS_ENABLED);

    unlock();
}

//////////
//
// disableDeviceEvents
//
//////////
void EventThread::disableDeviceEvents()
{
    static const Time WAIT_FOR_DEVICE_EVENTS_DISABLED(30);

    // Change the state into DISABLE_DEVICE_EVENTS to request device event
    // disabling, wait for the completion of disabling by periodically checking
    // whether the state has been changed into DEVICE_EVENTS_DISABLED.

    lock();

    if (m_state != DEVICE_EVENTS_ENABLED)
    {
        unlock();
        return; // nothing to do
    }

    m_state = DISABLE_DEVICE_EVENTS;
    pinch();

    while (m_state != DEVICE_EVENTS_DISABLED)
        wait(WAIT_FOR_DEVICE_EVENTS_DISABLED);

    unlock();

    NTRACE(TRACE_MEDIASTARTUP, "disableDeviceEvents: time=%llu", Time::mono().val());
}

//////////
//
// Run
//
//////////
void EventThread::Run()
{
    static const Time EVENT_POLLING_INTERVAL(100);

    // lock the thread
    ScopedMutex lock(m_workerMutex);

    while (m_running)
    {
        PERFORMANCE_MARKER_SCOPED("eventthread.loop");

        {
            PERFORMANCE_MARKER_SCOPED("eventthread.loop.process");

            // Check whether there is any listener event in queue first. If so, call the
            // corresponding listener notification functions.
            if (handleEventQueue())
                continue; // continue to try fetching the next event.

            // According to the state of the device event queue (enabled or disabled),
            // process device events or do nothing.

            switch (m_state)
            {
                case DISABLE_DEVICE_EVENTS:
                    handleDisableDeviceEvents();
                    continue;

                case DEVICE_EVENTS_DISABLED:
                    break; // nothing to do, go to sleep.

                case ENABLE_DEVICE_EVENTS:
                    if (handleEnableDeviceEvents())
                        continue;
                    break;

                case DEVICE_EVENTS_ENABLED:
                    if (handleDeviceEvents())
                    {
                        // event handled, try for more
                        continue;
                    }
                    break; // no more events, go to sleep
            }
        }

        PERFORMANCE_MARKER_SCOPED("eventthread.loop.wait");
        wait(EVENT_POLLING_INTERVAL); // sleep and check later.
    }
}

//////////
//
// callListener
//
//////////
void EventThread::callListener(const ListenerEvent<Listener>& event)
{
    std::vector<shared_ptr<Listener> > listeners;
    uint32_t i = 0;

    {
        // copy the listener vector under lock
        ScopedMutex lock(m_listenerMutex);
        listeners = m_listeners;
    }

    // notify listeners outside of lock
    for (i = 0; i< listeners.size(); ++i)
        event.fireTo(listeners[i]);
}

//////////
//
// handleEventQueue
//
//////////
bool EventThread::handleEventQueue()
{
    shared_ptr<ListenerEvent<Listener> > listenerEvent;

    if (m_eventQueue->get(listenerEvent))
    {
        --m_eventCount;

        // call listener with thread unlocked
        unlock();
        callListener(*listenerEvent);
        lock();

        listenerEvent.reset();

        return true; // event handled
    }

    return false;
}

//////////
//
// handleDisableDeviceEvents
//
//////////
void EventThread::handleDisableDeviceEvents()
{
    // worker lock should be held by calller

    // If disabling of device events is requested, change the state to signal
    // that disabling of events has become effective.

    m_state = DEVICE_EVENTS_DISABLED;
    pinch(); // wake up any waiters on our state
}

//////////
//
// handleEnableDeviceEvents
//
//////////
bool EventThread::handleEnableDeviceEvents()
{
    // worker lock should be held by caller

    // If enabling of device events is requested, fetch/discard all events in
    // the device event queue first since they are considered outdated.
    if (m_player.deviceInitialized_)
    {
        ScopedMutex playbackLock(m_player.playbackDeviceMutex_);

        IPlaybackDevice::EventID eventID;
        ullong param1;
        ullong param2;
        std::string param3;

        while (m_player.playbackDevice_->getEvent(
                   eventID, param1, param2, param3) == NFErr_OK)
        {
            // NOOP, drop event on floor
        }
    }

    // Change the state to signal that events have been enabled
    m_state = DEVICE_EVENTS_ENABLED;
    pinch(); // wake up any waiters on our state

    return true;
}

//////////
//
// handleDeviceEvents
//
//////////
bool EventThread::handleDeviceEvents()
{
    IPlaybackDevice::EventID eventID;
    ullong param1;
    ullong param2;
    std::string param3;

    // get a device event from playback device
    if (!m_player.deviceInitialized_)
        return false;

    {
        ScopedMutex playbackLock(m_player.playbackDeviceMutex_);

        // If there isn't any event in queue, go to sleep.
        NFErr err = m_player.playbackDevice_->getEvent(
            eventID, param1, param2, param3);
        if (!err.ok())
            return false; // no events, sleep the thread
    }

    // handle the event we got
    switch (eventID)
    {
        case IPlaybackDevice::EVENT_PTS_UPDATE:
        {
            handlePTSUpdate(param1, param2);
            break;
        }

        case IPlaybackDevice::EVENT_DATA_EXHAUSTED:
        {
            NTRACE(TRACE_MEDIACONTROL, "EVENT_DATA_EXHAUSTED from device");
            handleDataExhausted();
            break;
        }

        case IPlaybackDevice::EVENT_END_OF_STREAM:
        {
            handleEndOfStream();
            break;
        }

        case IPlaybackDevice::EVENT_ERROR:
        {
            handleDeviceError(param1, param2, param3);
            break;
        }

        case IPlaybackDevice::EVENT_STREAM_INFO:
        {
            handleHevcInfo(param3);
            break;
        }
    }

    return true; // device event handled, loop back thread to try for more
}

//////////
//
// handlePTSUpdate
//
//////////
void EventThread::handlePTSUpdate(ullong param1, ullong param2)
{
    // Update PTS only when:
    //   - we're not in buffering or paused state.
    //   - we haven't reached end-of-presentation.

    if (m_player.state_ == IAdaptiveStreamingPlayer::PLAYING &&
        !m_player.dataExhausted_ && !m_player.endOfPresentation_)
    {
        // This will log perftrace if this is the first PTS we have received or
        // if the PTS is less that the last one we received. The latter case
        // should cause the log to occur when a new movie is started from the
        // beginning.
        if (m_lastPTS > param1)
        {
            PERFTRACE("MCQ2 - Event Thread handling first PTS update: time=%llu\n",
                      Time::mono().val());
            NTRACE(TRACE_MEDIACONTROL, "First PTSUpdate from device: time=%llu queue=%u",
                   Time::mono().ms(), m_eventCount);
            m_firstPTSUpdate = true;
        }
        m_lastPTS = param1;

        m_player.updatePTS(param1, param2);
    }
}

void EventThread::postUpdatePTSEvent(uint32_t pts, ullong monotimeInMS)
{
    if (m_firstPTSUpdate)
    {
        NTRACE(TRACE_MEDIACONTROL,
               "First PTSUpdate added to event queue: time=%llu queue=%u",
               Time::mono().ms(), m_eventCount);
    }
    postEvent(shared_ptr<ListenerEvent<Listener> >(
                  new UpdatePtsEvent<Listener>(m_sessionId, pts, monotimeInMS, m_firstPTSUpdate)));
    m_firstPTSUpdate = false;
}

//////////
//
// handleDataExhausted
//
//////////
void EventThread::handleDataExhausted()
{
    // If "dataExhausted_" is already set, this device event is redundant.
    // Otherwise, set the flag and call the listener functions to notify
    // of data underflow.
    if (!m_player.dataExhausted_)
    {
        NTRACE(TRACE_MEDIACONTROL, "EventThread::handleDataExhausted");
        postEvent(shared_ptr<ListenerEvent<Listener> >(
                      new UnderflowEvent<Listener>(m_sessionId)));
        m_player.dataExhausted_ = true;
    }
}

//////////
//
// handleEndOfStream
//
//////////
void EventThread::handleEndOfStream()
{
    NTRACE(TRACE_MEDIACONTROL, "receive EVENT_END_OF_STREAM");
    if (!m_player.endOfPresentation_)
    {
        m_player.endOfPresentation_ = true;
        postEvent(shared_ptr<ListenerEvent<Listener> > (
                      new EndOfStreamEvent<Listener>(m_sessionId)));
    }
}

//////////
//
// handleDeviceError
//
//////////
void EventThread::handleDeviceError(ullong param1, ullong param2,
                                    const std::string &param3)
{
    // An unrecoverable playback error is reported by the device.
    // As a result:
    //   1) an error message is logged.
    //   2) "error_" is set to let the pumping thread starts idling.
    //   3) the listener functions are called to notify of this error.

    Log::warn(TRACE_MEDIACONTROL,
              "Device Error Posted, param1: %llu, param2: %llu, param3: %s",
              param1, param2, param3.c_str());

    NFErr err = new PlaybackDeviceError(param1, param2, param3);

#ifdef NRDP_HAS_ESPLAYER
    if(param1 == esplayer::DECRYPTION_ERROR)
    {
        err.push(NFErr_DRMFailed);
        err.push(NFErr_MC_DeviceDecryptionError);
    }
    else
#endif
    {
        err.push(NFErr_MC_DevicePlaybackError);
    }

    m_player.reportError(err);
}

void EventThread::handleHevcInfo(const std::string &info)
{
    //NTRACE(TRACE_MEDIACONTROL, "EventThread::handleHevcInfo %s", info.c_str());
    postEvent(shared_ptr<ListenerEvent<Listener> >(
                  new HevcInfoEvent<Listener>(m_sessionId, info)));
}
