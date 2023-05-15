/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef Variant_h
#define Variant_h

#include <stdio.h>
#include <assert.h>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <nrdbase/StdInt.h>
#include <stdio.h>
#include <nrdbase/tr1.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/RefCounted.h>
#include <nrdbase/ObjectCount.h>

namespace netflix {

class VariantStringStream
{
public:
    VariantStringStream(std::string *string)
        : mString(string)
    {
    }

    VariantStringStream(const VariantStringStream &other)
        : mString(other.mString)
    {
    }

    ~VariantStringStream()
    {
        if (mString) {
            (*mString) += mStream.str();
        }
    }

    template <typename T>
    VariantStringStream &operator<<(const T &t)
    {
        if (mString)
            mStream << t;
        return *this;
    }
private:
    mutable std::string *mString;
    std::ostringstream mStream;
};

class JsonValue;
class Variant
{
public:
    class CustomData
    {
    public:
        CustomData(int t) : type(t) {}
        virtual ~CustomData() {}
        virtual Variant toVariant() const { return Variant(); }
        // this should return a Variant with type != Type_Custom
        const int type;
    };
    enum Type {
        Type_Null,
        Type_String,
        Type_Array,
        Type_IntegerMap,
        Type_StringMap,
        Type_Integer,
        Type_Double,
        Type_Boolean,
        Type_DataBuffer,
        Type_Custom,
        Type_Pointer
    };

    typedef std::map<std::string, Variant>::const_iterator StringMapIterator;
    typedef std::vector<Variant>::const_iterator ArrayIterator;

    Variant();
    explicit Variant(Type type);
    Variant(const char *str);
    Variant(const std::string &str);
    Variant(const DataBuffer &dataBuffer);
    Variant(const std::string &key, const Variant &value);
    Variant(int key, const Variant &value);
    Variant(const std::vector<Variant> &vec);
    Variant(const std::map<std::string, Variant> &map);
    Variant(const std::map<int, Variant> &map);
    Variant(const shared_ptr<CustomData> &custom);
    Variant(bool b);

    Variant(int8_t val);
    Variant(int16_t val);
    Variant(int32_t val);
    Variant(long val);
    Variant(long long val);
    Variant(uint8_t val);
    Variant(uint16_t val);
    Variant(uint32_t val);
    Variant(unsigned long val);
    Variant(unsigned long long val);
    // Javascript can't represent 64-bit ints. Do not use the (u)llong
    // constructors for values that will be parsed in parsed in Javascript.  Use
    // double instead, or represent the data as a string.

    Variant(double val);
    Variant(void *ptr);
    Variant(const void *ptr);
    Variant(const Variant &other);
    ~Variant();

    template<typename T>
    Variant(const std::vector<T>& vec);

    template<typename A, typename B>
    Variant(const std::pair<A, B>& pair);

    template<typename T>
    Variant(const std::map<std::string, T>& map);

    template<typename T>
    Variant(const std::map<int, T>& map);

    static Variant fromQueryString(const std::string &queryString, bool decode=true);
    static Variant fromJSON(const char *string, int length, bool *ok = 0, int *used = 0);
    static Variant fromJSON(const DataBuffer &dataBuffer, bool *ok = 0, int *used = 0)
    {
        return fromJSON(dataBuffer.c_str(), dataBuffer.size(), ok, used);
    }
    static inline Variant fromJSON(const std::string &json, bool *ok = 0, int *used = 0)
    {
        return fromJSON(json.c_str(), json.size(), ok, used);
    }

    static Variant fromXML(const std::string &xml, bool *ok = 0, int *used = 0) { return fromXML(xml.c_str(), xml.size(), ok, used); }
    static Variant fromXML(const DataBuffer &xml, bool *ok = 0, int *used = 0) { return fromXML(xml.c_str(), xml.size(), ok, used); }
    static Variant fromXML(const char *data, int length, bool *ok = 0, int *used = 0);

    static Variant fromQueryString(const char *queryString, bool decode=true) {
        std::string str = queryString ? std::string(queryString) : std::string();
        return fromQueryString(str, decode);
    }
    std::string toJSON(bool pretty=false) const;

    bool isNull() const;
    bool isString() const;
    bool isDataBuffer() const;
    bool isArray() const;
    bool isStringMap() const;
    bool isIntegerMap() const;
    inline bool isNumber() const { return isInteger() || isDouble(); }
    bool isInteger() const;
    bool isDouble() const;
    bool isBoolean() const;
    bool isCustom() const;
    bool isPointer() const;

    template <typename T> T mapValue(const std::string &key, bool *okPtr = 0, const T &defaultValue = T()) const;
    template <typename T> T mapValue(int key, bool *okPtr = 0, const T &defaultValue = T()) const;
    template <typename T> T value(bool *ok = 0, const T &defaultValue = T()) const;
    Variant mapValue(const std::string &key, bool *okPtr = 0, const Variant &defaultValue = Variant()) const;
    template <typename T> static Variant::Type variantType();

    Variant convert(Type targetType) const;
    bool canConvert(Type targetType) const;
    Variant &operator=(const Variant &other);
    bool operator==(const Variant &other) const;
    bool operator!=(const Variant &other) const;
    inline void clear();
    Type type() const;

    bool boolean() const;
    llong integer() const;
    double dbl() const;
    void *pointer() const;
    shared_ptr<const CustomData> custom() const;
    std::string string() const;
    DataBuffer dataBuffer() const;
    std::vector<Variant> array() const;
    std::map<std::string, Variant> stringMap() const;
    std::map<int, Variant> integerMap() const;
    static inline const Variant &null();

    const char *c_str() const;

    VariantStringStream stringStream();

    inline Variant &operator[](const std::string &);
    inline const Variant &operator[](const std::string &) const;
    inline const Variant &at(const std::string &key) const { return operator[](key); }
    inline Variant &operator[](int);
    inline void resizeArray(int size);
    inline const Variant &operator[](int) const;
    inline const Variant &at(int idx) const { return operator[](idx); }
    inline bool contains(const std::string &key) const;
    inline bool contains(int key) const;
    inline void remove(const std::string &key);
    inline void remove(int index);
    inline StringMapIterator find(const std::string &name) const;
    inline StringMapIterator stringMapBegin() const;
    inline StringMapIterator stringMapEnd() const;

    inline ArrayIterator arrayBegin() const;
    inline ArrayIterator arrayEnd() const;

    // Array operations

    inline void push_back(const Variant &variant);
    inline Variant& back();
    inline Variant const& back() const;
    inline int size() const;
    inline int length() const { return size(); }
    inline bool empty() const;
    inline int insert(int pos, const Variant &value);
    inline void insert(const std::string &key, const Variant &value) { operator[](key) = value; }

    template<typename InputIterator>
    void insert(int pos, InputIterator first, InputIterator last);

    template<typename InputIterator>
    inline void extend(InputIterator first, InputIterator last);

    Variant value(int idx, bool *ok = 0, const Variant &defaultValue = Variant()) const;
    Variant value(const std::string &key, bool *ok = 0, const Variant &defaultValue = Variant()) const;

    // These are used to initialize variant maps and arrays inline, like this:
    // Given a function "foo" that takes a variant:
    // foo(Variant::StringMap("a", true)("b", Variant::Array(3.14)("hello")));

    class StringMap
    {
    public:
        StringMap(const std::string &key, const Variant &value = Variant()) { mMap[key] = value; }
        StringMap & operator()(const std::string &key, const Variant &value = Variant()) { mMap[key] = value; return *this; }
        const std::map<std::string, Variant> &map() const { return mMap; }
    private:
        std::map<std::string, Variant> mMap;
    };

    class Array
    {
    public:
        Array(const Variant & value) { mArray.push_back(value); }
        Array &operator()(const Variant &value) { mArray.push_back(value); return *this; }
        const std::vector<Variant> &array() const { return mArray; }
    private:
        std::vector<Variant> mArray;
    };

    Variant(const Variant::StringMap &map);
    Variant(const Variant::Array &array);

    void merge(const Variant &other);

    void detach();
    int refCount() const;
private:
    template <typename T> inline void variantValue(T *t) const { invalidVariantType(t); }
    static void jsonConstruct(const JsonValue &value, Variant &variant);

    Type mType;

    union {
        llong integer;
        double dbl;
        bool boolean;
        RefCountedBase *refCounted;
        void *pointer;
        char customDataBuffer[sizeof(shared_ptr<CustomData>)];
    } mData;
    template <typename T> const T *ptr() const { return static_cast<RefCounted<T> *>(mData.refCounted)->ptr(); }
    template <typename T> T *ptr() { return static_cast<RefCounted<T> *>(mData.refCounted)->ptr(); }
    template <typename T>
    void detach()
    {
        assert(refCount() >= 1);
        mData.refCounted = static_cast<RefCounted<T> *>(mData.refCounted)->detach();
    }

    const std::string *stringPtr() const { return ptr<std::string>(); }
    std::string *stringPtr() { return ptr<std::string>(); }
    const std::vector<Variant> *arrayPtr() const { return ptr<std::vector<Variant> >(); }
    std::vector<Variant> *arrayPtr() { return ptr<std::vector<Variant> >(); }
    const std::map<std::string, Variant> *stringMapPtr() const { return ptr<std::map<std::string, Variant> >(); }
    std::map<std::string, Variant> *stringMapPtr() { return ptr<std::map<std::string, Variant> >(); }
    const std::map<int, Variant> *integerMapPtr() const { return ptr<std::map<int, Variant> >(); }
    std::map<int, Variant> *integerMapPtr() { return ptr<std::map<int, Variant> >(); }
    const shared_ptr<CustomData> *customPtr() const { return reinterpret_cast<const shared_ptr<CustomData> *>(mData.customDataBuffer); }
    shared_ptr<CustomData> *customPtr() { return reinterpret_cast<shared_ptr<CustomData> *>(mData.customDataBuffer); }
    const DataBuffer *dataBufferPtr() const { return ptr<DataBuffer>(); }
    const DataBuffer *dataBufferPtr() { return ptr<DataBuffer>(); }

    static const std::map<std::string, Variant> sNullStringMap;
    static const std::vector<Variant> sNullArray;
};

template <> inline void Variant::variantValue<int8_t>(int8_t *val) const { *val = static_cast<int8_t>(mData.integer); }
template <> inline void Variant::variantValue<int16_t>(int16_t *val) const { *val = static_cast<int16_t>(mData.integer); }
template <> inline void Variant::variantValue<int32_t>(int32_t *val) const { *val = static_cast<int32_t>(mData.integer); }
template <> inline void Variant::variantValue<llong>(llong *val) const { *val = mData.integer; }
template <> inline void Variant::variantValue<uint8_t>(uint8_t *val) const { *val = static_cast<uint8_t>(mData.integer); }
template <> inline void Variant::variantValue<uint16_t>(uint16_t *val) const { *val = static_cast<uint16_t>(mData.integer); }
template <> inline void Variant::variantValue<uint32_t>(uint32_t *val) const { *val = static_cast<uint32_t>(mData.integer); }
template <> inline void Variant::variantValue<ullong>(ullong *val) const { *val = static_cast<ullong>(mData.integer); }
template <> inline void Variant::variantValue<void *>(void **val) const { *val = static_cast<void*>(mData.pointer); }
template <> inline void Variant::variantValue<bool>(bool *val) const { *val = mData.boolean; }
template <> inline void Variant::variantValue<float>(float *val) const { *val = static_cast<float>(mData.dbl); }
template <> inline void Variant::variantValue<double>(double *val) const { *val = mData.dbl; }
template <> inline void Variant::variantValue<std::vector<Variant> >(std::vector<Variant> *val) const { *val = *arrayPtr(); }
template <> inline void Variant::variantValue<std::string>(std::string *val) const { *val = *stringPtr(); }
template <> inline void Variant::variantValue<std::map<std::string, Variant> >(std::map<std::string, Variant> *val) const { *val = *stringMapPtr(); }
template <> inline void Variant::variantValue<DataBuffer>(DataBuffer *val) const { *val = *dataBufferPtr(); }

// #define VARIANT_STATS_ENABLED
#ifdef VARIANT_STATS_ENABLED
inline void CONSTRUCT(int type)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&mutex);
    static int *counts = 0;
    if (!counts) {
        counts = new int[Variant::Type_Pointer + 1];
        memset(counts, 0, sizeof(int) * (Variant::Type_Pointer + 1));
    }
    ++counts[type];
    static int count = 0;
    if (++count % 1000 == 0) {
        const char *names[] = {
            "Null",
            "String",
            "Array",
            "IntegerMap",
            "StringMap",
            "Integer",
            "Double",
            "Boolean",
            "DataBuffer",
            "Custom",
            "Pointer"
        };

        for (int i=0; i<=Variant::Type_Pointer; ++i) {
            printf("Created %s: %d\n", names[i], counts[i]);
        }
        printf("Total created: %d\n", count);
    }
    pthread_mutex_unlock(&mutex);
}
#else
#define CONSTRUCT(type) do {} while (false)
#endif

inline Variant::Variant()
    : mType(Type_Null)
{
    CONSTRUCT(mType);
}

inline Variant::Variant(Type type)
    : mType(type)
{
    CONSTRUCT(mType);
    switch (mType) {
    case Type_Null: break;
    case Type_String: mData.refCounted = new RefCounted<std::string>(Type_String); break;
    case Type_DataBuffer: mData.refCounted = new RefCounted<DataBuffer>(Type_DataBuffer); break;
    case Type_Array: mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array); break;
    case Type_IntegerMap: mData.refCounted = new RefCounted<std::map<int, Variant> >(Type_IntegerMap); break;
    case Type_StringMap: mData.refCounted = new RefCounted<std::map<std::string, Variant> >(Type_StringMap); break;
    case Type_Custom: mType = Type_Null; break; // can't have 0 custom
    case Type_Integer: mData.integer = 0; break;
    case Type_Double: mData.dbl = 0; break;
    case Type_Boolean: mData.boolean = false; break;
    case Type_Pointer: mData.pointer = 0; break;
    }
}

inline Variant::Variant(const char *str) : mType(Type_Null)
{
    if (str) {
        mType = Type_String;
        mData.refCounted = new RefCounted<std::string>(Type_String, std::string(str));
    }
    CONSTRUCT(mType);
}

inline Variant::Variant(const std::string &str)
{
    mType = Type_String;
    mData.refCounted = new RefCounted<std::string>(Type_String, str);
    CONSTRUCT(mType);
}

inline Variant::Variant(const DataBuffer &buffer)
{
    mType = Type_DataBuffer;
    mData.refCounted = new RefCounted<DataBuffer>(Type_DataBuffer, buffer);
    CONSTRUCT(mType);
}

inline Variant::Variant(const std::string &key, const Variant &value)
{
    mType = Type_StringMap;
    mData.refCounted = new RefCounted<std::map<std::string, Variant> >(Type_StringMap);
    (*stringMapPtr())[key] = value;
    CONSTRUCT(mType);
}

inline Variant::Variant(int key, const Variant &value)
{
    mType = Type_IntegerMap;
    mData.refCounted = new RefCounted<std::map<int, Variant> >(Type_IntegerMap);
    (*integerMapPtr())[key] = value;
    CONSTRUCT(mType);
}

inline Variant::Variant(const std::vector<Variant> &vec)
{
    mType = Type_Array;
    mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array, vec);
    CONSTRUCT(mType);
}

inline Variant::Variant(const std::map<std::string, Variant> &map)
{
    mType = Type_StringMap;
    mData.refCounted = new RefCounted<std::map<std::string, Variant> >(Type_StringMap, map);
    CONSTRUCT(mType);
}

inline Variant::Variant(const std::map<int, Variant> &map)
{
    mType = Type_IntegerMap;
    mData.refCounted = new RefCounted<std::map<int, Variant> >(Type_IntegerMap, map);
    CONSTRUCT(mType);
}

inline Variant::Variant(const Variant::StringMap &map)
{
    mType = Type_StringMap;
    mData.refCounted = new RefCounted<std::map<std::string, Variant> >(Type_StringMap, map.map());
    CONSTRUCT(mType);
}

inline Variant::Variant(const Variant::Array &array)
{
    mType = Type_Array;
    mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array, array.array());
    CONSTRUCT(mType);
}

template<typename T>
inline Variant::Variant(const std::vector<T> &vec)
{
    mType = Type_Array;
    mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array);

    std::vector<Variant> &array = *arrayPtr();
    array.resize(vec.size());

    int i = 0;
    for (typename std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
        array[i++] = *it;
    }
    CONSTRUCT(mType);
}

template<typename A, typename B>
inline Variant::Variant(const std::pair<A, B> &pair)
{
    mType = Type_Array;
    mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array);

    std::vector<Variant> &array = *arrayPtr();
    array.resize(2);
    array[0] = pair.first;
    array[1] = pair.second;
    CONSTRUCT(mType);
}

template<typename T>
inline Variant::Variant(const std::map<std::string, T> &map)
{
    mType = Type_StringMap;
    mData.refCounted = new RefCounted<std::map<std::string, Variant> >(Type_StringMap);
    std::map<std::string, Variant> &m = *stringMapPtr();

    for (typename std::map<std::string, T>::const_iterator it = map.begin(); it != map.end(); ++it) {
        m[(*it).first] = Variant((*it).second);
    }
    CONSTRUCT(mType);
}

template<typename T>
inline Variant::Variant(const std::map<int, T> &map)
{
    mType = Type_IntegerMap;
    mData.refCounted = new RefCounted<std::map<int, Variant> >(Type_IntegerMap);
    std::map<int, Variant> &m = *integerMapPtr();

    for (typename std::map<std::string, T>::const_iterator it = map.begin(); it != map.end(); ++it) {
        m[(*it).first] = Variant((*it).second);
    }
    CONSTRUCT(mType);
}


inline Variant::Variant(bool b)
{
    mType = Type_Boolean;
    mData.boolean = b;
    CONSTRUCT(mType);
}

inline Variant::Variant(int8_t val)
{
    mType = Type_Integer;
    mData.integer = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(int16_t val)
{
    mType = Type_Integer;
    mData.integer = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(int32_t val)
{
    mType = Type_Integer;
    mData.integer = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(long val)
{
    mType = Type_Integer;
    mData.integer = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(long long val)
{
    mType = Type_Integer;
    mData.integer = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(uint8_t val)
{
    mType = Type_Integer;
    mData.integer = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(uint16_t val)
{
    mType = Type_Integer;
    mData.integer = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(uint32_t val)
{
    mType = Type_Integer;
    mData.integer = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(unsigned long val)
{
    mType = Type_Integer;
    mData.integer = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(unsigned long long val)
{
    mType = Type_Integer;
    mData.integer = val;
    CONSTRUCT(mType);
}


inline Variant::Variant(double val)
{
    mType = Type_Double;
    mData.dbl = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(void *val)
{
    mType = Type_Pointer;
    mData.pointer = val;
    CONSTRUCT(mType);
}

inline Variant::Variant(const void *val)
{
    mType = Type_Pointer;
    mData.pointer = const_cast<void*>(val);
    CONSTRUCT(mType);
}

inline Variant::Variant(const shared_ptr<CustomData> &data)
    : mType(data.get() ? Type_Custom : Type_Null)
{
    if (data)
        new (mData.customDataBuffer) shared_ptr<CustomData>(data);
    CONSTRUCT(mType);
}

inline Variant::Variant(const Variant &other)
    : mType(Type_Null)
{
    operator=(other);
}

inline Variant::~Variant()
{
    clear();
}

inline void Variant::clear()
{
    switch (mType) {
    case Type_Null:
    case Type_Pointer:
    case Type_Integer:
    case Type_Boolean:
    case Type_Double:
        break;
    case Type_Custom:
        customPtr()->~shared_ptr<CustomData>();
        break;
    case Type_Array:
    case Type_String:
    case Type_StringMap:
    case Type_IntegerMap:
    case Type_DataBuffer:
        mData.refCounted->deref();
        break;
    }
    mType = Type_Null;
}

inline bool Variant::isNull() const
{
    return mType == Type_Null;
}

inline bool Variant::isString() const
{
    return mType == Type_String;
}

inline bool Variant::isDataBuffer() const
{
    return mType == Type_DataBuffer;
}

inline bool Variant::isArray() const
{
    return mType == Type_Array || mType == Type_IntegerMap;
}

inline bool Variant::isStringMap() const
{
    return mType == Type_StringMap;
}

inline bool Variant::isIntegerMap() const
{
    return mType == Type_IntegerMap;
}

inline bool Variant::isInteger() const
{
    return mType == Type_Integer;
}

inline bool Variant::isDouble() const
{
    return mType == Type_Double;
}

inline bool Variant::isPointer() const
{
    return mType == Type_Pointer;
}

inline bool Variant::isBoolean() const
{
    return mType == Type_Boolean;
}

inline bool Variant::isCustom() const
{
    return mType == Type_Custom;
}

inline Variant Variant::mapValue(const std::string &key, bool *okPtr, const Variant &defaultValue) const
{
    if (type() == Type_StringMap) {
        const std::map<std::string, Variant> *map = stringMapPtr();
        const StringMapIterator it = map->find(key);
        if (it != map->end()) {
            if (okPtr)
                *okPtr = true;
            return it->second;
        }
    }
    if (okPtr)
        *okPtr = false;
    return defaultValue;
}

template <typename T> inline T Variant::mapValue(const std::string &key, bool *okPtr, const T &defaultValue) const
{
    if (type() == Type_StringMap) {
        const StringMapIterator it = stringMapPtr()->find(key);
        if (it != stringMapPtr()->end())
            return (*it).second.value<T>(okPtr, defaultValue);
    }
    if (okPtr)
        *okPtr = false;
    return defaultValue;
}

template <typename T> inline T Variant::mapValue(int key, bool *okPtr, const T &defaultValue) const
{
    if (type() == Type_IntegerMap) {
        const std::map<int, Variant>::const_iterator it = integerMapPtr()->find(key);
        if (it != integerMapPtr()->end())
            return (*it).second.value<T>(okPtr, defaultValue);
    }
    if (okPtr)
        *okPtr = false;
    return defaultValue;
}

template <typename T> inline T Variant::value(bool *ok, const T &defaultValue) const
{
    const Variant::Type t = variantType<T>();
    if (t == type()) {
        if (ok)
            *ok = true;
        T out;
        variantValue(&out);
        return out;
    } else {
        const Variant v = convert(variantType<T>());
        if (!v.isNull()) {
            assert(v.type() == t);
            if (ok)
                *ok = true;
            T out;
            v.variantValue(&out);
            return out;
        } else if (ok) {
            *ok = false;
        }
    }
    return defaultValue;
}

namespace VariantType {
template <typename T> static inline Variant::Type variantType(T *t)
{
    invalidVariantType(t);
    return Variant::Type_Null;
}
template <> inline Variant::Type variantType(int8_t*) { return Variant::Type_Integer; }
template <> inline Variant::Type variantType(int16_t*) { return Variant::Type_Integer; }
template <> inline Variant::Type variantType(int32_t*) { return Variant::Type_Integer; }
template <> inline Variant::Type variantType(llong*) { return Variant::Type_Integer; }
template <> inline Variant::Type variantType(uint8_t*) { return Variant::Type_Integer; }
template <> inline Variant::Type variantType(uint16_t*) { return Variant::Type_Integer; }
template <> inline Variant::Type variantType(uint32_t*) { return Variant::Type_Integer; }
template <> inline Variant::Type variantType(ullong*) { return Variant::Type_Integer; }
template <> inline Variant::Type variantType(bool*) { return Variant::Type_Boolean; }
template <> inline Variant::Type variantType(double*) { return Variant::Type_Double; }
template <> inline Variant::Type variantType(float*) { return Variant::Type_Double; }
template <> inline Variant::Type variantType(std::vector<Variant>*) { return Variant::Type_Array; }
template <> inline Variant::Type variantType(std::map<std::string, Variant>*) { return Variant::Type_StringMap; }
template <> inline Variant::Type variantType(std::map<int, Variant>*) { return Variant::Type_IntegerMap; }
template <> inline Variant::Type variantType(std::string*) { return Variant::Type_String; }
template <> inline Variant::Type variantType(DataBuffer*) { return Variant::Type_DataBuffer; }
}

template <typename T> inline Variant::Type Variant::variantType()
{
    return VariantType::variantType<T>(static_cast<T*>(0));
}

inline Variant Variant::convert(Type targetType) const
{
    if (isNull())
        return Variant();
    if (targetType == mType)
        return *this;
    switch (targetType) {
    case Type_Boolean:
        switch (mType) {
        case Type_Null:
            return Variant(false);
        case Type_Integer:
            return (integer() != 0);
        case Type_Double:
            return (dbl() != 0);
        case Type_Pointer:
            return (pointer() != 0);
        case Type_Array:
        case Type_IntegerMap:
        case Type_StringMap:
        case Type_Custom:
        case Type_Boolean:
            break;
        case Type_String: {
            if (*stringPtr() == "true")
                return Variant(true);
            if (*stringPtr() == "false")
                return Variant(false);
            const Variant val = convert(Type_Double);
            if (val.type() == Type_Double)
                return static_cast<bool>(val.dbl());
            break; }
        case Type_DataBuffer: {
            const DataBuffer str = dataBuffer();
            if (str == "true")
                return Variant(true);
            if (str == "false")
                return Variant(false);
            const Variant val = convert(Type_Double);
            if (val.type() == Type_Double)
                return static_cast<bool>(val.dbl());
            break; }
        }
        break;
    case Type_Integer:
        switch (mType) {
        case Type_Boolean:
            return (mData.boolean ? 1 : 0);
        case Type_Null:
        case Type_Array:
        case Type_IntegerMap:
        case Type_StringMap:
        case Type_Custom:
        case Type_Integer:
        case Type_Pointer:
            break;
        case Type_Double:
            return static_cast<llong>(dbl());
        case Type_String:
        case Type_DataBuffer: {
            if (!(mType == Type_String ? stringPtr()->empty() : dataBufferPtr()->isEmpty())) {
                const char *data = (mType == Type_String ? stringPtr()->c_str() : dataBufferPtr()->c_str());
                char *endptr;
                const llong integer = strtoll(data, &endptr, 0);
                if (!*endptr)
                    return Variant(integer);
            }
            break; }
        }
        break;
    case Type_Double:
        switch (mType) {
        case Type_Boolean:
            return (mData.boolean ? 1.0 : 0.0);
        case Type_Double:
        case Type_Null:
        case Type_Array:
        case Type_IntegerMap:
        case Type_StringMap:
        case Type_Custom:
        case Type_Pointer:
            break;
        case Type_Integer:
            return static_cast<double>(integer());
        case Type_String:
        case Type_DataBuffer: {
            if (!(mType == Type_String ? stringPtr()->empty() : dataBufferPtr()->isEmpty())) {
                const char *data = (mType == Type_String ? stringPtr()->c_str() : dataBufferPtr()->c_str());
                char *endptr;
                const double dbl = strtod(data, &endptr);
                if (!*endptr)
                    return Variant(dbl);
            }
            break; }
        }
        break;
    case Type_String:
        switch (mType) {
        case Type_String:
            return string();
        case Type_DataBuffer:
            return dataBuffer().toString();
        default:
            return toJSON();
        }
        break;
    case Type_DataBuffer:
        switch (mType) {
        case Type_String:
            return DataBuffer(string());
        case Type_DataBuffer:
            return dataBuffer();
        default:
            return DataBuffer(toJSON());
        }
        break;
    case Type_IntegerMap:
    case Type_StringMap:
    case Type_Array:
    case Type_Null:
    case Type_Custom:
    case Type_Pointer:
        break;
    }
    return Variant();
}

inline bool Variant::canConvert(Type targetType) const
{
    if (targetType == mType || targetType == Type_Null)
        return true;
    return !convert(targetType).isNull();
}

inline Variant &Variant::operator=(const Variant &other)
{
#ifdef VARIANT_STATS_ENABLED
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&mutex);

    static int *copies = 0;
    if (!copies) {
        copies = new int[Type_Custom + 1];
        memset(copies, 0, sizeof(int) * (Type_Custom + 1));
    }
    ++copies[other.mType];
    static int count = 0;
    if (++count % 1000 == 0) {
        const char *names[] = {
            "Null",
            "String",
            "Array",
            "IntegerMap",
            "StringMap",
            "Integer",
            "Double",
            "Boolean",
            "DataBuffer",
            "Custom"
            };

        for (int i=0; i<=Type_Custom; ++i) {
            printf("Copied %s: %d\n", names[i], copies[i]);
        }
        printf("Total copies: %d\n", count);
    }
    pthread_mutex_unlock(&mutex);
#endif
    clear();
    mType = other.mType;
    switch (mType) {
    case Type_Null:
        break;
    case Type_Pointer:
        mData.pointer = other.mData.pointer;
        break;
    case Type_Boolean:
        mData.boolean = other.mData.boolean;
        break;
    case Type_Integer:
        mData.integer = other.mData.integer;
        break;
    case Type_Double:
        mData.dbl = other.mData.dbl;
        break;
    case Type_Custom:
        new (mData.customDataBuffer) shared_ptr<CustomData>(*other.customPtr());
        break;
    case Type_Array:
    case Type_StringMap:
    case Type_IntegerMap:
    case Type_String:
    case Type_DataBuffer:
        mData.refCounted = other.mData.refCounted;
        mData.refCounted->ref();
        break;
    }
    return *this;
}

inline bool Variant::operator==(const Variant &other) const
{
    const Type me = type();
    const Type him = other.type();
    if (me == him) {
        switch (me) {
        case Type_Custom:
            return customPtr()->get() == other.customPtr()->get();
        case Type_Null:
            return true;
        case Type_Integer:
            return (mData.integer == other.mData.integer);
        case Type_Pointer:
            return (mData.pointer == other.mData.pointer);
        case Type_Boolean:
            return (mData.boolean == other.mData.boolean);
        case Type_Double:
            return (mData.dbl == other.mData.dbl);
        case Type_DataBuffer:
            return dataBuffer() == other.dataBuffer();
        case Type_Array:
            return (*arrayPtr() == *other.arrayPtr());
        case Type_String:
            return (*stringPtr() == *other.stringPtr());
        case Type_StringMap:
            return (*stringMapPtr() == *other.stringMapPtr());
        case Type_IntegerMap:
            return (*integerMapPtr() == *other.integerMapPtr());
        }
    }
    return false;
}

inline bool Variant::operator!=(const Variant &other) const
{
    return !operator==(other);
}

inline Variant::Type Variant::type() const
{
    return mType;
}

inline bool Variant::boolean() const
{
    return isBoolean() ? mData.boolean : false;
}

inline llong Variant::integer() const
{
    return isInteger() ? mData.integer : 0;
}

inline double Variant::dbl() const
{
    return isDouble() ? mData.dbl : .0;
}

inline void *Variant::pointer() const
{
    return isPointer() ? mData.pointer : 0;
}

inline std::string Variant::string() const
{
    return isString() ? *stringPtr() : std::string();
}

inline DataBuffer Variant::dataBuffer() const
{
    return isDataBuffer() ? *dataBufferPtr() : DataBuffer();
}

inline std::vector<Variant> Variant::array() const
{
    if (isArray())
        return *arrayPtr();
    return std::vector<Variant>();
}

inline std::map<std::string, Variant> Variant::stringMap() const
{
    return isStringMap() ? *stringMapPtr() : std::map<std::string, Variant>();
}

inline std::map<int, Variant> Variant::integerMap() const
{
    return isIntegerMap() ? *integerMapPtr() : std::map<int, Variant>();
}

inline const Variant &Variant::null()
{
    static const Variant v;
    return v;
}

inline const char *Variant::c_str() const
{
    switch (mType) {
    case Type_String:
        return stringPtr()->c_str();
    case Type_DataBuffer:
        return dataBufferPtr()->c_str();
    default:
        break;
    }
    return 0;
}

inline VariantStringStream Variant::stringStream()
{
    switch (mType) {
    case Type_Null:
        mType = Type_String;
        mData.refCounted = new RefCounted<std::string>(Type_String);
        CONSTRUCT(mType);
        // fall through
    case Type_String:
        return VariantStringStream(stringPtr());
    default:
        break;
    }
    return VariantStringStream(0);
}

inline shared_ptr<const Variant::CustomData> Variant::custom() const
{
    return isCustom() ? *customPtr() : shared_ptr<Variant::CustomData>();
}

inline Variant &Variant::operator[](const std::string &key)
{
    switch (type()) {
    case Type_Null:
        mType = Type_StringMap;
        mData.refCounted = new RefCounted<std::map<std::string, Variant> >(Type_StringMap);
        CONSTRUCT(mType);
        break;
    case Type_StringMap:
        detach();
        break;
    default:
        assert(0);
        break;
    }
    return (*stringMapPtr())[key];
}

inline const Variant &Variant::operator[](const std::string &key) const
{
    if (isStringMap()) {
        const Variant::StringMapIterator it = stringMapPtr()->find(key);
        if (it != stringMapPtr()->end())
            return (*it).second;
    }
    return Variant::null();
}

inline Variant &Variant::operator[](int idx)
{
    assert(idx >= 0);
    switch (mType) {
    case Type_IntegerMap:
        detach();
        return (*integerMapPtr())[idx];
    case Type_Null: {
        mType = Type_Array;
        mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array);
        std::vector<Variant> &a = *arrayPtr();
        a.resize(idx + 1);
        CONSTRUCT(mType);
        return a[idx]; }
    case Type_Array: {
        detach();
        std::vector<Variant> &a = *arrayPtr();
        if (a.size() <= size_t(idx))
            a.resize(idx + 1);
        return a[idx];
        break; }
    default:
        assert(0);
        break;
    }
    return *new Variant; // this should never happen so just leak it to shut up the compiler
}

inline const Variant &Variant::operator[](int key) const
{
    assert(key >= 0);
    if (isArray() && key < size()) {
        return (*arrayPtr())[key];
    } else if (isIntegerMap()) {
        const std::map<int, Variant>::const_iterator it = integerMapPtr()->find(key);
        if (it != integerMapPtr()->end())
            return (*it).second;
    }
    return Variant::null();
}

inline Variant& Variant::back()
{
    return (*this)[ size() - 1 ];
}

inline Variant const& Variant::back() const
{
    return (*this)[ size() - 1 ];
}

inline void Variant::resizeArray(int size)
{
    assert(size >= 0);
    switch (type()) {
    case Type_Null:
        mType = Type_Array;
        mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array);
        CONSTRUCT(mType);
        arrayPtr()->resize(size);
        break;
    case Type_Array:
        detach();
        arrayPtr()->resize(size);
        break;
    default:
        assert(0);
        break;
    }
}

inline void Variant::push_back(const Variant &variant)
{
    switch (mType) {
    case Type_IntegerMap: {
        detach();
        std::vector<Variant> vec;
        for (std::map<int, Variant>::const_iterator it = integerMapPtr()->begin(); it != integerMapPtr()->end(); ++it)
            vec[(*it).first] = (*it).second;
        vec.push_back(variant);
        clear();
        mType = Type_Array;
        mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array, vec);
        break; }
    case Type_Array:
        detach();
        arrayPtr()->push_back(variant);
        break;
    case Type_Null:
        mType = Type_Array;
        mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array);
        arrayPtr()->push_back(variant);
        break;
    default:
        assert(0);
        break;
    }
}

inline int Variant::insert(int pos, const Variant &variant)
{
    switch (mType) {
    case Type_IntegerMap: {
        detach();
        std::vector<Variant> vec;
        for (std::map<int, Variant>::const_iterator it = integerMapPtr()->begin(); it != integerMapPtr()->end(); ++it)
            vec[(*it).first] = (*it).second;
        vec.insert(vec.begin() + pos, variant);
        clear();
        mType = Type_Array;
        mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array, vec);
        break; }
    case Type_Array:
        detach();
        arrayPtr()->insert(arrayPtr()->begin() + pos, variant);
        break;
    case Type_Null:
        mType = Type_Array;
        mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array);
        arrayPtr()->insert(arrayPtr()->begin() + pos, variant);
        break;
    default:
        assert(0);
        break;
    }

    return pos;
}

template<typename InputIterator>
inline void Variant::insert(int pos, InputIterator first, InputIterator last)
{
    switch (mType) {
    case Type_IntegerMap: {
        detach();
        std::vector<Variant> vec;
        for (std::map<int, Variant>::const_iterator it = integerMapPtr()->begin(); it != integerMapPtr()->end(); ++it)
            vec[(*it).first] = (*it).second;
        vec.insert(vec.begin() + pos, first, last);
        clear();
        mType = Type_Array;
        mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array, vec);
        break; }
    case Type_Array:
        detach();
        arrayPtr()->insert(arrayPtr()->begin() + pos, first, last);
        break;
    case Type_Null:
        mType = Type_Array;
        mData.refCounted = new RefCounted<std::vector<Variant> >(Type_Array);
        arrayPtr()->insert(arrayPtr()->begin() + pos, first, last);
        break;
    default:
        assert(0);
        break;
    }
}

template<typename InputIterator>
inline void Variant::extend(InputIterator first, InputIterator last)
{
    insert(size(), first, last);
}

inline bool Variant::contains(const std::string &key) const
{
    return isStringMap() ? stringMapPtr()->find(key) != stringMapPtr()->end() : false;
}

inline bool Variant::contains(int key) const
{
    if (isArray())
        return key >= 0 && key < (int)arrayPtr()->size();
    else if (isIntegerMap())
        return integerMapPtr()->find(key) != integerMapPtr()->end();
    return false;
}

inline void Variant::remove(const std::string &key)
{
    if (isStringMap()) {
        detach();
        stringMapPtr()->erase(key);
    }
}

inline void Variant::remove(int index)
{
    switch (mType) {
    case Type_Array:
        detach();
        arrayPtr()->erase(arrayPtr()->begin() + index);
        break;
    case Type_IntegerMap:
        detach();
        integerMapPtr()->erase(index);
        break;
    default:
        break;
    }
}

inline Variant::StringMapIterator Variant::find(const std::string &name) const
{
    return isStringMap() ? stringMapPtr()->find(name) : sNullStringMap.end();
}

inline Variant::StringMapIterator Variant::stringMapBegin() const
{
    return isStringMap() ? stringMapPtr()->begin() : sNullStringMap.begin();
}

inline Variant::StringMapIterator Variant::stringMapEnd() const
{
    return isStringMap() ? stringMapPtr()->end() : sNullStringMap.end();
}

inline Variant::ArrayIterator Variant::arrayBegin() const
{
    return isArray() ? arrayPtr()->begin() : sNullArray.begin();
}

inline Variant::ArrayIterator Variant::arrayEnd() const
{
    return isArray() ? arrayPtr()->end() : sNullArray.end();
}

inline int Variant::size() const
{
    switch (type()) {
    case Type_StringMap:
        return stringMapPtr()->size();
    case Type_IntegerMap:
        return integerMapPtr()->size();
    case Type_Array:
        return arrayPtr()->size();
    case Type_DataBuffer:
        return dataBufferPtr()->size();
    case Type_String:
        return stringPtr()->size();
    default:
        break;
    }
    return 0;
}

inline bool Variant::empty() const
{
    return !size();
}

inline Variant Variant::value(const std::string &key, bool *ok, const Variant &defaultValue) const
{
    if (isStringMap()) {
        const Variant::StringMapIterator it = stringMapPtr()->find(key);
        if (it != stringMapPtr()->end()) {
            if (ok)
                *ok = true;
            return (*it).second;
        }
    }
    if (ok)
        *ok = false;
    return defaultValue;
}

inline Variant Variant::value(int key, bool *ok, const Variant &defaultValue) const
{
    if (isArray()) {
        if (key >= 0 && static_cast<int>(arrayPtr()->size()) > key) {
            if (ok)
                *ok = true;
            return arrayPtr()->at(key);
        }
    } else if (isIntegerMap()) {
        const std::map<int, Variant>::const_iterator it = integerMapPtr()->find(key);
        if (it != integerMapPtr()->end()) {
            if (ok)
                *ok = true;
            return (*it).second;
        }
    }
    if (ok)
        *ok = false;
    return defaultValue;
}

inline void Variant::merge(const Variant &other)
{
    if (isStringMap()) {
        if (other.isStringMap()) {
            const StringMapIterator end = other.stringMapEnd();
            for (StringMapIterator it = other.stringMapBegin(); it != end; ++it) {
                Variant &t = operator[](it->first);
                t.merge(it->second);
            }
        } else if (!other.isNull()) {
            operator=(other);
        }
    } else {
        operator=(other);
    }
}


inline void Variant::detach()
{
    switch (mType) {
    case Type_Null:
    case Type_Integer:
    case Type_Boolean:
    case Type_Double:
    case Type_Custom:
    case Type_Pointer:
        break;
    case Type_Array:
        detach<std::vector<Variant> >();
        break;
    case Type_String:
        detach<std::string>();
        break;
    case Type_StringMap:
        detach<std::map<std::string, Variant> >();
        break;
    case Type_IntegerMap:
        detach<std::map<int, Variant> >();
        break;
    case Type_DataBuffer:
        detach<DataBuffer>();
        break;
    }
}


inline int Variant::refCount() const
{
    switch (mType) {
    case Type_Null:
    case Type_Integer:
    case Type_Boolean:
    case Type_Double:
    case Type_Custom:
    case Type_Pointer:
        break;
    case Type_Array:
    case Type_String:
    case Type_StringMap:
    case Type_IntegerMap:
    case Type_DataBuffer:
        return mData.refCounted->count();
    }
    return 0;
}

}

#endif
