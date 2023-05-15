/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __DISK_STORE_H__
#define __DISK_STORE_H__

#include <nrdbase/Mutex.h>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/DumpInfo.h>

namespace netflix {
class SecureStore;
class Variant;

class DiskStore : private Noncopyable, public enable_shared_from_this<DiskStore>
{
public:
    DiskStore(const shared_ptr<SecureStore> &secureStore);
    ~DiskStore();

    void init();

    enum Flag {
        None = 0x0,
        Signature = 0x1,
        Encrypted = 0x2
    };

    class ManifestScope
    {
    public:
        ManifestScope(const shared_ptr<DiskStore> &store);
        ~ManifestScope();

    private:
        const shared_ptr<DiskStore> mDiskStore;
    };

    class Context : public enable_shared_from_this<Context>
    {
    public:
        std::string name() const;
        std::string fileName(const std::string &key) const;
        int size() const;
        int usedSize() const;
        int freeSize() const;
        unsigned int flags() const;
        struct Entry {
            uint32_t size;
            DataBuffer signature;
        };
        std::map<std::string, Entry> keys() const;
        bool clear();
        bool clear(const std::string &key);
        bool create(const std::string &key, const DataBuffer &value);
        bool append(const std::string &key, const DataBuffer &value);
        bool read(const std::string &key, DataBuffer &value);
        bool validate(const std::string &key);
        enum CorruptionMode {
            Resize,
            Modify
        };
        bool corrupt(const std::string &key, CorruptionMode mode);
        void destroy();
    private:
        Context(const shared_ptr<DiskStore> &diskStore, const std::string &name, int size, unsigned int flags)
            : mDiskStore(diskStore), mName(name), mSize(size), mUsedSize(0), mFlags(flags)
        {}

        struct Lock
        {
            ~Lock();

            shared_ptr<DiskStore> diskStore;
        };

        bool lock(Lock & lock, const char *func) const;
        shared_ptr<DiskStore> diskStore() const;

        weak_ptr<DiskStore> mDiskStore;
        std::string mName;
        int mSize, mUsedSize;
        unsigned int mFlags;
        std::map<std::string, Entry> mFileMap;

        friend class DiskStore;
    };
    shared_ptr<Context> find(const::std::string &name) const;
    shared_ptr<Context> create(const::std::string &name, int size, unsigned int flags, std::string *error = 0);
    std::vector<std::string> contexts() const;

    void dump(const DumpInfo& info, Variant* variant = 0) const;

    std::string path(const std::string &context, const std::string &key) const;
    void reset();
private:
    enum InitializeKeyMode {
        RegenerateKey,
        LoadKey
    };
    bool initializeKey(InitializeKeyMode mode);
    bool loadManifest();
    bool saveManifest();
    bool encrypt(const unsigned char *in, uint32_t inSize, DataBuffer &out);
    bool decrypt(const unsigned char *in, uint32_t inSize, DataBuffer &out);

#ifndef NRDP_NO_FILESYSTEM
    shared_ptr<SecureStore> mSecureStore;
    mutable Mutex mMutex;
    int mManifestScopeDepth;
    bool mWriteDirty;
    DataBuffer mKey;
    std::string mPath;
    std::map<std::string, shared_ptr<Context> > mContexts;

    friend class Context;
    friend struct Context::Lock;
    friend class ManifestScope;
#endif
};
}

#endif // __DISK_STORE_H__
