/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef NRD_IP_ADDR_H
#define NRD_IP_ADDR_H

#include <nrdbase/StdInt.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <string>

#include <nrdbase/config.h>

namespace netflix {

/**
 * A generic IP address class.
 */
class NrdIpAddr
{
public:
    typedef enum
    {
        IP_V4 = 0,
        IP_V6
    } Version;

public:
    /**
     * Constructor.
     */
    NrdIpAddr();

    /**
     * Constructor
     */
    NrdIpAddr( const NrdIpAddr& nrdIpAddr );
    NrdIpAddr( const std::string& strIpAddr );
    NrdIpAddr( struct in_addr inAddr );

#ifdef NRDP_HAS_IPV6
    NrdIpAddr( struct in6_addr in6Addr, const std::string& in6ScopeId = "" );
#endif

    /**
     * Destructor.
     */
    ~NrdIpAddr();

    /** Assign operators. */
    const NrdIpAddr& operator=( const NrdIpAddr& nrdIpAddr );

    /** Check if the IP address is valid. */
    bool valid() const;

    /** Get the IP version. */
    Version getVersion() const;

    /** Setter/Getter functions. */
    void setStrAddr( const std::string& strIpAddr );
    std::string getStrAddr() const;

    void setInAddr( struct in_addr inAddr );
    struct in_addr getInAddr() const;

#ifdef NRDP_HAS_IPV6
    void setIn6Addr( struct in6_addr in6Addr );
    struct in6_addr getIn6Addr() const;

    void setIn6ScopeId( const std::string& in6ScopeId );
    const std::string& getIn6ScopeId() const;
#endif

    /** Comparison operators. */
    bool operator==( const NrdIpAddr& rhs ) const;
    bool operator!=( const NrdIpAddr& rhs ) const;
    bool operator<( const NrdIpAddr& rhs ) const;

private:
    //The flag indicates if the IP address is valid.
    bool mValidFlag;

    //The IP address version.
    Version mVersion;

    //The IPv4 address value.
    struct in_addr mInAddr;

#ifdef NRDP_HAS_IPV6
    //The IPv6 address value.
    struct in6_addr mIn6Addr;

    std::string mIn6ScopeId;
#endif

};

}

#endif // NRD_IP_ADDR_H
