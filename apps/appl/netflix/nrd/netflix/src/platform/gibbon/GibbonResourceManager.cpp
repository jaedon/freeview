/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "GibbonResourceManager.h"

#include "Debug.h"
#include "Widget.h"
#include "RemoteScreen.h"
#include "SurfaceCache.h"
#include "SurfaceDecoder.h"
#include "GibbonApplication.h"
#include "GibbonConfiguration.h"

#include <nrd/AppLog.h>

#include <nrdbase/Base64.h>
#include <nrdbase/EmbeddedResources.h>
#include <nrdbase/StringCompressor.h>
#include <nrdbase/UrlEncoder.h>
#include <nrdbase/tr1.h>

#include <nrdnet/X509Cert.h>

using namespace netflix;
using namespace netflix::gibbon;
using namespace netflix::mediacontrol;

#if defined(GIBBON_SURFACEDECODER_FORK)
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

namespace netflix {
namespace gibbon {

class SurfaceDecodeProcess : public enable_shared_from_this<SurfaceDecodeProcess>
{
public:
    typedef shared_ptr<SurfaceDecodeProcess> SharedPtr;

    ~SurfaceDecodeProcess() {
        kill(mPid, SIGKILL);

        close(mEncodedPipe[0]);
        close(mEncodedPipe[1]);
        close(mDecodedPipe[0]);
        close(mDecodedPipe[1]);

        int status;
        waitpid(mPid, &status, 0);
        NTRACE(TRACE_RESOURCEMANAGER, "ProcessCleanup: %d [%d]", mPid, status);
    }

    static SurfaceDecodeProcess::SharedPtr fork() {
        SurfaceDecodeProcess::SharedPtr result;
        {
            ScopedMutex _lock(sProcessesMutex);
            if(!sProcesses.empty()) {
                result = sProcesses.front();
                sProcesses.pop_front();
            }
        }
        if(!result) {
            result = SurfaceDecodeProcess::SharedPtr(new SurfaceDecodeProcess);
            NTRACE(TRACE_RESOURCEMANAGER, "ProcessCreate: %p", result.get());
        } else {
            NTRACE(TRACE_RESOURCEMANAGER, "ProcessResuse: %p", result.get());
        }
        return result;
    }

    inline bool isSuspended() const {
        ScopedMutex _lock(mMutex);
        return mSuspended;
    }

    inline std::string describe() const {
        std::ostringstream result;
        result << "Process[" << mPid;
        {
            ScopedMutex _lock(mMutex);
            if(mSuspended)
                result << ":SUSPENDED";
        }
        result << "]";
        return result.str();
    }

    static void cleanup() {
        NTRACE(TRACE_RESOURCEMANAGER, "ProcessesCleanup");
        std::list<SurfaceDecodeProcess::SharedPtr> processes;
        {
            ScopedMutex _lock(sProcessesMutex);
            std::swap(processes, sProcesses);
        }
    }

    inline Surface::SharedPtr decode(const DataBuffer &data, const SurfaceDecoder::SurfaceInfo &info, int group) {
        shared_ptr<SurfaceCache> surfaceCache = GibbonApplication::instance()->getSurfaceCache();
        while(surfaceCache->isDecodeSuspended()) {
            NTRACE(TRACE_RESOURCEMANAGER, "ProcessWait: %d", mPid);
            ScopedMutex _lock(mMutex);
            mSuspendCond.wait(&mMutex);
        }
        NTRACE(TRACE_RESOURCEMANAGER, "ProcessDecode: %d", mPid);

        Surface::Format format;
        if(info.flags & Surface::Flags_Text)
            format = GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Text);
        else if(info.flags & Surface::Flags_Alpha)
            format = GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Alpha);
        else
            format = GibbonApplication::instance()->getScreen()->getFormat(Screen::SurfaceFormat_Opaque);

        NRDP_UNUSED_RETURN(write(mEncodedPipe[1], &format, sizeof(format)));
        NRDP_UNUSED_RETURN(write(mEncodedPipe[1], &info.size.width, sizeof(info.size.width)));
        NRDP_UNUSED_RETURN(write(mEncodedPipe[1], &info.size.height, sizeof(info.size.width)));
        {
            const int length = data.size();
            NRDP_UNUSED_RETURN(write(mEncodedPipe[1], &length, sizeof(length)));
            NRDP_UNUSED_RETURN(write(mEncodedPipe[1], data.constData(), length));
        }

        Surface::SharedPtr surface = surfaceCache->requestSurface(info.size.width, info.size.height, info.flags, true, group);
        {
            Surface::Data data(surface, Surface::Data::WriteLock);
            assert(data.format() == format);
            const int bytesPerLine = info.size.width*Surface::bytesPerPixel(format);
            for(int y = 0; y < info.size.height; ++y) {
                const ssize_t r = read(mDecodedPipe[0], data.scanline(y), bytesPerLine);
                if(r <= 0) {
                    Log::error(TRACE_RESOURCEMANAGER, "ProcessSuspendError(%d): %zd", mPid, r);
                    if(!r) {
                        surface.reset();
                        break;
                    }
                }
            }
        }
        if(!GibbonApplication::instance()->hasQuit()) {
            ScopedMutex _lock(sProcessesMutex);
            sProcesses.push_back(shared_from_this());
        }
        return surface;
    }

    inline void setSuspended(bool suspend) {
        NTRACE(TRACE_RESOURCEMANAGER, "Process%s: %d", suspend ? "Suspend" : "Resume", mPid);
        ScopedMutex _lock(mMutex);
        if(suspend) {
            if(!mSuspended++)
                kill(mPid, SIGSTOP);
        } else if(mSuspended) {
            if(!--mSuspended)
                kill(mPid, SIGCONT);
        }
        mSuspendCond.broadcast();
    }

private:
    static int read(int pipe, void *data, size_t length) {
        for(size_t offset = 0; offset != length; ) {
            const ssize_t r = ::read(pipe, ((unsigned char*)data)+offset, length-offset);
            if(!r)
                return 0;
            else if(r == -1)
                continue;
            offset += r;
        }
        return length;
    }

    inline SurfaceDecodeProcess() : mMutex(ZERO_MUTEX, "GibbonResource::SurfaceDecodeProcess"), mSuspended(0) {
        if(pipe(mEncodedPipe) == -1)
            Log::error(TRACE_RESOURCEMANAGER, "Unable to create pipe for SurfaceDecodeProcess(encode) (%d)", errno);
        if(pipe(mDecodedPipe) == -1)
            Log::error(TRACE_RESOURCEMANAGER, "Unable to create pipe for SurfaceDecodeProcess(decode) (%d)", errno);
        mPid = ::fork();
        if(!mPid) {
            ObjectCount::init();
            int encoded_data_length = 0, decoded_data_length = 0;
            unsigned char *encoded_data = 0, *decoded_data = 0;
            while(true) {
                Surface::Format format;
                if(!read(mEncodedPipe[0], &format, sizeof(format)))
                    break;
                SurfaceDecoder::DecodeInfo decode;
                if(!read(mEncodedPipe[0], &decode.info.size.width, sizeof(decode.info.size.width)))
                    break;
                if(!read(mEncodedPipe[0], &decode.info.size.height, sizeof(decode.info.size.width)))
                    break;

                int encoded_length;
                if(!read(mEncodedPipe[0], &encoded_length, sizeof(encoded_length)))
                    break;
                if(encoded_length > encoded_data_length)
                    encoded_data = (unsigned char*)realloc(encoded_data, encoded_data_length=encoded_length);
                if(!read(mEncodedPipe[0], encoded_data, encoded_length))
                    break;

                shared_ptr<GibbonResource> resource(new GibbonResource);
                resource->setResponseData(DataBuffer::fromRawData(encoded_data, encoded_length));
                SurfaceDecoder::SharedPtr decoder = SurfaceDecoders::instance()->decoder(resource);
                assert(decoder && !(decoder->flags() & SurfaceDecoder::Flag_NeedSurface));
                const int decoded_length = decode.info.size.width*decode.info.size.height*Surface::bytesPerPixel(format);
                if(decoded_length > decoded_data_length)
                    decoded_data = (unsigned char*)realloc(decoded_data, decoded_data_length=decoded_length);
                decode.data.lock(decoded_data, decode.info.size.width, decode.info.size.height, format, Surface::Data::WriteLock);
                decoder->decode(&decode);
                for(int offset = 0; offset != decoded_length; ) {
                    const ssize_t w = write(mDecodedPipe[1], decoded_data+offset, decoded_length-offset);
                    if(w <= 0)
                        break;
                    offset += w;
                }
            }
            if(encoded_data)
                free(encoded_data);
            if(decoded_data)
                free(decoded_data);
            close(mEncodedPipe[0]);
            close(mEncodedPipe[1]);
            close(mDecodedPipe[0]);
            close(mDecodedPipe[1]);
            NTRACE(TRACE_RESOURCEMANAGER, "ProcessDone: %d", getpid());
            _exit(0);
        }
        NTRACE(TRACE_RESOURCEMANAGER, "ProcessFork: %d", mPid);
    }

    pid_t mPid;
    int mEncodedPipe[2], mDecodedPipe[2];

    mutable Mutex mMutex;
    int mSuspended;
    ConditionVariable mSuspendCond;

    static Mutex sProcessesMutex;
    static std::list<SurfaceDecodeProcess::SharedPtr> sProcesses;
};
Mutex SurfaceDecodeProcess::sProcessesMutex(ZERO_MUTEX, "SurfaceDecodeProcess::SurfaceDecodeProcesses");
std::list<SurfaceDecodeProcess::SharedPtr> SurfaceDecodeProcess::sProcesses;
}}
#endif

GibbonDiskCache::GibbonDiskCache()
    : NrdDiskCache(Configuration::diskCachePath() + "/diskcache/"), mWriteSpeed(-1)
{
}

DiskCache::Response::Type GibbonDiskCache::allowedType(const shared_ptr<ResourceRequest> &request) const
{
    return (request->flags() & GibbonResourceManager::Trusted) ? Response::All : Response::Secure;
}

bool GibbonDiskCache::writeAllowed(int bytes, const AseUrl &url)
{
    const bool ret = NrdDiskCache::writeAllowed(bytes, url);
    if (!ret) {
        Variant data;
        data["url"] = url.str();
        if (gibbonApp() && gibbonApp()->getBridge())
            gibbonApp()->getBridge()->sendEvent("diskCacheWriteLimiter", data);
    }
    return ret;
}

void GibbonDiskCache::updateConfiguration()
{
    shared_ptr<GibbonBridge> bridge = gibbonApp()->getBridge();
    if (!bridge)
        return;
    const int index = bridge->propertyIndex("diskCacheConfiguration");
    if (index != -1) {
        bridge->propertyUpdated(index);
    }
}

void GibbonDiskCache::factoryReset()
{
    NrdDiskCache::factoryReset();
    updateConfiguration();
}

void GibbonDiskCache::writeSpeedMeasured(const Time &time, int bytes)
{
    if (time.ms()) {
        mWriteSpeed = (bytes / time.ms()) * 1000;
        updateConfiguration();
    }
}

DataBuffer::Hash GibbonResource::hashType() const
{
    switch (requestFlags() & (GibbonResourceManager::HasHashSHA1|GibbonResourceManager::HasHashSHA256)) {
    case 0:
        break;
    case GibbonResourceManager::HasHashSHA256|GibbonResourceManager::HasHashSHA1:
        assert(0);
        break;
    case GibbonResourceManager::HasHashSHA256:
        return DataBuffer::Hash_SHA256;
    case GibbonResourceManager::HasHashSHA1:
        return DataBuffer::Hash_SHA1;
    }
    return DataBuffer::Hash_None;
}

DataBuffer GibbonResource::hash(DataBuffer::Hash type, DataBuffer::Encoding encoding) const
{
    if (type == hashType())
        return mHash.encode(encoding);
    return responseData().hash(type).encode(encoding);
}

std::string GibbonResource::describe() const
{
    std::ostringstream result;
    result << Resource::describe();
    const unsigned int flags = requestFlags();
    if (flags & GibbonResourceManager::Surface) {
        result << " surface";
        if (Surface::SharedPtr surface = GibbonApplication::instance()->getSurfaceCache()->find(cacheKey()))
            result << "(" << surface->describe() << ")";
    } else {
        if (flags & (GibbonResourceManager::Script|GibbonResourceManager::ScriptDownload))
            result << " script";
        if (flags & GibbonResourceManager::Trusted)
            result << " trusted";
    }
#if defined(GIBBON_SURFACEDECODER_FORK)
    if(mProcess)
        result << mProcess->describe();
#endif
    return result.str();
}

GibbonResourceManager::GibbonResourceManager()
    : ResourceManager(), mMutex(ZERO_MUTEX, "GibbonResourceManager"), mCookieManager(new GibbonCookieManager)
{
    setCapacity(Configuration::resourceManagerCapacity());
}

GibbonResourceManager::~GibbonResourceManager()
{
#if defined(GIBBON_SURFACEDECODER_FORK)
    SurfaceDecodeProcess::cleanup();
#endif
}

bool GibbonResourceManager::isNBP(const AseUrl &url)
{
    if(url.GetScheme() == "http") {
#ifdef NRDP_WEBSERVER_ENABLED
        if(url.GetPortNumber() == NrdConfiguration::nbpdPort() && (url.GetHost() == "127.0.0.1" || url.GetHost() == "localhost"))
            return true;
#endif
        return url.GetHost() == "localcontrol.netflix.com";
    }
    return false;
}

bool GibbonResourceManager::isSecure(const AseUrl &url)
{
    return url.GetScheme() != "http" || isNBP(url);
}

void GibbonResourceManager::requestRedirected(const ResourceRequest::SharedPtr &request, const Resource::SharedPtr &resource)
{
    if (shared_ptr<DebuggerBridge> debugger = GibbonApplication::instance()->getDebugger())
        debugger->endRequest(request, resource, Resource::State_Network);
    ResourceManager::requestRedirected(request, resource);
}

void GibbonResourceManager::resourceFinished(const Resource::SharedPtr &resource, Resource::State state)
{
    mCookieManager->processHeaders(resource->url(), resource->responseHeaders());

    if (resource->requestFlags() & Gibbon) {
        shared_ptr<GibbonResource> res = static_pointer_cast<GibbonResource>(resource);
        const DataBuffer::Hash hashType = res->hashType();
        if (hashType != DataBuffer::Hash_None)
            res->mHash = resource->responseData().hash(hashType);

        if ((resource->requestFlags() & Surface) && !(resource->requestFlags() & ResourceRequest::Synchronous)) {
            ScopedMutex _lock(Widget::lock());
            ResourceManager::resourceFinished(resource, state);
            return;
        }
    }
    ResourceManager::resourceFinished(resource, state);
}

void GibbonResourceManager::requestFinished(const ResourceRequest::SharedPtr &request,
                                            const Resource::SharedPtr &resource,
                                            Resource::State state)
{
    if (shared_ptr<DebuggerBridge> debugger = GibbonApplication::instance()->getDebugger())
        debugger->endRequest(request, resource, state);
    ResourceManager::requestFinished(request, resource, state);
}

static bool parseHashHeader(const ResourceRequest::SharedPtr &request,
                            DataBuffer::Hash *hashType,
                            std::string *headerPtr,
                            DataBuffer::Encoding *encoding)
{
    if (!request->hasRequestHeader("X-Gibbon-Hash")) {
        if (hashType)
            *hashType = DataBuffer::Hash_None;
        if (encoding)
            *encoding = DataBuffer::Encoding_None;
        if (headerPtr)
            headerPtr->clear();
        return true;
    }

    std::string header = request->requestHeader("X-Gibbon-Hash");
    trim(header);

    if (startsWith(header, "sha256=")) {
        if (hashType)
            *hashType = DataBuffer::Hash_SHA256;
        if (encoding)
            *encoding = (header.size() == 71) ? DataBuffer::Encoding_Hex : DataBuffer::Encoding_Base64;
        if (headerPtr)
            *headerPtr = header.substr(7);
    } else if (startsWith(header, "sha1=")) {
        if (hashType)
            *hashType = DataBuffer::Hash_SHA1;
        if (encoding)
            *encoding = (header.size() == 45) ? DataBuffer::Encoding_Hex : DataBuffer::Encoding_Base64;
        if (headerPtr)
            *headerPtr = header.substr(5);
    } else {
        if (hashType)
            *hashType = DataBuffer::Hash_None;
        if (encoding)
            *encoding = DataBuffer::Encoding_None;
        if (headerPtr)
            headerPtr->clear();
        Log::error(TRACE_RESOURCEMANAGER, "Invalid X-Gibbon-Hash header. [%s] for %s", header.c_str(), request->url().c_str());
        return false;
    }
    return true;
}

Resource::SharedPtr GibbonResourceManager::createResource(const ResourceRequest::SharedPtr &request)
{
    Resource::SharedPtr resource(new GibbonResource);
    registerResource(request, resource);
    return resource;
}

GibbonResourceRequest::GibbonResourceRequest(unsigned int flags, const AseUrl &url, Resource::Method method)
    : ResourceRequest(flags | GibbonResourceManager::Gibbon,
                      ResourceManager::fixRelativeUrl(GibbonApplication::instance()->getLocation(), url),
                      method),
      mState(Resource::State_None)
{
}

void GibbonResourceRequest::onComplete(const Resource::SharedPtr &resource, Resource::State state)
{
    assert(resource);
    // if we're synchronous we don't want to post an event to the js thread
    // since we'd let other events and timers slip in. In that case the js
    // thread thread has been halted all along waiting for this request to
    // finish so we just return so the startRequest() can call finish from
    // the right thread()

    mState = state;
    if (!GibbonResourceManager::validateHash(shared_from_this(), resource)) {
        mResource = resourceManager()->createResource(shared_from_this());
        mResource->setHttpStatusCode(400);
        mResource->setErrorCode(NetErrorCode::INVALIDHASH_ERROR);
    } else {
        mResource = resource;
    }

    const unsigned int requestFlags = flags();

    if (requestFlags & ResourceRequest::Synchronous) {
        return; // caller of sendRequest will call finish
    }

    if (requestFlags & GibbonResourceManager::MainThread) {
        GibbonResourceRequest::SharedPtr req = static_pointer_cast<GibbonResourceRequest>(shared_from_this());
        GibbonApplication::instance()->callInMainThread<GibbonResourceRequest>(req, &GibbonResourceRequest::complete);
    } else {
        complete();
    }
}

static inline void removeDoubleDots(std::string &path)
{
    size_t pos = 0;
    while (pos != std::string::npos) {
        pos = path.find("/../", pos);
        if (pos != std::string::npos) {
            path.erase(pos, 3);
            if (pos > 0) {
                size_t beg = path.rfind('/', pos - 1);
                path.erase(beg, pos - beg);
                pos = 0;
            }
        }
    }
}

bool GibbonResourceManager::handleLocalControl(const ResourceRequest::SharedPtr &request)
{
    Stopwatch sw;
    const AseUrl url = request->url();
    if (!GibbonResourceManager::isNBP(url))
        return false;
    std::string path = url.GetPath();
    if (path == "/uiboot") {
        request->setUrl(NrdConfiguration::bootRequest().mapValue<std::string>("url"));
        return false;
    } else if (path == "/favicon.ico") {
        request->setUrl(AseUrl("http://www.netflix.com/favicon.ico"));
        return false;
    }

    Resource::SharedPtr resource = createResource(request);
    assert(resource);

    const Variant params = Variant::fromQueryString(UrlEncoder::decode(url.GetQuery()));
    removeDoubleDots(path);
    Time mTime;
    DataBuffer content = GibbonApplication::instance()->resourceContent(path, params, &mTime);
    if (content.empty())
        content = GibbonApplication::instance()->dataURI(path);
    if (!content.isEmpty()) {
        NRDP_OBJECTCOUNT_DESCRIBE(content, "LOCALRESOURCE:"+request->url().str());
        HttpResponseHeaders headers;
        const std::string mimetype = ResourceManager::mimeType(url);
        if (!mimetype.empty())
            headers.push_back(std::make_pair("Content-Type", mimetype));
        if (mTime.ms())
            headers.push_back(std::make_pair("Last-Modified", mTime.rfc1123()));

        finish(resource, AS_NO_ERROR, 200, Resource::State_File,
               NetworkMetrics(0, 0, static_cast<uint32_t>(sw.elapsed())),
               headers, content);
    } else {
        finish(resource, AS_NO_ERROR, 404, Resource::State_File);
    }
    return true;
}

std::string GibbonResourceManager::cookies(const ResourceRequest::SharedPtr &request)
{
    return mCookieManager->cookies(request->url(), GibbonCookieManager::HttpOnly);
}

void GibbonResourceManager::setupTestDiskCacheHeaders(const ResourceRequest::SharedPtr &request)
{
    const AseUrl url = request->url();
    std::string key;
    int maxAge = -1;
    int refresh = -1;
    if (startsWith(url.str(), "https://api-global.netflix.com/")) {
        key = url.str();
        const size_t uuid_beg = key.find("&uuid=");
        if (uuid_beg == std::string::npos)
            return;

        size_t uuid_end = key.find("&", uuid_beg + 6);
        if (uuid_end == std::string::npos)
            uuid_end = key.length();
        key = key.replace(uuid_beg, uuid_end - uuid_beg, "") + cookies(request);
        refresh = 60;
        maxAge = 28800; // 8 hours
    } else if (contains(url.str(), "netflix.com/") && !contains(url.str(), "uiboot.netflix.com")) {
        maxAge = 86400; // 24 hours
        if (contains(url.str(), ".js"))
            refresh = 60;
    }
    if (maxAge != -1) {
        std::ostringstream str;
        str << "key=" << UrlEncoder::encode(key.empty() ? url.str() : key)
            << ",max-age=" << maxAge;
        if (refresh != -1)
            str << ",refresh=60";
        request->setRequestHeader("X-Gibbon-Cache-Control", str.str());
    }
}

void GibbonResourceManager::startRequest(const ResourceRequest::SharedPtr &request)
{
    request->setResourceManager(shared_from_this());
    if (handleLocalControl(request))
        return;
    const AseUrl url = request->url();
    if (request->flags() & Gibbon) {
        DataBuffer::Hash hash;
        const bool hashSuccess = ::parseHashHeader(request, &hash, 0, 0);
        if (hashSuccess) {
            switch (hash) {
            case DataBuffer::Hash_None:
                if (Debug::TestNetworkSecure || NrdConfiguration::allowInsecureNetworkRequests()) {
                    request->setFlag(Trusted);
                }
                break;
            case DataBuffer::Hash_SHA1:
                request->setFlag(HasHashSHA1);
                break;
            case DataBuffer::Hash_SHA256:
                request->setFlag(HasHashSHA256);
                break;
            case DataBuffer::Hash_MD5:
                assert(0);
                break;
            }
        }

        if (!hashSuccess || (!(request->flags() & Trusted) && !GibbonResourceManager::isSecure(url))) {
            Log::error(TRACE_RESOURCEMANAGER, "Blocked request %s", url.c_str());
            Resource::SharedPtr resource = request->resource();
            if (!resource)
                resource = createResource(request);
            finish(resource, AS_ILLEGAL_REQUEST, 400);
            return;
        }
    }

    if (Debug::TestDiskCache)
        setupTestDiskCacheHeaders(request);

    request->setRequestHeader("Cookie", cookies(request));
    request->setRequestHeader("Referer", GibbonApplication::instance()->getLocation().str());
    std::string language, userAgent;
    {
        ScopedMutex lock(mMutex);
        language = mLanguage;
        userAgent = mUserAgent;
    }
    request->setRequestHeader("Language", language);
    request->setRequestHeader("User-Agent", userAgent);

    if (request->flags() & (Script|ScriptDownload)) {
        if (!request->hasRequestHeader("X-Gibbon-Type"))
            request->setRequestHeader("X-Gibbon-Type", "application/javascript;compiled");
        if (!request->hasRequestHeader("Accept"))
            request->setRequestHeader("Accept", "application/javascript,text/javascript,application/x-javascript");
    } else if (!request->hasRequestHeader("Accept")) {
        request->setRequestHeader("Accept", "*/*");
    }

#ifdef NRDP_HAS_TRACING
    std::string h;
    const HttpHeaders headers = request->requestHeaders();
    for (HttpHeaders::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        if (it != headers.begin())
            h += '\n';
        h += (it->first + ": " + it->second);
    }

    NTRACE(TRACE_RESOURCEMANAGER, "Preparing request %s with headers\n%s", url.c_str(), h.c_str());
#endif

    bool send = true;
    if((request->flags() & Surface)) {
        if(shared_ptr<RemoteScreenServer> server = gibbonApp()->getRemoteScreenServer()) {
            if(shared_ptr<GibbonResource> resource = server->handleSurfaceDownload(request)) {
                registerResource(request, resource);
                send = false;
            }
        }
    }
    if(send)
        ResourceManager::startRequest(request);
    if (shared_ptr<DebuggerBridge> debugger = GibbonApplication::instance()->getDebugger())
        debugger->beginRequest(request);
}

int GibbonResourceManager::sslVerify(int status, void *x509ctx, const Resource::SharedPtr &resource,
                                     const NrdIpAddr &ipAddress)
{
    X509_STORE_CTX *ctx = static_cast<X509_STORE_CTX *>(x509ctx);
    if (!ctx) {
        Log::info(TRACE_RESOURCEMANAGER, "[CERT STATUS] : NULL params, x509 store ctx is NULL.");
        return 0;
    }

    if (!status) {
        Log::Message message;
        message.m_traceArea = &TRACE_RESOURCEMANAGER;

        message.m_tags["nwerr"] = "untrustedcert";
        message.m_tags["url"] = resource->url().str();
        message.m_tags["ipAddress"] = ipAddress.getStrAddr();

        // Get the SSL cert for the server failing the SSL signature, i.e. it's
        // the leaf of the x509 chain.
        if (ctx) {
            STACK_OF(X509) *xChain = X509_STORE_CTX_get_chain(ctx);
            if (xChain) {
                X509 *xCert = sk_X509_value(xChain, 0);
                if (xCert) {
                    X509Cert cert(xCert);

                    message.m_tags["certsubjectname"] = cert.getSubjectCommonName();
                    message.m_tags["certissuername"] = cert.getIssuerCommonName();
                    message.m_tags["certsernum"] = cert.getSerialNumberHex();
                    message.m_tags["notbefore"] = cert.getNotBeforePrettyString();
                    message.m_tags["notafter"] = cert.getNotAfterPrettyString();

                    char buf[32];
                    snprintf(buf, sizeof(buf), "%d", X509_STORE_CTX_get_error(ctx));
                    message.m_tags["errreason"] = buf;
                }
            }
        }


        message.m_logLevel = Log::Error;
        message.m_type = "certstatuserror";
        message.send();
        NTRACE(TRACE_RESOURCEMANAGER, "[CERT STATUS] - [NETWORK DIAGNOSTIC] - SSL Error");
        if(!NetConfiguration::getSSLPeerVerification())
            status = 1;
    }

    SSL *ssl = static_cast<SSL*>(X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx()));

    if (const char *version  = SSL_get_version(ssl)) {
        NetworkMetrics metrics = resource->networkMetrics();
        metrics.setSSLVersion(version);
        resource->setNetworkMetrics(metrics);
    }

    return status;
}

shared_ptr<DiskCache> GibbonResourceManager::createDiskCache()
{
    shared_ptr<GibbonDiskCache> ret(new GibbonDiskCache);
    return ret;
}

bool GibbonResourceManager::validateHash(const ResourceRequest::SharedPtr &request, const Resource::SharedPtr &resource)
{
    DataBuffer::Encoding encoding;
    DataBuffer::Hash hashType;
    std::string hash;
    if (!::parseHashHeader(request, &hashType, &hash, &encoding))
        return false;

    if (hashType == DataBuffer::Hash_None)
        return true;

    assert(encoding != DataBuffer::Encoding_None);

    const DataBuffer actual = static_pointer_cast<GibbonResource>(resource)->hash(hashType, encoding);
    if (actual.isEmpty() || strcasecmp(hash.c_str(), actual.data<const char>())) {
        Log::error(TRACE_RESOURCEMANAGER, "Invalid hash for %s\nExpected\n%s\nGot:\n%s",
                   resource->url().c_str(), hash.c_str(), actual.constData<const char>());
        if (NrdConfiguration::allowInsecureNetworkRequests() || Debug::TestNetworkSecure) {
            Log::info(TRACE_RESOURCEMANAGER, "Allowing insecure response from %s", resource->url().c_str());
            return true;
        }
        return false;
    }
    return true;
}

Resource::Method GibbonResourceManager::methodFromString(const std::string &method)
{
    Resource::Method op = Resource::Method_Get;
    if (!method.empty()) {
        if (method == "POST") {
            op = Resource::Method_Post;
        } else if (method == "HEAD") {
            op = Resource::Method_Head;
        } else if (method == "DELETE") {
            op = Resource::Method_Delete;
        } else if (method != "GET") {
            Log::error(TRACE_UI_SCRIPT, "Invalid method [%s]", method.c_str());
        }
    }
    return op;
}

void GibbonResourceManager::setupRequest(const shared_ptr<ResourceRequest> &request, const Variant &data)
{
    Variant::StringMapIterator it = data.find("body");
    if (it != data.stringMapEnd()) {
        DataBuffer body;
        if (it->second.isDataBuffer()) {
            body = it->second.dataBuffer();
        } else if (!it->second.isNull()) {
            body = it->second.value<DataBuffer>();
        }
        if (!body.empty()) {
            request->setRequestData(body);
        }
    }

    request->setTimeout(Time::fromMS(data.mapValue<int>("timeout", 0,
                                                        GibbonConfiguration::defaultNetworkTimeout())));
    request->setConnectTimeout(Time::fromMS(data.mapValue<int>("connectTimeout", 0,
                                                               GibbonConfiguration::defaultNetworkConnectTimeout())));
    request->setStallTimeout(Time::fromMS(data.mapValue<int>("stallTimeout", 0,
                                                             GibbonConfiguration::defaultNetworkStallTimeout())));

    it = data.find("headers");
    if (it != data.stringMapEnd() && it->second.isStringMap()) {
        HttpHeaders httpHeaders;
        const Variant::StringMapIterator end = it->second.stringMapEnd();
        for (Variant::StringMapIterator i = it->second.stringMapBegin(); i != end; ++i)
            httpHeaders[i->first] = i->second.value<std::string>();
        request->setRequestHeaders(httpHeaders);
    }
}

bool GibbonResource::shouldSuspend() const
{
    if(requestFlags() & GibbonResourceManager::Surface) {
        if(shared_ptr<SurfaceCache> surfaceCache = GibbonApplication::instance()->getSurfaceCache())
            return surfaceCache->isDecodeSuspended();
    }
    return Resource::shouldSuspend();
}

Surface::SharedPtr GibbonResource::surfaceDecode(int group, uint8_t flags) const
{
    if(SurfaceDecoders::SharedPtr decoders = SurfaceDecoders::instance()) {
        if(SurfaceDecoder::SharedPtr decoder = decoders->decoder(const_cast<GibbonResource*>(this)->shared_from_this())) {
            shared_ptr<SurfaceCache> surfaceCache = GibbonApplication::instance()->getSurfaceCache();
            SurfaceDecoder::DecodeInfo decode;
            if(decoder->decode(&decode)) {
                decode.info.flags |= flags;
#if defined(GIBBON_GRAPHICSENGINE_NULL)
                return surfaceCache->requestSurface(decode.info.size.width, decode.info.size.height, decode.info.flags, true, group);
#elif defined(GIBBON_SURFACEDECODER_FORK)
                if(Debug::TestSurfaceLoadFork && !(decoder->flags() & SurfaceDecoder::Flag_NeedSurface)) {
                    mProcess = SurfaceDecodeProcess::fork();
                    Surface::SharedPtr surface = mProcess->decode(responseData(), decode.info, group);
                    mProcess.reset();
                    return surface;
                }
#endif
                Surface::SharedPtr surface = surfaceCache->requestSurface(decode.info.size.width, decode.info.size.height, decode.info.flags, true, group);
                if(surface && surface->isValid()) {
                    decode.data.lock(surface, Surface::Data::WriteLock);
                    if(decoder->decode(&decode))
                        return surface;
                }
            }
        }
    }
    return Surface::SharedPtr();
}

SurfaceDecoder::SurfaceInfo GibbonResource::surfaceInfo() const
{
    if(SurfaceDecoders::SharedPtr decoders = SurfaceDecoders::instance()) {
        if(SurfaceDecoder::SharedPtr decoder = decoders->decoder(const_cast<GibbonResource*>(this)->shared_from_this())) {
            SurfaceDecoder::DecodeInfo decode;
            if(decoder->decode(&decode))
                return decode.info;
        }
    }
    return SurfaceDecoder::SurfaceInfo();
}

void GibbonResource::setSuspended(bool suspend)
{
#if defined(GIBBON_SURFACEDECODER_FORK)
    if(!GibbonApplication::instance()->hasQuit() && mProcess)
        mProcess->setSuspended(suspend);
#else
    NRDP_UNUSED(suspend);
#endif
}

std::string GibbonResource::decodeDescription() const
{
    std::ostringstream result;
#if defined(GIBBON_SURFACEDECODER_FORK)
    if(mProcess)
        result << mProcess->describe();
#endif
    return result.str();
}
