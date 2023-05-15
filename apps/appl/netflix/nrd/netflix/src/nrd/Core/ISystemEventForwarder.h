/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ISystemEventForwarder_h
#define ISystemEventForwarder_h

#include <nrd/ISystem.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/tr1.h>
#include <nrdbase/WeakPtrSet.h>
#include <vector>
#include <string>

namespace netflix {
namespace device {

class ISystemEventForwarder : public ISystem::EventDispatcher
{
public:
    ISystemEventForwarder()
        : mMutex(NRDPISYSTEMEVENTDISPATCHER_MUTEX, "NRDPISYSTEMEVENTDISPATCHER_MUTEX")
    {
    }

    void addDispatcher(const shared_ptr<ISystem::EventDispatcher> &dispatcher)
    {
        ScopedMutex lock(mMutex);
        mSystemDispatchers.insert(dispatcher);
    }

    void removeDispatcher(const shared_ptr<ISystem::EventDispatcher> &dispatcher)
    {
        ScopedMutex lock(mMutex);
        mSystemDispatchers.erase(dispatcher);
    }

    virtual void capabilityChanged()
    {
        execute(&ISystem::EventDispatcher::capabilityChanged);
    }
    virtual void videoOutputResolutionChanged()
    {
        execute(&ISystem::EventDispatcher::videoOutputResolutionChanged);
    }
    virtual void screensaverStateChanged()
    {
        execute(&ISystem::EventDispatcher::screensaverStateChanged);
    }
    virtual void networkChanged()
    {
        execute(&ISystem::EventDispatcher::networkChanged);
    }

    virtual void languageChanged()
    {
        execute(&ISystem::EventDispatcher::languageChanged);
    }
    virtual void commandReceived(const std::string &command)
    {
        std::vector<shared_ptr<ISystem::EventDispatcher> > locked;
        {
            ScopedMutex lock(mMutex);
            mSystemDispatchers.lock(locked);
        }

        for (std::vector<shared_ptr<ISystem::EventDispatcher> >::const_iterator it = locked.begin(); it != locked.end(); ++it)
            (*it)->commandReceived(command);
    }

    virtual void viewModeChanged()
    {

    }

    virtual void outputVolumeChanged()
    {
        execute(&ISystem::EventDispatcher::outputVolumeChanged);
    }

    virtual void videoOutputStatusChanged()
    {
        execute(&ISystem::EventDispatcher::videoOutputStatusChanged);
    }

protected:
    typedef void (ISystem::EventDispatcher::*Function)();
    void execute(Function func)
    {
        std::vector<shared_ptr<ISystem::EventDispatcher> > locked;
        {
            ScopedMutex lock(mMutex);
            mSystemDispatchers.lock(locked);
        }

        for (std::vector<shared_ptr<ISystem::EventDispatcher> >::const_iterator it = locked.begin(); it != locked.end(); ++it)
            (it->get()->*func)();
    }

private:
    WeakPtrSet<ISystem::EventDispatcher> mSystemDispatchers;
    Mutex mMutex;
};
}
}

#endif
