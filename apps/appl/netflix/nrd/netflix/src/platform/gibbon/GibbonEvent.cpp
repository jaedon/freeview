/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "GibbonEvent.h"
#include <nrdbase/PerformanceMarker.h>

using namespace netflix;
using namespace netflix::gibbon;

Time InputEvent::sLastInputEventTime(0);
Point MouseEvent::sLastPosition(0, 0);

void GibbonEvent::eventFired()
{
    gibbonApp()->handleEvent(this);
}

void InputEvent::Scope::sendEvent(const shared_ptr<InputEvent> &event)
{
    PERFORMANCE_MARKER_SCOPED("inputevent.sendevent");
    if(mEvent && event->type() == mEvent->type()) {
        if(mEvent->type() == InputEvent::Type_KeyPress) {
            shared_ptr<KeyEvent> newEvent = static_pointer_cast<KeyEvent>(event);
            shared_ptr<KeyEvent> oldEvent = static_pointer_cast<KeyEvent>(mEvent);
            if(oldEvent->mKey == newEvent->mKey && oldEvent->mText == newEvent->mText && oldEvent->mModifiers == newEvent->mModifiers) {
                oldEvent->mRepeat = true;
                ++oldEvent->mCount;
            }
            return;
        } else if(mEvent->type() == InputEvent::Type_MouseMove) {
            mEvent = event;
            return;
        }
    }
    flush();
    mEvent = event;
}

void InputEvent::Scope::flush()
{
    if(mEvent) {
        GibbonApplication::instance()->sendEvent(mEvent);
        mEvent.reset();
    }
}

