/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SocketizerIPAddress.h"
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>

using namespace netflix::socketizer;

IPAddress::IPAddress()
{
    memset(&mIp, '\0', sizeof(sockaddr_storage));
}

IPAddress::IPAddress(const sockaddr* sock)
{
    memset(&mIp, '\0', sizeof(sockaddr_storage));
    switch (sock->sa_family) {
    case AF_INET:
        memcpy(ipv4(), sock, sizeof(sockaddr_in));
        break;
    case AF_INET6:
        memcpy(ipv6(), sock, sizeof(sockaddr_in6));
        break;
    }
}

std::string IPAddress::str() const
{
    if (mIp.ss_family == AF_UNSPEC)
        return std::string();
    const bool isIPv6 = (mIp.ss_family == AF_INET6);
    assert(isIPv6 || mIp.ss_family == AF_INET);
    std::string str(INET6_ADDRSTRLEN, '\0');
    const void* addr;
    if (isIPv6)
        addr = &ipv6()->sin6_addr;
    else
        addr = &ipv4()->sin_addr;
    inet_ntop(mIp.ss_family, addr, &str[0], INET6_ADDRSTRLEN);
    str.resize(strlen(str.c_str()));
    return str;
}

void IPAddress::parse(const std::string& ip)
{
    const bool isIPv6 = (ip.find(':') != std::string::npos);
    memset(&mIp, '\0', sizeof(sockaddr_storage));
    mIp.ss_family = isIPv6 ? AF_INET6 : AF_INET;
    void* addr;
    if (isIPv6)
        addr = &ipv6()->sin6_addr;
    else
        addr = &ipv4()->sin_addr;
    if (inet_pton(mIp.ss_family, ip.c_str(), addr) != 1)
        mIp.ss_family = AF_UNSPEC;
}

IPAddress::Type IPAddress::type() const
{
    switch (mIp.ss_family) {
    case AF_INET6:
        return IPv6;
    case AF_INET:
        return IPv4;
    }
    assert(mIp.ss_family == AF_UNSPEC);
    return Invalid;
}

bool IPAddress::operator<(const IPAddress& other) const
{
    if (mIp.ss_family != other.mIp.ss_family) {
        // Unspec is before everything else
        if (mIp.ss_family == AF_UNSPEC)
            return true;
        if (other.mIp.ss_family == AF_UNSPEC)
            return false;
        // IPv4 before IPv6
        if (mIp.ss_family == AF_INET)
            return true;
        if (other.mIp.ss_family == AF_INET)
            return false;
        // at this point both would be IPv6, can't happen
        assert(false && "Both IPv6?");
        return true;
    }
    // if both are unspec, we're equal
    if (mIp.ss_family == AF_UNSPEC)
        return false;
    // IPv4 is simply an int compare
    if (mIp.ss_family == AF_INET) {
        return ipv4()->sin_addr.s_addr < other.ipv4()->sin_addr.s_addr;
    }
    assert(mIp.ss_family == AF_INET6);
    // Check IPv6
    return memcmp(&ipv6()->sin6_addr.s6_addr, &other.ipv6()->sin6_addr.s6_addr, sizeof(ipv6()->sin6_addr.s6_addr)) < 0;
}
