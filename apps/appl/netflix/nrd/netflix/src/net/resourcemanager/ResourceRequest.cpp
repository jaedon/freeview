/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ResourceRequest.h"

#include "ResourceManager.h"
#include <nrdbase/StringTokenizer.h>

using namespace netflix;

NRDP_DEFINE_OBJECTCOUNT_RECORD(ResourceRequestCount, ResourceRequest);

class ResourceRequestTimeoutTimer : public EventLoop::Timer
{
public:
    ResourceRequestTimeoutTimer(const Time &timeout, const ResourceRequest::SharedPtr &request)
        : EventLoop::Timer(timeout.val()), mRequest(request)
    {
        setSingleShot(true);
    }

    virtual std::string describe() const {
        if (ResourceRequest::SharedPtr req = mRequest.lock())
            return StringFormatter::sformat("ResourceRequestTimeoutTimer: %s", req->url().c_str());
        return "ResourceRequestTimeoutTimer";
    }

    virtual void timerFired()
    {
        if (ResourceRequest::SharedPtr req = mRequest.lock()) {
            if (req->abort()) {
                ResourceManager::SharedPtr manager = req->resourceManager();
                assert(manager);
                Resource::SharedPtr resource = manager->createResource(req);
                manager->finish(resource, AS_HTTP_TRANSACTION_TIMEOUT, 500);
            }
        }
    }
private:
    weak_ptr<ResourceRequest> mRequest;
};

ResourceRequest::ResourceRequest(const AseUrl &url, Resource::Method method)
    : mUrl(url), mBaseUrl(url), mMethod(method), mFlags(0), mTimeout(0)
{
    NRDP_OBJECTCOUNT_REF(ResourceRequest, this);
}

ResourceRequest::ResourceRequest(unsigned int flags, const AseUrl &url, Resource::Method method)
    : mUrl(url), mBaseUrl(url), mMethod(method), mFlags(flags), mTimeout(0)
{
    NRDP_OBJECTCOUNT_REF(ResourceRequest, this);
}

bool ResourceRequest::abort()
{
    NTRACE(TRACE_RESOURCEMANAGER, "ResourceRequest abort %s", url().c_str());
    ScopedMutex lock(ResourceManager::sMutex);
    if(EventLoop::Timer::SharedPtr timer = mTimeoutTimer.lock()) {
        timer->stop();
        mTimeoutTimer.reset();
    }
    const Resource::SharedPtr resource = mResource.lock();
    return resource && resource->removeRequest(shared_from_this());
}

void ResourceRequest::wait()
{
    NTRACE(TRACE_RESOURCEMANAGER, "ResourceRequest wait %s", url().c_str());
    if (const Resource::SharedPtr resource = mResource.lock())
        resource->wait();
}

shared_ptr<ResourceManager> ResourceRequest::resourceManager() const
{
    return mResourceManager.lock();
}

std::string ResourceRequestCount::describe(void *object) const
{
    return static_cast<ResourceRequest*>(object)->url().str();
}

void ResourceRequest::startTimeoutTimer()
{
    ScopedMutex lock(ResourceManager::sMutex);
    if(!mTimeoutTimer.lock() && mTimeout.val()) {
        shared_ptr<ResourceRequestTimeoutTimer> timer(new ResourceRequestTimeoutTimer(mTimeout, shared_from_this()));
        mTimeoutTimer = timer;
        timer->start();
    }
}
