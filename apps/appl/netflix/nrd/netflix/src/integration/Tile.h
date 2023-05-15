/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef Tile_h
#define Tile_h

#include <vector>
#include <string>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

namespace netflix {

static inline std::string tileCatalogFileName() { return "tile-catalog"; }

struct Tile
{
    inline Tile() : width(0), height(0), expiry(0) {}
    inline bool operator==(const Tile &other) const
    {
        return (url == other.url
                && title == other.title
                && description == other.description
                && shortText == other.shortText
                && deepLink == other.deepLink
                && width == other.width
                && height == other.height
                && expiry == other.expiry);

    }
    inline bool operator!=(const Tile &other) const
    {
        return !operator==(other);
    }

    std::string url, title, description, shortText, deepLink;
    int width, height;
    time_t expiry; // UTC
};

struct TileGroup
{
    std::string title;
    std::vector<Tile> tiles;

    inline bool operator==(const TileGroup &other) const
    {
        return title == other.title && tiles == other.tiles;
    }
    inline bool operator!=(const TileGroup &other) const
    {
        return title != other.title || tiles != other.tiles;
    }
};

static inline bool recursiveMakeDirectory(const std::string &dir)
{
    struct stat statBuf;
    if (!::stat(dir.c_str(), &statBuf))
        return true;

    std::string subdir = dir;
    if (subdir.size() < PATH_MAX && subdir.length()) {
        if (subdir[subdir.length()-1] != '/')
            subdir += '/';
        for (size_t i = 1; i < subdir.length(); ++i) {
            if (subdir[i] == '/') {
                subdir[i] = 0;
                const int r = mkdir(subdir.c_str(), S_IRWXU);
                if (r && errno != EEXIST && errno != EISDIR)
                    return false;
                subdir[i] = '/';
            }
        }
        return true;
    }
    return false;
}

}

#endif

