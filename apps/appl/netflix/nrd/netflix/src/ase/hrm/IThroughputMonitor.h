/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef I_THROUGHPUT_MONITOR_H
#define I_THROUGHPUT_MONITOR_H

#include <nrdnet/AseTimeVal.h>
#include <nrdase/AseCommonDataTypes.h>

namespace netflix {
namespace ase {

class IThroughputMonitor
{
public:
    typedef shared_ptr<IThroughputMonitor> IThroughputMonitorPtr;

    virtual ~IThroughputMonitor() {}

    /**
     * @param[in] mediaType the media type of the connection
     * @param[in] bytes the number of data bytes received
     * @param[in] drainingSocketReceiveBuffer true if these bytes were deliberately left in
     *                                        the socket buffer for some reason and so may
     *                                        actually have arrived some time ago, but not before
     *                                        requestTime
     * @param[in] earliestTime the earliest time at which these bytes could have been received
     *                         when drainingSocketReceiveBuffer is true
     */
    virtual void indicateBytesReceived( MediaType mediaType,
                                        uint32_t bytes,
                                        bool drainingSocketReceiveBuffer,
                                        AseTimeVal earliestTime ) = 0;

    /**
     * indicate that no bytes are being received either because
     * there are no outstanding requests or because reading from
     * the provided domain has been stopped for rate control reasons
     */
    virtual void indicateReceptionStopped( MediaType mediaType ) = 0;

    /**
     * Add tcp connect info, such as rtt, and ip
     * @param[in] connectTime: tcp connect time
     * @param[in] ipAddr: ip address of the domain
     */
    virtual void addConnectInfo( AseTimeVal const& connectTime,
                                 std::string const& ipAddr ) = 0;


    virtual void addConnectionFailure() = 0;
};

typedef IThroughputMonitor::IThroughputMonitorPtr IThroughputMonitorPtr;

}}


#endif
