#ifndef ApplicationEvent_h
#define ApplicationEvent_h

/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Variant.h>

namespace netflix {

namespace ApplicationEvent {
enum Type {
    Event_Started,
    Event_Shutdown,
    Event_PlaybackEntered,
    Event_PlaybackExited,
    Event_FactoryReset,
    Event_Suspend,
    Event_SuspendComplete,
    Event_Resume,
    Event_SystemDataDatumChanged,
    Event_UrlAboutToChange,
    Event_UrlChanged,
    Event_ServerTimeChanged
};
}

class ApplicationEventListener
{
public:
    virtual ~ApplicationEventListener() {}
    virtual void event(ApplicationEvent::Type, const Variant &) {}
};

}

#endif
