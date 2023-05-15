/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __DEBUGGERBRIDGE_H__
#define __DEBUGGERBRIDGE_H__

#include "ScriptEngine.h"

#include <nrdbase/Instrumentation.h>
#include <nrdbase/Stopwatch.h>
#include <nrdbase/LogSink.h>
#include <nrdbase/tr1.h>

#include <nrd/NfObject.h>

#include <nrdnet/ResourceRequest.h>
#include <nrdnet/AseUrl.h>

namespace netflix {
namespace gibbon {

class Widget;

class DebuggerBridge : public NfObject
{
public:
    DebuggerBridge();

    void init();
    void cleanup();
    inline bool isEnabled() const { return mEnableDebug; }

    void updateLocation(const AseUrl &url);

    void beginRequest(const ResourceRequest::SharedPtr &request);
    void endRequest(const ResourceRequest::SharedPtr &request,
                    const Resource::SharedPtr &response,
                    Resource::State);

    void onLocationChanged();
    void onSceneChanged();
    void updateWidget(const Variant& properties);
    void destroyWidget(int id);

    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &args);

    void receive(const Log::Message &msg);
    void processInstrumentation(const Variant &inst);

    void notifyClicked(const std::list<shared_ptr<Widget> >& widget);

    unsigned requestFlags(const Request &request) const;

private:
    void resolveChain(int frameNo, int scopeNo);

    void onChanged(ScriptEngine::DebuggerListener::Type type);
    class Listener : public ScriptEngine::DebuggerListener,
                     public enable_shared_from_this<ScriptEngine::DebuggerListener>
    {
    public:
        Listener(DebuggerBridge &bridge)
            : mBridge(bridge)
        {}

        virtual void onChanged(ScriptEngine::DebuggerListener::Type type)
        {
            mBridge.onChanged(type);
        }
        DebuggerBridge &mBridge;
    };

    ScriptEngine::SharedPtr scriptEngine() const;
    int mSendProfile, mSendNetwork, mEnableDebug;
    std::vector<ScriptEngine::Breakpoint> mBreakpoints;
    std::vector<ScriptEngine::StackFrame> mFrames;
    int mFrame;
    ScriptEngine::DebuggerState mDebuggerState;
    Variant mIdentifiers;
    std::vector<std::vector<Variant> > mScopeChain;
    shared_ptr<Listener> mListener;
#ifdef INSTRUMENTATION_ENABLED
    shared_ptr<instrumentation::Listener> mInstrumenation;
#endif

    shared_ptr<LogSinkForwarder<DebuggerBridge> > mSink;
    friend class DebuggerEvent;
};

}} // namespace netflix::gibbon

#endif /* __DEBUGGERBRIDGE_H__ */
