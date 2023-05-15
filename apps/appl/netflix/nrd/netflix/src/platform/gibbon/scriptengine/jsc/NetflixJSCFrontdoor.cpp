/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "JSC.h"
#include GIBBON_JSC_API(JSContextRef.h)
#include GIBBON_JSC_API(JSStringRef.h)
#include GIBBON_JSC_API(JSValueRef.h)

#include <vector>
#include <stdint.h>

#include <nrdbase/utf8.h>
#include <nrdbase/Log.h>

typedef void* Netflix_JSPropertyNameArrayRef;

using namespace netflix;

extern "C" {

bool Netflix_JSValueIsArray(JSContextRef ctx, JSValueRef value)
{
    JSType valueType = JSValueGetType(ctx, value);
    if (valueType == kJSTypeObject) {
        static const JSStringRef checkScript = JSStringCreateWithUTF8CString("Array.isArray(this);");
        JSValueRef result = JSEvaluateScript(ctx, checkScript, JSValueToObject(ctx, value, 0), NULL, 0, 0);
        return JSValueToBoolean(ctx, result);
    }
    return false;
}

JSStringRef Netflix_JSStringCreate(const char *text, int length)
{
    try {
        int utf16_length = 0;
        std::vector<JSChar> utf16(length);
        const char* end = text + length;
        while(text < end) {
            const uint32_t c = utf8::unchecked::next(text, end);
            if (c == 0)
                continue;
            utf16[utf16_length++] = c & 0xFFFF;
        }
        return JSStringCreateWithCharacters(&utf16[0], utf16_length);
    } catch(...) {
        std::vector<JSChar>buffer(length);
        JSChar *p = buffer.data();
        for (int i = 0; i < length; ++i)
            p[i] = text[i];
        return JSStringCreateWithCharacters(p, length);
    }
	return JSStringCreateWithUTF8CString("");
}

Netflix_JSPropertyNameArrayRef Netflix_JSObjectPropertyNames(JSContextRef ctx, JSObjectRef object)
{
    return JSObjectCopyPropertyNames(ctx, object);
}

Netflix_JSPropertyNameArrayRef Netflix_JSPropertyNameArrayRetain(Netflix_JSPropertyNameArrayRef array)
{
    JSPropertyNameArrayRef a = static_cast<JSPropertyNameArrayRef>(array);
    return JSPropertyNameArrayRetain(a);
}

void Netflix_JSPropertyNameArrayRelease(JSContextRef, Netflix_JSPropertyNameArrayRef array)
{
    JSPropertyNameArrayRef a = static_cast<JSPropertyNameArrayRef>(array);
    JSPropertyNameArrayRelease(a);
}

size_t Netflix_JSPropertyNameArrayGetCount(Netflix_JSPropertyNameArrayRef array)
{
    JSPropertyNameArrayRef a = static_cast<JSPropertyNameArrayRef>(array);
    return JSPropertyNameArrayGetCount(a);
}

const JSChar *Netflix_JSPropertyNameArrayGetNameAtIndex(JSContextRef, Netflix_JSPropertyNameArrayRef array, size_t index, size_t *size)
{
    JSPropertyNameArrayRef a = static_cast<JSPropertyNameArrayRef>(array);
    JSStringRef name = JSPropertyNameArrayGetNameAtIndex(a, index);
    if(size)
        *size = JSStringGetLength(name);
    return JSStringGetCharactersPtr(name);
}

JSValueRef Netflix_JSObjectGetProperty(JSContextRef ctx, JSObjectRef object, Netflix_JSPropertyNameArrayRef array, size_t index, JSValueRef* exception)
{
    JSPropertyNameArrayRef a = static_cast<JSPropertyNameArrayRef>(array);
    JSStringRef name = JSPropertyNameArrayGetNameAtIndex(a, index);
    return JSObjectGetProperty(ctx, object, name, exception);
}

size_t Netflix_GarbageCollect(JSContextRef ctx)
{
    JSGarbageCollect(ctx);
    return 0;
}

JSContextGroupRef Netflix_JSContextGroupCreate()
{
    return JSContextGroupCreate();
}

JSGlobalContextRef Netflix_JSGlobalContextCreateInGroup(JSContextGroupRef group)
{
    return JSGlobalContextCreateInGroup(group, NULL);
}

void Netflix_JSFunctionCode(JSContextRef ctx, JSObjectRef object, JSStringRef* str)
{
    JSStringRef functionSource = JSValueToStringCopy(ctx, object, 0);
    const JSChar *sourceString = JSStringGetCharactersPtr(functionSource);
    const size_t sourceSize = JSStringGetLength(functionSource);
    *str = JSStringCreateWithCharacters(sourceString, sourceSize);
    JSStringRelease(functionSource);
}

const JSChar* Netflix_JSValueGetString(JSContextRef ctx, JSValueRef value, size_t* size)
{
    const JSStringRef jsString = JSValueToStringCopy(ctx, value, NULL);
    if(size)
        *size = JSStringGetLength(jsString);
    return JSStringGetCharactersPtr(jsString);
}


size_t Netflix_JSValueGetUTF8CString(const JSChar* chars, size_t size, char* buffer, size_t bufferSize)
{
    if(!bufferSize)
        return 0;

	JSStringRef jsString = JSStringCreateWithCharacters(chars, size);
	const size_t result = JSStringGetUTF8CString(jsString, buffer, bufferSize);
	JSStringRelease(jsString);
    return result;
}
}
