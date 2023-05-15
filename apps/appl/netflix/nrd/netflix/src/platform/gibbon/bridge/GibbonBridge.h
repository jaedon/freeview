/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __GIBBONBRIDGE_H__
#define __GIBBONBRIDGE_H__

#include <ScreenBridge.h>
#include <DebuggerBridge.h>
#include <GibbonBaseBridge.h>
#include <nrdnet/AseUrl.h>

namespace netflix {
namespace gibbon {

class GibbonEvent;
class WidgetBridge;
class GibbonBridgeTask;
class GibbonBridgeTimer;
class GibbonBridgeRequest;
#ifdef BUILD_QA
class GibbonQABridge;
#endif

class GibbonBridge : public GibbonBaseBridge
{
public:
    GibbonBridge();
    ~GibbonBridge();

    void init();
    void cleanup();

    void setPrefetchUrls(const std::string &key, const Variant &urls);

    void clearScene();
    void removeWidget(int id);
    void handleEvent(GibbonEvent *e);
    void handleTimerEvent(int id);
    void handleTaskEvent(int id);
    void cookiesChanged();
    void locationChanged();
    void fontsChanged();
    void suspendChanged(bool suspend, const std::string &reason);

    shared_ptr<WidgetBridge> getRoot();
    shared_ptr<WidgetBridge> getOverlay();

    shared_ptr<WidgetBridge> getWidget(int id);
    shared_ptr<WidgetBridge> createWidget(int id);
    shared_ptr<WidgetBridge> findWidget(const DumpInfo &dump) const;

    inline shared_ptr<DebuggerBridge> getDebugger() const { return mDebuggerBridge; }

    void stopAllTimers();
    void startAllTimers();

    void dumpTasks() const;

    bool startRequest(const Variant &data);
    void stopRequest(int id);

    void updateDebug();

    void reportHighWatermark(ullong size);
    void reportCodepointMissing(const std::string& ident, uint32_t cp);

    virtual unsigned requestFlags(const Request &request) const;
    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &args);

    class PendingSyncBlock {
    public:
        PendingSyncBlock();
        ~PendingSyncBlock() { mBridge->endPendingSync(); }
    protected:
        shared_ptr<GibbonBridge> mBridge;
    };
    void pendingSync_createWidget(int widget);
    void pendingSync_destroyWidget(int widget);
    void pendingSync_addWidgetChange(int widget, int property, const Variant &value, bool append);
    void pendingSync_createImage(int widget, int id, const Variant &params);
    void pendingSync_destroyImage(int widget, int id);
    void pendingSync_addImageChange(int widget, int id, int property, const Variant &value, bool append);
    void pendingSync_createEffect(int widget, int id, const Variant &params);
    void pendingSync_destroyEffect(int widget, int id);
    void pendingSync_addEffectChange(int widget, int id, int property, const Variant &value, bool append);
    void pendingSync_createText(int widget);
    void pendingSync_destroyText(int widget);
    void pendingSync_addTextChange(int widget, int property, const Variant &value, bool append);
    void beginPendingSync();
    void endPendingSync();
    void sync();

protected:
    bool startRequest(const shared_ptr<GibbonBridgeRequest> &request);

    friend class GibbonBridgeRequest;
    friend class GibbonBridgeTask;

    mutable Mutex mMutex;
    shared_ptr<NfObject> mWidgetsBridge;
    shared_ptr<ScreenBridge> mSceneBridge;
    shared_ptr<DebuggerBridge> mDebuggerBridge;
#ifdef BUILD_QA
    shared_ptr<GibbonQABridge> mGibbonQABridge;
#endif

    void requestFinished(const shared_ptr<GibbonBridgeRequest> &request);
    std::map<int, weak_ptr<WidgetBridge> > mWidgets;
    std::map<int, shared_ptr<GibbonBridgeTask> > mTasks;
    std::map<int, shared_ptr<GibbonBridgeTimer> > mTimers;
    std::map<int, shared_ptr<GibbonBridgeRequest> > mRequests;
    std::string mPrefetchKey;
    Variant mEffectivePrefetchUrls;
    Variant mPersistentInjectJS;

private:
    void cpuProfileStart(const Variant& data);
    void cpuProfileStop(const Variant& data);
};

}} // namespace netflix::gibbon

#endif /* __GIBBONBRIDGE_H__ */
