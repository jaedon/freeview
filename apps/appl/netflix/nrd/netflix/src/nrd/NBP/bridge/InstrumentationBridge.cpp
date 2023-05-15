/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "InstrumentationBridge.h"
#include <nrdbase/Instrumentation.h>
#include <nrd/Request.h>
#include "NrdApplication.h"
#include "SystemData.h"

using namespace netflix;

//-----------------------------------------------------------------------------

#define InstrumentationPropertyList(Property) \
    Property(verbose, NfObject::ReadOnly)

PropertyTable(InstrumentationBridge, InstrumentationPropertyList, properties)

#define InstrumentationMethodList(Method)       \
    Method(event)                               \
    Method(flush)                               \
    Method(intervalCancel)                      \
    Method(intervalCount)                       \
    Method(intervalEnd)                         \
    Method(intervalStart)                       \
    Method(intervalTag)                         \
    Method(popStash)                            \
    Method(setParams)                           \
    Method(stash)

MethodTable(InstrumentationBridge, InstrumentationMethodList)

//-----------------------------------------------------------------------------

InstrumentationBridge::InstrumentationBridge()
    : NfObject("instrumentation",
               Properties::InstrumentationBridge::properties, Properties::InstrumentationBridge::PropertyCount,
               Methods::InstrumentationBridge::methods, Methods::InstrumentationBridge::MethodCount),
      mVerbose(true)
{
}

void InstrumentationBridge::setVerbose(bool verbose)
{
    mVerbose = verbose;
    propertyUpdated(Properties::InstrumentationBridge::verbose);
    sendEvent("verboseChanged");
}

#ifdef INSTRUMENTATION_ENABLED

void InstrumentationBridge::init()
{
    class Listener : public instrumentation::Listener
    {
    public:
        Listener(weak_ptr<InstrumentationBridge> bridge)
            : mBridge(bridge)
        {}

        virtual void receive(const Variant &)
        {}

        virtual void verboseChanged(bool verbose) {
            if (shared_ptr<InstrumentationBridge> bridge = mBridge.lock())
                bridge->setVerbose(verbose);
        }
    private:
        weak_ptr<InstrumentationBridge> mBridge;
    };
    instrumentation::Listener *listener = new Listener(static_pointer_cast<InstrumentationBridge>(shared_from_this()));
    INSTRUMENTATION_ADD_LISTENER(shared_ptr<instrumentation::Listener>(listener));
}

//-----------------------------------------------------------------------------

typedef std::pair<bool, std::string> CommonResult;

static CommonResult getCommon(const Variant &data, instrumentation::Type *type, std::string *name, Variant *value)
{
    assert(type);
    assert(name);

    bool ok;
    *type = static_cast<instrumentation::Type>(data.mapValue<int>("category", &ok));
    ok &= ((*type == instrumentation::On) || (*type == instrumentation::Switched) || (*type == instrumentation::Test)
           || (*type == instrumentation::Debug));
    if (!ok)
        return CommonResult(false, "category");

    *name = data.mapValue<std::string>("name", &ok);
    ok &= (!name->empty());
    if (!ok)
        return CommonResult(false, "name");

    if (value) {
        *value = data.value("value", &ok);
        ok &= (value->isNull() || value->isStringMap());
        if (!ok)
            return CommonResult(false, "value");
    }

    return CommonResult(true, std::string());
}

//-----------------------------------------------------------------------------

Variant InstrumentationBridge::invoke(int method, const Variant &data)
{
    instrumentation::Type type;
    std::string name;

    switch (method) {
    case Methods::InstrumentationBridge::event: {
        Variant value;
        CommonResult result = getCommon(data, &type, &name, &value);

        if (!result.first) {
            invalidArgumentError(method, result.second.c_str());
            return false;
        }

        switch (type) {
        case instrumentation::On:
            INSTRUMENTATION_EVENT_ON(name, value);
            break;
        case instrumentation::Switched:
            INSTRUMENTATION_EVENT_SWITCHED(name, value);
            break;
        case instrumentation::Test:
            INSTRUMENTATION_EVENT_TEST(name, value);
            break;
        case instrumentation::Debug:
            INSTRUMENTATION_EVENT_DEBUG(name, value);
            break;
        default:
            return false;
        }
        break;
    }
    case Methods::InstrumentationBridge::intervalStart: {
        Variant value;
        CommonResult result = getCommon(data, &type, &name, &value);

        if (!result.first) {
            invalidArgumentError(method, result.second.c_str());
            return false;
        }

        unsigned reset = data.mapValue<unsigned>("reset", 0, 0);

        switch (type) {
        case instrumentation::On:
            INSTRUMENTATION_INTERVAL_ON_START(name, value, reset);
            break;
        case instrumentation::Switched:
            INSTRUMENTATION_INTERVAL_SWITCHED_START(name, value, reset);
            break;
        case instrumentation::Test:
            INSTRUMENTATION_INTERVAL_TEST_START(name, value, reset);
            break;
        case instrumentation::Debug:
            INSTRUMENTATION_INTERVAL_DEBUG_START(name, value, reset);
            break;
        default:
            return false;
        }
        break;
    }
    case Methods::InstrumentationBridge::intervalCount: {
        CommonResult result = getCommon(data, &type, &name, 0);

        if (!result.first) {
            invalidArgumentError(method, result.second.c_str());
            return false;
        }

        std::string counter = data.mapValue<std::string>("counter");
        if (counter.empty()) {
            invalidArgumentError(method, "counter");
            return false;
        }

        llong increment = data.mapValue<llong>("increment", 0, 1);

        switch (type) {
        case instrumentation::On:
            INSTRUMENTATION_INTERVAL_ON_COUNT(name, counter, increment);
            break;
        case instrumentation::Switched:
            INSTRUMENTATION_INTERVAL_SWITCHED_COUNT(name, counter, increment);
            break;
        case instrumentation::Test:
            INSTRUMENTATION_INTERVAL_TEST_COUNT(name, counter, increment);
            break;
        case instrumentation::Debug:
            INSTRUMENTATION_INTERVAL_DEBUG_COUNT(name, counter, increment);
            break;
        default:
            return false;
        }
        break; }
    case Methods::InstrumentationBridge::intervalTag: {
        Variant value;
        CommonResult result = getCommon(data, &type, &name, &value);

        if (!result.first) {
            invalidArgumentError(method, result.second.c_str());
            return false;
        }

        switch (type) {
        case instrumentation::On:
            INSTRUMENTATION_INTERVAL_ON_TAG(name, value);
            break;
        case instrumentation::Switched:
            INSTRUMENTATION_INTERVAL_SWITCHED_TAG(name, value);
            break;
        case instrumentation::Test:
            INSTRUMENTATION_INTERVAL_TEST_TAG(name, value);
            break;
        case instrumentation::Debug:
            INSTRUMENTATION_INTERVAL_DEBUG_TAG(name, value);
            break;
        default:
            return false;
        }
        break; }
    case Methods::InstrumentationBridge::intervalEnd: {
        Variant value;
        CommonResult result = getCommon(data, &type, &name, &value);

        if (!result.first) {
            invalidArgumentError(method, result.second.c_str());
            return false;
        }

        switch (type) {
        case instrumentation::On:
            INSTRUMENTATION_INTERVAL_ON_END(name, value);
            break;
        case instrumentation::Switched:
            INSTRUMENTATION_INTERVAL_SWITCHED_END(name, value);
            break;
        case instrumentation::Test:
            INSTRUMENTATION_INTERVAL_TEST_END(name, value);
            break;
        case instrumentation::Debug:
            INSTRUMENTATION_INTERVAL_DEBUG_END(name, value);
            break;
        default:
            return false;
        }
        break; }
    case Methods::InstrumentationBridge::flush: {
        instrumentation::flush();
        Variant event;
        event["idx"] = data.mapValue<std::string>("idx");
        sendEvent("flush", event);
        break; }
    case Methods::InstrumentationBridge::intervalCancel: {
        CommonResult result = getCommon(data, &type, &name, 0);

        if (!result.first) {
            invalidArgumentError(method, result.second.c_str());
            return false;
        }

        switch (type) {
        case instrumentation::On:
            INSTRUMENTATION_INTERVAL_ON_CANCEL(name);
            break;
        case instrumentation::Switched:
            INSTRUMENTATION_INTERVAL_SWITCHED_CANCEL(name);
            break;
        case instrumentation::Test:
            INSTRUMENTATION_INTERVAL_TEST_CANCEL(name);
            break;
        case instrumentation::Debug:
            INSTRUMENTATION_INTERVAL_DEBUG_CANCEL(name);
            break;
        default:
            return false;
        }
        break; }
    case Methods::InstrumentationBridge::stash: {
        bool on = data.mapValue<bool>("on");
        instrumentation::stash(on);
        break; }
    case Methods::InstrumentationBridge::popStash: {
        Variant event;
        event["idx"] = data.mapValue<std::string>("idx");
        event["events"] = instrumentation::pop_stash();
        sendEvent("stashEvents", event);
        break; }
    case Methods::InstrumentationBridge::setParams: {
        bool enabled = data.mapValue<bool>("enabled");
        nrdApp()->systemData()->setInstrumentationEnabled(enabled);
        std::string events = data.mapValue<std::string>("events");
        nrdApp()->systemData()->setInstrumentationEvents(events);
        Log::info(TRACE_INSTRUMENTATION, "INSTRUMENTATION PARAMETERS CHANGED");
        instrumentation::config();
        break; }
    default: {
        return NfObject::invoke(method, data);
    }
    }
    return Variant();
}

bool InstrumentationBridge::getProperty(int index, Variant *result) const
{
    switch (index) {
    case Properties::InstrumentationBridge::verbose:
        *result = mVerbose;
        break;
    default:
        return false;
    }
    return true;
}

#else

void InstrumentationBridge::init()
{
}

bool InstrumentationBridge::invoke(int, const Variant &)
{
    return true;
}

bool InstrumentationBridge::getProperty(int index, Variant *result) const
{
    switch (index) {
    case Properties::InstrumentationBridge::verbose:
        *result = false;
        break;
    default:
        return false;
    }
    return true;
}

#endif
