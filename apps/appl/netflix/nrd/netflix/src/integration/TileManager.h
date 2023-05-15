/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TileManager_h
#define TileManager_h

#include <nrdil/Buffer.h>

#include <vector>
#include <string>
#include <pthread.h>
#include <assert.h>
#include <stdint.h>

namespace netflix {
struct TileGroup;

class TileManager
{
public:
    TileManager();
    virtual ~TileManager();

    bool start(const std::string &directory,
               const std::vector<unsigned char> &secret,
               int pollInterval = 10000);
    void stop(); // thread-safe

    struct Tiles {
        Tiles()
            : status(Buffer::Ok)
        {}
        Buffer::Status status;
        std::vector<TileGroup> groups;
    };

    Tiles tiles() const;

    virtual void tilesChanged() {} // called from worker thread
    virtual void error(const std::string &) {}
private:
    time_t currentTime() const;
    std::vector<TileGroup> readTiles(Buffer::Status &status, time_t &nextExpired) const;
    TileManager(const TileManager &);
    TileManager &operator=(const TileManager &);
    void run();
    enum ReadCatalogMode {
        IfChanged,
        Force
    };
    void readCatalog(ReadCatalogMode mode);
    static void *thread(void *data)
    {
        reinterpret_cast<TileManager*>(data)->run();
        return 0;
    }
    Tiles mLastTiles;
    std::string mDirectory, mFileName;
    std::vector<unsigned char> mSecret;
    pthread_t mThread;
    int mPollInterval;
    uint64_t mLastModTime;
    time_t mNextExpiredTile;
    int mPipe[2];
#ifndef NRDIL_HAS_SECURE_TIME
    static size_t curlHeaderCallback(void *ptr, size_t size, size_t nmemb, void *userp);
    int64_t mMonoOffset;
    bool mHasMonoOffset;
#endif
};

}

#endif
