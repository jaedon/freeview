/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "JSC.h"
#include "ArrayBufferJSC.h"
#include "GibbonApplication.h"
#include "GibbonConfiguration.h"
#include GIBBON_JSC_API(JSObjectRef.h)
#include GIBBON_JSC_API(JSStringRef.h)
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
# include GIBBON_JSC_API(JSBasePrivate.h)
#endif
#include <nrd/AppLog.h>
#include <nrdbase/Endian.h>
#include <limits.h>

using namespace netflix;
using namespace netflix::gibbon;
using namespace netflix::gibbon::JSC;

struct CountBase : public netflix::ObjectCount::Record
{
    CountBase(const std::string &name)
        : netflix::ObjectCount::Record(name)
    {}

    virtual void begin()
    {
#ifdef NRDP_HAS_OBJECTCOUNT
        DataBuffer::objectCountMutex().lock();
#endif
    }
    virtual void end()
    {
#ifdef NRDP_HAS_OBJECTCOUNT
        DataBuffer::objectCountMutex().unlock();
#endif
    }
};
struct ArrayBufferJSCCount : public CountBase
{
public:
    inline ArrayBufferJSCCount(const std::string &n)
        : CountBase(n)
    {}
    virtual size_t cost(void *object) const
    {
        return static_cast<ArrayBufferJSC*>(object)->mDataBuffer.capacity()+1;
    }
    virtual std::string describe(void *object) const
    {
        return static_cast<ArrayBufferJSC*>(object)->mDataBuffer.describe();
    }
};

struct ArrayBufferViewJSCCount : public CountBase
{
public:
    inline ArrayBufferViewJSCCount(const std::string &n)
        : CountBase(n)
    {}
    virtual std::string describe(void *object) const
    {
        return static_cast<ArrayBufferViewJSC*>(object)->dataBuffer().describe();
    }
};

NRDP_DEFINE_OBJECTCOUNT_RECORD(ArrayBufferJSCCount, ArrayBufferJSC);
NRDP_DEFINE_OBJECTCOUNT_RECORD(ArrayBufferViewJSCCount, TypedArrayJSC);
NRDP_DEFINE_OBJECTCOUNT_RECORD(ArrayBufferViewJSCCount, DataViewJSC);

// #define ARRAYBUFFER_VERBOSE
#ifdef ARRAYBUFFER_VERBOSE
#define VERBOSE_LOG(fmt, ...) NTRACE(TRACE_GIBBON_SCRIPT, fmt, ##__VA_ARGS__)
#else
#define VERBOSE_LOG(...)
#endif

extern "C" {
typedef void* Netflix_JSPropertyNameArrayRef;
bool Netflix_JSValueIsArray(JSContextRef ctx, JSValueRef value);
Netflix_JSPropertyNameArrayRef Netflix_JSObjectPropertyNames(JSContextRef ctx, JSObjectRef object);
void Netflix_JSPropertyNameArrayRelease(JSContextRef ctx, Netflix_JSPropertyNameArrayRef array);
size_t Netflix_JSPropertyNameArrayGetCount(Netflix_JSPropertyNameArrayRef array);
}

// ------------------------------- Pipe -------------------------------

class Pipe
{
public:
    Pipe(unsigned int f)
        : flags(f), srcData(0), srcLength(0), destinationData(0)
    {}
    virtual ~Pipe()
    {}

    virtual size_t count() const = 0;
    virtual size_t unitSize() const = 0;
    virtual double read(size_t index) const = 0;
    virtual void append(double val) = 0;

    enum Flag {
        None = 0x0,
        FloatingPoint = 0x1,
        Adjacent = 0x2
    };

    void send(Pipe *destination) const
    {
        const size_t size = unitSize();
        const size_t c = count();
        if (size == destination->unitSize() && flags == destination->flags) {
            memcpy(destination->destinationData, srcData, c * size);
        } else {
            for (size_t i=0; i<c; ++i) {
                destination->append(read(i));
            }
        }
    }

    const unsigned int flags;
    unsigned char const *srcData;
    size_t srcLength;
    unsigned char *destinationData;
};

// static inline double cast(double val) { return val; }
// static inline float cast(double val) { return static_cast<float>(val); }
// without this extra cast gcc misbehaves on certain casts
template <typename T> T cast(double val) { return static_cast<T>(static_cast<int64_t>(val)); }
template <> float cast(double val) { return static_cast<float>(val); }
template <> double cast(double val) { return val; }


class ArrayPipe : public Pipe
{
public:
    ArrayPipe(JSContextRef ctx, JSObjectRef a, size_t len)
        : Pipe(FloatingPoint), context(ctx), array(a), length(len), index(0)
    {}

    virtual size_t count() const
    {
        return length;
    }
    virtual size_t unitSize() const
    {
        return 0;
    }
    virtual double read(size_t index) const
    {
        JSValueRef val = JSObjectGetPropertyAtIndex(context, array, index, 0);
        double v = 0;
        if (val && JSValueIsNumber(context, val))
            v = JSValueToNumber(context, val, 0);
        return v;
    }

    virtual void append(double val)
    {
        assert(index < length);
        JSValueRef ref = JSValueMakeNumber(context, val);
        JSObjectSetPropertyAtIndex(context, array, index++, ref, 0);
    }

    JSContextRef context;
    JSObjectRef array;
    const size_t length;
    size_t index;
};

static inline JSObjectRef makeObject(JSContextRef ctx, JSClassRef classRef, JSObjectRef prototype, PrivateBase *priv)
{
    JSObjectRef ret = JSObjectMake(ctx, classRef, priv);
    JSObjectSetPrototype(ctx, ret, prototype);
    return ret;
}

static inline JSObjectRef makeTypedArray(JSContextRef ctx, ScriptEngine::TypedArrayType type, TypedArrayJSC *priv)
{
    shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine();
    return makeObject(ctx, engine->typedArrayClass(type), engine->typedArrayPrototype(type), priv);
}

static inline JSObjectRef makeArrayBuffer(JSContextRef ctx, ArrayBufferJSC *priv)
{
    shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine();
    return makeObject(ctx, engine->arrayBufferClass(), engine->arrayBufferPrototype(), priv);
}

static inline JSObjectRef makeDataView(JSContextRef ctx, DataViewJSC *priv)
{
    shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine();
    return makeObject(ctx, engine->dataViewClass(), engine->dataViewPrototype(), priv);
}

static Pipe *createPipe(ScriptEngine::TypedArrayType type);

template <typename T> T *get(JSObjectRef object)
{
    assert(object);
    T *t = reinterpret_cast<T*>(JSObjectGetPrivate(object));
    if (t && T::Type != -1 && t->type != static_cast<unsigned int>(T::Type))
        t = 0;
    return t;
}

// ------------------------------- ArrayBufferJSC -------------------------------

ArrayBufferJSC::ArrayBufferJSC(const DataBuffer &buffer)
    : PrivateBase(Type_ArrayBuffer), mDataBuffer(buffer), mObject(0), mRefCount(0)
{
    NRDP_OBJECTCOUNT_REF(ArrayBufferJSC, this);
    VERBOSE_LOG("ArrayBufferJSC(%p) created %d bytes\n", this, buffer.size());
}

ArrayBufferJSC::~ArrayBufferJSC()
{
    NRDP_OBJECTCOUNT_DEREF(ArrayBufferJSC, this);
    VERBOSE_LOG("ArrayBufferJSC(%p) destroyed %d bytes (%d ref)\n", this, mDataBuffer.size(), mRefCount);
}

void ArrayBufferJSC::allocate(int bytes)
{
    assert(ScriptEngine::currentContext());
#ifdef GIBBON_SCRIPT_JSC_BACKDOOR
    JSReportExtraMemoryCost(ScriptEngine::currentContext(), bytes);
#endif
}

void ArrayBufferJSC::finalize(JSObjectRef object)
{
    delete get<ArrayBufferJSC>(object);
}

bool ArrayBufferJSC::hasInstance(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (JSValueIsObject(ctx, possibleInstance)) {
        const PrivateBase *priv = reinterpret_cast<const PrivateBase*>(JSObjectGetPrivate(JSValueToObject(ctx, possibleInstance, exception)));
        return priv && priv->type == PrivateBase::Type_ArrayBuffer;
    }
    return false;
}

void ArrayBufferJSC::getPropertyNames(JSContextRef ctx, JSObjectRef, JSPropertyNameAccumulatorRef propertyNames)
{
    ScriptEngine::ContextScope scope(ctx);
    const char *properties[] = { "byteLength", 0 };
    for (int i=0; properties[i]; ++i) {
        ScopedString name(properties[i]);
        JSPropertyNameAccumulatorAddName(propertyNames, name);
    }
}

JSObjectRef ArrayBufferJSC::construct(JSContextRef ctx, JSObjectRef /*constructor*/, size_t argumentCount,
                                      const JSValueRef arguments[], JSValueRef *exception)
{
    ScriptEngine::ContextScope scope(ctx);
    DataBuffer dataBuffer;
    if (argumentCount) {
        if (!JSValueIsNumber(ctx, arguments[0])) {
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
            return 0; // ???
        }
        const unsigned long size = static_cast<unsigned long>(JSValueToNumber(ctx, arguments[0], exception));
        if (size > 20 * 1024 * 1024) { // 20 megs should be enough for everyone
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
            return 0; // ???
        }

        dataBuffer.resize(size);
        dataBuffer.fill('\0');
        ArrayBufferJSC::allocate(size + sizeof(ArrayBufferJSC));
    }
    NRDP_OBJECTCOUNT_DESCRIBE(dataBuffer, "ARRAYBUFFERJSC");

    ArrayBufferJSC *buf = new ArrayBufferJSC(dataBuffer);
    JSObjectRef ret = makeArrayBuffer(ctx, buf);
    buf->mObject = ret;
    return ret;
}

JSObjectRef ArrayBufferJSC::construct(JSContextRef ctx, const DataBuffer &buffer)
{
    ArrayBufferJSC *buf = new ArrayBufferJSC(buffer);
    JSObjectRef ret = makeArrayBuffer(ctx, buf);
    buf->mObject = ret;
    return ret;
}

JSValueRef ArrayBufferJSC::slice(JSContextRef ctx, JSObjectRef /*function*/, JSObjectRef thisObject, size_t argumentCount,
                                 const JSValueRef arguments[], JSValueRef *exception)
{
    ArrayBufferJSC *buf = get<ArrayBufferJSC>(thisObject);
    if (buf) {
        assert(buf->type == PrivateBase::Type_ArrayBuffer);
        if ((argumentCount != 1 && argumentCount != 2)
            || !JSValueIsNumber(ctx, arguments[0])
            || (argumentCount >= 2 && !JSValueIsNumber(ctx, arguments[1]))) {
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to slice"));
            return JSValueMakeUndefined(ctx);
        }
        const long long length = buf->mDataBuffer.length();
        long long start = (long long)JSValueToNumber(ctx, arguments[0], exception);
        long long end;
        if (start < 0)
            start = std::max(0ll, length + start);
        if (argumentCount >= 2) {
            end = (long long)JSValueToNumber(ctx, arguments[1], exception);
            if (end < 0)
                end = std::max(0ll, length + end);
        } else {
            end = length;
        }
        ArrayBufferJSC *b = new ArrayBufferJSC(buf->mDataBuffer.mid(start, end - start));
        b->mObject = makeArrayBuffer(ctx, b);
        return b->mObject;
    }
    return JSValueMakeUndefined(ctx);
}

JSValueRef ArrayBufferJSC::byteLength(JSContextRef ctx, JSObjectRef object,
                                      JSStringRef /*propertyName*/, JSValueRef*/*exception*/)
{
    ScriptEngine::ContextScope scope(ctx);
    ArrayBufferJSC *buf = get<ArrayBufferJSC>(object);
    if (buf) {
        assert(buf);
        assert(buf->type == PrivateBase::Type_ArrayBuffer);
        return JSValueMakeNumber(ctx, buf->mDataBuffer.size());
    }
    return JSValueMakeUndefined(ctx);
}

JSValueRef ArrayBufferJSC::data(JSContextRef ctx, JSObjectRef object,
                                JSStringRef /*propertyName*/, JSValueRef *exception)
{
    ScriptEngine::ContextScope scope(ctx);
    ArrayBufferJSC *buf = get<ArrayBufferJSC>(object);
    if (buf) {
        assert(buf->type == PrivateBase::Type_ArrayBuffer);
        const size_t count = buf->mDataBuffer.size();
        Pipe *src = createPipe(ScriptEngine::Type_Uint8);
        src->srcLength = count;
        src->srcData = buf->mDataBuffer.data();
        JSValueRef *values = new JSValueRef[count];
        memset(values, 0, sizeof(JSValueRef) * count);
        JSObjectRef array = JSObjectMakeArray(ctx, count, values, exception);
        delete[] values;
        JSValueProtect(ctx, array);
        ArrayPipe dest(ctx, array, count);
        src->send(&dest);
        delete src;
        JSValueUnprotect(ctx, array); // ### do I even need to do this?
        return array;
    }
    return JSValueMakeUndefined(ctx);
}

JSValueRef ArrayBufferJSC::base64(JSContextRef ctx, JSObjectRef object, JSStringRef, JSValueRef *)
{
    ScriptEngine::ContextScope scope(ctx);
    ArrayBufferJSC *buf = get<ArrayBufferJSC>(object);
    if (buf) {
        assert(buf);
        assert(buf->type == PrivateBase::Type_ArrayBuffer);

        const DataBuffer base64 = buf->mDataBuffer.toBase64();
        return stringToJSValue(ctx, base64.data<const char>(), base64.size());
    }
    return JSValueMakeUndefined(ctx);
}

JSValueRef ArrayBufferJSC::constructor(JSContextRef ctx, JSObjectRef, JSStringRef, JSValueRef *)
{
    ScriptEngine::ContextScope scope(ctx);
    shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine();
    return engine->arrayBufferPrototype();
}

JSValueRef ArrayBufferJSC::printable(JSContextRef ctx, JSObjectRef object, JSStringRef, JSValueRef *)
{
    ScriptEngine::ContextScope scope(ctx);
    ArrayBufferJSC *buf = get<ArrayBufferJSC>(object);
    if (buf) {
        assert(buf);
        assert(buf->type == PrivateBase::Type_ArrayBuffer);
        return buf->mDataBuffer.isBinary() ? 0 : stringToJSValue(ctx, buf->mDataBuffer.data<const char>(), buf->mDataBuffer.size());
    }
    return JSValueMakeUndefined(ctx);
}

void ArrayBufferJSC::ref(JSContextRef ctx)
{
    assert(mObject);
    VERBOSE_LOG("ArrayBufferJSC(%p) ref %d bytes (%d ref)\n", this, mDataBuffer.size(), mRefCount + 1);
    if (!mRefCount++) {
        assert(ctx);
        JSValueProtect(ctx, mObject);
    }
}

void ArrayBufferJSC::deref()
{
    assert(mObject);
    VERBOSE_LOG("ArrayBufferJSC(%p) deref %d bytes (%d ref)\n", this, mDataBuffer.size(), mRefCount - 1);

    if (!--mRefCount) {
        assert(ScriptEngine::currentContext());
        JSValueUnprotect(ScriptEngine::currentContext(), mObject);
    }
}

// ------------------------------- ArrayBufferView -------------------------------

ArrayBufferViewJSC::ArrayBufferViewJSC(unsigned int type)
    : PrivateBase(type), mByteOffset(0), mByteLength(0), mArrayBuffer(0)
{
    shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine();
    engine->addArrayBufferView(this);
}

ArrayBufferViewJSC::~ArrayBufferViewJSC()
{
    shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine();
    engine->removeArrayBufferView(this);
    clear();
}

void ArrayBufferViewJSC::clear()
{
    if (ArrayBufferJSC *buf = buffer())
        buf->deref();
    mArrayBuffer = 0;
}

JSValueRef ArrayBufferViewJSC::byteOffset(JSContextRef ctx, JSObjectRef object, JSStringRef, JSValueRef *)
{
    ScriptEngine::ContextScope scope(ctx);
    if (ArrayBufferViewJSC *view = get<ArrayBufferViewJSC>(object))
        return JSValueMakeNumber(ctx, view->mByteOffset);
    return JSValueMakeUndefined(ctx);
}

JSValueRef ArrayBufferViewJSC::byteLength(JSContextRef ctx, JSObjectRef object, JSStringRef, JSValueRef *)
{
    ScriptEngine::ContextScope scope(ctx);
    if (ArrayBufferViewJSC *view = get<ArrayBufferViewJSC>(object))
        return JSValueMakeNumber(ctx, view->mByteLength);
    return JSValueMakeUndefined(ctx);
}

ArrayBufferJSC *ArrayBufferViewJSC::buffer() const
{
    if (!mArrayBuffer)
        return 0;
    return get<ArrayBufferJSC>(mArrayBuffer);
}

DataBuffer ArrayBufferViewJSC::dataBuffer() const
{
    if (!mByteLength)
        return DataBuffer();
    if (const ArrayBufferJSC *buf = buffer()) {
        return buf->mDataBuffer.slice(mByteOffset, mByteLength);
    }
    return DataBuffer();
}

// ------------------------------- TypedArrayJSC -------------------------------

TypedArrayJSC::TypedArrayJSC(ScriptEngine::TypedArrayType type)
    : ArrayBufferViewJSC(Type_TypedArray), mArrayType(type)
{
    VERBOSE_LOG("TypedArrayJSC(%p) created 0x%0x\n", this, mArrayType);
    NRDP_OBJECTCOUNT_REF(TypedArrayJSC, this);
}

TypedArrayJSC::~TypedArrayJSC()
{
    NRDP_OBJECTCOUNT_DEREF(TypedArrayJSC, this);
    VERBOSE_LOG("TypedArrayJSC(%p) destroyed 0x%0x\n", this, mArrayType);
}

void TypedArrayJSC::finalize(JSObjectRef object)
{
    delete get<TypedArrayJSC>(object);
}

JSObjectRef TypedArrayJSC::construct(ScriptEngine::TypedArrayType arrayType, JSContextRef ctx, size_t argumentCount,
                                     const JSValueRef arguments[], JSValueRef *exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (!argumentCount)
        return construct(arrayType, ctx, DataBuffer());

    if (Netflix_JSValueIsArray(ctx, arguments[0])) {
        JSObjectRef array = JSValueToObject(ctx, arguments[0], exception);
        Netflix_JSPropertyNameArrayRef properties = Netflix_JSObjectPropertyNames(ctx, array);
        const size_t count = Netflix_JSPropertyNameArrayGetCount(properties);
        Netflix_JSPropertyNameArrayRelease(ctx, properties);
        ArrayPipe src(ctx, array, count);
        Pipe *dest = createPipe(arrayType);
        DataBuffer buffer(count * dest->unitSize());
        dest->destinationData = buffer.data();
        src.send(dest);
        delete dest;
        ArrayBufferJSC::allocate(buffer.size() + sizeof(TypedArrayJSC) + sizeof(ArrayBufferJSC));
        NRDP_OBJECTCOUNT_DESCRIBE(buffer, StringFormatter::sformat("TypedArrayJSC: 0x%x", arrayType));
        return construct(arrayType, ctx, buffer);
    } else if (JSValueIsNumber(ctx, arguments[0])) {
        if (argumentCount > 1) {
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
            return 0; // ???
        }
        unsigned long size = static_cast<unsigned long>(JSValueToNumber(ctx, arguments[0], exception));
        DataBuffer buf(size * ((arrayType & ScriptEngine::Size_Mask) / 8), '\0');
        NRDP_OBJECTCOUNT_DESCRIBE(buf, StringFormatter::sformat("TypedArrayJSC: 0x%x", arrayType));
        ArrayBufferJSC::allocate(buf.size() + sizeof(TypedArrayJSC) + sizeof(ArrayBufferJSC));
        return construct(arrayType, ctx, buf);
    } else if (JSValueIsObject(ctx, arguments[0])) {
        JSObjectRef object = JSValueToObject(ctx, arguments[0], exception);
        PrivateBase *base = object ? reinterpret_cast<PrivateBase*>(JSObjectGetPrivate(object)) : 0;
        const PrivateBase::Type privateType = (base
                                               ? static_cast<PrivateBase::Type>(base->type)
                                               : PrivateBase::Type_None);
        switch (privateType) {
        case PrivateBase::Type_ArrayBuffer: {
            unsigned long byteOffset = 0;
            unsigned long length;
            const unsigned long itemSize = (arrayType & ScriptEngine::Size_Mask) / 8;
            ArrayBufferJSC *buf = static_cast<ArrayBufferJSC*>(base);
            const unsigned long size = buf->mDataBuffer.size();
            if (argumentCount > 1) {
                if (!JSValueIsNumber(ctx, arguments[1])) {
                    *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
                    return 0; // ???
                }
                byteOffset = static_cast<unsigned long>(JSValueToNumber(ctx, arguments[1], exception));
                if (byteOffset > static_cast<unsigned long>(buf->mDataBuffer.size()) || (byteOffset % itemSize) != 0) {
                    // not sure why the third condition needs to be there but
                    // this is how both chrome and firefox behaves
                    *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
                    return 0; // ???
                }

                if (argumentCount > 2) {
                    if (!JSValueIsNumber(ctx, arguments[2])) {
                        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
                        return 0; // ???
                    }
                    length = static_cast<unsigned long>(JSValueToNumber(ctx, arguments[2], exception));
                    if (byteOffset + (length * itemSize) > static_cast<unsigned long>(buf->mDataBuffer.size())) {
                        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
                        return 0; // ???
                    }
                } else {
                    if (((buf->mDataBuffer.size() - byteOffset) % itemSize) != 0) {
                        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
                        return 0; // ???
                    }
                    length = (buf->mDataBuffer.size() - byteOffset) / itemSize;
                }
            } else {
                if ((size % itemSize) != 0) {
                    *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
                    return 0; // ???
                }
                length = size / itemSize;
            }
            TypedArrayJSC *typedArray = new TypedArrayJSC(arrayType);
            typedArray->mByteLength = length * itemSize;
            typedArray->mByteOffset = byteOffset;
            typedArray->mArrayBuffer = buf->mObject;
            buf->ref(ctx);
            return makeTypedArray(ctx, arrayType, typedArray); }
        case PrivateBase::Type_TypedArray: {
            const TypedArrayJSC *other = static_cast<const TypedArrayJSC*>(base);
            assert(other);
            const ArrayBufferJSC *otherBuf = get<const ArrayBufferJSC>(other->mArrayBuffer);
            DataBuffer copy;
            if (otherBuf) {
                if (other->mArrayType == arrayType) {
                    copy = otherBuf->mDataBuffer.mid(other->mByteOffset, other->mByteLength);
                } else {
                    const Pipe *src = other->pipe();
                    assert(src);
                    Pipe *dest = createPipe(arrayType);
                    assert(dest);
                    copy.resize(src->count() * dest->unitSize());
                    dest->destinationData = copy.data();
                    src->send(dest);
                    delete src;
                    delete dest;
                }
            }
            NRDP_OBJECTCOUNT_DESCRIBE(copy, StringFormatter::sformat("TypedArrayJSC: 0x%x", arrayType));
            return construct(arrayType, ctx, copy); }
        default:
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
            return 0; // ???
        }
    }
    *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
    return 0; // ???
}

JSObjectRef TypedArrayJSC::construct(ScriptEngine::TypedArrayType arrayType, JSContextRef ctx, const DataBuffer &buffer)
{
    ScriptEngine::ContextScope scope(ctx);
    TypedArrayJSC *typedArray = new TypedArrayJSC(arrayType);
    typedArray->mByteLength = buffer.size();
    ArrayBufferJSC *buf = new ArrayBufferJSC(buffer);
    buf->mObject = typedArray->mArrayBuffer = makeArrayBuffer(ctx, buf);
    buf->ref(ctx);
    return makeTypedArray(ctx, arrayType, typedArray);
}

bool TypedArrayJSC::hasInstance(ScriptEngine::TypedArrayType arrayType, JSContextRef ctx, JSValueRef possibleInstance, JSValueRef* exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (JSValueIsObject(ctx, possibleInstance)) {
        const PrivateBase *priv = reinterpret_cast<const PrivateBase*>(JSObjectGetPrivate(JSValueToObject(ctx, possibleInstance, exception)));
        if (priv && priv->type == PrivateBase::Type_TypedArray) {
            return static_cast<const TypedArrayJSC*>(priv)->mArrayType == arrayType;
        }
    }
    return false;
}


Pipe *TypedArrayJSC::pipe()
{
    if (ArrayBufferJSC *buf = buffer()) {
        Pipe *ret = createPipe(mArrayType);
        ret->srcLength = mByteLength;
        ret->srcData = buf->mDataBuffer.data() + mByteOffset;
        ret->destinationData = buf->mDataBuffer.data() + mByteOffset;
        return ret;
    }
    return 0;
}

const Pipe *TypedArrayJSC::pipe() const
{
    if (ArrayBufferJSC *buf = buffer()) {
        Pipe *ret = createPipe(mArrayType);
        ret->srcLength = mByteLength;
        ret->srcData = buf->mDataBuffer.data() + mByteOffset;
        return ret;
    }
    return 0;
}

JSValueRef TypedArrayJSC::length(JSContextRef ctx, JSValueRef*) const
{
    return JSValueMakeNumber(ctx, mByteLength / ((mArrayType & ScriptEngine::Size_Mask) / 8));
}

JSValueRef TypedArrayJSC::data(JSContextRef ctx, JSValueRef *exception) const
{
    if (buffer()) {
        assert(buffer()->type == PrivateBase::Type_ArrayBuffer);
        const Pipe *src = pipe();
        const size_t count = mByteLength / src->unitSize();
        JSValueRef *values = new JSValueRef[count];
        memset(values, 0, sizeof(JSValueRef) * count);
        JSObjectRef array = JSObjectMakeArray(ctx, count, values, exception);
        delete[] values;
        JSValueProtect(ctx, array);
        ArrayPipe dest(ctx, array, count);
        src->send(&dest);
        delete src;
        JSValueUnprotect(ctx, array);
        return array;
    }
    return JSValueMakeUndefined(ctx);
}

JSValueRef TypedArrayJSC::buffer(JSContextRef ctx, JSValueRef*) const
{
    return mArrayBuffer ? mArrayBuffer : JSValueMakeUndefined(ctx);
}

JSValueRef TypedArrayJSC::BYTES_PER_ELEMENT(JSContextRef ctx, JSValueRef *) const
{
    return JSValueMakeNumber(ctx, (mArrayType & ScriptEngine::Size_Mask) / 8);
}

JSValueRef TypedArrayJSC::constructor(JSContextRef, JSValueRef *) const
{
    shared_ptr<ScriptEngine> engine = GibbonApplication::instance()->getScriptEngine();
    return engine->typedArrayPrototype(mArrayType);
}

JSValueRef TypedArrayJSC::byteLength(JSContextRef ctx, JSValueRef *) const
{
    return JSValueMakeNumber(ctx, mByteLength);
}

JSValueRef TypedArrayJSC::byteOffset(JSContextRef ctx, JSValueRef *) const
{
    return JSValueMakeNumber(ctx, mByteOffset);
}

JSValueRef TypedArrayJSC::getAtIndex(JSContextRef ctx, size_t index, JSValueRef */*exception*/) const
{
    if (const ArrayBufferJSC *buf = buffer()) {
        assert(buf);
        assert(buf->type == PrivateBase::Type_ArrayBuffer);
        shared_ptr<Pipe> pipe(createPipe(mArrayType));
        pipe->srcLength = mByteLength;
        pipe->srcData = buf->mDataBuffer.data() + mByteOffset;
        if (index < pipe->count())
            return JSValueMakeNumber(ctx, pipe->read(index));
    }
    return JSValueMakeUndefined(ctx);
}

JSValueRef TypedArrayJSC::set(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    ScriptEngine::ContextScope scope(ctx);
    const TypedArrayJSC *that = get<const TypedArrayJSC>(thisObject);
    if (!that || !argumentCount || !JSValueIsObject(ctx, arguments[0])) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to set 1"));
        return JSValueMakeUndefined(ctx);
    }

    ArrayBufferJSC *buffer = that->buffer();
    if (!buffer)
        return JSValueMakeUndefined(ctx);

    Pipe *src = 0;
    if (Netflix_JSValueIsArray(ctx, arguments[0])) {
        JSObjectRef array = JSValueToObject(ctx, arguments[0], exception);
        Netflix_JSPropertyNameArrayRef properties = Netflix_JSObjectPropertyNames(ctx, array);
        const size_t count = Netflix_JSPropertyNameArrayGetCount(properties);
        Netflix_JSPropertyNameArrayRelease(ctx, properties);
        src = new ArrayPipe(ctx, array, count);
    } else if (JSValueIsObject(ctx, arguments[0])) {
        JSObjectRef object = JSValueToObject(ctx, arguments[0], exception);
        assert(object);
        TypedArrayJSC *array = get<TypedArrayJSC>(object);
        if (!array || array->type != PrivateBase::Type_TypedArray) {
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to set 2"));
            return JSValueMakeUndefined(ctx);
        }
        ArrayBufferJSC *buf = array->buffer();
        if (!buf || !array->mByteLength)
            return JSValueMakeUndefined(ctx);
        src = createPipe(array->mArrayType);
        assert(src);
        src->srcLength = array->mByteLength;
        src->srcData = buf->mDataBuffer.data() + array->mByteOffset;
    }
    size_t offset = 0;
    if (argumentCount >= 2) {
        if (!JSValueIsNumber(ctx, arguments[1])) {
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to set 3"));
            return JSValueMakeUndefined(ctx);
        }
        offset = (size_t)JSValueToNumber(ctx, arguments[1], exception);
    }
    Pipe *dest = createPipe(that->mArrayType);
    dest->destinationData = buffer->mDataBuffer.data() + offset;
    if (offset + src->count() > (that->mByteLength / dest->unitSize())) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to set 4"));
    } else {
        src->send(dest);
    }
    delete src;
    delete dest;
    return JSValueMakeUndefined(ctx);
}

JSValueRef TypedArrayJSC::subarray(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount,
                                   const JSValueRef arguments[], JSValueRef *exception)
{
    ScriptEngine::ContextScope scope(ctx);
    const TypedArrayJSC *that = get<const TypedArrayJSC>(thisObject);
    if (!that
        || (argumentCount > 0 && !JSValueIsNumber(ctx, arguments[0]))
        || (argumentCount > 1 && !JSValueIsNumber(ctx, arguments[1]))) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to subarray"));
        return JSValueMakeUndefined(ctx);
    }

    ArrayBufferJSC *buffer = that->buffer();
    if (!buffer)
        return construct(that->mArrayType, ctx, 0, 0, exception);

    const long long itemSize = (that->mArrayType & ScriptEngine::Size_Mask) / 8;
    const long long count = that->mByteLength / itemSize;
    long long start = argumentCount > 0 ? (long long)JSValueToNumber(ctx, arguments[0], exception) : 0;
    long long end;
    if (start < 0)
        start = std::max(0ll, count + start);
    if (argumentCount >= 2) {
        end = (long long)JSValueToNumber(ctx, arguments[1], exception);
        if (end < 0)
            end = std::max(0ll, count + end);
    } else {
        end = count;
    }

    TypedArrayJSC *array = new TypedArrayJSC(that->mArrayType);
    array->mByteLength = (end - start) * itemSize;
    array->mByteOffset = (start + that->mByteOffset) * itemSize;
    array->mArrayBuffer = buffer->mObject;
    buffer->ref(ctx);
    return makeTypedArray(ctx, that->mArrayType, array);
}

static inline int decodeNumber(const JSChar *chars, int len)
{
    if (chars[0] >= '0' && chars[0] <= '9') {
        int idx = 0;
        int multipler = 1;
        for (int i=len - 1; i>=0; --i) {
            int digit = chars[i] - '0';
            assert(digit >= 0 && digit <= 9);
            digit *= multipler;
            multipler *= 10;
            idx += digit;
        }

        return idx;
    }
    return -1;
}
enum Property {
    Invalid,
    Buffer,
    ByteLength,
    ByteOffset,
    BytesPerElement,
    Constructor,
    Data,
    Length
};

static inline Property decodeProperty(const JSChar *chars, int len)
{
    switch (len) {
    case 17: {
        const JSChar BYTES_PER_ELEMENT_data[] = {
            0x42, 0x59, 0x54, 0x45, 0x53, 0x5f, 0x50, 0x45, 0x52, 0x5f, 0x45, 0x4c, 0x45, 0x4d, 0x45, 0x4e, 0x54
        };

        if (!memcmp(BYTES_PER_ELEMENT_data, chars, len * sizeof(JSChar)))
            return BytesPerElement;
        break; }
    case 11: {
        const JSChar constructor_data[] = {
            0x63, 0x6f, 0x6e, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x6f, 0x72
        };
        if (!memcmp(constructor_data, chars, len * sizeof(JSChar))) {
            return Constructor;
        }
        break; }
    case 10: {
        const JSChar byteOffset_data[] = { 0x62, 0x79, 0x74, 0x65, 0x4f, 0x66, 0x66, 0x73, 0x65, 0x74 };
        const JSChar byteLength_data[] = { 0x62, 0x79, 0x74, 0x65, 0x4c, 0x65, 0x6e, 0x67, 0x74, 0x68 };
        if (!memcmp(byteOffset_data, chars, len * sizeof(JSChar))) {
            return ByteOffset;
        } else if (!memcmp(byteLength_data, chars, len * sizeof(JSChar))) {
            return ByteLength;
        }
        break; }
    case 6: {
        const JSChar buffer_data[] = { 0x62, 0x75, 0x66, 0x66, 0x65, 0x72 };
        const JSChar length_data[] = { 0x6c, 0x65, 0x6e, 0x67, 0x74, 0x68 };
        if (!memcmp(buffer_data, chars, len * sizeof(JSChar))) {
            return Buffer;
        } else if (!memcmp(length_data, chars, len * sizeof(JSChar))) {
            return Length;
        }
        break; }
    case 4:
        const JSChar data_data[] = { 0x64, 0x61, 0x74, 0x61 };
        if (!memcmp(data_data, chars, len * sizeof(JSChar)))
            return Data;
        break;
    }
    return Invalid;
}

JSValueRef TypedArrayJSC::getProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception)
{
    ScriptEngine::ContextScope scope(ctx);
    // printf("getProperty(%s)\n", JSC::toStdString(propertyName).c_str());
    const TypedArrayJSC *that = get<const TypedArrayJSC>(object);
    if (that) {
        assert(that);
        const size_t len = JSStringGetLength(propertyName);
        const JSChar *chars = JSStringGetCharactersPtr(propertyName);
        assert(len > 0);
        assert(chars);
        const int idx = decodeNumber(chars, len);
        if (idx != -1)
            return that->getAtIndex(ctx, idx, exception);

        switch (decodeProperty(chars, len)) {
        case BytesPerElement: return that->BYTES_PER_ELEMENT(ctx, exception);
        case Length: return that->length(ctx, exception);
        case ByteLength: return that->byteLength(ctx, exception);
        case ByteOffset: return that->byteOffset(ctx, exception);
        case Data: return that->data(ctx, exception);
        case Buffer: return that->buffer(ctx, exception);
        case Constructor: return that->constructor(ctx, exception);
        case Invalid: break;
        }
    }
    return JSValueMakeUndefined(ctx);
}

bool TypedArrayJSC::setProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef *exception)
{
    ScriptEngine::ContextScope scope(ctx);
    TypedArrayJSC *that = get<TypedArrayJSC>(object);
    if (!that)
        return false;

    const size_t len = JSStringGetLength(propertyName);
    const JSChar *chars = JSStringGetCharactersPtr(propertyName);
    assert(len > 0);
    assert(chars);
    const int idx = decodeNumber(chars, len);
    if (idx == -1)
        return false;

    if (!JSValueIsNumber(ctx, value))
        return false;

    if (ArrayBufferJSC *buf = that->buffer()) {
        assert(buf);
        assert(buf->type == PrivateBase::Type_ArrayBuffer);
        shared_ptr<Pipe> pipe(that->pipe());
        pipe->srcLength = that->mByteLength;
        assert(pipe);
        if (static_cast<size_t>(idx) < pipe->count()) {
            pipe->destinationData = buf->mDataBuffer.data() + that->mByteOffset + (pipe->unitSize() * idx);
            pipe->append(JSValueToNumber(ctx, value, exception));
            return true;
        }
    }
    return false;
}

bool TypedArrayJSC::hasProperty(JSContextRef, JSObjectRef, JSStringRef propertyName)
{
    const size_t len = JSStringGetLength(propertyName);
    const JSChar *chars = JSStringGetCharactersPtr(propertyName);
    assert(len > 0);
    assert(chars);
    const int idx = decodeNumber(chars, len);
    if (idx != -1)
        return true;
    const Property prop = decodeProperty(chars, len);
    return prop != Invalid && prop != BytesPerElement;
    // return (idx != -1 || decodeProperty(chars, len) != Invalid && );
}

void TypedArrayJSC::getPropertyNames(JSContextRef ctx, JSObjectRef object, JSPropertyNameAccumulatorRef propertyNames)
{
    ScriptEngine::ContextScope scope(ctx);
    // ### we don't want to enumerate constructor, we only need this to return true
    // var foo = new Uint8Array;
    // if (foo.constructor == Uint8Array) ...
    const char *properties[] = { "BYTES_PER_ELEMENT", "length", "byteLength", "byteOffset", 0 };
    for (int i=0; properties[i]; ++i) {
        ScopedString name(properties[i]);
        JSPropertyNameAccumulatorAddName(propertyNames, name);
    }
    if (TypedArrayJSC *typedArray = get<TypedArrayJSC>(object)) {
        if (typedArray->buffer()) {
            ScopedString name("buffer");
            JSPropertyNameAccumulatorAddName(propertyNames, name);
        }
        char buf[16];
        const int count = typedArray->mByteLength / typedArray->itemSize();
        for (int i=0; i<count; ++i) {
            snprintf(buf, sizeof(buf), "%d", i);
            ScopedString name(buf);
            JSPropertyNameAccumulatorAddName(propertyNames, name);
        }
    }
}

// ------------------------------- Pipe -------------------------------

DataViewJSC::DataViewJSC()
    : ArrayBufferViewJSC(Type)
{
    VERBOSE_LOG("DataViewJSC(%p) created\n", this);
    NRDP_OBJECTCOUNT_REF(DataViewJSC, this);
}

DataViewJSC::~DataViewJSC()
{
    VERBOSE_LOG("DataViewJSC(%p) destroyed\n", this);
    NRDP_OBJECTCOUNT_DEREF(DataViewJSC, this);
}

void DataViewJSC::finalize(JSObjectRef object)
{
    delete get<DataViewJSC>(object);
}

JSObjectRef DataViewJSC::construct(JSContextRef ctx, JSObjectRef, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (!argumentCount)
        return makeDataView(ctx, new DataViewJSC);

    if (!JSValueIsObject(ctx, arguments[0])) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
        return 0; // ???
    }

    JSObjectRef object = JSValueToObject(ctx, arguments[0], exception);
    ArrayBufferJSC *buf = object ? reinterpret_cast<ArrayBufferJSC*>(JSObjectGetPrivate(object)) : 0;
    if (!buf || buf->type != ArrayBufferJSC::Type) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
        return 0; // ???
    }

    unsigned long byteOffset = 0;
    unsigned long byteLength = ULONG_MAX;
    if (argumentCount > 1) {
        if (!JSValueIsNumber(ctx, arguments[1])) {
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
            return 0; // ???
        }
        byteOffset = static_cast<unsigned long>(JSValueToNumber(ctx, arguments[1], exception));
        if (byteOffset > static_cast<unsigned long long>(buf->mDataBuffer.size())) {
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
            return 0; // ???
        }

        if (argumentCount > 2) {
            if (!JSValueIsNumber(ctx, arguments[2])) {
                *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
                return 0; // ???
            }
            byteLength = static_cast<unsigned long>(JSValueToNumber(ctx, arguments[2], exception));
            if (byteOffset + byteLength > static_cast<unsigned long long>(buf->mDataBuffer.size())) {
                *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to constructor"));
                return 0; // ???
            }
        }
    }
    if (byteLength == ULONG_MAX)
        byteLength = (buf->mDataBuffer.size() - byteOffset);

    DataViewJSC *dataView = new DataViewJSC;
    dataView->mByteLength = byteLength;
    dataView->mByteOffset = byteOffset;
    dataView->mArrayBuffer = buf->mObject;
    buf->ref(ctx);
    return makeDataView(ctx, dataView);
}

bool DataViewJSC::hasInstance(JSContextRef ctx, JSObjectRef, JSValueRef possibleInstance, JSValueRef* exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (JSValueIsObject(ctx, possibleInstance)) {
        const PrivateBase *priv = reinterpret_cast<const PrivateBase*>(JSObjectGetPrivate(JSValueToObject(ctx, possibleInstance, exception)));
        return priv && priv->type == PrivateBase::Type_DataView;
    }
    return false;
}

void DataViewJSC::getPropertyNames(JSContextRef ctx, JSObjectRef object, JSPropertyNameAccumulatorRef propertyNames)
{
    ScriptEngine::ContextScope scope(ctx);
    const char *properties[] = { "byteLength", "byteOffset", 0 };

    for (int i=0; properties[i]; ++i) {
        ScopedString name(properties[i]);
        JSPropertyNameAccumulatorAddName(propertyNames, name);
    }
    if (DataViewJSC *view = get<DataViewJSC>(object)) {
        if (view->buffer()) {
            ScopedString name("buffer");
            JSPropertyNameAccumulatorAddName(propertyNames, name);
        }
    }
}

JSValueRef DataViewJSC::buffer(JSContextRef ctx, JSObjectRef object, JSStringRef, JSValueRef *)
{
    ScriptEngine::ContextScope scope(ctx);
    const DataViewJSC *view = get<DataViewJSC>(object);
    if (view && view->mArrayBuffer)
        return view->mArrayBuffer;
    return JSValueMakeUndefined(ctx);
}


// ### is this right for floats?
template <typename T> inline void flipBytes(T &t)
{
    unsigned char *data = reinterpret_cast<unsigned char*>(&t);
    for (size_t i=0; i<sizeof(T)/2; ++i) {
        std::swap(data[i], data[sizeof(T) - 1 - i]);
    }
}

template <typename T>
inline void toLittle(T &val)
{
#ifdef NRDP_BIG_ENDIAN
    flipBytes(val);
#endif
    (void)val;
}

static inline void toBig(int8_t &) {}
static inline void toBig(uint8_t &) {}
static inline void toLittle(int8_t &) {}
static inline void toLittle(uint8_t &) {}
static inline void toBig(uint16_t &val)
{
#ifdef NRDP_LITTLE_ENDIAN
    val = htons(val);
#endif
    (void)val;
}

static inline void toBig(int16_t &val)
{
#ifdef NRDP_LITTLE_ENDIAN
    val = htons(val);
#endif
    (void)val;
}

static inline void toBig(uint32_t &val)
{
#ifdef NRDP_LITTLE_ENDIAN
    val = htonl(val);
#endif
    (void)val;
}

static inline void toBig(int32_t &val)
{
#ifdef NRDP_LITTLE_ENDIAN
    val = htonl(val);
#endif
    (void)val;

}

static inline void toBig(float &val)
{
#ifdef NRDP_LITTLE_ENDIAN
    flipBytes(val);
#endif
    (void)val;
}

static inline void toBig(double &val)
{
#ifdef NRDP_LITTLE_ENDIAN
    flipBytes(val);
#endif
    (void)val;
}


static inline JSValueRef makeNumber(JSContextRef ctx, uint8_t val, bool) { return JSValueMakeNumber(ctx, val); }
static inline JSValueRef makeNumber(JSContextRef ctx, int8_t val, bool) { return JSValueMakeNumber(ctx, val); }
static inline JSValueRef makeNumber(JSContextRef ctx, uint16_t val, bool bigEndian) { if (bigEndian) toBig(val); else toLittle(val); return JSValueMakeNumber(ctx, val); }
static inline JSValueRef makeNumber(JSContextRef ctx, int16_t val, bool bigEndian) { if (bigEndian) toBig(val); else toLittle(val); return JSValueMakeNumber(ctx, val); }
static inline JSValueRef makeNumber(JSContextRef ctx, uint32_t val, bool bigEndian) { if (bigEndian) toBig(val); else toLittle(val); return JSValueMakeNumber(ctx, val); }
static inline JSValueRef makeNumber(JSContextRef ctx, int32_t val, bool bigEndian) { if (bigEndian) toBig(val); else toLittle(val); return JSValueMakeNumber(ctx, val); }
static inline JSValueRef makeNumber(JSContextRef ctx, float val, bool bigEndian) { if (bigEndian) toBig(val); else toLittle(val); return JSValueMakeNumber(ctx, val); }
static inline JSValueRef makeNumber(JSContextRef ctx, double val, bool bigEndian) { if (bigEndian) toBig(val); else toLittle(val); return JSValueMakeNumber(ctx, val); }

template <typename T>
static JSValueRef get(JSContextRef ctx, JSObjectRef thisObject, size_t argumentCount,
                      const JSValueRef *arguments, JSValueRef *exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (!argumentCount || !JSValueIsNumber(ctx, arguments[0])) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to get"));
        return JSValueMakeUndefined(ctx);
    }
    DataViewJSC *view = get<DataViewJSC>(thisObject);
    if (!view) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to get"));
        return JSValueMakeUndefined(ctx);
    }
    unsigned long offset = static_cast<unsigned long>(JSValueToNumber(ctx, arguments[0], exception));
    offset += view->mByteOffset;
    if (offset + sizeof(T) > view->mByteOffset + view->mByteLength) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to get"));
        return JSValueMakeUndefined(ctx);
    }
    ArrayBufferJSC *buf = view->buffer();
    if (!buf) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to get"));
        return JSValueMakeUndefined(ctx);
    }

    assert(static_cast<int>(offset + sizeof(T)) <= buf->mDataBuffer.size());

    bool bigEndian = true;
    if (argumentCount > 1) {
        if (JSValueIsBoolean(ctx, arguments[1])) {
            bigEndian = !JSValueToBoolean(ctx, arguments[1]);
        } else if (!JSValueIsUndefined(ctx, arguments[1])) {
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to get"));
            return JSValueMakeUndefined(ctx);
        }
    }

    T t;
    memcpy(&t, buf->mDataBuffer.constData() + offset, sizeof(T));
    return makeNumber(ctx, t, bigEndian);
}

JSValueRef DataViewJSC::getInt8(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return get<int8_t>(ctx, thisObject, argumentCount, arguments, exception);
}

JSValueRef DataViewJSC::getUInt8(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return get<uint8_t>(ctx, thisObject, argumentCount, arguments, exception);
}

JSValueRef DataViewJSC::getInt16(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return get<int16_t>(ctx, thisObject, argumentCount, arguments, exception);
}

JSValueRef DataViewJSC::getUInt16(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return get<uint16_t>(ctx, thisObject, argumentCount, arguments, exception);
}

JSValueRef DataViewJSC::getInt32(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return get<int32_t>(ctx, thisObject, argumentCount, arguments, exception);
}

JSValueRef DataViewJSC::getUInt32(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return get<uint32_t>(ctx, thisObject, argumentCount, arguments, exception);
}

JSValueRef DataViewJSC::getFloat32(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return get<float>(ctx, thisObject, argumentCount, arguments, exception);
}

JSValueRef DataViewJSC::getFloat64(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    return get<double>(ctx, thisObject, argumentCount, arguments, exception);
}

template <typename T>
static void set(JSContextRef ctx, JSObjectRef thisObject, size_t argumentCount,
                const JSValueRef *arguments, JSValueRef *exception)
{
    ScriptEngine::ContextScope scope(ctx);
    if (argumentCount < 2 || !JSValueIsNumber(ctx, arguments[0]) || !JSValueIsNumber(ctx, arguments[1])) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to set t1"));
        return;
    }
    DataViewJSC *view = get<DataViewJSC>(thisObject);
    if (!view) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to set t2"));
        return;
    }
    unsigned long offset = static_cast<int>(JSValueToNumber(ctx, arguments[0], exception));
    offset += view->mByteOffset;
    if (offset + sizeof(T) > view->mByteOffset + view->mByteLength) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to set t3"));
        return;
    }
    ArrayBufferJSC *buf = view->buffer();
    if (!buf) {
        *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to set t4"));
        return;
    }

    assert(static_cast<int>(offset + sizeof(T)) <= buf->mDataBuffer.size());

    bool bigEndian = true;
    if (argumentCount > 2) {
        if (JSValueIsBoolean(ctx, arguments[2])) {
            bigEndian = !JSValueToBoolean(ctx, arguments[2]);
        } else if (!JSValueIsUndefined(ctx, arguments[2])) {
            *exception = JSC::stringToJSValue(ctx, std::string("Invalid arguments to set t5"));
            return;
        }
    }
    T val = cast<T>(JSValueToNumber(ctx, arguments[1], exception));
    if (bigEndian) {
        toBig(val);
    } else {
        toLittle(val);
    }
    memcpy(buf->mDataBuffer.data() + offset, &val, sizeof(T));
}

JSValueRef DataViewJSC::setInt8(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    set<int8_t>(ctx, thisObject, argumentCount, arguments, exception);
    return JSValueMakeUndefined(ctx);
}

JSValueRef DataViewJSC::setUInt8(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    set<uint8_t>(ctx, thisObject, argumentCount, arguments, exception);
    return JSValueMakeUndefined(ctx);
}

JSValueRef DataViewJSC::setInt16(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    set<int16_t>(ctx, thisObject, argumentCount, arguments, exception);
    return JSValueMakeUndefined(ctx);
}

JSValueRef DataViewJSC::setUInt16(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    set<uint16_t>(ctx, thisObject, argumentCount, arguments, exception);
    return JSValueMakeUndefined(ctx);
}

JSValueRef DataViewJSC::setInt32(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    set<int32_t>(ctx, thisObject, argumentCount, arguments, exception);
    return JSValueMakeUndefined(ctx);
}

JSValueRef DataViewJSC::setUInt32(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    set<uint32_t>(ctx, thisObject, argumentCount, arguments, exception);
    return JSValueMakeUndefined(ctx);
}

JSValueRef DataViewJSC::setFloat32(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    set<float>(ctx, thisObject, argumentCount, arguments, exception);
    return JSValueMakeUndefined(ctx);
}

JSValueRef DataViewJSC::setFloat64(JSContextRef ctx, JSObjectRef, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef *exception)
{
    set<double>(ctx, thisObject, argumentCount, arguments, exception);
    return JSValueMakeUndefined(ctx);
}

// ------------------------------- PipeSpecializations -------------------------------

template <typename T>
class PipeSpecialization : public Pipe
{
public:
    PipeSpecialization(unsigned int flags)
        : Pipe(flags)
    {}
    virtual size_t count() const
    {
        return srcLength / unitSize();
    }

    virtual size_t unitSize() const
    {
        return sizeof(T);
    }

    virtual double read(size_t index) const
    {
        assert(srcLength);
        assert(srcData);
        return reinterpret_cast<const T*>(srcData)[index];
    }

    virtual void append(double val)
    {
        *reinterpret_cast<T*>(destinationData) = cast<T>(val);
        destinationData += sizeof(T);
    }
};

class Uint8ClampedPipe : public PipeSpecialization<uint8_t>
{
public:
    Uint8ClampedPipe()
        : PipeSpecialization<uint8_t>(Pipe::Adjacent)
    {}
    virtual void append(double val)
    {
        uint8_t v;
        if (val < 0) {
            v = 0;
        } else if (val > 255) {
            v = 255;
        } else {
            v = static_cast<uint8_t>(val);
        }
        uint8_t *t = reinterpret_cast<uint8_t*>(destinationData);
        *t++ = v;
        destinationData = t;
    }
};

static Pipe *createPipe(ScriptEngine::TypedArrayType type)
{
    switch (type) {
    case ScriptEngine::Type_Int8: return new PipeSpecialization<int8_t>(Pipe::Adjacent);
    case ScriptEngine::Type_Uint8: return new PipeSpecialization<uint8_t>(Pipe::Adjacent);
    case ScriptEngine::Type_Uint8Clamped: return new Uint8ClampedPipe;
    case ScriptEngine::Type_Int16: return new PipeSpecialization<int16_t>(Pipe::Adjacent);
    case ScriptEngine::Type_Uint16: return new PipeSpecialization<uint16_t>(Pipe::Adjacent);
    case ScriptEngine::Type_Int32: return new PipeSpecialization<int32_t>(Pipe::Adjacent);
    case ScriptEngine::Type_Uint32: return new PipeSpecialization<uint32_t>(Pipe::Adjacent);
    case ScriptEngine::Type_Float32: return new PipeSpecialization<float>(Pipe::Adjacent|Pipe::FloatingPoint);
    case ScriptEngine::Type_Float64: return new PipeSpecialization<double>(Pipe::Adjacent|Pipe::FloatingPoint);
    default: break;
    }
    return 0;
}
