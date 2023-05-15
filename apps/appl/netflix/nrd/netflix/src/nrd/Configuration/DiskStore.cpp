/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdbase/Configuration.h>
#include <nrdbase/EncodedFile.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Base64.h>
#include <nrdbase/Console.h>
#include <nrdbase/Dumper.h>
#include <nrdbase/ReadDir.h>
#include <nrdbase/Application.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/HMAC.h>
#include <nrdbase/Serialize.h>
#include <nrd/NrdApplication.h>
#include <openssl/evp.h>

#include "DiskStore.h"
#include "SecureStore.h"
#include "SystemData.h"

#ifndef NRDP_NO_FILESYSTEM
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#endif

#if DISKSTORE_ENABLE_DEBUG
#define DISKSTORE_DEBUG(FORMAT, ...) printf("DISKSTORE: %20s() - "FORMAT"\n", __FUNCTION__, ##__VA_ARGS__)
#define DISKSTORE_TRACE(FORMAT, ...) DISKSTORE_DEBUG(FORMAT, ##__VA_ARGS__)
#define DISKSTORE_ERROR(FORMAT, ...) DISKSTORE_DEBUG("ERROR - "FORMAT, ##__VA_ARGS__)
#else
#define DISKSTORE_DEBUG(FORMAT, ...)
#define DISKSTORE_TRACE(FORMAT, ...) NTRACE(TRACE_DISK_STORE, FORMAT, ##__VA_ARGS__)
#define DISKSTORE_ERROR(FORMAT, ...) Log::error(TRACE_DISK_STORE, "ERROR - " FORMAT, ##__VA_ARGS__)
#endif

#if DISKSTORE_ENABLE_PROFILE
#include <stdio.h>
#include <sys/times.h>
#define DISKSTORE_PROFILE_START(NAME) struct tms NAME##_tms; ullong NAME = times(&NAME##_tms);
#define DISKSTORE_PROFILE_STOP(NAME) printf("DISKSTORE: %20s() - "#NAME" (took %llu ms)\n", __FUNCTION__, times(&NAME##_tms) - NAME);
#else
#define DISKSTORE_PROFILE_START(NAME)
#define DISKSTORE_PROFILE_STOP(NAME)
#endif


using namespace netflix;

#ifdef NRDP_NO_FILESYSTEM
DiskStore::DiskStore(const shared_ptr<SecureStore>&) {}
DiskStore::~DiskStore() {}
void DiskStore::init() {}
void DiskStore::reset() {}
std::vector<std::string> DiskStore::contexts() const { return std::vector<std::string>(); }
shared_ptr<DiskStore::Context> DiskStore::create(const std::string &, int, unsigned int, std::string *) { return shared_ptr<Context>(); }
shared_ptr<DiskStore::Context> DiskStore::find(const std::string &) const { return shared_ptr<Context>(); }
bool DiskStore::Context::append(const std::string &, const DataBuffer &) { return false; }
bool DiskStore::Context::clear() { return false; }
unsigned int DiskStore::Context::flags() const { return 0; }
bool DiskStore::Context::validate(const std::string &) { return false; }
void DiskStore::Context::destroy() {}
std::string DiskStore::Context::fileName(const std::string &) const { return std::string(); }
bool DiskStore::Context::corrupt(const std::string &, CorruptionMode) { return false; }
bool DiskStore::Context::clear(const std::string &) { return false; }
bool DiskStore::Context::create(const std::string &, const DataBuffer &) { return false; }
std::map<std::string, DiskStore::Context::Entry> DiskStore::Context::keys() const { return std::map<std::string, Entry>(); }
void DiskStore::dump(const DumpInfo &, Variant*) const {}
bool DiskStore::Context::read(const std::string &, netflix::DataBuffer&) { return false; }
int DiskStore::Context::size() const { return 0; }
int DiskStore::Context::usedSize() const { return 0; }
int DiskStore::Context::freeSize() const { return 0; }
DiskStore::ManifestScope::ManifestScope(const shared_ptr<DiskStore> &) {}
DiskStore::ManifestScope::~ManifestScope() {}

#else
static const char *DISKSTORE_MANIFEST_VERSION = "v1.4";
static const char *DISKSTORE_MANIFEST_FILE = "manifest";
static const char *DISKSTORE_ENCRYPTION_IV = "_NFLX_DISKSTORE_";
static const char *DISKSTORE_MANIFEST_KEY = "Jmj3eBy7BJuvxBwr";
enum {
    DISKSTORE_KEY_SIZE = 16,
    DISKSTORE_ENABLE_DEBUG = 0,
    DISKSTORE_ENABLE_PROFILE = 0
};
DiskStore::DiskStore(const shared_ptr<SecureStore> &secureStore)
    : mSecureStore(secureStore), mMutex(DISKSTORE_MUTEX, "DiskStore"), mManifestScopeDepth(0), mWriteDirty(false)
{
    DISKSTORE_DEBUG("ctor");
}

DiskStore::~DiskStore()
{
    DISKSTORE_DEBUG("dtor");
}

void DiskStore::init()
{
    // get the disk store path from config
    mPath = Configuration::diskStorePath() + NRDP_PATH_SEPARATOR_CHAR + "diskstore";
    DISKSTORE_TRACE("initialize disk store - path[%s]", mPath.c_str());

    // initialize encryption key
    const bool loadedKey = initializeKey(LoadKey);

    // load manifest
    if (loadManifest() && !loadedKey) {
        ManifestScope scope(shared_from_this());
        int deleted = 0;
        std::map<std::string, shared_ptr<Context> >::iterator it = mContexts.begin();
        while (it != mContexts.end()) {
            if (it->second->flags() & (Signature|Encrypted)) {
                it++->second->destroy(); // destroy modifies mContexts
                ++deleted;
            } else {
                ++it;
            }
        }
        DISKSTORE_ERROR("Failed to load key, deleted %d contexts", deleted);
    }
}

shared_ptr<DiskStore::Context> DiskStore::find(const::std::string &name) const
{
    DISKSTORE_DEBUG("name[%s]", name.c_str());

    ScopedMutex lock(mMutex);
    return mapValue(mContexts, name);
}

std::vector<std::string> DiskStore::contexts() const
{
    ScopedMutex lock(mMutex);
    return mapKeys(mContexts);
}

shared_ptr<DiskStore::Context> DiskStore::create(const::std::string &name, int size,
                                                 unsigned int flags, std::string *error)
{
    DISKSTORE_DEBUG("name[%s] size[%d] signature[%s] encryption[%s]",
                    name.c_str(), size,
                    (flags & Signature ? "true" : "false"),
                    (flags & Encrypted ? "true" : "false"));

    ScopedMutex lock(mMutex);

    shared_ptr<Context> &context = mContexts[name];
    if (context) {
        const std::string err = StringFormatter::sformat("Context %s already exists", name.c_str());
        Log::error(TRACE_DISK_STORE, "%s", err.c_str());
        if (error)
            *error = err;
        return shared_ptr<Context>();
    }
    DISKSTORE_TRACE("create new context - context[%s]", name.c_str());
    const std::string path = mPath + "/" + name;
    // create context dir
    if (mkdir(path.c_str(), S_IRWXU) && errno != EEXIST) {
        const std::string err = StringFormatter::sformat("cannot create directory - path[%s] errno: %d", path.c_str(), errno);
        Log::error(TRACE_DISK_STORE, "%s", err.c_str());
        if (error)
            *error = err;
        DISKSTORE_ERROR("%s", err.c_str());
        mContexts.erase(name);
        return shared_ptr<Context>();
    }
    context.reset(new Context(shared_from_this(), name, size, flags));
    saveManifest();
    return context;
}

class DiskStoreDumper : public Dumper
{
public:
    virtual std::vector<std::string> consoleEntry(const unsigned int idx, const std::string& section, const Variant& value);
};

std::vector<std::string> DiskStoreDumper::consoleEntry(const unsigned int /*idx*/, const std::string& section, const Variant& value)
{
    assert(value.isStringMap());
    if (value.size() == 1) {
        // list
        return one(" " + value["name"].string());
    }
    // entry
    return one(StringFormatter::sformat("%s: %lld bytes used / %lld bytes total / %lld entries",
                                        section.c_str(), value["used"].integer(),
                                        value["total"].integer(), value["entries"].integer()));
}

void DiskStore::dump(const DumpInfo& info, Variant* variant) const
{
    DiskStoreDumper dumper;
    if (info.match.empty()) {
        dumper.keys() << "name";
        const std::vector<std::string>& ctxs = contexts();
        for (std::vector<std::string>::const_iterator it = ctxs.begin(); it != ctxs.end(); ++it) {
            dumper.values() << *it;
        }
    } else {
        shared_ptr<DiskStore::Context> ctx = find(info.match);
        if (ctx) {
            dumper.keys() << "used" << "total" << "entries" << "signature" << "encrypted";
            dumper.values(info.match) << ctx->usedSize() << ctx->size() << ctx->keys().size()
                                      << static_cast<bool>(ctx->flags() & Signature)
                                      << static_cast<bool>(ctx->flags() & Encrypted);
        }
    }

    if (variant) {
        *variant = dumper.variant();
    } else {
        const std::vector<std::string>& lines = dumper.toConsole();
        for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
            Log::warn(TRACE_DISK_STORE, "%s", it->c_str());
        }
    }
}

std::string DiskStore::Context::name() const
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return std::string();
    return mName;
}

std::string DiskStore::Context::fileName(const std::string &key) const
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return std::string();
    return l.diskStore->path(mName, key);
}

int DiskStore::Context::size() const
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return -1;
    return mSize;
}
int DiskStore::Context::usedSize() const
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return -1;
    return mUsedSize;
}

int DiskStore::Context::freeSize() const
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return -1;
    return mSize - mUsedSize;
}

unsigned int DiskStore::Context::flags() const
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return 0;
    return mFlags;
}

std::map<std::string, DiskStore::Context::Entry> DiskStore::Context::keys() const
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return std::map<std::string, Entry>();
    return mFileMap;
}

bool DiskStore::Context::clear()
{
    DISKSTORE_PROFILE_START(total);

    Lock l;
    if (!lock(l, __FUNCTION__))
        return false;

    DISKSTORE_TRACE("clear - context[%s] key[*]", mName.c_str());

    // clear context dir
    const std::string path = l.diskStore->mPath + "/" + mName;
    ReadDir::recursiveRemoveDirectory(path);
    ReadDir::recursiveMakeDirectory(path);

    // reset context used size and manifest
    mUsedSize = 0;
    mFileMap.clear();

    DISKSTORE_PROFILE_STOP(total);

    l.diskStore->saveManifest();

    return true;
}

bool DiskStore::Context::clear(const std::string &key)
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return false;
    DISKSTORE_PROFILE_START(total);

    DISKSTORE_TRACE("clear - context[%s] key[%s]", mName.c_str(), key.c_str());

    // make sure file is in manifest
    std::map<std::string, DiskStore::Context::Entry>::iterator it = mFileMap.find(key);
    if (it == mFileMap.end()) {
        DISKSTORE_TRACE("not in manifest - key[%s]", key.c_str());
        return false;
    }

    // remove the file
    const std::string path = l.diskStore->mPath + "/" + mName + "/" + key;
    mUsedSize -= (*it).second.size;
    mFileMap.erase(it);
    if (unlink(path.c_str())) {
        DISKSTORE_ERROR("could not remove file %s - key[%s] errno: %d", path.c_str(), key.c_str(), errno);
        return false;
    }

    DISKSTORE_PROFILE_STOP(total);
    l.diskStore->saveManifest();

    return true;
}

bool DiskStore::Context::create(const std::string &key, const DataBuffer &value)
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return false;

    int estimatedSize = value.size();
    if (mFlags & Signature)
        estimatedSize += EVP_MAX_MD_SIZE + sizeof(uint32_t);

    DISKSTORE_PROFILE_START(total);

    DISKSTORE_TRACE("create - context[%s] key[%s]", mName.c_str(), key.c_str());

    // initialize data
    const unsigned char *data = value.data();
    uint32_t dataSize = value.size();

    // encrypt data before writing to file
    DataBuffer encryptedData;
    if (mFlags & Encrypted) {
        if (!l.diskStore->encrypt(data, dataSize, encryptedData)) {
            DISKSTORE_ERROR("could not encrypt data - key[%s]", key.c_str());
            return false;
        }
        data = encryptedData.data();
        dataSize = encryptedData.size();
    }

    // entry already in manifest so subtract current size
    const std::map<std::string, DiskStore::Context::Entry>::iterator it = mFileMap.find(key);
    uint32_t currentSize = 0;
    if (it != mFileMap.end())
        currentSize = (*it).second.size;

    // check if we have enough space left
    if (mSize >= 0 && mSize - mUsedSize + currentSize < dataSize) {
        DISKSTORE_ERROR("no space left");
        return false;
    }

    if (it != mFileMap.end()) {
        mUsedSize -= currentSize;
        mFileMap.erase(it);
    }

    // write data and signature to file
    const std::string path = l.diskStore->path(mName, key);
    size_t lastSlash = key.rfind('/');
    if (lastSlash != std::string::npos) {
        lastSlash += (path.size() - key.size());
        DISKSTORE_TRACE("Creating directory %s (%s)", path.substr(0, lastSlash).c_str(), path.c_str());
        ReadDir::recursiveMakeDirectory(path.substr(0, lastSlash));
    }

    {
        EncodedFile file;
        if (!file.open(path, EncodedFile::Write)) {
            DISKSTORE_ERROR("could not open file for reading %s - key[%s] errno: %d", path.c_str(), key.c_str(), errno);
            return false;
        }

        if (!file.write(data, dataSize)) {
            DISKSTORE_ERROR("could not write file %s errno: %d", path.c_str(), errno);
            unlink(path.c_str());
            return false;
        }
    }

    // add to manifest and used size
    Entry &entry = mFileMap[key];
    DISKSTORE_TRACE("Added(%s): %s (file is %s)", mName.c_str(), key.c_str(), path.c_str());
    entry.size = dataSize;
    // calculate signature
    if (mFlags & Signature) {
        HMAC hmac(l.diskStore->mKey);
        hmac.update(data, dataSize);
        entry.signature = hmac.final<DataBuffer>();
    }

    mUsedSize += dataSize;

    DISKSTORE_DEBUG("  writeSize[%d] usedSize[%d] size[%d]", dataSize, mUsedSize, mSize);
    DISKSTORE_PROFILE_STOP(total);

    return l.diskStore->saveManifest();
}

bool DiskStore::Context::append(const std::string &key, const DataBuffer &value)
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return false;
    DISKSTORE_DEBUG("context[%s] key[%s] value[%s]", mName.c_str(), key.c_str(), value.c_str());
    DISKSTORE_PROFILE_START(total);

    const std::map<std::string, Entry>::iterator it = mFileMap.find(key);
    if (it == mFileMap.end()) {
        return create(key, value);
    }

    if (flags() & Encrypted) {
        // file is in manifest so read existing data and clear
        DataBuffer data;
        if (!read(key, data)) {
            l.diskStore->saveManifest();
            return false;
        }

        DISKSTORE_TRACE("clearing for append - context[%s] key[%s]", mName.c_str(), key.c_str());

        // make sure file is in manifest
        // remove the file
        const std::string path = l.diskStore->mPath + "/" + mName + "/" + key;
        mUsedSize -= (*it).second.size;
        mFileMap.erase(it);
        if (unlink(path.c_str())) {
            DISKSTORE_ERROR("could not remove file %s - key[%s] errno: %d", path.c_str(), key.c_str(), errno);
        }

        // if appended data creation fails attempt restoring original data
        if (!create(key, data + value)) {
            if (!create(key, data))
                l.diskStore->saveManifest();
            return false;
        }
    } else if (static_cast<int>(value.size()) > freeSize()) {
        DISKSTORE_ERROR("Not enough space to write more data for - key[%s](%zu/%d)", key.c_str(), value.size(), freeSize());
        return false;
    } else {
        const std::string path = l.diskStore->path(mName, key);
        int fd = open(path.c_str(), O_RDWR);
        if (fd == -1) {
            DISKSTORE_ERROR("could not open file for append %s - key[%s] errno: %d", path.c_str(), key.c_str(), errno);
            clear(key);
            return false;
        }

        if (flags() & Signature) {
            HMAC hmac(l.diskStore->mKey);
            char buf[1024];
            size_t read = 0;
            while (read < it->second.size) {
                const int s = std::min<int>(sizeof(buf), it->second.size - read);
                if (::read(fd, buf, s) != s) {
                    DISKSTORE_ERROR("Failed to read from file in append %s (%zu/%u/%d) - key[%s] errno: %d",
                                    path.c_str(), read, it->second.size, s, key.c_str(), errno);
                    close(fd);
                    clear(key);
                    return false;
                }
                hmac.update(buf, s);
                read += s;
            }

            hmac.update(value);
            it->second.signature = hmac.final<DataBuffer>();
        } else {
            if (lseek(fd, 0, SEEK_END) == -1) {
                DISKSTORE_ERROR("Failed to seek to end of file in append %s - key[%s] errno: %d",
                                path.c_str(), key.c_str(), errno);
                close(fd);
                clear(key);
                return false;
            }
        }

        if (::write(fd, value.data(), value.size()) != static_cast<int>(value.size())) {
            DISKSTORE_ERROR("could not write data for append %s. errno: %d", path.c_str(), errno);
            close(fd);
            clear(key);
            return false;
        }

        it->second.size += value.size();
        mUsedSize += value.size();
        close(fd);

        l.diskStore->saveManifest();
    }

    DISKSTORE_PROFILE_STOP(total);

    return true;
}

bool DiskStore::Context::read(const std::string &key, DataBuffer &value)
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return false;

    DISKSTORE_PROFILE_START(total);

    DISKSTORE_TRACE("read - context[%s] key[%s]", mName.c_str(), key.c_str());

    value.clear();

    // make sure file is in manifest
    std::map<std::string, Entry>::const_iterator it = mFileMap.find(key);
    if (it == mFileMap.end()) {
        DISKSTORE_TRACE("not in manifest - key[%s]", key.c_str());
        return false;
    }

    // open file for read
    const std::string path = l.diskStore->path(mName, key);
    const DataBuffer contents = DataBuffer::fromFile(path, it->second.size);
    if (contents.isEmpty()) {
        DISKSTORE_ERROR("could not open file for reading %s - key[%s] errno: %d", key.c_str(), path.c_str(), errno);
        clear(key);
        return false;
    }

    if (contents.size() != static_cast<int>(it->second.size)) {
        DISKSTORE_ERROR("unexpected file size - key[%s]. Expected %d, got %zu", key.c_str(),
                        it->second.size, contents.size());
        clear(key);
        return false;
    }

    if (mFlags & Signature) {
        HMAC hmac(l.diskStore->mKey);
        const DataBuffer signature = hmac.hmac<DataBuffer>(contents);
        if (signature != it->second.signature) {
            DISKSTORE_ERROR("signature mismatch - key[%s]", key.c_str());
            clear(key);
            return false;
        }
    }

    if (!(mFlags & Encrypted)) {
        value = contents;
    } else if (!l.diskStore->decrypt(contents.data(), contents.size(), value)) {
        DISKSTORE_ERROR("could not decrypt file %s - key[%s]", path.c_str(), key.c_str());
        clear(key);
        return false;
    }

    DISKSTORE_DEBUG("  value[%s]", value.c_str());
    DISKSTORE_PROFILE_STOP(total);

    return true;
}

void DiskStore::Context::destroy()
{
    Lock l;
    if (!lock(l, __FUNCTION__)) {
        return;
    }
    DISKSTORE_TRACE("release context - context[%s]", mName.c_str());
    ReadDir::recursiveRemoveDirectory(l.diskStore->mPath + "/" + mName);
    l.diskStore->mContexts.erase(mName);
    l.diskStore->saveManifest();
}

bool DiskStore::Context::validate(const std::string &key)
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return false;

    std::map<std::string, Entry>::const_iterator it = mFileMap.find(key);
    if (it == mFileMap.end()) {
        DISKSTORE_TRACE("not in manifest - key[%s]", key.c_str());
        return false;
    }
    const std::string fn = l.diskStore->path(mName, key);
    struct stat st;
    bool ok = false;
    if (!stat(fn.c_str(), &st) && S_ISREG(st.st_mode) && static_cast<size_t>(st.st_size) == it->second.size) {
        if (mFlags & Signature) {
            char buf[16384];
            FILE *f = fopen(fn.c_str(), "r");
            if (f) {
                HMAC hmac(l.diskStore->mKey);
                while (true) {
                    const int r = fread(buf, 1, sizeof(buf), f);
                    if (r > 0)
                        hmac.update(buf, r);
                    if (r < static_cast<int>(sizeof(buf)))
                        break;
                }
                fclose(f);

                const DataBuffer signature = hmac.final<DataBuffer>();
                ok = signature == it->second.signature;
            }
        } else {
            ok = true;
        }
    }

    if (!ok)
        clear(key);
    return ok;
}

bool DiskStore::Context::corrupt(const std::string &key, CorruptionMode mode)
{
    Lock l;
    if (!lock(l, __FUNCTION__))
        return false;

    std::map<std::string, Entry>::const_iterator it = mFileMap.find(key);
    if (it == mFileMap.end()) {
        DISKSTORE_TRACE("not in manifest - key[%s]", key.c_str());
        return false;
    }
    const std::string fn = l.diskStore->path(mName, key);
    if (mode == Resize) {
        if (!truncate(fn.c_str(), it->second.size - 1))
            return true;
        DISKSTORE_ERROR("could not truncate file %s - key[%s] errno: %d", fn.c_str(), key.c_str(), errno);
        clear(key);
        return false;
    }

    int fd = open(fn.c_str(), O_RDWR);
    if (fd == -1) {
        DISKSTORE_ERROR("could not open file for corrupting %s - key[%s] errno: %d", fn.c_str(), key.c_str(), errno);
        clear(key);
        return false;
    }
    const char buf = '\0';
    for (size_t i=0; i<it->second.size; ++i) {
        if (write(fd, &buf, sizeof(buf)) != 1) {
            DISKSTORE_ERROR("could not corrupt file %s - key[%s] errno: %d", fn.c_str(), key.c_str(), errno);
            close(fd);
            clear(key);
            return false;
        }
    }

    return true;
}

DiskStore::Context::Lock::~Lock()
{
    if (diskStore)
        diskStore->mMutex.unlock();
}

bool DiskStore::Context::lock(Lock & lock, const char *func) const
{
    lock.diskStore = mDiskStore.lock();
    if (lock.diskStore) {
        if (mapValue(lock.diskStore->mContexts, mName).get() == this) {
            lock.diskStore->mMutex.lock();
            return true;
        }

        lock.diskStore.reset();
        DISKSTORE_ERROR("Stale context: %s %s", func, mName.c_str());
        return false;
    }

    (void)func;
    DISKSTORE_ERROR("Can't lock disk store: %s", func);
    return false;
}

std::string DiskStore::path(const std::string &name, const std::string &key) const
{
    return mPath + NRDP_PATH_SEPARATOR_CHAR + name + NRDP_PATH_SEPARATOR_CHAR + key;
}

void DiskStore::reset()
{
    ScopedMutex lock(mMutex);
    DISKSTORE_PROFILE_START(total);
    DISKSTORE_TRACE("reset - clear disk store");

    // clear top level disk store path
    ReadDir::recursiveRemoveDirectory(mPath);
    ReadDir::recursiveMakeDirectory(mPath);
    DISKSTORE_TRACE("clear - context[*]");
    mContexts.clear();
    mWriteDirty = false;
    initializeKey(RegenerateKey);

    DISKSTORE_PROFILE_STOP(total);
}

bool DiskStore::initializeKey(InitializeKeyMode mode)
{
    DISKSTORE_PROFILE_START(total);
    DISKSTORE_TRACE("loading encryption and signature key from secure store");

    // get key from secure store
    std::string encoded;
    if (mode == LoadKey)
        encoded = mSecureStore->get<std::string>(SystemKeys::DiskStoreKey);

    bool ret = false;
    if (!encoded.empty()) {
        // decode key and set member variable
        mKey = Base64::decode(DataBuffer(encoded));
        ret = true;
    } else {
        DISKSTORE_TRACE("no key in secure store - generate new key");
        mKey = Application::generateSecret(DISKSTORE_KEY_SIZE);
        mSecureStore->set(SystemKeys::DiskStoreKey, Base64::encode(mKey).toString());
    }

    DISKSTORE_PROFILE_STOP(total);
    return ret;
}

static inline bool hasFiles(const std::string &path)
{
    class HasFilesReadDir : public ReadDir
    {
    public:
        bool hasEntries;
        virtual Result visitEntry(const Entry &)
        {
            hasEntries = true;
            return Stop;
        }
    };
    HasFilesReadDir r;
    r.hasEntries = false;
    r.visit(path);
    return r.hasEntries;
}

bool DiskStore::loadManifest()
{
    DISKSTORE_PROFILE_START(total);
    DISKSTORE_TRACE("loading manifest");

    // open manifest file
    const std::string manifestPath = mPath + NRDP_PATH_SEPARATOR_CHAR + DISKSTORE_MANIFEST_FILE;
    const DataBuffer contents = DataBuffer::fromFile(manifestPath, 1024 * 1024);
    if (contents.isEmpty()) {
        if (hasFiles(mPath))
            DISKSTORE_ERROR("signature problem 1 - path[%s]", manifestPath.c_str());
        reset();
        return false;
    }

    if (contents.size() < static_cast<int>(sizeof(int))) {
        reset();
        DISKSTORE_ERROR("signature problem 2 - path[%s]", manifestPath.c_str());
        return false;
    }

    int signatureSize;
    memcpy(&signatureSize, contents.constData(), sizeof(signatureSize));
    if (static_cast<size_t>(contents.size()) <= signatureSize + sizeof(int)) {
        reset();
        DISKSTORE_ERROR("signature problem 3 - path[%s]", manifestPath.c_str());
        return false;
    }

    const DataBuffer signature = contents.slice(sizeof(int), signatureSize);
    const DataBuffer data = contents.slice(signatureSize + sizeof(int));
    DataBuffer actualSignature = HMAC(DataBuffer::fromRawData(DISKSTORE_MANIFEST_KEY, DISKSTORE_KEY_SIZE)).hmac<DataBuffer>(data);

    if (actualSignature != signature) {
        actualSignature = HMAC(mKey).hmac<DataBuffer>(data);
        if (actualSignature != signature) {
            DISKSTORE_ERROR("signature mismatch - path[%s]", manifestPath.c_str());
            reset();
            return false;
        } else {
            Log::warn(TRACE_DISK_STORE, "Transitioned manifest from ephemeral key to static key");
        }
    }

    Deserializer deserializer(data);
    std::string version;
    deserializer >> version;
    if (strcmp(version.c_str(), DISKSTORE_MANIFEST_VERSION)) {
        DISKSTORE_ERROR("incompatible version %s/%s", version.c_str(), DISKSTORE_MANIFEST_VERSION);
        reset();
        return false;
    }

    size_t count;
    deserializer >> count;
    shared_ptr<DiskStore> that = shared_from_this();
    for (size_t i=0; i<count; ++i) {
        std::string name;
        unsigned int flags;
        int size;
        deserializer >> name >> flags >> size;
        shared_ptr<Context> &context = mContexts[name];
        context.reset(new Context(that, name, size, static_cast<Flag>(flags)));
        size_t entries;
        deserializer >> context->mUsedSize >> entries;
        for (size_t j=0; j<entries; ++j) {
            std::string key;
            deserializer >> key;
            Context::Entry &entry = context->mFileMap[key];
            DISKSTORE_TRACE("Read manifest entry(%s): %s (%s)", name.c_str(), path(name, key).c_str(), key.c_str());
            deserializer >> entry.size >> entry.signature;
        }
    }

    DISKSTORE_PROFILE_STOP(total);
    return true;
}

bool DiskStore::saveManifest()
{
    if (mManifestScopeDepth) {
        mWriteDirty = true;
        return true;
    }
    assert(mMutex.isLocked());
    DISKSTORE_PROFILE_START(total);
    DISKSTORE_TRACE("sync manifest");

    // open manifest file
    std::string err;
    const std::string path = mPath + NRDP_PATH_SEPARATOR_CHAR + DISKSTORE_MANIFEST_FILE;
    {
        EncodedFile file;
        if (!file.open(path, EncodedFile::Write)) {
            DISKSTORE_ERROR("Could not open manifest file for writing - %s errno: %d", path.c_str(), errno);
            reset();
            return false;
        }

        Serializer serializer;
        serializer << std::string(DISKSTORE_MANIFEST_VERSION) << mContexts.size();

        DISKSTORE_TRACE("manifest context count[%zu]", mContexts.size());
        for (std::map<std::string, shared_ptr<Context> >::const_iterator it = mContexts.begin(); it != mContexts.end(); ++it) {
            serializer << it->second->mName << it->second->mFlags << it->second->mSize << it->second->mUsedSize << it->second->mFileMap.size();
            std::map<std::string, Context::Entry>::const_iterator cit = it->second->mFileMap.begin();
            while (cit != it->second->mFileMap.end()) {
                serializer << cit->first << cit->second.size << cit->second.signature;
                ++cit;
            }
        }

        HMAC hmac(DataBuffer::fromRawData(DISKSTORE_MANIFEST_KEY, DISKSTORE_KEY_SIZE));
        const DataBuffer signature = hmac.hmac<DataBuffer>(serializer.data());
        const DataBuffer data = serializer.data();
        file << signature;
        if (!file.write(data.data(), data.size())) {
            DISKSTORE_ERROR("could not write to manifest file: %s errno: %d", path.c_str(), errno);
            reset();
            return false;
        }
    }

    DISKSTORE_PROFILE_STOP(total);
    return true;
}

bool DiskStore::encrypt(const unsigned char *in, uint32_t inSize, DataBuffer &out)
{
    int outSize, outSizeFinal;

    // initialize cipher
    EVP_CIPHER_CTX cipher;
    EVP_CIPHER_CTX_init(&cipher);

    // initialize encrypt
    if (!EVP_EncryptInit_ex(&cipher, EVP_aes_128_cbc(), NULL, mKey.constData(),
                            reinterpret_cast<const unsigned char *>(DISKSTORE_ENCRYPTION_IV))) {
        DISKSTORE_ERROR("EVP_EncryptInit_ex() failed");
        EVP_CIPHER_CTX_cleanup(&cipher);
        return false;
    }

    // reserve enough out buffer for both EVP_EncryptUpdate and EVP_EncryptFinal_ex
    out.resize(inSize + (EVP_CIPHER_CTX_block_size(&cipher) * 2) - 1);

    // update encrypt - max out buffer size (inSize + cipher_block_size - 1)
    if (!EVP_EncryptUpdate(&cipher, &out[0], &outSize, in, inSize)) {
        DISKSTORE_ERROR("EVP_EncryptUpdate() failed");
        EVP_CIPHER_CTX_cleanup(&cipher);
        return false;
    }

    // final decrypt - max out buffer size (cipher_block_size)
    if (!EVP_EncryptFinal_ex(&cipher, &out[outSize], &outSizeFinal)) {
        DISKSTORE_ERROR("EVP_EncryptFinal_ex() failed");
        EVP_CIPHER_CTX_cleanup(&cipher);
        return false;
    }

    // clean up cipher
    EVP_CIPHER_CTX_cleanup(&cipher);

    // sanity check to make sure we didn't exceed our buffer
    assert(static_cast<int>(out.size()) >= outSize + outSizeFinal);

    // resize to actual size
    out.resize(outSize + outSizeFinal);

    return true;
}

DiskStore::ManifestScope::ManifestScope(const shared_ptr<DiskStore> &store)
    : mDiskStore(store)
{
    assert(store);
    ScopedMutex lock(store->mMutex);
    ++store->mManifestScopeDepth;
}

DiskStore::ManifestScope::~ManifestScope()
{
    assert(mDiskStore);
    ScopedMutex lock(mDiskStore->mMutex);
    if (!--mDiskStore->mManifestScopeDepth && mDiskStore->mWriteDirty) {
        mDiskStore->mWriteDirty = false;
        mDiskStore->saveManifest();
    }
}

bool DiskStore::decrypt(const unsigned char *in, uint32_t inSize, DataBuffer &out)
{
    int outSize, outSizeFinal;

    // initialize cipher
    EVP_CIPHER_CTX cipher;
    EVP_CIPHER_CTX_init(&cipher);

    // initialize decrypt
    if (!EVP_DecryptInit_ex(&cipher, EVP_aes_128_cbc(), NULL, mKey.constData(),
                            reinterpret_cast<const unsigned char*>(DISKSTORE_ENCRYPTION_IV))) {
        DISKSTORE_ERROR("EVP_DecryptInit_ex() failed");
        EVP_CIPHER_CTX_cleanup(&cipher);
        return false;
    }

    // reserve enough out buffer for both EVP_DecryptUpdate and EVP_DecryptFinal_ex
    out.resize(inSize + (EVP_CIPHER_CTX_block_size(&cipher) * 2));

    // update decrypt - max out buffer size (inSize + cipher_block_size)
    if (!EVP_DecryptUpdate(&cipher, &out[0], &outSize, in, inSize)) {
        DISKSTORE_ERROR("EVP_DecryptUpdate() failed");
        EVP_CIPHER_CTX_cleanup(&cipher);
        return false;
    }

    // final decrypt - max out buffer size (cipher_block_size)
    if (!EVP_DecryptFinal_ex(&cipher, &out[outSize], &outSizeFinal)) {
        DISKSTORE_ERROR("EVP_DecryptFinal_ex() failed");
        EVP_CIPHER_CTX_cleanup(&cipher);
        return false;
    }

    // clean up cipher
    EVP_CIPHER_CTX_cleanup(&cipher);

    // sanity check to make sure we didn't exceed our buffer
    assert(static_cast<int>(out.size()) >= outSize + outSizeFinal);

    // resize to actual size
    out.resize(outSize + outSizeFinal);

    return true;
}
#endif // NRDP_NO_FILESYSTEM
