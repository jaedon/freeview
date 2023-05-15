/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "EventQueue.h"
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>


// RULES
//
// "WP" can only be modified by the event poster side.
// "RP" can only be modified by the event retriever side.
// "WP == RP" means queue is empty.
// "(WP+1) == RP" (with wrap-around handled) means queue is full.

using namespace netflix;

EventQueue::EventQueue(uint32_t maxEvents)
    : wp_(0), rp_(0),
      maxEvents_(maxEvents + 1),
      events_(new Event[maxEvents + 1]),
      mutex_(PLAYBACKDEVICEEVENT_MUTEX, "PlaybackDeviceEventMutex")
{
}

EventQueue::~EventQueue()
{
    delete [] events_;
}

bool EventQueue::postEvent(int eventID,
                           ullong eventParam1/* = 0*/,
                           ullong eventParam2/* = 0*/,
                           const std::string& eventParam3)
{
    ScopedMutex cs(mutex_);

    uint32_t next = wp_ + 1;
    if(next >= maxEvents_)
        next = 0;

    if(next == rp_)
        return false; // event queue is full

    events_[wp_].eventID_     = eventID;
    events_[wp_].eventParam1_ = eventParam1;
    events_[wp_].eventParam2_ = eventParam2;
    events_[wp_].eventParam3_ = eventParam3;

    wp_ = next;
    return true;
}

bool EventQueue::getEvent(int& eventID,
                          ullong& eventParam1,
                          ullong& eventParam2,
                          std::string& eventParam3)
{
    ScopedMutex cs(mutex_);

    if(rp_ == wp_)
        return false; // event queue is empty

    eventID     = events_[rp_].eventID_;
    eventParam1 = events_[rp_].eventParam1_;
    eventParam2 = events_[rp_].eventParam2_;
    eventParam3 = events_[rp_].eventParam3_;

    uint32_t next = rp_ + 1;
    if(next >= maxEvents_)
        next = 0;

    rp_ = next;
    return true;
}
