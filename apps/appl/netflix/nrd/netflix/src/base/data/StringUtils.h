/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <nrdbase/StringFormatter.h>

#include <algorithm>
#include <string>
#include <functional>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <string.h>
#include <nrdbase/tr1.h>

namespace netflix {

inline std::string prettyNumber(long long value)
{
    std::string result;
    while(value >= 1000) {
        result.insert(0, StringFormatter::sformat(",%03d", int(value % 1000)));
        value /= 1000;
    }
    result.insert(0, StringFormatter::sformat("%d", int(value % 1000)));
    return result;
}

inline std::string prettyBytes(double size)
{
    const char *suffix = "b";
    if(fabs(size) >= 1024) {
        size /= 1024;
        suffix = "Kb";
    }
    if(fabs(size) >= 1024) {
        size /= 1024;
        suffix = "Mb";
    }
    if(fabs(size) >= 1024) {
        size /= 1024;
        suffix = "Gb";
    }
    std::ostringstream str;
    if(size == (int)size)
        str << int(size);
    else
        str << std::setiosflags(std::ios::fixed) << std::setprecision(3) << size;
    str << suffix;
    return str.str();
}

inline std::string prettyMegaBytes(double size)
{
    size /= (1024 * 1024);
    std::ostringstream str;
    if(size == (int)size)
        str << int(size);
    else
        str << std::setiosflags(std::ios::fixed) << std::setprecision(3) << size;
    str << "Mb";
    return str.str();
}

enum Format {
    Format_None,
    Format_Number,
    Format_Bytes,
    Format_MegaBytes
};

inline std::string prettyFormat(Format format, long long value)
{
    if(format == Format_Number)
        return prettyNumber(value);
    else if(format == Format_Bytes)
        return prettyBytes(value);
    else if(format == Format_MegaBytes)
        return prettyMegaBytes(value);
    return StringFormatter::sformat("%lld", value);
}

inline std::string prettyFormat(Format format, long long value, long long prev)
{
    if (value != prev)
        return StringFormatter::sformat("%s (%s%s)", prettyFormat(format, value).c_str(),
                                        value < prev ? "" : "+", prettyFormat(format, value - prev).c_str());
    return StringFormatter::sformat("%s", prettyFormat(format, value).c_str());
}

inline int strCaseCmp(std::string const& strOne, std::string const& strTwo)
{
    return strcasecmp( strOne.c_str(), strTwo.c_str() );
}

inline int replaceAll(std::string &haystack, const std::string &from, const std::string &to,
                      size_t start = 0, size_t limit = std::string::npos)
{
    size_t s = start;
    int count = 0;
    while ((s = haystack.find(from, s)) != std::string::npos) {
        if (limit != std::string::npos && s > limit)
            break;
        if (to.empty()) {
            haystack.erase(s, from.size());
        } else {
            haystack.replace(s, from.size(), to);
        }
        s += to.size();
        ++count;
    }
    return count;
}

inline int removeAll(std::string &haystack, const std::string &needle, size_t from = 0, size_t limit = std::string::npos)
{
    return replaceAll(haystack, needle, std::string(), from, limit);
}

inline bool contains(const std::string &hayStack, const char *needle, size_t needleLen)
{
    return hayStack.find(needle, 0, needleLen) != std::string::npos;
}

inline bool startsWith(const std::string &hayStack, const char *needle, size_t needleLen)
{
    return !strncmp(hayStack.c_str(), needle, needleLen);
}

inline bool startsWithIgnoreCase(const std::string &hayStack, const char *needle, size_t needleLen)
{
    return !strncasecmp(hayStack.c_str(), needle, needleLen);
}

inline bool endsWith(const std::string &hayStack, const char *needle, size_t needleLen)
{
    return hayStack.size() >= needleLen && !strncmp(hayStack.c_str() + (hayStack.size() - needleLen), needle, needleLen);
}

inline bool endsWith(const std::string &hayStack, char needle)
{
    return endsWith(hayStack, &needle, 1);
}

inline bool startsWith(const std::string &hayStack, const std::string &needle)
{
    return startsWith(hayStack, needle.c_str(), needle.size());
}

inline bool endsWith(const std::string &hayStack, const std::string &needle)
{
    return endsWith(hayStack, needle.c_str(), needle.size());
}

template<typename Container, typename Key>
inline bool contains(const Container& container, const Key& key)
{
    return std::find(container.begin(), container.end(), key) != container.end();
}

template<>
inline bool contains(const std::string& hayStack, const std::string &needle)
{
    return contains(hayStack, needle.c_str(), needle.size());
}

inline bool contains(const std::string& hayStack, const char *needle)
{
    return contains(hayStack, needle, strlen(needle));
}

template <typename Key, typename Value>
inline Value mapValue(const std::map<Key, Value> &hayStack, const Key &needle, const Value &defaultValue = Value())
{
    typename std::map<Key, Value>::const_iterator it = hayStack.find(needle);
    return it == hayStack.end() ? defaultValue : it->second;
}

template <typename Key, typename Value>
inline std::vector<Key> mapKeys(const std::map<Key, Value> &map)
{
    std::vector<Key> ret;
    ret.reserve(map.size());
    typename std::map<Key, Value>::const_iterator it = map.begin();
    typename std::map<Key, Value>::const_iterator end = map.end();
    while (it != end) {
        ret.push_back(it->first);
        ++it;
    }

    return ret;
}

inline void trim(std::string &value)
{
    const char *str = value.c_str();
    size_t size = value.size();
    size_t start = 0;
    while (start < size && std::isspace((unsigned char)str[start]))
        ++start;
    if (start == size) {
        value.clear();
        return;
    }
    size_t end = size - 1;
    while (end > start && std::isspace((unsigned char)str[end]))
        --end;
    value = value.substr(start, end - start + 1);
}

inline std::string trimmed(std::string value)
{
    trim(value);
    return value;
}

inline void lowerCase(std::string &value)
{
    std::transform(value.begin(), value.end(), value.begin(), std::ptr_fun<int,int>(std::tolower));
}

inline std::string toLower(const std::string &value)
{
    std::string ret = value;
    lowerCase(ret);
    return ret;
}

inline std::string join(const std::vector<std::string> &strings, const std::string &separator = " ")
{
    std::string ret;
    for (std::vector<std::string>::const_iterator it = strings.begin(); it != strings.end(); ++it) {
        if (it != strings.begin())
            ret += separator;
        ret += *it;
    }
    return ret;
}

template <typename T>
static int binarySearchString(const std::string &name, const T &t, int count)
{
    const char *needle = name.c_str();
    switch (count) {
    case 0:
        return -1;
    case 1:
        return strcmp(t[0], needle) ? -1 : 0;
    default:
        break;
    }

    int left = 0;
    int right = count - 1;

    do {
        const int idx = (left + right) / 2;
        const int c = strcmp(t[idx], needle);
        if (!c) {
            return idx;
        } else if (c < 0) {
            left = idx + 1;
        } else {
            right = idx - 1;
        }
    } while (left <= right);

    return -1;
}


}
#endif //STRING_UTILS_H
