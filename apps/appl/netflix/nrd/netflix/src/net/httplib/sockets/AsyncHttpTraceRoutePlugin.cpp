/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <assert.h>

#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <fcntl.h>

#define __FAVOR_BSD
#include <netinet/udp.h>
#undef __FAVOR_BSD

#include <nrdnet/IAsyncHttpLib.h>
#include "AsyncHttpTraceRoutePlugin.h"

#define DEFAULT_ROUTE_MAX_HOP_NUMBER (20)   //Default route max hop number.
#define DEFAULT_ROUTE_MAX_PROBE_NUMBER (2)  //Default route max probe number.
#define DEFAULT_ROUTE_UDP_PORT_BASE (33434) //Default route UDP port number base.
#define DEFAULT_ROUTE_PROBE_TIMEOUT (5000)  //Default route probe timeout.


namespace netflix {

/* @class
 * @brief
 */
/** Constructor. */
AsyncHttpTraceRoutePlugin::AsyncHttpTraceRoutePlugin()
{
}

/** Destructor. */
AsyncHttpTraceRoutePlugin::~AsyncHttpTraceRoutePlugin()
{
    deinit();
}

/** Initialize the plugin */
int32_t AsyncHttpTraceRoutePlugin::init()
{
    int32_t retVal = AS_SOCKET_ERROR;

    while (true)
    {
        //Create the sending socket.
        mIpV4RouteSendSocket = socket(
                                AF_INET,
                                SOCK_DGRAM,
                                0 );

        if (mIpV4RouteSendSocket == -1)
        {
            retVal = AS_SOCKET_ERROR;

            break;
        }

        //Set the sending socket to nonblocking.
        int flags;

        flags = fcntl(
                    mIpV4RouteSendSocket,
                    F_GETFL,
                    0 ) ;

        if (flags == -1)
        {
            close( mIpV4RouteSendSocket );
            mIpV4RouteSendSocket = -1;

            break;
        }

        retVal = fcntl(
                    mIpV4RouteSendSocket,
                    F_SETFL,
                    flags | O_NONBLOCK );

        if (retVal == -1)
        {
            close( mIpV4RouteSendSocket );
            mIpV4RouteSendSocket = -1;

            break;
        }

        //Create the receiving socket.
        mIpV4RouteRecvSocket = socket(
                                AF_INET,
                                SOCK_RAW,
                                IPPROTO_ICMP );

        if (mIpV4RouteRecvSocket == -1)
        {
            close( mIpV4RouteSendSocket );
            mIpV4RouteSendSocket = -1;

            break;
        }

        flags = fcntl(
                    mIpV4RouteRecvSocket,
                    F_GETFL,
                    0 ) ;

        if (flags == -1)
        {
            close( mIpV4RouteSendSocket );
            mIpV4RouteSendSocket = -1;

            close( mIpV4RouteRecvSocket );
            mIpV4RouteRecvSocket = -1;

            break;
        }

        retVal = fcntl(
                    mIpV4RouteRecvSocket,
                    F_SETFL,
                    flags | O_NONBLOCK );

        if (retVal == -1)
        {
            close( mIpV4RouteSendSocket );
            mIpV4RouteSendSocket = -1;

            close( mIpV4RouteRecvSocket );
            mIpV4RouteRecvSocket = -1;

            break;
        }

        mIpV4RouteSourcePort = 0;

        retVal = AS_NO_ERROR;

        break;
    }

    return retVal;
}

/** Deinitialize the plugin */
int32_t AsyncHttpTraceRoutePlugin::deinit()
{
    int32_t retVal = AS_NO_ERROR;

    if (mIpV4RouteSendSocket != -1)
    {
        close( mIpV4RouteSendSocket );
        mIpV4RouteSendSocket = -1;
    }

    if (mIpV4RouteRecvSocket != -1)
    {
        close( mIpV4RouteRecvSocket );
        mIpV4RouteRecvSocket = -1;
    }

    mIpV4TraceTaskList.clear();

    return retVal;
}

/**
 *  Set the route listener.
 */
void AsyncHttpTraceRoutePlugin::setRouteListener( IRouteListener* pRouteListener )
{
    mRouteListenerPtr = pRouteListener;
}

/** Prepare the data for calling select(). */
void AsyncHttpTraceRoutePlugin::prepareSelect(
                                        const AseTimeVal& timeNow,
                                        int& maxSelectingFd,
                                        fd_set* pReadFdSet,
                                        fd_set* /* pWriteFdSet */,
                                        AseTimeVal& timeout )
{
    maxSelectingFd = -1;
    timeout = AseTimeVal::INFINITE;

    if (mIpV4RouteRecvSocket != -1)
    {
        maxSelectingFd = mIpV4RouteRecvSocket;

        FD_SET( mIpV4RouteRecvSocket, pReadFdSet );
    }

    if (!mIpV4TraceTaskList.empty())
    {
        AseTimeVal traceTimeout;

        traceTimeout = mIpV4TraceTaskList.front().mCurrentProbeSentTime
                        + AseTimeVal::fromMS( DEFAULT_ROUTE_PROBE_TIMEOUT );

        if (traceTimeout > timeNow)
        {
            timeout = traceTimeout - timeNow;
        }
        else
        {
            timeout  = AseTimeVal::ZERO;
        }
    }
}

/** Process the select events. */
void AsyncHttpTraceRoutePlugin::processSelect(
                                        const AseTimeVal& timeNow,
                                        fd_set* pReadFdSet,
                                        fd_set* /* pWriteFdSet */ )
{
    if ((mIpV4RouteRecvSocket != -1) && (FD_ISSET( mIpV4RouteRecvSocket, pReadFdSet )))
    {
        processRouteResponse( timeNow );
    }

    processRouteTimeout( timeNow );
}

/** Trace route a destination IP address. */
void AsyncHttpTraceRoutePlugin::traceRoute( const NrdIpAddr& destinationIpAddress )
{
    if ((mIpV4RouteSendSocket != -1)
        && (mIpV4RouteRecvSocket != -1)
        && (mIpV4RouteTracedList.find( destinationIpAddress ) == mIpV4RouteTracedList.end()))
    {
        TraceTask traceTask;

        traceTask.mDestinationIpAddress = destinationIpAddress;
        traceTask.mCurrentHopNumber = 1;
        traceTask.mCurrentProbeNumber = 1;

        if (mIpV4TraceTaskList.empty())
        {
            mIpV4TraceTaskList.push_back( traceTask );

            sendRouteProbe( AseTimeVal::now() );
        }
        else
        {
            mIpV4TraceTaskList.push_back( traceTask );
        }

        mIpV4RouteTracedList.insert( destinationIpAddress );
    }

    return;
}

/** Process route response. */
void AsyncHttpTraceRoutePlugin::processRouteResponse( const AseTimeVal& timeNow )
{
    int32_t retVal = 0;

    struct sockaddr_in remoteSockAddrIn;
    socklen_t remoteSockAddrLen;

    while (true)
    {
        memset( &remoteSockAddrIn, 0, sizeof( remoteSockAddrIn ) );

        remoteSockAddrLen = sizeof( remoteSockAddrIn );

        retVal = recvfrom(
                    mIpV4RouteRecvSocket,
                    mIpV4RouteRecvBuffer,
                    ROUTE_BUFFER_SIZE,
                    0,
                    (struct sockaddr*) &remoteSockAddrIn,
                    &remoteSockAddrLen );

        if (retVal < 0)
        {
            break;
        }

        struct ip* ip;
        int hlen1, hlen2, icmplen;
        struct icmp *icmp;
        struct ip *hip;
        struct udphdr *udp;

        ip = (struct ip *) mIpV4RouteRecvBuffer;

        hlen1 = ip->ip_hl << 2; /* length of IP header */
        icmp = (struct icmp *) (mIpV4RouteRecvBuffer + hlen1);
        icmplen = retVal - hlen1;

        if (icmplen < 8 + 20 + 8)
        {
            break;
        }

        if ((icmp->icmp_type != ICMP_TIMXCEED)
            && (icmp->icmp_type != ICMP_UNREACH))
        {
            break;
        }

        /* hip is the header of the enclosed IP packets, supposedly
           the header of the packet that caused the error */

        hip = (struct ip *) (mIpV4RouteRecvBuffer + hlen1 + 8);

        if (hip->ip_p != IPPROTO_UDP)
        {
            break;
        }

        hlen2 = hip->ip_hl << 2;

        udp = (struct udphdr *) (mIpV4RouteRecvBuffer + hlen1 + 8 + hlen2);

        if (udp->uh_sport != mIpV4RouteSourcePort)
        {
            break;
        }

        if (mIpV4TraceTaskList.empty()
            || (udp->uh_dport != htons(
                                    DEFAULT_ROUTE_UDP_PORT_BASE
                                    + (mIpV4TraceTaskList.front().mCurrentHopNumber - 1)
                                        * DEFAULT_ROUTE_MAX_PROBE_NUMBER
                                    + (mIpV4TraceTaskList.front().mCurrentProbeNumber - 1) )))
        {
            break;
        }

        /* now we know it's an ICMP packet caused by a UDP
           datagram sent by us and sent to the port we happen to
           be sending to.  It's probably one of ours. */

        if (((icmp->icmp_type == ICMP_TIMXCEED) && (icmp->icmp_code == ICMP_TIMXCEED_INTRANS))
            || ((icmp->icmp_type == ICMP_UNREACH) &&(icmp->icmp_code == ICMP_UNREACH_PORT)))
        {
            PacketTrace packetTrace;

            packetTrace.mHopNumber = mIpV4TraceTaskList.front().mCurrentHopNumber;
            packetTrace.mLocalIp = NrdIpAddr( hip->ip_src ).getStrAddr();
            packetTrace.mNodeIp = NrdIpAddr( remoteSockAddrIn.sin_addr ).getStrAddr();
            packetTrace.mRtt = (uint32_t) (timeNow - mIpV4TraceTaskList.front().mCurrentProbeSentTime).ms();

            mIpV4TraceTaskList.front().mRoutePacketTraces.push_back( packetTrace );
        }

        if ((icmp->icmp_type == ICMP_TIMXCEED) && (icmp->icmp_code == ICMP_TIMXCEED_INTRANS))
        {
            if (mIpV4TraceTaskList.front().mCurrentProbeNumber == DEFAULT_ROUTE_MAX_PROBE_NUMBER)
            {
                mIpV4TraceTaskList.front().mCurrentProbeNumber = 1;
                mIpV4TraceTaskList.front().mCurrentHopNumber++;
            }
            else
            {
                mIpV4TraceTaskList.front().mCurrentProbeNumber++;
            }

            if (mIpV4TraceTaskList.front().mCurrentHopNumber > DEFAULT_ROUTE_MAX_HOP_NUMBER)
            {
                mRouteListenerPtr->reportRoute(
                            mIpV4TraceTaskList.front().mDestinationIpAddress.getStrAddr(),
                            mIpV4TraceTaskList.front().mRoutePacketTraces );

                mIpV4TraceTaskList.pop_front();
            }

            if (!mIpV4TraceTaskList.empty())
            {
                sendRouteProbe( timeNow );
            }
        }

        if ((icmp->icmp_type == ICMP_UNREACH) &&(icmp->icmp_code == ICMP_UNREACH_PORT))
        {
            mRouteListenerPtr->reportRoute(
                        mIpV4TraceTaskList.front().mDestinationIpAddress.getStrAddr(),
                        mIpV4TraceTaskList.front().mRoutePacketTraces );

            mIpV4TraceTaskList.pop_front();

            if (!mIpV4TraceTaskList.empty())
            {
                sendRouteProbe( timeNow );
            }
        }

        break;
    }

    if (retVal < 0)
    {
        deinit();
    }

    return;
}

/** Process route timeout. */
void AsyncHttpTraceRoutePlugin::processRouteTimeout( const AseTimeVal& timeNow )
{
    if (!mIpV4TraceTaskList.empty()
        && (timeNow >= (mIpV4TraceTaskList.front().mCurrentProbeSentTime
                         + AseTimeVal::fromMS( DEFAULT_ROUTE_PROBE_TIMEOUT ))))
    {
        if (mIpV4TraceTaskList.front().mCurrentProbeNumber == DEFAULT_ROUTE_MAX_PROBE_NUMBER)
        {
            mIpV4TraceTaskList.front().mCurrentProbeNumber = 1;
            mIpV4TraceTaskList.front().mCurrentHopNumber++;
        }
        else
        {
            mIpV4TraceTaskList.front().mCurrentProbeNumber++;
        }

        if (mIpV4TraceTaskList.front().mCurrentHopNumber > DEFAULT_ROUTE_MAX_HOP_NUMBER)
        {
            if (!mIpV4TraceTaskList.front().mRoutePacketTraces.empty())
            {
                mRouteListenerPtr->reportRoute(
                            mIpV4TraceTaskList.front().mDestinationIpAddress.getStrAddr(),
                            mIpV4TraceTaskList.front().mRoutePacketTraces );
            }

            mIpV4TraceTaskList.pop_front();
        }

        if (!mIpV4TraceTaskList.empty())
        {
            sendRouteProbe( timeNow );
        }
    }

    return;
}

/** Send a route probe. */
void AsyncHttpTraceRoutePlugin::sendRouteProbe( const AseTimeVal& timeNow )
{
    int32_t retVal = 0;

    while (true)
    {
        int ttl = mIpV4TraceTaskList.front().mCurrentHopNumber;

        retVal = setsockopt(
                        mIpV4RouteSendSocket,
                        IPPROTO_IP,
                        IP_TTL,
                        &ttl,
                        sizeof( int ) );

        if (retVal < 0)
        {
            break;
        }

        struct sockaddr_in destinationSockAddrIn;

        destinationSockAddrIn.sin_family = AF_INET;
        destinationSockAddrIn.sin_addr = mIpV4TraceTaskList.front().mDestinationIpAddress.getInAddr();
        destinationSockAddrIn.sin_port = htons(
                            DEFAULT_ROUTE_UDP_PORT_BASE
                            + (mIpV4TraceTaskList.front().mCurrentHopNumber - 1)
                                * DEFAULT_ROUTE_MAX_PROBE_NUMBER
                            + (mIpV4TraceTaskList.front().mCurrentProbeNumber - 1) );

        int count = 0;

        while (count < 6)
        {
            retVal = sendto(
                        mIpV4RouteSendSocket,
                        mIpV4RouteSendBuffer,
                        ROUTE_BUFFER_SIZE,
                        0,
                        (struct sockaddr*) &destinationSockAddrIn,
                        sizeof( destinationSockAddrIn ) );

            if (retVal > 0)
            {
                retVal = 0;

                break;
            }
            else
            {
                retVal = -1;
            }

            count++;
        }

        if (retVal == 0)
        {
            mIpV4TraceTaskList.front().mCurrentProbeSentTime = timeNow;

            if ((!mIpV4RouteSourceIpAddress.valid()) && (mIpV4RouteSourcePort == 0))
            {
                struct sockaddr_in sourceSockAddrIn;
                socklen_t sourceSockAddrLen;

                memset( &sourceSockAddrIn, 0 , sizeof( sourceSockAddrIn ) );

                sourceSockAddrLen = sizeof( sourceSockAddrIn );

                retVal = getsockname(
                            mIpV4RouteSendSocket,
                            (struct sockaddr*) &sourceSockAddrIn,
                            &sourceSockAddrLen );

                if (retVal == 0)
                {
                    mIpV4RouteSourceIpAddress.setInAddr( sourceSockAddrIn.sin_addr );
                    mIpV4RouteSourcePort = sourceSockAddrIn.sin_port;
                }
            }
        }

        break;
    }

    if (retVal < 0)
    {
        deinit();
    }

    return;
}

}
