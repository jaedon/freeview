/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "JSC.h"
#include "XMLParserJSC.h"
#include GIBBON_JSC_API(JSObjectRef.h)
#include GIBBON_JSC_API(JSStringRef.h)

extern "C" {
JSStringRef Netflix_JSStringCreate(const char* string, int length);
inline JSStringRef Netflix_JSStringConcat(JSStringRef left, JSStringRef right)
{
    const int rightLength = JSStringGetLength(right);
    if (!rightLength)
        return left;
    const int currentLength = JSStringGetLength(left);
    if (!currentLength)
        return right;

    std::vector<JSChar> buffer(currentLength + rightLength);
    memcpy(&buffer[0], JSStringGetCharactersPtr(left), sizeof(JSChar) * currentLength);
    memcpy(&buffer[0] + currentLength, JSStringGetCharactersPtr(right), sizeof(JSChar) * rightLength);
    JSStringRef ref = JSStringCreateWithCharacters(&buffer[0], rightLength + currentLength);
    return ref;
}
};

using namespace netflix;
using namespace netflix::gibbon::JSC;

XMLParserJSC::XMLParserJSC(JSContextRef context)
    : mContext(context), mLength(Netflix_JSStringCreate("length", 6))
{}

XMLParserJSC::JSCText::JSCText(JSContextRef ctx, const char *str, int length)
    : Value(Text), context(ctx), text(Netflix_JSStringCreate(str, length))
{}

XMLParserJSC::JSCObject::JSCObject(XMLParserJSC *p, JSContextRef ctx)
    : Value(Object), context(ctx), object(JSObjectMake(ctx, 0, 0)), parser(p)
{}

void XMLParserJSC::JSCObject::set(const char *name, Mode mode, const shared_ptr<Value> &value)
{
    ScopedString key(Netflix_JSStringCreate(name, strlen(name)));
    switch (mode) {
    case Replace:
        if (value->type == Text) {
            JSObjectSetProperty(context, object, key,
                                JSValueMakeString(context, reinterpret_cast<JSCText*>(value.get())->text),
                                kJSPropertyAttributeNone, 0);
        } else {
            JSValueUnprotect(context, reinterpret_cast<JSCObject*>(value.get())->object);
            JSObjectSetProperty(context, object, key,
                                reinterpret_cast<JSCObject*>(value.get())->object,
                                kJSPropertyAttributeNone, 0);
        }
        break;
    case Append: {
        assert(value->type == Text);
        JSStringRef text = reinterpret_cast<JSCText*>(value.get())->text;
        JSValueRef cur = JSObjectGetProperty(context, object, key, 0);
        if (!cur || JSValueIsUndefined(context, cur)) {
            JSObjectSetProperty(context, object, key, JSValueMakeString(context, text),
                                kJSPropertyAttributeNone, 0);
        } else {
            ScopedString curString(JSValueToStringCopy(context, cur, 0));
            ScopedString concat(Netflix_JSStringConcat(curString, text));
            JSObjectSetProperty(context, object, key, JSValueMakeString(context, concat),
                                kJSPropertyAttributeNone, 0);
        }
        break; }
    case Array: {
        JSValueRef cur = JSObjectGetProperty(context, object, key, 0);
        JSValueRef val;
        if (value->type == Text) {
            val = JSValueMakeString(context, reinterpret_cast<JSCText*>(value.get())->text);
        } else {
            val = reinterpret_cast<JSCObject*>(value.get())->object;
            JSValueUnprotect(context, val);
        }

        if (cur && !JSValueIsUndefined(context, cur)) {
            assert(JSValueIsObject(context, cur));
            JSObjectRef array = JSValueToObject(context, cur, 0);
            JSValueRef ref = JSObjectGetProperty(context, array, parser->mLength, 0);
            unsigned int count = (unsigned int)JSValueToNumber(context, ref, 0);
            JSObjectSetPropertyAtIndex(context, array, count, val, 0);
        } else {
            JSObjectRef array = JSObjectMakeArray(context, 1, &val, 0);
            JSObjectSetProperty(context, object, key, array, kJSPropertyAttributeNone, 0);
        }
        break; }
    }
}

shared_ptr<XMLParser::Value> XMLParserJSC::createText(const char *string, int length)
{
    return shared_ptr<Value>(new JSCText(mContext, string, length));
}

shared_ptr<XMLParser::Value> XMLParserJSC::createObject()
{
    shared_ptr<JSCObject> ret(new JSCObject(this, mContext));
    JSValueProtect(mContext, ret->object);
    return ret;
}
