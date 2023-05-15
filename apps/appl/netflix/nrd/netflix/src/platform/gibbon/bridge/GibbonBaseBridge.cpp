/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "GibbonBaseBridge.h"

#include <GibbonBaseBridge.h>
#include <GibbonApplication.h>
#include <Animation.h>
#include <Screen.h>
#include <nrdbase/PerformanceMarker.h>

using namespace netflix;
using namespace netflix::gibbon;

void GibbonBaseBridge::setHaveProperty(int index)
{
    assert(index < int(sizeof(mSetProperties)*8) && index >= 0);
    mSetProperties |= (1LL << index);
}

bool GibbonBaseBridge::haveProperty(int index) const
{
    assert(index < int(sizeof(mSetProperties)*8) && index >= 0);
    return (mSetProperties & (1LL << index));
}

class GibbonPropertiesSendEvent;
static GibbonPropertiesSendEvent *sGibbonPropertiesSendEvent = 0;
static Mutex sGibbonPropertiesSendEventMutex(netflix::UNTRACKED_MUTEX, "GibbonPropertiesSendEventMutex");
class GibbonPropertiesSendEvent : public GibbonApplication::Event
{
public:
    inline GibbonPropertiesSendEvent() : GibbonApplication::Event(), mSent(false) { }
    inline ~GibbonPropertiesSendEvent()
    {
        if(!mSent) {
            ScopedMutex _lock(sGibbonPropertiesSendEventMutex);
            if(sGibbonPropertiesSendEvent == this)
                sGibbonPropertiesSendEvent = 0;
        }
    }
    inline void addBridge(const shared_ptr<GibbonBaseBridge> &bridge)
    {
        mBridges.insert(bridge);
    }
    std::string describe() const { return std::string("BridgePropertiesSendEvent"); }

    virtual void eventFired() {
        mSent = true;
        {
            ScopedMutex _lock(sGibbonPropertiesSendEventMutex);
            assert(sGibbonPropertiesSendEvent == this);
            sGibbonPropertiesSendEvent = 0;
        }
        ScopedMutex _lock(Widget::lock());
        for (WeakPtrSet<GibbonBaseBridge>::const_iterator it = mBridges.begin(); it != mBridges.end(); ++it) {
            if (shared_ptr<GibbonBaseBridge> bb = it->lock())
                bb->syncPendingProperties();
        }
    }
private:
    WeakPtrSet<GibbonBaseBridge> mBridges;
    bool mSent;
};

void GibbonBaseBridge::syncPendingProperties()
{
    unsigned int properties;
    {
        ScopedMutex _lock(sGibbonPropertiesSendEventMutex);
        properties = mPendingPropertiesSync;
        mPendingPropertiesSync = 0;
    }
    if(properties)
        syncPendingProperties(properties);
}

void GibbonBaseBridge::propertiesChanged(unsigned int properties, bool autoSync)
{
    ScopedMutex _lock(sGibbonPropertiesSendEventMutex);
    const unsigned int oldProperties = mPendingPropertiesSync;
    if((oldProperties ^ properties) & properties) {
        mPendingPropertiesSync |= properties;
        if(autoSync) {
            if(!sGibbonPropertiesSendEvent) {
                shared_ptr<GibbonPropertiesSendEvent> event(new GibbonPropertiesSendEvent);
#if defined(GIBBON_NO_THREADANIMATION)
                const bool posted = GibbonApplication::instance()->postEvent(event);
#else
                const bool posted = Animation::postEvent(event);
#endif
                if(posted)
                    sGibbonPropertiesSendEvent = event.get();
            }
            if(sGibbonPropertiesSendEvent)
                sGibbonPropertiesSendEvent->addBridge(static_pointer_cast<GibbonBaseBridge>(shared_from_this()));
        }
    }
}

void GibbonBaseBridge::objectId(std::string &key, Variant &id) const
{
    GibbonBaseBridge *that = const_cast<GibbonBaseBridge*>(this);
    id = ScriptEngine::objectId(static_pointer_cast<GibbonBaseBridge>(that->shared_from_this()));
    if(!id.isNull()) {
        key = "object";
        return;
    }
    NfObject::objectId(key, id);
}
