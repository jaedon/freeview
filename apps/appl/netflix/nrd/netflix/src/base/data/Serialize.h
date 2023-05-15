/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

#include <nrdbase/DataBuffer.h>
#include <nrdbase/Variant.h>
#include <nrdbase/Endian.h>

namespace netflix {

class Serializer
{
public:
    enum {
        LittleEndianFlag = 0x01,
    };

    inline Serializer(uint8_t flags = defaultFlags())
        : mFlags(flags)
    {}

    static uint8_t defaultFlags()
    {
        uint8_t flags = 0;
#ifdef NRDP_LITTLE_ENDIAN
        flags |= Serializer::LittleEndianFlag;
#endif
        return flags;
    }

    inline void clear() { mData.clear(); }

    inline void setFlags(int flags) { mFlags = flags; }
    inline uint8_t flags() const { return mFlags; }

    inline DataBuffer data() const { return mData; }
    inline unsigned int length() const { return mData.length(); }

    inline void serialize(const unsigned char *data, uint32_t length) { mData.append(data, length); }

private:
    uint8_t mFlags;
    DataBuffer mData;
};

class Deserializer
{
public:
    inline Deserializer()
        : mFlags(Serializer::defaultFlags()), mOffset(0)
    {}
    inline Deserializer(const DataBuffer &data, uint8_t flags = Serializer::defaultFlags())
        : mFlags(flags), mOffset(0), mData(data)
    {}
    inline Deserializer(Deserializer &other, int len)
        : mFlags(other.mFlags), mOffset(0), mData(other.deserialize(len))
    {}

    inline void clear() { setData(DataBuffer()); }

    inline void setData(const DataBuffer &data)
    {
        mOffset = 0;
        mData = data;
    }
    inline void setData(const DataBuffer &data, uint8_t flags)
    {
        setData(data);
        setFlags(flags);
    }
    inline void trim()
    {
        if (mOffset) {
            mData.remove(0, mOffset);
            mOffset = 0;
        }
    }
    inline void append(const unsigned char *data, int length) { mData.append(data, length); }

    inline DataBuffer data() const { return mData; }
    inline unsigned int offset() const { return mOffset; }
    inline unsigned int length() const { return mData.length() - mOffset; }

    inline uint8_t flags() const { return mFlags; }
    inline void setFlags(int flags) { mFlags = flags; }

    inline bool deserializeLine(DataBuffer &line)
    {
        const int l = length();
        const unsigned char *data = mData.constData() + mOffset;
        for (int i = 0; i < l; ++i) {
            if (*(data + i) == '\n') {
                line = deserialize(i + 1);
                return true;
            }
        }
        return false;
    }
    inline DataBuffer deserialize(uint32_t length)
    {
        assert(uint32_t(mData.length()) >= mOffset + length);
        DataBuffer result = mData.slice(mOffset, length);
        mOffset += length;
        return result;
    }
    inline void deserialize(unsigned char *data, uint32_t length)
    {
        assert(uint32_t(mData.length()) >= mOffset + length);
        memcpy(data, mData.data() + mOffset, length);
        mOffset += length;
    }

private:
    uint8_t mFlags;
    uint32_t mOffset;
    DataBuffer mData;
};

#define SERIALIZE_ALIAS(type, alias)                                    \
    inline Serializer &operator<<(Serializer &serializer, const type &_m) \
    {                                                                   \
        const alias m = _m;                                             \
        serializer << m;                                                \
        return serializer;                                              \
    }                                                                   \
    inline Deserializer &operator>>(Deserializer &deserializer, type &_m) \
    {                                                                   \
        alias m;                                                        \
        deserializer >> m;                                              \
        _m = (type)m;                                                   \
        return deserializer;                                            \
    }

template <typename T> inline Serializer &operator<<(Serializer &serializer, T n)
{
#ifdef NRDP_LITTLE_ENDIAN
    if (!(serializer.flags() & Serializer::LittleEndianFlag))
        n = Endian::native_to_big(n);
#else
    if (serializer.flags() & Serializer::LittleEndianFlag)
        n = Endian::native_to_little(n);
#endif
    serializer.serialize((const unsigned char *)&n, sizeof(n));
    return serializer;
}

template <typename T> inline Deserializer &operator>>(Deserializer &deserializer, T &n)
{
    deserializer.deserialize((unsigned char *)&n, sizeof(n));
#ifdef NRDP_LITTLE_ENDIAN
    if (!(deserializer.flags() & Serializer::LittleEndianFlag))
        n = Endian::big_to_native(n);
#else
    if (deserializer.flags() & Serializer::LittleEndianFlag)
        n = Endian::little_to_native(n);
#endif
    return deserializer;
}

inline Serializer &operator<<(Serializer &serializer, const float &f)
{
#if 0
    serializer << StringFormatter::sformat("%f", f);
#else
    serializer.serialize((unsigned char *)&f, sizeof(f));
#endif
    return serializer;
}

inline Deserializer &operator>>(Deserializer &deserializer, float &f)
{
#if 0
    std::string str;
    deserializer >> str;
    sscanf(str.c_str(), "%f", &f);
#else
    deserializer.deserialize((unsigned char *)&f, sizeof(f));
#endif
    return deserializer;
}

inline Serializer &operator<<(Serializer &serializer, const double &f)
{
#if 0
    serializer << StringFormatter::sformat("%f", f);
#else
    serializer.serialize((unsigned char *)&f, sizeof(f));
#endif
    return serializer;
}

inline Deserializer &operator>>(Deserializer &deserializer, double &f)
{
#if 0
    std::string str;
    deserializer >> str;
    sscanf(str.c_str(), "%f", &f);
#else
    deserializer.deserialize((unsigned char *)&f, sizeof(f));
#endif
    return deserializer;
}

SERIALIZE_ALIAS(bool, uint8_t)
SERIALIZE_ALIAS(int32_t, uint32_t)
SERIALIZE_ALIAS(unsigned long, uint32_t)

inline Serializer &operator<<(Serializer &serializer, const std::string &s)
{
    const uint32_t l = s.length();
    serializer << l;
    if (l)
        serializer.serialize((unsigned char *)s.c_str(), l);
    return serializer;
}

inline Deserializer &operator>>(Deserializer &deserializer, std::string &s)
{
    uint32_t l;
    deserializer >> l;
    if (l)
        s.assign((const char *)deserializer.deserialize(l).data(), l);
    return deserializer;
}

inline Serializer &operator<<(Serializer &serializer, const DataBuffer &d)
{
    const uint32_t l = d.length();
    serializer << l;
    if (l)
        serializer.serialize(d.data(), l);
    return serializer;
}

inline Deserializer &operator>>(Deserializer &deserializer, DataBuffer &d)
{
    uint32_t l;
    deserializer >> l;
    if (l)
        d = deserializer.deserialize(l);
    return deserializer;
}

inline Serializer &operator<<(Serializer &serializer, const Variant &v)
{
    serializer << uint8_t(v.type());
    switch (v.type()) {
    case Variant::Type_Null:
        break;
    case Variant::Type_Pointer:
        serializer << v.pointer();
        break;
    case Variant::Type_String:
        serializer << v.string();
        break;
    case Variant::Type_Array: {
        const std::vector<Variant> array = v.array();
        serializer << uint16_t(array.size());
        for (std::vector<Variant>::const_iterator it = array.begin(); it != array.end(); ++it)
            serializer << *it;
        break;
    }
    case Variant::Type_IntegerMap: {
        const std::map<int, Variant> map = v.integerMap();
        serializer << uint16_t(map.size());
        for (std::map<int, Variant>::const_iterator it = map.begin(); it != map.end(); ++it)
            serializer << uint32_t(it->first) << it->second;
        break;
    }
    case Variant::Type_StringMap: {
        const std::map<std::string, Variant> map = v.stringMap();
        serializer << uint16_t(map.size());
        for (std::map<std::string, Variant>::const_iterator it = map.begin(); it != map.end(); ++it)
            serializer << it->first << it->second;
        break;
    }
    case Variant::Type_Integer:
        serializer << v.integer();
        break;
    case Variant::Type_Double:
        serializer << v.dbl();
        break;
    case Variant::Type_Boolean:
        serializer << v.boolean();
        break;
    case Variant::Type_DataBuffer:
        serializer << v.dataBuffer();
        break;
    case Variant::Type_Custom:
        serializer << v.custom()->toVariant();
        break;
    }
    return serializer;
}

inline Deserializer &operator>>(Deserializer &deserializer, Variant &v)
{
    uint8_t type;
    deserializer >> type;
    switch (type) {
    case Variant::Type_Null:
        v = Variant();
        break;
    case Variant::Type_Pointer: {
        void *pointer;
        deserializer >> pointer;
        v = Variant(pointer);
        break; }
    case Variant::Type_String: {
        std::string string;
        deserializer >> string;
        v = Variant(string);
        break; }
    case Variant::Type_Array: {
        uint16_t size;
        deserializer >> size;
        std::vector<Variant> array;
        for (int i = 0; i < size; ++i) {
            Variant value;
            deserializer >> value;
            array.push_back(value);
        }
        v = Variant(array);
        break; }
    case Variant::Type_IntegerMap: {
        v = Variant(Variant::Type_IntegerMap);
        uint16_t size;
        deserializer >> size;
        for (int i = 0; i < size; ++i) {
            uint32_t key;
            deserializer >> key;
            Variant value;
            deserializer >> value;
            v[key] = value;
        }
        break; }
    case Variant::Type_StringMap: {
        uint16_t size;
        deserializer >> size;
        v = Variant(Variant::Type_StringMap);
        for (int i = 0; i < size; ++i) {
            std::string key;
            deserializer >> key;
            Variant value;
            deserializer >> value;
            v[key] = value;
        }
        break; }
    case Variant::Type_Integer: {
        llong integer;
        deserializer >> integer;
        v = Variant(integer);
        break; }
    case Variant::Type_Double: {
        double dbl;
        deserializer >> dbl;
        v = Variant(dbl);
        break; }
    case Variant::Type_Boolean: {
        bool boolean;
        deserializer >> boolean;
        v = Variant(boolean);
        break; }
    case Variant::Type_DataBuffer: {
        DataBuffer dataBuffer;
        deserializer >> dataBuffer;
        v = Variant(dataBuffer);
        break; }
    case Variant::Type_Custom: {
        deserializer >> v;
        break; }
    }
    assert(v.type() == type);
    return deserializer;
}

} // namespace netflix

#endif /* __SERIALIZE_H__ */
