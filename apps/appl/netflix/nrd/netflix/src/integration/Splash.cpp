/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Buffer.h"
#include "Splash.h"

#include <nrdbase/tr1.h>

// #define NRDIL_LOG_ENABLED
#ifdef NRDIL_LOG_ENABLED
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

using namespace netflix;

std::vector<Splash> Splash::read(const std::string &directory, const std::vector<unsigned char> &secret)
{
    if (directory.empty() || secret.empty())
        return std::vector<Splash>();

    std::string filename;
    if (directory[0] != '/') {
        char buf[1024];
        if (!getcwd(buf, sizeof(buf) - 1))
            return std::vector<Splash>();
        const int len = strlen(buf);
        buf[len] = '/';
        filename.assign(buf, len + 1);
        filename += directory;
    } else {
        filename = directory;
    }
    if (filename[directory.size() - 1] != '/')
        filename += '/';
    filename += splashCatalogFileName();
    LOG("Filename is %s\n", fileName.c_str());

    Buffer buffer;
    const Buffer::Status status = buffer.open(filename, secret);
    if (status == Buffer::Ok && !buffer.isEmpty()) {
        std::vector<Splash> ret;
        int count;
        buffer >> count;
        if (count > 0) {
            ret.resize(count);
            for (int i = 0; i < count && !buffer.hasError(); ++i) {
                Splash &splash = ret[i];
                buffer >> splash.width >> splash.height >> splash.data;
            }
        }
        if (!buffer.hasError())
            return ret;
    }
    return std::vector<Splash>();
}

static inline int square(int n) { return n * n; }

std::vector<netflix::Splash>::const_iterator Splash::findBestSplash(const std::vector<netflix::Splash> &splashes, int desiredWidth, int desiredHeight)
{
    std::vector<netflix::Splash>::const_iterator bestIt = splashes.end();
    int bestDistanceSquared = INT_MAX, bestWidth = 0, bestHeight = 0;
    for (std::vector<netflix::Splash>::const_iterator it = splashes.begin(); it != splashes.end(); ++it) {
        const int distanceSquared = square(it->width - desiredWidth) + square(it->height - desiredHeight);
        if (!distanceSquared)
            return it;
        const bool isCurrentBiggerThanDesired = (it->width >= desiredWidth) || (it->height >= desiredHeight),
                     isBestSmallerThanDesired = (bestWidth < desiredWidth) || (bestHeight < desiredHeight),
                             isCurrentClosest = distanceSquared < bestDistanceSquared;
        if(isBestSmallerThanDesired) {
            if(!isCurrentClosest && !isCurrentBiggerThanDesired)
                continue;
        } else if(!isCurrentClosest) {
            continue;
        }
        bestIt = it;
        bestWidth = it->width;
        bestHeight = it->height;
        bestDistanceSquared = distanceSquared;
    }
    return bestIt;
}
