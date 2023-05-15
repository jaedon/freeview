/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_DNS_CACHE_H
#define I_DNS_CACHE_H

#include <list>

#include <nrdnet/NrdIpAddr.h>
#include <nrdnet/NetCommonDataTypes.h>

namespace netflix {


class IDnsCache
{
public:
    /** Constructor. */
    IDnsCache()
    {
    }

    /** Destructor. */
    virtual ~IDnsCache()
    {
    }

    /** Set the nstance. */
    static void setInstance( IDnsCache* pDnsCache );

    /** Destroy the instance. */
    static void destroy();

    /** Get the instance. */
    static IDnsCache* instance();

    /** Add a host's IP addresses to the DNS cache.
     *
     *  @param hostName [In] The host name.
     *  @param ipVersion [In] The IP version, v4 or v6.
     *  @param ipAddrList [In] The list of IP addresses.
     *  @param ttl [In] The TTL of IP addresses, in seconds.
     */
    virtual void addIpAddress(
                    const std::string& hostName,
                    IpVersion ipVersion,
                    const std::list<NrdIpAddr>& ipAddrList,
                    uint32_t ttl ) = 0;

    /** Lookup IP addresses of a host.
     *
     *  @param hostName [In] The host name.
     *  @param ipVersion [In] The IP version, v4 or v6.
     *  @param ipAddrList [Out] The returned IP addresses, the list
     *                          is empty if the host name is not
     *                          found or the IP addresses have
     *                          exceeded the TTL.
     */
    virtual void lookupIpAddress(
                    const std::string& hostName,
                    IpVersion ipVersion,
                    std::list<NrdIpAddr>& ipAddrList ) = 0;

protected:
    static IDnsCache* mInstance;
};

}

#endif //I_DNS_CACHE_H
