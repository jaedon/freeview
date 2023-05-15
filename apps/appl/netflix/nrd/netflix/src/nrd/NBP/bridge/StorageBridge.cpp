/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "StorageBridge.h"

#include <nrdbase/Log.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/Base64.h>
#include <nrdbase/StringCompressor.h>
#include <nrdbase/tr1.h>

#include "ConfigurationKeys.h"
#include "DiskStore.h"
#include "NrdApplication.h"
#include "Request.h"
#include "Response.h"
#include "SecureStore.h"
#include "SystemData.h"

using namespace netflix;

#ifndef NRDP_STORAGE_MAX_FILENAME_LENGTH
#define NRDP_STORAGE_MAX_FILENAME_LENGTH 255
#endif

static inline std::string encodeKey(const std::string &deviceAccount, const std::string &key)
{
    std::string encodedKey = deviceAccount + "_" + Base64::encode(key, true);
    std::string ret;
    for (size_t i=0; i<encodedKey.size(); i+=(NRDP_STORAGE_MAX_FILENAME_LENGTH - 1)) {
        if (ret.size()) {
            ret.append("#/", 2);
        }
        ret.append(encodedKey.c_str() + i, std::min<size_t>(encodedKey.size() - i, NRDP_STORAGE_MAX_FILENAME_LENGTH - 1));
    }
    return ret;
}

static inline bool decodeKey(const std::string &k, std::string &dak, std::string &key)
{
    std::string encodedKey = k;
    replaceAll(encodedKey, "#/", std::string());

    const size_t underscore = encodedKey.rfind('_');
    if (underscore == std::string::npos)
        return false;
    dak = encodedKey.substr(0, underscore);
    key = Base64::decode(encodedKey.substr(underscore + 1));
    return true;
}

namespace netflix {
class StorageRequest
{
public:
    typedef enum {
        DISKSTORE_GET_SIZE,
        DISKSTORE_CLEAR_ALL,
        DISKSTORE_CLEAR,
        DISKSTORE_CREATE,
        DISKSTORE_APPEND,
        DISKSTORE_REMOVE,
        DISKSTORE_READ,
        DISKSTORE_CORRUPT,

        UISTORE_WRITE
    } Type;

    StorageRequest(Type t,
                   int i = -1,
                   const shared_ptr<DiskStore::Context> &c = shared_ptr<DiskStore::Context>(),
                   const std::string &d = std::string(),
                   const std::string &k = std::string(),
                   const DataBuffer &v = DataBuffer(),
                   int b = -1,
                   int e = -1)
        : type(t), index(i), context(c), dak(d), key(k), value(v), begin(b), end(e), next(0), prev(0)
    {}

    const Type type;
    const int index;
    const shared_ptr<DiskStore::Context> context;
    const std::string dak;
    const std::string key;
    const DataBuffer value;
    int begin, end;
    StorageRequest *next, *prev;
};


class StorageJob;
class StorageBridgePrivate
{
public:
    StorageBridgePrivate()
        : job(0), flushCount(0), shutdown(false)
    {}
    ~StorageBridgePrivate()
    {
        requests.deleteAll();
    }
    LinkedList<StorageRequest*> requests;
    StorageJob *job;

    Variant persistentData;
    Variant transientData;

    shared_ptr<DiskStore> diskStore;
    int flushCount;
    bool shutdown;

    shared_ptr<WriteTimer> writeTimer;

    shared_ptr<DiskStore::Context> findContext(const Variant &var) const
    {
        return diskStore->find(var.mapValue<std::string>("context"));
    }
};

class StorageJob : public ThreadPoolJob
{
public:
    StorageJob(const shared_ptr<StorageBridge> &bridge)
        : mBridge(bridge), mDiskStore(bridge->diskStore())
    {}

    virtual std::string description() const
    {
        return "StorageJob";
    }
    virtual void execute()
    {
        DiskStore::ManifestScope scope(mDiskStore);
        PERFORMANCE_MARKER_SCOPED("storagejob");

        NTRACE(TRACE_SECURE_STORE, "Job execute %p", this);

        while (true) {
            StorageRequest *request = 0;
            {
                ScopedMutex lock(StorageBridge::sMutex);
                if (mBridge->mPrivate->requests.isEmpty()) {
                    NTRACE(TRACE_SECURE_STORE, "Job finished %p", this);
                    mBridge->mPrivate->job = 0;
                    break;
                }
                if (mBridge->mPrivate->shutdown) {
                    NTRACE(TRACE_DISK_STORE, "Quitting");
                    mBridge->mPrivate->requests.deleteAll();
                    mBridge->mPrivate->job = 0;
                    break;
                }
                request = mBridge->mPrivate->requests.pop_front();
            }
            assert(request);
            NTRACE(TRACE_SECURE_STORE, "Job executing request %d %p", request->type, this);
            mBridge->executeRequest(request);
            delete request;
        }
    }
private:
    shared_ptr<StorageBridge> mBridge;
    shared_ptr<DiskStore> mDiskStore;
};

class WriteTimer : public Application::Timer
{
public:
    enum { WriteDelay = 3000 };
    WriteTimer(const shared_ptr<StorageBridge> &bridge)
        : Application::Timer(WriteDelay), mBridge(bridge)
    {
        setSingleShot(true);
    }

    virtual void timerFired()
    {
        PERFORMANCE_MARKER_SCOPED("writetimer.fired");
        if (shared_ptr<StorageBridge> bridge = mBridge.lock())
            bridge->addWriteTask();
    }
private:
    weak_ptr<StorageBridge> mBridge;
};
}

static inline uint32_t appLocalStorageBudget()
{
    uint32_t total = nrdApp()->getSystem()->getSecureStoreSize();
    uint32_t systemReserve = total >> 5; // 4k out of 128k, 4/128 = 1/32 -> total >> 5
    uint32_t secure = nrdApp()->systemData()->appTrustStoreBudget();
    return (total - systemReserve - secure);
}

#define StoragePropertyList(Property)                   \
    Property(data, NfObject::ReadOnly)                  \
    Property(diskStoreInfo, NfObject::ReadOnly)         \
    Property(flushCount, NfObject::ReadOnly)            \
    Property(secureStoreSize, NfObject::ReadOnly)       \
    Property(transientData, NfObject::ReadWrite)

PropertyTable(StorageBridge, StoragePropertyList, properties)

#define StorageMethodList(Method)               \
    Method(clear)                               \
    Method(clearAll)                            \
    Method(diskStoreAppend)                     \
    Method(diskStoreClear)                      \
    Method(diskStoreClearAll)                   \
    Method(diskStoreCorrupt)                    \
    Method(diskStoreCreate)                     \
    Method(diskStoreCreateContext)              \
    Method(diskStoreDestroyContext)             \
    Method(diskStoreGetSize)                    \
    Method(diskStoreInfo)                       \
    Method(diskStoreQuery)                      \
    Method(diskStoreRead)                       \
    Method(diskStoreRemove)                     \
    Method(flush)                               \
    Method(getFlushCount)                       \
    Method(getPersistentData)                   \
    Method(getUsedSecureStoreSize)              \
    Method(removeItem)                          \
    Method(setItem)                             \
    Method(setPersistentData)                   \
    Method(unsetPersistentData)

MethodTable(StorageBridge, StorageMethodList)

Mutex StorageBridge::sMutex(STORAGEBRIDGE_MUTEX, "StorageBridgeMutex");

StorageBridge::StorageBridge()
    : NfObject("storage",
               Properties::StorageBridge::properties, Properties::StorageBridge::PropertyCount,
               Methods::StorageBridge::methods, Methods::StorageBridge::MethodCount)
{
    mPrivate.reset(new StorageBridgePrivate);
    const std::string raw = nrdApp()->secureStore()->get<std::string>(SystemKeys::storageBridge);

    // decoded and inflate the data
    std::string value;
    StringCompressor::inflateB64(/*out*/value, raw);

    mPrivate->persistentData = Variant::fromJSON(value);
    mPrivate->diskStore = nrdApp()->diskStore();
}

void StorageBridge::init()
{
    startEventListener();
}

void StorageBridge::shutdown()
{
    stopEventListener();
    ScopedMutex lock(sMutex);
    mPrivate->shutdown = true;
    writeNow();
}

bool StorageBridge::getProperty(int index, Variant *result) const
{
    switch (index) {
    case Properties::StorageBridge::data:
        *result = mPrivate->persistentData;
        break;
    case Properties::StorageBridge::diskStoreInfo: {
        ScopedMutex lock(sMutex);
        *result = Variant(Variant::Type_StringMap);
        const std::vector<std::string> contextNames = mPrivate->diskStore->contexts();
        for (std::vector<std::string>::const_iterator it = contextNames.begin(); it != contextNames.end(); ++it) {
            shared_ptr<DiskStore::Context> ctx = mPrivate->diskStore->find(*it);
            if (ctx) {
                Variant data;
                data["size"] = ctx->size();
                const unsigned int flags = ctx->flags();
                data["encrypted"] = static_cast<bool>(flags & DiskStore::Encrypted);
                data["signature"] = static_cast<bool>(flags & DiskStore::Signature);
                result->insert(*it, data);
            }
        }
        break; }
    case Properties::StorageBridge::secureStoreSize:
        *result = nrdApp()->getDeviceLib()->getSystem()->getSecureStoreSize();
        break;
    case Properties::StorageBridge::flushCount:
        *result = nrdApp()->secureStore()->getFlushCount();
        break;
    case Properties::StorageBridge::transientData: {
        ScopedMutex lock(sMutex);
        *result = mPrivate->transientData;
        break; }
    default:
        return false;
    }
    return true;
}

bool StorageBridge::setProperty(int index, const Variant &value)
{
    switch (index) {
    case Properties::StorageBridge::transientData: {
        ScopedMutex lock(sMutex);
        mPrivate->transientData = value;
        break; }
    default:
        return false;
    }
    return true;
}

void StorageBridge::addRequest(StorageRequest *request)
{
    assert(request);
    NTRACE(TRACE_SECURE_STORE, "Adding request %d with %d requests", request->type, mPrivate->requests.size());
    mPrivate->requests.append(request);
    if (!mPrivate->job) {
        mPrivate->job = new StorageJob(static_pointer_cast<StorageBridge>(shared_from_this()));
        NTRACE(TRACE_SECURE_STORE, "Creating Job %p for %d", mPrivate->job, request->type);
        nrdApp()->threadPool()->post(mPrivate->job);
    }
}

void StorageBridge::addWriteTask()
{
    ScopedMutex lock(sMutex);
    NTRACE(TRACE_SECURE_STORE, "Queue write with %d requests", mPrivate->requests.size());
    StorageRequest *request = mPrivate->requests.last();
    while (request && request->type != StorageRequest::UISTORE_WRITE)
        request = request->prev;
    if (request) {
        mPrivate->requests.moveToEnd(request);
    } else {
        addRequest(new StorageRequest(StorageRequest::UISTORE_WRITE));
    }
}

void StorageBridge::queueWrite() // lock always held
{
    NTRACE(TRACE_SECURE_STORE, "Starting write timer with %d requests", mPrivate->requests.size());
    if (!mPrivate->writeTimer)
        mPrivate->writeTimer.reset(new WriteTimer(static_pointer_cast<StorageBridge>(shared_from_this())));
    mPrivate->writeTimer->restart();
}

void StorageBridge::writeNow()
{
    std::string encoded;
    {
        ScopedMutex lock(sMutex);
        ++mPrivate->flushCount;
        NTRACE(TRACE_SECURE_STORE, "writeNow number %d", mPrivate->flushCount);
        // compress and encode the data
        StringCompressor::deflateB64(/*out*/ encoded, mPrivate->persistentData.toJSON());
    }

    uint32_t budget = ::appLocalStorageBudget();
    if (encoded.length() > budget) {
        Log::error(TRACE_SECURE_STORE,
                   "WARNING: StorageBridge exceeds budgeted size: %zu > %u",
                   encoded.length(), budget);
        sendEvent("overbudget");
    }

    nrdApp()->secureStore()->set<std::string>(SystemKeys::storageBridge, encoded);
    nrdApp()->secureStore()->flushEncrypted();
    propertyUpdated(Properties::StorageBridge::flushCount);
}

// Returns true if the key contains a restricted auth key
static bool filterAuthKeys(const std::string &key)
{
    static const char *s_authKeys[] = { "Kce", "Kch", "Kee", "Keh" };

    for (int i=0; i<4; ++i) {
        if (contains(key, s_authKeys[i], 3)) {
            return true;
        }
    }

    return false;
}

Variant StorageBridge::invoke(int method, const Variant &var)
{
    bool ok;

    const int index  = var.mapValue<int>("idx");

    switch (method) {
    case Methods::StorageBridge::setItem: {
        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        const std::string key = var.mapValue<std::string>("key", &ok);
        if (!ok) {
            invalidArgumentError(method, "key");
            return false;
        }
        const Variant value = var.value("value", &ok);
        if (!ok) {
            invalidArgumentError(method, "value");
            return false;
        }
        ScopedMutex lock(sMutex);
        mPrivate->persistentData[deviceAccount][key] = value;
        NTRACE(TRACE_SECURE_STORE, "Queuing write because setItem('%s', '%s') dak: %s",
               key.c_str(), value.toJSON().c_str(), deviceAccount.c_str());
        queueWrite();
        break; }
    case Methods::StorageBridge::removeItem: {
        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        const std::string key = var.mapValue<std::string>("key", &ok);
        if (!ok) {
            invalidArgumentError(method, "key");
            return false;
        }
        ScopedMutex lock(sMutex);
        mPrivate->persistentData[deviceAccount].remove(key);
        NTRACE(TRACE_SECURE_STORE, "Queuing write because removeItem('%s') dak: %s", key.c_str(), deviceAccount.c_str());
        queueWrite();
        break; }
    case Methods::StorageBridge::clear: {
        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        ScopedMutex lock(sMutex);
        mPrivate->persistentData.remove(deviceAccount);
        NTRACE(TRACE_SECURE_STORE, "Queuing write because clear() dak: %s", deviceAccount.c_str());
        queueWrite();
        break; }
    case Methods::StorageBridge::clearAll: {
        ScopedMutex lock(sMutex);
        mPrivate->persistentData = Variant();
        mPrivate->transientData = Variant();
        NTRACE(TRACE_SECURE_STORE, "Queuing write because clearAll()");
        queueWrite();
        break; }
    case Methods::StorageBridge::diskStoreClear: {
        ScopedMutex lock(sMutex);
        shared_ptr<DiskStore::Context> context = mPrivate->findContext(var);
        if (!context) {
            invalidArgumentError(method, "context");
            return false;
        }
        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        addRequest(new StorageRequest(StorageRequest::DISKSTORE_CLEAR, index, context, deviceAccount));
        break; }
    case Methods::StorageBridge::diskStoreClearAll: {
        ScopedMutex lock(sMutex);
        shared_ptr<DiskStore::Context> context = mPrivate->findContext(var);
        if (!context) {
            invalidArgumentError(method, "context");
            return false;
        }
        addRequest(new StorageRequest(StorageRequest::DISKSTORE_CLEAR_ALL, index, context));
        break; }
    case Methods::StorageBridge::diskStoreCorrupt: {
        ScopedMutex lock(sMutex);
        shared_ptr<DiskStore::Context> context = mPrivate->findContext(var);
        if (!context) {
            invalidArgumentError(method, "context");
            return false;
        }

        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        const std::string key = var.mapValue<std::string>("key", &ok);
        if (!ok) {
            invalidArgumentError(method, "key");
            return false;
        }
        const DataBuffer mode = var.mapValue<std::string>("mode", &ok);
        if (!ok) {
            invalidArgumentError(method, "mode");
            return false;
        }

        addRequest(new StorageRequest(StorageRequest::DISKSTORE_CORRUPT, index, context, deviceAccount, key, mode));
        break; }
    case Methods::StorageBridge::diskStoreCreateContext: {
        ScopedMutex lock(sMutex);

        const std::string context = var.mapValue<std::string>("context");
        Variant ret;
        if (context.empty()) {
            ret["success"] = false;
            ret["error"] = "Missing context";
            invalidArgumentError(method, "context");
            return ret;
        }

        const int size = var.mapValue<int>("size");
        if (size <= 0) {
            ret["success"] = false;
            ret["error"] = "Invalid size";
            invalidArgumentError(method, "size");
            return ret;
        }

        unsigned int flags = 0;
        if (var.mapValue<bool>("encrypted"))
            flags |= DiskStore::Encrypted;
        if (var.mapValue<bool>("signature"))
            flags |= DiskStore::Signature;
        std::string err;
        if (mPrivate->diskStore->create(context, size, flags, &err).get()) {
            ret["success"] = true;
        } else {
            ret["success"] = false;
        }
        return ret; }
    case Methods::StorageBridge::diskStoreDestroyContext: {
        shared_ptr<DiskStore::Context> ctx;
        const std::string context = var.mapValue<std::string>("context");
        {
            ScopedMutex lock(sMutex);
            if (context.empty()) {
                invalidArgumentError(method, "context");
                Variant result;
                result["success"] = false;
                result["error"] = "Missing context name";
                return result;
            }
            ctx = mPrivate->diskStore->find(context);
        }
        Variant result;
        if (ctx) {
            result["success"] = true;
            ctx->destroy();
        } else {
            result["success"] = false;
            result["error"] = "Invalid context: " + context;
        }
        return result; }
    case Methods::StorageBridge::diskStoreGetSize: {
        ScopedMutex lock(sMutex);
        shared_ptr<DiskStore::Context> context = mPrivate->findContext(var);
        if (!context) {
            invalidArgumentError(method, "context");
            return false;
        }
        addRequest(new StorageRequest(StorageRequest::DISKSTORE_GET_SIZE, index, context));
        break; }
    case Methods::StorageBridge::diskStoreCreate: {
        ScopedMutex lock(sMutex);
        shared_ptr<DiskStore::Context> context = mPrivate->findContext(var);
        if (!context) {
            invalidArgumentError(method, "context");
            return false;
        }

        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        const std::string key = var.mapValue<std::string>("key", &ok);
        if (!ok) {
            invalidArgumentError(method, "key");
            return false;
        }
        const DataBuffer value = var.mapValue<DataBuffer>("value", &ok);
        if (!ok) {
            invalidArgumentError(method, "value");
            return false;
        }

        addRequest(new StorageRequest(StorageRequest::DISKSTORE_CREATE, index, context, deviceAccount, key, value));
        break; }
    case Methods::StorageBridge::diskStoreAppend: {
        ScopedMutex lock(sMutex);

        shared_ptr<DiskStore::Context> context = mPrivate->findContext(var);
        if (!context) {
            invalidArgumentError(method, "context");
            return false;
        }

        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        const std::string key = var.mapValue<std::string>("key", &ok);
        if (!ok) {
            invalidArgumentError(method, "key");
            return false;
        }
        const DataBuffer value = var.mapValue<DataBuffer>("value", &ok);
        if (!ok) {
            invalidArgumentError(method, "value");
            return false;
        }

        addRequest(new StorageRequest(StorageRequest::DISKSTORE_APPEND, index, context, deviceAccount, key, value));
        break; }
    case Methods::StorageBridge::diskStoreRemove: {
        ScopedMutex lock(sMutex);

        shared_ptr<DiskStore::Context> context = mPrivate->findContext(var);
        if (!context) {
            invalidArgumentError(method, "context");
            return false;
        }

        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        const std::string key = var.mapValue<std::string>("key", &ok);
        if (!ok) {
            invalidArgumentError(method, "key");
            return false;
        }

        addRequest(new StorageRequest(StorageRequest::DISKSTORE_REMOVE, index, context, deviceAccount, key));
        break; }
    case Methods::StorageBridge::diskStoreRead: {
        ScopedMutex lock(sMutex);

        shared_ptr<DiskStore::Context> context = mPrivate->findContext(var);
        if (!context) {
            invalidArgumentError(method, "context");
            return false;
        }
        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        const std::string key = var.mapValue<std::string>("key", &ok);
        if (!ok) {
            invalidArgumentError(method, "key");
            return false;
        }
        const int begin = var.mapValue<int>("begin", &ok);
        if (!ok) {
            invalidArgumentError(method, "begin");
            return false;
        }
        const int end = var.mapValue<int>("end", &ok);
        if (!ok) {
            invalidArgumentError(method, "end");
            return false;
        }
        addRequest(new StorageRequest(StorageRequest::DISKSTORE_READ, index, context, deviceAccount, key, DataBuffer(), begin, end));
        break; }
    case Methods::StorageBridge::diskStoreInfo: {
        ScopedMutex lock(sMutex);

        Variant map;
        Variant info;
        if (shared_ptr<DiskStore> diskStore = nrdApp()->diskStore()) {
            DumpInfo dump;
            const std::string context = var.mapValue<std::string>("context", &ok);
            if (ok)
                dump.match = context;
            diskStore->dump(dump, &info);
        }

        map["idx"] = index;
        map["info"] = info;
        sendEvent("diskStoreInfo", map);
        break; }
    case Methods::StorageBridge::setPersistentData: {
        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        const std::string key = var.mapValue<std::string>("key", &ok);
        if (!ok || filterAuthKeys(key)) {
            invalidArgumentError(method, "key");
            return false;
        }
        std::string data = var.mapValue<std::string>("data", &ok);
        if (!ok) {
            invalidArgumentError(method, "data");
            return false;
        }
        const bool deflate = var.mapValue<bool>("deflate", 0, false);
        if (deflate) {
            std::string encoded;
            StringCompressor::deflateB64(encoded, data);
            data = encoded;
        }

        shared_ptr<SecureStore> secureStore = nrdApp()->secureStore();
        secureStore->setByAccount(deviceAccount, key, data);
        Variant map;
        map["idx"] = index;
        queueWrite();
        sendEvent("persistentDataSet", map);
        break; }
    case Methods::StorageBridge::unsetPersistentData: {
        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        const std::string key = var.mapValue<std::string>("key", &ok);
        if (!ok || filterAuthKeys(key)) {
            invalidArgumentError(method, "key");
            return false;
        }
        shared_ptr<SecureStore> secureStore = nrdApp()->secureStore();
        secureStore->unsetByAccount(deviceAccount, key);
        Variant map;
        map["idx"] = index;
        queueWrite();
        sendEvent("persistentDataUnset", map);
        break; }
    case Methods::StorageBridge::getPersistentData: {
        const std::string deviceAccount = var.mapValue<std::string>("dak", &ok);
        if (!ok) {
            invalidArgumentError(method, "dak");
            return false;
        }
        const std::string key = var.mapValue<std::string>("key", &ok);
        if (!ok || filterAuthKeys(key)) {
            invalidArgumentError(method, "key");
            return false;
        }
        Variant map;
        map["idx"] = index;
        std::string data = nrdApp()->secureStore()->getByAccount(deviceAccount, key);
        const bool inflate = var.mapValue<bool>("inflate", 0, false);
        if (inflate) {
            std::string decoded;
            StringCompressor::inflateB64(decoded, data);
            data = decoded;
        }
        map["data"] = data;
        sendEvent("persistentData", map);
        break; }
    case Methods::StorageBridge::getFlushCount: {
        int count = 0;
        {
            ScopedMutex lock(sMutex);
            std::swap(count, mPrivate->flushCount);
        }
        sendEvent("flushCount", Variant("count", count));
        break; }
    case Methods::StorageBridge::getUsedSecureStoreSize:
        return nrdApp()->secureStore()->calculateUsedSpace();
    case Methods::StorageBridge::flush: {
        NTRACE(TRACE_SECURE_STORE, "nrdp.storage.flush explicitly called");
        ScopedMutex lock(sMutex);
        if (mPrivate->writeTimer && mPrivate->writeTimer->isRunning())
            mPrivate->writeTimer->stop();
        addWriteTask();
        break; }
    case Methods::StorageBridge::diskStoreQuery: {
        Variant ret;
        {
            ScopedMutex lock(sMutex);
            shared_ptr<DiskStore::Context> ctx = mPrivate->diskStore->find(var.mapValue<std::string>("context"));
            if (!ctx) {
                invalidArgumentError(method, "context");
                return false;
            }

            const std::string deviceAccount = var.mapValue<std::string>("dak");
            const bool validate = var.mapValue<bool>("validate");
            const std::map<std::string, DiskStore::Context::Entry> keys = ctx->keys();

            const std::string prefix = var.mapValue<std::string>("prefix");
            ret["idx"] = index;
            if (!deviceAccount.empty())
                ret["dak"] = deviceAccount;
            std::map<std::string, DiskStore::Context::Entry>::const_iterator it = keys.begin();
            Variant matchedKeys(Variant::Type_StringMap);
            while (it != keys.end()) {
                std::string dak, key;
                if (decodeKey(it->first, dak, key)
                    && (deviceAccount.empty() || dak == deviceAccount)
                    && (prefix.empty() || startsWith(key, prefix))
                    && (!validate || ctx->validate(it->first))) {
                    Variant obj;
                    obj["size"] = it->second.size;
                    obj["fileName"] = ctx->fileName(it->first);
                    matchedKeys[key] = obj;
                }
                ++it;
            }
            ret["keys"] = matchedKeys;
        }
        sendEvent("query", ret);
        break; }
    default:
        return NfObject::invoke(method, var);
    }
    return Variant();
}

void StorageBridge::executeRequest(StorageRequest *request)
{
    assert(!sMutex.isLocked());
    assert(request);
    Variant map;
    switch (request->type) {
    case StorageRequest::DISKSTORE_GET_SIZE: {
        PERFORMANCE_MARKER_SCOPED("storagerequest.getsize");
        map["idx"] = request->index;
        map["size"] = request->context->size();
        map["usedSize"] = request->context->usedSize();
        map["freeSize"] = request->context->freeSize();
        sendEvent("diskStoreGetSize", map);
        break; }
    case StorageRequest::DISKSTORE_CLEAR_ALL: {
        PERFORMANCE_MARKER_SCOPED("storagerequest.clearall");
        map["success"] = request->context->clear();
        map["idx"] = request->index;
        map["idx"] = request->index;
        map["size"] = request->context->size();
        map["usedSize"] = request->context->usedSize();
        map["freeSize"] = request->context->freeSize();
        sendEvent("diskStoreClearAll", map);
        break; }
    case StorageRequest::DISKSTORE_CLEAR: {
        PERFORMANCE_MARKER_SCOPED("storagerequest.clear");
        const std::map<std::string, DiskStore::Context::Entry> keys = request->context->keys();
        std::map<std::string, DiskStore::Context::Entry>::const_iterator it = keys.begin();
        const std::string prefix = request->dak + "_";
        bool success = true;
        while (it != keys.end()) {
            if (!strncmp(it->first.c_str(), prefix.c_str(), prefix.size()) && !request->context->clear(it->first)) {
                success = false;
            }
            ++it;
        }
        map["success"] = success;
        map["idx"] = request->index;
        map["size"] = request->context->size();
        map["usedSize"] = request->context->usedSize();
        map["freeSize"] = request->context->freeSize();
        sendEvent("diskStoreClear", map);
        break; }
    case StorageRequest::DISKSTORE_CREATE: {
        PERFORMANCE_MARKER_SCOPED("storagerequest.create");
        const std::string key = encodeKey(request->dak, request->key);
        map["success"] = request->context->create(key, request->value);
        map["idx"] = request->index;
        map["dak"] = request->dak;
        map["key"] = request->key;
        map["usedSize"] = request->context->usedSize();
        map["freeSize"] = request->context->freeSize();
        sendEvent("diskStoreCreate", map);
        break; }
    case StorageRequest::DISKSTORE_CORRUPT: {
        PERFORMANCE_MARKER_SCOPED("storagerequest.corrupt");
        const std::string key = encodeKey(request->dak, request->key);
        if (request->value == "resize") {
            map["success"] = request->context->corrupt(key, DiskStore::Context::Resize);
        } else if (request->value == "modify") {
            map["success"] = request->context->corrupt(key, DiskStore::Context::Modify);
        } else {
            Log::error(TRACE_SECURE_STORE, "Invalid corruption mode: %s", request->value.constData());
            map["success"] = false;
        }
        map["idx"] = request->index;
        map["dak"] = request->dak;
        map["key"] = request->key;
        map["usedSize"] = request->context->usedSize();
        map["freeSize"] = request->context->freeSize();
        sendEvent("diskStoreCorrupt", map);
        break; }
    case StorageRequest::DISKSTORE_APPEND: {
        PERFORMANCE_MARKER_SCOPED("storagerequest.append");
        const std::string key = encodeKey(request->dak, request->key);
        map["success"] = request->context->append(key, request->value);
        map["idx"] = request->index;
        map["dak"] = request->dak;
        map["key"] = request->key;
        map["usedSize"] = request->context->usedSize();
        map["freeSize"] = request->context->freeSize();
        sendEvent("diskStoreAppend", map);
        break; }
    case StorageRequest::DISKSTORE_REMOVE: {
        PERFORMANCE_MARKER_SCOPED("storagerequest.remove");
        const std::string key = encodeKey(request->dak, request->key);
        map["success"] = request->context->clear(key);
        map["idx"] = request->index;
        map["dak"] = request->dak;
        map["key"] = request->key;
        map["usedSize"] = request->context->usedSize();
        map["freeSize"] = request->context->freeSize();
        sendEvent("diskStoreRemove", map);
        break; }
    case StorageRequest::DISKSTORE_READ: {
        PERFORMANCE_MARKER_SCOPED("storagerequest.read");
        const std::string key = encodeKey(request->dak, request->key);
        DataBuffer value;
        const bool success = request->context->read(key, value);
        if (request->begin < 0)
            request->begin = 0;
        if (request->end < 0 || request->end > value.size() - 1)
            request->end = std::max(0, value.size() - 1);
        map["success"] = success;
        map["idx"] = request->index;
        map["dak"] = request->dak;
        map["key"] = request->key;
        map["begin"] = request->begin;
        map["end"] = request->end;
        if (success && (request->begin != 0 || request->end != value.size() - 1)) {
            map["value"] = value.slice(request->begin, request->end - request->begin);
        } else {
            map["value"] = value;
        }
        sendEvent("diskStoreRead", map);
        break; }
    case StorageRequest::UISTORE_WRITE: {
        PERFORMANCE_MARKER_SCOPED("storagerequest.uiwrite");
        if (!mPrivate->shutdown) // it will be written from the end of stop
            writeNow();
        break; }
    }
}

void StorageBridge::event(ApplicationEvent::Type type, const Variant &)
{
    if (type == ApplicationEvent::Event_FactoryReset) {
        mPrivate->diskStore->reset();
        propertyUpdated(Properties::StorageBridge::diskStoreInfo);
    }
}

shared_ptr<DiskStore> StorageBridge::diskStore() const
{
    ScopedMutex lock(StorageBridge::sMutex);
    return mPrivate->diskStore;
}
