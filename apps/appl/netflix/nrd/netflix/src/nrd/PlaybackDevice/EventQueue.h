/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_EVENTQUEUE_H
#define DEVICE_EVENTQUEUE_H

#include <string>
#include <nrdbase/Mutex.h>

namespace netflix {

/**
 * @class EventQueue EventQueue.h
 * @brief Implementation of event queue for posting/retrieving playback events.
 */
class EventQueue
{
public:
    /**
     * Constructor.
     *
     * @param[in] maxEvents number of events the queue should be able to hold.
     */
    EventQueue(uint32_t maxEvents);

    virtual ~EventQueue();

    /**
     * Post an event into the queue. If the queue is full, false will be returned.
     *
     * @param[in] eventID the event identifier for the type of event to post.
     * @param[in] eventParam1 event parameter 1, actual meaning depends on the eventID.
     * @param[in] eventParam2 event parameter 2, actual meaning depends on the eventID.
     *
     * @return true if succeeded, otherwise false (queue is full).
     */
    bool postEvent(int eventID,
                   ullong eventParam1 = 0,
                   ullong eventParam2 = 0,
                   const std::string& eventParam3 = std::string());

    /**
     * Get event (if any) from the queue (first posted event gets retrieved first).
     * If the queue is empty, false will be returned.
     *
     * @param[out] eventID the event identifier for the type of posted event.
     * @param[out] eventParam1 event parameter 1, actual meaning depends on the eventID.
     * @param[out] eventParam2 event parameter 2, actual meaning depends on the eventID.
     *
     * @return true if succeeded, otherwise false (queue is empty).
     */
    bool getEvent(int& eventID,
                  ullong& eventParam1,
                  ullong& eventParam2,
                  std::string& event);
private:

    struct Event
    {
        int eventID_;
        ullong eventParam1_;
        ullong eventParam2_;
        std::string eventParam3_;
    };

    uint32_t volatile wp_;
    uint32_t volatile rp_;
    uint32_t const maxEvents_;
    Event* const events_;
    Mutex mutex_;
};

} // namespace netflix

#endif // DEVICE_EVENTQUEUE_H
