/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SocketizerIPAddress_h
#define SocketizerIPAddress_h

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

namespace netflix {
namespace socketizer {

class IPAddress
{
public:
    IPAddress();
    IPAddress(const sockaddr* sock);
    IPAddress(const std::string& ip) { parse(ip); }
    ~IPAddress() { }

    void parse(const std::string& ip);

    enum Type { Invalid, IPv6, IPv4 };
    Type type() const;

    const sockaddr_in6* ipv6() const { return reinterpret_cast<const sockaddr_in6*>(&mIp); }
    sockaddr_in6* ipv6() { return reinterpret_cast<sockaddr_in6*>(&mIp); }
    const sockaddr_in* ipv4() const  { return reinterpret_cast<const sockaddr_in*>(&mIp); };
    sockaddr_in* ipv4()  { return reinterpret_cast<sockaddr_in*>(&mIp); };

    bool operator<(const IPAddress& other) const;

    std::string str() const;

private:
    sockaddr_storage mIp;
};
}
}

#endif
