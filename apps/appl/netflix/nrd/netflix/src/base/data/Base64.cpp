/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <iterator>
#include "Base64.h"

namespace netflix {
namespace Base64 {

std::string encode(const std::string& s, bool urlSafe)
{
    std::string value;
    value.reserve(encode_reserve(s.size()));
    encode(s.begin(), s.end(), std::back_inserter(value), urlSafe);
    return value;
}

std::string decode(const std::string& s)
{
    std::string value;
    value.reserve(decode_reserve(s.size()));
    decode(s.begin(), s.end(), std::back_inserter(value));
    return value;
}

DataBuffer encode(const DataBuffer& s, bool urlSafe)
{
    DataBuffer value;
    value.reserve(encode_reserve(s.size()));
    const char *data = s.c_str();
    encode(data, data + s.size(), std::back_inserter(value), urlSafe);
    return value;
}

DataBuffer decode(const DataBuffer& s)
{
    DataBuffer value;
    value.reserve(decode_reserve(s.size()));
    const char *data = s.c_str();
    decode(data, data + s.size(), std::back_inserter(value));
    return value;
}

std::vector<unsigned char> encode(const std::vector<unsigned char> &v, bool urlSafe)
{
    std::vector<unsigned char> value;
    value.reserve(encode_reserve(v.size()));
    encode(v.begin(), v.end(), std::back_inserter(value), urlSafe);
    return value;
}

std::vector<unsigned char> decode(const std::vector<unsigned char> &v)
{
    std::vector<unsigned char> value;
    value.reserve(decode_reserve(v.size()));
    decode(v.begin(), v.end(), std::back_inserter(value));
    return value;
}

} // namespace Base64
} // namespace netflix
