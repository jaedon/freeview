/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __DISKCACHE_H__
#define __DISKCACHE_H__

#include <nrdbase/DumpInfo.h>
#include <set>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/ObjectCount.h>
#include <nrdbase/LinkedList.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/ConditionVariable.h>
#include <nrdbase/Time.h>
#include <nrdnet/IAsyncHttpLib.h>
#include <nrdnet/Resource.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

NRDP_DECLARE_OBJECTCOUNT(DiskCacheResponse);
NRDP_DECLARE_OBJECTCOUNT(DiskCacheRequest);
NRDP_DECLARE_OBJECTCOUNT(DiskCacheEntry);

namespace netflix {

class DiskCacheThread;
class DiskCacheDumper;
struct WriteEntry;
class Resource;
class DiskCacheDumpBase;
class DiskCache : public enable_shared_from_this<DiskCache>
{
public:
    DiskCache(const std::string &path);
    virtual ~DiskCache();

    std::string path() const { return mPath; }

    void init(int capacity, int maxPending, int catalogTimer);
    void configure(int capacity, int maxPending, int catalogTimer);
    void shutdown();

    typedef shared_ptr<DiskCache> SharedPtr;

    void flush();

    class Response : public Noncopyable
    {
    public:
        typedef shared_ptr<Response> SharedPtr;
        typedef weak_ptr<Response> WeakPtr;

        ~Response() { NRDP_OBJECTCOUNT_DEREF(DiskCacheResponse, this); }

        inline int httpStatusCode() const { return mHttpStatusCode; }
        inline bool isExpired() const { return mExpiration <= Time::serverTime(); }
        inline Time expiration() const { return mExpiration; }

        inline DataBuffer responseData() const { return mResponseData; }
        inline const HttpResponseHeaders &responseHeaders() const { return mResponseHeaders; }
        std::string responseHeader(const std::string &header) const;
        inline int size() const { return mSize; }
        inline DataBuffer cacheKey() const { return mCacheKey; }
        inline void *id() const { return reinterpret_cast<void*>(mFileIndex); }
        inline int priority() const { return mPriority; }
        const AseUrl &url() const { return mUrl; }
        enum Type {
            Secure,
            Insecure,
            All
        };

    private:
        Response()
            : mFileIndex(0), mSize(0), mHttpStatusCode(0), mType(Secure)
        {
            NRDP_OBJECTCOUNT_REF(DiskCacheResponse, this);
        }
        AseUrl mUrl;
        uint32_t mFileIndex;
        int mSize, mPriority, mHttpStatusCode;
        DataBuffer mCacheKey;
        Time mExpiration;
        Type mType;
        HttpResponseHeaders mResponseHeaders;
        DataBuffer mResponseData;
        friend class DiskCache;
    };

    class Request : public Noncopyable
    {
    public:
        typedef shared_ptr<Request> SharedPtr;
        typedef weak_ptr<Request> WeakPtr;

        Request(const AseUrl &url, const DataBuffer &cacheKey, Response::Type allowedType)
            : mTime(Time::mono()), mCacheKey(cacheKey),
              mState(Pending), mAbort(false), mAllowedType(allowedType), mUrl(url)
        {
            NRDP_OBJECTCOUNT_REF(DiskCacheRequest, this);
        }
        virtual ~Request()
        {
            NRDP_OBJECTCOUNT_DEREF(DiskCacheRequest, this);
        }
        inline DataBuffer cacheKey() const { return mCacheKey; }
        enum State {
            Pending,
            Finished,
            Aborted
        };

        State state() const;
        virtual void onComplete(const Response::SharedPtr &response) = 0;
        AseUrl url() const { return mUrl; }
        State wait(const Time &max = Time());
        void abort();
        Response::Type allowedType() const { return mAllowedType; }
        Time time() const { return mTime; }
    private:
        const Time mTime;
        const DataBuffer mCacheKey;
        State mState;
        bool mAbort;
        const Response::Type mAllowedType;
        const AseUrl mUrl;
        friend class DiskCacheThread;
    };

    void purge();
    void clear();
    virtual void factoryReset();
    void dump(const DumpInfo &dump=DumpInfo(), Variant *variant=0);
    Variant toVariant() const;
    virtual Variant stats() const;
    std::list<Response::SharedPtr> find(const DumpInfo &dump) const;

    inline int capacity() const { ScopedMutex lock(sDiskCacheMutex); return mCapacity; }
    inline int maxPendingSize() const { ScopedMutex lock(sDiskCacheMutex); return mMaxPendingSize; }
    inline int catalogTimer() const { ScopedMutex lock(sDiskCacheMutex); return mCatalogTimer; }
    inline bool isEnabled() const { return capacity(); }

    std::vector<DataBuffer> cacheKeys() const;

    inline bool touch(const DataBuffer &cacheKey)
    {
        ScopedMutex lock(sDiskCacheMutex);
        return findEntry(cacheKey, Response::All);
    }
    enum Statistic {
        Hit,
        Miss,
        Expired
    };

    void report(Statistic statistic, const AseUrl &url);
    void clearStats();
    bool sendRequest(const Request::SharedPtr &request);
    bool remove(const DataBuffer &cacheKey);
    int purgeExpired();
    typedef void (*AddCallback)(void *, bool);
    bool addResource(const DataBuffer &cacheKey,
                     int httpStatusCode,
                     const AseUrl &url,
                     const HttpResponseHeaders &responseHeaders,
                     const DataBuffer &responseData,
                     int priority,
                     const Time &expiration,
                     int maxAge,
                     void *userData = 0,
                     AddCallback callback = 0);

    enum EncryptionKeyMode {
        EncryptionKey_Load,
        EncryptionKey_Regenerate
    };
    virtual DataBuffer encryptionKey(EncryptionKeyMode mode) = 0;
    virtual Response::Type allowedType(const shared_ptr<ResourceRequest> &request) const;
    virtual bool writeAllowed(int /*bytes*/, const AseUrl &/*url*/) { return true; }
    virtual void onBytesWritten(int /*bytes*/) {}
    virtual std::string consoleDumpExtra() const { return std::string(); }
    virtual void writeSpeedMeasured(const Time &/* time */, int /* bytes */) {}
private:
    void init(EncryptionKeyMode mode);
    Response::SharedPtr readRequest(const Request::SharedPtr &request);
    struct Entry : public Noncopyable {
        Entry()
            : fileIndex(0), size(0), priority(0), effectivePriority(0), flags(0), updated(false), hits(0), next(0), prev(0)
        {
            NRDP_OBJECTCOUNT_REF(DiskCacheEntry, this);
        }
        ~Entry()
        {
            NRDP_OBJECTCOUNT_DEREF(DiskCacheEntry, this);
        }

        DataBuffer cacheKey;
        uint32_t fileIndex;
        int size, priority, effectivePriority;
        Time expiration;
        enum Flag {
            None,
            Secure,
            Compressed
        };
        unsigned int flags;
        bool updated;
        int hits;

        Entry *next, *prev;
    };


    mutable int mCacheSize;
    int mCapacity, mMaxPendingSize, mPendingSize, mCatalogTimer;
    mutable LinkedList<Entry*> mEntryList;
    struct Stats {
        int hits, misses, expired, writes, bytesWritten, reads, decrypts, decompresses, hmacs, encrypts;
        double readCatalogTime, validateFileSystemTime;
    } mutable mStats;

    Entry *findEntry(const DataBuffer &cacheKey, Response::Type type);
#ifndef NRD_NO_FILESYSTEM
    int writeSignedFile(const DataBuffer &data,
                        const std::string &fileName, const char *context);
    void writeCatalog();
    bool writeEntry(WriteEntry *entry);

    bool purgeInternal(int needed, int priority);
    bool readCatalog(bool *corrupted);

    mutable std::map<DataBuffer, Entry*> mEntryMap;
    std::set<int> mIndexes;
    uint32_t mLastIndex;
    bool mValidatedFileSystem;
    bool mMeasuredWriteSpeed;

    friend class DiskCacheDumper;
    friend class DiskCacheThread;
    friend class DiskCache::Request;
    DiskCacheThread *mThread;

    std::string mPath;

    DataBuffer readSignedFile(const std::string &path, int expectedSize, bool *signatureError = 0) const;
    inline Response::SharedPtr readEntry(const Entry *entry) const
    {
        return readEntry(entry->fileIndex, entry->effectivePriority, entry->size,
                         entry->cacheKey, entry->expiration, entry->flags);
    }
    Response::SharedPtr readEntry(uint32_t fileIndex, int priority, int size,
                                  const DataBuffer &cacheKey,
                                  Time expiration, unsigned int flags) const;

    std::string generateFileName(int index) const;
    uint32_t nextFileIndex();
    Entry *removeEntry(Entry *entry);

    enum { CatalogVersion = 12 };
    DataBuffer mKey;
    mutable HMAC_CTX mHMAC;
    mutable EVP_CIPHER_CTX mCipherContext;
#endif
    static Mutex sDiskCacheMutex;
    static ConditionVariable sCondition;
    friend class DiskCacheDumpBase;
};
}

#endif /* __DISKCACHE_H__ */
