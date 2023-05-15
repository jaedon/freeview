/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TileManager.h"
#include "Tile.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <sstream>
#ifdef __APPLE__
#include <mach/mach_time.h>
#endif
#ifndef NRDIL_HAS_SECURE_TIME
#include <curl/curl.h>
#endif
#ifdef NRDIL_HAS_INOTIFY
#include <sys/inotify.h>
#endif

// #define NRDIL_LOG_ENABLED
#ifdef NRDIL_LOG_ENABLED
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

using namespace netflix;

static inline uint64_t mono()
{
#ifdef __APPLE__
    mach_timebase_info_data_t timebaseInfo;
    mach_timebase_info(&timebaseInfo);
    const uint64_t result = ((mach_absolute_time() * timebaseInfo.numer) / (1000 * 1000 * timebaseInfo.denom));
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    const uint64_t result = ((ts.tv_sec * 1000) + (ts.tv_nsec / (1000 * 1000)));
#endif
    return result;
}

static inline uint64_t modTime(const char *file)
{
    struct stat st;
    if (stat(file, &st) == -1)
        return 0;
#ifdef NRDIL_HAS_STATMTIM
    return st.st_mtim.tv_sec * static_cast<uint64_t>(1000) + st.st_mtim.tv_nsec / static_cast<uint64_t>(1000000);
#else
    return st.st_mtime * static_cast<uint64_t>(1000);
#endif
}

#ifndef NRDIL_HAS_SECURE_TIME
static int month_number_to_month_name(const char *s)
{
    static const char *month_names[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", 0
    };
    for (int i = 0; month_names[i]; i++) {
        if (!strcmp(s, month_names[i]))
            return i;
    }
    return -1;
}

static int offsetFromTZ(const char *tz, int len, bool *ok = 0)
{
    if (!tz || !len) {
        if (ok)
            *ok = false;
        return 0;
    }
    if (tz[0] == '+' || tz[0] == '-') {
        char *end;
        long int val = strtol(tz + 1, &end, 10);
        if (end != tz + 5) {
            if (ok)
                *ok = false;
            return 0;
        }
        if (ok)
            *ok = true;
        const int hrs = val / 100;
        const int mins = val % 100;
        val = ((hrs * 3600) + (mins * 60)) * ((tz[0] == '+') ? 1 : -1);
        return val;
    }
    if (ok)
        *ok = true;
    if ((len == 3 && (!strncmp(tz, "GMT", 3))) || (len == 2 && !strncmp(tz, "UT", 2)))
        return 0;
    if (len != 3) {
        if (ok)
            *ok = false;
        return 0;
    }
    if (!strncmp(tz, "EDT", 3))
        return -4 * 3600;
    if (!strncmp(tz, "EST", 3) || !strncmp(tz, "CDT", 3))
        return -5 * 3600;
    if (!strncmp(tz, "CST", 3) || !strncmp(tz, "MDT", 3))
        return -6 * 3600;
    if (!strncmp(tz, "MST", 3) || !strncmp(tz, "PDT", 3))
        return -7 * 3600;
    if (!strncmp(tz, "PST", 3))
        return -8 * 3600;
    if (ok)
        *ok = false;
    return 0;
}

static int calculateUTCOffset()
{
    time_t localTime = time(0);
    tm localt;
    localtime_r(&localTime, &localt);

    // Get the difference between this time zone and UTC on the 1st of January of this year.
    localt.tm_sec = 0;
    localt.tm_min = 0;
    localt.tm_hour = 0;
    localt.tm_mday = 1;
    localt.tm_mon = 0;
    // Not setting localt.tm_year!
    localt.tm_wday = 0;
    localt.tm_yday = 0;
    localt.tm_isdst = 0;
#ifdef NRDIL_HAS_TM_GMTOFF
    localt.tm_gmtoff = 0;
#endif
#ifdef NRDIL_HAS_TM_ZONE
    localt.tm_zone = 0;
#endif

#ifdef NRDIL_HAS_TIMEGM
    time_t utcOffset = timegm(&localt) - mktime(&localt);
#else
    // Using a canned date of 01/01/2009 on platforms with weaker date-handling foo.
    localt.tm_year = 109;
    time_t utcOffset = 1230768000 - mktime(&localt);
#endif

    return static_cast<int>(utcOffset);
}

static uint64_t parseRFC1123(const char *dateStr)
{
    int sec = 0, min = 0, hour = 0, mday = 0, month = 0, year = 0, tzb = 0, tze = 0;

    char mon[32];
    if (sscanf(dateStr, "%d/%3s/%d %d:%d:%d %n%*[+-0123456789UTGMESDCP]%n", &mday, mon, &year, &hour, &min, &sec, &tzb, &tze) != 6
        && sscanf(dateStr, "%d %3s %d %d:%d:%d %n%*[+-0123456789UTGMESDCP]%n", &mday, mon, &year, &hour, &min, &sec, &tzb, &tze) != 6
        && sscanf(dateStr, "%*3s, %d %3s %d %d:%d:%d %n%*[+-0123456789UTGMESDCP]%n", &mday, mon, &year, &hour, &min, &sec, &tzb, &tze) != 6
        && sscanf(dateStr, "%*3s, %d-%3s-%d %d:%d:%d %n%*[+-0123456789UTGMESDCP]%n", &mday, mon, &year, &hour, &min, &sec, &tzb, &tze) != 6
        && sscanf(dateStr, "%d-%3s-%d %d:%d:%d %n%*[+-0123456789UTGMESDCP]%n", &mday, mon, &year, &hour, &min, &sec, &tzb, &tze) != 6) {
        return 0;
    }

    month = month_number_to_month_name(mon);
    if (month == -1)
        return 0;

    struct tm tm;
    (void)memset(&tm, 0, sizeof(tm));

    tm.tm_mday = mday;
    tm.tm_mon = month;
    tm.tm_year = year;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;

    if (tm.tm_year > 1900) {
        tm.tm_year -= 1900;
    }
    else if (tm.tm_year < 70) {
        tm.tm_year += 100;
    }

    bool ok;
    const int oft = offsetFromTZ(dateStr + tzb, tze - tzb, &ok);
    if (!ok)
        return 0;

    const int system_clock_offset = calculateUTCOffset();
    const long long epoch = static_cast<long long>(mktime(&tm)); // coercing time_t into int64_t so that we can print it
    const long long seconds = epoch + oft + system_clock_offset;
    if (epoch == -1 || seconds < 0) {
        // fprintf(stderr, "Bad RFC1123 date: %s (%lld, %lld, %d, %d)\n", dateStr, seconds, epoch, oft, system_clock_offset);
        return 0;
    }
    return seconds * 1000;
}
#endif

static pthread_mutex_t sMutex = PTHREAD_MUTEX_INITIALIZER;

class MutexLocker
{
public:
    MutexLocker()
    {
        const int ret = pthread_mutex_lock(&sMutex);
        (void)ret;
        assert(!ret);
    }
    ~MutexLocker()
    {
        const int ret = pthread_mutex_unlock(&sMutex);
        (void)ret;
        assert(!ret);
    }
};

TileManager::TileManager()
    : mThread(0),
      mPollInterval(0),
      mLastModTime(0),
      mNextExpiredTile(0)
#ifndef NRDIL_HAS_SECURE_TIME
    , mMonoOffset(0), mHasMonoOffset(false)
#endif
{
    mPipe[0] = mPipe[1] = -1;
}

TileManager::~TileManager()
{
    stop();
    if (mPipe[0] != -1) {
        close(mPipe[0]);
        close(mPipe[1]);
    }
}

bool TileManager::start(const std::string &directory, const std::vector<unsigned char> &secret, int pollInterval)
{
    if (secret.empty() || directory.empty())
        return false;

    if (!mSecret.empty()) {
        return false;
    }

    if (directory[0] != '/') {
        char buf[1024];
        if (!getcwd(buf, sizeof(buf) - 1)) {
            return false;
        }
        const int len = strlen(buf);
        buf[len] = '/';
        mDirectory.assign(buf, len + 1);
        mDirectory += directory;
    } else {
        mDirectory = directory;
    }
    if (mDirectory[mDirectory.size() - 1] != '/')
        mDirectory += '/';
    mFileName = mDirectory + tileCatalogFileName();
    LOG("Filename is %s\n", mFileName.c_str());

    mSecret = secret;
    mPollInterval = pollInterval;

    assert(mPipe[0] == -1);
    assert(mPipe[1] == -1);
    int ret = pipe(mPipe);
    (void)ret;
    assert(!ret);
    ret = pthread_create(&mThread, 0, TileManager::thread, this);
    assert(!ret);

    return true;
}

void TileManager::run()
{
#ifndef NRDIL_HAS_SECURE_TIME
    int delay = 10;
    while (true) {
        const char *urls[] = {
            "http://www.netflix.com",
            "http://uiboot.netflix.com",
            "http://secure.netflix.com"
        };

        for (size_t i=0; i<sizeof(urls) / sizeof(urls[0]); ++i) {
            CURL *curl = curl_easy_init();
            if (curl) {
                curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
                curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "8.8.8.8");
                curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
                curl_easy_setopt(curl, CURLOPT_URL, urls[i]);
                curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 30000);
                curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 15000);
                curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &TileManager::curlHeaderCallback);
                curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
                curl_easy_perform(curl);
                curl_easy_cleanup(curl);
                if (!mHasMonoOffset) {
                    error(std::string("Failed to get proper time from: ") + urls[i]);
                } else {
                    break;
                }
            }
        }
        if (mHasMonoOffset)
            break;

        char buf[1024];
        snprintf(buf, sizeof(buf), "Failed to get proper time. Checking again in %d seconds", delay);

        int ret;
        do {
            fd_set read;
            FD_ZERO(&read);
            FD_SET(mPipe[0], &read);
            timeval to = { delay, 0 };
            ret = select(mPipe[0] + 1, &read, 0, 0, &to);
        } while (ret == -1 && errno == EINTR);

        assert(ret >= 0);
        if (ret > 0) // stop was called
            break;
        delay = std::min<int>(3600, (int)(delay * 1.5f));
    }
    if (!mHasMonoOffset)
        return;
#endif

#ifdef NRDIL_HAS_INOTIFY
    int inotifyFd = -1;
    int watchId = -1;
#endif

    const std::string catalogFileName = tileCatalogFileName();
    readCatalog(Force);
    while (true) {
#ifdef NRDIL_HAS_INOTIFY
        if (inotifyFd == -1) {
            inotifyFd = inotify_init();
        }
        if (inotifyFd && watchId == -1 && netflix::recursiveMakeDirectory(mDirectory)) {
            const unsigned int flags = IN_MOVED_FROM|IN_MOVED_TO|IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_ATTRIB|IN_CLOSE_WRITE;

            watchId = inotify_add_watch(inotifyFd, mDirectory.c_str(), flags);
        }
#endif
        fd_set read;
        int max = mPipe[0];
        FD_ZERO(&read);
        FD_SET(mPipe[0], &read);
        timeval to = { 0, 0 };
        timeval *tv = 0;
        bool useTimeout = false;
#ifdef NRDIL_HAS_INOTIFY
        if (inotifyFd != -1 && watchId != -1) {
            FD_SET(inotifyFd, &read);
            max = std::max(inotifyFd, max);
        } else
#endif
        {
            useTimeout = true;
        }

        if (useTimeout) {
            to.tv_sec = mPollInterval / 1000;
            to.tv_usec = (mPollInterval % 1000) * 1000;
            tv = &to;
        }

        ReadCatalogMode readMode = IfChanged;
        if (mNextExpiredTile) {
            const time_t seconds = std::max<int>(0, static_cast<int>(mNextExpiredTile + 1) - static_cast<int>(currentTime()));
            if (!tv || seconds <= to.tv_sec) {
                to.tv_sec = seconds;
                to.tv_usec = 0;
                tv = &to;
                readMode = Force;
            }
        }

        const int ret = select(max + 1, &read, 0, 0, tv);
        assert(ret != -1 || errno == EINTR);
        if (ret >= 0) {
            bool check = true;
            if (ret > 0) {
                if (FD_ISSET(mPipe[0], &read)) {
                    break;
                }
#ifdef NRDIL_HAS_INOTIFY
                if (FD_ISSET(inotifyFd, &read)) {
                    check = false;
                    int s = 0;
                    ioctl(inotifyFd, FIONREAD, &s);
                    if (!s)
                        break;
                    enum { StaticBufSize = 4096 };
                    char staticBuf[StaticBufSize];
                    char *buf = s > StaticBufSize ? new char[s] : staticBuf;
                    int read;
                    do {
                        read = ::read(inotifyFd, buf, s);
                    } while (read == -1 && errno == EINTR);

                    int idx = 0;
                    while (idx < read) {
                        inotify_event *event = reinterpret_cast<inotify_event *>(buf + idx);
                        idx += sizeof(inotify_event) + event->len;
                        LOG("inotify_event %s 0x%x\n", event->name, event->mask);
                        if (event->mask & (IN_MOVED_TO | IN_CREATE | IN_DELETE | IN_ATTRIB | IN_CLOSE_WRITE)
                            && event->name == catalogFileName) {
                            check = true;
                            readMode = IfChanged;
                            break;
                        }
                    }
                    if (buf != staticBuf)
                        delete[] buf;
                }
#endif
            }
            if (check) {
                readCatalog(readMode);
            }
        }
    }

#ifdef NRDIL_HAS_INOTIFY
    if (inotifyFd != -1) {
        if (watchId != -1)
            inotify_rm_watch(inotifyFd, watchId);
        close(inotifyFd);
    }
#endif
}

TileManager::Tiles TileManager::tiles() const
{
    MutexLocker lock;
    return mLastTiles;
}

std::vector<TileGroup> TileManager::readTiles(Buffer::Status &status, time_t &nextExpired) const
{
    nextExpired = 0;
    status = Buffer::Ok;

    Buffer buffer;
    status = buffer.open(mFileName, mSecret);
    if (status == Buffer::Ok && !buffer.isEmpty()) {
        std::vector<TileGroup> ret;
        const time_t now = currentTime();
        int groups;
        buffer >> groups;
        if (groups > 0) {
            ret.resize(groups);
            for (int i=0; i<groups && !buffer.hasError(); ++i) {
                TileGroup &group = ret[i];
                buffer >> group.title;
                int tiles;
                buffer >> tiles;
                group.tiles.reserve(tiles);
                for (int j=0; j<tiles; ++j) {
                    Tile tile;
                    buffer >> tile.url >> tile.title >> tile.description >> tile.shortText
                           >> tile.deepLink >> tile.width >> tile.height >> tile.expiry;
                    if (tile.expiry > now) {
                        if (!nextExpired || nextExpired > tile.expiry)
                            nextExpired = tile.expiry;
                        group.tiles.push_back(tile);
                    }
                }
            }
        }
        if (!buffer.hasError())
            return ret;
    }
    return std::vector<TileGroup>();
}

void TileManager::stop()
{
    if (mThread) {
        int ret;
        do {
            ret = write(mPipe[1], "q", 1);
        } while (ret == -1 && errno == EINTR);

        void *t;
        pthread_join(mThread, &t);
        mThread = 0;
    }
}

void TileManager::readCatalog(ReadCatalogMode mode)
{
    const uint64_t modTime = ::modTime(mFileName.c_str());
    LOG("reading catalog %lld\n", modTime);
    if (mode == Force || modTime != mLastModTime) {
        mLastModTime = modTime;
        Buffer::Status status;
        int attempts = 3;
        std::vector<TileGroup> t;
        while (true) {
            t = readTiles(status, mNextExpiredTile);
            if (status != Buffer::Ok && --attempts > 0) {
                usleep(100 * 1000);
            } else {
                break;
            }
        }
        if (status != Buffer::Ok && status != Buffer::NoFileError) {
            std::ostringstream stream;
            stream << "Failed to read tiles: " << status;
            error(stream.str());
        }
        bool changed = false;
        {
            MutexLocker lock;
            if (t != mLastTiles.groups || status != Buffer::Ok || status != mLastTiles.status) {
                mLastTiles.groups = t;
                mLastTiles.status = status;
                changed = true;
            }
        }
        if (changed) {
            tilesChanged();
        } else {
            LOG("Tiles are the same\n");
        }
    } else {
        LOG("Mod time is the same, not checking\n");
    }
}

time_t TileManager::currentTime() const
{
#ifdef NRDIL_HAS_SECURE_TIME
    return NRDIL_HAS_SECURE_TIME;
#else
    if (mHasMonoOffset)
        return (mono() + mMonoOffset) / 1000;
    return time(0);
#endif
}

#ifndef NRDIL_HAS_SECURE_TIME
size_t TileManager::curlHeaderCallback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    const char *str = reinterpret_cast<const char *>(ptr);
    if (!strncmp(str, "Date:", 5)) {
        str += 5;
        size_t len = size * nmemb - 5;
        while (isspace(*str)) {
            ++str;
            ++len;
        }
        if (str) {
            TileManager *thiz = reinterpret_cast<TileManager *>(userp);
            const int64_t timeMS = parseRFC1123(std::string(str, len).c_str());
            if (timeMS) {
                const int64_t mono = ::mono();
                thiz->mHasMonoOffset = true;
                thiz->mMonoOffset = timeMS - mono;
                return 0;
            } else {
                thiz->error("Failed to parse time header: " + std::string(reinterpret_cast<const char*>(ptr), nmemb * size));
            }
        }
    }
    return size * nmemb;
}
#endif
