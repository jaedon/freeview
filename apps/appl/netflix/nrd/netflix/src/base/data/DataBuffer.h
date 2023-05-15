/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DataBuffer_h
#define DataBuffer_h

#include <string.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/RefCounted.h>
#include <nrdbase/Compiler.h>
#include <vector>
#include <string>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <zlib.h>
#include <iostream>

namespace netflix {

class Mutex;
class DataBuffer
{
public:
    typedef const unsigned char &const_reference;
    typedef unsigned char &reference;
    typedef unsigned char value_type;

    inline DataBuffer(int cap = 0);
    inline DataBuffer(int cap, unsigned char fillByte);
    inline DataBuffer(const unsigned char *d, int len);
    inline DataBuffer(const char *d, int len = -1);
    inline DataBuffer(const std::string &str);
    inline DataBuffer(const std::vector<unsigned char> &v);
    inline DataBuffer(const DataBuffer &other);
    inline ~DataBuffer();

    DataBuffer &operator=(const DataBuffer &other);

    inline static DataBuffer fromRawData(const unsigned char *data, int size);
    inline static DataBuffer fromRawData(const char *data, int size);
    inline static DataBuffer fromRawData(const std::string &string) { return fromRawData(string.c_str(), string.size()); }

    inline static DataBuffer fromFile(const std::string &path, long max = -1, bool *ok = 0) { return fromFile(path.c_str(), max, ok); }
    inline static DataBuffer fromFile(const char *path, long max = -1, bool *ok = 0);

#ifdef NRDP_HAS_OBJECTCOUNT
    inline void setObjectCountDescription(const std::string &description);
#endif
    inline std::string describe() const { return mData ? mData->ptr()->describe(mOffset, mLength) : std::string("(empty)"); }

    inline void clear();
    inline void reserve(int cap);

    template <typename T> inline void append(const T *d, int len);
    template <typename T> inline void append(T t);
    inline void append(const std::string &data);
    inline void append(const char *nullTerminated);
    inline void append(const DataBuffer &buffer);

    template <typename T> DataBuffer &operator<<(const T &t);

    template <typename T> inline void push_back(const T *d, int len) { append<T>(d, len); }
    template <typename T> inline void push_back(T t) { append<T>(t); }
    inline void push_back(const std::string &data) { append(data); }
    inline void push_back(const char *nullTerminated) { append(nullTerminated); }
    inline void push_back(const DataBuffer &buffer) { append(buffer); }

    inline int indexOf(const DataBuffer &needle, int offset=0) const { return indexOf(needle.data(), needle.size(), offset); }
    inline int indexOf(const std::string &needle, int offset=0) const { return indexOf(needle.c_str(), needle.size(), offset); }
    inline int indexOf(const void *data, int len, int offset = 0) const;

    inline int lastIndexOf(const DataBuffer &needle, int offset=-1) const { return lastIndexOf(needle.data(), needle.size(), offset); }
    inline int lastIndexOf(const std::string &needle, int offset=-1) const { return lastIndexOf(needle.c_str(), needle.size(), offset); }
    inline int lastIndexOf(const void *data, int len, int offset = -1) const;

    inline void remove(int index, int len);

    inline int replace(const DataBuffer &needle, const DataBuffer &value);
    inline void replace(int index, int len, const DataBuffer &value);

    template <typename T>
    inline void insert(int index, const T *text, int length);
    inline void insert(int index, const DataBuffer &text) { insert(index, text.constData<const unsigned char>(), text.size()); }
    inline void insert(int index, const char *nullTerminated) { insert(index, nullTerminated, strlen(nullTerminated)); }
    template <typename T> inline void insert(int index, T t) { insert(index, &t, 1); }
    inline void prepend(const DataBuffer &text) { insert(0, text); }
    template <typename T> inline void prepend(T t) { insert(0, t); }
    template <typename T> inline void prepend(const T *t, int length) { insert(0, t, length); }
    template <typename T> inline void prepend(const char *nullTerminated) { insert(0, nullTerminated, strlen(nullTerminated)); }

    inline bool contains(const DataBuffer &needle) const { return indexOf(needle) != -1; }
    inline bool startsWith(const DataBuffer &needle) const;
    inline bool endsWith(const DataBuffer &needle) const;

    inline void setUsed(int bytes);
    inline void resize(int bytes);
    inline void fill(unsigned char fill);

    inline std::string toString() const; // deep copy
    inline std::vector<unsigned char> toVector() const; // deep copy

    inline bool isSlice() const { return mOffset || mLength != sourceSize(); }
    inline int offset() const { return mOffset; }
    inline int size() const { return mLength; }
    inline int sourceSize() const;
    inline int length() const { return size(); }
    inline bool isEmpty() const;
    inline bool empty() const;
    inline int capacity() const;

    inline const char *c_str();
    inline const char *c_str() const;
    inline unsigned char *data() { return data<unsigned char>(); }
    inline const unsigned char *data() const { return data<unsigned char>(); }
    template <typename T> inline T *data();
    template <typename T> inline const T *data() const;
    inline const unsigned char *constData() const { return data(); }
    template <typename T> inline const T *constData() const { return data<T>(); }

    inline bool ownsData() const;
    inline int refCount() const;

    inline int compare(const DataBuffer &other) const;
    inline int compare(const std::string &other) const;
    inline int compare(const char *other, int size = -1) const;

    inline DataBuffer mid(int from, int len = -1) const;
    inline DataBuffer left(int count) const;
    inline DataBuffer right(int count) const;

    inline DataBuffer slice(int offset, int len = -1) const;
    DataBuffer source() const;
    inline void setOffset(int offset);
    inline void setView(int offset, int len = -1);

    template <typename T> inline DataBuffer &operator+=(T t) { append(t); return *this; }

    const unsigned char &operator[](int idx) const { return data()[idx]; }
    unsigned char &operator[](int idx) { return data()[idx]; }
    const unsigned char &at(int idx) const { return operator[](idx); }

    enum Encoding {
        Encoding_None,
        Encoding_Base64,
        Encoding_Base64_UrlSafe,
        Encoding_Url,
        Encoding_Hex,
        Encoding_Percent = Encoding_Url
    };
    bool isBinary(int max = -1) const;
    DataBuffer encode(Encoding encoding) const;
    DataBuffer decode(Encoding encoding) const;
    inline DataBuffer toBase64() const { return encode(Encoding_Base64); }
    static inline DataBuffer toBase64(const unsigned char *data, int length) { return DataBuffer::fromRawData(data, length).toBase64(); }
    static inline DataBuffer toBase64(const std::string &str) { return DataBuffer::fromRawData(str.c_str(), str.size()).toBase64(); }
    std::string toBase64String() const;
    inline DataBuffer toUrlEncoded() { return encode(Encoding_Percent); }
    inline DataBuffer toHex() const { return encode(Encoding_Hex); }
    enum SplitFlags {
        Split_None = 0x0,
        Split_KeepEmptyParts = 0x1
    };
    inline std::vector<DataBuffer> split(char delimiter, unsigned int flags = 0) const { return split(&delimiter, 1, flags); }
    inline std::vector<DataBuffer> split(const std::string &delimiter, unsigned int flags = 0) const { return split(delimiter.c_str(), delimiter.size(), flags); }
    inline std::vector<DataBuffer> split(const char *delimiter, int length, unsigned int flags = 0) const;
    static inline DataBuffer fromBase64(const unsigned char *data, int length) { return fromBase64(DataBuffer::fromRawData(data, length)); }
    static inline DataBuffer fromBase64(const DataBuffer &encoded) { return encoded.decode(Encoding_Base64); }
    static inline DataBuffer fromBase64(const std::string &encoded) { return DataBuffer::fromBase64(DataBuffer::fromRawData(encoded.c_str(), encoded.size())); }
    static inline DataBuffer fromUrl(const DataBuffer &encoded) { return encoded.decode(Encoding_Url); }
    static inline DataBuffer fromHex(const DataBuffer &encoded) { return encoded.decode(Encoding_Hex); }

    enum Hash {
        Hash_None,
        Hash_SHA1,
        Hash_SHA256,
        Hash_MD5
    };
    DataBuffer hash(Hash hash, bool *ok = 0) const;
    static int hashLength(Hash hash);
    enum CompressionMode {
        Compression_Normal,
        Compression_GZip,
        Compression_Base64
    };
    DataBuffer compress(CompressionMode mode, bool *ok = 0) const;
    DataBuffer uncompress(CompressionMode mode, bool *ok = 0) const;

    inline bool detach();

#ifdef NRDP_HAS_OBJECTCOUNT
    static Mutex &objectCountMutex();
#endif
    static long long currentTotalCapacity();

protected:
    struct Data {
        Data();
        Data(const Data &other);
        ~Data();

        std::string describe(int offset = 0, int length = -1) const;
#ifdef NRDP_HAS_OBJECTCOUNT
        std::string *description;
#endif

        unsigned char *data;
        int size, capacity;
        bool ownsData;
    private:
        Data &operator=(const Data &);
    };
    friend struct DataBufferObjectCount;
    RefCounted<Data> *mData;
    int mOffset, mLength;
};

inline DataBuffer::DataBuffer(int cap)
    : mData(0), mOffset(0), mLength(0)
{
    if (cap)
        resize(cap);
}

inline DataBuffer::DataBuffer(int cap, unsigned char f)
    : mData(0), mOffset(0), mLength(0)
{
    if (cap) {
        resize(cap);
        fill(f);
    }
}

inline DataBuffer::DataBuffer(const unsigned char *d, int len)
    : mData(0), mOffset(0), mLength(0)
{
    if (!d) {
        assert(len <= 0);
        return;
    }
    assert(len >= 0);
    if (len)
        append(d, len);
}

inline DataBuffer::DataBuffer(const char *d, int len)
    : mData(0), mOffset(0), mLength(0)
{
    if (!d) {
        assert(len <= 0);
        return;
    }
    if (len == -1)
        len = strlen(d);
    append(d, len);
}

inline DataBuffer::DataBuffer(const std::string &str)
    : mData(0), mOffset(0), mLength(0)
{
    append(str.c_str(), str.size());
}

inline DataBuffer::DataBuffer(const std::vector<unsigned char> &v)
    : mData(0), mOffset(0), mLength(0)
{
    append(&v[0], v.size());
}

inline DataBuffer::DataBuffer(const DataBuffer &other)
    : mData(other.mData), mOffset(other.mOffset), mLength(other.mLength)
{
    if (mData)
        mData->ref();
}

inline DataBuffer::~DataBuffer()
{
    clear();
}

inline DataBuffer &DataBuffer::operator=(const DataBuffer &other)
{
    clear();
    mData = other.mData;
    mOffset = other.mOffset;
    mLength = other.mLength;
    if (mData)
        mData->ref();
    return *this;
}

inline DataBuffer DataBuffer::fromRawData(const unsigned char *data, int size)
{
    DataBuffer ret;
    if (!data || !size)
        return ret;
    ret.mData = new RefCounted<Data>();
    ret.mData->ptr()->ownsData = false;
    ret.mData->ptr()->data = const_cast<unsigned char *>(data); // awful
    ret.mData->ptr()->size = ret.mLength = size;
    return ret;
}

inline DataBuffer DataBuffer::fromRawData(const char *data, int size)
{
    DataBuffer ret;
    if (!data || !size)
        return ret;
    ret.mData = new RefCounted<Data>();
    ret.mData->ptr()->ownsData = false;
    ret.mData->ptr()->data = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(data)); // awful
    ret.mData->ptr()->size = ret.mLength = size;
    return ret;
}

inline DataBuffer DataBuffer::fromFile(const char *path, long max, bool *ok)
{
    DataBuffer ret;
    FILE *f = fopen(path, "r");
    if (!f) {
        if (ok)
            *ok = false;
        return ret;
    }
    if (fseek(f, 0, SEEK_END) < 0) {
        fclose(f);
        if (ok)
            *ok = false;
        return ret;
    }
    if (ok)
        *ok = true;
#ifdef NRDP_HAS_OBJECTCOUNT
    ret.setObjectCountDescription(path);
#endif
    const long fileSize = ftell(f);
    long size = fileSize;
    if (max > 0)
        size = std::min(max, fileSize);
    if (size > 0) {
        fseek(f, 0, SEEK_SET);
        ret.reserve(size);
        while (ret.mLength < size) {
            const int read = fread(ret.mData->ptr()->data + ret.mLength, sizeof(unsigned char), size - ret.mLength, f);
            if (read <= 0)
                break;
            ret.mLength += read;
        }
        ret.mData->ptr()->size = ret.mLength;
        if (ret.mLength != size)
            ret.clear();
    }
    fclose(f);

    return ret;
}

inline void DataBuffer::clear()
{
    mOffset = mLength = 0;
    if (mData) {
        mData->deref();
        mData = 0;
    }
}

inline void DataBuffer::reserve(int cap)
{
    assert(!isSlice());
    assert(cap >= 0);
    if (cap) {
        if (!mData) {
            mData = new RefCounted<Data>();
            mData->ptr()->data = reinterpret_cast<unsigned char *>(malloc(cap + 1));
            mData->ptr()->capacity = cap;
            mData->ptr()->data[cap] = '\0';
        } else if (cap != mData->ptr()->capacity && (cap > mData->ptr()->capacity || (cap >= mLength))) {
            detach();
            mData->ptr()->data = reinterpret_cast<unsigned char *>(realloc(mData->ptr()->data, cap + 1));
            mData->ptr()->capacity = cap;
            mData->ptr()->data[cap] = '\0';
        }
    } else if (mData && !mLength) {
        clear();
    }
}

template <typename T> inline void DataBuffer::append(const T *t, int len)
{
    assert(!isSlice());
    if (len) {
        (void)static_cast<long long>(T());
        len *= sizeof(T);
        assert(t);
        if (!mData) {
            reserve(len);
        } else {
            detach();
            const int available = mData->ptr()->capacity - mData->ptr()->size;
            if (available < len)
                reserve(mData->ptr()->capacity + std::max<int>(mData->ptr()->capacity, len - available));
        }
        assert(mData);
        memcpy(mData->ptr()->data + mLength, t, len);
        mLength += len;
        mData->ptr()->size += len;
        mData->ptr()->data[mLength] = '\0';
    }
}

template <typename T> inline void DataBuffer::append(T t)
{
    (void)static_cast<long long>(T());
    append(&t, 1);
}

inline void DataBuffer::append(const DataBuffer &buffer)
{
    append(buffer.data(), buffer.size());
}

inline void DataBuffer::append(const char *nullTerminated)
{
    append(nullTerminated, strlen(nullTerminated));
}

inline void DataBuffer::append(const std::string &data)
{
    append(data.c_str(), data.size());
}

#ifdef NRDP_HAS_OBJECTCOUNT
void DataBuffer::setObjectCountDescription(const std::string &d)
{
    assert(!isSlice());
    if (!mData) {
        mData = new RefCounted<Data>();
    } else {
        detach();
        if (mData->ptr()->description)
            delete mData->ptr()->description;
    }
    mData->ptr()->description = new std::string(d);
}
#endif

template <typename T> inline DataBuffer &DataBuffer::operator<<(const T &t)
{
    append(t);
    return *this;
}

inline void DataBuffer::replace(int index, int len, const DataBuffer &value)
{
    if (!len || index + len > mLength)
        return;

    detach();

    if (!value.size()) {
        remove(index, len);
    } else if (len == mLength) {
        operator=(value);
    } else if (len == value.size()) {
        memcpy(data() + index, value.data(), len);
    } else if (len > value.size()) { // this won't release preallocation
        unsigned char *bytes = data();
        memcpy(bytes + index, value.data(), value.size());
        memmove(bytes + index + value.size(), bytes + index + len, mLength - index - len + 1);
        mLength -= (len - value.size());
        mData->ptr()->size = mLength;
    } else {
        const int diff = (value.size() - len);
        reserve(mLength + diff);
        unsigned char *bytes = data();
        memmove(bytes + index + value.size(), bytes + index + len, mLength - index - len + 1);
        memcpy(bytes + index, value.data(), value.size());
        mLength += diff;
        mData->ptr()->size = mLength;
    }
}

template <typename T> inline void DataBuffer::insert(int index, const T *text, int count)
{
    assert(count >= 0);
    if (!count)
        return;

    assert(text);
    assert(index >= 0);
    assert(index <= mLength);
    if (index == mLength) {
        append<T>(text, count);
        return;
    }

    const int byteLength = count * sizeof(T);
    resize(mLength + byteLength);
    unsigned char *bytes = data();
    assert(bytes);
    memmove(bytes + index + byteLength, bytes + index, mLength - index - byteLength + 1);
    memcpy(bytes + index, text, byteLength);
}

inline int DataBuffer::replace(const DataBuffer &needle, const DataBuffer &value)
{
    int count = 0;
    int index = 0;
    while (true) {
        index = indexOf(needle, index);
        if (index == -1)
            break;
        replace(index, needle.size(), value);
        index += value.size();
        ++count;
    }
    return count;
}

inline void DataBuffer::remove(int index, int len)
{
    len = std::min(mLength - index, len);
    if (len) {
        detach();
        if (index + len == mLength) {
            if (!index) {
                clear();
            } else {
                mLength -= len;
                mData->ptr()->size = mLength;
                mData->ptr()->data[mLength] = '\0';
            }
        } else {
            unsigned char *bytes = data();
            memmove(bytes + index, bytes + len + index, mLength - len - index + 1); // include \0
            mLength -= len;
            mData->ptr()->size = mLength;
        }
    }
}

inline int DataBuffer::indexOf(const void *needle, int hisSize, int index) const
{
    const int mySize = size();
    if (mySize - index < hisSize || !hisSize)
        return -1;

    const unsigned char *me = data();
    const unsigned char *him = static_cast<const unsigned char*>(needle);

    int matched = 0;
    for (int i = index; i < mySize; ++i) {
        if (me[i] == him[matched]) {
            if (++matched == hisSize)
                return i - matched + 1;
        } else if (mySize - i > hisSize) {
            matched = 0;
        } else {
            break;
        }
    }
    return -1;
}

inline int DataBuffer::lastIndexOf(const void *needle, int hisSize, int index) const
{
    const int mySize = size();
    if (!hisSize || hisSize > mySize)
        return -1;
    if (index == -1)
        index = mySize - hisSize;

    const char *haystack = data<const char>();

    while (index >= 0) {
        if (!memcmp(haystack + index, needle, hisSize))
            return index;
        --index;
    }
    return -1;
}

inline bool DataBuffer::startsWith(const DataBuffer &needle) const
{
    const int me = size();
    if (!me)
        return false;
    const int him = needle.size();
    if (!him)
        return false;
    if (him > me)
        return false;
    return !memcmp(data(), needle.data(), him);
}

inline bool DataBuffer::endsWith(const DataBuffer &needle) const
{
    const int me = size();
    if (!me)
        return false;
    const int him = needle.size();
    if (!him)
        return false;
    if (him > me)
        return false;
    return !memcmp(data() + (me - him), needle.data(), him);
}

inline std::string DataBuffer::toString() const // inefficient
{
    return mData ? std::string(data<const char>(), size()) : std::string();
}

inline std::vector<unsigned char> DataBuffer::toVector() const // inefficient
{
    if (mData) {
        const unsigned char *d = mData->ptr()->data;
        return std::vector<unsigned char>(d, d + mLength);
    }
    return std::vector<unsigned char>();
}

inline int DataBuffer::sourceSize() const
{
    return mData ? mData->ptr()->size : 0;
}

inline bool DataBuffer::isEmpty() const
{
    return !size();
}

inline bool DataBuffer::empty() const
{
    return isEmpty();
}

inline const char *DataBuffer::c_str() const
{
    assert(mOffset + mLength == sourceSize());
    const char *str = data<char>();
    return str ? str : "";
}

inline const char *DataBuffer::c_str()
{
    if (isSlice())
        detach();
    return static_cast<const DataBuffer*>(this)->c_str();
}

template <typename T> inline T *DataBuffer::data()
{
    NRDP_STATIC_ASSERT(sizeof(T) == 1);
    if (detach())
        return reinterpret_cast<T*>(mData->ptr()->data + mOffset);
    return 0;
}

template <typename T> inline const T *DataBuffer::data() const
{
    NRDP_STATIC_ASSERT(sizeof(T) == 1);
    return reinterpret_cast<const T *>(mData ? mData->ptr()->data + mOffset : 0);
}

inline int DataBuffer::capacity() const
{
    return mData ? mData->ptr()->capacity : 0;
}

inline void DataBuffer::setUsed(int size)
{
    assert(!isSlice());
    detach();
    assert(mData);
    assert(size <= mData->ptr()->capacity);
    mData->ptr()->size = mLength = size;
    if (size > 0)
        mData->ptr()->data[size] = '\0';
}

inline void DataBuffer::resize(int size)
{
    assert(size >= 0);
    if (!size) {
        clear();
        return;
    }

    reserve(size);
    mData->ptr()->size = mLength = size;
}

inline void DataBuffer::fill(unsigned char f)
{
    if (detach()) {
        memset(mData->ptr()->data + mOffset, f, mLength);
    }
}

inline bool DataBuffer::ownsData() const
{
    return mData && mData->ptr()->ownsData;
}

inline int DataBuffer::refCount() const
{
    return mData && static_cast<int>(mData->count());
}

inline bool DataBuffer::isBinary(int max) const
{
    const int length = max == -1 ? size() : max;
    const unsigned char *ch = data();
    for (int i = 0; i < length; ++i) {
        if (!isprint(ch[i]) && !isspace(ch[i]))
            return true;
    }
    return false;
}

inline int DataBuffer::compare(const DataBuffer &other) const
{
    return compare(other.data<const char>(), other.size());
}

inline int DataBuffer::compare(const std::string &other) const
{
    return compare(other.c_str(), other.size());
}

inline int DataBuffer::compare(const char *him, int hisLength) const
{
    if (!mData) {
        return !him ? 0 : -1;
    } else if (!him) {
        return 1;
    }
    if (hisLength == -1)
        hisLength = strlen(him);
    if (mData->ptr()->data + mOffset == reinterpret_cast<const unsigned char *>(him) && hisLength == mLength)
        return 0;

    const char *me = reinterpret_cast<const char *>(mData->ptr()->data) + mOffset;
    const int minLength = std::min(mLength, hisLength);
    int ret = memcmp(me, him, minLength);
    if (!ret) {
        if (mLength < hisLength) {
            ret = -1;
        } else if (hisLength < mLength) {
            ret = 1;
        }
    }
    return ret;
}

inline DataBuffer DataBuffer::mid(int from, int len) const
{
    assert(from >= 0);
    const int s = size();
    if (!s || from >= s)
        return DataBuffer();
    if (len == -1) {
        len = s - from;
    } else if ((static_cast<long long>(from) + len) > s) {
        len = s - from;
    }

    if (!from && len == s && !isSlice())
        return *this;
    return DataBuffer(data() + from, len);
}

inline DataBuffer DataBuffer::left(int count) const
{
    if (count <= 0)
        return DataBuffer();
    const int s = size();
    if (count >= s && !isSlice())
        return *this;
    return DataBuffer(data(), count);
}

inline DataBuffer DataBuffer::right(int count) const
{
    if (count <= 0)
        return DataBuffer();
    const int s = size();
    if (count >= s && !isSlice())
        return *this;
    return DataBuffer(data() + (s - count), s - count);
}

inline DataBuffer DataBuffer::slice(int offset, int len) const
{
    if (len == -1)
        len = mLength - offset;
    assert(offset >= 0);
    assert(len >= 0);
    assert(offset + len <= mLength);
    DataBuffer ret = *this;
    ret.mOffset = offset + mOffset;
    ret.mLength = len;
    return ret;
}

inline void DataBuffer::setOffset(int offset)
{
    assert(mData);
    mOffset += offset;
    mLength -= offset;
    assert(mOffset >= 0 && mOffset + mLength <= sourceSize());
}

inline void DataBuffer::setView(int offset, int len)
{
    assert(mData);
    mOffset += offset;
    if (len == -1)
        len = mLength - mOffset;
    mLength = len;
    assert(mOffset >= 0 && mOffset + mLength <= sourceSize());
}

inline DataBuffer DataBuffer::source() const
{
    if (mOffset || mLength != sourceSize()) {
        assert(mData);
        DataBuffer ret;
        ret.mData = mData;
        ret.mData->ref();
        return ret;
    }
    return *this;
}

inline std::vector<DataBuffer> DataBuffer::split(const char *delimiter, int delimiterLength, unsigned int flags) const
{
    assert(delimiter);
    assert(delimiterLength > 0);
    std::vector<DataBuffer> ret;
    const DataBuffer needle = DataBuffer::fromRawData(delimiter, delimiterLength);
    int last = 0;
    const int len = size();
    while (1) {
        const int next = indexOf(needle, last);
        if (next == -1) {
            if (last + 1 < len || flags & Split_KeepEmptyParts)
                ret.push_back(mid(last));
            break;
        }
        if (next > last || flags & Split_KeepEmptyParts)
            ret.push_back(mid(last, next - last));
        last = next + 1;
    }
    return ret;
}

inline bool DataBuffer::detach()
{
    if (mData) {
        if (isSlice()) {
            operator=(DataBuffer(constData(), mLength));
            if (!mData)
                mData = new RefCounted<Data>();
            return true;
        } else {
            const RefCounted<Data>::DetachMode mode = mData->ptr()->ownsData ? RefCounted<Data>::Count : RefCounted<Data>::Force;
            mData = static_cast<RefCounted<Data> *>(mData)->detach(mode);
        }
        return true;
    }
    return false;
}

inline DataBuffer operator+(const DataBuffer &l, const DataBuffer &r)
{
    DataBuffer ret;
    ret.reserve(l.size() + r.size());
    ret.append(l);
    ret.append(r);
    return ret;
}

inline bool operator==(const DataBuffer &l, const DataBuffer &r) { return !l.compare(r); }
inline bool operator==(const DataBuffer &l, const std::string &r) { return !l.compare(r); }
inline bool operator==(const std::string &l, const DataBuffer &r) { return !r.compare(l); }
inline bool operator==(const DataBuffer &l, const char *r) { return !l.compare(r); }
inline bool operator==(const char *l, const DataBuffer &r) { return !r.compare(l); }

inline bool operator!=(const DataBuffer &l, const DataBuffer &r) { return l.compare(r) != 0; }
inline bool operator!=(const DataBuffer &l, const std::string &r) { return l.compare(r) != 0; }
inline bool operator!=(const std::string &l, const DataBuffer &r) { return r.compare(l) != 0; }
inline bool operator!=(const DataBuffer &l, const char *r) { return l.compare(r) != 0; }
inline bool operator!=(const char *l, const DataBuffer &r) { return r.compare(l) != 0; }

inline bool operator<(const DataBuffer &l, const DataBuffer &r) { return l.compare(r) < 0; }
inline bool operator<(const DataBuffer &l, const std::string &r) { return l.compare(r) < 0; }
inline bool operator<(const std::string &l, const DataBuffer &r) { return r.compare(l) > 0; }
inline bool operator<(const DataBuffer &l, const char *r) { return l.compare(r) < 0; }
inline bool operator<(const char *l, const DataBuffer &r) { return r.compare(l) > 0; }

inline bool operator>(const DataBuffer &l, const DataBuffer &r) { return l.compare(r) > 0; }
inline bool operator>(const DataBuffer &l, const std::string &r) { return l.compare(r) > 0; }
inline bool operator>(const std::string &l, const DataBuffer &r) { return r.compare(l) < 0; }
inline bool operator>(const DataBuffer &l, const char *r) { return l.compare(r) > 0; }
inline bool operator>(const char *l, const DataBuffer &r) { return r.compare(l) < 0; }

} // namespace netflix

#endif
