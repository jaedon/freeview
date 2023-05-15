/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "NfObject.h"

#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/JSONFormatter.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/config.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/tr1.h>

#include <nrd/Response.h>
#include <nrd/Request.h>
#include "NrdApplication.h"
#include "EventConnection.h"

using namespace netflix;

struct NfObjectCount : public netflix::ObjectCount::Record
{
public:
    inline NfObjectCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    inline virtual ~NfObjectCount() { }
    virtual std::string describe(void *object) const { return static_cast<NfObject*>(object)->path(); }
};

NRDP_DEFINE_OBJECTCOUNT_RECORD(NfObjectCount, NfObject);

std::map<std::string, NfObject::ClassInfo> *NfObject::sClassInfo = 0;

static inline void logRequest(const Request &request)
{
    (void)request;
#if defined(NRDP_HAS_TRACING)
    if (!(request.flags() & Request::NoLog)) {
        std::ostringstream str;
        assert(request.object());
        str << "Method: " << request.object()->path() << "." << request.methodName() << "[" << request.method() << "]";
        if (!(request.flags() & Request::NoLogArgs)) {
            str << '(';
            const Variant &variant = request.variant();
            const std::map<std::string, Variant> variantMap = variant.stringMap();
            bool first = true;
            for(Variant::StringMapIterator it = variantMap.begin(); it != variantMap.end(); ++it) {
                if (it->first == "method" || it->first == "object" || it->first == "jsDebuggerStack")
                    continue;
                std::string value = it->second.value<std::string>(0, "");
                if (!first)
                    str << '&';
                str << it->first << '=';
                if (it->second.isString())
                    str << '"' << value << '"';
                else
                    str << value;
                first = false;
            }
            str << ')';
        }
        NTRACE(TRACE_NBP, "%s", str.str().c_str());
    }
#endif
}

namespace netflix {
class NfObjectApplicationEventListener : public ApplicationEventListener
{
public:
    NfObjectApplicationEventListener(const shared_ptr<NfObject> &owner)
        : mOwner(owner)
    {
    }

    virtual void event(ApplicationEvent::Type type, const Variant &data)
    {
        shared_ptr<NfObject> object = mOwner.lock();
        if (object)
            object->event(type, data);
    }
private:
    weak_ptr<NfObject> mOwner;
};
}

Mutex NfObject::sMutex(ZERO_MUTEX, "NBPBridges");
// props must be sorted in ascending order.
NfObject::NfObject(const std::string &name,
                   const PropertyInfo *props, int propertyCount,
                   const char **methodList, int methodCount,
                   unsigned int flags)
    : mName(name), mProperties(props), mPropertyCount(propertyCount),
      mMethodList(methodList), mMethodCount(methodCount),
      mFlags(flags)
{
    NRDP_OBJECTCOUNT_REF(NfObject, this);
#ifdef BUILD_DEBUG
    for (int i=1; i<mPropertyCount; ++i) {
        if (strcmp(mProperties[i - 1].name, mProperties[i].name) >= 0) {
            Log::fatal(TRACE_NBP,
                       "Properties out of order for %s, %s is before %s.\n"
                       "Properties need to be sorted alphabetically",
                       name.c_str(), mProperties[i - 1].name, mProperties[i].name);
            assert(0);
        }
    }
    for (int i=1; i<mMethodCount; ++i) {
        if (strcmp(mMethodList[i - 1], mMethodList[i]) >= 0) {
            Log::fatal(TRACE_NBP,
                       "Methods out of order for %s, %s is before %s.\n"
                       "Methods need to be sorted alphabetically",
                       name.c_str(), mMethodList[i - 1], mMethodList[i]);
            assert(0);
        }
    }
#endif
}

NfObject::NfObject(const std::string &name,
                   const char **methodList, int methodCount,
                   unsigned int flags)
    : mName(name), mProperties(0), mPropertyCount(0),
      mMethodList(methodList), mMethodCount(methodCount),
      mFlags(flags)
{
    NRDP_OBJECTCOUNT_REF(NfObject, this);
#ifdef BUILD_DEBUG
    for (int i=1; i<mMethodCount; ++i) {
        if (strcmp(mMethodList[i - 1], mMethodList[i]) >= 0) {
            Log::fatal(TRACE_NBP,
                       "Methods out of order for %s, %s is before %s.\n"
                       "Methods need to be sorted alphabetically",
                       name.c_str(), mMethodList[i - 1], mMethodList[i]);
            assert(0);
        }
    }
#endif
}

NfObject::NfObject(const std::string &name,
                   const PropertyInfo *propertyList, const char **methodList,
                   unsigned int flags)
    : mName(name), mProperties(propertyList), mPropertyCount(0),
      mMethodList(methodList), mMethodCount(0),
      mFlags(flags)
{
    NRDP_OBJECTCOUNT_REF(NfObject, this);
    if (mProperties) {
        while (mProperties[mPropertyCount].name) {
#ifdef BUILD_DEBUG
            if (mPropertyCount > 0 && strcmp(mProperties[mPropertyCount - 1].name, mProperties[mPropertyCount].name) >= 0) {
                Log::fatal(TRACE_NBP,
                           "Properties out of order for %s, %s is before %s.\n"
                           "Properties need to be sorted alphabetically",
                           name.c_str(), mProperties[mPropertyCount - 1].name, mProperties[mPropertyCount].name);
                assert(0);
            }
#endif
            ++mPropertyCount;
        }
    }

    if (mMethodList) {
        while (mMethodList[mMethodCount]) {
#ifdef BUILD_DEBUG
            if (mMethodCount > 0 && strcmp(mMethodList[mMethodCount - 1], mMethodList[mMethodCount]) >= 0) {
                Log::fatal(TRACE_NBP,
                           "Methods out of order for %s, %s is before %s.\n"
                           "Methods need to be sorted alphabetically",
                           name.c_str(), mMethodList[mMethodCount - 1], mMethodList[mMethodCount]);
                assert(0);
            }
#endif
            ++mMethodCount;
        }
    }
}

NfObject::NfObject(const std::string &name, unsigned int flags)
    : mName(name), mProperties(0), mPropertyCount(0), mMethodList(0), mMethodCount(0), mFlags(flags)
{
    NRDP_OBJECTCOUNT_REF(NfObject, this);
}

NfObject::~NfObject()
{
    NRDP_OBJECTCOUNT_DEREF(NfObject, this);
    stopEventListener();
    clearChildren();
    // ### we should unset parent on children before killing them
    setParent(shared_ptr<NfObject>()); //remove from tree
}

void NfObject::clearChildren()
{
    mChildren.clear();
}

void NfObject::startEventListener()
{
    assert(!mListener);
    mListener.reset(new NfObjectApplicationEventListener(shared_from_this()));
    nrdApp()->addEventListener(mListener);
}

void NfObject::stopEventListener()
{
    if (mListener) {
        nrdApp()->removeEventListener(mListener);
        mListener.reset();
    }
}

const NfObject::PropertyInfo *NfObject::properties() const
{
    return mProperties;
}

int NfObject::propertyCount() const
{
    return mPropertyCount;
}

const char **NfObject::methods() const
{
    return mMethodList;
}

int NfObject::methodCount() const
{
    return mMethodCount;
}

Variant NfObject::invoke(const Request &request)
{
    const int method = request.method();
    if (method != -1) {
        return invoke(method, request.variant());
    }
    Log::error(TRACE_NBP, "%s has no method called %s", mName.c_str(), request.methodName().c_str());
    return Variant();
}

int NfObject::propertyIndex(const std::string &name) const
{
    return binarySearchString(name, mProperties, mPropertyCount);
}

int NfObject::methodIndex(const std::string &name) const
{
    return binarySearchString(name, mMethodList, mMethodCount);
}

bool NfObject::getProperty(int, Variant */*result*/) const
{
    return false;
}

bool NfObject::setProperty(int, const Variant &)
{
    return false;
}

void NfObject::propertiesUpdated(const int *indexes, int count, unsigned int flags)
{
    const Variant map = properties(indexes, count);
    const Response response = Response::createPropertiesUpdate(shared_from_this(), map, flags);
    nrdApp()->sendResponse(response);
}

Variant NfObject::properties(const int *indexes, int count) const
{
    ASSERT(count > 0, "bad count %d", count);
    Variant map;
    for (int i=0; i<count; ++i) {
        const int index = indexes[i];
        ASSERT((index >= 0 && index < mPropertyCount), "index %d out of range (%d)",
               index, mPropertyCount);
        Variant value;
        if (getProperty(index, &value)) {
            map[mProperties[index].name] = value;
        } else {
            Log::error(TRACE_NBP, "Can't read property %s from %s", mProperties[index].name, name().c_str());
        }
    }
    return map;
}

void NfObject::setParent(const shared_ptr<NfObject> &newParent)
{
    assert(!parent() || !newParent);
    ScopedMutex _lock(sMutex);
    if (shared_ptr<NfObject> oldP = mParent.lock()) {
        oldP->mChildren.erase(mName.c_str());
    } else if (!newParent) {
        return;
    }
    mParent = newParent;

    if (newParent) {
        newParent->mChildren[mName.c_str()] = shared_from_this();
        updateHierarchy(newParent->path() + ".");
    } else {
        updateHierarchy(std::string());
    }
}

void NfObject::updateHierarchy(const std::string &parentPath)
{
    std::string path;
    if (!parentPath.empty()) {
        mPath = parentPath + mName;
        path = mPath + ".";
    } else {
        mPath.clear();
    }

    for (ObjectMap::const_iterator it = mChildren.begin(); it != mChildren.end(); ++it) {
        it->second->updateHierarchy(path);
    }
}

int NfObject::sendEvent(const std::string &eventName, const Variant &data, unsigned int flags, const Time &time)
{
    PERFORMANCE_MARKER_SCOPED("nfobject.sendevent");

    return nrdApp()->sendResponse(Response::createEvent(eventName, shared_from_this(), data, flags, time));
}

Variant NfObject::syncProperties() const
{
    Variant ret;
    if (mPropertyCount) {
        std::vector<int> p(mPropertyCount);
        for (int i=0; i<mPropertyCount; ++i)
            p[i] = i;
        ret = properties(&p[0], p.size());
    }
    return ret;
}

void NfObject::sync(const shared_ptr<EventConnection> &connection) const
{
    if (!connection || !connection->isFiltered(path())) {
        const Variant props = syncProperties();
        if (!props.isNull()) {
            nrdApp()->sendResponse(Response::createPropertiesUpdate(shared_from_this(), props));
        }
    }
}

void NfObject::invalidArgumentError(const std::string &method, const char *argument)
{
    Log::error(TRACE_NBP, "InvalidArgumentError: Invalid or missing value for %s in %s.%s(...)",
               argument, mName.c_str(), method.c_str());
    nrdApp()->sendResponse(Response::createMethodError(shared_from_this(), methodIndex(method), argument));
}

Variant NfObject::send(const Request &request)
{
    const Request::TransactionType requestType = request.type();
    switch (requestType) {
    case Request::Invalid:
    case Request::Event:
    case Request::ClassSync:
    case Request::PropertiesUpdate:
        ASSERT(0, "Bad requestType %d", requestType);
        break;
    case Request::Method:
        logRequest(request);
        return invoke(request);
    case Request::GetProperty:
    case Request::SetProperty: {
        const int propertyIndex = request.property();
        if (propertyIndex == -1) {
            Log::error(TRACE_NBP, "protocol error, invalid property %s for object %s", request.propertyName().c_str(), path().c_str());
            return false;
        }

        if (requestType == Request::SetProperty) {
            const Variant value = request.propertyValue();
            if (!setProperty(propertyIndex, value)) {
                Log::error(TRACE_NBP, "protocol error, Can't set property %s[%d] for object %s to %d (%s)",
                           request.propertyName().c_str(), propertyIndex, path().c_str(), value.type(), value.toJSON().c_str());
                return false;
            }

            bool result = false;
            if (!(request.flags() & Request::Synchronous)) {
                const Response response = Response::createSetProperty(shared_from_this(), propertyIndex, value);
                nrdApp()->sendResponse(response);
                result = true;
            }

#ifdef NRDP_HAS_TRACING
            JSONFormatter formatter;
            formatter.setMaxStringLength(5 * 1024);
            NTRACE(TRACE_NBP, "SetProperty: %s.%s[%d] = %s;",
                   path().c_str(),
                   request.propertyName().c_str(), propertyIndex,
                   formatter.format(request.propertyValue()).c_str());
#endif
            return result;
        }
        propertyUpdated(propertyIndex);
        NTRACE(TRACE_NBP, "GetProperty: %s.%s[%d] (%s);", path().c_str(),
               request.propertyName().c_str(), propertyIndex, request.propertyValue().toJSON().c_str());

        return true; }
    }
    Log::error(TRACE_NBP, "Internal error. This should never happen");
    return false;
}

int NfObject::registerMethods(const std::string &className, const char **methods)
{
    if (!sClassInfo)
        sClassInfo = new std::map<std::string, ClassInfo>;
    ClassInfo &info = (*sClassInfo)[className];
    info.methods = methods;
    return 1;
}

int NfObject::registerProperties(const std::string &className, const PropertyInfo *properties)
{
    if(!sClassInfo)
        sClassInfo = new std::map<std::string, ClassInfo>;
    if(sClassInfo->find(className) == sClassInfo->end()) {
        ClassInfo info;
        info.properties = properties;
        info.methods = 0;
        sClassInfo->insert(std::make_pair(className, info));
    } else {
        (*sClassInfo)[className].properties = properties;
    }
    return 1;
}
