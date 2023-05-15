/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef HTTP_SERVICE_METRICS_H
#define HTTP_SERVICE_METRICS_H

#include "NrdIpAddr.h"

namespace netflix
{

/**
 * The HTTP service metrics class
 */
class HttpServiceMetrics
{
public:
    //The DNS lookup time, in ms.
    uint32_t mDnsLookupTime;

    //The HTTP connect time, in ms, this may include the time for multiple TCP connects.
    uint32_t mHttpConnectTime;

    //The HTTP transaction time, in ms.
    uint32_t mHttpTransactionTime;

    NrdIpAddr mServerIpAddress;

    std::string mCName;
public:
    //Constructor.
    HttpServiceMetrics() : mDnsLookupTime(0),
        mHttpConnectTime(0),
        mHttpTransactionTime(0)
    {
    }
};


}

#endif // HTTP_SERVICE_METRICS_H
