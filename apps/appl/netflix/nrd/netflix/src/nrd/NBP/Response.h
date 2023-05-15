/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef Response_h
#define Response_h

#include <nrd/Request.h>
#include <nrdbase/Variant.h>

NRDP_DECLARE_OBJECTCOUNT(NBPResponse);

namespace netflix {

class Response
{
public:
    inline Response() : mType(Request::Invalid), mTime(Time::mono()), mFlags(0)
    {
        NRDP_OBJECTCOUNT_REF(NBPResponse, this);
    }

    inline Response(const Response &other)
        : mType(other.mType), mObject(other.mObject), mName(other.mName),
          mValue(other.mValue), mTime(other.mTime), mFlags(other.mFlags)
    {
        NRDP_OBJECTCOUNT_REF(NBPResponse, this);
    }

    inline ~Response()
    {
        NRDP_OBJECTCOUNT_DEREF(NBPResponse, this);
    }

    enum Flag {
        Flag_None = 0x0,
        Flag_Synchronous = 0x1,
        Flag_HighPriority = 0x2
    };
    inline unsigned int flags() const { return mFlags; }

    inline Request::TransactionType type() const { return mType; }
    inline int method() const {
        assert(mType == Request::Method);
        return mName.integer();
    }
    inline int property() const {
        switch (mType) {
        case Request::GetProperty:
        case Request::SetProperty:
            break;
        default:
            assert(0);
            break;
        }
        return mName.integer();
    }
    inline std::string eventName() const {
        assert(mType == Request::Event || mType == Request::ClassSync);
        return mName.string();
    }
    inline const shared_ptr<const NfObject> &object() const { return mObject; }
    inline const Variant &value() const { return mValue; }
    inline Time time() const { return mTime; }

    static inline Response createSetProperty(const shared_ptr<const NfObject> &object, int property, const Variant& value)
    {
        assert(object);
        return Response(Request::SetProperty, object, property, value, Flag_None);
    }

    static inline Response createEvent(const std::string &eventName,
                                       const shared_ptr<const NfObject> &object = shared_ptr<const NfObject>(),
                                       const Variant& value = Variant(),
                                       unsigned int flags = 0, const Time &time = Time::mono())
    {
        return Response(Request::Event, object, eventName, value, flags, time);
    }

    static inline Response createPropertiesUpdate(const shared_ptr<const NfObject> &object,
                                                  const Variant &properties, unsigned int flags = 0)
    {
        assert(object);
        return Response(Request::PropertiesUpdate, object, std::string(), properties, flags);
    }

    static inline Response createMethodError(const shared_ptr<const NfObject> &object, int method, const char *argument)
    {
        assert(object);
        return Response(Request::Method, object, method, argument, Flag_None);
    }

    static inline Response createClassSync(const std::string &className, const Variant &classInfo)
    {
        return Response(Request::ClassSync, shared_ptr<const NfObject>(), className, classInfo, Flag_Synchronous);
    }

    Variant toVariant() const;
    static Response fromVariant(const Variant &json);

    inline void clear() {
        mType = Request::Invalid;
        mObject.reset();
        mName.clear();
        mValue.clear();
        mTime = Time();
        mFlags = 0;
    }

private:
    inline Response(Request::TransactionType type, const shared_ptr<const NfObject> &object,
                    const Variant &name, const Variant& value, unsigned int flags, const Time &time = Time::mono())
        : mType(type), mObject(object), mName(name), mValue(value), mTime(time), mFlags(flags)
    {
        NRDP_OBJECTCOUNT_REF(NBPResponse, this);
    }

    Request::TransactionType mType;
    shared_ptr<const NfObject> mObject;
    Variant mName, mValue;
    Time mTime;
    unsigned int mFlags;
};
}

#endif
