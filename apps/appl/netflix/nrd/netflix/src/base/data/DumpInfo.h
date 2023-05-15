/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DumpInfo_h
#define DumpInfo_h

#include <string>
#include <nrdbase/StringUtils.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/Variant.h>
#include <nrdbase/Log.h>

namespace netflix {

struct DumpInfo
{
    inline DumpInfo()
        : ptr(0), idx(-1), flags(None)
    {}
    inline DumpInfo(const std::string &info)
        : ptr(0), idx(-1), flags(None)
    {
        set(info);
    }

    inline bool empty() const
    {
        return ptr == 0 && idx == -1 && match.empty();
    }
    inline void set(const std::string &info)
    {
        if (startsWith(info, "0x")) {
            ptr = reinterpret_cast<void*>((uintptr_t)strtoll(info.c_str(), 0, 16));
        } else if (info[0] >= '0' && info[0] <= '9') {
            idx = atoi(info.c_str());
        } else {
            match = info;
        }
    }
    std::string match;
    void *ptr;
    int idx;
    enum Flag {
        None = 0x0,
        Raw = 0x1,
        Data = 0x2
    };
    unsigned int flags;
};

static void dump(const TraceArea &area, Variant *variant, const char *fmt, ...) _NETFLIX_PRINTF_ATTR(3,4);
static inline void dump(const TraceArea &area, Variant *variant, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    if (variant) {
        variant->push_back(StringFormatter::vsformat(fmt, args));
    } else {
        Log::Message msg(area, Log::Warn);
        msg.m_message = StringFormatter::vsformat(fmt, args);
        msg.send();
    }
    va_end(args);
}

}

#endif
