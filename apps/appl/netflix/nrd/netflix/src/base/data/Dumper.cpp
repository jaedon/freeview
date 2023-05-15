/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Dumper.h"

using namespace netflix;

Dumper::Keys::Keys(Dumper* dumper)
    : mDumper(dumper)
{
}

Dumper::Keys& Dumper::Keys::operator<<(const Key& key)
{
    mDumper->mKeys.push_back(key);
    mDumper->mFlags[key.key] = key.flags;
    return *this;
}

Dumper::Values::Values(Dumper* dumper, const std::string& key)
    : mData(new Data)
{
    mData->keyIdx = 0;
    mData->dumper = dumper;
    mData->key = key;
}

Dumper::Values::Data::~Data()
{
    Variant& data = *dumper->mVariantPtr;
    if (key.empty()) {
        assert(data.isNull() || data.isArray());
        data.push_back(variant);
    } else {
        assert(data.isNull() || data.isStringMap());
        data[key] = variant;
    }
}

Dumper::Values& Dumper::Values::operator<<(const Variant& value)
{
    assert(mData->keyIdx < mData->dumper->mKeys.size());
    const Key& k = mData->dumper->mKeys[mData->keyIdx++];
    mData->variant[k.key] = value;
    return *this;
}

static inline Variant localValues(const Variant& v)
{
    Variant ret;
    if (v.isNull())
        return ret;
    assert(v.isStringMap());
    Variant::StringMapIterator it = v.stringMapBegin();
    const Variant::StringMapIterator end = v.stringMapEnd();
    while (it != end) {
        if (it->first != "values")
            ret[it->first] = it->second;
        ++it;
    }
    return ret;
}

void Dumper::next(const std::string& name)
{
    assert(mVariant.isNull() || mVariant.isStringMap());
    mVariantPtr = &mVariant[name];
    mKeys.clear();
    mFlags.clear();
}

std::vector<std::string> Dumper::toConsole()
{
    Variant& data = mVariant["values"];

    std::vector<std::string> str;
    unsigned int idx = 0;
    assert(data.isNull() || data.isArray() || data.isStringMap());
    if (data.isArray()) {
        Variant::ArrayIterator it = data.arrayBegin();
        const Variant::ArrayIterator end = data.arrayEnd();
        while (it != end) {
            const std::vector<std::string> entries = consoleEntry(idx++, std::string(), *it);
            if (!entries.empty())
                str.insert(str.end(), entries.begin(), entries.end());
            ++it;
        }
    } else {
        Variant::StringMapIterator it = data.stringMapBegin();
        const Variant::StringMapIterator end = data.stringMapEnd();
        while (it != end) {
            const std::vector<std::string> entries = consoleEntry(idx++, it->first, it->second);
            if (!entries.empty())
                str.insert(str.end(), entries.begin(), entries.end());
            ++it;
        }
    }
    const std::vector<std::string>& post = consolePostProcess(localValues(mVariant));
    if (!post.empty())
        str.insert(str.end(), post.begin(), post.end());
    return str;
}

std::vector<std::string> Dumper::toHTML()
{
    Variant& data = mVariant["values"];

    std::vector<std::string> str;
    unsigned int idx = 0;
    assert(data.isNull() || data.isArray() || data.isStringMap());
    if (data.isArray()) {
        Variant::ArrayIterator it = data.arrayBegin();
        const Variant::ArrayIterator end = data.arrayEnd();
        while (it != end) {
            const std::vector<std::string> entries = htmlEntry(idx++, std::string(), *it);
            if (!entries.empty())
                str.insert(str.end(), entries.begin(), entries.end());
            ++it;
        }
    } else {
        Variant::StringMapIterator it = data.stringMapBegin();
        const Variant::StringMapIterator end = data.stringMapEnd();
        while (it != end) {
            const std::vector<std::string> entries = htmlEntry(idx++, it->first, it->second);
            if (!entries.empty())
                str.insert(str.end(), entries.begin(), entries.end());
            ++it;
        }
    }
    const std::vector<std::string>& post = htmlPostProcess(localValues(mVariant));
    if (!post.empty())
        str.insert(str.end(), post.begin(), post.end());
    return str;
}
