/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef Splash_h
#define Splash_h

#include <vector>
#include <string>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#include <sys/stat.h>

namespace netflix {

static inline std::string splashCatalogFileName() { return "splash-catalog"; }

struct Splash
{
    inline Splash() : width(0), height(0) {}
    inline bool operator==(const Splash &other) const
    {
        return (data == other.data
                && width == other.width
                && height == other.height);

    }
    inline bool operator!=(const Splash &other) const
    {
        return !operator==(other);
    }

    static std::vector<Splash> read(const std::string &directory, const std::vector<unsigned char> &secret);
    static std::vector<netflix::Splash>::const_iterator findBestSplash(const std::vector<netflix::Splash> &splashes, int width, int height);

    std::vector<unsigned char> data;
    int width, height;
};

}

#endif

