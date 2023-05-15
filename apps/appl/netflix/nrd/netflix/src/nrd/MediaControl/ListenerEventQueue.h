/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef MEDIACONTROL_LISTENER_EVENT_QUEUE_H
#define MEDIACONTROL_LISTENER_EVENT_QUEUE_H

#include "RWQueue.h"
#include "ListenerEvent.h"

namespace netflix {
namespace mediacontrol {

/**
 * @class ListenerEventQueue ListenerEventQueue.h
 * @brief A Single-Reader-Multi-Writer Queue for Listener Events.
 */
template <class T>
class ListenerEventQueue : public SRMWQueue< shared_ptr<ListenerEvent<T> > >
{
public:
    ListenerEventQueue(uint32_t maxItems)
      : SRMWQueue< shared_ptr<ListenerEvent<T> > >(maxItems) {}
};

}} // namespace netflix::mediacontrol

#endif // MEDIACONTROL_LISTENER_EVENT_QUEUE_H
