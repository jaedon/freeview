/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NrdDiskCache.h"
#include "NrdApplication.h"
#include <nrdbase/Base64.h>
#include <nrdbase/StringUtils.h>

using namespace netflix;

NrdDiskCache::NrdDiskCache(const std::string &path)
    : DiskCache(path), NrdWriteLimiter("diskCache")
{
    NrdWriteLimiter::init(Configuration::diskCacheWriteLimit());
}

DataBuffer NrdDiskCache::encryptionKey(EncryptionKeyMode mode)
{
    enum { KeySize = 16 };
    NrdApplication *app = nrdApp();
    if (mode == EncryptionKey_Load) {
        const std::string encoded = app->readSystemConfiguration(SystemKeys::DiskCacheKey);
        if (!encoded.empty()) {
            const std::vector<unsigned char> v(encoded.begin(), encoded.end());
            const std::vector<unsigned char> key = Base64::decode(v);
            if (key.size() == KeySize)
                return key;

            if (!key.empty()) {
                Log::error(TRACE_DISK_CACHE,
                           "Invalid key loaded from system configuration (%zu vs %d)",
                           key.size(), KeySize);
            }
        }
    }
    const DataBuffer key = Application::generateSecret(KeySize);
    app->writeSystemConfiguration(SystemKeys::DiskCacheKey, key.toBase64().toString());
    return key;
}

bool NrdDiskCache::writeAllowed(int bytes, const AseUrl &url)
{
    (void)url;
    if (WriteLimiter::isEnabled() && available() < bytes) {
        block(bytes);
        NTRACE(TRACE_WRITELIMITER, "Not writing %s (%d bytes, %d allowed) because of write limiter",
               url.c_str(), bytes, available());
        return false;
    }
    return true;
}

void NrdDiskCache::onBytesWritten(int bytes)
{
    commit(bytes);
}

Variant NrdDiskCache::stats() const
{
    Variant ret = DiskCache::stats();
    ret["used"] = used();
    ret["limit"] = limit();
    return ret;
}

std::string NrdDiskCache::consoleDumpExtra() const
{
    const int u = used();
    const int l = limit();
    return StringFormatter::sformat("WriteLimiter: %s/%s (%.1f%%)", prettyBytes(u).c_str(), prettyBytes(l).c_str(),
                                    l ? static_cast<float>((u / static_cast<float>(l))) * 100. : 0.);
}


void NrdDiskCache::factoryReset()
{
    DiskCache::factoryReset();
    setLimit(Configuration::diskCacheWriteLimit());
    resetUsed();
}
