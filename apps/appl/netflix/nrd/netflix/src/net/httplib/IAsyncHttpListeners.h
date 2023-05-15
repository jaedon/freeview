/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IASYNC_HTTP_LISTENERS_H
#define IASYNC_HTTP_LISTENERS_H

#include <vector>
#include <nrdbase/tr1.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdnet/TraceRecords.h>
#include <nrdnet/NetCommonDataTypes.h>

namespace netflix {

/** The HTTP request trace listener. */
class IHttpRequestTraceListener
{
public:
    typedef shared_ptr<IHttpRequestTraceListener> IHttpRequestTraceListenerPtr;

    virtual ~IHttpRequestTraceListener()
    {
    }

    /** Report bytes received for a request
     *
     *  @param [in] time The time at which the bytes were received
     *  @param [in] bytes The number of bytes received
     *  @param [in] header True if these were header bytes
     *  @param [in] complete True if these were the last bytes of the body
     */
    virtual void reportBytesReceived(
                                AseTimeVal const& time,
                                uint32_t bytes,
                                bool header,
                                bool complete ) = 0;
};

typedef IHttpRequestTraceListener::IHttpRequestTraceListenerPtr IHttpRequestTraceListenerPtr;

/** The HTTP connection trace listener. */
class IHttpConnectionTraceListener
{
public:
    virtual ~IHttpConnectionTraceListener()
    {
    }

    /** Report a successful TCP connect. */
    virtual void reportTcpConnect(
                            uint32_t connectionId,
                            uint32_t httpId,
                            const std::string& clientIpAddress,
                            uint16_t clientPort,
                            const std::string& serverHostName,
                            const std::string& serverIpAddress,
                            uint16_t serverPort,
                            const AseTimeVal& dnsLookupTime,
                            const AseTimeVal& connectTime ) = 0;

    /** Report a failed TCP connect. */
    virtual void reportTcpFailure(
                            uint32_t connectionId,
                            uint32_t httpId,
                            int32_t statusCode,
                            int32_t lowLevelCode,
                            const std::string& clientIpAddress,
                            uint16_t clientPort,
                            const std::string& serverHostName,
                            const std::string& serverIpAddress,
                            uint16_t serverPort,
                            const AseTimeVal& dnsLookupTime,
                            const AseTimeVal& failedTime ) = 0;

    /** Report a TCP disconnect. */
    virtual void reportTcpDisconnect(
                            uint32_t connectionId,
                            uint32_t httpId,
                            int32_t statusCode,
                            int32_t lowLevelCode,
                            TcpDisconnectState disconnectState,
                            bool midResponse ) = 0;

    /** Report byte received. */
    virtual void reportBytesReceived(
                            uint32_t connectionId,
                            const AseTimeVal& receivedTime,
                            uint32_t arrivedByteCount,
                            uint32_t receivedByteCount,
                            bool drainingSocketReceiveBuffer,
                            AseTimeVal const& eaeliestTime ) = 0;

    /** Report an HTTP connect failure.
     *
     *  This is to report a HTTP level conenect failure, which could be the result of:
     *
     *  * All DNS lookups failed.
     *  * All TCP connects failed.
     */
    virtual void reportHttpConnectFailure(
                            int32_t failureCode,
                            const std::string& serverHostName ) = 0;

    /** Report a  TCP connect start. */
    virtual void reportTcpStart(
                            uint32_t connectionId,
                            uint32_t httpId,
                            const std::string& serverHostName,
                            const std::string& serverIpAddress,
                            uint16_t serverPort,
                            bool primaryFlag ) = 0;
};

/** The DNS listener. */
class IDnsTraceListener
{
public:
    virtual ~IDnsTraceListener()
    {
    }

    /** Report a successful DNS resolution.
     *
     *  @param dnsQueryType [In] The DNS query type.
     *  @param nameServers [In] The IPs of the name serves used for resolvig host names.
     *  @param lookupTime [In] The duration that the lookup takes, note this is not RTT to name servers.
     *  @param cnames [In] The returned CNAME list. The first entry in the list is the original host name.
     *                     The last entry in the list is the official name.  For example, lookup on
     *                     www.yahoo.com would have the following cname chain:
     *
     *                       www.yahoo.com               canonical name = fp3.wg1.b.yahoo.com.
     *                       fp3.wg1.b.yahoo.com         canonical name = any-fp3-lfb.wa1.b.yahoo.com.
     *                       any-fp3-lfb.wa1.b.yahoo.com canonical name = any-fp3-real.wa1.b.yahoo.com.
     *
     *                     The cnames would then contain the following entries:
     *
     *                          www.yahoo.com
     *                          fp3.wg1.b.yahoo.com
     *                          any-fp3-lfb.wa1.b.yahoo.com
     *                          any-fp3-real.wa1.b.yahoo.com.
     *
     *  @param ipAddressTtlRecords [In] The returned IP address/TTL list. The first entry is the one used for establishing the TCP connection.
     */
    virtual void reportHostNameResolved(
                DnsQueryType dnsQueryType,
                const std::vector<std::string>& nameServers,
                const AseTimeVal& lookupTime,
                const std::vector<std::string>& cnames,
                const std::vector<IpAddressTtlRecord>& ipAddressTtlRecords ) = 0;

    /** Report a failed DNS resolution.
     *
     *  @param dnsQueryType [In] The DNS query type.
     *  @param nameServers [In] The IPs of the name serves used for resolvig host names.
     *  @param lookupTime [In] The duration that the lookup takes, note this is not RTT to name servers.
     *  @param hostName [In] The host name.
     *  @param failureCode [In] The failure code, as ASE error codes.
     */
    virtual void reportDnsLookupFailure(
                DnsQueryType dnsQueryType,
                const std::vector<std::string>& nameServers,
                const AseTimeVal& lookupTime,
                const std::string& hostName,
                int32_t failureCode ) = 0;
};

/** The route listener. */
class IRouteListener
{
public:
    virtual ~IRouteListener()
    {
    }

    /** Report the route packet traces.
     *
     *  @param destinationIpAddress [Out] The destination IP address.
     *  @param routePacketTraces [In] The route packet traces.
     */
    virtual void reportRoute(
                    const std::string& destinationIpAddress,
                    const std::vector<PacketTrace>& routePacketTraces ) = 0;
};

}

#endif //IASYNC_HTTP_LISTENERS_H
