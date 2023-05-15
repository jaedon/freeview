/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ArrayBufferJSC_h
#define ArrayBufferJSC_h

#include "JSC.h"
#include GIBBON_JSC_API(JSObjectRef.h)
#include <nrdbase/DataBuffer.h>

class Pipe;
namespace netflix {
namespace gibbon {
namespace JSC {

class ArrayBufferJSC : public PrivateBase
{
public:
    enum { Type = Type_ArrayBuffer };
    ArrayBufferJSC(const DataBuffer &buffer = DataBuffer());
    ~ArrayBufferJSC();

    static void allocate(int bytes);
    static void finalize(JSObjectRef object);
    static JSObjectRef construct(JSContextRef ctx, JSObjectRef constructor, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSObjectRef construct(JSContextRef ctx, const DataBuffer &buffer);
    static bool hasInstance(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);
    static void getPropertyNames(JSContextRef ctx, JSObjectRef object, JSPropertyNameAccumulatorRef propertyNames);

    // Functions
    static JSValueRef slice(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);

    // Values
    static JSValueRef byteLength(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception);
    static JSValueRef data(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception);
    static JSValueRef base64(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception);
    static JSValueRef printable(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception);
    static JSValueRef constructor(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception);

    void ref(JSContextRef ctx);
    void deref();

    DataBuffer mDataBuffer;
    JSObjectRef mObject;
    int mRefCount;
};

class ArrayBufferViewJSC : public PrivateBase
{
public:
    enum { Type = -1 };
    ArrayBufferViewJSC(unsigned int type);
    ~ArrayBufferViewJSC();

    void clear();

    // Values
    static JSValueRef byteOffset(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception);
    static JSValueRef byteLength(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception);

    ArrayBufferJSC *buffer() const;
    DataBuffer dataBuffer() const;

    unsigned long mByteOffset;
    unsigned long mByteLength;
    JSObjectRef mArrayBuffer;
};

class TypedArrayJSC : public ArrayBufferViewJSC
{
public:
    enum { Type = Type_TypedArray };
    TypedArrayJSC(ScriptEngine::TypedArrayType type);
    ~TypedArrayJSC();
    static void finalize(JSObjectRef object);
    inline static JSObjectRef constructInt8(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    inline static JSObjectRef constructUint8(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    inline static JSObjectRef constructUint8Clamped(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    inline static JSObjectRef constructInt16(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    inline static JSObjectRef constructUint16(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    inline static JSObjectRef constructInt32(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    inline static JSObjectRef constructUint32(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    inline static JSObjectRef constructFloat32(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    inline static JSObjectRef constructFloat64(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);

    inline static bool hasInstanceInt8(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);
    inline static bool hasInstanceUint8(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);
    inline static bool hasInstanceUint8Clamped(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);
    inline static bool hasInstanceInt16(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);
    inline static bool hasInstanceUint16(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);
    inline static bool hasInstanceInt32(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);
    inline static bool hasInstanceUint32(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);
    inline static bool hasInstanceFloat32(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);
    inline static bool hasInstanceFloat64(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);

    static JSValueRef getProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception);
    static bool setProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef *exception);
    static bool hasProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName);
    static void getPropertyNames(JSContextRef ctx, JSObjectRef object, JSPropertyNameAccumulatorRef propertyNames);

    // Functions
    static JSValueRef set(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef subarray(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);

    // Values
    JSValueRef getAtIndex(JSContextRef ctx, size_t index, JSValueRef *exception) const;
    JSValueRef length(JSContextRef ctx, JSValueRef *exception) const;
    JSValueRef data(JSContextRef ctx, JSValueRef *exception) const;
    JSValueRef buffer(JSContextRef ctx, JSValueRef *exception) const;
    JSValueRef BYTES_PER_ELEMENT(JSContextRef ctx, JSValueRef *exception) const;
    JSValueRef constructor(JSContextRef ctx, JSValueRef *exception) const;
    JSValueRef byteLength(JSContextRef ctx, JSValueRef *exception) const;
    JSValueRef byteOffset(JSContextRef ctx, JSValueRef *exception) const;

    using ArrayBufferViewJSC::buffer;
    Pipe *pipe();
    const Pipe *pipe() const;
    int itemSize() const { return (mArrayType & ScriptEngine::Size_Mask) / 8; }

    ScriptEngine::TypedArrayType mArrayType;

    static JSObjectRef construct(ScriptEngine::TypedArrayType arrayType, JSContextRef ctx, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSObjectRef construct(ScriptEngine::TypedArrayType arrayType, JSContextRef ctx, const DataBuffer &buffer);
    static bool hasInstance(ScriptEngine::TypedArrayType arrayType, JSContextRef ctx, JSValueRef possibleInstance, JSValueRef* exception);
};


class DataViewJSC : public ArrayBufferViewJSC
{
public:
    enum { Type = Type_DataView };
    DataViewJSC();
    ~DataViewJSC();
    static void finalize(JSObjectRef object);
    static JSObjectRef construct(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static bool hasInstance(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exception);
    static void getPropertyNames(JSContextRef ctx, JSObjectRef object, JSPropertyNameAccumulatorRef propertyNames);

    static JSValueRef buffer(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception);
    using ArrayBufferViewJSC::buffer;

    // Functions
    static JSValueRef getInt8(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef getUInt8(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef getInt16(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef getUInt16(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef getInt32(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef getUInt32(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef getFloat32(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef getFloat64(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);

    static JSValueRef setInt8(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef setUInt8(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef setInt16(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef setUInt16(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef setInt32(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef setUInt32(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef setFloat32(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
    static JSValueRef setFloat64(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception);
};

inline JSObjectRef TypedArrayJSC::constructInt8(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return construct(ScriptEngine::Type_Int8, ctx, argumentCount, arguments, exception);
}
inline JSObjectRef TypedArrayJSC::constructUint8(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return construct(ScriptEngine::Type_Uint8, ctx, argumentCount, arguments, exception);
}
inline JSObjectRef TypedArrayJSC::constructUint8Clamped(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return construct(ScriptEngine::Type_Uint8Clamped, ctx, argumentCount, arguments, exception);
}
inline JSObjectRef TypedArrayJSC::constructInt16(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return construct(ScriptEngine::Type_Int16, ctx, argumentCount, arguments, exception);
}
inline JSObjectRef TypedArrayJSC::constructUint16(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return construct(ScriptEngine::Type_Uint16, ctx, argumentCount, arguments, exception);
}
inline JSObjectRef TypedArrayJSC::constructInt32(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return construct(ScriptEngine::Type_Int32, ctx, argumentCount, arguments, exception);
}
inline JSObjectRef TypedArrayJSC::constructUint32(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return construct(ScriptEngine::Type_Uint32, ctx, argumentCount, arguments, exception);
}
inline JSObjectRef TypedArrayJSC::constructFloat32(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return construct(ScriptEngine::Type_Float32, ctx, argumentCount, arguments, exception);
}
inline JSObjectRef TypedArrayJSC::constructFloat64(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return construct(ScriptEngine::Type_Float64, ctx, argumentCount, arguments, exception);
}

inline bool TypedArrayJSC::hasInstanceInt8(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    return hasInstance(ScriptEngine::Type_Int8, ctx, possibleInstance, exception);
}
inline bool TypedArrayJSC::hasInstanceUint8(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    return hasInstance(ScriptEngine::Type_Uint8, ctx, possibleInstance, exception);
}
inline bool TypedArrayJSC::hasInstanceUint8Clamped(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    return hasInstance(ScriptEngine::Type_Uint8Clamped, ctx, possibleInstance, exception);
}
inline bool TypedArrayJSC::hasInstanceInt16(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    return hasInstance(ScriptEngine::Type_Int16, ctx, possibleInstance, exception);
}
inline bool TypedArrayJSC::hasInstanceUint16(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    return hasInstance(ScriptEngine::Type_Uint16, ctx, possibleInstance, exception);
}
inline bool TypedArrayJSC::hasInstanceInt32(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    return hasInstance(ScriptEngine::Type_Int32, ctx, possibleInstance, exception);
}
inline bool TypedArrayJSC::hasInstanceUint32(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    return hasInstance(ScriptEngine::Type_Uint32, ctx, possibleInstance, exception);
}
inline bool TypedArrayJSC::hasInstanceFloat32(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    return hasInstance(ScriptEngine::Type_Float32, ctx, possibleInstance, exception);
}
inline bool TypedArrayJSC::hasInstanceFloat64(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    return hasInstance(ScriptEngine::Type_Float64, ctx, possibleInstance, exception);
}

}}}

#endif
