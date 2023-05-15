/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ASYNC_HTTP_TRACE_ROUTE_PLUGIN_H
#define ASYNC_HTTP_TRACE_ROUTE_PLUGIN_H

#include <list>
#include <vector>
#include <set>

#include <nrdbase/tr1.h>
#include <nrdnet/NrdIpAddr.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdnet/AseTimeVal.h>

#include "AsyncHttpBasePlugin.h"

#define ROUTE_BUFFER_SIZE (1400)

namespace netflix {

class IRouteListener;

/* @class
 * @brief
 */
class AsyncHttpTraceRoutePlugin : public AsyncHttpBasePlugin
{
public:
    typedef shared_ptr<AsyncHttpTraceRoutePlugin> AsyncHttpTraceRoutePluginPtr;

public:
    /** Constructor. */
    AsyncHttpTraceRoutePlugin();

    /** Destructor. */
    virtual ~AsyncHttpTraceRoutePlugin();

    /** Initialize the plugin */
    virtual int32_t init();

    /** Deinitialize the plugin */
    virtual int32_t deinit();

    /**
     *  Set the route listener.
     */
    virtual void setRouteListener( IRouteListener* pRouteListener );

    /** Prepare the data for calling select(). This function is called before select()
     *  to fill up the FD sets and when the plugin is interested in a timeout signal.
     *
     *  @param timeNow [In] The current time.
     *  @param maxSelectingFd [Out] The max selecting FD to select.
     *  @param pReadFdSet [Out] The read FD set.
     *  @param pWriteFdSet [Out] The write FD set.
     *  @param timeout [Out] The time the connection is interested in a timeout signal.
     */
    virtual void prepareSelect(
                        const AseTimeVal& timeNow,
                        int& maxSelectingFd,
                        fd_set* pReadFdSet,
                        fd_set* pWriteFdSet,
                        AseTimeVal& timeout );

    /** Process the select events. This function is called to process events returned
     *  from select().
     *
     *  @param timeNow [In] The current time.
     *  @param pReadFdSet [In] The read FD set.
     *  @param pWriteFdSet [In] The write FD set.
     */
    virtual void processSelect(
                        const AseTimeVal& timeNow,
                        fd_set* pReadFdSet,
                        fd_set* pWriteFdSet );

    /** Trace route a destination IP address. */
    virtual void traceRoute( const NrdIpAddr& destinationIpAddress );

protected:
    /** The trace task. */
    class TraceTask
    {
    public:
        NrdIpAddr mDestinationIpAddress;

        uint32_t mCurrentHopNumber;

        uint32_t mCurrentProbeNumber;
        AseTimeVal mCurrentProbeSentTime;

        std::vector<PacketTrace> mRoutePacketTraces;
    };

    /** The type definition for the trace task list. */
    typedef std::list<TraceTask> TraceTaskList;

protected:
    /** Process route response. */
    virtual void processRouteResponse( const AseTimeVal& timeNow );

    /** Process route timeout. */
    virtual void processRouteTimeout( const AseTimeVal& timeNow );

    /** Send a route probe. */
    virtual void sendRouteProbe( const AseTimeVal& timeNow );

protected:

    /** Trace route variables. */
    IRouteListener* mRouteListenerPtr;

    int mIpV4RouteSendSocket;
    int mIpV4RouteRecvSocket;

    NrdIpAddr mIpV4RouteSourceIpAddress;
    uint16_t mIpV4RouteSourcePort;

    TraceTaskList mIpV4TraceTaskList;

    std::set<NrdIpAddr> mIpV4RouteTracedList;

    uint8_t mIpV4RouteSendBuffer[ROUTE_BUFFER_SIZE];
    uint8_t mIpV4RouteRecvBuffer[ROUTE_BUFFER_SIZE];

#ifdef NRDP_HAS_IPV6
    int mIpV6RouteSendSocket;
    int mIpV6RouteRecvSocket;

    NrdIpAddr mIpV6RouteSourceIpAddress;
    uint16_t mIpV6RouteSourcePort;

    TraceTaskList mIpV6TraceTaskList;

    std::set<NrdIpAddr> mIpV6RouteTracedList;

    uint8_t mIpV6RouteSendBuffer[ROUTE_BUFFER_SIZE];
    uint8_t mIpV6RouteRecvBuffer[ROUTE_BUFFER_SIZE];
#endif

};

typedef AsyncHttpTraceRoutePlugin::AsyncHttpTraceRoutePluginPtr AsyncHttpTraceRoutePluginPtr;

}
#endif //ASYNC_HTTP_TRACE_ROUTE_PLUGIN_H
