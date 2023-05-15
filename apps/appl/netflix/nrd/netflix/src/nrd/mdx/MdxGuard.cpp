/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxGuard.h"
#include "MdxInternal.h"
#include "MdxHttpClient.h"
#include "MdxMongooseNFLX.h"
#include "MdxTracing.h"

using namespace netflix;
using namespace netflix::mdx;

typedef std::map<llong, MdxHttpClient *> RequestMap;

static RequestMap sRequests;
static void trackRequest(MdxHttpClient *dispatch, llong xid);
static void untrackRequest(llong xid);
static void clearTrackedRequests();

void MdxGuard::shutdown()
{
    clearTrackedRequests();
}

bool MdxGuard::sendMdxHttpRequest(const std::string &targetUrl,
                                  const std::string &header,
                                  const std::string &nvpairs,
                                  llong xid,
                                  int curltimeout,
                                  int method,
                                  const std::string &userAgent)
{
    MdxHttpClient *dispatch = new MdxHttpClient(targetUrl, header, nvpairs, curltimeout, xid, method, userAgent);
    bool success;

    // no need to track synchronous requests
    success = dispatch->execute();

    onHttpRequestComplete(dispatch, success, NULL); // onHttpRequestComplete deletes the dispatch

    return success;
}

void MdxGuard::sendMdxHttpRequestAsync(CurlMultiThread *thread,
                                       const std::string &targetUrl,
                                       const std::string &header,
                                       const std::string &nvpairs,
                                       llong xid,
                                       int curltimeout,
                                       int method,
                                       const std::string &userAgent)
{
    MdxHttpClient *dispatch = new MdxHttpClient(targetUrl, header, nvpairs, curltimeout, xid, method, userAgent);

    // we need to track asynchronous requests handled by CurlMultiThread because
    // onHttpRequestComplete will not be called if CurlMultiThread is shutdown
    // mid-request
    trackRequest(dispatch, xid);

    dispatch->executeAsync(thread, onHttpRequestComplete, NULL); // onHttpRequestComplete deletes the dispatch
}

void MdxGuard::processIncomingRequest(std::string path, Variant headers, std::string nvpairs, void * conndata)
{
    string response;

    response = "status=ok";

    MdxMongooseNFLX::instance()->sendMongooseResponse(response, (void *)conndata);
    MdxInternal::getInstance()->callListener(IncomingMessageEvent(path, headers, nvpairs));
}

void MdxGuard::onHttpRequestComplete(MdxHttpClient *pClient, bool success, void *pUserData)
{
    (void)pUserData;
    ullong xid  = pClient->getXid();
    long responseCode;
    std::string response;
    std::string headers;

    EVENT_STEP("Message", xid, "MDX", "HttpRequestComplete", NULL);

    if (success) {
        responseCode = pClient->getResponseCode();
        response     = pClient->getResponse();
        headers      = pClient->getResponseHeaders();
    } else {
        responseCode = 0;
        response     = std::string("error ") + pClient->getLastResultString();
        headers      = "";
    }

    MdxInternal::getInstance()->callListener(HttpResponseEvent(pClient->getUrl(),
                                                               pClient->getMethod() ? 1 : 0,
                                                               xid,
                                                               responseCode,
                                                               headers,
                                                               response));

    untrackRequest(xid);
    delete pClient;
}

void trackRequest(MdxHttpClient *dispatch, llong xid)
{
    RequestMap::iterator iter = sRequests.find(xid);
    if(iter != sRequests.end()) {
        MDXWARN("Duplicated HTTP request xid? Will not track for cleanup");
    } else {
        sRequests[xid] = dispatch;
    }
}

void untrackRequest(llong xid)
{
    RequestMap::iterator iter = sRequests.find(xid);
    if(iter != sRequests.end()) {
        sRequests.erase(iter);
    }
}

void clearTrackedRequests()
{
    // Go through tracked requests. If any are left in the list, their callback
    // is never going to happen. They should be deleted to free resources.
    if(!sRequests.empty())
    {
        MDXINFO("%d outstanding HTTP requests at shutdown", sRequests.size());
        RequestMap::iterator iter = sRequests.begin();
        while(iter != sRequests.end())
        {
            MDXINFO("Freeing: %lld", iter->first);
            delete iter->second;
            iter++;
        }
        sRequests.clear();
    }
    else
    {
        MDXTRACE("No outstanding HTTP requests");
    }
}
