/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef Request_h
#define Request_h

#include <string.h>

#include <nrd/NfObject.h>
#include <nrdbase/Variant.h>

namespace netflix {

class Request
{
public:
    enum Flag {
        None = 0x00,
        FromTrustedSource = 0x01,
        Synchronous = 0x02,
        JSBacktrace = 0x04,
        NoLog = 0x08,
        NoLogArgs = 0x10,
        Queued = 0x20
    };

    enum TransactionType {
        Invalid = 0,
        Method = 1,
        SetProperty = 2,
        GetProperty = 3,
        Event = 4,
        PropertiesUpdate = 5,
        ClassSync = 6
    };

    Request(const Variant &data = Variant(), uint32_t flags=None);

    inline uint32_t flags() const { return mFlags; }
    inline TransactionType type() const { return mType; }

    std::string describe() const
    {
        std::ostringstream stream;
        stream << "Request(";
        switch (mType) {
        case Invalid: stream << "Invalid"; break;
        case Method: stream << "Method"; break;
        case SetProperty: stream << "SetProperty"; break;
        case GetProperty: stream << "GetProperty"; break;
        case Event: stream << "Event"; break;
        case PropertiesUpdate: stream << "PropertiesUpdate"; break;
        case ClassSync: stream << "ClassSync"; break;
        }
        stream << ", flags: " << mFlags;
        if (mFlags & NoLogArgs) {
            stream << " object: " << mVariant.mapValue<std::string>("object");
            switch (mType) {
            case Method:
                stream << " method: " << methodName();
                break;
            case GetProperty:
            case SetProperty:
                stream << " property: " << propertyName();
                break;
            case Event:
            case Invalid:
            case PropertiesUpdate:
            case ClassSync:
                break;
            }
        } else {
            stream << " data: " << mVariant.toJSON(true);
        }
        stream << ')';
        return stream.str();
    }

    inline const Variant &variant() const {
        return mVariant;
    }
    inline void setMethod(int method) { mMethod = method; }
    inline std::string methodName() const { return mVariant.mapValue<std::string>("method"); }
    inline int method() const {
        if (mMethod == -1) {
            if (shared_ptr<NfObject> o = object())
                mMethod = o->methodIndex(methodName());
        }
        return mMethod;
    }
    shared_ptr<NfObject> object() const { return mObject.lock(); }
    inline void setProperty(int property) { mProperty = property; }
    inline std::string propertyName() const { return mVariant.mapValue<std::string>("property"); }
    inline int property() const {
        if (mProperty == -1) {
            if (shared_ptr<NfObject> o = object())
                mProperty = o->propertyIndex(propertyName());
        }
        return mProperty;
    }

    inline bool hasPropertyValue() const {
        return mVariant.contains("value");
    }

    inline Variant propertyValue() const {
        return value("value");
    }
    inline bool hasValue(const char *name) const {
        return mVariant.contains(name);
    }
    inline Variant value(const char *name, bool *ok = 0) const {
        const std::map<std::string, Variant>::const_iterator it = mVariant.find(name);
        if (it != mVariant.stringMapEnd())
        {
            if (ok)
                *ok = true;
            return it->second;
        }
        if (ok)
            *ok = false;
        return Variant();
    }

    template <typename T> void setValue(const char *name, const T &t)
    {
        mVariant[name] = t;
    }
    template <typename T> T value(const char *name, bool *ok = 0, const T &t = T()) const
    {
        return mVariant.mapValue<T>(name,ok,t);
    }
private:
    inline TransactionType determineType() const {
        bool ok;
        mVariant.mapValue<std::string>("object", &ok);
        if (!ok)
            return Invalid;
        mVariant.mapValue<std::string>("method", &ok);
        if (ok)
            return Method;
        mVariant.mapValue<std::string>("property", &ok);
        if (!ok)
            return Invalid;
        return hasPropertyValue() ? SetProperty : GetProperty;
    }

    weak_ptr<NfObject> mObject;
    Variant mVariant;
    mutable int mProperty, mMethod;
    TransactionType mType;
    uint32_t mFlags;
};
}

#endif
