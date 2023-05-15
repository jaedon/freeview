/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DiskCache.h"

#include "Resource.h"
#include "ResourceRequest.h"
#include "ResourceManager.h"

#include <nrdbase/Base64.h>
#include <nrdbase/Dumper.h>
#include <nrdbase/Instrumentation.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/ReadDir.h>
#include <nrdbase/WriteLimiter.h>

#include <iostream>
#include <algorithm>
#include <queue>

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <limits>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <arpa/inet.h>

// #define DISKCACHE_NO_SIGNATURE
// #define DISKCACHE_NO_COMPRESSION
// #define DISKCACHE_NO_ENCRYPTION

using namespace netflix;

struct DiskCacheWriteEntryCount : public netflix::ObjectCount::Record
{
public:
    inline DiskCacheWriteEntryCount(const std::string &n) : netflix::ObjectCount::Record(n) { }
    virtual size_t cost(void *object) const;
    virtual std::string describe(void *object) const;
};

NRDP_DEFINE_OBJECTCOUNT(DiskCacheResponse);
NRDP_DEFINE_OBJECTCOUNT(DiskCacheRequest);
NRDP_DEFINE_OBJECTCOUNT(DiskCacheEntry);
NRDP_DEFINE_OBJECTCOUNT_RECORD(DiskCacheWriteEntryCount, DiskCacheWriteEntry);

Mutex DiskCache::sDiskCacheMutex(DISKCACHE_MUTEX, "DiskCache");
ConditionVariable DiskCache::sCondition;

#ifndef NRD_NO_FILESYSTEM
enum {
    MaxDirectoryEntries = 50,
    IVPSize = 16,
    BlockSize = 16,
    KeySize = 16,
    MaxPendingReads = 1000,
    MaxPendingReadTime = 10000
};
#ifndef DISKCACHE_NO_ENCRYPTION
static const char *ivp = "nflxdiskcacheivp";
#endif

static inline void cleanupDirectory(std::string dir);

namespace netflix {

class DiskCacheDumpBase
{
public:
    DiskCacheDumpBase()
        : next(0), prev(0), done(false)
    {}
    virtual ~DiskCacheDumpBase()
    {}

    void run(const shared_ptr<DiskCache> &cache)
    {
        invoke(cache);
        ScopedMutex lock(DiskCache::sDiskCacheMutex);
        done = true;
        DiskCache::sCondition.broadcast();
    }

    virtual void invoke(const shared_ptr<DiskCache> &) = 0;

    DiskCacheDumpBase *next, *prev;
    bool done;
};

template <typename Functor>
class DiskCacheDump : public DiskCacheDumpBase
{
public:
    DiskCacheDump(const Functor &functor) : mFunctor(functor) {}
    virtual void invoke(const shared_ptr<DiskCache> &diskCache) { mFunctor(diskCache); }
private:
    Functor mFunctor;
};

struct WriteEntry : public Noncopyable
{
    WriteEntry()
        : created(Time::mono())
    {
        NRDP_OBJECTCOUNT_REF(DiskCacheWriteEntry, this);
    }
    ~WriteEntry()
    {
        NRDP_OBJECTCOUNT_DEREF(DiskCacheWriteEntry, this);
    }
    int httpStatusCode, priority, estimatedSize;
    AseUrl url;
    Time expiration;
    int maxAge;
    DataBuffer responseData, cacheKey;
    HttpResponseHeaders responseHeaders;
    void *callbackUserData;
    DiskCache::AddCallback callback;
    WriteEntry *next, *prev;
    const Time created;
};

class DiskCacheThread : public Thread
{
public:
    DiskCacheThread(const shared_ptr<DiskCache> &diskCache);
    ~DiskCacheThread();
    void addReadRequest(const DiskCache::Request::SharedPtr &readRequest);
    void factoryReset() { dirty(FactoryReset); }
    virtual void Run();
    void stop();
    void validateFileSystem();
    static inline uint32_t decodeFileName(const std::vector<uint32_t> &path, int file);
    inline void recurseDir(const std::string &path,
                           std::map<uint32_t, int> *files,
                           std::vector<uint32_t> *paths,
                           uint32_t *max);
    template <typename T>
    void dump(const T &t)
    {
        assert(DiskCache::sDiskCacheMutex.isLocked());
        DiskCacheDump<T> dump(t);
        mDumps.push_back(&dump);
        dirty(Wakeup);
        while (!dump.done) {
            DiskCache::sCondition.wait(&DiskCache::sDiskCacheMutex);
        }
    }

    enum DirtyState {
        Wakeup = 0x0,
        Done = 0x01,
        CatalogDirty = 0x02,
        FlushCatalog = 0x04,
        FactoryReset = 0x08
    };
    unsigned mState;

    bool dirty(DirtyState state);
    void clearDirty();

    LinkedList<WriteEntry*> mPendingWriteEntries;
    LinkedList<DiskCacheDumpBase*> mDumps;
    std::queue<DiskCache::Request::SharedPtr> mReadRequests;

    shared_ptr<DiskCache> mDiskCache;
};
}

size_t DiskCacheWriteEntryCount::cost(void *object) const
{
    WriteEntry *entry = static_cast<WriteEntry*>(object);
    return entry->estimatedSize;
}

std::string DiskCacheWriteEntryCount::describe(void *object) const
{
    WriteEntry *entry = static_cast<WriteEntry*>(object);
    return StringFormatter::sformat("%s [%d]", entry->url.str().c_str(), entry->estimatedSize);
}

class Reader
{
public:
    Reader(const unsigned char *data, int size);

    template <typename T> Reader &operator>>(T &t);
    Reader &operator>>(std::string &string);
    Reader &operator>>(DataBuffer &buffer);
    bool isValid() const;
    int pos() const;
    bool atEnd() const;
private:
    const unsigned char *mData;
    const int mSize;
    int mPos;
    bool mValid;
};

class Writer
{
public:
    Writer(int reserveSize = 0);

    template <typename T> Writer &operator<<(const T &t);
    Writer &operator<<(const std::string &string);
    Writer &operator<<(const DataBuffer &buffer);
    void append(const unsigned char *data, int count);
    bool isValid() const;
    DataBuffer final();
    void reserve(int size);
private:
    bool mValid;
    DataBuffer mOut;
};

DiskCache::DiskCache(const std::string &path)
    : mCacheSize(0), mCapacity(0), mMaxPendingSize(0), mPendingSize(0), mCatalogTimer(0),
      mLastIndex(0), mValidatedFileSystem(false),
      mMeasuredWriteSpeed(false),
      mThread(0), mPath(path)
{
    memset(&mStats, 0, sizeof(mStats));
    assert(!mPath.empty());
    if (!endsWith(mPath, "/"))
        mPath += '/';
    memset(&mHMAC, 0, sizeof(mHMAC));
    memset(&mCipherContext, 0, sizeof(mCipherContext));
}

void DiskCache::init(EncryptionKeyMode mode)
{
    ScopedMutex lock(sDiskCacheMutex);
    mKey = encryptionKey(mode);
    if (mKey.isEmpty() && mode != EncryptionKey_Regenerate)
        mKey = encryptionKey(EncryptionKey_Regenerate);
    HMAC_CTX_init(&mHMAC);
    HMAC_Init(&mHMAC, mKey.constData(), mKey.size(), EVP_sha256());
    EVP_CIPHER_CTX_init(&mCipherContext);

    bool corrupted;
    if (!readCatalog(&corrupted)) {
        clear();
        if (corrupted) {
            Log::error(TRACE_DISK_CACHE, "Got signature error when loading catalog, regenerating key");
            mKey = encryptionKey(EncryptionKey_Regenerate);
            HMAC_cleanup(&mHMAC);
            EVP_CIPHER_CTX_cleanup(&mCipherContext);
            HMAC_CTX_init(&mHMAC);
            HMAC_Init(&mHMAC, mKey.constData(), mKey.size(), EVP_sha256());
            EVP_CIPHER_CTX_init(&mCipherContext);
        }
    }
    INSTRUMENTATION_INTERVAL_SWITCHED_START("dcache", Variant(), 120);
    if (!mThread) {
        mThread = new DiskCacheThread(shared_from_this());
        mThread->Start();
    }
    purge();
}

void DiskCache::init(int capacity, int maxPending, int catalogTimer)
{
    configure(capacity, maxPending, catalogTimer);
    init(EncryptionKey_Load);
}

void DiskCache::configure(int capacity, int maxPending, int catalogTimer)
{
    if (capacity < 0 || maxPending < 0)
        return;
    ScopedMutex lock(sDiskCacheMutex);
    const bool needPurge = mCapacity > capacity;
    mCapacity = capacity;
    mMaxPendingSize = maxPending;
    mCatalogTimer = catalogTimer;
    assert(capacity >= 0);
    assert(maxPending >= 0);
    if (!mCapacity) {
        clear();
    } else if (needPurge) {
        purge();
    }
}

void DiskCache::shutdown()
{
    unsigned state;
    {
        ScopedMutex lock(sDiskCacheMutex);
        state = mThread->mState;
        mThread->stop();
    }
    mThread->Wait();
    DiskCacheThread *thread = 0;
    {
        ScopedMutex lock(sDiskCacheMutex);
        if (state & DiskCacheThread::CatalogDirty) {
            writeCatalog();
        }
        mEntryList.deleteAll();
        std::swap(mThread, thread);
        mEntryMap.clear();
    }
    delete thread;
}


DiskCache::~DiskCache()
{
    HMAC_cleanup(&mHMAC);
    EVP_CIPHER_CTX_cleanup(&mCipherContext);
}

DiskCache::Request::State DiskCache::Request::state() const
{
    ScopedMutex lock(DiskCache::sDiskCacheMutex);
    return mState;
}

DiskCache::Request::State DiskCache::Request::wait(const Time &max)
{
    ScopedMutex lock(DiskCache::sDiskCacheMutex);
    if (!max.val()) {
        while (mState == Pending)
            sCondition.wait(&DiskCache::sDiskCacheMutex);
    } else {
        Time t = max;
        while (mState == Pending) {
            if (sCondition.wait(&DiskCache::sDiskCacheMutex, &t) != NFErr_OK)
                break;
        }
    }
    return mState;
}
void DiskCache::Request::abort()
{
    ScopedMutex lock(DiskCache::sDiskCacheMutex);
    mAbort = true;
}

void DiskCache::purge()
{
    ScopedMutex lock(sDiskCacheMutex);
    purgeInternal(0, 0);
}

void DiskCache::flush()
{
    ScopedMutex lock(sDiskCacheMutex);
    if (mThread && mThread->mState & DiskCacheThread::CatalogDirty) {
        mThread->dirty(DiskCacheThread::FlushCatalog);
    }
}

struct ToVariantFunctor
{
    void operator()(const shared_ptr<DiskCache> &diskCache) { variant = diskCache->toVariant(); }
    Variant &variant;
};

Variant DiskCache::toVariant() const
{
    Variant ret;
    ScopedMutex lock(sDiskCacheMutex);
    if (mThread->ThreadPtr() == Thread::CurrentThreadPtr()) {
        for (Entry *e = mEntryList.first(); e; e = e->next) {
            Response::SharedPtr response = readEntry(e);
            if (!response)
                continue;
            Variant data;
            data["cacheKey"] = e->cacheKey;
            data["url"] = response->url().str();
            data["httpStatusCode"] = response->httpStatusCode();
            data["priority"] = e->priority;
            data["effectivePriority"] = e->effectivePriority;
            data["file"] = generateFileName(e->fileIndex);
            data["size"] = e->size;
            data["expiration"] = e->expiration.seconds();
            if (e->flags & Entry::Compressed)
                data["compressed"] = true;
            if (e->flags & Entry::Secure)
                data["encrypted"] = true;
            data["hits"] = e->hits;
            data["updated"] = e->updated;
            ret.push_back(data);
        }
    } else {
        ToVariantFunctor functor = { ret };
        mThread->dump(functor);
    }
    return ret;
}

Variant DiskCache::stats() const
{
    ScopedMutex lock(sDiskCacheMutex);
    Variant ret;
    ret["decompresses"] = mStats.decompresses;
    ret["decrypts"] = mStats.decrypts;
    ret["encrypts"] = mStats.encrypts;
    ret["expired"] = mStats.expired;
    ret["hits"] = mStats.hits;
    ret["hmacs"] = mStats.hmacs;
    ret["misses"] = mStats.misses;
    ret["readCatalogTime"] = mStats.readCatalogTime;
    ret["reads"] = mStats.reads;
    ret["validateFileSystem"] = mStats.validateFileSystemTime;
    ret["writes"] = mStats.writes;
    ret["bytesWritten"] = mStats.bytesWritten;
    ret["total"] = mStats.hits + mStats.misses + mStats.expired;
    std::vector<std::string> version;
#ifdef DISKCACHE_NO_ENCRYPTION
    version.push_back("no encryption");
#else
    version.push_back("encryption");
#endif
#ifdef DISKCACHE_NO_COMPRESSION
    version.push_back("no compression");
#else
    version.push_back("compression");
#endif
#ifdef DISKCACHE_NO_SIGNATURE
    version.push_back("no signature");
#else
    version.push_back("signature");
#endif
    ret["settings"] = StringTokenizer::join(version, ", ");
    return ret;
}

namespace netflix {
class DiskCacheDumper : public Dumper
{
public:
    DiskCacheDumper() { }

protected:
    virtual std::vector<std::string> consoleEntry(const unsigned int, const std::string&, const Variant& value)
    {
        std::string key;
        const DataBuffer& cacheKey = value["cacheKey"].dataBuffer();
        const std::string& url = value["url"].string();
        if (!cacheKey.isBinary() && cacheKey != url)
            key = " key: " + cacheKey.toString();
        const unsigned int flags = value["flags"].integer();
        const bool updated = value["updated"].boolean();
        return one(StringFormatter::sformat(" [%lld] url: %s%s status: %lld size: %lld priority: %lld(%lld) file: %s expiration: %s(%lld) id: %p hits: %lld flags: 0x%x%s%s%s",
                                            value["idx"].integer(), url.c_str(), key.c_str(), value["status"].integer(),
                                            value["size"].integer(), value["responsePriority"].integer(),
                                            value["priority"].integer(), value["fileName"].string().c_str(),
                                            value["expiresAt"].string().c_str(), value["expiration"].integer(),
                                            value["id"].pointer(), value["hits"].integer(), flags,
                                            flags & DiskCache::Entry::Secure ? " encrypted" : "",
                                            flags & DiskCache::Entry::Compressed ? " compressed" : "",
                                            updated ? " *" : ""));

    }

    virtual std::vector<std::string> consolePostProcess(const Variant &values)
    {
        char writesBuf[16] = { 0 };
        if (values.contains("maxWrites"))
            snprintf(writesBuf, sizeof(writesBuf), "/%lld", values["maxWrites"].integer());
        std::string matchStr;
        if (values.contains("matched")) {
            const llong matched = values["matched"].integer();
            const llong entryListSize = values["entryListSize"].integer();
            const llong matchedSize = values["matchedSize"].integer();
            matchStr = StringFormatter::sformat("Matched (%lld/%lld %.1f%% => %lld). ",
                                                matched, entryListSize,
                                                entryListSize ? float((matched / float(entryListSize)) * 100.0) : 0.,
                                                matchedSize);
        }
        const llong cacheSize = values["cacheSize"].integer();
        const llong capacity = values["capacity"].integer();
        const double total = values["total"].integer();
        const llong hits = values["hits"].integer();
        const llong misses = values["misses"].integer();
        const llong expired = values["expired"].integer();
        const llong pendingSize = values["pendingSize"].integer();
        const llong maxPendingSize = values["maxPendingSize"].integer();
        std::string consoleDumpExtra = values["consoleDumpExtra"].string();
        if (!consoleDumpExtra.empty())
            consoleDumpExtra.insert(0, " ", 1);
        return one(StringFormatter::sformat("Disk Cache: (%s/%s %.1f%%). %sHits: %lld (%.1f%%) "
                                            "Misses: %lld (%.1f%%) Expired: %lld (%.1f%%). "
                                            "Pending: %lld/%s/%s (%.1f%%) Writes: %lld%s Timer: %lld%s",
                                            prettyBytes(cacheSize).c_str(), prettyBytes(capacity).c_str(),
                                            capacity ? static_cast<float>((cacheSize / static_cast<float>(capacity))) * 100. : 0.,
                                            matchStr.c_str(),
                                            hits, total != 0. ? (hits * 100. / total) : 0.,
                                            misses, total != 0. ? (misses * 100. / total) : 0.,
                                            expired, total != 0. ? (expired * 100. / total) : 0.,
                                            values["pendingEntriesSize"].integer(),
                                            prettyBytes(pendingSize).c_str(),
                                            prettyBytes(maxPendingSize).c_str(),
                                            maxPendingSize ? float((pendingSize / float(maxPendingSize)) * 100.) : 0.,
                                            values["writes"].integer(), writesBuf,
                                            values["catalogTimer"].integer(),
                                            consoleDumpExtra.c_str()));
    }
};
}

struct DumpFunctor
{
    void operator()(const shared_ptr<DiskCache> &diskCache) { diskCache->dump(dump, variant); }

    const DumpInfo &dump;
    Variant *&variant;
};

void DiskCache::dump(const DumpInfo &dump, Variant *variant)
{
    DiskCacheDumper dumper;
    Response::SharedPtr matchedResponse;
    {
        ScopedMutex lock(sDiskCacheMutex);
        if (mThread->ThreadPtr() == Thread::CurrentThreadPtr()) {
            int i = 0;
            int matched = 0;
            int matchedSize = 0;

            if (dump.flags & DumpInfo::Data) {
                dumper.keys() << "idx" << "url" << "cacheKey" << "status" << "size" << "responsePriority" << "priority"
                              << "fileName" << "expiresAt" << "expiration" << "id" << "hits" << "flags" << "updated";
                if (variant)
                    dumper.keys() << "headers" << "data";

                for (Entry *e = mEntryList.first(); e; e = e->next) {
                    Response::SharedPtr response = readEntry(e);
                    if (!response)
                        continue;

                    if (dump.idx == i || dump.ptr == response->id()
                        || (dump.idx == -1 && dump.ptr == 0 && dump.match.empty())
                        || (!dump.match.empty() && contains(response->url().str(), dump.match))
                        || e->cacheKey.toBase64() == dump.match) {
                        Dumper::Values values = dumper.values();
                        values << i << response->url().str() << e->cacheKey << response->httpStatusCode() << response->size()
                               << response->priority() << e->priority << generateFileName(e->fileIndex)
                               << response->expiration().calendarTimeText()
                               << std::max<unsigned long long>(0ull, (response->expiration() - Time::serverTime()).seconds())
                               << response->id() << e->hits << e->flags << e->updated;
                        if (variant) {
                            values << response->mResponseHeaders.toVariant() << response->mResponseData;
                        }
                        ++matched;
                        matchedSize += e->size;
                        if (dump.idx == i || dump.ptr == response->id()) {
                            matchedResponse = response;
                            break;
                        }
                    }
                    ++i;
                }
            }
            dumper["cacheSize"] = mCacheSize;
            dumper["capacity"] = mCapacity;
            dumper["catalogTimer"] = mCatalogTimer;
            dumper["pendingEntriesSize"] = mThread->mPendingWriteEntries.size();
            dumper["pendingSize"] = mPendingSize;
            dumper["maxPendingSize"] = mMaxPendingSize;

            if (matched != mEntryList.size() && matched) {
                dumper["matched"] = matched;
                dumper["entryListSize"] = mEntryList.size();
                dumper["matchedSize"] = matchedSize;
            }
            const Variant s = stats();
            const Variant::StringMapIterator end = s.stringMapEnd();
            for (Variant::StringMapIterator it = s.stringMapBegin(); it != end; ++it) {
                if (it->second.isNumber())
                    dumper[it->first] = it->second;
            }
        } else {
            DumpFunctor functor = { dump, variant };
            mThread->dump(functor);
            return;
        }
    }
    if (variant) {
        *variant = dumper.variant();
    } else {
        dumper["consoleDumpExtra"] = consoleDumpExtra();
        const std::vector<std::string>& data = dumper.toConsole();
        for (std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) {
            Log::warn(TRACE_DISK_CACHE, "%s", it->c_str());
        }

        if (matchedResponse) {
            for (size_t i = 0; i < matchedResponse->mResponseHeaders.size(); ++i)
                Log::warn(TRACE_DISK_CACHE, " ResponseHeader(%zu): %s: %s", i,
                          matchedResponse->mResponseHeaders[i].header.c_str(),
                          matchedResponse->mResponseHeaders[i].value.c_str());
            if (dump.flags & DumpInfo::Data) {
                if (dump.flags & DumpInfo::Raw) {
                    Log::warn(TRACE_DISK_CACHE, " Data\n%s", matchedResponse->mResponseData.c_str());
                } else {
                    Log::warn(TRACE_DISK_CACHE, " Data\n%s", Log::hexDump(matchedResponse->mResponseData).c_str());
                }
            }
        }
    }
}

struct FindFunctor
{
    void operator()(const shared_ptr<DiskCache> &diskCache) { responses = diskCache->find(dump); }

    const DumpInfo &dump;
    std::list<DiskCache::Response::SharedPtr> responses;
};

std::list<DiskCache::Response::SharedPtr> DiskCache::find(const DumpInfo &dump) const
{
    PERFORMANCE_MARKER_SCOPED("diskcache.find");

    std::list<Response::SharedPtr> ret;
    ScopedMutex lock(sDiskCacheMutex);
    if (mThread->ThreadPtr() == Thread::CurrentThreadPtr()) {
        int i = 0;
        for (const Entry *e = mEntryList.first(); e; e = e->next) {
            Response::SharedPtr response = readEntry(e);
            if (response) {
                if (dump.idx == i ||
                    dump.ptr == response->id() ||
                    (dump.idx == -1 && dump.ptr == 0 && dump.match.empty()) ||
                    contains(response->url().str(), dump.match) ||
                    e->cacheKey.toBase64() == dump.match) {
                    ret.push_back(response);
                }
            }
            ++i;
        }
    } else {
        FindFunctor functor = { dump, ret };
        mThread->dump(functor);
    }
    return ret;
}

void DiskCache::report(Statistic statistic, const AseUrl &url)
{
    NRDP_UNUSED(url);
    {
        ScopedMutex lock(sDiskCacheMutex);
        switch (statistic) {
        case Hit:
            INSTRUMENTATION_INTERVAL_SWITCHED_COUNT("dcache", "hits");
            ++mStats.hits;
            break;
        case Miss:
            INSTRUMENTATION_INTERVAL_SWITCHED_COUNT("dcache", "misses");
            ++mStats.misses;
            break;
        case Expired:
            INSTRUMENTATION_INTERVAL_SWITCHED_COUNT("dcache", "expired");
            ++mStats.expired;
            break;
        }
    }
    NTRACE(TRACE_DISK_CACHE, "%s %s", url.c_str(),
           statistic == Hit ? "Hit" : (statistic == Miss ? "Miss" : "Expired"));
}

void DiskCache::clearStats()
{
    ScopedMutex lock(sDiskCacheMutex);
    memset(&mStats, 0, sizeof(mStats));
    for (Entry *e = mEntryList.first(); e; e = e->next) {
        e->updated = false;
    }
}

template <typename T>
static inline std::string toString(const T &headers)
{
    std::ostringstream stream;
    for (typename T::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        const std::pair<std::string, std::string> pair = *it;
        stream << "  " << pair.first << ": " << pair.second << "\n";
    }
    return stream.str();
}

bool DiskCache::readCatalog(bool *corrupted)
{
    Stopwatch sw;
    PERFORMANCE_MARKER_SCOPED("diskcache.readcatalog");

    if (corrupted)
        *corrupted = false;
    ScopedMutex lock(sDiskCacheMutex);
    if (!isEnabled())
        return true;

    DataBuffer catalog = readSignedFile(mPath + "catalog", -1, corrupted);
    const int size = catalog.size();
    NTRACE(TRACE_DISK_CACHE, "Reading catalog size %d", size);
    if (!size) {
        return false;
    }
    Reader buffer(catalog.data<const unsigned char>(), size);

    uint8_t version = 0;
    buffer >> version;
    if (version != CatalogVersion) {
        Log::error(TRACE_DISK_CACHE, "Invalid catalog version (expected %d, got %d)", CatalogVersion, version);
        return false;
    }

    int count = 0;
    int lastPriority = INT_MIN;
    while (!buffer.atEnd()) {
        Entry *entry = new Entry;
        uint32_t expiration = 0;
        buffer >> entry->cacheKey >> entry->fileIndex >> entry->size
               >> entry->priority >> entry->effectivePriority >> expiration
               >> entry->flags;
        NRDP_OBJECTCOUNT_DESCRIBE(entry->cacheKey, "DISKCACHE_CACHEKEY:"+ResourceManager::prettyCacheKey(entry->cacheKey).toString());
        if (entry->effectivePriority)
            entry->effectivePriority /= 2;
        if (!buffer.isValid()) {
            Log::error(TRACE_DISK_CACHE, "Parse error in catalog %d/%d/%d", count, buffer.pos(), size);
            delete entry;
            return false;
        }
        assert(lastPriority <= entry->effectivePriority);
        NRDP_UNUSED(lastPriority);
        lastPriority = entry->effectivePriority;
        enum State {
            Ok,
            FileIndexError,
            KeyError
        } state = Ok;
        if (!mIndexes.insert(entry->fileIndex).second) {
            state = FileIndexError;
        } else if (!mEntryMap.insert(std::make_pair(entry->cacheKey, entry)).second) {
            state = KeyError;
        }

        if (state != Ok) {
            const Entry *previousEntry = mapValue(mEntryMap, entry->cacheKey);
            Response::SharedPtr previousResponse = previousEntry ? readEntry(previousEntry) : Response::SharedPtr();
            Response::SharedPtr response = readEntry(entry);
            Log::error(TRACE_DISK_CACHE, "Error %d (%s) is already in the catalog (%d)", entry->fileIndex,
                       generateFileName(entry->fileIndex).c_str(), state);
            if (previousResponse) {
                Log::error(TRACE_DISK_CACHE,
                           "Previous:\n"
                           "Url: %s\n"
                           "StatusCode: %d\n"
                           "Expiration: %s\n"
                           "ResponseHeaders:\n"
                           "%s\n"
                           "ResponseData:\n"
                           "%s",
                           previousResponse->url().c_str(),
                           previousResponse->httpStatusCode(),
                           previousResponse->expiration().calendarTimeText().c_str(),
                           toString(previousResponse->responseHeaders()).c_str(),
                           previousResponse->responseData().c_str());
            }
            if (response) {
                Log::error(TRACE_DISK_CACHE,
                           "New:\n"
                           "Url: %s\n"
                           "StatusCode: %d\n"
                           "Expiration: %s\n"
                           "ResponseHeaders:\n"
                           "%s\n"
                           "ResponseData:\n"
                           "%s",
                           response->url().c_str(),
                           response->httpStatusCode(),
                           response->expiration().calendarTimeText().c_str(),
                           toString(response->responseHeaders()).c_str(),
                           response->responseData().c_str());
            }
            delete entry;
            continue;
        }
        entry->expiration = Time::fromSeconds(expiration);
        mCacheSize += entry->size;
        mEntryList.append(entry);
        NTRACE(TRACE_DISK_CACHE,
               "Read entry:\n"
               "Key: %s\n"
               "File: %s (%d)\n"
               "Size: %d bytes\n"
               "priority: %d\n"
               "Expiration: %s\n"
               "Type: %s%s",
               ResourceManager::prettyCacheKey(entry->cacheKey).c_str(),
               generateFileName(entry->fileIndex).c_str(),
               ReadDir::exists(generateFileName(entry->fileIndex)),
               entry->size, entry->priority, entry->expiration.calendarTimeText().c_str(),
               entry->flags & Entry::Secure ? "Secure" : "Insecure",
               entry->flags & Entry::Compressed ? " (compressed)" : "");

        mLastIndex = std::max(mLastIndex, entry->fileIndex);
    }
    mStats.readCatalogTime = sw.elapsed();
    return true;
}

int DiskCache::writeSignedFile(const DataBuffer &data, const std::string &fileName, const char *context)
{
    PERFORMANCE_MARKER_SCOPED("diskcache.writesignedfile");

    assert(!fileName.empty());
    assert(!data.isEmpty());

    FILE *f = fopen(fileName.c_str(), "w");
    if (!f) {
        Log::error(TRACE_DISK_CACHE, "Failed to open %s for writing (%s)", fileName.c_str(), context);
        return -1;
    }
#ifndef DISKCACHE_NO_SIGNATURE
    HMAC_Init_ex(&mHMAC, 0, 0, 0, 0);
    HMAC_Update(&mHMAC, data.data(), data.size());
    unsigned char signature[EVP_MAX_MD_SIZE];
    unsigned int signatureSize = 0;
    HMAC_Final(&mHMAC, signature, &signatureSize);
#endif
    struct {
        const char *name;
        const void *data;
        const int length;
    } const pieces[] = {
#ifndef DISKCACHE_NO_SIGNATURE
        { "signature size", &signatureSize, sizeof(signatureSize) },
        { "signature", signature, (int)signatureSize },
#endif
        { "data", data.data(), data.size() },
        { 0, 0, 0 }
    };
    Stopwatch sw;
    int written = 0;
    for (int i=0; pieces[i].data; ++i) {
        const int w = fwrite(pieces[i].data, sizeof(char), pieces[i].length, f);
        if (w != pieces[i].length) {
            Log::error(TRACE_DISK_CACHE, "Write failure %s for %s (%s). Wanted to write %d bytes, got %d",
                       pieces[i].name, fileName.c_str(), context, pieces[i].length, w);
            written = -1;
            break;
        }
        written += w;
    }

    enum { MaxMeasureSize = 1024 * 200 };
    if (written != -1 && written <= MaxMeasureSize && !mMeasuredWriteSpeed) {
        fsync(fileno(f));
        const Time time = Time::fromMS(sw.elapsed());
        if (time.ms()) {
            mMeasuredWriteSpeed = true;
            writeSpeedMeasured(time, written);
        }
    }

    fclose(f);
    if (written == -1) {
        unlink(fileName.c_str());
    } else {
        ++mStats.writes;
        mStats.bytesWritten += written;
    }
    return written;
}

void DiskCache::writeCatalog()
{
    PERFORMANCE_MARKER_SCOPED("diskcache.writecatalog");

    if (!ReadDir::recursiveMakeDirectory(mPath)) {
        Log::error(TRACE_DISK_CACHE, "Cache could not be created %s", mPath.c_str());
        clear();
        return;
    }

    Writer writer;
    int count = 0;
    {
        ScopedMutex lock(sDiskCacheMutex);
        if (mEntryList.isEmpty()) {
            clear();
            return;
        }

        int needed = sizeof(uint8_t);
        for (const Entry *entry = mEntryList.first(); entry; entry = entry->next) {
            needed += entry->cacheKey.size() + sizeof(uint32_t);
            needed += sizeof(entry->fileIndex);
            needed += sizeof(entry->size);
            needed += sizeof(entry->priority);
            needed += sizeof(entry->effectivePriority);
            needed += sizeof(uint32_t);
            needed += sizeof(entry->flags);
        }
        writer.reserve(needed);

        writer << static_cast<uint8_t>(CatalogVersion);
        for (const Entry *entry = mEntryList.first(); entry; entry = entry->next) {
            writer << entry->cacheKey << entry->fileIndex << entry->size << entry->priority
                   << entry->effectivePriority << static_cast<uint32_t>(entry->expiration.seconds())
                   << entry->flags;
        }
        count = mEntryList.size();
    }
    const DataBuffer data = writer.final();
    NTRACE(TRACE_DISK_CACHE, "Writing catalog %d bytes %d entries", data.size(), count);
    NRDP_UNUSED(count);
    if (!writer.isValid()) {
        Log::error(TRACE_DISK_CACHE, "Couldn't encode catalog");
        clear();
        return;
    }
    if (writeSignedFile(data, mPath + "catalog", "catalog") == -1) {
        ScopedMutex lock(sDiskCacheMutex);
        int needed = data.size();
        while (needed > 0 && !mEntryList.isEmpty()) {
            Entry *entry = mEntryList.first();
            needed -= entry->size;
            removeEntry(entry);
        }
        if (writeSignedFile(data, mPath + "catalog", "catalog") == -1) {
            clear();
            return;
        }
    }
    INSTRUMENTATION_INTERVAL_SWITCHED_COUNT("dcache", "writeCatalog");
}

uint32_t DiskCache::nextFileIndex()
{
    uint32_t index = mLastIndex;
    while (true) {
        if (index == UINT_MAX) {
            index = 1;
        } else {
            ++index;
        }
        if (mIndexes.insert(index).second)
            break;
    }
    return index;
}

std::string DiskCache::generateFileName(int index) const
{
    assert(MaxDirectoryEntries % 2 == 0);
    std::vector<int> path;
    do {
        path.push_back(index % (MaxDirectoryEntries / 2));
        index /= (MaxDirectoryEntries / 2);
    } while (index > 0);

    if (path.size() == 1)
        path.push_back(0x00);

    std::vector<int>::const_iterator it = path.end();
    std::string ret = mPath;
    char buf[16];
    bool first = true;
    do {
        --it;
        const char *format;
        if (it == path.begin()) {
            format = "/%02x";
        } else if (first) {
            first = false;
            format = "%02x_";
        } else {
            format = "/%02x_";
        }
        snprintf(buf, sizeof(buf), format, *it);
        ret += buf;
    } while (it != path.begin());

    return ret;
}

static inline int estimateSize(WriteEntry *entry)
{
    int cost = sizeof(uint32_t) + entry->url.str().size();
    cost += entry->responseData.size() + sizeof(uint32_t);
    cost += sizeof(uint32_t);
    cost += sizeof(uint32_t);
    for (HttpResponseHeaders::const_iterator it = entry->responseHeaders.begin(); it != entry->responseHeaders.end(); ++it) {
        cost += it->header.size() + sizeof(uint32_t) + it->value.size() + sizeof(uint32_t);
    }
    cost += EVP_MAX_MD_SIZE + sizeof(uint32_t);
    return cost;
}

bool DiskCache::purgeInternal(int needed, int priority) // lock always held
{
    PERFORMANCE_MARKER_SCOPED("diskcache.purgeinternal");

    if (needed > mCapacity)
        return false;
    const int oldSize = mCacheSize;
    int purged = 0;
    Entry *entry = mEntryList.first();
    bool ok = true;
    while (entry && mCacheSize + needed > mCapacity) {
        if (needed && entry->priority > priority) {
            ok = false;
            break;
        }
        NTRACE(TRACE_DISK_CACHE, "Purged %s file: %s size: %d bytes",
               entry->cacheKey.encode(DataBuffer::Encoding_Base64).c_str(),
               generateFileName(entry->fileIndex).c_str(), entry->size);
        entry = removeEntry(entry);
        ++purged;
    }
    if (purged) {
        mThread->dirty(DiskCacheThread::CatalogDirty);
        INSTRUMENTATION_INTERVAL_SWITCHED_COUNT("dcache", "purge");
        Variant data;
        data["size"] = mCacheSize;
        data["count"] = mEntryList.size();
        data["purgedCount"] = purged;
        data["purgedSize"] = oldSize - mCacheSize;
        INSTRUMENTATION_EVENT_SWITCHED("dcache.purge", data);
        Log::info(TRACE_DISK_CACHE, "ResourceManager:purged %d/%d (%d/%d) [%d]", purged, oldSize - mCacheSize,
                  mEntryList.size(), mCacheSize, mCapacity);
    }
    return ok;
}

bool DiskCache::writeEntry(WriteEntry *writeEntry)
{
    PERFORMANCE_MARKER_SCOPED("diskcache.writeentry");

    NTRACE(TRACE_DISK_CACHE, "WriteEntry %s", writeEntry->url.c_str());
    Entry *entry = 0;
    {
        ScopedMutex lock(sDiskCacheMutex);

        //find the old entry
        std::map<DataBuffer, Entry*>::iterator it = mEntryMap.find(writeEntry->cacheKey);
        if (it == mEntryMap.end()) {
            if (!purgeInternal(writeEntry->estimatedSize, writeEntry->priority)) {
                mPendingSize -= writeEntry->estimatedSize;
                return false;
            }
            entry = new Entry;
            entry->cacheKey = writeEntry->cacheKey;
            entry->fileIndex = nextFileIndex();
        } else {
            entry = it->second;
            assert(entry->cacheKey == writeEntry->cacheKey);
            mCacheSize -= entry->size;
            mEntryList.remove(entry);
            entry->next = entry->prev = 0;
            mEntryMap.erase(it);
            if (!purgeInternal(writeEntry->estimatedSize - entry->size, writeEntry->priority)) {
                mPendingSize -= writeEntry->estimatedSize;
                delete entry;
                return false;
            }
        }
        entry->updated = true;
    }

    if (writeEntry->maxAge && !writeEntry->expiration.val()) {
        assert(Time::hasServerTime());
        writeEntry->expiration = Time::serverTime() + Time::fromSeconds(writeEntry->maxAge) - (Time::mono() - writeEntry->created);
        NTRACE(TRACE_DISK_CACHE, "Fixed maxage for %s to %s (max-age: %lld) (%d)",
               writeEntry->url.c_str(), writeEntry->expiration.calendarTimeText().c_str(), writeEntry->expiration.val(),
               writeEntry->maxAge);
    }
    entry->expiration = writeEntry->expiration;
    entry->priority = entry->effectivePriority = writeEntry->priority;
    entry->flags = writeEntry->url.IsHttps() ? Entry::Secure : Entry::None;
    std::string contentType = ResourceManager::findResponseHeader("Content-Type", writeEntry->responseHeaders);
#ifndef DISKCACHE_NO_COMPRESSION
    if (contains(contentType, "javascript") || contains(contentType, "text") || contains(contentType, "json"))
        entry->flags |= Entry::Compressed;
#endif

    std::string fileName = generateFileName(entry->fileIndex);
    const size_t lastSlash = fileName.rfind('/');
    if (lastSlash != std::string::npos) {
        fileName[lastSlash] = '\0';
        ReadDir::recursiveMakeDirectory(fileName, S_IRWXU);
        fileName[lastSlash] = '/';
    }

    // We only want to encrypt on demand if we're not compressing and we are Response::Secure
    Writer writer(writeEntry->estimatedSize);

    writer << writeEntry->url.str();
    writer << writeEntry->httpStatusCode;
    const HttpResponseHeaders &headers = writeEntry->responseHeaders;
    writer << headers.size();

    for (HttpResponseHeaders::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        writer << it->header << it->value;
    }
    writer << writeEntry->responseData;

    DataBuffer final = writer.final();
    if (entry->flags & Entry::Compressed) {
        bool ok;
        const DataBuffer compressed = final.compress(DataBuffer::Compression_GZip, &ok);
        if (!ok) {
            Log::error(TRACE_DISK_CACHE, "Failed to compress contents of %s (%d bytes)",
                       writeEntry->url.c_str(), final.size());
            return false;
        }
        final = compressed;
    }
#ifndef DISKCACHE_NO_ENCRYPTION
    if (entry->flags & Entry::Secure) {
        int status = EVP_EncryptInit_ex(&mCipherContext, EVP_aes_128_cbc(), 0, mKey.constData(),
                                        reinterpret_cast<const unsigned char*>(ivp));
        if (status != 1) {
            Log::error(TRACE_DISK_CACHE, "Failed to init encryption of %s (%d bytes)",
                       writeEntry->url.c_str(), final.size());
            return false;
        }

        DataBuffer encrypted;
        encrypted.reserve(final.size() + BlockSize + BlockSize - 1); // ### is this enough?
        int used = 0;
        status = EVP_EncryptUpdate(&mCipherContext, encrypted.data(), &used, final.data(), final.size());

        if (!status && (used > 0 || final.size() >= BlockSize)) {
            Log::error(TRACE_DISK_CACHE, "Failed to encrypt data %s (%d bytes)",
                       writeEntry->url.c_str(), final.size());
            return false;
        }
        encrypted.setUsed(used);
        encrypted.reserve(used + BlockSize);
        status = EVP_EncryptFinal(&mCipherContext, encrypted.data() + used, &used);
        if (!status) {
            Log::error(TRACE_DISK_CACHE, "Failed to finalize encryption data %s (%d bytes)",
                       writeEntry->url.c_str(), final.size());
            return false;
        }
        encrypted.setUsed(encrypted.size() + used);
        final = encrypted;
        ++mStats.encrypts;
    }
#endif

    const int ret = writeSignedFile(final, fileName, writeEntry->url.c_str());
    if (ret == -1)
        return false;

    entry->size = ret;
    {
        ScopedMutex lock(sDiskCacheMutex);
        mPendingSize -= writeEntry->estimatedSize;
        Entry *&inserted = mEntryMap[entry->cacheKey];
        assert(!inserted);
        inserted = entry;
        Entry *insertAfter = mEntryList.last();
        while (insertAfter && insertAfter->effectivePriority > entry->effectivePriority)
            insertAfter = insertAfter->prev;
        mEntryList.insertAfter(entry, insertAfter);
        mCacheSize += entry->size;
        purgeInternal(0, 0);
        INSTRUMENTATION_INTERVAL_SWITCHED_COUNT("dcache", "writeEntry");
    }
    onBytesWritten(ret);
    return true;
}

bool DiskCache::sendRequest(const Request::SharedPtr &request)
{
    PERFORMANCE_MARKER_SCOPED("diskcache.sendrequest");

    assert(request);
    assert(isEnabled());
    ScopedMutex lock(sDiskCacheMutex);
    if (!mThread) {
        return false;
    } else if (mValidatedFileSystem && mThread->mReadRequests.size() >= MaxPendingReads) {
        Log::info(TRACE_DISK_CACHE, "Disk cache backed up with reads (%zu) writes (%d/%d/%d). Refusing to even look for %s.",
                  mThread->mReadRequests.size(), mThread->mPendingWriteEntries.size(), mPendingSize, mMaxPendingSize, request->url().c_str());
        return false;
    } else if (!mThread->mReadRequests.empty() && (Time::mono() - mThread->mReadRequests.front()->time()).ms() > MaxPendingReadTime) {
        Log::info(TRACE_DISK_CACHE, "First entry in the disk cache (%s) is too old (%llu ms). Refusing to even look for %s.",
                  mThread->mReadRequests.front()->url().c_str(), (Time::mono() - mThread->mReadRequests.front()->time()).ms(), request->url().c_str());
        return false;
    } else if (!findEntry(request->cacheKey(), request->allowedType())) {
        NTRACE(TRACE_DISK_CACHE, "Couldn't find %s in DiskCache.", request->url().c_str());
        return false;
    }
    INSTRUMENTATION_INTERVAL_SWITCHED_START(INSTRUMENTATION_CTX("dcache.read", request.get()));
    NTRACE(TRACE_DISK_CACHE, "Found request for %s adding read request", request->url().c_str());
    mThread->addReadRequest(request);
    return true;
}

DiskCache::Response::SharedPtr DiskCache::readRequest(const Request::SharedPtr &request)
{
    PERFORMANCE_MARKER_SCOPED("diskcache.readrequest");

    NTRACE(TRACE_DISK_CACHE, "Reading request for %s", request->url().c_str());
    uint32_t fileIndex;
    int priority, size;
    Time expiration;
    unsigned int flags;
    assert(request);
    const DataBuffer cacheKey = request->cacheKey();
    {
        ScopedMutex lock(sDiskCacheMutex);
        Entry *entry = findEntry(cacheKey, request->allowedType());
        if (!entry) {
            NTRACE(TRACE_DISK_CACHE, "Entry is missing for %s", request->url().c_str());
            return DiskCache::Response::SharedPtr();
        }
        fileIndex = entry->fileIndex;
        priority = entry->effectivePriority;
        size = entry->size;
        expiration = entry->expiration;
        flags = entry->flags;
        ++entry->hits;
    }

    Response::SharedPtr response = readEntry(fileIndex, priority, size, cacheKey, expiration, flags);
    if (!response) {
        NTRACE(TRACE_DISK_CACHE, "Found %s (%s) in cache but failed to load it. Removing",
               ResourceManager::prettyCacheKey(cacheKey).c_str(), request->url().c_str());
        ScopedMutex lock(sDiskCacheMutex);
        Entry *entry = findEntry(cacheKey, request->allowedType());
        if (entry) {
            removeEntry(entry);
            if (mThread)
                mThread->dirty(DiskCacheThread::CatalogDirty);
        } else {
            Log::error(TRACE_DISK_CACHE, "Tried to read something that wasn't in the cache %s",
                       ResourceManager::prettyCacheKey(cacheKey).c_str());
        }
    }
    NTRACE(TRACE_DISK_CACHE, "Returning response for %s", request->url().c_str());
    return response;
}

DiskCache::Entry *DiskCache::findEntry(const DataBuffer &cacheKey,
                                       Response::Type allowedType) // lock always held
{
    PERFORMANCE_MARKER_SCOPED("diskcache.findentry");

    if (!isEnabled())
        return 0;

    std::map<DataBuffer, Entry*>::iterator it = mEntryMap.find(cacheKey);
    if (it == mEntryMap.end())
        return 0;

    Entry *entry = it->second;
    if (allowedType == Response::Secure && !(it->second->flags & Entry::Secure)) {
        Log::warn(TRACE_DISK_CACHE, "Blocked secure request for insecure data in disk cache %s",
                  cacheKey.encode(DataBuffer::Encoding_Base64).c_str());
        return 0;
    }

    entry->effectivePriority = entry->priority;
    Entry *prev = entry->prev;
    assert(entry->cacheKey == cacheKey);
    if (entry->next && entry->next->effectivePriority <= entry->effectivePriority) {
        mEntryList.remove(entry);
        Entry *insertAfter = mEntryList.last();
        while (insertAfter && insertAfter->effectivePriority > entry->effectivePriority)
            insertAfter = insertAfter->prev;
        mEntryList.insertAfter(entry, insertAfter);
        if (mThread && prev != entry->prev) {
            mThread->dirty(DiskCacheThread::CatalogDirty);
        }
    }
    NTRACE(TRACE_DISK_CACHE, "Found %s in cache", ResourceManager::prettyCacheKey(cacheKey).c_str());

    return entry;
}

std::vector<DataBuffer> DiskCache::cacheKeys() const
{
    std::vector<DataBuffer> ret;
    ScopedMutex lock(sDiskCacheMutex);
    ret.reserve(mEntryList.size());
    for (Entry *entry = mEntryList.first(); entry; entry = entry->next) {
        ret.push_back(entry->cacheKey);
    }
    return ret;
}

DiskCache::Entry * DiskCache::removeEntry(Entry *entry) // lock always held
{
    PERFORMANCE_MARKER_SCOPED("diskcache.removeentry");

    std::string file = generateFileName(entry->fileIndex);
    NTRACE(TRACE_DISK_CACHE, "Removing entry %s", file.c_str());
    mIndexes.erase(entry->fileIndex);
    unlink(file.c_str());
    file.resize(file.size() - 3);
    cleanupDirectory(file);
    mEntryMap.erase(entry->cacheKey);
    Entry *next = entry->next;
    mCacheSize -= entry->size;
    mEntryList.remove(entry);
    delete entry;
    return next;
}

bool DiskCache::remove(const DataBuffer &cacheKey)
{
    PERFORMANCE_MARKER_SCOPED("diskcache.remove");

    ScopedMutex lock(sDiskCacheMutex);
    Entry *entry = findEntry(cacheKey, Response::All);
    if (entry) {
        removeEntry(entry);
        if (mThread)
            mThread->dirty(DiskCacheThread::CatalogDirty);
        return true;
    }
    return false;
}

int DiskCache::purgeExpired()
{
    PERFORMANCE_MARKER_SCOPED("diskcache.purgeExpired");

    if (!Time::hasServerTime())
        return 0;
    int ret = 0;
    const Time now = Time::serverTime();
    ScopedMutex lock(sDiskCacheMutex);
    Entry *e = mEntryList.first();
    while (e) {
        NTRACE(TRACE_DISK_CACHE, "PurgeExpired entry %s %sexpired",
               ResourceManager::prettyCacheKey(e->cacheKey).c_str(),
               e->expiration <= now ? "" : "not ");
        if (e->expiration <= now) {
            e = removeEntry(e);
            ++ret;
        } else {
            e = e->next;
        }
    }
    return ret;
}

bool DiskCache::addResource(const DataBuffer &cacheKey,
                            int httpStatusCode,
                            const AseUrl &url,
                            const HttpResponseHeaders &responseHeaders,
                            const DataBuffer &responseData,
                            int priority,
                            const Time &expiration,
                            int maxAge,
                            void *userData,
                            AddCallback callback)
{
    NTRACE(TRACE_DISK_CACHE, "Adding resource %s %s max-age %d", url.c_str(), ResourceManager::prettyCacheKey(cacheKey).c_str(), maxAge);
    PERFORMANCE_MARKER_SCOPED("diskcache.addresource");

    std::vector<std::pair<void*, AddCallback> > callbacks;
    {
        ScopedMutex lock(sDiskCacheMutex);
        if (!mThread) {
            return false;
        }

        if (!expiration.val() && !maxAge) {
            NTRACE(TRACE_DISK_CACHE, "Adding resource %s %s no expiration %d", url.c_str(), ResourceManager::prettyCacheKey(cacheKey).c_str(), maxAge);
            return false;
        }

        Time exp = expiration;
        if (Time::hasServerTime()) {
            const Time now = Time::serverTime();
            if (maxAge)
                exp = now + Time::fromSeconds(maxAge);
            if (exp <= now) {
                NTRACE(TRACE_DISK_CACHE, "%s (%s) is already expired %lld vs %lld",
                       url.c_str(), ResourceManager::prettyCacheKey(cacheKey).c_str(),
                       exp.val(), now.val());
                return false;
            }
        }

        WriteEntry *entry = new WriteEntry;
        entry->next = entry->prev = 0;
        entry->httpStatusCode = httpStatusCode;
        entry->priority = priority;
        entry->expiration = exp;
        entry->maxAge = maxAge;
        entry->cacheKey = cacheKey;
        entry->url = url;
        entry->callbackUserData = userData;
        entry->callback = callback;

        entry->responseHeaders = responseHeaders;
        entry->responseData = responseData;
        entry->estimatedSize = estimateSize(entry);
        NRDP_OBJECTCOUNT_DESCRIBE(entry->cacheKey, "DISKCACHE_CACHEKEY:"+url.str());
        mThread->mPendingWriteEntries.append(entry);
        mPendingSize += entry->estimatedSize;
        while (mThread->mPendingWriteEntries.size() > 1 && mPendingSize > mMaxPendingSize) {
            WriteEntry *first = mThread->mPendingWriteEntries.pop_front();
            mPendingSize -= first->estimatedSize;
            NTRACE(TRACE_DISK_CACHE, "Discarding pending entry for %s of %d bytes", entry->url.c_str(), first->estimatedSize);
            if (first->callback)
                callbacks.push_back(std::make_pair(first->callbackUserData, first->callback));
            delete first;
        }
        mThread->mState |= DiskCacheThread::CatalogDirty;
        sCondition.broadcast();
    }
    for (std::vector<std::pair<void*, AddCallback> >::const_iterator it = callbacks.begin(); it != callbacks.end(); ++it) {
        it->second(it->first, false);
    }

    return true;
}

DataBuffer DiskCache::readSignedFile(const std::string &path, int expectedSize, bool *corruption) const
{
    PERFORMANCE_MARKER_SCOPED("diskcache.readsignedfile");

    if (corruption)
        *corruption = false;

    FILE *f = fopen(path.c_str(), "r");
    if (!f) {
        NTRACE(TRACE_DISK_CACHE, "Couldn't open file for reading %s errno: %d", path.c_str(), errno);
        return DataBuffer();
    }

    uint32_t signatureSize = 0;
#ifndef DISKCACHE_NO_SIGNATURE
    if (!fread(&signatureSize, sizeof(signatureSize), 1, f) || signatureSize > EVP_MAX_MD_SIZE) {
        Log::error(TRACE_DISK_CACHE, "Invalid signature size for file %s (%d) errno: %d/%d/%d", path.c_str(), signatureSize, errno, ferror(f), feof(f));
        fclose(f);
        if (corruption)
            *corruption = true;
        return DataBuffer();
    }
    char signature[EVP_MAX_MD_SIZE];
    if (!fread(signature, signatureSize, 1, f)) {
        Log::error(TRACE_DISK_CACHE, "Can't read signature for file %s %d errno: %d", path.c_str(), signatureSize, errno);
        fclose(f);
        if (corruption)
            *corruption = true;
        return DataBuffer();
    }
#endif

    if (fseek(f, 0, SEEK_END) == -1) {
        Log::error(TRACE_DISK_CACHE, "Can't seek to end of file %s errno: %d", path.c_str(), errno);
        fclose(f);
        return DataBuffer();
    }

    const long fileSize = ftell(f);
    if (fileSize == -1) {
        Log::error(TRACE_DISK_CACHE, "Can't seek to send of file %s errno: %d", path.c_str(), errno);
        fclose(f);
        return DataBuffer();
    }
    if (fileSize > 75 * 1024 * 1024) {
        Log::error(TRACE_DISK_CACHE, "File too big %s size: %ld errno: %d", path.c_str(), fileSize, errno);
        fclose(f);
        if (corruption)
            *corruption = true;
        return DataBuffer();
    }
#ifndef DISKCACHE_NO_SIGNATURE
    const int bufferSize = fileSize - signatureSize - sizeof(uint32_t);
#else
    const int bufferSize = fileSize;
#endif
    if (expectedSize != -1 && fileSize != expectedSize) {
        Log::error(TRACE_DISK_CACHE, "Invalid size, expected %d, got %ld for file %s",
                   expectedSize, fileSize, path.c_str());
        fclose(f);
        if (corruption)
            *corruption = true;
        return DataBuffer();
    }


#ifdef DISKCACHE_NO_SIGNATURE
    const long pos = 0;
#else
    const long pos = signatureSize + sizeof(uint32_t);
#endif
    if (fseek(f, pos, SEEK_SET) == -1) {
        Log::error(TRACE_DISK_CACHE, "Can't seek to end of signature for file %s %d errno: errno %d",
                   path.c_str(), static_cast<int>(signatureSize + sizeof(uint32_t)), errno);
        fclose(f);
        if (corruption)
            *corruption = true;
        return DataBuffer();
    }

    DataBuffer buffer(bufferSize); // ### protect against huge files?

    if (!fread(buffer.data(), bufferSize, 1, f)) {
        Log::error(TRACE_DISK_CACHE, "Can't read data from file %s %d", path.c_str(), bufferSize);
        fclose(f);
        return DataBuffer();
    }
    fclose(f);
#ifndef DISKCACHE_NO_SIGNATURE
    HMAC_Init_ex(&mHMAC, 0, 0, 0, 0);
    HMAC_Update(&mHMAC, buffer.data(), buffer.size());
    unsigned char actualSignature[EVP_MAX_MD_SIZE];
    unsigned int actualSignatureSize = 0;
    HMAC_Final(&mHMAC, actualSignature, &actualSignatureSize);
    ++mStats.hmacs;

    if (actualSignatureSize != signatureSize || memcmp(signature, actualSignature, signatureSize)) {
        Log::error(TRACE_DISK_CACHE, "Invalid hmac for file %s (%d/%d)", path.c_str(), actualSignatureSize, signatureSize);
        if (corruption)
            *corruption = true;
        return DataBuffer();
    }
#endif
    ++mStats.reads;
    return buffer;
}

void DiskCache::factoryReset()
{
    ScopedMutex lock(sDiskCacheMutex);
    if (mThread)
        mThread->factoryReset();
}

void DiskCache::clear()
{
    ScopedMutex lock(sDiskCacheMutex);
    memset(&mStats, 0, sizeof(mStats));
    mEntryList.deleteAll();
    mEntryMap.clear();
    mLastIndex = 0;
    mIndexes.clear();
    if (mCacheSize) {
        mCacheSize = 0;
    }
    if (ReadDir::recursiveRemoveDirectory(mPath.c_str())) {
        Log::info(TRACE_DISK_CACHE, "DiskCache:cleared (%d entries) %d of %d bytes",
                  mEntryList.size(), mCacheSize, mCapacity);
    }
    if (mThread)
        mThread->clearDirty();
}

DiskCache::Response::Type DiskCache::allowedType(const shared_ptr<ResourceRequest> &request) const
{
    return request->url().IsHttps() ? Response::Secure : Response::All;
}

DiskCache::Response::SharedPtr DiskCache::readEntry(uint32_t fileIndex, int priority, int size,
                                                    const DataBuffer &cacheKey,
                                                    Time expiration, unsigned int flags) const
{
    PERFORMANCE_MARKER_SCOPED("diskcache.readentry");

    DataBuffer buffer;
    const std::string fileName = generateFileName(fileIndex);
    NTRACE(TRACE_DISK_CACHE, "Read begin %s ", fileName.c_str());
    buffer = readSignedFile(fileName, size);
    if (buffer.isEmpty()) {
        Log::error(TRACE_DISK_CACHE, "Could not read file %s %d", fileName.c_str(), flags);
        return DiskCache::Response::SharedPtr();
    }
#ifndef DISKCACHE_NO_ENCRYPTION
    if (flags & Entry::Secure) {
        NTRACE(TRACE_DISK_CACHE, "Decrypting file %s (%d bytes)", fileName.c_str(), buffer.size());

        int status = EVP_DecryptInit_ex(&mCipherContext, EVP_aes_128_cbc(), 0, mKey.constData(),
                                        reinterpret_cast<const unsigned char*>(ivp));
        if (status != 1) {
            Log::error(TRACE_DISK_CACHE, "Unable to initialize EVP when decrypting file %s %d", fileName.c_str(), status);
            return DiskCache::Response::SharedPtr();
        }

        DataBuffer out;
        out.reserve(buffer.size() + BlockSize);
        int used = 0;
        status = EVP_DecryptUpdate(&mCipherContext, out.data(), &used, buffer.data(), buffer.size());
        if (!status && (used > 0 || buffer.size() >= BlockSize)) {
            Log::error(TRACE_DISK_CACHE, "Couldn't decrypt file %s (%d bytes) (update)", fileName.c_str(), buffer.size());
            return DiskCache::Response::SharedPtr();
        }

        const int currentSize = used;
        out.reserve(currentSize + BlockSize);
        used = 0;
        status = EVP_DecryptFinal(&mCipherContext, out.data() + currentSize, &used);
        if (!status) {
            Log::error(TRACE_DISK_CACHE, "Couldn't decrypt file %s (%d bytes) (final)", fileName.c_str(), buffer.size());
            return DiskCache::Response::SharedPtr();
        }
        out.setUsed(currentSize + used);
        buffer = out;
        ++mStats.decrypts;
    }
#endif
    if (flags & Entry::Compressed) {
        bool ok;
        const DataBuffer uncompressed = buffer.uncompress(DataBuffer::Compression_GZip, &ok);
        if (!ok) {
            Log::error(TRACE_DISK_CACHE, "Could not uncompress file %s %d", fileName.c_str(), flags);
            return DiskCache::Response::SharedPtr();
        }
        buffer = uncompressed;
        ++mStats.decompresses;
    }

    DiskCache::Response::SharedPtr ret(new Response);
    ret->mFileIndex = fileIndex;
    ret->mPriority = priority;
    ret->mSize = size;
    ret->mCacheKey = cacheKey;
    ret->mExpiration = expiration;
    ret->mType = flags & Entry::Secure ? DiskCache::Response::Secure : DiskCache::Response::Insecure;

    Reader reader(buffer.data(), buffer.size());

    std::string url;
    reader >> url;
    ret->mUrl = url;
    reader >> ret->mHttpStatusCode;

    size_t headerCount = 0;
    reader >> headerCount;
    if (headerCount) {
        ret->mResponseHeaders.resize(headerCount);
        for (size_t i=0; i<headerCount; ++i) {
            reader >> ret->mResponseHeaders[i].header >> ret->mResponseHeaders[i].value;
        }
    }
    reader >> ret->mResponseData;
    NRDP_OBJECTCOUNT_DESCRIBE(ret->mResponseData, "DISKCACHE:"+url);
    NTRACE(TRACE_DISK_CACHE, "Read end %s ", fileName.c_str());
    return ret;
}

std::string DiskCache::Response::responseHeader(const std::string &header) const
{
    return ResourceManager::findResponseHeader(header, mResponseHeaders);
}

DiskCacheThread::DiskCacheThread(const shared_ptr<DiskCache> &diskCache)
    : Thread(&THREAD_DISK_CACHE), mState(0), mDiskCache(diskCache)
{
}

DiskCacheThread::~DiskCacheThread()
{
    for (WriteEntry *entry = mPendingWriteEntries.first(); entry; entry = entry->next) {
        if (entry->callback)
            entry->callback(entry->callbackUserData, false);
    }
    mPendingWriteEntries.deleteAll();
}

void DiskCacheThread::addReadRequest(const DiskCache::Request::SharedPtr &request)
{
    assert(request);
    ScopedMutex lock(DiskCache::sDiskCacheMutex);
    mReadRequests.push(request);
    DiskCache::sCondition.broadcast();
}

bool DiskCacheThread::dirty(DirtyState state)
{
    ScopedMutex lock(DiskCache::sDiskCacheMutex);
    if (!mDiskCache)
        return false;
    if (state != Wakeup && (mState & state))
        return false;

    mState |= state;
    DiskCache::sCondition.broadcast();
    return true;
}

void DiskCacheThread::clearDirty()
{
    ScopedMutex lock(DiskCache::sDiskCacheMutex);
    mState &= Done;
    DiskCache::sCondition.broadcast();
}

void DiskCacheThread::Run()
{
    validateFileSystem();
    const Time threadStartTime = Time::mono();
    const llong diskCacheWriteSuspendInterval = std::max(1ll, Configuration::diskCacheWriteSuspendInterval());
    const Time maxDirtyCatalogWriteTime = threadStartTime + Time::fromSeconds(60 * 5);
    bool writtenCatalog = false;
    bool writeCatalog = false;
    bool writesSuspended = true;
    Time suspendEndTime = threadStartTime + Time::fromMS(diskCacheWriteSuspendInterval);
    while (true) {
        PERFORMANCE_MARKER_SCOPED("diskcache.loop");

        WriteEntry *entry = 0;
        int pendingReads = 0;
        int pendingWrites = 0;
        DiskCacheDumpBase *dump = 0;
        DiskCache::Request::SharedPtr request;
        writeCatalog = false;
        {
            ScopedMutex lock(DiskCache::sDiskCacheMutex);
            while (true) {
                const Time now = Time::mono();
                if (writesSuspended && now >= suspendEndTime) {
                    if (Time::hasServerTime()) {
                        NTRACE(TRACE_DISK_CACHE, "Writes no longer suspended");
                        writesSuspended = false;
                    } else {
                        while (suspendEndTime <= now)
                            suspendEndTime += Time::fromMS(diskCacheWriteSuspendInterval);
                    }
                }
#ifdef NRDP_HAS_TRACING
                if (writesSuspended && !mPendingWriteEntries.isEmpty()) {
                    NTRACE(TRACE_DISK_CACHE, "Not writing %d requests, suspended", mPendingWriteEntries.size());
                }
#endif

                if (mState & Done) {
                    while (!mDumps.isEmpty())
                        mDumps.pop_front()->done = true;
                    DiskCache::sCondition.broadcast();
                    break;
                }
                if (mState & (Done|FlushCatalog|FactoryReset))
                    break;
                if (!mReadRequests.empty())
                    break;
                if (!mDumps.isEmpty())
                    break;
                if (!mPendingWriteEntries.isEmpty() && !writesSuspended)
                    break;
                Time wait;
                if (writesSuspended) {
                    assert(suspendEndTime > now);
                    wait = suspendEndTime - now;
                } else if (mState & CatalogDirty) {
                    wait = Time::fromSeconds(mDiskCache->mCatalogTimer);
                    if (!writtenCatalog) { // we haven't written the catalog yet
                        if (now >= maxDirtyCatalogWriteTime) {
                            break;
                        }
                        wait = std::min(wait, maxDirtyCatalogWriteTime - now);
                    }
                }
                if (DiskCache::sCondition.wait(&DiskCache::sDiskCacheMutex, wait) == NFErr_TimedOut) {
                    break;
                }
            }

            PERFORMANCE_MARKER_SCOPED("diskcache.loop.process");

            if (mState & Done) {
                writeCatalog = mState & CatalogDirty;
                break;
            } else if (mState & FactoryReset) {
                mState &= ~FactoryReset;
                mDiskCache->clear();
                HMAC_cleanup(&mDiskCache->mHMAC);
                EVP_CIPHER_CTX_cleanup(&mDiskCache->mCipherContext);
                mDiskCache->mKey.clear();
                mDiskCache->init(DiskCache::EncryptionKey_Regenerate);
            } else if (mState & FlushCatalog) {
                writeCatalog = true;
            } else if (!mReadRequests.empty()) {
                request = mReadRequests.front();
                mReadRequests.pop();
                pendingReads = mReadRequests.size();

                if (request->mAbort) {
                    request->mState = DiskCache::Request::Aborted;
                    DiskCache::sCondition.broadcast();
                }
            } else if (!mPendingWriteEntries.isEmpty() && !writesSuspended) {
                entry = mPendingWriteEntries.pop_front();
                assert(entry);
                pendingWrites = mPendingWriteEntries.size();
            } else if (!mDumps.isEmpty()) {
                dump = mDumps.pop_front();
            } else if (mState & CatalogDirty && !writesSuspended) {
                writeCatalog = true;
            }
            if (writeCatalog) {
                writtenCatalog = true;
                mState &= ~(FlushCatalog|CatalogDirty);
            }
        }
        if (request) {
            INSTRUMENTATION_INTERVAL_SWITCHED_MARK(INSTRUMENTATION_CTX("dcache.read", request.get()), "wait");
            DiskCache::Response::SharedPtr response = mDiskCache->readRequest(request);
            INSTRUMENTATION_INTERVAL_SWITCHED_END(INSTRUMENTATION_CTX("dcache.read", request.get()),
                                                  Variant::StringMap("size", response ? Variant(response->responseData().size()) : Variant())
                                                  ("url", request->url().str())
                                                  ("pending", pendingReads));
            request->onComplete(response);
            {
                ScopedMutex lock(DiskCache::sDiskCacheMutex);
                request->mState = DiskCache::Request::Finished;
                DiskCache::sCondition.broadcast();
            }
        } else if (entry) {
            INSTRUMENTATION_INTERVAL_SWITCHED_START(INSTRUMENTATION_CTX("dcache.write", entry));
            Stopwatch sw;
            bool success = false;
            if (mDiskCache->writeAllowed(entry->estimatedSize, entry->url)) {
                success = mDiskCache->writeEntry(entry);
                INSTRUMENTATION_INTERVAL_SWITCHED_END(INSTRUMENTATION_CTX("dcache.write", entry),
                                                      Variant::StringMap("estimatedSize", entry->estimatedSize)
                                                      ("url", entry->url.str())
                                                      ("pending", pendingWrites));
            } else {
                INSTRUMENTATION_INTERVAL_SWITCHED_END(INSTRUMENTATION_CTX("dcache.write", entry),
                                                      Variant::StringMap("overBudget", true)
                                                      ("estimatedSize", entry->estimatedSize)
                                                      ("url", entry->url.str())
                                                      ("pending", pendingWrites));
            }
            if (entry->callback) {
                entry->callback(entry->callbackUserData, success);
            }
            delete entry;
        } else if (dump) {
            dump->run(mDiskCache);
        } else if (writeCatalog) {
            mDiskCache->writeCatalog();
        }
    }
    // while (true) {
    //     WriteEntry *entry = 0;
    //     {
    //         ScopedMutex lock(DiskCache::sDiskCacheMutex);
    //         if (!mPendingEntries.isEmpty()) {
    //             entry = mPendingEntries.pop_front();
    //             writeCatalog = true;
    //         }
    //     }
    //     if (entry) {
    //         Log::error(TRACE_DISK_CACHE, "[%s:%d]: Writing an entry on shutdown %s", __FILE__, __LINE__, entry->url.c_str());
    //         mDiskCache->writeEntry(entry);
    //     } else {
    //         break;
    //     }
    // }
    // if (writeCatalog) {
    //     Log::error(TRACE_DISK_CACHE, "[%s:%d]: Writing catalog on shutdown", __FILE__, __LINE__);
    //     mDiskCache->writeCatalog();
    // }
}

void DiskCacheThread::stop()
{
    ScopedMutex lock(DiskCache::sDiskCacheMutex);
    mState |= Done;
    DiskCache::sCondition.broadcast();
}

inline uint32_t DiskCacheThread::decodeFileName(const std::vector<uint32_t> &path, int file)
{
    assert(!path.empty());
    uint32_t ret = 0;
    uint32_t multiplier = MaxDirectoryEntries / 2;

    int idx = static_cast<int>(path.size()) - 1;
    do {
        ret += (multiplier * path.at(idx));
        multiplier *= 25;
    } while (--idx >= 0);

    return ret + file;
}

void DiskCacheThread::validateFileSystem()
{
    Stopwatch sw;
    PERFORMANCE_MARKER_SCOPED("diskcache.validatefilesystem");

    std::map<uint32_t, int> files;
    std::vector<uint32_t> path;
    uint32_t max = 0;
    recurseDir(mDiskCache->mPath, &files, &path, &max);
    {
        ScopedMutex lock(DiskCache::sDiskCacheMutex);
        mDiskCache->mLastIndex = std::max(max, mDiskCache->mLastIndex);
        mDiskCache->mValidatedFileSystem = true;

        DiskCache::Entry *entry = mDiskCache->mEntryList.first();
        while (entry) {
            const std::map<uint32_t, int>::iterator it = files.find(entry->fileIndex);
            if (it == files.end()) {
                mState |= CatalogDirty;
                Log::info(TRACE_DISK_CACHE, "Removing entry from catalog (%d) [%s]. File is missing.",
                          entry->fileIndex, mDiskCache->generateFileName(entry->fileIndex).c_str());
                entry = mDiskCache->removeEntry(entry);
            } else {
                if (it->second != entry->size) {
                    mState |= CatalogDirty;
                    Log::info(TRACE_DISK_CACHE, "Removing entry from catalog (%d) [%s]. File is the wrong size. Expected: %d, got %d.",
                              entry->fileIndex, mDiskCache->generateFileName(entry->fileIndex).c_str(),
                              entry->size, it->second);

                    entry = mDiskCache->removeEntry(entry);
                    unlink(mDiskCache->generateFileName(it->first).c_str());
                } else {
                    entry = entry->next;
                }
                files.erase(it);
            }
        }
    }

    for (std::map<uint32_t, int>::const_iterator it = files.begin(); it != files.end(); ++it) {
        std::string path = mDiskCache->generateFileName(it->first);
        const int status = unlink(path.c_str());
        Log::info(TRACE_DISK_CACHE, "Removed file not in catalog %d %s %d (%d bytes)", it->first, path.c_str(), status, it->second);
        path.resize(path.size() - 3);
        cleanupDirectory(path);
    }
}

class RecurseDirReadDir : public ReadDir
{
public:
    RecurseDirReadDir(DiskCacheThread *t, const std::string &pp, std::map<uint32_t, int> *f,
                      std::vector<uint32_t> *p, uint32_t *m)
        : thread(t), files(f), paths(p), max(m), path(pp)
    {}

    virtual Result visitEntry(const Entry &entry)
    {
        NTRACE(TRACE_DISK_CACHE, "Found %s %s", fileTypeToString(entry.type), entry.absolutePath.c_str());
        const char *fileName = entry.fileName();
        bool ok = false;
        switch (strlen(fileName)) {
        case 2:
            if (entry.type == File) {
                char *end = 0;
                const uint32_t val = strtoul(fileName, &end, 16);
                if (!*end && val < (MaxDirectoryEntries / 2)) {
                    ok = true;
                    const uint32_t file = DiskCacheThread::decodeFileName(*paths, val);
                    *max = std::max(*max, file);
                    (*files)[file] = entry.statEntry().st_size;
                }
            }
            break;
        case 4:
            if (entry.type == Directory) {
                char *end = 0;
                const uint32_t val = strtoul(fileName, &end, 16);

                if (*end == '_' && end - fileName == 2 && val < (MaxDirectoryEntries / 2)) {
                    assert(endsWith(path, "/"));
                    ok = true;
                    const std::string dir = path + fileName;
                    assert(endsWith(dir, "/"));
                    paths->push_back(val);
                    thread->recurseDir(dir, files, paths, max);
                    paths->pop_back();
                }
            }
            break;
        case 7:
            if (!strcmp(fileName, "catalog") && thread->mDiskCache->path() == path)
                ok = true;
            break;
        default:
            break;
        }
        if (!ok) {
            NTRACE(TRACE_DISK_CACHE, "Removed unexpected file %s", entry.absolutePath.c_str());
            unlink(entry.absolutePath.c_str());
        }
        return Continue;
    }

    DiskCacheThread *thread;
    std::map<uint32_t, int> *files;
    std::vector<uint32_t> *paths;
    uint32_t *max;
    const std::string path;
};

void DiskCacheThread::recurseDir(const std::string &path, std::map<uint32_t, int> *files, std::vector<uint32_t> *paths, uint32_t *max)
{
    PERFORMANCE_MARKER_SCOPED("diskcache.recursedir");

    RecurseDirReadDir rdrd(this, path, files, paths, max);
    rdrd.visit(path);
}

class CleanupDirectoryReadDir : public ReadDir
{
public:
    CleanupDirectoryReadDir()
        : empty(false)
    {}
    virtual Result visitEntry(const Entry &)
    {
        empty = false;
        return Stop;
    }
    bool empty;
};

static inline void cleanupDirectory(std::string dir)
{
    while (true) {
        CleanupDirectoryReadDir cdrd;
        if (!cdrd.visit(dir))
            break;

        if (cdrd.empty) {
            NTRACE(TRACE_DISK_CACHE, "Cleanup empty directory %s", dir.c_str());
            ::rmdir(dir.c_str());
            if (dir.size() > 4) {
                dir.resize(dir.size() - 4);
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

Reader::Reader(const unsigned char *data, int size)
    : mData(data), mSize(size), mPos(0), mValid(true)
{
}

template <typename T> Reader &Reader::operator>>(T &t)
{
    const long long cast = static_cast<long long>(T());
    NRDP_UNUSED(cast);
    if (mValid) {
        if (mPos + static_cast<int>(sizeof(T)) > mSize) {
            mValid = false;
        } else {
            t = *reinterpret_cast<const T*>(mData + mPos);
            mPos += sizeof(T);
        }
    }
    return *this;
}

Reader &Reader::operator>>(std::string &string)
{
    uint32_t size = 0;
    operator>>(size);
    if (size) {
        if (mPos + static_cast<int>(size) > mSize) {
            mValid = false;
        } else {
            string.assign(reinterpret_cast<const char*>(mData + mPos), size);
            mPos += size;
        }
    }
    return *this;
}

Reader &Reader::operator>>(DataBuffer &buffer)
{
    uint32_t size = 0;
    operator>>(size);
    if (size) {
        if (mPos + static_cast<int>(size) > mSize) {
            mValid = false;
        } else {
            buffer.append(mData + mPos, size);
            mPos += size;
        }
    }
    return *this;
}

bool Reader::isValid() const
{
    return mValid;
}

int Reader::pos() const
{
    return mPos;
}

bool Reader::atEnd() const
{
    assert(mPos <= mSize);
    return mPos == mSize;
}

Writer::Writer(int reserveSize)
    : mValid(true)
{
    mOut.reserve(reserveSize);
}


template <typename T> Writer &Writer::operator<<(const T &t)
{
    const long long cast = static_cast<long long>(T());
    NRDP_UNUSED(cast);
    append(reinterpret_cast<const unsigned char*>(&t), sizeof(T));
    return *this;
}

Writer &Writer::operator<<(const std::string &string)
{
    const uint32_t size = string.size();

    operator<<(size);
    if (size)
        append(reinterpret_cast<const unsigned char*>(string.c_str()), size);
    return *this;
}

Writer &Writer::operator<<(const DataBuffer &buffer)
{
    const uint32_t size = buffer.size();

    operator<<(size);
    if (size)
        append(buffer.data(), size);
    return *this;
}

void Writer::append(const unsigned char *data, int count)
{
    if (mValid)
        mOut.append(data, count);
}

bool Writer::isValid() const
{
    return mValid;
}

DataBuffer Writer::final()
{
    if (!mValid)
        return DataBuffer();

    return mOut;
}

void Writer::reserve(int size)
{
    mOut.reserve(size);
}

#else // NRD_NO_FILESYSTEM
DiskCache::DiskCache(const std::string &)
    : mCapacity(0), mMaxPendingSize(0), mPendingSize(0), mMaxWrites(0), mCatalogTimer(30)
{
}

DiskCache::~DiskCache()
{}

void DiskCache::init(int, int, int, int)
{
}

void DiskCache::configure(int, int, int, int)
{
}

bool DiskCache::addResource(const DataBuffer &, int, const AseUrl &, const HttpResponseHeaders &, const DataBuffer &, int, const Time &)
{
    return false;
}

void DiskCache::shutdown()
{
}

void DiskCache::dump(const DumpInfo &, Variant *)
{}

Variant DiskCache::toVariant() const
{
    return Variant();
}

void DiskCache::flush()
{
}

std::list<DiskCache::Response::SharedPtr> DiskCache::find(const DumpInfo &) const
{
    return std::list<Response::SharedPtr>();
}

void DiskCache::clear()
{}

DiskCache::Entry *DiskCache::findEntry(const DataBuffer &, Response::Type)
{
    return 0;
}

void DiskCache::report(Statistic, const AseUrl &)
{}

void DiskCache::clearStats()
{}

void DiskCache::remove(const DataBuffer &)
{}

DiskCache::Response::SharedPtr DiskCache::readRequest(const Request::SharedPtr &)
{
    return DiskCache::Response::SharedPtr();
}

bool DiskCache::sendRequest(const Request::SharedPtr &)
{
    return false;
}

void DiskCache::factoryReset()
{
}

DiskCache::Response::Type DiskCache::allowedType(const shared_ptr<ResourceRequest> &) const
{
    return Response::All;
}

std::string DiskCache::Response::responseHeader(const std::string &) const
{
    return std::string();
}

void DiskCache::Request::abort()
{
}

DiskCache::Request::State DiskCache::Request::wait(const Time &)
{
    return Aborted;
}
#endif
