/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef CURL_UTILS_H
#define CURL_UTILS_H

#include <string>
#include <vector>
#include <curl/curl.h>

namespace netflix {
namespace device {

class CurlUtils
{
public:
    CurlUtils();
    virtual ~CurlUtils();

    bool init();

    // returns true if the request was successful
    bool makeGetRequest( std::string url, std::vector<std::string> headerList );
    bool makePostRequest( std::string url, std::vector<std::string> headerList, std::string postData );
    std::vector<std::string> constructHeaderList(std::string list);
    std::string getHeaders()    {   return mResponseHeaders; }
    std::string getBody()       {   return mBody; }
    size_t appendHeader(void* ptr, size_t size, size_t nmemb);
    void append(char* ptr, size_t size);

private:
    CURL*               mHandle;
    std::string         mResponseHeaders;
    std::string         mBody;
    curl_slist *        mRequestHeaders;
    long                mResponseCode;
    const uint32_t      CURL_TIMEOUT;
    std::string         mPostData;

    void setupCurl(std::string url, std::vector<std::string> headerList, bool isPost);
};
}
}

#endif // CURL_UTILS_H
