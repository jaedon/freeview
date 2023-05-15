/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NrdDiskCache_h
#define NrdDiskCache_h

#include <nrdnet/DiskCache.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/Variant.h>
#include <nrd/NrdWriteLimiter.h>
#include <string>

namespace netflix {

class NrdDiskCache : public DiskCache, public NrdWriteLimiter
{
public:
    NrdDiskCache(const std::string &path);
protected:
    virtual void factoryReset();
    virtual bool writeAllowed(int bytes, const AseUrl &url);
    virtual void onBytesWritten(int bytes);
    virtual Variant stats() const;
    virtual std::string consoleDumpExtra() const;
    DataBuffer encryptionKey(EncryptionKeyMode mode);
};

}


#endif
