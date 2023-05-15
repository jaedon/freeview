/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "CurlUtils.h"
#include <nrd/AppLog.h>

using namespace netflix::device;

#define REQUEST_TIMEOUT (5)  // in seconds

CurlUtils::CurlUtils() :
    mHandle(NULL),
    mResponseHeaders(""),
    mBody(""),
    mRequestHeaders(NULL),
    mResponseCode(0),
    CURL_TIMEOUT(REQUEST_TIMEOUT)
{

}

bool CurlUtils::init()
{
    if ((mHandle = curl_easy_init()) == NULL) return false;
    return true;
}

std::vector<std::string> CurlUtils::constructHeaderList( std::string list)
{
    std::vector<std::string> elems;
    std::stringstream ss(list);
    std::string item;
    while (std::getline(ss, item, ',')) {
        elems.push_back(item);
    }
    return elems;
}

CurlUtils::~CurlUtils()
{
    if(mHandle)
    {
        curl_easy_cleanup(mHandle);
        mHandle=NULL;
    }
}

size_t CurlUtils::appendHeader(void* ptr, size_t size, size_t nmemb)
{
    // Convert the buffer to a string so we can parse out the header
    // name and value.
    size_t bytes = size * nmemb;
    if(bytes)
    {
        std::string header(static_cast<char*>(ptr), bytes);

        // If no colon, this is not a valid header. Drop it.
        size_t colon = header.find(':');
        if (colon == std::string::npos)
            return bytes;

        mResponseHeaders += header;
    }

    return bytes;
}
static size_t header_cb(
        void* ptr, size_t size, size_t nmemb, void* userp )
{
    CurlUtils* impl = static_cast<CurlUtils*>(userp);
    return impl->appendHeader(ptr, size, nmemb);
}

static size_t receiveData(
        void *ptr, size_t size, size_t nmemb, void *userdata)
{
    if((size*nmemb)!=0)
    {
        CurlUtils* impl = static_cast<CurlUtils*>(userdata);
        impl->append((char*)ptr,size*nmemb);
    }
    return (size * nmemb);
}

void CurlUtils::append(char* ptr, size_t size)
{
    mBody.append(ptr,size);
    NTRACE(TRACE_LOG, "response::%s, size:%zd", mBody.c_str(), size);
}

void CurlUtils::setupCurl(std::string url, std::vector<std::string> headerList, bool isPost)
{
    NTRACE(TRACE_LOG, "URL::%s", url.c_str());
    curl_easy_setopt(mHandle, CURLOPT_URL, url.c_str());
    if(isPost)
    {
        curl_easy_setopt(mHandle, CURLOPT_POST, 1L);
        curl_easy_setopt(mHandle, CURLOPT_POSTFIELDSIZE, mPostData.length());
        if (mPostData.length()) {
            curl_easy_setopt(mHandle, CURLOPT_POSTFIELDS, mPostData.c_str());
        }
    }
    else
    {
        curl_easy_setopt(mHandle, CURLOPT_HTTPGET, 1L);
    }

    curl_easy_setopt(mHandle, CURLOPT_HEADERFUNCTION, &header_cb);
    curl_easy_setopt(mHandle, CURLOPT_HEADERDATA, this);
    curl_easy_setopt(mHandle, CURLOPT_WRITEFUNCTION, &receiveData);
    curl_easy_setopt(mHandle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(mHandle, CURLOPT_TIMEOUT, CURL_TIMEOUT);

    // add custom headers
    for(std::vector<std::string>::iterator it = headerList.begin(); it < headerList.end(); ++it)
    {
        NTRACE(TRACE_LOG, "Adding header %s", (*it).c_str());
        mRequestHeaders = curl_slist_append(mRequestHeaders, (*it).c_str());
    }
    curl_easy_setopt(mHandle, CURLOPT_HTTPHEADER, mRequestHeaders);
}

bool CurlUtils::makeGetRequest( std::string url, std::vector<std::string> headerList )
{
    setupCurl(url,headerList,false);

    CURLcode result;
    result = curl_easy_perform(mHandle);
    if(result != CURLE_OK) {
        Log::error(TRACE_LOG, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        return false;
    }
    curl_easy_getinfo(mHandle, CURLINFO_RESPONSE_CODE, &mResponseCode);
    NTRACE( TRACE_LOG, "%s: response code: %ld", __FUNCTION__, mResponseCode );

    if(mResponseCode == 200 || mResponseCode == 201) return true;
    return false;
}

bool CurlUtils::makePostRequest( std::string url, std::vector<std::string> headerList, std::string postData )
{
    mPostData = postData;
    setupCurl(url,headerList,true);

    CURLcode result;
    result = curl_easy_perform(mHandle);
    if(result != CURLE_OK) {
        Log::error(TRACE_LOG, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        return false;
    }
    curl_easy_getinfo(mHandle, CURLINFO_RESPONSE_CODE, &mResponseCode);
    NTRACE( TRACE_LOG, "%s: response code: %ld", __FUNCTION__, mResponseCode );

    if(mResponseCode == 200 || mResponseCode == 201) return true;
    return false;
}

