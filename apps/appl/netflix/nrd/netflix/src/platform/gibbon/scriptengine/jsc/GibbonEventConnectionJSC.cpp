/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "JSC.h"
#include "GibbonEventConnectionJSC.h"
#include <Debug.h>
#include <GibbonApplication.h>
#include <nrd/AppLog.h>
#include <nrdbase/PerformanceMarker.h>
#include GIBBON_JSC_API(JSObjectRef.h)

extern "C" {
JSStringRef Netflix_JSStringCreate(const char* string, int length);
}

using namespace netflix;
using namespace netflix::gibbon::JSC;
using namespace netflix::gibbon;

Mutex GibbonEventConnection::sMutex(GIBBON_EVENTCONNECTIONJSC_MUTEX, "GibbonEventConnection::Mutex");
GibbonEventConnection::GibbonEventConnection(const ProtectedValue &callback)
    : EventConnection("JSC"), mCallback(callback)
{
    std::set<std::string> out;
    out.insert("nrdp.gibbon.debugger");
    setFilters(std::set<std::string>(), out);
}

void GibbonEventConnection::shutdown()
{
    mCallback.clear();
    mPending.clear();
}

static inline std::string describeResponse(const Response &response)
{
    switch(response.type()) {
    case Request::Invalid:
        return "Error";
    case Request::Method:
        return "Method";
    case Request::SetProperty:
        return "SetProperty";
    case Request::GetProperty:
        return "GetProperty";
    case Request::PropertiesUpdate:
        return "PropertiesUpdate";
    case Request::Event:
    case Request::ClassSync:
        return response.eventName();
    }
    return "NA";
}

void GibbonEventConnection::sendEvent(const Response &response, int timerPreemption)
{
    if (mCallback.isNull())
        return;
    GibbonBridge::PendingSyncBlock syncScope;
#if 0
    JSContextRef ctx = ScriptEngine::currentContext();
    ProtectedValue obj = JSObjectMake(ctx, 0, 0);
    JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_type),
                        JSValueMakeNumber(ctx, response.time().ms()),
                        kJSPropertyAttributeNone, 0);
    const shared_ptr<const NfObject> &object = response.object();
    if(object) {
        std::string key;
        Variant id;
        object->objectId(key, id);
        JSObjectSetProperty(ctx, obj, ScopedString(Netflix_JSStringCreate(key.c_str(), key.size())),
                            VariantToJSValue(ctx, id, 0),
                            kJSPropertyAttributeNone, 0);
    }
    switch(response.type()) {
    case Request::Invalid:
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_type),
                            JSValueMakeString(ctx, JSC::generatedString(JSC::String_Error)),
                            kJSPropertyAttributeNone, 0);
        break;
    case Request::PropertiesUpdate:
        assert(object);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_type),
                            JSValueMakeString(ctx, JSC::generatedString(JSC::String_PropertyUpdate)),
                            kJSPropertyAttributeNone, 0);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_properties),
                            VariantToJSValue(ctx, response.value(), 0),
                            kJSPropertyAttributeNone, 0);
        break;
    case Request::ClassSync:
        assert(!object);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_type),
                            JSValueMakeString(ctx, JSC::generatedString(JSC::String_ClassSync)),
                            kJSPropertyAttributeNone, 0);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_name),
                            stringToJSValue(ctx, response.eventName()),
                            kJSPropertyAttributeNone, 0);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_value),
                            VariantToJSValue(ctx, response.value(), 0),
                            kJSPropertyAttributeNone, 0);
        break;
    case Request::Event:
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_type),
                            JSValueMakeString(ctx, JSC::generatedString(JSC::String_Event)),
                            kJSPropertyAttributeNone, 0);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_name),
                            stringToJSValue(ctx, response.eventName()),
                            kJSPropertyAttributeNone, 0);
        if(!response.value().isNull())
            JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_data),
                                VariantToJSValue(ctx, response.value(), 0),
                                kJSPropertyAttributeNone, 0);
        break;
    case Request::Method:
        assert(object);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_type),
                            JSValueMakeString(ctx, JSC::generatedString(JSC::String_Method)),
                            kJSPropertyAttributeNone, 0);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_method),
                            stringToJSValue(ctx, std::string(object->methodName(response.method()))),
                            kJSPropertyAttributeNone, 0);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_returnValue),
                            VariantToJSValue(ctx, response.value(), 0),
                            kJSPropertyAttributeNone, 0);
        break;
    case Request::SetProperty:
    case Request::GetProperty:
        assert(object);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_type),
                            JSValueMakeString(ctx, JSC::generatedString(response.type() == Request::SetProperty ? JSC::String_SetProperty : JSC::String_GetProperty)),
                            kJSPropertyAttributeNone, 0);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_property),
                            stringToJSValue(ctx, std::string(object->propertyName(response.property()))),
                            kJSPropertyAttributeNone, 0);
        JSObjectSetProperty(ctx, obj, JSC::generatedString(JSC::String_value),
                            VariantToJSValue(ctx, response.value(), 0),
                            kJSPropertyAttributeNone, 0);
        break;

    }
#else
    Variant value = response.toVariant();
    if (timerPreemption)
        value["timerPreemption"] = timerPreemption;
    ProtectedValue obj = VariantToJSValue(value, 0);
#endif

    shared_ptr<GibbonBridge> bridge = gibbonApp()->getBridge();
    assert(bridge);
    PERFORMANCE_MARKER_SCOPED((std::string("gibboneventconnection.sendevent ") + describeResponse(response)).c_str(), false);
    // INSTRUMENTATION_INTERVAL_SWITCHED_START("gibbon.jsc.callback", Variant::StringMap("type", value["type"])("name", value["name"]));
    assert(Application::isAppThread());
    assert(ScriptEngine::currentContext());
    int bytes = 0;
    NRDP_UNUSED(bytes);
    NTRACE(TRACE_GIBBON_SCRIPT, "* JSC_Event: send: %s [%s] waited %llu ms",
           prettyBytes(bytes).c_str(), describeResponse(response).c_str(), (Time::mono() - response.time()).ms());
    const JSValueRef val = obj.value();
    JSObjectCallAsFunction(ScriptEngine::currentContext(), mCallback, mCallback, 1, &val, 0);
    ScriptEngine::SharedPtr engine = GibbonApplication::instance()->getScriptEngine();
    engine->dirtyGCTimer();
    INSTRUMENTATION_INTERVAL_SWITCHED_END("gibbon.jsc.callback");
}

bool GibbonEventConnection::writeResponse(const Response &response)
{
    if((response.flags() & Response::Flag_Synchronous) && Application::isAppThread() &&
       !GibbonApplication::instance()->getScriptEngine()->isInBreakpoint()) {
        sendEvent(response, 0);
        return true;
    }

    shared_ptr<GibbonJSEvent> event(new GibbonJSEvent(shared_from_this(), response));
    GibbonApplication::instance()->postEvent(event);
    return true;
}

void GibbonEventConnection::continueFromBreakpoint()
{
    assert(Application::isAppThread());
    std::list<shared_ptr<GibbonJSEvent> > pending;
    std::swap(pending, mPending);
    for (std::list<shared_ptr<GibbonJSEvent> >::const_iterator it = pending.begin(); it != pending.end(); ++it) {
        app()->postEvent(*it);
    }
}

void GibbonEventConnection::sendEvent(const shared_ptr<GibbonJSEvent> &event)
{
    GibbonApplication *app = GibbonApplication::instance();
    shared_ptr<ScriptEngine> scriptEngine = app->getScriptEngine();
    if(!scriptEngine || !scriptEngine->isStarted())
        return;
    if(scriptEngine->isInBreakpoint())
        mPending.push_back(event);
    else
        sendEvent(event->mResponse, event->timerPreemption());
}
