/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef TRACE_RECORDS_H
#define TRACE_RECORDS_H

#include <nrdbase/StdInt.h>
#include <string>

namespace netflix {

/**
 * The packet trace.
 */
class PacketTrace
{
public:
    uint32_t mHopNumber;
    std::string mLocalIp;
    std::string mNodeIp;
    uint32_t mRtt;
};

class IpAddressTtlRecord
{
public:
    std::string mIpAddress; //IP address.
    uint32_t mTtl;          //TTL, in seconds.
};

}

#endif //TRACE_RECORDS_H
