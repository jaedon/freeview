/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "Response.h"

#include <nrd/NfObject.h>
#include <nrd/NrdApplication.h>

using namespace netflix;

NRDP_DEFINE_OBJECTCOUNT(NBPResponse);

Variant Response::toVariant() const
{
    Variant map;
    switch (type()) {
    case Request::Invalid:
        map["type"] = "Error";
        break;
    case Request::PropertiesUpdate:
        assert(mObject);
        map["type"] = "PropertyUpdate";
        map["properties"] = mValue;
        break;
    case Request::ClassSync:
        assert(!mObject);
        map["type"] = "ClassSync";
        map["name"] = eventName();
        map["value"] = mValue;
        break;
    case Request::Event:
        map["type"] = "Event";
        map["name"] = eventName();
        if (!mValue.isNull())
            map["data"] = mValue;
        break;
    case Request::Method:
        assert(mObject);
        map["type"] = "Method";
        map["method"] = mObject->methodName(method());
        map["returnValue"] = mValue;
        break;
    case Request::SetProperty:
    case Request::GetProperty:
        assert(mObject);
        map["type"] = (type() == Request::SetProperty ? "SetProperty" : "GetProperty");
        map["property"] = mObject->propertyName(property());
        map["value"] = mValue;
        break;
    }
    map["time"] = mTime.ms();
    if (mObject) {
        std::string key;
        Variant id;
        mObject->objectId(key, id);
        map[key] = id;
    }
    return map;
}

Response Response::fromVariant(const Variant &map)
{
    Response result;
    const std::string type = map.mapValue<std::string>("type");
    if (type == "Event") {
        result.mValue = map.mapValue("data");
        result.mName = map.mapValue<std::string>("name");
        result.mType = Request::Event;
    } else if (type == "PropertyUpdate") {
        result.mValue = map.mapValue("properties");
        result.mType = Request::PropertiesUpdate;
    } else if (type == "Method") {
        result.mValue = map.mapValue("returnValue");
        result.mName = map.mapValue<std::string>("name");
        result.mType = Request::Method;
    } else if (type == "GetProperty") {
        result.mValue = map.mapValue("value");
        result.mName = map.mapValue<std::string>("name");
        result.mType = Request::GetProperty;
    } else if (type == "SetProperty") {
        result.mValue = map.mapValue("value");
        result.mName = map.mapValue<std::string>("name");
        result.mType = Request::SetProperty;
    }
    result.mTime = Time::fromMS(map.mapValue<ullong>("time"));
    result.mObject = nrdApp()->findObject(map.mapValue<std::string>("object"));
    return result;
}


