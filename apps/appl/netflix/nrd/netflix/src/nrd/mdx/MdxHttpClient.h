/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MDXHTTPCLIENT_H
#define MDXHTTPCLIENT_H

#include <curl/curl.h>
#include <assert.h>

#include <string>

#include <nrdbase/StdInt.h>

namespace netflix {
    class CurlMultiThread;

namespace mdx {
class MdxHttpClient
{
public:
    typedef void (*asyncCallback)(MdxHttpClient *pClient, bool result, void *pUserData);

    MdxHttpClient(const std::string &urlHost,
                  const std::string &headers,
                  const std::string &postdata,
                  const int curltimeout,
                  ullong xid,
                  bool getMethod = true,
                  const std::string &userAgent = "");
    virtual ~MdxHttpClient();

    bool execute();
    void executeAsync(CurlMultiThread *thread,
                      asyncCallback callback,
                      void *pUserData);

    std::string getUrl() const;
    bool        getMethod() const;
    std::string getResponse() const;
    std::string getResponseHeaders() const;
    int32_t     getResponseCode() const;
    std::string getLastResultString() const;
    ullong    getXid() const;

    // used in callbacks
    bool        append(char *ptr, size_t size);
    size_t      appendHeader(void* ptr, size_t size, size_t nmemb);

private:
    CURL *          setupCurl();
    void            cleanupCurl();
    static void     asyncFinishedCallback(long httpStatusCode, CURLcode code, void *pUserData);

    const std::string mUrlHost;
    const std::string mHeaders;
    const std::string mPostData;
    const std::string mUserAgent;
    std::string       mBuffer;
    int               mCurlTimeout;
    bool              mGetMethod;
    bool              mAbort;
    bool              mDataReceived;    //!< True if the response body has been seen.
    std::string       mResponseHeaders; //!< Received headers.
    int32_t           mResponseCode;
    CURL *            mCurl;
    CURLcode          mLastResult;
    ullong          mXid;
    asyncCallback     mCallback;
    void *            mUserData;
    curl_slist *      mHttpPostRequestHeaders;
};

}}  // namespace netflix::mdx
#endif

