/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MdxHttpClient.h"
#include "MdxLog.h"
#include "MdxTracing.h"
#include <nrdnet/CurlMultiThread.h>

using namespace netflix;
using namespace netflix::mdx;

MdxHttpClient::MdxHttpClient(const std::string &url,
                             const std::string &headers,
                             const std::string &postdata,
                             const int curltimeout,
                             ullong xid,
                             bool getMethod,
                             const std::string &userAgent)
    : mUrlHost(url)
    , mHeaders(headers)
    , mPostData(postdata)
    , mUserAgent(userAgent)
    , mCurlTimeout(curltimeout)
    , mGetMethod(getMethod)
    , mAbort(false)
    , mDataReceived(false)
    , mCurl(NULL)
    , mLastResult(CURLE_OK)
    , mXid(xid)
    , mCallback(NULL)
    , mUserData(NULL)
    , mHttpPostRequestHeaders(NULL)
{
}

MdxHttpClient::~MdxHttpClient()
{
    cleanupCurl();
}


size_t header_cb(void* ptr, size_t size, size_t nmemb,
                                void* userp)
{
    TIMER(__FUNCTION__, "HTTP");

    MdxHttpClient* impl = static_cast<MdxHttpClient*>(userp);

    return impl->appendHeader(ptr, size, nmemb);
}

static size_t receiveData(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    TIMER(__FUNCTION__, "HTTP");

    if ((size * nmemb) != 0) {
        MdxHttpClient *dispatch = (MdxHttpClient *)userdata;
        if (!dispatch->append((char*)ptr, size*nmemb))
            return 0;
    }
    return (size * nmemb);
}

bool MdxHttpClient::execute()
{
    TIMER(__FUNCTION__, "HTTP");

    mBuffer = "";
    mCurl = setupCurl();

    if(!mCurl)
    {
        return false;
    }

    mLastResult = curl_easy_perform(mCurl);
    long longCode;
    curl_easy_getinfo(mCurl, CURLINFO_RESPONSE_CODE, &longCode);
    mResponseCode = (int32_t)longCode;

    cleanupCurl();

    return (mLastResult == CURLE_OK);
}

void MdxHttpClient::executeAsync(CurlMultiThread *thread, void (*callback)(MdxHttpClient *pClient, bool result, void *pUserData), void *pUserData)
{
    TIMER(__FUNCTION__, "HTTP");

    mBuffer = "";

    mCurl = setupCurl();

    if(!mCurl)
    {
        callback(this, false, pUserData);
        return;
    }

    mCallback = callback;
    mUserData = pUserData;

    if(!thread->add(mCurl, MdxHttpClient::asyncFinishedCallback, this))
    {
        // failure
        cleanupCurl();
        callback(this, false, pUserData);
        return;
    }
}

std::string MdxHttpClient::getUrl() const
{
    return mUrlHost;
}

bool MdxHttpClient::getMethod() const
{
    return mGetMethod;
}

std::string MdxHttpClient::getResponse() const
{
    return mBuffer;
}

std::string MdxHttpClient::getResponseHeaders() const
{
    return mResponseHeaders;
}

int32_t MdxHttpClient::getResponseCode() const
{
    return mResponseCode;
}

std::string MdxHttpClient::getLastResultString() const
{
    return curl_easy_strerror(mLastResult);
}

ullong MdxHttpClient::getXid() const
{
    return mXid;
}

bool MdxHttpClient::append(char *ptr, size_t size)
{
    mBuffer.append(ptr, size);

    return true;
}

size_t MdxHttpClient::appendHeader(void* ptr, size_t size, size_t nmemb)
{
    // Return -1 to stop libcurl if aborting.
    if (mAbort)
        return static_cast<size_t>(-1);

    // Convert the buffer to a string so we can parse out the header
    // name and value.
    size_t bytes = size * nmemb;
    std::string header(static_cast<char*>(ptr), bytes);

    // If we've already received the body, this is a trailer. Drop it.
    if (mDataReceived)
        return bytes;

    // If no colon, this is not a valid header. Drop it.
    size_t colon = header.find(':');
    if (colon == std::string::npos)
        return bytes;

    mResponseHeaders += header;

    return bytes;
}

CURL *MdxHttpClient::setupCurl()
{
    TIMER(__FUNCTION__, "HTTP");

    CURL *curl = NULL;

#ifndef NRDP_NO_CURL_GLOBAL_INIT
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        fprintf(stderr, "curl_global_init() failed\n");
        return 0;
    }
#endif

    if ((curl = curl_easy_init()) == NULL) {
        fprintf(stderr, "curl_easy_init() failed\n");
#ifndef NRDP_NO_CURL_GLOBAL_INIT
        curl_global_cleanup();
#endif
        return 0;
    }

    curl_easy_setopt(curl, CURLOPT_URL, mUrlHost.c_str());
    if (mGetMethod) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    } else {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, mPostData.length());
        if (mPostData.length()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, mPostData.c_str());
        }
    }

    curl_easy_setopt(curl, CURLOPT_NOPROXY, "*");  // mdx does not function over proxies
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_cb);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receiveData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, mCurlTimeout);
    if(mUserAgent != "") {
        curl_easy_setopt(curl, CURLOPT_USERAGENT, mUserAgent.c_str());
    }

    mHttpPostRequestHeaders = curl_slist_append(mHttpPostRequestHeaders, "Content-Type: text/plain; charset=utf-8");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, mHttpPostRequestHeaders);

    return curl;
}

void MdxHttpClient::cleanupCurl()
{
    TIMER(__FUNCTION__, "HTTP");

    if(mCurl)
    {
        curl_easy_cleanup(mCurl);
#ifndef NRDP_NO_CURL_GLOBAL_INIT
        curl_global_cleanup();
#endif
        mCurl = NULL;
    }
}

void MdxHttpClient::asyncFinishedCallback(long httpStatusCode, CURLcode code, void *pUserData)
{
    MdxHttpClient *pClient = (MdxHttpClient *)pUserData;

    pClient->mResponseCode = (int32_t)httpStatusCode;

    pClient->cleanupCurl();

    pClient->mLastResult = code;

    if(pClient->mCallback)
    {
        pClient->mCallback(pClient, (code == CURLE_OK), pClient->mUserData);
    }
}
