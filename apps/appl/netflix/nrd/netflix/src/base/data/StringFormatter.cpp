/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <stdlib.h>
#include <cstdio>
#include <string>
#include <vector>

#include "StringFormatter.h"
#include <nrdbase/Log.h>

using namespace netflix;

std::string StringFormatter::sformat(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string str = vsformat(fmt, args);
    va_end(args);
    return str;
}

std::string StringFormatter::vsformat(const char *fmt, va_list args)
{
    va_list args2;
    va_copy(args2, args);

    enum { BufferSize = 256 };
    char staticBuffer[BufferSize];
    char *buf = staticBuffer;
    int size = vsnprintf(staticBuffer, BufferSize, fmt, args);
    if (size >= BufferSize) {
        buf = reinterpret_cast<char*>(malloc(size + 1));
        size = vsnprintf(buf, size+1, fmt, args2);
    }
    buf[size] = 0;
    va_end(args2);
    const std::string ret = buf;
    if (buf != staticBuffer)
        free(buf);
    return ret;
}
