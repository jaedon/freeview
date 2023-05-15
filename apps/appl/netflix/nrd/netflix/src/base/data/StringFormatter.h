/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef StringFormatter_h
#define StringFormatter_h

#include <cstdarg>
#include <string>

#ifdef __GNUC__
#define _NETFLIX_PRINTF_ATTR(x,y) __attribute__ ((__format__ (__printf__, x, y)))
#else
#define _NETFLIX_PRINTF_ATTR(x,y)
#endif

namespace netflix {

class StringFormatter
{
public:
    /**
     * Create a std::string using C-style string formatting.
     *
     * @param[in] fmt C-style format string.
     * @param[in] ... format string arguments.
     * @return the resulting string.
     */
    static std::string sformat(const char *fmt, ...) _NETFLIX_PRINTF_ATTR(1, 2);

    /**
     * Create a std::string using C-style string formatting.
     *
     * @param[in] fmt C-style format string.
     * @param[in] args format string variable arguments list.
     * @return the resulting string.
     */
    static std::string vsformat(const char *fmt, va_list args);

private:
    /** Private constructor. */
    StringFormatter();
};

} // namespace netflix

#endif
