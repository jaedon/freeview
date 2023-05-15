/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "JSC.h"
#include "JSONParserJSC.h"
#include GIBBON_JSC_API(JSStringRef.h)
#include GIBBON_JSC_API(JSObjectRef.h)

extern "C" {
JSStringRef Netflix_JSStringCreate(const char* string, int length);
};

using namespace netflix;
using namespace netflix::gibbon;
using namespace netflix::gibbon::JSC;

JSONParserJSC::JSONParserJSC(JSContextRef context)
    : mContext(context)
{}


JSONParserJSC::Base::Base(JSValueRef val, JSContextRef ctx)
    : value(val), unprotect(true), context(ctx)
{
    assert(context);
    if (value)
        JSValueProtect(context, value);
}

JSONParserJSC::Base::~Base()
{
    if (unprotect && value) {
        JSValueUnprotect(context, value);
    }
}

static inline JSONParserJSC::Base *cast(JSONParser::Value *value)
{
    if (value) {
        // we can't cast directly to a Base * because of multiple inheritance
        switch (value->type) {
        case JSONParser::Value::Type_Object:
            return static_cast<JSONParserJSC::JSCObject*>(value);
        case JSONParser::Value::Type_Array:
            return static_cast<JSONParserJSC::JSCArray*>(value);
        case JSONParser::Value::Type_Null:
        case JSONParser::Value::Type_Integer:
        case JSONParser::Value::Type_Double:
        case JSONParser::Value::Type_Boolean:
            return static_cast<JSONParserJSC::JSCValue*>(value);
        case JSONParser::Value::Type_String:
            return static_cast<JSONParserJSC::JSCString*>(value);
        }
    }
    return 0;
}

JSONParserJSC::JSCValue::JSCValue(Type type, JSValueRef val, JSContextRef ctx)
    : Value(type), Base(val, ctx)
{
}

JSValueRef JSONParserJSC::parse(const char *data, int size)
{
    Base *base = cast(JSONParser::parse(data, size));
    if (base) {
        JSValueRef ref = base->value;
        base->unprotect = false;
        // this one should stay protected
        delete base;
        return ref;
    }
    return JSValueMakeUndefined(mContext);
}

JSONParserJSC::JSCString::JSCString(Mode m, JSContextRef ctx)
    : Base(0, ctx), length(0), mode(m)
{}

void JSONParserJSC::JSCString::addData(const char *str, int len)
{
    pieces.push_back(Piece(str, len, true));
    length += len;
}

void JSONParserJSC::JSCString::addRange(const char *str, int len)
{
    pieces.push_back(Piece(str, len, false));
    length += len;
}

void JSONParserJSC::JSCString::finalize(const char *str, int len)
{
    length += len;
    if (pieces.empty()) {
        string.assign(Netflix_JSStringCreate(str, len));
    } else {
        char staticBuf[1024];
        char *buf;
        if (length >= sizeof(buf)) {
            buf = new char[length + 1];
        } else {
            buf = staticBuf;
        }

        int pos = 0;

        for (std::vector<Piece>::const_iterator it = pieces.begin(); it != pieces.end(); ++it) {
            if (it->owned) {
                memcpy(buf + pos, it->owned, it->len);
                free(it->owned);
            } else {
                memcpy(buf + pos, it->str, it->len);
            }
            pos += it->len;
        }
        if (len)
            memcpy(buf + pos, str, len);
        buf[length] = '\0';
        string.assign(Netflix_JSStringCreate(buf, length));
        if (buf != staticBuf)
            delete[] buf;
    }
    assert(string);
    if (mode == Mode_Value) {
        assert(!value);
        value = JSValueMakeString(context, string);
        assert(value);
        JSValueProtect(context, value);
    }
}

JSONParserJSC::JSCString::Piece::Piece(const char *s, int l, bool o)
    : str(o ? 0 : s), owned(o ? strndup(s, l) : 0), len(l)
{
}

JSONParserJSC::JSCObject::JSCObject(JSObjectRef obj, JSContextRef ctx)
    : Base(obj, ctx)
{
}

bool JSONParserJSC::JSCObject::setValue(String *key, Value *val)
{
    assert(val);
    JSStringRef k = static_cast<JSCString*>(key)->string;
    JSValueRef v = cast(val)->value;
    assert(k);
    assert(v);
    assert(value);
    JSObjectRef object = JSValueToObject(context, value, 0);
    JSObjectSetProperty(context, object, k, v, kJSPropertyAttributeNone, 0);
    return true;
}

JSONParserJSC::JSCArray::JSCArray(JSContextRef ctx)
    : Base(0, ctx)
{}

void JSONParserJSC::JSCArray::append(Value *value)
{
    assert(value);
    Base *base = cast(value);
    base->unprotect = false;
    // we need to keep these values protected until we finalize the array
    values.push_back(base->value);
}

void JSONParserJSC::JSCArray::finalize()
{
    JSObjectRef array = JSObjectMakeArray(context, values.size(), &values[0], 0);
    value = array;
    JSValueProtect(context, array);
    for (std::vector<JSValueRef>::const_iterator it = values.begin(); it != values.end(); ++it) {
        // these were protected in append so we need to unprotect them here
        JSValueUnprotect(context, *it);
    }
}

JSONParserJSC::Object *JSONParserJSC::createObject()
{
    return new JSCObject(JSObjectMake(mContext, 0, 0), mContext);
}

JSONParserJSC::Array *JSONParserJSC::createArray()
{
    return new JSCArray(mContext);
}

JSONParserJSC::String *JSONParserJSC::createStringKey()
{
    return new JSCString(JSCString::Mode_Key, mContext);
}

JSONParserJSC::String *JSONParserJSC::createStringValue()
{
    return new JSCString(JSCString::Mode_Value, mContext);
}

JSONParserJSC::Value *JSONParserJSC::create(double val)
{
    return new JSCValue(Value::Type_Double, JSValueMakeNumber(mContext, val), mContext);
}

JSONParserJSC::Value *JSONParserJSC::create(llong val)
{
    return new JSCValue(Value::Type_Integer, JSValueMakeNumber(mContext, val), mContext);
}

JSONParserJSC::Value *JSONParserJSC::create(bool val)
{
    return new JSCValue(Value::Type_Boolean, JSValueMakeBoolean(mContext, val), mContext);
}

JSONParserJSC::Value *JSONParserJSC::createNull()
{
    return new JSCValue(Value::Type_Null, JSValueMakeUndefined(mContext), mContext);
}
