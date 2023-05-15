/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "JSONParserV8.h"
#include <limits.h>

using namespace netflix;
using namespace netflix::gibbon;

JSONParserV8::Object *JSONParserV8::createObject()
{
    return new V8Object;
}

JSONParserV8::Array *JSONParserV8::createArray()
{
    return new V8Array;
}

JSONParserV8::String *JSONParserV8::createStringKey()
{
    return new V8String;
}

JSONParserV8::String *JSONParserV8::createStringValue()
{
    return new V8String;
}

JSONParserV8::Value *JSONParserV8::create(double val)
{
    return new V8Value(Value::Type_Double, v8::Number::New(v8::Isolate::GetCurrent(), val));
}

JSONParserV8::Value *JSONParserV8::create(bool val)
{
    return new V8Value(Value::Type_Boolean, v8::Boolean::New(v8::Isolate::GetCurrent(), val));
}

JSONParserV8::Value *JSONParserV8::create(llong val)
{
    if (val >= INT_MIN && val <= INT_MAX)
        return new V8Value(Value::Type_Integer, v8::Int32::New(v8::Isolate::GetCurrent(), val));
    return new V8Value(Value::Type_Integer, v8::Number::New(v8::Isolate::GetCurrent(), val));
}

JSONParserV8::Value *JSONParserV8::createNull()
{
    return new V8Value(Value::Type_Null, v8::Undefined(v8::Isolate::GetCurrent()));
}
