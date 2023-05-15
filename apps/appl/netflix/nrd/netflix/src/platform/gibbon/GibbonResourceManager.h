/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef GIBBONRESOURCEMANAGER_H
#define GIBBONRESOURCEMANAGER_H

#include <Rect.h>
#include <Debug.h>
#include <Surface.h>
#include <ScriptEngine.h>
#include <SurfaceDecoder.h>
#include <GibbonCookieManager.h>

#include <string>

#include <nrd/config.h>
#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdnet/DiskCache.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/DataBuffer.h>
#include <nrdnet/ResourceManager.h>
#include <nrdbase/Application.h>
#include <nrd/NrdWriteLimiter.h>
#include <nrd/NrdDiskCache.h>

namespace netflix {
namespace gibbon {

class Surface;
class SurfaceDecodeProcess;

class GibbonDiskCache : public NrdDiskCache
{
public:
    GibbonDiskCache();
    int writeSpeed() const { return mWriteSpeed; } // bytes/sec
protected:
    virtual void writeSpeedMeasured(const Time &time, int bytes);
    virtual void factoryReset();
    virtual Response::Type allowedType(const shared_ptr<ResourceRequest> &request) const;
    virtual bool writeAllowed(int bytes, const AseUrl &url);
private:
    void updateConfiguration();
    int mWriteSpeed;
};

class GibbonResource : public Resource
{
public:
    inline GibbonResource() {}

    virtual int cost() const { return Resource::cost() + DataBuffer::hashLength(hashType()); }
    virtual std::string describe() const;
    std::string decodeDescription() const;

    DataBuffer hash(DataBuffer::Hash type, DataBuffer::Encoding encoding) const;
    DataBuffer::Hash hashType() const;

    void setSuspended(bool suspend);

    virtual SurfaceDecoder::SurfaceInfo surfaceInfo() const;
    virtual Surface::SharedPtr surfaceDecode(int group, uint8_t flags) const;

    virtual bool shouldSuspend() const;

private:
#if defined(GIBBON_SURFACEDECODER_FORK)
    mutable shared_ptr<SurfaceDecodeProcess> mProcess;
#endif
    DataBuffer mHash;
    friend class GibbonResourceManager;
};

class GibbonResourceManager : public ResourceManager
{
public:
    GibbonResourceManager();
    ~GibbonResourceManager();
    inline void setLanguage(const std::string &language) { ScopedMutex lock(mMutex); mLanguage = language; }
    inline void setUserAgent(const std::string &userAgent) { ScopedMutex lock(mMutex); mUserAgent = userAgent; }
    inline shared_ptr<GibbonCookieManager> cookieManager() { return mCookieManager; }
    enum ResourceFlag {
        Surface            = 0x0001,
        Script             = 0x0002,
        Trusted            = 0x0004,
        Gibbon             = 0x0008,
        Format_XML         = 0x0010,
        Format_JSON        = 0x0020,
        Format_JSONStream  = 0x0040,
        Format_UInt8Array  = 0x0080,
        Format_ArrayBuffer = 0x0100,
        Format_String      = 0x0200,
        ScriptDownload     = 0x0400,
        HasHashSHA1        = Trusted|0x0800,
        HasHashSHA256      = Trusted|0x1000,
        HashRequest        = Trusted|0x2000,
        MainThread         = Script|Format_XML|Format_JSON|Format_JSONStream
    };

    static bool isNBP(const AseUrl &url);
    static bool isSecure(const AseUrl &url);

    virtual Resource::SharedPtr createResource(const ResourceRequest::SharedPtr &request);
    static bool validateHash(const ResourceRequest::SharedPtr &request, const Resource::SharedPtr &resource);
    static Resource::Method methodFromString(const std::string &method);
    static void setupRequest(const shared_ptr<ResourceRequest> &request, const Variant &data);
    shared_ptr<GibbonDiskCache> diskCache() const { return static_pointer_cast<GibbonDiskCache>(ResourceManager::diskCache()); }
protected:
    virtual void startRequest(const ResourceRequest::SharedPtr &request);
    virtual void requestRedirected(const ResourceRequest::SharedPtr &request,
                                   const Resource::SharedPtr &resource);
    virtual void resourceFinished(const Resource::SharedPtr &resource, Resource::State state);
    virtual void requestFinished(const ResourceRequest::SharedPtr &request,
                                 const Resource::SharedPtr &response,
                                 Resource::State state);
    virtual int sslVerify(int status, void *ctx, const Resource::SharedPtr &resource,
                          const NrdIpAddr &ipAddress);
    virtual shared_ptr<DiskCache> createDiskCache();

private:
    std::string cookies(const ResourceRequest::SharedPtr &request);
    bool handleLocalControl(const ResourceRequest::SharedPtr &request);
    void setupTestDiskCacheHeaders(const ResourceRequest::SharedPtr &request);

    Mutex mMutex;
    std::string mLanguage, mUserAgent;
    shared_ptr<GibbonCookieManager> mCookieManager;

};

class GibbonResourceRequest : public ResourceRequest
{
public:
    typedef shared_ptr<GibbonResourceRequest> SharedPtr;

    GibbonResourceRequest(unsigned int flags, const AseUrl &url, Resource::Method method);
    inline Resource::SharedPtr resource() { return mResource; }
    inline Resource::State state() const { return mState; }

    inline void clearResponse()
    {
        assert(!resourceManager());
        mResource.reset();
    }
    inline void complete()
    {
        assert(mResource);
        finish();
    }
    virtual void onComplete(const Resource::SharedPtr &resource, Resource::State state);
protected:
    virtual void finish() {}
private:
    Resource::SharedPtr mResource;
    Resource::State mState;
    friend class GibbonResourceManager;
};

}
}

#endif
