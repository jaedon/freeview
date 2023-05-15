/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DataBuffer.h"
#include "StringCompressor.h"
#include <nrdbase/Base64.h>
#include <nrdbase/UrlEncoder.h>
#include <nrdbase/ObjectCount.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <iterator>

using namespace netflix;

namespace netflix {
struct DataBufferObjectCount : public netflix::ObjectCount::Record
{
public:
    inline DataBufferObjectCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    virtual size_t cost(void *object) const {
        DataBuffer::Data *data = static_cast<DataBuffer::Data*>(object);
        return (data->ownsData && data->capacity) ? data->capacity+1 : 0;
    }
    virtual std::string describe(void *object) const { return static_cast<DataBuffer::Data*>(object)->describe(); }
};
}

NRDP_DEFINE_OBJECTCOUNT_RECORD(DataBufferObjectCount, DataBuffer);

std::string DataBuffer::Data::describe(int offset, int length) const
{
    if (length == -1)
        length = size - offset;
    length = std::min(length, 128);

    std::string result;
#ifdef NRDP_HAS_OBJECTCOUNT
    if (description) {
        result = StringFormatter::sformat("%p %d [%s]", this, capacity+1, description->c_str());
    } else
#endif
    {
        result = StringFormatter::sformat("%p %d ", this, capacity+1);
        if (size) {
            bool isBinary = false;
            for(int i = offset; i < length; ++i) {
                if (!isprint(data[i]) && !isspace(data[i])) {
                    isBinary = true;
                    break;
                }
            }
            if (isBinary) {
                result += Log::hexDump(data+offset, length);
            } else {
                const char *ch = reinterpret_cast<const char*>(data) + offset;
                length = std::min<int>(length, strlen(ch));
                result.append(ch, length);
            }
        }
    }
    return result;
}

DataBuffer::Data::Data() :
#ifdef NRDP_HAS_OBJECTCOUNT
    description(0),
#endif
    data(0), size(0), capacity(0), ownsData(true)
{
    NRDP_OBJECTCOUNT_REF(DataBuffer, this);
}

DataBuffer::Data::Data(const Data &other) :
#ifdef NRDP_HAS_OBJECTCOUNT
    description(0),
#endif
    data(0), size(other.size), capacity(other.capacity), ownsData(true)
{
    if (!other.ownsData) {
        capacity = other.size;
        data = reinterpret_cast<unsigned char*>(malloc(capacity + 1));
    } else if (other.capacity) {
        data = reinterpret_cast<unsigned char*>(malloc(capacity + 1));
    }
    if (other.size)
        memcpy(data, other.data, other.size);
#ifdef NRDP_HAS_OBJECTCOUNT
    if (other.description)
        description = new std::string(*other.description);
#endif
    if (capacity)
        data[size] = '\0';

    NRDP_OBJECTCOUNT_REF(DataBuffer, this);
}


DataBuffer::Data::~Data()
{
    NRDP_OBJECTCOUNT_DEREF(DataBuffer, this);
    if (data && ownsData && capacity)
        free(data);
#ifdef NRDP_HAS_OBJECTCOUNT
    delete description;
#endif
}

std::string DataBuffer::toBase64String() const
{
    std::string result;
    result.reserve(Base64::encode_reserve(size()));
    const char * source(data<const char>());
    Base64::encode(source, source + size(), std::back_inserter(result));
    return result;
}

DataBuffer DataBuffer::compress(CompressionMode mode, bool *ok) const
{
    bool success = false;
    DataBuffer ret;
    switch (mode) {
    case Compression_Normal:
        success = (StringCompressor::deflate(ret, *this) == NFErr_OK);
        break;
    case Compression_GZip:
        success = (StringCompressor::deflateGzip(ret, *this) == NFErr_OK);
        break;
    case Compression_Base64:
        success = (StringCompressor::deflateB64(ret, *this) == NFErr_OK);
        break;
    }
    if (ok)
        *ok = success;
    return ret;
}

DataBuffer DataBuffer::uncompress(CompressionMode mode, bool *ok) const
{
    DataBuffer ret;
    bool success = false;
    switch (mode) {
    case Compression_Normal:
        success = (StringCompressor::inflate(ret, *this) == NFErr_OK);
        break;
    case Compression_GZip:
        success = (StringCompressor::inflateGzip(ret, *this) == NFErr_OK);
        break;
    case Compression_Base64:
        success = (StringCompressor::inflateB64(ret, *this) == NFErr_OK);
        break;
    }
    if (ok)
        *ok = success;
    return ret;
}

DataBuffer DataBuffer::encode(Encoding encoding) const
{
    if (!isEmpty()) {
        switch (encoding) {
        case Encoding_None: break;
        case Encoding_Base64_UrlSafe: return Base64::encode(*this, true);
        case Encoding_Base64: return Base64::encode(*this);
        case Encoding_Url: return UrlEncoder::encode<DataBuffer>(data<const char>(), size());
        case Encoding_Hex: {
            const char hex[] = "0123456789ABCDEF";
            int s = size();
            DataBuffer ret(s * 2);
            const unsigned char *in = data();
            unsigned char *out = ret.data();
            while (s--) {
                assert(in);
                assert(out);
                *out++ = hex[(*in) >> 4];
                assert(out);
                *out++ = hex[(*in) & 0x0F];
                ++in;
            }

            return ret; }
        }
    }
    return DataBuffer();
}

DataBuffer DataBuffer::decode(Encoding encoding) const
{
    if (!isEmpty()) {
        switch (encoding) {
        case Encoding_None:
            break;
        case Encoding_Base64_UrlSafe:
        case Encoding_Base64: return Base64::decode(*this);
        case Encoding_Url: return UrlEncoder::decode<DataBuffer>(data<const char>(), size());
        case Encoding_Hex: {
            int s = size();
            if (s % 2 != 0)
                return DataBuffer();
            DataBuffer ret(s / 2);
            const unsigned char *in = data();
            assert(in);
            unsigned char *out = ret.data();
            assert(out);
            while (s--) {
                int val;
                assert(in);
                switch (*in++) {
                case '0': val = 0; break;
                case '1': val = 1; break;
                case '2': val = 2; break;
                case '3': val = 3; break;
                case '4': val = 4; break;
                case '5': val = 5; break;
                case '6': val = 6; break;
                case '7': val = 7; break;
                case '8': val = 8; break;
                case '9': val = 9; break;
                case 'A':
                case 'a': val = 10; break;
                case 'B':
                case 'b': val = 11; break;
                case 'C':
                case 'c': val = 12; break;
                case 'D':
                case 'd': val = 13; break;
                case 'E':
                case 'e': val = 14; break;
                case 'F':
                case 'f': val = 15; break;
                default: return DataBuffer();
                }
                if (s % 2 == 0) {
                    *out += val;
                    ++out;
                } else {
                    *out = (val << 4);

                }

            }
            return ret; }
        }
    }
    return DataBuffer();
}

int DataBuffer::hashLength(Hash hash)
{
    switch (hash) {
    case Hash_None:
        break;
    case Hash_SHA1:
        return SHA_DIGEST_LENGTH;
    case Hash_SHA256:
        return SHA256_DIGEST_LENGTH;
    case Hash_MD5:
        return MD5_DIGEST_LENGTH;
    }
    return 0;
}

DataBuffer DataBuffer::hash(Hash hash, bool *ok) const
{
    if (ok)
        *ok = true;
    DataBuffer ret;
    if (!isEmpty()) {
        switch (hash) {
        case Hash_None:
            break;
        case Hash_SHA1: {
            SHA_CTX ctx;
            if (!SHA1_Init(&ctx)) {
                if (ok)
                    *ok = false;
                break;
            }
            if (!SHA1_Update(&ctx, data<const char>(), size())) {
                unsigned char buf[SHA_DIGEST_LENGTH];
                SHA1_Final(buf, &ctx);
                if (ok)
                    *ok = false;
                break;
            }
            ret.resize(SHA_DIGEST_LENGTH);
            if (!SHA1_Final(ret.data(), &ctx)) {
                if (ok)
                    *ok = false;
                ret.clear();
            }
            break; }
        case Hash_SHA256: {
            SHA256_CTX ctx;
            if (!SHA256_Init(&ctx)) {
                if (ok)
                    *ok = false;
                break;
            }
            if (!SHA256_Update(&ctx, data<const char>(), size())) {
                unsigned char buf[SHA256_DIGEST_LENGTH];
                SHA256_Final(buf, &ctx);
                if (ok)
                    *ok = false;
                break;
            }
            ret.resize(SHA256_DIGEST_LENGTH);
            if (!SHA256_Final(ret.data(), &ctx)) {
                if (ok)
                    *ok = false;
                ret.clear();
            }
            break; }
        case Hash_MD5: {
            MD5_CTX ctx;
            if (!MD5_Init(&ctx)) {
                if (ok)
                    *ok = false;
                break;
            }
            if (!MD5_Update(&ctx, data<const char>(), size())) {
                unsigned char buf[SHA_DIGEST_LENGTH];
                MD5_Final(buf, &ctx);
                if (ok)
                    *ok = false;
                break;
            }
            ret.reserve(MD5_DIGEST_LENGTH);
            ret.setUsed(MD5_DIGEST_LENGTH);
            if (!MD5_Final(ret.data(), &ctx)) {
                if (ok)
                    *ok = false;
                ret.clear();
            }
            break; }

        }
    }
    return ret;
}

long long DataBuffer::currentTotalCapacity()
{
    return ObjectCount::DataBuffer.currentCost();
}

#ifdef NRDP_HAS_OBJECTCOUNT
Mutex &DataBuffer::objectCountMutex()
{
    return ObjectCount::DataBuffer.mutex;
}

#endif
