/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ITRANSPORT_REPORTER_H
#define ITRANSPORT_REPORTER_H

#include <nrdnet/TraceRecords.h>
#include <nrdnet/NetCommonDataTypes.h>

#include <nrdbase/Variant.h>
#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/HttpAttempt.h>

namespace netflix {
namespace ase {

/**
 * @class ITransportReporter
 * @brief Transport metrics listener is notified of
 *        transport layer events for reporting purposes.
 *
 *  These metrics align with those in the DASH specification
 */
class ITransportReporter
{
public:
    typedef shared_ptr<ITransportReporter> ITransportReporterPtr;

public:
    virtual ~ITransportReporter() {}

    /** Report establishment of a TCP connection
     *  @param[in] dltrackid The download track id
     *  @param[in] id A local connection id to be used to refer to
     *                  the connection in later calls
     *  @param[in] The ID of the HTTP this TCP connection is for.
     *  @param[in] hostport The host and port to which the connection was attempted
     *  @param[in] domainKey The domain key associated with the host
     *  @param[in] src The source IP address
     *  @param[in] dest The desintation IP address
     *  @param[in] connectTimeMs The connection time in ms
     *  @param[in] dnsLookupTimeMs The DNS lookup time in ms
     */
    virtual void tcpConnect(    uint32_t dltrackid,
                                int id,
                                int httpId,
                                AseUrl::HostPort const& hostport,
                                std::string const& src,
                                std::string const& dest,
                                MediaType mediaType,
                                int connectTimeMs,
                                int dnsLookupTimeMs ) = 0;

    /** Report failure to establish a TCP connection
     *  @param[in] dltrackid The download track id
     *  @param[in] id A local connection id to be used to refer to
     *                  the connection in later calls
     *  @param[in] The ID of the HTTP this TCP connection is for.
     *  @param[in] hostport The host and port to which the connection was attempted
     *  @param[in] domainKey The domain key associated with the host
     *  @param[in] src The source IP address
     *  @param[in] dest The desintation IP address
     *
     *  @param[in] connectTimeMs The connection time in ms, if DNS was successful
     *  @param[in] dnsLookupTimeMs The DNS lookup time in ms, if DNS lookup was needed
     */
    virtual void tcpFailure(    uint32_t dltrackid,
                                int id,
                                int httpId,
                                AseUrl::HostPort const& hostport,
                                std::string const& src,
                                std::string const& dest,
                                MediaType mediaType,
                                int connectTimeMs,
                                int dnsLookupTimeMs,
                                AseErrorCode reason,
                                int lowLevelStatus ) = 0;

    /** Report closure of a TCP connection
     *  @param[in] dltrackid The download track id
     *  @param[in] id Connection id
     *  @param[in] The ID of the HTTP this TCP connection is for.
     *  @param[in] reason See AseErrorCodes.h
     *  @param[in] tcpDisconnectState
     *  @param[in] Whether it is in the middle of receiving a response at disconnect.
     */
    virtual void tcpDisconnect(
                        uint32_t dltrackid,
                        int id,
                        int httpId,
                        AseErrorCode reason,
                        TcpDisconnectState tcpDisconnectState,
                        bool midResponse = false ) = 0;

    /** Report data download
     *  @param[in] dltrackid The download track id
     *  @param[in] httpAttempt  The record of the HTTP attempts for a single
     *                          resource
     */
    virtual void downloadRecord(    uint32_t dltrackid,
                                    HttpAttempt const& httpAttempt ) = 0;

    /** Deprecated: Report aggregate downloaded bytes
     *  @param[in] bwentry Trace of aggregated downloaed bytes
     */
    virtual void aggregateTrace( BwEntry const& bwentry ) = 0;

    /** Report trace routes.
     *
     *  @param dst Server source IP address
     *  @param packetTraces packet traces.
     */
   virtual void reportRoute(
            std::string const& dst,
            const std::vector<PacketTrace>& routePacketTraces ) = 0;

    /** Report a successful DNS resolution.
     *
     *  @param nameServers [In] The IPs of the name serves used for resolvig host names.
     *  @param lookupTimeMs [In] The duration that the lookup takes, note this is not RTT to name servers.
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
                    int lookupTimeMs,
                    const std::vector<std::string>& cnames,
                    const std::vector<IpAddressTtlRecord>& ipAddressTtlRecords ) = 0;

    /** Report an unsuccessful DNS resolution.
     *
     *  @param nameServers [In] The IPs of the name serves used for resolvig host names.
     *  @param lookupTimeMs [In] The duration that the lookup takes, note this is not RTT to name servers.
     *  @param hostName [In] The host name.
     *  @param failureCode [In] The failure code, as ASE error codes.
     */
    virtual void reportDnsLookupFailure(
                    DnsQueryType dnsQueryType,
                    const std::vector<std::string>& nameServers,
                    int lookupTimeMs,
                    const std::string& hostName,
                    int32_t failureCode ) = 0;

    /** Report the network monitor state
     *  @param netmon [in] The network monitor state
     */
    virtual void reportNetworkMonitorState( Variant const& netmon ) = 0;

    /**
     * Reports the total number of bytes transferred in the current network.
     *
     * (The current network is retrieved from the SystemData at the start of each
     * play session)
     *
     * @param[in] bytes The total number of bytes used by the network
     */
    virtual void setNetworkBytes(uint32_t bytes) = 0;

    /**
     * Adds additional bytes transferred for the current network to the total
     *
     * (The current network is retrieved from the SystemData at the start of each
     * play session)
     *
     * @param[in] bytes The additional number of bytes used by the network
     */
    virtual void addNetworkBytes(uint32_t bytes) = 0;
};

typedef ITransportReporter::ITransportReporterPtr ITransportReporterPtr;

}}
#endif
