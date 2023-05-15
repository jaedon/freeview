/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for ny
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "GibbonResourceManager.h"
#include "GibbonConfiguration.h"
#include "GibbonApplication.h"
#include "GibbonFreetype.h"
#ifdef GIBBON_TILEMANAGER
# include "TileManagerBridge.h"
#endif
#include "SurfaceDecoder.h"
#include "GibbonBridge.h"
#include "WidgetBridge.h"
#include "SurfaceCache.h"
#include "InputManager.h"
#include "FontManager.h"
#include "GibbonEvent.h"
#include "TextLocale.h"
#include "Screen.h"
#include "Debug.h"
#ifdef BUILD_QA
#include "GibbonQABridge.h"
#endif
#ifdef HAVE_GIBBON_TCMALLOC
#include <gperftools/malloc_extension.h>
#endif

#include <gibbon/config.h>
#include <GibbonFreetype.h>

#include <nrd/Response.h>

#include <nrdbase/Instrumentation.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/VarLengthArray.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/Version.h>
#include <nrdnet/NetError.h>

#include <sstream>
#include <algorithm>

using namespace netflix;
using namespace netflix::gibbon;

#define GibbonBridgePropertyList(Property)                  \
    Property(cookie, NfObject::ReadOnly)                    \
    Property(cookieFilters, NfObject::ReadWrite)            \
    Property(debugFlags, NfObject::ReadOnly)                \
    Property(defaultDirection, NfObject::ReadWrite)         \
    Property(defaultLocale, NfObject::ReadWrite)            \
    Property(diskCacheConfiguration, NfObject::ReadOnly)    \
    Property(effectivePrefetchUrls, NfObject::ReadOnly)     \
    Property(email, NfObject::ReadOnly)                     \
    Property(fontStoreCapacity, NfObject::ReadOnly)         \
    Property(fonts, NfObject::ReadWrite)                    \
    Property(garbageCollectTimeout, NfObject::ReadWrite)    \
    Property(imageFormats, NfObject::ReadOnly)              \
    Property(location, NfObject::ReadWrite)                 \
    Property(networkErrorCodes, NfObject::ReadOnly)         \
    Property(networkErrorGroups, NfObject::ReadOnly)        \
    Property(password, NfObject::ReadOnly)                  \
    Property(prefetchKey, NfObject::ReadOnly)               \
    Property(prefetchUrls, NfObject::ReadWrite)             \
    Property(resourceManagerCapacity, NfObject::ReadOnly)   \
    Property(surfaceCacheCapacity, NfObject::ReadOnly)

PropertyTable(GibbonBridge, GibbonBridgePropertyList, properties)

#define GibbonBridgeMethodList(Method)          \
    Method(addInjectJS)                         \
    Method(addSample)                           \
    Method(addSocketizerConditions)             \
    Method(beginPendingSync)                    \
    Method(clearDiskCache)                      \
    Method(clearDiskCacheStats)                 \
    Method(cpuProfileStart)                     \
    Method(cpuProfileStop)                      \
    Method(destroyWidget)                       \
    Method(diskCacheInfo)                       \
    Method(diskCacheInsert)                     \
    Method(diskCachePurgeExpired)               \
    Method(diskCacheRemove)                     \
    Method(diskCacheSetWriteLimit)              \
    Method(dumpDiskCache)                       \
    Method(endPendingSync)                      \
    Method(eval)                                \
    Method(flushDiskCache)                      \
    Method(fontManagerInfo)                     \
    Method(garbageCollect)                      \
    Method(getHeapSize)                         \
    Method(getRenderedFps)                      \
    Method(hash)                                \
    Method(invoke)                              \
    Method(loadFont)                            \
    Method(localeInfo)                          \
    Method(moveMouse)                           \
    Method(reinitDiskCache)                     \
    Method(reloadFonts)                         \
    Method(removeFont)                          \
    Method(removeInjectJS)                      \
    Method(resourceManagerClear)                \
    Method(resourceManagerDisable)              \
    Method(resourceManagerInfo)                 \
    Method(resourceManagerRemove)               \
    Method(resourceManagerSetCapacity)          \
    Method(runConsole)                          \
    Method(setAtlasSize)                        \
    Method(setCookie)                           \
    Method(setGlyphAtlasSize)                   \
    Method(setStat)                             \
    Method(startFpsTimer)                       \
    Method(startRequest)                        \
    Method(startTask)                           \
    Method(startTimer)                          \
    Method(stopFpsTimer)                        \
    Method(stopRequest)                         \
    Method(stopTask)                            \
    Method(stopTimer)                           \
    Method(surfaceCacheInfo)                    \
    Method(surfaceCacheSetBrowseCapacity)       \
    Method(surfaceCacheSetPlaybackCapacity)     \
    Method(sync)                                \
    Method(uiLoaded)                            \
    Method(widgetsAt)

MethodTable(GibbonBridge, GibbonBridgeMethodList);

namespace netflix {
namespace gibbon {

static int sGibbonPushDuplicated = 0, sGibbonPushReplaced = 0, sGibbonPushSet = 0;

static uint8_t convertGarbageCollect(const Variant &value)
{
    uint8_t flags = 0;
    if(value.isNull()) {
        flags = GibbonApplication::Collect_All;
    } else if(value.isNumber()) {
        flags = (uint8_t)value.value<int>();
    } else {
        const std::string v = value.value<std::string>();
        if(v == "all")
            flags = GibbonApplication::Collect_All;
        else if(v == "js")
            flags = GibbonApplication::Collect_JS;
        else if(v == "graphics")
            flags = GibbonApplication::Collect_Surfaces;
        else if(v == "tcmalloc")
            flags = GibbonApplication::Collect_TcMalloc;
    }
    return flags;
}

class GibbonBridgeTask : public GibbonApplication::Event
{
public:
    inline GibbonBridgeTask(EventLoop::Event::Priority priority,
                            const shared_ptr<GibbonBridge> &bridge, int id) : GibbonApplication::Event(priority), mBridge(bridge), mID(id) { }
    inline ~GibbonBridgeTask() { }

    std::string describe() const { return "GibbonBridgeTask";  }

    virtual void eventFired() {
        PERFORMANCE_MARKER_SCOPED("bridgetask.fired");
        if(shared_ptr<GibbonBridge> gibbonBridge = mBridge.lock()) {
            int id = -1;
            {
                ScopedMutex _lock(gibbonBridge->mMutex);
                id = mID;
            }
            if(id != -1)
                gibbonBridge->handleTaskEvent(id);
        }
    }

    inline void invalidate() {
        if(shared_ptr<GibbonBridge> gibbonBridge = mBridge.lock())
            mID = -1;
    }

private:
    weak_ptr<GibbonBridge> mBridge;
    int mID;
};

class GibbonJSSyncEvent : public GibbonApplication::Event
{
public:
    inline GibbonJSSyncEvent() { }
    inline ~GibbonJSSyncEvent() {  }

    inline std::string describe() const { return "GibbonJSSyncEvent";  }

    inline void eventPosted(const char *desc) {
        if(Debug::DebugWidgetPropertiesPush || Debug::DebugScriptEvents)
            netflix::Log::error(TRACE_LOG, "%s[%p]", desc, this);
    }

    virtual void eventFired() {
        ScopedMutex _lock(Widget::lock());
        if(Debug::DebugWidgetPropertiesPush || Debug::DebugScriptEvents)
            netflix::Log::error(TRACE_LOG, "PROCESS[%p]", this);
        for(std::map<int, shared_ptr<WidgetChanges> >::const_iterator it = mChanges.begin(); it != mChanges.end(); ++it) {
            const shared_ptr<WidgetChanges> &widgetChanges = it->second;
            if((widgetChanges->flags & (WidgetChanges::Create|WidgetChanges::Destroy)) == (WidgetChanges::Create|WidgetChanges::Destroy))
                continue;
            shared_ptr<WidgetBridge> widget = GibbonApplication::instance()->getBridge()->createWidget(it->first);
            assert(widget);
            if((widgetChanges->flags & (WidgetChanges::SetProperties|WidgetChanges::Destroy)) == (WidgetChanges::SetProperties)) {
                widgetChanges->setPropertyParams.handleChanges(widget);
            }
            if(const shared_ptr<TextChanges> &textChanges = widgetChanges->mTextChanges) {
                if(textChanges->flags && (textChanges->flags & (TextChanges::Create|TextChanges::Destroy)) != (TextChanges::Create|TextChanges::Destroy)) {
                    shared_ptr<TextBridge> text;
                    if(textChanges->flags & TextChanges::Create)
                        text = widget->createText();
                    else
                        text = widget->getText();
                    if(((textChanges->flags & (TextChanges::SetProperties|TextChanges::Destroy)) == (TextChanges::SetProperties)))
                        textChanges->setPropertyParams.handleChanges(text);
                }
            }
            for(std::map<int, shared_ptr<ImageChanges> >::const_iterator cit = widgetChanges->mImageChanges.begin(); cit != widgetChanges->mImageChanges.end(); ++cit) {
                const shared_ptr<ImageChanges> &imageChanges = cit->second;
                assert(imageChanges->flags);
                if((imageChanges->flags & (ImageChanges::Create|ImageChanges::Destroy)) != (ImageChanges::Create|ImageChanges::Destroy)) {
                    shared_ptr<ImageBridge> image;
                    if(imageChanges->flags & ImageChanges::Create)
                        image = widget->createImage(cit->first, imageChanges->createParams.value<bool>(0, false));
                    else
                        image = widget->getImage(cit->first);
                    if((imageChanges->flags & (ImageChanges::SetProperties|ImageChanges::Destroy)) == (ImageChanges::SetProperties))
                        imageChanges->setPropertyParams.handleChanges(image);
                    if(image && (imageChanges->flags & ImageChanges::Destroy))
                        widget->removeImage(image);
                }
            }
            for(std::map<int, shared_ptr<EffectChanges> >::const_iterator cit = widgetChanges->mEffectChanges.begin(); cit != widgetChanges->mEffectChanges.end(); ++cit) {
                const shared_ptr<EffectChanges> &effectChanges = cit->second;
                assert(effectChanges->flags);
                if((effectChanges->flags & (EffectChanges::Create|EffectChanges::Destroy)) != (EffectChanges::Create|EffectChanges::Destroy)) {
                    shared_ptr<EffectBridge> effect;
                    if(effectChanges->flags & EffectChanges::Create)
                        effect = widget->createEffect(cit->first, effectChanges->createParams);
                    else
                        effect = widget->getEffect(cit->first);
                    if((effectChanges->flags & (EffectChanges::SetProperties|EffectChanges::Destroy)) == (EffectChanges::SetProperties))
                        effectChanges->setPropertyParams.handleChanges(effect);
                    if(effect && (effectChanges->flags & EffectChanges::Destroy))
                        widget->removeEffect(effect);
                }
            }
            if(widgetChanges->flags & WidgetChanges::Destroy)
                widget->destroy();
        }
        if(Debug::DebugWidgetPropertiesPush || Debug::DebugScriptEvents)
            netflix::Log::error(TRACE_LOG, "~PROCESS[%p]", this);
    }

    inline bool isEmpty() const { return mChanges.empty(); }

    inline void createWidget(int widget) {
        shared_ptr<WidgetChanges> &changes = getWidgetChange(widget);
        changes->flags |= WidgetChanges::Create;
    }
    inline void destroyWidget(int widget) {
        shared_ptr<WidgetChanges> &changes = getWidgetChange(widget);
        if(Debug::DebugWidgetPropertiesPush)
            changes->checkDestroy("WIDGET", widget, 0);
        changes->flags |= WidgetChanges::Destroy;
    }
    inline void addWidgetChange(int widget, int property, const Variant &value, bool append) {
        shared_ptr<WidgetChanges> &changes = getWidgetChange(widget);
        if(Debug::DebugWidgetPropertiesPush)
            changes->checkSetProperty("WIDGET", widget, 0, property, value, append);
        changes->flags |= WidgetChanges::SetProperties;
        changes->setPropertyParams.setValue(property, value, append);
    }

    inline void createImage(int widget, int id, const Variant &params) {
        shared_ptr<ImageChanges> &changes = getWidgetChange(widget)->getImageChanges(id);
        changes->flags |= ImageChanges::Create;
        changes->createParams = params;
    }
    inline void destroyImage(int widget, int id) {
        shared_ptr<ImageChanges> &changes = getWidgetChange(widget)->getImageChanges(id);
        if(Debug::DebugWidgetPropertiesPush)
            changes->checkDestroy("IMAGE", widget, id);
        changes->flags |= ImageChanges::Destroy;
    }
    inline void addImageChange(int widget, int id, int property, const Variant &value, bool append) {
        shared_ptr<ImageChanges> &changes = getWidgetChange(widget)->getImageChanges(id);
        if(Debug::DebugWidgetPropertiesPush)
            changes->checkSetProperty("IMAGE", widget, id, property, value, append);
        changes->flags |= ImageChanges::SetProperties;
        changes->setPropertyParams.setValue(property, value, append);
    }

    inline void createEffect(int widget, int id, const Variant &params) {
        shared_ptr<EffectChanges> &changes = getWidgetChange(widget)->getEffectChanges(id);
        changes->flags |= EffectChanges::Create;
        changes->createParams = params;
    }
    inline void destroyEffect(int widget, int id) {
        shared_ptr<EffectChanges> &changes = getWidgetChange(widget)->getEffectChanges(id);
        if(Debug::DebugWidgetPropertiesPush)
            changes->checkDestroy("EFFECT", widget, id);
        changes->flags |= EffectChanges::Destroy;
    }
    inline void addEffectChange(int widget, int id, int property, const Variant &value, bool append) {
        shared_ptr<EffectChanges> &changes = getWidgetChange(widget)->getEffectChanges(id);
        if(Debug::DebugWidgetPropertiesPush)
            changes->checkSetProperty("EFFECT", widget, id, property, value, append);
        changes->flags |= EffectChanges::SetProperties;
        changes->setPropertyParams.setValue(property, value, append);
    }

    inline void createText(int widget) {
        shared_ptr<TextChanges> &changes = getWidgetChange(widget)->getTextChanges();
        changes->flags |= TextChanges::Create;
    }
    inline void destroyText(int widget) {
        shared_ptr<TextChanges> &changes = getWidgetChange(widget)->getTextChanges();
        if(Debug::DebugWidgetPropertiesPush)
            changes->checkDestroy("TEXT", widget, 0);
        changes->flags |= TextChanges::Destroy;
    }
    inline void addTextChange(int widget, int property, const Variant &value, bool append) {
        shared_ptr<TextChanges> &changes = getWidgetChange(widget)->getTextChanges();
        if(Debug::DebugWidgetPropertiesPush)
            changes->checkSetProperty("TEXT", widget, 0, property, value, append);
        changes->flags |= TextChanges::SetProperties;
        changes->setPropertyParams.setValue(property, value, append);
    }

protected:
    template<uint8_t SIZE> struct ChangesBase {
        inline ChangesBase() : flags(0) { }
        enum {
            Create        = 0x01,
            SetProperties = 0x02,
            Destroy       = 0x04
        };
        uint8_t flags;
        Variant createParams;
        class PropertyParams {
        public:
            inline PropertyParams() : mContains(0), mAppend(0) {
                assert(SIZE <= (sizeof(mContains)*8));
                memset(mValues, 0, sizeof(mValues));
            }
            inline ~PropertyParams() {
                for(size_t i = 0; i < mSet.size(); ++i) {
                    const uint8_t property = mSet[i];
                    assert(mContains & (uint64_t(1) << property));
                    ((Variant*)(mValues+(property*sizeof(Variant))))->~Variant();
                }
            }
            inline int size() const { return mSet.size(); }

            inline void handleChanges(const shared_ptr<GibbonBaseBridge> &bridge) const {
                if(!bridge)
                    return;
                for(size_t i = 0; i < mSet.size(); ++i) {
                    const uint8_t property = mSet[i];
                    const uint64_t field = (uint64_t(1) << property);
                    assert(mContains & field);
                    const Variant *value = (const Variant*)(mValues+(property*sizeof(Variant)));
                    if(mAppend & field)
                        bridge->setProperties(property, *value);
                    else
                        bridge->setProperty(property, *value);
                }
            }
            inline const Variant *value(uint8_t i) const {
                assert(i < SIZE);
                if(mContains & (uint64_t(1) << i))
                    return (const Variant*)(mValues+(i*sizeof(Variant)));
                return 0;
            }
            inline void setValue(uint8_t property, const Variant &value, bool append) {
                assert(property < SIZE);
                const uint64_t field = (uint64_t(1) << property);
                if(!(mContains & field)) {
                    mSet.append(property);
                    mContains |= field;
                    new (mValues+(property*sizeof(Variant))) Variant(value);
                } else if(append) {
                    if(mAppend & field) {
                        ((Variant*)(mValues+(property*sizeof(Variant))))->push_back(value);
                    } else {
                        mAppend |= field;
                        Variant arrayValue;
                        arrayValue.push_back(*((Variant*)(mValues+(property*sizeof(Variant)))));
                        arrayValue.push_back(value);
                        *((Variant*)(mValues+(property*sizeof(Variant)))) = arrayValue;
                    }
                } else {
                    mAppend &= ~field;
                    *((Variant*)(mValues+(property*sizeof(Variant)))) = value;
                }
            }
        protected:
            uint64_t mContains, mAppend;
            VarLengthArray<uint8_t, SIZE> mSet;
            char mValues[SIZE*sizeof(Variant)];
        };
        PropertyParams setPropertyParams;

        inline void checkDestroy(const char *desc, int widget, int id) {
            if(flags & Create)
                netflix::Log::error(TRACE_LOG, "%s[%d:%d]: Still-born object [%d]", desc, widget, id, (int)setPropertyParams.size());
            else if(flags & SetProperties)
                netflix::Log::error(TRACE_LOG, "%s[%d:%d]: Pushed changes for dead object [%d]", desc, widget, id, (int)setPropertyParams.size());
        }
        inline void checkSetProperty(const char *desc, int widget, int id, int property, const Variant &value, bool append) {
            if(const Variant *oldValue = setPropertyParams.value(property)) {
                const char *reason;
                if(append) {
                    ++sGibbonPushSet;
                    reason = "Append";
                } else if(*oldValue == value) {
                    ++sGibbonPushDuplicated;
                    reason = "Duplicate";
#if 1
                } else {
                    ++sGibbonPushReplaced;
                    reason = "Replaced";
#endif
                }
                if(reason)
                    netflix::Log::error(TRACE_LOG, "%s[%d:%d]: %s push[%d] %s->%s [%d:%d:%d]", desc, widget, id,
                                        reason, property, oldValue->toJSON().c_str(), value.toJSON().c_str(),
                                        sGibbonPushSet, sGibbonPushDuplicated, sGibbonPushReplaced);
#if 1
            } else {
                ++sGibbonPushSet;
                netflix::Log::error(TRACE_LOG, "%s[%d:%d]: SET push[%d] %s [%d:%d:%d]", desc, widget, id,
                                    property, value.toJSON().c_str(), sGibbonPushSet, sGibbonPushDuplicated, sGibbonPushReplaced);
#endif
            }
        }
    };

    typedef ChangesBase<10> ImageChanges;
    typedef ChangesBase<3>  EffectChanges;
    typedef ChangesBase<33> TextChanges;

    struct WidgetChanges : public ChangesBase<38> {
        shared_ptr<TextChanges> mTextChanges;
        std::map<int, shared_ptr<ImageChanges> > mImageChanges;
        std::map<int, shared_ptr<EffectChanges> > mEffectChanges;

        inline shared_ptr<TextChanges> &getTextChanges() {
            if(!mTextChanges)
                mTextChanges.reset(new TextChanges);
            return mTextChanges;
        }
        inline shared_ptr<ImageChanges> &getImageChanges(int id) {
            shared_ptr<ImageChanges> &changes = mImageChanges[id];
            if(!changes)
                changes.reset(new ImageChanges);
            return changes;
        }
        inline shared_ptr<EffectChanges> &getEffectChanges(int id) {
            shared_ptr<EffectChanges> &changes = mEffectChanges[id];
            if(!changes)
                changes.reset(new EffectChanges);
            return changes;
        }
    };
    std::map<int, shared_ptr<WidgetChanges> > mChanges;

    inline shared_ptr<WidgetChanges> &getWidgetChange(int widget) {
        shared_ptr<WidgetChanges> &changes = mChanges[widget];
        if(!changes)
            changes.reset(new WidgetChanges);
        return changes;
    }
};
static int sPendingSync = 0;
static shared_ptr<GibbonJSSyncEvent> sSyncEvent;
static inline void createSyncEvent()
{
    assert(Application::isAppThread());
    if(!sSyncEvent) {
        assert(sPendingSync);
        sSyncEvent.reset(new GibbonJSSyncEvent);
    }
}

class GibbonBridgeTimer : public Application::Timer
{
public:
    friend class GibbonBridge;
    GibbonBridgeTimer(int interval, bool singleShot, shared_ptr<GibbonBridge> bridge, int id) :
        Application::Timer(interval), mBridge(bridge), mID(id), mActive(true) { setSingleShot(singleShot); }

    virtual std::string describe() const { return "JSTimer"; }
    virtual void timerFired() {
        PERFORMANCE_MARKER_SCOPED("bridgetimer.fired");
        if(shared_ptr<GibbonBridge> bridge = mBridge.lock())
            bridge->handleTimerEvent(mID);
    }

    inline bool isActive() const { return mActive; }
    inline void setActive(bool active) { mActive = active; }

protected:
    weak_ptr<GibbonBridge> mBridge;
    int mID;
    bool mActive;
};

class GibbonBridgeRequest : public GibbonResourceRequest
{
public:
    inline GibbonBridgeRequest(unsigned flags, const AseUrl &url, Resource::Method op,
                               const shared_ptr<GibbonBridge> &bridge, int id)
        : GibbonResourceRequest(flags, url, op), mID(id), mValid(true), mBridge(bridge)
    {
    }

    inline void invalidate() { mValid = false; }
    virtual void finish()
    {
        if (shared_ptr<GibbonBridge> gibbonBridge = mBridge.lock()) {
            {
                ScopedMutex _lock(gibbonBridge->mMutex);
                if(!mValid)
                    return;
                gibbonBridge->mRequests.erase(mID);
            }
            gibbonBridge->requestFinished(static_pointer_cast<GibbonBridgeRequest>(shared_from_this()));
        }
    }
    inline int id() const { return mID; }
private:
    int mID;
    bool mValid;
    weak_ptr<GibbonBridge> mBridge;
};
}}

GibbonBridge::GibbonBridge()
    : GibbonBaseBridge("gibbon", Properties::GibbonBridge::properties, Properties::GibbonBridge::PropertyCount,
                       Methods::GibbonBridge::methods, Methods::GibbonBridge::MethodCount),
      mMutex(GIBBON_BRIDGE_MUTEX, "GibbonBridgeMutex"), mEffectivePrefetchUrls(Variant(Variant::Type_StringMap))
{
}

GibbonBridge::~GibbonBridge()
{
    clearScene();
    mSceneBridge.reset();
    mDebuggerBridge->cleanup();
    mDebuggerBridge.reset();
}

void GibbonBridge::init()
{
    mSceneBridge.reset(new ScreenBridge);
    mSceneBridge->setParent(shared_from_this());
    mDebuggerBridge.reset(new DebuggerBridge);
    mDebuggerBridge->setParent(shared_from_this());
#ifdef GIBBON_TILEMANAGER
    shared_ptr<TileManagerBridge> tilemanager(new TileManagerBridge);
    tilemanager->setParent(shared_from_this());
#endif

#ifdef BUILD_QA
    mGibbonQABridge.reset(new GibbonQABridge);
    mGibbonQABridge->setParent(shared_from_this());
#endif
    std::ostringstream os;
    os << "Gibbon/" << NRDP_VERSION_STRING << "/" << NRDP_VERSION_STRING << ": "
       << "Netflix/" << NRDP_VERSION_STRING << " (DEVTYPE="
       << nrdApp()->esnPrefix()
       << "; CERTVER="
       << nrdApp()->getSystem()->getCertificationVersion() << ")";
    GibbonApplication::instance()->resourceManager()->setUserAgent(os.str());
}

void GibbonBridge::cleanup()
{
    mDebuggerBridge->cleanup();
}

void GibbonBridge::clearScene()
{
    shared_ptr<NfObject> widgetsBridge;
    {
        ScopedMutex _lock(mMutex);
        if(!mTasks.empty()) {
            std::map<int, shared_ptr<GibbonBridgeTask> >::const_iterator task = mTasks.begin();
            const std::map<int, shared_ptr<GibbonBridgeTask> >::const_iterator taskEnd = mTasks.end();
            while (task != taskEnd) {
                task->second->invalidate();
                ++task;
            }
            mTasks.clear();
        }
        if(!mTimers.empty()) {
            std::map<int, shared_ptr<GibbonBridgeTimer> >::const_iterator timer = mTimers.begin();
            const std::map<int, shared_ptr<GibbonBridgeTimer> >::const_iterator timerEnd = mTimers.end();
            while (timer != timerEnd) {
                GibbonApplication::instance()->stopTimer(timer->second);
                ++timer;
            }
            mTimers.clear();
        }
        if(!mRequests.empty()) {
            std::map<int, shared_ptr<GibbonBridgeRequest> >::const_iterator request = mRequests.begin();
            const std::map<int, shared_ptr<GibbonBridgeRequest> >::const_iterator requestEnd = mRequests.end();
            while (request != requestEnd) {
                request->second->invalidate();
                ++request;
            }
            mRequests.clear();
        }
        if(mWidgetsBridge) {
            mWidgetsBridge->setParent(shared_ptr<NfObject>());
            assert(mWidgetsBridge.use_count() == 1);
            std::swap(widgetsBridge, mWidgetsBridge);
            mWidgets.clear();
        }
    }
    {
        ScopedMutex _widgetLock(Widget::lock());
        mSceneBridge->clearScene();
        widgetsBridge.reset();
    }
}

bool GibbonBridge::getProperty(int index, Variant *result) const
{
    switch(index) {
    case Properties::GibbonBridge::diskCacheConfiguration:
        if(shared_ptr<GibbonDiskCache> diskCache = GibbonApplication::instance()->resourceManager()->diskCache()) {
            Variant &m = *result;
            m["capacity"] = diskCache->capacity();
            m["maxPending"] = diskCache->maxPendingSize();
            m["catalogTimer"] = diskCache->catalogTimer();
            m["writeLimit"] = diskCache->limit();
            m["writeSpeed"] = diskCache->writeSpeed(); // bytes/sec
        }
        break;
    case Properties::GibbonBridge::resourceManagerCapacity:
        if(ResourceManager::SharedPtr manager = GibbonApplication::instance()->resourceManager())
            (*result)["capacity"] = manager->capacity();
        break;
    case Properties::GibbonBridge::fontStoreCapacity:
        *result = GibbonConfiguration::fontStoreCapacity();
        break;
    case Properties::GibbonBridge::surfaceCacheCapacity:
        (*result)["browse"] = GibbonApplication::instance()->surfaceCacheCapacity();
        (*result)["playback"] = GibbonApplication::instance()->surfaceCachePlaybackCapacity();
        break;
    case Properties::GibbonBridge::cookie: {
        const AseUrl location = GibbonApplication::instance()->getLocation();
        shared_ptr<GibbonCookieManager> cookieMan = GibbonApplication::instance()->resourceManager()->cookieManager();
        *result = cookieMan->cookies(location,
                                     // We always trust JavaScript; either it's
                                     // loaded from SSL, or it's loaded using
                                     // X-Gibbon-Hash. Or, you're using a debug
                                     // build, and this is what you want anyway.
                                     GibbonCookieManager::Trusted);
        break;
    }
    case Properties::GibbonBridge::location:
        (*result)["url"] = GibbonApplication::instance()->getLocation().str();
        (*result)["count"] = GibbonApplication::instance()->getLocationCount();
        break;
    case Properties::GibbonBridge::email:
        *result = GibbonConfiguration::userName();
        break;
    case Properties::GibbonBridge::fonts: {
        Variant fonts;
        Variant scripts;

        const FontManager::FontRegistry& registry = FontManager::getFontRegistry();
        FontManager::FontRegistry::const_iterator entry = registry.begin();
        const FontManager::FontRegistry::const_iterator end = registry.end();
        while (entry != end) {
            Variant subdata;
            subdata["weight"] = entry->second.weight;
            subdata["style"] = entry->second.style;
            subdata["attributes"] = entry->second.attributes;
            subdata["fileName"] = entry->second.fileName;
            subdata["downloaded"] = entry->second.downloaded;
            subdata["priority"] = entry->second.priority;
            subdata["version"] = entry->second.version;

            Variant languages;
            if (entry->second.languages.size() != 0) {
                bool star = false;
                for (std::vector<FontManager::Language>::size_type i = 0; i < entry->second.languages.size(); i++) {
                    Variant language;
                    language["language"] = entry->second.languages[i].language;
                    language["script"] = entry->second.languages[i].script;
                    language["country"] = entry->second.languages[i].country;
                    language["string"] = entry->second.languages[i].toString();
                    languages.push_back(language);

                    if (std::find(scripts[entry->second.languages[i].script]["languages"].arrayBegin(), scripts[entry->second.languages[i].script]["languages"].arrayEnd(), entry->second.languages[i].language) == scripts[entry->second.languages[i].script]["languages"].arrayEnd()) {
                        if (entry->second.languages[i].language == "*") {
                            star = true;
                            scripts[entry->second.languages[i].script]["languages"].clear();
                            scripts[entry->second.languages[i].script]["languages"].push_back("*");
                        }
                        if (!star) {
                            scripts[entry->second.languages[i].script]["languages"].push_back(entry->second.languages[i].language);
                        }
                    }
                }
            }

            subdata["languages"] = languages;
            fonts[entry->second.family].push_back(subdata);
            ++entry;
        }
        fonts["scripts"] = scripts;
        *result = fonts;
        break; }
    case Properties::GibbonBridge::password:
        *result = GibbonConfiguration::userPassword();
        break;
    case Properties::GibbonBridge::imageFormats:
        *result = SurfaceDecoders::instance()->formats();
        break;
    case Properties::GibbonBridge::defaultLocale:
        *result = GibbonConfiguration::defaultLocale();
        break;
    case Properties::GibbonBridge::defaultDirection:
        *result = GibbonConfiguration::defaultDirection();
        break;
    case Properties::GibbonBridge::debugFlags:
        *result = Debug::values();
        break;
    case Properties::GibbonBridge::garbageCollectTimeout:
        *result = GibbonApplication::instance()->getScriptEngine()->garbageCollectTimeout();
        break;
    case Properties::GibbonBridge::networkErrorGroups: {
        Variant ret;
        for(int i=0; i<NetErrorGroup::Count; ++i)
            ret[NetError::errorGroupToString(static_cast<NetError::ErrorGroup>(i))] = i;
        *result = ret;
        break; }
    case Properties::GibbonBridge::networkErrorCodes: {
        Variant ret;
        for(int i=0; i<NetErrorCode::Count; ++i)
            ret[NetError::errorCodeToString(static_cast<NetError::ErrorCode>(i))] = i;
        *result = ret;
        break; }
    case Properties::GibbonBridge::effectivePrefetchUrls:
        *result = mEffectivePrefetchUrls;
        break;
    case Properties::GibbonBridge::prefetchUrls:
        break;
    case Properties::GibbonBridge::prefetchKey:
        *result = mPrefetchKey;
        break;
    case Properties::GibbonBridge::cookieFilters:
        if (shared_ptr<GibbonResourceManager> manager = gibbonApp()->resourceManager()) {
            *result = manager->cookieManager()->filters();
        }
        break;
    default:
        return false;
    }
    return true;
}

bool GibbonBridge::setProperty(int index, const Variant &value)
{
    switch(index) {
    case Properties::GibbonBridge::location: {
        GibbonApplication::instance()->setLocation(value);
        break; }
    case Properties::GibbonBridge::defaultLocale:
        GibbonConfiguration::setDefaultLocale(value.string());
        propertyUpdated(Properties::GibbonBridge::defaultLocale);
        break;
    case Properties::GibbonBridge::defaultDirection:
        GibbonConfiguration::setDefaultDirection(value.string());
        propertyUpdated(Properties::GibbonBridge::defaultDirection);
        break;
    case Properties::GibbonBridge::garbageCollectTimeout:
        GibbonApplication::instance()->getScriptEngine()->setGarbageCollectTimeout(value.value<int>());
        propertyUpdated(Properties::GibbonBridge::garbageCollectTimeout);
        break;
    case Properties::GibbonBridge::prefetchUrls:
        if (value.isStringMap()) {
            gibbonApp()->writeSystemConfiguration(SystemKeys::PrefetchUrls, value.toJSON());
        } else if (value.isNull()) {
            gibbonApp()->writeSystemConfiguration(SystemKeys::PrefetchUrls, std::string());
        } else {
            return false;
        }
        break;
    case Properties::GibbonBridge::cookieFilters:
        if (shared_ptr<GibbonResourceManager> manager = gibbonApp()->resourceManager()) {
            const std::vector<Variant> f = value.value<std::vector<Variant> >();
            std::vector<std::string> filters;
            filters.reserve(f.size());
            for (std::vector<Variant>::const_iterator it = f.begin(); it != f.end(); ++it) {
                filters.push_back(it->value<std::string>());
            }

            manager->cookieManager()->setFilters(filters);
        }
        break;
#ifdef BUILD_QA
        // Only allow setting the fonts property when building QA
    case Properties::GibbonBridge::fonts: {
        FontManager::clear();
        if(!value.isStringMap())
            return true;
        const std::map<std::string, Variant> fonts = value.stringMap();
        Variant::StringMapIterator font = fonts.begin();
        const Variant::StringMapIterator end = fonts.end();
        while (font != end) {
            if(!font->second.isArray()) {
                ++font;
                continue;
            }
            for(int sub = 0; sub < font->second.size(); ++sub) {
                Variant entry = font->second[sub];
                if(!entry.isStringMap())
                    continue;
                std::vector<FontManager::UnicodeBlock> blocks;
                std::vector<FontManager::Language> languages;
                FontValues values;
                std::string version;
                FontManager::FontRegistryItem item(entry["fileName"].string(),
                                                   font->first, // family
                                                   static_cast<Font::Weight>(entry["weight"].integer()),
                                                   static_cast<Font::Style>(entry["style"].integer()),
                                                   entry["attributes"].integer(), entry["priority"].integer(),
                                                   blocks, languages, values, version, -1,
                                                   Rect::fromString(entry["bbox"].string()));
                FontManager::registerFont(item);
            }
            ++font;
        }
        break; }
#endif
    default:
        return false;
    }
    return true;
}

shared_ptr<WidgetBridge> GibbonBridge::createWidget(int id)
{
    ScopedMutex _lock(Widget::lock());
    shared_ptr<WidgetBridge> bridge = getWidget(id);
    if(!bridge) {
        Widget::SharedPtr widget(new Widget(id));
        widget->init();
        if(Debug::DebugWidgetProperties)
            Log::error(TRACE_LOG, "[%s:%d]: %s (CREATE)", __func__, __LINE__, widget->describe().c_str());

        std::ostringstream stream;
        stream << "widget" << id;
        bridge.reset(new WidgetBridge(stream.str(), widget));
        bridge->init();
        {
            ScopedMutex _lock(mMutex);
            if(!mWidgetsBridge) {
                mWidgetsBridge.reset(new NfObject("widgets"));
                mWidgetsBridge->setParent(shared_from_this());
            }
            bridge->setParent(mWidgetsBridge);
            mWidgets[id] = bridge;
        }
    }
    return bridge;
}

void GibbonBridge::cookiesChanged()
{
    propertyUpdated(Properties::GibbonBridge::cookie);
}

void GibbonBridge::fontsChanged()
{
    propertyUpdated(Properties::GibbonBridge::fonts);
}

void GibbonBridge::locationChanged()
{
    propertyUpdated(Properties::GibbonBridge::location);
    mDebuggerBridge->onLocationChanged();
}

struct DiskCacheCallbackUserData {
    Variant response;
    weak_ptr<GibbonBridge> bridge;
};

static void diskCacheInsertCallback(void *userData, bool success)
{
    assert(userData);
    DiskCacheCallbackUserData *u = reinterpret_cast<DiskCacheCallbackUserData*>(userData);
    shared_ptr<GibbonBridge> bridge = u->bridge.lock();
    if (bridge) {
        u->response["success"] = success;
        bridge->sendEvent("diskCacheInsert", u->response);
    }
    delete u;
}

void GibbonBridge::cpuProfileStart(const Variant& data)
{
    if (shared_ptr<ScriptEngine> scriptEngine = GibbonApplication::instance()->getScriptEngine()) {
        scriptEngine->startCPUProfiler(data.mapValue<int>("profileId"));
    }
}

void GibbonBridge::cpuProfileStop(const Variant& data)
{
    bool ok;
    const int id = data.mapValue<int>("id", &ok);
    if(!ok) {
        Log::error(TRACE_LOG, "No 'id' in cpuProfileStop");
        return;
    }

    Variant out;

    const int profileId = data.mapValue<int>("profileId");
    out["profileId"] = profileId;

    if (shared_ptr<ScriptEngine> scriptEngine = GibbonApplication::instance()->getScriptEngine()) {
        const std::string profile = scriptEngine->stopCPUProfiler(profileId);
        out["profile"] = Variant::fromJSON(profile);
    }

    Variant ret;
    ret["data"] = out;
    ret["id"] = id;
    sendEvent("cpuProfile", ret);
}

Variant GibbonBridge::invoke(int method, const Variant &data)
{
    bool ok;
    switch (method) {
    case Methods::GibbonBridge::setStat: {
        const std::string name = data.mapValue<std::string>("name", &ok);
        if(!ok) {
            invalidArgumentError(method, "name");
            return false;
        }
        GibbonApplication::instance()->getScriptEngine()->setScriptStat(name, data.value("value"));
        break; }
    case Methods::GibbonBridge::removeInjectJS: {
        const std::string url = data.mapValue<std::string>("url", &ok);
        if(!ok) {
            invalidArgumentError(method, "url");
            return false;
        }
        GibbonConfiguration::removeInjectJS(url);
        if (mPersistentInjectJS.isArray()) {
            int pos = 0;
            Variant::ArrayIterator it = mPersistentInjectJS.arrayBegin();
            const Variant::ArrayIterator end = mPersistentInjectJS.arrayEnd();
            while (it != end) {
                if (it->isStringMap() && it->mapValue<std::string>("url") == url) {
                    mPersistentInjectJS.remove(pos);
                    gibbonApp()->writeSystemConfiguration(SystemKeys::InjectJS, mPersistentInjectJS.toJSON());
                    break;
                }
                ++pos;
                ++it;
            }
        }
        break; }
    case Methods::GibbonBridge::addInjectJS: {
        Variant js = data.mapValue("js", &ok);
        if (!ok) {
            invalidArgumentError(method, "js");
            return false;
        }
        if (js.isString()) {
            // make an object out of the string
            const std::string url = js.string();
            js.clear();
            js["url"] = url;
        }
        Variant options = data.mapValue("options");
        bool persist = false;
        bool add = false;
        if (options.isStringMap()) {
            persist = options.mapValue<bool>("persist");
            add = options.mapValue<bool>("add");
        }

        if (add && !GibbonConfiguration::addInjectJS(js)) {
            Log::error(TRACE_LOG, "Unable to add inject JS: '%s'", js.toJSON().c_str());
            return false;
        }
        if (persist) {
            mPersistentInjectJS.push_back(js);
            gibbonApp()->writeSystemConfiguration(SystemKeys::InjectJS, mPersistentInjectJS.toJSON());
        }
        break; }
    case Methods::GibbonBridge::addSocketizerConditions: {
        const Variant cond = data["conditions"];
        if (!cond.isStringMap() && !cond.isArray()) {
            invalidArgumentError(method, "conditions");
            return false;
        }
        GibbonApplication::instance()->addSocketizerConditions(cond);
        break; }
    case Methods::GibbonBridge::fontManagerInfo: {
        enum Mode { Normal, Cache, Lru };

        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }
        const Mode mode = static_cast<Mode>(data.mapValue<unsigned int>("mode", &ok));
        if(!ok) {
            invalidArgumentError(method, "mode");
            return false;
        }

        Variant data;
        switch (mode) {
        case Normal:
            data["info"] = FontManager::fontInfo();
            break;
        case Cache:
            data["info"] = GibbonFreetype::cacheInformation();
            break;
        case Lru:
            data["info"] = GibbonFreetype::lruInformation();
            break;
        }
        data["id"] = id;
        sendEvent("fontManagerInfo", data);
        break; }
    case Methods::GibbonBridge::surfaceCacheInfo: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        Variant data;
        data["info"] = GibbonApplication::instance()->getSurfaceCache()->surfaceInfo();
        data["id"] = id;
        sendEvent("surfaceCacheInfo", data);
        break; }
    case Methods::GibbonBridge::surfaceCacheSetBrowseCapacity: {
        const int capacity = data.mapValue<int>("capacity", 0, -1);
        if (capacity < 0) {
            invalidArgumentError(method, "capacity");
            return false;
        }
        GibbonApplication::instance()->setSurfaceCacheCapacity(capacity);
        propertyUpdated(Properties::GibbonBridge::surfaceCacheCapacity);
        break; }
    case Methods::GibbonBridge::surfaceCacheSetPlaybackCapacity: {
        const int capacity = data.mapValue<int>("capacity", 0, -1);
        if (capacity < 0) {
            invalidArgumentError(method, "capacity");
            return false;
        }
        GibbonApplication::instance()->setSurfaceCachePlaybackCapacity(capacity);
        propertyUpdated(Properties::GibbonBridge::surfaceCacheCapacity);
        break; }
    case Methods::GibbonBridge::resourceManagerDisable: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        GibbonApplication::instance()->resourceManager()->setCapacity(0);

        Variant data;
        data["id"] = id;
        sendEvent("resourceManagerDisable", data);
        break; }
    case Methods::GibbonBridge::resourceManagerSetCapacity: {
        const int capacity = data.mapValue<int>("capacity", 0, -1);
        if (capacity < 0) {
            invalidArgumentError(method, "capacity");
            return false;
        }
        GibbonApplication::instance()->resourceManager()->setCapacity(capacity);
        propertyUpdated(Properties::GibbonBridge::resourceManagerCapacity);
        break; }
    case Methods::GibbonBridge::resourceManagerClear: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        GibbonApplication::instance()->resourceManager()->clear();

        Variant data;
        data["id"] = id;
        sendEvent("resourceManagerClear", data);
        break; }
    case Methods::GibbonBridge::resourceManagerInfo: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        Variant data;
        data["info"] = GibbonApplication::instance()->resourceManager()->resourceInfo();
        data["id"] = id;
        sendEvent("resourceManagerInfo", data);
        break; }
    case Methods::GibbonBridge::clearDiskCache: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        if(DiskCache::SharedPtr diskCache = GibbonApplication::instance()->resourceManager()->diskCache()) {
            diskCache->clear();
        }

        Variant data;
        data["id"] = id;
        sendEvent("diskCacheCleared", data); }
        break;
    case Methods::GibbonBridge::clearDiskCacheStats: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        if(DiskCache::SharedPtr diskCache = GibbonApplication::instance()->resourceManager()->diskCache()) {
            diskCache->clearStats();
        }

        Variant data;
        data["id"] = id;
        sendEvent("diskCacheStatsCleared", data); }
        break;
    case Methods::GibbonBridge::reinitDiskCache: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }
        const std::string spec = data.mapValue<std::string>("spec", &ok);
        if(!ok) {
            invalidArgumentError(method, "spec");
            return false;
        }
        nrdApp()->initDiskCache(spec);

        propertyUpdated(Properties::GibbonBridge::diskCacheConfiguration);

        Variant data;
        data["id"] = id;
        sendEvent("diskCacheReinited", data);
        break; }
    case Methods::GibbonBridge::flushDiskCache: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        if(DiskCache::SharedPtr diskCache = GibbonApplication::instance()->resourceManager()->diskCache()) {
            diskCache->flush();
        }

        Variant data;
        data["id"] = id;
        sendEvent("diskCacheFlushed", data); }
        break;
    case Methods::GibbonBridge::dumpDiskCache: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        Variant data;
        data["id"] = id;
        if(DiskCache::SharedPtr diskCache = GibbonApplication::instance()->resourceManager()->diskCache()) {
            data["dump"] = diskCache->toVariant();
            data["stats"] = diskCache->stats();
        }

        sendEvent("diskCacheDumped", data); }
        break;
    case Methods::GibbonBridge::diskCacheInfo: {
        const int id = data.mapValue<int>("id", &ok);
        if (!ok) {
            invalidArgumentError(method, "id");
            return false;
        }
        Variant ret;
        Variant value;
        if (shared_ptr<DiskCache> diskCache = gibbonApp()->resourceManager()->diskCache()) {
            DumpInfo info;
            if (data.mapValue<bool>("data"))
                info.flags |= DumpInfo::Data;
            diskCache->dump(info, &value);
        }
        ret["id"] = id;
        ret["data"] = value;
        sendEvent("diskCacheInfo", ret);
        break; }
    case Methods::GibbonBridge::diskCacheRemove: {
        const DataBuffer cacheKey = data.mapValue<DataBuffer>("cacheKey");
        if (cacheKey.isEmpty()) {
            invalidArgumentError(method, "cacheKey");
            return false;
        }

        Variant response;
        response["id"] = data.mapValue<int>("id", 0, -1);
        shared_ptr<DiskCache> diskCache = gibbonApp()->resourceManager()->diskCache();

        if (!diskCache) {
            response["success"] = false;
        } else {
            response["success"] = diskCache->remove(cacheKey);
        }
        sendEvent("diskCacheRemove", response);
        break; }

    case Methods::GibbonBridge::diskCacheSetWriteLimit: {
        const int limit = data.mapValue<int>("limit", 0, -1);
        if (limit < 0) {
            invalidArgumentError(method, "limit");
            return false;
        }
        if (shared_ptr<GibbonDiskCache> diskCache = gibbonApp()->resourceManager()->diskCache())
            diskCache->setLimit(limit);
        propertyUpdated(Properties::GibbonBridge::diskCacheConfiguration);
        break; }

    case Methods::GibbonBridge::resourceManagerRemove: {
        const DataBuffer cacheKey = data.mapValue<DataBuffer>("cacheKey");
        if (cacheKey.isEmpty()) {
            invalidArgumentError(method, "cacheKey");
            return false;
        }

        Variant response;
        response["id"] = data.mapValue<int>("id", 0, -1);

        response["success"] = nrdApp()->resourceManager()->remove(cacheKey);
        sendEvent("resourceManagerRemove", response);
        break; }
    case Methods::GibbonBridge::diskCachePurgeExpired: {
        Variant response;
        response["id"] = data.mapValue<int>("id", 0, -1);
        shared_ptr<DiskCache> diskCache = gibbonApp()->resourceManager()->diskCache();

        if (!diskCache) {
            response["count"] = 0;
        } else {
            response["count"] = diskCache->purgeExpired();
        }
        sendEvent("diskCachePurgeExpired", response);
        break; }
    case Methods::GibbonBridge::diskCacheInsert: {
        const int id = data.mapValue<int>("id", 0, -1);
        shared_ptr<DiskCache> diskCache = gibbonApp()->resourceManager()->diskCache();

        if (!diskCache) {
            Variant response;
            response["id"] = id;
            response["success"] = false;
            sendEvent("diskCacheInsert", response);
            return true;
        }
        const DataBuffer cacheKey = data.mapValue<DataBuffer>("cacheKey");
        if (cacheKey.isEmpty()) {
            invalidArgumentError(method, "cacheKey");
            return false;
        }

        const llong maxAge = data.mapValue<llong>("maxAge");
        if (maxAge <= 0) {
            invalidArgumentError(method, "maxAge");
            return false;
        }
        const AseUrl url = data.mapValue<std::string>("url");
        const DataBuffer responseData = data.mapValue<DataBuffer>("data");
        int priority = 0;
        if (data.contains("priority")) {
            bool ok;
            priority = data.mapValue<int>("priority", &ok);
            if (!ok || priority < 0) {
                invalidArgumentError(method, "priority");
                return false;
            }
        }
        HttpResponseHeaders responseHeaders;
        const std::map<std::string, Variant> headers = data.mapValue<std::map<std::string, Variant> >("headers");
        for (std::map<std::string, Variant>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            responseHeaders.push_back(HttpResponseHeaders::Header(it->first, it->second.value<std::string>()));
        }
        int statusCode = 0;
        if (data.contains("statusCode")) {
            bool ok;
            statusCode = data.mapValue<int>("statusCode", &ok);
            if (!ok || statusCode < 0) {
                invalidArgumentError(method, "statusCode");
                return false;
            }
        }

        DiskCacheCallbackUserData *u = new DiskCacheCallbackUserData;
        u->bridge = static_pointer_cast<GibbonBridge>(shared_from_this());
        u->response["id"] = id;
        if (!diskCache->addResource(cacheKey, statusCode, url, responseHeaders, responseData, priority,
                                    Time(), maxAge, u, diskCacheInsertCallback)) {
            u->response["success"] = false;
            sendEvent("diskCacheInsert", u->response);
            delete u;
        }
        break; }
    case Methods::GibbonBridge::startTask: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }
        EventLoop::Event::Priority priority = EventLoop::Event::LowPriority;
        if(data.contains("priority")) {
            const std::string priority_str = data.mapValue<std::string>("priority");
            if(priority_str == "medium")
                priority = EventLoop::Event::NormalPriority;
            else if(priority_str == "high")
                priority = EventLoop::Event::HighPriority;
            else if(priority_str == "low")
                priority = EventLoop::Event::LowPriority;
        }
        shared_ptr<GibbonBridgeTask> task(new GibbonBridgeTask(priority, static_pointer_cast<GibbonBridge>(shared_from_this()), id));
        {
            ScopedMutex lock(mMutex);
            mTasks[id] = task;
            GibbonApplication::instance()->postEvent(task);
        }
        break; }
    case Methods::GibbonBridge::stopTask: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        ScopedMutex lock(mMutex);
        std::map<int, shared_ptr<GibbonBridgeTask> >::iterator task = mTasks.find(id);
        if(task != mTasks.end()) {
            task->second->invalidate();
            mTasks.erase(task);
        }
        break; }
    case Methods::GibbonBridge::startTimer: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }
        const int interval = data.mapValue<int>("interval", &ok);
        if(!ok) {
            invalidArgumentError(method, "interval");
            return false;
        }
        const bool singleShot = data.mapValue<bool>("singleShot", 0, false);
        shared_ptr<GibbonBridgeTimer> timer(new GibbonBridgeTimer(interval, singleShot, static_pointer_cast<GibbonBridge>(shared_from_this()), id));
        {
            ScopedMutex lock(mMutex);
            mTimers[id] = timer;
            GibbonApplication::instance()->startTimer(timer);
        }
        break; }
    case Methods::GibbonBridge::stopTimer: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        ScopedMutex lock(mMutex);
        std::map<int, shared_ptr<GibbonBridgeTimer> >::iterator timer = mTimers.find(id);
        if(timer != mTimers.end()) {
            GibbonApplication::instance()->stopTimer(timer->second);
            mTimers.erase(timer);
        }
        break; }
    case Methods::GibbonBridge::eval: {
        const std::string file = data.mapValue<std::string>("file", &ok);
        if(!data.contains("script")) {
            invalidArgumentError(method, "script");
            return false;
        }
        const Variant &script = data["script"];
        Variant ret;
        if(script.isDataBuffer())
            GibbonApplication::instance()->getScriptEngine()->evaluate(script.dataBuffer(), file, 0, &ret);
        else if(script.isString())
            GibbonApplication::instance()->getScriptEngine()->evaluate(DataBuffer::fromRawData(script.c_str(), script.size()), file, 0, &ret);
        return ret; }
    case Methods::GibbonBridge::widgetsAt: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        Variant result;
        result["id"] = id;

        Point pos;
        pos.x = data.mapValue<Coordinate>("x", 0, CoordinateMax);
        pos.y = data.mapValue<Coordinate>("y", 0, CoordinateMax);
        {
            ScopedMutex _lock(Widget::lock());
            const std::list<Widget::SharedPtr> widgets = GibbonApplication::instance()->getScreen()->widgetsAt(pos);
            for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(); it != widgets.end(); ++it) {
                Variant widget;
                const Rect widgetRect = (*it)->getGlobalRect();
                widget["x"] = pos.x - widgetRect.x;
                widget["y"] = pos.y - widgetRect.y;
                widget["id"] = (*it)->getID();
                result["widgets"].push_back(widget);
            }
        }
        sendEvent("widgetsAt", result);
        break; }
    case Methods::GibbonBridge::uiLoaded:
        Log::milestone("gibbon.uiLoaded");
        INSTRUMENTATION_EVENT_ON("gibbon.ui.loaded");
        Log::error(TRACE_UI_SCRIPT, "UI 'says' it is loaded [%lld].", NrdApplication::timeSinceLocationChanged().ms());
        break;
    case Methods::GibbonBridge::startRequest:
        return startRequest(data);
    case Methods::GibbonBridge::hash: {
        bool ok = false;
        const std::string url = data.mapValue<std::string>("url", &ok);
        if (!ok) {
            invalidArgumentError("startRequest", "url");
            return false;
        }
        const int id = data.mapValue<int>("id", &ok);
        if (!ok) {
            invalidArgumentError("startRequest", "id");
            return false;
        }
        const Resource::Method op = GibbonResourceManager::methodFromString(data.mapValue<std::string>("requestMethod"));

        shared_ptr<GibbonBridgeRequest> request(new GibbonBridgeRequest(GibbonResourceManager::HashRequest, url, op,
                                                                        static_pointer_cast<GibbonBridge>(shared_from_this()),
                                                                        id));
        GibbonResourceManager::setupRequest(request, data);
        startRequest(request);
        return true; }
    case Methods::GibbonBridge::stopRequest: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }
        stopRequest(id);
        break; }
    case Methods::GibbonBridge::garbageCollect: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }
        GibbonApplication::instance()->garbageCollect(convertGarbageCollect(data["type"]));

        Variant data;
        data["id"] = id;
        sendEvent("garbageCollected", data);
        break; }
    case Methods::GibbonBridge::runConsole: {
        const std::string command = data.mapValue<std::string>("command", &ok);
        if(!ok) {
            invalidArgumentError(method, "command");
            return false;
        }
        if(shared_ptr<Console> console = app()->console())
            console->handleCommand(command, Console::Console_NoHistory);
        break; }
    case Methods::GibbonBridge::setAtlasSize: {
        const int group = data.mapValue<int>("group", &ok);
        if(!ok) {
            invalidArgumentError(method, "group");
            return false;
        }
        const Coordinate width = data.mapValue<int>("width", &ok);
        if(!ok) {
            invalidArgumentError(method, "width");
            return false;
        }
        const Coordinate height = data.mapValue<int>("height", &ok);
        if(!ok) {
            invalidArgumentError(method, "height");
            return false;
        }
        GibbonApplication::instance()->getSurfaceCache()->setAtlasSize(group, width, height);
        break; }
    case Methods::GibbonBridge::setGlyphAtlasSize: {
        const int width = data.mapValue<int>("width", &ok);
        if(!ok) {
            invalidArgumentError(method, "width");
            return false;
        }
        const int height = data.mapValue<int>("height", &ok);
        if(!ok) {
            invalidArgumentError(method, "height");
            return false;
        }
        GibbonFreetype::setCacheSizes(-1, width, height);
        break;
    }
    case Methods::GibbonBridge::setCookie: {
        GibbonApplication *app = GibbonApplication::instance();
        AseUrl location = app->getLocation();
        app->resourceManager()->cookieManager()->processCookie(location, data.mapValue<std::string>("cookie"));
        propertyUpdated(Properties::GibbonBridge::cookie);
        const int id = data.mapValue<int>("id", &ok);
        if(ok) {
            Variant data;
            data["id"] = id;
            sendEvent("setCookie", data);
        }
        break; }
    case Methods::GibbonBridge::startFpsTimer:
        GibbonApplication::instance()->startFpsTimer();
        break;
    case Methods::GibbonBridge::stopFpsTimer:
        GibbonApplication::instance()->stopFpsTimer();
        break;
    case Methods::GibbonBridge::getRenderedFps: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        Variant data;
        data["id"] = id;
        data["fps"] = GibbonApplication::instance()->getFps();
        sendEvent("getRenderedFps", data);
        break; }
    case Methods::GibbonBridge::getHeapSize: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }
        Variant data;
        data["id"] = id;
        const ScriptEngine::Stats stats = GibbonApplication::instance()->getScriptEngine()->stats();
        data["heapsize"] = stats.heapSize;
        data["heapcapacity"] = stats.heapCapacity;
        data["globalobjectcount"] = stats.globalObjectCount;
        data["protectedobjectcount"] = stats.protectedObjectCount;
        data["protectedglobalobjectcount"] = stats.protectedGlobalObjectCount;
        data["jsStringCount"] = stats.jsStringCount;
        data["jsStringsSize"] = stats.jsStringsSize;
        data["objectcount"] = stats.objectCount;
        data["fastmalloc"] = stats.fastMallocSystem;
        data["fastmalloc_used"] = stats.fastMallocSystem - stats.fastMallocFreebytes;
        data["osallocatorcommitted"] = stats.osAllocatorCommitted;
        data["osallocatordecommitted"] = stats.osAllocatorDecommitted;
#ifdef HAVE_GIBBON_TCMALLOC
        MallocExtension* ext = MallocExtension::instance();
        assert(ext);
        size_t allocated = 0, heap = 0, unmapped = 0;
        ext->GetNumericProperty("generic.current_allocated_bytes", &allocated);
        ext->GetNumericProperty("generic.heap_size", &heap);
        ext->GetNumericProperty("tcmalloc.pageheap_unmapped_bytes", &unmapped);
        data["systemused"] = allocated;
        data["systemallocated"] = heap - unmapped;
#endif
        sendEvent("heapsize", data);
        break; }
    case Methods::GibbonBridge::moveMouse: {
        Point pos;
        pos.x = data.mapValue<Coordinate>("x", 0, CoordinateMax);
        pos.y = data.mapValue<Coordinate>("y", 0, CoordinateMax);
        if(shared_ptr<InputManager> input = GibbonApplication::instance()->getInputManager())
            input->moveMouse(pos);
        break; }
    case Methods::GibbonBridge::cpuProfileStart: {
        app()->callInMainThread<GibbonBridge, Variant>(this, &GibbonBridge::cpuProfileStart, data);
        break; }
    case Methods::GibbonBridge::cpuProfileStop: {
        app()->callInMainThread<GibbonBridge, Variant>(this, &GibbonBridge::cpuProfileStop, data);
        break; }
    case Methods::GibbonBridge::reloadFonts:
#ifdef BUILD_QA
        FontManager::clear();
        FontManager::addFontsFromDirectory(GibbonConfiguration::textFontPath());
        fontsChanged();
#endif
        break;
    case Methods::GibbonBridge::localeInfo: {
        const std::string locale = data.mapValue<std::string>("locale", &ok);
        if(!ok) {
            invalidArgumentError(method, "locale");
            return false;
        }

        Variant result;
        result["direction"] = Locale(locale).getDirection() ? "rtl" : "ltr";
        return result; }
    case Methods::GibbonBridge::loadFont: {
        bool hasId = false;
        const int id = data.mapValue<int>("id", &hasId);

        const Variant font = data.value("font", &ok);
        if(!ok) {
            invalidArgumentError(method, "font");
            return false;
        }
        const int priority = data.mapValue<int>("priority", &ok);
        if(!ok) {
            invalidArgumentError(method, "priority");
            return false;
        }

        DataBuffer ttf;
        std::string filename;
        if (data.contains("data")) {
            filename = data.mapValue<std::string>("key", &ok);
            if(!ok) {
                invalidArgumentError(method, "key");
                return false;
            }
            ttf = data.mapValue<DataBuffer>("data", &ok);
        } else {
            filename = data.mapValue<std::string>("fileName", &ok);
        }

        const Font::Weight weight = static_cast<Font::Weight>(font.mapValue<unsigned int>("weight"));
        const Font::Style style = static_cast<Font::Style>(font.mapValue<unsigned int>("style"));
        const std::string family = font["family"].string();
        const std::string patch = font["patch"].string();
        const std::string version = font["version"].string();
        const int cmap = font.mapValue<int>("cmap", 0, -1);
        const Rect bbox = Rect::fromString(font["bbox"].string());
        const bool persist = font["persist"].boolean();
        const int attributes = font.mapValue<int>("attributes");

        std::vector<FontManager::UnicodeBlock> blocks;
        if (font.contains("blocks")) {
            FontManager::UnicodeBlock block;

            const Variant b = font["blocks"];
            Variant::ArrayIterator it = b.arrayBegin();
            const Variant::ArrayIterator end = b.arrayEnd();
            while (it != end) {
                block.start = it->mapValue<unsigned int>("start", &ok);
                if(!ok) {
                    invalidArgumentError(method, "start");
                    return false;
                }
                block.end = it->mapValue<unsigned int>("end", &ok);
                if(!ok) {
                    invalidArgumentError(method, "end");
                    return false;
                }
                NTRACE(TRACE_GIBBON_FONTS, "adding font for range U+%06X", (int)block.start);
                blocks.push_back(block);
                ++it;
            }
        }

        std::vector<FontManager::Language> languages;
        if (font.contains("languages")) {
            const Variant l = font["languages"];
            Variant::ArrayIterator it = l.arrayBegin();
            const Variant::ArrayIterator end = l.arrayEnd();
            while (it != end) {
                languages.push_back(FontManager::Language(it->string()));
                ++it;
            }
        }

        FontValues values;
        if (font.contains("values")) {
            const Variant v = font["values"];
            values.emboldenFactor = v.mapValue<double>("embolden");
            values.slantFactor = v.mapValue<double>("slant");
        }

        FontManager::FontRegistryItem item(filename, family, weight, style, attributes, priority, blocks,
                                           languages, values, version, cmap, bbox, ttf, patch, font, persist);
        const bool success = FontManager::registerFont(item);
        if (success)
            fontsChanged();

        if (hasId) {
            Variant data;
            Variant ret;
            ret["success"] = success;
            if (!success)
                ret["error"] = "Unable to register font";
            data["id"] = id;
            data["data"] = ret;
            sendEvent("fontLoaded", data);
        }
        break; }
    case Methods::GibbonBridge::removeFont: {
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        const std::string family = data.mapValue<std::string>("family", &ok);
        if(!ok) {
            invalidArgumentError(method, "family");
            return false;
        }
        const Font::Weight weight = static_cast<Font::Weight>(data.mapValue<unsigned int>("weight"));
        const Font::Style style = static_cast<Font::Style>(data.mapValue<unsigned int>("style"));

        const bool success = FontManager::unregisterFont(family, weight, style);

        fontsChanged();

        Variant data;
        Variant ret;
        ret["success"] = success;
        data["id"] = id;
        data["data"] = ret;
        sendEvent("fontRemoved", data);

        break; }
    case Methods::GibbonBridge::invoke: {
        NTRACE(TRACE_GIBBON_SCRIPT, "[%s:%d]: Handling: %s (%s)", __func__, __LINE__, methodName(method), data.toJSON(true).c_str());
        const std::string object = data.mapValue<std::string>("subObject", &ok);
        if(!ok) {
            invalidArgumentError(method, "subObject");
            return false;
        }

        if(shared_ptr<NfObject> obj = nrdApp()->findObject(object)) {
            const int func = obj->methodIndex(data.mapValue<std::string>("subMethod"));
            if(func == -1) {
                invalidArgumentError(method, "subMethod");
                return false;
            }
            obj->invoke(func, data["subArgs"]);
        } else {
            Log::error(TRACE_NBP, "Invalid object %s", object.c_str());
            return false;
        }
        break; }
    case Methods::GibbonBridge::beginPendingSync:
        beginPendingSync();
        break;
    case Methods::GibbonBridge::endPendingSync:
        endPendingSync();
        break;
    case Methods::GibbonBridge::sync:
        sync();
        break;
    }
    return Variant();
}

void GibbonBridge::handleTaskEvent(int taskEvent)
{
    {
        ScopedMutex lock(mMutex);
        std::map<int, shared_ptr<GibbonBridgeTask> >::iterator task = mTasks.find(taskEvent);
        if(task == mTasks.end())
            return;
        mTasks.erase(task);
    }
    sendEvent("task", taskEvent, Response::Flag_Synchronous);
}

void GibbonBridge::handleTimerEvent(int timerEvent)
{
    {
        ScopedMutex lock(mMutex);
        std::map<int, shared_ptr<GibbonBridgeTimer> >::iterator timer = mTimers.find(timerEvent);
        if(timer == mTimers.end())
            return;
        if(timer->second->isSingleShot())
            mTimers.erase(timer);
    }
    sendEvent("timer", timerEvent, Response::Flag_Synchronous);
}

void GibbonBridge::handleEvent(GibbonEvent *event)
{
    PERFORMANCE_MARKER_SCOPED("bridge.handleevent");

    Variant value;

    const GibbonEvent::Type type = static_cast<GibbonEvent::Type>(event->type());
    unsigned int modifiers = 0;
    switch (type) {
    case GibbonEvent::Type_KeyPress:
    case GibbonEvent::Type_KeyRelease:
    case GibbonEvent::Type_KeyClick: {
        switch (type) {
        case GibbonEvent::Type_KeyPress: value["type"] = "press"; break;
        case GibbonEvent::Type_KeyRelease: value["type"] = "release"; break;
        case GibbonEvent::Type_KeyClick: value["type"] = "click"; break;
        default: break;
        }

        const KeyEvent *keyEvent = static_cast<const KeyEvent*>(event);
        value["count"] = keyEvent->mCount;
        value["repeat"] = keyEvent->mRepeat;
        value["code"] = keyEvent->mKey;

        modifiers = keyEvent->mModifiers;

        if(!keyEvent->mText.empty())
            value["text"] = keyEvent->mText;
        if(!keyEvent->mUIKey.empty())
            value["uiEvent"] = keyEvent->mUIKey;
        break; }

    case GibbonEvent::Type_MouseChange: {
        const MouseChangeEvent *mouseEvent = static_cast<const MouseChangeEvent*>(event);
        value["type"] = "mouseChange";
        value["visible"] = mouseEvent->mVisible;
        break; }
    case GibbonEvent::Type_MouseMove:
    case GibbonEvent::Type_MousePress:
    case GibbonEvent::Type_MouseRelease: {
        switch (type) {
        case GibbonEvent::Type_MousePress: value["type"] = "mousePress"; break;
        case GibbonEvent::Type_MouseRelease: value["type"] = "mouseRelease"; break;
        case GibbonEvent::Type_MouseMove: value["type"] = "mouseMove"; break;
        default: break;
        }
        const MouseEvent *mouseEvent = static_cast<const MouseEvent*>(event);

        modifiers = mouseEvent->mModifiers;
        Point globalPos(mouseEvent->mPos.x, mouseEvent->mPos.y);
        if(GibbonConfiguration::screenWidth() != GibbonConfiguration::uiWidth() ||
            GibbonConfiguration::screenHeight() != GibbonConfiguration::uiHeight()) {
            globalPos.x *= float(GibbonConfiguration::uiWidth()) / float(GibbonConfiguration::screenWidth());
            globalPos.y *= float(GibbonConfiguration::uiHeight()) / float(GibbonConfiguration::screenHeight());
        }
        value["global"]["x"] = globalPos.x;
        value["global"]["y"] = globalPos.y;
        {
            ScopedMutex _lock(Widget::lock());
            const std::list<Widget::SharedPtr> widgets = GibbonApplication::instance()->getScreen()->widgetsAt(globalPos);
            for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(); it != widgets.end(); ++it) {
                Variant widget;
                const Rect widgetRect = (*it)->getGlobalRect();
                widget["x"] = mouseEvent->mPos.x - widgetRect.x;
                widget["y"] = mouseEvent->mPos.y - widgetRect.y;
                widget["id"] = (*it)->getID();
                value["widgets"].push_back(widget);
            }
        }
        value["button"] = mouseEvent->mButton;
        value["buttons"] = mouseEvent->mButtons;
        break; }

    case GibbonEvent::Type_TouchMove:
    case GibbonEvent::Type_TouchPress:
    case GibbonEvent::Type_TouchRelease: {
        switch (type) {
        case GibbonEvent::Type_TouchPress: value["type"] = "touchPress"; break;
        case GibbonEvent::Type_TouchRelease: value["type"] = "touchRelease"; break;
        case GibbonEvent::Type_TouchMove: value["type"] = "touchMove"; break;
        default: break;
        }

        const TouchEvent *touchEvent = static_cast<const TouchEvent*>(event);
        for(size_t i = 0; i < touchEvent->mPoints.size(); i++) {
            Variant point;
            point["id"] = touchEvent->mPoints[i].mID;
            point["x"] = touchEvent->mPoints[i].mPos.x;
            point["y"] = touchEvent->mPoints[i].mPos.y;
            point["pressure"] = touchEvent->mPoints[i].mPressure;
            value["global"].push_back(point);

            const std::list<Widget::SharedPtr> widgets = GibbonApplication::instance()->getScreen()->widgetsAt(touchEvent->mPoints[i].mPos);
            for(std::list<Widget::SharedPtr>::const_iterator it = widgets.begin(); it != widgets.end(); ++it) {
                Variant widget;
                const Rect widgetRect = (*it)->getGlobalRect();
                widget["id"] = (*it)->getID();
                widget["x"] = touchEvent->mPoints[i].mPos.x - widgetRect.x;
                widget["y"] = touchEvent->mPoints[i].mPos.y - widgetRect.y;
                value["widgets"].push_back(widget);
            }
        }
        std::map<int, int>::const_iterator it;
        break; }

    case GibbonEvent::Type_FocusChange: {
        const FocusChangeEvent *focusEvent = static_cast<const FocusChangeEvent*>(event);
        value["type"] = "focusChange";
        value["focus"] = focusEvent->mFocus;
        break; }

    case GibbonEvent::Type_Unknown:
        assert(false);
        break;
    }

    if(modifiers) {
        if(modifiers & KeyEvent::MODIFIER_CTRL)
            value["modifiers"].push_back("control");
        if(modifiers & KeyEvent::MODIFIER_ALT)
            value["modifiers"].push_back("alt");
        if(modifiers & KeyEvent::MODIFIER_SHIFT)
            value["modifiers"].push_back("shift");
        if(modifiers & KeyEvent::MODIFIER_META)
            value["modifiers"].push_back("meta");
        if(modifiers & KeyEvent::MODIFIER_KEYPAD)
            value["modifiers"].push_back("keypad");
    }

    if(!value.isNull()) {
        //Log::error(TRACE_LOG, "[%s] %s:%d: EVENT %s", __func__, __FILE__, __LINE__, value.toJSON().c_str());
        sendEvent("gibbonEvent", value, Response::Flag_Synchronous, event->mTime);
        //Log::error(TRACE_LOG, "[%s] %s:%d: ~EVENT", __func__, __FILE__, __LINE__);
    }
}

void GibbonBridge::requestFinished(const shared_ptr<GibbonBridgeRequest> &request)
{
    const Resource::State state = request->state();
    assert(state != Resource::State_None);
    const Resource::SharedPtr resource = request->resource();
    assert(resource);
    const uint32_t flags = request->flags();

    shared_ptr<ScriptEngine> scriptEngine = GibbonApplication::instance()->getScriptEngine();
    if(!scriptEngine)
        return;

    Variant value;
    value["id"] = request->id();
    if((flags & GibbonResourceManager::HashRequest) == GibbonResourceManager::HashRequest) {
        const DataBuffer hash = resource->responseData().hash(DataBuffer::Hash_SHA256).toHex();
        value["hash"] = "sha256=" + hash.toString();
        sendEvent("hash", value);
        return;
    }

    if (flags & GibbonResourceManager::Script) {
        assert(Application::isAppThread());
        Variant e;
        assert(scriptEngine);
        scriptEngine->evaluate(resource->responseData(), resource->url(), &e);
        value["exception"] = e;
    } else {
        const DataBuffer data = resource->responseData();
        const int statusCode = resource->httpStatusCode();
        bool handled = false;
        if (statusCode >= 200 && statusCode < 300) {
            if (flags & GibbonResourceManager::Format_String) {
                value["data"] = ScriptEngine::createString(data);
                handled = true;
            } else if (flags & GibbonResourceManager::Format_UInt8Array) {
                value["data"] = ScriptEngine::createTypedArray(ScriptEngine::Type_Uint8, data);
                handled = true;
            } else if (flags & GibbonResourceManager::Format_XML) {
                assert(Application::isAppThread());
                assert(scriptEngine);
                int used = -1;
                const Variant parsed = scriptEngine->parseXML(data.data<const char>(), data.size(), &handled, &used);
                if (handled) {
                    const char *ch = data.data<const char>();
                    while (used < data.size()) {
                        if (!isspace(static_cast<unsigned char>(ch[used++]) < data.size())) {
                            handled = false;
                            break;
                        }
                    }
                    if (handled)
                        value["xml"] = parsed;
                    handled = true;
                }
                value["parsed"] = handled;
            } else if (flags & (GibbonResourceManager::Format_JSON|GibbonResourceManager::Format_JSONStream)) {
                assert(Application::isAppThread());
                const char *str = data.data<const char>();
                int pos = 0;
                int length = data.size();
                assert(scriptEngine);
                Variant json;
                while (length) {
                    bool ok;
                    int used;
                    const Variant parsed = scriptEngine->parseJSON(str + pos, length, &ok, &used);
                    if(!ok) {
                        value["jsonError"] = true;
                        json.clear();
                        break;
                    }

                    pos += used;
                    if(flags & GibbonResourceManager::Format_JSONStream) {
                        json.push_back(parsed);
                        length -= used;
                    } else {
                        json = parsed;
                        break;
                    }
                }
                if (handled) {
                    while (pos < data.size()) {
                        if (!isspace(static_cast<unsigned char>(str[pos++]))) {
                            json.clear();
                            break;
                        }
                    }
                }

                if (!json.isNull()) {
                    value["json"] = json;
                    handled = true;
                }
            } else if (!(flags & GibbonResourceManager::Format_String)) {
                value["data"] = data;
                handled = true;
            }
        }

        if (!handled)
            value["data"] = scriptEngine->createString(data);
        value["size"] = data.size();
    }

    value["statusCode"] = resource->httpStatusCode();
    const NetError::ErrorCode errorCode = resource->errorCode();
    const NetError::ErrorGroup ng = NetError::mapErrorGroup(errorCode);
    value["errorcode"] = errorCode;
    value["nativeErrorCode"] = resource->nativeErrorCode();
    value["reason"] = errorCode;
    value["errorgroup"] = ng;
    value["state"] = Resource::stateToString(state);
    if (state == Resource::State_DiskCacheRefreshPending)
        value["refresh"] = true;

    if (state == Resource::State_Network || state == Resource::State_Network_Validated) {
        if(state == Resource::State_Network_Validated)
            value["validated"] = true;

        const NetworkMetrics metrics = resource->networkMetrics();
        value["dnsTime"] = metrics.dnsTime();
        value["connectTime"] = metrics.connectTime();
        value["transactionTime"] = metrics.transactionTime();
        value["duration"] = metrics.duration();
        if (metrics.serverIpAddress().valid())
            value["serverIp"] = metrics.serverIpAddress().getStrAddr();
        const std::string cname = metrics.cname();
        if (!cname.empty())
            value["cname"] = cname;
        const std::string sslVersion = metrics.sslVersion();
        if (!sslVersion.empty())
            value["sslVersion"] = sslVersion;
    }
    value["url"] = resource->baseUrl().str();
    value["finalURL"] = resource->url().str();
    value["headers"] = resource->responseHeaders().toVariant();
#if 0
    printf("%s: %s\n", request->url().str().c_str(), data.c_str());
#endif
    Response::Flag flag = Response::Flag_None;
    if (flags & ResourceRequest::Synchronous) {
        flag = Response::Flag_Synchronous;
    }
    sendEvent("requestFinished", value, flag);
}

bool GibbonBridge::startRequest(const Variant &data)
{
    bool ok = false;
    const std::string url = data.mapValue<std::string>("url", &ok);
    if(!ok) {
        invalidArgumentError("startRequest", "url");
        return false;
    }
    const int id = data.mapValue<int>("id", &ok);
    if(!ok) {
        invalidArgumentError("startRequest", "id");
        return false;
    }
    unsigned flags = 0;
    if(!data.mapValue<bool>("async", 0, true))
        flags |= ResourceRequest::Synchronous;
    if(data.mapValue<bool>("eval"))
        flags |= GibbonResourceManager::Script;
    if(data.mapValue<bool>("secure"))
        flags |= GibbonResourceManager::Trusted;
    const std::string format = data.mapValue<std::string>("format");
    if(format.empty()) {
        flags |= GibbonResourceManager::Format_String;
    } else if(format == "xml") {
        flags |= GibbonResourceManager::Format_XML;
    } else if(format == "json") {
        flags |= GibbonResourceManager::Format_JSON;
    } else if(format == "jsonstream") {
        flags |= GibbonResourceManager::Format_JSONStream;
    } else if(format == "arraybuffer") {
        flags |= GibbonResourceManager::Format_ArrayBuffer;
    } else if(format == "uint8array") {
        flags |= GibbonResourceManager::Format_UInt8Array;
    } else {
        flags |= GibbonResourceManager::Format_String;
    }

    const Resource::Method op = GibbonResourceManager::methodFromString(data.mapValue<std::string>("requestMethod"));
    shared_ptr<GibbonBridgeRequest> request(new GibbonBridgeRequest(flags, url, op,
                                                                   static_pointer_cast<GibbonBridge>(shared_from_this()),
                                                                   id));
    GibbonResourceManager::setupRequest(request, data);
    return startRequest(request);
}

bool GibbonBridge::startRequest(const shared_ptr<GibbonBridgeRequest> &request)
{
    {
        ScopedMutex _lock(mMutex);
        mRequests[request->id()] = request;
    }
    nrdApp()->resourceManager()->sendRequest(request);
    if (request->flags() & ResourceRequest::Synchronous)
        request->complete();
    return true;
}

void GibbonBridge::stopRequest(int id)
{
    shared_ptr<GibbonBridgeRequest> request;
    {
        ScopedMutex _lock(mMutex);
        std::map<int, shared_ptr<GibbonBridgeRequest> >::iterator it = mRequests.find(id);
        if(it != mRequests.end()) {
            request = it->second;
            mRequests.erase(it);
        }
    }
    if(request)
        request->abort();
}

void GibbonBridge::dumpTasks() const
{
    for(std::map<int, shared_ptr<GibbonBridgeTask> >::const_iterator it = mTasks.begin(); it != mTasks.end(); ++it) {
        if(it->second)
            Log::warn(TRACE_LOG, "Task: %d", it->second->priority());
    }
}

void GibbonBridge::stopAllTimers()
{
    ScopedMutex lock(mMutex);
    for(std::map<int, shared_ptr<GibbonBridgeTimer> >::const_iterator it = mTimers.begin(); it != mTimers.end(); ++it) {
        if(it->second) {
            GibbonApplication::instance()->stopTimer(it->second);
            it->second->setActive(false);
        }
    }
}

void GibbonBridge::startAllTimers()
{
    ScopedMutex lock(mMutex);
    for(std::map<int, shared_ptr<GibbonBridgeTimer> >::const_iterator it = mTimers.begin(); it != mTimers.end(); ++it) {
        if(it->second && !it->second->isActive()) {
            GibbonApplication::instance()->startTimer(it->second);
            it->second->setActive(true);
        }
    }
}

void GibbonBridge::removeWidget(int id)
{
    ScopedMutex _lock(mMutex);
    mWidgets.erase(id);
}

shared_ptr<WidgetBridge> GibbonBridge::findWidget(const DumpInfo &dump) const
{
    ScopedMutex _lock(mMutex);
    if(!dump.empty()) {
        for(std::map<int, weak_ptr<WidgetBridge> >::const_iterator it = mWidgets.begin(); it != mWidgets.end(); ++it) {
            if(shared_ptr<WidgetBridge> bridge = it->second.lock()) {
                Widget::SharedPtr w = bridge->getWidget();
                if(w->getID() == dump.idx || w.get() == dump.ptr || bridge.get() == dump.ptr ||
                   (dump.match.length() && w->getName() == dump.match))
                    return bridge;
            }
        }
    }
    return shared_ptr<WidgetBridge>();
}

shared_ptr<WidgetBridge> GibbonBridge::getRoot()
{
    if(Widget::SharedPtr w = mSceneBridge->getRoot())
        return getWidget(w->getID());
    return shared_ptr<WidgetBridge>();
}

shared_ptr<WidgetBridge> GibbonBridge::getOverlay()
{
    if(Widget::SharedPtr w = mSceneBridge->getOverlay())
        return getWidget(w->getID());
    return shared_ptr<WidgetBridge>();
}

shared_ptr<WidgetBridge> GibbonBridge::getWidget(int id)
{
    ScopedMutex _lock(mMutex);
    if(id == -1) {
        if(shared_ptr<WidgetBridge> w = getRoot())
            return w;
        return getOverlay();
    }
    std::map<int, weak_ptr<WidgetBridge> >::const_iterator widget = mWidgets.find(id);
    if(widget != mWidgets.end())
        return widget->second.lock();
    return shared_ptr<WidgetBridge>();
}

unsigned GibbonBridge::requestFlags(const Request &request) const
{
    unsigned ret = GibbonBaseBridge::requestFlags(request);
    if(request.type() == Request::Method) {
        switch (request.method()) {
        case Methods::GibbonBridge::invoke:
        case Methods::GibbonBridge::setGlyphAtlasSize:
            ret |= Request::Queued;
        default:
            break;
        }
    }
    return ret;
}

void GibbonBridge::suspendChanged(bool suspend, const std::string &reason)
{
    Variant data;
    data["suspended"] = suspend;
    data["reason"] = reason;
    mSceneBridge->sendEvent("suspendChanged", data);
}

void GibbonBridge::reportHighWatermark(ullong size)
{
    Variant data;
    data["size"] = size;
    sendEvent("capacityReached", data);
}

void GibbonBridge::reportCodepointMissing(const std::string& ident, uint32_t cp)
{
    Variant data;
    data["ident"] = ident;
    data["codepoint"] = cp;
    sendEvent("codepointMissing", data);
}

void GibbonBridge::pendingSync_createWidget(int widget)
{
    createSyncEvent();
    sSyncEvent->createWidget(widget);
}

void GibbonBridge::pendingSync_destroyWidget(int widget)
{
    createSyncEvent();
    sSyncEvent->destroyWidget(widget);
}

void GibbonBridge::pendingSync_addWidgetChange(int widget, int property, const Variant &value, bool append)
{
    createSyncEvent();
    sSyncEvent->addWidgetChange(widget, property, value, append);
}

void GibbonBridge::pendingSync_createImage(int widget, int id, const Variant &params)
{
    createSyncEvent();
    sSyncEvent->createImage(widget, id, params);
}

void GibbonBridge::pendingSync_destroyImage(int widget, int id)
{
    createSyncEvent();
    sSyncEvent->destroyImage(widget, id);
}

void GibbonBridge::pendingSync_addImageChange(int widget, int id, int property, const Variant &value, bool append)
{
    createSyncEvent();
    sSyncEvent->addImageChange(widget, id, property, value, append);
}

void GibbonBridge::pendingSync_createEffect(int widget, int id, const Variant &params)
{
    createSyncEvent();
    sSyncEvent->createEffect(widget, id, params);
}

void GibbonBridge::pendingSync_destroyEffect(int widget, int id)
{
    createSyncEvent();
    sSyncEvent->destroyEffect(widget, id);
}

void GibbonBridge::pendingSync_addEffectChange(int widget, int id, int property, const Variant &value, bool append)
{
    createSyncEvent();
    sSyncEvent->addEffectChange(widget, id, property, value, append);
}

void GibbonBridge::pendingSync_createText(int widget)
{
    createSyncEvent();
    sSyncEvent->createText(widget);
}

void GibbonBridge::pendingSync_destroyText(int widget)
{
    createSyncEvent();
    sSyncEvent->destroyText(widget);
}

void GibbonBridge::pendingSync_addTextChange(int widget, int property, const Variant &value, bool append)
{
    createSyncEvent();
    sSyncEvent->addTextChange(widget, property, value, append);
}

void GibbonBridge::beginPendingSync()
{
    assert(Application::isAppThread());
    ++sPendingSync;
}

void GibbonBridge::endPendingSync()
{
    assert(Application::isAppThread());
    if(!--sPendingSync && sSyncEvent) {
        sSyncEvent->eventPosted("SEND");
#if defined(GIBBON_NO_THREADANIMATION)
        GibbonApplication::instance()->postEvent(sSyncEvent);
#else
        Animation::postEvent(sSyncEvent);
#endif
        sSyncEvent.reset();
    }
}

void GibbonBridge::sync()
{
    assert(Application::isAppThread());
    if(sSyncEvent && !sSyncEvent->isEmpty()) {
        sSyncEvent->eventPosted("SYNC");
#if defined(GIBBON_NO_THREADANIMATION)
        GibbonApplication::instance()->postEvent(sSyncEvent);
#else
        Animation::sendEvent(sSyncEvent);
#endif
        sSyncEvent.reset(new GibbonJSSyncEvent);
    }
}

GibbonBridge::PendingSyncBlock::PendingSyncBlock()
{
    mBridge = GibbonApplication::instance()->getBridge();
    mBridge->beginPendingSync();
}
void GibbonBridge::setPrefetchUrls(const std::string &key, const Variant &urls)
{
    mPrefetchKey = key;
    if (urls.isNull()) {
        mEffectivePrefetchUrls = Variant(Variant::Type_StringMap);
    } else {
        mEffectivePrefetchUrls = urls;
    }
    const int properties[] = {
        Properties::GibbonBridge::prefetchKey,
        Properties::GibbonBridge::effectivePrefetchUrls
    };

    propertiesUpdated(properties, 2);
}

void GibbonBridge::updateDebug()
{
    propertyUpdated(Properties::GibbonBridge::debugFlags);
}

