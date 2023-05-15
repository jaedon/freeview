/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MDXGUARD_H
#define MDXGUARD_H

#include <curl/curl.h>
#include <assert.h>

#include <string>
#include <nrdbase/Variant.h>

namespace netflix {
    class CurlMultiThread;

namespace mdx {

class MdxHttpClient;

class MdxGuard
{
public:
    static void shutdown();

    static bool sendMdxHttpRequest(const std::string &targetUrl,
                                   const std::string &header,
                                   const std::string &nvpairs,
                                   llong xid,
                                   int curltimeout,
                                   int method,
                                   const std::string &userAgent);

    static void sendMdxHttpRequestAsync(CurlMultiThread *thread,
                                        const std::string &targetUrl,
                                        const std::string &header,
                                        const std::string &nvpairs,
                                        llong xid,
                                        int curltimeout,
                                        int method,
                                        const std::string &userAgent);

    static void processIncomingRequest(std::string targetUrl, Variant headers, std::string nvpairs, void * connData);

private:
    static void onHttpRequestComplete(MdxHttpClient *pClient, bool success, void *pUserData);
    static MdxGuard* pInstance;
};

}}  // namespace netflix::mdx
#endif

