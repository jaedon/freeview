/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef GibbonEventConnectionJSC_h
#define GibbonEventConnectionJSC_h

#include <nrd/EventConnection.h>
#include <nrdbase/Application.h>
#include <nrdbase/Variant.h>
#include <nrdbase/Time.h>
#include <nrdbase/tr1.h>
#include <list>
#include "JSC.h"

namespace netflix {
namespace gibbon {
namespace JSC {
class GibbonEventConnection : public EventConnection, public enable_shared_from_this<GibbonEventConnection>
{
public:
    GibbonEventConnection(const JSC::ProtectedValue &callback);
    void shutdown();
    void continueFromBreakpoint();
protected:
    virtual bool writeResponse(const Response &response);
private:
    class GibbonJSEvent : public Application::Event, public enable_shared_from_this<GibbonJSEvent>
    {
    public:
        inline GibbonJSEvent(const shared_ptr<GibbonEventConnection> &conn,
                             const Response &response)
            : Application::Event((response.flags() & Response::Flag_HighPriority) ? GibbonJSEvent::HighPriority : GibbonJSEvent::NormalPriority),
              mResponse(response), mConnection(conn)
        {
        }

        virtual std::string describe() const { return std::string("GibbonJSEvent: ") + mResponse.toVariant().toJSON(); }
        virtual void eventFired()
        {
            if (shared_ptr<GibbonEventConnection> conn = mConnection.lock())
                conn->sendEvent(shared_from_this());
        }
        virtual void eventCanceled()
        {
            mConnection.reset();
            mResponse.clear();
        }

        Response mResponse;
        weak_ptr<GibbonEventConnection> mConnection;
    };
    void sendEvent(const shared_ptr<GibbonJSEvent> &event);
    void sendEvent(const Response &response, int timerPreemption);

    JSC::ProtectedValue mCallback;
    std::list<shared_ptr<GibbonJSEvent> > mPending;
    static Mutex sMutex;

    friend class GibbonJSEvent;
};
}
}
}

#endif
